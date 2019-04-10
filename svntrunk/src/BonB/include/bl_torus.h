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
//      File: blade/include/sys/machine/bl_torus.h
//
//   Purpose: Define BG/L's Torus Memory Map and DCR Layout.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes:
//
//   History: 05/01/2002: MEG - Created.
//            08/23/2002: MEG and Dong Chen - Update for Issue #274.
//            10/18/2002: MEG - Update to Torus DCR v1.9
//
//
#ifndef _BL_TORUS_H // Prevent multiple inclusion
#define _BL_TORUS_H

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

// Torus bitmap of the BIC's Standard Group 1 (Non-Critical) Interrupt Vector Register
//  See blade/include/sys/machine/bl_bic.h
//  Each bit corresponds to an error counter whose value are set by the
//    Threshold registers.
#define _TS_IC_SG1_NRE_SUMM          _BN( 0)  // Summary Bit for All Non-Recoverable Errors
#define _TS_IC_SG1_INPUT_XP          _BN( 1)  // Receiver X+             (DCR_TS_RE_INPUT_XP
#define _TS_IC_SG1_INPUT_XM          _BN( 2)  // Receiver X-             (DCR_TS_RE_INPUT_XM
#define _TS_IC_SG1_INPUT_YP          _BN( 3)  // Receiver Y+             (DCR_TS_RE_INPUT_YP
#define _TS_IC_SG1_INPUT_YM          _BN( 4)  // Receiver Y-             (DCR_TS_RE_INPUT_YM
#define _TS_IC_SG1_INPUT_ZP          _BN( 5)  // Receiver Z+             (DCR_TS_RE_INPUT_ZP
#define _TS_IC_SG1_INPUT_ZM          _BN( 6)  // Receiver Z-             (DCR_TS_RE_INPUT_ZM
#define _TS_IC_SG1_SEND_SRAM_SBE     _BN( 7)  // Sender SRAM 0,1 SBE     (DCR_TS_RE_SEND_SRAM_SBE
#define _TS_IC_SG1_VCFIFO_SRAM_SBE   _BN( 8)  // Sender VC Fifo SRAM SBE (DCR_TS_RE_VCFIFO_SRAM_SBE
#define _TS_IC_SG1_SEND_RETRY_XP     _BN( 9)  // Sender X+               (DCR_TS_RE_SEND_RETRY_XP
#define _TS_IC_SG1_SEND_RETRY_XM     _BN(10)  // Sender X-               (DCR_TS_RE_SEND_RETRY_XM
#define _TS_IC_SG1_SEND_RETRY_YP     _BN(11)  // Sender Y+               (DCR_TS_RE_SEND_RETRY_YP
#define _TS_IC_SG1_SEND_RETRY_YM     _BN(12)  // Sender Y-               (DCR_TS_RE_SEND_RETRY_YM
#define _TS_IC_SG1_SEND_RETRY_ZP     _BN(13)  // Sender Z+               (DCR_TS_RE_SEND_RETRY_ZP
#define _TS_IC_SG1_SEND_RETRY_ZM     _BN(14)  // Sender Z-               (DCR_TS_RE_SEND_RETRY_ZM
#define _TS_IC_SG1_RAND              _BN(15)  // Rand Num Gen            (DCR_TS_RE_RAND
#define _TS_IC_SG1_INJ_SRAM_SBE      _BN(16)  // Injection SRAM SBE      (DCR_TS_RE_INJ_SRAM_SBE
#define _TS_IC_SG1_REC_SRAM_SBE      _BN(17)  // Reception SRAM SBE      (DCR_TS_RE_REC_SRAM_SBE
#define _TS_IC_SG1_BADHDR_FULLFIFO   _BN(18)  // See counter comments    (DCR_TS_RE_BADHDR_FULLFIFO
#define _TS_IC_SG1_VCFIFO_OVERFLOW   _BN(19)  // See counter comments    (DCR_TS_RE_VCFIFO_OVERFLOW

#define _TS_IC_SG1_INIT_ENABLE (_TS_IC_SG1_NRE_SUMM       |\
                                _TS_IC_SG1_INPUT_XP       |\
                                _TS_IC_SG1_INPUT_XM       |\
                                _TS_IC_SG1_INPUT_YP       |\
                                _TS_IC_SG1_INPUT_YM       |\
                                _TS_IC_SG1_INPUT_ZP       |\
                                _TS_IC_SG1_INPUT_ZM       |\
                                _TS_IC_SG1_SEND_SRAM_SBE  |\
                                _TS_IC_SG1_VCFIFO_SRAM_SBE|\
                                _TS_IC_SG1_SEND_RETRY_XP  |\
                                _TS_IC_SG1_SEND_RETRY_XM  |\
                                _TS_IC_SG1_SEND_RETRY_YP  |\
                                _TS_IC_SG1_SEND_RETRY_YM  |\
                                _TS_IC_SG1_SEND_RETRY_ZP  |\
                                _TS_IC_SG1_SEND_RETRY_ZM  |\
                                _TS_IC_SG1_RAND           |\
                                _TS_IC_SG1_INJ_SRAM_SBE   |\
                                _TS_IC_SG1_REC_SRAM_SBE   |\
                                _TS_IC_SG1_BADHDR_FULLFIFO|\
                                _TS_IC_SG1_VCFIFO_OVERFLOW   )

// Torus bitmap of the BIC's Critical Group 2 Interrupt Vector Register
//  See blade/include/sys/machine/bl_bic.h
//@MG: Need to fix the naming inconsistency to agree
// with WM registers below and fifo addresses above.
#define _TS_IC_CG2_WM_G0_I0    _BN( 0)  // Normal   Injection Fifo 0 < Watermark
#define _TS_IC_CG2_WM_G0_I1    _BN( 1)  // Normal   Injection Fifo 1 < Watermark
#define _TS_IC_CG2_WM_G0_I2    _BN( 2)  // Normal   Injection Fifo 2 < Watermark
#define _TS_IC_CG2_WM_G1_I0    _BN( 3)  // Normal   Injection Fifo 3 < Watermark
#define _TS_IC_CG2_WM_G1_I1    _BN( 4)  // Normal   Injection Fifo 4 < Watermark
#define _TS_IC_CG2_WM_G1_I2    _BN( 5)  // Normal   Injection Fifo 5 < Watermark
#define _TS_IC_CG2_WM_G0_IH    _BN( 6)  // Priority Injection Fifo 0 < Watermark
#define _TS_IC_CG2_WM_G1_IH    _BN( 7)  // Priority Injection Fifo 1 < Watermark
#define _TS_IC_CG2_WM_G0_R0    _BN( 8)  // Normal   Reception Fifo 0 > Watermark
#define _TS_IC_CG2_WM_G0_R1    _BN( 9)  // Normal   Reception Fifo 1 > Watermark
#define _TS_IC_CG2_WM_G0_R2    _BN(10)  // Normal   Reception Fifo 2 > Watermark
#define _TS_IC_CG2_WM_G0_R3    _BN(11)  // Normal   Reception Fifo 3 > Watermark
#define _TS_IC_CG2_WM_G0_R4    _BN(12)  // Normal   Reception Fifo 4 > Watermark
#define _TS_IC_CG2_WM_G0_R5    _BN(13)  // Normal   Reception Fifo 5 > Watermark
#define _TS_IC_CG2_WM_G1_R0    _BN(14)  // Normal   Reception Fifo 0 > Watermark
#define _TS_IC_CG2_WM_G1_R1    _BN(15)  // Normal   Reception Fifo 1 > Watermark
#define _TS_IC_CG2_WM_G1_R2    _BN(16)  // Normal   Reception Fifo 2 > Watermark
#define _TS_IC_CG2_WM_G1_R3    _BN(17)  // Normal   Reception Fifo 3 > Watermark
#define _TS_IC_CG2_WM_G1_R4    _BN(18)  // Normal   Reception Fifo 4 > Watermark
#define _TS_IC_CG2_WM_G1_R5    _BN(19)  // Normal   Reception Fifo 5 > Watermark
#define _TS_IC_CG2_WM_G0_RH    _BN(20)  // Priority Reception Fifo 0 > Watermark
#define _TS_IC_CG2_WM_G1_RH    _BN(21)  // Priority Reception Fifo 1 > Watermark

#define _TS_IC_CG2_INIT_ENABLE (_TS_IC_CG2_WM_G0_I0 |\
                                _TS_IC_CG2_WM_G0_I1 |\
                                _TS_IC_CG2_WM_G0_I2 |\
                                _TS_IC_CG2_WM_G1_I0 |\
                                _TS_IC_CG2_WM_G1_I1 |\
                                _TS_IC_CG2_WM_G1_I2 |\
                                _TS_IC_CG2_WM_G0_IH |\
                                _TS_IC_CG2_WM_G1_IH |\
                                _TS_IC_CG2_WM_G0_R0 |\
                                _TS_IC_CG2_WM_G0_R1 |\
                                _TS_IC_CG2_WM_G0_R2 |\
                                _TS_IC_CG2_WM_G0_R3 |\
                                _TS_IC_CG2_WM_G0_R4 |\
                                _TS_IC_CG2_WM_G0_R5 |\
                                _TS_IC_CG2_WM_G1_R0 |\
                                _TS_IC_CG2_WM_G1_R1 |\
                                _TS_IC_CG2_WM_G1_R2 |\
                                _TS_IC_CG2_WM_G1_R3 |\
                                _TS_IC_CG2_WM_G1_R4 |\
                                _TS_IC_CG2_WM_G1_R5 |\
                                _TS_IC_CG2_WM_G0_RH |\
                                _TS_IC_CG2_WM_G1_RH  )

// Torus Map of DCR Groupings: (see bl_dcrmap.h for DCR_TORUS)
#define DCR_TS_CTRL   (DCR_TORUS + 0x00)        // Control Settings (R/W)
#define DCR_TS_THR    (DCR_TORUS + 0x16)        // Threshold Settings (R/W)
#define DCR_TS_CLR    (DCR_TORUS + 0x1E)        // Clear Controls (W)
#define DCR_TS_NREM   (DCR_TORUS + 0x20)        // Non-Recoverable Error Masks (R/W)
#define DCR_TS_REM    (DCR_TORUS + 0x30)        // Recoverable Error Masks (RC/W)
#define DCR_TS_STAT   (DCR_TORUS + 0x32)        // Status Info (R)
#define DCR_TS_NRE    (DCR_TORUS + 0x5C)        // Non-Recoverable Errors (RO, needs TS reset to clear!)
#define DCR_TS_RE     (DCR_TORUS + 0x6C)        // Recoverable Errors (RC/W)

