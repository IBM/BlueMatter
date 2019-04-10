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
#      (c) COPYRIGHT IBM CORP. 2005, 2007 ALL RIGHTS RESERVED.      #
#                                                                   #
#                LICENSED MATERIALS-PROPERTY OF IBM.                #
#                                                                   #
#####################################################################
*/

/* $Id    : PrivateFFT.hpp
   Date   : 07/23/2003  11:41:51
   Name   : Maria Eleftheriou, IBM T.J.W R.C.
   Last Modified: 06/02/06 
   By     : ME
*/

#if defined(USE_PK_ACTORS)
#include <Pk/API.hpp>
#include <Pk/Compatibility.hpp>
#else
#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/pktrace.hpp>
#endif

//#if defined(PK_XLC)
//#include <builtins.h>
//#endif

//#include <string.h>
//#include <stdlib.h>

#ifndef MPI_ALLTOALL
#define MPI_ALLTOALL 1
#endif

#if !defined(PKFXLOG_FFT_ALLOCATION)
#define PKFXLOG_FFT_ALLOCATION (0) 
#endif

#if !defined(PKFXLOG_FFT_VALUES)
#define PKFXLOG_FFT_VALUES (0)
#endif

#if !defined(PKFXLOG_FFT_SUMMARY)
#define PKFXLOG_FFT_SUMMARY (0)
#endif

#if !defined(PKFXLOG_FFT_DETAIL)
#define PKFXLOG_FFT_DETAIL (0)
#endif

#if !defined(PKTRACE_BGL3DFFT_BENCH)
#define PKTRACE_BGL3DFFT_BENCH (0)
#endif


#ifndef PKTRACE_TUNE 
#define PKTRACE_TUNE ( 0 )
#endif


//#define  BENCH_A2A 1

#include <comm3DLayerABC.hpp>
#if defined(FFT_ESSL)
#include <fftOneDimESSL.hpp>
#else
#include <fftOneDim.hpp>
#endif
#include <arrayBounds.hpp>
#ifdef PK_BLADE_SPI
#include <spiDrc.hpp>
#else
#include <mpiDrc.hpp>
#endif

// #include <complexTemplate.hpp>
#include <PrivateFFT.hpp>

static TraceClient TR_FFT_1D_Start;
static TraceClient TR_FFT_1D_Finis;

static TraceClient TR_BGL3DFFT_Start;
static TraceClient TR_BGL3DFFT_Finis;

static TraceClient TR_FFT_1D_R_Start;
static TraceClient TR_FFT_1D_R_Finis;

//Trace points for the a2a comm operator

static TraceClient TR_BGL3DFFT_FW_A2A_Z_Start;
static TraceClient TR_BGL3DFFT_FW_A2A_Z_Finis;

static TraceClient TR_BGL3DFFT_FW_A2A_Y_Start;
static TraceClient TR_BGL3DFFT_FW_A2A_Y_Finis;

static TraceClient TR_BGL3DFFT_FW_A2A_X_Start;
static TraceClient TR_BGL3DFFT_FW_A2A_X_Finis;

static TraceClient TR_BGL3DFFT_FW_COMP_Z_Start;
static TraceClient TR_BGL3DFFT_FW_COMP_Z_Finis;

static TraceClient TR_BGL3DFFT_FW_COMP_Y_Start;
static TraceClient TR_BGL3DFFT_FW_COMP_Y_Finis;

static TraceClient TR_BGL3DFFT_FW_COMP_X_Start;
static TraceClient TR_BGL3DFFT_FW_COMP_X_Finis;

#ifndef FORWARD
#define FORWARD -1
#endif

#undef MAX
#define MAX(i, j) ( ((i)>(j))?(i):(j) )


