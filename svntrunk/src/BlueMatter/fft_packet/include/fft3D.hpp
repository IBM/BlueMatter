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
 //
//  IBM T.J.W  R.C.
//  Date : 24/01/2003
//  Name : Alex Rayshubskiy, Maria Eleftheriou
//  FFT.hpp
//
//  The FFT Class provides an interface for performing a sequencial 3D FFT
//  on distribute data on a 3D-processor mesh. It is templated on the type
//  of 1D FFT to perform and on parallel communication library to use.
//  Parallel Communication libraries available : MPI, BGL_BLADE, BGL_ ...
//
#ifndef __FFT3D_H_
#define __FFT3D_H_

#ifndef DIRECT_BLNIE_BUILD
#include <BlueMatter/fft_one_dim.hpp>
#include <BlueMatter/ImportExportFence.hpp>
#include <BlueMatter/PacketCounterManager.hpp>
// #include <pk/fxlogger.hpp>
#else
#include "blade.h"
#include "fft_one_dim.hpp"
#include "ImportExportFence.hpp"
#include "fxlogger.hpp"
#include "PacketCounterManager.hpp"
#include "fftw.h"

#include <assert.h>
#include "math.h"
#include <stdlib.h>
#include <unistd.h>
#endif


#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG ( 0 )
#endif

#ifndef PKFXLOG_EXECUTE
#define PKFXLOG_EXECUTE ( 1 )
#endif

#ifndef PKFXLOG_DEBUG_DATA
#define PKFXLOG_DEBUG_DATA ( 1 )
#endif

#ifndef PKFXLOG_PACKET_ACTOR_FX_Z  
#define PKFXLOG_PACKET_ACTOR_FX_Z  ( 1 )
#endif
#ifndef PKFXLOG_PACKET_ACTOR_FX_Y  
#define PKFXLOG_PACKET_ACTOR_FX_Y  ( 1 )
#endif
#ifndef PKFXLOG_PACKET_ACTOR_FX_X  
#define PKFXLOG_PACKET_ACTOR_FX_X  ( 1 )
#endif
#ifndef PKFXLOG_PACKET_ACTOR_FX_B  
#define PKFXLOG_PACKET_ACTOR_FX_B  ( 1 )
#endif

#ifndef PKFXLOG_GATHER_Z
#define PKFXLOG_GATHER_Z  ( 1 )
#endif
#ifndef PKFXLOG_GATHER_X
#define PKFXLOG_GATHER_X  ( 1 )
#endif
#ifndef PKFXLOG_GATHER_Y
#define PKFXLOG_GATHER_Y  ( 1 )
#endif
#ifndef PKFXLOG_GATHER_B
#define PKFXLOG_GATHER_B  ( 1 )
#endif

#ifndef PKFXLOG_ACTIVE_MSG_SEND
#define PKFXLOG_ACTIVE_MSG_SEND ( 0 )
#endif

#ifndef PKFXLOG_ACTIVE_MSG_BARRIER
#define PKFXLOG_ACTIVE_MSG_BARRIER ( 0 )
#endif

#ifndef PKFXLOG_INIT
#define PKFXLOG_INIT ( 1 )
#endif

#ifndef PKFXLOG_INIT_HEADERS
#define PKFXLOG_INIT_HEADERS ( 0 )
#endif

#ifndef PKFXLOG_GET_RECIPSPACE
#define PKFXLOG_GET_RECIPSPACE ( 1 )
#endif

#ifndef PKFXLOG_PUT_RECIPSPACE
#define PKFXLOG_PUT_RECIPSPACE ( 1 )
#endif

#ifndef PKFXLOG_GET_REALSPACE
#define PKFXLOG_GET_REALSPACE ( 1 )
#endif

#ifndef PKFXLOG_PUT_REALSPACE
#define PKFXLOG_PUT_REALSPACE ( 1 )
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

//   inline void ForwardTransform();
//   inline void InverseTransform();
  
  inline void ZeroRealSpace();
  inline double GetRealSpaceElement( int x, int y, int z );
  inline void PutRealSpaceElement( int x, int y, int z, double );
  inline complex GetRecipSpaceElement( int kx, int ky, int kz );
  inline void PutRecipSpaceElement( int kx, int ky, int kz, complex );
  
  inline void ScaleIn(double);
//   complex*** GetLocalOut();
//   complex*** GetLocalIn();
//   int CompareOut( complex array[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ] );
//   int CompareOutFFTW( complex array[ GLOBAL_N_X ][ GLOBAL_N_Y ][ GLOBAL_N_Z ] );
//   int CompareIn( complex array[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ] );
//   int Compare( complex array[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ],
//                complex array1[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ],
//                int xSize, int ySize, int zSize );

  FFT3D() {}
  ~FFT3D();

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
    ELEMENTS_PER_PACKET = 8
  };
  
//   enum{ DIM = 3 };
//   enum{ PX_MAX = 6, PY_MAX = 6, PZ_MAX = 6 };
  
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
  FFT_1D_X* mFFT_x;
  FFT_1D_Y* mFFT_y;
  FFT_1D_Z* mFFT_z;
  
    //  complex*** mLocalIn;  // localIn  [nBars][nFFTs][localSizeOfFFT];
  //  complex*** mLocalOut; // localOut [nBars][nFFTs][localSizeOfFFT];
public:
  complex mLocalIn[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ];
  complex mLocalOut[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ];

private:
  //   complex** mGlobalIn_g;  // globalIn [nFFTs][sizeof(1DFFT)];
  //   complex** mGlobalOut_g; // globalOut[nFFTs][sizeof(1DFFT)];
  
  //   complex** mGlobalIn_x;  // globalIn [nFFTs][sizeof(1DFFT)];
  //   complex** mGlobalOut_x; // globalOut[nFFTs][sizeof(1DFFT)];
  complex mGlobalIn_x[ TOTAL_FFTs_IN_BAR_X ][ FFT_PLAN::GLOBAL_SIZE_X ];
  complex mGlobalOut_x[ TOTAL_FFTs_IN_BAR_X ][ FFT_PLAN::GLOBAL_SIZE_X ];
  
  //   complex** mGlobalIn_y;  // globalIn [nFFTs][sizeof(1DFFT)];
  //   complex** mGlobalOut_y; // globalOut[nFFTs][sizeof(1DFFT)];
  complex mGlobalIn_y[ TOTAL_FFTs_IN_BAR_Y ][ FFT_PLAN::GLOBAL_SIZE_Y ];
  complex mGlobalOut_y[ TOTAL_FFTs_IN_BAR_Y ][ FFT_PLAN::GLOBAL_SIZE_Y ];
  
  // complex** mGlobalIn_z;  // globalIn [nFFTs][sizeof(1DFFT)];
  // complex** mGlobalOut_z; // globalOut[nFFTs][sizeof(1DFFT)];
  complex mGlobalIn_z[ TOTAL_FFTs_IN_BAR_Z ][ FFT_PLAN::GLOBAL_SIZE_Z ];
  complex mGlobalOut_z[ TOTAL_FFTs_IN_BAR_Z ][ FFT_PLAN::GLOBAL_SIZE_Z ];
  
  // complex** mGlobalIn_b;  // globalIn [nFFTs][sizeof(1DFFT)];
  // complex** mGlobalOut_b; // globalOut[nFFTs][sizeof(1DFFT)];
  
  // Size of the fft mesh on the local node in 3D
  //unsigned int mLocalN_x, mLocalN_y, mLocalN_z;
    
  // Topology of the machine in 3D
  // unsigned int mP_x, mP_y, mP_z;

  int mMyP_x, mMyP_y, mMyP_z;

  // Size bars in 3D
  //  unsigned int mBarSize_x, mBarSize_y, mBarSize_z;

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

