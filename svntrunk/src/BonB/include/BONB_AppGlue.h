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
 
#if 0
#include <bonb/blade_on_blrts.h>
#endif
#include <assert.h>

__BEGIN_DECLS

static void _ts_GetStatus0( _BGL_TorusFifoStatus *stat )
{
//   assert( ((unsigned long)stat & 0x000000F) == 0 );
//   assert( stat );
//for( int i=0; i<10; i++ )
//  {
   asm volatile( "msync;"
                 "lfpdx   0,0,%0;"
                 "stfpdx  0,0,%1;"
                 :
                 : "b" (TS0_S0),
                   "r" (stat)
                 : "memory", "fr0" );
//  }
}

static void _ts_GetStatus1( _BGL_TorusFifoStatus *stat )
{
//   assert( ((unsigned long)stat & 0x000000F) == 0 );
//   assert( stat );
/* for( int i=0; i<10; i++ ) */
/*   { */
   asm volatile( "msync;"
                 "lfpdx   0,0,%0;"
                 "stfpdx  0,0,%1;"
                 :               // TS1_S1 also worked indicating a double inversion
                 : "b" (TS0_S1), // Mark says this isn't right.  It should be TS1_S0.  Mark thinks there might be a typo in the TLBswhere the fifos are right and the status wrong.  inconsistent
                   "r" (stat)
                 : "memory", "fr0" );
//   }
}

static void _ts_GetStatusBoth( _BGL_TorusFifoStatus *stat0, _BGL_TorusFifoStatus *stat1 )
{

//assert( 0 );

  assert( ((unsigned long)stat0 & 0x000000F) == 0 );
  assert( stat0 );

  assert( ((unsigned long)stat1 & 0x000000F) == 0 );
  assert( stat1 );

   asm volatile( "msync;"
                 "lfpdx   0,0,%0;"
                 "lfpdx   1,0,%2;"
                 "stfpdx  0,0,%1;"
                 "stfpdx  1,0,%3;"
                 :
                 : "b" (TS0_S0),
                   "r" (stat0),
                   "b" (TS0_S1),
                   "r" (stat1)
                 : "memory", "fr0", "fr1" );
}

////////////////////////////////////////////////////////////////////////
////////////////////////// SEND SIDE ///////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Set hints for choosing an injection Fifo on Group 0
//
//  This is very similar to a Hint Bit calculation.
//
static void _ts_MakeSendHintsF0( _BGL_TorusPktHdr *hdr )
{
   int fifos = 0;              // result
   int hints = hdr->hwh0.Hint; // get current hint bits

   // if the packet is HP, the choice is obvious.
   if ( hdr->hwh0.VC == _BGL_TORUS_PKT_VC_BP )
      {
      hdr->hwh1.Resvd0 = (_BGL_TORUS_FIFO_G0IH);
      return;
      }

   // if no hints were set, do the calculation.
   if ( !hints )
      hints = BGLTorusCalcHints( hdr->hwh0.X, hdr->hwh1.Y, hdr->hwh1.Z );

   // if *still* no hints, than all normal fifos are good (must be sending to self)
   if ( !hints )
      {
      fifos |= (_BGL_TORUS_FIFO_G0I0 | _BGL_TORUS_FIFO_G0I1 | _BGL_TORUS_FIFO_G0I2);
      }
   else
      {
      // based on direction, indicate suitable fifos: X+/- in same fifo, Y+/- in same fifo, etc.
      if ( hints & (_BGL_TORUS_PKT_HINT_XP | _BGL_TORUS_PKT_HINT_XM) ) fifos |= _BGL_TORUS_FIFO_G0I0;
      if ( hints & (_BGL_TORUS_PKT_HINT_YP | _BGL_TORUS_PKT_HINT_YM) ) fifos |= _BGL_TORUS_FIFO_G0I1;
      if ( hints & (_BGL_TORUS_PKT_HINT_ZP | _BGL_TORUS_PKT_HINT_ZM) ) fifos |= _BGL_TORUS_FIFO_G0I2;
      }

   // return with bitmask of suitable fifos set into the reserved 0 field.
   hdr->hwh1.Resvd0 = fifos;
}


