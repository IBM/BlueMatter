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
//      File: blade/spi/BGLTreeMakeHdr.c
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
//   History: 04/11/2003: MAB - Created.
//
//
#include <blade_on_blrts.h>


// Create a COLLECTIVE H/W tree header.  This function allows full control of header creation.
#undef  FN_NAME
#define FN_NAME "BGLTreeMakeCollectiveHdr"
_BGL_TreeHwHdr *BGLTreeMakeCollectiveHdr(
                    _BGL_TreeHwHdr *hdr,         // Filled in on return
                    int vt,                      // class (virtual tree)
                    int irq,                     // irq.  If combine, must be the same for all callers.
                    int opcode,                  // ALU opcode (000 for ordinary packets)
                    int opsize,                  // ALU operand size
                    int tag,                     // software tag. If combine, must be the same for all callers.
                    int csumMode )               // injection checksum mode
{
#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME" called.\n" );
#endif

   hdr->CtvHdr.Class    = vt;        // Class number (a.k.a virtual tree number)
   hdr->CtvHdr.Ptp      = 0;         // Must be 0 for collective header format.
   hdr->CtvHdr.Irq      = irq;       // 1=request interrupt when received, 0=not
   hdr->CtvHdr.OpCode   = opcode;    // 000=ordinary routed packet, else ALU opcode
   hdr->CtvHdr.OpSize   = opsize;    // Operand size
   hdr->CtvHdr.Tag      = tag;       // Software tag
   hdr->CtvHdr.CsumMode = csumMode;  // Injection checksum mode
   return( hdr );
}


// Create a POINT-TO-POINT H/W tree header.  This function allows full control of header construction.
_BGL_TreeHwHdr *BGLTreeMakePtpHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int irq,                     // interrupt request
                           int PtpTarget,               // point-to-point target (node address to accept)
                           int csumMode )               // injection checksum mode
{
   hdr->PtpHdr.Class     = vt;          // Class number (a.k.a virtual tree number)
   hdr->PtpHdr.Ptp       = 1;           // Must be 1 for PTP header format.
   hdr->PtpHdr.Irq       = irq;         // 1=request interrupt when received, 0=not
   hdr->PtpHdr.PtpTarget = PtpTarget;   // Ptp packet target (matched to node address)
   hdr->PtpHdr.CsumMode  = csumMode;    // Injection checksum mode
   return( hdr );
}

// Create a point-to-point header for the common case.  In this header:
// (1) The irq bit is always set
// (2) The injection checksum mode is set to 1 (include H/W header, exclude first qword of payload)
_BGL_TreeHwHdr *BGLTreeMakeSendHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int PtpTarget )              // point-to-point target (node address to accept)
{
   hdr->PtpHdr.Class     = vt;                  // Class number (a.k.a virtual tree number)
   hdr->PtpHdr.Ptp       = 1;                   // Must be 1 for PTP header format.
   hdr->PtpHdr.Irq       = 1;                   // 1=request interrupt when received, 0=not (usually Masked in BIC)
   hdr->PtpHdr.PtpTarget = PtpTarget;           // Ptp packet target (matched to node address)
   hdr->PtpHdr.CsumMode  = _BGL_TREE_CSUM_SOME; // Injection checksum mode (All but 1st Quad)
   return( hdr );
}

// Create a broadcast header for the common case.  In this header:
// (1) the irq bit is always cleared
// (2) the opcode is set to NONE (so operand size is irrelevant).
// (3) the injection checksum mode is set to 3 (include everything)
_BGL_TreeHwHdr *BGLTreeMakeBcastHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int tag )                    // software tag.  If combine, must be the same for all callers.
{
   hdr->CtvHdr.Class    = vt;                    // Class number (a.k.a virtual tree number)
   hdr->CtvHdr.Ptp      = 0;                     // Must be 0 for collective header format.
   hdr->CtvHdr.Irq      = 0;                     // 1=request interrupt when received, 0=not
   hdr->CtvHdr.OpCode   = _BGL_TREE_OPCODE_NONE; // 000=ordinary routed packet, else ALU opcode
   hdr->CtvHdr.OpSize   = 0;                     // Operand size
   hdr->CtvHdr.Tag      = tag;                   // Software tag
   hdr->CtvHdr.CsumMode = _BGL_TREE_CSUM_ALL;    // Injection checksum mode
   return( hdr );
}

// Create a collective header for the common ALU case.  In this header:
// (1) the irq bit is always cleared
// (2) the tag is undefined
// (3) the injection checksum mode is set to 3 (include everything)
_BGL_TreeHwHdr *BGLTreeMakeAluHdr(
                     _BGL_TreeHwHdr *hdr,         // Filled in on return
                     int vt,                      // class (virtual tree)
                     int opcode,                  // ALU opcode
                     int opsize )                 // ALU operand size
{
   hdr->CtvHdr.Class    = vt;                    // Class number (a.k.a virtual tree number)
   hdr->CtvHdr.Ptp      = 0;                     // Must be 0 for collective header format.
   hdr->CtvHdr.Irq      = 0;                     // 1=request interrupt when received, 0=not
   hdr->CtvHdr.OpCode   = opcode;                // 000=ordinary routed packet, else ALU opcode
   hdr->CtvHdr.OpSize   = opsize;                // Operand size
   hdr->CtvHdr.Tag      = 0;                     // Software tag
   hdr->CtvHdr.CsumMode = _BGL_TREE_CSUM_ALL;    // Injection checksum mode
   return( hdr );
}

#ifdef NEVERDEFINED
// Software headers T.B.D.
_BGL_TreeSwHdr *BGLTreeMakeSwHdr(
                           _BGL_TreeSwHdr *hdr,         // Filled in on return
                           int vt
                           // T.B.D.
                           )
{
   return( hdr );
}
#endif

