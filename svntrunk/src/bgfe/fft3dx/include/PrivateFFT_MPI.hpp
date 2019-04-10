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

// #define TMP_IN_FREESTORE

#if !defined(PKFXLOG_FFT_SCALE)
#define PKFXLOG_FFT_SCALE (0) 
#endif

#if !defined(PKFXLOG_FFT_SCALE_DETAIL)
#define PKFXLOG_FFT_SCALE_DETAIL (0) 
#endif

#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include <MPI/comm3DLayer_MPI.hpp>
#include <MPI/fft_one_dim_MPI.hpp>
#include <MPI/arrayBounds_MPI.hpp>


// class complexInteger: public complexTemplate<int> { } ;

template <typename T>  
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

  unsigned int   _checksum;
  
  Rectangle*** _sendIndex;
  Rectangle*** _recvIndex;
  int _globalSendRank[3][ 1024 ];
  int _globalRecvRank[3][ 1024 ];

  MatrixIndices *_ab;
  int _pX;  int _pY;  int _pZ;
  int _myPx;  int _myPy;  int _myPz,_myRank, _globalID;

  complexTemplate<T> *_localDataIn;
  complexTemplate<T> *_localDataTmp1;
  complexTemplate<T> *_localDataTmp2;
  complexTemplate<T> *_localDataOut;
  
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)
  complexInteger *_localDataIntegerIn;
  complexInteger *_localDataIntegerTmp1;
  complexInteger *_localDataIntegerTmp2;
  complexInteger *_localDataIntegerOut;
#endif
  // Scaling factors, which we revise as we go along
  double forwardScale[DIM-1] ;
  double reverseScale[DIM-1] ;
  double forwardRScale[DIM-1] ;
  double reverseRScale[DIM-1] ;
  double localPeak[2*(DIM-1)] ;
   // and when we want to do the next global revision 
  int sequenceNumber ;
  int nextGlobalRevision ;
  
  // private functions
  
         void Initialize( int fwd_rev );
         void Plan( int fwd_rev );
  
         void AllocateMemory();
  inline void DoLocalMapping(Point3D &pointMin, Point3D &pointMax, int x, int y, int z);
  
         void SetSendRecvIndices();
         void SetAllToAllIndexInPlane(int aDir, int srcP, int trgP);
  
         void calcLocalIndexOZ(int dir);
         void calcLocalIndexZY(int dir);
         void calcLocalIndexYX(int dir);
 
  unsigned int GetChecksum()
    {
      int commSize = -1;
      unsigned int checksum = 0;

      for( int dir=0; dir<DIM-1; dir++ )
        {  
        (*_comm).getSubCommSize(dir, &commSize);
        for( int jj=0; jj<commSize; jj++ )
          {
          checksum += ( _recvCount[ dir ][ jj ] + _sendCount[ dir ][ jj ] +
                        _recvDisp[ dir ][ jj ] + _sendDisp[ dir ][ jj ] );
          }
        }
      
      return checksum;
    }
 
  inline void IsValidChecksum()
    {
    assert( GetChecksum() == _checksum );
    }  
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)    
  inline double MaxAbs(double Running, double Candidate)
  {
  	double absCandidate=abs(Candidate) ;
  	return fsel(absCandidate-Running,absCandidate,Running) ;
  }
  inline double ScaleFromInteger(int x, double scaleFactor ) { return x*scaleFactor ; } 
  inline int ScaleToInteger(double x, double scaleFactor)  { return x*scaleFactor ; } 
  
  inline void PackToInteger(int dir, 
			   int* sendCount, 
			   int* sendDisp,
			   complexTemplate<T> *localDataSrc,
			   complexInteger *localDataIntegerSrc,
			   double scaleFactor )
  {
  	int commSize = -1;
    int myID = -1;
    
    (*_comm).getSubCommSize(dir, &commSize);
    (*_comm).getSubCommRank(dir, &myID);
    unsigned int numProcs     = commSize;
    unsigned int numOfRequest = 2*commSize -2;     
    int counter               = 0;
    
    double remax = 0.0 ; 
    double immax = 0.0 ; 
    
    assert(numOfRequest>= 0); 
    assert(numProcs>0);
    assert(numProcs ==commSize);      
    
    for(int trgPy=0; trgPy<(numProcs/2); trgPy++)
      {
	int sendDispTmp = sendDisp[trgPy];
	int sendCount=_sendCount[dir][trgPy];
	for(int trgPz=0; trgPz<sendCount; trgPz+=1)
	{
		double re = localDataSrc[sendDispTmp+trgPz].re ;
		double im = localDataSrc[sendDispTmp+trgPz].im ; 
		remax=MaxAbs(remax,re) ;
		immax=MaxAbs(immax,im) ;
		BegLogLine(PKFXLOG_FFT_SCALE_DETAIL)
		  << "scaling " << re << " " << remax
		  << " " << im << " " << immax 
          << EndLogLine ;
		localDataIntegerSrc[sendDispTmp+trgPz].re=ScaleToInteger(re,scaleFactor) ;
		localDataIntegerSrc[sendDispTmp+trgPz].im=ScaleToInteger(im,scaleFactor) ;
	}
      }
      
    BegLogLine(PKFXLOG_FFT_SCALE) 
      << "PackToInteger scaling "
      << (remax * scaleFactor / MAXINT )
      << " " << (immax * scaleFactor / MAXINT )
      << EndLogLine ; 
      
    if ( ( remax * scaleFactor > MAXINT ) 
        || (immax * scaleFactor > MAXINT ) )
    {
    	BegLogLine(1)
    	  << "FFT PackToInteger tried to produce " << (remax * scaleFactor)
    	  << " " << (immax * scaleFactor )
    	  << " with scaleFactor=" << scaleFactor
    	  << EndLogLine ;
    	PLATFORM_ABORT("FFT PackToInteger overflowed ") ; 
    	
    }        
        
  }
    inline void UnpackFromInteger(int dir, 
			   int* recvCount,
			   int* recvDisp,
			   complexTemplate<T> *localDataTrg,
			   complexInteger *localDataIntegerTgt,
			   double scaleFactor)
  {
  	int commSize = -1;
    int myID = -1;
    
    (*_comm).getSubCommSize(dir, &commSize);
    (*_comm).getSubCommRank(dir, &myID);
    unsigned int numProcs     = commSize;
    unsigned int numOfRequest = 2*commSize -2;     
    int counter               = 0;
    
    assert(numOfRequest>= 0); 
    assert(numProcs>0);
    assert(numProcs ==commSize);      
    
//	(*_comm).IrecvGlobal((void*)(&(localDataTrg[recvDisp[srcPy]])),
//		       recvCount[srcPy], 
//		       _globalRecvRank[dir][srcPy], dir,counter);
   
    for(int srcPy=0; srcPy<(numProcs/2); srcPy++)
      {
	    int recvDispTmp = recvDisp[srcPy];
	    int recvCountTmp = recvCount[srcPy] ;
	     	
		for(int i=0; i<recvCountTmp; i+=1)
		{
			localDataTrg[recvDispTmp+i].re=ScaleFromInteger(localDataIntegerTgt[recvDispTmp+i].re,scaleFactor) ;
			localDataTrg[recvDispTmp+i].im=ScaleFromInteger(localDataIntegerTgt[recvDispTmp+i].im,scaleFactor) ;
		}
	
      }
  	
  }
