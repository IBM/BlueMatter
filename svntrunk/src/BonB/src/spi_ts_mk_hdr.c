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
//      File: blade/spi/spi_ts_mk_hdr.c
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
#include <BonB/blade_on_blrts.h>


/*---------------------------------------------------------------------------*
 *     Torus Header Construction Interfaces                                  *
 *---------------------------------------------------------------------------*/

// Simple H/W + S/W Torus Header Creation using suitable defaults
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Dynamic on VCD0,
//                  Hardware applied Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
_BGL_TorusPktHdr *BGLTorusMakeHdr(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f,            // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature )        // Actor Signature from above (must match Actor)
{
   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = 0;                            // h/w will apply hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_POINT2POINT;   // Point to Point Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = dest_x;                       // Destination X coord

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y;                       // Destination Y coord
   hdr->hwh1.Z         = dest_z;                       // Destination Z coord

   // fill out software header word 0
   hdr->swh0.extra     = extra;
   hdr->swh0.fcn       = (((Bit32)actor & 0x00FFFFFC) >> 2);

   // fill out software header word 1
   hdr->swh1.arg       = arg;

   // use the reserved fields as hints to the SPI to choose which
   //  injection fifo(s) are candidates at packet injection time.
   // Note that non-zero packet hint bits override any choice made here.
   _ts_MakeSendHints( hdr );

   return( hdr );
}


// High Priority H/W + S/W Torus Header Creation
//  Setup used:     Point-to-Point,
//                  High Priority,
//                  Deterministic on VCBP,
//                  Calculated Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
_BGL_TorusPktHdr *BGLTorusMakeHdrHP(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f,            // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature )        // Actor Signature from above (must match Actor)
{
   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                           // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                           // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = 0;                           // h/w will apply hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_POINT2POINT;  // Point to Point packet
   hdr->hwh0.Pid       = dest_f;                      // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                           // init chunks
   hdr->hwh0.SW_Avail  = signature;                   // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DETERMINSTIC; // use Deterministic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_BP;        // inject into VC Bubble Priority
   hdr->hwh0.X         = dest_x;                      // Destination X coord

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y;                      // Destination Y coord
   hdr->hwh1.Z         = dest_z;                      // Destination Z coord

   // fill out software header word 0
   hdr->swh0.extra     = extra;
   hdr->swh0.fcn       = (((Bit32)actor & 0x00FFFFFC) >> 2);

   // fill out software header word 1
   hdr->swh1.arg       = arg;

   // use the reserved fields as hints to the SPI to choose which
   //  injection fifo(s) are candidates at packet injection time.
   // Note that non-zero packet hint bits override any choice made here.
   _ts_MakeSendHints( hdr );

   return( hdr );
}


// H/W + S/W Torus Header Creation using deterministic X,Y,Z routing.
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Deterministic Routing,
//                  Calculated Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
_BGL_TorusPktHdr *BGLTorusMakeHdrDeterministic(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f,            // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature )        // Actor Signature from above (must match Actor)
{
   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                           // skip 4x2=8 byte hardware header
   hdr->hwh0.Sk        = 0;                           // use CSum_Skip (ie, don't skip entire packet)

   // calc hints: deterministic routing goes X, then Y then Z
   hdr->hwh0.Hint = BGLTorusCalcHints( dest_x, dest_y, dest_z );

   hdr->hwh0.Dp        = _BGL_TORUS_PKT_POINT2POINT;  // Point to Point packet
   hdr->hwh0.Pid       = dest_f;                      // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                           // just init chunks
   hdr->hwh0.SW_Avail  = signature;                   // Acvive Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DETERMINSTIC; // use Deterministic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_BN;        // inject into VC Bubble Normal
   hdr->hwh0.X         = dest_x;                      // Destination X coord

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y;                      // Destination Y coord
   hdr->hwh1.Z         = dest_z;                      // Destination Z coord

   // fill out software header word 0
   hdr->swh0.extra     = extra;
   hdr->swh0.fcn       = (((Bit32)actor & 0x00FFFFFC) >> 2);

   // fill out software header word 1
   hdr->swh1.arg       = arg;

   // use the reserved fields as hints to the SPI to choose which
   //  injection fifo(s) are candidates at packet injection time.
   // Note that non-zero packet hint bits override any choice made here.
   _ts_MakeSendHints( hdr );

   return( hdr );
}


