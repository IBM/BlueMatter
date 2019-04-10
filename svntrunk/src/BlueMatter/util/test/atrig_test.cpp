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
 /*
 * Test case for the 'atrig' routine.
 * This computes sines and cosines which are knowable geometrically for certain angles
 * starting with 180 degrees -> sin=0, cos=-1
 * then halving angles and using cos(T)=sqrt((1+cos(2T))/2), sin(T) = sqrt(1-cos(T)cos(T))
 * then producing using sin(A+B) = sin(A) cos(B) + cos(A) sin(B), cos(A+B) = ...
 * resulting in 65536 test cases covering the whole circle.
 * It uses 'atrig' to recover the angle from the sin and cos, and reports the maximum error.
 *
 * Chris Ward 20010817
 */
#include <BlueMatter/math.hpp>
#include <iostream.h>

void test_angles(void)
{
   double costable[16] ;
   double sintable[16] ;

   costable[15] = -1.0 ;
   sintable[15] = 0.0 ;

   double curcos = 0.0 ;
   double cursin = 1.0 ;

   // Angle-halving pass
   for (int a=14; a>=0 ; a-=1 )
   {
      costable[a] = curcos ;
      sintable[a] = cursin ;
      double nextcos = sqrt((1.0+curcos)/2.0) ;
      double nextsin = cursin / ( 2.0 * nextcos ) ;
      curcos = nextcos ;
      cursin = nextsin ;
   } /* endfor */

   // Test case generation
   double maxerr = 0.0 ;
   for (int b=0; b<=32768; b+=1)
   {
      // Work out the sin and cos by angle summing in binary
      double ccos=1.0 ;
      double ssin=0.0 ;
      for (int c=0; c<=15; c+=1)
      {
         if (b & (1 << c) )
         {
            double nsin = ssin * costable[c] + ccos * sintable[c] ;
            double ncos = ccos * costable[c] - ssin * sintable[c] ;
            ssin = nsin ;
            ccos = ncos ;
         } /* endif */
      } /* endfor */

      // Drive the routine under test for + angles
      double angle = Math::atrig(ssin, ccos) ;
      double actual_angle = ( b / 32768.0 ) * M_PI ;
      double err = fabs(angle-actual_angle) ;
      if ( err > maxerr) maxerr = err ;
//        cout << b << ' '  << ssin << ' ' << ccos << ' ' << angle << ' ' << actual_angle << ' ' << angle - actual_angle << '\n' ;

      // Drive the routine under test for - angles
      double nangle = Math::atrig(-ssin, ccos) ;
//        cout << b << ' ' << -actual_angle << ' ' << nangle << '\n' ;
      double nerr = fabs(-nangle-actual_angle) ;
      // '- 180 degrees' should be reported as '+ 180 degrees'
      if ( nerr > maxerr && b < 32768) maxerr = nerr ;

   } /* endfor */
   cout << "Largest error was " << maxerr << " radians\n" ;
}

int main(void)
{
//   cout << Math::cospiby8 << ' ' << Math::sinpiby8 << '\n' ;
   test_angles() ;
   return 0 ;
}
