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
   Last Modified: 03/10/2005  
   By     : ME
*/

#ifndef PRIVATE_FFT_HPP
#define PRIVATE_FFT_HPP

#if !defined(PKFXLOG_FFT_ALLOCATION)
#define PKFXLOG_FFT_ALLOCATION (0) 
#endif

#if !defined(PKFXLOG_FFT_VALUES)
#define PKFXLOG_FFT_VALUES (0)
#endif

	// #define TMP_IN_FREESTORE

//#include <string.h>
//#include <stdlib.h>
//#include "mpi.h"
#include "comm3DLayer_MPI.hpp"
#include "fft_one_dim_MPI.hpp"
#include "arrayBounds_MPI.hpp"

#ifndef MPI_ALLTOALL
#define MPI_ALLTOALL 1
#endif

#define FFTP_FORWARD 1
#define FFTP_REVERSE 0

template <class T>  
class PrivateFFT
{
public:
  enum{DIR_OZ, DIR_ZY, DIR_YX};
  enum{DIM=4};
  FFTOneD<T> *_fft[DIM-1];

  Comm3DLayer<T> *_comm;
  
  // data related   
  int _strideXYZ[DIM-1];
  
  int _strideR0, _strideR1, _strideR2;
 
  int _nFFTs[DIM-1];
  complexTemplate<T> *_input;
  complexTemplate<T> *_output;
  
  int _localNx[4];
  int _localNy[4];
  int _localNz[4];
  int _totalLocal ;

  int _localPx[2*DIM], _localPy[2*DIM],_localPz[2*DIM];
  
  int _strideTrgA[DIM][DIM-1];
  int _strideSrcA[DIM][DIM-1];
  int _localSizeA[DIM][DIM-1];
  
  int _totalLocalSize[DIM];

  int**** _trgIndex;
#if defined(TMP_IN_FREESTORE)  
  complexTemplate<T>* _tmpSrc;
  complexTemplate<T>* _tmpTrg;
#endif
  int** _sendCount;
  int** _recvCount;
  
  int** _sendDisp;
  int** _recvDisp;
  
  Rectangle*** _sendIndex;
  Rectangle*** _recvIndex;
  int _globalSendRank[3][1024];
  int _globalRecvRank[3][1024];

  MatrixIndices *_ab;
  int _pX;  int _pY;  int _pZ;
  int _myPx;  int _myPy;  int _myPz,_myRank, _globalID;

  complexTemplate<T> *_localDataIn;
#if defined(FFT_LOCAL_IN_FREESTORE)  
  complexTemplate<T> *_localDataTmp1;
  complexTemplate<T> *_localDataTmp2;
#endif  
  complexTemplate<T> *_localDataOut;
    
  complexTemplate<T> *_localDataBase ;
  
  // private functions
  
  inline void Initialize( int fwd_rev );
  inline void Plan( int fwd_rev );
  
  inline void AllocateMemory();
  inline void DoLocalMapping(Point3D &pointMin, Point3D &pointMax, int x, int y, int z);
  
  inline void SetSendRecvIndices();
  inline void SetAllToAllIndexInPlane(int aDir, int srcP, int trgP);
  
  inline void calcLocalIndexOZ(int dir);
  inline void calcLocalIndexZY(int dir);
  inline void calcLocalIndexYX(int dir);
   
  
  inline void Redistribute(int dir, 
                           int Fwd,
			   complexTemplate<T> *localDataSrc,
			   complexTemplate<T> *localDataTrg )
  {    
//     (*_comm).AllToAllV((void*)localDataSrc, sendCount, sendDisp,
// 	      (void*)localDataTrg, recvCount, recvDisp, dir);

  BegLogLine(PKFXLOG_FFT_ALLOCATE)
    << "Redistribute dir=" << dir
    << " Fwd=" << Fwd
    << " localDataSrc=" << localDataSrc
    << " localDataTrg=" << localDataTrg
    << EndLogLine ;
    
  (*_comm).AllToAllV_Execute( (void*) localDataSrc,
                              (void*) localDataTrg,
                              dir, Fwd );
  }
  
    
  inline  void ReorderDoFFTs(int dir,
			     int* srcStride, 
			     int* trgStride,
			     int* localSize,
			     complexTemplate<T>* localDataSrc,
			     complexTemplate<T>* localDataTrg,
			     FFTOneD<T> *fft)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];
    
    int srcStride0 = srcStride[0];
    int srcStride1 = srcStride[1];
    int srcStride2 = srcStride[2];
    
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    
    int counter = 0;
    
    int ***trgIndexTmp = _trgIndex[dir];
