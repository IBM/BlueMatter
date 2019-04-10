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
 #include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/pktrace.hpp>

double Tolerance = 0.0000000000001;
double dumbsqrt( double x )
  {
  double g;
  double gnew;
  double diff;

  if( x == 0 )
    return( 0 );
  if( x == 1 )
    return( 1 );
  if( x < 0 )
    PLATFORM_ABORT("dumbsqrt::x < 0");
    // exit( -1 );

  g = x;
  while( 1 )
    {
    gnew = 0.5 * ( g + x / g );
    diff = x - gnew * gnew;
    if( diff < 0 )
      diff *= -1;
    if( diff < Tolerance )
      break;
    g = gnew;
    }
  return( g );
  }


#ifdef FFT3D_SENDER_COMPUTES
#include <fft3d/fft3d_sender_computes.hpp>
#else
#include <fft3d/fft3d.hpp>
#endif

#include <assert.h>

// #include <blade.h>


// #ifdef USE_SETPRI
// #include "setpri.hpp"
// #endif // USE_SETPRI

TraceClient fftForwardTracerStart;
TraceClient fftForwardTracerFinis;

TraceClient fftReverseTracerStart;
TraceClient fftReverseTracerFinis;

#include <math.h>

Platform::Lock::LockWord   LogMutex;

#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG ( 1 )
#endif

#ifndef PKTRACE_TUNE
#define PKTRACE_TUNE ( 1 )
#endif

#ifndef PKFXLOG_SUCCESS
#define PKFXLOG_SUCCESS ( 1 )
#endif

struct FFT_PLAN1
  {
  enum
    {
    P_X = 1,
    P_Y = 1,
    P_Z = 1 
    };

  enum
    {
    GLOBAL_SIZE_X = 64,
    GLOBAL_SIZE_Y = 64,
    GLOBAL_SIZE_Z = 64
    };
  };

struct FFT_PLAN2
  {
  enum
    {
    P_X = 4,
    P_Y = 4,
    P_Z = 2 
    };

  enum
    {
    GLOBAL_SIZE_X = 64,
    GLOBAL_SIZE_Y = 64,
    GLOBAL_SIZE_Z = 64
    };
  };

//#define REGRESS_FFTW_TEXT 

#ifdef USE_FFTW_ONE_DIM
#include <BlueMatter/fftw.h>
#endif

#define DO_REGRESSION

#ifndef PK_BGL
  #define REGRESS_FFTW
  #define REGRESS_FFTW_FORWARD
#endif

#define DO_REVERSE

#ifndef PK_BGL
#define REGRESS_FFTW_REVERSE
#endif

#define REGRESS_FFT3D_REVERSE_TEST

#ifdef REGRESS_FFTW_FORWARD
fft3d_complex src [ FFT_PLAN2::GLOBAL_SIZE_X ][ FFT_PLAN2::GLOBAL_SIZE_Y ][ FFT_PLAN2::GLOBAL_SIZE_Z ];
fft3d_complex dst1[ FFT_PLAN2::GLOBAL_SIZE_X ][ FFT_PLAN2::GLOBAL_SIZE_Y ][ FFT_PLAN2::GLOBAL_SIZE_Z ];
#endif

#ifdef REGRESS_FFTW_REVERSE
fft3d_complex final[ FFT_PLAN2::GLOBAL_SIZE_X ][ FFT_PLAN2::GLOBAL_SIZE_Y ][ FFT_PLAN2::GLOBAL_SIZE_Z ];
#endif

typedef  FFT3D< FFT_PLAN1, FORWARD > FFT_FORWARD1 FFT3D_STUCT_ALIGNMENT;
typedef  FFT3D< FFT_PLAN1, REVERSE > FFT_REVERSE1 FFT3D_STUCT_ALIGNMENT;

typedef  FFT3D< FFT_PLAN2, FORWARD > FFT_FORWARD2 FFT3D_STUCT_ALIGNMENT;
typedef  FFT3D< FFT_PLAN2, REVERSE > FFT_REVERSE2 FFT3D_STUCT_ALIGNMENT;

