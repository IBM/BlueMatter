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
//      File: blade/spi/BGLTreeRawReceive.c
//
//   Purpose: Implement Blade's Tree Network System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Matthias A. Blumrich (blumrich@us.ibm.com)
//
//     Notes: This SPI is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the Tree.
//
//   History: 04/14/2003: MAB - Created.
//
//
#include <BonB/blade_on_blrts.h>
#include <string.h>  // for memcpy

#if !defined(__GNUC__)
extern "builtin" double _Complex __lfpd(volatile double *) ;
extern "builtin" void __stfpd(volatile double *, double _Complex) ;
extern "builtin" void __sync(void) ;
#endif

// Payload Buffers for Unaligned Accesses (H/W Hdrs never have to be buffered)
_BGL_TreePayload  _tr0_data_rec_bounce,
                  _tr1_data_rec_bounce;


////////////////////////////////////////////////////////////////////////////////
// _vc0_pkt_receive: Receive a tree packet (header and full payload) from VC 0.
//
//  Assumes payload destination address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__)
#define _vc0_pkt_receive( header, payload ) do { \
           int vc_base; \
           int hdr_reg; \
           int bytes_per_quad; \
           int payload_fifo; \
           asm volatile( "lis     %0,%c6@H      \n\t"  /* %0 = upper half of VC0 base address */        \
                         "li      %2,16         \n\t"  /* %2 = 16 bytes per quad */                     \
                         "lwz     %1,%c7(%0)    \n\t"  /* load HR into %1 */                            \
                         "la      %3,%c8(%0)    \n\t"  /* %3 = data reception address */                \
                         "lfpdx   0,0,%3        \n\t"  /* F0=Q0  load */                                \
                         "lfpdx   1,0,%3        \n\t"  /* F1=Q1  load */                                \
                         "lfpdx   2,0,%3        \n\t"  /* F2=Q2  load */                                \
                         "stw     %1,0(%4)      \n\t"  /* Store header to caller's header buffer */     \
                         "lfpdx   3,0,%3        \n\t"  /* F3=Q3  load */                                \
                         "lfpdx   4,0,%3        \n\t"  /* F4=Q4  load */                                \
                         "lfpdx   5,0,%3        \n\t"  /* F5=Q5  load */                                \
                         "stfpdx  0,0,%5        \n\t"  /* Q0  store to caller's payload buffer */       \
                         "stfpdux 1,%5,%2       \n\t"  /* Q1  store to (%5 += 16) */                    \
                         "stfpdux 2,%5,%2       \n\t"  /* Q2  store */                                  \
                         "lfpdx   6,0,%3        \n\t"  /* F6=Q6  load */                                \
                         "lfpdx   7,0,%3        \n\t"  /* F7=Q7  load */                                \
                         "lfpdx   8,0,%3        \n\t"  /* F8=Q8  load */                                \
                         "stfpdux 3,%5,%2       \n\t"  /* Q3  store */                                  \
                         "stfpdux 4,%5,%2       \n\t"  /* Q4  store */                                  \
                         "stfpdux 5,%5,%2       \n\t"  /* Q5  store */                                  \
                         "lfpdx   9,0,%3        \n\t"  /* F9=Q9  load */                                \
                         "lfpdx   0,0,%3        \n\t"  /* F0=Q10 load */                                \
                         "lfpdx   1,0,%3        \n\t"  /* F1=Q11 load */                                \
                         "stfpdux 6,%5,%2       \n\t"  /* Q6  store */                                  \
                         "stfpdux 7,%5,%2       \n\t"  /* Q7  store */                                  \
                         "stfpdux 8,%5,%2       \n\t"  /* Q8  store */                                  \
                         "lfpdx   2,0,%3        \n\t"  /* F2=Q12 load */                                \
                         "lfpdx   3,0,%3        \n\t"  /* F3=Q13 load */                                \
                         "lfpdx   4,0,%3        \n\t"  /* F4=Q14 load */                                \
                         "stfpdux 9,%5,%2       \n\t"  /* Q9  store */                                  \
                         "stfpdux 0,%5,%2       \n\t"  /* Q10 store */                                  \
                         "stfpdux 1,%5,%2       \n\t"  /* Q11 store */                                  \
                         "lfpdx   5,0,%3        \n\t"  /* F5=Q15 load */                                \
                         "stfpdux 2,%5,%2       \n\t"  /* Q12 store */                                  \
                         "stfpdux 3,%5,%2       \n\t"  /* Q13 store */                                  \
                         "stfpdux 4,%5,%2       \n\t"  /* Q14 store */                                  \
                         "stfpdux 5,%5,%2       \n\t"  /* Q15 store */                                  \
                         : "=r" (vc_base),     /* Temporaries */  \
                           "=r" (hdr_reg),                        \
                           "=r" (bytes_per_quad),                 \
                           "=r" (payload_fifo)                    \
                         : "g" (header),        /* Inputs */      \
                           "g" (payload),                         \
                           "i" (PA_TREE_VC0),   /* Constants */   \
                           "i" (TRx_HR),                          \
                           "i" (TRx_DR)                           \
                         : "fr0", "fr1", "fr2", /* Clobbers */    \
                           "fr3", "fr4", "fr5",                   \
                           "fr6", "fr7", "fr8",                   \
                           "fr9", "memory" );                     \
        } while(0)
