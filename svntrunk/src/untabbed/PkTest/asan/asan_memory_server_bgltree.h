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
 #ifndef __ASAN_MEMORY_SERVER__
#define __ASAN_MEMORY_SERVER__

#ifndef DEBUG_ASAN_SORT
#define DEBUG_ASAN_SORT ( 0 )
#endif

#ifndef PKFXLOG_DEBUG_ASAN_SORT
#define PKFXLOG_DEBUG_ASAN_SORT ( 0 )
#endif

#include <execinfo.h>

#include <BonB/BGL_PartitionSPI.h>
#include <rts.h>
#include <Pk/API.hpp>
#include <Pk/Compatibility.hpp>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>

#include <gpfs2cn_protocol.h>

#include <map>
#include <list>
#include <vector>
#include <inmemdb/BGL_Alloc.hpp>
#include <inmemdb/hashfunc.hpp>

#ifndef PKFXLOG_BGLTREE_DEBUG
#define PKFXLOG_BGLTREE_DEBUG ( 0 )
#endif

#define DEBUG_LOG ( 0 )

#define SOCKET_ERROR (-1)
// States
enum 
{
  MEMORY_SERVER_STATE_START,
  MEMORY_SERVER_STATE_CONNECTED,
  MEMORY_SERVER_STATE_DISCONNECTED
};

static void error_exit(char *str) 
{ 
  fprintf(stderr,"\n%s\n",str); 
  fprintf(stderr,"errno %d\n ", errno); 
  fflush(stderr); 
  exit(-1); 
}

//#define BGL_TREE_PAYLOAD_SIZE ( _BGL_TREE_PKT_MAX_BYTES - sizeof( _BGL_TreeHwHdr ) )

_BGL_TreeHwHdr DummyHdr ALIGN_QUADWORD;
char RecvTreePayload[ _BGL_TREE_PKT_MAX_BYTES ] ALIGN_QUADWORD;
char SendTreePayload[ _BGL_TREE_PKT_MAX_BYTES ] ALIGN_QUADWORD;

static void writePacket( _BGL_TreeHwHdr* hdr, char* Buf )
{
  BGLTreeSend( 1, hdr, Buf, _BGL_TREE_PKT_MAX_BYTES );
}

static int IsTreeFifoEmpty( int vc )
  {
    _BGL_TreeFifoStatus stat;

    BGLTreeGetStatus( vc, &stat );
    
    return (stat.RecHdrCount == 0) || (stat.RecPyldCount < 0x10);
  }

/* Obtain a backtrace and print it to stdout. */
static void print_trace (void)
{
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);

  BegLogLine( 1 )
    << "print_trace:: " 
    << " Obtained " << size << " stack frames. "
    << EndLogLine;

/*   char* Str = "Hello World!"; */
/*   write( 1, Str, strlen( Str ) ); */

  for (i = 0; i < size; i++)
    {
/*     BegLogLine( 1 ) */
/*       << " print_trace:: " */
/*       << " traceback_symbol[ " << i << " ]: " << strings[ i ]  */
/*       << " " << array[ i ]  */
/*       << EndLogLine;  */
      printf( "print_trace:: traceback_symbol[ %d ]: %s %08x\n", 
        i, strings[ i ], array[ i ] );
      
      fflush( stdout );
    }

  free (strings);
}

static void readPacket( char* Buf )
{
  BegLogLine( PKFXLOG_BGLTREE_DEBUG )
    << "readPacket:: Entering... "
    << EndLogLine;

  // BGLTreeReceive( 1, &DummyHdr, Buf, _BGL_TREE_PKT_MAX_BYTES );
  BGLTreeReceiveWithYield( 1, &DummyHdr, Buf, _BGL_TREE_PKT_MAX_BYTES, PkFiberYield );

  BegLogLine( PKFXLOG_BGLTREE_DEBUG )
    << "readPacket:: Leaving... "
    << EndLogLine;  
}

