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

#if !defined(PKFXLOG_FFT_ALLOCATE)
#define PKFXLOG_FFT_ALLOCATE (0) 
#endif
//#include <iostream>
//#include <assert.h>
// #include "mpi.h"
#ifdef FFT_STANDALONE
#include "complex_MPI.hpp"
#endif



#include <spi/packet_alltoallv.hpp>
//class SubGroupElement

// extern "C" 
//   {
//   void Packet_Alltoallv( void* sendBuf, int* sendCount, int* sendDisp, int send_type_size,
//                          void* recvBuf, int* recvCount, int* recvDisp, int recv_type_size,
//                          GroupContainer aGroup );
//   }

// 3 forward 3 reverse
extern PacketAlltoAllv A2Av[ 6 ] MEMORY_ALIGN( 5 );
#if !defined(LEAVE_SYMS_EXTERN)
PacketAlltoAllv A2Av[ 6 ] MEMORY_ALIGN( 5 );
#endif

// Z group, YZ group, XY group
extern GroupContainer Groups[ 3 ] MEMORY_ALIGN( 5 );  
#if !defined(LEAVE_SYMS_EXTERN)
GroupContainer Groups[ 3 ] MEMORY_ALIGN( 5 );
#endif

template< class T >
class Comm3DLayer 
{
    enum { DIM=3 };
private:
  complexTemplate<T>* _buffer;   
  int size;
  int _pX, _pY, _pZ, _myPx, _myPy, _myPz;
  int _maxDim[DIM+1];
  
//   MPI_Request *_request;
//   MPI_Status  *_status; 
public:
//   MPI_Comm _cart_comm;
//   MPI_Comm _comm[DIM+1];

  

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
  inline void GlobalBarrier(){    Platform::Control::Barrier(); }
  inline void getSubCommSize( int subCommDir, int *size) const;

  void AllToAllV_Execute( void* sendBuf, 
                          void* recvBuf, 
                          int dir,
                          int Fwd )
    {    
    assert( dir >=0 && dir < 3 );
    int index = Fwd ? dir : dir+3;
    assert( index >=0 && index < 6 );


    BegLogLine(PKFXLOG_FFT_ALLOCATE)
      << "AllToAllV_Execute sendBuf=" << sendBuf
      << " recvBuf=" << recvBuf
      << " index=" << index 
     << EndLogLine ;
//     long long TimeStart = rts_get_timebase();
    A2Av[ index ].ExecuteSingleCore1< 0 >( sendBuf, recvBuf );    
//     long long TimeFinis = rts_get_timebase();
//     double TimeInPhase = 1.0*(TimeFinis-TimeStart) / (700.0 * 1000.0) ;
//     printf("TimeInPhase %d: %18.16f\n", dir, TimeInPhase );
    }
    
  void AllToAllV_Init( int*    sendCount, int*    sendDisp,
                       int*    recvCount, int*    recvDisp, 
                       int dir, int Fwd)
        {
//             MPI_Alltoallv(sendBuf, sendCount, sendDisp, MPI_DOUBLE,
//                           recvBuf, recvCount, recvDisp, MPI_DOUBLE,
//                           _comm[dir]);
        int index = Fwd ? dir : dir+3;
        assert( index >=0 && index < 6 );
        A2Av[ index ].InitSingleCore( sendCount, sendDisp, sizeof( T ),
                                      recvCount, recvDisp, sizeof( T ),
                                      &Groups[ dir ] );
        }
  
//   void Isend(void* Buf, int sendCount, const int trgP, const int dir, const int counter)
//   {
//     MPI_Isend(Buf, sendCount, MPI_DOUBLE, trgP, dir,
// 	      _comm[dir], &_request[counter]);
//   }
  
//  void IsendGlobal(void* Buf, int sendCount, const int trgP, const int dir, const int counter)
//   {
//     MPI_Isend(Buf, sendCount, MPI_DOUBLE, trgP, dir,
// 	      _cart_comm, &_request[counter]);
//   }

    // void Wait(int counter) { MPI_Waitall(counter, _request, _status); }

//   void Irecv(void* Buf,int recvCount, const int srcP, const int dir, const int counter)
//   {
//     MPI_Irecv(Buf,
// 	      recvCount, 
// 	      MPI_DOUBLE, 
// 	      srcP, dir,
// 	      _comm[dir], &_request[counter]);
//   }

//   void IrecvGlobal(void* Buf,int recvCount, const int srcP, const int dir, const int counter)
//   {
//     MPI_Irecv(Buf,
// 	      recvCount, 
// 	      MPI_DOUBLE, 
// 	      srcP, dir,
// 	      _cart_comm, &_request[counter]);
//   }

  // void getSubCommCoords( int subCommDir, int* coords, int rank) const;
  
  void MPIGlobalBarrier( ) 
    {
    // MPI_Barrier( _cart_comm );
    Platform::Control::Barrier();
    }

//   void MPISubBarrier( int subCommDir ) 
//     {
//       MPI_Barrier( _comm[subCommDir] );
//     }
  
};

