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
//      File: blade/spi/spi_ts_send.c
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
#include <blade_on_blrts.h>

__BEGIN_DECLS

#if !defined(__GNUC__)
extern "builtin" double _Complex __lfpd(volatile double *) ;
extern "builtin" void __stfpd(volatile double *, double _Complex) ;
extern "builtin" void __sync(void) ;
#endif

#include <string.h> // for memcpy

/*---------------------------------------------------------------------------*
 *     Torus Packet Layer Send Routines                                      *
 *---------------------------------------------------------------------------*/

//
// Note: These "do the right thing" for:
//         Balanced Mode  - uses both Fifo Groups,
//         Split Mode     - uses only calling Core's Fifo Group.
//         Symmetric Mode - Caller on Core 0 uses only Fifo Group 0, and
//                          caller on Core 1 uses only Fifo Group 1.
//

// Send a Torus Direct Header Packet. Must be ALIGNED!
//  Note: For on-the-fly header creation, call one of the above header creation
//   routines as a parameter to this function.
void BGLTorusSendPacket(
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data (16byte aligned)
       int bytes )              // Payload bytes = ((chunks * 32) - 16)
{
   int chunks = ((bytes + 15) >> 5);
   _BGL_TorusFifo *fifo = (_BGL_TorusFifo *)0;
   uint8_t *pdata = (uint8_t *)data;           // silence overly-pedantic GCC 3.4 lvalue cast warning

   // update chunks in the header
   hdr->hwh0.Chunks = chunks;

   // block until there's room in a suitable injection fifo
   while ( (fifo = _ts_ChooseInjectionFifo( hdr )) == (_BGL_TorusFifo *)0 )
      ;

#if defined(__GNUC__)
   QuadLoad( hdr, 0 );
#else
#endif

   // compute total quads to store
   int quads = ((chunks << 1) + 1);

   register int u asm("r6") = 16;
   int n4 = (quads >> 2);

   // store header
#if defined(__GNUC__)
   QuadStore( fifo, 0 );
#else
   __stfpd((volatile double *)fifo, *(double _Complex *) hdr) ;

#endif
   while( n4-- )
      {
      asm volatile( "lfpdx   1,0,%0;"
                    "lfpdux  2,%0,%1;"
                    "lfpdux  3,%0,%1;"
                    "lfpdux  4,%0,%1;"
                    "stfpdx  1,0,%2;"
                    "stfpdx  2,0,%2;"
                    "stfpdx  3,0,%2;"
                    "stfpdx  4,0,%2;"
                    : "+r" (pdata)
                    : "r"  (u),
                      "r"  (fifo),
                      "0"  (pdata)
                    : "memory",
                      "fr1", "fr2", "fr3", "fr4" );

      pdata += 16;
      }

   if ( quads & 2 )
      {
      asm volatile( "lfpdx   1,0,%0;"
                    "lfpdux  2,%0,%1;"
                    "stfpdx  1,0,%2;"
                    "stfpdx  2,0,%2;"
                    : "+r" (pdata)
                    : "r"  (u),
                      "r"  (fifo),
                      "0"  (pdata)
                    : "memory",
                      "fr1", "fr2" );
      pdata += 16;
      }

   if ( quads & 1 )
      {
      QuadMove( pdata, fifo, 1 );
      }
}

// Inject a Torus Direct Header Packet to the Specified FIFO. Must be ALIGNED!
//  Note: For on-the-fly header creation, call one of the above header creation
//   routines as a parameter to this function.
void BGLTorusInjectPacket(
       _BGL_TorusFifo   *fifo,  // Injection FIFO to use (Must have space available!)
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data (16byte aligned)
       int bytes )              // Payload bytes = ((chunks * 32) - 16)
{
   int chunks = ((bytes + 15) >> 5);
   uint8_t *pdata = (uint8_t *)data;   // silence overly-pedantic GCC 3.4 lvalue cast warning

   // update chunks in the header
   hdr->hwh0.Chunks = chunks;

   // compute total quads to store in addition to the header
   int quads = ((chunks << 1) + 1);

   // load header
#if defined(__GNUC__)
   QuadLoad( hdr, 0 );
#endif

   register int u asm("r7") = 16;
   int n4 = (quads >> 2);

   // store header
#if defined(__GNUC__)
   QuadStore( fifo, 0 );
#else
   __stfpd((volatile double *)fifo, *(double _Complex *) hdr) ;

#endif
   while( n4-- )
      {
      asm volatile( "lfpdx   1,0,%0;"
                    "lfpdux  2,%0,%1;"
                    "lfpdux  3,%0,%1;"
                    "lfpdux  4,%0,%1;"
                    "stfpdx  1,0,%2;"
                    "stfpdx  2,0,%2;"
                    "stfpdx  3,0,%2;"
                    "stfpdx  4,0,%2;"
                    : "+r" (pdata)
                    : "r"  (u),
                      "r"  (fifo),
                      "0"  (pdata)
                    : "memory",
                      "fr1", "fr2", "fr3", "fr4" );

      pdata += 16;
      }

   if ( quads & 2 )
      {
      asm volatile( "lfpdx   1,0,%0;"
                    "lfpdux  2,%0,%1;"
                    "stfpdx  1,0,%2;"
                    "stfpdx  2,0,%2;"
                    : "+r" (pdata)
                    : "r"  (u),
                      "r"  (fifo),
                      "0"  (pdata)
                    : "memory",
                      "fr1", "fr2" );
      pdata += 16;
      }

   if ( quads & 1 )
      {
      QuadMove( pdata, fifo, 1 );
      }
}

