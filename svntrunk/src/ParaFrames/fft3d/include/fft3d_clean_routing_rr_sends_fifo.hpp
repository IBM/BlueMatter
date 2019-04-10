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
 #ifndef __FFT3D_H_
#define __FFT3D_H_

#include <MPI/fft_one_dim.hpp>

#include <fft/ffttypes.hpp>
#include <fft/testelt.hpp>

#include <fft/xyz2ziterator.hpp>
#include <fft/zxy2yiterator.hpp>  
#include <fft/yxz2xyziterator.hpp>
#include <fft/xyz2yiterator.hpp>
#include <fft/yzx2zyxiterator.hpp>
#include <fft/zyx2xyzvoliterator.hpp>
#include <BlueMatter/RandomNumberGenerator.hpp>

//#include <assert.h>
//#include "math.h"
//#include <stdlib.h>
//#include <unistd.h>

#ifndef PKFXLOG_METHOD_ENTER_EXIT
#define PKFXLOG_METHOD_ENTER_EXIT ( 0 )
#endif

#ifndef PKFXLOG_EXECUTE
#define PKFXLOG_EXECUTE ( 0 )
#endif

#ifndef PKFXLOG_CORRUPT_FFT_HEADERS
#define PKFXLOG_CORRUPT_FFT_HEADERS ( 0 )
#endif

#ifndef PKFXLOG_DEBUG_DATA
#define PKFXLOG_DEBUG_DATA ( 0 )
#endif

#ifndef PKTRACE_ACTIVE_MSG_BARRIER
#define PKTRACE_ACTIVE_MSG_BARRIER ( 0 )
#endif

#ifndef PKTRACE_IMPORT_FENCE
#define PKTRACE_IMPORT_FENCE ( 0 )
#endif

#ifndef PKTRACE_PACKET_ACTOR_FX_X
#define PKTRACE_PACKET_ACTOR_FX_X  ( 0 )
#endif

#ifndef PKTRACE_PACKET_ACTOR_FX_Y
#define PKTRACE_PACKET_ACTOR_FX_Y  ( 0 )
#endif

#ifndef PKTRACE_PACKET_ACTOR_FX_Z
#define PKTRACE_PACKET_ACTOR_FX_Z  ( 0 )
#endif


#ifndef PKFXLOG_PACKET_ACTOR_FX_Z
#define PKFXLOG_PACKET_ACTOR_FX_Z  ( 0 )
#endif
#ifndef PKFXLOG_PACKET_ACTOR_FX_Y
#define PKFXLOG_PACKET_ACTOR_FX_Y  ( 0 )
#endif
#ifndef PKFXLOG_PACKET_ACTOR_FX_X
#define PKFXLOG_PACKET_ACTOR_FX_X  ( 0 )
#endif
#ifndef PKFXLOG_PACKET_ACTOR_FX_B
#define PKFXLOG_PACKET_ACTOR_FX_B  ( 0 )
#endif

#ifndef PKFXLOG_GATHER_Z
#define PKFXLOG_GATHER_Z  ( 0 )
#endif
#ifndef PKFXLOG_GATHER_X
#define PKFXLOG_GATHER_X  ( 0 )
#endif
#ifndef PKFXLOG_GATHER_Y
#define PKFXLOG_GATHER_Y  ( 0 )
#endif
#ifndef PKFXLOG_GATHER_B
#define PKFXLOG_GATHER_B  ( 0 )
#endif

#ifndef PKFXLOG_ACTIVE_MSG_SEND
#define PKFXLOG_ACTIVE_MSG_SEND ( 0 )
#endif

#ifndef PKFXLOG_ACTIVE_MSG_BARRIER
#define PKFXLOG_ACTIVE_MSG_BARRIER ( 0 )
#endif

#ifndef PKFXLOG_INIT
#define PKFXLOG_INIT ( 0 )
#endif

#ifndef PKFXLOG_PACKET_PACKER
#define PKFXLOG_PACKET_PACKER ( 0 )
#endif

#ifndef PKFXLOG_INIT_E
#define PKFXLOG_INIT_E ( 0 )
#endif

#ifndef PKFXLOG_INIT_HEADERS
#define PKFXLOG_INIT_HEADERS ( 0 )
#endif

#ifndef PKFXLOG_GET_RECIPSPACE
#define PKFXLOG_GET_RECIPSPACE ( 0 )
#endif

#ifndef PKFXLOG_PUT_RECIPSPACE
#define PKFXLOG_PUT_RECIPSPACE ( 0 )
#endif

#ifndef PKFXLOG_GET_REALSPACE
#define PKFXLOG_GET_REALSPACE ( 0 )
#endif

#ifndef PKFXLOG_PUT_REALSPACE
#define PKFXLOG_PUT_REALSPACE ( 0 )
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT
#define PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT ( 0 )
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT
#define PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT ( 0 )
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_X_1D_FFT
#define PKFXLOG_PACKET_ACTOR_FX_X_1D_FFT ( 0 )
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_B_1D_FFT
#define PKFXLOG_PACKET_ACTOR_FX_B_1D_FFT ( 0 )
#endif

#ifndef PKTRACE_TUNE 
#define PKTRACE_TUNE ( 1 )
#endif

#ifndef PKTRACE_NOT_SENT
#define PKTRACE_NOT_SENT ( 0 )
#endif

#ifndef PKTRACE_PACK_PACKET
#define PKTRACE_PACK_PACKET ( 0 )
#endif

#ifndef PKTRACE_FORWARD_FFT 
#define PKTRACE_FORWARD_FFT ( 1 )
#endif

#ifndef PKTRACE_REVERSE_FFT 
#define PKTRACE_REVERSE_FFT ( 0 )
#endif

#ifndef PKTRACE_TUNE_1D_FFT_Y
#define PKTRACE_TUNE_1D_FFT_Y ( 0 )
#endif

#ifndef PKTRACE_TUNE_1D_FFT_X
#define PKTRACE_TUNE_1D_FFT_X ( 0 )
#endif


#ifndef PKFXLOG_GATHER_Z_TRIM
#define PKFXLOG_GATHER_Z_TRIM ( 0 )
#endif

#ifndef PKFXLOG_LOAD_BALANCING
#define PKFXLOG_LOAD_BALANCING ( 0 )
#endif

#ifndef PKFXLOG_COMPUTEFFTSINQUEUE_Z
#define PKFXLOG_COMPUTEFFTSINQUEUE_Z ( 0 )
#endif

#ifndef PKFXLOG_COMPUTEFFTSINQUEUE_Y
#define PKFXLOG_COMPUTEFFTSINQUEUE_Y ( 0 )
#endif

#ifndef PKFXLOG_COMPUTEFFTSINQUEUE_X
#define PKFXLOG_COMPUTEFFTSINQUEUE_X ( 0 )
#endif

#ifndef PKTRACE_OFF
#define PKTRACE_OFF ( 1 )
#endif

#ifndef PKTRACE_SEND_IN_Z
#define PKTRACE_SEND_IN_Z ( 0 )
#endif

#ifndef PKTRACE_SEND_IN_Y
#define PKTRACE_SEND_IN_Y ( 0 )
#endif

#ifndef PKTRACE_SEND_IN_X
#define PKTRACE_SEND_IN_X ( 0 )
#endif

#ifndef PKFXLOG_RESET
#define PKFXLOG_RESET ( 0 )
#endif

#ifndef FFT3D_STUCT_ALIGNMENT
#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))
#endif

#ifndef FFT3D_BARRIER_PHASES
#define FFT3D_BARRIER_PHASES 1
#endif

#ifndef PKTRACE_CORE_1_DONE_COMPUTE
#define PKTRACE_CORE_1_DONE_COMPUTE ( 0 )
#endif

#ifndef PKTRACE_CORE_1_DONE_FFT_RECV
#define PKTRACE_CORE_1_DONE_FFT_RECV ( 0 )
#endif

//#define FORCE_SELF_SEND_HINT_BITS_Y

static TraceClient Core1ComputationStart;
static TraceClient Core1ComputationFinis;
static TraceClient Core0ComputationStart;
static TraceClient Core0ComputationFinis;
static TraceClient Core1FFTRecvDone;

static TraceClient fftForwardTracerStart;
static TraceClient fftForwardTracerFinis;

static TraceClient fftReverseTracerStart;
static TraceClient fftReverseTracerFinis;

static TraceClient TR_SendPackets_ZStart;
static TraceClient TR_SendPackets_ZFinis;

static TraceClient TR_SendPackets_YStart;
static TraceClient TR_SendPackets_YFinis;

static TraceClient TR_SendPackets_XStart;
static TraceClient TR_SendPackets_XFinis;

static TraceClient TR_FFT_1D_ZStart;
static TraceClient TR_FFT_1D_ZFinis;

static TraceClient TR_FFT_1D_YStart;
static TraceClient TR_FFT_1D_YFinis;

static TraceClient TR_FFT_1D_XStart;
static TraceClient TR_FFT_1D_XFinis;

static TraceClient TR_1DFFT_READY_X;
static TraceClient TR_1DFFT_READY_Y;
static TraceClient TR_1DFFT_READY_Z;

static TraceClient TR_BGLActivePacketSendStart;
static TraceClient TR_BGLActivePacketSendFinis;

static TraceClient TR_Sender_Compute_Z_Start;
static TraceClient TR_Sender_Compute_Z_Finis;

static TraceClient TR_Sender_Compute_Y_Start;
static TraceClient TR_Sender_Compute_Y_Finis;

static TraceClient TR_Sender_Compute_X_Start;
static TraceClient TR_Sender_Compute_X_Finis;

static TraceClient SendInX_fwdStart;
static TraceClient SendInX_fwdFinis;
static TraceClient SendInY_fwdStart;
static TraceClient SendInY_fwdFinis;
static TraceClient SendInZ_fwdStart;
static TraceClient SendInZ_fwdFinis;

static TraceClient BGLHeaderCreation;

static TraceClient SendInY_rev;
static TraceClient SendInZ_rev;
static TraceClient SendInXYZ_rev;

static TraceClient  Full_Send_Queue_State_Start;
static TraceClient  Full_Send_Queue_State_Finis;

static TraceClient TracingCost;

static TraceClient FillPacketStart;
static TraceClient FillPacketFinis;

static TraceClient NotSentStart;
static TraceClient NotSentFinis;

static TraceClient ActiveMsgBarrierStart;
static TraceClient ActiveMsgBarrierFinis;

static TraceClient ImportFenceStart;
static TraceClient ImportFenceFinis;

#define max(i, j) ( (i>j) ? i:j )
#define min(i, j) ( (i<j) ? i:j )

#define END_OF_PACKET 0xabdcefed

// alignment of 8 is double, 16 is quad, 2**5=32 BGL cache, 2**6=64 BGL L3



void BGLTorusSimpleSend( _BGL_TorusPktHdr & aPktHdr,
                         void             * aData,
                         int                aLen,
                         int                aSendToSelf )
{           
  if( aSendToSelf )
    {
      VirtualFifo_BGLTorusSendPacketUnaligned( & aPktHdr,      // Previously created header to use
                                               aData,     // Source address of data (16byte aligned)
                                               aLen );      
    }
  else
    {
      BGLTorusSendPacket( & aPktHdr,  // Previously created header to use
                          aData,    // Source address of data (16byte aligned)
                          aLen );   // Payload bytes = ((chunks * 32) - 16)                   
    }
}

extern "C"
{
void* memcpy_old(void * trg, void* src, unsigned int size );
void fastmover_quads(_BGL_TorusFifo *fifo, void* data_dst, int n );
int MyBGLTorusInjectPacket( _BGL_TorusFifo          *fifo,  // Injection Fifo to use (must have space available!)
                            _BGL_TorusPktHdr        *hdr,   // Previously created header to use
                            Bit8                    *FifoStatus,
                            _BGL_TorusFifoStatus    *FifoStatusSet, 
                            Bit128                  *FifoGroup,
                            void *data,              // Source address of data (16byte aligned)
                            int bytes );             // Payload bytes = ((chunks * 32) - 16)

};
#if 0
inline void
BGLTorusFifoActivePacketSend( _BGL_TorusPktHdr     & aPktHdr,
                              void                 * aData, 
                              int                    aLen,
                              int                    aSendToSelf,
                              int                    aFifoIndex );
//                               _BGL_TorusFifo       * aFifo,
//                               Bit8                 * aFifoStatus,
//                               _BGL_TorusFifoStatus * aFifoStatusSet,                                 
//                               Bit128               * aFifoGroup)                                        
{
  if( aSendToSelf )
    {
      VirtualFifo_BGLTorusSendPacketUnaligned( & aPktHdr, // Previously created header to use
                                               aData,     // Source address of data (16byte aligned)
                                               aLen );      
    }
  else
    {
      MyBGLTorusInjectPacket( aFifo, 
                              & aPktHdr,
                              aFifoStatus,
                              aFifoStatusSet,
                              aFifoGroup,
                              aData,
                              aLen );
//       BGLTorusSendPacket( & aPktHdr,  // Previously created header to use
//                           aData,    // Source address of data (16byte aligned)
//                           aLen );   // Payload bytes = ((chunks * 32) - 16)                         
    }
}
#endif

inline int round_to_32( int in )
{
  int Rmd = in & 0x0000001F;
  in += ( 32 - Rmd ) & 0x0000001F;
  
  return in;
}

extern void VirtualFifo_BGLTorusSendPacketUnaligned( _BGL_TorusPktHdr *hdr,   
                                                     void             *data,  
                                                     int               bytes );

// extern "C" 
// {
// void* marks_memcpy(void * trg, void* src, unsigned int size );
// };

//#undef memcpy

#define pk512_MB ( 512 * 1024 * 1024 )

inline void alex_memcpy1( char* t, char* s, unsigned int numBytes)
{    
  for(int i=0; i < numBytes; i++)
    {
      t[ i ] = s[ i ];
    }
}

// void memcpy(void *trg, void* src, unsigned int  

#define alex_memcpy memcpy
#define double_copy memcpy

// inline
// void
// double_copy( double* t, double* s, unsigned int numDoubles)
// {
// //   int i ;               
// //   for(i=0; i < numDoubles-3; i+=4)
// //     {
// //       double s0 = s[i] ; 
// //       double s1 = s[i+1] ; 
// //       double s2 = s[i+2] ; 
// //       double s3 = s[i+3] ; 
      
// //       t[ i ] = s0;
// //       t[ i + 1 ] = s1;
// //       t[ i + 2 ] = s2;
// //       t[ i + 3 ] = s3;
// //     }

//   BegLogLine( 0 )
//     << "double_copy()::  "
//     << "t=" << (void *) t
//     << "s=" << (void *) s
//     << "numDoubles=" << numDoubles
//     << EndLogLine;

//   for (int i=0;i<numDoubles;i+=1)
//     {
//       t[i] = s[i];
//     }
// }


struct ActorPacket
{
  char  mData[ sizeof( _BGL_TorusPktPayload ) ];
};

struct EndOfPacket
{
  unsigned mValue;
};

#ifdef TEST_FFT_DATA_MOVEMENT
    typedef fft::TestElt Value;
#else
    typedef fft3d_complex Value;
#endif

template< class Router_T, class FFT_T >
class PacketPacker
{
public:
  typedef  typename Router_T::iterator Iterator FFT3D_STUCT_ALIGNMENT;

  Router_T*   mRouter FFT3D_STUCT_ALIGNMENT;
  Iterator iter FFT3D_STUCT_ALIGNMENT;

  typedef typename FFT_T::FFT_Hdr FFT_Hdr;

  int      PacketCapacity FFT3D_STUCT_ALIGNMENT;
  int      SizeOfFFTHdrAndOneElement FFT3D_STUCT_ALIGNMENT;
  int      mDestX FFT3D_STUCT_ALIGNMENT;
  int      mDestY FFT3D_STUCT_ALIGNMENT;

  FFT_Hdr  mSkratchFFTHeader FFT3D_STUCT_ALIGNMENT;
 
  int      mDestZ FFT3D_STUCT_ALIGNMENT;
  Bit32    mActorFx FFT3D_STUCT_ALIGNMENT;  
  EndOfPacket mEndOfPacket FFT3D_STUCT_ALIGNMENT;
  int      mPreserveState FFT3D_STUCT_ALIGNMENT;
  
  int      mSavedOffset FFT3D_STUCT_ALIGNMENT;
  int      dummy1, dummy2,dummy3;
  
  int      mSavedCount FFT3D_STUCT_ALIGNMENT;
  int      mSavedPrevCount FFT3D_STUCT_ALIGNMENT;
  FFT_T**  mStaticThisPtr  FFT3D_STUCT_ALIGNMENT;
  const fft::RLEPos*   rle FFT3D_STUCT_ALIGNMENT;
  unsigned int                  mGenDataPtr  FFT3D_STUCT_ALIGNMENT;

  PacketPacker(){}
  
  void Init( int destX, int destY, int destZ, Bit32 ActorFx, Router_T* Router, FFT_T** aThis, int debug=0 )
    {
      BegLogLine( 0 )
        << "PacketPacker::Init() "
        << " destX=" << destX
        << " destY=" << destY
        << " destZ=" << destZ
        << " Router*=" << (void *) Router
        << " ActorFx*=" << (void *) ActorFx
        << EndLogLine;                 
      
      mStaticThisPtr = aThis;
      mDestX = destX;
      mDestY = destY;
      mDestZ = destZ;
      
      mActorFx = ActorFx;
      mRouter = Router;
      
      mPreserveState = 0;

      rle  = mRouter->rle( destX, destY, destZ );
      // mRouter->ValidateCheckSum( __LINE__ );

      iter = mRouter->begin( destX, destY, destZ );

//       BegLogLine( debug )
//         << "PacketPacker::Init() "
//         << "iter.x()= " << iter.x()  
//         << EndLogLine;
        
      SizeOfFFTHdrAndOneElement = sizeof( FFT_Hdr ) + sizeof( Value );
      PacketCapacity = sizeof( _BGL_TorusPktPayload ) - sizeof( EndOfPacket );
      mEndOfPacket.mValue = END_OF_PACKET;
    }  
    
