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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define KILO (1024)
#define K_DATA /**/
#define ALIGN_L3_CACHE /**/
#define s0printf printf

#define CONFIG_MALLOC_SUPPORT 1

#if defined( CONFIG_MALLOC_SUPPORT )

#if 1

static inline int
hard_processor_id()
  {
  return( 0 );
  }

static inline int cntlz( uint32_t v )
{
   int i;
   uint32_t z;

   for ( i = 0, z = 0x80000000 ; i < 32 ; i++, z >>= 1 )
      if ( v & z )
         return(i);

   return(32);
}
#endif

#define _BL_MALLOC_DEBUG (0)
//#define _BL_MALLOC_DEBUG (1)
//#define _BL_MALLOC_DEBUG (2)
//#define _BL_MALLOC_DEBUG (3) // you'll be sorry

#if _BL_MALLOC_DEBUG > 0
#define _BLMDB1( Args )    do { TRC_MSG_FCN Args ; } while(0)
#define _BLMDB1_Fcn( Fcn ) do { Fcn ; } while(0)
#else
#define _BLMDB1( Args )    do { /* nothing */ } while(0)
#define _BLMDB1_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

#if _BL_MALLOC_DEBUG > 1
#define _BLMDB2( Args )    do { TRC_MSG_FCN Args ; } while(0)
#define _BLMDB2_Fcn( Fcn ) do { Fcn ; } while(0)
#else
#define _BLMDB2( Args )    do { /* nothing */ } while(0)
#define _BLMDB2_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

#if _BL_MALLOC_DEBUG > 2
#define _BLMDB3( Args )    do { TRC_MSG_FCN Args ; } while(0)
#define _BLMDB3_Fcn( Fcn ) do { Fcn ; } while(0)
#else
#define _BLMDB3( Args )    do { /* nothing */ } while(0)
#define _BLMDB3_Fcn( Fcn ) do { /* nothing */ } while(0)
#endif

//#define _BLM_CRASH(rc) Sim_Terminate( (rc) )
#define _BLM_CRASH(rc) (printf("crash\n"),exit(-1));


#define _BL_ROUND_UP_CACHELINE(v) (((v) + 0x1F) & ~0x1F)

#define _BL_ROUND_DOWN_CACHELINE(v) ((v) & ~0x1F)

#define _BLMEMCHUNK_POOL_SIZE (1UL * KILO)

// This malloc manages cachelines (32 bytes), so
//  lower 5 bits of addr are available for flags
#define _BLMEMCHUNK_ADDR_MASK (0xFFFFFFE0)
#define _BLMEMCHUNK_ADDR_FREE (0x00000001)


// MemChunk: track free/busy memory (@MG: rework for smaller size)
//  MemChunks can be in 3 states:
//   1) in the free_pool
//   2) in the free_chunks list organized by log2 of size. sorted by size within the list
//   3) busy_chunks, hashed by address for quick retrieval
//  Allocations smaller than a chunk split the chunk into 2 chunks
//  Frees of chunks attempt to coalesce chunks with free neighboring chunks
//  So, all chunks have to be simultaneously free/busy and sorted by address
typedef struct T_BL_MemChunk
                {
                struct T_BL_MemChunk *next,
                                     *prev,
                                     *byaddr_next,
                                     *byaddr_prev;
                uint32_t addr,
                         size;
                struct T_BL_MemChunkPool *my_pool;
                }
                _BL_MemChunk;

// Note: The first pool allocated can never be freed because
//       2 of it's chunks are always used: one for free memory,
//       and the second for itself.
typedef struct T_BL_MemChunkPool
                {
                struct T_BL_MemChunkPool *next,
                                         *prev;
                int used_count;
                _BL_MemChunk *last_chunk;   // emergency use to alloc new pool
                _BL_MemChunk *chunk_anchor;
                _BL_MemChunk chunks[ _BLMEMCHUNK_POOL_SIZE ];
                }
                _BL_MemChunkPool;

typedef struct T_BL_MemMgr
                {
                uint32_t mem_start,
                         mem_size,
                         mem_avail,
                         mem_busy;
                uint32_t         busy_count;
                _BL_MemChunk     *byaddr_anchor;
                _BL_MemChunk     *free_chunks[  32 ]; // arranged by log2 of size
                _BL_MemChunk     *busy_chunks[ 256 ]; // open-hashed on address
                _BL_MemChunkPool *pool_anchor;
                _BL_MemChunkPool *free_pool_later_anchor; // de-recurse free
                }
                _BL_MemMgr;


