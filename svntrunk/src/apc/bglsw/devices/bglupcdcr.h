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

#ifndef __bglupcdcr_h__
#define __bglupcdcr_h__

#include "bgldcr.h"

/* ************************************************************************* */
/*                 BGL UPC DCR base and range                                */
/* ************************************************************************* */

#define BGL_UPCDCR_BASE                  (BGL_DCR_PERFCOUNTERS)
#define BGL_UPCDCR_RANGE                 (0x3f)
#define BGL_UPCDCR_END                   (BGL_DCR_PERFCOUNTERS + BGL_UPCDCR_RANGE)


/* Offset within the Group */

#define BGL_UPCDCR_GROUP_CNTA_OFFSET     (0x00)
#define BGL_UPCDCR_GROUP_CNTB_OFFSET     (0x01)
#define BGL_UPCDCR_GROUP_CNTC_OFFSET     (0x02)
#define BGL_UPCDCR_GROUP_GRC_OFFSET      (0x03)


////////////////////////////////////////////////////////////////////
// definitions for performance counters 0..15...
//
#define BGL_UPC_CNTA(n)  (BGL_DCR_PERFCOUNTERS + (((n) & 0xf) << 2) + 0 )   // counter A - n
#define BGL_UPC_CNTB(n)  (BGL_DCR_PERFCOUNTERS + (((n) & 0xf) << 2) + 1 )   // counter B - n
#define BGL_UPC_CNTC(n)  (BGL_DCR_PERFCOUNTERS + (((n) & 0xf) << 2) + 2 )   // counter C - n
#define BGL_UPC_GCR(n)   (BGL_DCR_PERFCOUNTERS + (((n) & 0xf) << 2) + 3 )   // Group control register - n

////////////////////////////////////////////////////////////////////////////////////
// Group control register is defined as follows:
//
// 0..1     : EDGE SELECT A 
//              0 -- Increment counter A each cycle while selected input is HIGH.
//              1 -- Increment counter A if input changes from LOW to HIGH
//              2 -- Increment counter A if input changes from HIGH to LOW.
//              3 -- Increment counter A each cycle while selected input is LOW.
// 2..5     : CNTA IRQ frequency
//              0 -- Disable counter A interrupts.
//              1..14 --
//              15 -- Counter A generates one interrupt per count event.
// 6..10    : CNTA EVENT Selector 0..31
// 11..12   : EDGE SELECT B
//              0 -- Increment counter B each cycle while selected input is HIGH.
//              1 -- Increment counter B if input changes from LOW to HIGH
//              2 -- Increment counter B if input changes from HIGH to LOW.
//              3 -- Increment counter B each cycle while selected input is LOW.
// 13..15   : CNTB IRQ frequency
//              0 -- Disable counter B interrupts.
//              1..14 --
//              15 -- Counter B generates one interrupt per count event.
// 16..19   : CNTB EVENT Selector 0..15
// 20..21  : EDGE SELECT C
//              0 -- Increment counter C each cycle while selected input is HIGH.
//              1 -- Increment counter C if input changes from LOW to HIGH
//              2 -- Increment counter C if input changes from HIGH to LOW.
//              3 -- Increment counter C each cycle while selected input is LOW.
// 22..25   : CNTC IRQ frequency
//              0 -- Disable counter B interrupts.
//              1..14 --
//              15 -- Counter B generates one interrupt per count event.
// 26..30   : CNTC EVENT Selector 0..31
// 31       : CNTC-n Mode
//              0 -- Counter C counts continously.
//              1 -- Counter C stops counts at MAXCOUNT and is set to 
//                   MINCOUNT upon DCR read request;
//                   MAXCOUNT = 0xFFFFFFFFFF (32 bit mode) or
//                   0xFFFFFFFFFFFFFFFF (64 bit mode),
//                   MINCOUNT = 0x00000000 (32 bit mode),
//                   0x0000000000000000 (64 bit mode).
//
//
#define BGL_UPC_GCR_CNTA_EDGE_SELECT(n) ( ((n) & 0x3)  << (31-1) )
#define BGL_UPC_GCR_CNTA_IRQ_FREQ(n)    ( ((n) & 0xf)  << (31-5) )
#define BGL_UPC_GCR_CNTA_EVENT_SEL(n)   ( ((n) & 0x1f) << (31-10) )
#define BGL_UPC_GCR_CNTA_EVENT_GET(n)   ( ((n) >> (31-10)) & 0x1f )

#define BGL_UPC_GCR_CNTB_EDGE_SELECT(n) ( ((n) & 0x3)  << (31-12) )
#define BGL_UPC_GCR_CNTB_IRQ_FREQ(n)    ( ((n) & 0x7)  << (31-15) )
#define BGL_UPC_GCR_CNTB_EVENT_SEL(n)   ( ((n) & 0xf) << (31-19) )
#define BGL_UPC_GCR_CNTB_EVENT_GET(n)   ( ((n) >> (31-19)) & 0xf )

#define BGL_UPC_GCR_CNTC_EDGE_SELECT(n) ( ((n) & 0x3)  << (31-21) )
#define BGL_UPC_GCR_CNTC_IRQ_FREQ(n)    ( ((n) & 0xf)  << (31-25) )
#define BGL_UPC_GCR_CNTC_EVENT_SEL(n)   ( ((n) & 0x1f) << (31-30) )
#define BGL_UPC_GCR_CNTC_EVENT_GET(n)   ( ((n) >> (31-30)) & 0x1f )

#define BGL_UPC_GCR_CNTC_MODE(n)        ( ((n) & 0x1)  << (31-31) )

/* ************************************************************************* */
/*          BGL UPC Memory Map base and range for non-interfering READ       */
/* ************************************************************************* */

#define BGL_UPCMEM_BASE                  (BGL_MEM_UPC_BASE)
#define BGL_UPCMEM_RANGE                 (0xff)
#define BGL_UPCMEM_END                   (BGL_UPCMEM_BASE + BGL_UPCMEM_RANGE)

#define BGL_UPCMEM_CNTA(n)  (BGL_UPCMEM_BASE + (((n) & 0xf) << 4) + 0 )   // counter A - n
#define BGL_UPCMEM_CNTB(n)  (BGL_UPCMEM_BASE + (((n) & 0xf) << 4) + 4 )   // counter B - n
#define BGL_UPCMEM_CNTC(n)  (BGL_UPCMEM_BASE + (((n) & 0xf) << 4) + 8 )   // counter C - n
#define BGL_UPCMEM_GCR(n)   (BGL_UPCMEM_BASE + (((n) & 0xf) << 4) +12 )   // Group control register - n

#endif /* __bglupcdcr_h__ */

