/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 /*
 * File: treereduce.cpp
 *
 * Purpose: Implement floating-point 'reduce' on the BLADE tree SPI
 *
 * Author: Chris Ward (tjcw@us.ibm.com)
 *         (c) IBM, 2003
 *
 * History : 04/25/2003: TJCW - Created
 *
 * To do :
 *  NIckel-and-dime optimisations of the block loops
 *  Figure a way to exploit the second core.
 *  Implement 'short' blocks more efficiently.
 */


#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }

#include <sys/blade_types.h>
#include <pk/treereduce.hpp>
#include <spi/BGL_TreePktSPI.h>

extern "C" {
  void s0printf(const char * , ...) ;
}
/*
 * Storage mapping of an IEEE double-precision number, for access to parts of it as integers or bits
 */
class doublemap_without_constructor
{
  public:
  class uintpair
  {
     public:
    unsigned int m_hi ;
    unsigned int m_lo ;
  } ;
   union {
      double m_d ;
      uintpair m_u ;
      } m_value ;
  double get_value(void) const { return m_value.m_d ; } ;

  void set_value(double X) { m_value.m_d = X ; } ;
  void set_value(
    unsigned int Xsign ,   // 0 for positive, 1 for negative
    unsigned int Xexponent ,
    unsigned int Xsignificand_hi ,  // The 0x00100000 bit had better be set, to get the right answer
    unsigned int Xsignificand_lo
    ) {
       m_value.m_u.m_hi = ( ( Xsign << 31 )                 & 0x80000000 )
                        | ( ( ( Xexponent + 1023 )  << 20 ) & 0x7ff00000 )
                        | ( Xsignificand_hi                 & 0x000fffff ) ;
       m_value.m_u.m_lo = Xsignificand_lo ;
   } ;

  unsigned int hiword(void) const { return m_value.m_u.m_hi ; } ;
  unsigned int loword(void) const { return m_value.m_u.m_lo ; } ;

  unsigned int sign_bit(void) const { return hiword() & 0x80000000 ; } ;
  unsigned int exponent_bits(void) const { return hiword() & 0x7ff00000 ; } ;
  unsigned int significand_hi_bits(void) const { return hiword() & 0x000fffff ; } ;
  unsigned int significand_lo_bits(void) const { return loword() ; } ;

  void set_significand_hi_bits(unsigned int new_hi_bits) { m_value.m_u.m_hi = ( m_value.m_u.m_hi & 0xfff00000 )
                                                                            | ( new_hi_bits & 0x000fffff ) ; }
  int exponent(void) const { return ( exponent_bits() >> 20 ) - 1023 ; } ;
  unsigned int significand_hi(void) const { return significand_hi_bits() | 0x00100000 ; } ;
  unsigned int significand_lo(void) const { return significand_lo_bits() ; } ;
  bool is_negative(void) const { return  0 != sign_bit() ; } ;
} ;

class doublemap: public doublemap_without_constructor
{
        public:
  doublemap(double X) { m_value.m_d = X ; } ;
  doublemap(
    unsigned int Xsign ,   // 0 for positive, 1 for negative
    unsigned int Xexponent ,
    unsigned int Xsignificand_hi ,  // The 0x00100000 bit had better be set, to get the right answer
    unsigned int Xsignificand_lo
    ) {
       m_value.m_u.m_hi = ( ( Xsign << 31 )                & 0x80000000 )
                        | ( ( (Xexponent + 1023 )  << 20 ) & 0x7ff00000 )
                        | ( Xsignificand_hi                & 0x000fffff ) ;
       m_value.m_u.m_lo = Xsignificand_lo ;
   } ;
}       ;

#include <cassert>
using namespace std ;

enum {
   k_diagnose_fp_reduce = 0
   }  ;

extern "C" {
   void dump_block(const char * text, const void * block_ptr) ;
    void s0printf(const char *, ...) ;
   }
// What we send through the tree ALU for add-reducing mantissae; really, a 64-bit integer
class TMantissaBase
{
public :
   int Hi ;
   unsigned int Lo ;
} ;

