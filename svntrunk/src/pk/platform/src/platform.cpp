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
 #ifndef PKFXLOG_PLATFORM_STARTUP
#define PKFXLOG_PLATFORM_STARUP 1
#endif

#ifndef PK_THREAD_STACKSIZE_MB
#define PK_THREAD_STACKSIZE_MB 10
#endif

#ifndef PKFXLOG_PLATFORM_THREADCEATE
#define PKFXLOG_PLATFORM_THREADCEATE ( 0 )
#endif

#ifndef PKFXLOG_BLADE_PROFILING
#define PKFXLOG_BLADE_PROFILING ( 1 )
#endif

#ifndef PKFXLOG_PLATFORM_THREADSTART
#define PKFXLOG_PLATFORM_THREADSTART ( 0 )
#endif

#ifndef PKFXLOG_IMPORT_FENCE_ACTOR
#define PKFXLOG_IMPORT_FENCE_ACTOR ( 0 )
#endif

#ifndef PKFXLOG_EXPORT_FENCE_ACTOR
#define PKFXLOG_EXPORT_FENCE_ACTOR ( 0 )
#endif

#ifndef PKFXLOG_SEND_REDUCTION_DATA
#define PKFXLOG_SEND_REDUCTION_DATA ( 0 )
#endif

#ifndef PKFXLOG_VIRTUAL_FIFO
#define PKFXLOG_VIRTUAL_FIFO ( 0 )
#endif

#ifndef PKFXLOG_NEW
#define PKFXLOG_NEW ( 0 )
#endif

#ifndef PKFXLOG_MAIN
#define PKFXLOG_MAIN ( 0 )
#endif

#ifndef PKFXLOG_IONODEWORLD_IOP
#define PKFXLOG_IONODEWORLD_IOP ( 0 )
#endif

#ifndef PKTRACE_IOP_POLL
#define PKTRACE_IOP_POLL ( 0 )
#endif

#ifndef PKFXLOG_COLLECTIVE
#define PKFXLOG_COLLECTIVE ( 0 )
#endif

#ifndef PKFXLOG_COPROCESSOR
#define PKFXLOG_COPROCESSOR (0)
#endif

#ifndef PKFXLOG_QSORT
#define PKFXLOG_QSORT (0)
#endif

#ifndef PKFXLOG_QSORT_DETAIL
#define PKFXLOG_QSORT_DETAIL (0)
#endif

#ifdef PK_BLADE
#define BGL_VIRTUAL_FIFO
#endif

#define PKTRACE_TUNE 1

#ifndef ONE_LOG_FILE_PER_NODE
#define ONE_LOG_FILE_PER_NODE (0)
#endif

#include <pk/platform.hpp>

#include <rptree.hpp>

#if defined(PK_BGL) && !defined( PK_BGL_USE_POSIX_MALLOC )
#include <pk/HeapManager.hpp>
#endif


// This file is the compilation containing platform global variables.
// On instance of these are expected per address space


// From pkfxlog.hpp

// A reactor thread function must be linked in.
// NEED: try to encapsulate this so that there is a common interface to starting the reactor.
extern void * ReactorThreadMain( void * );
extern void * ReactorThread( void * );
extern void   ReactorInitialize();

extern "C" {
extern void * PkMain( int, char**, char** );
};

int     Platform_ArgC = 0;
char ** Platform_ArgV = NULL;
char ** Platform_EnvP = NULL;

extern "C" void moncontrol(int) ; // Turning on and off the profiling timer

#ifndef PK_BLADE // WHAT IS THIS DOING HERE??????? IT SHOULD BE IN HPP, NO?
#if defined(PK_SMP)
#include <pthread.h>
#endif
#endif

#ifdef PK_SMP
#ifdef PK_AIX
#include <pk/setpri.hpp>
#include <sys/processor.h>
#include <sys/thread.h>
#endif
#endif

#if defined(PK_BLADE) && defined(PK_BGL)
#define BLADE_PROFILING_TRACE_DATA_SIZE 1024 * 1024
//#define BLADE_PROFILING_TRACE_DATA_SIZE 100

int FullPartitionClass ;
int IntegerReductionClass ;
int ProfilingReductionClass ;
int FullPartitionRoot = 0 ;


int IO_COMP_comm_class;
int IO_COMP_comm_root;

char RecvReduceBuff[ 256 ] MEMORY_ALIGN(5);
char SendReduceBuff[ 256 ] MEMORY_ALIGN(5);

#define NUMBER_OF_ADDRESSES_PER_PACKET 64

unsigned long ProfilingRecvBuff[ NUMBER_OF_ADDRESSES_PER_PACKET ] MEMORY_ALIGN(5);
unsigned long ProfilingSendBuff[ NUMBER_OF_ADDRESSES_PER_PACKET ] MEMORY_ALIGN(5);

// double PofilingHistogramWindow[ 256 ] MEMORY_ALIGN(5);

unsigned long BladeProfilingTraceData[ BLADE_PROFILING_TRACE_DATA_SIZE ] MEMORY_ALIGN(5);
// _BGL_TreeFifoStatus treeSendStatus MEMORY_ALIGN(5);


AtomicNative<Bit32> serv_ip_addr;
AtomicNative<Bit16> serv_ip_port;

char * UdpPacketSentPtr = NULL;
int    UdpPacketSentLen = 0;
int UdpPacketAckedFlag=0;
int UdpPacketSeqNum=0;
int UdpPacketIndex=0;

UdpPacket udpPacket MEMORY_ALIGN(5);

//char SendPacketBuffer[ 2048 ] MEMORY_ALIGN(5);
//char SendPacketBuffer[ 2048 ] MEMORY_ALIGN(5);

#endif

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
/**********************************************
 *
 *********************************************/
#if defined(PK_BLADE) && defined(PK_BGL)
////////////#include <math.h>
extern "C" {

_BGL_TreeHwHdr addReduceHdr MEMORY_ALIGN(5);
_BGL_TreeHwHdr addReduceHdrReturned MEMORY_ALIGN(5);
_BGL_TorusPktHdr   SendReductionDataHdr MEMORY_ALIGN(5);

int BGLInformationChoice(int newchoice);

int BGL_TreeRouteComputeNodes(
  int *root,              // INOUT: the root node.
                          //   If *root>=0 on input, use that *root.
                          //   else retun optimal *root on output.
  int broadcast,          // IN: 0/1 = broadcast no/yes:
                          //           no  = only root node receive packet.
                          //           yes = all compute nodes receive packet.
  int *class1              // OUT: the class of the virtual tree.
);

typedef enum {
  // Assume root node has local source.

  TR_PKT_ONLY_TO_ROOT_NODE,
  // Only the root node, no other node, gets a copy of pkt sent by any node,
  // even if packet is injected by root.
  // All nodes including root participate in reductions
  // and the result goes only to root.

  TR_PKT_TO_ALL_NODES,
  // All nodes, including root, get a copy of pkt sent by any node,
  // including root.
  // All nodes including root participate in reductions
  // and the result goes to all nodes, including root.

  TR_PKT_TO_ROOT_XOR_OTHER_NODES,
  // A packet sent by root, only goes to other nodes, not back to the root.
  //   e.g. in bstrap tree bcast of kernel+application.
  // A packet sent by any non-root node only goes to the root node.
  // All nodes, except the root node, participate in reductions
  // and the result goes only to root.
} TR_PKT_ROOT_BEHAVIOR;


int BGL_TreeRouteAllNodes(
  int *root,              // INOUT: the root node.
                          //   If *root>=0 on input, use that *root.
                          //   else retun optimal *root on output.
  TR_PKT_ROOT_BEHAVIOR behavior, // IN: See BGL_TreeRoute.h
  int *Class              // OUT: the class of the virtual tree.
);

int BGL_TreeRuntimeBfsRoute(
  int my_rank,            // rank of calling node
  int root,               // rank of root node
  int node_set            // 0=all nodes, 1=compute nodes only
);

}
#endif
/*********************************************/
// for unix::system() call.

#ifdef PK_BGL
BGL_Barrier* Platform::Control::mIntraCoreBarrier = NULL;
#endif

using namespace std ;

// Default loadbalance tuning parameters (used by ORB vector-verlet)
// indicating 'how many times as expensive is a below-switch IFP eval than an out-of-range one'
// and 'how many times as expensive is an in-switch IFP eval than an out-of-range one'

double Platform::LoadBalance::mTune[] = { 5.57, 6.36 } ;

int Platform::Topology::xSize_cmdprompt = 0;
int Platform::Topology::ySize_cmdprompt = 0;
int Platform::Topology::zSize_cmdprompt = 0;
int Platform::Topology::mVoxDimx = 0;
int Platform::Topology::mVoxDimy = 0;
int Platform::Topology::mVoxDimz = 0;

int Platform::Topology::mfftProcDimx = 0;
int Platform::Topology::mfftProcDimy = 0;
int Platform::Topology::mfftProcDimz = 0;


int Platform::Topology::mDimX = 0;
int Platform::Topology::mDimY = 0;
int Platform::Topology::mDimZ = 0;

int Platform::Topology::mMachineDimX = 0;
int Platform::Topology::mMachineDimY = 0;
int Platform::Topology::mMachineDimZ = 0;

int Platform::Topology::ThisAddressSpaceId = -1;

int Platform::Topology::mSubPartitionCount = 1;
int Platform::Topology::mSubPartitionId = 0;

int* Platform::Collective::A2ASendCount  = NULL;
int* Platform::Collective::A2ASendOffset = NULL;
int* Platform::Collective::A2ARecvCount  = NULL;
int* Platform::Collective::A2ARecvOffset = NULL;

int* Platform::Collective::SendNodeSet = NULL;
int* Platform::Collective::RecvNodeSet = NULL;

int Platform::Collective::SendNodeSetCount = 0;
int Platform::Collective::RecvNodeSetCount = 0;

int* Platform::Collective::BcastSendNodeSet = NULL;
int* Platform::Collective::BcastRecvNodeSet = NULL;

int Platform::Collective::BcastSendNodeSetCount = 0;
int Platform::Collective::BcastRecvNodeSetCount = 0;

int* Platform::Collective::ReduceSendNodeSet = NULL;
int* Platform::Collective::ReduceRecvNodeSet = NULL;

int Platform::Collective::ReduceSendNodeSetCount = 0;
int Platform::Collective::ReduceRecvNodeSetCount = 0;


void Platform::Profiling::TurnOn(void)
{
        moncontrol(1) ;
}

