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
//      File: blade/spi/BGL_TorusPktSPI.h
//
//   Purpose: Define Blade's Torus Packet Layer System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: The packet layer is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the Torus.
//
//   History: 05/15/2002: MEG - Created.
//
//
#ifndef _BGL_TORUS_PKT_SPI_H  // Prevent multiple inclusion
#define _BGL_TORUS_PKT_SPI_H

#if !defined(__ASSEMBLY__)

__BEGIN_DECLS

/*---------------------------------------------------------------------------*
 *    Torus Memory Map                                                       *
 *---------------------------------------------------------------------------*/
#define PA_TORUS_0      (0xB1140000)         // Torus Group 0 Base Physical Address
#define PA_TORUS_1      (0xB1150000)         // Torus Group 1 Base Physical Address

// Offsets within Torus Group for Reception Fifos
//   Read-Only, QuadWord Access Only.
#define TSx_R0       (0x0000)  // Offset for NormPri Reception Fifo 0
#define TSx_R1       (0x0400)  // Offset for NormPri Reception Fifo 1
#define TSx_R2       (0x0800)  // Offset for NormPri Reception Fifo 2
#define TSx_R3       (0x0C00)  // Offset for NormPri Reception Fifo 3
#define TSx_R4       (0x1000)  // Offset for NormPri Reception Fifo 4
#define TSx_R5       (0x1400)  // Offset for NormPri Reception Fifo 5
#define TSx_RH       (0x2000)  // Offset for HighPri Reception Fifo H

// Offsets within Torus Group for Injection Fifos
//   Write-Only, QuadWord Access Only.
#define TSx_I0       (0x3000)  // Offset for NormPri Injection Fifo 0
#define TSx_I1       (0x4000)  // Offset for NormPri Injection Fifo 1
#define TSx_I2       (0x5000)  // Offset for NormPri Injection Fifo 2
#define TSx_IH       (0x6000)  // Offset for HighPri Injection Fifo H

// Offsets within Torus Group for Status
//   Read-Only, Quad-Word
#define TSx_Sx       (0x7000)  // Offset for Status of this Group
#define TSx_SO       (0x7010)  // Offset for Status of the Other Group

// Physical Addresses of Torus Group 0 (P=V to use these)
//  Note: in "split mode", both halves should use these addresses
//   and the VMM should map the cores' group at the same VA, different PA.
#define TS0_R0    (PA_TORUS_0 | TSx_R0)
#define TS0_R1    (PA_TORUS_0 | TSx_R1)
#define TS0_R2    (PA_TORUS_0 | TSx_R2)
#define TS0_R3    (PA_TORUS_0 | TSx_R3)
#define TS0_R4    (PA_TORUS_0 | TSx_R4)
#define TS0_R5    (PA_TORUS_0 | TSx_R5)
#define TS0_RH    (PA_TORUS_0 | TSx_RH)
#define TS0_I0    (PA_TORUS_0 | TSx_I0)
#define TS0_I1    (PA_TORUS_0 | TSx_I1)
#define TS0_I2    (PA_TORUS_0 | TSx_I2)
#define TS0_IH    (PA_TORUS_0 | TSx_IH)
#define TS0_S0    (PA_TORUS_0 | TSx_Sx)
#define TS0_S1    (PA_TORUS_0 | TSx_SO)

// Physical Addresses of Torus Group 1 (P=V to use these)
#define TS1_R0    (PA_TORUS_1 | TSx_R0)
#define TS1_R1    (PA_TORUS_1 | TSx_R1)
#define TS1_R2    (PA_TORUS_1 | TSx_R2)
#define TS1_R3    (PA_TORUS_1 | TSx_R3)
#define TS1_R4    (PA_TORUS_1 | TSx_R4)
#define TS1_R5    (PA_TORUS_1 | TSx_R5)
#define TS1_RH    (PA_TORUS_1 | TSx_RH)
#define TS1_I0    (PA_TORUS_1 | TSx_I0)
#define TS1_I1    (PA_TORUS_1 | TSx_I1)
#define TS1_I2    (PA_TORUS_1 | TSx_I2)
#define TS1_IH    (PA_TORUS_1 | TSx_IH)
#define TS1_S1    (PA_TORUS_1 | TSx_Sx)
#define TS1_S0    (PA_TORUS_1 | TSx_SO)