#else
static inline void _vc0_pkt_receive(
       Bit32          *hdrHW,   // Where to put the hardware header (any type)
       void           *pyld    // Target address of payload (16-byte aligned)
       ) {
        *hdrHW = *(volatile Bit32 *) TR0_HR ;
       	volatile double * pyldv = (volatile double *)pyld ;
       	double _Complex data0 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data1 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data2 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data3 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data4 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data5 = __lfpd((volatile double *)TR0_DR) ;
       __stfpd(pyldv+0,data0) ;
       __stfpd(pyldv+2*1,data1) ;
       __stfpd(pyldv+2*2,data2) ;
       	double _Complex data6 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data7 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data8 = __lfpd((volatile double *)TR0_DR) ;
       __stfpd(pyldv+2*3,data3) ;
       __stfpd(pyldv+2*4,data4) ;
       __stfpd(pyldv+2*5,data5) ;
       	double _Complex data9 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data10 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data11 = __lfpd((volatile double *)TR0_DR) ;
       __stfpd(pyldv+2*6,data6) ;
       __stfpd(pyldv+2*7,data7) ;
       __stfpd(pyldv+2*8,data8) ;
       	double _Complex data12 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data13 = __lfpd((volatile double *)TR0_DR) ;
       	double _Complex data14 = __lfpd((volatile double *)TR0_DR) ;
       __stfpd(pyldv+2*9,data9) ;
       __stfpd(pyldv+2*10,data10) ;
       __stfpd(pyldv+2*11,data11) ;
       	double _Complex data15 = __lfpd((volatile double *)TR0_DR) ;
       __stfpd(pyldv+2*12,data12) ;
       __stfpd(pyldv+2*13,data13) ;
       __stfpd(pyldv+2*14,data14) ;
       __stfpd(pyldv+2*15,data15) ;
       }
