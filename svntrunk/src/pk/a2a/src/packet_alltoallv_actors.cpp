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

class complex
  {
  public:
    double re ;
    double im ;
    complex()       { }
    complex(double r, double i = 0.0)       { re=r; im=i; }
    complex(const complex& c) { re = c.re; im = c.im; }
  } ;

int alltoallv_debug = 0;
/// #define DEBUG_ALL2ALL

#define PACKET_POLL_COUNT 4
#define PACKETS_TO_INJECT 6

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/pktrace.hpp>
#include <pk/a2a/packet_alltoallv_actors.hpp>
#include <BonB/BGLTorusAppSupport.c>


static TraceClient AlltoallvInitStart;
static TraceClient AlltoallvInitFinis;
static TraceClient AlltoallvExecBarrierStart;
static TraceClient AlltoallvExecBarrierFinis;

static TraceClient AlltoallvPermRanksStart;
static TraceClient AlltoallvPermRanksFinis;

static TraceClient AlltoallvShellStart;

#ifndef PKTRACE_ALLTOALLV_INIT
#define PKTRACE_ALLTOALLV_INIT ( 0 )
#endif

#ifndef PKFXLOG_ALLTOALLV
#define PKFXLOG_ALLTOALLV (0)
#endif

#ifndef PKFXLOG_PACKET_ALLTOALLV
#define PKFXLOG_PACKET_ALLTOALLV (0)
#endif

static int DriveCount ;

static PacketAlltoAllv_Actors * CurrentA2A ;

// static int CurrentBlockShift ;
static void Actor_AllToAllV(unsigned int aFlags, unsigned int aParam, unsigned int aCells, void * aPayload)
{
	BegLogLine(PKFXLOG_ALLTOALLV)
	  << " Actor_AllToAllV CurrentA2A=" << CurrentA2A
	  << EndLogLine ; 
	CurrentA2A -> Actor(aFlags, aParam, aCells, aPayload ) ;
}

void
PacketAlltoAllv_Actors::Actor(unsigned int aFlags, unsigned int aParam, unsigned int aCells, void * aPayload)
{
	DriveCount += 1 ;
	int SourceRank = aParam >> mBlockShift ; 
	int SourceSequence = aParam & ( ( 1 << mBlockShift ) - 1 ) ;
	int ByteCount = mSendBytes >> mBlockShift ;
	char * RealDst = ((char*)mRecvBuff) + (mRecvOffset[ SourceRank ]*mRecvTypeSize) + SourceSequence*ByteCount;
	BegLogLine(PKFXLOG_ALLTOALLV)
	  << "Source rank=" << SourceRank
	  << " Source sequence=" << SourceSequence
	  << " mSendBytes=" << mSendBytes 
	  << " mRecvBuff=" << mRecvBuff
	  << " mRecvOffset[ SourceRank ]=" << mRecvOffset[ SourceRank ]
	  << " mRecvTypeSize=" << mRecvTypeSize
	  << " Proposing to place " << ByteCount
	  << " bytes at address 0x" << (void *) RealDst
      << EndLogLine ; 	
    assert(0 == (( int ) RealDst) & 0x07) ; // Only do aligned stuff
    assert(0 == (ByteCount & 0x7) ) ; // Only whole doublewords
    double * RealDestDbl = (double *) RealDst ; 
    double * QuarterChunks = (double *) aPayload ; 
  	double QC0 = QuarterChunks[0] ;
  	double QC1 = QuarterChunks[1] ;
  	double QC2 = QuarterChunks[2] ;
  	double QC3 = QuarterChunks[3] ;
  	int PayloadIndex = 0 ; 
  	while ( ByteCount > 24 )
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
  		ByteCount -= 32 ;
  	}
  	if ( ByteCount > 0 )
  	{
  		RealDestDbl[PayloadIndex] = QC0 ;
  	    if ( ByteCount > 8 )
  	    {
  		  RealDestDbl[PayloadIndex+1] = QC1 ;
  	      if ( ByteCount > 16 )
  	      {
  		    RealDestDbl[PayloadIndex+2] = QC2 ;
  	      }
   	   }
    }
    
}

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
PacketAlltoAllv_Actors::RandomizeHeaders( BGLAlltoallvHeaders* aHdrs, int aCount )
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

// Pro tem, we will do our own 'actors' ...
//class BGL_AllToAll_TorusPktHdr
//{
//	public: 
//	_BGL_Torus_HdrHW0 hwh0 ; 
//	_BGL_Torus_HdrHW1 hwh1 ; 
//	BGL_AlltoallvPacketMetaData PacketMetaData ;
//} ;

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
    template< int NON_ALIGNED1 > static inline int _ts_AppReceiver_Alltoallv_Put_Actor( _BGL_TorusFifo *fifo, PacketAlltoAllv_Actors* ThisPtr);

// Drain: move possibly several packets (up to 'chunks' chunks) from fifo to buf
    template< int NON_ALIGNED1 > static inline int _ts_AppReceiver_Alltoallv_Drain_Actor( _BGL_TorusFifo *fifo, PacketAlltoAllv_Actors* ThisPtr, int Chunks);

    template< int NON_ALIGNED1 > static inline void _ts_PollBoth_OnePassReceive_Alltoallv_Put_Actor( _BGL_TorusFifoStatus *statF0, _BGL_TorusFifoStatus *statF1, PacketAlltoAllv_Actors* ThisPtr );
    template< int NON_ALIGNED1 > static inline void _ts_Poll0_OnePassReceive_Alltoallv_Put_Actor( _BGL_TorusFifoStatus *statF0, PacketAlltoAllv_Actors* ThisPtr );
    template< int NON_ALIGNED1 > static inline int  _ts_Poll0_OnePassReceive_Alltoallv_Chunk_Put_Actor( _BGL_TorusFifoStatus *statF0, PacketAlltoAllv_Actors* ThisPtr );
    template< int NON_ALIGNED1 > static inline int  _ts_Poll0_OnePassReceive_Alltoallv_Drain_Actor( _BGL_TorusFifoStatus *statF0, PacketAlltoAllv_Actors* ThisPtr );
    template< int NON_ALIGNED1 > static inline void _ts_Poll1_OnePassReceive_Alltoallv_Put_Actor( _BGL_TorusFifoStatus *statF1, PacketAlltoAllv_Actors* ThisPtr );
    template< int NON_ALIGNED1 > static inline int  _ts_Poll1_OnePassReceive_Alltoallv_Drain_Actor( _BGL_TorusFifoStatus *statF1, PacketAlltoAllv_Actors* ThisPtr );
    template< int NON_ALIGNED1 > static inline int _ts_PollBoth_OnePassReceive_Alltoallv_Drain_Actor( _BGL_TorusFifoStatus *statF0, _BGL_TorusFifoStatus *statF1, PacketAlltoAllv_Actors* ThisPtr );


