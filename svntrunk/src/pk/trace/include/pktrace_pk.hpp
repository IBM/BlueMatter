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
 * Module:          pktrace.hpp
 *
 * Purpose:         provide multi source point trace streams.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         090497 BGF Created.
 *
 *
 ***************************************************************************/

#ifndef __PKTRACE_PK_HPP__
#define __PKTRACE_PK_HPP__

#include <assert.h>

#ifndef PKFXLOG_TRACESTREAM
#define PKFXLOG_TRACESTREAM (0) 
#endif

#ifndef TRACE_DEST_PK
#define TRACE_DEST_PK
#endif

#ifndef PK_MAP_TRACE_DIR
#define PK_MAP_TRACE_DIR "./Maptrc"
#endif

#ifndef OUT_OF_SPACE
#define OUT_OF_SPACE -2
#endif

#ifndef MAX_FILENAME
#define MAX_FILENAME 1024
#endif

#ifndef MAX_TRACES
#define MAX_TRACES (84*1024)
#endif

#ifndef MAP_FILE_SIZE
#define MAP_FILE_SIZE (256 * 1024 * 1024) // 256Mb mapped file space
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef HIGHER
#define HIGHER 2
#endif

#ifndef LOWER
#define LOWER 0
#endif

#ifndef PKTRACE_MAGIC_NUMBER
#define PKTRACE_MAGIC_NUMBER 102
#endif

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

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
//#include <assert.h>
#include <locale.h>
#include <fcntl.h>
//#include <assert.h>

#include <time.h>
extern "C" {
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>
}

#include <regex.h>
#include <fnmatch.h>
//#include <pthread.h>

#ifndef MAP_FILE
#define MAP_FILE
#endif

#ifndef FILE_MODE
#define FILE_MODE ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )
#endif

#ifdef _THREAD_SAFE
//#include <pk/pthread.hpp>
#endif

// should have the trace points register with a global server.
// This will allow controlling trace points with class and number
// values.

#if PK_LINUX
#ifndef TIMEBASE_SZ  // LINUX AIX COMMON
#define TIMEBASE_SZ (1)
typedef struct {unsigned tb_high; unsigned tb_low; } timebasestruct_t;
#endif
#endif
// #include <pk/time.hpp>
#include <pk/fxlogger.hpp>

extern const char * pkTraceFileName ;

inline smalltime TbToSmalltime( timebasestruct_t tb)
{
   return ( *((long long*)(&(tb.tb_high))));
}

// Structure containing the trace entry data
struct TraceData
{
      char TraceName[ MAX_FILENAME ];
      int  CurrentOffset ;
      int  TotalNumBlocks;

};

//// enum enLockType {LOCK, UNLOCK};  ///  SHOULD INCLUDE yieldlock.hpp

// #include <pk/yieldlock.hpp>
enum YieldLockOpCode { YieldLockOpCode_LOCK, YieldLockOpCode_UNLOCK };


class YieldLockable
{
  public:
  static void YieldLockOp(YieldLockOpCode Request)
  {
// Not releasing for (SMP) environments which support yield locks    
//    static Platform::Mutex::MutexWord LockVar;
//
//    if (Request == YieldLockOpCode_LOCK)
//      Platform::Mutex::YieldLock( &LockVar );
//    else
//      Platform::Mutex::Unlock( &LockVar );
  }
};

#ifdef PK_LINUX
#ifndef NBPG // going after LINUX AIX COMMON
#define NBPG (4096) //AIXism... number of bytes per page
#endif
#endif

#ifndef HITS_PER_BLOCK
#define HITS_PER_BLOCK ( ( NBPG - (2* ((int)sizeof(int)))) /((int)sizeof(smalltime)))
#endif

struct MappedBlock {
   smalltime TimeStamp[HITS_PER_BLOCK];
   int       PaddingInt;  // Not used
   int       NextBlockOffset;
};

struct MapFileHeader
{
      int               MagicNumber;