static _BL_MemMgr K_DATA _blMemMgrC0 ALIGN_L3_CACHE = { 0, };

static _BL_MemMgr K_DATA _blMemMgrC1 ALIGN_L3_CACHE = { 0, };

// forward decls
static void *_bl_MemMgr_Malloc( _BL_MemMgr *mgr, uint32_t rbytes, _BL_MemChunk *new_chunk );

//// extern void _blade_vmm_sbrk_Claim( uint32_t *vstart, size_t *mem_bytes );
void _blade_vmm_sbrk_Claim( uint32_t *vstart, size_t *mem_bytes )
  {
  * mem_bytes = 500 * 1024 * 1024;
  * vstart    = (uint32_t) malloc( * mem_bytes );
  }

static inline  void _bl_MemMgr_InitQByAddr( _BL_MemChunk *chunk )
{
   chunk->byaddr_next = NULL;
   chunk->byaddr_prev = NULL;
}

static inline void _bl_MemMgr_EnQByAddr( _BL_MemMgr *mgr, _BL_MemChunk *n, _BL_MemChunk *o )
{
   _BLMDB3(("EnQ: new->addr = 0x%08x, old->addr = 0x%08x\n", n->addr, o->addr ));

   if ( n->addr < o->addr )
      {
      s0printf("(E) _bl_MemMgr Internal Error: _EnQByAddr: new < old\n");
      _BLM_CRASH( 3 );
      }

   if ( (n->byaddr_next = o->byaddr_next) )
      n->byaddr_next->byaddr_prev = n;
   o->byaddr_next = n;
   n->byaddr_prev = o;
}

static inline void _bl_MemMgr_DeQByAddr( _BL_MemMgr *mgr, _BL_MemChunk *c )
{
   if ( c->byaddr_prev )
      c->byaddr_prev->byaddr_next = c->byaddr_next;
   else
      mgr->byaddr_anchor = c->byaddr_next;

   if ( c->byaddr_next )
      c->byaddr_next->byaddr_prev = c->byaddr_prev;

   c->byaddr_next = NULL;
   c->byaddr_prev = NULL;
}


static inline int _bl_MemMgr_BusyAddrHash( uint32_t v )
{
   int a,b;

   // Large allocations given equal weight as small allocations
   a = ((v & 0x03FC0000) >> 18); // big ones
   b = ((v & 0x00007F80) >>  7); // small ones

   _BLMDB3(("_bl_MemMgr_BusyAddrHash 0x%08x: a = %d, b = %d, a+b = %d\n", v, a, b, (a+b) ));

   return( (a + b) & 0xFF );
}

static inline int _bl_MemMgr_GetBucket( uint32_t v )
{
   int b = cntlz(v);

   _BLMDB3(("cntlz(0x%08x) = %d\n", v, cntlz(v) ));

   if ( b == 0 )
      return( 31 );
   else if ( b > 27 )
      return( 5 );
   else
      return( 32 - b );
}

#if _BL_MALLOC_DEBUG > 1
void _showFreePool( _BL_MemMgr *mgr )
{
   int i, j;
   _BL_MemChunkPool *pool = mgr->pool_anchor;
   _BL_MemChunk *ch;

   if ( !pool )
      {
      s0printf("(E) _showFreePool: No pool!\n");
      return;
      }

   for ( j = 0 ; pool ; pool = pool->next , j++ )
      {
      s0printf("(I) Free Pool(%d):\n", j );

      for ( i = 0, ch = pool->chunk_anchor ; ch ; ch = ch->next, i++ )
         s0printf("  %2d.%4d: 0x%08x\n", j, i, (uint32_t)ch );
      }
}

void _showFreeChunks( _BL_MemMgr *mgr )
{
   int i, cnt;
   _BL_MemChunk *ch;

   for ( i = 0 ; i < 32 ; i++ )
      {
      cnt = 0;
      s0printf("Free Chunks[%2d]:", i );
      for ( ch = mgr->free_chunks[i] ; ch ; ch = ch->next )
         {
         cnt++;
         s0printf(" 0x%08x@0x%08x", ch->size, ch->addr );
         }
      s0printf(". (%d)\n", cnt );
      }
}