//   int mTotalFFTsInBar_x;
//   int mTotalFFTsInBar_y;
//   int mTotalFFTsInBar_z;

//   int mPointsPerPacket_x;
//   int mPointsPerPacket_y;
//   int mPointsPerPacket_z;
//   int mPointsPerPacket_b;

//   int*  mCompletePacket_x;
//   int*  mCompletePacket_y;
//   int*  mCompletePacket_z;
//   int** mCompletePacket_b;

  int  mCompleteFFTsInBarCount_x;
  int  mCompleteFFTsInBarCount_y;
  int  mCompleteFFTsInBarCount_z;
  //  int* mCompleteFFTsInBarCount_b;

  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_Z;
  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_Y;
  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_X;
  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_B;

  int mPacketsPerFFT_x;
  int mPacketsPerFFT_y;
  int mPacketsPerFFT_z;
  int mPacketsPerFFT_b;

  PacketCounterManager* mPcm;
  
//   void Init( unsigned int localNx,
//              unsigned int localNy,
//              unsigned int localNz,
//              unsigned int Px,
//              unsigned int Py,
//              unsigned int Pz,
//              int fftDir );  

  //  void Execute(complex ***in, complex ***out);

//   void GetPx(int &px);
//   void GetPy(int &py);
//   void GetPz(int &pz);

//   void GetLocalNx(int &localNx);
//   void GetLocalNy(int &localNy);
//   void GetLocalNz(int &localNz);

    static int PacketActorFx_X(void* pkt);
    static int PacketActorFx_Y(void* pkt);
    static int PacketActorFx_Z(void* pkt);
    static int PacketActorFx_B(void* pkt);
    
    void GatherZ( complex in[ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z ] );
    void GatherX( complex in[ TOTAL_FFTs_IN_BAR_X ][ GLOBAL_N_X ] );
    void GatherY( complex in[ TOTAL_FFTs_IN_BAR_Y ][ GLOBAL_N_Y ] );
    void GatherB( complex out[ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z ] );
    
    void InitHeaders();

  
//   template< int FWD_REV >
//   class FwdRevFFTBody 
//   {
//   public:
//     static int PacketActorFx_X(void* pkt);
//     static int PacketActorFx_Y(void* pkt);
//     static int PacketActorFx_Z(void* pkt);
//     static int PacketActorFx_B(void* pkt);
    
//     void GatherZ( complex*** in );
//     void GatherX( complex**  in );
//     void GatherY( complex**  in );
//     void GatherB( complex*** out );
    
//     void InitHeaders();

//     void DoFFT();
    
//     FwdRevFFTBody() {};
//     ~FwdRevFFTBody();
//  };
  
//   FwdRevFFTBody< FORWARD > mForwardDriver;
//   FwdRevFFTBody< REVERSE > mReverseDriver;

  static void ActiveMsgBarrier( int & completePencilsInBarCount,
                                int   barSize,
                                int   packetsPerFFT );
  
  // methods for debbugging ...
//   void PrintComplexArrayR(complex ** array, int nrow, int ncol);
//   void PrintComplexArrayI(complex ** array, int nrow, int ncol);
//   int Validate3DFFT(complex ***in, const int globalNx);

};

void
BGLTorusSimpleActivePacketSend( _BGL_TorusPktHdr &pktHdr, Bit32 ActorFx, 
                                int x, int y, int z, int len, Bit32 data )
  {
  // NEED TO SHORT CUT LOCAL SENDS
  _BGL_TorusPkt    Pkt;
  ///  int chunks = 7;
  Pkt.hdr = pktHdr;
  char *str = (char *) Pkt.payload;

//   BGLTorusMakeHdr( & Pkt.hdr,               // Filled in on return
//                      x,y,z,                 // destination coordinates
//                      0,          //???      // destination Fifo Group
//                      ActorFx,       // Active Packet Function matching signature above
//                      0,                     // primary argument to actor
//                      0,                     // secondary 10bit argument to actor
//                      _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

  memcpy( Pkt.payload, (const void*) data, len );
  int chunk_count = (len +  _BGL_TORUS_SIZEOF_CHUNK - 1) /  _BGL_TORUS_SIZEOF_CHUNK;

  int rc = 0;

  // local actors to occur on caller's stack
  int myPx, myPy, myPz;
  BGLPartitionGetCoords( &myPx, &myPy, &myPz );
  
  BegLogLine( PKFXLOG_ACTIVE_MSG_SEND )
      << "BGLTorusSimpleActivePacketSend:: "
      << " Send To: " << x << ","<< y << ","<< z 
      << EndLogLine;
  
  if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
    {
    ((BGLTorusBufferedActivePacketHandler)(ActorFx))( & Pkt.payload, Pkt.hdr.swh1.arg, Pkt.hdr.swh0.extra );
    }
  else
    {
      BGLTorusSendPacketUnaligned( & Pkt.hdr ,      // Previously created header to use
                                   Pkt.payload,     // Source address of data (16byte aligned)
                                   len ); // Payload bytes = ((chunks * 32) - 16)
    }
  }


