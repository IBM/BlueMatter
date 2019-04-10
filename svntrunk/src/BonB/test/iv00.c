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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bonb/blade_on_blrts.h>
#include <rts.h>


//@MG: in Blrts Virtual Node Mode, the Torus virtual address mapping
//  is flipped between cores. So, core 0 sees Group 0


// I'll pre-create my headers and store them here
_BGL_TorusPktHdr ts_hdr;

Bit128 ts_payload[ 3 ];

int total_pkts = 0;
int bad_pkts = 0;
int good_pkts = 0;

int
my_buffered_receiver( _BGL_TorusPktPayload *payload,
                       Bit32                arg,
                       Bit32                extra )
   {
   total_pkts++;

   if ( extra != (Bit32) hard_processor_id() )
      {
      bad_pkts++;
      fprintf(stderr,"my_buffered_receiver(%d.%d): tot=%d, good=%d, bad=%d, arg=%ld, extra=%ld\n",
              BGLPartitionGetRank(), hard_processor_id(), total_pkts, good_pkts, bad_pkts, arg, extra );
      }
   else
      {
      good_pkts++;
      //printf("my_buffered_receiver(%d.%d): tot=%d, good=%d, bad=%d, arg=%ld, extra=%ld\n",
      //       BGLPartitionGetRank(), hard_processor_id(), total_pkts, good_pkts, bad_pkts, arg, extra );
      }

   return(0);
   }

int
do_test1( void )
   {
   int core  = hard_processor_id();
   int rank  = BGLPartitionGetRank();
   int nodes = BGLPartitionGetNumNodesCompute();
   int count = (nodes - 1);  // we'll get this many packets

   if ( !rank )
      {
      // Node 0 receives from everyone (flipped virtual mapping, so each core using different group
      BGLTorusWaitCountF0( &count );
      }
   else
      {
      memset( &ts_hdr,          0, sizeof(ts_hdr)     );
      memset( &(ts_payload[0]), -1, sizeof(ts_payload) );

      // make a header to send to same core on Node 0
      BGLTorusMakeHdr( &ts_hdr,
                       0, 0, 0,                     // destination XYZ coordinates
                       core,                        // destination Fifo Group
                       (long unsigned int) &my_buffered_receiver,       // Actor function address
                       rank,                        // primary argument to actor
                       core,                        // secondary 10bit argument to actor
                       _BGL_TORUS_ACTOR_BUFFERED ); // Actor Signature

      // printf("ts_hdr.Pid = %d\n", ts_hdr.hwh0.Pid );

      // Send the packet (flipped virtual mapping, so each core using different group
      BGLTorusInjectPacket( (_BGL_TorusFifo *)TS0_I0, &ts_hdr, ts_payload, sizeof(ts_payload) );
      }

   return(0);

   }

int
main( int argc, char *argv[], char **envp )
   {
   int rc;

   if( (rc = _blade_on_blrts_init()) )
      {
      printf("main: blade_on_blrts_init: rc = %d.\n", rc );
      exit( rc );
      }

   BGLPartitionBarrierCompute();
   printf("main: (%d.%d) past first barrier.\n", BGLPartitionGetRank(), hard_processor_id() );

   rc = do_test1();

   BGLPartitionBarrierCompute();

   printf("main: (%d.%d) done.\n", BGLPartitionGetRank(), hard_processor_id() );
   fflush( stdout );


   exit( rc );
   }

