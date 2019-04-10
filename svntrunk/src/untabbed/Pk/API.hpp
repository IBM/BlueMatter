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
 #ifndef __PK_API_HPP__
#define __PK_API_HPP__

#include <rts.h>
static long long PkTimeGetLongLong(void)
{
  return rts_get_timebase() ;
}

#include <BonB/BGL_PartitionSPI.h>

// Node         = chip, address space, cache levels closer to main memory
// Core         = processor, process, thread. some environments may support virtual cores.
// Fiber        = coroutine, user thread
// Actor Packet = a single network packet sent to a Proc

// Heap           Dynamic memory allocation with the scope of a node.

// inlcudes
#include <Pk/Fiber.hpp>

#define PK_CORES_PER_NODE (2)

// data types

typedef unsigned short PkType_NodeId;

typedef long long PkType_TimeRaw;   // measured in processor clocks

//
#include <assert.h>
#ifndef NDEBUG
  #define PkAssert(cond,msg) \
  if( !(cond) )        \
    fputs(msg,stderr),fflush(stderr),assert(cond)
#else
  #define PkAssert(cond,msg)
#endif

//// Try very hard to get PLATFORM_ABORT to trap; spin if it won't trap.
//#if !defined(PLATFORM_ABORT)
//#define PLATFORM_ABORT(msg) \
//   {                        \
//    fputs(msg,stderr) ;     \
//    fflush(stderr) ;        \
//    assert(0) ;             \
//    *(int *)0=0 ;           \
//    for(;;) { }             \
//   }
//
//#endif
// PkMain is the entry point to the application code
// Depending on configuration, it will be started
// 1) only on node 0, core 0     (DEFAULT)
// 2) on all nodes, core 0       (#define PKMAIN_ALL_NODES)
// 3) on all nodes and all cores (#define PKMAIN_ALL_CORES)
// Any core which doesn't execute PkMain will execute an idle fiber in a Yield loop.
int PkMain( int, char**, char** );

// interfaces

BGL_Barrier* IntraChipBarrierPtr;

unsigned long long pkTerminationTime ;

static
inline
void
PkIntraChipBarrier( )
{
  BGL_Barrier_Pass( IntraChipBarrierPtr );
}

inline
void
PkNodeBarrier()
  {
   #ifdef PK_MPI_ALL
   // MPI_Barrier( MPI_COMM_WORLD );
   MPI_Barrier( Topology::cart_comm );
   #endif

  #ifdef PK_BLADE_SPI
     int processor_id = rts_get_processor_id() ;
     if( processor_id == 0 )
       {
       BGLPartitionBarrierComputeCore0();
       }
     else if ( processor_id == 1 )
       {
       BGLPartitionBarrierComputeCore1();
       }
     else
       {
//       	  StrongAssertLogLine(0)
//       	    << "Barrier(): hard_processor_id() did not return 0 or 1 ... not a BG/L? processor_id=" << processor_id
//       	    << EndLogLine ;
       }
  #endif
  }
//
//
// PkCoreGetCoresPerNodeCount() : Get the number of cores in each node
//
//PkFiberCreate()
//PkFiberBlock()
//PkFiberUnblock()
//PkFiberYield()
//PkFiberJoin()
//PkFiberExit()



//PkActorReserveBuffer()
//PkActorReleaseBuffer()
//PkActorEnqueueBuffer()


// includes of stuff dependant on the above but which might be included elsewhere without it.


#include <Pk/FxLogger.hpp>
#include <Pk/Time.hpp>
#include <Pk/Trace.hpp>
#include <Pk/Heap.hpp>
#include <Pk/FASched.Inline.cpp>
#include <Pk/TreeReduce.hpp>
#include <Pk/GloMem.hpp>

enum {
  PkStartNode0Core0 ,
  PkStartAllNodesCore0 ,
  PkStartAllCores
} ;

//int PkStartType ;

#endif