template< int NON_ALIGNED1 >
static inline
int _ts_AppReceiver_Alltoallv_Drain_Actor( _BGL_TorusFifo *fifo, PacketAlltoAllv_Actors* aThis, int ChunksInFifo )
  {
    enum { k_DrainDepth = _BGL_TORUS_RECEPTION_FIFO_CHUNKS } ;
    fStatus.recordReceive(ChunksInFifo) ;
     
    register int u = 16;
    assert(ChunksInFifo <=  k_DrainDepth) ;
    // Get all the lines
    double _Complex HalfChunks[2*k_DrainDepth] ;
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
    
    // The lines are all in memory now; process them
    

  int PacketsInFifo = 0 ; 
  int Chunks = ((_BGL_TorusPktHdr *) HalfChunks)->hwh0.Chunks ;
//  int SourceRank = ((_BGL_TorusPktHdr *) HalfChunks)->PacketMetaData.mSourceRank ;
//  int TotalCountToRecv = ((_BGL_TorusPktHdr *) HalfChunks)->PacketMetaData.mTotalCount ;
//  int Offset = ((_BGL_TorusPktHdr *) HalfChunks)->PacketMetaData.mOffset ;
//  int BytesInPayload = ((_BGL_TorusPktHdr *) HalfChunks)->PacketMetaData.mBytesInPayload ;
  int ThisPacketChunkIndex=0 ;
  while ( ThisPacketChunkIndex < ChunksInFifo )
  { 
  	  _BGL_TorusPktHdr *hdr = (_BGL_TorusPktHdr *)(HalfChunks+2*ThisPacketChunkIndex) ;
	  unsigned int extra = hdr->swh0.extra ;
	  unsigned int fcn = hdr->swh0.fcn << 2;
	  unsigned int arg = hdr->swh1.arg ; 
	  BegLogLine(PKFXLOG_ALLTOALLV) 
	    << "_ts_AppReceiver_Alltoallv_Drain_Actor extra=" << extra
	    << " fcn=0x" << (void *) fcn
	    << " Actor_AllToAllV=0x" << (void *) Actor_AllToAllV
	    << " arg=" << arg 
	    << " Chunks=" << Chunks 
	    << EndLogLine ;
	
	  PacketsInFifo += 1 ; 
	  void (*ActorFunction)(unsigned int, unsigned int, unsigned int, void *) 
	    = (void (*)(unsigned int, unsigned int, unsigned int, void *)) fcn ;
	  
	
	  int PayloadHalfChunkIndex = 2*ThisPacketChunkIndex+1 ;
// Drive the actor function on the packet	    
	  (*ActorFunction)(extra,arg,Chunks,(void *)(HalfChunks+2*ThisPacketChunkIndex+1)) ;
	  // Set up for next packet
	  ThisPacketChunkIndex += (Chunks+1) ; 
      Chunks = ((_BGL_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->hwh0.Chunks ;
//      // and drop 'this' packet into is proper store
//      if ( NON_ALIGNED1 )
//      {
//      	// If things might be misaligned or not multiples of a doubleword
//	  	memcpy(RealDst,HalfChunks+PayloadHalfChunkIndex,BytesInPayloadThis) ; 
//      } else {
//      	// Things are aligned doublewords
//      	double * QuarterChunks = (double *) (HalfChunks+PayloadHalfChunkIndex) ;
//      	double * RealDestDbl = (double *) RealDst ;
//      	double QC0 = QuarterChunks[0] ;
//      	double QC1 = QuarterChunks[1] ;
//      	double QC2 = QuarterChunks[2] ;
//      	double QC3 = QuarterChunks[3] ;
//      	int PayloadIndex = 0 ; 
//      	while ( BytesInPayloadThis > 24 )
//      	{
//      		RealDestDbl[PayloadIndex] = QC0 ;
//      		RealDestDbl[PayloadIndex+1] = QC1 ;
//      		RealDestDbl[PayloadIndex+2] = QC2 ;
//      		RealDestDbl[PayloadIndex+3] = QC3 ;
//      		QC0 = QuarterChunks[PayloadIndex+4] ;
//      		QC1 = QuarterChunks[PayloadIndex+5] ;
//      		QC2 = QuarterChunks[PayloadIndex+6] ;
//      		QC3 = QuarterChunks[PayloadIndex+7] ;
//      		PayloadIndex += 4 ;
//      		BytesInPayloadThis -= 32 ;
//      	}
//      	if ( BytesInPayloadThis > 0 )
//      	{
//      		RealDestDbl[PayloadIndex] = QC0 ;
//      	    if ( BytesInPayloadThis > 8 )
//      	    {
//      		  RealDestDbl[PayloadIndex+1] = QC1 ;
//      	      if ( BytesInPayloadThis > 16 )
//      	      {
//      		    RealDestDbl[PayloadIndex+2] = QC2 ;
//      	      }
//       	   }
//        }
//      }
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
int _ts_AppReceiver_Alltoallv_Put_Actor( _BGL_TorusFifo *fifo, PacketAlltoAllv_Actors* aThis )
  {
//       _BGL_TorusPkt *pkt = (_BGL_TorusPkt *)addr;
//       Bit8 *dst          = (Bit8 *)addr;
  _BGL_TorusPktHdr hdr;
  int chunks, c2;
  register int u = 16;
//  _BGL_TorusPktPayload Chunk; // a 'scratch' place to put the payload if the real target is misaligned, and to park the first half-chunk anyway

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
      NumberOfChunksInStat = FifoStatus.r0;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R1 )
      {
      NumberOfChunksInStat = FifoStatus.r1;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R2 )
      {
      NumberOfChunksInStat = FifoStatus.r2;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R3 )
      {
      NumberOfChunksInStat = FifoStatus.r3;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R4 )
      {
      NumberOfChunksInStat = FifoStatus.r4;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R5 )
      {
      NumberOfChunksInStat = FifoStatus.r5;
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
      NumberOfChunksInStat = FifoStatus.r0;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R1 )
      {
      NumberOfChunksInStat = FifoStatus.r1;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R2 )
      {
      NumberOfChunksInStat = FifoStatus.r2;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGLu_TorusFifo *)TS1_R3 )
      {
      NumberOfChunksInStat = FifoStatus.r3;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R4 )
      {
      NumberOfChunksInStat = FifoStatus.r4;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R5 )
      {
      NumberOfChunksInStat = FifoStatus.r5;
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
  	BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
  	  << "Pulling header and first half-chunk"
  	  << EndLogLine ;
// pull 1st chunk into buffer
  asm volatile( "lfpdx   1,0,%0;"
                "lfpdx   7,0,%0;"
                "stfpdx  1,0,%1;"
                "stfpdx  7,0,%2;"
                :
                : "r"  (fifo), "r"  (&hdr), "r" (&Chunk1)
                 : "memory",  FR1STR, FR7STR );
  } while(0);

//    printf("(I) _ts_AppReceiver_Put(0x%08lx): *stat = %d, swh0.arg0 = %4ld, swh1.arg = %4ld, swa = %d\n",
//            (unsigned long)fifo, *stat, hdr->swh0.arg0, hdr->swh1.arg, hdr->hwh0.SW_Avail );

  chunks = hdr.hwh0.Chunks;  // hw hdr says (chunks - 1), we did chunk 0 already
//  assert( chunks+1 <= NumberOfChunksInStat );
  unsigned int extra = hdr.swh0.extra ;
  unsigned int fcn = hdr.swh0.fcn << 2;
  unsigned int arg = hdr.swh1.arg ; 
  BegLogLine(PKFXLOG_ALLTOALLV) 
    << "_ts_AppReceiver_Alltoallv_Put_Actor extra=" << extra
    << " fcn=0x" << (void *) fcn
    << " Actor_AllToAllV=0x" << (void *) Actor_AllToAllV
    << " arg=" << arg 
    << " chunks=" << chunks 
    << EndLogLine ;
    
  Bit8 * dst =  Chunk1;
  if( chunks < 2 )
    {
    Bit8 *dstTmp = Chunk1;

    // read a chunk from the fifo
    for( int i=0; i<chunks; i++ )
      {
      dstTmp += 16;
	  	BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
	  	  << "Pulling chunk"
	  	  << EndLogLine ;
      asm volatile( "lfpdx   7,0,%2;"
                    "lfpdx   8,0,%2;"
                    "stfpdx  7,0,%0;"
                    "stfpdux 8,%0,%1;"
                    : "=b" (dstTmp)
                    : "b"  (u),
                    "r"  (fifo) ,"0"  (dstTmp)
                    : "memory", FR7STR, FR8STR );
      }

    }
  else
    {

    dst    += 16;
      int chunkIndex=0 ;
      for (;chunkIndex<chunks-1;chunkIndex+=2) 
      {
	  	BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
	  	  << "Pulling chunkpair, chunkIndex=" << chunkIndex
	  	  << " chunks=" << chunks
	  	  << EndLogLine ;
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

      if ( chunkIndex < chunks )
      {
	  	BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
	  	  << "Pulling chunk"
	  	  << EndLogLine ;
      asm volatile( "lfpdx   7,0,%2;"
                    "lfpdx   8,0,%2;"
                    "stfpdx  7,0,%0;"
                    "stfpdux 8,%0,%1;"
                    : "=b" (dst)
                    : "b"  (u), "r"  (fifo),"0"  (dst)
                    : "memory", FR7STR, FR8STR );
      }

    }

	  void (*ActorFunction)(unsigned int, unsigned int, unsigned int, void *) 
	    = (void (*)(unsigned int, unsigned int, unsigned int, void *)) fcn ;
	    
// Drive the actor function on the packet	 
     (*ActorFunction)(extra,arg,chunks,(void *)(&Chunk1)) ;
  

  return( chunks );
  }


template< int NON_ALIGNED1 >
static inline
int
_ts_Poll0_OnePassReceive_Alltoallv_Chunk_Put_Actor( _BGL_TorusFifoStatus *statF0,
                                                               PacketAlltoAllv_Actors* aThis )
  {
  int TotalPackets = 0;
  
  fStatus.recordReceive(statF0->r0) ;

  if ( statF0->r0 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r0=" << statF0->r0 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis );
    TotalPackets++;
    statF0->r0 -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r1) ;
  if ( statF0->r1 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r1=" << statF0->r1 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R1, aThis );
    TotalPackets++;
    statF0->r1 -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r2) ;
  if ( statF0->r2 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r2=" << statF0->r2
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );
    TotalPackets++;
    statF0->r2 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );

  fStatus.recordReceive(statF0->r3) ;
  if ( statF0->r3 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r3=" << statF0->r3 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );
    TotalPackets++;
    statF0->r3 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );

  fStatus.recordReceive(statF0->r4) ;
  if ( statF0->r4 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r4=" << statF0->r4 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );
    TotalPackets++;
    statF0->r4 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );

  fStatus.recordReceive(statF0->r5) ;
  if ( statF0->r5 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r5=" << statF0->r5 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );
    TotalPackets++;
    statF0->r5 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );

  return TotalPackets;
  }

template< int NON_ALIGNED1 >  
static inline
int
_ts_Poll0_OnePassReceive_Alltoallv_Drain_Actor( _BGL_TorusFifoStatus *statF0,
                                                               PacketAlltoAllv_Actors* aThis )
  {
  int TotalPackets = 0;

  int r0 = statF0->r0 ;
//  statF0->r0 = 0 ;
//  statF0->r1 = 0 ;
//  statF0->r2 = 0 ;
//  statF0->r3 = 0 ;
//  statF0->r4 = 0 ;
//  statF0->r5 = 0 ;
  if ( r0 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis, r0 ) ;
  } 
  int r1 = statF0->r1 ;
  if ( r1 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R1, aThis, r1 ) ;
  } 
  int r2 = statF0->r2 ;
  if ( r2 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R2, aThis, r2 ) ;
  } 
  int r3 = statF0->r3 ;
  if ( r3 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R3, aThis, r3 ) ;
  } 
  int r4 = statF0->r4 ;
  if ( r4 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R4, aThis, r4 ) ;
  } 
  int r5 = statF0->r5 ;
  if ( r5 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R5, aThis, r5 ) ;
  } 

  return TotalPackets;
  }