// We have a number of implementations of what to send round the tree for 'mantissa'.
// For 'well-conditioned' addends, there is not much difference; but for 'ill-conditioned'
// addends, it is possible to preserve more precision at the cost of significant extra
// processing time.
// 'well-conditioned' addends are those where the sum is not dominated by a small number
// of addends.
// An example of 'ill-conditioned' addends would be 1,0,0,0,0,0,0,0

// The most precise way performs the 'add' as 64-bit integer, scaling so that the sum
// will not overflow in 64 bits.

// The fastest way performs the 'add' in 52-bit integer, scaling so that the sum will
// not overflow in 52 bits. This way gains speed because the FPU can be used to do
// conversion to fixed point, renormalisation, handling of infinities, zeros, denormals,
// and not-a-numbers.

// There should be an in-between way, adding in 64-bit integer, and using integer
// arithmetic (64-bit shifts) for renormalisation. This not yet implemented.


// 2 is fastest and least precise
class TMantissa2
{
public:
   double Representation ;
   inline void Init(double Source, double AlignFactor) ;
   inline void GenerateFixMap(doublemap_without_constructor * fixMap, int maxExponent) ;
   inline double GenerateTarget( double fixValue, double AlignFactor) ;
} ;

// Select the 'fast' version. (Change this to inherit from TMantissa0 if you want the 'precise' one)
class TMantissa: public TMantissa2
{
} ;

// What we send through the tree ALU for finding the max exponent to prenormalise with; 16 bits is enough
class TExponent
{
public :
  unsigned short Exp ;
} ;

enum {
   k_TreePacketSize = _BGL_TREE_PKT_MAX_BYTES ,
   k_TreeMantissaBlockSize = k_TreePacketSize/sizeof(TMantissa) ,
   k_TreeExponentBlockSize = k_TreePacketSize/sizeof(TExponent) ,
   k_TreeMantissaBlockCount = k_TreeExponentBlockSize/k_TreeMantissaBlockSize ,
   k_PartitionSizeLog2 = 9 , // Will be used to scale to prevent overflows
   k_SpareBits = k_PartitionSizeLog2 - 7 , // Will be used to scale to prevent overflows

   k_BufferMultiple = 2 , // Number of (exponent) buffers to avoid waits for the tree
   } ;

static const double PartitionNodes = 512.0 ; // Number of nodes in partition as floating-point value,
                                             // must be power of 2, (could be rounded up if necessary)
static const double PartitionNodes32 = 768.0 ; // 1.5 * PartitionNodes

class Aligner
{
        public:
        doublemap_without_constructor AlignValue ;
} ;

// A tree packet made out of scaled mantissae
class TMantissaBlock
{
public :
  TMantissa Items[k_TreeMantissaBlockSize] ;
  inline void GenerateTarget(double * Target,
                       const TExponent * maxExponents,
                       const Aligner * AlignFactors) ;
  inline double GenerateTarget1( const TExponent * maxExponents,
                          const Aligner * AlignFactors) ;
} ;

class TMantissaAlignerBlock
{
   public:
          Aligner AlignFactor[k_TreeMantissaBlockSize] ;
} ;

class TMantissaBlockForAligner
{
        public:
          TMantissa Items[k_TreeMantissaBlockSize] ;

  inline void Init(TMantissaAlignerBlock& MantissaAlignerBlock, const double * Source, const TExponent * maxExponents) ;
  inline void Init1(Aligner& MantissaAligner, double Source, const TExponent * maxExponents) ;

} ;

// A tree packet made out of exponents
class TExponentBlock
{
public:
   TExponent Items[k_TreeExponentBlockSize] ;

   inline void Init(const double * Source) ;
   inline void Init1(double Source) ;
} ;

// We send several mantissa packets for each exponent packet
class TMantissaGroup
{
public:
  TMantissaBlock Group[k_TreeMantissaBlockCount] ;

  inline void GenerateTarget(double * Target, const TExponent * maxExponents) ;
} ;

