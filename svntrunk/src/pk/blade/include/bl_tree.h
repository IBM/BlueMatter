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
//      File: blade/include/sys/machine/bl_tree.h
//
//   Purpose: Define BG/L's Global Tree Memory and DCR maps.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//            Matthias Blumrich (blumrich@us.ibm.com)
//
//     Notes:
//
//   History: 05/01/2002: MEG - Created.
//
#ifndef _BL_TREE_H // Prevent multiple inclusion
#define _BL_TREE_H


#define TRx_DI      (0x00)    // Offset from Tree VCx for Data   Injection   (WO,Quad)
#define TRx_HI      (0x10)    // Offset from Tree VCx for Header Injection   (WO,Word)
#define TRx_DR      (0x20)    // Offset from Tree VCx for Data   Reception   (RO,Quad)
#define TRx_HR      (0x30)    // Offset from Tree VCx for Header Reception   (RO,Word)
#define TRx_Sx      (0x40)    // Offset from Tree VCx for Status             (RO,Word)
#define TRx_SO      (0x50)    // Offset from Tree VCx for Status of Other VC (RO,Word)

// Physical Addresses for Tree VC0 (P=V to use these addresses)
#define TR0_DI    (PA_TREE_VC0 | TRx_DI)
#define TR0_HI    (PA_TREE_VC0 | TRx_HI)
#define TR0_DR    (PA_TREE_VC0 | TRx_DR)
#define TR0_HR    (PA_TREE_VC0 | TRx_HR)
#define TR0_S0    (PA_TREE_VC0 | TRx_Sx)
#define TR0_S1    (PA_TREE_VC0 | TRx_SO)

// Physical Addresses for Tree VC1 (P=V to use these addresses)
#define TR1_DI    (PA_TREE_VC1 | TRx_DI)
#define TR1_HI    (PA_TREE_VC1 | TRx_HI)
#define TR1_DR    (PA_TREE_VC1 | TRx_DR)
#define TR1_HR    (PA_TREE_VC1 | TRx_HR)
#define TR1_S1    (PA_TREE_VC1 | TRx_Sx)
#define TR1_S0    (PA_TREE_VC1 | TRx_SO)

// Packet Payload: fixed size for all Tree packets
#define _TR_PKT_BYTES    (256)       // bytes in a tr packet
#define _TR_PKT_QUADS     (16)       // quads in a tr packet

// Packet header
#define  _TR_HDR_CLASS(x)           _B4( 3,x)              //  Packet class (virtual tree)
#define  _TR_HDR_P2P                _BN( 4)                //  Point-to-point enable
#define  _TR_HDR_IRQ                _BN( 5)                //  Interrupt request (at receiver) enable
#define  _TR_HDR_OPCODE(x)          _B3( 8,x)              //  ALU opcode
#define    _TR_OP_NONE                0x0                  //    No operand.  Use for ordinary routed packets.
#define    _TR_OP_OR                  0x1                  //    Bitwise logical OR.
#define    _TR_OP_AND                 0x2                  //    Bitwise logical AND.
#define    _TR_OP_XOR                 0x3                  //    Bitwise logical XOR.
#define    _TR_OP_MAX                 0x5                  //    Unsigned integer maximum.
#define    _TR_OP_ADD                 0x6                  //    Unsigned integer addition.
#define  _TR_HDR_OPSIZE(x)          _B7(15,x)              //  Operand size (# of 16-bit words minus 1)
#define  _TR_HDR_TAG(x)             _B14(29,x)             //  User-specified tag (for ordinary routed packets only)
#define  _TR_HDR_NADDR(x)           _B24(29,x)             //  Target address (for P2P packets only)
#define  _TR_HDR_CSUM(x)            _B2(31,x)              //  Injection checksum mode
#define    _TR_CSUM_NONE              0x0                  //    Do not include packet in checksums.
#define    _TR_CSUM_SOME              0x1                  //    Include header in header checksum.  Include all but
                                                           //     first quadword in payload checksum.
#define    _TR_CSUM_CFG               0x2                  //    Include header in header checksum.  Include all but
                                                           //     specified number of 16-bit words in payload checksum.
#define    _TR_CSUM_ALL               0x3                  //    Include entire packet in checksums.

// Packet status
#define  _TR_STAT_IPY_CNT(x)        _B8( 7,x)              //  Injection payload qword count
#define  _TR_STAT_IHD_CNT(x)        _B4(15,x)              //  Injection header word count
#define  _TR_STAT_RPY_CNT(x)        _B8(23,x)              //  Reception payload qword count
#define  _TR_STAT_IRQ               _BN(27)                //  One or more reception headers with IRQ bit set
#define  _TR_STAT_RHD_CNT(x)        _B4(31,x)              //  Reception header word count

