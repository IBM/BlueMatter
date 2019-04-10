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
 //
//      File: blade/include/sys/blade_types.h
//
//   Purpose: Define types and alignment macros.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//            (c) IBM, 2001
//
//     Notes:
//
//   History: 08/01/2001: MEG - Created.
//
//
#ifndef _BLADE_TYPES_H  // Prevent multiple inclusion.
#define _BLADE_TYPES_H

#ifndef __ASSEMBLY__

// Defines and macros for Host Compiler Support
#ifdef __cplusplus

#define __BEGIN_DECLS extern "C" {

#define __END_DECLS   }

#else  // not __cplusplus

#define __BEGIN_DECLS

#define __END_DECLS

#endif // __cplusplus


#define ALIGN16        __attribute__ ((aligned ( 16)))
#define ALIGN_QUADWORD __attribute__ ((aligned ( 16)))
#define ALIGN_CACHE    __attribute__ ((aligned ( 32)))
#define ALIGN_L1_CACHE __attribute__ ((aligned ( 32)))
#define ALIGN_L2_CACHE __attribute__ ((aligned (128)))
#define ALIGN_L3_CACHE __attribute__ ((aligned (128)))

// Pack C data structures.  Use with care to avoid alignment problems.
#define PACKED __attribute__ ((packed))

typedef   signed char   int8;
typedef unsigned char  uint8;
typedef unsigned char   bit8;
typedef unsigned char   Bit8;

typedef   signed short  int16;
typedef unsigned short uint16;
typedef unsigned short  bit16;
typedef unsigned short  Bit16;

typedef   signed long   int32;
typedef unsigned long  uint32;
typedef unsigned long   bit32;
typedef unsigned long   Bit32;

typedef   signed long long  int64;
typedef unsigned long long uint64;
typedef unsigned long long  bit64;
typedef unsigned long long  Bit64;

typedef union T_QuadWord
               {
               Bit8   ub[16];
               Bit16  us[ 8];
               Bit32  ul[ 4];
               Bit64 ull[ 2];
               }
               ALIGN_QUADWORD _QuadWord;

// Need a 128bit aggregate to pass quadwords by value.
// This definition will change when the ABI is enhanced.
// Currently only the gnu compilers allow this.
#if !defined(PK_XLC)
#define QUADWORD __complex__ double ALIGN16

typedef QUADWORD quad;
typedef QUADWORD bit128;
typedef QUADWORD Bit128;
#endif

typedef struct T_iovec
                {
                   char *base;
                int  len;
                }
                iovec;

#endif // __ASSEMBLY

#endif // Add nothing below this line.

