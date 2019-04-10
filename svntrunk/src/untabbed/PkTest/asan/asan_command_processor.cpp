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
 #include <stdio.h>
#include <string.h>
#include <sstream>

#include <asan_memory_server_bgltree.h>

#include <inmemdb/GlobalParallelObjectManager.hpp>
#include <inmemdb/NamedAllReduce.hpp>

NamedAllReduce<unsigned long long, NAMED_ALLREDUCE_SUM>   AllReduce;
NamedAllReduce<char,               NAMED_ALLREDUCE_SUM>   AllReduceChar;
#include <inmemdb/NamedBarrier.hpp>


#include <inmemdb/Table.hpp>

#include <inmemdb/types.hpp>
#include <inmemdb/utils.hpp>
#include <inmemdb/SortGen.hpp>
#include <inmemdb/IndySort.hpp>

#include <sstream>
//#include <sys/types.h>
//#include <regex.h>

#include <pcreposix.h>

enum 
{
  SORT_COMMAND = 1,
  GREP_COMMAND = 2
};

#ifndef PKFXLOG_RUN_SORT
#define PKFXLOG_RUN_SORT ( 0 )
#endif

#ifndef PKFXLOG_ASAN_GREP
#define PKFXLOG_ASAN_GREP ( 0 )
#endif

#define TEST_RECORD_SIZE 100

int PkStartType = PkStartAllCores;

#if defined(PKTRACE)
int PkTracing = 1;
#else
int PkTracing = 0;
#endif

#ifndef CORE_FOR_MEMORY_SERVER
#define CORE_FOR_MEMORY_SERVER (1)
#endif

#ifndef CORE_FOR_COMMAND_SERVER
#define CORE_FOR_COMMAND_SERVER (0)
#endif

#ifndef RUN_SORT
#define RUN_SORT (0)
#endif

// This object is used by both cores
ASAN_MemoryServer asanMemoryServer;

struct DataGetRemainderOfRecordRequest
  {
    char      mDelimiter;
    int       mInode;
    long long mBlockIndex;
    char*     mRecvBuffer;
    int*      mRecordIndexAddress;
    int       mRecvBufferSizeFree;
  };

struct DataGetRemainderOfRecordResponse
  {
    char*     mRecvBuffer;
    pkFiberControlBlockT * FCB;
    int       mLength;
    int*      mRecordIndexAddress;
    char      mData[ 0 ];
  };

struct FlushMemoryAck
{
    pkFiberControlBlockT * FCB;
};

int FlushMemoryAckFx( void* arg )
  {
    FlushMemoryAck * Pkt = (FlushMemoryAck *) arg;
    
    pkFiberUnblock( Pkt->FCB );

    BegLogLine( DEBUG_ASAN_SORT )
      << "FlushMemoryAckFx:: "
      << " Got an ack for context: " << Pkt->FCB
      << EndLogLine;

    return 0;
  }

int FlushMemoryFx( void* arg )
  {
    rts_dcache_evict_normal();
    
    FlushMemoryAck* ResPkt = (FlushMemoryAck *)  PkActorReserveResponsePacket( FlushMemoryAckFx,
            sizeof( FlushMemoryAck ) );
    
    unsigned long SourceNodeId;
    unsigned long SourceCoreId;
    pkFiberControlBlockT* Context = NULL;
    PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );   
    
    ResPkt->FCB           = Context;
    
    PkActorPacketDispatch( ResPkt );

    BegLogLine( DEBUG_ASAN_SORT )
      << "FlushMemoryFx:: "
      << " Flushed memory, returning ack to: "
      << " SourceNodeId: " << SourceNodeId
      << " SourceCoreId: " << SourceCoreId
      << " Context: " << Context
      << EndLogLine;
    
    return 0;
  }

int ProcessDataGetRemoteRemaiderOfRecordResponseFx( void * arg )
{
  DataGetRemainderOfRecordResponse* Pkt = (DataGetRemainderOfRecordResponse *) arg;
 
  char* BufferPtr = Pkt->mRecvBuffer;
  int Length  = Pkt->mLength;
  
  memcpy( BufferPtr, Pkt->mData, Length );  
  
  int*  RecordIndexAddress      = Pkt->mRecordIndexAddress;
  *RecordIndexAddress += Length;
  
  pkFiberUnblock( Pkt->FCB );

  BegLogLine( DEBUG_ASAN_SORT )
    << "ProcessDataGetRemoteRemaiderOfRecordResponseFx:: Just called pkFiberUnblock" 
    << " Length: " << Length
    << " BufferPtr: " << (void *) BufferPtr
    << EndLogLine;
  
  return 0;
}

int ProcessDataGetRemoteRemaiderOfRecordRequestFx( void * arg )
{
  DataGetRemainderOfRecordRequest* Pkt = (DataGetRemainderOfRecordRequest *) arg;
  
  char Delimiter = Pkt->mDelimiter;
  int  Inode     = Pkt->mInode;
  long long BlockIndex     = Pkt->mBlockIndex;
  char* RecvBuffer         = Pkt->mRecvBuffer;
  int   RecvBufferSizeFree = Pkt->mRecvBufferSizeFree;
  int*  RecordIndexAddress      = Pkt->mRecordIndexAddress;
  
  unsigned long SourceNodeId;
  unsigned long SourceCoreId;
  pkFiberControlBlockT* Context = NULL;
  PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );   

  BegLogLine( DEBUG_ASAN_SORT )
    << "ProcessDataGetRemoteRemaiderOfRecordRequestFx:: "
    << " Inode: " << Inode
    << " Delimiter: " << (int) Delimiter
    << " BlockIndex: " << BlockIndex
    << " RecvBuffer: " << (void *) RecvBuffer
    << " RecvBufferSizeFree: " << (void *) RecvBufferSizeFree
    << " SourceNodeId: " << SourceNodeId
    << " SourceCoreId: " << SourceCoreId
    << EndLogLine;

  char* LocalBlockPtr = GetLocalDataPtr( Inode, BlockIndex, 0 );
  
  StrongAssertLogLine( LocalBlockPtr != NULL )
    << "ProcessDataGetRemoteRemaiderOfRecordRequestFx:: "
    << " ERROR:: LocalBlockPtr is NULL!"
    << " Inode: " << Inode
    << " BlockIndex: " << BlockIndex
    << EndLogLine;
  
