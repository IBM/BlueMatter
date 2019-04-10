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

//#ifndef PK_BLADE
  #include <MPI/fft_one_dim.hpp>
  // #include <BlueMatter/pk/platform.hpp>
  #include <MPI/PacketCounterManager.hpp>
// #include <pk/fxlogger.hpp>
// #else
//   #include "blade.h"
//   #include "fft_one_dim.hpp"
//   #include "pk/platform.hpp"
//   #include "pk/fxlogger.hpp"
//   #include "PacketCounterManager.hpp"

#ifdef USE_FFTW_ONE_DIM
#include "fftw.h"
#endif

#include <assert.h>
#include "math.h"
#include <stdlib.h>
#include <unistd.h>

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

#ifndef PKTRACE_TUNE_1D_FFT_Y
#define PKTRACE_TUNE_1D_FFT_Y ( 1 )
#endif

#ifndef PKTRACE_TUNE_1D_FFT_X
#define PKTRACE_TUNE_1D_FFT_X ( 1 )
#endif


#ifndef PKFXLOG_GATHER_Z_TRIM
#define PKFXLOG_GATHER_Z_TRIM ( 0 )
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

#ifndef FFT3D_STUCT_ALIGNMENT
#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))
#endif

//#define FORCE_SELF_SEND_HINT_BITS_Y


TraceClient TR_SendPackets_ZStart;
TraceClient TR_SendPackets_ZFinis;

TraceClient TR_SendPackets_YStart;
TraceClient TR_SendPackets_YFinis;

TraceClient TR_SendPackets_XStart;
TraceClient TR_SendPackets_XFinis;

TraceClient TR_SendPackets_BStart;
TraceClient TR_SendPackets_BFinis;

TraceClient TR_FFT_1D_ZStart;
TraceClient TR_FFT_1D_ZFinis;

TraceClient TR_FFT_1D_YStart;
TraceClient TR_FFT_1D_YFinis;

TraceClient TR_FFT_1D_XStart;
TraceClient TR_FFT_1D_XFinis;

TraceClient TR_1DFFT_READY_X;
TraceClient TR_1DFFT_READY_Y;
TraceClient TR_1DFFT_READY_Z;

TraceClient TR_BGLActivePacketSendStart;
TraceClient TR_BGLActivePacketSendFinis;

TraceClient TR_Sender_Compute_Z_Start;
TraceClient TR_Sender_Compute_Z_Finis;

TraceClient TR_Sender_Compute_Y_Start;
TraceClient TR_Sender_Compute_Y_Finis;

TraceClient TR_Sender_Compute_X_Start;
TraceClient TR_Sender_Compute_X_Finis;

TraceClient Full_Send_Queue_State_Start;
TraceClient Full_Send_Queue_State_Finis;


#define max(i, j) ( (i>j) ? i:j )
#define min(i, j) ( (i<j) ? i:j )

// alignment of 8 is double, 16 is quad, 2**5=32 BGL cache, 2**6=64 BGL L3


extern void VirtualFifo_BGLTorusSendPacketUnaligned( _BGL_TorusPktHdr *hdr,
                                                     void             *data,
                                                     int               bytes );


extern int  BGLTorusSendPacketNonBlocking( _BGL_TorusPktHdr *hdr,       // Previously created header to use
                                           void             *data,      // Source address of data (16byte aligned)
                                           int               bytes );   // Payload bytes = ((chunks * 32) - 16)


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
  int i ;
  for(i=0; i < numDoubles-3; i+=4)
    {
      double s0 = s[i] ;
      double s1 = s[i+1] ;
      double s2 = s[i+2] ;
      double s3 = s[i+3] ;

      t[ i ] = s0;
      t[ i + 1 ] = s1;
      t[ i + 2 ] = s2;
      t[ i + 3 ] = s3;
    }

  for (;i<numDoubles;i+=1)
    {
      t[i] = s[i];
    }
}


//FFT3D_STUCT_ALIGNMENT
template< class FFT_PLAN, int FWD_REV >
class FFT3D
  {
  public:

    typedef FFT_PLAN FFT_PLAN_IF;

    int Init( FFT3D** ptr );
    inline void GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
                                    int& aLocalY, int& aLocalSizeY,
                                    int& aLocalZ, int& aLocalSizeZ,
                                    int& aLocalRecipX, int& aLocalRecipSizeX,
                                    int& aLocalRecipY, int& aLocalRecipSizeY,
                                    int& aLocalRecipZ, int& aLocalSizeRecipZ );

    inline void DoFFT();
    inline void Reset();

    inline void ZeroRealSpace();
    inline double GetRealSpaceElement( int x, int y, int z );
    inline void PutRealSpaceElement( int x, int y, int z, double );
    inline fft3d_complex GetRecipSpaceElement( int kx, int ky, int kz );
    inline void PutRecipSpaceElement( int kx, int ky, int kz, fft3d_complex );

    inline void ScaleIn(double);

    FFT3D()
    {
      // Init();
    }
    // ~FFT3D();


    struct FFT_Hdr
      {
      FFT3D *This;
      int aIndex;
      int bIndex;
      short cIndex;
      short pointsPerPacket;
      } FFT3D_STUCT_ALIGNMENT;

  private:


    typedef fft3d_complex Value;

    enum
      {
      // ELEMENTS_PER_PACKET = ( (sizeof(_BGL_TorusPktPayload ) - sizeof(FFT_Hdr)) / sizeof( Value) ),
      ELEMENTS_PER_PACKET = 8
      // magic number 8 is a work around for some bug that Alex knows about - should be computed max packet size.
      // ELEMENTS_PER_PACKET = 14
      };



public:
    enum { P_X = FFT_PLAN::P_X };
    enum { P_Y = FFT_PLAN::P_Y };
    enum { P_Z = FFT_PLAN::P_Z };

    enum { GLOBAL_N_X = FFT_PLAN::GLOBAL_SIZE_X };
    enum { GLOBAL_N_Y = FFT_PLAN::GLOBAL_SIZE_Y };
    enum { GLOBAL_N_Z = FFT_PLAN::GLOBAL_SIZE_Z };


    enum { LOCAL_N_X = GLOBAL_N_X / P_X };
    enum { LOCAL_N_Y = GLOBAL_N_Y / P_Y };
    enum { LOCAL_N_Z = GLOBAL_N_Z / P_Z };

private:
    enum { BAR_SIZE_X = LOCAL_N_Y / P_X };
    enum { BAR_SIZE_Y = LOCAL_N_Z / P_Y };
    enum { BAR_SIZE_Z = LOCAL_N_X / P_Z };

    enum { TOTAL_FFTs_IN_BAR_Z = BAR_SIZE_Z * LOCAL_N_Y };
    enum { TOTAL_FFTs_IN_BAR_Y = BAR_SIZE_Y * LOCAL_N_X };
    enum { TOTAL_FFTs_IN_BAR_X = BAR_SIZE_X * LOCAL_N_Z };

    enum { POINTS_PER_PACKET_X = min( BAR_SIZE_Z, ELEMENTS_PER_PACKET ) };
    enum { POINTS_PER_PACKET_Y = min( BAR_SIZE_X, ELEMENTS_PER_PACKET ) };
    enum { POINTS_PER_PACKET_Z = min( LOCAL_N_Z, ELEMENTS_PER_PACKET ) };
    enum { POINTS_PER_PACKET_B = min( LOCAL_N_Y, ELEMENTS_PER_PACKET ) };

    int mIsUni FFT3D_STUCT_ALIGNMENT;

    int mCompletePacket_x[ TOTAL_FFTs_IN_BAR_X ] FFT3D_STUCT_ALIGNMENT;
    int mCompletePacket_y[ TOTAL_FFTs_IN_BAR_Y ] FFT3D_STUCT_ALIGNMENT;
    int mCompletePacket_z[ TOTAL_FFTs_IN_BAR_Z ] FFT3D_STUCT_ALIGNMENT;
    int mCompletePacket_b[ LOCAL_N_X ][ LOCAL_N_Z ] FFT3D_STUCT_ALIGNMENT;

    typedef AtomicNative<int> FFT_Counter;

    FFT_Counter mCompleteFFTsInBarCount_b[ LOCAL_N_X ] FFT3D_STUCT_ALIGNMENT;
    FFT_Counter mCompleteFFTsInBarCount_x FFT3D_STUCT_ALIGNMENT;
    FFT_Counter mCompleteFFTsInBarCount_y FFT3D_STUCT_ALIGNMENT;
    FFT_Counter mCompleteFFTsInBarCount_z FFT3D_STUCT_ALIGNMENT;

    FFT_Counter mIOP_FFT_Compute_Done_x FFT3D_STUCT_ALIGNMENT;
    FFT_Counter mIOP_FFT_Compute_Done_y FFT3D_STUCT_ALIGNMENT;
    FFT_Counter mIOP_FFT_Compute_Done_z FFT3D_STUCT_ALIGNMENT;

    typedef FFTOneD< GLOBAL_N_X, FWD_REV > FFT_1D_X;
    typedef FFTOneD< GLOBAL_N_Y, FWD_REV > FFT_1D_Y;
    typedef FFTOneD< GLOBAL_N_Z, FWD_REV > FFT_1D_Z;

  //   // 1D FFTs in each direction

    FFT_1D_X mFFT_x FFT3D_STUCT_ALIGNMENT;
    FFT_1D_Y mFFT_y FFT3D_STUCT_ALIGNMENT;
    FFT_1D_Z mFFT_z FFT3D_STUCT_ALIGNMENT;

  public:
    fft3d_complex mLocalIn [ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ] FFT3D_STUCT_ALIGNMENT;

    fft3d_complex mLocalOut[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ] FFT3D_STUCT_ALIGNMENT;

  private:

    fft3d_complex mGlobalData_x [ TOTAL_FFTs_IN_BAR_X ][ FFT_PLAN::GLOBAL_SIZE_X ] FFT3D_STUCT_ALIGNMENT;
    fft3d_complex mGlobalData_y [ TOTAL_FFTs_IN_BAR_Y ][ FFT_PLAN::GLOBAL_SIZE_Y ] FFT3D_STUCT_ALIGNMENT;
    fft3d_complex mGlobalData_z [ TOTAL_FFTs_IN_BAR_Z ][ FFT_PLAN::GLOBAL_SIZE_Z ] FFT3D_STUCT_ALIGNMENT;

    fft3d_complex* mFFT_1D_Queue_X[ TOTAL_FFTs_IN_BAR_X ] FFT3D_STUCT_ALIGNMENT;
    fft3d_complex* mFFT_1D_Queue_Y[ TOTAL_FFTs_IN_BAR_Y ] FFT3D_STUCT_ALIGNMENT;
    fft3d_complex* mFFT_1D_Queue_Z[ TOTAL_FFTs_IN_BAR_Z ] FFT3D_STUCT_ALIGNMENT;


    int mMyP_x FFT3D_STUCT_ALIGNMENT;
    int mMyP_y FFT3D_STUCT_ALIGNMENT;
    int mMyP_z FFT3D_STUCT_ALIGNMENT;

    // communication related data struct

    struct BGLTorus_FFT_Headers
      {
      _BGL_TorusPktHdr mBGLTorus_Hdr;
      FFT_Hdr          mFFT_Hdr;
      } FFT3D_STUCT_ALIGNMENT;

    struct ActorPacket
      {
      FFT_Hdr mHdr;
      Value   mData[ ELEMENTS_PER_PACKET ];
      } FFT3D_STUCT_ALIGNMENT;

    //  int* mCompleteFFTsInBarCount_b;