/*---------------------------------------------------------------------------*
 *     Torus Packet Structure                                                *
 *---------------------------------------------------------------------------*/

//
// Torus packets are variable size, consisting of from 1 to 8 "chunks" of 32 bytes.
// On injection and reception, packet sizes must be rounded up to a multiple of
//   chunk size (32 bytes).
//
// Each Torus packet contains a hardware header, a software header, and a payload.
//
// The Torus hardware header, described below, is 8 bytes.
//
// There are 2 formats of Torus software packet headers: Direct and Indirect.
//
//  Direct s/w headers, 8 bytes, are used for the vast majority of Torus packets and
//   contain an Active Packet Function address, a 32bit untyped argument, and an
//   additional 10bit untyped argument.  Both of these arguments are passed to
//   the Active Packet function.  The 10bit argument might be used as, eg, the bytes
//   in the payload if the function doesn't know that apriori.
//
//  Indirect software headers contain an additional 16 bytes of "information", for a
//   total of 24 bytes, and are used in certain system-software supplied Active Packet
//   functions.  As the name implies, Indirect Headers allow for an indirection prior
//   to invoking the destination Active Packet function.  A good example of this is
//   employed by low-level system software to implement complex Torus Class Routing
//   functionality.  Torus Class routing hardware allows for deposit-and-forward
//   routing along any single dimension (X, Y or Z) of the Torus when the sending
//   node is a member of the group of nodes along that dimension.  Such class routed
//   packets can used Direct Headers.  However, when software wishes to target a row
//   of nodes where the sending node is not a member, or when using class routing to
//   fill a plane of nodes or a sub-cube, packets must be removed from the network
//   and reinjected at each corner turn. This store-and-forward is implemented using
//   Indirect software headers, where a special system-supplied corner-turning active
//   function is invoked at intermediate nodes.
//
//  Important for 2 reasons: (1) when using Indirect headers, packet payload is
//   reduced by 16 bytes, and (2) this function is available for library/application
//   clients of the Torus packet layer.  The indirect header extension is void typed
//   in this interface, so you can make up your own extensions.
//

#define _BGL_TORUS_SIZEOF_CHUNK    (32)
#define _BGL_TORUS_PKT_MAX_CHUNKS  ( 8)

// Each FIFO is really 32 chunks deep; but if you read the status of the
// transmission FIFOs, you are likely to get a 'stale' answer. One consequence
// of this is, it may tell you that it has 24 chunks in, when really it has
// 32 chunks; and if you then write it, things will overflow.
// Work round it by pretending there are only 24 chunks in a send FIFO
//#define _BGL_TORUS_FIFO_CHUNKS     (32)  // max chunks in a fifo
#define _BGL_TORUS_FIFO_CHUNKS     (24)  // max chunks in a fifo
#define _BGL_TORUS_RECEPTION_FIFO_CHUNKS (32) 

#define _BGL_TORUS_PKT_MAX_BYTES   (_BGL_TORUS_SIZEOF_CHUNK * _BGL_TORUS_PKT_MAX_CHUNKS)

#define _BGL_TORUS_PKT_MAX_PAYLOAD (_BGL_TORUS_PKT_MAX_BYTES - sizeof(_BGL_TorusPktHdr))

typedef Bit128 _BGL_TorusFifo;

// Torus Packet Header Hint Bits (for use in structure bit-fields)
#define _BGL_TORUS_PKT_HINT_XP  (0x20)
#define _BGL_TORUS_PKT_HINT_XM  (0x10)
#define _BGL_TORUS_PKT_HINT_YP  (0x08)
#define _BGL_TORUS_PKT_HINT_YM  (0x04)
#define _BGL_TORUS_PKT_HINT_ZP  (0x02)
#define _BGL_TORUS_PKT_HINT_ZM  (0x01)

// Torus Packet Header Virtual Circuits
#define _BGL_TORUS_PKT_VC_D0 (0)
#define _BGL_TORUS_PKT_VC_D1 (1)
#define _BGL_TORUS_PKT_VC_BN (2)
#define _BGL_TORUS_PKT_VC_BP (3)

// Torus Packet Header Dynamic Routing
#define _BGL_TORUS_PKT_DETERMINSTIC (0)
#define _BGL_TORUS_PKT_DYNAMIC      (1)

