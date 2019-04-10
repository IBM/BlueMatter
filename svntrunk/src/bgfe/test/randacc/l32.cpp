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
 //  xlC -O3 -qtune=pwr4 -qarch=pwr4 l32.cpp -o l32

#include <stdio.h>
#include <math.h>
#include <unistd.h>

#ifdef MPI
#include <mpi.h>
#endif

#include "rand.c"

randctx ctx;
void InitRand()
  {
  ctx.randa=ctx.randb=ctx.randc=(ub4)0;
  for (int i=0; i<256; ++i)
    ctx.randrsl[i]=(ub4)0;
  randinit(&ctx, TRUE);
  }

unsigned int GetRandWord()
  {
  unsigned int word = rand( &ctx );
  //printf("%08X\n",word);
  return word ;
  //return(  rand() );
  }

#define L3ARRAYRANGE ( 5000 )
#define L3ARRAY_SIZE_BITS  ( L3ARRAYRANGE * L3ARRAYRANGE  )  // at 5000, this is 3MB
#define SHORTLISTSIZE  (2*1024)                        // will do 1/2(SHORTLISTSIZE**2) accesses

#define L3ARRAY_SIZE_WORDS ( L3ARRAY_SIZE_BITS / (8 * sizeof(int)) )

int L3Array[ L3ARRAY_SIZE_WORDS ];

int Dummy1[ L3ARRAY_SIZE_WORDS ];

#define RANDOM_L3ARRAY_BIT_OFFSET()    (GetRandWord() % L3ARRAY_SIZE_BITS )

int main(int argc, char **argv, char **envp)
  {
  #ifdef MPI
    MPI_Init( &argc, &argv );  /* initialize MPI environment */
  #endif

  InitRand();

  printf("sizeof( L3Array ) %d\n", sizeof( L3Array ) );

  for( int i = 0; i < 10000; i ++ )
    {
    int BitOffset = RANDOM_L3ARRAY_BIT_OFFSET();
    L3Array[  BitOffset / (8*sizeof(int)) ] |= (0x01 << (BitOffset % (sizeof(int) * 8)) );
    }

  int ShortList[ SHORTLISTSIZE ];

  for( int i = 0; i < SHORTLISTSIZE; i++ )
    {
    ShortList[ i ] = GetRandWord() % L3ARRAYRANGE ;
    }

  // hammer another area of memory
  for( int i = 0; i < 1000000; i ++ )
    {
    int BitOffset = RANDOM_L3ARRAY_BIT_OFFSET();
    Dummy1[  BitOffset / (8*sizeof(int)) ] |= (0x01 << (BitOffset % (sizeof(int) * 8)) );
    }

  printf("Range %d Range**2 %d  Array Bytes %d  ShortListSize %d\n",
          L3ARRAYRANGE, L3ARRAY_SIZE_BITS, L3ARRAY_SIZE_WORDS * sizeof(int), SHORTLISTSIZE );

  #ifdef MPI
    double start, end;
    start = MPI_Wtime();
  #else
    timebasestruct_t tstart, tend;
    read_real_time( &tstart, TIMEBASE_SZ );
  #endif

  int found = 0;
  for(int a = 0; a < SHORTLISTSIZE; a++)
    {
    int aoffset = ShortList[ a ] * L3ARRAYRANGE ;
    int l3index[ SHORTLISTSIZE ];
    for( int b = a; b < SHORTLISTSIZE; b++ )
      {
      int pairoffset = aoffset + ShortList[ b ];
      l3index[ b ] = pairoffset / (8*sizeof(int));
      }
    int l3word[ SHORTLISTSIZE ];
    for( int b = a; b < SHORTLISTSIZE  ; b++ )
      {
      l3word[ b ] = L3Array[ l3index[b] ];
      }
    for( int b = a; b < SHORTLISTSIZE; b++ )
      {
      int word = l3word[b];
      int pairoffset = aoffset + ShortList[ b ];
      int bit = word & (0x01 << (pairoffset % (sizeof(int) * 8)));
      if( bit )
        found++;
      }
    }

  #ifdef MPI
    end = MPI_Wtime();
  #else
    read_real_time( &tend, TIMEBASE_SZ );
    time_base_to_time( &tstart, TIMEBASE_SZ );
    time_base_to_time( &tend, TIMEBASE_SZ );
    double start = tstart.tb_low * 0.000000001 + tstart.tb_high;
    double end = tend.tb_low * 0.000000001 + tend.tb_high;
  #endif

  printf("time total %16.12f  ave per access %16.10f ns  %d \n",
          end - start, (end - start) / (0.5 * SHORTLISTSIZE * SHORTLISTSIZE) * 1000.0*1000.0*1000.0, found);

  #ifdef MPI
    MPI_Finalize();
  #endif

  return(0);
  }
