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
//      File: blade/include/sys/machine/bl_gints.h
//
//   Purpose: Define the Global AND/OR Interface.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes:
//
//   History: 03/17/2002: MEG - Created for GI interface.
//
//
#ifndef _BL_GINTS_H // Prevent multiple inclusion
#define _BL_GINTS_H

// Channel 2 is a barrier for all nodes and channel 3 is a barriers for compute nodes only.
#define _BGL_GI_BARRIER_ALL_NODES       (_BGL_GI_CH2)
#define _BGL_GI_BARRIER_COMPUTE_NODES   (_BGL_GI_CH3)

// Notification over all nodes uses channels 0 and 1.
#define _BGL_GI_NOTIFICATION_ABORT      (_BGL_GI_CH0)
#define _BGL_GI_NOTIFICATION_ALERT      (_BGL_GI_CH1)

// For global interrupt usage, see the header file BGL_GlobalIntSPI.h.  Briefly,
// 1. After initialization, the OS global barrier is called as follows:
//       BGLGiBarrier( _BGL_GI_SYS_BARRIER );
// 2. And the OS global notification is activated as follows:
//       BGLGiSendNotification( _BGL_GI_SYS_NOTIFICATION );
// 3. And the OS global notification is polled as follows:
//       result = BGLGiPollNotification( _BGL_GI_SYS_NOTIFICATION );
// 4. And the OS global notification can be blocked on as follows:
//       BGLGiWaitForNotification( _BGL_GI_SYS_NOTIFICATION );


// Offsets of memory-mapped registers:
#define _GI0_DIRECT                0x00
#define _GI1_DIRECT                0x10
#define _GI2_DIRECT                0x20
#define _GI3_DIRECT                0x30
#define _GI_READ_MODE              0x40
#define _GI_RESET                  0x50
#define _GI_USR_PERMIT             0x60
#define _GI0_GINT_CTRL             0x80
#define _GI1_GINT_CTRL             0x90
#define _GI2_GINT_CTRL             0xa0
#define _GI3_GINT_CTRL             0xb0
#define _GI0_GINT_TYPE             0xc0
#define _GI1_GINT_TYPE             0xd0
#define _GI2_GINT_TYPE             0xe0
#define _GI3_GINT_TYPE             0xf0

// Memory-mapped registers that are accessible from the privileged image:
#define GI_SYS_STATUS             ( PA_GI_SYS | _GI0_DIRECT )       // Returns status when read

// Mark, is there already some illegal data address?
#define BL_DATA_CRASH 0xFFFFFFFF

#define GI_SYS_CH0_DIRECT_CTRL    ( PA_GI_SYS | _GI0_DIRECT )
#define GI_SYS_CH1_DIRECT_CTRL    ( PA_GI_SYS | _GI1_DIRECT )
#define GI_SYS_CH2_DIRECT_CTRL    ( PA_GI_SYS | _GI2_DIRECT )
#define GI_SYS_CH3_DIRECT_CTRL    ( PA_GI_SYS | _GI3_DIRECT )
#define GI_SYS_CHx_DIRECT_CTRL(X) ((X)==0?GI_SYS_CH0_DIRECT_CTRL: \
                                   (X)==1?GI_SYS_CH1_DIRECT_CTRL: \
                                   (X)==2?GI_SYS_CH2_DIRECT_CTRL: \
                                   (X)==3?GI_SYS_CH3_DIRECT_CTRL: BL_DATA_CRASH)

#define GI_SYS_SET_READ_MODE      ( PA_GI_SYS | _GI_READ_MODE )
#define GI_SYS_RESET              ( PA_GI_SYS | _GI_RESET )
#define GI_SYS_PERMIT_USR         ( PA_GI_SYS | _GI_USR_PERMIT )

#define GI_SYS_CH0_STICKY_CTRL    ( PA_GI_SYS | _GI0_GINT_CTRL )
#define GI_SYS_CH1_STICKY_CTRL    ( PA_GI_SYS | _GI1_GINT_CTRL )
#define GI_SYS_CH2_STICKY_CTRL    ( PA_GI_SYS | _GI2_GINT_CTRL )
#define GI_SYS_CH3_STICKY_CTRL    ( PA_GI_SYS | _GI3_GINT_CTRL )
#define GI_SYS_CHx_STICKY_CTRL(X) ((X)==0?GI_SYS_CH0_STICKY_CTRL: \
                                   (X)==1?GI_SYS_CH1_STICKY_CTRL: \
                                   (X)==2?GI_SYS_CH2_STICKY_CTRL: \
                                   (X)==3?GI_SYS_CH3_STICKY_CTRL: BL_DATA_CRASH)

