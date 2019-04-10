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
///////////#include <blade.h>

#ifndef __HEAP_MANAGER_HPP__
#define __HEAP_MANAGER_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <bgllockbox.h>
#include <rts.h>
// #include <mpi.h>
#include <assert.h>

#define KILO (1024)
#define K_DATA /**/
#if !defined(ALIGN_L3_CACHE)
#define ALIGN_L3_CACHE /**/
#endif
#define s0printf printf

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

#define BL_MALLOC_DEBUG (0)
//#define BL_MALLOC_DEBUG (1)
//#define BL_MALLOC_DEBUG (2)
//#define BL_MALLOC_DEBUG (3) // you'll be sorry

#define TRC_MSG_FCN printf

#if BL_MALLOC_DEBUG > 0
#define BLMDB1( Args )    do { TRC_MSG_FCN Args ; } while(0)
#define BLMDB1_Fcn( Fcn ) do { Fcn ; } while(0)
#else
#define BLMDB1( Args )    do { /* nothing */ } while(0)
#define BLMDB1_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

#if BL_MALLOC_DEBUG > 1
#define BLMDB2( Args )    do { TRC_MSG_FCN Args ; } while(0)
#define BLMDB2_Fcn( Fcn ) do { Fcn ; } while(0)
#else
#define BLMDB2( Args )    do { /* nothing */ } while(0)
#define BLMDB2_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

#if BL_MALLOC_DEBUG > 2
#define BLMDB3( Args )    do { TRC_MSG_FCN Args ; } while(0)
#define BLMDB3_Fcn( Fcn ) do { Fcn ; } while(0)
#else
#define BLMDB3( Args )    do { /* nothing */ } while(0)
#define BLMDB3_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

//#define BLM_CRASH(rc) Sim_Terminate( (rc) )
#define BLM_CRASH(rc) (printf("crash\n"),exit(-1));


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
            struct BL_MemChunk       chunks[ BLMEMCHUNK_POOL_SIZE ];
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
            };


