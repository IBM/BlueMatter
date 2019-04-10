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
 // **********************************************************************
//                       CVS INFORMATION
//
// SOURCE: $Source$
// REVISION: $Revision: 5381 $
// DATE: $Date: 2003-06-25 10:02:21 -0400 (Wed, 25 Jun 2003) $
// AUTHOR: $Author: tjcw $
// TAG NAME (if applicable): $Name$
// **********************************************************************

#include <math.h>
void tenrootl(double* f, const double* x, int groupcount)
{
   double x0 = x[0] ;
   double x1 = x[1] ;
   double x2 = x[2] ;
   double x3 = x[3] ;
   double x4 = x[4] ;
   double x5 = x[5] ;
   double x6 = x[6] ;
   double x7 = x[7] ;
   double x8 = x[8] ;
   double x9 = x[9] ;
   for (int a=0; a < groupcount; a+=1)
   {
     double r0 = 1.0/sqrt(x0) ;
     double r1 = 1.0/sqrt(x1) ;
     double r2 = 1.0/sqrt(x2) ;
     double r3 = 1.0/sqrt(x3) ;
     double r4 = 1.0/sqrt(x4) ;
     double r5 = 1.0/sqrt(x5) ;
     double r6 = 1.0/sqrt(x6) ;
     double r7 = 1.0/sqrt(x7) ;
     double r8 = 1.0/sqrt(x8) ;
     double r9 = 1.0/sqrt(x9) ;
     x0 = x[a*10+10] ;
     x1 = x[a*10+11] ;
     x2 = x[a*10+12] ;
     x3 = x[a*10+13] ;
     x4 = x[a*10+14] ;
     x5 = x[a*10+15] ;
     x6 = x[a*10+16] ;
     x7 = x[a*10+17] ;
     x8 = x[a*10+18] ;
     x9 = x[a*10+19] ;
     f[a*10+0] = r0 ;
     f[a*10+1] = r1 ;
     f[a*10+2] = r2 ;
     f[a*10+3] = r3 ;
     f[a*10+4] = r4 ;
     f[a*10+5] = r5 ;
     f[a*10+6] = r6 ;
     f[a*10+7] = r7 ;
     f[a*10+8] = r8 ;
     f[a*10+9] = r9 ;
   } /* endfor */
}
