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
 * Module:          pktrace_pk.cpp
 *
 * Purpose:
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         090497 BGF Created.
 *
 ***************************************************************************/

// Don't build this for the BG/L kernel
#if !defined(PK_BLADE)

#define NO_PK_PLATFORM
#include <pk/fxlogger.hpp>
#include <pk/pktrace_pk.hpp>

#if !defined(PKFXLOG_PKTRACE_PK)
#define PKFXLOG_PKTRACE_PK (0) 
#endif

int QsortHeaderComp(const void* Data1 , const void* Data2 )
{
   return strcmp(((const TraceData*)(Data1))->TraceName,
                 ((const TraceData*)(Data2))->TraceName);
}

__pkTraceServer::__pkTraceServer( int taskId )
{
  timebasestruct_t InitStartTime;
  double ConvtbStartTime;
  char achTime[16];
  time_t FileTime;
  struct tm* ConvTime;

#if !defined(PK_BLADE)

  // Try creating a directory
  mkdir( PK_MAP_TRACE_DIR, S_IRUSR | S_IWUSR | S_IXUSR );

  // Allocate thread
  // This style of tracing won't work on BG/L or its simulator becuase
  // of no mapped files, etc.; but should all be discarded at link-edit
  // if there are no trace calls
#if !defined(PK_POSIX)
  mLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mLock,NULL);
#endif

  assert(time(&FileTime) != -1);
  ConvTime = localtime(&FileTime);

  strftime(achTime,15,"%d%b%y.%H%M%S",ConvTime);

  // Create the file name
  sprintf(MappedFileName,"%s/TrcRun%s.%d.p%d",PK_MAP_TRACE_DIR,achTime, taskId, getpid());

  printf("%s\n", MappedFileName);
  fflush(stdout);

  Fd = open( MappedFileName, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR );

  if( Fd < 0 )
    printf("Error errno %d is: %s\n", errno, strerror( errno ) );

  // Check the file settings
  assert( Fd >= 0 );

  if(lseek(Fd, MAP_FILE_SIZE - 1, SEEK_SET) == -1)
    {
    assert(0);
    }

  if(write(Fd,"",1) != 1)
    {
    assert(0);
    }

  // map the file
  FileHeader = (MapFileHeader*)mmap(0, MAP_FILE_SIZE,
                                     PROT_READ | PROT_WRITE, MAP_SHARED, Fd, 0);

  // Check file was mapped OK
  assert((caddr_t)FileHeader != (caddr_t)-1);
#else
#endif


  // Set the magic number for file id
  FileHeader->MagicNumber = PKTRACE_MAGIC_NUMBER;

  // Initialize the header information
  FileHeader->NumTraces = 0;

  FileHeader->MappedBlocksInFile =
            (MAP_FILE_SIZE - sizeof(struct MapFileHeader))/ sizeof(MappedBlock);


  // Set up the start times
  read_real_time( &InitStartTime, TIMEBASE_SZ );
  FileHeader->RawStartTime = TbToSmalltime(InitStartTime);

  // Convert the time
  // Get the current time
  time_base_to_time(&InitStartTime,TIMEBASE_SZ);

  // Convert the captured time to LTime
  ConvtbStartTime = (((long long) InitStartTime.tb_high) *
                                  (long long)1000000000) + InitStartTime.tb_low;

  // Calculate the conversion
  FileHeader->ConversionFactor = ConvtbStartTime/ FileHeader->RawStartTime;
  

  // Set the flag to show file names are unsorted
  FileHeader->SortedFlag      = FALSE;
  FileHeader->FileResizedFlag = FALSE;


  FileHeader->LastBlockOffset = 0;
  SetStartingBlockAddress();

}

