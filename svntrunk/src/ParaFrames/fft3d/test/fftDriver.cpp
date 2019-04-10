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
#include <complex_MPI.hpp>
#include <bgl3dfft.hpp>
#include <iostream.h>
#include <math.h>
#include "mpi.h"
#define TOLERANCE  (1./1000000000000.)

#define ITER 1;   // number of iterations for the fft benchmark
#define TYPE double 

#define FORWARD -1
#define REVERSE 1

BGL3DFFT < REVERSE, TYPE> *fftReverse; 
BGL3DFFT <FORWARD, TYPE> *fftForward; 

void init( int argc, char*argv[], int &choice, int& globalNx, int& globalNy, int& globalNz,
	  int& pX, int& pY, int& pZ)
{
  for(int i=0; i<argc; i++)
    {
      if (!strcmp(argv[i],"-procmesh"))
	{
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

      if (!strcmp(argv[i],"-fftsize")) 
	{
	  int j = 0;

	  while ((i+j+1) < argc)
	    {
	      if (argv[i+j+1][0], '-') break;
	      j++;
	    }
  
	  globalNx = atoi(argv[++i]);
	  globalNy = atoi(argv[++i]);
	  globalNz = atoi(argv[++i]);
	}
      
      if (!strcmp(argv[i],"-option")) 
	{
	  int j=0;
	  while ((i+j+1) < argc)
	    {
	      if (argv[i+j+1][0], '-') break;
	      j++;
	    }
	  choice= atoi(argv[++i]);
	}
    }
}


void finalize()
{
  MPI_Finalize();
}

void printInfo(int globalNx, int globalNy, int globalNz, int pX, int pY, int pZ) 
{
  int myRank=-1;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
  int localNx = globalNx/pX;  
  int localNy = globalNy/pY;
  int localNz = globalNz/pZ;
  
  if (myRank == 0) 
    {
      cout<<" 3D FFT of size  " << globalNx << " , " 
	  << globalNy << " , "  << globalNz << endl;
      cout<<" local FFT sizes " << localNx  << " , "
	  <<  localNy << " , "  << localNz  << endl;
      cout<<" Processor Mesh  " << pX << " , " 
	  << pY << " , " << pZ  << endl;
    }
}

void DoFwd3DFFTBenchmark( int globalNx, int globalNy, int globalNz,
		int pX, int pY, int pZ)
{
  int myRank=-1;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
  int arraySz= (globalNx*globalNy*globalNz)/(pX*pY*pZ);
  complexType<TYPE>* in  = new complexType<TYPE>[arraySz];
  complexType<TYPE>* out = new complexType<TYPE>[arraySz];
  
  int localNx=0;
  int localNy=0;
  int localNz=0;

  int sX, sY, sZ;   
  int rlocalNx, rlocalNy, rlocalNz;
  int srX, srY, srZ;

  fftForward = new BGL3DFFT< FORWARD,TYPE>(); 
  (*fftForward).Init(globalNx, globalNy, globalNz, pX, pY, pZ);
  int incr = 0;
  
  (*fftForward).GetLocalDimensions(sX,  localNx,
				   sY,  localNy,
				   sZ,  localNz,
				   srX, rlocalNx,
				   srY, rlocalNy,
				   srZ, rlocalNz);
  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
	{ 
	  double rdata = drand48();
	  double idata = drand48();	  
	  int myIndex  = i*localNy*localNz+j*localNz+k;
	  in[myIndex]._re = (TYPE) rdata;
	  in[myIndex]._im = (TYPE) idata;
	}
 
  MPI_Barrier(MPI_COMM_WORLD); 
  int iter=100;
  
  double now = MPI_Wtime();     
  for(int i=0; i<iter; i++)
    (*fftForward).DoFFT(in,out);

  MPI_Barrier(MPI_COMM_WORLD);  
  
  double elapsed = MPI_Wtime()-now; 
  int size=0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  if(myRank==0)
    printf(" elapsed time on  %d for %dx%dx%d FFT from %d iters = %lf\n", 
	   size, globalNx, globalNy, globalNz, iter, elapsed/double(iter));    
}

void DoFwdRevTest
( int globalNx, int globalNy, int globalNz,
		int pX, int pY, int pZ)
{
  int myRank=-1;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
  int arraySz= (globalNx*globalNy*globalNz)/(pX*pY*pZ);
  complexType<TYPE>* in  = new complexType<TYPE>[arraySz];
  complexType<TYPE>* out = new complexType<TYPE>[arraySz];
  complexType<TYPE>* revout = new complexType<TYPE>[arraySz];

 
  int localNx=0;
  int localNy=0;
  int localNz=0;

  int sX, sY, sZ;   
  int rlocalNx, rlocalNy, rlocalNz;
  int srX, srY, srZ;

  fftForward = new BGL3DFFT< FORWARD,TYPE>(); 
  (*fftForward).Init(globalNx, globalNy, globalNz, pX, pY, pZ);
  int incr = 0;
  
  (*fftForward).GetLocalDimensions(sX,  localNx,
				   sY,  localNy,
				   sZ,  localNz,
				   srX, rlocalNx,
				   srY, rlocalNy,
				   srZ, rlocalNz);
  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
	{ 
	  double rdata = drand48();
	  double idata = drand48();	  
	  int myIndex  = i*localNy*localNz+j*localNz+k;
	  in[myIndex]._re = (TYPE) rdata;
	  in[myIndex]._im = (TYPE) idata;
	}
 
  (*fftForward).DoFFT(in,out);
  MPI_Barrier(MPI_COMM_WORLD);
  
  fftReverse = new BGL3DFFT< REVERSE,TYPE>(); 
  (*fftReverse).Init(globalNx, globalNy, globalNz, pX, pY, pZ);
  (*fftReverse).DoFFT(out, revout);
   
 int error=0;
 for(int i=0; i<localNx*localNy*localNz; i++)
   if((in[i]._re!=revout[i]._re)||(in[i]._im!=revout[i]._im))
     {
       int error=1;
     }
 
if(error==1) cout<<"FFT_FORWARD_REVERSE_TEST FAILED"  <<endl;
 else cout<< "FFT_FORWARD_REVERSE_TEST PASSED"  <<endl;
}

int main(int argc, char*argv[]) 
{
  MPI_Init(&argc, &argv);
  int pX=0; int pY=0;int pZ=0;
  int procs;
  int globalNx, globalNy, globalNz;
  int myRank=-1;

  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
   if(myRank==0)
     cout << endl
	  << " Welcome to the bgl3dfft benchmark and test driver "
	  << " Options: "<< " " <<endl<<endl
	  << "\t(1) Benchmark forward complex to complex FFT." <<endl
	  << "\t(2) Benchmark reverse complex to complex FFT." <<endl
	  << "\t(3) forward-reverse FFT test"<<endl<<endl;
 
   int option; 
   
   init(argc, argv, option, globalNx,globalNy,globalNz, pX, pY, pZ);	

   if(myRank==0) 
     {
       printInfo(globalNx, globalNy, globalNz, pX, pY,  pZ);
       cout << option <<endl;
     }
   switch(option)
     {
     case 1 :  
       if(myRank==0) printInfo(globalNx, globalNy, globalNz, pX, pY,  pZ);
       DoFwd3DFFTBenchmark(globalNx, globalNy, globalNz, pX, pY, pZ);
       break;
     case 2:
       if(myRank==0) printInfo(globalNx, globalNy, globalNz, pX, pY,  pZ);
       //	  DoRev3DFFTBenchmark(globalNx, globalNy, globalNz, pX, pY, pZ);
       break;
     case 3 :
       if(myRank==0) printInfo(globalNx, globalNy, globalNz, pX, pY,  pZ);
       DoFwdRevTest(globalNx, globalNy, globalNz, pX, pY, pZ);
       break;
     default:
       if(myRank==0) cout << "Please make another selection or type 0 for stopping the testing " << endl;
     }
}
          


