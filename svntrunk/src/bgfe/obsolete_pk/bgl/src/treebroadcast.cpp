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
 /*
 * File: treebroadcast.cpp
 *
 * Purpose: Implement 'broadcast' on the BLADE tree SPI
 *
 * Author: Chris Ward (tjcw@us.ibm.com)
 *         (c) IBM, 2003
 *
 * History : 04/25/2003: TJCW - Created
 *
 * To do :
 *  Implement block scheduling; i.e. make the choice between 'send packet now'
 *  and 'receive packet now' explicit, so that CPU computation and tree-ALU
 *  traffic can be better overlapped.
 *  Quadword-align tree transmit and receive buffers.
 *  Implement 'short' blocks, to allow arbitrary-length broadcasts.
 */


#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }


#include <sys/blade_types.h>
#include <pk/treebroadcast.hpp>
#include <spi/BGL_TreePktSPI.h>
#include <varargs.h>
#include <spi/BGL_PartitionSPI.h>
//extern "C" {
//   int BGLPartitionGetRank( void ) ;
//   } ;
#include <assert.h>
using namespace std ;

#define DIAG_BROADCAST 0
extern "C" {
//    void dump_block(const char * text, const void * block_ptr) ;
//    void dump_block_partial(const char * text, const void * block_ptr, int Length) ;
   int s0printf(const char *, ... );
   }

enum {
   k_TreePacketSize = _BGL_TREE_PKT_MAX_BYTES ,
   k_TreeBlockSize = k_TreePacketSize / sizeof(double)
   } ;

extern int FullPartitionClass ;

static const char hexbyte[17] = "0123456789abcdef" ;

extern "C" {
void dump_block(const char * text, const void * block_ptr)
{
  char dump_buffer[33] ;
  char * cbp = ( char * ) block_ptr ;
  int q0 ;
  int q1 ;
  s0printf("dump_block %s %08x\n", text, (int)block_ptr) ;
  for (q0 = 0 ; q0 < 16 ; q0 += 1)
  {
    for (q1 = 0 ; q1 < 16 ; q1 += 1)
    {
      int cb = cbp[16*q0+q1] ;
      dump_buffer[q1*2  ] = hexbyte[(cb >> 4) & 0x0f] ;
      dump_buffer[q1*2+1] = hexbyte[(cb     ) & 0x0f] ;
    }
    dump_buffer[32] = 0;
    s0printf("%s\n",dump_buffer) ;
  }
}

void dump_block_partial(const char * text, const void * block_ptr, int Length)
{
  char * cbp = ( char * ) block_ptr ;
  int q0 ;
  int q1 ;
  s0printf("dump_block %s %08x", text, (int) block_ptr) ;
  for (q0 = 0 ; q0 < 16 ; q0 += 1)
  {
    s0printf("\n") ;
    for (q1 = 0 ; q1 < 16 ; q1 += 1)
    {
      int x = 16*q0+q1 ;
      if ( x < Length )
      {
        s0printf("%02x", cbp[16*q0+q1]) ;
      } else {
        s0printf("--") ;
      }
    }
  }
  s0printf("\n") ;
}
};
class TBroadcast
{
   public:
   void BroadcastFullBlock(double * Source) ;
   void ReceiveFullBlock(double * Target ) ;
} ;
static   double dummyBlock[k_TreeBlockSize] __attribute__ ((aligned(16)));

void TBroadcast::BroadcastFullBlock(double * BlockToBroadcast)
{
   _BGL_TreeHwHdr broadcastHdr ;
   _BGL_TreeHwHdr broadcastHdrReturned ;
   BGLTreeMakeBcastHdr( &broadcastHdr,
                      FullPartitionClass,             // class
                      0                               // software tag, unused
                    ) ;
#if DIAG_BROADCAST
   dump_block("Broadcasting block", BlockToBroadcast) ;
#endif
   int broadcastResult = BGLTreeSend(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // virtual channel
                         &broadcastHdr,                  // Header we made before
                         BlockToBroadcast,               // Operands being sent
                         k_TreePacketSize                // one block, by construction
                         ) ;
   int broadcastReturnedResult = BGLTreeReceive(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // class
                         &broadcastHdrReturned,          // Header after circulating the tree
                         dummyBlock,                     // Operands being received
                         k_TreePacketSize                // one block, by construction
                         ) ;
#if DIAG_BROADCAST
   dump_block("Received dummy block", dummyBlock) ;
#endif
}