template <typename T, typename Tcomplex > 
void PrivateFFT<T,Tcomplex>::Initialize( int fwd_rev )
{  
  // initialize parms to set the mpi_cartesian
  int dimSz[PrivateFFT::DIM-1] = {_pX, _pY, _pZ};
  int periods[PrivateFFT::DIM-1]  = {0, 0, 0};  // logical array, specifying whether the
  
  // grid is periodic, in each dim.   
  int reorder = 0;       //ranking is not reordered.
  
#ifdef PK_BLADE_SPI
  _comm = new SpiDRC<T>(_pX, _pY, _pZ, DIM-1, dimSz, periods, reorder, fwd_rev );

#if defined(USE_PK_ACTORS) 
  _myRank = PkNodeGetId() ;
#else
  _myRank=Platform::Topology::GetAddressSpaceId();
#endif
  
#else
  _comm = new MpiDRC<T>(_pX, _pY, _pZ, DIM-1, dimSz, periods, reorder);  
  MPI_Comm_rank(MPI_COMM_WORLD, &_myRank);
#endif

  assert(_comm!=NULL);
  (*_comm).getCartCoords(&_myPx, &_myPy, &_myPz);
  
  Plan( fwd_rev );
  
  SetSendRecvIndices();

  AllocateMemory();

  SetAllToAllIndexInPlane(DIR_OZ,   1, _pZ);
  SetAllToAllIndexInPlane(DIR_ZY, _pZ, _pY);
  SetAllToAllIndexInPlane(DIR_YX, _pY, _pX);
  
  BegLogLine(PKFXLOG_FFT_ALLOCATION)
    << "Calling AllToAllV_Init"
    << EndLogLine ;

#ifdef PK_BLADE_SPI
  
  for (int bdir=0; bdir<(DIM-1); bdir++) 
    {
      if( fwd_rev== FORWARD )
	(*_comm).AllToAllV_Init( _sendCount[ bdir], _sendDisp[ bdir ],
				 _recvCount[ bdir], _recvDisp[ bdir ],
				 bdir );
      else
	(*_comm).AllToAllV_Init( _recvCount[ bdir], _recvDisp[ bdir ],
				 _sendCount[ bdir], _sendDisp[ bdir ],
				 bdir );
    } 
#endif
  
  // Checksum _recvCount and _recvDisp
  //          _sendCount and _sendDisp
  
  _checksum = GetChecksum(); 
  
  _strideXYZ[0] = _localNy[0]*_localNz[0];
  _strideXYZ[1] = _localNz[0];
  _strideXYZ[2] =  1;

  _strideR0     = 1; 
  _strideR1     = _localNx[0]*_pX;
  _strideR2     = _strideR1*_localNy[3];
  
  //int commSize=-1;
  //(*_comm).getSubCommSize(0, &commSize); 
  
  _strideSrcA[0][2] = 1;
  _strideSrcA[0][1] = _localNz[0]*_nFFTs[0];
  _strideSrcA[0][0] = _localNz[0];
   
  _strideTrgA[0][2] = _nFFTs[0];
  _strideTrgA[0][1] = _nFFTs[0]*_localNz[0];
  _strideTrgA[0][0] = 1;

  _localSizeA[0][2] = _localNz[0];
  _localSizeA[0][1] =  _pZ;
  _localSizeA[0][0] = _nFFTs[0];

  //commSize=-1;
  
  //(*_comm).getSubCommSize(1, &commSize);
    
  _strideSrcA[1][2] =  1;
  _strideSrcA[1][1] = _localNy[0]*_nFFTs[1]; 
  _strideSrcA[1][0] = _localNy[0];
 
  _strideTrgA[1][2] = _nFFTs[1];
  _strideTrgA[1][1] = _nFFTs[1]*_localNy[0];
  _strideTrgA[1][0] = 1;

   
  _localSizeA[1][2] = _localNy[0];
  _localSizeA[1][1] = _pY;
  _localSizeA[1][0] = _nFFTs[1];


  _strideSrcA[2][2] = 1;
  _strideSrcA[2][1] = _localNx[2]*_nFFTs[2]; 
  _strideSrcA[2][0] = _localNx[2];
  
  _strideTrgA[2][2] = _nFFTs[2]*(_pX);
  _strideTrgA[2][1] = _nFFTs[2];
  _strideTrgA[2][0] = 1;
 
  _localSizeA[2][2] = _localNx[2];


  //(_pX/_pStep[0])*(_localNx[0]/_localNx[2]);

  if(_localNx[2]!=0)_localSizeA[2][1] = (_pX)*((_localNx[0]/_localNx[2]));
  else _localSizeA[2][1] = 0;

   //printf("LOCALNX %d %d %d %d %d \n", _localNx[0], _localNx[2], _pStep[0], (_localNx[0]/_localNx[2])/_pStep[0],
//
	//		_localNx[0]/(_localNx[2]/_pStep[0])  ); 



  _localSizeA[2][0] = _nFFTs[2];
  
  if(_myPx==10000&& _myPy==0)
    for(int d=0; d<3; d++)
      {
       
        BegLogLine(PKFXLOG_FFT_ALLOCATION) 
          << " localSizeA[" << _myRank 
          << "][" << d
          << "][0, 1, 2] = (" << _localSizeA[d][0]
          << ", " << _localSizeA[d][1]
          << ", " << _localSizeA[d][2]
          << EndLogLine ;
        BegLogLine(PKFXLOG_FFT_ALLOCATION) 
          << " strideSrcA[" << _myRank 
          << "][" << d
          << "][0, 1, 2] = (" << _strideSrcA[d][0]
          << ", " << _strideSrcA[d][1]
          << ", " << _strideSrcA[d][2]
          << EndLogLine ;
        BegLogLine(PKFXLOG_FFT_ALLOCATION) 
          << " strideTrgA[" << _myRank 
          << "][" << d
          << "][0, 1, 2] = (" << _strideTrgA[d][0]
          << ", " << _strideTrgA[d][1]
          << ", " << _strideTrgA[d][2]
          << EndLogLine ;
      }
  
  
  int xSize = _pStep[0]*MAX(MAX(_localSizeA[0][0],_localSizeA[1][0]),_localSizeA[2][0]);
  int ySize = _pStep[1]*MAX(MAX(_localSizeA[0][1],_localSizeA[1][1]),_localSizeA[2][1]);
  int zSize = _pStep[2]*MAX(MAX(_localSizeA[0][2],_localSizeA[1][2]),_localSizeA[2][2]);

  _trgIndex = new int***[DIM-1];
  
  BegLogLine(PKFXLOG_FFT_ALLOCATION)
    << " this=" << this
    << " _trgIndex=" << _trgIndex
    << EndLogLine ;
  
  //assert(xSize!=0&&ySize!=0&&zSize!=0);
  
  for(int d=0; d<DIM-1; d++)
    {
      if(xSize==0 || ySize==0 || zSize==0) continue;
      BegLogLine(PKFXLOG_FFT_ALLOCATION)
	<< " d=" << d
	<< " allocating int**[" << xSize
	<< "]"
	<< EndLogLine ;
      _trgIndex[d] = new int**[xSize];
     
      assert(_trgIndex[d]!=NULL);
      for(int i=0; i<xSize; i++)
	{
	  BegLogLine(PKFXLOG_FFT_ALLOCATION)
	      << " d=" << d
	      << " i=" << i
	      << " allocating int*[" << ySize
	      << "]"
	      << EndLogLine ;
 	  _trgIndex[d][i] = new int*[ySize];
	  assert(_trgIndex[d][i]!=NULL);
	  
	  for(int j=0; j<ySize; j++)
	    {
	    BegLogLine(PKFXLOG_FFT_ALLOCATION)
	      << " d=" << d
	      << " i=" << i
	      << " j=" << j
	      << " allocating int[" << zSize
	      << "]"
	      << EndLogLine ;
	      _trgIndex[d][i][j] = new int[zSize];
	    BegLogLine(PKFXLOG_FFT_ALLOCATION)
	      << "_trgIndex[d][i][j]=" << (void *)_trgIndex[d][i][j]
	      << EndLogLine ;
	      
	      assert(_trgIndex[d][i][j]!=NULL);
	    }
	}
    }
      //MPI_Barrier(MPI_COMM_WORLD);if(_myRank==0) printf("after allocating ... \n");                                                                                             
  for(int d=0; d<DIM-1; d++)
    {
      // if(_myPx==0&&_myPy==0) 
      // printf("myFFT [%d] [%d] = %d \n", _myPz, d, _nFFTs[d]); 
      int localNx    = _localSizeA[d][0];
      int localNy    = _localSizeA[d][1];
      int localNz    = _localSizeA[d][2];
      
      if ((localNx==0 ||localNy==0 || localNz==0)&&_myPx==0) continue;

      int trgStride0 = _strideTrgA[d][0];
      int trgStride1 = _strideTrgA[d][1];
      int trgStride2 = _strideTrgA[d][2];
          
      int counter=0;
      
      for(int i=0; i<localNx; i++)
	for(int j=0; j<localNy; j++)
	  for(int k=0; k<localNz; k++)
	    {
	      if(d==0)
		{ 		    
		  _trgIndex[d][i][j][k] =  i*trgStride0+j*trgStride1+k*trgStride2;
		  
		  //if(_myRank==2) printf(" ID[%d] trgIndex[%d][%d][%d][%d]= %d \n",_myRank, d, i,j,k,  _trgIndex[d][i][j][k]); 

		}
	      else if(d==2)
		{
		  _trgIndex[d][i][j][k] = counter;
		  counter++;
		  // if(_myRank==0) printf(" ID[%d] trgIndex[%d][%d][%d][%d]= %d \n",_myRank,_myRank, d, i,j,k,  _trgIndex[d][i][j][k]); 
		} 
	      else if(d==1)
		{
		  int  trgIndex = i*trgStride0+j*trgStride1+k*trgStride2;
		  int howMany = (_localNx[d]*_localNz[d+1])/(_pZ*_pY);
		  if(_localNx[d]!=0 &&_localNz[d+1]!=0)
		    {
		      int ii    = i     % _localNx[d];
		      int alpha = i     / _localNx[d];
		      int kk    = alpha % _localNz[d+1];
		      int pZ    = alpha / _localNz[d+1];
		      
		      trgIndex  = j*trgStride1+(((kk)*_localNy[d]+k))
			*_localNx[d+1]+ (pZ*_localNx[d]+ii);
		      
		      _trgIndex[d][i][j][k] =  trgIndex;
		      //if(_myRank==0||_myRank==1) printf(" ID[%d] trgIndex[%d][%d][%d][%d]= %d \n",_myRank,_myRank, d, i,j,k,  _trgIndex[d][i][j][k]); 
		    }
		  
		}
		 
	    }
    }
    
  for(int d=0; d<DIM-1; d++)
    {
      if(xSize==0 || ySize==0 || zSize==0) continue;
      BegLogLine(PKFXLOG_FFT_ALLOCATION)
	<< " d=" << d
	<< " _trgIndex[d]=" << _trgIndex[d]
	<< EndLogLine ;
      assert(_trgIndex[d]!=NULL);
    }

//MPI_Barrier(MPI_COMM_WORLD);if(_myRank==0) printf("allocationg 1D fft buffers\n");    
  //---------------------//
  _fft[2] = new  FFTOneD<T,Tcomplex>(_localNx[0]*_pX, fwd_rev); 
  // 1D fft in the x-dim
  _fft[1] = new  FFTOneD<T,Tcomplex>(_localNy[0]*_pY, fwd_rev); 
  // 1D fft in the y-dim
  _fft[0] = new  FFTOneD<T,Tcomplex>(_localNz[0]*_pZ,  fwd_rev); 
  // 1D fft in the y-dim

  assert(_fft[0]!=NULL);
  assert(_fft[1]!=NULL);
  assert(_fft[2]!=NULL);
  //---------------------//

  (*_comm).globalBarrier();
//MPI_Barrier(MPI_COMM_WORLD);if(_myRank==0) printf("PRivateFFT::Initialize()... done\n");
  //MPI_Finalize(); exit(0);                                                                                                                                                                  
}


