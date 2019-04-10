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
//      File: blade/spi/BGL_TreePktSPI.h
//
//   Purpose: Define Blade's Tree Network System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//            Matthias A. Blumrich (blumrich@us.ibm.com)
//
//     Notes: This SPI is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the Tree.
//
//   History: 01/09/2003: MAB - Created.
//
//
#ifndef _BGL_TREE_PKT_SPI_H  // Prevent multiple inclusion
#define _BGL_TREE_PKT_SPI_H


// Define to make SPI check for obvious errors.  WARNING: THIS REDUCES PERFORMANCE!
//#define _SPI_CHECK 1

// Define to make SPI announce every call that is made.  WARNING: THIS REDUCES PERFORMANCE!
// Also, this is not fully implemented.
//#define _SPI_CHECK_INFO  1

// Define to cause BGLTreeSend* to maintain the software injection checksums
// #define _SPI_TREE_DO_INJ_CSUM

#define _BLADE_ABORT(rc)  exit(rc)


#if !defined(__ASSEMBLY__)

__BEGIN_DECLS

/*---------------------------------------------------------------------------*
 *     Introduction to the Tree System Programming Interface                 *
 *---------------------------------------------------------------------------*/

// This interface is intended to simplify the process of writing BG/L tree network
// code.  Therefore, it naturally constrains the range of configuration and usage
// possibilities into a collection that should be commonly used.  There are some
// important assumptions in this design, stemming from both the tree architecture
// and a sensible use of the hardware as implemented.
//
// The tree provides two principle services to BG/L: I/O messaging, and collective
// operations (combining and broadcast).  Therefore, one virtual channel will always
// be reserved for use by the OS for point-to-point I/O messaging and some
// restricted broadcast and combining.  The other VC will be used by applications
// for combining and broadcast, but no point-to-point messaging.  Interrupts will be
// supported for point-to-point messages on the OS channel only.
//
// For early bringup, there are a few low-level functions that expose the entire
// tree architecture.
//
// In normal operation, the tree is expected to be used in one of three modes:
//
//   1) Balanced: In balanced mode, the two BlueGene/L Cores on each node are
//       logically labeled the Compute Processor and the I/O Processor, but they
//       share one address space.
//   2) Symmetric: Each core runs its own application thread, and the threads are
//       both using the tree.
//   3) Split: In this mode, each core runs a completely separate process.  In this
//       mode, the tree must be "virtualized", because there is only one application
//       VC (the other is used by the OS (or OS's).

/*---------------------------------------------------------------------------*
 *     Tree Packet Structure                                                 *
 *---------------------------------------------------------------------------*/

// All tree packets consist of a 32-bit header and a 256-byte payload.
//
// Point-to-point (P2P) tree packets, which are primarily used for communication
// between compute and I/O nodes, contain a hardware header, and software header
// at the beginning of the payload.  Collective tree packets contain just the
// hardware header and a payload.
//
// Software headers, 16 bytes, contain an Active Packet Function address, and T.B.D. ...

#define _BGL_TREE_PKT_MAX_BYTES   256
#define _BGL_TREE_PKT_MAX_BIT8    256
#define _BGL_TREE_PKT_MAX_BIT16   128
#define _BGL_TREE_PKT_MAX_BIT32    64
#define _BGL_TREE_PKT_MAX_BIT64    32
#define _BGL_TREE_PKT_MAX_BIT128   16

// Functions that are for use in balanced mode will respect these definitions:
#define _BGL_TREE_BALANCED_MODE_APP_VC      0
#define _BGL_TREE_BALANCED_MODE_OS_VC       1

typedef Bit128 _BGL_TreePyldFifo;

// The following definitions can be used when creating headers.

// ALU opcodes for collectives.
#define _BGL_TREE_OPCODE_NONE    (0x0)
#define _BGL_TREE_OPCODE_OR      (0x1)
#define _BGL_TREE_OPCODE_AND     (0x2)
#define _BGL_TREE_OPCODE_XOR     (0x3)
#define _BGL_TREE_OPCODE_MAX     (0x5)
#define _BGL_TREE_OPCODE_ADD     (0x6)

// Common operand sizes.
#define _BGL_TREE_OPSIZE_BIT16   (0x0)
#define _BGL_TREE_OPSIZE_BIT32   (0x1)
#define _BGL_TREE_OPSIZE_BIT64   (0x3)
#define _BGL_TREE_OPSIZE_BIT128  (0x7)
#define _BGL_TREE_OPSIZE_SHORT   (0x0)
#define _BGL_TREE_OPSIZE_INT     (0x1)
#define _BGL_TREE_OPSIZE_LONG    (0x3)
#define _BGL_TREE_OPSIZE_QUAD    (0x7)

// Software checksum counters.  Use these when calling BGLTreeInjectionCsumGet.
#define _BGL_TREE_CSUM_VC0_HEADER   (0)
#define _BGL_TREE_CSUM_VC0_PAYLOAD  (1)
#define _BGL_TREE_CSUM_VC1_HEADER   (2)
#define _BGL_TREE_CSUM_VC1_PAYLOAD  (3)

// Checksum mode.
#define _BGL_TREE_CSUM_NONE      (0x0)   // Do not include packet in checksums.
#define _BGL_TREE_CSUM_SOME      (0x1)   // Include header in header checksum.  Include all but
                                         //   first quadword in payload checksum.
#define _BGL_TREE_CSUM_CFG       (0x2)   // Include header in header checksum.  Include all but
                                         //   specified number of 16-bit words in payload checksum.
#define _BGL_TREE_CSUM_ALL       (0x3)   // Include entire packet in checksums.

