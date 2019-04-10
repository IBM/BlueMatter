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
//      File: blade/spi/BGLTreeBcast.c
//
//   Purpose: Implement Broadcast functions for Blade's Tree Network System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Matthias A. Blumrich (blumrich@us.ibm.com)
//
//     Notes: This SPI is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the Tree.
//
//   History: 11/13/2003: MAB - Created.
//
//
#include <BonB/blade_on_blrts.h>

#define _PCKTS_PER_MSG  12
#define _BYTES_PER_MSG  _PCKTS_PER_MSG * _BGL_TREE_PKT_MAX_BYTES

// Blocking broadcast called by all nodes with rank of sending node.  Sending nodes's buffer contains source
// data.  Upon completion, all nodes have the data.
#undef  FN_NAME
#define FN_NAME "BGLTreeBcastUnaligned"
void BGLTreeBcastUnaligned(
       int  sender,   // rank of sending node
       int  vt,       // Class (virtual tree)
       int  vc,       // Virtual channel
       void *data,    // Data to send
       int  bytes )   // length in bytes
{
   int rank = BGLPartitionGetRank();
   int bytesleft;
   int bytestosend;
   char *cp = (char *)data;
   _BGL_TreeHwHdr snd_hdr, rcv_hdr;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, %d, %d, 0x%.8x, %d ) called.\n", sender, vt, vc, (int)data, bytes );
#endif

   if( sender == rank ) {
      // Sender

      // Create a broadcast header with tag=0
      BGLTreeMakeBcastHdr( &snd_hdr, vt, 0 );

      for( bytesleft = bytes; bytesleft > 0; bytesleft = bytesleft - _BYTES_PER_MSG ) {
         bytestosend = (bytesleft > _BYTES_PER_MSG ? _BYTES_PER_MSG : bytesleft);

         // Inject a "message"
         BGLTreeSendUnaligned( vc, &snd_hdr, cp, bytestosend );

         // Receive it back
         BGLTreeReceiveUnaligned( vc, &rcv_hdr, cp, bytestosend );

         cp += bytestosend;
      }
   } else {
      // Receiver
      BGLTreeReceiveUnaligned( vc, &rcv_hdr, data, bytes );
   }
}

