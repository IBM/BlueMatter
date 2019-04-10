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
 /*********************************************************************/
/*                        I.B.M. CONFIDENTIAL                        */
/*      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      */
/*                                                                   */
/*                LICENSED MATERIALS-PROPERTY OF IBM.                */
/*                                                                   */
/*********************************************************************/

/*
 * This file contains the register definitions specific to the power pc 440 core.
 */

#ifndef _PPC440_CORE_H
#define _PPC440_CORE_H

// make sure we have one of these...
#ifndef _BN
#define _BN(b) ((1<<(31-(b))))
#define _B1(b,x)  (((x)&0x1)<<(31-(b)))
#define _B2(b,x) (((x)&0x3)<<(31-(b)))
#define _B3(b,x) (((x)&0x7)<<(31-(b)))
#define _B4(b,x) (((x)&0xF)<<(31-(b)))
#define _B5(b,x) (((x)&0x1F)<<(31-(b)))
#define _B6(b,x) (((x)&0x3F)<<(31-(b)))
#define _B7(b,x) (((x)&0x7F)<<(31-(b)))
#define _B8(b,x) (((x)&0xFF)<<(31-(b)))
#define _B9(b,x) (((x)&0x1FF)<<(31-(b)))
#define _B10(b,x) (((x)&0x3FF)<<(31-(b)))
#define _B11(b,x) (((x)&0x7FF)<<(31-(b)))
#define _B12(b,x) (((x)&0xFFF)<<(31-(b)))
#define _B13(b,x) (((x)&0x1FFF)<<(31-(b)))
#define _B14(b,x) (((x)&0x3FFF)<<(31-(b)))
#define _B15(b,x) (((x)&0x7FFF)<<(31-(b)))
#define _B16(b,x) (((x)&0xFFFF)<<(31-(b)))
#define _B17(b,x) (((x)&0x1FFFF)<<(31-(b)))
#define _B18(b,x) (((x)&0x3FFFF)<<(31-(b)))
#define _B19(b,x) (((x)&0x7FFFF)<<(31-(b)))
#define _B20(b,x) (((x)&0xFFFFF)<<(31-(b)))
#define _B21(b,x) (((x)&0x1FFFFF)<<(31-(b)))
#define _B22(b,x) (((x)&0x3FFFFF)<<(31-(b)))
#define _B23(b,x) (((x)&0x7FFFFF)<<(31-(b)))
#define _B24(b,x) (((x)&0xFFFFFF)<<(31-(b)))
#define _B25(b,x) (((x)&0x1FFFFFF)<<(31-(b)))
#define _B26(b,x) (((x)&0x3FFFFFF)<<(31-(b)))
#define _B27(b,x) (((x)&0x7FFFFFF)<<(31-(b)))
#define _B28(b,x) (((x)&0xFFFFFFF)<<(31-(b)))
#define _B29(b,x) (((x)&0x1FFFFFFF)<<(31-(b)))
#define _B30(b,x) (((x)&0x3FFFFFFF)<<(31-(b)))
#define _B31(b,x) (((x)&0x7FFFFFFF)<<(31-(b)))

#endif


// PPC440 32K Level 1 I-Cache: Virtual Tag = A[0..22], Set = A[23..26]
#define L1I_CACHE_LINES           1024 // 64 ways of 16 sets per way
#define L1I_CACHE_LINE_SIZE         32 // 8 words
#define L1I_CACHE_ALIGN              5 // zero bits for 32 byte alignment
#define L1I_CACHE_MASK      0xFFFFFFE0 // mask for 32 byte alignment
#define L1I_CACHE_MAXFLUSH         512 // above this num lines, invalidate the whole thing
#define L1I_CACHE_BYTES    (L1I_CACHE_LINES * L1I_CACHE_LINE_SIZE)

// PPC440 32K Level 1 D-Cache: Physical Tag = A[0..22], Set = A[23..26]
#define L1D_CACHE_LINES           1024 // 64 ways of 16 sets per way
#define L1D_CACHE_SETS              16 // 64 ways of 16 sets per way
#define L1D_CACHE_WAYS              64 // 64 ways of 16 sets per way
#define L1D_CACHE_LINE_SIZE         32 // 8 words
#define L1D_CACHE_ALIGN              5 // zero bits for 32 byte alignment
#define L1D_CACHE_MASK      0xFFFFFFE0 // mask for 32 byte alignment
#define L1D_CACHE_MAXFLUSH         512 // above this num lines, use fast flush-assist
#define L1D_CACHE_BYTES    (L1D_CACHE_LINES * L1D_CACHE_LINE_SIZE)


// Make sure we don't collidde with any system definitions
#undef MSR_POW
#undef MSR_WE
#undef MSR_CE
#undef MSR_EE
#undef MSR_PR
#undef MSR_FP
#undef MSR_ME
#undef MSR_FE0
#undef MSR_SE
#undef MSR_DWE
#undef MSR_BE
#undef MSR_DE
#undef MSR_FE1
#undef MSR_IS
#undef MSR_DS

// Machine State Register (MSR) Bits (440_UM p.10-46)
#define MSR_POW        _BN(13)      // Enable Power Management
#define MSR_WE         _BN(13)      // Wait State Enable
#define MSR_CE         _BN(14)      // Critical Interrupt Enable
#define MSR_EE         _BN(16)      // External Interrupt Enable
#define MSR_PR         _BN(17)      // Problem State (0=Sup, 1=Usr)
#define MSR_FP         _BN(18)      // Floating Point Available/Enable
#define MSR_ME         _BN(19)      // Machine Check Enable
#define MSR_FE0        _BN(20)      // Floating-Point Exception Mode 0
#define MSR_SE         _BN(21)      // Single-Step Trace Enable
#define MSR_DWE        _BN(21)      // Debug Wait Enable
#define MSR_BE         _BN(22)      // Branch Trace Enable
#define MSR_DE         _BN(22)      // Debug Exception Enable
#define MSR_FE1        _BN(23)      // Floating Exception mode 1
#define MSR_IS         _BN(26)      // Instruction Address Space
#define MSR_DS         _BN(27)      // Data Address Space

#define MSR_DISABLED (MSR_ME | MSR_FP) // Enable MachineCheck and the FPU while initializing