// The Tree Hardware Packet Headers
//  There are two hardware header formats: one for point-to-point packets and the
//  other for collective (basically all other) packets.
//
//  These structures should be considered "opaque".  They are supplied for informational
//  purposes only.  Functions are provided to construct/fillout these headers.
typedef struct T_BGL_TreePtpHdr
                {
                unsigned Class      : 4; // Class number (a.k.a virtual tree number)
                unsigned Ptp        : 1; // Must be 1 for PTP header format.
                unsigned Irq        : 1; // 1=request interrupt when received, 0=not
                unsigned PtpTarget  :24; // Ptp packet target (matched to node address)
                unsigned CsumMode   : 2; // Injection checksum mode
                }
                _BGL_TreePtpHdr;


typedef struct T_BGL_TreeCollectiveHdr
                {
                unsigned Class      : 4; // Class number (a.k.a virtual tree number)
                unsigned Ptp        : 1; // Must be 0 for collective header format.
                unsigned Irq        : 1; // 1=request interrupt when received, 0=not
                unsigned OpCode     : 3; // 000=ordinary routed packet, else ALU opcode
                unsigned OpSize     : 7; // Operand size
                unsigned Tag        :14; // Software tag
                unsigned CsumMode   : 2; // Injection checksum mode
                }
                _BGL_TreeCollectiveHdr;

// The Tree Packet Software Header: Active Function and Extra Argument
typedef struct T_BGL_Tree_HdrSW0
                {
                unsigned extra      : 10; // additional untyped 10bit argument (may become 8bits)
                unsigned fcn        : 22; // "massaged" Active Function Pointer (not directly usable)
                }
                _BGL_Tree_HdrSW0;

// The Tree Packet Software Header: 32bit untyped argument
typedef struct T_BGL_Tree_HdrSW1
                {
                unsigned long arg;   // untyped 32bit argument
                }
                _BGL_Tree_HdrSW1;

// These two words are T.B.D....
typedef struct T_BGL_Tree_HdrSW2
                {
                unsigned long arg;   // untyped 32bit argument
                }
                _BGL_Tree_HdrSW2;

typedef struct T_BGL_Tree_HdrSW3
                {
                unsigned long arg;   // untyped 32bit argument
                }
                _BGL_Tree_HdrSW3;

// Pull together all components of the Tree Software Header
typedef struct T_BGL_TreeSwHdr
                {
                _BGL_Tree_HdrSW0 swh0;
                _BGL_Tree_HdrSW1 swh1;
                _BGL_Tree_HdrSW2 swh2;
                _BGL_Tree_HdrSW3 swh3;
                }
                ALIGN_QUADWORD _BGL_TreeSwHdr;


// Convenient union of the two hardware header types
typedef union T_BGL_TreeHwHdr
               {
               _BGL_TreePtpHdr         PtpHdr;
               _BGL_TreeCollectiveHdr  CtvHdr;
               }
               _BGL_TreeHwHdr;

// Describe the layout of a point-to-point tree packet payload.
typedef struct T_BGL_TreePtpPktPyld
                {
                _BGL_Tree_HdrSW0   swh0;
                _BGL_Tree_HdrSW1   swh1;
                _BGL_Tree_HdrSW2   swh2;
                _BGL_Tree_HdrSW3   swh3;
                Bit8               payload[ _BGL_TREE_PKT_MAX_BYTES - sizeof(_BGL_TreeSwHdr) ];
                }
                ALIGN_QUADWORD _BGL_TreePtpPktPyld;

// Describe the layout of a collective tree packet payload.
typedef struct T_BGL_TreeCollectivePktPyld
                {
                Bit8               payload[ _BGL_TREE_PKT_MAX_BYTES ];
                }
                ALIGN_QUADWORD _BGL_TreeCollectivePktPyld;

// Union all Tree Packet Payloads, and align to L3.
typedef union T_BGL_TreePayload
               {
               Bit8                       flat  [ _BGL_TREE_PKT_MAX_BYTES ];
               Bit8                       flat8 [ _BGL_TREE_PKT_MAX_BIT8  ];
               Bit16                      flat16[ _BGL_TREE_PKT_MAX_BIT16 ];
               Bit32                      flat32[ _BGL_TREE_PKT_MAX_BIT32 ];
               Bit64                      flat64[ _BGL_TREE_PKT_MAX_BIT64 ];
               Bit128                     flat128[_BGL_TREE_PKT_MAX_BIT128];
               _BGL_TreePtpPktPyld        ptp;
               _BGL_TreeCollectivePktPyld col;
               }
               ALIGN_L3_CACHE _BGL_TreePayload;

/*---------------------------------------------------------------------------*
 *     Tree Fifo Status                                                      *
 *---------------------------------------------------------------------------*/
//
// Tree Fifo Status:
//   Each 32-bit status word contains all of the fifo status for a virtual channel
//   and the reception fifo interrupt request status.  Note that in reality, only
//   the low-order bit of the RecIrq field is relevant.  Similarly, InjHdrCount and
//   RecHdrCount are really 4-bit values, so only the 4 low-order bits are relevant.
//   In both cases, the hardware returns 0 for undefined bits.
#define _BGL_TREE_STATUS_MAX_PKTS (8)

typedef struct T_BGL_TreeFifoStatus
                {
                unsigned InjPyldCount  : 8; // Injection payload fifo entry (qword) count
                unsigned _notdefined1  : 4;
                unsigned InjHdrCount   : 4; // Injection header fifo entry count
                unsigned RecPyldCount  : 8; // Reception payload fifo entry (qword) count
                unsigned _notdefined2  : 3;
                unsigned RecIrq        : 1; // If non-zero, reception fifo contains an IRQ packet
                unsigned RecHdrCount   : 4; // Reception header fifo entry count
                }
                ALIGN_QUADWORD _BGL_TreeFifoStatus;

// Obtain status for a Tree Virtual Channel
void BGLTreeGetStatus( int vc, _BGL_TreeFifoStatus *stat );

// Obtain status for Tree Virtual Channel 0
void BGLTreeGetStatusVC0( _BGL_TreeFifoStatus *stat );

