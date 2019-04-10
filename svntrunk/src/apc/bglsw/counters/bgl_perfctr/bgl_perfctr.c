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

static char COPYRIGHT[] =
"Licensed Materials - Property                                          \
 of IBM, 5733-BG1 (C) COPYRIGHT 2004, 2004                              \
 All Rights Reserved. US Government Users restricted Rights -           \
 Use, Duplication or Disclosure restricted by GSA ADP Schedule          \
 Contract with IBM Corp.";

/* Configure bgl_perfctr to use the MMAP interface to UPC counters */
#define USE_UPC_MMAP 0
#define MAX_NUM_CORES 2

#include <bglupcdcr.h>
#include <bglfpudcr.h>
#include "apc_perfctr.h"
#include "bgl_perfctr_define.h"
//#include "../../blrts/memory.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BGLGi.h"
#include "bgllockbox.h"
#include <sys/time.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>

#define BGL_MEM_SRAM_PAGE_3_4K                   0xfffff000
#define BGL_MEM_BGLPERFCTR_BASE BGL_MEM_SRAM_PAGE_3_4K
#define PAGE_SIZE_4K (4 * 1024)
#define BGL_MEM_BGLPERFCTR_SIZE PAGE_SIZE_4K

#define BGLPERFCTR_INTERNAL ((bgl_perfctr_internal_t *) BGL_MEM_BGLPERFCTR_BASE)

/* #define DEBUG 5 */
#ifdef DEBUG
#define DBG(a) {if(DEBUG) { \
  printf("DBG0@%s:%d[CPU%d]:: ",__func__,__LINE__,rts_get_processor_id()); \
  printf a; \
}}
#define DBGN(n,a) {if(n<DEBUG) { \
  printf("DBG%d@%s:%d[CPU%d]:: ",n,__func__,__LINE__,rts_get_processor_id()); \
  printf a; \
}}
#else
#define DBG(a)
#define DBGN(n,a)
#endif

