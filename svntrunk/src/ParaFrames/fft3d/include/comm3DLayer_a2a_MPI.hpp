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
//  comm3DLayer.hpp
//

//
//  An interface w the mpi. Allows the communication between 
// processor in the same
//

#ifndef __COMM3DLAYER_H_
#define __COMM3DLAYER_H_
#include <iostream>
#include <assert.h>
#include "mpi.h"
#include <MPI/complex_MPI.hpp>

class Comm3DLayer 
{
protected:
  enum{DIM=3};
  
private:
  complex* _buffer;   
  int size;
  int _pX, _pY, _pZ, _myPx, _myPy, _myPz;
  int _maxDim[DIM+1];
public:
  MPI_Comm _cart_comm;
  MPI_Comm _comm[DIM+1];

  Comm3DLayer(int pX, int pY,int pZ, MPI_Comm comm);
  ~Comm3DLayer();

  void getCartXCoord( int *myPx ) const;
  void getCartYCoord( int *myPy ) const;
  void getCartZCoord( int *myPz ) const;
  void getCartRank( int *rank ) const;
  void getSubRank( int subCommDir, int* coords, int *rank) const;
  void getSubSize( int subCommDir, int* comm_size) const;

  void AllToAllV( double* sendBuf,   int*    sendCount, 
		  int*    sendDisp,  double* recvBuf,
		  int*    recvCount, int*    recvDisp, int dir)
    {
      MPI_Alltoallv(sendBuf, sendCount, sendDisp, MPI_DOUBLE,
		    recvBuf, recvCount, recvDisp, MPI_DOUBLE,
		    _comm[dir]);

    }
  

  void getSubCoords( int subCommDir, int* coords, int rank) const;

  void getSubCommSize( int subCommDir, int *size) const
    {
      MPI_Comm_size( _comm[subCommDir], size);
    }
  
  void MPIGlobalBarrier( ) 
    {
      MPI_Barrier( _cart_comm );
    }

  void MPISubBarrier( int subCommDir ) 
    {
      MPI_Barrier( _comm[subCommDir] );
    }
  
};


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



Comm3DLayer::Comm3DLayer(int pX, int pY, int pZ, MPI_Comm comm)
{
  _pX = pX;  _pY = pY, _pZ = pZ;
  
  MPI_Comm_dup(comm, &_cart_comm);
  int error_zz, error_zy, error_yx, error_xx;
  int remain_dims_zz[DIM] = {0,0,1};
  int remain_dims_zy[DIM] = {0,1,1};
  int remain_dims_yx[DIM] = {1,1,0};
  int remain_dims_xx[DIM] = {1,0,0};
     
  error_zz = MPI_Cart_sub(_cart_comm, remain_dims_zz, &_comm[0]);
  error_zy = MPI_Cart_sub(_cart_comm, remain_dims_zy, &_comm[1]);
  error_yx = MPI_Cart_sub(_cart_comm, remain_dims_yx, &_comm[2]);    
  error_xx = MPI_Cart_sub(_cart_comm, remain_dims_xx, &_comm[3]);
  

  if(error_zz || error_zy || error_yx || error_xx) 
    { 
      cerr << "Error in creating the sub-communicator" <<endl; 
      exit(1);
    }
  
  int myRank, coords[DIM], error_coords;

  _maxDim[0]=_maxDim[3]=1;
  _maxDim[1]=_maxDim[2]=2;

  MPI_Comm_rank(_cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
 }

Comm3DLayer::~Comm3DLayer() 
{
  
}

void Comm3DLayer::getCartXCoord(int *myPx) const
{
  int coords[DIM], myRank;
  MPI_Comm_rank( _cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPx)=coords[0];
}

void Comm3DLayer::getCartYCoord(int *myPy) const
{
  int coords[DIM], myRank;
  MPI_Comm_rank(_cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPy)=coords[1];
}

void Comm3DLayer::getCartZCoord(int *myPz) const
{
  int coords[DIM], myRank;
  MPI_Comm_rank( _cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPz)=coords[2];
}

void Comm3DLayer::getCartRank(int *rank) const
{
  MPI_Comm_rank(_cart_comm, rank);
}

void Comm3DLayer::getSubRank(int subCommDir, int* coords, int *rank) const
{
  MPI_Cart_rank(_comm[subCommDir],coords, rank);
}

void Comm3DLayer::getSubCoords(int subCommDir, int* coords, int rank) const
{
   MPI_Cart_coords(_comm[subCommDir], rank, _maxDim[subCommDir], coords);
}

void Comm3DLayer::getSubSize(int subCommDir, int* comm_size) const
{
  MPI_Comm_size(_comm[subCommDir], comm_size);
}


#endif
