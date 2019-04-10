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



#ifdef PK_SPI
//#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <spi.h>
#include <errno.h>

// Make-ups
static inline int svcGetPartitionSize(void) { return 1 ; }
#endif

#ifdef PK_SMP
#define PK_PTHREADS
#endif

#ifdef PK_MPI
#define PK_PTHREADS
#endif

#ifdef PK_MPICH
#define PK_PTHREADS
#endif

#ifdef PK_LINUX
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>  // for waitpid() in set
#ifdef PK_PTHREADS
#include <pthread.h>
#endif
#endif

#ifdef PK_BGL
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

extern "C"
{
#include <sys/time.h>
}
#endif

// Support for explicitly declaring parallel computation
#ifdef PK_BGL
extern "builtin" void __compute_parallel(double, double, ...) ;
#define compute_parallel __compute_parallel
#else
// No-op it on compilers that don't know about Double Hummer
static inline void compute_parallel(double X0, double X1, ...)
{
}
#endif

#ifdef PK_AIX
extern "C"
{
extern void exit(int);
extern void yield(void);
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/atomic_op.h>
#include <sched.h>
#ifdef  PK_PTHREADS
#include <pthread.h>
#endif
}

#if __IBMCPP__ >= 500
// 'new' VisualAge C++ supports and requires typename
#define A98_TYPENAME typename
#else
    // 'old' VisualAge C++ doesn't support typename and doesn't know bool
#define A98_TYPENAME

#if !defined(bool)
typedef unsigned int bool ;
#endif

#endif
#endif

#ifdef PK_POSIX
#include <iomanip.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
extern "C"
{
#include <sys/time.h>
}
#endif

// The following takes care of accessing the field of an MPI_Status struct for IBM and MPICH
#ifdef  PK_MPICH
#define PK_MPI_ALL
extern "C"
{
#include <mpi.h>
}
#define    MPI_STATUS_TAG(Status)    (Status.MPI_TAG)
#define    MPI_STATUS_SOURCE(Status) (Status.MPI_SOURCE)
#define    MPI_STATUS_ERROR(Status)  (Status.MPI_ERROR)
#endif

#ifdef  PK_MPI
#define PK_MPI_ALL
extern "C"
{
#include <mpi.h>
}
#define    MPI_STATUS_TAG(Status)    (Status.tag)
#define    MPI_STATUS_SOURCE(Status) (Status.source)
#define    MPI_STATUS_ERROR(Status)  (Status.error)
#endif

#ifdef PK_A98
// Things to do if ansi98
#endif

#ifdef PK_XLC
//////// BGF why is this here? typedef int bool ;
#ifndef A98_TYPENAME
#define A98_TYPENAME  // Shouldn't this be down in PK_GCC ?
#endif
#endif

#ifdef PK_GCC
////// BGF why is this here?  typedef int bool ;
#define A98_TYPENAME typename  // Shouldn't this be down in PK_GCC ?
#endif

#ifdef PK_PTHREADS
#if !defined(PK_BGL_UNI)
#include <pk/setpri.hpp>
#endif
#endif

// Universal macros

#ifndef TRUE
enum { TRUE = 1 };
#endif
#ifndef FALSE
enum { FALSE = 0 };
#endif

#define PLATFORM_ABORT(reason) \
   Platform::Control::Abort( reason, __FILE__, __LINE__);

//Define assert to call platform abort.
// DO NOT INCLUDE assert.h !!!!
#ifdef NO_PK_PLATFORM
#include <assert.h>
#else
#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
#define assert(__EX) ((__EX) ? ((void)0) : Platform::Control::Abort(# __EX, __FILE__, __LINE__))
#endif
#endif
// Set up diagnostic versions of math functions, so we can see callers
#if defined(BLUEGENE_DIAG_MATH)
#include <math.h>
// Wrapper for square root calls, so we can see who is using so many
double sqrt_wrap(double X) ;
#if defined(sqrt)
#undef sqrt
#endif
#define sqrt(X) sqrt_wrap(X)
#endif

typedef void * (*PointerToThreadFunction)( void* );

