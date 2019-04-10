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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          packetlayer_alltoall.hpp
 *
 * Purpose:         Implement FIFO access for xlC-buildable all-to-all.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         20060306 TJCW Created from packet_alltoallv.cpp
 *
 ***************************************************************************/
#if !defined(__PACKETLAYER_ALLTOALLV__)
#define __PACKETLAYER_ALLTOALLV__

#ifndef PKFXLOG_PACKET_ALLTOALLV
#define PKFXLOG_PACKET_ALLTOALLV (0)
#endif
#ifndef PKFXLOG_PACKET_ALLTOALLV_SUMMARY
#define PKFXLOG_PACKET_ALLTOALLV_SUMMARY (0)
#endif
#if defined(FFT_COMPILE) || !defined(FFT_SEPARATE)

#include <spi/packet_alltoallv.hpp>
#include <spi/BGLTorusAppSupport.c>

// xlC builtins to force parallel loads and stores
// extern "builtin" double _Complex __lfpd(volatile double *) ;
// extern "builtin" void __stfpd(volatile double *, double _Complex) ;
// The compiler now has these declared without the 'volatile', which may or may not work ...
extern "builtin" void __sync(void) ;

int alltoallv_debug = 1;
/// #define DEBUG_ALL2ALL

#define PACKET_POLL_COUNT 1
#define PACKET_POLL_COUNT_ALIGNED 1
#define PACKETS_TO_INJECT 6

typedef double * fifoType ;
class fifoStatus
{
  public: 
  int sendFifoHistogram[256] ;
  int sendSimpleFifoHistogram[256] ;
  int receiveFifoHistogram[256] ;
#if !defined(REPORT_FIFO_HISTOGRAM)	
  void recordSend(unsigned int statusValue)
  {
  }
  void recordSendSimple(unsigned int statusValue)
  {
  }
  void recordReceive(unsigned int statusValue)
  {
  }
  void display(void)
  {
  }
#else
  void recordSend(unsigned int statusValue)
  {
    sendFifoHistogram[statusValue] += 1 ;
  }
  void recordSendSimple(unsigned int statusValue)
  {
    sendSimpleFifoHistogram[statusValue] += 1 ;
  }
  void recordReceive(unsigned int statusValue)
  {
    receiveFifoHistogram[statusValue] += 1 ;
  }
  void display(void)
  {
    for(unsigned int a=0;a<32;a+=1)
    {
      BegLogLine(1)
        << "sendFifoHistogram[" << a
        << "]=" << sendFifoHistogram[a]
        << "sendSimpleFifoHistogram[" << a
        << "]=" << sendSimpleFifoHistogram[a]
        << "receiveFifoHistogram[" << a
        << "]=" << receiveFifoHistogram[a]
        << EndLogLine ;
    }
  }
#endif	
} ;
static fifoStatus fStatus ;


int Verbose = 0;

inline static void ts_GetStatusBoth(_BGL_TorusFifoStatus *stat0, _BGL_TorusFifoStatus *stat1 )
{
  __sync() ; 
  double _Complex status0 = __lfpd((fifoType)TS0_S0) ;
  double _Complex status1 = __lfpd((fifoType)TS0_S1) ;
  *(double _Complex *) stat0 = status0 ;
  *(double _Complex *) stat1 = status1 ;
}
//static void _ts_GetStatusBoth( _BGL_TorusFifoStatus *stat0, _BGL_TorusFifoStatus *stat1 )
//{
////   assert( ((unsigned long)stat & 0x000000F) == 0 );
////   assert( stat );
//
//   asm volatile( "msync;"
//                 "lfpdx   0,0,%0;"
//                 "lfpdx   1,0,%2;"
//                 "stfpdx  0,0,%1;"
//                 "stfpdx  1,0,%3;"
//                 :
//                 : "b" (TS0_S0),
//                   "r" (stat0),
//                   "b" (TS0_S1),
//                   "r" (stat1)
//                 : "memory", "fr0", "fr1" );
//}

static void RandomizeLinks( int* aLinks, int aCount, int aMyRank )
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



//_BGL_TorusPktPayload Chunk1;

class BGL_AllToAll_TorusPktHdr
{
  public: 
  _BGL_Torus_HdrHW0 hwh0 ; 
  _BGL_Torus_HdrHW1 hwh1 ; 
  BGL_AlltoallvPacketMetaData PacketMetaData ;
} ;
typedef union {
  double _Complex fifoValue ;
  BGL_AllToAll_TorusPktHdr PacketHeader ;
} BGL_AllToAll_TorusPktHdr_Union ; 

static inline void BGLTorusInjectPacketSeparateHeaderApp(
        _BGL_TorusFifo *fifo,  // Injection FIFO to use (Must have space available!)
        _BGL_TorusPktHdr *header, // Prebuilt header
        void* dst // Data (must be appropriately aligned)
         )  // Address of ready-to-go packet
{
   int chunks = header->hwh0.Chunks;
   char * dstc = (char *) dst ;
   // Push out the header and the first 16 bytes
   __stfpd((fifoType)fifo, *(double _Complex *) header) ;
   __stfpd((fifoType)fifo, *(double _Complex *) dstc );
   
   // Push out 64-byte blocks
   dstc += 16 ; 
   while ( chunks > 1 )
   {
      chunks -= 2 ; 
        double _Complex Chunk0 = *(double _Complex *)(dstc) ;
      double _Complex Chunk1 = *(double _Complex *)(dstc+16) ;
      double _Complex Chunk2 = *(double _Complex *)(dstc+32) ;
      double _Complex Chunk3 = *(double _Complex *)(dstc+48) ;
       __stfpd((fifoType)fifo,Chunk0) ;
       __stfpd((fifoType)fifo,Chunk1) ;
       __stfpd((fifoType)fifo,Chunk2) ;
       __stfpd((fifoType)fifo,Chunk3) ;
      dstc += 64 ;
   }
   // If there's a 32-byte block to go, push it out too
   if ( chunks )
   {
        double _Complex Chunk0 = *(double _Complex *)(dstc) ;
      double _Complex Chunk1 = *(double _Complex *)(dstc+16) ;
      __stfpd((fifoType)fifo,Chunk0) ;
      __stfpd((fifoType)fifo,Chunk1) ;
   }
}

// Put: move entire packet from fifo to buf
// Aligned version
template< int NON_ALIGNED1 >
inline
int PacketAlltoAllv::_ts_AppReceiver_Alltoallv_Put( _BGL_TorusFifo *fifo, PacketAlltoAllv* aThis )
  {
//       _BGL_TorusPkt *pkt = (_BGL_TorusPkt *)addr;
//       Bit8 *dst          = (Bit8 *)addr;
  BGL_AllToAll_TorusPktHdr_Union hdr ; 
  int chunks, c2;
  register int u = 16;

//#ifdef DEBUG_ALL2ALL
#if 1
  aThis->mTotalRecvPackets++;
#endif

  assert( ((unsigned long)&hdr & 0x000000F) == 0 );

// Pull in the packet header and the first chunk of payload (there must be some ...)
  hdr.fifoValue = __lfpd((fifoType)fifo) ;
  double _Complex FirstChunk = __lfpd((fifoType)fifo) ;
  BGL_AlltoallvPacketMetaData PacketMetaData =  hdr.PacketHeader.PacketMetaData ;
  

//    printf("(I) _ts_AppReceiver_Put(0x%08lx): *stat = %d, swh0.arg0 = %4ld, swh1.arg = %4ld, swa = %d\n",
//            (unsigned long)fifo, *stat, hdr->swh0.arg0, hdr->swh1.arg, hdr->hwh0.SW_Avail );

  chunks = hdr.PacketHeader.hwh0.Chunks;  // hw hdr says (chunks - 1), we did chunk 0 already
  int SourceRank       = PacketMetaData.mSourceRank;
  int TotalCountToRecv = PacketMetaData.mTotalCount;
  int Offset           = PacketMetaData.mOffset;
  int BytesInPayload   = PacketMetaData.mBytesInPayload;

  BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    << "_ts_AppReceiver_Alltoallv_Put fifo=" << fifo
    << " SourceRank=" << SourceRank
    << " TotalCountToRecv=" << TotalCountToRecv
    << " Offset=" << Offset
    << " BytesInPayload=" <<  BytesInPayload
    << " chunks=" << chunks 
    << EndLogLine ;
  assert( SourceRank != aThis->mMyRank );
  assert( BytesInPayload );

//   if( !( Offset + BytesInPayload <= TotalCountToRecv * aThis->mRecvTypeSize ) )
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

  int BytesRecvFromNode=aThis->mBytesRecvFromNode[ SourceRank ] +BytesInPayload ;
  aThis->mBytesRecvFromNode[ SourceRank ] = BytesRecvFromNode;

  int ReceiveTypeSize = aThis->mRecvTypeSize ;
  char * dst = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*ReceiveTypeSize) + Offset;
  int BytesToGoFromNode = (TotalCountToRecv * ReceiveTypeSize) - BytesRecvFromNode ;
  int CompletionFromNode = ( BytesToGoFromNode == 0 ) ;
  aThis->mRecvCountDone += CompletionFromNode ;
  aThis->mRecvCount[ SourceRank ] = TotalCountToRecv;
  
#if   PKFXLOG_PACKET_ALLTOALLV
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
    << " dst=" << (void *)dst 
    << " BytesToGoFromNode=" << BytesToGoFromNode
    << " mRecvCountDone=" << aThis->mRecvCountDone
  << " mRecvDoneBits=" << (void *)(aThis->mRecvDoneBits) 
    << EndLogLine ;
//  if( (TotalCountToRecv * ReceiveTypeSize) == BytesRecvFromNode )
//    {
//    aThis->mRecvCount[ SourceRank ] = TotalCountToRecv;
//    aThis->mRecvCountDone++;
//    BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
//      << "Completion from SourceRank=" << SourceRank
//      << " mRecvCountDone=" << aThis->mRecvCountDone
//      << EndLogLine ;
//
//    } else {
//      BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
//        << "Not complete from SourceRank=" << SourceRank
//        << " bytes to go=" << (TotalCountToRecv * ReceiveTypeSize)-BytesRecvFromNode
//        << EndLogLine ;
//    }
  if ( ( ((unsigned long)dst & 0x7) == 0 ) && ( BytesInPayload %  16 == 0 ) )
  {
     assert( ((unsigned long)dst & 0x7) == 0 );

  // Now we know where the first 16 bytes of the data can go ...
    *(double _Complex *) dst = FirstChunk ; 
    dst += 16 ; 
    BytesInPayload -= 16 ;
    
  // The full-speed loop takes 64 bytes at a time
    while ( BytesInPayload > 48 )
    {
      double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
      *(double _Complex *) dst = Chunk0 ;
      *(double _Complex *) (dst+16) = Chunk1 ;
      *(double _Complex *) (dst+32) = Chunk2 ;
      *(double _Complex *) (dst+48) = Chunk3 ;
      dst += 64 ;
      BytesInPayload -= 64  ;
    }
  // And then we will have 0, 1, or 2 16-byte lines; 1 or 2 chunks  
    if ( BytesInPayload > 0 )
    {
      double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
      if ( BytesInPayload > 32 )
      {
          double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
          double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
          *(double _Complex *) dst = Chunk0 ;
          *(double _Complex *) (dst+16) = Chunk1 ;
          *(double _Complex *) (dst+32) = Chunk2 ; 	  	
      } else {
          *(double _Complex *) dst = Chunk0 ;
          if ( BytesInPayload > 16 )
          {
             *(double _Complex *) (dst+16) = Chunk1 ;
          }
      }
    }
  } else {
    double _Complex tmpBuffer[16] ; 
    tmpBuffer[0] = FirstChunk ;
    int remainingPayload = BytesInPayload-16 ; 
    int tmpIndex = 1 ;
  // The full-speed loop takes 64 bytes at a time
    while ( remainingPayload > 48 )
    {
      double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
      tmpBuffer[tmpIndex] = Chunk0 ;
      tmpBuffer[tmpIndex+1] = Chunk1 ;
      tmpBuffer[tmpIndex+2] = Chunk2 ;
      tmpBuffer[tmpIndex+3] = Chunk3 ;
      tmpIndex += 4 ;
      remainingPayload -= 64  ;
    }
  // And then we will have 0, 1, or 2 chunks left to go  
    if ( remainingPayload > 0 )
    {
      double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
      double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
      if ( remainingPayload > 32 )
      {
        double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
        tmpBuffer[tmpIndex] = Chunk0 ;
        tmpBuffer[tmpIndex+1] = Chunk1 ;
        tmpBuffer[tmpIndex+2] = Chunk2 ; 	  	
      } else {
        tmpBuffer[tmpIndex] = Chunk0 ;
        tmpBuffer[tmpIndex+1] = Chunk1 ;
      }
    }
    memcpy(dst,tmpBuffer,BytesInPayload) ; 
  }



  return( chunks );
  }