// Tree Map of DCR Groupings: (see bl_dcrmap.h for DCR_TREE)
#define DCR_TR_CLASS  (DCR_TREE + 0x00)      // Class Definition Registers (R/W)
#define DCR_TR_DMA    (DCR_TREE + 0x0C)      // Network Port Diagnostic Memory Access Registers (R/W)
#define DCR_TR_ARB    (DCR_TREE + 0x10)      // Arbiter Control Registers (R/W)
#define DCR_TR_CH0    (DCR_TREE + 0x20)      // Channel 0 Control Registers (R/W)
#define DCR_TR_CH1    (DCR_TREE + 0x28)      // Channel 1 Control Registers (R/W)
#define DCR_TR_CH2    (DCR_TREE + 0x30)      // Channel 2 Control Registers (R/W)
#define DCR_TR_GLOB   (DCR_TREE + 0x40)      // Global Registers (R/W)
#define DCR_TR_REC    (DCR_TREE + 0x44)      // Processor Reception Registers (R/W)
#define DCR_TR_INJ    (DCR_TREE + 0x48)      // Processor Injection Registers (R/W)
#define DCR_TR_LCRC   (DCR_TREE + 0x50)      // Link CRC's


// Tree Class Registers
// Note: each route descriptor register contains two class descriptors.  "LO" will refer to the lower-numbered
//       of the two and "HI" will refer to the higher numbered.
#define DCR_TR_CLASS_RDR0           (DCR_TR_CLASS + 0x00)  // CLASS: Route Descriptor Register for classes 0,  1
#define DCR_TR_CLASS_RDR1           (DCR_TR_CLASS + 0x01)  // CLASS: Route Descriptor Register for classes 2,  3
#define DCR_TR_CLASS_RDR2           (DCR_TR_CLASS + 0x02)  // CLASS: Route Descriptor Register for classes 4,  5
#define DCR_TR_CLASS_RDR3           (DCR_TR_CLASS + 0x03)  // CLASS: Route Descriptor Register for classes 6,  7
#define DCR_TR_CLASS_RDR4           (DCR_TR_CLASS + 0x04)  // CLASS: Route Descriptor Register for classes 8,  9
#define DCR_TR_CLASS_RDR5           (DCR_TR_CLASS + 0x05)  // CLASS: Route Descriptor Register for classes 10, 11
#define DCR_TR_CLASS_RDR6           (DCR_TR_CLASS + 0x06)  // CLASS: Route Descriptor Register for classes 12, 13
#define DCR_TR_CLASS_RDR7           (DCR_TR_CLASS + 0x07)  // CLASS: Route Descriptor Register for classes 14, 15
#define  _TR_CLASS_RDR_LO_SRC2      _BN( 1)                //  Class low,  source channel 2
#define  _TR_CLASS_RDR_LO_SRC1      _BN( 2)                //  Class low,  source channel 1
#define  _TR_CLASS_RDR_LO_SRC0      _BN( 3)                //  Class low,  source channel 0
#define  _TR_CLASS_RDR_LO_TGT2      _BN( 5)                //  Class low,  target channel 2
#define  _TR_CLASS_RDR_LO_TGT1      _BN( 6)                //  Class low,  target channel 1
#define  _TR_CLASS_RDR_LO_TGT0      _BN( 7)                //  Class low,  target channel 0
#define  _TR_CLASS_RDR_LO_SRCL      _BN(14)                //  Class low,  source local client (injection)
#define  _TR_CLASS_RDR_LO_TGTL      _BN(15)                //  Class low,  target local client (reception)
#define  _TR_CLASS_RDR_HI_SRC2      _BN(17)                //  Class high, source channel 2
#define  _TR_CLASS_RDR_HI_SRC1      _BN(18)                //  Class high, source channel 1
#define  _TR_CLASS_RDR_HI_SRC0      _BN(19)                //  Class high, source channel 0
#define  _TR_CLASS_RDR_HI_TGT2      _BN(21)                //  Class high, target channel 2
#define  _TR_CLASS_RDR_HI_TGT1      _BN(22)                //  Class high, target channel 1
#define  _TR_CLASS_RDR_HI_TGT0      _BN(23)                //  Class high, target channel 0
#define  _TR_CLASS_RDR_HI_SRCL      _BN(30)                //  Class high, source local client (injection)
#define  _TR_CLASS_RDR_HI_TGTL      _BN(31)                //  Class high, target local client (reception)
#define DCR_TR_CLASS_ISRA           (DCR_TR_CLASS + 0x08)  // CLASS: Bits 0-31 of 64-bit idle pattern
#define DCR_TR_CLASS_ISRB           (DCR_TR_CLASS + 0x09)  // CLASS: Bits 32-63 of 64-bit idle pattern

// Tree Network Port Diagnostic Memory Access Registers
// Note: Diagnostic access to processor injection and reception fifos is through TR_REC and TR_INJ registers.
#define DCR_TR_DMA_DMAA             (DCR_TR_DMA + 0x00)    // DMA: Diagnostic SRAM address
#define  _TR_DMA_DMAA_TGT(x)        _B3(21,x)              //  Target
#define   _TR_DMAA_TGT_RCV0           0x0                  //   Channel 0 receiver
#define   _TR_DMAA_TGT_RCV1           0x1                  //   Channel 1 receiver
#define   _TR_DMAA_TGT_RCV2           0x2                  //   Channel 2 receiver
#define   _TR_DMAA_TGT_SND0           0x4                  //   Channel 0 sender
#define   _TR_DMAA_TGT_SND1           0x5                  //   Channel 1 sender
#define   _TR_DMAA_TGT_SND2           0x6                  //   Channel 2 sender
#define  _TR_DMA_DMAA_VC(x)         _B1(22,x)              //  Virtual channel
#define  _TR_DMA_DMAA_PCKT(x)       _B2(24,x)              //  Packet number
#define  _TR_DMA_DMAA_WORD(x)       _B7(31,x)              //  Word offset within packet
#define DCR_TR_DMA_DMAD             (DCR_TR_DMA + 0x01)    // DMA: Diagnostic SRAM data
#define DCR_TR_DMA_DMADI            (DCR_TR_DMA + 0x02)    // DMA: Diagnostic SRAM data with address increment
#define  _TR_DMA_DMAD_ECC(x)        _B6(15,x)              //  ECC
#define  _TR_DMA_DMAD_DATA(x)       _B16(31,x)             //  Data
#define DCR_TR_DMA_DMAH             (DCR_TR_DMA + 0x03)    // DMA: Diagnostic header access

