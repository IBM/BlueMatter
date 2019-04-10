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
 
#ifndef SPI_TORUS_H // prevent multiple inclusion
#define SPI_TORUS_H

#include <blhpk.h>

// Torus Packet Format: Bit encodings for HW Word 0
// Apps should consider this stuff to be 'opaque'.
#define TORUS_H0_TYPE_IDLE0      _B8( 7,0x69) // "0110 1001"
#define TORUS_H0_TYPE_IDLE1      _B8( 7,0xB8) // "1011 1000"
#define TORUS_H0_TYPE_DATA       _B8( 7,0x2E) // "0010 1110"  we only send these
#define TORUS_H0_TYPE_TOKEN_ACK  _B8( 7,0x47) // "0100 0111"
#define TORUS_H0_TYPE_ACK_ONLY   _B8( 7,0xD1) // "1101 0001"

#define TORUS_H0_HINT_X_PLUS     _B6(13,0x20)
#define TORUS_H0_HINT_X_MINUS    _B6(13,0x10)
#define TORUS_H0_HINT_Y_PLUS     _B6(13,0x08)
#define TORUS_H0_HINT_Y_MINUS    _B6(13,0x04)
#define TORUS_H0_HINT_Z_PLUS     _B6(13,0x02)
#define TORUS_H0_HINT_Z_MINUS    _B6(13,0x01)

#define TORUS_H0_DEPOSIT         _BN(14)
#define TORUS_H0_FIFO_ID         _BN(15)

#define TORUS_H0_CHUNKS(x)       _B3(18,(x))
#define TORUS_H0_AVAIL(x)        _B2(21,(x))
#define TORUS_H0_DYNAMIC_ROUTE   _BN(22)

#define TORUS_H0_VC_D0           _B2(24,0)
#define TORUS_H0_VC_D1           _B2(24,1)
#define TORUS_H0_VC_BN           _B2(24,2)
#define TORUS_H0_VC_BP           _B2(24,3)

#define TORUS_H0_DST_X(x)        _B8(31,(x))

// Torus Packet Format: Bit encodings for HW Word 1
#define TORUS_H1_DST_Y(y)        _B8( 7,(y))
#define TORUS_H1_DST_Y(z)        _B8(15,(z))
#define TORUS_H1_SEQNO(s)        _B8(23,(s))
#define TORUS_H1_CHKSUM(c)       _B8(31,(c))

// Macros to create the usual header types
#define TORUS_H0_P2P_DATA(chunks,avail,dst_x)  (TORUS_H0_TYPE_DATA | \
                                                TORUS_H0_CHUNKS((chunks)) | \
                                                TORUS_H0_AVAIL((avail)) | \
                                                TORUS_H0_DYNAMIC_ROUTE | \
                                                TORUS_H0_VC_D0 | \
                                                TORUS_H0_DST_X((dst_x)))

#define TORUS_H1_P2P_DATA(dst_y,dst_z) (TORUS_H1_DST_Y((dst_y)) | TORUS_H1_DST_X((dst_x)))

#define TORUS_H0_P2P_PROTO(chunks,avail,dst_x)  (TORUS_H0_TYPE_DATA | \
                                                 TORUS_H0_CHUNKS((chunks)) | \
                                                 TORUS_H0_AVAIL((avail)) | \
                                                 TORUS_H0_VC_D0 | \
                                                 TORUS_H0_DST_X((dst_x)))

#define TORUS_H1_P2P_PROTO(dst_y,dst_z) (TORUS_H1_DST_Y((dst_y)) | TORUS_H1_DST_X((dst_x)))

#ifndef __ASSEMBLY__

// 8 byte hardware header: Word 0
typedef struct T_Torus_HWHdrW0
                {
                unsigned pkt_type  :8;
                unsigned hint      :6;
                unsigned deposit   :1;
                unsigned fifo_id   :1;
                unsigned chunks    :3;
                unsigned avail     :2;
                unsigned dyn_route :1;
                unsigned vc        :2;
                unsigned dst_x     :8;
                }
                PACKED _Torus_HWHdrW0;

// 8 byte hardware header: Word 1
typedef struct T_Torus_HWHdrW1
                {
                unsigned dst_y    :8;
                unsigned dst_z    :8;
                unsigned seqno    :8;
                unsigned chksum   :8
                }
                PACKED _Torus_HWHdrW1;

#endif // __ASSEMBLY__