// Torus Packet Header Routing
#define _BGL_TORUS_PKT_POINT2POINT  (0)
#define _BGL_TORUS_PKT_CLASS        (1)

// The Resvd0 field is used to assist in the selection of Injection Fifos
//   in the packet *Send* calls. (Recall that for *Inject*, the caller
//   chooses the fifo.)
// In Balanced and Symmetric Mode, both FIFO Groups are used, but only
//   one group is in Split Mode.
// This field contains a bit-mask of suitable fifos as follows:
#define _BGL_TORUS_FIFO_G0I0  (1 << 0)
#define _BGL_TORUS_FIFO_G0I1  (1 << 1)
#define _BGL_TORUS_FIFO_G0I2  (1 << 2)
#define _BGL_TORUS_FIFO_G0IH  (1 << 3)

#define _BGL_TORUS_FIFO_G1I0  (1 << 4)
#define _BGL_TORUS_FIFO_G1I1  (1 << 5)
#define _BGL_TORUS_FIFO_G1I2  (1 << 6)
#define _BGL_TORUS_FIFO_G1IH  (1 << 7)

// The Torus Hardware Packet Header
//  This structure should be considered "opaque".  It is supplied for informational
//  purposes only.  Functions are provided to construct/fillout these headers.
typedef struct T_BGL_Torus_HdrHW0   // first 32bit word of a Torus Hardware Header
                {
                unsigned CSum_Skip  : 7; // Number of shorts (2B) to skip in CheckSum.
                unsigned Sk         : 1; // 0=use CSum_Skip, 1=Skip entire pkt.

                unsigned Hint       : 6; // Hint Bits
                unsigned Dp         : 1; // Deposit Bit for Class Routed MultiCast
                unsigned Pid        : 1; // Destination Fifo Group

                unsigned Chunks     : 3; // Size in Chunks of 32B (0 for 1 chunk, ... , 7 for 8 chunks)
                unsigned SW_Avail   : 2; // Available for S/W Use (encodes Actor Signature)
                unsigned Dynamic    : 1; // 1=Dynamic Routing, 0=Deterministic Routing.
                unsigned VC         : 2; // Virtual Channel (0=D0,1=D1,2=BN,3=BP)

                unsigned X          : 8; // Destination X Physical Coordinate
                }
                _BGL_Torus_HdrHW0;

typedef struct T_BGL_Torus_HdrHW1   // second 32bit word of a Torus Hardware Header
                {
                unsigned Y          : 8; // Destination Y Physical Coordinate
                unsigned Z          : 8; // Destination Z Physical Coordinate
                unsigned Resvd0     : 8; // Reserved (pkt crc)
                unsigned Resvd1     : 8; // Reserved (pkt crc)
                }
                _BGL_Torus_HdrHW1;

// The Torus Packet Software Header: Active Function and Extra Argument
typedef union T_BGL_Torus_HdrSW0
               {
                unsigned long arg0;
                struct {
                       unsigned extra      : 10; // additional untyped 10bit argument (may become 8bits)
                       unsigned fcn        : 22; // "massaged" Active Function Pointer (not directly usable)
                       };
                }
                _BGL_Torus_HdrSW0;

// The Torus Packet Software Header: 32bit untyped argument
typedef struct T_BGL_Torus_HdrSW1
                {
                unsigned long arg;   // untyped 32bit argument
                }
                _BGL_Torus_HdrSW1;


// Pull together all components of the Torus "Direct" Packet Header
typedef struct T_BGL_TorusPktHdr
                {
                _BGL_Torus_HdrHW0 hwh0;
                _BGL_Torus_HdrHW1 hwh1;
                _BGL_Torus_HdrSW0 swh0;
                _BGL_Torus_HdrSW1 swh1;
                }
                ALIGN_QUADWORD _BGL_TorusPktHdr;

typedef Bit8 _BGL_TorusPktPayload[ _BGL_TORUS_PKT_MAX_BYTES - sizeof(_BGL_TorusPktHdr) ] ALIGN_QUADWORD ;


// _BGL_TorusPkt: describe the layout of a Torus Packet
//   Note: The "Indirect Header" just uses first quadword of the payload.
typedef struct T_BGL_TorusPkt
                {
                _BGL_TorusPktHdr      hdr;
                _BGL_TorusPktPayload  payload;
                }
                ALIGN_QUADWORD _BGL_TorusPkt;