class TMantissaGroupWithAligner
{
public:
  TMantissaBlockForAligner Group[k_TreeMantissaBlockCount] ;
  TMantissaAlignerBlock AlignerGroup[k_TreeMantissaBlockCount] ;

  inline void Init(const double * mapSource, const TExponentBlock & maxExponents) ;
  inline void Init1(double Source, const TExponentBlock& MaxExponentBlock) ;
} ;


// Internals of the 'reduce'
// extern int FullPartitionClass ;
// extern int FullPartitionRoot ;

class TReduce
{
public:
   TExponentBlock LocalExponentBlock[k_BufferMultiple] ;
   TExponentBlock MaxExponentBlock[k_BufferMultiple] ;
   TMantissaGroupWithAligner LocalMantissaGroup[k_BufferMultiple] ;
   TMantissaGroup ReducedMantissaGroup[k_BufferMultiple] ;

   _BGL_TreeHwHdr maxReduceHdr ;
   _BGL_TreeHwHdr addReduceHdr ;
   int ReductionChannel ;

   void Init(int aReductionChannel, int aReductionClass) ;
   void AllReduce( double * Target, const double * Source, unsigned int Length ) ;
   double AllReduce1( double Source ) ;

   inline void GenerateMaxExponents(unsigned int BufferNumber) ;
   inline void GenerateReducedMantissae(unsigned int BufferNumber) ;
   inline void GenerateTarget(unsigned int BufferNumber, double * Target) ;
   inline double GenerateTarget1(unsigned int BufferNumber) ;



   inline void SendLocalExponents(unsigned int BufferNumber) ;
   inline void ReceiveMaxExponents(unsigned int BufferNumber) ;
   inline void SendLocalMantissae(unsigned int BufferNumber) ;
   inline void SendLocalMantissae1(unsigned int BufferNumber) ;
   inline void ReceiveReducedMantissae(unsigned int BufferNumber) ;
   inline void ReceiveReducedMantissae1(unsigned int BufferNumber) ;


} __attribute__((aligned(32)));


// Initialise the 'reducer'; i.e. construct the appropriate tree headers for the required ops
void TReduce::Init(int aReductionChannel, int aReductionClass)
{
   ReductionChannel = aReductionChannel ;
   BGLTreeMakeAluHdr( &maxReduceHdr,
                      aReductionClass,             // class
                      _BGL_TREE_OPCODE_MAX,           // opcode
                      _BGL_TREE_OPSIZE_BIT16          // size
                    ) ;
   BGLTreeMakeAluHdr( &addReduceHdr,
                      aReductionClass,             // class
                      _BGL_TREE_OPCODE_ADD,           // opcode
                      _BGL_TREE_OPSIZE_BIT64          // size
                    ) ;
} ;

void TExponentBlock::Init(const double * Source)
{
   const doublemap * mapSource = ( const doublemap * ) Source ;
   for (int x=0; x<k_TreeExponentBlockSize; x+=1)
   {
      // We need to pick off the exponent bits for normalisation. However, '0' in
      // the exponent bits is a special case meaning the number is zero or denormal.
      // Here is a good place to spot this and arrange in advance that the reduction
      // will go ahead normally even if all the numbers being reduced are zero or
      // denormal. We can't unconditionally add 1, because that would stop NaNs and
      // infinities from being processed sensibly.
      unsigned int expBits = mapSource[x].exponent_bits() >> 20 ; // Pick off the biassed IEEE exponent
      unsigned int bexpBits = expBits + ( 0 == expBits ) ; // Increment if zero ...
      Items[x].Exp = bexpBits ;
   } /* endfor */
}

void TExponentBlock::Init1(double Source)
{
   const doublemap mapSource(Source) ;
   // We need to pick off the exponent bits for normalisation. However, '0' in
   // the exponent bits is a special case meaning the number is zero or denormal.
   // Here is a good place to spot this and arrange in advance that the reduction
   // will go ahead normally even if all the numbers being reduced are zero or
   // denormal. We can't unconditionally add 1, because that would stop NaNs and
   // infinities from being processed sensibly.
   unsigned int expBits = mapSource.exponent_bits() >> 20 ; // Pick off the biassed IEEE exponent
   unsigned int bexpBits = expBits + ( 0 == expBits ) ; // Increment if zero ...
   Items[0].Exp = bexpBits ;
}