template< class T >
Comm3DLayer<T>::Comm3DLayer(int pX, int pY, int pZ,int dim, int *dimSz, int *periods, int reorder)
{
  _pX = pX;  _pY = pY, _pZ = pZ;

//   MPI_Cart_create( MPI_COMM_WORLD, dim, dimSz, 
//                    periods, reorder, &_cart_comm );
  
  // assert( _cart_comm != MPI_NULL ); 

//   int error_zz, error_zy, error_yx, error_xx;
//   int remain_dims_zz[DIM] = {0,0,1};
//   int remain_dims_zy[DIM] = {0,1,1};
//   int remain_dims_yx[DIM] = {1,1,0};
//   int remain_dims_xx[DIM] = {1,0,0};
     
//   error_zz = MPI_Cart_sub(_cart_comm, remain_dims_zz, &_comm[0]);
//   error_zy = MPI_Cart_sub(_cart_comm, remain_dims_zy, &_comm[1]);
//   error_yx = MPI_Cart_sub(_cart_comm, remain_dims_yx, &_comm[2]);    
//   error_xx = MPI_Cart_sub(_cart_comm, remain_dims_xx, &_comm[3]);  

  int myX, myY, myZ;
  getCartCoords( &myX, &myY, &myZ );  

  int z_row[ 3 ]    = { 0, 0, 1 };
  int yz_plane[ 3 ] = { 0, 1, 1 };
  int xy_plane[ 3 ] = { 1, 1, 0 };
  
  Groups[ 0 ].Init( z_row,    pX, pY, pZ );
  Groups[ 1 ].Init( yz_plane, pX, pY, pZ );
  Groups[ 2 ].Init( xy_plane, pX, pY, pZ );
  
  // int myRank, coords[DIM], error_coords;
  _maxDim[0]=_maxDim[3]=1;
  _maxDim[1]=_maxDim[2]=2;

  // MPI_Comm_rank(_cart_comm, &myRank);
  // myRank = Platform::Topology::GetAddressSpaceId();
  // MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
  // Platform::Topology::GetMyCoords( &coords[0], &coords[1], &coords[2] );
  
// #define MAX(i, j) ( (i>j)?i:j )

//   unsigned int numOfRequest = 2*MAX(_pZ*_pY, _pY*_pX)-2; 
//   _request = new MPI_Request[numOfRequest];
//   _status  = new MPI_Status[numOfRequest];
}

template< class T >
Comm3DLayer<T>::~Comm3DLayer() 
{
//   delete[] _request;
//   delete[] _status;

//   _request = NULL;
//   _status  = NULL;
}

template< class T >
void Comm3DLayer<T>::getCartXCoord(int *myPx) const
  {
//   int coords[DIM], myRank;
//   MPI_Comm_rank( _cart_comm, &myRank);
//   MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
//   (*myPx)=coords[0];
  int dummy;
  Platform::Topology::GetMyCoords( myPx, &dummy, &dummy );
  }

template< class T >
void Comm3DLayer<T>::getCartYCoord(int *myPy) const
{
//   int coords[DIM], myRank;
//   MPI_Comm_rank(_cart_comm, &myRank);
//   MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
//   (*myPy)=coords[1];
  int dummy;
  Platform::Topology::GetMyCoords( &dummy, myPy, &dummy );
}

template< class T >
void Comm3DLayer<T>::getCartZCoord(int *myPz) const
{
//   int coords[DIM], myRank;
//   MPI_Comm_rank( _cart_comm, &myRank);
//   MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
//   (*myPz)=coords[2];

  int dummy;
  Platform::Topology::GetMyCoords( &dummy, &dummy, myPz );
}

template< class T >
void Comm3DLayer<T>::getCartCoords(int *myPx, int *myPy, int *myPz) const
{
//   int coords[DIM], myRank;
//   MPI_Comm_rank( _cart_comm, &myRank);
//   MPI_Cart_coords(_cart_comm, myRank, DIM, coords);
//   (*myPx)=coords[0]; (*myPy)=coords[1]; (*myPz)=coords[2];
  Platform::Topology::GetMyCoords( myPx, myPy, myPz );
}

template< class T >
void Comm3DLayer<T>::getCartRank(int *rank) const
  {
// MPI_Comm_rank(_cart_comm, rank);
  *rank =  Platform::Topology::GetAddressSpaceId();
  }

template< class T >
void Comm3DLayer<T>::getSubCommSize( int subCommDir, int *size) const
  {
  // MPI_Comm_size( _comm[subCommDir], size);
  assert( subCommDir >= 0 && subCommDir < 4 );
  *size = Groups[ subCommDir ].mSize;
  }

template< class T >
void Comm3DLayer<T>::getSubCommRank(int subCommDir, int* coords, int *rank) const
  {
  // MPI_Cart_rank(_comm[subCommDir],coords, rank);
  assert( subCommDir >= 0 && subCommDir < 4 );
  
  *rank = Groups[ subCommDir ].GetRankFromCoord( coords );
  }

template< class T >
void Comm3DLayer<T>::getSubCommRank(int subCommDir, int *rank) const
  {
  // MPI_Comm_rank(_comm[subCommDir], rank);
  assert( subCommDir >= 0 && subCommDir < 4 );
  *rank = Groups[ subCommDir ].mMyRank;    
  }

// void Comm3DLayer::getSubCommCoords(int subCommDir, int* coords, int rank) const
// {
//    MPI_Cart_coords(_comm[subCommDir], rank, _maxDim[subCommDir], coords);
// }

template< class T >
void Comm3DLayer<T>::getCartRank( int* coords, int* rank) const
  {
// MPI_Cart_rank(_cart_comm,  coords, rank);  
  getCartRank( rank );
  getCartCoords( &coords[0], &coords[1], &coords[2] );
  }
#endif
