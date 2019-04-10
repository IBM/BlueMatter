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

extern "C" {
  int _CP_main(void)  ;
  int _IOP_main(void) ;
  void s0printf(const char *, ...) ;
  unsigned long long GetTimeBase(void) ;
} ;

static void call_fft1dfr(void)
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
   long long tbstart = GetTimeBase() ;
   fft128_forward(block_a, block_b, block_b, block_a) ;
   long long tblap  = GetTimeBase() ;
   for (int b=0; b<1000; b+=1)
   {
      fft128_forward(block_a, block_b, block_b, block_a) ;
   } /* endfor */
   long long tbend = GetTimeBase() ;
   int taken_1 = tblap-tbstart ;
   int taken_1000 = tbend - tblap ;
   s0printf("2 FFTs in %i cycles, 2000 FFTs in %i cycles\n",taken_1,taken_1000) ;
} ;

enum {
   k_BlockCount = 1000
   } ;
fft3d_complex block_a[128*k_BlockCount] ;
fft3d_complex block_b[128*k_BlockCount]  ;

static void call_fft1dfr_longblock(void)
{
   int ticq[k_BlockCount] ;
   double d = 1.0 ;
   ticq[0] = 0 ;
   for (int a=0; a<128*k_BlockCount; a+=1)
   {
      block_a[a].re = d ;
      block_a[a].im = d+1.0 ;
      block_b[a].re = d+2.0 ;
      block_b[a].im = d+3.0 ;
      d += 4.0 ;
   } /* endfor */
   fft128_forward(block_a, block_b, block_b, block_a) ;
   fft128_inverse(block_a, block_b, block_b, block_a) ;
   for (int b=1; b<k_BlockCount; b+=1)
   {
      // Touch all the memory by doing an inverse transform
      for (int c=0; c<b; c+=1)
      {
         fft128_inverse(block_a+128*c, block_b+128*c, block_b+128*c, block_a+128*c) ;
      } /* endfor */
      // Do the forwards transforms
      long long tbstart = GetTimeBase() ;
      for (int c=0; c<b; c+=1)
      {
         fft128_forward(block_a+128*c, block_b+128*c, block_b+128*c, block_a+128*c) ;
      } /* endfor */
      long long tblap  = GetTimeBase() ;
      ticq[b] = tblap - tbstart ;
   } /* endfor */

   for (int d=1; d<k_BlockCount; d += 1)
   {
      s0printf("%i %i %i (pair-FFTs in cycles, delta )\n",d,ticq[d],ticq[d]-ticq[d-1]) ;
   } /* endfor */
} ;


int _CP_main(void)
{
   call_fft1dfr_longblock() ;
   return 0 ;
}

int _IOP_main(void)
{
//   call_fft1dfr() ;
   return 0 ;
}