  void GetNextPacket( ActorPacket* aPacketOut, int* SizeOfPacket, int aPreGeneration=0, int debug=0 )
    {
      BegLogLine( PKFXLOG_PACKET_PACKER )
        << "GetNextPacket():: entering"
        << EndLogLine;

      int      CurrentOffsetInPacket = 0;
      int      PacketSpaceLeft       = PacketCapacity;
 
      // Iterate over the linked list and fill the packets
      while( rle != NULL )
        {
          // mRouter->ValidateCheckSum( __LINE__ );

          int FFTIndex = rle->fftIndex();
          int Offset   = rle->offset();                    
          int CurrentCount    = rle->count(); 
          int prevCount = 0;

          if( mPreserveState )
            {
            CurrentCount = mSavedCount;
            Offset       = mSavedOffset;
            prevCount    = mSavedPrevCount;
            }
          
          mSkratchFFTHeader.ThisPtr  = mStaticThisPtr;
          mSkratchFFTHeader.fftIndex = FFTIndex;
          mSkratchFFTHeader.offset   = Offset;
          

          BegLogLine( PKFXLOG_PACKET_PACKER )
            << "GetNextPacket():: "
            << " FFTIndex=" << FFTIndex
            << " Offset=" << Offset
            << " CurrentCount=" << CurrentCount
            << EndLogLine;

          // Iterate
          while( CurrentCount != 0 )
            {              
              // mRouter->ValidateCheckSum( __LINE__ );

              PacketSpaceLeft -= sizeof( FFT_Hdr );
              
              int NumberOfElementsNeededToFit        = CurrentCount;
              int NumberOfElementsThatCanFitInPacket = ( PacketSpaceLeft >> 4 );
              
              int SpaceDiff = NumberOfElementsThatCanFitInPacket - NumberOfElementsNeededToFit;
              
              // CONDITION:: There's at least enough room for an fft hdr and one element
              if( SpaceDiff >= 0 )
                {
                  // Let's add the chunk of data into the current packet            
                  mSkratchFFTHeader.count    = CurrentCount;                            
                }
              else 
                {
                  // Fit as much data in the packet as you can and let the subsequent packets
                  // take care of the rest.
                  mSkratchFFTHeader.count = NumberOfElementsThatCanFitInPacket;                  
                }
              
              mSkratchFFTHeader.offset += prevCount;
              
              // check if the count can fit into the current packet
              alex_memcpy((char *) & aPacketOut->mData[ CurrentOffsetInPacket ],  (char *) &mSkratchFFTHeader, sizeof( FFT_Hdr ));
              CurrentOffsetInPacket += sizeof( FFT_Hdr );
              
              for( int eleI=0; eleI < mSkratchFFTHeader.count; eleI++ )
                {
                  if( (CurrentOffsetInPacket >= sizeof( _BGL_TorusPktPayload )) || (CurrentOffsetInPacket < 0 ) )
                    BegLogLine( 0 )
                      << "GetNextPacket:: "
                      << "CurrentOffsetInPacket=" << CurrentOffsetInPacket
                      << "aPacketOut->mData[ " << CurrentOffsetInPacket << "]=" 
                      << (void * ) &aPacketOut->mData[ CurrentOffsetInPacket ]
                      << EndLogLine;
                  
                  if ( aPreGeneration )
                    {
                      // Store the address instead of the data
                      // During the actual send we convert to real data
                      mGenDataPtr = (unsigned int) &( *iter );                      
                      alex_memcpy( &aPacketOut->mData[ CurrentOffsetInPacket ], &mGenDataPtr, sizeof( unsigned int ) );                      
                      
                      BegLogLine( 0 )
                        << "GetNextPacket:: "
                        << " sizeof( unsigned )= " << sizeof( unsigned )
                        << " sizeof( unsigned int )= " << sizeof( unsigned int )
                        << " mGenDataPtr=" << (void *) mGenDataPtr
                        << " &mGenDataPtr=" << (void *) &mGenDataPtr
                        << " aPacketOut->mData[ " << CurrentOffsetInPacket << "]=" << (void *) aPacketOut->mData[ CurrentOffsetInPacket ]
                        << EndLogLine;

                    }
                  else
                    {   
                      alex_memcpy( &aPacketOut->mData[ CurrentOffsetInPacket ], &( *iter ), sizeof( Value ) );
                    }
                  
                  CurrentOffsetInPacket += sizeof( Value );
                  ++iter;
                }
                            
              PacketSpaceLeft -= ( sizeof( Value ) * mSkratchFFTHeader.count );
              
              CurrentCount -= mSkratchFFTHeader.count;
              prevCount = mSkratchFFTHeader.count;
              
              // Always reset the preserve state bit.
              mPreserveState = 0;
              if( PacketSpaceLeft < SizeOfFFTHdrAndOneElement )
                {                                    
                  /******************************************************************************
                   *                            Send Packet
                   ******************************************************************************/                  
                  // We've filled a full packed. Time to seal and send 'em out.                  
                  alex_memcpy( (char *)  & aPacketOut->mData[ CurrentOffsetInPacket ], (char *) &mEndOfPacket, sizeof( EndOfPacket ) );                
                  CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                  
                  int SizeToSend = CurrentOffsetInPacket;
                  if( SizeToSend % 32 != 0 )
                    SizeToSend = SizeToSend + (32 - ( SizeToSend % 32 ));                  
                  
                  // *SizeOfPacket = SizeToSend;
                  *SizeOfPacket = CurrentOffsetInPacket;

                  // 1. We could exit with CurrentCount != 0, in which case the next call to GetNext() needs to 
                  // preserve the CurrentCount, mSkratchFFTHeader
                  if( CurrentCount != 0 )
                    {
                    mPreserveState  = 1;
                    mSavedCount     = CurrentCount;
                    mSavedOffset    = mSkratchFFTHeader.offset;
                    mSavedPrevCount = prevCount;
                    }
                  else
                    {
                      // If CurrentCount == 0 then we need to update the rle pointer. Remember that we're returning.
                      rle = rle->next();
                    }

                  // 2. We could exit with CurrentCount == 0, no need to preserve anything, the next call to GetNext()
                  // should start off with a new link in the linked list. No need to preserve state.

                  BegLogLine( PKFXLOG_PACKET_PACKER )
                    << "GetNextPacket():0: "
                    << " Sending a packet "
                    << " CurrentOffsetInPacket=" << CurrentOffsetInPacket
                    << " CurrentCount=" << CurrentCount
                    << " mSkratchFFTHeader.offset=" << mSkratchFFTHeader.offset                    
                    << " prevCount=" << prevCount
                    << " PacketSpaceLeft=" << PacketSpaceLeft
                    << EndLogLine;

                  return;

//                   _BGL_TorusPktHdr   BGLTorus_Hdr;
//                   BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
//                                    mDestX,
//                                    mDestY,
//                                    mDestZ,                       // destination coordinates
//                                    0,                           // ???      // destination Fifo Group
//                                    (Bit32) mActorFx,     // Active Packet Function matching signature above
//                                    0,                           // primary argument to actor
//                                    0,                           // secondary 10bit argument to actor
//                                    _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                                    
//                   Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,.rle( destPy, destPz );
//                                                                      (Bit32) mActorFx,
//                                                                      mDestX,
//                                                                      mDestY,
//                                                                      mDestZ,
//                                                                      CurrentOffsetInPacket,
//                                                                      &xyz_ap);                  
                  /*******************************************************************************/
                }              
            }
          
          rle =  rle->next();
          
          if( rle == NULL )
            {
              if( CurrentOffsetInPacket != 0 )
                {
                  // Send the last packet if one is pending, break from the loop
                  /******************************************************************************
                   *                            Send Packet
                   ******************************************************************************/                  
                  alex_memcpy((char *)  &aPacketOut->mData[ CurrentOffsetInPacket ], (char *) &mEndOfPacket, sizeof( EndOfPacket ) );
                  CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        

                  int SizeToSend = CurrentOffsetInPacket;
                  if( SizeToSend % 32 != 0 )
                    SizeToSend = SizeToSend + (32 - ( SizeToSend % 32 ));                  
                  
                  // *SizeOfPacket = SizeToSend;
                  *SizeOfPacket = CurrentOffsetInPacket;
                  
                  BegLogLine( PKFXLOG_PACKET_PACKER )
                    << "GetNextPacket():1: "
                    << " Sending a packet "
                    << " CurrentOffsetInPacket=" << CurrentOffsetInPacket
                    << " CurrentCount=" << CurrentCount
                    << " mSkratchFFTHeader.offset=" << mSkratchFFTHeader.offset                    
                    << " prevCount=" << prevCount
                    << EndLogLine;

                  return;
//                   _BGL_TorusPktHdr   BGLTorus_Hdr;
//                   BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return                                   
                  //                                    mDestX,
//                                    mDestY,
//                                    mDestZ,                      // destination coordinates
//                                    0,                           // destination Fifo Group
//                                    (Bit32) mActorFx, // Active Packet Function matching signature above
                  //                                    0,                           // primary argument to actor
                  //                                    0,                           // secondary 10bit argument to actor
                  //                                    _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                  
                  //                   Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                  //                                                                      (Bit32) mActorFx,
                  //                                                                      mDestX,
                  //                                                                      mDestY,
                  //                                                                      mDestZ,
                  //                                                                      CurrentOffsetInPacket,
                  //                                                                      & xyz_ap);
                  
                  /*******************************************************************************/
                  CurrentOffsetInPacket = 0;
                  PacketSpaceLeft = PacketCapacity;
                }
              break;
            }
        }
      
      if( rle == NULL )
        {
          aPacketOut = NULL;
          *SizeOfPacket = 0;          
        }
    }
};

//FFT3D_STUCT_ALIGNMENT
template< class FFT_PLAN >
class FFT3D
{
public:
  
  typedef FFT_PLAN FFT_PLAN_IF;
  
  int Init( FFT3D** StaticThisPtr, 
            int proc_x, int proc_y, int proc_z ) ;
  //  int mesh_x, int mesh_y, int mesh_z );
  
  inline void GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
                                  int& aLocalY, int& aLocalSizeY,
                                  int& aLocalZ, int& aLocalSizeZ,
                                  int& aLocalRecipX, int& aLocalRecipSizeX,
                                  int& aLocalRecipY, int& aLocalRecipSizeY,
                                  int& aLocalRecipZ, int& aLocalSizeRecipZ );
  
  inline void RunForward();
  inline void RunReverse();
  
  inline void Reset();
  
  inline void ZeroRealSpace();
  inline double GetRealSpaceElement( int x, int y, int z );
  inline void PutRealSpaceElement( int x, int y, int z, double );
  inline Value GetRecipSpaceElement( int kx, int ky, int kz );
  inline void PutRecipSpaceElement( int kx, int ky, int kz, Value );
  
  inline void ScaleIn(double);
  
  FFT3D()
    {}
  
  struct FFT_Hdr
  {
    FFT3D **ThisPtr;
    short fftIndex;
    short offset;
    short count;
    short padding;
    int   padding1;  
  }; // should now be 16 bytes.
  
private:
  
  enum
  {
    // ELEMENTS_PER_PACKET = ( (sizeof(_BGL_TorusPktPayload ) - sizeof(FFT_Hdr)) / sizeof( Value) ),
    ELEMENTS_PER_PACKET = 14
  };
  
public:
  
  enum { N_X = FFT_PLAN::GLOBAL_SIZE_X };
  enum { N_Y = FFT_PLAN::GLOBAL_SIZE_Y };
  enum { N_Z = FFT_PLAN::GLOBAL_SIZE_Z };
  
#ifdef TEST_FFT_DATA_MOVEMENT
  void CheckDataMovementAfterFWD();
  void InitCoords();
#endif
  
private:
  
  int PX;
  int PY;
  int PZ;
  
  fft::FFTMesh*  mFFTMesh  FFT3D_STUCT_ALIGNMENT;
  fft::ProcMesh* mProcMesh FFT3D_STUCT_ALIGNMENT;
  
  // 1D FFTs in each direction
  typedef FFTOneD< N_X, -1 > FFT_1D_X_FWD;
  typedef FFTOneD< N_Y, -1 > FFT_1D_Y_FWD;
  typedef FFTOneD< N_Z, -1 > FFT_1D_Z_FWD;
  
  typedef FFTOneD< N_X, 1 > FFT_1D_X_REV;
  typedef FFTOneD< N_Y, 1 > FFT_1D_Y_REV;
  typedef FFTOneD< N_Z, 1 > FFT_1D_Z_REV;
  
  FFT_1D_X_FWD mFFT_FWD_x FFT3D_STUCT_ALIGNMENT;
  FFT_1D_Y_FWD mFFT_FWD_y FFT3D_STUCT_ALIGNMENT;
  FFT_1D_Z_FWD mFFT_FWD_z FFT3D_STUCT_ALIGNMENT;
  
  FFT_1D_X_REV mFFT_REV_x FFT3D_STUCT_ALIGNMENT;
  FFT_1D_Y_REV mFFT_REV_y FFT3D_STUCT_ALIGNMENT;
  FFT_1D_Z_REV mFFT_REV_z FFT3D_STUCT_ALIGNMENT;
  
  typedef fft::SubVolume< Value > SubVolume;    

    // We need 2 containers. Send from 1st, 2nd recvs, swap for next phase
    // Initial data goes into 1st.
    // Send_Z_fwd: Send from 1st, recv 2nd, do ffts in 2nd
    // Send_Y_fwd: Send from 2nd, recv 1st, do ffts in 1st
    // Send_X_fwd: Send from 1nd, recv 2st, do ffts in 2st

    // Send_x_rev: Send from 2st, recv 1nd, do ffts in 1nd
    // Send_Y_rev: Send from 1nd, recv 2st, do ffts in 2st
    // Send_Z_rev: Send from 2nd, recv 1st, do ffts in 1st

    SubVolume   mDataContainer1 FFT3D_STUCT_ALIGNMENT;
    SubVolume   mDataContainer2 FFT3D_STUCT_ALIGNMENT;
    
    SubVolume*  mZYXVol2ZyxPhaseData_S FFT3D_STUCT_ALIGNMENT;
    SubVolume*  mZYXVol2ZyxPhaseData_R FFT3D_STUCT_ALIGNMENT;

    SubVolume*  mZyx2YxzPhaseData_S FFT3D_STUCT_ALIGNMENT;
    SubVolume*  mZyx2YxzPhaseData_R FFT3D_STUCT_ALIGNMENT;

    SubVolume*  mYxz2XyzPhaseData_S FFT3D_STUCT_ALIGNMENT;
    SubVolume*  mYxz2XyzPhaseData_R FFT3D_STUCT_ALIGNMENT;

    SubVolume*  mXyz2YzxPhaseData_S FFT3D_STUCT_ALIGNMENT;
    SubVolume*  mXyz2YzxPhaseData_R FFT3D_STUCT_ALIGNMENT;
    
    SubVolume*  mYzx2ZyxPhaseData_S FFT3D_STUCT_ALIGNMENT;
    SubVolume*  mYzx2ZyxPhaseData_R FFT3D_STUCT_ALIGNMENT;

    SubVolume*  mZyx2XYZVolPhaseData_S FFT3D_STUCT_ALIGNMENT;
    SubVolume*  mZyx2XYZVolPhaseData_R FFT3D_STUCT_ALIGNMENT;

    int mLocalNx FFT3D_STUCT_ALIGNMENT;  
    int mLocalNy FFT3D_STUCT_ALIGNMENT; 
    int mLocalNz FFT3D_STUCT_ALIGNMENT;

    int mIsUni FFT3D_STUCT_ALIGNMENT;
    
    typedef fft::XYZ2ZRouter< SubVolume >      ZYXVol2ZyxRouter;
    typedef fft::Zxy2YRouter< SubVolume >      Zyx2YxzRouter;
    typedef fft::Yxz2XyzRouter< SubVolume >    Yxz2XyzRouter;
    typedef fft::Xyz2YRouter< SubVolume >      Xyz2YzxRouter;    
    typedef fft::Yzx2ZyxRouter< SubVolume >    Yzx2ZyxRouter;
    typedef fft::Zyx2XYZVolRouter< SubVolume > Zyx2ZyxVolRouter;


    typedef fft::XYZ2ZIterator< SubVolume > ZYXVol2ZyxIterator;
    typedef fft::Zxy2YIterator< SubVolume > Zyx2YxzIterator;
    typedef fft::Yxz2XyzIterator< SubVolume > Yxz2XyzIterator;
    typedef fft::Xyz2YIterator< SubVolume > Xyz2YzxIterator;
    typedef fft::Yzx2ZyxIterator< SubVolume > Yzx2ZyxIterator;
    typedef fft::Zyx2XYZVolIterator< SubVolume > Zyx2ZyxVolIterator;

    PacketPacker< ZYXVol2ZyxRouter, FFT3D > mZYXVol2ZyxPP[2] FFT3D_STUCT_ALIGNMENT;
    PacketPacker< Zyx2YxzRouter, FFT3D >    mZyx2YxzPP[4] FFT3D_STUCT_ALIGNMENT;
    PacketPacker< Yxz2XyzRouter, FFT3D >    mYxz2XyzPP[4] FFT3D_STUCT_ALIGNMENT;

  PacketPacker< Xyz2YzxRouter, FFT3D >    mXyz2YzxPP[4] FFT3D_STUCT_ALIGNMENT;
  PacketPacker< Yzx2ZyxRouter, FFT3D >    mYzx2ZyxPP[4] FFT3D_STUCT_ALIGNMENT;
  PacketPacker< Zyx2ZyxVolRouter, FFT3D > mZyx2ZyxVolPP[2] FFT3D_STUCT_ALIGNMENT;
  
  
  // Forward Routers
  ZYXVol2ZyxRouter mSendInZRouterFwd FFT3D_STUCT_ALIGNMENT;
  Zyx2YxzRouter    mSendInYRouterFwd FFT3D_STUCT_ALIGNMENT;
  Yxz2XyzRouter    mSendInXRouterFwd FFT3D_STUCT_ALIGNMENT;
  
  // Reverse Routers
  Xyz2YzxRouter    mSendInYRouterRev FFT3D_STUCT_ALIGNMENT;
  Yzx2ZyxRouter    mSendInZRouterRev FFT3D_STUCT_ALIGNMENT;
  Zyx2ZyxVolRouter mSendInXYZVolRouterRev FFT3D_STUCT_ALIGNMENT;
  
  int* mTotalRecvData_x FFT3D_STUCT_ALIGNMENT;
  int* mTotalRecvData_y FFT3D_STUCT_ALIGNMENT;
  int* mTotalRecvData_z FFT3D_STUCT_ALIGNMENT;
  int  mTotalRecvData_xyzvol FFT3D_STUCT_ALIGNMENT;
  
  typedef AtomicNative< int > FFT_Counter;
  
  FFT_Counter mTotalPointsRecv_x FFT3D_STUCT_ALIGNMENT;
  FFT_Counter mTotalPointsRecv_y FFT3D_STUCT_ALIGNMENT;
  FFT_Counter mTotalPointsRecv_z FFT3D_STUCT_ALIGNMENT;
  
  FFT_Counter mIOP_FFT_Compute_Done_x FFT3D_STUCT_ALIGNMENT;
  FFT_Counter mIOP_FFT_Compute_Done_y FFT3D_STUCT_ALIGNMENT;
  FFT_Counter mIOP_FFT_Compute_Done_z FFT3D_STUCT_ALIGNMENT;
  
  FFT_Counter mCompleteFFTs_x FFT3D_STUCT_ALIGNMENT;
  FFT_Counter mCompleteFFTs_y FFT3D_STUCT_ALIGNMENT;
  FFT_Counter mCompleteFFTs_z FFT3D_STUCT_ALIGNMENT;
  FFT_Counter mCompleteXYZVol FFT3D_STUCT_ALIGNMENT;
  
  
  Value** mFFT_1D_Queue_X FFT3D_STUCT_ALIGNMENT;
  Value** mFFT_1D_Queue_Y FFT3D_STUCT_ALIGNMENT;
  Value** mFFT_1D_Queue_Z FFT3D_STUCT_ALIGNMENT;
  
  int mMyP_x FFT3D_STUCT_ALIGNMENT;
  int mMyP_y FFT3D_STUCT_ALIGNMENT;
  int mMyP_z FFT3D_STUCT_ALIGNMENT;
  
  // communication related data struct
  FFT_Hdr mSkratchFFTHeader FFT3D_STUCT_ALIGNMENT;    
  
  char mRecvBuff[ sizeof( FFT_Hdr ) ] FFT3D_STUCT_ALIGNMENT;

  int mNprocs_x FFT3D_STUCT_ALIGNMENT;
  int mNprocs_y FFT3D_STUCT_ALIGNMENT;
  int mNprocs_z FFT3D_STUCT_ALIGNMENT;
  
  enum 
  { 
    TORUS_XP=0,
    TORUS_XM=1,
    TORUS_YP=2,
    TORUS_YM=3,
    TORUS_ZP=4,
    TORUS_ZM=5
  } TORUS_ENUM;
  
  struct BGLTorus_FFT_Headers
  {
    _BGL_TorusPktHdr mBGLTorus_Hdr;
    FFT_Hdr          mFFT_Hdr;
  } FFT3D_STUCT_ALIGNMENT;
  
  struct BGLSendArgs
  {      
    int mSendToSelf;
    int mSizeOfPacket;
    int mProc_Y;
    int mProc_Z;
  } FFT3D_STUCT_ALIGNMENT;
  
  struct BGLFifoInfo
  {
    _BGL_TorusFifo*          mFifo;
    Bit8*                    mFifoStatusPtr;              
    _BGL_TorusFifoStatus*    mFifoStatusSetPtr; 
    Bit128*                  mFifoGroup;    
  } FFT3D_STUCT_ALIGNMENT;
  
  
  BGLFifoInfo mFifos[ 6 ];

  struct PregeneratedBGLMetaPacket
  {
    _BGL_TorusPktHdr  mBGLTorus_Hdr;
    ActorPacket       mPayload;      
    BGLSendArgs       mBGLSendArgs;
    int               mFifoIndex[ 2 ]; // At most 1 possible directions (on a mesh)
    // BGLFifoInfo       mFifoInfo;
  }  FFT3D_STUCT_ALIGNMENT;

  PregeneratedBGLMetaPacket mTempSwap FFT3D_STUCT_ALIGNMENT;

  struct ComputeFFTsInQueueActorPacket
  {
    int           mStart;
    int           mCount;
    FFT3D         **ThisPtr;      
  } FFT3D_STUCT_ALIGNMENT;
  
  EndOfPacket mEndOfPacket FFT3D_STUCT_ALIGNMENT;
  
    ActorPacket mTempActorPacket FFT3D_STUCT_ALIGNMENT;
    _BGL_TorusPktHdr mTempBglHdr FFT3D_STUCT_ALIGNMENT;
    unsigned int* mDataPtr FFT3D_STUCT_ALIGNMENT;
    

    ActorPacket mPackets[ 4 ] FFT3D_STUCT_ALIGNMENT;
    // ActorPacket mPackets1[ 4 ] FFT3D_STUCT_ALIGNMENT;

    int         mRetSizes[ 4 ]  FFT3D_STUCT_ALIGNMENT;
    
    ActorPacket xyz_ap   FFT3D_STUCT_ALIGNMENT;

    RandomNumberGenerator mRand FFT3D_STUCT_ALIGNMENT;
    long                  mSeed FFT3D_STUCT_ALIGNMENT;

    PregeneratedBGLMetaPacket* mGeneratedPackets_fwd_X FFT3D_STUCT_ALIGNMENT;
    PregeneratedBGLMetaPacket* mGeneratedPackets_fwd_Y FFT3D_STUCT_ALIGNMENT;
    PregeneratedBGLMetaPacket* mGeneratedPackets_fwd_Z FFT3D_STUCT_ALIGNMENT;

    PregeneratedBGLMetaPacket* mGeneratedPackets_rev_Y FFT3D_STUCT_ALIGNMENT;
    PregeneratedBGLMetaPacket* mGeneratedPackets_rev_Z FFT3D_STUCT_ALIGNMENT;
    PregeneratedBGLMetaPacket* mGeneratedPackets_rev_XYZVol FFT3D_STUCT_ALIGNMENT;

    int                        mGP_fwd_X_size FFT3D_STUCT_ALIGNMENT;
    int                        mGP_fwd_Y_size FFT3D_STUCT_ALIGNMENT;
    int                        mGP_fwd_Z_size FFT3D_STUCT_ALIGNMENT;

    int                        mGP_rev_Y_size FFT3D_STUCT_ALIGNMENT;
    int                        mGP_rev_Z_size FFT3D_STUCT_ALIGNMENT;
    int                        mGP_rev_XYZVol_size FFT3D_STUCT_ALIGNMENT;

    int* mSentToTask;
    int  mNumberOfNodes;

    // ActorPacket xap FFT3D_STUCT_ALIGNMENT;
    // ActorPacket yap FFT3D_STUCT_ALIGNMENT;

    FFT3D** mStaticThisPtr FFT3D_STUCT_ALIGNMENT;

    int mTotalFFTsRecv_x;
    int mTotalFFTsRecv_y;
    int mTotalFFTsRecv_z;

    void ChooseFifosForDestinationYZ( int aDest_y, int aDest_z, int aFifoIndex[2] );
    void ChooseFifosForDestinationXY( int aDest_x, int aDest_y, int aFifoIndex[2] );

    static int PacketActorFx_FWD_X( void* pkt );
    static int PacketActorFx_FWD_Y( void* pkt );
    static int PacketActorFx_FWD_Z( void* pkt );

    static int PacketActorFx_FWD_X_FIFO( _BGL_TorusFifo *fifo, Bit32  arg, Bit32  extra, void* indirect  );
    static int PacketActorFx_FWD_Y_FIFO( _BGL_TorusFifo *fifo, Bit32  arg, Bit32  extra, void* indirect  );
    static int PacketActorFx_FWD_Z_FIFO( _BGL_TorusFifo *fifo, Bit32  arg, Bit32  extra, void* indirect  );

    static int PacketActorFx_REV_2Y( void* pkt );
    static int PacketActorFx_REV_2Z( void* pkt );
    static int PacketActorFx_REV_2XYZ( void* pkt );

    static int ComputeFFTsInQueue_FWD_X( void* pkt );
    static int ComputeFFTsInQueue_FWD_Y( void* pkt );
    static int ComputeFFTsInQueue_FWD_Z( void* pkt );    
  
  static int ComputeFFTsInQueue_REV_X( void* pkt );
  static int ComputeFFTsInQueue_REV_Y( void* pkt );
  static int ComputeFFTsInQueue_REV_Z( void* pkt );    
  
  void SetupFifo( BGLFifoInfo* aFifoInfo, int aFifoDirection );
  
  void GeneratePackets_FWD_X();
  void GeneratePackets_FWD_Y();
  void GeneratePackets_FWD_Z();

  void RandomizePackets();

