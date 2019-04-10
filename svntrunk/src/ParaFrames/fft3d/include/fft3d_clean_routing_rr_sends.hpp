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

//#include <assert.h>
//#include "math.h"
//#include <stdlib.h>
//#include <unistd.h>

#ifndef PKFXLOG_LINEAR
#define PKFXLOG_LINEAR ( 0 )
#endif

#ifndef PKFXLOG_LINEAR
#define PKFXLOG_QUADRATIC ( 0 )
#endif

#ifndef PKFXLOG_LINEAR
#define PKFXLOG_CUBIC ( 0 )
#endif

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
#define PKTRACE_TUNE ( 0 )
#endif

#ifndef PKTRACE_FORWARD_FFT 
#define PKTRACE_FORWARD_FFT ( 1 )
#endif

#ifndef PKTRACE_REVERSE_FFT 
#define PKTRACE_REVERSE_FFT ( 1 )
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
#define PKTRACE_OFF ( 0 )
#endif

#ifndef FFT3D_STUCT_ALIGNMENT
#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))
#endif

#define FFT3D_BARRIER_PHASES 1

//#define FORCE_SELF_SEND_HINT_BITS_Y

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

static TraceClient Full_Send_Queue_State_Start;
static TraceClient Full_Send_Queue_State_Finis;


#define max(i, j) ( (i>j) ? i:j )
#define min(i, j) ( (i<j) ? i:j )

#define END_OF_PACKET 0xabdcefed

// alignment of 8 is double, 16 is quad, 2**5=32 BGL cache, 2**6=64 BGL L3


extern void VirtualFifo_BGLTorusSendPacketUnaligned( _BGL_TorusPktHdr *hdr,   
                                                     void             *data,  
                                                     int               bytes );

void memcpy( char* t, char* s, unsigned int numBytes)
{  
  for(int i=0; i < numBytes; i++)
    {
      t[ i ] = s[ i ];
    }
}
  
