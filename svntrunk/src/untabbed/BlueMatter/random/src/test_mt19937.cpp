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
 // ******************************************************************
// Test driver for MT19937 class
// ******************************************************************

#include <BlueMatter/mt19937.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char** argv)
{
  MT19937 gen;

  int repCount=1000;
  int printOut = 1;
  unsigned long outInt;
  double outDouble;
  unsigned long oneSeed = 4357UL;
  unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
  gen.init_by_array(init, length);
  /* This is an example of initializing by an array.       */
  /* You may use init_genrand(seed) with any 32bit integer */
  /* as a seed for a simpler initialization                */
  if (argc > 1)
    {
      repCount = atoi(argv[1]);
      if (repCount < 0)
  {
    printOut = 0;
    repCount = -repCount;
  }
    }
  
  printf( "Testing output and speed of mt19937ar.c\n" );
  //  printf( "\nTest of random integer generation:\n" );


  printf("%d outputs of genrand_int32()\n", repCount);
  for (int i=0; i<repCount; ++i)
    {
      outInt = gen.genrand_int32();
      if (printOut != 0)
        {
          printf("%10lu ", outInt);
          if (i%5==4) printf("\n");
        }
    }

  //  printf( "\nTest of random real number [0,1) generation:\n" );
  //  gen.init_genrand(oneSeed);
  printf("\n%d outputs of genrand_real2()\n", repCount);
  for (int i=0; i<repCount; ++i)
    {
      outDouble = gen.genrand_real2();
      if (printOut != 0)
        {
          printf("%10.8f ", outDouble);
          if (i%5==4) printf("\n");
        }
    }

  printf( "\nTest of time to generate 300 million random integers:\n" );
  //  gen.init_genrand( oneSeed );
  unsigned long junk;
  unsigned long nReps = 300000000;
  clock_t start = clock();
  for( int i = 0; i < nReps; ++i )
    {
      junk = gen.genrand_int32();
    }
  clock_t stop = clock();
  printf( "Time elapsed = " );
  double tElapsed = (double)( stop - start ) / CLOCKS_PER_SEC; 
  printf( "%8.3f", tElapsed );
  printf( " s\n" );
  printf( "%12.3f per second\n", (double)nReps/tElapsed);
  
  return 0;
}
