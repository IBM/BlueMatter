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
 
#include <Pk/Heap.hpp>

/* -------------------------------------------------------------  */
/* Product(s):                                                    */
/* 5733-BG1                                                       */
/*                                                                */
/* (C)Copyright IBM Corp. 2004, 2004                              */
/* All rights reserved.                                           */
/* US Government Users Restricted Rights -                        */
/* Use, duplication or disclosure restricted                      */
/* by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                */
/* Licensed Materials-Property of IBM                             */
/* -------------------------------------------------------------  */
//
//      File: blade/lib/bl_malloc.c
//
//   Purpose: malloc/calloc/realloc/free support.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: Separate memory pools per core, but in Symmetric-Mode, each
//              core can access the other's allocated memory (implicitly shared).
//            Each allocation is L1-D Cache Aligned (32B) and is rounded up
//              to a multiple of L1-D line size to prevent false sharing.
//            An allocation must be freed by same core that allocated it.
//            Realloc to shrink is efficient, but growing is malloc+copy+free.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <bgllockbox.h>
#include <rts.h>
// #include <mpi.h>
#include <assert.h>

#include <BonB/blade_on_blrts.h>  // for alignment macros

#define KILO (1024)
#define K_DATA /**/
#if !defined(ALIGN_L3_CACHE)
#define ALIGN_L3_CACHE /**/
#endif
#define s0printf printf
#define s0flush fflush(stdout) 

// How much memory to grab from 'malloc' into the heap before we go dual-core
#if !defined(PK_HEAP_MB)
#define PK_HEAP_MB ( 508 )
#endif

static
inline int
cntlz( uint32_t v )
  {
  int i;
  uint32_t z;

  for ( i = 0, z = 0x80000000 ; i < 32 ; i++, z >>= 1 )
     if ( v & z )
        return(i);

  return(32);
  }


#if !defined(BL_MALLOC_DEBUG)
#define BL_MALLOC_DEBUG (0)
//#define BL_MALLOC_DEBUG ( 1 )
//#define BL_MALLOC_DEBUG (2)
//#define BL_MALLOC_DEBUG (3) // you'll be sorry
#endif

#define TRC_MSG_FCN printf

#if BL_MALLOC_DEBUG > 0
#define BLMDB1( Args )    do { TRC_MSG_FCN Args ; fflush(stdout); } while(0)
#define BLMDB1_Fcn( Fcn ) do { Fcn ; fflush(stdout); } while(0)
#else
#define BLMDB1( Args )    do { /* nothing */ } while(0)
#define BLMDB1_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

#if BL_MALLOC_DEBUG > 1
#define BLMDB2( Args )    do { TRC_MSG_FCN Args ; fflush(stdout); } while(0)
#define BLMDB2_Fcn( Fcn ) do { Fcn ; fflush(stdout); } while(0)
#else
#define BLMDB2( Args )    do { /* nothing */ } while(0)
#define BLMDB2_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

#if BL_MALLOC_DEBUG > 2
#define BLMDB3( Args )    do { TRC_MSG_FCN Args ; fflush(stdout); } while(0)
#define BLMDB3_Fcn( Fcn ) do { Fcn ; } while(0)
#else
#define BLMDB3( Args )    do { /* nothing */ } while(0)
#define BLMDB3_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

//#define BLM_CRASH(rc) Sim_Terminate( (rc) )
#define BLM_CRASH(rc) (printf("crash %s %d\n",__FILE__,__LINE__),fflush(stdout),abort(),exit(-1));


#define BL_ROUND_UP_CACHELINE(v) (((v) + 0x1F) & ~0x1F)

#define BL_ROUND_DOWN_CACHELINE(v) ((v) & ~0x1F)

#define BLMEMCHUNK_POOL_SIZE (1UL * KILO)

// This malloc manages cachelines (32 bytes), so
//  lower 5 bits of addr are available for flags
#define BLMEMCHUNK_ADDR_MASK (0xFFFFFFE0)
#define BLMEMCHUNK_ADDR_FREE (0x00000001)


    // MemChunk: track free/busy memory (@MG: rework for smaller size)
    //  MemChunks can be in 3 states:
    //   1) in the free_pool
    //   2) in the free_chunks list organized by log2 of size. sorted by size within the list
    //   3) busy_chunks, hashed by address for quick retrieval
    //  Allocations smaller than a chunk split the chunk into 2 chunks
    //  Frees of chunks attempt to coalesce chunks with free neighboring chunks
    //  So, all chunks have to be simultaneously free/busy and sorted by address

    struct BL_MemChunk
            {
            struct   BL_MemChunk     * next;
            struct   BL_MemChunk     * prev;
            struct   BL_MemChunk     * byaddr_next;
            struct   BL_MemChunk     * byaddr_prev;
            uint32_t                   addr;
            uint32_t                   size;
            struct   BL_MemChunkPool * my_pool;
#if defined(BL_MALLOC_VALIDATE)
            uint32_t                   checksum ;
#endif      
        void                       checksum_set(void) ;
        void                       checksum_check(void) const ;      
            };


    // Note: The first pool allocated can never be freed because
    //       2 of it's chunks are always used: one for free memory,
    //       and the second for itself.
    struct BL_MemChunkPool
            {
            struct BL_MemChunkPool * next;
            struct BL_MemChunkPool * prev;
            int                      used_count;
            struct BL_MemChunk     * last_chunk;   // emergency use to alloc new pool
            struct BL_MemChunk     * chunk_anchor;
#if defined(BL_MALLOC_VALIDATE)
            int                      checksum ;
#endif            
            struct BL_MemChunk       chunks[ BLMEMCHUNK_POOL_SIZE ];
        void                       checksum_set(void) ;
        void                       checksum_check(void) const ;      
            };

    struct BL_MemMgr
            {
            uint32_t                   mem_start;
            uint32_t                   mem_size;
            uint32_t                   mem_avail;
            uint32_t                   mem_busy;
            uint32_t                   busy_count;
            struct   BL_MemChunk     * byaddr_anchor;
            struct   BL_MemChunk     * free_chunks[  32 ]; // arranged by log2 of size
            struct   BL_MemChunk     * busy_chunks[ 256 ]; // open-hashed on address
            struct   BL_MemChunkPool * pool_anchor;
            struct   BL_MemChunkPool * free_pool_later_anchor; // de-recurse free
#if defined(BL_MALLOC_VALIDATE)
            uint32_t                   checksum ;
#endif            
        void                       checksum_set(void) ;
        void                       checksum_check(void) const ;      
            };
            