// FFT_FORWARD1* fftForward1;
#ifdef DO_REVERSE
// FFT_REVERSE1* fftReverse1;
#endif

FFT_FORWARD2* fftForward2;
#ifdef DO_REVERSE
FFT_REVERSE2* fftReverse2;
#endif

// Platform::Lock::LockWord   FFT_InitMutex = 1;

extern Platform::Mutex::MutexWord IOP_CP_Barrier;


#ifndef NO_PK_PLATFORM

// extern int _CP_main1( int argc, char *argv[], char **envp );
 
extern "C" {

  extern void* _CP_main1( int argc, char *argv[], char **envp );

  void *
  PkMain( int argc, char ** argv, char ** envp )
  {
    // s0printf("File: %s, ln %d\n", __FILE__, __LINE__ );
    _CP_main1( argc, argv, envp );
    return ( NULL );
  }
};
// #define _BN( n ) (0x01 << n)


void GetXYZForRank(int &x, int &y, int &z, int rank)
{
  int maxX, maxY, maxZ;
  BGLPartitionGetDimensions( &maxX, &maxY, &maxZ );
  
  for(x=0; x<maxX; x++)
    for(y=0; y<maxY; y++)
      for(z=0; z<maxZ; z++)
        {
          if( BGLPartitionMakeRank(x,y,z) == rank )
            return;
        }
}

#if 0
void CascadeCall( void(*pf)() ) 
{   
 // pkTraceServer::FlushAllBothBuffers();
 // Call the below function on all the nodes in sequence   
 pf();   
 
 int myRank          = BGLPartitionGetRank();
 int myPartitionSize = BGLPartitionGetSize();
 
 if( myRank != myPartitionSize-1 )
   {
     int dX,dY,dZ;
     
     GetXYZForRank(dX,dY,dZ, myRank+1);
     
     // Send the active Message to myRank+1 node;
     _BGL_TorusPktHdr   BGLTorus_Hdr;
     BGLTorusMakeHdr( & BGLTorus_Hdr,
                      dX,
                      dY,
                      dZ,         
                      0,          
                      (Bit32) CascadeCall,      
                      0,                   
                      0,                   
                      _BGL_TORUS_ACTOR_BUFFERED );    
     
     BGLTorusSimpleActivePacketSend( BGLTorus_Hdr, 
                                     (Bit32) CascadeCall,      
                                     dX,dY,dZ, 0, NULL );
     
   }
}


AtomicNative<int> AllToAllCounter[ 8 ][ 8 ][ 8 ];

typedef int YoYoYoActorPacket[3];

AtomicNative<int> AACounter;

int YoYoYoActor( void * pkt )
{
   YoYoYoActorPacket *yo = (YoYoYoActorPacket *) pkt;
   AllToAllCounter[ (*yo)[0] ][ (*yo)[1] ][ (*yo)[2] ].mVar ++;
   Platform::Memory::ExportFence((void *) &AllToAllCounter[ (*yo)[0] ][ (*yo)[1] ][ (*yo)[2] ].mVar,  sizeof(int));
   AACounter.mVar++;
   Platform::Memory::ExportFence((void *)& AACounter.mVar,  sizeof(int));

   int MyX, MyY, MyZ;
   BGLPartitionGetCoords(&MyX, &MyY, &MyZ);   

   //   s0printf("Got packet on (%d,%d,%d) from (%d,%d,%d)\n", MyX, MyY, MyZ, (*yo)[0], (*yo)[1], (*yo)[2] );
   return 0;
}
#endif