#define GetLock(lock)                                                                           \
    BGL_Mutex_Acquire(lock);                                                                    \
    ++(CNTR_STATE->gen_start);                                                                  \
    DBGN(4,("Got lock " #lock " at %llu with gen=%d\n",getTimeBase(), CNTR_STATE->gen_start));
#define ReleaseLock(lock)                                                                               \
    ++(CNTR_STATE->gen_end);                                                                            \
    DBGN(4,("Releasing lock " #lock " at %llu with gen=%d\n",getTimeBase(),CNTR_STATE->gen_end));       \
    BGL_Mutex_Release(lock);



typedef struct bgl_prefctr_internal_state_t_ {
  unsigned user_app_core_map;
  volatile unsigned long long MagicWord[2];
  bgl_perfctr_control_t state;
} bgl_perfctr_internal_t;

/* Internal static variables used for timed interrupt driven reads */
/* These will be processor local */
typedef void handler_fcn(int);
static handler_fcn *bgl_perfctr_curr_handler = NULL;

bgl_perfctr_control_t *CNTR_STATE = &BGLPERFCTR_INTERNAL->state;

// LCK_A lock against changes to the data structures controlling counter mapping
static BGL_Mutex *LCK_A = NULL;
// LCK_B lock against changes to the values in the virtual counters
static BGL_Mutex *LCK_B = NULL;
// BARRIER is the barrier reserved for bglperfctr
static BGL_Barrier *BARRIER = NULL;

/* Set of internal support routines */
static int bgl_perfctr_setup_timed_read(int desired_mode);
static int bgl_perfctr_shutdown_timed_read(void);
static int timeoutguard(void *limit);
static int do_bgl_perfctr_update(void);


/* Reading counter registers from memory mapped region */
#ifdef USE_UPC_MMAP
#include "bglmemmap.h"
#define UPCMEMDCR ((unsigned int *)BGL_MEM_UPC_BASE)
#endif

unsigned int get_cpu_mhz(void) {
#include "bglpersonality.h"
  BGLPersonality bgl;
  if(rts_get_personality(&bgl,sizeof bgl))
    return 911;

  return bgl.clockHz/1000000;
}

#include "ppc440_core.h"
static inline unsigned
getPIR()
   {
   unsigned pid;
   __asm__ __volatile__("mfspr %0,%1" : "=r"(pid) : "i"(SPRN_PIR));
   return pid;
   }

/* Type enumerators for the different counter types */
#define CNTRA  0  /* UPC counter type A */
#define CNTRB  1  /* UPC counter type B */
#define CNTRC  2  /* UPC counter type C */
#define CNTRFP 3  /* Double Hummer FPU counter */

/* These are the encoding of counter overflow interrupt
 * thresholds available in the A and C counters on BG/L
 * I.e. the bit pattern 0xA corresponds to an overflow
 * interrupt when the counter reaches 256 (0x100)
 */
static inline unsigned int cntr_AC_interrupt_count(int val) {
  switch (val) {
  case  0: return 0x00000000U; break;  /* Disabled       */
  case 15: return 0x00000001U; break;  /* bit 31 changes */
  case 14: return 0x00000002U; break;  /* bit 30 changes */
  case 13: return 0x00000004U; break;  /* bit 29 changes */
  case 12: return 0x00000008U; break;  /* bit 28 changes */
  case 11: return 0x00000010U; break;  /* bit 27 changes */
  case 10: return 0x00000100U; break;  /* bit 23 changes */
  case  9: return 0x00000800U; break;  /* bit 20 changes */
  case  8: return 0x00004000U; break;  /* bit 17 changes */
  case  7: return 0x00020000U; break;  /* bit 14 changes */
  case  6: return 0x00100000U; break;  /* bit 11 changes */
  case  5: return 0x00800000U; break;  /* bit  8 changes */
  case  4: return 0x04000000U; break;  /* bit  5 changes */
  case  3: return 0x20000000U; break;  /* bit  2 changes */
  case  2: return 0x40000000U; break;  /* bit  1 changes */
  case  1: return 0x80000000U; break;  /* bit  0 changes */
  }
  return 0;
}

/* These are the corresponding thresholds and encodings
 * for the B register counters
 */
static inline unsigned int cntr_B_interrupt_count(int val) {
  switch (val) {
  case  0: return 0x00000000U; break;  /* Disabled       */
  case  7: return 0x00000001U; break;  /* bit 31 changes */
  case  6: return 0x00000002U; break;  /* bit 30 changes */
  case  5: return 0x00000800U; break;  /* bit 20 changes */
  case  4: return 0x00004000U; break;  /* bit 17 changes */
  case  3: return 0x00020000U; break;  /* bit 14 changes */
  case  2: return 0x00100000U; break;  /* bit 11 changes */
  case  1: return 0x80000000U; break;  /* bit  0 changes */
  }
  return 0;
}

static inline unsigned int encoding2hwreg(BGL_PERFCTR_event_encoding_t enc) {
  unsigned int g=enc.group, regoff=0;

  if(g<16)  /* UPC counter group */
    return 3*g+enc.counter;

  g-=16;
  regoff+=3*16;

  if(g<2)  /* FPU and FPU2 counter group */
    return regoff+2*g+enc.counter;
  g-=2;
  regoff+=2*2;

  return -1;
}

/* BGL_perfctr counter register number to DCR translation */
static inline unsigned int hwregister2dcr(int regnum) {
  if(regnum< 0)
    return -1;
  else if(regnum< 3*16)
    return regnum/3+regnum+BGL_UPCDCR_BASE;
  /* Someone decided to be helpful and made the DCRs flip-flop when seen from the different
   * cores. Now, this will cause disaster when doing updates to a shared memory area. So we
   * flop-flip the registers back again here. Reg 48 is arith counts in FPU0, reg 49 is ldst
   * counts in FPU0, reg 50 is arith counts in FPU1 and reg 51 is ldst in FPU1.
   */
  else if(regnum==3*16)
    return BGL_FPU_ARITH_INSTR_COUNT + (rts_get_processor_id() ? BGL_FPUDCR_OTHER_CORE : 0);
  else if(regnum==3*16+1)
    return BGL_FPU_LDST_INSTR_COUNT + (rts_get_processor_id() ? BGL_FPUDCR_OTHER_CORE : 0);
  else if(regnum==3*16+2)
    return BGL_FPU_ARITH_INSTR_COUNT + (rts_get_processor_id() ? 0 : BGL_FPUDCR_OTHER_CORE);
  else if(regnum==3*16+3)
    return BGL_FPU_LDST_INSTR_COUNT + (rts_get_processor_id() ? 0 : BGL_FPUDCR_OTHER_CORE);
  else
    return -1;
}

/* BGL_perfctr counter register number to counter type translation */
static inline unsigned int hwregister2type(int regnum) {
  if(regnum< 0)
    return -1;
  else if(regnum< 3*16)
    return regnum%3;  /* CNTRA/CNTRB/CNTRC */
  else if(regnum<3*16+4)
    return CNTRFP;
  else
    return -1;
}

// read the control register from DCR because now Kernel has wrote non-zeros into the CRs
static int init_control_register_value( unsigned int *ctrlwords ) {
  unsigned int ctrl, temp;
  int retval;
  unsigned int dcr;
  for(ctrl=0;ctrl<BGL_PERFCTR_NUM_CTRL;ctrl++) {
        dcr=BGL_UPC_GCR(ctrl);
        retval=rts_read_dcr(dcr, &temp);
        // retval=rts_read_dcr(dcr, &ctrlwords[ctrl]);
        ctrlwords[ctrl] = temp;
        // printf( "\tinit_control_register_value, ctrlwords[%2d] = 0x%8x, retval = %d\n",
	// 	ctrl, ctrlwords[ctrl], retval );
	if (retval) return retval;
  }
  return 0;
}

/* This routine allocates all necessary internal storage */
int bgl_perfctr_init(void) {
  int retval = 0 ;
  DBGN(4,("Called at %llu\n",getTimeBase()));

  CNTR_STATE->gen_start = 0;
  CNTR_STATE->gen_end   = 0;
  GetLock(LCK_A);
  {
    unsigned core, other_rank, numProcs;
    BGLPersonality BGLP;

    rts_get_personality(&BGLP, sizeof BGLP);
    DBGN(4,("x=%2d, y=%2d, z=%2d, t=%d\n",BGLP.xCoord,BGLP.yCoord,BGLP.zCoord,
           rts_get_processor_id()));

    for(core=0; core<MAX_NUM_CORES; core++)
      if(
           !(rts_rankForCoordinates(BGLP.xCoord, BGLP.yCoord, BGLP.zCoord, core, &other_rank, &numProcs)) &&
           (other_rank < numProcs)
        )
        BGLPERFCTR_INTERNAL->user_app_core_map |= 0x1 << core;
  }
  ReleaseLock(LCK_A);

  if(BGLPERFCTR_INTERNAL->user_app_core_map == 0x3) {
    DBGN(4,("Before shutdown barrier. TB=%llu\n",getTimeBase()));
    BGL_Barrier_Pass(BARRIER);
    DBGN(4,("Passed shutdown barrier. TB=%llu\n",getTimeBase()));
  }

  GetLock(LCK_A);
  {
    // read the control register from DCR because now Kernel has wrote non-zeros into the CRs
    if (init_control_register_value(CNTR_STATE->ctrl)) retval = -1;
    if (!retval && BGLPERFCTR_INTERNAL->MagicWord[rts_get_processor_id()]) retval = -1;
    if (!retval) BGLPERFCTR_INTERNAL->MagicWord[rts_get_processor_id()] = getTimeBase();
  }
  ReleaseLock(LCK_A);

  DBGN(4,("Map=0x%x \n",BGLPERFCTR_INTERNAL->user_app_core_map));
  return retval;
}

/* This routine allocates all necessary internal storage */
int bgl_perfctr_init_synch(int mode) {
  char* env = NULL;
  void* lockp;
  char c;
  int i,j,index;

  switch (mode) {
  case BGL_PERFCTR_MODE_LOCAL:
  case BGL_PERFCTR_MODE_REMOTE:
  case BGL_PERFCTR_MODE_ASYNC:
  case BGL_PERFCTR_MODE_SYNC:
    break;
  default:
    return -1;
  }

  for (index = BGL_MUTEX_MIN; index < BGL_MUTEX_MAX; ++index) {
    if (rts_alloc_lockbox(index, &lockp, BGL_LOCKBOX_ORDERED_ALLOC) == 0) {
      LCK_A = (BGL_Mutex *)lockp;
      break;
    }
  }

  for (++index ; index < BGL_MUTEX_MAX; ++index) {
    if (rts_alloc_lockbox(index, &lockp, BGL_LOCKBOX_ORDERED_ALLOC) == 0) {
      LCK_B = (BGL_Mutex *)lockp;
      break;
    }
  }

  for (index = BGL_BARRIER_MIN; index < BGL_BARRIER_MAX; ++index) {
    if (rts_alloc_lockbox(index, &lockp, BGL_LOCKBOX_ORDERED_ALLOC) == 0) {
      BARRIER = (BGL_Barrier *)lockp;
      break;
    }
  }

  if (mode == BGL_PERFCTR_MODE_REMOTE) {
    /* Retrieve environment variable BGL_PERFMON */
    env = getenv("BGL_PERFMON");

    /* Process the environment variable which identifies the counter */
    /* definition id to use.  If not defined or not specified, the   */
    /* counter definition id to use is 1004.  If a value is          */
    /* specified, but is not legal, then counter events will not be  */
    /* enabled.                                                      */
    CNTR_STATE->defn_id = 0;
    if (env != NULL) {
      j = strlen(env);
      if (j) {
        if (j < 5) {
          for (i=0;i<j;i++) {
              c = env[i];
              if (!isdigit(c))
                  return 0;
          }
          i = atoi(env);
          if (i>=0 && ((i<17) || (i >= 1000 && i <= 1004)))
              CNTR_STATE->defn_id = i;
          else
              return 0;
        }
        else
          return 0;
      }
      else
        /* Set the default counter definition id */
        CNTR_STATE->defn_id = 1004;
    }
    else
      /* Set the default counter definition id */
      CNTR_STATE->defn_id = 1004;
    CNTR_STATE->remote_flag.collect = 0;
  } else
    if (CNTR_STATE->remote_flag.collect)
      bgl_perfctr_shutdown();

  if(bgl_perfctr_init())
    return -1;

  /* If a local mode without interrupt was asked for we are done */
  if(BGL_PERFCTR_MODE_LOCAL == mode)
    return mode;

  /* We have a request for timed interrupts (remote, sync, or async) */
  mode = bgl_perfctr_setup_timed_read(mode);

  if ( (BGL_PERFCTR_MODE_REMOTE == mode) && (0 != bgl_perfctr_add_remote_counters()) )
    return -1;

  return mode;
}

/* This routine frees all internal allocated memory.
 * Uninstall any interrupts and handlers etc
 */
int bgl_perfctr_shutdown(void){
  DBGN(2,("Begin shutdown\n"));
  if(bgl_perfctr_curr_handler) {
    bgl_perfctr_shutdown_timed_read();
  }

  GetLock(LCK_A);

  /* Free any counters added by this core */
  BGLPERFCTR_INTERNAL->MagicWord[rts_get_processor_id()] = 0;

  /* If all MagicWords are 0, memclear the structure (but not the lock pointers */
  if((BGLPERFCTR_INTERNAL->MagicWord[0] == 0) &&
     (BGLPERFCTR_INTERNAL->MagicWord[1] == 0)) {
    unsigned ctrl, dcr;
    int retval;

    DBGN(2,("Clearing all data structures %p size %d\n",CNTR_STATE,sizeof *CNTR_STATE));
    CNTR_STATE->remote_flag.collect = 0;
    CNTR_STATE->defn_id = 0;
    {  //memset generates a SIGBUS error here...
      int i;
      int *p=(int*) CNTR_STATE;
      for(i=0;i < sizeof(*CNTR_STATE)/sizeof(int); i++)
        *(p++)=0x0;
    }

    for(ctrl=0;ctrl<BGL_PERFCTR_NUM_CTRL;ctrl++){
      if(ctrl < 16)
        dcr=BGL_UPC_GCR(ctrl);
      else if(ctrl==16)
        dcr=BGL_FPU_PERF_COUNT_CTRL;
      else if(ctrl==17)
        dcr=BGL_FPU_PERF_COUNT_CTRL+BGL_FPUDCR_OTHER_CORE;
      else
        abort();

      retval=rts_write_dcr(dcr,0x0);
      assert(retval==0);
    }
  }
  ReleaseLock(LCK_A);

  DBGN(2,("Done.\n"));
  return 0;
}

/* This routine updates the part of a register control word
 * for the correct counter. Counters A, B and C of each group
 * share one control word as do the two FPU counters.
 */
static int set_control_register_value(unsigned int *ctrlwords,
                            BGL_PERFCTR_event_encoding_t *ev,
                                      unsigned int edge_bits,
                                      unsigned int irq_bits) {
  unsigned int ctrlreg;
  unsigned int cword,code;

  code= ev ? ev->code : 0;
  ctrlreg=ev->group;        /*operating on NULL*/
  cword=ctrlwords[ctrlreg];

  if(ctrlreg<16) {
    switch (ev->counter) {
    case 0:
      cword &= ~ BGL_UPC_GCR_CNTA_EVENT_SEL(~0);
      cword |=   BGL_UPC_GCR_CNTA_EVENT_SEL(code);
      cword |=   BGL_UPC_GCR_CNTA_EDGE_SELECT(edge_bits);
      cword |=   BGL_UPC_GCR_CNTA_IRQ_FREQ(irq_bits);
      break;
    case 1:
      cword &= ~ BGL_UPC_GCR_CNTB_EVENT_SEL(~0);
      cword |=   BGL_UPC_GCR_CNTB_EVENT_SEL(code);
      cword |=   BGL_UPC_GCR_CNTB_EDGE_SELECT(edge_bits);
      cword |=   BGL_UPC_GCR_CNTB_IRQ_FREQ(irq_bits);
      break;
    case 2:
      cword &= ~ BGL_UPC_GCR_CNTC_EVENT_SEL(~0);
      cword |=   BGL_UPC_GCR_CNTC_EDGE_SELECT(edge_bits);
      cword |=   BGL_UPC_GCR_CNTC_IRQ_FREQ(irq_bits);
      cword |=   (BGL_UPC_GCR_CNTC_EVENT_SEL(code) |
                  BGL_UPC_GCR_CNTC_MODE(0));
      break;
    default:
      return -1;
    }

  } else if(ctrlreg<18) {
    switch (ev->counter) {
    case 0:
      cword &= ~(BGL_FPU_PERF_ARITH_MASK);
      cword |= BGL_FPU_PERF_ARITH_ACTIVE | BGL_FPU_PERF_SET_ARITH_MODE(code);
      break;
    case 1:
      cword &= ~(BGL_FPU_PERF_LDST_MASK);
      cword |= BGL_FPU_PERF_LDST_ACTIVE | BGL_FPU_PERF_SET_LDST_MODE(code);
      break;
    default:
      return -1;
    }

  } else {
    return -1;
  }

  ctrlwords[ctrlreg]=cword;
  return 0;
}

/* This routine tries to see if an event can be scheduled to be added
 * at the next commit
 */
int bgl_perfctr_add_event(BGL_PERFCTR_event_t event){
  int i,k,err;
  unsigned long long position;
  unsigned long long occupied;
  unsigned irq_bits=0;
  BGL_PERFCTR_event_descr_t *table=BGL_PERFCTR_event_table+event.num;
  BGL_PERFCTR_event_encoding_t *encoding=table->encoding;

  if((event.num<0)||(event.num>BGL_PERFCTR_NULL_EVENT))
    return -1;

  GetLock(LCK_A);

  occupied = CNTR_STATE->in_use;

  /* The event is already running. Increase the
     reference count and exit */
  for(i=0;i<CNTR_STATE->nmapped;i++) {
    if((CNTR_STATE->map[i].event.num==event.num) &&
       (CNTR_STATE->map[i].event.edge==event.edge) ) {
       // && (event.irq_freq == 0) &&
       //(CNTR_STATE->map[i].event.irq_freq==0)

      ++CNTR_STATE->map[i].new_count;
      ReleaseLock(LCK_A);
      return 0;
    }
    /* If this counter is not actively used, but being schedule to be used
     * mark it as in use
     */
    if( (CNTR_STATE->map[i].ref_count==0) &&
        (CNTR_STATE->map[i].new_count>0) )
      occupied |= 0x1ULL << CNTR_STATE->map[i].counter_register;
  }

  /* Try each encoding and see if any if them matches a
     free counter register. If so, insert it in the event map */
  for(i=table->num_encodings-1;i>-1;i--) {
    int reg;

    reg=encoding2hwreg(encoding[i]);

    if(hwregister2type(reg)==CNTRFP) /* FPU register do not have edge or irq settings */
      if(event.edge) // || event.irq_freq)
        continue;

    //printf("Checking event %s instance %d group=%d reg=%d\n",
    //       table->event_name,i,encoding[i].group,reg);
    position = 0x1ULL << reg;
    if(position & occupied)
      continue;

    //CNTR_STATE->in_use |= position;
    CNTR_STATE->modified |= 0x1ULL << encoding[i].group;

    for(k=CNTR_STATE->nmapped;k>0 && (CNTR_STATE->map[k-1].event.num>event.num); k--)
      CNTR_STATE->map[k]=CNTR_STATE->map[k-1];

    CNTR_STATE->map[k].event=event;
    CNTR_STATE->map[k].new_count=1;
    CNTR_STATE->map[k].ref_count=0;
    CNTR_STATE->map[k].counter_register=reg;
    CNTR_STATE->map[k].cntrl_register=encoding[i].group;

/*******************************************************
    CNTR_STATE->map[k].restart_value=0;
    if(event.irq_freq) {
      int bits;
      unsigned ival;
      if(hwregister2type(reg)==CNTRB){
        for(bits=7;bits;bits--) {
          ival=cntr_B_interrupt_count(bits);
          if(ival>event.irq_freq) {
            irq_bits=bits;
            CNTR_STATE->map[k].restart_value=ival-event.irq_freq;
            break;
          }
        }
      } else {
        for(bits=15;bits;bits--) {
          ival=cntr_AC_interrupt_count(bits);
          if(ival>event.irq_freq) {
            irq_bits=bits;
            CNTR_STATE->map[k].restart_value=ival-event.irq_freq;
            break;
          }
        }
      }
    }
****************************************************/

    CNTR_STATE->nmapped++;

    err=set_control_register_value(CNTR_STATE->ctrl,&encoding[i],event.edge,irq_bits);
    if(err) {
      ReleaseLock(LCK_A);
      return err;
    }
    break;
  }

  ReleaseLock(LCK_A);
  if(i<0)
    return -1;

  return 0;
}

/* This routine schedules an event for removal at the next commit */
int bgl_perfctr_remove_event(BGL_PERFCTR_event_t event){
  int pos;
  int retval;

  if((event.num<0)||(event.num>BGL_PERFCTR_NULL_EVENT))
    return -1;

  GetLock(LCK_A);

  for(pos=0;pos<CNTR_STATE->nmapped;pos++) {
    if( (CNTR_STATE->map[pos].event.num==event.num) &&
        //(CNTR_STATE->map[pos].event.irq_freq==event.irq_freq) &&
        (CNTR_STATE->map[pos].event.edge==event.edge)){
      break;
    }
  }

  retval=-1;
  if(pos<CNTR_STATE->nmapped) {
    if(CNTR_STATE->map[pos].new_count>0) {
      --CNTR_STATE->map[pos].new_count;
      retval=0;
    }
  }

  ReleaseLock(LCK_A);
  return retval;
}

static int clean_empty_map_slots(void) {
  int i,pos,next,skip;

  for(pos=0;pos<CNTR_STATE->nmapped;pos++) {
    if(CNTR_STATE->map[pos].ref_count<1) {
      for(next=pos+1;next<CNTR_STATE->nmapped;next++)
        if(CNTR_STATE->map[next].ref_count>0)
          break;

      skip=next-pos;
      CNTR_STATE->nmapped-=skip;
      for(i=pos;i<CNTR_STATE->nmapped;i++)
        CNTR_STATE->map[i]=CNTR_STATE->map[i+skip];
    }
  }

  return 0;
}

/* This routine clears all pending actions (removals and
 * additions. It recovers to the current state by reading
 * all counter control words for the running counters.
 */
int bgl_perfctr_revoke(void){
  int pos;
  unsigned int ctrl;

  /* Set the current count to be ref_count. */
  for(pos=0;pos<CNTR_STATE->nmapped;pos++) {
    if((CNTR_STATE->map[pos].new_count=CNTR_STATE->map[pos].ref_count) == 0)
      CNTR_STATE->in_use &= ~( 0x1ULL << CNTR_STATE->map[pos].counter_register );
  }

  /* Remove any map positions that are not actually used */
  clean_empty_map_slots();

  /* All modified ctrl registers are read back to their real state */
  for(ctrl=0;ctrl<BGL_PERFCTR_NUM_CTRL;ctrl++) {
    if(CNTR_STATE->modified & (0x1ULL << ctrl)) {
      unsigned int dcr;
      int err;

      if(ctrl < 16) {
#ifdef USE_UPC_MMAP
        *(CNTR_STATE->ctrl+ctrl) =  *(UPCMEMDCR+4*ctrl+3);
#else
        dcr=BGL_UPC_GCR(ctrl);
        err=rts_read_dcr(dcr,CNTR_STATE->ctrl+ctrl);
        if(err)
          return err;
#endif
      }
      else if(ctrl<18) {
        unsigned curr_status;
        if(ctrl==16)
          dcr=BGL_FPU_PERF_COUNT_CTRL + (rts_get_processor_id() ? BGL_FPUDCR_OTHER_CORE : 0) ;
        else
          dcr=BGL_FPU_PERF_COUNT_CTRL + (rts_get_processor_id() ? 0 : BGL_FPUDCR_OTHER_CORE ) ;

        err=rts_read_dcr(dcr,&curr_status);
        if(err)
          return err;

        *(CNTR_STATE->ctrl+ctrl)=BGL_FPU_CTRL_READorder2WRITEorder(curr_status);
      }
      else
        return -1;

      CNTR_STATE->modified &= ~(0x1ULL << ctrl);
    }
  }

  return 0;
}

/* Perform all pending changes to the current counters */
int bgl_perfctr_commit(){
  int ctrl,pos;
  int retval=0;
  unsigned flipFPUregs=rts_get_processor_id();

  GetLock(LCK_A);

  DBGN(2,("At %llu, nmapped=%d, modified=0x%llx\n",getTimeBase(),
          CNTR_STATE->nmapped,CNTR_STATE->modified));

  /* Reference count is now the new count */
  for(pos=0;pos<CNTR_STATE->nmapped;pos++) {
    if((CNTR_STATE->map[pos].ref_count == 0) &&
       (CNTR_STATE->map[pos].new_count > 0)) {
      unsigned int dcr;
      int reg=CNTR_STATE->map[pos].counter_register;
      int err;

      GetLock(LCK_B);

      DBGN(2,("Initiating new counter in counter pos %d.\n",reg));

      dcr=hwregister2dcr(reg);

      /* In BG/L control register 16 is the FPU of THIS core, while 17 is FPU of OTHER core
      *  To be able to have a shared view of things in virtual node mode we need to flop-flip
      *  back to a common 1-1 mapping. After this 16 is FPU0 and 17 is FPU1 throughout.
      */
      if(flipFPUregs){
        if(reg>49) { /* This is the 2ND FPU registers. On core1 this is "THIS" */
          dcr=hwregister2dcr(reg-2);
        } else if(reg>47) { /* This is 1ST FPU registers. On core 1 this is "OTHER" */
          dcr=hwregister2dcr(reg+2);
        }
      }

      err=rts_write_dcr(dcr,0x0);
      if(err)
        fprintf(stderr,"%s[%d]:: rts_write_dcr returned error %d for DCR 0x%x\n",
                __func__,__LINE__,err,dcr);
      CNTR_STATE->last[reg]=0;
      CNTR_STATE->vdata[reg]=0ULL;

      ReleaseLock(LCK_B);
    }
    if((CNTR_STATE->map[pos].ref_count=CNTR_STATE->map[pos].new_count) == 0)
      CNTR_STATE->in_use &= ~( 0x1ULL << CNTR_STATE->map[pos].counter_register );
    else if(CNTR_STATE->map[pos].ref_count)
      CNTR_STATE->in_use |= ( 0x1ULL << CNTR_STATE->map[pos].counter_register );
  }

  /* Unused map slots are removed */
  clean_empty_map_slots();

  /* Write any modified DCR registers */
  /* We should disable interrupts while we are mucking around here */
  for(ctrl=0;ctrl<BGL_PERFCTR_NUM_CTRL;ctrl++) {
    if(CNTR_STATE->modified & (0x1ULL << ctrl)) {
      unsigned int dcr, ctrl_word;

      if(ctrl < 16)
        dcr=BGL_UPC_GCR(ctrl);
      else if(ctrl==16)
        dcr=BGL_FPU_PERF_COUNT_CTRL;
      else if(ctrl==17)
        dcr=BGL_FPU_PERF_COUNT_CTRL+BGL_FPUDCR_OTHER_CORE;
      else {
        retval=-1;
        goto early_exit;
      }

      ctrl_word=CNTR_STATE->ctrl[ctrl];
      retval=rts_write_dcr(dcr,ctrl_word);
      if(retval)
        goto early_exit;

      /* Counters that are not generating interrupts, but have reached a zero
       * access count can be left running (it saves us DCR writes)
       */

/*************************************************
      if((ctrl<16) && (BGL_UPC_GCR_IS_INTERRUPTING(ctrl_word))) {
        unsigned int reg;
        reg=3*ctrl;
        if(BGL_UPC_GCR_GET_CNTA_IRQ_FREQ(ctrl_word)) {
          retval=rts_write_dcr(hwregister2dcr(reg),CNTR_STATE->map[reg].restart_value);
          if(retval)
            goto early_exit;
          CNTR_STATE->last[reg] = CNTR_STATE->map[reg].restart_value;
        }
        ++reg;
        if(BGL_UPC_GCR_GET_CNTB_IRQ_FREQ(ctrl_word)) {
          retval=rts_write_dcr(hwregister2dcr(reg),CNTR_STATE->map[reg].restart_value);
          if(retval)
            goto early_exit;
          CNTR_STATE->last[reg] = CNTR_STATE->map[reg].restart_value;
        }
        ++reg;
        if(BGL_UPC_GCR_GET_CNTC_IRQ_FREQ(ctrl_word)) {
          retval=rts_write_dcr(hwregister2dcr(reg),CNTR_STATE->map[reg].restart_value);
          if(retval)
            goto early_exit;
          CNTR_STATE->last[reg] = CNTR_STATE->map[reg].restart_value;

        }
      }
*************************************************/

      CNTR_STATE->modified &= ~(0x1ULL << ctrl);
    }
  }

early_exit:
  DBGN(1,("Finished\n"));
  ReleaseLock(LCK_A);
  return retval;
}

int bgl_perfctr_dump_state(FILE *fh){
  int i;

  if(fh==NULL)
    fh=stderr;
  if(!CNTR_STATE){
    fprintf(fh,"No perfctr handle available!\n");
    return -1;
  }

  GetLock(LCK_A);

  fprintf(fh,"-------- bgl_perfctr_dump_state -------\n");
  fprintf(fh,"  %d defined events. in_use=0x%.8llx modified=0x%.8llx\n",
          CNTR_STATE->nmapped,CNTR_STATE->in_use,CNTR_STATE->modified);
  fprintf(fh," Id      code   - Interpretation\n");
  for(i=0;i<BGL_PERFCTR_NUM_CTRL;i++) {
    unsigned int code;

    if(i==0)
      fprintf(fh,"%-14s  A: edge  code IRQ | B: edge  code IRQ | "
              "C: edge  code IRQ\n","     UPC events");
    else if(i==16)
      fprintf(fh,"%-14s  ARITH:   Act Code | LD/ST:   Act Code\n",
              "     FPU Hummer");
    else if(i==17)
      fprintf(fh,"%-14s  ARITH:   Act Code | LD/ST:   Act Code\n",
              "FPU Hummer CPU2");

    code=CNTR_STATE->ctrl[i];
    fprintf(fh,"%3d: 0x%.8x\n",i,code);
    fprintf(fh," %c\n",CNTR_STATE->modified & (0x1ULL<<i) ? 'M':'-');
    if(i<16)
      fprintf(fh,"%5d %5d %3c  |  %5d %5d %3c  |  %5d %5d %3c  c-mode=%1d\n",
              (code >> 30 ) & 0x03U, (code >> 21 ) & 0x1FU,
              (code >> 26 ) & 0x0FU ? 'I' : '-',
              (code >> 19 ) & 0x03U, (code >> 12 ) & 0x1FU,
              (code >> 16 ) & 0x07U ? 'I' : '-',
              (code >> 10 ) & 0x03U, (code >>  1 ) & 0x1FU,
              (code >>  6 ) & 0x0FU ? 'I' : '-',
              code & 0x1U);
    else if(i<18)
      fprintf(fh,"   %8d %3d  |     %8d %3d\n",
              BGL_FPU_PERF_ARITH_IS_ACTIVE(code),
              BGL_FPU_PERF_GET_ARITH_MODE(code),
              BGL_FPU_PERF_LDST_IS_ACTIVE(code),
              BGL_FPU_PERF_GET_LDST_MODE(code));
    else
      fprintf(fh,"---- Not implemented this counter group register ---\n");
  }

  fprintf(fh,"\n");
  fprintf(fh," Id   Event      H/W  CtrlReg RefCount NewCount (Name)\n");
  for(i=0;i<CNTR_STATE->nmapped;i++) {
    bgl_perfctr_control_map_t *p=CNTR_STATE->map+i;
    fprintf(fh,"%3d: %6d %8d %8d %8d %8d (%s)\n",
            i,
            p->event.num,
            p->counter_register,
            p->cntrl_register,
            p->ref_count,
            p->new_count,
            BGL_PERFCTR_event_table[p->event.num].event_name);
  }
  ReleaseLock(LCK_A);

  GetLock(LCK_B);
  fprintf(fh,"\n");
  fprintf(fh,"Current cached values in the active counters\n");
  fprintf(fh,"\n");
  fprintf(fh,"         Last               Virtual\n");
  for(i=0;i<BGL_PERFCTR_NUM_COUNTERS;i++) {
    if(CNTR_STATE->in_use & ( 0x1ULL << i))
      fprintf(fh,"%3d: %12u   %18llu\n", i,
              CNTR_STATE->last[i],CNTR_STATE->vdata[i]);
  }
  ReleaseLock(LCK_B);

  return 0;
}

/***********************************************
int bgl_perfctr_restart_irqcounter(int map_id) {
  int counter=CNTR_STATE->map[map_id].counter_register;
  unsigned long long new_value;

  new_value=CNTR_STATE->map[map_id].restart_value;
  CNTR_STATE->last[counter]=new_value;

  return rts_write_dcr(hwregister2dcr(counter),new_value);

}
***********************************************/

bgl_perfctr_control_t *bgl_perfctr_hwstate(void){
  return CNTR_STATE;
}

/***********************************************
static unsigned long long *bgl_perfctr_get_counters(void){
  int retval;

  DBGN(2,("At %llu\n",getTimeBase()));
  GetLock(LCK_B);

  retval=do_bgl_perfctr_update();

  if(retval)
    return NULL;
  else
    return CNTR_STATE->vdata;
}

static void bgl_perfctr_release_counters(void){

  DBGN(2,("At %llu\n",getTimeBase()));
  ReleaseLock(LCK_B);
}
***********************************************/

int bgl_perfctr_copy_state(bgl_perfctr_control_t *hw_state, size_t buffsize) {
  int retval;
  const size_t nbytes=sizeof *hw_state;
  if(!hw_state)
    return -1;
  if(buffsize < nbytes)
    return -2;

  if(!CNTR_STATE)
    return -1;
  if(!CNTR_STATE)
    return -1;

  DBGN(2,("At %llu\n",getTimeBase()));
  GetLock(LCK_A);
  GetLock(LCK_B);

  retval=do_bgl_perfctr_update();
  if(!retval)
    memcpy(hw_state,CNTR_STATE,nbytes);

  ReleaseLock(LCK_B);
  ReleaseLock(LCK_A);
  DBGN(2,("Done at %llu\n",getTimeBase()));

  return retval;
}

int bgl_perfctr_copy_counters(unsigned long long *values, size_t buffsize) {
  int retval;
  const size_t nbytes=BGL_PERFCTR_NUM_COUNTERS * (sizeof *values);
  if(!values)
    return -1;
  if(buffsize < nbytes)
    return -2;

  if(!CNTR_STATE)
    return -1;

  DBGN(2,("At %llu\n",getTimeBase()));
  GetLock(LCK_B);

  retval=do_bgl_perfctr_update();
  if(!retval)
    memcpy(values,CNTR_STATE->vdata,nbytes);

  ReleaseLock(LCK_B);
  DBGN(2,("Done at %llu\n",getTimeBase()));

  return retval;
}

int bgl_perfctr_update(void) {
  int retval;

  if(!CNTR_STATE)
    return -1;
  if(CNTR_STATE->modified)
    return -1;

  DBGN(2,("At %llu\n",getTimeBase()));
  GetLock(LCK_B);

  retval=do_bgl_perfctr_update();
  ReleaseLock(LCK_B);

  DBGN(2,("Done at %llu\n",getTimeBase()));
  return retval;
}

static int do_bgl_perfctr_update(void) {
  int i;
/*   int bit; */
  unsigned curr_count, *last_p;
  unsigned long long *virt_p;
  unsigned long long in_use = CNTR_STATE->in_use;

  assert(BGL_Mutex_Is_Locked(LCK_B));

  last_p=CNTR_STATE->last;
  virt_p=CNTR_STATE->vdata;

  CNTR_STATE->latest_update=getTimeBase();

  /* All counters should be read in _ONE_ system call outside the
     loop. This lacks O/S system support at the moment */
  /*
    rts_read_all_hwcounters(&curr_dcr_counts);
   */
  for(i=0;in_use && (i<BGL_PERFCTR_NUM_COUNTERS); ) {
    if(!(in_use & 0xFF)) {
      DBGN(4,("Jumping 8 counters... 0x%llx  i=%d\n",in_use,i));
      i+=8;
      in_use = in_use >>8;
      last_p += 8;
      virt_p += 8;
      continue;
    }
    if(in_use & 0x1) {
      if(i<48) { // UPC counters can be read using memory mapped area
#ifdef USE_UPC_MMAP
        /* This is inefficient in that we reread the counter values from uncached memory
         * instead of reading 128 bits with a load quad word instruction once every fourth
         * iteration. But it works reasonably well as we only load for the counters actually
         * in use.
         */
        curr_count=*(UPCMEMDCR+i+(i/3));

#if 0  // Verbose output to double check memory read against DCR read
        { // Verbose screen clobbering to make sure we get the right map and take care of
          // counter resets correctly
          unsigned int dcr_id=hwregister2dcr(i);
          unsigned int cnt;
          int err;

          err=rts_read_dcr(dcr_id,&cnt);
          if(err)
            fprintf(stderr,"%s[%d]:: rts_read_dcr returned error!\n");
          fprintf(stderr,"%s[%d]:: %.2d 0x%.4x -> Mem 0x%.8x DCR 0x%.8x\n",
                  __func__ ,__LINE__,i,dcr_id,curr_count,cnt);
          curr_count=*(UPCMEMDCR+i+(i/3));
          rts_read_dcr(dcr_id,&cnt);
          fprintf(stderr,"%s[%d]:: %.2d 0x%.4x -> Mem 0x%.8x DCR 0x%.8x\n",
                  __func__ ,__LINE__,i,dcr_id,curr_count,cnt);
        }
#endif

#else
        unsigned int dcr_id=hwregister2dcr(i);
        int err;
        rts_read_dcr(dcr_id,&curr_count);
        if(err)
          fprintf(stderr,"%s[%d]:: rts_read_dcr returned error!\n");
#endif
      } else { // Other counters (i.e. the FPU, must used system call
        unsigned int dcr_id=hwregister2dcr(i);
        int err;
        err=rts_read_dcr(dcr_id,&curr_count);
        if(err)
          fprintf(stderr,"%s[%d]:: i=%d; rts_read_dcr returned error code %d for DCR 0x%x\n",
                  __func__ ,__LINE__,i,err,dcr_id);
      }

      /* The following subtraction will give the right result even
       * if the counter has wrapped and curr_count is less
       * than *last_p.
       */
      DBGN(4,("*virt_p += curr_count - *last_p;  %llu, %u, %u\n",*virt_p,curr_count,*last_p));
      *virt_p += curr_count - *last_p;

#ifdef USE_UPC_MMAP
      /* Even the B-counters keep running when they are read through
       *  memory. Possibly more logic is needed here if we look at
       * interrupting counters.
       */
        *last_p = curr_count;
#else
      if(hwregister2type(i) != CNTRB )
        *last_p = curr_count;
      else { /* The B counters are reset upon read through DCRs */
        if(!BGL_UPC_GCR_GET_CNTB_IRQ_FREQ(CNTR_STATE->ctrl[i/3])) {
          *last_p = 0;
        } else { /* We need to restore our interrupting counter */
          *last_p = curr_count;
          rts_write_dcr(hwregister2dcr(i),curr_count);
        }
      }
#endif
    }
    last_p++; virt_p++;
    in_use=in_use>>1;
    i++;
  }

  CNTR_STATE->latest_update=getTimeBase();
  return 0;
}

void bgl_perfctr_intr_handler(int signal) {
  DBGN(4,("Called at %llu\n",getTimeBase()));
  if(!CNTR_STATE) {
    fprintf(stderr,"bgl_perfctr handler called without a proper handler!\n");
    return;
  }

  /* TEMPORARILY BACKED OUT
     It is always the case that gen_start and gen_end should be equal if neither
     LCK_A nor LCK_B are currently held.  If gen_start and gen_end are not equal
     and neither lock is currently held, perfmon.py will no longer collect any
     counters.  The following code is an attempt to detect that situation and
     get gen_start and gen_end 'back in sync'.  If upon entrance into this
     interrupt handler, if gen_start does not equal gen_end, we check to see
     if either lock is currently held.  If so, we note that and exit the handler.
     If not, then we set gen_start equal to the gen_end value before any
     processing is performed by this handler.                                     */

  if((1==2) & (CNTR_STATE->gen_start != CNTR_STATE->gen_end)) {
    int have_locks = 0;
    if((!BGL_Mutex_Is_Locked(LCK_A)) & BGL_Mutex_Try(LCK_A)) {
      ++(CNTR_STATE->gen_start);
      if((!BGL_Mutex_Is_Locked(LCK_B)) & BGL_Mutex_Try(LCK_B)) {
        ++(CNTR_STATE->gen_start);
        /* +2 to compensate for the two releases below...                        */
        CNTR_STATE->gen_start = CNTR_STATE->gen_end + 2;
        DBGN(4,("Interrupt handler invoked with gen_start != gen_end.  Locks not held.  Start and end were synced.  Now gen_start=%d\n", CNTR_STATE->gen_start));
        ReleaseLock(LCK_B);
        ReleaseLock(LCK_A);
      }
      else {
        ReleaseLock(LCK_A);
        have_locks = 1;
      }
    }
    else {
      have_locks = 1;
    }
    if (have_locks) {
      DBGN(4,("Interrupt handler invoked with gen_start != gen_end.  Locks were already held.  Now gen_start=%d, gen_end=%d.  Early exit.\n", CNTR_STATE->gen_start, CNTR_STATE->gen_end));
      DBGN(4,("Handler completed at %llu\n",getTimeBase()));
      return;
    }
  }

  if(BGL_Mutex_Try(LCK_B)) {
    ++(CNTR_STATE->gen_start);
    DBGN(4,("Got lock LCK_B at %llu with gen=%d\n",getTimeBase(), CNTR_STATE->gen_start));
    do_bgl_perfctr_update();
    ReleaseLock(LCK_B);
  } else {
    DBGN(4,("Interrupt handler invoked with user code in bgl_perfctr_read -> Early exit.\n"));
  }

  DBGN(4,("Handler completed at %llu\n",getTimeBase()));
  return;
}

int bgl_perfctr_setup_timed_read(int desired_mode) {
  int retval=0;
  handler_fcn *o_handler;
  struct itimerval interval={{0,},}, old_interval={{0,},};
  unsigned timeval;

  /* Set the static variables to the CNTR_STATE we use and the handler we want to use */
  DBGN(2,("Begin at %llu\n",getTimeBase()));

  o_handler=signal(SIGALRM, bgl_perfctr_intr_handler);
  if(!((o_handler==SIG_DFL)||(o_handler==SIG_IGN))) {
    if(o_handler!=SIG_ERR){
      fprintf(stderr,"%s[%d]:: A SIGALRM signal handler is already installed, o_handler=<%p>.\n"
              ,__func__,__LINE__,o_handler);
    } else {
      fprintf(stderr,"%s[%d]:: Installing the SIGALRM signal handler failed with SIGERR.\n"
              ,__func__,__LINE__);
      return -1;
    }
  }

  bgl_perfctr_curr_handler=bgl_perfctr_intr_handler;

  /* Time value based on not overflowing the current processor
   * if 1 count is generated per clock, with a safety margin of
   * 100 msec.
   */
  timeval=0xFFFFFFFFUL / get_cpu_mhz() - 100000;
  interval.it_interval.tv_sec=timeval/1000000;
  interval.it_interval.tv_usec=timeval % 1000000;
  interval.it_value=interval.it_interval;

  DBGN(1,("Timer interrupt installed at %ld.%.6lds  <%ld.%.6lds>\n",
             interval.it_interval.tv_sec,interval.it_interval.tv_usec,
             interval.it_value.tv_sec,interval.it_value.tv_usec));

  /* Make an attempt at a global synchronization
   * If the user asked for it :)
   */

  if(desired_mode == BGL_PERFCTR_MODE_SYNC) {
    // We'll make this BG/L specific. We are either one or two cores on the local node
    // If we are two, we will synchronize locally, do the barrier on one core and then
    // synchronize locally again.

    if(BGLPERFCTR_INTERNAL->user_app_core_map == 0x3) {
      DBGN(4,("Before first barrier. TB=%llu\n",getTimeBase()));
      BGL_Barrier_Pass(BARRIER);
      DBGN(4,("Passed first barrier. TB=%llu\n",getTimeBase()));
    }

    if((rts_get_processor_id()==0 ) || (BGLPERFCTR_INTERNAL->user_app_core_map == 0x2)){
      DBGN(4,("Performing Global Interrupt Barrier. TB=%llu\n",getTimeBase()));
      unsigned long long timetodie=getTimeBase() + get_cpu_mhz()*5000000LL;
      retval=BGLGI_Barrier((void *)BGL_MEM_GI_USR_BASE, 0, 1, timeoutguard, &timetodie);
      DBGN(4,("Global Interrupt Barrier completed   TB=%llu.  Status=0x%x\n",getTimeBase(),retval));
    }

    if(BGLPERFCTR_INTERNAL->user_app_core_map == 0x3) {
      DBGN(4,("Before second barrier. TB=%llu\n",getTimeBase()));
      BGL_Barrier_Pass(BARRIER);
      DBGN(4,("Passed second barrier. TB=%llu\n",getTimeBase()));
    }

    if(retval)
      desired_mode=BGL_PERFCTR_MODE_ASYNC;
  }


  DBGN(4,("Calling setitimer...\n"));
  retval=setitimer(ITIMER_REAL,&interval,&old_interval);
  if(retval) {
    fprintf(stderr,"%s[%d]:: setitimer failed with code %d: %s\n",__func__,__LINE__,
            errno,strerror(errno));
    //return -1;
  }

  if(old_interval.it_interval.tv_sec||old_interval.it_interval.tv_usec||
     old_interval.it_value.tv_sec||old_interval.it_value.tv_usec) {
    fprintf(stderr,"%s[%d]:: ITIMER_REAL is already in use.\n"
            "\t{ old_value={ %ld, %ld }, { %ld, %ld }}\n",
            __func__,__LINE__,
            old_interval.it_interval.tv_sec,old_interval.it_interval.tv_usec,
            old_interval.it_value.tv_sec,old_interval.it_value.tv_usec);
    assert(1);
  }

  DBGN(2,("Sucess at %llu\n",getTimeBase()))
  return desired_mode;
}

int timeoutguard(void *limit) {
  const unsigned long long * const bound=limit;

  if(getTimeBase()>*bound){
    fprintf(stderr,"Barrier timeout expired %llu > %llu\n",rts_get_timebase(),*bound);
    return -1;
  }
  return 0;
}

int bgl_perfctr_shutdown_timed_read(void){
  int retval=0;
  handler_fcn *o_handler;
  struct itimerval interval={{0,},}, old_interval;

  /* Stop the interval timer */
  setitimer(ITIMER_REAL,&interval,&old_interval);
  /*  It should have been an interval installed at this point
  if(old_interval.it_interval.tv_sec||old_interval.it_interval.tv_usec||
     old_interval.it_value.tv_sec||old_interval.it_value.tv_usec) {
    fprintf(stderr,"%s[%d]:: ITIMER_REAL is already in use.\n"
            "\t{ old_value={ %ld, %ld }, { %ld, %ld }}\n",
            __func__,__LINE__,
            old_interval.it_interval.tv_sec,old_interval.it_interval.tv_usec,
            old_interval.it_value.tv_sec,old_interval.it_value.tv_usec);
  }
  */

  /* Uninstall the signal handler */
  o_handler=signal(SIGALRM, SIG_DFL);
    if(o_handler!=bgl_perfctr_intr_handler)
      fprintf(stderr,"%s[%d]:: There was another signal handler installed than bgl_perfctr.\n"
              ,__func__,__LINE__);

  /* Set the static variables to the CNTR_STATE we use and the handler we want to use */
  bgl_perfctr_curr_handler=NULL;

    return retval;
}

static void __attribute__ ((constructor)) bgl_perfctr_init_remote(void)
{
  bgl_perfctr_init_synch(BGL_PERFCTR_MODE_REMOTE);
}

static void __attribute__ ((destructor))  bgl_perfctr_fini_remote(void)
{
  bgl_perfctr_shutdown();
}

void bgl_perfctr_void(void)
{
}