__pkTraceServer::__pkTraceServer(char* FileName)
{
  struct stat statbuf;


  // Set the file name
  strcpy(MappedFileName,FileName);

  // Create the file name

  Fd = open( MappedFileName, O_RDWR , S_IRUSR | S_IWUSR );


  // Check file was opened OK
  if( Fd < 0 )
    printf("Error errno %d is: %s\n", errno, strerror( errno ) );

  assert( Fd >= 0 );

  // Obtain size of file
  int fstat_rc = fstat(Fd,&statbuf) ;
  assert(fstat_rc >= 0);

  // map the file
  FileHeader = (MapFileHeader*)mmap(0, statbuf.st_size,
                                     PROT_READ | PROT_WRITE, MAP_SHARED, Fd, 0);

  // Check file was mapped OK
  assert((caddr_t)FileHeader != (caddr_t)-1);

  if (FileHeader->FileResizedFlag == FALSE)
  {
    SetFileSize(FileHeader->LastBlockOffset);
  }

  // Check whether the header is sorted
  if (FileHeader->SortedFlag == FALSE)
  {
    // Not sorted.
    qsort (&(FileHeader->TraceDataMatrix[0]),
             FileHeader->NumTraces,
             sizeof(TraceData),
             QsortHeaderComp);

    FileHeader->SortedFlag = TRUE;
  }

  SetStartingBlockAddress();
}

__pkTraceServer::~__pkTraceServer()
{
  if (FileHeader->FileResizedFlag == FALSE)
  {
    SetFileSize(FileHeader->LastBlockOffset);
  }

  if (munmap(FileHeader,MAP_FILE_SIZE) == -1)
     printf("Error unmapping file\n");

  close (Fd);

  __pkTraceServer::GetIF(NULL, TRUE);
}

__pkTraceServer* __pkTraceServer::Load(char* MappedFile)
{
  __pkTraceServer* LoadServer = new __pkTraceServer(MappedFile);
  return (LoadServer);
}

void __pkTraceServer::Destroy()
{
  delete this;
}


__pkTraceServer* __pkTraceServer::GetIF(char* FileName ,
                                        int   Kill )
{
  static __pkTraceServer *SingletonPtr;

  // ON destruction of the __pkTraceServer the SingletonPtr should be set
  // to NULL to allow a new one to be reopenned in the same run
  if (Kill == TRUE)
  {
    SingletonPtr = NULL;
    return NULL;
  }

  if ( SingletonPtr == NULL )
  {
    assert(FileName != NULL);
    SingletonPtr = new __pkTraceServer(FileName);
  }
  assert( SingletonPtr != NULL );

  return( SingletonPtr );
}

__pkTraceServer* __pkTraceServer::LockIF( int taskId )
{
  YieldLockOp(YieldLockOpCode_LOCK);
  static __pkTraceServer *SingletonPtr;

  if ( SingletonPtr == NULL )
  {
    SingletonPtr = new __pkTraceServer( taskId );
  }
  assert( SingletonPtr != NULL );

  return( SingletonPtr );
}

void __pkTraceServer::UnLockIF()
{
  YieldLockOp(YieldLockOpCode_UNLOCK);
}

LTime __pkTraceServer::SmalltimeToLTime(smalltime st)
{
  if (FileHeader->ConversionFactor != 0)
    return (LTime)(st * FileHeader->ConversionFactor );

  return (LTime)st;
}

smalltime __pkTraceServer::LTimeToSmalltime(LTime lt)
{
  if (FileHeader->ConversionFactor != 0)
    return (smalltime)(lt / FileHeader->ConversionFactor );

  return (smalltime)lt;
}

void __pkTraceServer::SetStartingBlockAddress()
{
  // Calculate new start address for block
  int iFileHeader = (int)FileHeader;
  int SizeOverPage = sizeof(MapFileHeader) % NBPG;
  int NewAddress =
               iFileHeader + (int)sizeof(MapFileHeader) + (NBPG - SizeOverPage);
  FileHeader->StartingBlock = (MappedBlock*)NewAddress;
}