template<class FFT1, class FFT2>
void DoFFT( FFT1* aFFTForward, FFT2* aFFTReverse )
{
  
  int FFT_ITERATIONS = 2;
  
  for( int i=0; i < FFT_ITERATIONS; i++)
    {
      
      fftForwardTracerStart.HitOE( PKTRACE_TUNE,
                                   "fftForward_DoFFT_Start", 
                                   Platform::Thread::GetId(),
                                   fftForwardTracerStart );
      aFFTForward->DoFFT();
      
      fftForwardTracerFinis.HitOE( PKTRACE_TUNE,
                                   "fftForward_DoFFT_Finis", 
                                   Platform::Thread::GetId(),
                                   fftForwardTracerFinis );  
  
#ifdef DO_REVERSE
      fft3d_complex checksum;
      checksum.re=0.0;
      checksum.im=0.0;      

      for( int i=0; i<FFT1::LOCAL_N_X; i++)
        for( int j=0; j<FFT1::LOCAL_N_Y; j++)
          for( int k=0; k<FFT1::LOCAL_N_Z; k++)
            {
              fft3d_complex Elem = aFFTForward->GetRecipSpaceElement( i, j, k );
              checksum.re += Elem.re;
              checksum.im += Elem.im;
              
              aFFTReverse->PutRecipSpaceElement( i, j, k,  Elem);
            }
      
      if( ( FFT1::GLOBAL_N_X == 64 ) && ( FFT1::GLOBAL_N_Y == 64 ) && ( FFT1::GLOBAL_N_Z == 64 ) )
        {
          // double csRe_fftw = 0xBED2780000000000ll;
          // double csIm_fftw = 0xBED4400000000000ll;
          
          double csRe_bgl = 0x3EB0800000000000ll;
          double csIm_bgl = 0xBEDEC00000000000ll;

          // Re = ( 3EB0800000000000 )
          // Im = ( BEDEC00000000000 )

          double reDiff = checksum.re - csRe_bgl;
          double imDiff = checksum.im - csIm_bgl;
          
          double tolerance = 1E-15;
          
          if( reDiff  > tolerance || imDiff > tolerance )
            {
              BegLogLine( 1 )
                << "ERROR:: CHECKSUM TEST FAILED"
                << " reDiff=" << reDiff
                << " imDiff=" << imDiff
                << EndLogLine;
            }
          else
            {
              BegLogLine( PKFXLOG_SUCCESS )
                << "FFT3D "
                << " mesh size: (" << FFT1::GLOBAL_N_X << "," << FFT1::GLOBAL_N_Y << "," << FFT1::GLOBAL_N_Z << ") "
                << " proc size: (" << FFT1::P_X << "," << FFT1::P_Y << "," << FFT1::P_Z << ") "
                << "checksum test PASSED."
                << EndLogLine;              
            }
//           else
//             {              
//               int * re = (int *) &checksum.re;
//               int * im = (int *) &checksum.im;
              
//               s0printf("checkSum(64cubed).Re = ( %08X%08X )\n", re[ 0 ], re[ 1 ] );         
//               s0printf("checkSum(64cubed).Im = ( %08X%08X )\n", im[ 0 ], im[ 1 ] );         
//             }
        }
      
      BegLogLine( PKFXLOG_DEBUG )
        << "_CP_main: "
        << " About to call fftReverse->DoFFT()"
        << EndLogLine;
      
      fftReverseTracerStart.HitOE( PKTRACE_TUNE,
                                   "fftReverse_DoFFT_Start", 
                                   Platform::Thread::GetId(),
                                   fftReverseTracerStart );
      
      aFFTReverse->DoFFT();
      
      fftReverseTracerFinis.HitOE( PKTRACE_TUNE,
                                   "fftReverse_DoFFT_Finis", 
                                   Platform::Thread::GetId(),
                                   fftReverseTracerFinis );      
      
      double fftSz = 1./ (double) ( FFT1::GLOBAL_N_X * FFT1::GLOBAL_N_Y * FFT1::GLOBAL_N_Z );
      aFFTReverse->ScaleIn( fftSz );
      
#endif
      
#if PKTRACE_TUNE
      if( i != FFT_ITERATIONS-1 )
        {
          ForceExportFence();
          pkTraceServer::Reset();
          Platform::Memory::ExportFence();
          ForceImportFence();
        }      
      
      BGLPartitionBarrier();  
#endif
    }
  
  return;
}