static unsigned int readAll( char* Buf, unsigned int len )
{
/*   while( IsTreeFifoEmpty( 1 ) ) */
/*     PkFiberYield(); */
  
  // int rc = BGLTreeReceiveUnaligned( 1, &DummyHdr, Buf, len );
  BegLogLine( PKFXLOG_BGLTREE_DEBUG )
    << "readAll:: Entering... "
    << EndLogLine;

  int rc = BGLTreeReceiveUnalignedWithYield( 1, &DummyHdr, Buf, len, PkFiberYield );

  BegLogLine( PKFXLOG_BGLTREE_DEBUG )
    << "readAll:: Leaving... "
    << EndLogLine;
  
  return rc;
}

static unsigned int writeAll( _BGL_TreeHwHdr* hdr, char* Buf, unsigned int len )
{
  return BGLTreeSendUnaligned( 1, hdr, Buf, len );
}

class ASAN_BufferManager
{
 public:
  static char * Buffer;
  static int    BufferSize;
  static int    BufferIndex;
  
  static void AllocateBuffer( int MBCount );
  static char* GetBufferPtr();
  static int GetBufferSize();  
};

#define STL_VECTOR( Key ) std::vector< Key, inmemdb::BGL_Alloc< Key > >
#define STL_LIST( Key ) std::list< Key, inmemdb::BGL_Alloc< Key > >
#define STL_MAP( Key, Data ) std::map< Key, Data, less< Key >, inmemdb::BGL_Alloc< std::pair< const Key, Data > > >

#define ASAN_MEMORY_BLOCK_SIZE ( 128 * 1024 )

class BlockRep
  {
  public:
  unsigned long long mCheckSum;
  long long  mBlockIndex;
  char*      mBlockPtr;
  
  bool operator<(const BlockRep& aBlockRep ) const
    {
      return mBlockIndex < aBlockRep.mBlockIndex; 
    }

  unsigned long long checksum()
    {
      unsigned long long Result = 0;
      
      unsigned long long * BlockPtrLL = (unsigned long long *) mBlockPtr;
      
      int NumberOfLongLongs = ASAN_MEMORY_BLOCK_SIZE / sizeof( unsigned long long );

      for( int i=0; i < NumberOfLongLongs; i++ )
  {
    Result += BlockPtrLL[ i ];
  }
      
      
      return Result;
    }

  void SetChecksum()
    {
      mCheckSum = checksum();
    }

  unsigned long long GetChecksum()
    {
      return mCheckSum;
    }

  // returns 0 on failure
  int DoChecksum()
    {
      return (mCheckSum == checksum());
    }
  
  };

