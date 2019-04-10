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
 /* -------------------------------------------------------------  */
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
/* -------------------------------------------------------------  */
//
//      File: blade/spi/spi_ts_buffers.c
//
//   Purpose: Implement Blade's Torus Packet Layer SPI.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: See include/spi/BGL_TorusPktSPI.h
//
//   History: 05/15/2002: MEG - Created.
//
//
#include <blade_on_blrts.h>

__BEGIN_DECLS


// Status Cache: Sender Use Only.
_BGL_TorusFifoStatus _ts0_stat0_bounce ALIGN_L3_CACHE,
                     _ts0_stat1_bounce;

_BGL_TorusFifoStatus _ts1_stat0_bounce ALIGN_L3_CACHE,
                     _ts1_stat1_bounce;

// Status Cache: Receiver Use Only.
_BGL_TorusFifoStatus _ts0_stat0recv_bounce ALIGN_L3_CACHE,
                     _ts0_stat1recv_bounce;

_BGL_TorusFifoStatus _ts1_stat0recv_bounce ALIGN_L3_CACHE,
                     _ts1_stat1recv_bounce;



// Per-FIFO Buffer for "Buffered" or "Unaligned" packets
_BGL_TorusPkt _ts0_r0_bounce ALIGN_L3_CACHE,
              _ts0_r1_bounce,
              _ts0_r2_bounce,
              _ts0_r3_bounce,
              _ts0_r4_bounce,
              _ts0_r5_bounce,
              _ts0_rH_bounce,
              _ts0_i0_bounce,
              _ts0_i1_bounce,
              _ts0_i2_bounce,
              _ts0_iH_bounce;

_BGL_TorusPkt _ts1_r0_bounce ALIGN_L3_CACHE,
              _ts1_r1_bounce,
              _ts1_r2_bounce,
              _ts1_r3_bounce,
              _ts1_r4_bounce,
              _ts1_r5_bounce,
              _ts1_rH_bounce,
              _ts1_i0_bounce,
              _ts1_i1_bounce,
              _ts1_i2_bounce,
              _ts1_iH_bounce;


//
// Select a packet buffer based on the fifo
//
_BGL_TorusPkt *_ts_Fifo2Buffer( Bit32 fifo )
{

   switch( (Bit32)fifo )
      {
      case TS0_R0: return( &(_ts0_r0_bounce) ); break;
      case TS0_R1: return( &(_ts0_r1_bounce) ); break;
      case TS0_R2: return( &(_ts0_r2_bounce) ); break;
      case TS0_R3: return( &(_ts0_r3_bounce) ); break;
      case TS0_R4: return( &(_ts0_r4_bounce) ); break;
      case TS0_R5: return( &(_ts0_r5_bounce) ); break;
      case TS0_RH: return( &(_ts0_rH_bounce) ); break;
      case TS0_I0: return( &(_ts0_i0_bounce) ); break;
      case TS0_I1: return( &(_ts0_i1_bounce) ); break;
      case TS0_I2: return( &(_ts0_i2_bounce) ); break;
      case TS0_IH: return( &(_ts0_iH_bounce) ); break;
      case TS1_R0: return( &(_ts1_r0_bounce) ); break;
      case TS1_R1: return( &(_ts1_r1_bounce) ); break;
      case TS1_R2: return( &(_ts1_r2_bounce) ); break;
      case TS1_R3: return( &(_ts1_r3_bounce) ); break;
      case TS1_R4: return( &(_ts1_r4_bounce) ); break;
      case TS1_R5: return( &(_ts1_r5_bounce) ); break;
      case TS1_RH: return( &(_ts1_rH_bounce) ); break;
      case TS1_I0: return( &(_ts1_i0_bounce) ); break;
      case TS1_I1: return( &(_ts1_i1_bounce) ); break;
      case TS1_I2: return( &(_ts1_i2_bounce) ); break;
      case TS1_IH: return( &(_ts1_iH_bounce) ); break;
      default:
         return( (_BGL_TorusPkt *)0 );
         break;
      }
}

//
// Return the Fifo address based on the Buffer (diagnostics use)
//
Bit32 _ts_Buffer2Fifo( _BGL_TorusPkt *buf )
{
   // can't use a switch statement
   if ( (Bit32)buf == (Bit32)&_ts0_r0_bounce ) return( TS0_R0 );
   if ( (Bit32)buf == (Bit32)&_ts0_r1_bounce ) return( TS0_R1 );
   if ( (Bit32)buf == (Bit32)&_ts0_r2_bounce ) return( TS0_R2 );
   if ( (Bit32)buf == (Bit32)&_ts0_r3_bounce ) return( TS0_R3 );
   if ( (Bit32)buf == (Bit32)&_ts0_r4_bounce ) return( TS0_R4 );
   if ( (Bit32)buf == (Bit32)&_ts0_r5_bounce ) return( TS0_R5 );
   if ( (Bit32)buf == (Bit32)&_ts0_rH_bounce ) return( TS0_RH );
   if ( (Bit32)buf == (Bit32)&_ts0_i0_bounce ) return( TS0_I0 );
   if ( (Bit32)buf == (Bit32)&_ts0_i1_bounce ) return( TS0_I1 );
   if ( (Bit32)buf == (Bit32)&_ts0_i2_bounce ) return( TS0_I2 );
   if ( (Bit32)buf == (Bit32)&_ts0_iH_bounce ) return( TS0_IH );
   if ( (Bit32)buf == (Bit32)&_ts1_r0_bounce ) return( TS1_R0 );
   if ( (Bit32)buf == (Bit32)&_ts1_r1_bounce ) return( TS1_R1 );
   if ( (Bit32)buf == (Bit32)&_ts1_r2_bounce ) return( TS1_R2 );
   if ( (Bit32)buf == (Bit32)&_ts1_r3_bounce ) return( TS1_R3 );
   if ( (Bit32)buf == (Bit32)&_ts1_r4_bounce ) return( TS1_R4 );
   if ( (Bit32)buf == (Bit32)&_ts1_r5_bounce ) return( TS1_R5 );
   if ( (Bit32)buf == (Bit32)&_ts1_rH_bounce ) return( TS1_RH );
   if ( (Bit32)buf == (Bit32)&_ts1_i0_bounce ) return( TS1_I0 );
   if ( (Bit32)buf == (Bit32)&_ts1_i1_bounce ) return( TS1_I1 );
   if ( (Bit32)buf == (Bit32)&_ts1_i2_bounce ) return( TS1_I2 );
   if ( (Bit32)buf == (Bit32)&_ts1_iH_bounce ) return( TS1_IH );

   return( 0 );
}

__END_DECLS
