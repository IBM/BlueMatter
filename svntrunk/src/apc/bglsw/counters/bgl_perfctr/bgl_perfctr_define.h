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

#ifndef BGL_PERFCTR_DEFINE_H
#define BGL_PERFCTR_DEFINE_H

#define BGL_PERFCTR_MODE_REMOTE  90  /* Completely local - Only used for remote polling (with timer) */

#define BGL_UPC_GCR_IS_INTERRUPTING(n)   ( (n) & ((0xf)<<(31-5) | (0x3)<<(31-15)  | (0xf)<<(31-25) ))
#define BGL_UPC_GCR_GET_CNTA_IRQ_FREQ(n) (((n) >> (31- 5)) & 0xf)
#define BGL_UPC_GCR_GET_CNTB_IRQ_FREQ(n) (((n) >> (31-15)) & 0x3)
#define BGL_UPC_GCR_GET_CNTC_IRQ_FREQ(n) (((n) >> (31-25)) & 0xf)

/* In bgl_perfctr the "state" of the FPU counters control register is
 * _always_ handled in its "write" bit order. That is, any rts_read_dcr
 * of this register is most certainly going to be followed by passing that
 * bit pattern through BGL_FPU_CTRL_READorder2WRITEorder.
 * The "GET" bit operations of the FPU control word thus operates on
 * the bit format suitable for a later control word write.
 * As an example, the following call sequence copies the counter setup
 * from one FPU to another.
 * 	rts_read_dcr("fpu0_counterctrl_dcr",&bits)
 * 	rts_write_dcr("fpu1_counterctrl_dcr",BGL_FPU_CTRL_READorder2WRITEorder(bits))
*/

/* Location of bits in control word pertinent to the arithmetics counter */
#define BGL_FPU_PERF_ARITH_MASK    0x10000030U
#define BGL_FPU_PERF_ARITH_ACTIVE  0x10000000U
/* Location of bits in control word pertinent to the load/store counter */
#define BGL_FPU_PERF_LDST_MASK     0x01000003U
#define BGL_FPU_PERF_LDST_ACTIVE   0x01000000U

#define BGL_FPU_PERF_ARITH_IS_ACTIVE(a) ( a & BGL_FPU_PERF_ARITH_ACTIVE )
#define BGL_FPU_PERF_LDST_IS_ACTIVE(a)  ( a & BGL_FPU_PERF_LDST_ACTIVE )

#define BGL_FPU_PERF_SET_ARITH_MODE(a)  ((a & 0x03U) << 4)
#define BGL_FPU_PERF_SET_LDST_MODE(a)   (a & 0x03U)
#define BGL_FPU_PERF_GET_ARITH_MODE(a)   ((a >> 4 ) & 0x03U)
#define BGL_FPU_PERF_GET_LDST_MODE(a)   (a & 0x03U)

#define BGL_FPU_PERF_MAKE_CNTRL_WD(a_active,a_mode,m_active,m_mode) \
    ((a_active ? BGL_FPU_PERF_ARITH_ACTIVE : 0x0U) | \
     BGL_FPU_PERF_SET_ARITH_MODE(a_mode) | \
     (m_active ? BGL_FPU_PERF_LDST_ACTIVE : 0x0U) | \
     BGL_FPU_PERF_SET_LDST_MODE(m_mode))
#define BGL_FPU_CTRL_READorder2WRITEorder(a) ( ((a&0x20)<<(28-5)) | \
                                               ((a&0x10)<<(24-4)) | \
                                               ((a&0x0C)<<( 4-2)) | \
                                               ((a&0x03)<<( 0-0))  )

#define bgl_perfctr_add_remote_counters_ENABLE_COUNTER(event_num, event_edge)                                                           \
  event.num = event_num;                                                                                                                \
  event.edge = event_edge;                                                                                                              \
  retval = bgl_perfctr_add_event(event);                                                                                                \
  if (0 != retval) {                                                                                                                    \
/*     fprintf(stderr, "Error adding counter for event: %d (%s)\n", event.num, BGL_PERFCTR_event_table[event.num].event_name); */       \
    return(retval);                                                                                                                     \
  }

#endif /* BGL_PERFCTR_DEFINE_H */