// Put: move entire fifo to buffers
#if defined(DRAIN_BY_MESSAGE)
template< int NON_ALIGNED1 >
inline
void PacketAlltoAllv::_ts_AppReceiver_Alltoallv_Drain( _BGL_TorusFifo *fifo, PacketAlltoAllv* aThis, int ChunksInFifo )
  {
  BGL_AllToAll_TorusPktHdr_Union hdr ; 

  BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    << "_ts_AppReceiver_Alltoallv_Drain fifo=" << fifo
    << " ChunksInFifo=" << ChunksInFifo
    << EndLogLine ;
  assert( ((unsigned long)&hdr & 0x000000F) == 0 );
  int PacketsInFifo = 0 ; 
  do { 
  // Pull in the packet header and the first chunk of payload (there must be some ...)
    hdr.fifoValue = __lfpd((fifoType)fifo) ;
    double _Complex FirstChunk = __lfpd((fifoType)fifo) ;
    BGL_AlltoallvPacketMetaData PacketMetaData =  hdr.PacketHeader.PacketMetaData ;
    
  
  //    printf("(I) _ts_AppReceiver_Put(0x%08lx): *stat = %d, swh0.arg0 = %4ld, swh1.arg = %4ld, swa = %d\n",
  //            (unsigned long)fifo, *stat, hdr->swh0.arg0, hdr->swh1.arg, hdr->hwh0.SW_Avail );
  
    int chunks = hdr.PacketHeader.hwh0.Chunks;  // hw hdr says (chunks - 1), we did chunk 0 already
    int SourceRank       = PacketMetaData.mSourceRank;
    int TotalCountToRecv = PacketMetaData.mTotalCount;
    int Offset           = PacketMetaData.mOffset;
    int BytesInPayload   = PacketMetaData.mBytesInPayload;
  
    PacketsInFifo += 1 ; 
    ChunksInFifo -= (chunks+1) ; // i.e. the number of chunks that will be in the fifo after this packet has been taken out
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
      << "_ts_AppReceiver_Alltoallv_Put fifo=" << fifo
      << " SourceRank=" << SourceRank
      << " TotalCountToRecv=" << TotalCountToRecv
      << " Offset=" << Offset
      << " BytesInPayload=" <<  BytesInPayload
      << " chunks=" << chunks 
      << EndLogLine ;
    assert( SourceRank != aThis->mMyRank );
    assert( BytesInPayload );
  
  
    assert( Offset + BytesInPayload <= TotalCountToRecv * aThis->mRecvTypeSize );
    assert( SourceRank >=0 && SourceRank < aThis->mGroupSize );
  
    int BytesRecvFromNode=aThis->mBytesRecvFromNode[ SourceRank ] +BytesInPayload ;
    aThis->mBytesRecvFromNode[ SourceRank ] = BytesRecvFromNode;
  
    int ReceiveTypeSize = aThis->mRecvTypeSize ;
    char * dst = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*ReceiveTypeSize) + Offset;
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
      << " dst=" << (void *)dst 
      << " BytesToGoFromNode=" << BytesToGoFromNode
      << " mRecvDoneBits=" << (void *)(aThis->mRecvDoneBits) 
      << EndLogLine ;
    if ( ( ((unsigned long)dst & 0x7) == 0 ) && ( BytesInPayload %  16 == 0 ) )
    {
  
    // Now we know where the first 16 bytes of the data can go ...
      *(double _Complex *) dst = FirstChunk ; 
      dst += 16 ; 
      BytesInPayload -= 16 ;
      
    // The full-speed loop takes 64 bytes at a time
      while ( BytesInPayload > 48 )
      {
        double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
        *(double _Complex *) dst = Chunk0 ;
        *(double _Complex *) (dst+16) = Chunk1 ;
        *(double _Complex *) (dst+32) = Chunk2 ;
        *(double _Complex *) (dst+48) = Chunk3 ;
        dst += 64 ;
        BytesInPayload -= 64  ;
      }
    // And then we will have 0, 1, or 2 16-byte lines; 1 or 2 chunks  
      if ( BytesInPayload > 0 )
      {
        double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
        if ( BytesInPayload > 32 )
        {
            double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
            double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
            *(double _Complex *) dst = Chunk0 ;
            *(double _Complex *) (dst+16) = Chunk1 ;
            *(double _Complex *) (dst+32) = Chunk2 ; 	  	
        } else {
            *(double _Complex *) dst = Chunk0 ;
            if ( BytesInPayload > 16 )
            {
               *(double _Complex *) (dst+16) = Chunk1 ;
            }
        }
      }
    } else {
      double _Complex tmpBuffer[16] ; 
      tmpBuffer[0] = FirstChunk ;
      int remainingPayload = BytesInPayload-16 ; 
      int tmpIndex = 1 ;
    // The full-speed loop takes 64 bytes at a time
      while ( remainingPayload > 48 )
      {
        double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
        tmpBuffer[tmpIndex] = Chunk0 ;
        tmpBuffer[tmpIndex+1] = Chunk1 ;
        tmpBuffer[tmpIndex+2] = Chunk2 ;
        tmpBuffer[tmpIndex+3] = Chunk3 ;
        tmpIndex += 4 ;
        remainingPayload -= 64  ;
      }
    // And then we will have 0, 1, or 2 chunks left to go  
      if ( remainingPayload > 0 )
      {
        double _Complex Chunk0 = __lfpd((fifoType)fifo) ;
        double _Complex Chunk1 = __lfpd((fifoType)fifo) ;
        if ( remainingPayload > 32 )
        {
          double _Complex Chunk2 = __lfpd((fifoType)fifo) ;
          double _Complex Chunk3 = __lfpd((fifoType)fifo) ;
          tmpBuffer[tmpIndex] = Chunk0 ;
          tmpBuffer[tmpIndex+1] = Chunk1 ;
          tmpBuffer[tmpIndex+2] = Chunk2 ; 	  	
        } else {
          tmpBuffer[tmpIndex] = Chunk0 ;
          tmpBuffer[tmpIndex+1] = Chunk1 ;
        }
      }
      memcpy(dst,tmpBuffer,BytesInPayload) ; 
    }
  
  } while ( ChunksInFifo > 0 ) ; 
  
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    << "_ts_AppReceiver_Alltoallv_Drain PacketsInFifo=" << PacketsInFifo
    << EndLogLine ;
  
  aThis->mTotalRecvPackets += PacketsInFifo ;

  }
#else
template< int NON_ALIGNED1 >
inline
void PacketAlltoAllv::_ts_AppReceiver_Alltoallv_Drain( _BGL_TorusFifo *fifo, PacketAlltoAllv* aThis, int ChunksInFifo )
  {
    enum { k_DrainDepth = _BGL_TORUS_RECEPTION_FIFO_CHUNKS } ; 
    
    assert(ChunksInFifo <=  k_DrainDepth) ;
    // Get all the lines
    double _Complex HalfChunks[2*k_DrainDepth] ;
    int x=ChunksInFifo & 1 ;
    if ( x != 0 )
    {
    double _Complex HalfChunk0 = __lfpd((fifoType)fifo) ;
    double _Complex HalfChunk1 = __lfpd((fifoType)fifo) ;
      HalfChunks[0] = HalfChunk0 ; 
      HalfChunks[1] = HalfChunk1 ;
    }
    for ( int a=0;a<ChunksInFifo/2;a+=1)
    {
    double _Complex HalfChunk0 = __lfpd((fifoType)fifo) ;
    double _Complex HalfChunk1 = __lfpd((fifoType)fifo) ;
    double _Complex HalfChunk2 = __lfpd((fifoType)fifo) ;
    double _Complex HalfChunk3 = __lfpd((fifoType)fifo) ;
      HalfChunks[2*x]=HalfChunk0 ;
      HalfChunks[2*x+1]=HalfChunk1 ;
      HalfChunks[2*x+2]=HalfChunk2 ;
      HalfChunks[2*x+3]=HalfChunk3 ;
      x+=2 ;
    }
    
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
  
    PacketsInFifo += 1 ; 
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
      << "_ts_AppReceiver_Alltoallv_Put"
      << " SourceRank=" << SourceRank
      << " TotalCountToRecv=" << TotalCountToRecv
      << " Offset=" << Offset
      << " BytesInPayload=" <<  BytesInPayload
      << " Chunks=" << Chunks 
      << EndLogLine ;
    assert( SourceRank != aThis->mMyRank );
    assert( BytesInPayload );
  
  
    assert( Offset + BytesInPayload <= TotalCountToRecv * aThis->mRecvTypeSize );
    assert( SourceRank >=0 && SourceRank < aThis->mGroupSize );
  
    int BytesRecvFromNode=aThis->mBytesRecvFromNode[ SourceRank ] +BytesInPayload ;
    aThis->mBytesRecvFromNode[ SourceRank ] = BytesRecvFromNode;
  
    int ReceiveTypeSize = aThis->mRecvTypeSize ;
    char * dst = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*ReceiveTypeSize) + Offset;
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
      << " dst=" << (void *)dst 
      << " BytesToGoFromNode=" << BytesToGoFromNode
      << " mRecvDoneBits=" << (void *)(aThis->mRecvDoneBits) 
      << " mRecvCountDone=" << aThis->mRecvCountDone 
      << EndLogLine ;
    int PayloadHalfChunkIndex = 2*ThisPacketChunkIndex+1 ;
    int BytesInPayloadThis = BytesInPayload ;
    // Set up for next packet
    ThisPacketChunkIndex += (Chunks+1) ; 
      Chunks = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->hwh0.Chunks ;
      SourceRank = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mSourceRank ;
      TotalCountToRecv = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mTotalCount ;
      Offset = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mOffset ;
      BytesInPayload = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+2*ThisPacketChunkIndex))->PacketMetaData.mBytesInPayload ;
      // and drop 'this' packet into is proper store
    if ( ( ((unsigned long)dst & 0x7) == 0 ) && ( BytesInPayloadThis %  16 == 0 ) )
    { 
      while ( BytesInPayloadThis > 48 )
      {
        double _Complex Chunk0 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex)) ;
        double _Complex Chunk1 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+1));
        double _Complex Chunk2 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+2));
        double _Complex Chunk3 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+3));
        PayloadHalfChunkIndex += 4 ;
        *(double _Complex *) dst = Chunk0 ;
        *(double _Complex *) (dst+16) = Chunk1 ;
        *(double _Complex *) (dst+32) = Chunk2 ;
        *(double _Complex *) (dst+48) = Chunk3 ;
        dst += 64 ;
        BytesInPayloadThis -= 64  ;
        
      }
      double _Complex Chunk0 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex)) ;
      double _Complex Chunk1 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+1));
      double _Complex Chunk2 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+2));
        if ( BytesInPayloadThis > 0 )
        {
        *(double _Complex *) dst = Chunk0 ;
          if ( BytesInPayloadThis > 16 )
          {
           *(double _Complex *) (dst+16) = Chunk1 ;
           if ( BytesInPayloadThis > 32 )
           {
              *(double _Complex *) (dst+32) = Chunk2 ;
           }	
          }
        }
    } else {
      memcpy(dst,HalfChunks+PayloadHalfChunkIndex,BytesInPayloadThis) ; 
    }	  	
  }  
  
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    << "_ts_AppReceiver_Alltoallv_Drain PacketsInFifo=" << PacketsInFifo
    << EndLogLine ;
  
  aThis->mTotalRecvPackets += PacketsInFifo ;
    
  }
#endif  
  

