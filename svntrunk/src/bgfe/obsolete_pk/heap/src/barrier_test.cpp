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

BGL_Barrier *b;

unsigned long long CoreTime0;
unsigned long long CoreTime1;


static void* TestActorFx( void* arg )
  {
  char StrBuf[ 4096 ];
  int core = rts_get_processor_id();

  if( !core )
    {
    int dummy = 0;
    for( int i=0; i < 500000; i++ )
      {
      dummy++;
      }
    
    sprintf( StrBuf, "dummy: %d %d\n", dummy, BGL_Barrier_Is_Raised( b ) );
    
    write( 1, StrBuf, strlen( StrBuf ) );
    }   

  BGL_Barrier_Pass( b );
  
  if( core )
       CoreTime1 = rts_get_timebase();
   else
       CoreTime0 = rts_get_timebase();
   
   rts_dcache_evict_normal();

  //printf( " %llu\n", rts_get_timebase() );
  sprintf( StrBuf, "Leaving TestActorFx Core: %d\n", core );
    
  write( 1, StrBuf, strlen( StrBuf ) );
  }

int main( int argc, char *argv[], char **envp )
  {
  printf( "Entering main()\n" );

  BGL_Mutex *m = rts_allocate_mutex();
  assert( m );

  printf("Locked0: %d ptr: %08X\n", BGL_Mutex_Is_Locked( m ), m);

  BGL_Mutex_Acquire( m );

  printf("Locked1: %d\n", BGL_Mutex_Is_Locked( m ));


  BGL_Mutex_Release( m );

  printf("Locked2: %d\n", BGL_Mutex_Is_Locked( m ));

#if 1
  b = rts_allocate_barrier();
  assert( b );
  // BGL_Barrier_Pass( b );    

  printf("0BGL_Barrier_Is_Raised(): %d\n", BGL_Barrier_Is_Raised( b ) );
  
//   if( BGL_Barrier_Is_Raised( b ) )
//       BGL_Barrier_Lower( b );

  rts_dcache_evict_normal();

  MPI_Init( &argc, &argv );

//   if( BGL_Barrier_Is_Raised( b ) )
//       BGL_Barrier_Lower( b );

//   BGL_Barrier_Lower( b );

  printf("1BGL_Barrier_Is_Raised(): %d\n", BGL_Barrier_Is_Raised( b ) );

  int handle = PMI_co_start( TestActorFx, NULL );
  TestActorFx( NULL );
  PMI_co_join( handle );
  
  printf( "core0: %llu core1: %llu diff: %llu\n",
          CoreTime0, CoreTime1, CoreTime1-CoreTime0 );
#endif

  printf( "Leaving main()\n" );

  return NULL;
  }
