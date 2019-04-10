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
 //  
//  IBM T.J.W  R.C.
//  Date : 24/01/2003
//  Name : Maria Eleftheriou
//  Last Modified on: 09/24/03 by Maria Eleftheriou
//  fftmain.hpp
//  tests the 3D fft using MPI and fftw 


#if defined(USE_PK_ACTORS)
#include <BonB/BGL_PartitionSPI.h>
#include <rts.h>
#include <Pk/API.hpp>
#include <Pk/Compatibility.hpp>
#include <stdio.h>
#include <BonB/BGLTorusAppSupport.h>
#include <BonB/BGLTorusAppSupport.c>

#else
#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#endif
#include <BlueMatter/complex.hpp>
//#include <assert.h>
#include <fft3D.hpp>
//#include <stdlib.h>
//#include <string.h>
////#include <iostream.h>
#include <math.h>
// #include <fft3d/timer_MPI.hpp>

#define TYPE double 

#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))
#define TOLERANCE  (1./1000000000.)
#define N_SIZE 128
#define MESS_SIZE 8
#define ITERATIONS 10

#define REGRESS_REVERSE

#ifndef PKFXLOG_FFTMAIN
#define PKFXLOG_FFTMAIN (0)
#endif

#ifndef PKFXLOG_FFTINTER
#define PKFXLOG_FFTINTER (0)
#endif

#ifndef PKFXLOG_FFTITERATION
#define PKFXLOG_FFTITERATION (0) 
#endif

struct FFT_PLAN
{
  enum
  {
    P_X = MESS_SIZE,
    P_Y = MESS_SIZE,
    P_Z = MESS_SIZE 
  };
  enum
  {
    GLOBAL_SIZE_X = N_SIZE,
    GLOBAL_SIZE_Y = N_SIZE,
    GLOBAL_SIZE_Z = N_SIZE
  };
};

// CHECK needs to be removed
#define FORWARD -1
#define REVERSE 1

typedef  FFT3D< FFT_PLAN, FORWARD, TYPE > FFT_FORWARD FFT3D_STUCT_ALIGNMENT;
typedef  FFT3D< FFT_PLAN, REVERSE, TYPE > FFT_REVERSE FFT3D_STUCT_ALIGNMENT;

void forward_init(FFT_FORWARD &aFFT
                , int globalNx
                , int globalNy
                , int globalNz
                , int pX
                , int pY
                , int pZ
                )
{
	aFFT.Init(globalNx,globalNy,globalNz,pX,pY,pZ) ;
}
                
void reverse_init(FFT_REVERSE &aFFT
                , int globalNx
                , int globalNy
                , int globalNz
                , int pX
                , int pY
                , int pZ
                )
{
	aFFT.Init(globalNx,globalNy,globalNz,pX,pY,pZ) ;
}                

void forward_do(FFT_FORWARD &aFFT, complexTemplate<TYPE>* in, complexTemplate<TYPE>* out)
{
	aFFT.DoFFT(in, out) ; 
}

void reverse_do(FFT_REVERSE &aFFT, complexTemplate<TYPE>* in, complexTemplate<TYPE>* out)
{
	aFFT.DoFFT(in, out) ; 
}