template< int NON_ALIGNED1 >
static inline
void
_ts_Poll0_OnePassReceive_Alltoallv_Put_Actor( _BGL_TorusFifoStatus *statF0,
                                                            PacketAlltoAllv_Actors* aThis )
  {
  fStatus.recordReceive(statF0->r0) ;
  	
  if ( statF0->r0 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r0=" << statF0->r0 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis );
    statF0->r0 -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r1) ;
  if ( statF0->r1 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r1=" << statF0->r1 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R1, aThis );
    statF0->r1 -= (chunks+1);
    }

  fStatus.recordReceive(statF0->r2) ;
  if ( statF0->r2 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r2=" << statF0->r2 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );
    statF0->r2 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );

  fStatus.recordReceive(statF0->r3) ;
  if ( statF0->r3 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r3=" << statF0->r3 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );
    statF0->r3 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );

  fStatus.recordReceive(statF0->r4) ;
  if ( statF0->r4 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r4=" << statF0->r4 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );
    statF0->r4 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );

  fStatus.recordReceive(statF0->r5) ;
  if ( statF0->r5 )
    {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r5=" << statF0->r5 
    	  << EndLogLine ;
    int chunks = _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );
    statF0->r5 -= (chunks+1);
    }
  // _ts_AppReceiver_Alltoallv_Put< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );

  return;
  }

