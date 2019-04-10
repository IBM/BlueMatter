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


#include <assert.h>
#include <MPI/fft3D_MPI.hpp>
#include <iostream.h>
#include <math.h>

#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))
#define TOLERANCE  (1./1000000000.)

#define N_SIZE 64 
#define MESS_SIZE  1

#define GLOBAL_NX (N_SIZE)
#define GLOBAL_NY (N_SIZE)
#define GLOBAL_NZ  N_SIZE

struct FFT_PLAN
{
  enum
  {
    P_X = MESS_SIZE,
    P_Y = MESS_SIZE,
    P_Z = (1*MESS_SIZE)
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
#define REGRESS_REVERSE 1 
#ifdef REGRESS_REVERSE
typedef  FFT3D <FFT_PLAN, REVERSE> FFT_REVERSE FFT3D_STUCT_ALIGNMENT;
FFT_REVERSE *fftReverse;
#endif

typedef  FFT3D <FFT_PLAN, FORWARD> FFT_FORWARD  FFT3D_STUCT_ALIGNMENT;

FFT_FORWARD *fftForward;

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
  int localNx , localNy , localNz;

  complex in[GLOBAL_NX][GLOBAL_NY][GLOBAL_NZ];
  complex out[GLOBAL_NX][GLOBAL_NY][GLOBAL_NZ];
  complex tmp[GLOBAL_NX][GLOBAL_NY][GLOBAL_NZ];

 
  int niter =10;
  fftForward = new FFT3D<FFT_PLAN, FORWARD>();

#ifdef REGRESS_REVERSE  
  fftReverse = new FFT3D<FFT_PLAN, REVERSE>();
#endif

  MPI_Init(&argc, &argv);  
  int pX, pY, pZ, procs;
  int globalNx, globalNy, globalNz;
  
  for(int i=0; i<argc; i++)
    {
      if (!strcmp(argv[i],"-procmesh")) {
    int j = 0;
    while ((i+j+1) < argc)
      {
        if (argv[i+j+1][0], '-') break;
              j++;
      }
    
    pX = atoi(argv[++i]);
    pY = atoi(argv[++i]);
    pZ = atoi(argv[++i]);

  }
    }
  
  localNx = GLOBAL_NX/pX;  
  localNy = GLOBAL_NY/pY;
  localNz = GLOBAL_NZ/pZ;
 
  int myRank=-1;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
   
 #define mainMax(i, j) ( (i>j)?i:j )
  
  int maxLocalN = mainMax(mainMax(localNx, localNy), localNz);
  
  MPI_Barrier(MPI_COMM_WORLD);
  (*fftForward).Init(localNx, localNy, localNz, pX, pY, pZ);
  
  MPI_Barrier(MPI_COMM_WORLD);
 
  int incr = 0;
  int sX  = 0;
  int sY  = 0;
  int sZ  = 0;
  int srX = 0;
  int srY = 0;
  int srZ = 0;
 
  int rlocalNx, rlocalNy, rlocalNz;

  (*fftForward).GetLocalDimensions(sX,  localNx,
           sY,  localNy,
           sZ,  localNz,
           srX, rlocalNx,
           srY, rlocalNy,
           srZ, rlocalNz);
 

  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++){ 
        double data = drand48(); 
        in[i][j][k]=complex(data,0);
  ((*fftForward).PutRealSpaceElement(i,j,k, data));
      }
 
 
  int iter =1;
  MPI_Barrier(MPI_COMM_WORLD);
  double now = etime();
  for(int i=0; i<iter; i++)
    {
      (*fftForward).DoFFT();
    }

  
  double elapsed = etime()-now;
  if(myRank==0)
  printf(" elapsed time for %dx%dx%d FFT from %d iters = %lf\n", 
   GLOBAL_NX,GLOBAL_NY,GLOBAL_NZ, iter, elapsed/double(iter));	


  for(unsigned int i=0; i<rlocalNx; i++) 
    for(unsigned int j=0; j<rlocalNy; j++) 
      for(unsigned int k=0; k<rlocalNz; k++){ 
  assert(rlocalNx*rlocalNy*rlocalNz);
  out[i][j][k] = ((*fftForward).GetRecipSpaceElement(i,j,k));
      }
 
 MPI_Barrier(MPI_COMM_WORLD);


#ifdef REGRESS_REVERSE  
     
  (*fftReverse).Init(localNx, localNy, localNz, pX, pY, pZ);
   
  for( int i=0; i<rlocalNx; i++)
    for( int j=0; j<rlocalNy; j++)
      for( int k=0; k<rlocalNz; k++)
  {
    complex x = (*fftForward).GetRecipSpaceElement( i, j, k ) ;
    (*fftReverse).PutRecipSpaceElement(i, j, k,x);	
  }
 
   MPI_Barrier(MPI_COMM_WORLD);

  (*fftReverse).DoFFT();
  
  double fftSz = 1./(double)(localNx*pX*
           localNy*pY*
           localNz*pZ);
  int error=0;
  double tolerance = 1./1000000.;

  for( int i=0; i<localNx; i++)
    for( int j=0; j<localNy; j++)
      for( int k=0; k<localNz; k++)
  {
    tmp[i][j][k] = ((*fftReverse).GetRealSpaceElement(i, j, k ));
    tmp[i][j][k].re = fftSz*tmp[i][j][k].re;
    tmp[i][j][k].im = fftSz*tmp[i][j][k].im;
          if(fabs(in[i][j][k].re-tmp[i][j][k].re)>tolerance
         ||fabs(in[i][j][k].im-tmp[i][j][k].im)>tolerance )
          {
       error =1;
          }
        }	

  if(error==1) 
    { 
      printf( "FAILED FFT-REVERSE TEST :-( \n" );
    }
  else 
    {
      printf( "PASSED FFT-REVERSE TEST :-) \n" );
    }
 
  MPI_Barrier(MPI_COMM_WORLD);
#endif

  MPI_Finalize();
}