      int               NumTraces;
      int               MappedBlocksInFile;

      // The last allocated block number
      int               LastBlockOffset;

      // Time values to assist conversion later
      smalltime         RawStartTime;
      double            ConversionFactor;

      TraceData         TraceDataMatrix[ MAX_TRACES ];

      // After collection data manipulation flags
      int               SortedFlag;
      int               FileResizedFlag;

      // Starting position for the blocks of hit data
      MappedBlock*       StartingBlock;
};

//static int QsortHeaderComp(const void* Data1 , const void* Data2 );

/*******************************************************************************
*
* Class:   __pkTraceServer
* Purpose: Storage of trace points in a single mapped file
*
*******************************************************************************/
class __pkTraceServer : public YieldLockable
{
  public:
      char            MappedFileName[MAX_FILENAME];

  private:
    pthread_mutex_t   *mLock;
    int             Fd;

  public:
    MapFileHeader  *FileHeader;
    smalltime LTimeToSmalltime(LTime lt);
    LTime     SmalltimeToLTime(smalltime st);

  private:
    void      SetStartingBlockAddress();
  //__pkTraceServer();
    __pkTraceServer(int taskId);
    __pkTraceServer(char* FileName);

  public:
///////////////////
  void SetRawStartTime( unsigned long long aEarlyTime )
    {
    FileHeader->RawStartTime = aEarlyTime;
    }

  void ZeroConversionFactor( )
    {
    FileHeader->ConversionFactor = 0;
    }
//////////////////

    ~__pkTraceServer();

    static    __pkTraceServer* Load(char* MappedFile);

    void Destroy();

    static    __pkTraceServer* GetIF(char* FileName = NULL, int   Kill = FALSE);
    static    __pkTraceServer* LockIF( int taskId );
    void      UnLockIF();

    int       InitNewStream(char* StreamName, smalltime** ptrNewTime,
                             smalltime** ptrLastHitInBlock);

    int       GetNewBlock(smalltime** ptrNewTimeAddress,
                          smalltime** ptrLastHitInBlock,
                          int         TraceIndex = -1);

    LTime     GetLTimeFromBC(smalltime st);
    smalltime GetBCFromLTime(LTime lt);
    void      DisplayTraceInfo(int Index);

    void      SetFileSize(int NumBlocks);
};

/*******************************************************************************
*
* Class:   __pkTraceStream
* Purpose: Interface with application code to store trace points
*
*******************************************************************************/

class __pkTraceStream
{
  private:
    int MapIndex;
    MappedBlock *CurrentBlock;

    smalltime* ptrCurrentHit;
    smalltime* ptrLastHitInBlock;

    void AllocNewBlock( __pkTraceServer* pServer);

  public:
    __pkTraceStream();

    ~__pkTraceStream();

    int
    __pkTraceStreamConnect( char     *SourceFileName,
                            int       SourceLineNo,
                            unsigned  Class,
                            char     *TraceStreamContext,
                            int       TraceStreamContextOrdinal,
                            char     *TraceStreamName );


////////// BEGIN BLOCK ADDED FOR FXLOG TRACING //////////////////////////////////////////////////////////
   int
   __pkTraceStreamConnect( char     *TraceStreamName );