template< int NON_ALIGNED1 >
static inline
void
_ts_Poll1_OnePassReceive_Alltoallv_Put_Actor( _BGL_TorusFifoStatus *statF1,
                                                         PacketAlltoAllv_Actors* aThis )
  {
  fStatus.recordReceive(statF1->r0) ;
  if ( statF1->r0 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r0=" << statF1->r0 
    	  << EndLogLine ;
    _ts_AppReceiver_Alltoallv_Put_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R0, aThis );
  }
  fStatus.recordReceive(statF1->r1) ;
  if ( statF1->r1 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r1=" << statF1->r1 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R1, aThis );
  }
      
  fStatus.recordReceive(statF1->r2) ;
  if ( statF1->r2 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r2=" << statF1->r2 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R2, aThis );
  }

  fStatus.recordReceive(statF1->r3) ;
  if ( statF1->r3 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r3=" << statF1->r3 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R3, aThis );
  }

  fStatus.recordReceive(statF1->r4) ;
  if ( statF1->r4 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r4=" << statF1->r4 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R4, aThis );
  }

  fStatus.recordReceive(statF1->r5) ;
  if ( statF1->r5 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r5=" << statF1->r5 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R5, aThis );
  }

  return;
  }

template< int NON_ALIGNED1 >
static inline
int
_ts_Poll1_OnePassReceive_Alltoallv_Drain_Actor( _BGL_TorusFifoStatus *statF1,
                                                               PacketAlltoAllv_Actors* aThis )
  {
  int TotalPackets = 0;

  int r0 = statF1->r0 ;
//  statF1->r0 = 0 ;
//  statF1->r1 = 0 ;
//  statF1->r2 = 0 ;
//  statF1->r3 = 0 ;
//  statF1->r4 = 0 ;
//  statF1->r5 = 0 ;
  if ( r0 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R0, aThis, r0 ) ;
  } 
  int r1 = statF1->r1 ;
  if ( r1 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R1, aThis, r1 ) ;
  } 
  int r2 = statF1->r2 ;
  if ( r2 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R2, aThis, r2 ) ;
  } 
  int r3 = statF1->r3 ;
  if ( r3 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R3, aThis, r3 ) ;
  } 
  int r4 = statF1->r4 ;
  if ( r4 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R4, aThis, r4 ) ;
  } 
  int r5 = statF1->r5 ;
  if ( r5 ) {
  	TotalPackets += _ts_AppReceiver_Alltoallv_Drain_Actor<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R5, aThis, r5 ) ;
  } 

  return TotalPackets;
  }