// Simple H/W + S/W Torus Header Creation using suitable defaults.  NON-ACTOR, Application Receiver!!
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Fifo Group 0,
//                  Dynamic on VCD0,
//                  Hardware applied Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
static _BGL_TorusPktHdr *BGLTorusMakeHdrApp(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     Bit32 arg0,            // first  argument
                     Bit32 arg1,            // second argument
                     int payload_bytes )    // bytes in payload, I convert to chunks
{
   int chunks = ((payload_bytes + 15) >> 5);

   assert( (payload_bytes >= 0) && (payload_bytes <= 240) );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = 0;                            // h/w will apply hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_POINT2POINT;   // Point to Point Packet
   hdr->hwh0.Pid       = hard_processor_id();          // Destination Fifo Group
   hdr->hwh0.Chunks    = chunks;                       // calculated chunks
   hdr->hwh0.SW_Avail  = _BGL_TORUS_ACTOR_NONE;        // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = dest_x;                       // Destination X coord

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y;                       // Destination Y coord
   hdr->hwh1.Z         = dest_z;                       // Destination Z coord

   // fill out software header words 0 and 1
   hdr->swh0.arg0      = arg0;
   hdr->swh1.arg       = arg1;

   return( hdr );
}

//  NON-ACTOR, Application Receiver!!
//  We use reserved fields to hold the min/max coords in the line.
//  The send function will edit X,Y or Z and hint bits as needed,
//   and will inject the packet once or twice.
static _BGL_TorusPktHdr *BGLTorusMakeHdrAppDeposit(
    _BGL_TorusPktHdr *hdr, // Filled in on return
    Bit32 arg0,            // first  argument
    Bit32 arg1,            // second argument
    int payload_bytes )    // bytes in payload, I convert to chunks
{
   int chunks = ((payload_bytes + 15) >> 5);

   assert( (payload_bytes >= 0) && (payload_bytes <= 240) );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = 0;                            // h/w will apply hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Deposit Packet
   hdr->hwh0.Pid       = hard_processor_id();          // Destination Fifo Group
   hdr->hwh0.Chunks    = chunks;                       // calculated chunks
   hdr->hwh0.SW_Avail  = _BGL_TORUS_ACTOR_NONE;        // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0

   // fill out software header words 0 and 1
   hdr->swh0.arg0      = arg0;
   hdr->swh1.arg       = arg1;

   return( hdr );
}

// Simple H/W + S/W Torus Header Creation using suitable defaults.  NON-ACTOR, Application Receiver!!
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Fifo Group 0,
//                  Dynamic on VCD0,
//                  Hardware applied Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
static _BGL_TorusPktHdr *BGLTorusMakeHdrAppChooseRecvFifo(
    _BGL_TorusPktHdr *hdr, // Filled in on return
    int dest_x,            // destination coordinates
    int dest_y,
    int dest_z,
    Bit32 arg0,            // first  argument
    Bit32 arg1,            // second argument
    int payload_bytes,     // bytes in payload, I convert to chunks
    int RecvFifoPicker )   // either { 0, 1 } picks either fifos0 or fifos1 to recv
{
   int chunks = ((payload_bytes + 15) >> 5);

   assert( (payload_bytes >= 0) && (payload_bytes <= 240) );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = 0;                            // h/w will apply hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_POINT2POINT;   // Point to Point Packet
   hdr->hwh0.Pid       = RecvFifoPicker;               // Which ever core calls this, the packet is going
                                                       // to destination core 1.
   hdr->hwh0.Chunks    = chunks;                       // calculated chunks
   hdr->hwh0.SW_Avail  = _BGL_TORUS_ACTOR_NONE;        // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = dest_x;                       // Destination X coord

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y;                       // Destination Y coord
   hdr->hwh1.Z         = dest_z;                       // Destination Z coord

   // fill out software header words 0 and 1
   hdr->swh0.arg0      = arg0;
   hdr->swh1.arg       = arg1;

   return( hdr );
}

// Choose a Low Priority Fifo: NOTE BLAKE IS FORCING X+ and X- etc, INTO SAME FIFO!!! (that's bad)
static _BGL_TorusFifo *_ts_CheckClearToSendOnLink0( _BGL_TorusFifoStatus *stat, int fifo_number, int chunks );
static _BGL_TorusFifo *_ts_CheckClearToSendOnLink( _BGL_TorusFifoStatus *stat, int fifo_number, int chunks )
{
return(_ts_CheckClearToSendOnLink0( stat, fifo_number, chunks ) );
}