#define MSR_KERNEL   (MSR_ME | MSR_FP | MSR_CE | MSR_EE | MSR_SE | MSR_BE | MSR_DE) // Normal Kernel Run-Mode
//
#define MSR_USER     (MSR_KERNEL | MSR_PR | MSR_IS | MSR_DS)

#define MSR_STD_INT_MASK ( MSR_EE | MSR_ME | MSR_FE0 | MSR_FE1 )
#define MSR_INT_MASK (MSR_STD_INT_MASK | MSR_CE | MSR_DE)


#define SPRN_CCR0        0x3B3      // Core Configuration Register
#define   CCR0_DSTG     _BN(10)     //  Disable Store Gathering
#define   CCR0_DAPUIB   _BN(11)     //  Disable APU Instruction Broadcast
#define   CCR0_DTB      _BN(16)     //  Disable Trace Broadcast
#define   CCR0_GICBT    _BN(17)     //  Guaranteed Instructiton Cache Block Touch
#define   CCR0_GDCBT    _BN(18)     //  Guaranteed Data Cache Block Touch
#define   CCR0_FLSTA    _BN(23)     //  Force Load/Store Alignment
#define   CCR0_ICSLC(x) _B2(29,x)   //  Instruction Cache Speculative Line Count (number of additional lines to fill on i-cache miss (0-3)
#define   CCR0_ICSLT(x) _B2(31,x)   //  Instruction Cache Speculative Line Threshold (number of in-order double words before abandonment

// DL: see PPC440G5V1 Errata 37 regarding CCR0_GICBT
// DL: see PPC440G5V1 Errata 38 regarding CCR0_ICSLC(1)
///#define   CCR0_INIT     (CCR0_GICBT | CCR0_GDCBT | CCR0_ICSLC(1))
   #define   CCR0_INIT     (CCR0_GDCBT | CCR0_ICSLC(0))

#define SPRN_CCR1        0x378      // Configuration Control Register 1 (Avenger Only)
#define   CCR1_ICDPEI(x) _B8( 7,x)  //   I-Cache Data  Parity Error Insertion
#define   CCR1_ICTPEI(x) _B2( 9,x)  //   I-Cache Tag   Parity Error Insertion
#define   CCR1_DCTPEI(x) _B2(11,x)  //   D-Cache Tag   Parity Error Insertion
#define   CCR1_DCDPEI    _BN(12)    //   D-Cache Data  Parity Error Insertion
#define   CCR1_DCUPEI    _BN(13)    //   D-Cache U-Bit Parity Error Insertion
#define   CCR1_DCMPEI    _BN(14)    //   D-Cache Modified-Bit Parity Error Insertion
#define   CCR1_FCOM      _BN(15)    //   Force Cache Operation Miss
#define   CCR1_MMUPEI(x) _B4(19,x)  //   Memory Management Unit Parity Error Insertion
#define   CCR1_FFF       _BN(20)    //   Force Full-line D-Cache Flush (even when 1/2 line is required)
#define   CCR1_Rsvd_DCPC _BN(23)    //   RESERVED/UNDOCUMENTED Disable Cache Parity Checking
#define   CCR1_TCS       _BN(24)    //   Timer Clock Select (Set to Zero!)

#define SPRN_CSRR0     0x03A        // Critical Save/Restore Register 0: Return Addr
#define SPRN_CSRR1     0x03B        // Critical Save/Restore Register 1: MSR

#define SPRN_MCSRR0    0x23A        // Machine Check Save/Restore Register 0: Return Addr
#define SPRN_MCSRR1    0x23B        // Machine Check Save/Restore Register 1: MSR

#define SPRN_MCSR      0x23C        // Machine Check Status Register (Read/WriteClear)
#define  MCSR_MCS       _BN(0)      //   Machine Check Summary
#define  MCSR_IPLBE     _BN(1)      //   Instruction PLB Error
#define  MCSR_DRPLBE    _BN(2)      //   Data Read  PLB Error
#define  MCSR_DWPLBE    _BN(3)      //   Data Write PLB Error
#define  MCSR_TLBE      _BN(4)      //   TLB Error
#define  MCSR_ICPE      _BN(5)      //   I-Cache Parity Error
#define  MCSR_DCSPE     _BN(6)      //   D-Cache Search Parity Error
#define  MCSR_DCFPE     _BN(7)      //   D-Cache Flush  Parity Error
#define  MCSR_IMCE      _BN(8)      //   Imprecise Machine Check Exception