// Tree Arbiter Control Registers
#define DCR_TR_ARB_RCFG             (DCR_TR_ARB + 0x00)    // ARB: General router configuration
#define  _TR_ARB_RCFG_SRC00         _BN( 0)                //  Disable source channel 0, VC0
#define  _TR_ARB_RCFG_SRC01         _BN( 1)                //  Disable source channel 0, VC1
#define  _TR_ARB_RCFG_TGT00         _BN( 2)                //  Disable target channel 0, VC0
#define  _TR_ARB_RCFG_TGT01         _BN( 3)                //  Disable target channel 0, VC1
#define  _TR_ARB_RCFG_SRC10         _BN( 4)                //  Disable source channel 1, VC0
#define  _TR_ARB_RCFG_SRC11         _BN( 5)                //  Disable source channel 1, VC1
#define  _TR_ARB_RCFG_TGT10         _BN( 6)                //  Disable target channel 1, VC0
#define  _TR_ARB_RCFG_TGT11         _BN( 7)                //  Disable target channel 1, VC1
#define  _TR_ARB_RCFG_SRC20         _BN( 8)                //  Disable source channel 2, VC0
#define  _TR_ARB_RCFG_SRC21         _BN( 9)                //  Disable source channel 2, VC1
#define  _TR_ARB_RCFG_TGT20         _BN(10)                //  Disable target channel 2, VC0
#define  _TR_ARB_RCFG_TGT21         _BN(11)                //  Disable target channel 2, VC1
#define  _TR_ARB_RCFG_LB2           _BN(25)                //  Channel 2 loopback enable
#define  _TR_ARB_RCFG_LB1           _BN(26)                //  Channel 1 loopback enable
#define  _TR_ARB_RCFG_LB0           _BN(27)                //  Channel 0 loopback enable
#define  _TR_ARB_RCFG_TOM(x)        _B2(29,x)              //  Timeout mode
#define   _TR_RCFG_TOM_NONE           0x0                  //   Disable.
#define   _TR_RCFG_TOM_NRML           0x1                  //   Normal mode, irq enabled.
#define   _TR_RCFG_TOM_WD             0x2                  //   Watchdog mode, irq enabled.
#define  _TR_ARB_RCFG_MAN           _BN(30)                //  Manual mode (router is disabled).
#define  _TR_ARB_RCFG_RST           _BN(31)                //  Full arbiter reset.
#define DCR_TR_ARB_RTO              (DCR_TR_ARB + 0x01)    // ARB: 32 MSBs of router timeout value
#define DCR_TR_ARB_RTIME            (DCR_TR_ARB + 0x02)    // ARB: Value of router timeout counter
#define DCR_TR_ARB_RSTAT            (DCR_TR_ARB + 0x03)    // ARB: General router status
#define  _TR_ARB_RSTAT_REQ20        _BN( 0)                //  Packet available in channel 2, VC0
#define  _TR_ARB_RSTAT_REQ10        _BN( 1)                //  Packet available in channel 1, VC0
#define  _TR_ARB_RSTAT_REQ00        _BN( 2)                //  Packet available in channel 0, VC0
#define  _TR_ARB_RSTAT_REQP0        _BN( 3)                //  Packet available in local client, VC0
#define  _TR_ARB_RSTAT_REQ21        _BN( 4)                //  Packet available in channel 2, VC1
#define  _TR_ARB_RSTAT_REQ11        _BN( 5)                //  Packet available in channel 1, VC1
#define  _TR_ARB_RSTAT_REQ01        _BN( 6)                //  Packet available in channel 0, VC1
#define  _TR_ARB_RSTAT_REQP1        _BN( 7)                //  Packet available in local client, VC1
#define  _TR_ARB_RSTAT_FUL20        _BN( 8)                //  Channel 2, VC0 is full
#define  _TR_ARB_RSTAT_FUL10        _BN( 9)                //  Channel 1, VC0 is full
#define  _TR_ARB_RSTAT_FUL00        _BN(10)                //  Channel 0, VC0 is full
#define  _TR_ARB_RSTAT_FULP0        _BN(11)                //  Local client, VC0 is full
#define  _TR_ARB_RSTAT_FUL21        _BN(12)                //  Channel 2, VC1 is full
#define  _TR_ARB_RSTAT_FUL11        _BN(13)                //  Channel 1, VC1 is full
#define  _TR_ARB_RSTAT_FUL01        _BN(14)                //  Channel 0, VC1 is full
#define  _TR_ARB_RSTAT_FULP1        _BN(15)                //  Local client, VC1 is full
#define  _TR_ARB_RSTAT_MAT20        _BN(16)                //  Channel 2, VC0 is mature
#define  _TR_ARB_RSTAT_MAT10        _BN(17)                //  Channel 1, VC0 is mature
#define  _TR_ARB_RSTAT_MAT00        _BN(18)                //  Channel 0, VC0 is mature
#define  _TR_ARB_RSTAT_MATP0        _BN(19)                //  Local client, VC0 is mature
#define  _TR_ARB_RSTAT_MAT21        _BN(20)                //  Channel 2, VC1 is mature
#define  _TR_ARB_RSTAT_MAT11        _BN(21)                //  Channel 1, VC1 is mature
#define  _TR_ARB_RSTAT_MAT01        _BN(22)                //  Channel 0, VC1 is mature
#define  _TR_ARB_RSTAT_MATP1        _BN(23)                //  Local client, VC1 is mature
#define  _TR_ARB_RSTAT_BSY20        _BN(24)                //  Channel 2, VC0 is busy
#define  _TR_ARB_RSTAT_BSY10        _BN(25)                //  Channel 1, VC0 is busy
#define  _TR_ARB_RSTAT_BSY00        _BN(26)                //  Channel 0, VC0 is busy
#define  _TR_ARB_RSTAT_BSYP0        _BN(27)                //  Local client, VC0 is busy
#define  _TR_ARB_RSTAT_BSY21        _BN(28)                //  Channel 2, VC1 is busy
#define  _TR_ARB_RSTAT_BSY11        _BN(29)                //  Channel 1, VC1 is busy
#define  _TR_ARB_RSTAT_BSY01        _BN(30)                //  Channel 0, VC1 is busy
#define  _TR_ARB_RSTAT_BSYP1        _BN(31)                //  Local client, VC1 is busy
#define DCR_TR_ARB_HD00             (DCR_TR_ARB + 0x04)    // ARB: Next header, channel 0, VC0
#define DCR_TR_ARB_HD01             (DCR_TR_ARB + 0x05)    // ARB: Next header, channel 0, VC1
#define DCR_TR_ARB_HD10             (DCR_TR_ARB + 0x06)    // ARB: Next header, channel 1, VC0
#define DCR_TR_ARB_HD11             (DCR_TR_ARB + 0x07)    // ARB: Next header, channel 1, VC1
#define DCR_TR_ARB_HD20             (DCR_TR_ARB + 0x08)    // ARB: Next header, channel 2, VC0
#define DCR_TR_ARB_HD21             (DCR_TR_ARB + 0x09)    // ARB: Next header, channel 2, VC1
#define DCR_TR_ARB_HDI0             (DCR_TR_ARB + 0x0A)    // ARB: Next header, injection, VC0
#define DCR_TR_ARB_HDI1             (DCR_TR_ARB + 0x0B)    // ARB: Next header, injection, VC1
#define DCR_TR_ARB_FORCEC           (DCR_TR_ARB + 0x0C)    // ARB: Force control for manual mode
#define  _TR_ARB_FORCEC_CH0         _BN( 0)                //  Channel 0 is a target
#define  _TR_ARB_FORCEC_CH1         _BN( 1)                //  Channel 1 is a target
#define  _TR_ARB_FORCEC_CH2         _BN( 2)                //  Channel 2 is a target
#define  _TR_ARB_FORCEC_P           _BN( 3)                //  Local client is a target
#define  _TR_ARB_FORCEC_ALU         _BN( 4)                //  ALU is a target
#define  _TR_ARB_FORCEC_RT          _BN( 5)                //  Force route immediately
#define  _TR_ARB_FORCEC_STK         _BN( 6)                //  Sticky route: always force route
#define DCR_TR_ARB_FORCER           (DCR_TR_ARB + 0x0D)    // ARB: Forced route for manual mode
#define  _TR_ARB_FORCER_CH20        _BN( 0)                //  Channel 2 is a source for channel 0
#define  _TR_ARB_FORCER_CH10        _BN( 1)                //  Channel 1 is a source for channel 0
#define  _TR_ARB_FORCER_CH00        _BN( 2)                //  Channel 0 is a source for channel 0
#define  _TR_ARB_FORCER_CHP0        _BN( 3)                //  Local client is a source for channel 0
#define  _TR_ARB_FORCER_CHA0        _BN( 4)                //  ALU is a source for channel 0
#define  _TR_ARB_FORCER_VC0         _BN( 5)                //  VC that is source for channel 0
#define  _TR_ARB_FORCER_CH21        _BN( 6)                //  Channel 2 is a source for channel 1
#define  _TR_ARB_FORCER_CH11        _BN( 7)                //  Channel 1 is a source for channel 1
#define  _TR_ARB_FORCER_CH01        _BN( 8)                //  Channel 0 is a source for channel 1
#define  _TR_ARB_FORCER_CHP1        _BN( 9)                //  Local client is a source for channel 1
#define  _TR_ARB_FORCER_CHA1        _BN(10)                //  ALU is a source for channel 1
#define  _TR_ARB_FORCER_VC1         _BN(11)                //  VC that is source for channel 1
#define  _TR_ARB_FORCER_CH22        _BN(12)                //  Channel 2 is a source for channel 2
#define  _TR_ARB_FORCER_CH12        _BN(13)                //  Channel 1 is a source for channel 2
#define  _TR_ARB_FORCER_CH02        _BN(14)                //  Channel 0 is a source for channel 2
#define  _TR_ARB_FORCER_CHP2        _BN(15)                //  Local client is a source for channel 2
#define  _TR_ARB_FORCER_CHA2        _BN(16)                //  ALU is a source for channel 2
#define  _TR_ARB_FORCER_VC2         _BN(17)                //  VC that is source for channel 2
#define  _TR_ARB_FORCER_CH2P        _BN(18)                //  Channel 2 is a source for local client
#define  _TR_ARB_FORCER_CH1P        _BN(19)                //  Channel 1 is a source for local client
#define  _TR_ARB_FORCER_CH0P        _BN(20)                //  Channel 0 is a source for local client
#define  _TR_ARB_FORCER_CHPP        _BN(21)                //  Local client is a source for local client
#define  _TR_ARB_FORCER_CHAP        _BN(22)                //  ALU is a source for local client
#define  _TR_ARB_FORCER_VCP         _BN(23)                //  VC that is source for local client
#define  _TR_ARB_FORCER_CH2A        _BN(24)                //  Channel 2 is a source for ALU
#define  _TR_ARB_FORCER_CH1A        _BN(25)                //  Channel 1 is a source for ALU
#define  _TR_ARB_FORCER_CH0A        _BN(26)                //  Channel 0 is a source for ALU
#define  _TR_ARB_FORCER_CHPA        _BN(27)                //  Local client is a source for ALU
#define  _TR_ARB_FORCER_CHAA        _BN(28)                //  ALU is a source for ALU
#define  _TR_ARB_FORCER_VCA         _BN(29)                //  VC that is source for ALU
#define DCR_TR_ARB_FORCEH           (DCR_TR_ARB + 0x0E)    // ARB: Forced header for manual mode
#define DCR_TR_ARB_XSTAT            (DCR_TR_ARB + 0x0F)    // ARB: Extended router status
#define  _TR_ARB_XSTAT_BLK20        _BN( 0)                //  Request from channel 2, VC0 is blocked
#define  _TR_ARB_XSTAT_BLK10        _BN( 1)                //  Request from channel 1, VC0 is blocked
#define  _TR_ARB_XSTAT_BLK00        _BN( 2)                //  Request from channel 0, VC0 is blocked
#define  _TR_ARB_XSTAT_BLKP0        _BN( 3)                //  Request from local client, VC0 is blocked
#define  _TR_ARB_XSTAT_BLK21        _BN( 4)                //  Request from channel 2, VC1 is blocked
#define  _TR_ARB_XSTAT_BLK11        _BN( 5)                //  Request from channel 1, VC1 is blocked
#define  _TR_ARB_XSTAT_BLK01        _BN( 6)                //  Request from channel 0, VC1 is blocked
#define  _TR_ARB_XSTAT_BLKP1        _BN( 7)                //  Request from local client, VC1 is blocked
#define  _TR_ARB_XSTAT_BSYR2        _BN( 8)                //  Channel 2 receiver is busy
#define  _TR_ARB_XSTAT_BSYR1        _BN( 9)                //  Channel 1 receiver is busy
#define  _TR_ARB_XSTAT_BSYR0        _BN(10)                //  Channel 0 receiver is busy
#define  _TR_ARB_XSTAT_BSYPI        _BN(11)                //  Local client injection is busy
#define  _TR_ARB_XSTAT_BSYA         _BN(12)                //  ALU is busy
#define  _TR_ARB_XSTAT_BSYS2        _BN(13)                //  Channel 2 sender is busy
#define  _TR_ARB_XSTAT_BSYS1        _BN(14)                //  Channel 1 sender is busy
#define  _TR_ARB_XSTAT_BSYS0        _BN(15)                //  Channel 0 sender is busy
#define  _TR_ARB_XSTAT_BSYPR        _BN(16)                //  Local client reception is busy
#define  _TR_ARB_XSTAT_ARB_TO(x)    _B15(31,x)             //  Greedy-Arbitration timeout

