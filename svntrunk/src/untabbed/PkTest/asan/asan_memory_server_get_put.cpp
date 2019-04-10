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
 #include <asan_memory_server_bgltree.h>

double padd_localdatamap0[ 4 ];
LocalDataMap_T LocalDataMap;
int NSDInitialised ;
double padd_localdatamap1[ 4 ];

#define DEBUG_INDEX ( 0 )


#ifndef PKFXLOG_ASAN_GET_PUT
#define PKFXLOG_ASAN_GET_PUT ( 0 )
#endif

#ifndef PKFXLOG_ASAN_GET_PUT_DATA
#define PKFXLOG_ASAN_GET_PUT_DATA ( 0 )
#endif

TraceClient GPFS_ReadStart;
TraceClient GPFS_ReadFinis;

TraceClient GPFS_WriteStart;
TraceClient GPFS_WriteFinis;

#ifndef PKTRACE_GPFS_WRITE
#define PKTRACE_GPFS_WRITE ( 1 )
#endif

#ifndef PKTRACE_GPFS_READ
#define PKTRACE_GPFS_READ ( 1 )
#endif

// #define STL_MAP( Key, Data ) std::map< Key, Data, less< Key >, inmemdb::BGL_Alloc< std::pair< const Key, Data > > >

#define GPFS_METADATA_BLOCK_SIZE ( 128 * 1024 )
#define DATA_INODE_LIMIT (4000)

#define ASAN_MEMORY_PUT_DATA_BUFFER_SIZE ( 4 * ASAN_MEMORY_BLOCK_SIZE )
int  AsanPutDataBufferIndex;
char AsanPutDataBuffer[ ASAN_MEMORY_PUT_DATA_BUFFER_SIZE ];

#define GPFS_DISK_METADATA_BUFFER_SIZE ( 128 * GPFS_METADATA_BLOCK_SIZE )
int     GpfsDiskMetadataBufferIndex = 0;
char    GpfsDiskMetadataBuffer[ GPFS_DISK_METADATA_BUFFER_SIZE ];

struct GpfsDiskMetadataAccessKey
{
  unsigned BlockIndex;

  bool operator<(const GpfsDiskMetadataAccessKey& aKey ) const
  {
    // KeyCompare kc;
    return ( BlockIndex < aKey.BlockIndex );
  }
  
  bool operator<=(const GpfsDiskMetadataAccessKey& aKey ) const
  {
    return BlockIndex <= aKey.BlockIndex;
  }
};

struct BlockInfoPerInode
  {
    unsigned int                 mBlockCount;
    LocalBlocksForInodeManagerIF mLocalBlocksForInodeManagerIF; 
  };

typedef STL_MAP( int, BlockInfoPerInode ) InodeToBlockInfoMap_T;

InodeToBlockInfoMap_T InodeToBlockInfoMap;

typedef STL_MAP( GpfsDiskMetadataAccessKey, char* ) GpfsDiskMetadataMap_T;

GpfsDiskMetadataMap_T GpfsDiskMetadataMap;

char* GetGpfsMetadata( int BlockIndex )
  {
    GpfsDiskMetadataAccessKey key;
    key.BlockIndex = BlockIndex;
    
    GpfsDiskMetadataMap_T::iterator iter = GpfsDiskMetadataMap.find( key );

    if( iter != GpfsDiskMetadataMap.end() )
      {
  return (*iter).second;
      }
    
    return NULL;
  }

char * ReserveForGpfsMetadata( int BlockIndex )
  {
    StrongAssertLogLine( GpfsDiskMetadataBufferIndex < GPFS_DISK_METADATA_BUFFER_SIZE )
      << "Error:: No memory to satisfy a gpfs metadata map reservation" 
      << EndLogLine;

    char* ReservedGpfsBlock = & GpfsDiskMetadataBuffer[ GpfsDiskMetadataBufferIndex ];
    GpfsDiskMetadataBufferIndex += GPFS_METADATA_BLOCK_SIZE;

    GpfsDiskMetadataAccessKey key;
    key.BlockIndex = BlockIndex;

    int rc = GpfsDiskMetadataMap.insert( std::make_pair( key, ReservedGpfsBlock ) ).second;

    StrongAssertLogLine( rc > 0 )
      << "ERROR:: Insert into GpfsDiskMetadataMap failed" 
      << EndLogLine;
    
    return ReservedGpfsBlock;
  }

#ifdef ASAN_SINGLE_DISK_IMAGE
#include <asan_gpfs_metadata_get_put.h>
#endif

struct DataGetPacketRequest
{
  int Inode;
  long long BlockIndexInFile;
  int Length;  
  int OffsetInBlock;
  char* ReceiveBuffer;
};

struct DataGetPacketResponse
{
  char                   Length;
  char*                  ReceiveBuffer;
  pkFiberControlBlockT * FCB;
  char                   Data[ 0 ];
};

struct DataPutPacketRequest
  {
  int Inode;
  long long IndexOfDataInFile;
  int Length;  
  char Data[0];
  };

struct DataPutPacketResponse
  {
  pkFiberControlBlockT * FCB;
  };

struct SendDataGetResponseFxArgs
  {
  int      Length;  

  unsigned CurrentActorPacketSourceNode;
  unsigned CurrentActorPacketSourceCore;
  unsigned CurrentActorPacketContext;    
  };

int ProcessDataGetResponseFx( void * arg )
  {
  DataGetPacketResponse* Pkt = (DataGetPacketResponse *) arg;

  char* ReceiveBuffer = Pkt->ReceiveBuffer;
  int   Length        = Pkt->Length;  

  int * DataAsInt = (int *) ReceiveBuffer ;

  BegLogLine(PKFXLOG_ASAN_GET_PUT)
    << "ProcessDataGetResponseFx Length " << (int) Length
    << " Data 0x" << (void *) (DataAsInt[0])
    << (void *) (DataAsInt[1])
    << (void *) (DataAsInt[2])
    << (void *) (DataAsInt[3])
    << (void *) (DataAsInt[4])
    << (void *) (DataAsInt[5])
    << (void *) (DataAsInt[6])
    << (void *) (DataAsInt[7])
    << EndLogLine ;
  
  memcpy( ReceiveBuffer,
    Pkt->Data,
    Pkt->Length );
  
  // Unblock the requester fiber
  pkFiberUnblock( Pkt->FCB );
  
  return 0;
  }

#define MAX_DATA_BYTES_IN_RESPONSE_GET_PACKET ( _BGL_TORUS_PKT_MAX_PAYLOAD - sizeof( DataGetPacketResponse ) )
#define MAX_DATA_BYTES_IN_REQUEST_PUT_PACKET ( _BGL_TORUS_PKT_MAX_PAYLOAD - sizeof( DataPutPacketRequest ) )

void WrappedMemcpy(void * dest, void * src, int length)
{
    BegLogLine(PKFXLOG_ASAN_GET_PUT)
      << "WrappedMemcpy dest 0x" << dest
      << " src 0x" << src
      << " length " << length
      << EndLogLine ; 
    memcpy(dest,src,length) ;
}

void ChecksumBlock( int inode,
        long long blockIndex )
{
  InodeToBlockInfoMap_T::iterator iter = InodeToBlockInfoMap.find( inode );

  if( iter != InodeToBlockInfoMap.end() )
    {
      StrongAssertLogLine( iter->second.mLocalBlocksForInodeManagerIF.DoChecksum( blockIndex ) )
  << "ChecksumBlock:: ERROR:: checksum failure"
  << " inode: " << inode
  << " blockIndex: " << blockIndex	
  << EndLogLine;
    }
  else
    StrongAssertLogLine( 0 )
      << "ChecksumBlock:: ERROR:: Block not found."
      << " inode: " << inode
      << " blockIndex: " << blockIndex
      << EndLogLine;
}


char* GetLocalDataPtr( int inode,
           long long blockIndex,
           int indexInBlock )
{
  LocalDataAccessKey key;
  key.inode      = (inode >= DATA_INODE_LIMIT) ? inode : 0 ;
  key.blockIndex = blockIndex;

  LocalDataMap_T::iterator iter = LocalDataMap.find( key );
  
  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "GetLocalDataPtr: "
    << " inode: " << inode
    << " blockIndex: " << blockIndex
    << " indexInBlock: " << indexInBlock
    << " IsFound: " << (iter != LocalDataMap.end())
    << EndLogLine;
  
  if( iter != LocalDataMap.end() )
    {
      char* BlockPtr = (*iter).second.mBlockRep.mBlockPtr;

      AssertLogLine( indexInBlock >= 0 && indexInBlock < ASAN_MEMORY_BLOCK_SIZE )
  << "ERROR:: indexInBlock is out of bounds "
  << " indexInBlock: " << indexInBlock
  << " ASAN_MEMORY_BLOCK_SIZE: " << ASAN_MEMORY_BLOCK_SIZE
  << EndLogLine;

      return & BlockPtr[ indexInBlock ];
    }
  else
    return NULL;
}

