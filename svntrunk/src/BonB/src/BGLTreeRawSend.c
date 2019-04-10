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
//      File: blade/spi/BGLTreeRawSend.c
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
#include <BonB/blade_on_blrts.h>
#include <string.h>  // for memcpy

#if !defined(__GNUC__)
extern "builtin" double _Complex __lfpd(volatile double *) ;
extern "builtin" void __stfpd(volatile double *, double _Complex) ;
extern "builtin" void __sync(void) ;
#endif

// Payload Buffers for Unaligned Accesses.
_BGL_TreePayload  _tr0_data_inj_bounce,
                  _tr1_data_inj_bounce;

// Fixed zero for partial packet fill
Bit64  _BGL_ZeroBit64[2] ALIGN_CACHE = { 0ULL, 0ULL };
Bit128 *_BGL_ZeroQuad = (Bit128 *)_BGL_ZeroBit64;

////////////////////////////////////////////////////////////////////////////////
// _vc0_pkt_inject: Inject a tree packet (header and full payload) to VC 0.
//
//  Assumes payload source address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__)
#define _vc0_pkt_inject(header, payload) do { \
           int vc_base; \
           int hdr_reg; \
           int bytes_per_quad; \
           int payload_fifo; \
           asm volatile( "lis     %0,%c6@H      \n\t"  /* %0 = upper half of VC0 base address */  \
                     "lwz     %1,0(%4)      \n\t"  /* %1 = header */                          \
                     "li      %2,16         \n\t"  /* %2 = 16 bytes per quad */               \
                     "lfpdx   0,0,%5        \n\t"  /* F0=Q0 load */                           \
                     "stw     %1,%c7(%0)    \n\t"  /* store header to HI offset from VC0 base */ \
                     "la      %3,%c8(%0)    \n\t"  /* %3 = data injection address */          \
                     "lfpdux  1,%5,%2       \n\t"  /* F1=Q1 load from (%5 += 16) */           \
                     "lfpdux  2,%5,%2       \n\t"  /* F2=Q2 load */                           \
                     "lfpdux  3,%5,%2       \n\t"  /* F3=Q3 load */                           \
                     "stfpdx  0,0,%3        \n\t"  /* Q0 store to TR0_DI */                   \
                     "lfpdux  4,%5,%2       \n\t"  /* F4=Q4 load */                           \
                     "lfpdux  5,%5,%2       \n\t"  /* F5=Q5 load */                           \
                     "lfpdux  6,%5,%2       \n\t"  /* F6=Q6 load */                           \
                     "stfpdx  1,0,%3        \n\t"  /* Q1 store */                             \
                     "stfpdx  2,0,%3        \n\t"  /* Q2 store */                             \
                     "stfpdx  3,0,%3        \n\t"  /* Q3 store */                             \
                     "lfpdux  7,%5,%2       \n\t"  /* F7=Q7 load */                           \
                     "lfpdux  8,%5,%2       \n\t"  /* F8=Q8 load */                           \
                     "lfpdux  9,%5,%2       \n\t"  /* F9=Q9 load */                           \
                     "stfpdx  4,0,%3        \n\t"  /* Q4 store */                             \
                     "stfpdx  5,0,%3        \n\t"  /* Q5 store */                             \
                     "stfpdx  6,0,%3        \n\t"  /* Q6 store */                             \
                     "lfpdux  0,%5,%2       \n\t"  /* F0=Q10 load */                          \
                     "lfpdux  1,%5,%2       \n\t"  /* F1=Q11 load */                          \
                     "lfpdux  2,%5,%2       \n\t"  /* F2=Q12 load */                          \
                     "stfpdx  7,0,%3        \n\t"  /* Q7 store */                             \
                     "stfpdx  8,0,%3        \n\t"  /* Q8 store */                             \
                     "stfpdx  9,0,%3        \n\t"  /* Q9 store */                             \
                     "lfpdux  3,%5,%2       \n\t"  /* F3=Q13 load */                          \
                     "lfpdux  4,%5,%2       \n\t"  /* F4=Q14 load */                          \
                     "lfpdux  5,%5,%2       \n\t"  /* F5=Q15 load */                          \
                     "stfpdx  0,0,%3        \n\t"  /* Q10 store */                            \
                     "stfpdx  1,0,%3        \n\t"  /* Q11 store */                            \
                     "stfpdx  2,0,%3        \n\t"  /* Q12 store */                            \
                     "stfpdx  3,0,%3        \n\t"  /* Q13 store */                            \
                     "stfpdx  4,0,%3        \n\t"  /* Q14 store */                            \
                     "stfpdx  5,0,%3        \n\t"  /* Q15 store */                            \
                     : "=b" (vc_base),     /* Temporaries */  \
                       "=b" (hdr_reg),                        \
                       "=b" (bytes_per_quad),                 \
                       "=b" (payload_fifo)                    \
                     : "g" (header),        /* Inputs */      \
                       "g" (payload),                         \
                       "i" (PA_TREE_VC0),   /* Constants */   \
                       "i" (TRx_HI),                          \
                       "i" (TRx_DI)                           \
                     : "fr0", "fr1", "fr2", /* Clobbers */    \
                       "fr3", "fr4", "fr5",                   \
                       "fr6", "fr7", "fr8",                   \
                       "fr9", "memory" );                     \
        } while(0)