void Platform::Profiling::TurnOff(void)
{
        moncontrol(0) ;
}
void Platform::Algorithm::Qsort(void *base,
                size_t nmemb,
                size_t size,
        int(*compar)(const void *, const void *)
       )
{
        BegLogLine(PKFXLOG_QSORT)
          << "Platform::Algorithm::Qsort base=0x" << base
          << " nmemb=" << nmemb
          << " size=" << size
          << " compar=0x" << (void *) compar
          << " nmemb*size=" << nmemb*size
          << EndLogLine ;

        // glibc 'qsort' will call malloc if it needs more than 1 Kbyte of
        // scratch space (nmemb*size >= 1024). This won't work on BG/L core 1.
        // For the moment, trap out if we try sorting anything on core 1.
    if (Platform::Topology::GetLocationInASIC() != 0 )
    {
        BegLogLine(1)
          << "Platform::Algorithm::Qsort called on core 1, halting !!!"
          << " base=0x" << base
          << " nmemb=" << nmemb
          << " size=" << size
          << " compar=0x" << (void *) compar
          << " nmemb*size=" << nmemb*size
          << EndLogLine ;
        PLATFORM_ABORT("Platform::Algorithm::Qsort called on core 1") ;
    }
        qsort(base,nmemb,size,compar) ;
}



#if defined(PK_BGL)
#if !defined( PK_BGL_USE_POSIX_MALLOC )
static HeapManager HM ;
#endif

void *
Platform::Heap::Allocate(size_t aRequest)
  {
  void * mem = NULL;
  #if defined( PK_BGL ) && !defined( PK_BGL_USE_POSIX_MALLOC )
    BegLogLine(0) << "Platform::Heap::Allocate() Size " << aRequest << EndLogLine;
    mem = HM.bgl_malloc(aRequest) ;
    BegLogLine(0) << "Platform::Heap::Allocate() Addr " << mem << EndLogLine;
  #else
    mem = malloc( aRequest );
  #endif
  return( mem );
  }

void
Platform::Heap::Free(void * aAddress)
  {
  #if defined( PK_BGL ) && !defined( PK_BGL_USE_POSIX_MALLOC )
    BegLogLine(0) << "Platform::Heap::Free() " << (void*)aAddress << EndLogLine;
    HM.bgl_free( aAddress ) ;
  #else
    free( aAddress );
  #endif
  }

//////#else
//////void * Platform::Heap::Allocate(size_t aRequest)
//////{
//////return malloc(aRequest) ;
//////}
//////void Platform::Heap::Free(void * aAddress)
//////{
//////free(aAddress) ;
//////}
#endif

#ifdef PK_BLADE_SPI
GroupContainer Platform::Topology::mGroup;
#endif

#if defined( PK_MPI_ALL )
MPI_Comm Platform::Topology::cart_comm;
#endif

int Platform::Topology::Size = -1;
int      Platform::Comm::mRemotePort = 0;
unsigned Platform::Comm::mRemoteHostIP = 0;
int      Platform::Comm::mDebugP1_5ListeningPort = 0;
struct sockaddr_in Platform::Comm::mRemoteAddr;
int sockfd;

Platform::Mutex::MutexWord   Platform::FxLogger::LogLockVar;
int                          Platform::FxLogger::LogFd = -1;
unsigned                     LogMask         = 0xFFFFFFFF;  // default everything on

Platform::ExternalDatagramInterface::Address Platform::ExternalDatagramInterface::mAddress;
Platform::ExternalDatagramInterface::Address Platform::ExternalDatagramInterface::mRDG_Ext;
Platform::Mutex::MutexWord                   Platform::ExternalDatagramInterface::RdgLockVar;
Platform::Mutex::MutexWord                   KSpaceForceMutex;

int                     Platform::Reactor::mGoAhead = 0;
PointerToThreadFunction Platform::Reactor::mCurrentThreadFx;
void*                   Platform::Reactor::mCurrentData = NULL;

AtomicNative< unsigned >  myKend;

//#if defined(PK_BGL)
//char  Platform::ExternalDatagramInterface::Buffer[ Platform::ExternalDatagramInterface::kBufferSize] ;
//unsigned int Platform::ExternalDatagramInterface::BufferIndex ;
//#endif

double                       Platform::Clock::mTimeBase;          // This is the base value
Platform::Clock::TimeValue   Platform::Clock::mTimeValueBase;

// tjcw 20050224 mSPI_TimeEvent is not currently declared anywhere
//#ifdef PK_SPI
//// This would need to be atomic.
//unsigned int                          Platform::Clock::mSPI_TimeEvent = 0;   // Uses a call count to make time an event ordering.
//#endif

int Platform::FxLogger::OpenLogPort()
  {
// BGL bringup doesn't support functional logging
#if !defined(PK_BLADE)
  //See if log file has been opened.  This deals with multithreading
  //NEED: this should be handled differently.  Mayby FileHandle should have a method?
  if( LogFd == -1 )
    {

    #ifndef PK_BGL //// THIS LOCK HAS NOT BEEN ALLOCATED : NEED TO WORK ON MUTEX VS LOCKS
    Platform::Mutex::YieldLock( &LogLockVar );  // Protect against other threads doing init.
    #endif

    if( LogFd == -1 )          // Check again after gaining lock.
      {

      if( (ONE_LOG_FILE_PER_NODE) || (Platform::Topology::mSubPartitionCount > 1) )
        {
        char LogFileName[1024];

        sprintf( LogFileName,
                 "S.%05d:%05d.N.%05d:%05d.LOG",
                  Platform::Topology::mSubPartitionId,
                  Platform::Topology::mSubPartitionCount,
                  Platform::Topology::GetAddressSpaceId(),
                  Platform::Topology::GetAddressSpaceCount()
                  );

  #ifdef PK_AIX
        int open_perms = S_IRUSR | S_IWUSR ;  //AIXism
  #else
        int open_perms = 0770 ;
  #endif

        // open a file for each node.
        int open_flags = O_CREAT | O_TRUNC | O_WRONLY | O_APPEND;  // O_APPEND Required to ensure proper writes by threaded apps
        LogFd = open( LogFileName , open_flags, open_perms );

        }
      else
        {
        // Use stdout
        LogFd = 1;
        }


      if( LogFd < 0 )  // Log file failed open.
        PLATFORM_ABORT("Failed to open fxlog file");

      }

    #ifndef PK_BGL //// THIS LOCK HAS NOT BEEN ALLOCATED : NEED TO WORK ON MUTEX VS LOCKS
    Platform::Mutex::Unlock( &LogLockVar );
    #endif

    }
#endif
  return(0);
  }

void
Platform::Coprocessor::Init()
  {
    BegLogLine(PKFXLOG_COPROCESSOR)
      << "Platform::Coprocessor::Init" 
      << EndLogLine ;
  #ifdef PK_BLADE_SPI
    co_co_init();
  #endif
  }

 int
 Platform::Coprocessor::Start( CO_FuncPtr aFp, void* aArg, int aArgSize )
   {
     BegLogLine(PKFXLOG_COPROCESSOR)
       << "Platform::Coprocessor::Start aFp=0x" << (void *) aFp
       << " aArg=0x" << (void *) aArg
       << " aArgSize=" << aArgSize
       << EndLogLine ;
     int rc ;
     #if defined( PK_BGL )
	   #if defined( PK_BLADE_SPI )
         rc=co_co_start( aFp, aArg, aArgSize );
       #elif defined( PK_MPI_ALL )
         // Note:: PMI interfaces need MPI_Init() to be called first.
         rc= PMI_co_start( aFp, aArg );
       #else
         assert(0);
         rc=-1;
       #endif
     #else
       assert(0);
       rc=-1;
     #endif
       
     BegLogLine(PKFXLOG_COPROCESSOR)
       << "Platform::Coprocessor::Start rc=" << rc
       << EndLogLine ;
     return rc ;
   }

 void
 Platform::Coprocessor::Join1( int aHandle )
   {
     BegLogLine(PKFXLOG_COPROCESSOR)
       << "Platform::Coprocessor::Join1 aHandle=" << aHandle
       << EndLogLine ;
   #if defined( PK_BGL )
     #if defined( PK_BLADE_SPI )
       co_co_join();
     #elif defined( PK_MPI_ALL )
       // Note:: PMI interfaces need MPI_Init() to be called first.
       PMI_co_join( aHandle );
     #endif
   #else
     assert( 0 );
   #endif
   }

void Platform::Coprocessor::Join( int aHandle )
    {
      BegLogLine(PKFXLOG_COPROCESSOR)
        << "Platform::Coprocessor::Join aHandle=" << aHandle
        << EndLogLine ;
    #if defined( PK_BGL )
      Join1( aHandle );

      // NOTE: Flush for SPI happens in the co_co_poll routine();

     #if defined( PK_MPI_ALL )
      int handle = Start( FlushCore1, NULL, 0 );
      Join1( handle );
     #endif

    #else
      assert( 0 );
    #endif
    }


//***************** Platform::Initialize *********************

typedef struct
  {
  Platform::Mutex::MutexWord   LockVar;        // Needs to be mutex'd
  PointerToThreadFunction     aThreadFx;       // What function to run
#ifdef PK_BGL
  unsigned int                 tid;   // I guess this is just going to be the physical processor id
  void *                       attr;  // dummy
#else //  Assume pthreads otherwise.  Maybe this struct should be in platform.hpp?
  pthread_t                    tid;
  pthread_attr_t               attr;
#endif
  void                        *ArgPtr;
  int                          Flags;
  int                          StackSize;
  int                          ProcAssign;
  void                        *Rc;
  int                          ErrorNo;
  } ThreadContext;


static ThreadContext ThreadContextArray[ Platform::Thread::MAX_THREAD_COUNT ];
static int           ThreadContextNext = Platform::Thread::MAX_THREAD_COUNT - 1;
static int           TotalThreadsAllocated = 0;

static int           PhysicalCpuCount = 4;