public:
    struct ComputeFFTsInQueueActorPacket
    {
      int           mStart;
      int           mCount;
      FFT3D         *This;
    } FFT3D_STUCT_ALIGNMENT;

    enum { GEN_HEADER_SIZE_Z = ( LOCAL_N_X * LOCAL_N_Y * ( LOCAL_N_Z + POINTS_PER_PACKET_Z - 1) / POINTS_PER_PACKET_Z ) };
    enum { GEN_HEADER_SIZE_Y = ( P_X * P_Y * ( LOCAL_N_X * BAR_SIZE_Y ) * ((BAR_SIZE_X + POINTS_PER_PACKET_Y - 1 ) / POINTS_PER_PACKET_Y )) };
    enum { GEN_HEADER_SIZE_X = ( P_Z * P_X * LOCAL_N_Z * BAR_SIZE_X * ( BAR_SIZE_Z+POINTS_PER_PACKET_X-1 ) / POINTS_PER_PACKET_X ) };
    enum { GEN_HEADER_SIZE_B = ((( LOCAL_N_Z / P_Y ) * LOCAL_N_X )*( (LOCAL_N_Y * P_Y) + POINTS_PER_PACKET_B - 1) / POINTS_PER_PACKET_B ) };

    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_Z[ GEN_HEADER_SIZE_Z ] FFT3D_STUCT_ALIGNMENT;
    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_Y[ GEN_HEADER_SIZE_Y ] FFT3D_STUCT_ALIGNMENT;
    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_X[ GEN_HEADER_SIZE_X ] FFT3D_STUCT_ALIGNMENT;
    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_B[ GEN_HEADER_SIZE_B ] FFT3D_STUCT_ALIGNMENT;


    int mPacketsPerFFT_x FFT3D_STUCT_ALIGNMENT;
    int mPacketsPerFFT_y FFT3D_STUCT_ALIGNMENT;
    int mPacketsPerFFT_z FFT3D_STUCT_ALIGNMENT;
    int mPacketsPerFFT_b FFT3D_STUCT_ALIGNMENT;

    PacketCounterManager* mPcm FFT3D_STUCT_ALIGNMENT;


    ActorPacket zap FFT3D_STUCT_ALIGNMENT;
    ActorPacket xap FFT3D_STUCT_ALIGNMENT;
    ActorPacket yap FFT3D_STUCT_ALIGNMENT;
    ActorPacket bap FFT3D_STUCT_ALIGNMENT;

    static int PacketActorFx_X( void* pkt );
    static int PacketActorFx_Y( void* pkt );
    static int PacketActorFx_Z( void* pkt );
    static int PacketActorFx_B( void* pkt );

    static int ComputeFFTsInQueue_X( void* pkt );
    static int ComputeFFTsInQueue_Y( void* pkt );
    static int ComputeFFTsInQueue_Z( void* pkt );

    void SendPackets_Z( fft3d_complex in [ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z  ] );
    void SendPackets_X();
    void SendPackets_Y();
    void SendPackets_B( fft3d_complex out[ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z  ] );

    void InitHeaders();

    inline int UniProcFilter( int proc );

    static void ActiveMsgBarrier( FFT_Counter & completePencilsInBarCount,
                                  int   barSize,
                                  int   packetsPerFFT );

       // methods for debbugging ...
     //   void PrintComplexArrayR(complex ** array, int nrow, int ncol);
     //   void PrintComplexArrayI(complex ** array, int nrow, int ncol);
     //   int Validate3DFFT(complex ***in, const int globalNx);

  };


void
BGLTorusSimpleActivePacketSend( _BGL_TorusPktHdr & pktHdr,
                                Bit32              ActorFx,
                                int                x, int y, int z,
                                int                len,
                                void             * data )
  {
  BegLogLine( PKFXLOG_ACTIVE_MSG_SEND )
      << "BGLTorusSimpleActivePacketSend:: "
      << " Send To: " << x << ","<< y << ","<< z
      << EndLogLine;

//   TR_BGLActivePacketSendStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
//                                      "TR_TR_BGLActivePacketSendStart",
//                                      Platform::Thread::GetId(),
//                                      TR_BGLActivePacketSendStart );


  // RULE TO FORCE SENDS OF ALL PACKETS WHEN USING BOTH CORES.
  // Application loopback on stack is dangerous because it causes a dual writer situation.
  // On BG/L, this could cause sharing of cache lines between cores and thus, data loss.
  #ifdef USE_2_CORES_FOR_FFT
  #define FFT3D_SEND_LOCAL_ACTORS
  #endif

#ifndef FFT3D_SEND_LOCAL_ACTORS  // Allow a flag to test loop back

  int myPx, myPy, myPz;
  BGLPartitionGetCoords( &myPx, &myPy, &myPz );
  if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
    {
    ((BGLTorusBufferedActivePacketHandler)(ActorFx))( (Bit8 (*)[240])data, pktHdr.swh1.arg, pktHdr.swh0.extra );
    }

  else

#endif

#ifdef USE_2_CORES_FOR_FFT
  int myPx, myPy, myPz;
  BGLPartitionGetCoords( &myPx, &myPy, &myPz );
    if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
      {
        VirtualFifo_BGLTorusSendPacketUnaligned( & pktHdr,      // Previously created header to use
                                                 data,     // Source address of data (16byte aligned)
                                                 len );
      }
    else
      {

//         BGLTorusSendPacketUnaligned( & pktHdr,      // Previously created header to use
//                                      data,     // Source address of data (16byte aligned)
//                                      len ); // Payload bytes = ((chunks * 32) - 16)

        int PrevFullQueue = 0;
        int FullQueue = 0;
        while(1)
          {

            int rc = BGLTorusSendPacketNonBlocking( & pktHdr,  // Previously created header to use
                                                    data,    // Source address of data (16byte aligned)
                                                    len );   // Payload bytes = ((chunks * 32) - 16)

            if(rc == 1)
              {
                // if in PrevFullQueue state dump the finis.
                if( PrevFullQueue )
                  Full_Send_Queue_State_Finis.HitOE( 0,// PKTRACE_TUNE && !PKTRACE_OFF,
                                                     "Full_Send_Queue_State_Finis",
                                                     0,
                                                     Full_Send_Queue_State_Finis );

                FullQueue = 0;
                break;
              }

            FullQueue = 1;

            // Start of the edge
            if( PrevFullQueue != FullQueue )
              Full_Send_Queue_State_Start.HitOE( 0,//PKTRACE_TUNE && !PKTRACE_OFF,
                                                 "Full_Send_Queue_State_Start",
                                                 0,
                                                 Full_Send_Queue_State_Start );

            PrevFullQueue = FullQueue;
          }
      }
#else
    {

    int PollCount = 0;
    while( 1 )
      {
      int rc = BGLTorusSendPacketUnalignedNonBlocking( & pktHdr,  // Previously created header to use
                                                         data,    // Source address of data (16byte aligned)
                                                         len );   // Payload bytes = ((chunks * 32) - 16)
      // send succeds if rc == 0 , otherwise go round, drain network and try again.
      if( rc == 0 )
        break;

      PollCount++;

      #ifndef USE_2_CORES_FOR_FFT
        // NEED TO DRAIN THE NETWORK
        while( 1 )
          {
          int count = 1;
          BGLTorusPoll( &count );
          if( count == 1 )
            break;
          }
      #endif

      #ifndef FFT3D_SPINWAIT
        if( (PollCount % 16) == 0 )
          {
          BegLogLine(1)
            << "Polling to send active message packet! "
            << PollCount
            << EndLogLine;
          usleep(10000);
          }
      #endif
      }

    }
#endif

//     TR_BGLActivePacketSendFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
//                                        "TR_TR_BGLActivePacketSendFinis",
//                                        Platform::Thread::GetId(),
//                                        TR_BGLActivePacketSendFinis );
  }