int __pkTraceServer::GetNewBlock(smalltime** ptrNewTimeAddress,
                                 smalltime** ptrLastHitInBlock,
                                 int         TraceIndex )
{
  int NewBlock;
  int ChainBlock;

#if !defined(PK_POSIX)
  pthread_mutex_lock(mLock);
#endif

  // Check blocks are available to handout
  if (FileHeader->LastBlockOffset >= FileHeader->MappedBlocksInFile)
  {
    // No free blocks remaining, need to start circular
    // Find the first block
    ChainBlock = FileHeader->TraceDataMatrix[TraceIndex].CurrentOffset;

    // loop through the blocks
    while((FileHeader->StartingBlock[ChainBlock].NextBlockOffset != -1) ||
          (FileHeader->StartingBlock[ChainBlock].NextBlockOffset ==
                         FileHeader->TraceDataMatrix[TraceIndex].CurrentOffset))
    {
      ChainBlock = FileHeader->StartingBlock[ChainBlock].NextBlockOffset;
    }
    NewBlock = ChainBlock;
  }
  else
  {
    // Free blocks, use one
    NewBlock = FileHeader->LastBlockOffset;
    FileHeader->LastBlockOffset++;
  }

  // Initialize the block position
  FileHeader->StartingBlock[NewBlock].NextBlockOffset =
                          FileHeader->TraceDataMatrix[TraceIndex].CurrentOffset;

  FileHeader->TraceDataMatrix[TraceIndex].CurrentOffset = NewBlock;

  // Set the return value
  *ptrNewTimeAddress = &(FileHeader->StartingBlock[NewBlock].TimeStamp[0]);

  *ptrLastHitInBlock =
           &(FileHeader->StartingBlock[NewBlock].TimeStamp[HITS_PER_BLOCK - 1]);

  // Check a reference has been provided
  if (TraceIndex != -1)
  {
    // Up the counter
    (FileHeader->TraceDataMatrix[TraceIndex].TotalNumBlocks)++;
  }

#if !defined(PK_POSIX)
  pthread_mutex_unlock(mLock);
#endif

  return NewBlock;
}

int __pkTraceServer::InitNewStream(char* StreamName,
                                   smalltime** ptrNewTime,
                                   smalltime** ptrLastHitInBlock)
{
  int StartingBlockPosition ;

  assert (FileHeader->NumTraces < MAX_TRACES);

  strcpy (FileHeader->TraceDataMatrix[FileHeader->NumTraces].TraceName,
                StreamName);

  FileHeader->TraceDataMatrix[FileHeader->NumTraces].TotalNumBlocks = 0;

  FileHeader->TraceDataMatrix[FileHeader->NumTraces].CurrentOffset = -1;

  // Get a block position
  StartingBlockPosition = GetNewBlock(ptrNewTime,
                                      ptrLastHitInBlock,
                                      FileHeader->NumTraces);

  BegLogLine(PKFXLOG_PKTRACE_PK)
    << "__pkTraceServer::InitNewStream StreamName=" << StreamName
    << " FileHeader->NumTraces=" << FileHeader->NumTraces
    << EndLogLine ;
  // Up the counter
  FileHeader->NumTraces++;

  return (FileHeader->NumTraces - 1);
}

LTime __pkTraceServer::GetLTimeFromBC(smalltime st)
{
  if (FileHeader != NULL)
    return (SmalltimeToLTime(st - FileHeader->RawStartTime));

  return SmalltimeToLTime(st);
}

void __pkTraceServer::SetFileSize(int NumBlocks)
{
  int CorrectFileSize;

  CorrectFileSize = sizeof(MapFileHeader) +
                         ((NumBlocks + 1) * sizeof(MappedBlock)) +
                          ( NBPG - (sizeof(MapFileHeader)%NBPG));

  ftruncate(Fd,CorrectFileSize);

  FileHeader->FileResizedFlag = TRUE;
}

smalltime __pkTraceServer::GetBCFromLTime(LTime lt)
{
  if (FileHeader != NULL)
    return (LTimeToSmalltime(lt) + FileHeader->RawStartTime);

  return (LTimeToSmalltime(lt));
}

int FindHitInBlock(__pktraceLoadBlock* BlockAddress, smalltime TargetTime)
{
  return (BinarySearch(BlockAddress,TargetTime));
}

int BinarySearch(__pktraceLoadBlock* BlockAddress, smalltime TargetTime )
{

  int        Start, Middle, End;
  int    Found = 0;
  int        Position ;
  Start =    0;

  //
  //  Start Binary chop
  //
  End = BlockAddress->NumHitsInBlock;

  Middle = (int)((Start + End) / 2)  ;

  while ((Start < Middle) && (Found == 0) )
  {

    if (TargetTime ==
                   BlockAddress->pServerBlock->TimeStamp[Middle])
    {
      Found = 1;
      Position = Middle;
    }
    else if (TargetTime >
                   BlockAddress->pServerBlock->TimeStamp[Middle])
    {
      Start = Middle ;
    }
    else
    {
      End = Middle ;
    }
    Middle = ((Start + End) / 2)  ;

  } /* End while loop */

  if (Found == 0)
  {
    Position = Middle;
  }

  return (Position);

}