template< int NON_ALIGNED1 >
static inline
void
_ts_PollBoth_OnePassReceive_Alltoallv_Put_Actor( _BGL_TorusFifoStatus *statF0,
                                                            _BGL_TorusFifoStatus *statF1,
                                                            PacketAlltoAllv_Actors* aThis )
  {
  fStatus.recordReceive(statF0->r0) ;
  if ( statF0->r0 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r0=" << statF0->r0 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R0, aThis );
  }

  fStatus.recordReceive(statF0->r1) ;
  if ( statF0->r1 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r1=" << statF0->r1 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R1, aThis );
  }

  fStatus.recordReceive(statF0->r2) ;
  if ( statF0->r2 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r2=" << statF0->r2 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R2, aThis );
  }

  fStatus.recordReceive(statF0->r3) ;
  if ( statF0->r3 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r3=" << statF0->r3 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R3, aThis );
  }

  fStatus.recordReceive(statF0->r4) ;
  if ( statF0->r4 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r4=" << statF0->r4 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R4, aThis );
  }

  fStatus.recordReceive(statF0->r5) ;
  if ( statF0->r5 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF0->r5=" << statF0->r5 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS0_R5, aThis );
  }

  fStatus.recordReceive(statF1->r0) ;
  if ( statF1->r0 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r0=" << statF1->r0 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R0, aThis );
  }

  fStatus.recordReceive(statF1->r1) ;
  if ( statF1->r1 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r1=" << statF1->r1
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R1, aThis );
  }

  fStatus.recordReceive(statF1->r2) ;
  if ( statF1->r2 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r2=" << statF1->r2 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R2, aThis );
  }

  fStatus.recordReceive(statF1->r3) ;
  if ( statF1->r3 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r3=" << statF1->r3 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R3, aThis );
  }

  fStatus.recordReceive(statF1->r4) ;
  if ( statF1->r4 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r4=" << statF1->r4 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1 >( (_BGL_TorusFifo *)TS1_R4, aThis );
  }

  fStatus.recordReceive(statF1->r5) ;
  if ( statF1->r5 )
  {
    	BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    	  << "Chunk_Put_Actor statF1->r5=" << statF1->r5 
    	  << EndLogLine ;
      _ts_AppReceiver_Alltoallv_Put_Actor< NON_ALIGNED1  >( (_BGL_TorusFifo *)TS1_R5, aThis );
  }

  return;
  }

template< int NON_ALIGNED1 >
static inline
int
_ts_PollBoth_OnePassReceive_Alltoallv_Drain_Actor(_BGL_TorusFifoStatus *statF0,
                                                            _BGL_TorusFifoStatus *statF1,
                                                            PacketAlltoAllv_Actors* aThis )
{
	return 	_ts_Poll0_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( statF0, aThis )
	      + _ts_Poll1_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( statF1, aThis );
}            
                                                
void PacketAlltoAllv_Actors::PermuteActiveSendRanks( int* aRanks, int aSize )
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

int PacketAlltoAllv_Actors::GetNextRankToSend()
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

struct PacketAlltoAllv_ActorsInitActorArgs
   {
   PacketAlltoAllv_Actors* mThis;
   int * mSendCount;
   int * mSendOffsets;
   int   mSendTypeSize;
   int * mRecvCount;
   int * mRecvOffsets;
   int   mRecvTypeSize;
   GroupContainer_Actors* mGroup;
   int   mDebug;
   };