//     void GeneratePackets_REV_Y();
//     void GeneratePackets_REV_Z();
//     void GeneratePackets_REV_XYZVol();

    void SendPackets_FWD_Z();
    void SendPackets_FWD_Y();
    void SendPackets_FWD_X();

    void SendPackets_REV_Y();
    void SendPackets_REV_Z();
    void SendPackets_REV_XYZVol();

    // Checksuming the data for debugging.
    void InitHeaders();
    void InitFifos();

  inline void
  BGLTorusFifoActivePacketSend( _BGL_TorusPktHdr     & aPktHdr,
                                void                 * aData, 
                                int                    aLen,
                                int                    aSendToSelf,
                                int                    aFifoIndex[ 2 ] );

  void ActiveMsgBarrier( FFT_Counter & completePencilsInBarCount,
                         int   barSize,
                         void* ptr );
}; 



template<class FFT_PLAN >
inline void FFT3D< FFT_PLAN >::BGLTorusFifoActivePacketSend( _BGL_TorusPktHdr     & aPktHdr,
                                                             void                 * aData, 
                                                             int                    aLen,
                                                             int                    aSendToSelf,
                                                             int                    aFifoIndex[2] )
{
  if( aSendToSelf )
    {
      VirtualFifo_BGLTorusSendPacketUnaligned( & aPktHdr, // Previously created header to use
                                               aData,     // Source address of data (16byte aligned)
                                               aLen );      
    }
  else
    {
      while( 1 )
        {
          for( int i=0; i<2; i++)
            {
              int sent = MyBGLTorusInjectPacket( mFifos[ aFifoIndex[ i ] ].mFifo, 
                                                 & aPktHdr,
                                                 mFifos[ aFifoIndex[ i ] ].mFifoStatusPtr,
                                                 mFifos[ aFifoIndex[ i ] ].mFifoStatusSetPtr,
                                                 mFifos[ aFifoIndex[ i ] ].mFifoGroup,
                                                 aData,
                                                 aLen );
              if( sent )
                return;
              else
                {
                  NotSentStart.HitOE(  PKTRACE_NOT_SENT,
                                       "NotSent", 
                                       Platform::Thread::GetId(),
                                       NotSentStart );                  
                }
            }
        }
    }
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
ChooseFifosForDestinationYZ( int aDest_y, int aDest_z, int aFifoIndex[2] )
{
  if (( aDest_y - mMyP_y > 0 ) && ( aDest_z - mMyP_z > 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YP;
      aFifoIndex[ 1 ] = TORUS_ZP;
    }
  else if( ( ( aDest_y - mMyP_y ) < 0 ) && ( aDest_z - mMyP_z > 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YM;
      aFifoIndex[ 1 ] = TORUS_ZP;      
    }
  else if( ( ( aDest_z - mMyP_z ) < 0 ) && (( aDest_y - mMyP_y ) > 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YP;
      aFifoIndex[ 1 ] = TORUS_ZM;      
    }  
  else if( ( ( aDest_y - mMyP_y ) < 0 ) && (( aDest_z - mMyP_z ) < 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YM;
      aFifoIndex[ 1 ] = TORUS_ZM;            
    }
  else if(( aDest_y == mMyP_y ) && ( aDest_z == mMyP_z ) )
    {
      // This is arbitrary
      aFifoIndex[ 0 ] = TORUS_YM;
      aFifoIndex[ 1 ] = TORUS_ZM;                  
    }
  else if( aDest_y == mMyP_y )
    {
      if( ( aDest_z - mMyP_z ) < 0 )
        {
          aFifoIndex[ 0 ] = TORUS_ZM;
          aFifoIndex[ 1 ] = TORUS_ZM;                            
        }
      else
        {
          aFifoIndex[ 0 ] = TORUS_ZP;
          aFifoIndex[ 1 ] = TORUS_ZP;                                      
        }
    }
  else if( aDest_z == mMyP_z )
    {
      if( ( aDest_y - mMyP_y ) < 0 )
        {
          aFifoIndex[ 0 ] = TORUS_YM;
          aFifoIndex[ 1 ] = TORUS_YM;                            
        }
      else
        {
          aFifoIndex[ 0 ] = TORUS_YP;
          aFifoIndex[ 1 ] = TORUS_YP;                                      
        }
    }
  else
    {
      BegLogLine( 1 )
        << " ChooseFifosForDestinationYZ():: "
        << " aDest_y="<< aDest_y
        << " aDest_z="<< aDest_z
        << " aFifoIndex could not be set"
        << EndLogLine;
      PLATFORM_ABORT("ERROR:: see above");
    }
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
ChooseFifosForDestinationXY( int aDest_x, int aDest_y, int aFifoIndex[2] )
{
  if (( aDest_y - mMyP_y > 0 ) && ( aDest_x - mMyP_x > 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YP;
      aFifoIndex[ 1 ] = TORUS_XP;
    }
  else if( ( ( aDest_y - mMyP_y ) < 0 ) && ( aDest_x - mMyP_x > 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YM;
      aFifoIndex[ 1 ] = TORUS_XP;      
    }
  else if( ( ( aDest_x - mMyP_x ) < 0 ) && (( aDest_y - mMyP_y ) > 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YP;
      aFifoIndex[ 1 ] = TORUS_XM;      
    }  
  else if( ( ( aDest_y - mMyP_y ) < 0 ) && (( aDest_x - mMyP_x ) < 0 ) )
    {
      aFifoIndex[ 0 ] = TORUS_YM;
      aFifoIndex[ 1 ] = TORUS_XM;            
    }
  else if(( aDest_y == mMyP_y ) && ( aDest_x == mMyP_x ) )
    {
      // This is arbitrary
      aFifoIndex[ 0 ] = TORUS_YM;
      aFifoIndex[ 1 ] = TORUS_XM;                  
    }
  else if( aDest_y == mMyP_y )
    {
      if( ( aDest_x - mMyP_x ) < 0 )
        {
          aFifoIndex[ 0 ] = TORUS_XM;
          aFifoIndex[ 1 ] = TORUS_XM;                            
        }
      else
        {
          aFifoIndex[ 0 ] = TORUS_XP;
          aFifoIndex[ 1 ] = TORUS_XP;                                      
        }
    }
  else if( aDest_x == mMyP_x )
    {
      if( ( aDest_y - mMyP_y ) < 0 )
        {
          aFifoIndex[ 0 ] = TORUS_YM;
          aFifoIndex[ 1 ] = TORUS_YM;                            
        }
      else
        {
          aFifoIndex[ 0 ] = TORUS_YP;
          aFifoIndex[ 1 ] = TORUS_YP;                                      
        }
    }
  else
    {
      BegLogLine( 1 )
        << " ChooseFifosForDestinationYX():: "
        << " aDest_y="<< aDest_y
        << " aDest_x="<< aDest_x
        << " aFifoIndex could not be set"
        << EndLogLine;
      PLATFORM_ABORT("ERROR:: see above");
    }
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
InitFifos()
{
  SetupFifo( &mFifos[ TORUS_XP ], TORUS_XP );
  SetupFifo( &mFifos[ TORUS_XM ], TORUS_XM );
  SetupFifo( &mFifos[ TORUS_YP ], TORUS_YP );
  SetupFifo( &mFifos[ TORUS_YM ], TORUS_YM );
  SetupFifo( &mFifos[ TORUS_ZP ], TORUS_ZP );
  SetupFifo( &mFifos[ TORUS_ZM ], TORUS_ZM );
  return;
}

template<class FFT_PLAN >
int FFT3D< FFT_PLAN >::
ComputeFFTsInQueue_FWD_Z(void * args)
{
  if( Platform::Thread::GetId() == 1 )    
    Core1ComputationStart.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core1Computation",
                                 Platform::Thread::GetId(), 
                                 Core1ComputationStart );
  else
    Core0ComputationStart.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core0Computation",
                                 Platform::Thread::GetId(), 
                                 Core0ComputationStart );

  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;
  
  int start = cqap->mStart;
  int count   = cqap->mCount;

  FFT3D< FFT_PLAN > *This = *( cqap->ThisPtr );

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  BegLogLine( PKFXLOG_COMPUTEFFTSINQUEUE_Z )
    << "ComputeFFTsInQueue_Z "
    << " start=" << start
    << " count=" << count
    << EndLogLine;

  for( int i = 0; i < count; i++ )
    {
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
#else
      int index = i + start;
#endif

#ifndef TEST_FFT_DATA_MOVEMENT
//       ((*This).mFFT_FWD_z).fftInternal( (*This).mFFT_1D_Queue_Z[ index ],
//                                         (*This).mFFT_1D_Queue_Z[ index + 1 ] );      
      
      ((*This).mFFT_FWD_z).fftInternal( &((*This).mZYXVol2ZyxPhaseData_R->Zxy( index, 0 )),
                                        &((*This).mZYXVol2ZyxPhaseData_R->Zxy( index+1, 0 )) );
                                        
#endif
      
      
      TR_FFT_1D_ZFinis.HitOE(  0 && ( Platform::Thread::GetId() == 0 ),//PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_FFT_1D_ZFinis", 
                               Platform::Thread::GetId(),
                               TR_FFT_1D_ZFinis );
    }  

  BegLogLine( PKFXLOG_COMPUTEFFTSINQUEUE_Z )
    << "ComputeFFTsInQueue_Z "
    << " Platform::Thread::GetId()=" << Platform::Thread::GetId()
    << EndLogLine;

  if( Platform::Thread::GetId() == 1 )
    {
      Platform::Memory::ExportFence( (*This).mZYXVol2ZyxPhaseData_R, 
                                sizeof( Value ) * (*This).mLocalNx
                                * (*This).mLocalNy 
                                * (*This).mLocalNz );
      

    (*This).mIOP_FFT_Compute_Done_z.mVar = 1;
    Platform::Memory::ExportFence( & (*This).mIOP_FFT_Compute_Done_z, sizeof( FFT_Counter ));
    Platform::Memory::ImportFence( & (*This).mIOP_FFT_Compute_Done_z, sizeof( FFT_Counter ));

    Core1ComputationFinis.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core1Computation",
                                 Platform::Thread::GetId(), 
                                 Core1ComputationFinis );

    // Platform::Memory::ExportFence();
    }
  else
    Core0ComputationFinis.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core0Computation",
                                 Platform::Thread::GetId(), 
                                 Core0ComputationFinis );    

  return 0;
}

template<class FFT_PLAN >
int FFT3D< FFT_PLAN >::
ComputeFFTsInQueue_FWD_X(void * args)
{
  if( Platform::Thread::GetId() == 1 )    
    Core1ComputationStart.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core1Computation",
                                 Platform::Thread::GetId(), 
                                 Core1ComputationStart );
  else
    Core0ComputationStart.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core0Computation",
                                 Platform::Thread::GetId(), 
                                 Core0ComputationStart );
  
  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;
  
  int start = cqap->mStart;
  int count   = cqap->mCount;
  FFT3D< FFT_PLAN > *This = *(cqap->ThisPtr );  

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  for( int i = 0; i < count; i++ )
    {
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
#else
      int index = i + start;
#endif

#ifndef TEST_FFT_DATA_MOVEMENT
      //       ((*This).mFFT_FWD_x).fftInternal( (*This).mFFT_1D_Queue_X[ index ], 
      //                                         (*This).mFFT_1D_Queue_X[ index + 1 ] );      
      ((*This).mFFT_FWD_z).fftInternal( &((*This).mYxz2XyzPhaseData_R->Xyz( index, 0 )),
                                        &((*This).mYxz2XyzPhaseData_R->Xyz( index+1, 0 )) );
#endif
      
      TR_FFT_1D_XFinis.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,
                              "TR_FFT_1D_XFinis", 
                              Platform::Thread::GetId(),
                              TR_FFT_1D_XFinis );
    }

  if( Platform::Thread::GetId() == 1 )
    {
      // (*This).mIOP_FFT_Compute_Done_x.mVar = 1;
      // Platform::Memory::ExportFence();

      Platform::Memory::ExportFence( (*This).mYxz2XyzPhaseData_R, 
                                     sizeof( Value ) * (*This).mLocalNx
                                     * (*This).mLocalNy 
                                     * (*This).mLocalNz );
      

      (*This).mIOP_FFT_Compute_Done_x.mVar = 1;
      Platform::Memory::ExportFence( & (*This).mIOP_FFT_Compute_Done_x, sizeof( FFT_Counter ));
      Platform::Memory::ImportFence( & (*This).mIOP_FFT_Compute_Done_x, sizeof( FFT_Counter ));

      Core1ComputationFinis.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                   "Core1Computation",
                                   Platform::Thread::GetId(), 
                                   Core1ComputationFinis );
    }
  else
    Core0ComputationFinis.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core0Computation",
                                 Platform::Thread::GetId(), 
                                 Core0ComputationFinis );
    

  return 0;
}

template<class FFT_PLAN >
int FFT3D< FFT_PLAN >::
ComputeFFTsInQueue_FWD_Y(void * args)
{
  if( Platform::Thread::GetId() == 1 )    
    Core1ComputationStart.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core1Computation",
                                 Platform::Thread::GetId(), 
                                 Core1ComputationStart );
  else
    Core0ComputationStart.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core0Computation",
                                 Platform::Thread::GetId(), 
                                 Core0ComputationStart );

  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;
  
  int start = cqap->mStart;
  int count   = cqap->mCount;
  FFT3D< FFT_PLAN > *This = *(cqap->ThisPtr);
  
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  for( int i = 0; i < count ; i++ )
    {
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
#else
      int index = i + start;
#endif

#ifndef TEST_FFT_DATA_MOVEMENT
//       ((*This).mFFT_FWD_y).fftInternal( (*This).mFFT_1D_Queue_Y[ index ],
//                                         (*This).mFFT_1D_Queue_Y[ index + 1] );      
      ((*This).mFFT_FWD_z).fftInternal( & ((*This).mZyx2YxzPhaseData_R->Yxz( index, 0 )),
                                        & ((*This).mZyx2YxzPhaseData_R->Yxz( index+1, 0 )) );
      
#endif
      
      TR_FFT_1D_YFinis.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,
                              "TR_FFT_1D_YFinis", 
                              Platform::Thread::GetId(),
                              TR_FFT_1D_YFinis );
    }

  if( Platform::Thread::GetId() == 1 )
    {
      // (*This).mIOP_FFT_Compute_Done_y.mVar = 1;
      // Platform::Memory::ExportFence();
      
      Platform::Memory::ExportFence( (*This).mZyx2YxzPhaseData_R, 
                                     sizeof( Value ) * (*This).mLocalNx
                                     * (*This).mLocalNy 
                                     * (*This).mLocalNz );

      (*This).mIOP_FFT_Compute_Done_y.mVar = 1;
      Platform::Memory::ExportFence( & (*This).mIOP_FFT_Compute_Done_y, sizeof( FFT_Counter ));
      Platform::Memory::ImportFence( & (*This).mIOP_FFT_Compute_Done_y, sizeof( FFT_Counter ));

      Core1ComputationFinis.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                   "Core1Computation",
                                   Platform::Thread::GetId(), 
                                   Core1ComputationFinis );
    }
  else
    Core0ComputationFinis.HitOE( PKTRACE_CORE_1_DONE_COMPUTE,
                                 "Core0Computation",
                                 Platform::Thread::GetId(), 
                                 Core0ComputationFinis );


return 0;
}


template<class FFT_PLAN >
int FFT3D< FFT_PLAN >::
ComputeFFTsInQueue_REV_Z(void * args)
{
  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;
  
  int start = cqap->mStart;
  int count   = cqap->mCount;

  FFT3D< FFT_PLAN > *This = *( cqap->ThisPtr );

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  BegLogLine( PKFXLOG_COMPUTEFFTSINQUEUE_Z )
    << "ComputeFFTsInQueue_Z "
    << " start=" << start
    << " count=" << count
    << EndLogLine;

  for( int i = 0; i < count; i++ )
    {
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
#else
      int index = i + start;
#endif

#ifndef TEST_FFT_DATA_MOVEMENT
      ((*This).mFFT_REV_z).fftInternal( (*This).mFFT_1D_Queue_Z[ index ],
                                        (*This).mFFT_1D_Queue_Z[ index + 1 ] );      
#endif
      
      TR_FFT_1D_ZFinis.HitOE(  0,//PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_FFT_1D_ZFinis", 
                               Platform::Thread::GetId(),
                               TR_FFT_1D_ZFinis );
    }  

  BegLogLine( PKFXLOG_COMPUTEFFTSINQUEUE_Z )
    << "ComputeFFTsInQueue_Z "
    << " Platform::Thread::GetId()=" << Platform::Thread::GetId()
    << EndLogLine;

  if( Platform::Thread::GetId() == 1 )
    {
    (*This).mIOP_FFT_Compute_Done_z.mVar = 1;
    Platform::Memory::ExportFence();
    }
  
  return 0;
}