//template void PrivateFFT<double>::Plan( int ) ;
//template void PrivateFFT<float>::Plan( int ) ;
//

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::Plan( int fwd_rev )
{
#pragma execution_frequency(very_low)
  _ab = NULL;
  _ab = new MatrixIndices(_localNx[0]*_pX, _localNy[0]*_pY,_localNz[0]*_pZ,
                         _pX,    _pY,  _pZ,
			  _pStep[0], _pStep[1], _pStep[2],
			  _myPx, _myPy, _myPz); 
  assert(_ab!=NULL);
  
  int size = MAX(MAX(_pX,_pY),_pZ);
      
  _sendIndex = NULL;
  _recvIndex = NULL;
  
  _sendIndex = new DistributionOps**[DIM-1];
  _recvIndex = new DistributionOps**[DIM-1];

  assert(_sendIndex!=NULL);
  assert(_recvIndex!=NULL);
  
  for(int dir=0; dir<DIM-1; dir++)
    {
      // need to optimize the max_size by exact size
      _sendIndex[dir]  = new DistributionOps*[size];
      _recvIndex[dir]  = new DistributionOps*[size];
      
      assert(_sendIndex[dir]!=NULL);
      assert(_recvIndex[dir]!=NULL);

      for(int p=0; p<size; p++)
	{
	  _sendIndex[dir][p]  = new DistributionOps[size];
	  _recvIndex[dir][p]  = new DistributionOps[size];
	  
	  assert(_sendIndex[dir][p]!= NULL);
	  assert(_recvIndex[dir][p]!= NULL);
	}
    }
}

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::SetSendRecvIndices()
{
#pragma execution_frequency(very_low)
  BegLogLine(PKFXLOG_FFT_ALLOCATION) 
    << "before IndexOZ myRank=" << _myRank
    << EndLogLine ; 
  
  calcLocalIndexOZ(DIR_OZ);
 
  BegLogLine(PKFXLOG_FFT_ALLOCATION) 
    << "before IndexZY myRank=" << _myRank
    << EndLogLine ; 
  
  calcLocalIndexZY(DIR_ZY);
  BegLogLine(PKFXLOG_FFT_ALLOCATION) 
    << "before IndexYX myRank=" << _myRank
    << EndLogLine ; 

  calcLocalIndexYX(DIR_YX);
  BegLogLine(PKFXLOG_FFT_ALLOCATION) 
    << "after IndexYX myRank=" << _myRank
    << EndLogLine ; 

  // calculated how many FFTs myrank has to do for each phase
  _nFFTs[0] = (_localNx[1]*_localNy[1]);
  _nFFTs[1] = (_localNx[2]*_localNz[2]);
  _nFFTs[2] = (_localNz[3]*_localNy[3]);

 // MPI_Barrier(MPI_COMM_WORLD);  
}

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::SetAllToAllIndexInPlane(int aDir, int srcP, int trgP)
{
#pragma execution_frequency(very_low)
  int dir = aDir;
  int xMin, xMax, yMin, yMax, zMin, zMax;
  int maxDim = 3;
  int coords[2];
  int rank;
  int commSize;

  // this function returns the coords of the sub communicator.
  
  int sendCount = 0;
  int sendDisp  = 0;
  
  for(int trgP1=0; trgP1<srcP; trgP1++)    
    for(int trgP0=0; trgP0<trgP; trgP0++)        
      {
	coords[0] = trgP0;
	coords[1] = trgP1;
	
	(*_comm).getSubCommRank(dir%(DIM+1), coords, &rank);	 
	sendDisp += sendCount;
	_sendDisp[dir][rank]    = sendDisp;
	
	int globalCoord[DIM];
	
	//MPI_Comm_rank((*_comm)._cart_comm, &myRank);
	(*_comm).getCartRank(&_myRank); 
	//	MPI_Comm_rank(MPI_COMM_WORLD, &_myRank);
	//MPI_Cart_coords((*_comm)._cart_comm, myRank, DIM, globalCoord);
	(*_comm).getCartCoords(&globalCoord[0], &globalCoord[1], &globalCoord[2]);       
	int  myRank=_myRank;

	if(dir==0) {globalCoord[2]=coords[0]; }
	if(dir==1) {globalCoord[1]=coords[0], globalCoord[2]=coords[1];}
	if(dir==2) {globalCoord[0]=coords[0], globalCoord[1]=coords[1];}
	
	int globalRank;
       	(*_comm).getCartRank(&globalCoord[0], &globalRank);	
	_globalSendRank[dir][rank] = globalRank;	
	    
#ifdef MPI_ALLTOALL
	_sendDisp[dir][rank]    =  2*sendDisp;  
#endif

	if( _sendIndex[dir][trgP1][trgP0].isEmpty()!=true)
	  {	
	    xMin = _sendIndex[dir][trgP1][trgP0].min().x();
	    xMax = _sendIndex[dir][trgP1][trgP0].max().x();
	    
	    yMin = _sendIndex[dir][trgP1][trgP0].min().y();
	    yMax = _sendIndex[dir][trgP1][trgP0].max().y();
	    
	    zMin = _sendIndex[dir][trgP1][trgP0].min().z();
	    zMax = _sendIndex[dir][trgP1][trgP0].max().z();
	    sendCount = (xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
	  }
	else
	  {
	    sendCount =0;
	  } 
	     
	_sendCount[dir][rank]   = 2*sendCount;   
      }

  int recvCount = 0;
  int recvDisp  = 0;
      
  for(int srcP0=0; srcP0<trgP; srcP0++)        	
    for(int srcP1=0; srcP1<srcP; srcP1++)        
      {
	xMin = _recvIndex[dir][srcP1][srcP0].min().x();
	xMax = _recvIndex[dir][srcP1][srcP0].max().x();
	
	yMin = _recvIndex[dir][srcP1][srcP0].min().y();
	yMax = _recvIndex[dir][srcP1][srcP0].max().y();
	
	zMin = _recvIndex[dir][srcP1][srcP0].min().z();
	zMax = _recvIndex[dir][srcP1][srcP0].max().z();
	
	int rank;
       
	coords[0] = srcP0;
	coords[1] = srcP1;
       
	(*_comm).getSubCommRank(dir%(DIM+1), coords, &rank);
	
	// Get the global communicator 
	int globalCoord[DIM];
	int myRank=-1;
	//	MPI_Comm_rank((*_comm)._cart_comm, &myRank);
	(*_comm).getCartRank(&myRank);

	//	MPI_Cart_coords((*_comm)._cart_comm, myRank, DIM, globalCoord);
	(*_comm).getCartCoords(&globalCoord[0], &globalCoord[1], &globalCoord[2]);
	(*_comm).globalBarrier();
	if(dir==0) { globalCoord[2]=coords[0];}
	if(dir==1) { globalCoord[1]=coords[0], globalCoord[2]=coords[1];}
	if(dir==2) { globalCoord[0]=coords[0], globalCoord[1]=coords[1];}
	int globalRank;
   
	(*_comm).getCartRank(&globalCoord[0], &globalRank);	
	
	_globalRecvRank[dir][rank]=globalRank;	
	recvDisp               += recvCount;
	//where + both
	
	_recvDisp[dir][rank]   = recvDisp;
#ifdef   MPI_ALLTOALL
	_recvDisp[dir][rank]   = 2*recvDisp; 
#endif
	
	if(_recvIndex[dir][srcP1][srcP0].isEmpty()!=true)
	  {      
	    xMin = _recvIndex[dir][srcP1][srcP0].min().x();
	    xMax = _recvIndex[dir][srcP1][srcP0].max().x();
	    
	    yMin = _recvIndex[dir][srcP1][srcP0].min().y();
	    yMax = _recvIndex[dir][srcP1][srcP0].max().y();
	    
	    zMin = _recvIndex[dir][srcP1][srcP0].min().z();
	    zMax = _recvIndex[dir][srcP1][srcP0].max().z();
	    recvCount = (xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
	  }
	else
	  {
	    recvCount = 0;
	  }
	
	_recvCount[dir][rank]   = 2*recvCount;
      }

  
  return;
}

template <typename T, class Tcomplex> 
void  PrivateFFT<T,Tcomplex>::DoLocalMapping(Distribution3DPoint &pointMin,
				    Distribution3DPoint &pointMax, 
				    int x, int y, int z)
{
#pragma execution_frequency(very_low)
  // if not send to a specific node then your rectangle is empty w coords
  // (1,1,1) (0,0,0)
  
  if(pointMax.x()-pointMin.x()<0 ||
     pointMax.y()-pointMin.y()<0 ||
     pointMax.z()-pointMin.z()<0)
    {
      pointMin.setX(1); pointMin.setY(1); pointMin.setZ(1);
      pointMax.setX(0); pointMax.setY(0); pointMax.setZ(0);
      return;
    }

  int xMin = pointMin.x() - _myPx*x;
  int yMin = pointMin.y() - _myPy*y;
  int zMin = pointMin.z() - _myPz*z;
  
  int xMax = pointMax.x() - _myPx*x;
  int yMax = pointMax.y() - _myPy*y;
  int zMax = pointMax.z() - _myPz*z;
  
  pointMin.setX(xMin); pointMin.setY(yMin); pointMin.setZ(zMin);
  pointMax.setX(xMax); pointMax.setY(yMax); pointMax.setZ(zMax);
}


template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::calcLocalIndexOZ(int dir)
{
#pragma execution_frequency(very_low)
  assert(dir==DIR_OZ);
  //int sumRecvX = 0;  int sumRecvY = 0; int sumRecvZ = 0;
  
  for(int trgPz=0; trgPz<_pZ; trgPz++)
    {
      _sendIndex[dir][0][trgPz] = (*_ab).calcSendOtoZdataDistribution(_myPx, _myPy, _myPz, _myPx, _myPy, trgPz);
      
//MPI_Finalize(); exit(0);            
      if(_sendIndex[dir][0][trgPz].isEmpty()!=true) 
	{
	  Distribution3DPoint sendMin = _sendIndex[dir][0][trgPz].min();
	  Distribution3DPoint sendMax = _sendIndex[dir][0][trgPz].max();
	  
	  DoLocalMapping(sendMin, sendMax, _localNx[0], _localNy[0], _localNz[0]);
	  
	  _sendIndex[dir][0][trgPz].setMin(sendMin);
	  _sendIndex[dir][0][trgPz].setMax(sendMax);
	}

      
      _recvIndex[dir][0][trgPz] = (*_ab).calcRecvZfromOdataDistribution
	(_myPx, _myPy, _myPz, _myPx, _myPy, trgPz);  
      
      if(_recvIndex[dir][0][trgPz].isEmpty()!=true) 
	{
	  Distribution3DPoint recvMin = _recvIndex[dir][0][trgPz].min();
	  Distribution3DPoint recvMax = _recvIndex[dir][0][trgPz].max();  
	  
	  DoLocalMapping(recvMin, recvMax,_localNx[0], _localNy[0], 0);
	  
	  _recvIndex[dir][0][trgPz].setMin(recvMin);
	  _recvIndex[dir][0][trgPz].setMax(recvMax);
	  
	  //  sumRecvX += (recvMax.x()-recvMin.x())+1;  
	  //sumRecvY += (recvMax.y()-recvMin.y())+1;  
	  //sumRecvZ += (recvMax.z()-recvMin.z())+1;	  
	}
     
    }  
  
  Distribution3DPoint *tmpMin = &((*_ab)._Rz[_myPx][_myPy][_myPz].min());
  Distribution3DPoint *tmpMax = &((*_ab)._Rz[_myPx][_myPy][_myPz].max());
  
  if(((*_ab)._Rz[_myPx][_myPy][_myPz].isEmpty())==true)
    { 
      _localNx[dir+1]=0;
      _localNy[dir+1]=0;
      _localNz[dir+1]=0;      
    }
  else
    {
      _localNx[dir+1] = (tmpMax->x()-tmpMin->x()+1);
      _localNy[dir+1] = (tmpMax->y()-tmpMin->y()+1);
      _localNz[dir+1] = (tmpMax->z()-tmpMin->z()+1);
    }
  
  //  printf("localN(%d)[%d][%d][%d]=[%d %d %d]\n", dir+1,_myPx, _myPy,_myPz, _localNx[dir+1], _localNy[dir+1], _localNz[dir+1]);
  
}

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::calcLocalIndexZY(int dir)
{
#pragma execution_frequency(very_low)
  assert(dir==DIR_ZY);
  
  int sumRecvX = 0; int sumRecvY = 0; int sumRecvZ = 0;
  for(int trgPy=0; trgPy<_pY; trgPy++)
    {
      for(int trgPz=0; trgPz<_pZ; trgPz++)
	{
	  _sendIndex[dir][trgPz][trgPy] = 
	    (*_ab).calcSendZtoYdataDistribution(_myPx, _myPy, _myPz,
						_myPx, trgPy, trgPz);      
	  
	  if(_sendIndex[dir][trgPz][trgPy].isEmpty()!=true) 
	    {
	      Distribution3DPoint sendMin = _sendIndex[dir][trgPz][trgPy].min();
	      Distribution3DPoint sendMax = _sendIndex[dir][trgPz][trgPy].max();
	      
	      DoLocalMapping(sendMin, sendMax, _localNx[0], _localNy[0], 0);
	      
	      _sendIndex[dir][trgPz][trgPy].setMin(sendMin);
	      _sendIndex[dir][trgPz][trgPy].setMax(sendMax);
	    }
	 
	  
	  _recvIndex[dir][trgPz][trgPy] = (*_ab).calcRecvYfromZdataDistribution(_myPx, _myPy, _myPz, _myPx, trgPy, trgPz);
	  
	  if(_recvIndex[dir][trgPz][trgPy].isEmpty()!=true) 
	    {
	      Distribution3DPoint recvMin = _recvIndex[dir][trgPz][trgPy].min();
	      Distribution3DPoint recvMax = _recvIndex[dir][trgPz][trgPy].max();
	      
	      DoLocalMapping(recvMin, recvMax, _localNx[0], 0, _localNz[0]);
	      
	      _recvIndex[dir][trgPz][trgPy].setMin(recvMin);
	      _recvIndex[dir][trgPz][trgPy].setMax(recvMax);
	     
	    }
	 
	  
	}
    }
  
  Distribution3DPoint *tmpMin = &((*_ab)._Ry[_myPx][_myPy][_myPz].min());
  Distribution3DPoint *tmpMax = &((*_ab)._Ry[_myPx][_myPy][_myPz].max());

  if((*_ab)._Ry[_myPx][_myPy][_myPz].isEmpty()==true)
    {
      _localNx[dir+1]=0;
      _localNy[dir+1]=0;
      _localNz[dir+1]=0;
    }
  else
    
    {
      _localNx[dir+1] = (tmpMax->x()-tmpMin->x()+1);
      _localNy[dir+1] = (tmpMax->y()-tmpMin->y()+1);
      _localNz[dir+1] = (tmpMax->z()-tmpMin->z()+1);
      
      if(_localNx[dir+1]<=0) _localNx[dir+1]=1;
      if(_localNy[dir+1]<=0) _localNy[dir+1]=1;
      if(_localNz[dir+1]<=0) _localNz[dir+1]=1;
      
      assert(_localNx[dir+1]>0 &&
	     _localNy[dir+1]>0 &&
	     _localNz[dir+1]>0);
    }
  
  
}


template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::calcLocalIndexYX(int dir)
{
#pragma execution_frequency(very_low)
  assert(dir == DIR_YX);
  int sumRecvX = 0; int sumRecvY = 0; int sumRecvZ = 0;

  for(int trgPx=0; trgPx<_pX; trgPx++)
    for(int trgPy=0; trgPy<_pY; trgPy++)
      {
	_sendIndex[dir][trgPy][trgPx] = (*_ab).calcSendYtoXdataDistribution(_myPx, _myPy, _myPz, trgPx, trgPy, _myPz);

	if(_sendIndex[dir][trgPy][trgPx].isEmpty()!=true) 	
	  {
	    Distribution3DPoint sendMin = _sendIndex[dir][trgPy][trgPx].min();
	    Distribution3DPoint sendMax = _sendIndex[dir][trgPy][trgPx].max();
	
	    DoLocalMapping(sendMin, sendMax,_localNx[0], 0, _localNz[0]);
	    
	    _sendIndex[dir][trgPy][trgPx].setMin(sendMin);
	    _sendIndex[dir][trgPy][trgPx].setMax(sendMax);
	  }
	
	_recvIndex[dir][trgPy][trgPx] = (*_ab).calcRecvXfromYdataDistribution(_myPx, _myPy, _myPz, trgPx, trgPy, _myPz);
	
	if(_recvIndex[dir][trgPy][trgPx].isEmpty()!=true) 	
	  {
	    Distribution3DPoint recvMin = _recvIndex[dir][trgPy][trgPx].min();
	    Distribution3DPoint recvMax = _recvIndex[dir][trgPy][trgPx].max();
	    
	    DoLocalMapping(recvMin, recvMax, 0, _localNy[0], _localNz[0]);	
	    // reset the recvIndex with local values. 
	    _recvIndex[dir][trgPy][trgPx].setMin(recvMin);
	    _recvIndex[dir][trgPy][trgPx].setMax(recvMax);
	  }
      }
  
  Distribution3DPoint tmpMin = ((*_ab)._Rx[_myPx][_myPy][_myPz].min());
  Distribution3DPoint tmpMax = ((*_ab)._Rx[_myPx][_myPy][_myPz].max());
  
  if((*_ab)._Rx[_myPx][_myPy][_myPz].isEmpty()==true)
    {
      _localNx[dir+1] = 0;
      _localNx[dir+1] = 0;
      _localNx[dir+1] = 0;
    }
  else
    {
      _localNx[dir+1] = (tmpMax.x()-tmpMin.x()+1);
      _localNy[dir+1] = (tmpMax.y()-tmpMin.y()+1);
      _localNz[dir+1] = (tmpMax.z()-tmpMin.z()+1);
      
      if(_localNx[dir+1]<=0) _localNx[dir+1]=1;
      if(_localNy[dir+1]<=0) _localNy[dir+1]=1;
      if(_localNz[dir+1]<=0) _localNz[dir+1]=1;
      
      assert(_localNx[dir+1]>0 &&_localNy[dir+1]>0 &&_localNz[dir+1]>0);
    }
 
}


template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::AllocateMemory()
{ 
#pragma execution_frequency(very_low)
  _sendCount = new int*[DIM-1];
  _recvCount = new int*[DIM-1];
  _sendDisp  = new int*[DIM-1];
  _recvDisp  = new int*[DIM-1];
  
  assert(_sendCount!=NULL);
  assert(_recvCount!=NULL); 
  assert(_sendDisp!=NULL);
  assert(_recvDisp!=NULL);
 
  int commSize=-1;
  for(int dir=0; dir<DIM-1; dir++)
    {  
      (*_comm).getSubCommSize(dir, &commSize);
      
      assert((dir==0 && commSize ==_pZ)||
	     (dir==1 && commSize ==(_pZ*_pY))||
	     (dir==2 && commSize ==(_pX*_pY))||
	     (dir==3 && commSize ==_pX));
      
      _sendCount[dir] = new int[commSize];
      _recvCount[dir] = new int[commSize];
      assert(_sendCount[dir]!=NULL);
      assert(_recvCount[dir]!=NULL);
      
      _sendDisp[dir]  = new int[commSize];
      _recvDisp[dir]  = new int[commSize];
      assert(_sendDisp[dir]!=NULL);
      assert(_recvDisp[dir]!=NULL);     
    }

  int maxStep = MAX(_pStep[0], MAX(_pStep[1], _pStep[2]));

  BegLogLine(PKFXLOG_FFT_ALLOCATION)
    << "Allocation _pStep[0]=" << _pStep[0]
    << " _pStep[1]=" << _pStep[1]
    << " _pStep[2]=" << _pStep[2]
    << " _localNx[0]=" << _localNx[0]
    << " _localNx[1]=" << _localNx[1]
    << " _localNx[2]=" << _localNx[2]
    << EndLogLine ;
    
     int step = _pStep[0]*_pStep[1]*_pStep[2];
      totalLocal      = step*_localNx[0]*_localNy[0]*_localNz[0]+1;

      maxLocal        = maxStep*MAX(_localNx[0], MAX(_localNy[0], _localNz[0]));
      maxLocalSize    = (MAX(_pX,MAX(_pY, _pZ)))*totalLocal;
      maxLocalTmpSize = (MAX(_pX,MAX(_pY, _pZ)))*maxLocal;
  BegLogLine(PKFXLOG_FFT_ALLOCATION)
    << "FFT totalLocal=" << totalLocal
    << " maxLocal=" << maxLocal
    << " maxLocalSize=" << maxLocalSize
    << " maxLocalTmpSize=" << maxLocalTmpSize
    << EndLogLine ;
//#ifdef  ELEM_BY_ELEM    	
  _localDataIn   = new Tcomplex[totalLocal];
  _localDataOut  = new Tcomplex[totalLocal];
  assert(_localDataIn!=NULL);
  assert(_localDataOut!=NULL);
//#endif 

}

template <typename T, class Tcomplex>   
void PrivateFFT<T,Tcomplex>::Finalize()
{
#pragma execution_frequency(very_low)
  delete _ab;
  
//#ifdef  ELEM_BY_ELEM    	
  delete[] _localDataIn;
  delete[] _localDataOut;
  _localDataIn = NULL;
  _localDataOut = NULL;  
  assert(_localDataIn == NULL);
  assert( _localDataOut == NULL); 
//#endif

  int trgP = MAX(_pX, MAX(_pY,_pZ));      
  for(int dir=0; dir<DIM-1;dir++)
    {
      delete[] _sendCount[dir]; 
      delete[] _recvCount[dir];	
      delete[] _sendDisp[dir];	  
      delete[] _recvDisp[dir];

      _sendCount[dir] = NULL; 
      _recvCount[dir] = NULL; 	
      _sendDisp[dir] = NULL; 	  
      _recvDisp[dir] = NULL; 
	
      assert(_sendCount[dir]   == NULL); 
      assert(_recvCount[dir]   == NULL);
      assert(_recvDisp[dir]    == NULL);
      assert(_sendDisp[dir]    == NULL);
    }     	 
  
  delete[] _sendCount; 
  delete[] _recvDisp;
  delete[] _sendDisp;
  delete[] _recvCount;

  _sendCount = NULL; 
  _recvCount = NULL; 	
  _sendDisp  = NULL; 	  
  _recvDisp  = NULL; 
      
  assert( _sendCount   == NULL); 
  assert(_recvDisp     == NULL);
  assert(_sendDisp     == NULL);
  assert( _recvCount   == NULL);
       
  _comm = NULL;
  
  int size = MAX(MAX(_pX,_pY),_pZ);
  for(int dir=0; dir<DIM-1; dir++)
    for(int p=0; p<size; p++)
      {
	delete[] _sendIndex[dir][p];
	delete[] _recvIndex[dir][p];
	_sendIndex[dir][p]=NULL;
	_recvIndex[dir][p]=NULL;
	
	assert(_sendIndex[dir][p]== NULL);
	assert(_recvIndex[dir][p]== NULL);
      }
  
  for(int dir=0; dir<DIM-1; dir++)
    {
      delete[] _sendIndex[dir]; 
      delete[] _recvIndex[dir];
      _sendIndex[dir]=NULL;
      _recvIndex[dir]=NULL;
	   
      assert(_sendIndex[dir]==NULL);
      assert(_recvIndex[dir]==NULL);
    }      
            
  delete[] _sendIndex; 
  delete[] _recvIndex;
      
  _sendIndex=NULL;
  _recvIndex=NULL;
 
  assert(_sendIndex==NULL);
  assert(_recvIndex==NULL);

  int xSize = MAX(MAX(_localSizeA[0][0],_localSizeA[1][0]),_localSizeA[2][0]);
  int ySize = MAX(MAX(_localSizeA[0][1],_localSizeA[1][1]),_localSizeA[2][1]);
  int zSize = MAX(MAX(_localSizeA[0][2],_localSizeA[1][2]),_localSizeA[2][2]);
  
  for(int d=0; d<DIM-1; d++)
    {
      for(int i=0; i<xSize; i++)
	{    
	  for(int j=0; j<ySize; j++)
	    {
	      delete  _trgIndex[d][i][j];
	      _trgIndex[d][i][j]=NULL;
	      assert(_trgIndex[d][i][j]==NULL);
	    }
	  delete  _trgIndex[d][i];
	  _trgIndex[d][i]=NULL;	
	  assert(_trgIndex[d][i]==NULL);
	} 
      delete _trgIndex[d];
      _trgIndex[d]=NULL;
      assert(_trgIndex[d]==NULL);
    }
  
  delete _trgIndex;
  _trgIndex ==NULL;
  
  
  for(int i=0; i<DIM-1; i++)
    {
       delete _fft[i];
        _fft[i] = NULL;
        assert(_fft[i]==NULL);
     }
}

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::ReorderDoFFTs(int dir,
			    int* srcStride, 
			    int* trgStride,
			    int* localSize,
			    Tcomplex* localDataSrc,
			    Tcomplex* localDataTrg,
			    FFTOneD<T,Tcomplex> *fft) 
  {
#pragma execution_frequency(very_high)
	BegLogLine(PKFXLOG_FFT_SUMMARY)
	  << "ReorderDoFFTs dir=" << dir
	  << " localDataSrc=" << localDataSrc
	  << " localDataTrg=" << localDataTrg
	  << EndLogLine ;
#if defined(PK_BGL) && defined(PK_XLC)
    // Promise the compiler that the source and target are correctly aligned for parallell loads and stores
    __alignx(16, localDataSrc) ;
    __alignx(16, localDataTrg) ; 
#endif

    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];
    
    int srcStride0 = srcStride[0];
    int srcStride1 = srcStride[1];
    int srcStride2 = srcStride[2];
    static callcounter=0;
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    int counter = 0;
    
    int ***trgIndexTmp = _trgIndex[dir];
    
    TR_FFT_1D_Start.HitOE( PKTRACE_TUNE,
			   "TR_FFT_1D_Start", 
			   Platform::Thread::GetId(),
			   TR_FFT_1D_Start );
    
    double sumSqIn = 0.0 ;
    double sumSqOut = 0.0 ; 
    
    // Help the optimiser out a bit; it was generating 'special-case' code for when localNz was zero ...                              
    if ( localNx > 0 && localNy > 0 && localNz > 0 )
      {
        // The commented out version doesn't compile for complex types with initialisers
//	Tcomplex _tmpSrc[localNy*localNz] ;
//	Tcomplex _tmpTrg[localNy*localNz] ;
  T _tmpSrcT[localNy*localNz*2] ;
  Tcomplex* _tmpSrc = (Tcomplex*) _tmpSrcT ;
  T _tmpTrgT[localNy*localNz*2] ;
  Tcomplex* _tmpTrg = (Tcomplex*) _tmpTrgT ;
	for(int i=0; i<localNx; i++)
	  {
	    for(int j=0; j<localNy; j++)
	      { 
		for(int k=0; k<localNz; k++)
		  {
		    srcIndex = i*srcStride0+j*srcStride1+k*srcStride2;
		    
		    _tmpSrc[k+j*localNz] = localDataSrc[srcIndex] ;

		    // assert(srcIndex <_ _localNx[0]*_localNy[0]*_localNz[0]);
		    BegLogLine(PKFXLOG_FFT_DETAIL)
		      << "fft in[" << i
		      << "," << j
		      << "," << k
		      << "]=(" << creal(_tmpSrc[k+j*localNz])
		      << "," << cimag(_tmpSrc[k+j*localNz])
		      << ") "
		      << EndLogLine ;
		    
		  }
	      }
	    
	    assert(fft!=NULL);
	    (*fft).fftInternal(_tmpSrc, _tmpTrg);
	    
	    for(int j=0; j<localNy; j++)
	      {
		for(int k=0; k<localNz; k++)
		  {
		    trgIndex = _trgIndex[dir][i][j][k];		
	            localDataTrg[trgIndex] = _tmpTrg[k+j*localNz] ;
		    
		    BegLogLine(PKFXLOG_FFT_DETAIL)
		      << "fft out[" << i
		      << "," << j
		      << "," << k
		      << "]=(" << creal(_tmpTrg[k+j*localNz])
		      << "," << cimag(_tmpTrg[k+j*localNz])
		      << ") "
		      << EndLogLine ;
		  }
	      }
	  }
      }
    TR_FFT_1D_Finis.HitOE( PKTRACE_TUNE,
			   "TR_FFT_1D_Finis", 
			   Platform::Thread::GetId(),
			   TR_FFT_1D_Finis );
    
    
    BegLogLine(PKFXLOG_FFT_SUMMARY)
      << "ReorderDoFFTs dir=" << dir
      << " sumSqIn=" << sumSqIn
      << " sumSqOut=" << sumSqOut
      << EndLogLine ;
    
  }
  

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,int *localSize,
			      Tcomplex* localDataSrc, Tcomplex* localDataTrg, FFTOneD<T,Tcomplex>* fft)
  {
#pragma execution_frequency(very_high)
	BegLogLine(PKFXLOG_FFT_SUMMARY)
	  << "ReorderDoFFTs_R this=" << this
	  << " dir=" << dir
	  << " localDataSrc=" << localDataSrc
	  << " localDataTrg=" << localDataTrg
	  << EndLogLine ;
#if defined(PK_BGL) && defined(PK_XLC)
    // Promise the compiler that the source and target are correctly aligned for parallel loads and stores
    __alignx(16, localDataSrc) ;
    __alignx(16, localDataTrg) ; 
#endif
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];  
    
    int trgStride0 = trgStride[0];
    int trgStride1 = trgStride[1];
    int trgStride2 = trgStride[2];
    
    int nFFTs = _nFFTs[dir];
    
    int trgIndex=0, srcIndex=0;
    int counter = 0;
    
    int ***srcIndexD =  _trgIndex[dir];  
	BegLogLine(PKFXLOG_FFT_SUMMARY)
	  << "ReorderDoFFTs_R dir=" << dir
	  << " localDataSrc=" << localDataSrc
	  << " localDataTrg=" << localDataTrg
	  << " _trgIndex=" << _trgIndex
	  << " srcIndexD=" << srcIndexD
	  << " maxLocalTmpSize=" << maxLocalTmpSize
	  << EndLogLine ;
    TR_FFT_1D_R_Start.HitOE( PKTRACE_TUNE,
                              "TR_FFT_1D_R_Start", 
                              Platform::Thread::GetId(),
                              TR_FFT_1D_R_Start );
	double sumSqIn = 0.0 ;
	double sumSqOut = 0.0 ; 