void* PacketAlltoAllv_Actors::InitActor( void* arg )
{
  PacketAlltoAllv_ActorsInitActorArgs* Args =   (PacketAlltoAllv_ActorsInitActorArgs*) arg;

  // printf("Got here %d\n", __LINE__);

  PacketAlltoAllv_Actors* ThisPtr = Args->mThis;
  int* SendCount    = Args->mSendCount;
  int* RecvCount    = Args->mRecvCount;
  int* SendOffset   = Args->mSendOffsets;
  int* RecvOffset   = Args->mRecvOffsets;
  int  SendTypeSize = Args->mSendTypeSize;
  int  RecvTypeSize = Args->mRecvTypeSize;
  GroupContainer_Actors* Group = Args->mGroup;

  BegLogLine(PKFXLOG_ALLTOALLV)
    << "PacketAlltoAllv_Actors::InitActor" 
    << EndLogLine ;

  alltoallv_debug    = Args->mDebug;

 // printf( "Group: %08x line: \n", Group, __LINE__ );

//  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
//      printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
//           sizeof( BGL_AlltoallvPacketMetaData ) );

//  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );

  ThisPtr->mPacketsToInject = PACKETS_TO_INJECT;
  ThisPtr->mGroup     = Group;
  ThisPtr->mMyRank    = ThisPtr->mGroup->mMyRank;  // BGLPartitionGetRank();

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

// Work out how much data we are sending. This version only works for 'symmetrical'
// all-to-alls, where we send as much as we receive, and we send the same amount of
// data to all nodes we send anything to.
  int SendSize = -1 ;
  for ( int a=0;a<ThisPtr->mGroupSize;a+=1)
  {
  	 BegLogLine(PKFXLOG_ALLTOALLV)
  	   << "Rank " << a 
  	   << " Configuring to send " << ThisPtr->mSendCount[a] 
  	   << " items and receive " << ThisPtr->mRecvCount[a]
  	   << EndLogLine ;
  	 if ( ThisPtr->mSendCount[a] != 0 )
  	 {
  	 	if ( SendSize == -1 )
  	 	{
  	 		SendSize = ThisPtr->mSendCount[a] ;
  	 	}
  	 	assert(SendSize == ThisPtr->mSendCount[a] ) ;
  	 }
  }
  assert(0 == (SendSize & (SendSize-1) ) ) ; // we only handle powers of 2
  int SendBytes = SendSize * ThisPtr->mSendTypeSize ;
  
// Figure how many messages we will need to send to carry the total payload  
  int BlockShift = 0 ; 
  for (;(BlockShift<32) && ( (SendBytes >> BlockShift) > _BGL_TORUS_PKT_MAX_PAYLOAD); BlockShift += 1)
  {
  }
  int NumberOfHeaders = 1 << BlockShift ;
  ThisPtr-> mBlockShift = BlockShift ;
  ThisPtr -> mSendBytes = SendBytes ;
  BegLogLine(PKFXLOG_ALLTOALLV)
    << "Sending " << SendBytes
    << " setting BlockShift=" << BlockShift
    << EndLogLine ;

  MaxNumberHeaders = NumberOfHeaders ;
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
//      int NumberOfHeaders = (int) ceil( (1.0*( ThisPtr->mSendCount[ i ] * ThisPtr->mSendTypeSize)) / (_BGL_TORUS_PKT_MAX_PAYLOAD*1.0) );
//
//      if( NumberOfHeaders > MaxNumberHeaders )
//          MaxNumberHeaders = NumberOfHeaders;

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

      BegLogLine(PKFXLOG_ALLTOALLV )
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
    
//    while( BytesSent < TotalBytesToSend )
    if (ThisPtr->mSendCount[ Rank ] > 0 )
    {
		for ( int HeaderIndex = 0 ; HeaderIndex < (1<<BlockShift) ; HeaderIndex += 1)
	      {
	//      int BytesToSend = 0;
	//      if( ( BytesSent + _BGL_TORUS_PKT_MAX_PAYLOAD ) <=  TotalBytesToSend )
	//        {
	//        BytesToSend = _BGL_TORUS_PKT_MAX_PAYLOAD;
	//        }
	//      else
	//        {
	//        // Last packet might not be full
	//        BytesToSend = ( TotalBytesToSend - BytesSent );
	//        }
	//
	//      assert( BytesToSend );
	
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
	                                        TotalBytesToSend >> BlockShift ,
	#ifdef PK_PHASE5_SPLIT_COMMS
	                                        1 ); // fft on core 1
	#else
	                                        ((FifoPicker++) & 0x1 ) );
	#endif
	
	
	//      BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &HdrPtr->swh0;
	
	      assert( ThisPtr->mMyRank >= 0            && ThisPtr->mMyRank < PACKET_ALLTOALLV_LIMIT_RANK );
	      assert( ThisPtr->mSendCount[ Rank ] >= 0 && ThisPtr->mSendCount[ Rank ] < PACKET_ALLTOALLV_LIMIT_COUNT );
	      assert( BytesSent >= 0                   && BytesSent < PACKET_ALLTOALLV_LIMIT_OFFSET );
	
	//      MetaData->mSourceRank          = ThisPtr->mMyRank;
	//      MetaData->mTotalCount          = ThisPtr->mSendCount[ Rank ];
	//      MetaData->mOffset              = BytesSent;
	//      MetaData->mBytesInPayload      = BytesToSend;
	      BglHdr->mDestRank              = Rank;
	      assert( BglHdr->mDestRank != ThisPtr->mMyRank );
	        HdrPtr->swh0.extra = 0 ; 
	        HdrPtr->swh0.fcn = (unsigned int)(Actor_AllToAllV) >> 2 ;
	        HdrPtr->swh1.arg = (ThisPtr->mMyRank << BlockShift) + HeaderIndex ;
	
	    
	      ThisPtr->mHeadersForLinkCount[ Link ]++;
	//      BytesSent += BytesToSend;
	
	      // mSendOffsets[ DestRank ] += BytesToSend;
	      }
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

#if !defined(SKIP_RANDOMIZING)
// Randomize Links
  RandomizeLinks( ThisPtr->mActiveLink, ThisPtr->mActiveLinkCount, ThisPtr->mMyRank );
#endif

// We have 2 strategies for ordering within the list
// 1. Randomize
// 2. Farthest packets leave first

#if !defined(SKIP_RANDOMIZING)
  for( int i=0; i<ThisPtr->mActiveLinkCount; i++ )
    {
    int Link = ThisPtr->mActiveLink[ i ];

    RandomizeHeaders( ThisPtr->mHeadersForLink[ Link ], ThisPtr->mHeadersForLinkCount[ Link ] );
    }
#endif
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

