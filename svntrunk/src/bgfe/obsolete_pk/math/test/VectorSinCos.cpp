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
#include <iomanip>
using namespace std ;
#include <math.h>

#include <pk/sincos.hpp>
// These classes are to assure the compiler that input and output vectors are not aliases
class double_wrap_1
{
   public:
   double A ;
} ;

class double_wrap_2
{
   public:
   double A ;
} ;


// A couple of routines callable from C or Fortran, for sines/cosines of a vector of angles
//   void VectorSin(double *, double *, unsigned int *) ;
//   void VectorCos(double *, double *, unsigned int *) ;
//   function VectorSin(real*8 (*), real*8(*), integer)
//   function VectorCos(real*8 (*), real*8(*), integer)
extern "C" {
        void VectorSin(double_wrap_1 * Results, const double_wrap_2 * Sources, unsigned int * CountP)
        {
           unsigned int Count = *CountP ;
           for (unsigned int X=0; X<Count;X+=1 )
           {
              Results[X].A = Trig::Sin(Sources[X].A) ;
           } /* endfor */
        }
         void VectorCos(double_wrap_1 * Results, const double_wrap_2 * Sources, unsigned int * CountP)
        {
           unsigned int Count = *CountP ;
           for (unsigned int X=0; X<Count;X+=1 )
           {
              Results[X].A = Trig::Cos(Sources[X].A) ;
           } /* endfor */
        }
   } ;
