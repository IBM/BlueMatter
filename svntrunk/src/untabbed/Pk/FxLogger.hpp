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
 * Module:          pkfxlog.hpp
 *
 * Purpose:         Functional log stuff.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         170297 BGF Created.
 *                  240397 BGF Sacked the complex template typing.
 *                  030797 TJCW Arranged wrappering so that logging can
 *                              be compiled in on demand
 *                  130603 AR BGF rewrite to be based on sprintf rather than streams.
 *
 ***************************************************************************/
#ifndef __PKFXLOG_HPP__
#define __PKFXLOG_HPP__

#include <errno.h>
#include <assert.h>
#include <Pk/Time.hpp>
#include <Pk/Core.hpp>
#include <Pk/Node.hpp>
#include <Pk/FASched.hpp>

//NEED: this perhaps default to packet size.  Or be drawn from platform spec
#define PKLOG_MAXBUF ( 2 * 4096 )

#ifndef PKFXLOG_PATHNAME_DIRECTORIES_LIMIT
#define PKFXLOG_PATHNAME_DIRECTORIES_LIMIT ( 3 )
#endif
// Turn PKFXLOG macro into an enum - tjcw prefers enums to macros because tjcw likes writing in C++ rather than macro language

enum
  {
  PkAnyLogging =
  #if defined( PKFXLOG )
     1
  #else
     0
  #endif
  } ;

#ifndef PKFXLOG_ALL
#define PKFXLOG_ALL 0
#endif

#include <stdio.h>  // required for all due to use of sprintf as formatting engine
#include <unistd.h> // for write() system call ... should be moved

#ifndef __PK_API_HPP__  // this file is usually included by the pk/api.hpp but can be used without.
    #include <cstdlib>
  extern "C"
    {
    #include <unistd.h>
    #include <signal.h>
    #include <stdio.h>
    #include <time.h>
    #include <fcntl.h>
    #include <sys/time.h>
    #include <string.h>
    }
#endif

#define __PK_FXLOG_SERIOUS_ABORT \
        {                          \
        fflush(stdout);            \
        fflush(stderr);            \
        assert(0);                 \
        _exit(__LINE__*-1);        \
        char *x=NULL; x = "doom";  \
        for(;;);                   \
        }

static
void
FxLogger_Abort( char * str )
  {
   #ifndef NO_PK_PLATFORM
     //assert(0);

     fprintf(stderr,"errno: %d ... FxLogger going down: Cause: %s \n", errno, str);
     fflush(stderr);

     __PK_FXLOG_SERIOUS_ABORT

     // _exit(__LINE__*-1);
     //PkAssert(0,str);
     //PLATFORM_ABORT( str );
   #else
     // more or less, posix with PK_PLATFORM
     printf( "FATAL ERROR IN FXLOGGER  >%s<\n", str );
     perror( "FxLogger causing abort" );
     fflush( stderr );
     fflush( stdout );
     kill( getpid(), SIGTERM ); // give debugger half a chance catch this
     exit( -1 );
   #endif
  }

static
void
FxLogger_WriteBufferToLogPort( char * aBuffer, int aLen )
  {
  static int PortOpenedFlag = 0;

  #ifndef NO_PK_PLATFORM
  int BytesWritten = 0;
  while( BytesWritten < aLen )
    {
      int wlen = write( 1, & aBuffer[ BytesWritten ], aLen-BytesWritten );  // gotta think about this.
      
      if( wlen < 0 )
  {
    FxLogger_Abort( "FxLogger_WriteBufferToLogPort:: Aborting on a rc < 0" );
  }
      
      BytesWritten += wlen;
    }

  #else
    if( PortOpenedFlag == 0 )
      {
      fcntl(2, F_SETFL, O_APPEND );
      PortOpenedFlag == 1;
      }
    write( 2, aBuffer, aLen );
  #endif
  }


// This calss when constructed takes a snapshot of the current time
class FxLogger_TimeStamp
  {
public:
    unsigned mSeconds, mNanoseconds;

    FxLogger_TimeStamp()
      {
      #ifndef NO_PK_PLATFORM

         PkTimeGetSecsNanos( &mSeconds, &mNanoseconds );

      #else

        #ifdef PK_AIX  // for finer resolution timer

          timebasestruct_t Now;
          read_real_time( &Now, TIMEBASE_SZ );
          time_base_to_time( &Now, TIMEBASE_SZ );
          mSeconds      = Now.tb_high;
          mNanoseconds  = Now.tb_low ;

        #else // assume POSIX interfaces

          struct timeval  tv;
          struct timezone tz;
          gettimeofday( &tv, &tz );
          mSeconds     = tv.tv_sec;
          mNanoseconds = tv.tv_usec * 1000 ;
        #endif

      #endif

      }

  };