// Torus Control Group
#define DCR_TS_CTRL_RESET           (DCR_TS_CTRL + 0x00)   // CTRL: Reset Register
#define  _TS_CTRL_RESET_REGS         _BN( 0)               //  Register File Unit Reset
#define  _TS_CTRL_RESET_RAND         _BN( 1)               //  Random Number Generator Unit
#define  _TS_CTRL_RESET_INJ          _BN( 2)               //  Injection Units
#define  _TS_CTRL_RESET_REC          _BN( 3)               //  Reception Units
#define  _TS_CTRL_RESET_SEND_XP      _BN( 4)               //  Sending Unit X+
#define  _TS_CTRL_RESET_SEND_XM      _BN( 5)               //  Sending Unit X-
#define  _TS_CTRL_RESET_SEND_YP      _BN( 6)               //  Sending Unit Y+
#define  _TS_CTRL_RESET_SEND_YM      _BN( 7)               //  Sending Unit Y-
#define  _TS_CTRL_RESET_SEND_ZP      _BN( 8)               //  Sending Unit Z+
#define  _TS_CTRL_RESET_SEND_ZM      _BN( 9)               //  Sending Unit Z-
#define  _TS_CTRL_RESET_SEND_SRAM0   _BN(10)               //  Send SRAM 0, shared by X+,X-,Y+
#define  _TS_CTRL_RESET_SEND_SRAM1   _BN(11)               //  Send SRAM 1, shared by Y-,Z+,Z-
#define  _TS_CTRL_RESET_RECV_XP      _BN(12)               //  Receiving Unit X+
#define  _TS_CTRL_RESET_RECV_XM      _BN(13)               //  Receiving Unit X-
#define  _TS_CTRL_RESET_RECV_YP      _BN(14)               //  Receiving Unit Y+
#define  _TS_CTRL_RESET_RECV_YM      _BN(15)               //  Receiving Unit Y-
#define  _TS_CTRL_RESET_RECV_ZP      _BN(16)               //  Receiving Unit Z+
#define  _TS_CTRL_RESET_RECV_ZM      _BN(17)               //  Receiving Unit Z-
#define DCR_TS_CTRL_NODES           (DCR_TS_CTRL + 0x01)   // CTRL: Machine Nodes (Coordinate means number of nodes -1)
#define   _TS_CTRL_NODES_X(x)         _B8( 7,x)            //  Coordinate of Max Node in X
#define   _TS_CTRL_NODES_Y(x)         _B8(15,x)            //  Coordinate of Max Node in Y
#define   _TS_CTRL_NODES_Z(x)         _B8(23,x)            //  Coordinate of Max Node in Z
#define DCR_TS_CTRL_COORDS_NODE     (DCR_TS_CTRL + 0x02)   // CTRL: Node Coordinates
#define   _TS_CTRL_COORDS_NODE_X(x)   _B8( 7,x)            //  Coordinate of this Node in X
#define   _TS_CTRL_COORDS_NODE_Y(x)   _B8(15,x)            //  Coordinate of this Node in Y
#define   _TS_CTRL_COORDS_NODE_Z(x)   _B8(23,x)            //  Coordinate of this Node in Z
#define DCR_TS_CTRL_COORDS_PLUS     (DCR_TS_CTRL + 0x03)   // CTRL: Plus-Neighbor's Coordinates
#define   _TS_CTRL_COORDS_PLUS_X(x)   _B8( 7,x)            //  Coordinate of this Node's X+ Neighbor
#define   _TS_CTRL_COORDS_PLUS_Y(x)   _B8(15,x)            //  Coordinate of this Node's Y+ Neighbor
#define   _TS_CTRL_COORDS_PLUS_Z(x)   _B8(23,x)            //  Coordinate of this Node's Z+ Neighbor
#define DCR_TS_CTRL_COORDS_MINUS    (DCR_TS_CTRL + 0x04)   // CTRL: Minus-Neighbor's Coordinates
#define   _TS_CTRL_COORDS_MINUS_X(x)  _B8( 7,x)            //  Coordinate of this Node's X- Neighbor
#define   _TS_CTRL_COORDS_MINUS_Y(x)  _B8(15,x)            //  Coordinate of this Node's Y- Neighbor
#define   _TS_CTRL_COORDS_MINUS_Z(x)  _B8(23,x)            //  Coordinate of this Node's Z- Neighbor
#define DCR_TS_CTRL_HINT_PLUS       (DCR_TS_CTRL + 0x05)   // CTRL: Hint Bit Cutoff Plus
#define   _TS_CTRL_HINT_PLUS_X(x)     _B8( 7,x)            //  Set Cut-off X+
#define   _TS_CTRL_HINT_PLUS_Y(x)     _B8(15,x)            //  Set Cut-off Y+
#define   _TS_CTRL_HINT_PLUS_Z(x)     _B8(23,x)            //  Set Cut-off Z+
#define   _GET_TS_CTRL_HINT_PLUS_X(x) _G8(x, 7)            //  Get Cut-off X+
#define   _GET_TS_CTRL_HINT_PLUS_Y(x) _G8(x,15)            //  Get Cut-off Y+
#define   _GET_TS_CTRL_HINT_PLUS_Z(x) _G8(x,23)            //  Get Cut-off Z+
#define DCR_TS_CTRL_HINT_MINUS      (DCR_TS_CTRL + 0x06)   // CTRL: Hint Bit Cutoff Minus
#define   _TS_CTRL_HINT_MINUS_X(x)     _B8( 7,x)           //  Set Cut-off X-
#define   _TS_CTRL_HINT_MINUS_Y(x)     _B8(15,x)           //  Set Cut-off Y-
#define   _TS_CTRL_HINT_MINUS_Z(x)     _B8(23,x)           //  Set Cut-off Z-
#define   _GET_TS_CTRL_HINT_MINUS_X(x) _G8(x, 7)           //  Get Cut-off X-
#define   _GET_TS_CTRL_HINT_MINUS_Y(x) _G8(x,15)           //  Get Cut-off Y-
#define   _GET_TS_CTRL_HINT_MINUS_Z(x) _G8(x,23)           //  Get Cut-off Z-
#define DCR_TS_CTRL_VCTHBD          (DCR_TS_CTRL + 0x07)   // CTRL: Receiver VC Threshold for Bypass Disable
#define  _TS_CTRL_VCTHBD_VCD0(x)      _B7( 7,x)            //  VC-D0 Threshold for Bypass Disable
#define  _TS_CTRL_VCTHBD_VCD1(x)      _B7(15,x)            //  VC-D1 Threshold for Bypass Disable
#define  _TS_CTRL_VCTHBD_VCBN(x)      _B7(23,x)            //  VC-BN Threshold for Bypass Disable
#define  _TS_CTRL_VCTHBD_VCBP(x)      _B7(31,x)            //  VC-BP Threshold for Bypass Disable
#define DCR_TS_CTRL_RETRANS_TOX     (DCR_TS_CTRL + 0x08)   // CTRL: Retransmission Time Outs X+ and X-
#define  _TS_CTRL_RETRANS_TOX_XP(x)   _B16(15,x)           //  Sender X+ in Torus-Cycles
#define  _TS_CTRL_RETRANS_TOX_XM(x)   _B16(31,x)           //  Sender X- in Torus-Cycles
#define DCR_TS_CTRL_RETRANS_TOY     (DCR_TS_CTRL + 0x09)   // CTRL: Retransmission Time Outs Y+ and Y-
#define  _TS_CTRL_RETRANS_TOY_YP(x)   _B16(15,x)           //  Sender Y+ in Torus-Cycles
#define  _TS_CTRL_RETRANS_TOY_YM(x)   _B16(31,x)           //  Sender Y- in Torus-Cycles
#define DCR_TS_CTRL_RETRANS_TOZ     (DCR_TS_CTRL + 0x0A)   // CTRL: Retransmission Time Outs Z+ and Z-
#define  _TS_CTRL_RETRANS_TOZ_ZP(x)   _B16(15,x)           //  Sender Z+ in Torus-Cycles
#define  _TS_CTRL_RETRANS_TOZ_ZM(x)   _B16(31,x)           //  Sender Z- in Torus-Cycles
#define DCR_TS_CTRL_STOPS_RAND      (DCR_TS_CTRL + 0x0B)   // CTRL: Sender Stops and Random Number Generators
#define  _TS_CTRL_STOPS_RAND_XP       _BN( 0)              //  X+ Disable
#define  _TS_CTRL_STOPS_RAND_XM       _BN( 1)              //  X- Disable
#define  _TS_CTRL_STOPS_RAND_YP       _BN( 2)              //  Y+ Disable
#define  _TS_CTRL_STOPS_RAND_YM       _BN( 3)              //  Y- Disable
#define  _TS_CTRL_STOPS_RAND_ZP       _BN( 4)              //  Z+ Disable
#define  _TS_CTRL_STOPS_RAND_ZM       _BN( 5)              //  Z- Disable
#define  _TS_CTRL_STOPS_RAND_FULL(x)  _B8(13,x)            //  Torus Cycles Arbitration based on Fifo Fullness (157)
#define  _TS_CTRL_STOPS_RAND_RAND(x)  _B8(21,x)            //  Torus Cycles Arbitration done Randomly (53)
#define  _TS_CTRL_STOPS_RAND_HPINJ(x) _B5(26,x)            //  Prob HP Injection wins over HP Network (0)
#define DCR_TS_CTRL_CLK_RATIO       (DCR_TS_CTRL + 0x0C)   // CTRL: Clock_ts to Clock_x2 Ratio
#define  _TS_CTRL_CLK_RATIO(x)       _B3( 2,x)             //  Clock_ts to Clock_x2 Ration Setting
#define  _TS_CTRL_CLK_RATIO_X2       _TS_CTRL_CLK_RATIO(0) //  clock_ts = clock_x2      = clock_x2
#define  _TS_CTRL_CLK_RATIO_X4       _TS_CTRL_CLK_RATIO(1) //  clock_ts = clock_x2 /  2 = clock_x4
#define  _TS_CTRL_CLK_RATIO_X8       _TS_CTRL_CLK_RATIO(2) //  clock_ts = clock_x2 /  4 = clock_x8
#define  _TS_CTRL_CLK_RATIO_X16      _TS_CTRL_CLK_RATIO(3) //  clock_ts = clock_x2 /  8 = clock_x16
#define  _TS_CTRL_CLK_RATIO_X32      _TS_CTRL_CLK_RATIO(4) //  clock_ts = clock_x2 / 16 = clock_x32
#define DCR_TS_CTRL_WM_G0           (DCR_TS_CTRL + 0x0D)   // CTRL: WaterMarks for Fifo Group 0 (units=32B, 0=Empty, 0x20=Full)
#define  _TS_CTRL_WM_G0_RN0(x)        _B5( 4,x)            //  Group 0 Normal   Reception Pool
#define  _TS_CTRL_WM_G0_RP0(x)        _B5( 9,x)            //  Group 0 Priority Reception 0
#define  _TS_CTRL_WM_G0_IN0(x)        _B5(14,x)            //  Group 0 Normal   Injection 0
#define  _TS_CTRL_WM_G0_IN1(x)        _B5(19,x)            //  Group 0 Normal   Injection 1
#define  _TS_CTRL_WM_G0_IN2(x)        _B5(24,x)            //  Group 0 Normal   Injection 2
#define  _TS_CTRL_WM_G0_IP0(x)        _B5(29,x)            //  Group 0 Priority Injection 0
#define DCR_TS_CTRL_WM_G1           (DCR_TS_CTRL + 0x0E)   // CTRL: WaterMarks for Fifo Group 1 (units=32B, 0=Empty, 0x20=Full)
#define  _TS_CTRL_WM_G1_RN0(x)        _B5( 4,x)            //  Group 1 Normal   Reception Pool
#define  _TS_CTRL_WM_G1_RP0(x)        _B5( 9,x)            //  Group 1 Priority Reception 0
#define  _TS_CTRL_WM_G1_IN0(x)        _B5(14,x)            //  Group 1 Normal   Injection 0
#define  _TS_CTRL_WM_G1_IN1(x)        _B5(19,x)            //  Group 1 Normal   Injection 1
#define  _TS_CTRL_WM_G1_IN2(x)        _B5(24,x)            //  Group 1 Normal   Injection 2
#define  _TS_CTRL_WM_G1_IP0(x)        _B5(29,x)            //  Group 1 Priority Injection 0
#define DCR_TS_CTRL_SRAM_WD_0_31    (DCR_TS_CTRL + 0x0F)   // CTRL: SRAM Backdoor Write-Data bits 0  - 31
#define DCR_TS_CTRL_SRAM_WD_32_63   (DCR_TS_CTRL + 0x10)   // CTRL: SRAM Backdoor Write-Data bits 32 - 63
#define DCR_TS_CTRL_SRAM_WD_CTRL    (DCR_TS_CTRL + 0x11)   // CTRL: SRAM Backdoor Write-Data Control
#define  _TS_CTRL_SRAM_WD_CTRL_LSB(x)   _B8( 7,x)          //  Backdoor Write Data bits 64-71 (ie, LSB)
#define  _TS_CTRL_SRAM_WD_CTRL_ADDR(x)  _B16(32,x)         //  Address bits 0-15 (See Table 1.1.13)
#define  _TS_CTRL_SRAM_WD_CTRL_WE       _BN(24)            //  Write Enable
#define  _TS_CTRL_SRAM_WD_CTRL_RE       _BN(25)            //  Read  Enable
#define  _TS_CTRL_SRAM_WD_CTRL_WECC     _BN(26)            //  Write Enable Invoke ECC Disable
#define  _TS_CTRL_SRAM_WD_CTRL_RECC     _BN(27)            //  Read  Enable Invoke ECC Disable
#define  _TS_CTRL_SRAM_WD_CTRL_VIADCR   _BN(28)            //  Read/Write via DCR when 1, via PLB when 0
#define DCR_TS_CTRL_LINKS           (DCR_TS_CTRL + 0x12)   // CTRL: Link Control (LoopBack and Receiver/Retransmisstion Stops)
#define  _TS_CTRL_LINKS_LOOPBACK_EN     _BN( 0)            //  LoopBack Enable: 0=Disable, 1=Enable (0 on Reset)
#define  _TS_CTRL_LINKS_RECV_STOP_XP    _BN( 1)            //  Stop Receiving from X+ Link
#define  _TS_CTRL_LINKS_RECV_STOP_XM    _BN( 2)            //  Stop Receiving from X- Link
#define  _TS_CTRL_LINKS_RECV_STOP_YP    _BN( 3)            //  Stop Receiving from Y+ Link
#define  _TS_CTRL_LINKS_RECV_STOP_YM    _BN( 4)            //  Stop Receiving from Y- Link
#define  _TS_CTRL_LINKS_RECV_STOP_ZP    _BN( 5)            //  Stop Receiving from Z+ Link
#define  _TS_CTRL_LINKS_RECV_STOP_ZM    _BN( 6)            //  Stop Receiving from Z- Link
#define  _TS_CTRL_LINKS_RETR_STOP_XP    _BN( 7)            //  Stop Retransmisstion for X+ Link
#define  _TS_CTRL_LINKS_RETR_STOP_XM    _BN( 8)            //  Stop Retransmisstion for X- Link
#define  _TS_CTRL_LINKS_RETR_STOP_YP    _BN( 9)            //  Stop Retransmisstion for Y+ Link
#define  _TS_CTRL_LINKS_RETR_STOP_YM    _BN(10)            //  Stop Retransmisstion for Y- Link
#define  _TS_CTRL_LINKS_RETR_STOP_ZP    _BN(11)            //  Stop Retransmisstion for Z+ Link
#define  _TS_CTRL_LINKS_RETR_STOP_ZM    _BN(12)            //  Stop Retransmisstion for Z- Link
#define DCR_TS_CTRL_FIFO            (DCR_TS_CTRL + 0x13)   // CTRL: FIFO Read Control
#define  _TS_CTRL_FIFO_XP_VC_PTRS      0x00000000          //  Fifo Read Pointers for X+ BP,BN,D1,D0 VC Fifos
#define  _TS_CTRL_FIFO_XM_VC_PTRS      0x00000001          //  Fifo Read Pointers for X- BP,BN,D1,D0 VC Fifos
#define  _TS_CTRL_FIFO_YP_VC_PTRS      0x00000002          //  Fifo Read Pointers for Y+ BP,BN,D1,D0 VC Fifos
#define  _TS_CTRL_FIFO_YM_VC_PTRS      0x00000003          //  Fifo Read Pointers for Y- BP,BN,D1,D0 VC Fifos
#define  _TS_CTRL_FIFO_ZP_VC_PTRS      0x00000004          //  Fifo Read Pointers for Z+ BP,BN,D1,D0 VC Fifos
#define  _TS_CTRL_FIFO_ZM_VC_PTRS      0x00000005          //  Fifo Read Pointers for Z- BP,BN,D1,D0 VC Fifos
#define  _TS_CTRL_FIFO_INJ_G0_PTRS     0x00000006          //  Processor Injection Fifo Read Pointers for Group 0 (p0,n0,n1,n2)
#define  _TS_CTRL_FIFO_INJ_G1_PTRS     0x00000007          //  Processor Injection Fifo Read Pointers for Group 1 (n3,n4,n5,p1)
#define  _TS_CTRL_FIFO_XP_BP_PF_D0H    0x01000000          //  X+ VC Fifo BP Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_BP_PF_D0L    0x01000001          //  X+ VC Fifo BP Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XP_BP_PF_D1H    0x01000002          //  X+ VC Fifo BP Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_BP_PF_D1L    0x01000003          //  X+ VC Fifo BP Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_XP_BN_PF_D0H    0x01000004          //  X+ VC Fifo BN Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_BN_PF_D0L    0x01000005          //  X+ VC Fifo BN Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XP_BN_PF_D1H    0x01000006          //  X+ VC Fifo BN Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_BN_PF_D1L    0x01000007          //  X+ VC Fifo BN Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_XP_D1_PF_D0H    0x01000008          //  X+ VC Fifo D1 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_D1_PF_D0L    0x01000009          //  X+ VC Fifo D1 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XP_D1_PF_D1H    0x0100000A          //  X+ VC Fifo D1 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_D1_PF_D1L    0x0100000B          //  X+ VC Fifo D1 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_XP_D0_PF_D0H    0x0100000C          //  X+ VC Fifo D0 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_D0_PF_D0L    0x0100000D          //  X+ VC Fifo D0 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XP_D0_PF_D1H    0x0100000E          //  X+ VC Fifo D0 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XP_D0_PF_D1L    0x0100000F          //  X+ VC Fifo D0 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_BP_PF_D0H    0x02000000          //  X- VC Fifo BP Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_BP_PF_D0L    0x02000001          //  X- VC Fifo BP Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_BP_PF_D1H    0x02000002          //  X- VC Fifo BP Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_BP_PF_D1L    0x02000003          //  X- VC Fifo BP Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_BN_PF_D0H    0x02000004          //  X- VC Fifo BN Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_BN_PF_D0L    0x02000005          //  X- VC Fifo BN Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_BN_PF_D1H    0x02000006          //  X- VC Fifo BN Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_BN_PF_D1L    0x02000007          //  X- VC Fifo BN Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_D1_PF_D0H    0x02000008          //  X- VC Fifo D1 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_D1_PF_D0L    0x02000009          //  X- VC Fifo D1 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_D1_PF_D1H    0x0200000A          //  X- VC Fifo D1 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_D1_PF_D1L    0x0200000B          //  X- VC Fifo D1 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_D0_PF_D0H    0x0200000C          //  X- VC Fifo D0 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_D0_PF_D0L    0x0200000D          //  X- VC Fifo D0 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_XM_D0_PF_D1H    0x0200000E          //  X- VC Fifo D0 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_XM_D0_PF_D1L    0x0200000F          //  X- VC Fifo D0 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_BP_PF_D0H    0x03000000          //  Y+ VC Fifo BP Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_BP_PF_D0L    0x03000001          //  Y+ VC Fifo BP Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_BP_PF_D1H    0x03000002          //  Y+ VC Fifo BP Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_BP_PF_D1L    0x03000003          //  Y+ VC Fifo BP Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_BN_PF_D0H    0x03000004          //  Y+ VC Fifo BN Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_BN_PF_D0L    0x03000005          //  Y+ VC Fifo BN Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_BN_PF_D1H    0x03000006          //  Y+ VC Fifo BN Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_BN_PF_D1L    0x03000007          //  Y+ VC Fifo BN Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_D1_PF_D0H    0x03000008          //  Y+ VC Fifo D1 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_D1_PF_D0L    0x03000009          //  Y+ VC Fifo D1 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_D1_PF_D1H    0x0300000A          //  Y+ VC Fifo D1 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_D1_PF_D1L    0x0300000B          //  Y+ VC Fifo D1 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_D0_PF_D0H    0x0300000C          //  Y+ VC Fifo D0 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_D0_PF_D0L    0x0300000D          //  Y+ VC Fifo D0 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YP_D0_PF_D1H    0x0300000E          //  Y+ VC Fifo D0 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YP_D0_PF_D1L    0x0300000F          //  Y+ VC Fifo D0 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_BP_PF_D0H    0x04000000          //  Y- VC Fifo BP Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_BP_PF_D0L    0x04000001          //  Y- VC Fifo BP Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_BP_PF_D1H    0x04000002          //  Y- VC Fifo BP Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_BP_PF_D1L    0x04000003          //  Y- VC Fifo BP Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_BN_PF_D0H    0x04000004          //  Y- VC Fifo BN Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_BN_PF_D0L    0x04000005          //  Y- VC Fifo BN Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_BN_PF_D1H    0x04000006          //  Y- VC Fifo BN Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_BN_PF_D1L    0x04000007          //  Y- VC Fifo BN Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_D1_PF_D0H    0x04000008          //  Y- VC Fifo D1 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_D1_PF_D0L    0x04000009          //  Y- VC Fifo D1 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_D1_PF_D1H    0x0400000A          //  Y- VC Fifo D1 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_D1_PF_D1L    0x0400000B          //  Y- VC Fifo D1 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_D0_PF_D0H    0x0400000C          //  Y- VC Fifo D0 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_D0_PF_D0L    0x0400000D          //  Y- VC Fifo D0 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_YM_D0_PF_D1H    0x0400000E          //  Y- VC Fifo D0 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_YM_D0_PF_D1L    0x0400000F          //  Y- VC Fifo D0 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_BP_PF_D0H    0x05000000          //  Z+ VC Fifo BP Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_BP_PF_D0L    0x05000001          //  Z+ VC Fifo BP Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_BP_PF_D1H    0x05000002          //  Z+ VC Fifo BP Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_BP_PF_D1L    0x05000003          //  Z+ VC Fifo BP Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_BN_PF_D0H    0x05000004          //  Z+ VC Fifo BN Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_BN_PF_D0L    0x05000005          //  Z+ VC Fifo BN Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_BN_PF_D1H    0x05000006          //  Z+ VC Fifo BN Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_BN_PF_D1L    0x05000007          //  Z+ VC Fifo BN Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_D1_PF_D0H    0x05000008          //  Z+ VC Fifo D1 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_D1_PF_D0L    0x05000009          //  Z+ VC Fifo D1 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_D1_PF_D1H    0x0500000A          //  Z+ VC Fifo D1 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_D1_PF_D1L    0x0500000B          //  Z+ VC Fifo D1 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_D0_PF_D0H    0x0500000C          //  Z+ VC Fifo D0 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_D0_PF_D0L    0x0500000D          //  Z+ VC Fifo D0 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZP_D0_PF_D1H    0x0500000E          //  Z+ VC Fifo D0 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZP_D0_PF_D1L    0x0500000F          //  Z+ VC Fifo D0 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_BP_PF_D0H    0x06000000          //  Z- VC Fifo BP Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_BP_PF_D0L    0x06000001          //  Z- VC Fifo BP Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_BP_PF_D1H    0x06000002          //  Z- VC Fifo BP Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_BP_PF_D1L    0x06000003          //  Z- VC Fifo BP Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_BN_PF_D0H    0x06000004          //  Z- VC Fifo BN Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_BN_PF_D0L    0x06000005          //  Z- VC Fifo BN Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_BN_PF_D1H    0x06000006          //  Z- VC Fifo BN Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_BN_PF_D1L    0x06000007          //  Z- VC Fifo BN Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_D1_PF_D0H    0x06000008          //  Z- VC Fifo D1 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_D1_PF_D0L    0x06000009          //  Z- VC Fifo D1 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_D1_PF_D1H    0x0600000A          //  Z- VC Fifo D1 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_D1_PF_D1L    0x0600000B          //  Z- VC Fifo D1 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_D0_PF_D0H    0x0600000C          //  Z- VC Fifo D0 Prefetch Data 0 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_D0_PF_D0L    0x0600000D          //  Z- VC Fifo D0 Prefetch Data 0 Low  (32-63)
#define  _TS_CTRL_FIFO_ZM_D0_PF_D1H    0x0600000E          //  Z- VC Fifo D0 Prefetch Data 1 High ( 0-31)
#define  _TS_CTRL_FIFO_ZM_D0_PF_D1L    0x0600000F          //  Z- VC Fifo D0 Prefetch Data 1 Low  (32-63)
#define  _TS_CTRL_FIFO_INJ_P0_PF_D00   0x07000000          //  Processor Injection P0 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_P0_PF_D01   0x07000001          //  Processor Injection P0 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_P0_PF_D02   0x07000002          //  Processor Injection P0 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_P0_PF_D03   0x07000003          //  Processor Injection P0 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_P0_PF_D10   0x07000004          //  Processor Injection P0 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_P0_PF_D11   0x07000005          //  Processor Injection P0 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_P0_PF_D12   0x07000006          //  Processor Injection P0 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_P0_PF_D13   0x07000007          //  Processor Injection P0 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_INJ_P1_PF_D00   0x07000008          //  Processor Injection P1 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_P1_PF_D01   0x07000009          //  Processor Injection P1 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_P1_PF_D02   0x0700000A          //  Processor Injection P1 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_P1_PF_D03   0x0700000B          //  Processor Injection P1 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_P1_PF_D10   0x0700000C          //  Processor Injection P1 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_P1_PF_D11   0x0700000D          //  Processor Injection P1 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_P1_PF_D12   0x0700000E          //  Processor Injection P1 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_P1_PF_D13   0x0700000F          //  Processor Injection P1 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_INJ_N0_PF_D00   0x08000000          //  Processor Injection N0 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_N0_PF_D01   0x08000001          //  Processor Injection N0 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_N0_PF_D02   0x08000002          //  Processor Injection N0 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_N0_PF_D03   0x08000003          //  Processor Injection N0 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_N0_PF_D10   0x08000004          //  Processor Injection N0 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_N0_PF_D11   0x08000005          //  Processor Injection N0 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_N0_PF_D12   0x08000006          //  Processor Injection N0 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_N0_PF_D13   0x08000007          //  Processor Injection N0 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_INJ_N1_PF_D00   0x08000008          //  Processor Injection N1 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_N1_PF_D01   0x08000009          //  Processor Injection N1 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_N1_PF_D02   0x0800000A          //  Processor Injection N1 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_N1_PF_D03   0x0800000B          //  Processor Injection N1 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_N1_PF_D10   0x0800000C          //  Processor Injection N1 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_N1_PF_D11   0x0800000D          //  Processor Injection N1 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_N1_PF_D12   0x0800000E          //  Processor Injection N1 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_N1_PF_D13   0x0800000F          //  Processor Injection N1 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_INJ_N2_PF_D00   0x09000000          //  Processor Injection N2 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_N2_PF_D01   0x09000001          //  Processor Injection N2 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_N2_PF_D02   0x09000002          //  Processor Injection N2 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_N2_PF_D03   0x09000003          //  Processor Injection N2 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_N2_PF_D10   0x09000004          //  Processor Injection N2 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_N2_PF_D11   0x09000005          //  Processor Injection N2 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_N2_PF_D12   0x09000006          //  Processor Injection N2 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_N2_PF_D13   0x09000007          //  Processor Injection N2 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_INJ_N3_PF_D00   0x09000008          //  Processor Injection N3 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_N3_PF_D01   0x09000009          //  Processor Injection N3 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_N3_PF_D02   0x0900000A          //  Processor Injection N3 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_N3_PF_D03   0x0900000B          //  Processor Injection N3 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_N3_PF_D10   0x0900000C          //  Processor Injection N3 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_N3_PF_D11   0x0900000D          //  Processor Injection N3 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_N3_PF_D12   0x0900000E          //  Processor Injection N3 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_N3_PF_D13   0x0900000F          //  Processor Injection N3 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_INJ_N4_PF_D00   0x0A000000          //  Processor Injection N4 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_N4_PF_D01   0x0A000001          //  Processor Injection N4 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_N4_PF_D02   0x0A000002          //  Processor Injection N4 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_N4_PF_D03   0x0A000003          //  Processor Injection N4 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_N4_PF_D10   0x0A000004          //  Processor Injection N4 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_N4_PF_D11   0x0A000005          //  Processor Injection N4 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_N4_PF_D12   0x0A000006          //  Processor Injection N4 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_N4_PF_D13   0x0A000007          //  Processor Injection N4 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_INJ_N5_PF_D00   0x0A000008          //  Processor Injection N5 Prefetch Data 0  0- 31
#define  _TS_CTRL_FIFO_INJ_N5_PF_D01   0x0A000009          //  Processor Injection N5 Prefetch Data 0 32- 63
#define  _TS_CTRL_FIFO_INJ_N5_PF_D02   0x0A00000A          //  Processor Injection N5 Prefetch Data 0 64- 95
#define  _TS_CTRL_FIFO_INJ_N5_PF_D03   0x0A00000B          //  Processor Injection N5 Prefetch Data 0 96-127
#define  _TS_CTRL_FIFO_INJ_N5_PF_D10   0x0A00000C          //  Processor Injection N5 Prefetch Data 1  0- 31
#define  _TS_CTRL_FIFO_INJ_N5_PF_D11   0x0A00000D          //  Processor Injection N5 Prefetch Data 1 32- 63
#define  _TS_CTRL_FIFO_INJ_N5_PF_D12   0x0A00000E          //  Processor Injection N5 Prefetch Data 1 64- 95
#define  _TS_CTRL_FIFO_INJ_N5_PF_D13   0x0A00000F          //  Processor Injection N5 Prefetch Data 1 96-127
#define  _TS_CTRL_FIFO_X_VC_PF_VALID   0x0B000000          //  X +/- VC Prefetch Data Valids
#define  _TS_CTRL_FIFO_Y_VC_PF_VALID   0x0B000001          //  Y +/- VC Prefetch Data Valids
#define  _TS_CTRL_FIFO_Z_VC_PF_VALID   0x0B000002          //  Z +/- VC Prefetch Data Valids
#define  _TS_CTRL_FIFO_INJ_PF_VALID    0x0B000003          //  Processor Injection Prefetch Data Valids