template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV > :: Init()
{
//   mLocalN_x = FFT_PLAN::GLOBAL_SIZE_X / aLocalN_x;
//   LOCAL_N_Y = aLocalN_y; 
//   LOCAL_N_Z = aLocalN_z;
//   P_X = aP_x; 
//   P_Y = aP_y; 
//   P_Z = aP_z; 
//  mFFTDirection = aFFTDir;
  
  BegLogLine( PKFXLOG_INIT ) 
    << "FFT::Init:: Start"
    << EndLogLine;

  char filename[ 128 ];
  
  if( FWD_REV == FORWARD )
    sprintf(filename,"%s", "/tmp/packet_counter.forward" );
  else
    sprintf(filename,"%s", "/tmp/packet_counter.backward" );

  mPcm = new PacketCounterManager( P_X * P_Y * P_Z, 1, filename );
  assert( mPcm != NULL );

//   BAR_SIZE_X = LOCAL_N_Y / P_X; // size of bars in the y-dim
//   BAR_SIZE_Y = LOCAL_N_Z / P_Y; // size of bars in the z-dim
//   BAR_SIZE_Z = mLocalN_x / P_Z; // size of bars in the x-dim

  BegLogLine( PKFXLOG_INIT ) 
    << "FFT::Init:: "
    <<" Size of barZ: " <<  BAR_SIZE_X
    <<" Size of barY: " <<  BAR_SIZE_Y
    <<" Size of barX: " <<  BAR_SIZE_Z
    << EndLogLine;
  
//   TOTAL_FFTs_IN_BAR_Z  = BAR_SIZE_Z * LOCAL_N_Y;
//   TOTAL_FFTs_IN_BAR_Y  = BAR_SIZE_Y * mLocalN_x;
//   TOTAL_FFTs_IN_BAR_X  = BAR_SIZE_X * LOCAL_N_Z;

   int bytesAllocated = 0;
       
//   mCompletePacket_x = new int[ TOTAL_FFTs_IN_BAR_X ];
//   assert( mCompletePacket_x != NULL );

//   mCompletePacket_y = new int[ TOTAL_FFTs_IN_BAR_Y ];
//   assert( mCompletePacket_y != NULL );

//   mCompletePacket_z = new int[ TOTAL_FFTs_IN_BAR_Z ];
//   assert( mCompletePacket_z != NULL );

//   mCompletePacket_b = (int **) new int[ mLocalN_x ];
//   assert( mCompletePacket_b != NULL );

//   for(int i=0; i < mLocalN_x; i++)
//     {
//       mCompletePacket_b[ i ] = new int[ LOCAL_N_Z ];
//       assert( mCompletePacket_b[ i ] != NULL );
//     }

//   bytesAllocated +=
//     ( TOTAL_FFTs_IN_BAR_X * sizeof(int) +
//       TOTAL_FFTs_IN_BAR_Y * sizeof(int) +
//       TOTAL_FFTs_IN_BAR_Z * sizeof(int) +
//       mLocalN_x * LOCAL_N_Z * sizeof(int) );

//   BegLogLine( PKFXLOG_INIT ) 
//     << "FFT::Init:: "
//     << "mCompletePacket Allocated Bytes: " << bytesAllocated
//     << EndLogLine; 

//   for(int i=0; i < mLocalN_x; i++)
//     for(int k=0; k < LOCAL_N_Z; k++)
//       {
//         mCompletePacket_b[ i ][ k ] = 0;
//       }
  
//   for(int i=0; i < TOTAL_FFTs_IN_BAR_X ; i++) 
//     mCompletePacket_x[ i ] = 0;
  
//   for(int i=0; i < TOTAL_FFTs_IN_BAR_Y; i++)
//     mCompletePacket_y[ i ] = 0;

//   for(int i=0; i < TOTAL_FFTs_IN_BAR_Z; i++)
//     mCompletePacket_z[ i ] = 0;
  
  mCompleteFFTsInBarCount_x = 0;
  mCompleteFFTsInBarCount_y = 0;
  mCompleteFFTsInBarCount_z = 0;
  
//   mCompleteFFTsInBarCount_b = new int[ mLocalN_x ];
//   assert( mCompleteFFTsInBarCount_b != NULL );

//   bytesAllocated += sizeof(int)*mLocalN_x;

//   BegLogLine( PKFXLOG_INIT ) 
//     << "FFT::Init:: "
//     << "mCompleteFFTsInBarCount_b Allocated Bytes: " << bytesAllocated
//     << EndLogLine; 
  

//   for(int i=0; i < mLocalN_x; i++)
//     mCompleteFFTsInBarCount_b[ i ] = 0;
  
//   POINTS_PER_PACKET_X = min( BAR_SIZE_Z, ELEMENTS_PER_PACKET );
//   POINTS_PER_PACKET_Y = min( BAR_SIZE_X, ELEMENTS_PER_PACKET );
//   POINTS_PER_PACKET_Z = min( LOCAL_N_Z, ELEMENTS_PER_PACKET );
//   POINTS_PER_PACKET_B = min( LOCAL_N_Y, ELEMENTS_PER_PACKET );
  
  mPacketsPerFFT_x = (int) ( P_X * ceil( (double)LOCAL_N_X / (double)POINTS_PER_PACKET_X ));
  mPacketsPerFFT_y = (int) ( P_Y * ceil( (double)LOCAL_N_Y / (double)POINTS_PER_PACKET_Y ));
  mPacketsPerFFT_z = (int) ( P_Z * ceil( (double)LOCAL_N_Z / (double)POINTS_PER_PACKET_Z ));
  mPacketsPerFFT_b = (int) (        ceil( (double)LOCAL_N_Y / (double)POINTS_PER_PACKET_B ));
 
  mFFT_x = new FFT_1D_X();  // 1D fft in the x-dim
  mFFT_y = new FFT_1D_Y();  // 1D fft in the y-dim
  mFFT_z = new FFT_1D_Z();  // 1D fft in the z-dim
  
//   int maxBarSz   = max( max( BAR_SIZE_X, BAR_SIZE_Y ), BAR_SIZE_Z);
//   int maxLocalN  = max( max( LOCAL_N_X, LOCAL_N_Y ), LOCAL_N_Z );
//   int maxGlobalN = max( max( LOCAL_N_X * P_X, LOCAL_N_Y * P_Y ), LOCAL_N_Z * P_Z);
//   int maxBarNum  = max( max( P_X, P_Y ), P_Z);
  
//   maxBarSz += 1;
//   if( maxBarSz == 0 || maxLocalN == 0 || maxGlobalN == 0 || maxBarNum == 0 )
//     {
//       printf( " Error in the allocation\n") ;
//       printf( " maxBarSz = %d\n", maxBarSz );
//       printf( " barSzX = %d\n", BAR_SIZE_X );     
//       printf( " barSzY = %d\n", BAR_SIZE_Y );    
//       printf( " barSzZ = %d\n", BAR_SIZE_Z );
//       printf( " maxLocalN = %d\n", maxLocalN );
//       printf( " maxGlobalN = %d\n", maxGlobalN );
//       printf( " maxBarNum  = %d\n", maxBarNum );
//   }

//  int maxTotalFFTsInBar = max( max( TOTAL_FFTs_IN_BAR_Z, TOTAL_FFTs_IN_BAR_Y ), TOTAL_FFTs_IN_BAR_X );

//   mGlobalIn_g  = (complex**) new complex[ maxTotalFFTsInBar ];
//   assert( mGlobalIn_g != NULL );

//   mGlobalOut_g = (complex**) new complex[ maxTotalFFTsInBar ];
//   assert( mGlobalOut_g != NULL );

//   for(int i=0; i < maxTotalFFTsInBar; i++) 
//     {
//       mGlobalIn_g[ i ]  = (complex *) new complex[ maxGlobalN ];
//       assert( mGlobalIn_g[ i ] != NULL );

//       mGlobalOut_g[ i ] = (complex *) new complex[ maxGlobalN ];
//       assert( mGlobalOut_g[ i ] != NULL );
//     }


//   mGlobalIn_z  = (complex**) new complex[ TOTAL_FFTs_IN_BAR_Z ];
//   assert( mGlobalIn_z != NULL );

//   mGlobalOut_z = (complex**) new complex[ TOTAL_FFTs_IN_BAR_Z ];
//   assert( mGlobalOut_z != NULL );

//   for(int i=0; i < TOTAL_FFTs_IN_BAR_Z; i++) 
//     {
//       mGlobalIn_z[ i ]  = (complex*) new complex[ LOCAL_N_Z * P_Z ];
//       assert( mGlobalIn_z[ i ] != NULL );

//       mGlobalOut_z[ i ] = (complex*) new complex[ LOCAL_N_Z * P_Z ];
//       assert( mGlobalOut_z[ i ] != NULL );
//     }

//   bytesAllocated += ( 2 * TOTAL_FFTs_IN_BAR_Z * sizeof( complex ) * LOCAL_N_Z * P_Z );

//   BegLogLine( PKFXLOG_INIT ) 
//     << "FFT::Init:: "
//     << "mGlobal{In,Out}_z Allocated Bytes: " << bytesAllocated
//     << EndLogLine; 


//   mGlobalIn_x  = (complex**) new complex[ TOTAL_FFTs_IN_BAR_X ];
//   assert( mGlobalIn_x != NULL );
//   mGlobalOut_x = (complex**) new complex[ TOTAL_FFTs_IN_BAR_X ];
//   assert( mGlobalOut_x != NULL );

//   for(int i=0; i < TOTAL_FFTs_IN_BAR_X; i++) 
//     {
//       mGlobalIn_x[ i ]  = (complex*) new complex[ LOCAL_N_X * P_X ];
//       assert( mGlobalIn_x[ i ] != NULL );

//       mGlobalOut_x[ i ] = (complex*) new complex[ LOCAL_N_X * P_X ];
//       assert( mGlobalOut_x[ i ] != NULL );
//     }


//   bytesAllocated += ( 2 * TOTAL_FFTs_IN_BAR_X * sizeof( complex ) * LOCAL_N_X * P_X );

//   BegLogLine( PKFXLOG_INIT ) 
//     << "FFT::Init:: "
//     << "mGlobal{In,Out}_x Allocated Bytes: " << bytesAllocated
//     << EndLogLine; 

//   mGlobalIn_y  = (complex**) new complex[ TOTAL_FFTs_IN_BAR_Y ];
//   assert( mGlobalIn_y != NULL );
//   mGlobalOut_y = (complex**) new complex[ TOTAL_FFTs_IN_BAR_Y ];
//   assert( mGlobalOut_y != NULL );

//    for(int i=0; i < TOTAL_FFTs_IN_BAR_Y; i++) 
//      {
//        mGlobalIn_y[i]  = (complex*) new complex[ LOCAL_N_Y * P_Y ];
//        assert( mGlobalIn_y[ i ] != NULL );

//        mGlobalOut_y[i] = (complex*) new complex[ LOCAL_N_Y * P_Y ];
//        assert( mGlobalOut_y[ i ] != NULL );
//      }
  
//   bytesAllocated += ( 2 * TOTAL_FFTs_IN_BAR_Y * sizeof( complex ) * LOCAL_N_Y * P_Y );
  
//   BegLogLine( PKFXLOG_INIT ) 
//     << "FFT::Init:: "
//     << "mGlobal{In,Out}_y Allocated Bytes: " << bytesAllocated
//     << EndLogLine; 

//   mLocalOut = (complex***) new complex[ LOCAL_N_X ];
//   assert( mLocalOut != NULL );

//   for(int i=0; i<LOCAL_N_X; i++) 
//     {
//       mLocalOut[ i ] = (complex**) new complex[ LOCAL_N_Y ];
//       assert( mLocalOut[ i ] != NULL );

//       for(int j=0; j<LOCAL_N_Y; j++) 
//         {
//           mLocalOut[ i ][ j ] = (complex*) new complex[ LOCAL_N_Z ];
//           assert( mLocalOut[ i ][ j ] != NULL );
//         }
//     }

//   bytesAllocated += ( LOCAL_N_X * LOCAL_N_Y * LOCAL_N_Z ) * sizeof(complex);

//   BegLogLine( PKFXLOG_INIT ) 
//     << "FFT::Init:: "
//     << "mLocalOut Allocated Bytes: " << bytesAllocated
//     << EndLogLine;   

  //printf("Begin header's init\n");

  BegLogLine( PKFXLOG_INIT ) 
      << "FFT::Init:: "    
      << " LOCAL_N_X=" << LOCAL_N_X
      << " LOCAL_N_Y=" << LOCAL_N_Y
      << " LOCAL_N_Z=" << LOCAL_N_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT ) 
      << "FFT::Init:: "    
      << " GLOBAL_N_X=" << GLOBAL_N_X
      << " GLOBAL_N_Y=" << GLOBAL_N_Y
      << " GLOBAL_N_Z=" << GLOBAL_N_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT ) 
      << "FFT::Init:: "    
      << " P_X=" << P_X
      << " P_Y=" << P_Y
      << " P_Z=" << P_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT ) 
      << "FFT::Init:: "    
      << " BAR_SIZE_X=" << BAR_SIZE_X
      << " BAR_SIZE_Y=" << BAR_SIZE_Y
      << " BAR_SIZE_Z=" << BAR_SIZE_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT ) 
      << "FFT::Init:: "    
      << " TOTAL_FFTs_IN_BAR_X=" << TOTAL_FFTs_IN_BAR_X
      << " TOTAL_FFTs_IN_BAR_Y=" << TOTAL_FFTs_IN_BAR_Y
      << " TOTAL_FFTs_IN_BAR_Z=" << TOTAL_FFTs_IN_BAR_Z
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT ) 
      << "FFT::Init:: "    
      << " POINTS_PER_PACKET_X=" << POINTS_PER_PACKET_X
      << " POINTS_PER_PACKET_Y=" << POINTS_PER_PACKET_Y
      << " POINTS_PER_PACKET_Z=" << POINTS_PER_PACKET_Z
      << " POINTS_PER_PACKET_B=" << POINTS_PER_PACKET_B
      << EndLogLine;

  BegLogLine( PKFXLOG_INIT ) 
      << "FFT::Init:: "    
      << " mPacketsPerFFT_x=" << mPacketsPerFFT_x
      << " mPacketsPerFFT_y=" << mPacketsPerFFT_y
      << " mPacketsPerFFT_z=" << mPacketsPerFFT_z
      << " mPacketsPerFFT_b=" << mPacketsPerFFT_b
      << EndLogLine;

  

  BGLPartitionGetCoords( &mMyP_x, &mMyP_y, &mMyP_z );
  
  InitHeaders();
  
  BegLogLine( PKFXLOG_INIT ) 
    << "FFT::Init:: "    
    << "Done Header's Init"
    << EndLogLine;

  return 1;
}