// Tree Channel 0 Control Registers
#define DCR_TR_CH0_RSTAT            (DCR_TR_CH0 + 0x00)    // CH0: Receiver status
#define  _TR_RSTAT_RCVERR           _BN( 0)                //  Receiver error
#define  _TR_RSTAT_LHEXP            _BN( 1)                //  Expect link header
#define  _TR_RSTAT_PH0EXP           _BN( 2)                //  Expect packet header 0
#define  _TR_RSTAT_PH1EXP           _BN( 3)                //  Expect packet header 1
#define  _TR_RSTAT_PDRCV            _BN( 4)                //  Receive packet data
#define  _TR_RSTAT_CWEXP            _BN( 5)                //  Expect packet control word
#define  _TR_RSTAT_CSEXP            _BN( 6)                //  Expect packet checksum
#define  _TR_RSTAT_SCRBRD0          _B8(14,0xff)           //  VC0 fifo scoreboard
#define  _TR_RSTAT_SCRBRD1          _B8(22,0xff)           //  VC1 fifo scoreboard
#define  _TR_RSTAT_RMTSTAT          _B9(31,0x1ff)          //  Remote status
#define DCR_TR_CH0_RCTRL            (DCR_TR_CH0 + 0x01)    // CH0: Receiver control
#define  _TR_RCTRL_FERR             _BN( 0)                //  Force receiver into error state
#define  _TR_RCTRL_RST              _BN( 1)                //  Reset all internal pointers
#define  _TR_RCTRL_FRZ0             _BN( 2)                //  Freeze VC0
#define  _TR_RCTRL_FRZ1             _BN( 3)                //  Freeze VC1
#define  _TR_RCTRL_RCVALL           _BN( 4)                //  Disable receiver CRC check and accept all packets
#define DCR_TR_CH0_SSTAT            (DCR_TR_CH0 + 0x02)    // CH0: Sender status
#define  _TR_SSTAT_SYNC             _BN( 0)                //  Phase of sender
#define  _TR_SSTAT_ARB              _BN( 1)                //  Arbitrating
#define  _TR_SSTAT_PH0SND           _BN( 2)                //  Sending packet header 0
#define  _TR_SSTAT_PH1SND           _BN( 3)                //  Sending packet header 1
#define  _TR_SSTAT_PDSND            _BN( 4)                //  Sending packet payload
#define  _TR_SSTAT_CWSND            _BN( 5)                //  Sending packet control word
#define  _TR_SSTAT_CSSND            _BN( 6)                //  Sending packet checksum
#define  _TR_SSTAT_IDLSND           _BN( 7)                //  Sending idle packet
#define  _TR_SSTAT_RPTR0            _B3(10,0x7)            //  VC0 read pointer
#define  _TR_SSTAT_WPTR0            _B3(13,0x7)            //  VC0 write pointer
#define  _TR_SSTAT_RPTR1            _B3(16,0x7)            //  VC1 read pointer
#define  _TR_SSTAT_WPTR1            _B3(19,0x7)            //  VC1 write pointer
#define DCR_TR_CH0_SCTRL            (DCR_TR_CH0 + 0x03)    // CH0: Sender control
#define  _TR_SCTRL_SYNC             _BN( 0)                //  Force sender to send SYNC
#define  _TR_SCTRL_IDLE             _BN( 1)                //  Force sender to send IDLE
#define  _TR_SCTRL_RST              _BN( 2)                //  Reset all internal pointers
#define  _TR_SCTRL_INVMSB           _BN( 3)                //  Invert MSB of class for loopback packets
#define  _TR_SCTRL_OFF              _BN( 4)                //  Disable (black hole) the sender
#define DCR_TR_CH0_TNACK            (DCR_TR_CH0 + 0x04)    // CH0: Tolerated dalay from NACK to ACK status
#define DCR_TR_CH0_CNACK            (DCR_TR_CH0 + 0x05)    // CH0: Time since last NACK received
#define DCR_TR_CH0_TIDLE            (DCR_TR_CH0 + 0x06)    // CH0: Frequency to send IDLE packets
#define DCR_TR_CH0_CIDLE            (DCR_TR_CH0 + 0x07)    // CH0: Time since last IDLE sent