// Obtain status for Tree Virtual Channel 1
void BGLTreeGetStatusVC1( _BGL_TreeFifoStatus *stat );

/*---------------------------------------------------------------------------*
 *     Tree Active Point-To-Point Packet Signatures for Reception            *
 *---------------------------------------------------------------------------*/

// THIS SECTION TAKEN FROM TORUS SPI.  IT WILL BE RE_THOUGHT...
//
// The Tree Active Packet functions are typically dispatched from one of the
// poll routines.  The poll routines will check the function's return code, and
// when non-zero, return to the caller of the poll routine with that return code.
// On zero return code, the poll routine will continue polling for packets until
// its normal return condition(s) are met (eg, empty fifos, number of packets, etc.).

// Non-Buffered Tree Packet Active Function Signature:
//  When this function is called, the head of the fifo is the first quadword
//   of the packet payload.
typedef int (*BGLTreeActivePacketHandler)(
              _BGL_TreePyldFifo *fifo,    // Address of Tree Payload Reception Fifo.
              Bit32  arg,                 // 32 bits of primary arg info
              Bit32  extra );             // 10 bits of extra arg info (promoted to 32bits)


// Buffered Tree Packet Active Function Signature:
//  Use when you require random or unaligned access to payload data.  For example, you
//    might cast a "struct" over the packet payload and access that structure arbitrarily.
//    E.g., System software uses this for message layer protocol packets.
//  When your Active Function returns, the buffer is discarded.
typedef int (*BGLTreeBufferedActivePacketHandler)(
              void   *payload,            // Address of Packet Payload Stored in Memory (assume L1 cache)
              Bit32  arg,                 // 32 bits of primary arg info
              Bit32  extra );             // 10 bits of extra arg info (promoted to 32bits)


// Indirect Tree Packet Active Function Signature:
//  Implies Unbuffered Payload, but Indirect Argument is buffered.
//  When this function is called, the head of the fifo is the first quadword
//   of the packet payload.
typedef int (*BGLTreeIndirectActivePacketHandler)(
              _BGL_TreePyldFifo *fifo,    // Address of Torus Reception Fifo containing Payload.
              Bit32  arg,                 // 32 bits of primary arg info
              Bit32  extra,               // 10 bits of extra arg info (promoted to 32bits)
              void  *indirect );          // Indirect Argument Buffered Quadword

/*---------------------------------------------------------------------------*
 *     Virtual Tree (Class) Construction Interfaces                          *
 *---------------------------------------------------------------------------*/

// Virtual trees are constructed by programming the route descriptor registers
// at each node.  There are two basic types of virtual trees: collective and
// point-to-point (PTP).  Collective trees are used by applications to perform
// collective operations and broadcasts.  PTP trees are used by the OS to send
// and receive active point-to-point packets.
//
// On both types of virtual trees, there are two types of nodes: root and non-root.
// Non-root nodes are always configured with the downtree channel(s) as sources
// the uptree channel as the target, and the local injection as a source if the
// local node is participating.  On PTP trees, a root node receives packets from
// down tree and sends packets back down tree.  Therefore, it is configured with the
// downtree channel(s) as sources, and the local reception as the target.  A
// collective tree root bounces collective operations and broadcasts back down
// the tree.  It is configured with the downtree channel(s) as both sources
// and targets.  If the root node is participating in the collective tree, then
// the local injection is also a source.  This leads to the following definition:

#define _BGL_NODETYPE_COLLECTIVEROOT   (0)
#define _BGL_NODETYPE_PTPROOT          (1)
#define _BGL_NODETYPE_NONROOT          (2)

// This structure is used to specify which channels are uptree and downtree on
// a virtual tree.
typedef struct T_BGL_TreeChannelMap
                {
                Bit8     uptree;      // Uptree channel (0, 1, or 2), or -1 if none.
                Bit8     downtreeA;   // Downtree channel A (0, 1, or 2), or -1 if none.
                Bit8     downtreeB;   // Downtree channel B (0, 1, or 2), or -1 if none.
                Bit8     reserved;
                }
                _BGL_TreeChannelMap;

//@MG: added to get this to compile
typedef struct T_BGL_TreeLayout
                {
                }
                _BGL_TreeLayout;

// This function retrieves the physical layout of the tree within the partition.
int BGLTreeGetPhysicalLayout(
        _BGL_TreeLayout  *layout );   // Previously created data structure for layout

// This function extracts useful information from the physical tree layout.
int BGLTreeGetConnectivity(
        _BGL_TreeLayout      *layout,      // Physical tree layout to parse
        _BGL_TreeChannelMap  *computeMap,  // Returns local channel connectivity of collective tree
        _BGL_TreeChannelMap  *IoMap,       // Returns local channel connectivity of PTP (I/O) tree
        int                  *IOnodeAddr,  // Returns address of I/O node
        int                  nodeType );   // Collective root, PTP root, or non-root

// Configure a Route Descriptor Register to add this node to a virtual tree.
// Multiple collective trees can be created by finagling the channel map and
// node type.
int BGLTreeCreateVirtualTree(
        int                  vt,         // Virtual tree (class) number (12-15 are reserved for OS)
        _BGL_TreeChannelMap  *chanMap,   // Local channel connectivity for this tree
        int                  nodeType,   // Collective root, PTP root, or non-root
        int          IamParticipating ); // If non-zero, local is participating

// Enable local loopback on a channel (0, 1, or 2).
void BGLTreeWrapChannel( int channel );

// Disable local loopback on a channel (0, 1, or 2).
void BGLTreeUnwrapChannel( int channel );

// Enable header class MSB inversion on a send channel (0, 1, or 2).  Note that inversion occurs
// before the header enters the send fifo!
void BGLTreeInvertClassMsb( int channel );

// Disable header class MSB inversion on a send channel (0, 1, or 2).
void BGLTreeUninvertClassMsb( int channel );