// Help the optimiser out a bit; it was generating 'special-case' code for when localNz was zero ...                              
    if ( localNx > 0 && localNy > 0 && localNz > 0 )
    {
//	    Tcomplex _tmpSrc[localNy*localNz] ;
//	    Tcomplex _tmpTrg[localNy*localNz] ;
      T _tmpSrcT[localNy*localNz*2] ;
      T _tmpTrgT[localNy*localNz*2] ;
      Tcomplex* _tmpSrc=(Tcomplex*) _tmpSrcT ;
      Tcomplex* _tmpTrg=(Tcomplex*) _tmpTrgT ;
	    for(int i=0; i<localNx; i++)
	      {
	      	BegLogLine(PKFXLOG_FFT_DETAIL)
	      	  << "srcIndexD[i]=" << srcIndexD[i] 
	      	  << EndLogLine ;
			for(int j=0; j<localNy; j++)
			  { 
		      	BegLogLine(PKFXLOG_FFT_DETAIL)
		      	  << "srcIndexD[i][j]=" << srcIndexD[i][j]
		      	  << EndLogLine ;
			    for(int k=0; k<localNz; k++)
			      {
			      	BegLogLine(PKFXLOG_FFT_DETAIL)
			      	  << "srcIndexD[i][j][k]=" << srcIndexD[i][j][k]
			      	  << EndLogLine ;
					
		  		      srcIndex = srcIndexD[i][j][k];
					//	assert(srcIndex == _trgIndex[dir][i][j][k]);
					//assert(srcIndex == _trgIndex[dir][i][j][k]);
					//assert(k+j*localNz < localNz*localNy);
					
			//	      double re = localDataSrc[srcIndex].re ;
			//	      double im = localDataSrc[srcIndex].im ;
			//	      sumSqIn += ( re*re+im*im ) ;
			//		_tmpSrc[k+j*localNz].re = localDataSrc[srcIndex].re;
			//		_tmpSrc[k+j*localNz].im = localDataSrc[srcIndex].im;	      
			
			BegLogLine(PKFXLOG_FFT_DETAIL)
			  << "fft_R_index index=" << k+j*localNz
			  << " maxLocalTmpSize=" << maxLocalTmpSize
			  << EndLogLine ;
			        assert(k+j*localNz < maxLocalTmpSize) ;
			        _tmpSrc[k+j*localNz] = localDataSrc[srcIndex] ;
	      BegLogLine(PKFXLOG_FFT_DETAIL)
	        << "fft_R in[" << i
	        << "," << j
	        << "," << k
	        << "]=(" << creal(_tmpSrc[k+j*localNz])
	        << "," << cimag(_tmpSrc[k+j*localNz])
	        << ") _tmpSrc+k+j*localNz=0x" << (void *)(_tmpSrc+k+j*localNz)
	        << EndLogLine ;
			      }
			}
		
	                               
			(*fft).fftInternal(_tmpSrc, _tmpTrg);
			
			for(int j=0; j<localNy; j++)
			  {
			    for(int k=0; k<localNz; k++)
			      {
			      	
				trgIndex = i*trgStride0+j*trgStride1+k*trgStride2;
				
				BegLogLine(PKFXLOG_FFT_DETAIL)
				  << "fft_R_trgindex trgIndex=" << trgIndex
				  << EndLogLine ;
				//assert(trgIndex < _localNx[0]*_localNy[0]*_localNz[0]);
				//assert(k+j*localNz < localNz*localNy);
				
		//		double re = _tmpTrg[k+j*localNz].re ;
		//		double im = _tmpTrg[k+j*localNz].im ;
		//		sumSqOut += ( re*re+im*im ) ; 
		//		localDataTrg[trgIndex].re = _tmpTrg[k+j*localNz].re;
		//		localDataTrg[trgIndex].im = _tmpTrg[k+j*localNz].im;	      
		
	      BegLogLine(PKFXLOG_FFT_DETAIL)
	        << "fft_R out[" << i
	        << "," << j
	        << "," << k
	        << "]=(" << creal(_tmpTrg[k+j*localNz])
	        << "," << cimag(_tmpTrg[k+j*localNz])
	        << ") localDataTrg+trgIndex=0x" << (void *)(localDataTrg+trgIndex) 
	        << EndLogLine ;
		        localDataTrg[trgIndex] = _tmpTrg[k+j*localNz] ;
			      } 
		  } 
      }
    }
      TR_FFT_1D_R_Finis.HitOE( PKTRACE_TUNE,
                              "TR_FFT_1D_R_Finis", 
                              Platform::Thread::GetId(),
                              TR_FFT_1D_R_Finis );
                              
	BegLogLine(PKFXLOG_FFT_SUMMARY)
	  << "ReorderDoFFTs_R dir=" << dir
	  << " sumSqIn=" << sumSqIn
	  << " sumSqOut=" << sumSqOut
	  << EndLogLine ;
  }


