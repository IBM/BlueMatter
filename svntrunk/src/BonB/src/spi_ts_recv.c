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
//      File: blade/spi/spi_ts_recv.c
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
#if defined(PK_XLC)
#define FR0STR "f0"
#define FR1STR "f1"
#define FR2STR "f2"
#define FR3STR "f3"
#define FR4STR "f4"
#define FR5STR "f5"
#define FR6STR "f6"
#define FR7STR "f7"
#define FR8STR "f8"
#else
#define FR0STR "fr0"
#define FR1STR "fr1"
#define FR2STR "fr2"
#define FR3STR "fr3"
#define FR4STR "fr4"
#define FR5STR "fr5"
#define FR6STR "fr6"
#define FR7STR "fr7"
#define FR8STR "fr8"
#endif

__BEGIN_DECLS

#if defined(__GNUC__)
//
// Inline Status Access Routines internal to SPI
//
static inline void _ts_GetStatus0( void *stat )
{
   eieio();
   QuadMove( TS0_S0, stat, 0 );
}

extern inline void _ts_GetStatus1( void *stat )
{
   eieio();
   QuadMove( TS0_S1, stat, 0 );
}

extern inline void _ts_GetStatusBoth( void *s0, void *s1 )
{
   eieio();
   QuadLoad( TS0_S0, 0 );
   QuadLoad( TS0_S1, 1 );
   QuadStore( s0, 0 );
   QuadStore( s1, 1 );
}
#else
// xlC builtins to force parallel loads and stores
extern "builtin" double _Complex __lfpd(volatile double *) ;
extern "builtin" void __stfpd(volatile double *, double _Complex) ;
extern "builtin" void __sync(void) ;
inline static void _ts_GetStatusBoth(_BGL_TorusFifoStatus *stat0, _BGL_TorusFifoStatus *stat1 )
{
	__sync() ; 
	double _Complex status0 = __lfpd((volatile double *)TS0_S0) ;
	double _Complex status1 = __lfpd((volatile double *)TS0_S1) ;
	*(double _Complex *) stat0 = status0 ;
	*(double _Complex *) stat1 = status1 ;
}
inline static void _ts_GetStatus0(_BGL_TorusFifoStatus *stat0 )
{
	__sync() ; 
	double _Complex status0 = __lfpd((volatile double *)TS0_S0) ;
	*(double _Complex *) stat0 = status0 ;
}

inline static void _ts_GetStatus1(_BGL_TorusFifoStatus *stat1 )
{
	__sync() ; 
	double _Complex status1 = __lfpd((volatile double *)TS0_S1) ;
	*(double _Complex *) stat1 = status1 ;
}

#endif // __GNUC__

#if 0
void _ts_MoveFifo2Buffer1( _BGL_TorusFifo *fifo, _BGL_TorusPkt *buf, int quads )
{
   register int u asm("r6") = 16;
   Bit8 *dst = (Bit8 *)buf;

   int n4 = (quads >> 2);
   while( n4-- )
      {
      asm volatile( "lfpdx   1,0,%2;"
                    "lfpdx   2,0,%2;"
                    "lfpdx   3,0,%2;"
                    "lfpdx   4,0,%2;"
                    "stfpdx  1,0,%0;"
                    "stfpdux 2,%0,%1;"
                    "stfpdux 3,%0,%1;"
                    "stfpdux 4,%0,%1;"
                    : "+r" (dst)
                    : "r" (u),
                      "r" (fifo),
                      "0" (dst)
                    : "memory",
                      "fr1", "fr2", "fr3", "fr4" );
      dst += 16;
      }

   if ( quads & 2 )
      {
      asm volatile( "lfpdx   1,0,%2;"
                    "lfpdx   2,0,%2;"
                    "stfpdx  1,0,%0;"
                    "stfpdux 2,%0,%1;"
                    : "+r" (dst)
                    : "r" (u),
                      "r" (fifo),
                      "0" (dst)
                    : "memory",
                      "fr1", "fr2" );
      dst += 16;
      }

   if ( quads & 1 )
      {
      QuadMove( fifo, dst, 1 );
      }
}

