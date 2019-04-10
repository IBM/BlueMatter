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
//      File: blade/spi/spi_ts_updt_hdr.c
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

__BEGIN_DECLS

// Update the primary and secondary arguments in a previously created header
_BGL_TorusPktHdr *BGLTorusUpdateHdrArgs(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra )          // secondary 10bit argument to actor
{
   hdr->swh0.extra = extra;
   hdr->swh1.arg   = arg;

   return( hdr );
}


// Update the destination coordinates in a previously created header
_BGL_TorusPktHdr *BGLTorusUpdateHdrDest(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f )           // destination Fifo Group
{
   hdr->hwh0.Pid = dest_f;     // Destination Fifo Group
   hdr->hwh0.X   = dest_x;     // Destination X coord
   hdr->hwh1.Y   = dest_y;     // Destination Y coord
   hdr->hwh1.Z   = dest_z;     // Destination Z coord

   // reset any hint bits previously set
   hdr->hwh0.Hint = 0;

   // also recompute the fifo chooser assist
   _ts_MakeSendHints( hdr );

   return( hdr );
}


// Update the hint bits in a previously created header
_BGL_TorusPktHdr *BGLTorusUpdateHdrHints(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     int hints )            // hint bits
{
   hdr->hwh0.Hint = hints;

   // also recompute the fifo chooser assist
   _ts_MakeSendHints( hdr );

   return( hdr );
}


// Update the size field in a previously created header
// Note: This calculates chunks by adding the size of the packet header,
//   possibly plus the size of an additional "indirect" software header,
//   plus the bytes parameter, then rounds up that total to chunksize,
//   and finally sets the (chunks - 1) into the header.
_BGL_TorusPktHdr *BGLTorusUpdateHdrSize(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     int bytes )            // Payload bytes (header size is added to this)
{
   int chunks = ((bytes + 15) >> 5);

#if 0
   if ( (bytes < 0) || (chunks > 7) )
      _blnie_node_crash("BGLTorusUpdateHdrSize: Bad Size.");
#endif

   hdr->hwh0.Chunks = chunks;
   return( hdr );
}


__END_DECLS
