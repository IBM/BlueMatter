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
 #ifndef __PKTRACE_HPP__
#define __PKTRACE_HPP__

//#ifndef PK_BGL
//#include <pk/pktrace_pk.hpp>
//#else

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#if defined(PKTRACE_BGL_PERFCTR)
#include <pk/pktrace_perfctr.hpp>
#else
typedef long long LTime;
typedef long long smalltime;

// Trying to express things as 'trace metrics' ... any counter on the chip ... as a generalisation of 'time',
// which on any PowerPC chip is a 64-bit counter of bus cycles.
#define LTraceMetric LTime
#define smalltracemetric smalltime 
#endif

// extern "C" {
//     extern unsigned long long a_GetTimeBase(void);
// }

#ifdef PKTRACE_ON
#define PKFXLOG_TRACE_ON ( 1 )
#else
#define PKFXLOG_TRACE_ON ( 0 )
#endif

#ifndef PKFXLOG_TRACE_ADDHIT
#define PKFXLOG_TRACE_ADDHIT ( 0 ) 
#endif

#ifndef PKFXLOG_TRACE_CONNECT
#define PKFXLOG_TRACE_CONNECT ( 0 )
#endif

// #define TIME_STAMP_BUFFER_SIZE 512 * 512
// #define TIME_STAMP_BUFFER_SIZE 1
#define PKTRACE_NUMBER_OF_TIMESTAMPS 512*512*4
#define PKTRACE_NUMBER_OF_TRACE_NAMES 1024
#define MAX_TRACE_NAME         1022

// If we are recording 'performance counters', we may be taking several per trace point
enum {
	kTraceIdsPerTracePoint =
#if defined(PKTRACE_BGL_PERFCTR)
    PKTRACE_BGL_PERFCTR+1
#else
    1
#endif
} ;


extern  AtomicNative<unsigned short> GlobalTraceIdCounter[2];
extern Platform::Mutex::MutexWord* PKTRACE_Mutex;
extern Platform::Mutex::MutexWord* PKTRACE_TimeHit_Mutex;
extern Platform::Mutex::MutexWord* PKTRACE_Flush_Mutex;

struct TimeStampName
{
unsigned short mTraceId;
char mName[MAX_TRACE_NAME];
};

static void
InitTraceMutexes()
  {
#ifdef PK_BGL
  PKTRACE_Mutex = rts_allocate_mutex();
  PKTRACE_TimeHit_Mutex = rts_allocate_mutex();
  PKTRACE_Flush_Mutex = rts_allocate_mutex();
#endif
  }

static int
GetThreadId()
{
#ifdef PK_MPI_ALL
    return 0;
#else
    return Platform::Thread::GetId();
#endif
}


class pkTraceServer
{
public:
  static unsigned long long* mTimeStampsBuffer[ 2 ];//[ TIME_STAMP_BUFFER_SIZE ];

  static TimeStampName* mTimeStampNames[2];
  static AtomicNative<unsigned long> mHits[2];
  static AtomicNative<unsigned long> mNameCount[2];

  static
  void
  Init()
    {
      mHits[ 0 ].mVar = 0;
      mHits[ 1 ].mVar = 0;
      mNameCount[ 0 ].mVar = 0;
      mNameCount[ 1 ].mVar = 0;
    }

