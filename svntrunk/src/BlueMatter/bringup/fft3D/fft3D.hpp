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
  #include <BlueMatter/fft_one_dim.hpp>
  // #include <BlueMatter/pk/platform.hpp>
  #include <BlueMatter/PacketCounterManager.hpp>
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

#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG ( 1 )
#endif

#ifndef PKFXLOG_EXECUTE
#define PKFXLOG_EXECUTE ( 1 )
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
#define PKFXLOG_ACTIVE_MSG_BARRIER ( 1 )
#endif

#ifndef PKFXLOG_INIT
#define PKFXLOG_INIT ( 1 )
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
#define PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT (0)
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT
#define PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT (0)
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_X_1D_FFT
#define PKFXLOG_PACKET_ACTOR_FX_X_1D_FFT (0)
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_B_1D_FFT
#define PKFXLOG_PACKET_ACTOR_FX_B_1D_FFT (0)
#endif

#define max(i, j) ( (i>j) ? i:j )
#define min(i, j) ( (i<j) ? i:j )

template< class FFT_PLAN, int FWD_REV >
class FFT3D
  {
  public:

    typedef FFT_PLAN FFT_PLAN_IF;

    int Init();
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
    inline complex GetRecipSpaceElement( int kx, int ky, int kz );
    inline void PutRecipSpaceElement( int kx, int ky, int kz, complex );

    inline void ScaleIn(double);

    FFT3D()
    {
      // Init();
    }
    // ~FFT3D();

  private:

    struct FFT_Hdr
      {
      FFT3D *This;
      int aIndex;
      int bIndex;
      int cIndex;
      };

    typedef complex Value;

    enum
      {
      // ELEMENTS_PER_PACKET = ( (sizeof(_BGL_TorusPktPayload ) - sizeof(FFT_Hdr)) / sizeof( Value) ),
      //ELEMENTS_PER_PACKET = 8
      // magic number 8 is a work around for some bug that Alex knows about - should be computed max packet size.
      ELEMENTS_PER_PACKET = 8
      };

    enum { P_X = FFT_PLAN::P_X };
    enum { P_Y = FFT_PLAN::P_Y };
    enum { P_Z = FFT_PLAN::P_Z };

    enum { GLOBAL_N_X = FFT_PLAN::GLOBAL_SIZE_X };
    enum { GLOBAL_N_Y = FFT_PLAN::GLOBAL_SIZE_Y };
    enum { GLOBAL_N_Z = FFT_PLAN::GLOBAL_SIZE_Z };

    enum { LOCAL_N_X = GLOBAL_N_X / P_X };
    enum { LOCAL_N_Y = GLOBAL_N_Y / P_Y };
    enum { LOCAL_N_Z = GLOBAL_N_Z / P_Z };

    enum { BAR_SIZE_X = LOCAL_N_X / P_X };
    enum { BAR_SIZE_Y = LOCAL_N_Y / P_Y };
    enum { BAR_SIZE_Z = LOCAL_N_Z / P_Z };

    enum { TOTAL_FFTs_IN_BAR_Z = BAR_SIZE_Z * LOCAL_N_Y };
    enum { TOTAL_FFTs_IN_BAR_Y = BAR_SIZE_Y * LOCAL_N_X };
    enum { TOTAL_FFTs_IN_BAR_X = BAR_SIZE_X * LOCAL_N_Z };

    enum { POINTS_PER_PACKET_X = min( BAR_SIZE_Z, ELEMENTS_PER_PACKET ) };
    enum { POINTS_PER_PACKET_Y = min( BAR_SIZE_X, ELEMENTS_PER_PACKET ) };
    enum { POINTS_PER_PACKET_Z = min( LOCAL_N_Z, ELEMENTS_PER_PACKET ) };
    enum { POINTS_PER_PACKET_B = min( LOCAL_N_Y, ELEMENTS_PER_PACKET ) };

    int mCompletePacket_x[ TOTAL_FFTs_IN_BAR_X ];
    int mCompletePacket_y[ TOTAL_FFTs_IN_BAR_Y ];
    int mCompletePacket_z[ TOTAL_FFTs_IN_BAR_Z ];

    int mCompletePacket_b[ LOCAL_N_X ][ LOCAL_N_Z ];
    int mCompleteFFTsInBarCount_b[ LOCAL_N_X ];

    typedef FFTOneD< GLOBAL_N_X, FWD_REV > FFT_1D_X;
    typedef FFTOneD< GLOBAL_N_Y, FWD_REV > FFT_1D_Y;
    typedef FFTOneD< GLOBAL_N_Z, FWD_REV > FFT_1D_Z;

  //   // 1D FFTs in each direction
    FFT_1D_X mFFT_x;
    FFT_1D_X mFFT_y;
    FFT_1D_Z mFFT_z;

  public:
    complex mLocalIn [ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ];
    complex mLocalOut[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ];

  private:
    complex mGlobalIn_x [ TOTAL_FFTs_IN_BAR_X ][ FFT_PLAN::GLOBAL_SIZE_X ];
    complex mGlobalOut_x[ TOTAL_FFTs_IN_BAR_X ][ FFT_PLAN::GLOBAL_SIZE_X ];

    complex mGlobalIn_y [ TOTAL_FFTs_IN_BAR_Y ][ FFT_PLAN::GLOBAL_SIZE_Y ];
    complex mGlobalOut_y[ TOTAL_FFTs_IN_BAR_Y ][ FFT_PLAN::GLOBAL_SIZE_Y ];

    complex mGlobalIn_z [ TOTAL_FFTs_IN_BAR_Z ][ FFT_PLAN::GLOBAL_SIZE_Z ];
    complex mGlobalOut_z[ TOTAL_FFTs_IN_BAR_Z ][ FFT_PLAN::GLOBAL_SIZE_Z ];

    int mMyP_x, mMyP_y, mMyP_z;

    // communication related data struct
    struct BGLTorus_FFT_Headers
      {
      _BGL_TorusPktHdr mBGLTorus_Hdr;
      FFT_Hdr          mFFT_Hdr;
      };

    struct ActorPacket
      {
      FFT_Hdr mHdr;
      Value   mData[ ELEMENTS_PER_PACKET ];
      };

    int  mCompleteFFTsInBarCount_x;
    int  mCompleteFFTsInBarCount_y;
    int  mCompleteFFTsInBarCount_z;
    //  int* mCompleteFFTsInBarCount_b;

    enum { GEN_HEADER_SIZE_Z = ( LOCAL_N_X * LOCAL_N_Y * ( LOCAL_N_Z + POINTS_PER_PACKET_Z - 1) / POINTS_PER_PACKET_Z ) };
    enum { GEN_HEADER_SIZE_Y = ( P_X * P_Y * ( LOCAL_N_X * BAR_SIZE_Y ) *(BAR_SIZE_X + POINTS_PER_PACKET_Y - 1 ) / POINTS_PER_PACKET_Y ) };
    enum { GEN_HEADER_SIZE_X = ( P_Z * P_X * LOCAL_N_Z * BAR_SIZE_X * ( BAR_SIZE_Z+POINTS_PER_PACKET_X-1 ) / POINTS_PER_PACKET_X ) };
    enum { GEN_HEADER_SIZE_B = ((( LOCAL_N_Z / P_Y ) * LOCAL_N_X )*( (LOCAL_N_Y * P_Y) + POINTS_PER_PACKET_B - 1) / POINTS_PER_PACKET_B ) };


    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_Z[ GEN_HEADER_SIZE_Z ];
    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_Y[ GEN_HEADER_SIZE_Y ];
    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_X[ GEN_HEADER_SIZE_X ];
    BGLTorus_FFT_Headers mPregeneratedBGLTorusAndFFTHeaders_B[ GEN_HEADER_SIZE_B ];

    int mPacketsPerFFT_x;
    int mPacketsPerFFT_y;
    int mPacketsPerFFT_z;
    int mPacketsPerFFT_b;

    PacketCounterManager* mPcm;

    static int PacketActorFx_X( void* pkt );
    static int PacketActorFx_Y( void* pkt );
    static int PacketActorFx_Z( void* pkt );
    static int PacketActorFx_B( void* pkt );

    void GatherZ( complex in [ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z  ] );
    void GatherX( complex in [ TOTAL_FFTs_IN_BAR_X   ][ GLOBAL_N_X ] );
    void GatherY( complex in [ TOTAL_FFTs_IN_BAR_Y   ][ GLOBAL_N_Y ] );
    void GatherB( complex out[ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z  ] );

    void InitHeaders();


    static void ActiveMsgBarrier( int & completePencilsInBarCount,
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
    {
      BGLTorusSendPacketUnaligned( & pktHdr ,      // Previously created header to use
                                   data,     // Source address of data (16byte aligned)
                                   len ); // Payload bytes = ((chunks * 32) - 16)
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
  }

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV > :: Init()
  {

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;


  char filename[ 128 ];

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
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


  mCompleteFFTsInBarCount_x = 0;
  mCompleteFFTsInBarCount_y = 0;
  mCompleteFFTsInBarCount_z = 0;

  BegLogLine( PKFXLOG_INIT )
    << "FFT3D::Init() "
    << EndLogLine;

  mPacketsPerFFT_x = (int) ( P_X * ( LOCAL_N_X + POINTS_PER_PACKET_X - 1) / POINTS_PER_PACKET_X );
  mPacketsPerFFT_y = (int) ( P_Y * ( LOCAL_N_Y + POINTS_PER_PACKET_Y - 1) / POINTS_PER_PACKET_Y );
  mPacketsPerFFT_z = (int) ( P_Z * ( LOCAL_N_Z + POINTS_PER_PACKET_Z - 1) / POINTS_PER_PACKET_Z );
  mPacketsPerFFT_b = (int) (       ( LOCAL_N_Y + POINTS_PER_PACKET_B - 1) / POINTS_PER_PACKET_B );

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
  ActorPacket *ap = (ActorPacket*) pkt;

  FFT3D<FFT_PLAN,FWD_REV> *This = (*ap).mHdr.This;

  int myRank = BGLPartitionGetRankXYZ();
  //(*This).mPcm->TickRecv( myRank );

  int a = (*ap).mHdr.aIndex;
  int b = (*ap).mHdr.bIndex;
  int globalNz = (*This).LOCAL_N_Z * (*This).P_Z;

  int zPointsPerPacket = (*This).POINTS_PER_PACKET_Z;

  if(   (globalNz-b) % (*This).LOCAL_N_Z != 0
     && (globalNz-b) < (*This).POINTS_PER_PACKET_Z)
    {
    zPointsPerPacket = (globalNz-b) % (*This).POINTS_PER_PACKET_Z;
    }

   BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z  )
     << "PacketActorFx_Z:: zPointsPerPacket  "
     << zPointsPerPacket
     << EndLogLine;

  for(int k=0; k < zPointsPerPacket; k++)
    {
    if( (b+k) >= globalNz )
      {
      printf("====Error inside PacketActorFx_Z %d %d\n", b+k, globalNz);
      return -1;
      }

      (*This).mGlobalIn_z[ a ][ b+k ].re = (*ap).mData[ k ].re;
      (*This).mGlobalIn_z[ a ][ b+k ].im = (*ap).mData[ k ].im;

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z & PKFXLOG_DEBUG_DATA )
        << "PacketActorFx_Z:: "
        << "Recv a packet( "
        << (*This).mMyP_z << " ab " << a
        << "," << b+k << ") =  "
        << (*This).mGlobalIn_z[ a ][ b+k ].re
        << ","
        << (*This).mGlobalIn_z[ a ][ b+k ].im
        << EndLogLine;
    }

  // NOTE: IF ALL ACTIVE MESSAGES ARE HANDLED BY ON PROC, THIS IS NOT REQUIRED
  //       REMEMBER THAT THE LOCAL PROC IS
  // This patch should probably constructed otherwise...
  //Platform::Memory::ExportFence();  // THIS SHOULD REALLY BE FOR THE PACKET WE JUST GOT
  // Platform::Memory::AtomicAdd( &(*This).mCompletePacket_z[ a ], 1 );
  Platform::Memory::ImportFence();
  (*This).mCompletePacket_z[ a ]++;
  Platform::Memory::ExportFence();

  int cmpl = (*This).mCompletePacket_z[ a ];

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
    << " PacketActorFx_Z:: "
    << " (*This).mCompletePacket_z[ " << a << " ]= "
    << (*This).mCompletePacket_z[ a ]
    << " (*This).mPacketsPerFFT_z "
    << (*This).mPacketsPerFFT_z
    << EndLogLine;

  if( cmpl == (*This).mPacketsPerFFT_z )
    {

    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT  )
      << " PacketActorFx_Z:: "
      << " Got " << (*This).mPacketsPerFFT_z << " of " << cmpl
      << " DOING FFT "
      << EndLogLine;

     for(int b=0; b < zPointsPerPacket; b++)
       {
       BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z & PKFXLOG_DEBUG_DATA )
         << " PacketActorFx_Z:: "
         << "BeforeFFT "
         <<  (*This).mMyP_z << " (a,b) = " << a
         << "," << b << "  "
         << (*This).mGlobalIn_z[ a ][ b ].re << ","
         << (*This).mGlobalIn_z[ a ][ b ].im
         << EndLogLine;
       }

     ((*This).mFFT_z).fftInternal( (*This).mGlobalIn_z[ a ],
                                    (*This).mGlobalOut_z[ a ]);

     for(int b=0; b < zPointsPerPacket; b++)
       {
       BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z & PKFXLOG_DEBUG_DATA )
         << " PacketActorFx_Z:: "
         << " AfterFFT "
         <<  (*This).mMyP_z << " ab " << a
         << "," << b << "  " << "("
         << (*This).mGlobalOut_z[ a ][ b ].re << ","
         << (*This).mGlobalOut_z[ a ][ b ].im
         << EndLogLine;
       }

     Platform::Memory::ExportFence();
     (*This).mCompleteFFTsInBarCount_z += 1;
     Platform::Memory::ExportFence();

    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z_1D_FFT  )
      << "PacketActorFx_Z "
      << " Done incremented mCompleteFFTsInBarCount_z to "
      << (*This).mCompleteFFTsInBarCount_z
      << EndLogLine;
    }

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
    << "PacketActorFx_Z "
    << " mCompletePacket_z "
    << cmpl
    << "\\"
    << (*This).mPacketsPerFFT_z
    << "  mCompleteFFTsInBarCount_z is "
    << (*This).mCompleteFFTsInBarCount_z
    << EndLogLine;

  Platform::Memory::ExportFence();

  return 0;
  }

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::
GatherZ(complex in[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ])
  {
  BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
      << "Entering GatherZ"
      << EndLogLine;

  ActorPacket zap;

  // int count = 0;
  int headerCount = 0;
  for(int i=0; i < LOCAL_N_X; i++)
    {
    int trgPz = i / BAR_SIZE_Z;

    for(int j=0; j < LOCAL_N_Y; j++)
      {
      int loadCountZ = 0;
      int zPointsPerPacket = POINTS_PER_PACKET_Z;

      for(int k =0; k < LOCAL_N_Z; k++)
        {
        BegLogLine( PKFXLOG_GATHER_Z )
                << "GatherZ:: "
                << " i=" << i
                << " j=" << j
                << " k=" << k
                << " loadCountZ=" << loadCountZ
                << " zPointsPerPacket=" << zPointsPerPacket
                << EndLogLine;

        BegLogLine( PKFXLOG_GATHER_Z )
          << "GatherZ:: "
          << " in=" << (void*) in
          << EndLogLine;

        BegLogLine( PKFXLOG_GATHER_Z )
          << "GatherZ:: "
          << " in[ "<<i<<" ][ "<<j<<" ][ "<<k<<" ].re=" << in[ i ][ j ][ k ].re
          << " in[ "<<i<<" ][ "<<j<<" ][ "<<k<<" ].im=" << in[ i ][ j ][ k ].im
          << EndLogLine;


        zap.mData[ loadCountZ ].re = in[ i ][ j ][ k ].re;
        zap.mData[ loadCountZ ].im = in[ i ][ j ][ k ].im;
        loadCountZ++;

        BegLogLine( PKFXLOG_GATHER_Z )
                << "GatherZ:: "
                << " i=" << i
                << " j=" << j
                << " k=" << k
                << " loadCountZ=" << loadCountZ
                << " zPointsPerPacket=" << zPointsPerPacket
                << EndLogLine;


        if( ( k == LOCAL_N_Z-1 ) && ( loadCountZ < POINTS_PER_PACKET_Z ) )
          {
          // Points in the last packet
          zPointsPerPacket = LOCAL_N_Z % POINTS_PER_PACKET_Z;
          }

        if( loadCountZ == zPointsPerPacket )
          {

          for(int kk=0; kk < zPointsPerPacket; kk++)
            {
            BegLogLine( PKFXLOG_GATHER_Z & PKFXLOG_DEBUG_DATA )
              << "GatherZ::"
              << "Sending a packet to { " << mMyP_x << " , " << mMyP_y << " , " << trgPz << " }"
              <<  " (a,b) " << (i % BAR_SIZE_Z) * LOCAL_N_Y + j
              << "," << k+1 - loadCountZ + mMyP_z * LOCAL_N_Z <<" data( "
              << zap.mData[ kk ].re << ","
              << zap.mData[ kk ].im << ")"
              << EndLogLine;
            }

          zap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mFFT_Hdr;

          int myRank = BGLPartitionGetRankXYZ();
          //mPcm->TickSend( myRank );

          BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mBGLTorus_Hdr,
                                          (Bit32) PacketActorFx_Z,
                                          mMyP_x,
                                          mMyP_y,
                                          trgPz,
                                          sizeof( zap ),
                                          &zap);
          headerCount++;
          loadCountZ = 0;
          }
        }
      }
    }
  BegLogLine( PKFXLOG_GATHER_Z || PKFXLOG_METHOD_ENTER_EXIT )
      << "Exiting GatherZ"
      << EndLogLine;

  }

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::
PacketActorFx_X(void* pkt)
  {
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D< FFT_PLAN, FWD_REV > *This = (*ap).mHdr.This;

  int myRank = BGLPartitionGetRankXYZ();
  //(*This).mPcm->TickRecv( myRank );

  int a = (*ap).mHdr.aIndex;
  int b = (*ap).mHdr.bIndex;

   BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X )
     << "PacketActorFx_X:: "
     << "Before the Increment the packet-counterX[ " << a << "]="
     << (*This).mCompletePacket_x[ a ]
     << EndLogLine;

  int globalNx = (*This).LOCAL_N_X * (*This).P_X;
  int xPointsPerPacket = (*This).POINTS_PER_PACKET_X;
  if((globalNx-b) % (*This).LOCAL_N_X !=0
     && (globalNx-b) < (*This).POINTS_PER_PACKET_X)
    {
    xPointsPerPacket = (globalNx-b) % (*This).POINTS_PER_PACKET_X;
    }

  for(int i=0; i < xPointsPerPacket; i++)
    {
    if( (b+i) == globalNx )
      {
      printf( "XPacket::wrong index %d , %d\n" , b+i, (*This).LOCAL_N_X );
      return -1;
      }

    (*This).mGlobalIn_x[ a ][ b+i ] = (*ap).mData[ i ];

    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X  & PKFXLOG_DEBUG_DATA )
      << "PacketActorFx_X:: "
      << "mGlobalIn_x[" << a << " ][" << b+i << "] ="
      << (*This).mGlobalIn_x[ a ][ b+i ].re << " , "
      << (*This).mGlobalIn_x[ a ][ b+i ].im << " ) "
      << EndLogLine;

    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X  & PKFXLOG_DEBUG_DATA )
      << "PacketActorFx_X:: "
      << "xCompletePacket[ " << a << "]"
      << ((*This).mCompletePacket_x[ a ]) << "  "
      << (*This).mPacketsPerFFT_x
      << EndLogLine;

    }

  //  Platform::Memory::ImportFence();
  Platform::Memory::ImportFence();
  (*This).mCompletePacket_x[ a ] ++;// (*This).POINTS_PER_PACKET_X;
  Platform::Memory::ExportFence();

  int cmpl = ((*This).mCompletePacket_x[ a ]);

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X )
    << " PacketActorFx_x:: "
    << " (*This).mCompletePacket_x[ " << a << " ]= "
    << (*This).mCompletePacket_x[ a ]
    << " (*This).mPacketsPerFFT_x "
    << (*This).mPacketsPerFFT_x
    << EndLogLine;


  if( cmpl == (*This).mPacketsPerFFT_x )
    {
    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT  )
      << " PacketActorFx_X:: "
      << " Got " << (*This).mPacketsPerFFT_x << " of " << cmpl
      << " DOING FFT "
      << EndLogLine;

    ((*This).mFFT_x).fftInternal( (*This).mGlobalIn_x[ a ],
                                   (*This).mGlobalOut_x[ a ]);

     Platform::Memory::ExportFence();
    (*This).mCompleteFFTsInBarCount_x++;
    Platform::Memory::ExportFence();

    BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X_1D_FFT )
      << "PacketActorFx_X:: "
      << "After the Increment the fft-counterX = "
      << (*This).mCompleteFFTsInBarCount_x
      << EndLogLine;
    }

  return 0;
  }