static _BGL_TorusFifo *_ts_GetNextFifoCheckClearToSendOnLink0( _BGL_TorusFifoStatus *stat, int chunks )
{
   int tot_c = (chunks + 1);

   if ( (tot_c + stat->i0) <= _BGL_TORUS_FIFO_CHUNKS )
     {
     stat->i0 += tot_c;
     return( (_BGL_TorusFifo *)TS0_I0 );
     }
   else if ( (tot_c + stat->i1) <= _BGL_TORUS_FIFO_CHUNKS )
     {
     stat->i1 += tot_c;
     return( (_BGL_TorusFifo *)TS0_I1 );

     }
   else if ( (tot_c + stat->i2) <= _BGL_TORUS_FIFO_CHUNKS )
     {
     stat->i2 += tot_c;
     return( (_BGL_TorusFifo *)TS0_I2 );
     }

   return( (_BGL_TorusFifo *)0 );
}

static _BGL_TorusFifo *_ts_GetNextFifoCheckClearToSendOnLink1( _BGL_TorusFifoStatus *stat, int chunks )
{
   int tot_c = (chunks + 1);

   if ( (tot_c + stat->i0) <= _BGL_TORUS_FIFO_CHUNKS )
     {
     stat->i0 += tot_c;
     return( (_BGL_TorusFifo *)TS1_I0 );
     }
   else if ( (tot_c + stat->i1) <= _BGL_TORUS_FIFO_CHUNKS )
     {
     stat->i1 += tot_c;
     return( (_BGL_TorusFifo *)TS1_I1 );

     }
   else if ( (tot_c + stat->i2) <= _BGL_TORUS_FIFO_CHUNKS )
     {
     stat->i2 += tot_c;
     return( (_BGL_TorusFifo *)TS1_I2 );
     }

   return( (_BGL_TorusFifo *)0 );
}

static _BGL_TorusFifo *_ts_CheckClearToSendOnLink0( _BGL_TorusFifoStatus *stat, int fifo_number, int chunks )
{
#if 0
   if( rts_get_processor_id() != 0 )
     {
     char* message = "rts_get_processor_id() != 0";
     write( 1, message, strlen( message ) );
     assert( 0 );
     char* abort = NULL;
     abort[ 0 ] = 0;
     for(;;);
     }

   _BGL_TorusFifoStatus FifoStatus;
   _ts_GetStatus0( & FifoStatus );
   *stat = FifoStatus;
#endif

   int tot_c = (chunks + 1);

   if ( fifo_number == 0 )
      {
      if ( (tot_c + stat->i0) <= _BGL_TORUS_FIFO_CHUNKS )
         {
         stat->i0 += tot_c;
         return( (_BGL_TorusFifo *)TS0_I0 );
         }
      }
   else if ( fifo_number == 1 )
      {
      if ( (tot_c + stat->i1) <= _BGL_TORUS_FIFO_CHUNKS )
         {
         stat->i1 += tot_c;
         return( (_BGL_TorusFifo *)TS0_I1 );
         }
      }
   else if ( fifo_number == 2 )
      {
      if ( (tot_c + stat->i2) <= _BGL_TORUS_FIFO_CHUNKS )
         {
         stat->i2 += tot_c;
         return( (_BGL_TorusFifo *)TS0_I2 );
         }
      }

   return( (_BGL_TorusFifo *)0 );
}

