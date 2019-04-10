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
 * File: "apc.c"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <apc_perfctr.h>

#include <apc.h>

#define     APC_SIZE_DEFAULT  (32)
#define     APC_ENABLE_FPU
#define     APC_ENABLE_2NDFPU
#define     APC_ENABLE_DEFAULT
//#define     APC_PRINT_RAW


/*
 * Internal only routines.
 */

extern void ApcInitNode();



/*
 * Macros.
 */

#define EnableCounter(event_num, event_edge) \
  event.num = event_num; \
  event.edge = event_edge; \
  if ( bgl_perfctr_add_event(event) != 0 ) \
  { \
    printf("ApcInit: Error adding counter for event: %d (%s)\n", \
           event.num, BGL_PERFCTR_event_table[event.num].event_name); \
  } 



/*
 * Typedefs.
 */

typedef struct apc_counter_set_s
{
  int active;
  int running;
  
  unsigned long long start_tbr;
  unsigned long long start_counter[BGL_PERFCTR_NUM_COUNTERS];

  unsigned long long tbr;
  unsigned long long counter[BGL_PERFCTR_NUM_COUNTERS];

} apc_counter_set_t;



/*
 * Static storage definitions
 */

static apc_counter_set_t ApcSets[APC_SETS];

static int ApcActive = 0;

static int ApcNode;
static int ApcCore;
static int ApcTwoCoresActive;

static BGL_Barrier *ApcBarrier;





/*
 * void ApcInit()
 */

void ApcInit()
{
  BGLPersonality personality;
  int size, node, proc, other_proc, node_count;
  char *enable_p, *size_p;

  unsigned int rank, num_procs_1, other_rank, num_procs_2 ;

  /*
   * Get processor info, determine if both cores are active, and init barrier if both active
   */

  rts_get_personality(&personality, sizeof(personality));

  node = ((personality.zCoord * personality.ySize  
           + personality.yCoord) * personality.xSize 
          + personality.xCoord);

  ApcNode = node;

  node_count = personality.xSize*personality.ySize*personality.zSize;
 
  proc = rts_get_processor_id();

  ApcCore = proc;

  other_proc = (proc+1)%2;
  
  if ( rts_rankForCoordinates(personality.xCoord, personality.yCoord, personality.zCoord, proc, &rank, &num_procs_1) )
  {
    printf("ApcInit: error getting rank for this processor\n");
  };

  if ( rts_rankForCoordinates(personality.xCoord, personality.yCoord, personality.zCoord, other_proc, &other_rank, &num_procs_2) )
  {
    other_rank = -1;

    ApcTwoCoresActive = 0;
    ApcBarrier = (void *) 0;
  }
  else
  {
    int b;
    void *lock_p;
    
    ApcTwoCoresActive = 1;
    for ( b = BGL_BARRIER_MIN; b < BGL_BARRIER_MAX; b++ )
    {
      if ( rts_alloc_lockbox(b, &lock_p, BGL_LOCKBOX_ORDERED_ALLOC) == 0)
      {
        ApcBarrier = (BGL_Barrier *) lock_p;
        break;
      }
    }
  }
  

  /*
   * If both cores are active, then barrier here to confirm both have set up correctly.
   */

  #if 0
  printf("Node: %d of %d, My Rank/Core: %d/%d, Other Rank/Core: %d/%d, TwoCoresActive: %d\n",
         node, node_count, rank, proc, other_rank, other_proc, ApcTwoCoresActive);
  fflush(stdout);
  #endif
    
  if ( ApcTwoCoresActive )
  {
    BGL_Barrier_Pass(ApcBarrier);
  }
  

  /*
   * Check for enable request from environment
   */

  enable_p = getenv("APC_ENABLE");
  if (  ( enable_p )
        && ( ( strcmp(enable_p, "YES") == 0 )
             || ( strcmp(enable_p, "Yes") == 0 )
             || ( strcmp(enable_p, "yes") == 0 ) ) )
  {
    if ( (ApcNode == 0) && (ApcCore == 0))
      printf("ApcInit: APC_ENABLE=yes, performance counters enabled.\n");
  }
  else
  {
    #if 0
    if ( node == 0 ) 
      printf("ApcInit: APC_ENABLE != \"yes\", disabling performance counters.\n");
    #endif
    return;
  }


  /*
   * At this point user has requested counters to be enabled.
   */

  size = APC_SIZE_DEFAULT;
        
  size_p = getenv("APC_SIZE");

  if ( size_p ) 
  {
    if ( sscanf(size_p, "%d", &size) != 1 )
    {
      size = APC_SIZE_DEFAULT;
      fprintf(stderr, 
              "ApcInit: error reading APC_SIZE value, setting to %d\n", 
              APC_SIZE_DEFAULT);
    }
  }
  
  if ( size < 1 )
  {
    size = APC_SIZE_DEFAULT;
  }
  if ( size > node_count )
  {
    size = node_count;
  }
  
  #if 0
  if ( node == 0 )
    printf("ApcInit: APC_SIZE set to %d (after validation checks)\n", size);
  #endif


  /*
   * Now initialize counters for all nodes upto "size"
   */

  if ( node < size )
  {
    ApcInitNode();  
  }

  return;
}