#ifndef NDEBUG  
  ChecksumBlock( Inode, BlockIndex );
#endif

  int RemainderDataLength = 0;
  for( int c = 0; c < ASAN_MEMORY_BLOCK_SIZE; c++ )
    {
      char CurrentChar = LocalBlockPtr[ c ];
      
      if( CurrentChar == Delimiter )
  {
    RemainderDataLength = c + 1;
    // Send the last packet
    // ASSUMPTION: There's at most one packet worth to send back
    
    StrongAssertLogLine( RemainderDataLength <= RecvBufferSizeFree )
      << "ERROR:: RemainderDataLength: " << RemainderDataLength
      << " RecvBufferSizeFree: " << RecvBufferSizeFree
      <<  EndLogLine;
    
    int SizeToSend = sizeof( DataGetRemainderOfRecordResponse ) + RemainderDataLength;
    
    DataGetRemainderOfRecordResponse* ResPkt = (DataGetRemainderOfRecordResponse *)
      PkActorReserveResponsePacket( ProcessDataGetRemoteRemaiderOfRecordResponseFx,
            SizeToSend );
    
    ResPkt->mLength = RemainderDataLength;
    ResPkt->mRecvBuffer = RecvBuffer;
    ResPkt->mRecordIndexAddress = RecordIndexAddress;
    
    memcpy( ResPkt->mData, LocalBlockPtr, RemainderDataLength );
        
    ResPkt->FCB           = Context;
    
    BegLogLine( DEBUG_ASAN_SORT )
      << "ProcessDataGetRemoteRemaiderOfRecordRequestFx:: "
      << " SourceNodeId: " << SourceNodeId
      << " SourceCoreId: " << SourceCoreId
      << " Context: " << Context
      << " RemainderDataLength: " << RemainderDataLength
      << " SizeToSend: " << SizeToSend
      << " RecvBuffer: " << (void *) RecvBuffer
      << EndLogLine;
    
    PkActorPacketDispatch( ResPkt );

    break; 
  } 
    } 

  // Tick the IsFirstRecordTaken flag
  LocalDataAccessKey key;
  key.inode      = Inode;
  key.blockIndex = BlockIndex;
  
  LocalDataMap_T::iterator iter = LocalDataMap.find( key );

  StrongAssertLogLine( iter != LocalDataMap.end() )
    << "ERROR:: Valid mapping to a block is expected "
    << " Inode: " << Inode
    << " BlockIndex: " << BlockIndex
    << EndLogLine;
  
  iter->second.IsFirstRecordTaken = 1;

  BegLogLine( DEBUG_ASAN_SORT )
    << "ProcessDataGetRemoteRemaiderOfRecordRequestFx:: "
    << " Set iter->second.IsFirstRecordTaken to: "  << (int ) iter->second.IsFirstRecordTaken
    << " at addr: " << (void * ) &iter->second.IsFirstRecordTaken
    << EndLogLine;

  return 0; 
}

// NOTE: After return of the below function LocalRecordsIndex 
// Should be updated with the length of the remainder
void GetRemoteRemainderOfRecord( int DataOwner,
         int Inode,
         long long BlockIndex,
         char* LocalRecords,
         int*   LocalRecordsIndex,
         int   MaxLocalRecordsSize,
         char  Delimiter )
{
  DataGetRemainderOfRecordRequest* Pkt = (DataGetRemainderOfRecordRequest *)
    PkActorReservePacket( DataOwner,
        PkCoreGetId(),
        ProcessDataGetRemoteRemaiderOfRecordRequestFx,
        sizeof( DataGetRemainderOfRecordRequest ) );
  
  Pkt->mInode = Inode;
  Pkt->mDelimiter = Delimiter;
  Pkt->mBlockIndex = BlockIndex;
  Pkt->mRecvBuffer = & LocalRecords[ *LocalRecordsIndex ];
  Pkt->mRecvBufferSizeFree = MaxLocalRecordsSize - *LocalRecordsIndex;
  Pkt->mRecordIndexAddress = LocalRecordsIndex;
  
  BegLogLine( DEBUG_LOG )
    << "GetRemoteRemainderOfRecord: About to dispatch a packet to "
    << " NodeId: " << DataOwner
    << " Inode: " << Inode
    << " BlockIndex: " << BlockIndex
    << EndLogLine;
  
  PkActorPacketDispatch( Pkt );
  
  PkFiberBlock();

}