int ProcessDataGetRequestFx( void* arg )
{  
  DataGetPacketRequest* DataRequestArgsPtr = (DataGetPacketRequest *) arg;

  int Length           = DataRequestArgsPtr->Length;
  int Inode            = DataRequestArgsPtr->Inode;
  long long BlockIndexInFile = DataRequestArgsPtr->BlockIndexInFile;
  int OffsetInBlock    = DataRequestArgsPtr->OffsetInBlock;
  char* ReceiveBuffer  = DataRequestArgsPtr->ReceiveBuffer;

  StrongAssertLogLine( Length <= MAX_DATA_BYTES_IN_RESPONSE_GET_PACKET )
    << "ERROR:: Length in request is larger then response packet capacity "
    << EndLogLine;
  
  int SizeToSend = sizeof( DataGetPacketResponse ) + Length;

  DataGetPacketResponse* ResPkt = (DataGetPacketResponse *)
    PkActorReserveResponsePacket( ProcessDataGetResponseFx,
          SizeToSend );
  
  // Assumes that length does not cross block boundaries.
  ResPkt->Length        = Length;
  ResPkt->ReceiveBuffer = ReceiveBuffer;

  unsigned long SourceNodeId;
  unsigned long SourceCoreId;
  pkFiberControlBlockT* Context = NULL;
  PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );   

  ResPkt->FCB           = Context;

  char* LocalDataPtr = GetLocalDataPtr( Inode, BlockIndexInFile, OffsetInBlock );

  StrongAssertLogLine( Inode < DATA_INODE_LIMIT || LocalDataPtr )
    << "ERROR:: LocalData does not exist when requested from: "
    << " NodeId: " << SourceNodeId
    << " Inode: " << Inode
    << " BlockIndexInFile: " << BlockIndexInFile
    << " OffsetInBlock: " << OffsetInBlock
    << EndLogLine;

    if ( LocalDataPtr != NULL )
    {
      int *LocalDataPtrInt = (int *) LocalDataPtr;
    
      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "ProcessDataGetRequestFx: "
        << " NodeId: " << SourceNodeId
        << " Inode: " << Inode
        << " BlockIndexInFile: " << BlockIndexInFile
        << " OffsetInBlock: " << OffsetInBlock
        << " LocalDataPtr[ " << DEBUG_INDEX << " ]: " << LocalDataPtrInt[ DEBUG_INDEX  ]
        << " @LocalDataPtr[ " << DEBUG_INDEX << " ]: " << (void *) & LocalDataPtrInt[ DEBUG_INDEX ]
        << EndLogLine;
    
          int * DataAsInt = (int *) LocalDataPtr ;
          BegLogLine(PKFXLOG_ASAN_GET_PUT)
            << "ProcessDataGetRequestFx Length " << (int) (ResPkt->Length)
            << " Data 0x" << (void *) (DataAsInt[0])
                          << (void *) (DataAsInt[1])
                          << (void *) (DataAsInt[2])
                          << (void *) (DataAsInt[3])
                          << (void *) (DataAsInt[4])
                          << (void *) (DataAsInt[5])
                          << (void *) (DataAsInt[6])
                          << (void *) (DataAsInt[7])
            << EndLogLine ;
      WrappedMemcpy( ResPkt->Data, LocalDataPtr, ResPkt->Length );
    } else {
        BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << " ProcessDataGetRequestFx: "
          << " Inode: " << Inode
          << " BlockIndexInFile: " << BlockIndexInFile
          << " OffsetInBlock: " << OffsetInBlock
          << " Length: " << (int) Length
          << " Supplying dummy zeros"
          << EndLogLine;
        
        bzero(ResPkt->Data,ResPkt->Length) ; 
    }

  PkActorPacketDispatch( ResPkt );

  return 0;
}


char * SendDataGetRequest( int NodeId, 
         int Inode, 
         long long BlockIndexInFile, 
         int OffsetInBlock,
         int Length, 
         char* RecvBuffer )
{
  StrongAssertLogLine( Length <= MAX_DATA_BYTES_IN_RESPONSE_GET_PACKET )
    << "SendDataGetRequest: "
    << " Length: " << (int) Length
    << " MAX_DATA_BYTES_IN_RESPONSE_GET_PACKET: " << MAX_DATA_BYTES_IN_RESPONSE_GET_PACKET
    << EndLogLine;
  
  DataGetPacketRequest* Pkt = (DataGetPacketRequest *) 
    PkActorReservePacket( NodeId,
        PkCoreGetId(),
        ProcessDataGetRequestFx,
        sizeof( DataGetPacketRequest ) );
  
  Pkt->Inode              = Inode;
  Pkt->BlockIndexInFile   = BlockIndexInFile;
  Pkt->Length             = Length;
  Pkt->ReceiveBuffer      = RecvBuffer;
  Pkt->OffsetInBlock      = OffsetInBlock;

  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "SendDataGetRequest: About to dispatch a packet to "
    << " NodeId: " << NodeId
    << " Inode: " << Inode
    << " BlockIndexInFile: " << BlockIndexInFile
    << " Length: " << (int) Length
    << " OffsetInBlock: " << OffsetInBlock
    << EndLogLine;

  PkActorPacketDispatch( Pkt );

  PkFiberBlock();

  BegLogLine( 0 ) 
    << "SendDataGetRequest: Leaving " 
    << EndLogLine; 

  return RecvBuffer;
}


char* ReserveLocalBlock( int aInode, long long BlockIndex )
{
  LocalDataAccessKey key;
  int Inode = (aInode >= DATA_INODE_LIMIT) ? aInode : 0 ;
  key.inode      = Inode ;
  key.blockIndex = BlockIndex;

  // char* ReservedBlockPtr = (char *) PkHeapAllocate( sizeof(char) * ASAN_MEMORY_BLOCK_SIZE );
  
  StrongAssertLogLine( ASAN_BufferManager::BufferIndex <  ASAN_BufferManager::BufferSize ) 
    << "ERROR:: Out of memory on reserving blocks" 
    << EndLogLine;   

  char* ReservedBlockPtr = ASAN_BufferManager::Buffer + ASAN_BufferManager::BufferIndex;
  ASAN_BufferManager::BufferIndex += ASAN_MEMORY_BLOCK_SIZE;

  LocalDataAccessRecord Record;
  Record.mBlockRep.mBlockPtr = ReservedBlockPtr;
  Record.IsFirstRecordTaken = 0;

  int rc = LocalDataMap.insert( std::make_pair( key, Record ) ).second;
  StrongAssertLogLine( rc > 0 )
    << "ERROR:: Insert of reserved block failed "
    << EndLogLine;

  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "ReserveLocalBlock:: Just created a new block and inserted into the LocalDataMap "
    << " Inode: " << Inode
    << " BlockIndex: " << BlockIndex
    << EndLogLine;

  // Update the metadata for the inode
  InodeToBlockInfoMap_T::iterator iter = InodeToBlockInfoMap.find( Inode );
  if( iter == InodeToBlockInfoMap.end() )
    {
      // This is the first inode entry 
      BlockInfoPerInode bipi;
      bipi.mBlockCount = 1;
      bipi.mLocalBlocksForInodeManagerIF.Init( Inode );
      bipi.mLocalBlocksForInodeManagerIF.AddBlock( BlockIndex, ReservedBlockPtr );
      
      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
      << "ReserveLocalBlock:: About to insert inode " << Inode
        << "  BlockInfoPerInode (" << bipi.mBlockCount
        << " " << Inode
        << " " << BlockIndex
        << " 0x" << (void *) ReservedBlockPtr
        << " ) pair into InodeBlockInfoMap"
      << EndLogLine;

      int rc = InodeToBlockInfoMap.insert( std::make_pair( Inode, bipi ) ).second;

      StrongAssertLogLine( rc > 0 )
  << "ERROR:: Insert of inode metadata failed "
  << EndLogLine;      
      
    }
  else
    {
      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
  << "ReserveLocalBlock:: About to add a block for "
  << " Inode: " << Inode
  << " BlockCount: " << iter->second.mBlockCount
  << EndLogLine;

      iter->second.mBlockCount++;
      iter->second.mLocalBlocksForInodeManagerIF.AddBlock( BlockIndex, ReservedBlockPtr );
      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
        << "ReserveLocalBlock:: About to add a block for inode " << Inode
        << "  iter->second.mBlockCount " << iter->second.mBlockCount
        << " " << BlockIndex
        << " 0x" << (void *) ReservedBlockPtr
        << EndLogLine;

    }  

  return ReservedBlockPtr;
}

int ProcessDataPutResponseFx( void* arg )
{
  DataPutPacketResponse* Pkt = (DataPutPacketResponse *) arg;
  
  pkFiberUnblock( Pkt->FCB );
  
  BegLogLine( 0 )
    << "ProcessDataPutResponseFx: " 
    << " Pkt->FCB: " << Pkt->FCB
    << EndLogLine;

  return 0;
}