#endif  
  
  inline void Redistribute(int dir, 
			   int* sendCount, 
			   int* sendDisp,
			   complexTemplate<T> *localDataSrc,
			   int* recvCount,
			   int* recvDisp,
			   complexTemplate<T> *localDataTrg)
  {
    
#ifdef MPI_ALLTOALL

    (*_comm).AllToAllV((void*)localDataSrc, sendCount, sendDisp,
	      (void*)localDataTrg, recvCount, recvDisp, dir);

#else 
    
    int commSize = -1;
    int myID = -1;
    
    (*_comm).getSubCommSize(dir, &commSize);
    (*_comm).getSubCommRank(dir, &myID);
    unsigned int numProcs     = commSize;
    unsigned int numOfRequest = 2*commSize -2;     
    int counter               = 0;
    
    assert(numOfRequest>= 0); 
    assert(numProcs>0);
    assert(numProcs ==commSize);      
    
    for(int srcPy=0; srcPy<numProcs; srcPy++)
      {
	if(srcPy==myID) continue;
	
	(*_comm).IrecvGlobal((void*)(&(localDataTrg[recvDisp[srcPy]])),
		       recvCount[srcPy], 
		       _globalRecvRank[dir][srcPy], dir,counter);
	counter++;
      }
    
    for(int trgPy=0; trgPy<numProcs; trgPy++)
      {
	int sendDispTmp = sendDisp[trgPy];
	if(trgPy==myID)
	  { 	 	
	    memcpy((void*)&(localDataTrg[recvDisp[trgPy]]), 
		   (void*)&(localDataSrc[sendDispTmp]), 
		   recvCount[trgPy]*8);
	  }
	else
	  {
	    (*_comm).IsendGlobal((void*)&(localDataSrc[sendDispTmp]),
			   _sendCount[dir][trgPy],
			   _globalSendRank[dir][trgPy], dir,
			   counter);
	    counter++;
	  } 
      }
    
    // wait for pending sends and receives to complete
    assert(counter==numOfRequest);
    if(counter>0) (*_comm).Wait(counter);
#endif	
  }
  #if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)    
  inline void RedistributeInteger(int dir, 
			   int* sendCount, 
			   int* sendDisp,
			   complexInteger *localDataIntegerSrc,
			   int* recvCount,
			   int* recvDisp,
			   complexInteger *localDataIntegerTrg)
  {
    
#ifdef MPI_ALLTOALL

    (*_comm).AllToAllVInteger((void*)localDataIntegerSrc, sendCount, sendDisp,
	      (void*)localDataIntegerTrg, recvCount, recvDisp, dir);

#else 
    
    int commSize = -1;
    int myID = -1;
    
    (*_comm).getSubCommSize(dir, &commSize);
    (*_comm).getSubCommRank(dir, &myID);
    unsigned int numProcs     = commSize;
    unsigned int numOfRequest = 2*commSize -2;     
    int counter               = 0;
    
    assert(numOfRequest>= 0); 
    assert(numProcs>0);
    assert(numProcs ==commSize);      
    
    for(int srcPy=0; srcPy<numProcs; srcPy++)
      {
	if(srcPy==myID) continue;
	
	(*_comm).IrecvGlobalInteger((void*)(&(localDataIntegerTrg[recvDisp[srcPy]])),
		       recvCount[srcPy], 
		       _globalRecvRank[dir][srcPy], dir,counter);
	counter++;
      }
    
    for(int trgPy=0; trgPy<numProcs; trgPy++)
      {
	int sendDispTmp = sendDisp[trgPy];
	if(trgPy==myID)
	  { 	 	
	    memcpy((void*)&(localDataIntegerTrg[recvDisp[trgPy]]), 
		   (void*)&(localDataIntegerSrc[sendDispTmp]), 
		   recvCount[trgPy]*sizeof(integer));
	  }
	else
	  {
	    (*_comm).IsendGlobalInteger((void*)&(localDataSrcInteger[sendDispTmp]),
			   _sendCount[dir][trgPy],
			   _globalSendRank[dir][trgPy], dir,
			   counter);
	    counter++;
	  } 
      }
    
    // wait for pending sends and receives to complete
    assert(counter==numOfRequest);
    if(counter>0) (*_comm).Wait(counter);
#endif	
  }
#endif  
  
  
  
  inline  void ReorderDoFFTs(int dir,
			     int* srcStride, 
			     int* trgStride,
			     int* localSize,
			     complexTemplate<T>* localDataSrc,
			     complexTemplate<T>* localDataTrg,
			     FFTOneD<T> *fft,
			     double& maxModSQOut)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];
    
#if !defined(TMP_IN_FREESTORE)    
  complexTemplate<T> _tmpSrc[localNz*localNy];
  complexTemplate<T> _tmpTrg[localNz*localNy];
#endif    
    int srcStride0 = srcStride[0];
    int srcStride1 = srcStride[1];
    int srcStride2 = srcStride[2];
    
    double MaxModSQ = 0.0 ; 
    double SumModSQ = 0.0 ;
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    
    int counter = 0;
    
    int ***trgIndexTmp = _trgIndex[dir];
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		srcIndex = i*srcStride0+j*srcStride1+k*srcStride2;
	      
          T re=localDataSrc[srcIndex].re;
          T im=localDataSrc[srcIndex].im;
          double modsq = re*re + im*im ; 
          MaxModSQ=fsel(modsq-MaxModSQ,modsq,MaxModSQ) ;
		assert(k+j*localNz  < localNz*localNy);
		_tmpSrc[k+j*localNz].re=re;
		_tmpSrc[k+j*localNz].im=im;
#if PKFXLOG_FFT_SCALE
        {
          SumModSQ += modsq ;
        }
#endif		
		assert(srcIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
	      }
	  }
	assert(fft!=NULL);
	(*fft).fftInternal(_tmpSrc, _tmpTrg);
	for(int j=0; j<localNy; j++)
	  {
	    for(int k=0; k<localNz; k++)
	      {
		trgIndex = _trgIndex[dir][i][j][k];
		
		assert(trgIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
		assert(k+j*localNz  < localNz*localNy);
		
		localDataTrg[trgIndex].re = _tmpTrg[k+j*localNz].re;
		localDataTrg[trgIndex].im = _tmpTrg[k+j*localNz].im;
	      }
	  }
      }
    maxModSQOut = MaxModSQ ;
   	BegLogLine(PKFXLOG_FFT_SCALE)
     << "MaxModSQ FFT = " << MaxModSQ 
     << " SumModSQ = " << SumModSQ
	 << EndLogLine ; 
      
  }
  
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)    
  inline  void ReorderDoFFTsIntegerDouble(int dir,
			     int* srcStride, 
			     int* trgStride,
			     int* localSize,
			     complexInteger* localDataIntegerSrc,
			     complexTemplate<T>* localDataTrg,
			     FFTOneD<T> *fft,
			     double scaleFactor,
			     double& maxModSQOut)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];
    
