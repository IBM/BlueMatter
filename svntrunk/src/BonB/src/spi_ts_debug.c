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
//     File:  spi_ts_debug.c
//
//  Purpose:  Misc. debugging routines for the Torus
//
//  Program:  Blade Network Interface Emulator
//
//   Author:  Mark E. Giampapa (giampapa@us.ibm.com)
//
//    Notes:
//
#include <BonB/blade_on_blrts.h>
#include <stdio.h>

#define DBG_PRINTF printf

extern "C" {
void _show_ts_hdr( _BGL_TorusPktHdr *hdr )
{
   DBG_PRINTF("_show_ts_hdr: hdr @ 0x%08lx\n", (Bit32)hdr );

   DBG_PRINTF("   CSumSkip = %d\n", hdr->hwh0.CSum_Skip  ); // Number of shorts (2B) to skip in CheckSum.
   DBG_PRINTF("   Sk       = %d\n", hdr->hwh0.Sk         ); // 0=use CSum_Skip, 1=Skip entire pkt.

   DBG_PRINTF("   Hint     = %d =", hdr->hwh0.Hint             ); // Hint Bits
   if ( hdr->hwh0.Hint & _BGL_TORUS_PKT_HINT_XP ) DBG_PRINTF(" X+");
   if ( hdr->hwh0.Hint & _BGL_TORUS_PKT_HINT_XM ) DBG_PRINTF(" X-");
   if ( hdr->hwh0.Hint & _BGL_TORUS_PKT_HINT_YP ) DBG_PRINTF(" Y+");
   if ( hdr->hwh0.Hint & _BGL_TORUS_PKT_HINT_YM ) DBG_PRINTF(" Y-");
   if ( hdr->hwh0.Hint & _BGL_TORUS_PKT_HINT_ZP ) DBG_PRINTF(" Z+");
   if ( hdr->hwh0.Hint & _BGL_TORUS_PKT_HINT_ZM ) DBG_PRINTF(" Z-");
   DBG_PRINTF(".\n");

   DBG_PRINTF("   Dp       = %d\n", hdr->hwh0.Dp         ); // Deposit Bit for MultiCast
   DBG_PRINTF("   Pid      = %d\n", hdr->hwh0.Pid        ); // Destination Fifo Group
   DBG_PRINTF("   Chunks   = %d\n", hdr->hwh0.Chunks     ); // Size in Chunks of 32B
   DBG_PRINTF("   SW_Avail = %d\n", hdr->hwh0.SW_Avail   ); // Available for S/W Use.
   DBG_PRINTF("   Dynamic  = %d\n", hdr->hwh0.Dynamic    ); // 1=Dynamic Routing, 0=Determinstic Routing.
   DBG_PRINTF("   VC       = %d\n", hdr->hwh0.VC         ); // Virtual Channel (0=D0,1=D1,2=BN,3=BP)
   DBG_PRINTF("   X        = %d\n", hdr->hwh0.X          ); // Destination X
   DBG_PRINTF("   Y        = %d\n", hdr->hwh1.Y          ); // Destination Y
   DBG_PRINTF("   Z        = %d\n", hdr->hwh1.Z          ); // Destination Z
   DBG_PRINTF("   Resvd0   = %d\n", hdr->hwh1.Resvd0     ); // Reserved
   DBG_PRINTF("   Resvd1   = %d\n", hdr->hwh1.Resvd1     ); // Reserved

   DBG_PRINTF("   Fcn      = 0x%08x\n",  hdr->swh0.fcn   ); // Function Pointer
   DBG_PRINTF("   Arg      = 0x%08lx\n", hdr->swh1.arg   ); // Function Argument
   DBG_PRINTF("   Extra    = 0x%08x\n",  hdr->swh0.extra ); // Function Extra Argument

}
};

void _ts_show_send_status( _BGL_TorusFifoStatus *stat, char *msg )
{

   DBG_PRINTF("%s i0=0x%02x i1=0x%02x i2=0x%02x iH=0x%02x\n",
          (msg ? msg : ""),
          stat->i[0], stat->i[1], stat->i[2], stat->iH );
}

void _ts_show_recv_status( _BGL_TorusFifoStatus *stat, char *msg )
{

   DBG_PRINTF("%s r0=0x%02x r1=0x%02x r2=0x%02x r3=0x%02x r4=0x%02x r5=0x%02x rH=0x%02x\n",
          (msg ? msg : ""),
          stat->r[0], stat->r[1], stat->r[2],
          stat->r[3], stat->r[4], stat->r[5],
          stat->rH );
}