  static
  void
  FlushNames( int bufferId )
    {
      BegLogLine( 0 )
        << "FlushNames:: "
        << "mNameCount[ " << bufferId << " ].mVar=" << mNameCount[ bufferId ].mVar
        << EndLogLine;

      int gConfigured = 
#if defined(PKTRACE_BGL_PERFCTR)
         __pktrace_perfctr::counter_groups_configured    
#else
         0 
#endif  
       ;
#if defined(PKTRACE_BGL_PERFCTR)
      for( int i=0; i < mNameCount[ bufferId ].mVar; i++)
        {
        	if ( 0 == gConfigured )
        	{
	          BegLogLine( PKFXLOG_TRACE_ON )
	            << "PKTRACE ID "
	            << FormatString ("%04x")
	            << Platform::Topology::GetAddressSpaceId() << " "
	            << FormatString ("%04x")
	            << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1) )
	            << " " << mTimeStampNames[ bufferId ][ i ].mName
	            << EndLogLine;
        	} else {
	          BegLogLine( PKFXLOG_TRACE_ON )
	            << "PKTRACE ID "
	            << FormatString ("%04x")
	            << Platform::Topology::GetAddressSpaceId() << " "
	            << FormatString ("%04x")
	            << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1) )
	            << " " << mTimeStampNames[ bufferId ][ i ].mName
	            << ":TIME:0"
	            << EndLogLine;
	            int q=0 ;
	        	for ( int g=0; g<gConfigured;g+=1)
	        	{
	        		int gq = __pktrace_perfctr::counter_groups[g] ;
	        		switch (gq) {
	        			case 1: 
	              BegLogLine( PKFXLOG_TRACE_ON )
	                << "PKTRACE ID "
	                << FormatString ("%04x")
	                << Platform::Topology::GetAddressSpaceId() << " "
	                << FormatString ("%04x")
	                << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1)+q+1)
	                << " " << mTimeStampNames[ bufferId ][ i ].mName
	                << ":" << __pktrace_perfctr::counter_names[q]
	                << ":" << __pktrace_perfctr::counter_edges[q]
	                << EndLogLine;
	        			 break ;
	        			case 2:
	              BegLogLine( PKFXLOG_TRACE_ON )
	                << "PKTRACE ID "
	                << FormatString ("%04x")
	                << Platform::Topology::GetAddressSpaceId() << " "
	                << FormatString ("%04x")
	                << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1)+q+1)
	                << " " << mTimeStampNames[ bufferId ][ i ].mName
	                << ":" << __pktrace_perfctr::counter_names[q]
	                << "+" << __pktrace_perfctr::counter_names[q+1]
	                << ":" << __pktrace_perfctr::counter_edges[q]
	                << "+" << __pktrace_perfctr::counter_edges[q+1]
	                << EndLogLine;
	        			 break ;
	        			case 3: 
	              BegLogLine( PKFXLOG_TRACE_ON )
	                << "PKTRACE ID "
	                << FormatString ("%04x")
	                << Platform::Topology::GetAddressSpaceId() << " "
	                << FormatString ("%04x")
	                << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1)+q+1)
	                << " " << mTimeStampNames[ bufferId ][ i ].mName
	                << ":" << __pktrace_perfctr::counter_names[q]
	                << "+" << __pktrace_perfctr::counter_names[q+1]
	                << "+" << __pktrace_perfctr::counter_names[q+2]
	                << ":" << __pktrace_perfctr::counter_edges[q]
	                << "+" << __pktrace_perfctr::counter_edges[q+1]
	                << "+" << __pktrace_perfctr::counter_edges[q+2]
	                << EndLogLine;
	        			 break ;
	        			case 4:
	              BegLogLine( PKFXLOG_TRACE_ON )
	                << "PKTRACE ID "
	                << FormatString ("%04x")
	                << Platform::Topology::GetAddressSpaceId() << " "
	                << FormatString ("%04x")
	                << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1)+q+1)
	                << " " << mTimeStampNames[ bufferId ][ i ].mName
	                << ":" << __pktrace_perfctr::counter_names[q]
	                << "+" << __pktrace_perfctr::counter_names[q+1]
	                << "+" << __pktrace_perfctr::counter_names[q+2]
	                << "+" << __pktrace_perfctr::counter_names[q+3]
	                << ":" << __pktrace_perfctr::counter_edges[q]
	                << "+" << __pktrace_perfctr::counter_edges[q+1]
	                << "+" << __pktrace_perfctr::counter_edges[q+2]
	                << "+" << __pktrace_perfctr::counter_edges[q+3]
	                << EndLogLine;
	        			 break ;
	        			case 5: 
	              BegLogLine( PKFXLOG_TRACE_ON )
	                << "PKTRACE ID "
	                << FormatString ("%04x")
	                << Platform::Topology::GetAddressSpaceId() << " "
	                << FormatString ("%04x")
	                << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1)+q+1)
	                << " " << mTimeStampNames[ bufferId ][ i ].mName
	                << ":" << __pktrace_perfctr::counter_names[q]
	                << "+" << __pktrace_perfctr::counter_names[q+1]
	                << "+" << __pktrace_perfctr::counter_names[q+2]
	                << "+" << __pktrace_perfctr::counter_names[q+3]
	                << "+" << __pktrace_perfctr::counter_names[q+4]
	                << ":" << __pktrace_perfctr::counter_edges[q]
	                << "+" << __pktrace_perfctr::counter_edges[q+1]
	                << "+" << __pktrace_perfctr::counter_edges[q+2]
	                << "+" << __pktrace_perfctr::counter_edges[q+3]
	                << "+" << __pktrace_perfctr::counter_edges[q+4]
	      	        << EndLogLine;
	        			 break ;
	        			case 6: 
	              BegLogLine( PKFXLOG_TRACE_ON )
	                << "PKTRACE ID "
	                << FormatString ("%04x")
	                << Platform::Topology::GetAddressSpaceId() << " "
	                << FormatString ("%04x")
	                << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1)+q+1)
	                << " " << mTimeStampNames[ bufferId ][ i ].mName
	                << ":" << __pktrace_perfctr::counter_names[q]
	                << "+" << __pktrace_perfctr::counter_names[q+1]
	                << "+" << __pktrace_perfctr::counter_names[q+2]
	                << "+" << __pktrace_perfctr::counter_names[q+3]
	                << "+" << __pktrace_perfctr::counter_names[q+4]
	                << "+" << __pktrace_perfctr::counter_names[q+5]
	                << ":" << __pktrace_perfctr::counter_edges[q]
	                << "+" << __pktrace_perfctr::counter_edges[q+1]
	                << "+" << __pktrace_perfctr::counter_edges[q+2]
	                << "+" << __pktrace_perfctr::counter_edges[q+3]
	                << "+" << __pktrace_perfctr::counter_edges[q+4]
	                << "+" << __pktrace_perfctr::counter_edges[q+5]
	      	        << EndLogLine;
	        			 break ;
	        			default:
	              BegLogLine( PKFXLOG_TRACE_ON )
	                << "PKTRACE ID "
	                << FormatString ("%04x")
	                << Platform::Topology::GetAddressSpaceId() << " "
	                << FormatString ("%04x")
	                << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1)+q+1)
	                << " " << mTimeStampNames[ bufferId ][ i ].mName
	                << ":" << __pktrace_perfctr::counter_names[q]
	                << "+" << __pktrace_perfctr::counter_names[q+1]
	                << "+" << __pktrace_perfctr::counter_names[q+2]
	                << "+" << __pktrace_perfctr::counter_names[q+3]
	                << "+" << __pktrace_perfctr::counter_names[q+4]
	                << "+" << __pktrace_perfctr::counter_names[q+5]
	                << "+..."
	                << ":" << __pktrace_perfctr::counter_edges[q]
	                << "+" << __pktrace_perfctr::counter_edges[q+1]
	                << "+" << __pktrace_perfctr::counter_edges[q+2]
	                << "+" << __pktrace_perfctr::counter_edges[q+3]
	                << "+" << __pktrace_perfctr::counter_edges[q+4]
	                << "+" << __pktrace_perfctr::counter_edges[q+5]
	                << "+..."
	      	        << EndLogLine;
	        		} 
	        		
	        	
	        	    q += gq ; 
        	    }
            }
        }