static _BGL_TorusFifo *_ts_CheckClearToSendOnLink1( _BGL_TorusFifoStatus *stat, int fifo_number, int chunks )
{
#if TORUS_CHECK
   if( rts_get_processor_id() != 1 )
     {
     char* message = "rts_get_processor_id() != 1";
     write( 1, message, strlen( message ) );
     assert( 0 );
     char* abort = NULL;
     abort[ 0 ] = 0;
     for(;;);
     }

   _BGL_TorusFifoStatus FifoStatus;
   _ts_GetStatus1( & FifoStatus );
   *stat = FifoStatus;
#endif

   int tot_c = (chunks + 1);

   if ( fifo_number == 0 )
      {
      if ( (tot_c + stat->i0) <= _BGL_TORUS_FIFO_CHUNKS )
         {
         stat->i0 += tot_c;
         return( (_BGL_TorusFifo *)TS1_I0 );
         }
      }
   else if ( fifo_number == 1 )
      {
      if ( (tot_c + stat->i1) <= _BGL_TORUS_FIFO_CHUNKS )
         {
         stat->i1 += tot_c;
         return( (_BGL_TorusFifo *)TS1_I1 );
         }
      }
   else if ( fifo_number == 2 )
      {
      if ( (tot_c + stat->i2) <= _BGL_TORUS_FIFO_CHUNKS )
         {
         stat->i2 += tot_c;
         return( (_BGL_TorusFifo *)TS1_I2 );
         }
      }

   return( (_BGL_TorusFifo *)0 );
}

extern _BGL_TorusPktHdr expect0;
extern _BGL_TorusPktHdr expect1;

extern _BGL_TorusPktHdr mask;

static void InitHeader( _BGL_TorusPktHdr* hdr_check, int aCoreId )
  {
  BGLTorusMakeHdrAppChooseRecvFifo( hdr_check,
				     0, 0, 0, 0, 0, 0, aCoreId );


  }

