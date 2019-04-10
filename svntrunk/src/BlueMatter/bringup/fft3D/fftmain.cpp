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

#include <BlueMatter/fft3D.hpp>
#include <assert.h>

// #include <blade.h>


// #ifdef USE_SETPRI
// #include "setpri.hpp"
// #endif // USE_SETPRI



#include <math.h>

Platform::Lock::LockWord   LogMutex = 0;

#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG ( 1 )
#endif

#ifndef PKFXLOG_SUCCESS
#define PKFXLOG_SUCCESS ( 1 )
#endif

struct FFT_PLAN1
  {
  enum
    {
    P_X = 1,
    P_Y = 2,
    P_Z = 1
    };

  enum
    {
    GLOBAL_SIZE_X = 4,
    GLOBAL_SIZE_Y = 4,
    GLOBAL_SIZE_Z = 4
    };
  };

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
complex_without_constructor src [ FFT_PLAN1::GLOBAL_SIZE_X ][ FFT_PLAN1::GLOBAL_SIZE_Y ][ FFT_PLAN1::GLOBAL_SIZE_Z ];
complex_without_constructor dst1[ FFT_PLAN1::GLOBAL_SIZE_X ][ FFT_PLAN1::GLOBAL_SIZE_Y ][ FFT_PLAN1::GLOBAL_SIZE_Z ];
#endif

#ifdef REGRESS_FFTW_REVERSE
complex_without_constructor final[ FFT_PLAN1::GLOBAL_SIZE_X ][ FFT_PLAN1::GLOBAL_SIZE_Y ][ FFT_PLAN1::GLOBAL_SIZE_Z ];
#endif

FFT3D< FFT_PLAN1, FORWARD >* fftForward;
#ifdef DO_REVERSE
FFT3D< FFT_PLAN1, REVERSE >* fftReverse;
#endif

Platform::Lock::LockWord   FFT_InitMutex = 1;

#ifndef NO_PK_PLATFORM

extern int _CP_main1( int argc, char *argv[], char **envp );

int
PkMain( int argc, char ** argv, char ** envp )
{
    return _CP_main1( argc, argv, envp );
}

// #define _BN( n ) (0x01 << n)