#else
      for( int i=0; i < mNameCount[ bufferId ].mVar; i++)
        {
          BegLogLine( PKFXLOG_TRACE_ON )
            << "PKTRACE ID "
            << FormatString ("%04x")
            << Platform::Topology::GetAddressSpaceId() << " "
            << FormatString ("%04x")
            << (int) (mTimeStampNames[ bufferId ][ i ].mTraceId*(gConfigured+1) )
            << " " << mTimeStampNames[ bufferId ][ i ].mName
            << EndLogLine;
        }
        	
#endif  

      //BGF FlushNames must clear the names buffer or, the next time it's called, duplicates will be produced!
      mNameCount[ bufferId ].mVar = 0;

    }

  static
  void
  Reset()
    {
      mHits[ 0 ].mVar = 0;
      mHits[ 1 ].mVar = 0;
    }

  static
  void
  ResetAndFlushNames()
    {
      Reset();
      FlushNames(0);
      FlushNames(1);
    }

  static void
  FlushBuffer(int bufferId=-1)
    {
      int myProc = -1;
      if( bufferId == -1)
        myProc = GetThreadId();
      else
        myProc = bufferId;

      FlushNames( myProc );

      Platform::Mutex::YieldLock( PKTRACE_Flush_Mutex );
      int numberOfLogLines = ( 8 * mHits[ myProc ].mVar ) / 64;
      int remainderHits    = ( 8 * mHits[ myProc ].mVar ) % 64;

      for( int line=0; line < numberOfLogLines; line++)
        {
            char buff[ 128 ];

          // sprintf( buff, "%.64s", (char *)mTimeStampsBuffer[ bufferId ] + 512 * i);

            for (int tIndex=0; tIndex < 8 /*(64 / 8)*/ ; tIndex += 1)
                {
                    unsigned long long bytes8 = mTimeStampsBuffer[ bufferId ][ 8 * line + tIndex ] ;

                    char* EB = (char *) & bytes8;

                    int BaseIndex = tIndex*16;
                    for( int i=0; i < 8; i++)
                        {
                            buff[ BaseIndex + 2*i ]     = HexToChar[(EB[i] >> 4) & 0x0f ] ;
                            buff[ BaseIndex + (2*i + 1) ] = HexToChar[ EB[i]       & 0x0f ] ;
                        }
                } /* endfor */

          BegLogLine( PKFXLOG_TRACE_ON )
              << "PKTRACE DATA "
              << FormatString ("%04x")
              << Platform::Topology::GetAddressSpaceId() << " "
              << FormatString ("%.128s")
              << buff
              << EndLogLine;
        }

      if( remainderHits != 0 )
        {
            char buff[ 2 * 64 ];
            for (int tIndex=0; tIndex < (int) (remainderHits / 8 ); tIndex += 1)
                {
                    unsigned long long bytes8 = mTimeStampsBuffer[ bufferId ][ 8 * numberOfLogLines + tIndex ] ;

                    char* EB = (char *) & bytes8;

                    int BaseIndex = tIndex*16;
                    for( int i=0; i < 8; i++)
                        {
                            buff[ BaseIndex + 2*i ]     = HexToChar[(EB[i] >> 4) & 0x0f ] ;
                            buff[ BaseIndex + (2*i + 1) ] = HexToChar[ EB[i]       & 0x0f ] ;
                        }
                } /* endfor */

            buff[ remainderHits*2 ] = 0;

            BegLogLine( PKFXLOG_TRACE_ON )
                << "PKTRACE DATA "
                << FormatString ("%04x")
                << Platform::Topology::GetAddressSpaceId() << " "
                << FormatString ("%s")
                << buff
                << EndLogLine;
        }

      mHits[ myProc ].mVar = 0;
      Platform::Mutex::Unlock( PKTRACE_Flush_Mutex );
    }

  static void
  FlushAllBothBuffers()
    {
#ifdef PKTRACE_ON
      FlushBuffer( 0 );
      FlushBuffer( 1 );
#endif
    }

  static void
  AddHit(unsigned long long time, unsigned short aTraceId, int aTracePart=0, int aCountersConfigured=0)
    {
      int myProc = GetThreadId();

      if( mHits[ myProc ].mVar >= PKTRACE_NUMBER_OF_TIMESTAMPS )
        {
        printf( "WARNING: Flushing trace buffer because full... \n" );
        fflush( stdout );
        FlushBuffer( myProc );
        }

      // unsigned long long temp = 0x00ffffffffffffff;
      //

      mTimeStampsBuffer[ myProc ][ mHits[ myProc ].mVar ] = time;
      char* EB = (char *) & mTimeStampsBuffer[ myProc ][ mHits[ myProc ].mVar ];
      unsigned short reportedTraceId = aTraceId*(aCountersConfigured+1)+aTracePart ;
      char* traceId = (char *) & reportedTraceId;

      EB[0] = traceId[0];
      EB[1] = traceId[1];

//       char buff[ 16 ];
//       for( int i=0; i < 8; i++)
//           {
//               buff[ 2*i ]     = HexToChar[ (EB[i] >> 4) & 0x0f ] ;
//               buff[ 2*i + 1 ] = HexToChar[  EB[i]       & 0x0f ] ;
//           }


//       if( aTraceId == 1 )
//           {
//               BegLogLine( PKDEBUG )
//                   << "AddHit() "
//                   << " myProc=" << myProc
//                   << " traceId=" << (int) aTraceId
//                   << " mHits[ " << myProc << " ]=" << (int)mHits[ myProc ].mVar
//                   << " mTimeStampsBuffer[ " << myProc << "][" <<(int) mHits[ myProc ].mVar << "]="
//                   << FormatString ("%.16s")
//                   << buff
//                   << EndLogLine;
//           }

      mHits[ myProc ] ++;

      //
      BegLogLine(PKFXLOG_TRACE_ADDHIT)
        << "AddHit aTraceId=" << aTraceId
        << " aTracePart=" << aTracePart
        << " time=" << (long long) time
        << EndLogLine ;
    }
};