#endif
////////////////////////////////////////////////////////////////////////////////
// _vc1_pkt_receive: Receive a tree packet (header and full payload) from VC 1.
//
//  Assumes payload destination address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__)
#define _vc1_pkt_receive( header, payload ) do { \
           int vc_base; \
           int hdr_reg; \
           int bytes_per_quad; \
           int payload_fifo; \
           asm volatile( "lis     %0,%c6@H      \n\t"  /* %0 = upper half of VC1 base address */        \
                         "li      %2,16         \n\t"  /* %2 = 16 bytes per quad */                     \
                         "lwz     %1,%c7(%0)    \n\t"  /* load HR into %1 */                            \
                         "la      %3,%c8(%0)    \n\t"  /* %3 = data reception address */                \
                         "lfpdx   0,0,%3        \n\t"  /* F0=Q0  load */                                \
                         "lfpdx   1,0,%3        \n\t"  /* F1=Q1  load */                                \
                         "lfpdx   2,0,%3        \n\t"  /* F2=Q2  load */                                \
                         "stw     %1,0(%4)      \n\t"  /* Store header to caller's header buffer */     \
                         "lfpdx   3,0,%3        \n\t"  /* F3=Q3  load */                                \
                         "lfpdx   4,0,%3        \n\t"  /* F4=Q4  load */                                \
                         "lfpdx   5,0,%3        \n\t"  /* F5=Q5  load */                                \
                         "stfpdx  0,0,%5        \n\t"  /* Q0  store to caller's payload buffer */       \
                         "stfpdux 1,%5,%2       \n\t"  /* Q1  store to (%5 += 16) */                    \
                         "stfpdux 2,%5,%2       \n\t"  /* Q2  store */                                  \
                         "lfpdx   6,0,%3        \n\t"  /* F6=Q6  load */                                \
                         "lfpdx   7,0,%3        \n\t"  /* F7=Q7  load */                                \
                         "lfpdx   8,0,%3        \n\t"  /* F8=Q8  load */                                \
                         "stfpdux 3,%5,%2       \n\t"  /* Q3  store */                                  \
                         "stfpdux 4,%5,%2       \n\t"  /* Q4  store */                                  \
                         "stfpdux 5,%5,%2       \n\t"  /* Q5  store */                                  \
                         "lfpdx   9,0,%3        \n\t"  /* F9=Q9  load */                                \
                         "lfpdx   0,0,%3        \n\t"  /* F0=Q10 load */                                \
                         "lfpdx   1,0,%3        \n\t"  /* F1=Q11 load */                                \
                         "stfpdux 6,%5,%2       \n\t"  /* Q6  store */                                  \
                         "stfpdux 7,%5,%2       \n\t"  /* Q7  store */                                  \
                         "stfpdux 8,%5,%2       \n\t"  /* Q8  store */                                  \
                         "lfpdx   2,0,%3        \n\t"  /* F2=Q12 load */                                \
                         "lfpdx   3,0,%3        \n\t"  /* F3=Q13 load */                                \
                         "lfpdx   4,0,%3        \n\t"  /* F4=Q14 load */                                \
                         "stfpdux 9,%5,%2       \n\t"  /* Q9  store */                                  \
                         "stfpdux 0,%5,%2       \n\t"  /* Q10 store */                                  \
                         "stfpdux 1,%5,%2       \n\t"  /* Q11 store */                                  \
                         "lfpdx   5,0,%3        \n\t"  /* F5=Q15 load */                                \
                         "stfpdux 2,%5,%2       \n\t"  /* Q12 store */                                  \
                         "stfpdux 3,%5,%2       \n\t"  /* Q13 store */                                  \
                         "stfpdux 4,%5,%2       \n\t"  /* Q14 store */                                  \
                         "stfpdux 5,%5,%2       \n\t"  /* Q15 store */                                  \
                         : "=r" (vc_base),     /* Temporaries */  \
                           "=r" (hdr_reg),                        \
                           "=r" (bytes_per_quad),                 \
                           "=r" (payload_fifo)                    \
                         : "g" (header),        /* Inputs */      \
                           "g" (payload),                         \
                           "i" (PA_TREE_VC1),   /* Constants */   \
                           "i" (TRx_HR),                          \
                           "i" (TRx_DR)                           \
                         : "fr0", "fr1", "fr2", /* Clobbers */    \
                           "fr3", "fr4", "fr5",                   \
                           "fr6", "fr7", "fr8",                   \
                           "fr9", "memory" );                     \
        } while(0)