int ProcessDataPutRequestFx( void* arg )
{  
  DataPutPacketRequest* Pkt = (DataPutPacketRequest *) arg;

  int Inode                     = Pkt->Inode;
  long long IndexOfDataInFile   = Pkt->IndexOfDataInFile;
  int Length                    = Pkt->Length;
  
  long long BlockIndexInFile = IndexOfDataInFile / ASAN_MEMORY_BLOCK_SIZE;
  int IndexOfDataInBlock        = IndexOfDataInFile % ASAN_MEMORY_BLOCK_SIZE;

  unsigned long SourceNodeId;
  unsigned long SourceCoreId;
  pkFiberControlBlockT* Context = NULL;
  PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );

  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "ProcessDataPutRequestFx: "
    << " Inode: " << Inode
    << " BlockIndexInFile: " << BlockIndexInFile
    << " Length: " << Length
    << " IndexOfDataInBlock: " << IndexOfDataInBlock
    << " SourceNodeId: " << SourceNodeId
    << " SourceCoreId: " << SourceCoreId
    << EndLogLine;

  char* LocalDataPtr = GetLocalDataPtr( Inode, BlockIndexInFile, IndexOfDataInBlock );
  if( LocalDataPtr == NULL )
    {
      // No record exists on this node
      char* BlockPtr = ReserveLocalBlock( Inode, BlockIndexInFile );

      LocalDataPtr = &BlockPtr[ IndexOfDataInBlock ];
    }
  
      int * DataAsInt = (int *) &(Pkt->Data) ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "ProcessDataPutRequestFx Length " << (int) Length
        << " Data 0x" << (void *) (DataAsInt[0])
  << (void *) (DataAsInt[1])
  << (void *) (DataAsInt[2])
  << (void *) (DataAsInt[3])
  << (void *) (DataAsInt[4])
  << (void *) (DataAsInt[5])
  << (void *) (DataAsInt[6])
  << (void *) (DataAsInt[7])
        << EndLogLine ;
  
      WrappedMemcpy( LocalDataPtr, 
         Pkt->Data, 
         Length );
  
  // NOTE:: THIS IS A STOP GAP, ONLY NEEDED UNTIL PK CAN DO SELF SENDS 
  // DUAL CORE
  rts_dcache_evict_normal();
  
  BegLogLine( 0 )
    << "TEMPORARY FIX: rts_dcache_evict_normal is called on every new block"
    << EndLogLine;

  DataPutPacketResponse* ResPkt = (DataPutPacketResponse *)
    PkActorReserveResponsePacket( ProcessDataPutResponseFx,
          sizeof( DataPutPacketResponse ) );
    
  ResPkt->FCB           = Context;

  PkActorPacketDispatch( ResPkt );

  int *LocalDataPtrInt = (int *) LocalDataPtr;

  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "ProcessDataPutRequestFx: "
    << " Inode: " << Inode
    << " BlockIndexInFile: " << BlockIndexInFile
    << " Length: " << (int) Length
    << " IndexOfDataInBlock: " << IndexOfDataInBlock
    << " ResPkt->FCB: " << ResPkt->FCB
    << " LocalDataPtr[ "<<DEBUG_INDEX<<" ]: " << LocalDataPtrInt[ DEBUG_INDEX ]
    << " @LocalDataPtr[ "<<DEBUG_INDEX<<" ]: " << (void *) & LocalDataPtrInt[ DEBUG_INDEX ]
    << EndLogLine;

  return 0;
}

void SendDataPutRequest( int       NodeId,
       int       Inode,
       long long IndexOfDataInFile,
       int       Length,
       char*     Data,
       int       IsLastPacket )
{
  StrongAssertLogLine( Length <= MAX_DATA_BYTES_IN_REQUEST_PUT_PACKET )
    << "SendDataGetRequest: "
    << " Length: " << (int) Length
    << " MAX_DATA_BYTES_IN_REQUEST_PUT_PACKET: " << MAX_DATA_BYTES_IN_REQUEST_PUT_PACKET
    << EndLogLine;

  int SizeOfPacket = Length + sizeof( DataPutPacketRequest );

  DataPutPacketRequest* Pkt = (DataPutPacketRequest *) 
    PkActorReservePacket( NodeId,
        PkCoreGetId(),
        ProcessDataPutRequestFx,
        SizeOfPacket );

  Pkt->Inode             = Inode;
  Pkt->IndexOfDataInFile = IndexOfDataInFile;
  Pkt->Length            = Length;

      int * DataAsInt = (int *) (Data) ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "SendDataPutRequest Pkt 0x" << (void *) Pkt
        << " Length " << (int) Length
        << " Data address 0x" << (void *) Data
        << " value 0x" << (void *) (DataAsInt[0])
                       << (void *) (DataAsInt[1])
                       << (void *) (DataAsInt[2])
                       << (void *) (DataAsInt[3])
                       << (void *) (DataAsInt[4])
                       << (void *) (DataAsInt[5])
                       << (void *) (DataAsInt[6])
                       << (void *) (DataAsInt[7])
                       << " ..."
        << EndLogLine ;
  WrappedMemcpy( &(Pkt->Data[0]), Data, Length );
  
  int * PktAsInt = (int *) Pkt ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "SendDataPutRequest Pkt address 0x" << (void *) Pkt
        << " Length " << (int) Length
        << " Pkt data 0x" << (void *) (PktAsInt[0])
                      << (void *) (PktAsInt[1])
                      << (void *) (PktAsInt[2])
                      << (void *) (PktAsInt[3])
                      << (void *) (PktAsInt[4])
                      << (void *) (PktAsInt[5])
                      << (void *) (PktAsInt[6])
                      << (void *) (PktAsInt[7])
                      << " ..."
        << EndLogLine ;
  

  PkActorPacketDispatch( Pkt );
  
  PkFiberBlock();
  
  return;
}

void PutAsanData( int       Inode, 
      long long IndexInFile, 
      char*     Data,
      int       Length,
      int       IsLastPacket )
{
  long long BlockIndex   = IndexInFile / ASAN_MEMORY_BLOCK_SIZE;
  int IndexOfDataInBlock = IndexInFile % ASAN_MEMORY_BLOCK_SIZE;
  
  int OwnerNodeId = GetDataOwner( Inode, BlockIndex );
  
  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "PutAsanData: "
    << " OwnerNodeId: " << OwnerNodeId
    << " Index " << IndexInFile
    << " BlockIndex: " << BlockIndex
    << " IndexOfDataInBlock: " << IndexOfDataInBlock
    << " Data 0x" << (void *) Data
    << " Length " << Length 
    << EndLogLine;

  if( OwnerNodeId == PkNodeGetId() )
    {
      char* LocalDataPtr = GetLocalDataPtr( Inode, BlockIndex, IndexOfDataInBlock );
      if( LocalDataPtr == NULL )
  {
    // No record exists on this node
    char* BlockPtr = ReserveLocalBlock( Inode, BlockIndex );
    LocalDataPtr   = & BlockPtr[ IndexOfDataInBlock ];
  }

      int * DataAsInt = (int *) Data ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "PutAsanData Length " << (int) Length
        << " Data 0x" << (void *) (DataAsInt[0])
                      << (void *) (DataAsInt[1])
                      << (void *) (DataAsInt[2])
                      << (void *) (DataAsInt[3])
                      << (void *) (DataAsInt[4])
                      << (void *) (DataAsInt[5])
                      << (void *) (DataAsInt[6])
                      << (void *) (DataAsInt[7])
        << EndLogLine ;
      WrappedMemcpy( LocalDataPtr, Data, Length );
      
      int *LocalDataPtrInt = (int *) LocalDataPtr;

      return;
    }
  else
    {
      // SendDataGetRequest will put the data in the local buffer;
      SendDataPutRequest( OwnerNodeId, 
        Inode, 
        IndexInFile,
        Length, 
        Data,
        IsLastPacket );
      
      
      return;
    }
}

void GetAsanDataPtr( int       Inode, 
         long long IndexInFile, 
         int       Length,
         char*     RecvBuffer )
{
  // Figure out the block offset of the request
  long long BlockIndex         = IndexInFile / ASAN_MEMORY_BLOCK_SIZE;
  int       IndexOfDataInBlock = IndexInFile % ASAN_MEMORY_BLOCK_SIZE;
    
  int OwnerNodeId = GetDataOwner( Inode, BlockIndex );

  if( OwnerNodeId == PkNodeGetId() )
    {
      char* LocalDataPtr =   GetLocalDataPtr( Inode, BlockIndex, IndexOfDataInBlock );

      StrongAssertLogLine( Inode < DATA_INODE_LIMIT || LocalDataPtr != NULL )
  << "ERROR:: Data at " 
  << " Inode: " << Inode
  << " IndexInFile: " << IndexInFile
  << " Length: " << (int) Length
  << " does not exist"
  << EndLogLine;
      
    if ( LocalDataPtr != NULL )
    {
      int *LocalDataPtrInt = (int *) LocalDataPtr;

      BegLogLine( PKFXLOG_ASAN_GET_PUT )
  << " GetAsanData: "
  << " Inode: " << Inode
  << " BlockIndexInFile: " << BlockIndex
  << " Length: " << (int) Length
  << " LocalDataPtr[ "<<DEBUG_INDEX<<" ]: 0x" << (void *)(LocalDataPtrInt[ DEBUG_INDEX ])
  << EndLogLine;

      int * DataAsInt = (int *) LocalDataPtr ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "GetAsanData Length " << (int) Length
        << " Data 0x" << (void *) (DataAsInt[0])
                      << (void *) (DataAsInt[1])
                      << (void *) (DataAsInt[2])
                      << (void *) (DataAsInt[3])
                      << (void *) (DataAsInt[4])
                      << (void *) (DataAsInt[5])
                      << (void *) (DataAsInt[6])
                      << (void *) (DataAsInt[7])
        << EndLogLine ;
      WrappedMemcpy( RecvBuffer, LocalDataPtr, Length );
    } else {
        
      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << " GetAsanData: "
        << " Inode: " << Inode
        << " BlockIndexInFile: " << BlockIndex
        << " Length: " << (int) Length
        << " Supplying dummy zeros"
        << EndLogLine;
       
       bzero(RecvBuffer,Length) ;
    }
    }
  else
    {
      // SendDataGetRequest will put the data in the local buffer;
      char* LocalDataPtr = SendDataGetRequest( OwnerNodeId, 
                 Inode, 
                 BlockIndex, 
                 IndexOfDataInBlock, 
                 Length, 
                 RecvBuffer );
    }
}

