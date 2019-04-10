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
 






THIS FILE HAS BEEN RETIRED AND SHOULD NOT BE INCLUDED








/***************************************************************************
 * Project:         pK
 *
 * Module:          pkreactor.hpp
 *
 * Purpose:         Provide reactor messages using mpi.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/
#ifndef __PKREACTOR_HPP__
#define __PKREACTOR_HPP__

#ifndef PKFXLOG_PKREACTOR
#define PKFXLOG_PKREACTOR (0)
#endif

#ifndef PKREACTOR_DATAGRAM_SIZE
#define PKREACTOR_DATAGRAM_SIZE (32 * 1024)
#endif

#define PKREACTOR_OK (0)
#define PKREACTOR_DETATCH_BUFFER (1)
#define PKREACTOR_ERROR (-1)

#include <stdio.h>
#include <assert.h>

#ifdef MPI
#include <pk/mpi.hpp>
#endif

#include <pk/pthread.hpp>
#include <pk/pktrace.hpp>
#include <pk/fxlogger.hpp>
#include <pk/yieldlock.hpp>

#if BLUEGENE

#ifndef BLUEGENE_PACKET_PAYLOAD_SIZE
#define BLUEGENE_PACKET_PAYLOAD_SIZE 250
#endif
// Redefine datagram size to bluegene payload size.
#undef  PKREACTOR_DATAGRAM_SIZE
#define PKREACTOR_DATAGRAM_SIZE BLUEGENE_PACKET_PAYLOAD_SIZE

typedef int (*PkActorFxPtr)( // no src for BLUEGENE
                            //  no len for BLUEGENE
                            void     *MsgBuf   );

// A reactor provides local state with the function invocation.
typedef int (*PkReactorFxPtr)(void     *State,
                              void     *MsgBuf   );

#else

typedef int (*PkActorFxPtr)(unsigned  SrcProc,
                            unsigned  MsgLen,
                            void     *MsgBuf   );

// A reactor provides local state with the function invocation.
typedef int (*PkReactorFxPtr)(void     *State,
                              unsigned  SrcProc,
                              unsigned  MsgLen,
                              void     *MsgBuf   );
#endif

typedef struct ReactorTableEntry
  {
  char           *State;
  PkReactorFxPtr  RFxPtr;
  };

typedef struct
  {
  int                Task;
  ReactorTableEntry *ReactorTableEntryPtr;
  int IsValid() { return( ReactorTableEntryPtr != NULL ); }
  } ReactorHandle;

#ifdef MPI
class PkActiveMsgSubSys : public PhysicalThreadContext, public YieldLockable
#else
#ifdef SMP
class PkActiveMsgSubSys : public PhysicalThreadContext, public YieldLockable
#else
class PkActiveMsgSubSys : public YieldLockable
#endif
#endif
  {
  public :

    PkActiveMsgSubSys( );

    ReactorTableEntry  ReactorTable[1024];
    ReactorTableEntry *ReactorTableNext;


  public:
    static int Init();
    static int IsInit();

    static int Trigger( int             TaskNo,
                        PkActorFxPtr    FxPtr,
                        void           *Msg,
                        unsigned        MsgLen);

    static int Trigger( ReactorHandle  Handle,
                        void          *Msg,
                        unsigned       MsgLen);

    static
    ReactorHandle RegisterReactor( char *State, PkReactorFxPtr RFxPtr );

    void *PhysicalThreadMain( void *arg );

    typedef char    Buffer[PKREACTOR_DATAGRAM_SIZE];
    typedef char   *BufferPtr;

#ifdef MPI

    MPI_Comm REACTIVE_MESSAGE_COMM_WORLD;

    BufferPtr      *AllBuf     ;    // A pointer to an array of pointers.
    MPI_Request    *AllRequests;
    MPI_Status     *AllStatus  ;

#endif // end ifdef mpi

    // Pk Trace point state
    TraceClient                   TraceTrigger;
    TraceClient                   TraceExecute;
    int                           TracePPid;
    char                          TraceName[128];

// The buffer pool is a stack of free buffers.  It is worth maintaining
// as a stack rather than a circular queue becuase it is hoped that the
// bottom of the stack will point to buffers that have been paged out
// of memory because they haven't been used.

// NEED: The GetBuffer/FreeBuffer routines contain dyanamic memory calls.
// Although the delete call may never be made,  An assert it placed after
// the 'new' call which will catch the worst offenders.

#define FREE_BUFFER_POOL_SIZE (16 * 1024)
    BufferPtr       FreeBufferPool[FREE_BUFFER_POOL_SIZE];
    int             FreeBufferPoolCount;
    int             FreeBufferPoolLockWord;
    int             BuffersInService;

    BufferPtr
    GetBuffer()
      {
      BufferPtr ReturnBufferPtr;
      YieldLock( FreeBufferPoolLockWord );
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

        ReturnBufferPtr = new Buffer;

        // The following assert means that something is consuming
        // actor receive buffers.  If this assert goes off, think hard!
        assert( ReturnBufferPtr != NULL );
        }
      UnYieldLock( FreeBufferPoolLockWord );
      return( ReturnBufferPtr );
      }

    void
    FreeBuffer( void * BufferToFree )
      {
      YieldLock( FreeBufferPoolLockWord );
      if( FreeBufferPoolCount < FREE_BUFFER_POOL_SIZE )
        {
        FreeBufferPool[ FreeBufferPoolCount ] = ( BufferPtr ) BufferToFree;
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
        delete (BufferPtr) BufferToFree;  //LINUX/GCC cast pointer to type to delete
        }
      UnYieldLock( FreeBufferPoolLockWord );
      }


    static PkActiveMsgSubSys* GetIFPtr();

  };
#endif

