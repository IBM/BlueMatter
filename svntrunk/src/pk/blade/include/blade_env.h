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
//      File: blade/include/sys/blade_env.h
//
//   Purpose: Support macros for Header files useful to Assembler and C code.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes:
//
//   History: 10/01/2000: MEG - Created.
//
//
#ifndef _BLADE_ENV_H  // Prevent multiple inclusion
#define _BLADE_ENV_H

// These defines allows use of IBM's bit numberings (MSb=0, LSb=31)for multi-bit fields
//  b = IBM bit number of the least significant bit (highest number)
//  x = value to set in field
#define _BN(b)    ((1<<(31-(b))))
#define _B1(b,x)  (((x)&0x1)<<(31-(b)))
#define _B2(b,x)  (((x)&0x3)<<(31-(b)))
#define _B3(b,x)  (((x)&0x7)<<(31-(b)))
#define _B4(b,x)  (((x)&0xF)<<(31-(b)))
#define _B5(b,x)  (((x)&0x1F)<<(31-(b)))
#define _B6(b,x)  (((x)&0x3F)<<(31-(b)))
#define _B7(b,x)  (((x)&0x7F)<<(31-(b)))
#define _B8(b,x)  (((x)&0xFF)<<(31-(b)))
#define _B9(b,x)  (((x)&0x1FF)<<(31-(b)))
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

#ifndef __ASSEMBLY__

#define MIN( a, b )  (((a) < (b)) ? (a) : (b))
#define MAX( a, b )  (((a) > (b)) ? (a) : (b))

// These defines ease extraction of bitfields.  (Not useful in assembler code.)
//  x = 32 bit value
//  b = IBM bit number of least significant bit of field
//  when b is a const, compiler should generate a single rotate-and-mask instruction
#define _GN(x,b)  (((x)>>(31-(b)))&0x1)
#define _G2(x,b)  (((x)>>(31-(b)))&0x3)
#define _G3(x,b)  (((x)>>(31-(b)))&0x7)
#define _G4(x,b)  (((x)>>(31-(b)))&0xF)
#define _G5(x,b)  (((x)>>(31-(b)))&0x1F)
#define _G6(x,b)  (((x)>>(31-(b)))&0x3F)
#define _G7(x,b)  (((x)>>(31-(b)))&0x7F)
#define _G8(x,b)  (((x)>>(31-(b)))&0xFF)
#define _G9(x,b)  (((x)>>(31-(b)))&0x1FF)
#define _G10(x,b) (((x)>>(31-(b)))&0x3FF)
#define _G11(x,b) (((x)>>(31-(b)))&0x7FF)
#define _G12(x,b) (((x)>>(31-(b)))&0xFFF)
#define _G13(x,b) (((x)>>(31-(b)))&0x1FFF)
#define _G14(x,b) (((x)>>(31-(b)))&0x3FFF)
#define _G15(x,b) (((x)>>(31-(b)))&0x7FFF)
#define _G16(x,b) (((x)>>(31-(b)))&0xFFFF)
#define _G17(x,b) (((x)>>(31-(b)))&0x1FFFF)
#define _G18(x,b) (((x)>>(31-(b)))&0x3FFFF)
#define _G19(x,b) (((x)>>(31-(b)))&0x7FFFF)
#define _G20(x,b) (((x)>>(31-(b)))&0xFFFFF)
#define _G21(x,b) (((x)>>(31-(b)))&0x1FFFFF)
#define _G22(x,b) (((x)>>(31-(b)))&0x3FFFFF)
#define _G23(x,b) (((x)>>(31-(b)))&0x7FFFFF)
#define _G24(x,b) (((x)>>(31-(b)))&0xFFFFFF)
#define _G25(x,b) (((x)>>(31-(b)))&0x1FFFFFF)
#define _G26(x,b) (((x)>>(31-(b)))&0x3FFFFFF)
#define _G27(x,b) (((x)>>(31-(b)))&0x7FFFFFF)
#define _G28(x,b) (((x)>>(31-(b)))&0xFFFFFFF)
#define _G29(x,b) (((x)>>(31-(b)))&0x1FFFFFFF)
#define _G30(x,b) (((x)>>(31-(b)))&0x3FFFFFFF)
#define _G31(x,b) (((x)>>(31-(b)))&0x7FFFFFFF)


#endif // __ASSEMBLY__

#endif // Add nothing below this line.

