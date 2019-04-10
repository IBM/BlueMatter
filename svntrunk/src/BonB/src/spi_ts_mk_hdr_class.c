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
//      File: blade/spi/spi_ts_hdr_class.c
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
 *     Torus Class Routed Header Construction Interfaces                     *
 *---------------------------------------------------------------------------*/

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single X range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassDirectX(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_x0,            // destination X range start
                     int dest_x1,            // destination X range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;
   int hints;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // calc hints based on start node of X Range
   if ( dest_x0 < x )
      hints = _BGL_TORUS_PKT_HINT_XM;
   else
      hints = _BGL_TORUS_PKT_HINT_XP;

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = hints;                        // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = dest_x1;                      // Destination X coord is end of X Range

   // fill out hardware header word 1
   hdr->hwh1.Y         = y;                            // Destination Y coord is same as this node's
   hdr->hwh1.Z         = z;                            // Destination Z coord is same as this node's

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single Y range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassDirectY(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_y0,            // destination Y range start
                     int dest_y1,            // destination Y range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;
   int hints;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // calc hints based on start node of X Range
   if ( dest_y0 < y )
      hints = _BGL_TORUS_PKT_HINT_YM;
   else
      hints = _BGL_TORUS_PKT_HINT_YP;

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = hints;                        // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = x;                            // Destination X coord is same as this node's

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y1;                      // Destination Y coord is end of Y Range
   hdr->hwh1.Z         = z;                            // Destination Z coord is same as this node's

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single Z range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassDirectZ(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_z0,            // destination Z range start
                     int dest_z1,            // destination Z range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;
   int hints;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // calc hints based on start node of X Range
   if ( dest_z0 < z )
      hints = _BGL_TORUS_PKT_HINT_ZM;
   else
      hints = _BGL_TORUS_PKT_HINT_ZP;

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = hints;                        // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = x;                            // Destination X coord is same as this node's

   // fill out hardware header word 1
   hdr->hwh1.Y         = y;                            // Destination Y coord is same as this node's
   hdr->hwh1.Z         = dest_z1;                      // Destination Z coord is end of Z Range

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Deposit packets along X+, starting at calling nodes X+ neighbor, ending at dest_x
_BGL_TorusPktHdr *BGLTorusMakeHdrClassXP(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_x,             // destination X range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = _BGL_TORUS_PKT_HINT_XP;       // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = dest_x;                       // Destination X coord is end of X Range

   // fill out hardware header word 1
   hdr->hwh1.Y         = y;                            // Destination Y coord is same as this node's
   hdr->hwh1.Z         = z;                            // Destination Z coord is same as this node's

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Deposit packets along X-, starting at calling nodes X- neighbor, ending at dest_x
_BGL_TorusPktHdr *BGLTorusMakeHdrClassXM(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_x,             // destination X range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = _BGL_TORUS_PKT_HINT_XM;       // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = dest_x;                       // Destination X coord is end of X Range

   // fill out hardware header word 1
   hdr->hwh1.Y         = y;                            // Destination Y coord is same as this node's
   hdr->hwh1.Z         = z;                            // Destination Z coord is same as this node's

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Deposit packets along Y+, starting at calling nodes Y+ neighbor, ending at dest_y
_BGL_TorusPktHdr *BGLTorusMakeHdrClassYP(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_y,             // destination Y range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = _BGL_TORUS_PKT_HINT_YP;       // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = x;                            // Destination X coord is same as this node's

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y;                       // Destination Y coord is end of Y range
   hdr->hwh1.Z         = z;                            // Destination Z coord is same as this node's

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Deposit packets along Y-, starting at calling nodes Y- neighbor, ending at dest_y
_BGL_TorusPktHdr *BGLTorusMakeHdrClassYM(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_y,             // destination Y range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = _BGL_TORUS_PKT_HINT_YM;       // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = x;                            // Destination X coord is same as this node's

   // fill out hardware header word 1
   hdr->hwh1.Y         = dest_y;                       // Destination Y coord is end of range
   hdr->hwh1.Z         = z;                            // Destination Z coord is same as this node's

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Deposit packets along Z+, starting at calling nodes Z+ neighbor, ending at dest_z
_BGL_TorusPktHdr *BGLTorusMakeHdrClassZP(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_z,             // destination Z range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = _BGL_TORUS_PKT_HINT_ZP;       // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = x;                            // Destination X coord is same as this node's

   // fill out hardware header word 1
   hdr->hwh1.Y         = y;                            // Destination Y coord is same as this node's
   hdr->hwh1.Z         = dest_z;                       // Destination Z coord is end of range

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

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Deposit packets along Z-, starting at calling nodes Z- neighbor, ending at dest_z
_BGL_TorusPktHdr *BGLTorusMakeHdrClassZM(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_z,             // destination Z range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature )         // Actor Signature from above (must match Actor)
{
   int x, y, z;

   // obtain node coordinate
   BGLPartitionGetCoords( &x, &y, &z );

   // fill out hardware header word 0
   hdr->hwh0.CSum_Skip = 4;                            // skip 8 byte hardware header
   hdr->hwh0.Sk        = 0;                            // use CSum_Skip (ie, don't skip entire packet)
   hdr->hwh0.Hint      = _BGL_TORUS_PKT_HINT_ZM;       // set calculated hint bits
   hdr->hwh0.Dp        = _BGL_TORUS_PKT_CLASS;         // Class Routed Packet
   hdr->hwh0.Pid       = dest_f;                       // Destination Fifo Group
   hdr->hwh0.Chunks    = 0;                            // init chunks
   hdr->hwh0.SW_Avail  = signature;                    // Active Function Signature type
   hdr->hwh0.Dynamic   = _BGL_TORUS_PKT_DYNAMIC;       // use Dynamic Routing
   hdr->hwh0.VC        = _BGL_TORUS_PKT_VC_D0;         // inject into VC Dynamic 0
   hdr->hwh0.X         = x;                            // Destination X coord is same as this node's

   // fill out hardware header word 1
   hdr->hwh1.Y         = y;                            // Destination Y coord is same as this node;s
   hdr->hwh1.Z         = dest_z;                       // Destination Z coord is end of range

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


// Construct a Torus Class Routed Header when "Indirect" Sends are required.
//  Use when filling an arbitrary plane or contiguous sub-cube, whether or not this
//   node is a member.
// Implementation note: the primary packet header will be set to invoke a system
//  library supplied function at intermediate nodes.  At destination nodes, the
//  actor function will be invoked.  In reality, this function is just syntatic
//  sugar for simply calling one of the simple header creation routines, then
//  programming an indirection argument, and calling it from your own primary
//  actor.  There is one exception to that rule, but it is beyond the scope of
//  this header file (hint: multiple injection).
// Current thinking is that if sending to a range of fifo groups, the packet will
//  be duplicated internally in destination nodes via s/w.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassIndirect(
                     _BGL_TorusPktHdr *hdr, // Filled in on return                   (output)
                     void *indirect,        // Indirect Header filled in on return   (output)
                     int dest_x0,           // destination X range coordinates start (rest are inputs)
                     int dest_x1,           // destination X range coordinates end
                     int dest_y0,           // destination Y range coordinates start
                     int dest_y1,           // destination Y range coordinates end
                     int dest_z0,           // destination Z range coordinates start
                     int dest_z1,           // destination Z range coordinates end
                     int dest_f0,           // destination Fifo Group range start (eg 0-0, 0-1, 1-1)
                     int dest_f1,           // destination Fifo Group range end
                     ACTOR_DECL(actor),     // Active Packet Function matching signature
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature )        // Actor Signature from above (must match Actor)
{
   return( hdr );
}