template<int NON_ALIGNED1>
inline
void
PacketAlltoAllv::_ts_PollBoth_OnePassReceive_Alltoallv_Put( _BGL_TorusFifoStatus *statF0,
                                                            _BGL_TorusFifoStatus *statF1,
                                                            PacketAlltoAllv* aThis )
  {
    _BGL_TorusFifoStatus StatusF0 = *statF0 ;
    int sF0r0 = StatusF0.r0 ;
    int sF0r1 = StatusF0.r1 ;
    int sF0r2 = StatusF0.r2 ;
    int sF0r3 = StatusF0.r3 ;
    int sF0r4 = StatusF0.r4 ;
    int sF0r5 = StatusF0.r5 ;
    fStatus.recordReceive(sF0r0) ;
    fStatus.recordReceive(sF0r1) ;
    fStatus.recordReceive(sF0r2) ;
    fStatus.recordReceive(sF0r3) ;
    fStatus.recordReceive(sF0r4) ;
    fStatus.recordReceive(sF0r5) ;
    
    int zF0r0 = ( sF0r0 == 0 ) ;
    int zF0r1 = ( sF0r1 == 0 ) ;
    int zF0r2 = ( sF0r2 == 0 ) ;
    int zF0r3 = ( sF0r3 == 0 ) ;
    int zF0r4 = ( sF0r4 == 0 ) ;
    int zF0r5 = ( sF0r5 == 0 ) ;
    int mF0 = (zF0r0 << 0) | (zF0r1 << 1) | (zF0r2 << 2) | (zF0r3 << 3) | (zF0r4 << 4) | ( zF0r5 << 5 );
    if ( ! (mF0 & 0x01) )
//  	if ( sF0r0 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis);
    }
    if ( !(mF0 & 0x02) )
//  	if ( sF0r1 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R1, aThis);
    }
  if ( !(mF0 & 0x04) )
//  	if ( sF0r2 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R2, aThis);
    }
  if ( !(mF0 & 0x08) )
//  	if ( sF0r3 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R3, aThis);
    }
  if ( !(mF0 & 0x10) )
//  	if ( sF0r4 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R4, aThis);
    }
  if ( !(mF0 & 0x20) )
//  	if ( sF0r5 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R5, aThis);
    }
    _BGL_TorusFifoStatus StatusF1 = *statF1 ;
    int sF1r0 = StatusF1.r0 ;
    int sF1r1 = StatusF1.r1 ;
    int sF1r2 = StatusF1.r2 ;
    int sF1r3 = StatusF1.r3 ;
    int sF1r4 = StatusF1.r4 ;
    int sF1r5 = StatusF1.r5 ;
    fStatus.recordReceive(sF1r0) ;
    fStatus.recordReceive(sF1r1) ;
    fStatus.recordReceive(sF1r2) ;
    fStatus.recordReceive(sF1r3) ;
    fStatus.recordReceive(sF1r4) ;
    fStatus.recordReceive(sF1r5) ;
    
    int zF1r0 = ( sF1r0 == 0 ) ;
    int zF1r1 = ( sF1r1 == 0 ) ;
    int zF1r2 = ( sF1r2 == 0 ) ;
    int zF1r3 = ( sF1r3 == 0 ) ;
    int zF1r4 = ( sF1r4 == 0 ) ;
    int zF1r5 = ( sF1r5 == 0 ) ;
  int mF1 = (zF1r0 << 0) | (zF1r1 << 1) | (zF1r2 << 2) | (zF1r3 << 3) | (zF1r4 << 4) | ( zF1r5 << 5 );
    if ( !(mF1 & 0x01 ) )
//  	if ( sF1r0 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R0, aThis);
    }
  if ( !(mF1 & 0x02 ) )
//  	if ( sF1r1 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R1, aThis);
    }
  if ( !(mF1 & 0x04 ) )
//  	if ( sF1r2 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R2, aThis);
    }
  if ( !(mF1 & 0x08 ) )
//  	if ( sF1r3 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R3, aThis);
    }
  if ( !(mF1 & 0x10 ) )
//  	if ( sF1r4 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R4, aThis);
    }
  if ( !(mF1 & 0x20 ) )
//  	if ( sF1r5 )
    {
      _ts_AppReceiver_Alltoallv_Put<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R5, aThis);
    }
    
  }

#if !defined(DRAIN_BY_NODE)
template<int NON_ALIGNED1>
inline
int
PacketAlltoAllv::_ts_PollBoth_OnePassReceive_Alltoallv_Drain( _BGL_TorusFifoStatus *statF0,
                                                            _BGL_TorusFifoStatus *statF1,
                                                            PacketAlltoAllv* aThis )
  {
    int result = 0 ; 
    _BGL_TorusFifoStatus StatusF0 = *statF0 ;
    int sF0r0 = StatusF0.r0 ;
    int sF0r1 = StatusF0.r1 ;
    int sF0r2 = StatusF0.r2 ;
    int sF0r3 = StatusF0.r3 ;
    int sF0r4 = StatusF0.r4 ;
    int sF0r5 = StatusF0.r5 ;
    fStatus.recordReceive(sF0r0) ;
    fStatus.recordReceive(sF0r1) ;
    fStatus.recordReceive(sF0r2) ;
    fStatus.recordReceive(sF0r3) ;
    fStatus.recordReceive(sF0r4) ;
    fStatus.recordReceive(sF0r5) ;
    int zF0r0 = ( sF0r0 == 0 ) ;
    int zF0r1 = ( sF0r1 == 0 ) ;
    int zF0r2 = ( sF0r2 == 0 ) ;
    int zF0r3 = ( sF0r3 == 0 ) ;
    int zF0r4 = ( sF0r4 == 0 ) ;
    int zF0r5 = ( sF0r5 == 0 ) ;
    int mF0 = (zF0r0 << 0) | (zF0r1 << 1) | (zF0r2 << 2) | (zF0r3 << 3) | (zF0r4 << 4) | ( zF0r5 << 5 );
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV && (mF0 != 0x3f))
      << " mF0=" << mF0 
      << EndLogLine ;
    if ( ! (mF0 & 0x01) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R0, aThis, sF0r0);
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( ! (mF0 & 0x02) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R1, aThis, sF0r1 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( ! (mF0 & 0x04) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R2, aThis, sF0r2 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( ! (mF0 & 0x08) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R3, aThis, sF0r3 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( ! (mF0 & 0x10) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R4, aThis, sF0r4 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( ! (mF0 & 0x20) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS0_R5, aThis, sF0r5 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    _BGL_TorusFifoStatus StatusF1 = *statF1 ;
    int sF1r0 = StatusF1.r0 ;
    int sF1r1 = StatusF1.r1 ;
    int sF1r2 = StatusF1.r2 ;
    int sF1r3 = StatusF1.r3 ;
    int sF1r4 = StatusF1.r4 ;
    int sF1r5 = StatusF1.r5 ;
    fStatus.recordReceive(sF1r0) ;
    fStatus.recordReceive(sF1r1) ;
    fStatus.recordReceive(sF1r2) ;
    fStatus.recordReceive(sF1r3) ;
    fStatus.recordReceive(sF1r4) ;
    fStatus.recordReceive(sF1r5) ;
    int zF1r0 = ( sF1r0 == 0 ) ;
    int zF1r1 = ( sF1r1 == 0 ) ;
    int zF1r2 = ( sF1r2 == 0 ) ;
    int zF1r3 = ( sF1r3 == 0 ) ;
    int zF1r4 = ( sF1r4 == 0 ) ;
    int zF1r5 = ( sF1r5 == 0 ) ;
    int mF1 = (zF1r0 << 0) | (zF1r1 << 1) | (zF1r2 << 2) | (zF1r3 << 3) | (zF1r4 << 4) | ( zF1r5 << 5 );
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV && (mF1 != 0x3f))
      << " mF1=" << mF1 
      << EndLogLine ;
    if ( !(mF1 & 0x01 ) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R0, aThis, sF1r0);
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( !(mF1 & 0x02 ) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R1, aThis, sF1r1 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( !(mF1 & 0x04 ) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R2, aThis, sF1r2 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( !(mF1 & 0x08 ) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R3, aThis, sF1r3 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( !(mF1 & 0x10 ) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R4, aThis, sF1r4 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    if ( !(mF1 & 0x20 ) )
    {
      _ts_AppReceiver_Alltoallv_Drain<NON_ALIGNED1>( (_BGL_TorusFifo *)TS1_R5, aThis, sF1r5 );
      result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    }
    return result ;
  }
#else
static inline void BuildFifoLineAddresses(int * aFifoLineAddress, int ThisFifo, int LineCount)
{
  for (int x=0;x<LineCount;x+=1)
  {
    aFifoLineAddress[x]=ThisFifo ;
  }
}
template<int NON_ALIGNED1>
inline
int
PacketAlltoAllv::_ts_PollBoth_OnePassReceive_Alltoallv_Drain( _BGL_TorusFifoStatus *statF0,
                                                            _BGL_TorusFifoStatus *statF1,
                                                            PacketAlltoAllv* aThis )
  {
    int result = 0 ; 
    _BGL_TorusFifoStatus StatusF0 = *statF0 ;
    int sF0r0 = StatusF0.r0 ;
    int sF0r1 = StatusF0.r1 ;
    int sF0r2 = StatusF0.r2 ;
    int sF0r3 = StatusF0.r3 ;
    int sF0r4 = StatusF0.r4 ;
    int sF0r5 = StatusF0.r5 ;
    _BGL_TorusFifoStatus StatusF1 = *statF1 ;
    int sF1r0 = StatusF1.r0 ;
    int sF1r1 = StatusF1.r1 ;
    int sF1r2 = StatusF1.r2 ;
    int sF1r3 = StatusF1.r3 ;
    int sF1r4 = StatusF1.r4 ;
    int sF1r5 = StatusF1.r5 ;

    // Construct a table of FIFO addresses to get all the lines buffered
    enum { k_DrainDepth = 12*_BGL_TORUS_RECEPTION_FIFO_CHUNKS } ; 
    
    assert(sF0r0+sF0r1+sF0r2+sF0r3+sF0r4+sF0r5+sF1r0+sF1r1+sF1r2+sF1r3+sF1r4+sF1r5 <=  k_DrainDepth) ;
    int fifoLineAddresses[k_DrainDepth] ;
    BuildFifoLineAddresses(fifoLineAddresses              ,TS0_R0,sF0r0) ;
    int fifoLineIndex = sF0r0 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS0_R1,sF0r1) ;
    fifoLineIndex += sF0r1 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS0_R2,sF0r2) ;
    fifoLineIndex += sF0r2 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS0_R3,sF0r3) ;
    fifoLineIndex += sF0r3 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS0_R4,sF0r4) ;
    fifoLineIndex += sF0r4 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS0_R5,sF0r5) ;
    fifoLineIndex += sF0r5 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS1_R0,sF1r0) ;
    fifoLineIndex += sF1r0 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS1_R1,sF1r1) ;
    fifoLineIndex += sF1r1 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS1_R2,sF1r2) ;
    fifoLineIndex += sF1r2 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS1_R3,sF1r3) ;
    fifoLineIndex += sF1r3 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS1_R4,sF1r4) ;
    fifoLineIndex += sF1r4 ;
    BuildFifoLineAddresses(fifoLineAddresses+fifoLineIndex,TS1_R5,sF1r5) ;
    fifoLineIndex += sF1r5 ;
    
    
    // Get all the lines
    double _Complex HalfChunks[2*k_DrainDepth] ;
    int x=fifoLineIndex & 1 ;
    if ( x != 0 )
    {
    double _Complex HalfChunk0 = __lfpd((fifoType)fifoLineAddresses[0]) ;
    double _Complex HalfChunk1 = __lfpd((fifoType)fifoLineAddresses[0]) ;
      HalfChunks[0] = HalfChunk0 ; 
      HalfChunks[1] = HalfChunk1 ;
    }
    for ( int a=0;a<fifoLineIndex/2;a+=1)
    {
    double _Complex HalfChunk0 = __lfpd((fifoType)fifoLineAddresses[x]) ;
    double _Complex HalfChunk1 = __lfpd((fifoType)fifoLineAddresses[x]) ;
    double _Complex HalfChunk2 = __lfpd((fifoType)fifoLineAddresses[x+1]) ;
    double _Complex HalfChunk3 = __lfpd((fifoType)fifoLineAddresses[x+1]) ;
      HalfChunks[2*x]=HalfChunk0 ;
      HalfChunks[2*x+1]=HalfChunk1 ;
      HalfChunks[2*x+2]=HalfChunk2 ;
      HalfChunks[2*x+3]=HalfChunk3 ;
      x+=2 ;
    }
    
    // The lines are all in memory now; process them
    

  int PacketsInFifo = 0 ; 
  int Chunks = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->hwh0.Chunks ;
  int SourceRank = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mSourceRank ;
  int TotalCountToRecv = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mTotalCount ;
  int Offset = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mOffset ;
  int BytesInPayload = ((BGL_AllToAll_TorusPktHdr *) HalfChunks)->PacketMetaData.mBytesInPayload ;
  int ThisPacketHalfChunkIndex=0 ;
  while ( ThisPacketHalfChunkIndex < fifoLineIndex*2 )
  { 
  result = _BGL_TORUS_PKT_MAX_CHUNKS ;
    PacketsInFifo += 1 ; 
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
      << "_ts_AppReceiver_Alltoallv_Put"
      << " SourceRank=" << SourceRank
      << " TotalCountToRecv=" << TotalCountToRecv
      << " Offset=" << Offset
      << " BytesInPayload=" <<  BytesInPayload
      << " Chunks=" << Chunks 
      << EndLogLine ;
    assert( SourceRank != aThis->mMyRank );
    assert( BytesInPayload );
  
  
    assert( Offset + BytesInPayload <= TotalCountToRecv * aThis->mRecvTypeSize );
    assert( SourceRank >=0 && SourceRank < aThis->mGroupSize );
  
    int BytesRecvFromNode=aThis->mBytesRecvFromNode[ SourceRank ] +BytesInPayload ;
    aThis->mBytesRecvFromNode[ SourceRank ] = BytesRecvFromNode;
  
    int ReceiveTypeSize = aThis->mRecvTypeSize ;
    char * dst = ((char*)aThis->mRecvBuff) + (aThis->mRecvOffset[ SourceRank ]*ReceiveTypeSize) + Offset;
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
      << " dst=" << (void *)dst 
      << " BytesToGoFromNode=" << BytesToGoFromNode
      << " mRecvDoneBits=" << (void *)(aThis->mRecvDoneBits) 
      << " mRecvCountDone=" << aThis->mRecvCountDone 
      << EndLogLine ;
    int PayloadHalfChunkIndex = ThisPacketHalfChunkIndex+1 ;
    if ( ( ((unsigned long)dst & 0x7) == 0 ) && ( BytesInPayload %  16 == 0 ) )
    { 
      while ( BytesInPayload > 48 )
      {
        double _Complex Chunk0 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex)) ;
        double _Complex Chunk1 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+1));
        double _Complex Chunk2 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+2));
        double _Complex Chunk3 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+3));
        PayloadHalfChunkIndex += 4 ;
        *(double _Complex *) dst = Chunk0 ;
        *(double _Complex *) (dst+16) = Chunk1 ;
        *(double _Complex *) (dst+32) = Chunk2 ;
        *(double _Complex *) (dst+48) = Chunk3 ;
        dst += 64 ;
        BytesInPayload -= 64  ;
        
      }
      double _Complex Chunk0 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex)) ;
      double _Complex Chunk1 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+1));
      double _Complex Chunk2 = __lfpd((fifoType)(HalfChunks+PayloadHalfChunkIndex+2));
        if ( BytesInPayload > 0 )
        {
        *(double _Complex *) dst = Chunk0 ;
          if ( BytesInPayload > 16 )
          {
           *(double _Complex *) (dst+16) = Chunk1 ;
           if ( BytesInPayload > 32 )
           {
              *(double _Complex *) (dst+32) = Chunk2 ;
           }	
          }
        }
    } else {
      memcpy(dst,HalfChunks+PayloadHalfChunkIndex,BytesInPayload) ; 
    }	  	
   ThisPacketHalfChunkIndex += 2*(Chunks+1) ; 
     Chunks = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+ThisPacketHalfChunkIndex))->hwh0.Chunks ;
     SourceRank = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+ThisPacketHalfChunkIndex))->PacketMetaData.mSourceRank ;
     TotalCountToRecv = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+ThisPacketHalfChunkIndex))->PacketMetaData.mTotalCount ;
     Offset = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+ThisPacketHalfChunkIndex))->PacketMetaData.mOffset ;
     BytesInPayload = ((BGL_AllToAll_TorusPktHdr *) (HalfChunks+ThisPacketHalfChunkIndex))->PacketMetaData.mBytesInPayload ;
  }  
  
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
    << "_ts_AppReceiver_Alltoallv_Drain PacketsInFifo=" << PacketsInFifo
    << EndLogLine ;
  
  aThis->mTotalRecvPackets += PacketsInFifo ;
    return result ; 
  }
