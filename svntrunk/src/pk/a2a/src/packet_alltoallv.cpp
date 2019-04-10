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
 #define CACHE_ISOLATION_PADDING_SIZE ( 0 )

//#include <complex>
//using namespace std ;
//class complex
//  {
//  public:
//    double re ;
//    double im ;
//    complex()       { }
//    complex(double r, double i = 0.0)       { re=r; im=i; }
//    complex(const complex& c) { re = c.re; im = c.im; }
//  } ;

class cx
{
public:
  double re ;
  double im ;
};

int alltoallv_debug = 0;
/// #define DEBUG_ALL2ALL

#define PACKET_POLL_COUNT 4
#define PACKETS_TO_INJECT 6

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/pktrace.hpp>
#include <pk/a2a/packet_alltoallv.hpp>
#include <BonB/BGLTorusAppSupport.c>


static TraceClient AlltoallvInitStart;
static TraceClient AlltoallvInitFinis;
static TraceClient AlltoallvSimpleExecBarrierStart;
static TraceClient AlltoallvSimpleExecBarrierFinis;
static TraceClient AlltoallvExecBarrierStart;
static TraceClient AlltoallvExecBarrierFinis;

static TraceClient AlltoallvPermRanksStart;
static TraceClient AlltoallvPermRanksFinis;

static TraceClient AlltoallvShellStart;

static TraceClient AlltoallvExecuteStart;
static TraceClient AlltoallvExecuteFinis;
static TraceClient AlltoallvExecuteSimpleStart ;
static TraceClient AlltoallvExecuteSimpleFinis ;

#ifndef PKTRACE_ALLTOALLV_INIT
#define PKTRACE_ALLTOALLV_INIT ( 0 )
#endif

#ifndef PKTRACE_ALLTOALLV_DETAIL
#define PKTRACE_ALLTOALLV_DETAIL ( 0 )
#endif

#ifndef PKTRACE_ALLTOALLV_EXECUTE
#define PKTRACE_ALLTOALLV_EXECUTE ( 0 )
#endif

#ifndef PKFXLOG_ALLTOALLV
#define PKFXLOG_ALLTOALLV (0)
#endif

#ifndef PKFXLOG_ALLTOALLV_DATA
#define PKFXLOG_ALLTOALLV_DATA (0)
#endif

#ifndef PKFXLOG_PACKET_ALLTOALLV
#define PKFXLOG_PACKET_ALLTOALLV (0)
#endif

#ifndef DIAGNOSE_PACKET_ALLTOALLV
#define DIAGNOSE_PACKET_ALLTOALLV (1)
#endif


int Verbose = 0;

void memcpy_double( double* aTrg, double* aSrc, int aNumberOfChars )
{
int NumberOfDoubles = aNumberOfChars / sizeof( double );
int NumberRemainingChars = aNumberOfChars - (sizeof(double) * NumberOfDoubles );
for( int i=0; i<NumberOfDoubles; i++ )
    aTrg[ i ] = aSrc[ i ];

if( NumberRemainingChars )
    memcpy( (void *) & aTrg[ NumberOfDoubles ],
            (void *) & aSrc[ NumberOfDoubles ],
            NumberRemainingChars );
}

void RandomizeLinks( int* aLinks, int aCount, int aMyRank )
  {
  char state[256];
  char *is;
  is = initstate(aMyRank, (char *)&state, sizeof(state));
  for( int i=0; i < aCount; i++ )
    {
    int index = random() % aCount;
    assert( index >= 0 && index < aCount );

    int temp = aLinks[ index ];
    aLinks[ index ] = aLinks[ i ];
    aLinks[ i ] = temp;
    }
  setstate(is);
  return;
  }

class BGLAlltoallvHeadersSorter
{
public:
    static TorusXYZ mMyCoord;
    static TorusXYZ mPeriod;

    static
    void
    Init( TorusXYZ aMyCoord, TorusXYZ aPeriod )
        {
        mPeriod = aPeriod;
        mMyCoord = aMyCoord;
        }

    static int Compare_BGL_TorusPktHdr( const void* elem1, const void* elem2 )
        {
        BGLAlltoallvHeaders *e1 = (BGLAlltoallvHeaders *) elem1;
        BGLAlltoallvHeaders *e2 = (BGLAlltoallvHeaders *) elem2;

        TorusXYZ DestCoord1;
        DestCoord1.mX = e1->mHdr.hwh0.X;
        DestCoord1.mY = e1->mHdr.hwh1.Y;
        DestCoord1.mZ = e1->mHdr.hwh1.Z;

        TorusXYZ DestCoord2;
        DestCoord2.mX = e2->mHdr.hwh0.X;
        DestCoord2.mY = e2->mHdr.hwh1.Y;
        DestCoord2.mZ = e2->mHdr.hwh1.Z;

        TorusXYZ Vec1 = mMyCoord.ManhattanVectorImaged( mPeriod, DestCoord1 );
        TorusXYZ Vec2 = mMyCoord.ManhattanVectorImaged( mPeriod, DestCoord2 );

        char Dist1 = (char) Vec1.Max();
        char Dist2 = (char) Vec2.Max();
        if( Dist1 < Dist2 )
            return -1;
        else if( Dist1 == Dist2 )
            return 0;
        else
            return 1;
        }

    static
    void
    Execute( BGLAlltoallvHeaders* aHdrs, int aCount )
        {
        Platform::Algorithm::Qsort( aHdrs, aCount, sizeof( BGLAlltoallvHeaders ), Compare_BGL_TorusPktHdr );
        }
};

TorusXYZ BGLAlltoallvHeadersSorter::mMyCoord;
TorusXYZ BGLAlltoallvHeadersSorter::mPeriod;

// Randomize Tools
void
PacketAlltoAllv::RandomizeHeaders( BGLAlltoallvHeaders* aHdrs, int aCount )
  {
  char state[256];
  char *is;
  is = initstate(365, (char *)&state, sizeof(state));
  for( int i=0; i < aCount ; i++ )
    {
    int index = random() % aCount;
    assert( index >= 0 && index < aCount );

    BGLAlltoallvHeaders temp = aHdrs[ index ];
    aHdrs[ index ] = aHdrs[ i ];
    aHdrs[ i ] = temp;
    }
  setstate(is);
  return;
  }

class BGL_AllToAll_TorusPktHdr
{
	public: 
	_BGL_Torus_HdrHW0 hwh0 ; 
	_BGL_Torus_HdrHW1 hwh1 ; 
	BGL_AlltoallvPacketMetaData PacketMetaData ;
} ;

static inline void BGLTorusInjectPacketSeparateHeaderApp(
        _BGL_TorusFifo *fifo,  // Injection FIFO to use (Must have space available!)
        _BGL_TorusPktHdr *header, // Prebuilt header
        void* dst // Data (must be appropriately aligned)
         )  // Address of ready-to-go packet
{
   int chunks = header->hwh0.Chunks;
   char * dstc = (char *) dst ;
   register int u=16 ;
   // Push out the header and the first 16 bytes
//   __stfpd((fifoType)fifo, *(double _Complex *) header) ;
//   __stfpd((fifoType)fifo, *(double _Complex *) dstc );
      asm volatile( "lfpdx   3,0,%0;"
                    "lfpdx   4,0,%1;"
                    "stfpdx  3,0,%2;"
                    "stfpdx  4,0,%2;"
                    :
                    : "b" (header), "b" (dstc), "r"  (fifo)
                    : "memory",  FR3STR, FR4STR );
   
   // Push out 64-byte blocks
//   dstc += 16 ; 
   while ( chunks > 1 )
   {
   	  chunks -= 2 ; 
//   	  	double _Complex Chunk0 = *(double _Complex *)(dstc) ;
// 	  	double _Complex Chunk1 = *(double _Complex *)(dstc+16) ;
//	  	double _Complex Chunk2 = *(double _Complex *)(dstc+32) ;
//	  	double _Complex Chunk3 = *(double _Complex *)(dstc+48) ;
//	  	 __stfpd((fifoType)fifo,Chunk0) ;
//	  	 __stfpd((fifoType)fifo,Chunk1) ;
//	  	 __stfpd((fifoType)fifo,Chunk2) ;
//	  	 __stfpd((fifoType)fifo,Chunk3) ;
//	  	dstc += 64 ;
      asm volatile( "lfpdux   3,%0,%1;"
                    "lfpdux   4,%0,%1;"
                    "lfpdux   5,%0,%1;"
                    "lfpdux   6,%0,%1;"
                    "stfpdx 3,0,%2;"
                    "stfpdx 4,0,%2;"
                    "stfpdx 5,0,%2;"
                    "stfpdx 6,0,%2;"
                    : "=b" (dstc)
                    : "b"  (u), "r"  (fifo), "0"(dstc)
                    : "memory", FR3STR, FR4STR, FR5STR, FR6STR );
   }
   // If there's a 32-byte block to go, push it out too
   if ( chunks )
   {
//   	  	double _Complex Chunk0 = *(double _Complex *)(dstc) ;
// 	  	double _Complex Chunk1 = *(double _Complex *)(dstc+16) ;
//	  	__stfpd((fifoType)fifo,Chunk0) ;
//	  	__stfpd((fifoType)fifo,Chunk1) ;
      asm volatile( "lfpdux   3,%0,%1;"
                    "lfpdux   4,%0,%1;"
                    "stfpdx 3,0,%2;"
                    "stfpdx 4,0,%2;"
                    : "=b" (dstc)
                    : "b"  (u), "r"  (fifo), "0"(dstc)
                    : "memory", FR3STR, FR4STR );
   }
}
// Put: move entire packet from fifo to buf
    template< int NON_ALIGNED1 > static inline int _ts_AppReceiver_Alltoallv_Put( _BGL_TorusFifo *fifo, PacketAlltoAllv* ThisPtr);

// Drain: move possibly several packets (up to 'chunks' chunks) from fifo to buf
    template< int NON_ALIGNED1 > static inline int _ts_AppReceiver_Alltoallv_Drain( _BGL_TorusFifo *fifo, PacketAlltoAllv* ThisPtr, int Chunks);

    template< int NON_ALIGNED1 > static inline void _ts_PollBoth_OnePassReceive_Alltoallv_Put( _BGL_TorusFifoStatus *statF0, _BGL_TorusFifoStatus *statF1, PacketAlltoAllv* ThisPtr );
    template< int NON_ALIGNED1 > static inline void _ts_Poll0_OnePassReceive_Alltoallv_Put( _BGL_TorusFifoStatus *statF0, PacketAlltoAllv* ThisPtr );
    template< int NON_ALIGNED1 > static inline int  _ts_Poll0_OnePassReceive_Alltoallv_Chunk_Put( _BGL_TorusFifoStatus *statF0, PacketAlltoAllv* ThisPtr );
    template< int NON_ALIGNED1 > static inline int  _ts_Poll0_OnePassReceive_Alltoallv_Drain( _BGL_TorusFifoStatus *statF0, PacketAlltoAllv* ThisPtr );
    template< int NON_ALIGNED1 > static inline void _ts_Poll1_OnePassReceive_Alltoallv_Put( _BGL_TorusFifoStatus *statF1, PacketAlltoAllv* ThisPtr );
    template< int NON_ALIGNED1 > static inline int  _ts_Poll1_OnePassReceive_Alltoallv_Drain( _BGL_TorusFifoStatus *statF1, PacketAlltoAllv* ThisPtr );
    template< int NON_ALIGNED1 > static inline int _ts_PollBoth_OnePassReceive_Alltoallv_Drain( _BGL_TorusFifoStatus *statF0, _BGL_TorusFifoStatus *statF1, PacketAlltoAllv* ThisPtr );