void __pkTraceServer::DisplayTraceInfo(int Index)
{
  int CurrentOffset;
  printf("Name:[%s]\n",FileHeader->TraceDataMatrix[Index].TraceName);
  printf("CurrentOffset:[%d]\n",
                            FileHeader->TraceDataMatrix[Index].CurrentOffset);
  printf("NumBlocks:[%d]\n",
                           FileHeader->TraceDataMatrix[Index].TotalNumBlocks);
  CurrentOffset = FileHeader->TraceDataMatrix[Index].CurrentOffset;
  while (CurrentOffset != -1)
  {
     printf("BlockNumber:[%d]\n",CurrentOffset);
     printf("FirstHitTIme:[%lld]\n",
     GetLTimeFromBC(FileHeader->StartingBlock[CurrentOffset].TimeStamp[0]));
     printf("Next Block:[%d]\n",
                      FileHeader->StartingBlock[CurrentOffset].NextBlockOffset);
     CurrentOffset =
                       FileHeader->StartingBlock[CurrentOffset].NextBlockOffset;
  }
}

/*******************************************************************************
*
* Class:   __pkTraceStream
* Purpose: Interface with application code to store trace points
*
*******************************************************************************/

__pkTraceStream::__pkTraceStream()
{
  CurrentBlock = NULL;
  MapIndex = -1;

  ptrCurrentHit     = (smalltime*)0x00000001;
  ptrLastHitInBlock = (smalltime*)0x00000001;

}

__pkTraceStream::~__pkTraceStream()
{
}


////////// BEGIN BLOCK ADDED FOR FXLOG TRACING //////////////////////////////////////////////////////////
int
__pkTraceStream::__pkTraceStreamConnect( char     *TraceStreamName )
{
  __pkTraceServer* pServer;

  // Create entry in the server
  pServer = __pkTraceServer::LockIF( 0 );  // shouldn't need to lock - but what the hell.

  assert( pServer != NULL );

  MapIndex = pServer->InitNewStream(  TraceStreamName,
                                      &ptrCurrentHit,
                                      &ptrLastHitInBlock);
  pServer->UnLockIF();

  return(0);
}
///////////////////  END BLOCK ADDED FOR FXLOG TRACING //////////////////////////////////

int
__pkTraceStream::__pkTraceStreamConnect( char     *SourceFileName,
                                         int       SourceLineNo,
                                         unsigned  Class,
                                         char     *TraceStreamContext,
                                         int       TraceStreamContextOrdinal,
                                         char     *TraceStreamName )
{
  unsigned tno;
#ifdef _THREAD_SAFE
  tno = ((unsigned) pthread_self()) & 0x0000FFFF;
#else
  tno = 0;
#endif
  __pkTraceServer* pServer;

  char ComposedTraceName[ MAX_FILENAME ];
  char MappedFileName[ MAX_FILENAME ];
  char TraceFileName[ MAX_FILENAME ];

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

  sprintf( MappedFileName, "%s::p%05u_t%05u_%s[%04u]",
               ComposedTraceName,
               getpid(),
               tno,
               SourceFileName,
               SourceLineNo);

  // Replace the dodgy characters
  for( unsigned long nloop = 0; nloop < strlen( MappedFileName ); nloop++ )
  {
    if( MappedFileName[nloop] == '/' )
      MappedFileName[nloop] = '|';
    else if( MappedFileName[nloop] == ' ' )
      MappedFileName[nloop] = '_';
  }

  // Create entry in the server
  pServer = __pkTraceServer::LockIF( TraceStreamContextOrdinal );

  assert( pServer != NULL );

  MapIndex = pServer->InitNewStream(MappedFileName,
                                    &ptrCurrentHit,
                                    &ptrLastHitInBlock);
  pServer->UnLockIF();

  return(0);
}

void __pkTraceStream::AllocNewBlock( __pkTraceServer* pServer)
{
  int          NewBlock;

  // Next hit will require a new block
  assert(pServer != NULL);
  NewBlock = pServer->GetNewBlock(&ptrCurrentHit,&ptrLastHitInBlock, MapIndex);

  // Check block allocation was successful
  if (NewBlock == -1)
  {
    // No room to allocate a new block
    // This is where we would make it a circular filling list
    printf("Error allocating block\n");
  }
}