template<class FFT_PLAN >
int FFT3D< FFT_PLAN >::
ComputeFFTsInQueue_REV_X(void * args)
{
  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;
  
  int start = cqap->mStart;
  int count   = cqap->mCount;
  FFT3D< FFT_PLAN > *This = *(cqap->ThisPtr );  

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  for( int i = 0; i < count; i++ )
    {
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
#else
      int index = i + start;
#endif

#ifndef TEST_FFT_DATA_MOVEMENT
      ((*This).mFFT_REV_x).fftInternal( (*This).mFFT_1D_Queue_X[ index ], 
                                        (*This).mFFT_1D_Queue_X[ index + 1 ] );      
#endif
      
      TR_FFT_1D_XFinis.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,
                              "TR_FFT_1D_XFinis", 
                              Platform::Thread::GetId(),
                              TR_FFT_1D_XFinis );
    }

  if( Platform::Thread::GetId() == 1 )
    {
    (*This).mIOP_FFT_Compute_Done_x.mVar = 1;
    Platform::Memory::ExportFence();
    }

  return 0;
}

template<class FFT_PLAN >
int FFT3D< FFT_PLAN >::
ComputeFFTsInQueue_REV_Y(void * args)
{
  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;
  
  int start = cqap->mStart;
  int count   = cqap->mCount;
  FFT3D< FFT_PLAN > *This = *(cqap->ThisPtr);
  
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  for( int i = 0; i < count ; i++ )
    {
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
#else
      int index = i + start;
#endif

#ifndef TEST_FFT_DATA_MOVEMENT
      ((*This).mFFT_REV_y).fftInternal( (*This).mFFT_1D_Queue_Y[ index ],
                                        (*This).mFFT_1D_Queue_Y[ index + 1] );      
#endif
      
      TR_FFT_1D_YFinis.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,
                              "TR_FFT_1D_YFinis", 
                              Platform::Thread::GetId(),
                              TR_FFT_1D_YFinis );
    }

  if( Platform::Thread::GetId() == 1 )
    {
    (*This).mIOP_FFT_Compute_Done_y.mVar = 1;
    Platform::Memory::ExportFence();
    }
  
  return 0;
}


// *********************************************************************
// Check the data distribution after third phase:
// Should be such that px((x,y,z)) = floor(((y % DNY) + DNY*(z % DNZ))/DXZ);
//                     py((x,y,z)) = floor(y/DNY);
//                     pz((x,y,z)) = floor(z/DNZ);
// and the local fftIndex of an element on a node with address component px
// is (y % DNY) + DNY*(z % DNZ) - px*DYZ
// *********************************************************************
#ifdef TEST_FFT_DATA_MOVEMENT
template< class FFT_PLAN >
void FFT3D< FFT_PLAN >::CheckDataMovementAfterFWD()
{
  int errY2x = 0;
  
#if 1
  for ( int dyz = mMyP_x * mDataContainer1.DYZ();
        dyz < ( mMyP_x + 1 ) * mDataContainer1.DYZ();
        ++dyz )
    {
      int y = dyz % mDataContainer1.DNY(); // dy now
      int z = (dyz - y) / mDataContainer1.DNY(); // dz now
      y = y + mMyP_y * mDataContainer1.DNY();
      z = z + mMyP_z * mDataContainer1.DNZ();

      for ( int x = 0; x < mFFTMesh->d_NX; ++x )
        {
          Value foo;
          foo.d_coord.first = x;
          foo.d_coord.second = y;
          foo.d_coord.third = z;

          if (mDataContainer2.Xyz( dyz % mDataContainer1.DYZ(), x) != foo )
            {
              BegLogLine( 1 )
                << "inVol[" << mMyP_x << "][" << mMyP_y << "][" << mMyP_z << "].Xyz("
                << dyz % mDataContainer1.DYZ() << ", " << x << ") = {"
                << mDataContainer2.Xyz( dyz % mDataContainer1.DYZ(), x ).d_coord.first << ","
                << mDataContainer2.Xyz( dyz % mDataContainer1.DYZ(), x ).d_coord.second << ","
                << mDataContainer2.Xyz( dyz % mDataContainer1.DYZ(), x ).d_coord.third
                << "} != { " 
                << foo.d_coord.first << ","
                << foo.d_coord.second << ","
                << foo.d_coord.third << "}"
                << EndLogLine;
              
              ++errY2x;
            }
        }      
    }    
  
  BegLogLine( 1 )
    << "CheckDataMovementAfterFWD():: "
    << (( errY2x == 0 ) ? "SUCCESS :-)" : "FAIL :-(" )
    << EndLogLine;
#endif
} 
#endif

#ifdef TEST_FFT_DATA_MOVEMENT
template< class FFT_PLAN >
void FFT3D< FFT_PLAN > :: InitCoords()
{
#if 1
  for( int x=0; x < mDataContainer1.DNX(); x++ )
    {
      for( int y=0; y < mDataContainer1.DNY(); y++ )
        {
          for( int z=0; z < mDataContainer1.DNZ(); z++ )
            {
              Value foo;
              foo.d_coord.first  = x + mMyP_x * mDataContainer1.DNX();
              foo.d_coord.second = y + mMyP_y * mDataContainer1.DNY();
              foo.d_coord.third  = z + mMyP_z * mDataContainer1.DNZ();
              mDataContainer1.xyz( x, y ,z ) = foo;               
            }
        }
    }
#endif
}
#endif

template< class FFT_PLAN >
int FFT3D< FFT_PLAN > :: Init( FFT3D** aStaticThisPtr, 
                               int proc_x, int proc_y, int proc_z )
                               // int mesh_x, int mesh_y, int mesh_z )
  {

  BegLogLine( PKFXLOG_INIT ) 
    << "Init() "
    << " sizeof( FFT_Hdr ) = " << sizeof( FFT_Hdr )
    << " sizeof( ActorPacket ) = " << sizeof( ActorPacket )
    << " sizeof( EndOfPacket ) = " << sizeof( EndOfPacket )
    << EndLogLine;
        

  mStaticThisPtr = aStaticThisPtr;
  mEndOfPacket.mValue = END_OF_PACKET;

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  BGLPartitionGetCoords( &mMyP_x, &mMyP_y, &mMyP_z );
  
  BGLPartitionGetDimensions( &mNprocs_x, &mNprocs_y, &mNprocs_z );

  PX = proc_x;
  PY = proc_y;
  PZ = proc_z;
   
  mIsUni = ((PX == 1) && (PY==1) && (PZ==1));

  mFFTMesh = new fft::FFTMesh( N_X, N_Y, N_Z );

  mProcMesh = new fft::ProcMesh( PX, PY, PZ );
  
  int dimX, dimY, dimZ;
  BGLPartitionGetDimensions( &dimX, &dimY, &dimZ );
  mNumberOfNodes = dimX * dimY * dimZ;

  mSentToTask = (int *) pkMalloc( sizeof(int) * mNumberOfNodes );

  mDataContainer1.init( *mFFTMesh, *mProcMesh );
  mDataContainer2.init( *mFFTMesh, *mProcMesh );
  
  mLocalNx = mDataContainer1.DNX();
  mLocalNy = mDataContainer1.DNY();
  mLocalNz = mDataContainer1.DNZ();

#ifdef TEST_FFT_DATA_MOVEMENT
  InitCoords();
#endif

  // We need 2 containers. Send from 1st, 2nd recvs, swap for next phase
  // Initial data goes into 1st.
  // Send_Z_fwd: Send from 1st, recv 2nd, do ffts in 2nd
  // Send_Y_fwd: Send from 2nd, recv 1st, do ffts in 1st
  // Send_X_fwd: Send from 1nd, recv 2st, do ffts in 2st
  
  // Send_x_rev: Send from 2st, recv 1nd, do ffts in 1nd
  // Send_Y_rev: Send from 1nd, recv 2st, do ffts in 2st
  // Send_Z_rev: Send from 2nd, recv 1st, do ffts in 1st

  mZYXVol2ZyxPhaseData_S = &mDataContainer1;
  mZYXVol2ZyxPhaseData_R = &mDataContainer2;
  
  mZyx2YxzPhaseData_S = &mDataContainer2;
  mZyx2YxzPhaseData_R = &mDataContainer1;
  
  mYxz2XyzPhaseData_S = &mDataContainer1;
  mYxz2XyzPhaseData_R = &mDataContainer2;
  
  mXyz2YzxPhaseData_S = &mDataContainer2;
  mXyz2YzxPhaseData_R = &mDataContainer1;
  
  mYzx2ZyxPhaseData_S = &mDataContainer1;
  mYzx2ZyxPhaseData_R = &mDataContainer2;
  
  mZyx2XYZVolPhaseData_S = &mDataContainer2;
  mZyx2XYZVolPhaseData_R = &mDataContainer1;
    
  mSendInZRouterFwd.init( *mZYXVol2ZyxPhaseData_S, mMyP_x, mMyP_y, mMyP_z );
  mSendInYRouterFwd.init( *mZyx2YxzPhaseData_S,    mMyP_x, mMyP_y, mMyP_z, 0 );  
  mSendInXRouterFwd.init( *mYxz2XyzPhaseData_S,    mMyP_x, mMyP_y, mMyP_z, 0 );

  mSendInYRouterRev.init( *mXyz2YzxPhaseData_S,    mMyP_x, mMyP_y, mMyP_z, 0 );
  // mSendInYRouterRev.ValidateCheckSum( __LINE__ );

  mSendInZRouterRev.init( *mYzx2ZyxPhaseData_S,    mMyP_x, mMyP_y, mMyP_z, 0 );
  mSendInXYZVolRouterRev.init( *mZyx2XYZVolPhaseData_S, mMyP_x, mMyP_y, mMyP_z, 0 );
   
  mTotalRecvData_x = (int *) malloc( sizeof(int) * mDataContainer1.DYZ() );
  mTotalRecvData_y = (int *) malloc( sizeof(int) * mDataContainer1.DXZ() );
  mTotalRecvData_z = (int *) malloc( sizeof(int) * mDataContainer1.DXY() );
    
  mFFT_1D_Queue_X = (Value **) malloc( sizeof(Value *) * mDataContainer1.DYZ() );
  mFFT_1D_Queue_Y = (Value **) malloc( sizeof(Value *) * mDataContainer1.DXZ() );
  mFFT_1D_Queue_Z = (Value **) malloc( sizeof(Value *) * mDataContainer1.DXY() );

  ActiveMsgBarrierStart.init();
  ActiveMsgBarrierFinis.init();

  ImportFenceStart.init();
  ImportFenceFinis.init();

  fftForwardTracerStart.init();
  fftForwardTracerFinis.init();
  
  fftReverseTracerStart.init();
  fftReverseTracerFinis.init();
  
  TR_SendPackets_ZStart.init();
  TR_SendPackets_ZFinis.init();
  
  TR_SendPackets_YStart.init();
  TR_SendPackets_YFinis.init();
  
  TR_SendPackets_XStart.init();
  TR_SendPackets_XFinis.init();

  Core0ComputationStart.init();
  Core0ComputationFinis.init();
  
  Core1ComputationStart.init();
  Core1ComputationFinis.init();

  Core1FFTRecvDone.init();

  
  TR_FFT_1D_ZStart.init();
  TR_FFT_1D_ZFinis.init();
  
  TR_FFT_1D_YStart.init();
  TR_FFT_1D_YFinis.init();
  
  TR_FFT_1D_XStart.init();
  TR_FFT_1D_XFinis.init();
  
  TR_1DFFT_READY_X.init();
  TR_1DFFT_READY_Y.init();
  TR_1DFFT_READY_Z.init();
  
  TR_BGLActivePacketSendStart.init();
  TR_BGLActivePacketSendFinis.init();
  
  TR_Sender_Compute_Z_Start.init();
  TR_Sender_Compute_Z_Finis.init();
  
  TR_Sender_Compute_Y_Start.init();
  TR_Sender_Compute_Y_Finis.init();
  
  TR_Sender_Compute_X_Start.init();
  TR_Sender_Compute_X_Finis.init();
  
  Full_Send_Queue_State_Start.init();
  Full_Send_Queue_State_Finis.init();
  
  BGLHeaderCreation.init();

  SendInX_fwdStart.init();
  SendInX_fwdFinis.init();
  SendInY_fwdStart.init();
  SendInY_fwdFinis.init();
  SendInZ_fwdStart.init();
  SendInZ_fwdFinis.init();
  
  TracingCost.init();

 FillPacketStart.init();
 FillPacketFinis.init();
 
 NotSentStart.init();
 NotSentFinis.init();

 Reset();       

 InitFifos();

 mGP_fwd_X_size = 0;
 mGP_fwd_Y_size = 0;
 mGP_fwd_Z_size = 0;
 
 mGP_rev_Y_size = 0;
 mGP_rev_Z_size = 0;
 mGP_rev_XYZVol_size = 0;
 
 mGeneratedPackets_fwd_X = NULL;
 mGeneratedPackets_fwd_Y = NULL;
 mGeneratedPackets_fwd_Z = NULL;
 
 mGeneratedPackets_rev_Y = NULL;
 mGeneratedPackets_rev_Z = NULL;
 mGeneratedPackets_rev_XYZVol = NULL;

 GeneratePackets_FWD_X();
 GeneratePackets_FWD_Y();
 GeneratePackets_FWD_Z();

 mRand.Init();
 mSeed = 1234312;

 RandomizePackets();

//  GeneratePackets_REV_Y();
//  GeneratePackets_REV_Z();
//  GeneratePackets_REV_XYZVol();

  // InitHeaders();

  return 1;
  }

template< class FFT_PLAN >
int FFT3D< FFT_PLAN >::
PacketActorFx_FWD_Z(void* pkt)
{
  ActorPacket *ap = (ActorPacket *) pkt;
  
  char *Data = ap->mData;
  int CurrentOffsetInData = 0;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
    << "PacketActor FWD Z:: "
    << "DataPtr = " << (void *) Data
    << EndLogLine;

  EndOfPacket *EOM = (EndOfPacket *) Data;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
    << "PacketActor FWD Z:: "
    << "EOM Ptr = " << (void *) EOM
    << EndLogLine;

  while( EOM->mValue != END_OF_PACKET )
    {
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: Top of the loop"        
        << EndLogLine;      

      FFT_Hdr* hdr = (FFT_Hdr *) & Data[ CurrentOffsetInData ];

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: "
        << "FFT_Hdr* hdr=" << (void *) hdr
        << EndLogLine;      

      FFT3D<FFT_PLAN> *This = *(hdr->ThisPtr);
      int fftIndex = hdr->fftIndex;
      int offset = hdr->offset;
      int count = hdr->count;

      // if( ( (*This).mMyP_x == 0 ) && ( (*This).mMyP_y == 0 ) && ( (*This).mMyP_z == 0 ) )        
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        //  BegLogLine( 1 )
        << "PacketActor FWD Z:: "
        << " This = " << (void *) This
        << " fftIndex = " << fftIndex
        << " offset = " << offset
        << " count = " << count
        << " &((*This).mZYXVol2ZyxPhaseData_R->Zxy( " << fftIndex << ",  " << offset << " )) = " 
        << (void * ) &((*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex,  offset ))
        << EndLogLine;      

      CurrentOffsetInData += sizeof( FFT_Hdr );

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: "
        << " CurrentOffsetInData = " << CurrentOffsetInData
        << " &((*ap).mData[ " << CurrentOffsetInData << " ]) =  " << (void *) &((*ap).mData[ CurrentOffsetInData ]) 
        << EndLogLine;      

      double_copy( (double *) &((*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex,  offset )), 
                   (double *) &((*ap).mData[ CurrentOffsetInData ]), 
                   sizeof(double) * count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: "
        << " After double_copy of data"
        << " CurrentOffsetInData = " << CurrentOffsetInData
        << EndLogLine;      
      
      
//       if( fftIndex >= (*This).mTotalFFTsRecv_z )
//         {
//           BegLogLine( 1 )
//             << "PacketActor FWD Z:: "
//             << " ERROR:: fftIndex(" << fftIndex <<") >= mTotalFFTsRecv_z("<< (*This).mTotalFFTsRecv_z << ") "
//             << " (*This).mTotalRecvData_z=" << (void *) (*This).mTotalRecvData_z
//             << EndLogLine;                    
//         }
        
      (*This).mTotalRecvData_z[ fftIndex ] -= count;  

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
      // BegLogLine( 1 )
        << "PacketActor FWD Z:: "
        << " mTotalRecvData_z[ " << fftIndex << "] = " << (*This).mTotalRecvData_z[ fftIndex ]
        << " count=" << count
        << EndLogLine;      

      if(  (*This).mTotalRecvData_z[ fftIndex ] == 0 ) 
        {      
          // BegLogLine( 1 )
          BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
            << "PacketActor FWD Z:: "
            << " Got all the data for fftIndex=" << fftIndex
            << EndLogLine;      
          
          // Platform::Memory::ExportFence( (void *) &( (*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex, 0 ) ), 
          //                                 sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_Z );
          

          BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
            << "PacketActor FWD Z:: "
            << " About to assign the Queue pointer at index=" << (*This).mCompleteFFTs_z.mVar
            << EndLogLine;      

//           (*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTs_z.mVar ] = & ( (*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex, 0 ) );      
//           Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTs_z.mVar ], sizeof(Value *) );                         

          TR_1DFFT_READY_Z.HitOE( PKTRACE_PACKET_ACTOR_FX_Z,
                                  "TR_1DFFT_READY_Z", 
                                  Platform::Thread::GetId(),
                                  TR_1DFFT_READY_Z );      
          
          //  Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTs_z.mVar), sizeof( FFT_Counter ) );          
          if( (*This).mCompleteFFTs_z.mVar+1 == (*This).mTotalFFTsRecv_z )
            {
              // Platform::Memory::ExportFence();
              Platform::Memory::ExportFence( (*This).mZYXVol2ZyxPhaseData_R, sizeof( Value ) 
                                             * (*This).mLocalNx 
                                             * (*This).mLocalNy 
                                             * (*This).mLocalNz );
              
              Platform::Memory::ImportFence( (void *) (*This).mTotalRecvData_z, sizeof( int ) * (*This).mDataContainer1.DXY() );          

              (*This).mCompleteFFTs_z.mVar += 1;
              Platform::Memory::ExportFence( (void *) & (*This).mCompleteFFTs_z, sizeof( FFT_Counter ) );          
              Platform::Memory::ImportFence( (void *) & (*This).mCompleteFFTs_z, sizeof( FFT_Counter ) );          

              Core1FFTRecvDone.HitOE( PKTRACE_CORE_1_DONE_FFT_RECV,
                                      "Core1FFTRecvDone",
                                      Platform::Thread::GetId(), 
                                      Core1FFTRecvDone );

              break;
            }                    

          (*This).mCompleteFFTs_z.mVar += 1;
          
          BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
            << "PacketActor FWD Z:: "
            << " About to export fence the complete counter pointer at index=" << (*This).mCompleteFFTs_z.mVar
            << EndLogLine;                
        }  
     
      EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
    }

  return 0;
  }

template< class FFT_PLAN >
int FFT3D< FFT_PLAN >::
PacketActorFx_FWD_Z_FIFO( _BGL_TorusFifo *fifo, Bit32  arg, Bit32  extra, void* indirect  )
  {
    //     ActorPacket *ap = (ActorPacket *) indirect;

    // char *Data = ap->mData;
  char * Data = (char *) indirect;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
  // BegLogLine( 1 )
    << "PacketActor FWD Z:: "
    << "DataPtr = " << (void *) Data
    << EndLogLine;

  EndOfPacket *EOM = (EndOfPacket *) Data;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
    << "PacketActor FWD Z:: "
    << "EOM Ptr = " << (void *) EOM
    << EndLogLine;

  while( EOM->mValue != END_OF_PACKET )
    {
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: Top of the loop"        
        << EndLogLine;      

      FFT_Hdr* hdr = (FFT_Hdr *) & Data[ 0 ];

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: "
        << "FFT_Hdr* hdr=" << (void *) hdr
        << EndLogLine;      

      FFT3D<FFT_PLAN> *This = *(hdr->ThisPtr);
      int fftIndex = hdr->fftIndex;
      int offset = hdr->offset;
      int count = hdr->count;

      // if( ( (*This).mMyP_x == 0 ) && ( (*This).mMyP_y == 0 ) && ( (*This).mMyP_z == 0 ) )        
      // BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
      BegLogLine( 1 )
        << "PacketActor FWD Z:: "
        << " This = " << (void *) This
        << " fftIndex = " << fftIndex
        << " offset = " << offset
        << " count = " << count
        << " &((*This).mZYXVol2ZyxPhaseData_R->Zxy( " << fftIndex << ",  " << offset << " )) = " 
        << (void * ) &((*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex,  offset ))
        << EndLogLine;      

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: "
        << EndLogLine;      

//       double_copy( (double *) &((*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex,  offset )), 
//                    (double *) &((*ap).mData[ CurrentOffsetInData ]), 
//                    sizeof(double) * count * 2 );

      fastmover_quads( fifo, 
                       (void *) &((*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex,  offset )), 
                       count );
      
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActor FWD Z:: "
        << " After double_copy of data"
        << EndLogLine;      
      
      
//       if( fftIndex >= (*This).mTotalFFTsRecv_z )
//         {
//           BegLogLine( 1 )
//             << "PacketActor FWD Z:: "
//             << " ERROR:: fftIndex(" << fftIndex <<") >= mTotalFFTsRecv_z("<< (*This).mTotalFFTsRecv_z << ") "
//             << " (*This).mTotalRecvData_z=" << (void *) (*This).mTotalRecvData_z
//             << EndLogLine;                    
//         }
        
      (*This).mTotalRecvData_z[ fftIndex ] -= count;  

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        // BegLogLine( 1 )
        << "PacketActor FWD Z:: "
        << " mTotalRecvData_z[ " << fftIndex << "] = " << (*This).mTotalRecvData_z[ fftIndex ]
        << EndLogLine;      

      if(  (*This).mTotalRecvData_z[ fftIndex ] == 0 ) 
        {      
          // BegLogLine( 1 )
          BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
            << "PacketActor FWD Z:: "
            << " Got all the data for fftIndex=" << fftIndex
            << EndLogLine;      
          
          // Platform::Memory::ExportFence( (void *) &( (*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex, 0 ) ), 
          //                                 sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_Z );

          BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
            << "PacketActor FWD Z:: "
            << " About to assign the Queue pointer at index=" << (*This).mCompleteFFTs_z.mVar
            << EndLogLine;      

          (*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTs_z.mVar ] = & ( (*This).mZYXVol2ZyxPhaseData_R->Zxy( fftIndex, 0 ) );      
          // Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTs_z.mVar ], sizeof(Value *) );                         

          (*This).mCompleteFFTs_z.mVar += 1;
          
          BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
            << "PacketActor FWD Z:: "
            << " About to export fence the complete counter pointer at index=" << (*This).mCompleteFFTs_z.mVar
            << EndLogLine;                

          //  Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTs_z.mVar), sizeof( FFT_Counter ) );          
          if( (*This).mCompleteFFTs_z.mVar == (*This).mTotalFFTsRecv_z )
            {
              Platform::Memory::ExportFence();

              // Clean out the fifo
              fastmover_quads( fifo, 
                               Data, 
                               1 );
              break;
            }          
          
          TR_1DFFT_READY_Z.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,
                                  "TR_1DFFT_READY_Z", 
                                  Platform::Thread::GetId(),
                                  TR_1DFFT_READY_Z );      
        }  
     
      Data = (*This).mRecvBuff;
      fastmover_quads( fifo, 
                       Data, 
                       1 );
      
      EOM = (EndOfPacket *) &Data[ 0 ];
    }

  return 0;
  }

