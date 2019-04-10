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
 * (c) Copyright IBM Corp. 2006. CPL
 */ 
/*
 * Storage mapping of an IEEE double-precision number, for access to parts of it as integers or bits
 * This is big-endian specific, for little-endian you have to swap m_hi and m_lo, then test it !
 * The intended use of this is in calculating exp(x)
 */
class DoubleMap
{
  public:
  class UIntPair
  {
     public:
    unsigned int m_hi ;
    unsigned int m_lo ;
  } ;
   union {
      double m_d ;
      UIntPair m_u ;
      } m_value ;
  DoubleMap(void) { } ;
  DoubleMap(double X) { m_value.m_d = X ; } ;
  DoubleMap(
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
  double GetValue(void) const { return m_value.m_d ; } ;

  void SetValue(double X) { m_value.m_d = X ; } ;
  void SetValue(
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

  unsigned int HiWord(void) const { return m_value.m_u.m_hi ; } ;
  unsigned int LoWord(void) const { return m_value.m_u.m_lo ; } ;

  unsigned int SignBit(void) const { return HiWord() & 0x80000000 ; } ;
  unsigned int ExponentBits(void) const { return HiWord() & 0x7ff00000 ; } ;
  unsigned int SignificandHiBits(void) const { return HiWord() & 0x000fffff ; } ;
  unsigned int SignificandLoBits(void) const { return LoWord() ; } ;

  void SetSignificandHiBits(unsigned int new_hi_bits) { m_value.m_u.m_hi = ( m_value.m_u.m_hi & 0xfff00000 )
                                                                            | ( new_hi_bits & 0x000fffff ) ; }
  int Exponent(void) const { return ( ExponentBits() >> 20 ) - 1023 ; } ;
  unsigned int SignificandHi(void) const { return SignificandHiBits() | 0x00100000 ; } ;
  unsigned int SignificandLo(void) const { return SignificandLoBits() ; } ;
  bool IsNegative(void) const { return  0 != SignBit() ; } ;
} ;
  /*
   * Branchless exp(x), with a view to vectorising on Double Hummer
   */

  static inline double hexp(double x)
  {

    const double tp10 = 1024.0 ;
    const double tp20 = tp10*tp10 ;
    const double tp40 = tp20*tp20 ;

     // Dividing by ln(2) gives a value such that we can put the integer part into an exponent.
     // Adding (2**44+2**43) aligns and rounds this so that
     // bottom 8 bits can be used for lookup
     // higher bits can be stuffed into exponent
     // truncated bits (recovered by subtraction) can be fed to power series
    const double x1 = x * ( 1.0 / M_LN2 ) + ( tp40 * ( 16.0 + 8.0 ) ) ;
    const DoubleMap m1(x1) ;
    // Figure the appropriate power of 2 from the significand high bits
    const unsigned int sig_lo = m1.SignificandLoBits() ;
    const DoubleMap m2(0,sig_lo >> 8, 0, 0) ;
    // Recover the number that we will have 'exponentiated' by the bit twiddling
    const double xl2 =  x1 - ( tp40 * ( 16.0 + 8.0 ) ) ;
    // Can range-check xl2 to see if sig_lo put a sensible value in m2
    const double xx4 = xl2 * M_LN2 ;

    // Look up the next several bits (4) in a multiplication table
    const unsigned int tabits =(sig_lo >> 4) & 0x0f ;
    const double x31 = ExpTable1[tabits] ;

    // And the next 4 bits in another table
    const unsigned int tabits2=sig_lo & 0x0f ;
    const double x32 = ExpTable2[tabits2] ;
    const double x3 = x31*x32 ;

    // Figure the remaining part of the original number
    const double z = x - xx4 ;
    // z should be between +- (2**-8); feed in to polynomial for exp(z)

    const double f5 =          1.0/(2.0*3.0*4.0*5.0) ;
    const double f4 = z * f5 + 1.0/(2.0*3.0*4.0) ;
    const double f3 = z * f4 + 1.0/(2.0*3.0) ;
    const double f2 = z * f3 + 1.0/2.0 ;
    const double f1 = z * f2 + 1.0 ;
    const double f0 = z * f1 + 1.0 ;

    const double p0 = f0 * x3 ;

    const double x2=m2.GetValue() ;

    const double r0 = p0 * x2 ;

    // Fixup for out-of-range parameter
    const double resultl = fsel(x+709.0, r0, 0.0) ;
    const double resulth = fsel(x-709.0, Infinity, resultl) ;

    return resulth ;
  }

 // Log base e, along similar lines
 // (actually computes log(abs(x)), gives a large negative number for f(0), gives a large positive
 //  number if fed Inf or Nan)
 static inline double hlog(double x)
 {
   const DoubleMap m(x) ;
   int exponent = m.Exponent() ;
   unsigned int sig_hi = m.SignificandHiBits() ;
   DoubleMap m1(0,0,sig_hi, m.SignificandLoBits()) ;
   unsigned int tableIndex = sig_hi >> 16 ;
   double xx=m1.GetValue()*hlogComp[tableIndex] - 1.0 ;

   // There should be scope for shortening this polynomial
   // (1) rescale for 'xx' to be in (-1/32, 1/32) rather than (0,1/16)
   // (2) have a larger table, and take more bits to index it
   // (actually, not sure this is large enough for double precision)
   double p10 =         (-1.0/10.0) ;
   double p9  = p10 * xx + (  1.0 / 9.0 ) ;
   double p8  = p9 * xx + ( -1.0 / 8.0 ) ;
   double p7  = p8 * xx + (  1.0 / 7.0 ) ;
   double p6  = p7 * xx + ( -1.0 / 6.0 ) ;
   double p5  = p6 * xx + (  1.0 / 5.0 ) ;
   double p4  = p5 * xx + ( -1.0 / 4.0 ) ;
   double p3  = p4 * xx + (  1.0 / 3.0 ) ;
   double p2  = p3 * xx + ( -1.0 / 2.0 ) ;
   double p1  = p2 * xx + (  1.0 )  ;
   double p0  = p1 * xx ;
   double result= exponent*M_LN2 + hlogTable[tableIndex] + p0 ;

/*
 * BegLogLine(1)
 *   << "hlog x=" << x
 *   << " exponent=" << exponent
 *   << " tableIndex=" << tableIndex
 *   << " xx=" << xx
 *   << " p0=" << p0
 *   << " result=" << result
 *   << EndLogLine ;
 */
   return result ;
 }
const double Math::hlogTable[16] = {
  0.0 // ::log(1.0) ,
  , 0.06062462181643484 // ::log(1.0 + 1.0/16.0),
  , 0.11778303565638346 // ::log(1.0 + 2.0/16.0),
  , 0.17185025692665923 // ::log(1.0 + 3.0/16.0),
  , 0.22314355131420976 // ::log(1.0 + 4.0/16.0),
  , 0.27193371548364176 // ::log(1.0 + 5.0/16.0),
  , 0.31845373111853459 // ::log(1.0 + 6.0/16.0),
  , 0.36290549368936847 // ::log(1.0 + 7.0/16.0),
  , 0.40546510810816438 // ::log(1.0 + 8.0/16.0),
  , 0.44628710262841953 // ::log(1.0 + 9.0/16.0),
  , 0.48550781578170082 // ::log(1.0 + 10.0/16.0),
  , 0.52324814376454787 // ::log(1.0 + 11.0/16.0),
  , 0.55961578793542266 // ::log(1.0 + 12.0/16.0),
  , 0.59470710774669278 // ::log(1.0 + 13.0/16.0),
  , 0.62860865942237409 // ::log(1.0 + 14.0/16.0),
  , 0.66139848224536502 // ::log(1.0 + 15.0/16.0)
} ;
const double Math::hlogComp[16] = {
  1.0 / ( 1.0 + 0.0/16.0 ) ,
  1.0 / ( 1.0 + 1.0/16.0 ) ,
  1.0 / ( 1.0 + 2.0/16.0 ) ,
  1.0 / ( 1.0 + 3.0/16.0 ) ,
  1.0 / ( 1.0 + 4.0/16.0 ) ,
  1.0 / ( 1.0 + 5.0/16.0 ) ,
  1.0 / ( 1.0 + 6.0/16.0 ) ,
  1.0 / ( 1.0 + 7.0/16.0 ) ,
  1.0 / ( 1.0 + 8.0/16.0 ) ,
  1.0 / ( 1.0 + 9.0/16.0 ) ,
  1.0 / ( 1.0 + 10.0/16.0 ) ,
  1.0 / ( 1.0 + 11.0/16.0 ) ,
  1.0 / ( 1.0 + 12.0/16.0 ) ,
  1.0 / ( 1.0 + 13.0/16.0 ) ,
  1.0 / ( 1.0 + 14.0/16.0 ) ,
  1.0 / ( 1.0 + 15.0/16.0 )
} ;
 #ifdef DBL_MAX
const double Math::Infinity = DBL_MAX * 2.0 ;  // Intended to overflow; will be used as result of hexp(x) for x > 709
#else
#if defined(PK_BGL)
// BG/L compiler doesn't support HUGE_VAL quite the way we want ...
const double Math::Infinity = (1e200*1e200) ; // Intended to overflow
#else
const double Math::Infinity = HUGE_VAL * HUGE_VAL ;
#endif
#endif
const double ExpK1 = M_LN2/16.0 ;
const double ExpK2 = M_LN2/256.0 ;
const double Math::ExpTable1[16] = {
//      ::exp( 0.0*ExpK1) ,       ::exp( 1.0*ExpK1) ,       ::exp( 2.0*ExpK1) ,       ::exp( 3.0*ExpK1) ,
        1.00000000000000000000 ,  1.04427378242741383881 ,  1.09050773266525765605 ,  1.13878863475669164875
//      ::exp( 4.0*ExpK1) ,       ::exp( 5.0*ExpK1) ,       ::exp( 6.0*ExpK1) ,       ::exp( 7.0*ExpK1) ,
     ,  1.18920711500272105982 ,  1.24185781207348403959 ,  1.29683955465100965466 ,  1.35425554693689271456
//      ::exp( 8.0*ExpK1) ,       ::exp( 9.0*ExpK1) ,       ::exp(10.0*ExpK1) ,       ::exp(11.0*ExpK1) ,
     ,  1.41421356237309503240 ,  1.47682614593949929212 ,  1.54221082540794080126 ,  1.61049033194925428250
//      ::exp(12.0*ExpK1) ,       ::exp(13.0*ExpK1) ,       ::exp(14.0*ExpK1) ,       ::exp(15.0*ExpK1) ,
     ,  1.68179283050742905681 ,  1.75625216037329945002 ,  1.83400808640934242627 ,  1.91520656139714725223

   } ;
const double Math::ExpTable2[16] = {
//      ::exp( 0.0*ExpK2) ,       ::exp( 1.0*ExpK2) ,       ::exp( 2.0*ExpK2) ,       ::exp( 3.0*ExpK2) ,
        1.00000000000000000000 ,  1.00271127505020248534 ,  1.00542990111280282117 ,  1.00815589811841751551
//      ::exp( 4.0*ExpK2) ,       ::exp( 5.0*ExpK2) ,       ::exp( 6.0*ExpK2) ,       ::exp( 7.0*ExpK2) ,
     ,  1.01088928605170045965 ,  1.01363008495148943838 ,  1.01637831491095303739 ,  1.01913399607773794904
//      ::exp( 8.0*ExpK2) ,       ::exp( 9.0*ExpK2) ,       ::exp(10.0*ExpK2) ,       ::exp(11.0*ExpK2) ,
     ,  1.02189714865411667749 ,  1.02466779289713564431 ,  1.02744594911876369561 ,  1.03023163768604101185
//      ::exp(12.0*ExpK2) ,       ::exp(13.0*ExpK2) ,       ::exp(14.0*ExpK2) ,       ::exp(15.0*ExpK2) ,
     ,  1.03302487902122842138 ,  1.03582569360195711881 ,  1.03863410196137878930 ,  1.04145012468831613985
   } ;
 