#if !defined(TMP_IN_FREESTORE)
    complexTemplate<T> _tmpSrc[localNy*localNz ] ;
    complexTemplate<T> _tmpTrg[localNy*localNz ] ;
#endif  
  BegLogLine(PKFXLOG_FFT_ALLOCATE)
   << "ReorderDoFFTs localNy=" << localNy
   << " localNz=" << localNz
   << " _tmpSrc=" << _tmpSrc
   << " _tmpTrg=" << _tmpTrg
   << EndLogLine ;
    
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		srcIndex = i*srcStride0+j*srcStride1+k*srcStride2;
	      
		_tmpSrc[k+j*localNz].re=localDataSrc[srcIndex].re;
		_tmpSrc[k+j*localNz].im=localDataSrc[srcIndex].im;
		assert(srcIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
		BegLogLine(PKFXLOG_FFT_VALUES)
		  << " FFT Src[" << i
		  << "," << j
		  << "," << k 
		  << "]=(" << _tmpSrc[k+j*localNz].re
		  << "," << _tmpSrc[k+j*localNz].im
		  << ")"
		  << EndLogLine ;
	      }
	  }
	assert(fft!=NULL);
	(*fft).fftInternal(_tmpSrc, _tmpTrg);
	for(int j=0; j<localNy; j++)
	  {
	  	int trgIndices[localNz] ;
	  	for (int k0=0;k0<localNz;k0+=1)
	  	{
	  		trgIndices[k0]=_trgIndex[dir][i][j][k0];
	  	}
	    for(int k=0; k<localNz; k++)
	      {
//		trgIndex = _trgIndex[dir][i][j][k];
        trgIndex = trgIndices[k] ;
		
		assert(trgIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
		assert(k+j*localNz  < localNz*localNy);
		
		BegLogLine(PKFXLOG_FFT_VALUES)
		  << " FFT Trg[" << i
		  << "," << j
		  << "," << k 
		  << "]=(" << _tmpTrg[k+j*localNz].re
		  << "," << _tmpTrg[k+j*localNz].im
		  << ")"
		  << EndLogLine ;
		localDataTrg[trgIndex].re = _tmpTrg[k+j*localNz].re;
		localDataTrg[trgIndex].im = _tmpTrg[k+j*localNz].im;
	      }
	  }
      }
  }
  
  
  inline void ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,
			      int *localSize,
			      complexTemplate<T>* localDataSrc, complexTemplate<T>* localDataTrg, FFTOneD<T> *fft)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];  
    
    int trgStride0 = trgStride[0];
    int trgStride1 = trgStride[1];
    int trgStride2 = trgStride[2];
    
    int nFFTs = _nFFTs[dir];
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    int counter = 0;
    
    int ***srcIndexD =  _trgIndex[dir];  
#if !defined(TMP_IN_FREESTORE)
    complexTemplate<T> _tmpSrc[localNy*localNz ] ;
    complexTemplate<T> _tmpTrg[localNy*localNz ] ;