inline
void
double_copy( double* t, double* s, unsigned int numDoubles)
{
//   int i ;               
//   for(i=0; i < numDoubles-3; i+=4)
//     {
//       double s0 = s[i] ; 
//       double s1 = s[i+1] ; 
//       double s2 = s[i+2] ; 
//       double s3 = s[i+3] ; 
      
//       t[ i ] = s0;
//       t[ i + 1 ] = s1;
//       t[ i + 2 ] = s2;
//       t[ i + 3 ] = s3;
//     }

  BegLogLine( 0 )
    << "double_copy()::  "
    << "t=" << (void *) t
    << "s=" << (void *) s
    << "numDoubles=" << numDoubles
    << EndLogLine;

  for (int i=0;i<numDoubles;i+=1)
    {
      t[i] = s[i];
    }
}


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

      rle   = mRouter->rle( destX, destY, destZ );
      iter = mRouter->begin( destX, destY, destZ );

      SizeOfFFTHdrAndOneElement = sizeof( FFT_Hdr ) + sizeof( Value );
      PacketCapacity = sizeof( _BGL_TorusPktPayload ) - sizeof( EndOfPacket );
      mEndOfPacket.mValue = END_OF_PACKET;
    }  
    
  void GetNextPacket( ActorPacket* aPacketOut, int* SizeOfPacket )
    {
      BegLogLine( PKFXLOG_PACKET_PACKER )
        << "GetNextPacket():: entering"
        << EndLogLine;

      int      CurrentOffsetInPacket = 0;
      int      PacketSpaceLeft       = PacketCapacity;
 
      // Iterate over the linked list and fill the packets
      while( rle != NULL )
        {
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
              memcpy( & aPacketOut->mData[ CurrentOffsetInPacket ],  &mSkratchFFTHeader, sizeof( FFT_Hdr ));
              CurrentOffsetInPacket += sizeof( FFT_Hdr );
              
              for( int eleI=0; eleI < mSkratchFFTHeader.count; eleI++ )
                {
                  memcpy( &aPacketOut->mData[ CurrentOffsetInPacket ], &(*iter), sizeof( Value ) );
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
                  memcpy( & aPacketOut->mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );                
                  CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                  
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
                  memcpy( &aPacketOut->mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );
                  CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        

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
    
    struct BGLTorus_FFT_Headers
      {
      _BGL_TorusPktHdr mBGLTorus_Hdr;
      FFT_Hdr          mFFT_Hdr;
      } FFT3D_STUCT_ALIGNMENT;
    
    struct ComputeFFTsInQueueActorPacket
    {
      int           mStart;
      int           mCount;
      FFT3D         **ThisPtr;      
    } FFT3D_STUCT_ALIGNMENT;

    EndOfPacket mEndOfPacket FFT3D_STUCT_ALIGNMENT;

    ActorPacket mPackets[ 4 ] FFT3D_STUCT_ALIGNMENT;
    int         mRetSizes[ 4 ]  FFT3D_STUCT_ALIGNMENT;
    
    ActorPacket xyz_ap   FFT3D_STUCT_ALIGNMENT;

    int* mSentToTask;
    int  mNumberOfNodes;

    // ActorPacket xap FFT3D_STUCT_ALIGNMENT;
    // ActorPacket yap FFT3D_STUCT_ALIGNMENT;

    FFT3D** mStaticThisPtr FFT3D_STUCT_ALIGNMENT;

    int mTotalFFTsRecv_x;
    int mTotalFFTsRecv_y;
    int mTotalFFTsRecv_z;

    static int PacketActorFx_FWD_X( void* pkt );
    static int PacketActorFx_FWD_Y( void* pkt );
    static int PacketActorFx_FWD_Z( void* pkt );

    static int PacketActorFx_REV_2Y( void* pkt );
    static int PacketActorFx_REV_2Z( void* pkt );
    static int PacketActorFx_REV_2XYZ( void* pkt );

    static int ComputeFFTsInQueue_FWD_X( void* pkt );
    static int ComputeFFTsInQueue_FWD_Y( void* pkt );
    static int ComputeFFTsInQueue_FWD_Z( void* pkt );    

    static int ComputeFFTsInQueue_REV_X( void* pkt );
    static int ComputeFFTsInQueue_REV_Y( void* pkt );
    static int ComputeFFTsInQueue_REV_Z( void* pkt );    

    void SendPackets_FWD_Z();
    void SendPackets_FWD_Y();
    void SendPackets_FWD_X();

    void SendPackets_REV_Y();
    void SendPackets_REV_Z();
    void SendPackets_REV_XYZVol();

    // Checksuming the data for debugging.
    void InitHeaders();

    static void ActiveMsgBarrier( FFT_Counter & completePencilsInBarCount,
                                  int   barSize );

  }; 

template<class FFT_PLAN >
int FFT3D< FFT_PLAN >::
ComputeFFTsInQueue_FWD_Z(void * args)
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
      ((*This).mFFT_FWD_z).fftInternal( (*This).mFFT_1D_Queue_Z[ index ],
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
ComputeFFTsInQueue_FWD_X(void * args)
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
      ((*This).mFFT_FWD_x).fftInternal( (*This).mFFT_1D_Queue_X[ index ], 
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
ComputeFFTsInQueue_FWD_Y(void * args)
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
      ((*This).mFFT_FWD_y).fftInternal( (*This).mFFT_1D_Queue_Y[ index ],
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
    << "Init():: "
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
  
  PX = proc_x;
  PY = proc_y;
  PZ = proc_z;
   
  mFFTMesh = new fft::FFTMesh( N_X, N_Y, N_Z );

  mProcMesh = new fft::ProcMesh( PX, PY, PZ );
  
  int dimX, dimY, dimZ;
  BGLPartitionGetDimensions( &dimX, &dimY, &dimZ );
  mNumberOfNodes = dimX * dimY * dimZ;

  mSentToTask = (int *) pkMalloc( sizeof(int) * mNumberOfNodes );

  mDataContainer1.init( *mFFTMesh, *mProcMesh );
  mDataContainer2.init( *mFFTMesh, *mProcMesh );

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
  mSendInZRouterRev.init( *mYzx2ZyxPhaseData_S,    mMyP_x, mMyP_y, mMyP_z, 0 );
  mSendInXYZVolRouterRev.init( *mZyx2XYZVolPhaseData_S, mMyP_x, mMyP_y, mMyP_z, 0 );
   
  mTotalRecvData_x = (int *) malloc( sizeof(int) * mDataContainer1.DYZ() );
  mTotalRecvData_y = (int *) malloc( sizeof(int) * mDataContainer1.DXZ() );
  mTotalRecvData_z = (int *) malloc( sizeof(int) * mDataContainer1.DXY() );
    
  mFFT_1D_Queue_X = (Value **) malloc( sizeof(Value *) * mDataContainer1.DYZ() );
  mFFT_1D_Queue_Y = (Value **) malloc( sizeof(Value *) * mDataContainer1.DXZ() );
  mFFT_1D_Queue_Z = (Value **) malloc( sizeof(Value *) * mDataContainer1.DXY() );

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
  

  // InitHeaders();

  Reset();      
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
                   count * 2 );

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
        << "PacketActor FWD Z:: "
        << " mTotalRecvData_z[ " << fftIndex << "] = " << (*This).mTotalRecvData_z[ fftIndex ]
        << EndLogLine;      

      if(  (*This).mTotalRecvData_z[ fftIndex ] == 0 ) 
        {      
          
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
              break;
            }          
          
          TR_1DFFT_READY_Z.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,
                                  "TR_1DFFT_READY_Z", 
                                  Platform::Thread::GetId(),
                                  TR_1DFFT_READY_Z );      
        }  
     
      EOM = (EndOfPacket *) &Data[ CurrentOffsetInData ];
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
                   count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_y[ fftIndex ] -= count;  

      if(  (*This).mTotalRecvData_y[ fftIndex ] == 0 ) 
        {      
          // Platform::Memory::ExportFence( (void *) &( (*This).mZyx2YxzPhaseData_R->Yxz( fftIndex, 0 ) ), 
          //                               sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_Y );
          
          (*This).mFFT_1D_Queue_Y[ (*This).mCompleteFFTs_y.mVar ] = & ( (*This).mZyx2YxzPhaseData_R->Yxz( fftIndex, 0 ) );      
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
                   count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_x[ fftIndex ] -= count;  

      if(  (*This).mTotalRecvData_x[ fftIndex ] == 0 ) 
        {      
          //Platform::Memory::ExportFence( (void *) &( (*This).mYxz2XyzPhaseData_R->Xyz( fftIndex, 0 ) ), 
          //                               sizeof( Value ) * FFT_PLAN::GLOBAL_SIZE_X );
          
          (*This).mFFT_1D_Queue_X[ (*This).mCompleteFFTs_x.mVar ] = & ( (*This).mYxz2XyzPhaseData_R->Xyz( fftIndex, 0 ) );      
          // Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_X[ (*This).mCompleteFFTs_x.mVar ], sizeof(Value *) );      
          
          (*This).mCompleteFFTs_x.mVar += 1;
          
          // Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTs_x.mVar), sizeof( FFT_Counter ) );          

          if( (*This).mCompleteFFTs_x.mVar == (*This).mTotalFFTsRecv_x )
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
                   count * 2 );

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
                   count * 2 );

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
                   count * 2 );

      CurrentOffsetInData += sizeof( Value ) * count;
      
      (*This).mTotalRecvData_xyzvol -= count;  

      if(  (*This).mTotalRecvData_xyzvol == 0 ) 
        {              
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
void FFT3D< FFT_PLAN >::
SendPackets_FWD_Z()
{
  BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
    << "Entering SendPackets_Z"
      << EndLogLine;

  // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes

  int PacketsSent = 0;
 
  for( int i=1; i <= PZ / 2; i++ )
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

      mZYXVol2ZyxPP[ 0 ].Init( mMyP_x, mMyP_y, dest1, 
                               (Bit32) PacketActorFx_FWD_Z, &mSendInZRouterFwd, mStaticThisPtr );

      mZYXVol2ZyxPP[ 1 ].Init( mMyP_x, mMyP_y, dest2, 
                               (Bit32) PacketActorFx_FWD_Z, &mSendInZRouterFwd, mStaticThisPtr );

      BegLogLine( PKFXLOG_GATHER_Z )
        << "SendPackets_FWD_Z():: "
        << "Done with Inits"
        << EndLogLine;
      
      int EmptyList1 = 0;
      int EmptyList2 = 0;
      while( 1 )
        {
          if( !EmptyList1 )
            {
              mZYXVol2ZyxPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ] );

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
                                   (Bit32) PacketActorFx_FWD_Z,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                  
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_FWD_Z,
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
              mZYXVol2ZyxPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ] );
              
              if( mRetSizes[ 1 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   mMyP_y,
                                   dest2,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_FWD_Z,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                  
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_FWD_Z,
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
                
      
  BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_Z"
    << EndLogLine;  
  }
}

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
SendPackets_FWD_Y()
  {
  BegLogLine( PKFXLOG_GATHER_Y || PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering SendPackets_Y"
      << EndLogLine;

  // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes

  int PacketsSent = 0;

  for( int i=1; i <= PY / 2; i++ )
    {
      for( int j=1; j <= PZ / 2; j++ )
        {
          
          // In the 2D the 2 destinations are:
          // dest1 = ( i + mMyP_z ) % PZ
          // dest2 = ( 1-i + mMyP_z ) % PZ
          int dest_z1 = ( j + mMyP_z )  % PZ;
          int dest_z2 = ( ( (1-j) + mMyP_z ) + PZ ) % PZ;      
          int dest_y1 = ( i + mMyP_y ) % PY;
          int dest_y2 = ( ( (1-i) + mMyP_y ) + PY ) % PY;      
          
          mZyx2YxzPP[ 0 ].Init( mMyP_x, dest_y1, dest_z1,
                                (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr, 1 );          
          mZyx2YxzPP[ 1 ].Init( mMyP_x, dest_y1, dest_z2,
                                (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr, 1 );
          mZyx2YxzPP[ 2 ].Init( mMyP_x, dest_y2, dest_z1,
                                (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr, 1 );
          mZyx2YxzPP[ 3 ].Init( mMyP_x, dest_y2, dest_z2,
                                (Bit32) PacketActorFx_FWD_Y, &mSendInYRouterFwd, mStaticThisPtr, 1 );
          
          int EmptyList0 = 0;
          int EmptyList1 = 0;
          int EmptyList2 = 0;
          int EmptyList3 = 0;
          
          while( 1 )
            {
              if( !EmptyList0 )
                {
                  mZyx2YxzPP[ 0 ].GetNextPacket( & mPackets[ 0 ], & mRetSizes[ 0 ] );
                  
                  if( mRetSizes[ 0 ] != 0 ) 
                    {              
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       mMyP_x,
                                       dest_y1,
                                       dest_z1,                       // destination coordinates
                                       0,                           // ???      // destination Fifo Group
                                       (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_FWD_Y,
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
              mZyx2YxzPP[ 1 ].GetNextPacket( & mPackets[ 1 ], & mRetSizes[ 1 ] );
              
              if( mRetSizes[ 1 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   dest_y1,
                                   dest_z2,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_FWD_Y,
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
              mZyx2YxzPP[ 2 ].GetNextPacket( & mPackets[ 2 ], & mRetSizes[ 2 ] );
              
              if( mRetSizes[ 2 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   dest_y2,
                                   dest_z1,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_FWD_Y,
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
              mZyx2YxzPP[ 3 ].GetNextPacket( & mPackets[ 3 ], & mRetSizes[ 3 ] );
              
              if( mRetSizes[ 3 ] != 0 ) 
                {              
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   dest_y2,
                                   dest_z2,                       // destination coordinates
                                   0,                           // ???      // destination Fifo Group
                                   (Bit32) PacketActorFx_FWD_Y,     // Active Packet Function matching signature above
                                   0,                           // primary argument to actor
                                   0,                           // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                  Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                     (Bit32) PacketActorFx_FWD_Y,
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

  // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes

  int PacketCapacity = sizeof( _BGL_TorusPktPayload ) - sizeof( EndOfPacket );
  int CurrentOffsetInPacket = 0;
  int PacketSpaceLeft = PacketCapacity;

  int PacketsSent = 0;

  int SizeOfFFTHdrAndOneElement = sizeof( FFT_Hdr ) + sizeof( Value );

  for( int j=0; j < PX; j++ )
    {
      for( int i=0; i < PY; i++ )
        {
          int destPy = ( mMyP_y + i ) % PY ;
          int destPx = ( mMyP_x + j ) % PX ;          

          const fft::RLEPos*   rle  = mSendInXRouterFwd.rle( destPx, destPy, mMyP_z );
          Yxz2XyzIterator iter = mSendInXRouterFwd.begin( destPx, destPy, mMyP_z );
          
          // Iterate over the linked list and fill the packets
          while( rle != NULL )
            {
              int FFTIndex = rle->fftIndex();
              int Offset   = rle->offset();
              int CurrentCount    = rle->count();                             

              mSkratchFFTHeader.ThisPtr  = mStaticThisPtr;
              mSkratchFFTHeader.fftIndex = FFTIndex;
              mSkratchFFTHeader.offset   = Offset;
              int prevCount = 0;

              // Iterate
              while( CurrentCount != 0 )
                {              
                  int BytesOfData = ( sizeof( Value ) * CurrentCount );
              
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
                  memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ],  &mSkratchFFTHeader, sizeof( FFT_Hdr ));
                  CurrentOffsetInPacket += sizeof( FFT_Hdr );

                  for( int eleI=0; eleI < mSkratchFFTHeader.count; eleI++ )
                    {
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &(*iter), sizeof( Value ) );
                      CurrentOffsetInPacket += sizeof( Value );
                      ++iter;
                    }
              
                  PacketSpaceLeft -= ( sizeof( Value ) * mSkratchFFTHeader.count );
              
                  CurrentCount -= mSkratchFFTHeader.count;
                  prevCount = mSkratchFFTHeader.count;
              
                  if( PacketSpaceLeft < SizeOfFFTHdrAndOneElement )
                    {                                    
                      /******************************************************************************
                       *                            Send Packet
                       ******************************************************************************/                  
                      // We've filled a full packed. Time to seal and send 'em out.                  
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );                                      
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        

                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       destPx,
                                       destPy,
                                       mMyP_z,                       // destination coordinates
                                       0,                           // ???      // destination Fifo Group
                                       (Bit32) PacketActorFx_FWD_X,     // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                       Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                          (Bit32) PacketActorFx_FWD_X,
                                                                          destPx,
                                                                          destPy,
                                                                          mMyP_z,
                                                                          CurrentOffsetInPacket,
                                                                          &xyz_ap );

                      int RankToSend = BGLPartitionMakeRank( destPx, destPy, mMyP_z );
                      mSentToTask[ RankToSend ]++;

                      PacketsSent++;                                            
                      /*******************************************************************************/

                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;
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
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                      
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       destPx,
                                       destPy,
                                       mMyP_z,                      // destination coordinates
                                       0,                           // destination Fifo Group
                                       (Bit32) PacketActorFx_FWD_X, // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                       Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                          (Bit32) PacketActorFx_FWD_X,
                                                                          destPx,
                                                                          destPy,
                                                                          mMyP_z,
                                                                          CurrentOffsetInPacket,
                                                                          & xyz_ap);

                      int RankToSend = BGLPartitionMakeRank( destPx, destPy, mMyP_z );
                      mSentToTask[ RankToSend ]++;
                      PacketsSent++;                                            
                      /*******************************************************************************/
                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;
                    }
                  break;
                }
            }
        }
    }
        
  BegLogLine( PKFXLOG_LOAD_BALANCING )
    << "SendPackets_FWD_X"
    << "Packets Sent = " << PacketsSent
    << EndLogLine;

  for( int i=0; i < mNumberOfNodes; i++ )
    {
      BegLogLine( PKFXLOG_LOAD_BALANCING )
        << "mSentToTask_X[ " << i << " ] = " << mSentToTask[ i ]
        << EndLogLine;

      mSentToTask[ i ] = 0;
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

  // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes

  int PacketCapacity = sizeof( _BGL_TorusPktPayload ) - sizeof( EndOfPacket );
  int CurrentOffsetInPacket = 0;
  int PacketSpaceLeft = PacketCapacity;

  int SizeOfFFTHdrAndOneElement = sizeof( FFT_Hdr ) + sizeof( Value );

  for( int j=0; j < PX; j++ )
    {
      for( int i=0; i < PY; i++ )
        {
          int destPy = ( mMyP_y + i ) % PY ;
          int destPx = ( mMyP_x + j ) % PX ;          

          const fft::RLEPos*   rle  = mSendInYRouterRev.rle( destPx, destPy, mMyP_z );
          Xyz2YzxIterator iter = mSendInYRouterRev.begin( destPx, destPy, mMyP_z );
          
          // Iterate over the linked list and fill the packets
          while( rle != NULL )
            {
              int FFTIndex = rle->fftIndex();
              int Offset   = rle->offset();
              int CurrentCount    = rle->count(); 
              
              mSkratchFFTHeader.ThisPtr  = mStaticThisPtr;
              mSkratchFFTHeader.fftIndex = FFTIndex;
              mSkratchFFTHeader.offset   = Offset;
              int prevCount = 0;
          
              // Iterate
              while( CurrentCount != 0 )
                {              
                  int BytesOfData = ( sizeof( Value ) * CurrentCount );
              
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
                  memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ],  &mSkratchFFTHeader, sizeof( FFT_Hdr ));
                  CurrentOffsetInPacket += sizeof( FFT_Hdr );

                  for( int eleI=0; eleI < mSkratchFFTHeader.count; eleI++ )
                    {
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &(*iter), sizeof( Value ) );
                      CurrentOffsetInPacket += sizeof( Value );
                      ++iter;
                    }
              
                  PacketSpaceLeft -= ( sizeof( Value ) * mSkratchFFTHeader.count );
              
                  CurrentCount -= mSkratchFFTHeader.count;
                  prevCount = mSkratchFFTHeader.count;                  
              
                  if( PacketSpaceLeft < SizeOfFFTHdrAndOneElement )
                    {                                    
                      /******************************************************************************
                       *                            Send Packet
                       ******************************************************************************/                  
                      // We've filled a full packed. Time to seal and send 'em out.                  
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );                
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                                            
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       destPx,
                                       destPy,
                                       mMyP_z,                       // destination coordinates
                                       0,                           // ???      // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2Y,     // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2Y,
                                                                         destPx,
                                                                         destPy,
                                                                         mMyP_z,
                                                                         CurrentOffsetInPacket,
                                                                         &xyz_ap );
                      /*******************************************************************************/
                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;
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
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                      
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       destPx,
                                       destPy,
                                       mMyP_z,                      // destination coordinates
                                       0,                           // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2Y, // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2Y,
                                                                         destPx,
                                                                         destPy,
                                                                         mMyP_z,
                                                                         CurrentOffsetInPacket,
                                                                         & xyz_ap);
                      /*******************************************************************************/
                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;
                    }
                  break;
                }
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

  // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes

  int PacketCapacity = sizeof( _BGL_TorusPktPayload ) - sizeof( EndOfPacket );
  int CurrentOffsetInPacket = 0;
  int PacketSpaceLeft = PacketCapacity;

  int SizeOfFFTHdrAndOneElement = sizeof( FFT_Hdr ) + sizeof( Value );

  for( int i=0; i < PY; i++ )
    {
      for( int j=0; j < PZ; j++ )
        {
          int destPy = ( mMyP_y + i ) % PY ;
          int destPz = ( mMyP_z + j ) % PZ ;          

          const fft::RLEPos*   rle  = mSendInZRouterRev.rle( mMyP_x, destPy, destPz );
          Yzx2ZyxIterator iter = mSendInZRouterRev.begin( mMyP_x, destPy, destPz );
          
          // Iterate over the linked list and fill the packets
          while( rle != NULL )
            {
              int FFTIndex = rle->fftIndex();
              int Offset   = rle->offset();
              int CurrentCount    = rle->count(); 
              
              mSkratchFFTHeader.ThisPtr  = mStaticThisPtr;
              mSkratchFFTHeader.fftIndex = FFTIndex;
              mSkratchFFTHeader.offset   = Offset;
              int prevCount = 0;
          
              // Iterate
              while( CurrentCount != 0 )
                {              
                  int BytesOfData = ( sizeof( Value ) * CurrentCount );
              
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
                  memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ],  &mSkratchFFTHeader, sizeof( FFT_Hdr ));
                  CurrentOffsetInPacket += sizeof( FFT_Hdr );

                  for( int eleI=0; eleI < mSkratchFFTHeader.count; eleI++ )
                    {
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &(*iter), sizeof( Value ) );
                      CurrentOffsetInPacket += sizeof( Value );
                      ++iter;
                    }
              
                  PacketSpaceLeft -= ( sizeof( Value ) * mSkratchFFTHeader.count );
              
                  CurrentCount -= mSkratchFFTHeader.count;
                  prevCount = mSkratchFFTHeader.count;              

                  if( PacketSpaceLeft < SizeOfFFTHdrAndOneElement )
                    {                                    
                      /******************************************************************************
                       *                            Send Packet
                       ******************************************************************************/                  
                      // We've filled a full packed. Time to seal and send 'em out.                  
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );                
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                      
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       mMyP_x,
                                       destPy,
                                       destPz,
                                       0,                           // ???      // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2Z,     // Active Packet Function matching signature above
                                       0,                           // primary argument to actor
                                       0,                           // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2Z,
                                                                         mMyP_x,
                                                                         destPy,
                                                                         destPz,
                                                                         CurrentOffsetInPacket,
                                                                         &xyz_ap );
                      /*******************************************************************************/

                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;
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
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                     
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       mMyP_x,
                                       destPy,
                                       destPz,
                                       0,                            // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2Z, // Active Packet Function matching signature above
                                       0,                            // primary argument to actor
                                       0,                            // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED );  // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2Z,
                                                                         mMyP_x,
                                                                         destPy,
                                                                         destPz,
                                                                         CurrentOffsetInPacket,
                                                                         & xyz_ap);
                      /*******************************************************************************/
                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;
                    }
                  break;
                }
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

  // Iterate over the ZYXVol2ZyxRouter and pick out all the data points     
  
  // The first phase has the data going to each node in the same way.
  // Hence the linked list of dest(fftIndex, offset, count) is the same on all nodes

  int PacketCapacity = sizeof( _BGL_TorusPktPayload ) - sizeof( EndOfPacket );
  int CurrentOffsetInPacket = 0;
  int PacketSpaceLeft = PacketCapacity;

  int SizeOfFFTHdrAndOneElement = sizeof( FFT_Hdr ) + sizeof( Value );

  for( int i=0; i < PZ; i++ )
    {
      int destPz = ( mMyP_z + i ) % PZ ;

      const fft::RLEPos*      rle  = mSendInXYZVolRouterRev.rle( mMyP_x, mMyP_y, destPz );
      Zyx2ZyxVolIterator iter = mSendInXYZVolRouterRev.begin( mMyP_x, mMyP_y, destPz );
      
      // Iterate over the linked list and fill the packets
      while( rle != NULL )
        {
          int FFTIndex = rle->fftIndex();
          int Offset   = rle->offset();
          int CurrentCount    = rle->count(); 
          
          mSkratchFFTHeader.ThisPtr  = mStaticThisPtr;
          mSkratchFFTHeader.fftIndex = FFTIndex;
          mSkratchFFTHeader.offset   = Offset;
          int prevCount = 0;
          
          // Iterate
          while( CurrentCount != 0 )
            {              
              int BytesOfData = ( sizeof( Value ) * CurrentCount );
              
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
                  memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ],  &mSkratchFFTHeader, sizeof( FFT_Hdr ));
                  CurrentOffsetInPacket += sizeof( FFT_Hdr );

                  for( int eleI=0; eleI < mSkratchFFTHeader.count; eleI++ )
                    {
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &(*iter), sizeof( Value ) );
                      CurrentOffsetInPacket += sizeof( Value );
                      ++iter;
                    }
              
                  PacketSpaceLeft -= ( sizeof( Value ) * mSkratchFFTHeader.count );
              
                  CurrentCount -= mSkratchFFTHeader.count;
                  prevCount = mSkratchFFTHeader.count;
              
                  if( PacketSpaceLeft < SizeOfFFTHdrAndOneElement )
                    {                                    
                      /******************************************************************************
                       *                            Send Packet
                       ******************************************************************************/                  
                      // We've filled a full packed. Time to seal and send 'em out.                  
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );                
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        
                      
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       mMyP_x,
                                       mMyP_y,
                                       destPz,
                                       0,                              // ???      // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2XYZ, // Active Packet Function matching signature above
                                       0,                              // primary argument to actor
                                       0,                              // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED );    // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2XYZ,
                                                                         mMyP_x,
                                                                         mMyP_y,
                                                                         destPz,
                                                                         CurrentOffsetInPacket,
                                                                         &xyz_ap );
                      /*******************************************************************************/

                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;
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
                      memcpy( &xyz_ap.mData[ CurrentOffsetInPacket ], &mEndOfPacket, sizeof( EndOfPacket ) );
                      CurrentOffsetInPacket += sizeof( EndOfPacket );                                                        

                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       mMyP_x,
                                       mMyP_y,
                                       destPz,
                                       0,                            // destination Fifo Group
                                       (Bit32) PacketActorFx_REV_2XYZ, // Active Packet Function matching signature above
                                       0,                            // primary argument to actor
                                       0,                            // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED );  // Actor Signature from above (must match Actor)
                      
                      Platform::Reactor::BGLTorusSimpleActivePacketSend( BGLTorus_Hdr,
                                                                         (Bit32) PacketActorFx_REV_2XYZ,
                                                                         mMyP_x,
                                                                         mMyP_y,
                                                                         destPz,
                                                                         CurrentOffsetInPacket,
                                                                         & xyz_ap);
                      /*******************************************************************************/
                      CurrentOffsetInPacket = 0;
                      PacketSpaceLeft = PacketCapacity;                      
                    }
                  break;
                }
        }
    }  

  BegLogLine( PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_REV_XYZVol"
    << EndLogLine;  
  }

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::
ActiveMsgBarrier(FFT_Counter & aCurrentValue,
                 int           aTargetValue )
  {

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
  
  Platform::Memory::ImportFence();
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

  mTotalPointsRecv_x.mVar = N_X * mDataContainer1.DYZ();
  mTotalPointsRecv_y.mVar = N_Y * mDataContainer1.DXZ();
  mTotalPointsRecv_z.mVar = N_Z * mDataContainer1.DXY();

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
  BGLPartitionBarrier();
  };

template<class FFT_PLAN >
void FFT3D< FFT_PLAN >::RunForward()
{  
  fftForwardTracerStart.HitOE(  PKTRACE_FORWARD_FFT,
                                "ForwardFFT", 
                                Platform::Thread::GetId(),
                                fftForwardTracerStart ); 
  
  BGLPartitionBarrier();

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
  ActiveMsgBarrier( mCompleteFFTs_z, mTotalFFTsRecv_z );

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


  ActiveMsgBarrier( mIOP_FFT_Compute_Done_z, 1 );
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

  ActiveMsgBarrier( mCompleteFFTs_y, mTotalFFTsRecv_y );

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

  ComputeFFTsInQueue_FWD_Y( (void *) &cqap_CP );

  TR_Sender_Compute_Y_Finis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Y_Finis",
                                   0,
                                   TR_Sender_Compute_Y_Finis );


  ActiveMsgBarrier( mIOP_FFT_Compute_Done_y, 1 );
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

  ActiveMsgBarrier( mCompleteFFTs_x, mTotalFFTsRecv_x );
  
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
    
  ActiveMsgBarrier( mIOP_FFT_Compute_Done_x, 1 );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_X Active Message Handlers "
    << EndLogLine;


  // BGLPartitionBarrier();
  Reset();

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << "All nodes finished and passed barrier."
    << EndLogLine;

  fftForwardTracerFinis.HitOE(  PKTRACE_FORWARD_FFT,
                                 "ForwardFFT", 
                                 Platform::Thread::GetId(),
                                 fftForwardTracerFinis ); 
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

  ActiveMsgBarrier( mIOP_FFT_Compute_Done_x, 1 );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_X Active Message Handlers "
    << EndLogLine;

  SendPackets_REV_Y();
    
  // The IO core is receiving data. When all the data is here start 
  // computing the 1D FFTs
  // FUTURE:: Optimize to start computing FFTs as soon as you can
  ActiveMsgBarrier( mCompleteFFTs_y, mTotalFFTsRecv_y );

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

  ActiveMsgBarrier( mIOP_FFT_Compute_Done_y, 1 );
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

  ActiveMsgBarrier( mCompleteFFTs_z, mTotalFFTsRecv_z );
  
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
    
  ActiveMsgBarrier( mIOP_FFT_Compute_Done_z, 1 );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << " After SendPackets_Z Active Message Handlers "
    << EndLogLine;

  SendPackets_REV_XYZVol();
  
  ActiveMsgBarrier( mCompleteXYZVol, 1 );
  
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