template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::DoFwdFFT(Tcomplex* bglfftIn, Tcomplex* bglfftOut)
  {
    //IsValidChecksum();
    // If we have 'external' buffers, leave the 'allocated' buffers alone 
    // declare it but not allocate it until now.This way we can make call DoFwdFFT from here.. 

    TR_BGL3DFFT_Start.HitOE(  PKTRACE_BGL3DFFT_BENCH ,
			      "TR_BGL3DFFT_Start",
			      Platform::Thread::GetId(),
			      TR_BGL3DFFT_Start);

    Tcomplex* _localDataIn=bglfftIn;
    Tcomplex* _localDataOut=bglfftOut;
    
//    Tcomplex _localDataTmp1[totalLocal];
//    Tcomplex _localDataTmp2[totalLocal];
    T _localDataTmp1T[totalLocal*2];
    T _localDataTmp2T[totalLocal*2];
    Tcomplex* _localDataTmp1 = (Tcomplex*) _localDataTmp1T ;
    Tcomplex* _localDataTmp2 = (Tcomplex*) _localDataTmp2T ;
  
    int dir=0;
#ifdef BENCH_A2A
    long long TimeStartRed0 = rts_get_timebase();
    _comm->globalBarrier();
#endif
      

    TR_BGL3DFFT_FW_A2A_Z_Start.HitOE(  PKTRACE_BGL3DFFT_BENCH ,
				       "TR_BGL3DFFT_FW_A2A_Z_Start",
				       Platform::Thread::GetId(),
				       TR_BGL3DFFT_FW_A2A_Z_Start);
    Redistribute(dir,
		 _sendCount[dir],
		 _sendDisp[dir], 
		 _localDataIn,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataTmp2 );
    
    TR_BGL3DFFT_FW_A2A_Z_Finis.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				      "TR_BGL3DFFT_FW_A2A_Z_Finis",
				      Platform::Thread::GetId(),
				      TR_BGL3DFFT_FW_A2A_Z_Finis );
