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
 #ifndef __BONB_FIBER_HPP__
#define __BONB_FIBER_HPP__

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "XYZ.hpp"
#include <string.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <rts.h>
#include <blade_on_blrts.h>

#include <spi/BGLTorusAppSupport.c>
#include <pk/fxlogger.hpp>
#include <pk/platform.hpp>

#include "BonB_CoRoutine.h"

#define FCB_AlignmentPadding (0x01 << 10)
#define FCB_AlignmentMask   (FCB_AlignmentPadding - 1)

typedef unsigned long long FID;

struct
FID_MAP
  {
  unsigned short AddressSpaceId;
  unsigned short Thread;
  unsigned       Fiber;
  };

struct
Fiber
  {
  static
  inline
  BundleControlBlock *
  GetBundlePtr()
    {
    static BundleControlBlock ProcessBundleControlBlocks[2];  // Needs to be an indexable range of thread ordinals
    Platform_Thread_Id_T ThreadId = Platform_Thread_GetId();
    FiberBundle::BundleControlBlock * bcb = ProcessBundleControlBlocks[ ThreadId ];
    return( bcb );
    }

  // yield the current fiber - that is, call the scheduler
  static
  inline
  void
  Yield()
    {
    BundleControlBlock * bcb = GetBundlePtr();
    bcb->Yield();
    }

  // take the current fiber out of the run queue and yield
  static
  inline
  void
  Block()
    {
    BundleControlBlock * bcb = GetBundlePtr();
    bcb->Block();
    }

  static
  inline
  void
  Unblock( FID aFID )
    {
    BundleControlBlock * bcb = GetBundlePtr();
    bcb->Block();
    }

  static
  inline
  void
  Join( FID aFID )
    {
    BundleControlBlock * bcb = GetBundlePtr();
    bcb->Join( FiberControlBlock *);
    }

  static
  inline
  FID
  Create( BG_CoRoutineFunctionType aFiberMainFx,
          void *                   aFiberArg     = NULL,
          unsigned                 aReqStackSize = 0,
          PkThread_Id_T            aThreadId     = PkThread_Id_LOCAL,
          PkProc_Id_T              aProcId       = PkProc_Id_LOCAL
         )
    {
    }
  };

