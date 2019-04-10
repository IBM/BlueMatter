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
 #include "fftw.h"

// #include <pk/platform.hpp>
// #include <pk/fxlogger.hpp>

struct FFT_PLAN1
  {
  enum
    {
    P_X = 1,
    P_Y = 1,
    P_Z = 1
    };

  enum
    {
    GLOBAL_SIZE_X = 64,
    GLOBAL_SIZE_Y = 64,
    GLOBAL_SIZE_Z = 64
    };
  };

fftw_complex src [ FFT_PLAN1::GLOBAL_SIZE_X ][ FFT_PLAN1::GLOBAL_SIZE_Y ][ FFT_PLAN1::GLOBAL_SIZE_Z ];
fftw_complex dst1[ FFT_PLAN1::GLOBAL_SIZE_X ][ FFT_PLAN1::GLOBAL_SIZE_Y ][ FFT_PLAN1::GLOBAL_SIZE_Z ];

int main( int argc, char ** argv, char ** envp )
{
  int globalNx = FFT_PLAN1::GLOBAL_SIZE_X;
  int globalNy = FFT_PLAN1::GLOBAL_SIZE_Y;
  int globalNz = FFT_PLAN1::GLOBAL_SIZE_Z;
  
  for(int x=0; x < globalNx; x++)
    for(int y=0; y < globalNy; y++)
      for(int z=0; z < globalNz; z++)
        {
          double data = 1.0*x*y*z; //(x*GlobalNy+y)*GlobalNz+z;
          //////src[ x ][ y ][ z ] = complex( data, 0.0 );
          src[ x ][ y ][ z ].re = data;
          src[ x ][ y ][ z ].im = 0.0 ;
        }
  

  fftwnd_plan forward3DPlanF =  fftw3d_create_plan( globalNx, globalNy, globalNz,
                                                    FFTW_FORWARD, FFTW_ESTIMATE );
  
  fftwnd_one( forward3DPlanF,
              (fftw_complex*) &src[ 0 ][ 0 ][ 0 ],
              (fftw_complex*) &dst1[ 0 ][ 0 ][ 0 ] );
  
  
  int count =0;
  double csRe=0.0;
  double csIm=0.0;
  for(int x=0; x < globalNx; x++)
    for(int y=0; y < globalNy; y++)
      for(int z=0; z < globalNz; z++)
        {          
          double real = dst1[ x ][ y ] [ z ].re;
          double img = dst1[ x ][ y ] [ z ].im;
          csRe += real;
          csIm += img;
          
//           int * re = (int *) &real;
//           int * im = (int *) &img;
          
//           printf("(%08X%08X,%08X%08X)\n", re[0],re[1],im[0],im[1]);         
        }
  
  int * re = (int *) &csRe;
  int * im = (int *) &csIm;
  
  printf("csRe = ( %08X%08X )\n", re[0],re[1]);         
  printf("csIm = ( %08X%08X )\n", im[0],im[1]);         
}