#endif  

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
  {
    return SEND_DONE;
  }

  assert( *mCurrentRankPtr >= 0 && *mCurrentRankPtr < mGroupSize );

  if( mSendOffsets[ *mCurrentRankPtr ] < (mSendCount[ *mCurrentRankPtr ] * mSendTypeSize) )
    {
    // There's more
    if( mCurrentInject < mPktInject )
      {
      mCurrentInject++;
      BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
        << "GetNextRankToSend " << *mCurrentRankPtr
        << EndLogLine ;
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
    {
      
        return SEND_DONE;
    }

    *mCurrentRankPtr = *mEndRankPtr;

    if( mCurrentRankPtr == mEndRankPtr )
      {
      mCurrentRankPtr = mActiveSendRanksTransient;
      }
    mEndRankPtr--;
    }

  assert( *mCurrentRankPtr < mGroupSize );
  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
    << "GetNextRankToSend " << *mCurrentRankPtr 
    << EndLogLine ;
  
  return *mCurrentRankPtr;
  }


inline
void
PacketAlltoAllv::SendMessages( PacketAlltoAllv* aThis )
  {
  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;

  aThis->mTotalSentPackets = 0;

  int LinkFifoHeaderCounters[ NUMBER_OF_LINKS ];
  for( int Link=0; Link < NUMBER_OF_LINKS; Link++ )
    {
    LinkFifoHeaderCounters[ Link ] = 0;
    }

  int LinkListsDone = 0;

  while( LinkListsDone < aThis->mActiveLinkCount )
    {
    // Read the status
    _ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );
    aThis->mFifoStats[ 0 ].mStat = FifoStatus0.i0;
    aThis->mFifoStats[ 1 ].mStat = FifoStatus0.i1;
    aThis->mFifoStats[ 2 ].mStat = FifoStatus0.i2;
    aThis->mFifoStats[ 3 ].mStat = FifoStatus1.i0;
    aThis->mFifoStats[ 4 ].mStat = FifoStatus1.i1;
    aThis->mFifoStats[ 5 ].mStat = FifoStatus1.i2;

    // Stuff as many packets into fifos as you can before coming around
    for( int LinkIndex=0; LinkIndex < aThis->mActiveLinkCount; LinkIndex++ )
      {
      int Link = aThis->mActiveLink[ LinkIndex ];
      // Stuff as many packets down this link as there's space.
      // Pick up the next Packet Header destined for this link
      int InjectionCount = 0;

      while( LinkFifoHeaderCounters[ Link ] < aThis->mHeadersForLinkCount[ Link ] )
        {
        // Check to see if this packet could be sent
        int HeaderIndex = LinkFifoHeaderCounters[ Link ];

        assert( HeaderIndex >= 0 && HeaderIndex < aThis->mHeadersForLinkAllocatedCount );
        assert( HeaderIndex >= 0 && HeaderIndex < aThis->mHeadersForLinkCount[ Link ] );
        _BGL_TorusPktHdr* HdrPtr = &aThis->mHeadersForLink[ Link ][ HeaderIndex ].mHdr;
        int tot_c = HdrPtr->hwh0.Chunks + 1;

#ifdef DEBUG_ALL2ALL
//#if 1
        if( alltoallv_debug && aThis->mMyRank == 194 )
          {
          printf( "tot_c: %d LinkFifoHeaderCounters[ %d ]: %d fstat: %d max_chunk: %d\n",
                  tot_c,
                  Link,
                  LinkFifoHeaderCounters[ Link ],
                  aThis->mFifoStats[ Link ].mStat,
                  _BGL_TORUS_FIFO_CHUNKS );
          }
#endif

        if(( aThis->mFifoStats[ Link ].mStat + tot_c ) <= _BGL_TORUS_FIFO_CHUNKS )
          {
          // There's room to send! There goes a packet!
          //int DestRank = BGLPartitionMakeRank( HdrPtr->hwh0.X, HdrPtr->hwh1.Y, HdrPtr->hwh1.Z );
          int DestRank             = aThis->mHeadersForLink[ Link ][ HeaderIndex ].mDestRank;
          assert( DestRank != aThis->mMyRank );

          BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &HdrPtr->swh0;

          int BytesToSend = MetaData->mBytesInPayload;
          assert( BytesToSend );

//           if( !( (MetaData->mOffset + BytesToSend) <= ( MetaData->mTotalCount * aThis->mRecvTypeSize ) ) )
//             {
//             printf("B: %d O: %d TC: %d RS: %d DR: %d link %d HI: %d HC: %d LI: %d ALC: %d\n",
//                    BytesToSend,
//                    MetaData->mOffset,
//                    MetaData->mTotalCount,
//                    aThis->mRecvTypeSize,
//                    DestRank,
//                    Link,
//                    HeaderIndex,
//                    aThis->mHeadersForLinkCount[ Link ],
//                    LinkIndex,
//                    aThis->mActiveLinkCount );
//             }

          assert( (MetaData->mOffset + BytesToSend) <= ( MetaData->mTotalCount * aThis->mRecvTypeSize ) );

          assert( DestRank >= 0 && DestRank < aThis->mGroupSize );
          char* BlockToSend = ((char *)aThis->mSendBuff) + (aThis->mSendOffsetIn[ DestRank ] * aThis->mSendTypeSize) + MetaData->mOffset;

          // assert( ((unsigned long)BlockToSend & 0xf) == 0);

          memcpy( aThis->mAlltoAllvPacket.payload, BlockToSend, BytesToSend );

          aThis->mAlltoAllvPacket.hdr = *HdrPtr;
//          QuadMove( HdrPtr, &aThis->mAlltoAllvPacket.hdr, 1 );

          _BGL_TorusFifo * NextFifoToSend = aThis->mFifoStats[ Link ].mLink;

#ifdef DEBUG_ALL2ALL
          if ( 1 )
            {
            complex* CompPtr = (complex *)aThis->mAlltoAllvPacket.payload;
            for( int i=0; i < (BytesToSend / sizeof( complex )); i++ )
              {
              if( CompPtr[ i ].re != aThis->mMyRank ||
                  CompPtr[ i ].im != (-1.0*aThis->mMyRank) )
                {
                printf("mMyRank=%d CompPtr[ %d ].im= %f CompPtr[ %d ].re= %f \n",
                       aThis->mMyRank, i, CompPtr[ i ].re, CompPtr[ i ].im );

                assert( 0 );
                }
              }
            }
#endif

          BGLTorusInjectPacketImageApp( NextFifoToSend,
                                        &aThis->mAlltoAllvPacket );


          aThis->mTotalSentPackets++;
#ifdef DEBUG_ALL2ALL
          // if( Verbose )
          printf( "Sent: %d %d %d\n",
                  aThis->mMyRank,
                  BytesToSend,
                  MetaData->mOffset );
#endif
          // +1 becuase
          // giampapa (9:54 PM) - hardware is "org -1", ie 3bit field
          // need to represent values 1 through 8, so we subtracted 1

          aThis->mFifoStats[ Link ].mStat += tot_c;
          LinkFifoHeaderCounters[ Link ]++;

          if( LinkFifoHeaderCounters[ Link ] == aThis->mHeadersForLinkCount[ Link ] )
              LinkListsDone++;

          InjectionCount++;
          if( InjectionCount == aThis->mPacketsToInject )
              break;
          }
        else
          {
          // Fifo is full, try another one
          break;
          }
        }
      }
    }

#ifdef DEBUG_ALL2ALL
  if( alltoallv_debug )
      printf( "mTotalSentPackets: %d\n", aThis->mTotalSentPackets );
#endif
// assert( mTotalSentPackets == 254 );
  }