/*
 * void ApcInitNode()
 */

void ApcInitNode()
{
  int s, c;
  BGL_PERFCTR_event_t event;  /* required by EnableCounter macro */
  


  /*
   * Make sure this is first call.
   */

  if ( ApcActive )
  {
    printf("ApcInitNode: apc counters already active.\n");
    exit(1);
  }


  /*
   * Setup and configure counters.  
   * 
   * Note, bgl_perfctr_init_sync() executes a barrier if both cores are active
   */
  
  {
    int return_val;
    return_val = bgl_perfctr_init_synch(BGL_PERFCTR_MODE_ASYNC);
    if ( return_val != BGL_PERFCTR_MODE_ASYNC )
    {
      fprintf(stderr,
              "ApcInitNode: Error initialising counter structures, return_val=%d\n", 
             return_val);
      exit(1);
    }
  }
  

  /*
   * Setup storage for counter sets.
   */
  
  for ( s = 0; s < APC_SETS; s++ )
  {
    ApcSets[s].active = 0;
    ApcSets[s].start_tbr = 0;
    ApcSets[s].tbr = 0;

    for ( c = 0; c < BGL_PERFCTR_NUM_COUNTERS; c++ )
    {
      ApcSets[s].start_counter[c] = 0;
      ApcSets[s].counter[c] = 0;
    }
  }
  

  /*
   * Install counters.  This only gets done on core 0.  Core 1 waits at barrier if it is active
   */

  if ( ApcCore == 0 )
  {
    switch ( ApcNode % 16 )
  {

  default:
  case 0:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT 
    EnableCounter(BGL_UPC_L3_CACHE_HIT, 0x0);
    EnableCounter(BGL_UPC_L3_CACHE_MISS_DATA_ALRDY_WAY_DDR, 0x0);
    EnableCounter(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0x0);
    EnableCounter(BGL_UPC_L3_EDRAM_ACCESS_CYCLE, 0x0);
    EnableCounter(BGL_UPC_L3_EDRAM_RFR_CYCLE, 0x0);
    EnableCounter(BGL_UPC_L3_LINE_STARTS_EVICT_LINE_NUM_PRESSURE, 0x0);
    EnableCounter(BGL_UPC_L3_MISS_DIR_SET_DISBL, 0x0);
    EnableCounter(BGL_UPC_L3_MISS_NO_WAY_SET_AVAIL, 0x0);
    EnableCounter(BGL_UPC_L3_MISS_REQUIRING_CASTOUT, 0x0);
    EnableCounter(BGL_UPC_L3_MISS_REQUIRING_REFILL_NO_WR_ALLOC, 0x0);
    EnableCounter(BGL_UPC_L3_MSHNDLR_TOOK_REQ, 0x0);
    EnableCounter(BGL_UPC_L3_MSHNDLR_TOOK_REQ_PLB_RDQ, 0x0);
    EnableCounter(BGL_UPC_L3_MSHNDLR_TOOK_REQ_RDQ0, 0x0);
    EnableCounter(BGL_UPC_L3_MSHNDLR_TOOK_REQ_RDQ1, 0x0);
    EnableCounter(BGL_UPC_L3_MSHNDLR_TOOK_REQ_WRBUF, 0x0);
    EnableCounter(BGL_UPC_L3_PAGE_CLOSE, 0x0);
    EnableCounter(BGL_UPC_L3_PAGE_OPEN, 0x0);
    EnableCounter(BGL_UPC_L3_PLB_WRQ_DEP_DBUF, 0x0);
    EnableCounter(BGL_UPC_L3_PLB_WRQ_DEP_DBUF_HIT, 0x0);
    EnableCounter(BGL_UPC_L3_PREF_REINS_PULL_OUT_NEXT_LINE, 0x0);
    EnableCounter(BGL_UPC_L3_PREF_REQ_ACC_BY_PREF_UNIT, 0x0);
    EnableCounter(BGL_UPC_L3_RD_BURST_1024B_LINE_RD, 0x0);
    EnableCounter(BGL_UPC_L3_RD_EDR__ALL_KINDS_OF_RD, 0x0);
    EnableCounter(BGL_UPC_L3_RD_MODIFY_WR_CYCLE_EDR, 0x0);
    EnableCounter(BGL_UPC_L3_REQ_TKN_CACHE_INHIB_RD_REQ, 0x0);
    EnableCounter(BGL_UPC_L3_REQ_TKN_CACHE_INHIB_WR, 0x0);
    EnableCounter(BGL_UPC_L3_REQ_TKN_NEEDS_CASTOUT, 0x0);
    EnableCounter(BGL_UPC_L3_REQ_TKN_NEEDS_REFILL, 0x0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 1:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_PU0_DCURD_SRAM_REQ_PEND, 0x0);

    EnableCounter(BGL_UPC_L3_WRBUF_LINE_ALLOC, 0x0);
    EnableCounter(BGL_UPC_L3_WRQ0_DEP_DBUF, 0x0);
    EnableCounter(BGL_UPC_L3_WRQ0_DEP_DBUF_HIT, 0x0);
    EnableCounter(BGL_UPC_L3_WR_EDRAM__INCLUDING_RMW, 0x0);

    EnableCounter(BGL_UPC_PU0_DCURD_1_RD_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_2_RD_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_3_RD_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_BLIND_REQ, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_COHERENCY_STALL_WAR, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_L3_REQ, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_L3_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_LINK_REQ, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_LINK_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_LOCK_REQ, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_LOCK_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_RD_REQ, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_SRAM_REQ, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_WAIT_L3, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_WAIT_LINK, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_WAIT_LOCK, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_FILTER_HIT, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_PREF_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_REQ_VALID, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_SELF_HIT, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_SNOOP_HIT_OTHER, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_SNOOP_HIT_PLB, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_SNOOP_HIT_SAME, 0x0);
    EnableCounter(BGL_UPC_PU0_PREF_STREAM_HIT, 0x0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 2:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_L3_WRQ1_DEP_DBUF, 0x0);
    EnableCounter(BGL_UPC_L3_WRQ1_DEP_DBUF_HIT, 0x0);

    EnableCounter(BGL_UPC_PU0_DCURD_PLB_REQ, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_PLB_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_WAIT_PLB, 0x0);
    EnableCounter(BGL_UPC_PU0_DCURD_WAIT_SRAM, 0x0);

    EnableCounter(BGL_UPC_PU1_DCURD_SRAM_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_PLB_REQ, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_PLB_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_WAIT_PLB, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_WAIT_SRAM, 0x0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_ARB_CORE_CH2_VC0_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_INJ_VC0_HAVE, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC1_CUT_THROUGH, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 3:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TR_ARB_SNDR0_BUSY, 0);
    EnableCounter(BGL_UPC_PU1_DCURD_LINK_REQ_PEND, 0x0);

    EnableCounter(BGL_UPC_PU1_DCURD_1_RD_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_2_RD_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_3_RD_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_BLIND_REQ, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_COHERENCY_STALL_WAR, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_L3_REQ, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_L3_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_LINK_REQ, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_LOCK_REQ, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_LOCK_REQ_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_RD_REQ, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_SRAM_REQ, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_WAIT_L3, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_WAIT_LINK, 0x0);
    EnableCounter(BGL_UPC_PU1_DCURD_WAIT_LOCK, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_FILTER_HIT, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_PREF_PEND, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_REQ_VALID, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_SELF_HIT, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_SNOOP_HIT_OTHER, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_SNOOP_HIT_PLB, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_SNOOP_HIT_SAME, 0x0);
    EnableCounter(BGL_UPC_PU1_PREF_STREAM_HIT, 0x0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TI_TESTINT_ERR_EVENT, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 4:
    #if ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TS_XP_VCBN_CHUNKS, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TS_XM_32B_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_XM_ACKS, 0);
    EnableCounter(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCBP_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_XM_TOKEN_ACKS, 0);
    EnableCounter(BGL_UPC_TS_XM_VCBN_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_XM_VCBP_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_XM_VCD0_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_XM_VCD1_CHUNKS, 0);

    EnableCounter(BGL_UPC_TS_XP_32B_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_XP_ACKS, 0);
    EnableCounter(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCBP_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
    //EnableCounter(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_XP_TOKEN_ACKS, 0);
    EnableCounter(BGL_UPC_TS_XP_VCBP_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_XP_VCD0_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_XP_VCD1_CHUNKS, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 5:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TR_ARB_CLASS9_WINS, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TS_YM_32B_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YM_ACKS, 0);
    EnableCounter(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCBP_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YM_TOKEN_ACKS, 0);
    EnableCounter(BGL_UPC_TS_YM_VCBN_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YM_VCBP_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YM_VCD0_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YM_VCD1_CHUNKS, 0);

    EnableCounter(BGL_UPC_TS_YP_32B_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YP_ACKS, 0);
    EnableCounter(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCBP_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_YP_TOKEN_ACKS, 0);
    EnableCounter(BGL_UPC_TS_YP_VCBN_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YP_VCBP_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YP_VCD0_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_YP_VCD1_CHUNKS, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 6:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TS_ZP_VCD1_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZM_32B_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZM_ACKS, 0);
    EnableCounter(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCBP_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZM_TOKEN_ACKS, 0);
    EnableCounter(BGL_UPC_TS_ZM_VCBN_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZM_VCBP_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZM_VCD0_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZM_VCD1_CHUNKS, 0);

    EnableCounter(BGL_UPC_TS_ZP_32B_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZP_ACKS, 0);
    EnableCounter(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCBP_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
    EnableCounter(BGL_UPC_TS_ZP_TOKEN_ACKS, 0);
    EnableCounter(BGL_UPC_TS_ZP_VCBN_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZP_VCBP_CHUNKS, 0);
    EnableCounter(BGL_UPC_TS_ZP_VCD0_CHUNKS, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 7:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_ARB_RCV2_BUSY, 0);
    EnableCounter(BGL_UPC_TR_ARB_SNDR1_BUSY, 0);
    EnableCounter(BGL_UPC_TR_ARB_RCV1_BUSY, 0);

    EnableCounter(BGL_UPC_TR_ARB_ALU_BUSY, 0);
    EnableCounter(BGL_UPC_TR_ARB_CH0_VC0_HAVE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CH1_VC0_HAVE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CH1_VC1_HAVE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CH2_VC0_HAVE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CH2_VC1_HAVE, 0);

    EnableCounter(BGL_UPC_TR_ARB_CLASS10_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS11_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS12_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS13_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS14_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS15_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS1_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS3_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS4_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS7_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS8_WINS, 0);

    EnableCounter(BGL_UPC_TR_ARB_RCV0_BUSY, 0);
    EnableCounter(BGL_UPC_TR_ARB_SNDR2_BUSY, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 8:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TR_REC_VC0_PKT_ADDED, 0);
    EnableCounter(BGL_UPC_TR_REC_VC1_HDR_TKN, 0);
    EnableCounter(BGL_UPC_TR_REC_VC1_PYLD_TKN, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_INJ_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_INJ_VC0_HDR_ADDED, 0);
    EnableCounter(BGL_UPC_TR_INJ_VC0_PKT_TKN, 0);
    EnableCounter(BGL_UPC_TR_INJ_VC0_PYLD_ADDED, 0);
    EnableCounter(BGL_UPC_TR_INJ_VC1_HDR_ADDED, 0);
    EnableCounter(BGL_UPC_TR_INJ_VC1_PKT_TKN, 0);
    EnableCounter(BGL_UPC_TR_INJ_VC1_PYLD_ADDED, 0);
    EnableCounter(BGL_UPC_TR_REC_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_REC_VC0_HDR_TKN, 0);
    EnableCounter(BGL_UPC_TR_REC_VC0_PKT_DISC, 0);
    EnableCounter(BGL_UPC_TR_REC_VC0_PYLD_TKN, 0);
    EnableCounter(BGL_UPC_TR_REC_VC1_PKT_ADDED, 0);
    EnableCounter(BGL_UPC_TR_REC_VC1_PKT_DISC, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 9:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TR_RCV_0_UNEXPCT_HDR_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_HDR_PARITY_ERR, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_RCV_0_CRC_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_IDLPKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_KNOWN_BAD_PKT_MARKER, 0);
    //EnableCounter(BGL_UPC_TR_RCV_0_RESYNCH_MODE_AFTER_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC0_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC0_DPKT_RCV, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC0_EMPTY_PKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC0_FULL, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC1_DPKT_RCV, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC1_EMPTY_PKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_0_VC1_FULL, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 10:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_RCV_1_CRC_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_HDR_PARITY_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_IDLPKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_KNOWN_BAD_PKT_MARKER, 0);
    //EnableCounter(BGL_UPC_TR_RCV_1_RESYNCH_MODE_AFTER_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_UNEXPCT_HDR_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC0_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC0_DPKT_RCV, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC0_EMPTY_PKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC0_FULL, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC1_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC1_DPKT_RCV, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC1_EMPTY_PKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_1_VC1_FULL, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 11:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_RCV_2_CRC_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_HDR_PARITY_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_IDLPKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_KNOWN_BAD_PKT_MARKER, 0);
    //EnableCounter(BGL_UPC_TR_RCV_2_RESYNCH_MODE_AFTER_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_UNEXPCT_HDR_ERR, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC0_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC0_DPKT_RCV, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC0_EMPTY_PKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC0_FULL, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC1_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC1_DPKT_RCV, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC1_EMPTY_PKT, 0);
    EnableCounter(BGL_UPC_TR_RCV_2_VC1_FULL, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 12:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    /* problem */ EnableCounter(BGL_UPC_TR_SNDR_2_IDLPKTS_SENT, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_SNDR_2_RESEND_ATTS, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC0_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC0_DPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC0_EMPTY, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC0_PKT_SENT_TOTAL, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC1_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC1_DPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC1_EMPTY, 0);
    EnableCounter(BGL_UPC_TR_SNDR_2_VC1_PKT_SENT_TOTAL, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 13:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_FPU_LDST_DBL_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_MULT_DIV, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_DBL_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TR_SNDR_1_VC1_DPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_VC0_PKT_SENT_TOTAL, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_SNDR_1_IDLPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_RESEND_ATTS, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_VC0_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_VC0_DPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_VC0_EMPTY, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_VC1_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_VC1_EMPTY, 0);
    EnableCounter(BGL_UPC_TR_SNDR_1_VC1_PKT_SENT_TOTAL, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 14:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_LD, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_LD, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TR_SNDR_0_VC1_PKT_SENT_TOTAL, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_SNDR_0_IDLPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_RESEND_ATTS, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_SRAM_ERR_CORR, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_VC0_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_VC0_DPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_VC0_EMPTY, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_VC0_PKT_SENT_TOTAL, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_VC1_CUT_THROUGH, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_VC1_DPKTS_SENT, 0);
    EnableCounter(BGL_UPC_TR_SNDR_0_VC1_EMPTY, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  case 15:
    #ifdef APC_ENABLE_FPU
    EnableCounter(BGL_FPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_FPU_LDST_QUAD_ST, 0x0);
    #endif
    #ifdef APC_ENABLE_2NDFPU
    EnableCounter(BGL_2NDFPU_ARITH_TRINARY_OP, 0x0);
    EnableCounter(BGL_2NDFPU_LDST_QUAD_ST, 0x0);
    #endif

    #ifdef APC_ENABLE_DEFAULT
    EnableCounter(BGL_UPC_TR_ARB_CLASS0_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS6_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CH0_VC1_HAVE, 0);
    EnableCounter(BGL_UPC_TR_ARB_LOCAL_CLIENT_PKT_TKN, 0);

    EnableCounter(BGL_UPC_TR_ARB_LOCAL_CLIENT_ABORT, 0);
    EnableCounter(BGL_UPC_TR_ARB_RCV0_PKT_TKN, 0);

    EnableCounter(BGL_UPC_TS_XM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_XP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_YP_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZM_PKTS, 0);
    EnableCounter(BGL_UPC_TS_ZP_PKTS, 0);

    EnableCounter(BGL_UPC_TR_ARB_CORE_CH0_VC1_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_CH1_VC0_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_GREEDY_MODE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_INJ_VC0_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_REQ_PEND, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_REQ_WAITING_RDY_GO, 0);
    EnableCounter(BGL_UPC_TR_ARB_LOCAL_CLIENT_BUSY_INJ, 0);
    EnableCounter(BGL_UPC_TR_ARB_LOCAL_CLIENT_BUSY_REC, 0);

    EnableCounter(BGL_UPC_TR_ARB_CLASS2_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CLASS5_WINS, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_CH0_VC0_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_CH1_VC1_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_CH2_VC1_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_CORE_INJ_VC1_MATURE, 0);
    EnableCounter(BGL_UPC_TR_ARB_INJ_VC1_HAVE, 0);
    EnableCounter(BGL_UPC_TR_ARB_RCV0_ABORT, 0);
    EnableCounter(BGL_UPC_TR_ARB_RCV1_ABORT, 0);
    EnableCounter(BGL_UPC_TR_ARB_RCV1_PKT_TKN, 0);
    EnableCounter(BGL_UPC_TR_ARB_RCV2_ABORT, 0);
    EnableCounter(BGL_UPC_TR_ARB_RCV2_PKT_TKN, 0);
    #endif

    EnableCounter(BGL_PERFCTR_NULL_EVENT, 0x3);
    break;

  }
  }
  
  if ( bgl_perfctr_commit() != 0 )
  {
    printf("ApcInitNode: Error committing counter selections\n");
    exit(1);
  }

  ApcActive = 1;
  

  /*
   * Make sure to barrier if both cores active.
   */

  if ( ApcTwoCoresActive )
    BGL_Barrier_Pass(ApcBarrier);


  /*
   * Start global counter (Set 0)
   */

  ApcStart(0);
  

  /*
   * All done
   */
  
  return;
}



/*
 * void ApcFinalize()
 */

void ApcFinalize()
{
  int s, c;
  FILE *file_p = (void *)0;
  char name[32];
  bgl_perfctr_control_t hw_state;
  

  /*
   * Skip if we haven't perviously initialized state
   */

  if ( ! ApcActive )
    return;

  /*
   * Stop all active and running counter sets.
   */

  ApcStop(0);

  for ( s = 1; s < APC_SETS; s ++ )
  {
    if ( ApcSets[s].active && ApcSets[s].running )
      ApcStop(s);
  }
  
  #if 0
  UpcFileOpen(ApcNode, ApcCore);
  UpcPrintCounters(0,0);
  UpcFileClose();
  #endif

  
  /*
   * Save data in binary format.
   */

  #if 0
  if ( ApcNode == 0 )
    printf("ApcFinalize: writing out counter data.\n");
  #endif
  
  if ( bgl_perfctr_copy_state(&hw_state, sizeof(hw_state)) )
  {
    printf("ApcFinalize: error getting hw_state\n");
    exit(1);
  }
       
  #ifdef APC_PRINT_RAW
  printf("ApcFinalize: raw counter data:\n");
  #endif

  // make a directory to collect apc files.
  mkdir("apcdata", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

  sprintf(name, "apcdata/apc.%5.5d.%1.1d", ApcNode, ApcCore);
  file_p = fopen(name, "w");
  if ( ! file_p )
  {
    printf("ApcFinalize: error opening file %s for counter output\n", name);
    exit(1);
  }

  for ( s = 0; s < APC_SETS; s++ )
  {
    if (ApcSets[s].active)
    {
      apc_counter_data_t counter_data;
        
      counter_data.value = ApcSets[s].tbr;
      counter_data.node = ApcNode;
      counter_data.id = BGL_PERFCTR_NUMEVENTS; /* tbr not a normal counter, so give number different from upc events */
      counter_data.core = ApcCore;
      counter_data.edge = 0;
      counter_data.set = s;
      counter_data.dummy = 0;

      fwrite (&counter_data, sizeof(counter_data), 1, file_p);

      #ifdef APC_PRINT_RAW
      printf("  %20llu %16llx [%2d %1d %5d] [%3d %d] %s\n",
             counter_data.value,
             counter_data.value,
             counter_data.set,
             counter_data.core,
             counter_data.node,
             counter_data.id,
             counter_data.edge,
             "CYCLE_COUNT");
      #endif

      for ( c = 0; c < hw_state.nmapped; c++ )
      {
        bgl_perfctr_control_map_t *map_p;
        int event_num, event_edge, reg;
        
        map_p = &hw_state.map[c];
        event_num = map_p->event.num;
        event_edge = map_p->event.edge;
        reg = map_p->counter_register;
        
        counter_data.value = ApcSets[s].counter[reg];
        counter_data.node = ApcNode;
        counter_data.id = event_num;
        counter_data.core = ApcCore;
        counter_data.edge = event_edge;
        counter_data.set = s;

        fwrite (&counter_data, sizeof(counter_data), 1, file_p);

        #ifdef APC_PRINT_RAW
        printf("  %20llu %16llx [%2d %1d %5d] [%3d %d] %s\n",
               counter_data.value,
               counter_data.value,
               counter_data.set,
               counter_data.core,
               counter_data.node,
               counter_data.id,
               counter_data.edge,
               BGL_PERFCTR_event_table[counter_data.id].event_name);
        #endif
      }
    }
  }
  fclose(file_p);


  /*
   * Barrier if both cores active.
   */

  if ( ApcTwoCoresActive )
    BGL_Barrier_Pass(ApcBarrier);
  

  /*
   * Shutdown kernel counter control
   */

  if ( bgl_perfctr_shutdown() )
  {
    printf("ApcFinalize: Error disposing counter control\n");
    exit(1);
  };

  ApcActive = 0;

  return;
}



/*
 * void ApcStart(int set)
 */

void ApcStart(int set)
{
  if ( ! ApcActive )
    return;
  
  if ( set < 0 || set >= APC_SETS )
  {
    fprintf(stderr,
            "ApcStart: specified set %d out of bounds\n", 
            set);
    return;
  }
  if ( ApcSets[set].running == 1 )
  {
    fprintf(stderr,
            "ApcStart: set already active\n");
    return;
  }
  
  ApcSets[set].active = 1;
  ApcSets[set].running = 1;
  
  bgl_perfctr_copy_counters(ApcSets[set].start_counter, 
                            sizeof(ApcSets[set].start_counter));
  ApcSets[set].start_tbr = rts_get_timebase();

  return;
}



/*
 * void ApcStop(int set)
 */

void ApcStop(int set)
{
  int c;
  unsigned long long tbr, values[BGL_PERFCTR_NUM_COUNTERS];


  /*
   * Return if nothing to do.
   */

  if ( ! ApcActive )
    return;


  /*
   * Get timebase register and counter values
   */

  tbr = rts_get_timebase();
  bgl_perfctr_copy_counters(values, sizeof(values));


  /*
   * Make sure this was a valid stop request.
   */

  if ( set < 0 || set >= APC_SETS )
  {
    fprintf(stderr, "ApcStop: specified set %d out of bounds\n", set);
    return;
  }
  if ( (! ApcSets[set].active) 
       || (! ApcSets[set].running) )
  {
    fprintf(stderr, "ApcStop: counter set %d was not active or running.\n", set);
    return;
  }


  /*
   * Update the set counter data.
   */

  ApcSets[set].tbr += (tbr - ApcSets[set].start_tbr);

  for ( c = 0; c < BGL_PERFCTR_NUM_COUNTERS; c++ )
    ApcSets[set].counter[c] += (values[c]-ApcSets[set].start_counter[c]);

  ApcSets[set].running = 0;

  return;
}



/*
 * double ApcClockTicks(int set)
 */

double ApcClockTicks(int set)
{
  if ( ! ApcActive )
    return 0.0;

  
  if ( ( set < 0 ) || ( set > APC_SETS ) )
  {
    fprintf(stderr, "ApcClockTicks: set %d is not a valid set number, zero clock tick count returned to caller.\n", set);
    fflush(stderr);
    return 0.0;
  }
  if ( ! ApcSets[set].active )
  {
    fprintf(stderr, "ApcClockTicks: set %d is not active, zero clock tick count returned to caller.\n", set);
    fflush(stderr);
    return 0.0;
  }
  if ( ApcSets[set].running )
  {
    fprintf(stderr, "ApcClockTicks: set %d is running, zero clock tick count returned to caller.\n", set);
    fflush(stderr);
    return 0.0;
  }

  return (double) ApcSets[set].tbr;
}


/*
 * APC Fortran Interfaces
 */

void apc_init()
{
  ApcInit();
}

void apc_finalize()
{
  ApcFinalize();
}


void apc_start(int *set_p)
{
  ApcStart(*set_p);
}

void apc_stop(int *set_p)
{
  ApcStop(*set_p);
}
