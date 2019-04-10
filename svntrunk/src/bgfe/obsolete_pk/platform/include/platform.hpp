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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          platform.hpp
 *
 * Purpose:         encapsulate interactions with the OS and Hardware.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010101 BGF Created from several other files in pk
 *
 ***************************************************************************/

#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#ifndef PKFXLOG_PKNEW
#define PKFXLOG_PKNEW ( 0 )
#endif

#ifndef PKFXLOG_SENT_TO
#define PKFXLOG_SENT_TO ( 0 )
#endif

#ifndef PKFXLOG_TOPOLOGY
#define PKFXLOG_TOPOLOGY ( 1 )
#endif

#ifndef PKFXLOG_STATISTIC
#define PKFXLOG_STATISTIC ( 1 )
#endif

#ifndef  PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR
#define  PKFXLOG_SEND_PACKET_THROUGH_ETHERNET_ACTOR ( 1 )
#endif

#ifndef PKFXLOG_COLLECTIVE
#define PKFXLOG_COLLECTIVE (0)
#endif

using namespace std ;
/// #define PK_MUTEX_LOGGING

// The following takes care of accessing the field of an MPI_Status struct for IBM and MPICH
#ifdef  PK_MPICH
  #define PK_MPI_ALL
//  extern "C"
//  {
#if !defined(PK_BLADE_SPI)
  #include <mpi.h>
#endif
//  }
  #define    MPI_STATUS_TAG(Status)    (Status.MPI_TAG)
  #define    MPI_STATUS_SOURCE(Status) (Status.MPI_SOURCE)
  #define    MPI_STATUS_ERROR(Status)  (Status.MPI_ERROR)
  #define     MPI_STATUS_COUNT(Status, Count)   ( Count = Status.count )

//extern "C" {
  int PMI_Comm_torus2rank( MPI_Comm, int,int,int,int );
//};

#endif

#ifdef  PK_MPI
  #define PK_MPI_ALL
//  extern "C"
//  {
#if !defined(PK_BLADE_SPI)
  #include <mpi.h>
#endif
//  }
  #define    MPI_STATUS_TAG(Status)    ( Status.tag )
  #define    MPI_STATUS_SOURCE(Status) ( Status.source )
  #define    MPI_STATUS_ERROR(Status)  ( Status.error )
  #define    MPI_STATUS_COUNT(Status, Count)  ( MPI_Get_count( &Status, MPI_CHAR, &Count ) )
#endif

#include <stddef.h>

#include <pk/XYZ.hpp>
#include <limits.h>

#ifdef PK_BGL
  #define MEMORY_ALIGN(NUMBITS) __attribute__(( aligned(0x01<<NUMBITS )))
#else
  #define MEMORY_ALIGN(NUMBITS)
#endif

typedef void*(*CO_FuncPtr)(void *);

#if defined(PK_BGL)
// POSIX-type file descriptors
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#define PK_POSIX 1
//#include <bgllockbox.h>
#include <bgllockbox.h>
#include <rts.h>
#include <bglpersonality.h>

// struct ActorWithAFlushStruct
// {
// CO_FuncPtr mFuncPtr;
// void*      mArgs;
// };

static void* FlushCore1( void* arg )
   {
   rts_dcache_evict_normal();
   return NULL;
   }


#if defined( PK_BLADE_SPI )
#define CACHE_ISOLATION_PADDING_SIZE ( 0 )

#include <spi/blade_on_blrts.h>
#include <pk/coprocessor_on_coprocessor.hpp>
#include <spi/packet_alltoallv.hpp>
#include <spi/tree_reduce.hpp>

////////////static PacketAlltoAllv BroadcastAlltoallv MEMORY_ALIGN(5);
static PacketAlltoAllv GeneralAlltoallv MEMORY_ALIGN(5);
#else

// We're using MPI massage layers co_start and co_join
extern "C"
{
int PMI_co_start( void*(*fp)(void *), void* arg );
void* PMI_co_join( int handle );
};
#endif

#define GET_BEGIN_PTR_ALIGNED( ptr ) ( (void *) ( ((unsigned )(ptr)) & ~0x1f ) )
#define GET_END_PTR_ALIGNED( ptr ) ( (void*) (((unsigned)(ptr)+31) & ~0x1f ) )

#endif

#if ( defined( PK_BGL ) && defined( PK_MPICH ) )
  // THIS IS BLRTS MPI ON SYSTEM SOFTWARE
//   #define PK_POSIX 1
//   #include <rts.h>
//   #include <bglpersonality.h>


#ifdef BGL_PERFCTR_OFF
//   #include <bgl_perfctr.h>
extern "C" {
int bgl_perfctr_shutdown(void);
};
#endif
//   #include <coprocessor.h>
//   #include <spad.h>

extern "C" {
  int co_start ( void *(*fp)(void *), void *arg );
  void *co_join (int);
};

#endif


#if defined(PK_BGLSIM)
  void turn_trace_on(void) ;
  void turn_trace_off(void) ;
#else
  static inline void turn_trace_on(void) { } ;
  static void turn_trace_off(void) { } ;
#endif

#define BGL_TREE_PACKET_PAYLOAD 256
//NOTE NOTE NOTE:  The 240 in the folling line is becuase this must fit in a virt fifo torus pkt
#define UDP_TREE_PACKET_PAYLOAD ( 240 - sizeof(int) )

struct IncomingUDPPacket
{
  int mAck;
};

extern "C" unsigned htoi( char* str );

#ifdef PK_BLADE
extern "C"
{
extern int FullPartitionClass;
extern int IntegerReductionClass;
extern int FullPartitionRoot;

extern int IO_COMP_comm_class;
extern int IO_COMP_comm_root;

extern char RecvReduceBuff[ 256 ] MEMORY_ALIGN(5);
extern char SendReduceBuff[ 256 ] MEMORY_ALIGN(5);
}


void SendReductionData( void* args );

// Set up for moving udp packets through IO node
// An int is a little heavy for the lenth - a byte or short would do.

#define UDP_PACKET_PAYLOAD_SIZE 1472

struct UdpPacket
{
  int   mLen;
  int   mSeqNum;
  char  mData[ UDP_PACKET_PAYLOAD_SIZE ];
};

struct UdpTreePacket
  {
  int  mLen;
  char mData[  BGL_TREE_PACKET_PAYLOAD - 2 * sizeof(int) ];
  } MEMORY_ALIGN( 5 );

UdpTreePacket UdpTreePacketBuffer MEMORY_ALIGN( 5 );
#endif

#if 1
template < class TYPE, int T_AlignmentBits = 5 >
class AtomicNative
  {
  public:
    enum { Alignment        = 0x01 << T_AlignmentBits };
    ///// TYPE mVar __attribute__ (( aligned( Alignment ) ));

    // COMPILER PROBLEM: g++ does not do constant folding on a shift
    TYPE mVar __attribute__(( aligned( 32 ))); //MEMORY_ALIGN( 5 );
    char mPadding[ Alignment - sizeof( TYPE ) ];

    inline
    TYPE
    operator ++ ( int )
      {
      mVar++;
      return( mVar );
      }

    inline
    TYPE
    operator=( TYPE& thing )
    {
      mVar = thing;
      return mVar;
    }

//     inline
//     TYPE &
//     operator & ()
//       {
//       return( mVar );
//       }

  } ;

template< class T_TrueType, int T_AlignmentBits = 5 >
class CacheIsolator
  {
  public:

    typedef T_TrueType TrueType;
    enum { AlignmentBits  = T_AlignmentBits };
    enum { Alignment      = 0x01 << AlignmentBits };
    enum { PaddedSize     = ( sizeof( T_TrueType ) + Alignment - 1) & (~ (Alignment - 1)) };

    union
      {
      char mBuf[ PaddedSize ];
      T_TrueType Instance;

      // COMPILER PROBLEM: g++ does not do constant folding on a shift
      } mThisUnion  __attribute__(( aligned( 32 )));
  } ;
#endif

//#endif

static char *
GetWriteErrorMessage();  // see bottom of this file.

#if defined( PK_BLADE ) || defined(PK_SPI)
  #include <string.h>
  #include <fcntl.h>
  #include <stdlib.h>
  #include <cstdio>
  using namespace std ;

  extern "C"
  {
  #include <sys/time.h>
  }
#endif // PK_BGL

#if defined( PK_BLADE) || defined(PK_SPI)
#include <errno.h>  // this need EAGAIN and should match BLADE
#include <sys/socket.h>

#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }

#define ALIGN_QUADWORD __attribute__ ((aligned ( 16)))
#define ALIGN_CACHELINE __attribute__((aligned(32)))
#define ALIGN_L3_CACHE __attribute__ ((aligned (128)))
typedef unsigned char       Bit8;
typedef unsigned short      Bit16;
typedef unsigned int        Bit32;
typedef unsigned long long  Bit64;
typedef unsigned long long  Bit64;
typedef unsigned int        Bit128[4];
typedef unsigned int        uint32_t;


#include <sys/blade_profiling.h>
#include <spi/BGL_PartitionSPI.h>
#include <spi/BGL_TorusPktSPI.h>
#include <spi/BGL_TreePktSPI.h>
#include <spi/BGL_EnetPktSPI.h>
#include <pk/treebroadcast.hpp>
#include <pk/treereduce.hpp>

#if defined(PK_BLACE) && !defined(PK_SPI)
#undef malloc
#undef free
#include <sys/malloc_DougLea.h>
#endif

//  #include <sys/machine/bl_lockbox.h>
#define LOCKBOX_NUM_LOCKS (256)

#define PA_TORUS_0      (0xB1140000)         // Torus Group 0 Base Physical Address
#define PA_TORUS_1      (0xB1150000)         // Torus Group 1 Base Physical Address

// Offsets within Torus Group for Reception Fifos
//   Read-Only, QuadWord Access Only.
#define TSx_R0       (0x0000)  // Offset for NormPri Reception Fifo 0
#define TSx_R1       (0x0400)  // Offset for NormPri Reception Fifo 1
#define TSx_R2       (0x0800)  // Offset for NormPri Reception Fifo 2
#define TSx_R3       (0x0C00)  // Offset for NormPri Reception Fifo 3
#define TSx_R4       (0x1000)  // Offset for NormPri Reception Fifo 4
#define TSx_R5       (0x1400)  // Offset for NormPri Reception Fifo 5
#define TSx_RH       (0x2000)  // Offset for HighPri Reception Fifo H

// Offsets within Torus Group for Injection Fifos
//   Write-Only, QuadWord Access Only.
#define TSx_I0       (0x3000)  // Offset for NormPri Injection Fifo 0
#define TSx_I1       (0x4000)  // Offset for NormPri Injection Fifo 1
#define TSx_I2       (0x5000)  // Offset for NormPri Injection Fifo 2
#define TSx_IH       (0x6000)  // Offset for HighPri Injection Fifo H

// Offsets within Torus Group for Status
//   Read-Only, Quad-Word
#define TSx_Sx       (0x7000)  // Offset for Status of this Group
#define TSx_SO       (0x7010)  // Offset for Status of the Other Group


// Physical Addresses of Torus Group 0 (P=V to use these)
//  Note: in "split mode", both halves should use these addresses
//   and the VMM should map the cores' group at the same VA, different PA.
#define TS0_R0    (PA_TORUS_0 | TSx_R0)
#define TS0_R1    (PA_TORUS_0 | TSx_R1)
#define TS0_R2    (PA_TORUS_0 | TSx_R2)
#define TS0_R3    (PA_TORUS_0 | TSx_R3)
#define TS0_R4    (PA_TORUS_0 | TSx_R4)
#define TS0_R5    (PA_TORUS_0 | TSx_R5)
#define TS0_RH    (PA_TORUS_0 | TSx_RH)
#define TS0_I0    (PA_TORUS_0 | TSx_I0)
#define TS0_I1    (PA_TORUS_0 | TSx_I1)
#define TS0_I2    (PA_TORUS_0 | TSx_I2)
#define TS0_IH    (PA_TORUS_0 | TSx_IH)
#define TS0_S0    (PA_TORUS_0 | TSx_Sx)
#define TS0_S1    (PA_TORUS_0 | TSx_SO)

// Physical Addresses of Torus Group 1 (P=V to use these)
#define TS1_R0    (PA_TORUS_1 | TSx_R0)
#define TS1_R1    (PA_TORUS_1 | TSx_R1)
#define TS1_R2    (PA_TORUS_1 | TSx_R2)
#define TS1_R3    (PA_TORUS_1 | TSx_R3)
#define TS1_R4    (PA_TORUS_1 | TSx_R4)
#define TS1_R5    (PA_TORUS_1 | TSx_R5)
#define TS1_RH    (PA_TORUS_1 | TSx_RH)
#define TS1_I0    (PA_TORUS_1 | TSx_I0)
#define TS1_I1    (PA_TORUS_1 | TSx_I1)
#define TS1_I2    (PA_TORUS_1 | TSx_I2)
#define TS1_IH    (PA_TORUS_1 | TSx_IH)
#define TS1_S1    (PA_TORUS_1 | TSx_Sx)
#define TS1_S0    (PA_TORUS_1 | TSx_SO)


// X+ TS0_I0
// X- TS0_I1

// Y+ TS0_I2
// Y- TS1_I0

// Z+ TS1_I1
// Z- TS1_I2

static void VirtualFifo_BGLTorusSendPacketUnaligned( _BGL_TorusPktHdr* hdr, void* data, int bytes );