void PacketAlltoAllv_Actors::InitSingleCore( int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                      int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                      GroupContainer_Actors* aGroup )
  {
  PacketAlltoAllv_ActorsInitActorArgs Core0Args;

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

void PacketAlltoAllv_Actors::InitSingleCoreSimple( int aSendCount[], int* aSendOffset, int aSendTypeSize,
                                            int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                                            GroupContainer_Actors* aGroup,
                                            int* aSendNodeList, int aSendNodeListCount,
                                            int* aRecvNodeList, int aRecvNodeListCount )
  {
  AlltoallvInitStart.HitOE( PKTRACE_ALLTOALLV_INIT,
                           "AlltoallvInitStart",
                            Platform::Topology::GetAddressSpaceId(),
                           AlltoallvInitStart );

  // alltoallv_debug    = 0;


  mPacketsToInject = PACKETS_TO_INJECT;
  mGroup     = aGroup;
  mMyRank    = mGroup->mMyRank;

  mGroupSize = mGroup->mSize;
  mGroupElements = mGroup->mElements;

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
BegLogLine( 0  )
    << SimTick
    << " aSendNodeListCount: " << aSendNodeListCount
    << " aRecvNodeListCount: " << aRecvNodeListCount
    << " "
    << EndLogLine;

if(!( (aSendNodeList == NULL ) && ( aSendNodeListCount == 0  ) && ( aRecvNodeListCount == 0 ) ))
///if( 0 )
    {
    for( int i=0; i < aSendNodeListCount; i++ )
      {
      if( aSendNodeList[ i ] != mMyRank )
        {
        mActiveSendRanks[ mActiveSendRanksCount ] = aSendNodeList[ i ];
        mActiveSendRanksCount++;
        }
      }

    // Loop to see if MyRank is in the list
    int IsMyRankOnList = 0;
    for( int i=0; i < aRecvNodeListCount; i++ )
      {
      int nodeId = aRecvNodeList[ i ];
      mBytesRecvFromNode[ nodeId ] = 0;

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
  PacketAlltoAllv_Actors* mThis;
  };

void* PacketAlltoAllv_Actors::FinalizeActor( void* arg )
  {
  FinalizeActorArg* Args =   (FinalizeActorArg *) arg;

  PacketAlltoAllv_Actors* ThisPtr = Args->mThis;

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
void PacketAlltoAllv_Actors::Finalize()
  {
  FinalizeActorArg Core0Args;
  Core0Args.mThis        = this;

  FinalizeActor( &Core0Args );
  }
#endif

void PacketAlltoAllv_Actors::FinalizeSingleCore()
  {
  FinalizeActorArg         Core0Args;
  Core0Args.mThis        = this;
  FinalizeActor( &Core0Args );
  }

void PacketAlltoAllv_Actors::FinalizeSingleCoreSimple()
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
void PacketAlltoAllv_Actors::ExecuteSingleCoreSimple1( void* aSendBuff, void* aRecvBuff )
  {
  	BegLogLine(PKFXLOG_ALLTOALLV) 
  	  << "PacketAlltoAllv_Actors::ExecuteSingleCoreSimple1 mRecvNonActiveCount=" << mRecvNonActiveCount
  	  << " mActiveSendRanksCount=" << mActiveSendRanksCount
  	  << " mGroupSize=" << mGroupSize
  	  << EndLogLine ;
  AlltoallvExecBarrierStart.HitOE( PKTRACE_ALLTOALLV_INIT,
      "AlltoallvExecBarrierStart",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvExecBarrierStart );

  BGLPartitionBarrierCore();

  AlltoallvExecBarrierFinis.HitOE( PKTRACE_ALLTOALLV_INIT,
      "AlltoallvExecBarrierFinis",
      Platform::Topology::GetAddressSpaceId(),
      AlltoallvExecBarrierFinis );

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
  // We are going to try to terminate when we have sent everything and received as much as we have sent;
  // i.e. 'symmetrical' all-to-alls
  int StartDriveCount = DriveCount ; 

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
           ( DriveCount != StartDriveCount+mTotalSentPackets ) )
//         ( mRecvCountDone != mGroupSize ) )
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

//        // if first packet of message, send the control key which has the number of elements the other size expects to receive.
//        // The packet header has 2 words. Below is my current strategy for using that memory
//        // 2 bytes source
//        // 3 bytes TotalCount
//        // 3 bytes Offset
//        BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &mAlltoAllvPacket.hdr.swh0;
//
//        MetaData->mSourceRank          = mMyRank;
//        MetaData->mTotalCount          = mSendCount[ DestRank ];
//        MetaData->mOffset              = mSendOffsets[ DestRank ];
//        MetaData->mBytesInPayload      = BytesToSend;
        mAlltoAllvPacket.hdr.swh0.extra = mMyRank ; 
        mAlltoAllvPacket.hdr.swh0.fcn = (unsigned int)(Actor_AllToAllV) >> 2 ;
        mAlltoAllvPacket.hdr.swh1.arg = mSendOffsetIn[ DestRank ] ;
        
        char* BlockToSend = ((char *)mSendBuff) + (mSendOffsetIn[ DestRank ]*mSendTypeSize) + mSendOffsets[ DestRank ];

        memcpy_double( (double *) mAlltoAllvPacket.payload, (double *)BlockToSend, BytesToSend );
         //memcpy( mAlltoAllvPacket.payload, BlockToSend, BytesToSend );

          BegLogLine(PKFXLOG_ALLTOALLV) 
            << "BGLTorusInjectPacketImageApp"
            << " mMyRank=" << mMyRank
            << " DestRank=" << DestRank
            << " BytesToSend=" << BytesToSend
            << " mSendOffsets[DestRank]=" << mSendOffsets[ DestRank ]
            << " mSendCount[DestRank]=" << mSendCount[ DestRank ] 
//            << " MetaData->mOffset=" << MetaData->mOffset
//            << " MetaData->mTotalCount=" << MetaData->mTotalCount
            << " mRecvTypeSize=" << mRecvTypeSize
            << " NextFifoToSend=" << NextFifoToSend
            << " &mAlltoAllvPacket=" << &mAlltoAllvPacket
            << EndLogLine ; 
        BGLTorusInjectPacketImageApp( NextFifoToSend,
                                      &mAlltoAllvPacket );
          BegLogLine(PKFXLOG_ALLTOALLV) 
            << "BGLTorusInjectPacketImageApp returned"
            << EndLogLine ; 
        mTotalSentPackets += 1 ;    

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
   if ( 
//   mRecvCountDone != mGroupSize )
           ( DriveCount != StartDriveCount+mTotalSentPackets ) )
   {
   	
#ifndef PK_PHASE5_SPLIT_COMMS
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }
#else
      // If we have nothing to receive try to send
//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus0, this );
//      if( !_ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus, this ) )
//        break;

//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

#endif

//      assert( mRecvCountDone <= mGroupSize );
   }
#else    
    int PollCount = PACKET_POLL_COUNT ;
    while( PollCount && (  DriveCount != StartDriveCount+mTotalSentPackets  ) )//
//    mRecvCountDone != mGroupSize) )
      {
      // Read the status.
#ifndef PK_PHASE5_SPLIT_COMMS
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }
#else
      // If we have nothing to receive try to send
//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

      if( !_ts_Poll0_OnePassReceive_Alltoallv_Chunk_Put_Actor< NON_ALIGNED1 >( & FifoStatus, this ) )
        break;

//       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
//       fflush( stdout );

#endif

//      assert( mRecvCountDone <= mGroupSize );
      PollCount--;
      }
#endif      
    }
   BegLogLine(PKFXLOG_ALLTOALLV) 
  	  << "PacketAlltoAllv_Actors::ExecuteSingleCoreSimple1 complete" 
  	  << EndLogLine ;
    
  }