template<class FFT1, class FFT2>
int RegressFFT( FFT1* aFFTForward, FFT2* aFFTReverse )
{
    double MaxAbsDiffRe = 0.0;
    double MaxAbsDiffIm = 0.0;

    int error = 0;
    double tolerance = 1.0 / 1000000000.0;

    for( int x = 0; x < FFT1::LOCAL_N_X; x++ )
      for( int y = 0; y < FFT1::LOCAL_N_Y; y++ )
        for( int z = 0; z < FFT1::LOCAL_N_Z; z++ )
          {
              double AbsDiffRe = fabs( aFFTForward->mLocalIn[ x ][ y ][ z ].re - aFFTReverse->mLocalIn[ x ][ y ][ z ].re );
              double AbsDiffIm = fabs( aFFTForward->mLocalIn[ x ][ y ][ z ].im - aFFTReverse->mLocalIn[ x ][ y ][ z ].im );
              
              if( MaxAbsDiffRe < AbsDiffRe )
                  MaxAbsDiffRe = AbsDiffRe;
              if( MaxAbsDiffIm < AbsDiffIm )
                  MaxAbsDiffIm = AbsDiffIm;
              
              BegLogLine( 0 )
                << " MaxAbsDiffRe=" << MaxAbsDiffRe
                << " MaxAbsDiffIm=" << MaxAbsDiffIm
                << EndLogLine;
              
              if(  AbsDiffRe > tolerance || AbsDiffIm > tolerance )
                error = 1;
          }
    
    BegLogLine( PKFXLOG_SUCCESS )
        << "FFT3D "
        << " mesh size: (" << FFT1::GLOBAL_N_X << "," << FFT1::GLOBAL_N_Y << "," << FFT1::GLOBAL_N_Z << ") "
        << " proc size: (" << FFT1::P_X << "," << FFT1::P_Y << "," << FFT1::P_Z << ") "
        << "internal test (forward[ x ][y][z] - reverse[x][y][z]) < tolerance : "
        << EndLogLine;

    BegLogLine( PKFXLOG_SUCCESS )
      << "FFT3D "
      << ( error ? "FAILED :-( " : "SUCCEEDED :-) " )
      << " Tolerance used "
      << tolerance
      << " MaxAbsDiff (re,im) "
      << MaxAbsDiffRe
      << ","
      << MaxAbsDiffIm
      << EndLogLine;

   return  error; 
      
}