#define SPRN_CTR       0x009        // Count Register
#define SPRN_DAC1      0x13C        // Data Address Compare 1
#define SPRN_DAC2      0x13D        // Data Address Compare 2
#define SPRN_DBCR0     0x134                    // Debug Control Register 0
#define   DBCR0_EDM     _BN( 0)                 //  External Debug Mode
#define   DBCR0_IDM     _BN( 1)                 //  Internal Debug Mode
#define   DBCR0_RST(x)  _B2(3,x)                //  Reset
#define     DBCR0_RST_NONE   DBCR0_RST(0)       //   None
#define     DBCR0_RST_CORE   DBCR0_RST(1)       //   Core
#define     DBCR0_RST_CHIP   DBCR0_RST(2)       //   Chip
#define     DBCR0_RST_SYS    DBCR0_RST(3)       //   System
#define   DBCR0_ICMP    _BN( 4)                 //  Instruction Completion Debug Event
#define   DBCR0_BRT     _BN( 5)                 //  Branch Taken Debug Event
#define   DBCR0_IRPT    _BN( 6)                 //  Interrupt Debug Event
#define   DBCR0_TRAP    _BN( 7)                 //  Trap Debug Event
#define   DBCR0_IAC1    _BN( 8)                 //  IAC 1 Debug Event
#define   DBCR0_IAC2    _BN( 9)                 //  IAC 2 Debug Event
#define   DBCR0_IAC3    _BN(10)                 //  IAC 3 Debug Event
#define   DBCR0_IAC4    _BN(11)                 //  IAC 4 Debug Event
#define   DBCR0_DAC1R   _BN(12)                 //  DAC 1 Read  Debug Event
#define   DBCR0_DAC1W   _BN(13)                 //  DAC 1 Write Debug Event
#define   DBCR0_DAC2R   _BN(14)                 //  DAC 2 Read  Debug Event
#define   DBCR0_DAC2W   _BN(15)                 //  DAC 2 Write Debug Event
#define   DBCR0_RET     _BN(16)                 //  Return Debug Event (rfi, rfci)
#define   DBCR0_FT      _BN(31)                 //  Freeze Timers on Debug Event
#define   DBCR0_INIT    0x00000000              //  Disable all debug events!
#define SPRN_DBCR1     0x135                    // Debug Control Register 1
#define   DBCR1_IAC1US(x)     _B2( 1,x)         //  User or Supervisor state
#define   DBCR1_IAC1US_BOTH   DBCR1_IAC1US(0)   //   Both
#define   DBCR1_IAC1US_SUP    DBCR1_IAC1US(2)   //   Sup  Only: MSR[PR]=0
#define   DBCR1_IAC1US_USR    DBCR1_IAC1US(3)   //   User Only: MSR[PR]=1
#define   DBCR1_IAC1ER(x)     _B2( 3,x)         //  IAC1 Effective or Real
#define   DBCR1_IAC1ER_EFF    DBCR1_IAC1ER(0)   //   Effective, MSR[IS] = Don't Care
#define   DBCR1_IAC1ER_VIS0   DBCR1_IAC1ER(2)   //   Virtual, MSR[IS] = 0
#define   DBCR1_IAC1ER_VIS1   DBCR1_IAC1ER(3)   //   Virtual, MSR[IS] = 1
#define   DBCR1_IAC2US(x)     _B2( 5,x)         //  IAC2 User or Supervisor
#define   DBCR1_IAC2US_BOTH   DBCR1_IAC2US(0)   //   Both
#define   DBCR1_IAC2US_SUP    DBCR1_IAC2US(2)   //   Sup  Only: MSR[PR]=0
#define   DBCR1_IAC2US_USR    DBCR1_IAC2US(3)   //   USer Only: MSR[PR]=1
#define   DBCR1_IAC2ER(x)     _B2( 7,x)         //  IAC2 Effective or Real
#define   DBCR1_IAC2ER_EFF    DBCR1_IAC2ER(0)   //   Effective, MSR[IS] = Don't Care
#define   DBCR1_IAC2ER_VIS0   DBCR1_IAC2ER(2)   //   Virtual, MSR[IS] = 0
#define   DBCR1_IAC2ER_VIS1   DBCR1_IAC2ER(3)   //   Virtual, MSR[IS] = 1
#define   DBCR1_IAC12M(x)     _B2( 9,x)         //  IAC 1/2 Mode
#define   DBCR1_IAC12M_EXCT   DBCR1_IAC12M(0)   //   Exact Match
#define   DBCR1_IAC12M_INCL   DBCR1_IAC12M(2)   //   Range Inclusive
#define   DBCR1_IAC12M_EXCL   DBCR1_IAC12M(3)   //   Range Exclusive
#define   DBCR1_IAC12AT       _BN(15)           //  IAC 1/2 Toggle Enable
#define   DBCR1_IAC3US(x)     _B2(17,x)         //  IAC3 User or Supervisor
#define   DBCR1_IAC3US_BOTH   DBCR1_IAC3US(0)   //   Both
#define   DBCR1_IAC3US_SUP    DBCR1_IAC3US(2)   //   Sup  Only: MSR[PR]=0
#define   DBCR1_IAC3US_USR    DBCR1_IAC3US(3)   //   USer Only: MSR[PR]=1
#define   DBCR1_IAC3ER(x)     _B2(19,x)         //  IAC3 Effective or Real
#define   DBCR1_IAC3ER_EFF    DBCR1_IAC3ER(0)   //   Effective, MSR[IS] = Don't Care
#define   DBCR1_IAC3ER_VIS0   DBCR1_IAC3ER(2)   //   Virtual, MSR[IS] = 0
#define   DBCR1_IAC3ER_VIS1   DBCR1_IAC3ER(3)   //   Virtual, MSR[IS] = 1
#define   DBCR1_IAC4US(x)     _B2(21,x)         //  IAC4 User or Supervisor
#define   DBCR1_IAC4US_BOTH   DBCR1_IAC4US(0)   //   Both
#define   DBCR1_IAC4US_SUP    DBCR1_IAC4US(2)   //   Sup  Only: MSR[PR]=0
#define   DBCR1_IAC4US_USR    DBCR1_IAC4US(3)   //   USer Only: MSR[PR]=1
#define   DBCR1_IAC4ER(x)     _B2(23,x)         //  IAC4 Effective or Real
#define   DBCR1_IAC4ER_EFF    DBCR1_IAC4ER(0)   //   Effective, MSR[IS] = Don't Care
#define   DBCR1_IAC4ER_VIS0   DBCR1_IAC4ER(2)   //   Virtual, MSR[IS] = 0
#define   DBCR1_IAC4ER_VIS1   DBCR1_IAC4ER(3)   //   Virtual, MSR[IS] = 1
#define   DBCR1_IAC34M(x)     _B2(25,x)         //  IAC 3/4 Mode
#define   DBCR1_IAC34M_EXCT   DBCR1_IAC34M(0)   //   Exact Match
#define   DBCR1_IAC34M_INCL   DBCR1_IAC34M(2)   //   Range Inclusive
#define   DBCR1_IAC34M_EXCL   DBCR1_IAC34M(3)   //   Range Exclusive
#define   DBCR1_IAC34AT       _BN(31)           //  IAC 3/4 Toggle Enable
#define   DBCR1_INIT          0x00000000        //  Initialization value.
#define SPRN_DBCR2     0x136                    // Debug Control Register 2
#define   DBCR2_DAC1US(x)     _B2( 1,x)         //  DAC1 User or Supervisor state
#define   DBCR2_DAC1US_BOTH   DBCR2_DAC1US(0)   //   Both
#define   DBCR2_DAC1US_SUP    DBCR2_DAC1US(2)   //   Sup  Only: MSR[PR]=0
#define   DBCR2_DAC1US_USR    DBCR2_DAC1US(3)   //   User Only: MSR[PR]=1
#define   DBCR2_DAC1ER(x)     _B2( 3,x)         //  DAC1 Effective or Real
#define   DBCR2_DAC1ER_EFF    DBCR2_DAC1ER(0)   //   Effective, MSR[DS] = Don't Care
#define   DBCR2_DAC1ER_VDS0   DBCR2_DAC1ER(2)   //   Virtual, MSR[DS] = 0
#define   DBCR2_DAC1ER_VDS1   DBCR2_DAC1ER(3)   //   Virtual, MSR[DS] = 1
#define   DBCR2_DAC2US(x)     _B2( 5,x)         //  DAC2 User or Supervisor state
#define   DBCR2_DAC2US_BOTH   DBCR2_DAC2US(0)   //   Both
#define   DBCR2_DAC2US_SUP    DBCR2_DAC2US(2)   //   Sup  Only: MSR[PR]=0
#define   DBCR2_DAC2US_USR    DBCR2_DAC2US(3)   //   User Only: MSR[PR]=1
#define   DBCR2_DAC2ER(x)     _B2( 7,x)         //  DAC2 Effective or Real
#define   DBCR2_DAC2ER_EFF    DBCR2_DAC2ER(0)   //   Effective, MSR[DS] = Don't Care
#define   DBCR2_DAC2ER_VDS0   DBCR2_DAC2ER(2)   //   Virtual, MSR[DS] = 0
#define   DBCR2_DAC2ER_VDS1   DBCR2_DAC2ER(3)   //   Virtual, MSR[DS] = 1
#define   DBCR2_DAC12M(x)     _B2( 9,x)         //  DAC 1/2 Mode
#define   DBCR2_DAC12M_EXCT   DBCR2_DAC12M(0)   //   Exact Match
#define   DBCR2_DAC12M_AMSK   DBCR2_DAC12M(1)   //   Address Bit Mask
#define   DBCR2_DAC12M_INCL   DBCR2_DAC12M(2)   //   Range Inclusive
#define   DBCR2_DAC12M_EXCL   DBCR2_DAC12M(3)   //   Range Exclusive
#define   DBCR2_DAC12A        _BN(10)           //  DAC 1/2 Asynchronous Enable
#define   DBCR2_DVC1M(x)      _B2(13,x)         //  Data Value Compare 1 Mode
#define   DBCR2_DVC1M_AND     DBCR2_DVC1M(1)    //   AND all bytes enabled by DVC2BE
#define   DBCR2_DVC1M_OR      DBCR2_DVC1M(2)    //   OR  all bytes enabled by DVC2BE
#define   DBCR2_DVC1M_AND_OR  DBCR2_DVC1M(3)    //   AND-OR byte pairs enabled by DVC2BE
#define   DBCR2_DVC2M(x)      _B2(15,x)         //  Data Value Compare 2 Mode
#define   DBCR2_DVC2M_AND     DBCR2_DVC2M(1)    //   AND all bytes enabled by DVC2BE
#define   DBCR2_DVC2M_OR      DBCR2_DVC2M(2)    //   OR  all bytes enabled by DVC2BE
#define   DBCR2_DVC2M_AND_OR  DBCR2_DVC2M(3)    //   AND-OR byte pairs enabled by DVC2BE
#define   DBCR2_1BE(x)        _B4(23,x)         //  DVC 1 Byte Enables 0:3
#define   DBCR2_2BE(x)        _B4(31,x)         //  DVC 2 Byte Enables 0:3
#define   DBCR2_INIT          0x00000000        //  Initialization value.
#define SPRN_DBDR      0x3F3                    // Debug Data Register
#define SPRN_DBSR      0x130                    // Debug Status Register
#define   DBSR_IDE      _BN( 0)                 //  Imprecise Debug Event
#define   DBSR_UDE      _BN( 1)                 //  Unconditional Debug Event
#define   DBSR_MRR(x)   _B2( 3,x)               //  Most Recent Reset
#define   DBSR_MRR_NONE DBSR_MRR(0)             //   No reset since last cleared
#define   DBSR_MRR_CORE DBSR_MRR(1)             //   Core
#define   DBSR_MRR_CHIP DBSR_MRR(2)             //   Chip
#define   DBSR_MRR_SYS  DBSR_MRR(3)             //   System
#define   DBSR_ICMP     _BN( 4)                 //  Instruction Completion Debug Event
#define   DBSR_BRT      _BN( 5)                 //  Branch Taken Debug Event
#define   DBSR_IRPT     _BN( 6)                 //  Interrupt Debug Event
#define   DBSR_TRAP     _BN( 7)                 //  Trap Debug Event
#define   DBSR_IAC1     _BN( 8)                 //  IAC 1 Debug Event
#define   DBSR_IAC2     _BN( 9)                 //  IAC 2 Debug Event
#define   DBSR_IAC3     _BN(10)                 //  IAC 3 Debug Event
#define   DBSR_IAC4     _BN(11)                 //  IAC 4 Debug Event
#define   DBSR_DAC1R    _BN(12)                 //  DAC 1 Read  Debug Event
#define   DBSR_DAC1W    _BN(13)                 //  DAC 1 Write Debug Event
#define   DBSR_DAC2R    _BN(14)                 //  DAC 2 Read  Debug Event
#define   DBSR_DAC2W    _BN(15)                 //  DAC 2 Write Debug Event
#define   DBSR_RET      _BN(16)                 //  Return Debug Event
#define   DBSR_IAC12ATS _BN(30)                 //  IAC 1/2 Auto-Toggle Status
#define   DBSR_IAC34ATS _BN(31)                 //  IAC 3/4 Auto-Toggle Status
#define   DBSR_INIT     0xFFFFFFFF              //  Clear all Pending Debug Events
#define SPRN_DCDBTRH   0x39D                    // Data Cache Debug Tag Register High
#define SPRN_DCDBTRL   0x39C                    // Data Cache Debug Tag Register Low
#define SPRN_DEAR      0x03D        // Data Exception Address Register
#define SPRN_DEC       0x016        // Decrementer
#define SPRN_DECAR     0x036        // Decrementer Auto-Reload (Sup R/O)
#define SPRN_DNV0      0x390        // Data Cache Normal Victim 0

