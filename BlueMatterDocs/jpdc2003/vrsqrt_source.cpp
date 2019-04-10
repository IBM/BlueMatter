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
  VisualAge C++ for AIX Compiler (DEVELOPMENT/BETA) Version 6.0.0.0 ---

           11 | #include <math.h>
           12 | void tenrootl(double* f, const double* x, int groupcount)
           13 | {
           14 |    double x0 = x[0] ;
           15 |    double x1 = x[1] ;
           16 |    double x2 = x[2] ;
           17 |    double x3 = x[3] ;
           18 |    double x4 = x[4] ;
           19 |    double x5 = x[5] ;
           20 |    double x6 = x[6] ;
           21 |    double x7 = x[7] ;
           22 |    double x8 = x[8] ;
           23 |    double x9 = x[9] ;
           24 |    for (int a=0; a < groupcount; a+=1)
           25 |    {
           26 |      double r0 = 1.0/sqrt(x0) ;
           27 |      double r1 = 1.0/sqrt(x1) ;
           28 |      double r2 = 1.0/sqrt(x2) ;
           29 |      double r3 = 1.0/sqrt(x3) ;
           30 |      double r4 = 1.0/sqrt(x4) ;
           31 |      double r5 = 1.0/sqrt(x5) ;
           32 |      double r6 = 1.0/sqrt(x6) ;
           33 |      double r7 = 1.0/sqrt(x7) ;
           34 |      double r8 = 1.0/sqrt(x8) ;
           35 |      double r9 = 1.0/sqrt(x9) ;
           36 |      x0 = x[a*10+10] ;
           37 |      x1 = x[a*10+11] ;
           38 |      x2 = x[a*10+12] ;
           39 |      x3 = x[a*10+13] ;
           40 |      x4 = x[a*10+14] ;
           41 |      x5 = x[a*10+15] ;
           42 |      x6 = x[a*10+16] ;
           43 |      x7 = x[a*10+17] ;
           44 |      x8 = x[a*10+18] ;
           45 |      x9 = x[a*10+19] ;
           46 |      f[a*10+0] = r0 ;
           47 |      f[a*10+1] = r1 ;
           48 |      f[a*10+2] = r2 ;
           49 |      f[a*10+3] = r3 ;
           50 |      f[a*10+4] = r4 ;
           51 |      f[a*10+5] = r5 ;
           52 |      f[a*10+6] = r6 ;
           53 |      f[a*10+7] = r7 ;
           54 |      f[a*10+8] = r8 ;
           55 |      f[a*10+9] = r9 ;
           56 |    } /* endfor */
           57 | }