//#define DEBUG_Z 1
template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::PacketActorFx_Z(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;

  FFT3D<FFT_PLAN,FWD_REV> *This = (*ap).mHdr.This;

  int myRank = BGLPartitionGetRankXYZ();
  (*This).mPcm->TickRecv( myRank );

  int a = (*ap).mHdr.aIndex;
  int b = (*ap).mHdr.bIndex;
  int globalNz = (*This).LOCAL_N_Z * (*This).P_Z;

   BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z  )
     << "PacketActorFx_Z:: "
     << "Before the Increment the packet-counter[ " << a << "]=" 
     << (*This).mCompletePacket_z[ a ]
     << "After the Increment the packet-counter[ " << a << "]=" 
     << (*This).mCompletePacket_z[ a ]
     << EndLogLine;
  
  int zPointsPerPacket = (*This).POINTS_PER_PACKET_Z;
    if( (globalNz-b) % (*This).LOCAL_N_Z != 0 
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
      
//       (*This).mGlobalIn_z[ a ][ b+k ].re = (*ap).mData[ k ].re;
//       (*This).mGlobalIn_z[ a ][ b+k ].im = (*ap).mData[ k ].im;
      (*This).mGlobalIn_z[ a ][ b+k ].re = (*ap).mData[ k ].re;
      (*This).mGlobalIn_z[ a ][ b+k ].im = (*ap).mData[ k ].im;
      
      
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z )
        << "PacketActorFx_Z:: "
        << "Recv a packet( "
        << (*This).mMyP_z << " ab " << a
        << "," << b+k << ") =  "
        << (*This).mGlobalIn_z[ a ][ b+k ].re 
        << ","
        << (*This).mGlobalIn_z[ a ][ b+k ].im 
         << EndLogLine;
    }
  
  ImportFence();
  (*This).mCompletePacket_z[ a ]++;
  ExportFence();

  int cmpl = (*This).mCompletePacket_z[ a ]; 

  if( cmpl == (*This).mPacketsPerFFT_z )
    {

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z  )
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
      
       ((*This).mFFT_z)->fftInternal( (*This).mGlobalIn_z[ a ],
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

      (*This).mCompleteFFTsInBarCount_z += 1;
	
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Z  )
 	<< "PacketActorFx_Z:: "
 	<< " Done incremented mCompleteFFTsInBarCount_z to "
 	<< (*This).mCompleteFFTsInBarCount_z
 	<< EndLogLine;
    }

  return 0;
}

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::GatherZ(complex in[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ]) 
//void FFT3D< FFT_PLAN, FWD_REV >::GatherZ(complex*** in) 
{
  BegLogLine( PKFXLOG_GATHER_Z )
      << "Entering GatherZ"
      << EndLogLine;

  ActorPacket zap;  

  int count = 0;
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
                    // zap.mHdr.This = this;
                    // zap.mHdr.aIndex = ( i % BAR_SIZE_Z ) * LOCAL_N_Y + j;
                    // zap.mHdr.bIndex = k+1 - loadCountZ + myPz * LOCAL_N_Z;
                  
                  for(int kk=0; kk < zPointsPerPacket; kk++)                    
                    BegLogLine( PKFXLOG_GATHER_Z & PKFXLOG_DEBUG_DATA )
                      << "GatherZ::"
                      << "Sending a packet to { " << mMyP_x << " , " << mMyP_y << " , " << trgPz << " }"
                      <<  " (a,b) " << (i % BAR_SIZE_Z) * LOCAL_N_Y + j
                      << "," << k+1 - loadCountZ + mMyP_z * LOCAL_N_Z <<" data( "
                      << zap.mData[ kk ].re << "," 
                      << zap.mData[ kk ].im << ")"
                      << EndLogLine;
                  
                  zap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mFFT_Hdr;
                  
//                   printf( "Node(%d,%d,%d) sending to Node(%d,%d,%d)\n", 
//                           mMyP_x, mMyP_y, mMyP_z,
//                           mMyP_x, mMyP_y, trgPz );

                  int myRank = BGLPartitionGetRankXYZ();
                  mPcm->TickSend( myRank );

                  BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mBGLTorus_Hdr,
                                                  (Bit32) PacketActorFx_Z,
                                                  mMyP_x,
                                                  mMyP_y,
                                                  trgPz,
                                                  sizeof( zap ),
                                                  (Bit32) &zap);
                  headerCount++;
                  loadCountZ = 0;
                }
            }
        }
    }
}

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::PacketActorFx_X(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D< FFT_PLAN, FWD_REV > *This = (*ap).mHdr.This;

  int myRank = BGLPartitionGetRankXYZ();
  (*This).mPcm->TickRecv( myRank );

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
            
       // (*This).mGlobalIn_x[ a ][ b+i ] = (*ap).mData[ i ];
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
  
  //  ImportFence();
  ImportFence();
  (*This).mCompletePacket_x[ a ] ++;// (*This).POINTS_PER_PACKET_X;
  ExportFence();

  int cmpl = ((*This).mCompletePacket_x[ a ]);
  
  if( cmpl == (*This).mPacketsPerFFT_x )
    {
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X  )
 	<< " PacketActorFx_X:: "
 	<< " Got " << (*This).mPacketsPerFFT_x << " of " << cmpl
 	<< " DOING FFT "
 	<< EndLogLine;
      
      ((*This).mFFT_x)->fftInternal( (*This).mGlobalIn_x[ a ], 
                                     (*This).mGlobalOut_x[ a ]);
      
      (*This).mCompleteFFTsInBarCount_x++;
      
      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_X ) 
        << "PacketActorFx_X:: "
        << "After the Increment the fft-counterX = "
        << (*This).mCompleteFFTsInBarCount_x
        << EndLogLine;
    }
  
  return 0;
}