char * ASAN_BufferManager::Buffer = NULL;
int ASAN_BufferManager::BufferSize = 0;
int ASAN_BufferManager::BufferIndex = 0;

void ASAN_BufferManager::AllocateBuffer( int MBCount )
  {
  int   MemoryBufferMB   = MBCount;
  BufferSize = MemoryBufferMB * 1024 * 1024;
  Buffer     = (char *) PkHeapAllocate( BufferSize );
  
  while( Buffer == NULL )
    {
      MemoryBufferMB--;
      BufferSize = MemoryBufferMB * 1024 * 1024;
      Buffer     = (char *) PkHeapAllocate( BufferSize );      
      
      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
  << "ASAN_BufferManager::AllocateBuffer : " << (void *) Buffer
  << " MemoryBufferMB: " << MemoryBufferMB
  << " BufferSize: " <<  BufferSize 
  << EndLogLine;
    }

  bzero( Buffer, BufferSize );

  BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
    << "ASAN_BufferManager::AllocateBuffer : " << (void *) Buffer
    << " MemoryBufferMB: " << MemoryBufferMB
    << " BufferSize: " <<  BufferSize 
    << EndLogLine;
  }

int ASAN_MemoryServer::GetLocalBlocksCount( int inode )
  {
    InodeToBlockInfoMap_T::iterator iter = InodeToBlockInfoMap.find( inode );
    if( iter != InodeToBlockInfoMap.end() )
      {
  return iter->second.mBlockCount;
      }
    else
      return 0;
  }

LocalBlocksForInodeManagerIF * ASAN_MemoryServer::GetLocalBlocks( int inode )
  {
    InodeToBlockInfoMap_T::iterator iter = InodeToBlockInfoMap.find( inode );
    if( iter != InodeToBlockInfoMap.end() )
      {
  return & ( iter->second.mLocalBlocksForInodeManagerIF );
      }
    else
      return NULL;    
  }

void ASAN_MemoryServer::Init()
{
  CurrentState = MEMORY_SERVER_STATE_CONNECTED;
  BufferPtr = NULL;
  
  BGLPersonality Personality;
  rts_get_personality( &Personality, sizeof( BGLPersonality ) );

  unsigned MyIONodeTreeAddress = Personality.treeIONodeP2PAddr();

  BGLTreeMakeSendHdr( &TreeHdr, 0, MyIONodeTreeAddress );
  
  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "ASAN_MemoryServer:: MyIONodeTreeAddress " <<  MyIONodeTreeAddress 
    << EndLogLine;
  ASAN_BufferManager::AllocateBuffer( 200 );
  SetBufferPtr( ASAN_BufferManager::GetBufferPtr() );
  
  // Signal the I/O node to start gpfs
  char SignalFile[] = "/var/asan/ComputeNodeReady";
  int fd = open( SignalFile, 
     O_CREAT | O_TRUNC | O_RDWR,
     S_IRUSR | S_IWUSR );

  StrongAssertLogLine( fd >= 0 )
    << "ASAN_MemoryServer:: Error creating the file: " << fd
    << "errno: " << errno
    << EndLogLine;

  close( fd );
     
  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "ASAN_MemoryServer:: Created a file: " <<  SignalFile 
    << EndLogLine;
}

void ASAN_MemoryServer::SetBufferPtr( char * aBufferPtr )
{
  BufferPtr = aBufferPtr;
}

char* 
ASAN_BufferManager::GetBufferPtr()
{
  return Buffer;
}

int 
ASAN_BufferManager::GetBufferSize()
{
  return BufferSize;
}

void ASAN_MemoryServer::SendOnTree( char*          Data, 
            int            Len, 
            unsigned long* ResponseReceivedPtr,
            char*          BufferBase )
{
  int BytesOfDataInFullPacket = _BGL_TREE_PKT_MAX_BYTES - sizeof( ComputeNodePutPacket );
  int NumberOfFullPackets     = Len / BytesOfDataInFullPacket;
  int BytesOfDataInLastPacket = Len % BytesOfDataInFullPacket;

  int CurrentIndex = 0;
  for( int i = 0; i < NumberOfFullPackets; i++ )
    {
      ComputeNodePutPacket* cnPutPacketPtr = (ComputeNodePutPacket* ) SendTreePayload;
      
      cnPutPacketPtr->BufferStart =  BufferBase + CurrentIndex;
      cnPutPacketPtr->ResponseReceivedPtr = ResponseReceivedPtr;
      
      if( (i == NumberOfFullPackets-1) && (BytesOfDataInLastPacket == 0) )
  cnPutPacketPtr->IsLastPacket = 1;
      else
  cnPutPacketPtr->IsLastPacket = 0;
      
      cnPutPacketPtr->Length = BytesOfDataInFullPacket;
            
      int * DataAsInt = (int *) (& Data[ CurrentIndex ]) ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x00])
                      << (void *) (DataAsInt[0x01])
                      << (void *) (DataAsInt[0x02])
                      << (void *) (DataAsInt[0x03])
                      << (void *) (DataAsInt[0x04])
                      << (void *) (DataAsInt[0x05])
                      << (void *) (DataAsInt[0x06])
                      << (void *) (DataAsInt[0x07])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x08])
                      << (void *) (DataAsInt[0x09])
                      << (void *) (DataAsInt[0x0a])
                      << (void *) (DataAsInt[0x0b])
                      << (void *) (DataAsInt[0x0c])
                      << (void *) (DataAsInt[0x0d])
                      << (void *) (DataAsInt[0x0e])
                      << (void *) (DataAsInt[0x0f])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x10])
                      << (void *) (DataAsInt[0x11])
                      << (void *) (DataAsInt[0x12])
                      << (void *) (DataAsInt[0x13])
                      << (void *) (DataAsInt[0x14])
                      << (void *) (DataAsInt[0x15])
                      << (void *) (DataAsInt[0x16])
                      << (void *) (DataAsInt[0x17])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x18])
                      << (void *) (DataAsInt[0x19])
                      << (void *) (DataAsInt[0x1a])
                      << (void *) (DataAsInt[0x1b])
                      << (void *) (DataAsInt[0x1c])
                      << (void *) (DataAsInt[0x1d])
                      << (void *) (DataAsInt[0x1e])
                      << (void *) (DataAsInt[0x1f])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x20])
                      << (void *) (DataAsInt[0x21])
                      << (void *) (DataAsInt[0x22])
                      << (void *) (DataAsInt[0x23])
                      << (void *) (DataAsInt[0x24])
                      << (void *) (DataAsInt[0x25])
                      << (void *) (DataAsInt[0x26])
                      << (void *) (DataAsInt[0x27])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x28])
                      << (void *) (DataAsInt[0x29])
                      << (void *) (DataAsInt[0x2a])
                      << (void *) (DataAsInt[0x2b])
                      << (void *) (DataAsInt[0x2c])
                      << (void *) (DataAsInt[0x2d])
                      << (void *) (DataAsInt[0x2e])
                      << (void *) (DataAsInt[0x2f])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x30])
                      << (void *) (DataAsInt[0x31])
                      << (void *) (DataAsInt[0x32])
                      << (void *) (DataAsInt[0x33])
                      << (void *) (DataAsInt[0x34])
                      << (void *) (DataAsInt[0x35])
                      << (void *) (DataAsInt[0x36])
                      << (void *) (DataAsInt[0x37])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x38])
                      << (void *) (DataAsInt[0x39])
                      << (void *) (DataAsInt[0x3a])
                      << (void *) (DataAsInt[0x3b])
                      << (void *) (DataAsInt[0x3c])
                      << (void *) (DataAsInt[0x3d])
                      << (void *) (DataAsInt[0x3e])
                      << (void *) (DataAsInt[0x3f])
        << EndLogLine ;
      WrappedMemcpy( cnPutPacketPtr->DataPtr,
        & Data[ CurrentIndex ], 
        cnPutPacketPtr->Length );
      
      CurrentIndex += cnPutPacketPtr->Length;

      StrongAssertLogLine( CurrentIndex <= Len )
  << "CurrentIndex: " << CurrentIndex
  << " Len: " << Len
  << EndLogLine;
      
      writePacket( &TreeHdr, SendTreePayload );
      
      BegLogLine( 0 )
  << "Wrote packet:  "
  << " BufferStart: "  << (void *) cnPutPacketPtr->BufferStart
  << " Length: "       << cnPutPacketPtr->Length
  << " IsLastPacket: " << cnPutPacketPtr->IsLastPacket
  << " ResponseReceivedPtr: " << (void *) cnPutPacketPtr->ResponseReceivedPtr
  << EndLogLine;      
    } 
  
  if( BytesOfDataInLastPacket )
    {
      ComputeNodePutPacket* cnPutPacketPtr = (ComputeNodePutPacket* ) SendTreePayload;
      
      cnPutPacketPtr->BufferStart =  BufferBase + CurrentIndex;
      cnPutPacketPtr->ResponseReceivedPtr = ResponseReceivedPtr;		      
      cnPutPacketPtr->IsLastPacket = 1;
      
      cnPutPacketPtr->Length = BytesOfDataInLastPacket;
      
      WrappedMemcpy( cnPutPacketPtr->DataPtr, 
        & Data[ CurrentIndex ], 
        cnPutPacketPtr->Length );
      
      int * DataAsInt = (int *) (& (cnPutPacketPtr->DataPtr )) ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x00])
                      << (void *) (DataAsInt[0x01])
                      << (void *) (DataAsInt[0x02])
                      << (void *) (DataAsInt[0x03])
                      << (void *) (DataAsInt[0x04])
                      << (void *) (DataAsInt[0x05])
                      << (void *) (DataAsInt[0x06])
                      << (void *) (DataAsInt[0x07])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x08])
                      << (void *) (DataAsInt[0x09])
                      << (void *) (DataAsInt[0x0a])
                      << (void *) (DataAsInt[0x0b])
                      << (void *) (DataAsInt[0x0c])
                      << (void *) (DataAsInt[0x0d])
                      << (void *) (DataAsInt[0x0e])
                      << (void *) (DataAsInt[0x0f])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x10])
                      << (void *) (DataAsInt[0x11])
                      << (void *) (DataAsInt[0x12])
                      << (void *) (DataAsInt[0x13])
                      << (void *) (DataAsInt[0x14])
                      << (void *) (DataAsInt[0x15])
                      << (void *) (DataAsInt[0x16])
                      << (void *) (DataAsInt[0x17])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x18])
                      << (void *) (DataAsInt[0x19])
                      << (void *) (DataAsInt[0x1a])
                      << (void *) (DataAsInt[0x1b])
                      << (void *) (DataAsInt[0x1c])
                      << (void *) (DataAsInt[0x1d])
                      << (void *) (DataAsInt[0x1e])
                      << (void *) (DataAsInt[0x1f])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x20])
                      << (void *) (DataAsInt[0x21])
                      << (void *) (DataAsInt[0x22])
                      << (void *) (DataAsInt[0x23])
                      << (void *) (DataAsInt[0x24])
                      << (void *) (DataAsInt[0x25])
                      << (void *) (DataAsInt[0x26])
                      << (void *) (DataAsInt[0x27])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x28])
                      << (void *) (DataAsInt[0x29])
                      << (void *) (DataAsInt[0x2a])
                      << (void *) (DataAsInt[0x2b])
                      << (void *) (DataAsInt[0x2c])
                      << (void *) (DataAsInt[0x2d])
                      << (void *) (DataAsInt[0x2e])
                      << (void *) (DataAsInt[0x2f])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x30])
                      << (void *) (DataAsInt[0x31])
                      << (void *) (DataAsInt[0x32])
                      << (void *) (DataAsInt[0x33])
                      << (void *) (DataAsInt[0x34])
                      << (void *) (DataAsInt[0x35])
                      << (void *) (DataAsInt[0x36])
                      << (void *) (DataAsInt[0x37])
        << EndLogLine ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT_DATA)
        << "ASAN_MemoryServer::SendOnTree Length " << cnPutPacketPtr->Length
        << " Data 0x" << (void *) (DataAsInt[0x38])
                      << (void *) (DataAsInt[0x39])
                      << (void *) (DataAsInt[0x3a])
                      << (void *) (DataAsInt[0x3b])
                      << (void *) (DataAsInt[0x3c])
                      << (void *) (DataAsInt[0x3d])
                      << (void *) (DataAsInt[0x3e])
                      << (void *) (DataAsInt[0x3f])
        << EndLogLine ;
      CurrentIndex += cnPutPacketPtr->Length;
      
      writePacket( &TreeHdr, SendTreePayload );
      
      BegLogLine( 0 )
  << "Wrote packet:  "
  << " BufferStart: "  << (void *) cnPutPacketPtr->BufferStart
  << " Length: "       << cnPutPacketPtr->Length
  << " IsLastPacket: " << cnPutPacketPtr->IsLastPacket
  << " ResponseReceivedPtr: " << (void *) cnPutPacketPtr->ResponseReceivedPtr
  << EndLogLine;
    }
  
  StrongAssertLogLine( CurrentIndex == Len )
    << "ASAN_MemoryServer::SendOnTree:: "
    << " CurrentIndex: " << CurrentIndex
    << " Len: " << Len
    << EndLogLine;
}