void _showBusyChunks( _BL_MemMgr *mgr )
{
   int i, j;
   _BL_MemChunk *ch;

   for ( i = 0 ; i < 256 ; i++ )
      {
      s0printf("Busy Chunks[%2d]: ", i );
      for ( j = 0, ch = mgr->busy_chunks[i] ; ch ; ch = ch->next, j++ )
          s0printf(" 0x%08x@0x%08x", ch->size, ch->addr );
      s0printf(" (Total = %d).\n", j );
      }
}

void _showByAddrList( _BL_MemMgr *mgr )
{
   int i;
   uint32_t a = 0;
   _BL_MemChunk *ch = mgr->byaddr_anchor;


   s0printf("_showByAddrList: start\n");
   for ( i = 0 ; ch ; ch = ch->byaddr_next, i++ )
      {
      s0printf("[%3d]: 0x%08x@0x%08x %s\n", i, ch->size, ch->addr,
             ((ch->addr > a) ? "Ok" : "ERROR") );
      a = ch->addr;
      }
   s0printf("_showByAddrList: done.\n");

}

void _showStats( _BL_MemMgr *mgr )
{
  s0printf("_showStats: mem_start = 0x%08x, mem_size = 0x%08x\n",
         mgr->mem_start, mgr->mem_size );
  s0printf("_showStats: mem_avail = 0x%08x, mem_busy = 0x%08x, busy_count = %d.\n",
         mgr->mem_avail, mgr->mem_busy, mgr->busy_count );

  _showFreeChunks( mgr );

  _showByAddrList( mgr );
}

void _showBusy( _BL_MemMgr *mgr )
{
  s0printf("_showStats: mem_avail = %u, mem_busy = %u.\n",
         mgr->mem_avail, mgr->mem_busy );
}
#endif // _BL_MALLOC_DEBUG > 0


// remove specific chunk from it's bucket
static inline void _bl_MemMgr_DeQFreeMemChunk( _BL_MemMgr *mgr, _BL_MemChunk *chunk )
{
   int b = _bl_MemMgr_GetBucket( chunk->size );

   if ( chunk->prev )
      chunk->prev->next = chunk->next;
   else
      mgr->free_chunks[b] = chunk->next;

   if ( chunk->next )
      chunk->next->prev = chunk->prev;

   chunk->next = NULL;
   chunk->prev = NULL;
   mgr->mem_avail -= chunk->size;
}


// insert into appropriate bucket sorted by size
static void _bl_MemMgr_EnQFreeMemChunk( _BL_MemMgr *mgr, _BL_MemChunk *chunk )
{
   int b = _bl_MemMgr_GetBucket( chunk->size );
   _BL_MemChunk *t, *tn;

   _BLMDB1_Fcn( if ( !chunk->addr || !chunk->size )
                   {
                   s0printf("(E) _bl_MemMgr_EnQFreeMemChunk: chunk->addr = 0x%08x chunk->size = 0x%08x\n",
                            chunk->addr, chunk->size );
                   _BLM_CRASH( -1 );
                   }
               if ( chunk->next || chunk->prev )
                  {
                  s0printf("(E) _bl_MemMgr_EnQFreeMemChunk: chunk->next = 0x%08x chunk->prev = 0x%08x\n",
                           (uint32_t)chunk->next, (uint32_t)chunk->prev );
                  _BLM_CRASH( -1 );
                  } );

   if ( (t = mgr->free_chunks[b]) )
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
      mgr->free_chunks[b] = chunk;
      chunk->next = NULL;
      chunk->prev = NULL;
      }

   chunk->addr |= _BLMEMCHUNK_ADDR_FREE;
   mgr->mem_avail += chunk->size;
}


// insert into busy mem hash bucket
//  newest chunks at head because more like to free newer chunks
static inline void _bl_MemMgr_EnQBusyMemChunk( _BL_MemMgr *mgr, _BL_MemChunk *chunk )
{
   int busybuck = _bl_MemMgr_BusyAddrHash( chunk->addr );

   if ( (chunk->next = mgr->busy_chunks[busybuck]) )
      chunk->next->prev = chunk;
   mgr->busy_chunks[busybuck] = chunk;
   chunk->prev = NULL;

   mgr->mem_busy += chunk->size;
   mgr->busy_count++;
}