#define  DNVx_VNDXA(x)  _B6(7,x)   // bits [2:7] 
#define  DNVx_VNDXB(x)  _B6(15,x)  // bits [10:15]
#define  DNVx_VNDXC(x)  _B6(23,x)  // bits [18:23]
#define  DNVx_VNDXD(x)  _B6(31,x)  // bits [26:31]

#define  DNV0_INIT     (DNVx_VNDXA(0)| DNVx_VNDXB(16) | DNVx_VNDXC(32) | DNVx_VNDXD(48))
#define SPRN_DNV1      0x391        // Data Cache Normal Victim 1
#define  DNV1_INIT     (DNVx_VNDXA(0)| DNVx_VNDXB(16) | DNVx_VNDXC(32) | DNVx_VNDXD(48))
#define SPRN_DNV2      0x392        // Data Cache Normal Victim 2
#define  DNV2_INIT     (DNVx_VNDXA(0)| DNVx_VNDXB(16) | DNVx_VNDXC(32) | DNVx_VNDXD(48))
#define SPRN_DNV3      0x393        // Data Cache Normal Victim 3
#define  DNV3_INIT     (DNVx_VNDXA(0)| DNVx_VNDXB(16) | DNVx_VNDXC(32) | DNVx_VNDXD(48))
#define SPRN_DTV0      0x394        // Data Transient Normal Victim 0

