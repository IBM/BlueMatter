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
 * File: "apc_raw.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <apc_perfctr.h>

#include <apc.h>

/*
 * Defines
 */

#define UPC_MEM_BASE  (0xB0010000)
#define UPC_DCR_BASE  (0x180)

#define UPC_CTR_A     (0)
#define UPC_CTR_B     (1)
#define UPC_CTR_C     (2)
#define UPC_CTR_CTRL  (3)


static FILE *UpcFile_p = 0;

static unsigned int UpcCountersMem[2][16][4];
static unsigned int UpcCountersDCR[2][16][4];

const char *UpcEventName(int group, int counter, int code)
{
  int e, f, matches;
  const char *name_p = 0;
  
  matches = 0;
  
  for ( e = 0; e < BGL_PERFCTR_NUMEVENTS; e++ )
  {
    for ( f = 0; f < BGL_PERFCTR_event_table[e].num_encodings; f++ )
    {
      if (  (BGL_PERFCTR_event_table[e].encoding[f].group == group)
	    && (BGL_PERFCTR_event_table[e].encoding[f].counter == counter)
	    && (BGL_PERFCTR_event_table[e].encoding[f].code == code ) )
      {
	matches ++;
	name_p = BGL_PERFCTR_event_table[e].event_name;
      }
    }
  }
  
  if ( matches == 0 )
    return "(none)";
  else if ( matches == 1 )
    return name_p;
  else
    return "(multiple)";
}


void UpcFileOpen(rank, proc)
{
  char name[32];
  
  sprintf(name, "apc.%5.5d.%1.1d.check", rank, proc);
  UpcFile_p = fopen(name, "w");
  if (!UpcFile_p)
  {
    fprintf(stderr, "UpcFileOpen: error opening file %s for dcr counter output\n", name);
    exit(1);
  }

  return;
}

void UpcFileClose()
{
  fclose(UpcFile_p);

  return;
}

void UpcZeroCounters()
{
  int c, i;
  unsigned status;

  status = 0;
  
  for ( c = 0; c < 16; c++)
    for ( i = 0; i < 4; i ++ )
    {
      status |= rts_write_dcr(UPC_DCR_BASE+c*4+i, 0);
    }

  for ( c = 0; c < 16; c++)
    for ( i = 0; i < 4; i ++ )
    {
      status |= rts_write_dcr(UPC_DCR_BASE+c*4+i, 0);
    }

  if ( status )
  {
    printf("Error writing dcr\n");
    exit(0);
  }
  
  return;
}