int BlockDataToRecordData( int   inode, 
         int   sortFileSize,
         char  delimiter, 
         char* LocalRecords,
         int   MaxLocalRecordsSize,
         int&  RecordsSize )
  {
    BegLogLine( DEBUG_ASAN_SORT )
      << "BlockDataToRecordData: Entering..."
      << " inode: " << inode
      << " sortFileSize: " << sortFileSize
      << " delimiter: " << (int) delimiter
      << " MaxLocalRecordsSize: " << MaxLocalRecordsSize
      << " @LocalRecords: " << (void *) LocalRecords
      << EndLogLine;
    
    LocalBlocksForInodeManagerIF* LocalBlockManagerIF = asanMemoryServer.GetLocalBlocks( inode );
    
    int LocalRecordsIndex = 0;
    int LocalRecordsCount = 0;

    if( LocalBlockManagerIF != NULL )
      {	
  // LocalBlockManagerIF->SortBlockList();

  LocalBlockManagerIF->ChecksumBlocks();

  BlockRep* LocalBlock = LocalBlockManagerIF->GetFirstBlock();	

  // Figure out if this node has the very first record. 
  // In this case the first record starts at offset=0 and goes up to the
  // first delimiter
  
  // Seek up to first delimiter and start moving records out.
  // The node that owns the first byte of a record owns the record
  
  // On the last record in the block, if end of block is reached
  // and no delimiter is found then we fetch the rest of the record 
  // from the neighboring node
  
  int LocalRecordCount = 0;
  // char* StartOfRecord = LocalBlock->mBlockPtr;
  int StartOfRecord = -1;
  int CurrentRecordLength = 0;
    
  // Iterate over local blocks
  while( LocalBlock != NULL )
    {       	

      char* BlockPtr = LocalBlock->mBlockPtr;

      long long BlockIndex = LocalBlock->mBlockIndex;
      long long StartBlockOffsetInFile = BlockIndex * ASAN_MEMORY_BLOCK_SIZE;
      
      StrongAssertLogLine( BlockPtr != NULL )
        << "BlockDataToRecordData:: ERROR:: " 
        << " BlockPtr should not be NULL here"
        << " BlockIndex: " << LocalBlock->mBlockIndex
        << " inode: " << inode
        << " LocalBlock: " << (void *) LocalBlock
        << EndLogLine;	    

      StrongAssertLogLine( LocalBlock->DoChecksum() )
        << "BlockDataToRecordData:: ERROR:: " 
        << " checksum failed"
        << " BlockIndex: " << LocalBlock->mBlockIndex
        << " inode: " << inode
        << " LocalBlock: " << (void *) LocalBlock
        << EndLogLine;

      BegLogLine( DEBUG_ASAN_SORT )
        << "BlockDataToRecordData:: Working on block ptr: " << (void * ) BlockPtr
        << " BlockIndex: " << LocalBlock->mBlockIndex
        << " inode: " << inode
        << EndLogLine;

      StartOfRecord = -1;
      CurrentRecordLength = 0;
      // Skip to the start of the second record
      for( int c = 0; c < ASAN_MEMORY_BLOCK_SIZE; c++ )
        {
    char CurrentChar = BlockPtr[ c ];
    if( CurrentChar == delimiter )
      {
        StartOfRecord = c+1;
        break;
      }
        
    StrongAssertLogLine(( c + StartBlockOffsetInFile ) < sortFileSize )
      << "BlockDataToRecordData:: ERROR:: End of file reached before start of second record found "
      << " sortFileSize: " << sortFileSize
      << " c: " << c
      << " StartBlockOffsetInFile: " << StartBlockOffsetInFile
      << EndLogLine;
        }

      BegLogLine( DEBUG_ASAN_SORT )
        << "BlockDataToRecordData: StartOfRecord: " << StartOfRecord
        << EndLogLine;
      
      // ASSUMPTION: This block contains a record boundary
      StrongAssertLogLine( StartOfRecord != -1 )
        << "ERROR:: ASSUMPTION: Each block contains a record boundary."
        << EndLogLine;
      
      int EndOfFile = 0;
      // Go down the block moving records
      
      #define RECORD_BUFFER_FOR_LOGGING_SIZE 1024
      char RecordBuffForLogging[ RECORD_BUFFER_FOR_LOGGING_SIZE ];
      bzero( RecordBuffForLogging, RECORD_BUFFER_FOR_LOGGING_SIZE );

      for( int c = StartOfRecord; c < ASAN_MEMORY_BLOCK_SIZE; c++ )
        {
    char CurrentChar = BlockPtr[ c ];
    CurrentRecordLength++;
    
    if( CurrentChar == delimiter )
      {	      
        // Got a record		    
        // Include the delimiter in the record
        memcpy( & LocalRecords[ LocalRecordsIndex ], 
          & BlockPtr[ StartOfRecord ],
          CurrentRecordLength );

        AssertLogLine( CurrentRecordLength < RECORD_BUFFER_FOR_LOGGING_SIZE )
          << "BlockDataToRecordData:: "
          << "ERROR:: CurrentRecordLength: " 
          << CurrentRecordLength 
          << " RECORD_BUFFER_FOR_LOGGING_SIZE: " 
          << RECORD_BUFFER_FOR_LOGGING_SIZE
          << EndLogLine;

#if DEBUG_ASAN_SORT
        bzero( RecordBuffForLogging, RECORD_BUFFER_FOR_LOGGING_SIZE );
        memcpy( RecordBuffForLogging, 
          &LocalRecords[ LocalRecordsIndex ],
          CurrentRecordLength );
        
        BegLogLine( DEBUG_ASAN_SORT )
          << "BlockDataToRecordData: "
          << " OffsetOfDataInBlock: " << c
          << " DataOffsetInFile: " << (c+StartBlockOffsetInFile)
          << " Moved record index: " << LocalRecordsIndex
          << " CurrentRecordLength: " << CurrentRecordLength
          // << " MoveTarget: " << (void *) & LocalRecords[ LocalRecordsIndex ]
          // << " MoveSource: " << (void *) & BlockPtr[ StartOfRecord ]
          << " LocalRecords[ " << LocalRecordsIndex << " ]: "
          << RecordBuffForLogging
          << EndLogLine;
#endif
        
        LocalRecordsCount++;

        LocalRecordsIndex += CurrentRecordLength;
        
        StrongAssertLogLine( LocalRecordsIndex <= MaxLocalRecordsSize )
          << "ERROR:: LocalRecordsIndex: " << LocalRecordsIndex
          << " MaxLocalRecordsSize: " << MaxLocalRecordsSize
          << EndLogLine;
                
        StrongAssertLogLine( CurrentRecordLength == TEST_RECORD_SIZE )
          << "ERROR:: For the Indy sort the record length has to be: " 
          << TEST_RECORD_SIZE
          << " CurrentRecordLength: " << CurrentRecordLength
          << " LocalRecordsIndex: " << LocalRecordsIndex
          << EndLogLine;

        StartOfRecord = c + 1;
        CurrentRecordLength = 0;	      
      }
    
    if( ( c + StartBlockOffsetInFile ) >= sortFileSize )
      {
        // Reached the end of file. Time to break.
        BegLogLine( DEBUG_ASAN_SORT )
          << "BlockDataToRecordData:: End of File "
          << " c: " << c
          << " StartBlockOffsetInFile: " << StartBlockOffsetInFile
          << " sortFileSize: " << sortFileSize
          << " BlockIndex: " << BlockIndex
          << " StartOfRecord: " << StartOfRecord
          << EndLogLine;

        EndOfFile = 1;
        break;		    
      }
        }
      
      // Last record is not complete, fetch the data from another node.
      if( ! EndOfFile && ( CurrentRecordLength > 0 ) )
        {
    AssertLogLine( CurrentRecordLength < TEST_RECORD_SIZE )
      << "ERROR:: CurrentRecordLength: " << CurrentRecordLength
      << " TEST_RECORD_SIZE: " << TEST_RECORD_SIZE
      << " BlockIndex: " << BlockIndex
      << " StartBlockOffsetInFile: " << StartBlockOffsetInFile
      << EndLogLine;

    long long NextBlockIndex = LocalBlock->mBlockIndex + 1;
    
    int DataOwner = GetDataOwner( inode, NextBlockIndex );
    
    BegLogLine( DEBUG_ASAN_SORT )
      << "BlockDataToRecordData: Getting the rest of the record from node: "
      << " DataOwner: " << DataOwner
      << " NextBlockIndex: " << NextBlockIndex
      << " CurrentRecordLength: " << CurrentRecordLength
      << EndLogLine;
    
    int StartingLocalRecordIndex = LocalRecordsIndex;

    memcpy( & LocalRecords[ LocalRecordsIndex ],
      & BlockPtr[ StartOfRecord ],
      CurrentRecordLength );
    
    LocalRecordsIndex += CurrentRecordLength;
    
    GetRemoteRemainderOfRecord( DataOwner, inode, 
              NextBlockIndex, LocalRecords,
              &LocalRecordsIndex,
              MaxLocalRecordsSize,
              delimiter );
    LocalRecordsCount++;

    // Indy check: Check to see if the incomming record is TEST_RECORD_SIZE bytes.
    int IncommingRecordSize = 0;		
    int CurrentTestIndex = StartingLocalRecordIndex;
    while( 1 )
      {
        char CurrentChar = LocalRecords[ CurrentTestIndex ];
        IncommingRecordSize++;
        CurrentTestIndex++;
        
        if( CurrentChar == delimiter )
          {
      StrongAssertLogLine( IncommingRecordSize == TEST_RECORD_SIZE )
        << "ERROR:: Incoming record not correct: " 
        << " IncommingRecordSize: " << IncommingRecordSize
        << " StartingLocalRecordIndex: " << StartingLocalRecordIndex
        << EndLogLine;

      break;
          }
        
        StrongAssertLogLine( IncommingRecordSize < TEST_RECORD_SIZE )
          << "ERROR:: Incoming record not correct: " 
          << " IncommingRecordSize: " << IncommingRecordSize
          << " StartingLocalRecordIndex: " << StartingLocalRecordIndex
          << " TEST_RECORD_SIZE: " << TEST_RECORD_SIZE
          << EndLogLine;		    
      }
        }
      
      LocalBlock = LocalBlockManagerIF->GetNextBlock();
      
      if( EndOfFile )
        StrongAssertLogLine( LocalBlock == NULL )
    << "BlockDataToRecordData:: Error:: End of file encountered, "
    << "but there are more records to process. "		
    << " LocalBlock: " << (void * ) LocalBlock
    << EndLogLine;
    }
  
  BegLogLine( DEBUG_ASAN_SORT )
    << "BlockDataToRecordData: "
    << "Waiting for the rest of the nodes to finish receiving it's records"
    << EndLogLine;
      }
    
    // Wait for every node to be finished with receiving it's data
    // PkCo_Barrier();  
    unsigned long long  TmpBuffer[ 1 ];
    TmpBuffer[ 0 ] = 0;
    AllReduce.Execute( TmpBuffer, 1 );
        
    if( LocalBlockManagerIF != NULL )
      {
  BlockRep* LocalBlock = LocalBlockManagerIF->GetFirstBlock();

  int RecordBufferIndex = 0;

  while( LocalBlock != NULL )
    {
      LocalDataAccessKey key;
      key.inode = inode;
      key.blockIndex = LocalBlock->mBlockIndex;
      
      BegLogLine( DEBUG_ASAN_SORT )
        << "BlockDataToRecordData:: Find a block in LocalDataMap for: "
        << " key.inode: " << key.inode
        << " key.blockIndex: " << key.blockIndex
        << EndLogLine;

      LocalDataMap_T::iterator iter = LocalDataMap.find( key );
      
      StrongAssertLogLine( iter != LocalDataMap.end() )
        << "ERROR:: Expected to find a local data block record for "
        << " inode: " << inode
        << " blockIndex: " << key.blockIndex
        << EndLogLine;
      
      BegLogLine( DEBUG_ASAN_SORT )
        << "BlockDataToRecordData: "
        << " Check if the first record of the block belongs to this node"
        << " iter->second.IsFirstRecordTaken: " << (int) iter->second.IsFirstRecordTaken
        << " at address: " << (void *) & iter->second.IsFirstRecordTaken
        << EndLogLine;
      
      if( !iter->second.IsFirstRecordTaken )	      
        {
    // First record of this block is complete and belongs on this node
    StrongAssertLogLine( iter->second.mBlockRep.mBlockPtr == LocalBlock->mBlockPtr )
      << "ERROR:: Expect the block pointer in the LocalBlockManagerIF"
      << " and LocalDataMap to be the same" 
      << EndLogLine;

    // Move *only* the first record out.
    char* FirstBlockPtr = iter->second.mBlockRep.mBlockPtr;
    int CurrentRecordLength = 0;
    for( int c = 0; c < ASAN_MEMORY_BLOCK_SIZE; c++ )
      {
        char CurrentChar = FirstBlockPtr[ c ];
        CurrentRecordLength++;
        if( CurrentChar == delimiter )
          {
      memcpy( & LocalRecords[ LocalRecordsIndex ], 
        FirstBlockPtr,
        CurrentRecordLength );
      
      BegLogLine( DEBUG_ASAN_SORT )
        << "BlockDataToRecordData: "
        << " Moved record index: " << LocalRecordsIndex
        << " CurrentRecordLength: " << CurrentRecordLength
        << " MoveTarget: " << (void *) & LocalRecords[ LocalRecordsIndex ]
        << " MoveSource: " << (void *) FirstBlockPtr
        << EndLogLine;			

      LocalRecordsIndex += CurrentRecordLength;
      LocalRecordsCount++;

      StrongAssertLogLine( CurrentRecordLength == TEST_RECORD_SIZE )
        << "ERROR:: For the Indy sort the record length has to be : "
        << TEST_RECORD_SIZE
        << " CurrentRecordLength: " << CurrentRecordLength
        << " LocalRecordsIndex: " << LocalRecordsIndex
        << EndLogLine;

      break;
          }
      }
        }

      LocalBlock = LocalBlockManagerIF->GetNextBlock();
    }
    
  BegLogLine( DEBUG_ASAN_SORT )
    << "BlockDataToRecordData: Total size of local records: " << LocalRecordsIndex
    << EndLogLine;

  RecordsSize = LocalRecordsIndex;

#if 0	
  char RecordBuff[ 1024 ];
  bzero( RecordBuff, 1024 );
  int RecordIndex = 0;
  int StartOfRecord = 0;
  int RecordSize    = 0;
  int RecordCount   = 0;
  while( RecordIndex < LocalRecordsIndex )
    {
      RecordSize++;
      if( LocalRecords[ RecordIndex ] == delimiter )
        {
    StrongAssertLogLine( RecordSize == TEST_RECORD_SIZE )
      << "ERROR:: Wrong record size for record at:  "
      << " RecordSize: " << RecordSize		  
      << " StartOfRecord: " << StartOfRecord	  
      << EndLogLine;
    
    memcpy( RecordBuff, 
      & LocalRecords[ StartOfRecord ],
      RecordSize );
    
    RecordCount++;
    
    BegLogLine( DEBUG_ASAN_SORT )
      << "BlockDataToRecordData: "
      << " LocalRecord[ " << ( RecordCount - 1 )<< " ]: " << RecordBuff
      << EndLogLine;
    
    RecordSize = 0;
    StartOfRecord = RecordIndex + 1;
        }
      
      RecordIndex++;	
    }
  
  StrongAssertLogLine( RecordCount == LocalRecordsCount )
    << "ERROR: RecordCount: " << RecordCount
    << " LocalRecordsCount: " << LocalRecordsCount
    << EndLogLine;

  BegLogLine( DEBUG_ASAN_SORT )
    << "BlockDataToRecordData: Total record count: " << RecordCount
    << EndLogLine;
#endif
      }    

    BegLogLine( DEBUG_ASAN_SORT )
      << "BlockDataToRecordData: Leaving..."
      << EndLogLine;
    
    return LocalRecordsCount;
  }

