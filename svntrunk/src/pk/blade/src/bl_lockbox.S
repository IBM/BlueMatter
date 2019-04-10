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
//      File: blade/kernel/bl_lockbox.S
//
//   Purpose: LockBox access routines when not using in-lines.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: Enabled when CONFIG_LOCKBOX_INLINES is OFF.
//
#include <blade_on_blrts.h>

#define LOCKBOX_NUM_LOCKS           (128)  // number of user locks under blrts
#define LOCKBOX_ADDR_SHIFT            (4)
#define LOCKBOX_QUERY_OFFSET          (4)
#define LOCKBOX_SYNC_OFFSET           (8)

// Read=Lock, Write 0 to Clear, Write 1 to force locked.
#define LOCKBOX_LOCK_ADDR(l)   (PA_LOCKBOX | ((l) << LOCKBOX_ADDR_SHIFT))

// Read-Only: Return current state without modification
#define LOCKBOX_QUERY_ADDR(l)  (LOCKBOX_LOCK_ADDR(l) | LOCKBOX_QUERY_OFFSET)

// Barrier Function Address
#define LOCKBOX_SYNC_ADDR(l)   (LOCKBOX_LOCK_ADDR(l) | LOCKBOX_SYNC_OFFSET)


//------------------------------------------------------------------------------
//  _blLockBoxForce: force a lock to a state.
//
//  Syntax:   void _blLockBoxForce( int lock_num, int val );
//
//  Taken:     R3 = lock_num, R4 = val (0 or 1).
//  Returned:  nothing
//  Destroyed: r3, r5
//
        _PROLOG( _blLockBoxForce )
_blLockBoxForce:
        msync
        slwi    %r3,%r3,LOCKBOX_ADDR_SHIFT
        IMM32C( %r5, PA_LOCKBOX )
        stwx    %r4,%r5,%r3
        msync
        blr
        _EPILOG( _blLockBoxForce )

//------------------------------------------------------------------------------
//  _blLockBoxQuery: obtain the state of a lock with no state change.
//
//  Syntax:   void _blLockBoxQuery( int lock_num );
//
//  Taken:     R3 = lock_num.
//  Returned:  R3 = lock state
//  Destroyed: r3, r4, r5
//
        _PROLOG( _blLockBoxQuery )
_blLockBoxQuery:
        msync
        slwi    %r4,%r3,LOCKBOX_ADDR_SHIFT
        IMM32C( %r5, PA_LOCKBOX )
        ori     %r4,%r4,LOCKBOX_QUERY_OFFSET
        lwzx    %r3,%r5,%r4
        msync
        blr
        _EPILOG( _blLockBoxQuery )

//------------------------------------------------------------------------------
//  _blLockBoxTryLock: non-blocking attempt to obtain a lock.
//
//  Syntax:   int _blLockBoxTryLock( int lock_num );
//
//  Taken:     R3 = lock_num.
//  Returned:  R3 = zero to winner, non-zero to loser
//  Destroyed: r3, r4, r5
//
        _PROLOG( _blLockBoxTryLock )
_blLockBoxTryLock:
        msync
        slwi    %r4,%r3,LOCKBOX_ADDR_SHIFT
        IMM32C( %r5, PA_LOCKBOX )
        lwzx    %r3,%r5,%r4
        msync
        blr
        _EPILOG( _blLockBoxTryLock )

//------------------------------------------------------------------------------
//  _blLockBoxSpinLock: block until a lock is obtained.
//
//  Syntax:   void _blLockBoxSpinLock( int lock_num );
//
//  Taken:     R3 = lock_num.
//  Returned:  nothing
//  Destroyed: r3, r4, r5, cr0
//
        _PROLOG( _blLockBoxSpinLock )
_blLockBoxSpinLock:
        msync
        slwi    %r4,%r3,LOCKBOX_ADDR_SHIFT
        IMM32C( %r5, PA_LOCKBOX )
99:     lwzx    %r3,%r5,%r4
        cmpwi   cr0,%r3,0
        bne     99b
        msync
        blr
        _EPILOG( _blLockBoxSpinLock )


//------------------------------------------------------------------------------
//  _blLockBoxBarrier: Perform a local barrier between the 2 processors.
//
//  Syntax:   void _blLockBoxBarrier( int lock_num );
//
//  Taken:     R3 = lock_num.
//  Returned:  nothing
//  Destroyed: r0,r3, r4, r5
//
        _PROLOG( _blLockBoxBarrier )
_blLockBoxBarrier:
        msync
        li      %r0,0
        slwi    %r3,%r3,LOCKBOX_ADDR_SHIFT
        IMM32C( %r5, PA_LOCKBOX )
        ori     %r3,%r3,LOCKBOX_SYNC_OFFSET
        stwx    %r0,%r5,%r3
        msync
98:     lwzx    %r0,%r5,%r3
        cmpwi   cr0,%r0,0
        bne     98b
        msync
        blr
        _EPILOG( _blLockBoxBarrier )