void TReduce::SendLocalExponents(unsigned int BufferNumber)
{

   if (k_diagnose_fp_reduce)
   {
     dump_block("Sending exponent", LocalExponentBlock+BufferNumber) ;
   } /* endif */
   int maxSendResult = BGLTreeSend(
                       ReductionChannel, // virtual channel
                       &maxReduceHdr,                  // Header we made before
                       LocalExponentBlock+BufferNumber,            // Operands being sent
                       k_TreePacketSize                // one block, by construction
                       ) ;
   if (k_diagnose_fp_reduce)
   {
     s0printf("Send result %d\n",maxSendResult) ;
   } /* endif */
}

void TReduce::ReceiveMaxExponents(unsigned int BufferNumber)
{
   _BGL_TreeHwHdr maxReduceHdrReturned ;
   int maxRecvResult = BGLTreeReceive(
                                          ReductionChannel, // Class for the reduction
                      &maxReduceHdrReturned,          // Header after circulating the tree
                      MaxExponentBlock+BufferNumber,              // Operands being received
                      k_TreePacketSize                // one block, by construction
                      ) ;
   if (k_diagnose_fp_reduce)
   {
     s0printf("Receive result %d\n",maxRecvResult) ;
     dump_block("Received exponent", MaxExponentBlock+BufferNumber) ;
   } /* endif */
}

void TReduce::GenerateMaxExponents(unsigned int BufferNumber)
{

   SendLocalExponents(BufferNumber) ;
   ReceiveMaxExponents(BufferNumber) ;

}


void TMantissaGroupWithAligner::Init(const double * Source, const TExponentBlock& MaxExponentBlock)
{
   for (int x=0;x<k_TreeMantissaBlockCount ; x+=1)
   {
      Group[x].Init(AlignerGroup[x],Source+x*k_TreeMantissaBlockSize,MaxExponentBlock.Items+x*k_TreeMantissaBlockSize ) ;
   } /* endfor */
}

void TMantissaGroupWithAligner::Init1(double Source, const TExponentBlock& MaxExponentBlock)
{
    Group[0].Init1(AlignerGroup[0].AlignFactor[0],Source,MaxExponentBlock.Items ) ;
}

void TMantissaBlockForAligner::Init(TMantissaAlignerBlock& MantissaAlignerBlock, const double * Source, const TExponent * maxExponents)
{
  for (int x0=0; x0<k_TreeMantissaBlockSize; x0+=1)
  {
      MantissaAlignerBlock.AlignFactor[x0].AlignValue.set_value(0,maxExponents[x0].Exp-1023,0,0) ;
  } /* endfor */

   for (int x1=0; x1<k_TreeMantissaBlockSize; x1+=8)
   {
      double s0 = Source[x1+0] ;
      double s1 = Source[x1+1] ;
      double s2 = Source[x1+2] ;
      double s3 = Source[x1+3] ;
      double s4 = Source[x1+4] ;
      double s5 = Source[x1+5] ;
      double s6 = Source[x1+6] ;
      double s7 = Source[x1+7] ;
      Items[x1+0].Init(s0, MantissaAlignerBlock.AlignFactor[x1+0].AlignValue.get_value()) ;
      Items[x1+1].Init(s1, MantissaAlignerBlock.AlignFactor[x1+1].AlignValue.get_value()) ;
      Items[x1+2].Init(s2, MantissaAlignerBlock.AlignFactor[x1+2].AlignValue.get_value()) ;
      Items[x1+3].Init(s3, MantissaAlignerBlock.AlignFactor[x1+3].AlignValue.get_value()) ;
      Items[x1+4].Init(s4, MantissaAlignerBlock.AlignFactor[x1+4].AlignValue.get_value()) ;
      Items[x1+5].Init(s5, MantissaAlignerBlock.AlignFactor[x1+5].AlignValue.get_value()) ;
      Items[x1+6].Init(s6, MantissaAlignerBlock.AlignFactor[x1+6].AlignValue.get_value()) ;
      Items[x1+7].Init(s7, MantissaAlignerBlock.AlignFactor[x1+7].AlignValue.get_value()) ;
   } /* endfor */
}