#ifdef BENCH_A2A
    _comm->globalBarrier();
    long long TimeFinisRed0 = rts_get_timebase();
#endif
    
#ifdef DEBUG_FFT
    IsValidChecksum();
#endif
    
    TR_BGL3DFFT_FW_COMP_Z_Start.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				       "TR_BGL3DFFT_FW_COMP_Z_Start",
				       Platform::Thread::GetId(),
				       TR_BGL3DFFT_FW_COMP_Z_Start);
    ReorderDoFFTs(dir,
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataTmp2,
		  _localDataTmp1, 
		  _fft[dir]);

    TR_BGL3DFFT_FW_COMP_Z_Finis.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				       "TR_BGL3DFFT_FW_COMP_Z_Finis",
				       Platform::Thread::GetId(),
				       TR_BGL3DFFT_FW_COMP_Z_Finis );
    
#ifdef DEBUG_FFT
    IsValidChecksum();
#endif
    
    dir=1;
#ifdef BENCH_A2A
    long long TimeStartRed1 = rts_get_timebase();
    _comm->globalBarrier();
#endif
    
    TR_BGL3DFFT_FW_A2A_Y_Start.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				      "TR_BGL3DFFT_FW_A2A_Y_Start",
				      Platform::Thread::GetId(),
				      TR_BGL3DFFT_FW_A2A_Y_Start);
    Redistribute(dir,
		 _sendCount[dir], 
		 _sendDisp[dir],
		 _localDataTmp1,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataTmp2);
    
    TR_BGL3DFFT_FW_A2A_Y_Finis.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				      "TR_BGL3DFFT_FW_A2A_Y_Finis",
				      Platform::Thread::GetId(),
				      TR_BGL3DFFT_FW_A2A_Y_Finis);
