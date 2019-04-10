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


#include <fft3d/fft3d_clean_routing_rr_sends_fifo.hpp>
Platform::Lock::LockWord   LogMutex;

#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG ( 0 )
#endif

#ifndef PKTRACE_TUNE
#define PKTRACE_TUNE ( 1 )
#endif

#ifndef PKFXLOG_SUCCESS
#define PKFXLOG_SUCCESS ( 1 )
#endif

struct FFT_PLAN
  {
  enum
    {
    GLOBAL_SIZE_X = 128,
    GLOBAL_SIZE_Y = 128,
    GLOBAL_SIZE_Z = 128
    };
  };

typedef  FFT3D< FFT_PLAN > FFT_TYPE FFT3D_STUCT_ALIGNMENT;

FFT_TYPE* FastFourierTransform;

extern Platform::Mutex::MutexWord IOP_CP_Barrier;

#ifndef NO_PK_PLATFORM 
extern "C" {

  extern void* _CP_main1( int argc, char *argv[], char **envp );

  void *
  PkMain( int argc, char ** argv, char ** envp )
  {
    _CP_main1( argc, argv, envp );
    return ( NULL );
  }
};

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

extern "C" {
void*  _CP_main1( int argc, char *argv[], char **envp )
#else
extern "C" {
int _CP_main( int argc, char *argv[], char **envp )
#endif
  {
    int maxX, maxY, maxZ;
    BGLPartitionGetDimensions( &maxX, &maxY, &maxZ );

    int MyX, MyY, MyZ;
    BGLPartitionGetCoords(&MyX, &MyY, &MyZ);   


#if PKTRACE_TUNE
    pkTraceServer::Init();  
#endif

  int myRank;
  myRank = BGLPartitionGetRankXYZ();

  if( argc != 4 )
    {
      PLATFORM_ABORT("ERROR:: FFT takes 3 arguments: px py pz");
    }
  
//   int pX = htoi( argv[ 1 ] );
//   int pY = htoi( argv[ 2 ] );
//   int pZ = htoi( argv[ 3 ] );

 int pX = maxX;
 int pY = maxY;
 int pZ = maxZ;
  
  if( myRank == 0 )
    {
    if( maxX != pX || maxY != pY || maxZ != pZ )
      {
      BegLogLine( 1 )
        << "FFT 3D FAILED "
        << " Not built for correct partition size "
        << " Built for: "
        << pX << " " << pY << " " << pZ << " "
        << " Partitions dim: "
        << maxX << " " << maxY << " " << maxZ << " "
        << EndLogLine;
      PLATFORM_ABORT( "executable built for fft size that doesn't match physical partition size" );
      }
    }

  int globalNx = FFT_PLAN::GLOBAL_SIZE_X;
  int globalNy = FFT_PLAN::GLOBAL_SIZE_Y;
  int globalNz = FFT_PLAN::GLOBAL_SIZE_Z;
  
  int localNx = (int) ( globalNx / pX );
  int localNy = (int) ( globalNy / pY );
  int localNz = (int) ( globalNz / pZ );

  if( myRank == 0 )
    {
    BegLogLine ( PKFXLOG_DEBUG )
      << " 3D FFT of size: " << globalNx << "," << globalNy << "," << globalNz
      << " Local FFT sizes: " << localNx << "," << localNy << "," << localNz
      << " Processors mesh: " << pX << "," << pY << "," << pZ
      << EndLogLine;
    }

  pkNew( &FastFourierTransform, 1);

  BegLogLine( PKFXLOG_DEBUG )
      << "_CP_main: "
      << " After pkNew( FFT ) "
      << EndLogLine; 
  
  FastFourierTransform->Init( &FastFourierTransform, pX, pY, pZ );

  BegLogLine( PKFXLOG_DEBUG )
      << "_CP_main: "
      << " After fft->Init() "
      << EndLogLine;
  
  for( int iter=0; iter < 1; iter ++ )
    {
#ifdef TEST_FFT_DATA_MOVEMENT
      FastFourierTransform->InitCoords(); 
#else
      for(unsigned int i=0; i<localNx; i++)
        for(unsigned int j=0; j<localNy; j++)
          for(unsigned int k=0; k<localNz; k++)
            {
              double x = i*j*k;                    
              FastFourierTransform->PutRealSpaceElement( i, j, k, x );
            }
#endif            

      BegLogLine( 0 )
        // BegLogLine( PKFXLOG_DEBUG )
        << "_CP_main: "
        << " fft->RunForward() about to start"
        << EndLogLine;
      
#if defined( PK_BLADE_PROFILING )
      _blade_profile_on();
#endif

#ifdef FFT3D_BARRIER_PHASES
      BGLPartitionBarrier();  
#endif
      
      long long StartTime = a_GetTimeBase();
      
      FastFourierTransform->RunForward(); 

      long long FinishTime = a_GetTimeBase();      

      s0printf("RunForward(): StartTime=%Ld, FinishTime=%Ld, Diff: %Ld\n", StartTime, FinishTime, (FinishTime-StartTime));

      FastFourierTransform->Reset();       

#if defined( PK_BLADE_PROFILING )
      _blade_profile_off();
#endif

#ifdef TEST_FFT_DATA_MOVEMENT
      FastFourierTransform->CheckDataMovementAfterFWD();
#endif
        
#ifdef FFT3D_BARRIER_PHASES
      BGLPartitionBarrier();
#endif

      // FastFourierTransform->RunReverse(); 
      
      // BegLogLine( PKFXLOG_DEBUG )
      BegLogLine( 0 )        
        << "_CP_main: "
        << " fft->RunReverse() finished"
        << EndLogLine;
    }

#if PKTRACE_TUNE      
    ForceExportFence();
    
    Platform::Memory::ExportFence();
        
    for( int i=0; i <  BGLPartitionGetSize() ; i++ ) 
      {       
        if( i == myRank )
          pkTraceServer::FlushAllBothBuffers();

        // BGLPartitionBarrier();
      }
#endif  

    BegLogLine( 1 )
      << "End of test."
      << EndLogLine;

    our_check_for_link_errors(MyX, MyY, MyZ);   

#ifndef PK_BLADE_PROFILING    
    BGLPartitionExit(0);
#endif

  return ( NULL );
  }
};

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