/*******************************************************************************
*
* Class:   __pktraceRead
* Purpose: Class that reads in the trace data for delivery to the GUI
*
*******************************************************************************/

__pktraceRead::__pktraceRead(char* Filename)
{
  strcpy(TraceMappedFile,Filename);
  ClassServer = __pkTraceServer::Load(TraceMappedFile);
  NumLoadedTraces = 0;
}

int __pktraceRead::IsValidMagicNum()
{
  assert (ClassServer != NULL);

  if (ClassServer->FileHeader->MagicNumber != PKTRACE_MAGIC_NUMBER)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

__pktraceRead::~__pktraceRead()
{
  register int loop;

  for(loop = 0; loop < NumLoadedTraces; loop++)
  {
    if( LoadedTraces[loop] != NULL)
      delete (LoadedTraces[loop]);
  }
  NumLoadedTraces = 0;

  if (ClassServer != NULL)
  {
    delete ClassServer;
  }
}

void __pktraceRead::LoadAllTraceData()
{
  register int loop;

  assert (ClassServer != NULL);

  // Load traces into local memeory
  for(loop = 0;
      loop < ClassServer->FileHeader->NumTraces;
      loop++)
  {
    // Construct a new Trace object
    LoadedTraces[NumLoadedTraces] = new __pktraceLoadedTrace(loop, ClassServer);

    // Increment number of traces loaded
    NumLoadedTraces++;

  }// End loop of

} /* LoadAllTraceData */

void __pktraceRead::LoadTraceData(int Position)
{

  assert (ClassServer != NULL);

  // Construct a new Trace object
  LoadedTraces[Position] = new __pktraceLoadedTrace(Position,ClassServer);

  // Increment number of traces loaded
  NumLoadedTraces++;

}

int __pktraceRead::GetNumTraces()
{
  assert (ClassServer != NULL);

  return ClassServer->FileHeader->NumTraces;
}

char* __pktraceRead::GetTraceName(int Position)
{
  assert (ClassServer != NULL);

  return ClassServer->FileHeader->TraceDataMatrix[Position].TraceName;
}

LTime __pktraceRead::GetRunStartTime()
{
  LTime StartTime;
  assert (ClassServer != NULL);

  StartTime =ClassServer->GetLTimeFromBC(ClassServer->FileHeader->RawStartTime);
  return StartTime;
}

/*******************************************************************************
*
* Class:   __pktraceLoadedTrace
*
*******************************************************************************/
__pktraceLoadedTrace::__pktraceLoadedTrace(int Index,__pkTraceServer* NewServer)
{
  MappedBlock *pCurrentBlock;
  int CurrentBlock;
  NumBlocks = 0;
  int BlocksInTrace;
  register int counter;
  ClassServer = NewServer;
  assert (ClassServer != NULL);

  NumHits = 0;

  // Obtain the current block
  CurrentBlock = ClassServer->FileHeader->TraceDataMatrix[Index].CurrentOffset;
  NumBlocks = ClassServer->FileHeader->TraceDataMatrix[Index].TotalNumBlocks;

  BegLogLine(PKFXLOG_PKTRACE_PK) 
    << "Index=" << Index
    << " CurrentBlock=" << CurrentBlock
    << " NumBlocks=" << NumBlocks
    << EndLogLine ;
      
  BlocksInTrace = NumBlocks - 1 ;

  Trace = (__pktraceLoadBlock*)malloc(NumBlocks * (sizeof(__pktraceLoadBlock)));

  // Loop through all the blocks used by the trace
  while(BlocksInTrace >= 0)
  {
    BegLogLine(PKFXLOG_PKTRACE_PK) 
      << "BlocksInTrace=" << BlocksInTrace
      << EndLogLine ;
    Trace[BlocksInTrace].pServerBlock =
                        &(ClassServer->FileHeader->StartingBlock[CurrentBlock]);

    // Loop to find the number of hits
    for (counter = 0; counter < HITS_PER_BLOCK; counter++)
    {
      BegLogLine(PKFXLOG_PKTRACE_PK) 
        << "counter=" << counter
        << EndLogLine ;
      if (Trace[BlocksInTrace].pServerBlock->TimeStamp[counter] == 0)
      {
        Trace[BlocksInTrace].NumHitsInBlock = counter;
        break;
      }

      else if (counter > 0)
      {
        if (Trace[BlocksInTrace].pServerBlock->TimeStamp[counter] <
            Trace[BlocksInTrace].pServerBlock->TimeStamp[counter - 1])
        {
          Trace[BlocksInTrace].NumHitsInBlock = counter;
          break;
        }
      }

      else if (counter == 0)
      {
        if (Trace[BlocksInTrace].pServerBlock->TimeStamp[counter + 1] <
            Trace[BlocksInTrace].pServerBlock->TimeStamp[counter])
        {
          Trace[BlocksInTrace].NumHitsInBlock = 1;
          counter = 1;
          break;
        }
      }

    }

    // Calculate the total hit count
    NumHits += counter;

    Trace[BlocksInTrace].Starttime =
                                Trace[BlocksInTrace].pServerBlock->TimeStamp[0];

    // Calculate the end time for the block
    // Check whether there is a next block
    if (BlocksInTrace == NumBlocks - 1)
    {
      // There is no next block, so take end time as being the last hit in
      // this block
      Trace[BlocksInTrace].Endtime =
               Trace[BlocksInTrace].pServerBlock->TimeStamp[counter - 1];
    }
    else
    {
      // There is a next block so set end time to its start time
      Trace[BlocksInTrace].Endtime = (Trace[BlocksInTrace + 1].Starttime - 1);
    }

    // Decrement the number of blocks
    BlocksInTrace--;

    // Move on to the next block
    CurrentBlock = ClassServer->
                        FileHeader->StartingBlock[CurrentBlock].NextBlockOffset;
  }
}

int __pktraceLoadedTrace::BinarySearchBlocks( smalltime TargetTime )
{

  int        Start, Middle, End;
  int    Found = 0;
  int        Position ;
  Start =    SearchRangeLower;

  //
  //  Start Binary chop
  //
  End = SearchRangeUpper + 1 ;

  Middle = (int)((Start + End) / 2)  ;

  while ((Start < Middle) && (Found == 0) )
  {
    if ( (TargetTime >= Trace[Middle].Starttime ) &&
       (TargetTime <= Trace[Middle].Endtime ))
    {
      Found = 1;
      Position = Middle;
    }
    else if (TargetTime > Trace[Middle].Endtime)
    {
      Start = Middle ;
    }

    else if (TargetTime < Trace[Middle].Starttime)
    {
      End = Middle ;
    }

    else
    {
      printf("Unexpected value\n") ;
    }
    Middle = ((Start + End) / 2)  ;
  } /* End while loop */

  if ( (TargetTime >= Trace[Middle].Starttime ) &&
        (TargetTime <= Trace[Middle].Endtime ))
  {
     Found = 1;
     Position = Middle;
  }

  if (Found == 0)
  {
    printf("BinarySearchBlocks::Don't expect to be here Middle:[%d]\n",
                                                        Middle);
    if ((TargetTime >= Trace[0].Starttime) &&
                                   (TargetTime <= Trace[NumBlocks - 1].Endtime))
    {
      printf("Hit should be somewhere here\n");
    }
    else
    {
       printf("Hit not here\n");
    }
    Position = -1;
  }

  return (Position);
}

////////////////////////////////////////////////////////////////////////////
// Function: IsHitInRange
//
// Purpose :
////////////////////////////////////////////////////////////////////////////
int __pktraceLoadedTrace::IsHitInRange( LTime TargetTime , LTime ScaleValue)
{
  int Result;
  assert (ClassServer != NULL);

  smalltime SearchTime = ClassServer->GetBCFromLTime(TargetTime);

  if ((SearchTime >= Trace[0].Starttime) &&
                                   (SearchTime <= Trace[NumBlocks - 1].Endtime))
  {
    Result =  TRUE;
  }

  else
  {
    if (SearchTime < Trace[0].Starttime)
    {
       Result = LOWER;
    }
    else if (SearchTime > Trace[NumBlocks - 1].Endtime)
    {
      Result = HIGHER;
    }
    else
    {
      printf("IsHitInRange::Don't expect to be here\n");
      printf("Trace[0].Starttime:  [%lld]\n",Trace[0].Starttime);
      printf("SearchTime:          [%lld]\n",SearchTime);
      printf("Trace[%d].Endtime:   [%lld]\n",NumBlocks - 1,
                                                  Trace[NumBlocks - 1].Endtime);
      Result = -1;
    }
  }
  return Result;
}

////////////////////////////////////////////////////////////////////////////
// Function: SetSearchBounds
//
// Purpose :
////////////////////////////////////////////////////////////////////////////
void __pktraceLoadedTrace::SetSearchBounds(LTime LowerTargetTime,
                                           LTime UpperTargetTime )
{

  assert (ClassServer != NULL);

  SearchRangeLower = 0;
  SearchRangeUpper = NumBlocks - 1;
  return;
  if (IsHitInRange(LowerTargetTime) == TRUE)
  {
    SearchRangeLower = SearchForBlock(LowerTargetTime);
  }
  else
  {
    SearchRangeLower = 0;
  }

  if (IsHitInRange(UpperTargetTime) == TRUE)
  {
    SearchRangeUpper = SearchForBlock(UpperTargetTime);
  }
  else
  {
    SearchRangeUpper = NumBlocks - 1;
  }
}

////////////////////////////////////////////////////////////////////////////
// Function: SearchForBlock
//
// Purpose :
////////////////////////////////////////////////////////////////////////////
int __pktraceLoadedTrace::SearchForBlock( LTime TargetTime )
{
  assert (ClassServer != NULL);
  int Position ;
  smalltime SearchTime = ClassServer->GetBCFromLTime(TargetTime);

  Position = BinarySearchBlocks(SearchTime);

  if (Position == -1)
    printf("Error in SearchForBlock TargetTime:[%lld]\n",TargetTime);

  return Position;
}

////////////////////////////////////////////////////////////////////////////
// Function: FindHitPos
//
// Purpose :
////////////////////////////////////////////////////////////////////////////
int __pktraceLoadedTrace::FindHitPos(LTime TargetTime)
{
  int LocalBlockNumber;
  int HitNumber;
  smalltime SearchTime = ClassServer->GetBCFromLTime(TargetTime);

  LocalBlockNumber = SearchForBlock(TargetTime);

  if (LocalBlockNumber == -1)
  {
     printf("Error with target time [%lld]\n",TargetTime);

     return -1;
  }

  HitNumber = FindHitInBlock(&(Trace[LocalBlockNumber]),
                                          SearchTime);

  return ( (LocalBlockNumber * HITS_PER_BLOCK) + HitNumber );
}

LTime __pktraceLoadedTrace::GetStartTime()
{
  assert (ClassServer != NULL);

  return ClassServer->GetLTimeFromBC( Trace[0].Starttime);
}

LTime __pktraceLoadedTrace::GetEndTime()
{
  assert (ClassServer != NULL);

  return ClassServer->GetLTimeFromBC( Trace[NumBlocks - 1].Endtime);
}

LTime __pktraceLoadedTrace::GetLTimeAtPosition(int Position)
{
  int LocalBlockNumber;
  int BlockHit;
  assert (ClassServer != NULL);


  LocalBlockNumber = Position / HITS_PER_BLOCK;
  BlockHit = Position%HITS_PER_BLOCK;

  return ClassServer->GetLTimeFromBC(
           Trace[LocalBlockNumber].pServerBlock->TimeStamp[BlockHit]);
}

smalltime __pktraceLoadedTrace::GetsmalltimeAtPosition(int Position)
{
  int LocalBlockNumber;
  int BlockHit;
  assert (ClassServer != NULL);


  LocalBlockNumber = Position / HITS_PER_BLOCK;
  BlockHit = Position%HITS_PER_BLOCK;

  return Trace[LocalBlockNumber].pServerBlock->TimeStamp[BlockHit];
}

int __pktraceLoadedTrace::FindEarliestVisibleHit (LTime TargetValue)
{
  int ReturnValue;
  int RangeResult;
  assert (ClassServer != NULL);

  // Check if the value is in the range of the
  RangeResult = IsHitInRange(TargetValue);

  if (RangeResult == TRUE)
  {
    // To get here the value is in range of the trace
    // Need to find the earliest hit
    ReturnValue = FindHitPos(TargetValue);
  }
  else if (RangeResult == LOWER)
  {
    ReturnValue = 0; // Fisrt hit
  }
  else if (RangeResult == HIGHER)
  {
    ReturnValue = NumHits; // Last hit
  }

  return ReturnValue;
}

#endif