void TBroadcast::ReceiveFullBlock(double * BlockToReceive)
{
   _BGL_TreeHwHdr broadcastHdrReturned ;
   int broadcastReturnedResult = BGLTreeReceive(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // class
                         &broadcastHdrReturned,          // Header after circulating the tree
                         BlockToReceive,                 // Operands being received
                         k_TreePacketSize                // one block, by construction
                         ) ;
#if DIAG_BROADCAST
   dump_block("Received block", BlockToReceive) ;
#endif
}

// Interface function, users don't need the internals ...
void TreeBroadcast::Broadcast (
   double * SourceTarget,
   unsigned int Length,
   unsigned int Root
   )
{
   TBroadcast Broadcaster ;

   unsigned int GroupCount = Length / k_TreeBlockSize ;
   assert(Length == GroupCount*k_TreeBlockSize) ; // Don't support partial blocks at the moment

   if (Root == BGLPartitionGetRank() )
   {
      // We are sending, need to dump packets being received
      for (int x=0; x<GroupCount; x+=1)
      {
         Broadcaster.BroadcastFullBlock(
           SourceTarget+x*k_TreeBlockSize
         ) ;
      } /* endfor */
   }
   else
   {
      // We are receiving
      for (int x=0; x<GroupCount; x+=1)
      {
         Broadcaster.ReceiveFullBlock(
           SourceTarget+x*k_TreeBlockSize
         ) ;
      } /* endfor */
   } /* endif */
}

// And a version callable from C
void tr_Broadcast (
   double * SourceTarget,
   unsigned int Length,
   unsigned int Root
)
{
   TreeBroadcast::Broadcast(SourceTarget,Length,Root) ;
}

class TBroadcastUnaligned
{
   public:
   void BroadcastFullBlock(char * Source) ;
   void ReceiveFullBlock(char * Target ) ;
   void BroadcastPartialBlock(char * Source, int Length) ;
   void ReceivePartialBlock(char * Source, int Length) ;
} ;

void TBroadcastUnaligned::BroadcastFullBlock(char * BlockToBroadcast)
{
   _BGL_TreeHwHdr broadcastHdr ;
   _BGL_TreeHwHdr broadcastHdrReturned ;
   BGLTreeMakeBcastHdr( &broadcastHdr,
                      FullPartitionClass,             // class
                      0                               // software tag, unused
                    ) ;
#if DIAG_BROADCAST
   dump_block("Broadcasting unaligned block", BlockToBroadcast) ;
#endif
   int broadcastResult = BGLTreeSendUnaligned(
                        _BGL_TREE_BALANCED_MODE_APP_VC, // virtual channel
                         &broadcastHdr,                  // Header we made before
                         BlockToBroadcast,               // Operands being sent
                         k_TreePacketSize                // one block
                         ) ;
   int broadcastReturnedResult = BGLTreeReceive(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // class
                         &broadcastHdrReturned,          // Header after circulating the tree
                         dummyBlock,                     // Operands being received
                         k_TreePacketSize                // one block
                         ) ;
#if DIAG_BROADCAST
   dump_block("Received dummy block", dummyBlock) ;
#endif
}