// Tree Channel 1 Control Registers
// Note: Register definitions are the same as those of channel 0.
#define DCR_TR_CH1_RSTAT            (DCR_TR_CH1 + 0x00)    // CH1: Receiver status
#define DCR_TR_CH1_RCTRL            (DCR_TR_CH1 + 0x01)    // CH1: Receiver control
#define DCR_TR_CH1_SSTAT            (DCR_TR_CH1 + 0x02)    // CH1: Sender status
#define DCR_TR_CH1_SCTRL            (DCR_TR_CH1 + 0x03)    // CH1: Sender control
#define DCR_TR_CH1_TNACK            (DCR_TR_CH1 + 0x04)    // CH1: Tolerated dalay from NACK to ACK status
#define DCR_TR_CH1_CNACK            (DCR_TR_CH1 + 0x05)    // CH1: Time since last NACK received
#define DCR_TR_CH1_TIDLE            (DCR_TR_CH1 + 0x06)    // CH1: Frequency to send IDLE packets
#define DCR_TR_CH1_CIDLE            (DCR_TR_CH1 + 0x07)    // CH1: Time since last IDLE sent

// Tree Channel 2 Control Registers
// Note: Register definitions are the same as those of channel 0.
#define DCR_TR_CH2_RSTAT            (DCR_TR_CH2 + 0x00)    // CH2: Receiver status
#define DCR_TR_CH2_RCTRL            (DCR_TR_CH2 + 0x01)    // CH2: Receiver control
#define DCR_TR_CH2_SSTAT            (DCR_TR_CH2 + 0x02)    // CH2: Sender status
#define DCR_TR_CH2_SCTRL            (DCR_TR_CH2 + 0x03)    // CH2: Sender control
#define DCR_TR_CH2_TNACK            (DCR_TR_CH2 + 0x04)    // CH2: Tolerated dalay from NACK to ACK status
#define DCR_TR_CH2_CNACK            (DCR_TR_CH2 + 0x05)    // CH2: Time since last NACK received
#define DCR_TR_CH2_TIDLE            (DCR_TR_CH2 + 0x06)    // CH2: Frequency to send IDLE packets
#define DCR_TR_CH2_CIDLE            (DCR_TR_CH2 + 0x07)    // CH2: Time since last IDLE sent