template< class FFT_PLAN >
int FFT3D< FFT_PLAN >::
PacketActorFx_FWD_Y(void* pkt)
  {
  ActorPacket *ap = (ActorPacket *) pkt;

  char *Data = ap->mData;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y )
  // BegLogLine( 1 )
    << "PacketActor FWD Y:: "
    << "DataPtr = " << (void *) Data
    << EndLogLine;

  int CurrentOffsetInData = 0;

  EndOfPacket *EOM = (EndOfPacket *) Data;

  while( EOM->mValue != END_OF_PACKET )
    {
      FFT_Hdr* hdr = (FFT_Hdr *) &Data[ CurrentOffsetInData ];

      FFT3D<FFT_PLAN> *This = *(hdr->ThisPtr);
      int fftIndex = hdr->fftIndex;
      int offset = hdr->offset;
      int count = hdr->count;

      CurrentOffsetInData += sizeof( FFT_Hdr );

      double_copy( (double *) &((*This).mZyx2YxzPhaseData_R->Yxz( fftIndex,  offset )), 
                   (double *) &((*ap).mData[ CurrentOffsetInData ]), 
                   sizeof(double) * count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_y[ fftIndex ] -= count;  

      if(  (*This).mTotalRecvData_y[ fftIndex ] == 0 ) 
        {      
          // Platform::Memory::ExportFence( (void *) &( (*This).mZyx2YxzPhaseData_R->Yxz( fftIndex, 0 ) ), 
          //                               sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_Y );
          
//           (*This).mFFT_1D_Queue_Y[ (*This).mCompleteFFTs_y.mVar ] = & ( (*This).mZyx2YxzPhaseData_R->Yxz( fftIndex, 0 ) );      
//           Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_Y[ (*This).mCompleteFFTs_y.mVar ], sizeof(Value *) );      
          
          TR_1DFFT_READY_Y.HitOE( PKTRACE_PACKET_ACTOR_FX_Y,
                                  "1DFFT_Actor",
                                  Platform::Thread::GetId(),
                                  TR_1DFFT_READY_Y );
          
          
          if( (*This).mCompleteFFTs_y.mVar+1 == (*This).mTotalFFTsRecv_y )
            {
              // Platform::Memory::ExportFence();
              Platform::Memory::ExportFence( (*This).mZyx2YxzPhaseData_R, sizeof( Value ) 
                                             * (*This).mLocalNx 
                                             * (*This).mLocalNy 
                                             * (*This).mLocalNz );
              
              Platform::Memory::ImportFence( (void *) (*This).mTotalRecvData_y, sizeof( int ) * (*This).mDataContainer1.DXZ() );          
              
              (*This).mCompleteFFTs_y.mVar += 1;
              Platform::Memory::ExportFence( (void *) &(*This).mCompleteFFTs_y, sizeof( FFT_Counter ) );          
              Platform::Memory::ImportFence( (void *) & (*This).mCompleteFFTs_y, sizeof( FFT_Counter ) );          

              Core1FFTRecvDone.HitOE( PKTRACE_CORE_1_DONE_FFT_RECV,
                                      "Core1FFTRecvDone",
                                      Platform::Thread::GetId(), 
                                      Core1FFTRecvDone );
              break;
            }                    

          (*This).mCompleteFFTs_y.mVar += 1;
        }  
     
      EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
    }

  return 0;
  }


template< class FFT_PLAN >
int FFT3D< FFT_PLAN >::
PacketActorFx_FWD_X(void* pkt)
  {
  ActorPacket *ap = (ActorPacket *) pkt;

  char *Data = ap->mData;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X )
  // BegLogLine( 1 )
    << "PacketActor FWD X:: "
    << "DataPtr = " << (void *) Data
    << EndLogLine;

  int CurrentOffsetInData = 0;

  EndOfPacket *EOM = (EndOfPacket *) Data;

  while( EOM->mValue != END_OF_PACKET )
    {
      FFT_Hdr* hdr = (FFT_Hdr *) &Data[ CurrentOffsetInData ];

      FFT3D<FFT_PLAN> *This = *(hdr->ThisPtr);
      int fftIndex = hdr->fftIndex;
      int offset = hdr->offset;
      int count = hdr->count;

      CurrentOffsetInData += sizeof( FFT_Hdr );

      double_copy( (double *) &((*This).mYxz2XyzPhaseData_R->Xyz( fftIndex,  offset )), 
                   (double *) &((*ap).mData[ CurrentOffsetInData ]), 
                   sizeof(double) * count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_x[ fftIndex ] -= count;  

      if(  (*This).mTotalRecvData_x[ fftIndex ] == 0 ) 
        {      
          //Platform::Memory::ExportFence( (void *) &( (*This).mYxz2XyzPhaseData_R->Xyz( fftIndex, 0 ) ), 
          //                               sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_X );
          
          (*This).mFFT_1D_Queue_X[ (*This).mCompleteFFTs_x.mVar ] = & ( (*This).mYxz2XyzPhaseData_R->Xyz( fftIndex, 0 ) );      
          Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_X[ (*This).mCompleteFFTs_x.mVar ], sizeof(Value *) );      
          
          
          // Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTs_x.mVar), sizeof( FFT_Counter ) );          
          TR_1DFFT_READY_X.HitOE( PKTRACE_PACKET_ACTOR_FX_X,
                                  "TR_1DFFT_READY_X", 
                                  Platform::Thread::GetId(),
                                  TR_1DFFT_READY_X );      

          if( (*This).mCompleteFFTs_x.mVar+1 == (*This).mTotalFFTsRecv_x )
            {
              // Platform::Memory::ExportFence();
              Platform::Memory::ExportFence( (*This).mYxz2XyzPhaseData_R, sizeof( Value ) 
                                             * (*This).mLocalNx 
                                             * (*This).mLocalNy 
                                             * (*This).mLocalNz );
              
              Platform::Memory::ImportFence( (void *) (*This).mTotalRecvData_x, sizeof( int ) * (*This).mDataContainer1.DYZ() );          
              
              (*This).mCompleteFFTs_x.mVar += 1;
              Platform::Memory::ExportFence( (void *) &(*This).mCompleteFFTs_x, sizeof( FFT_Counter ) );          
              Platform::Memory::ImportFence( (void *) & (*This).mCompleteFFTs_x, sizeof( FFT_Counter ) );                        

              Core1FFTRecvDone.HitOE( PKTRACE_CORE_1_DONE_FFT_RECV,
                                      "Core1FFTRecvDone",
                                      Platform::Thread::GetId(), 
                                      Core1FFTRecvDone );                           
              break;
            }                    
          
          (*This).mCompleteFFTs_x.mVar += 1;
        }  
     
      EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
    }

  return 0;
  }

template< class FFT_PLAN >
int FFT3D< FFT_PLAN >::
PacketActorFx_REV_2Y(void* pkt)
  {
  ActorPacket *ap = (ActorPacket *) pkt;

  char *Data = ap->mData;
  int CurrentOffsetInData = 0;

  EndOfPacket *EOM = (EndOfPacket *) Data;

  while( EOM->mValue != END_OF_PACKET )
    {
      FFT_Hdr* hdr = (FFT_Hdr *) &Data[ CurrentOffsetInData ];

      FFT3D<FFT_PLAN> *This = *(hdr->ThisPtr);
      int fftIndex = hdr->fftIndex;
      int offset = hdr->offset;
      int count = hdr->count;

      CurrentOffsetInData += sizeof( FFT_Hdr );

      double_copy( (double *) & ((*This).mXyz2YzxPhaseData_R->Yzx( fftIndex,  offset )), 
                   (double *) & ((*ap).mData[ CurrentOffsetInData ]), 
                   sizeof(double) * count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_y[ fftIndex ] -= count;  

      if(  (*This).mTotalRecvData_y[ fftIndex ] == 0 ) 
        {      
          // Platform::Memory::ExportFence( (void *) &( (*This).mXyz2YzxPhaseData_R->Yzx( fftIndex, 0 ) ), 
          //                                sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_Y );
          
          (*This).mFFT_1D_Queue_Y[ (*This).mCompleteFFTs_y.mVar ] = & ( (*This).mXyz2YzxPhaseData_R->Yzx( fftIndex, 0 ) );      
          // Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_Y[ (*This).mCompleteFFTs_y.mVar ], sizeof(Value *) );      
          
          (*This).mCompleteFFTs_y.mVar += 1;
          
          // Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTs_y.mVar), sizeof( FFT_Counter ) );          
          if( (*This).mCompleteFFTs_y.mVar == (*This).mTotalFFTsRecv_y )
            {
              Platform::Memory::ExportFence();
              break;
            }          
        }  
     
      EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
    }

  return 0;
  }

template< class FFT_PLAN >
int FFT3D< FFT_PLAN >::
PacketActorFx_REV_2Z(void* pkt)
  {
  ActorPacket *ap = (ActorPacket *) pkt;

  char *Data = ap->mData;
  int CurrentOffsetInData = 0;

  EndOfPacket *EOM = (EndOfPacket *) Data;

  while( EOM->mValue != END_OF_PACKET )
    {
      FFT_Hdr* hdr = (FFT_Hdr *) & Data[ CurrentOffsetInData ];

      FFT3D<FFT_PLAN> *This = *(hdr->ThisPtr);
      int fftIndex = hdr->fftIndex;
      int offset = hdr->offset;
      int count = hdr->count;

      CurrentOffsetInData += sizeof( FFT_Hdr );
      
      double_copy( (double *) &((*This).mYzx2ZyxPhaseData_R->Zxy( fftIndex,  offset )), 
                   (double *) &((*ap).mData[ CurrentOffsetInData ]), 
                   sizeof(double) * count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_z[ fftIndex ] -= count;  

      if(  (*This).mTotalRecvData_z[ fftIndex ] == 0 ) 
        {      
          // Platform::Memory::ExportFence( (void *) &( (*This).mYzx2ZyxPhaseData_R->Zxy( fftIndex, 0 ) ), 
          //                               sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_Z );
          
          (*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTs_z.mVar ] = & ( (*This).mYzx2ZyxPhaseData_R->Zxy( fftIndex, 0 ) );      
          // Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTs_z.mVar ], sizeof(Value *) );      
          
          (*This).mCompleteFFTs_z.mVar += 1;
          
          // Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTs_z.mVar), sizeof( FFT_Counter ) );                    
          if( (*This).mCompleteFFTs_z.mVar == (*This).mTotalFFTsRecv_z )
            {
              Platform::Memory::ExportFence();
              break;
            }          
        }  
     
      EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
    }

  return 0;
  }

template< class FFT_PLAN >
int FFT3D< FFT_PLAN >::
PacketActorFx_REV_2XYZ(void* pkt)
  {
  ActorPacket *ap = (ActorPacket *) pkt;

  char *Data = ap->mData;
  int CurrentOffsetInData = 0;

  EndOfPacket *EOM = (EndOfPacket *) Data;

  while( EOM->mValue != END_OF_PACKET )
    {
      FFT_Hdr* hdr = (FFT_Hdr *) &Data[ CurrentOffsetInData ];

      FFT3D<FFT_PLAN> *This = *(hdr->ThisPtr);
      int dx = hdr->fftIndex;
      int dy = hdr->offset;
      int count = hdr->count;

      CurrentOffsetInData += sizeof( FFT_Hdr );

      double_copy( (double *) &((*This).mZyx2XYZVolPhaseData_R->xyz( dx, dy, 0 )), 
                   (double *) &((*ap).mData[ CurrentOffsetInData ]), 
                   sizeof(double) * count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_xyzvol -= count;  

      if(  (*This).mTotalRecvData_xyzvol == 0 ) 
        {              
          BegLogLine( 0 )
            << "PacketActorFx_REV_2XYZ got all of data"
            << EndLogLine;

          (*This).mCompleteXYZVol.mVar = 1;
          Platform::Memory::ExportFence();
          break;
          // Platform::Memory::ExportFence( (void *) &((*This).mCompleteXYZVol.mVar), sizeof( FFT_Counter ) );                    
        }  
      
      EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
    }

  return 0;
  }


template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::RandomizePackets()
{
  for( int j=0; j < 100; j++ )
    {
    for( int i=0; i < mGP_fwd_Z_size; i++ )
      {
        // Get 2 random indices from 0 to mGP_fwd_Z_size and swap the 2 elements
        int ind1 = mGP_fwd_Z_size * mRand.randomize( mSeed );
        int ind2 = mGP_fwd_Z_size * mRand.randomize( mSeed );

        memcpy( &mTempSwap, &mGeneratedPackets_fwd_Z[ ind1 ], sizeof( PregeneratedBGLMetaPacket ) );
        memcpy( &mGeneratedPackets_fwd_Z[ ind1 ], &mGeneratedPackets_fwd_Z[ ind2 ], sizeof( PregeneratedBGLMetaPacket ) );
        memcpy( &mGeneratedPackets_fwd_Z[ ind2 ], &mTempSwap, sizeof( PregeneratedBGLMetaPacket ) );
      }

    for( int i=0; i < mGP_fwd_Y_size; i++ )
      {
        // Get 2 random indices from 0 to mGP_fwd_Z_size and swap the 2 elements
        int ind1 = mGP_fwd_Y_size * mRand.randomize( mSeed );
        int ind2 = mGP_fwd_Y_size * mRand.randomize( mSeed );

        memcpy( &mTempSwap, &mGeneratedPackets_fwd_Y[ ind1 ], sizeof( PregeneratedBGLMetaPacket ) );
        memcpy( &mGeneratedPackets_fwd_Y[ ind1 ], &mGeneratedPackets_fwd_Y[ ind2 ], sizeof( PregeneratedBGLMetaPacket ) );
        memcpy( &mGeneratedPackets_fwd_Y[ ind2 ], &mTempSwap, sizeof( PregeneratedBGLMetaPacket ) );
      }

    for( int i=0; i < mGP_fwd_X_size; i++ )
      {
        // Get 2 random indices from 0 to mGP_fwd_Z_size and swap the 2 elements
        int ind1 = mGP_fwd_X_size * mRand.randomize( mSeed );
        int ind2 = mGP_fwd_X_size * mRand.randomize( mSeed );

        memcpy( &mTempSwap, &mGeneratedPackets_fwd_X[ ind1 ], sizeof( PregeneratedBGLMetaPacket ) );
        memcpy( &mGeneratedPackets_fwd_X[ ind1 ], &mGeneratedPackets_fwd_X[ ind2 ], sizeof( PregeneratedBGLMetaPacket ) );
        memcpy( &mGeneratedPackets_fwd_X[ ind2 ], &mTempSwap, sizeof( PregeneratedBGLMetaPacket ) );
      }
    }
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
GeneratePackets_FWD_Z()
{  
  
  // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes
  
  int count=0;
  
  // First iteration gets out the size of the generated packets array
  for( int twice=0; twice < 2; twice++ )
    {      
      int limit1 = ( PZ == 1 ) ? 1 : ( PZ / 2 );

      for( int i=1; i <= limit1; i++ )
        {      
          int dest1 = ( i + mMyP_z ) % PZ;
          int dest2 = (( (1-i) + mMyP_z ) + PZ ) % PZ;      
          
          BegLogLine( PKFXLOG_GATHER_Z )
            << "SendPackets_FWD_Z():: "
            << " dest1=" << dest1
            << " dest2=" << dest2
            << EndLogLine;
          
          mZYXVol2ZyxPP[ 0 ].Init( mMyP_x, mMyP_y, dest1, 
                                   (Bit32) PacketActorFx_FWD_Z, &mSendInZRouterFwd, mStaticThisPtr );
          
          mZYXVol2ZyxPP[ 1 ].Init( mMyP_x, mMyP_y, dest2, 
                                   (Bit32) PacketActorFx_FWD_Z, &mSendInZRouterFwd, mStaticThisPtr );
          
          BegLogLine( PKFXLOG_GATHER_Z )
            << "SendPackets_FWD_Z():: "
            << "Done with Inits"
            << EndLogLine;
          
          int EmptyList1 = 0;
          int EmptyList2 = mIsUni;

          while( 1 )
            {
              if( !EmptyList1 )
                {
                  mZYXVol2ZyxPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ], 1 );
                  
                  BegLogLine( PKFXLOG_GATHER_Z )
                    << "SendPackets_FWD_Z():: "
                    << "mRetSizes[ 0 ]=" << mRetSizes[ 0 ] 
                    << EndLogLine;
                  
                  if( mRetSizes[ 0 ] != 0 ) 
                    {                                    
                      if( twice==1 )
                        {
                          unsigned int IsSelfSend =  ( mMyP_z == dest1 ); 
                          
                          BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                           mMyP_x,
                                           mMyP_y,
                                           dest1,                       // destination coordinates
                                           0,                           // ???      // destination Fifo Group
                                           (Bit32) PacketActorFx_FWD_Z,     // Active Packet Function matching signature above
                                           0,                           // primary argument to actor
                                           0,                           // secondary 10bit argument to actor
                                           _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)        
                  
                          
                          mGeneratedPackets_fwd_Z[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                          alex_memcpy( mGeneratedPackets_fwd_Z[ count ].mPayload.mData, mPackets[ 0 ].mData, sizeof( ActorPacket ) );
                           
                          mGeneratedPackets_fwd_Z[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 0 ]; 
                          mGeneratedPackets_fwd_Z[ count ].mBGLSendArgs.mSendToSelf = IsSelfSend; 

#if 0
                          if( dest1 - mMyP_z > 0 )
                            {
                            mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 0 ] = TORUS_ZP;
                            mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 1 ] = TORUS_ZP;
                            }
                          else
                            {
                            mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 0 ] = TORUS_ZM;
                            mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 1 ] = TORUS_ZM;
                            }
#endif
                        }
                      
                      count++;                                                    
                    }        
                  else
                    EmptyList1=1;
                }
              
              if( !EmptyList2 )
                {
                  mZYXVol2ZyxPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ], 1 );
                  
                  if( mRetSizes[ 1 ] != 0 ) 
                    {              
                      if( twice==1 )
                        {
                          unsigned int IsSelfSend =  ( mMyP_z == dest2 ); 
                          BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                           mMyP_x,
                                           mMyP_y,
                                           dest2,                       // destination coordinates
                                           0,                           // ???      // destination Fifo Group
                                           (Bit32) PacketActorFx_FWD_Z,     // Active Packet Function matching signature above
                                           0,                           // primary argument to actor
                                           0,                           // secondary 10bit argument to actor
                                           _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)                          
                          mGeneratedPackets_fwd_Z[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                          alex_memcpy( mGeneratedPackets_fwd_Z[ count ].mPayload.mData, mPackets[ 1 ].mData, sizeof( ActorPacket ) ); 
                          mGeneratedPackets_fwd_Z[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 1 ]; 
                          mGeneratedPackets_fwd_Z[ count ].mBGLSendArgs.mSendToSelf = IsSelfSend; 
                          
#if 0
                          if( dest2 - mMyP_z > 0 )
                            {
                              mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 0 ] = TORUS_ZP;
                              mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 1 ] = TORUS_ZP;
                            }
                          else
                            {
                              mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 0 ] = TORUS_ZM;
                              mGeneratedPackets_fwd_Z[ count ].mFifoIndex[ 1 ] = TORUS_ZM;
                            }