void FlushCore1()
{
  BegLogLine( DEBUG_ASAN_SORT )
    << "FlushCore1:: Entering... "
    << EndLogLine;

  int* Pkt = (int *)
    PkActorReservePacket( PkNodeGetId(),
        1,
        FlushMemoryFx,
        sizeof( int ) );
  
  PkActorPacketDispatch( Pkt );
  
  PkFiberBlock();

  BegLogLine( DEBUG_ASAN_SORT )
    << "FlushCore1:: Leaving... "
    << EndLogLine;
}

typedef inmemdb::Table< SortTupleInfo > TestTableType;
typedef TestTableType::Record StoredRecord;

void asan_grep(unsigned long long GrepFileInode, 
         unsigned long long GrepFileSize,
         char*              GrepRegExp )
{
  BegLogLine( PKFXLOG_ASAN_GREP )
    << "asan_grep:: Entering... "
    << EndLogLine;
   
  char delimiter = '\n';
  
  int  LocalBlockCount = asanMemoryServer.GetLocalBlocksCount( GrepFileInode );  

  BegLogLine( PKFXLOG_ASAN_GREP )
    << "asan_grep():: "
    << " LocalBlockCount: " << LocalBlockCount
    << EndLogLine;

  char* Records = NULL;
  int  MaxRecordsSize = 0;
  if( LocalBlockCount > 0 )
    {
      MaxRecordsSize = sizeof( char ) * ASAN_MEMORY_BLOCK_SIZE * ( 2 * LocalBlockCount );
      
      BegLogLine( PKFXLOG_RUN_SORT )
  << "asan_grep():: "
  << " LocalBlockCount: " << LocalBlockCount
  << " MaxRecordsSize: " << MaxRecordsSize
  << EndLogLine;
      
      Records = (char *) PkHeapAllocate( MaxRecordsSize );
      
      StrongAssertLogLine( Records != NULL )
  << "ERROR: Not enough memory for records allocation "
  << EndLogLine;
    }

  int RecordsSize = 0;
  int LocalRecordCount = BlockDataToRecordData( GrepFileInode, 
            GrepFileSize, 
            delimiter, 
            Records, 
            MaxRecordsSize,
            RecordsSize ); // return
  
  regex_t PatternBuffer;
  regcomp( &PatternBuffer, (const char *) GrepRegExp, 0 );
    
  int StartOfRecord = 0;
  int RecordSize = 0;
  char StringBuffer[ 4096 ];
  for( int i = 0; i < RecordsSize; i++ )
    {
      char CurChar = Records[ i ];
      RecordSize++;

      if( CurChar == delimiter )
  {
    // Got a record.
    // reg exp it
    bzero( StringBuffer, 4096 );
    
    StrongAssertLogLine( RecordSize == 100 )
      << "ERROR:: Current record is larger then the string buffer to reg exp"
      << " RecordsSize: " << RecordsSize
      << " i: " << i
      << EndLogLine;
    
    memcpy( StringBuffer, &Records[ StartOfRecord ], RecordSize );
    
    if(( regexec( &PatternBuffer, StringBuffer, 0, NULL, 0 ) ) == 0 )
      {
        BegLogLine( 1 )
    << "GREP MATCH: " << StringBuffer
    << EndLogLine
    
      }
    
    StartOfRecord = i+1;
    RecordSize = 0;
  }
    }

  BegLogLine( PKFXLOG_ASAN_GREP )
    << "asan_grep:: Leaving... "
    << EndLogLine;
}