/*---------------------------------------------------------------------------*
 *     Torus Header Hint-Bit Information and Calculators                     *
 *---------------------------------------------------------------------------*/

//
// Query if the Torus is fully connected, or if it is a Mesh
//
//  Returns True  for any Dimension that is a Mesh,
//          False if the Dimension is a Torus.
//
void BGLTorusIsMesh( int *is_mesh_X, int *is_mesh_Y, int *is_mesh_Z );

//
// Get the current Hint-Bit Cut-Off Settings.
//
void BGLTorusGetHintCutoffPlus(  int *cutoff_xp, int *cutoff_yp, int *cutoff_zp );
void BGLTorusGetHintCutoffMinus( int *cutoff_xm, int *cutoff_ym, int *cutoff_zm );

//
// Set the Hint-Bit Cut-Off Settings.
//
//  Note: for advanced users only. The SPI installs suitable defaults.
//
void BGLTorusSetHintCutoffPlus(  int *cutoff_xp, int *cutoff_yp, int *cutoff_zp );
void BGLTorusSetHintCutoffMinus( int *cutoff_xm, int *cutoff_ym, int *cutoff_zm );

//
// BGLTorusCalcHints: Return HintBits for Only Profitable Routes.
//
//  Taken:    Destination node's physical x,y,z coordinates.
//  Returned: Hint bits set for use in a packet header.
//
//  Note: This provides no default hints.  If sending to yourself, setting
//        at least 1 hint bit is required by the hardware.
//
unsigned int BGLTorusCalcHints( int dest_x,
                                int dest_y,
                                int dest_z  );

/*---------------------------------------------------------------------------*
 *     Torus Fifo Status                                                     *
 *---------------------------------------------------------------------------*/
//
// Torus Fifo Status:
//   Each byte contains the number of 32 byte chunks of data in a fifo.
//   Each Group (0 and 1) of fifos has its own status register.
//   For performance, this structure should be aligned on a 16byte boundary,
//    but access functions (below) work regardless of alignment.
typedef struct T_BGL_TorusFifoStatus
                {
                Bit8 r[6],    // Normal Priority Reception Fifos
                     rH,                   // High   Priority Reception Fifo
                     i[3],             // Normal Priority Injection Fifos
                     iH,                   // High   Priority Injection Fifo
                     pad[5];               // Pad to 16 bytes
                }
                ALIGN_QUADWORD _BGL_TorusFifoStatus;

//// Another view of the same thing, this time so we can index through the Fifos
//typedef struct T_BGL_TorusFifoStatusIndexable
//                {
//                	Bit8 r[6],             // Normal Priority Reception Fifos
//                	     rH,               // High   Priority Reception Fifo
//                	     i[3],             // Normal Priority Injection Fifos
//                	     iH,               // High   Priority Injection Fifo
//                	     pad[5]  ;         // Pad to 16 bytes
//                }
//                ALIGN_QUADWORD _BGL_TorusFifoStatusIndexable ;
                
// Obtain status for Torus Fifo Group 0
void BGLTorusGetStatus0( _BGL_TorusFifoStatus *stat );

// Obtain status for Torus Fifo Group 1
void BGLTorusGetStatus1( _BGL_TorusFifoStatus *stat );


/*---------------------------------------------------------------------------*
 *     Torus Active Packet Signatures for Reception                          *
 *---------------------------------------------------------------------------*/
//
// The Torus Active Packet functions are typically dispatched from one of the
// poll routines.  The poll routines will check the function's return code, and
// when non-zero, return to the caller of the poll routine with that return code.
// On zero return code, the poll routine will continue polling for packets until
// its normal return condition(s) are met (eg, empty fifos, number of packets, etc.).

// Non-Buffered Torus Packet Active Function Signature:
//  When this function is called, the head of the fifo is the first quadword
//   of the packet payload.
typedef int (*BGLTorusActivePacketHandler)(
              _BGL_TorusFifo *fifo,   // Address of Torus Reception Fifo containing Payload.
              Bit32  arg,             // 32 bits of primary arg info
              Bit32  extra );         // 10 bits of extra arg info (promoted to 32bits)

