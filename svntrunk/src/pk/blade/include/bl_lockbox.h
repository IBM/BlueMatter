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
//      File: blade/include/sys/machine/lockbox.h
//
//   Purpose: Define BG/L Lockbox interface.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: Inlines are compiled away if CONFIG_BLNIE is set.
//
//   History: 07/08/2000: MEG - Created.
//            05/15/2002: MEG - Updated to final Lockbox architecture.
//            05/28/2002: MEG - Fixed barrier code, removed old lockbox arch.
//            07/05/2003: MEG - Increased Strength of Synchronizations for RIT 1.0 H/W.
//
//
#ifndef _LOCKBOX_H // Prevent multiple inclusion
#define _LOCKBOX_H

#include <spi/blade_on_blrts.h>

__BEGIN_DECLS

//
// Under blrts, there are 128 user locks available.  Some have
//  defined uses for this library, the rest are available.
//

// NOTE: Barrier locks numbers should be divisible by 8, and do not touch
//        the next 7 locks with any other functions.
#define _BLADE_LOCK_0      (0)    // reserved for Blade
#define _BLADE_LOCK_1      (1)    // reserved for Blade
#define _BLADE_LOCK_2      (2)    // reserved for Blade
#define _BLADE_LOCK_3      (3)    // reserved for Blade
#define _BLADE_LOCK_4      (4)    // reserved for Blade
#define _BLADE_LOCK_5      (5)    // reserved for Blade
#define _BLADE_LOCK_6      (6)    // reserved for Blade
#define _BLADE_LOCK_7      (7)    // reserved for Blade
#define _BLADE_VNM_BARRIER (8)    // takes up 8-15 inclusive
#define _BLADE_LOCKBOX_AVAIL (16) // you may use locks >= 16 for whatever you want.

// see blade/kernel/bl_lockbox.S
extern int  _blLockBoxQuery( int b );
extern int  _blLockBoxTryLock( int b );
extern void _blLockBoxSpinLock( int b );
extern void _blLockBoxForce( int lock_num, int val );
extern void _blLockBoxBarrier( int b );
#define _blLockBoxForceLock( lock_num ) _blLockBoxForce( (lock_num), 1 )
#define _blLockBoxUnLock( lock_num )    _blLockBoxForce( (lock_num), 0 )


__END_DECLS

#endif // Add nothing below this line