static char itox[] = "0123456789ABCDEF";

class FxLogger_NodeName
  {
  public:

    char * mNodeName;

    FxLogger_NodeName()
      {
      static char static_NodeName[32];
      static int entry1 ;
      if( ! entry1 )
        {
        #ifndef NO_PK_PLATFORM
          //int NodeId = Platform::Topology::GetNodeId();
          int NodeId = PkNodeGetId();
        #else
          int NodeId = getpid() ;
        #endif
        if( NodeId >= 0 && NodeId < (1024*1024) )
          {
          entry1 = 1;
          }
        else
          {
          NodeId = (int) static_NodeName;
          }
        sprintf(static_NodeName, "% 5d", NodeId );
        }

      mNodeName = static_NodeName;


      }

  };


// Contains thread name after constructed.
class FxLogger_CoreName
  {
  public:
    char mCoreName[9];

    FxLogger_CoreName()
      {
      unsigned CoreId = 0;

      #ifndef NO_PK_PLATFORM
        //CoreId = Platform::Core::GetId() ;
        CoreId = PkCoreGetId();
      #else
        #ifdef THREAD_SAFE
          pthread_t  tid = pthread_self() ; //& 0x0000FFFF);
          CoreId = tid & 0x0000FFFF;
        #endif
      #endif

      int SigFigFlag = 0;
      int ci = 0;
      for( int i = 0; i < 8 ; i++ )
        {
        if( CoreId & 0xF0000000 )
          SigFigFlag = 1;
        if( SigFigFlag )
          {
          mCoreName[ci] = itox[ (CoreId >> 28) & 0x0F ];
          ci++;
          }
        CoreId = CoreId << 4;
        }
      if( ci > 0 )
        mCoreName[ ci ] = '\0';
      else
        {
        // ... otherwise, it must be thread 0, right?
        mCoreName[0] = '0';
        mCoreName[1] = '\0';
        }
      }
  };


// Contains thread name after constructed.
class FxLogger_FiberName
  {
  public:
    char mFiberName[9];

    FxLogger_FiberName()
      {
      unsigned FiberId = 0;

      #ifndef NO_PK_PLATFORM
        //FiberId = Platform::Fiber::GetId() ;
        //FiberId = PkFiberGetId();
        FiberId = PkFiberGetNumber();
      #else
        #ifdef THREAD_SAFE
          pthread_t  tid = pthread_self() ; //& 0x0000FFFF);
          FiberId = tid & 0x0000FFFF;
        #endif
      #endif

      int SigFigFlag = 0;
      int ci = 0;
      for( int i = 0; i < 8 ; i++ )
        {
        if( FiberId & 0xF0000000 )
          SigFigFlag = 1;
        if( SigFigFlag )
          {
          mFiberName[ci] = itox[ (FiberId >> 28) & 0x0F ];
          ci++;
          }
        FiberId = FiberId << 4;
        }
      if( ci > 0 )
        mFiberName[ ci ] = '\0';
      else
        {
        // ... otherwise, it must be thread 0, right?
        mFiberName[0] = '0';
        mFiberName[1] = '\0';
        }
      }
  };

inline
char *
FxLogger_GetStartPointInFilePathName( char * aFN )
  {
  int starti = 0;

  // Find the end of the pathname
  int endi = 0; 
  // while( aFN[ endi ] != '\0' )
#define PKFXLOG_MAX_PATH 240

  while( endi < PKFXLOG_MAX_PATH && aFN[ endi ] != '\0' )
    {      
    endi++ ;
    }

  if( endi == PKFXLOG_MAX_PATH )
    FxLogger_Abort( "Unreasonably long file pathname" );

  #if defined( PKFXLOG_PATHNAME_DIRECTORIES_LIMIT )
    int slashcount = 0;
    for( int slashstart = endi; slashstart != 0; slashstart-- )
      {
      if( aFN[ slashstart ] ==  '/' )
         slashcount++;
      if( slashcount == PKFXLOG_PATHNAME_DIRECTORIES_LIMIT )
        {
        starti = slashstart;
        break;
        }
      }
  #endif

  #ifdef PKFXLOG_PATHNAME_SIZE_LIMIT
  if( starti > PKFXLOG_PATHNAME_SIZE_LIMIT )
    starti = endi - PKFXLOG_PATHNAME_SIZE_LIMIT;
  #endif

  return( & aFN[ starti ] );
  }

#ifdef PKFXLOG_DUMMY_OUT

    // For BlueLight bringup, logging is not available.
    // Dummy it out here.
    #define BegLogLine(MyLogMask) { \
        LogStream PkLog ;           \
        PkLog                       \

    #define EndLogLine              \
        0 ; }

    class LogStream
      {
      } ;

    template <class X> static inline LogStream& operator<< (LogStream& ls, const X& x)
      {
      return ls ;
      }

    static inline LogStream& operator<< (LogStream& ls, int x)
      {
      return ls ;
      }

