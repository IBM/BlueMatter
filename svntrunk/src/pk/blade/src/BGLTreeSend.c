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
//      File: blade/spi/BGLTreeSend.c
//
//   Purpose: Implement Status Queries for Blade's Tree Network System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Matthias A. Blumrich (blumrich@us.ibm.com)
//
//     Notes: This SPI is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the Tree.
//
//   History: 04/30/2003: MAB - Created.
//
//
#include <blade_on_blrts.h>


// The following routines will block indefinitely until there is room in the specified
// injection fifo. This implies that they poll the status register.
//
// !! WARNING: If the payload of a packet send routine is greater than a single packet's
// !! worth of data, then every packet will use the specified header.


#ifdef _SPI_TREE_DO_INJ_CSUM
#define BGLTREEINJECTIONCSUMADD( a, b, c, d ) BGLTreeInjectionCsumAdd( a, b, c, d )
#else
#define BGLTREEINJECTIONCSUMADD( a, b, c, d )
#endif


////////////////////////////////////////////////////////////////////////////////
// Use for aligned payload that is a multiple of 16 bytes in length.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeSend"
int BGLTreeSend(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload (16-byte aligned)
       int            bytes )   // Payload bytes (multiple of 16)
{
   _BGL_TreeFifoStatus stat;
   char *cp = (char *)pyld;
   int  bytesleft;
#if 0
   int bytestosend;
#endif
   int  rc = 0;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x, 0x%.8x, %d ) called.\n", vc, (int)hdrHW, (int)pyld, bytes );
#endif

#ifdef CONFIG_BLNIE
#if defined(_SPI_CHECK_ALGN)
   if( ((Bit32)pyld) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": pyld=0x%x not 16-byte aligned.\n", pyld ));
      _BLADE_ABORT( -1 );
   }
#endif
   if ( (bytes < 16) ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d not 16 or more.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   if( bytes & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d not a multiple of 16.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
#endif

   cp = (char *)pyld;

   // Inject headers and payloads in pairs.
   for( bytesleft = bytes; bytesleft > 0; bytesleft = bytesleft - _BGL_TREE_PKT_MAX_BYTES ) {
      // Block until there is room for a packet.
      do {
         BGLTreeGetStatus( vc, &stat );
      } while( (stat.InjHdrCount == 0x8) || (stat.InjPyldCount > 0x70) );

      if( bytesleft >= _BGL_TREE_PKT_MAX_BYTES ) {
         rc |= BGLTreeRawSendPacket( vc, hdrHW, (void *)cp );
         BGLTREEINJECTIONCSUMADD( vc, *((Bit32 *)hdrHW), (Bit16 *)cp, _BGL_TREE_PKT_MAX_BYTES );
         cp += _BGL_TREE_PKT_MAX_BYTES;
      } else {
         rc |= BGLTreeRawSendPacketPartial( vc, hdrHW, (void *)cp, bytesleft );
         BGLTREEINJECTIONCSUMADD( vc, *((Bit32 *)hdrHW), (Bit16 *)cp, bytesleft );
      }

#if 0
      bytestosend = (bytesleft > _BGL_TREE_PKT_MAX_BYTES ? _BGL_TREE_PKT_MAX_BYTES : bytesleft);
      rc |= BGLTreeRawSendPacketPartial( vc, hdrHW, (void *)cp, bytestosend );
      BGLTREEINJECTIONCSUMADD( vc, *((Bit32 *)hdrHW), (Bit16 *)cp, bytestosend );
      cp += bytestosend;
#endif
   }

   return( rc );
}

////////////////////////////////////////////////////////////////////////////////
// Use for any size payload at any alignment.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeSendUnaligned"
int BGLTreeSendUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload
       int            bytes )   // Payload bytes
{
   _BGL_TreeFifoStatus stat;
   char *cp = (char *)pyld;
   int  bytesleft, bytestosend;
   int  rc = 0;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x, 0x%.8x, %d ) called.\n", vc, (int)hdrHW, (int)pyld, bytes );
#endif

#ifdef _SPI_CHECK
   if ( bytes < 1 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d not 1 or more.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Inject headers and payloads in pairs.
   for( bytesleft = bytes; bytesleft > 0; bytesleft = bytesleft - _BGL_TREE_PKT_MAX_BYTES ) {
      // Block until there is room for a packet.
      do {
         BGLTreeGetStatus( vc, &stat );
      } while( (stat.InjHdrCount == 0x8) || (stat.InjPyldCount > 0x70) );

      bytestosend = (bytesleft > _BGL_TREE_PKT_MAX_BYTES ? _BGL_TREE_PKT_MAX_BYTES : bytesleft);
      rc |= BGLTreeRawSendPacketUnaligned( vc, hdrHW, (void *)cp, bytestosend );
      BGLTREEINJECTIONCSUMADD( vc, *((Bit32 *)hdrHW), (Bit16 *)cp, bytestosend );

      cp += bytestosend;
   }

   return( rc );
}