// The following interface allows for fine-grain control of the RDR register
// contents.  Use bit-wize OR'd together to create a route specification.
#define _BGL_TREE_RDR_SRC0    (0x1000)  // Bit Number  3 (MSb is bit number 0)
#define _BGL_TREE_RDR_SRC1    (0x2000)  // Bit Number  2
#define _BGL_TREE_RDR_SRC2    (0x4000)  // Bit Number  1
#define _BGL_TREE_RDR_SRCL    (0x0002)  // Bit Number 14
#define _BGL_TREE_RDR_TGT0    (0x0100)  // Bit Number  7
#define _BGL_TREE_RDR_TGT1    (0x0200)  // Bit Number  6
#define _BGL_TREE_RDR_TGT2    (0x0400)  // Bit Number  5
#define _BGL_TREE_RDR_TGTL    (0x0001)  // Bit Number 15

// OR of all valid Source and Target bits for SrtTgtEnable validation.
#define _BGL_TREE_RDR_ACCEPT (0x7703)

// Specify class route using a bit vector of the _BGL_RDR_* definitions.
int BGLTreeConfigureClass(
        int  vt,              // Virtual tree (class) number (12-15 are reserved for OS)
        int  SrcTgtEnable );  // Bit vector defining sources and targets.

// Return the _BGL_RDR_* definition of the specified Class.
int BGLTreeGetClass(
        int  vt );            // Virtual tree (class) number (12-15 are reserved for OS)


#define _BGL_TREE_PRIV_CLASS_START   (12)    // Reserve classes 12-15 for the OS

// Allocates and returns the lowest-numbered available "application" class (those less
// than _BGL_TREE_PRIV_CLASS_START).  Returns a negative number if there are no classes
// available.
int BGLTreeAllocateClass( void );

// Just like calling BGLTreeConfigureClass( BGLTreeAllocateClass(), SrcTgtEnable ).
int BGLTreeAllocateAndConfigureClass( int SrcTgtEnable );

// Allocates and returns the lowest-numbered available "privileged" class (those
// beginning with _BGL_TREE_PRIV_CLASS_START).  Returns a negative number if there are
// no privileged classes available.
int BGLTreeAllocateClassPrivileged( void );

// Forces the allocation of a specific class.  Non-zero return indicates that the class
// has already been allocated, and no action has been taken.
int BGLTreeAllocateThisClass( int vt );

// De-allocates the specified class, and clears the associated RDR register bits.
void BGLTreeFreeClass( int vt );

// Query number of allocated classes
int BGLTreeClassesAllocated( void );
int BGLTreeClassesAllocatedPrivileged( void );

// Query number of free classes
int BGLTreeClassesFree( void );
int BGLTreeClassesFreePrivileged( void );

// Set point-to-point receive address.
int BGLTreeSetPtpAddress( int addr );

// Get point-to-point receive address.
int BGLTreeGetPtpAddress( void );   // Set NADDR

/*---------------------------------------------------------------------------*
 *     Tree Header Construction Interfaces                                   *
 *---------------------------------------------------------------------------*/

// Because Tree Headers are expensive to create, the interface provides a choice
// of creating the headers "on the fly" or pre-creating the headers.  When you
// use regular and repeated communication patterns (a.k.a persistent messages),
// pre-creating headers during the setup phase can provide significant performance
// benefits.

// Create a COLLECTIVE H/W tree header.  This function allows full control of header creation.
// QUESTION: SHOULD THIS RETURN AN ERROR CODE, RATHER THAN THE POINTER?
_BGL_TreeHwHdr *BGLTreeMakeCollectiveHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int irq,                     // irq.  If combine, must be the same for all callers.
                           int opcode,                  // ALU opcode (000 for ordinary packets)
                           int opsize,                  // ALU operand size
                           int tag,                     // software tag. If combine, must be the same for all callers.
                           int csumMode );              // injection checksum mode

// Create a POINT-TO-POINT H/W tree header.  This function allows full control of header construction.
_BGL_TreeHwHdr *BGLTreeMakePtpHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int irq,                     // interrupt request
                           int PtpTarget,               // point-to-point target (node address to accept)
                           int csumMode );              // injection checksum mode

// Create a point-to-point header for the common case.  In this header:
// (1) The irq bit is always set
// (2) The injection checksum mode is set to 1 (include H/W header, exclude first qword of payload)
_BGL_TreeHwHdr *BGLTreeMakeSendHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int PtpTarget );             // point-to-point target (node address to accept)

// Create a broadcast header for the common case.  In this header:
// (1) the irq bit is always cleared
// (2) the opcode is set to NONE (so operand size is irrelevant).
// (3) the injection checksum mode is set to 3 (include everything)
_BGL_TreeHwHdr *BGLTreeMakeBcastHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int tag );                   // software tag.  If combine, must be the same for all callers.

// Create a collective header for the common ALU case.  In this header:
// (1) the irq bit is always cleared
// (2) the tag is undefined
// (3) the injection checksum mode is set to 3 (include everything)
_BGL_TreeHwHdr *BGLTreeMakeAluHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int opcode,                  // ALU opcode
                           int opsize );                // ALU operand size

// Software headers T.B.D.
_BGL_TreeSwHdr *BGLTreeMakeSwHdr(
                           _BGL_TreeSwHdr *hdr,         // Filled in on return
                           int vt
                           // T.B.D.
                           );

/*---------------------------------------------------------------------------*
 *     Diagnostic Injection Checksum Routines
 *---------------------------------------------------------------------------*/

// These routines assume that partial packets are always rounded to quadword
// granularity and padded with zeros.

// Initialize header and payload injection checksums.
void BGLTreeInjectionCsumInit( int vc, Bit32 hdr_init, Bit32 pyld_init );

// Add header and payload to injection checksums.
void BGLTreeInjectionCsumAdd( int vc, Bit32 hdr, Bit16 *pyld, int bytes );