#endif  
  BegLogLine(PKFXLOG_FFT_ALLOCATE)
   << "ReorderDoFFTs_R localNy=" << localNy
   << " localNz=" << localNz
   << " _tmpSrc=" << _tmpSrc
   << " _tmpTrg=" << _tmpTrg
   << EndLogLine ;
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	  	int srcIndices[localNz] ;
	  	for (int k0=0;k0<localNz;k0+=1)
	  	{
	  		srcIndices[k0]=srcIndexD[i][j][k0];
	  	}
	    for(int k=0; k<localNz; k++)
	      {
//  		srcIndex = srcIndexD[i][j][k];
        srcIndex = srcIndices[k] ;
		
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(k+j*localNz < localNz*localNy);
		
		_tmpSrc[k+j*localNz].re = localDataSrc[srcIndex].re;
		_tmpSrc[k+j*localNz].im = localDataSrc[srcIndex].im;	      
		BegLogLine(PKFXLOG_FFT_VALUES)
		  << " FFT Src[" << i
		  << "," << j
		  << "," << k 
		  << "]=(" << _tmpSrc[k+j*localNz].re
		  << "," << _tmpSrc[k+j*localNz].im
		  << ")"
		  << EndLogLine ;
	      }
		
	      
	}
	
	(*fft).fftInternal(_tmpSrc, _tmpTrg);
	
	for(int j=0; j<localNy; j++)
	  {
	    for(int k=0; k<localNz; k++)
	      {
		trgIndex = i*trgStride0+j*trgStride1+k*trgStride2;
		
		assert(trgIndex < _localNx[0]*_localNy[0]*_localNz[0]);
		assert(k+j*localNz < localNz*localNy);
		
		BegLogLine(PKFXLOG_FFT_VALUES)
		  << " FFT Trg[" << i
		  << "," << j
		  << "," << k 
		  << "]=(" << _tmpTrg[k+j*localNz].re
		  << "," << _tmpTrg[k+j*localNz].im
		  << ")"
		  << EndLogLine ;
		localDataTrg[trgIndex].re = _tmpTrg[k+j*localNz].re;
		localDataTrg[trgIndex].im = _tmpTrg[k+j*localNz].im;	      
	      } 
	  } 
      }
  }
  
  inline void DoFwdFFT()
  {
    int dir=0;
#if !defined(FFT_LOCAL_IN_FREESTORE)  
    complexTemplate<T> _localDataTmp1[_totalLocal] ;
    complexTemplate<T> _localDataTmp2[_totalLocal] ;
#endif    
    Redistribute(dir,
                 FFTP_FORWARD,
		 _localDataIn,
		 _localDataTmp2 );
    
    ReorderDoFFTs(dir,
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataTmp2,
		  _localDataTmp1, 
		  _fft[dir]);
    dir=1;
    
    Redistribute(dir,
                 FFTP_FORWARD,
		 _localDataTmp1,
		 _localDataOut );

    ReorderDoFFTs(dir, 
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataOut,
		  _localDataTmp2, _fft[dir]);
    dir=2;
    
    Redistribute(dir,
                 FFTP_FORWARD,
		 _localDataTmp2,
		 _localDataTmp1);
    
    ReorderDoFFTs(dir, _strideSrcA[dir],
		  _strideTrgA[dir], 
		  _localSizeA[dir],
		  _localDataTmp1,
		  _localDataOut, _fft[dir]);
  }

  
  inline  void DoRevFFT()
  {

    int  dir=2;       
#if !defined(FFT_LOCAL_IN_FREESTORE)  
    complexTemplate<T> _localDataTmp1[_totalLocal] ;
    complexTemplate<T> _localDataTmp2[_totalLocal] ;
#endif    
    ReorderDoFFTs_R(dir, _strideTrgA[dir], 
		    _strideSrcA[dir],
		    _localSizeA[dir],
		    _localDataIn,
		    _localDataTmp2, _fft[dir]);
    
    Redistribute(dir,
                 FFTP_REVERSE,
		 _localDataTmp2,		   
		 _localDataTmp1);
    
    dir=1;  
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp1, _localDataOut, _fft[dir]);
    
    Redistribute(dir,
                 FFTP_REVERSE,
		 _localDataOut,
		 _localDataTmp2);
      
    dir=0;   
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp2, _localDataTmp1, _fft[dir]);
    Redistribute(dir,
                 FFTP_REVERSE,
		 _localDataTmp1,
		 _localDataOut);
  }
  
  inline void Finalize();
};



