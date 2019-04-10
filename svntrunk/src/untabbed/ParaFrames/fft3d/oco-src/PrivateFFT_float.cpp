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
#include <PrivateFFTTemplate.hpp>
// Explicit instantiations for FFT of 'float' data

template 
void PrivateFFT<float, COMPLEXFLOAT>::Initialize( int fwd_rev ) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::Plan( int fwd_rev ) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::SetSendRecvIndices() ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::SetAllToAllIndexInPlane(int aDir, int srcP, int trgP) ;

template  
void  PrivateFFT<float, COMPLEXFLOAT>::DoLocalMapping(Distribution3DPoint &pointMin,
            Distribution3DPoint &pointMax, 
            int x, int y, int z) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::calcLocalIndexOZ(int dir) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::calcLocalIndexZY(int dir) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::calcLocalIndexYX(int dir) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::AllocateMemory() ;

template    
void PrivateFFT<float, COMPLEXFLOAT>::Finalize() ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::ReorderDoFFTs(int dir,
          int* srcStride, 
          int* trgStride,
          int* localSize,
          COMPLEXFLOAT* localDataSrc,
          COMPLEXFLOAT* localDataTrg,
          FFTOneD<float, COMPLEXFLOAT> *fft) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,int *localSize,
            COMPLEXFLOAT* localDataSrc, COMPLEXFLOAT* localDataTrg, FFTOneD<float, COMPLEXFLOAT>* fft) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::DoFwdFFT(COMPLEXFLOAT* bglfftIn, COMPLEXFLOAT* bglfftOut) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::DoFwdFFT() ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::DoRevFFT(COMPLEXFLOAT* bglfftIn,COMPLEXFLOAT*  bglfftOut) ;

template  
void PrivateFFT<float, COMPLEXFLOAT>::DoRevFFT() ;