#if !defined(TMP_IN_FREESTORE)    
  complexTemplate<T> _tmpSrc[localNz*localNy];
  complexTemplate<T> _tmpTrg[localNz*localNy];
#endif    

    int srcStride0 = srcStride[0];
    int srcStride1 = srcStride[1];
    int srcStride2 = srcStride[2];

    double MaxModSQ = 0.0 ; 
    double SumModSQ = 0.0 ;
  
    
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    
    int counter = 0;
    
    int ***trgIndexTmp = _trgIndex[dir];
    
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		  srcIndex = i*srcStride0+j*srcStride1+k*srcStride2;
	      
		  double re=ScaleFromInteger(localDataIntegerSrc[srcIndex].re,scaleFactor);
		  double im=ScaleFromInteger(localDataIntegerSrc[srcIndex].im,scaleFactor);
          double modsq = re*re + im*im ; 
          MaxModSQ=fsel(modsq-MaxModSQ,modsq,MaxModSQ) ;
		  assert(k+j*localNz  < localNz*localNy);
		  _tmpSrc[k+j*localNz].re=re;
		  _tmpSrc[k+j*localNz].im=im;
#if PKFXLOG_FFT_SCALE
        {
          SumModSQ += modsq ;
        }
#endif		
		assert(srcIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
	      }
	  }
	assert(fft!=NULL);
	(*fft).fftInternal(_tmpSrc, _tmpTrg);
	for(int j=0; j<localNy; j++)
	  {
	    for(int k=0; k<localNz; k++)
	      {
		trgIndex = _trgIndex[dir][i][j][k];
		
		assert(trgIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
		assert(k+j*localNz  < localNz*localNy);
		
		localDataTrg[trgIndex].re = _tmpTrg[k+j*localNz].re;
		localDataTrg[trgIndex].im = _tmpTrg[k+j*localNz].im;
	      }
	  }
      }
    maxModSQOut = MaxModSQ ;
   	BegLogLine(PKFXLOG_FFT_SCALE)
     << "MaxModSQ FFT = " << MaxModSQ 
     << " SumModSQ = " << SumModSQ
	 << EndLogLine ; 
  }
  
  inline  void ReorderDoFFTsIntegerInteger(int dir,
			     int* srcStride, 
			     int* trgStride,
			     int* localSize,
			     complexInteger* localDataIntegerSrc,
			     complexInteger* localDataIntegerTrg,
			     FFTOneD<T> *fft,
			     double scaleFactor,
			     double scaleFactorOut,
			     double& maxModSQOut)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];
    
#if !defined(TMP_IN_FREESTORE)    
  complexTemplate<T> _tmpSrc[localNz*localNy];
  complexTemplate<T> _tmpTrg[localNz*localNy];
#endif    
    int srcStride0 = srcStride[0];
    int srcStride1 = srcStride[1];
    int srcStride2 = srcStride[2];

    double MaxModSQ = 0.0 ; 
    double SumModSQ = 0.0 ;
  
    double remax = 0.0 ; 
    double immax = 0.0 ; 
    
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    
    int counter = 0;
    
    int ***trgIndexTmp = _trgIndex[dir];
    
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		  srcIndex = i*srcStride0+j*srcStride1+k*srcStride2;
	      
		  double re=ScaleFromInteger(localDataIntegerSrc[srcIndex].re,scaleFactor);
		  double im=ScaleFromInteger(localDataIntegerSrc[srcIndex].im,scaleFactor);
          double modsq = re*re + im*im ; 
          MaxModSQ=fsel(modsq-MaxModSQ,modsq,MaxModSQ) ;
		  _tmpSrc[k+j*localNz].re=re;
		  _tmpSrc[k+j*localNz].im=im;
#if PKFXLOG_FFT_SCALE
        {
          SumModSQ += modsq ;
        }
#endif		
		assert(srcIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
	      }
	  }
	assert(fft!=NULL);
	(*fft).fftInternal(_tmpSrc, _tmpTrg);
	for(int j=0; j<localNy; j++)
	  {
	    for(int k=0; k<localNz; k++)
	      {
		trgIndex = _trgIndex[dir][i][j][k];
		
		assert(trgIndex < _localNx[0]*_localNy[0]*_localNz[0]);     
		assert(k+j*localNz  < localNz*localNy);
		
		double re = _tmpTrg[k+j*localNz].re ;
		double im = _tmpTrg[k+j*localNz].im ; 
		remax=MaxAbs(remax,re) ;
		immax=MaxAbs(immax,im) ;
		BegLogLine(PKFXLOG_FFT_SCALE_DETAIL)
		  << "scaling " << re << " " << remax
		  << " " << im << " " << immax 
          << EndLogLine ;

		
		localDataIntegerTrg[trgIndex].re = ScaleToInteger(re,scaleFactorOut);
		localDataIntegerTrg[trgIndex].im = ScaleToInteger(im,scaleFactorOut);
	      }
	  }
      }
    maxModSQOut = MaxModSQ ;
   	BegLogLine(PKFXLOG_FFT_SCALE)
     << "MaxModSQ FFT = " << MaxModSQ 
     << " SumModSQ = " << SumModSQ
	 << EndLogLine ; 
	 
	BegLogLine(PKFXLOG_FFT_SCALE) 
      << "PackToInteger scaling "
      << (remax * scaleFactorOut / MAXINT )
      << " " << (immax * scaleFactorOut / MAXINT )
      << EndLogLine ; 
      
    if ( ( remax * scaleFactorOut > MAXINT ) 
        || (immax * scaleFactorOut > MAXINT ) )
    {
    	BegLogLine(1)
    	  << "FFT PackToInteger tried to produce " << (remax * scaleFactorOut)
    	  << " " << (immax * scaleFactorOut )
    	  << " with scaleFactor=" << scaleFactorOut
    	  << EndLogLine ;
    	PLATFORM_ABORT("FFT PackToInteger overflowed ") ; 
    	
    }        
	 
  }
#endif  
  
  inline void ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,
			      int *localSize,
			      complexTemplate<T>* localDataSrc, complexTemplate<T>* localDataTrg, FFTOneD<T>* fft, double& maxModSQOut )
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];  
    
#if !defined(TMP_IN_FREESTORE)    
  complexTemplate<T> _tmpSrc[localNz*localNy];
  complexTemplate<T> _tmpTrg[localNz*localNy];
