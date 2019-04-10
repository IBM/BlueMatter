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
 #ifndef __PLIMPTONTARGET_HPP__
#define __PLIMPTONTARGET_HPP__

//  Give the address spaces a 2-d numbering (for kExtentY=3, kExtentX=4)
//    0   1   2   3
//    4   5   6   7
//    8   9  10  11

//  Then a target numbering as below is required
//    .   .   3   .
//    0   1   .   2
//    .   .   4   .
//  for a cross-centre of mY=1, mX=2.
//  (don't drive the multicast to self)

class PlimptonTarget {
public:
   enum {
      kExtentY = 3 ,
      kExtentX = 4
      } ;
   int mY ;
   int mX ;
   int TargetCount(void) const            // How many places this message should be sent to
   {
      return ( kExtentY + kExtentX - 2) ;
   }
   int Target(int N) const   // Which address space is the n-th place this message should be sent to
   {
      if (N < kExtentX-1)
      {
         if (N < mX)
         {
           return (mY*kExtentX) + N ;
         }
         else
         {
           return (mY*kExtentX) + (N+1) ;
         } /* endif */
      }
      else
      {
         int N1 = N-(kExtentX-1) ;
         if (N1 < mY)
         {
            return (N1*kExtentX) + mX ;
         }
         else
         {
            return ((N1+1)*kExtentX) + mX ;
         } /* endif */
      } /* endif */
   }

} ;


#endif