#define PKTRACE_TRACE_ID_INVALID  (0xFFFF)

class pkTraceStream
{
public:
  int mFirstEntry;
  unsigned short mTraceId;
  //BGF g++ added next line to stop using -1.
  //const unsigned short PKTRACE_TRACE_ID_INVALID = 0xFFFF;

  pkTraceStream()
    {
      mFirstEntry = 1;
      //BGF g++ mTraceId = -1;
      mTraceId = PKTRACE_TRACE_ID_INVALID ;
    }

  void init()
    {
      mFirstEntry = 1;
      //BGF g++ mTraceId = -1;
      mTraceId = PKTRACE_TRACE_ID_INVALID ;
    }

  unsigned short
  pkTraceStreamConnect( char     *SourceFileName,
                        int       SourceLineNo,
                        unsigned  Class,
                        char     *TraceStreamContext,
                        int       TraceStreamContextOrdinal,
                        char     *TraceStreamName )
    {

      char ComposedTraceName[ MAX_TRACE_NAME ];
      char TraceName[ MAX_TRACE_NAME ];

      if( TraceStreamContextOrdinal == -1 )
        {
          sprintf( ComposedTraceName, "%s:%s", TraceStreamContext, TraceStreamName );
        }
      else
        {
          sprintf( ComposedTraceName, "%s[%d]:%s",
                   TraceStreamContext,
                   TraceStreamContextOrdinal,
                   TraceStreamName );
        }


      char* ShortSourceFileName  = FxLogger_GetStartPointInFilePathName( SourceFileName );

      sprintf( TraceName, "%04d:%d:%s::%s[%04u]",
               Platform::Topology::GetAddressSpaceId(),
               GetThreadId(),
               ComposedTraceName,
               ShortSourceFileName,
               SourceLineNo);

      int nloop = 0;
      while(TraceName[nloop] != '\0')
        {
          if( TraceName[nloop] == '/' )
            TraceName[nloop] = '|';
          else if( TraceName[nloop] == ' ' )
            TraceName[nloop] = '_';

          nloop++;
        }

      //BGF g++ unsigned short traceId=-1;
      unsigned short traceId = PKTRACE_TRACE_ID_INVALID ;
      Platform::Mutex::YieldLock( PKTRACE_Mutex );
      int threadId = GetThreadId() ;
      traceId = GlobalTraceIdCounter[threadId].mVar+threadId;
#if defined(PK_BGL)      
      GlobalTraceIdCounter[threadId]++;
      GlobalTraceIdCounter[threadId]++;
#else
      GlobalTraceIdCounter[0]++ ;
#endif      
      Platform::Mutex::Unlock( PKTRACE_Mutex );


      int traceIndex = pkTraceServer::mNameCount[ threadId ].mVar;

      pkTraceServer::mTimeStampNames[ threadId ][ traceIndex ].mTraceId = traceId;
      sprintf( pkTraceServer::mTimeStampNames[ threadId ][ traceIndex ].mName, "%s", TraceName );

      pkTraceServer::mNameCount[ threadId ].mVar++;

//       int myRank = BGLPartitionGetRankXYZ();
       BegLogLine( PKFXLOG_TRACE_CONNECT )
         << "pkTraceStreamConnect "
         << FormatString ("%04x")
         << Platform::Topology::GetAddressSpaceId() << " "
         << FormatString ("%04x")
         << (int) traceId
         << " " << TraceName
         << EndLogLine;

      return traceId;
    }