   void
   AddHitToTraceStream( long long aTime )
     {
     __pkTraceServer* pServer;
     BegLogLine(PKFXLOG_TRACESTREAM)
       << "AddHitToTraceStream aTime=" << (int) aTime 
       << " ptrCurrentHit=" << ptrCurrentHit
       << " ptrLastHitInBlock=" << ptrLastHitInBlock
       << EndLogLine ;
     // First call to function, do connect
     if ((ptrCurrentHit == (smalltime*)0x00000001) &&
         (ptrLastHitInBlock == (smalltime*)0x00000001))
       {
       // should be connected.
       }
     else if (ptrCurrentHit == ptrLastHitInBlock)
       {
       // Check for end of block
       pServer =  __pkTraceServer::LockIF( 0 );  // shouldn't need to lock
       assert( pServer != NULL );
       BegLogLine(PKFXLOG_TRACESTREAM)
        << "Allocating new block" 
        << EndLogLine ;
       AllocNewBlock(pServer);
       pServer->UnLockIF();
       }

     // Just add the hit
     *ptrCurrentHit = aTime;
     ptrCurrentHit++;
     }

////////// END BLOCK ADDED FOR FXLOG TRACING //////////////////////////////////////////////////////////

#ifndef PKTRACE
    inline void
    __pkTraceStreamSource( int       LineNo,
                           char     *FileName,
                           unsigned  TpClass,
                           char     *TpContext,
                           int       TpOrdinal,
                           char     *TpName )
      {
      return;
      }
#else
    inline void
    __pkTraceStreamSource( int       LineNo,
                           char     *FileName,
                           unsigned  TpFlag,
                           char     *TpContext,
                           int       TpOrdinal,
                           char     *TpName )
      {
        __pkTraceServer* pServer;

        //BGF: MAY 98 dunno if this is required, but i can find no otherway to turn
        //BGF  trace points off.
        //BGF: Might want to mask TpClass against something set at runtime here.

        if( ! TpFlag )
            return;

#if !defined(PKTRACE_BGL_PERFCTR)
        timebasestruct_t CurrentTimeStamp;

        // Get the time
        read_real_time( &CurrentTimeStamp, TIMEBASE_SZ );
#endif

        // First call to function, do connect
        if ((ptrCurrentHit == (smalltime*)0x00000001) &&
            (ptrLastHitInBlock == (smalltime*)0x00000001))
        {
          __pkTraceStreamConnect( FileName, LineNo, TpFlag,
                                  TpContext, TpOrdinal, TpName );
        }
        else if (ptrCurrentHit == ptrLastHitInBlock)
        {
        // Check for end of block
          pServer =  __pkTraceServer::LockIF( TpOrdinal );
          assert( pServer != NULL );
          AllocNewBlock(pServer);
          pServer->UnLockIF();
        }

        // Just add the hit
#if defined(PKTRACE_BGL_PERFCTR)
        *ptrCurrentHit = TbToSmalltime(CurrentTimeStamp);
#else
	*ptrCurrentHit = __pktrace_perfctr::CurrentTraceMetric() ;
#endif
        ptrCurrentHit++;

      }
#endif

  };

/*******************************************************************************
*
* Structure:   __pktraceLoadBlock
* Purpose: Structure to store summary of block data when loading traces
*
*******************************************************************************/
struct __pktraceLoadBlock
{
    MappedBlock *pServerBlock;
//    int       BlockNumber;
    int       NumHitsInBlock;
    smalltime Starttime;
    smalltime Endtime;
};

/*******************************************************************************
*
* Class:   __pktraceLoadedTrace
* Purpose: Class to fill the local block data summarys
*
*******************************************************************************/
class __pktraceLoadedTrace
{
  public:
    int SearchRangeLower;
    int SearchRangeUpper;

    int  NumHits;
    int  NumBlocks;
    __pktraceLoadBlock  *Trace;
    __pkTraceServer* ClassServer;

    __pktraceLoadedTrace(int Index, __pkTraceServer* NewServer);
    inline ~__pktraceLoadedTrace() { }
    int BinarySearchBlocks( smalltime TargetTime );
    int IsHitInRange( LTime TargetTime , LTime ScaleValue = 0);
    void SetSearchBounds(LTime LowerTargetTime, LTime UpperTargetTime );
    int SearchForBlock( LTime TargetTime );
    int FindHitPos(LTime TargetTime);

    LTime GetStartTime();
    LTime GetEndTime();

    LTime GetLTimeAtPosition(int Position);
    smalltime GetsmalltimeAtPosition(int Position);

    int FindEarliestVisibleHit (LTime TargetValue);
};