//
// Buffered Receive helper routine to move 0-15 quadwords from a fifo to a buffer.
//
//@MG: the switch uses branch via ctr which kills this in all cases.
void _ts_MoveFifo2Buffer( _BGL_TorusFifo *fifo, _BGL_TorusPkt *buf, int quads )
{
   register int u asm("r6") = 16;
   uint8_t *dp = (uint8_t *)buf;

   switch ( quads & 15 )
      {
      case 15:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "lfpdx   9,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       "stfpdux 9,%1,%2;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdux 1,%1,%2;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 14:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "lfpdx   9,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       "stfpdux 9,%1,%2;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "stfpdux 1,%1,%2;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 13:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "lfpdx   9,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       "stfpdux 9,%1,%2;"
                       "lfpdx   4,0,%0;"
                       "stfpdux 1,%1,%2;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "stfpdux 4,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 12:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "lfpdx   9,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       "stfpdux 9,%1,%2;"
                       "stfpdux 1,%1,%2;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 11:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "lfpdx   9,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       "stfpdux 9,%1,%2;"
                       "stfpdux 1,%1,%2;"
                       "stfpdux 2,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 10:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "lfpdx   9,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "lfpdx   1,0,%0;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       "stfpdux 9,%1,%2;"
                       "stfpdux 1,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 9:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "lfpdx   9,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       "stfpdux 9,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 8:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "lfpdx   8,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "stfpdux 7,%1,%2;"
                       "stfpdux 8,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 7:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   7,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       "stfpdux 7,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 6:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "lfpdx   6,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       "stfpdux 6,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 5:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   4,0,%0;"
                       "lfpdx   5,0,%0;"
                       "stfpdux 4,%1,%2;"
                       "stfpdux 5,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 4:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       "lfpdx   4,0,%0;"
                       "stfpdux 4,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 3:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "lfpdx   3,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       "stfpdux 3,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 2:
         asm volatile( "lfpdx   1,0,%0;"
                       "lfpdx   2,0,%0;"
                       "stfpdx  1,0,%1;"
                       "stfpdux 2,%1,%2;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 1:
         asm volatile( "lfpdx   1,0,%0;"
                       "stfpdx  1,0,%1;"
                       :
                       : "r" (fifo),
                         "r" (dp),
                         "r" (u)
                       : "memory",
                         "fr1", "fr2", "fr3",
                         "fr4", "fr5", "fr6",
                         "fr7", "fr8", "fr9"
                         );
         break;
      case 0:
      default:
         break;
      }
}
#endif

/*---------------------------------------------------------------------------*
 *     Torus Packet Layer Receive Initiators                                 *
 *                                                                           *
 *            See below for special Symmetric Mode support.                  *
 *---------------------------------------------------------------------------*/

// polling helper routine (assembler on the real h/w)
static inline int _ts_poll( int *count, Bit8 *stat, _BGL_TorusFifo *fifo, _BGL_TorusPkt *buf )
{
   BGLTorusActivePacketHandler         Actor;
   BGLTorusBufferedActivePacketHandler Actor_Buffered;
   BGLTorusIndirectActivePacketHandler Actor_Indirect;
   _BGL_TorusPktHdr *hdr;
   int rc, chunks, quads;
   Bit32 fcn, arg, extra, app_cs;
   Bit8 *dst;

   // anything in the fifo?
   if ( ! *stat )
      return( 0 );
#if defined(__GNUC__)
   QuadLoad( fifo, 0 );
#else
   *(double _Complex *)buf = __lfpd((volatile double *)fifo) ;
#endif
   // cover some stall cycles
   if ( count ) (*count)--;
   app_cs = _Blade_Config_Area.AppCodeSeg;
   rc = 0;

#if defined(__GNUC__)
   QuadStore( buf, 0 );
#else
#endif
   hdr = &(buf->hdr);

   chunks = (hdr->hwh0.Chunks + 1);
   fcn    = ((hdr->swh0.fcn << 2) | app_cs);
   extra  = hdr->swh0.extra;
   arg    = hdr->swh1.arg;

   quads  = ((chunks << 1) - 1);

   switch( hdr->hwh0.SW_Avail )
      {
      case _BGL_TORUS_ACTOR_UNBUFFERED:
         Actor = (BGLTorusActivePacketHandler)fcn;
         // leave the rest of the pkt in the fifo
         rc = (*Actor)( fifo, arg, extra );
         break;

      default:
      case _BGL_TORUS_ACTOR_BUFFERED:
         // pull the entire payload into the buffer
         {
#if defined(__GNUC__)
         register int u asm("r7") = 16;
#else
         int u=16 ;
#endif
         int n4 = (quads >> 2);
         dst = &(buf->payload[0]);
         while( n4-- )
            {
            asm volatile( "lfpdx   1,0,%2;"
                          "lfpdx   2,0,%2;"
                          "lfpdx   3,0,%2;"
                          "lfpdx   4,0,%2;"
                          "stfpdx  1,0,%0;"
                          "stfpdux 2,%0,%1;"
                          "stfpdux 3,%0,%1;"
                          "stfpdux 4,%0,%1;"
                          : "=b" (dst)
                          : "b" (u),
                            "r" (fifo),
                            "0" (dst)
                          : "memory",
                            FR1STR, FR2STR, FR3STR, FR4STR 
                            );
            dst += 16;
            }
         if ( quads & 2 )
            {
            asm volatile( "lfpdx   1,0,%2;"
                          "lfpdx   2,0,%2;"
                          "stfpdx  1,0,%0;"
                          "stfpdux 2,%0,%1;"
                          : "=b" (dst)
                          : "b" (u),
                            "r" (fifo),
                            "0" (dst)
                          : "memory",
                          FR1STR, FR2STR
                            );
            dst += 16;
            }
         if ( quads & 1 )
            {
            QuadMove( fifo, dst, 1 );
            }
         }
         Actor_Buffered = (BGLTorusBufferedActivePacketHandler)fcn;
         rc = (*Actor_Buffered)( &(buf->payload), arg, extra );
         break;

      case _BGL_TORUS_ACTOR_INDIRECT:
         // pull the indirect header into the buffer
#if defined(__GNUC__)
         
         QuadLoad( fifo, 1 );
#else
	     double _Complex indirectHeader = __lfpd((volatile double *)fifo) ;
#endif
         dst = &(buf->payload[ sizeof(Bit128) ]);
         Actor_Indirect = (BGLTorusIndirectActivePacketHandler)fcn;
#if defined(__GNUC__)
         QuadStore( dst, 1 );
#else 
         *(double _Complex *) dst = indirectHeader ;
#endif        
         rc = (*Actor_Indirect)( fifo, arg, extra, dst );
         break;
      }

   // update the cached status and return the Actor's rc
   *stat -= chunks;

   return( rc );
}


//
// Wait for: count packets, or
//           until an Actor returns non-zero.
//   On return, count will be updated with number of packets received.
//   The return code from this function will be the return code from last actor invoked.
//
// Note: These "do the right thing" for:
//         Balanced Mode  - uses both Fifo Groups,
//         Split Mode     - uses only calling Core's Fifo Group.
//         Symmetric Mode - Caller on Core 0 uses only Fifo Group 0, and
//                          caller on Core 1 uses only Fifo Group 1.
//
// Note: The Actor function is dispatched in the thread that called these routines.
//
int BGLTorusWaitCount_original( int *count )
{
   int rc = 0;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      // Refresh Group 0 status cache, and check each fifo for packets
      _ts_GetStatus0( &_ts0_stat0recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.rH, (_BGL_TorusFifo *)TS0_RH, &_ts0_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;

      // Refresh Group 1 status cache, and check each fifo for packets
      _ts_GetStatus1( &_ts0_stat1recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.rH, (_BGL_TorusFifo *)TS1_RH, &_ts1_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}

int BGLTorusWaitCountF0_original( int *count )
{
   int rc = 0;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      // Refresh Group 0 status cache, and check each fifo for packets
      _ts_GetStatus0( &_ts0_stat0recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.rH, (_BGL_TorusFifo *)TS0_RH, &_ts0_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}

int BGLTorusWaitCountF1_original( int *count )
{
   int rc = 0;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      // Refresh Group 1 status cache, and check each fifo for packets
      _ts_GetStatus1( &_ts0_stat1recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.rH, (_BGL_TorusFifo *)TS1_RH, &_ts1_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}

// Wait forever until an Actor returns non-zero
int BGLTorusWait( void  )
{
   int rc  = 0;
   int cnt;

   for ( ; ; )
      {
      cnt = 14;

      // Refresh Group 0 status cache, and check each fifo for packets
      _ts_GetStatus0( &_ts0_stat0recv_bounce );
      if ( (rc = _ts_poll( &cnt, &_ts0_stat0recv_bounce.rH, (_BGL_TorusFifo *)TS0_RH, &_ts0_rH_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) ) break;

      // Refresh Group 1 status cache, and check each fifo for packets
      _ts_GetStatus1( &_ts0_stat1recv_bounce );
      if ( (rc = _ts_poll( &cnt, &_ts0_stat1recv_bounce.rH, (_BGL_TorusFifo *)TS1_RH, &_ts1_rH_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) ) break;
      if ( (rc = _ts_poll( &cnt, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) ) break;
      }

   // return first non-zero rc from an Actor
   return( rc );
}

// Wait forever until an Actor returns non-zero
int BGLTorusWaitF0( void  )
{
   int rc = 0;

   for ( ; ; )
      {
      // Refresh Group 0 status cache, and check each fifo for packets
      _ts_GetStatus0( &_ts0_stat0recv_bounce );
      if ( (rc = _ts_poll( 0, &_ts0_stat0recv_bounce.rH, (_BGL_TorusFifo *)TS0_RH, &_ts0_rH_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) ) break;
      }

   // return first non-zero rc from an Actor
   return( rc );
}

// Wait forever until an Actor returns non-zero
int BGLTorusWaitF1( void  )
{
   int rc = 0;

   for ( ; ; )
      {
      // Refresh Group 1 status cache, and check each fifo for packets
      _ts_GetStatus1( &_ts0_stat1recv_bounce );
      if ( (rc = _ts_poll( 0, &_ts0_stat1recv_bounce.rH, (_BGL_TorusFifo *)TS1_RH, &_ts1_rH_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) ) break;
      if ( (rc = _ts_poll( 0, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) ) break;
      }

   // return first non-zero rc from an Actor
   return( rc );
}


//
// Poll for Any Available Packets
//
//    Returns when the Reception Fifos are empty, or
//            when an Actor returns non-zero, or
//            when count reaches zero.
//
//   On return, count will be debited by the number of packets received.
//   The return code from this function will be the return code from last actor invoked.
//
int BGLTorusPoll_original( int *count )   // Poll both Fifo Groups
{
   int rc = 0;
   int last_count;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      last_count = *count;  // when last_count stops changing, we return

      // Refresh Group 0 status cache, and check each fifo for packets
      _ts_GetStatus0( &_ts0_stat0recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.rH, (_BGL_TorusFifo *)TS0_RH, &_ts0_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;

      // Refresh Group 1 status cache, and check each fifo for packets
      _ts_GetStatus1( &_ts0_stat1recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.rH, (_BGL_TorusFifo *)TS1_RH, &_ts1_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;

      if ( *count == last_count ) // nothing was received
         break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}

int BGLTorusPollF0_original( int *count )   // Poll only Fifo Group 0
{
   int rc = 0;
   int last_count;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      last_count = *count;  // when last_count stops changing, we return

      _ts_GetStatus0( &_ts0_stat0recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.rH, (_BGL_TorusFifo *)TS0_RH, &_ts0_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;

      if ( *count == last_count ) // nothing was received
         break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}

int BGLTorusPollF1_original( int *count )   // Poll only Fifo Group 1
{
   int rc = 0;
   int last_count;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      last_count = *count;  // when last_count stops changing, we return

      _ts_GetStatus1( &_ts0_stat1recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.rH, (_BGL_TorusFifo *)TS1_RH, &_ts1_rH_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;

      if ( *count == last_count ) // nothing was received
         break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}


// a modified version of the blade BGLTorusPollF0
// to correct a fairness problem in BGLTorusPollF0

int fifo_id0 ALIGN_L1_CACHE =0;


int BGLTorusPollF0( int *count )   // Poll only Fifo Group 0
{
   int rc = 0;
   int last_count;
   int this_time;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      last_count = *count;  // when last_count stops changing, we return

      _ts_GetStatus0( &_ts0_stat0recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.rH, (_BGL_TorusFifo *)TS0_RH, &_ts0_rH_bounce)) || ! *count ) break;

      this_time = fifo_id0;
      fifo_id0 ++;
      if (fifo_id0 >= 6) fifo_id0 = 0;

      switch(this_time) {

      case 0:  // plus direction
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;
        break;

      case 1:  // minus direction
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
        break;

      case 2: // plus direction
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
        break;

      case 3: // minus direction
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
        break;

      case 4: // plus direction
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
        break;

      default: // minus direction
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[5], (_BGL_TorusFifo *)TS0_R5, &_ts0_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[4], (_BGL_TorusFifo *)TS0_R4, &_ts0_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[3], (_BGL_TorusFifo *)TS0_R3, &_ts0_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[2], (_BGL_TorusFifo *)TS0_R2, &_ts0_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[1], (_BGL_TorusFifo *)TS0_R1, &_ts0_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat0recv_bounce.r[0], (_BGL_TorusFifo *)TS0_R0, &_ts0_r0_bounce)) || ! *count ) break;
        break;
      }

      if ( *count == last_count ) // nothing was received
         break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}

int fifo_id1 ALIGN_L1_CACHE =0;



int BGLTorusPollF1( int *count )   // Poll only Fifo Group 1
{
   int rc = 0;
   int last_count;
   int this_time;

   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      last_count = *count;  // when last_count stops changing, we return

      _ts_GetStatus1( &_ts0_stat1recv_bounce );
      if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.rH, (_BGL_TorusFifo *)TS1_RH, &_ts1_rH_bounce)) || ! *count ) break;

      this_time = fifo_id1;
      fifo_id1 ++;
      if (fifo_id1 >= 6) fifo_id1 = 0;

      switch(this_time) {

      case 0:  // plus direction
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
        break;

      case 1:  // minus direction
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
        break;

      case 2: // plus direction
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
        break;

      case 3: // minus direction
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
        break;

      case 4: // plus direction
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
        break;

      default: // minus direction
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[5], (_BGL_TorusFifo *)TS1_R5, &_ts1_r5_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[4], (_BGL_TorusFifo *)TS1_R4, &_ts1_r4_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[3], (_BGL_TorusFifo *)TS1_R3, &_ts1_r3_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[2], (_BGL_TorusFifo *)TS1_R2, &_ts1_r2_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[1], (_BGL_TorusFifo *)TS1_R1, &_ts1_r1_bounce)) || ! *count ) break;
        if ( (rc = _ts_poll( count, &_ts0_stat1recv_bounce.r[0], (_BGL_TorusFifo *)TS1_R0, &_ts1_r0_bounce)) || ! *count ) break;
        break;
      }

      if ( *count == last_count ) // nothing was received
         break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}


// modified version to avoid unfairness
// for simplicity call BGLTorusPollF0 and BGLTorusPollF1,
// rather than writing a 12 case switch,  if count is very big, then this
// induces some unfairness.  This could be avoided by doing the polls
// on smaller counts, until the total count is reached, or the routine
// other wise exits.

int grp_id ALIGN_L1_CACHE =0;

int BGLTorusPoll( int *count )   // Poll both Fifo Groups
{
   int rc = 0;
   int last_count;


   if ( !count )
      return( -1 );

   for ( ; *count ; )
      {
      last_count = *count;  // when last_count stops changing, we return

      if (grp_id == 0) {
    // Poll Group 0 first
        rc =  BGLTorusPollF0(count);
        if ( rc || ! *count ) break;
        rc =  BGLTorusPollF1(count);
        if ( rc || ! *count ) break;
        grp_id =1;
      }

      else {
    // Poll Group 1 first
        rc =  BGLTorusPollF1(count);
        if ( rc || ! *count ) break;
        rc =  BGLTorusPollF0(count);
        if ( rc || ! *count ) break;
        grp_id =0;
      }


      if ( *count == last_count ) // nothing was received
         break;
      }

   // return with debited count and rc from last Actor
   return( rc );
}


int BGLTorusWaitCountF0( int *count )
{
   int rc = 0;

   if ( !count )
      return( -1 );

   while(1) {
      rc =  BGLTorusPollF0(count);
      if ( (rc!= 0)  || (*count == 0))  {  return( rc );}
   }

   return (1);  // should never get here
}





int BGLTorusWaitCountF1( int *count )
{
   int rc = 0;

   if ( !count )
      return( -1 );

   while(1) {
      rc =  BGLTorusPollF1(count);
      if ( (rc!= 0)  || (*count == 0))  {  return( rc );}
   }

   return (1);  // should never get here
}




int BGLTorusWaitCount( int *count )
{
   int rc = 0;

   if ( !count )
      return( -1 );

   while(1) {
      rc =  BGLTorusPoll(count);
      if ( (rc!= 0)  || (*count == 0))  {  return( rc );}
   }

   return (1);  // should never get here
}


__END_DECLS