#endif    
    int trgStride0 = trgStride[0];
    int trgStride1 = trgStride[1];
    int trgStride2 = trgStride[2];
    
    double MaxModSQ = 0.0 ; 
    double SumModSQ = 0.0 ;
    
    int nFFTs = _nFFTs[dir];
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    int counter = 0;
    
    int ***srcIndexD =  _trgIndex[dir];  
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		srcIndex = srcIndexD[i][j][k];
		
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(k+j*localNz < localNz*localNy);
		
		_tmpSrc[k+j*localNz].re = localDataSrc[srcIndex].re;
		_tmpSrc[k+j*localNz].im = localDataSrc[srcIndex].im;	      
		
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
		
		T re = _tmpTrg[k+j*localNz].re;
		T im = _tmpTrg[k+j*localNz].im;	      
		localDataTrg[trgIndex].re = re;
		localDataTrg[trgIndex].im = im;
        double modsq = re*re + im*im ;
        MaxModSQ=fsel(modsq-MaxModSQ,modsq,MaxModSQ) ;
         
#if PKFXLOG_FFT_SCALE
        {
          SumModSQ += modsq ;
        }
#endif		
	      } 
	  } 
      }
    maxModSQOut = MaxModSQ ;
   	BegLogLine(PKFXLOG_FFT_SCALE)
     << "MaxModSQ FFT = " << MaxModSQ 
     << " SumModSQ = " << SumModSQ
	 << EndLogLine ; 
  }
  
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)    
  inline void ReorderDoFFTsIntegerDouble_R(int dir, int *srcStride, int *trgStride,
			      int *localSize,
			      complexInteger* localDataIntegerSrc,
			      complexTemplate<T>* localDataTrg,
			      FFTOneD<T> *fft, double scaleFactor,
			      double& maxModSQOut)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];  
    
#if !defined(TMP_IN_FREESTORE)    
  complexTemplate<T> _tmpSrc[localNz*localNy];
  complexTemplate<T> _tmpTrg[localNz*localNy];
#endif    
    int trgStride0 = trgStride[0];
    int trgStride1 = trgStride[1];
    int trgStride2 = trgStride[2];
    
    double MaxModSQ = 0.0 ; 
    double SumModSQ = 0.0 ;
    
    int nFFTs = _nFFTs[dir];
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    int counter = 0;
    
    int ***srcIndexD =  _trgIndex[dir];  
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		srcIndex = srcIndexD[i][j][k];
		
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(k+j*localNz < localNz*localNy);
		
		_tmpSrc[k+j*localNz].re = ScaleFromInteger(localDataIntegerSrc[srcIndex].re,scaleFactor);
		_tmpSrc[k+j*localNz].im = ScaleFromInteger(localDataIntegerSrc[srcIndex].im,scaleFactor);	      
		
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
		
		double re = _tmpTrg[k+j*localNz].re;
		double im = _tmpTrg[k+j*localNz].im;	      
		localDataTrg[trgIndex].re = re;
		localDataTrg[trgIndex].im = im;
        double modsq = re*re + im*im ;
        MaxModSQ=fsel(modsq-MaxModSQ,modsq,MaxModSQ) ;
			      
#if PKFXLOG_FFT_SCALE
        {
          SumModSQ += modsq ;
        }
#endif		
	      } 
	  } 
      }
    maxModSQOut = MaxModSQ ;
   	BegLogLine(PKFXLOG_FFT_SCALE)
     << "MaxModSQ FFT = " << MaxModSQ 
     << " SumModSQ = " << SumModSQ
	 << EndLogLine ; 
  }
  inline void ReorderDoFFTsIntegerInteger_R(int dir, int *srcStride, int *trgStride,
			      int *localSize,
			      complexInteger* localDataIntegerSrc, 
			      complexInteger* localDataIntegerTrg, 
			      FFTOneD<T> *fft, 
			      double scaleFactor,
			      double scaleFactorOut,
			      double& maxModSQOut)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];  
    
#if !defined(TMP_IN_FREESTORE)    
  complexTemplate<T> _tmpSrc[localNz*localNy];
  complexTemplate<T> _tmpTrg[localNz*localNy];
#endif    
    int trgStride0 = trgStride[0];
    int trgStride1 = trgStride[1];
    int trgStride2 = trgStride[2];
    
    double MaxModSQ = 0.0 ; 
    double SumModSQ = 0.0 ;
    
    double remax=0.0 ; 
    double immax = 0.0 ; 
    
    int nFFTs = _nFFTs[dir];
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    int counter = 0;
    
    int ***srcIndexD =  _trgIndex[dir];  
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		srcIndex = srcIndexD[i][j][k];
		
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(k+j*localNz < localNz*localNy);
		
		_tmpSrc[k+j*localNz].re = ScaleFromInteger(localDataIntegerSrc[srcIndex].re,scaleFactor);
		_tmpSrc[k+j*localNz].im = ScaleFromInteger(localDataIntegerSrc[srcIndex].im,scaleFactor);	      
		
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
		double re=_tmpTrg[k+j*localNz].re ;
		double im=_tmpTrg[k+j*localNz].im ;
        double modsq = re*re + im*im ;
        MaxModSQ=fsel(modsq-MaxModSQ,modsq,MaxModSQ) ;
		
		localDataIntegerTrg[trgIndex].re = ScaleToInteger(re,scaleFactorOut);
		localDataIntegerTrg[trgIndex].im = ScaleToInteger(im,scaleFactorOut);	      
		remax=MaxAbs(remax,re) ;
		immax=MaxAbs(immax,im) ;
		BegLogLine(PKFXLOG_FFT_SCALE_DETAIL)
		  << "scaling " << re << " " << remax
		  << " " << im << " " << immax 
          << EndLogLine ;

#if PKFXLOG_FFT_SCALE
        {
          SumModSQ += modsq ;
        }