// Old SPI returned nothing; current SPI mapped through pthreads
// #ifdef PK_SPI
#if 0
static void
ThreadStart( void * aThreadContextArrayIndex )
#else
static void *
ThreadStart( void * aThreadContextArrayIndex )
#endif
  {
#if !defined( PK_BGL )

  BegLogLine( PKFXLOG_PLATFORM_THREADSTART )
    << " ThreadStart() running "
    << EndLogLine;

#ifdef PK_AIX
  //THIS FX DOESN'T DO MUCH but sit at the base of the newly created
  //threads call stack and wait for someone to code evil function
  //into it.
  PointerToThreadFunction ThreadFx =
    ThreadContextArray[ (int) aThreadContextArrayIndex ].aThreadFx;
  void *ArgPtr =
    ThreadContextArray[ (int) aThreadContextArrayIndex ].ArgPtr;



//   int myRank=0;
//   MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

//   bindprocessor( BINDTHREAD,
//                  thread_self(),
//                  Platform::Topology::ThisAddressSpaceId % 4 );



  if( ThreadContextArray[ (int) aThreadContextArrayIndex ].ProcAssign != -1 )
    {
    bindprocessor( BINDTHREAD,
                   thread_self(),
                   ThreadContextArray[ (int) aThreadContextArrayIndex ].ProcAssign % PhysicalCpuCount);
    }



  // Fix the priority if the current thread, which is newly created.
  SchedTuneSelf();
#endif


  BegLogLine( PKFXLOG_PLATFORM_THREADSTART )
    << "ThreadStart(): STARTING Thread "
    << (int) aThreadContextArrayIndex
    << " ArgPtr "
    << ArgPtr
    << EndLogLine;

  Platform::Memory::ImportFence();

  void *rc = (*ThreadFx)(ArgPtr);

  BegLogLine( PKFXLOG_PLATFORM_THREADSTART )
    << "ThreadStart(): ENDING Thread "
    << (int) aThreadContextArrayIndex
    << " ArgPtr "
    << ArgPtr
    << EndLogLine;

  //NEED: Should call Platform::Threads::Exit().
// New SPI is pthreads
// #ifdef PK_SPI
#if 0
  return;
#else
  return( rc );
#endif

#else
  return ( NULL );
#endif

  };



#if !defined(PK_BGL)
int
Platform::Thread::
Create( PointerToThreadFunction  aThreadFx,       // What function to run
        void                    *aArg,                      // Argument data pointer
        int                      aProcAssign,
        int                      aStackSize,
        int                      aFlags      )          // Operation flags - eg: sync.
  {
  int rc = -1;  // must be set to 0 for success.

  int CreatorThreadId = Platform::Thread::GetId();

  BegLogLine( PKFXLOG_PLATFORM_THREADCEATE )
    << "Platform::Thread::Create() "
    << " Fx@ "
    << (void *) aThreadFx
    << " ProcAssign "
    << aProcAssign
    << " StackSize "
    << aStackSize
    << " Flags "
    << (void *) aFlags
    << EndLogLine;

  int NewThreadIndex = -1;

#if 0
  // THIS BLOCK DIDN'T WORK WELL
  // It equated processor assignment with user level logical thread id assignment.
  // NEED: probably to have an additional argument of aLogicalThreadAssign or some such.

  if( aProcAssign != -1 )
    {
    //A Create with an assigned processor/id MUST be able to have it.
    assert( Platform::Mutex::TryLock( ThreadContextArray[ aProcAssign ].LockVar ) );
    NewThreadIndex = aProcAssign;
    }
  else

#endif

    {
    // This block of code starts giving out thread ids from Max down to 0.
    NewThreadIndex = ThreadContextNext;
    while( ! Platform::Mutex::TryLock( &(ThreadContextArray[ NewThreadIndex ].LockVar) ) )
      {
      BegLogLine( PKFXLOG_PLATFORM_THREADCEATE )
        << "Platform::Thread::Create(): failed to lock thread index "
        << NewThreadIndex
        << EndLogLine;
      while( NewThreadIndex == ThreadContextNext )  // No point in moving on till ThreadCondextNext is changed
        {
        BegLogLine( PKFXLOG_PLATFORM_THREADCEATE )
          << "Platform::Thread::Create(): waiting  "
          << NewThreadIndex
          << EndLogLine;
        assert( TotalThreadsAllocated < Platform::Thread::MAX_THREAD_COUNT );  // Blow an assert if no more threads.
        Platform::Thread::Yield();
        }
      NewThreadIndex = ThreadContextNext;  // Set up to try lock again.
      }
    ThreadContextNext--;
    }
  TotalThreadsAllocated++;

  BegLogLine( PKFXLOG_PLATFORM_THREADCEATE )
    << "Platform::Thread::Create(): "
    << " Allocated Thread "
    << NewThreadIndex
    << " Total Threads Alloced "
    << EndLogLine;

  assert( NewThreadIndex < Platform::Thread::MAX_THREAD_COUNT );

  ThreadContext *tcp = &ThreadContextArray[ NewThreadIndex ];

  tcp->aThreadFx = aThreadFx;
  tcp->Flags     = aFlags;
  tcp->Rc = NULL;
  tcp->ErrorNo = 0;
  tcp->ProcAssign = aProcAssign;
  tcp->ArgPtr = aArg;  // NO special arg copying... just the pointer.  Higher level interface might copy args


#ifdef PK_SPI
// Cyclops threads mapped through stripped-down pthreads nowadays
#if 1
  rc = pthread_create( &( tcp->tid  ), //NEED: to think about a potential race cond here.
                       NULL,
                       ThreadStart,
                       (void *) NewThreadIndex );
#else

  // can we check stack size args?

  rc = svcThreadCreate( &(tcp->tid),              // logical thread id
                        ThreadStart,              // thread entry point
                        (void *) NewThreadIndex );// index into global structure cast to void * and sent as arg
#endif

#else // assume pthreads
  pthread_attr_init( &( tcp->attr ) );

  // NEED: to code this system to automatically reap zombie threads -
  // for now, create detached.

  pthread_attr_setdetachstate( &( tcp->attr ), PTHREAD_CREATE_DETACHED);


  if( aStackSize != -1 )
    {
    pthread_attr_setstacksize( &( tcp->attr ), aStackSize);
    }
  else
    {
    pthread_attr_setstacksize( &( tcp->attr ), PK_THREAD_STACKSIZE_MB * 1024*1024 ) ;
    }
  tcp->StackSize = aStackSize;

  // THIS SHOULD BE THE ONLY PLACE WHERE pthrea_create IS CALLED!
  BegLogLine( PKFXLOG_PLATFORM_THREADCEATE )
    << "Platform::Thread::Create(): calling pthread_create for "
    << NewThreadIndex
    << EndLogLine;
  errno = 0;
  rc = pthread_create( &( tcp->tid  ), //NEED: to think about a potential race cond here.
                       &( tcp->attr ),
                       ThreadStart,
                       (void *) NewThreadIndex );
  BegLogLine( PKFXLOG_PLATFORM_THREADCEATE )
    << "Platform::Thread::Create(): returned from pthread_create for "
    << NewThreadIndex
    << " RC "
    << rc
    << " errno "
    << errno
    << EndLogLine;

#endif

  return( rc );
  }
#endif

/************************************************************************************/



//************************************************************************

static
int
AttachDebugger( char *file )
  {
  // To use this, simply edit the display name to put debug windows where
  // you want.  This could be modified to be a routine automatically called
  // when an assert fails.

#ifdef PK_MPI_ALL

  char hostname[80];
  char db_cmd[1024];

  gethostname( hostname, 80 );

  int TaskNo  = Platform::Topology::GetAddressSpaceId();
  int TaskCnt = Platform::Topology::GetAddressSpaceCount();

  sprintf( db_cmd,
           "%s %d %d %d %s %s &",
           "pkdebug",
           getpid(),
           TaskNo,
           TaskCnt,
           hostname,
           file );

  fprintf(stderr, "%s", db_cmd);
  system( db_cmd );
  sleep(1);  //Make sure poe doesn't send us oun our way too soon.
  sleep(1);
  sleep(1);
  sleep(1);
  sleep(1);
  /* kill( getpid(), 18); */
#endif

  return(0);
  }


// By default, we'll checksum only the first 64KB of the executable.
#ifndef PK_MAX_EXCUTABLE_CHECKSUM
#define PK_MAX_EXCUTABLE_CHECKSUM ( 64 * 1024 )
#endif

#ifdef PK_AIX
long _pk_ExecutableCheckSum = 0;

int
AssertExecutableCheckSum( void *TriggerMsg )
  {
  assert( *((long *)TriggerMsg) == _pk_ExecutableCheckSum );
  fprintf(stderr,"CheckSum confirmed task %d\n", Platform::Topology::GetAddressSpaceId()  );
  fflush(stderr);
  return(0);
  }

#endif


// GLOBALS
int _pk_PPL_REGLOG_fd      = 0;
int _pk_PPL_REGLOG_fd_LOCK = 0;

#include <pk/pktrace.hpp>

#if defined(PK_BLADE) && defined(PK_BGL)
void SendReductionData( void* args )
{

  BegLogLine( PKFXLOG_SEND_REDUCTION_DATA )
    << "SendReductionData:: Entering..."
    << EndLogLine;

  SendReductionDataPacket* srdp = (SendReductionDataPacket *) args;

  int FullBlocks   = srdp->mLength / _BGL_TREE_PKT_MAX_BYTES;
  int PartialBlockSize = srdp->mLength % _BGL_TREE_PKT_MAX_BYTES;
  int PartialBlockStart = FullBlocks * _BGL_TREE_PKT_MAX_BYTES;

  BegLogLine( PKFXLOG_SEND_REDUCTION_DATA )
    << "SendReductionData:: "
    << " srdp->mAddress=" << srdp->mAddress
    << " srdp->mLength=" << srdp->mLength
    << " FullBlocks=" << FullBlocks
    << " PartialBlockSize=" << PartialBlockSize
    << " PartialBlockStart=" << PartialBlockStart
    << EndLogLine;

  int i=0;
  for( i=0; i < FullBlocks; i++ )
    {
      BegLogLine( PKFXLOG_SEND_REDUCTION_DATA )
        << "SendReductionData:: Before BGLTreeSend in loop "
        <<  EndLogLine;

      // int maxSendResult = BGLTreeSend( IntegerReductionClass,  // class
      //                                  &addReduceHdr,          // Header we made before
      //                                  &srdp->mAddress[ i * _BGL_TREE_PKT_MAX_BYTES ],  // Operands being sent
      //                                  _BGL_TREE_PKT_MAX_BYTES // appropriate number of blocks, by construction
      //                                );

      checkTreeSendFifoStatusVC0();
      int maxSendResult = BGLTreeRawSendPacket( 0,  // class
                                                &addReduceHdr,          // Header we made before
                                                &srdp->mAddress[ i * _BGL_TREE_PKT_MAX_BYTES ]  // Operands being sent
                                               );

      BegLogLine( PKFXLOG_SEND_REDUCTION_DATA )
        << "SendReductionData:: After BGLTreeSend in loop"
        <<  EndLogLine;

    }

    if ( PartialBlockSize > 0 )
    {
      char * InOutEndPkt = (char *) &( srdp->mAddress[ i * _BGL_TREE_PKT_MAX_BYTES] ) ;
      char * EndPktBuf   = (char *) SendReduceBuff;

      for(int i=0; i < PartialBlockSize; i++ )
        {
          EndPktBuf[i] = InOutEndPkt[i];
        }

      BegLogLine( PKFXLOG_SEND_REDUCTION_DATA )
        << "SendReductionData:: Before BGLTreeSend partial"
        <<  EndLogLine;

//       int maxSendResult = BGLTreeSend( IntegerReductionClass,                // class
//                                        &addReduceHdr,          // Header we made before
//                                        SendReduceBuff,            // Operands being sent
//                                        _BGL_TREE_PKT_MAX_BYTES // appropriate number of blocks, by construction
//                                        );
      checkTreeSendFifoStatusVC0();
      int maxSendResult = BGLTreeRawSendPacket( 0,  // class
                                                &addReduceHdr,          // Header we made before
                                                SendReduceBuff          // Operands being sent
                                              );

      BegLogLine( PKFXLOG_SEND_REDUCTION_DATA )
        << "SendReductionData:: After BGLTreeSend partial"
        <<  EndLogLine;
    }

    Platform::Memory::ImportFence( srdp->mAddress, srdp->mLength );
}

