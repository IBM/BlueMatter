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
//      File: blade/include/sys/machine/bl_dcrmap.h
//
//   Purpose: Define the Device Control Register Map
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes:
//
//   History: 05/01/2002: MEG - Created for new DCR map.
//
//
#ifndef _BL_DCRMAP_H // Prevent multiple inclusion
#define _BL_DCRMAP_H

//
// DCR Number Base Addresses for the various units
//
#define DCR_IC        (0x000)     // Basic Interrupt Controller Base DCR Address

#define DCR_PLB       (0x090)     // PLB Arbiter.  See bl_plb.h.

#define DCR_FPU       (0x0A0)     // Floating Point Unit for This Core, Other Core at +8

#define DCR_OPB       (0x0B0)     // PLB-OPB Bridge
#define DCR_TEST      (0x0C0)     // Test Interface
#define DCR_LOCKBOX   (0x100)     // LockBox
#define DCR_PLBSLAVE  (0x110)     // PLB Slave
#define DCR_SRAM      (0x120)     // SRAM  See bl_sram.h.
#define DCR_L3        (0x140)     // L3

#define DCR_MCCU      (0x160)     // Machine Check Control Unit for This Core, Other Core at +0x10
#define DCR_L2        (0x164)     // L2 Cache Config and Control Registers, Other L2 at +0x10

#define DCR_UPC       (0x180)     // Universal Performance Counters.  See bl_upc.h.
#define DCR_TREE      (0x1C0)     // Tree
#define DCR_TRCU      (0x220)     // Capture Unit for Tree
#define DCR_TSCU      (0x250)     // Capture Unit for Torus
#define DCR_TORUS     (0x280)     // Torus
#define DCR_MCMAL8    (0x300)     // McMal8
#define DCR_DDR       (0x380)     // DDR Controller

#endif // Add nothing below this line.