#else        
static inline void _vc0_pkt_inject(
       Bit32          *hdrHW,   // Previously created hardware header (any type)
       void           *pyld    // Source address of payload (16-byte aligned)
       ) {
       	volatile double * pyldv = (volatile double *)pyld ;
       	double _Complex data0 = __lfpd(pyldv) ;
       	*(volatile Bit32 *) TR0_HI = *hdrHW ;
       	double _Complex data1 = __lfpd(pyldv+2*1) ;
       	double _Complex data2 = __lfpd(pyldv+2*2) ;
       	double _Complex data3 = __lfpd(pyldv+2*3) ;
       	__stfpd((volatile double *)TR0_DI, data0) ;
       	double _Complex data4 = __lfpd(pyldv+2*4) ;
       	double _Complex data5 = __lfpd(pyldv+2*5) ;
       	double _Complex data6 = __lfpd(pyldv+2*6) ;
       	__stfpd((volatile double *)TR0_DI, data1) ;
       	__stfpd((volatile double *)TR0_DI, data2) ;
       	__stfpd((volatile double *)TR0_DI, data3) ;
       	double _Complex data7 = __lfpd(pyldv+2*7) ;
       	double _Complex data8 = __lfpd(pyldv+2*8) ;
       	double _Complex data9 = __lfpd(pyldv+2*9) ;
       	__stfpd((volatile double *)TR0_DI, data4) ;
       	__stfpd((volatile double *)TR0_DI, data5) ;
       	__stfpd((volatile double *)TR0_DI, data6) ;
       	double _Complex data10 = __lfpd(pyldv+2*10) ;
       	double _Complex data11 = __lfpd(pyldv+2*11) ;
       	double _Complex data12 = __lfpd(pyldv+2*12) ;
       	__stfpd((volatile double *)TR0_DI, data7) ;
       	__stfpd((volatile double *)TR0_DI, data8) ;
       	__stfpd((volatile double *)TR0_DI, data9) ;
       	double _Complex data13 = __lfpd(pyldv+2*13) ;
       	double _Complex data14 = __lfpd(pyldv+2*14) ;
       	double _Complex data15 = __lfpd(pyldv+2*15) ;
       	__stfpd((volatile double *)TR0_DI, data10) ;
       	__stfpd((volatile double *)TR0_DI, data11) ;
       	__stfpd((volatile double *)TR0_DI, data12) ;
       	__stfpd((volatile double *)TR0_DI, data13) ;
       	__stfpd((volatile double *)TR0_DI, data14) ;
       	__stfpd((volatile double *)TR0_DI, data15) ;
       	
       }
#endif       

