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
 * File: "apc_scan.c"
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <apc_perfctr_events.h>

#include "apc.h"



/*
 * Macros and Typedefs
 */

#define PRINT_LEVEL_0   0
#define PRINT_LEVEL_1   1

#define CLOCK_HZ    ((double) 700000000.0)
#define EVENTS_COUNT (BGL_PERFCTR_NUMEVENTS+1) /* extra event is raw cycle count from tbr */



/*
 * Structures.
 */

typedef struct apc_stats_s
{
  unsigned long long avr;
  unsigned long long min;
  unsigned long long max;
  int                count;
  short              valid;
  short              edge;
}
apc_stats_t;




/*
 * Globals
 */

int ApcDataCount;
int ApcNodeCount;
int ApcSetCount;

apc_counter_data_t * ApcData;

apc_stats_t ApcStats[EVENTS_COUNT][APC_SETS];



/*
 * Functions defined in this file
 */

extern void Swap2(void *dst_p, void *src_p);
extern void Swap4(void *dst_p, void *src_p);
extern void Swap8(void *dst_p, void *src_p);
extern void ByteReverse(apc_counter_data_t *counter_data_p);
extern const char *ApcCounterName(int id);
extern void LoadApcData(int print);
extern void CalculateStats();



/*
 * main()
 */

int main(int argc, char **argv)
{
  /*
   * Read in counter data binary files
   */

  LoadApcData(PRINT_LEVEL_1);

  CalculateStats();
  

  /*
   * All done
   */

  exit(0);
}



/*
 * Swap routines for endian changes
 */

void Swap2(void *dst_p, void *src_p)
{
  char *d_p, *s_p;
  
  d_p = dst_p;
  s_p = src_p;
  
  d_p[0] = s_p[1];
  d_p[1] = s_p[0];
}

void Swap4(void *dst_p, void *src_p)
{
  char *d_p, *s_p;
  
  d_p = dst_p;
  s_p = src_p;
  
  d_p[0] = s_p[3];
  d_p[1] = s_p[2];
  d_p[2] = s_p[1];
  d_p[3] = s_p[0];
}

void Swap8(void *dst_p, void *src_p)
{
  char *d_p, *s_p;
  
  d_p = dst_p;
  s_p = src_p;
  
  d_p[0] = s_p[7];
  d_p[1] = s_p[6];
  d_p[2] = s_p[5];  
  d_p[3] = s_p[4];
  d_p[4] = s_p[3];
  d_p[5] = s_p[2];
  d_p[6] = s_p[1];
  d_p[7] = s_p[0];
}

void ByteReverse(apc_counter_data_t *counter_data_p)
{
  apc_counter_data_t new_counter_data;
  
  Swap2(&new_counter_data.id, &counter_data_p->id);
  Swap2(&new_counter_data.edge, &counter_data_p->edge);
  Swap2(&new_counter_data.set, &counter_data_p->set);
  Swap2(&new_counter_data.node, &counter_data_p->node);
  Swap8(&new_counter_data.value, &counter_data_p->value);
  
  *counter_data_p = new_counter_data;

  return;
}



/*
 * ApcCounterName(id)
 *
 *      Gets the counter name from its id.
 */

const char *ApcCounterName(int id)
{
  if ( id >= 0 && id < BGL_PERFCTR_NUMEVENTS )
  {
    char * name_p;
    name_p = (char *) BGL_PERFCTR_event_table[id].event_name;
    if ( ( name_p[0] == 'B' )
         && ( name_p[1] == 'G' )
         && ( name_p[2] == 'L' )
         && ( name_p[3] == '_' ) )
      return name_p+4;
    else
      return name_p;
  }
  else if ( id == BGL_PERFCTR_NUMEVENTS )
  {
    return "CYCLE_COUNT";
  }
  else
  {
    return "BGL_UNDEFINED";
  }
}



/*
 * void LoadApcData()
 *
 *      This routine loads raw counter data from binary files 
 *      named apc.00000.0, apc.00000.1, ...
 */

