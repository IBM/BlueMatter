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
//  Name : Maria Eleftheriou
//  fftmain.hpp
//  tests the 3D fft using MPI and fftw

#include <iostream>
using namespace std;

#include "fft3D.hpp"
#include <cassert>

#ifndef BUILD_FOR_BLNIE
#include "BladeMpi.hpp"
#else
#include "blade.h"
#endif

#include <math.h>
//#include "../fftw/complex.hpp"
#include "fxlogger.hpp"

#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG 0
#endif

#define PX     2
#define PY     2
#define PZ     2

#define GlobalNx 4
#define GlobalNy 4 
#define GlobalNz 4

// #define GlobalNx ( 2 * PZ * PX )
// #define GlobalNy ( 2 * PX * PY )
// #define GlobalNz ( 2 * PY * PZ )


//#define  REGRESS_REVERSE 1
#define REGRESS_FFT3DW 1

#ifdef REGRESS_FFT3DW
#include "fftw.h"
#endif

#ifdef REGRESS_FFT3DW
complex src[ GlobalNx ][ GlobalNy ][ GlobalNz ];
complex dst1[ GlobalNx ][ GlobalNy ][ GlobalNz ];
#endif

#ifdef USE_FFTW_ONED
#define FFT_DIRF FFTW_FORWARD
#define FFT_DIRR FFTW_FORWARD
#else
#define FFT_DIRF 1
#define FFT_DIRR 1
#endif

FFT3D< FFTOneD > forward( GlobalNx / PX, GlobalNy / PY, GlobalNz / PZ,
                          PX, PY, PZ, FFTOneD::FORWARD );

FFT3D< FFTOneD > reverse( GlobalNx / PX, GlobalNy / PY, GlobalNz / PZ,
                          PX, PY, PZ, FFTOneD::REVERSE );

int compare( complex*** in, 
             complex*** out,
             int localNx, 
             int localNy, 
             int localNz )
{
  double tolerance = 1. / 1000000000.;
  
  for(int i=0; i < localNx; i++)
    for(int j=0; j < localNy; j++)
      for(int k=0; k < localNz; k++) 
        {
          if( fabs( in[i][j][k].re - out[i][j][k].re ) > tolerance ) 
            {
              cout << "in [" << i << "][" << j << "][" << k << "]=";
              cout << in[ i ][ j ][ k ] << endl;
              
              cout << "out[" << i << "][" << j << "][" << k << "]=";
              cout << out[ i ][ j ][ k ] << endl;
              
              return 1;
            }

          if( fabs( in[i][j][k].im - out[i][j][k].im ) > tolerance ) 
            {
              cout << "in ["<<i<<"]["<< j<<"]["<<k<<"]=";
              //cout<< in[i][j][k]<<endl;
              cout << "out["<<i<<"]["<< j<<"]["<<k<<"]=";
              //cout<<out[i][j][k]<<endl;
              return 1;
            }
        }
  
  return 0;
}