#endif		
	      } 
	  } 
      }
    maxModSQOut = MaxModSQ ;
   	BegLogLine(PKFXLOG_FFT_SCALE)
     << "MaxModSQ FFT = " << MaxModSQ 
     << " SumModSQ = " << SumModSQ
	 << EndLogLine ; 
	 
	BegLogLine(PKFXLOG_FFT_SCALE) 
      << "PackToInteger scaling "
      << (remax * scaleFactorOut / MAXINT )
      << " " << (immax * scaleFactorOut / MAXINT )
      << EndLogLine ; 
      
    if ( ( remax * scaleFactorOut > MAXINT ) 
        || (immax * scaleFactorOut > MAXINT ) )
    {
    	BegLogLine(1)
    	  << "FFT PackToInteger tried to produce " << (remax * scaleFactorOut)
    	  << " " << (immax * scaleFactorOut )
    	  << " with scaleFactor=" << scaleFactorOut
    	  << EndLogLine ;
    	PLATFORM_ABORT("FFT PackToInteger overflowed ") ; 
    	
    }        
  }
  inline void ReorderDoFFTsDoubleInteger_R(int dir, int *srcStride, int *trgStride,
			      int *localSize,
			      complexTemplate<T>* localDataSrc, 
			      complexInteger* localDataIntegerTrg, 
			      FFTOneD<T> *fft, 
			      double scaleFactorOut,
			      double& maxModSQOut)
  {
    int localNx = localSize[0];
    int localNy = localSize[1];
    int localNz = localSize[2];  
    
#if !defined(TMP_IN_FREESTORE)    
  complexTemplate<T> _tmpSrc[localNz*localNy];
  complexTemplate<T> _tmpTrg[localNz*localNy];
#endif    
    int trgStride0 = trgStride[0];
    int trgStride1 = trgStride[1];
    int trgStride2 = trgStride[2];
    
    double MaxModSQ = 0.0 ; 
    double SumModSQ = 0.0 ;
    
    double remax=0.0 ; 
    double immax = 0.0 ; 
    
    int nFFTs = _nFFTs[dir];
    
    //  FFTOneD *fft = _fft[dir];
    int trgIndex=0, srcIndex=0;
    
    int counter = 0;
    
    int ***srcIndexD =  _trgIndex[dir];  
    for(int i=0; i<localNx; i++)
      {
	for(int j=0; j<localNy; j++)
	  { 
	    for(int k=0; k<localNz; k++)
	      {
		srcIndex = srcIndexD[i][j][k];
		
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(srcIndex == _trgIndex[dir][i][j][k]);
		assert(k+j*localNz < localNz*localNy);
		
		_tmpSrc[k+j*localNz].re = localDataSrc[srcIndex].re;
		_tmpSrc[k+j*localNz].im = localDataSrc[srcIndex].im;	      
		
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
		double re=_tmpTrg[k+j*localNz].re ;
		double im=_tmpTrg[k+j*localNz].im ;
		localDataIntegerTrg[trgIndex].re = ScaleToInteger(re,scaleFactorOut);
		localDataIntegerTrg[trgIndex].im = ScaleToInteger(im,scaleFactorOut);	      
		remax=MaxAbs(remax,re) ;
		immax=MaxAbs(immax,im) ;
        double modsq = re*re + im*im ;
        MaxModSQ=fsel(modsq-MaxModSQ,modsq,MaxModSQ) ;
		BegLogLine(PKFXLOG_FFT_SCALE_DETAIL)
		  << "scaling " << re << " " << remax
		  << " " << im << " " << immax 
          << EndLogLine ;

#if PKFXLOG_FFT_SCALE
        {
          SumModSQ += modsq ;
        }
#endif		
	      } 
	  } 
      }
    maxModSQOut = MaxModSQ ;
   	BegLogLine(PKFXLOG_FFT_SCALE)
     << "MaxModSQ FFT = " << MaxModSQ 
     << " SumModSQ = " << SumModSQ
	 << EndLogLine ; 
	 
	BegLogLine(PKFXLOG_FFT_SCALE) 
      << "PackToInteger scaling "
      << (remax * scaleFactorOut / MAXINT )
      << " " << (immax * scaleFactorOut / MAXINT )
      << EndLogLine ; 
    if ( ( remax * scaleFactorOut > MAXINT ) 
        || (immax * scaleFactorOut > MAXINT ) )
    {
    	BegLogLine(1)
    	  << "FFT PackToInteger tried to produce " << (remax * scaleFactorOut)
    	  << " " << (immax * scaleFactorOut )
    	  << " with scaleFactor=" << scaleFactorOut
    	  << EndLogLine ;
    	PLATFORM_ABORT("FFT PackToInteger overflowed ") ; 
    	
    }        
  }