//int main(int argc, char* argv[])
void ASAN_MemoryServer::Run(int TraceDumpFreq )
{
  BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "sizeof( DataHashArgs ): "  << sizeof( DataHashArgs )
    << " sizeof( unsigned int ): " << sizeof( unsigned int )
    << " sizeof( DataHashArgs ) / sizeof ( unsigned int ): " << sizeof( DataHashArgs ) / sizeof ( unsigned int )
    << EndLogLine;

  unsigned long long counter = 0;

  while( 1 )
    {      
      BegLogLine( PKFXLOG_ASAN_GET_PUT )
  << "ASAN_MemoryServer:: Entering current state: " <<  CurrentState
  << EndLogLine;
      
#if defined(PKTRACE)
      if ( TraceDumpFreq != 0 )
         {
        if(counter % TraceDumpFreq == 0 )
        {
          pkTraceServer::FlushBuffer() ;
        }
        counter++;
         } 
#endif

      switch( CurrentState )
  {
  case MEMORY_SERVER_STATE_CONNECTED:
    {
      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "ASAN_MemoryServer:: Entering : MEMORY_SERVER_STATE_CONNECTED"
        << EndLogLine;

      // Read and process incoming events
      readPacket( RecvTreePayload );
      
      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "ASAN_MemoryServer:: "
        << " RecvTreePayload: " << (void *) RecvTreePayload
        << EndLogLine;

      Gpfs2CN_Header* hdr = (Gpfs2CN_Header *) RecvTreePayload;

      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "ASAN_MemoryServer:: Operation: " << hdr->Op
        << EndLogLine;
      
      switch( hdr->Op )
        {
        case GPFS2CN_OPERATION_CONNECT:
    {
      // This is the result of multiple threads opening
      // disks. We could get a connect request in a connected state
      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
        << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_CONNECT "
        << "Warning:: Got connection request when memory server " 
        << " is in a connected state, as if memory client Disconnect() "
        << " was never called\n" 
        << EndLogLine;

      // SendConnectionAck();
      break;
    }
        case GPFS2CN_OPERATION_DISCONNECT:
    {    
      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
        << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_DISCONNECT "
        << EndLogLine;
      
      // SendDisconnectionAck();

      // CurrentState = MEMORY_SERVER_STATE_DISCONNECTED;
      break;
    }
        case GPFS2CN_OPERATION_READ:
    {
      GPFS_ReadStart.HitOE( PKTRACE_GPFS_READ,
          "GPFS_ReadStart",
          Platform::Thread::GetId(),
          GPFS_ReadStart );
      
      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
        << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_READ"
        << EndLogLine;		  

      Gpfs2CN_ReadRequest* ReadRequest = 
        (Gpfs2CN_ReadRequest *) (RecvTreePayload + sizeof( Gpfs2CN_Header ));
            
      unsigned int len = ReadRequest->Len;
      unsigned long long offset = ReadRequest->Offset;

      int inode                 = ReadRequest->Inode;
      unsigned long long offsetInFile    = ReadRequest->OffsetInFile * ASAN_MEMORY_BLOCK_SIZE;

      unsigned long* ResponseReceivedPtr = ReadRequest->ResponseReceivedPtr;
      char*          BufferBase          = ReadRequest->BufferOnIONode;

      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
        << "ASAN_MemoryServer:: About to read " 
        << " offset: " << offset 
        << " len: " << len 
        << " inode: " << inode
        << " offsetInFile: " << offsetInFile
        << " ResponseReceivedPtr: 0x" << (void *) ResponseReceivedPtr
        << " BufferBase: 0x" << (void *) BufferBase
        << EndLogLine;
      
      if( inode < DATA_INODE_LIMIT )
        {
#if defined(ASAN_SINGLE_DISK_IMAGE)
          //          if ( inode < 4096 )
          //          {
          //            BegLogLine( PKFXLOG_ASAN_GET_PUT )
          //              << "ASAN_MemoryServer Read in metadata, using fake inode 0"
          //              << EndLogLine ;
          //            inode = 0 ;
          //          }
          
          // + 0 is to account for the char DataPtr[0] in the sizeof( ComputeNodePutPacket )
          int BytesOfDataInFullTreePacket = _BGL_TREE_PKT_MAX_BYTES - sizeof( ComputeNodePutPacket ) ;
          int BytesInFullTorusPacket      = MAX_DATA_BYTES_IN_RESPONSE_GET_PACKET;
          
          int BytesOfDataInFullPacket = min( BytesInFullTorusPacket, BytesOfDataInFullTreePacket );
          
          int NumberOfFullPackets      = len / BytesOfDataInFullPacket;
          int BytesOfDataInLastPacket  = len % BytesOfDataInFullPacket;
          
          BegLogLine( PKFXLOG_ASAN_GET_PUT )
      << "NumberOfFullPackets: " << NumberOfFullPackets
      << " BytesOfDataInLastPacket: " << BytesOfDataInLastPacket
      << " BytesOfDataInFullPacket: " << BytesOfDataInFullPacket
      << EndLogLine;
          
          int IndexInReadRequest = 0;
          
          char PacketRecvBuffer[ 4 * ASAN_MEMORY_BLOCK_SIZE ];
          
          StrongAssertLogLine( len <= 4 * ASAN_MEMORY_BLOCK_SIZE )
      << "ERROR:: Length request exceeds buffer length "
      << " len: " << len
      << " buffer length: " << (4 * ASAN_MEMORY_BLOCK_SIZE)
      << EndLogLine;
          
          for( int i = 0; i < NumberOfFullPackets; i++ )
      {
        long long Index = offset + IndexInReadRequest;
        
        GetAsanDataPtr( inode,
            Index,
            BytesOfDataInFullPacket,
            & PacketRecvBuffer[ IndexInReadRequest ] );
        
        int* DataInt = (int *) & PacketRecvBuffer[ IndexInReadRequest ];
        
        BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "Metadata READ "
          << " Inode: " << inode
          << " Index: " << Index
          << " IndexInReadRequest: " << IndexInReadRequest
          << " Length: " << BytesOfDataInFullPacket
          << " DataInt[ "<<DEBUG_INDEX<<" ]: 0x" << (void *)(DataInt[ DEBUG_INDEX ])
          << EndLogLine;
        
        IndexInReadRequest += BytesOfDataInFullPacket;
      }
          
          if( BytesOfDataInLastPacket )
      {
        long long Index = offset + IndexInReadRequest;
        
        GetAsanDataPtr( inode,
            Index,
            BytesOfDataInLastPacket,
            & PacketRecvBuffer[ IndexInReadRequest ] );
        
        int* DataInt = (int *) & PacketRecvBuffer[ IndexInReadRequest ];
        
        BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "Metadata READ "
          << " Inode: " << inode
          << " Index: " << Index
          << " IndexInReadRequest: " << IndexInReadRequest
          << " BytesOfDataInLastPacket: " << BytesOfDataInLastPacket
          << " DataInt[ "<<DEBUG_INDEX<<" ]: 0x" << (void *)(DataInt[ DEBUG_INDEX ])
          << EndLogLine;
        IndexInReadRequest += BytesOfDataInLastPacket;
      }
          
          SendOnTree( PacketRecvBuffer, IndexInReadRequest,
          ResponseReceivedPtr, BufferBase );
#else            
          // This is a read request for gpfs disk metadata
          // Check to see if this offset is in the map.
        int BlockIndex          = offset / GPFS_METADATA_BLOCK_SIZE;
        int OffsetOfDataInBlock = offset % GPFS_METADATA_BLOCK_SIZE;
        
        // Get the storage of the gpfs metadata
        char* StartOfMetadataBlock = GetGpfsMetadata( BlockIndex );
        if( StartOfMetadataBlock == NULL )
    StartOfMetadataBlock = ReserveForGpfsMetadata( BlockIndex );
        
        char* RequestedDataPtr = & StartOfMetadataBlock[ OffsetOfDataInBlock ];
        
        // This is the distance RequestedDataPtr to the end of the block;		      
        int DataLeftInBlock = GPFS_METADATA_BLOCK_SIZE - OffsetOfDataInBlock;
              
              BegLogLine(PKFXLOG_ASAN_GET_PUT)
                << "ASAN_MemoryServer:: gpfs-disk-metadata BlockIndex=" << BlockIndex
                << " OffsetOfDataInBlock=" << OffsetOfDataInBlock
                << " StartOfMetadataBlock=0x" << (void *)StartOfMetadataBlock
                << " RequestedDataPtr=0x" << (void *)RequestedDataPtr
                << " DataLeftInBlock=" << DataLeftInBlock
                << EndLogLine ;
        
        StrongAssertLogLine( len <= DataLeftInBlock )
    << "ERROR:: GPFS metadata request spans blocks "
    << EndLogLine;
        
        // #ifdef ASAN_SINGLE_DISK_IMAGE
        //		      // Get the actual data from node 0
        //		      GetAsanGpfsMetadata( 0, BlockIndex, OffsetOfDataInBlock, 
        //					   RequestedDataPtr, len );
        // #endif
        // Send the packet over the tree.
        SendOnTree( RequestedDataPtr, len, 
        ResponseReceivedPtr, BufferBase );
        
#endif        
      } 		  
          else 
      {	      
        // + 0 is to account for the char DataPtr[0] in the sizeof( ComputeNodePutPacket )
        int BytesOfDataInFullTreePacket = _BGL_TREE_PKT_MAX_BYTES - sizeof( ComputeNodePutPacket ) ;
        int BytesInFullTorusPacket      = MAX_DATA_BYTES_IN_RESPONSE_GET_PACKET;
        
        int BytesOfDataInFullPacket = min( BytesInFullTorusPacket, BytesOfDataInFullTreePacket );
        
        int NumberOfFullPackets      = len / BytesOfDataInFullPacket;
        int BytesOfDataInLastPacket  = len % BytesOfDataInFullPacket;
        
        BegLogLine( PKFXLOG_ASAN_GET_PUT )
    << "NumberOfFullPackets: " << NumberOfFullPackets
    << " BytesOfDataInLastPacket: " << BytesOfDataInLastPacket
    << " BytesOfDataInFullPacket: " << BytesOfDataInFullPacket
    << EndLogLine;
        
        int IndexInReadRequest = 0;
        
        char PacketRecvBuffer[ 4 * ASAN_MEMORY_BLOCK_SIZE ];
        
        StrongAssertLogLine( len <= 4 * ASAN_MEMORY_BLOCK_SIZE )
    << "ERROR:: Length request exceeds buffer length "
    << " len: " << len
    << " buffer length: " << (4 * ASAN_MEMORY_BLOCK_SIZE)
    << EndLogLine;
        
        for( int i = 0; i < NumberOfFullPackets; i++ )
    {
      long long IndexInFile = offsetInFile + IndexInReadRequest;
      
      GetAsanDataPtr( inode,
          IndexInFile,
          BytesOfDataInFullPacket,
          & PacketRecvBuffer[ IndexInReadRequest ] );
      
      int* DataInt = (int *) & PacketRecvBuffer[ IndexInReadRequest ];

      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "READ "
        << " Inode: " << inode
        << " IndexInFile: " << IndexInFile
        << " IndexInReadRequest: " << IndexInReadRequest
        << " Length: " << BytesOfDataInFullPacket
        << " DataInt[ "<<DEBUG_INDEX<<" ]: " << DataInt[ DEBUG_INDEX ]
        << EndLogLine;
      
      IndexInReadRequest += BytesOfDataInFullPacket;
    }
        
        if( BytesOfDataInLastPacket )
    {
      long long IndexInFile = offsetInFile + IndexInReadRequest;
      
      GetAsanDataPtr( inode,
          IndexInFile,
          BytesOfDataInLastPacket,
          & PacketRecvBuffer[ IndexInReadRequest ] );
      
      int* DataInt = (int *) & PacketRecvBuffer[ IndexInReadRequest ];
      
      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "READ "
        << " Inode: " << inode
        << " IndexInFile: " << IndexInFile
        << " IndexInReadRequest: " << IndexInReadRequest
        << " Length: " << BytesOfDataInLastPacket
        << " DataInt[ "<<DEBUG_INDEX<<" ]: " << DataInt[ DEBUG_INDEX ]
        << EndLogLine;
      IndexInReadRequest += BytesOfDataInLastPacket;
    }
        
        SendOnTree( PacketRecvBuffer, IndexInReadRequest,
        ResponseReceivedPtr, BufferBase );
        
        
      }
      
    GPFS_ReadFinis.HitOE( PKTRACE_GPFS_READ,
        "GPFS_ReadFinis",
        Platform::Thread::GetId(),
        GPFS_ReadFinis );
          BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
            << "ASAN_MemoryServer Read completed"
            << EndLogLine;
    
  } 
   break ; 
    case GPFS2CN_OPERATION_WRITE:
        {
          BegLogLine( PKFXLOG_ASAN_GET_PUT )
            << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_WRITE"
            << EndLogLine;
    
          GPFS_WriteStart.HitOE( PKTRACE_GPFS_WRITE,
               "GPFS_WriteStart",
               Platform::Thread::GetId(),
               GPFS_WriteStart );
    
          Gpfs2CN_WriteRequest* WriteRequest = 
            (Gpfs2CN_WriteRequest *) (RecvTreePayload + sizeof( Gpfs2CN_Header ));
          
          unsigned int len = WriteRequest->Len;
          unsigned long long offset = WriteRequest->Offset;
          
          int inode = WriteRequest->Inode;
          unsigned long long offsetInFile = WriteRequest->OffsetInFile * ASAN_MEMORY_BLOCK_SIZE;
    
          // BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
          BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
            << "ASAN_MemoryServer:: About to write " 
            << " offset: " << offset 
            << " len: " << len 
            << " inode: " << inode
            << " offsetInFile: " << offsetInFile
            << EndLogLine;
    
          // unsigned int rlen = readAll( (char *) &BufferPtr[offset], len );
          
          int BytesInFullPutPacket      = MAX_DATA_BYTES_IN_REQUEST_PUT_PACKET;
    
          int NumberOfFullPackets      = len / BytesInFullPutPacket;
          int BytesOfDataInLastPacket  = len % BytesInFullPutPacket;
    
        StrongAssertLogLine( len <= ASAN_MEMORY_PUT_DATA_BUFFER_SIZE )
          << "ERROR:: Local buffer is smaller then requested write"
          << EndLogLine;
        
        BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "OPERATION_WRITE: About to write: " 
          << " AsanPutDataBuffer: 0x" << (void *) AsanPutDataBuffer
          << " ASAN_MEMORY_PUT_DATA_BUFFER_SIZE: " << ASAN_MEMORY_PUT_DATA_BUFFER_SIZE
          << EndLogLine;
        
          unsigned int rlen = readAll( AsanPutDataBuffer, 
               len );
          
          
        if( PKFXLOG_ASAN_GET_PUT )
          {
            int * DataAsInt = (int *)AsanPutDataBuffer ;
            BegLogLine( PKFXLOG_ASAN_GET_PUT )
              << "AsanPutDataBuffer[0x00000000] 0x" << (void *)(DataAsInt[0])
                        << (void *)(DataAsInt[1])
                    << (void *)(DataAsInt[2])
                    << (void *)(DataAsInt[3])
                    << (void *)(DataAsInt[4])
                    << (void *)(DataAsInt[5])
                    << (void *)(DataAsInt[6])
                    << (void *)(DataAsInt[7])
            << EndLogLine ;
           for( int x=32;x<rlen;x+=32)
              {
             int * DataAsInt = (int *)(AsanPutDataBuffer+x) ;
               BegLogLine( PKFXLOG_ASAN_GET_PUT_DATA )
                 << "AsanPutDataBuffer[0x" << (void *) x
                 << "] 0x" << (void *)(DataAsInt[0])
                           << (void *)(DataAsInt[1])
                       << (void *)(DataAsInt[2])
                       << (void *)(DataAsInt[3])
                       << (void *)(DataAsInt[4])
                       << (void *)(DataAsInt[5])
                       << (void *)(DataAsInt[6])
                       << (void *)(DataAsInt[7])
               << EndLogLine ;
              }
          }
          StrongAssertLogLine( rlen == len )
      << "ERROR: rlen: " << rlen
      << " len: " << len
      << EndLogLine;
          
          if( inode < DATA_INODE_LIMIT )
            {
#if defined(ASAN_SINGLE_DISK_IMAGE)
        if ( inode != 0 || (0 == NSDInitialised && 0 == PkNodeGetId()))
      {
          if ( 0 == inode ) NSDInitialised = 1 ;
        int IndexInWriteRequest = 0;
        
        for( int i=0; i < NumberOfFullPackets; i++ )
          {
            long long Index = offset + IndexInWriteRequest;
            
            int* DataInt = (int *) (AsanPutDataBuffer + IndexInWriteRequest);
            
            BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "Putting asan metadata for "
        << " inode: " << inode
        << " Index: " << Index
        << " BytesInFullPutPacket: " << BytesInFullPutPacket
        << " IndexInWriteRequest: " << IndexInWriteRequest
        << " Source-address 0x" << (void *) (AsanPutDataBuffer + IndexInWriteRequest)
        << " DataInt[ "<<DEBUG_INDEX<<" ]: 0x" << (void *)(DataInt[ DEBUG_INDEX ])
        << EndLogLine;
            
            PutAsanData( inode,
             Index,
             AsanPutDataBuffer + IndexInWriteRequest , 
             BytesInFullPutPacket,
             BytesOfDataInLastPacket ? 0 : (i==NumberOfFullPackets-1) );
            
            IndexInWriteRequest += BytesInFullPutPacket;
          }
        
        if( BytesOfDataInLastPacket )
          {             
            long long Index = offset + IndexInWriteRequest;
            
            int* DataInt = (int *) (AsanPutDataBuffer + IndexInWriteRequest);
            BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "Putting asan metadata for "
        << " inode: " << inode
        << " Index: " << Index
        << " BytesOfDataInLastPacket: " << BytesOfDataInLastPacket
        << " IndexInWriteRequest: " << IndexInWriteRequest
        << " Source-address 0x" << (void *) (AsanPutDataBuffer + IndexInWriteRequest)
        << " DataInt[ "<<DEBUG_INDEX<<" ]: 0x" << (void *)(DataInt[ DEBUG_INDEX ])
        << EndLogLine;
            
            PutAsanData( inode,
             Index,
             AsanPutDataBuffer + IndexInWriteRequest , 
             BytesOfDataInLastPacket,
             1 );
            
            IndexInWriteRequest += BytesOfDataInLastPacket;             
          }
        
                 } 
         else 
                 {
       BegLogLine( PKFXLOG_ASAN_GET_PUT )
         << "Skipping put of asan metadata for "
         << " inode: " << inode
         << EndLogLine;
                 }
#else           
              // This is a write request for gpfs disk metadata
              // Check to see if this offset is in the map.
              int BlockIndex          = offset / GPFS_METADATA_BLOCK_SIZE;
              int OffsetOfDataInBlock = offset % GPFS_METADATA_BLOCK_SIZE;
              
              char* StartOfMetadataBlock = GetGpfsMetadata( BlockIndex );
              if( StartOfMetadataBlock == NULL )
          StartOfMetadataBlock = ReserveForGpfsMetadata( BlockIndex );
              
              char* RequestedDataPtr = & StartOfMetadataBlock[ OffsetOfDataInBlock ];
              
              // This is the distance RequestedDataPtr to the end of the block;		      
              int DataLeftInBlock = GPFS_METADATA_BLOCK_SIZE - OffsetOfDataInBlock;
    
              // Make sure that the requested length fits in a block
              StrongAssertLogLine( len <= DataLeftInBlock )
          << "ERROR:: GPFS metadata request spans blocks "
          << " DataLeftInBlock: " << DataLeftInBlock
          << " OffsetOfDataInBlock: " << OffsetOfDataInBlock
          << " BlockIndex: " << BlockIndex
          << " GPFS_METADATA_BLOCK_SIZE: " << GPFS_METADATA_BLOCK_SIZE
          << " len: " << len
          << EndLogLine;
    
              // Make sure that the requested length fits in a block
              BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "GPFS Metadata write: "
          << " DataLeftInBlock: " << DataLeftInBlock
          << " OffsetOfDataInBlock: " << OffsetOfDataInBlock
          << " BlockIndex: " << BlockIndex
          << " GPFS_METADATA_BLOCK_SIZE: " << GPFS_METADATA_BLOCK_SIZE
          << " len: " << len
          << EndLogLine;
    
    
#endif
          break;
// 		    }
     
// 		  StrongAssertLogLine( len <= ASAN_MEMORY_PUT_DATA_BUFFER_SIZE )
// 		    << "ERROR:: Local buffer is smaller then requested read"
// 		    << EndLogLine;

// 		  unsigned int rlen = readAll( AsanPutDataBuffer, 
// 					       len );

// 		  StrongAssertLogLine( rlen == len )
// 		    << "ERROR: rlen: " << rlen
// 		    << " len: " << len
// 		    << EndLogLine;

// 		  int IndexInWriteRequest = 0;

// 		  for( int i=0; i < NumberOfFullPackets; i++ )
// 		    {
// 		      long long IndexInFile = offsetInFile + IndexInWriteRequest;
          
// 		      PutAsanData( inode,
// 				   IndexInFile,
// 				   & AsanPutDataBuffer[ IndexInWriteRequest ], 
// 				   BytesInFullPutPacket );

// 		      int* DataInt = (int *) & AsanPutDataBuffer[ IndexInWriteRequest ];
          
// 		      BegLogLine( DEBUG_LOG )
// 			<< "Just put asan data for "
// 			<< " inode: " << inode
// 			<< " IndexInFile: " << IndexInFile
// 			<< " BytesInFullPutPacket: " << BytesInFullPutPacket
// 			<< " IndexInWriteRequest: " << IndexInWriteRequest
// 			<< " DataInt[ "<<DEBUG_INDEX<<" ]: " << DataInt[ DEBUG_INDEX ]
// 			<< EndLogLine;
          
// 		      IndexInWriteRequest += BytesInFullPutPacket;
// 		    }
      
// 		  if( BytesOfDataInLastPacket )
// 		    {		      
// 		      long long IndexInFile = offsetInFile + IndexInWriteRequest;

// 		      PutAsanData( inode,
// 				   IndexInFile,
// 				   & AsanPutDataBuffer[ IndexInWriteRequest ], 
// 				   BytesOfDataInLastPacket );
          
// 		      BegLogLine( DEBUG_LOG )
// 			<< "Just put asan data for "
// 			<< " inode: " << inode
// 			<< " IndexInFile: " << IndexInFile
// 			<< " BytesOfDataInLastPacket: " << BytesOfDataInLastPacket
// 			<< " IndexInWriteRequest: " << IndexInWriteRequest
// 			<< EndLogLine;

// 		      IndexInWriteRequest += BytesOfDataInLastPacket;		      
// 		    }

// 		  StrongAssertLogLine( IndexInWriteRequest == len )
// 		    << "IndexInWriteRequest: " << IndexInWriteRequest
// 		    << " len: " << len
// 		    << EndLogLine;

// 		  GPFS_WriteFinis.HitOE( PKTRACE_GPFS_WRITE,
// 					 "GPFS_WriteFinis",
// 					 Platform::Thread::GetId(),
// 					 GPFS_WriteFinis );

// 		  // NOTE:: THIS IS A STOP GAP, ONLY NEEDED UNTIL PK CAN DO SELF SENDS 
// 		  // DUAL CORE
// 		  rts_dcache_evict_normal();
      
// 		  BegLogLine( DEBUG_LOG )
// 		    << "TEMPORARY FIX: rts_dcache_evict_normal is called on every new block"
// 		    << EndLogLine;
      
// 		  break;
// 		}    		    
               }
     else 
       {
          
    
          int IndexInWriteRequest = 0;
    
          for( int i=0; i < NumberOfFullPackets; i++ )
            {
              long long IndexInFile = offsetInFile + IndexInWriteRequest;
              
              int* DataInt = (int *) ( AsanPutDataBuffer + IndexInWriteRequest );
              
              BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "Putting asan data for "
          << " inode: " << inode
          << " IndexInFile: " << IndexInFile
          << " BytesInFullPutPacket: " << BytesInFullPutPacket
          << " IndexInWriteRequest: " << IndexInWriteRequest
      << " Source-address 0x" << (void *) (AsanPutDataBuffer + IndexInWriteRequest)
          << " DataInt[ "<<DEBUG_INDEX<<" ]: 0x" << (void *)(DataInt[ DEBUG_INDEX ])
          << EndLogLine;
              
          PutAsanData( inode,
           IndexInFile,
           AsanPutDataBuffer + IndexInWriteRequest , 
           BytesInFullPutPacket,
           BytesOfDataInLastPacket ? 0 : (i==NumberOfFullPackets-1) );
          
              IndexInWriteRequest += BytesInFullPutPacket;
            }
          
          if( BytesOfDataInLastPacket )
            {		      
              long long IndexInFile = offsetInFile + IndexInWriteRequest;
    
          int* DataInt = (int *) ( AsanPutDataBuffer + IndexInWriteRequest );

              BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "Putting asan data for "
          << " inode: " << inode
          << " IndexInFile: " << IndexInFile
          << " BytesOfDataInLastPacket: " << BytesOfDataInLastPacket
          << " IndexInWriteRequest: " << IndexInWriteRequest
      << " Source-address 0x" << (void *) (AsanPutDataBuffer + IndexInWriteRequest)
          << EndLogLine;
    
          PutAsanData( inode,
           IndexInFile,
           & AsanPutDataBuffer[ IndexInWriteRequest ], 
           BytesOfDataInLastPacket, 
           1 );
          
              IndexInWriteRequest += BytesOfDataInLastPacket;
            }
                
      StrongAssertLogLine( IndexInWriteRequest == len )
        << "IndexInWriteRequest: " << IndexInWriteRequest
        << " len: " << len
        << EndLogLine;
      
    
            }
          GPFS_WriteFinis.HitOE( PKTRACE_GPFS_WRITE,
               "GPFS_WriteFinis",
               Platform::Thread::GetId(),
               GPFS_WriteFinis );
    
//     		  // NOTE:: THIS IS A STOP GAP, ONLY NEEDED UNTIL PK CAN DO SELF SENDS 
//     		  // DUAL CORE
//     		  rts_dcache_evict_normal();
          
//     		  BegLogLine( PKFXLOG_ASAN_GET_PUT )
//     		    << "TEMPORARY FIX: rts_dcache_evict_normal is called on every new block"
//     		    << EndLogLine;
          
          break;
    } 
        default:
    {
      StrongAssertLogLine( 1 )
        << "ASAN_MemoryServer:: ERROR:: unrecognized operation: "
        << hdr->Op
        << EndLogLine;

      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "ERROR:: Unrecognized operation: " << hdr->Op
        << " in disconnected state"
        << EndLogLine;
    }
        }
      break;
    }
  case MEMORY_SERVER_STATE_DISCONNECTED:
    {
      BegLogLine( PKFXLOG_ASAN_GET_PUT )
        << "ASAN_MemoryServer:: Entering : MEMORY_SERVER_STATE_DISCONNECTED"
        << EndLogLine;
      
      Gpfs2CN_Header hdr;
      
      unsigned int rlen = readAll( (char *) &hdr, sizeof( Gpfs2CN_Header ) );
      StrongAssertLogLine( rlen == sizeof( Gpfs2CN_Header ) )
        << "rlen: " << rlen
        << " sizeof( Gpfs2CN_Header ): " << sizeof( Gpfs2CN_Header )
        << EndLogLine;

      BegLogLine( PKFXLOG_ASAN_GET_PUT ) 
        << "ASAN_MemoryServer:: state::disconnected:: op: " <<  hdr.Op
        << EndLogLine;
      
      switch( hdr.Op )
        {
        case GPFS2CN_OPERATION_CONNECT:
    {	      
      // SendConnectionAck();
      CurrentState = MEMORY_SERVER_STATE_CONNECTED;
        BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "ASAN_MemoryServer:: Entering : MEMORY_SERVER_STATE_CONNECTED"
          << EndLogLine;
      break;
    }
        case GPFS2CN_OPERATION_DISCONNECT:
    {		
      // Result of multiple gpfs threads opening/closing disks
      // SendDisconnectionAck();
      CurrentState = MEMORY_SERVER_STATE_DISCONNECTED;
        BegLogLine( PKFXLOG_ASAN_GET_PUT )
          << "ASAN_MemoryServer:: Entering : MEMORY_SERVER_STATE_DISCONNECTED"
          << EndLogLine;
      break;
    }
        default:
    {		  
      
      StrongAssertLogLine( 1 )
        << "ERROR:: Unrecognized operation: " << hdr.Op
        << " in disconnected state"
        << EndLogLine;

      BegLogLine( 1 )
        << "ERROR:: Unrecognized operation: " << hdr.Op
        << " in disconnected state"
        << EndLogLine;
      
      break;
    }	    
        }
      
      break;
    }
  default:
    {
      StrongAssertLogLine( 0 )
        << "ERROR: Unrecognized state encountered: " << CurrentState
        << EndLogLine;
    }
  }
    }

  PkCo_Barrier();  
}