////////////////////////////////////////////////////////////////////////////////
// _vc1_pkt_inject: Inject a tree packet (header and full payload) to VC 1.
//
//  Assumes payload source address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__)
#define _vc1_pkt_inject(header, payload) do { \
           int vc_base; \
           int hdr_reg; \
           int bytes_per_quad; \
           int payload_fifo; \
           asm volatile( "lis     %0,%c6@H      \n\t"  /* %0 = upper half of VC1 base address */  \
                         "lwz     %1,0(%4)      \n\t"  /* %1 = header */                          \
                         "li      %2,16         \n\t"  /* %2 = 16 bytes per quad */               \
                         "lfpdx   0,0,%5        \n\t"  /* F0=Q0 load */                           \
                         "stw     %1,%c7(%0)    \n\t"  /* store header to HI offset from VC1 base */    \
                         "la      %3,%c8(%0)    \n\t"  /* %3 = data injection address */                \
                         "lfpdux  1,%5,%2 \n\t"  /* F1=Q1 load from (%5 += 16) */                 \
                         "lfpdux  2,%5,%2 \n\t"  /* F2=Q2 load */                                 \
                         "lfpdux  3,%5,%2 \n\t"  /* F3=Q3 load */                                 \
                         "stfpdx  0,0,%3        \n\t"  /* Q0 store to TR1_DI */                   \
                         "lfpdux  4,%5,%2       \n\t"  /* F4=Q4 load */                           \
                         "lfpdux  5,%5,%2       \n\t"  /* F5=Q5 load */                           \
                         "lfpdux  6,%5,%2       \n\t"  /* F6=Q6 load */                           \
                         "stfpdx  1,0,%3        \n\t"  /* Q1 store */                             \
                         "stfpdx  2,0,%3        \n\t"  /* Q2 store */                             \
                         "stfpdx  3,0,%3        \n\t"  /* Q3 store */                             \
                         "lfpdux  7,%5,%2       \n\t"  /* F7=Q7 load */                           \
                         "lfpdux  8,%5,%2 \n\t"  /* F8=Q8 load */                           \
                         "lfpdux  9,%5,%2 \n\t"  /* F9=Q9 load */                           \
                         "stfpdx  4,0,%3  \n\t"  /* Q4 store */                             \
                         "stfpdx  5,0,%3  \n\t"  /* Q5 store */                             \
                         "stfpdx  6,0,%3  \n\t"  /* Q6 store */                             \
                         "lfpdux  0,%5,%2 \n\t"  /* F0=Q10 load */                          \
                         "lfpdux  1,%5,%2 \n\t"  /* F1=Q11 load */                          \
                         "lfpdux  2,%5,%2 \n\t"  /* F2=Q12 load */                          \
                         "stfpdx  7,0,%3  \n\t"  /* Q7 store */                             \
                         "stfpdx  8,0,%3  \n\t"  /* Q8 store */                             \
                         "stfpdx  9,0,%3  \n\t"  /* Q9 store */                             \
                         "lfpdux  3,%5,%2 \n\t"  /* F3=Q13 load */                          \
                         "lfpdux  4,%5,%2 \n\t"  /* F4=Q14 load */                          \
                         "lfpdux  5,%5,%2 \n\t"  /* F5=Q15 load */                          \
                         "stfpdx  0,0,%3  \n\t"  /* Q10 store */                            \
                         "stfpdx  1,0,%3  \n\t"  /* Q11 store */                            \
                         "stfpdx  2,0,%3  \n\t"  /* Q12 store */                            \
                         "stfpdx  3,0,%3  \n\t"  /* Q13 store */                            \
                         "stfpdx  4,0,%3  \n\t"  /* Q14 store */                            \
                         "stfpdx  5,0,%3  \n\t"  /* Q15 store */                            \
                         : "=r" (vc_base),      /* Temporaries */  \
                           "=r" (hdr_reg),                   \
                           "=r" (bytes_per_quad),            \
                           "=r" (payload_fifo)               \
                         : "g" (header),        /* Inputs */ \
                           "g" (payload),                    \
                           "i" (PA_TREE_VC1),   /* Constants */    \
                           "i" (TRx_HI),                     \
                           "i" (TRx_DI)                      \
                         : "fr0", "fr1", "fr2", /* Clobbers */     \
                           "fr3", "fr4", "fr5",              \
                           "fr6", "fr7", "fr8",              \
                           "fr9", "memory" );                \
        } while(0)