// Torus Counter Threshold for Recoverable (Corrected) Error Interrupts
#define DCR_TS_THR_RECV_RETRY       (DCR_TS_THR + 0x00)    // THR: Receiver Retry Interrupt Threshold
#define DCR_TS_THR_SRAM_ECC         (DCR_TS_THR + 0x01)    // THR: Sender SBE SRAM 0,1 Interrupt Threshold
#define DCR_TS_THR_SEND_RETRY       (DCR_TS_THR + 0x02)    // THR: Sender 0-5 Resend Attempts
#define DCR_TS_THR_RAND_ERR         (DCR_TS_THR + 0x03)    // THR: Random Number Generator Non-Fatal Error
#define DCR_TS_THR_RECV_PIPE        (DCR_TS_THR + 0x04)    // THR: Any Receiver PipeLine non-CRC Error Threshold
#define DCR_TS_THR_VCFIFO_OVERFLOW  (DCR_TS_THR + 0x05)    // THR: Chunks dropped in a VC Fifo because no space. (Header ECC Escape Error)


// Torus Clear Controls Group (access is Write+Clear)
#define DCR_TS_CLR_CTRL             (DCR_TS_CLR + 0x00)    // CLR: Clear Control Bits (WO)
#define  _TS_CLR_CTRL_LINK_CRC_RXP    _BN( 0)              //   Link CRC Clear Receiver X+
#define  _TS_CLR_CTRL_LINK_CRC_RXM    _BN( 1)              //   Link CRC Clear Receiver X-
#define  _TS_CLR_CTRL_LINK_CRC_RYP    _BN( 2)              //   Link CRC Clear Receiver Y+
#define  _TS_CLR_CTRL_LINK_CRC_RYM    _BN( 3)              //   Link CRC Clear Receiver Y-
#define  _TS_CLR_CTRL_LINK_CRC_RZP    _BN( 4)              //   Link CRC Clear Receiver Z+
#define  _TS_CLR_CTRL_LINK_CRC_RZM    _BN( 5)              //   Link CRC Clear Receiver Z-
#define  _TS_CLR_CTRL_VCFIFO_ECC_RXP  _BN( 6)              //   VC Fifo ECC Clear Receiver X+
#define  _TS_CLR_CTRL_VCFIFO_ECC_RXM  _BN( 7)              //   VC Fifo ECC Clear Receiver X-
#define  _TS_CLR_CTRL_VCFIFO_ECC_RYP  _BN( 8)              //   VC Fifo ECC Clear Receiver Y+
#define  _TS_CLR_CTRL_VCFIFO_ECC_RYM  _BN( 9)              //   VC Fifo ECC Clear Receiver Y-
#define  _TS_CLR_CTRL_VCFIFO_ECC_RZP  _BN(10)              //   VC Fifo ECC Clear Receiver Z+
#define  _TS_CLR_CTRL_VCFIFO_ECC_RZM  _BN(11)              //   VC Fifo ECC Clear Receiver Z-
#define  _TS_CLR_CTRL_LINK_CRC_SXP    _BN(12)              //   Link CRC Clear Sender X+
#define  _TS_CLR_CTRL_LINK_CRC_SXM    _BN(13)              //   Link CRC Clear Sender X-
#define  _TS_CLR_CTRL_LINK_CRC_SYP    _BN(14)              //   Link CRC Clear Sender Y+
#define  _TS_CLR_CTRL_LINK_CRC_SYM    _BN(15)              //   Link CRC Clear Sender Y-
#define  _TS_CLR_CTRL_LINK_CRC_SZP    _BN(16)              //   Link CRC Clear Sender Z+
#define  _TS_CLR_CTRL_LINK_CRC_SZM    _BN(17)              //   Link CRC Clear Sender Z-
#define  _TS_CLR_CTRL_SRAM0_ECC       _BN(18)              //   Sender Fatal ECC Error in SRAM 0
#define  _TS_CLR_CTRL_SRAM1_ECC       _BN(19)              //   Sender Fatal ECC Error in SRAM 1
#define  _TS_CLR_CTRL_INJ_N0_CKSUM    _BN(20)              //   Injection Normal Fifo 0 CheckSum Clear
#define  _TS_CLR_CTRL_INJ_N1_CKSUM    _BN(21)              //   Injection Normal Fifo 1 CheckSum Clear
#define  _TS_CLR_CTRL_INJ_N2_CKSUM    _BN(22)              //   Injection Normal Fifo 2 CheckSum Clear
#define  _TS_CLR_CTRL_INJ_N3_CKSUM    _BN(23)              //   Injection Normal Fifo 3 CheckSum Clear
#define  _TS_CLR_CTRL_INJ_N4_CKSUM    _BN(24)              //   Injection Normal Fifo 4 CheckSum Clear
#define  _TS_CLR_CTRL_INJ_N5_CKSUM    _BN(25)              //   Injection Normal Fifo 5 CheckSum Clear
#define  _TS_CLR_CTRL_INJ_P0_CKSUM    _BN(26)              //   Injection Priority 0 Fifo CheckSum Clear
#define  _TS_CLR_CTRL_INJ_P1_CKSUM    _BN(27)              //   Injection Priority 1 Fifo CheckSum Clear
#define  _TS_CLR_CTRL_INJ_ECC_SRAM    _BN(28)              //   Injection Clear for Fatal ECC in Any SRAM
#define  _TS_CLR_CTRL_REC_ECC_SRAM    _BN(29)              //   Reception Clear for Fatal ECC in Any SRAM