#ifdef BENCH_A2A
    _comm->globalBarrier();    
    long long TimeFinisRed1 = rts_get_timebase();
#endif
    
#ifdef DEBUG_FFT
    IsValidChecksum(); 
#endif
    
    TR_BGL3DFFT_FW_COMP_Y_Start.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				       "TR_BGL3DFFT_FW_COMP_Y_Start",
				       Platform::Thread::GetId(),
				       TR_BGL3DFFT_FW_COMP_Y_Start);
    ReorderDoFFTs(dir, 
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataTmp2,
		  _localDataTmp1, 
		  _fft[dir]);
    
    TR_BGL3DFFT_FW_COMP_Y_Finis.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				       "TR_BGL3DFFT_FW_COMP_Y_Finis",
				       Platform::Thread::GetId(),
				       TR_BGL3DFFT_FW_COMP_Y_Finis);

#ifdef DEBUG_FFT
    IsValidChecksum();
#endif
    
    dir=2;
#ifdef BENCH_A2A
    long long TimeStartRed2 = rts_get_timebase();
    _comm->globalBarrier();
#endif

    TR_BGL3DFFT_FW_A2A_X_Start.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				      "TR_BGL3DFFT_FW_A2A_X_Start",
				      Platform::Thread::GetId(),
				      TR_BGL3DFFT_FW_A2A_X_Start);
    Redistribute(dir,
		 _sendCount[dir],
		 _sendDisp[dir],
		 _localDataTmp1,
		 _recvCount[dir], 
		 _recvDisp[dir],
		 _localDataTmp2);
    
    TR_BGL3DFFT_FW_A2A_X_Finis.HitOE( PKTRACE_BGL3DFFT_BENCH ,
				      "TR_BGL3DFFT_FW_A2A_X_Finis",
				      Platform::Thread::GetId(),
				      TR_BGL3DFFT_FW_A2A_X_Finis);

    TR_BGL3DFFT_Finis.HitOE(  PKTRACE_BGL3DFFT_BENCH ,
			      "TR_BGL3DFFT_Finis",
			      Platform::Thread::GetId(),
			      TR_BGL3DFFT_Finis);