#if 0
inline
void
PacketAlltoAllv::SendMessages_Old()
  {
//***
//** Init execute state
//***
  for( int i=0; i<mActiveSendRanksCount; i++ )
    {
    int Rank = mActiveSendRanks[ i ];

    assert( Rank >=0 && Rank < mGroupSize );
    mSendOffsets[ Rank ] = 0;
    }
  memcpy( mActiveSendRanksTransient, mActiveSendRanks, sizeof(int)*mActiveSendRanksCount);
  mCurrentRankPtr = mActiveSendRanksTransient;
  mEndRankPtr = &(mActiveSendRanksTransient[ mActiveSendRanksCount - 1 ]);
//***********************************************************************************

  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;

  int LinkPicker = 0;

  int DestRank = SEND_DONE;

  int i=0;

  while( 1 )
    {
    // Pick the next random rank to sent to
    DestRank = GetNextRankToSend();
    if( DestRank == SEND_DONE )
        return;

    assert( DestRank >= 0 && DestRank < mGroupSize );
    int DestCoordX = mGroup->mElements[ DestRank ].mCoord.mX;
    int DestCoordY = mGroup->mElements[ DestRank ].mCoord.mY;
    int DestCoordZ = mGroup->mElements[ DestRank ].mCoord.mZ;

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

    // Send a full packet
    BGLTorusMakeHdrAppChooseRecvFifo( & ( mAlltoAllvPacket.hdr ),
                                      DestCoordX,
                                      DestCoordY,
                                      DestCoordZ,
                                      0,
                                      0,
                                      BytesToSend,
                                      ((i++) & 0x1 ) );

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

    memcpy( mAlltoAllvPacket.payload, BlockToSend, BytesToSend );

    mSendOffsets[ DestRank ] += BytesToSend;

    int chunks = ((BytesToSend + 15) >> 5) + 1;

#ifdef DEBUG_ALL2ALL
    if( Verbose )
      {
      printf("Send packet() SourceRank: %d TotalCountToRecv: %d Offset: %d BytesInPayload: %d\n",
             MetaData->mSourceRank,
             MetaData->mTotalCount,
             MetaData->mOffset,
             MetaData->mBytesInPayload  );

      printf("Send packet %d bytes to { %d, %d %d }\n",
             MetaData->mBytesInPayload,
             DestCoordX,
             DestCoordY,
             DestCoordZ );
      }
#endif
    while( 1 )
      {
      _ts_GetStatusBoth( & FifoStatus0, &FifoStatus1 );

      mFifoStats[ 0 ].mStat = FifoStatus0.i0;
      mFifoStats[ 1 ].mStat = FifoStatus0.i1;
      mFifoStats[ 2 ].mStat = FifoStatus0.i2;
      mFifoStats[ 3 ].mStat = FifoStatus1.i0;
      mFifoStats[ 4 ].mStat = FifoStatus1.i1;
      mFifoStats[ 5 ].mStat = FifoStatus1.i2;

      LinkPicker++;
      if( LinkPicker > 5 )
          LinkPicker = 0;

      _BGL_TorusFifo * NextFifoToSend = NULL;
      if( mFifoStats[ LinkPicker ].mStat + chunks <=  _BGL_TORUS_FIFO_CHUNKS )
        {
        mFifoStats[ LinkPicker ].mStat += chunks;
        NextFifoToSend = mFifoStats[ LinkPicker ].mLink;

#ifdef DEBUG_ALL2ALL
        if( Verbose )
          {
          printf("Send packet() chunks: %d LinkToSend: %d NextFifoToSend: %08X\n",
                 chunks,
                 LinkPicker,
                 NextFifoToSend );
          }
#endif

        BGLTorusInjectPacketImageApp( NextFifoToSend,
                                      &mAlltoAllvPacket );
        break;
        }
      }
    }
  }
#endif

template< int NON_ALIGNED1 >
inline
void
PacketAlltoAllv::RecvMessages( PacketAlltoAllv* aThis )
  {
  aThis->mRecvCountDone = aThis->mRecvNonActiveCount;
  for( int i=0; i<aThis->mGroupSize; i++ )
      aThis->mBytesRecvFromNode[ i ] = 0;

  aThis->mTotalRecvPackets = 0;

#ifdef DEBUG_ALL2ALL
  if( Verbose )
    {
    printf("Recv packet() mRecvCountDone: %d mGroupSize: %d\n", aThis->mRecvCountDone, aThis->mGroupSize );
    }
#endif

//       if( mSendCount[ mMyRank ] )
//          {
//          char * Src = (char*)mSendBuff + (mSendOffsetIn[ mMyRank ] * mSendTypeSize);
//          char * Tgt = (char*)mRecvBuff + (mRecvOffset[ mMyRank ] * mRecvTypeSize);
//          int Size   = mSendCount[ mMyRank ] * mSendTypeSize;
//          memcpy( Tgt, Src, Size );
//          }
//        mRecvCount[ mMyRank ] = mSendCount[ mMyRank ];

  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;

  while( aThis->mRecvCountDone != aThis->mGroupSize )
    {
    ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );

    _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, aThis );
    assert( aThis->mRecvCountDone <= aThis->mGroupSize );
    }

#ifdef DEBUG_ALL2ALL
  if( alltoallv_debug )
      printf( "mTotalRecvPackets: %d\n", aThis->mTotalRecvPackets );
#endif
// assert( mTotalRecvPackets == 254 );
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

// PacketAlltoAllv::PacketAlltoAllvInitActor

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

  alltoallv_debug    = Args->mDebug;

 BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
   << "PacketAlltoAllv::InitActor ThisPtr=" << ThisPtr
   << " SendCount=" << SendCount
   << " RecvCount=" << RecvCount
   << " SendOffset=" << SendOffset
   << " RecvOffset=" << RecvOffset
   << " SendTypeSize=" << SendTypeSize
   << " RecvTypeSize" << RecvTypeSize
   << EndLogLine ; 
 // printf( "Group: %08x line: \n", Group, __LINE__ );

  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
    printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
           sizeof( BGL_AlltoallvPacketMetaData ) );
  
  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );

  ThisPtr->mPacketsToInject = PACKETS_TO_INJECT;
  ThisPtr->mGroup     = Group;
  ThisPtr->mMyRank    = ThisPtr->mGroup->mMyRank;  // BGLPartitionGetRank();

  // printf( "ThisPtr->mMyRank: %d\n", ThisPtr->mMyRank );

  ThisPtr->mGroupSize = ThisPtr->mGroup->mSize;
  ThisPtr->mGroupElements = ThisPtr->mGroup->mElements;

  if( !( ThisPtr->mMyRank >= 0 && ThisPtr->mMyRank < ThisPtr->mGroupSize ) )
    {
    printf("%d %d\n", ThisPtr->mMyRank, ThisPtr->mGroupSize );
    }

  assert( ThisPtr->mMyRank >= 0 && ThisPtr->mMyRank < ThisPtr->mGroupSize );

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

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

  ThisPtr->mBytesRecvFromNode = (int *) malloc( ThisPtr->mGroupSize * sizeof(int));
  assert( ThisPtr->mBytesRecvFromNode );

  ThisPtr->mSendOffsets = (int *) malloc( ThisPtr->mGroupSize * sizeof(int));
  assert( ThisPtr->mSendOffsets );

  ThisPtr->mActiveSendRanks = (int *) malloc( ThisPtr->mGroupSize * sizeof(int));
  assert( ThisPtr->mActiveSendRanks );
  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

// We never send to self.
  ThisPtr->mRecvNonActiveCount = 1;
  ThisPtr->mRecvDoneExpectBits = 0 ;

  ThisPtr->mActiveSendRanksCount = 0;
  
  int LargestMessageSoFar = 0 ;

  int MaxNumberHeaders = 0;

  if( alltoallv_debug && ThisPtr->mMyRank == 194 )
    {
    printf("ThisPtr->GroupSize: %d d: %d mr: %d\n",
           ThisPtr->mGroupSize,
           alltoallv_debug,
           ThisPtr->mMyRank );
    }

  for( int i=0; i < ThisPtr->mGroupSize; i++ )
    {
      BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
        << "InitActor i=" << i
        << " mSendCount=" << ThisPtr->mSendCount[ i ]
        << " mRecvCount=" << ThisPtr->mRecvCount[ i ]
        << EndLogLine ;
    if( alltoallv_debug && ThisPtr->mMyRank == 194 )
      {
      printf("ThisPtr->mSendCount[ %d ]: %d ThisPtr->mRecvCount[ %d ]: %d\n",
             i, ThisPtr->mSendCount[ i ],
             i, ThisPtr->mRecvCount[ i ] );
      }

    if( ThisPtr->mSendCount[ i ] != 0 && i != ThisPtr->mMyRank )
      {
      int NumberOfHeaders = (int) ceil( (1.0*( ThisPtr->mSendCount[ i ] * ThisPtr->mSendTypeSize)) / (_BGL_TORUS_PKT_MAX_PAYLOAD*1.0) );

      if( NumberOfHeaders > MaxNumberHeaders )
          MaxNumberHeaders = NumberOfHeaders;

      ThisPtr->mActiveSendRanks[ ThisPtr->mActiveSendRanksCount ] = i;
      ThisPtr->mActiveSendRanksCount++;
      ThisPtr->mRecvDoneExpectBits |= (1<<i) ;
      
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
          (BGLAlltoallvHeaders *) malloc( sizeof( BGLAlltoallvHeaders ) * ThisPtr->mHeadersForLinkAllocatedCount );

      if( BGLPartitionGetRank() == 0 )
        {
        printf( "Allocate mHeadersForLink[ %d ]: size: %d\n", i, sizeof( BGLAlltoallvHeaders ) * ThisPtr->mHeadersForLinkAllocatedCount );
        }
      
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
    ThisPtr->mRecvDoneExpectBits |= (1<<Rank) ;
    TorusXYZ DestCoord = ThisPtr->mGroupElements[ Rank ].mCoord;

    // Create headers for this rank
    TorusXYZ LinkXYZ = GetRootBoundTreeLink( ThisPtr->mPeriod, DestCoord, ThisPtr->mMyCoord );
    int      Link    = GetLinkNumber( LinkXYZ );

    assert( Link >=0 && Link < 6 );

    int TotalBytesToSend = ThisPtr->mSendCount[ Rank ] * ThisPtr->mSendTypeSize;

    int BytesSent = 0;

    if( TotalBytesToSend > LargestMessageSoFar ) LargestMessageSoFar=TotalBytesToSend ;
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
                                        ((FifoPicker++) & 0x1 ) );

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
    
  ThisPtr->mLargestPacketChunks=( LargestMessageSoFar >= _BGL_TORUS_PKT_MAX_PAYLOAD ) 
                               ? _BGL_TORUS_PKT_MAX_CHUNKS 
                               : ((LargestMessageSoFar
                                   +sizeof(_BGL_TorusPktHdr)
                                   +_BGL_TORUS_SIZEOF_CHUNK-1)
                                  /_BGL_TORUS_SIZEOF_CHUNK) ;
  // Given an 'empty' FIFO, we can write this many packets without
  // rechecking the status                                  
  ThisPtr-> mMinPacketsInSendFIFO = _BGL_TORUS_FIFO_CHUNKS/ThisPtr->mLargestPacketChunks ;
  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

  for ( int StatusValue=0; StatusValue<_BGL_TORUS_FIFO_CHUNKS; StatusValue+=1)
  {
    ThisPtr->mPacketsToInjectByStatus[StatusValue] = 
      (_BGL_TORUS_FIFO_CHUNKS-StatusValue) / ThisPtr->mLargestPacketChunks ;
  }
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

  ThisPtr->mFifoStats[ 0 ].mLink = (_BGL_TorusFifo *) TS0_I0;
  ThisPtr->mFifoStats[ 1 ].mLink = (_BGL_TorusFifo *) TS0_I1;
  ThisPtr->mFifoStats[ 2 ].mLink = (_BGL_TorusFifo *) TS0_I2;
  ThisPtr->mFifoStats[ 3 ].mLink = (_BGL_TorusFifo *) TS1_I0;
  ThisPtr->mFifoStats[ 4 ].mLink = (_BGL_TorusFifo *) TS1_I1;
  ThisPtr->mFifoStats[ 5 ].mLink = (_BGL_TorusFifo *) TS1_I2;

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

#ifdef DEBUG_ALL2ALL
  if( Verbose )
      printf("Core(%d): Got here: %d\n",rts_get_processor_id(), __LINE__ );
#endif
  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
    "InitActor ThisPtr->mRecvNonActiveCount=" << ThisPtr->mRecvNonActiveCount 
    << EndLogLine ;
  return NULL;
  }

