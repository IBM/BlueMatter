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
//  FFT.hpp
//
//  The FFT Class provides an interface for performing a sequencial 3D FFT 
//  on distribute data on a 3D-processor mesh. It is templated on the type
//  of 1D FFT to perform and on parallel communication library to use. 
//  This is the MPI implementation.
//

#ifndef __FFT3D_H_
#define __FFT3D_H_

#include <iostream>
#include "fft_one_dim.hpp"
#include "comm3DLayer.hpp"
#include "mpi.h"
#include "../fftw/complex.hpp"

template<class FFT_T, class COMM_T>
class FFT3D {
  enum{DIM=3};
private:
  FFT_T *_fftx;
  FFT_T *_ffty;
  FFT_T *_fftz;

  COMM_T *_comm;

   int _fftDir;

  complex ***_localIn;  // localIn  [nBars][nFFTs][localSizeOfFFT];
  complex ***_localOut; // localOut [nBars][nFFTs][localSizeOfFFT];
  complex **_globalIn;  // globalIn [nFFTs][sizeof(1DFFT)];
  complex **_globalOut; // globalOut[nFFTs][sizeof(1DFFT)];

  unsigned int _localNx, _localNy, _localNz;
  unsigned int _pX, _pY, _pZ;
  
  unsigned int _barSzX, _barSzY, _barSzZ;
  int _wX[DIM];
  int _wY[DIM];
  int _wZ[DIM];
  
public:
  FFT3D(unsigned int localNx,
	unsigned int localNy,
        unsigned int localNz,
	unsigned int Px,
	unsigned int Py,
        unsigned int Pz,
	int fftDir,
	MPI_Comm comm);
  
  ~FFT3D();
  
  void fft(complex ***in, complex ***out);
  
  void getPx(int &px);
  void getPy(int &py);
  void getPz(int &pz);
  
  void getLocalNx(int &localNx);
  void getLocalNy(int &localNy);
  void getLocalNz(int &localNz);
  void copyZData(complex***in);
  void gatherZ(complex***in, int* w, int dim);
  void gatherX(complex**in, int* w, int dim);
  void gatherY(complex**in, int* w, int dim);
  void gather(complex***in, int*w, int dim);
  // methods for debbugging ...
  void _printComplexArrayR(complex ** array, int nrow, int ncol);
  void _printComplexArrayI(complex ** array, int nrow, int ncol);
#define max(i, j) ( (i>j)?i:j )
};

#endif
