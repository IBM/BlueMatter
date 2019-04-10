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
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <rts.h>
#include <BonB/blade_on_blrts.h>

#include <BonB/BGLTorusAppSupport.c>

#include <spi/tree_reduce.hpp>

#define BGL_TREE_PAYLOAD_SIZE ( sizeof( _BGL_TreeCollectivePktPyld ) )

// Tree reduce up to a payload full on the tree
// 0 <= aSize <= sizeof( _BGL_TreeCollectivePktPyld )
static void ReduceOnTree( _BGL_TreeHwHdr* aHdr, unsigned long * aReduceIn, unsigned long * aReduceOut, int aSize )
{
    _BGL_TreeCollectivePktPyld Payload;
    _BGL_TreeHwHdr TreeHdrDummy;    
    _BGL_TreePayload PayloadDummy;

    assert( aSize >= 0 && aSize <= BGL_TREE_PAYLOAD_SIZE );
    
    memset( &PayloadDummy, 0, aSize );
    memset( &Payload, 0, aSize );
    
    memcpy( &Payload, aReduceIn, aSize );
    
    BGLTreeRawSendPacket(1, aHdr, &Payload );
    BGLTreeReceive( 1, &TreeHdrDummy, &PayloadDummy, BGL_TREE_PAYLOAD_SIZE );
    memcpy( aReduceOut, &PayloadDummy, aSize );
}

static void ReduceOnTree64( _BGL_TreeHwHdr* aHdr, long long * aReduceIn, long long * aReduceOut, int aSize )
{
    _BGL_TreeCollectivePktPyld Payload;
    _BGL_TreeHwHdr TreeHdrDummy;    
    _BGL_TreePayload PayloadDummy;

    assert( aSize >= 0 && aSize <= BGL_TREE_PAYLOAD_SIZE );
    
    memset( &PayloadDummy, 0, aSize );
    memset( &Payload, 0, aSize );
    
    memcpy( &Payload, aReduceIn, aSize );
    
    BGLTreeRawSendPacket(1, aHdr, &Payload );
    BGLTreeReceive( 1, &TreeHdrDummy, &PayloadDummy, BGL_TREE_PAYLOAD_SIZE );
    memcpy( aReduceOut, &PayloadDummy, aSize );
}

void GlobalTreeReduce( unsigned long * aReduceIn, unsigned long * aReduceOut, int aSize, unsigned aOperation )
  {
  if( !hard_processor_id() )
    {    
    _BGL_TreeHwHdr TreeHdr;
    /// NOTE NOTE NOTE!!! Route 1, ONE!!! Is the only reduction route
    /// NOTE NOTE NOTE!!! Route 1, ONE!!! Is the only reduction route

    /// Saw 3us for a reduction on 1 packet on 4/6/05
    /// Much jitter in one packet timing if I/O is done prior to tree send/recv
    
    BGLTreeMakeCollectiveHdr( &TreeHdr,
                              1,
                              0,
                              aOperation,
                              _BGL_TREE_OPSIZE_BIT32,
                              999,
                              _BGL_TREE_CSUM_NONE );

    int NumberOfFullPackets = aSize / BGL_TREE_PAYLOAD_SIZE;
    int BytesInLastPacket   = aSize % BGL_TREE_PAYLOAD_SIZE;

    for( int i=0; i<NumberOfFullPackets; i++ )
      {
      int index = ( i*BGL_TREE_PAYLOAD_SIZE ) >> 2;
      assert( index>=0 && index< aSize );
      ReduceOnTree( &TreeHdr, &aReduceIn[ index ], &aReduceOut[ index ], BGL_TREE_PAYLOAD_SIZE );      
      }
    
    if( BytesInLastPacket )
      {
      int index = ( NumberOfFullPackets * BGL_TREE_PAYLOAD_SIZE ) >> 2;
      assert( index >= 0 && index < aSize );      
      ReduceOnTree( &TreeHdr, &aReduceIn[ index ], &aReduceOut[ index ], BytesInLastPacket );
      }
    }
  }
  