/*******************************************************************************
*
* Class:   __pktraceRead
* Purpose: Class that reads in the trace data for delivery to the GUI
*
*******************************************************************************/
class __pktraceRead
{

  private:
    int NumLoadedTraces;
    char TraceMappedFile[1024];
    __pkTraceServer *ClassServer;

  public:
    __pktraceLoadedTrace* LoadedTraces[MAX_TRACES];


     // Constructor
     __pktraceRead(char* Filename);
    ~__pktraceRead();

    int IsValidMagicNum();

    void LoadAllTraceData();
    void LoadTraceData(int Position);

    int GetNumTraces();

    char* GetTraceName(int Position);

    LTime GetRunStartTime();

};

// Landing in this macro means that a trace stream source has been crossed
// with no prior declairation. This is legal, but slow.  We'll create the
// stream source instance, use it and let it go out of scope in this routine.
// A warning is printed once.

inline void
__pkTraceStreamSource( int LineNo, char *FileName, unsigned TpFlag,
                       char *TpContext, int TpOrdinal, char *TpName )
  {
  static int entry = 0;
  if( entry == 0 )
    {
    }
  __pkTraceStream DummyStream;
  DummyStream.__pkTraceStreamSource( LineNo, FileName,
                                     TpFlag, TpContext, TpOrdinal, TpName );
  return;
  }


// DclAndSource declairs the trace source in current scope... if this scope
// is closed, destructors will be called on the Trace Source.  If this
// scope is reopened later, the trace source will have to be reconnected.


//Sadly, the following just doesn't seem to work...
//#define PkTraceStreamDclAndSource(TraceStreamName,TraceStreamClass) __pkTraceStream XXX__LINE__; XXX__LINE__.PkTraceStreamSource(TraceStreamName,TraceSteamClass)

// ... so, we do the following.

#define PkTraceStreamDclAndSource(TraceStreamName,TraceStreamClass) \
                   PkTraceStreamSourceN(TraceStreamClass,TraceStreamName)

#define PkTraceStreamDcl  __pkTraceStream

#define PkTraceStreamSource(TraceStreamClass)  \
        __pkTraceStreamSource(__LINE__,__FILE__,TraceStreamClass,"",-1,"")

#define PkTraceStreamSourceN(TraceStreamClass,TSName)  \
        __pkTraceStreamSource(__LINE__,__FILE__,TraceStreamClass,"",-1,TSName)

#define PkTraceStreamSourceCN(TraceStreamClass,TSContext,TSName)  \
   __pkTraceStreamSource(__LINE__,__FILE__,TraceStreamClass,TSContext,-1,TSName)

#define PkTraceStreamSourceCON(TraceStreamFlag,TSContext,TSOrdinal,TSName)  \
   __pkTraceStreamSource(__LINE__,__FILE__,TraceStreamFlag,TSContext,TSOrdinal,TSName)


// Mappings for the 'converged' trace interface ...
#define TraceContext PkTraceStreamDcl

#define TracePointCOI(Flag, Name, Context, Ordinal, Integer) \
      PkTraceStreamSourceCON(Flag, #Context, Ordinal, #Name)
#define TracePointCO(Flag, Name, Context, Ordinal) \
      PkTraceStreamSourceCON(Flag, #Context, Ordinal, #Name)

#define TraceClient PkTraceStreamDcl

#define HitOE(Flag, ContextName, Ordinal, EventName) \
  PkTraceStreamSourceCON(Flag, ContextName, Ordinal, #EventName)

#define HitOEI(Flag,Class,ContextName,Ordinal,EventName,Integer) \
  PkTraceStreamSourceCON(Flag, ContextName, Ordinal, #EventName)



int       FindHitInBlock(__pktraceLoadBlock* BlockAddress,
                         smalltime           TargetTime);

int       BinarySearch(__pktraceLoadBlock* BlockAddress,
                       smalltime           TargetTime );

#endif