// Return a checksum.
Bit32 BGLTreeInjectionCsumGet( int offset );

// Print all calculated checksums.
void BGLTreeInjectionCsumPrint( char *preprint, int print_header );


/*---------------------------------------------------------------------------*
 *     Low-Level Tree Packet Send Routines                                   *
 *---------------------------------------------------------------------------*/

// The following routines should only be called when there is fifo room to send.
// That is, they will inject unconditionally and over-run if necessary.
// Note: For on-the-fly header creation, call one of the above header creation
// routines as a parameter to this function.

// Use for aligned payload of 16 quadwords.
int BGLTreeRawSendPacket(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld );  // Source address of payload (16-byte aligned)

// Use for aligned payload of 1-16 quadwords.
int BGLTreeRawSendPacketPartial(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload (16-byte aligned)
       int            bytes );  // Payload bytes (multiple of 16, up to 256)

// Use for unaligned payload or bytes = 1-256.
int BGLTreeRawSendPacketUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload
       int            bytes );  // Payload bytes (1-256)

// Use for aligned payload of 1-16 quadwords.
int BGLTreeRawSendPayloadPartial(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload (16-byte aligned)
       int            bytes );  // Payload bytes (multiple of 16, up to 256)

// Use for unaligned payload or bytes = 1-256.
int BGLTreeRawSendPayloadUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload
       int            bytes );  // Payload bytes (1-256)

// Inject a single header
int BGLTreeRawSendHeader(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW ); // Previously created hardware header (any type)

// Use for unaligned payload or bytes = 1-256. This function will not fill out a payload, but will fill to quadword.
int BGLTreeRawInjectBytesUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload
       int            bytes );  // Payload bytes (1-256)

// The following routines will block indefinitely until there is room in the specified
// injection fifo. This implies that they poll the status register.
//
// !! WARNING: If the payload of a packet send routine is greater than a single packet's
// !! worth of data, then every packet will use the specified header.

// Use for aligned payload that is a multiple of 16 bytes in length.
int BGLTreeSend(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload (16-byte aligned)
       int            bytes );  // Payload bytes (multiple of 16)

// Use for any size payload at any alignment.
int BGLTreeSendUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
       void           *pyld,    // Source address of payload
       int            bytes );  // Payload bytes


/*---------------------------------------------------------------------------*
 *     Low-Level Tree Packet Receive Routines                                *
 *---------------------------------------------------------------------------*/

// The following routines should only be called when there is something to receive.
// That is, they will receive unconditionally and under-run if necessary.

// Use for aligned payload of 16 quadwords.
int BGLTreeRawReceivePacket(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld );  // Payload buffer (16-byte aligned)

// Use for aligned payload of 1-16 quadwords.
int BGLTreeRawReceivePacketPartial(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld,    // Payload buffer (16-byte aligned)
       int            bytes );  // Payload bytes (multiple of 16, up to 256)

// Use for unaligned payload or bytes = 1-256.
int BGLTreeRawReceivePacketUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld,    // Payload buffer
       int            bytes );  // Payload bytes (1-256)

// Use for aligned payload of 1-16 quadwords.
int BGLTreeRawReceivePayloadPartial(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Payload buffer (16-byte aligned)
       int            bytes );  // Payload bytes (multiple of 16, up to 256)

// Use for unaligned payload or bytes = 1-256.
int BGLTreeRawReceivePayloadUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Payload buffer
       int            bytes );  // Payload bytes (1-256)

// Receive a single header
int BGLTreeRawReceiveHeader(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW ); // Hardware header buffer

// Use for unaligned payload or bytes = 1-256. This function will not dump the remainder of a payload, but will
// dump the remainder of a quadword.
int BGLTreeRawReceiveBytesUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       void           *pyld,    // Source address of payload
       int            bytes );  // Payload bytes (1-256)

// The following routines will block indefinitely until there is data in the specified
// receive fifo. This implies that they poll the status register.

// Use for aligned payload that is a multiple of 16 bytes in length.
int BGLTreeReceive(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld,    // Payload buffer (16-byte aligned)
       int            bytes );  // Payload bytes (multiple of 16)

typedef void (* YieldFuncType ) ();

int BGLTreeReceiveWithYield(
			    int            vc,       // Virtual channel (0 or 1)
			    _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
			    void           *pyld,    // Payload buffer (16-byte aligned)
			    int            bytes,
			    YieldFuncType  YieldFunc );  // Payload bytes (multiple of 16)

// Use for any size payload at any alignment.
int BGLTreeReceiveUnaligned(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld,    // Payload buffer
       int            bytes );  // Payload bytes

int BGLTreeReceiveUnalignedWithYield(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld,    // Payload buffer
       int            bytes,
       YieldFuncType  YieldFunc );  // Payload bytes


/*---------------------------------------------------------------------------*
 *     Low-Level Tree Interrupt Interface                                    *
 *---------------------------------------------------------------------------*/

// Specify the SLIHs to handle standard and critical interrupts.  The same ISR
// can be used for both.
int BGLTreeSetStandardISR( void (*TreeStdISR()) );
int BGLTreeSetCriticalISR( void (*TreeCritISR()) );

// These two functions control the masking and un-masking of reception interrupt
// requests at the tree (not the BIC). The bit positions are defined in bl_tree.h.
int BGLTreeEnableInjectionIrqs( Bit32 flags );
int BGLTreeDisableInjectionIrqs( Bit32 flags );

// These two functions control the masking and un-masking of reception interrupt
// requests at the tree (not the BIC). The bit positions are defined in bl_tree.h.
int BGLTreeEnableReceptionIrqs( Bit32 flags );
int BGLTreeDisableReceptionIrqs( Bit32 flags );

