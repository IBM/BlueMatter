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
//  An object of class Comm3D implements communications between two procs
//  in a 3D grid. 
//
#include <iostream.h>
#include "comm3DLayer.hpp"
#include <assert.h>

Comm3DLayer::Comm3DLayer(int pX, int pY, int pZ, MPI_Comm comm)
{
  _pX = pX;  _pY = pY, _pZ = pZ;
  complex* _buffer = (complex*) new complex[ MAX_BUFFER_SIZE ];
  
  //MPI_Comm_dup(comm, &_cart_comm);
  //  int error_zz, error_zx, error_xy, error_xx;
  //   int remain_dims_zz[DIM]={0,0,1};
  //   int remain_dims_zx[DIM]={1,0,1};
  //   int remain_dims_xy[DIM]={1,1,0};
  //   int remain_dims_xx[DIM]={1,0,0};
  
  // error_zz = MPI_Cart_sub(_cart_comm, remain_dims_zz, &_comm[0]);
  //   error_zx = MPI_Cart_sub(_cart_comm, remain_dims_zx, &_comm[1]);
  //   error_xy = MPI_Cart_sub(_cart_comm, remain_dims_xy, &_comm[2]);    
  //   error_xx = MPI_Cart_sub(_cart_comm, remain_dims_xx, &_comm[3]);
  
  //   if(error_zz || error_zx || error_xy || error_xx) 
  //   { 
  //     cout << "Error in creating the sub-communicator" <<endl; 
  //     exit(1);
  //   }
  
  int myRank, coords[ DIM ], error_coords;

  //  MPI_Comm_rank(_cart_comm, &myRank);
  // MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  
  //getMyPx(&_myPx);
  //getMyPy(&_myPy);
  //getMyPz(&_myPz);
}

Comm3DLayer::~Comm3DLayer() 
{
  delete[] _buffer;
}


void Comm3DLayer::send(int targetPx, int targetPy, int targetPz, 
		       complex* buffer, int size, int dim)
{
  assert(dim==0 || dim==1 || dim==2 || dim==3);
  int targetRank;
  
  if(  (dim ==0 && targetPx != _myPx && targetPy != _myPy)
       || (dim ==1 && targetPy != _myPy)
       || (dim ==2 && targetPz != _myPz)
       || (dim ==3 && targetPx != _myPx && targetPz != _myPz)) 
    {
      
      cout<<"Error: You are using wrong communicator " << endl;
      cout <<"targetPz ="<< targetPz<<
        "myPz ="<< _myPz<<endl;
    }
  
  int error;
  int coords[DIM] = {targetPx, targetPy, targetPz};   
  //MPI_Cart_rank(_cart_comm, coords, &targetRank);
  //MPI_Send((void*)buffer, 2, MPI_DOUBLE, targetRank, 1, _cart_comm);
}

void Comm3DLayer::recv(int sourcePx, int sourcePy, int sourcePz, 
		      complex* buffer,  int size, int dim) {
  assert(dim==0 || dim==1 || dim==2 || dim==3); 
  int sourceRank;
  if(  (dim ==0 && sourcePx != _myPx && sourcePy != _myPy)
    || (dim ==1 && sourcePy != _myPy)
    || (dim ==2 && sourcePz != _myPz)
    || (dim ==3 && sourcePx != _myPx && sourcePz != _myPz)) 
    {
      cout<<"Error: You are using wrong communicator " << endl;
    }
  
  int coords[ DIM ] = { sourcePx, sourcePy, sourcePz };
 
 //  MPI_Cart_rank(_cart_comm, coords, &sourceRank);
  
//   MPI_Status error; 
//   MPI_Recv((void*)buffer, 2, MPI_DOUBLE, sourceRank, 1, _cart_comm, &error);
}

void Comm3DLayer::getMyPx(int *myPx)
{
  int coords[DIM], myRank;
  MPI_Comm_rank( _cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPx)=coords[0];
}

void Comm3DLayer::getMyPy(int *myPy) 
{
  int coords[DIM], myRank;
  //  MPI_Comm_rank( _cart_comm, &myRank);
  //   MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  //   (*myPy)=coords[1];
}

void Comm3DLayer::getMyPz(int *myPz)
{
  //   int coords[DIM], myRank;
  //   MPI_Comm_rank( _cart_comm, &myRank);
  //   MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  //   (*myPz)=coords[2];
}

void Comm3DLayer::getMyRank(int *rank)
{
  //MPI_Comm_rank(_cart_comm, rank);
}