#ifdef BUILD_FOR_BLNIE
int _CP_main( int argc, char *argv[], char **envp )
#else
int main( int argc, char*argv[] ) 
#endif
{ 
  cout << "main " << endl;
  complex ***in, ***tmp, ***out;

#ifndef BUILD_FOR_BLNIE    
  MPI_Init(&argc, &argv);
  BladeInit(&argc, &argv);
  //  MPI_Comm comm;
#endif

  const int ndims = 3; // num of dims of cartesian grid
  int meshSz;

  BegLogLine( PKFXLOG_DEBUG )
    << "main "
    << EndLogLine;

#ifndef BUILD_FOR_BLNIE  
  AttachDebugger( argv[ 0 ] );
#endif

  //  MPI_Comm_size(MPI_COMM_WORLD, &meshSz);
  int maxX, maxY, maxZ;
  BGLPartitionGetDimensions( &maxX, &maxY, &maxZ );
  meshSz = maxX * maxY * maxZ;

  if( meshSz != PX * PY * PZ ) 
    {
      printf("Error: MPI_MESS %d static_MESS %d \n", meshSz, PX * PY * PZ );
      /// MPI_Finalize(); 
      /// exit(0);
      return 0;
    }
  
  int pX = PX, pY = PY, pZ = PZ, procs;
  int globalNx=GlobalNx, globalNy=GlobalNy, globalNz=GlobalNz;
  
  int localNx = (int) (ceil( (double) globalNx / (double) pX ));
  int localNy = (int) (ceil( (double) globalNy / (double) pY ));
  int localNz = (int) (ceil( (double) globalNz / (double) pZ ));
  
  int myRank;  
  // MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  myRank = BGLPartitionGetRankXYZ();
  
  cout<< "main "<<endl;
  
  if (myRank == 0) 
    {
      cout<<" 3D FFT of size  " << globalNx << " , "
          << globalNy << " , "  << globalNz << endl;
      cout<<" local FFT sizes " << localNx  << " , "
          << localNy << " , "  << localNz  << endl;
      cout<<" Processor Mesh  " << pX << " , "
          << pY << " , " << pZ  << endl;
    }
  
  
  in  = (complex***) new complex[ localNx * 4 ];
  tmp = (complex***) new complex[ localNx * 4 ];
  out = (complex***) new complex[ localNx * 4 ];
  
  for(unsigned int i=0; i<localNx; i++) 
    {
      in[ i ]  = (complex **) new complex[ localNy * 4 ];
      tmp[ i ] = (complex **) new complex[ localNy * 4 ];
      out[ i ] = (complex **) new complex[ localNy * 4 ];
      
      for(unsigned int j=0; j<localNy; j++) 
        {
          in[ i ][ j ]  = (complex *) new complex[ localNz * 4 ];
          tmp[ i ][ j ] = (complex *) new complex[ localNz * 4 ];
          out[ i ][ j ] = (complex *) new complex[ localNz * 4 ];
        }
    }
  
  int ii=0;
  int MyX, MyY, MyZ;
  BGLPartitionGetCoords(&MyX, &MyY, &MyZ);

//   int MyX = ( myRank / pZ ) / pX;
//   int MyY = ( myRank / pZ ) % pX;
//   int MyZ = ( myRank % pZ);
  
#ifdef REGRESS_FFT3DW
  
  for(int x=0; x<GlobalNx; x++)
    for(int y=0; y<GlobalNy; y++)
      for(int z=0; z<GlobalNz; z++) 
        {
          double data = drand48(); //(x*GlobalNy+y)*GlobalNz+z;
          src[ x ][ y ][ z ] = complex( data, data+1 );
          
          //           BegLogLine(1)<< "main::input " 
          //                        << src[x][y][z].re 
          //                        << src[x][y][z].im
          //                        << EndLogLine;
        }
  
  for(unsigned int i=0; i < localNx; i++)
    for(unsigned int j=0; j < localNy; j++)
      for(unsigned int k=0; k < localNz; k++)
        {
          int a = MyX * localNx + i;
          int b = MyY * localNy + j;
          int c = MyZ * localNz + k;
          
          in[ i ][ j ][ k ].re = src[ a ][ b ][ c ].re;
          in[ i ][ j ][ k ].im = src[ a ][ b ][ c ].im;
          
          BegLogLine( PKFXLOG_DEBUG ) << "main::input[" << i << "][" << j << "][" << k << "]["
                                      << MyX << " ," << MyY << "," << MyZ << "]= "
                                      << in[ i ][ j ][ k ].re << " " << in[ i ][ j ][ k ].im 
                                      << EndLogLine;
        }
#else
  for(unsigned int i=0; i<localNx; i++)
    for(unsigned int j=0; j<localNy; j++)
      for(unsigned int k=0; k<localNz; k++)
        {
          double x = drand48(); //(double)((i*localNy+j)*localNz+k);
          
          in[ i ][ j ][ k ].re = x;
          in[ i ][ j ][ k ].im = x+1;
          
          BegLogLine( PKFXLOG_DEBUG )<< "msin::input["<<i<<"]["<< j<<"]["<<k<<"]["
                                     << MyX << " ," << MyY <<"," << MyZ << "]= "
                                     << in[i][j][k].re <<" "<< in[i][j][k].im 
                                     << EndLogLine;
        }
#endif
  
  forward.Init();
  forward.Execute(in, tmp);
  // MPI_Barrier(MPI_COMM_WORLD);
  BGLPartitionBarrier();

  
#ifdef REGRESS_FFT3DW

  printf("Regressing against FFTW\n");
  fftwnd_plan forward3DPlan =  fftw3d_create_plan(GlobalNx,GlobalNy, GlobalNz,
                                                  FFTW_FORWARD, FFTW_ESTIMATE );

  fftwnd_one( forward3DPlan, (fftw_complex*) &src[0][0][0], (fftw_complex*) &dst1[0][0][0]);
  
  int error=0;
  double tolerance = 1./1000000.;

  for(int i=0; i<localNx; i++)
    for(int j=0; j<localNy; j++)
      for(int k=0; k<localNz; k++) 
        {
          int a = MyX*localNx +i;
          int b = MyY*localNy + j;
          int c = MyZ*localNz + k;

          if( fabs( tmp[ i ][ j ][ k ].re - dst1[ a ][ b ][ c ].re ) > tolerance
             || fabs( tmp[ i ][ j ][ k ].im - dst1[ a ][ b ][ c ].im ) > tolerance )
            {
              if( fabs( src[ a ][ b ][ c ].re - in[ i ][ j ][ k ].re ) > tolerance
                 || fabs( src[ a ][ b ][ c ].im - in[ i ][ j ][ k ].im ) > tolerance )
                {
                  cout << " Check the fft data input "<< endl;
                  cout << "in["<<i<<"]["<< j<<"]["<<k<<"]="
                       << in[i][j][k]<<endl;
                  cout << "src["<<a<<"]["<< b<<"]["<<c<<"]="
                       << dst1[a][b][c]<<endl;
                  // MPI_Finalize();
                  // exit(0);
                  return( 0 );
                }

              cout << "coord " << MyX << "," << MyY << "," << MyZ << endl;
              cout << "tmp [" << i << "][" << j << "][" << k << "]=" << tmp[ i ][ j ][ k ] << endl;
              cout << "dst[" << a << "][" << b << "][" << c << "]=" << dst1[ a ][ b ][ c ] << endl;
              error=1;
            }
        }
  
  
  if(error==1) 
    {
      cout<< "FAILED 3D FFT TEST :-("<<endl;
    }
  else 
    {
      cout<< "PASSED 3D FFT TEST :-)" <<endl;
    }
  
  
  // for(unsigned int i=0; i<localNx; i++)
  //     for(unsigned int j=0; j<localNy; j++)
  //       for(unsigned int k=0; k<localNz; k++){
  //      cout << "in["<<i<<"]["<< j<<"]["<<k<<"]=";
  //      cout << in[i][j][k] <<endl;
  //      cout << "tmp["<<i<<"]["<< j<<"]["<<k<<"]=";
  //      cout << tmp[i][j][k]
  //           <<endl<<endl;
  //       }
  
  
  
#endif
  
  
  //#define Deb 0
#ifdef Deb
  
  //  MPI_Barrier(MPI_COMM_WORLD);
  BGLPartitionBarrier();
  reverse.Init();
  reverse.Execute(tmp, out);
  
  double fftSz = 1./(double)(localNx*pX*localNy*pY*localNz*pZ);
  
  for(unsigned int i=0; i<localNx; i++)
    for(unsigned int j=0; j<localNy; j++)
      for(unsigned int k=0; k<localNz; k++) 
        {
          out[i][j][k].re = fftSz*out[i][j][k].re;
          out[i][j][k].im = fftSz*out[i][j][k].im;
        }
  
  for(unsigned int i=0; i<localNx; i++)
    for(unsigned int j=0; j<localNy; j++)
      for(unsigned int k=0; k<localNz; k++)
        {
          cout << "out[" << i << "][" << j << "][" << k << "]=";
          //cout << out[i][j][k] <<endl;
          cout << "tmp[" << i << "][" << j << "][" << k << "]="
            //cout << tmp[i][j][k]
               << endl << endl;
        }
  

  if( compare(in, out, localNx, localNy, localNz) == 1 ) 
    {
      cout << "FAILED FFT-REVERSE TEST :-(" << endl; 
    }
  else 
    {
      cout << "PASSED FFT-REVERSE TEST :-)" << endl;
    }
#endif

  ///  BGLPartitionBarrier();
  return ( 1 );
  //  MPI_Finalize();
}

#ifdef BUILD_FOR_BLNIE
int _IOP_main( int argc, char *argv[], char **envp )
{
   int NodesX;
   int NodesY;
   int NodesZ;
   int MyX;
   int MyY;
   int MyZ;

   BGLPartitionGetCoords( &MyX, &MyY, &MyZ );

   BGLPartitionGetDimensions( &NodesX, &NodesY, &NodesZ );

   // each node sends 1 pkt to every other node including self.
   int count = ( NodesX * NodesY * NodesZ );

   printf( "_IOP_main[%3d,%3d,%3d]: Waiting for %d packets.\n", MyX, MyY, MyZ, count );

   int rc = BGLTorusWaitCount( &count );  // wait on both fifo groups

   printf( "_IOP_main[%3d,%3d,%3d]: All Packets Received, exiting.\n", MyX, MyY, MyZ );

   return(rc);
}
#endif