extern "C" {

  extern _BGL_TreeHwHdr addReduceHdr MEMORY_ALIGN(5);
  extern _BGL_TreeHwHdr addReduceHdrReturned MEMORY_ALIGN(5);
  extern _BGL_TorusPktHdr   SendReductionDataHdr MEMORY_ALIGN(5);

    // extern unsigned int BladeProfilingTraceData[ BLADE_PROFILING_TRACE_DATA_SIZE ] MEMORY_ALIGN(5);

  extern void aLockBoxSpinLock( int b );
  extern int aLockBoxTryLock( int b );
  extern void aLockBoxUnLock( int b );
  extern int a_hard_processor_id();
  extern unsigned long long a_GetTimeBase(void);

  extern void dcache_flush_bytes( void *start, Bit32 bytes );
  extern void dcache_flush_all(void);
  extern void dcache_store_bytes( void *start, Bit32 bytes );
  extern void __delay( Bit32 loops );

  extern unsigned GetExtendedMemory(void);
  // extern AtomicNative< unsigned long long > BladeBaseTime;

  extern int our_check_for_link_errors(int my_x, int my_y, int my_z);

  // extern void GetMagicDataPoiters( unsigned* p1, unsigned* p2);

//   extern int _blade_vmm_map( Bit32 paddr,
//                              Bit32 vaddr,
//                              Bit32 bytes,
//                              Bit32 vmm_flags );

}

extern void  ForceExportFence();
extern void  ForceImportFence();
extern AtomicNative< unsigned >  myKend;
template <class Type> inline void pkNew( Type* objToNewPtr, unsigned size );
char strBuf[512] MEMORY_ALIGN(5);

// #define _BN(b)    ((1<<(31-(b))))
// #define VMM_L2_PF_OPTIMISTIC       _BN( 5 ) // default is Automatic Stream Detection
// #define VMM_KERNEL_DATA            _BN( 9 ) // Implies SR and SW, excludes SX


// #define LockBoxUnLock(lock_num)    LockBoxForce((lock_num),0)

#define PK_LOCKBOX_PlatformLock_Bit 255
#define PK_USER_LOCKBOX_LOCKS       128

#endif // PK_BLADE

// Support for explicitly declaring parallel computation
#if defined( PK_BGL ) && defined( PK_XLC )
  extern "builtin" void __compute_parallel(double, double, ...) ;
  #define compute_parallel __compute_parallel
#else
  // No-op it on compilers that don't know about Double Hummer
  static inline void compute_parallel(double X0, double X1, ...)
    {
    }
#endif
#if defined( PK_BLADE ) && defined ( PK_XLC )
  #define PRINTF_FUNCTION s0printf
  #define EXIT_FUNCITON Sim_Terminate
  extern "C" {
     int s0printf(const char *, ... ) ;
     void Sim_Terminate(int) ;
     }
#else
  #define PRINTF_FUNCTION printf
  #define EXIT_FUNCTION exit
#endif // PK_BGL


#ifdef PK_LINUX
  #include <sys/wait.h>  // for waitpid() in set
  #define PK_POSIX
#endif

#ifdef PK_AIX
  extern "C"
  {

  extern void exit(int);

  extern void yield(void);

  #ifdef PK_SMP
  #include <sys/atomic_op.h>
  #endif

  #include <sched.h>

  #include <pk/setpri.hpp>
  #define PK_POSIX  // pull in furhter header files

  }
#endif // PK_AIX




#ifdef PK_POSIX

  #include <cstdlib>
  extern "C"
  {
  #include <malloc.h>  // for NULL???
  #include <stddef.h>
  #include <signal.h>

  //#include <stdlib.h>
  #include <string.h>
  #include <stdio.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <assert.h>
  #include <errno.h>
  #include <time.h>
  #include <sys/time.h>

#ifdef PK_AIX
  #include <sys/types.h>
  #include <netinet/in.h>
#else
    struct in_addr {
        unsigned int s_addr;
    };

    struct sockaddr_in {
        unsigned short sin_family;
        unsigned short sin_port;
        struct in_addr sin_addr;
        unsigned char sin_zero[8];
   };
#endif

  #include <sys/socket.h>

extern int sockfd;

  #ifdef PK_SMP
    #define PK_PTHREADS
  #endif // PK_SMP
  }

#endif // PK_POSIX


#ifdef  PK_PTHREADS
  #include <pthread.h>
#endif

#ifdef PK_XLC

  #if __IBMCPP__ >= 500
    // 'new' VisualAge C++ supports and requires typename
    #define A98_TYPENAME typename
  #else
    // 'old' VisualAge C++ doesn't support typename and doesn't know bool
    #define A98_TYPENAME

    #if !defined(bool)
      typedef unsigned int bool ;
    #endif

  #endif // __IBMCPP__ >= 500

#endif

#ifdef PK_GCC
  ////// BGF why is this here?  typedef int bool ;
  #define A98_TYPENAME typename  // Shouldn't this be down in PK_GCC ?
#endif

// Universal macros

#ifndef TRUE
  enum { TRUE = 1 };
#endif
#ifndef FALSE
  enum { FALSE = 0 };
#endif

#define PLATFORM_ABORT(reason) \
   Platform::Control::Abort( reason, __FILE__, __LINE__)

//Define assert to call platform abort.
// DO NOT INCLUDE assert.h !!!!
#ifdef NO_PK_PLATFORM
  #include <assert.h>
#else
  // vacpp on linux will pull 'assert.h' in (via STL ?), so pretend we already
  // have it, to avoid macro mismatches
  #undef _ASSERT_H
  #define _ASSERT_H 1
  #undef assert

  #ifdef NDEBUG
    #define assert(ignore) ((void)0)
  #else
    #define assert(__EX) ((__EX) ? ((void)0) : Platform::Control::Abort(# __EX, __FILE__, __LINE__))
  #endif
#endif // NO_PK_PLATFORM

// Set up diagnostic versions of math functions, so we can see callers

#if defined(USE_SQRT_WRAP)
  #include <math.h>
  // Wrapper for square root calls, so we can see who is using so many
  double sqrt_wrap(double X) ;
  #if defined(sqrt)
    #undef sqrt
  #endif
  #define sqrt(X) sqrt_wrap(X)
#endif // defined(BLUEGENE_DIAG_MATH)

typedef void * (*PointerToThreadFunction)( void* );

static const char HexToChar[17] = "0123456789abcdef" ;

struct SendReductionDataPacket
{
  char * mAddress;
  int    mLength;
};

#ifdef PK_BLADE
inline void checkTreeRecvFifoStatusVC0()
{
  _BGL_TreeFifoStatus stat;
  while(1)
    {
    BGLTreeGetStatusVC0( &stat );

    if( ( stat.RecHdrCount == 0 ) ||
        ( stat.RecPyldCount < 16 ) )
      {
      continue;
      }
    break;
    }
}

inline void checkTreeSendFifoStatusVC0()
{
  _BGL_TreeFifoStatus stat;
  while(1)
    {
    BGLTreeGetStatusVC0( &stat );

    if( ( stat.InjHdrCount == 8 ) ||
        ( stat.InjPyldCount > 112 ) )
      {
      continue;
      }
    break;
    }
}
#endif