#define CHUNK_SIZE                (32)     // torus packets are multiples of 32 bytes (1 to 8)
#define QUAD_SIZE                 (16)     // does not belong in this file
#define CHUNK_ALIGN               (L1D_CACHE_LINE_ALIGN)  // chunk size/alignment happens to be same as L1D cache
#define PACKET_MAX_CHUNKS          (8)     // of which 7.5 are availabe for Payload
#define PACKET_MAX_PAYLOAD_BYTES (240)     // bytes available in a payload
#define PACKET_MAX_PAYLOAD_QUADS  (15)     // 1 Quad of 16 taken by (8B H/W Hdr plus 8B S/W Hdr)

#define PACKET_MAX_ATOMS          (20)     // does not belong here (Blake's number)

// Receive Side: Active Function signature (read notes below)
typedef void (*blActor)(unsigned long arg, void *payload);

// Note: I am considering the following: (but the above should give you the idea for now)
//  1. An Active function could be called with the entire payload in quadword registers!
//      - this would not be quite as fast as overlapping fifo-reads with memory-writes.
//  2. A blSPIrun() that asks you for packet buffers before reading the fifo and depositing data there.
//  3. Moving the payload into locked L1 cache lines, then calling you.  Great for unaligned stuff.
//      - this is like 2. but I don't need to call you for buffer(s).
//  4. A flexible combination of the above that use the 2 "avail" h/w hdr bits to decide on the Actor signature.
//  5. Adding a "Put" on the send side.  When combined with deterministic routing (ie, in-order packets)
//      you could 'put' several packets of data, then fire off an actor, knowing that all the data got there.
//      Or, a call that puts data and writes a flag elsewhere. (likely to implement this)
//


// Torus Point-to-Point Packet Level
//  - create a stored packet header, primarily used for "persistent sends"
//  - the hope is that the header would hit in L1 when used, and I may
//     provide a simple way to lock persistent headers into L1.
int blTorusMakePacketHeader( Quadword *hdr,
                             int dest_x, int dest_y, int dest_z,
                             int chunks,
                             blActor fcn, unsigned long arg );

// Change the function and/or argument in a persistent packet header
int blTorusUpdatePacketHeaderFcn( Quadword *hdr,
                                  blActor fcn, unsigned long arg );

// Change the destination in a persistent packet header
// - this is a few cycles more expensive than updating the fcn/arg above (probably must read/modify/write)
int blTorusUpdatePacketHeaderDest( Quadword *hdr,
                                   int dest_x, int dest_y, int dest_z );

// Send a packet worth of unaligned data via a previously created header (~1.4 to 2.2 bytes/cycle depending)
int blTorusSendPacketUnaligned( Quadword *hdr,
                                void *data, int size );

// Non-persistent-header version, builds header on the fly (size <= PACKET_MAX_PAYLOAD_BYTES)
int blTorusMakePacket( int dest_x, int dest_y, int dest_z,
                       blActor fcn, unsigned long arg,
                       void *data, int size );

// Must be quadword aligned and multiple of quadwords (~2.5bytes/cycle)
int blTorusSendPacketAligned( Quadword *hdr,
                              Quadword *data, int num_quads );

// Torus Class Routed: Fill 1 line in X-Dimension when this node *is* a member
int blTorusFill_X( blActor fcn, unsigned long arg,
                   void *data, int size );

// Torus Class Routed: Fill 1 line in X Dimension when this node is NOT a member
//  Payload =- 1 quadword for store-n-forward header needed
int blTorusFill_X_Remote( int x,
                          blActor fcn, unsigned long arg,
                          void *data, int size );

// Torus Class Routed: Fill 1 line in the Y dimension when this node *is* a member
int blTorusFill_Y( blActor fcn, unsigned long arg,
                   void *data, int size );

// Torus Class Routed: Fill line in the Y dimension when this node is NOT a member
//  Payload =- 1 quadword for store-n-forward header needed
int blTorusFill_Y_Remote( int y,
                          blActor fcn, unsigned long arg,
                          void *data, int size );

// Torus Class Routed: Fill line in the Z dimension when this node is a member
int blTorusFill_Z( blActor fcn, unsigned long arg,
                   void *data, int size );

// Torus Class Routed: Fills line in Z dimension when this node is NOT a member
//  Payload =- 1 quadword for store-n-forward header needed
int blTorusFill_Z_Remote( int z,
                          blActor fcn, unsigned long arg,
                          void *data, int size );

// Torus Class Routed: Fill a Sub-Cube: doesn't much matter if this node is a member
//  Note that you are discouraged from using this as a full Torus-based broadcast, use the Tree for that!
//  Payload =- 1 quadword for store-n-forward header needed
//  Basically, this combines class-routing with an SPI defined Actor fcn.
int blTorusFill_XYZ( int x0, int y0, int z0,
                     int x1, int y1, int z1,
                     blActor fcn, unsigned long arg,
                     void *data, int size );

#endif // Add nothing below this line.

