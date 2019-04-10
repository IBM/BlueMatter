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
 #ifndef _BonB_Threads_H_  // Prevent multiple inclusion
#define _BonB_Threads_H_


#define _BG_NUM_GPRS (32)
#define _BG_NUM_FPRS (32)

#define _BG_COR_OFS_GPR0    (  0)
#define _BG_COR_OFS_GPR1    (  4)
#define _BG_COR_OFS_GPR2    (  8)
#define _BG_COR_OFS_GPR3    ( 12)
#define _BG_COR_OFS_GPR4    ( 16)
#define _BG_COR_OFS_GPR5    ( 20)
#define _BG_COR_OFS_GPR6    ( 24)
#define _BG_COR_OFS_GPR7    ( 28)
#define _BG_COR_OFS_GPR8    ( 32)
#define _BG_COR_OFS_GPR9    ( 36)
#define _BG_COR_OFS_GPR10   ( 40)
#define _BG_COR_OFS_GPR11   ( 44)
#define _BG_COR_OFS_GPR12   ( 48)
#define _BG_COR_OFS_GPR13   ( 52)
#define _BG_COR_OFS_GPR14   ( 56)
#define _BG_COR_OFS_GPR15   ( 60)
#define _BG_COR_OFS_GPR16   ( 64)
#define _BG_COR_OFS_GPR17   ( 68)
#define _BG_COR_OFS_GPR18   ( 72)
#define _BG_COR_OFS_GPR19   ( 76)
#define _BG_COR_OFS_GPR20   ( 80)
#define _BG_COR_OFS_GPR21   ( 84)
#define _BG_COR_OFS_GPR22   ( 88)
#define _BG_COR_OFS_GPR23   ( 92)
#define _BG_COR_OFS_GPR24   ( 96)
#define _BG_COR_OFS_GPR25   (100)
#define _BG_COR_OFS_GPR26   (104)
#define _BG_COR_OFS_GPR27   (108)
#define _BG_COR_OFS_GPR28   (112)
#define _BG_COR_OFS_GPR29   (116)
#define _BG_COR_OFS_GPR30   (120)
#define _BG_COR_OFS_GPR31   (124)
//
#define _BG_COR_OFS_FPR0    (128)
#define _BG_COR_OFS_FPR1    (144)
#define _BG_COR_OFS_FPR2    (160)
#define _BG_COR_OFS_FPR3    (176)
#define _BG_COR_OFS_FPR4    (192)
#define _BG_COR_OFS_FPR5    (208)
#define _BG_COR_OFS_FPR6    (224)
#define _BG_COR_OFS_FPR7    (240)
#define _BG_COR_OFS_FPR8    (256)
#define _BG_COR_OFS_FPR9    (272)
#define _BG_COR_OFS_FPR10   (288)
#define _BG_COR_OFS_FPR11   (304)
#define _BG_COR_OFS_FPR12   (320)
#define _BG_COR_OFS_FPR13   (336)
#define _BG_COR_OFS_FPR14   (352)
#define _BG_COR_OFS_FPR15   (368)
#define _BG_COR_OFS_FPR16   (384)
#define _BG_COR_OFS_FPR17   (400)
#define _BG_COR_OFS_FPR18   (416)
#define _BG_COR_OFS_FPR19   (432)
#define _BG_COR_OFS_FPR20   (448)
#define _BG_COR_OFS_FPR21   (464)
#define _BG_COR_OFS_FPR22   (480)
#define _BG_COR_OFS_FPR23   (496)
#define _BG_COR_OFS_FPR24   (512)
#define _BG_COR_OFS_FPR25   (528)
#define _BG_COR_OFS_FPR26   (544)
#define _BG_COR_OFS_FPR27   (560)
#define _BG_COR_OFS_FPR28   (576)
#define _BG_COR_OFS_FPR29   (592)
#define _BG_COR_OFS_FPR30   (608)
#define _BG_COR_OFS_FPR31   (624)
//
#define _BG_COR_OFS_LR      (640)
#define _BG_COR_OFS_CR      (644)
#define _BG_COR_OFS_CTR     (648)
#define _BG_COR_OFS_XER     (652)

#if !defined(__ASSEMBLY__)

#include <stdint.h>
#include <string.h>

#define ALIGN_L1D_CACHE   __attribute__ ((aligned ( 32)))
#define ALIGN_QUADWORD    __attribute__ ((aligned ( 16)))

typedef union _BG_QuadWord_t
               {
               uint32_t ul[ 4];
               float     f[ 4];
               double    d[ 2];
               }
               ALIGN_QUADWORD _BG_QuadWord_t;

//
// CoRoutine State
//   these structs must always be aligned at least 16 bytes, 32 is better, 128 is best.
//   size of this struct must be multiple of 16 bytes.
//   assembler code knows about this struct.
//
typedef struct _BG_CoRoutine_t
                {
                uint32_t        gpr[ _BG_NUM_GPRS ];
                //
                _BG_QuadWord_t  fpr[ _BG_NUM_FPRS ];
                //
                uint32_t        lr,
                                cr,
                                ctr,
                                xer;
                //
                // If needed, add other stuff here.
                // But keep this struct a multiple of 16 bytes.
                //
                }
                ALIGN_L1D_CACHE _BG_CoRoutine_t;

//
// _BG_CoRoutine_Swap: Save current in Co_From, and swap to Co_To
//
extern "C" {
extern void _BG_CoRoutine_Swap( _BG_CoRoutine_t *Co_From, // save current CoRoutine here
                                _BG_CoRoutine_t *Co_To ); // restore and return to this CoRoutine
};

typedef void (*BG_CoRoutineFunctionType)(void*);
//
// _BG_CoRoutine_Init: Call this before swapping to a CoRoutine
//
 inline void _BG_CoRoutine_Init( _BG_CoRoutine_t *Co,
                                  BG_CoRoutineFunctionType Fcn,    /////void (*Fcn)(void*),
                                       void * Arg,
                                       void * Stack )
{
   memset( Co, 0, sizeof(_BG_CoRoutine_t) );

   Co->gpr[3] = (uint32_t)Arg;
   Co->gpr[1] = (uint32_t)Stack;
   Co->lr     = (uint32_t)Fcn;
}

#endif // __ASSEMBLY__

#endif // Add nothing below this line