template<class FFT_PLAN, int FWD_REV>
void FFT3D< FFT_PLAN, FWD_REV >::
GatherX(complex in[ TOTAL_FFTs_IN_BAR_X ][ GLOBAL_N_X ])
  {
  ActorPacket xap;

  int loadCountX;
  int headerCount=0;

  for(int trgPz=0; trgPz < P_Z; trgPz++)
    {
      for(int trgPx=0; trgPx < P_X; trgPx++)
        {
          for(int ax=0; ax < LOCAL_N_Z * BAR_SIZE_X; ax++)
            {
              loadCountX=0;
              for(int bx = mMyP_z * BAR_SIZE_Z + mMyP_x * LOCAL_N_X;
                      bx < ( mMyP_z+1 ) * BAR_SIZE_Z + mMyP_x * LOCAL_N_X;
                      bx++)
                  {
                    int az = ((bx%LOCAL_N_X)%BAR_SIZE_Z)*LOCAL_N_Y+ax/LOCAL_N_Z + trgPx*BAR_SIZE_X;
                    int bz = ax%LOCAL_N_Z + LOCAL_N_Z*trgPz;

                     BegLogLine( PKFXLOG_GATHER_X  )
                       << "GatherX:: "
                       << "ax = "<< ax << "bx = "<< bx
                       << "az = "<< az << " bz = "<< bz
                       << EndLogLine;

                     BegLogLine( PKFXLOG_GATHER_X & PKFXLOG_DEBUG_DATA )
                     //BegLogLine( 1 )
                       << "GatherX:: "
                       << "mGlobalOut_ztoX[" << az << "][" << bz << "]="
                       << mGlobalOut_z[ az ][ bz ].re <<","
                       << mGlobalOut_z[ az ][ bz ].im
                       << EndLogLine;

                    xap.mData[ loadCountX ].re = mGlobalOut_z[ az ][ bz ].re;
                    xap.mData[ loadCountX ].im = mGlobalOut_z[ az ][ bz ].im;
                    loadCountX++;

                    int xPointsPerPacket =  POINTS_PER_PACKET_X;

                if( ( bx == (mMyP_z + 1) * BAR_SIZE_Z + mMyP_x * LOCAL_N_X-1 )
                   && (loadCountX<POINTS_PER_PACKET_X))
                  {
                    xPointsPerPacket =
                      ( (mMyP_z+1) * BAR_SIZE_Z + mMyP_x * LOCAL_N_X ) % POINTS_PER_PACKET_X;
                  }

                if( loadCountX == xPointsPerPacket )
                  {
//                  xap.mHdr.This = this;
//                  xap.mHdr.aIndex = ax;
//                  xap.mHdr.bIndex = bx - loadCountX + 1;
//                  xap.mHdr.cIndex = 0;

                    for(int kk=0; kk < xPointsPerPacket; kk++)
                      BegLogLine( PKFXLOG_GATHER_X & PKFXLOG_DEBUG_DATA )
                      // BegLogLine( 1 )
                        << "GatherX::"
                        << "Sending a packet to { " << trgPx << " , " << mMyP_y << " , " << trgPz << " }"
                        << " data( "
                        << xap.mData[ kk ].re << ","
                        << xap.mData[ kk ].im << ")"
                        << EndLogLine;

                    xap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mFFT_Hdr;


                    int myRank = BGLPartitionGetRankXYZ();
                    //mPcm->TickSend( myRank );

                    BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mBGLTorus_Hdr,
                                                    (Bit32) PacketActorFx_X,
                                                    trgPx,
                                                    mMyP_y,
                                                    trgPz,
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
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D<FFT_PLAN, FWD_REV> *This = (*ap).mHdr.This;

  int myRank = BGLPartitionGetRankXYZ();
  //(*This).mPcm->TickRecv( myRank );

  int a = (*ap).mHdr.aIndex;
  int b = (*ap).mHdr.bIndex;

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y )
    << "PacketActorFx_Y:: "
    << "Before the Increment the packet-counter[ " << a << "]="
    << (*This).mCompletePacket_y[ a ]
    << "After the Increment the packet-counter[ " << a << "]="
    << (*This).mCompletePacket_y[ a ]
    << EndLogLine;


  for(int i=0;  i<(*This).POINTS_PER_PACKET_Y; i++)
    {
      if( b + i == (*This).LOCAL_N_Y * (*This).P_Y)
          break;

      (*This).mGlobalIn_y[ a ][ b+i ] = (*ap).mData[ i ];

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y )
        << "PacketActorFx_Y:: "
        << "Recv a packet( "
        << (*This).mMyP_y << " ab " << a
        << "," << b+i << ") =  "
        << (*This).mGlobalIn_y[ a ][ b+i ].re
        << ","
        << (*This).mGlobalIn_y[ a ][ b+i ].im
        << EndLogLine;
    }

  Platform::Memory::ImportFence();
  (*This).mCompletePacket_y[a] ++; // (*This).POINTS_PER_PACKET_X;
  Platform::Memory::ExportFence();

  int cmpl = ((*This).mCompletePacket_y[a]);

  BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y )
    << " PacketActorFx_Y:: "
    << " (*This).mCompletePacket_y[ " << a << " ]= "
    << (*This).mCompletePacket_y[ a ]
    << " (*This).mPacketsPerFFT_y "
    << (*This).mPacketsPerFFT_y
    << EndLogLine;


  if(cmpl == (*This).mPacketsPerFFT_y )
    {

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT  )
        << " PacketActorFx_Y:: "
        << " Got " << (*This).mPacketsPerFFT_y << " of " << cmpl
        << " DOING FFT "
        << EndLogLine;

      int yPointsPerPacket = (*This).POINTS_PER_PACKET_Y;
       for(int b=0; b < yPointsPerPacket; b++)
         {
           BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y & PKFXLOG_DEBUG_DATA )
             << " PacketActorFx_Y:: "
             << "BeforeFFT "
             <<  (*This).mMyP_y << " (a,b) = " << a
             << "," << b << "  "
             << (*This).mGlobalIn_y[ a ][ b ].re << ","
             << (*This).mGlobalIn_y[ a ][ b ].im
             << EndLogLine;
        }

      ((*This).mFFT_y).fftInternal((*This).mGlobalIn_y[ a ],
                                    (*This).mGlobalOut_y[ a ]);

      for(int b=0; b < yPointsPerPacket; b++)
        {
          BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y & PKFXLOG_DEBUG_DATA )
            << " PacketActorFx_Y:: "
            << " AfterFFT "
            <<  (*This).mMyP_y << " ab " << a
            << "," << b << "  " << "("
            << (*This).mGlobalIn_y[ a ][ b ].re << ","
            << (*This).mGlobalIn_y[ a ][ b ].im
            << EndLogLine;
        }

      Platform::Memory::ExportFence();
      (*This).mCompleteFFTsInBarCount_y++;
      Platform::Memory::ExportFence();

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y_1D_FFT )
        << "PacketActorFx_Y:: "
        << " Done incremented mCompleteFFTsInBarCount_y to "
        << (*This).mCompleteFFTsInBarCount_y
        << EndLogLine;
    }

  return 0;
  }

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::
GatherY(complex  in[ TOTAL_FFTs_IN_BAR_Y ][ GLOBAL_N_Y ])
  {
  ActorPacket yap;
  int loadCountY;
  int headerCount=0;

  for(int trgPx=0; trgPx < P_X; trgPx++)
    {
      for(int trgPy=0; trgPy < P_Y; trgPy++)
        {
          for(int ay=0; ay < LOCAL_N_X * BAR_SIZE_Y; ay++)
            {
              loadCountY = 0;
              for(int by = mMyP_x * BAR_SIZE_X  + mMyP_y * LOCAL_N_Y;
                      by < (mMyP_x+1) * BAR_SIZE_X + mMyP_y * LOCAL_N_Y;
                      by ++ )
                {
                  int ax = ( ( by % LOCAL_N_Y ) % BAR_SIZE_X ) * LOCAL_N_Z +
                      ay / LOCAL_N_X + trgPy * BAR_SIZE_Y;

                  int bx = ay % LOCAL_N_X + LOCAL_N_X * trgPx;

                  yap.mData[ loadCountY ].re = mGlobalOut_x[ ax ][ bx ].re;
                  yap.mData[ loadCountY ].im = mGlobalOut_x[ ax ][ bx ].im;
                  loadCountY++;

                   BegLogLine( PKFXLOG_GATHER_Y & PKFXLOG_DEBUG_DATA )
                     << "GatherY::"
                     << "ax = "<< ax << " bx = "<< bx
                     << "ay = "<< ay << " bz = "<< by
                     << "mGlobalOut_xtoY[" << ax << "][" << bx << "]"
                     << mGlobalOut_x[ ax ][ bx ].re <<","
                     << mGlobalOut_x[ ax ][ bx ].im
                     << EndLogLine;

//                    BegLogLine( 0 )
//                      << "GatherY::POINTS_PER_PACKET_Y "
//                      << POINTS_PER_PACKET_Y << " , "
//                      << loadCountY << "  , "
//                      << "barSzY " << BAR_SIZE_Y
//                      << EndLogLine;

                  if( loadCountY == POINTS_PER_PACKET_Y )
                    {
//                       yap.mHdr.This = this;
//                       yap.mHdr.aIndex = ay;
//                       yap.mHdr.bIndex = by - loadCountY+1;

                       for(int kk=0; kk < POINTS_PER_PACKET_Y; kk++)
                           BegLogLine( PKFXLOG_GATHER_Y )
                               << "GatherY::"
                               << "Sending a packet to procXY  " << trgPx << ","
                               << trgPy << " ab " << ay << "," << by-loadCountY+1
                               << "data ( "
                               << yap.mData[ kk ].re << "," << yap.mData[ kk ].im << " )"
                               << EndLogLine;

                       yap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mFFT_Hdr;

                       int myRank = BGLPartitionGetRankXYZ();
                       //mPcm->TickSend( myRank );

                       BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mBGLTorus_Hdr,
                                                       (Bit32) PacketActorFx_Y,
                                                       trgPx,
                                                       trgPy,
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

  int myRank = BGLPartitionGetRankXYZ();
  //(*This).mPcm->TickRecv( myRank );

  int i = (*ap).mHdr.aIndex;
  int j = (*ap).mHdr.bIndex;
  int k = (*ap).mHdr.cIndex;

  for(int jp=0;  jp < (*This).POINTS_PER_PACKET_B; jp++)
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

  Platform::Memory::ImportFence();
  ( (*This).mCompletePacket_b[ i ][ k ] )++;// (*This).POINTS_PER_PACKET_X;
  Platform::Memory::ExportFence();

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
      Platform::Memory::ExportFence();
      (*This).mCompleteFFTsInBarCount_b[ i ]++;
      Platform::Memory::ExportFence();

       BegLogLine( PKFXLOG_PACKET_ACTOR_FX_B_1D_FFT )
         << "PacketActorFx_B:: "
         << "bCompleteFFTs[" << i << "]="
         << (*This).mCompleteFFTsInBarCount_b[ i ]
         << EndLogLine;
    }
  return 0;
  }

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::
GatherB(complex out[ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z ])
  {
  ActorPacket bap;
  int headerCount=0;

  for(int a=0; a < ( LOCAL_N_Z / P_Y ) * LOCAL_N_X; a++)
    {
      int loadCount = 0;
      // int i = a % LOCAL_N_X;
      // int k = a / LOCAL_N_X + mMyP_y * BAR_SIZE_Y;

      for(int b=0; b < LOCAL_N_Y * P_Y; b++)
        {
            // int j = b % LOCAL_N_Y;
          int trgPy = b / LOCAL_N_Y;

          bap.mData[ loadCount ].re = mGlobalOut_y[ a ][ b ].re;
          bap.mData[ loadCount ].im = mGlobalOut_y[ a ][ b ].im;
          loadCount++;

          if( loadCount == POINTS_PER_PACKET_B )
            {
              bap.mHdr.This = this;
              bap.mHdr.aIndex = a % LOCAL_N_X;
              bap.mHdr.bIndex = b % LOCAL_N_Y - loadCount+1;
              bap.mHdr.cIndex = a / LOCAL_N_X + mMyP_y * BAR_SIZE_Y;

              BegLogLine( PKFXLOG_GATHER_B )
                << "GatherB::POINTS_PER_PACKET_B "
                << POINTS_PER_PACKET_B << " , "
                << loadCount << "  , "
                << "localNy" <<LOCAL_N_Y
                << EndLogLine;

              bap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mFFT_Hdr;

              int myRank = BGLPartitionGetRankXYZ();
              //mPcm->TickSend( myRank );

              BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr,
                                              (Bit32) PacketActorFx_B,
                                              mMyP_x,
                                              trgPy,
                                              mMyP_z,
                                              sizeof( bap ),
                                             &bap );
              loadCount = 0;
              headerCount++;
            }
        }
    }
  }


template<class FFT_PLAN, int FWD_REV >
void FFT3D<FFT_PLAN, FWD_REV>::
ActiveMsgBarrier(int &aCompleteFFTsInBarCount,
                 int aTotalFFTsInBar,
                 int aPacketsPerBar )
  {

  BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER )
    << "ActiveMsgBarrier:: "
    << "ActiveMsgBarrier BEGIN, have " << aCompleteFFTsInBarCount
    << " of " << aTotalFFTsInBar
    << EndLogLine;

  // BarMon is a monitor of progress in filling in the bar.
  // BarMon loosly tracks this progress.
  int barMon = aCompleteFFTsInBarCount ;

  while( aCompleteFFTsInBarCount < aTotalFFTsInBar )
    {
    Platform::Memory::ImportFence();

//     #ifndef USE_2_CORES_FOR_FFT
//       int count1 = 1;
//       BGLTorusWaitCount( &count1 );
//     #endif

    if( barMon != aCompleteFFTsInBarCount )
      {
      BegLogLine( 0 )
          << "ActiveMsgBarrier:: "
          <<" ActiveMsgBarrier : waiting, have "
          << aCompleteFFTsInBarCount
          << " of "
          << aTotalFFTsInBar
          << EndLogLine;

      barMon = aCompleteFFTsInBarCount;

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
    << aCompleteFFTsInBarCount
    << " of " << aTotalFFTsInBar
    << EndLogLine;
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
    // int PacketsToSendInGatherZ = (int )  ;

  // mPregeneratedBGLTorusAndFFTHeaders_Z = new BGLTorus_FFT_Headers[ PacketsToSendInGatherZ ];

  int headerCount=0;
  for(int i=0; i < LOCAL_N_X; i++)
    {
    int trgPz = i / BAR_SIZE_Z;

    for(int j=0; j < LOCAL_N_Y; j++)
      {
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
          fftHdr.bIndex = k+1 - loadCountZ + mMyP_z * LOCAL_N_Z;;

          _BGL_TorusPktHdr   BGLTorus_Hdr;
          BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                           mMyP_x,
                           mMyP_y,
                           trgPz,                 // destination coordinates
                           0,          //???      // destination Fifo Group
                           (Bit32) PacketActorFx_Z,       // Active Packet Function matching signature above
                           0,                     // primary argument to actor
                           0,                     // secondary 10bit argument to actor
                           _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)


//Need a hint bit to send to self - Mark should help with this.
#if 0
if( trgPz == mMyP_z )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );
#else
if(  mMyP_y == 0 )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
else
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
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

  int temp1 = mMyP_y * LOCAL_N_Y;
  int temp2 = mMyP_x * BAR_SIZE_X  + temp1;
  int temp3 = temp2 + BAR_SIZE_X;

//   int PacketsToSendInGatherY = (int) ( P_X * P_Y * ( LOCAL_N_X * BAR_SIZE_Y ) *
//                                        (BAR_SIZE_X + POINTS_PER_PACKET_Y - 1 ) / POINTS_PER_PACKET_Y ) ;

  //printf("PacketsToSendInGatherY:: %d\n", PacketsToSendInGatherY );

  // mPregeneratedBGLTorusAndFFTHeaders_Y = new BGLTorus_FFT_Headers[ PacketsToSendInGatherY ];


  for(int trgPx=0; trgPx < P_X; trgPx++)
    {
      for(int trgPy=0; trgPy < P_Y; trgPy++)
        {
          for(int ay=0; ay < LOCAL_N_X * BAR_SIZE_Y; ay++)
            {
              int loadCountY = 0;
              for(int by = temp2;
                      by < temp3;
                      by ++ )
                {
//                   int ax = ( ( by % LOCAL_N_Y ) % BAR_SIZE_X ) * LOCAL_N_Z +
//                       ay / LOCAL_N_X + trgPy * BAR_SIZE_Y;

//                   int bx = ay % LOCAL_N_X + LOCAL_N_X * trgPx;

//                   yap.mData[ loadCountY ].re = mGlobalOut_x[ ax ][ bx ].re;
//                   yap.mData[ loadCountY ].im = mGlobalOut_x[ ax ][ bx ].im;
                  loadCountY++;

                  if( loadCountY == POINTS_PER_PACKET_Y )
                    {
//                       yap.mHdr.This = this;
//                       yap.mHdr.aIndex = ay;
//                       yap.mHdr.bIndex = by - loadCountY+1;

//                       BGLTorusSimpleActivePacketSendTemp( PacketActorFx_Y,
//                                                           trgPx,
//                                                           trgPy,
//                                                           mMyP_z,
//                                                           sizeof( yap ),
//                                                           (void *) &yap );
                      FFT_Hdr fftHdr;
                      fftHdr.This = this;
                      fftHdr.aIndex = ay;
                      fftHdr.bIndex = by - loadCountY+1;

                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       trgPx,
                                       trgPy,
                                       mMyP_z,                 // destination coordinates
                                       0,          //???      // destination Fifo Group
                                       (Bit32) PacketActorFx_Y,       // Active Packet Function matching signature above
                                       0,                     // primary argument to actor
                                       0,                     // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

//Need a hint bit to send to self - Mark should help with this.
#if 0
if( trgPx == mMyP_x && trgPy == mMyP_y )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_ZP );
#else
if(  mMyP_y == 0 )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
else
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
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

  temp1 = mMyP_x * LOCAL_N_X;
  temp2 = mMyP_z * BAR_SIZE_Z + temp1;
  temp3 = temp2 + BAR_SIZE_Z;

//   int PacketsToSendInGatherX = (int) ( P_Z * P_X * LOCAL_N_Z * BAR_SIZE_X *
//                                        ( BAR_SIZE_Z+POINTS_PER_PACKET_X-1 ) / POINTS_PER_PACKET_X );

//   //printf("PacketsToSendInGatherX:: %d\n", PacketsToSendInGatherX );

//   mPregeneratedBGLTorusAndFFTHeaders_X = new BGLTorus_FFT_Headers[ PacketsToSendInGatherX ];

  for(int trgPz=0; trgPz < P_Z; trgPz++)
    {
      for(int trgPx=0; trgPx < P_X; trgPx++)
        {
          for(int ax=0; ax < LOCAL_N_Z * BAR_SIZE_X; ax++)
            {
              int loadCountX=0;
              for(int bx = temp2;
                      bx < temp3;
                      bx++ )
                {
                  loadCountX++;
                  int xPointsPerPacket =  POINTS_PER_PACKET_X;

                  if( ( bx == (mMyP_z + 1) * BAR_SIZE_Z + mMyP_x * LOCAL_N_X-1 )
                      && (loadCountX<POINTS_PER_PACKET_X))
                    {
                      xPointsPerPacket =
                        ( (mMyP_z+1) * BAR_SIZE_Z + mMyP_x * LOCAL_N_X ) % POINTS_PER_PACKET_X;
                    }

                  if( loadCountX == xPointsPerPacket )
                    {
//                       xap.mHdr.This = this;
//                       xap.mHdr.aIndex = ax;
//                       xap.mHdr.bIndex = bx - loadCountX + 1;
//                       xap.mHdr.cIndex = 0;

//                       BGLTorusSimpleActivePacketSendTemp( PacketActorFx_X,
//                                                           trgPx,
//                                                           mMyP_y,
//                                                           trgPz,
//                                                           sizeof( xap ),
//                                                           (void *) &xap );

                      FFT_Hdr fftHdr;
                      fftHdr.This = this;
                      fftHdr.aIndex = ax;
                      fftHdr.bIndex = bx - loadCountX + 1;
                      fftHdr.cIndex = 0;

                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       trgPx,
                                       mMyP_y,
                                       trgPz,                 // destination coordinates
                                       0,          //???      // destination Fifo Group
                                       (Bit32) PacketActorFx_X,       // Active Packet Function matching signature above
                                       0,                     // primary argument to actor
                                       0,                     // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

//Need a hint bit to send to self - Mark should help with this.
#if 0
if( trgPx == mMyP_x && trgPz == mMyP_z )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
#else
if(  mMyP_y == 0 )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
else
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
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

//   int PacketsToSendInGatherB = (int) ( ( ( LOCAL_N_Z / P_Y ) * LOCAL_N_X ) *
//                                       ( (LOCAL_N_Y * P_Y) + POINTS_PER_PACKET_B - 1) / POINTS_PER_PACKET_B ) ;

//   mPregeneratedBGLTorusAndFFTHeaders_B = new BGLTorus_FFT_Headers[ PacketsToSendInGatherB ];

  for(int a=0; a < ( LOCAL_N_Z / P_Y ) * LOCAL_N_X; a++)
    {
      int loadCount = 0;

      for(int b=0; b < LOCAL_N_Y * P_Y; b++)
        {
          int trgPy = b / LOCAL_N_Y;

          loadCount++;

          if( loadCount == POINTS_PER_PACKET_B )
            {

              FFT_Hdr fftHdr;
              fftHdr.This = this;
              fftHdr.aIndex = a % LOCAL_N_X;
              fftHdr.bIndex = b % LOCAL_N_Y - loadCount + 1;
              fftHdr.cIndex = a / LOCAL_N_X + mMyP_y * BAR_SIZE_Y;

              _BGL_TorusPktHdr   BGLTorus_Hdr;
              BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                               mMyP_x,
                               trgPy,
                               mMyP_z,                 // destination coordinates
                               0,          //???      // destination Fifo Group
                               (Bit32) PacketActorFx_B,       // Active Packet Function matching signature above
                               0,                     // primary argument to actor
                               0,                     // secondary 10bit argument to actor
                               _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

//Need a hint bit to send to self - Mark should help with this.
#if 0
if( trgPy == mMyP_y )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );
#else
if(  mMyP_y == 0 )
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YP );
else
  BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_YM );
#endif

              assert( headerCount < GEN_HEADER_SIZE_B );
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mFFT_Hdr      = fftHdr;

              headerCount++;
              loadCount = 0;
            }
        }
    }

  assert( headerCount == GEN_HEADER_SIZE_B );
  }


template<class FFT_PLAN, int FWD_REV >
inline void FFT3D< FFT_PLAN, FWD_REV >::Reset()
  {
  mCompleteFFTsInBarCount_x = 0;
  mCompleteFFTsInBarCount_y = 0;
  mCompleteFFTsInBarCount_z = 0;

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
    mCompleteFFTsInBarCount_b[ i ] = 0;

  };

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::DoFFT()
{
    BegLogLine( PKFXLOG_INIT )
        << "FFT::DoFFT:: Start"
        << EndLogLine;


    BegLogLine( PKFXLOG_INIT )
        << "FFT::DoFFT:: "
        <<" Size of barZ: " <<  BAR_SIZE_X
        <<" Size of barY: " <<  BAR_SIZE_Y
        <<" Size of barX: " <<  BAR_SIZE_Z
        << EndLogLine;

  BegLogLine( PKFXLOG_INIT )
      << "FFT::DoFFT:: "
      << " LOCAL_N_X=" << LOCAL_N_X
      << " LOCAL_N_Y=" << LOCAL_N_Y
      << " LOCAL_N_Z=" << LOCAL_N_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT )
      << "FFT::DoFFT:: "
      << " GLOBAL_N_X=" << GLOBAL_N_X
      << " GLOBAL_N_Y=" << GLOBAL_N_Y
      << " GLOBAL_N_Z=" << GLOBAL_N_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT )
      << "FFT::DoFFT:: "
      << " P_X=" << P_X
      << " P_Y=" << P_Y
      << " P_Z=" << P_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT )
      << "FFT::DoFFT:: "
      << " BAR_SIZE_X=" << BAR_SIZE_X
      << " BAR_SIZE_Y=" << BAR_SIZE_Y
      << " BAR_SIZE_Z=" << BAR_SIZE_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT )
      << "FFT::DoFFT:: "
      << " TOTAL_FFTs_IN_BAR_X=" << TOTAL_FFTs_IN_BAR_X
      << " TOTAL_FFTs_IN_BAR_Y=" << TOTAL_FFTs_IN_BAR_Y
      << " TOTAL_FFTs_IN_BAR_Z=" << TOTAL_FFTs_IN_BAR_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT )
      << "FFT::DoFFT:: "
      << " POINTS_PER_PACKET_X=" << POINTS_PER_PACKET_X
      << " POINTS_PER_PACKET_Y=" << POINTS_PER_PACKET_Y
      << " POINTS_PER_PACKET_Z=" << POINTS_PER_PACKET_Z
      << " POINTS_PER_PACKET_B=" << POINTS_PER_PACKET_B
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT )
      << "FFT::DoFFT:: "
      << " mPacketsPerFFT_x=" << mPacketsPerFFT_x
      << " mPacketsPerFFT_y=" << mPacketsPerFFT_y
      << " mPacketsPerFFT_z=" << mPacketsPerFFT_z
      << " mPacketsPerFFT_b=" << mPacketsPerFFT_b
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

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before GatherZ Sends "
    << EndLogLine;

  if( FWD_REV == FORWARD )
      GatherZ( mLocalIn );
  else
      GatherZ( mLocalOut );

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherZ Sends "
    << EndLogLine;

  ActiveMsgBarrier( mCompleteFFTsInBarCount_z, TOTAL_FFTs_IN_BAR_Z, mPacketsPerFFT_z );
  mCompleteFFTsInBarCount_z = 0;


  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherZ Active Message Handlers "
    << EndLogLine;

#ifdef PKFXLOG_DEBUG_DATA
  for(int a=0; a < TOTAL_FFTs_IN_BAR_Z; a++)
    {
      for(int k=0; k < GLOBAL_N_Z; k++)
        {
          BegLogLine( PKFXLOG_DEBUG_DATA )
            << "Execute:: mGlobalOut_z[" << a << "][" << k <<"]"
            << mGlobalOut_z[ a ][ k ].re << " ,"
            << mGlobalOut_z[ a ][ k ].im
            << EndLogLine;
        }
    }
#endif

  //mPcm->TickBarrier( myRank, __LINE__ );

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before GatherX Sends "
    << EndLogLine;

  GatherX( mGlobalIn_x );

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherX Sends "
    << EndLogLine;

  ActiveMsgBarrier( mCompleteFFTsInBarCount_x, TOTAL_FFTs_IN_BAR_X, mPacketsPerFFT_x );
  mCompleteFFTsInBarCount_x = 0;

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherX Active Message Handlers "
    << EndLogLine;

  //mPcm->TickBarrier( myRank, __LINE__ );

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

#ifdef PKFXLOG_DEBUG_DATA
  for(int a=0; a < TOTAL_FFTs_IN_BAR_X; a++)
     {
       for(int k=0; k < GLOBAL_N_X; k++)
        {
          BegLogLine( PKFXLOG_DEBUG_DATA )
            << "Execute:: mGlobalOut_x[" << a << "][" << k << "]"
            << mGlobalOut_x[ a ][ k ].re << " ,"
            << mGlobalOut_x[ a ][ k ].im
            << EndLogLine;
        }
     }
#endif

  //mPcm->TickBarrier( myRank, __LINE__ );

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before GatherY Sends "
    << EndLogLine;

  GatherY( mGlobalIn_y );

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherY Sends "
    << EndLogLine;

  ActiveMsgBarrier( mCompleteFFTsInBarCount_y, TOTAL_FFTs_IN_BAR_Y, mPacketsPerFFT_y );
  mCompleteFFTsInBarCount_y = 0;

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherY Active Message Handlers "
    << EndLogLine;

#ifdef PKFXLOG_DEBUG_DATA
  for(int a=0; a < TOTAL_FFTs_IN_BAR_Y; a++)
    for(int k=0; k < GLOBAL_N_Y; k++)
      {
        BegLogLine( PKFXLOG_DEBUG_DATA )
          << "Execute:: mGlobalOut_y[" << a << "]["<< k << "]"
          << mGlobalOut_y[ a ][ k ].re << " ,"
          << mGlobalOut_y[ a ][ k ].im
          << EndLogLine;
      }
#endif

  //mPcm->TickBarrier( myRank, __LINE__ );

  #ifdef FFT3D_BARRIER_PHASES
    BGLPartitionBarrier();
  #endif

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " Before GatherB Sends "
    << EndLogLine;

  if( FWD_REV == FORWARD )
      GatherB( mLocalOut );
  else
      GatherB( mLocalIn );

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherB Sends "
    << EndLogLine;

  for(int i=0; i < LOCAL_N_X; i++)
    {
    ActiveMsgBarrier( mCompleteFFTsInBarCount_b[ i ], LOCAL_N_Z, mPacketsPerFFT_b );
    mCompleteFFTsInBarCount_b[ i ] = 0;
    }

  BegLogLine(1)
    << "DoFFT "
    << (FWD_REV == FORWARD ? "FORWARD " : "REVERSE " )
    << " After GatherB Active Message Handlers "
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

  BegLogLine( 1 )
    << "DoFFT "
    << "This node finished - about to barrier"
    << EndLogLine;

  //mPcm->TickBarrier( myRank, __LINE__ );

  BGLPartitionBarrier();
  Reset();

  Platform::Memory::ExportFence();

  BegLogLine( 1 )
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
PutRecipSpaceElement( int kx, int ky, int kz, complex value )
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
inline complex FFT3D<FFT_PLAN, FWD_REV>::
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