// Buffered Torus Packet Active Function Signature:
//  Use when you require random or unaligned access to payload data.  For example, you
//    might cast a "struct" over the packet payload and access that structure arbitrarily.
//    E.g., System software uses this for message layer protocol packets.
//  When your Active Function returns, the buffer is discarded.
typedef int (*BGLTorusBufferedActivePacketHandler)(
              _BGL_TorusPktPayload *payload, // Address of Packet Payload Stored in Memory (assume L1 cache)
              Bit32  arg,                    // 32 bits of primary arg info
              Bit32  extra );                // 10 bits of extra arg info (promoted to 32bits)

// Indirect Torus Packet Active Function Signature:
//  Implies Unbuffered Payload, but Indirect Argument is buffered.
//  When this function is called, the head of the fifo is the first quadword
//   of the packet payload.
//  E.g., System software uses this for corner-turning in Torus Class-Routed plane-filling.
typedef int (*BGLTorusIndirectActivePacketHandler)(
              _BGL_TorusFifo *fifo,   // Address of Torus Reception Fifo containing Payload.
              Bit32  arg,             // 32 bits of primary arg info
              Bit32  extra,           // 10 bits of extra arg info (promoted to 32bits)
              void  *indirect );      // Indirect Argument Buffered Quadword

/*---------------------------------------------------------------------------*
 *     Torus Header Construction Interfaces                                  *
 *---------------------------------------------------------------------------*/
//
// Torus Packet Addressing: { X, Y, Z, F }
//
//  Torus Packets destinations are a 4-tuple, consisting of X, Y and Z physical
//  coordinates, plus a destination Fifo Group.
//
//  Each node has 2 Fifo groups (0 and 1), to which packets can be addressed
//   separately.  This supports 3 modes of operation:
//   1) Balanced: In balanced mode, the two BlueGene/L Cores on each node are
//       logically labeled the Compute Processor and the I/O Processor. In this
//       mode, each core generally uses the injection-side of 1 fifo group, while
//       the I/O Processor handles the reception side of both groups. This allows,
//       e.g., the Compute Processor to initiate Protocol Packets while the I/O
//       Processor handles all reception and bulk data movement.
//   2) Symmetric: Each core owns 1 complete fifo group and packets/messages should
//       be addressed to the appropriate core.  Typically, an applcation thread would
//       run on each of the cores receiving messages specifically addressed to that
//       thread.
//   3) Split: In this mode, each core runs a completely separate process.  Typically,
//       applications "see" this as logically doubling one dimension of the Torus.
//       For example, an {8x8x8} partition would look like {16x8x8}, where the Fifo
//       Groups are used implicitly.  MPI in split-mode should use this policy.
//       Alternatively, depending on application communication patterns, one can
//       simply think of F as a 2-deep (ie, 8x8x8x2) fourth Torus dimension.
//
// Because Torus Headers are expensive to create, the interface provides a choice
//  of creating the headers "on the fly" or pre-creating the headers.  When you
//  use regular and repeated communication patterns (a.k.a persistent messages),
//  pre-creating headers during the setup phase can provide significant performance
//  benefits.
//

#define _BGL_TORUS_ACTOR_UNBUFFERED (0)  // Actor Signature is Unbuffered
#define _BGL_TORUS_ACTOR_BUFFERED   (1)  // Actor Signature is Buffered
#define _BGL_TORUS_ACTOR_INDIRECT   (2)  // Actor Signature is Indirect (implies unbuffered payload)
#define _BGL_TORUS_ACTOR_NONE       (3)  // No Actor! Application Direct Reception.

#if defined(__cplusplus)
// Gnu C++ does not think "void *" means "void *".  GCC 3.3 is supposed to fix this.
#define ACTOR_DECL(var) Bit32 var
#define ACTOR_CAST(fcn) (Bit32)fcn
#else
#define ACTOR_DECL(var) void *var
#define ACTOR_CAST(fcn) fcn
#endif

// Simple H/W + S/W Torus Header Creation using suitable defaults.  NON-ACTOR, Application Receiver!!
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Destination Fifo Group 0,
//                  Dynamic on VCD0,
//                  Hardware applied Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
static _BGL_TorusPktHdr *BGLTorusMakeHdrApp(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     Bit32 arg0,            // first  argument
                     Bit32 arg1,            // second argument
                     int payload_bytes );   // Bytes in payload, must be <= 240.