template<class FFT_PLAN, int FWD_REV >
inline int FFT3D< FFT_PLAN, FWD_REV >::
UniProcFilter( int proc )
{
  return ( mIsUni ? 0 : proc );
}

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
ComputeFFTsInQueue_Z(void * args)
{
  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;

  int start = cqap->mStart;
  int count   = cqap->mCount;


  FFT3D< FFT_PLAN, FWD_REV > *This = cqap->This;

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

//       TR_FFT_1D_ZStart.HitOE( PKTRACE_TUNE,
//                               "TR_FFT_1D_ZStart",
//                               Platform::Thread::GetId(),
//                               TR_FFT_1D_ZStart );

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
      // BegLogLine( PKFXLOG_COMPUTEFFTSINQUEUE_Z )
//       BegLogLine( 0 )
//         << "ComputeFFTsInQueue_Z "
//         << "About to do 1D FFT on index " << index
//         << " (*This).mFFT_1D_Queue_Z[ " << index <<" ]=  " << (void *) (*This).mFFT_1D_Queue_Z[ index ]
//         << " (*This).mFFT_1D_Queue_Z[ " << index+1 <<" ]=  " << (void *) (*This).mFFT_1D_Queue_Z[ index+1 ]
//         << EndLogLine;
#else
      int index = i + start;
#endif

      ((*This).mFFT_z).fftInternal( (*This).mFFT_1D_Queue_Z[ index ],
                                    (*This).mFFT_1D_Queue_Z[ index + 1 ] );

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

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
ComputeFFTsInQueue_X(void * args)
{
  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;

  int start = cqap->mStart;
  int count   = cqap->mCount;
  FFT3D< FFT_PLAN, FWD_REV > *This = cqap->This;

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  for( int i = 0; i < count; i++ )
    {

//       TR_FFT_1D_XStart.HitOE( PKTRACE_TUNE,
//                               "TR_FFT_1D_XStart",
//                               Platform::Thread::GetId(),
//                               TR_FFT_1D_XStart );

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
      BegLogLine( PKFXLOG_COMPUTEFFTSINQUEUE_X )
        << "ComputeFFTsInQueue_X "
        << "About to do 1D FFT on index " << index
        << " (*This).mFFT_1D_Queue_X[ " << index <<" ]=  " << (void *) (*This).mFFT_1D_Queue_X[ index ]
        << " (*This).mFFT_1D_Queue_X[ " << index+1 <<" ]=  " << (void *) (*This).mFFT_1D_Queue_X[ index+1 ]
        << EndLogLine;

#else
      int index = i + start;
#endif



       ((*This).mFFT_x).fftInternal( (*This).mFFT_1D_Queue_X[ index ],
                                     (*This).mFFT_1D_Queue_X[ index + 1 ] );

//        ((*This).mFFT_x).fftInternal( (*This).mFFT_1D_Queue_X[ index + 1 ],
//                                      (*This).mFFT_1D_Queue_X[ index + 1 ] );

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

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
ComputeFFTsInQueue_Y(void * args)
{
  ComputeFFTsInQueueActorPacket* cqap = (ComputeFFTsInQueueActorPacket *) args;

  int start = cqap->mStart;
  int count   = cqap->mCount;
  FFT3D< FFT_PLAN, FWD_REV > *This = cqap->This;

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
  count = count / 2;
#endif

  for( int i = 0; i < count ; i++ )
    {

//       TR_FFT_1D_YStart.HitOE( PKTRACE_TUNE,
//                               "TR_FFT_1D_YStart",
//                               Platform::Thread::GetId(),
//                               TR_FFT_1D_YStart );

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
      int index = 2*i + start;
      BegLogLine( PKFXLOG_COMPUTEFFTSINQUEUE_Y )
        << "ComputeFFTsInQueue_Y "
        << "About to do 1D FFT on index " << index
        << " (*This).mFFT_1D_Queue_Y[ " << index <<" ]=  " << (void *) (*This).mFFT_1D_Queue_Y[ index ]
        << " (*This).mFFT_1D_Queue_Y[ " << index+1 <<" ]=  " << (void *) (*This).mFFT_1D_Queue_Y[ index+1 ]
        << EndLogLine;
#else
      int index = i + start;
#endif
       ((*This).mFFT_y).fftInternal( (*This).mFFT_1D_Queue_Y[ index ],
                                     (*This).mFFT_1D_Queue_Y[ index + 1] );

//        ((*This).mFFT_y).fftInternal( (*This).mFFT_1D_Queue_Y[ index + 1 ],
//                                      (*This).mFFT_1D_Queue_Y[ index + 1 ] );

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


template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV > :: Init( FFT3D** ptr )
  {

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;


  char filename[ 128 ];

  BegLogLine( PKFXLOG_INIT )    << "FFT3D::Init() "
    << EndLogLine;


#ifdef PK_AIX
  if( FWD_REV == FORWARD )
    sprintf(filename,"/tmp/fft_packet_counter.forward.%d", getuid() );
  else
    sprintf(filename,"/tmp/fft_packet_counter.reverse.%d", getuid() );
#endif

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  // mPcm = new PacketCounterManager( P_X * P_Y * P_Z, 1, filename );
  //assert( mPcm != NULL );

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;


  mIsUni = ( ( P_X == 1 ) && ( P_Y == 1 ) && ( P_Z == 1 ) );

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << " mIsUni=" << mIsUni
    << EndLogLine;

  mCompleteFFTsInBarCount_x.mVar = 0;
  mCompleteFFTsInBarCount_y.mVar = 0;
  mCompleteFFTsInBarCount_z.mVar = 0;

  mIOP_FFT_Compute_Done_x.mVar = 0;
  mIOP_FFT_Compute_Done_y.mVar = 0;
  mIOP_FFT_Compute_Done_z.mVar = 0;

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  if( ELEMENTS_PER_PACKET == 14 )
    {
      mPacketsPerFFT_x = (int) ((( P_X * LOCAL_N_X ) + POINTS_PER_PACKET_X - 1) / POINTS_PER_PACKET_X );
      mPacketsPerFFT_y = (int) ((( P_Y * LOCAL_N_Y ) + POINTS_PER_PACKET_Y - 1) / POINTS_PER_PACKET_Y );
      mPacketsPerFFT_z = (int) ((( P_Z * LOCAL_N_Z ) + POINTS_PER_PACKET_Z - 1) / POINTS_PER_PACKET_Z );
      mPacketsPerFFT_b = (int)   (       ( LOCAL_N_Y + POINTS_PER_PACKET_B - 1) / POINTS_PER_PACKET_B );
    }
  else
    {
      mPacketsPerFFT_x = (int) ( P_X * ( LOCAL_N_X / POINTS_PER_PACKET_X ));
      mPacketsPerFFT_y = (int) ( P_Y * ( LOCAL_N_Y / POINTS_PER_PACKET_Y ));
      mPacketsPerFFT_z = (int) ( P_Z * ( LOCAL_N_Z / POINTS_PER_PACKET_Z ));
      mPacketsPerFFT_b = (int) (       ( LOCAL_N_Y / POINTS_PER_PACKET_B ));
    }

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

//   mFFT_x = new FFT_1D_X();  // 1D fft in the x-dim
//   mFFT_y = new FFT_1D_Y();  // 1D fft in the y-dim
//   mFFT_z = new FFT_1D_Z();  // 1D fft in the z-dim

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  BGLPartitionGetCoords( &mMyP_x, &mMyP_y, &mMyP_z );

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  InitHeaders();

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  Platform::Memory::ExportFence();

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  return 1;
  }

//#define DEBUG_Z 1
template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
PacketActorFx_Z(void* pkt)
  {
  ActorPacket *ap = (ActorPacket *) pkt;

  FFT3D<FFT_PLAN,FWD_REV> *This = (*ap).mHdr.This;

  int fftIndex                        = (*ap).mHdr.aIndex; // formally a
  int startOfDataIn1DFFTForThisPacket = (*ap).mHdr.bIndex;
  int PointsPerPacket                 = (*ap).mHdr.pointsPerPacket;

  // int zPointsPerPacket = (*This).POINTS_PER_PACKET_Z;

  double_copy( (double *) &((*This).mGlobalData_z[ fftIndex ][ startOfDataIn1DFFTForThisPacket ]),
               (double *) (*ap).mData,
               PointsPerPacket * 2 );

  for(int k=0; k < POINTS_PER_PACKET_Z; k++)
    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z & PKFXLOG_DEBUG_DATA )
      << "PacketActorFx_Z:: "
      << "Recv a packet( "
      << (*This).mMyP_z << " ab " << fftIndex
      << "," << startOfDataIn1DFFTForThisPacket+k << ") =  "
      << (*This).mGlobalData_z[ fftIndex ][ startOfDataIn1DFFTForThisPacket + k ].re
      << ","
      << (*This).mGlobalData_z[ fftIndex ][ startOfDataIn1DFFTForThisPacket + k ].im
      << EndLogLine;


  (*This).mCompletePacket_z[ fftIndex ]++;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
    << " PacketActorFx_Z:: "
    << " (*This).mCompletePacket_z[ " << fftIndex << " ]= "
    << (*This).mCompletePacket_z[ fftIndex ]
    << " (*This).mPacketsPerFFT_z "
    << (*This).mPacketsPerFFT_z
    << EndLogLine;


  if(  (*This).mCompletePacket_z[ fftIndex ] == (*This).mPacketsPerFFT_z )
    {
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT )
        << " PacketActorFx_Z:: "
        << " Got " << (*This).mPacketsPerFFT_z << " packets"
        << " 1D_FFT_Z [ " << fftIndex << "] is READY"
        << EndLogLine;

      Platform::Memory::ExportFence( (void *) (*This).mGlobalData_z[ fftIndex ], sizeof((*This).mGlobalData_z[ fftIndex ]) );

      (*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTsInBarCount_z.mVar ] = (*This).mGlobalData_z[ fftIndex ];
      Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_Z[ (*This).mCompleteFFTsInBarCount_z.mVar ], sizeof(fft3d_complex *) );

      (*This).mCompleteFFTsInBarCount_z.mVar += 1;

      Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTsInBarCount_z), sizeof(FFT_Counter) );

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT  )
        << "PacketActorFx_Z "
        << " Done incremented mCompleteFFTsInBarCount_z to "
        << (*This).mCompleteFFTsInBarCount_z.mVar
        << EndLogLine;

      TR_1DFFT_READY_Z.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,
                              "TR_1DFFT_READY_Z",
                              Platform::Thread::GetId(),
                              TR_1DFFT_READY_Z );
    }

  return 0;
  }

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::
SendPackets_Z(  fft3d_complex in [ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z  ] )
  {
  BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering SendPackets_Z"
      << EndLogLine;

  // ActorPacket zap;

  BegLogLine( PKFXLOG_GATHER_Z )
    << "SendPackets_Z "
    << " &zap=" << (void*) &zap
    << EndLogLine;

  // int count = 0;
  int headerCount = 0;

  int zPointsInLastPacket = LOCAL_N_Z % POINTS_PER_PACKET_Z;

  for(int j=0; j < LOCAL_N_Y; j++)
      {
        for(int i=0; i < LOCAL_N_X; i++)
          {
            int loadCountZ = 0;
            int trgPz = i / BAR_SIZE_Z;
            int zPointsPerPacket = POINTS_PER_PACKET_Z;

            for(int k =0; k < LOCAL_N_Z; k++)
              {
                BegLogLine( PKFXLOG_GATHER_Z_TRIM )
                  << "SendPackets_Z "
                  << " trgPz=" << trgPz
                  << EndLogLine;

                BegLogLine( PKFXLOG_GATHER_Z )
                  << "SendPackets_Z:: "
                  << " i=" << i
                  << " j=" << j
                  << " k=" << k
                  << " loadCountZ=" << loadCountZ
                  << " zPointsPerPacket=" << zPointsPerPacket
                  << EndLogLine;

                BegLogLine( PKFXLOG_GATHER_Z )
                  << "SendPackets_Z:: "
                  << " in=" << (void*) in
                  << EndLogLine;

                BegLogLine( PKFXLOG_GATHER_Z )
                  << "SendPackets_Z:: "
                  << " in[ "<<i<<" ][ "<<j<<" ][ "<<k<<" ].re=" << in[ i ][ j ][ k ].re
                  << " in[ "<<i<<" ][ "<<j<<" ][ "<<k<<" ].im=" << in[ i ][ j ][ k ].im
                  << EndLogLine;


                zap.mData[ loadCountZ ].re = in[ i ][ j ][ k ].re;
                zap.mData[ loadCountZ ].im = in[ i ][ j ][ k ].im;
                loadCountZ++;

                BegLogLine( PKFXLOG_GATHER_Z )
                  << "SendPackets_Z:: "
                  << " i=" << i
                  << " j=" << j
                  << " k=" << k
                  << " loadCountZ=" << loadCountZ
                  << " zPointsPerPacket=" << zPointsPerPacket
                  << EndLogLine;


                if( ( k == LOCAL_N_Z-1 ) && ( loadCountZ < POINTS_PER_PACKET_Z ) )
                  {
                    // Points in the last packet
                    zPointsPerPacket = zPointsInLastPacket;
                  }

                if( loadCountZ == zPointsPerPacket )
                  {

                    for(int kk=0; kk < zPointsPerPacket; kk++)
                      {
                        BegLogLine( PKFXLOG_GATHER_Z & PKFXLOG_DEBUG_DATA  | PKFXLOG_GATHER_Z_TRIM )
                          << "SendPackets_Z::"
                          << "Sending a packet to { " << mMyP_x << " , " << mMyP_y << " , " << trgPz << " }"
                          <<  " (a,b) " << (i % BAR_SIZE_Z) * LOCAL_N_Y + j
                          << "," << k+1 - loadCountZ + mMyP_z * LOCAL_N_Z <<" data( "
                          << zap.mData[ kk ].re << ","
                          << zap.mData[ kk ].im << ")"
                          << EndLogLine;
                      }

                    zap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mFFT_Hdr;

                    //int myRank = BGLPartitionGetRankXYZ();
                    //mPcm->TickSend( myRank );

                    BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mBGLTorus_Hdr,
                                                    (Bit32) PacketActorFx_Z,
                                                    mMyP_x,
                                                    mMyP_y,
                                                    mIsUni ? mMyP_z : trgPz,
                                                    sizeof( zap ),
                                                    &zap);
                    headerCount++;
                    loadCountZ = 0;
                  }
              }
          }
      }

  BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
    << "Exiting SendPackets_Z"
    << EndLogLine;
  }

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
PacketActorFx_X(void* pkt)
  {
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D< FFT_PLAN, FWD_REV > *This = (*ap).mHdr.This;

  int fftIndex                        = (*ap).mHdr.aIndex; // formally a
  int startOfDataIn1DFFTForThisPacket = (*ap).mHdr.bIndex;
  int PointsPerPacket                 = (*ap).mHdr.pointsPerPacket;


//   memcpy( (char*) &((*This).mGlobalData_x[ fftIndex ][ startOfDataIn1DFFTForThisPacket ]),
//           (char*) (*ap).mData,
//           sizeof(fft3d_complex) * POINTS_PER_PACKET_X );
   double_copy( (double *) &((*This).mGlobalData_x[ fftIndex ][ startOfDataIn1DFFTForThisPacket ]),
                (double *) (*ap).mData,
                PointsPerPacket * 2 );


  (*This).mCompletePacket_x[ fftIndex ] ++;

  for(int k=0; k < POINTS_PER_PACKET_X; k++)
    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X  & PKFXLOG_DEBUG_DATA )
      << "PacketActorFx_X:: "
      << "Recv a packet( "
      << (*This).mMyP_z << " ab " << fftIndex
      << "," << startOfDataIn1DFFTForThisPacket+k << ") =  "
      << (*This).mGlobalData_x[ fftIndex ][ startOfDataIn1DFFTForThisPacket + k ].re
      << ","
      << (*This).mGlobalData_x[ fftIndex ][ startOfDataIn1DFFTForThisPacket + k ].im
      << EndLogLine;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X )
    << " PacketActorFx_x:: "
    << " (*This).mCompletePacket_x[ " << fftIndex << " ]= "
    << (*This).mCompletePacket_x[ fftIndex ]
    << " (*This).mPacketsPerFFT_x "
    << (*This).mPacketsPerFFT_x
    << EndLogLine;


  if(  (*This).mCompletePacket_x[ fftIndex ] == (*This).mPacketsPerFFT_x )
    {
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT )
        << " PacketActorFx_X:: "
        << " Got " << (*This).mPacketsPerFFT_x << " packets"
        << " 1D_FFT_X [ " << fftIndex << "] is READY"
        << EndLogLine;

      Platform::Memory::ExportFence( (void *) (*This).mGlobalData_x[ fftIndex ], sizeof((*This).mGlobalData_x[ fftIndex ]) );

      (*This).mFFT_1D_Queue_X[ (*This).mCompleteFFTsInBarCount_x.mVar ] = (*This).mGlobalData_x[ fftIndex ];
      Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_X[ (*This).mCompleteFFTsInBarCount_x.mVar ], sizeof(fft3d_complex *) );

      (*This).mCompleteFFTsInBarCount_x.mVar += 1;

      Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTsInBarCount_x), sizeof(FFT_Counter) );

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X_1D_FFT  )
        << "PacketActorFx_X "
        << " Done incremented mCompleteFFTsInBarCount_x to "
        << (*This).mCompleteFFTsInBarCount_x.mVar
        << EndLogLine;

      TR_1DFFT_READY_X.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,//PKTRACE_TUNE,
                              "TR_1DFFT_READY_X",
                              Platform::Thread::GetId(),
                              TR_1DFFT_READY_X );
    }

  return 0;
  }