template< int NON_ALIGNED1 >
static inline
int _ts_AppReceiver_Alltoallv_Drain( _BGL_TorusFifo *fifo, PacketAlltoAllv* aThis, int ChunksInFifo )
  {
    enum { k_DrainDepth = _BGL_TORUS_RECEPTION_FIFO_CHUNKS } ;
    fStatus.recordReceive(ChunksInFifo) ;
     
    register int u = 16;
    assert(ChunksInFifo <=  k_DrainDepth) ;
    // Get all the lines
    cx HalfChunks[2*k_DrainDepth] ALIGN_QUADWORD ;
    char * ScratchDst= ((char *) HalfChunks) - 16 ;
    int HalfchunksInFifo = 2*ChunksInFifo ; 
    while ( HalfchunksInFifo >= 3 )
    {
      HalfchunksInFifo -= 3 ;
      asm volatile( "lfpdx   3,0,%2;"
                    "lfpdx   4,0,%2;"
                    "lfpdx   5,0,%2;"
                    "stfpdux 3,%0,%1;"
                    "stfpdux 4,%0,%1;"
                    "stfpdux 5,%0,%1;"
                    : "=b" (ScratchDst)
                    : "b"  (u), "r"  (fifo), "0" (ScratchDst)
                    : "memory", FR3STR, FR4STR, FR5STR );
    	
    }
    if ( 2 == HalfchunksInFifo )
    {
       asm volatile( "lfpdx   3,0,%2;"
                     "lfpdx   4,0,%2;"
                    "stfpdux 3,%0,%1;"
                    "stfpdux 4,%0,%1;"
                    : "=b" (ScratchDst)
                    : "b"  (u), "r"  (fifo), "0" (ScratchDst)
                    : "memory", FR3STR, FR4STR );
    } else if ( 1 == HalfchunksInFifo )
    {
       asm volatile( "lfpdx   3,0,%2;"
                    "stfpdux 3,%0,%1;"
                    : "=b" (ScratchDst)
                    : "b"  (u), "r"  (fifo), "0" (ScratchDst)
                    : "memory", FR3STR );
    }
//    int x=ChunksInFifo & 1 ;
//    if ( x != 0 )
//    {
////		double _Complex HalfChunk0 = __lfpd((fifoType)fifo) ;
////		double _Complex HalfChunk1 = __lfpd((fifoType)fifo) ;
////    	HalfChunks[0] = HalfChunk0 ; 
////    	HalfChunks[1] = HalfChunk1 ;
//      asm volatile( "lfpdx   3,0,%2;"
//                    "lfpdx   4,0,%2;"
//                    "stfpdux 3,%0,%1;"
//                    "stfpdux 4,%0,%1;"
//                    : "+b" (ScratchDst)
//                    : "b"  (u),
//                    "r"  (fifo)
//                    : "memory",
//                    FR3STR, FR4STR );
//
//    }
//    
//    for ( int a=0;a<ChunksInFifo/2;a+=1)
//    {
////		double _Complex HalfChunk0 = __lfpd((fifoType)fifo) ;
////		double _Complex HalfChunk1 = __lfpd((fifoType)fifo) ;
////		double _Complex HalfChunk2 = __lfpd((fifoType)fifo) ;
////		double _Complex HalfChunk3 = __lfpd((fifoType)fifo) ;
////    	HalfChunks[2*x]=HalfChunk0 ;
////    	HalfChunks[2*x+1]=HalfChunk1 ;
////    	HalfChunks[2*x+2]=HalfChunk2 ;
////    	HalfChunks[2*x+3]=HalfChunk3 ;
////    	x+=2 ;
//      asm volatile( "lfpdx   3,0,%2;"
//                    "lfpdx   4,0,%2;"
//                    "lfpdx   5,0,%2;"
//                    "lfpdx   6,0,%2;"
//                    "stfpdux 3,%0,%1;"
//                    "stfpdux 4,%0,%1;"
//                    "stfpdux 5,%0,%1;"
//                    "stfpdux 6,%0,%1;"
//                    : "+b" (ScratchDst)
//                    : "b"  (u),
//                    "r"  (fifo)
//                    : "memory",
//                    FR3STR, FR4STR, FR5STR, FR6STR );
//    }
    
    // The lines are all in memory now; process them
    

  int PacketsInFifo = 0 ; 
  int Chunks = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->hwh0.Chunks ;
  int SourceRank = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mSourceRank ;
  int TotalCountToRecv = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mTotalCount ;
  int Offset = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mOffset ;
  int BytesInPayload = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mBytesInPayload ;
  int ThisPacketChunkIndex=0 ;
  while ( ThisPacketChunkIndex < ChunksInFifo )
  { 
	
	  	void ** RawRecV = (void **) (HalfChunks+2*ThisPacketChunkIndex) ;
	  	BegLogLine(PKFXLOG_ALLTOALLV_DATA)
	  	    << " Raw packet 0x" << RawRecV[0]
	  	    << RawRecV[1]
	  	    << RawRecV[2]
	  	    << RawRecV[3]
	  	 	<< " "
	  	    << RawRecV[4]
	  	    << RawRecV[5]
	  	    << RawRecV[6]
	  	    << RawRecV[7]
	  	    << EndLogLine ;
	  PacketsInFifo += 1 ; 
	  BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
	    << "_ts_AppReceiver_Alltoallv_Put"
	    << " SourceRank=" << SourceRank
	    << " TotalCountToRecv=" << TotalCountToRecv
	    << " aThis->mRecvTypeSize=" << aThis->mRecvTypeSize
	    << " Offset=" << Offset
	    << " BytesInPayload=" <<  BytesInPayload
	    << " Chunks=" << Chunks 
	    << EndLogLine ;
	  assert( SourceRank != aThis->mMyRank );
	  assert( BytesInPayload );
	
	
	  assert( Offset + BytesInPayload <= TotalCountToRecv * aThis->mRecvTypeSize );
	  assert( SourceRank >=0 && SourceRank < aThis->mGroupSize );
	
	  int PrevBytesRecvFromNode=aThis->mBytesRecvFromNode[ SourceRank ] ;
	  if( DIAGNOSE_PACKET_ALLTOALLV )
	    {
		  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
		    << "PrevBytesRecvFromNode=" << PrevBytesRecvFromNode
		    << EndLogLine ;
		  // This catches cases where we get a packet from a node we were not expecting
		  assert(PrevBytesRecvFromNode != -1) ;
	    }
	  int BytesRecvFromNode=PrevBytesRecvFromNode +BytesInPayload ;
	  aThis->mBytesRecvFromNode[ SourceRank ] = BytesRecvFromNode;
	
	  int ReceiveTypeSize = aThis->mRecvTypeSize ;
	  char * RealDst = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*ReceiveTypeSize) + Offset;
	  int BytesToGoFromNode = (TotalCountToRecv * ReceiveTypeSize) - BytesRecvFromNode ;
	  int CompletionFromNode = ( BytesToGoFromNode == 0 ) ;
	  aThis->mRecvCountDone += CompletionFromNode ;
      aThis->mRecvCount[ SourceRank ] = TotalCountToRecv;
#if PKFXLOG_PACKET_ALLTOALLV	  
      if ( CompletionFromNode )
      {
  	    aThis->mRecvDoneBits |= ( 1 << SourceRank ) ; 
      }
#endif      
	  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
	    << "_ts_AppReceiver_Alltoallv_Put aThis=" << aThis
	    << " mRecvBuff=" << aThis->mRecvBuff
	    << " ReceiveTypeSize=" << ReceiveTypeSize
	    << " SourceRank=" << SourceRank
	    << " aThis->mRecvOffset[ SourceRank ]=" << aThis->mRecvOffset[ SourceRank ]
	    << " Offset=" << Offset
	    << " RealDst=" << (void *)RealDst 
        << " PrevBytesRecvFromNode=" << PrevBytesRecvFromNode 
	    << " BytesToGoFromNode=" << BytesToGoFromNode
	    << " mRecvDoneBits=" << (void *)(aThis->mRecvDoneBits) 
	    << " mRecvCountDone=" << aThis->mRecvCountDone 
	    << EndLogLine ;
      assert(BytesToGoFromNode >= 0) ;
	  int PayloadHalfChunkIndex = 2*ThisPacketChunkIndex+1 ;
	  int BytesInPayloadThis = BytesInPayload ;
	  int BytesInPayloadCopy = BytesInPayload ;
      int SourceRankCopy = SourceRank ; 
	  // Set up for next packet
	  ThisPacketChunkIndex += (Chunks+1) ; 
      Chunks = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->hwh0.Chunks ;
      SourceRank = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mSourceRank ;
      TotalCountToRecv = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mTotalCount ;
      Offset = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mOffset ;
      BytesInPayload = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mBytesInPayload ;
      // and drop 'this' packet into is proper store
      if ( NON_ALIGNED1 )
      {
      	// If things might be misaligned or not multiples of a doubleword
	  	memcpy(RealDst,HalfChunks+PayloadHalfChunkIndex,BytesInPayloadThis) ; 
      } else {
      	// Things are aligned doublewords
      	double * QuarterChunks = (double *) (HalfChunks+PayloadHalfChunkIndex) ;
      	double * RealDestDbl = (double *) RealDst ;
      	double QC0 = QuarterChunks[0] ;
      	double QC1 = QuarterChunks[1] ;
      	double QC2 = QuarterChunks[2] ;
      	double QC3 = QuarterChunks[3] ;
      	int PayloadIndex = 0 ; 
      	while ( BytesInPayloadThis > 24 )
      	{
      		RealDestDbl[PayloadIndex] = QC0 ;
      		RealDestDbl[PayloadIndex+1] = QC1 ;
      		RealDestDbl[PayloadIndex+2] = QC2 ;
      		RealDestDbl[PayloadIndex+3] = QC3 ;
      		QC0 = QuarterChunks[PayloadIndex+4] ;
      		QC1 = QuarterChunks[PayloadIndex+5] ;
      		QC2 = QuarterChunks[PayloadIndex+6] ;
      		QC3 = QuarterChunks[PayloadIndex+7] ;
      		PayloadIndex += 4 ;
      		BytesInPayloadThis -= 32 ;
      	}
      	if ( BytesInPayloadThis > 0 )
      	{
      		RealDestDbl[PayloadIndex] = QC0 ;
      	    if ( BytesInPayloadThis > 8 )
      	    {
      		  RealDestDbl[PayloadIndex+1] = QC1 ;
      	      if ( BytesInPayloadThis > 16 )
      	      {
      		    RealDestDbl[PayloadIndex+2] = QC2 ;
      	      }
       	   }
        }
      }
  	void ** BlockRecV = (void **) RealDst ;
  	BegLogLine(PKFXLOG_ALLTOALLV_DATA)
  	    << "Length=" << BytesInPayloadCopy
          << " SourceRank=" << SourceRankCopy
  	    << " Data 0x" << BlockRecV[0]
  	    << BlockRecV[1]
  	    << BlockRecV[2]
  	    << BlockRecV[3]
  	    << BlockRecV[4]
  	    << BlockRecV[5]
  	    << BlockRecV[6]
  	    << BlockRecV[7]
  	    << EndLogLine ;
//	  if ( ( ((unsigned long)RealDst & 0x7) == 0 ) && ( BytesInPayloadThis %  16 == 0 ) )
//	  { 
//	  	while ( BytesInPayloadThis > 48 )
//	  	{
////	  		double _Complex Chunk0 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex)) ;
////	  		double _Complex Chunk1 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+1));
////	  		double _Complex Chunk2 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+2));
////	  		double _Complex Chunk3 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+3));
//	  		double _Complex Chunk0 = HalfChunks[PayloadHalfChunkIndex] ;
//	  		double _Complex Chunk1 = HalfChunks[PayloadHalfChunkIndex+1] ;
//	  		double _Complex Chunk2 = HalfChunks[PayloadHalfChunkIndex+2] ;
//	  		double _Complex Chunk3 = HalfChunks[PayloadHalfChunkIndex+3] ;
//	  		PayloadHalfChunkIndex += 4 ;
//		  	*(double _Complex *) RealDst = Chunk0 ;
//		  	*(double _Complex *) (RealDst+16) = Chunk1 ;
//		  	*(double _Complex *) (RealDst+32) = Chunk2 ;
//		  	*(double _Complex *) (RealDst+48) = Chunk3 ;
//		  	RealDst += 64 ;
//		  	BytesInPayloadThis -= 64  ;
//	  		
//	  	}
//	  	double _Complex Chunk0 = HalfChunks[PayloadHalfChunkIndex] ;
//	  	double _Complex Chunk1 = HalfChunks[PayloadHalfChunkIndex+1] ;
//	  	double _Complex Chunk2 = HalfChunks[PayloadHalfChunkIndex+2] ;
////	  	double _Complex Chunk0 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex)) ;
////	  	double _Complex Chunk1 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+1));
////	  	double _Complex Chunk2 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+2));
//        if ( BytesInPayloadThis > 0 )
//        {
//		  	*(double _Complex *) RealDst = Chunk0 ;
//        	if ( BytesInPayloadThis > 16 )
//        	{
//		  	   *(double _Complex *) (RealDst+16) = Chunk1 ;
//		  	   if ( BytesInPayloadThis > 32 )
//		  	   {
//		  	      *(double _Complex *) (RealDst+32) = Chunk2 ;
//		  	   }	
//        	}
//        }
//	  } else {
//	  	memcpy(RealDst,HalfChunks+PayloadHalfChunkIndex,BytesInPayloadThis) ; 
//	  }	  	
  }  
  
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    << "_ts_AppReceiver_Alltoallv_Drain PacketsInFifo=" << PacketsInFifo
    << EndLogLine ;
  
  aThis->mTotalRecvPackets += PacketsInFifo ;
  	
  return PacketsInFifo ;
  }