// Simple H/W + S/W Torus Header Creation using suitable defaults
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Dynamic on VCD0,
//                  Hardware applied Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
_BGL_TorusPktHdr *BGLTorusMakeHdr(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f,            // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature );       // Actor Signature from above (must match Actor)

// High Priority H/W + S/W Torus Header Creation
//  Setup used:     Point-to-Point,
//                  High Priority,
//                  Deterministic on VCBP,
//                  Calculated Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
_BGL_TorusPktHdr *BGLTorusMakeHdrHP(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f,            // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature );       // Actor Signature from above (must match Actor)

// H/W + S/W Torus Header Creation using determinstic X,Y,Z routing.
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Deterministic Routing,
//                  Calculated Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
_BGL_TorusPktHdr *BGLTorusMakeHdrDeterministic(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f,            // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature );       // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single X range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassDirectX(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_x0,            // destination X range start
                     int dest_x1,            // destination X range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single Y range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassDirectY(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_y0,            // destination Y range start
                     int dest_y1,            // destination Y range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single Z range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassDirectZ(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_z0,            // destination Z range start
                     int dest_z1,            // destination Z range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)


// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single X range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassXP(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_x,             // destination X range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single X range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassXM(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_x,             // destination X range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single X range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassYP(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_y,             // destination Y range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single Y range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassYM(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_y,             // destination Y range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single Z range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassZP(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_z,             // destination Z range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)

// Construct a Torus Class Routed Header when simple "Direct" Sends are possible.
//  Use when filling a single Z range when this node is a member of the range.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassZM(
                     _BGL_TorusPktHdr *hdr,  // Filled in on return
                     int dest_z,             // destination Z range end
                     int dest_f,             // destination Fifo Group
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,              // primary argument to actor
                     Bit32 extra,            // secondary 10bit argument to actor
                     int signature );        // Actor Signature from above (must match Actor)


// Construct a Torus Class Routed Header when "Indirect" Sends are required.
//  Use when filling an arbitrary plane or contiguous sub-cube, whether or not this
//   node is a member of that plane or sub-cube.
// Implementation note: the primary packet header will be set to invoke a system
//  library supplied function at intermediate nodes.  At destination nodes, the
//  actor function will be invoked.  In reality, this function is just syntatic
//  sugar for simply calling one of the simple header creation routines, then
//  programming an indirection argument, and calling it from your own primary
//  actor.
_BGL_TorusPktHdr *BGLTorusMakeHdrClassIndirect(
                     _BGL_TorusPktHdr *hdr, // Filled in on return                   (output)
                     void *indirect,        // Indirect Header filled in on return   (output)
                     int dest_x0,           // destination X range coordinates start (rest are inputs)
                     int dest_x1,           // destination X range coordinates end
                     int dest_y0,           // destination Y range coordinates start
                     int dest_y1,           // destination Y range coordinates end
                     int dest_z0,           // destination Z range coordinates start
                     int dest_z1,           // destination Z range coordinates end
                     int dest_f0,           // destination Fifo Group range start (eg 0-0, 0-1, 1-1)
                     int dest_f1,           // destination Fifo Group range end
                     ACTOR_DECL(actor),     // Actor function address
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra,           // secondary 10bit argument to actor
                     int signature );       // Actor Signature from above (must match Actor)


// Update the primary and secondary arguments in a previously created header
_BGL_TorusPktHdr *BGLTorusUpdateHdrArgs(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     Bit32 arg,             // primary argument to actor
                     Bit32 extra );         // secondary 10bit argument to actor


// Update the destination coordinates in a previously created header
_BGL_TorusPktHdr *BGLTorusUpdateHdrDest(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     int dest_f );          // destination Fifo Group

// Update the hint bits in a previously created header
_BGL_TorusPktHdr *BGLTorusUpdateHdrHints(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     int hints );           // hint bits

// Update the size field in a previously created header
// Note: This calculates chunks by adding the size of the packet header,
//   possibly plus the size of an additional "indirect" software header,
//   plus the bytes parameter, then rounds up that total to chunksize,
//   and finally sets the total number of chunks into the header.
_BGL_TorusPktHdr *BGLTorusUpdateHdrSize(
                     _BGL_TorusPktHdr *hdr, // Updated on return
                     int bytes );           // Payload bytes (header size is added to this)


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
//       "*Inject*" means the caller chooses the fifo.
//       "*Send*"   means the SPI    chooses the fifo.
//


// Send a Torus Direct Header Packet. Must be ALIGNED!
//  Note: For on-the-fly header creation, call one of the above header creation
//   routines as a parameter to this function.
void BGLTorusSendPacket(
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data (16byte aligned)
       int bytes );             // Payload bytes = ((chunks * 32) - 16)

// Inject a Torus Direct Header Packet to the specified FIFO. Must be ALIGNED!
//  Note: For on-the-fly header creation, call one of the above header creation
//   routines as a parameter to this function.
void BGLTorusInjectPacket(
       _BGL_TorusFifo   *fifo,  // Injection Fifo to use (must have space available!)
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data (16byte aligned)
       int bytes );             // Payload bytes = ((chunks * 32) - 16)

// Send a completely filled out packet from memory
//  Size of the packet is given by header chunks.
void BGLTorusSendPacketImage(
        _BGL_TorusPkt *pkt );     // Address of ready-to-go packet

// Inject a completely filled out packet from memory
//  Size of the packet is given by header chunks.
void BGLTorusInjectPacketImage(
        _BGL_TorusFifo *fifo,    // Injection Fifo to use (must have space available!)
        _BGL_TorusPkt  *pkt );   // Address of ready-to-go packet

// Use for unaligned data or bytes < full chunks
void BGLTorusSendPacketUnaligned(
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data
       int bytes );             // Payload bytes <= ((chunks * 32) - 16)

// Use for unaligned data or bytes < full chunks
void BGLTorusInjectPacketUnaligned(
       _BGL_TorusFifo *fifo,    // Injection Fifo to use (must have space available!)
       _BGL_TorusPktHdr *hdr,   // Previously created header to use
       void *data,              // Source address of data
       int bytes );             // Payload bytes <= ((chunks * 32) - 16)

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
       int vec_count );         // Count of elements in the i/o vector


// Send a Torus Indirect Header Packet
void BGLTorusSendPacketIndirect(
       _BGL_TorusPktHdr *hdr,   // Previously created primary header to use
       void *indirect,          // 16byte aligned secondary s/w header
       void *data,              // Source address of data (16byte aligned)
       int bytes );             // Payload bytes = ((chunks * 32) - 32)


/*---------------------------------------------------------------------------*
 *     Torus Packet Layer Receive Initiators                                 *
 *                                                                           *
 *  Two main varieties:                                                      *
 *   1) "Wait" - blocking receives until count packets have been received,   *
 *               or, an Actor returns non-zero.                              *
 *                                                                           *
 *   2) "Poll" - non-blocking receives until the Fifos are empty,            *
 *               or, an Actor returns non-zero,                              *
 *               or, the count parameter reaches zero.                       *
 *                                                                           *
 *---------------------------------------------------------------------------*/

//
// Wait for: count packets, or until an Actor returns non-zero.
//
//   On return, count will be debited by the number of packets received.
//   The return code from this function will be the return code from last actor invoked.
//
// Note: These "do the right thing" for:
//         Balanced Mode  - uses both Fifo Groups,
//         Split Mode     - uses only calling Core's Fifo Group.
//         Symmetric Mode - Caller on Core 0 uses only Fifo Group 0, and
//                          caller on Core 1 uses only Fifo Group 1.
//
// Note: The Actor function is dispatched in the thread that called these routines.
//       Count cannot be NULL.
//
int BGLTorusWaitCount(   int *count );  // wait on both fifo groups
int BGLTorusWaitCountF0( int *count );  // wait on just fifo group 0
int BGLTorusWaitCountF1( int *count );  // wait on just fifo group 1

//
// Wait forever, or until an Actor returns non-zero.
//
//   In other words, the Actor, via its return code, tells this function when to stop waiting.
//
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
int BGLTorusWait(   void  );  // wait on both fifo groups
int BGLTorusWaitF0( void  );  // wait on just fifo group 0
int BGLTorusWaitF1( void  );  // wait on just fifo group 1

//
// Non-Blocking Receive Any Available Packets
//
//    Returns when the Reception Fifos are empty, or
//            when an Actor returns non-zero, or
//            when count reaches zero.
//
//   On return, count will be debited by the number of packets received.
//   The return code from this function will be the return code from last actor invoked.
//
int BGLTorusPoll(   int *count );  // Poll both Fifo Groups
int BGLTorusPollF0( int *count );  // Poll only Fifo Group 0
int BGLTorusPollF1( int *count );  // Poll only Fifo Group 1


// Persistent MultiPacket Send Support
#define _BGL_SENDITEM_FCN_ALIGNED   (0)
#define _BGL_SENDITEM_FCN_UNALIGNED (1)
#define _BGL_SENDITEM_FCN_IOVEC     (2)
#define _BGL_SENDITEM_FCN_INDIRECT  (3)

// organize parameters to any of the send calls
typedef struct T_BGL_Torus_SendItem
                {
                int  send_fcn;          // which of above send functions to use
                _BGL_TorusPktHdr *hdr;  // address of primary (or only) header
                void *indirect;         // address of indirect header or NULL.
                void *data_or_iovec;    // either pointer to data or pointer to iovec
                int  bytes_or_count;    // either data bytes, or vec_count
                }
                _BGL_Torus_SendItem;

// an array of send-items
typedef _BGL_Torus_SendItem _BGL_Torus_SendList[];

// While Processing any incoming receives until recv_count expires,
//  send the list of packets.
// Returns when send_count and recv_count are both complete, or
//  when an Actor returns non-zero from a receive.
// On return, send_count and recv_count will be updated to indicate progress made.
// The return code from this function will be the return code from last actor invoked.
// For performance, you should create the send-list during your setup phase.
int BGLTorusWaitSendRecv( _BGL_Torus_SendItem send_array[], int *send_count, int *recv_count );

/*---------------------------------------------------------------------------*
 *     Torus Packet Layer Utility Routines and Data Declarations             *
 *---------------------------------------------------------------------------*/

extern _BGL_TorusFifoStatus _ts0_stat0_bounce,
                            _ts0_stat1_bounce,
                            _ts0_stat0recv_bounce,
                            _ts0_stat1recv_bounce;

extern _BGL_TorusFifoStatus _ts1_stat0_bounce,
                            _ts1_stat1_bounce,
                            _ts1_stat0recv_bounce,
                            _ts1_stat1recv_bounce;

// Per-FIFO Buffer for "Buffered" or "Unaligned" packets
extern _BGL_TorusPkt _ts0_r0_bounce,
                     _ts0_r1_bounce,
                     _ts0_r2_bounce,
                     _ts0_r3_bounce,
                     _ts0_r4_bounce,
                     _ts0_r5_bounce,
                     _ts0_rH_bounce,
                     _ts0_i0_bounce,
                     _ts0_i1_bounce,
                     _ts0_i2_bounce,
                     _ts0_iH_bounce;

extern _BGL_TorusPkt _ts1_r0_bounce,
                     _ts1_r1_bounce,
                     _ts1_r2_bounce,
                     _ts1_r3_bounce,
                     _ts1_r4_bounce,
                     _ts1_r5_bounce,
                     _ts1_rH_bounce,
                     _ts1_i0_bounce,
                     _ts1_i1_bounce,
                     _ts1_i2_bounce,
                     _ts1_iH_bounce;


//
// internal low-level functions
//

//
// Set hints for choosing an injection Fifo on both Groups 0 and 1
//
extern void _ts_MakeSendHints( _BGL_TorusPktHdr *hdr );

//
// Set hints for choosing an injection Fifo on Group 0
//
static void _ts_MakeSendHintsF0( _BGL_TorusPktHdr *hdr );

//
// Choose a Normal Priority Injection Fifo based on Header Contents and Chunks
//
//@MG Note: much of this code is only for the Emulator.
//
extern _BGL_TorusFifo *_ts_ChooseInjectionFifo( _BGL_TorusPktHdr *hdr );

//
// Return the Fifo address based on the Buffer (diagnostics use)
//
extern Bit32 _ts_Buffer2Fifo( _BGL_TorusPkt *buf );

//
// Select a packet buffer based on the fifo
//
extern _BGL_TorusPkt *_ts_Fifo2Buffer( Bit32 fifo );

//
// Choose a Normal Priority Injection Fifo based on Header Contents and Chunks
//
//@MG Note: much of this code is only for the Emulator.
//
_BGL_TorusFifo *_ts_ChooseInjectionFifo( _BGL_TorusPktHdr *hdr );


__END_DECLS

#endif // __ASSEMBLY__

#endif // Add nothing below this line.