#if defined(BL_MALLOC_VALIDATE)
void BL_MemChunk::checksum_set  (void) 
{ 
  int sum = (int) next
          + (int) prev
          + (int) byaddr_next
          + (int) byaddr_prev
          + (int) addr
          + (int) size
          + (int) my_pool ;
  checksum = sum ; 
} 
void BL_MemChunk::checksum_check(void) const
{ 
  int sum = (int) next
          + (int) prev
          + (int) byaddr_next
          + (int) byaddr_prev
          + (int) addr
          + (int) size
          + (int) my_pool ;
   if ( sum != checksum )
   {
      assert(0) ; 
      *(int *)0 = 0 ; // Crash with a SIGSEGV
   }
} 
void BL_MemChunkPool::checksum_set  (void) 
{ 
  int sum = (int) next
          + (int) prev
          + (int) used_count
          + (int) last_chunk
          + (int) chunk_anchor ;
  checksum = sum ; 
          
} 
void BL_MemChunkPool::checksum_check(void)  const
{ 
  int sum = (int) next
          + (int) prev
          + (int) used_count
          + (int) last_chunk
          + (int) chunk_anchor;
   if ( sum != checksum )
   {
      assert(0) ; 
      *(int *)0 = 0 ; // Crash with a SIGSEGV
   }
} 
void BL_MemMgr::checksum_set  (void) 
{ 
    int sum_free_chunks = 0 ; 
    for ( int i=0 ;i<32;i+=1)
    {
      sum_free_chunks += (int) free_chunks[i] ;
    }
    int sum_busy_chunks = 0 ; 
    for ( int j=0 ;j<256;j+=1)
    {
      sum_busy_chunks += (int) busy_chunks[j] ;
    }
  int sum = (int) mem_start
          + (int) mem_size
          + (int) mem_avail
          + (int) mem_busy
          + (int) busy_count
          + (int) byaddr_anchor  
          + (int) pool_anchor 
          + (int) free_pool_later_anchor 
          + sum_free_chunks
          + sum_busy_chunks ;
   checksum = sum ;
}
void BL_MemMgr::checksum_check(void)  const
{ 
    int sum_free_chunks = 0 ; 
    for ( int i=0 ;i<32;i+=1)
    {
      sum_free_chunks += (int) free_chunks[i] ;
    }
    int sum_busy_chunks = 0 ; 
    for ( int j=0 ;j<256;j+=1)
    {
      sum_busy_chunks += (int) busy_chunks[j] ;
    }
  int sum = (int) mem_start
          + (int) mem_size
          + (int) mem_avail
          + (int) mem_busy
          + (int) busy_count
          + (int) byaddr_anchor 
          + (int) pool_anchor 
          + (int) free_pool_later_anchor 
          + sum_free_chunks
          + sum_busy_chunks ;
   if ( sum != checksum )
   {
      assert(0) ; 
      *(int *)0 = 0 ; // Crash with a SIGSEGV
   }
} 
#else
void BL_MemChunk::checksum_set  (void) { } 
void BL_MemChunk::checksum_check(void) const { } 
void BL_MemChunkPool::checksum_set  (void) { } 
void BL_MemChunkPool::checksum_check(void) const { } 
void BL_MemMgr::checksum_set  (void) { } 
void BL_MemMgr::checksum_check(void) const { } 
#endif

// Record the 'land usage' as fallow (unused), or growing crops (handed out by 'allocate', or containing metadata )
// but only if debugging.

// As this stands, it is a single-core checker; i.e. no support for the incoherent L1.
#if BL_MALLOC_DEBUG > 1
class LandUsageT
{
  public:
    enum {
      k_FieldSize = 32 ,
      k_WorldSize = 512*1024*1024 ,  // This is right for a 512MB node. Would want to change for other RAM sizes
      k_ByteSize = 8 ,
      k_WorldMapSize = k_WorldSize/(k_FieldSize*k_ByteSize)
    } ;
    static unsigned char GrowingMap[k_WorldMapSize] ;
    int SowField(uint32_t field)
    {
      int fieldbyte = ( field / k_ByteSize ) % k_WorldMapSize ;
      int fieldbit = field % k_ByteSize ;
      int result = 0 ; 
      if ( GrowingMap[fieldbyte] & ( 0x80 >> fieldbit ) )
      {
        BLMDB1(("(E) Field 0x%08x already in use\n", field)) ;
        result = 1 ;
      } 
      GrowingMap[fieldbyte] |= ( 0x80 >> fieldbit ) ;
      return result ; 
    }
  int Sow(uint32_t addr, uint32_t len)
  {
    int result = 0 ;
    BLMDB2(("(I) Seeding addr = 0x%08x , len = 0x%08x \n", addr, len ));
    for ( int field = addr/k_FieldSize; field<(addr+len)/k_FieldSize;field+=1)
    {
      result += SowField(field) ;
    }
    return result ; 
  }
    int ReapField(uint32_t field)
    {
      int fieldbyte = ( field / k_ByteSize ) % k_WorldMapSize ;
      int fieldbit = field % k_ByteSize ;
      int result =0 ;
      if ( 0 == (GrowingMap[fieldbyte] & ( 0x80 >> fieldbit ) ) )
      {
        BLMDB1(("(E) Field 0x%08x not in use\n", field)) ;
        result = 1 ;
      } 
      GrowingMap[fieldbyte] &= ~( 0x80 >> fieldbit ) ;
      return result ;
    }
  int Reap(uint32_t addr, uint32_t len)
  {
    int result = 0 ; 
    BLMDB2(("(I) Harvesting addr = 0x%08x , len = 0x%08x \n", addr, len ));		
    for ( int field = addr/k_FieldSize; field<(addr+len)/k_FieldSize;field+=1)
    {
      result += ReapField(field) ;
    }
    return result ;
  }
  
  int IsVacant(uint32_t addr)
  {
    int field = addr/k_FieldSize ;
      int fieldbyte = ( field / k_ByteSize ) % k_WorldMapSize ;
      int fieldbit = field % k_ByteSize ;
        return ( 0 == (GrowingMap[fieldbyte] & ( 0x80 >> fieldbit ) ) ) ;
  }
  int IsOccupied(uint32_t addr)
  {
    int field = addr/k_FieldSize ;
      int fieldbyte = ( field / k_ByteSize ) % k_WorldMapSize ;
      int fieldbit = field % k_ByteSize ;
        return ( 0 != (GrowingMap[fieldbyte] & ( 0x80 >> fieldbit ) ) ) ;
  }
  
} ;