// We nees an aligned buffer in case we are trying to receive to misaligned data  
static  _BGL_TorusPktPayload Chunk1;

// Put: move entire packet from fifo to buf
template< int NON_ALIGNED1 >
static inline
int _ts_AppReceiver_Alltoallv_Put( _BGL_TorusFifo *fifo, PacketAlltoAllv* aThis )
  {
//       _BGL_TorusPkt *pkt = (_BGL_TorusPkt *)addr;
//       Bit8 *dst          = (Bit8 *)addr;
  _BGL_TorusPktHdr hdr;
  int chunks, c2;
  register int u = 16;

///#if TORUS_CHECK
#if 0
  _BGL_TorusFifoStatus FifoStatus;
  int core = rts_get_processor_id();
  char NumberOfChunksInStat;

  if( core == 0 )
    {
    _ts_GetStatus0( & FifoStatus );
    if( fifo == (_BGL_TorusFifo *)TS0_R0 )
      {
      NumberOfChunksInStat = FifoStatus.r[0];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R1 )
      {
      NumberOfChunksInStat = FifoStatus.r[1];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R2 )
      {
      NumberOfChunksInStat = FifoStatus.r[2];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R3 )
      {
      NumberOfChunksInStat = FifoStatus.r[3];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R4 )
      {
      NumberOfChunksInStat = FifoStatus.r[4];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R5 )
      {
      NumberOfChunksInStat = FifoStatus.r[5];
      assert( NumberOfChunksInStat );
      }
    else
      assert( 0 );

    if(( (unsigned) fifo & 0xffff0000 ) == ( PA_TORUS_1 & 0xffff0000))
      assert( 0 );
    }
  else
    {
    _ts_GetStatus1( & FifoStatus );

    if( fifo == (_BGL_TorusFifo *)TS1_R0 )
      {
      NumberOfChunksInStat = FifoStatus.r[0];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R1 )
      {
      NumberOfChunksInStat = FifoStatus.r[1];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R2 )
      {
      NumberOfChunksInStat = FifoStatus.r[2];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R3 )
      {
      NumberOfChunksInStat = FifoStatus.r[3];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R4 )
      {
      NumberOfChunksInStat = FifoStatus.r[4];
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R5 )
      {
      NumberOfChunksInStat = FifoStatus.r[5];
      assert( NumberOfChunksInStat );
      }
    else
      assert( 0 );

    if(( (unsigned) fifo & 0xffff0000 ) == ( PA_TORUS_0 & 0xffff0000))
      assert( 0 );
    }
#endif
//#ifdef DEBUG_ALL2ALL
#if 0
  aThis->mTotalRecvPackets++;
#endif

  assert( ((unsigned long)&hdr & 0x000000F) == 0 );

  do {
// pull 1st chunk into buffer
  asm volatile( "lfpdx   1,0,%0;"
                "lfpdx   7,0,%0;"
                "stfpdx  1,0,%1;"
                :
                : "r"  (fifo), "r"  (&hdr)
                 : "memory",  FR1STR, FR7STR );
  } while(0);

//    printf("(I) _ts_AppReceiver_Put(0x%08lx): *stat = %d, swh0.arg0 = %4ld, swh1.arg = %4ld, swa = %d\n",
//            (unsigned long)fifo, *stat, hdr->swh0.arg0, hdr->swh1.arg, hdr->hwh0.SW_Avail );

  chunks = hdr.hwh0.Chunks;  // hw hdr says (chunks - 1), we did chunk 0 already
//  assert( chunks+1 <= NumberOfChunksInStat );
  BGL_AlltoallvPacketMetaData* CurPtrMetaData = (BGL_AlltoallvPacketMetaData *) &hdr.swh0;
  int SourceRank       = CurPtrMetaData->mSourceRank;
  int TotalCountToRecv = CurPtrMetaData->mTotalCount;
  int Offset           = CurPtrMetaData->mOffset;
  int BytesInPayload   = CurPtrMetaData->mBytesInPayload;

  assert( SourceRank != aThis->mMyRank );
  assert( BytesInPayload );

  // if( !( Offset + BytesInPayload <= TotalCountToRecv * aThis->mRecvTypeSize ) )
//   if( alltoallv_debug && (aThis->mMyRank == 0) && (SourceRank == 14) )
//       printf( "Recv: %d %d %d %d %d %d %d\n",
//               SourceRank,
//               BytesInPayload,
//               Offset,
//               TotalCountToRecv,
//               aThis->mRecvTypeSize,
//               aThis->mBytesRecvFromNode[ SourceRank ],
//               aThis->mRecvCountDone );

  assert( Offset + BytesInPayload <= TotalCountToRecv * aThis->mRecvTypeSize );
  assert( SourceRank >=0 && SourceRank < aThis->mGroupSize );

  aThis->mBytesRecvFromNode[ SourceRank ] += BytesInPayload;

  char * dst = NULL;

  if( ! NON_ALIGNED1 )
    {
    // Make sure the dude is alligned
    dst = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*aThis->mRecvTypeSize) + Offset;
    assert( dst );
    assert( ((unsigned long)dst & 0xf) == 0 );
    }
  else
    {
    dst = (char *) &Chunk1;
    }

// Check that we are only writing to non written memory
  int BytesInPayloadTemp   = CurPtrMetaData->mBytesInPayload;

//#ifdef DEBUG_ALL2ALL
//  // static int HeardFrom[ 8 ] = { 0,0,0,0,0,0,0,0 };
//  // HeardFrom[ SourceRank ]++;
//
//  if ( 1 )
//    {
//    complex<double>* CompPtr = (complex<double> *)dst;
//    int error = 0;
//    for( int i=0; i < (BytesInPayloadTemp / sizeof( complex<double> )); i++ )
//      {
//      if( CompPtr[ i ].real() != 66666666.0 ||
//          CompPtr[ i ].imag() != -66666666.0 || error )
//        {
//        error=1;
//        printf("s: %d C[ %d ].re: %f C[ %d ].im: %f tr: %d o: %d b: %d c: %d line: %d\n",
//               SourceRank,
//               i,
//               creal(CompPtr[ i ]),
//               i,
//               cimag(CompPtr[ i ]),
//               TotalCountToRecv,
//               Offset,
//               BytesInPayloadTemp,
//               chunks,
//               __LINE__ );
//
//        // assert( 0 );
//        }
//      }
//    assert( !error );
//    }
//  char * dstTemp = dst;
//#endif

// upto 3 chunks received, strictly less the 3 chunks of
// header + useful payload
  if( BytesInPayload < 80 )
    {
    _BGL_TorusPktPayload Chunk;
    Bit8 *dstTmp = (Bit8 *)&Chunk;
    QuadStore( dstTmp, 7 );

//         if( BytesInPayload == 16 )
//           assert( chunks == 0 );

    // read a chunk from the fifo
    for( int i=0; i<chunks; i++ )
      {
      dstTmp += 16;
      asm volatile( "lfpdx   7,0,%2;"
                    "lfpdx   8,0,%2;"
                    "stfpdx  7,0,%0;"
                    "stfpdux 8,%0,%1;"
                    : "=b" (dstTmp)
                    : "b"  (u),
                    "r"  (fifo) ,"0"  (dstTmp)
                    : "memory", FR7STR, FR8STR );
      }

    memcpy( dst, &Chunk, BytesInPayload );
    }
  else
    {
    // at least 3 chunks of useful payload
    QuadStore( dst, 7 );

    BytesInPayload -= 16;
    dst    += 16;

    for( ; BytesInPayload >= 64; BytesInPayload -= 64 )
      {
      asm volatile( "lfpdx   3,0,%2;"
                    "lfpdx   4,0,%2;"
                    "lfpdx   5,0,%2;"
                    "lfpdx   6,0,%2;"
                    "stfpdx  3,0,%0;"
                    "stfpdux 4,%0,%1;"
                    "stfpdux 5,%0,%1;"
                    "stfpdux 6,%0,%1;"
                    : "=b" (dst)
                    : "b"  (u), "r"  (fifo) ,"0"  (dst)
                    : "memory",  FR3STR, FR4STR, FR5STR, FR6STR );
      dst += 16;
      }

    if( BytesInPayload >= 32 )
      {
      asm volatile( "lfpdx   7,0,%2;"
                    "lfpdx   8,0,%2;"
                    "stfpdx  7,0,%0;"
                    "stfpdux 8,%0,%1;"
                    : "=b" (dst)
                    : "b"  (u), "r"  (fifo),"0"  (dst)
                    : "memory", FR7STR, FR8STR );
      dst += 16;
      BytesInPayload -= 32;
      }

    if( BytesInPayload )
      {
      _BGL_TorusPktPayload Chunk;
      Bit8* dstTmp = (Bit8 *) &Chunk;
      asm volatile( "lfpdx   7,0,%2;"
                    "lfpdx   8,0,%2;"
                    "stfpdx  7,0,%0;"
                    "stfpdux 8,%0,%1;"
                    : "=b" (dstTmp)
                    : "b"  (u), "r"  (fifo), "0" (dstTmp)
                    : "memory", FR7STR, FR8STR );

      memcpy( dst, &Chunk, BytesInPayload );
      }
    }

  if( NON_ALIGNED1 )
    {
    char* OrigDest = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*aThis->mRecvTypeSize) + Offset;
    memcpy( OrigDest, &Chunk1, BytesInPayloadTemp );
	void ** BlockRecV = (void **) OrigDest ;
	BegLogLine(PKFXLOG_ALLTOALLV_DATA)
	    << "Length=" << BytesInPayloadTemp
        << " SourceRank=" << SourceRank
	    << " Data 0x" << BlockRecV[0]
	    << BlockRecV[1]
	    << BlockRecV[2]
	    << BlockRecV[3]
	    << BlockRecV[4]
	    << BlockRecV[5]
	    << BlockRecV[6]
	    << BlockRecV[7]
	    << EndLogLine ;

//     short *BlockToSendAsShort = (short *) OrigDest;
//     for( int i = 0; i < BytesInPayloadTemp / sizeof(short); i++ )
//       {
//       if( alltoallv_debug && (aThis->mMyRank == 0) && (SourceRank == 14) )
//         {
//         short id = (short) BlockToSendAsShort[ i ];
//         int id1 = id;
//         printf( "RECEIVING: %d %d \n", aThis->mMyRank, id );
//         fflush( stdout );
//         }
//       }
    } else {
        char* OrigDest = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*aThis->mRecvTypeSize) + Offset;
    	void ** BlockRecV = (void **) OrigDest ;
    	BegLogLine(PKFXLOG_ALLTOALLV_DATA)
    	    << "Length=" << BytesInPayloadTemp
            << " SourceRank=" << SourceRank
    	    << " Data 0x" << BlockRecV[0]
    	    << BlockRecV[1]
    	    << BlockRecV[2]
    	    << BlockRecV[3]
    	    << BlockRecV[4]
    	    << BlockRecV[5]
    	    << BlockRecV[6]
    	    << BlockRecV[7]
    	    << EndLogLine ;
    }

  if( (TotalCountToRecv * aThis->mRecvTypeSize) == ( aThis->mBytesRecvFromNode[ SourceRank ] ) )
    {
    aThis->mRecvCount[ SourceRank ] = TotalCountToRecv;
    aThis->mRecvCountDone++;

// #ifdef DEBUG_ALL2ALL
//     if( aThis->mRecvCountDone == aThis->mGroupSize )
//       {
//       printf("MyRank: %d HeardFrom: { %d %d %d %d %d %d %d %d }\n",
//              aThis->mMyRank,
//              HeardFrom[ 0 ],
//              HeardFrom[ 1 ],
//              HeardFrom[ 2 ],
//              HeardFrom[ 3 ],
//              HeardFrom[ 4 ],
//              HeardFrom[ 5 ],
//              HeardFrom[ 6 ],
//              HeardFrom[ 7 ] );
//       }
// #endif
    }