#define GI_SYS_CH0_STICKY_TYPE    ( PA_GI_SYS | _GI0_GINT_TYPE )
#define GI_SYS_CH1_STICKY_TYPE    ( PA_GI_SYS | _GI1_GINT_TYPE )
#define GI_SYS_CH2_STICKY_TYPE    ( PA_GI_SYS | _GI2_GINT_TYPE )
#define GI_SYS_CH3_STICKY_TYPE    ( PA_GI_SYS | _GI3_GINT_TYPE )
#define GI_SYS_CHx_STICKY_TYPE(X) ((X)==0?GI_SYS_CH0_STICKY_TYPE: \
                                   (X)==1?GI_SYS_CH1_STICKY_TYPE: \
                                   (X)==2?GI_SYS_CH2_STICKY_TYPE: \
                                   (X)==3?GI_SYS_CH3_STICKY_TYPE: BL_DATA_CRASH)

// Memory-mapped registers that are accessible from the non-privileged image:
#define GI_USR_STATUS             ( PA_GI_USR | _GI0_DIRECT )        // Returns status when read

#define GI_USR_CH0_DIRECT_CTRL    ( PA_GI_USR | _GI0_DIRECT )
#define GI_USR_CH1_DIRECT_CTRL    ( PA_GI_USR | _GI1_DIRECT )
#define GI_USR_CH2_DIRECT_CTRL    ( PA_GI_USR | _GI2_DIRECT )
#define GI_USR_CH3_DIRECT_CTRL    ( PA_GI_USR | _GI3_DIRECT )
#define GI_USR_CHx_DIRECT_CTRL(X) ((X)==0?GI_USR_CH0_DIRECT_CTRL: \
                                   (X)==1?GI_USR_CH1_DIRECT_CTRL: \
                                   (X)==2?GI_USR_CH2_DIRECT_CTRL: \
                                   (X)==3?GI_USR_CH3_DIRECT_CTRL: BL_DATA_CRASH)

#define GI_USR_CH0_STICKY_CTRL    ( PA_GI_USR | _GI0_GINT_CTRL )
#define GI_USR_CH1_STICKY_CTRL    ( PA_GI_USR | _GI1_GINT_CTRL )
#define GI_USR_CH2_STICKY_CTRL    ( PA_GI_USR | _GI2_GINT_CTRL )
#define GI_USR_CH3_STICKY_CTRL    ( PA_GI_USR | _GI3_GINT_CTRL )
#define GI_USR_CHx_STICKY_CTRL(X) ((X)==0?GI_USR_CH0_STICKY_CTRL: \
                                   (X)==1?GI_USR_CH1_STICKY_CTRL: \
                                   (X)==2?GI_USR_CH2_STICKY_CTRL: \
                                   (X)==3?GI_USR_CH3_STICKY_CTRL: BL_DATA_CRASH)

#define GI_USR_CH0_STICKY_TYPE    ( PA_GI_USR | _GI0_GINT_TYPE )
#define GI_USR_CH1_STICKY_TYPE    ( PA_GI_USR | _GI1_GINT_TYPE )
#define GI_USR_CH2_STICKY_TYPE    ( PA_GI_USR | _GI2_GINT_TYPE )
#define GI_USR_CH3_STICKY_TYPE    ( PA_GI_USR | _GI3_GINT_TYPE )
#define GI_USR_CHx_STICKY_TYPE(X) ((X)==0?GI_USR_CH0_STICKY_TYPE: \
                                   (X)==1?GI_USR_CH1_STICKY_TYPE: \
                                   (X)==2?GI_USR_CH2_STICKY_TYPE: \
                                   (X)==3?GI_USR_CH3_STICKY_TYPE: BL_DATA_CRASH)

#ifdef __KERNEL__


#endif // __KERNEL__


#ifndef __ASSEMBLY__

__BEGIN_DECLS

extern int _blade_gints_init( void );

typedef int (*_BL_GI_Handler)( void );

// see kernel/bl_gints.c
typedef struct T_GI_Info
                {
                int abort_count;
                int alert_count;
                _BL_GI_Handler abort_fcn;
                _BL_GI_Handler alert_fcn;
                Bit32 pad[4];
                }
                _GI_Info;

extern _GI_Info _gi_info;

extern void _blade_gints_Abort();
extern void _blade_gints_Alert();

extern int _blade_SC2C_GI_Abort_Handler( void *data, int data_len );
extern int _blade_SC2C_GI_Alert_Handler( void *data, int data_len );


__END_DECLS

#endif // __ASSEMBLY__


#endif // Add nothing below this line.
