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
#                        I.B.M. CONFIDENTIAL                        #
#      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      #
#                                                                   #
#                LICENSED MATERIALS-PROPERTY OF IBM.                #
#                                                                   #
#####################################################################
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

template <class T>
class Comm3DLayer 
{
protected:
  enum{DIM=3};
  
private:
  int size;
  int _pX, _pY, _pZ, _myPx, _myPy, _myPz;
  int _maxDim[DIM+1];
  
  MPI_Request *_request;
  MPI_Status  *_status; 
public:
  MPI_Comm _cart_comm;
  MPI_Comm _comm[DIM+1];
  MPI_Datatype _mpitype;

  inline Comm3DLayer(int pX, int pY,int pZ, int dim, int *dimSz, int *periods, int reorder);
  inline ~Comm3DLayer();
  inline void getCartCoords(int *myPx, int *myPy, int *myPz) const;
  inline void getCartXCoord( int *myPx ) const;
  inline void getCartYCoord( int *myPy ) const;
  inline void getCartZCoord( int *myPz ) const;
  inline void getCartRank( int *rank ) const;
  inline void getSubCommRank( int subCommDir, int* coords, int *rank) const;
  inline void getSubCommRank( int subCommDir, int *rank) const;
  inline void getCartRank( int* coords, int* rank) const;
  inline void GlobalBarrier(){  MPI_Barrier( MPI_COMM_WORLD); }

  inline void AllToAllV( void* sendBuf,   int*    sendCount, 
		  int*    sendDisp,  void* recvBuf,
		  int*    recvCount, int*    recvDisp, int dir)
    {
      
      MPI_Alltoallv(sendBuf, sendCount, sendDisp, _mpitype,
		    recvBuf, recvCount, recvDisp, _mpitype,
		    _comm[dir]);
//  MPI_Alltoallv(sendBuf, sendCount, sendDisp, MPI_FLOAT,
// 		    recvBuf, recvCount, recvDisp, MPI_FLOAT,
// 		    _comm[dir]);

    }

  inline void AllToAllVInteger( void* sendBuf,   int*    sendCount, 
		  int*    sendDisp,  void* recvBuf,
		  int*    recvCount, int*    recvDisp, int dir)
    {
      MPI_Alltoallv(sendBuf, sendCount, sendDisp, MPI_INTEGER,
		    recvBuf, recvCount, recvDisp, MPI_INTEGER,
		    _comm[dir]);
    }

  inline void AllMaxDouble(void* sendBuf, void* recvBuf, int count)
  {
  	MPI_Allreduce(sendBuf,recvBuf,count, MPI_DOUBLE, MPI_MAX, _cart_comm) ;
  }
  
  inline void Isend(void* Buf, int sendCount, const int trgP, const int dir, const int counter)
  {
    MPI_Isend(Buf, sendCount, MPI_DOUBLE, trgP, dir,
	      _comm[dir], &_request[counter]);
  }
  inline void IsendInteger(void* Buf, int sendCount, const int trgP, const int dir, const int counter)
  {
    MPI_Isend(Buf, sendCount, MPI_INTEGER, trgP, dir,
	      _comm[dir], &_request[counter]);
  }
  
  
 inline void IsendGlobal(void* Buf, int sendCount, const int trgP, const int dir, const int counter)
  {
    MPI_Isend(Buf, sendCount, MPI_DOUBLE, trgP, dir,
	      _cart_comm, &_request[counter]);
  }

 inline void IsendGlobalInteger(void* Buf, int sendCount, const int trgP, const int dir, const int counter)
  {
    MPI_Isend(Buf, sendCount, MPI_INTEGER, trgP, dir,
	      _cart_comm, &_request[counter]);
  }


  inline void Wait(int counter) {MPI_Waitall(counter, _request, _status);}
  inline void Irecv(void* Buf,int recvCount, const int srcP, const int dir, const int counter)
  {
    MPI_Irecv(Buf,
	      recvCount, 
	      MPI_DOUBLE, 
	      srcP, dir,
	      _comm[dir], &_request[counter]);
  }
  
  inline void IrecvInteger(void* Buf,int recvCount, const int srcP, const int dir, const int counter)
  {
    MPI_Irecv(Buf,
	      recvCount, 
	      MPI_INTEGER, 
	      srcP, dir,
	      _comm[dir], &_request[counter]);
  }