// Send a completely filled out packet from memory
//  Size of the packet is given by header chunks.
void BGLTorusSendPacketImage(
        _BGL_TorusPkt *pkt )      // Address of ready-to-go packet
{
   _BGL_TorusPktHdr *hdr = &(pkt->hdr);
   int chunks            = hdr->hwh0.Chunks;
   Bit8 *data            = &(pkt->payload[0]);
   _BGL_TorusFifo *fifo  = (_BGL_TorusFifo *)0;
   int i, quads;

   // block until there's room in a suitable injection fifo
   while ( (fifo = _ts_ChooseInjectionFifo( hdr )) == (_BGL_TorusFifo *)0 )
        ;

#if defined(__GNUC__)
   QuadLoad( hdr, 0 );
#endif
   // compute total quads to store
   quads = ((chunks << 1) + 1);

   // store header
#if defined(__GNUC__)
   QuadStore( fifo, 0 );
#else
   __stfpd((volatile double *)fifo, *(double _Complex *) hdr) ;
#endif
   // store payload starting at quad#1
   for ( i = 0 ; i < quads ; i++ )
      {
      QuadMove( (void *)(data + (i << 4)), fifo, 1 );
      }
}

// Inject a completely filled out packet from memory
//  Size of the packet is given by header chunks.
void BGLTorusInjectPacketImage(
        _BGL_TorusFifo *fifo,  // Injection FIFO to use (Must have space available!)
        _BGL_TorusPkt  *pkt )  // Address of ready-to-go packet
{
   int chunks = pkt->hdr.hwh0.Chunks;
   Bit8 *data = &(pkt->payload[0]);
   int i, quads;

#if defined(__GNUC__)

   QuadLoad( pkt, 0 );
#endif
   // compute total quads to store in addition to the header
   quads = ((chunks << 1) + 1);

   // store header
#if defined(__GNUC__)
   QuadStore( fifo, 0 );
#else
   __stfpd((volatile double *)fifo, *(double _Complex *) pkt) ;
#endif
   // store payload starting at quad#1
   for ( i = 0 ; i < quads ; i++ )
      {
      QuadMove( (void *)(data + (i << 4)), fifo, 1 );
      }
}

// Use for unaligned data or bytes < full chunks
void BGLTorusSendPacketUnaligned(
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data
       int bytes )              // Payload bytes <= ((chunks * 32) - 16)
{
   int chunks = ((bytes + 15) >> 5);
   _BGL_TorusFifo *fifo = (_BGL_TorusFifo *)0;
   _BGL_TorusPkt *buf   = (_BGL_TorusPkt *)0;
   int i, quads;

   // update chunks in the header
   hdr->hwh0.Chunks = chunks;

   // block until there's room in a suitable injection fifo
   while ( (fifo = _ts_ChooseInjectionFifo( hdr )) == (_BGL_TorusFifo *)0 )
        ;

   buf = _ts_Fifo2Buffer( (Bit32)fifo );

   // set the payload into the buffer
   memcpy( &(buf->payload[0]), data, bytes );
   data = &(buf->payload[0]);

#if defined(__GNUC__)
   QuadLoad( hdr, 0 );
#endif

   // compute total quads to store
   quads = ((chunks << 1) + 1);

   // store header
#if defined(__GNUC__)   
   QuadStore( fifo, 0 );
#else
    __stfpd((volatile double *)fifo, *(double _Complex *) hdr) ;
   
#endif
   // store payload starting at quad#1
   for ( i = 0 ; i < quads ; i++ )
      {
      QuadMove( (void *)((uint8_t *)data + (i << 4)), fifo, 0 );
      }
}