#endif
  inline void DoFwdFFT(complexTemplate<T>* bglfftIn, complexTemplate<T>* bglfftOut)
  {
    IsValidChecksum();
    
    _localDataIn=bglfftIn;
    _localDataOut=bglfftOut;
    
    double Peak0 ; 
    double Peak1 ; 
    double Peak2 ; 
    int dir=0;
    Redistribute(dir,
		 _sendCount[dir],
		 _sendDisp[dir], 
		 _localDataIn,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataTmp2);
    
    IsValidChecksum();
    
    ReorderDoFFTs(dir,
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataTmp2,
		  _localDataTmp1, 
		  _fft[dir],
		  Peak0);
    dir=1;
    IsValidChecksum();
    
    Redistribute(dir,
		 _sendCount[dir], 
		 _sendDisp[dir],
		 _localDataTmp1,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataOut);
    
   IsValidChecksum();
   ReorderDoFFTs(dir, 
		 _strideSrcA[dir],
		 _strideTrgA[dir],
		 _localSizeA[dir],
		 _localDataOut,
		 _localDataTmp2, 
		 _fft[dir],
		 Peak1);
   dir=2;
   
   IsValidChecksum();
   Redistribute(dir,
		_sendCount[dir],
		_sendDisp[dir],
		_localDataTmp2,
		_recvCount[dir], 
		_recvDisp[dir],
		_localDataTmp1);
   
   IsValidChecksum();
   ReorderDoFFTs(dir, _strideSrcA[dir],
		 _strideTrgA[dir], 
		 _localSizeA[dir],
		 _localDataTmp1,
		 _localDataOut, 
		 _fft[dir],
		 Peak2);
   IsValidChecksum();
   if ( Peak0 > localPeak[0] ) localPeak[0] = Peak0 ; 
   if ( Peak1 > localPeak[1] ) localPeak[1] = Peak1 ; 
   if ( Peak2 > localPeak[2] ) localPeak[2] = Peak2 ; 
  }
  


 inline void DoFwdFFT()
  {
    IsValidChecksum();
    
    double Peak0 ; 
    double Peak1 ; 
    double Peak2 ; 
    
    int dir=0;
    Redistribute(dir,
		 _sendCount[dir],
		 _sendDisp[dir], 
		 _localDataIn,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataTmp2);
    
    IsValidChecksum();
    
    ReorderDoFFTs(dir,
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataTmp2,
		  _localDataTmp1, 
		  _fft[dir],
		  Peak0);
    dir=1;
    IsValidChecksum();
    
    Redistribute(dir,
		 _sendCount[dir], 
		 _sendDisp[dir],
		 _localDataTmp1,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataOut);
    
   IsValidChecksum();
   ReorderDoFFTs(dir, 
		 _strideSrcA[dir],
		 _strideTrgA[dir],
		 _localSizeA[dir],
		 _localDataOut,
		 _localDataTmp2, 
		 _fft[dir],
		 Peak1);
   dir=2;
   
   IsValidChecksum();
   Redistribute(dir,
		_sendCount[dir],
		_sendDisp[dir],
		_localDataTmp2,
		_recvCount[dir], 
		_recvDisp[dir],
		_localDataTmp1);
   
   IsValidChecksum();
   ReorderDoFFTs(dir, _strideSrcA[dir],
		 _strideTrgA[dir], 
		 _localSizeA[dir],
		 _localDataTmp1,
		 _localDataOut, 
		 _fft[dir],
		 Peak2);
   IsValidChecksum();
   if ( Peak0 > localPeak[0] ) localPeak[0] = Peak0 ; 
   if ( Peak1 > localPeak[1] ) localPeak[1] = Peak1 ; 
   if ( Peak2 > localPeak[2] ) localPeak[2] = Peak2 ; 
   
  }
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)   




  inline void DoFwdFFTIntegerComms()
  {
  	BegLogLine(PKFXLOG_FFT_SCALE) 
  	  << "DoFwdFFTIntegerComms"
  	  << EndLogLine ;
//  	double scale0 = ( (1<<30) / sqrt(FFTSCALE0) ) * 2.0 ; 
//  	double rscale0 = 1.0 / scale0 ; 
//  	
//  	double scale1 = ( (1<<30) / sqrt(FFTSCALE1) ) * 2.0 ;
//  	double rscale1 = 1.0/scale1 ;
//  	
//  	double scale2 = ( (1<<30) / sqrt(FFTSCALE2) ) * 2.0 ; 
//  	double rscale2 = 1.0/scale2 ;
  	
  	if ( sequenceNumber >= nextGlobalRevision )
  	{
  		nextGlobalRevision += 8 ;
  		double globalPeak[2*(DIM-1)] ;
        (*_comm).AllMaxDouble((void*)localPeak,(void*)globalPeak,2*(DIM-1) ) ;
        
        for ( int dim=0; dim<DIM-1;dim+=1)
        {
        	forwardScale[dim]=(1<<30) / sqrt(globalPeak[dim]) ;
        	reverseScale[dim]=(1<<30) / sqrt(globalPeak[dim+DIM-1]) ;
        	forwardRScale[dim] = 1.0/forwardScale[dim] ;
        	reverseRScale[dim] = 1.0/reverseScale[dim] ;
        	BegLogLine(PKFXLOG_FFT_SCALE)
        	  "Dimension " << dim 
        	  << " globalPeak " << globalPeak[dim] 
        	  << " " << globalPeak[dim+DIM-1]
        	  << " forward scale " << forwardScale[dim] 
        	  << " reverse scale " << reverseScale[dim]
        	  << EndLogLine ;
        }
  		
  	}
  	sequenceNumber += 1 ;
  	
    double Peak0 ; 
    double Peak1 ; 
    double Peak2 ; 
  	
   IsValidChecksum();

    int dir=0;
    PackToInteger( dir, 
			   _sendCount[dir], 
			   _sendDisp[dir],
			   _localDataIn,
			   _localDataIntegerIn,
			   forwardScale[0] ) ;
    RedistributeInteger(dir,
		 _sendCount[dir],
		 _sendDisp[dir], 
		 _localDataIntegerIn,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataIntegerTmp2 );

//   IsValidChecksum();
    
//    ReorderDoFFTsInteger(dir,
//		  _strideSrcA[dir],
//		  _strideTrgA[dir],
//		  _localSizeA[dir],
//		  _localDataIntegerTmp2,
//		  _localDataTmp1, 
//		  _fft[dir],
//		  rscale0 );
//    dir=1;
//   IsValidChecksum();
//    
//    PackToInteger(dir, 
//			   _sendCount[dir], 
//			   _sendDisp[dir],
//			   _localDataTmp1,
//			   _localDataIntegerTmp1,
//			   scale1 ) ;
    ReorderDoFFTsIntegerInteger(dir,
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataIntegerTmp2,
		  _localDataIntegerTmp1, 
		  _fft[dir],
		  forwardRScale[0],
		  forwardScale[1],Peak0 );
    
    dir=1;
    RedistributeInteger(dir,
		 _sendCount[dir], 
		 _sendDisp[dir],
		 _localDataIntegerTmp1,
		 _recvCount[dir],
		 _recvDisp[dir],
		 _localDataIntegerOut);

//    ReorderDoFFTsInteger(dir, 
//		  _strideSrcA[dir],
//		  _strideTrgA[dir],
//		  _localSizeA[dir],
//		  _localDataIntegerOut,
//		  _localDataTmp2,
//		  _fft[dir],
//		  rscale1);
//    dir=2;
//   PackToInteger(dir, 
//			   _sendCount[dir], 
//			   _sendDisp[dir],
//			   _localDataTmp2,
//			   _localDataIntegerTmp2,
//			   scale2) ; 
    ReorderDoFFTsIntegerInteger(dir, 
		  _strideSrcA[dir],
		  _strideTrgA[dir],
		  _localSizeA[dir],
		  _localDataIntegerOut,
		  _localDataIntegerTmp2,
		  _fft[dir],
		  forwardRScale[1],
		  forwardScale[2],Peak1);
    dir=2;
    
//   IsValidChecksum();
    RedistributeInteger(dir,
		 _sendCount[dir],
		 _sendDisp[dir],
		 _localDataIntegerTmp2,
		 _recvCount[dir], 
		 _recvDisp[dir],
		 _localDataIntegerTmp1);
    
//   IsValidChecksum();
    ReorderDoFFTsIntegerDouble(dir,
          _strideSrcA[dir],
		  _strideTrgA[dir], 
		  _localSizeA[dir],
		  _localDataIntegerTmp1,
		  _localDataOut,
		  _fft[dir],
		  forwardRScale[2],Peak2);
   IsValidChecksum();
   if ( Peak0 > localPeak[0] ) localPeak[0] = Peak0 ; 
   if ( Peak1 > localPeak[1] ) localPeak[1] = Peak1 ; 
   if ( Peak2 > localPeak[2] ) localPeak[2] = Peak2 ; 
   
  }