template<class FFT_PLAN, int FWD_REV>
void FFT3D< FFT_PLAN, FWD_REV >::
SendPackets_X()
  {
    // ActorPacket xap;

  int loadCountX;
  int headerCount=0;

  int temp1 = UniProcFilter( mMyP_x ) * LOCAL_N_X;
  int temp2 = UniProcFilter( mMyP_z ) * BAR_SIZE_Z + temp1;
  int temp3 = temp2 + BAR_SIZE_Z;

  for(int ax=0; ax < LOCAL_N_Z * BAR_SIZE_X; ax++)
    {
      for(int trgPz=0; trgPz < P_Z; trgPz++)
        {
          for(int trgPx=0; trgPx < P_X; trgPx++)
            {
              int xPointsPerPacket =  POINTS_PER_PACKET_X;
              loadCountX=0;
              for(int bx = temp2;
                  bx < temp3;
                  bx++)
                {
                    int az = ((bx%LOCAL_N_X)%BAR_SIZE_Z)*LOCAL_N_Y+ax/LOCAL_N_Z + trgPx*BAR_SIZE_X;
                    int bz = ax%LOCAL_N_Z + LOCAL_N_Z*trgPz;

                     BegLogLine( PKFXLOG_GATHER_X  )
                       << "SendPackets_X:: "
                       << "ax = "<< ax << "bx = "<< bx
                       << "az = "<< az << " bz = "<< bz
                       << EndLogLine;

                     BegLogLine( PKFXLOG_GATHER_X & PKFXLOG_DEBUG_DATA )
                       << "SendPackets_X:: "
                       << "mGlobalData_z[" << az << "][" << bz << "]="
                       << mGlobalData_z[ az ][ bz ].re <<","
                       << mGlobalData_z[ az ][ bz ].im
                       << EndLogLine;

                    xap.mData[ loadCountX ].re = mGlobalData_z[ az ][ bz ].re;
                    xap.mData[ loadCountX ].im = mGlobalData_z[ az ][ bz ].im;
                    loadCountX++;

                    if( bx == ((temp3 - temp2) - 1) && (loadCountX < POINTS_PER_PACKET_X) )
                      xPointsPerPacket = (temp3 - temp2) % POINTS_PER_PACKET_X;

                    if( loadCountX == xPointsPerPacket )
                      {

                        for(int kk=0; kk < xPointsPerPacket; kk++)
                          BegLogLine( PKFXLOG_GATHER_X & PKFXLOG_DEBUG_DATA )
                            << "SendPackets_X::"
                            << "Sending a packet to { " << trgPx << " , " << mMyP_y << " , " << trgPz << " }"
                            << " data( "
                            << xap.mData[ kk ].re << ","
                            << xap.mData[ kk ].im << ")"
                            << EndLogLine;

                    xap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mFFT_Hdr;


                    // int myRank = BGLPartitionGetRankXYZ();
                    //mPcm->TickSend( myRank );


                    BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mBGLTorus_Hdr,
                                                    (Bit32) PacketActorFx_X,
                                                    mIsUni ? mMyP_x : trgPx,
                                                    mMyP_y,
                                                    mIsUni ? mMyP_z : trgPz,
                                                    sizeof( xap ),
                                                   &xap );

                    loadCountX = 0;
                    headerCount++;
                  }
              }
            }
        }
    }
  }