extern "C" {
void*  _CP_main1( int argc, char *argv[], char **envp )
#else
extern "C" {
int _CP_main( int argc, char *argv[], char **envp )
#endif
  {
    // s0printf("_CP_main() reporting via fxlogging!\n");

//     BegLogLine( 0 )
//       << "_CP_main() reporting via fxlogging!"
//       << " Compiled on " << __DATE__ << " at " << __TIME__
//       << EndLogLine;

    // s0printf("_CP_main() after log line!\n");

    int maxX, maxY, maxZ;
    BGLPartitionGetDimensions( &maxX, &maxY, &maxZ );

    int MyX, MyY, MyZ;
    BGLPartitionGetCoords(&MyX, &MyY, &MyZ);   


#if PKTRACE_TUNE
    pkTraceServer::Init();  
#endif

  int myRank;
  myRank = BGLPartitionGetRankXYZ();

  int pX1 = FFT_PLAN1::P_X, pY1 = FFT_PLAN1::P_Y, pZ1 = FFT_PLAN1::P_Z;
  int pX2 = FFT_PLAN2::P_X, pY2 = FFT_PLAN2::P_Y, pZ2 = FFT_PLAN2::P_Z;

  if( myRank == 0 )
    {
    if( maxX != pX2 || maxY != pY2 || maxZ != pZ2 )
      {
      BegLogLine( 1 )
        << "FFT 3D FAILED "
        << " Not built for correct partition size "
        << " Built for: "
        << pX2 << " " << pY2 << " " << pZ2 << " "
        << " Partitions dim: "
        << maxX << " " << maxY << " " << maxZ << " "
        << EndLogLine;
      PLATFORM_ABORT( "executable built for fft size that doesn't match physical partition size" );
      }
    }

  int globalNx1 = FFT_PLAN1::GLOBAL_SIZE_X;
  int globalNy1 = FFT_PLAN1::GLOBAL_SIZE_Y;
  int globalNz1 = FFT_PLAN1::GLOBAL_SIZE_Z;

  int globalNx2 = FFT_PLAN2::GLOBAL_SIZE_X;
  int globalNy2 = FFT_PLAN2::GLOBAL_SIZE_Y;
  int globalNz2 = FFT_PLAN2::GLOBAL_SIZE_Z;

   int localNx1 = (int) ( globalNx1 / pX1 );
   int localNy1 = (int) ( globalNy1 / pY1 );
   int localNz1 = (int) ( globalNz1 / pZ1 );

   int localNx2 = (int) ( globalNx2 / pX2 );
   int localNy2 = (int) ( globalNy2 / pY2 );
   int localNz2 = (int) ( globalNz2 / pZ2 );

  if( myRank == 0 )
    {
    BegLogLine ( PKFXLOG_DEBUG )
      << " 3D FFT of size: " << globalNx2 << "," << globalNy2 << "," << globalNz2
      << " Local FFT sizes: " << localNx2 << "," << localNy2 << "," << localNz2
      << " Processors mesh: " << pX2 << "," << pY2 << "," << pZ2
      << EndLogLine;

    BegLogLine ( 0 )
      << "PkMain "
      << " sizeof(_BGL_TorusPktPayload)=" << (int) sizeof(_BGL_TorusPktPayload)
      << " sizeof(fft3d_complex)=" << (int) sizeof(fft3d_complex)      
      << EndLogLine;
    }

  

  // unsigned myKend = GetExtendedMemory();

//   BegLogLine( PKFXLOG_DEBUG )
//     << "_CP_main: "
//     << " myKend=" << (void *)myKend
//     << EndLogLine;

//   fftForward = (FFT_FORWARD *) myKend;
//   myKend += sizeof(FFT_FORWARD);
  // Platform::Memory::ExportFence( fftForward, sizeof(FFT_FORWARD) );

  //  pkNew( &fftForward1, 1);
  pkNew( &fftForward2, 1);
  
//   BegLogLine( PKFXLOG_DEBUG )
//     << "_CP_main: "
//     << " fftForward=" << (void *)fftForward1
//     << " sizeof( FFT3D< FFT_PLAN1, FORWARD > )=" << sizeof(FFT_FORWARD1)
//     << EndLogLine;

  //  fftForward1->Init();
  fftForward2->Init();

  BegLogLine( PKFXLOG_DEBUG )
      << "_CP_main: "
      << " After fftForward->Init() "
      << EndLogLine;
  
  #ifdef DO_REVERSE
  //    pkNew( &fftReverse1, 1);
    pkNew( &fftReverse2, 1);
    
//     BegLogLine( PKFXLOG_DEBUG )
//         << "_CP_main: "
//         << " fftReverse=" << (void *) fftReverse1
//         << " sizeof( FFT_REVERSE )=" << sizeof(FFT_REVERSE1)
//         << EndLogLine;
    
    //    fftReverse1->Init();
    fftReverse2->Init();
    
    BegLogLine( PKFXLOG_DEBUG )
        << "_CP_main: "
        << " After fftReverse->Init() "
        << EndLogLine;     
  #endif

  // This mutex is really making sure the second proc doesn't use data structs before above init.
  // THis should really be handled inside the Init routine itself.
  //  Platform::Mutex::Unlock( &FFT_InitMutex );

#ifdef REGRESS_FFTW
    for(int x=0; x < globalNx2; x++)
      for(int y=0; y < globalNy2; y++)
        for(int z=0; z < globalNz2; z++)
          {
            double data = drand48(); 

            src[ x ][ y ][ z ].re = data;
            src[ x ][ y ][ z ].im = 0.0 ;
          }

    for( int i=0; i < localNx2; i++)
        for( int j=0; j < localNy2; j++)
            for( int k=0; k < localNz2; k++)
                {
                    int a = MyX * localNx2 + i;
                    int b = MyY * localNy2 + j;
                    int c = MyZ * localNz2 + k;
                    
                    if( a >= globalNx2 || b >= globalNy2 || c >= globalNz2 )
                        {
                            printf("Error:: (a,b,c)=(%d,%d,%d)\n",a,b,c);
                            return (NULL);
                        }
                    
                    fftForward2->PutRealSpaceElement( i, j, k, src[ a ][ b ][ c ].re );
                }
#else 
//     for(unsigned int i=0; i<localNx1; i++)
//         for(unsigned int j=0; j<localNy1; j++)
//             for(unsigned int k=0; k<localNz1; k++)
//                 {
//                     double x = i*j*k;                    
//                     fftForward1->PutRealSpaceElement( i, j, k, x );
//                 }

    for(unsigned int i=0; i<localNx2; i++)
        for(unsigned int j=0; j<localNy2; j++)
            for(unsigned int k=0; k<localNz2; k++)
                {
                    double x = i*j*k;                    
                    fftForward2->PutRealSpaceElement( i, j, k, x );
                }
#endif
    

  BegLogLine( PKFXLOG_DEBUG )
    << "_CP_main: "
    << " About to call fftForward->DoFFT()"
    << EndLogLine;
  
  Platform::Mutex::Unlock( &IOP_CP_Barrier );
  BGLPartitionBarrier();      
  
  // DoFFT( fftForward1, fftReverse1 );

  //#if 0

#ifdef DO_REVERSE
#ifdef REGRESS_FFT3D_REVERSE_TEST  

  int error = 0;//RegressFFT( fftForward1, fftReverse1 );
      
  if( error )
    {
      char errorMessage[ 128 ];
      sprintf(errorMessage, "ERROR:: 3D FFT FAILED UNI test on rank=%d, node (%d,%d,%d)", 
              myRank, MyX, MyY, MyZ );
      
      // PLATFORM_ABORT( errorMessage );
    }

  Platform::Control::Barrier();

  DoFFT( fftForward2, fftReverse2 );
  int error2 = RegressFFT( fftForward2, fftReverse2 );
  if( error2 )
    {
      char errorMessage[ 128 ];
      sprintf(errorMessage, "ERROR:: 3D FFT FAILED DISTRIBUTED test on rank=%d, node (%d,%d,%d)", 
              myRank, MyX, MyY, MyZ );
      
      //PLATFORM_ABORT( errorMessage );
    }

#ifdef REGRESS_FFTW_TEXT

      for(int x=0; x < globalNx2; x++)
        for(int y=0; y < globalNy2; y++)
          for(int z=0; z < globalNz2; z++)
            {          
              double real = fftForward2->mLocalOut[ x ][ y ][ z ].re;
              double img = fftForward2->mLocalOut[ x ][ y ][ z ].im;
              
              int * re = (int *) &real;
              int * im = (int *) &img;
              
              s0printf("(%08X%08X,%08X%08X)\n", re[0],re[1],im[0],im[1]);
            }
#endif

#endif
#endif

  /****************************************************************
   *  At this point we have all the fft data and can do regression
   ****************************************************************/
      if( myRank == 0 )
        {
#ifdef DO_REGRESSION
    #ifdef REGRESS_FFTW_FORWARD

          BegLogLine(1)
            << "FFT3D "
            << "Regressing against FFTW... "
            << EndLogLine;

          fftwnd_plan forward3DPlanF =  fftw3d_create_plan( globalNx, globalNy, globalNz,
                                                            FFTW_FORWARD, FFTW_ESTIMATE );

          fftwnd_one( forward3DPlanF,
                      (fftw_complex*) &src[ 0 ][ 0 ][ 0 ],
                      (fftw_complex*) &dst1[ 0 ][ 0 ][ 0 ] );

          BegLogLine( PKFXLOG_DEBUG )
            << "Compare our FFT to FFTW"
            << EndLogLine;


          // if( compareFFTW( tmp, dst1, localNx, localNy, localNz ) == 1 )
          MaxAbsDiffRe = 0.0;
          MaxAbsDiffIm = 0.0;
          error = 0;
          for(int i=0; i < localNx; i++)
            for(int j=0; j < localNy; j++)
              for(int k=0; k < localNz; k++)
                {
                int a = MyX*localNx + i;
                int b = MyY*localNy + j;
                int c = MyZ*localNz + k;

                double AbsDiffRe = fabs( fftForward->mLocalOut[ i ][ j ][ k ].re - dst1[ a ][ b ][ c ].re );
                double AbsDiffIm = fabs( fftForward->mLocalOut[ i ][ j ][ k ].im - dst1[ a ][ b ][ c ].im );

                if( MaxAbsDiffRe < AbsDiffRe )
                  MaxAbsDiffRe = AbsDiffRe;
                if( MaxAbsDiffIm < AbsDiffIm )
                  MaxAbsDiffIm = AbsDiffIm;

                if(  AbsDiffRe > tolerance || AbsDiffIm > tolerance )
                //////////if(    fabs( fftForward.mLocalOut[ i ][ j ][ k ].re - dst1[ a ][ b ][ c ].re ) > tolerance
                //////////    || fabs( fftForward.mLocalOut[ i ][ j ][ k ].im - dst1[ a ][ b ][ c ].im ) > tolerance )
                  {
                  // printf("Coord: %d,%d,%d \n", MyX, MyY, MyZ );
                  // printf("fftFWD.mLocalOut[ %d ][ %d ][ %d ]=%f\n", i, j, k, fftForward.mLocalOut[ i ][ j ][ k ].re);
                  // printf("dst1[ %d ][ %d ][ %d ]=%f\n", a, b, c, dst1[ a ][ b ][ c ].re);
                  error = 1;
                  }
                }

          // NOTE: error is sense IN THE LOG LINE to create the message.
          BegLogLine(1)
            << "FFT3D "
            << "REGRESSION OF FORWARD RESULTS WITH FFTW "
            << ( error ? "FAILED :-( " : "SUCCEEDED :-) " )
            << " Tolerance used "
            << FormatString("%g")
            << tolerance
            << " MaxAbsDiff (re,im) "
            << FormatString("%g")
            << MaxAbsDiffRe
            << ","
            << FormatString("%g")
            << MaxAbsDiffIm
            << EndLogLine;

    #endif

    #ifdef DO_REVERSE
    #ifdef REGRESS_FFTW_REVERSE
          fftwnd_plan forward3DPlanR =  fftw3d_create_plan( globalNx, globalNy, globalNz,
                                                            FFTW_BACKWARD, FFTW_ESTIMATE );

          fftwnd_one( forward3DPlanR,
                      (fftw_complex *) &dst1[ 0 ][ 0 ][ 0 ],
                      (fftw_complex *) &final[ 0 ][ 0 ][ 0 ] );


          double fftSz1 = 1./(double)( globalNx * globalNy * globalNz );
          for(int x=0; x < globalNx; x++)
            for(int y=0; y < globalNy; y++)
              for(int z=0; z < globalNz; z++)
                {
                  final[ x ][ y ][ z ].re = fftSz1 * final[ x ][ y ][ z ].re;
                  final[ x ][ y ][ z ].im = fftSz1 * final[ x ][ y ][ z ].im;
                }

          MaxAbsDiffRe = 0.0;
          MaxAbsDiffIm = 0.0;
          error=0;
          for(int i=0; i < localNx; i++)
            for(int j=0; j < localNy; j++)
              for(int k=0; k < localNz; k++)
                {
                int a = MyX * localNx + i;
                int b = MyY * localNy + j;
                int c = MyZ * localNz + k;

                double AbsDiffRe = fabs( fftReverse->mLocalIn[ i ][ j ][ k ].re - final[ a ][ b ][ c ].re );
                double AbsDiffIm = fabs( fftReverse->mLocalIn[ i ][ j ][ k ].im - final[ a ][ b ][ c ].im );

                if( MaxAbsDiffRe < AbsDiffRe )
                  MaxAbsDiffRe = AbsDiffRe;
                if( MaxAbsDiffIm < AbsDiffIm )
                  MaxAbsDiffIm = AbsDiffIm;

                if(  AbsDiffRe > tolerance || AbsDiffIm > tolerance )
                  {
                  error = 1;
                  }

                //////////if(  AbsDiffRe > tolerance || AbsDiffIm > tolerance )
                /////////////////////if(    fabs( fftReverse.mLocalIn[ i ][ j ][ k ].re - final[ a ][ b ][ c ].re ) > tolerance
                /////////////////////    || fabs( fftReverse.mLocalIn[ i ][ j ][ k ].im - final[ a ][ b ][ c ].im ) > tolerance )
                //////////  {
                //////////  // printf("Coord: %d,%d,%d \n", MyX, MyY, MyZ);
                //////////  // printf("fftReverse.mLocalIn[ %d ][ %d ][ %d ]=%f\n", i, j, k, fftReverse.mLocalIn[ i ][ j ][ k ].re);
                //////////  // printf("final[ %d ][ %d ][ %d ]=%f\n", a, b, c, final[ a ][ b ][ c ].re);
                //////////  error=1;
                //////////  }
                }

          // NOTE: error is sense IN THE LOG LINE to create the message.
          BegLogLine(1)
            << "FFT3D "
            << "REGRESSION OF REVERSE RESULTS WITH FFTW "
            << ( error ? "FAILED :-( " : "SUCCEEDED :-) " )
            << " Tolerance used "
            << FormatString("%g")
            << tolerance
            << " MaxAbsDiff (re,im) "
            << FormatString("%g")
            << MaxAbsDiffRe
            << ","
            << FormatString("%g")
            << MaxAbsDiffIm
            << EndLogLine;

    #endif
    #endif
    #endif
    }

    BGLPartitionBarrier();  

#if PKTRACE_TUNE  
    
    ForceExportFence();
    
    Platform::Memory::ExportFence();
        
    for( int i=0; i <  BGLPartitionGetSize() ; i++ ) 
      {       
        if( i == myRank )
          pkTraceServer::FlushAllBothBuffers();

        BGLPartitionBarrier();
      }
#endif  

    BegLogLine( 1 )
      << "End of test."
      << EndLogLine;

    our_check_for_link_errors(MyX, MyY, MyZ);   

#ifdef PK_BLADE
    BGLPartitionExit(0);
#endif

    //#endif
  return ( NULL );
  }

  //#ifdef NO_PK_PLATFORM
};
  // #endif

#ifdef NO_PK_PLATFORM
// Just dispatch active packets as they arrive.
extern "C" {
int _IOP_main( int argc, char *argv[], char **envp )
  {
  int rc = 0;

  BegLogLine( 1 )
    << "_IOP_main() running ... reporting via fxlogging!"
    << EndLogLine;

  // Hold off until released by the compute node.
  //   Platform::Mutex::YieldLock( &FFT_InitMutex );

  Platform::Memory::ImportFence();

  #ifdef USE_2_CORES_FOR_FFT

  rc = BGLTorusWait();  // wait on both fifo groups

  BegLogLine(1)
    << "_IOP_main: All Packets Received, returning"
    << " BGLTorusWait rc "
    << rc
    << EndLogLine;

  #endif

  return(rc);
  }
};
#endif