// Code graveyard ?
//#if 0
//        for( int i = 0; i < NumberOfFullPackets; i++ )
//          {
//            ComputeNodePutPacket* cnPutPacketPtr = (ComputeNodePutPacket* ) SendTreePayload;
//            
//            cnPutPacketPtr->BufferStart =  BufferBase + IndexInReadRequest;
//            cnPutPacketPtr->ResponseReceivedPtr = ResponseReceivedPtr;
//            
//            if( (i == NumberOfFullPackets-1) && (BytesOfDataInLastPacket == 0) )
//          cnPutPacketPtr->IsLastPacket = 1;
//            else
//          cnPutPacketPtr->IsLastPacket = 0;
//
//            cnPutPacketPtr->Length = BytesOfDataInFullPacket;
//            
//            long long IndexInFile = offsetInFile + IndexInReadRequest;
//
//            // Assures one 'get' packet response for every request
//
//            char* AsanDataPtr = GetAsanDataPtr( inode,
//                            IndexInFile,
//                            cnPutPacketPtr->Length,
//                            PacketRecvBuffer );
//    
//            memcpy( cnPutPacketPtr->DataPtr,
//                AsanDataPtr,
//                cnPutPacketPtr->Length );
//
//            IndexInReadRequest += cnPutPacketPtr->Length;
//
//            StrongAssertLogLine( IndexInReadRequest <= len )
//          << "IndexInReadRequest: " << IndexInReadRequest
//          << " len: " << len
//          << EndLogLine;
//
//            writePacket( &TreeHdr, SendTreePayload );
//            
//            BegLogLine( 0 )
//          << "Wrote packet:  "
//          << " BufferStart: "  << (void *) cnPutPacketPtr->BufferStart
//          << " Length: "       << cnPutPacketPtr->Length
//          << " IsLastPacket: " << cnPutPacketPtr->IsLastPacket
//          << " ResponseReceivedPtr: " << (void *) cnPutPacketPtr->ResponseReceivedPtr
//          << EndLogLine;
//
//          }
//        
//        if( BytesOfDataInLastPacket )
//          {
//            ComputeNodePutPacket* cnPutPacketPtr = (ComputeNodePutPacket* ) SendTreePayload;
//            
//            cnPutPacketPtr->BufferStart =  BufferBase + IndexInReadRequest;
//            cnPutPacketPtr->ResponseReceivedPtr = ResponseReceivedPtr;              
//            cnPutPacketPtr->IsLastPacket = 1;
//            
//            // + 0 is to account for the char DataPtr[0] in the sizeof( ComputeNodePutPacket )
//            cnPutPacketPtr->Length = BytesOfDataInLastPacket;
//            
//            long long IndexInFile = offsetInFile + IndexInReadRequest;
//
//            char* AsanDataPtr = GetAsanDataPtr( inode, 
//                             IndexInFile, 
//                             cnPutPacketPtr->Length,
//                             PacketRecvBuffer );
//
//            memcpy( cnPutPacketPtr->DataPtr, 
//                AsanDataPtr,
//                cnPutPacketPtr->Length );
//            
//            IndexInReadRequest += cnPutPacketPtr->Length;
//
//            writePacket( &TreeHdr, SendTreePayload );
//
//            BegLogLine( 0 )
//          << "Wrote packet:  "
//          << " BufferStart: "  << (void *) cnPutPacketPtr->BufferStart
//          << " Length: "       << cnPutPacketPtr->Length
//          << " IsLastPacket: " << cnPutPacketPtr->IsLastPacket
//          << " ResponseReceivedPtr: " << (void *) cnPutPacketPtr->ResponseReceivedPtr
//          << EndLogLine;
//          }
//        
//#endif