template<class FFT_PLAN, int FWD_REV>
void FFT3D< FFT_PLAN, FWD_REV >::GatherX(complex in[ TOTAL_FFTs_IN_BAR_X ][ GLOBAL_N_X ]) 
// void FFT3D< FFT_PLAN, FWD_REV >::GatherX(complex ** in) 
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
// 		    xap.mHdr.This = this;
// 		    xap.mHdr.aIndex = ax;
// 		    xap.mHdr.bIndex = bx - loadCountX + 1;
// 		    xap.mHdr.cIndex = 0;

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
                    mPcm->TickSend( myRank );

                    BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mBGLTorus_Hdr,
                                                    (Bit32) PacketActorFx_X,
                                                    trgPx,
                                                    mMyP_y,
                                                    trgPz,
                                                    sizeof( xap ),
                                                    (Bit32) &xap );
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
int FFT3D< FFT_PLAN, FWD_REV >::PacketActorFx_Y(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D<FFT_PLAN, FWD_REV> *This = (*ap).mHdr.This;

  int myRank = BGLPartitionGetRankXYZ();
  (*This).mPcm->TickRecv( myRank );

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

  ImportFence();
  (*This).mCompletePacket_y[a] ++; // (*This).POINTS_PER_PACKET_X;
  ExportFence();

  int cmpl = ((*This).mCompletePacket_y[a]);

  if(cmpl == (*This).mPacketsPerFFT_y )
    {

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y  )
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

      ((*This).mFFT_y)->fftInternal((*This).mGlobalIn_y[ a ],
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

      (*This).mCompleteFFTsInBarCount_y++;        

      BegLogLine( PKFXLOG_PACKET_ACTOR_FX_Y )
 	<< "PacketActorFx_Y:: "
 	<< " Done incremented mCompleteFFTsInBarCount_y to "
 	<< (*This).mCompleteFFTsInBarCount_y
 	<< EndLogLine;
    }

  return 0;
}

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::GatherY(complex  in[ TOTAL_FFTs_IN_BAR_Y ][ GLOBAL_N_Y ]) 
// void FFT3D< FFT_PLAN, FWD_REV >::GatherY(complex ** in) 
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
                       mPcm->TickSend( myRank );

                       BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mBGLTorus_Hdr,
                                                       (Bit32) PacketActorFx_Y,
                                                       trgPx,
                                                       trgPy,
                                                       mMyP_z,
                                                       sizeof( yap ),
                                                       (Bit32) &yap );
                       loadCountY = 0;
                       headerCount++;
                    }
                }
            }
        }
    }
}

template<class FFT_PLAN, int FWD_REV >
int FFT3D< FFT_PLAN, FWD_REV >::PacketActorFx_B(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D<FFT_PLAN, FWD_REV> *This = (*ap).mHdr.This;

  int myRank = BGLPartitionGetRankXYZ();
  (*This).mPcm->TickRecv( myRank );

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
  
  ImportFence();
  ( (*This).mCompletePacket_b[ i ][ k ] )++;// (*This).POINTS_PER_PACKET_X;
  ExportFence();

  int cmpl = ((*This).mCompletePacket_b[ i ][ k ]);
  
  if( cmpl ==(*This).mPacketsPerFFT_b )
    { 
      (*This).mCompleteFFTsInBarCount_b[ i ]++;

       BegLogLine( PKFXLOG_PACKET_ACTOR_FX_B ) 
         << "PacketActorFx_B:: "
         << "bCompleteFFTs[" << i << "]="
         << (*This).mCompleteFFTsInBarCount_b[ i ]
         << EndLogLine;

//     // for(int i=0; i<(*This).LOCAL_N_X; i++)
//  	{
// 	  for(int k=0; k<(*This).LOCAL_N_Z; k++){ 	 
// 	      (*This).mCompletePacket_b[i][k]=0;
// 	  }
//  	}
    }
  return 0;
}

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::GatherB(complex out[ LOCAL_N_X][ LOCAL_N_Y ][ LOCAL_N_Z ]) 
//void FFT3D< FFT_PLAN, FWD_REV >::GatherB(complex *** out) 
{
  ActorPacket bap;
  int headerCount=0;
  
  for(int a=0; a < ( LOCAL_N_Z / P_Y ) * LOCAL_N_X; a++)
    {
      int loadCount = 0;
      int i = a % LOCAL_N_X;
      int k = a / LOCAL_N_X + mMyP_y * BAR_SIZE_Y;

      for(int b=0; b < LOCAL_N_Y * P_Y; b++) 
	{
	  int j = b % LOCAL_N_Y;
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
              mPcm->TickSend( myRank );
              
              BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr,
                                              (Bit32) PacketActorFx_B,
                                              mMyP_x,
                                              trgPy,
                                              mMyP_z,
                                              sizeof( bap ),
                                              (Bit32) &bap );
	      loadCount = 0;
              headerCount++;
	    }
	}
    }
}


