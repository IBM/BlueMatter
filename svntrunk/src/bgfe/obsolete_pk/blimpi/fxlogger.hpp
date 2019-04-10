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
 *
 ***************************************************************************/
#ifndef __PKFXLOG_HPP__
#define __PKFXLOG_HPP__

extern int       LogLockVar;
extern int       LogFd;
extern long long LogBaseTime;
extern unsigned  LogMask;
extern unsigned  LogMask;

#include "BladeMpi.hpp"
#include <unistd.h>

#ifndef PKFXLOG_ALL
#define PKFXLOG_ALL 0
#endif

#include <stdio.h>
#include <string.h>
#include <sys/signal.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#if !defined(PK_BGL_UNI)
// For other than BGL bringup, we use strstreams to support functional logging
#include <strstream.h>
#include <iomanip.h>
#endif

#ifdef NO_PK_PLATFORM

// This ifdef block resolves the fx logger stuff without dependency
// on pk platform stuff. No need for external globals.
// HOWEVER, if you include this file with <pk/fxlogger.hpp> you
// will need to move this file into a directory called pk/
// Using #include <pk/fxlogger.hpp> is probably a good idea if you
// will later moving the code into a build that uses pk/platform.

// NOTE:  YOU MUST SET -D NO_PK_PLATFORM to avoid the platform depenency.
//        LOG LINES WILL GO TO 'cerr'

#define BegLogLine(MyLogMask) if( (MyLogMask) || PKFXLOG_ALL )  { cerr << " >"

#define EndLogLine                                                   "<     " \
        << __FILE__                                                   << " " \
        << __LINE__                                                   << " " \
        << endl ; }    // This terminates the scope of the above macro.

#else
// *************** DO THE REAL PK LOGGER STUFF ************************

////#include <pk/platform.hpp>

extern int                   LogLockVar;
extern unsigned              LogMask;

//NEED: this perhaps default to packet size.  Or be drawn from platform spec
#define PKLOG_MAXBUF (1024)

enum {
  PkAnyLogging =
#if defined(PKFXLOG)
   1
#else
   0
#endif
} ;

#if defined(PK_BGL_UNI)
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

#else
// The following macro begins the definition of a log line.
// This includes opening a '{ ... }' scope.
// In theory, the 'if( mask stuff )' should drop the scope out if not used.
// Every log line is includes the address space and thread number as well
// as a time stamp so they can be merged and sorted.

static
inline
double
BlimpiGetElapsedTime()
  {
          timebasestruct_t Now;
          read_real_time( &Now, TIMEBASE_SZ );
          time_base_to_time( &Now, TIMEBASE_SZ );
          int Seconds      = Now.tb_high;
          int Nanoseconds  = Now.tb_low ;
          double rc = Nanoseconds * 0.000000001 + Seconds;
          return( rc );
  }

static
inline
int
BlimpiGetId()
  {
  int        ThreadId = (((int)pthread_self()) % 0x0000FFFF); //////////////// & 0x0000FFFF);
  return( ThreadId );
  }


inline
int
GetMyXCoord()
  {
  int x, y, z;
  BGLPartitionGetCoords( &x, &y, &z );
  return( x );
  }
inline
int
GetMyYCoord()
  {
  int x, y, z;
  BGLPartitionGetCoords( &x, &y, &z );
  return( y );
  }
inline
int
GetMyZCoord()
  {
  int x, y, z;
  BGLPartitionGetCoords( &x, &y, &z );
  return( z );
  }

#define BegLogLine(MyLogMask) if( PkAnyLogging && ( ( MyLogMask & LogMask ) || PKFXLOG_ALL ) )  {   \
        LogStream PkLog;                                                        \
        PkLog.setf(ios::fixed,ios::floatfield);                                 \
        PkLog                                                                   \
        << setw(12) << setprecision(6)                                          \
        << BlimpiGetElapsedTime()                          << " "    \
        << setw( 4 )                                                            \
        << BlimpiGetId() << " "                                     \
        << setw( 4 )                                                            \
        << "[" << GetMyXCoord() <<","<<GetMyYCoord()<<","<<GetMyZCoord()<< "] "                       \
        << " >"