void asan_sort(unsigned long long SortFileInode, 
         unsigned long long SortFileSize )
{
  BegLogLine( PKFXLOG_RUN_SORT )
    << "asan_sort():: After the all reduce of "
    << " SortFileInode: " << SortFileInode
    << " SortFileSize: " << SortFileSize
    << EndLogLine;
   
  char delimiter = '\n'; 
  
  int  LocalBlockCount = asanMemoryServer.GetLocalBlocksCount( SortFileInode );  

  BegLogLine( PKFXLOG_RUN_SORT )
    << "asan_sort():: "
    << " LocalBlockCount: " << LocalBlockCount
    << EndLogLine;

  char* Records = NULL;
  int  MaxRecordsSize = 0;
  if( LocalBlockCount > 0 )
    {
      MaxRecordsSize = sizeof( char ) * ASAN_MEMORY_BLOCK_SIZE * ( 2 * LocalBlockCount );
      
      BegLogLine( PKFXLOG_RUN_SORT )
  << "asan_sort():: "
  << " LocalBlockCount: " << LocalBlockCount
  << " MaxRecordsSize: " << MaxRecordsSize
  << EndLogLine;
      
      Records = (char *) PkHeapAllocate( MaxRecordsSize );
      
      StrongAssertLogLine( Records != NULL )
  << "ERROR: Not enough memory for records allocation "
  << EndLogLine;
    }

  int RecordsSize = 0;
  int LocalRecordCount = BlockDataToRecordData( SortFileInode, 
            SortFileSize, 
            delimiter, 
            Records, 
            MaxRecordsSize,
            RecordsSize );
    
  int max_mem = 100;
    
  // The records are ready.
  int MAX_MEMORY_FOR_SORT_PER_NODE = max_mem * 1024 * 1024;
  int MAX_RECORDS_PER_NODE         = MAX_MEMORY_FOR_SORT_PER_NODE / sizeof( StoredRecord );

  int    BinExpansionFactor         = BIN_COUNT_FACTOR;
  double TargetRecordsPerNodeFactor = TARGET_RECORDS_PER_NODE_FACTOR;  

  BegLogLine( PKFXLOG_RUN_SORT )
    << "asan_sort: After BlockDataToRecordData:: "
    << " LocalRecordCount: " << LocalRecordCount
    << " max_mem: " << max_mem
    << " MAX_MEMORY_FOR_SORT_PER_NODE: " << MAX_MEMORY_FOR_SORT_PER_NODE
    << " MAX_RECORDS_PER_NODE: " << MAX_RECORDS_PER_NODE
    << " BinExpansionFactor: " << BinExpansionFactor
    << " TargetRecordsPerNodeFactor: " << TargetRecordsPerNodeFactor
    << EndLogLine;
  
  TestTableType* SortT = TestTableType::instance<inmemdb::BGL_Alloc< TestTableType > >
    ( "SortContainer",
      PkNodeGetCount(),
      BinExpansionFactor,
      TargetRecordsPerNodeFactor,
      MAX_RECORDS_PER_NODE );

  record* LocalRecords = (record *) Records;

  for( int i = 0; i < LocalRecordCount; i++ )
    {
      SortKey Key( LocalRecords[ i ].sortkey,
       LocalRecords[ i ].recnum, 
       LocalRecords[ i ].txtfld );
      
      SortTuple Tuple;

      if( SortT->insert( Key, Tuple ) )
  {
    StrongAssertLogLine( 1 )
      << "ERROR:: Insert failed on " 
      << " Key: " << Key
      << " Tuple: " << Tuple
      << EndLogLine;	  
  }
    }

  BegLogLine( PKFXLOG_RUN_SORT )
    << "asan_sort: Before Barrier:: "
    << EndLogLine;

  // Barrier
  unsigned long long TmpBuffer[ 1 ];
  TmpBuffer[ 0 ] = 0;
  AllReduce.Execute( TmpBuffer, 1 );
  
  double tstart = PkTimeGetSecs();  
  
  BegLogLine( PKFXLOG_RUN_SORT )
    << "asan_sort: Before sort():: "
    << EndLogLine;

  SortT->sort();

  double tfinish = PkTimeGetSecs();

  // Check if the records are sorted.
  BegLogLine( PKFXLOG_RUN_SORT )
    << "asan_sort: Before checkSort():: "
    << EndLogLine;

  long long TotalRows = SortFileSize / 100;
  SortT->checkSort( TotalRows );
  
  unsigned long long NumRowsOnThisNode = LocalRecordCount;
  unsigned long long SizePerNode = ( sizeof( record ) * NumRowsOnThisNode );
  
#define B1024B (1024)
  unsigned long long ONE_GB = B1024B * B1024B * B1024B;
  unsigned long long ONE_TB = B1024B * B1024B * B1024B * B1024B;

  unsigned long long TotalBytes = ((unsigned long long) SortFileSize )
    * B1024B * B1024B * B1024B * B1024B ;
  
  double TotalSizeGB = (1.0 * TotalBytes) / ONE_GB;
  double TotalSizeTB = (1.0 * TotalBytes) / ONE_TB;
  
  // unsigned long long TotalNumberOfRows = TotalBytes / sizeof( record );
  TotalRows = TotalBytes / sizeof( record );
  
  BegLogLine( 1 )
    << "asan_sort::PkMain:: Sorting: "
    << " Rows on this node: " << NumRowsOnThisNode
    << " Total Rows: " << TotalRows
    << " Size of row: " << sizeof( record )
    << " Size of stored row: " << sizeof( StoredRecord )
    << " Size per node: " << SizePerNode
    << " Size per node (MB): " << (1.0*SizePerNode) / (1024.0*1024.0)
    << " Total size (bytes) : " << TotalBytes
    << " Total size (GB) : " << TotalSizeGB
    << " Total size (TB) : " << TotalSizeTB
    << " MAX_MEMORY_FOR_SORT_PER_NODE (MB): " << max_mem
    << " MAX_RECORDS_PER_NODE: " << MAX_RECORDS_PER_NODE
    << " BinExpansionFactor: " << BinExpansionFactor
    << " Timing: " << (tfinish-tstart)
    << EndLogLine;
    
  SortT->Finalize();
}



