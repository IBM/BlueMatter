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

#ifndef PRIVATE_FFT_HPP
#define PRIVATE_FFT_HPP
#if defined(PK_XLC)
#include <builtins.h>
#endif
#include <pk/pktrace.hpp>
#include <string.h>
#include <stdlib.h>

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

#include <BlueMatter/complex.hpp>


template <typename T, typename Tcomplex>
class PrivateFFT
{

public:
  enum{DIR_OZ, DIR_ZY, DIR_YX};
  enum{DIM=4};
  FFTOneD<T, Tcomplex> *_fft[DIM-1];
  
#ifdef PK_BLADE_SPI
  SpiDRC<T> *_comm;
#else
  MpiDRC<T> *_comm;
#endif

  // data related   
  int _strideXYZ[DIM-1];  
  int _strideR0, _strideR1, _strideR2;
  
  int _nFFTs[DIM-1];
  
  Tcomplex *_input ;
  Tcomplex *_output ;
  
  int _localNx[DIM];
  int _localNy[DIM];
  int _localNz[DIM];
  
  int _strideTrgA[DIM][DIM-1];
  int _strideSrcA[DIM][DIM-1];
  int _localSizeA[DIM][DIM-1];
  int _fwd_rev;  
  int _totalLocalSize[DIM];
  int _pStep[DIM-1];
  
  Tcomplex *_localDataIn ;
  Tcomplex *_localDataOut;
  
  int totalLocal      ; // = step*_localNx[0]*_localNy[0]*_localNz[0]+1;
  
  // Sizes of scratchpads
  int maxLocal        ; // = step*MAX(_localNx[0], MAX(_localNy[0], _localNz[0]));
  int maxLocalSize    ; // = (MAX(_pX,MAX(_pY, _pZ)))*totalLocal;
  int maxLocalTmpSize ; // = (MAX(_pX,MAX(_pY, _pZ)))*maxLocal*2;
  
  int**** _trgIndex;
  int** _sendCount;
  int** _recvCount;
  
  int** _sendDisp;
  int** _recvDisp;

  unsigned int  _checksum;
  
  DistributionOps*** _sendIndex;
  DistributionOps*** _recvIndex;

  int _globalSendRank[DIM-1][ 1024 ]; // this is for 32x32 processor-plane
  int _globalRecvRank[DIM-1][ 1024 ];

  MatrixIndices *_ab;
  int _pX;  int _pY;  int _pZ;
  int _myPx;  int _myPy;  int _myPz,_myRank, _globalID;

  
  // private functions  
  inline int getLocalNx(const int index) const
  {
    return _localNx[index];
  }

  inline int getLocalNy(const int index) const
  {
    return _localNy[index];
  }

  inline int getLocalNz(const int index) const
  {
    return _localNz[index];
  }
  
  inline void setLocalNz(const int index,  int value)
  {
    _localNz[index] = value;
  }

  inline void setLocalNy(const int index,  int value)
  {
    _localNy[index] = value;
  }
  
  inline void setLocalNx(const int index,  int value)
  {
    _localNx[index] = value;
  }
  
  void Initialize( int fwd_rev );
  void Plan( int fwd_rev );
  
  void AllocateMemory();
  inline void DoLocalMapping(Distribution3DPoint &pointMin, Distribution3DPoint &pointMax, int x, int y, int z);
  
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
  
  inline void Redistribute(int dir, 
                           int* sendCount, 
                           int* sendDisp,
                           Tcomplex *localDataSrc,
                           int* recvCount,
                           int* recvDisp,
                           Tcomplex *localDataTrg
                           )
  {
    BegLogLine(PKFXLOG_FFT_SUMMARY) 
      << "Redistribute _comm=0x" << (void *) _comm
      << " &_comm=0x" << (void *) &_comm
      << EndLogLine ;
    
    (*_comm).allToAllv((void*)localDataSrc, sendCount, sendDisp,
                       (void*)localDataTrg, recvCount, recvDisp, dir );
    
    BegLogLine(PKFXLOG_FFT_SUMMARY) 
      << "Redistribute _comm=0x" << (void *) _comm
      << " &_comm=0x" << (void *) &_comm
      << EndLogLine ;
        
  }
  
  inline
  void ReorderDoFFTs(int dir,
                            int* srcStride, 
                            int* trgStride,
                            int* localSize,
                            Tcomplex *localDataSrc,
                            Tcomplex *localDataTrg,
                            FFTOneD<T,Tcomplex> *fft) ;

  inline 
  void ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,int *localSize,
                       Tcomplex* localDataSrc, 
                       Tcomplex* localDataTrg, 
                              FFTOneD<T,Tcomplex>* fft) ; 

  void DoFwdFFT(Tcomplex* bglfftIn, Tcomplex* bglfftOut);
  void DoRevFFT(Tcomplex* bglfftIn, Tcomplex* bglfftOut);
 
  void DoFwdFFT() ;
  void DoRevFFT() ;
  
  void Finalize();
};
#endif