#define EndLogLine                                                  "<     " \
        << __FILE__                                                   << " " \
        << __LINE__                                                   << " " \
        << endl ; }    // This terminates the scope of the above macro.


//NEED: To think through why this is a new delete...
//      I think it was on the stack, then remained
//      in scope all the time.
//      Maybe malloca ? (stack malloc?)
class
LogStreamBuffer
  {
  public:
      char *StrBuf;
      LogStreamBuffer()
        {
        StrBuf = new char [ PKLOG_MAXBUF + 1 ];
        if( StrBuf == NULL )
          {
          cerr << " FATAL ERROR IN FXLOGGER - could not allocate buffer " << endl;
          cerr << " FATAL ERROR IN FXLOGGER - could not allocate buffer " << endl;
          cerr << " FATAL ERROR IN FXLOGGER - could not allocate buffer " << endl;
          kill( getpid(), SIGTERM );
          assert( StrBuf != NULL );
          exit( -1 );
          }
        StrBuf[0] = 0;
        }

      ~LogStreamBuffer()
        {
        assert( StrBuf != NULL );
        delete[] StrBuf;
        }
  } ;

class
LogStream : public LogStreamBuffer, public ostrstream
  {
  public:
    LogStream()
      : ostrstream( StrBuf, PKLOG_MAXBUF, 0 )
      {
      // Hook in the buffer to the ostrstream interface
      }

    ~LogStream()
      {
      // Get a char pointer to the oststream buffer so we can write it out.
      char *cp = str();
      // Check that we haven't over run the buffer - would be nice to check this earlier.
      assert( pcount() < PKLOG_MAXBUF  );
      // Write this log line to log port.
      //////Platform::FxLogger::WriteBufferToLogPort( cp, pcount() );
if( LogFd < 0 )
  {

      int open_perms = S_IRUSR | S_IWUSR ;  //AIXism
      char LogFileName[1024];
      int x, y, z;
      BGLPartitionGetCoords( &x, &y, &z );
      sprintf( LogFileName, "LOG.%d.%d.%d", x, y, z );
      int fd = open( LogFileName ,
                    O_CREAT | O_TRUNC | O_WRONLY | O_APPEND,  // O_APPEND Required to ensure proper writes by threaded apps
                    open_perms );

      assert( fd > 0 );  // Log file failed open.
      if( LogFd < 0 )
        {
        LogFd = fd;  // RACE COND!!!!
        if( LogFd != fd )
          close( fd );
        fd = LogFd;
        }
      assert( LogFd == fd );
  }

::write( LogFd, cp, pcount() );
      // Note that buffer decommit should occur during destruction of LogStreamBuffer
      }

  };
#endif
#endif
// end of DO THE REAL PK LOGGER STUFF

// This set of macro enables dumping words out as hex.  Might be a better way.

#define LogHexDump1(Thing)    " 0x" << hex << setw(8) << ((int *)Thing)[0] << " "
#define LogHexDump2(Thing)    " 0x" << hex << setw(8) << ((int *)Thing)[0] << " " \
                           << " 0x" << hex << setw(8) << ((int *)Thing)[1]
#define LogHexDump3(Thing)    " 0x" << hex << setw(8) << ((int *)Thing)[0] << " " \
                           << " 0x" << hex << setw(8) << ((int *)Thing)[1] << " " \
                           << " 0x" << hex << setw(8) << ((int *)Thing)[2] << " "
#define LogHexDump4(Thing)    " 0x" << hex << setw(8) << ((int *)Thing)[0] << " " \
                           << " 0x" << hex << setw(8) << ((int *)Thing)[1] << " " \
                           << " 0x" << hex << setw(8) << ((int *)Thing)[2] << " " \
                           << " 0x" << hex << setw(8) << ((int *)Thing)[3]


#endif
// end of block of fuctions.