void Run_CommandProcessor_Fiber( void * arg )
{
  // Wait to hear a signal from the host node
  BegLogLine( PKFXLOG_RUN_SORT )
    << "Entering Run_CommandProcessor()"
    << EndLogLine;

  unsigned long long FileInode = 0;
  unsigned long long FileSize = 0;

  int RequestedCommand = 0;

  char* GrepRegExp = NULL;
  
  if( PkNodeGetId() == 0 )
    {
      BGLPersonality Personality;
      rts_get_personality( &Personality, sizeof( BGLPersonality ) );
      
      char CommandFilePathName[ 256 ];
      char * BlockIdName = Personality.blockID;
      // sprintf( CommandFilePathName, "/ActiveSAN000/%s/public/Command.txt", BlockIdName );
      sprintf( CommandFilePathName, "/tmp/Command.txt", BlockIdName );
  
      BegLogLine( PKFXLOG_RUN_SORT )
  << "Run_CommandProcessor: "
  << " CommandFilePathName: " << CommandFilePathName
  << EndLogLine;
      

      int fp = -1;
      
      double LastOpen = 0.0;
      do
  {
    double CurrentTime = PkTimeGetSecs();
    
    if( CurrentTime - LastOpen  > 1.0 )
      {
        fp = open( CommandFilePathName, O_RDONLY );
        
        if( fp == -1 )
    {
      if ( errno == ENOENT )
        {
          LastOpen = CurrentTime;
          // sleep( 1 );
          PkFiberYield();
        }
      else
        {
          StrongAssertLogLine( 0 )
      << "Failed to open filename: " 
      << CommandFilePathName
      << EndLogLine;
        }
    }
        else
    break;
      }
    else
      {
        PkFiberYield();
      }
  }
      while( 1 );
      
      BegLogLine( PKFXLOG_RUN_SORT )
  << "Opened the command file: " << CommandFilePathName
  << EndLogLine;
      
      // Command file found in the asan file system
      // But not all the data might be visible yet.
      // fsync( fp );

      int CommandFileSize = 0;
      do
  {
    struct stat statbuf_commandfile;
    fstat( fp, &statbuf_commandfile );
    CommandFileSize = statbuf_commandfile.st_size;
    
    BegLogLine( PKFXLOG_RUN_SORT )
      << "CommandFileSize: " << CommandFileSize
      << EndLogLine;
    
    if( CommandFileSize == 0 )
      {
        // sleep( 1 );
      PkFiberYield();
      }	  
  } 
      while( CommandFileSize == 0 );
      
      char * CommandFileBuffer = (char *) PkHeapAllocate ( CommandFileSize );
      StrongAssertLogLine( CommandFileBuffer != NULL )
  << "ERROR: Allocating memory for command file buffer "
  << EndLogLine;      
      
      bzero( CommandFileBuffer, CommandFileSize );
      
      int SizeRead = 0;
      while( SizeRead < CommandFileSize )
  {
    int readLength = read( fp, 
         &CommandFileBuffer[ SizeRead ], 
         CommandFileSize - SizeRead );
    
    StrongAssertLogLine( readLength >= 0 )
      << "Failed to read the command file: " << CommandFilePathName
      << EndLogLine;

    SizeRead += readLength;
  }
            
      char delim[] = " ";
      char* CommandFileBufferPtr = CommandFileBuffer;
      char* CommandName = strsep( &CommandFileBufferPtr, delim );
      
      char* GrepRegExpString = NULL;
      char* GrepSearchFile   = NULL;
       
      if( strcmp( CommandName, "sort" )  == 0 )
  {
    RequestedCommand = SORT_COMMAND;
    char* SortFilename = strsep( &CommandFileBufferPtr, delim );	  
    
    BegLogLine( PKFXLOG_RUN_SORT )
      << "Doing a sort on filename: " << SortFilename
      << EndLogLine;
    
    int sortfile_fd = -1;
    do 
      {
        sortfile_fd = open( SortFilename, O_RDONLY );
        
        if( sortfile_fd <= 0 )
    {
      BegLogLine( 0 )
        << "Run_CommandProcessor:: Looking to open file: "
        << SortFilename
        << EndLogLine;
      
      // sleep( 1 );
      PkFiberYield();
    }
        else
    break;
      }
    while( sortfile_fd <= 0 );
    
    StrongAssertLogLine( sortfile_fd > 0 )
      << "ERROR:: Could not open sortfile: " << SortFilename
      << " errno: " << errno
      << EndLogLine;
    
    struct stat64 statbuf;
    fstat64( sortfile_fd, &statbuf );
    FileInode = statbuf.st_ino;
    FileSize  = statbuf.st_size;
    
    BegLogLine( PKFXLOG_RUN_SORT )
      << "Run_CommandProcessor():: Before the all reduce"
      << " SortFileInode: " << FileInode
      << " SortFileSize: " << FileSize
      << EndLogLine;
    
    PkHeapFree( CommandFileBuffer );
    CommandFileBuffer = NULL;
  }
      else if( strcmp( CommandName, "grep" )  == 0 )
  {
    RequestedCommand = GREP_COMMAND;

    GrepRegExp = strsep( &CommandFileBufferPtr, delim );	  
    
    GrepSearchFile = strsep( &CommandFileBufferPtr, delim );	  
    
    BegLogLine( PKFXLOG_RUN_SORT )
      << "Doing a grep on filename: " << GrepSearchFile
      << " GrepRegExp: " << GrepRegExp
      << EndLogLine;
    
    StrongAssertLogLine( GrepSearchFile != NULL )
      << "ERROR:: Grep file name is not found"
      << EndLogLine;      

    StrongAssertLogLine( GrepRegExp != NULL )
      << "ERROR:: GrepRegExp is not found"
      << EndLogLine;      
    
    int grepfile_fd = -1;
    do 
      {
        grepfile_fd = open( GrepSearchFile, O_RDONLY );
        
        if( grepfile_fd <= 0 )
    {
      BegLogLine( 0 )
        << "Run_CommandProcessor:: Looking to open file: "
        << GrepSearchFile
        << EndLogLine;
      
      // sleep( 1 );
      PkFiberYield();
    }
        else
    break;
      }
    while( grepfile_fd <= 0 );
    
    StrongAssertLogLine( grepfile_fd > 0 )
      << "ERROR:: Could not open sortfile: " << GrepSearchFile
      << " errno: " << errno
      << EndLogLine;
    
    struct stat64 statbuf;
    fstat64( grepfile_fd, &statbuf );
    FileInode = statbuf.st_ino;
    FileSize  = statbuf.st_size;
  }      
    }

#define FILE_INFO_BUFFER_SIZE ( 3 )
  unsigned long long FileInfoBuffer[ FILE_INFO_BUFFER_SIZE ];
  
  if( PkNodeGetId() != 0 )
    {
      StrongAssertLogLine( FileInode == 0 )
  << "Run_CommandProcessor():: On non-zero nodes, the FileInode has to be zero "
  << " FileInode: " << FileInode
  << EndLogLine;
      
      StrongAssertLogLine( FileSize == 0 )
  << "Run_CommandProcessor():: On non-zero nodes, the FileSize has to be zero "
  << " FileSize: " << FileSize
  << EndLogLine;
      
      StrongAssertLogLine( RequestedCommand == 0 )
  << "Run_CommandProcessor():: On non-zero nodes, the RequestedCommand has to be zero "
  << " RequestedCommand: " << RequestedCommand
  << EndLogLine;
    }

  FileInfoBuffer[ 0 ] = FileInode;
  FileInfoBuffer[ 1 ] = FileSize;
  FileInfoBuffer[ 2 ] = RequestedCommand;
    
  BegLogLine( PKFXLOG_RUN_SORT )
    << "Run_CommandProcessor():: Before the all reduce of "
    << " FileInode: " << FileInode
    << " FileSize: " << FileSize
    << " RequestedCommand: " << RequestedCommand
    << EndLogLine;  
  
  
  AllReduce.Execute( FileInfoBuffer, FILE_INFO_BUFFER_SIZE );
  
  // Every node has the inode for the file to sort
  FileInode         = FileInfoBuffer[ 0 ];
  FileSize          = FileInfoBuffer[ 1 ];
  RequestedCommand  = FileInfoBuffer[ 2 ];

  LocalBlocksForInodeManagerIF* LocalBlockManagerIF = asanMemoryServer.GetLocalBlocks( FileInode );
  LocalBlockManagerIF->ChecksumBlocks();
  
  // This serves the function of a barrier
  AllReduce.Execute( FileInfoBuffer, FILE_INFO_BUFFER_SIZE );

  switch( RequestedCommand )
    {
    case SORT_COMMAND:
      {	
  asan_sort( FileInode, FileSize );
  break;
      }
    case GREP_COMMAND:
      {	
  unsigned long long GrepRegExpSize = 0;
  if( PkNodeGetId() == 0 )
    {
          StrongAssertLogLine( GrepRegExp != NULL )
      << "ERROR:: GrepRegExp is not found"
      << EndLogLine;      
    
    // Need to broadcast the grep reg expression
    GrepRegExpSize = strlen( GrepRegExp );
    }
  
  AllReduce.Execute( &GrepRegExpSize, 1 );
  
  StrongAssertLogLine( GrepRegExpSize != 0 )
    << "ERROR:: GrepRepExpSize should not be zero"
    << EndLogLine;
  
  if( PkNodeGetId() != 0 )
    {
      // +1 for the null terminating character
      GrepRegExp = (char * )PkHeapAllocate( sizeof( char ) * (GrepRegExpSize+1) );

      StrongAssertLogLine( GrepRegExp != NULL )
        << "ERROR:: Not enough memory for GrepRegExp "
        << EndLogLine;
    }
  
  AllReduceChar.Execute( GrepRegExp, GrepRegExpSize+1 );

  asan_grep( FileInode, FileSize, GrepRegExp );

  break;
      }
    default: 
      {
  StrongAssertLogLine( 0 )
    << "Run_CommandProcessor:: Command: " << RequestedCommand
    << " is not supported."
    << EndLogLine;
      }
    }

  return;
}