// Torus Status Values
#define DCR_TS_STAT_REC_CRC_XP       (DCR_TS_STAT + 0x00)  // STAT: Receiver Link CRC X+
#define DCR_TS_STAT_REC_CRC_XM       (DCR_TS_STAT + 0x01)  // STAT: Receiver Link CRC X-
#define DCR_TS_STAT_REC_CRC_YP       (DCR_TS_STAT + 0x02)  // STAT: Receiver Link CRC Y+
#define DCR_TS_STAT_REC_CRC_YM       (DCR_TS_STAT + 0x03)  // STAT: Receiver Link CRC Y-
#define DCR_TS_STAT_REC_CRC_ZP       (DCR_TS_STAT + 0x04)  // STAT: Receiver Link CRC Z+
#define DCR_TS_STAT_REC_CRC_ZM       (DCR_TS_STAT + 0x05)  // STAT: Receiver Link CRC Z-
#define DCR_TS_STAT_REC_FULL_XP      (DCR_TS_STAT + 0x06)  // STAT: Receiver VC Fullness X+
#define  _TS_STAT_REC_FULL_XP_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_REC_FULL_XP_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_REC_FULL_XP_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_REC_FULL_XP_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_REC_FULL_XM      (DCR_TS_STAT + 0x07)  // STAT: Receiver VC Fullness X-
#define  _TS_STAT_REC_FULL_XM_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_REC_FULL_XM_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_REC_FULL_XM_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_REC_FULL_XM_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_REC_FULL_YP      (DCR_TS_STAT + 0x08)  // STAT: Receiver VC Fullness Y+
#define  _TS_STAT_REC_FULL_YP_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_REC_FULL_YP_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_REC_FULL_YP_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_REC_FULL_YP_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_REC_FULL_YM      (DCR_TS_STAT + 0x09)  // STAT: Receiver VC Fullness Y-
#define  _TS_STAT_REC_FULL_YM_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_REC_FULL_YM_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_REC_FULL_YM_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_REC_FULL_YM_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_REC_FULL_ZP      (DCR_TS_STAT + 0x0A)  // STAT: Receiver VC Fullness Z+
#define  _TS_STAT_REC_FULL_ZP_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_REC_FULL_ZP_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_REC_FULL_ZP_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_REC_FULL_ZP_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_REC_FULL_ZM      (DCR_TS_STAT + 0x0B)  // STAT: Receiver VC Fullness Z-
#define  _TS_STAT_REC_FULL_ZM_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_REC_FULL_ZM_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_REC_FULL_ZM_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_REC_FULL_ZM_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_SEND_TOK_XP      (DCR_TS_STAT + 0x0C)  // STAT: Sender VC Tokens X+
#define  _TS_STAT_SEND_TOK_XP_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_SEND_TOK_XP_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_SEND_TOK_XP_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_SEND_TOK_XP_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_SEND_TOK_XM      (DCR_TS_STAT + 0x0D)  // STAT: Sender VC Tokens X-
#define  _TS_STAT_SEND_TOK_XM_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_SEND_TOK_XM_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_SEND_TOK_XM_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_SEND_TOK_XM_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_SEND_TOK_YP      (DCR_TS_STAT + 0x0E)  // STAT: Sender VC Tokens Y+
#define  _TS_STAT_SEND_TOK_YP_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_SEND_TOK_YP_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_SEND_TOK_YP_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_SEND_TOK_YP_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_SEND_TOK_YM      (DCR_TS_STAT + 0x0F)  // STAT: Sender VC Tokens Y-
#define  _TS_STAT_SEND_TOK_YM_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_SEND_TOK_YM_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_SEND_TOK_YM_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_SEND_TOK_YM_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_SEND_TOK_ZP      (DCR_TS_STAT + 0x10)  // STAT: Sender VC Tokens Z+
#define  _TS_STAT_SEND_TOK_ZP_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_SEND_TOK_ZP_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_SEND_TOK_ZP_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_SEND_TOK_ZP_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_SEND_TOK_ZM      (DCR_TS_STAT + 0x11)  // STAT: Sender VC Tokens Z-
#define  _TS_STAT_SEND_TOK_ZM_VCD0(x)  _B8( 7,x)           //  VC Dynamic 0 Fullness
#define  _TS_STAT_SEND_TOK_ZM_VCD1(x)  _B8(15,x)           //  VC Dynamic 1 Fullness
#define  _TS_STAT_SEND_TOK_ZM_VCBN(x)  _B8(23,x)           //  VC Bubble Normal Fullness
#define  _TS_STAT_SEND_TOK_ZM_VCBP(x)  _B8(31,x)           //  VC Bubble Priority Fullness
#define DCR_TS_STAT_LINK_CRC_XP      (DCR_TS_STAT + 0x12)  // STAT: Sender Link CRC X+
#define DCR_TS_STAT_LINK_CRC_XM      (DCR_TS_STAT + 0x13)  // STAT: Sender Link CRC X-
#define DCR_TS_STAT_LINK_CRC_YP      (DCR_TS_STAT + 0x14)  // STAT: Sender Link CRC Y+
#define DCR_TS_STAT_LINK_CRC_YM      (DCR_TS_STAT + 0x15)  // STAT: Sender Link CRC Y-
#define DCR_TS_STAT_LINK_CRC_ZP      (DCR_TS_STAT + 0x16)  // STAT: Sender Link CRC Z+
#define DCR_TS_STAT_LINK_CRC_ZM      (DCR_TS_STAT + 0x17)  // STAT: Sender Link CRC Z-
#define DCR_TS_STAT_RESEND_PTR_X     (DCR_TS_STAT + 0x18)  // STAT: Resend Fifo Begin/End Pointers X+/X-
#define  _TS_STAT_RESEND_PTR_XPB(x)    _B8( 7,x)           //  X+ Begin pointer
#define  _TS_STAT_RESEND_PTR_XPE(x)    _B8(15,x)           //  X+ End   pointer
#define  _TS_STAT_RESEND_PTR_XMB(x)    _B8(23,x)           //  X- Begin pointer
#define  _TS_STAT_RESEND_PTR_XME(x)    _B8(31,x)           //  X- End   pointer
#define DCR_TS_STAT_RESEND_PTR_Y     (DCR_TS_STAT + 0x19)  // STAT: Resend Fifo Begin/End Pointers Y+/Y-
#define  _TS_STAT_RESEND_PTR_YPB(x)    _B8( 7,x)           //  Y+ Begin pointer
#define  _TS_STAT_RESEND_PTR_YPE(x)    _B8(15,x)           //  Y+ End   pointer
#define  _TS_STAT_RESEND_PTR_YMB(x)    _B8(23,x)           //  Y- Begin pointer
#define  _TS_STAT_RESEND_PTR_YME(x)    _B8(31,x)           //  Y- End   pointer
#define DCR_TS_STAT_RESEND_PTR_Z     (DCR_TS_STAT + 0x1A)  // STAT: Resend Fifo Begin/End Pointers Z+/Z-
#define  _TS_STAT_RESEND_PTR_ZPB(x)    _B8( 7,x)           //  Z+ Begin pointer
#define  _TS_STAT_RESEND_PTR_ZPE(x)    _B8(15,x)           //  Z+ End   pointer
#define  _TS_STAT_RESEND_PTR_ZMB(x)    _B8(23,x)           //  Z- Begin pointer
#define  _TS_STAT_RESEND_PTR_ZME(x)    _B8(31,x)           //  Z- End   pointer
#define DCR_TS_STAT_CKSUM_INJ_N0     (DCR_TS_STAT + 0x1B)  // STAT: Injection Checksum Normal 0
#define DCR_TS_STAT_CKSUM_INJ_N1     (DCR_TS_STAT + 0x1C)  // STAT: Injection Checksum Normal 1
#define DCR_TS_STAT_CKSUM_INJ_N2     (DCR_TS_STAT + 0x1D)  // STAT: Injection Checksum Normal 2
#define DCR_TS_STAT_CKSUM_INJ_N3     (DCR_TS_STAT + 0x1E)  // STAT: Injection Checksum Normal 3
#define DCR_TS_STAT_CKSUM_INJ_N4     (DCR_TS_STAT + 0x1F)  // STAT: Injection Checksum Normal 4
#define DCR_TS_STAT_CKSUM_INJ_N5     (DCR_TS_STAT + 0x20)  // STAT: Injection Checksum Normal 5
#define DCR_TS_STAT_CKSUM_INJ_P0     (DCR_TS_STAT + 0x21)  // STAT: Injection Checksum Priority 0
#define DCR_TS_STAT_CKSUM_INJ_P1     (DCR_TS_STAT + 0x22)  // STAT: Injection Checksum Priority 1
#define DCR_TS_STAT_SRAM_RD_0_31     (DCR_TS_STAT + 0x23)  // STAT: SRAM Back-Door Read Data  0-31
#define DCR_TS_STAT_SRAM_RD_32_63    (DCR_TS_STAT + 0x24)  // STAT: SRAM Back-Door Read Data 32-63
#define DCR_TS_STAT_SRAM_RD_CTRL     (DCR_TS_STAT + 0x25)  // STAT: SRAM Back-Door Read Data 64-71 and Control
#define  _TS_STAT_SRAM_RD_CTRL_LSB(x)  _B8( 7,x)           //  SRAM Back-door Read Data 64-71 (ie, LSB)
#define  _TS_STAT_SRAM_RD_CTRL_RDV     _BN(8)              //  Read Valid Bit for SRAM back-door read