void TMantissaBlockForAligner::Init1(Aligner& MantissaAligner,double Source, const TExponent * maxExponents)
{
   MantissaAligner.AlignValue.set_value(0,maxExponents[0].Exp-1023,0,0) ;

   Items[0].Init(Source, MantissaAligner.AlignValue.get_value()) ;
}

// Fastest/least precise implementation
void TMantissa2::Init(double Source, double AlignFactor)
{
  double Offset = AlignFactor*PartitionNodes32 ;
  Representation = Offset + Source ; // Allow the FPU to align things
   if (k_diagnose_fp_reduce)
   {
     doublemap m(Representation) ;
     s0printf("Representing %d/1000 on align factor %d/1000 Offset %d/1000 as 0x%08x%08x\n"
                  ,(int)(Source * 1000.0), (int)(AlignFactor * 1000.0 ), (int)(Offset * 1000.0), m.hiword(), m.loword())  ;
   } /* endif */
}
/*
 * Walkthrough for 1 + 0.5 on 2 node tree
 *  maxExponent = 0x3ff
 *  alignFactor = 1.0
 *  Offset = 3
 *  Representation = (4, 3.5) ... ?
 */

double TMantissa2::GenerateTarget( double FixValue, double AlignFactor)
{
//   doublemap repMap(Representation) ;
//   doublemap fixMap(0,maxExponent-1023,repMap.significand_hi_bits() ^ 0x00080000 ,repMap.significand_lo_bits()) ;
   double Offset = AlignFactor * PartitionNodes32 ;
   double Target = FixValue * PartitionNodes - Offset ;
   if (k_diagnose_fp_reduce)
   {
     doublemap m(Representation) ;
     s0printf("Rebuilding from 0x%08x%08x with FixValue %d/1000 offset %d/1000 as %d/1000\n"
                              ,m.hiword(), m.loword(), (int)(FixValue * 1000.0)
                              ,(int)(Offset*1000), (int)(Target * 1000.0) ) ;
   } /* endif */
   return Target ;
}

void TMantissa2::GenerateFixMap( doublemap_without_constructor * fixMap ,int maxExponent )
{
   doublemap repMap(Representation) ;
   fixMap -> set_value(
   0,maxExponent-1023,repMap.significand_hi_bits() ^ 0x00080000 ,repMap.significand_lo_bits()
   ) ;
//   double Offset = AlignFactor * PartitionNodes32 ;
//   *Target = fixMap.get_value() * PartitionNodes - Offset ;
//   if (k_diagnose_fp_reduce)
//   {
//     doublemap m(Representation) ;
//     s0printf("Rebuilding from 0x%08x%08x with max exp %d fixMap 0x%08x%08x offset %d/1000 as %d/1000\n"
//                              ,m.hiword(), m.loword(),      maxExponent, fixMap.hiword(), fixMap.loword()
//                              ,(int)(Offset*1000), (int)(*Target * 1000.0) ) ;
//   } /* endif */
}

void TReduce::SendLocalMantissae(unsigned int BufferNumber)
{
   // Issue the tree ops and assume they work. Hope there is enough buffering to accept (4) packets before starting to drain.

   if (k_diagnose_fp_reduce)
   {
     dump_block("Sending mantissa 0", LocalMantissaGroup[BufferNumber].Group) ;
     dump_block("Sending mantissa 1", LocalMantissaGroup[BufferNumber].Group+1) ;
     dump_block("Sending mantissa 2", LocalMantissaGroup[BufferNumber].Group+2) ;
     dump_block("Sending mantissa 3", LocalMantissaGroup[BufferNumber].Group+3) ;
   } /* endif */

   int maxSendResult = BGLTreeSend(
                      ReductionChannel, // Virtual Channel for the reduction
                      &addReduceHdr,                  // Header we made before
                      LocalMantissaGroup+BufferNumber,            // Operands being sent
                      k_TreePacketSize*k_TreeMantissaBlockCount  // appropriate number of blocks, by construction
                      ) ;
}