#if 0
void PacketAlltoAllv::Init( int* aSendCount, int* aSendOffset, int aSendTypeSize,
                            int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                            GroupContainer* aGroup )
  {
  PacketAlltoAllvInitActorArgs Core0Args;

  Core0Args.mThis         =  this;
  Core0Args.mSendCount    = aSendCount;
  Core0Args.mRecvCount    = aRecvCount;
  Core0Args.mSendOffsets  = aSendOffset;
  Core0Args.mRecvOffsets  = aRecvOffset;
  Core0Args.mSendTypeSize = aSendTypeSize;
  Core0Args.mRecvTypeSize = aRecvTypeSize;
  Core0Args.mGroup        = aGroup;
  Core0Args.mDebug        = alltoallv_debug;

  InitActor( &Core0Args );
  rts_dcache_evict_normal();
  }
#endif

void PacketAlltoAllv::InitSingleCore( int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                      int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                      GroupContainer* aGroup )
  {
  PacketAlltoAllvInitActorArgs Core0Args;

  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
    << "InitSingleCore aSendCount=" << aSendCount
    << " aSendOffset=" << aSendOffset
    << " aSendTypeSize=" << aSendTypeSize
    << " aRecvCount=" << aRecvCount
    << " aRecvOffset=" << aRecvOffset
    << " aRecvTypeSize=" << aRecvTypeSize
    << " aGroup=" << (void *) aGroup
    << EndLogLine ;
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

  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
    printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
           sizeof( BGL_AlltoallvPacketMetaData ) );

  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );

  mPacketsToInject = PACKETS_TO_INJECT;
  mGroup     = aGroup;
  mMyRank    = mGroup->mMyRank;

  mGroupSize = mGroup->mSize;
  mGroupElements = mGroup->mElements;

  if( !( mMyRank >= 0 && mMyRank < mGroupSize ) )
    {
    printf("%d %d\n", mMyRank, mGroupSize );
    }

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
    
    printf( "Done allocating simple alltoallv members\n" );
    fflush( stdout );

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
//void PacketAlltoAllv::InitSingleCoreSimple( int* aSendCount, int* aSendOffset, int aSendTypeSize,
//                                            int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
//                                            GroupContainer* aGroup )
//  {
//  alltoallv_debug    = 0;
//
//  BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
//    << "PacketAlltoAllv::InitSingleCoreSimple this=" << this
//    << " aSendTypeSize=" << aSendTypeSize
//    << " aRecvTypeSize=" << aRecvTypeSize
//    << EndLogLine ;
//  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
//    printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
//           sizeof( BGL_AlltoallvPacketMetaData ) );
//
//  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );
//
//  mPacketsToInject = PACKETS_TO_INJECT;
//  mGroup     = aGroup;
//  mMyRank    = mGroup->mMyRank;
//
//  mGroupSize = mGroup->mSize;
//  mGroupElements = mGroup->mElements;
//
//  if( !( mMyRank >= 0 && mMyRank < mGroupSize ) )
//    {
//    printf("%d %d\n", mMyRank, mGroupSize );
//    }
//
//  assert( mMyRank >= 0 && mMyRank < mGroupSize );
//
//  mMyCoord   = mGroupElements[ mMyRank ].mCoord;
//
//  mSendCount     = aSendCount;
//  mSendOffsetIn  = aSendOffset;
//  mSendTypeSize  = aSendTypeSize;
//
//  mRecvCount    = aRecvCount;
//  mRecvOffset   = aRecvOffset;
//  mRecvTypeSize = aRecvTypeSize;
//
//  mBytesRecvFromNode = (int *) malloc( mGroupSize * sizeof(int));
//  assert( mBytesRecvFromNode );
//
//  mSendOffsets = (int *) malloc( mGroupSize * sizeof(int));
//  assert( mSendOffsets );
//
//  mActiveSendRanks = (int *) malloc( mGroupSize * sizeof(int));
//  assert( mActiveSendRanks );
//
//// We never send to self.
//  mRecvNonActiveCount = 1;
//  mRecvDoneExpectBits = 0 ;
//
//  mActiveSendRanksCount = 0;
//
//  int MaxNumberHeaders = 0;
//
//  if( alltoallv_debug && mMyRank == 194 )
//    {
//    printf("GroupSize: %d d: %d mr: %d\n",
//           mGroupSize,
//           alltoallv_debug,
//           mMyRank );
//    }
//
//  for( int i=0; i < mGroupSize; i++ )
//    {
//    if( alltoallv_debug && mMyRank == 194 )
//      {
//      printf("mSendCount[ %d ]: %d mRecvCount[ %d ]: %d\n",
//             i, mSendCount[ i ],
//             i, mRecvCount[ i ] );
//      }
//
//    if( mSendCount[ i ] != 0 && i != mMyRank )
//      {
//      mActiveSendRanks[ mActiveSendRanksCount ] = i;
//      mActiveSendRanksCount++;
//      mRecvDoneExpectBits |= (1<<i) ;
//      }
//
//    if( mRecvCount[ i ] == 0 && i != mMyRank )
//      {
//      // Self sends have been accounted
//      mRecvNonActiveCount++;
//      }
//    }
//  PermuteActiveSendRanks( mActiveSendRanks, mActiveSendRanksCount );
//
//  // printf("mActiveSendRanksCount: %d\n", mActiveSendRanksCount);
//  
//  // Transient gets filled in SendMessages
//  mActiveSendRanksTransient = (int *) malloc( sizeof(int) * mActiveSendRanksCount );
//  assert( mActiveSendRanksTransient );
//
//  mPktInject = 2;
//  mCurrentInject = 0;
//
//  mFifoStats[ 0 ].mLink = (_BGL_TorusFifo *) TS0_I0;
//  mFifoStats[ 1 ].mLink = (_BGL_TorusFifo *) TS0_I1;
//  mFifoStats[ 2 ].mLink = (_BGL_TorusFifo *) TS0_I2;
//  mFifoStats[ 3 ].mLink = (_BGL_TorusFifo *) TS1_I0;
//  mFifoStats[ 4 ].mLink = (_BGL_TorusFifo *) TS1_I1;
//  mFifoStats[ 5 ].mLink = (_BGL_TorusFifo *) TS1_I2;
//
//  }

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
    free( mActiveSendRanksTransient );
    mActiveSendRanksTransient = NULL;
    }  
  }

struct ExecuteActorArgs
{
    PacketAlltoAllv* mThis;
    void *mSendBuff;
    void *mRecvBuff;
};

#if 0
void* PacketAlltoAllv::ExecuteActor( void * arg )
  {
  ExecuteActorArgs* Args = (ExecuteActorArgs *) arg;

  PacketAlltoAllv* ThisPtr = Args->mThis;
  ThisPtr->mSendBuff   = Args->mSendBuff;
  ThisPtr->mRecvBuff   = Args->mRecvBuff;

  // Assume that Init has been called.
  // rts_dcache_evict_normal();
  // printf("Core(%d) debug: %d Got here %d\n", hard_processor_id(), alltoallv_debug,  __LINE__);

  if( hard_processor_id() )
    {
    // Core 1 sends messages
    // printf("Got here %d\n", __LINE__);
    _blLockBoxBarrier( 64 );
    // Core 1 is second to flush it's cache.
    // It looks
#if 1
    if( alltoallv_debug )
        printf("Core(%d): Got here: %d\n", rts_get_processor_id(), __LINE__ );
#endif
    rts_dcache_evict_normal();
    SendMessages( ThisPtr );
    // printf("Got here %d\n", __LINE__);

//#ifdef DEBUG_ALL2ALL
#if 1
    if( alltoallv_debug )
        printf("Core(%d): Got here: %d\n", rts_get_processor_id(), __LINE__ );
#endif

    _blLockBoxBarrier( 64 );
    }
  else
    {
    // Core 0 receives messages

    // Core 0 needs to flush all the fft data out first.
    // So that core 1 gets a chance to send correct data
    rts_dcache_evict_normal();
    _blLockBoxBarrier( 64 );

#ifdef DEBUG_ALL2ALL
    if( Verbose )
        printf("Core(%d): Got here: %d\n",rts_get_processor_id(), __LINE__ );
#endif
    // printf("Got here %d\n", __LINE__);
#if 1
    if( alltoallv_debug )
        printf("Core(%d): Got here: %d\n", rts_get_processor_id(), __LINE__ );
#endif

    RecvMessages< NON_ALIGNED1 >( ThisPtr );
#if 1
    if( alltoallv_debug )
        printf("Core(%d): Got here: %d\n", rts_get_processor_id(), __LINE__ );
#endif
    // printf("Got here %d\n", __LINE__);

    // Memcpy self sends
    if( ThisPtr->mSendCount[ ThisPtr->mMyRank ] )
      {
      char * Src = (char*)ThisPtr->mSendBuff + (ThisPtr->mSendOffsetIn[ ThisPtr->mMyRank ] * ThisPtr->mSendTypeSize);
      char * Tgt = (char*)ThisPtr->mRecvBuff + (ThisPtr->mRecvOffset[ ThisPtr->mMyRank ] * ThisPtr->mRecvTypeSize);
      int Size   = ThisPtr->mSendCount[ ThisPtr->mMyRank ] * ThisPtr->mSendTypeSize;
      memcpy( Tgt, Src, Size );
      }
    ThisPtr->mRecvCount[ ThisPtr->mMyRank ] = ThisPtr->mSendCount[ ThisPtr->mMyRank ];

#ifdef DEBUG_ALL2ALL
    if( Verbose )
        printf("Core(%d): Got here: %d\n",rts_get_processor_id(), __LINE__ );

    if( 1 )
      {
      for( int i=0; i < ThisPtr->mGroupSize; i++ )
        {
        char* Target = (char *)ThisPtr->mRecvBuff + (ThisPtr->mRecvOffset[ i ] * ThisPtr->mRecvTypeSize );
        // char* Source = (char *) ((unsigned) (Target) + 0x50000000);
        // memcpy( Target, Source, mRecvCount[ i ]*mRecvTypeSize );

        complex* CompPtr = (complex *)Target;
        for( int j=0; j < ThisPtr->mRecvCount[ i ]; j++ )
          {
          if( CompPtr[ j ].re != i ||
              CompPtr[ j ].im != (-1.0*i) )
            {
            printf("myRank=%d i=%d CompPtr[ %d ].re= %f CompPtr[ %d ].im= %f \n",
                   ThisPtr->mMyRank, i, j, CompPtr[ j ].re, j, CompPtr[ j ].im );

            assert( 0 );
            }
          }
        }
      }
#endif

    // rts_dcache_evict_normal();

    _blLockBoxBarrier( 64 );
    }

  return NULL;
  }

void PacketAlltoAllv::Execute( void* aSendBuff, void* aRecvBuff )
  {
  BGLPartitionBarrierComputeCore0();

  ExecuteActorArgs Core0Args;
  Core0Args.mThis = this;
  Core0Args.mSendBuff = aSendBuff;
  Core0Args.mRecvBuff = aRecvBuff;

  ExecuteActorArgs Core1Args;
  Core1Args.mThis = this;
  Core1Args.mSendBuff = (void *) ( 0x50000000 + (unsigned long) aSendBuff);
  Core1Args.mRecvBuff = (void *) ( 0x50000000 + (unsigned long) aRecvBuff);

  vnm_co_start( ExecuteActor, &Core1Args, sizeof( ExecuteActorArgs ) );
  ExecuteActor( &Core0Args );
  vnm_co_join();

  // Barrier the phase. We need to receive packets from another phase
  BGLPartitionBarrierComputeCore0();
  }
#endif