#endif
  
  
  inline  void DoRevFFT(complexTemplate<T>* bglfftIn,complexTemplate<T>*  bglfftOut)
  {
  	 BegLogLine(PKFXLOG_FFT_SCALE) 
  	  << "DoRevFFT"
  	  << EndLogLine ;
  	
    double Peak0 ; 
    double Peak1 ; 
    double Peak2 ; 
    IsValidChecksum();
    int  dir=2;      
    _localDataIn  = bglfftIn;
    _localDataOut = bglfftOut;
    
    ReorderDoFFTs_R(dir, _strideTrgA[dir], 
		    _strideSrcA[dir],
		    _localSizeA[dir],
		    _localDataIn,
		    _localDataTmp2, _fft[dir],Peak0);
    
   IsValidChecksum();
   Redistribute(dir,
		_recvCount[dir], 
		 _recvDisp[dir],
		_localDataTmp2,		   
		_sendCount[dir],
		 _sendDisp[dir],
		_localDataTmp1);
   
   IsValidChecksum();
    dir=1;  
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp1, _localDataOut, _fft[dir],Peak1);
    
    IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataOut,
		 _sendCount[dir], _sendDisp[dir], _localDataTmp2);
    
   IsValidChecksum();
   dir=0;   
   ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		   _localDataTmp2, _localDataTmp1, _fft[dir],Peak2);
   
   IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataTmp1,
		 _sendCount[dir], _sendDisp[dir], _localDataOut);
   if ( Peak0 > localPeak[3] ) localPeak[3] = Peak0 ; 
   if ( Peak1 > localPeak[4] ) localPeak[4] = Peak1 ; 
   if ( Peak2 > localPeak[5] ) localPeak[5] = Peak2 ; 
  }
  /////////
  inline  void DoRevFFT()
  {
  	 BegLogLine(PKFXLOG_FFT_SCALE) 
  	  << "DoRevFFT"
  	  << EndLogLine ;
  	
    double Peak0 ; 
    double Peak1 ; 
    double Peak2 ; 
    IsValidChecksum();
    int  dir=2;      
    
    ReorderDoFFTs_R(dir, _strideTrgA[dir], 
		    _strideSrcA[dir],
		    _localSizeA[dir],
		    _localDataIn,
		    _localDataTmp2, _fft[dir],Peak0);
    
   IsValidChecksum();
   Redistribute(dir,
		_recvCount[dir], 
		 _recvDisp[dir],
		_localDataTmp2,		   
		_sendCount[dir],
		 _sendDisp[dir],
		_localDataTmp1);
   
   IsValidChecksum();
    dir=1;  
    ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataTmp1, _localDataOut, _fft[dir],Peak1);
    
    IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataOut,
		 _sendCount[dir], _sendDisp[dir], _localDataTmp2);
    
   IsValidChecksum();
   dir=0;   
   ReorderDoFFTs_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		   _localDataTmp2, _localDataTmp1, _fft[dir],Peak2);
   
   IsValidChecksum();
    Redistribute(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataTmp1,
		 _sendCount[dir], _sendDisp[dir], _localDataOut);
   if ( Peak0 > localPeak[3] ) localPeak[3] = Peak0 ; 
   if ( Peak1 > localPeak[4] ) localPeak[4] = Peak1 ; 
   if ( Peak2 > localPeak[5] ) localPeak[5] = Peak2 ; 
  }
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)    
  
  
  inline  void DoRevFFTIntegerComms()
  {
  	BegLogLine(PKFXLOG_FFT_SCALE) 
  	  << "DoRevFFTIntegerComms"
  	  << EndLogLine ;
  	
//  	double scale0 = ( (1<<30) / sqrt(FFTSCALE3) ) * 2.0 ; 
//  	double rscale0 = 1.0 / scale0 ; 
//  	
//  	double scale1 = ( (1<<30) / sqrt(FFTSCALE4) ) * 2.0 ;
//  	double rscale1 = 1.0/scale1 ;
//  	
//  	double scale2 = ( (1<<30) / sqrt(FFTSCALE5) ) * 2.0 ; 
//  	double rscale2 = 1.0/scale2 ;
  	if ( sequenceNumber >= nextGlobalRevision )
  	{
  		nextGlobalRevision += 8 ;
  		double globalPeak[2*(DIM-1)] ;
        (*_comm).AllMaxDouble((void*)localPeak,(void*)globalPeak,2*(DIM-1) ) ;
        
        for ( int dim=0; dim<DIM-1;dim+=1)
        {
        	forwardScale[dim]=(1<<30) / sqrt(globalPeak[dim]) ;
        	reverseScale[dim]=(1<<30) / sqrt(globalPeak[dim+DIM-1]) ;
        	forwardRScale[dim] = 1.0/forwardScale[dim] ;
        	reverseRScale[dim] = 1.0/reverseScale[dim] ;
        	BegLogLine(PKFXLOG_FFT_SCALE)
        	  "Dimension " << dim 
        	  << " globalPeak " << globalPeak[dim] 
        	  << " " << globalPeak[dim+DIM-1]
        	  << " forward scale " << forwardScale[dim] 
        	  << " reverse scale " << reverseScale[dim]
        	  << EndLogLine ;
        	
        }
  		
  	}
  	sequenceNumber += 1 ;
  	
    double Peak0 ; 
    double Peak1 ; 
    double Peak2 ; 
  	
   IsValidChecksum();
    int  dir=2;       
    ReorderDoFFTsDoubleInteger_R(dir, _strideTrgA[dir], 
		    _strideSrcA[dir],
		    _localSizeA[dir],
		    _localDataIn,
		    _localDataIntegerTmp2, _fft[dir],reverseScale[0],Peak0);
//    ReorderDoFFTs_R(dir, _strideTrgA[dir], 
//		    _strideSrcA[dir],
//		    _localSizeA[dir],
//		    _localDataIn,
//		    _localDataTmp2, _fft[dir]);
//    
//    PackToInteger(dir, 
//			   _sendCount[dir], 
//			   _sendDisp[dir],
//			   _localDataTmp2,
//			   _localDataIntegerTmp2,
//			   scale0) ;
//   IsValidChecksum();
    RedistributeInteger(dir,
		 _recvCount[dir], 
		 _recvDisp[dir],
		 _localDataIntegerTmp2,		   
		 _sendCount[dir],
		 _sendDisp[dir],
		 _localDataIntegerTmp1);
    
//   IsValidChecksum();
    dir=1;  
    ReorderDoFFTsIntegerInteger_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataIntegerTmp1, _localDataIntegerOut, _fft[dir],reverseRScale[0],reverseScale[1],Peak1);
    
//    ReorderDoFFTsInteger_R(dir, _strideTrgA[dir],
//		    _strideSrcA[dir], _localSizeA[dir],
//		    _localDataIntegerTmp1, _localDataOut, _fft[dir],rscale0);
//    
//   IsValidChecksum();
//    PackToInteger(dir, 
//			   _sendCount[dir], 
//			   _sendDisp[dir],
//			   _localDataOut,
//			   _localDataIntegerOut,reverseScale[1) ;
   
    RedistributeInteger(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataIntegerOut,
		 _sendCount[dir], _sendDisp[dir], _localDataIntegerTmp2);
      
//   IsValidChecksum();
    dir=0;   
    ReorderDoFFTsIntegerInteger_R(dir, _strideTrgA[dir],
		    _strideSrcA[dir], _localSizeA[dir],
		    _localDataIntegerTmp2, _localDataIntegerTmp1, _fft[dir],reverseRScale[1],reverseScale[2],Peak2);

//    ReorderDoFFTsInteger_R(dir, _strideTrgA[dir],
//		    _strideSrcA[dir], _localSizeA[dir],
//		    _localDataIntegerTmp2, _localDataTmp1, _fft[dir],rscale1);
//
//   IsValidChecksum();
//    PackToInteger(dir, 
//			   _sendCount[dir], 
//			   _sendDisp[dir],
//			   _localDataTmp1,
//			   _localDataIntegerTmp1,scale2) ;
    RedistributeInteger(dir,
		 _recvCount[dir], _recvDisp[dir], _localDataIntegerTmp1,
		 _sendCount[dir], _sendDisp[dir], _localDataIntegerOut);
		 
    UnpackFromInteger(dir, 
			   _recvCount[dir],
			   _recvDisp[dir],
			   _localDataOut,
			   _localDataIntegerOut,reverseRScale[2]) ;
//    Redistribute(dir,
//		 _recvCount[dir], _recvDisp[dir], _localDataTmp1,
//		 _sendCount[dir], _sendDisp[dir], _localDataOut);
   if ( Peak0 > localPeak[3] ) localPeak[3] = Peak0 ; 
   if ( Peak1 > localPeak[4] ) localPeak[4] = Peak1 ; 
   if ( Peak2 > localPeak[5] ) localPeak[5] = Peak2 ; 

  }
#endif
  
  inline void Finalize();
};