template<class FFT_PLAN, int FWD_REV >
void FFT3D<FFT_PLAN, FWD_REV>::ActiveMsgBarrier(int &aCompleteFFTsInBarCount,
                                                int aTotalFFTsInBar, 
                                                int aPacketsPerBar ) 
{
    int barMon = -1;
    
    BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER ) 
        << "ActiveMsgBarrier:: " 
        << "ActiveMsgBarrier BEGIN, have " << aCompleteFFTsInBarCount
        << " of " << aTotalFFTsInBar
        << EndLogLine;
    
    //  printf( "ActiveMsgBarrier BEGIN, have %d of %d\n", aCompleteFFTsInBarCount, aTotalFFTsInBar );
    
    while( aCompleteFFTsInBarCount < aTotalFFTsInBar )
        {
            ImportFence();
            
#ifndef USE_2_CORES_FOR_FFT
            int count1 = 1;
            BGLTorusWaitCount(&count1);
#endif
            
            if( barMon != aCompleteFFTsInBarCount )
                {
                    BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER ) 
                        << "ActiveMsgBarrier:: " 
                        <<" ActiveMsgBarrier : waiting, have "
                        << aCompleteFFTsInBarCount
                        << " of "
                        << aTotalFFTsInBar
                        << EndLogLine;
                    
                    // printf( "ActiveMsgBarrier: waiting, have %d of %d\n", aCompleteFFTsInBarCount, aTotalFFTsInBar );
                    barMon = aCompleteFFTsInBarCount;
                }
            
            sleep( 1 );
        }
    
    //  printf( "ActiveMsgBarrier: Done %d of %d\n", aCompleteFFTsInBarCount, aTotalFFTsInBar );
    
    
    BegLogLine( PKFXLOG_ACTIVE_MSG_BARRIER ) 
        << "ActiveMsgBarrier  Done! " 
        << aCompleteFFTsInBarCount
        << " of " << aTotalFFTsInBar
        << EndLogLine;
}

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::InitHeaders() 
{
  ///  int myPx, myPy, myPz;
  ///  int myRank;  
  ///  BGLPartitionGetCoords( &myPx, &myPy, &myPz);  

  BegLogLine( PKFXLOG_INIT_HEADERS )
    << "FFT3D::InitHeaders" 
    << EndLogLine;
      

  /*****************************************************************/
  // Z direction pregeneration  
  int PacketsToSendInGatherZ = (int ) ( LOCAL_N_X * LOCAL_N_Y * ceil( (double)LOCAL_N_Z / (double)POINTS_PER_PACKET_Z ) );
  
  //printf("PacketsToSendInGatherZ:: %d\n", PacketsToSendInGatherZ );

  mPregeneratedBGLTorusAndFFTHeaders_Z = new BGLTorus_FFT_Headers[ PacketsToSendInGatherZ ];  

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
                  //                   zap.mHdr.This = this;
                  //                   zap.mHdr.aIndex = ( i % BAR_SIZE_Z ) * LOCAL_N_Y + j;
                  //                   zap.mHdr.bIndex = k+1 - loadCountZ + myPz * LOCAL_N_Z;
                  
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
                 
                  assert( headerCount < PacketsToSendInGatherZ );
                  mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                  mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mFFT_Hdr      = fftHdr;
                  headerCount++;

                  BegLogLine( PKFXLOG_INIT_HEADERS )
                    << "FFT3D::InitHeaders" 
                    << " Generate headersZ: headerCount: " << headerCount
                    << " zPointsPerPacket: " << zPointsPerPacket
                    << " loadCountZ: " << loadCountZ 
                    << EndLogLine;

                  loadCountZ=0;
                }
            }
        }
    }


  BegLogLine( PKFXLOG_INIT_HEADERS )
    << "FFT3D::InitHeaders " 
    << "PacketsToSendInGatherZ: " << PacketsToSendInGatherZ
    << " headerCount: " << headerCount
    << EndLogLine;

  assert( headerCount == PacketsToSendInGatherZ );    
      
  /*****************************************************************/
  // Pregenerate Y
  headerCount = 0;

  int temp1 = mMyP_y * LOCAL_N_Y;
  int temp2 = mMyP_x * BAR_SIZE_X  + temp1;
  int temp3 = temp2 + BAR_SIZE_X;

  int PacketsToSendInGatherY = (int) ( P_X * P_Y * ( LOCAL_N_X * BAR_SIZE_Y ) * ceil( (double)BAR_SIZE_X / (double)POINTS_PER_PACKET_Y) );
  
  //printf("PacketsToSendInGatherY:: %d\n", PacketsToSendInGatherY );

  mPregeneratedBGLTorusAndFFTHeaders_Y = new BGLTorus_FFT_Headers[ PacketsToSendInGatherY ];

  
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
                      
                      assert( headerCount < PacketsToSendInGatherY );
                      mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                      mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mFFT_Hdr      = fftHdr;
                      
                      headerCount++;
                      loadCountY = 0;
                    }
                }
            }
        }
    }
  assert( headerCount == PacketsToSendInGatherY );
  

  /*****************************************************************/
  // Pregenerate X
  headerCount = 0;
  
  temp1 = mMyP_x * LOCAL_N_X;
  temp2 = mMyP_z * BAR_SIZE_Z + temp1;
  temp3 = temp2 + BAR_SIZE_Z;

  int PacketsToSendInGatherX = (int) ( P_Z * P_X * LOCAL_N_Z * BAR_SIZE_X * 
                                       ceil( (double)BAR_SIZE_Z / (double)POINTS_PER_PACKET_X) );

  //printf("PacketsToSendInGatherX:: %d\n", PacketsToSendInGatherX );

  mPregeneratedBGLTorusAndFFTHeaders_X = new BGLTorus_FFT_Headers[ PacketsToSendInGatherX ];
    
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
                      
                      assert( headerCount < PacketsToSendInGatherX );
                      mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                      mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mFFT_Hdr      = fftHdr;
                      headerCount++;
                      loadCountX = 0;
                    }
                }
	    }
	}
    }
  
  assert( headerCount == PacketsToSendInGatherX );

  /*****************************************************************/
  // Pregenerate B
  headerCount=0;
  
  int PacketsToSendInGatherB = (int) (( ( LOCAL_N_Z / P_Y ) * LOCAL_N_X ) * 
                                      ceil ( (double)(LOCAL_N_Y * P_Y)/(double)(POINTS_PER_PACKET_B) ) );
  
  //printf("PacketsToSendInGatherB:: %d\n", PacketsToSendInGatherB );


  mPregeneratedBGLTorusAndFFTHeaders_B = new BGLTorus_FFT_Headers[ PacketsToSendInGatherB ];


  for(int a=0; a < ( LOCAL_N_Z / P_Y ) * LOCAL_N_X; a++)
    {
      int loadCount = 0;
      //      int i = a % LOCAL_N_X;
      // int k = a / LOCAL_N_X + mMyP_y * BAR_SIZE_Y;

      for(int b=0; b < LOCAL_N_Y * P_Y; b++) 
	{
          //  int j = b % LOCAL_N_Y;
	  int trgPy = b / LOCAL_N_Y;

// 	  bap.mData[ loadCount ].re = mGlobalOut_y[ a ][ b ].re;
// 	  bap.mData[ loadCount ].im = mGlobalOut_y[ a ][ b ].im;
	  loadCount++;

	  if( loadCount == POINTS_PER_PACKET_B )
	    {
// 	      bap.mHdr.This = this;
// 	      bap.mHdr.aIndex = a % LOCAL_N_X;
// 	      bap.mHdr.bIndex = b % LOCAL_N_Y - loadCount+1;
// 	      bap.mHdr.cIndex = a / LOCAL_N_X + mMyP_y * BAR_SIZE_Y;
	     
// 	      BegLogLine(0) 
//                   << "GatherB::POINTS_PER_PACKET_B "
//                   << POINTS_PER_PACKET_B << " , "
//                   << loadCount << "  , " 
//                   << "localNy" <<LOCAL_N_Y
//                   << EndLogLine;
              
// 	      BGLTorusSimpleActivePacketSend( PacketActorFx_B,
//                                               mMyP_x,
//                                               trgPy,
//                                               mMyP_z,
//                                               sizeof( bap ),
//                                               (void *) &bap );
              
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
              
              assert( headerCount < PacketsToSendInGatherB );
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mFFT_Hdr      = fftHdr;

              headerCount++;              
	      loadCount = 0;
	    }
	}
    }
  
  assert( headerCount == PacketsToSendInGatherB );
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

