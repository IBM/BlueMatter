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

#include <assert.h>
#include "fft3D.hpp"
#include <iostream.h>
#include <math.h>
#include "timer.hpp"

#define GLOBAL_NX 4
#define GLOBAL_NY 4
#define GLOBAL_NZ 4

#define  REGRESS_FFT3DW  1
//#define  REGRESS_REVERSE 1

#ifdef REGRESS_FFT3DW
complex src[GLOBAL_NX][GLOBAL_NY][GLOBAL_NZ];
complex dst1[GLOBAL_NX][GLOBAL_NY][GLOBAL_NZ];
#endif  

int compare ( complex*** in, complex*** out, 
	      int localNx, int localNy, int localNz){
  double tolerance = 1./1000000000.;
  for(int i=0; i<localNx; i++)
    for(int j=0; j<localNy; j++) 
      for(int k=0; k<localNz; k++) {
	if(fabs(in[i][j][k].re-out[i][j][k].re)>tolerance) { 
	  cout << "in ["<<i<<"]["<< j<<"]["<<k<<"]="<< in[i][j][k]<<endl;
	  cout << "out["<<i<<"]["<< j<<"]["<<k<<"]="<< out[i][j][k]<<endl;
	  return 1;
	}
       	if(fabs(in[i][j][k].im-out[i][j][k].im)>tolerance) {
	  cout << "in ["<<i<<"]["<< j<<"]["<<k<<"]="<< in[i][j][k]<<endl;
	  cout << "out["<<i<<"]["<< j<<"]["<<k<<"]="<< out[i][j][k]<<endl;
	  	  return 1;
	  }
      }
  
  return 0;
}


main(int argc, char*argv[]) {

  int loop = 1;
  //while (loop);

  unsigned int localNx = 1, localNy = 1, localNz=1;
  complex ***in, ***tmp, ***out;
  
  
  MPI_Init(&argc, &argv);
  MPI_Comm comm;
  const int ndims = 3; // num of dims of cartesian grid
  int meshSz;
  MPI_Comm_size(MPI_COMM_WORLD, &meshSz);
  
  int pX, pY, pZ, procs;
  int globalNx, globalNy, globalNz;
  int nsize; //global x or y or z dimension
  
  
  for(int i=0; i<argc; i++){
    
    if(!strcmp(argv[i],"-fftsize")) {
      int j = 0;
      while ((i+j+1) < argc) {
	if (argv[i+j+1][0], '-') break;
        j++;
      }
      
      globalNx = atoi(argv[++i]);
      globalNy = atoi(argv[++i]);
      globalNz = atoi(argv[++i]);
#ifdef REGRESS_FFT3DW
      if(globalNx!=GLOBAL_NX ||
	 globalNy!=GLOBAL_NY ||
	 globalNz!=GLOBAL_NZ ) {
	cout << "ERROR : Wrong number of FFT size " <<endl;  
	exit(0);
      }
#endif
    } else if (!strcmp(argv[i],"-procmesh")) {
      int j = 0;
      while ((i+j+1) < argc) {
	if (argv[i+j+1][0], '-') break;
        j++;
      }
      
      pX = atoi(argv[++i]);
      pY = atoi(argv[++i]);
      pZ = atoi(argv[++i]);
    }
 
  }

  if(pX ==0 || pY==0 || pZ==0){
    cout << "ERROR : Wrong number of proccessor mesh " <<endl;  
    cout << pX <<pY<<pZ<<endl;
    exit(0);
}
  if(globalNx == 0 || globalNy ==0 || globalNz == 0) 
    cout << "ERROR : Wrong size of the 3D FFT " <<endl;
  
  localNx = ceil((double)globalNx/(double)pX);  
  localNy = ceil((double)globalNy/(double)pY);
  localNz = ceil((double)globalNz/(double)pZ);
  
  int myRank;
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
  
  if (myRank == 0) {
    cout<<" 3D FFT of size  " << globalNx << " , " 
	<< globalNy << " , "  << globalNz << endl;
    cout<<" local FFT sizes " << localNx  << " , "
	<< localNy << " , "  << localNz  << endl;
    cout<<" Processor Mesh  " << pX << " , " 
	<< pY << " , " << pZ  << endl;
  }
  
  int dimSz[ndims]={pX, pY, pZ};
  
  int periods[ndims]  = {0, 0, 0};  // logical array, specifying whether the
  // grid is periodic, in each dim. 
  
  int reorder = 0;                  //ranking is not reordered.
  
  MPI_Cart_create(MPI_COMM_WORLD, ndims, dimSz,
		  periods,reorder, &comm);
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
  
  int ii=0;
  int MyX = (myRank/pZ)/pX;
  int MyY = (myRank/pZ)%pX;
  int MyZ = (myRank%pZ);
  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++){  
        double data = ((i+localNx*MyX)*GLOBAL_NY  + j + localNy*MyY)
			     *GLOBAL_NY+ k+localNz*MyZ;
 	in[i][j][k]=complex(  data, data+1);
      }
 
#ifdef REGRESS_FFT3DW  
for(int x=0; x<GLOBAL_NX; x++)
  for(int y=0; y<GLOBAL_NY; y++)
    for(int z=0; z<GLOBAL_NZ; z++) {
      double data = drand48(); //(x*GLOBAL_NY+y)*GLOBAL_NZ+z;
	src[x][y][z] = complex(data,data+1 );
    }
			
 for(unsigned int i=0; i<localNx; i++) 
   for(unsigned int j=0; j<localNy; j++) 
     for(unsigned int k=0; k<localNz; k++){  
       int a = MyX*localNx +i;
       int b = MyY*localNy + j;
       int c = MyZ*localNz + k;
       in[i][j][k].re=src[a][b][c].re;
       in[i][j][k].im=src[a][b][c].im;
     } 