extern "C" {
  Platform::Mutex::MutexWord IOP_CP_Barrier;
};
#endif


extern "C" {
unsigned htoi( char* str )
  {
  // s0printf("htoi: input=%s\n", str);
  unsigned rc = 0;
  for( int i=0; str[i] != '\0'; i++)
    {
       int nibble;
      if( str[i] >= '0' && str[i] <= '9' )
        nibble = str[i] - '0';
      else if( str[i] >= 'a' && str[i] <= 'f' )
        nibble = str[i] - 'a' + 10;
      else if( str[i] >= 'A' && str[i] <= 'F' )
        nibble = str[i] - 'A' + 10;
      else
        PLATFORM_ABORT( "Bad value passed to htoi - must be hex digits 0-9 A-F only (no 0x)");
      rc = (rc << 4) + nibble;
    }

  // s0printf("htoi: output=%08x\n", rc);
  return(rc);
  }
};

char *rdgnameinargv = 0;

void AbortArgv( char *msg )
{
    perror( msg );
    PLATFORM_ABORT( msg );
}

int
ComputeNodeWorld_CP( int argc, char *argv[], char **envp )
  {

/// HACK to not interfere with MPI's locks.
#ifdef PK_BGL
BGL_Mutex* hack1;
for( int i=0; i<16; i++ )
  {
  hack1 = rts_allocate_mutex();
  if( hack1 == NULL )
    {
    }
  }

assert( hack1 );
BGL_Mutex_Release( hack1 );

BGL_Barrier* hack2;
for( int i=0; i<4; i++ )
//// this worked in debugging - probably not required for( int i=0; i<6; i++ )
  {
  hack2 = rts_allocate_barrier();
  if( hack2 == NULL )
    {
    PLATFORM_ABORT("Spin past blrts requered barriers failed");
    }
  }

#endif

#if ( defined( PK_BGL ) && defined( BGL_PERFCTR_OFF ) )
  bgl_perfctr_shutdown();
#endif

#if defined( PK_BLADE_SPI )
  int rc_g;
  if( (rc_g = _blade_on_blrts_init()) )
    {
    printf("main: blade_on_blrts_init: rc = %d.\n", rc_g );
    exit( rc_g );
    }

  //  co_co_init();
#endif

#ifdef PKTRACE_ON
  InitTraceMutexes();
#endif

#ifndef PK_PARALLEL
  Platform::Topology::ThisAddressSpaceId = 0;
  Platform::Topology::Size               = 1;
#endif

//  if( argc < 2 )
//    {
//    fprintf(stderr, "\n****** bad args.  use: %s <dvs_fn>  (Date %s %s )*********\n", argv[0], __DATE__, __TIME__ );
//    fprintf(stderr, "\n******            use: %s <dvs_fn> <rdg_fn>  *********\n", argv[0] );
//    fprintf(stderr, "\n******            use: %s <dvs_fn> <monitor_ipaddr> <monitor_port> *********\n", argv[0] );
//    fprintf(stderr, "\n******            use: %s <dvs_fn> -p [ port ] -h [ host ip ] *********\n", argv[0] );
//    fflush(stderr);
//    PLATFORM_ABORT("BAD ARG COUNT");
//    }
//
//  int argcount = 2;
  int argcount = 1 ;
  int AttachDebuggerFlag = 0;

  while( argcount < argc )
    {
    // ADD COMMAND ARGUMENTS HANDLING HERE AND DON'T FORGET TO UPDATE argcount!!!

    if( ! strcmp( argv[argcount], "-procmesh" ) )
      {
      argcount++;

      if( argc - argcount < 3 )
        AbortArgv( "The -procmesh flag requires three numerical arguments" );

      int pcx = atoi( argv[argcount++] );
      int pcy = atoi( argv[argcount++] );
      int pcz = atoi( argv[argcount++] );

      if( pcx < 0 || pcy < 0 || pcz < 0 )
        AbortArgv( "bad arguments for -procmesh" );

      Platform::Topology::xSize_cmdprompt = pcx;
      Platform::Topology::ySize_cmdprompt = pcy;
      Platform::Topology::zSize_cmdprompt = pcz;
      }
    else if( ! strcmp( argv[argcount], "-rdgname" ) )
      {
      argcount++;

      if( argc - argcount < 1 )
        AbortArgv( "The -rdgname must be followed by a filename" );

      rdgnameinargv = argv[argcount++];
      }
    else if( ! strcmp( argv[argcount], "-p" ) )
      {
      argcount++;

      if( argcount < argc )
          Platform::Comm::mRemotePort = atoi( argv[ argcount ] );
      else
          PLATFORM_ABORT("ERROR:: -p needs a [ port ] argument");

      argcount++;
      }
    else if( ! strcmp( argv[argcount], "-debug_p1_5_port" ) )
      {
      argcount++;

      printf("argcount: %d argc: %d\n", argcount, argc );

      if( argcount < argc )
          Platform::Comm::mDebugP1_5ListeningPort = atoi( argv[ argcount ] );
      else
          PLATFORM_ABORT("ERROR:: -debug_p1_5_port needs a [ port ] argument");

      argcount++;
      }
    else if( ! strcmp( argv[argcount], "-h" ) )
      {
      argcount++;

      if( argcount < argc )
          Platform::Comm::mRemoteHostIP = htoi( argv[ argcount ] );
      else
          PLATFORM_ABORT("ERROR:: -h needs a [ host ip ] argument");
      argcount++;
      }
    else if( ! strcmp( argv[argcount], "-ad" ) )
      {
      ////BGF Cannot call this here because MPI isnpt up yet ..   AttachDebugger( argv[0] );
      AttachDebuggerFlag = 1;
      printf("AttachDebuggerFlag: %d\n", AttachDebuggerFlag );

      argcount++;
      }
    else if( ! strcmp( argv[argcount], "-sub_part_count" ) )
      {
      argcount++;

      Platform::Topology::mSubPartitionCount = atoi( argv[ argcount ] );

      argcount++;
      }
    else if ( ! strcmp( argv[argcount], "-loadtune" ) )
      {
      argcount++;

      if( argc - argcount < 2 )
        AbortArgv( "The -loadtune flag requires two numerical arguments" );

      int LoadTuneIndex = atoi( argv[argcount++] );
      double LoadTuneValue = atoi( argv[argcount++] ) * 0.001 ;
      Platform::LoadBalance::SetTune(LoadTuneIndex,LoadTuneValue) ;

      }
    else if ( ! strcmp(argv[argcount], "--" ) )
      {
        break ;
      }
    else
      {
//      fprintf( stderr, "Use: %s <dvs_fn> %s %s\n", argv[0], __DATE__, __LINE__ );
      fprintf ( stderr, "Use: %s pk_args -- app_args\n", argv[0] ) ;
      fprintf ( stderr, "Unknown pk arg (%s)\n", argv[argcount]) ;
      AbortArgv( "Unknown flag on the command line" );
      }
    }

#ifdef USE_BGL_TREE
  int rc = BGL_TreeRouteComputeNodes(&FullPartitionRoot, 1, &FullPartitionClass );
  rc = BGL_TreeRouteComputeNodes(&FullPartitionRoot, 1, &IntegerReductionClass );
  rc = BGL_TreeRouteComputeNodes(&FullPartitionRoot, 0, &ProfilingReductionClass );

  Platform::Collective::Init( FullPartitionClass, IntegerReductionClass );
#endif

  Platform::Topology::Init( &argc, &argv );
  Platform::Coprocessor::Init();
  Platform::Control::Init();
  Platform::Collective::Init();

  Platform::FxLogger::OpenLogPort();

#if defined(PKTRACE_BGL_PERFCTR)
    __pktrace_perfctr::__pktrace_perfctr_init() ;
#endif    

#if PKTRACE_TUNE
    pkNew( &pkTraceServer::mTimeStampsBuffer[ 0 ],
           PKTRACE_NUMBER_OF_TIMESTAMPS, __FILE__, __LINE__ );

    pkNew( &pkTraceServer::mTimeStampsBuffer[ 1 ],
           PKTRACE_NUMBER_OF_TIMESTAMPS, __FILE__, __LINE__ );

    pkNew( &pkTraceServer::mTimeStampNames[ 0 ],
           PKTRACE_NUMBER_OF_TRACE_NAMES, __FILE__, __LINE__ );

    pkNew( &pkTraceServer::mTimeStampNames[ 1 ],
           PKTRACE_NUMBER_OF_TRACE_NAMES, __FILE__, __LINE__ );
#endif

    Platform::Memory::ExportFence();

#ifdef PK_PARALLEL

#ifdef DEMO
  // Create a TCP socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero( &Platform::Comm::mRemoteAddr, sizeof( Platform::Comm::mRemoteAddr ) );
  Platform::Comm::mRemoteAddr.sin_family       =  AF_INET;
  Platform::Comm::mRemoteAddr.sin_port         =  Platform::Comm::mRemotePort;
  Platform::Comm::mRemoteAddr.sin_addr.s_addr  =  Platform::Comm::mRemoteHostIP;
#endif

#ifdef PK_AIX
  nice(1);

  // DO a check sum of the executable file to make sure that
  // all address spaces have been loaded with the same executable.

  FILE *fp = fopen( argv[0], "r" );
  assert( fp != NULL );
  // Add up the first bit of the executable file.
  for(int b = 0; (b < PK_MAX_EXCUTABLE_CHECKSUM) && (! feof( fp )); b++ )
    _pk_ExecutableCheckSum += fgetc(fp);
  fclose( fp );
#endif

#ifdef PK_MPI_ALL
  // MPI_Init( &argc, &argv );  /* initialize MPI environment */

  // Now that MPI is up, bring up debugger if required before going on.
  if(  AttachDebuggerFlag == 1 )
    {
    AttachDebugger( argv[0] );
    }


  // Todos for mapping the application on the machine
  // 1. Check the processor coordinate -> MPI rank map
  //  Mapping: sextonjc (5:01 PM) - mpi rank =   xCoord + yCoord*xSize + zCoord*xSize*ySize

  // 2. Get dimensions of the *block* and permute to satisfy pX >= pY >= pZ constraint
  //    Note: for MPI partitions that don't fill the physical partition, the extent
  //          of each dim must be figured out from the above mapping scheme.

//   int pX, pY, pZ;
//   Platform::Topology::GetDimensions( &pX, &pY, &pZ );
  // Platform::Topology::Init( &pX, &pY, &pZ );

//   int dimSz[ 3 ]    = { pX, pY, pZ };
//   int periods[ 3 ]  = {0, 0, 0};

//   printf(" pX=%d pY=%d pZ=%d\n", pX, pY, pZ );

//   MPI_Cart_create( MPI_COMM_WORLD, 3, dimSz, periods, 0, &Platform::Topology::cart_comm );
//   MPI_Comm_rank( Platform::Topology::cart_comm, &Platform::Topology::ThisAddressSpaceId );
//   MPI_Comm_size( Platform::Topology::cart_comm, &Platform::Topology::Size );
#endif

#if defined(PK_SPI)
  ReactorInitialize() ; // Patches in addresses of memoty mapping, and clears hardware
#endif
  // Try to bring up the trace system under a uniform mpi timing condition
  TraceClient SyncStart;
  TraceClient SyncFinis;

  // Prime a barrier loop, grab time, then hit trace point.  Less skew that this will
  // take actual thought.

  Platform::Control::Barrier();         //Wait for all processes to get here.
  Platform::Clock::mTimeValueBase = Platform::Clock::GetTimeValue();
  Platform::Control::Barrier();         //Wait for all processes to get here.
  Platform::Clock::mTimeValueBase = Platform::Clock::GetTimeValue();
  Platform::Control::Barrier();         //Wait for all processes to get here.
  Platform::Clock::mTimeValueBase = Platform::Clock::GetTimeValue();
  Platform::Clock::mTimeBase      = Platform::Clock::ConvertTimeValueToDouble( Platform::Clock::mTimeValueBase );

  Platform::Control::Barrier();         //Wait for all processes to get here.
  Platform::Control::Barrier();         //Wait for all processes to get here.
  Platform::Control::Barrier();         //Wait for all processes to get here.

  // Time trace point overhead
  for( int i = 0; i < 10; i++ )
    {
    SyncStart.HitOE(     1,
                        "SyncStart",
                         Platform::Topology::GetAddressSpaceId(),
                         SyncStart );

    SyncFinis.HitOE(     1,
                        "SyncFinis",
                         Platform::Topology::GetAddressSpaceId(),
                         SyncFinis );
    }

  for( int i = 0; i < 10; i++ )
    {
    SyncStart.HitOE(     1,
                        "SyncStart",
                         Platform::Topology::GetAddressSpaceId(),
                         SyncStart );
    for( int j = 0; j < (100 * i); j++ )
      Platform::Control::Barrier();         //Wait for all processes to get here.
    SyncFinis.HitOE(     1,
                        "SyncFinis",
                         Platform::Topology::GetAddressSpaceId(),
                         SyncFinis );
    }

    // Start the IOP thread
#if defined( PK_BLADE )
  Platform::Mutex::Unlock( & IOP_CP_Barrier );
  // s0printf("ComputeNode_CP unlocked the IOP_CP_Barrier\n ");
  BGLPartitionBarrier();
#endif
#endif

  BegLogLine(PKFXLOG_MAIN)
    << "About to call PkMain"
    << EndLogLine ;
    
  PkMain( argc-argcount, argv+argcount, envp );

  BegLogLine(PKFXLOG_MAIN)
    << "Back from PkMain"
    << EndLogLine ;
    
#if PKTRACE_TUNE
  pkTraceServer::FlushAllBothBuffers();
    
    
//  for( int i=0; i <  Platform::Topology::GetSize() ; i++ )
//    {
//    if( i == Platform::Topology::GetAddressSpaceId() )
//      {
//      pkTraceServer::FlushAllBothBuffers();
//      }
//    }
  Platform::Control::Barrier();
#endif

  Platform::Control::Finalize();
  // PLATFORM_ABORT( "main()::PkMain returned");
  return(0);
}