void TBroadcastUnaligned::BroadcastPartialBlock(char * BlockToBroadcast, int Length)
{
   _BGL_TreeHwHdr broadcastHdr ;
   _BGL_TreeHwHdr broadcastHdrReturned ;
   BGLTreeMakeBcastHdr( &broadcastHdr,
                      FullPartitionClass,             // class
                      0                               // software tag, unused
                    ) ;
#if DIAG_BROADCAST
   dump_block_partial("Broadcasting unaligned partial block", BlockToBroadcast, Length) ;
#endif
   int broadcastResult = BGLTreeSendUnaligned(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // virtual channel
                         &broadcastHdr,                  // Header we made before
                         BlockToBroadcast,               // Operands being sent
                         Length                          // Length to send
                         ) ;
   int broadcastReturnedResult = BGLTreeReceive(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // class
                         &broadcastHdrReturned,          // Header after circulating the tree
                         dummyBlock,                     // Operands being received
                         k_TreePacketSize                // one block
                         ) ;
#if DIAG_BROADCAST
   dump_block("Received dummy block", dummyBlock) ;
#endif
}

void TBroadcastUnaligned::ReceiveFullBlock(char * BlockToReceive)
{
   _BGL_TreeHwHdr broadcastHdrReturned ;
   int broadcastReturnedResult = BGLTreeReceiveUnaligned(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // class
                         &broadcastHdrReturned,          // Header after circulating the tree
                         BlockToReceive,                 // Operands being received
                         k_TreePacketSize                // one block, by construction
                         ) ;
#if DIAG_BROADCAST
   dump_block("Received block", BlockToReceive) ;
#endif
}

void TBroadcastUnaligned::ReceivePartialBlock(char * BlockToReceive, int Length)
{
   _BGL_TreeHwHdr broadcastHdrReturned ;
   int broadcastReturnedResult = BGLTreeReceiveUnaligned(
                         _BGL_TREE_BALANCED_MODE_APP_VC, // class
                         &broadcastHdrReturned,          // Header after circulating the tree
                         BlockToReceive,                 // Operands being received
                         Length                          // Length to receive
                         ) ;
#if DIAG_BROADCAST
   dump_block_partial("Received block", BlockToReceive,Length) ;
#endif
}

// Interface function, users don't need the internals ...
void TreeBroadcast::BroadcastUnaligned (
   char * SourceTarget,
   unsigned int Length,
   unsigned int Root
   )
{
   TBroadcastUnaligned Broadcaster ;

   unsigned int GroupCount = Length / k_TreePacketSize ;
   unsigned int Remainder = Length - k_TreePacketSize*GroupCount ;
   s0printf("TreeBroadcast::BroadcastUnaligned at line %d\n", __LINE__ );
   if (Root == BGLPartitionGetRank() )
   {
      // We are sending, need to dump packets being received
      for (int x=0; x<GroupCount; x+=1)
      {
         Broadcaster.BroadcastFullBlock(
           SourceTarget+x*k_TreePacketSize
         ) ;
      } /* endfor */
      if (Remainder > 0)
      {
         Broadcaster.BroadcastPartialBlock(
           SourceTarget+GroupCount*k_TreePacketSize,
           Remainder
         ) ;
      } /* endif */
   }
   else
   {
      // We are receiving
      for (int x=0; x<GroupCount; x+=1)
      {
         Broadcaster.ReceiveFullBlock(
           SourceTarget+x*k_TreePacketSize
         ) ;
         s0printf("TreeBroadcast::BroadcastUnaligned at line %d\n", __LINE__ );
      } /* endfor */
      if (Remainder > 0)
      {
         Broadcaster.ReceivePartialBlock(
           SourceTarget+GroupCount*k_TreePacketSize,
           Remainder
         ) ;
         s0printf("TreeBroadcast::BroadcastUnaligned at line %d\n", __LINE__ );
      } /* endif */
   } /* endif */
}

// And a version callable from C
void tr_BroadcastUnaligned (
   char * SourceTarget,
   unsigned int Length,
   unsigned int Root
)
{
   TreeBroadcast::BroadcastUnaligned(SourceTarget,Length,Root) ;
}