// Common Bit definitions for Receiver Errors's (NRE+0 thru NRE+5 and NREM+0 thru NREM+5)
#define  _TS_NRE_REC_REQCOMB           _BN( 0)             //  Request Combine Unit
#define  _TS_NRE_REC_TOKREL            _BN( 1)             //  Token Release Combine Unit
#define  _TS_NRE_REC_RECVRTE           _BN( 2)             //  Receiver Route
#define  _TS_NRE_REC_CHANSEL_ORDER     _BN( 3)             //  Channel Selector: No Strict Ordering
#define  _TS_NRE_REC_CHANSEL_GRANT     _BN( 4)             //  Channel Selector: >1 Grant
#define  _TS_NRE_REC_VCD0_CHANARB      _BN( 5)             //  VCD0 Channel Arbiter: Exactly 1 Grant
#define  _TS_NRE_REC_VCD0_STATE_GRANT  _BN( 6)             //  VCD0 Channel Arbiter FSM: Grant != Req
#define  _TS_NRE_REC_VCD0_STATE_UNK    _BN( 7)             //  VCD0 Channel Arbiter FSM: Unknown State
#define  _TS_NRE_REC_VCD1_CHANARB      _BN( 8)             //  VCD1 Channel Arbiter: Exactly 1 Grant
#define  _TS_NRE_REC_VCD1_STATE_GRANT  _BN( 9)             //  VCD1 Channel Arbiter FSM: Grant != Req
#define  _TS_NRE_REC_VCD1_STATE_UNK    _BN(10)             //  VCD1 Channel Arbiter FSM: Unknown State
#define  _TS_NRE_REC_VCBN_CHANARB      _BN(11)             //  VCBN Channel Arbiter: Exactly 1 Grant
#define  _TS_NRE_REC_VCBN_STATE_GRANT  _BN(12)             //  VCBN Channel Arbiter FSM: Grant != Req
#define  _TS_NRE_REC_VCBN_STATE_UNK    _BN(13)             //  VCBN Channel Arbiter FSM: Unknown State
#define  _TS_NRE_REC_VCBP_CHANARB      _BN(14)             //  VCBP Channel Arbiter: Exactly 1 Grant
#define  _TS_NRE_REC_VCBP_STATE_GRANT  _BN(15)             //  VCBP Channel Arbiter FSM: Grant != Req
#define  _TS_NRE_REC_VCBP_STATE_UNK    _BN(16)             //  VCBP Channel Arbiter FSM: Unknown State
#define  _TS_NRE_REC_BYP_CHANARB       _BN(17)             //  Bypass Channel Arbiter: Exactly 1 Grant
#define  _TS_NRE_REC_BYP_STATE_GRANT   _BN(18)             //  Bypass Channel Arbiter FSM: Grant != Req
#define  _TS_NRE_REC_BYP_STATE_UNK     _BN(19)             //  Bypass Channel Arbiter FSM: Unknown State
#define  _TS_NRE_REC_VCINPUT_PARITY    _BN(20)             //  VC-Fifo Input Data Bus Parity
#define  _TS_NRE_REC_VCCAPTURE_PARITY  _BN(21)             //  VC-Fifo Input Data Capture Latch
#define  _TS_NRE_REC_VCBPDL_PARITY     _BN(22)             //  VCBP Data Latch Parity
#define  _TS_NRE_REC_VCBNDL_PARITY     _BN(23)             //  VCBN Data Latch Parity
#define  _TS_NRE_REC_VCD1DL_PARITY     _BN(24)             //  VCD1 Data Latch Parity
#define  _TS_NRE_REC_VCD0DL_PARITY     _BN(25)             //  VCD0 Data Latch Parity
#define  _TS_NRE_REC_VCBPDL_ECC        _BN(26)             //  VCBP Uncorrectable ECC Error
#define  _TS_NRE_REC_VCBNDL_ECC        _BN(27)             //  VCBN Uncorrectable ECC Error
#define  _TS_NRE_REC_VCD1DL_ECC        _BN(28)             //  VCD1 Uncorrectable ECC Error
#define  _TS_NRE_REC_VCD0DL_ECC        _BN(29)             //  VCD0 Uncorrectable ECC Error
#define  _TS_NRE_REC_RECVFIFO_ENABLE   _BN(30)             //  >1 Receive Fifo Enabled
#define  _TS_NRE_REC_RECVPIPE_UNK      _BN(31)             //  Receive Pipe Unknown State