  void IrecvGlobal(void* Buf,int recvCount, const int srcP, const int dir, const int counter)
  {
    MPI_Irecv(Buf,
	      recvCount, 
	      MPI_DOUBLE, 
	      srcP, dir,
	      _cart_comm, &_request[counter]);
  }
  
  void IrecvGlobalInteger(void* Buf,int recvCount, const int srcP, const int dir, const int counter)
  {
    MPI_Irecv(Buf,
	      recvCount, 
	      MPI_INTEGER, 
	      srcP, dir,
	      _cart_comm, &_request[counter]);
  }

  inline void getSubCommCoords( int subCommDir, int* coords, int rank) const;

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


template <class T>
Comm3DLayer<T>::Comm3DLayer(int pX, int pY, int pZ,int dim, int *dimSz, int *periods, int reorder)
{
  _pX = pX;  _pY = pY, _pZ = pZ;


//  BegLogLine( 0 ) 
//    << "dimSz: { "
//    << dimSz[ 0 ] << " , "
//    << dimSz[ 1 ] << " , "
//    << dimSz[ 2 ] << " }"
//    << EndLogLine;

  MPI_Cart_create( MPI_COMM_WORLD, dim, dimSz, 
                   periods, reorder, &_cart_comm);
  
  assert(_cart_comm!=NULL); 
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
     printf( "Error in creating the sub-communicator \n"); 
      exit(1);
    }
  
  int myRank, coords[DIM], error_coords;

  _maxDim[0]=_maxDim[3]=1;
  _maxDim[1]=_maxDim[2]=2;

  MPI_Comm_rank(_cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
#define MAX(i, j) ( (i>j)?i:j )
  unsigned int numOfRequest = 2*MAX(_pZ*_pY, _pY*_pX)-2; 
  _request = new MPI_Request[numOfRequest];
  _status  = new MPI_Status[numOfRequest];
   switch (sizeof(T))
	{
	case 4:
	_mpitype = MPI_FLOAT;
        break;
        case 8:
        _mpitype = MPI_DOUBLE;
        break;
        default:
        if(_pX==0&&_pY==0&&_pZ==0) printf("Only supports double and float point 3D FFTs\n");
        MPI_Finalize(); exit(1);
	}
}

template <class T>
Comm3DLayer<T>::~Comm3DLayer() 
{
  delete[] _request;
  delete[] _status;

  _request = NULL;
  _status  = NULL;
}

template <class T>
void Comm3DLayer<T>::getCartXCoord(int *myPx) const
{
  int coords[DIM], myRank;
  MPI_Comm_rank( _cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPx)=coords[0];
}

template <class T>
void Comm3DLayer<T>::getCartYCoord(int *myPy) const
{
  int coords[DIM], myRank;
  MPI_Comm_rank(_cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPy)=coords[1];
}

template <class T>
void Comm3DLayer<T>::getCartZCoord(int *myPz) const
{
  int coords[DIM], myRank;
  MPI_Comm_rank( _cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPz)=coords[2];
}

template <class T>
void Comm3DLayer<T>::getCartCoords(int *myPx, int *myPy, int *myPz) const
{
  int coords[DIM], myRank;
  MPI_Comm_rank( _cart_comm, &myRank);
  MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  (*myPx)=coords[0]; (*myPy)=coords[1]; (*myPz)=coords[2];
  assert((*myPx)<_pX && (*myPy)<_pY &&  (*myPz)<_pZ);
}

template <class T>
void Comm3DLayer<T>::getCartRank(int *rank) const
{
  MPI_Comm_rank(_cart_comm, rank);
}

template <class T>
void Comm3DLayer<T>::getSubCommRank(int subCommDir, int* coords, int *rank) const
{
  MPI_Cart_rank(_comm[subCommDir],coords, rank);
}
template <class T>
void Comm3DLayer<T>::getSubCommRank(int subCommDir, int *rank) const
{
  MPI_Comm_rank(_comm[subCommDir], rank);
}


template <class T>
void Comm3DLayer<T>::getSubCommCoords(int subCommDir, int* coords, int rank) const
{
   MPI_Cart_coords(_comm[subCommDir], rank, _maxDim[subCommDir], coords);
}
template <class T>
void Comm3DLayer<T>::getCartRank( int* coords, int* rank) const
{
  MPI_Cart_rank(_cart_comm,  coords, rank);
}




#endif
