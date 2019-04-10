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
//  Date : 02/10/2003
//  Name : Maria Eleftheriou
//  FFT.hpp
//

//
//  An object of class Comm2D implements communications between two procs
//  in a 2D grid. 
//

#ifndef __COMM3DLAYER_H_
#define __COMM3DLAYER_H_
#include <iostream>
#include "mpi.h"
#include "complex.hpp"
//#include "../fftw/complex.hpp"

class Comm3DLayer {
protected:
  enum{ DIM=3, MAX_BUFFER_SIZE=100 };


  //private:
public:
  complex* _buffer;   
  int size;
  MPI_Comm _cart_comm;
  MPI_Comm _comm[ DIM ];
  int _pX, _pY, _pZ, _myPx, _myPy, _myPz;

 public:
  Comm3DLayer(int pX, int pY,int pZ, MPI_Comm comm);
  ~Comm3DLayer();
 
  
  void getMyPx(int *myPx);
  void getMyPy(int *myPy);
  void getMyPz(int *myPz);
  void getMyRank(int *rank);

  void MPIBarrier(int dim) { MPI_Barrier(_cart_comm);}
  // send method sends a vector of complex numbers
  void send(int targetPx, int targetPy, int targetPz, 
	    complex* buffer, int size, int dim);
  // recv method, receives a vector of complex numbers
  void recv(int sourcePx, int sourcePy, int sourcePz, 
	    complex* buffer, int size, int dim);


};

#endif



