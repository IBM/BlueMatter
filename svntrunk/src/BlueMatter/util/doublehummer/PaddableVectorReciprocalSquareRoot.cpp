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
 #include <math.h>

        void PaddableVectorReciprocalSquareRoot(
          double * Target,
          const double * Source,
          unsigned int Count
        ) {

                double x0 = Source[0];
                double x1 = Source[1];
                double x2 = Source[2];
                double x3 = Source[3];
                double x4 = Source[4];
                double x5 = Source[5];
                double x6 = Source[6];
                double x7 = Source[7];
                double x8 = Source[8];

                double r0 = 1.0/sqrt(x0);
                double r1 = 1.0/sqrt(x1);
                double r2 = 1.0/sqrt(x2);
                double r3 = 1.0/sqrt(x3);
                double r4 = 1.0/sqrt(x4);
                double r5 = 1.0/sqrt(x5);
                double r6 = 1.0/sqrt(x6);
                double r7 = 1.0/sqrt(x7);
                double r8 = 1.0/sqrt(x8);

                Target[0] = r0 ;
                Target[1] = r1 ;
                Target[2] = r2 ;
                Target[3] = r3 ;
                Target[4] = r4 ;
                Target[5] = r5 ;
                Target[6] = r6 ;
                Target[7] = r7 ;
                Target[8] = r8 ;

        }

