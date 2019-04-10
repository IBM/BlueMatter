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
// Explicit instantiations for FFT of 'double' data

template 
void PrivateFFT<double, COMPLEXDOUBLE>::Initialize( int fwd_rev ) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::Plan( int fwd_rev ) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::SetSendRecvIndices() ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::SetAllToAllIndexInPlane(int aDir, int srcP, int trgP) ;

template  
void  PrivateFFT<double, COMPLEXDOUBLE>::DoLocalMapping(Distribution3DPoint &pointMin,
				    Distribution3DPoint &pointMax, 
				    int x, int y, int z) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::calcLocalIndexOZ(int dir) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::calcLocalIndexZY(int dir) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::calcLocalIndexYX(int dir) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::AllocateMemory() ;

template    
void PrivateFFT<double, COMPLEXDOUBLE>::Finalize() ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::ReorderDoFFTs(int dir,
			    int* srcStride, 
			    int* trgStride,
			    int* localSize,
			    COMPLEXDOUBLE* localDataSrc,
			    COMPLEXDOUBLE* localDataTrg,
			    FFTOneD<double, COMPLEXDOUBLE> *fft) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,int *localSize,
			      COMPLEXDOUBLE* localDataSrc, COMPLEXDOUBLE* localDataTrg, FFTOneD<double, COMPLEXDOUBLE>* fft) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::DoFwdFFT(COMPLEXDOUBLE* bglfftIn, COMPLEXDOUBLE* bglfftOut) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::DoFwdFFT() ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::DoRevFFT(COMPLEXDOUBLE* bglfftIn,COMPLEXDOUBLE*  bglfftOut) ;

template  
void PrivateFFT<double, COMPLEXDOUBLE>::DoRevFFT() ;
