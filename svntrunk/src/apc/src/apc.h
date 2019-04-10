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
 * File: "apc.h"
 */

#define APC_SETS 8


/*
 * Data structure used to report a counter value
 */

typedef struct apc_counter_data_s
{
  unsigned long long  value;

  unsigned short      node;
  unsigned short      id;

  unsigned char       core;
  unsigned char       set;
  unsigned char       edge;
  unsigned char       dummy;
}
apc_counter_data_t;



/*
 * APC C Interface
 */

extern void ApcInit();

extern void ApcFinalize();

extern void ApcStart(int set);

extern void ApcStop(int set);



/*
 * APC Fortran Interface
 */

extern void apc_init();

extern void apc_finalize();

extern void apc_start(int *set_p);

extern void apc_stop(int *set_p);



/*
 * Raw Access
 */

const char *UpcEventName(int group, int counter, int code);

void UpcFileOpen(int rank, int proc);

void UpcFileClose();

void UpcZeroCounters();

void UpcPrintCounters(int set, int start);