// Tree Global Registers
#define DCR_TR_GLOB_FPTR            (DCR_TR_GLOB + 0x00)   // GLOB: Fifo Pointer Register
#define  _TR_GLOB_FPTR_IPY0(x)      _B3( 3,x)              //  VC0 injection payload FIFO packet write pointer
#define  _TR_GLOB_FPTR_IHD0(x)      _B3( 7,x)              //  VC0 injection header  FIFO packet write pointer
#define  _TR_GLOB_FPTR_IPY1(x)      _B3(11,x)              //  VC1 injection payload FIFO packet write pointer
#define  _TR_GLOB_FPTR_IHD1(x)      _B3(15,x)              //  VC1 injection header  FIFO packet write pointer
#define  _TR_GLOB_FPTR_RPY0(x)      _B3(19,x)              //  VC0 reception payload FIFO packet read  pointer
#define  _TR_GLOB_FPTR_RHD0(x)      _B3(23,x)              //  VC0 reception header  FIFO packet read  pointer
#define  _TR_GLOB_FPTR_RPY1(x)      _B3(27,x)              //  VC1 reception payload FIFO packet read  pointer
#define  _TR_GLOB_FPTR_RHD1(x)      _B3(31,x)              //  VC1 reception header  FIFO packet read  pointer
#define DCR_TR_GLOB_NADDR           (DCR_TR_GLOB + 0x01)   // GLOB: Node Address Register
#define  _TR_GLOB_NADDR(x)          _B24(31,x)             //  Node address
#define DCR_TR_GLOB_VCFG0           (DCR_TR_GLOB + 0x02)   // GLOB: VC0 Configuration Register (use macros below)
#define DCR_TR_GLOB_VCFG1           (DCR_TR_GLOB + 0x03)   // GLOB: VC1 Configuration Register
#define  _TR_GLOB_VCFG_RCVALL       _BN( 0)                //  Disable P2P reception filering
#define  _TR_GLOB_VCFG_CSUMX(x)     _B8(15,x)              //  Injection checksum mode 2 exclusion
#define  _TR_GLOB_VCFG_RWM(x)       _B3(23,x)              //  Payload reception FIFO watermark
#define  _TR_GLOB_VCFG_IWM(x)       _B3(31,x)              //  Payload injection FIFO watermark