#if 0
   if( alltoallv_debug )
    printf( "(I) _ts_AppReceiver_Put(0x%08lx): s: %d t: %d o: %d b: %d btr: %d rd: %d precv: %d\n",
            (unsigned long)fifo,
            SourceRank,
            TotalCountToRecv*aThis->mRecvTypeSize,
            Offset,
            BytesInPayloadTemp,
            aThis->mBytesRecvFromNode[ SourceRank ],
            aThis->mRecvCountDone,
            aThis->mTotalRecvPackets );

    fflush( stdout );
#endif

//#ifdef DEBUG_ALL2ALL
//  if ( 1 )
//    {
//    COMPLEXDOUBLE* CompPtr = (COMPLEXDOUBLE *)dstTemp;
//    int error = 0;
//    for( int i=0; i < (BytesInPayloadTemp / sizeof( COMPLEXDOUBLE )); i++ )
//      {
//      if( creal(CompPtr[ i ]) != SourceRank ||
//          cimag(CompPtr[ i ]) != (-1.0*SourceRank) || error )
//        {
//        error=1;
//        printf("s: %d C[ %d ].re: %f C[ %d ].im: %f tr: %d o: %d b: %d c: %d line: %d\n",
//               SourceRank,
//               i,
//               creal(CompPtr[ i ]),
//               i,
//               cimag(CompPtr[ i ]),
//               TotalCountToRecv,
//               Offset,
//               BytesInPayloadTemp,
//               chunks,
//               __LINE__ );
//
//        // assert( 0 );
//        }
//      }
//    assert( !error );
//    }
//#endif

  return( chunks );
  }


template< int NON_ALIGNED1 >
static inline
int
_ts_Poll0_OnePassReceive_Alltoallv_Chunk_Put( _BGL_TorusFifoStatus *statF0,
                                                               PacketAlltoAllv* aThis )
  {
  int TotalPackets = 0;
  
  fStatus.recordReceive(statF0->r[0]) ;

  if ( statF0->r[0] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis );
    TotalPackets++;
    statF0->r[0] -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r[1]) ;
  if ( statF0->r[1] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R1, aThis );
    TotalPackets++;
    statF0->r[1] -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r[2]) ;
  if ( statF0->r[2] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );
    TotalPackets++;
    statF0->r[2] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );

  fStatus.recordReceive(statF0->r[3]) ;
  if ( statF0->r[3] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );
    TotalPackets++;
    statF0->r[3] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );

  fStatus.recordReceive(statF0->r[4]) ;
  if ( statF0->r[4] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );
    TotalPackets++;
    statF0->r[4] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );

  fStatus.recordReceive(statF0->r[5]) ;
  if ( statF0->r[5] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );
    TotalPackets++;
    statF0->r[5] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );

  return TotalPackets;
  }

template< int NON_ALIGNED1 >  
static inline
int
_ts_Poll0_OnePassReceive_Alltoallv_Drain( _BGL_TorusFifoStatus *statF0,
                                                               PacketAlltoAllv* aThis )
  {
  int TotalPackets = 0;

  int r0 = statF0->r[0] ;
//  statF0->r[0] = 0 ;
//  statF0->r[1] = 0 ;
//  statF0->r[2] = 0 ;
//  statF0->r[3] = 0 ;
//  statF0->r[4] = 0 ;
//  statF0->r[5] = 0 ;
  if ( r0 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis, r0 ) ;
  } 
  int r1 = statF0->r[1] ;
  if ( r1 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R1, aThis, r1 ) ;
  } 
  int r2 = statF0->r[2] ;
  if ( r2 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R2, aThis, r2 ) ;
  } 
  int r3 = statF0->r[3] ;
  if ( r3 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R3, aThis, r3 ) ;
  } 
  int r4 = statF0->r[4] ;
  if ( r4 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R4, aThis, r4 ) ;
  } 
  int r5 = statF0->r[5] ;
  if ( r5 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R5, aThis, r5 ) ;
  } 

  return TotalPackets;
  }

template< int NON_ALIGNED1 >
static inline
void
_ts_Poll0_OnePassReceive_Alltoallv_Put( _BGL_TorusFifoStatus *statF0,
                                                            PacketAlltoAllv* aThis )
  {
  fStatus.recordReceive(statF0->r[0]) ;
  	
  if ( statF0->r[0] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis );
    statF0->r[0] -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r[1]) ;
  if ( statF0->r[1] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R1, aThis );
    statF0->r[1] -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r[2]) ;
  if ( statF0->r[2] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );
    statF0->r[2] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );

  fStatus.recordReceive(statF0->r[3]) ;
  if ( statF0->r[3] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );
    statF0->r[3] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );

  fStatus.recordReceive(statF0->r[4]) ;
  if ( statF0->r[4] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );
    statF0->r[4] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );

  fStatus.recordReceive(statF0->r[5]) ;
  if ( statF0->r[5] )
    {
    int chunks = _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );
    statF0->r[5] -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );

  return;
  }

template< int NON_ALIGNED1 >
static inline
void
_ts_Poll1_OnePassReceive_Alltoallv_Put( _BGL_TorusFifoStatus *statF1,
                                                         PacketAlltoAllv* aThis )
  {
  fStatus.recordReceive(statF1->r[0]) ;
  if ( statF1->r[0] )
    _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R0, aThis );

  fStatus.recordReceive(statF1->r[1]) ;
  if ( statF1->r[1] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R1, aThis );

  fStatus.recordReceive(statF1->r[2]) ;
  if ( statF1->r[2] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R2, aThis );

  fStatus.recordReceive(statF1->r[3]) ;
  if ( statF1->r[3] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R3, aThis );

  fStatus.recordReceive(statF1->r[4]) ;
  if ( statF1->r[4] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R4, aThis );

  fStatus.recordReceive(statF1->r[5]) ;
  if ( statF1->r[5] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R5, aThis );

  return;
  }

template< int NON_ALIGNED1 >
static inline
int
_ts_Poll1_OnePassReceive_Alltoallv_Drain( _BGL_TorusFifoStatus *statF1,
                                                               PacketAlltoAllv* aThis )
  {
  int TotalPackets = 0;

  int r0 = statF1->r[0] ;
//  statF1->r[0] = 0 ;
//  statF1->r[1] = 0 ;
//  statF1->r[2] = 0 ;
//  statF1->r[3] = 0 ;
//  statF1->r[4] = 0 ;
//  statF1->r[5] = 0 ;
  if ( r0 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R0, aThis, r0 ) ;
  } 
  int r1 = statF1->r[1] ;
  if ( r1 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R1, aThis, r1 ) ;
  } 
  int r2 = statF1->r[2] ;
  if ( r2 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R2, aThis, r2 ) ;
  } 
  int r3 = statF1->r[3] ;
  if ( r3 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R3, aThis, r3 ) ;
  } 
  int r4 = statF1->r[4] ;
  if ( r4 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R4, aThis, r4 ) ;
  } 
  int r5 = statF1->r[5] ;
  if ( r5 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R5, aThis, r5 ) ;
  } 

  return TotalPackets;
  }
template< int NON_ALIGNED1 >
static inline
void
_ts_PollBoth_OnePassReceive_Alltoallv_Put( _BGL_TorusFifoStatus *statF0,
                                                            _BGL_TorusFifoStatus *statF1,
                                                            PacketAlltoAllv* aThis )
  {
  fStatus.recordReceive(statF0->r[0]) ;
  if ( statF0->r[0] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R0, aThis );

  fStatus.recordReceive(statF0->r[1]) ;
  if ( statF0->r[1] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R1, aThis );

  fStatus.recordReceive(statF0->r[2]) ;
  if ( statF0->r[2] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );

  fStatus.recordReceive(statF0->r[3]) ;
  if ( statF0->r[3] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );

  fStatus.recordReceive(statF0->r[4]) ;
  if ( statF0->r[4] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );

  fStatus.recordReceive(statF0->r[5]) ;
  if ( statF0->r[5] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );

  fStatus.recordReceive(statF1->r[0]) ;
  if ( statF1->r[0] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R0, aThis );

  fStatus.recordReceive(statF1->r[1]) ;
  if ( statF1->r[1] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R1, aThis );

  fStatus.recordReceive(statF1->r[2]) ;
  if ( statF1->r[2] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R2, aThis );

  fStatus.recordReceive(statF1->r[3]) ;
  if ( statF1->r[3] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R3, aThis );

  fStatus.recordReceive(statF1->r[4]) ;
  if ( statF1->r[4] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R4, aThis );

  fStatus.recordReceive(statF1->r[5]) ;
  if ( statF1->r[5] )
      _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1  >( (_BGL_TorusFifo *)TS1_R5, aThis );

  return;
  }

template< int NON_ALIGNED1 >
static inline
int
_ts_PollBoth_OnePassReceive_Alltoallv_Drain(_BGL_TorusFifoStatus *statF0,
                                                            _BGL_TorusFifoStatus *statF1,
                                                            PacketAlltoAllv* aThis )
{
	return 	_ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( statF0, aThis )
	      + _ts_Poll1_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( statF1, aThis );
}            
                                                
void PacketAlltoAllv::PermuteActiveSendRanks( int* aRanks, int aSize )
  {
  char state[256];
  char *is;
  is = initstate(365, (char *)&state, sizeof(state));
  for( int i=0; i<aSize; i++ )
    {
    int index = random() % aSize;
    assert( index >= 0 && index < aSize );

    int temp = aRanks[ index ];
    aRanks[ index ] = aRanks[ i ];
    aRanks[ i ] = temp;
    }
  setstate(is);
  }

int PacketAlltoAllv::GetNextRankToSend()
  {
  if( mActiveSendRanksCount == 0 )
    return SEND_DONE;

  assert( *mCurrentRankPtr >= 0 && *mCurrentRankPtr < mGroupSize );

  if( mSendOffsets[ *mCurrentRankPtr ] < (mSendCount[ *mCurrentRankPtr ] * mSendTypeSize) )
    {
    // There's more
    if( mCurrentInject < mPktInject )
      {
      mCurrentInject++;
      return *mCurrentRankPtr;
      }

    if( mCurrentRankPtr == mEndRankPtr )
      {
      mCurrentRankPtr = mActiveSendRanksTransient;
      }
    else
      {
      mCurrentRankPtr++;
      }
    }

  mCurrentInject = 0;

// Iterate till the next incompletely sent message
  while( mSendOffsets[ *mCurrentRankPtr ] >= (mSendCount[ *mCurrentRankPtr ] * mSendTypeSize) )
    {
    if(mCurrentRankPtr == mEndRankPtr && mCurrentRankPtr == mActiveSendRanksTransient )
        return SEND_DONE;

    *mCurrentRankPtr = *mEndRankPtr;

    if( mCurrentRankPtr == mEndRankPtr )
      {
      mCurrentRankPtr = mActiveSendRanksTransient;
      }
    mEndRankPtr--;
    }

  assert( *mCurrentRankPtr < mGroupSize );
  return *mCurrentRankPtr;
  }

struct PacketAlltoAllvInitActorArgs
   {
   PacketAlltoAllv* mThis;
   int * mSendCount;
   int * mSendOffsets;
   int   mSendTypeSize;
   int * mRecvCount;
   int * mRecvOffsets;
   int   mRecvTypeSize;
   GroupContainer* mGroup;
   int   mDebug;
   };