void TReduce::SendLocalMantissae1(unsigned int BufferNumber)
{
   // Issue the tree ops and assume they work. Hope there is enough buffering to accept (4) packets before starting to drain.

   if (k_diagnose_fp_reduce)
   {
     dump_block("Sending mantissa 0", LocalMantissaGroup[BufferNumber].Group) ;
   } /* endif */

   int maxSendResult = BGLTreeSend(
                      ReductionChannel, // Virtual Channel for the reduction
                      &addReduceHdr,                  // Header we made before
                      LocalMantissaGroup+BufferNumber,            // Operands being sent
                      k_TreePacketSize                           // appropriate number of blocks, by construction
                      ) ;
}

void TReduce::ReceiveReducedMantissae(unsigned int BufferNumber)
{
   _BGL_TreeHwHdr addReduceHdrReturned ;


   int maxRecvResult = BGLTreeReceive(
                      ReductionChannel, // Virtual Channel for the reduction
                      &addReduceHdrReturned,          // Header after circulating the tree
                      ReducedMantissaGroup+BufferNumber,        // Operands being received
                      k_TreePacketSize*k_TreeMantissaBlockCount  // appropriate number of blocks, by construction
                      ) ;
   if (k_diagnose_fp_reduce)
   {
     dump_block("Received mantissa 0", ReducedMantissaGroup[BufferNumber].Group) ;
     dump_block("Received mantissa 1", ReducedMantissaGroup[BufferNumber].Group+1) ;
     dump_block("Received mantissa 2", ReducedMantissaGroup[BufferNumber].Group+2) ;
     dump_block("Received mantissa 3", ReducedMantissaGroup[BufferNumber].Group+3) ;
   } /* endif */
}

void TReduce::ReceiveReducedMantissae1(unsigned int BufferNumber)
{
   _BGL_TreeHwHdr addReduceHdrReturned ;


   int maxRecvResult = BGLTreeReceive(
                      ReductionChannel, // Virtual Channel for the reduction
                      &addReduceHdrReturned,          // Header after circulating the tree
                      ReducedMantissaGroup+BufferNumber,        // Operands being received
                      k_TreePacketSize                           // appropriate number of blocks, by construction
                      ) ;
   if (k_diagnose_fp_reduce)
   {
     dump_block("Received mantissa 0", ReducedMantissaGroup[BufferNumber].Group) ;
   } /* endif */
}

void TReduce::GenerateReducedMantissae(unsigned int BufferNumber)
{
   SendLocalMantissae(BufferNumber) ;
   ReceiveReducedMantissae(BufferNumber) ;

}

void TReduce::GenerateTarget(unsigned int BufferNumber,double * mapTarget)
{

   for (int x=0;x<k_TreeMantissaBlockCount ; x+=1)
   {
      ReducedMantissaGroup[BufferNumber].Group[x].GenerateTarget(
       mapTarget+x*k_TreeMantissaBlockSize,
       MaxExponentBlock[BufferNumber].Items+x*k_TreeMantissaBlockSize,
       LocalMantissaGroup[BufferNumber].AlignerGroup[x].AlignFactor
      ) ;
   } /* endfor */
}

double TReduce::GenerateTarget1(unsigned int BufferNumber)
{

   double result = ReducedMantissaGroup[BufferNumber].Group[0].GenerateTarget1(
    MaxExponentBlock[BufferNumber].Items,
    LocalMantissaGroup[BufferNumber].AlignerGroup[0].AlignFactor
    ) ;
   return result ;
}