void LoadApcData(int print)
{
  char name[128];
  int node, core, node_count, data_count, this_data_count, set_count = 0;
  apc_counter_data_t * data_p;
  

  /*
   * Print 
   */

  printf("=== Loading counter data\n");


  /*
   * This is stupid way to find how many counter files, and to get the total size of data.
   */

  printf("  Scanning for files\n");
  
  node_count = 0;
  data_count = 0;

  while ( 1 )
  {
    struct stat stat_buf;
    
    // core 0 data

    sprintf(name, "apcdata/apc.%5.5d.0", node_count);

    if ( stat(name, &stat_buf) )
    {
      // ok, failed to find core 0 file, so all done.
      break;
    }

    if ( stat_buf.st_size % sizeof(apc_counter_data_t) )
    {
      printf("  LoadApcData: File \"%s\" has incorrect size, should be a multiple of %d\n", 
             name, sizeof(apc_counter_data_t));
      exit(1);
    }

    data_count += stat_buf.st_size/sizeof(apc_counter_data_t);
    
    // core 1 data, may or may not be present

    sprintf(name, "apcdata/apc.%5.5d.1", node_count);

    if ( ! stat(name, &stat_buf) )
    {
      if ( stat_buf.st_size % sizeof(apc_counter_data_t) )
      {
        printf("  LoadApcData: File \"%s\" has incorrect size, should be a multiple of %d\n", 
               name, sizeof(apc_counter_data_t));
        exit(1);
      }

      data_count += stat_buf.st_size/sizeof(apc_counter_data_t);
    }
    
    node_count ++;
  }
  

  /*
   * Allocate data structure to read counter data.
   */

  ApcData = malloc(data_count*sizeof(apc_counter_data_t));


  /*
   * Read in the data files
   */

  data_p = ApcData;

  this_data_count = 0;
  
  for ( node = 0; node < node_count; node++ )
  {
    FILE *file_p;
    apc_counter_data_t this_data;
    
    for ( core = 0; core < 2; core ++ )
    {
      sprintf(name, "apcdata/apc.%5.5d.%1.1d", node, core);
      
      if ( print > PRINT_LEVEL_0 )
        printf("    reading %s\n", name);
      
      file_p = fopen(name, "r");

      if ( file_p )
      {
        int this_file_count;
        
        this_file_count = 0;

        while ( fread(&this_data, sizeof(apc_counter_data_t), 1, file_p) )
        {
          this_data_count ++;

          if ( this_data_count > data_count )
          {
            printf("  LoadApcData: have exceeded expected record count\n");
            exit(1);
          }
            
          this_file_count ++;

          *data_p = this_data;

          if ( set_count <= this_data.set )
            set_count = this_data.set+1;
          
          data_p ++;
        }
        fclose(file_p);
      }
    }
  }

  if ( this_data_count != data_count )
  {
    printf("  LoadApcData: failed to read expected count of records\n");
    exit(1);
  }
  
  /*
   * Save Data and Node count in global variables
   */

  ApcNodeCount = node_count;
  ApcSetCount  = set_count;
  ApcDataCount = data_count;
  
  printf("  Nodes: %d, Sets: %d, Records: %d\n\n",
         ApcNodeCount, ApcSetCount, ApcDataCount);
  

  /*
   * Print 
   */

  if ( print > PRINT_LEVEL_0 )
  {
    int rec;
    
    printf("  Raw Performance Counter Data:\n");
    
    for ( rec = 0; rec < ApcDataCount; rec++ )
    {
      apc_counter_data_t *data_p;
      
      data_p = &ApcData[rec];
      
      printf("  %20llu %16llx [%2d %1d %5d] [%3d %d] %s\n",
             data_p->value,
             data_p->value,
             data_p->set,
             data_p->core,
             data_p->node, 
             data_p->id,
             data_p->edge,
             ApcCounterName(data_p->id));

    }
    
    printf("\n");
  }
  
  

  return;
}



/*
 * Calculate Stats
 */

void CalculateStats()
{
  int e, s;
  
  for ( e = 0; e < EVENTS_COUNT; e++ )
    for ( s = 0; s < ApcSetCount; s++ )
    {
      int count, edge, r;
      unsigned long long min=0, max=0, sum=0;

      count = 0;
      edge = -1;
      min = 0xffffffffffffffffULL;
      max = 0;
      sum = 0;
      
      for ( r = 0; r < ApcDataCount; r++ )
      {
        if ( (ApcData[r].id == e) && (ApcData[r].set == s) )
        {
          if ( edge == -1 )
          {
            edge = ApcData[r].edge;
          }
          else
          {
            if ( edge != ApcData[r].edge )
            {
              printf("Calculate Stats: differing edge values\n");
            }
          }
          
          if ( min > ApcData[r].value )
            min = ApcData[r].value;
          
          if ( max < ApcData[r].value )
            max = ApcData[r].value;
          
          sum += ApcData[r].value;
          
          count += 1;
        }
      }

      if ( count > 0 )
      {
        ApcStats[e][s].valid = 1;
        ApcStats[e][s].count = count;
        ApcStats[e][s].edge = edge;
        ApcStats[e][s].avr = sum / count;
        ApcStats[e][s].min = min;
        ApcStats[e][s].max = max;
      }
      else
      {
        ApcStats[e][s].valid = 0;
        ApcStats[e][s].count = 0;
        ApcStats[e][s].edge = 0;
        ApcStats[e][s].avr = 0;
        ApcStats[e][s].min = 0;
        ApcStats[e][s].max = 0;
      }

      printf("  %3d %d %d: %20lld %20lld %20lld %2d %d %s\n",
             e, s,
             ApcStats[e][s].valid,
             ApcStats[e][s].avr,
             ApcStats[e][s].min,
             ApcStats[e][s].max,
             ApcStats[e][s].count,
             ApcStats[e][s].edge,
             ApcCounterName(e));
    }
}