// These four functions control the masking and un-masking of standard and critical
// tree interrupt requests at the BIC. They may be unnecessary if equivalents are
// provided elsewhere in the SPI.  The bit positions are defined in bl_bic.h.
int BGLTreeEnableStandardBicIrqs( Bit32 flags );
int BGLTreeDisableStandardBicIrqs( Bit32 flags );
int BGLTreeEnableCriticalBicIrqs( Bit32 flags );
int BGLTreeDisableCriticalBicIrqs( Bit32 flags );

/*---------------------------------------------------------------------------*
 *     Tree Point-To-Point Send Routines                                     *
 *---------------------------------------------------------------------------*/

// These functions are expected to be used for point-to-point messaging by the
// OS.  A message consists of a stream of active packets, each of which contains a
// software header, traveling on a single virtual channel and class.

// Send a Tree Point-To-Point Packet
//  The packet payload consists of a 16-byte software header and 240 bytes of data.
//  Note: For on-the-fly header creation, call one of the above header creation
//  routines as a parameter to these functions.

// Use for aligned payload of 1-16 quadwords.
int BGLTreeSendPtpPacket(
       int             vc,       // Virtual channel (0 or 1)
       _BGL_TreePtpHdr *hdrHW,   // Previously created PTP hardware header
       _BGL_TreeSwHdr  *hdrSW,   // Previously created software header (16byte-aligned)
       void            *data,    // Source address of payload (16byte-aligned)
       int             bytes );  // Payload bytes (multiple of 16, up to 256)

// Use for unaligned data or bytes <= 256.  (256-bytes) bytes of undefined
// padding will be added to fill the payload.
int BGLTreeSendPtpPacketUnaligned(
       int             vc,       // Virtual channel (0 or 1)
       _BGL_TreePtpHdr *hdrHW,   // Previously created PTP hardware header
       _BGL_TreeSwHdr  *hdrSW,   // Previously created software header
       void            *data,    // Source address of payload (16byte aligned)
       int             bytes );  // Payload bytes

// Gather a packet using an iovec_g descriptor (array of {base, len} pairs).
//  Presumably you will send such a packet to an active function
//  that scatters the data according to some iovec_g it knows about. Or
//  perhaps you passed the address of that iovec_g as an argument when
//  you created the header.
//  The total size of the iovec_g must not exceed 240 bytes.
//  The packet is "packed" meaning that no alignment or padding is
//  added between iovec_g data elements.  For performance, please try
//  to maintain reasonable data alignment.

int BGLTreeSendPtpPacketIovec(
       int             vc,           // Virtual channel (0 or 1)
       _BGL_TreePtpHdr *hdrHW,       // Previously created PTP hardware header
       _BGL_TreeSwHdr  *hdrSW,       // Previously created software header
       iovec_g           *data_vec,    // Data "Gather" information
       int             vec_count );  // Count of elements in the i/o vector

// Possible VC-specific variants of PTP send functions:
int BGLTreeSendPtpPacketVC0( _BGL_TreePtpHdr *hdrHW, _BGL_TreeSwHdr *hdrSW, void *data );
int BGLTreeSendPtpPacketVC1( _BGL_TreePtpHdr *hdrHW, _BGL_TreeSwHdr *hdrSW, void *data );

int BGLTreeSendPtpPacketUnalignedVC0( _BGL_TreePtpHdr *hdrHW, _BGL_TreeSwHdr *hdrSW, void *data, int bytes );
int BGLTreeSendPtpPacketUnalignedVC1( _BGL_TreePtpHdr *hdrHW, _BGL_TreeSwHdr *hdrSW, void *data, int bytes );

int BGLTreeSendPtpPacketIovecVC0( _BGL_TreePtpHdr *hdrHW, _BGL_TreeSwHdr *hdrSW, iovec_g *data_vec, int vec_count );
int BGLTreeSendPtpPacketIovecVC1( _BGL_TreePtpHdr *hdrHW, _BGL_TreeSwHdr *hdrSW, iovec_g *data_vec, int vec_count );


/*---------------------------------------------------------------------------*
 *     Tree Point-To-Point Receive Initiators                                *
 *---------------------------------------------------------------------------*/

// Poll for: count packets, or
//           forever if count is NULL, or
//           until an Actor returns non-zero.
//   On return, count (if non-NULL) will be updated with number of packets received.
//   The return code from this function will be the return code from last actor invoked.
//
// Note: The Actor function is dispatched in the thread that called these routines.

int BGLTreePollCount(
        int vc,           // Virtual channel to poll.  Both if -1.
        int *count );     // In: maximum packets; Out: packets received.

int BGLTreePoll( int vc );    // equivalent to BGLTreePollCount( vc, NULL ).

// Persistent MultiPacket Send Support
#define _BGL_SENDITEM_FCN_ALIGNED   (0)
#define _BGL_SENDITEM_FCN_UNALIGNED (1)
#define _BGL_SENDITEM_FCN_IOVEC     (2)

// organize parameters to any of the send calls
typedef struct T_BGL_Tree_SendItem
                {
                int  vc;                // virtual channel
                int  send_fcn;          // which of above send functions to use
                _BGL_TreeHwHdr *hdrHW;  // address of primary (or only) hardware header
                _BGL_TreeSwHdr *hdrSW;  // address of software header or NULL.
                void *data_or_iovec;    // either pointer to data or pointer to iovec_g
                int  bytes_or_count;    // either data bytes, or vec_count
                }
                _BGL_Tree_SendItem;

#ifdef NEVERDEFINED
// an array of send-items
typedef _BGL_Tree_SendItem _BGL_Tree_SendList[];
#endif

// While Processing any incoming receives until recv_count expires,
//  send the list of packets.
// Returns when send_count and recv_count are both complete, or
//  when an Actor returns non-zero from a receive.
// On return, send_count and recv_count will be updated to indicate progress made.
// The return code from this function will be the return code from last actor invoked.
// For performance, you should create the send-list during your setup phase.