#define MY_MAX(i, j) ( (i>j)?i:j )
template< class T >
void PrivateFFT<T>::Initialize( int fwd_rev )
{
  int dimSz[PrivateFFT::DIM-1] = {_pX, _pY, _pZ};
 
  int periods[PrivateFFT::DIM-1]  = {0, 0, 0};  // logical array, specifying whether the
  
  assert(_localNx[0]*_localNy[0] >= _pZ);
  assert(_localNx[0]*_localNz[0] >= _pY);
  assert(_localNz[0]*_localNy[0] >= _pX);
  
 	
  // grid is periodic, in each dim.   
  int reorder = 0;       //ranking is not reordered.
 
  _comm = new Comm3DLayer<T>(_pX, _pY, _pZ, DIM-1, dimSz, periods, reorder);
  assert(_comm!=NULL);
  (*_comm).getCartXCoord(&_myPx);
  (*_comm).getCartYCoord(&_myPy);
  (*_comm).getCartZCoord(&_myPz);
    
  Plan( fwd_rev );
  SetSendRecvIndices();
  AllocateMemory();
  SetAllToAllIndexInPlane(DIR_OZ,   1, _pZ);
  SetAllToAllIndexInPlane(DIR_ZY, _pZ, _pY);
  SetAllToAllIndexInPlane(DIR_YX, _pY, _pX);
  
  _strideXYZ[0] = _localNy[0]*_localNz[0];
  _strideXYZ[1] = _localNz[0];
  _strideXYZ[2] =  1;

  _strideR0     = 1; 
  _strideR1     = _localNx[0]*_pX;
  _strideR2     = _strideR1*_localNy[3];
  
  int commSize=-1;
  (*_comm).getSubCommSize(0, &commSize); 
  _strideSrcA[0][2] = 1;
  _strideSrcA[0][1] = _localNz[0]*_nFFTs[0];
  _strideSrcA[0][0] = _localNz[0];
   
  _strideTrgA[0][2] = _nFFTs[0];
  _strideTrgA[0][1] = _nFFTs[0]*_localNz[0];
  _strideTrgA[0][0] = 1;

  _localSizeA[0][2] = _localNz[0];
  _localSizeA[0][1] =  commSize;
  _localSizeA[0][0] = _nFFTs[0];

  commSize=-1;

  (*_comm).getSubCommSize(1, &commSize);
    
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
  
  _strideTrgA[2][2] = _nFFTs[2]*_pX;
  _strideTrgA[2][1] = _nFFTs[2];
  _strideTrgA[2][0] = 1;
 
  _localSizeA[2][2] = _localNx[2];
  _localSizeA[2][1] = _pX*(_localNx[0]/_localNx[2]);
  _localSizeA[2][0] = _nFFTs[2];

  int xSize = MY_MAX(MY_MAX(_localSizeA[0][0],_localSizeA[1][0]),_localSizeA[2][0]);
  int ySize = MY_MAX(MY_MAX(_localSizeA[0][1],_localSizeA[1][1]),_localSizeA[2][1]);
  int zSize = MY_MAX(MY_MAX(_localSizeA[0][2],_localSizeA[1][2]),_localSizeA[2][2]);

  _trgIndex = new int***[DIM-1];
  for(int d=0; d<DIM-1; d++)
    {
      _trgIndex[d] = new int**[xSize];
      assert(_trgIndex[d]!=NULL);
      for(int i=0; i<xSize; i++)
	{
	  _trgIndex[d][i] = new int*[ySize];
	  assert(_trgIndex[d][i]!=NULL);
	  for(int j=0; j<ySize; j++)
	    {
	      _trgIndex[d][i][j] = new int[zSize];
	      assert(_trgIndex[d][i][j]!=NULL);
	    }
	  
	} 
    }
 
  for(int d=0; d<DIM-1; d++)
    {
      int localNx    = _localSizeA[d][0];
      int localNy    = _localSizeA[d][1];
      int localNz    = _localSizeA[d][2];
      
      int trgStride0 = _strideTrgA[d][0];
      int trgStride1 = _strideTrgA[d][1];
      int trgStride2 = _strideTrgA[d][2];

      int counter=0;

      assert( (_localNx[0]*_localNy[0]*_localNz[0]) == (localNx*localNy*localNz)); 
      for(int i=0; i<localNx; i++)
	{
	  for(int j=0; j<localNy; j++)
	    {
	      for(int k=0; k<localNz; k++)
		{
		  if(d==0){
		    _trgIndex[d][i][j][k] = 
		      i*trgStride0+j*trgStride1+k*trgStride2;
		    assert(_trgIndex[d][i][j][k] < _localNx[0]*_localNy[0]*_localNz[0]) ;
		  }
		  else if(d==2)
		    {
		      _trgIndex[d][i][j][k] = counter;
		      counter++;
		      
		      assert(_trgIndex[d][i][j][k] < _localNx[0]*_localNy[0]*_localNz[0]) ;
            
		    }

		  else if(d==1)
		    {
		      int  trgIndex = i*trgStride0+j*trgStride1+k*trgStride2;
		      int howMany = (_localNx[d]*_localNz[d+1])/(_pZ*_pY);
		      
		      int ii    = i     % _localNx[d];
		      int alpha = i     / _localNx[d];
		      int kk    = alpha % _localNz[d+1];
		      int pZ    = alpha / _localNz[d+1];
	
		      trgIndex  = j*trgStride1+(((kk)*_localNy[d]+k))
			*_localNx[d+1]+ (pZ*_localNx[d]+ii);
		      
		      _trgIndex[d][i][j][k] =  trgIndex;
		      
// 		      if(_trgIndex[d][i][j][k] >=  (_localNx[0]*_localNy[0]*_localNz[0]))
// 			{
// 			  printf("DIR %d %d %d %d %d \n", d, i, j, k, _trgIndex[d][i][j][k]);
// 			}
		      assert(_trgIndex[d][i][j][k] <( _localNx[0]*_localNy[0]*_localNz[0])) ;   
		    }
		}
	    }
        }
    }

 //---------------------//
  _fft[2] = new  FFTOneD<T>(_localNx[0]*_pX, fwd_rev); 
  // 1D fft in the x-dim
  _fft[1] = new  FFTOneD<T>(_localNy[0]*_pY, fwd_rev); 
  // 1D fft in the y-dim
  _fft[0] = new  FFTOneD<T>(_localNz[0]*_pZ,  fwd_rev); 
  // 1D fft in the y-dim

  assert(_fft[0]!=NULL);
  assert(_fft[1]!=NULL);
  assert(_fft[2]!=NULL);
  //---------------------//

  (*_comm).GlobalBarrier();

}