static void BGLTorusInjectPacketImageApp(
        _BGL_TorusFifo *fifo,  // Injection FIFO to use (Must have space available!)
        _BGL_TorusPkt  *pkt )  // Address of ready-to-go packet
{
   Bit8 *dst = (Bit8 *)pkt;
   _BGL_TorusPktHdr *hdr = &(pkt->hdr);
   int chunks = hdr->hwh0.Chunks, c2;
   register int u = 16;

   // Check to see if the header isn't complete junk.
   int tot_c = chunks + 1;
#if 0
   unsigned int* IntHdrPtr = (unsigned int *) hdr;
   unsigned int* MaskPtr   = (unsigned int *) & mask;
   unsigned int* ExpectPtr = (unsigned int *) & expect0;

   if( (MaskPtr[ 0 ] & IntHdrPtr[ 0 ]) ^ ExpectPtr[ 0 ] )
     {
       printf( "line: Mask: %08X Hdr: %08X Expext: %08X And: %08X Xor: %08X\n",
	       MaskPtr[ 0 ], IntHdrPtr[ 0 ], ExpectPtr[ 0 ],
	       (MaskPtr[ 0 ] & IntHdrPtr[ 0 ]),
	       ( (MaskPtr[ 0 ] & IntHdrPtr[ 0 ]) ^ ExpectPtr[ 0 ] ) );

       assert( 0 );
     }

   if( (MaskPtr[ 1 ] & IntHdrPtr[ 1 ]) ^ ExpectPtr[ 1 ] )
     {
       printf( "line: Mask: %08X Hdr: %08X Expext: %08X And: %08X Xor: %08X\n",
	       MaskPtr[ 1 ], IntHdrPtr[ 1 ], ExpectPtr[ 1 ],
	       (MaskPtr[ 1 ] & IntHdrPtr[ 1 ]),
	       ( (MaskPtr[ 1 ] & IntHdrPtr[ 1 ]) ^ ExpectPtr[ 1 ] ) );

       assert( 0 );
     }
#endif
#if 0
  _BGL_TorusFifoStatus FifoStatus;
  int core = rts_get_processor_id();
  if( core == 0 )
    {
      unsigned int* ExpectPtr = (unsigned int *) & expect0;

      if( (MaskPtr[ 0 ] & IntHdrPtr[ 0 ]) ^ ExpectPtr[ 0 ] )
	{
        printf( "line: Mask: %08X Hdr: %08X Expext: %08X And: %08X Xor: %08X\n",
		MaskPtr[ 0 ], IntHdrPtr[ 0 ], ExpectPtr[ 0 ],
		(MaskPtr[ 0 ] & IntHdrPtr[ 0 ]),
		( (MaskPtr[ 0 ] & IntHdrPtr[ 0 ]) ^ ExpectPtr[ 0 ] ) );

	assert( 0 );
	}

      if( (MaskPtr[ 1 ] & IntHdrPtr[ 1 ]) ^ ExpectPtr[ 1 ] )
	{
        printf( "line: Mask: %08X Hdr: %08X Expext: %08X And: %08X Xor: %08X\n",
		MaskPtr[ 1 ], IntHdrPtr[ 1 ], ExpectPtr[ 1 ],
		(MaskPtr[ 1 ] & IntHdrPtr[ 1 ]),
		( (MaskPtr[ 1 ] & IntHdrPtr[ 1 ]) ^ ExpectPtr[ 1 ] ) );

	assert( 0 );
	}

    _ts_GetStatus0( & FifoStatus );
    if( fifo == (_BGL_TorusFifo *)TS0_I0 )
      {
      assert( (tot_c + FifoStatus.i0) <= _BGL_TORUS_FIFO_CHUNKS );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_I1 )
      {
      assert( (tot_c + FifoStatus.i1) <= _BGL_TORUS_FIFO_CHUNKS );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_I2 )
      {
      assert( (tot_c + FifoStatus.i2) <= _BGL_TORUS_FIFO_CHUNKS );
      }
    else
      assert( 0 );

    if(( (unsigned) fifo & 0xffff0000 ) == ( PA_TORUS_1 & 0xffff0000))
      assert( 0 );
    }
  else
    {
    unsigned int* ExpectPtr = (unsigned int *) & expect1;

    if( (MaskPtr[ 0 ] & IntHdrPtr[ 0 ]) ^ ExpectPtr[ 0 ] )
      {
        printf( "line: Mask: %08X Hdr: %08X Expext: %08X And: %08X Xor: %08X\n",
		MaskPtr[ 0 ], IntHdrPtr[ 0 ], ExpectPtr[ 0 ],
		(MaskPtr[ 0 ] & IntHdrPtr[ 0 ]),
		( (MaskPtr[ 0 ] & IntHdrPtr[ 0 ]) ^ ExpectPtr[ 0 ] ) );

	assert( 0 );
	}

      if( (MaskPtr[ 1 ] & IntHdrPtr[ 1 ]) ^ ExpectPtr[ 1 ] )
	{
        printf( "line: Mask: %08X Hdr: %08X Expext: %08X And: %08X Xor: %08X\n",
		MaskPtr[ 1 ], IntHdrPtr[ 1 ], ExpectPtr[ 1 ],
		(MaskPtr[ 1 ] & IntHdrPtr[ 1 ]),
		( (MaskPtr[ 1 ] & IntHdrPtr[ 1 ]) ^ ExpectPtr[ 1 ] ) );

	assert( 0 );
	}

    _ts_GetStatus1( & FifoStatus );

    if( fifo == (_BGL_TorusFifo *)TS1_I0 )
      {
      assert( (tot_c + FifoStatus.i0) <= _BGL_TORUS_FIFO_CHUNKS );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_I1 )
      {
      assert( (tot_c + FifoStatus.i1) <= _BGL_TORUS_FIFO_CHUNKS );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_I2 )
      {
      assert( (tot_c + FifoStatus.i2) <= _BGL_TORUS_FIFO_CHUNKS );
      }
    else
      assert( 0 );

    if(( (unsigned) fifo & 0xffff0000 ) == ( PA_TORUS_0 & 0xffff0000))
      assert( 0 );
    }
#endif

   assert( ((unsigned long)pkt & 0x000000F) == 0 );

//   printf("(I)BGLTorusInjectPacketImageApp (0x%08lx): hwh0.Pid = %d, swh0.arg0 = %4ld, swh1.arg = %4ld, swa = %d Chunks = %d\n",
//           (unsigned long)fifo, hdr->hwh0.Pid, hdr->swh0.arg0, hdr->swh1.arg, hdr->hwh0.SW_Avail, hdr->hwh0.Chunks );
//   fflush(stdout);

   do {
      // move 1st chunk into fifo
      asm volatile( "lfpdx   1, 0,%0;"
                    "lfpdux  2,%0,%1;"
                    "stfpdx  1, 0,%2;"
                    "stfpdx  2, 0,%2;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo)
//                      , "0"  (dst)
                    : "memory",
                      "fr1", "fr2" );
      } while(0);

   dst    += 16;
   c2     = (chunks >> 1);

   while( c2-- )
      {
      asm volatile( "lfpdx   1, 0,%0;"
                    "lfpdux  2,%0,%1;"
                    "lfpdux  3,%0,%1;"
                    "lfpdux  4,%0,%1;"
                    "stfpdx  1, 0,%2;"
                    "stfpdx  2, 0,%2;"
                    "stfpdx  3, 0,%2;"
                    "stfpdx  4, 0,%2;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo)
//                      ,"0"  (dst)
                    : "memory",
                      "fr1", "fr2", "fr3", "fr4" );
      dst += 16;
      }

   if ( chunks & 1 )
      {
      asm volatile( "lfpdx   1, 0,%0;"
                    "lfpdux  2,%0,%1;"
                    "stfpdx  1, 0,%2;"
                    "stfpdx  2, 0,%2;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo)
//                      , "0"  (dst)
                    : "memory",
                      "fr1", "fr2" );
      //dst += 16;
      }
}