#endif
                        }                      
                      count++;                                                                      
                    }        
                  else
                    EmptyList2=1;
                }
              
              if( EmptyList1 && EmptyList2 )
                break;          
            }            
        }

      if( twice == 0 )
        {
          int SizeToAlloc = count * sizeof( PregeneratedBGLMetaPacket ) ;          
          mGeneratedPackets_fwd_Z = (PregeneratedBGLMetaPacket *) pkMalloc( SizeToAlloc );
          mGP_fwd_Z_size          = count;         
          count=0;
        }
    }
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SetupFifo( BGLFifoInfo* aFifoInfo, int aFifoDirection )
{  
  switch( aFifoDirection )
    {
    case TORUS_XP:
      {
        aFifoInfo->mFifo         = (_BGL_TorusFifo *) TS0_I0;
        aFifoInfo->mFifoStatusSetPtr = (_BGL_TorusFifoStatus *) & _ts0_stat0_bounce;
        aFifoInfo->mFifoStatusPtr    = (Bit8 *) & _ts0_stat0_bounce.i0;
        aFifoInfo->mFifoGroup        = (Bit128 *) TS0_S0;             

        break;
      }
    case TORUS_XM:
      {
        aFifoInfo->mFifo         = (_BGL_TorusFifo *) TS0_I1; 
        aFifoInfo->mFifoStatusSetPtr = (_BGL_TorusFifoStatus *) & _ts0_stat0_bounce;
        aFifoInfo->mFifoStatusPtr    = (Bit8 *) & _ts0_stat0_bounce.i1;
        aFifoInfo->mFifoGroup        = (Bit128 *) TS0_S0;   

        break;
      }
    case TORUS_YP:
      {
        aFifoInfo->mFifo         = (_BGL_TorusFifo *) TS0_I2;
        aFifoInfo->mFifoStatusSetPtr = (_BGL_TorusFifoStatus *) & _ts0_stat0_bounce;
        aFifoInfo->mFifoStatusPtr    = (Bit8 *) & _ts0_stat0_bounce.i2;
        aFifoInfo->mFifoGroup        = (Bit128 *) TS0_S0;             

        break;
      }
    case TORUS_YM:
      {
        aFifoInfo->mFifo         = (_BGL_TorusFifo *) TS1_I0;
        aFifoInfo->mFifoStatusSetPtr = (_BGL_TorusFifoStatus *) & _ts0_stat1_bounce;
        aFifoInfo->mFifoStatusPtr    = (Bit8 *) & _ts0_stat1_bounce.i0;
        aFifoInfo->mFifoGroup        = (Bit128 *) TS0_S1;             

        break;
      }
    case TORUS_ZP:
      {
        aFifoInfo->mFifo             = (_BGL_TorusFifo*) TS1_I1;
        aFifoInfo->mFifoStatusSetPtr = (_BGL_TorusFifoStatus *) & _ts0_stat1_bounce;
        aFifoInfo->mFifoStatusPtr    = (Bit8 *) & _ts0_stat1_bounce.i1;
        aFifoInfo->mFifoGroup        = (Bit128 *) TS0_S1;              
                             
        break;
      }
    case TORUS_ZM:
      {
        aFifoInfo->mFifo             = (_BGL_TorusFifo*) TS1_I2;
        aFifoInfo->mFifoStatusSetPtr = (_BGL_TorusFifoStatus *) & _ts0_stat1_bounce;
        aFifoInfo->mFifoStatusPtr    = (Bit8 *) & _ts0_stat1_bounce.i2;
        aFifoInfo->mFifoGroup        = (Bit128 *) TS0_S1;                                                                 
        
        break;
      }
    }
}