template< class T >
void PrivateFFT<T>::Plan( int fwd_rev )
{
  _ab = NULL;
  _ab = new MatrixIndices(_localNx[0]*_pX, _localNy[0]*_pY,_localNz[0]*_pZ,
			  _pX,    _pY,  _pZ,
			  _myPx, _myPy, _myPz); 
  assert(_ab!=NULL);

  int size = MY_MAX(MY_MAX(_pX,_pY),_pZ);
      
  _sendIndex = NULL;
  _recvIndex = NULL;
  
  _sendIndex = new Rectangle**[DIM-1];
  _recvIndex = new Rectangle**[DIM-1];

  assert(_sendIndex!=NULL);
  assert(_recvIndex!=NULL);
  
  for(int dir=0; dir<DIM-1; dir++)
    {
      // need to optimize the max_size by exact size
            
      _sendIndex[dir]  = new Rectangle*[size];
      _recvIndex[dir]  = new Rectangle*[size];

      assert(_sendIndex[dir]!=NULL);
      assert(_recvIndex[dir]!=NULL);

      for(int p=0; p<size; p++)
	{
	  _sendIndex[dir][p]  = new Rectangle[size];
	  _recvIndex[dir][p]  = new Rectangle[size];

	  assert(_sendIndex[dir][p]!= NULL);
	  assert(_recvIndex[dir][p]!= NULL);
	}
    }
}

template< class T >
void PrivateFFT<T>::SetSendRecvIndices()
{
  calcLocalIndexOZ(DIR_OZ);
  calcLocalIndexZY(DIR_ZY);
  calcLocalIndexYX(DIR_YX);

  _nFFTs[0] = (_localNx[0]*_localNy[0])/_pZ;
  _nFFTs[1] = (_localNx[0]*_localNz[0])/_pY;
  _nFFTs[2] = (_localNz[0]*_localNy[0])/_pX;
}