//   int mCompletePacket_y[ TOTAL_FFTs_IN_BAR_Y ];
//   int mCompletePacket_z[ TOTAL_FFTs_IN_BAR_Z ];

  for(int i=0; i < LOCAL_N_X; i++)
    for(int j=0; j < LOCAL_N_Z; j++)      
      mCompletePacket_b[ i ][ j ] = 0;
  
  for(int i=0; i < LOCAL_N_X; i++)
    mCompleteFFTsInBarCount_b[ i ] = 0;

};

template<class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::DoFFT()
{     
  // Start by collecting and doing a 1D FFT in the z-direction
#ifdef PKFXLOG_DEBUG_DATA
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

  
  if( FWD_REV == FORWARD )
      GatherZ( mLocalIn );
  else
      GatherZ( mLocalOut );

  // GatherZ(in, 0);
  // BGLPartitionBarrier();
  ActiveMsgBarrier( mCompleteFFTsInBarCount_z, TOTAL_FFTs_IN_BAR_Z, mPacketsPerFFT_z );
  mCompleteFFTsInBarCount_z = 0;

  BegLogLine( PKFXLOG_EXECUTE )
    << "Execute:: Done GatherZ"
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

  mPcm->TickBarrier( myRank, __LINE__ );
  BGLPartitionBarrier();
  
  GatherX( mGlobalIn_x );
  //BGLPartitionBarrier();

//   for(int a=0; a < BAR_SIZE_X * LOCAL_N_Z; a++)
//     {
//       for(int k=0; k < LOCAL_N_X * P_X; k++)
//         {  
//           BegLogLine( PKFXLOG_DEBUG_DATA ) 
//             << "Execute:: mGlobalIn_x[" << a << "]["<< k << "]"
//             << mGlobalIn_x[ a ][ k ].re << " ," 
//             << mGlobalIn_x[ a ][ k ].im
//             << EndLogLine;
//         }
//     }

  
  BegLogLine( PKFXLOG_EXECUTE )  
    << "Execute:: mCompleteFFTsInBarCount_x = "
    << mCompleteFFTsInBarCount_x << " "
    << "LOCAL_N_Z*BAR_SIZE_X = "
    << LOCAL_N_Z * BAR_SIZE_X
    << EndLogLine;
  
  ActiveMsgBarrier( mCompleteFFTsInBarCount_x, TOTAL_FFTs_IN_BAR_X, mPacketsPerFFT_x );
  mCompleteFFTsInBarCount_x = 0;
  //printf("Got to Barrier1\n");  
  //fflush(stdout);
  mPcm->TickBarrier( myRank, __LINE__ );
  BGLPartitionBarrier();  

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
  // if( myRank == 0 )  
  // printf("Done GatherX(%d)\n",myRank);
  BegLogLine( PKFXLOG_EXECUTE )
    << "Execute:: Done GatherX"
    << EndLogLine;
  //printf("Got to Barrier2\n");
  //fflush(stdout);
  mPcm->TickBarrier( myRank, __LINE__ );
  BGLPartitionBarrier();
// #ifdef PKFXLOG_DEBUG_DATA 
//   for(int a=0; a < BAR_SIZE_Y * LOCAL_N_X; a++)
//     {
//       for(int k=0; k < LOCAL_N_Y * P_Y; k++)
//  	{
//  	  BegLogLine( PKFXLOG_DEBUG_DATA )
//             << "Execute:: mGlobalIn_y[" << a << "]["<< k << "]" 
//             << mGlobalIn_y[ a ][ k ].re << " ," 
//             << mGlobalIn_y[ a ][ k ].im
//             << EndLogLine;
//  	}
//     }
// #endif
  
  // FFT3D< FFT_PLAN >::FwdRevFFTBody< FWD_REV >::GatherY( mGlobalIn_y );
  GatherY( mGlobalIn_y );
  // BGLPartitionBarrier();

  ActiveMsgBarrier( mCompleteFFTsInBarCount_y, TOTAL_FFTs_IN_BAR_Y, mPacketsPerFFT_y );
  mCompleteFFTsInBarCount_y = 0;

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
  // if( myRank == 0 )
  // printf("Done GatherY(%d)\n",myRank);
  BegLogLine( PKFXLOG_EXECUTE )
    << "Execute:: Done GatherY"
    << EndLogLine;

  mPcm->TickBarrier( myRank, __LINE__ );
  BGLPartitionBarrier();

  if( FWD_REV == FORWARD )
      GatherB( mLocalOut );
  else
      GatherB( mLocalIn );
  
  // GatherB( out, 0 );
  // BGLPartitionBarrier();
  
  for(int i=0; i < LOCAL_N_X; i++)
    {
      ActiveMsgBarrier( mCompleteFFTsInBarCount_b[ i ], LOCAL_N_Z, mPacketsPerFFT_b );
      mCompleteFFTsInBarCount_b[ i ] = 0;
    } 

  BegLogLine( PKFXLOG_EXECUTE )
    << "Execute:: Done GatherB"
    << EndLogLine;

  mPcm->TickBarrier( myRank, __LINE__ );  
  BGLPartitionBarrier();

#ifdef PKFXLOG_DEBUG_DATA   
  if( FWD_REV == FORWARD )
    {
      for(int i=0; i < LOCAL_N_X; i++)
        {
          for(int j=0; j < LOCAL_N_Y; j++)
            {
              for(int k=0; k < LOCAL_N_Z; k++)
                {
                  // 	      out[ i ][ j ][ k ].re = mLocalOut[ i ][ j ][ k ].re;
                  // 	      out[ i ][ j ][ k ].im = mLocalOut[ i ][ j ][ k ].im;
                  
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
                  // 	      out[ i ][ j ][ k ].re = mLocalOut[ i ][ j ][ k ].re;
                  // 	      out[ i ][ j ][ k ].im = mLocalOut[ i ][ j ][ k ].im;
                  
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
  
  // if( myRank == 0 )
  // printf("=====  Done FFT(%d) ======\n",myRank);
  BegLogLine( PKFXLOG_EXECUTE )
    << "=====  Done FFT ======"
    << EndLogLine;
  
  Reset();

  mPcm->TickBarrier( myRank, __LINE__ );
  BGLPartitionBarrier();
}


template<class FFT_PLAN, int FWD_REV >
inline void FFT3D< FFT_PLAN, FWD_REV >::ZeroRealSpace()
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
inline void FFT3D<FFT_PLAN, FWD_REV>::PutRealSpaceElement( int x, int y, int z, double value)
{
    BegLogLine( PKFXLOG_PUT_REALSPACE )   
      << "PutRealSpaceElement: " 
      << " x=" << x
      << " y=" << y
      << " z=" << z
      << " data= " << value
      << EndLogLine;

  mLocalIn[ x ][ y ][ z ].re = value;
  mLocalIn[ x ][ y ][ z ].im = 0.0; 
}

template<class FFT_PLAN, int FWD_REV>
inline double FFT3D<FFT_PLAN, FWD_REV>::GetRealSpaceElement( int x, int y, int z )
{
  BegLogLine( PKFXLOG_GET_REALSPACE )   
      << "GetRealSpaceElement: " 
      << " x=" << x
      << " y=" << y
      << " z=" << z
      << " data= " << mLocalIn[ x ][ y ][ z ].re
      << EndLogLine;

  return mLocalIn[ x ][ y ][ z ].re;
}

template<class FFT_PLAN, int FWD_REV>
inline void FFT3D<FFT_PLAN, FWD_REV>::PutRecipSpaceElement( int kx, int ky, int kz, complex value )
{
    BegLogLine( PKFXLOG_PUT_RECIPSPACE )   
        << "PutRecipSpaceElement: " 
        << " kx=" << kx
        << " ky=" << ky
        << " kz=" << kz
        << " data.re= " << value.re
        << " data.im= " << value.im
        << EndLogLine;
    
    mLocalOut[ kx ][ ky ][ kz ] = value;
}

template<class FFT_PLAN, int FWD_REV>
inline complex FFT3D<FFT_PLAN, FWD_REV>::GetRecipSpaceElement( int kx, int ky, int kz )
{

    BegLogLine( PKFXLOG_GET_RECIPSPACE )   
        << "GetRecipSpaceElement: " 
        << " kx=" << kx
        << " ky=" << ky
        << " kz=" << kz
        << " data.re= " << mLocalOut[ kx ][ ky ][ kz ].re
        << " data.im= " << mLocalOut[ kx ][ ky ][ kz ].im
        << EndLogLine;

  return mLocalOut[ kx ][ ky ][ kz ];
}

template<class FFT_PLAN, int FWD_REV>
FFT3D<FFT_PLAN, FWD_REV>::~FFT3D()
{
}

template<class FFT_PLAN, int FWD_REV>
inline void FFT3D<FFT_PLAN, FWD_REV>::GetLocalDimensions( int& aLocalX, int& aLocalSizeX, 
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
inline void FFT3D< FFT_PLAN, FWD_REV >::ScaleIn( double aScaleFactor ) 
{
  for(int x=0; x < LOCAL_N_X; x++)
    for(int y=0; y < LOCAL_N_Y; y++)
      for(int z=0; z < LOCAL_N_Z; z++)
        {
          mLocalIn[ x ][ y ][ z ].re = aScaleFactor * mLocalIn[ x ][ y ][ z ].re;
          mLocalIn[ x ][ y ][ z ].im = aScaleFactor * mLocalIn[ x ][ y ][ z ].im;     
        }  
}

// template<class FFT_PLAN, int FWD_REV>
// int FFT3D< FFT_PLAN, FWD_REV >::CompareInOut() 
// { 
//   return compare( (complex ***) &mLocalOut[0][0][0], (complex ***) &mLocalIn[0][0][0], 
//                   LOCAL_N_X, LOCAL_N_Y, LOCAL_N_Z );
// }
 
// template<class FFT_PLAN, int FWD_REV>
// int FFT3D< FFT_PLAN, FWD_REV >::CompareOut(complex array[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ]) 
// { 
//     return Compare( mLocalOut, array, 
//                     LOCAL_N_X, LOCAL_N_Y, LOCAL_N_Z );
// }

// template<class FFT_PLAN, int FWD_REV>
// int FFT3D< FFT_PLAN, FWD_REV >::CompareOutFFTW(complex array[ GLOBAL_N_X ][ GLOBAL_N_Y ][ GLOBAL_N_Z ]) 
// { 
// //     return CompareFFTW( mLocalOut, array, 
// //                         LOCAL_N_X, LOCAL_N_Y, LOCAL_N_Z );
//     int error=0;
//     double tolerance = 1./1000000.;
//     for(int i=0; i < LOCAL_N_X; i++)
//         for(int j=0; j < LOCAL_N_Y; j++)
//             for(int k=0; k < LOCAL_N_Z; k++) 
//                 {
//                     int a = mMyX*LOCAL_N_X + i;
//                     int b = mMyY*LOCAL_N_Y + j;
//                     int c = mMyZ*LOCAL_N_Z + k;
                    
//                     if( fabs( mLocalOut[ i ][ j ][ k ].re - array[ a ][ b ][ c ].re ) > tolerance
//                         || fabs( mLocalOut[ i ][ j ][ k ].im - array[ a ][ b ][ c ].im ) > tolerance )
//                         {              
//                             printf("Coord: %d,%d,%d \n", mMyX, mMyY, mMyZ);
//                             printf("mLocalOut[ %d ][ %d ][ %d ]=%f\n", i, j, k, mLocalOut[ i ][ j ][ k ].re);
//                             printf("array[ %d ][ %d ][ %d ]=%f\n", a, b, c, array[ a ][ b ][ c ].re);              
//                             error=1;              
//                         }
//                 }
    
//     return error;
// }

// template<class FFT_PLAN, int FWD_REV>
// int FFT3D< FFT_PLAN, FWD_REV >::CompareIn(complex array[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ]) 
// { 
//     return Compare( mLocalIn, array, 
//                     LOCAL_N_X, LOCAL_N_Y, LOCAL_N_Z );
// }

// template<class FFT_PLAN, int FWD_REV>
// complex***  FFT3D< FFT_PLAN, FWD_REV >::GetLocalIn() 
// { 
//   return (complex ***) mLocalIn;
// }

// template<class FFT_PLAN, int FWD_REV>
// complex*** FFT3D< FFT_PLAN, FWD_REV >::GetLocalOut() 
// { 
//   return (complex ***)mLocalOut;
// }

// template<class FFT_PLAN, int FWD_REV >
// int FFT3D< FFT_PLAN, FWD_REV >::Compare(complex in1[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ], 
//                                         complex in2[ LOCAL_N_X ][ LOCAL_N_Y ][ LOCAL_N_Z ],
//                                         int xSize, int ySize, int zSize ) 
// {
//  double tolerance = 1. / 1000000000.;
  
//   for( int x = 0; x < xSize; x++ )
//     for( int y = 0; y < ySize; y++ )
//       for( int z = 0; z < zSize; z++ )
//         {
//           if( fabs( in1[ x ][ y ][ z ].re - in2[ x ][ y ][ z ].re ) < tolerance )
//             {
//               return 1;
//             }          

//           if( fabs( in1[ x ][ y ][ z ].im - in2[ x ][ y ][ z ].im ) < tolerance )
//             {
//               return 1;
//             }          
//         }  

//   return 0;
// }

#endif