// Tree Processor Reception Registers
#define DCR_TR_REC_PRXF             (DCR_TR_REC + 0x00)    // REC: Receive Exception Flag Register
#define DCR_TR_REC_PRXEN            (DCR_TR_REC + 0x01)    // REC: Receive Exception Enable Register
#define  _TR_REC_PRX_APAR0          _BN( 8)                //  P0 address parity error
#define  _TR_REC_PRX_APAR1          _BN( 9)                //  P1 address parity error
#define  _TR_REC_PRX_ALIGN0         _BN(10)                //  P0 address alignment error
#define  _TR_REC_PRX_ALIGN1         _BN(11)                //  P1 address alignment error
#define  _TR_REC_PRX_ADDR0          _BN(12)                //  P0 bad (unrecognized) address error
#define  _TR_REC_PRX_ADDR1          _BN(13)                //  P1 bad (unrecognized) address error
#define  _TR_REC_PRX_COLL           _BN(14)                //  FIFO read collision error
#define  _TR_REC_PRX_UE             _BN(15)                //  Uncorrectable SRAM ECC error
#define  _TR_REC_PRX_PFU0           _BN(26)                //  VC0 payload FIFO under-run error
#define  _TR_REC_PRX_PFU1           _BN(27)                //  VC1 payload FIFO under-run error
#define  _TR_REC_PRX_HFU0           _BN(28)                //  VC0 header FIFO under-run error
#define  _TR_REC_PRX_HFU1           _BN(29)                //  VC1 header FIFO under-run error
#define  _TR_REC_PRX_WM0            _BN(30)                //  VC0 payload FIFO above watermark
#define  _TR_REC_PRX_WM1            _BN(31)                //  VC1 payload FIFO above watermark
#define DCR_TR_REC_PRDA             (DCR_TR_REC + 0x02)    // REC: Receive Diagnostic Address Register
#define  _TR_PRDA_VC(x)             _B1(21,x)              //  Select VC to access
#define  _TR_PRDA_MAC(x)            _B1(22,x)              //  Select SRAM macro to access
#define  _TR_PRDA_LINE(x)           _B7(29,x)              //  Select line in SRAM or RA
#define  _TR_PRDA_TGT(x)            _B2(31,x)              //  Select target sub-line or RA
#define   _TR_PRDA_TGT_LO             0x0                  //   Least significant word of SRAM
#define   _TR_PRDA_TGT_HI             0x1                  //   Most significant word of SRAM
#define   _TR_PRDA_TGT_ECC            0x2                  //   ECC syndrome of SRAM
#define   _TR_PRDA_TGT_HDR            0x3                  //   Header fifo
#define DCR_TR_REC_PRDD             (DCR_TR_REC + 0x03)    // REC: Receive Diagnostic Data Register
#define  _TR_PRDD_ECC(x)            _B8(31,x)              //  ECC
#define  _TR_PRDD_DATA(x)           x                      //  Data

