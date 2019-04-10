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

#ifndef __bgl_dcr_h__
#define __bgl_dcr_h__

/* ************************************************************************* */
/*      Architected DCR registers for Torus, tree, memory and so on          */
/* Include this file if your code does anything with these DCRs.             */
/* ************************************************************************* */




#define BGL_DCR_BIC                      0x000 /* bgl interrupt controller */
#define BGL_DCR_UIC                      0x180 /* interrupt controller - this must disappear after linux and hpk can deal with BIC */ 
#define BGL_DCR_PLBARBITER               0x090 /* PLB arbiter */
#define BGL_DCR_FPU                      0x0A0 /* FPU */
#define BGL_DCR_PLBOPB                   0x0B0 /* PLB/OPB bridge */
#define BGL_DCR_TESTINTF                 0x0C0 /* test interface */
#define BGL_DCR_LOCKBOX                  0x100 /* lockbox */
#define BGL_DCR_PLBSLAVE                 0x110 /* PLB slave */
#define BGL_DCR_SRAM                     0x120 /* SRAM */
#define BGL_DCR_L3                       0x140 /* L3 */
#define BGL_DCR_L2                       0x160 /* L2 with MCCU */
#define BGL_DCR_PERFCOUNTERS             0x180 /* performance counters */
#define BGL_DCR_TREE                     0x1C0 /* tree */
#define BGL_DCR_TREECAP                  0x220 /* capture unit tree */
#define BGL_DCR_TORUSCAP                 0x250 /* capture unit torus */
#define BGL_DCR_TORUS                    0x280 /* torus */
#define BGL_DCR_MAL                      0x300 /* McMAL */
#define BGL_DCR_DDR                      0x380 /* DDR controller */

#endif /* __bgl_dcr_h__ */
