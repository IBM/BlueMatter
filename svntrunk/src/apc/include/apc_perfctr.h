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
/* (C)Copyright IBM Corp. 2004, 2004                              */
/* All rights reserved.                                           */
/* US Government Users Restricted Rights -                        */
/* Use, duplication or disclosure restricted                      */
/* by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                */
/* Licensed Materials-Property of IBM                             */
/* -------------------------------------------------------------- */

#ifndef APC_PERFCTR_H
#define APC_PERFCTR_H

#include <stdio.h>
#include <rts.h>
<<<<<<< .mine
#include <apc_perfctr_events.h>
=======
#include "apc_perfctr_events.h"
>>>>>>> .r14839
#if defined(__cplusplus)
  extern "C" {
#endif

#define BGL_PERFCTR_NUM_COUNTERS 52
#define BGL_PERFCTR_NUM_CTRL 18
#define getTimeBase rts_get_timebase

/* bgl_perfctr run-modes */
#define BGL_PERFCTR_MODE_LOCAL    0  /* Completely local - No interval timer reads */
#define BGL_PERFCTR_MODE_ASYNC  100  /* Completely local - Local timer used to read counters */
#define BGL_PERFCTR_MODE_SYNC   200  /* Local timers started after global barrier */


typedef struct BGL_PERFCTR_event {
  BGL_PERFCTR_event_num_t num;
  unsigned int edge;
} BGL_PERFCTR_event_t;

typedef struct bgl_perfctr_control_map {
  BGL_PERFCTR_event_t event;
/* counter_register is location in of value in virtual array */
  int counter_register;
  int cntrl_register;
  int ref_count;
  int new_count;
} bgl_perfctr_control_map_t;

typedef struct bgl_perfctr_remote_flag {
  unsigned collect:1;
} bgl_perfctr_remote_flag_t;

typedef struct bgl_perfctr_control {
  volatile unsigned int gen_end;
  unsigned int defn_id;
  /* Bit pattern (one bit per counter register) */
  unsigned long long in_use;
  /* Bit pattern (one bit per counter control register (i.e. group) */
  unsigned long long modified;

  /* the stuff for external polling: */
  /* moved latest_update from the end so I can grab less */
  volatile long long latest_update;
  bgl_perfctr_remote_flag_t remote_flag;

  unsigned long long vdata[BGL_PERFCTR_NUM_COUNTERS];
  volatile unsigned int gen_start;
  unsigned int ctrl[BGL_PERFCTR_NUM_CTRL];
  unsigned int last[BGL_PERFCTR_NUM_COUNTERS];
  int nmapped;
  bgl_perfctr_control_map_t map[BGL_PERFCTR_NUM_COUNTERS];
} bgl_perfctr_control_t;

/*****************************************************/
/*               Basic functions                     */
/*****************************************************/
/*
 *  NOTE: bgl_perfctr_init_synch is now core-synchronous.
 *        When in VNM, It must be called from both cores to pass.
 */
int bgl_perfctr_init_synch(int mode);
int bgl_perfctr_shutdown(void);

int bgl_perfctr_add_event(BGL_PERFCTR_event_t event);
int bgl_perfctr_remove_event(BGL_PERFCTR_event_t event);
int bgl_perfctr_commit(void);
int bgl_perfctr_revoke(void);
int bgl_perfctr_add_remote_counters(void);


/* Print out the internal state of the library to the provided file handle */
int bgl_perfctr_dump_state(FILE *fh);

/* Make an update of the virtual counters from the hardware counters */
int bgl_perfctr_update(void);

/* Update the virtual counters and return a copy of the virtual counters 
 * The target array needs to be at least unsigned long long[BGL_PERFCTR_NUM_COUNTERS]
 */
int bgl_perfctr_copy_counters(unsigned long long values[], size_t size_of_values);

/* Update the virtual counters and return a copy of the complete state of the API */
int bgl_perfctr_copy_state(bgl_perfctr_control_t *hw_state, size_t size_of_buffer);

/*****************************************************/
/*               Advanced functions                  */
/*****************************************************/

/* Give me the pointer to the shared data structure */
bgl_perfctr_control_t *bgl_perfctr_hwstate(void);

/* Lock the virtual counters, update them with the current value and return
 * the pointer to the shared virtual counters array
 * You'd better very soon release the counters again....
 * Automatic updates are skipped while the counters are locked. Access to 
 * the counters by other means may be blocked.
 */
/* static unsigned long long *bgl_perfctr_get_counters(void); */
/* Release the lock on the counters (see previous) */
/* static void bgl_perfctr_release_counters(void); */

#if defined(__cplusplus)
  }
#endif
#endif /* BGL_PERFCTR_H */