template< int NON_ALIGNED1 >
void PacketAlltoAllv::ExecuteSingleCoreSimple1( void* aSendBuff, void* aRecvBuff )
  {
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
      << "ExecuteSingleCoreSimple1 this=" << this
      << " mSendTypeSize=" << mSendTypeSize
      << " mRecvTypeSize=" << mRecvTypeSize
      << EndLogLine ;
  BGLPartitionBarrierCore();

  mSendBuff = aSendBuff;
  mRecvBuff = aRecvBuff;

  // memcpy data local to this node.
  if( mSendCount[ mMyRank ] )
    {
    char * Src = (char*)mSendBuff + (mSendOffsetIn[ mMyRank ] * mSendTypeSize);
    char * Tgt = (char*)mRecvBuff + (mRecvOffset[ mMyRank ] * mRecvTypeSize);
    int Size   = mSendCount[ mMyRank ] * mSendTypeSize;
    memcpy( Tgt, Src, Size );
    }
  mRecvCount[ mMyRank ] = mSendCount[ mMyRank ];
  mRecvDoneBits = 0xffffffff - mRecvDoneExpectBits ;

  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
    << "mRecvDoneExpectBits=" << (void *) mRecvDoneExpectBits 
    << EndLogLine ;
    
  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;

  mTotalSentPackets = 0;
  mTotalRecvPackets = 0;
  mRecvCountDone = mRecvNonActiveCount;
  for( int i=0; i<mGroupSize; i++ )
    mBytesRecvFromNode[ i ] = 0;

  for( int i=0; i<mActiveSendRanksCount; i++ )
    {
    int Rank = mActiveSendRanks[ i ];

    assert( Rank >=0 && Rank < mGroupSize );
    mSendOffsets[ Rank ] = 0;
    }

  memcpy( mActiveSendRanksTransient, mActiveSendRanks, sizeof(int)*mActiveSendRanksCount);
  mCurrentRankPtr = mActiveSendRanksTransient;

  if( mActiveSendRanksCount == 0 )
    mEndRankPtr = mCurrentRankPtr;
  else
    mEndRankPtr = &(mActiveSendRanksTransient[ mActiveSendRanksCount - 1 ]);

//   printf( "mCurrentRankPtr: %08X mEndRankPtr: %08X \n",
//           mCurrentRankPtr, mEndRankPtr );
  
  int LinkPicker = 0;
  int DestRank = SEND_DONE;
  
  int PktCount = 0;

  int LinkListsDone = 0;
  int CurLinkIndex  = 0;
  int q=1 ;
  while( (( DestRank = GetNextRankToSend()) != SEND_DONE ) ||
         ( mRecvCountDone != mGroupSize ) )
    {
      BegLogLine(PKFXLOG_PACKET_ALLTOALLV && DestRank==SEND_DONE && q && (q=0,1) ) 
        << "ExecuteSingleCoreSimple1 Sending completed"
        << EndLogLine ;
    // While there are packets to send keep trying to inject packets.

    if( DestRank != SEND_DONE )
      {    
      assert( DestRank >= 0 && DestRank < mGroupSize );
      int DestCoordX = mGroup->mElements[ DestRank ].mCoord.mX;
      int DestCoordY = mGroup->mElements[ DestRank ].mCoord.mY;
      int DestCoordZ = mGroup->mElements[ DestRank ].mCoord.mZ;
      
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
      
      // Send a full packet
      BGLTorusMakeHdrAppChooseRecvFifo( & ( mAlltoAllvPacket.hdr ),
                                        DestCoordX,
                                        DestCoordY,
                                        DestCoordZ,
                                        0,
                                        0,
                                        BytesToSend,
                                        ((PktCount++) & 0x1 ) );
      
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
      
      memcpy( mAlltoAllvPacket.payload, BlockToSend, BytesToSend );
      
      mSendOffsets[ DestRank ] += BytesToSend;
      
      int chunks = ((BytesToSend + 15) >> 5) + 1;
      
#ifdef DEBUG_ALL2ALL
      if( Verbose )
        {
        printf("Send packet() SourceRank: %d TotalCountToRecv: %d Offset: %d BytesInPayload: %d\n",
               MetaData->mSourceRank,
               MetaData->mTotalCount,
               MetaData->mOffset,
               MetaData->mBytesInPayload  );
        
        printf("Send packet %d bytes to { %d, %d %d }\n",
               MetaData->mBytesInPayload,
               DestCoordX,
               DestCoordY,
               DestCoordZ );
        }
#endif
      BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
        << "Simple1 DestRank=" << DestRank
        << " SourceRank=" << (int) MetaData->mSourceRank
        << " TotalCount=" << (int) MetaData->mTotalCount
        << " Offset=" << (int) MetaData->mOffset
        << " BytesInPayload=" << (int) MetaData->mBytesInPayload
        << EndLogLine ;
      int spinlimit = 6 ; 
      while( 1  )
        {
        ts_GetStatusBoth( & FifoStatus0, &FifoStatus1 );
        
        mFifoStats[ 0 ].mStat = FifoStatus0.i0;
        mFifoStats[ 1 ].mStat = FifoStatus0.i1;
        mFifoStats[ 2 ].mStat = FifoStatus0.i2;
        mFifoStats[ 3 ].mStat = FifoStatus1.i0;
        mFifoStats[ 4 ].mStat = FifoStatus1.i1;
        mFifoStats[ 5 ].mStat = FifoStatus1.i2;
        fStatus.recordSendSimple(FifoStatus0.i0) ;
        fStatus.recordSendSimple(FifoStatus0.i1) ;
        fStatus.recordSendSimple(FifoStatus0.i2) ;
        fStatus.recordSendSimple(FifoStatus1.i0) ;
        fStatus.recordSendSimple(FifoStatus1.i1) ;
        fStatus.recordSendSimple(FifoStatus1.i2) ;
        
        LinkPicker++;
        if( LinkPicker > 5 )
          LinkPicker = 0;
        
        _BGL_TorusFifo * NextFifoToSend = NULL;
        if( mFifoStats[ LinkPicker ].mStat + chunks <=  _BGL_TORUS_FIFO_CHUNKS )
          {
          BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
            << "Using link=" << LinkPicker
            << " mStat=" << mFifoStats[ LinkPicker ].mStat
            << EndLogLine ;
          mFifoStats[ LinkPicker ].mStat += chunks;
          NextFifoToSend = mFifoStats[ LinkPicker ].mLink;
          
#ifdef DEBUG_ALL2ALL
          if( Verbose )
            {
            printf("Send packet() chunks: %d LinkToSend: %d NextFifoToSend: %08X\n",
                   chunks,
                   LinkPicker,
                   NextFifoToSend );
            }
#endif
          BGLTorusInjectPacketImageApp( NextFifoToSend,
                                        &mAlltoAllvPacket );                                        
          BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
            << "Injected"
            << EndLogLine ;
          break;
          }
          spinlimit -- ; 
          if ( spinlimit < 0 ) 
          {
        int PollCount = PACKET_POLL_COUNT ;
        while( PollCount && (mRecvCountDone != mGroupSize) )
          {
          // Read the status. 
          ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );
    
          _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
          assert( mRecvCountDone <= mGroupSize );
          PollCount--;
          }
          }
        }
      }

    // While we haven't received all needed packets keep listening
    int PollCount = PACKET_POLL_COUNT ;
    while( PollCount && (mRecvCountDone != mGroupSize) )
      {
      // Read the status. 
      ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );

      _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
      assert( mRecvCountDone <= mGroupSize );
      PollCount--;
      }
    }
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
      << "ExecuteSingleCoreSimple1 complete"
      << EndLogLine ;
  }