// int  FFT3D<FFT_PLAN>::PacketActorFx_Y(void* pkt)
template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
PacketActorFx_Y(void* pkt)
  {

  ActorPacket *ap = (ActorPacket *) pkt;

  FFT3D<FFT_PLAN,FWD_REV> *This = (*ap).mHdr.This;

  int fftIndex                        = (*ap).mHdr.aIndex; // formally a
  int startOfDataIn1DFFTForThisPacket = (*ap).mHdr.bIndex;
  int PointsPerPacket                 = (*ap).mHdr.pointsPerPacket;


//   memcpy( (char*) &((*This).mGlobalData_y[ fftIndex ][ startOfDataIn1DFFTForThisPacket ]),
//           (char*) (*ap).mData,
//           sizeof(fft3d_complex) *  POINTS_PER_PACKET_Y );
   double_copy( (double *) &((*This).mGlobalData_y[ fftIndex ][ startOfDataIn1DFFTForThisPacket ]),
                (double *) (*ap).mData,
                PointsPerPacket * 2 );

  for(int k=0; k < POINTS_PER_PACKET_Y; k++)
    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y & PKFXLOG_DEBUG_DATA )
      << "PacketActorFx_Y:: "
      << "Recv a packet( "
      << (*This).mMyP_y << " ab " << fftIndex
      << "," << startOfDataIn1DFFTForThisPacket+k << ") =  "
      << (*This).mGlobalData_y[ fftIndex ][ startOfDataIn1DFFTForThisPacket + k ].re
      << ","
      << (*This).mGlobalData_y[ fftIndex ][ startOfDataIn1DFFTForThisPacket + k ].im
      << EndLogLine;


  (*This).mCompletePacket_y[ fftIndex ]++;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y )
    << " PacketActorFx_Y:: "
    << " (*This).mCompletePacket_y[ " << fftIndex << " ]= "
    << (*This).mCompletePacket_y[ fftIndex ]
    << " (*This).mPacketsPerFFT_y "
    << (*This).mPacketsPerFFT_y
    << EndLogLine;


  if(  (*This).mCompletePacket_y[ fftIndex ] == (*This).mPacketsPerFFT_y )
    {
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT )
        << " PacketActorFx_Y:: "
        << " Got " << (*This).mPacketsPerFFT_y << " packets"
        << " 1D_FFT_Y [ " << fftIndex << "] is READY"
        << EndLogLine;

      Platform::Memory::ExportFence( (void *) (*This).mGlobalData_y[ fftIndex ], sizeof((*This).mGlobalData_y[ fftIndex ]) );

      (*This).mFFT_1D_Queue_Y[ (*This).mCompleteFFTsInBarCount_y.mVar ] = (*This).mGlobalData_y[ fftIndex ];
      Platform::Memory::ExportFence( (void *) &(*This).mFFT_1D_Queue_Y[ (*This).mCompleteFFTsInBarCount_y.mVar ], sizeof(fft3d_complex *) );

      (*This).mCompleteFFTsInBarCount_y.mVar += 1;

      Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTsInBarCount_y), sizeof(FFT_Counter) );

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT  )
        << "PacketActorFx_Y "
        << " Done incremented mCompleteFFTsInBarCount_y to "
        << (*This).mCompleteFFTsInBarCount_y.mVar
        << EndLogLine;

      TR_1DFFT_READY_Y.HitOE( 0,//PKTRACE_TUNE & !PKTRACE_OFF,//PKTRACE_TUNE,
                              "TR_1DFFT_READY_Y",
                              Platform::Thread::GetId(),
                              TR_1DFFT_READY_Y );
    }


  return 0;
  }

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::
SendPackets_Y()
  {
    // ActorPacket yap;
  int loadCountY;
  int headerCount=0;

  int temp1 = UniProcFilter( mMyP_y ) * LOCAL_N_Y;
  int temp2 = UniProcFilter( mMyP_x ) * BAR_SIZE_X  + temp1;
  int temp3 = temp2 + BAR_SIZE_X;

  for(int ay=0; ay < LOCAL_N_X * BAR_SIZE_Y; ay++)
    {

      for(int trgPx=0; trgPx < P_X; trgPx++)
        {
          for(int trgPy=0; trgPy < P_Y; trgPy++)
            {
              int yPointsPerPacket = POINTS_PER_PACKET_Y;
              loadCountY = 0;
              for(int by = temp2;
                      by < temp3;
                      by ++ )
                {
                  int ax = ( ( by % LOCAL_N_Y ) % BAR_SIZE_X ) * LOCAL_N_Z +
                      ay / LOCAL_N_X + trgPy * BAR_SIZE_Y;

                  int bx = ay % LOCAL_N_X + LOCAL_N_X * trgPx;

                  yap.mData[ loadCountY ].re = mGlobalData_x[ ax ][ bx ].re;
                  yap.mData[ loadCountY ].im = mGlobalData_x[ ax ][ bx ].im;
                  loadCountY++;

                   BegLogLine( PKFXLOG_GATHER_Y & PKFXLOG_DEBUG_DATA )
                     << "SendPackets_Y::"
                     << "ax = "<< ax << " bx = "<< bx
                     << "ay = "<< ay << " bz = "<< by
                     << "mGlobalData_x[" << ax << "][" << bx << "]"
                     << mGlobalData_x[ ax ][ bx ].re <<","
                     << mGlobalData_x[ ax ][ bx ].im
                     << EndLogLine;

                   if( by == ((temp3 - temp2) - 1) && (loadCountY < POINTS_PER_PACKET_Y) )
                     yPointsPerPacket = (temp3 - temp2) % POINTS_PER_PACKET_Y;

                   if( loadCountY == yPointsPerPacket )
                     {
                       for(int kk=0; kk < POINTS_PER_PACKET_Y; kk++)
                         BegLogLine( PKFXLOG_GATHER_Y )
                               << "SendPackets_Y::"
                               << "Sending a packet to procXY  " << trgPx << ","
                               << trgPy << " ab " << ay << "," << by-loadCountY+1
                               << "data ( "
                               << yap.mData[ kk ].re << "," << yap.mData[ kk ].im << " )"
                               << EndLogLine;

                       yap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mFFT_Hdr;

                       BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mBGLTorus_Hdr,
                                                       (Bit32) PacketActorFx_Y,
                                                       mIsUni ? mMyP_x : trgPx,
                                                       mIsUni ? mMyP_y : trgPy,
                                                       mMyP_z,
                                                       sizeof( yap ),
                                                      &yap );
                       loadCountY = 0;
                       headerCount++;
                    }
                }
            }
        }
    }
  }

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
PacketActorFx_B(void* pkt)
  {
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D<FFT_PLAN, FWD_REV> *This = (*ap).mHdr.This;

  // int myRank = BGLPartitionGetRankXYZ();

  int i = (*ap).mHdr.aIndex;
  int j = (*ap).mHdr.bIndex;
  int k = (*ap).mHdr.cIndex;
  int pointsPerPacket = (*ap).mHdr.pointsPerPacket;

  for(int jp=0;  jp < pointsPerPacket; jp++)
    {
      if( FWD_REV == FORWARD )
        {
        (*This).mLocalOut[ i ][ j+jp ][ k ] = (*ap).mData[ jp ];

        BegLogLine( PKFXLOG_PACKET_ACTOR_FX_B & PKFXLOG_DEBUG_DATA )
          << "PacketActorFx_B:: "
          << "OutB[" << i << " ]["
          << j + jp << "][" << k << "] ="
          << (*This).mLocalOut[ i ][ j+jp ][ k ].re <<" , "
          << (*This).mLocalOut[ i ][ j+jp ][ k ].im <<" )"
          << EndLogLine;
        }
      else
        {
        (*This).mLocalIn[ i ][ j+jp ][ k ] = (*ap).mData[ jp ];

        BegLogLine( PKFXLOG_PACKET_ACTOR_FX_B & PKFXLOG_DEBUG_DATA )
          << "PacketActorFx_B:: "
          << "OutB[" << i << " ]["
          << j + jp << "][" << k << "] ="
          << (*This).mLocalIn[ i ][ j+jp ][ k ].re <<" , "
          << (*This).mLocalIn[ i ][ j+jp ][ k ].im <<" )"
          << EndLogLine;
        }
    }

   BegLogLine( PKFXLOG_PACKET_ACTOR_FX_B )
     << "PacketActorFx_B:: "
     << "bCompletePacket[" << i << "][" << k << " ]="
     << (*This).mCompletePacket_b[ i ][ k ] << "    "
     << (*This).mPacketsPerFFT_b
     << EndLogLine;

  // Platform::Memory::ImportFence();
  ( (*This).mCompletePacket_b[ i ][ k ] )++;// (*This).POINTS_PER_PACKET_X;
  // Platform::Memory::ExportFence();

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_B )
    << " PacketActorFx_b:: "
    << " (*This).mCompletePacket_b[ " << i << " ][" << k << "]= "
    << (*This).mCompletePacket_b[ i ][ k ]
    << " (*This).mPacketsPerFFT_b "
    << (*This).mPacketsPerFFT_b
    << EndLogLine;


  int cmpl = ((*This).mCompletePacket_b[ i ][ k ]);

  if( cmpl ==(*This).mPacketsPerFFT_b )
    {

      (*This).mCompleteFFTsInBarCount_b[ i ]++;
      // Platform::Memory::ExportFence();
      if( (*This).mCompleteFFTsInBarCount_b[ i ].mVar == LOCAL_N_Z )
        Platform::Memory::ExportFence();

      Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTsInBarCount_b[ i ]), sizeof(FFT_Counter) );

//        if( FWD_REV == FORWARD )
//          Platform::Memory::ExportFence((void*) (*This).mLocalOut, sizeof((*This).mLocalOut));
//        else
//          Platform::Memory::ExportFence((void*) (*This).mLocalIn, sizeof((*This).mLocalOut));

//       (*This).mCompleteFFTsInBarCount_b[ i ] += 1;
//       Platform::Memory::ExportFence( (void *) &((*This).mCompleteFFTsInBarCount_b[ i ]), sizeof(int) );


     BegLogLine( PKFXLOG_PACKET_ACTOR_FX_B_1D_FFT )
       << "PacketActorFx_B:: "
       << "bCompleteFFTs[" << i << "]="
       << (*This).mCompleteFFTsInBarCount_b[ i ].mVar
       << EndLogLine;
    }
  return 0;
  }

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::
SendPackets_B(fft3d_complex out[ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z ])
  {
    // ActorPacket bap;
  int headerCount=0;

  for(int a=0; a < ( LOCAL_N_Z / P_Y ) * LOCAL_N_X; a++)
    {
      int loadCountB = 0;
      int bPointsPerPacket = POINTS_PER_PACKET_B;

      for(int b=0; b < LOCAL_N_Y * P_Y; b++)
        {
            // int j = b % LOCAL_N_Y;
          int trgPy = b / LOCAL_N_Y;

          bap.mData[ loadCountB ].re = mGlobalData_y[ a ][ b ].re;
          bap.mData[ loadCountB ].im = mGlobalData_y[ a ][ b ].im;
          loadCountB++;

          if( ( b == (LOCAL_N_Y * P_Y) - 1 ) && (loadCountB < POINTS_PER_PACKET_B ))
            bPointsPerPacket = ( LOCAL_N_Y * P_Y ) % POINTS_PER_PACKET_B;

          if( loadCountB == bPointsPerPacket )
            {
//               bap.mHdr.This = this;
//               bap.mHdr.aIndex = a % LOCAL_N_X;
//               bap.mHdr.bIndex = b % LOCAL_N_Y - loadCountB + 1;
//               bap.mHdr.cIndex = a / LOCAL_N_X + mMyP_y * BAR_SIZE_Y;

              BegLogLine( PKFXLOG_GATHER_B )
                << "SendPackets_B::POINTS_PER_PACKET_B "
                << POINTS_PER_PACKET_B << " , "
                << loadCountB << "  , "
                << "localNy" <<LOCAL_N_Y
                << EndLogLine;

              bap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mFFT_Hdr;

              // int myRank = BGLPartitionGetRankXYZ();
              //mPcm->TickSend( myRank );

              BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr,
                                              (Bit32) PacketActorFx_B,
                                              mMyP_x,
                                              mIsUni ? mMyP_y : trgPy,
                                              mMyP_z,
                                              sizeof( bap ),
                                             &bap );
              loadCountB = 0;
              headerCount++;
            }
        }
    }
  }


