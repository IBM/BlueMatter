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
using namespace std ;
#include <BlueMatter/fft4.hpp>

void fft4::forward(void)
{
   inline_forward() ;
}

void fft4::inverse(void)
{
   inline_inverse() ;
}

complex u[4] ;

int main(void)
{
// for (int a=0; a<4; a+=1)
// {
//    u[a].re = 0.0 ;
//    u[a].im = 0.0 ;
// } /* endfor */
// u[1].re = 1.0 ;

   u[0].re = 0.304657 ; u[0].im = 1.304657 ;
   u[1].re = 0.151860 ; u[1].im = 1.151860 ;
   u[2].re = 0.337662 ; u[2].im = 1.337662 ;
   u[3].re = 0.387477 ; u[3].im = 1.387477 ;

   for (int a=0; a<4; a+=1)
   {
      cout << u[a] << '\n' ;
   } /* endfor */
   fft4 transformer(u) ;
   cout << '\n' ;
   transformer.forward() ;
   for (int b=0; b<4; b+=1)
   {
      cout << u[b] << '\n' ;
   } /* endfor */
   transformer.inverse() ;
   cout << '\n' ;
   for (int c=0; c<4; c+=1)
   {
      cout << u[c] * 0.25  << '\n' ;
   } /* endfor */

}

