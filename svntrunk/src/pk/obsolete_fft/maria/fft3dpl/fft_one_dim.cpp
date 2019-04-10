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
 #include <iostream>
using namespace std;
#include <cassert>
#include <fft_one_dim.hpp>

///// #include "fftw.h"

//template <int SIZE, int DIRECTION>
FFTOneD::~FFTOneD() 
{
#ifdef USE_FFTW_ONED
  fftw_destroy_plan(_plan);
#endif
}

///template <int SIZE, int DIRECTION>
void  FFTOneD::fftInternal ( complex *in, 
                             complex *out ) 
{
  //#ifdef USE_FFTW_ONED
  fftw_one(_plan, (fftw_complex *) in, (fftw_complex *)out); 
  //#else
   
  complex temp[ 4 ];

  memcpy( temp, in, sizeof(complex) * 4 );

  fft4 fftf1( temp );
  if( mDirection == FFTOneD::FORWARD )
    fftf1.inline_forward();
  else if( mDirection == FFTOneD::REVERSE )
    fftf1.inline_inverse();

  temp[ 0 ].re *= 2; 
  temp[ 1 ].re *= 2; 
  temp[ 2 ].re *= 2; 
  temp[ 3 ].re *= 2; 

  temp[ 0 ].im *= 2; 
  temp[ 1 ].im *= 2; 
  temp[ 2 ].im *= 2; 
  temp[ 3 ].im *= 2; 

  for(int i=0; i < 4; i++) 
    {
      if(( abs( out[ i ].re - temp[ i ].re ) > 0.000001 ) ||
         ( abs( out[ i ].im - temp[ i ].im ) > 0.000001 ) )
        {
          
          printf("in[0]= (%f,%f)\n", in[ 0 ].re, in[ 0 ].im);
          printf("in[1]= (%f,%f)\n", in[ 1 ].re, in[ 1 ].im);
          printf("in[2]= (%f,%f)\n", in[ 2 ].re, in[ 2 ].im);
          printf("in[3]= (%f,%f)\n", in[ 3 ].re, in[ 3 ].im);

          printf("temp[0]= (%f,%f)\n", temp[ 0 ].re, temp[ 0 ].im);
          printf("temp[1]= (%f,%f)\n", temp[ 1 ].re, temp[ 1 ].im);
          printf("temp[2]= (%f,%f)\n", temp[ 2 ].re, temp[ 2 ].im);
          printf("temp[3]= (%f,%f)\n", temp[ 3 ].re, temp[ 3 ].im);
          
          printf("out[0]= (%f,%f)\n", out[ 0 ].re, out[ 0 ].im);
          printf("out[1]= (%f,%f)\n", out[ 1 ].re, out[ 1 ].im);
          printf("out[2]= (%f,%f)\n", out[ 2 ].re, out[ 2 ].im);
          printf("out[3]= (%f,%f)\n", out[ 3 ].re, out[ 3 ].im);         
          exit( 1 );
        }            
    }   
  //#endif
}

//template <int SIZE, int DIRECTION>
FFTOneD::FFTOneD(unsigned int vectorSz,
		 short int fftDir,
		 unsigned int numOfFFTs):
  _vectorSz(vectorSz)
{

   assert(fftDir!=1||fftDir!=-1);
   assert(vectorSz>= 0);
   _numOfFFTs=numOfFFTs;
      
   mDirection = fftDir;

#ifdef USE_FFTW_ONED 
   if( fftDir == FORWARD ) 
     {
       _fftw_direction = FFTW_FORWARD;
     }
   else if( fftDir == REVERSE ) 
     {
       _fftw_direction = FFTW_BACKWARD;
     }
   
   //  cout<< " fft_direction " << _fftw_direction
   //      << " vectorSz    " << _vectorSz << endl;
   _plan = fftw_create_plan(_vectorSz, _fftw_direction, FFTW_MEASURE);
   assert( _plan != NULL );
#endif
}

//template <int SIZE, int DIRECTION>
void FFTOneD::printFFT(complex *out) 
{
#ifdef USE_FFTW_ONED
  fftw_complex *output = (fftw_complex*) out;
  
  for(int i=0; i<_vectorSz; i++) 
    {
      cout<< " FFT(vector[ " << i << " ]) = " 
          << " (" <<output[i].re << " , "<< output[i].im <<")" << endl;
    }
#endif
} 