static void BGLTorusInjectAppDeposit(
        _BGL_TorusFifo   *fifo,          // Injection FIFO to use (Must have space available!)
        _BGL_TorusPktHdr *hdr,           // Packet header we edit here
        _BGL_TorusPktPayload    *payload,       // Packet Payload
        unsigned         hint_bit,       // one of _BGL_TORUS_PKT_HINT_{X|Y|Z}{P|M}
        int              dest_hard_processor_id,
        int              dest_x,         // destination coordinates
        int              dest_y,         //
        int              dest_z)         //
{
   Bit8 *dst = (Bit8 *)payload;
   int chunks, c2;
   register int u = 16;

   // this is only set here so i don't have to mod a function that other people call
   hdr->hwh0.Pid       = dest_hard_processor_id ;          // Destination Fifo Group

   chunks = hdr->hwh0.Chunks;

   assert( ((unsigned long)hdr     & 0xF) == 0 );
   assert( ((unsigned long)payload & 0xF) == 0 );

   hdr->hwh0.Hint   = hint_bit;
   hdr->hwh0.X      = dest_x;
   hdr->hwh1.Y      = dest_y;
   hdr->hwh1.Z      = dest_z;

   do {
      // move 1st chunk (hdr + 1st quad of payload) into fifo
      asm volatile( "lfpdx   1,0,%1;"
                    "lfpdx   2,0,%2;"
                    "stfpdx  1,0,%0;"
                    "stfpdx  2,0,%0;"
                    :
                    : "b"  (fifo),
                      "b"  (hdr),
                      "b"  (dst)
                    : "memory",
                      "fr1", "fr2" );
      } while(0);

   dst    += 16;
   c2     = (chunks >> 1);

   while( c2-- )
      {
      asm volatile( "lfpdx   1, 0,%0;"
                    "lfpdux  2,%0,%1;"
                    "lfpdux  3,%0,%1;"
                    "lfpdux  4,%0,%1;"
                    "stfpdx  1, 0,%2;"
                    "stfpdx  2, 0,%2;"
                    "stfpdx  3, 0,%2;"
                    "stfpdx  4, 0,%2;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo)
//                    ,"0"  (dst)
                    : "memory",
                      "fr1", "fr2", "fr3", "fr4" );
      dst += 16;
      }

   if ( chunks & 1 )
      {
      asm volatile( "lfpdx   1, 0,%0;"
                    "lfpdux  2,%0,%1;"
                    "stfpdx  1, 0,%2;"
                    "stfpdx  2, 0,%2;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo)
//                      , "0"  (dst)
                    : "memory",
                      "fr1", "fr2" );
      //dst += 16;
      }
}


////////////////////////////////////////////////////////////////////////
////////////////////////// RECV SIDE ///////////////////////////////////
////////////////////////////////////////////////////////////////////////

static int PullHeader( _BGL_TorusFifo *fifo, void *addr )
  {
   Bit8 *dst         = (Bit8 *)addr;
   register int u = 16;

/*    QuadMove( fifo, dst, 0 ); */
/*    QuadMove( fifo, dst+16, 0 ); */

#if 0
  do {
      // pull 1st chunk into buffer
      asm volatile( "lfpdx   1,0,%2;"
                    "lfpdx   2,0,%2;"
                    "stfpdx  1,0,%0;"
                    "stfpdux 2,%0,%1;"
                    "msync;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo),
                      "0"  (dst)
                    : "memory",
                      "fr1", "fr2" );
      } while(0);
#endif

  }