unsigned char LandUsageT::GrowingMap[LandUsageT::k_WorldMapSize] ;
#else
class LandUsageT
{
  public: 
  int Sow(uint32_t addr, uint32_t len)
  {
    return 0 ;
  }
  int Reap(uint32_t addr, uint32_t len)
  {
    return 0 ;
  }
  int IsVacant(uint32_t addr)
  {
        return 1 ;
  }
  int IsOccupied(uint32_t addr)
  {
        return 1 ;
  }
  
} ;
#endif


class HeapManagerT: public LandUsageT
  {
  public:
  
    void checksum_check(int addr, int len) const ;
    void checksum_set(int addr, int len) ; 

    BL_MemMgr K_DATA mBL_MemMgr ALIGN_L3_CACHE ;
  
    inline
    void
    InitQByAddr( BL_MemChunk *chunk )
      {
      chunk->byaddr_next = NULL;
      chunk->byaddr_prev = NULL;
      chunk->checksum_set() ; 
      }

    inline
    void
    EnQByAddr( BL_MemChunk *n, BL_MemChunk *o )
      {
      BLMDB3(("EnQ: new->addr = 0x%08x, old->addr = 0x%08x\n", n->addr, o->addr ));

      n->checksum_check() ; 
      o->checksum_check() ; 
      if( n->addr < o->addr )
        {
        s0printf("(E) MemMgr Internal Error: _EnQByAddr: new < old\n");
        fflush(stdout) ; 
        BLM_CRASH( 3 );
        }

      if( (n->byaddr_next = o->byaddr_next) )
      {
         n->byaddr_next->checksum_check() ;
         n->byaddr_next->byaddr_prev = n;
         n->byaddr_next->checksum_set() ;
      }
      o->byaddr_next = n;
      o->checksum_set() ; 
      n->byaddr_prev = o;
      n->checksum_set() ; 
      }

    inline
    void
    DeQByAddr( BL_MemChunk *c )
      {
      c->checksum_check() ; 
      if( c->byaddr_prev )
      {
         c->byaddr_prev->checksum_check() ;
         c->byaddr_prev->byaddr_next = c->byaddr_next;
         c->byaddr_prev->checksum_set() ;
      }
      else {
         mBL_MemMgr.checksum_check() ;
         mBL_MemMgr.byaddr_anchor = c->byaddr_next;
         mBL_MemMgr.checksum_set() ;
      }

      if( c->byaddr_next )
      {
         c->byaddr_next->checksum_check() ;
         c->byaddr_next->byaddr_prev = c->byaddr_prev;
         c->byaddr_next->checksum_set() ;
      }

      c->byaddr_next = NULL;
      c->byaddr_prev = NULL;
      c->checksum_set() ; 
      }


    inline
    int
    BusyAddrHash( uint32_t v )
      {
      int a,b;

      // Large allocations given equal weight as small allocations
      a = ((v & 0x03FC0000) >> 18); // big ones
      b = ((v & 0x00007F80) >>  7); // small ones

      BLMDB3(("BusyAddrHash 0x%08x: a = %d, b = %d, a+b = %d\n", v, a, b, (a+b) ));

      return( (a + b) & 0xFF );
      }

    inline
    int
    GetBucket( uint32_t v )
      {
      int b = cntlz(v);

      BLMDB3(("cntlz(0x%08x) = %d\n", v, cntlz(v) ));

      if( b == 0 )
        return( 31 );
      else if ( b > 27 )
        return( 5 );
      else
        return( 32 - b );
      }

    #if BL_MALLOC_DEBUG > 1
    void
    ShowFreePool( )
      {
      int i, j;
      BL_MemChunkPool *pool = mBL_MemMgr.pool_anchor;
      BL_MemChunk *ch;

      if( !pool )
        {
        s0printf("(E) ShowFreePool: No pool!\n");
        fflush(stdout) ; 
        
        return;
        }

      for( j = 0 ; pool ; pool = pool->next , j++ )
        {
          s0printf("(I) Free Pool(%d):\n", j );
          fflush(stdout) ; 
  
          for ( i = 0, ch = pool->chunk_anchor ; ch ; ch = ch->next, i++ )
          {
             s0printf("  %2d.%4d: 0x%08x\n", j, i, (uint32_t)ch );
             fflush(stdout) ; 
          }
        }
      }

    void
    ShowFreeChunks( )
      {
      int i, cnt;
      BL_MemChunk *ch;

      for ( i = 0 ; i < 32 ; i++ )
         {
         cnt = 0;
         s0printf("Free Chunks[%2d]:", i );
         fflush(stdout) ; 
         for ( ch = mBL_MemMgr.free_chunks[i] ; ch ; ch = ch->next )
            {
            cnt++;
            s0printf(" 0x%08x@0x%08x", ch->size, ch->addr );
            fflush(stdout) ; 
            }
         s0printf(". (%d)\n", cnt );
         fflush(stdout) ; 
         }
      }

       
    void
    CheckFreeChunks( )
      {
      int i;
      BL_MemChunk *ch;

      for ( i = 0 ; i < 32 ; i++ )
         {
         BL_MemChunk * chp = NULL ;
         int chainfault = 0 ; 
         for ( ch = mBL_MemMgr.free_chunks[i] ; ch ; ch = ch->next )
            {
          if ( 0 == IsVacant(ch->addr) ) 
          {
             BLMDB1(("(E) CheckFreeChunks chunk = 0x%08x addr = 0x%08x , size = 0x%08x , i = %d should be on busy list\n", ch, ch->addr, ch->size, i ));
             chainfault = 1 ;
          }
          if ( ch->prev != chp )
          {
             BLMDB1(("(E) CheckFreeChunks chunk = 0x%08x addr = 0x%08x , size = 0x%08x , i = %d prev=%08x chp=%08x chain broken\n", ch, ch->addr, ch->size, i, ch->prev, chp ));
             chainfault = 1 ;
          }
          if ( 0 == ( ch->addr & BLMEMCHUNK_ADDR_FREE ) )
          {
             BLMDB1(("(E) CheckFreeChunks chunk = 0x%08x addr = 0x%08x , size = 0x%08x , i = %d prev=%08x chp=%08x 'free' bit clear\n", ch, ch->addr, ch->size, i, ch->prev, chp ));
             chainfault = 1 ;
          }
          chp=ch ;
           }
         if ( chainfault )
         {
         for ( ch = mBL_MemMgr.free_chunks[i] ; ch ; ch = ch->next )
          {
             BLMDB1(("(I) CheckFreeChunks chunk=0x%08x next=0x%08x prev=0x%08x addr=0x%08x size=0x%08x\n", ch, ch->next, ch->prev, ch->addr, ch->size ));
          }
         }
         }
      }

    void
    CheckBusyChunks( )
      {
      int i;
      BL_MemChunk *ch;
      int count = 0 ; 
      int size = 0 ;
      for ( i = 0 ; i < 256 ; i++ )
         {
         BL_MemChunk * chp = NULL ;
         int chainfault = 0 ; 
         for ( ch = mBL_MemMgr.busy_chunks[i] ; ch ; ch = ch->next )
         {
          if ( 0 == IsOccupied(ch->addr) ) 
          {
             BLMDB1(("(E) CheckBusyChunks chunk = 0x%08x addr = 0x%08x , size = 0x%08x , i = %d should be on free list\n", ch, ch->addr, ch->size, i ));
             chainfault = 1 ;
          }
          if ( ch->prev != chp )
          {
             BLMDB1(("(E) CheckBusyChunks chunk = 0x%08x addr = 0x%08x , size = 0x%08x , i = %d prev=%08x chp=%08x chain broken\n", ch, ch->addr, ch->size, i, ch->prev, chp ));
             chainfault = 1 ;
          }
          if ( ch->addr & BLMEMCHUNK_ADDR_FREE )
          {
             BLMDB1(("(E) CheckBusyChunks chunk = 0x%08x addr = 0x%08x , size = 0x%08x , i = %d prev=%08x chp=%08x 'free' bit set\n", ch, ch->addr, ch->size, i, ch->prev, chp ));
             chainfault = 1 ;
          }
          chp=ch ;
          count += 1 ;
          size += ch->size ;
         }
         
         if ( chainfault )
         {
           for ( ch = mBL_MemMgr.free_chunks[i] ; ch ; ch = ch->next )
            {
               BLMDB1(("(I) CheckBusyChunks chunk=0x%08x next=0x%08x prev=0x%08x addr=0x%08x size=0x%08x\n", ch, ch->next, ch->prev, ch->addr, ch->size ));
            }
         }
         }
         if ( count != mBL_MemMgr.busy_count )
         {
             BLMDB1(("(E) CheckBusyChunks count=%d busy_count=%d\n", count, mBL_MemMgr.busy_count ));        	
         }
         if ( size != mBL_MemMgr.mem_busy )
         {
             BLMDB1(("(E) CheckBusyChunks size=0x%08x mem_busy=0x%08xi\n", size, mBL_MemMgr.mem_busy ));        	
         }
      }
      
    void
    ShowBusyChunks( )
      {
      int i, j;
      BL_MemChunk *ch;

      for ( i = 0 ; i < 256 ; i++ )
         {
         s0printf("Busy Chunks[%2d]: ", i );
         fflush(stdout) ;
         for ( j = 0, ch = mBL_MemMgr.busy_chunks[i] ; ch ; ch = ch->next, j++ )
         {
             s0printf(" 0x%08x@0x%08x", ch->size, ch->addr );
             fflush(stdout) ;
         }
         s0printf(" (Total = %d).\n", j );
         fflush(stdout) ;
         }
      }

    void
    ShowByAddrList( )
      {
      int i;
      uint32_t a = 0;
      BL_MemChunk *ch = mBL_MemMgr.byaddr_anchor;


      s0printf("ShowByAddrList: start\n");
      fflush(stdout) ; 
      for ( i = 0 ; ch ; ch = ch->byaddr_next, i++ )
         {
         s0printf("[%3d]: 0x%08x@0x%08x %s\n", i, ch->size, ch->addr,
                ((ch->addr > a) ? "Ok" : "ERROR") );
         fflush(stdout) ; 
         a = ch->addr;
         }
      s0printf("ShowByAddrList: done.\n");
      fflush(stdout) ; 

      }

    void
    ShowStats( )
      {
      s0printf("ShowStats: mem_start = 0x%08x, mem_size = 0x%08x\n",
             mBL_MemMgr.mem_start, mBL_MemMgr.mem_size );
      fflush(stdout) ; 
      s0printf("ShowStats: mem_avail = 0x%08x, mem_busy = 0x%08x, busy_count = %d.\n",
             mBL_MemMgr.mem_avail, mBL_MemMgr.mem_busy, mBL_MemMgr.busy_count );
      fflush(stdout) ; 

      }

    void
    ShowBusy( )
      {
      s0printf("ShowStats: mem_avail = %u, mem_busy = %u.\n",
               mBL_MemMgr.mem_avail, mBL_MemMgr.mem_busy );
      fflush(stdout) ; 
      }

    void
    ShowAll( )
      {
      ShowFreePool( );
      ShowFreeChunks( );
      ShowBusyChunks( );
      ShowByAddrList( );
      ShowStats( );
      ShowBusy( );
      }

    void CheckAll(void)
    {
      BLMDB3(("(I) Checking\n")) ;
      CheckBusyChunks() ; 
      CheckFreeChunks() ; 
    }

    #endif // BL_MALLOC_DEBUG > 0


    // remove specific chunk from its bucket
    inline
    void
    DeQFreeMemChunk( BL_MemChunk *chunk )
      {
    BLMDB3(("(I) DeQFreeMemChunk chunk = 0x%08x addr = 0x%08x , size = 0x%08x \n", chunk, chunk->addr, chunk->size ));
 #if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif 
      mBL_MemMgr.checksum_check() ;
      chunk->checksum_check() ;      
      int b = GetBucket( chunk->size );

      if ( chunk->prev )
      {
        chunk->prev->checksum_check() ;
        chunk->prev->next = chunk->next;
        chunk->prev->checksum_set() ;
      }
      else
      {
        mBL_MemMgr.free_chunks[b] = chunk->next;
      }

      if ( chunk->next )
      {
        chunk->next->checksum_check() ;
        chunk->next->prev = chunk->prev;
        chunk->next->checksum_set() ;
      }

      chunk->next = NULL;
      chunk->prev = NULL;
      chunk->checksum_set() ; 
      mBL_MemMgr.mem_avail -= chunk->size;
      mBL_MemMgr.checksum_set() ;
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      }


    // insert into appropriate bucket sorted by size
    void
    EnQFreeMemChunk( BL_MemChunk *chunk )
      {
    BLMDB3(("(I) EnQFreeMemChunk chunk = 0x%08x addr = 0x%08x , size = 0x%08x \n", chunk, chunk->addr, chunk->size ));
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      mBL_MemMgr.checksum_check() ;
      chunk->checksum_check() ;      
      int b = GetBucket( chunk->size );
      BL_MemChunk *t, *tn;

      BLMDB1_Fcn( if ( !chunk->addr || !chunk->size )
                      {
                      s0printf("(E) EnQFreeMemChunk: chunk->addr = 0x%08x chunk->size = 0x%08x\n",
                               chunk->addr, chunk->size );
                      BLM_CRASH( -1 );
                      }
                  if ( chunk->next || chunk->prev )
                     {
                     s0printf("(E) EnQFreeMemChunk: chunk->next = 0x%08x chunk->prev = 0x%08x\n",
                              (uint32_t)chunk->next, (uint32_t)chunk->prev );
                     BLM_CRASH( -1 );
                     } );

      if( (t = mBL_MemMgr.free_chunks[b]) )
        {
          t->checksum_check() ; 
        // skip t past smaller chunks
        int sq=0 ; // Diagnostics -- count how many we are skipping past
        for ( tn = t->next ; tn ; tn = tn->next )
           {
          tn->checksum_check() ; 
           if ( tn->size <= chunk->size )
              break;
           t = tn;
           sq += 1 ;
           }
        // insert before t
      BLMDB3(("(I) EnQFreeMemChunk bucket=%d sq=%d t=0x%08x t->prev=0x%08x\n", b, sq, t, t->prev ));
        chunk->next = t;
        if ( (chunk->prev = t->prev) )
        {
           chunk->prev->checksum_check() ;
           chunk->prev->next = chunk;
           chunk->prev->checksum_set() ;
        } else {
          // Tacking this chunk in at the beginning of the chain
          mBL_MemMgr.free_chunks[b] = chunk;
        }
        t->prev = chunk;
        t->checksum_set() ; 
        }
      else
        {
        // bucket is empty
      BLMDB3(("(I) EnQFreeMemChunk bucket=%d was empty\n", b ));
        mBL_MemMgr.free_chunks[b] = chunk;
        chunk->next = NULL;
        chunk->prev = NULL;
        }

      chunk->addr |= BLMEMCHUNK_ADDR_FREE;
      chunk->checksum_set() ; 
      mBL_MemMgr.mem_avail += chunk->size;
      mBL_MemMgr.checksum_set() ;
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      }


    // insert into busy mem hash bucket
    //  newest chunks at head because more like to free newer chunks
    inline
    void
    EnQBusyMemChunk( BL_MemChunk *chunk )
      {
    BLMDB3(("(I) EnQBusyMemChunk chunk = 0x%08x addr = 0x%08x , size = 0x%08x \n", chunk, chunk->addr, chunk->size ));
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      mBL_MemMgr.checksum_check() ;
      chunk->checksum_check() ;      
      int busybuck = BusyAddrHash( chunk->addr );

      if ( (chunk->next = mBL_MemMgr.busy_chunks[busybuck]) )
      {
         chunk->next->checksum_check() ;
         chunk->next->prev = chunk;
         chunk->next->checksum_set() ;
      }
      mBL_MemMgr.busy_chunks[busybuck] = chunk;
      chunk->prev = NULL;

      chunk->checksum_set() ; 
      mBL_MemMgr.mem_busy += chunk->size;
      mBL_MemMgr.busy_count++;
      mBL_MemMgr.checksum_set() ;
      
      Sow(chunk->addr, chunk->size) ;
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      }


    // find busy chunk and remove it from it's hash bucket
    inline
    BL_MemChunk *
    DeQBusyMemChunk( uint32_t addr )
      {
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      mBL_MemMgr.checksum_check() ;
      int busybucket = BusyAddrHash( addr );
      BL_MemChunk *t;

      for( t = mBL_MemMgr.busy_chunks[busybucket] ; t ; t = t->next )
        {
          t->checksum_check() ; 
        if ( t->addr == addr )
           break;
        }
      if( !t )
      {
        BLMDB1(("(E) Chunk at 0x%08x not found in busy list\n")) ;
        return( NULL );
      }
        
    BLMDB3(("(I) DeQBusyMemChunk chunk = 0x%08x addr = 0x%08x , size = 0x%08x \n", t, t->addr, t->size ));

      if( t->prev )
        {
          t->prev->checksum_check() ;
        t->prev->next = t->next;
        t->prev->checksum_set() ;
        if( t->next )
        {
          t->next->checksum_check() ;
           t->next->prev = t->prev;
          t->next->checksum_set() ;
        }
        }
      else // !t->prev
        {
        mBL_MemMgr.busy_chunks[busybucket] = t->next;
        if( t->next )
        {
          t->next->checksum_check() ;
           t->next->prev = NULL;
          t->next->checksum_set() ;
        }
        }

      t->next = NULL;
      t->prev = NULL;
      t->checksum_set() ; 
      mBL_MemMgr.mem_busy -= t->size;
      mBL_MemMgr.busy_count--;
      mBL_MemMgr.checksum_set() ;
      
      Reap(t->addr, t->size) ;
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      

      return( t );
      }


    void
    ChunkFreePoolInit( BL_MemChunkPool * pool )
      {
      int i;
      BL_MemChunk *c;

      memset( (void *) pool, 0, sizeof(BL_MemChunkPool) );

      // save 1 chunk for emergency use (allocation of new pools)
      c = &(pool->chunks[0]);
      c->my_pool = pool;
      c->checksum_set() ;
      pool->last_chunk = c;

      // free list the rest of the chunks
      for ( i = 1 ; i < BLMEMCHUNK_POOL_SIZE ; i++ )
        {
        c = &(pool->chunks[i]);
        c->my_pool = pool;
        if ( (c->next = pool->chunk_anchor) )
        {
           c->next->checksum_check() ; 
           c->next->prev = c;
           c->next->checksum_set() ; 
        }
        c->prev = NULL;
        c->checksum_set() ;
        pool->chunk_anchor = c;
        }
        pool->checksum_set() ;
      }


    void
    Chunk2FreePool( BL_MemChunk *chunk )
      { 
        mBL_MemMgr.checksum_check() ; 
        chunk->checksum_check() ; 
      BL_MemChunkPool *pool = chunk->my_pool;
      if( !pool )
        {
        s0printf("(E) Chunk2FreePool: No pool!\n");
        BLM_CRASH( -4 );
        }
      pool->checksum_check() ;

      if( chunk->size )
        s0printf("(E) Chunk2FreePool: chunk not empty! size = %d\n", chunk->size );

      if( !(pool->last_chunk) )
        {
        pool->last_chunk = chunk;
        chunk->next = NULL;
        chunk->prev = NULL;
        }
      else
        {
        if( (chunk->next = pool->chunk_anchor) )
        {
          chunk->next->checksum_check() ;
           chunk->next->prev = chunk;
          chunk->next->checksum_set() ;
        }
        chunk->prev = NULL;
        pool->chunk_anchor = chunk;
        }
        chunk->checksum_set() ; 

      InitQByAddr( chunk );

      pool->used_count--;

      if( !pool->used_count )
        {
        //@MG: Free cannot recurse!
        BLMDB2(("(I) Chunk2FreePool: Freeing Pool at 0x%08x!\n", (uint32_t)pool ));

        if( pool->prev )
          {
          pool->prev->checksum_check() ;
          pool->prev->next = pool->next;
          pool->prev->checksum_set() ;
          if ( pool->next )
          {
            pool->next->checksum_check() ;
            pool->next->prev = pool->prev;
            pool->next->checksum_set() ;
          }
          }
        else // !pool->prev
          {
          if ( pool->next )
          {
            pool->next->checksum_check() ;
            pool->next->prev = NULL;
            pool->next->checksum_set() ;
          }
          }

        pool->next = mBL_MemMgr.free_pool_later_anchor;
        pool->prev = NULL;
        mBL_MemMgr.free_pool_later_anchor = pool;
        mBL_MemMgr.checksum_set() ; 
        }
        pool->checksum_set() ; 
      }


    BL_MemChunk *
    GetFreePoolChunk( )
      {
      BL_MemChunk     * c = NULL;
      BL_MemChunkPool * tpool, *new_pool, *first_pool;
      mBL_MemMgr.checksum_check() ;
      for( first_pool = NULL, tpool = mBL_MemMgr.pool_anchor ; tpool ; tpool = tpool->next )
        {
        if( !first_pool )
          {
            tpool->checksum_check() ;
          if( tpool->last_chunk )
          {
            first_pool = tpool;
          }
          }
        if( (c = tpool->chunk_anchor) )
          {
          break;
          }
        }

      if( c )
        {
        c->checksum_check() ;
        tpool = c->my_pool;
        tpool->checksum_check() ;
        if ( (tpool->chunk_anchor = c->next) )
        {
           tpool->chunk_anchor->checksum_check() ;
           tpool->chunk_anchor->prev = NULL;
           tpool->chunk_anchor->checksum_set() ;
        }
        c->next    = NULL;
        c->prev    = NULL;
        c->checksum_set() ;
        tpool->used_count++;
        tpool->checksum_set() ;
        return( c );
        }

      if( !first_pool )
        {
        BLMDB1(("GetFreePoolChunk: No pool with last_chunk!\n"));
        return( NULL );
        }
      else
        {
        BLMDB2(("GetFreePoolChunk: Growing Free Pool!\n"));
        if ( !(c = first_pool->last_chunk) )
           return( NULL );
        first_pool->last_chunk = NULL;
        first_pool->used_count++;
        first_pool->checksum_set() ;

        // note: if alloc fails, last_chunk is restored
        new_pool = (BL_MemChunkPool *) Allocate( BL_ROUND_UP_CACHELINE( sizeof(BL_MemChunkPool) ),
                                                         c );
        if( !new_pool )
          {
          BLMDB1(("GetFreePoolChunk: malloc fails for new pool!\n"));
          return( NULL );
          }

        ChunkFreePoolInit( new_pool );

        // new pool has to go on end of list so it's chunks are used last
        for( tpool = mBL_MemMgr.pool_anchor ; tpool ; tpool = tpool->next )
          {
            tpool->checksum_check() ;
          if( !tpool->next )
            break;
          }
        tpool->next = new_pool;
        tpool->checksum_set() ;
        new_pool->prev = tpool;

        // grab a chunk from the new chunk pool
        c = new_pool->chunk_anchor;
        c->checksum_check() ; 
        if ( (new_pool->chunk_anchor = c->next) )
        {
              new_pool->chunk_anchor->checksum_check() ;
              new_pool->chunk_anchor->prev = NULL;
              new_pool->chunk_anchor->checksum_set() ;
        }
        c->next    = NULL;
        c->prev    = NULL;
        c->checksum_set() ; 
        new_pool->used_count++;
        new_pool->checksum_set() ;
        }

      return( c );
      }

    void
    FreeChunk( BL_MemChunk * chunk )
      {
      BL_MemChunk *t;
      mBL_MemMgr.checksum_check() ;
      chunk->checksum_check() ;
      
      checksum_set(chunk->addr, chunk->size) ; // No-one should access this chunk now until 'Allocaste' hands it out again
      
      if( !chunk->addr || !chunk->size )
        {
        BLMDB1(("(E) FreeChunk: chunk->addr = 0x%08x chunk->size = 0x%08x\n",
               chunk->addr, chunk->size ));
        BLM_CRASH( -1 );
        }

      //can we merge this chunk with free neighbors?
      if( (t = chunk->byaddr_prev) )
        {
        t->checksum_check() ; 
        if( t->addr & BLMEMCHUNK_ADDR_FREE )
          {
          BLMDB2(("FreeChunk: (prev) merge chunk@0x%08x(%d) with free@0x%08x(%d)\n",
                   chunk->addr, chunk->size, t->addr, t->size ));

          DeQFreeMemChunk( t );
          t->size += chunk->size;
          t->checksum_set() ; 
          chunk->addr = 0;
          chunk->size = 0;
          chunk->checksum_set() ; 
          DeQByAddr( chunk );
          Chunk2FreePool( chunk );
          chunk = t;
          }
        }
      if( (t = chunk->byaddr_next) )
        {
        t->checksum_check() ; 
        if( t->addr & BLMEMCHUNK_ADDR_FREE )
          {
          BLMDB2(("FreeChunk: (next) merge chunk@0x%08x(%d) with free@0x%08x(%d)\n",
                   chunk->addr, chunk->size, t->addr, t->size ));

          DeQFreeMemChunk( t );
          chunk->size += t->size;
          chunk->checksum_set() ; 
          t->addr = 0;
          t->size = 0;
          t->checksum_set() ; 
          DeQByAddr( t );
          Chunk2FreePool( t );
          }
        }

      EnQFreeMemChunk( chunk );
      }


    // low-level malloc: rbytes pre-checked and new_chunk pre-allocated
    void *
    Allocate( uint32_t rbytes, BL_MemChunk *new_chunk )
      {
      BL_MemChunk *ch;
      int need_buck = GetBucket( rbytes );
      int have_buck;
      
      mBL_MemMgr.checksum_check() ;
      new_chunk->checksum_check() ; 

      // find smallest available chunk in smallest bucket that fits this allocation
      for ( have_buck = need_buck ; have_buck < 32 ; have_buck++ )
        {
        if ( mBL_MemMgr.free_chunks[have_buck] )
           break;
        }
      if( have_buck >= 32 )
        {
        Chunk2FreePool( new_chunk );
        return(NULL);
        }

      // search for smallest chunk in this bucket that fits this allocation
      for( ch = mBL_MemMgr.free_chunks[have_buck] ; ch ; ch = ch->next )
        {
        ch->checksum_check() ; 
        if( ch->size >= rbytes )
           break;
        }
      if( ! ch )
        {
        Chunk2FreePool( new_chunk );
        return( NULL );
        }

      // remove chunk from free mem bucket
      DeQFreeMemChunk( ch );

      // do we have to split the chunk?
      if( ch->size > rbytes )
        {
        // split this chunk into ch (the allocation) and new_chunk (the left-over)
        new_chunk->addr = ch->addr + rbytes;
        new_chunk->size = ch->size - rbytes;
        new_chunk->checksum_set() ; 
        ch->size        = rbytes;
        ch->checksum_set() ; 
        EnQByAddr( new_chunk, ch );

        // return left-over in new_chunk to free pool
        EnQFreeMemChunk( new_chunk );
        }
      else
        {
        // we didn't need new_chunk afterall, so free it
        Chunk2FreePool( new_chunk );
        }

      // clear the free flag
      ch->addr &= BLMEMCHUNK_ADDR_MASK;
      ch->checksum_set() ; 

      // put chunk on busy list
      EnQBusyMemChunk( ch );
      
      checksum_check(ch->addr, ch->size) ; // Block should be intact when handed out

      return( (void *)ch->addr );
      }

    //
    // Public routines: malloc, calloc, free, realloc, and HeapManager_Init.
    //

    void *
    bgl_malloc( size_t bytes )
      {
      Mutex( 1 );
      void * rc = backend_malloc( bytes );
      Mutex( 0 );
      return( rc );
      }

    void *
    backend_malloc( size_t bytes )
      {
      BL_MemChunk *new_chunk;
      uint32_t rbytes = BL_ROUND_UP_CACHELINE( bytes );

      BLMDB2(("_blHeapManager: 0x%08x bytes = 0x%08x rbytes.\n", bytes, rbytes ));
      mBL_MemMgr.checksum_check() ;
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      if ( (bytes == 0) || (mBL_MemMgr.mem_avail < rbytes) )
         return( NULL );

      // get chunk from free chunk pool (if none, fail allocation)
      if ( !(new_chunk = GetFreePoolChunk()) )
         return( NULL );

      void * result = Allocate( rbytes, new_chunk );
      
      
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      
      
      return result ;

      }


    void
    bgl_free( void *addr )
      {
      Mutex( 1 );
      backend_free( addr );
      Mutex( 0 );
      }

    void
    backend_free( void *addr )
      {
      BL_MemChunk *chunk;
      uint32_t a = (uint32_t)addr;

      BLMDB2(("(I) _blFree: free addr 0x%08x.\n", a ));
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      

      if( !(chunk = DeQBusyMemChunk( a )) )
        {
        s0printf("(E) _blFree: addr 0x%08x not allocated!\n", a );
        BLMDB2_Fcn(( ShowByAddrList() ));
        BLM_CRASH( -2 );
        }

      FreeChunk( chunk );

      while ( mBL_MemMgr.free_pool_later_anchor )
        {
        a = (uint32_t)mBL_MemMgr.free_pool_later_anchor;
        mBL_MemMgr.free_pool_later_anchor = mBL_MemMgr.free_pool_later_anchor->next;
        mBL_MemMgr.checksum_set() ; 
        BLMDB2(("(I) _blFree De-Recurse: free addr 0x%08x.\n", a ));

        if( !(chunk = DeQBusyMemChunk( a )) )
          {
          s0printf("(E) _blFree De-Recurse: addr 0x%08x not allocated!\n", a );
          BLMDB2_Fcn(( ShowByAddrList()));
          BLM_CRASH(-2);
          }

        FreeChunk( chunk );
        }
#if BL_MALLOC_DEBUG > 2
      CheckAll() ;
#endif      

      }

    static
    inline
    void
    Mutex( int aLockAction ) // 1 for acquire, 0 for release
      {
      #if 1
      static BGL_Mutex *m = NULL;

      //printf("Entered Mutex LockAction %d  m @%08X\n", aLockAction, m);

      if( m == NULL )
        {
        m = rts_allocate_mutex();
        //printf("Mutex allocated at  m @%08X\n", m);
        assert( m );
        }

      if( aLockAction == 1 )
        {
        BGL_Mutex_Acquire( m );
        rts_dcache_evict_normal();
        }
      else
        {
        rts_dcache_evict_normal();
        BGL_Mutex_Release( m );
        }
      #endif
      return;
      }

    unsigned int
    Init( int aHeapMegs = -1 )
      {
      Mutex( 1 );

      BL_MemChunk *pch, *fch;  // chunks for pool and free mem
      uint32_t psz = BL_ROUND_UP_CACHELINE( sizeof(BL_MemChunkPool) );
      uint32_t start = 0;
      size_t   bytes = 0;

      BLMDB2(("(I) _blHeapManager_Init: sizeof(BL_MemMgr) = %d, sizeof(BL_MemChunkPool) = %d, psz = %d, sizeof(BL_MemChunk) = %d.\n",
               sizeof(BL_MemMgr), sizeof(BL_MemChunkPool), psz, sizeof(BL_MemChunk) ));

      memset( &mBL_MemMgr, 0, sizeof(BL_MemMgr) );

      if( aHeapMegs == -1 )
        aHeapMegs = PK_HEAP_MB;   // leave something, but not much, for system software

      bytes = aHeapMegs * 1024 * 1024;  // Try a '10 MB' shareable heap

      if( bytes <= 0 )
        BLM_CRASH( -1 );

      BLMDB1(("(I) HeapManager::Init: Trying to allocate %d bytes (%f MB )\n", bytes, bytes / (1024.0*1024.0) ));

      while( start == 0 && bytes >= 1 )
        {
        start = (uint32_t) malloc( bytes );
        bytes--;
        }

      bytes++;

      if( start == 0 )
        {
        s0printf("(E) _blHeapManager_Init: No Memory!\n");
        BLM_CRASH( -1 );
        }

//      // Init all memory words to 0xDEADBEEF
//      for( int i = 0; i < (bytes / sizeof(int)); i++ )
//        {
//        // would prefer to set to a value other than zero - but heap allocs do not fully init.
//        ////((int*)start)[ i ] = 0xDEADBEEF;
//        //((int*)start)[ i ] = 0;
//        }

      BLMDB1(("(I) HeapManager::Init: start = 0x%08x, bytes = %d. (%f MB )\n", start, bytes, bytes / (1024.0*1024.0) ));

      mBL_MemMgr.mem_start = BL_ROUND_UP_CACHELINE(   start );
      mBL_MemMgr.mem_size  = BL_ROUND_DOWN_CACHELINE( bytes );
      mBL_MemMgr.mem_avail = 0;
      mBL_MemMgr.mem_busy  = 0;
      
      checksum_set(mBL_MemMgr.mem_start,mBL_MemMgr.mem_size) ; 

      // grab the initial chunk pool
      mBL_MemMgr.pool_anchor = (BL_MemChunkPool *)mBL_MemMgr.mem_start;
      ///memset( (void*) mBL_MemMgr.mem_start, 0, sizeof(BL_MemChunkPool) );
      mBL_MemMgr.checksum_set() ; 

      ChunkFreePoolInit( mBL_MemMgr.pool_anchor );

      // create the initial pool chunk and free chunk
      pch = GetFreePoolChunk();
      fch = GetFreePoolChunk();
      if( !pch || !fch )
        {
        s0printf("(E) HeapManager::Init: Could not get initial free & pool chunks!\n");
        BLM_CRASH( -6 );
        }
      pch->addr = mBL_MemMgr.mem_start;
      pch->size = psz;
      pch->checksum_set() ; 
      mBL_MemMgr.byaddr_anchor = pch;
      mBL_MemMgr.checksum_set() ; 

      fch->addr = (mBL_MemMgr.mem_start + psz);
      fch->size = (mBL_MemMgr.mem_size  - psz);
      fch->addr |= BLMEMCHUNK_ADDR_FREE;
      fch->checksum_set() ; 

      EnQByAddr( fch, pch );

      // put free mem in free pool
      EnQFreeMemChunk( fch );

      // put pool chunk on busy list
      EnQBusyMemChunk( pch );

      Mutex( 0 );
      return( bytes );
      }

  };
  
