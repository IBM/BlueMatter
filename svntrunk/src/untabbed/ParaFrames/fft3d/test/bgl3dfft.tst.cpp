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
 /*#####################################################################
#                                                                   #
#      (c) COPYRIGHT IBM CORP. 2006 ALL RIGHTS RESERVED.             #
#                                                                   #
#                LICENSED MATERIALS-PROPERTY OF IBM.                #
#                                                                   #
#####################################################################
*/


#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <complex.hpp>
#include <fft3D.hpp>
#include <math.h>


#include "mpi.h"
#define TYPE double 

#define FFT_FORWARD  -1
#define FFT_REVERSE 1
#define N_SIZE 128 

#define MESS_SIZE 16

struct FFT_PLAN
{
  enum
  {
    P_X = MESS_SIZE,
    P_Y = MESS_SIZE,
    P_Z =(MESS_SIZE)
  };
  enum
  {
    GLOBAL_SIZE_X = N_SIZE,
    GLOBAL_SIZE_Y = N_SIZE,
    GLOBAL_SIZE_Z = N_SIZE
  };
};

FFT3D < FFT_PLAN, FFT_FORWARD, TYPE> *fftForward; 
FFT3D < FFT_PLAN, FFT_REVERSE, TYPE> *fftReverse; 

// get processor mesh and fft size
void init( int argc, char*argv[],  int& globalNx, int& globalNy, int& globalNz,
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
      
    }
}


void finalize()
{
  //MPI_Finalize();
}


// do fft
void Fwd3DFFT( int globalNx, int globalNy, int globalNz,int pX, int pY, int pZ)
{
  int myRank=-1;
  myRank=Platform::Topology::GetAddressSpaceId();
  
  int arraySz= (globalNx*globalNy*globalNz)/(pX*pY*pZ);

  complexTemplate<TYPE>* in  = new complexTemplate<TYPE>[arraySz];
  complexTemplate<TYPE>* out = new complexTemplate<TYPE>[arraySz];
  
  int localNx=globalNx/pX;
  int localNy=globalNy/pY;
  int localNz=globalNz/pZ;

  fftForward = new FFT3D< FFT_PLAN, FFT_FORWARD,TYPE>();  
  (*fftForward).Init(globalNx, globalNy, globalNz, pX, pY, pZ);
  int incr = 0;
  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
  { 
    double rdata = drand48();
    double idata = drand48();	  
    int myIndex  = i*localNy*localNz+j*localNz+k;
    in[myIndex].re = (TYPE) rdata;
    in[myIndex].im = (TYPE) idata;
  }
  
  (*fftForward).DoFFT(in,out);
}


extern "C" 
{
  void* PkMain(int argc, char**argv, char** envp); 
}


void* PkMain(int argc, char**argv, char** envp) 
{
  //MPI_Init(&argc, &argv);

  int pX=0; int pY=0;int pZ=0;
  int procs;
  int globalNx, globalNy, globalNz;
  int myRank=-1;
  myRank=Platform::Topology::GetAddressSpaceId();
  
  init(argc, argv, globalNx,globalNy,globalNz, pX, pY, pZ);	
  Fwd3DFFT(globalNx, globalNy, globalNz, pX, pY, pZ);
}
          