#else
static inline void _vc1_pkt_receive(
       Bit32          *hdrHW,   // Where to put the hardware header (any type)
       void           *pyld    // Target address of payload (16-byte aligned)
       ) {
        *hdrHW = *(volatile Bit32 *) TR1_HR ;
       	volatile double * pyldv = (volatile double *)pyld ;
       	double _Complex data0 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data1 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data2 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data3 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data4 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data5 = __lfpd((volatile double *)TR1_DR) ;
       __stfpd(pyldv+0,data0) ;
       __stfpd(pyldv+2*1,data1) ;
       __stfpd(pyldv+2*2,data2) ;
       	double _Complex data6 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data7 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data8 = __lfpd((volatile double *)TR1_DR) ;
       __stfpd(pyldv+2*3,data3) ;
       __stfpd(pyldv+2*4,data4) ;
       __stfpd(pyldv+2*5,data5) ;
       	double _Complex data9 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data10 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data11 = __lfpd((volatile double *)TR1_DR) ;
       __stfpd(pyldv+2*6,data6) ;
       __stfpd(pyldv+2*7,data7) ;
       __stfpd(pyldv+2*8,data8) ;
       	double _Complex data12 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data13 = __lfpd((volatile double *)TR1_DR) ;
       	double _Complex data14 = __lfpd((volatile double *)TR1_DR) ;
       __stfpd(pyldv+2*9,data9) ;
       __stfpd(pyldv+2*10,data10) ;
       __stfpd(pyldv+2*11,data11) ;
       	double _Complex data15 = __lfpd((volatile double *)TR1_DR) ;
       __stfpd(pyldv+2*12,data12) ;
       __stfpd(pyldv+2*13,data13) ;
       __stfpd(pyldv+2*14,data14) ;
       __stfpd(pyldv+2*15,data15) ;
       }
#endif


////////////////////////////////////////////////////////////////////////////////
// Use for aligned payload of 16 quadwords.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawReceivePacket"
int BGLTreeRawReceivePacket(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld )   // Payload buffer (must be 256 bytes, qword-aligned)
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   int rc = 0;

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

#if defined(_SPI_CHECK_ALGN)
   if( ((Bit32)pyld) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": pyld=%x is not quadword aligned.\n", pyld ));
      _BLADE_ABORT( -1 );
   }
#endif
   BGLTreeGetStatus( vc, &stat );
   if ( stat.RecHdrCount == 0 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": No data in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
   if ( stat.RecPyldCount < 16 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": Insufficient data in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( vc ) {
      _vc1_pkt_receive( hdr, pyld );
   } else {
      _vc0_pkt_receive( hdr, pyld );
   }

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for aligned payload of 1-16 quadwords.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawReceivePacketPartial"
int BGLTreeRawReceivePacketPartial(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld,    // Payload buffer (must be 256 bytes, qword-aligned)
       int            bytes )   // Payload bytes (multiple of 16, up to 256)
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   Bit128 *tr_dr, *q1, *q2, *q3;
   int i;
   int rc = 0;

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

#if defined(_SPI_CHECK_ALGN)
   if( ((Bit32)pyld) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": pyld=%x is not quadword aligned.\n", pyld ));
      _BLADE_ABORT( -1 );
   }