#else

 for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++){ 
        double data = drand48(); 
 	in[i][j][k]=complex(data,data+1);
      }
 
#endif
  int fftdir = -1;
  FFT3D<FFTOneD, Comm3DLayer> *forward = 
    new FFT3D<FFTOneD, Comm3DLayer>(localNx, localNy, localNz,
				    pX, pY, pZ, fftdir, comm);
  fftdir = 1;
  double now = etime();
  int niter = 1;
  for(int i=0; i<niter; i++)
    (*forward).fft(in, tmp);
  double elapsed = etime()-now;
   if(myRank == 0) 
   {
    printf("time for fft  iteration = %f seconds\n",elapsed/niter );
   }

#ifdef REGRESS_REVERSE  
  FFT3D<FFTOneD, Comm3DLayer> *reverse = 
    new FFT3D<FFTOneD, Comm3DLayer>(localNx, localNy, localNz,
				    pX, pY, pZ, fftdir, comm);
  MPI_Barrier(MPI_COMM_WORLD);

  (*reverse).fft(tmp, out);
#endif
  
  double fftSz = 1./(double)(localNx*pX*localNy*pY*localNz*pZ);
  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++) {
	out[i][j][k].re = fftSz*out[i][j][k].re;
	out[i][j][k].im = fftSz*out[i][j][k].im;
      }
  
#ifdef REGRESS_REVERSE
  if(compare(in, out, localNx, localNy, localNz) == 1) { 
    cout<< "FAILED FFT-REVERSE TEST :-("<<endl; }
  else {
    cout<< "PASSED FFT-REVERSE TEST :-)" <<endl;
  } 
  
#endif

#ifdef REGRESS_FFT3DW
  fftwnd_plan forward3DPlan = 
    fftw3d_create_plan(GLOBAL_NX, GLOBAL_NY, GLOBAL_NZ,
		       FFTW_FORWARD, FFTW_ESTIMATE );

 double nowfftw = etime();
 int niterfftw = 10;
 for(int i=0; i<niterfftw; i++) {
   fftwnd_one(forward3DPlan,(fftw_complex*) &src[0][0][0], 
	      (fftw_complex*)&dst1[0][0][0]);
 }
 double elapsedfftw = etime()-nowfftw;
 if(myRank == 0) 
   {
    printf("time for fft  iteration = %f seconds\n",elapsed/niter );
    printf("time for fftw iteration = %f seconds\n",elapsedfftw/niterfftw);
     
   }
 int error=0;
  double tolerance = 1./1000000.;
  for(int i=0; i<localNx; i++)
    for(int j=0; j<localNy; j++) 
      for(int k=0; k<localNz; k++) {
        int a = MyX*localNx +i;
	int b = MyY*localNy + j;
	int c = MyZ*localNz + k;
	if(fabs(tmp[i][j][k].re-dst1[a][b][c].re)>tolerance
	   ||fabs(tmp[i][j][k].im-dst1[a][b][c].im)>tolerance )
	  {
	    if(fabs(src[a][b][c].re-in[i][j][k].re)>tolerance
 	       ||fabs(src[a][b][c].im-in[i][j][k].im)>tolerance )
 	      {
 		cout << " Check the fft data input "<< endl;
		MPI_Finalize();
 		exit(0);
 	      }
	    cout << "tmp ["<<i<<"]["<< j<<"]["<<k<<"]="<< tmp[i][j][k]<<endl;
	    cout << "dst["<<i<<"]["<< j<<"]["<<k<<"]="<< dst1[a][b][c]<<endl;
	    error=1;
	  }
      }

  //  if(myRank == 0)
  if(error==1) {
    cout<< "FAILED 3D FFT TEST :-("<<endl;
  }
  else {      
    cout<< "PASSED 3D FFT TEST :-)" <<endl;
  }
#endif
  
  // #define DEBUG 1
#ifdef DEBUG
  for(int i=0; i<localNx; i++)
    for(int j=0; j<localNy; j++) 
      for(int k=0; k<localNz; k++) {	
	cout << "fft3D_IN   ["<<i<<"]["<< j<<"]["<<k<<"]["
	     << MyX << "," << MyY << "," << MyZ << "]=" 
	     << in[i][j][k]<<endl;

	cout << "fft3D_OUT  ["<<i<<"]["<< j<<"]["<<k<<"]["
	     << MyX << "," << MyY << "," << MyZ << "]=" 
	     << tmp[i][j][k]<<endl;
      }

  if(myRank==0)
    for(int a=0; a<GLOBAL_NX; a++)
      for(int b=0; b<GLOBAL_NY; b++) 
	for(int c=0; c<GLOBAL_NZ; c++) {
        
         cout << "fft3DW_IN  ["<<a<<"]["<< b<<"]["<<c<<"][" << MyX 
	       << "," << MyY << "," << MyZ << "]=" << src[a][b][c]<<endl;		
	  cout << "fft3DW_OUT  ["<<a<<"]["<< b<<"]["<<c<<"][" << MyX 
	       << "," << MyY << "," << MyZ << "]=" << dst1[a][b][c]<<endl;
	}

#endif
  MPI_Finalize();
}