// find busy chunk and remove it from it's hash bucket
static inline _BL_MemChunk *_bl_MemMgr_DeQBusyMemChunk( _BL_MemMgr *mgr, uint32_t addr )
{
   int busybucket = _bl_MemMgr_BusyAddrHash( addr );
   _BL_MemChunk *t;

   for ( t = mgr->busy_chunks[busybucket] ; t ; t = t->next )
      {
      if ( t->addr == addr )
         break;
      }
   if ( !t )
      return( NULL );

   if ( t->prev )
      {
      t->prev->next = t->next;
      if ( t->next )
         t->next->prev = t->prev;
      }
   else // !t->prev
      {
      mgr->busy_chunks[busybucket] = t->next;
      if ( t->next )
         t->next->prev = NULL;
      }

   t->next = NULL;
   t->prev = NULL;
   mgr->mem_busy -= t->size;
   mgr->busy_count--;

   return( t );
}


static void _bl_MemMgr_ChunkFreePoolInit( _BL_MemChunkPool *pool )
{
   int i;
   _BL_MemChunk *c;

   memset( pool, 0, sizeof(_BL_MemChunkPool) );

   // save 1 chunk for emergency use (allocation of new pools)
   c = &(pool->chunks[0]);
   c->my_pool = pool;
   pool->last_chunk = c;

   // free list the rest of the chunks
   for ( i = 1 ; i < _BLMEMCHUNK_POOL_SIZE ; i++ )
      {
      c = &(pool->chunks[i]);
      c->my_pool = pool;
      if ( (c->next = pool->chunk_anchor) )
         c->next->prev = c;
      c->prev = NULL;
      pool->chunk_anchor = c;
      }
}


static void _bl_MemMgr_Chunk2FreePool( _BL_MemMgr *mgr, _BL_MemChunk *chunk )
{
   _BL_MemChunkPool *pool = chunk->my_pool;

   if ( !pool )
      {
      s0printf("(E) _bl_MemMgr_Chunk2FreePool: No pool!\n");
      _BLM_CRASH( -4 );
      }

   if ( chunk->size )
      s0printf("(E) _bl_MemMgr_Chunk2FreePool: chunk not empty! size = %d\n", chunk->size );

   if ( !(pool->last_chunk) )
      {
      pool->last_chunk = chunk;
      chunk->next = NULL;
      chunk->prev = NULL;
      }
   else
      {
      if ( (chunk->next = pool->chunk_anchor) )
         chunk->next->prev = chunk;
      chunk->prev = NULL;
      pool->chunk_anchor = chunk;
      }

   _bl_MemMgr_InitQByAddr( chunk );

   pool->used_count--;

   if ( !pool->used_count )
      {
      //@MG: Free cannot recurse!
      _BLMDB2(("(I) _bl_MemMgr_Chunk2FreePool: Freeing Pool at 0x%08x!\n", (uint32_t)pool ));

      if ( pool->prev )
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

      pool->next = mgr->free_pool_later_anchor;
      pool->prev = NULL;
      mgr->free_pool_later_anchor = pool;
      }
}