#if defined(BL_MALLOC_VALIDATE)

// Use a cache-line-repeating pattern to try and spot if 'free' memory is being scribbled on
#define CHECK_PATTERN_0 0xbaddf001
#define CHECK_PATTERN_1 0xc01dcaff
#define CHECK_PATTERN_2 0xbaddf003
#define CHECK_PATTERN_3 0xc01dcafd
#define CHECK_PATTERN_4 0xbaddf005
#define CHECK_PATTERN_5 0xc01dcafb
#define CHECK_PATTERN_6 0xbaddf007
#define CHECK_PATTERN_7 0xc01dcaf9

void HeapManagerT::checksum_check(int addr, int len) const 
{ 
  // Only do cache-line-size chunks
  assert(addr == BL_ROUND_DOWN_CACHELINE(addr)) ; 
  assert(len == BL_ROUND_DOWN_CACHELINE(len)) ; 
  int r = 0 ; 
  for (int a=addr;a<addr+len;a+=32)
  {
    int * ap = (int *) a ;
    int v0 = ap[0] ;
    int v1 = ap[1] ;
    int v2 = ap[2] ;
    int v3 = ap[3] ;
    int v4 = ap[4] ;
    int v5 = ap[5] ;
    int v6 = ap[6] ;
    int v7 = ap[7] ;
    r |= (v0 - CHECK_PATTERN_0)
       | (v1 - CHECK_PATTERN_1)
       | (v2 - CHECK_PATTERN_2)
       | (v3 - CHECK_PATTERN_3)
       | (v4 - CHECK_PATTERN_4)
       | (v5 - CHECK_PATTERN_5)
       | (v6 - CHECK_PATTERN_6)
       | (v7 - CHECK_PATTERN_7) ;
  }
  if ( r != 0 )
  {
    assert(0) ; 
    *(int *)0=0 ; // Trap if the assert returns
  }
  
} 
void HeapManagerT::checksum_set(int addr, int len)  
{ 
  // Only do cache-line-size chunks
  assert(addr == BL_ROUND_DOWN_CACHELINE(addr)) ; 
  assert(len == BL_ROUND_DOWN_CACHELINE(len)) ; 
  for (int a=addr;a<addr+len;a+=32)
  {
    int * ap = (int *) a ;
    ap[0] = CHECK_PATTERN_0 ;
    ap[1] = CHECK_PATTERN_1 ;
    ap[2] = CHECK_PATTERN_2 ;
    ap[3] = CHECK_PATTERN_3 ;
    ap[4] = CHECK_PATTERN_4 ;
    ap[5] = CHECK_PATTERN_5 ;
    ap[6] = CHECK_PATTERN_6 ;
    ap[7] = CHECK_PATTERN_7 ;
  }
} 
#else
void HeapManagerT::checksum_check(int addr, int len) const { } 
void HeapManagerT::checksum_set(int addr, int len)  { } 
#endif

HeapManagerT _pkHeapManager;

void
_pkInitHeapManager()
  {
  _pkHeapManager.Init();
  }


void *
pkHeapAllocate(int bytes)
  {
  void *rc = _pkHeapManager.bgl_malloc(bytes);
  return(rc);
  }

void
pkHeapFree(void *addr)
  {
  _pkHeapManager.bgl_free(addr);
  }

void* pkPlugHeapAllocate( size_t bytes )
{
  void * result = pkHeapAllocate(bytes) ;
  BegLogLine(PKFXLOG_HEAP_PLUG) 
    << "pkPlugHeapAllocate bytes=" << bytes
    << " result=0x" << result
    << EndLogLine ;
  return result ;
}

void  pkPlugHeapFree( void * addr )
{
  BegLogLine(PKFXLOG_HEAP_PLUG) 
    << "pkPlugHeapFree addr=0x" << addr
    << EndLogLine ;
#if !defined(PK_LEAKY_HEAP)
  pkHeapFree(addr) ; 
#endif	
}
  