#else // FXLOGGING not dummied out.

    // The following macro begins the definition of a log line.
    // This includes opening a '{ ... }' scope.
    // In theory, the 'if( mask stuff )' should drop the scope out if not used.
    // Every log line is includes the address space and thread number as well
    // as a time stamp so they can be merged and sorted.

#ifdef NDEBUG
enum {pkfxlog_debug = 0};
#else
enum {pkfxlog_debug = 1};
#endif

    #define __PKFXLOG_BASIC_LOG_LINE(__PKFXLOG_ABORT_FLAG)                                \
            {                                                                             \
            int _pkFxLogAbortFlag = __PKFXLOG_ABORT_FLAG;                                 \
            {                                                                             \
            FxLogger_TimeStamp        ts;                                                 \
            FxLogger_NodeName         an;                                                 \
            FxLogger_CoreName         ac;                                                 \
            FxLogger_FiberName        af;                                                 \
            LogStream              PkLog;                                                 \
            PkLog                                                                         \
                  << FormatString("%6d")   << ts.mSeconds                                 \
                                           << "."                                         \
                  << FormatString("%09d")  << ts.mNanoseconds                             \
                                           << " "                                         \
                                           << an.mNodeName                                \
                                           << " "                                         \
                                           << ac.mCoreName                                \
                                           << " "                                         \
                                           << af.mFiberName                               \
                                           << " >"

    #define BegLogLine(MyLogMask) \
      if( ( PkAnyLogging && MyLogMask ) || PKFXLOG_ALL )  \
        __PKFXLOG_BASIC_LOG_LINE(0)

    // Runtime check that is turned off by NDEBUG flag
    #define AssertLogLine(MyAssertCond)                       \
      if( ( pkfxlog_debug ) && ( ! (MyAssertCond) ) ) \
         __PKFXLOG_BASIC_LOG_LINE(1)                                        \
         << " FAILED ASSERT( " << #MyAssertCond << ") >"

#if defined(PK_STRONG_ASSERT_ONLY_TRAP)
//    // Runtime check that is not turned off by NDEBUG flag, convert it to just cause a core dump
//    #define StrongAssertLogLine(MyAssertCond)                       \
//      if( ! ((MyAssertCond) || (((*(int *)0)=0),1) )) \
//         __PKFXLOG_BASIC_LOG_LINE(1)                                        \
//         << " FAILED ASSERT( " << #MyAssertCond << " ) >"
    // Runtime check that is turned off by NDEBUG flag
    #define StrongAssertLogLine(MyAssertCond)                       \
      if( ( pkfxlog_debug ) && ( ! (MyAssertCond) ) ) \
         __PKFXLOG_BASIC_LOG_LINE(1)                                        \
         << " FAILED ASSERT( " << #MyAssertCond << ") >"

#else
    // Runtime check that is not turned off by NDEBUG flag
    #define StrongAssertLogLine(MyAssertCond)                       \
      if( ! (MyAssertCond) ) \
         __PKFXLOG_BASIC_LOG_LINE(1)                                        \
         << " FAILED ASSERT( " << #MyAssertCond << " ) >"

#endif

    #define EndLogLine    "<    "                                                      \
             << FxLogger_GetStartPointInFilePathName( __FILE__ )  << " "               \
             << __LINE__   << " "; }                                                   \
             if(_pkFxLogAbortFlag) __PK_FXLOG_SERIOUS_ABORT }

    class
    LogStreamBuffer
      {
      public:

        char* StrBuf;

        LogStreamBuffer()
          {
          // NEED TO MOVE THIS TO THE CORE CONTROL BLOCK
          // MAXBUF + 1 byte for null term + 64 bytes to keep off same L1 cache line
          // NOTE: This won't work for more than 4 cores
          static char StrBufSpace[ 4 ][ PKLOG_MAXBUF + 1 + 64 ] ;
          ///// int procId = a_hard_processor_id() & 3;
          int procId = rts_get_processor_id() & 1;
          StrBuf = StrBufSpace[ procId ];
          StrBuf[0] = 0;
          }

      } ;

    class SprintfStream
      {
      public:
        char * mBuffStart;
        char * mCurrentBufferPointer;
        int    mLength;
        int    mRemainingBufferSpace;

        char *mFormat;
        int   mFormatFlag;

        SprintfStream( char* aBuffer, int aLength )
          {
          mBuffStart            = aBuffer;
          mCurrentBufferPointer = aBuffer;
          mLength               = aLength;
          mRemainingBufferSpace = aLength; // for new line?
          mFormatFlag           = 0;
          }

        ~SprintfStream(){}

        template<typename T>
        inline void
        SprintfToBuffer( T aArg, char * aFmt)
          {
          if( mFormatFlag )
            {
            aFmt = mFormat;
            mFormatFlag = 0;
            }

          #ifdef PK_BLADE
            int rc = sprintf( mCurrentBufferPointer,
                              aFmt,
                              aArg );
          #else
            int rc = snprintf( mCurrentBufferPointer,
                               mRemainingBufferSpace,
                               aFmt,
                               aArg );
          #endif

          if( rc < 0 )
            FxLogger_Abort("snprintf failed");

          mCurrentBufferPointer += rc;
          mRemainingBufferSpace -= rc;
          return;
          }

        inline int GetBuffLength()
          {
          return ( mLength - mRemainingBufferSpace );
          }

        inline char* GetStartOfBuffer()
          {
          return mBuffStart;
          }

      };

   class FormatString
      {
      public:
        char *mFmtStr;
        FormatString( char *aFmtStr )
          {
          mFmtStr = aFmtStr;
          }
      };

    static SprintfStream& operator<< (SprintfStream& ls, FormatString fs )
      {
      ls.mFormat     = fs.mFmtStr;
      ls.mFormatFlag = 1;
      return ls;
      }

    static SprintfStream& operator<< (SprintfStream& ls, char x)
      {
      ls.SprintfToBuffer( x, "%c" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, double x)
      {
      #if defined(PK_BLADE) || defined(PKLOG_HEXREALS)
        int * ip = (int *) &x;
        ls.SprintfToBuffer( ":XD:","%s" );
        ls.SprintfToBuffer( ip[0], "%08X" );
        ls.SprintfToBuffer( ip[1], "%08X" );
      #else
        /////  BGF we need to regularly be looking at more sigfigs.  ls.SprintfToBuffer( x, "%f" );
        // 16.13f should leave space for full precission including decimal place and signe.
        ls.SprintfToBuffer( x, "% 16.13f" );
      #endif
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, float x)
      {
      #if defined(PK_BLADE) || defined(PKLOG_HEXREALS)
        int * ip = (int *) &x;
        ls.SprintfToBuffer( ":XD:","%s" );
        ls.SprintfToBuffer( ip[0], "%08X" );
      #else
        ls.SprintfToBuffer( x, "%f" );
      #endif
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, short x)
      {
      ls.SprintfToBuffer( x, "%d" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, int x)
      {
      ls.SprintfToBuffer( x, "%d" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, unsigned long long x)
      {
      ///ls.SprintfToBuffer( x, "%lld" );
      /// Mark G. says this is gonna work in AIX/XLC
      ls.SprintfToBuffer( x, "%llu" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, long long x)
      {
      ///ls.SprintfToBuffer( x, "%lld" );
      /// Mark G. says this is gonna work in AIX/XLC
      ls.SprintfToBuffer( x, "%Ld" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, unsigned int x)
      {
      ls.SprintfToBuffer( x, "%u" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, unsigned long x)
      {
      ls.SprintfToBuffer( x, "%u" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, void* x)
      {
      ls.SprintfToBuffer( x, "%08X" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, char* x)
      {
  if( strlen( x ) > ls.mRemainingBufferSpace )
    FxLogger_Abort( "SprintfStream& operator<< (SprintfStream& ls, char* x):  Log line would over run buffer" );

      ls.SprintfToBuffer( x, "%s" );
      return( ls );
      }

    static SprintfStream& operator<< (SprintfStream& ls, const char * x)
      {
      if( strlen( x ) > ls.mRemainingBufferSpace )
  FxLogger_Abort( "SprintfStream& operator<< (SprintfStream& ls, char* x):  Log line would over run buffer" );
      
      ls.SprintfToBuffer( x, "%s" );
      return( ls );
      }

    class
    LogStream : public LogStreamBuffer, public SprintfStream//ostrstream
      {
      public:
        LogStream()
          : SprintfStream( StrBuf, PKLOG_MAXBUF )
          {
          // Hook in the buffer to the ostrstream interface
          }

        ~LogStream()
          {
          // Get a char pointer to the oststream buffer so we can write it out.
          //char *cp = str();

          char *cp = GetStartOfBuffer();

          int bw = sprintf(mCurrentBufferPointer, "\n");
          mRemainingBufferSpace -= bw;

          // Check that we haven't over run the buffer - would be nice to check this earlier.

          if( GetBuffLength() >= PKLOG_MAXBUF  )
            FxLogger_Abort( "~LogStream() : Log line over ran buffer " );

          // Write this log line to log port.
          FxLogger_WriteBufferToLogPort( cp, GetBuffLength() );

          }

       };

#endif
#endif