class LocalBlocksForInodeManagerIF
  {
    int            mInode;

  // typedef STL_VECTOR( BlockRep ) BlockRepList_T;
  typedef STL_MAP( long long, BlockRep ) BlockRepList_T;
  BlockRepList_T*                        mBlockRepList;
  BlockRepList_T::iterator               mCurrentIter;
  

  public:    
    void Init( int aInode )
      {	
      mInode = aInode;	

      inmemdb::BGL_Alloc< BlockRepList_T > alloc;
  
      mBlockRepList = (BlockRepList_T *) alloc.allocate( 1 );
      alloc.construct( mBlockRepList, BlockRepList_T() );
      
      }

    void ChecksumBlocks()
      {
  BlockRepList_T::iterator cur = mBlockRepList->begin();
  BlockRepList_T::iterator end = mBlockRepList->end();
  for( ; cur != end; cur++ )
    {
      (*cur).second.SetChecksum();
    }
      }

    int DoChecksum( long long blockIndex )
      {
  BlockRepList_T::iterator Cur = mBlockRepList->find( blockIndex );
  
  if( Cur != mBlockRepList->end() )
    {
      return Cur->second.DoChecksum();
    }
  else
    StrongAssertLogLine( 0 )
      << "DoChecksum( blockIndex ):: ERROR:: "
      << " did not find entry in mBlockRepList for "
      << " blockIndex: " << blockIndex
      << EndLogLine;

  return 0;
      }
    
/*     void SortBlockList() */
/*       { */
/* 	mBlockRepList->sort(); */
/*       } */
    
    void AddBlock( long long BlockIndex, char* BlockPtr )
      {
  BlockRep BR;
  BR.mBlockIndex = BlockIndex;
  BR.mBlockPtr   = BlockPtr;

  // mBlockRepList->push_back( BR );
  int rc = mBlockRepList->insert( std::make_pair( BlockIndex, BR ) ).second;
  
  AssertLogLine( rc > 0 )
    << "AddBlock:: ERROR:: Failed on inserting  "
    << " BlockIndex: " << BlockIndex
    << " BlockPtr: " << (void *) BlockPtr
    << EndLogLine;

  BegLogLine( DEBUG_ASAN_SORT )
    << "AddBlock:: Just added:"
    << " BlockIndex: " << BlockIndex
    << " BlockPtr: " << (void *) BlockPtr
    << EndLogLine;	
      }
    
    // This call is 
    BlockRep* GetFirstBlock() 
      {
  mCurrentIter = mBlockRepList->begin();
  return (BlockRep *) & ((*mCurrentIter).second);
      }
    
    BlockRep* GetNextBlock()
      {
  mCurrentIter++;
  
  if( mCurrentIter == mBlockRepList->end() )
    return NULL;
  
  return (BlockRep *) & ((*mCurrentIter).second);
      }
  };

class ASAN_MemoryServer
{
  int    CurrentState;
  char * BufferPtr;

  _BGL_TreeHwHdr TreeHdr ALIGN_QUADWORD;
  unsigned       MyIONodeTreeAddress;

  in_addr_t GetIONodeIP();  
  // void SendDisconnectionAck();
  // void SendConnectionAck();
  void AcceptConnection();
  
  // This happens once for now
  in_addr_t GenerateDiskPortFile();
  void SetBufferPtr( char * buff );

public:
  void Init();
  ASAN_MemoryServer() {}

  void Run( int tracefreq ); 

  int GetLocalBlocksCount( int inode );
  
  LocalBlocksForInodeManagerIF * GetLocalBlocks( int inode );  

  void SendOnTree( char* Data, 
       int Len, 
       unsigned long* ResponseReceivedPtr,
       char*          BufferBase );

};

struct DataHashArgs
{
  long long blockIndex;
  long long inode;
};

struct LocalDataAccessKey
{
  int       inode;
  long long blockIndex;

  bool operator<(const LocalDataAccessKey& aKey ) const
  {
    // KeyCompare kc;
    if( inode < aKey.inode )
      return 1;
    else if( inode == aKey.inode )
      return ( blockIndex < aKey.blockIndex );
    else
      return 0;    
  }  
};

struct LocalDataAccessRecord
{
  // This is a flag that's set
  // When the end of the record is 
  // taken by the owner of the record
  char        IsFirstRecordTaken; 

  // Make this block rep
  BlockRep       mBlockRep;
};

typedef STL_MAP( LocalDataAccessKey, LocalDataAccessRecord ) LocalDataMap_T;

extern LocalDataMap_T LocalDataMap;

static int GetDataOwner(int inode, long long blockIndex )         
{

  DataHashArgs harg;
  harg.inode = inode;
  harg.blockIndex = blockIndex;

  if ( inode < 4096 )
  {
    return 0 ; // Force 'metadata' to node 0
  }
  unsigned int hash_val = hashword( (const unsigned int *) &harg, sizeof( DataHashArgs ) / sizeof ( unsigned int ),  0 );
  
  // It's best if the N is a power of 2
  unsigned int hash_mask = PkNodeGetCount() - 1;
  
  return ( hash_val & hash_mask ); 
}

char* GetLocalDataPtr( int inode,
           long long blockIndex,
           int indexInBlock );

void ChecksumBlock( int inode,
        long long blockIndex );

#endif