int BGLTreePollSendRecv(
        int vc,           // Virtual channel to poll.  Both if -1.
        _BGL_Tree_SendItem *send_array,
        int *send_count,
        int *recv_count );

/*---------------------------------------------------------------------------*
 *     Split Mode Tree Reduction Interface                                   *
 *---------------------------------------------------------------------------*/

// These functions are appropriate for execution on one core.
//
// This function starts a single reduction and completes it before returning.
// IMPORTANT:
//  (1) the specified VC must be dedicated to this function during its operation
//  (2) The irq bit must not be set in any of the headers
int BGLTreeReduce(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Must be qword-aligned.  Len mod 256 must be 0.
       int                     bytes );       // Total length in bytes

// Variant for use with unaligned data or an operand array that is not a multiple of 256 bytes
int BGLTreeReduceUnaligned(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Contiguous array of operands
       int                     bytes );       // Size must be an integral number of operands

// Basic streaming of multiple reductions
//  This function processes a queue of work items specified as an array of headers and a
//  matching array of io vectors ({base, len} pairs) containing the operands.  The operand
//  arrays specified in the io vectors can be any multiple of 256 bytes in length, and
//  must be quadword-aligned.  Multi-packet operand arrays from a single io vector will use
//  the same header.  This function will send the operands and receive the results,
//  which will overwrite the operands.  It will return when the entire set of reductions
//  is complete.
// IMPORTANT:
//  (1) the specified VC must be dedicated to this function during its operation
//  (2) The irq bit must not be set in any of the headers

int BGLTreeReduceIovec(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdrs,         // Previously created array of collective headers to use.
       iovec_g                   *data_vec,     // Base must be qword-aligned.  Len mod 256 must be 0.
       int                     vec_count );   // Count of headers and io vectors.

// Variant for use with unaligned data or a operand arrays that are not multiples of 256 bytes
int BGLTreeReduceIovecUnaligned(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdrs,         // Previously created array of collective headers to use.
       iovec_g                   *data_vec,     // Operand arrays
       int                     vec_count );   // Count of headers and io vectors.

// The following interface generalizes pipelined reductions from a single core.  It
// allows for the gathering of individual operand arrays, as well as the ability to pipeline
// several reductions, as in BGLTreeReduceIovec().  It may be overkill...

#define _BGL_REDUCEITEM_FCN_ALIGNED   (0)
#define _BGL_REDUCEITEM_FCN_UNALIGNED (1)
#define _BGL_REDUCEITEM_FCN_IOVEC     (2)

// A send item specifies a single reduction to perform, and allows the operands to be
// gathered.
typedef struct T_BGL_Tree_ReduceSendItem
                {
                int  reduce_fcn;        // which of above reduce functions to use
                int  vc;                // virtual channel
                _BGL_TreeHwHdr *hdrHW;  // address of primary (or only) hardware header
                void *data_or_iovec;    // either pointer to data or pointer to iovec_g
                int  bytes_or_count;    // either data bytes, or vec_count
                }
                _BGL_Tree_ReduceSendItem;

// An array of reduction send items
typedef _BGL_Tree_ReduceSendItem _BGL_Tree_ReduceSendList[];

// While processing any incoming reductions until recv_count expires,
//  send the list of reduction packets.
// Returns when send_count and recv_count are both complete.
// On return, send_count and recv_count will be updated to indicate progress made.
// For performance, you should create the send_array during your setup phase.

int BGLTreeReduceList( _BGL_Tree_ReduceSendItem send_array[], int *send_count, int *recv_count );

/*---------------------------------------------------------------------------*
 *     Symmetric Mode Tree Reduction Interface                               *
 *---------------------------------------------------------------------------*/

// The following interface is used in symmetric mode, where one thread injects operands while
// another, running on the other core, receives results.
//
// NOTE: Alternatively, there could be another argument to the split-mode functions to
// specify whether they are send-only or receive-only.
//
// IMPORTANT:
//  (1) the specified VC must be dedicated to this function during its operation
//  (2) the irq bit must not be set in any of the headers
//  (3) if *operandBuf is the same as *operands, then *operands must be flushed from L1

int BGLTreeSendReduce(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Must be qword-aligned.  Len mod 256 must be 0.
       int                     bytes );       // Total length in bytes

int BGLTreeRecvReduce(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdr,          // Header, for verification.  Skip, if NULL.
       void                    *operandBuf,   // Must be qword-aligned.  Len mod 256 must be 0.
       int                     bytes );       // Total buffer size in bytes.  Will receive this much.

// Variant for use with unaligned data or an operand array that is not a multiple of 256 bytes.
int BGLTreeSendReduceUnaligned(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Contiguous array of operands
       int                     bytes );       // Size must be an integral number of operands

int BGLTreeRecvReduceUnaligned(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdr,          // Header, for verification.  Skip, if NULL.
       void                    *operandBuf,   // Buffer to fill
       int                     bytes );       // Total buffer size in bytes.  Will receive this much.

// The following functions operate like BGLTreeReduceIovec and its variants above.

int BGLTreeSendReduceIovec(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdrs,         // Previously created array of collective headers to use.
       iovec_g                   *data_vec,     // Base must be qword-aligned.  Len mod 256 must be 0.
       int                     vec_count );   // Count of headers and io vectors.

int BGLTreeRecvReduceIovec(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdrs,         // Headers, for verification.  Skip, if NULL.
       iovec_g                   *data_vec,     // Base must be qword-aligned.  Len mod 256 must be 0.
       int                     vec_count );   // Count of headers and io vectors.  Will recv this much.

int BGLTreeSendReduceIovecUnaligned(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdrs,         // Previously created array of collective headers to use.
       iovec_g                   *data_vec,     // Operand arrays
       int                     vec_count );   // Count of headers and io vectors.

int BGLTreeRecvReduceIovecUnaligned(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdrs,         // Headers, for verification.  Skip, if NULL.
       iovec_g                   *data_vec,     // Buffers to fill.
       int                     vec_count );   // Count of headers and io vectors.  Will recv this much.

