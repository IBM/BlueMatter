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
 #include <stdio.h>
#include "../include/HeapManager.hpp"

#define EXPECTED_ALLOC_VALUE (0) // should be 0xDEADBEEF

#if 0
int
main( int argc, char ** argv, char** envp )
  {
  HeapManager M;
  M.Init( );
  char * m = (char *) M.bgl_malloc( 10 );
  M.bgl_free( m );
  printf("done!\n");
  return( 0 );
  }
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <bgllockbox.h>
#include <rts.h>
#include <mpi.h>
#include <assert.h>

extern "C"
{
int PMI_co_start( void*(*fp)(void *), void* arg );
void* PMI_co_join( int handle );
};

inline
int
inc_rand( int rand_seed )
  {
  return ((rand_seed * 1103515245 + 12345) & 0x7fffffff);
  }

int Rank = -1;
BGLPersonality personality;

unsigned long long CoreTime0;
unsigned long long CoreTime1;


HeapManager HM;

struct
AppRecord
  {
  struct Hdr
    {
    unsigned int CheckSum;
    unsigned int Size;
    unsigned int Core;
    unsigned int Index;
    };
  unsigned int Data[ 1 ];
  };

#if 1
static
void*
TestActorFx( void* arg )
  {
  char StrBuf[ 4096 ];
  int core = rts_get_processor_id();
  unsigned long long StartTime = rts_get_timebase();

  sprintf( StrBuf, "N %5d C %d : StartTime %lld\n", Rank, core, StartTime );
  write( 1, StrBuf, strlen( StrBuf ) );
  ///fsync(1);

  #if 0
  if( core )
       CoreTime1 = rts_get_timebase();
   else
       CoreTime0 = rts_get_timebase();
  #endif

  int HeapPtrCount = 5 ;

  int ** HeapPtrs = (int **) HM.bgl_malloc( HeapPtrCount * sizeof( int * ) );

  for(int i = 0; i < HeapPtrCount; i++)
    {
    HeapPtrs[ i ] = (int *) EXPECTED_ALLOC_VALUE;
    }

  int NewMemoryError     = 0;
  int TouchedMemoryError = 0;

  int AllocNext = 0;

  int AllocSize = 0;

  for(int l = 0; l < 2500; l++)
    {
    int AllocCount = 0;

    for( int Trys = 0; (Trys < 4) && (AllocCount < HeapPtrCount); Trys++)
      {
      HeapPtrs[ AllocNext ] = (int *) HM.bgl_malloc( (AllocNext + 1) * sizeof( int ) );

      if( HeapPtrs[ AllocNext ] != NULL )
        {
        AllocCount++;
        for( int m = 0; m < (AllocNext + 1); m++ )
          {
                // Expect all new memory to be EXPECTED_ALLOC_VALUE
             #if 0 // cannot yet expect values
                if( HeapPtrs[ AllocNext ][ m ] != EXPECTED_ALLOC_VALUE )
                  {
                  sprintf(StrBuf,
                         "N %5d C %d : New memory not DEADBEEF  Alloc %d index %d (HeapPtrs[ AllocNext ][ m ] %d) \n",
                          Rank, core, AllocNext, m, HeapPtrs[ AllocNext ][ m ] );
                  write( 1, StrBuf, strlen( StrBuf ) );
                  //fsync(1);
                  exit(-1);
                  }
             #endif
          // Assign a number to expect
          HeapPtrs[ AllocNext ][ m ] = AllocNext;
          }

        AllocNext = (AllocNext + 1) % HeapPtrCount;
        }
      else
        {
        sprintf(StrBuf,"N %5d C %d : malloc returned NULL breaking on alloc loop %d AllocNext %d AllocCount %d\n",
                Rank, core, l, AllocNext, AllocCount );
        write( 1, StrBuf, strlen( StrBuf ) );
        break;
        }
      }

    int FreeIndex = (AllocNext - 1 + HeapPtrCount) % HeapPtrCount;

    sprintf(StrBuf,"N %5d C %d : switching from allocs to frees iter %d AllocNext %d AllocCount %d First FreeIndex %d\n",
            Rank, core, l, AllocNext, AllocCount, FreeIndex);
    write( 1, StrBuf, strlen( StrBuf ) );

#if 1
    for( int FreeCount = 0; FreeCount < AllocCount; FreeCount++ )
      {
      for( int m = 0; m < FreeIndex + 1; m++ )
        {
        if( HeapPtrs[ FreeIndex ][ m ] != FreeIndex )
          {
          sprintf(StrBuf,
                 "N %5d C %d : Memory TouchedError in value of area Alloc %d index %d (HeapPtrs[ Frees ][ m ] %d) \n",
                  Rank, core, FreeIndex, m, HeapPtrs[ FreeIndex ][ m ] );
          write( 1, StrBuf, strlen( StrBuf ) );
          //fsync(1);
          exit(-1);
          }
        // Set area to be freed back to deadbeef.
        HeapPtrs[ FreeIndex ][ m ] = EXPECTED_ALLOC_VALUE;
        }

      HM.bgl_free( HeapPtrs[ FreeIndex ] );

      HeapPtrs[ FreeIndex ] = (int*)0xDEADBEEF;

      FreeIndex = (FreeIndex - 1 + HeapPtrCount) % HeapPtrCount;
      }

    sprintf(StrBuf,"N %5d C %d : frees done : iter %d\n", Rank, core, l );
    write( 1, StrBuf, strlen( StrBuf ) );
#endif

    }

  HM.bgl_free( HeapPtrs );

  sprintf(StrBuf,
         "N %5d C %d : Done with loop \n", Rank, core );
  write( 1, StrBuf, strlen( StrBuf ) );
  //fsync(1);

  return( NULL );
  }
#endif


int
main( int argc, char *argv[], char **envp )
  {
  char StrBuf[ 4096 ];

  rts_get_personality( &personality, sizeof(personality) );

  int procsX = personality.xSize;
  int procsY = personality.ySize;
  int procsZ = personality.zSize;

  int CoordX = personality.xCoord;
  int CoordY = personality.yCoord;
  int CoordZ = personality.zCoord;

  Rank = CoordX * procsY * procsZ + CoordY * procsZ + CoordZ;

  sprintf(StrBuf, "N %5d C 0 : ProcDims %4d %4d %4d    Coord %4d %4d %4d \n",
          Rank, procsX, procsY, procsZ, CoordX, CoordY, CoordZ );

  write( 1, StrBuf, strlen( StrBuf ) );

  //printf( "Entering main()\n" );
  //fflush(stdout);

  MPI_Init( &argc, &argv );

  HM.Init( );

  //printf("Init done\n");
  //fflush(stdout);

  int handle = PMI_co_start( TestActorFx, NULL );
  TestActorFx( NULL );
  unsigned long long EndTime = rts_get_timebase();
  PMI_co_join( handle );

  sleep(3);

  sprintf(StrBuf, "N %5d C 0 : Done EndTime %lld\n", Rank, EndTime );
  write( 1, StrBuf, strlen( StrBuf ) );

  //printf("Init Done\n");
  //printf( "Leaving main()\n" );
  //fflush(stdout);
  return 0;
  }