class HeapManager
  {
  public:

    BL_MemMgr K_DATA mBL_MemMgr ALIGN_L3_CACHE ;

    inline
    void
    InitQByAddr( BL_MemChunk *chunk )
      {
      chunk->byaddr_next = NULL;
      chunk->byaddr_prev = NULL;
      }

    inline
    void
    EnQByAddr( BL_MemChunk *n, BL_MemChunk *o )
      {
      BLMDB3(("EnQ: new->addr = 0x%08x, old->addr = 0x%08x\n", n->addr, o->addr ));

      if( n->addr < o->addr )
        {
        s0printf("(E) MemMgr Internal Error: _EnQByAddr: new < old\n");
        BLM_CRASH( 3 );
        }

      if( (n->byaddr_next = o->byaddr_next) )
         n->byaddr_next->byaddr_prev = n;
      o->byaddr_next = n;
      n->byaddr_prev = o;
      }

    inline
    void
    DeQByAddr( BL_MemChunk *c )
      {
      if( c->byaddr_prev )
         c->byaddr_prev->byaddr_next = c->byaddr_next;
      else
         mBL_MemMgr.byaddr_anchor = c->byaddr_next;

      if( c->byaddr_next )
         c->byaddr_next->byaddr_prev = c->byaddr_prev;

      c->byaddr_next = NULL;
      c->byaddr_prev = NULL;
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
        return;
        }

      for( j = 0 ; pool ; pool = pool->next , j++ )
        {
        s0printf("(I) Free Pool(%d):\n", j );

        for ( i = 0, ch = pool->chunk_anchor ; ch ; ch = ch->next, i++ )
           s0printf("  %2d.%4d: 0x%08x\n", j, i, (uint32_t)ch );
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
         for ( ch = mBL_MemMgr.free_chunks[i] ; ch ; ch = ch->next )
            {
            cnt++;
            s0printf(" 0x%08x@0x%08x", ch->size, ch->addr );
            }
         s0printf(". (%d)\n", cnt );
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
         for ( j = 0, ch = mBL_MemMgr.busy_chunks[i] ; ch ; ch = ch->next, j++ )
             s0printf(" 0x%08x@0x%08x", ch->size, ch->addr );
         s0printf(" (Total = %d).\n", j );
         }
      }

    void
    ShowByAddrList( )
      {
      int i;
      uint32_t a = 0;
      BL_MemChunk *ch = mBL_MemMgr.byaddr_anchor;


      s0printf("ShowByAddrList: start\n");
      for ( i = 0 ; ch ; ch = ch->byaddr_next, i++ )
         {
         s0printf("[%3d]: 0x%08x@0x%08x %s\n", i, ch->size, ch->addr,
                ((ch->addr > a) ? "Ok" : "ERROR") );
         a = ch->addr;
         }
      s0printf("ShowByAddrList: done.\n");

      }

    void
    ShowStats( )
      {
      s0printf("ShowStats: mem_start = 0x%08x, mem_size = 0x%08x\n",
             mBL_MemMgr.mem_start, mBL_MemMgr.mem_size );
      s0printf("ShowStats: mem_avail = 0x%08x, mem_busy = 0x%08x, busy_count = %d.\n",
             mBL_MemMgr.mem_avail, mBL_MemMgr.mem_busy, mBL_MemMgr.busy_count );

      ShowFreeChunks();

      ShowByAddrList();
      }

    void
    ShowBusy( )
      {
      s0printf("ShowStats: mem_avail = %u, mem_busy = %u.\n",
               mBL_MemMgr.mem_avail, mBL_MemMgr.mem_busy );
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


    #endif // BL_MALLOC_DEBUG > 0


    // remove specific chunk from it's bucket
    inline
    void
    DeQFreeMemChunk( BL_MemChunk *chunk )
      {
      int b = GetBucket( chunk->size );

      if ( chunk->prev )
         chunk->prev->next = chunk->next;
      else
         mBL_MemMgr.free_chunks[b] = chunk->next;

      if ( chunk->next )
         chunk->next->prev = chunk->prev;

      chunk->next = NULL;
      chunk->prev = NULL;
      mBL_MemMgr.mem_avail -= chunk->size;
      }


    // insert into appropriate bucket sorted by size
    void
    EnQFreeMemChunk( BL_MemChunk *chunk )
      {
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
        // skip t past smaller chunks
        for ( tn = t->next ; tn ; tn = tn->next )
           {
           if ( tn->size <= chunk->size )
              break;
           t = tn;
           }
        // insert before t
        chunk->next = t;
        if ( (chunk->prev = t->prev) )
           chunk->prev->next = chunk;
        t->prev = chunk;
        }
      else
        {
        // bucket is empty
        mBL_MemMgr.free_chunks[b] = chunk;
        chunk->next = NULL;
        chunk->prev = NULL;
        }

      chunk->addr |= BLMEMCHUNK_ADDR_FREE;
      mBL_MemMgr.mem_avail += chunk->size;
      }


    // insert into busy mem hash bucket
    //  newest chunks at head because more like to free newer chunks
    inline
    void
    EnQBusyMemChunk( BL_MemChunk *chunk )
      {
      int busybuck = BusyAddrHash( chunk->addr );

      if ( (chunk->next = mBL_MemMgr.busy_chunks[busybuck]) )
         chunk->next->prev = chunk;
      mBL_MemMgr.busy_chunks[busybuck] = chunk;
      chunk->prev = NULL;

      mBL_MemMgr.mem_busy += chunk->size;
      mBL_MemMgr.busy_count++;
      }


    // find busy chunk and remove it from it's hash bucket
    inline
    BL_MemChunk *
    DeQBusyMemChunk( uint32_t addr )
      {
      int busybucket = BusyAddrHash( addr );
      BL_MemChunk *t;

      for( t = mBL_MemMgr.busy_chunks[busybucket] ; t ; t = t->next )
        {
        if ( t->addr == addr )
           break;
        }
      if( !t )
        return( NULL );

      if( t->prev )
        {
        t->prev->next = t->next;
        if( t->next )
           t->next->prev = t->prev;
        }
      else // !t->prev
        {
        mBL_MemMgr.busy_chunks[busybucket] = t->next;
        if( t->next )
           t->next->prev = NULL;
        }

      t->next = NULL;
      t->prev = NULL;
      mBL_MemMgr.mem_busy -= t->size;
      mBL_MemMgr.busy_count--;

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
      pool->last_chunk = c;

      // free list the rest of the chunks
      for ( i = 1 ; i < BLMEMCHUNK_POOL_SIZE ; i++ )
        {
        c = &(pool->chunks[i]);
        c->my_pool = pool;
        if ( (c->next = pool->chunk_anchor) )
           c->next->prev = c;
        c->prev = NULL;
        pool->chunk_anchor = c;
        }
      }


    void
    Chunk2FreePool( BL_MemChunk *chunk )
      {
      BL_MemChunkPool *pool = chunk->my_pool;

      if( !pool )
        {
        s0printf("(E) Chunk2FreePool: No pool!\n");
        BLM_CRASH( -4 );
        }

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
           chunk->next->prev = chunk;
        chunk->prev = NULL;
        pool->chunk_anchor = chunk;
        }

      InitQByAddr( chunk );

      pool->used_count--;

      if( !pool->used_count )
        {
        //@MG: Free cannot recurse!
        BLMDB2(("(I) Chunk2FreePool: Freeing Pool at 0x%08x!\n", (uint32_t)pool ));

        if( pool->prev )
          {
          pool->prev->next = pool->next;
          if ( pool->next )
             pool->next->prev = pool->prev;
          }
        else // !pool->prev
          {
          if ( pool->next )
             pool->next->prev = NULL;
          }

        pool->next = mBL_MemMgr.free_pool_later_anchor;
        pool->prev = NULL;
        mBL_MemMgr.free_pool_later_anchor = pool;
        }
      }


    BL_MemChunk *
    GetFreePoolChunk( )
      {
      BL_MemChunk     * c = NULL;
      BL_MemChunkPool * tpool, *new_pool, *first_pool;

      for( first_pool = NULL, tpool = mBL_MemMgr.pool_anchor ; tpool ; tpool = tpool->next )
        {
        if( !first_pool )
          {
          if( tpool->last_chunk )
            first_pool = tpool;
          }
        if( (c = tpool->chunk_anchor) )
          {
          break;
          }
        }

      if( c )
        {
        tpool = c->my_pool;
        if ( (tpool->chunk_anchor = c->next) )
           tpool->chunk_anchor->prev = NULL;
        c->next    = NULL;
        c->prev    = NULL;
        tpool->used_count++;
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
          if( !tpool->next )
            break;
          }
        tpool->next = new_pool;
        new_pool->prev = tpool;

        // grab a chunk from the new chunk pool
        c = new_pool->chunk_anchor;
        if ( (new_pool->chunk_anchor = c->next) )
              new_pool->chunk_anchor->prev = NULL;
        c->next    = NULL;
        c->prev    = NULL;
        new_pool->used_count++;
        }

      return( c );
      }

    void
    FreeChunk( BL_MemChunk * chunk )
      {
      BL_MemChunk *t;

      if( !chunk->addr || !chunk->size )
        {
        BLMDB1(("(E) FreeChunk: chunk->addr = 0x%08x chunk->size = 0x%08x\n",
               chunk->addr, chunk->size ));
        BLM_CRASH( -1 );
        }

      //can we merge this chunk with free neighbors?
      if( (t = chunk->byaddr_prev) )
        {
        if( t->addr & BLMEMCHUNK_ADDR_FREE )
          {
          BLMDB2(("FreeChunk: (prev) merge chunk@0x%08x(%d) with free@0x%08x(%d)\n",
                   chunk->addr, chunk->size, t->addr, t->size ));

          DeQFreeMemChunk( t );
          t->size += chunk->size;
          chunk->addr = 0;
          chunk->size = 0;
          DeQByAddr( chunk );
          Chunk2FreePool( chunk );
          chunk = t;
          }
        }
      if( (t = chunk->byaddr_next) )
        {
        if( t->addr & BLMEMCHUNK_ADDR_FREE )
          {
          BLMDB2(("FreeChunk: (next) merge chunk@0x%08x(%d) with free@0x%08x(%d)\n",
                   chunk->addr, chunk->size, t->addr, t->size ));

          DeQFreeMemChunk( t );
          chunk->size += t->size;
          t->addr = 0;
          t->size = 0;
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
        ch->size        = rbytes;
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

      // put chunk on busy list
      EnQBusyMemChunk( ch );

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

      if ( (bytes == 0) || (mBL_MemMgr.mem_avail < rbytes) )
         return( NULL );

      // get chunk from free chunk pool (if none, fail allocation)
      if ( !(new_chunk = GetFreePoolChunk()) )
         return( NULL );

      return Allocate( rbytes, new_chunk );

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

        BLMDB2(("(I) _blFree De-Recurse: free addr 0x%08x.\n", a ));

        if( !(chunk = DeQBusyMemChunk( a )) )
          {
          s0printf("(E) _blFree De-Recurse: addr 0x%08x not allocated!\n", a );
          BLMDB2_Fcn(( ShowByAddrList()));
          BLM_CRASH(-2);
          }

        FreeChunk( chunk );
        }
      }

    static
    inline
    void
    Mutex( int aLockAction ) // 1 for aquire, 0 for release
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

    void
    Init( void )
      {
      Mutex( 1 );

      BL_MemChunk *pch, *fch;  // chunks for pool and free mem
      uint32_t psz = BL_ROUND_UP_CACHELINE( sizeof(BL_MemChunkPool) );
      uint32_t start = 0;
      size_t   bytes = 0;

      BLMDB2(("(I) _blHeapManager_Init: sizeof(BL_MemMgr) = %d, sizeof(BL_MemChunkPool) = %d, psz = %d, sizeof(BL_MemChunk) = %d.\n",
               sizeof(BL_MemMgr), sizeof(BL_MemChunkPool), psz, sizeof(BL_MemChunk) ));

      memset( &mBL_MemMgr, 0, sizeof(BL_MemMgr) );

      bytes = 10 * 1024 * 1024;  // Try a '10 MB' shareable heap
      start = (uint32_t) malloc( bytes );

      if( !start || !bytes )
        {
        s0printf("(E) _blHeapManager_Init: No Memory!\n");
        BLM_CRASH( -1 );
        }

      // Init all memory words to 0xDEADBEEF
      for( int i = 0; i < (bytes / sizeof(int)); i++ )
        {
        // would prefer to set to a value other than zero - but heap allocs do not fully init.
        ////((int*)start)[ i ] = 0xDEADBEEF;
        //((int*)start)[ i ] = 0;
        }

      BLMDB1(("(I) HeapManager::Init: start = 0x%08x, bytes = %d.\n", start, bytes ));

      mBL_MemMgr.mem_start = BL_ROUND_UP_CACHELINE(   start );
      mBL_MemMgr.mem_size  = BL_ROUND_DOWN_CACHELINE( bytes );
      mBL_MemMgr.mem_avail = 0;
      mBL_MemMgr.mem_busy  = 0;

      // grab the initial chunk pool
      mBL_MemMgr.pool_anchor = (BL_MemChunkPool *)mBL_MemMgr.mem_start;

      ///memset( (void*) mBL_MemMgr.mem_start, 0, sizeof(BL_MemChunkPool) );

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
      mBL_MemMgr.byaddr_anchor = pch;

      fch->addr = (mBL_MemMgr.mem_start + psz);
      fch->size = (mBL_MemMgr.mem_size  - psz);
      fch->addr |= BLMEMCHUNK_ADDR_FREE;

      EnQByAddr( fch, pch );

      // put free mem in free pool
      EnQFreeMemChunk( fch );

      // put pool chunk on busy list
      EnQBusyMemChunk( pch );

      Mutex( 0 );
      }

  };
#endif