#ifdef BENCH_A2A
 _comm->globalBarrier();
 long long TimeFinisRed2 = rts_get_timebase();
#endif
    
#ifdef DEBUG_FFT
    IsValidChecksum();
#endif
    
    TR_BGL3DFFT_FW_COMP_X_Start.HitOE( PKTRACE_TUNE,
				       "TR_BGL3DFFT_FW_COMP_X_Start",
				       Platform::Thread::GetId(),
				       TR_BGL3DFFT_FW_COMP_X_Start);
    ReorderDoFFTs(dir,
		  _strideSrcA[dir],
		  _strideTrgA[dir], 
		  _localSizeA[dir],
		  _localDataTmp2,
		  _localDataOut,
		  _fft[dir]);
    
    TR_BGL3DFFT_FW_COMP_X_Finis.HitOE( PKTRACE_TUNE,
				       "TR_BGL3DFFT_FW_COMP_X_Finis",
				       Platform::Thread::GetId(),
				       TR_BGL3DFFT_FW_COMP_X_Finis);

#ifdef DEBUG_FFT
    IsValidChecksum();
#endif

#ifdef BENCH_A2A
    
    _comm->globalBarrier();
    zeroDataDistributions
    static double totalElapsedTimeX = 0.0;
    static double totalElapsedTimeY = 0.0;
    static double totalElapsedTimeZ = 0.0;
    static int iters = 1;
    
    if(_myRank==0)
      {
	double elapsedRed0 = 1.0*(TimeFinisRed0-TimeStartRed0) / (700.0*1000.0);
	double elapsedRed1 = 1.0*(TimeFinisRed1-TimeStartRed1) / (700.0*1000.0);
	double elapsedRed2 = 1.0*(TimeFinisRed2-TimeStartRed2) / (700.0*1000.0);
	
	totalElapsedTimeZ += elapsedRed0;
	totalElapsedTimeY += elapsedRed1;
	totalElapsedTimeX += elapsedRed2;
	if(iters==1 || iters%10==0)
	printf("Elapsed time on  %dx%dx%d for %dx%dx%d FFT complex Redistribution[0] = %lf ms \n Redistribution[1] = %lf ms\n  Redistribution[2] = %lf ms \n",
	       _pX, _pY, _pZ,
	       _localNx[0]*_pX, _localNy[0]*_pY, _localNz[0]*_pZ,
	       totalElapsedTimeZ/iters,  totalElapsedTimeY/iters, totalElapsedTimeX/iters);
      }
   iters++; 
#endif
}
  

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::DoFwdFFT()
  {
    //    IsValidChecksum();
    
//  Tcomplex _localDataTmp1  [totalLocal];
//  Tcomplex _localDataTmp2  [totalLocal];
  T _localDataTmp1T  [totalLocal*2];
  T _localDataTmp2T  [totalLocal*2];
  Tcomplex* _localDataTmp1 = (Tcomplex*) _localDataTmp1T ;
  Tcomplex* _localDataTmp2 = (Tcomplex*) _localDataTmp2T ;
    int dir=0;
    Redistribute(dir,
		 _sendCount[dir],
		 _sendDisp[dir], 
		 _localDataIn,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataTmp2);
    
    // IsValidChecksum();
    
    ReorderDoFFTs(dir,
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataTmp2,
		  _localDataTmp1, 
		  _fft[dir]);
		

    //MPI_Finalize();exit(0);
    dir=1;
    //IsValidChecksum();
    
    Redistribute(dir,
		 _sendCount[dir], 
		 _sendDisp[dir],
		 _localDataTmp1,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataTmp2 );
    
    // IsValidChecksum();
   ReorderDoFFTs(dir, 
		 _strideSrcA[dir],
		 _strideTrgA[dir],
		 _localSizeA[dir],
		 _localDataTmp2,
		 _localDataTmp1, _fft[dir]);
   dir=2;
   
   //IsValidChecksum();
   Redistribute(dir,
		_sendCount[dir],
		_sendDisp[dir],
		_localDataTmp1,
		_recvCount[dir], 
		_recvDisp[dir],
		_localDataTmp2 );
   
   //IsValidChecksum();
   
   ReorderDoFFTs(dir, _strideSrcA[dir],
		 _strideTrgA[dir], 
		 _localSizeA[dir],
		 _localDataTmp2,
		 _localDataOut, _fft[dir]);
   //IsValidChecksum();
  }
  
  
template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::DoRevFFT(Tcomplex* bglfftIn,Tcomplex*  bglfftOut)
  {

    //IsValidChecksum();
    // If we have 'external' buffers, leave the 'allocated' buffers alone    
    Tcomplex* _localDataIn  = bglfftIn;
    Tcomplex* _localDataOut = bglfftOut;
    
    //  Tcomplex _localDataTmp1  [totalLocal];
    //  Tcomplex _localDataTmp2  [totalLocal];
      T _localDataTmp1T  [totalLocal*2];
      T _localDataTmp2T  [totalLocal*2];
      Tcomplex* _localDataTmp1 = (Tcomplex*) _localDataTmp1T ;
      Tcomplex* _localDataTmp2 = (Tcomplex*) _localDataTmp2T ;
    BegLogLine(PKFXLOG_FFT_DETAIL)
      << "DoRevFFT totalLocal=" << totalLocal
      << " _localDataTmp1=0x" << (void *)_localDataTmp1
      << " _localDataTmp2=0x" << (void *)_localDataTmp2
      << " totalLocal=" << totalLocal
      << EndLogLine ;
    int  dir=2;      
    ReorderDoFFTs_R(dir, _strideTrgA[dir], 
		    _strideSrcA[dir],
		    _localSizeA[dir],
		    _localDataIn,
		    _localDataTmp2, _fft[dir]);
    
    //IsValidChecksum();
   Redistribute(dir,
		_recvCount[dir], 
		 _recvDisp[dir],
		_localDataTmp2,		   
		_sendCount[dir],
		 _sendDisp[dir],
		_localDataTmp1 );
   
   // IsValidChecksum();
    dir=1;  
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp1, _localDataTmp2, _fft[dir]);
    
    //IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataTmp2,
		 _sendCount[dir], _sendDisp[dir], _localDataTmp1 );
    
    //IsValidChecksum();
    dir=0;   
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp1, _localDataTmp2, _fft[dir]);
   
    //IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataTmp2,
		 _sendCount[dir], _sendDisp[dir], _localDataOut );
  }
  

template <typename T, class Tcomplex> 
void PrivateFFT<T,Tcomplex>::DoRevFFT()
  {
    // IsValidChecksum();
    //  Tcomplex _localDataTmp1  [totalLocal];
    //  Tcomplex _localDataTmp2  [totalLocal];
      T _localDataTmp1T  [totalLocal*2];
      T _localDataTmp2T  [totalLocal*2];
      Tcomplex* _localDataTmp1 = (Tcomplex*) _localDataTmp1T ;
      Tcomplex* _localDataTmp2 = (Tcomplex*) _localDataTmp2T ;
    int  dir=2;      
    BegLogLine(PKFXLOG_FFT_DETAIL)
      << "DoRevFFT totalLocal=" << totalLocal
      << " _localDataTmp1=0x" << (void *)_localDataTmp1
      << " _localDataTmp2=0x" << (void *)_localDataTmp2
      << EndLogLine ;
    
    ReorderDoFFTs_R(dir, _strideTrgA[dir], 
		    _strideSrcA[dir],
		    _localSizeA[dir],
		    _localDataIn,
		    _localDataTmp2, _fft[dir]);
    
    // IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], 
		 _recvDisp[dir],
		 _localDataTmp2,		   
		 _sendCount[dir],
		 _sendDisp[dir],
		 _localDataTmp1 );
   
    // IsValidChecksum();
    dir=1;  
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp1, _localDataTmp2, _fft[dir]);
    
    // IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataTmp2,
		 _sendCount[dir], _sendDisp[dir], _localDataTmp1 );
    
    // IsValidChecksum();
    dir=0;   
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp1, _localDataTmp2, _fft[dir]);
    
    // IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataTmp2,
		 _sendCount[dir], _sendDisp[dir], _localDataOut );
  }
  
