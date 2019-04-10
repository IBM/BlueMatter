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
 
// These routines start the fiber system on a core

#include <Pk/API.hpp>
#include <Pk/Node.hpp>
#include <stdlib.h>

#include <BonB/BGLTorusAppSupport.h>

#ifndef PK_FASCHED_INIT_LOGGING
#define PK_FASCHED_INIT_LOGGING ( 1 )
#endif

extern const int PkStartType ;
extern const int PkTracing ;

// This fiber will be started on cores that are supposed to have a pk main
void
pkMainFiber(void*)
  {
  BegLogLine( PK_FASCHED_INIT_LOGGING )
    << "PkMainFiber(): Fiber Started."
    << " argc " << pkNode.Main_ArgC
    << EndLogLine;

//  #ifdef PKTRACE_ON
  if( PkTracing )
    {
      pkTraceServer::Init();
    
      if ( 0 == PkCoreGetId() )
         {
        TraceClient InitialBarrierStart;
        TraceClient InitialBarrierFinis;
      
        // Try to sync up trace output.
        PkCo_Barrier();
        PkCo_Barrier();
        for( int b = 0; b < 10; b++ )
           {
             PkCo_Barrier();
             InitialBarrierStart.HitOE( 1,
                                        "InitialBarrier",
                                        0,
                                        InitialBarrierStart );
      
             PkCo_Barrier();
             InitialBarrierFinis.HitOE( 1,
                                        "InitialBarrier",
                                        0,
                                        InitialBarrierFinis );
          }
      }
     }
//  #endif

  PkMain( pkNode.Main_ArgC - pkNode.PkArgCount,
          pkNode.Main_ArgV + pkNode.PkArgCount,
          pkNode.Main_EnvP );

//  #ifdef PKTRACE_ON
    if ( PkTracing )
       {
          pkTraceServer::FlushBuffer();
       }
//  #endif

  //_exit(__LINE__*-1);
  return;
  }

void _pkExit(int rc)
 {
  int coreId = PkCoreGetId() ;
  if ( 0 != coreId)
  {
    // Core 0 may be running destructors for items touched on core 1. Evict core 1's dcache to 
    //  satisfy coherence requirements for this case.
    rts_dcache_evict_normal() ;
  }
    if( PkTracing )
      {
           pkTraceServer::FlushBuffer();
      }
    if ( 0 == coreId )
       {
        BegLogLine(1) 
          << "_pkExit called, core 0 stopping now."
          << EndLogLine ;
        if( PkTracing )
          {
            BegLogLine(PK_FASCHED_INIT_LOGGING)
              << "Spinning to allow core 1 to write its trace"
              << EndLogLine ;
          long long TerminationTime = PkTimeGetExpiry(pkTerminationTime, 5) ;
            while (PkTimeGetRaw() < TerminationTime)
            {
              // Nothing
            }
              BegLogLine(PK_FASCHED_INIT_LOGGING)
                << "Exiting anyway"
                << EndLogLine ;
            
          
          }        
        exit(rc) ;
       } 
    else 
       {
        BegLogLine(1) 
          << "_pkExit called, aux core spinning now."
          << EndLogLine ;
        for(;;) ; 
       }
    abort() ; // Not expecting to get here ...
 }  

#ifndef PK_MAIN_STACK_SIZE_MB
#define PK_MAIN_STACK_SIZE_MB (8)
#endif

#include <Pk/FASched.hpp>

void
_pkCreatePkMainFiber()
  {
  BegLogLine( PK_FASCHED_INIT_LOGGING )
    << "pkCreatePkMainFiber(): Fiber Started."
    << EndLogLine;

  pkFiberControlBlockT *rc = PkFiberCreate( (PK_MAIN_STACK_SIZE_MB * (1024*1024)),
                                             pkMainFiber,
                                             NULL );
  BegLogLine( PK_FASCHED_INIT_LOGGING )
    << "pkCreatePkMainFiber(): Fiber Finished."
    << EndLogLine;
  }

// This is the core entry point - on aix, this would be a pthread.
void
_pkCoreMain(void *)
  {
  // LOG LINES NOW AVAILABLE
  BegLogLine( PK_FASCHED_INIT_LOGGING )
    << "_pkCoreMain(): "
    << " Running."
    << EndLogLine;

  BegLogLine(PKFXLOG_NODE)
    << "PkNodeXYZ::Init  mMaskX=" << pkNode.NodeXYZ.mMaskX
    << " mMaskY=" << pkNode.NodeXYZ.mMaskY
    << " mShiftY=" << pkNode.NodeXYZ.mShiftY
    << " mShiftZ=" << pkNode.NodeXYZ.mShiftZ
    << EndLogLine ;

  // Create a FiberControlBlock for the current execution context presumed to be the
  // first and only one on this core.
  _pkFiberActorSystemPerCoreInit();

  int ThisCoreCreatesAPkMainFiber = 0;

  // In cases where required, start a PkMain on the current core.
  if( PkStartType == PkStartAllCores )
    {
    BegLogLine( PK_FASCHED_INIT_LOGGING ) << "_pkCoreMain(): " << "Creating main fiber on ALL CORES." << EndLogLine;
    _pkCreatePkMainFiber();
    ThisCoreCreatesAPkMainFiber = 1;
    }
  else if ( PkStartType == PkStartAllNodesCore0 )
    {
    if( PkCoreGetId() == 0 )
      {
      BegLogLine( PK_FASCHED_INIT_LOGGING ) << "_pkCoreMain(): " << "Creating main fiber on ALL NODES." << EndLogLine;
      _pkCreatePkMainFiber();
      ThisCoreCreatesAPkMainFiber = 1;
      }
    }
  else
    {
    if( PkNodeGetId() == 0 && PkCoreGetId() == 0 )
      {
      BegLogLine( PK_FASCHED_INIT_LOGGING ) << "_pkCoreMain(): " << "Creating main fiber on ALL CORES of ALL NODES." << EndLogLine;
      _pkCreatePkMainFiber();
      ThisCoreCreatesAPkMainFiber = 1;
      }
    }

  if( ThisCoreCreatesAPkMainFiber )
    {
    BegLogLine( PK_FASCHED_INIT_LOGGING )
      << "_pkCoreMain(): "
      << "transfering control to PkMainFiber for inital run phase"
      << EndLogLine;

    // this makes the MainFiber next to run (if there is one)
    PkAdvanceReadyQueueHead();

    // this swaps control to the next to run fiber without checking sched or comms
    PkFiberSwapToReadyQueueHead();
    }

  BegLogLine( PK_FASCHED_INIT_LOGGING )
    << "_pkCoreMain(): "
    << "CoreMain fiber Blocking to communications and PkMain fiber if created"
    << EndLogLine;

  // any core not starting a PkMain immediately blocks.
  PkFiberBlock();
  }