int _CP_main1( int argc, char *argv[], char **envp )
#else
extern "C" {
int _CP_main( int argc, char *argv[], char **envp )
#endif
  {

  BegLogLine( 1 )
    << "_CP_main() reporting via fxlogging!"
    << " Compiled on " << __DATE__ << " at " << __TIME__ 
    << EndLogLine;

  int myRankmyRankmyRankmyRankmyRankmyRank;
  myRankmyRankmyRankmyRankmyRankmyRank = BGLPartitionGetRankXYZ();

  #ifdef USE_SETPRI
    //   int myRank1 = BGLPartitionGetRankXYZ();
    int randomD = (int) ceil(drand48() * myRank);
    int randomF = randomD % 60;

    printf("CP_MAIN(%d):: Random time to sleep: %d\n", myRank, randomF);
    sleep( randomD );
    SchedTuneSelf();

    printf("CP_MAIN(%d):: Before barrier\n", myRank);
    BGLPartitionBarrier();
  #endif

  #ifdef PK_BLMPI
    MPI_Init(&argc, &argv);
    BladeInit(&argc, &argv);
  #endif

  #ifdef PK_BLMPI
    AttachDebugger( argv[ 0 ] );
  #endif

  int maxX, maxY, maxZ;
  BGLPartitionGetDimensions( &maxX, &maxY, &maxZ );

  int pX = FFT_PLAN1::P_X, pY = FFT_PLAN1::P_Y, pZ = FFT_PLAN1::P_Z;

  if( myRankmyRankmyRankmyRankmyRankmyRank == 0 )
    {
    if( maxX != pX || maxY != pY || maxZ != pZ )
      {
      BegLogLine(1)
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

  int globalNx = FFT_PLAN1::GLOBAL_SIZE_X;
  int globalNy = FFT_PLAN1::GLOBAL_SIZE_Y;
  int globalNz = FFT_PLAN1::GLOBAL_SIZE_Z;

//   int localNx = (int) (ceil( (double) globalNx / (double) pX ));
//   int localNy = (int) (ceil( (double) globalNy / (double) pY ));
//   int localNz = (int) (ceil( (double) globalNz / (double) pZ ));
   int localNx = (int) ( globalNx / pX );
   int localNy = (int) ( globalNy / pY );
   int localNz = (int) ( globalNz / pZ );

  if(myRankmyRankmyRankmyRankmyRankmyRank == 0)
    {
    BegLogLine ( PKFXLOG_DEBUG )
      << " 3D FFT of size: " << globalNx << "," << globalNy << "," << globalNz
      << " Local FFT sizes: " << localNx << "," << localNy << "," << localNz
      << " Processors mesh: " << pX << "," << pY << "," << pZ
      << EndLogLine;
    }

  int MyX, MyY, MyZ;
  BGLPartitionGetCoords(&MyX, &MyY, &MyZ);

  int error = 0;
  double tolerance = 1.0 / 1000000000.0;

  unsigned myKend = GetExtendedMemory();

  BegLogLine(1)
    << "_CP_main: "
    << " myKend=" << (void *)myKend
    << EndLogLine;
  
  fftForward = (FFT3D< FFT_PLAN1, FORWARD > *) myKend; 

  BegLogLine(1)
    << "_CP_main: "
    << " fftForward=" << (void *)fftForward
    << " sizeof( FFT3D< FFT_PLAN1, FORWARD > )=" << sizeof(FFT3D< FFT_PLAN1, FORWARD >)
    << EndLogLine;
  

  fftForward->Init();

  BegLogLine(1)
    << "_CP_main: "
    << " After fftForward->Init() "
    << EndLogLine;

   #ifdef DO_REVERSE
    unsigned fftForwardEndPtr = myKend + sizeof(FFT3D< FFT_PLAN1, FORWARD >);
    unsigned fftReversePtr = fftForwardEndPtr + ((16*1024*1024) - fftForwardEndPtr % (16*1024*1024));

    fftReverse = (FFT3D< FFT_PLAN1, REVERSE > *) ( fftReversePtr ); 

    BegLogLine(1)
      << "_CP_main: "
      << " fftReverse=" << (void *) fftReverse
      << " sizeof( FFT3D< FFT_PLAN1, REVERSE > )=" << sizeof(FFT3D< FFT_PLAN1, REVERSE >)
      << EndLogLine;

    fftReverse->Init();

    BegLogLine(1)
      << "_CP_main: "
      << " After fftReverse->Init() "
      << EndLogLine;

   #endif

  // This mutex is really making sure the second proc doesn't use data structs before above init.
  // THis should really be handled inside the Init routine itself.
  //  Platform::Mutex::Unlock( &FFT_InitMutex );

  #ifdef REGRESS_FFTW
    for(int x=0; x < globalNx; x++)
      for(int y=0; y < globalNy; y++)
        for(int z=0; z < globalNz; z++)
          {
            double data = drand48(); //(x*GlobalNy+y)*GlobalNz+z;
            //////src[ x ][ y ][ z ] = complex( data, 0.0 );
            src[ x ][ y ][ z ].re = data;
            src[ x ][ y ][ z ].im = 0.0 ;
          }

    for( int i=0; i < localNx; i++)
      for( int j=0; j < localNy; j++)
        for( int k=0; k < localNz; k++)
          {
          int a = MyX * localNx + i;
          int b = MyY * localNy + j;
          int c = MyZ * localNz + k;

          if( a >= globalNx || b >= globalNy || c >= globalNz )
            {
            printf("Error:: (a,b,c)=(%d,%d,%d)\n",a,b,c);
            return -1;
            }

          // in[ i ][ j ][ k ].re = src[ a ][ b ][ c ].re;
          // in[ i ][ j ][ k ].im = src[ a ][ b ][ c ].im;
          fftForward->PutRealSpaceElement( i, j, k, src[ a ][ b ][ c ].re );
          }
  #else // ! REGRESS_FFTW
    for(unsigned int i=0; i<localNx; i++)
      for(unsigned int j=0; j<localNy; j++)
        for(unsigned int k=0; k<localNz; k++)
          {
            double x = i*j*k;//drand48(); //(double)((i*localNy+j)*localNz+k);

            // in[ i ][ j ][ k ].re = x;
            // in[ i ][ j ][ k ].im = 0.0;
            fftForward->PutRealSpaceElement( i, j, k, x );
          }
  #endif


  BegLogLine(1)
    << "_CP_main: "
    << " About to call fftForward->DoFFT()"
    << EndLogLine;

  fftForward->DoFFT();

  #ifdef DO_REVERSE
    for( int i=0; i<localNx; i++)
      for( int j=0; j<localNy; j++)
        for( int k=0; k<localNz; k++)
          {
          fftReverse->PutRecipSpaceElement( i, j, k, fftForward->GetRecipSpaceElement( i, j, k ) );
          }

  BegLogLine(1)
    << "_CP_main: "
    << " About to call fftReverse->DoFFT()"
    << EndLogLine;

    fftReverse->DoFFT();

    double fftSz = 1./(double)( globalNx * globalNy * globalNz );
    fftReverse->ScaleIn( fftSz );

  #endif

  #ifdef DO_REVERSE
  #ifdef REGRESS_FFT3D_REVERSE_TEST

    double MaxAbsDiffRe = 0.0;
    double MaxAbsDiffIm = 0.0;

    BegLogLine(1)
      << " fftmain:: "
      << " about to do the REGRESS_FFT3D_REVERSE_TEST "
      << " on rank=" << myRankmyRankmyRankmyRankmyRankmyRank
      << EndLogLine;

    
    s0printf("Value of myRank=%d\n", myRankmyRankmyRankmyRankmyRankmyRank);
    //   if ( myRankmyRankmyRankmyRankmyRankmyRank == 0)
      {

    BegLogLine(1)
      << " fftmain:: "
      << " rank=" << myRankmyRankmyRankmyRankmyRankmyRank
      << EndLogLine;

      for( int x = 0; x < localNx; x++ )
        for( int y = 0; y < localNy; y++ )
          for( int z = 0; z < localNz; z++ )
            {
            double AbsDiffRe = fabs( fftForward->mLocalIn[ x ][ y ][ z ].re - fftReverse->mLocalIn[ x ][ y ][ z ].re );
            double AbsDiffIm = fabs( fftForward->mLocalIn[ x ][ y ][ z ].im - fftReverse->mLocalIn[ x ][ y ][ z ].im );

            if( MaxAbsDiffRe < AbsDiffRe )
              MaxAbsDiffRe = AbsDiffRe;
            if( MaxAbsDiffIm < AbsDiffIm )
              MaxAbsDiffIm = AbsDiffIm;

            BegLogLine( 0 )
              << " MaxAbsDiffRe=" << MaxAbsDiffRe
              << " MaxAbsDiffIm=" << MaxAbsDiffIm
              << EndLogLine;

            if(  AbsDiffRe > tolerance || AbsDiffIm > tolerance )
              {
              error = -1;
              }

            ////////////if( fabs( fftForward.mLocalIn[ x ][ y ][ z ].re - fftReverse.mLocalIn[ x ][ y ][ z ].re ) > tolerance )
            ////////////  {
            ////////////    //printf("ERROR::fftForward.mLocal[%d][%d][%d].re = %f\n", x, y, z, fftForward.mLocalIn[ x ][ y ][ z ].re  );
            ////////////    //printf("ERROR::fftReverse.mLocal[%d][%d][%d].re = %f\n", x, y, z, fftReverse.mLocalIn[ x ][ y ][ z ].re  );
            ////////////    error = 1;
            ////////////  }
            ////////////
            ////////////if( fabs( fftForward.mLocalIn[ x ][ y ][ z ].im - fftReverse.mLocalIn[ x ][ y ][ z ].im ) > tolerance )
            ////////////  {
            ////////////    //printf("ERROR::fftForward.mLocal[%d][%d][%d].im = %f\n", x, y, z, fftForward.mLocalIn[ x ][ y ][ z ].im  );
            ////////////    //printf("ERROR::fftReverse.mLocal[%d][%d][%d].im = %f\n", x, y, z, fftReverse.mLocalIn[ x ][ y ][ z ].im  );
            ////////////    error = 1;
            ////////////  }
            }

      BegLogLine(1)
        << "FFT3D "
        << "internal test (forward[x][y][z] - reverse[x][y][z]) < tolerance : "
        << ( error ? "FAILED :-( " : "SUCCEEDED :-) " )
        << " Tolerance used "
        //<< FormatString("%g")
        << tolerance
        << " MaxAbsDiff (re,im) "
        //        << FormatString("%g")
        << MaxAbsDiffRe
        << ","
        //        << FormatString("%g")
        << MaxAbsDiffIm
        << EndLogLine;

      }
  #endif
  #endif

  /****************************************************************
   *  At this point we have all the fft data and can do regression
   ****************************************************************/
    if( myRankmyRankmyRankmyRankmyRankmyRank == 0 )
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

  BegLogLine( PKFXLOG_DEBUG )
    << "Finito"
    << EndLogLine;

  #ifdef PK_BLADE
    BGLPartitionExit(0);
  #endif

  return ( 1 );
  }

#ifdef NO_PK_PLATFORM
}
#endif

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