static _BL_MemChunk *_bl_MemMgr_GetFreePoolChunk( _BL_MemMgr *mgr )
{
   _BL_MemChunk *c = NULL;
   _BL_MemChunkPool *tpool, *new_pool, *first_pool;

   for ( first_pool = NULL, tpool = mgr->pool_anchor ; tpool ; tpool = tpool->next )
      {
      if ( !first_pool )
         if ( tpool->last_chunk )
            first_pool = tpool;
      if ( (c = tpool->chunk_anchor) )
         break;
      }

   if ( c )
      {
      tpool = c->my_pool;
      if ( (tpool->chunk_anchor = c->next) )
         tpool->chunk_anchor->prev = NULL;
      c->next    = NULL;
      c->prev    = NULL;
      tpool->used_count++;
      return( c );
      }

   if ( !first_pool )
      {
      _BLMDB1(("_bl_MemMgr_GetFreePoolChunk: No pool with last_chunk!\n"));
      return( NULL );
      }
   else
      {
      _BLMDB2(("_bl_MemMgr_GetFreePoolChunk: Growing Free Pool!\n"));
      if ( !(c = first_pool->last_chunk) )
         return( NULL );
      first_pool->last_chunk = NULL;
      first_pool->used_count++;

      // note: if alloc fails, last_chunk is restored
      new_pool = _bl_MemMgr_Malloc( mgr,
                                    _BL_ROUND_UP_CACHELINE( sizeof(_BL_MemChunkPool) ),
                                    c );
      if ( !new_pool )
         {
         _BLMDB1(("_bl_MemMgr_GetFreePoolChunk: malloc fails for new pool!\n"));
         return( NULL );
         }

      _bl_MemMgr_ChunkFreePoolInit( new_pool );

      // new pool has to go on end of list so it's chunks are used last
      for ( tpool = mgr->pool_anchor ; tpool ; tpool = tpool->next )
         if ( !tpool->next )
            break;
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

static void _bl_MemMgr_FreeChunk( _BL_MemMgr *mgr, _BL_MemChunk *chunk )
{
   _BL_MemChunk *t;

   if ( !chunk->addr || !chunk->size )
      {
      _BLMDB1(("(E) _bl_MemMgr_FreeChunk: chunk->addr = 0x%08x chunk->size = 0x%08x\n",
             chunk->addr, chunk->size ));
      _BLM_CRASH( -1 );
      }

   // can we merge this chunk with free neighbors?
   if ( (t = chunk->byaddr_prev) )
      {
      if ( t->addr & _BLMEMCHUNK_ADDR_FREE )
         {
         _BLMDB2(("_bl_MemMgr_FreeChunk: (prev) merge chunk@0x%08x(%d) with free@0x%08x(%d)\n",
                  chunk->addr, chunk->size, t->addr, t->size ));

         _bl_MemMgr_DeQFreeMemChunk( mgr, t );
         t->size += chunk->size;
         chunk->addr = 0;
         chunk->size = 0;
         _bl_MemMgr_DeQByAddr( mgr, chunk );
         _bl_MemMgr_Chunk2FreePool( mgr, chunk );
         chunk = t;
         }
      }
   if ( (t = chunk->byaddr_next) )
      {
      if ( t->addr & _BLMEMCHUNK_ADDR_FREE )
         {
         _BLMDB2(("_bl_MemMgr_FreeChunk: (next) merge chunk@0x%08x(%d) with free@0x%08x(%d)\n",
                  chunk->addr, chunk->size, t->addr, t->size ));

         _bl_MemMgr_DeQFreeMemChunk( mgr, t );
         chunk->size += t->size;
         t->addr = 0;
         t->size = 0;
         _bl_MemMgr_DeQByAddr( mgr, t );
         _bl_MemMgr_Chunk2FreePool( mgr, t );
         }
      }

   _bl_MemMgr_EnQFreeMemChunk( mgr, chunk );
}


// low-level malloc: rbytes pre-checked and new_chunk pre-allocated
static void *_bl_MemMgr_Malloc( _BL_MemMgr *mgr, uint32_t rbytes, _BL_MemChunk *new_chunk )
{
   _BL_MemChunk *ch;
   int need_buck = _bl_MemMgr_GetBucket( rbytes );
   int have_buck;

   // find smallest available chunk in smallest bucket that fits this allocation
   for ( have_buck = need_buck ; have_buck < 32 ; have_buck++ )
      {
      if ( mgr->free_chunks[have_buck] )
         break;
      }
   if ( have_buck >= 32 )
      {
      _bl_MemMgr_Chunk2FreePool( mgr, new_chunk );
      return(NULL);
      }

   // search for smallest chunk in this bucket that fits this allocation
   for ( ch = mgr->free_chunks[have_buck] ; ch ; ch = ch->next )
      {
      if ( ch->size >= rbytes )
         break;
      }
   if ( ! ch )
      {
      _bl_MemMgr_Chunk2FreePool( mgr, new_chunk );
      return( NULL );
      }

   // remove chunk from free mem bucket
   _bl_MemMgr_DeQFreeMemChunk( mgr, ch );

   // do we have to split the chunk?
   if ( ch->size > rbytes )
      {
      // split this chunk into ch (the allocation) and new_chunk (the left-over)
      new_chunk->addr = ch->addr + rbytes;
      new_chunk->size = ch->size - rbytes;
      ch->size        = rbytes;
      _bl_MemMgr_EnQByAddr( mgr, new_chunk, ch );

      // return left-over in new_chunk to free pool
      _bl_MemMgr_EnQFreeMemChunk( mgr, new_chunk );
      }
   else
      {
      // we didn't need new_chunk afterall, so free it
      _bl_MemMgr_Chunk2FreePool( mgr, new_chunk );
      }

   // clear the free flag
   ch->addr &= _BLMEMCHUNK_ADDR_MASK;

   // put chunk on busy list
   _bl_MemMgr_EnQBusyMemChunk( mgr, ch );

   return( (void *)ch->addr );
}

//
// Public routines: malloc, calloc, free, realloc, and _blade_Malloc_Init.
//

void *bgl_malloc( size_t bytes )
{
   _BL_MemMgr *mgr;
   _BL_MemChunk *new_chunk;
   uint32_t rbytes = _BL_ROUND_UP_CACHELINE( bytes );

   if ( hard_processor_id() )
      mgr = &_blMemMgrC1;
   else
      mgr = &_blMemMgrC0;

   _BLMDB2(("_blMalloc: 0x%08x bytes = 0x%08x rbytes.\n", bytes, rbytes ));

   if ( (bytes == 0) || (mgr->mem_avail < rbytes) )
      return( NULL );

   // get chunk from free chunk pool (if none, fail allocation)
   if ( !(new_chunk = _bl_MemMgr_GetFreePoolChunk(mgr)) )
      return( NULL );

   return _bl_MemMgr_Malloc( mgr, rbytes, new_chunk );
}


void *bgl_calloc( size_t nmemb, size_t size )
{
   uint32_t bytes = (nmemb * size);
   void *mem = malloc( bytes );

   if ( mem )
      memset( mem, 0, bytes );

   return( mem );
}


void bgl_free( void *addr )
{
   _BL_MemMgr *mgr;
   _BL_MemChunk *chunk;
   uint32_t a = (uint32_t)addr;

   if ( hard_processor_id() )
      mgr = &_blMemMgrC1;
   else
      mgr = &_blMemMgrC0;

   _BLMDB2(("(I) _blFree: free addr 0x%08x.\n", a ));

   if ( !(chunk = _bl_MemMgr_DeQBusyMemChunk( mgr, a )) )
      {
      s0printf("(E) _blFree: addr 0x%08x not allocated!\n", a );
      _BLMDB2_Fcn(( _showByAddrList( mgr ) ));
      _BLM_CRASH( -2 );
      }

   _bl_MemMgr_FreeChunk( mgr, chunk );

   while ( mgr->free_pool_later_anchor )
      {
      a = (uint32_t)mgr->free_pool_later_anchor;
      mgr->free_pool_later_anchor = mgr->free_pool_later_anchor->next;

      _BLMDB2(("(I) _blFree De-Recurse: free addr 0x%08x.\n", a ));

      if ( !(chunk = _bl_MemMgr_DeQBusyMemChunk( mgr, a )) )
         {
         s0printf("(E) _blFree De-Recurse: addr 0x%08x not allocated!\n", a );
         _BLMDB2_Fcn(( _showByAddrList( mgr )));
         _BLM_CRASH(-2);
         }

      _bl_MemMgr_FreeChunk( mgr, chunk );
      }
}


void *bgl_realloc( void *addr, size_t size )
{
   _BL_MemMgr *mgr;
   uint32_t a = (uint32_t)addr;
   int busybucket = _bl_MemMgr_BusyAddrHash( a );
   uint32_t new_size = _BL_ROUND_UP_CACHELINE( size );
   _BL_MemChunk *t, *new_chunk;

   if ( hard_processor_id() )
      mgr = &_blMemMgrC1;
   else
      mgr = &_blMemMgrC0;

   // realloc to zero == free()
   if ( !new_size )
      {
      free( addr );
      return( NULL );
      }

   // realloc( NULL, ... ) == malloc( ... )
   if ( !a )
      return malloc( size );

   // find the old allocation
   for ( t = mgr->busy_chunks[busybucket] ; t ; t = t->next )
      {
      if ( t->addr == a )
         break;
      }
   if ( !t )
      {
      s0printf("(E) _blRealloc: addr 0x%08x not allocated!\n", a );
      _BLMDB2_Fcn(( _showByAddrList( mgr )));
      return( NULL );
      }

   if ( new_size == t->size )
      {
      return( addr );
      }

   // to grow or shrink we need a chunk
   if ( !(new_chunk = _bl_MemMgr_GetFreePoolChunk(mgr)) )
      return( NULL );

   if ( new_size < t->size )
      {
      // shrink the allocation by splitting the chunk
      uint32_t diff = (t->size - new_size);
      t->size -= diff;
      mgr->mem_busy -= diff;
      new_chunk->addr = (t->addr + diff);
      new_chunk->size = diff;
      new_chunk->addr |= _BLMEMCHUNK_ADDR_FREE;

      // hook in new chunk
      _bl_MemMgr_EnQByAddr( mgr, new_chunk, t );

      // free the left-over (old chunk stays busy, just got smaller)
      _bl_MemMgr_FreeChunk( mgr, new_chunk );

      return( (void *)t->addr );
      }
   else
      {
      // grow the allocation (Malloc frees chunk if failure)
      void *new_mem = _bl_MemMgr_Malloc( mgr, new_size, new_chunk );

      if ( !new_mem )
         return( NULL );

      memcpy( new_mem, addr, t->size );

      free( addr );

      return( new_mem );
      }

  // not reached
}


void _blade_Malloc_Init( void )
{
   _BL_MemMgr *mgr;
   _BL_MemChunk *pch, *fch;  // chunks for pool and free mem
   uint32_t psz = _BL_ROUND_UP_CACHELINE( sizeof(_BL_MemChunkPool) );
   uint32_t start = 0;
   size_t   bytes = 0;

   if ( hard_processor_id() )
      mgr = &_blMemMgrC1;
   else
      mgr = &_blMemMgrC0;

   _BLMDB2(("(I) _blMalloc_Init: sizeof(_BL_MemMgr) = %d, sizeof(_BL_MemChunkPool) = %d, psz = %d, sizeof(_BL_MemChunk) = %d.\n",
            sizeof(_BL_MemMgr), sizeof(_BL_MemChunkPool), psz, sizeof(_BL_MemChunk) ));

   memset( mgr, 0, sizeof(_BL_MemMgr) );

   _blade_vmm_sbrk_Claim( &start, &bytes );
   if ( !start || !bytes )
      {
      s0printf("(E) _blMalloc_Init: No Memory!\n");
      _BLM_CRASH( -1 );
      }

   _BLMDB1(("(I) _blMalloc_Init[%d]: start = 0x%08x, bytes = %d.\n",
             hard_processor_id(), start, bytes ));

   mgr->mem_start = _BL_ROUND_UP_CACHELINE(   start );
   mgr->mem_size  = _BL_ROUND_DOWN_CACHELINE( bytes );
   mgr->mem_avail = 0;
   mgr->mem_busy  = 0;

   // grab the initial chunk pool
   mgr->pool_anchor = (_BL_MemChunkPool *)mgr->mem_start;
   _bl_MemMgr_ChunkFreePoolInit( mgr->pool_anchor );

   // create the initial pool chunk and free chunk
   pch = _bl_MemMgr_GetFreePoolChunk(mgr);
   fch = _bl_MemMgr_GetFreePoolChunk(mgr);
   if ( !pch || !fch )
      {
      s0printf("(E) _blMalloc_Init: Could not get initial free & pool chunks!\n");
      _BLM_CRASH( -6 );
      }
   pch->addr = mgr->mem_start;
   pch->size = psz;
   mgr->byaddr_anchor = pch;

   fch->addr = (mgr->mem_start + psz);
   fch->size = (mgr->mem_size  - psz);
   fch->addr |= _BLMEMCHUNK_ADDR_FREE;

   _bl_MemMgr_EnQByAddr( mgr, fch, pch );

   // put free mem in free pool
   _bl_MemMgr_EnQFreeMemChunk( mgr, fch );

   // put pool chunk on busy list
   _bl_MemMgr_EnQBusyMemChunk( mgr, pch );
}

#else // not CONFIG_MALLOC_SUPPORT

void *malloc( size_t bytes ) { return( (void *)0); }

void *calloc( size_t nmemb, size_t size ) { return( (void *)0); }

void free( void *addr ) { return; }

void *realloc( void *addr, size_t size ) { return( (void *)0); }

void _blade_Malloc_Init( void ) { return; }

#endif // CONFIG_MALLOC_SUPPORT

void
main( int argc, char ** argv, char** envp )
  {
  _blade_Malloc_Init( );
  char * m = bgl_malloc( 10 );
  bgl_free( m );
  return;
  }