void* PacketAlltoAllv::InitActor( void* arg )
{
  PacketAlltoAllvInitActorArgs* Args =   (PacketAlltoAllvInitActorArgs*) arg;

  // printf("Got here %d\n", __LINE__);

  PacketAlltoAllv* ThisPtr = Args->mThis;
  int* SendCount    = Args->mSendCount;
  int* RecvCount    = Args->mRecvCount;
  int* SendOffset   = Args->mSendOffsets;
  int* RecvOffset   = Args->mRecvOffsets;
  int  SendTypeSize = Args->mSendTypeSize;
  int  RecvTypeSize = Args->mRecvTypeSize;
  GroupContainer* Group = Args->mGroup;

  BegLogLine(PKFXLOG_ALLTOALLV)
    << "PacketAlltoAllv::InitActor sizeof( BGL_AlltoallvPacketMetaData )= " << sizeof( BGL_AlltoallvPacketMetaData )
    << EndLogLine ;

  alltoallv_debug    = Args->mDebug;

 // printf( "Group: %08x line: \n", Group, __LINE__ );

//  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
//      printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
//           sizeof( BGL_AlltoallvPacketMetaData ) );

  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );

  ThisPtr->mPacketsToInject = PACKETS_TO_INJECT;
  ThisPtr->mGroup     = Group;
  ThisPtr->mMyRank    = ThisPtr->mGroup->mMyRank;  // BGLPartitionGetRank();
  ThisPtr->mRecvDoneBits = 0 ;
  // printf( "ThisPtr->mMyRank: %d\n", ThisPtr->mMyRank );

  ThisPtr->mGroupSize = ThisPtr->mGroup->mSize;
  ThisPtr->mGroupElements = ThisPtr->mGroup->mElements;

  BegLogLine(PKFXLOG_ALLTOALLV)
    << "ThisPtr->mMyRank=" << ThisPtr->mMyRank
    << " ThisPtr->mGroupSize=" << ThisPtr->mGroupSize
    << EndLogLine ;
//  if( !( ThisPtr->mMyRank >= 0 && ThisPtr->mMyRank < ThisPtr->mGroupSize ) )
//    {
//    printf("%d %d\n", ThisPtr->mMyRank, ThisPtr->mGroupSize );
//    }

  assert( ThisPtr->mMyRank >= 0 && ThisPtr->mMyRank < ThisPtr->mGroupSize );

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

#ifdef PK_PHASE5_SPLIT_COMMS
  ThisPtr->mMyCore    = 1;
  ThisPtr->mNumberOfFifosInUse = 3;
#else
  ThisPtr->mNumberOfFifosInUse = 6;
  ThisPtr->mMyCore    = 0;
#endif

  ThisPtr->mMyCoord   = ThisPtr->mGroupElements[ ThisPtr->mMyRank ].mCoord;

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );
  // mSendBuff      = aSendBuf;
  ThisPtr->mSendCount     = SendCount;
  ThisPtr->mSendOffsetIn  = SendOffset;
  ThisPtr->mSendTypeSize  = SendTypeSize;

  // mRecvBuff     = aRecvBuf;
  ThisPtr->mRecvCount    = RecvCount;
  ThisPtr->mRecvOffset   = RecvOffset;
  ThisPtr->mRecvTypeSize = RecvTypeSize;

  ThisPtr->mBytesRecvFromNode = (int *) malloc( ThisPtr->mGroupSize * sizeof(int) + CACHE_ISOLATION_PADDING_SIZE);
  assert( ThisPtr->mBytesRecvFromNode );

  ThisPtr->mSendOffsets = (int *) malloc( ThisPtr->mGroupSize * sizeof(int) + CACHE_ISOLATION_PADDING_SIZE);
  assert( ThisPtr->mSendOffsets );

  ThisPtr->mActiveSendRanks = (int *) malloc( ThisPtr->mGroupSize * sizeof(int) + CACHE_ISOLATION_PADDING_SIZE);
  assert( ThisPtr->mActiveSendRanks );
  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

  if( DIAGNOSE_PACKET_ALLTOALLV )
    {
  	// So we can check later if we ever get a packet from a node that should not be sending to us
	int GroupSize=ThisPtr->mGroupSize ;
  	for( int i=0;i<GroupSize;i+=1)
  	   {
  		  ThisPtr->mBytesRecvFromNode[ i ] = -1 ;
  	   }
    }

// We never send to self.
  ThisPtr->mRecvNonActiveCount = 1;

  ThisPtr->mActiveSendRanksCount = 0;

  int MaxNumberHeaders = 0;

//   if( alltoallv_debug && ThisPtr->mMyRank == 194 )
//     {
//     printf("ThisPtr->GroupSize: %d d: %d mr: %d\n",
//            ThisPtr->mGroupSize,
//            alltoallv_debug,
//            ThisPtr->mMyRank );
//     }

  for( int i=0; i < ThisPtr->mGroupSize; i++ )
    {
//     if( alltoallv_debug && ThisPtr->mMyRank == 194 )
//       {
//       printf("ThisPtr->mSendCount[ %d ]: %d ThisPtr->mRecvCount[ %d ]: %d\n",
//              i, ThisPtr->mSendCount[ i ],
//              i, ThisPtr->mRecvCount[ i ] );
//       }

    if( ThisPtr->mSendCount[ i ] != 0 && i != ThisPtr->mMyRank )
      {
      int NumberOfHeaders = (int) ceil( (1.0*( ThisPtr->mSendCount[ i ] * ThisPtr->mSendTypeSize)) / (_BGL_TORUS_PKT_MAX_PAYLOAD*1.0) );

      if( NumberOfHeaders > MaxNumberHeaders )
          MaxNumberHeaders = NumberOfHeaders;

      ThisPtr->mActiveSendRanks[ ThisPtr->mActiveSendRanksCount ] = i;
      ThisPtr->mActiveSendRanksCount++;
      }

    if( ThisPtr->mRecvCount[ i ] == 0 && i != ThisPtr->mMyRank )
      {
      // Self sends have been accounted
      ThisPtr->mRecvNonActiveCount++;
      }
    }

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

#if 0
  PermuteActiveSendRanks( mActiveSendRanks, mActiveSendRanksCount );

// Transient gets filled in SendMessages
  mActiveSendRanksTransient = (int *) malloc( sizeof(int) * mActiveSendRanksCount );
  assert( mActiveSendRanksTransient );

  mPktInject = 2;
  mCurrentInject = 0;
#endif

  int xDim, yDim, zDim;
  BGLPartitionGetDimensions( &xDim, &yDim, &zDim );

  ThisPtr->mPeriod.Set( xDim , yDim, zDim );

  ThisPtr->mHeadersForLinkAllocatedCount = MaxNumberHeaders * ThisPtr->mActiveSendRanksCount;

  int PresentLinks[ NUMBER_OF_LINKS ];
  for(int i=0; i<NUMBER_OF_LINKS; i++ )
    {
    PresentLinks[ i ] = 0;
    }

  for( int i=0; i < ThisPtr->mActiveSendRanksCount; i++ )
    {
    int Rank = ThisPtr->mActiveSendRanks[ i ];
    TorusXYZ DestCoord = ThisPtr->mGroupElements[ Rank ].mCoord;

    // Create headers for this rank
    TorusXYZ LinkXYZ = GetRootBoundTreeLink( ThisPtr->mPeriod, DestCoord, ThisPtr->mMyCoord );
    int      Link    = GetLinkNumber( LinkXYZ );
    PresentLinks[ Link ]++;
    }

  for( int i=0; i<NUMBER_OF_LINKS; i++ )
    {
    ThisPtr->mHeadersForLinkCount[ i ] = 0;
    if( PresentLinks[ i ] )
      {
      ThisPtr->mHeadersForLink[ i ] =
          (BGLAlltoallvHeaders *) malloc( sizeof( BGLAlltoallvHeaders ) * ThisPtr->mHeadersForLinkAllocatedCount + CACHE_ISOLATION_PADDING_SIZE );

      BegLogLine(PKFXLOG_ALLTOALLV && 0 == BGLPartitionGetRank())
        << "Allocate mHeadersForLink[ " << i 
        << " ]: size: " << sizeof( BGLAlltoallvHeaders ) * ThisPtr->mHeadersForLinkAllocatedCount
        << EndLogLine ; 
//      if( BGLPartitionGetRank() == 0 )
//        {
//        printf( "Allocate mHeadersForLink[ %d ]: size: %d\n", i, sizeof( BGLAlltoallvHeaders ) * ThisPtr->mHeadersForLinkAllocatedCount );
//        }

      assert( ThisPtr->mHeadersForLink[ i ] );
      }
    else
      {
      ThisPtr->mHeadersForLink[ i ] = NULL;
      }
    }
  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

  int FifoPicker = 0;

  // Create 6 lists of _BGL_TorusPktHdrs based on link to send
  for( int i=0; i < ThisPtr->mActiveSendRanksCount; i++ )
    {
    int Rank = ThisPtr->mActiveSendRanks[ i ];
    TorusXYZ DestCoord = ThisPtr->mGroupElements[ Rank ].mCoord;

    // Create headers for this rank
    TorusXYZ LinkXYZ = GetRootBoundTreeLink( ThisPtr->mPeriod, DestCoord, ThisPtr->mMyCoord );
    int      Link    = GetLinkNumber( LinkXYZ );

    assert( Link >=0 && Link < 6 );

    int TotalBytesToSend = ThisPtr->mSendCount[ Rank ] * ThisPtr->mSendTypeSize;

    int BytesSent = 0;

    while( BytesSent < TotalBytesToSend )
      {
      int BytesToSend = 0;
      if( ( BytesSent + _BGL_TORUS_PKT_MAX_PAYLOAD ) <=  TotalBytesToSend )
        {
        BytesToSend = _BGL_TORUS_PKT_MAX_PAYLOAD;
        }
      else
        {
        // Last packet might not be full
        BytesToSend = ( TotalBytesToSend - BytesSent );
        }

      assert( BytesToSend );

//           if( !( mHeadersForLinkCount[ Link ] >= 0 && mHeadersForLinkCount[ Link ] < mHeadersForLinkAllocatedCount ) )
//             {
//             if( mMyRank == 0 )
//               {
//               printf("mHeadersForLinkCount[ %d ]: %d mHeadersForLinkAllocatedCount: %d\n",
//                      Link,
//                      mHeadersForLinkCount[ Link ],
//                      mHeadersForLinkAllocatedCount );
//               }
//             }

      assert( ThisPtr->mHeadersForLinkCount[ Link ] >= 0 && ThisPtr->mHeadersForLinkCount[ Link ] < ThisPtr->mHeadersForLinkAllocatedCount );

      BGLAlltoallvHeaders* BglHdr = &ThisPtr->mHeadersForLink[ Link ][ ThisPtr->mHeadersForLinkCount[ Link ] ];

      _BGL_TorusPktHdr* HdrPtr = &(BglHdr->mHdr);

      BGLTorusMakeHdrAppChooseRecvFifo( HdrPtr,
                                        DestCoord.mX,
                                        DestCoord.mY,
                                        DestCoord.mZ,
                                        0,
                                        0,
                                        BytesToSend,
#ifdef PK_PHASE5_SPLIT_COMMS
                                        1 ); // fft on core 1
#else
                                        ((FifoPicker++) & 0x1 ) );
#endif


      BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &HdrPtr->swh0;

      assert( ThisPtr->mMyRank >= 0            && ThisPtr->mMyRank < PACKET_ALLTOALLV_LIMIT_RANK );
      assert( ThisPtr->mSendCount[ Rank ] >= 0 && ThisPtr->mSendCount[ Rank ] < PACKET_ALLTOALLV_LIMIT_COUNT );
      assert( BytesSent >= 0                   && BytesSent < PACKET_ALLTOALLV_LIMIT_OFFSET );

      MetaData->mSourceRank          = ThisPtr->mMyRank;
      MetaData->mTotalCount          = ThisPtr->mSendCount[ Rank ];
      MetaData->mOffset              = BytesSent;
      MetaData->mBytesInPayload      = BytesToSend;

      BglHdr->mDestRank              = Rank;
      assert( BglHdr->mDestRank != ThisPtr->mMyRank );

      ThisPtr->mHeadersForLinkCount[ Link ]++;
      BytesSent += BytesToSend;

#ifdef DEBUG_ALL2ALL
      if( alltoallv_debug && ThisPtr->mMyRank == 194 )
        printf("rank: %d link: %d TotalBytesToSend: %d BytesSent: %d mHeadersForLinkCount[ Link ]: %d\n",
               Rank,
               Link,
               TotalBytesToSend,
               BytesSent,
               ThisPtr->mHeadersForLinkCount[ Link ] );
#endif
      // mSendOffsets[ DestRank ] += BytesToSend;
      }
    }
  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

