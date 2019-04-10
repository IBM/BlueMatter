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
 

// This reactor buffer management should probably be made a class.

// Keep in mind, for machines like Blue Light, we might want to draw
// these buffers out of some sort of special memory pool.

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_PKREACTOR
#define PKFXLOG_PKREACTOR (0)
#endif


#define PKREACTOR_OK              (0)
#define PKREACTOR_DETATCH_BUFFER  (1)
#define PKREACTOR_ERROR          (-1)


// Drop the buffer pool for BGL simulation, so it initialises reasonably quickly
#if defined( PK_BLADE )
#define FREE_BUFFER_POOL_SIZE (64)
#else
#define FREE_BUFFER_POOL_SIZE (16 * 1024)
#endif

// Allocate buffers to be double aligned.
typedef double   Buffer[ (Platform::Reactor::PAYLOAD_SIZE + 7) / sizeof(double) ];
typedef void*  BufferPtr;

char *                             FreeBufferPool[ FREE_BUFFER_POOL_SIZE ];
int                                FreeBufferPoolCount;
Platform::Mutex::MutexWord         FreeBufferPoolLockWord;
int                                BuffersInService;


void *
Platform::Reactor::GetBuffer()
  {
  BufferPtr ReturnBufferPtr;
  Platform::Mutex::YieldLock( &FreeBufferPoolLockWord );
  if( FreeBufferPoolCount > 0 )
    {
    int Index = --FreeBufferPoolCount; // NEED: make this SMP safe!
    ReturnBufferPtr = FreeBufferPool[ Index ];
    }
  else
    {
    BuffersInService++;
    BegLogLine( PKFXLOG_PKREACTOR )
      << " GetBuffer() "
      << " allocating Buffer to cnt: "
      << BuffersInService
      << EndLogLine;

#if defined (PK_BLADE)
    Buffer* ReturnBufferPtr1;
    pkNew( &ReturnBufferPtr1, 1, __FILE__, __LINE__ );
    ReturnBufferPtr = ReturnBufferPtr1;
#else
    ReturnBufferPtr = new Buffer;
#endif
    }
  Platform::Mutex::Unlock( &FreeBufferPoolLockWord );
  // The following assert may mean that something is consuming
  // actor receive buffers.  If this assert goes off, think hard!
  // It might alternatively mean that the buffer pool is corrupt of course.
  assert( ReturnBufferPtr != NULL );
  return( (void *) ReturnBufferPtr );
  }

void
Platform::Reactor::FreeBuffer( void * BufferToFree )
  {
  assert( BufferToFree != NULL );
  Platform::Mutex::YieldLock( &FreeBufferPoolLockWord );
  if( FreeBufferPoolCount < FREE_BUFFER_POOL_SIZE )
    {
    FreeBufferPool[ FreeBufferPoolCount ] = ( char * ) BufferToFree;
    FreeBufferPoolCount++;
    }
  else
    {
    BuffersInService--;
    BegLogLine( PKFXLOG_PKREACTOR )
      << " FreeBuffer() "
      << " Forced to do delete from cnt: "
      << BuffersInService
      << EndLogLine;
// Don't have 'delele' support for BGL at the moment ...
#if !defined(PK_BLADE)
    delete (Buffer *) BufferToFree;  //LINUX/GCC cast pointer to type to delete
#endif
    }
  Platform::Mutex::Unlock( &FreeBufferPoolLockWord );
  }