#define MAX(i, j) ( (i>j)?i:j )
template <typename T > 
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
  
  // Checksum _recvCount and _recvDisp
  //          _sendCount and _sendDisp
  _checksum = GetChecksum();

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

  int xSize = MAX(MAX(_localSizeA[0][0],_localSizeA[1][0]),_localSizeA[2][0]);
  int ySize = MAX(MAX(_localSizeA[0][1],_localSizeA[1][1]),_localSizeA[2][1]);
  int zSize = MAX(MAX(_localSizeA[0][2],_localSizeA[1][2]),_localSizeA[2][2]);

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
template <typename T> 
void PrivateFFT<T>::Plan( int fwd_rev )
{
  _ab = NULL;
  _ab = new MatrixIndices(_localNx[0]*_pX, _localNy[0]*_pY,_localNz[0]*_pZ,
			  _pX,    _pY,  _pZ,
			  _myPx, _myPy, _myPz); 
  assert(_ab!=NULL);

  int size = MAX(MAX(_pX,_pY),_pZ);
      
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

template <typename T> 
void PrivateFFT<T>::SetSendRecvIndices()
{
  calcLocalIndexOZ(DIR_OZ);
  calcLocalIndexZY(DIR_ZY);
  calcLocalIndexYX(DIR_YX);

  _nFFTs[0] = (_localNx[0]*_localNy[0])/_pZ;
  _nFFTs[1] = (_localNx[0]*_localNz[0])/_pY;
  _nFFTs[2] = (_localNz[0]*_localNy[0])/_pX;
}

template <typename T> 
void PrivateFFT<T>::SetAllToAllIndexInPlane(int aDir, int srcP, int trgP)
{
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
        int  myRank=-1;
        MPI_Comm_rank((*_comm)._cart_comm, &myRank); 
        MPI_Cart_coords((*_comm)._cart_comm, myRank, DIM, globalCoord);
        
        
        if(dir==0) { globalCoord[2]=coords[0]; }
        if(dir==1) {  globalCoord[1]=coords[0],globalCoord[2]=coords[1];}
        if(dir==2) {globalCoord[0]=coords[0], globalCoord[1]=coords[1];}
        
        int globalRank;
        //printf("= = = %d %d %d %d \n",  dir, myPx, myPy, myPz);
	
        (*_comm).getCartRank(&globalCoord[0], &globalRank);	
        _globalSendRank[dir][rank] = globalRank;	
        
        
#ifdef MPI_ALLTOALL
        _sendDisp[dir][rank]    =  2*sendDisp;  
#endif
	
        sendCount               = (xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
	_sendCount[dir][rank]   = 2*sendCount;
        
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
    int myRank=-1;
    MPI_Comm_rank((*_comm)._cart_comm, &myRank);
    MPI_Cart_coords((*_comm)._cart_comm, myRank, DIM, globalCoord);

    if(dir==0) {  globalCoord[2]=coords[0];}
    if(dir==1) { globalCoord[1]=coords[0], globalCoord[2]=coords[1];}
    if(dir==2) {globalCoord[0]=coords[0], globalCoord[1]=coords[1];}
    int globalRank;

	
    (*_comm).getCartRank(&globalCoord[0], &globalRank);	

    _globalRecvRank[dir][rank]=globalRank;	
     recvDisp               += recvCount;
	//where + both

	_recvDisp[dir][rank]   = recvDisp;
#ifdef   MPI_ALLTOALL
        _recvDisp[dir][rank]   = 2*recvDisp; 
#endif
	recvCount               = (xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
	_recvCount[dir][rank]   = 2*recvCount;

#ifdef   MPI_ALLTOALL
// 	if(_recvDisp[dir][rank]> 2*_localNx[0]*_localNy[0]*_localNz[0]+1)
// 	  printf("recv [%d][%d] = %d \n", dir, rank, _recvDisp[dir][rank]);
#endif 
     }

  return;
}

template <typename T> 
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

	
template <typename T> 
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

template <typename T> 
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


template <typename T> 
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


template <typename T> 
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
  int maxLocal        = MAX(_localNx[0], MAX(_localNy[0], _localNz[0]));
  int maxLocalSize    = (MAX(_pX,MAX(_pY, _pZ)))*totalLocal;
  int maxLocalTmpSize = (MAX(_pX,MAX(_pY, _pZ)))*maxLocal;

#if defined(TMP_IN_FREESTORE) 
  _tmpSrc = new complexTemplate<T>[maxLocalTmpSize];        
  _tmpTrg = new complexTemplate<T>[maxLocalTmpSize];  
#endif  
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)
  _localDataIntegerIn   = new complexInteger[totalLocal];
  _localDataIntegerOut  = new complexInteger[totalLocal];
  _localDataIntegerTmp1 = new complexInteger[totalLocal];
  _localDataIntegerTmp2 = new complexInteger[totalLocal];
#endif

//#ifdef  ELEM_BY_ELEM    	
  _localDataIn   = new complexTemplate<T>[totalLocal];
  _localDataOut  = new complexTemplate<T>[totalLocal];
  assert(_localDataIn!=NULL);
  assert(_localDataOut!=NULL);
//#endif 

  _localDataTmp1 = new complexTemplate<T>[totalLocal];
  _localDataTmp2 = new complexTemplate<T>[totalLocal];

#if defined(TMP_IN_FREESTORE)  
  assert(_tmpSrc!=NULL);
  assert(_tmpTrg!=NULL);
#endif  
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)
  assert(_localDataIntegerIn!=NULL);
  assert(_localDataIntegerOut!=NULL);
  assert(_localDataIntegerTmp1!=NULL);
  assert(_localDataIntegerTmp2!=NULL);    
#endif
 
  assert(_localDataTmp1!=NULL);
  assert(_localDataTmp2!=NULL);    
}

template <typename T>   
void PrivateFFT<T>::Finalize()
{
  delete _ab;
  
//#ifdef  ELEM_BY_ELEM    	
  delete[] _localDataIn;
  delete[] _localDataOut;
  _localDataIn = NULL;
  _localDataOut = NULL;  
  assert(_localDataIn == NULL);
  assert( _localDataOut == NULL); 
//#endif
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)
  delete[] _localDataIntegerIn ;
  delete[] _localDataIntegerOut ;
  delete[] _localDataIntegerTmp1 ;
  delete[] _localDataIntegerTmp2 ;
#endif  
  
  delete[] _localDataTmp1;
  delete[] _localDataTmp2;
  
#if defined(TMP_IN_FREESTORE)
  delete[] _tmpSrc;
  delete[] _tmpTrg;
#endif  
  
  _localDataTmp1 = NULL;
  _localDataTmp2 = NULL;

  assert( _localDataTmp1 == NULL);
  assert( _localDataTmp2 == NULL);
 
#if !defined(SKIP_SCALED_INTEGER_FFT_COMMS)
  _localDataIntegerIn = NULL ;
  _localDataIntegerOut = NULL ;
  _localDataIntegerTmp1 = NULL ;
  _localDataIntegerTmp2 = NULL ;
#endif  

#if defined(TMP_IN_FREESTORE)  
  _tmpSrc = NULL; _tmpTrg = NULL;
  assert( _tmpSrc == NULL);
  assert( _tmpTrg == NULL);
#endif
  
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






#endif