// 6 Link lists are ready, cull them
  ThisPtr->mActiveLinkCount = 0;
  for( int i=0; i<NUMBER_OF_LINKS; i++ )
    {
    if( ThisPtr->mHeadersForLinkCount[ i ] != 0 )
      {
      ThisPtr->mActiveLink[ ThisPtr->mActiveLinkCount++ ] = i;
      }
    }
  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

// Randomize Links
  RandomizeLinks( ThisPtr->mActiveLink, ThisPtr->mActiveLinkCount, ThisPtr->mMyRank );

// We have 2 strategies for ordering within the list
// 1. Randomize
// 2. Farthest packets leave first

  for( int i=0; i<ThisPtr->mActiveLinkCount; i++ )
    {
    int Link = ThisPtr->mActiveLink[ i ];

    RandomizeHeaders( ThisPtr->mHeadersForLink[ Link ], ThisPtr->mHeadersForLinkCount[ Link ] );
    }

//   if( ThisPtr->mMyCore == 0 )
//     {
//     ThisPtr->mFifoStats[ 0 ].mLink = (_BGL_TorusFifo *) TS0_I0;
//     ThisPtr->mFifoStats[ 1 ].mLink = (_BGL_TorusFifo *) TS0_I1;
//     ThisPtr->mFifoStats[ 2 ].mLink = (_BGL_TorusFifo *) TS0_I2;
//     if( ThisPtr->mNumberOfFifosInUse == 6 )
//       {
//       ThisPtr->mFifoStats[ 3 ].mLink = (_BGL_TorusFifo *) TS1_I0;
//       ThisPtr->mFifoStats[ 4 ].mLink = (_BGL_TorusFifo *) TS1_I1;
//       ThisPtr->mFifoStats[ 5 ].mLink = (_BGL_TorusFifo *) TS1_I2;
//       }
//     }
//   else
//     {
//     ThisPtr->mFifoStats[ 0 ].mLink = (_BGL_TorusFifo *) TS1_I0;
//     ThisPtr->mFifoStats[ 1 ].mLink = (_BGL_TorusFifo *) TS1_I1;
//     ThisPtr->mFifoStats[ 2 ].mLink = (_BGL_TorusFifo *) TS1_I2;
//     if( ThisPtr->mNumberOfFifosInUse == 6 )
//       {
//       ThisPtr->mFifoStats[ 3 ].mLink = (_BGL_TorusFifo *) TS0_I0;
//       ThisPtr->mFifoStats[ 4 ].mLink = (_BGL_TorusFifo *) TS0_I1;
//       ThisPtr->mFifoStats[ 5 ].mLink = (_BGL_TorusFifo *) TS0_I2;
//       }
//     }

#ifdef DEBUG_ALL2ALL
  if( Verbose )
      printf("Core(%d): Got here: %d\n",rts_get_processor_id(), __LINE__ );
#endif
  return NULL;
  }

void PacketAlltoAllv::InitSingleCore( int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                      int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                      GroupContainer* aGroup )
  {
  PacketAlltoAllvInitActorArgs Core0Args;

  Core0Args.mThis        = this;
  Core0Args.mSendCount   = aSendCount;
  Core0Args.mRecvCount   = aRecvCount;
  Core0Args.mSendOffsets = aSendOffset;
  Core0Args.mRecvOffsets = aRecvOffset;
  Core0Args.mSendTypeSize = aSendTypeSize;
  Core0Args.mRecvTypeSize = aRecvTypeSize;
  Core0Args.mGroup       = aGroup;
  Core0Args.mDebug       = alltoallv_debug;

  InitActor( &Core0Args );
  }

extern unsigned SimTick;

void PacketAlltoAllv::InitSingleCoreSimple( int aSendCount[], int* aSendOffset, int aSendTypeSize,
                                            int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                                            GroupContainer* aGroup,
                                            int* aSendNodeList, int aSendNodeListCount,
                                            int* aRecvNodeList, int aRecvNodeListCount )
  {
  AlltoallvInitStart.HitOE( PKTRACE_ALLTOALLV_INIT,
                           "AlltoallvInitStart",
                            Platform::Topology::GetAddressSpaceId(),
                           AlltoallvInitStart );

  // alltoallv_debug    = 0;

  BegLogLine(PKFXLOG_ALLTOALLV)
    << "sizeof( BGL_AlltoallvPacketMetaData )=" << sizeof( BGL_AlltoallvPacketMetaData )
    << EndLogLine ;
//  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
//    printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
//           sizeof( BGL_AlltoallvPacketMetaData ) );

  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );

  mPacketsToInject = PACKETS_TO_INJECT;
  mGroup     = aGroup;
  mMyRank    = mGroup->mMyRank;

  mGroupSize = mGroup->mSize;
  mGroupElements = mGroup->mElements;
  mRecvDoneBits = 0 ; 
  BegLogLine(PKFXLOG_ALLTOALLV)
    << "mMyRank=" << mMyRank
    << " mGroupSize=" << mGroupSize
    << EndLogLine ;
//  if( !( mMyRank >= 0 && mMyRank < mGroupSize ) )
//    {
//    printf("%d %d\n", mMyRank, mGroupSize );
//    }

  assert( mMyRank >= 0 && mMyRank < mGroupSize );

  mMyCoord   = mGroupElements[ mMyRank ].mCoord;

  mSendCount     = aSendCount;
  mSendOffsetIn  = aSendOffset;
  mSendTypeSize  = aSendTypeSize;

//   if( alltoallv_debug && ( mMyRank == 14 ) )
//     {
//     printf( "SCOUNT: %d\n", mSendCount[ 0 ] );
//     }

  mRecvCount    = aRecvCount;
  mRecvOffset   = aRecvOffset;
  mRecvTypeSize = aRecvTypeSize;

  if( mFirstEntry )
    {
    mBytesRecvFromNode = (int *) malloc( mGroupSize * sizeof(int)+ CACHE_ISOLATION_PADDING_SIZE);
    assert( mBytesRecvFromNode );

    mSendOffsets = (int *) malloc( mGroupSize * sizeof(int)+ CACHE_ISOLATION_PADDING_SIZE);
    assert( mSendOffsets );

    mActiveSendRanks = (int *) malloc( mGroupSize * sizeof(int)+ CACHE_ISOLATION_PADDING_SIZE);
    assert( mActiveSendRanks );

    mActiveSendRanksTransient = (int *) malloc( sizeof(int) * mGroupSize + CACHE_ISOLATION_PADDING_SIZE);
    assert( mActiveSendRanksTransient );

    BegLogLine(PKFXLOG_ALLTOALLV) 
      << "Done allocating simple alltoallv members"
      << EndLogLine ;
//    printf( "Done allocating simple alltoallv members\n" );
//    fflush( stdout );

    mFirstEntry = 0;
    }

// We never send to self.
  mRecvNonActiveCount = 1;

  mActiveSendRanksCount = 0;

  int MaxNumberHeaders = 0;

  AlltoallvShellStart.HitOE( PKTRACE_ALLTOALLV_INIT,
                             "AlltoallvShellStart",
                             Platform::Topology::GetAddressSpaceId(),
                             AlltoallvShellStart );

//mActiveSendRanks[ mActiveSendRanksCount ] = i;
//mActiveSendRanksCount++;

// mActiveSendRanksCount = aSendNodeListCount;
//BegLogLine( 0  )
//    << SimTick
//    << " aSendNodeListCount: " << aSendNodeListCount
//    << " aRecvNodeListCount: " << aRecvNodeListCount
//    << " "
//    << EndLogLine;

if( DIAGNOSE_PACKET_ALLTOALLV )
  {
	// So we can check later if we ever get a packet from a node that should not be sending to us
	for( int i=0;i<mGroupSize;i+=1)
	   {
		  mBytesRecvFromNode[ i ] = -1 ;
	   }
  }

if(!( (aSendNodeList == NULL ) && ( aSendNodeListCount == 0  ) && ( aRecvNodeListCount == 0 ) ))
///if( 0 )
    {
    for( int i=0; i < aSendNodeListCount; i++ )
      {
      if( aSendNodeList[ i ] != mMyRank )
        {
	        mActiveSendRanks[ mActiveSendRanksCount ] = aSendNodeList[ i ];
	        BegLogLine(PKFXLOG_ALLTOALLV)
	          << "InitSingleCoreSimple mActiveSendRanks[" << mActiveSendRanksCount
	          << "]=" << mActiveSendRanks[ mActiveSendRanksCount ]
	          << EndLogLine ;
	        mActiveSendRanksCount++;
        }
      }

    // Loop to see if MyRank is in the list
    int IsMyRankOnList = 0;
    for( int i=0; i < aRecvNodeListCount; i++ )
      {
      int nodeId = aRecvNodeList[ i ];
      mBytesRecvFromNode[ nodeId ] = 0;
      BegLogLine(PKFXLOG_ALLTOALLV)
        << "InitSingleCoreSimple zeroing mBytesRecvFromNode[" << nodeId
        << "]"
        << EndLogLine ;
      
      if( nodeId == mMyRank )
        {
        IsMyRankOnList=1;
        // break;
        }
      }

    //
    mRecvNonActiveCount = 0;
    mRecvNonActiveCount += ( (IsMyRankOnList) ?
                             (mGroupSize - (aRecvNodeListCount - 1)): (mGroupSize - aRecvNodeListCount) );

#if 0
    int TempmRecvNonActiveCount = 1;
    int  TempmActiveSendRanksCount = 0;
    int* TempList = (int *) malloc( sizeof(int) * mGroupSize );

    for( int i=0; i < mGroupSize; i++ )
      {
      mBytesRecvFromNode[ i ] = 0;

      if( mSendCount[ i ] != 0 && i != mMyRank )
        {
        TempList[ TempmActiveSendRanksCount ] = i;
        TempmActiveSendRanksCount++;
        }

      if( mRecvCount[ i ] == 0 && i != mMyRank )
        {
        // Self sends have been accounted
        TempmRecvNonActiveCount++;
        }
      }

    if( TempmRecvNonActiveCount != mRecvNonActiveCount )
      {
      BegLogLine( 1 )
        << "TempmRecvNonActiveCount: " << TempmRecvNonActiveCount
        << " mRecvNonActiveCount: " << mRecvNonActiveCount
        << EndLogLine;
      }

    if( TempmActiveSendRanksCount != mActiveSendRanksCount )
      {
      BegLogLine( 1 )
        << "TempmActiveSendRanksCount: " << TempmActiveSendRanksCount
        << " mActiveSendRanksCount: " << mActiveSendRanksCount
        << EndLogLine;
      }

    for( int i=0; i < mActiveSendRanksCount; i++ )
      {
      if( TempList[i] != mActiveSendRanks[ i ] )
        {
        BegLogLine( 1 )
          << "TempList[" << i << "]: " << TempList[i]
          << "mActiveSendRanks[" << i << "]: " << mActiveSendRanks[i]
          << EndLogLine;
        break;
        }
      }
#endif
    }
  else
    {
    for( int i=0; i < mGroupSize; i++ )
      {
      mBytesRecvFromNode[ i ] = 0;
      BegLogLine(PKFXLOG_ALLTOALLV)
        << "InitSingleCoreSimple zeroing mBytesRecvFromNode[" << i
        << "]"
        << EndLogLine ;

      if( mSendCount[ i ] != 0 && i != mMyRank )
        {
        mActiveSendRanks[ mActiveSendRanksCount ] = i;
        mActiveSendRanksCount++;
        }

      if( mRecvCount[ i ] == 0 && i != mMyRank )
        {
        // Self sends have been accounted
        mRecvNonActiveCount++;
        }
      }
    }

#if 0
  AlltoallvPermRanksStart.HitOE( PKTRACE_ALLTOALLV_INIT,
                           "AlltoallvPermRanksStart",
                            Platform::Topology::GetAddressSpaceId(),
                           AlltoallvPermRanksStart );

 PermuteActiveSendRanks( mActiveSendRanks, mActiveSendRanksCount );

  AlltoallvPermRanksFinis.HitOE( PKTRACE_ALLTOALLV_INIT,
                                 "AlltoallvPermRanksFinis",
                                 Platform::Topology::GetAddressSpaceId(),
                                 AlltoallvPermRanksFinis );