  inline void pkTraceStreamSourceDummy(void) { } 
#if !PKFXLOG_TRACE_ON
// With the 'function' version, spurious calls to rts_get_processor_id were happening for TpOrdinal
#define pkTraceStreamSource(LineNo,FileName,TpFlag,TpContext,TpOrdinal,TpName) pkTraceStreamSourceDummy() 

//  inline void
//  pkTraceStreamSource( int       LineNo,
//                       char     *FileName,
//                       unsigned  TpFlag,
//                       char     *TpContext,
//                       int       TpOrdinal,
//                       char     *TpName )
//    {
//      return;
//    }
#else
  inline void
  pkTraceStreamSource( int       LineNo,
                       char     *FileName,
                       unsigned  TpFlag,
                       char     *TpContext,
                       int       TpOrdinal,
                       char     *TpName )
    {
      // pkTraceServer* pServer;

      //BGF: MAY 98 dunno if this is required, but i can find no otherway to turn
      //BGF  trace points off.
      //BGF: Might want to mask TpClass against something set at runtime here.

      if( ! TpFlag )
        return;


////////        // Get the time
////////        // read_real_time( &CurrentTimeStamp, TIMEBASE_SZ );
////////        #if defined( PK_BLADE_SPI )
////////          // unsigned long long CurrentTimeStamp = a_GetTimeBase();
////////          // s0printf("TRACING:: About to call Platform::Clock::GetBladeElapsedTime();\n");
////////          double eTime = Platform::Clock::GetElapsedTime();
////////          unsigned long long CurrentTimeStamp = Platform::Clock::ConvertDoubleToNanoseconds( eTime );
//////////        #elif defined( PK_MPI_ALL )
////////////           timebasestruct_t  CurrentTimeStamp1;
////////////           read_real_time( &CurrentTimeStamp1, TIMEBASE_SZ );
////////////           time_base_to_time( &CurrentTimeStamp1, TIMEBASE_SZ );
////////////           unsigned long long seconds = CurrentTimeStamp1.tb_high;
////////////           unsigned long long nanoseconds = CurrentTimeStamp1.tb_low;
////////////           unsigned long long CurrentTimeStamp = (seconds * 1000 * 1000 * 1000) + nanoseconds;
//////////          double eTime = MPI_Wtime();
//////////          unsigned long long CurrentTimeStamp = Platform::Clock::ConvertDoubleToNanoseconds( eTime );
////////        #else
////////        #endif

        // First call to function, do connect
        if ( mFirstEntry )
          {
            Platform::Mutex::YieldLock( PKTRACE_TimeHit_Mutex );
            Platform::Memory::ImportFence( (void *)& mFirstEntry, sizeof(int));
            if( mFirstEntry )
              {
                unsigned short traceId = pkTraceStreamConnect( FileName, LineNo, TpFlag,
                                                               TpContext, TpOrdinal, TpName );
                if(traceId != -1)
                  {
                    mTraceId = traceId;
                  }
                else
                  PLATFORM_ABORT("traceId != -1");

                Platform::Memory::ExportFence();
                mFirstEntry = 0;
                Platform::Memory::ExportFence();
              }
            Platform::Mutex::Unlock( PKTRACE_TimeHit_Mutex );
          }
          double OscillatorTime = Platform::Clock::Oscillator() ;
          double NanosecondTime = OscillatorTime * (1.0 / 0.7) ;
          long long ReportedTime = NanosecondTime ;
          
#if defined(PKTRACE_BGL_PERFCTR)
//          int cConfigured =  __pktrace_perfctr::counters_configured  ;
          int gConfigured = __pktrace_perfctr::counter_groups_configured ;
          pkTraceServer::AddHit(ReportedTime,mTraceId,0,gConfigured) ;
//          if ( cConfigured > 0 )
          if ( gConfigured > 0 )
          {
	          __pktrace_perfctr_snap Snapshot ;
	                    
//	          for ( int q=0;q<cConfigured;q+=1)
//	          {
//	          	 pkTraceServer::AddHit( Snapshot.Metric(q),
//	          	                        mTraceId,
//	          	                        q+1,
//	          	                        cConfigured) ;
//	          }
              int counterIndex = 0 ; 
              for ( int g=0;g<gConfigured; g+=1)
              {
              	int gq = __pktrace_perfctr::counter_groups[g] ;
              	int counterSum = Snapshot.Metric(counterIndex) ;
              	for ( int c=1;c<gq;c+=1) 
              	{
              		counterSum += Snapshot.Metric(counterIndex+c) ;
              	}
              	counterIndex += gq ;
              	pkTraceServer::AddHit(counterSum,mTraceId,g+1,gConfigured) ;
              }
          } 
#else          
          pkTraceServer::AddHit(ReportedTime,mTraceId,0,0) ;
#endif          
    }
#endif
};

// #define PkTraceStreamSourceCON(TraceStreamFlag,TSContext,TSOrdinal,TSName)  \
//    pkTraceStreamSource(__LINE__,__FILE__,TraceStreamFlag,TSContext,TSOrdinal,TSName)

#define HitOE(Flag, ContextName, Ordinal, EventName) \
  pkTraceStreamSource(__LINE__,__FILE__,Flag, ContextName, Ordinal, #EventName)

#define TraceClient pkTraceStream

//#endif

#endif /* __PKTRACE_HPP__ */