void TMantissaBlock::GenerateTarget(double * Target,
                                    const TExponent * maxExponents,
                                    const Aligner * AlignFactor )
{
   doublemap_without_constructor fixMaps[k_TreeMantissaBlockSize] ;
   for (int x0=0; x0<k_TreeMantissaBlockSize; x0+=1)
   {
     Items[x0].GenerateFixMap(fixMaps+x0, maxExponents[x0].Exp) ;
   } /* endfor */
   for (int x1=0; x1<k_TreeMantissaBlockSize; x1+=8)
   {
     double t0 = Items[x1+0].GenerateTarget(fixMaps[x1+0].get_value(), AlignFactor[x1+0].AlignValue.get_value()) ;
     double t1 = Items[x1+1].GenerateTarget(fixMaps[x1+1].get_value(), AlignFactor[x1+1].AlignValue.get_value()) ;
     double t2 = Items[x1+2].GenerateTarget(fixMaps[x1+2].get_value(), AlignFactor[x1+2].AlignValue.get_value()) ;
     double t3 = Items[x1+3].GenerateTarget(fixMaps[x1+3].get_value(), AlignFactor[x1+3].AlignValue.get_value()) ;
     double t4 = Items[x1+4].GenerateTarget(fixMaps[x1+4].get_value(), AlignFactor[x1+4].AlignValue.get_value()) ;
     double t5 = Items[x1+5].GenerateTarget(fixMaps[x1+5].get_value(), AlignFactor[x1+5].AlignValue.get_value()) ;
     double t6 = Items[x1+6].GenerateTarget(fixMaps[x1+6].get_value(), AlignFactor[x1+6].AlignValue.get_value()) ;
     double t7 = Items[x1+7].GenerateTarget(fixMaps[x1+7].get_value(), AlignFactor[x1+7].AlignValue.get_value()) ;
     Target[x1+0] = t0 ;
     Target[x1+1] = t1 ;
     Target[x1+2] = t2 ;
     Target[x1+3] = t3 ;
     Target[x1+4] = t4 ;
     Target[x1+5] = t5 ;
     Target[x1+6] = t6 ;
     Target[x1+7] = t7 ;
   } /* endfor */
}

double TMantissaBlock::GenerateTarget1( const TExponent * maxExponents,
                                    const Aligner * AlignFactor )
{
   doublemap_without_constructor fixMap ;
   Items[0].GenerateFixMap(&fixMap, maxExponents[0].Exp) ;
     double result = Items[0].GenerateTarget(fixMap.get_value(), AlignFactor[0].AlignValue.get_value()) ;
     return result ;
}

