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
 * Build a FFT 'butterfly' table
 *
 * MakeFFTTable 7    # make a table for 2**7 element transform
 *
 */
#include <stdlib.h>
#include <stdio.h>

unsigned int bitreverse(unsigned int x, unsigned int bitcount)
{
   unsigned int result = 0 ;
   for (unsigned int bx=0; bx<bitcount; bx+=1)
   {
      if (x & ( 1 << bx ) )
      {
         result |= ( 1 << (bitcount-1-bx) ) ;
      } /* endif */
   } /* endfor */
   return result ;
}

void genbutterfly(unsigned int bitcount)
{
  unsigned int lim = ( 1 << bitcount ) ;
  for (unsigned int x0=0; x0<lim; x0+=1)
  {
     unsigned int y = bitreverse(x0,bitcount) ;
     if (y > x0)
     {
       printf("     br1( 0x%08x,0x%08x ) ;\n",x0,y ) ;
     } /* endif */
  } /* endfor */
  for (unsigned int x1=0; x1<lim; x1+=1)
  {
     unsigned int y = bitreverse(x1,bitcount) ;
     if (y == x1)
     {
       printf("     bsc( 0x%08x ) ;\n",x1 ) ;
     } /* endif */
  } /* endfor */
}

int main(int argc, const char ** argv, const char ** envp)
{
   if (2 == argc)
   {
     unsigned int bitcount = atoi(argv[1]) ;
     if (bitcount > 0 && bitcount < 32)
     {
        genbutterfly(bitcount) ;
     } /* endif */
   }
   else
   {
   } /* endif */
}