#define  DTVx_VNDXA(x)  _B6(7,x)    //  Victim A: bits[2:7]
#define  DTVx_VNDXB(x)  _B6(15,x)   //  Victim B: bits[10:15]
#define  DTVx_VNDXC(x)  _B6(23,x)   //  Victim C: bits[18:23]
#define  DTVx_VNDXD(x)  _B6(31,x)   //  Victim D: bits[26:31]

#define  DTV0_INIT     (DTVx_VNDXA(0)| DTVx_VNDXB(16) | DTVx_VNDXC(32) | DTVx_VNDXD(48))

#define SPRN_DTV1      0x395        // Data Transient Normal Victim 1
#define  DTV1_INIT     (DTVx_VNDXA(0)| DTVx_VNDXB(16) | DTVx_VNDXC(32) | DTVx_VNDXD(48))
#define SPRN_DTV2      0x396        // Data Transient Normal Victim 2
#define  DTV2_INIT     (DTVx_VNDXA(0)| DTVx_VNDXB(16) | DTVx_VNDXC(32) | DTVx_VNDXD(48))
#define SPRN_DTV3      0x397        // Data Transient Normal Victim 3
#define  DTV3_INIT     (DTVx_VNDXA(0)| DTVx_VNDXB(16) | DTVx_VNDXC(32) | DTVx_VNDXD(48))
#define SPRN_DCV1      0x13E        // Data Value Compare Registers 1
#define SPRN_DCV2      0x13F        // Data Value Compare Registers 2
#define SPRN_DVLIM     0x398        // Data Cache Victim Limit
#define  DVLIM_TFLOOR(x)   _B8( 9,x)   //  Transient Floor
#define  DVLIM_TCEILING(x) _B8(20,x)   //  Transient Ceiling
#define  DVLIM_NFLOOR(x)   _B8(31,x)   //  Normal Floor
                       // INIT: Normal and Transient share entire cache, nothing locked
                       // To lock lines, raise TFLOOR and NFLOOR,
                       //  but always keep TFLOOR <= NFLOOR
                       //  ie, Locked lines:    0..TFLOOR-1
                       //      Transient Lines: TFLOOR..TCEILING-1
                       //      Normal Lines:    TFLOOR..63
