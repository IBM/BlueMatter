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
//  Last modified : 06/02/2006
//  BY   : ME
//  comm3DLayer.hpp
//
// This file provides the implementation of the Comm3DLayer class.
//  

#ifndef __SPI_DRC_H_
#define __SPI_DRC_H_

#if !defined(PKFXLOG_FFT_ALLOCATE)
#define PKFXLOG_FFT_ALLOCATE (0) 
#endif

#ifdef FFT_STANDALONE
#include "complex_MPI.hpp"
#endif

#include <comm3DLayerABC.hpp>
#include <spi/packet_alltoallv.hpp>
// #include <pk/packetlayer_alltoallv.hpp>

// 3 forward 3 reverse
//extern PacketAlltoAllv A2Av[ 6 ] MEMORY_ALIGN( 5 );
//#if !defined(LEAVE_SYMS_EXTERN)
//PacketAlltoAllv A2Av[ 6 ] MEMORY_ALIGN( 5 );
//#endif

// Z group, YZ group, XY group
//extern GroupContainer Groups[ 3 ] MEMORY_ALIGN( 5 );  

//#if !defined(LEAVE_SYMS_EXTERN)
//GroupContainer Groups[ 3 ] MEMORY_ALIGN( 5 );
//#endif

template< class T >
class SpiDRC 
{
  enum { DIM=3 };
private:
  int size;
  int _pX, _pY, _pZ, _myPx, _myPy, _myPz;
  int _maxDim[DIM+1];
  int _Fwd;
  PacketAlltoAllv mA2Av[ 3 ]  MEMORY_ALIGN( 5 );
  GroupContainer mGroups[ 3 ] MEMORY_ALIGN( 5 );

public: 
  SpiDRC(int pX, int pY, int pZ,int dim, int *dimSz, int *periods, int reorder, int fwd_rev)
    :_pX(pX),_pY(pY), _pZ(pZ)
  {  
    int myX, myY, myZ;
    getCartCoords( &myX, &myY, &myZ );  
    
    int z_row[ 3 ]    = { 0, 0, 1 };
    int yz_plane[ 3 ] = { 0, 1, 1 };
    int xy_plane[ 3 ] = { 1, 1, 0 };
    
    mGroups[ 0 ].Init( z_row,    pX, pY, pZ );
    mGroups[ 1 ].Init( yz_plane, pX, pY, pZ );
    mGroups[ 2 ].Init( xy_plane, pX, pY, pZ );

    _maxDim[0]=_maxDim[3]=1;
    _maxDim[1]=_maxDim[2]=2;
  }

  ~SpiDRC(){};

  inline void getCartCoords(int *myPx, int *myPy, int *myPz) const
  { Platform::Topology::GetMyCoords( myPx, myPy, myPz ); }
  
  inline void getCartRank( int *rank ) const
  { *rank =  Platform::Topology::GetAddressSpaceId(); }

  inline void getCartRank( int* coords, int* rank) const
  {
    getCartRank( rank );
    getCartCoords( &coords[0], &coords[1], &coords[2] );
  }

  inline void getSubCommRank( int subCommDir, int* coords, int *rank) const
  { *rank = mGroups[ subCommDir ].GetRankFromCoord( coords );}
 
  inline void getSubCommRank( int subCommDir, int *rank) const
  {
    assert( subCommDir >= 0 && subCommDir < 4 );
    *rank = mGroups[ subCommDir ].mMyRank;    
  }

  inline void getSubCommSize( int subCommDir, int *size) const
  {
    assert( subCommDir >= 0 && subCommDir < 4 );
    *size = mGroups[ subCommDir ].mSize;
  }

  inline void globalBarrier(){ Platform::Control::Barrier(); }
  
  void allToAllv(void* sendBuf,int* sendCount, int* sendDisp,  
		 void* recvBuf,int* recvCount, int* recvDisp, int dir )
  {    
    assert( dir >=0 && dir < 3 );
    //int index = Fwd ? dir : dir+3;
    //assert( index >=0 && index < 6 );
    
    //    BegLogLine(PKFXLOG_FFT_ALLOCATE)
    // << "AllToAllV_Execute sendBuf=" << sendBuf
    // << " recvBuf=" << recvBuf
    // << " index=" << index 
    // << EndLogLine ;
    
    mA2Av[ dir ].ExecuteSingleCore1< 0 >( sendBuf, recvBuf );    
  }
  
  void AllToAllV_Init( int* sendCount, int* sendDisp,
                       int* recvCount, int* recvDisp, 
                       int dir)
  {
    //int index = Fwd ? dir : dir+3;
    //assert( index >=0 && index < 6 );
    mA2Av[ dir ].InitSingleCore( sendCount, sendDisp, sizeof( T ),
				 recvCount, recvDisp, sizeof( T ),
				 &mGroups[ dir ] );
  }
  
};


#endif
