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
//  Date : 05/30/2006
//  Name : Maria Eleftheriou
//  Last Modified: 
//  fft_one_dim_MPI.hpp
//

#ifndef __FFTONEDIM_H_
#define __FFTONEDIM_H_

#include <math.h>
#include <assert.h>
#include <pk/pktrace.hpp>
#include <BlueMatter/complex.hpp>

// #include <MPI/complex_MPI.hpp>

#ifdef PK_BGL
#ifdef ENABLE_FLOAT_BGLFFT 
#include "/bgl/local/bglfftwgel-2.1.5/include/sfftw.h"
#else
#include "/bgl/local/bglfftwgel-2.1.5/include/fftw.h"
#endif
#else 
#include <fftw.h>
#endif

//static TraceClient TR_FFT_INTERNAL_Start;
//static TraceClient TR_FFT_INTERNAL_Finis;

#if !defined(PKFXLOG_FFT_ONED)
#define PKFXLOG_FFT_ONED (0) 
#endif

template <class T, class Tcomplex> 
class FFTOneD {
private:
  unsigned int   _vectorSz;  // size of the 1D-FFT
  unsigned int   _numOfFFTs; // how many 1D-FFTs
  short int      _fftDir;
  
  fftw_plan      _plan;
  fftw_direction _fftw_direction;
  
public:
  inline FFTOneD(unsigned int vectorSz, short int fftDir, unsigned int numOfFFTs = 1);
  inline ~FFTOneD();
  inline void fftInternal(Tcomplex* globalIn, Tcomplex* globalOut);

  // For debugging...
  inline void printFFT(Tcomplex *in);  
};

template <class T, class Tcomplex> 
FFTOneD<T, Tcomplex>::~FFTOneD() 
{
  fftw_destroy_plan(_plan);
}

template <class T, class Tcomplex> 
void  FFTOneD<T, Tcomplex>::fftInternal(Tcomplex *in, 
            Tcomplex *out) 
{
//  TR_FFT_INTERNAL_Start.HitOE( PKTRACE_TUNE,
//                              "TR_FFT_INTERNAL_Start", 
//                              Platform::Thread::GetId(),
//                              TR_FFT_INTERNAL_Start );
  Tcomplex diagnosticStackVariable ;                              
  BegLogLine(PKFXLOG_FFT_ONED)                               
    << "fftInternal in=0x" << (void *) in
    << " out=0x" << (void *) out 
    << " &diagnosticStackVariable=0x" << (void *) &diagnosticStackVariable
    << EndLogLine ;
  fftw_one(_plan, (fftw_complex *) in, (fftw_complex *)out); 
//  TR_FFT_INTERNAL_Finis.HitOE( PKTRACE_TUNE,
//                              "TR_FFT_INTERNAL_Finis", 
//                              Platform::Thread::GetId(),
//                              TR_FFT_INTERNAL_Finis );
  
}

template <class T, class Tcomplex> 
FFTOneD<T, Tcomplex>::FFTOneD(unsigned int vectorSz,
        short int fftDir,
        unsigned int numOfFFTs):
  _vectorSz(vectorSz), _fftDir(fftDir), _numOfFFTs(numOfFFTs)
{
  assert(((_fftDir==1)||(_fftDir==-1)) &&(_vectorSz>=0));
  
  _fftw_direction = (fftDir==-1)? FFTW_FORWARD:FFTW_BACKWARD;
    BegLogLine(PKFXLOG_FFT_ONED)
    << "FFTOneD size=" << _vectorSz
    << " direction=" << _fftw_direction
    << EndLogLine ; 
 
  _plan = fftw_create_plan(_vectorSz, _fftw_direction, FFTW_ESTIMATE);
    BegLogLine(PKFXLOG_FFT_ONED)
    << "FFTOneD plan=" << (void *) _plan
    << EndLogLine ; 
  
}

template <class T, class Tcomplex>
void FFTOneD<T, Tcomplex>::printFFT(Tcomplex *out) 
{
  fftw_complex *output = (fftw_complex*) out;
} 

#endif