void TReduce::AllReduce(
   double * Target,
   const double * Source,
   unsigned int Length
)  {
   // Careful about 'int GroupCount', we are going to subtract 1 or 2, needs to be signed
   int GroupCount = Length / k_TreeExponentBlockSize ;
   unsigned int PartialStart = GroupCount*k_TreeExponentBlockSize ;
   unsigned int PartialSize = Length - PartialStart ;
   // 'long vector' case, interleaved scheduling for efficiency

   // Fill the pipeline
   LocalExponentBlock[0].Init(Source+0*k_TreeExponentBlockSize)  ;
   SendLocalExponents(0) ;

   int x ;
   // use the full pipeline
   for (x=0; x<GroupCount-2; x+=2)
   {
     LocalExponentBlock[1].Init(Source+(x+1)*k_TreeExponentBlockSize)  ;
     SendLocalExponents(1) ;
     ReceiveMaxExponents(0) ;
     LocalMantissaGroup[0].Init(Source+(x+0)*k_TreeExponentBlockSize,MaxExponentBlock[0]) ;
     ReceiveMaxExponents(1) ;
     SendLocalMantissae(0) ;
     LocalMantissaGroup[1].Init(Source+(x+1)*k_TreeExponentBlockSize,MaxExponentBlock[1]) ;
     ReceiveReducedMantissae(0) ;
     SendLocalMantissae(1) ;
     GenerateTarget(0,Target+(x+0)*k_TreeExponentBlockSize) ;
     LocalExponentBlock[0].Init(Source+(x+2)*k_TreeExponentBlockSize)  ;
     ReceiveReducedMantissae(1) ;
     SendLocalExponents(0) ;
     GenerateTarget(1,Target+(x+1)*k_TreeExponentBlockSize) ;
   } /* endfor */

   // Drain the pipeline
   ReceiveMaxExponents(0) ;
   LocalMantissaGroup[0].Init(Source+(x+0)*k_TreeExponentBlockSize,MaxExponentBlock[0]) ;
   SendLocalMantissae(0) ;
   ReceiveReducedMantissae(0) ;
   GenerateTarget(0,Target+(x+0)*k_TreeExponentBlockSize) ;

   // Push the last block through the pipeline if there is one
   if (x<GroupCount-1)
   {
     LocalExponentBlock[1].Init(Source+(x+1)*k_TreeExponentBlockSize)  ;
     SendLocalExponents(1) ;
     ReceiveMaxExponents(1) ;
     LocalMantissaGroup[1].Init(Source+(x+1)*k_TreeExponentBlockSize,MaxExponentBlock[1]) ;
     SendLocalMantissae(1) ;
     ReceiveReducedMantissae(1) ;
     GenerateTarget(1,Target+(x+1)*k_TreeExponentBlockSize) ;
   } /* endif */

   // Handle a partial block by zero-padding to a full block
   if (PartialSize > 0)
   {
      double LocalTarget[k_TreeExponentBlockSize] ;
      double LocalSource[k_TreeExponentBlockSize] ;
      for (unsigned int x=0;x<PartialSize ;x+=1 )
      {
         LocalSource[x] = Source[PartialStart+x] ;
      } /* endfor */
      for (unsigned int y=PartialSize; y<k_TreeExponentBlockSize; y+=1)
      {
         LocalSource[y] = 0.0 ;
      } /* endfor */

	// Single block reduce
      LocalExponentBlock[0].Init(LocalSource)  ;
      SendLocalExponents(0) ;
      ReceiveMaxExponents(0) ;
      LocalMantissaGroup[0].Init(LocalSource,MaxExponentBlock[0]) ;
      SendLocalMantissae(0) ;
      ReceiveReducedMantissae(0) ;
      GenerateTarget(0,LocalTarget) ;

      for (unsigned int z=0; z<PartialSize;z+=1 )
      {
         Target[PartialStart+z] = LocalTarget[z] ;
      } /* endfor */
   } /* endif */
   } ;

// 'Latency' version, all-reduce of a single 'double'
double TReduce::AllReduce1(
   double Source
)  {
     if ( k_diagnose_fp_reduce )
     {
       s0printf("Point reduce on %d/1000\n",(int)(Source*1000.0)) ;
     } /* endif */
     LocalExponentBlock[0].Init1(Source) ;
     SendLocalExponents(0) ;
     ReceiveMaxExponents(0) ;
     LocalMantissaGroup[0].Init1(Source,MaxExponentBlock[0]) ;
     SendLocalMantissae1(0) ;
     ReceiveReducedMantissae1(0) ;
     double result = GenerateTarget1(0) ;
     return result ;
   }

static TReduce Reducer[2] ;

// Interface function, users don't need the internals ...
void TreeReduce::Init(unsigned int aChannel,unsigned int aClass)
{
   Reducer[aChannel].Init(aChannel,aClass) ;
} ;

void TreeReduce::AllReduce (
   double * Target,
   const double * Source,
   unsigned int Length,
   unsigned int Channel
   )
{
  Reducer[Channel].AllReduce(Target,Source,Length) ;
}

double TreeReduce::AllReduce1 (
   double Source,
   unsigned int Channel
   )
{
  return Reducer[Channel].AllReduce1(Source) ;
}

// And a version callable from C
void tr_AllReduceInit ( unsigned int aChannel ,unsigned int aClass )
{
   Reducer[aChannel].Init(aChannel,aClass) ;
}

void tr_AllReduce (
   double * Target,
   const double * Source,
   unsigned int Length,
   unsigned int Channel
)
{
   Reducer[Channel].AllReduce(Target,Source,Length) ;
}

double tr_AllReduce1 (
   double Source,
   unsigned int Channel
)
{
   return Reducer[Channel].AllReduce1(Source) ;
}