template< int NON_ALIGNED1 >
void PacketAlltoAllv::ExecuteSingleCore1( void* aSendBuff, void* aRecvBuff )
  {
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
      << "ExecuteSingleCore1 this=" << this
      << EndLogLine ;
  BGLPartitionBarrierCore();

  mSendBuff = aSendBuff;
  mRecvBuff = aRecvBuff;

  // alltoallv_debug = 1;

  // memcpy data local to this node.
  if( mSendCount[ mMyRank ] ) 
    {
    char * Src = (char*)mSendBuff + (mSendOffsetIn[ mMyRank ] * mSendTypeSize);
    char * Tgt = (char*)mRecvBuff + (mRecvOffset[ mMyRank ] * mRecvTypeSize);
    int Size   = mSendCount[ mMyRank ] * mSendTypeSize;
    memcpy( Tgt, Src, Size );
    }
  mRecvCount[ mMyRank ] = mSendCount[ mMyRank ];

  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;

  mTotalSentPackets = 0;
  mTotalRecvPackets = 0;
  mRecvCountDone = mRecvNonActiveCount;
  mRecvDoneBits = 0xffffffff - mRecvDoneExpectBits ;
  for( int i=0; i<mGroupSize; i++ )
    mBytesRecvFromNode[ i ] = 0;
    
  int ActiveLinkCount = mActiveLinkCount ;

  int LinkFifoHeaderCounters[ NUMBER_OF_LINKS ];
  int LinkFifoHeaderLimit[NUMBER_OF_LINKS] ;
  for( int Link=0; Link < NUMBER_OF_LINKS; Link++ )
    {
    LinkFifoHeaderCounters[ Link ] = 0;
    }
    
  for ( int ActiveLink=0 ; ActiveLink < ActiveLinkCount ; ActiveLink += 1 )
  {
      int Link=mActiveLink[ActiveLink] ;
      LinkFifoHeaderLimit[ActiveLink]=mHeadersForLinkCount[Link] ;
  }
    
  int HeaderIndex = 0 ; 
//  int HeaderIndex1 = 0 ; 
//  int HeaderIndex2 = 0 ; 
//  int HeaderIndex3 = 0 ; 
//  int HeaderIndex4 = 0 ; 
//  int HeaderIndex5 = 0 ; 
  
  int HeadersForLinkCount0 = mHeadersForLinkCount[ 0 ] ;
  int HeadersForLinkCount1 = mHeadersForLinkCount[ 1 ] ;
  int HeadersForLinkCount2 = mHeadersForLinkCount[ 2 ] ;
  int HeadersForLinkCount3 = mHeadersForLinkCount[ 3 ] ;
  int HeadersForLinkCount4 = mHeadersForLinkCount[ 4 ] ;
  int HeadersForLinkCount5 = mHeadersForLinkCount[ 5 ] ;
  
  mTotalSentPackets += (HeadersForLinkCount0+HeadersForLinkCount1+HeadersForLinkCount2+HeadersForLinkCount3+HeadersForLinkCount4+HeadersForLinkCount5) ;
  
  
  
  int PacketsRemaining = 1;
  int CurLinkIndex  = 0;
  _BGL_TorusFifo * FifoToSend0 = mFifoStats[ 0 ].mLink;
  _BGL_TorusFifo * FifoToSend1 = mFifoStats[ 1 ].mLink;
  _BGL_TorusFifo * FifoToSend2 = mFifoStats[ 2 ].mLink;
  _BGL_TorusFifo * FifoToSend3 = mFifoStats[ 3 ].mLink;
  _BGL_TorusFifo * FifoToSend4 = mFifoStats[ 4 ].mLink;
  _BGL_TorusFifo * FifoToSend5 = mFifoStats[ 5 ].mLink;

  BegLogLine(PKFXLOG_PACKET_ALLTOALLV_SUMMARY)
    << "HeadersForLinkCount mAllocated=" << mHeadersForLinkAllocatedCount
    << " 0=" << HeadersForLinkCount0
    << " 1=" << HeadersForLinkCount1
    << " 2=" << HeadersForLinkCount2
    << " 3=" << HeadersForLinkCount3
    << " 4=" << HeadersForLinkCount4
    << " 5=" << HeadersForLinkCount5
    << EndLogLine ;
    
    
  int SendTypeSize = mSendTypeSize ; 
  int GroupSize = mGroupSize ;

  char StatusValue[NUMBER_OF_LINKS] ;
    
  if ( SendTypeSize % 8 )
  {
      // Data may not be on 8-byte boundaries; arrange to 'memcpy' it all
    while( ( 0 != PacketsRemaining )  )
    {
        PacketsRemaining = 0 ;
        int q=1 ;
          // Read the status
          ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );

            // Find the size of the fullest FIFO
            int Stat0 = FifoStatus0.i0 ;
            int Stat1 = FifoStatus0.i1 ;
            int Stat2 = FifoStatus0.i2 ;
            int Stat3 = FifoStatus1.i0 ;
            int Stat4 = FifoStatus1.i1 ;
            int Stat5 = FifoStatus1.i2 ;
            StatusValue[0] = Stat0 ;
            StatusValue[1] = Stat1 ;
            StatusValue[2] = Stat2 ;
            StatusValue[3] = Stat3 ;
            StatusValue[4] = Stat4 ;
            StatusValue[5] = Stat5 ;
//            fStatus.recordSend(Stat0) ;
//            fStatus.recordSend(Stat1) ;
//            fStatus.recordSend(Stat2) ;
//            fStatus.recordSend(Stat3) ;
//            fStatus.recordSend(Stat4) ;
//            fStatus.recordSend(Stat5) ;
      
          
          for ( int ActiveLink=0 ; ActiveLink < ActiveLinkCount ; ActiveLink += 1 )
          {
          int Link=mActiveLink[ActiveLink] ;
          int FifoHeaderCounter = LinkFifoHeaderCounters[ActiveLink] ;
              int FifoHeaderLimit=LinkFifoHeaderLimit[ActiveLink] ;
          int LinkStatus=StatusValue[Link] ; 
          int PacketSpaceForLink=mPacketsToInjectByStatus[LinkStatus] ;
          int PacketAskForLink = FifoHeaderLimit-FifoHeaderCounter ;
          int PacketsToSend = ( PacketAskForLink > PacketSpaceForLink )
                            ? PacketSpaceForLink
                            : PacketAskForLink ;
          PacketsRemaining += PacketAskForLink ;            
          _BGL_TorusFifo * NextFifoToSend = mFifoStats[ Link ].mLink;
//                if ( LinkStatus > 0 || PacketsToSend == 0 ) 
//                { 
//                	PacketsToSend = 0 ; 
//                } else {
//                	PacketsToSend = 1 ; 
//                }
                  
              LinkFifoHeaderCounters[ActiveLink] = FifoHeaderCounter+PacketsToSend;
              if ( PacketAskForLink > 0 )
              {
                fStatus.recordSend(LinkStatus) ;
              }
          for ( int PacketIndex=0;PacketIndex<PacketsToSend;PacketIndex+=1) 
          {
            int CurrentHeaderIndex = FifoHeaderCounter+PacketIndex;
              _BGL_TorusPktHdr* HdrPtr = &mHeadersForLink[ Link ][ CurrentHeaderIndex ].mHdr;
              int DestRank             = mHeadersForLink[ Link ][ CurrentHeaderIndex ].mDestRank;
              assert( DestRank != mMyRank );
      
              BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &HdrPtr->swh0;
                BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
                  << " Putting a packet into FIFO " << Link 
                  << " SourceRank=" << (int)MetaData->mSourceRank 
                  << " TotalCount=" << (int)MetaData->mTotalCount
                  << " Offset=" << (int)MetaData->mOffset
                  << " BytesInPayload=" << (int)MetaData->mBytesInPayload
                  << " PacketsRemaining=" << PacketsRemaining
                    << EndLogLine ;
      
              int BytesToSend = MetaData->mBytesInPayload;
              assert( BytesToSend );
              assert( DestRank >= 0 && DestRank < mGroupSize );
              char* BlockToSend = ((char *)mSendBuff) + (mSendOffsetIn[ DestRank ] * mSendTypeSize) + MetaData->mOffset;
      
              // assert( ((unsigned long)BlockToSend & 0xf) == 0);
            _BGL_TorusFifo * NextFifoToSend = mFifoStats[ Link ].mLink;
            memcpy( mAlltoAllvPacket.payload, BlockToSend, BytesToSend );
        
            mAlltoAllvPacket.hdr = *HdrPtr;
        
            BGLTorusInjectPacketImageApp( NextFifoToSend,&mAlltoAllvPacket );
          }
          
          }
      BegLogLine(PKFXLOG_PACKET_ALLTOALLV && (0 == PacketsRemaining ) )
               << " mRecvCountDone=" << mRecvCountDone
               << " GroupSize=" << GroupSize 
               << EndLogLine ; 
       
       

    // While we haven't received all needed packets keep listening
    for ( int k=0;k<PACKET_POLL_COUNT;k+=1)
    {
      // Read the status. TEMPORARY
      ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );

      _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
      assert( mRecvCountDone <= GroupSize );
      }
  }
      
    
  } else {
    // Data known to be on 8-byte boundaries; can go through floating-point regs
    while( ( 0 != PacketsRemaining )  )
      {
          PacketsRemaining = 0 ; 
              // Read the status
              ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );
      
        
  
                // Find the size of the fullest FIFO
            int Stat0 = FifoStatus0.i0 ;
            int Stat1 = FifoStatus0.i1 ;
            int Stat2 = FifoStatus0.i2 ;
            int Stat3 = FifoStatus1.i0 ;
            int Stat4 = FifoStatus1.i1 ;
            int Stat5 = FifoStatus1.i2 ;
            StatusValue[0] = Stat0 ;
            StatusValue[1] = Stat1 ;
            StatusValue[2] = Stat2 ;
            StatusValue[3] = Stat3 ;
            StatusValue[4] = Stat4 ;
            StatusValue[5] = Stat5 ;
//		      fStatus.recordSend(Stat0) ;
//              fStatus.recordSend(Stat1) ;
//              fStatus.recordSend(Stat2) ;
//              fStatus.recordSend(Stat3) ;
//              fStatus.recordSend(Stat4) ;
//              fStatus.recordSend(Stat5) ;
          
           // We have room for k_BlockFactor packets in every injection FIFO
           for ( int ActiveLink=0 ; ActiveLink < ActiveLinkCount ; ActiveLink += 1 )
         {
          int Link=mActiveLink[ActiveLink] ;
          int FifoHeaderCounter = LinkFifoHeaderCounters[ActiveLink] ;
              int FifoHeaderLimit=LinkFifoHeaderLimit[ActiveLink] ;
          int LinkStatus=StatusValue[Link] ; 
          int PacketSpaceForLink=mPacketsToInjectByStatus[LinkStatus] ;
          int PacketAskForLink = FifoHeaderLimit-FifoHeaderCounter ;
          int PacketsToSend = ( PacketAskForLink > PacketSpaceForLink )
                            ? PacketSpaceForLink
                            : PacketAskForLink ;
          PacketsRemaining += PacketAskForLink ;            
          _BGL_TorusFifo * NextFifoToSend = mFifoStats[ Link ].mLink;

//                if ( LinkStatus > 0 || PacketsToSend == 0 ) 
//                { 
//                	PacketsToSend = 0 ; 
//                } else {
//                	PacketsToSend = 1 ; 
//                }
              LinkFifoHeaderCounters[ActiveLink] = FifoHeaderCounter+PacketsToSend;
              if ( PacketAskForLink > 0 )
              {
                fStatus.recordSend(LinkStatus) ;
              }
          for ( int PacketIndex=0;PacketIndex<PacketsToSend;PacketIndex+=1) 
          {
            int CurrentHeaderIndex = FifoHeaderCounter+PacketIndex;
              _BGL_TorusPktHdr* HdrPtr = &mHeadersForLink[ Link ][ CurrentHeaderIndex ].mHdr;
              int DestRank             = mHeadersForLink[ Link ][ CurrentHeaderIndex ].mDestRank;
              assert( DestRank != mMyRank );
      
              BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &HdrPtr->swh0;
                BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
                  << " Putting a packet into FIFO " << Link 
                  << " SourceRank=" << (int)MetaData->mSourceRank 
                  << " TotalCount=" << (int)MetaData->mTotalCount
                  << " Offset=" << (int)MetaData->mOffset
                  << " BytesInPayload=" << (int)MetaData->mBytesInPayload
                  << " PacketsRemaining=" << PacketsRemaining
                    << EndLogLine ;
      
              int BytesToSend = MetaData->mBytesInPayload;
              assert( BytesToSend );
              assert( DestRank >= 0 && DestRank < mGroupSize );
              char* BlockToSend = ((char *)mSendBuff) + (mSendOffsetIn[ DestRank ] * mSendTypeSize) + MetaData->mOffset;
      
              // assert( ((unsigned long)BlockToSend & 0xf) == 0);
                BGLTorusInjectPacketSeparateHeaderApp(NextFifoToSend,HdrPtr,BlockToSend) ;
                
            }
             }
             BegLogLine(PKFXLOG_PACKET_ALLTOALLV && (0 == PacketsRemaining) )
               << " mRecvCountDone=" << mRecvCountDone
               << " GroupSize=" << GroupSize 
               << EndLogLine ; 
      
        
         
  
      // While we haven't received all needed packets keep listening
      for ( int k=0;k<PACKET_POLL_COUNT;k+=1)
      {
        // Read the status. TEMPORARY
        ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );
  
        _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
        assert( mRecvCountDone <= GroupSize );
        }
    }
        
  
  }
  
    // Sending is completed. Listen until reception is completed too.
    while( mRecvCountDone != GroupSize )
      {
      // Read the status. TEMPORARY
      ts_GetStatusBoth( & FifoStatus0, & FifoStatus1 );

      _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
      assert( mRecvCountDone <= GroupSize );
      }
    
  
  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
    << "ExecuteSingleCore1 complete" 
    << EndLogLine ; 

  // Barrier the phase. We need to receive packets from another phase
  // BGLPartitionBarrierComputeCore0();
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
      << "ExecuteSingleCore1 this=" << this
      << EndLogLine ;

  }

#if 0
void PacketAlltoAllv::Execute( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
                               void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                               GroupContainer* aGroup, int debug )
  {
  alltoallv_debug = debug;

  Init( aSendCount, aSendOffset, aSendTypeSize,
        aRecvCount, aRecvOffset, aRecvTypeSize, aGroup );

  Execute( aSendBuf, aRecvBuf );
  Finalize();
  alltoallv_debug = 0;
  }
#endif

static int countToReport=10 ;

void PacketAlltoAllv::ExecuteSingleCore( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                         void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                         GroupContainer* aGroup, int aligned, int debug )
  {
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
      << "ExecuteSingleCore this=" << this
      << EndLogLine ;
  alltoallv_debug = debug;
  InitSingleCore( aSendCount, aSendOffset, aSendTypeSize,
                  aRecvCount, aRecvOffset, aRecvTypeSize, aGroup );
  
//  assert(aligned) ; 
  if( aligned )
    {
    ExecuteSingleCore1<0>( aSendBuf, aRecvBuf );
    }
  else
    {
      ExecuteSingleCore1<1>( aSendBuf, aRecvBuf );
    }
  
  FinalizeSingleCore();
  countToReport -= 1 ;
  if ( 0 == countToReport )
  {
    fStatus.display() ;
  }
  
  alltoallv_debug = 0;
    BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
      << "ExecuteSingleCore this=" << this
      << EndLogLine ;
  
  }

//void PacketAlltoAllv::ExecuteSingleCoreSimple( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
//                                               void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
//                                               GroupContainer* aGroup, int aligned, int debug )
//  {
//  	BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
//  	  << "ExecuteSingleCoreSimple this=" << this
//  	  << EndLogLine ;
////  static int x=0 ;
////  if ( x++ == 92 )
////  {
////  	for(int a=0;a<100000000;a+=1) ; // spin for a second or so
////  	assert(0) ; 
////  }
//  alltoallv_debug = debug;
//  InitSingleCoreSimple( aSendCount, aSendOffset, aSendTypeSize,
//                        aRecvCount, aRecvOffset, aRecvTypeSize, aGroup );
//
//  BegLogLine(PKFXLOG_PACKET_ALLTOALLV) 
//    << "PacketAlltoAllv::ExecuteSingleCoreSimple aSendBuf=" << aSendBuf
//    << " aRecvBuf=" << aRecvBuf
//    << " aligned=" << aligned
//    << EndLogLine ;
////  assert(aligned) ; 
////  if( aligned )
//    ExecuteSingleCoreSimple1<0>( aSendBuf, aRecvBuf );
////  else
////    ExecuteSingleCoreSimple1<1>( aSendBuf, aRecvBuf );
//    
//  FinalizeSingleCoreSimple();
//  countToReport -= 1 ;
//  if ( 0 == countToReport )
//  {
//  	fStatus.display() ;
//  }
//  alltoallv_debug = 0;
//  BegLogLine(PKFXLOG_PACKET_ALLTOALLV)
//  	  << "ExecuteSingleCoreSimple this=" << this
//  	  << EndLogLine ;
//  
//  }
#endif

#endif