template<class FFT_PLAN, int FWD_REV >
void FFT3D<FFT_PLAN, FWD_REV>::
ActiveMsgBarrier(FFT_Counter &aCompleteFFTsInBarCount,
                 int aTotalFFTsInBar,
                 int aPacketsPerBar )
  {

  BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER )
    << "ActiveMsgBarrier:: "
    << "ActiveMsgBarrier BEGIN, have " << aCompleteFFTsInBarCount.mVar
    << " of " << aTotalFFTsInBar
    << EndLogLine;

  // BarMon is a monitor of progress in filling in the bar.
  // BarMon loosly tracks this progress.
  int barMon = aCompleteFFTsInBarCount.mVar ;

  Platform::Memory::ImportFence((void *) &aCompleteFFTsInBarCount, sizeof(FFT_Counter));
  while( aCompleteFFTsInBarCount.mVar < aTotalFFTsInBar )
    {
    Platform::Memory::ImportFence((void *) &aCompleteFFTsInBarCount, sizeof(FFT_Counter));

//     #ifndef USE_2_CORES_FOR_FFT
//       int count1 = 1;
//       BGLTorusWaitCount( &count1 );
//     #endif

    if( barMon != aCompleteFFTsInBarCount.mVar )
      {
      BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER )
          << "ActiveMsgBarrier:: "
          <<" ActiveMsgBarrier : waiting, have "
          << aCompleteFFTsInBarCount.mVar
          << " of "
          << aTotalFFTsInBar
          << EndLogLine;

      barMon = aCompleteFFTsInBarCount.mVar;

      }
    else
      {
      #if !defined(FFT3D_SPINWAIT) & !defined(PK_BGL)
        // If no progress was made, sleep for a timeslice.
        usleep( 10000 );
      #endif
      }
    }

  BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER )
    << "ActiveMsgBarrier  Done! "
    << aCompleteFFTsInBarCount.mVar
    << " of " << aTotalFFTsInBar
    << EndLogLine;

  Platform::Memory::ImportFence();
  }

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::
InitHeaders()
  {

//   BegLogLine( PKFXLOG_INIT_HEADERS )
//     << "FFT3D::InitHeaders"
//     << EndLogLine;


  /*****************************************************************/
  // Z direction pregeneration
    // int PacketsToSendInSendPackets_Z = (int )  ;

  // mPregeneratedBGLTorusAndFFTHeaders_Z = new BGLTorus_FFT_Headers[ PacketsToSendInSendPackets_Z ];

  int headerCount=0;

    for(int j=0; j < LOCAL_N_Y; j++)
      {

      for(int i=0; i < LOCAL_N_X; i++)
        {
          int trgPz = i / BAR_SIZE_Z;
          int loadCountZ = 0;
          int zPointsPerPacket = POINTS_PER_PACKET_Z;
          for(int k =0; k < LOCAL_N_Z; k++)
            {
              loadCountZ++;

              if( ( k == LOCAL_N_Z-1 ) && ( loadCountZ < POINTS_PER_PACKET_Z ) )
                {
                  // Points in the last packet
                  zPointsPerPacket = LOCAL_N_Z % POINTS_PER_PACKET_Z;
                }

              if( loadCountZ == zPointsPerPacket )
                {

                  FFT_Hdr fftHdr;
                  fftHdr.This = this;
                  fftHdr.aIndex = ( i % BAR_SIZE_Z ) * LOCAL_N_Y + j;
                  fftHdr.bIndex = k+1 - loadCountZ + UniProcFilter( mMyP_z ) * LOCAL_N_Z;;
                  fftHdr.pointsPerPacket = zPointsPerPacket;

                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   mMyP_y,
                                   mIsUni ? mMyP_z : trgPz,                 // destination coordinates
                                   0,          //???      // destination Fifo Group
                                   (Bit32) PacketActorFx_Z,       // Active Packet Function matching signature above
                                   0,                     // primary argument to actor
                                   0,                     // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)


                  //Need a hint bit to send to self - Mark should help with this.
#ifdef USE_HINT_BITS
#if !defined( FORCE_SELF_SEND_HINT_BITS_Y )
                  if( trgPz == mMyP_z )
                    BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );
#else
                  if(  mMyP_y == 0 )
                    BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
                  else
                    BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
#endif
#endif
                  assert( headerCount < GEN_HEADER_SIZE_Z);
                  mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                  mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mFFT_Hdr      = fftHdr;
                  headerCount++;

                  //           BegLogLine( PKFXLOG_INIT_HEADERS )
                  //             << "FFT3D::InitHeaders"
                  //             << " Generate headersZ: headerCount: " << headerCount
                  //             << " zPointsPerPacket: " << zPointsPerPacket
                  //             << " loadCountZ: " << loadCountZ
                  //             << EndLogLine;

                  loadCountZ=0;
                }
            }
        }
      }


    //   BegLogLine( PKFXLOG_INIT_HEADERS )
    //     << "FFT3D::InitHeaders "
    //     << "GEN_HEADER_SIZE_Z: " << GEN_HEADER_SIZE_Z
    //     << " headerCount: " << headerCount
    //     << EndLogLine;

    assert( headerCount == GEN_HEADER_SIZE_Z );

    /*****************************************************************/
    // Pregenerate Y
    headerCount = 0;

    int temp1 = UniProcFilter( mMyP_y ) * LOCAL_N_Y;
    int temp2 = UniProcFilter( mMyP_x ) * BAR_SIZE_X  + temp1;
    int temp3 = temp2 + BAR_SIZE_X;

    for(int ay=0; ay < LOCAL_N_X * BAR_SIZE_Y; ay++)
      {
        for(int trgPx=0; trgPx < P_X; trgPx++)
          {
            for(int trgPy=0; trgPy < P_Y; trgPy++)
              {
                int loadCountY = 0;
                int yPointsPerPacket = POINTS_PER_PACKET_Y;
                for(int by = temp2;
                    by < temp3;
                    by ++ )
                  {
                    loadCountY++;

                    if( by == ((temp3 - temp2) - 1) && (loadCountY < POINTS_PER_PACKET_Y) )
                      yPointsPerPacket = (temp3 - temp2) % POINTS_PER_PACKET_Y;

                    if( loadCountY == yPointsPerPacket )
                      {
                        FFT_Hdr fftHdr;
                        fftHdr.This = this;
                        fftHdr.aIndex = ay;
                        fftHdr.bIndex = by - loadCountY+1;
                        fftHdr.pointsPerPacket = yPointsPerPacket;

                        _BGL_TorusPktHdr   BGLTorus_Hdr;
                        BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                         mIsUni ? mMyP_x : trgPx,
                                         mIsUni ? mMyP_y : trgPy,
                                         mMyP_z,                 // destination coordinates
                                         0,          //???      // destination Fifo Group
                                         (Bit32) PacketActorFx_Y,       // Active Packet Function matching signature above
                                         0,                     // primary argument to actor
                                         0,                     // secondary 10bit argument to actor
                                         _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

                        //Need a hint bit to send to self - Mark should help with this.
#ifdef USE_HINT_BITS
#if !defined( FORCE_SELF_SEND_HINT_BITS_Y )
                        if( trgPx == mMyP_x && trgPy == mMyP_y )
                          BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_ZP );
#else
                        if(  mMyP_y == 0 )
                          BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
                        else
                          BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
#endif
#endif

                        assert( headerCount < GEN_HEADER_SIZE_Y );
                        mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                        mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mFFT_Hdr      = fftHdr;

                        headerCount++;
                        loadCountY = 0;
                      }
                  }
              }
          }
      }
    assert( headerCount == GEN_HEADER_SIZE_Y );


    /*****************************************************************/
    // Pregenerate X
    headerCount = 0;

    temp1 = UniProcFilter( mMyP_x ) * LOCAL_N_X;
    temp2 = UniProcFilter( mMyP_z ) * BAR_SIZE_Z + temp1;
    temp3 = temp2 + BAR_SIZE_Z;

    //   int PacketsToSendInSendPackets_X = (int) ( P_Z * P_X * LOCAL_N_Z * BAR_SIZE_X *
    //                                        ( BAR_SIZE_Z+POINTS_PER_PACKET_X-1 ) / POINTS_PER_PACKET_X );

    //   //printf("PacketsToSendInSendPackets_X:: %d\n", PacketsToSendInSendPackets_X );

    //   mPregeneratedBGLTorusAndFFTHeaders_X = new BGLTorus_FFT_Headers[ PacketsToSendInSendPackets_X ];

    for(int ax=0; ax < LOCAL_N_Z * BAR_SIZE_X; ax++)
      {
        for(int trgPz=0; trgPz < P_Z; trgPz++)
          {
            for(int trgPx=0; trgPx < P_X; trgPx++)
              {
                int loadCountX=0;
                int xPointsPerPacket =  POINTS_PER_PACKET_X;
                for(int bx = temp2;
                    bx < temp3;
                    bx++ )
                  {
                    loadCountX++;

                    if( bx == ((temp3 - temp2) - 1) && (loadCountX < POINTS_PER_PACKET_X) )
                      xPointsPerPacket = (temp3 - temp2) % POINTS_PER_PACKET_X;

                    if( loadCountX == xPointsPerPacket )
                      {

                        FFT_Hdr fftHdr;
                        fftHdr.This = this;
                        fftHdr.aIndex = ax;
                        fftHdr.bIndex = bx - loadCountX + 1;
                        fftHdr.pointsPerPacket = xPointsPerPacket;
                        fftHdr.cIndex = 0;

                        _BGL_TorusPktHdr   BGLTorus_Hdr;
                        BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                         mIsUni ? mMyP_x : trgPx,
                                         mMyP_y,
                                         mIsUni ? mMyP_z : trgPz,                 // destination coordinates
                                         0,          //???      // destination Fifo Group
                                         (Bit32) PacketActorFx_X,       // Active Packet Function matching signature above
                                         0,                     // primary argument to actor
                                         0,                     // secondary 10bit argument to actor
                                         _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

                        //Need a hint bit to send to self - Mark should help with this.
#ifdef USE_HINT_BITS
#if !defined( FORCE_SELF_SEND_HINT_BITS_Y )
                        if( trgPx == mMyP_x && trgPz == mMyP_z )
                          BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
#else
                        if(  mMyP_y == 0 )
                          BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
                        else
                          BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
#endif
#endif
                        assert( headerCount < GEN_HEADER_SIZE_X );
                        mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                        mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mFFT_Hdr      = fftHdr;
                        headerCount++;
                        loadCountX = 0;
                      }
                  }
              }
          }
      }

    assert( headerCount == GEN_HEADER_SIZE_X );

    /*****************************************************************/
    // Pregenerate B
    headerCount=0;

    //   int PacketsToSendInSendPackets_B = (int) ( ( ( LOCAL_N_Z / P_Y ) * LOCAL_N_X ) *
    //                                       ( (LOCAL_N_Y * P_Y) + POINTS_PER_PACKET_B - 1) / POINTS_PER_PACKET_B ) ;

    //   mPregeneratedBGLTorusAndFFTHeaders_B = new BGLTorus_FFT_Headers[ PacketsToSendInSendPackets_B ];

    for(int a=0; a < ( LOCAL_N_Z / P_Y ) * LOCAL_N_X; a++)
      {
      int loadCountB = 0;
      int bPointsPerPacket = POINTS_PER_PACKET_B;
      for(int b=0; b < LOCAL_N_Y * P_Y; b++)
        {
          int trgPy = b / LOCAL_N_Y;

          loadCountB++;

          if( ( b == (LOCAL_N_Y * P_Y) - 1 ) && (loadCountB < POINTS_PER_PACKET_B ))
            bPointsPerPacket = ( LOCAL_N_Y * P_Y ) % POINTS_PER_PACKET_B;

          if( loadCountB == bPointsPerPacket )
            {
              FFT_Hdr fftHdr;
              fftHdr.This = this;
              fftHdr.aIndex = a % LOCAL_N_X;
              fftHdr.bIndex = b % LOCAL_N_Y - loadCountB + 1;
              fftHdr.cIndex = a / LOCAL_N_X + UniProcFilter( mMyP_y ) * BAR_SIZE_Y;
              fftHdr.pointsPerPacket = bPointsPerPacket;

              _BGL_TorusPktHdr   BGLTorus_Hdr;
              BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                               mMyP_x,
                               mIsUni ? mMyP_y : trgPy,
                               mMyP_z,                 // destination coordinates
                               0,          //???      // destination Fifo Group
                               (Bit32) PacketActorFx_B,       // Active Packet Function matching signature above
                               0,                     // primary argument to actor
                               0,                     // secondary 10bit argument to actor
                               _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

//Need a hint bit to send to self - Mark should help with this.
#ifdef USE_HINT_BITS
#if !defined( FORCE_SELF_SEND_HINT_BITS_Y )
if( trgPy == mMyP_y )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );
#else
if(  mMyP_y == 0 )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
else
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
#endif
#endif
              assert( headerCount < GEN_HEADER_SIZE_B );
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mFFT_Hdr      = fftHdr;

              headerCount++;
              loadCountB = 0;
            }
        }
    }

  assert( headerCount == GEN_HEADER_SIZE_B );
  }