#endif
// if( mMyRank == 0 )
// {
//   printf("S: %d AC: %d\n", SimTick, mActiveSendRanksCount);
//   fflush(stdout);
// }

  mPktInject = 2;
  mCurrentInject = 0;

#ifdef PK_PHASE5_SPLIT_COMMS
  mMyCore             = rts_get_processor_id();
  mNumberOfFifosInUse = 3;
#else
  mMyCore             = 0;
  mNumberOfFifosInUse = 6;
#endif
  }

struct FinalizeActorArg
  {
  PacketAlltoAllv* mThis;
  };

void* PacketAlltoAllv::FinalizeActor( void* arg )
  {
  FinalizeActorArg* Args =   (FinalizeActorArg *) arg;

  PacketAlltoAllv* ThisPtr = Args->mThis;

  if( ThisPtr->mBytesRecvFromNode != NULL )
    {
    free( ThisPtr->mBytesRecvFromNode );
    ThisPtr->mBytesRecvFromNode = NULL;
    }

  if( ThisPtr->mSendOffsets != NULL )
    {
    free( ThisPtr->mSendOffsets );
    ThisPtr->mSendOffsets = NULL;
    }

  if( ThisPtr->mActiveSendRanks != NULL )
    {
    free( ThisPtr->mActiveSendRanks );
    ThisPtr->mActiveSendRanks = NULL;
    }

  for( int i=0; i<NUMBER_OF_LINKS; i++ )
    {
    if( ThisPtr->mHeadersForLink[ i ] != NULL )
      {
      free( ThisPtr->mHeadersForLink[ i ] );
      ThisPtr->mHeadersForLink[ i ] = NULL;
      }
    }

  return NULL;
  }

#if 0
void PacketAlltoAllv::Finalize()
  {
  FinalizeActorArg Core0Args;
  Core0Args.mThis        = this;

  FinalizeActor( &Core0Args );
  }
#endif

void PacketAlltoAllv::FinalizeSingleCore()
  {
  FinalizeActorArg         Core0Args;
  Core0Args.mThis        = this;
  FinalizeActor( &Core0Args );
  }

void PacketAlltoAllv::FinalizeSingleCoreSimple()
  {
  if( mBytesRecvFromNode != NULL )
    {
    free( mBytesRecvFromNode );
    mBytesRecvFromNode = NULL;
    }

  if( mSendOffsets != NULL )
    {
    free( mSendOffsets );
    mSendOffsets = NULL;
    }

  if( mActiveSendRanks != NULL )
    {
    free( mActiveSendRanks );
    mActiveSendRanks = NULL;
    }

  if( mActiveSendRanksTransient != NULL )
    {
        //free( mActiveSendRanksTransient );
    mActiveSendRanksTransient = NULL;
    }
  }

template< int NON_ALIGNED1 >
void PacketAlltoAllv::ExecuteSingleCoreSimple1( void* aSendBuff, void* aRecvBuff )
  {
  	BegLogLine(PKFXLOG_ALLTOALLV) 
  	  << "PacketAlltoAllv::ExecuteSingleCoreSimple1 mRecvNonActiveCount=" << mRecvNonActiveCount
  	  << " mActiveSendRanksCount=" << mActiveSendRanksCount
  	  << " mGroupSize=" << mGroupSize
  	  << EndLogLine ;
    AlltoallvExecuteSimpleStart.HitOE( PKTRACE_ALLTOALLV_EXECUTE,
        "AlltoallvExecuteSimpleStart",
        Platform::Topology::GetAddressSpaceId(),
        AlltoallvExecuteSimpleStart );
    

  AlltoallvSimpleExecBarrierStart.HitOE( PKTRACE_ALLTOALLV_INIT,
      "AlltoallvSimpleExecBarrierStart",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvSimpleExecBarrierStart );

  BGLPartitionBarrierCore();

  AlltoallvSimpleExecBarrierFinis.HitOE( PKTRACE_ALLTOALLV_INIT,
      "AlltoallvSimpleExecBarrierFinis",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvSimpleExecBarrierFinis );

  mSendBuff = aSendBuff;
  mRecvBuff = aRecvBuff;

  // memcpy data local to this node.
  if( mSendCount[ mMyRank ] )
    {
    char * Src = (char*)mSendBuff + (mSendOffsetIn[ mMyRank ] * mSendTypeSize);
    char * Tgt = (char*)mRecvBuff + (mRecvOffset[ mMyRank ] * mRecvTypeSize);
    int Size   = mSendCount[ mMyRank ] * mSendTypeSize;
    memcpy_double( (double *) Tgt, (double *) Src, Size );
    }
  mRecvCount[ mMyRank ] = mSendCount[ mMyRank ];

//   if( alltoallv_debug && ( mMyRank == 14 ) )
//     {
//     printf( "SCOUNT: %d\n", mSendCount[ 0 ] );
//     }

  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;
  _BGL_TorusFifoStatus FifoStatus;

  mTotalSentPackets = 0;
  mTotalRecvPackets = 0;
  mRecvCountDone = mRecvNonActiveCount;

//   for( int i=0; i<mGroupSize; i++ )
//     mBytesRecvFromNode[ i ] = 0;

  for( int i=0; i<mActiveSendRanksCount; i++ )
    {
    int Rank = mActiveSendRanks[ i ];

    assert( Rank >=0 && Rank < mGroupSize );
    mSendOffsets[ Rank ] = 0;
    }

  if( mActiveSendRanksCount > 0 )
    {
    memcpy( mActiveSendRanksTransient,
            mActiveSendRanks, sizeof(int)*mActiveSendRanksCount);
    mCurrentRankPtr = mActiveSendRanksTransient;
    }

  if( mActiveSendRanksCount == 0 )
    mEndRankPtr = mCurrentRankPtr;
  else
    mEndRankPtr = &(mActiveSendRanksTransient[ mActiveSendRanksCount - 1 ]);

  int LinkPicker = 0;
  int  DestRank = GetNextRankToSend();

  int PktCount = 0;

  int LinkListsDone = 0;
  int CurLinkIndex  = 0;
  int StuckToSendCount = 0;

  int FifoPicker;
  if( mMyCore == 0 )
    FifoPicker = 0;
  else
    FifoPicker = 1;

  AlltoallvInitFinis.HitOE( PKTRACE_ALLTOALLV_INIT,
                            "AlltoallvInitFinis",
                            Platform::Topology::GetAddressSpaceId(),
                            AlltoallvInitFinis );

  while( ( DestRank != SEND_DONE ) ||
         ( mRecvCountDone != mGroupSize ) )
    {
    if( mMyCore == 0 )
    {
      _ts_GetStatus0_nosync( & FifoStatus );
    }
    else
    {
      _ts_GetStatus1_nosync( & FifoStatus );
    }

    StuckToSendCount = 0;

    // While there are packets to send keep trying to inject packets.
    while( DestRank != SEND_DONE )
      {
      // Check if there's room in the fifo to send

      int TotalBytesToSend = mSendCount[ DestRank ] * mSendTypeSize;
      int BytesSent        = mSendOffsets[ DestRank ];
      int BytesToSend      = 0;

      assert( BytesSent < TotalBytesToSend );
      if( ( BytesSent + _BGL_TORUS_PKT_MAX_PAYLOAD ) <=  TotalBytesToSend )
        {
        BytesToSend = _BGL_TORUS_PKT_MAX_PAYLOAD;
        }
      else
        {
        // Last packet might not be full
        BytesToSend = ( TotalBytesToSend - BytesSent );
        }

      int chunks = ((BytesToSend + 15) >> 5) ;

      _BGL_TorusFifo * NextFifoToSend;
      if( mMyCore == 0 ) NextFifoToSend = _ts_GetNextFifoCheckClearToSendOnLink0( & FifoStatus, chunks );
      else               NextFifoToSend = _ts_GetNextFifoCheckClearToSendOnLink1( & FifoStatus, chunks );

      if( NextFifoToSend != NULL )
        {
        // We can send!
        assert( DestRank >= 0 && DestRank < mGroupSize );
        int DestCoordX = mGroup->mElements[ DestRank ].mCoord.mX;
        int DestCoordY = mGroup->mElements[ DestRank ].mCoord.mY;
        int DestCoordZ = mGroup->mElements[ DestRank ].mCoord.mZ;

        // Send a full packet
        BGLTorusMakeHdrAppChooseRecvFifo( & ( mAlltoAllvPacket.hdr ),
                                          DestCoordX,
                                          DestCoordY,
                                          DestCoordZ,
                                          0,
                                          0,
                                          BytesToSend,
                                          FifoPicker );

        // if first packet of message, send the control key which has the number of elements the other size expects to receive.
        // The packet header has 2 words. Below is my current strategy for using that memory
        // 2 bytes source
        // 3 bytes TotalCount
        // 3 bytes Offset
        BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &mAlltoAllvPacket.hdr.swh0;

        MetaData->mSourceRank          = mMyRank;
        MetaData->mTotalCount          = mSendCount[ DestRank ];
        MetaData->mOffset              = mSendOffsets[ DestRank ];
        MetaData->mBytesInPayload      = BytesToSend;

        char* BlockToSend = ((char *)mSendBuff) + (mSendOffsetIn[ DestRank ]*mSendTypeSize) + MetaData->mOffset;

        memcpy_double( (double *) mAlltoAllvPacket.payload, (double *)BlockToSend, BytesToSend );
         //memcpy( mAlltoAllvPacket.payload, BlockToSend, BytesToSend );

          BegLogLine(PKFXLOG_ALLTOALLV_DATA) 
            << "BGLTorusInjectPacketImageApp"
            << " mMyRank=" << mMyRank
            << " DestRank=" << DestRank
            << " BytesToSend=" << BytesToSend
            << " MetaData->mOffset=" << MetaData->mOffset
            << " MetaData->mTotalCount=" << MetaData->mTotalCount
            << " mRecvTypeSize=" << mRecvTypeSize
            << " NextFifoToSend=" << NextFifoToSend
            << " &mAlltoAllvPacket=" << &mAlltoAllvPacket
            << EndLogLine ; 
          
     	  void ** BlockToSendV = (void **) &mAlltoAllvPacket ;
      	  BegLogLine(PKFXLOG_ALLTOALLV_DATA)
      	    << "Length=" << BytesToSend
            << " DestRank=" << DestRank
      	    << " Raw Data 0x" << BlockToSendV[0]
      	    << BlockToSendV[1]
      	    << BlockToSendV[2]
      	    << BlockToSendV[3]
      	    << BlockToSendV[4]
      	    << BlockToSendV[5]
      	    << BlockToSendV[6]
      	    << BlockToSendV[7]
      	    << EndLogLine ;
        BGLTorusInjectPacketImageApp( NextFifoToSend,
                                      &mAlltoAllvPacket );
          BegLogLine(PKFXLOG_ALLTOALLV_DATA) 
            << "BGLTorusInjectPacketImageApp returned"
            << EndLogLine ; 

//         printf( "Chunks %d sent to dest: %d\n", chunks, DestRank );
//         fflush( stdout);
        // Need to do this once we actually send something
        mSendOffsets[ DestRank ] += BytesToSend;
        DestRank = GetNextRankToSend();
        }
      else
        {
        StuckToSendCount++;

        if( StuckToSendCount == mNumberOfFifosInUse )
          break;
        }
      }

    // While we haven't received all needed packets keep listening
#if !defined(PK_ALL2ALL_BY_MESSAGE)
   if ( mRecvCountDone != mGroupSize )
   {
   	
#ifndef PK_PHASE5_SPLIT_COMMS
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }
#else
      // If we have nothing to receive try to send
//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, this );
//      if( !_ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus, this ) )
//        break;

//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

#endif

      assert( mRecvCountDone <= mGroupSize );
   }
#else    
    int PollCount = PACKET_POLL_COUNT ;
    while( PollCount && (mRecvCountDone != mGroupSize) )
      {
      // Read the status.
#ifndef PK_PHASE5_SPLIT_COMMS
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }
#else
      // If we have nothing to receive try to send
//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

      if( !_ts_Poll0_OnePassReceive_Alltoallv_Chunk_Put< NON_ALIGNED1 >( & FifoStatus, this ) )
        break;

//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

#endif

      assert( mRecvCountDone <= mGroupSize );
      PollCount--;
      }