#endif
   if ( (bytes < 16) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": bytes=%d out of range 16-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   if( ((Bit32)bytes) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": Invalid bytes=%d argument is not a multiple of 16.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.RecHdrCount == 0 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": No data in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
   if ( stat.RecPyldCount < 16 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": Insufficient data in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( vc ) {

      // Receive header.
      *hdr = in32( (Bit32 *)TR1_HR );

      // Prepare to receive payload.
      tr_dr = (Bit128 *)TR1_DR;

   } else {

      // Receive header.
      *hdr = in32( (Bit32 *)TR0_HR );

      // Prepare to receive payload.
      tr_dr = (Bit128 *)TR0_DR;

   }

   q1 = (Bit128 *)pyld;
   q2 = q1+1;
   q3 = q1+2;

   // bytes will be number of quadwords.
   bytes >>= 4;

   for ( i = 0 ; i < (bytes/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadLoad  (tr_dr, 6);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
      QuadStore (q3, 6);
#else
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q3v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      __stfpd((volatile double *)q3, q3v) ;

#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadStore (q1, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      }
#endif
      
   }

   // Dump remainder of packet, if necessary.
   for( i = bytes; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
#else
      __lfpd((volatile double *)tr_dr) ;
#endif

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for unaligned payload or bytes = 1-256.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawReceivePacketUnaligned"
int BGLTreeRawReceivePacketUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld,    // Payload buffer
       int            bytes )   // Payload bytes (1-256)
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   Bit128 *tr_dr, *q1, *q2, *q3;
   int bytes16;
   int i;
   int rc = 0;
   char *sp, *dp;
   int  count;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x, 0x%.8x, %d ) called.\n", vc, (int)hdrHW, (int)pyld, bytes );
#endif

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

   if ( (bytes < 1) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": bytes=%d out of range 0-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.RecHdrCount == 0 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": No data in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
   if ( stat.RecPyldCount < 16 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": No data in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Round bytes to next multiple of 16.
   bytes16 = bytes & 0xfffffff0;
   if( bytes != bytes16 ) {
      bytes16 += 16;
   }

   if( vc ) {

      // Receive header.
      *hdr = in32( (Bit32 *)TR1_HR );

      // Prepare to receive payload.
      tr_dr = (Bit128 *)TR1_DR;
      q1 = (Bit128 *)&_tr1_data_rec_bounce;

   } else {

      // Receive header.
      *hdr = in32( (Bit32 *)TR0_HR );

      // Prepare to receive payload.
      tr_dr = (Bit128 *)TR0_DR;
      q1 = (Bit128 *)&_tr0_data_rec_bounce;

   }

   q2 = q1+1;
   q3 = q1+2;

   // bytes16 will be number of quadwords.
   bytes16 >>= 4;

   for ( i = 0 ; i < (bytes16/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadLoad  (tr_dr, 6);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
      QuadStore (q3, 6);
#else
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q3v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      __stfpd((volatile double *)q3, q3v) ;

#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes16-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadStore (q1, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      }
#endif
   }

   // Dump remainder of packet, if necessary.
   for( i = bytes16; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
#else
      __lfpd((volatile double *)tr_dr) ;
#endif

   // Copy to unaligned destination.
   if( vc ) {
      //memcpy( pyld, &(_tr1_data_rec_bounce.flat[0]), bytes );
      for( count = bytes, dp = (char *)pyld, sp=(char *)&(_tr1_data_rec_bounce.flat[0]); count > 0; count-- )
         *dp++ = *sp++;
   } else {
      //memcpy( pyld, &(_tr0_data_rec_bounce.flat[0]), bytes );
      for( count = bytes, dp = (char *)pyld, sp=(char *)&(_tr0_data_rec_bounce.flat[0]); count > 0; count-- )
         *dp++ = *sp++;
   }

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for aligned payload of 1-16 quadwords.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawReceivePayloadPartial"
int BGLTreeRawReceivePayloadPartial(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Payload buffer (must be 256 bytes, qword-aligned)
       int            bytes )   // Payload bytes (multiple of 16, up to 256)
{
   Bit128 *tr_dr, *q1, *q2, *q3;
   int i;
   int rc = 0;

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

#if defined(_SPI_CHECK_ALGN)
   if( ((Bit32)pyld) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": pyld=%x is not quadword aligned.\n", pyld ));
      _BLADE_ABORT( -1 );
   }
#endif
   if ( (bytes < 16) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": bytes=%d out of range 16-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   if( ((Bit32)bytes) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": Invalid bytes=%d argument is not a multiple of 16.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.RecPyldCount < 16 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": Insufficient data in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( vc )
      tr_dr = (Bit128 *)TR1_DR;
   else
      tr_dr = (Bit128 *)TR0_DR;

   q1 = (Bit128 *)pyld;
   q2 = q1+1;
   q3 = q1+2;

   // bytes will be number of quadwords.
   bytes >>= 4;

   for ( i = 0 ; i < (bytes/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadLoad  (tr_dr, 6);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
      QuadStore (q3, 6);
#else
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q3v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      __stfpd((volatile double *)q3, q3v) ;

#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadStore (q1, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      }
#endif
   }

   // Dump remainder of packet, if necessary.
   for( i = bytes; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
#else
      __lfpd((volatile double *)tr_dr) ;
#endif

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for unaligned payload or bytes = 1-256.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawReceivePayloadUnaligned"
int BGLTreeRawReceivePayloadUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Payload buffer
       int            bytes )   // Payload bytes (1-256)
{
   Bit128 *tr_dr, *q1, *q2, *q3;
   int bytes16;
   int i;
   int rc = 0;

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

   if ( (bytes < 1) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": bytes=%d out of range 1-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.RecPyldCount < 16 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": Insufficient data in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Round bytes to next multiple of 16.
   bytes16 = bytes & 0xfffffff0;
   if( bytes != bytes16 ) {
      bytes16 += 16;
   }

   // Prepare to receive payload.
   if( vc ) {

      tr_dr = (Bit128 *)TR1_DR;
      q1 = (Bit128 *)&_tr1_data_rec_bounce;

   } else {

      tr_dr = (Bit128 *)TR0_DR;
      q1 = (Bit128 *)&_tr0_data_rec_bounce;

   }

   q2 = q1+1;
   q3 = q1+2;

   // bytes16 will be number of quadwords.
   bytes16 >>= 4;

   for ( i = 0 ; i < (bytes16/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadLoad  (tr_dr, 6);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
      QuadStore (q3, 6);
#else
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q3v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      __stfpd((volatile double *)q3, q3v) ;

#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes16-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadStore (q1, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      }
#endif
   }

   // Dump remainder of packet, if necessary.
   for( i = bytes16; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
#else
      __lfpd((volatile double *)tr_dr) ;
#endif

   // Copy to unaligned destination.
   if( vc )
      memcpy( pyld, &(_tr1_data_rec_bounce.flat[0]), bytes );
   else
      memcpy( pyld, &(_tr0_data_rec_bounce.flat[0]), bytes );

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Receive a single header.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawReceiveHeader"
int BGLTreeRawReceiveHeader(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW )  // Hardware header buffer
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   int rc = 0;

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

   BGLTreeGetStatus( vc, &stat );
   if ( stat.RecHdrCount == 0 ) {
      TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": No data in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( vc )
      *hdr = in32( (Bit32 *)TR1_HR );
   else
      *hdr = in32( (Bit32 *)TR0_HR );

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for unaligned payload or bytes = 1-256. This function will not dump the
// remainder of a payload, but will dump the remainder of a quadword.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawReceiveBytesUnaligned"
int BGLTreeRawReceiveBytesUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload
       int            bytes )   // Payload bytes (1-256)
{
   Bit128 *tr_dr, *q1, *q2, *q3;
   int bytes16;
   int i;
   int rc = 0;

   // Round bytes to next multiple of 16.
   bytes16 = bytes & 0xfffffff0;
   if( bytes != bytes16 ) {
      bytes16 += 16;
   }

#ifdef _SPI_CHECK
   {
      _BGL_TreeFifoStatus stat;

      if ( (bytes < 1) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
         TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": bytes=%d out of range 1-256.\n", bytes ));
         _BLADE_ABORT( -1 );
      }
      BGLTreeGetStatus( vc, &stat );
      if ( stat.RecPyldCount < (unsigned)(bytes16 >> 4) ) {
         TrSPI_TRC( TrSPI_TRC_RECV,( FN_NAME": Insufficient data in VC%d payload fifo.\n", vc ));
         _BLADE_ABORT( -1 );
      }
   }
#endif

   // Prepare to receive payload.
   if( vc ) {

      tr_dr = (Bit128 *)TR1_DR;
      q1 = (Bit128 *)&_tr1_data_rec_bounce;

   } else {

      tr_dr = (Bit128 *)TR0_DR;
      q1 = (Bit128 *)&_tr0_data_rec_bounce;

   }

   q2 = q1+1;
   q3 = q1+2;

   // bytes16 will be number of quadwords.
   bytes16 >>= 4;

   for ( i = 0 ; i < (bytes16/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadLoad  (tr_dr, 6);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
      QuadStore (q3, 6);
#else
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q3v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      __stfpd((volatile double *)q3, q3v) ;

#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes16-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadStore (q1, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (tr_dr, 4);
      QuadLoad  (tr_dr, 5);
      QuadStore (q1, 4);
      QuadStore (q2, 5);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)tr_dr) ;
      double _Complex q2v = __lfpd((volatile double *)tr_dr) ;
      __stfpd((volatile double *)q1, q1v) ;
      __stfpd((volatile double *)q2, q2v) ;
      }
#endif
   }

   // Copy to unaligned destination.
   if( vc )
      memcpy( pyld, &(_tr1_data_rec_bounce.flat[0]), bytes );
   else
      memcpy( pyld, &(_tr0_data_rec_bounce.flat[0]), bytes );

   return( rc );
}