template<class FFT_PLAN, int FWD_REV >
inline void FFT3D< FFT_PLAN, FWD_REV >::Reset()
  {
  mCompleteFFTsInBarCount_x.mVar = 0;
  mCompleteFFTsInBarCount_y.mVar = 0;
  mCompleteFFTsInBarCount_z.mVar = 0;

  mIOP_FFT_Compute_Done_x.mVar = 0;
  mIOP_FFT_Compute_Done_y.mVar = 0;
  mIOP_FFT_Compute_Done_z.mVar = 0;

  for(int i=0; i < TOTAL_FFTs_IN_BAR_X; i++)
    mCompletePacket_x[ i ] = 0;

  for(int i=0; i < TOTAL_FFTs_IN_BAR_Y; i++)
    mCompletePacket_y[ i ] = 0;

  for(int i=0; i < TOTAL_FFTs_IN_BAR_Z; i++)
    mCompletePacket_z[ i ] = 0;

  for(int i=0; i < LOCAL_N_X; i++)
    for(int j=0; j < LOCAL_N_Z; j++)
      mCompletePacket_b[ i ][ j ] = 0;

  for(int i=0; i < LOCAL_N_X; i++)
    mCompleteFFTsInBarCount_b[ i ].mVar = 0;

  Platform::Memory::ExportFence();
  };

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::DoFFT()
{
    BegLogLine( PKFXLOG_INIT_E )
        << "FFT::DoFFT:: Start"
        << EndLogLine;

    BegLogLine( PKFXLOG_INIT_E )
        << "FFT::DoFFT:: "
        <<" Size of barZ: " <<  BAR_SIZE_X
        <<" Size of barY: " <<  BAR_SIZE_Y
        <<" Size of barX: " <<  BAR_SIZE_Z
        << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " LOCAL_N_X=" << LOCAL_N_X
      << " LOCAL_N_Y=" << LOCAL_N_Y
      << " LOCAL_N_Z=" << LOCAL_N_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " GLOBAL_N_X=" << GLOBAL_N_X
      << " GLOBAL_N_Y=" << GLOBAL_N_Y
      << " GLOBAL_N_Z=" << GLOBAL_N_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " P_X=" << P_X
      << " P_Y=" << P_Y
      << " P_Z=" << P_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " BAR_SIZE_X=" << BAR_SIZE_X
      << " BAR_SIZE_Y=" << BAR_SIZE_Y
      << " BAR_SIZE_Z=" << BAR_SIZE_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " TOTAL_FFTs_IN_BAR_X=" << TOTAL_FFTs_IN_BAR_X
      << " TOTAL_FFTs_IN_BAR_Y=" << TOTAL_FFTs_IN_BAR_Y
      << " TOTAL_FFTs_IN_BAR_Z=" << TOTAL_FFTs_IN_BAR_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " POINTS_PER_PACKET_X=" << POINTS_PER_PACKET_X
      << " POINTS_PER_PACKET_Y=" << POINTS_PER_PACKET_Y
      << " POINTS_PER_PACKET_Z=" << POINTS_PER_PACKET_Z
      << " POINTS_PER_PACKET_B=" << POINTS_PER_PACKET_B
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mPacketsPerFFT_x=" << mPacketsPerFFT_x
      << " mPacketsPerFFT_y=" << mPacketsPerFFT_y
      << " mPacketsPerFFT_z=" << mPacketsPerFFT_z
      << " mPacketsPerFFT_b=" << mPacketsPerFFT_b
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mCompletePacket_x=" << (void *) mCompletePacket_x
      << " mCompletePacket_y=" << (void *) mCompletePacket_y
      << " mCompletePacket_z=" << (void *) mCompletePacket_z
      << " mCompletePacket_b=" << (void *) mCompletePacket_b
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mCompleteFFTsInBarCount_b=" << (void *) mCompleteFFTsInBarCount_b
      << " mCompleteFFTsInBarCount_x=" << (void *) & mCompleteFFTsInBarCount_x
      << " mCompleteFFTsInBarCount_y=" << (void *) & mCompleteFFTsInBarCount_y
      << " mCompleteFFTsInBarCount_z=" << (void *) & mCompleteFFTsInBarCount_z
      << EndLogLine;


  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mFFT_x=" << (void *) &mFFT_x
      << " mFFT_y=" << (void *) &mFFT_y
      << " mFFT_z=" << (void *) &mFFT_z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mLocalIn=" << (void *) mLocalIn
      << " mLocalOut=" << (void *) mLocalOut
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
    //<< " mGlobalIn_x=" << (void *) mGlobalIn_x
      << " mGlobalData_x=" << (void *) mGlobalData_x
    //  << " mGlobalIn_y=" << (void *) mGlobalIn_y
      << " mGlobalData_y=" << (void *) mGlobalData_y
    //  << " mGlobalIn_z=" << (void *) mGlobalIn_z
      << " mGlobalData_z=" << (void *) mGlobalData_z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mMyP_x=" << (void *)& mMyP_x
      << " mMyP_y=" << (void *)& mMyP_y
      << " mMyP_z=" << (void *)& mMyP_z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mPregeneratedBGLTorusAndFFTHeaders_Z=" << (void *) mPregeneratedBGLTorusAndFFTHeaders_Z
      << " mPregeneratedBGLTorusAndFFTHeaders_Y=" << (void *) mPregeneratedBGLTorusAndFFTHeaders_Y
      << " mPregeneratedBGLTorusAndFFTHeaders_X=" << (void *) mPregeneratedBGLTorusAndFFTHeaders_X
      << " mPregeneratedBGLTorusAndFFTHeaders_B=" << (void *) mPregeneratedBGLTorusAndFFTHeaders_B
      << EndLogLine;


  BegLogLine( PKFXLOG_INIT_E )
      << "FFT::DoFFT:: "
      << " mPacketsPerFFT_x=" << (void *) &mPacketsPerFFT_x
      << " mPacketsPerFFT_y=" << (void *) &mPacketsPerFFT_y
      << " mPacketsPerFFT_z=" << (void *) &mPacketsPerFFT_z
      << " mPacketsPerFFT_b=" << (void *) &mPacketsPerFFT_b
      << EndLogLine;


  // Start by collecting and doing a 1D FFT in the z-direction
#if PKFXLOG_DEBUG_DATA
  if( FWD_REV == FORWARD )
    {
      for(int x=0; x < LOCAL_N_Y; x++)
        for(int y=0; y < LOCAL_N_Y; y++)
          for(int z=0; z < LOCAL_N_Z;  z++)
            {
              BegLogLine( PKFXLOG_DEBUG_DATA )
                << " DoFFT()::Forward Mode"
                << " mLocalIn[" << x << "][" << y << "][" << z <<  "]"
                << mLocalIn[ x ][ y ][ z ].re << " ," << mLocalIn[ x ][ y ][ z ].im
                << EndLogLine;
            }
    }
  else
    {
      for(int x=0; x < LOCAL_N_Y; x++)
        for(int y=0; y < LOCAL_N_Y; y++)
          for(int z=0; z < LOCAL_N_Z;  z++)
            {
              BegLogLine( PKFXLOG_DEBUG_DATA )
                << " DoFFT() Reverse Mode::"
                << " mLocalOut[" << x << "][" << y << "][" << z << "]"
                << mLocalOut[ x ][ y ][ z ].re << " ," << mLocalOut[ x ][ y ][ z ].im
                    << EndLogLine;
            }
    }
#endif

  int myRank = BGLPartitionGetRankXYZ();

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before SendPackets_Z Sends "
    << EndLogLine;


  TR_SendPackets_ZStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                         "TR_SendPackets_ZStart",
                         Platform::Thread::GetId(),
                         TR_SendPackets_ZStart );

  if( FWD_REV == FORWARD )
      SendPackets_Z( mLocalIn );
  else
      SendPackets_Z( mLocalOut );

  TR_SendPackets_ZFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                         "TR_SendPackets_ZFinis",
                         Platform::Thread::GetId(),
                         TR_SendPackets_ZFinis );

  // The IO core is receiving data. When all the data is here start
  // computing the 1D FFTs
  // FUTURE:: Optimize to start computing FFTs as soon as you can

  // while( mCompleteFFTsInBarCount_z.mVar < TOTAL_FFTs_IN_BAR_Z );
  ActiveMsgBarrier( mCompleteFFTsInBarCount_z, TOTAL_FFTs_IN_BAR_Z, mPacketsPerFFT_z );


  /*************************************************************
   *   Trigger Computation in Z
   ************************************************************/
  ComputeFFTsInQueueActorPacket cqap_IOP;
  cqap_IOP.mStart = TOTAL_FFTs_IN_BAR_Z / 2;
  cqap_IOP.mCount = TOTAL_FFTs_IN_BAR_Z - cqap_IOP.mStart;
  cqap_IOP.This = this;

  _BGL_TorusPktHdr   BGLTorus_Hdr;
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,
                   0,
                   (Bit32) ComputeFFTsInQueue_Z,
                   0,
                   0,
                   _BGL_TORUS_ACTOR_BUFFERED );

  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );

  ComputeFFTsInQueueActorPacket cqap_CP;
  cqap_CP.mStart = 0;
  cqap_CP.mCount = TOTAL_FFTs_IN_BAR_Z / 2;
  cqap_CP.This = this;

  TR_Sender_Compute_Z_Start.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Z_Start",
                                   0,
                                   TR_Sender_Compute_Z_Start );

  ComputeFFTsInQueue_Z( (void *) &cqap_CP );

  TR_Sender_Compute_Z_Finis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Z_Finis",
                                   0,
                                   TR_Sender_Compute_Z_Finis );


  ActiveMsgBarrier( mIOP_FFT_Compute_Done_z, 1, 0 );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After SendPackets_Z Active Message Handlers "
    << EndLogLine;

#ifdef PKFXLOG_DEBUG_DATA
  for(int a=0; a < TOTAL_FFTs_IN_BAR_Z; a++)
    {
      for(int k=0; k < GLOBAL_N_Z; k++)
        {
          BegLogLine( PKFXLOG_DEBUG_DATA )
            << "Execute:: mGlobalData_z[" << a << "][" << k <<"]"
            << mGlobalData_z[ a ][ k ].re << " ,"
            << mGlobalData_z[ a ][ k ].im
            << EndLogLine;
        }
    }
#endif

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before SendPackets_X Sends "
    << EndLogLine;

  TR_SendPackets_XStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_XStart",
                               Platform::Thread::GetId(),
                               TR_SendPackets_XStart );

  SendPackets_X();

  TR_SendPackets_XFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_XFinis",
                               Platform::Thread::GetId(),
                               TR_SendPackets_XFinis );

  ActiveMsgBarrier( mCompleteFFTsInBarCount_x, TOTAL_FFTs_IN_BAR_X, mPacketsPerFFT_x );

  /*************************************************************
   *   Trigger Computation in X
   ************************************************************/
  cqap_IOP.mStart = TOTAL_FFTs_IN_BAR_X / 2;
  cqap_IOP.mCount = TOTAL_FFTs_IN_BAR_X - cqap_IOP.mStart;
  cqap_IOP.This = this;

  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,
                   0,
                   (Bit32) ComputeFFTsInQueue_X,
                   0,
                   0,
                   _BGL_TORUS_ACTOR_BUFFERED );

  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );

  cqap_CP.mStart = 0;
  cqap_CP.mCount = TOTAL_FFTs_IN_BAR_X / 2;
  cqap_CP.This = this;

  TR_Sender_Compute_X_Start.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_X_Start",
                                   0,
                                   TR_Sender_Compute_X_Start );

  ComputeFFTsInQueue_X( (void *) &cqap_CP );

  TR_Sender_Compute_X_Finis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_X_Finis",
                                   0,
                                   TR_Sender_Compute_X_Finis );


  ActiveMsgBarrier( mIOP_FFT_Compute_Done_x, 1, 0 );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After SendPackets_X Sends "
    << EndLogLine;


  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After SendPackets_X Active Message Handlers "
    << EndLogLine;

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