#endif      
    }
  AlltoallvExecuteSimpleFinis.HitOE( PKTRACE_ALLTOALLV_EXECUTE,
      "AlltoallvExecuteSimpleFinis",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvExecuteSimpleFinis );


   BegLogLine(PKFXLOG_ALLTOALLV) 
  	  << "PacketAlltoAllv::ExecuteSingleCoreSimple1 complete" 
  	  << EndLogLine ;
    
  }

template< int NON_ALIGNED1 >
void PacketAlltoAllv::ExecuteSingleCore1( void* aSendBuff, void* aRecvBuff )
  {
    AlltoallvExecuteStart.HitOE( PKTRACE_ALLTOALLV_EXECUTE,
        "AlltoallvExecuteStart",
        Platform::Topology::GetAddressSpaceId(),
        AlltoallvExecuteStart );
    

  AlltoallvExecBarrierStart.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvExecBarrierStart",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvExecBarrierStart );

  #ifdef PK_PHASE5_SPLIT_COMMS
    unsigned long dummy1;
    unsigned long dummy2;
    GlobalTreeReduce( &dummy1, &dummy2, sizeof( unsigned long ), _BGL_TREE_OPCODE_MAX );
  #else
    BGLPartitionBarrierCore();
  #endif
  AlltoallvExecBarrierFinis.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvExecBarrierFinis",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvExecBarrierFinis );
  	

//   printf( "ExecuteSingleCore1: Got Here: %d\n", __LINE__ );
//   fflush( stdout );

  mSendBuff = aSendBuff;
  mRecvBuff = aRecvBuff;

  // alltoallv_debug = 1;

  // memcpy data local to this node.
  BegLogLine(PKFXLOG_ALLTOALLV)
    << "mSendCount=" << mSendCount
    << EndLogLine ;

  if( mSendCount[ mMyRank ] )
    {
    char * Src = (char*)mSendBuff + (mSendOffsetIn[ mMyRank ] * mSendTypeSize);
    char * Tgt = (char*)mRecvBuff + (mRecvOffset[ mMyRank ] * mRecvTypeSize);
    int Size   = mSendCount[ mMyRank ] * mSendTypeSize;
    memcpy( Tgt, Src, Size );
    }
  mRecvCount[ mMyRank ] = mSendCount[ mMyRank ];

  _BGL_TorusFifoStatus FifoStatus;
  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;
  // _BGL_TorusFifoStatus FifoStatus1;

  mTotalSentPackets = 0;
  mTotalRecvPackets = 0;
  mRecvCountDone = mRecvNonActiveCount;
  for( int i=0; i<mGroupSize; i++ )
    mBytesRecvFromNode[ i ] = 0;

  int LinkFifoHeaderCounters[ NUMBER_OF_LINKS ];
  for( int Link=0; Link < NUMBER_OF_LINKS; Link++ )
    {
    LinkFifoHeaderCounters[ Link ] = 0;
    }

  int LinkListsDone = 0;
  int CurLinkIndex  = 0;
  while( ( LinkListsDone  != mActiveLinkCount ) ||
         ( mRecvCountDone != mGroupSize ) )
    {
    if( mMyCore == 0 )
    {
      _ts_GetStatus0_nosync( & FifoStatus );
    }
    else
    {
      _ts_GetStatus1_nosync( & FifoStatus );
    }

    // While there are packets to send keep trying to inject packets.
    int InjectionCount = 0;
    int FullFifos = 0;
    while( LinkListsDone < mActiveLinkCount )
      {
      // Pick a header list to send from
      int BreakFromSend = 0;
      int Link = 0;

      // There has to be at least one incompletely sent header list
      // lets find it
      do
        {
        // Try to send on a new link every time.
        CurLinkIndex++;
        if( CurLinkIndex >= mActiveLinkCount )
          {
          CurLinkIndex = 0;
          }

        Link = mActiveLink[ CurLinkIndex ];
        }
      while( LinkFifoHeaderCounters[ Link ] >= mHeadersForLinkCount[ Link ] );

      //int FifoSelect = Link;

      // if( mNumberOfFifosInUse == 3 )
      // Since we're only sending on 1 set of links, FifoSelect has to range from 0-2
      int FifoSelect = Link >> 1;

      // There's something to send for Link
      while( LinkFifoHeaderCounters[ Link ] < mHeadersForLinkCount[ Link ] )
        {
        // Check to see if this packet could be sent
        int HeaderIndex = LinkFifoHeaderCounters[ Link ];

        assert( HeaderIndex >= 0 && HeaderIndex < mHeadersForLinkAllocatedCount );
        assert( HeaderIndex >= 0 && HeaderIndex < mHeadersForLinkCount[ Link ] );
        _BGL_TorusPktHdr* HdrPtr = &mHeadersForLink[ Link ][ HeaderIndex ].mHdr;

        _BGL_TorusFifo * NextFifoToSend;
        int chunks = HdrPtr->hwh0.Chunks;
        if( mMyCore == 0 ) NextFifoToSend = _ts_CheckClearToSendOnLink0( & FifoStatus, FifoSelect, chunks );
        else               NextFifoToSend = _ts_CheckClearToSendOnLink1( & FifoStatus, FifoSelect, chunks );

        if( NextFifoToSend != NULL )
          {
        // int tot_c = HdrPtr->hwh0.Chunks + 1;

//         if(( mFifoStats[ FifoSelect ].mStat + tot_c ) <= _BGL_TORUS_FIFO_CHUNKS )
//           {
          // There's room to send! There goes a packet!
          //int DestRank = BGLPartitionMakeRank( HdrPtr->hwh0.X, HdrPtr->hwh1.Y, HdrPtr->hwh1.Z );
          int DestRank             = mHeadersForLink[ Link ][ HeaderIndex ].mDestRank;
          assert( DestRank != mMyRank );

          BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &HdrPtr->swh0;

          int BytesToSend = MetaData->mBytesInPayload;
          BegLogLine(PKFXLOG_ALLTOALLV_DATA) 
            << " mMyRank=" << mMyRank
            << " DestRank=" << DestRank
            << " BytesToSend=" << BytesToSend
            << " MetaData->mOffset=" << MetaData->mOffset
            << " MetaData->mTotalCount=" << MetaData->mTotalCount
            << " mRecvTypeSize=" << mRecvTypeSize
            << EndLogLine ; 
            
          assert( BytesToSend );

          assert( (MetaData->mOffset + BytesToSend) <= ( MetaData->mTotalCount * mRecvTypeSize ) );

          assert( DestRank >= 0 && DestRank < mGroupSize );
          char* BlockToSend = ((char *)mSendBuff) + (mSendOffsetIn[ DestRank ] * mSendTypeSize) + MetaData->mOffset;

          // assert( ((unsigned long)BlockToSend & 0xf) == 0);

         	  void ** BlockToSendV = (void **) BlockToSend ;
          	  BegLogLine(PKFXLOG_ALLTOALLV_DATA)
          	    << "Length=" << BytesToSend
                << " DestRank=" << DestRank
          	    << " Data 0x" << BlockToSendV[0]
          	    << BlockToSendV[1]
          	    << BlockToSendV[2]
          	    << BlockToSendV[3]
          	    << BlockToSendV[4]
          	    << BlockToSendV[5]
          	    << BlockToSendV[6]
          	    << BlockToSendV[7]
          	    << EndLogLine ;
          
		  if ( ((unsigned int)BlockToSend) & 0x0f )
		  {
  	        // Data are not be on 16-byte boundaries; arrange to 'memcpy' it all
	          memcpy( mAlltoAllvPacket.payload, BlockToSend, BytesToSend );
	
	          // mAlltoAllvPacket.hdr = *HdrPtr;
	          QuadMove( HdrPtr, &mAlltoAllvPacket.hdr, 1 );
	
	          /// _BGL_TorusFifo * NextFifoToSend = mFifoStats[ FifoSelect ].mLink;
	
	          BGLTorusInjectPacketImageApp( NextFifoToSend,
	                                        &mAlltoAllvPacket );

		  } else {
			// Data are on 16-byte boundaries, can go 'direct'
			        // assert( ((unsigned long)BlockToSend & 0xf) == 0);
	        	    BGLTorusInjectPacketSeparateHeaderApp(NextFifoToSend,HdrPtr,BlockToSend) ;
		  }
          mTotalSentPackets++;
          // +1 becuase
          // giampapa (9:54 PM) - hardware is "org -1", ie 3bit field
          // need to represent values 1 through 8, so we subtracted 1

          // mFifoStats[ FifoSelect ].mStat += tot_c;
          LinkFifoHeaderCounters[ Link ]++;

          if( LinkFifoHeaderCounters[ Link ] == mHeadersForLinkCount[ Link ] )
            LinkListsDone++;

          InjectionCount++;
          if( InjectionCount == mPacketsToInject )
            {
            InjectionCount = 0;
            BreakFromSend = 1;
            break;
            }
          }
        else
          {
          // Fifo is full.
          // Try sending to another fifo.
          // If all fifos are full, Take a break from sending.
//           FullFifos++;
//           if( FullFifos == ( mActiveLinkCount - LinkListsDone ) )
          BreakFromSend = 1;
          break;
          }
        }

      if( BreakFromSend )
        break;
      }

    // While we haven't received all needed packets keep listening
#if !defined(PK_ALL2ALL_BY_MESSAGE)
   if ( mRecvCountDone != mGroupSize )
   {
   	
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }
   }
#else
    int PollCount = PACKET_POLL_COUNT ;
    while( PollCount && (mRecvCountDone != mGroupSize) )
      {
      // Read the status. TEMPORARY
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }

      assert( mRecvCountDone <= mGroupSize );
      PollCount--;
      }
#endif      
    }

  // Barrier the phase. We need to receive packets from another phase
// BGLPartitionBarrierComputepaCore0();
  AlltoallvExecuteFinis.HitOE( PKTRACE_ALLTOALLV_EXECUTE,
      "AlltoallvExecuteFinis",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvExecuteFinis );


  }

void PacketAlltoAllv::ExecuteSingleCore( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                         void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                         GroupContainer* aGroup, int aligned, int debug )
  {
  alltoallv_debug = debug;
  InitSingleCore( aSendCount, aSendOffset, aSendTypeSize,
                  aRecvCount, aRecvOffset, aRecvTypeSize, aGroup );

  if( aligned && ( 0 == (mSendTypeSize & 8) ))
    {
    ExecuteSingleCore1<0>( aSendBuf, aRecvBuf );
    }
  else
    {
    ExecuteSingleCore1<1>( aSendBuf, aRecvBuf );
    }

  FinalizeSingleCore();
  alltoallv_debug = 0;
  }

void PacketAlltoAllv::ExecuteSingleCoreSimple( void* aSendBuf, int aSendCount[], int* aSendOffset, int aSendTypeSize,
                                               void* aRecvBuf, int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                                               GroupContainer* aGroup, int aligned, int debug,
                                               int* aSendNodeList, int aSendNodeListCount,
                                               int* aRecvNodeList, int aRecvNodeListCount )
  {
  alltoallv_debug = debug;

//   printf( "alltoallv_debug= %d\n", alltoallv_debug );
//   fflush( stdout );

  InitSingleCoreSimple( aSendCount, aSendOffset, aSendTypeSize,
                        aRecvCount, aRecvOffset, aRecvTypeSize, aGroup,
      aSendNodeList, aSendNodeListCount, aRecvNodeList, aRecvNodeListCount );

  if( aligned && ( 0 == (mSendTypeSize & 8) ) )
    // We are carrying around things which are at least doubleword-aligne doubles
    ExecuteSingleCoreSimple1<0>( aSendBuf, aRecvBuf );
  else
    ExecuteSingleCoreSimple1<1>( aSendBuf, aRecvBuf );

#if 0
  FinalizeSingleCoreSimple();
#endif

  alltoallv_debug = 0;
  }

void PacketAlltoAllv::ReportFifoHistogram(void)
{
	fStatus.display() ;
}
  