// Torus Non-Recoverable Errors
#define DCR_TS_NRE_RXP               (DCR_TS_NRE + 0x00)   // NRE: X+ Receiver Errors
#define DCR_TS_NRE_RXM               (DCR_TS_NRE + 0x01)   // NRE: X- Receiver Errors
#define DCR_TS_NRE_RYP               (DCR_TS_NRE + 0x02)   // NRE: Y+ Receiver Errors
#define DCR_TS_NRE_RYM               (DCR_TS_NRE + 0x03)   // NRE: Y- Receiver Errors
#define DCR_TS_NRE_RZP               (DCR_TS_NRE + 0x04)   // NRE: Z+ Receiver Errors
#define DCR_TS_NRE_RZM               (DCR_TS_NRE + 0x05)   // NRE: Z- Receiver Errors
#define DCR_TS_NRE_SHDR_CHK          (DCR_TS_NRE + 0x06)   // NRE: Sender Header Check Error
#define  _TS_NRE_SHDR_CHK_XP_HINT     _BN( 0)              //  X+: +/- Hint Bits Simultaneously Set Error
#define  _TS_NRE_SHDR_CHK_XP_NEIGH    _BN( 1)              //  X+: Destination Neighbor vis-a-vis Hint Error
#define  _TS_NRE_SHDR_CHK_XP_DYNVC    _BN( 2)              //  X+: Dynamic and VC not Consistent
#define  _TS_NRE_SHDR_CHK_XP_DEST     _BN( 3)              //  X+: Destination out of Range
#define  _TS_NRE_SHDR_CHK_XP_DEP      _BN( 4)              //  X+: Deposit Bit Bad Destination
#define  _TS_NRE_SHDR_CHK_XM_HINT     _BN( 5)              //  X-: +/- Hint Bits Simultaneously Set Error
#define  _TS_NRE_SHDR_CHK_XM_NEIGH    _BN( 6)              //  X-: Destination Neighbor vis-a-vis Hint Error
#define  _TS_NRE_SHDR_CHK_XM_DYNVC    _BN( 7)              //  X-: Dynamic and VC not Consistent
#define  _TS_NRE_SHDR_CHK_XM_DEST     _BN( 8)              //  X-: Destination out of Range
#define  _TS_NRE_SHDR_CHK_XM_DEP      _BN( 9)              //  X-: Deposit Bit Bad Destination
#define  _TS_NRE_SHDR_CHK_YP_HINT     _BN(10)              //  Y+: +/- Hint Bits Simultaneously Set Error
#define  _TS_NRE_SHDR_CHK_YP_NEIGH    _BN(11)              //  Y+: Destination Neighbor vis-a-vis Hint Error
#define  _TS_NRE_SHDR_CHK_YP_DYNVC    _BN(12)              //  Y+: Dynamic and VC not Consistent
#define  _TS_NRE_SHDR_CHK_YP_DEST     _BN(13)              //  Y+: Destination out of Range
#define  _TS_NRE_SHDR_CHK_YP_DEP      _BN(14)              //  Y+: Deposit Bit Bad Destination
#define  _TS_NRE_SHDR_CHK_YM_HINT     _BN(15)              //  Y-: +/- Hint Bits Simultaneously Set Error
#define  _TS_NRE_SHDR_CHK_YM_NEIGH    _BN(16)              //  Y-: Destination Neighbor vis-a-vis Hint Error
#define  _TS_NRE_SHDR_CHK_YM_DYNVC    _BN(17)              //  Y-: Dynamic and VC not Consistent
#define  _TS_NRE_SHDR_CHK_YM_DEST     _BN(18)              //  Y-: Destination out of Range
#define  _TS_NRE_SHDR_CHK_YM_DEP      _BN(19)              //  Y-: Deposit Bit Bad Destination
#define  _TS_NRE_SHDR_CHK_ZP_HINT     _BN(20)              //  Z+: +/- Hint Bits Simultaneously Set Error
#define  _TS_NRE_SHDR_CHK_ZP_NEIGH    _BN(21)              //  Z+: Destination Neighbor vis-a-vis Hint Error
#define  _TS_NRE_SHDR_CHK_ZP_DYNVC    _BN(22)              //  Z+: Dynamic and VC not Consistent
#define  _TS_NRE_SHDR_CHK_ZP_DEST     _BN(23)              //  Z+: Destination out of Range
#define  _TS_NRE_SHDR_CHK_ZP_DEP      _BN(24)              //  Z+: Deposit Bit Bad Destination
#define  _TS_NRE_SHDR_CHK_ZM_HINT     _BN(25)              //  Z-: +/- Hint Bits Simultaneously Set Error
#define  _TS_NRE_SHDR_CHK_ZM_NEIGH    _BN(26)              //  Z-: Destination Neighbor vis-a-vis Hint Error
#define  _TS_NRE_SHDR_CHK_ZM_DYNVC    _BN(27)              //  Z-: Dynamic and VC not Consistent
#define  _TS_NRE_SHDR_CHK_ZM_DEST     _BN(28)              //  Z-: Destination out of Range
#define  _TS_NRE_SHDR_CHK_ZM_DEP      _BN(29)              //  Z-: Deposit Bit Bad Destination
#define DCR_TS_NRE_SPAR_SRAM         (DCR_TS_NRE + 0x07)   // NRE: Sender Parity and SRAM ECC Double-Bit
#define  _TS_NRE_SPAR_SRAM_XP         _BN( 0)              //  X+ Sender Parity Error
#define  _TS_NRE_SPAR_SRAM_XM         _BN( 1)              //  X- Sender Parity Error
#define  _TS_NRE_SPAR_SRAM_YP         _BN( 2)              //  Y+ Sender Parity Error
#define  _TS_NRE_SPAR_SRAM_YM         _BN( 3)              //  Y- Sender Parity Error
#define  _TS_NRE_SPAR_SRAM_ZP         _BN( 4)              //  Z+ Sender Parity Error
#define  _TS_NRE_SPAR_SRAM_ZM         _BN( 5)              //  Z- Sender Parity Error
#define  _TS_NRE_SPAR_SRAM_R0         _BN( 6)              //  Resend Fifo SRAM 0 DBE
#define  _TS_NRE_SPAR_SRAM_R1         _BN( 7)              //  Resend Fifo SRAM 1 DBE
#define DCR_TS_NRE_INJ_FIFO_G0       (DCR_TS_NRE + 0x08)   // NRE: Injection Fifos Group 0
#define  _TS_NRE_INJ_FIFO_G0_N0(x)    _B8( 7,x)            //  Process Injection Normal 0
#define  _TS_NRE_INJ_FIFO_G0_N1(x)    _B8(15,x)            //  Process Injection Normal 1
#define  _TS_NRE_INJ_FIFO_G0_N2(x)    _B8(23,x)            //  Process Injection Normal 2
#define  _TS_NRE_INJ_FIFO_G0_P0(x)    _B8(31,x)            //  Process Injection Priority 0
#define DCR_TS_NRE_INJ_FIFO_G1       (DCR_TS_NRE + 0x09)   // NRE: Injection Fifos Group 1
#define  _TS_NRE_INJ_FIFO_G1_N3(x)    _B8( 7,x)            //  Process Injection Normal 3
#define  _TS_NRE_INJ_FIFO_G1_N4(x)    _B8(15,x)            //  Process Injection Normal 4
#define  _TS_NRE_INJ_FIFO_G1_N5(x)    _B8(23,x)            //  Process Injection Normal 5
#define  _TS_NRE_INJ_FIFO_G1_P1(x)    _B8(31,x)            //  Process Injection Priority 1
#define DCR_TS_NRE_INJ_MEM           (DCR_TS_NRE + 0x0A)   // NRE: Injection Memory and Latch Errors
#define  _TS_NRE_INJ_MEM_SRAM0         _BN( 0)             //  ECC Error in SRAM 0
#define  _TS_NRE_INJ_MEM_SRAM1         _BN( 1)             //  ECC Error in SRAM 1
#define  _TS_NRE_INJ_MEM_SRAM0DL       _BN( 2)             //  Parity Error in SRAM 0 Data Latches
#define  _TS_NRE_INJ_MEM_SRAM1DL       _BN( 3)             //  Parity Error in SRAM 1 Data Latches
#define  _TS_NRE_INJ_MEM_P0DL          _BN( 4)             //  Parity Error in P0 Data Latches
#define  _TS_NRE_INJ_MEM_P1DL          _BN( 5)             //  Parity Error in P1 Data Latches
#define  _TS_NRE_INJ_MEM_N0DL          _BN( 6)             //  Parity Error in N0 Data Latches
#define  _TS_NRE_INJ_MEM_N1DL          _BN( 7)             //  Parity Error in N1 Data Latches
#define  _TS_NRE_INJ_MEM_N2DL          _BN( 8)             //  Parity Error in N2 Data Latches
#define  _TS_NRE_INJ_MEM_N3DL          _BN( 9)             //  Parity Error in N3 Data Latches
#define  _TS_NRE_INJ_MEM_N4DL          _BN(10)             //  Parity Error in N4 Data Latches
#define  _TS_NRE_INJ_MEM_N5DL          _BN(11)             //  Parity Error in N5 Data Latches
#define DCR_TS_NRE_REC_MEM           (DCR_TS_NRE + 0x0B)   // NRE: Reception Memory and Latch Errors
#define  _TS_NRE_REC_MEM_SRAM0         _BN( 0)             //  ECC Error in SRAM 0
#define  _TS_NRE_REC_MEM_SRAM1         _BN( 1)             //  ECC Error in SRAM 1
#define  _TS_NRE_REC_MEM_N0DL          _BN( 2)             //  Parity Error in Fifo N0 Data Latches
#define  _TS_NRE_REC_MEM_N1DL          _BN( 3)             //  Parity Error in Fifo N1 Data Latches
#define  _TS_NRE_REC_MEM_N2DL          _BN( 4)             //  Parity Error in Fifo N2 Data Latches
#define  _TS_NRE_REC_MEM_N3DL          _BN( 5)             //  Parity Error in Fifo N3 Data Latches
#define  _TS_NRE_REC_MEM_N4DL          _BN( 6)             //  Parity Error in Fifo N4 Data Latches
#define  _TS_NRE_REC_MEM_N5DL          _BN( 7)             //  Parity Error in Fifo N5 Data Latches
#define  _TS_NRE_REC_MEM_P0DL          _BN( 8)             //  Parity Error in Fifo P0 Data Latches
#define  _TS_NRE_REC_MEM_P1DL          _BN( 9)             //  Parity Error in Fifo P1 Data Latches
#define  _TS_NRE_REC_MEM_SRAM0DL       _BN(10)             //  Parity Error in SRAM 0 Data Latches
#define  _TS_NRE_REC_MEM_SRAM1DL       _BN(11)             //  Parity Error in SRAM 1 Data Latches
#define  _TS_NRE_REC_MEM_PU0DL0        _BN(12)             //  Parity Error in PU0 Data Latch 0
#define  _TS_NRE_REC_MEM_PU0DL1        _BN(13)             //  Parity Error in PU0 Data Latch 1
#define  _TS_NRE_REC_MEM_PU1DL0        _BN(14)             //  Parity Error in PU1 Data Latch 0
#define  _TS_NRE_REC_MEM_PU1DL1        _BN(15)             //  Parity Error in PU1 Data Latch 1
#define DCR_TS_NRE_DCR               (DCR_TS_NRE + 0x0C)   // NRE: DCR Errors
#define  _TS_NRE_DCR_CTRL_PAR          _BN(0)              //  Control Registers Parity Error
#define  _TS_NRE_DCR_THR_PAR           _BN(1)              //  Threshold Registers Parity Error
#define  _TS_NRE_DCR_NREM_PAR          _BN(2)              //  Non-Recoverable Mask Registers Parity Error
#define  _TS_NRE_DCR_REM_PAR           _BN(3)              //  Recoverable Mask Registers Parity Error
#define  _TS_NRE_DCR_P0_READ_EMPTY     _BN(4)              //  Processor 0 Tried to Read from Empty Fifo.
#define  _TS_NRE_DCR_P1_READ_EMPTY     _BN(5)              //  Processor 1 Tried to Read from Empty Fifo.
#define  _TS_NRE_DCR_WRITE_FULL        _BN(6)              //  Attempt to write to Full Fifo.