void UpcPrintCounters(int set, int start)
{
  int c, i;
  unsigned *m_p, *d_p, *s_p;
  
  m_p = &UpcCountersMem[0][0][0];
  d_p = &UpcCountersDCR[0][0][0];
  s_p = ((unsigned *) UPC_MEM_BASE);
  
  for ( c = 0; c < 16; c++ )
    for ( i = 0; i < 4; i++ )
    {
      *m_p ++ = *s_p ++;
      rts_read_dcr(UPC_DCR_BASE+c*4+i, d_p++);
    }
  

  m_p = &UpcCountersMem[1][0][0];
  d_p = &UpcCountersDCR[1][0][0];
  s_p = ((unsigned *) UPC_MEM_BASE);
  
  for ( c = 0; c < 16; c++ )
    for ( i = 0; i < 4; i++ )
    {
      *m_p ++ = *s_p ++;
      rts_read_dcr(UPC_DCR_BASE+c*4+i, d_p++);
    }
  

  fprintf(UpcFile_p, "  Mem/DCR values:\n");
  for ( c = 0; c < 16; c++ )
  {
    unsigned ctrl;
    unsigned a_edge, a_irq, a_event;
    unsigned b_edge, b_irq, b_event;
    unsigned c_edge, c_irq, c_event, c_mode;
    const char *a_name, *b_name, *c_name;
    
    fprintf(UpcFile_p, 
	    "    %2d: %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x\n",
	    c,
	    UpcCountersMem[0][c][0], UpcCountersDCR[0][c][0], 
	    UpcCountersMem[0][c][1], UpcCountersDCR[0][c][1],
	    UpcCountersMem[0][c][2], UpcCountersDCR[0][c][2],
	    UpcCountersMem[0][c][3], UpcCountersDCR[0][c][3]);
    fprintf(UpcFile_p, 
	    "    %2d: %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x\n",
	    c,
	    UpcCountersMem[1][c][0], UpcCountersDCR[1][c][0], 
	    UpcCountersMem[1][c][1], UpcCountersDCR[1][c][1],
	    UpcCountersMem[1][c][2], UpcCountersDCR[1][c][2],
	    UpcCountersMem[1][c][3], UpcCountersDCR[1][c][3]);

    ctrl = UpcCountersMem[0][c][3];
    
    a_edge  = (ctrl & 0xc0000000) >> 30;
    a_irq   = (ctrl & 0x3c000000) >> 26;
    a_event = (ctrl & 0x03e00000) >> 21;

    b_edge  = (ctrl & 0x00180000) >> 19;
    b_irq   = (ctrl & 0x00070000) >> 16;
    b_event = (ctrl & 0x0000f000) >> 12;

    c_edge  = (ctrl & 0x00000c00) >> 10;
    c_irq   = (ctrl & 0x000003c0) >>  6;
    c_event = (ctrl & 0x0000003e) >>  1;
    c_mode  = (ctrl & 0x00000001);
    
    a_name = UpcEventName(c, 0, a_event);
    b_name = UpcEventName(c, 1, b_event);
    c_name = UpcEventName(c, 2, c_event);

    fprintf(UpcFile_p, "    %2d: [%d,%d,%d]%s  [%d,%d,%d]%s  [%d,%d,%d]%s\n", 
	    c, 
	    c, 0, a_event, a_name, 
	    c, 1, b_event, b_name, 
	    c, 2, c_event, c_name);

    if ( UpcCountersMem[0][c][1] == 0xffffffff )
      fprintf(UpcFile_p, "  B counter overflow: %s\n", b_name);
  }
  
  #if 0
  fprintf(UpcFile_p, "Counter DCR: %s(%d)\n",
	  (start ? "ApcStart" : "ApcStop"),
	  set);

  for ( i = 0; i < 16; i++ )
  {
    unsigned int ctrl;
    unsigned int a_edge, a_irq, a_event;
    unsigned int b_edge, b_irq, b_event;
    unsigned int c_edge, c_irq, c_event, c_mode;
    const char * a_name, * b_name, * c_name;

    ctrl = UpcCountersMem[i][UPC_CTR_CTRL];
    
    a_edge  = (ctrl & 0xc0000000) >> 30;
    a_irq   = (ctrl & 0x3c000000) >> 26;
    a_event = (ctrl & 0x03e00000) >> 21;

    b_edge  = (ctrl & 0x00180000) >> 19;
    b_irq   = (ctrl & 0x00070000) >> 16;
    b_event = (ctrl & 0x0000f000) >> 12;

    c_edge  = (ctrl & 0x00000c00) >> 10;
    c_irq   = (ctrl & 0x000003c0) >>  6;
    c_event = (ctrl & 0x0000003e) >>  1;
    c_mode  = (ctrl & 0x00000001);
    
    a_name = UpcEventName(i, 0, a_event);
    b_name = UpcEventName(i, 1, b_event);
    c_name = UpcEventName(i, 2, c_event);

    fprintf(UpcFile_p, 
	    "M%2d: %8.8x [%x.%x.%2d](%10u) [%x.%x.%2d](%10u)[%x.%x.%2d.%x](%10u) %s,%s,%s\n",
	    i,
	    UpcCountersMem[i][UPC_CTR_CTRL],
	    a_edge, a_irq, a_event,
	    UpcCountersMem[i][UPC_CTR_A],
	    b_edge, b_irq, b_event,
	    UpcCountersMem[i][UPC_CTR_B],
	    c_edge, c_irq, c_event, c_mode,
	    UpcCountersMem[i][UPC_CTR_C],
	    a_name, b_name, c_name);


    #if 1
    ctrl = UpcCountersDCR[i][UPC_CTR_CTRL];
    
    a_edge  = (ctrl & 0xc0000000) >> 30;
    a_irq   = (ctrl & 0x3c000000) >> 26;
    a_event = (ctrl & 0x03e00000) >> 21;

    b_edge  = (ctrl & 0x00180000) >> 19;
    b_irq   = (ctrl & 0x00070000) >> 16;
    b_event = (ctrl & 0x0000f000) >> 12;

    c_edge  = (ctrl & 0x00000c00) >> 10;
    c_irq   = (ctrl & 0x000003c0) >>  6;
    c_event = (ctrl & 0x0000003e) >>  1;
    c_mode  = (ctrl & 0x00000001);
    
    a_name = UpcEventName(i, 0, a_event);
    b_name = UpcEventName(i, 1, b_event);
    c_name = UpcEventName(i, 2, c_event);

    fprintf(UpcFile_p, 
	    "D%2d: %8.8x [%x.%x.%2d](%10u) [%x.%x.%2d](%10u)[%x.%x.%2d.%x](%10u) %s,%s,%s\n",
	    i,
	    UpcCountersDCR[i][UPC_CTR_CTRL],
	    a_edge, a_irq, a_event,
	    UpcCountersDCR[i][UPC_CTR_A],
	    b_edge, b_irq, b_event,
	    UpcCountersDCR[i][UPC_CTR_B],
	    c_edge, c_irq, c_event, c_mode,
	    UpcCountersDCR[i][UPC_CTR_C],
	    a_name, b_name, c_name);
    #endif
  }
  fprintf(UpcFile_p, "\n");
  #endif
  
  return;
}