// Use for unaligned data or bytes < full chunks
void BGLTorusInjectPacketUnaligned(
       _BGL_TorusFifo   *fifo,  // Injection FIFO to use (Must have space available!)
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data
       int bytes )              // Payload bytes <= ((chunks * 32) - 16)
{
   int chunks = ((bytes + 15) >> 5);
   _BGL_TorusPkt *buf   = _ts_Fifo2Buffer( (Bit32)fifo );
   int i, quads;

   // update chunks in the header
   hdr->hwh0.Chunks = chunks;

   // compute total quads to store in addition to the header
   quads = ((chunks << 1) + 1);

   // move the payload into the buffer
   memcpy( &(buf->payload[0]), data, bytes );
   data = &(buf->payload[0]);

#if defined(__GNUC__)
   QuadLoad( hdr, 0 );

   // store header
   QuadStore( fifo, 0 );
#else
   __stfpd((volatile double *)fifo, *(double _Complex *) hdr) ;

#endif
   // store payload starting at quad#1
   for ( i = 0 ; i < quads ; i++ )
      {
      QuadMove( (void *)((uint8_t *)data + (i << 4)), fifo, 1 );
      }
}

// Gather a packet using an iovec descriptor (array of {base, len} pairs).
//  Presumably you will send such a packet to an active function
//  that scatters the data according to some iovec it knows about. or
//  perhaps you passed the address of that iovec as an argument when
//  you created the header.
//  The total size of the iovec must not exceed packet payload size,
//  The packet is "packed" meaning that no alignment or padding is
//  added between iovec data elements.  For performance, please try
//  to maintain reasonable data alignment.
void BGLTorusSendPacketIOVEC(
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       iovec_g *data_vec,         // Data "Gather" information
       int vec_count )          // Count of elements in the i/o vector
{
   int bytes;
   int chunks;
   Bit8 *data;
   _BGL_TorusFifo *fifo = (_BGL_TorusFifo *)0;
   _BGL_TorusPkt  *buf  = (_BGL_TorusPkt *)0;
   int i, quads;

   // in the emulator, do a first pass for extra hand-holding
   for ( bytes = 0, i = 0 ; i < vec_count ; i++ )
      {
      bytes += data_vec->len;
      }

   chunks = ((bytes + 15) >> 5);

   // update chunks in the header
   hdr->hwh0.Chunks = chunks;

   // block until there's room in a suitable injection fifo
   while ( (fifo = _ts_ChooseInjectionFifo( hdr )) == (_BGL_TorusFifo *)0 )
        ;

   buf        = _ts_Fifo2Buffer( (Bit32)fifo );

   // accumulate the payload into the buffer
   for ( data = &(buf->payload[0]), i = 0  ; i < vec_count ; i++ )
      {
      //@MG: needs optimization
      memcpy( data, data_vec[i].base, data_vec[i].len );
      data += data_vec[i].len;
      }

   data = &(buf->payload[0]);

#if defined(__GNUC__)
   QuadLoad( hdr, 0 );
#endif
   // compute total quads to store
   quads = ((chunks << 1) + 1);

   // store header
#if defined(__GNUC__)
   QuadStore( fifo, 0 );
#else
   __stfpd((volatile double *)fifo, *(double _Complex *) hdr) ;

#endif
   // store payload starting at quad#1
   for ( i = 0 ; i < quads ; i++ )
      {
      QuadMove( (void *)(data + (i << 4)), fifo, 1 );
      }
}


// Send a Torus Indirect Header Packet
void BGLTorusSendPacketIndirect(
       _BGL_TorusPktHdr *hdr,   // Previously created primary header to use
       void *indirect,          // 16byte aligned secondary s/w header
       void *data,              // Source address of data (16byte aligned)
       int bytes )              // Payload bytes = ((chunks * 32) - 32)
{
   int chunks = ((bytes + 31) >> 5);
   _BGL_TorusFifo *fifo = (_BGL_TorusFifo *)0;
   int i, quads;

   // update chunks in the header
   hdr->hwh0.Chunks = chunks;

   // block until there's room in a suitable injection fifo
   while ( (fifo = _ts_ChooseInjectionFifo( hdr )) == (_BGL_TorusFifo *)0 )
        ;

   // compute payload quads to store (entire 1st chunk is headers)
   quads = (chunks << 1);

   // store both headers
#if defined(__GNUC__)
   
   QuadLoad( hdr,      0 );
   QuadLoad( indirect, 1 );
   QuadStore( fifo, 0 );
   QuadStore( fifo, 1 );
#else
   __stfpd((volatile double *)fifo, *(double _Complex *) hdr) ;
   __stfpd((volatile double *)fifo, *(double _Complex *) indirect) ;

#endif
   // store payload starting at quad#2
   for ( i = 0 ; i < quads ; i++ )
      {
      QuadMove( (void *)((uint8_t *)data + (i << 4)), fifo, 2 );
      }
}

__END_DECLS