class Platform  // this class is mainly to provide a name space
  {
  public:
    class Control
      {
      public:
#ifdef PK_BGL
        static BGL_Barrier *mIntraCoreBarrier;
#endif

        static void
        Init()
          {
          #ifdef PK_BGL
            mIntraCoreBarrier = rts_allocate_barrier();
            assert( mIntraCoreBarrier );
          #endif
          }

#ifdef PK_BGL
////////////////// USED FOR DEBUGGING - PROBABLY NOT USEFUL
//////////////////// Code in 'bgllockbox' looks to be making a mess of barrier pass for xlC.
//////////////////// ( compiler bug ?) However, xlC seems to make the right code from the gcc source.
//////////////////// Try it as workaround.
//////////////////
//////////////////static inline void
//////////////////IntraCoreBarrier(BGL_Barrier *p)
//////////////////      {
//////////////////        BGL_Lock_Msync();
//////////////////        p->_lock[0]._word2 = 0;
//////////////////        while (p->_lock[0]._word2)
//////////////////           ;
//////////////////        BGL_Lock_Msync();
//////////////////      }
        static void
        IntraCoreBarrier()
          {
          BGL_Barrier_Pass( mIntraCoreBarrier );
          }


        static void
        BarrierWithWaitFunction( void *fx, void *arg )
          {
 #ifdef PK_BLADE_SPI
          BGLGI_BarrierWithWaitFunction( fx, arg );
 #endif
          }

#endif
        static void
        Barrier()
          {
           #ifdef PK_MPI_ALL
              // MPI_Barrier( MPI_COMM_WORLD );
           MPI_Barrier( Topology::cart_comm );
           #endif

          #ifdef PK_BLADE_SPI
             if( rts_get_processor_id() == 0 )
               {
               BGLPartitionBarrierComputeCore0();
               }
             else if ( rts_get_processor_id() == 1 )
               {
               BGLPartitionBarrierComputeCore1();
               }
             else
               {
               PLATFORM_ABORT( "Barrier(): hard_processor_id() did not return 0 or 1 ... not a BG/L?" );
               }
          #endif
        }

        static void
        BarrierBothCores()
          {
          #ifdef PK_BLADE_SPI
             BGLPartitionBarrierCompute();
          #else
             assert( 0 );
          #endif
        }

        static void
        Abort(char *reason, char *file, int line )
          {

#ifdef PK_POSIX

          fprintf(stderr, "Platform::Abort() [%d:%d] >%s< line %d, file >%s<\n",
             Platform::Topology::GetAddressSpaceId(),
             Platform::Thread::GetId(),
             reason, line, file );
          perror("errno at platform abort");
          fflush(stderr);  // shouldn't be required but shouldn't hurt either.
          kill( getpid(), SIGTERM ); // give a debugger half a chance catch this
#endif

#ifdef PK_BLADE
          s0printf( "Platform::Abort() [%d:%d] >%s< line %d, file >%s<\n",
                     Platform::Topology::GetAddressSpaceId(),
                     Platform::Thread::GetId(),
                     reason, line, file );
          Sim_Terminate(-1) ;
#endif

#ifdef PK_MPI_ALL
          MPI_Abort(MPI_COMM_WORLD, -1);
#endif

#ifdef PK_POSIX
          exit(-1);
#endif
          }

        static void
        Finalize()
          {
          #ifdef PK_MPI_ALL
            MPI_Finalize();
          #else
            exit( 0 );
          #endif
          }
      };

    //*********************** Reactor *************************************


    class Reactor
      {
      public:

        static PointerToThreadFunction mCurrentThreadFx;
        static void*                   mCurrentData;
        static int                     mGoAhead;

        enum{ RETURN_OK=0, RETURN_DETATCH_BUFFER=1, RETURN_ERROR=-1 };

#ifndef PK_PLATFORM_REACTOR_PAYLOAD_SIZE
#ifdef  PK_SPI

        //todo: tjcw 200502223 get this value from its proper place
#if !defined(BG_MESSAGE_LARGE_DATA_LIMIT)
#define BG_MESSAGE_LARGE_DATA_LIMIT 32
#endif

#define PK_PLATFORM_REACTOR_PAYLOAD_SIZE  (BG_MESSAGE_LARGE_DATA_LIMIT*sizeof(double))
#else
// All other times, figure it's MPI or SMP on AIX and use large datagrams.
#define PK_PLATFORM_REACTOR_PAYLOAD_SIZE  32*1024
#endif
#endif //#ifndef PK_PLATFORM_REACTOR_PAYLOAD_SIZE

        enum{ PAYLOAD_SIZE = PK_PLATFORM_REACTOR_PAYLOAD_SIZE };
        //enum{ PAYLOAD_SIZE = 256 };

#ifdef PK_UDP
        // It doesn't really work to set this hear - then, the UDP reactor has to count on getting this plus header space in the MTU
        enum{ BROADCAST_PAYLOAD_SIZE = 1400 };  // ethernet packet
#else
        enum{ BROADCAST_PAYLOAD_SIZE = PAYLOAD_SIZE };
#endif

#undef  PK_PLATFORM_REACTOR_PAYLOAD_SIZE

        // All functions to be triggered by active packets must have the following type.
        typedef int (*FunctionPointerType)( void     *MsgBuf   );

        static void
        FreeBuffer( void *MsgBuf );

        ///  IS THERE A CASE WHERE WE WANT TO REQUIRE THE USER TO PRE-ALLOCATE BUFFERS?
        ///  OR IS IT GOING TO BE OK TO MAKE BUFFERS OUT OF NORMAL C++ MEMORY.
        static void *
        GetBuffer();

#if defined( PK_BLADE ) && defined( PK_BGL )
          static void BGLTorusSimpleActivePacketSend( _BGL_TorusPktHdr & pktHdr,
                                                      Bit32              ActorFx,
                                                      int                x, int y, int z,
                                                      int                len,
                                                      void             * data )
           {
               // RULE TO FORCE SENDS OF ALL PACKETS WHEN USING BOTH CORES.
           // Application loopback on stack is dangerous because it causes a dual writer situation.
           // On BG/L, this could cause sharing of cache lines between cores and thus, data loss.
#ifdef USE_2_CORES_FOR_FFT
#define FFT3D_SEND_LOCAL_ACTORS
#endif

#ifndef FFT3D_SEND_LOCAL_ACTORS  // Allow a flag to test loop back

           int myPx, myPy, myPz;
           BGLPartitionGetCoords( &myPx, &myPy, &myPz );
           if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
             {
             ((BGLTorusBufferedActivePacketHandler)(ActorFx))( (Bit8 (*)[240])data, pktHdr.swh1.arg, pktHdr.swh0.extra );
             }
           else
#endif

#ifdef USE_2_CORES_FOR_FFT
           int myPx, myPy, myPz;
           BGLPartitionGetCoords( &myPx, &myPy, &myPz );
           if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
             {
             VirtualFifo_BGLTorusSendPacketUnaligned( & pktHdr,      // Previously created header to use
                                                      data,     // Source address of data (16byte aligned)
                                                      len );
             }
           else
             {
             BGLTorusSendPacket( & pktHdr,  // Previously created header to use
                                 data,    // Source address of data (16byte aligned)
                                 len );   // Payload bytes = ((chunks * 32) - 16)

#if 0
             if( rc == 1 )
               {
               // if in PrevFullQueue state dump the finis.
               if( PrevFullQueue )
                Full_Send_Queue_State_Finis.HitOE( 0,// PKTRACE_TUNE && !PKTRACE_OFF,
                                                   "Full_Send_Queue_State_Finis",
                                                   0,
                                                   Full_Send_Queue_State_Finis );

               FullQueue = 0;
               break;
               }

             FullQueue = 1;

             // Start of the edge
             if( PrevFullQueue != FullQueue )
               Full_Send_Queue_State_Start.HitOE( 0,//PKTRACE_TUNE && !PKTRACE_OFF,
                                                  "Full_Send_Queue_State_Start",
                                                  0,
                                                  Full_Send_Queue_State_Start );

             PrevFullQueue = FullQueue;
             }
#endif
           }
#else
        {

            int PollCount = 0;
            while( 1 )
              {
              int rc = BGLTorusSendPacketUnalignedNonBlocking( & pktHdr,  // Previously created header to use
                                                               data,    // Source address of data (16byte aligned)
                                                               len );   // Payload bytes = ((chunks * 32) - 16)
              // send succeds if rc == 0 , otherwise go round, drain network and try again.
              if( rc == 0 )
                break;

              PollCount++;

               #ifndef USE_2_CORES_FOR_FFT
                // NEED TO DRAIN THE NETWORK
                 while( 1 )
                  {
                  int count = 1;
                  BGLTorusPoll( &count );
                  if( count == 1 )
                    break;
                  }
               #endif

               #ifndef FFT3D_SPINWAIT
                 if( (PollCount % 16) == 0 )
                   {
                   usleep(10000);
                   }
               #endif
              }

        }
#endif
    }
#endif

#if defined(PK_SPI)
                // 'poll' and 'drain' likely only implemented in contexts where we do not have threads, e.g. SPI
                static int      Poll(void) ;  // Synchronously fetch and execute one active message
                static int Drain(void) ;  // Synchronousld fetch and execute all pending messages
#endif

        static int
        Trigger( int                             AddressSpaceId, // Target address space
                 Reactor::FunctionPointerType    FxPtr,          // actor function to execute
                 void                           *MsgBuf,         // pointer to payload ( len < PAYLOAD_SIZE )
                 unsigned                        MsgLen);

#ifdef PK_UDP
        // THIS SIG IS A BCAST
        static int
        Trigger( Reactor::FunctionPointerType    FxPtr,          // actor function to execute
                 void                           *MsgBuf,         // pointer to payload ( len < PAYLOAD_SIZE )
                 unsigned                        MsgLen);
#else
        static int
        Trigger( Reactor::FunctionPointerType    FxPtr,          // actor function to execute
                 void                           *MsgBuf,         // pointer to payload ( len < PAYLOAD_SIZE )
                 unsigned                        MsgLen)
          {
          int rc = 0;
          for(int i = 0; i < Platform::Topology::GetAddressSpaceCount(); i++ )
            {
            rc = Trigger(  i , FxPtr, MsgBuf, MsgLen );
            if( rc != 0 )
              break;
            }
          return( rc );
          }
    #endif

        static void SetGoAheadAndFunctionData(PointerToThreadFunction ThreadFx, void* data )
        {

            // printf("Platform::Reactor::SetGoAheadAndFunctionData\n");
            // fflush(stdout);

          Platform::Memory::ImportFence();
          mCurrentThreadFx = ThreadFx;
          mCurrentData     = data;
          mGoAhead         = 1;
          Platform::Memory::ExportFence();
          // printf("mGoAhead2=%d\n", mGoAhead);
        }

        static void* PollAndExecute( void* aBarrierData )
        {

          while(1)
            {
              // Poll on an address.
              // If the GO_AHEAD flag is non-zero execute the function at some address.
              // else poll nicely.
              Platform::Memory::ImportFence();
              if( mGoAhead )
                {
                  (*mCurrentThreadFx)( mCurrentData );
                  mGoAhead=0;
                }
              else
                Platform::Thread::Yield();
            }

          return NULL;
        }
      };

    //*********************** Sync *************************************

    class Lock
      {
      // As a user of the platform:: interface, you should probably use MUTEX.
      // This is because these locks don't fence memory - bad for RS6K.
      public:
        #if defined( PK_BGL )
        // typedef volatile int LockWord ;
          typedef BGL_Mutex LockWord;
        #else
          typedef int LockWord;
        #endif

        //Takes a reference to a LockWord
        //Returns TRUE for success and FALSE for failure.
        static inline int
        TryLock( LockWord* x )
          {
          int rc = 0;

#ifndef PK_BGL
          #if ( defined( PK_SMP ) && defined (PK_AIX) )
            int y = 0, z = 1;
            rc =  ! _check_lock ( x, y, z );
          #elif defined( PK_BGL )
            assert( 0 );
          #else
            if( *x == 0 )
              {
              *x = 1;
              rc = 1;
              }
          #endif
#endif
          return( rc );
          }

        static inline void
        Unlock( LockWord* x )
          {
#ifndef PK_BGL
          #if ( defined( PK_SMP ) && defined (PK_AIX) )
            int y = 0;
            _clear_lock( x, y ) ;
          #else
            //NOTE: this works for SPI currently but would like to get a SPI call
            *x = 0;
          #endif
#endif
          }
      };

    class Comm
    {
    public:
      static int      mRemotePort;
      static unsigned mRemoteHostIP;
      static struct sockaddr_in mRemoteAddr;

      static int       mDebugP1_5ListeningPort;
    };

    // *********************** Memory Control *********************************

    class Memory
      {
      public:

        // 'safe' alignment so that loads and stores will work whatever size of data they are loading
        enum {
           Alignment = 8
           } ;
        int Align (int Offset)
        {
           return ( ( Offset + (Alignment-1)) / Alignment ) * Alignment ;
        } ;
        int MarkRegionUncached( void *, int len) { return(0);}

#ifndef PK_AIX
        typedef volatile unsigned int AtomicInteger;  // per spi/locks.h ... would like an opaque SPI type.
#else
        typedef int AtomicInteger;
#endif

        // NEED: check with Derek about ADD_M.  Need an interface to use this for counters.
        // NEED: look at AtomicOps.hpp and square what is to be done there and what here.
        static inline int
        AtomicAdd( AtomicInteger &aAtomicInteger, int aValue )
          {

#if ( defined( PK_SMP ) && defined ( PK_AIX ) )
          int Rc = fetch_and_add( &aAtomicInteger, aValue );
#elif defined( PK_BLADE )
          // This spin will be very brief - so we don't return without getting in.
          aLockBoxSpinLock( PK_LOCKBOX_PlatformLock_Bit );
          Platform::Memory::ImportFence( (void *)&aAtomicInteger, sizeof( aAtomicInteger ) );
          aAtomicInteger += aValue;
          Platform::Memory::ExportFence( (void *)&aAtomicInteger, sizeof( aAtomicInteger ) );
          aLockBoxUnLock( PK_LOCKBOX_PlatformLock_Bit  );
          int Rc = aAtomicInteger;
#else
          aAtomicInteger += aValue;
          int Rc = aAtomicInteger;
#endif
          return( Rc );
          }

// For BG/L SPI, we're not going to use export/import fence. They are
// intended for coherent-memory SMPs, and here we have an incoherent-memory
// SMP. We'll put in heavier things when we need them
// tjcw 2005-03-09
        // Export and Import fences deal with losely ordered memory architectures.
        // ExportFence ensures that all stores have been done (as far as L1 cache) before
        // letting any more stores start
        static inline void
        ExportFence( void *start = NULL, int bytes = -1 )
          {
#ifdef PK_SPI
// Ignore 'export fence' requests when in virtual-node mode
#endif

#if ( defined( PK_SMP ) && defined ( PK_AIX ) )
          // Uses the side effects of _clear_lock to export all data to main memory
          // Will be implemented more directly as asm soon.
          int a ;
          _clear_lock(&a,0) ;
#elif defined( PK_BLADE )
          // dcache_flush_all();
          if( start == NULL )
             dcache_flush_all();
          else
            dcache_store_bytes( start, (Bit32) bytes );
#endif
          }

        // ImportFence ensures that any 'load' instructions before here in program order
        // complete before any 'load' instructions after here in program order are
        // started
        static inline void
        ImportFence( void *start = NULL, int bytes = -1 )
          {
#ifdef PK_SPI
// Ignore 'import fence' requests in virtual node mode
#endif
#if ( defined( PK_SMP ) && defined (PK_AIX) )
          // Uses the side effects of _clear_lock to export all data to main memory
          // Will be implemented more directly as asm soon.
          int a = 0 ;
          (void) _check_lock(&a, 0, 0) ;
#elif defined( PK_BLADE )
          // dcache_flush_all();
          if( start == NULL )
            dcache_flush_all();
          else
            dcache_flush_bytes( start, (Bit32) bytes );
#endif
          }
        // ExportFence and ImportFence between them can be used to ensure that on a
        // weakly-ordered-memory SMP, a 'reader' thread can coherently see all the
        // changes made by a 'writer' thread. Only use them when this coherence is
        // necessary; the purpose of weakly-ordered-memory is to improve performance
        // when the coherence is not necessary
        // If you are running non-threaded, then you could dummy out these functions.
        // If you are running on a uniprocessor, you could dummy out these functions,
        // or you could leave them and they will be fairly-efficiently dummied by the
        // AIX kernel millicode
      };

    //*********************** mutex *************************************
    // Mutex: a lock with a memory fence
    //        Although these are built from above primitives, it is thought
    //        that some platforms might provide a more direct interface.

    class Mutex
      {
      public:
       typedef Platform::Lock::LockWord MutexWord;

       static inline void
        Acquire( MutexWord *aLockWord )
          {
          #ifdef PK_MUTEX_NOOP
            return;
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Entering Acquire...\n" );
            fflush( stdout );
          #endif

          #ifdef PK_BGL
            BGL_Mutex_Acquire( aLockWord );
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Leaving Acquire...\n" );
            fflush( stdout );
          #endif
          }

        static inline int
        IsLocked( MutexWord *aLockWord )
          {
          #ifdef PK_BGL
            return BGL_Mutex_Is_Locked( aLockWord );
          #else
            return 0;
          #endif
          }

        static inline int
        TryLock( MutexWord *aLockWord )
          {
          #ifdef PK_MUTEX_NOOP
            return 0;
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Entering Trylock...\n" );
            fflush( stdout );
          #endif

          #ifdef PK_BGL
            return BGL_Mutex_Try( aLockWord );
          #else
            int rc = Lock::TryLock( aLockWord );
            Memory::ImportFence( (void *) aLockWord, sizeof( MutexWord ) );
            return( rc );
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Leaving Trylock...\n" );
            fflush( stdout );
          #endif
          }

        static inline void
        YieldLock( Platform::Lock::LockWord* aLockWord )
          {
          #ifdef PK_MUTEX_NOOP
            return;
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Entering YieldLock...\n" );
            fflush( stdout );
          #endif

          #ifdef PK_BGL
            SpinLock( aLockWord );
          #else
            while( ! Lock::TryLock( aLockWord ) )
              Platform::Thread::Yield();
            Memory::ImportFence();
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Leaving YieldLock...\n" );
            fflush( stdout );
          #endif
          }

        //// SpinLock() not used today, probably a bad idea but might have a place on
        //// on a platform which had multiple physical processors AND a greater number of
        //// threads.  Really though, 3YieldLock ought to be good enough!
        ////inline void
        ////SpinLock( Platform::Lock::LockWord &aLockWord )
        ////  {
        ////  while( ! Lock::TryLock( aLockWord ) );
        ////  }

        static inline int
        SpinLock( MutexWord* x )
          {
          #ifdef PK_MUTEX_NOOP
            return 0;
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Entering SpinLock...\n" );
            fflush( stdout );
          #endif

          int rc = 0;

          #ifdef PK_BGL
            Acquire( x );
          #else
            while( rc == 0 )
              rc = TryLock( x );
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Leaving SpinLock...\n" );
            fflush( stdout );
          #endif
          return( rc );
          }


       /****************************************
        * NOTE: Need to figure out how to unify
        * Unlock and Release
        **********************************/
        static inline void
        Unlock( MutexWord* aLockWord )
          {
          #ifdef PK_MUTEX_NOOP
            return;
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Entering Unlock...\n" );
            fflush( stdout );
          #endif

          #ifdef PK_BGL
            BGL_Mutex_Release( aLockWord );
          #else
            Memory::ExportFence( (void *) aLockWord, sizeof( MutexWord ));
            Lock::Unlock( aLockWord );
          #endif

          #ifdef PK_MUTEX_LOGGING
            printf( "Leaving Unlock...\n" );
            fflush( stdout );
          #endif
          }
      };

    //*********************** Thread *************************************
    // Physical threads:
    // Make progress independently and asynchronously
    // Either processors or time-sliced processes - Yields no-oped for dedicated proc.
    // Share a Address Space of loosely ordered main memory.

    class Thread
      {
      public:
        enum{ MAX_THREAD_COUNT = 500 };

        // For a machine like cyclops, the above thread count
        // and the return of the following method are the same.
        static inline int
        GetCpusPerAddressSpaceCount()
          {
#ifndef PK_SET_COUNT_CPU_PER_ADDRESS_SPACE
          int Rc = 1;
#else
          int Rc = PK_SET_COUNT_CPU_PER_ADDRESS_SPACE;
#endif
          return( Rc );
          }

        // Might like this to be a member template function.
        // This allows scheduler to poll application conditions for
        // ready-to-run status without context switching into yielded thread.
        static inline void
        YieldUntil( int ((*UntilFx)(void *data)), void *data )
          {
          while( ! ((*UntilFx)(data)) )
            Platform::Thread::Yield();
          }


        static inline int
        Yield(int To = -1 ) // dubious argument that is a hint on which thread to yeild to, ignored in pthreads
          {
          #ifdef PK_POSIX
            #if !defined(PK_BGL)
            sched_yield();
            #endif
          #endif
          return(0);
          }

        static inline int
        Sleep( int timeUnit ) // I'm not too happy about this here. It could be done as YieldUntil() fx.
          {
          #if defined( PK_BLADE )
            __delay( timeUnit );
          #elif defined ( PK_POSIX )
            sleep( timeUnit );
          #endif
          return(0);
          }

        static
        int
        Create( PointerToThreadFunction  ThreadFx, // What function to run
                void          *Arg = NULL,         // Argument data pointer
                int            IdAssignment = -1,  // Bind to a processor id: -1
                int            StackSize = -1,     // Stack size in bytes: -1 is default
                int            Flags  = 0     );   // Operation flags - eg: sync.

        static void
        Exit( void * value )
          {
          #ifdef PK_PTHREADS
            pthread_exit( value );
          #endif
          }

        enum { ID_INVALID = -1 };

        static inline
        int
        GetId()
          {
          int ThreadId = 0;

          #if defined(PK_BGL)
            ThreadId = rts_get_processor_id();
          #elif defined ( PK_PTHREADS )
          // Need to get the context array struct index as the id.
            ThreadId = (((int)pthread_self()) % 0x0000FFFF);
          #endif
          return( ThreadId );
          }

      };

    //*********************** Clock and time values ****************************

    class Clock
      {
      public:
        static inline long long Oscillator(void) {
        // Get a 'steady monotonic' time representation
        // For ppc-type machines like BG/L, this will be the 64-bit
        // timebase register value.
        // For 'ancient' power1-type machines it would be misleading
        // If we are on a non-BGL non-AIX machine, return 0.
                  #if defined(PK_BGL)
                  return rts_get_timebase() ;
                  #elif defined(PK_AIX)
                  timebasestruct_t Now;
                  read_real_time( &Now, TIMEBASE_SZ );
                  return (((unsigned long long) Now.tb_high) << 32) | Now.tb_low ;
                  #else
                  return 0 ;
                  #endif

        }

        class TimeValue
        {
        public:
          int Seconds;      // often seconds
          int Nanoseconds;  // often nano-seconds

          static inline TimeValue MinValue()
            {
            TimeValue rc;
            rc.Seconds = 0;
            rc.Nanoseconds = 0;
            return ( rc );
            }

          static inline TimeValue MaxValue()
            {
            TimeValue rc;
            rc.Seconds = 0x7fffffff;
            rc.Nanoseconds = 0x7fffffff;
            return ( rc );
            }

          inline void Zero()
            {
            Seconds = 0;
            Nanoseconds = 0;
            }

          inline
          TimeValue operator+=( const TimeValue& aOther )
            {
            int nanoSum = Nanoseconds + aOther.Nanoseconds;

            int difference = nanoSum - 1000000000;

            // If carry over occured adjust for it
            if ( difference < 0 )
              {
              Seconds += aOther.Seconds;
              Nanoseconds = nanoSum;
              }
            else
              {
              Seconds += (aOther.Seconds + 1);
              Nanoseconds = difference;
              }

            return (*this);
            }

          inline
          TimeValue operator-( const TimeValue& aOther ) const
            {
              TimeValue rc;
              rc.Seconds = Seconds - aOther.Seconds;
              rc.Nanoseconds = Nanoseconds - aOther.Nanoseconds;

              // If carry over occured adjust for it
              if( rc.Nanoseconds < 0 )
                {
                  rc.Seconds--;
                  rc.Nanoseconds += 1000000000;
                }

              return ( rc );
            }

          inline
          int operator<( const TimeValue& aOther )
            {
              if(Seconds < aOther.Seconds)
                return 1;
              else if(Seconds ==  aOther.Seconds)
                if(Nanoseconds < aOther.Nanoseconds)
                  return 1;
                else
                  return 0;
              else
                return 0;
            }

          inline
          int operator<=( const TimeValue& aOther )
            {
              if(Seconds < aOther.Seconds)
                return 1;
              else if (Seconds == aOther.Seconds)
                if(Nanoseconds <=  aOther.Nanoseconds)
                  return 1;
                else
                  return 0;
              else
                return 0;
            }

          inline
          int operator>=( const TimeValue& aOther )
            {
              if(Seconds > aOther.Seconds)
                return 1;
              else if (Seconds == aOther.Seconds)
                if(Nanoseconds >=  aOther.Nanoseconds)
                  return 1;
                else
                  return 0;
              else
                return 0;
            }

          inline
          int operator>( const TimeValue& aOther )
            {
              if(Seconds > aOther.Seconds)
                return 1;
              else if(Seconds ==  aOther.Seconds)
                if(Nanoseconds > aOther.Nanoseconds)
                  return 1;
                else
                  return 0;
              else
                return 0;
            }
        };

        static TimeValue mTimeValueBase;
        static double    mTimeBase;


        static inline
        TimeValue
        GetTimeValue()
          {
          TimeValue aTimeValue;
          aTimeValue.Nanoseconds = 0 ;
          aTimeValue.Seconds     = 0 ;

          #if (defined( PK_BGL ) && defined( PK_BLADE_SPI ))

          // NEED TO COME BACK HERE AND RETHINK
          long long timeNow = rts_get_timebase();
          double timeVal = timeNow / 700000000.0;

          aTimeValue.Seconds = (int) timeVal ;
          aTimeValue.Nanoseconds = (int) ( (timeVal - aTimeValue.Seconds)*1000000000.0) ;

//             unsigned long long numberOfTicks = a_GetTimeBase();
//             double timeVal = numberOfTicks / 500000000.0;

//             aTimeValue.Seconds = (int) timeVal ;
//             aTimeValue.Nanoseconds = (int) ( (timeVal - aTimeValue.Seconds)*1000000000.0) ;

          #elif defined( PK_MPI_ALL )

            double timeVal = MPI_Wtime();
            aTimeValue.Seconds = (int) timeVal ;
            aTimeValue.Nanoseconds = (int) ( (timeVal - aTimeValue.Seconds)*1000000000.0) ;

          #elif defined( PK_AIX )
            timebasestruct_t Now;
            read_real_time( &Now, TIMEBASE_SZ );
            time_base_to_time( &Now, TIMEBASE_SZ );
            aTimeValue.Seconds      = Now.tb_high;
            aTimeValue.Nanoseconds  = Now.tb_low ;
          #else
            #if defined(PK_POSIX)
              struct timeval  tv;
              struct timezone tz;
              gettimeofday( &tv, &tz );
              aTimeValue.Seconds     = tv.tv_sec;
              aTimeValue.Nanoseconds = tv.tv_usec * 1000 ;
            #endif
          #endif

          return( aTimeValue );
          }
        static inline
        TimeValue
        ConvertDoubleToTimeValue( double &eTime )
        {
          TimeValue rc;
          rc.Seconds     = ( int )  eTime ;
          rc.Nanoseconds = ( int ) (1000.0 * 1000.0 * 1000.0 * ( eTime - ( 1.0 * rc.Seconds ) ) );

          return rc ;
        }

        static inline
        double
        ConvertTimeValueToDouble( TimeValue &aTimeValue )
          {
          double rc = aTimeValue.Nanoseconds * 0.000000001 + aTimeValue.Seconds;
          return ( rc );
          }

#if 0
        static
        inline
        unsigned long long
        ConvertDoubleToNanoseconds( double eTime )
        {
          unsigned long long nanoseconds = 1000.0 * 1000.0 * 1000.0 * eTime;
          return nanoseconds;
        }
#endif

        static inline
        long long
        GetTimeValueNanoseconds()
        {
          TimeValue now = GetTimeValue();
//          double nowD = ConvertTimeValueToDouble( now );
//          return ConvertDoubleToNanoseconds( nowD );
          long long s= now.Seconds ;
          long long ns= now.Nanoseconds ;
          long long result = s*1000000000LL+ns ;
          return result ;
        }

        static inline
        double
        GetTimeBase()
          {
          return( mTimeBase );
          }

        static inline
        double
        GetElapsedTime()
          {
          Platform::Clock::TimeValue aTimeValue = Platform::Clock::GetTimeValue();
          double rc = ConvertTimeValueToDouble( aTimeValue ) - Platform::Clock::GetTimeBase();
          return(rc);
          }

#if 0
        static inline
        unsigned long long
        GetBladeElapsedTime()
          {
          double ElapsedTime = GetElapsedTime();
          unsigned long long x = ConvertDoubleToNanoseconds( ElapsedTime );
          return( x );
          }
#endif

      };

    class Coprocessor
      {
      public:

      static void
      Init()
        {
        #ifdef PK_BLADE_SPI
          co_co_init();
        #endif
        }

       static
       int
       Start( CO_FuncPtr aFp, void* aArg, int aArgSize )
         {
         #if defined( PK_BGL )
           #if defined( PK_BLADE_SPI )
             return co_co_start( aFp, aArg, aArgSize );
           #elif defined( PK_MPI_ALL )
             // Note:: PMI interfaces need MPI_Init() to be called first.
             return PMI_co_start( aFp, aArg );
           #endif
         #else
           assert( 0 );
           return -1;
         #endif
         }

       static
       void
       Join1( int aHandle )
         {
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

        static void Join( int aHandle )
          {
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
      };

    //********************** Topology ***********************************
    // Get the logical address space id - that is, an index into the route table.
    class Topology
      {
      public:

        static int mSubPartitionCount;

        static int mSubPartitionId;

        static int xSize_cmdprompt;
        static int ySize_cmdprompt;
        static int zSize_cmdprompt;

        static int mVoxDimx;
        static int mVoxDimy;
        static int mVoxDimz;

	static int mfftProcDimx;
        static int mfftProcDimy;
        static int mfftProcDimz;

        static int ThisAddressSpaceId;
        static int Size;

        static int mDimX;
        static int mDimY;
        static int mDimZ;

        static int mMachineDimX;
        static int mMachineDimY;
        static int mMachineDimZ;

        enum{ MAX_DIMENSIONALITY = 512 };  // Maximum logical dimensionality supported.

          // class Group {};  // Group of address space ids.  We support this in concept.

#ifdef PK_BLADE_SPI
        static GroupContainer mGroup;
#endif

        typedef int AddressSpaceId;

        // Stateless calls to virtualiziation layer.


        // CREATES THE TOPOLOGY FOR RUNNING THE BE-INSTANCES
#if defined( PK_MPI_ALL )
        static
        void
        InitCommunicator()
          {
          int xDim, yDim, zDim;
          GetMachineDimensions( &xDim, &yDim, &zDim );
          int MachineDims[ 3 ] = { xDim, yDim, zDim };
          int periods[ 3 ]   = { 0, 0, 0 };
          int reorder = 0;

          MPI_Comm WorldCartComm;

          MPI_Cart_create( MPI_COMM_WORLD, 3, MachineDims,
                           periods, reorder, &WorldCartComm );


          int nOrbs = (int) (log((double) mSubPartitionCount ) / log(2.) + 0.5);
          assert(nOrbs>=0);

          int xCuts = ( nOrbs / 3 );
          int yCuts = ( nOrbs / 3 );
          int zCuts = ( nOrbs / 3 );

          if (nOrbs % 3 == 1)
            {
            if( xDim > zDim )
              xCuts++;
            else
              zCuts++;
            }
          else if( nOrbs % 3 == 2 )
            {
            yCuts++;
            if( xDim > zDim )
              xCuts++;
            else
              zCuts++;
            }

          int xIncr = xDim / (double) pow( 2., xCuts );
          int yIncr = yDim / (double) pow( 2., yCuts );
          int zIncr = zDim / (double) pow( 2., zCuts );

          assert( xIncr >= 1 && yIncr >= 1 && zIncr >= 1 );

          // fill in the ranks within the rectangle of procs

          int MachineSize;
          MPI_Comm_size( MPI_COMM_WORLD, & MachineSize );

          int* rank2GroupId = new int[ MachineSize ];
          if( rank2GroupId == NULL )
            PLATFORM_ABORT("rank2GroupId == NULL");

          int** procRanks = new int*[ mSubPartitionCount ];
          if( procRanks == NULL )
            PLATFORM_ABORT("procRanks == NULL");

          for( int i = 0; i < mSubPartitionCount; i++ )
            {
            procRanks[ i ] = new int[ MachineSize ];
            if( procRanks[ i ] == NULL )
              PLATFORM_ABORT("procRanks[ i ] == NULL");
            }

          int* procSize = new int[ mSubPartitionCount ];
          if( procSize == NULL )
            PLATFORM_ABORT("procSize == NULL");

          int* subCartDims[ 3 ];
          for( int i = 0; i < 3; i++ )
            {
            subCartDims[ i ] = new int[ mSubPartitionCount ];

            if( subCartDims[ i ] == NULL )
              PLATFORM_ABORT("subCartDims[ i ] == NULL");
            }

          int groupID = -1;
          for(int i = 0; i < (int) pow( 2., xCuts ); i++)
            for(int j = 0; j < (int) pow( 2., yCuts ); j++)
              for(int k = 0; k < (int) pow( 2., zCuts ); k++)
                {
                groupID++;
                int counter=0;

                for(int ii = xIncr * i; ii < xIncr * (i+1); ii++)
                  for(int jj = yIncr * j; jj < yIncr * (j+1); jj++)
                    for(int kk = zIncr * k; kk < zIncr * (k+1); kk++)
                      {
                      int coords[ 3 ];
                      coords[ 0 ] = ii;
                      coords[ 1 ] = jj;
                      coords[ 2 ] = kk;

                      int rank = -1;
                      MPI_Cart_rank( WorldCartComm, &coords[ 0 ], &rank );

                      rank2GroupId[ rank ] = groupID;
                      procRanks[ groupID ][ counter ] = rank;
                      counter++;
                      }

                procSize[ groupID ] = counter;
                subCartDims[ 0 ][ groupID ] = xIncr;
                subCartDims[ 1 ][ groupID ] = yIncr;
                subCartDims[ 2 ][ groupID ] = zIncr;


                // printf( "groupID: %d\n", groupID );
                }

          int MyGlobalRank;
          MPI_Comm_rank( MPI_COMM_WORLD, &MyGlobalRank );

          assert( MyGlobalRank >= 0 && MyGlobalRank < MachineSize );
          int MyGroupId = rank2GroupId[ MyGlobalRank ];
          mSubPartitionId = MyGroupId;

          assert( MyGroupId >= 0 && MyGroupId < mSubPartitionCount );

          // Set partition dimension
          mDimX = subCartDims[ 0 ][ MyGroupId ];
          mDimY = subCartDims[ 1 ][ MyGroupId ];
          mDimZ = subCartDims[ 2 ][ MyGroupId ];

          MPI_Group NewGroup;
          MPI_Group WorldGroup;

          MPI_Comm_group( MPI_COMM_WORLD, &WorldGroup );

          MPI_Group_incl( WorldGroup,
                          procSize[ MyGroupId ],
                          procRanks[ MyGroupId ],
                          &NewGroup );

          MPI_Comm NewComm;
          MPI_Comm_create( MPI_COMM_WORLD, NewGroup, &NewComm );

          int PartitionDims[ 3 ] = { mDimX, mDimY, mDimZ };

          MPI_Cart_create( NewComm, 3, PartitionDims,
                           periods, reorder, &cart_comm );

          MPI_Comm_rank( cart_comm, &ThisAddressSpaceId );

          Size = mDimX * mDimY * mDimZ;

//           printf( "Size: %d ThisAddressSpaceId: %d dims { %d %d %d } cuts { %d %d %d } incr { %d %d %d }\n",
//                   Size, ThisAddressSpaceId,
//                   mDimX, mDimY, mDimZ,
//                   xCuts, yCuts, zCuts,
//                   xIncr, yIncr, zIncr );

          // Free memory
          delete [] rank2GroupId;

          for( int i = 0; i < mSubPartitionCount; i++ )
            {
            delete [] procRanks[ i ];
            }
          delete [] procRanks;

          delete [] procSize;

          for( int i = 0; i < 3; i++ )
            delete [] subCartDims[ i ];

          MPI_Barrier( MPI_COMM_WORLD );
          }
#endif

        static void Init( int* argc, char **argv[] )
          {
#if defined(PK_BLADE_SPI)
          int all_planes[ 3 ] = { 1, 1, 1 };
          int xDim, yDim, zDim;
          GetMachineDimensions( &xDim, &yDim, &zDim );

          Size = xDim * yDim * zDim;

          mGroup.Init( all_planes, xDim, yDim, zDim );

          ThisAddressSpaceId = mGroup.mMyRank;

          // Set partition dimensions
          mDimX = xDim;
          mDimY = yDim;
          mDimZ = zDim;

#elif defined( PK_MPI_ALL )
          // Figure out the dimensions of the machine
          // based on the partition size
          MPI_Init( argc, argv );

          InitCommunicator();
#endif
          }

#if defined(PK_BLADE_SPI)
        static GroupContainer* GetGroup()
          {
          return &mGroup;
          }
#endif

        static inline AddressSpaceId
        GetAddressSpaceAtCoord( int   aDimensionality, // number of dimensions
                                int * aExtent,         // extent in each dimension
                                int * aCoordVector )   // coords
          {
          return(0);
          }

        static inline int
        GetCoordOfAddressSpace( AddressSpaceId   aAddressSpaceId,
                                int              aDimensionality,
                                int            * aExtent,
                                int            * aCoordVector     )
          {
          return(0);
          }

        // OR... ..... .... .... ... ..  .... .. .. ... ..... ...

        // Might be more efficient to have a setup phase:

        // Structure to hold the dimensionality and the extent in each each dimension.
        // A context is virtual mapping of a perfect grid machine.
        class Context
          {
          public:
            int mDimensionality;
            int mExtent[ MAX_DIMENSIONALITY ];
            //Machine dependant member vars (maybe).
          };

        // Setup the context
        static inline int
        SetupContext( int      aDimensionality, // Number of dimensions
                      int     *aExtent,   // Extent in each dimension - must fit in partition size.
                      Context &aContext ) // reference to an area for returned context
          {
          aContext.mDimensionality = aDimensionality;
          for(int i = 0; i < aDimensionality; i++ )
            aContext.mExtent[ i ] = aExtent[i];
          // Machine dependant code to figure out mapping
          return(0);
          }

        // Given a context and a coord vector, return an AddressSpaceId
        static inline AddressSpaceId
        GetAddressSpaceAtCoord( Context        &aContext,
                                int            *aCoordVector,
                                AddressSpaceId &aAddressSpaceId )
          {
          return(0);
          }

        // Given a context, and an address space id, return the coords.
        static inline int
        GetCoordOfAddressSpace( Context         &aContext,
                               AddressSpaceId  aAddressSpaceId,
                               int            *aCoordVector   ) // return
          {
          return(0);
          }

        // This will be -1 until it is set - it is questionable how useful this will be, but there
        // is the $(DEITY) awful chance that static constructors will call this GetAddressSpaceId before
        // the environment has set it up.  Maybe we should assert in this case???
        #if  defined( PK_MPI_ALL )
        static MPI_Comm cart_comm;
        #endif

        static inline
        int
        GetAddressSpaceId()
          {
          return( ThisAddressSpaceId );
          }

        // Find where we are within a chip. Intended for BG/L, where we want
        // to know whether to come in at 'main' or be a helper
        static inline
        int
        GetLocationInASIC()
          {
#if PK_SPI
          return rts_get_processor_id();
#else
          return 0 ;
#endif
          }

        static inline
        int
        GetSize()
          {
          return( Size );
          }

        static inline void xyzFromProcCount(int procs, int &x, int &y, int &z)
        {
          // Find full space power of 2

          // As long as x >= y >= z the orb will create the correct partitioning.
          // y >= 1/2 x && z >= 1/2 y

          int d = 3;
          int levels = 0;
          int t;
          do
            {
              levels++;
              t = 1 << (levels*d);
            }while (procs > t);
          int base = (levels-1)*d;
          while(d > 0)
            {
              t = 1 << (base + d);
              if( procs == t ) break;
              d--;
            }

          assert(d>0);

//           switch(d)
//             {
//             case 3:
//               x = 1 << (base + 1);
//               y = 1 << (base + 1);
//               z = 1 << (base + 1);
//               break;
//             case 2:
//               x = 1 << (base + 1);
//               y = 1 << (base + 1);
//               z = 1 << base;
//               break;
//             case 1:
//               x = 1 << (base + 1);
//               y = 1 << base;
//               z = 1 << base;
//               break;
//             }
          switch( d )
            {
            case 3:
              x = 1 << levels;
              y = 1 << levels;
              z = 1 << levels;
              break;
            case 2:
              x = 1 << levels;
              y = 1 << levels;
              z = 1 << levels-1;
              break;
            case 1:
              x = 1 << levels;
              y = 1 << levels-1;
              z = 1 << levels-1;
              break;
            }
        }

        static
        inline
        int
        MakeRankFromCoords( int x, int y, int z )
        {
         #ifndef NDEBUG
           int sizeX, sizeY, sizeZ;
           GetDimensions( &sizeX, &sizeY, &sizeZ );
           assert( x >= 0 && x < sizeX );
           assert( y >= 0 && y < sizeY );
           assert( z >= 0 && z < sizeZ );
         #endif

#if defined( PK_BLADE_SPI )
           // return BGLPartitionMakeRank( x, y, z );
          int coords[3];
          coords[ 0 ] = x;
          coords[ 1 ] = y;
          coords[ 2 ] = z;

          return mGroup.GetRankFromCoord( coords );
#elif defined( PK_MPI_ALL )
          int coords[3];
          coords[ 0 ] = x;
          coords[ 1 ] = y;
          coords[ 2 ] = z;
          int rank;

          MPI_Cart_rank(cart_comm, coords, &rank);
          return rank;
#else
         return x;
#endif
        }

        static
        inline
        void
        GetCoordsFromRank( int rank, int* x, int* y, int* z )
        {
#if defined( PK_BLADE_SPI )
          // BGLPartitionMakeXYZ( rank, x, y, z );
            TorusXYZ Coords = mGroup.GetCoordFromRank( rank );
            *x = Coords.mX;
            *y = Coords.mY;
            *z = Coords.mZ;
#elif defined( PK_MPI_ALL )
          int coords[ 3 ], myRank;
          MPI_Cart_coords( cart_comm, rank, 3, coords);
          *x = coords[ 0 ];
          *y = coords[ 1 ];
          *z = coords[ 2 ];
#else
          *x=1;
          *y=1;
          *z=1;
#endif
        }

        // This call is used by the application configurator
        // to attempt to best map the application topology
        // to the machine topology.
        static
        inline
        void
        GetMyCoords( int* procsX, int* procsY, int* procsZ )
        {
#if defined( PK_BLADE_SPI )
        mGroup.GetMyCoords( procsX, procsY, procsZ );
        // BGLPartitionGetCoords( procsX, procsY, procsZ );
        // BGLPartitionMakeZYX( BGLPartitionGetRank(), procsX, procsY, procsZ );
#elif defined( PK_MPI_ALL )
          int coords[ 3 ], myRank;
          MPI_Cart_coords( cart_comm, ThisAddressSpaceId, 3, coords);
          *procsX = coords[ 0 ];
          *procsY = coords[ 1 ];
          *procsZ = coords[ 2 ];
#elif defined( PK_BGL )
          return GetCoordsFromRank( ThisAddressSpaceId, procsX, procsY, procsZ );
#else
          *procsX = 0;
          *procsY = 0;
          *procsZ = 0;
#endif
        }

        static
        inline
        void
        GetMachineDimensions( int* procsX, int* procsY, int* procsZ )
          {
#if defined( PK_BLADE_SPI )
        BGLPartitionGetDimensions( procsX, procsY, procsZ );
#elif defined( PK_MPI_ALL )

#ifndef PK_BGL
        int s;
        MPI_Comm_size(MPI_COMM_WORLD, &s );
        // MPI_Comm_size( cart_comm, &s );

            if( xSize_cmdprompt > 0 )  // then use it
              {
                *procsX = xSize_cmdprompt;
                *procsY = ySize_cmdprompt;
                *procsZ = zSize_cmdprompt;
              }
            else
              {
              xyzFromProcCount( s, *procsX, *procsY, *procsZ );
              }
#else
            if( xSize_cmdprompt > 0 )  // then use it
              {
                *procsX = xSize_cmdprompt;
                *procsY = ySize_cmdprompt;
                *procsZ = zSize_cmdprompt;
              }
            else
              {
              BGLPersonality personality;
              rts_get_personality( &personality, sizeof(personality) );
              *procsX = personality.xSize;
              *procsY = personality.ySize;
              *procsZ = personality.zSize;
              }
#endif
#else
          *procsX = 1;
          *procsY = 1;
          *procsZ = 1;
#endif
          }

        static
        inline
        void
        GetDimensions( int* procsX, int* procsY, int* procsZ )
          {
          *procsX = mDimX;
          *procsY = mDimY;
          *procsZ = mDimZ;
          }

        // This is mostly a statement about the distance between nodes.
        static inline
        int
        GetPhysicalDescription( int &AddressSpaceCount,
                                int &AddressSpaceConnectivity,
                                int &PhysicalDimensionality )
          {
#ifdef PK_BLADE_SPI
          int xNodes, yNodes, zNodes;
#ifdef PK_PARALLEL
          BGLPartitionGetDimensions( &xNodes, &yNodes, &zNodes );
#else
          // If we're not running parallel, we are '1x1x1'
          xNodes = 1 ;
          yNodes = 1 ;
          zNodes = 1 ;
#endif
          // MPI_Comm_size(MPI_COMM_WORLD, &s );
          AddressSpaceCount = xNodes * yNodes * zNodes;
          AddressSpaceConnectivity = AddressSpaceCount - 1;
          PhysicalDimensionality = 0;
#else
#ifdef PK_MPI_ALL
          int s;
          MPI_Comm_size(cart_comm, &s );
          AddressSpaceCount = s;
          AddressSpaceConnectivity = s - 1;
          PhysicalDimensionality = 0;
#else
          // Answers for a single address space partition.
          AddressSpaceCount = 1;
          AddressSpaceConnectivity = 0;
          PhysicalDimensionality = 0;
#endif
#endif
          return(0);
          }

        // Cheep little call (only use when dimensionality > 0 )
        // used to get the extent of grid space in each dimension.
        static inline int
        GetPhysicalExtentInDimension( int DimensionNumber )
          {
          //SP pops an assert on this.
          return(0);
          }

        static inline
        int
        GetAddressSpaceCount()
          {
#if 0
          int AddressSpaceCount;
          int AddressSpaceConnectivity;
          int PhysicalDimensionality;
          GetPhysicalDescription( AddressSpaceCount,
                                  AddressSpaceConnectivity,
                                  PhysicalDimensionality );
          return( AddressSpaceCount );
#endif
          return ( Size );
          }

      };


    //**************** Functional Logging *******************
    class FxLogger
      {
      public:
        static int LogFd;
        static Mutex::MutexWord LogLockVar;

        static int OpenLogPort();

        static inline
        void
        WriteBufferToLogPort( char *aBuffer, int aLen )
          {
              // BGL bringup supports 'stdio' to console but not much else
#if defined( PKFXLOG_TO_STDOUT )
            int Lines = aLen / 256 ;
            int Remainder = aLen % 256 ;
            for (int Line=0;Line < Lines ; Line += 1)
            {
               PRINTF_FUNCTION("%.256s",aBuffer+256*Line) ;
            } /* endfor */
            if (Remainder > 0 )
            {
              PRINTF_FUNCTION("%.*s",Remainder,aBuffer+256*Lines) ;
            } /* endif */
//            for ( int x=0;x<aLen;x+=1)
//              {
////              putchar(aBuffer[x]) ;
//                PRINTF_FUNCTION("%c",aBuffer[x]) ; // slow, but portable across different libc's ...
//              }
#else

          // This is the business end of the logging
          if( LogFd <= 0 )    // Questionable attempt to open logging on first use.
            OpenLogPort();

          if( LogFd < 0 )
            PLATFORM_ABORT( "Could not open pkfxlog file" );

          int write_rc = ::write( LogFd, aBuffer, aLen);
          if( write_rc != aLen )
            PLATFORM_ABORT( "Failed to complete write to pkfxlog.");
#endif
          }
      };

      //****************** Collective *****************************

      class Collective
      {
          static int* A2ASendCount;
          static int* A2ASendOffset;
          static int* A2ARecvCount;
          static int* A2ARecvOffset;

          static int* SendNodeSet;
          static int  SendNodeSetCount;
          static int* RecvNodeSet;
          static int  RecvNodeSetCount;

          static int* BcastSendNodeSet;
          static int  BcastSendNodeSetCount;
          static int* BcastRecvNodeSet;
          static int  BcastRecvNodeSetCount;

          static int* ReduceSendNodeSet;
          static int  ReduceSendNodeSetCount;
          static int* ReduceRecvNodeSet;
          static int  ReduceRecvNodeSetCount;

          public:
          static
          void
          Init( )
            {
            A2ASendCount = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( A2ASendCount == NULL )
                PLATFORM_ABORT( "A2ASendCount == NULL" );

            A2ASendOffset = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( A2ASendOffset == NULL )
                PLATFORM_ABORT( "A2ASendOffset == NULL" );

            A2ARecvCount = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( A2ARecvCount == NULL )
                PLATFORM_ABORT( "A2ARecvCount == NULL" );

            A2ARecvOffset = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( A2ARecvOffset == NULL )
                PLATFORM_ABORT( "A2ARecvOffset == NULL" );

            SendNodeSet = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( SendNodeSet == NULL )
                PLATFORM_ABORT( "SendNodeSet == NULL" );

            RecvNodeSet = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( RecvNodeSet == NULL )
                PLATFORM_ABORT( "RecvNodeSet == NULL" );

            BcastSendNodeSet = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( BcastSendNodeSet == NULL )
              PLATFORM_ABORT( "BcastSendNodeSet == NULL" );

            BcastRecvNodeSet = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( BcastRecvNodeSet == NULL )
                PLATFORM_ABORT( "BcastRecvNodeSet == NULL" );

            ReduceSendNodeSet = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( ReduceSendNodeSet == NULL )
                PLATFORM_ABORT( "ReduceSendNodeSet == NULL" );

            ReduceRecvNodeSet = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
            if( ReduceRecvNodeSet == NULL )
                PLATFORM_ABORT( "ReduceRecvNodeSet == NULL" );
            }

          static
          void
          GlobalMax( unsigned long* aIn, unsigned long* aOut )
            {
#if defined( PK_MPI_ALL )
            MPI_Allreduce( aIn, aOut, 1, MPI_LONG, MPI_MAX, Topology::cart_comm );
#elif defined( PK_BLADE_SPI )
            GlobalTreeReduce( aIn, aOut, sizeof( unsigned long ), _BGL_TREE_OPCODE_MAX );
#else
            assert( 0 );
#endif
            }

          static
          void
          GlobalOR( unsigned long* aIn, unsigned long* aOut )
            {
#if defined( PK_MPI_ALL )
            MPI_Allreduce( aIn, aOut, 1, MPI_LONG, MPI_BOR, Topology::cart_comm );
#elif defined( PK_BLADE_SPI )
            GlobalTreeReduce( aIn, aOut, sizeof( unsigned long ), _BGL_TREE_OPCODE_OR );
#else
            assert( 0 );
#endif
            }

          static
          void
          Alltoallv( void* aSendBuff, int * aSendCount, int* aSendOffsets, unsigned aSendTypeSize,
                     void* aRecvBuff, int * aRecvCount, int* aRecvOffsets, unsigned aRecvTypeSize,
                     int debug=0, int RecvCountOverposted=0, int aBuffers32ByteAligned=1,
                     int aUseCacheListFromLastCall=0, int aCommType=0 )
              {
//              	BegLogLine(PKFXLOG_COLLECTIVE)
//              	 << "Alltoallv aSendBuff=" << aSendBuff
//              	 << " aSendTypeSize=" << aSendTypeSize
//              	 << " RecvCountOverposted=" << RecvCountOverposted
//              	 << " aBuffers32ByteAligned=" << aBuffers32ByteAligned
//              	 << EndLogLine ;
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
                      BcastSendNodeSetCount++;
                      }

                    if ( aRecvCount[ i ] != 0 )
                      {
                      BcastRecvNodeSet[ BcastRecvNodeSetCount ] = i;
                      BcastRecvNodeSetCount++;
                      }
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
                    if ( aSendCount[ i ] != 0 )
                      {
                      ReduceSendNodeSet[ ReduceSendNodeSetCount ] = i;
                      ReduceSendNodeSetCount++;
                      }

                    if ( aRecvCount[ i ] != 0 )
                      {
                      ReduceRecvNodeSet[ ReduceRecvNodeSetCount ] = i;
                      ReduceRecvNodeSetCount++;
                      }
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
                  if ( aSendCount[ i ] != 0 )
                    {
                    SendNodeSet[ SendNodeSetCount ] = i;
                    SendNodeSetCount++;
                    }

                  if ( aRecvCount[ i ] != 0 )
                    {
                    RecvNodeSet[ RecvNodeSetCount ] = i;
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



              }

          static
          void
          BroadcastBlock( char* aSource, int aNumberOfBytes, int aRoot )
            {

#ifdef PK_BLADE_SPI
            if( Platform::Topology::GetAddressSpaceId() != aRoot )
              {
              bzero( aSource, aNumberOfBytes );
              }

            GlobalTreeReduce( (unsigned long *)aSource, (unsigned long *)aSource, aNumberOfBytes, _BGL_TREE_OPCODE_OR );
#endif
            }

          static
          void
          Broadcast( char* aSource, int aNumberOfBytes, int aRoot )
          {
          #ifdef PK_BLADE_SPI
            // int S4K = 64 * 1024;
            int S4K = 16 * 1024;
            int NumberOf64KBlocks = aNumberOfBytes / S4K;
            int BytesLeft         = aNumberOfBytes % S4K;

            for( int i=0; i<NumberOf64KBlocks; i++ )
              {
              BroadcastBlock( &aSource[ i * S4K ], S4K, aRoot );
              }

            if( BytesLeft )
              {
              BroadcastBlock( &aSource[  NumberOf64KBlocks* S4K ], BytesLeft, aRoot );
              }
          #endif

          #ifdef PK_MPI_ALL
            MPI_Bcast( aSource, aNumberOfBytes, MPI_CHAR, aRoot, Topology::cart_comm );
          #endif
          }

      static
      void
      Init( int FullPartitionClass, int IntegerReductionClass )
        {
        }

      static
      void
      AllReduce_Sum( char* aSourceTarget, int aLength )
        {
        #ifdef PK_BLADE_SPI
          assert( 0 );
        #endif
        }

      static
      void
      FP_AllReduce_Sum( double* aSource, double* aTarget, int aNumberOfDoubles )
        {
        #ifdef PK_MPI_ALL
          MPI_Allreduce( aSource, aTarget , aNumberOfDoubles , MPI_DOUBLE, MPI_SUM, Topology::cart_comm );
        #endif

        #ifdef PK_BLADE_SPI
          assert( 0 );
        #endif
        }

      static
      void
      LONGLONG_Reduce_Sum( long long* aSource, long long* aTarget, int aNumberOfLongLongs, int aRoot=0 )
        {
        #ifdef PK_BLADE_SPI
        GlobalTreeReduce64( aSource, aTarget,
                            aNumberOfLongLongs * sizeof( long long ),
                            _BGL_TREE_OPCODE_ADD );
//        #else
//          MPI_Reduce( aSource, aTarget , aNumberOfLongLongs , MPI_LONG_LONG , MPI_SUM, aRoot, Topology::cart_comm );
        #endif
        
        #ifdef PK_MPI_ALL
          MPI_Reduce( aSource, aTarget , aNumberOfLongLongs , MPI_LONG_LONG , MPI_SUM, aRoot, Topology::cart_comm );
        #endif
        }

      static
      void
      FP_Reduce_Sum( double* aSource, double* aTarget, int aNumberOfDoubles, int aRoot=0 )
        {
        #ifdef PK_MPI_ALL
          MPI_Reduce( aSource, aTarget , aNumberOfDoubles , MPI_DOUBLE, MPI_SUM, aRoot, Topology::cart_comm );
        #endif

        #ifdef PK_BLADE_SPI
          // Need to scale doubles to unsigned long longs
          long long* Source = (long long *) malloc( sizeof( long long ) * aNumberOfDoubles );
          if( Source == NULL )
            PLATFORM_ABORT( "Source == NULL" );

          long long* Target = (long long *) malloc( sizeof( long long ) * aNumberOfDoubles );
          if( Target == NULL )
            PLATFORM_ABORT( "Target == NULL" );

// 	  printf( "S: %08X T: %08X ND: %d\n", Source, Target, aNumberOfDoubles );
// 	  fflush( stdout );

          double SCALING_FACTOR = 1E12;

          for( int i = 0; i < aNumberOfDoubles; i ++ )
            {
            Source[ i ] = (long long) (aSource[ i ] * SCALING_FACTOR);
            }

          GlobalTreeReduce64( Source, Target,
                              aNumberOfDoubles * sizeof( long long ),
                              _BGL_TREE_OPCODE_ADD );

          for( int i = 0; i < aNumberOfDoubles; i ++ )
            {
            aTarget[ i ] = (double) (Target[ i ] / SCALING_FACTOR );
            }

// 	  printf( "S: %08X T: %08X ND: %d\n", Source, Target, aNumberOfDoubles );
// 	  fflush( stdout );

          free( Source );
          free( Target );
        #endif
        }

      static
      void
      INT_AllReduce_Sum( int* aSource, int* aTarget, int aNumberOfIntegers )
        {
        #ifdef PK_MPI_ALL
          MPI_Allreduce( aSource, aTarget , aNumberOfIntegers , MPI_INT, MPI_SUM, Topology::cart_comm );
        #endif

        #ifdef PK_BLADE_SPI
          GlobalTreeReduce( (unsigned long*) aSource, (unsigned long*) aTarget,
                            aNumberOfIntegers * sizeof( unsigned long ),
                            _BGL_TREE_OPCODE_ADD );
        #endif
        }
      static
      void
      LONGLONG_AllReduce_Sum( int* aSource, int* aTarget, int aNumberOfIntegers )
        {
        #ifdef PK_MPI_ALL
            PLATFORM_ABORT( "FATAL ERROR: No such thing as 'long long int' for MPI reduce");
//          MPI_Allreduce( aSource, aTarget , aNumberOfIntegers , MPI_LONGLONG, MPI_SUM, Topology::cart_comm );
        #endif

        #ifdef PK_BLADE_SPI
          GlobalTreeReduce64( (long long*) aSource, (long long*) aTarget,
                            aNumberOfIntegers * sizeof(long long),
                            _BGL_TREE_OPCODE_ADD );
        #endif
        }
      };

    //**************** External Datagram Interface *******************

    // Currently this only shall enable "sendto" to route datagrams
    // to an anticipated UDP/IP based server set.

    // This might use internal or external routing.  That is,
    // we might just use the active meessage system to route
    // packets to a single task that serves as a port, most likely
    // for writing a single, unordered file of datagrams.

    class ExternalDatagramInterface
      {
      public:
        static Lock::LockWord RdgLockVar;

        typedef int Address;  // for now a file or socket handle.

        static Address mAddress;
        static int     mRDG_Ext;

#if !defined(RDG_TO_FXLOGGER)

        // NOTE : YOU MUST BE IN A CRIT SECT TO DO THIS CALL
        // NOTE : YOU MUST BE IN A CRIT SECT TO DO THIS CALL
        // NOTE : YOU MUST BE IN A CRIT SECT TO DO THIS CALL
        static void
        OpenRDGFile( char * aPathName )
          {
          int open_perms = 0660 ;
          // printf("Entering OpenRDGFile\n");

#ifndef PK_PARALLEL
          mAddress = open( aPathName ,
                           O_CREAT | O_TRUNC | O_WRONLY | O_APPEND,  // O_APPEND Required to ensure proper writes by threaded apps
                           open_perms );
#else
          if( Platform::Topology::GetAddressSpaceId() == 0 )
            {
                mAddress = open( aPathName ,
                                 O_CREAT | O_TRUNC | O_WRONLY | O_APPEND,  // O_APPEND Required to ensure proper writes by threaded apps
                                 open_perms );
                Platform::Control::Barrier();         //Wait for all processes to get here.
            }
          else
            {
            Platform::Control::Barrier();         //Wait for all processes to get here.
            mAddress = open( aPathName ,
                          O_WRONLY | O_APPEND,  // O_APPEND Required to ensure proper writes by threaded apps
                          open_perms );
            }
#endif
          if( mAddress < 0 )  // Log file failed open.
            PLATFORM_ABORT( "FATAL ERROR: Failed to open Raw Datagram Output port");

          // printf("Leaving OpenRDGFile\n");
          }

        static
        void
        ReopenRawDataPort()
          {
          Mutex::YieldLock( &RdgLockVar );

          if( mAddress < 0 )  // RDG file MUST be opened.
            PLATFORM_ABORT( "FATAL ERROR: Cannot repopen RDG file that hasn't been opened");

          close( mAddress );

          Platform::Control::Barrier();         //Wait for all processes to get here.

          char buffer[4096];

          mRDG_Ext++;
          sprintf( buffer, "RawDatagramOutput.%d", mRDG_Ext );

          OpenRDGFile( buffer );

          Mutex::Unlock( &RdgLockVar );
          }

        enum{ HEADER_SIZE = 2 * sizeof( int ) };
        enum{ PAYLOAD_SIZE = Platform::Reactor::PAYLOAD_SIZE - HEADER_SIZE };

        typedef struct
          {
          int  mLen;
          int  mAddress;
          char mData[ PAYLOAD_SIZE ];
          } SendTo_ActorPacket;

        static inline
        void
        SendTo_BaseFx( Address aDummy, void * aBuffer, int aLen )
          {

          // If this whole routine is handled as a critical section, we can avoid opening the RDG O_APPEND
          // Note that since this method is called from an active message, it will stall the active message
          // handler and thus back up the network.

          Mutex::YieldLock( &RdgLockVar );

          char *buffer = (char *) aBuffer;
          // This is the business end of the logging
          if( mAddress <= 0 )    // Questionable attempt to open logging on first use.
            OpenRDGFile( "RawDatagramOutput" );

          assert( mAddress > 0 );
          int write_rc = ::write( mAddress, buffer, aLen);

          // The follow hack-block should be located in the BlueMatter monitor - but this'll get mike going.
          if( write_rc != aLen )
            {
            int bytes_left;
            char username[1024];
            char hostname[1024];
            if( write_rc < aLen && write_rc > 0 )
              bytes_left = aLen - write_rc;
            else
              bytes_left = aLen;
            strncpy( username, getlogin(), sizeof(username) );
            gethostname( hostname, sizeof(hostname) );
            sleep(10);
            int write_rc2 = ::write( mAddress, &( buffer[ aLen - bytes_left ] ), bytes_left );
            if(  write_rc2 != bytes_left )
              {
              perror("WARNING: Trouble writing rdg - errno is ");
              fprintf( stderr,
                       "WARNING: Trouble writing rdg - error is %s\n",
                       GetWriteErrorMessage() );
              char mailbuf[2048];
              sprintf(  mailbuf,
                       "echo 'Trouble on %s pid(%d) writing RawDatagramOuput \nProblem reported is >%s< ' | mail -s BlueMatterRunStalled %s",
                        hostname,
                        getpid(),
                        GetWriteErrorMessage(),
                        username );
              fprintf( stderr, "WARNING: Doing the following *>%s<* ", mailbuf);
              fflush(stderr);
              system( mailbuf );
              while( write_rc2 != bytes_left )
                {
                if( write_rc2 < bytes_left && write_rc2 > 0 )
                  bytes_left -= write_rc2;
                sleep( 60 );
                write_rc2 = ::write( mAddress, &(buffer[ aLen - bytes_left ]), bytes_left );
                }
              fprintf( stderr, "Fixed trouble writing rdg " );
              sprintf(  mailbuf,
                       "echo 'Trouble fixed on %s pid(%d) writing RawDatagramOuput ' | mail -s BlueMatterRunResumed %s",
                        hostname,
                        getpid(),
                        username );
              fprintf( stderr, "doing the following *>%s<* ", mailbuf );
              fflush( stderr );
              system( mailbuf );
              }
            }

          // End critical section
          Mutex::Unlock( &RdgLockVar );

          }

        static
        int
        SendTo_ActorFx( void * data )
          {
          SendTo_ActorPacket *stap = (SendTo_ActorPacket *) data;
          SendTo_BaseFx( stap->mAddress, stap->mData, stap->mLen );
          return( Platform::Reactor::RETURN_OK );
          }

        static inline
        void
        SendTo( Address aDummy, void *aBuffer, int aLen )
          {
            SendTo_BaseFx( aDummy, aBuffer, aLen );
#if 0
          // Short circut if we are on node zero already.
          if( Platform::Topology::GetAddressSpaceId() == 0 )
            {
            SendTo_BaseFx( aDummy, aBuffer, aLen );
            }
          else
            {
            // THIS IS ON THE STACK - MIGHT CAUSE CYCLOPS PROBLEMS.
            // ALSO, This is another DATA COPY that could be avoided by letting app build packet directly.
            // Further, the app may already have these values in named storage that is in registers.
            // ALL THIS IS HORRIBLE... NEED TO CLEAN UP BEFORE INTERFACE IS TO WIDELY USED!!!!!!!!!!!!!
            // Oh yeah... don't forget, for very large machines, this is creating a hotspot, but should be good for now.
            SendTo_ActorPacket stap;
            stap.mAddress = aDummy;
            stap.mLen     = aLen;
            assert( aLen <= PAYLOAD_SIZE );

            memcpy( stap.mData, aBuffer, aLen );  // $(ALIGNMENT) <insert_deity_of_your_choice>!

            Platform::Reactor::Trigger( 0,                           // at address space id 0
                                        SendTo_ActorFx,
                                        (void *) &stap,
                                        HEADER_SIZE + aLen ); // send as little as possible.
            }
#endif
          }
#else
        enum {
           kBufferSize = 100000
           } ;
//      static char Buffer[kBufferSize] ;
//      static unsigned int BufferIndex ;

        static inline
        void
        OpenRawDataPort() { } ;

// In BGL simulator bringup, we put 'datagrams' into a buffer for examining later
// Also attempt to drive them through the 'console 'interface

        static inline
        void SendTo_BaseFx( Address aDummy, void * aBuffer, int aLen )
        {
        for( int i=0; i <  Platform::Topology::GetSize(); i++ )
          {
          if( i == Platform::Topology::GetAddressSpaceId() )
            SendTo_BaseFx_Uni(  aDummy, aBuffer, aLen );
          // PRINTF_FUNCTION("STBFX: Before barrier\n");
          Platform::Control::Barrier();
          // PRINTF_FUNCTION("STBFX: After barrier\n");
          }
        } ;

        static inline
        void
        SendTo_BaseFx_Uni( Address aDummy, void * aBuffer, int aLen )
          {
            if( aLen <= 0 )
              return;

             char HexBuffer[64] ;
             static int seq_num = 0;
//           int Available = kBufferSize - BufferIndex ;
//           int Actual = ( aLen < Available ) ? aLen : Available ;
//           memcpy(Buffer+BufferIndex,aBuffer,Actual) ;
//           BufferIndex += Actual ;

             char * BufChar = (char *) aBuffer ;
             int FullLines = aLen / 32 ;
             for (int LineIndex=0 ; LineIndex < FullLines; LineIndex += 1)
             {
                for (int CharIndex=0; CharIndex < 32; CharIndex += 1)
                {
                   int xByte = BufChar[32*LineIndex+CharIndex] ;
                   HexBuffer[CharIndex*2  ] = HexToChar[(xByte >> 4) & 0x0f ] ;
                   HexBuffer[CharIndex*2+1] = HexToChar[ xByte       & 0x0f ] ;
                } /* endfor */
                PRINTF_FUNCTION("STBFX %d %d SendTo_BaseFx >%.64s<\n", Platform::Topology::GetAddressSpaceId(), seq_num++, HexBuffer ) ;
             } /* endfor */
             int Remaining = aLen - 32 * FullLines ;
             if (Remaining > 0)
             {
                for (int CharIndex=0; CharIndex < Remaining; CharIndex += 1)
                {
                   int xByte = BufChar[32*FullLines+CharIndex] ;
                   HexBuffer[CharIndex*2  ] = HexToChar[(xByte >> 4) & 0x0f ] ;
                   HexBuffer[CharIndex*2+1] = HexToChar[ xByte       & 0x0f ] ;
                } /* endfor */
                PRINTF_FUNCTION("STBFX %d %d SendTo_BaseFx >%.*s<\n",Platform::Topology::GetAddressSpaceId(), seq_num++,
                                2*Remaining, HexBuffer ) ;
             } /* endif */
          } ;

        static inline
        void
        SendTo( Address aDummy, void *aBuffer, int aLen )
          {
          SendTo_BaseFx( aDummy, aBuffer, aLen );
          }
#endif

      };
// No ability to read anything forBG/L bringup
#if !defined(PK_BLADE)
     // File descriptor IO. This covers up CYCLOPS returning short reads
     class FileIO
     {
        public:
       static int Read (int FileDescriptor, void* Buffer, int Length )
         {
         return read(FileDescriptor, Buffer, Length) ;
         }
     };
#endif
     // Tuning parameters for load balancing
     class LoadBalance
     {
        public:
        enum {
          k_TuneCount = 2
        } ;
        static double mTune[2] ;
        static void SetTune(unsigned int x, double aTune)
        {
                if ( x < k_TuneCount )
                {
                        mTune[x] = aTune ;
                }
        }
        static double GetTune(unsigned int x)
        {
                assert(x<k_TuneCount) ;
                return mTune[x] ;
        }
     } ;
    class Profiling
    {
    	public:
    	static void TurnOff(void) ;
    	static void TurnOn(void) ;
    } ;
    class Heap
    {
    	public:
    	static void * Allocate(size_t) ;
    	static void Free(void *) ;
    } ;
  };




///////////
static char *
GetWriteErrorMessage()
  {
  char *rc = "Unknown";

#ifdef PK_POSIX

  switch( errno )
    {
    case EBADF :
       rc =  "fd is not a valid file descriptor or  is?not  open \
              for writing.";
       break;

    case EINVAL :
       rc =  "fd is attached to an object which is unsuitable for \
       ?      writing.";
       break;

    case EFAULT :
       rc =  "buf is outside your accessible address space.";
       break;

    case EFBIG :
       rc =  "An attempt was made to write a  file  that  exceeds \n\
       ?      the implementation-defined maximum file size or the \n\
       ?      process' file size limit, or to write at a position \n\
       ?      past than the maximum allowed offset.";
       break;

    case EPIPE :
       rc =  "fd  is  connected to a pipe or socket whose reading \n\
       ?      end is closed.  When this happens the writing  pro- \n\
       ?      cess  will receive a SIGPIPE signal; if it catches, \n\
       ?      blocks or ignores this the error EPIPE is returned.";
       break;

    case EAGAIN :
       rc =  "Non-blocking I/O has been selected using O_NONBLOCK \n\
       ?      and the write would block.";
       break;

    case EINTR :
       rc =  "The call was interrupted by  a  signal  before  any \n\
       ?      data was written.";
       break;

    case ENOSPC :
       rc =  "The  device  containing  the file referred to by fd \n\
       ?      has no room for the data.";
       break;

    case EIO :
       rc =  "A low-level I/O error occurred while modifying  the \n\
       ?      inode.";
       break;

    default:
       rc =  "errno not recognized";
    };

#endif
  return( rc );
  }
// #undef PRINTF_FUNCTION
// #undef EXIT_FUNCTION
///////////


//#define BGL_VIRTUAL_FIFO
#ifdef PK_BLADE

enum { VirtualFifo_LogBase2_PacketCount = 15 };
enum { VirtualFifo_PacketCount          = 0x01 <<  VirtualFifo_LogBase2_PacketCount };
enum { VirtualFifo_CounterToIndexMask   = VirtualFifo_PacketCount - 1 };

typedef AtomicNative< unsigned long > T_VirtualFifo_Counter;
//typedef CacheIsolator< unsigned long > T_VirtualFifo_Counter;


typedef CacheIsolator< _BGL_TorusPkt, 6 > T_CacheIsolatedBGLPacket;

extern "C" {
extern T_VirtualFifo_Counter VirtualFifo_PutCnt; // ++ only by putter
extern T_VirtualFifo_Counter VirtualFifo_GotCnt;
extern T_VirtualFifo_Counter VirtualFifo_CachedPutCnt;
extern T_VirtualFifo_Counter VirtualFifo_CachedGotCnt;
  extern T_CacheIsolatedBGLPacket* VirtualFifo_PacketBuffer;// [ VirtualFifo_PacketCount ];
  //extern T_CacheIsolatedBGLPacket VirtualFifo_PacketBuffer[ VirtualFifo_PacketCount ];
}

static int
BGLVirtualFifoPoll( int aCount = 1 )
  {

//    BegLogLine( PKFXLOG_VIRTUAL_FIFO )
//       << "BGLVirtualFifoPoll "
//       << "Entering BGLVirtualFifoPoll "
//       << EndLogLine;

    // s0printf("BGLVirtualFifoPoll entering BGLVirtualFifoPoll\n");


  int last_count;
  int Rc = 0;
  while( aCount > 0 )
    {

      last_count = aCount;
    // If queue looks empty in cached values, fence in some new data
    if( VirtualFifo_CachedPutCnt.mVar == VirtualFifo_GotCnt.mVar )
      {
      Platform::Memory::ImportFence( (void *) &( VirtualFifo_PutCnt ) , sizeof( VirtualFifo_PutCnt ) );
      VirtualFifo_CachedPutCnt = VirtualFifo_PutCnt;

//        BegLogLine( PKFXLOG_VIRTUAL_FIFO )
//          << "BGLVirtualFifoPoll "
//          << " VirtualFifo_CachedPutCnt=" << (int) VirtualFifo_CachedPutCnt.mVar
//          << " VirtualFifo_GotCnt=" << (int) VirtualFifo_GotCnt.mVar
//          << EndLogLine;

//       s0printf("BGLVirtualFifoPoll VirtualFifo_CachedPutCnt=%d VirtualFifo_GotCnt=%d\n",
//                VirtualFifo_CachedPutCnt.mVar,
//                VirtualFifo_GotCnt.mVar );
     }

    if( VirtualFifo_CachedPutCnt.mVar != VirtualFifo_GotCnt.mVar )
      {
      Rc++;
      int PacketIndex = VirtualFifo_GotCnt.mVar & VirtualFifo_CounterToIndexMask;

      if( PacketIndex >= VirtualFifo_PacketCount )
        PLATFORM_ABORT("PacketIndex >= VirtualFifo_PacketCount");

      _BGL_TorusPkt  *Pkt = &( VirtualFifo_PacketBuffer[ PacketIndex ].mThisUnion.Instance );

      Platform::Memory::ImportFence( (void *)Pkt, sizeof(_BGL_TorusPkt));

      unsigned actor = Pkt->hdr.swh0.fcn;
      actor = ( actor << 2 ) | ((( unsigned ) BGLVirtualFifoPoll ) & 0xFF000000 );

      // unsigned actor = _TS_GET_FCN( Pkt->hdr.swh0 )

//       BegLogLine( PKFXLOG_VIRTUAL_FIFO )
//         << "BGLVirtualFifoPoll "
//         << " VirtualFifo_CachedPutCnt=" << (int) VirtualFifo_CachedPutCnt.mVar
//         << " VirtualFifo_GotCnt=" << (int) VirtualFifo_GotCnt.mVar
//         << " PacketIndex=" << PacketIndex
//         << " VirtualFifo_CounterToIndexMask" << VirtualFifo_CounterToIndexMask
//         << " actor=" << (void *) actor
//         << EndLogLine;

//       s0printf("BGLVirtualFifoPoll VirtualFifo_CachedPutCnt=%d\n",  VirtualFifo_CachedPutCnt.mVar );
//       s0printf("BGLVirtualFifoPoll VirtualFifo_GotCnt=%d\n",  VirtualFifo_GotCnt.mVar );
//       s0printf("BGLVirtualFifoPoll PacketIndex=%d\n", PacketIndex );
//       s0printf("BGLVirtualFifoPoll VirtualFifo_CounterToIndexMask=%d\n", VirtualFifo_CounterToIndexMask );
//       s0printf("BGLVirtualFifoPoll actor=%08X\n", (void *) actor );

      if( (void *) actor == NULL )
        PLATFORM_ABORT( "ERROR:: BGLVirtualFifoPoll about to call an actor == NULL" );

      ((BGLTorusBufferedActivePacketHandler)(actor))( & Pkt->payload, Pkt->hdr.swh1.arg, Pkt->hdr.swh0.extra );

      // Should invalidate the packet now so that it needn't be stored back

      // This could also be cached and exported only once in a while... as in the pk queues.
      VirtualFifo_GotCnt.mVar ++;
      Platform::Memory::ExportFence( (void * )&( VirtualFifo_GotCnt ) , sizeof( VirtualFifo_GotCnt ) );
      aCount--;
      }

    // Didn't find anything
    if(last_count==aCount)
      break;
    }
  return( Rc );
  }


// Use for unaligned data or bytes < full chunks
static void
VirtualFifo_BGLTorusSendPacketUnaligned( _BGL_TorusPktHdr *hdr,   // Previously created header to use
                                         void             *data,              // Source address of data
                                         int               bytes )             // Payload bytes <= ((chunks * 32) - 16)
  {
  // If the queue looks full, fence in some new data and cache it
  // change the following while to IF for non-blocking
  while( (VirtualFifo_CachedGotCnt.mVar + VirtualFifo_PacketCount) == VirtualFifo_PutCnt.mVar  )
    {
    Platform::Memory::ImportFence( (void *) &( VirtualFifo_GotCnt ) , sizeof( VirtualFifo_GotCnt ) );
    VirtualFifo_CachedGotCnt = VirtualFifo_GotCnt;

//     BegLogLine( PKFXLOG_VIRTUAL_FIFO )
//       << "VirtualFifo_BGLTorusSendPacketUnaligned"
//       << " VirtualFifo_CachedGotCnt=" << (int) VirtualFifo_CachedGotCnt.mVar
//       << " VirtualFifo_PutCnt=" << (int) VirtualFifo_PutCnt.mVar
//       << EndLogLine;

//     s0printf("VirtualFifo_BGLTorusSendPacketUnaligned VirtualFifo_CachedGotCnt=%d\n", VirtualFifo_CachedGotCnt.mVar );
//     s0printf("VirtualFifo_BGLTorusSendPacketUnaligned VirtualFifo_PutCnt=%d\n", VirtualFifo_PutCnt.mVar );
    }

  // If the queue isn't full...
  if( VirtualFifo_CachedGotCnt.mVar + VirtualFifo_PacketCount != VirtualFifo_PutCnt.mVar )
    {
    int PacketIndex = VirtualFifo_PutCnt.mVar & VirtualFifo_CounterToIndexMask;

    if( PacketIndex >= VirtualFifo_PacketCount )
      PLATFORM_ABORT("VirtualFifo_BGLTorusSendPacketUnaligned(): PacketIndex >= VirtualFifo_PacketCount");

    _BGL_TorusPkt  *Pkt = &( VirtualFifo_PacketBuffer[ PacketIndex ].mThisUnion.Instance );

    Pkt->hdr = *hdr;

    if( bytes > sizeof( _BGL_TorusPktPayload ) )
      {
      sprintf(SendReduceBuff, "VirtualFifo_BGLTorusSendPacketUnaligned(): send bytes=%d > payload size=%d" , bytes, sizeof( _BGL_TorusPktPayload ) );
      PLATFORM_ABORT( SendReduceBuff );
      }

    memcpy( Pkt->payload, data, bytes );
    Platform::Memory::ExportFence((void *)Pkt, sizeof(_BGL_TorusPkt));

//     BegLogLine( PKFXLOG_VIRTUAL_FIFO )
//       << "VirtualFifo_BGLTorusSendPacketUnaligned"
//       << " VirtualFifo_CachedGotCnt=" << (int) VirtualFifo_CachedGotCnt.mVar
//       << " VirtualFifo_PutCnt=" << (int) VirtualFifo_PutCnt.mVar
//       << " PacketIndex=" << PacketIndex
//       << " Pkt=" << (void *) Pkt
//       << " Pkt->hdr.swh0.fcn" << (void *)Pkt->hdr.swh0.fcn
//       << EndLogLine;

//     s0printf("VirtualFifo_BGLTorusSendPacketUnaligned VirtualFifo_CachedGotCnt=%d\n", VirtualFifo_CachedGotCnt.mVar );
//     s0printf("VirtualFifo_BGLTorusSendPacketUnaligned VirtualFifo_PutCnt=%d\n", VirtualFifo_PutCnt.mVar );
//     s0printf("VirtualFifo_BGLTorusSendPacketUnaligned PacketIndex=%d\n", PacketIndex );
//     s0printf("VirtualFifo_BGLTorusSendPacketUnaligned Pkt=%08X\n", (void *) Pkt );
//     s0printf("VirtualFifo_BGLTorusSendPacketUnaligned Pkt->hdr.swh0.fcn=%08X\n", (void *) Pkt->hdr.swh0.fcn );

    // Should invalidate the packet now so that it needn't be stored back

    // This could also be cached and exported only once in a while... as in the pk queues.
    VirtualFifo_PutCnt.mVar ++;
    Platform::Memory::ExportFence( (void *)&( VirtualFifo_PutCnt ) , sizeof( VirtualFifo_PutCnt ) );
    }
  }

#endif

#define pk_16B ( 16 )
#define pk_32B ( 32 )
#define pk_4KB ( 4*1024 )
#define pk_1MB (1*1024*1024)
#define pk_16MB (16*1024*1024)
#define pk_256MB (256*1024*1024)

/***************************************************
 * This call assumes that it's only called from one
 * core. size arg might not be equal to sizeof( Type)
 * if we allocate an array
 **************************************************/
// #include <pk/fxlogger.hpp>

extern void pkDelete( void* aToDelete, int isArray );

// int pknew_fxlog;

template <class Type>
inline void pkNew( Type** objToNewPtr, int count, char* file="none", int line=-1, int toPrint=0  )
{
#ifdef PK_BLADE

  unsigned sizeRequested = sizeof(Type) * count;

  // Make sure that we always start on a 16B bountry
  unsigned SizeOfPadding    = pk_16B - ( sizeRequested ) % pk_16B;

  int sizeToAllocate = sizeRequested + SizeOfPadding;

  *objToNewPtr = ( Type *) malloc( sizeToAllocate + CACHE_ISOLATION_PADDING_SIZE );

  if( PKFXLOG_PKNEW && ( Platform::Topology::GetAddressSpaceId() == 0 ) && toPrint )
    {
      PRINTF_FUNCTION("pkNew entering ");
      PRINTF_FUNCTION(" size(bytes) to allocate: %d", sizeToAllocate );
      PRINTF_FUNCTION(" at Ptr: %08x", (void *) (*objToNewPtr) );
      PRINTF_FUNCTION(" caller: %s\n", file );
      PRINTF_FUNCTION(" line: %d\n", line );
    }


#if 0
  int remainingMemory = pk_256MB - myKend.mVar;

  if( PKFXLOG_PKNEW && ( Platform::Topology::GetAddressSpaceId() == 0 ) && toPrint )
    {
      PRINTF_FUNCTION("pkNew entering ");
      PRINTF_FUNCTION(" size(bytes) to allocate: %d", sizeof( Type ) * count );
      PRINTF_FUNCTION(" remaining memory: %d", remainingMemory );
      PRINTF_FUNCTION(" at Ptr: %08x", (void *) (*objToNewPtr) );
      PRINTF_FUNCTION(" myKend: %d\n",myKend.mVar );
      PRINTF_FUNCTION(" caller: %s\n", file );
      PRINTF_FUNCTION(" line: %d\n", line );
    }

  if( (sizeof( Type ) * count) > remainingMemory )
    PLATFORM_ABORT("ERROR:: No more store for pkNew") ;


  unsigned SizeOfAllocation = ( sizeof(Type) * count );
  unsigned SizeOfPadding    = pk_16B - ( myKend.mVar + SizeOfAllocation ) % pk_16B;

  unsigned BytesToAllocate = SizeOfAllocation + SizeOfPadding;

  // Check to make sure that we're giving away 0xdeadbeef
  for( int i=0; i < ( BytesToAllocate / sizeof(unsigned) ); i++)
    {
      if( *((unsigned *) myKend.mVar + i ) != 0xdeadbeef )
        {
          sprintf(strBuf, "ERROR:: pkNew can not return clean memory to caller: %s line: %d BytesToAllocate: %d myKendPtr: %08X myKendValue: %08X i: %d ", file, line, BytesToAllocate, myKend.mVar, *((unsigned *)myKend.mVar), i );

          PLATFORM_ABORT( strBuf );
        }
    }

  *objToNewPtr = (Type *) myKend.mVar;

  // Mark the memory as allocated
  for( int i=0; i < ( BytesToAllocate / sizeof(unsigned) ); i++)
    {
      *((unsigned *) myKend.mVar + i ) = 0xfefefefe;
    }

  myKend.mVar += BytesToAllocate;


//   myKend.mVar += ( sizeof(Type) * count );
//   myKend.mVar += ( pk_16B - myKend.mVar % pk_16B );
  Platform::Memory::ExportFence( (void*) objToNewPtr, sizeof( Type* ) );

  remainingMemory = pk_256MB - myKend.mVar;

//   BegLogLine( PKFXLOG_PKNEW )
//     << "pkNew leaving "
//     << " size(bytes) to allocate: " << sizeof( Type ) * count
//     << " remaining memory: " << remainingMemory
//     << " allocated Ptr: " << (void *) (*objToNewPtr)
//     << " myKend: " << myKend.mVar
//     << EndLogLine;

  if( PKFXLOG_PKNEW && ( Platform::Topology::GetAddressSpaceId() == 0 ) && toPrint )
    {
      PRINTF_FUNCTION("pkNew leaving ");
      PRINTF_FUNCTION(" size(bytes) to allocate: %d", sizeof( Type ) * count );
      PRINTF_FUNCTION(" remaining memory: %d", remainingMemory );
      PRINTF_FUNCTION(" at Ptr: %08x", (void *) (*objToNewPtr) );
      PRINTF_FUNCTION(" myKend: %d\n",myKend.mVar);
    }
#endif
#else

  // printf("pknew_fxlog=%d\n", pknew_fxlog );
  // toPrint = pknew_fxlog;

  if( PKFXLOG_PKNEW && ( Platform::Topology::GetAddressSpaceId() == 0 ) && toPrint )
  //  if( toPrint )
    {
      PRINTF_FUNCTION("pkNew entering from file: %s, line %d", file, line);
      PRINTF_FUNCTION(" size(bytes) to allocate: %d", sizeof( Type ) * count );
      PRINTF_FUNCTION(" at Ptr: %08x\n", (void *) (*objToNewPtr) );
    }

  if( count == 1 )
    {
    *objToNewPtr = new Type;
    if( *objToNewPtr == NULL )
      PLATFORM_ABORT("*objToNewPtr == NULL");
    }
  else
    {
      if( count == 0 )
        {
        PRINTF_FUNCTION( "pkNew() Called from file: %s line %d\n", file, line );
        assert( 0 );
        }
        // PLATFORM_ABORT("count == 0");

      *objToNewPtr = new Type[ count ];
      if( *objToNewPtr == NULL )
        PLATFORM_ABORT("*objToNewPtr == NULL");
    }

  if( PKFXLOG_PKNEW && ( Platform::Topology::GetAddressSpaceId() == 0 ) && toPrint )
    {
      PRINTF_FUNCTION("pkNew leaving");
      PRINTF_FUNCTION(" size(bytes) to allocate: %d", sizeof( Type ) * count );
      PRINTF_FUNCTION(" at Ptr: %08x\n", (void *) (*objToNewPtr) );
    }

  fflush(stdout);
#endif
}



#ifdef PK_BLADE
#include <new>

void* pkMalloc( unsigned size );
void pkFree( void* ptr );
void* operator new( unsigned size );
void operator delete( void * ptr );
#endif

#endif
