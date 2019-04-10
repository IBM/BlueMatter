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
//      File: blade/include/sys/machine/bl_physmap.h
//
//   Purpose: Define BG/L's physical memory map.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes:
//
//   History: 05/01/2002: MEG - Created for new memory map.
//
#ifndef _BL_PHYSMAP_H // Prevent multiple inclusion
#define _BL_PHYSMAP_H

#ifndef __ASSEMBLY__

#define _PA_IsDDR(pa)       (((unsigned long)(pa)) <= ((unsigned long)PM_DDR))
#define _PA_IsScratch(pa)   ((((unsigned long)(pa)) & 0xF0000000) == (unsigned long)PA_SCRATCH)
#define _PA_IsSRAM(pa)      (((unsigned long)(pa)) >= ((unsigned long)PA_SRAM))

#endif // not __ASSEMBLY__

//
// Note:
//   PA_ = Physical Address Lower 32 Address Bits
//   PS_ = Physical Size in bytes
//   PM_ = Mask to check that Physical Address is in range
//

#define PA_DDR          (0x00000000)         // DDR Base Physical Address
#define PS_DDR_CN       (256 * MEGA)         // DDR Size (Compute Node)
#define PS_DDR_IN       (512 * MEGA)         // DDR Size (I/O Node)
#define PS_DDR_MAX      (  2 * GIGA)         // Maximum Supported DDR Size
#define PM_DDR          (0x7FFFFFFF)

#define PA_SCRATCH      (0x80000000)         // Scratch Base Physical Address
#define PS_SCRATCH      (  4 * MEGA)         // Scratch Max Size
#define PM_SCRATCH      (PA_SCRATCH | (PS_SCRATCH-1))

#define PA_TORUS_0      (0xB1140000)         // Torus Group 0 Base Physical Address
#define PA_TORUS_1      (0xB1150000)         // Torus Group 1 Base Physical Address

#define PA_TREE_VC0     (0xB0000000)         // Tree VC0 Base Physical Address
#define PA_TREE_VC1     (0xB1000000)         // Tree VC1 Base Physical Address

#define PA_GI_SYS       (0xB0100000)         // Global Interrupts System Base Physical Address
#define PA_GI_USR       (0xB1100000)         // Global Interrupts User   Base Physical Address

#define PA_UPC          (0xB0010000)         // Universal Performance Counters Base Physical Address

#define PA_BLIND        (0xC0000000)         // Blind Device Base Physical Address (MAPPED V=R)
#define PS_BLIND        (256 * MEGA)         // Blind Device Size
#define PA_BLIND_NORM   (0xC0000000)         //  "Normal" Area of Blind Device (User+Sup RW Normal)
#define PS_BLIND_NORM   ( 16 * MEGA)         //    16MB for Normal Area
#define PA_BLIND_TRANS  (0xC1000000)         //  "Transient" Area of Blind Device (User+Sup RW Transient)
#define PS_BLIND_TRANS  ( 16 * MEGA)         //    16MB for Transient Area

#define PA_LOCKBOX      (0xD0000000)         // LockBox Base Physical Address
#define PS_LOCKBOX      (256 * MEGA)         // LockBox Size

#define PA_OPBARB       (0xE0000600)         // PLB-OPB Arbitor Base Physical Address
#define PA_EMAC4        (0xE0000800)         // EMAC4 Base Physical Address

#define PA_SRAM         (0xFFFFC000)         // SRAM Base Physical Address
#define PS_SRAM         ( 16 * KILO)         // SRAM Size

#endif // Add nothing below this line.