enum {
  k_CoreIdForMemoryServer = CORE_FOR_MEMORY_SERVER ,
  k_CoreIdForCommandServer = CORE_FOR_COMMAND_SERVER ,
  k_RunCommandServer = RUN_SORT
};


int PkMain(int argc, char** argv, char** envp)
{  
  int TraceDumpFreq = atoi( argv[ 0 ] );
  int CoreId = rts_get_processor_id();
  
  BegLogLine( 1 )
    << "TraceDumpFreq: " << TraceDumpFreq
    << " CoreId: " << CoreId
    << EndLogLine;

  if( CoreId == 0 )
    {
    GlobalParallelObject::Init();
    AllReduce.Init( "AAA", PkNodeGetCount() );
    AllReduceChar.Init( "BBB", PkNodeGetCount() );
    asanMemoryServer.Init();
    }
  
  rts_dcache_evict_normal();
  PkIntraChipBarrier();
  
  if( k_CoreIdForMemoryServer == k_CoreIdForCommandServer)
    {
    // Running both parts as fibers on one core
    if( CoreId == k_CoreIdForMemoryServer)
      {
      if( k_RunCommandServer)
        {  
             PkFiberCreate( 1024 * 1024, Run_CommandProcessor_Fiber, NULL );
        }
         asanMemoryServer.Run( TraceDumpFreq );
      }
    }
  else
    {
     // Running as fibers on different cores
    
    if( CoreId == k_CoreIdForMemoryServer )
      asanMemoryServer.Run( TraceDumpFreq );  
    else
      {
      if( k_RunCommandServer)
        {  
             Run_CommandProcessor_Fiber( NULL );
        }
      else
        {
          while( 1 ) {
            PkFiberYield();
          }
        }
      }
    }


  return 0;
}


//int PkMainTmp(int argc, char** argv, char** envp)
//{  
//  int TraceDumpFreq = atoi( argv[ 0 ] );
//  int CoreId = rts_get_processor_id();
//  
//  BegLogLine( 1 )
//    << "TraceDumpFreq: " << TraceDumpFreq
//    << " CoreId: " << CoreId
//    << EndLogLine;
//
//  if( CoreId == 0 )
//    {
//    GlobalParallelObject::Init();
//    AllReduce.Init( "AAA", PkNodeGetCount() );
//    AllReduceChar.Init( "BBB", PkNodeGetCount() );
//    asanMemoryServer.Init();
//    
//    PkFiberCreate( 1024 * 1024, Run_CommandProcessor_Fiber, NULL );
//    
//    asanMemoryServer.Run( TraceDumpFreq );
//
//    }
//  else
//    while( 1 );
//  
//  return 0;
//}