// NOTE: Could add the really generic function with the gather-scatter list here...

/*---------------------------------------------------------------------------*
 *     Balanced Mode Tree Reduction Interface                                *
 *---------------------------------------------------------------------------*/

// The following interface is used in balanced mode, where the IO processor performs the reduction.
// Every reduction that is started will be completed in place, with no guaranteed delay between
// the start and when the operands are overwritten with results.  The Waitfor function is used to
// rendezvous with the IO proceesor to complete all or some number of the reductions that have
// been started.  However, the data could have returned long before Waitfor is called.
// IMPORTANT:
//  (1) The virtual channel defined by BALANCED_MODE_APP_VC will be used
//  (2) The irq bit must not be set in any of the headers

// Tell the IO processor to start a reduction.  The operands are expected to be flushed from L1!
// This function can be called multiple times before calling the waitfor function.
int BGLTreeStartReduce(
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Must be qword-aligned.  Len mod 256 must be 0.
       int                     bytes );       // Operand array size in bytes.

// Variant for use with unaligned data or an operand array that is not a multiple of 256 bytes
int BGLTreeStartReduceUnaligned(
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Contiguous array of operands.
       int                     bytes );       // Operand array size in bytes.

// Wait for recv_count reductions to complete, or all if recv_count is -1.  Returns the number
// completed in recv_count.
int BGLTreeWaitforReduce( int *recv_count );

// NOTE: Could add a gather-scatter start function too...
// Note 2: Perhaps the core calling Start should inject, avoiding the flush.

/*---------------------------------------------------------------------------*
 *     Split And Symmetric Mode Tree Broadcast Interface                     *
 *---------------------------------------------------------------------------*/

// For all of these functions, it is assumed that one node sends the broadcast and
// the other nodes receive it.  Therefore, they can be used for split or symmetric
// mode.
// IMPORTANT:
//  (1) the specified VC must be dedicated to these functions during their operation
//  (2) the irq bit must not be set in any of the headers
//  (3) the sending node will also receive broadcasts, but will discard them.

int BGLTreeSendBcast(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Must be qword-aligned.  Len mod 256 must be 0.
       int                     bytes );       // Total length in bytes

int BGLTreeRecvBcast(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdr,          // Header, for verification.  Skip, if NULL.
       void                    *operandBuf,   // Must be qword-aligned.  Len mod 256 must be 0.
       int                     bytes );       // Total length in bytes.  Will recv this much.


int BGLTreeSendBcastIovec(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdrs,         // Previously created array of collective headers to use.
       iovec_g                   *data_vec,     // Base must be qword-aligned.  Len mod 256 must be 0.
       int                     vec_count );   // Count of headers and io vectors.

int BGLTreeRecvBcastIovec(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdrs,         // Headers, for verification.  Skip, if NULL.
       iovec_g                   *data_vec,     // Base must be qword-aligned.  Len mod 256 must be 0.
       int                     vec_count );   // Count of headers and io vectors.  Will recv this much.

// Variants for use with unaligned data or operand arrays that are not a multiple of 256 bytes
int BGLTreeSendBcastUnaligned(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Operand array.
       int                     bytes );       // Total length in bytes

int BGLTreeRecvBcastUnaligned(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdr,          // Header, for verification.  Skip, if NULL.
       void                    *operandBuf,   // Bcast target.
       int                     bytes );       // Total length in bytes.  Will recv this much.

int BGLTreeSendBcastIovecUnaligned(
       int                     vc,            // Virtual channel to use.
       _BGL_TreeCollectiveHdr  *hdrs,         // Previously created array of collective headers to use.
       iovec_g                   *data_vec,     // Operand arrays.
       int                     vec_count );   // Count of headers and io vectors.

int BGLTreeRecvBcastIovecUnaligned(
       int                     vc,            // Virtual channel to poll.
       _BGL_TreeCollectiveHdr  *hdrs,         // Headers, for verification.  Skip, if NULL.
       iovec_g                   *data_vec,     // Bcast targets.
       int                     vec_count );   // Count of headers and io vectors.  Will recv this many.

// NOTE: Could add really generic functions with the gather-scatter list here...

/*---------------------------------------------------------------------------*
 *     Balanced Mode Tree Broadcast Interface                                *
 *---------------------------------------------------------------------------*/

// Uses the I/O processor to send and receive.
// IMPORTANT:
//  (1) The virtual channel defined by BALANCED_MODE_APP_VC will be used
//  (2) The irq bit must not be set in any of the headers
//  (3) *operands must be flushed from L1

// Blocking broadcast called by all nodes with rank of sending node.  Sending nodes's buffer contains source
// data.  Upon completion, all nodes have the data.
void BGLTreeBcastUnaligned(
       int  sender,   // rank of sending node
       int  vt,       // Class (virtual tree)
       int  vc,       // Virtual channel
       void *data,    // Data to send
       int  bytes );  // length in bytes

int BGLTreeStartBcast(
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Must be qword-aligned.  Len mod 256 must be 0.
       int                     bytes );       // Total length in bytes

// Variant for use with unaligned data or an operand array that is not a multiple of 256 bytes
int BGLTreeStartBcastUnaligned(
       _BGL_TreeCollectiveHdr  *hdr,          // Previously created collective header to use.
       void                    *operands,     // Contiguous array of operands.
       int                     bytes );       // Operand array size in bytes.

// Must be called by senders as well as receivers!  Waits for recv_count broadcast sends
// or receives to complete, or all if recv_count is -1.  Returns the number
// completed in recv_count.
int BGLTreeWaitforBcast( int *recv_count );

// NOTE: Could add a gather-scatter start function too...
// Note 2: Perhaps the core calling Start should inject, avoiding the flush.

__END_DECLS

#endif // __ASSEMBLY__

#endif // Add nothing below this line.