template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
GeneratePackets_FWD_Y()
{  
  int count=0;
  
  // First iteration gets out the size of the generated packets array
  for( int twice=0; twice < 2; twice++ )
    {
      int limit1 = ( PY == 1 ) ? 1 : ( PY / 2 );
      int limit2 = ( PZ == 1 ) ? 1 : ( PZ / 2 );

      for( int i=1; i <= limit1; i++ )
        {
          for( int j=1; j <= limit2; j++ )
            {          
              // In the 2D the 2 destinations are:
              // dest1 = ( i + mMyP_z ) % PZ
              // dest2 = ( 1-i + mMyP_z ) % PZ
              int dest_z1 = ( j + mMyP_z )  % PZ;
              int dest_z2 = ( ( (1-j) + mMyP_z ) + PZ ) % PZ;      
              int dest_y1 = ( i + mMyP_y ) % PY;
              int dest_y2 = ( ( (1-i) + mMyP_y ) + PY ) % PY;      
              
              mZyx2YxzPP[ 0 ].Init( mMyP_x, dest_y1, dest_z1,
                                    (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr);          
              mZyx2YxzPP[ 1 ].Init( mMyP_x, dest_y1, dest_z2,
                                    (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr);
              mZyx2YxzPP[ 2 ].Init( mMyP_x, dest_y2, dest_z1,
                                    (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr);
              mZyx2YxzPP[ 3 ].Init( mMyP_x, dest_y2, dest_z2,
                                    (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr);
          
              int EmptyList0 = 0;
              int EmptyList1 = mIsUni;
              int EmptyList2 = mIsUni;
              int EmptyList3 = mIsUni;
              
              while( 1 )
                {
                  if( !EmptyList0 )
                    {
                      mZyx2YxzPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ], 1 );
                      
                      if( mRetSizes[ 0 ] != 0 ) 
                        {              
                          // i==0: Getting the size
                          if( twice==1 )
                            {
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               mMyP_x,
                                               dest_y1,
                                               dest_z1,                       // destination coordinates
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                              
                              
                              mGeneratedPackets_fwd_Y[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( mGeneratedPackets_fwd_Y[ count ].mPayload.mData, mPackets[ 0 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 0 ]; 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_y == dest_y1 ) && ( mMyP_z == dest_z1 ));                               
                              ChooseFifosForDestinationYZ( dest_y1, dest_z1, mGeneratedPackets_fwd_Y[ count ].mFifoIndex );
                            }
                          count++;                              
                        }        
                      else
                        EmptyList0=1;
                    }
                  
                  if( !EmptyList1 )
                    {              
                      mZyx2YxzPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ], 1 );
                      
                      if( mRetSizes[ 1 ] != 0 ) 
                        {                                        
                          
                          // i==0: Getting the size
                          if( twice==1 )
                            {
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               mMyP_x,
                                               dest_y1,
                                               dest_z2,                       // destination coordinates
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                              
                              
                              mGeneratedPackets_fwd_Y[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( mGeneratedPackets_fwd_Y[ count ].mPayload.mData, mPackets[ 1 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 1 ]; 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_y == dest_y1 ) && ( mMyP_z == dest_z2 )); 
                              
                              ChooseFifosForDestinationYZ( dest_y1, dest_z2, mGeneratedPackets_fwd_Y[ count ].mFifoIndex );
                            }
                          
                          count++;                              
                        }        
                      else
                        EmptyList1=1;
                    }     
                  
                  if( !EmptyList2 )
                    {              
                      mZyx2YxzPP[ 2 ].GetNextPacket( & mPackets[ 2 ], & mRetSizes[ 2 ], 1 );
                      
                      if( mRetSizes[ 2 ] != 0 ) 
                        {                                                                  
                          // i==0: Getting the size
                          if( twice==1 )
                            {
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               mMyP_x,
                                               dest_y2,
                                               dest_z1,                       // destination coordinates
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)

                              mGeneratedPackets_fwd_Y[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( mGeneratedPackets_fwd_Y[ count ].mPayload.mData, mPackets[ 2 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 2 ]; 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_y == dest_y2 ) && ( mMyP_z == dest_z1 )); 

                              ChooseFifosForDestinationYZ( dest_y2, dest_z1, mGeneratedPackets_fwd_Y[ count ].mFifoIndex );
                            }
                          
                          count++;                              
                        }        
                      else
                        EmptyList2=1;
                    }     
                  
                  if( !EmptyList3 )
                    {              
                      mZyx2YxzPP[ 3 ].GetNextPacket( & mPackets[ 3 ], & mRetSizes[ 3 ], 1 );
                      
                      if( mRetSizes[ 3 ] != 0 ) 
                        {                           
                          // i==0: Getting the size
                          if( twice==1 )
                            {                              
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               mMyP_x,
                                               dest_y2,
                                               dest_z2,                       // destination coordinates
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                              
                              mGeneratedPackets_fwd_Y[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( &mGeneratedPackets_fwd_Y[ count ].mPayload.mData, mPackets[ 3 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 3 ]; 
                              mGeneratedPackets_fwd_Y[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_y == dest_y2 ) && ( mMyP_z == dest_z2 )); 
                             
                              ChooseFifosForDestinationYZ( dest_y2, dest_z2, mGeneratedPackets_fwd_Y[ count ].mFifoIndex ); 
                            }
                          
                          count++;                              
                        }        
                      else
                        EmptyList3=1;
                    }     
                  
                  if( EmptyList0 && EmptyList1 && EmptyList2 && EmptyList3  )
                    break;          
                }          
            }
        }

      if( twice == 0 )
        {
          int SizeToAlloc = count * sizeof( PregeneratedBGLMetaPacket ) ;
          
          mGeneratedPackets_fwd_Y = (PregeneratedBGLMetaPacket *) pkMalloc( SizeToAlloc );
          mGP_fwd_Y_size          = count;

          BegLogLine( 0 )
            << "SizeToAlloc=" << SizeToAlloc
            << "mGP_fwd_Y_size" << mGP_fwd_Y_size
            << "mGeneratedPackets_fwd_Y" << (void *) mGeneratedPackets_fwd_Y
            << EndLogLine;

          count=0;
        }
    }    
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
GeneratePackets_FWD_X()
{
   // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes
  int count=0;

  // First iteration gets out the size of the generated packets array
  for( int twice=0; twice < 2; twice++ )
    {
      int limit1 = ( PX == 1 ) ? 1 : ( PX / 2 );
      int limit2 = ( PY == 1 ) ? 1 : ( PY / 2 );

      for( int i=1; i <= limit1; i++ )
        {
          for( int j=1; j <= limit2; j++ )
            {
              // In the 2D the 2 destinations are:
              // dest1 = ( i + mMyP_z ) % PZ
              // dest2 = ( 1-i + mMyP_z ) % PZ
              int dest_y1 = ( j + mMyP_y )  % PY;
              int dest_y2 = ( ( (1-j) + mMyP_y ) + PY ) % PY;      
              int dest_x1 = ( i + mMyP_x ) % PX;
              int dest_x2 = ( ( (1-i) + mMyP_x ) + PX ) % PX;      
              
              mYxz2XyzPP[ 0 ].Init( dest_x1, dest_y1, mMyP_z,
                                    (Bit32) PacketActorFx_FWD_X, &mSendInXRouterFwd, mStaticThisPtr );          
              mYxz2XyzPP[ 1 ].Init( dest_x1, dest_y2, mMyP_z,
                                    (Bit32) PacketActorFx_FWD_X, &mSendInXRouterFwd, mStaticThisPtr );
              mYxz2XyzPP[ 2 ].Init( dest_x2, dest_y1, mMyP_z,
                                    (Bit32) PacketActorFx_FWD_X, &mSendInXRouterFwd, mStaticThisPtr );
              mYxz2XyzPP[ 3 ].Init( dest_x2, dest_y2, mMyP_z,
                                    (Bit32) PacketActorFx_FWD_X, &mSendInXRouterFwd, mStaticThisPtr );
              
              int EmptyList0 = 0;
              int EmptyList1 = mIsUni;
              int EmptyList2 = mIsUni;
              int EmptyList3 = mIsUni;
              
              while( 1 )
                {
                  if( !EmptyList0 )
                    {
                      mYxz2XyzPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ], 1 );
                      
                      if( mRetSizes[ 0 ] != 0 ) 
                        {              
                          if( twice==1 )
                            {
                              
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               dest_x1,
                                               dest_y1,
                                               mMyP_z,                       // destination coordinates
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_X,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)

                              mGeneratedPackets_fwd_X[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( mGeneratedPackets_fwd_X[ count ].mPayload.mData, mPackets[ 0 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 0 ]; 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_x == dest_x1 ) && ( mMyP_y == dest_y1 )); 
                              ChooseFifosForDestinationXY( dest_x1, dest_y1, mGeneratedPackets_fwd_X[ count ].mFifoIndex );
                                                            

#if 0 
                             if( (BGLPartitionGetRank() % 2)  == 0 )                          
                                if( dest_x1 - mMyP_x > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XM );
                              else
                                if( dest_y1 - mMyP_y > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YM );
#endif

                            }
                          
                          count++;                                                        
                        }        
                      else
                        EmptyList0=1;
                    }
                  
                  if( !EmptyList1 )
                    {              
                      mYxz2XyzPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ], 1 );
                      
                      if( mRetSizes[ 1 ] != 0 ) 
                        {              
                          if( twice==1 )
                            {                              
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               dest_x1,
                                               dest_y2,
                                               mMyP_z,                       // destination coordinates
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_X,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                              
                              mGeneratedPackets_fwd_X[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( mGeneratedPackets_fwd_X[ count ].mPayload.mData, mPackets[ 1 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 1 ]; 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_x == dest_x1 ) && ( mMyP_y == dest_y2 )); 
                              ChooseFifosForDestinationXY( dest_x1, dest_y2, mGeneratedPackets_fwd_X[ count ].mFifoIndex );                              
#if 0
                              if( (BGLPartitionGetRank() % 2)  == 0 )                          
                                if( dest_x1 - mMyP_x > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XM );
                              else
                                if( dest_y2 - mMyP_y > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YM );
#endif
                            }
                          
                          count++;                                                                                  
                        }        
                      else
                        EmptyList1=1;
                    }     
          
                  if( !EmptyList2 )
                    {              
                      mYxz2XyzPP[ 2 ].GetNextPacket( & mPackets[ 2 ], & mRetSizes[ 2 ], 1 );
                      
                      if( mRetSizes[ 2 ] != 0 ) 
                        {              
                          if( twice==1 )
                            {                              
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               dest_x2,
                                               dest_y1,
                                               mMyP_z,        
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_X,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                              
                              mGeneratedPackets_fwd_X[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( mGeneratedPackets_fwd_X[ count ].mPayload.mData, mPackets[ 2 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 2 ]; 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_x == dest_x2 ) && ( mMyP_y == dest_y1 )); 
                              ChooseFifosForDestinationXY( dest_x2, dest_y1, mGeneratedPackets_fwd_X[ count ].mFifoIndex );                              
#if 0
                              if( (BGLPartitionGetRank() % 2)  == 0 )                          
                                if( dest_x2 - mMyP_x > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XM );
                              else
                                if( dest_y1 - mMyP_y > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YM );
#endif
                            }                          
                          count++;                                                                                  
                        }        
                      else
                        EmptyList2=1;
                    }     
          
                  if( !EmptyList3 )
                    {              
                      mYxz2XyzPP[ 3 ].GetNextPacket( & mPackets[ 3 ], & mRetSizes[ 3 ], 1 );
              
                      if( mRetSizes[ 3 ] != 0 ) 
                        {              
                          if( twice==1 )
                            {
                              
                              BGLTorusMakeHdr( & mTempBglHdr,              // Filled in on return
                                               dest_x2,
                                               dest_y2,
                                               mMyP_z,        
                                               0,                           // ???      // destination Fifo Group
                                               (Bit32) PacketActorFx_FWD_X,     // Active Packet Function matching signature above
                                               0,                           // primary argument to actor
                                               0,                           // secondary 10bit argument to actor
                                               _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                              
                              mGeneratedPackets_fwd_X[ count ].mBGLTorus_Hdr               = mTempBglHdr; 
                              alex_memcpy( mGeneratedPackets_fwd_X[ count ].mPayload.mData, mPackets[ 3 ].mData, sizeof( ActorPacket ) ); 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSizeOfPacket = mRetSizes[ 3 ]; 
                              mGeneratedPackets_fwd_X[ count ].mBGLSendArgs.mSendToSelf = (( mMyP_x == dest_x2 ) && ( mMyP_y == dest_y2 )); 
                              ChooseFifosForDestinationXY( dest_x2, dest_y2, mGeneratedPackets_fwd_X[ count ].mFifoIndex );
#if 0                              
                              if( (BGLPartitionGetRank() % 2)  == 0 )                          
                                if( dest_x2 - mMyP_x > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_XM );
                              else
                                if( dest_y2 - mMyP_y > 0 )
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YP );
                                else
                                  SetupFifo( &mGeneratedPackets_fwd_X[ count ].mFifoInfo, TORUS_YM );                              
#endif
                            }                          
                          count++;                                                                                  
                        }        
                      else
                        EmptyList3=1;
                    }     
                  
                  if( EmptyList0 && EmptyList1 && EmptyList2 && EmptyList3  )
                    break;          
                }          
            }
        }
      
      if( twice == 0 )
        {
          int SizeToAlloc = count * sizeof( PregeneratedBGLMetaPacket ) ;
          
          mGeneratedPackets_fwd_X = (PregeneratedBGLMetaPacket *) pkMalloc( SizeToAlloc );
          mGP_fwd_X_size          = count;                   
          count=0;
        }
    }
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SendPackets_FWD_Z()
{
  BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
    << "Entering SendPackets_Z"
      << EndLogLine;

  // Update the fifo status. These status registers will get
  // updated accordingly as the fifos fill up.
  BGLTorusGetStatus0( &_ts0_stat0_bounce );
  BGLTorusGetStatus1( &_ts0_stat1_bounce );

  for( int i=0; i < mGP_fwd_Z_size; i++ )
    {
      int SizeOfData = mGeneratedPackets_fwd_Z[ i ].mBGLSendArgs.mSizeOfPacket;

      alex_memcpy( mTempActorPacket.mData, mGeneratedPackets_fwd_Z[ i ].mPayload.mData, SizeOfData );

      char *Data = mTempActorPacket.mData;
      EndOfPacket *EOM = (EndOfPacket *) Data;

      // Convert Data Ptrs to Data
      int CurrentOffsetInData = 0;
      while( EOM->mValue != END_OF_PACKET )
        {
          FFT_Hdr* hdr = (FFT_Hdr *) &Data[ CurrentOffsetInData ];
          int count = hdr->count;

          CurrentOffsetInData += sizeof( FFT_Hdr );

          for( int d=0; d < count; d++ )
            {
              mDataPtr = (unsigned int*) &Data[ CurrentOffsetInData ]; 

              double_copy( (double *) & Data[ CurrentOffsetInData ],
                           (double *) (*mDataPtr),
                           2 * sizeof(double) );     
              
              CurrentOffsetInData += sizeof( Value );
            }
          
          EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
        }
      
      // Need to send rounded to the next full chunk
      // The 16 is in there

      int SizeToAlign = 16 + SizeOfData;     
      SizeToAlign = round_to_32( SizeToAlign );
      SizeOfData = SizeToAlign - 16;

      SendInZ_fwdStart.HitOE( PKTRACE_SEND_IN_Z,
                              "ForwardFFT", 
                              Platform::Thread::GetId(),
                              SendInZ_fwdStart ); 
            

//       BGLTorusFifoActivePacketSend( mGeneratedPackets_fwd_Z[ i ].mBGLTorus_Hdr,
//                                     (void *) mTempActorPacket.mData,
//                                     SizeOfData,
//                                     mGeneratedPackets_fwd_Z[ i ].mBGLSendArgs.mSendToSelf,
//                                     mGeneratedPackets_fwd_Z[ i ].mFifoInfo.mFifo,
//                                     mGeneratedPackets_fwd_Z[ i ].mFifoInfo.mFifoStatusPtr,
//                                     mGeneratedPackets_fwd_Z[ i ].mFifoInfo.mFifoStatusSetPtr,
//                                     mGeneratedPackets_fwd_Z[ i ].mFifoInfo.mFifoGroup );                                                                        
      BGLTorusSimpleSend( mGeneratedPackets_fwd_Z[ i ].mBGLTorus_Hdr,
                          (void *) mTempActorPacket.mData,
                          SizeOfData,
                          mGeneratedPackets_fwd_Z[ i ].mBGLSendArgs.mSendToSelf );
      
      SendInZ_fwdFinis.HitOE( PKTRACE_SEND_IN_Z,
                              "ForwardFFT", 
                              Platform::Thread::GetId(),
                              SendInZ_fwdFinis );       
    }


    BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
      << "Exiting SendPackets_Z"
      << EndLogLine;      
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SendPackets_FWD_Y()
  {
  BegLogLine( PKFXLOG_GATHER_Y || PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering SendPackets_Y"
      << EndLogLine;

  // Update the fifo status. These status registers will get
  // updated accordingly as the fifos fill up.
  BGLTorusGetStatus0( &_ts0_stat0_bounce );
  BGLTorusGetStatus1( &_ts0_stat1_bounce );

  for( int i=0; i < mGP_fwd_Y_size; i++ )
    {
      int SizeOfData = mGeneratedPackets_fwd_Y[ i ].mBGLSendArgs.mSizeOfPacket;

      alex_memcpy( mTempActorPacket.mData, mGeneratedPackets_fwd_Y[ i ].mPayload.mData, SizeOfData );

      char *Data = mTempActorPacket.mData;
      EndOfPacket *EOM = (EndOfPacket *) Data;

      // Convert Data Ptrs to Data
      int CurrentOffsetInData = 0;
      while( EOM->mValue != END_OF_PACKET )
        {
          FFT_Hdr* hdr = (FFT_Hdr *) &Data[ CurrentOffsetInData ];
          int count = hdr->count;

          CurrentOffsetInData += sizeof( FFT_Hdr );

          for( int d=0; d < count; d++ )
            {
              mDataPtr = (unsigned int*) &Data[ CurrentOffsetInData ]; 

              double_copy( (double *) & Data[ CurrentOffsetInData ],
                           (double *) (*mDataPtr),
                           2 * sizeof(double) );     
              
              CurrentOffsetInData += sizeof( Value );
            }
          
          EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
        }

      // Need to send rounded to the next full chunk
      // The 16 is in there

      int OldSize = SizeOfData;
      int SizeToAlign = 16 + SizeOfData;     
      SizeToAlign = round_to_32( SizeToAlign );
      SizeOfData = SizeToAlign - 16;

//       BegLogLine( 1 )
//         << " OldSize=" << OldSize
//         << " SizeToAlign=" << SizeToAlign
//         << EndLogLine;
      
      SendInY_fwdStart.HitOE( PKTRACE_SEND_IN_Y,
                              "ForwardFFT", 
                              Platform::Thread::GetId(),
                              SendInY_fwdStart ); 
      
#if 1
       BGLTorusSimpleSend( mGeneratedPackets_fwd_Y[ i ].mBGLTorus_Hdr,
                           (void *) mTempActorPacket.mData,
                           SizeOfData,
                           mGeneratedPackets_fwd_Y[ i ].mBGLSendArgs.mSendToSelf );      
#else            
        BGLTorusFifoActivePacketSend( mGeneratedPackets_fwd_Y[ i ].mBGLTorus_Hdr,
                                      (void *) mTempActorPacket.mData,
                                      SizeOfData,
                                      mGeneratedPackets_fwd_Y[ i ].mBGLSendArgs.mSendToSelf,
                                      mGeneratedPackets_fwd_Y[ i ].mFifoIndex );
#endif
      
      SendInY_fwdFinis.HitOE( PKTRACE_SEND_IN_Y,
                              "ForwardFFT", 
                              Platform::Thread::GetId(),
                              SendInY_fwdFinis );       
    }

  BegLogLine( PKFXLOG_GATHER_Y || PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_FWD_Y"
    << EndLogLine;      
  }

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SendPackets_FWD_X()
  {
  BegLogLine( PKFXLOG_GATHER_X || PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering SendPackets_X"
      << EndLogLine;

  BGLTorusGetStatus0( &_ts0_stat0_bounce );
  BGLTorusGetStatus1( &_ts0_stat1_bounce );

  for( int i=0; i < mGP_fwd_X_size; i++ )
  // for( int i=mGP_fwd_X_size-1; i >= 0; i-- )
    {
      int SizeOfData = mGeneratedPackets_fwd_X[ i ].mBGLSendArgs.mSizeOfPacket;

      alex_memcpy( mTempActorPacket.mData, mGeneratedPackets_fwd_X[ i ].mPayload.mData, SizeOfData );

      char *Data = mTempActorPacket.mData;
      EndOfPacket *EOM = (EndOfPacket *) Data;

      // Convert Data Ptrs to Data
      int CurrentOffsetInData = 0;
      while( EOM->mValue != END_OF_PACKET )
        {
          FFT_Hdr* hdr = (FFT_Hdr *) &Data[ CurrentOffsetInData ];
          int count = hdr->count;

          CurrentOffsetInData += sizeof( FFT_Hdr );

          for( int d=0; d < count; d++ )
            {
              mDataPtr = (unsigned int*) &Data[ CurrentOffsetInData ]; 

              double_copy( (double *) & Data[ CurrentOffsetInData ],
                           (double *) (*mDataPtr),
                           2 * sizeof(double) );     
              
              CurrentOffsetInData += sizeof( Value );
            }
          
          EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
        }
      
      int SizeToAlign = 16 + SizeOfData;     
      SizeToAlign = round_to_32( SizeToAlign );
      SizeOfData = SizeToAlign - 16;
      
      SendInX_fwdStart.HitOE( PKTRACE_SEND_IN_X,
                              "ForwardFFT", 
                              Platform::Thread::GetId(),
                              SendInX_fwdStart ); 
      
#if 1
      BGLTorusSimpleSend( mGeneratedPackets_fwd_X[ i ].mBGLTorus_Hdr,
                          (void *) mTempActorPacket.mData,
                          SizeOfData,
                          mGeneratedPackets_fwd_X[ i ].mBGLSendArgs.mSendToSelf );
#else      
       BGLTorusFifoActivePacketSend( mGeneratedPackets_fwd_X[ i ].mBGLTorus_Hdr,
                                     (void *) mTempActorPacket.mData,
                                     SizeOfData,
                                     mGeneratedPackets_fwd_X[ i ].mBGLSendArgs.mSendToSelf,
                                     mGeneratedPackets_fwd_X[ i ].mFifoIndex );
#endif

      
      SendInX_fwdFinis.HitOE( PKTRACE_SEND_IN_X,
                              "ForwardFFT", 
                              Platform::Thread::GetId(),
                              SendInX_fwdFinis );       
    }
  
  BegLogLine( PKFXLOG_GATHER_X || PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_FWD_X"
    << EndLogLine;  

  }

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SendPackets_REV_Y()
  {
  BegLogLine( PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering SendPackets_REV_Y"
      << EndLogLine;

  int PacketsSent = 0;
  
  int limit1 = ( PX == 1 ) ? 1 : ( PX / 2 );
  int limit2 = ( PY == 1 ) ? 1 : ( PY / 2 );
  
  for( int i=1; i <= limit1; i++ )
    {
      for( int j=1; j <= limit2; j++ )
        {
          
          // In the 2D the 2 destinations are:
          // dest1 = ( i + mMyP_z ) % PZ
          // dest2 = ( 1-i + mMyP_z ) % PZ
          int dest_y1 = ( j + mMyP_y )  % PY;
          int dest_y2 = ( ( (1-j) + mMyP_y ) + PY ) % PY;      
          int dest_x1 = ( i + mMyP_x ) % PX;
          int dest_x2 = ( ( (1-i) + mMyP_x ) + PX ) % PX;      
          
          mXyz2YzxPP[ 0 ].Init( dest_x1, dest_y1, mMyP_z,
                                (Bit32) PacketActorFx_REV_2Y, &mSendInYRouterRev, mStaticThisPtr, 1 );          
          mXyz2YzxPP[ 1 ].Init( dest_x1, dest_y2, mMyP_z,
                                (Bit32) PacketActorFx_REV_2Y, &mSendInYRouterRev, mStaticThisPtr, 1 );
          mXyz2YzxPP[ 2 ].Init( dest_x2, dest_y1, mMyP_z,
                                (Bit32) PacketActorFx_REV_2Y, &mSendInYRouterRev, mStaticThisPtr, 1 );
          mXyz2YzxPP[ 3 ].Init( dest_x2, dest_y2, mMyP_z,
                                (Bit32) PacketActorFx_REV_2Y, &mSendInYRouterRev, mStaticThisPtr, 1 );
          
          // mSendInYRouterRev.ValidateCheckSum( __LINE__ );

          int EmptyList0 = 0;
          int EmptyList1 = mIsUni ;
          int EmptyList2 = mIsUni ;
          int EmptyList3 = mIsUni ;
          
          while( 1 )
            {
              if( !EmptyList0 )
                {
                  // mSendInYRouterRev.ValidateCheckSum(  __LINE__ );
                  mXyz2YzxPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ], 1 );
                  // mSendInYRouterRev.ValidateCheckSum(  __LINE__ );
        
                  if( mRetSizes[ 0 ] != 0 ) 
                    {              
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       dest_x1,
                                       dest_y1,
                                       mMyP_z,                       // destination coordinates
                                       0,                           // ???      // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2Y,     // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2Y,
                                                                         dest_x1,
                                                                         dest_y1,
                                                                         mMyP_z,
                                                                         mRetSizes[ 0 ],
                                                                         &mPackets[ 0 ] );              
                    }        
                  else
                    EmptyList0=1;
                }

          if( !EmptyList1 )
            {              
              // mSendInYRouterRev.ValidateCheckSum(  __LINE__ );
              mXyz2YzxPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ], 1 );
              // mSendInYRouterRev.ValidateCheckSum(  __LINE__ );
      
              if( mRetSizes[ 1 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   dest_x1,
                                   dest_y2,
                                   mMyP_z,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2Y,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2Y,
                                                                     dest_x1,
                                                                     dest_y2,
                                                                     mMyP_z,        
                                                                     mRetSizes[ 1 ],
                                                                     &mPackets[ 1 ] );              
                }        
              else
                EmptyList1=1;
            }     
          
          if( !EmptyList2 )
            {              
              // mSendInYRouterRev.ValidateCheckSum(  __LINE__ );
              mXyz2YzxPP[ 2 ].GetNextPacket( & mPackets[ 2 ], & mRetSizes[ 2 ], 1  );
              // mSendInYRouterRev.ValidateCheckSum( __LINE__ );

              if( mRetSizes[ 2 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   dest_x2,
                                   dest_y1,
                                   mMyP_z,        
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2Y,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2Y,
                                                                     dest_x2,
                                                                     dest_y1,
                                                                     mMyP_z,        
                                                                     mRetSizes[ 2 ],
                                                                     &mPackets[ 2 ] );              
                }        
              else
                EmptyList2=1;
            }     
          
          if( !EmptyList3 )
            {              
              // mSendInYRouterRev.ValidateCheckSum(  __LINE__ );
              mXyz2YzxPP[ 3 ].GetNextPacket( & mPackets[ 3 ], & mRetSizes[ 3 ], 1 );
              // mSendInYRouterRev.ValidateCheckSum( __LINE__ );

              if( mRetSizes[ 3 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   dest_x2,
                                   dest_y2,
                                   mMyP_z,        
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2Y,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                  
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2Y,
                                                                     dest_x2,
                                                                     dest_y2,
                                                                     mMyP_z,        
                                                                     mRetSizes[ 3 ],
                                                                     &mPackets[ 3 ] );              
                }        
              else
                EmptyList3=1;
            }     
          
          if( EmptyList0 && EmptyList1 && EmptyList2 && EmptyList3  )
            break;          
            }          
        }
    }
        
  BegLogLine( PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_REV_Y"
    << EndLogLine;  
  }

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SendPackets_REV_Z()
  {
  BegLogLine( PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering SendPackets_REV_Z"
      << EndLogLine;

  int PacketsSent = 0;

  int limit1 = ( PY == 1 ) ? 1 : ( PY / 2 );
  int limit2 = ( PZ == 1 ) ? 1 : ( PZ / 2 );

  for( int i=1; i <= limit1; i++ )
    {
      for( int j=1; j <= limit2; j++ )
        {
          
          // In the 2D the 2 destinations are:
          // dest1 = ( i + mMyP_z ) % PZ
          // dest2 = ( 1-i + mMyP_z ) % PZ
          int dest_z1 = ( j + mMyP_z )  % PZ;
          int dest_z2 = ( ( (1-j) + mMyP_z ) + PZ ) % PZ;      
          int dest_y1 = ( i + mMyP_y ) % PY;
          int dest_y2 = ( ( (1-i) + mMyP_y ) + PY ) % PY;      
          
          mYzx2ZyxPP[ 0 ].Init( mMyP_x, dest_y1, dest_z1,
                                (Bit32) PacketActorFx_REV_2Z, &mSendInZRouterRev, mStaticThisPtr, 1 );          
          mYzx2ZyxPP[ 1 ].Init( mMyP_x, dest_y1, dest_z2,
                                (Bit32) PacketActorFx_REV_2Z, &mSendInZRouterRev, mStaticThisPtr, 1 );
          mYzx2ZyxPP[ 2 ].Init( mMyP_x, dest_y2, dest_z1,
                                (Bit32) PacketActorFx_REV_2Z, &mSendInZRouterRev, mStaticThisPtr, 1 );
          mYzx2ZyxPP[ 3 ].Init( mMyP_x, dest_y2, dest_z2,
                                (Bit32) PacketActorFx_REV_2Z, &mSendInZRouterRev, mStaticThisPtr, 1 );
          
          int EmptyList0 = 0;
          int EmptyList1 = mIsUni;
          int EmptyList2 = mIsUni;
          int EmptyList3 = mIsUni;
          
          while( 1 )
            {
              if( !EmptyList0 )
                {
                  mYzx2ZyxPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ] );
                  
                  if( mRetSizes[ 0 ] != 0 ) 
                    {              
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       mMyP_x,
                                       dest_y1,
                                       dest_z1,                       // destination coordinates
                                       0,                           // ???      // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2Z,     // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2Z,
                                                                         mMyP_x,
                                                                         dest_y1,
                                                                         dest_z1,
                                                                         mRetSizes[ 0 ],
                                                                         &mPackets[ 0 ] );              
                    }        
                  else
                    EmptyList0=1;
                }

          if( !EmptyList1 )
            {              
              mYzx2ZyxPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ] );
              
              if( mRetSizes[ 1 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   dest_y1,
                                   dest_z2,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2Z,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2Z,
                                                                     mMyP_x,
                                                                     dest_y1,
                                                                     dest_z2,
                                                                     mRetSizes[ 1 ],
                                                                     &mPackets[ 1 ] );              
                }        
              else
                EmptyList1=1;
            }     
          
          if( !EmptyList2 )
            {              
              mYzx2ZyxPP[ 2 ].GetNextPacket( & mPackets[ 2 ], & mRetSizes[ 2 ] );
              
              if( mRetSizes[ 2 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   dest_y2,
                                   dest_z1,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2Z,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2Z,
                                                                     mMyP_x,
                                                                     dest_y2,
                                                                     dest_z1,
                                                                     mRetSizes[ 2 ],
                                                                     &mPackets[ 2 ] );              
                }        
              else
                EmptyList2=1;
            }     
          
          if( !EmptyList3 )
            {              
              mYzx2ZyxPP[ 3 ].GetNextPacket( & mPackets[ 3 ], & mRetSizes[ 3 ] );
              
              if( mRetSizes[ 3 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   dest_y2,
                                   dest_z2,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2Z,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2Z,
                                                                     mMyP_x,
                                                                     dest_y2,
                                                                     dest_z2,
                                                                     mRetSizes[ 3 ],
                                                                     &mPackets[ 3 ] );              
                }        
              else
                EmptyList3=1;
            }     
          
          if( EmptyList0 && EmptyList1 && EmptyList2 && EmptyList3  )
            break;          
            }          
        }
    }
        
  BegLogLine( PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_REV_Y"
    << EndLogLine;  
  }

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SendPackets_REV_XYZVol()
{
  BegLogLine( PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering SendPackets_REV_XYZVol"
      << EndLogLine;

  int PacketsSent = 0;

  int limit1 = ( PZ == 1 ) ? 1 : ( PZ / 2 );
 
  for( int i=1; i <= limit1; i++ )
    {      
      // In the 2D the 2 destinations are:
      // dest1 = ( i + mMyP_z ) % PZ
      // dest2 = ( 1-i + mMyP_z ) % PZ
      int dest1 = ( i + mMyP_z ) % PZ;
      int dest2 = (( (1-i) + mMyP_z ) + PZ ) % PZ;      

      BegLogLine( PKFXLOG_GATHER_Z )
        << "SendPackets_FWD_Z():: "
        << " dest1=" << dest1
        << " dest2=" << dest2
        << EndLogLine;

      mZyx2ZyxVolPP[ 0 ].Init( mMyP_x, mMyP_y, dest1, 
                               (Bit32)  PacketActorFx_REV_2XYZ, &mSendInXYZVolRouterRev, mStaticThisPtr );
      
      mZyx2ZyxVolPP[ 1 ].Init( mMyP_x, mMyP_y, dest2, 
                               (Bit32)  PacketActorFx_REV_2XYZ, &mSendInXYZVolRouterRev, mStaticThisPtr );

      BegLogLine( PKFXLOG_GATHER_Z )
        << "SendPackets_FWD_Z():: "
        << "Done with Inits"
        << EndLogLine;
      
      int EmptyList1 = 0;
      int EmptyList2 = mIsUni;
      while( 1 )
        {
          if( !EmptyList1 )
            {
              mZyx2ZyxVolPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ] );

              BegLogLine( PKFXLOG_GATHER_Z )
                << "SendPackets_FWD_Z():: "
                << "mRetSizes[ 0 ]=" << mRetSizes[ 0 ] 
                << EndLogLine;
              
              if( mRetSizes[ 0 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   mMyP_y,
                                   dest1,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2XYZ,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                  
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2XYZ,
                                                                     mMyP_x,
                                                                     mMyP_y,
                                                                     dest1,
                                                                     mRetSizes[ 0 ],
                                                                     &mPackets[ 0 ] );              
                }        
              else
                EmptyList1=1;
            }

          if( !EmptyList2 )
            {
              mZyx2ZyxVolPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ] );
              
              if( mRetSizes[ 1 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   mMyP_y,
                                   dest2,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_REV_2XYZ,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                  
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_REV_2XYZ,
                                                                     mMyP_x,
                                                                     mMyP_y,
                                                                     dest2,
                                                                     mRetSizes[ 1 ],
                                                                     &mPackets[ 1 ] );              
                }        
              else
                EmptyList2=1;
            }     
          
          if( EmptyList1 && EmptyList2 )
            break;          
        }      
    }

  BegLogLine( PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_REV_XYZVol"
    << EndLogLine;  
  
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
ActiveMsgBarrier(FFT_Counter & aCurrentValue,
                 int           aTargetValue,
                 void*         aPtr )
  {

    ActiveMsgBarrierStart.HitOE( PKTRACE_ACTIVE_MSG_BARRIER, 
                                 "ActiveMsgBarrier", 
                                 Platform::Thread::GetId(),
                                 ActiveMsgBarrierStart );

  BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER )
    << "ActiveMsgBarrier:: "
    << " aCurrentValue" << aCurrentValue.mVar
    << " of " << aTargetValue
    << EndLogLine;

  // BarMon is a monitor of progress in filling in the bar.
  // BarMon loosly tracks this progress.
  int barMon = aCurrentValue.mVar ;

  Platform::Memory::ImportFence((void *) & aCurrentValue, sizeof( FFT_Counter ));
  while( aCurrentValue.mVar != aTargetValue )
    {
    Platform::Memory::ImportFence((void *) & aCurrentValue, sizeof( FFT_Counter ));

    if( barMon != aCurrentValue.mVar )
      {
      BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER )
          << "ActiveMsgBarrier:: "
          << aCurrentValue.mVar
          << " of "
          << aTargetValue
          << EndLogLine;

      barMon = aCurrentValue.mVar;
      }
    }

  BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER )
    << "ActiveMsgBarrier  Done! "
    << aCurrentValue.mVar
    << " of " << aTargetValue
    << EndLogLine;
  

  ImportFenceStart.HitOE( PKTRACE_IMPORT_FENCE,
                          "ImportFence", 
                          Platform::Thread::GetId(),
                          ImportFenceStart );  
  
  // Platform::Memory::ImportFence();
  Platform::Memory::ImportFence( aPtr, sizeof( Value ) * mLocalNx * mLocalNy * mLocalNz );
  // Platform::Memory::ImportFence( &mDataContainer2, sizeof( Value ) * mLocalNx * mLocalNy * mLocalNz );
  
  ImportFenceFinis.HitOE( PKTRACE_IMPORT_FENCE,
                          "ImportFence", 
                          Platform::Thread::GetId(),
                          ImportFenceFinis );  

  ActiveMsgBarrierFinis.HitOE( PKTRACE_ACTIVE_MSG_BARRIER, 
                               "ActiveMsgBarrier", 
                               Platform::Thread::GetId(),
                               ActiveMsgBarrierFinis );
    
  }

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
InitHeaders()
  {

    // Generate the headers in Z      
    // First we need to obtain the number of packets to send
    // Traverse the linked lists and get the number of packets to send

  }