static Bit8 PullFromRecvTorusFifo( Bit8 *stat, _BGL_TorusFifo *fifo, void *addr )
  {
  _BGL_TorusPkt *pkt = (_BGL_TorusPkt *)addr;
   Bit8 *dst         = (Bit8 *)addr;
   _BGL_TorusPktHdr *hdr;
   int chunks, c2;
   register int u = 16;

  do {
      // pull 1st chunk into buffer
      asm volatile( "lfpdx   1,0,%2;"
                    "lfpdx   2,0,%2;"
                    "stfpdx  1,0,%0;"
                    "stfpdux 2,%0,%1;"
                    "msync;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo)
//                      , "0"  (dst)
                    : "memory",
                      "fr1", "fr2" );
      } while(0);

//   PullHeader( fifo, addr );

   hdr    = &(pkt->hdr);
   //printf("(I) _ts_AppReceiver_Put(0x%08lx): *stat = %d, hwh0.Pid = %d, swh0.arg0 = %4ld, swh1.arg = %4ld, swa = %d Chunks = %d\n",
   //        (unsigned long)fifo, *stat, hdr->hwh0.Pid, hdr->swh0.arg0, hdr->swh1.arg, hdr->hwh0.SW_Avail, hdr->hwh0.Chunks );
   chunks = hdr->hwh0.Chunks;  // hw hdr says (chunks - 1), we did chunk 0 already
/*    if( chunks+1 > aNumChunksToExpect ) */
/*      return chunks; */

   ///// THIS GOES WITH THE PullHeader() routine
   dst    += 16;
   // dst    += 32;
   c2     = (chunks >> 1);

   while( c2-- )
      {
      asm volatile( "lfpdx   1,0,%2;"
                    "lfpdx   2,0,%2;"
                    "lfpdx   3,0,%2;"
                    "lfpdx   4,0,%2;"
                    "stfpdx  1,0,%0;"
                    "stfpdux 2,%0,%1;"
                    "stfpdux 3,%0,%1;"
                    "stfpdux 4,%0,%1;"
                    : "+b" (dst)
                    : "b"  (u),
                      "r"  (fifo)
//                      , "0"  (dst)
                    : "memory",
                      "fr1", "fr2", "fr3", "fr4" );
      dst += 16;
      }

   if ( chunks & 1 )
      {
      asm volatile( "lfpdx   1,0,%2;"
                    "lfpdx   2,0,%2;"
                    "stfpdx  1,0,%0;"
                    "stfpdux 2,%0,%1;"
                    : "+b" (dst)
                    : "b"  (u),                ///////// Before Mark's advice of jun 15 06 : "r"  (u),
                      "r"  (fifo)
//                      ,"0"  (dst)
                    : "memory",
                      "fr1", "fr2" );
      dst += 16;
      }

   // update the cached status
   *stat -= (chunks + 1);
    return chunks;
  }