int Platform::JobInfo::JobId()
   {
#if defined( PK_BGL )
   return(rts_get_jobid()); // from rts.h
#else
   return( -1 ); // no-op otherwise
#endif
   }

void
Platform::Collective::Alltoallv( void* aSendBuff, int * aSendCount, int* aSendOffsets, unsigned aSendTypeSize,
           void* aRecvBuff, int * aRecvCount, int* aRecvOffsets, unsigned aRecvTypeSize,
           int debug, int RecvCountOverposted, int aBuffers32ByteAligned,
           int aUseCacheListFromLastCall, int aCommType )
    {
    	BegLogLine(PKFXLOG_COLLECTIVE)
    	 << "Alltoallv aSendBuff=" << aSendBuff
    	 << " aSendTypeSize=" << aSendTypeSize
    	 << " RecvCountOverposted=" << RecvCountOverposted
    	 << " aBuffers32ByteAligned=" << aBuffers32ByteAligned
    	 << " aUseCacheListFromLastCall=" << aUseCacheListFromLastCall
    	 << " aCommType=" << aCommType
    	 << EndLogLine ;
    	if( PKFXLOG_COLLECTIVE )
    	  {
    		int AddressSpaceCount=Platform::Topology::GetAddressSpaceCount() ;
    	    for( int i=0; i<AddressSpaceCount; i++ )
    	       {
    		      BegLogLine( PKFXLOG_COLLECTIVE )
    		        << "Alltoallv aSendCount[" << i
    		        << "] is " << aSendCount[ i ]
    		        << EndLogLine ;
    	       }
    	  }
    	
    	
#if defined( PK_MPI_ALL )
    MPI_Datatype SendType;
    MPI_Datatype RecvType;

    switch( aSendTypeSize )
      {
      case 1:
        {
        SendType = MPI_CHAR;
        break;
        }
      case 2:
        {
        SendType = MPI_SHORT;
        break;
        }
      case 4:
        {
        SendType = MPI_INT;
        break;
        }
      case 8:
        {
        SendType = MPI_DOUBLE;
        break;
        }
      default:
        {
        assert( 0 );
        break;
        }
      }

    switch( aRecvTypeSize )
      {
      case 1:
        {
        RecvType = MPI_CHAR;
        break;
        }
      case 2:
       {
        RecvType = MPI_SHORT;
        break;
        }
      case 4:
        {
        RecvType = MPI_INT;
        break;
        }
      case 8:
        {
        RecvType = MPI_DOUBLE;
        break;
        }
      default:
        {
        assert( 0 );
        break;
        }
      }

    if( RecvCountOverposted )
      {
//                 int* SendCount = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( SendCount == NULL )
//                   PLATFORM_ABORT( "SendCount == NULL" );

//                 int* SendOffset = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( SendOffset == NULL )
//                   PLATFORM_ABORT( "SendOffset == NULL" );

//                 int* RecvCount = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( RecvCount == NULL )
//                   PLATFORM_ABORT( "RecvCount == NULL" );

//                 int* RecvOffset = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( RecvOffset == NULL )
//                   PLATFORM_ABORT( "RecvOffset == NULL" );

      for( int i=0; i < Platform::Topology::GetAddressSpaceCount(); i++ )
        {
        A2ASendCount[ i ] = ( aSendCount[ i ] != 0 );
        A2ASendOffset[ i ] = i;

        if( aSendCount[ i ] == -1 )
          aSendCount[ i ] = 0;

        A2ARecvCount[ i ] = ( aRecvCount[ i ] > 0 );
        A2ARecvOffset[ i ] = i;

        }

//                  if( Platform::Topology::GetAddressSpaceId() == 0 )
//                    {
//                    for( int i=0; i < Platform::Topology::GetAddressSpaceCount(); i++ )
//                      {
//                      printf("%d SendCount: %d RecvCount: %d\n", i, SendCount[ i ], RecvCount[ i ] );
//                      }
//                    }

//               if( Topology::GetAddressSpaceId() == 0 )
//                   printf("PLATFORM: Got Here: %d\n", __LINE__ );

      MPI_Alltoallv( aSendCount, A2ASendCount, A2ASendOffset, MPI_INT,
                     aRecvCount, A2ARecvCount, A2ARecvOffset, MPI_INT, Topology::cart_comm );

//                 free( SendCount );
//                 free( SendOffset );
//                 free( RecvCount );
//                 free( RecvOffset );
      }

//               if( Topology::GetAddressSpaceId() == 0 )
//                 printf("PLATFORM: Got Here: %d\n", __LINE__ );

    MPI_Alltoallv( aSendBuff, aSendCount, aSendOffsets, SendType,
                   aRecvBuff, aRecvCount, aRecvOffsets, RecvType, Topology::cart_comm );

//               if( Topology::GetAddressSpaceId() == 0 )
//                 printf("PLATFORM: Got Here: %d\n", __LINE__ );

//              printf("PLATFORM: Got Here: %d\n", __LINE__ );

#elif defined( PK_BLADE_SPI )

    if( RecvCountOverposted )
      {
//                 int* SendCount = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( SendCount == NULL )
//                   PLATFORM_ABORT( "SendCount == NULL" );

//                 int* SendOffset = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( SendOffset == NULL )
//                   PLATFORM_ABORT( "SendOffset == NULL" );

//                 int* RecvCount = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( RecvCount == NULL )
//                   PLATFORM_ABORT( "RecvCount == NULL" );

//                 int* RecvOffset = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
//                 if( RecvOffset == NULL )
//                   PLATFORM_ABORT( "RecvOffset == NULL" );
      SendNodeSetCount = 0;
      RecvNodeSetCount = 0;
      for( int i=0; i < Platform::Topology::GetAddressSpaceCount(); i++ )
        {
        A2ASendCount[ i ] = ( aSendCount[ i ] != 0 );
        if( A2ASendCount[ i ] )
          {
              SendNodeSet[ SendNodeSetCount ] = i;
            	BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv SendNodeSet[" << SendNodeSetCount
                	 << "]=" << i
                	 << EndLogLine ;
            	SendNodeSetCount++;
          }

        A2ASendOffset[ i ] = i;

        // -1 is the special signal that we want that node out of the comm
        if( aSendCount[ i ] == -1 )
          aSendCount[ i ] = 0;

        A2ARecvCount[ i ] = ( aRecvCount[ i ] > 0 );

        if( A2ARecvCount[ i ] )
          {
              RecvNodeSet[ RecvNodeSetCount ] = i;
            	BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv RecvNodeSet[" << RecvNodeSetCount
                	 << "]=" << i
                	 << EndLogLine ;
              RecvNodeSetCount++;
          }

        A2ARecvOffset[ i ] = i;
        }

      GeneralAlltoallv.ExecuteSingleCoreSimple( aSendCount, A2ASendCount, A2ASendOffset, sizeof(int),
                                                aRecvCount, A2ARecvCount, A2ARecvOffset, sizeof(int),
                                                Platform::Topology::GetGroup(), 0, debug,
                                                SendNodeSet, SendNodeSetCount,
                                                RecvNodeSet, RecvNodeSetCount );

//                 free( SendCount );
//                 free( SendOffset );
//                 free( RecvCount );
//                 free( RecvOffset );
      }

    // Broadcast == 1, Reduce == 2
    if( aCommType == 1 )
      {
      if( !aUseCacheListFromLastCall )
        {
        BcastSendNodeSetCount = 0;
        BcastRecvNodeSetCount = 0;
        int MachineSize999999 = Platform::Topology::GetAddressSpaceCount();
        for( int i=0; i < MachineSize999999; i++ )
          {
          if ( aSendCount[ i ] != 0 )
            {
              BcastSendNodeSet[ BcastSendNodeSetCount ] = i;
            	BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv BcastSendNodeSet[" << BcastSendNodeSetCount
                	 << "]=" << i
                	 << " aSendCount=" << aSendCount[ i ]
                	 << EndLogLine ;
              BcastSendNodeSetCount++;
            }

          if( aRecvCount[ i ] != 0 )
            {
              BcastRecvNodeSet[ BcastRecvNodeSetCount ] = i;
            	BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv BcastRecvNodeSet[" << BcastRecvNodeSetCount
                	 << "]=" << i
                	 << " aRecvCount=" << aRecvCount[ i ]
                	 << EndLogLine ;
              BcastRecvNodeSetCount++;
            }
          }
        }
      else if( PKFXLOG_COLLECTIVE )
        {
      	for( int i=0;i<ReduceSendNodeSetCount;i+=1)
      	   {
            	  BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv aUseCacheListFromLastCall BcastSendNodeSet[" << i
                	 << "]=" << BcastSendNodeSet[ i ]
                	 << EndLogLine ;
      	   }
      	for( int j=0;j<BcastRecvNodeSetCount;j+=1)
      	   {
            	  BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv aUseCacheListFromLastCall BcastRecvNodeSet[" << j
                	 << "]=" << BcastRecvNodeSet[ j ]
                	 << EndLogLine ;
      	   }
        }

      GeneralAlltoallv.ExecuteSingleCoreSimple( aSendBuff, aSendCount, aSendOffsets, aSendTypeSize,
                                                aRecvBuff, aRecvCount, aRecvOffsets, aRecvTypeSize,
                                                Platform::Topology::GetGroup(), aBuffers32ByteAligned, debug,
                                                BcastSendNodeSet, BcastSendNodeSetCount,
                                                BcastRecvNodeSet, BcastRecvNodeSetCount );
      }
    else if( aCommType == 2 )
      {
      if( !aUseCacheListFromLastCall )
        {
        ReduceSendNodeSetCount = 0;
        ReduceRecvNodeSetCount = 0;
        int MachineSize999999 = Platform::Topology::GetAddressSpaceCount();
        for( int i=0; i < MachineSize999999; i++ )
          {
          if( aSendCount[ i ] != 0 )
            {
              ReduceSendNodeSet[ ReduceSendNodeSetCount ] = i;
              BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv ReduceSendNodeSet[" << ReduceSendNodeSetCount
                	 << "]=" << i
                	 << " aSendCount=" << aSendCount[ i ]
                	 << EndLogLine ;
              ReduceSendNodeSetCount++;
            }

          if( aRecvCount[ i ] != 0 )
            {
              ReduceRecvNodeSet[ ReduceRecvNodeSetCount ] = i;
              BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv ReduceRecvNodeSet[" << ReduceRecvNodeSetCount
                	 << "]=" << i
                	 << " aRecvCount=" << aRecvCount[ i ]
                	 << EndLogLine ;
              ReduceRecvNodeSetCount++;
            }
          }
        }
      else if( PKFXLOG_COLLECTIVE )
        {
      	for( int i=0;i<ReduceSendNodeSetCount;i+=1)
      	   {
            	  BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv aUseCacheListFromLastCall ReduceSendNodeSet[" << i
                	 << "]=" << ReduceSendNodeSet[ ReduceSendNodeSetCount ]
                	 << EndLogLine ;
      	   }
      	for( int j=0;j<ReduceRecvNodeSetCount;j+=1)
      	   {
            	  BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv aUseCacheListFromLastCall ReduceRecvNodeSet[" << j
                	 << "]=" << ReduceRecvNodeSet[ j ]
                	 << EndLogLine ;
      	   }
        }

      GeneralAlltoallv.ExecuteSingleCoreSimple( aSendBuff, aSendCount, aSendOffsets, aSendTypeSize,
                                                aRecvBuff, aRecvCount, aRecvOffsets, aRecvTypeSize,
                                                Platform::Topology::GetGroup(), aBuffers32ByteAligned, debug,
                                                ReduceSendNodeSet, ReduceSendNodeSetCount,
                                                ReduceRecvNodeSet, ReduceRecvNodeSetCount );
      }
    else
      {
      SendNodeSetCount = 0;
      RecvNodeSetCount = 0;
      int MachineSize999999 = Platform::Topology::GetAddressSpaceCount();
      for( int i=0; i < MachineSize999999; i++ )
        {
        if( aSendCount[ i ] != 0 )
          {
            SendNodeSet[ SendNodeSetCount ] = i;
            BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv SendNodeSet[" << SendNodeSetCount
                	 << "]=" << i
                	 << " aSendCount=" << aSendCount[ i ]
                	 << EndLogLine ;
            SendNodeSetCount++;
          }

        if( aRecvCount[ i ] != 0 )
          {
            RecvNodeSet[ RecvNodeSetCount ] = i;
            BegLogLine(PKFXLOG_COLLECTIVE)
                	 << "Alltoallv RecvNodeSet[" << RecvNodeSetCount
                	 << "]=" << i
                	 << " aRecvCount=" << aRecvCount[ i ]
                	 << EndLogLine ;
            RecvNodeSetCount++;
          }
        }

      GeneralAlltoallv.ExecuteSingleCoreSimple( aSendBuff, aSendCount, aSendOffsets, aSendTypeSize,
                                                aRecvBuff, aRecvCount, aRecvOffsets, aRecvTypeSize,
                                                Platform::Topology::GetGroup(), aBuffers32ByteAligned, debug,
                                                SendNodeSet, SendNodeSetCount,
                                                RecvNodeSet, RecvNodeSetCount );
      }

#else
    assert( 0 );
#endif

	if( PKFXLOG_COLLECTIVE )
	  {
		int AddressSpaceCount=Platform::Topology::GetAddressSpaceCount() ;
	    for( int i=0; i<AddressSpaceCount; i++ )
	       {
		      BegLogLine( PKFXLOG_COLLECTIVE )
		        << "Alltoallv aSendCount[" << i
		        << "] is " << aSendCount[ i ]
		        << EndLogLine ;
	       }
	  }


    }


#if defined(PK_BLADE) && defined(PK_BGL)
extern "C" {

TraceClient NoPacketsFromVirtualPollStart;
TraceClient NoPacketsFromTorusPollStart;

TraceClient NoPacketsFromVirtualPollFinis;
TraceClient NoPacketsFromTorusPollFinis;

int
ComputeNodeWorld_IOP()
  {
  int NodesX;
  int NodesY;
  int NodesZ;
  int MyX;
  int MyY;
  int MyZ;

#if defined(REALSPACE_OVERLAPPED_WITH_KSPACE)
  Platform::Reactor::PollAndExecute( NULL );
#endif

#if defined(USE_2_CORES_FOR_FFT)

  BGLPartitionGetCoords( &MyX, &MyY, &MyZ );
  BGLPartitionGetDimensions( &NodesX, &NodesY, &NodesZ );

#ifdef BGL_VIRTUAL_FIFO
  int c=1;
  int count=0;
  int vfBusyFlag=1;
  int torusBusyFlag=1;
  int prevVfBusyFlag=1;
  int prevTorusBusyFlag=1;

  if( VirtualFifo_PacketBuffer == NULL )
    PLATFORM_ABORT("VirtualFifo_PacketBuffer == NULL") ;

  int BGL_POLL_COUNT = 14;

  // while(1);

  volatile int dummy2 = 0;

  while( 1 )
    {

    c=BGL_POLL_COUNT;
    BGLTorusPoll( &c );

    if( c != BGL_POLL_COUNT )
      continue;

    int vfCount = BGLVirtualFifoPoll( 14 );

    if( ( vfCount == 0 ) && ( c == BGL_POLL_COUNT ) )
      {
      vfBusyFlag=0;

#if 0
      double dummy1 = 1.0;
      for( int k=1; k < 1000000; k++ )
        {
          dummy1 = dummy1 + 1.0 / k + 2.0 / k + 3.0 / k;
        }

      dummy2 = dummy1;

      if( dummy1 == 0 )
        s0printf("dummy1\n", dummy1);
#endif
      }
    else
      vfBusyFlag=1;

    if( vfBusyFlag != prevVfBusyFlag && vfBusyFlag == 1 )
      NoPacketsFromVirtualPollStart.HitOE( PKTRACE_IOP_POLL,
                                           "NoPacketsFromVirtualPollStart",
                                           Platform::Thread::GetId(),
                                           NoPacketsFromVirtualPollStart);

    if( vfBusyFlag != prevVfBusyFlag && vfBusyFlag == 0 )
      NoPacketsFromVirtualPollFinis.HitOE( PKTRACE_IOP_POLL,
                                           "NoPacketsFromVirtualPollFinis",
                                           Platform::Thread::GetId(),
                                           NoPacketsFromVirtualPollFinis);

    if( c == BGL_POLL_COUNT )
      torusBusyFlag=0;
    else
      torusBusyFlag=1;

    if( torusBusyFlag != prevTorusBusyFlag && torusBusyFlag == 1)
      NoPacketsFromTorusPollStart.HitOE( PKTRACE_IOP_POLL,
                                         "NoPacketsFromTorusPollStart",
                                         Platform::Thread::GetId(),
                                         NoPacketsFromTorusPollStart);

    if( torusBusyFlag != prevTorusBusyFlag && torusBusyFlag == 0)
      NoPacketsFromTorusPollFinis.HitOE( PKTRACE_IOP_POLL,
                                         "NoPacketsFromTorusPollFinis",
                                         Platform::Thread::GetId(),
                                         NoPacketsFromTorusPollFinis );

    prevVfBusyFlag = vfBusyFlag;
    prevTorusBusyFlag = torusBusyFlag;
    }
#else
   BegLogLine( 1 )
     << "_IOP_main "
     << " About to call BGLTorusWait()"
     << EndLogLine;
   s0printf("_IOP_main running... About to call BGLTorusWait()\n");

   int rc = BGLTorusWait();  // wait on both fifo groups
#endif

  BegLogLine( 0 )
       << "_IOP_main[" << MyX << " , " << MyY << " , " << MyZ << "]: All Packets Received, exiting."
       << EndLogLine;

#endif

  BegLogLine( 1 )
    << "_IOP_main() "
    << "Entering spin forever loop"
    << EndLogLine;

  while(1)
    ;

  return ( 0 );
  }


int
EthernetPacketDispatcher( Bit32 ulSrcAddr,  // IP Address of sender
                          Bit16 usSrcPort,  // IP Port used by sender
                          Bit32 ulDestAddr, // IP Address of Receiver (this node)
                          Bit16 usDestPort, // IP Port on which Packet Arrived
                          void  *ppData,    // Address of UDP Pkt data
                          int   nLen        // Length in bytes of UDP Pkt data
                          )
  {
  int error=0;

  BegLogLine( 1 || PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
    << "EthernetPacketDispatcher:: "
    << " UdpPacketAckedFlag=" << UdpPacketAckedFlag
    << " From ip/port sender=" << (void *) ulSrcAddr << " / " << (void *) usSrcPort
    << " nLen=" << nLen
    << EndLogLine;

  if( ( ulSrcAddr != serv_ip_addr.mVar ) && ( usSrcPort != serv_ip_port.mVar ) )
    return ( 0 );

  BegLogLine( 0 && PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
    << "EthernetPacketDispatcher:: "
    << " UdpPacketAckedFlag=" << UdpPacketAckedFlag
    << " From ip/port sender=" << (void *) ulSrcAddr << " / " << (void *) usSrcPort
    << " nLen=" << nLen
    << EndLogLine;

  IncomingUDPPacket* pkt = (IncomingUDPPacket *) ppData;

  if( UdpPacketSeqNum == pkt->mAck )
    UdpPacketAckedFlag = 1;

  return ( 1 );
  }


int
DoUdpTransport()
  {
  UdpPacketAckedFlag=0;
  udpPacket.mSeqNum = UdpPacketSeqNum;

  int SendCount = 0;

  unsigned long long PacketRoundTripStartTime = a_GetTimeBase();

  int Done = 0;
  while( ! Done )
    {
      //  0x902d916 0x8cea
      //int rc = BGLEnet_sendto( 0x902d916, 0x8cea, 0x8cea, cp->mData, cp->mLen );
      int rc = BGLEnet_sendto( serv_ip_addr.mVar, serv_ip_port.mVar, serv_ip_port.mVar, &udpPacket, sizeof( UdpPacket ) );

      BegLogLine( PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
        << "SendPacketThroughEthernetActor "
        << " rc from BGLEnet_sendto()=" << rc
        << " sizeof(udpPacket) "
        << sizeof(udpPacket)
        << EndLogLine;

    SendCount++;

    if( rc == EAGAIN )
      {
      // either sleep or loop
      // Platform::Thread::Sleep( 500 * 1000 * 1000 ); // processor cycles
      for(  unsigned long long StartT =  a_GetTimeBase() ;
           (a_GetTimeBase() - StartT) < ( 500 * 1000 * 1000 ) ;
           )
        {
        BGLEnet_Poll();
        }
      continue;
      }
    else if ( rc == EBUSY )
      {
      // either sleep or loop
      Platform::Thread::Sleep( 500 * 1000 ); // processor cycles
      continue;
      }

    BegLogLine( PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
      << "SendPacketThroughEthernetActor"
      << " Done with BGLEnet_sendto() "
      << " return code=" << rc
      << " SendCount " << SendCount
      << " Ip Addr/Port " << (void *) serv_ip_addr.mVar << " / " << ( void *)  serv_ip_port.mVar
      << EndLogLine;

    // Successfully sent
    if( rc == 0 )
      {
      BegLogLine( PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
        << "SendPacketThroughEthernetActor"
        << " Done with BGLEnet_sendto() "
        << " return code=" << rc
        << " SendCount " << SendCount
        << " Ip Addr/Port " << (void *) serv_ip_addr.mVar << " / " << ( void *)  serv_ip_port.mVar
        << EndLogLine;

      // Poll the receive queue
      unsigned long long StartingTime = a_GetTimeBase();
      for( ;; )
        {
        BGLEnet_Poll();

        BegLogLine( PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
          << "SendPacketThroughEthernetActor"
          << " Poll loop... ackValue=" << UdpPacketAckedFlag
          << EndLogLine;

        if( UdpPacketAckedFlag )
          {
          Done = 1;
          break;
          }
        else
          {
          unsigned long long CurrentTime = a_GetTimeBase();
          if( CurrentTime - StartingTime > 250 * 1000 * 1000 )
            break; // retry
          }
        }
      }
    }

  unsigned long long PacketRoundTripEndTime = a_GetTimeBase();
  int ElapsedTime = PacketRoundTripEndTime - PacketRoundTripStartTime ;

  BegLogLine( PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
    << "SendPacketThroughEthernetActor"
    << " Finished with reliable transfer "
    << " RoundTrip (pclocks) " << ElapsedTime
    << " Trys " << SendCount
    << EndLogLine;

  UdpPacketSeqNum++;
  udpPacket.mLen = 0;
  return (1);
  }

int
SendPacketThroughEthernetActor( void* args )
  {
  UdpTreePacket* cp = (UdpTreePacket* ) args;

  BegLogLine( PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR )
    << "SendPacketThroughEthernetActor "
    << " IsIONode=" << BGLPartitionIsIONode()
    << " cp->mLen=" << cp->mLen
    << " cp->mData=" << (void *) cp->mData
    << EndLogLine;


  if( cp->mLen + udpPacket.mLen > UDP_PACKET_PAYLOAD_SIZE )
      DoUdpTransport();

  memcpy( &( udpPacket.mData[ udpPacket.mLen ] ), cp->mData, cp->mLen);
  udpPacket.mLen += cp->mLen;

  return (1);
  }

int
IONodeWorld_CP()
  {
  //  BGLDoNothingOnIONode();
  // Loop forever reading from the Virtual Fifo

  BegLogLine( PKFXLOG_IONODEWORLD_IOP )
    << "IONodeWorld_CP:: "
    << " running "
    << EndLogLine;

  BGLEnet_Install_UDP_ISR( &EthernetPacketDispatcher,
                           -1 // Poll mode
                           );

  Platform::Mutex::Unlock( &IOP_CP_Barrier );
  // s0printf("IONode_CP unlocked the IOP_CP_Barrier\n ");
  // BGLPartitionBarrier();

  while( 1 )
    {
    int vfCount = BGLVirtualFifoPoll( 1 );
    if( vfCount == 0 && udpPacket.mLen > 0)
      DoUdpTransport();
    }
  }

char localBuff[ BGL_TREE_PACKET_PAYLOAD ] MEMORY_ALIGN(5);

int
IONodeWorld_IOP()
  {
  UdpTreePacket *UTP = (UdpTreePacket *) localBuff;

  // Intra node barrier
  //while( !Platform::Mutex::TryLock( & IOP_CP_Barrier ) )
  //  ;

  BegLogLine( PKFXLOG_IONODEWORLD_IOP )
    << "IONodeWorld_IOP:: "
    << " running "
    << EndLogLine;

  _BGL_TreeHwHdr p2pHdr ;
  int MyX, MyY, MyZ;
  BGLPartitionGetCoords(&MyX, &MyY, &MyZ);

  _BGL_TorusPktHdr   BGLTorus_Hdr;
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   MyX,
                   MyY,
                   MyZ,
                   0,
                   (Bit32) SendPacketThroughEthernetActor,
                   0,
                   0,
                   _BGL_TORUS_ACTOR_BUFFERED );

  while( 1 )
    {
    // Receive all tree packets from compute nodes.
    int ReturnedResult = BGLTreeReceive( _BGL_TREE_BALANCED_MODE_OS_VC, // class
                                         &p2pHdr,                        // Header after circulating the tree
                                         localBuff,                     // Operands being received
                                         BGL_TREE_PACKET_PAYLOAD // one block
                                         );

    BegLogLine( PKFXLOG_IONODEWORLD_IOP )
      << "IONodeWorld_IOP:: "
      << " Received a packet... "
      << " ReturnedResult=" << ReturnedResult
      << EndLogLine;

    // Stuff the packet into the Virtual Fifo
    VirtualFifo_BGLTorusSendPacketUnaligned( &BGLTorus_Hdr,
                                             localBuff,
                                             UTP->mLen );   //// BGF BGL_TREE_PACKET_PAYLOAD );

    BegLogLine( PKFXLOG_IONODEWORLD_IOP )
      << "IONodeWorld_IOP:: "
      << " Sent a packet to the VirtualFifo"
      << EndLogLine;
    }
  }


int ControlMain( int, char**, char** );

int _CP_main( int argc, char *argv[], char **envp )
  {
  return( ControlMain( argc, argv, envp ) );
  }

};

#else

// This is the lil' ol main used by anything resembling posix...

static int ControlMain( int, char**, char** );
static int AuxMain( int, char**, char** );

int main( int argc, char *argv[], char **envp )
  {

  Platform_ArgC = argc;
  Platform_ArgV = argv;
  Platform_EnvP = envp;

#if defined(PK_BGL)
#if !defined( PK_BGL_USE_POSIX_MALLOC )
  HM.Init() ;
#endif  
#endif
//printf("Here line: %d\n", __LINE__);
#if PK_SPI
   BegLogLine(PKFXLOG_TOPOLOGY)
   << "main processor-id=" << rts_get_processor_id()
   << EndLogLine ;

  return( ( 0 == rts_get_processor_id() )
        ? ControlMain( argc, argv, envp )
        : AuxMain(argc, argv, envp) );
#else

  return ControlMain( argc, argv, envp );
#endif
  }

#if PK_SPI
static int AuxMain(int argc, char ** argv, char ** envp)
{
        BegLogLine(PKFXLOG_MAIN)
        << "AuxMain entered ..."
        << EndLogLine ;
        ReactorInitialize() ;
        for(;;)
        {
          Platform::Reactor::Poll() ;
        }
        BegLogLine(PKFXLOG_MAIN)
        << "AuxMain returning ..."
        << EndLogLine ;
        return 0 ;
}
#endif
#endif

static int
ControlMain(int argc, char **argv, char **envp)
  {
#if defined(PK_BLADE) && defined(PK_BGL)


  BegLogLine( PKFXLOG_MAIN  )
    << "ControlMain entered..."
    << EndLogLine;

  BGLInformationChoice( 0 );

#if defined( PK_BLADE_PROFILING )
  _blade_profile_setup( 10000,
                        &BladeProfilingTraceData[ 0 ],
                        BLADE_PROFILING_TRACE_DATA_SIZE,
                        0,
                        BLADE_PROFILING_TRACE_DATA_SIZE, // (1024 * 1024),
                        _BL_PROF_SIMPLE );
#endif

  if( argc == 3 )
    {
      serv_ip_addr.mVar = ( Bit32 ) htoi( argv[ 1 ] );
      serv_ip_port.mVar = ( Bit16 ) htoi( argv[ 2 ] );
      //serv_ip_addr.mVar = 0x902d916;
      //serv_ip_port.mVar = 0xebf5;

      // s0printf("IP Address of Server = %08x\n", serv_ip_addr.mVar );
      // s0printf("IP Port of Server = %04x\n", serv_ip_port.mVar );
    }
  else
    {
    serv_ip_addr.mVar = ( Bit32 ) 0x902d916 ;  // bln1en0
    serv_ip_port.mVar = ( Bit16 ) 0xc350 ;      // decimal 1390
    // serv_ip_addr.mVar = ( Bit32 ) 0x0902B8EB ;  // hsm00
    // serv_ip_addr.mVar = ( Bit32 ) 0x902b8bf ;  // locutus
    // serv_ip_port.mVar = ( Bit16 ) 0xb37e ;      // decimal 1390
    }

  Platform::Memory::ExportFence( (void*) &serv_ip_addr , sizeof( AtomicNative< Bit32 > ));
  Platform::Memory::ExportFence( (void*) &serv_ip_port , sizeof( AtomicNative< Bit16 > ));

  BegLogLine( PKFXLOG_MAIN )
    << " _CP_Main() "
    << "Sending to ip addr "
    << (void *) serv_ip_addr.mVar
    << " ip port "
    << (void *) serv_ip_port.mVar
    << EndLogLine;

  myKend.mVar = GetExtendedMemory();
  int NumberOfWords = (256*1024*1024 - myKend.mVar) / 4;
  for(int word=0; word <  NumberOfWords; word++)
    {
      *((unsigned *) myKend.mVar + word ) = 0xdeadbeef;
    }

  BegLogLine( PKFXLOG_MAIN  )
    << "ControlMain "
    << " About to set up the virtual fifo"
    << EndLogLine;

#ifdef BGL_VIRTUAL_FIFO
  pkNew( &VirtualFifo_PacketBuffer, VirtualFifo_PacketCount, __FILE__, __LINE__ );
  if( VirtualFifo_PacketBuffer == NULL )
    PLATFORM_ABORT("FAILED:: VirtualFifo_PacketBuffer == NULL");

#endif

  BGLTreeSetPtpAddress( BGLPartitionGetRank() );

  // The first IO node is the root of the tree.
  IO_COMP_comm_root = BGLPartitionGetNumNodesCompute();

#ifdef USE_BGL_TREE
  int rc = BGL_TreeRouteAllNodes( &IO_COMP_comm_root, TR_PKT_ONLY_TO_ROOT_NODE, &IO_COMP_comm_class );
#endif

  BegLogLine( PKFXLOG_MAIN )
    << "main:: "
    << " IO_COMP_comm_root=" << IO_COMP_comm_root
    << " IO_COMP_comm_class=" << IO_COMP_comm_class
    << EndLogLine;

    if( BGLPartitionIsIONode() )
      {
        //s0printf("_CP_Main() calling IONodeWorld_CP()\n");
      IONodeWorld_CP();
      }
    else
      {
        //s0printf("_CP_Main() calling ComputeNodeWorld_CP\n");
      ComputeNodeWorld_CP( argc, argv, envp );
      }

  s0printf("_CP_Main() returning to BLADE\n");
#else

  ComputeNodeWorld_CP( argc, argv, envp );

#endif

  return ( 1 );
  }


#ifdef BGL_VIRTUAL_FIFO
extern "C" {
  // T_CacheIsolatedBGLPacket* VirtualFifo_PacketBuffer = NULL;
T_VirtualFifo_Counter VirtualFifo_PutCnt; // ++ only by putter
T_VirtualFifo_Counter VirtualFifo_GotCnt;
T_VirtualFifo_Counter VirtualFifo_CachedPutCnt;
T_VirtualFifo_Counter VirtualFifo_CachedGotCnt;
T_CacheIsolatedBGLPacket* VirtualFifo_PacketBuffer = NULL ;// [ VirtualFifo_PacketCount ];
  //T_CacheIsolatedBGLPacket VirtualFifo_PacketBuffer[ VirtualFifo_PacketCount ];
}
#endif


#if defined( PK_BLADE )

class DummyInit
  {
  public:
    DummyInit()
      {
      /////IOP_CP_Barrier.mVar = 1;

      IOP_CP_Barrier.mVar = Platform::Lock::LOCKWORD_VALUE_MEMORY_LOCKED;

      ///s0printf("DummyInit constructor() loc var @0x%08x - %d\n",
      ///         &(IOP_CP_Barrier.mVar), IOP_CP_Barrier.mVar );
      }
  };

DummyInit dummy;

extern "C"
{
int
_IOP_main( int argc, char *argv[], char **envp )
  {
  // This Mutex value is create locked and is unlcoked by the CP after inits.
  IOP_CP_Barrier.mVar = Platform::Lock::LOCKWORD_VALUE_MEMORY_LOCKED;
  Platform::Mutex::SpinLock( & IOP_CP_Barrier );

  //s0printf("_IOP_main running ... done spinning\n");

  #if defined(PK_BLADE)
    GetExtendedMemory();
  #endif

    if( BGLPartitionIsIONode() )
      {
        //s0printf("_IOP_main calling IONodeWorld_IOP()\n");
      IONodeWorld_IOP();
      }
    else
      {
        //s0printf("_IOP_main calling ComputeNodeWorld_IOP()\n");
      ComputeNodeWorld_IOP();
      }

    s0printf("_IOP_main returning to BLADE\n");
    return ( 1 );
    }
};
#endif

#if defined( PK_BLADE )
AtomicNative< int > ExportFenceDone;
AtomicNative< int > ImportFenceDone;

int ExportFenceActor( void * args )
  {
  BegLogLine( PKFXLOG_EXPORT_FENCE_ACTOR )
    << "ExportFenceActor Entering... "
    << EndLogLine;

  Platform::Memory::ExportFence();

  BegLogLine( PKFXLOG_EXPORT_FENCE_ACTOR )
    << "ExportFenceActor Leaving... "
    << EndLogLine;

  ExportFenceDone.mVar=1;
  Platform::Memory::ExportFence( (void*) &ExportFenceDone, sizeof(AtomicNative<int>) );
  Platform::Memory::ImportFence( (void*) &ExportFenceDone, sizeof(AtomicNative<int>) );
  return 0;
  };

int ImportFenceActor( void * args )
  {
  BegLogLine( PKFXLOG_IMPORT_FENCE_ACTOR )
    << "ImportFenceActor Entering... "
    << EndLogLine;

  Platform::Memory::ImportFence();

  BegLogLine( PKFXLOG_IMPORT_FENCE_ACTOR )
    << "ImportFenceActor Leaving... "
    << EndLogLine;

  ImportFenceDone.mVar=1;
  Platform::Memory::ExportFence( (void*) &ImportFenceDone, sizeof(AtomicNative<int>) );
  Platform::Memory::ImportFence( (void*) &ImportFenceDone, sizeof(AtomicNative<int>) );

  return 0;
  };

void
ForceExportFence()
  {

  int MyX, MyY, MyZ;
  BGLPartitionGetCoords(&MyX, &MyY, &MyZ);

  _BGL_TorusPktHdr   BGLTorus_Hdr;
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   MyX,
                   MyY,
                   MyZ,
                   0,
                   (Bit32) ExportFenceActor,
                   0,
                   0,
                   _BGL_TORUS_ACTOR_BUFFERED );

  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , NULL, 0 );
  while( ExportFenceDone.mVar != 1 )
    {
      Platform::Memory::ImportFence( (void*) &ExportFenceDone, sizeof(AtomicNative<int>) );
    }
  ExportFenceDone.mVar=0;
  Platform::Memory::ExportFence( (void*) &ExportFenceDone, sizeof(AtomicNative<int>) );
  }

void
ForceImportFence()
  {
  int MyX, MyY, MyZ;
  BGLPartitionGetCoords(&MyX, &MyY, &MyZ);

  _BGL_TorusPktHdr   BGLTorus_Hdr;
  BGLTorusMakeHdr( & BGLTorus_Hdr,
                   MyX,
                   MyY,
                   MyZ,
                   0,
                   (Bit32) ImportFenceActor,
                   0,
                   0,
                   _BGL_TORUS_ACTOR_BUFFERED );

  VirtualFifo_BGLTorusSendPacketUnaligned( & BGLTorus_Hdr , NULL, 0 );
  while( ImportFenceDone.mVar != 1 )
    {
    Platform::Memory::ImportFence( (void*) &ImportFenceDone, sizeof(AtomicNative<int>) );
    }

  ImportFenceDone.mVar=0;
  Platform::Memory::ExportFence( (void*) &ImportFenceDone, sizeof(AtomicNative<int>) );
  }

#endif

void pkDelete( void* aToDelete, int isArray=0 )
{
  BegLogLine( 0 )
    << "pkDelete:: aToDelete = " << aToDelete
    << EndLogLine;

  if( aToDelete != NULL )
    {
#if PK_BLADE
      free( aToDelete );
#else
      if( isArray )
        delete [] aToDelete;
      else
        delete aToDelete;
#endif
    }
}

#ifdef PK_BLADE
///////////////////////#include <new>
void* operator new( unsigned size )
{
  unsigned paddedSize = pk_16B - ( size ) % pk_16B;
  unsigned newSize = size + paddedSize;

  void * rl = malloc( newSize );

  BegLogLine( PKFXLOG_NEW )
    << " new:: rl = " << rl
    << " newSize=" << newSize
    << EndLogLine;

  return rl;
}

void operator delete( void * ptr )
{
  BegLogLine( PKFXLOG_NEW )
    << "delete:: ptr = " << ptr
    << EndLogLine;

  free( ptr );
}

void* pkMalloc( unsigned size )
{
  unsigned paddedSize = pk_16B - ( size ) % pk_16B;
  unsigned newSize = size + paddedSize;
  void * rl = malloc( newSize );

  BegLogLine( PKFXLOG_NEW )
    << " pkMalloc:: rl = " << rl
    << " newSize=" << newSize
    << EndLogLine;

  return rl;
}

void pkFree( void* ptr )
{
  BegLogLine( PKFXLOG_NEW )
    << " pkFree:: ptr = " << ptr
    << EndLogLine;

  free( ptr );
}
#endif