struct
FiberBundle
  {
  // One of these structs exists for each process or processor that will schedule fibers
  FiberControlBlock * ReadyQueueHead    = NULL;
  FiberControlBlock * RunningFibersFCB  = NULL;

  // One of these structs exists with the stack for each fiber created.
  struct
  FiberControlBlock
    {
    void * StackSeparationPadding[16];  // stack will grow in other dir from tcb

    _BG_CoRoutine_t SwapData;

    BundleControlBlock *mBundle;  // pointer to scheduling bundle struct - mostly for double checks

    FiberControlBlock *Next;
    FiberControlBlock *Prev;

    void * mFiberMemoryMallocBase;

    BG_CoRoutineFunctionType mFiberMainFx;
    void *                   mFiberArg;

    _BGL_TorusPkt   mResponsePacket;
    _BGL_TorusPkt * mResponsePacketPtr;

    int mExited;  // should be a state
    int mDetatched;

    FiberControlBlock * mJoiner; // probably want to allow a list the list of threads joined to me

    };

  static
  inline
  void
  Unblock(FiberControlBlock * aFCB = NULL )
    {
    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "Unblock() "
      << " starting on  "    << (void*) aFCB
      << " ReadyQueueHead "  << (void*) ReadyQueueHead
      << EndLogLine;
    assert( aFCB       != NULL );
    assert( aFCB       == aFCB->Next );
    assert( aFCB->Next == aFCB->Prev );

    if( aFCB != aFCB->Next || aFCB->Next != aFCB->Next )
      PLATFORM_ABORT("Unblock - aFCB->Next != aFCB || aFCB->Prev != aFCB" );

    if( ReadyQueueHead == NULL )
      {
      ReadyQueueHead = aFCB;
      }
    else
      {
      aFCB->Next                 = ReadyQueueHead;
      aFCB->Prev                 = ReadyQueueHead->Prev;
      ReadyQueueHead->Prev->Next = aFCB;
      ReadyQueueHead->Prev       = aFCB;
      }
    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "Unblock() "
      << " finished on "     << (void*) aFCB
      << " ReadyQueueHead "  << (void*) ReadyQueueHead
      << EndLogLine;
    }


  void
  PkCo_Barrier()
    {
    Platform::Control::BarrierWithWaitFunction( (void*) KickPipes, NULL );
    }

  void
  Yield()
    {
    BundleControlBlock* bcb = GetBundleControlBlock();

    BegLogLine( PK_FIBER )
      << (void*) bcb->RunningFibersFCB << " "
      << "Yield() "
      << " starting "
      << EndLogLine;
    // this block would be handled bellow - but it fast paths a non blocking yield
    // really shouldn't make a diff...
    if( bcb->ReadyQueueHead == bcb->RunningFibersFCB )
      {
      KickPipes( NULL );
      return;
      }

    for(;;) // Either return or swap
      {
      KickPipes( NULL );

      if( bcb->ReadyQueueHead != NULL )
        {
        if( bcb->ReadyQueueHead == bcb->RunningFibersFCB )
          {
          BegLogLine( PK_FIBER )
            << (void*) bcb->RunningFibersFCB << " "
            << "Yield() "
            << " finished - NO SWAP "
            << EndLogLine;
          return;
          }
        else
          {
          BegLogLine( PK_FIBER )
            << (void*) bcb->RunningFibersFCB << " "
            << "Yield()"
            << " SWAPPING TO "
            << " ReadyQueueHead "   << (void*) bcb->ReadyQueueHead
            << " ReadyQueueHead->mFiberMainFx " << (void*) bcb->ReadyQueueHead->mFiberMainFx
            << " @ "                << (void*) (&(bcb->ReadyQueueHead->mFiberMainFx))
            << " ReadyQueueHead->mFiberArg "    << (void*) bcb->ReadyQueueHead->mFiberArg
            << EndLogLine;

          assert( bcb->ReadyQueueHead    != NULL );
          assert( bcb->RunningFibersFCB != NULL );
          FiberControlBlock * FromFCB = RunningFibersFCB;
          RunningFibersFCB = ReadyQueueHead; // This is the only place this should be changed

  BegLogLine( PK_FIBER )
    << (void*) RunningFibersFCB << " (changed for swap) "
    << "Yield()"
    << " FromFCB @" << (void*) FromFCB
    << " FromFCB->SwapData @"        << (void*) &FromFCB->SwapData
    << " ... @"                      << (void*) (((unsigned)&FromFCB->SwapData) + sizeof( FromFCB->SwapData ))
    << " ReadyQueueHead->SwapData @" << (void*) &ReadyQueueHead->SwapData
    << " ... @"                      << (void*) (((unsigned)&ReadyQueueHead->SwapData) + sizeof( FromFCB->SwapData))
    << " sizeof( SwapData ) "        << sizeof( FromFCB->SwapData )
    << EndLogLine;

          _BG_CoRoutine_Swap( & (FromFCB->SwapData), & (ReadyQueueHead->SwapData) );

          BegLogLine( PK_FIBER )
            << (void*) RunningFibersFCB << " "
            << "Yield() "
            << " finished - AFTER SWAP "
            << " ReadyQueueHead "   << (void*) ReadyQueueHead
            << " RQH->Next "        << (void*) ReadyQueueHead->Next
            << " RQH->Prev "        << (void*) ReadyQueueHead->Prev
            << EndLogLine;
          assert( FromFCB == RunningFibersFCB );
          return;
          }
        }
      }
    }

  void
  Block()
    {
    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "Block() "
      << " starting "
      << " ReadyQueueHead " << (void*) ReadyQueueHead
      << " Next "          << (void*) RunningFibersFCB->Next
      << " Prev "          << (void*) RunningFibersFCB->Prev
      << EndLogLine;

    // The following block will move ReadyQueueHead off blocking FCB if there is another FCB in queue
    if( ReadyQueueHead == RunningFibersFCB )
      {
      ReadyQueueHead = ReadyQueueHead->Next;
      BegLogLine( PK_FIBER )
        << (void*) RunningFibersFCB << " "
        << "Block() "
        << " RunningFibersFCB == ReadyQueueHead "
        << " ReadyQueueHead = ReadyQueueHead->Next "
        << ReadyQueueHead
        << EndLogLine;
      }

    // Pull running thread's FCB out of ReadyQueueHead
    FiberControlBlock *Next = RunningFibersFCB->Next;
    FiberControlBlock *Prev = RunningFibersFCB->Prev;
    Next->Prev = Prev;
    Prev->Next = Next;

    // Make running thread's FCB self referencing
    RunningFibersFCB->Next = RunningFibersFCB;
    RunningFibersFCB->Prev = RunningFibersFCB;

    if( ReadyQueueHead == RunningFibersFCB )
      ReadyQueueHead = NULL;

    Yield();

    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "Block() "
      << " finished "
      << EndLogLine;
    }


  // This starter function catches returns from the thread main
  void
  FiberStarter( void * aFCB )
    {
    assert( aFCB != NULL );
    FiberControlBlock * FCB = (FiberControlBlock *) aFCB;

    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "FiberStarter() " << " Arg FCB " << aFCB
      << " Func "           << (void*) FCB->mFiberMainFx
      << " @ "              << (void*) (&(FCB->mFiberMainFx))
      << " Arg "            << (void*) FCB->mFiberArg
      << " RTFCB->Next "    << (void*) RunningFibersFCB->Next
      << " RTFCB->Prev "    << (void*) RunningFibersFCB->Prev
      << " ReadyQueueHead " << (void*) ReadyQueueHead
      << " RQH->Head "      << (void*) ReadyQueueHead->Next
      << " RQH->Prev "      << (void*) ReadyQueueHead->Prev
      << EndLogLine;

    assert( RunningFibersFCB == FCB );
    assert( FCB->mFiberMainFx != NULL );

    FCB->mFiberMainFx( FCB->mFiberArg );

    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "FiberStarter() " << " Returned from FiberMainFx() FCB " << aFCB << EndLogLine;

    FCB->mExited = 1;

    // do join stuff here... should be a way to detach threads
    if( FCB->mJoiner != NULL )
      {
      assert( FCB->mDetatched == 0 );
      FCB->mDetatched = 1;
      Unblock( FCB->mJoiner );
      }

    if( FCB->mDetatched )
      {
      BegLogLine( PK_FIBER )
        << (void*) RunningFibersFCB << " "
        << "FiberStarter() "
        << " Detached Fiber calling free() on stack @"
        << (void*) FCB->mFiberMemoryMallocBase
        << EndLogLine;
      //free( (char*) FCB->mFiberMemoryMallocBase );
      Platform::Heap::Free( (char*) FCB->mFiberMemoryMallocBase );
      }
    // PROBLEM HERE - we've freed the stack but we need to call yield.
    // For now, we use the stack one last time
    Block();
    }

  void
  Join( FiberControlBlock *aFCB )
    {
    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "Join() "
      << " starting join on FCB " << (void*) aFCB
      << " aFCB->mExited " << aFCB->mExited
      << " aFCB->mDetatched " << aFCB->mDetatched
      << EndLogLine;
    if( aFCB->mExited == 1 && ! aFCB->mDetatched )
      {
      BegLogLine( PK_FIBER )
        << (void*) RunningFibersFCB << " "
        << "Join() "
        << " TargetFCB->mExited == 1 && mDetached == 0 "
        << " aFCB "         << (void*) aFCB
        << " stack addr "   <<  aFCB->mFiberMemoryMallocBase
        << EndLogLine;
      /////free( aFCB->mFiberMemoryMallocBase );   // this should perhaps be done after the join
      Platform::Heap::Free( (char*) aFCB->mFiberMemoryMallocBase );
      return;
      }
    else
      {
      assert( aFCB->mJoiner == NULL ); // for now, only one joiner
      assert( RunningFibersFCB != aFCB );  // would be deadlock without third party kill
      aFCB->mJoiner = RunningFibersFCB;
      Block();
      }
    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "Join() "
      << " finished join on FCB " << (void*) aFCB
      << EndLogLine;
    }

  // CreateFiberControlBlock() allocates a tcb/stack area.  This area is buffered so that
  // thread ids can be the actual address of the FCB.

  void
  CreateFiberControlBlock( FiberControlBlock * *aFCB, unsigned aReqStackSize )
    {
    unsigned MallocStackSize = FCB_AlignmentPadding + sizeof( FiberControlBlock ) + aReqStackSize;

    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "CreateFCB() "
      << " ReqStackSize "          << (void*) aReqStackSize      << " " << aReqStackSize
      << " sizeof( FiberControlBlock ) " << (void*) sizeof( FiberControlBlock ) << " " << sizeof( FiberControlBlock )
      << " FCB_AlignmentPadding "   << (void*) FCB_AlignmentPadding << " " << FCB_AlignmentPadding
      << " MallocStackSize "       << (void*) MallocStackSize << " " << MallocStackSize
      << EndLogLine;

    unsigned MallocAddr = (unsigned) Platform::Heap::Allocate( MallocStackSize );

    if( MallocAddr == 0 )
      PLATFORM_ABORT( "Failed to allocate a thread stack" );

    // Force allignment on stack to reduce the number of significant bits in the thread id/address
    // Note that FCB is at the high address end of the malloced region.  The stack begins at the base
    // (low) address of the FCB and grows toward zero.
    unsigned FCB_Address = ( MallocAddr + aReqStackSize + FCB_AlignmentPadding - 1 ) &  ( ~FCB_AlignmentMask);

    FiberControlBlock * FCB = (FiberControlBlock *) FCB_Address;

    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "CreateFCB() "
      << " New FCB "              << (void*) FCB
      << " aReqStackSize "        << (void*) aReqStackSize
      << " MallocSize "           << (void*) MallocStackSize
      << " MallocAddr "           << (void*) MallocAddr
      << " Used FCB addr "        << (void*) FCB_Address
      << EndLogLine;


    FCB->mExited = 0;
    FCB->mFiberMemoryMallocBase = (void *) MallocAddr;  // so we can free the the stack later
    FCB->mFiberMainFx           = NULL;
    FCB->mFiberArg              = 0;
    FCB->mJoiner                 = NULL;
    FCB->mDetatched              = 0;
    FCB->Next                    = FCB;
    FCB->Prev                    = FCB;


    *aFCB = FCB;
    }

  FiberControlBlock *
  CreateFiber( unsigned                 aReqStackSize,
                BG_CoRoutineFunctionType aFiberMainFx,
                void *                   aFiberArg )
    {
    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "CreateFiber() "
      << " aReqStackSize "      << (void*) aReqStackSize
      << " aFiberMainFx "      << (void*) aFiberMainFx
      << " aFiberArg "         << (void*) aFiberArg
      << " FiberStarter "      << (void*) FiberStarter
      << EndLogLine;

    FiberControlBlock *FCB;

    // Allocate the block of memory that will hold the FCB and the thread stack.
    // Only one addr - FCB - is returned.  The stack is toward zero from this and the tcb the other way.
    CreateFiberControlBlock( & FCB, aReqStackSize );

    FCB->mFiberMainFx           = aFiberMainFx;
    FCB->mFiberArg              = aFiberArg;

    BegLogLine( PK_FIBER )
      << (void*) RunningFibersFCB << " "
      << "CreateFiber() "
      << " New FCB "            << (void*) FCB
      << " FCB->mFiberMainFx " << (void*) FCB->mFiberMainFx
      << " FCB->mFiberArg "    << (void*) FCB->mFiberArg
      << " FCB->SwapData @"     << (void*) &FCB->SwapData
      << " sizeof(SwapData) "   << sizeof( FCB->SwapData )
      << " FiberStarter "      << (void*) FiberStarter
      << EndLogLine;

    // Init all but the main threads SwapData area.
    // Note: FCB us used as the stack pointer since it is aligned.
    // Stack grows from FCB toward zero, tcb body is the other way.
    _BG_CoRoutine_Init( & FCB->SwapData, FiberStarter, FCB, (void*)FCB  );

    Unblock( FCB );

    return( FCB );
    }

  static
  void
  InitFiberSystem()
    {
    assert( RunningFibersFCB == NULL );
    CreateFiberControlBlock( & RunningFibersFCB, 0);
    RunningFibersFCB->Next = RunningFibersFCB;
    RunningFibersFCB->Prev = RunningFibersFCB;
    ReadyQueueHead = RunningFibersFCB;  // main thread is ready until it blocks
    assert( RunningFibersFCB );
    return;
    }
  }; // End struct Fiber
#endif