void GlobalTreeReduce64( long long * aReduceIn, long long * aReduceOut, int aSize, unsigned aOperation )
  {
  if( !hard_processor_id() )
    {    
    _BGL_TreeHwHdr TreeHdr;
    /// NOTE NOTE NOTE!!! Route 1, ONE!!! Is the only reduction route
    /// NOTE NOTE NOTE!!! Route 1, ONE!!! Is the only reduction route

    /// Saw 3us for a reduction on 1 packet on 4/6/05
    /// Much jitter in one packet timing if I/O is done prior to tree send/recv
    
    BGLTreeMakeCollectiveHdr( &TreeHdr,
                              1,
                              0,
                              aOperation,
                              _BGL_TREE_OPSIZE_BIT64,
                              999,
                              _BGL_TREE_CSUM_NONE );

    int NumberOfFullPackets = aSize / BGL_TREE_PAYLOAD_SIZE;
    int BytesInLastPacket   = aSize % BGL_TREE_PAYLOAD_SIZE;

    for( int i=0; i<NumberOfFullPackets; i++ )
      {
      int index = ( i*BGL_TREE_PAYLOAD_SIZE ) >> 3;
      assert( index>=0 && index< aSize );
      ReduceOnTree64( &TreeHdr, &aReduceIn[ index ], &aReduceOut[ index ], BGL_TREE_PAYLOAD_SIZE );      
      }
    
    if( BytesInLastPacket )
      {
      int index = ( NumberOfFullPackets * BGL_TREE_PAYLOAD_SIZE ) >> 3;
      assert( index >= 0 && index < aSize );      
      ReduceOnTree64( &TreeHdr, &aReduceIn[ index ], &aReduceOut[ index ], BytesInLastPacket );
      }
    }
  }
  

#if 0
int
main( int argc, char **argv, char **envp )
  {
  int rc;
  
  if( (rc = _blade_on_blrts_init()) )
    {
    printf("main: blade_on_blrts_init: rc = %d.\n", rc );
    exit( rc );
    }
  
  int MyRank = BGLPartitionGetRank();
  unsigned long InOR  = 0x01 << (MyRank%31);
  unsigned long OutOR = 0;
  
  GlobalTreeReduce( &InOR, &OutOR, sizeof( unsigned long), _BGL_TREE_OPCODE_OR );  
  printf("InOR: %08x OutOR: %08x \n", InOR, OutOR );

  unsigned long InAdd = MyRank;
  unsigned long OutAdd = 0;

  GlobalTreeReduce( &InAdd, &OutAdd, sizeof( unsigned long), _BGL_TREE_OPCODE_ADD );
  printf("InAdd: %d OutAdd: %d \n", InAdd, OutAdd );

  unsigned long InMax = MyRank;
  unsigned long OutMax = 0;

  GlobalTreeReduce( &InMax, &OutMax, sizeof( unsigned long), _BGL_TREE_OPCODE_MAX );
  printf("InMax: %d OutMax: %d \n", InMax, OutMax );

  int arraysize = 1024;
  unsigned long* arrayIn = (unsigned long *) malloc( sizeof( unsigned long ) * arraysize );
  assert( arrayIn );

  unsigned long* arrayOut = (unsigned long *) malloc( sizeof( unsigned long ) * arraysize );
  assert( arrayOut );

  for( int i=0; i<arraysize; i++ )
    {
    arrayIn[ i ] = i;
    arrayOut[ i ] = 0;    
    }

  GlobalTreeReduce( arrayIn, arrayOut, arraysize*sizeof(unsigned long), _BGL_TREE_OPCODE_ADD );
  
  int PartitionSize = BGLPartitionGetSize();

  for( int i=0; i<arraysize; i++ )
    {
    if( arrayOut[ i ] != i*PartitionSize )
      {
      printf("arrayOut[ %d ]: %d i*PartitionSize=%d\n",
             i, arrayOut[i], i*PartitionSize );
      assert( 0 );
      }
    }
  printf("Reduce test passed!\n");

  _blLockBoxBarrier( 64 );
  }
#endif
