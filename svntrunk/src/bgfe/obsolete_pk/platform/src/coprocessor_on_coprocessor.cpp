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
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <cstdlib>
#include <rts.h>
#include <bgllockbox.h>

#include <pk/coprocessor_on_coprocessor.hpp>

char*          ScratchPadWindow;
unsigned int   ScratchPadWindowSize;

BGL_Mutex* co_co_lock;

struct co_co_element
  {
  CO_FuncPtrType mFuncPtr;
  void     * mArgSrc;
  int        mArgSize;
  double     mArg[1];  // over subscripted, passed as char but declared double for allignment
  };

void co_co_poll(void *arg)
  {
  while( 1 )
    {
    // make sure core 1 has an empty cache on co_start ... most of time, cycles for this flush will be hidden
//this would be good to clear core 1 ... but leaves actors to flush before join...     rts_dcache_evict_normal();
//so we put one before the join signal.... could make that optional at co_start interface

    while( BGL_Mutex_Is_Locked( co_co_lock ) )
      ;

    // If unlocked
    // Look for function, arg, size in a well known
    // location - scratch pad area
    co_co_element* co_element_ptr = (co_co_element * )ScratchPadWindow;
    CO_FuncPtrType Func    = co_element_ptr->mFuncPtr;
    char*   Arg     = (char *) co_element_ptr->mArg;
    int     ArgSize = co_element_ptr->mArgSize;

    // probably ought to put the args into cached memory... at least as an option.

    assert( Func );

    // printf("Arg: %08x  ArgSize: %d fp: %08x\n", Arg, ArgSize, Func );

    if( ArgSize )
      Func( Arg );
    else
      Func( NULL );

    rts_dcache_evict_normal();

    // lock Core 1
    BGL_Mutex_Acquire( co_co_lock );
    }
  }

void co_co_init()
  {
//  printf("Got Here: %d\n", __LINE__ );

  rts_get_scratchpad_window( (void **)&ScratchPadWindow,
                             &ScratchPadWindowSize );

  co_co_lock = rts_allocate_mutex();
  assert( co_co_lock );

  BGL_Mutex_Acquire( co_co_lock );

  // _blLockBoxForceLock( COPROCESSOR_LOCK );

  rts_dcache_evict_normal();

//  printf("Got Here: %d\n", __LINE__ );

  rts_run_coprocessor( co_co_poll, (void *)NULL );

//  printf("Got Here: %d\n", __LINE__ );
  }


int co_co_start( CO_FuncPtrType aFp, void* aArg, int aArgSize )
  {
  // Put the function pointer, size and args in a known
  // place. In this case the known place is the start of the
  // Scratch pad
  co_co_element* co_element_ptr = (co_co_element * )ScratchPadWindow;
  co_element_ptr->mFuncPtr = aFp;
  co_element_ptr->mArgSize = aArgSize;
  co_element_ptr->mArgSrc  = aArg;

  if( aArg != NULL )
    {
    assert( aArgSize > 0 && aArgSize < ( ScratchPadWindowSize - sizeof( co_co_element ) ) );
    // printf("co_element_ptr->mArg: %08x  aArg: %08x  ArgSize: %d fp: %08x\n",
    //         &co_element_ptr->mArg, aArg, aArgSize, aFp );

    memcpy( (void *)co_element_ptr->mArg, (void *)aArg, aArgSize );
    }
  else
    assert( !aArgSize );

  // Signal Core 1 that function/args are ready
  // _blLockBoxUnLock( COPROCESSOR_LOCK );
  BGL_Mutex_Release( co_co_lock );
  return 1;
  }

void co_co_join()
  {
  // Spin till the other core is locked (finished )
      // while( ! _blLockBoxQuery( COPROCESSOR_LOCK ) )
  while( ! BGL_Mutex_Is_Locked( co_co_lock ) )
    ;

  co_co_element* co_element_ptr = (co_co_element * )ScratchPadWindow;

  // This block copies the args back
  if( co_element_ptr->mArgSrc != NULL )
    {
    memcpy( co_element_ptr->mArgSrc, (void*) co_element_ptr->mArg, co_element_ptr->mArgSize );
    }
  }

#if 0


void* TestCode( void* arg )
  {
  int * SendCore = (int *) arg;

  char StrBuff[ 512 ];

    sprintf( StrBuff, "Hello from core: %d on core %d\n", *SendCore,  rts_get_processor_id() );

    write( 1, StrBuff, strlen( StrBuff ) );

  return NULL;
  }


extern "C"
{
int PMI_co_start( void*(*fp)(void *), void* arg );
void* PMI_co_join( int handle );
};

int
main( int argc, char **argv, char **envp )
{
   co_co_init();

///  MPI_Init( &argc, &argv );

#if 1
  int Core = rts_get_processor_id();

   co_co_start( TestCode, &Core, sizeof( int ) );

   TestCode( &Core );

   co_co_join();
#else

  int Core = rts_get_processor_id();
  int handle = PMI_co_start( TestCode, &Core );

  TestCode( &Core );
  PMI_co_join( handle );

#endif


//  BGLPartitionBarrierComputeCore0();

  return 1;
}
#endif