template< class T >
void PrivateFFT<T>::SetAllToAllIndexInPlane(int aDir, int srcP, int trgP)
{
  int dir = aDir;
  int xMin, xMax, yMin, yMax, zMin, zMax;
  int maxDim = 3;
  int coords[3];
  int rank;
  int commSize;
  
  // this function returns the coords of the sub communicator.
  
  int sendCount = 0;
  int sendDisp  = 0;
  
  for(int trgP1=0; trgP1<srcP; trgP1++)    
    for(int trgP0=0; trgP0<trgP; trgP0++)        
      {
      xMin = _sendIndex[dir][trgP1][trgP0].min().x();
      xMax = _sendIndex[dir][trgP1][trgP0].max().x();
      
      yMin = _sendIndex[dir][trgP1][trgP0].min().y();
      yMax = _sendIndex[dir][trgP1][trgP0].max().y();
      
      zMin = _sendIndex[dir][trgP1][trgP0].min().z();
      zMax = _sendIndex[dir][trgP1][trgP0].max().z();
      
      coords[0] = trgP0;
      coords[1] = trgP1;
      
      (*_comm).getSubCommRank(dir%(DIM+1), coords, &rank);	 
      sendDisp += sendCount;
      
      _sendDisp[dir][rank]    = sendDisp;
      
      
      int globalCoord[DIM];
      
//   int  myRank=-1;
//   (*_comm).getCommRank( &myRank );
      // (*_comm).getCommCoords( &myRank, globalCoord  );
      
      (*_comm).getCartCoords( &globalCoord[0], &globalCoord[1], &globalCoord[2]  );
      
      // MPI_Comm_rank((*_comm)._cart_comm, &myRank); 
      // MPI_Cart_coords((*_comm)._cart_comm, myRank, DIM, globalCoord);
      
      
      if(dir==0) { globalCoord[2]=coords[0]; }
      if(dir==1) { globalCoord[1]=coords[0], globalCoord[2]=coords[1]; }
      if(dir==2) { globalCoord[0]=coords[0], globalCoord[1]=coords[1]; }
      
      int globalRank;
      //printf("= = = %d %d %d %d \n",  dir, myPx, myPy, myPz);
      
      (*_comm).getCartRank(&globalCoord[0], &globalRank);	
      _globalSendRank[dir][rank] = globalRank;	

      
#ifdef MPI_ALLTOALL
      _sendDisp[dir][rank]    =  2*sendDisp;  
#endif
      
      sendCount               = (xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
      _sendCount[dir][rank]   = 2*sendCount;
      
      // printf("_sendCount[ %d ][ %d ]: %d\n", dir, rank, _sendCount[dir][rank]);
      }
  int recvCount = 0;
  int recvDisp  = 0;
//   int rankSize;
//   int sendRanks[rankSize];
  
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
      
      // int myRank=-1;
      // MPI_Comm_rank((*_comm)._cart_comm, &myRank);
      // MPI_Cart_coords((*_comm)._cart_comm, myRank, DIM, globalCoord);
      (*_comm).getCartCoords( &globalCoord[0], &globalCoord[1], &globalCoord[2]  );
      
      if(dir==0) {  globalCoord[2]=coords[0];}
      if(dir==1) { globalCoord[1]=coords[0], globalCoord[2]=coords[1];}
      if(dir==2) {globalCoord[0]=coords[0], globalCoord[1]=coords[1];}
      int globalRank;
      
      
      (*_comm).getCartRank(&globalCoord[0], &globalRank);	
      
      _globalRecvRank[dir][rank]=globalRank;	
      recvDisp               += recvCount;
      //where + both
      
      _recvDisp[dir][rank]   = recvDisp;
#ifdef MPI_ALLTOALL
      _recvDisp[dir][rank]   = 2*recvDisp; 
#endif
      recvCount               = (xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
      _recvCount[dir][rank]   = 2*recvCount;
      }
  
  // Init the all to all for this phase
  (*_comm).AllToAllV_Init( _sendCount[ dir], _sendDisp[ dir ],
                           _recvCount[ dir], _recvDisp[ dir ],
                           dir, FFTP_FORWARD );

  (*_comm).AllToAllV_Init( _recvCount[ dir], _recvDisp[ dir ],
                           _sendCount[ dir], _sendDisp[ dir ],
                           dir, FFTP_REVERSE );
  return;
}

template< class T >
void  PrivateFFT<T>::DoLocalMapping(Point3D &pointMin,
					       Point3D &pointMax, 
					       int x, int y, int z)
{
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
       
template< class T >
void PrivateFFT<T>::calcLocalIndexOZ(int dir)
{
  assert(dir==DIR_OZ);
  int sumRecvX = 0;  int sumRecvY = 0; int sumRecvZ = 0;

  for(int trgPz=0; trgPz<_pZ; trgPz++)
    {
      //get global send indecies
      _sendIndex[dir][0][trgPz] = (*_ab).calcSendOtoZdataDistribution
	(_myPx, _myPy, _myPz, _myPx, _myPy, trgPz);
      
      Point3D sendMin = _sendIndex[dir][0][trgPz].min();
      Point3D sendMax = _sendIndex[dir][0][trgPz].max();
      
      DoLocalMapping(sendMin, sendMax, _localNx[0], _localNy[0], _localNz[0]);

      _sendIndex[dir][0][trgPz].setMin(sendMin);
      _sendIndex[dir][0][trgPz].setMax(sendMax);

      _recvIndex[dir][0][trgPz] = (*_ab).calcRecvZfromOdataDistribution
	(_myPx, _myPy, _myPz, _myPx, _myPy, trgPz);  
    
      Point3D recvMin = _recvIndex[dir][0][trgPz].min();
      Point3D recvMax = _recvIndex[dir][0][trgPz].max();  
      
       DoLocalMapping(recvMin, recvMax,_localNx[0], _localNy[0], 0);
     
      _recvIndex[dir][0][trgPz].setMin(recvMin);
      _recvIndex[dir][0][trgPz].setMax(recvMax);
      
      sumRecvX += (recvMax.x()-recvMin.x())+1;  
      sumRecvY += (recvMax.y()-recvMin.y())+1;  
      sumRecvZ += (recvMax.z()-recvMin.z())+1;  
    }  

  // set local sizes
  _localNx[dir+1] = sumRecvX/_pZ;
  _localNy[dir+1] = sumRecvY/_pZ;
  _localNz[dir+1] = sumRecvZ;
  _localPx[dir]=1;  _localPy[dir]=1;  _localPz[dir]=1;

  _localPx[dir+1] = 1;
  _localPy[dir+1] = 1;
  _localPz[dir+1] = _pZ;

  Point3D tmpMin = (*_ab)._Rz[_myPx][_myPy][_myPz].min();
  Point3D tmpMax = (*_ab)._Rz[_myPx][_myPy][_myPz].max();
  
  assert(_localNx[dir+1] == (tmpMax.x()-tmpMin.x()+1) &&
	 _localNy[dir+1] == (tmpMax.y()-tmpMin.y()+1) &&
	 _localNz[dir+1] == (tmpMax.z()-tmpMin.z()+1));
}

template< class T >
void PrivateFFT<T>::calcLocalIndexZY(int dir)
{
  assert(dir==DIR_ZY);
 
  int sumRecvX = 0; int sumRecvY = 0; int sumRecvZ = 0;
  for(int trgPy=0; trgPy<_pY; trgPy++)
    {
      for(int trgPz=0; trgPz<_pZ; trgPz++)
	{
	  _sendIndex[dir][trgPz][trgPy] = 
	    (*_ab).calcSendZtoYdataDistribution(_myPx, _myPy, _myPz,
						_myPx, trgPy, trgPz);      
	  
	  Point3D sendMin = _sendIndex[dir][trgPz][trgPy].min();
	  Point3D sendMax = _sendIndex[dir][trgPz][trgPy].max();
	  
	  DoLocalMapping(sendMin, sendMax, _localNx[0], _localNy[0], 0);
	  
	  _sendIndex[dir][trgPz][trgPy].setMin(sendMin);
	  _sendIndex[dir][trgPz][trgPy].setMax(sendMax);
	  
	  _recvIndex[dir][trgPz][trgPy] = 
	    (*_ab).calcRecvYfromZdataDistribution
	    (_myPx, _myPy, _myPz, _myPx, trgPy, trgPz);
	  
	  Point3D recvMin = _recvIndex[dir][trgPz][trgPy].min();
	  Point3D recvMax = _recvIndex[dir][trgPz][trgPy].max();
	  
	  DoLocalMapping(recvMin, recvMax, _localNx[0], 0, _localNz[0]);

	  _recvIndex[dir][trgPz][trgPy].setMin(recvMin);
	  _recvIndex[dir][trgPz][trgPy].setMax(recvMax);
	  
	  sumRecvX += recvMax.x()-recvMin.x()+1;  
	  sumRecvY += recvMax.y()-recvMin.y()+1;  
	  sumRecvZ += recvMax.z()-recvMin.z()+1;  
	}
    }
 
  
  Point3D *tmpMin = &((*_ab)._Ry[_myPx][_myPy][_myPz].min());
  Point3D *tmpMax = &((*_ab)._Ry[_myPx][_myPy][_myPz].max());

  _localNx[dir+1] = (tmpMax->x()-tmpMin->x()+1);
  _localNy[dir+1] = (tmpMax->y()-tmpMin->y()+1);
  _localNz[dir+1] = (tmpMax->z()-tmpMin->z()+1);
  

  if(_localNx[dir+1]<=0) _localNx[dir+1]=1;
  if(_localNy[dir+1]<=0) _localNy[dir+1]=1;
  if(_localNz[dir+1]<=0) _localNz[dir+1]=1;

  assert(_localNx[dir+1]>0 &&
 	 _localNy[dir+1]>0 &&
	 _localNz[dir+1]>0);

  _localPx[dir+1] = 1;
  _localPy[dir+1] = _pY;
  _localPz[dir+1] = _pZ;
}

template< class T >
void PrivateFFT<T>::calcLocalIndexYX(int dir)
{
  assert(dir == DIR_YX);
  int sumRecvX = 0; int sumRecvY = 0; int sumRecvZ = 0;

  for(int trgPx=0; trgPx<_pX; trgPx++)
    for(int trgPy=0; trgPy<_pY; trgPy++)
      {
	_sendIndex[dir][trgPy][trgPx] =
	  (*_ab).calcSendYtoXdataDistribution
	  (_myPx, _myPy, _myPz, trgPx, trgPy, _myPz);
	
	Point3D sendMin = _sendIndex[dir][trgPy][trgPx].min();
	Point3D sendMax = _sendIndex[dir][trgPy][trgPx].max();
	
       	DoLocalMapping(sendMin, sendMax,_localNx[0], 0, _localNz[0]);
	
	_sendIndex[dir][trgPy][trgPx].setMin(sendMin);
	_sendIndex[dir][trgPy][trgPx].setMax(sendMax);
	
	_recvIndex[dir][trgPy][trgPx] = 
	  (*_ab).calcRecvXfromYdataDistribution
	  (_myPx, _myPy, _myPz, trgPx, trgPy, _myPz);
	
	Point3D recvMin = _recvIndex[dir][trgPy][trgPx].min();
	Point3D recvMax = _recvIndex[dir][trgPy][trgPx].max();

	DoLocalMapping(recvMin, recvMax, 0, _localNy[0], _localNz[0]);	

	_recvIndex[dir][trgPy][trgPx].setMin(recvMin);
	_recvIndex[dir][trgPy][trgPx].setMax(recvMax);

	sumRecvX += recvMax.x()-recvMin.x()+1;  
	sumRecvY += recvMax.y()-recvMin.y()+1;  
	sumRecvZ += recvMax.z()-recvMin.z()+1;  
      }
 
  Point3D tmpMin = ((*_ab)._Rx[_myPx][_myPy][_myPz].min());
  Point3D tmpMax = ((*_ab)._Rx[_myPx][_myPy][_myPz].max());
      
  _localNx[dir+1] = (tmpMax.x()-tmpMin.x()+1);
  _localNy[dir+1] = (tmpMax.y()-tmpMin.y()+1);
  _localNz[dir+1] = (tmpMax.z()-tmpMin.z()+1);
  
  //check
  if(_localNx[dir+1]<=0) _localNx[dir+1]=1;
  if(_localNy[dir+1]<=0) _localNy[dir+1]=1;
  if(_localNz[dir+1]<=0) _localNz[dir+1]=1;
  assert(_localNx[dir+1]>0 &&
 	 _localNy[dir+1]>0 &&
	 _localNz[dir+1]>0);  
}

template< class T >
void PrivateFFT<T>::AllocateMemory()
{ 
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

  int totalLocal      = _localNx[0]*_localNy[0]*_localNz[0]+1;
  _totalLocal         = totalLocal ;
  int maxLocal        = MY_MAX(_localNx[0], MY_MAX(_localNy[0], _localNz[0]));
  int maxLocalSize    = (MY_MAX(_pX,MY_MAX(_pY, _pZ)))*totalLocal;
  int maxLocalTmpSize = (MY_MAX(_pX,MY_MAX(_pY, _pZ)))*maxLocal;

#if defined(TMP_IN_FREESTORE) 
  _tmpSrc = new complexTemplate<T>[maxLocalTmpSize];        
  _tmpTrg = new complexTemplate<T>[maxLocalTmpSize];  
#endif  
      	
//  _localDataIn   = new complexTemplate<T>[totalLocal];
//  _localDataOut  = new complexTemplate<T>[totalLocal];
// Allocate 'localData' with 32 bytes of padding before and after, so that
// the data will not share cache lines with metadata; we think this sharing
// can cause problems when running k-space on core 1
    _localDataBase = new complexTemplate<T>[2*totalLocal+32/sizeof(T)] ;
    
    _localDataIn = _localDataBase+16/sizeof(T) ;
    _localDataOut = _localDataIn+totalLocal ; 
  
#if defined(FFT_LOCAL_IN_FREESTORE)  
  _localDataTmp1 = new complexTemplate<T>[totalLocal];
  _localDataTmp2 = new complexTemplate<T>[totalLocal];
#endif
  
#if defined(TMP_IN_FREESTORE) 
  assert(_tmpSrc!=NULL);
  assert(_tmpTrg!=NULL);
#endif  
  
//  assert(_localDataIn!=NULL);
//  assert(_localDataOut!=NULL);
  assert(_localDataBase != NULL) ;
  
#if defined(FFT_LOCAL_IN_FREESTORE)  
  assert(_localDataTmp1!=NULL);
  assert(_localDataTmp2!=NULL);
#endif    
}

template< class T >
void PrivateFFT<T>::Finalize()
{
  
  delete _ab;
//  delete[] _localDataIn;
#if defined(FFT_LOCAL_IN_FREESTORE)  
  delete[] _localDataTmp1;
  delete[] _localDataTmp2;
#endif
//  delete[] _localDataOut;

  delete[] _localDataBase ;
  _localDataBase = NULL ; 
  
#if defined(TMP_IN_FREESTORE) 
  delete[] _tmpSrc;
  delete[] _tmpTrg;
#endif
  
  _localDataIn = NULL;
#if defined(FFT_LOCAL_IN_FREESTORE)  
  _localDataTmp1 = NULL;
  _localDataTmp2 = NULL;
#endif  
  _localDataOut  = NULL;  
  assert(_localDataIn   == NULL);
  
#if defined(FFT_LOCAL_IN_FREESTORE)  
  assert( _localDataTmp1 == NULL);
  assert( _localDataTmp2 == NULL);
#endif  
  assert( _localDataOut  == NULL);
  
#if defined(TMP_IN_FREESTORE) 
  _tmpSrc = NULL; _tmpTrg = NULL;
  assert( _tmpSrc == NULL);
  assert( _tmpTrg == NULL);
#endif
  
  int trgP = MY_MAX(_pX, MY_MAX(_pY,_pZ));      
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
  
  int size = MY_MAX(MY_MAX(_pX,_pY),_pZ);
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

  int xSize = MY_MAX(MY_MAX(_localSizeA[0][0],_localSizeA[1][0]),_localSizeA[2][0]);
  int ySize = MY_MAX(MY_MAX(_localSizeA[0][1],_localSizeA[1][1]),_localSizeA[2][1]);
  int zSize = MY_MAX(MY_MAX(_localSizeA[0][2],_localSizeA[1][2]),_localSizeA[2][2]);
  
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






#endif