#else        
static inline void _vc1_pkt_inject(
       Bit32          *hdrHW,   // Previously created hardware header (any type)
       void           *pyld    // Source address of payload (16-byte aligned)
       ) {
       	volatile double * pyldv = (volatile double *)pyld ;
       	double _Complex data0 = __lfpd(pyldv) ;
       	*(volatile Bit32 *) TR1_HI = *hdrHW ;
       	double _Complex data1 = __lfpd(pyldv+2*1) ;
       	double _Complex data2 = __lfpd(pyldv+2*2) ;
       	double _Complex data3 = __lfpd(pyldv+2*3) ;
       	__stfpd((volatile double *)TR1_DI, data0) ;
       	double _Complex data4 = __lfpd(pyldv+2*4) ;
       	double _Complex data5 = __lfpd(pyldv+2*5) ;
       	double _Complex data6 = __lfpd(pyldv+2*6) ;
       	__stfpd((volatile double *)TR1_DI, data1) ;
       	__stfpd((volatile double *)TR1_DI, data2) ;
       	__stfpd((volatile double *)TR1_DI, data3) ;
       	double _Complex data7 = __lfpd(pyldv+2*7) ;
       	double _Complex data8 = __lfpd(pyldv+2*8) ;
       	double _Complex data9 = __lfpd(pyldv+2*9) ;
       	__stfpd((volatile double *)TR1_DI, data4) ;
       	__stfpd((volatile double *)TR1_DI, data5) ;
       	__stfpd((volatile double *)TR1_DI, data6) ;
       	double _Complex data10 = __lfpd(pyldv+2*10) ;
       	double _Complex data11 = __lfpd(pyldv+2*11) ;
       	double _Complex data12 = __lfpd(pyldv+2*12) ;
       	__stfpd((volatile double *)TR1_DI, data7) ;
       	__stfpd((volatile double *)TR1_DI, data8) ;
       	__stfpd((volatile double *)TR1_DI, data9) ;
       	double _Complex data13 = __lfpd(pyldv+2*13) ;
       	double _Complex data14 = __lfpd(pyldv+2*14) ;
       	double _Complex data15 = __lfpd(pyldv+2*15) ;
       	__stfpd((volatile double *)TR1_DI, data10) ;
       	__stfpd((volatile double *)TR1_DI, data11) ;
       	__stfpd((volatile double *)TR1_DI, data12) ;
       	__stfpd((volatile double *)TR1_DI, data13) ;
       	__stfpd((volatile double *)TR1_DI, data14) ;
       	__stfpd((volatile double *)TR1_DI, data15) ;
       	
       }
#endif       


////////////////////////////////////////////////////////////////////////////////
// Use for aligned payload of 16 quadwords.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawSendPacket"
int BGLTreeRawSendPacket(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld )   // Source address of payload (16-byte aligned)
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   int rc = 0;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x, 0x%.8x ) called.\n", vc, (int)hdrHW, (int)pyld );
#endif

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

#if defined(_SPI_CHECK_ALGN)
   if( ((Bit32)pyld) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Invalid pyld argument alignment.\n" ));
      _BLADE_ABORT( -1 );
   }