// Tree Processor Injection Registers
#define DCR_TR_INJ_PIXF             (DCR_TR_INJ + 0x00)    // INJ: Injection Exception Flag Register
#define DCR_TR_INJ_PIXEN            (DCR_TR_INJ + 0x01)    // INJ: Injection Exception Enable Register
#define  _TR_INJ_PIX_APAR0          _BN( 6)                //  P0 address parity error
#define  _TR_INJ_PIX_APAR1          _BN( 7)                //  P1 address parity error
#define  _TR_INJ_PIX_ALIGN0         _BN( 8)                //  P0 address alignment error
#define  _TR_INJ_PIX_ALIGN1         _BN( 9)                //  P1 address alignment error
#define  _TR_INJ_PIX_ADDR0          _BN(10)                //  P0 bad (unrecognized) address error
#define  _TR_INJ_PIX_ADDR1          _BN(11)                //  P1 bad (unrecognized) address error
#define  _TR_INJ_PIX_DPAR0          _BN(12)                //  P0 data parity error
#define  _TR_INJ_PIX_DPAR1          _BN(13)                //  P1 data parity error
#define  _TR_INJ_PIX_COLL           _BN(14)                //  FIFO write collision error
#define  _TR_INJ_PIX_UE             _BN(15)                //  Uncorrectable SRAM ECC error
#define  _TR_INJ_PIX_PFO0           _BN(25)                //  VC0 payload FIFO overflow error
#define  _TR_INJ_PIX_PFO1           _BN(26)                //  VC1 payload FIFO overflow error
#define  _TR_INJ_PIX_HFO0           _BN(27)                //  VC0 header FIFO overflow error
#define  _TR_INJ_PIX_HFO1           _BN(28)                //  VC1 header FIFO overflow error
#define  _TR_INJ_PIX_WM0            _BN(29)                //  VC0 payload FIFO at or below watermark
#define  _TR_INJ_PIX_WM1            _BN(30)                //  VC1 payload FIFO at or below watermark
#define  _TR_INJ_PIX_ENABLE         _BN(31)                //  Injection interface enable (if enabled in PIXEN)
#define DCR_TR_INJ_PIDA             (DCR_TR_INJ + 0x02)    // INJ: Injection Diagnostic Address Register
//        Use _TR_PRDA_* defined above.
#define DCR_TR_INJ_PIDD             (DCR_TR_INJ + 0x03)    // INJ: Injection Diagnostic Data Register
//        Use _TR_PRDD_* defined above.
#define DCR_TR_INJ_CSPY0            (DCR_TR_INJ + 0x04)    // INJ: VC0 payload checksum
#define DCR_TR_INJ_CSHD0            (DCR_TR_INJ + 0x05)    // INJ: VC0 header checksum
#define DCR_TR_INJ_CSPY1            (DCR_TR_INJ + 0x06)    // INJ: VC1 payload checksum
#define DCR_TR_INJ_CSHD1            (DCR_TR_INJ + 0x07)    // INJ: VC1 header checksum


// Link CRC's for the receivers 0..2 (vc0,1)
#define DCR_TR_LCRC_R00  (DCR_TR_LCRC + 0)
#define DCR_TR_LCRC_R01  (DCR_TR_LCRC + 1)
#define DCR_TR_LCRC_R10  (DCR_TR_LCRC + 2)
#define DCR_TR_LCRC_R11  (DCR_TR_LCRC + 3)
#define DCR_TR_LCRC_R20  (DCR_TR_LCRC + 4)
#define DCR_TR_LCRC_R21  (DCR_TR_LCRC + 5)

// Link CRC'c for the senders 0..2 (vc0,1)
#define DCR_TR_LCRC_S00  (DCR_TR_LCRC + 8)
#define DCR_TR_LCRC_S01  (DCR_TR_LCRC + 9)
#define DCR_TR_LCRC_S10  (DCR_TR_LCRC + 10)
#define DCR_TR_LCRC_S11  (DCR_TR_LCRC + 11)
#define DCR_TR_LCRC_S20  (DCR_TR_LCRC + 12)
#define DCR_TR_LCRC_S21  (DCR_TR_LCRC + 13)


// For _bl_tree_configure_class
#define _BL_TREE_CLASS_NUM      (16)  // classes are 0..15

#define _BL_TREE_CLASS_SRC2  _BN( 1)
#define _BL_TREE_CLASS_SRC1  _BN( 2)
#define _BL_TREE_CLASS_SRC0  _BN( 3)
#define _BL_TREE_CLASS_TGT2  _BN( 5)
#define _BL_TREE_CLASS_TGT1  _BN( 6)
#define _BL_TREE_CLASS_TGT0  _BN( 7)
#define _BL_TREE_CLASS_SRCL  _BN(14)
#define _BL_TREE_CLASS_TGTL  _BN(15)

// all allowed valid bits in route specifier
#define _BL_TREE_CLASS_ACCEPT (_BL_TREE_CLASS_SRC2 | _BL_TREE_CLASS_SRC1 |\
                               _BL_TREE_CLASS_SRC0 | _BL_TREE_CLASS_TGT2 |\
                               _BL_TREE_CLASS_TGT1 | _BL_TREE_CLASS_TGT0 |\
                               _BL_TREE_CLASS_SRCL | _BL_TREE_CLASS_TGTL)


#endif // Add nothing below this line.