#define  DVLIM_INIT    (DVLIM_TFLOOR(0) | DVLIM_TCEILING(63) | DVLIM_NFLOOR(0))
#define SPRN_ESR       0x03E        // Exception Syndrome Register
#define   ESR_MCI       _BN( 0)     //  Machine Check I-Fetch Exception
#define   ESR_PIL       _BN( 4)     //  Program Interrupt Illegal Instruction Exception
#define   ESR_PPR       _BN( 5)     //  Program Interrupt Privileged Instruction Exception
#define   ESR_PTR       _BN( 6)     //  Program Interrupt Trap Exception
#define   ESR_FP        _BN( 7)     //  Floating Point Operation
#define   ESR_ST        _BN( 8)     //  Store Operation
#define   ESR_DLK(x)    _B2(11,x)   //  Data Store Interrupt: Locking Exception
#define    ESR_DLK_NONE ESR_DLK(0)  //   Did not occur
#define    ESR_DLK_DCBF ESR_DLK(1)  //   Caused by dcbf
#define    ESR_DLK_ICBI ESR_DLK(2)  //   Caused by icbi
#define   ESR_AP        _BN(12)     //  Aux Processor Operation
#define   ESR_PUO       _BN(13)     //  Program Interrupt - Unimplemented Operation Exception
#define   ESR_BO        _BN(14)     //  Byte Ordering Exception
#define   ESR_PIE       _BN(15)     //  Program Interrupt - Imprecise Exception
#define   ESR_PCRE      _BN(27)     //  Program Interrupt - FP Condition Register Update
#define   ESR_PCMP      _BN(28)     //  Program Interrupt - FP Compare Instruction
#define   ESR_PCRF(x)   _B3(31,x)   //  Program Interrupt - Condition Register Field
#define   ESR_INIT      0x00000000  //  Clear all ESR interrupts
#define SPRN_IAC1      0x138        // Instruction Address Compare Register 1
#define SPRN_IAC2      0x139        // Instruction Address Compare Register 2
#define SPRN_IAC3      0x13A        // Instruction Address Compare Register 3
#define SPRN_IAC4      0x13B        // Instruction Address Compare Register 4
#define SPRN_ICDBDR    0x3D3        // I-Cache Debug Data Register
#define SPRN_ICDBTRH   0x39F        // I-Cache Debug Tag Register High
#define SPRN_ICDBTRL   0x39E        // I-Cache Debug Tag Register Low
#define SPRN_INV0      0x370        // I-Cache Normal Victim 0
#define  INVx_VNDXA(x)  _B8( 0,x)   //  Victim A: EA[25:26] = 0b00
#define  INVx_VNDXB(x)  _B8( 8,x)   //  Victim B: EA[25:26] = 0b01
#define  INVx_VNDXC(x)  _B8(16,x)   //  Victim C: EA[25:26] = 0b10
#define  INVx_VNDXD(x)  _B8(24,x)   //  Victim D: EA[25:26] = 0b11
#define  INV0_INIT     INVx_VNDXA(0) | INVx_VNDXB(16) | INVx_VNDXC(32) | INVx_VNDXD(48)
#define SPRN_INV1      0x371        // I-Cache Normal Victim 1
#define  INV1_INIT     (INVx_VNDXA(0)| INVx_VNDXB(16) | INVx_VNDXC(32) | INVx_VNDXD(48))
#define SPRN_INV2      0x372        // I-Cache Normal Victim 2
#define  INV2_INIT     (INVx_VNDXA(0)| INVx_VNDXB(16) | INVx_VNDXC(32) | INVx_VNDXD(48))
#define SPRN_INV3      0x373        // I-Cache Normal Victim 3
#define  INV3_INIT     (INVx_VNDXA(0)| INVx_VNDXB(16) | INVx_VNDXC(32) | INVx_VNDXD(48))
#define SPRN_ITV0      0x374        // I-Cache Transient Victim 0
#define  ITVx_VNDXA(x)  _B8( 0,x)   //  Victim A: EA[25:26] = 0b00
#define  ITVx_VNDXB(x)  _B8( 8,x)   //  Victim B: EA[25:26] = 0b01
#define  ITVx_VNDXC(x)  _B8(16,x)   //  Victim C: EA[25:26] = 0b10
#define  ITVx_VNDXD(x)  _B8(24,x)   //  Victim D: EA[25:26] = 0b11
#define  ITV0_INIT     (ITVx_VNDXA(0)| ITVx_VNDXB(16) | ITVx_VNDXC(32) | ITVx_VNDXD(48))
#define SPRN_ITV1      0x375        // I-Cache Transient Victim 1
#define  ITV1_INIT     (ITVx_VNDXA(0)| ITVx_VNDXB(16) | ITVx_VNDXC(32) | ITVx_VNDXD(48))
#define SPRN_ITV2      0x376        // I-Cache Transient Victim 2
#define  ITV2_INIT     (ITVx_VNDXA(0)| ITVx_VNDXB(16) | ITVx_VNDXC(32) | ITVx_VNDXD(48))
#define SPRN_ITV3      0x377        // I-Cache Transient Victim 3
#define  ITV3_INIT     (ITVx_VNDXA(0)| ITVx_VNDXB(16) | ITVx_VNDXC(32) | ITVx_VNDXD(48))
#define SPRN_IVLIM     0x399        // I-Cache Victim Limit
#define  IVLIM_TFLOOR(x)   _B8( 9,x)   //  Transient Floor
#define  IVLIM_TCEILING(x) _B8(20,x)   //  Transient Ceiling
#define  IVLIM_NFLOOR(x)   _B8(31,x)   //  Normal Floor
                       // INIT: Normal and Transient share entire cache, nothing locked
                       //  To lock lines, raise TFLOOR and NFLOOR,
                       //   but always keep TFLOOR <= NFLOOR
                       //   ie, Locked lines:    0..TFLOOR-1
                       //       Transient Lines: TFLOOR..TCEILING-1
                       //       Normal Lines:    TFLOOR..63
