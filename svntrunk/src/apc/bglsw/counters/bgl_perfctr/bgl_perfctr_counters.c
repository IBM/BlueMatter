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
 /* -------------------------------------------------------------- */
/* Product(s):                                                    */
/* 5733-BG1                                                       */
/*                                                                */
/* (C)Copyright IBM Corp. 2004, 2006                              */
/* All rights reserved.                                           */
/* US Government Users Restricted Rights -                        */
/* Use, duplication or disclosure restricted                      */
/* by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                */
/* Licensed Materials-Property of IBM                             */
/* -------------------------------------------------------------- */

#include "apc_perfctr.h"
#include "bgl_perfctr_define.h"
#include "bglpersonality.h"

extern bgl_perfctr_control_t *CNTR_STATE;

int bgl_perfctr_add_remote_counters(void) {

  int retval = 0;       /* Needed by macro bgl_perfctr_add_remote_counters_ENABLE_COUNTER  */
  unsigned long index, index2;
  BGL_PERFCTR_event_t event;

  index = CNTR_STATE->defn_id;
  switch(index) {
    BGLPersonality BGLP;

    case 16:
    case 1004:
        rts_get_personality(&BGLP, sizeof(BGLP));
        index2 = ((BGLP.xCoord*BGLP.ySize*BGLP.zSize)+(BGLP.yCoord*BGLP.zSize)+BGLP.zCoord)%4;
        break;

    default:
        index2 = 4;         /* Set to 4 so we fail below if we ever try and use it for   */
                            /* a 'wrong' counter definition id...                        */
  }

  switch(index) {

    case 0:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_ALRDY_WAY_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_EDRAM_ACCESS_CYCLE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_EDRAM_RFR_CYCLE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_LINE_STARTS_EVICT_LINE_NUM_PRESSURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_DIR_SET_DISBL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_NO_WAY_SET_AVAIL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_REQUIRING_CASTOUT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_REQUIRING_REFILL_NO_WR_ALLOC, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_PLB_RDQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_RDQ0, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_RDQ1, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_WRBUF, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PAGE_CLOSE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PAGE_OPEN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PLB_WRQ_DEP_DBUF, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PLB_WRQ_DEP_DBUF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PREF_REINS_PULL_OUT_NEXT_LINE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PREF_REQ_ACC_BY_PREF_UNIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_RD_BURST_1024B_LINE_RD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_RD_EDR__ALL_KINDS_OF_RD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_RD_MODIFY_WR_CYCLE_EDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_CACHE_INHIB_RD_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_CACHE_INHIB_WR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_NEEDS_CASTOUT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_NEEDS_REFILL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 1:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_WRBUF_LINE_ALLOC, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_WRQ0_DEP_DBUF, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_WRQ0_DEP_DBUF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_WR_EDRAM__INCLUDING_RMW, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_1_RD_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_2_RD_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_3_RD_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_COHERENCY_STALL_WAR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_L3_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_LINK_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_LINK_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_LOCK_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_LOCK_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_RD_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_SRAM_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_WAIT_L3, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_WAIT_LINK, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_WAIT_LOCK, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_FILTER_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_PREF_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SNOOP_HIT_OTHER, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SNOOP_HIT_PLB, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SNOOP_HIT_SAME, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_STREAM_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 2:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_WRQ1_DEP_DBUF, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_WRQ1_DEP_DBUF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_PLB_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_PLB_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_WAIT_PLB, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_WAIT_SRAM, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_PLB_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_PLB_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_SRAM_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_WAIT_PLB, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_WAIT_SRAM, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_INJ_VC0_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_CH2_VC0_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC1_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 3:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_1_RD_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_2_RD_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_3_RD_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_COHERENCY_STALL_WAR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_L3_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_LINK_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_LINK_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_LOCK_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_LOCK_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_RD_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_WAIT_L3, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_WAIT_LINK, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_WAIT_LOCK, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_FILTER_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_PREF_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SNOOP_HIT_OTHER, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SNOOP_HIT_PLB, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SNOOP_HIT_SAME, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_STREAM_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TI_TESTINT_ERR_EVENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_SNDR0_BUSY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 4:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_32B_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCBP_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_TOKEN_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_VCBN_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_VCBP_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_VCD0_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_VCD1_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_32B_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCBP_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_TOKEN_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_VCBN_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_VCBP_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_VCD0_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_VCD1_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 5:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS9_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_32B_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCBP_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_TOKEN_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_VCBN_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_VCBP_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_VCD0_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_VCD1_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_32B_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCBP_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_TOKEN_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_VCBN_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_VCBP_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_VCD0_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_VCD1_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 6:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_32B_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCBP_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_TOKEN_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_VCBN_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_VCBP_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_VCD0_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_VCD1_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_32B_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCBP_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCD0_VCD1_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_LINK_AVAIL_NO_VCD0_VCD_VCBN_TOKENS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_TOKEN_ACKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_VCBN_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_VCBP_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_VCD0_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_VCD1_CHUNKS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 7:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CH2_VC0_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CH1_VC0_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CH0_VC0_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CH2_VC1_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CH1_VC1_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS1_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS3_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS4_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS7_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS8_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS10_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS11_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS12_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS13_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS14_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS15_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_SNDR2_BUSY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_SNDR1_BUSY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV2_BUSY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV1_BUSY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV0_BUSY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_ALU_BUSY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 8:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_HDR_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_HDR_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_PKT_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_PKT_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PKT_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PKT_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_HDR_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_HDR_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PKT_DISC, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PKT_DISC, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 9:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC0_DPKT_RCV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC1_DPKT_RCV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC0_EMPTY_PKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC1_EMPTY_PKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_IDLPKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_KNOWN_BAD_PKT_MARKER, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC0_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC0_FULL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_VC1_FULL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_HDR_PARITY_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_CRC_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_UNEXPCT_HDR_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_0_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 10:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC0_DPKT_RCV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC1_DPKT_RCV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC0_EMPTY_PKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC1_EMPTY_PKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_IDLPKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_KNOWN_BAD_PKT_MARKER, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC0_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC1_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC0_FULL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_VC1_FULL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_HDR_PARITY_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_CRC_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_UNEXPCT_HDR_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_1_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 11:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC0_DPKT_RCV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC1_DPKT_RCV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC0_EMPTY_PKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC1_EMPTY_PKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_IDLPKT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_KNOWN_BAD_PKT_MARKER, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC0_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC1_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC0_FULL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_VC1_FULL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_HDR_PARITY_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_CRC_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_UNEXPCT_HDR_ERR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_RCV_2_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 12:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC0_EMPTY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC1_EMPTY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC0_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC1_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC0_PKT_SENT_TOTAL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC1_PKT_SENT_TOTAL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC0_DPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_VC1_DPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_IDLPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_RESEND_ATTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_2_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 13:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC0_EMPTY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC1_EMPTY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC0_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC1_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC0_PKT_SENT_TOTAL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC1_PKT_SENT_TOTAL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC0_DPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_VC1_DPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_IDLPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_RESEND_ATTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_1_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 14:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC0_EMPTY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC1_EMPTY, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC0_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC1_CUT_THROUGH, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC0_PKT_SENT_TOTAL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC1_PKT_SENT_TOTAL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC0_DPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_VC1_DPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_IDLPKTS_SENT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_RESEND_ATTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_SNDR_0_SRAM_ERR_CORR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 15:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CH0_VC1_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_INJ_VC1_HAVE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_CH1_VC0_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_CH0_VC0_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_INJ_VC0_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_CH2_VC1_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_CH1_VC1_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_CH0_VC1_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_INJ_VC1_MATURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_GREEDY_MODE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_REQ_PEND, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CORE_REQ_WAITING_RDY_GO, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS0_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS2_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS5_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_CLASS6_WINS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_LOCAL_CLIENT_BUSY_REC, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_LOCAL_CLIENT_BUSY_INJ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV2_ABORT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV1_ABORT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV0_ABORT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_LOCAL_CLIENT_ABORT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV2_PKT_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV1_PKT_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_RCV0_PKT_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_ARB_LOCAL_CLIENT_PKT_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        break;

    case 16:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_ALRDY_WAY_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_EDRAM_ACCESS_CYCLE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_EDRAM_RFR_CYCLE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_LINE_STARTS_EVICT_LINE_NUM_PRESSURE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_DIR_SET_DISBL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_NO_WAY_SET_AVAIL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_REQUIRING_CASTOUT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MISS_REQUIRING_REFILL_NO_WR_ALLOC, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_PLB_RDQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_RDQ0, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_RDQ1, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_MSHNDLR_TOOK_REQ_WRBUF, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PAGE_CLOSE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PAGE_OPEN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PLB_WRQ_DEP_DBUF, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PLB_WRQ_DEP_DBUF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PREF_REINS_PULL_OUT_NEXT_LINE, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_PREF_REQ_ACC_BY_PREF_UNIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_RD_BURST_1024B_LINE_RD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_RD_EDR__ALL_KINDS_OF_RD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_RD_MODIFY_WR_CYCLE_EDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_CACHE_INHIB_RD_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_CACHE_INHIB_WR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_NEEDS_CASTOUT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_REQ_TKN_NEEDS_REFILL, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_PERFCTR_NULL_EVENT, 0);
        switch(index2) {
            case 0:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_ADD_SUBTRACT, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_LD, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_LD, 0);
              break;
            case 1:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_MULT_DIV, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_ST, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_MULT_DIV, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_ST, 0);
              break;
            case 2:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_OEDIPUS_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_LD, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_LD, 0);
              break;
            case 3:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_TRINARY_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_ST, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_TRINARY_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_ST, 0);
              break;
            default:
              return(-1);
        }
        break;

    case 1000:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_LD, 0);
        break;

    case 1001:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_MULT_DIV, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_ST, 0);
        break;

    case 1002:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_LD, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_LD, 0);
        break;

    case 1003:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_ST, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_TRINARY_OP, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_ST, 0);
        break;

    case 1004:
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_L3_CACHE_MISS_DATA_WILL_BE_REQED_DDR, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC0_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_INJ_VC1_PYLD_ADDED, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC0_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TR_REC_VC1_PYLD_TKN, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU0_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_REQ_VALID, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_BLIND_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_L3_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_DCURD_SRAM_REQ, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_PU1_PREF_SELF_HIT, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_XP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_YP_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZM_PKTS, 0);
        bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_UPC_TS_ZP_PKTS, 0);
        switch(index2) {
            case 0:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_ADD_SUBTRACT, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_LD, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_ADD_SUBTRACT, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_LD, 0);
              break;
            case 1:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_MULT_DIV, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_DBL_ST, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_MULT_DIV, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_DBL_ST, 0);
              break;
            case 2:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_OEDIPUS_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_LD, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_OEDIPUS_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_LD, 0);
              break;
            case 3:
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_ARITH_TRINARY_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_FPU_LDST_QUAD_ST, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_ARITH_TRINARY_OP, 0);
              bgl_perfctr_add_remote_counters_ENABLE_COUNTER(BGL_2NDFPU_LDST_QUAD_ST, 0);
              break;
            default:
              return(-1);
        }
        break;

    default:
      return(-1);
  }

  if(0 != (retval = bgl_perfctr_commit())) {
    bgl_perfctr_revoke();
    return(-1);
  }
  CNTR_STATE->remote_flag.collect = 1;

  return (0);
}