#ifdef PKFXLOG_DEBUG_DATA
  for(int a=0; a < TOTAL_FFTs_IN_BAR_X; a++)
     {
       for(int k=0; k < GLOBAL_N_X; k++)
        {
          BegLogLine( PKFXLOG_DEBUG_DATA )
            << "Execute:: mGlobalData_x[" << a << "][" << k << "]"
            << mGlobalData_x[ a ][ k ].re << " ,"
            << mGlobalData_x[ a ][ k ].im
            << EndLogLine;
        }
     }
#endif


  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before SendPackets_Y Sends "
    << EndLogLine;

  TR_SendPackets_YStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_YStart",
                               Platform::Thread::GetId(),
                               TR_SendPackets_YStart );

  SendPackets_Y();

  TR_SendPackets_YFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                               "TR_SendPackets_YFinis",
                               Platform::Thread::GetId(),
                               TR_SendPackets_YFinis );



  ActiveMsgBarrier( mCompleteFFTsInBarCount_y, TOTAL_FFTs_IN_BAR_Y, mPacketsPerFFT_y );

  /*************************************************************
   *   Trigger Computation in Y
   ************************************************************/
  cqap_IOP.mStart = TOTAL_FFTs_IN_BAR_Y / 2;
  cqap_IOP.mCount = TOTAL_FFTs_IN_BAR_Y - cqap_IOP.mStart;
  cqap_IOP.This = this;

  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   mMyP_x,
                   mMyP_y,
                   mMyP_z,
                   0,
                   (Bit32) ComputeFFTsInQueue_Y,
                   0,
                   0,
                   _BGL_TORUS_ACTOR_BUFFERED );

  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , (void *) &cqap_IOP, sizeof(ComputeFFTsInQueueActorPacket) );

  cqap_CP.mStart = 0;
  cqap_CP.mCount = TOTAL_FFTs_IN_BAR_Y / 2;
  cqap_CP.This = this;

  TR_Sender_Compute_Y_Start.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Y_Start",
                                   0,
                                   TR_Sender_Compute_Y_Start );

  ComputeFFTsInQueue_Y( (void *) &cqap_CP );

  TR_Sender_Compute_Y_Finis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                                   "TR_Sender_Compute_Y_Finis",
                                   0,
                                   TR_Sender_Compute_Y_Finis );

  ActiveMsgBarrier( mIOP_FFT_Compute_Done_y, 1, 0 );
  /*************************************************************/

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After SendPackets_Y Sends "
    << EndLogLine;

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After SendPackets_Y Active Message Handlers "
    << EndLogLine;

#ifdef PKFXLOG_DEBUG_DATA
  for(int a=0; a < TOTAL_FFTs_IN_BAR_Y; a++)
    for(int k=0; k < GLOBAL_N_Y; k++)
      {
        BegLogLine( PKFXLOG_DEBUG_DATA )
          << "Execute:: mGlobalData_y[" << a << "]["<< k << "]"
          << mGlobalData_y[ a ][ k ].re << " ,"
          << mGlobalData_y[ a ][ k ].im
          << EndLogLine;
      }
#endif

  //mPcm->TickBarrier( myRank, __LINE__ );

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before SendPackets_B Sends "
    << EndLogLine;

  TR_SendPackets_BStart.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                         "TR_SendPackets_BStart",
                         Platform::Thread::GetId(),
                         TR_SendPackets_BStart );

  if( FWD_REV == FORWARD )
      SendPackets_B( mLocalOut );
  else
      SendPackets_B( mLocalIn );

  TR_SendPackets_BFinis.HitOE( PKTRACE_TUNE & !PKTRACE_OFF,
                         "TR_SendPackets_BFinis",
                         Platform::Thread::GetId(),
                         TR_SendPackets_BFinis );


  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After SendPackets_B Sends "
    << EndLogLine;

  for(int i=0; i < LOCAL_N_X; i++)
    {
      ActiveMsgBarrier( mCompleteFFTsInBarCount_b[ i ], LOCAL_N_Z, mPacketsPerFFT_b );
    }

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After SendPackets_B Active Message Handlers "
    << EndLogLine;

  //mPcm->TickBarrier( myRank, __LINE__ );

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  #ifdef PKFXLOG_DEBUG_DATA
  if( FWD_REV == FORWARD )
    {
      for(int i=0; i < LOCAL_N_X; i++)
        {
          for(int j=0; j < LOCAL_N_Y; j++)
            {
              for(int k=0; k < LOCAL_N_Z; k++)
                {
                  //          out[ i ][ j ][ k ].re = mLocalOut[ i ][ j ][ k ].re;
                  //          out[ i ][ j ][ k ].im = mLocalOut[ i ][ j ][ k ].im;

                  BegLogLine( PKFXLOG_DEBUG_DATA )
                    << "DoFFT<FWD>:: mLocalOut[" << i << " ][" << j << "][" << k << "]"
                    << mLocalOut[ i ][ j ][ k ].re
                    << " ," << mLocalOut[ i ][ j ][ k ].im
                    << EndLogLine;
                }
            }
        }
    }
  else
    {
      for(int i=0; i < LOCAL_N_X; i++)
        {
          for(int j=0; j < LOCAL_N_Y; j++)
            {
              for(int k=0; k < LOCAL_N_Z; k++)
                {
                  //          out[ i ][ j ][ k ].re = mLocalOut[ i ][ j ][ k ].re;
                  //          out[ i ][ j ][ k ].im = mLocalOut[ i ][ j ][ k ].im;

                  BegLogLine( PKFXLOG_DEBUG_DATA )
                    << "DoFFT<REV>:: mLocalIn[" << i << " ][" << j << "][" << k << "]"
                    << mLocalIn[ i ][ j ][ k ].re
                    << " ," << mLocalIn[ i ][ j ][ k ].im
                    << EndLogLine;
                }
            }
        }
    }
  #endif

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << "This node finished - about to barrier"
    << EndLogLine;

  //mPcm->TickBarrier( myRank, __LINE__ );

  BGLPartitionBarrier();
  Reset();

  // Platform::Memory::ExportFence();

  BegLogLine( PKFXLOG_EXECUTE )
    << "DoFFT "
    << "All nodes finished and passed barrier."
    << EndLogLine;

  }


template<class FFT_PLAN, int FWD_REV >
inline void FFT3D< FFT_PLAN, FWD_REV >::
ZeroRealSpace()
  {
  for(int x=0; x < LOCAL_N_X; x++)
    for(int y=0; y < LOCAL_N_Y; y++)
      for(int z=0; z < LOCAL_N_Z; z++)
        {
        mLocalIn[ x ][ y ][ z ].re = 0.0;
        mLocalIn[ x ][ y ][ z ].im = 0.0;
        }
  }

template<class FFT_PLAN, int FWD_REV>
inline void FFT3D<FFT_PLAN, FWD_REV>::
PutRealSpaceElement( int x, int y, int z, double value)
  {
//   BegLogLine( PKFXLOG_PUT_REALSPACE )
//     << "PutRealSpaceElement: "
//     << " x=" << x
//     << " y=" << y
//     << " z=" << z
//     << " data= " << value
//     << EndLogLine;

  mLocalIn[ x ][ y ][ z ].re = value;
  mLocalIn[ x ][ y ][ z ].im = 0.0;
  }

template<class FFT_PLAN, int FWD_REV>
inline double FFT3D<FFT_PLAN, FWD_REV>::
GetRealSpaceElement( int x, int y, int z )
  {
//   BegLogLine( PKFXLOG_GET_REALSPACE )
//       << "GetRealSpaceElement: "
//       << " x=" << x
//       << " y=" << y
//       << " z=" << z
//       << " data= " << mLocalIn[ x ][ y ][ z ].re
//       << EndLogLine;

  return mLocalIn[ x ][ y ][ z ].re;
  }

template<class FFT_PLAN, int FWD_REV>
inline void FFT3D<FFT_PLAN, FWD_REV>::
PutRecipSpaceElement( int kx, int ky, int kz, fft3d_complex value )
  {
//   BegLogLine( PKFXLOG_PUT_RECIPSPACE )
//       << "PutRecipSpaceElement: "
//       << " kx=" << kx
//       << " ky=" << ky
//       << " kz=" << kz
//       << " data.re= " << value.re
//       << " data.im= " << value.im
//       << EndLogLine;

    mLocalOut[ kx ][ ky ][ kz ] = value;
  }

template<class FFT_PLAN, int FWD_REV>
inline fft3d_complex FFT3D<FFT_PLAN, FWD_REV>::
GetRecipSpaceElement( int kx, int ky, int kz )
  {

//   BegLogLine( PKFXLOG_GET_RECIPSPACE )
//       << "GetRecipSpaceElement: "
//       << " kx=" << kx
//       << " ky=" << ky
//       << " kz=" << kz
//       << " data.re= " << mLocalOut[ kx ][ ky ][ kz ].re
//       << " data.im= " << mLocalOut[ kx ][ ky ][ kz ].im
//       << EndLogLine;

  return mLocalOut[ kx ][ ky ][ kz ];
  }

template<class FFT_PLAN, int FWD_REV>
FFT3D<FFT_PLAN, FWD_REV>::
~FFT3D()
  {
  }

template<class FFT_PLAN, int FWD_REV>
inline void FFT3D<FFT_PLAN, FWD_REV>::
GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
                    int& aLocalY, int& aLocalSizeY,
                    int& aLocalZ, int& aLocalSizeZ,
                    int& aLocalRecipX, int& aLocalRecipSizeX,
                    int& aLocalRecipY, int& aLocalRecipSizeY,
                    int& aLocalRecipZ, int& aLocalRecipSizeZ )
  {

  aLocalX = mMyP_x * LOCAL_N_X;
  aLocalY = mMyP_y * LOCAL_N_Y;
  aLocalZ = mMyP_z * LOCAL_N_Z;

  aLocalSizeX = LOCAL_N_X;
  aLocalSizeY = LOCAL_N_Y;
  aLocalSizeZ = LOCAL_N_Z;

  aLocalRecipX = mMyP_x * LOCAL_N_X;
  aLocalRecipY = mMyP_y * LOCAL_N_Y;
  aLocalRecipZ = mMyP_z * LOCAL_N_Z;

  aLocalRecipSizeX = LOCAL_N_X;
  aLocalRecipSizeY = LOCAL_N_Y;
  aLocalRecipSizeZ = LOCAL_N_Z;
  }

template<class FFT_PLAN, int FWD_REV>
inline void FFT3D< FFT_PLAN, FWD_REV >::
ScaleIn( double aScaleFactor )
  {
  for(int x=0; x < LOCAL_N_X; x++)
    for(int y=0; y < LOCAL_N_Y; y++)
      for(int z=0; z < LOCAL_N_Z; z++)
        {
        mLocalIn[ x ][ y ][ z ].re = aScaleFactor * mLocalIn[ x ][ y ][ z ].re;
        mLocalIn[ x ][ y ][ z ].im = aScaleFactor * mLocalIn[ x ][ y ][ z ].im;
        }
  }


#endif
