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
 #include <pk/platform.hpp>
#include <pk/fxlogger.hpp>


#include <BlueMatter/fft128.hpp>

void fft128::forward(void)
{
   inline_forward() ;
}

void fft128::inverse(void)
{
   inline_inverse() ;
}

complex_without_constructor u[128] ;

int PkMain(int arg, char ** argv, char ** envp)
{
   for (int a=0; a<128; a+=1)
   {
      u[a].re = 0.0 ;
      u[a].im = 0.0 ;
   } /* endfor */
   u[1].re = 1.0 ;

   fft128 transformer(u) ;
   transformer.forward() ;
   for (int b=0; b<128; b+=1)
   {
     BegLogLine(1)
       << "Forward[" << b << "] = " << u[b]
       << EndLogLine ;
   } /* endfor */
   transformer.inverse() ;
   for (int c=0; c<128; c+=1)
   {
     BegLogLine(1)
       << "Inverse[" << c << "] = " << u[c]
       << EndLogLine ;
   } /* endfor */

   return 0 ;
}

