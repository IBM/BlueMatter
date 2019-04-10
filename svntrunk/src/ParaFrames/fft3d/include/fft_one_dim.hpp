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
 #ifndef __FFTONED_H_
#define __FFTONED_H_

/////#include <iostream.h>
#include <assert.h>
#include <math.h>
#include <MPI/complex.hpp>

#ifdef USE_FFTW_ONE_DIM
#include <MPI/fftw.h>
#else

//#if defined( USE_DOUBLE_HUMMER_1D_FFT )
#include <MPI/fft64a.hpp>
#include <MPI/fft128a.hpp>
//#else
#include <MPI/fft4.hpp>
#include <MPI/fft64.hpp>
#include <MPI/fft32a.hpp>
#include <MPI/fft128.hpp>
//#endif

#endif

#define FORWARD -1
#define REVERSE  1

#ifndef PKFXLOGGER_FFTW_ONE_DIM
#define PKFXLOGGER_FFTW_ONE_DIM ( 0 )
#endif

#ifdef PK_BGL
void memcpy(fft3d_complex* trg, fft3d_complex* src, int num)
{
  for(int i=0; i < num; i++)
    {
      trg[ i ] = src[ i ];
    }
}

#endif

template < int FFT_SIZE, int FWD_REV >
class FFTOneD
{
private:

  unsigned int mVectorSz;  // size of the 1D-FFT
  unsigned int mNumOfFFTs; // how many 1D-FFTs

#ifdef USE_FFTW_ONE_DIM
  fftw_plan mPlan;
  fftw_direction mFFTw_direction;
#endif

public:
  FFTOneD();// ( unsigned int vectorSz,
//     short int fftDir,
//            unsigned int numOfFFTs = 1);

  ~FFTOneD();

  inline void fftInternal( fft3d_complex *globalIn, fft3d_complex *globalOut );

  void printFFT( fft3d_complex *in );
};

template < int FFT_SIZE, int FWD_REV >
FFTOneD< FFT_SIZE, FWD_REV >::~FFTOneD()
{
#ifdef USE_FFTW_ONE_DIM
  fftw_destroy_plan( mPlan );
#endif
}

template < int FFT_SIZE, int FWD_REV >
inline void  FFTOneD< FFT_SIZE, FWD_REV >::fftInternal( fft3d_complex *in,
                                                        fft3d_complex *out )
{
#if 1
#ifdef USE_FFTW_ONE_DIM

  for( int i=0; i < FFT_SIZE; i++)
    {
      BegLogLine( PKFXLOGGER_FFTW_ONE_DIM )
        << " in[ " << i << " ].re: " << in[ i ].re
        << " in[ " << i << " ].im: " << in[ i ].im
        << EndLogLine;
    }

  fftw_one( mPlan, (fftw_complex *) in, (fftw_complex *) out);

  for( int i=0; i < FFT_SIZE; i++)
    {
      BegLogLine( PKFXLOGGER_FFTW_ONE_DIM )
        << " out[ " << i << "].re: " << out[ i ].re
        << " out[ " << i << "].im: " << out[ i ].im
        << EndLogLine;
    }

#else

#if 0
#ifndef FFT3D_SENDER_COMPUTES
   #ifdef PK_BGL
     memcpy( out, in, FFT_SIZE );
   #else
     memcpy( out, in, sizeof(fft3d_complex) * FFT_SIZE );
   #endif
#endif
#endif

  // NEED: Redesign the below code. But the function is there for now.
    
    BegLogLine( PKFXLOGGER_FFTW_ONE_DIM )
      << "fftInternal() "
      << "About to do a 1D FFT of size=" << FFT_SIZE
      << EndLogLine;
    
   if (FFT_SIZE == 4)
     {
       fft4 one_dim_fft1( in );
       
       if( FWD_REV == FORWARD )
         one_dim_fft1.inline_forward();
       else
         one_dim_fft1.inline_inverse();
       
       fft4 one_dim_fft2( out );
       
       if( FWD_REV == FORWARD )
         one_dim_fft2.inline_forward();
       else
         one_dim_fft2.inline_inverse();
     }   
   else if( FFT_SIZE == 32 )
     {
#if defined( USE_DOUBLE_HUMMER_1D_FFT )
       if( FWD_REV == FORWARD )
         fft32_forward(in, out, in, out);
       else
         fft32_inverse(in, out, in, out);       
#endif
     }
   else if( FFT_SIZE == 64 )
     {

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
        if( FWD_REV == FORWARD )
          fft64_forward(in, out, in, out);
        else
          fft64_inverse(in, out, in, out);

#else

#ifdef FFT3D_SENDER_COMPUTES
       fft64_s one_dim_fft1( in );
       
       if( FWD_REV == FORWARD )
         one_dim_fft1.inline_forward();
       else
         one_dim_fft1.inline_inverse();
#endif
       
       fft64_s one_dim_fft2( out );
       
       if( FWD_REV == FORWARD )
         one_dim_fft2.inline_forward();
       else
         one_dim_fft2.inline_inverse();
#endif       

     }
  else if( FFT_SIZE == 128 )
    {

#if defined( USE_DOUBLE_HUMMER_1D_FFT )
        if( FWD_REV == FORWARD )
          fft128_forward(in, out, in, out);
        else
          fft128_inverse(in, out, in, out);
#else
#ifdef FFT3D_SENDER_COMPUTES
      fft128_s one_dim_fft1( in );
      
      if( FWD_REV == FORWARD )
        one_dim_fft1.inline_forward();
      else
        one_dim_fft1.inline_inverse();
#endif
      
      fft128_s one_dim_fft2( out );
      
      if( FWD_REV == FORWARD )
        one_dim_fft2.inline_forward();
      else
        one_dim_fft2.inline_inverse();

#endif

    }
  else
    {
      printf("ERROR:: FFTSIZE=%d is not supported", FFT_SIZE);
      PLATFORM_ABORT("ERROR:: FFTSIZE");
    }

#endif
#endif
}

template <int FFT_SIZE, int FWD_REV >
FFTOneD< FFT_SIZE, FWD_REV >::FFTOneD()
{
#ifdef USE_FFTW_ONE_DIM
  mPlan = fftw_create_plan( FFT_SIZE, (fftw_direction) FWD_REV, FFTW_MEASURE );
#endif
}

template < int FFT_SIZE, int FWD_REV >
void FFTOneD< FFT_SIZE, FWD_REV >::printFFT(fft3d_complex *out)
{
#ifdef USE_FFTW_ONE_DIM
  fftw_complex *output = (fftw_complex *) out;

  for( int i=0; i < FFT_SIZE; i++ )
    printf( " FFT( vector[ %d ] ) = ( %f, %f )\n", i, output[ i ].re, output[ i ].im );
#endif
}

#endif