// Put: move entire packet from fifo to buf
static int _ts_AppReceiver_Put( Bit8 *stat, _BGL_TorusFifo *fifo, void *addr )
{
#if TORUS_CHECK
  _BGL_TorusFifoStatus FifoStatus;
  int core = rts_get_processor_id();
  Bit8 NumberOfChunksInStat;

  if( core == 0 )
    {
    _ts_GetStatus0( & FifoStatus );
    if( fifo == (_BGL_TorusFifo *)TS0_R0 )
      {
      NumberOfChunksInStat = FifoStatus.r0;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R1 )
      {
      NumberOfChunksInStat = FifoStatus.r1;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R2 )
      {
      NumberOfChunksInStat = FifoStatus.r2;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R3 )
      {
      NumberOfChunksInStat = FifoStatus.r3;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R4 )
      {
      NumberOfChunksInStat = FifoStatus.r4;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS0_R5 )
      {
      NumberOfChunksInStat = FifoStatus.r5;
      assert( NumberOfChunksInStat );
      }
    else
      assert( 0 );

    if(( (unsigned) fifo & 0xffff0000 ) == ( PA_TORUS_1 & 0xffff0000))
      assert( 0 );
    }
  else
    {
    _ts_GetStatus1( & FifoStatus );

    if( fifo == (_BGL_TorusFifo *)TS1_R0 )
      {
      NumberOfChunksInStat = FifoStatus.r0;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R1 )
      {
      NumberOfChunksInStat = FifoStatus.r1;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R2 )
      {
      NumberOfChunksInStat = FifoStatus.r2;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R3 )
      {
      NumberOfChunksInStat = FifoStatus.r3;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R4 )
      {
      NumberOfChunksInStat = FifoStatus.r4;
      assert( NumberOfChunksInStat );
      }
    else if( fifo == (_BGL_TorusFifo *)TS1_R5 )
      {
      NumberOfChunksInStat = FifoStatus.r5;
      assert( NumberOfChunksInStat );
      }
    else
      assert( 0 );

    if(( (unsigned) fifo & 0xffff0000 ) == ( PA_TORUS_0 & 0xffff0000))
      assert( 0 );
    }
#endif

   assert( ((unsigned long)addr & 0x000000F) == 0 );
   assert( *stat );

   Bit8 chunks = PullFromRecvTorusFifo( stat, fifo, addr );

#if TORUS_CHECK
   int chunks1 = chunks;
   int tmp = NumberOfChunksInStat;
   if ( chunks + 1 > NumberOfChunksInStat )
     {
     printf( "ERROR: %d %d \n", chunks1, tmp );
     fflush( stdout );
     assert( 0 );
     }
#endif

  return( 1 );
  }

static int _ts_PollF0_OnePassReceive_Put( _BGL_TorusFifoStatus *statF0, void *base_addr, int stride )
{
   int pkts;
   char *addr = (char *)base_addr;

   pkts = 0;

   if ( statF0->r0 )
      {
      pkts += _ts_AppReceiver_Put( &(statF0->r0), (_BGL_TorusFifo *)TS0_R0, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF0->r1 )
      {
      pkts += _ts_AppReceiver_Put( &(statF0->r1), (_BGL_TorusFifo *)TS0_R1, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF0->r2 )
      {
      pkts += _ts_AppReceiver_Put( &(statF0->r2), (_BGL_TorusFifo *)TS0_R2, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF0->r3 )
      {
      pkts += _ts_AppReceiver_Put( &(statF0->r3), (_BGL_TorusFifo *)TS0_R3, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF0->r4 )
      {
      pkts += _ts_AppReceiver_Put( &(statF0->r4), (_BGL_TorusFifo *)TS0_R4, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF0->r5 )
      {
      pkts += _ts_AppReceiver_Put( &(statF0->r5), (_BGL_TorusFifo *)TS0_R5, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( pkts )
      {
      //printf("(I) _ts_PollF0_OnePassReceive_Put: pkts = %d\n", pkts );
      }

   return( pkts );
}

// same as above but with c/F0/F1/*m
static int _ts_PollF1_OnePassReceive_Put( _BGL_TorusFifoStatus *statF1, void *base_addr, int stride )
{
   int pkts;
   char *addr = (char *)base_addr;

   pkts = 0;

   if ( statF1->r0 )
      {
      pkts += _ts_AppReceiver_Put( &(statF1->r0), (_BGL_TorusFifo *)TS1_R0, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF1->r1 )
      {
      pkts += _ts_AppReceiver_Put( &(statF1->r1), (_BGL_TorusFifo *)TS1_R1, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF1->r2 )
      {
      pkts += _ts_AppReceiver_Put( &(statF1->r2), (_BGL_TorusFifo *)TS1_R2, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF1->r3 )
      {
      pkts += _ts_AppReceiver_Put( &(statF1->r3), (_BGL_TorusFifo *)TS1_R3, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF1->r4 )
      {
      pkts += _ts_AppReceiver_Put( &(statF1->r4), (_BGL_TorusFifo *)TS1_R4, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( statF1->r5 )
      {
      pkts += _ts_AppReceiver_Put( &(statF1->r5), (_BGL_TorusFifo *)TS1_R5, (_BGL_TorusPkt *)addr  );
      addr += stride;
      }

   if ( pkts )
      {
      //printf("(I) _ts_PollF1_OnePassReceive_Put: pkts = %d\n", pkts );
      }

   return( pkts );
}

__END_DECLS