class Platform  // this class is mainly to provide a name space
  {
  public:
    class Control
      {
      public:

        static void
        Abort(char *reason, char *file, int line )
          {

//               BegLogLine(1)
//                   << "Platform::Abort(): Reason >" << reason << "<  file >" << file << " line " << line
//                   << EndLogLine;


#if !defined(PK_BGL_UNI)
          fprintf(stderr, "Platform::Abort() [%d:%d] >%s< line %d, file >%s<\n",
             Platform::Topology::GetAddressSpaceId(),
             Platform::Thread::GetId(),
             reason, line, file );
          fflush(stderr);  // shouldn't be required but shouldn't hurt either.
#endif

#ifdef PK_MPI_ALL
          MPI_Abort(MPI_COMM_WORLD, -1);
#endif
          abort(); // in unix stdlib.h
          }

        static void
        Finalize()
          {
            //          fprintf(stderr, "Platform::Finalize()\n" );
#if !defined(PK_BGL_UNI)
          fflush(stderr);  // shouldn't be required but shouldn't hurt either.
#endif

#ifdef PK_MPI_ALL
          MPI_Finalize();
#endif
          exit( 0 );
          }
      };

    //*********************** Reactor *************************************


    class Reactor
      {
      public:
        enum{ RETURN_OK=0, RETURN_DETATCH_BUFFER=1, RETURN_ERROR=-1 };

#ifndef PK_PLATFORM_REACTOR_PAYLOAD_SIZE
#ifdef  PK_SPI
#define PK_PLATFORM_REACTOR_PAYLOAD_SIZE  (BG_MESSAGE_LARGE_DATA_LIMIT*sizeof(double))
#else
// All other times, figure it's MPI or SMP on AIX and use large datagrams.
#define PK_PLATFORM_REACTOR_PAYLOAD_SIZE  32*1024
#endif
#endif //#ifndef PK_PLATFORM_REACTOR_PAYLOAD_SIZE

        enum{ PAYLOAD_SIZE = PK_PLATFORM_REACTOR_PAYLOAD_SIZE };

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
#endif

#if 0 // Broadcast active message... needs member template support to do right!
      // Note: there is no similar low level support for reduce because reduce is not
      //       as low a level primitive as broadcast.

        static int
        Trigger( Topology::Group                *GroupIFPtr, // GROUP interface pointer.
                 Reactor::FunctionPointerType    FxPtr,
                 void                           *MsgBuf,
                 unsigned                        MsgLen)
          {
          for(int i = 0; i < GroupIFPtr->Count; i-- )
            {
            int rc = 0;
            Trigger( GroupIFPtr->Get.Target( i ), FxPtr, MsgBuf, MsgLen );
            if( rc != 0 )
              break;
            }
          return( rc );
          }
#endif
      };

    //*********************** Sync *************************************

    class Lock
      {
      // As a user of the platform:: interface, you should probably use MUTEX.
      // This is because these locks don't fence memory - bad for RS6K.
      public:

#ifdef PK_SPI
        typedef volatile unsigned int LockWord;  // per spi/locks.h ... would like an opaque SPI type.
#else
        typedef int LockWord;
#endif

        //Takes a reference to a LockWord
        //Returns TRUE for success and FALSE for failure.
        static inline int
        TryLock( LockWord &x )
          {
          int rc = 0;
#ifdef PK_SPI
#if 1
          // Nowadays this is in the SPI header.
          int rcn = bg_try_lock(&x) ;
          rc = !rcn ;
#else
          //NEED: to ask Derek for an inline SPI call interface to this
          __asm__ volatile ("ts %0,0(%1)" : "=r"(rc) : "r"(&x));
          rc = !rc;
#endif
#else
#ifdef PK_AIX
          int y = 0, z = 1;
          rc =  ! _check_lock ( &x, y, z );
#else
          if( x == 0 )
            {
            x = 1;
            rc = 1;
            }
#endif
#endif
          return( rc );
          }

        static inline void
        Unlock( LockWord &x )
          {
#ifdef PK_AIX
          int y = 0;
          _clear_lock( &x, y ) ;
#else
          //NOTE: this works for SPI currently but would like to get a SPI call
          x = 0;
#endif
          }

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
#ifdef PK_AIX
          int Rc = fetch_and_add( &aAtomicInteger, aValue );
#else
          aAtomicInteger += aValue;
          int Rc = aAtomicInteger;
#endif
          return( Rc );
          }

        // Export and Import fences deal with losely ordered memory architectures.
        // ExportFence ensures that all stores have been done (as far as L1 cache) before
        // letting any more stores start
        static inline void
        ExportFence(void)
          {
#ifdef PK_SPI
//          memsync();
          bg_sync();
#endif
#ifdef PK_AIX
          // Uses the side effects of _clear_lock to export all data to main memory
          // Will be implemented more directly as asm soon.
          int a ;
          _clear_lock(&a,0) ;
#endif
          }

        // ImportFence ensures that any 'load' instructions before here in program order
        // complete before any 'load' instructions after here in program order are
        // started
        static inline void
        ImportFence(void)
          {
#ifdef PK_SPI
          //CURRENTLY 12/02/01 not required today for cyclops NEED: check again.
#endif
#ifdef PK_AIX
          // Uses the side effects of _clear_lock to export all data to main memory
          // Will be implemented more directly as asm soon.
          int a = 0 ;
          (void) _check_lock(&a, 0, 0) ;
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

        static inline int
        TryLock( Platform::Lock::LockWord &aLockWord )
          {
          int rc = Lock::TryLock( aLockWord );
          Memory::ImportFence();
          return( rc );
          }

        static inline void
        YieldLock( Platform::Lock::LockWord &aLockWord )
          {
          while( ! Lock::TryLock( aLockWord ) )
            Platform::Thread::Yield();
          Memory::ImportFence();
          }

        //// SpinLock() not used today, probably a bad idea but might have a place on
        //// on a platform which had multiple physical processors AND a greater number of
        //// threads.  Really though, YieldLock ought to be good enough!
        ////inline void
        ////SpinLock( Platform::Lock::LockWord &aLockWord )
        ////  {
        ////  while( ! Lock::TryLock( aLockWord ) );
        ////  }

        static inline void
        Unlock( Platform::Lock::LockWord &aLockWord )
          {
          Memory::ExportFence();
          Lock::Unlock( aLockWord );
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
          // On aix or with pthread, you need to fix the priorities. (setpri.hpp)
#ifdef PK_SPI
          // There is no thread yield in SPI for which is ok because threads are dedicated.
#else
#ifdef PK_BGL_UNI
          // There is no thread yield on the 'bringup' BGL
#else
          yield();
#endif
#endif
          return(0);
          }
#if !defined(PK_BGL_UNI)
        static inline int
        Sleep(int Secs) // I'm not too happy about this here. It could be done as YieldUntil() fx.
          {
#ifdef PK_SPI
          Platform::Clock::TimeValue end, now;
          end = Platform::Clock::GetTimeValue();
          end.Seconds += Secs;
          while( 1 )
            {
            now = Platform::Clock::GetTimeValue();
            if( (now.Seconds > end.Seconds)
                 ||
                ( (now.Seconds == end.Seconds) && (now.Nanoseconds > end.Nanoseconds ) ) )
              {
              break;
              }
            Platform::Thread::Yield();
            }
#else
          sleep(Secs);
#endif
          return(0);
          }
#endif

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
#ifdef    PK_SPI
          // Seems that we'll want a thread exit that updates Platform::Thread structures
          // and then joins itself thus freeing the proc for other thread allocations.
//          svcThreadExit();
          bg_svc_thread_exit() ;
#else
#ifdef   PK_BGL_UNI
         // Shouldn't get here
          assert(0) ;
          exit(1) ;
#else
          // Go clear out platform thread data structures.
          pthread_exit( value );
#endif
#endif
          }

        enum { ID_INVALID = -1 };

        static inline
        int
        GetId()
          {
          int ThreadId = 0;
#if !defined(PK_BGL_UNI)
#ifdef PK_PTHREADS
          // Need to get the context array struct index as the id.
          ThreadId = (((int)pthread_self()) % 0x0000FFFF); //////////////// & 0x0000FFFF);
#elif defined(PK_SPI)
          ThreadId = bg_thread_get_id() ;
#endif
#endif
          return( ThreadId );
          }

      };

    //*********************** Clock and time values ****************************

    class Clock
      {
      public:
#ifdef PK_SPI
        static unsigned int mSPI_TimeEvent;
#endif
        typedef struct
          {
          // ????
          } RawClockValue;

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

        static inline TimeValue
        ConvertRawClockValueToTimeValue( RawClockValue &aTimeValue )
          { assert(0); TimeValue x;  x.Seconds=0; x.Nanoseconds=0; return(x);  }

        static inline
        TimeValue
        GetTimeValue()
          {
          TimeValue aTimeValue;

#ifdef PK_SPI
          //NEED: an SPI call here - for now count calls
          aTimeValue.Nanoseconds = 0;
          aTimeValue.Seconds     = mSPI_TimeEvent++;
#endif


#ifdef PK_LINUX
          struct timeval  tv;
          struct timezone tz;
          gettimeofday( &tv, &tz );
          aTimeValue.Seconds     = tv.tv_sec;
          aTimeValue.Nanoseconds = tv.tv_usec * 1000 ;
#endif

#ifdef PK_AIX
          timebasestruct_t Now;
          read_real_time( &Now, TIMEBASE_SZ );
          time_base_to_time( &Now, TIMEBASE_SZ );
          aTimeValue.Seconds      = Now.tb_high;
          aTimeValue.Nanoseconds  = Now.tb_low ;
#endif

#ifdef PK_BGL_UNI
          aTimeValue.Nanoseconds = 0 ;
          aTimeValue.Seconds     = 0 ;
#endif

          return( aTimeValue );
          }

        // NEED: Move this out to a header file..

        // Should move the stuff that works on doubles elsewhere.
        static inline
        double
        ConvertTimeValueToDouble( TimeValue &aTimeValue )
          {
          double rc = aTimeValue.Nanoseconds * 0.000000001 + aTimeValue.Seconds;
          return(rc);
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
      };

    //********************** Topology ***********************************
    // Get the logical address space id - that is, an index into the route table.
    class Topology
      {
      public:

        enum{ MAX_DIMENSIONALITY = 512 };  // Maximum logical dimensionality supported.

        class Group {};  // Group of address space ids.  We support this in concept.

        typedef int AddressSpaceId;

        // Stateless calls to virtualiziation layer.

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

        static int ThisAddressSpaceId;

        static inline
        int
        GetAddressSpaceId()
          {
          return( ThisAddressSpaceId );
          }


        // This call is used by the application configurator
        // to attempt to best map the application topology
        // to the machine topology.
        // This is mostly a statement about the distance between nodes.
        static inline
        int
        GetPhysicalDescription( int &AddressSpaceCount,
                                int &AddressSpaceConnectivity,
                                int &PhysicalDimensionality )
          {
#ifdef PK_MPI_ALL
          int s;
          MPI_Comm_size(MPI_COMM_WORLD, &s );
          AddressSpaceCount = s;
          AddressSpaceConnectivity = s - 1;
          PhysicalDimensionality = 0;
#else
#ifdef PK_SPI
          //NEED: might need to virtualize returns from svc***
          AddressSpaceCount = svcGetPartitionSize();
          AddressSpaceConnectivity = 6;
          PhysicalDimensionality = 3;
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
          int AddressSpaceCount;
          int AddressSpaceConnectivity;
          int PhysicalDimensionality;
          GetPhysicalDescription( AddressSpaceCount,
                                  AddressSpaceConnectivity,
                                  PhysicalDimensionality );
          return( AddressSpaceCount );
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
// BGL bringup doesn't support functional logging
#if !defined(PK_BGL_UNI)
          // This is the business end of the logging
          if( LogFd <= 0 )    // Questionable attempt to open logging on first use.
            OpenLogPort();
          assert( LogFd > 0 );
          ::write( LogFd, aBuffer, aLen);
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
        static Mutex::MutexWord LogLockVar;

        typedef int Address;  // for now a file or socket handle.

        static Address mAddress;

#if !defined(PK_BGL_UNI)
        static
        void
        OpenRawDataPort()
          {
          int open_perms = 0660 ;
          mAddress = open( "RawDatagramOutput" ,
                        O_CREAT | O_TRUNC | O_WRONLY | O_APPEND,  // O_APPEND Required to ensure proper writes by threaded apps
                        open_perms );
          assert( mAddress > 0 );  // Log file failed open.
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
          // This is the business end of the logging
          if( mAddress <= 0 )    // Questionable attempt to open logging on first use.
            OpenRawDataPort();
          assert( mAddress > 0 );
          ::write( mAddress, aBuffer, aLen);
          ///::fsync( mAddress );
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
          }
#else
        enum {
           kBufferSize = 100000
           } ;
        static char Buffer[kBufferSize] ;
        static unsigned int BufferIndex ;

        static inline
        void
        OpenRawDataPort() { } ;

// In BGL simulator bringup, we put 'datagrams' into a buffer for examining later
        static inline
        void
        SendTo_BaseFx( Address aDummy, void * aBuffer, int aLen )
          {
             int Available = kBufferSize - BufferIndex ;
             int Actual = ( aLen < Available ) ? aLen : Available ;
             memcpy(Buffer+BufferIndex,aBuffer,Actual) ;
             BufferIndex += Actual ;
          } ;

        static inline
        void
        SendTo( Address aDummy, void *aBuffer, int aLen )
          {
            SendTo_BaseFx( aDummy, aBuffer, aLen );
          }
#endif

      };
     // File descriptor IO. This covers up CYCLOPS returning short reads
     class FileIO
     {
        public:
       static int Read (int FileDescriptor, void* Buffer, int Length )
       {
#if defined(CYCLOPS)
          int readsofar = 0 ;
          do
          {
             int partread = read(FileDescriptor, ((char *)Buffer)+readsofar, Length-readsofar) ;
             if (partread <= 0 ) return readsofar ;
             readsofar += partread ;
          } while ( readsofar < Length ); /* enddo */
          return Length ;
#else
          return read(FileDescriptor, Buffer, Length) ;
#endif
       }
     };
  };

#endif
