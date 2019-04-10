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
#include <fft3D.hpp>
#include <math.h>


#define N_SIZE 128
#define MESS_SIZE 8

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
#define PK_FORWARD -1
#define PK_REVERSE 1

//template
//  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double>::Init(BGL3DFFT** ptr, const int& globalNx, const int& globalNy, const int& globalNz);  
template
  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::Init(BGL3DFFT** ptr, const int& globalNx, const int& globalNy, const int& globalNz, 
	    int subPx, int subPy, int subPz);

//template
//  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double>::Init(const int& localNx, const int& localNy, const int& localNz, const int& pX, const int& pY, const int& pZ);
template
  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::Init(const int& localNx, const int& localNy, const int& localNz, const int& pX, const int& pY, const int& pZ,
	    int subPx, int subPy, int subPz, int compress); 

template
  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::Finalize();

template
  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::ZeroRealSpace();
  
  // Function calls to retrive data element by element.

template
  inline double BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::GetRealSpaceElement(const int x, const int y, const int z ) const;
template
  inline void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::PutRealSpaceElement(const int x, const int y, const int z, double realInput);
template
  inline COMPLEXDOUBLE BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::GetRecipSpaceElement(const int kx,const int ky, const int kz) const;
template
  inline void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::PutRecipSpaceElement( int kx, int ky, int kz,COMPLEXDOUBLE value);
  
  // Returns the original and the transform data distributions
template
  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
			   int& aLocalY, int& aLocalSizeY,
			   int& aLocalZ, int& aLocalSizeZ,
			   int& aLocalRecipX, int& aLocalRecipSizeX,
			   int& aLocalRecipY, int& aLocalRecipSizeY,
			   int& aLocalRecipZ, int& aLocalSizeRecipZ );
  
template
  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::DoFFT(COMPLEXDOUBLE* bglfftIn, COMPLEXDOUBLE* bglfftOut) ; 
template
  void BGL3DFFT<FFT_PLAN, PK_FORWARD,double, COMPLEXDOUBLE>::DoFFT();

//template
//  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double>::Init(BGL3DFFT** ptr, const int& globalNx, const int& globalNy, const int& globalNz);  
template
  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::Init(BGL3DFFT** ptr, const int& globalNx, const int& globalNy, const int& globalNz, 
	    int subPx, int subPy, int subPz);

//template
//  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double>::Init(const int& localNx, const int& localNy, const int& localNz, const int& pX, const int& pY, const int& pZ);
template
  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::Init(const int& localNx, const int& localNy, const int& localNz, const int& pX, const int& pY, const int& pZ,
	    int subPx, int subPy, int subPz, int compress); 

template
  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::Finalize(); 

template
  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::ZeroRealSpace();
  
  // Function calls to retrive data element by element.

template
  inline double BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::GetRealSpaceElement(const int x, const int y, const int z ) const;
template
  inline void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::PutRealSpaceElement(const int x, const int y, const int z, double realInput);
template
  inline COMPLEXDOUBLE BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::GetRecipSpaceElement(const int kx,const int ky, const int kz) const;
template
  inline void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::PutRecipSpaceElement( int kx, int ky, int kz,COMPLEXDOUBLE value);
  
  // Returns the original and the transform data distributions
template
  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
			   int& aLocalY, int& aLocalSizeY,
			   int& aLocalZ, int& aLocalSizeZ,
			   int& aLocalRecipX, int& aLocalRecipSizeX,
			   int& aLocalRecipY, int& aLocalRecipSizeY,
			   int& aLocalRecipZ, int& aLocalSizeRecipZ );
  
template
  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::DoFFT(COMPLEXDOUBLE* bglfftIn, COMPLEXDOUBLE* bglfftOut) ; 
template
  void BGL3DFFT<FFT_PLAN, PK_REVERSE,double, COMPLEXDOUBLE>::DoFFT();

//template void BGL3DFFT<FFT_PLAN, PK_FORWARD,double>::DoFFT() ; 
//template void BGL3DFFT<FFT_PLAN, PK_REVERSE,double>::DoFFT() ; 
//template void BGL3DFFT<FFT_PLAN, PK_FORWARD,double>::DoFFT(complexTemplate<double>*, complexTemplate<double>*) ; 
//template void BGL3DFFT<FFT_PLAN, PK_REVERSE,double>::DoFFT(complexTemplate<double>*, complexTemplate<double>*) ; 
//template void BGL3DFFT<FFT_PLAN, PK_FORWARD,double>::Init( const int & globalNx, const int & globalNy, const int & globalNz, const int & pX, const int & pY, const int & pZ) ; 
//template void BGL3DFFT<FFT_PLAN, PK_REVERSE,double>::Init( const int & globalNx, const int & globalNy, const int & globalNz, const int & pX, const int & pY, const int & pZ) ; 