#endif

   BGLTreeGetStatus( vc, &stat );
   if ( stat.InjHdrCount == 8 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
   if ( stat.InjPyldCount > 112 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( vc ) {
      _vc1_pkt_inject(hdr, pyld);
   } else {
      _vc0_pkt_inject(hdr, pyld);
   }

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for aligned payload of 1-16 quadwords.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawSendPacketPartial"
int BGLTreeRawSendPacketPartial(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload (16-byte aligned)
       int            bytes )   // Payload bytes (multiple of 16, up to 256)
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   Bit128 *tr_di, *q1, *q2, *q3;
   int i;
   int rc = 0;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x, 0x%.8x, %d ) called.\n", vc, (int)hdrHW, (int)pyld, bytes );
#endif

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

#if defined(_SPI_CHECK_ALGN)
   if( ((Bit32)pyld) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Invalid pyld argument alignment.\n" ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( ((Bit32)bytes) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Invalid bytes argument (not multiple of 16).\n" ));
      _BLADE_ABORT( -1 );
   }
   if ( (bytes < 16) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d out of range 16-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.InjHdrCount == 8 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
   if ( stat.InjPyldCount > 112 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( vc ) {

      // Inject header.
      *( (volatile Bit32 *)TR1_HI ) = *hdr;

      tr_di = (Bit128 *)TR1_DI;

   } else {

      // Inject header.
      *( (volatile Bit32 *)TR0_HI ) = *hdr;

      tr_di = (Bit128 *)TR0_DI;

   }

   // Inject payload.
   q1 = (Bit128 *)pyld;
   q2 = q1+1;
   q3 = q1+2;

   // bytes will be number of quadwords.
   bytes >>= 4;

   for ( i = 0 ; i < (bytes/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadLoad  (q3, 5);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
      QuadStore (tr_di, 5);
#else
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;
      double _Complex q3v = __lfpd((volatile double *)q3) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      __stfpd((volatile double *)tr_di, q3v) ;
#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadStore (tr_di, 3);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;      
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      }
#endif
   }

   // Fill remainder of packet with 0, if necessary.
#if defined(__GNUC__)
   QuadLoad  (_BGL_ZeroQuad, 3);
#else
      double _Complex qz = __lfpd((volatile double *)_BGL_ZeroQuad) ;
#endif      
   for( i = bytes; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadStore (tr_di, 3);
#else
      __stfpd((volatile double *)tr_di, qz) ;
#endif      

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for unaligned payload or bytes = 1-256.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawSendPacketUnaligned"
int BGLTreeRawSendPacketUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload
       int            bytes )   // Payload bytes (1-256)
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   Bit128 *tr_di, *q1, *q2, *q3;
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
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d out of range 0-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.InjHdrCount == 8 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
   if ( stat.InjPyldCount > 112 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Round bytes to next multiple of 16.
   bytes16 = bytes & 0xfffffff0;
   if( bytes != bytes16 ) {
      bytes16 += 16;
   }

   if( vc ) {

      // Inject header.
      *( (volatile Bit32 *)TR1_HI ) = *hdr;

      // Prepare to inject payload.
      //memcpy( &(_tr1_data_inj_bounce.flat[0]), pyld, bytes );
      for( count = bytes, sp = (char *)pyld, dp=(char *)&(_tr1_data_inj_bounce.flat[0]); count > 0; count-- )
         *dp++ = *sp++;

      tr_di = (Bit128 *)TR1_DI;
      q1 = (Bit128 *)&_tr1_data_inj_bounce;

   } else {

      // Inject header.
      *( (volatile Bit32 *)TR0_HI ) = *hdr;

      // Prepare to inject payload.
      //memcpy( &(_tr0_data_inj_bounce.flat[0]), pyld, bytes );
      for( count = bytes, sp = (char *)pyld, dp=(char *)&(_tr0_data_inj_bounce.flat[0]); count > 0; count-- )
         *dp++ = *sp++;

      tr_di = (Bit128 *)TR0_DI;
      q1 = (Bit128 *)&_tr0_data_inj_bounce;

   }

   // Inject payload.
   q2 = q1+1;
   q3 = q1+2;

   // bytes16 will be number of quadwords.
   bytes16 >>= 4;

   for ( i = 0 ; i < (bytes16/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadLoad  (q3, 5);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
      QuadStore (tr_di, 5);
#else
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;
      double _Complex q3v = __lfpd((volatile double *)q3) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      __stfpd((volatile double *)tr_di, q3v) ;
#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes16-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadStore (tr_di, 3);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;      
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      }
#endif
   }

   // Fill remainder of packet, if necessary.
#if defined(__GNUC__)
   QuadLoad  (_BGL_ZeroQuad, 3);
#else
      double _Complex qz = __lfpd((volatile double *)_BGL_ZeroQuad) ;
#endif      
   for( i = bytes16; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadStore (tr_di, 3);
#else
      __stfpd((volatile double *)tr_di, qz) ;
#endif      

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for aligned payload of 1-16 quadwords.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawSendPayloadPartial"
int BGLTreeRawSendPayloadPartial(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload (16-byte aligned)
       int            bytes )   // Payload bytes (multiple of 16, up to 256)
{
   Bit128 *tr_di, *q1, *q2, *q3;
   int i;
   int rc = 0;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x, %d ) called.\n", vc, (int)pyld, bytes );
#endif

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

#if defined(_SPI_CHECK_ALGN)
   if( ((Bit32)pyld) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Invalid pyld argument alignment.\n" ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( ((Bit32)bytes) & 0xf ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Invalid bytes argument (not multiple of 16).\n" ));
      _BLADE_ABORT( -1 );
   }
   if ( (bytes < 16) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d out of range 16-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.InjPyldCount > 112 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( vc ) {
      tr_di = (Bit128 *)TR1_DI;
   } else {
      tr_di = (Bit128 *)TR0_DI;
   }

   // Inject payload.
   q1 = (Bit128 *)pyld;
   q2 = q1+1;
   q3 = q1+2;

   // bytes will be number of quadwords.
   bytes >>= 4;

   for ( i = 0 ; i < (bytes/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadLoad  (q3, 5);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
      QuadStore (tr_di, 5);
#else
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;
      double _Complex q3v = __lfpd((volatile double *)q3) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      __stfpd((volatile double *)tr_di, q3v) ;
#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadStore (tr_di, 3);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;      
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      }
#endif
   }

   // Fill remainder of packet, if necessary.
#if defined(__GNUC__)
   QuadLoad  (_BGL_ZeroQuad, 3);
#else
      double _Complex qz = __lfpd((volatile double *)_BGL_ZeroQuad) ;
#endif      
   for( i = bytes; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadStore (tr_di, 3);
#else
      __stfpd((volatile double *)tr_di, qz) ;
#endif      

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for unaligned payload or bytes = 1-256.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawSendPayloadUnaligned"
int BGLTreeRawSendPayloadUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload
       int            bytes )   // Payload bytes
{
   Bit128 *tr_di, *q1, *q2, *q3;
   int bytes16;
   int i;
   int rc = 0;

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

   if ( (bytes < 1) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d out of range 0-256.\n", bytes ));
      _BLADE_ABORT( -1 );
   }
   BGLTreeGetStatus( vc, &stat );
   if ( stat.InjPyldCount > 112 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d payload fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Round bytes to next multiple of 16.
   bytes16 = bytes & 0xfffffff0;
   if( bytes != bytes16 ) {
      bytes16 += 16;
   }

   if( vc ) {

      // Prepare to inject payload.
      memcpy( &(_tr1_data_inj_bounce.flat[0]), pyld, bytes );

      tr_di = (Bit128 *)TR1_DI;
      q1 = (Bit128 *)&_tr1_data_inj_bounce;

   } else {

      // Prepare to inject payload.
      memcpy( &(_tr0_data_inj_bounce.flat[0]), pyld, bytes );

      tr_di = (Bit128 *)TR0_DI;
      q1 = (Bit128 *)&_tr0_data_inj_bounce;

   }

   // Inject payload.
   q2 = q1+1;
   q3 = q1+2;

   // bytes16 will be number of quadwords.
   bytes16 >>= 4;

   for ( i = 0 ; i < (bytes16/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadLoad  (q3, 5);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
      QuadStore (tr_di, 5);
#else
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;
      double _Complex q3v = __lfpd((volatile double *)q3) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      __stfpd((volatile double *)tr_di, q3v) ;
#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes16-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadStore (tr_di, 3);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;      
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      }
#endif
   }

   // Fill remainder of packet, if necessary.
#if defined(__GNUC__)
   QuadLoad  (_BGL_ZeroQuad, 3);
#else
      double _Complex qz = __lfpd((volatile double *)_BGL_ZeroQuad) ;
#endif      
   for( i = bytes16; i < (_BGL_TREE_PKT_MAX_BYTES >> 4); i++ )
#if defined(__GNUC__)
      QuadStore (tr_di, 3);
#else
      __stfpd((volatile double *)tr_di, qz) ;
#endif      

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Inject a single header.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawSendHeader"
int BGLTreeRawSendHeader(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW )  // Previously created hardware header (any type)
{
   Bit32 *hdr = (Bit32 *)hdrHW;
   int rc = 0;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x ) called.\n", vc, (int)hdrHW );
#endif

#ifdef _SPI_CHECK
   _BGL_TreeFifoStatus stat;

   BGLTreeGetStatus( vc, &stat );
   if ( stat.InjHdrCount == 8 ) {
      TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d header fifo.\n", vc ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Inject header.
   if( vc ) {
      *( (volatile Bit32 *)TR1_HI ) = *hdr;
   } else {
      *( (volatile Bit32 *)TR0_HI ) = *hdr;
   }

   return( rc );
}


////////////////////////////////////////////////////////////////////////////////
// Use for unaligned payload or bytes = 1-256. This function will not fill out a
// payload, but will fill to quadword.
////////////////////////////////////////////////////////////////////////////////
#undef  FN_NAME
#define FN_NAME "BGLTreeRawInjectBytesUnaligned"
int BGLTreeRawInjectBytesUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload
       int            bytes )   // Payload bytes (1-256)
{
   Bit128 *tr_di, *q1, *q2, *q3;
   int bytes16;
   int i;
   int rc = 0;

#ifdef _SPI_CHECK_INFO
   s0printf( FN_NAME"( %d, 0x%.8x, %d ) called.\n", vc, (int)pyld, bytes );
#endif

   // Round bytes to next multiple of 16.
   bytes16 = bytes & 0xfffffff0;
   if( bytes != bytes16 ) {
      bytes16 += 16;
   }

#ifdef _SPI_CHECK
   {
      _BGL_TreeFifoStatus stat;

      if ( (bytes < 1) || (bytes > _BGL_TREE_PKT_MAX_BYTES) ) {
         TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": bytes=%d out of range 1-256.\n", bytes ));
         _BLADE_ABORT( -1 );
      }
      BGLTreeGetStatus( vc, &stat );
      if ( stat.InjPyldCount > (unsigned)(128 - (bytes16 >> 4)) ) {
         TrSPI_TRC( TrSPI_TRC_SEND,( FN_NAME": Not enough room in VC%d payload fifo.\n", vc ));
         _BLADE_ABORT( -1 );
      }
   }
#endif

   if( vc ) {

      // Prepare to inject payload.
      memcpy( &(_tr1_data_inj_bounce.flat[0]), pyld, bytes );

      tr_di = (Bit128 *)TR1_DI;
      q1 = (Bit128 *)&_tr1_data_inj_bounce;

   } else {

      // Prepare to inject payload.
      memcpy( &(_tr0_data_inj_bounce.flat[0]), pyld, bytes );

      tr_di = (Bit128 *)TR0_DI;
      q1 = (Bit128 *)&_tr0_data_inj_bounce;

   }

   // Inject payload.
   q2 = q1+1;
   q3 = q1+2;

   // bytes16 will be number of quadwords.
   bytes16 >>= 4;

   for ( i = 0 ; i < (bytes16/3) ; i++ ) {
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadLoad  (q3, 5);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
      QuadStore (tr_di, 5);
#else
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;
      double _Complex q3v = __lfpd((volatile double *)q3) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      __stfpd((volatile double *)tr_di, q3v) ;
#endif
      q1 += 3;
      q2 += 3;
      q3 += 3;
   }

   // Zero, one or two left to do.
   switch( bytes16-(3*i) ) {
   case 1:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadStore (tr_di, 3);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      __stfpd((volatile double *)tr_di, q1v) ;
      }
#endif
      break;
   case 2:
#if defined(__GNUC__)
      QuadLoad  (q1, 3);
      QuadLoad  (q2, 4);
      QuadStore (tr_di, 3);
      QuadStore (tr_di, 4);
#else
      {
      double _Complex q1v = __lfpd((volatile double *)q1) ;
      double _Complex q2v = __lfpd((volatile double *)q2) ;      
      __stfpd((volatile double *)tr_di, q1v) ;
      __stfpd((volatile double *)tr_di, q2v) ;
      }
#endif
   }

   return( rc );
}