template< int NON_ALIGNED1 >
void PacketAlltoAllv_Actors::ExecuteSingleCore1( void* aSendBuff, void* aRecvBuff )
  {
  #ifdef PK_PHASE5_SPLIT_COMMS
    unsigned long dummy1;
    unsigned long dummy2;
    GlobalTreeReduce( &dummy1, &dummy2, sizeof( unsigned long ), _BGL_TREE_OPCODE_MAX );
  #else
    BGLPartitionBarrierCore();
  #endif

//   printf( "ExecuteSingleCore1: Got Here: %d\n", __LINE__ );
//   fflush( stdout );

  mSendBuff = aSendBuff;
  mRecvBuff = aRecvBuff;

  // alltoallv_debug = 1;

  CurrentA2A = this ;
  BegLogLine(PKFXLOG_ALLTOALLV)
    << "PacketAlltoAllv_Actors::ExecuteSingleCore1 mSendCount=" << mSendCount
    << " mBlockShift=" << mBlockShift 
    << EndLogLine ;

  // memcpy data local to this node.
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
  
  // We are going to try to terminate when we have sent everything and received as much as we have sent;
  // i.e. 'symmetrical' all-to-alls
  int StartDriveCount = DriveCount ; 
  
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
//         ( mRecvCountDone != mGroupSize ) )
           ( DriveCount != StartDriveCount+mTotalSentPackets ) )
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

//          BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &HdrPtr->swh0;

//          int BytesToSend = MetaData->mBytesInPayload;
          BegLogLine(PKFXLOG_ALLTOALLV) 
            << "Sending packet, extra=" << HdrPtr->swh0.extra 
            << " fcn=0x" << (void *) (HdrPtr->swh0.fcn << 2)
            << " arg=" << HdrPtr->swh1.arg
            << " mMyRank=" << mMyRank
            << " DestRank=" << DestRank
//            << " BytesToSend=" << BytesToSend
//            << " MetaData->mOffset=" << MetaData->mOffset
//            << " MetaData->mTotalCount=" << MetaData->mTotalCount
            << " mRecvTypeSize=" << mRecvTypeSize
            << EndLogLine ; 
//          assert( BytesToSend );

//          assert( (MetaData->mOffset + BytesToSend) <= ( MetaData->mTotalCount * mRecvTypeSize ) );

          assert( DestRank >= 0 && DestRank < mGroupSize );
//          char* BlockToSend = ((char *)mSendBuff)  + (mSendOffsetIn[ DestRank ] * mSendTypeSize) + MetaData->mOffset;
		  unsigned int arg=HdrPtr->swh1.arg ;
//		  int SourceRank = arg >> mBlockShift ; 
		  int SourceSequence = arg & ( ( 1 << mBlockShift ) - 1 ) ;
		  int ByteCount = mSendBytes >> mBlockShift ;
		  char* BlockToSend = ((char *)mSendBuff)  + (mSendOffsetIn[ DestRank ] * mSendTypeSize) + SourceSequence*ByteCount;
		  BegLogLine(PKFXLOG_ALLTOALLV)
			  << "Dest rank=" << DestRank
			  << " Source sequence=" << SourceSequence
			  << " mSendBytes=" << mSendBytes 
			  << " mSendBuff=" << mSendBuff
			  << " mSendOffsetIn[ DestRank ]=" << mSendOffsetIn[ DestRank ]
			  << " mSendTypeSize=" << mSendTypeSize
			  << " Proposing to send " << ByteCount
			  << " bytes from address 0x" << (void *) BlockToSend
		      << EndLogLine ; 	

          // assert( ((unsigned long)BlockToSend & 0xf) == 0);

		  if ( ((unsigned int)BlockToSend) & 0x0f )
		  {
  	        // Data are not be on 16-byte boundaries; arrange to 'memcpy' it all
	          memcpy( mAlltoAllvPacket.payload, BlockToSend, ByteCount );
	
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
   if ( ( LinkListsDone  != mActiveLinkCount ) ||
           ( DriveCount != StartDriveCount+mTotalSentPackets ) )
//   ( mRecvCountDone != mGroupSize )
   {
   	
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Drain_Actor< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }
   }
#else
    int PollCount = PACKET_POLL_COUNT ;
    while( PollCount && ( ( LinkListsDone  != mActiveLinkCount ) ||
           ( DriveCount != StartDriveCount+mTotalSentPackets ) ) )
//           mRecvCountDone != mGroupSize) )
      {
      // Read the status. TEMPORARY
      if( mMyCore == 0 )
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus0_nosync( & FifoStatus0 );
          _ts_Poll0_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus0, this );
          }
        }
      else
        {
        if( mNumberOfFifosInUse == 6 )
          {
          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
          _ts_PollBoth_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          }
        else
          {
          _ts_GetStatus1_nosync( & FifoStatus1 );
          _ts_Poll1_OnePassReceive_Alltoallv_Put_Actor< NON_ALIGNED1 >( & FifoStatus1, this );
          }
        }

//      assert( mRecvCountDone <= mGroupSize );
      PollCount--;
      }
#endif      
    }

  // Barrier the phase. We need to receive packets from another phase
// BGLPartitionBarrierComputepaCore0();
  }

void PacketAlltoAllv_Actors::ExecuteSingleCore( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                         void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                         GroupContainer_Actors* aGroup, int aligned, int debug )
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

void PacketAlltoAllv_Actors::ExecuteSingleCoreSimple( void* aSendBuf, int aSendCount[], int* aSendOffset, int aSendTypeSize,
                                               void* aRecvBuf, int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                                               GroupContainer_Actors* aGroup, int aligned, int debug,
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

void PacketAlltoAllv_Actors::ReportFifoHistogram(void)
{
	fStatus.display() ;
}
  