// Torus Non-Recoverable Error Masks (bits defined as in NRE above)
#define DCR_TS_NREM_RXP              (DCR_TS_NREM + 0x00)  // NREM: X+ Receiver Error Mask
#define DCR_TS_NREM_RXM              (DCR_TS_NREM + 0x01)  // NREM: X- Receiver Error Mask
#define DCR_TS_NREM_RYP              (DCR_TS_NREM + 0x02)  // NREM: Y+ Receiver Error Mask
#define DCR_TS_NREM_RYM              (DCR_TS_NREM + 0x03)  // NREM: Y- Receiver Error Mask
#define DCR_TS_NREM_RZP              (DCR_TS_NREM + 0x04)  // NREM: Z+ Receiver Error Mask
#define DCR_TS_NREM_RZM              (DCR_TS_NREM + 0x05)  // NREM: Z- Receiver Error Mask
#define DCR_TS_NREM_SHDR_CHK         (DCR_TS_NREM + 0x06)  // NREM: Sender Header Check Error  Mask
#define DCR_TS_NREM_SPAR_SRAM        (DCR_TS_NREM + 0x07)  // NREM: Sender Parity and SRAM ECC Double-Bit Mask
#define DCR_TS_NREM_INJ_FIFO_G0      (DCR_TS_NREM + 0x08)  // NREM: Injection Fifos Group 0 Mask
#define DCR_TS_NREM_INJ_FIFO_G1      (DCR_TS_NREM + 0x09)  // NREM: Injection Fifos Group 1 Mask
#define DCR_TS_NREM_INJ_MEM          (DCR_TS_NREM + 0x0A)  // NREM: Injection Memory and Latch Error Mask
#define DCR_TS_NREM_REC_MEM          (DCR_TS_NREM + 0x0B)  // NREM: Reception Memory and Latch Error Mask
#define DCR_TS_NREM_DCR              (DCR_TS_NREM + 0x0C)  // NREM: DCR Error Mask

// Torus Recoverable Error Counters. Access is RC/W.
#define DCR_TS_RE_INPUT_XP           (DCR_TS_RE + 0x00)    // RE: Receiver Input Pipe Errors X+
#define DCR_TS_RE_INPUT_XM           (DCR_TS_RE + 0x01)    // RE: Receiver Input Pipe Errors X-
#define DCR_TS_RE_INPUT_YP           (DCR_TS_RE + 0x02)    // RE: Receiver Input Pipe Errors Y+
#define DCR_TS_RE_INPUT_YM           (DCR_TS_RE + 0x03)    // RE: Receiver Input Pipe Errors Y-
#define DCR_TS_RE_INPUT_ZP           (DCR_TS_RE + 0x04)    // RE: Receiver Input Pipe Errors Z+
#define DCR_TS_RE_INPUT_ZM           (DCR_TS_RE + 0x05)    // RE: Receiver Input Pipe Errors Z-
#define DCR_TS_RE_SEND_SRAM_SBE      (DCR_TS_RE + 0x06)    // RE: Sender Single Bit Errors in SRAM 0,1
#define DCR_TS_RE_VCFIFO_SRAM_SBE    (DCR_TS_RE + 0x07)    // RE: Receiver VC Fifo SRAM Single Bit Errors
#define DCR_TS_RE_SEND_RETRY_XP      (DCR_TS_RE + 0x08)    // RE: Sender Retry Errors X+
#define DCR_TS_RE_SEND_RETRY_XM      (DCR_TS_RE + 0x09)    // RE: Sender Retry Errors X-
#define DCR_TS_RE_SEND_RETRY_YP      (DCR_TS_RE + 0x0A)    // RE: Sender Retry Errors Y+
#define DCR_TS_RE_SEND_RETRY_YM      (DCR_TS_RE + 0x0B)    // RE: Sender Retry Errors Y-
#define DCR_TS_RE_SEND_RETRY_ZP      (DCR_TS_RE + 0x0C)    // RE: Sender Retry Errors Z+
#define DCR_TS_RE_SEND_RETRY_ZM      (DCR_TS_RE + 0x0D)    // RE: Sender Retry Errors Z-
#define DCR_TS_RE_RAND               (DCR_TS_RE + 0x0E)    // RE: Corrected Random Number Generator Errors
#define DCR_TS_RE_INJ_SRAM_SBE       (DCR_TS_RE + 0x0F)    // RE: Injection Single Bit Errors in all Fifo SRAMs
#define DCR_TS_RE_REC_SRAM_SBE       (DCR_TS_RE + 0x10)    // RE: Injection Single Bit Errors in all Fifo SRAMs
#define DCR_TS_RE_BADHDR_FULLFIFO    (DCR_TS_RE + 0x11)    // RE: Counts Bad Packet Header Check or Attempt to Write to Full Fifo
#define DCR_TS_RE_VCFIFO_OVERFLOW    (DCR_TS_RE + 0x12)    // RE: Counts dropped chunks due to attempt to write to full fifo (header CRC escape)