#define  IVLIM_INIT    (IVLIM_TFLOOR(0) | IVLIM_TCEILING(63) | IVLIM_NFLOOR(0))
#define SPRN_IVOR0     0x190        // Interrupt Vector Offset Register  0
#define SPRN_IVOR1     0x191        // Interrupt Vector Offset Register  1
#define SPRN_IVOR2     0x192        // Interrupt Vector Offset Register  2
#define SPRN_IVOR3     0x193        // Interrupt Vector Offset Register  3
#define SPRN_IVOR4     0x194        // Interrupt Vector Offset Register  4
#define SPRN_IVOR5     0x195        // Interrupt Vector Offset Register  5
#define SPRN_IVOR6     0x196        // Interrupt Vector Offset Register  6
#define SPRN_IVOR7     0x197        // Interrupt Vector Offset Register  7
#define SPRN_IVOR8     0x198        // Interrupt Vector Offset Register  8
#define SPRN_IVOR9     0x199        // Interrupt Vector Offset Register  9
#define SPRN_IVOR10    0x19A        // Interrupt Vector Offset Register 10
#define SPRN_IVOR11    0x19B        // Interrupt Vector Offset Register 11
#define SPRN_IVOR12    0x19C        // Interrupt Vector Offset Register 12
#define SPRN_IVOR13    0x19D        // Interrupt Vector Offset Register 13
#define SPRN_IVOR14    0x19E        // Interrupt Vector Offset Register 14
#define SPRN_IVOR15    0x19F        // Interrupt Vector Offset Register 15
#define SPRN_IVPR      0x03F        // Interrupt Vector Prefix Register
#define SPRN_LR        0x008        // Link Register
#define SPRN_MMUCR     0x3B2        // Memory Management Control Register
#define   MMUCR_SWOA    _BN( 7)     //  Store WithOut Allocate
#define   MMUCR_U1TE    _BN( 9)     //  U1 Transient Enable
#define   MMUCR_U2SWOAE _BN(10)     //  U2 Store WithOut Allocate Enable
#define   MMUCR_DULXE   _BN(12)     //  Data Cache Unlock Exception Enable
#define   MMUCR_IULXE   _BN(13)     //  Instruction Cache Unlock Exception Enable
#define   MMUCR_STS     _BN(15)     //  Search Translation Space
#define   MMUCR_STID(x) _B8(31,x)   //  Search Translation ID: MMUCR[24:31]
#define   MMUCR_INIT    0x00000000  //  Initialization Value
#define SPRN_PID       0x030        // Process ID
#define  PID_PID(x)     _B8(31)     //  8 bit Process ID Field
#define  PID_INIT       0x00000000  // Init to PID=0
#define SPRN_PIR       0x11E        // Processor Identification Register
#define  PIR_PIN(x)     _B8(31,x)   //  8 bit Processor ID Number (Sup R/O)
#define  PIR_PIN_COMP   PIR_PIN(0)  //  BlueLight Compute Processor
#define  PIR_PIN_IOP    PIR_PIN(1)  //  BlueLight I/O Processor
#define SPRN_PVR       0x11F        // Processor Version Register (Sup R/O)
#define SPRN_RSTCFG    0x39B        // Reset Configuration (Sup R/O)
#define  RSTCFG_U0      _BN(16)     //  U0 Storage Attribute Enable
#define  RSTCFG_U1      _BN(17)     //  U1 Storage Attribute Enable
#define  RSTCFG_U2      _BN(18)     //  U2 Storage Attribute Enable
#define  RSTCFG_U3      _BN(19)     //  U3 Storage Attribute Enable
#define  RSTCFG_E       _BN(24)     //  Endian Attribute (0=Big, 1=Little)
#define  RSTCFG_EPRN(x) _B8(31)     //  Extended Real Page Number
#define SPRN_SPRG0RO    0x110       // SPR General 0 (Sup R/W)
#define SPRN_SPRG0WO    0x110       // SPR General 0 (Sup R/W)
#define SPRN_SPRG1RO    0x111       // SPR General 1 (Sup R/W)
#define SPRN_SPRG1WO    0x111       // SPR General 1 (Sup R/W)
#define SPRN_SPRG2RO    0x112       // SPR General 2 (Sup R/W)
#define SPRN_SPRG2WO    0x112       // SPR General 2 (Sup R/W)
#define SPRN_SPRG3RO    0x113       // SPR General 3 (Sup R/W)
#define SPRN_SPRG3WO    0x113       // SPR General 3 (Sup R/W)
#define SPRN_SPRG4WO    0x114       // SPR General 4 (Sup W/O)
#define SPRN_SPRG5WO    0x115       // SPR General 5 (Sup W/O)
#define SPRN_SPRG6WO    0x116       // SPR General 6 (Sup W/O)
#define SPRN_SPRG7WO    0x117       // SPR General 7 (Sup W/O)
#define SPRN_SRR0       0x01A       // Save/Restore Register 0: Return Addr for Non-Critical
#define SPRN_SRR1       0x01B       // Save/Restore Register 1: MSR for Non-Critical
#define SPRN_TBWL       0x11C       // Time Base Lower Register (sup, W/O)
#define SPRN_TBWU       0x11D       // Time Base Upper Register (sup, W/O)
#define SPRN_TCR        0x154       // Timer Control Register
#define   TCR_WP(x)      _B2( 1,x)  //  WDT Period
#define   TCR_WP_2_21    TCR_WP(0)  //   2^21 clocks
#define   TCR_WP_2_25    TCR_WP(1)  //   2^25 clocks
#define   TCR_WP_2_29    TCR_WP(2)  //   2^29 clocks
#define   TCR_WP_2_33    TCR_WP(3)  //   2^33 clocks
#define   TCR_WRC(x)     _B2( 3,x)  //  WDT Reset Control
#define   TCR_WRC_NONE   TCR_WRC(0) //   No reset will occur
#define   TCR_WRC_CORE   TCR_WRC(1) //   Core reset will occur
#define   TCR_WRC_CHIP   TCR_WRC(2) //   Chip reset will occur
#define   TCR_WRC_SYS    TCR_WRC(3) //   System reset will occur
#define  TCR_WIE        _BN( 4)    // WDT Interrupt Enable
#define  TCR_DIE        _BN( 5)    // DECrementer Interrupt Enable
#define  TCR_FP(x)      _B2( 7,x)  // FIT Period
#define  TCR_FP_2_13    TCR_FP(0)  //   2^13 clocks
#define  TCR_FP_2_17    TCR_FP(1)  //   2^17 clocks
#define  TCR_FP_2_21    TCR_FP(2)  //   2^21 clocks
#define  TCR_FP_2_25    TCR_FP(3)  //   2^25 clocks
#define   TCR_FIE        _BN( 8)    // FIT Interrupt Enable
#define   TCR_ARE        _BN( 9)    // Auto Reload Enable
#define   TCR_INIT      0x00000000  // Disable all Timer Interrupts
#define SPRN_TSR       0x150        // Timer Status Register (Read/Clear)
#define   TSR_ENW       _BN( 0)     //  Enable Next Watchdog
#define   TSR_WIS       _BN( 1)     //  WDT Interrupt Status
#define   TSR_WRS(x)    _B2( 3,x)   //  WDT Reset Status
#define   TSR_WRS_NONE  TSR_WRS(0)  //   No WDT reset occurred
#define   TSR_WRS_CORE  TSR_WRS(1)  //   WDT forced core reset
#define   TSR_WRS_CHIP  TSR_WRS(2)  //   WDT forced chip reset
#define   TSR_WRS_SYS   TSR_WRS(3)  //   WDT forced system reset
#define   TSR_DIS          _BN( 4)     //  DEC Interrupt Status
#define   TSR_FIS       _BN( 5)     //  FIT Interrupt Status
#define   TSR_INIT    0xFFFFFFFF    //  Clear all Pending Interrupt/Status Bits
#define SPRN_XER     0x001          // Fixed-Point Exception Register
#define   XER_SO      _BN( 0)       //  Summary Overflow
#define   XER_OV      _BN( 1)       //  Overflow
#define   XER_CA      _BN( 2)       //  Carry
#define   XER_TBC(x) ((x)&0x7f)     //  Transfer Byte Count (for lswx and stswx)

