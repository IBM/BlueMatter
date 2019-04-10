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
//  Last Modified on: 09/24/03 by Maria Eleftheriou
//  fftmain.hpp
//  tests the 3D fft using MPI and fftw 

#include <assert.h>
#include <fft3d/fft3D_MPI.hpp>
#include <stdlib.h>
#include <string.h>
////#include <iostream.h>
#include <math.h>
#include <fft3d/timer_MPI.hpp>


#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))
#define TOLERANCE  (1./1000000000.)
#define N_SIZE 128
#define MESS_SIZE 4
struct FFT_PLAN
{
  enum
  {
    P_X = MESS_SIZE,
    P_Y = MESS_SIZE,
    P_Z = (2*MESS_SIZE)
  };
  enum
  {
    GLOBAL_SIZE_X = N_SIZE,
    GLOBAL_SIZE_Y = N_SIZE,
    GLOBAL_SIZE_Z = N_SIZE
  };
};

// CHECK needs to be removed
#define FORWARD -1
#define REVERSE 1

typedef  FFT3D< FFT_PLAN, FORWARD> FFT_FORWARD FFT3D_STUCT_ALIGNMENT;
typedef  FFT3D< FFT_PLAN, REVERSE > FFT_REVERSE FFT3D_STUCT_ALIGNMENT;

FFT_FORWARD fftForward;
#define REGRESS_REVERSE 1
#ifdef REGRESS_REVERSE
FFT_REVERSE fftReverse;
#endif


#define  REGRESS_REVERSE 1

int compare (complex*** in, complex*** out, 
       int localNx, int localNy, int localNz)
{
  for(int i=0; i<localNx; i++)
    for(int j=0; j<localNy; j++) 
      for(int k=0; k<localNz; k++) 
  {
    if(fabs(in[i][j][k].re-out[i][j][k].re)>TOLERANCE)
      { 
        cout << "in ["<<i<<"]["<< j<<"]["<<k<<"]="<< in[i][j][k]<<endl;
        cout << "out["<<i<<"]["<< j<<"]["<<k<<"]="<< out[i][j][k]<<endl;
        return 1;
      }
    if(fabs(in[i][j][k].im-out[i][j][k].im)>TOLERANCE) 
      {
        cout << "in ["<<i<<"]["<< j<<"]["<<k<<"]="<< in[i][j][k]<<endl;
        cout << "out["<<i<<"]["<< j<<"]["<<k<<"]="<< out[i][j][k]<<endl;
        return 1;
      }
  }
  return 0;
}


main(int argc, char*argv[])
{

  unsigned int localNx, localNy, localNz;
  complex ***in, ***tmp, ***out;
  complex ***localOut;
  MPI_Init(&argc, &argv);

  localNx = FFT_PLAN::GLOBAL_SIZE_X/FFT_PLAN::P_X;
  localNy = FFT_PLAN::GLOBAL_SIZE_X/FFT_PLAN::P_Y;
  localNz = FFT_PLAN::GLOBAL_SIZE_X/FFT_PLAN::P_Z;
  
  int myRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  if (myRank == 0) 
    {
      printf( " proc mesh GLOBAL_SIZE_X = %d GLOBAL_SIZE_Y = %d GLOBAL_SIZE_Z = %d\n",
        FFT_PLAN::GLOBAL_SIZE_X,
        FFT_PLAN::GLOBAL_SIZE_Y, 
        FFT_PLAN::GLOBAL_SIZE_Z);
      
      printf(" 3D FFT of size [%d %d %d] on [%d %d %d] \n",
       localNx, localNy, localNz,
       FFT_PLAN::P_X, FFT_PLAN::P_Y, FFT_PLAN::P_Z);
    }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
#define mainMax(i, j) ( (i>j)?i:j )
  
  int maxLocalN = mainMax(mainMax(localNx, localNy), localNz);
  in  = (complex***) new complex[maxLocalN];
  tmp = (complex***) new complex[maxLocalN];
  out = (complex***) new complex[maxLocalN];
  
  for(unsigned int i=0; i<localNx; i++) {
    in[i]  = (complex **) new complex[maxLocalN];
    tmp[i] = (complex **) new complex[maxLocalN];
    out[i] = (complex **) new complex[maxLocalN];
  
    for(unsigned int j=0; j<localNy; j++) {
      in[i][j]  = (complex *) new complex[maxLocalN];
      tmp[i][j] = (complex *) new complex[maxLocalN];
      out[i][j] = (complex *) new complex[maxLocalN];
    }
  }

  (fftForward).Init();   
  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
  { 
    double data = drand48(); 
    in[i][j][k]=complex(data,0);
    fftForward.PutRealSpaceElement(i, j, k, in[i][j][k].re);
  }
  
  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
  { 
    tmp[i][j][k].re= fftForward.GetRealSpaceElement(i, j, k);
    
    if(in[i][j][k].re-tmp[i][j][k].re>TOLERANCE)
      {
        printf(" Check input data. Exiting...\n");
        MPI_Finalize(); exit(1);
      }
  }

  int iter =20;
  for(int i=0; i<iter; i++)
    (fftForward).DoFFT();
  
 // if(myRank==0)
//   for(unsigned int i=0; i<localNx; i++) 
//     for(unsigned int j=0; j<localNy; j++) 
//       for(unsigned int k=0; k<localNz; k++)
// 	{ 
//     	  printf(" proc = %d out_forward[%d %d %d] = %f\n", 
// 		 myRank, i, j, k, tmp[i][j][k].re, tmp[i][j][k].im );
// 	}
#ifdef REGRESS_REVERSE  
  
  (fftReverse).Init();

  for( int i=0; i<localNx; i++)
    for( int j=0; j<localNy; j++)
      for( int k=0; k<localNz; k++)
  {
    fftReverse.PutRecipSpaceElement(i, j, k,
         fftForward.GetRecipSpaceElement( i, j, k ) );
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  (fftReverse).DoFFT();

  double fftSz = 1./(double)(localNx*FFT_PLAN::P_X*
           localNy*FFT_PLAN::P_Y*
           localNz*FFT_PLAN::P_Z);

  for( int i=0; i<localNx; i++)
    for( int j=0; j<localNy; j++)
      for( int k=0; k<localNz; k++)
  {
    out[i][j][k] = (fftReverse.GetRealSpaceElement(i, j, k ));
    out[i][j][k].re = fftSz*out[i][j][k].re;
    out[i][j][k].im = fftSz*out[i][j][k].im;
    // cout<<"OUT["<<i<<"]["<<j<<"][" <<k<< "] =" << out[i][j][k] <<endl;
  }

  if(compare(in, out, localNx, localNy, localNz) == 1) { 
   printf( "FAILED FFT-REVERSE TEST :-( \n" ); }
  else {
   printf( "PASSED FFT-REVERSE TEST :-) \n" );
  } 
#endif
//#define  PRINT_OUTPUT
#ifdef PRINT_OUTPUT
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
  {          
    tmp[i][j][k] =   fftForward.GetRecipSpaceElement( i, j, k );
    printf(" proc = %d out_forward[%d %d %d] = %f\n", 
     myRank, i, j, k, tmp[i][j][k].re, tmp[i][j][k].im );
    printf(" proc %d out_reverse[%d %d %d] = %f\n", 
     myRank, i, j, k, out[i][j][k].re, tmp[i][j][k].im );
    
      }
#endif
  MPI_Finalize();
}