template<class FFT_PLAN >
inline void FFT3D< FFT_PLAN >::Reset()
  {
    
  for( int i=0; i<4; i++)   
    mRetSizes[i] = 0;

  BegLogLine( PKFXLOG_RESET )
    << "FFT3D::Reset() " 
    << " DYZ=" << mDataContainer1.DYZ()
    << " DXZ=" << mDataContainer1.DXZ()
    << " DXY=" << mDataContainer1.DXY()
    << EndLogLine;

  mTotalPointsRecv_x.mVar = N_X * mDataContainer1.DYZ();
  mTotalPointsRecv_y.mVar = N_Y * mDataContainer1.DXZ();
  mTotalPointsRecv_z.mVar = N_Z * mDataContainer1.DXY();

  BegLogLine( PKFXLOG_RESET )
    << "FFT3D::Reset() " 
    << " mTotalPointsRecv_x.mVar=" << mTotalPointsRecv_x.mVar
    << " mTotalPointsRecv_y.mVar=" << mTotalPointsRecv_y.mVar
    << " mTotalPointsRecv_z.mVar=" << mTotalPointsRecv_z.mVar
    << EndLogLine;

  mTotalFFTsRecv_x = mDataContainer1.DYZ();
  mTotalFFTsRecv_y = mDataContainer1.DXZ();
  mTotalFFTsRecv_z = mDataContainer1.DXY();

  mTotalRecvData_xyzvol = mDataContainer1.DNX() * 
                          mDataContainer1.DNY() * 
                          mDataContainer1.DNZ();

  mCompleteFFTs_x.mVar = 0;
  mCompleteFFTs_y.mVar = 0; 
  mCompleteFFTs_z.mVar = 0;
  mCompleteXYZVol.mVar = 0;

  mIOP_FFT_Compute_Done_x.mVar = 0;
  mIOP_FFT_Compute_Done_y.mVar = 0;
  mIOP_FFT_Compute_Done_z.mVar = 0;

  for(int i=0; i < mDataContainer1.DYZ(); i++)
    mTotalRecvData_x[ i ] = N_X;

  for(int i=0; i < mDataContainer1.DXZ(); i++)
    mTotalRecvData_y[ i ] = N_Y;

  for(int i=0; i < mDataContainer1.DXY(); i++)
    mTotalRecvData_z[ i ] = N_Z;

  for(int i=0; i<mNumberOfNodes; i++ )
    {
      mSentToTask[ i ] =0; 
    }

  Platform::Memory::ExportFence();
  Platform::Memory::ImportFence();

#ifdef FFT3D_BARRIER_PHASES
  BGLPartitionBarrier();
#endif
  };

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::RunForward()
{  
  fftForwardTracerStart.HitOE(  PKTRACE_FORWARD_FFT,
                                "ForwardFFT", 
                                Platform::Thread::GetId(),
                                fftForwardTracerStart ); 

//   TracingCost.HitOE(  PKTRACE_FORWARD_FFT,
//                       "TracingCost", 
//                       Platform::Thread::GetId(),
//                       TracingCost ); 

#ifdef FFT3D_BARRIER_PHASES  
  BGLPartitionBarrier();
#endif

  BegLogLine( PKFXLOG_EXECUTE )
    << "FFT::DoFFT:: Start"
    << EndLogLine;
  
  // Start by collecting and doing a 1D FFT in the z-direction
  int myRank = BGLPartitionGetRankXYZ();
  
  TR_SendPackets_ZStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_ZStart", 
                               Platform::Thread::GetId(),
                               TR_SendPackets_ZStart );
  
  SendPackets_FWD_Z();
    
  TR_SendPackets_ZFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                         "TR_SendPackets_ZFinis", 
                         Platform::Thread::GetId(),
                         TR_SendPackets_ZFinis );

  // The IO core is receiving data. When all the data is here start 
  // computing the 1D FFTs
  // FUTURE:: Optimize to start computing FFTs as soon as you can
  ActiveMsgBarrier( mCompleteFFTs_z, mTotalFFTsRecv_z, mZYXVol2ZyxPhaseData_R );  

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After ActiveMsgBarrier "
    << EndLogLine;

  /*************************************************************
   *   Trigger Computation in Z
   ************************************************************/
  ComputeFFTsInQueueActorPacket cqap_IOP;
  cqap_IOP.mStart = mTotalFFTsRecv_z / 2;
  cqap_IOP.mCount = mTotalFFTsRecv_z - cqap_IOP.mStart;
  cqap_IOP.ThisPtr = mStaticThisPtr;
  
  BegLogLine(0)
    << "  cqap_IOP.mStart = " << cqap_IOP.mStart
    << "  cqap_IOP.mCount = " << cqap_IOP.mCount
    << EndLogLine;

  _BGL_TorusPktHdr   BGLTorus_Hdr;
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,         
                   0,          
                   (Bit32) ComputeFFTsInQueue_FWD_Z,      
                   0,                   
                   0,                   
                   _BGL_TORUS_ACTOR_BUFFERED );       
  
  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );  
  
  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After  VirtualFifo_BGLTorusSendPacketUnaligned"
    << EndLogLine;

  ComputeFFTsInQueueActorPacket cqap_CP;
  cqap_CP.mStart  = 0;
  cqap_CP.mCount  = mTotalFFTsRecv_z / 2;
  cqap_CP.ThisPtr = mStaticThisPtr;

  BegLogLine(0)
    << "  cqap_CP.mStart = " << cqap_CP.mStart
    << "  cqap_CP.mCount = " << cqap_CP.mCount
    << EndLogLine;


  TR_Sender_Compute_Z_Start.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Z_Start",
                                   0,
                                   TR_Sender_Compute_Z_Start );

  ComputeFFTsInQueue_FWD_Z( (void *) &cqap_CP );

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After  ComputeFFTsInQueue_Z"
    << EndLogLine;

  TR_Sender_Compute_Z_Finis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Z_Finis",
                                   0,
                                   TR_Sender_Compute_Z_Finis );


  ActiveMsgBarrier( mIOP_FFT_Compute_Done_z, 1, mZYXVol2ZyxPhaseData_R );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_Z Active Message Handlers "
    << EndLogLine;

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " Before SendPackets_Y Sends "
    << EndLogLine;

  TR_SendPackets_YStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_YStart", 
                               Platform::Thread::GetId(),
                               TR_SendPackets_YStart );

  SendPackets_FWD_Y();

  TR_SendPackets_YFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_YFinis", 
                               Platform::Thread::GetId(),
                               TR_SendPackets_YFinis );

  ActiveMsgBarrier( mCompleteFFTs_y, mTotalFFTsRecv_y, mZyx2YxzPhaseData_R );

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After   ActiveMsgBarrier( mCompleteFFTs_y, mTotalFFTsRecv_y, mZyx2YxzPhaseData_R );"
    << EndLogLine;

  /*************************************************************
   *   Trigger Computation in Y
   ************************************************************/
  cqap_IOP.mStart  = mTotalFFTsRecv_y / 2;
  cqap_IOP.mCount  = mTotalFFTsRecv_y - cqap_IOP.mStart;
  cqap_IOP.ThisPtr = mStaticThisPtr;
  
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,         
                   0,          
                   (Bit32) ComputeFFTsInQueue_FWD_Y,
                   0,                   
                   0,                   
                   _BGL_TORUS_ACTOR_BUFFERED );       
  
  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );  

  cqap_CP.mStart  = 0;
  cqap_CP.mCount  = mTotalFFTsRecv_y / 2;
  cqap_CP.ThisPtr = mStaticThisPtr;
  
  TR_Sender_Compute_Y_Start.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Y_Start",
                                   0,
                                   TR_Sender_Compute_Y_Start );

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " Before    ComputeFFTsInQueue_FWD_Y( (void *) &cqap_CP )"
    << EndLogLine;
  
  ComputeFFTsInQueue_FWD_Y( (void *) &cqap_CP );

  TR_Sender_Compute_Y_Finis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Y_Finis",
                                   0,
                                   TR_Sender_Compute_Y_Finis );


  ActiveMsgBarrier( mIOP_FFT_Compute_Done_y, 1, mZyx2YxzPhaseData_R );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_Y Sends "
    << EndLogLine;


  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  TR_SendPackets_XStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_XStart", 
                               Platform::Thread::GetId(),
                               TR_SendPackets_XStart );

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " Before SendPackets_X Sends "
    << EndLogLine;

  SendPackets_FWD_X();


  TR_SendPackets_XFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_XFinis", 
                               Platform::Thread::GetId(),
                               TR_SendPackets_XFinis );

  ActiveMsgBarrier( mCompleteFFTs_x, mTotalFFTsRecv_x, mYxz2XyzPhaseData_R );


  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After ActiveMsgBarrier( mCompleteFFTs_x, mTotalFFTsRecv_x, mYxz2XyzPhaseData_R );"
    << EndLogLine;
  
  /*************************************************************
   *   Trigger Computation in X
   ************************************************************/
  cqap_IOP.mStart = mTotalFFTsRecv_x / 2;
  cqap_IOP.mCount = mTotalFFTsRecv_x - cqap_IOP.mStart;
  cqap_IOP.ThisPtr = mStaticThisPtr;
  
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,         
                   0,          
                   (Bit32) ComputeFFTsInQueue_FWD_X,
                   0,                   
                   0,                   
                   _BGL_TORUS_ACTOR_BUFFERED );          

  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );  
  
  cqap_CP.mStart = 0;
  cqap_CP.mCount = mTotalFFTsRecv_x / 2;
  cqap_CP.ThisPtr = mStaticThisPtr;
  
  TR_Sender_Compute_X_Start.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_X_Start",
                                   0,
                                   TR_Sender_Compute_X_Start );

  ComputeFFTsInQueue_FWD_X( (void *) &cqap_CP );

  TR_Sender_Compute_X_Finis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_X_Finis",
                                   0,
                                   TR_Sender_Compute_X_Finis );
    
  ActiveMsgBarrier( mIOP_FFT_Compute_Done_x, 1, mYxz2XyzPhaseData_R );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_X Active Message Handlers "
    << EndLogLine;

#ifdef FFT3D_BARRIER_PHASES
  BGLPartitionBarrier();
#endif

  fftForwardTracerFinis.HitOE(  PKTRACE_FORWARD_FFT,
                                "ForwardFFT", 
                                Platform::Thread::GetId(),
                                fftForwardTracerFinis ); 

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << "All nodes finished and passed barrier."
    << EndLogLine;

}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::RunReverse()
{
  fftReverseTracerStart.HitOE(  PKTRACE_REVERSE_FFT,
                                "ReverseFFT", 
                                Platform::Thread::GetId(),
                                fftReverseTracerStart ); 
  
    BegLogLine( PKFXLOG_INIT_E )
        << "FFT::DoFFT:: Start"
        << EndLogLine;

  int myRank = BGLPartitionGetRankXYZ();

  /*************************************************************
   *   Trigger Computation in X
   ************************************************************/
  ComputeFFTsInQueueActorPacket cqap_IOP;
  cqap_IOP.mStart = mTotalFFTsRecv_x / 2;
  cqap_IOP.mCount = mTotalFFTsRecv_x - cqap_IOP.mStart;
  cqap_IOP.ThisPtr = mStaticThisPtr;
  
  _BGL_TorusPktHdr   BGLTorus_Hdr;
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,         
                   0,          
                   (Bit32) ComputeFFTsInQueue_REV_X,      
                   0,                   
                   0,                   
                   _BGL_TORUS_ACTOR_BUFFERED );       
  
  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );  
  
  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After  VirtualFifo_BGLTorusSendPacketUnaligned"
    << EndLogLine;

  ComputeFFTsInQueueActorPacket cqap_CP;
  cqap_CP.mStart  = 0;
  cqap_CP.mCount  = mTotalFFTsRecv_x / 2;
  cqap_CP.ThisPtr = mStaticThisPtr;

  ComputeFFTsInQueue_REV_X( (void *) &cqap_CP );

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After  ComputeFFTsInQueue_X"
    << EndLogLine;

  ActiveMsgBarrier( mIOP_FFT_Compute_Done_x, 1, NULL );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_X Active Message Handlers "
    << EndLogLine;

  SendPackets_REV_Y();
    
  // The IO core is receiving data. When all the data is here start 
  // computing the 1D FFTs
  // FUTURE:: Optimize to start computing FFTs as soon as you can
  ActiveMsgBarrier( mCompleteFFTs_y, mTotalFFTsRecv_y, NULL );

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After ActiveMsgBarrier "
    << EndLogLine;


  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  /*************************************************************
   *   Trigger Computation in Y
   ************************************************************/
  cqap_IOP.mStart  = mTotalFFTsRecv_y / 2;
  cqap_IOP.mCount  = mTotalFFTsRecv_y - cqap_IOP.mStart;
  cqap_IOP.ThisPtr = mStaticThisPtr;
  
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,         
                   0,          
                   (Bit32) ComputeFFTsInQueue_REV_Y,
                   0,                   
                   0,                   
                   _BGL_TORUS_ACTOR_BUFFERED );       
  
  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );  

  cqap_CP.mStart  = 0;
  cqap_CP.mCount  = mTotalFFTsRecv_y / 2;
  cqap_CP.ThisPtr = mStaticThisPtr;
  
  ComputeFFTsInQueue_REV_Y( (void *) &cqap_CP );

  ActiveMsgBarrier( mIOP_FFT_Compute_Done_y, 1, NULL );
  /*************************************************************/
  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_Y Sends "
    << EndLogLine;

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " Before SendPackets_REV_Y Sends "
    << EndLogLine;

  SendPackets_REV_Z();

  ActiveMsgBarrier( mCompleteFFTs_z, mTotalFFTsRecv_z, NULL );
  
  /*************************************************************
   *   Trigger Computation in Z
   ************************************************************/
  cqap_IOP.mStart = mTotalFFTsRecv_z / 2;
  cqap_IOP.mCount = mTotalFFTsRecv_z - cqap_IOP.mStart;
  cqap_IOP.ThisPtr = mStaticThisPtr;
  
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,         
                   0,          
                   (Bit32) ComputeFFTsInQueue_REV_Z,
                   0,                   
                   0,                   
                   _BGL_TORUS_ACTOR_BUFFERED );       
  
  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );  
  
  cqap_CP.mStart = 0;
  cqap_CP.mCount = mTotalFFTsRecv_z / 2;
  cqap_CP.ThisPtr = mStaticThisPtr;
  
  ComputeFFTsInQueue_REV_Z( (void *) &cqap_CP );
    
  ActiveMsgBarrier( mIOP_FFT_Compute_Done_z, 1, NULL );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_Z Active Message Handlers "
    << EndLogLine;

  SendPackets_REV_XYZVol();
  
  ActiveMsgBarrier( mCompleteXYZVol, 1, NULL );
  
  // BGLPartitionBarrier();
  Reset();
  
  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << "All nodes finished and passed barrier."
    << EndLogLine;
  
  fftReverseTracerFinis.HitOE(  PKTRACE_REVERSE_FFT,
                                "ReverseFFT", 
                                Platform::Thread::GetId(),
                                fftReverseTracerFinis ); 
}

template< class FFT_PLAN >
inline void FFT3D< FFT_PLAN >::
ZeroRealSpace()
  {
    
  }

template< class FFT_PLAN >
inline void FFT3D< FFT_PLAN >::
PutRealSpaceElement( int x, int y, int z, double value)
  {
#ifndef TEST_FFT_DATA_MOVEMENT
    Value * elem = & mZYXVol2ZyxPhaseData_S->xyz( x, y, z ); 
    elem->re = value;
    elem->im = 0.0;
#endif
  }

template< class FFT_PLAN >
inline double FFT3D< FFT_PLAN >::
GetRealSpaceElement( int x, int y, int z )
  {
#ifndef TEST_FFT_DATA_MOVEMENT
    Value * elem = &xmZyx2XYZVolPhaseData_R->xyz( x, y, z );    
    return elem->re;
#else
    return 0.0;
#endif
  }

template< class FFT_PLAN >
inline void FFT3D< FFT_PLAN >::
PutRecipSpaceElement( int kx, int ky, int kz, Value value )
  {
  }

template< class FFT_PLAN >
inline Value FFT3D< FFT_PLAN >::
GetRecipSpaceElement( int kx, int ky, int kz )
  {
  }

template< class FFT_PLAN >
FFT3D< FFT_PLAN >::
~FFT3D()
  {
  }

template< class FFT_PLAN >
inline void FFT3D< FFT_PLAN >::
GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
                    int& aLocalY, int& aLocalSizeY,
                    int& aLocalZ, int& aLocalSizeZ,
                    int& aLocalRecipX, int& aLocalRecipSizeX,
                    int& aLocalRecipY, int& aLocalRecipSizeY,
                    int& aLocalRecipZ, int& aLocalRecipSizeZ )
  {

  aLocalX = mMyP_x * mDataContainer1.DNX();
  aLocalY = mMyP_y * mDataContainer1.DNY();
  aLocalZ = mMyP_z * mDataContainer1.DNZ();

  aLocalSizeX = mDataContainer1.DNX();
  aLocalSizeY = mDataContainer1.DNY();
  aLocalSizeZ = mDataContainer1.DNZ();
  
//   aLocalRecipX = mMyP_x * LOCAL_N_X;
//   aLocalRecipY = mMyP_y * LOCAL_N_Y;
//   aLocalRecipZ = mMyP_z * LOCAL_N_Z;

//   aLocalRecipSizeX = LOCAL_N_X;
//   aLocalRecipSizeY = LOCAL_N_Y;
//   aLocalRecipSizeZ = LOCAL_N_Z;
  }

template< class FFT_PLAN >
inline void FFT3D< FFT_PLAN >::
ScaleIn( double aScaleFactor )
  {
  }
#endif