// Torus Recoverable Error Masks
#define DCR_TS_REM_MASKS             (DCR_TS_REM + 0x00)   // REM: Recoverable Error Masks
#define DCR_TS_REM_Resvd             (DCR_TS_REM + 0x01)   // REM: Reserved

// 0000 0000 0011 1111 1111 2222 2222 2233 | 0000 0000 0011 1111 1111 2222 2222 2233
// 0123 4567 8901 2345 6789 0123 4567 8901 | 0123 4567 8901 2345 6789 0123 4567 8901
// CCCC CCCS HHHH HHDP SSSU UDVV XXXX XXXX | YYYY YYYY ZZZZ ZZZZ RRRR RRRR RRRR RRRR

#define _TS_PKT_MAX  (256)

// Describe the Torus Packet Hardware header
#define _TS_HWH0_CSUM_SKIP(x)       _B7( 6,x)
#define _TS_HWH0_SK(x)              _BN( 7)
#define _TS_HWH0_HINT(x)            _B6(13,x)              // Hint bits: all at once
#define _TS_HWH0_HINT_XP            _BN( 8)                //  or individually...
#define _TS_HWH0_HINT_XM            _BN( 9)
#define _TS_HWH0_HINT_YP            _BN(10)
#define _TS_HWH0_HINT_YM            _BN(11)
#define _TS_HWH0_HINT_ZP            _BN(12)
#define _TS_HWH0_HINT_ZM            _BN(13)
#define _TS_HWH0_DP                 _BN(14)
#define _TS_HWH0_PID                _BN(15)
#define _TS_HWH0_CHUNKS(x)          _B3(18,x)
#define _TS_HWH0_SW_AVAIL(x)        _B2(20,x)
#define _TS_HWH0_DYNAMIC            _BN(21)
#define _TS_HWH0_VC(x)              _B2(23,x)
#define _TS_HWH0_VC_D0              _TS_HWH0_VC(0)
#define _TS_HWH0_VC_D1              _TS_HWH0_VC(1)
#define _TS_HWH0_VC_BN              _TS_HWH0_VC(2)
#define _TS_HWH0_VC_BP              _TS_HWH0_VC(3)
#define _TS_HWH0_X(x)               _B8(31,x)

#define _TS_HWH1_Y(x)               _B8(  7,x)
#define _TS_HWH1_Z(x)               _B8( 15,x)
#define _TS_HWH1_R(x)               _B16(31,x)

#define _TS_SWH_FCN                 ( 8)    // offset of s/w fcn in pkt hdr
#define _TS_SWH_ARG                 (12)    // offset of s/w arg in pkt hdr

#ifndef __ASSEMBLY__

__BEGIN_DECLS

// Extract and Right-Justify Torus Packet Hardware header fields
#define _GET_TS_HWH0_CSUM_SKIP(x)   _G7(x, 6)
#define _GET_TS_HWH0_SK(x)          _GN(x, 7)
#define _GET_TS_HWH0_HINT(x)        _G6(x,13)              // Hint bits: all at once
#define _GET_TS_HWH0_HINT_XP(x)     _GN(x, 8)              //  or individually...
#define _GET_TS_HWH0_HINT_XM(x)     _GN(x, 9)
#define _GET_TS_HWH0_HINT_YP(x)     _GN(x,10)
#define _GET_TS_HWH0_HINT_YM(x)     _GN(x,11)
#define _GET_TS_HWH0_HINT_ZP(x)     _GN(x,12)
#define _GET_TS_HWH0_HINT_ZM(x)     _GN(x,13)
#define _GET_TS_HWH0_DP(x)          _GN(x,14)
#define _GET_TS_HWH0_PID(x)         _GN(x,15)
#define _GET_TS_HWH0_CHUNKS(x)      _G3(x,18)
#define _GET_TS_HWH0_SW_AVAIL(x)    _G2(x,20)
#define _GET_TS_HWH0_DYNAMIC(x)     _GN(x,21)
#define _GET_TS_HWH0_VC(x)          _G2(x,23)
#define _GET_TS_HWH0_X(x)           _G8(x,31)

#define _GET_TS_HWH1_Y(x)           _G8(x, 7)
#define _GET_TS_HWH1_Z(x)           _G8(x,15)
#define _GET_TS_HWH1_R(x)           _G16(x,31)

#if defined( CONFIG_BLNIE )  // Use the newer SPI definitions

//@MG: the user code segment is virtually located at 0x00000000 thru 0x00FFFFFF
// This means that 8+2 bits of a 32 bit function address are available for use
//  as an extra argument of 10 bits to the active message receive functions.
#define SET_FCN( fcn )     ((((Bit32)(fcn  )) & 0x00FFFFFC) >> 2)
#define SET_EXTRA( extra ) ((((Bit32)(extra)) & 0x000003FF) << 22)
#define SET_FCN_AND_EXTRA( fcn, extra ) (SET_EXTRA((extra)) | SET_FCN((fcn)))

//@MG: The upper 8 bits of the Active Function Pointer is set to the
//      upper 8 bits of the application's entry point.
#define _TS_GET_FCN( w0 )   ((((w0) & 0x003FFFFF) <<  2) | _Blade_Config_Area.AppCodeSeg)
#define _TS_GET_EXTRA( w0 )  (((w0) & 0xFFC00000) >> 22)


typedef struct T_TS_HdrHW
                {
                Bit32          w[2];
                }
                _TS_HdrHW;

typedef struct T_TS_HdrSW
                {
                Bit32          w[2];
                }
                _TS_HdrSW;

typedef struct T_TS_Hdr
                {
                _TS_HdrHW hwh;
                _TS_HdrSW swh;
                }
                ALIGN_QUADWORD _TS_Hdr;

typedef struct T_TS_Pkt
                {
                _TS_Hdr hdr;
                Bit8    payload[ _TS_PKT_MAX - sizeof(_TS_Hdr) ];
                }
                ALIGN_QUADWORD _TS_Pkt;


#else // not BLNIE (this will go away soon)

typedef void _TS_ActiveMessageHandler( Bit32  *fifo,
                                       Bit32  arg,         // 32 bits of primary arg info
                                       Bit32  extra,       // 10 bits of extra arg info
                                       double pkt_quad1,   // quad0 is hw+sw hdr, this is 2nd quad
                                       double pkt_quad2,   // second chunk's 1st quad (3rd of pkt)
                                       double pkt_quad3 ); // second chunk's 2nd quad (4th of pkt)

typedef struct T_TS_HdrHW_Bits
                {
                unsigned CSum_Skip  : 7; // Number of shorts (2B) to skip in CheckSum.
                unsigned Sk         : 1; // 0=use CSum_Skip, 1=Skip entire pkt.

                unsigned Hint       : 6; // Hint Bits
                unsigned Dp         : 1; // Deposit Bit for MultiCast
                unsigned Pid        : 1; // Destination Fifo Group

                unsigned Chunks     : 3; // Size in Chunks of 32B
                unsigned SW_Avail   : 2; // Available for S/W Use.
                unsigned Dynamic    : 1; // 1=Dynamic Routing, 0=Deterministic Routing.
                unsigned VC         : 2; // Virtual Channel (0=D0,1=D1,2=BN,3=BP)

                unsigned X          : 8; // Destination X

                unsigned Y          : 8; // Destination Y
                unsigned Z          : 8; // Destination Z
                unsigned Resvd1     : 8; // Reserved
                unsigned Resvd2     : 8; // Reserved
                }
                _TS_HdrHW_Bits;

typedef union T_TS_HdrHW
               {
               _TS_HdrHW_Bits b;
               Bit32          w[2];
               Bit64          d;
               }
               _TS_HdrHW;

//@MG: the user code segment is virtually located at 0x00000000 thru 0x00FFFFFF
// This means that 8+2 bits of a 32 bit function address are available for use
//  as an extra argument of 10 bits to the active message receive functions.
#define SET_FCN( fcn )     (((Bit32)(fcn) & 0x00FFFFFC) >> 2)
#define SET_EXTRA( extra ) _B10(9,(extra))
#define SET_FCN_AND_EXTRA( fcn, extra ) (SET_EXTRA((extra)) | SET_FCN((fcn)))

typedef struct T_TS_HdrSW_Bits
                {
                unsigned extra : 10;
                unsigned fcn   : 22;
                Bit32    arg;
                }
                _TS_HdrSW_Bits;

typedef union T_TS_HdrSW
                {
                _TS_HdrSW_Bits b;
                Bit32          w[2];
                Bit64          d;
                }
                _TS_HdrSW;

typedef struct T_TS_Hdr
                {
                _TS_HdrHW hwh;
                _TS_HdrSW swh;
                }
                ALIGN_QUADWORD _TS_Hdr;

typedef struct T_TS_Pkt
                {
                _TS_Hdr hdr;
                Bit8    payload[ _TS_PKT_MAX - sizeof(_TS_Hdr) ];
                }
                ALIGN_QUADWORD _TS_Pkt;

#endif // CONFIG_BLNIE

typedef struct T_TS_Stat
                {
                Bit8 r0,r1,r2,r3,r4,r5,rH; // Reception Fifo Status in Chunks (32B)
                Bit8 i0,i1,i2,iH;          // Injection Fifo Status in Chunks (32B)
                Bit8 pad[5];               // Pad to 16B.
                }
                ALIGN_QUADWORD _TS_Stat;


extern int _blade_torus_init( int loopback ); // see kernel/bl_torus.c
extern int _blade_torus_release( void );      // see kernel/bl_torus.c

// Power-Off the Torus on I/O Nodes. See kernel/bl_torus.c, and bl_test.h.
extern void _blade_torus_PowerOff( void );


// see bl_torus_pkt.S
extern void _ts_put_quads_mid( Bit32 *fifo, _TS_Hdr *hdr, Bit32 src_addr );

extern void _ts0_pkt_recv( Bit128 *fifo );

extern void _ts1_pkt_recv( Bit128 *fifo );


__END_DECLS

#endif // __ASSEMBLY__

#endif // Add nothing below this line.
