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
/* (C)Copyright IBM Corp. 2004, 2005                              */
/* All rights reserved.                                           */
/* US Government Users Restricted Rights -                        */
/* Use, duplication or disclosure restricted                      */
/* by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                */
/* Licensed Materials-Property of IBM                             */
/* -------------------------------------------------------------  */

#ifndef __fpudcr_h__
#define __fpudcr_h__

/* ************************************************************************* */
/*  Description of FPU (Double Hummer) DCRs:                                 */
/*       addresses of DCRs                                                   */
/*       offsets of the same                                                 */
/*       structure definition for register contents (bitfield defs)          */
/*       access functions (set and get DCR contents)                         */
/* ************************************************************************* */
/*                                                                           */
/* ************************************************************************* */

#include "bgldcr.h"

/* ************************************************************************* */
/*         Addresses of FPU (Double Hummer) DCR registers                    */
/* ************************************************************************* */

#define BGL_FPUDCR_OTHER_CORE   8              

#define BGL_FPUDCR_BASE      (BGL_DCR_FPU)
#define BGL_FPUDCR_RANGE     (16)
#define BGL_FPUDCR_END       (BGL_DCR_DDR + 0x0F)


#define BGL_FPU_ARITH_INSTR_COUNT             (BGL_FPUDCR_BASE + 0x01)
#define BGL_FPU_LDST_INSTR_COUNT              (BGL_FPUDCR_BASE + 0x02)
#define BGL_FPU_PERF_COUNT_CTRL               (BGL_FPUDCR_BASE + 0x03)
#define BGL_FPU_RDWR_CTRL                     (BGL_FPUDCR_BASE + 0x04)
#define BGL_FPU_RDWR_HIGH_DATA                (BGL_FPUDCR_BASE + 0x06)
#define BGL_FPU_RDWR_LOW_DATA                 (BGL_FPUDCR_BASE + 0x07)

#endif
