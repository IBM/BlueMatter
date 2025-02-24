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
 #include <fft3d/fft128a.hpp>
#include <cstdio>
using namespace std;

int call_fft1dfr(void)
{
   fft3d_complex block_a[128] ;
   fft3d_complex block_b[128]  ;
   double d = 1.0 ;
   for (int a=0; a<128; a+=1)
   {
      block_a[a].re = d ;
      block_a[a].im = d+1.0 ;
      block_b[a].re = d+2.0 ;
      block_b[a].im = d+3.0 ;
      d += 4.0 ;
   } /* endfor */
   fft128_forward(block_a, block_b, block_b, block_a) ;
   fft128_forward(block_a, block_b, block_b, block_a) ;
   for (int b=0; b<1000000; b+=1)
   {
      fft128_forward(block_a, block_b, block_b, block_a) ;
   } /* endfor */
   return block_a[0].re+ block_b[0].re ;
} ;

int main(void)
{
   int a=call_fft1dfr() ;
   return (a<0) ? 0 : 1 ;
}