// Describe the FPSCR
#define FPSCR_FX        _BN( 0)            //   FP Exception Summary
#define FPSCR_FEX       _BN( 1)            //   FP Enabled Exception Summary
#define FPSCR_VX        _BN( 2)            //   FP Invalid Operation Exception Summary
#define FPSCR_OX        _BN( 3)            //   FP Invalid Overflow Exception (Stickey)
#define FPSCR_UX        _BN( 4)            //   FP Invalid Underflow Exception (Stickey)
#define FPSCR_ZX        _BN( 5)            //   FP Zero Divide Exception (Stickey)
#define FPSCR_XX        _BN( 6)            //   FP Inexact Exception (Stickey)
#define FPSCR_VXSNAN    _BN( 7)            //   FP Invalid Operation Exception for SNaN (Stickey)
#define FPSCR_VXISI     _BN( 8)            //   FP Invalid Operation Exception for Inf-Inf (Stickey)
#define FPSCR_VXIDI     _BN( 9)            //   FP Invalid Operation Exception for Inf/Inf (Stickey)
#define FPSCR_VXZDZ     _BN(10)            //   FP Invalid Operation Exception for 0/0 (Stickey)
#define FPSCR_VXIMZ     _BN(11)            //   FP Invalid Operation Exception for Inf*0 (Stickey)
#define FPSCR_VXVC      _BN(12)            //   FP Invalid Operation Exception for Invalid Compare (Stickey)
#define FPSCR_FR        _BN(13)            //   FP Fraction Rounded
#define FPSCR_FI        _BN(14)            //   FP Fraction Inexact
#define FPSCR_FPRF(x)   _B5(19,x)          //   FP Result Flags (see 6xx_pem.pdf Table 2-5)
#define   FPSCR_FPRF_QNaN FPSCR_FPRF(0x11) //     Quiet NaN
#define   FPSCR_FPRF_MINF FPSCR_FPRF(0x09) //     Minus Inf
#define   FPSCR_FPRF_MNOR FPSCR_FPRF(0x08) //     Minus Normalized Number
#define   FPSCR_FPRF_MDEN FPSCR_FPRF(0x18) //     Minus Denormalized Number
#define   FPSCR_FPRF_MZ   FPSCR_FPRF(0x12) //     Minus Zero
#define   FPSCR_FPRF_PZ   FPSCR_FPRF(0x02) //     Plus  Zero
#define   FPSCR_FPRF_PDEN FPSCR_FPRF(0x14) //     Plus  Denormalized number
#define   FPSCR_FPRF_PNOR FPSCR_FPRF(0x04) //     Plus  Normalized number
#define   FPSCR_FPRF_PINF FPSCR_FPRF(0x03) //     Plus  Inf
#define FPSCR_Rsvd      _BN(20)            //   Reserved
#define FPSCR_VXSOFT    _BN(21)            //   FP Invalid Operation Exception for Software Request (Stickey)
#define FPSCR_VXSQRT    _BN(22)            //   FP Invalid Operation Exception for Invalid Square Root (Stickey)
#define FPSCR_VXCVI     _BN(23)            //   FP Invalid Operation Exception for Invalid Integer Convert (Stickey)
#define FPSCR_VE        _BN(24)            //   FP Invalid Operation Exception Enable
#define FPSCR_OE        _BN(25)            //   FP Overflow Exception Enable
#define FPSCR_UE        _BN(26)            //   FP Underflow Exception Enable
#define FPSCR_ZE        _BN(27)            //   FP Zero-Divide Exception Enable
#define FPSCR_XE        _BN(28)            //   FP Inexact Exception Enable
#define FPSCR_NI        _BN(29)            //   FP Non-IEEE Mode
#define FPSCR_RN(x)     _B2(30,x)          //   FP Rounding Control
#define   FPSCR_RN_NEAR   FPSCR_RN(0)      //     Round Nearest
#define   FPSCR_RN_ZERO   FPSCR_RN(1)      //     Round Toward Zero
#define   FPSCR_RN_PINF   FPSCR_RN(2)      //     Round Toward +Infinity
#define   FPSCR_RN_MINF   FPSCR_RN(3)      //     Round Toward -Infinity

// RB. removed the floating point interrupt bits, we want to enable interrupts, not cause them.
//#define FPSCR_INIT      (FPSCR_VXSOFT | FPSCR_VXSQRT | FPSCR_VXCVI | FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE )
#define FPSCR_INIT      (FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE )

// User-Mode SPRs. (Most are Read-Only.)
#define SPRN_SPRG4RO    0x104       // SPR General 4 (user & sup R/0)
#define SPRN_SPRG5RO    0x105       // SPR General 5 (user & sup R/0)
#define SPRN_SPRG6RO    0x106       // SPR General 6 (user & sup R/0)
#define SPRN_SPRG7RO    0x107       // SPR General 7 (user & sup R/0)

#define SPRN_TBRL       0x10C         // Time Base Read Lower Register (user & sup R/O)
#define SPRN_TBRU       0x10D         // Time Base Read Upper Register (user & sup R/O)

#define SPRN_USPRG0     0x100       // User SPR General 0 (user, R/W)

/************************************************************
*
* $Id: ppc440_core.h,v 1.19 2006/09/14 04:00:58 pjmccart Exp $
*
* $Log: ppc440_core.h,v $
* Revision 1.19  2006/09/14 04:00:58  pjmccart
* Issue 6722: transient storage
*
* Revision 1.18  2003/11/05 19:32:00  almasig
* Turned off icache prefetch following verbal comm with Matt Blumrich
*
* Revision 1.17  2003/09/30 15:02:03  derek
* disable CCR0_GICBT to see if it makes icache machine checks go away
*
* Revision 1.16  2003/09/04 17:20:32  derek
* add errata info for CCR0, add CCR1
*
* Revision 1.15  2003/09/03 18:25:32  derek
* add MCSR definitions, fix DBCR2 definition
*
* Revision 1.14  2003/08/25 14:46:38  ralphbel
* add undefs to get rid of warnings in aix
*
* Revision 1.13  2003/06/24 18:00:34  ralphbel
* add _Bx definitions
*
* Revision 1.12  2003/06/18 14:41:08  brunhe
* empty
*
* Revision 1.11  2003/06/18 08:52:14  brunhe
* Fixed 'DNVx_VNDX()' and 'DTVx_VNDX()' - 6-bit for each field
*
* Revision 1.10  2003/05/30 15:30:51  almasig
* made sprn_ccr0(icslc)=1 to fix Avenger Erratum #38
*
* Revision 1.9  2003/05/09 12:41:22  derek
* enable mcsrr0,mcsrr1 unconditionally
*
* Revision 1.8  2003/05/08 21:58:14  alda
* machine check save/restore registers added
*
* Revision 1.7  2003/03/04 14:50:56  ralphbel
* copyright
*
* Revision 1.6  2003/02/14 02:22:33  ralphbel
* fix defaults for the floating point control register initial value
*
* Revision 1.5  2002/11/27 01:27:24  alda
* turn ccr0 flsta off (force load store alignment)
*
* Revision 1.4  2002/09/10 13:39:18  almasig
* *** empty log message ***
*
* Revision 1.3  2002/08/06 13:20:30  derek
* remove ifdef __KERNEL__ guards so everybody can use this headerfile
*
* Revision 1.2  2002/08/05 14:32:17  ralphbel
* Do first cut for consolidated reset code
*
*
*************************************************************/

#endif
