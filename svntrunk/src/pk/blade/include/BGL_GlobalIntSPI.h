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
//      File: blade/spi/BGL_GlobalIntSPI.h
//
//   Purpose: Define Blade's Global Interrupt System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Matthias A. Blumrich (blumrich@us.ibm.com)
//
//     Notes: This SPI is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the global interrupts.
//
//   History: 03/21/2003: MAB - Created.
//
//
#ifndef _BGL_GLOB_INT_SPI_H  // Prevent multiple inclusion
#define _BGL_GLOB_INT_SPI_H

#if !defined(__ASSEMBLY__)

__BEGIN_DECLS

// Define to make SPI check for obvious errors.  WARNING: THIS REDUCES PERFORMANCE!
//#define _SPI_GI_CHECK

// Define to make SPI announce every call that is made.  WARNING: THIS REDUCES PERFORMANCE!
//#define _SPI_GI_CHECK_INFO

// Don't inline if any diagnostic code is added or on BLC1.0
#if defined( _SPI_GI_CHECK_INFO ) || defined( _SPI_GI_CHECK ) || defined( CONFIG_BLC10 )
#define _SPI_GI_INLINE
#else
#define _SPI_GI_INLINE  inline
#endif

// Define termination behavior
#define _BLADE_ABORT(rc)  exit(rc)

// Worst-case roundtrip GI latency at 700 MHz = (2 us * 700 cyc/us)
#ifdef CONFIG_SIM
#define _BGL_GI_MAX_RT_LATENCY   0       // No timeout in simulation
#else
#define _BGL_GI_MAX_RT_LATENCY   1400    // Max GI roundtrip in CPU cycles
#endif

// Channel specifiers.
#define _BGL_GI_CH0    0x00
#define _BGL_GI_CH1    0x10
#define _BGL_GI_CH2    0x20
#define _BGL_GI_CH3    0x30
#define _BGL_GI_CHx(X) ((X)==0?_BGL_GI_CH0: \
                        (X)==1?_BGL_GI_CH1: \
                        (X)==2?_BGL_GI_CH2: \
                        (X)==3?_BGL_GI_CH3: BL_DATA_CRASH)

// Used in high-level interface.  A handle is one of the channel specifiers.
typedef Bit32 _BGL_GiHandle;

// Used to configure sticky mode channels
#define _BGL_GI_STICKY_OR   (0x0)
#define _BGL_GI_STICKY_AND  (0x1)

// Read mode for status userEnables field
#define _BGL_GI_READMODE_NRML   (0x0)
#define _BGL_GI_READMODE_DIAG   (0x1)

// For high-level interface
#define _BGL_GI_PENDING    (0x00000000)
#define _BGL_GI_COMPLETE   (0x00000001)
#define _BGL_GI_ERROR      (-1)

// Information returned by status reads.  Note that userEnables returns the uptree state in diagnostic read mode.
typedef struct T_BGL_GiStatus
                {
                unsigned mode:4;          // Mode 0-3 (1=sticky, 0=direct)
                unsigned stickyInt:4;     // Sticky mode interrupt 0-3 (1=active)
                unsigned stickyActive:4;  // Sticky mode in progress 0-3 (1=true)
                unsigned stickyType:4;    // Sticky mode type 0-3 (0=OR, 1=AND)
                unsigned readMode:1;      // Read mode (0=normal, 1=diagnostic)
                unsigned p0addrErr:1;     // Processor 0 address error (1=true)
                unsigned p1addrErr:1;     // Processor 1 address error (1=true)
                unsigned parityErr:1;     // Parity error (1=true)
                unsigned giRecvState:4;   // Global interrupt recv state 0-3 (filtered "downtree" state)
                unsigned userEnables:4;   // User-level enables 0-3 (1=enabled).
                                          //   Actual "uptree" state when readMode=1.
                unsigned giSendState:4;   // Global interrupt send state 0-3 ("uptree" state)
                } _BGL_GiStatus;

#define _BGL_GI_PRIV_CHANNEL_START   (2)    // Reserve channels 2-3 for the OS


////////////////////////////////////////////////////////////////////////////////////////////////////
// User-level Functions
//  The following set of functions can be called in user (unprivileged) mode.  They always access
//  the hardware through the upper address image (0xB11000x0).
////////////////////////////////////////////////////////////////////////////////////////////////////

// Configure specified channel for sticky logical OR or AND mode.  The channel specified must allow
// user-level control or the call fails.
void BGLGiEnableStickyOr( int channel );
void BGLGiEnableStickyAnd( int channel );

// Sticky mode.  Activate uptree channel in the previously-configured sticky mode (OR or AND).
// The channels specified must allow user-level control or the call fails.  This call sets
// the uptree state.
void BGLGiStartStickyInt( int channel );

// Sticky mode.  Deactivate uptree channel in the previously-configured sticky mode (OR or AND),
// and clears the sticky mode interrupt.  Note that the uptree channel will usually be deactivated
// already as a result of the downtree interrupt returning.  The channel specified must allow
// user-level control or the call fails.
void BGLGiClearStickyInt( int channel );

// Direct mode.  Configure channel for direct mode, and force the uptree state (0 or 1).
// The channel specified must allow user-level control or the call fails.
void BGLGiSetDirectState(
        int  channel,          // Channel to force to specified state
        int  state );          // 0 if zero, 1 if non-zero

// Return global interrupt hardware status.
_BGL_GiStatus BGLGiGetStatus( void );


////////////////////////////////////////////////////////////////////////////////////////////////////
// System-level Functions
//  The following set of functions can only be called in system (privileged) mode.  They always
//  access the hardware through the lower address image (0xB01000x0).
////////////////////////////////////////////////////////////////////////////////////////////////////

// Reset channel to initial state.
void BGLGiResetChannel( int channel );

// Set the read mode to normal or diagnostic.
void BGLGiSetReadModeNormal( void );
void BGLGiSetReadModeDiagnostic( void );

// Enable and disable user-level (upper address image) access to the specified channel.
void BGLGiEnableUserAccess( int channel );
void BGLGiDisableUserAccess( int channel );

// Configure specified channel for sticky logical OR or AND mode.
void BGLGiEnableStickyOrPrivileged( int channel );
void BGLGiEnableStickyAndPrivileged( int channel );

// Sticky mode.  Activate uptree channel in the previously-configured sticky mode (OR or AND).
// This sets the uptree state.
void BGLGiStartStickyIntPrivileged( int channel );

// Sticky mode.  Deactivate uptree channel in the previously-configured sticky mode (OR or AND),
// and clear the sticky mode interrupt.  Note that the uptree channel will usually be deactivated
// already as a result of the downtree interrupt returning.
void BGLGiClearStickyIntPrivileged( int channel );

// Direct mode.  Configure channel for direct mode, and force the uptree state (0 or 1).
void BGLGiSetDirectStatePrivileged(
        int  channel,          // Channel to force to specified state
        int  state );          // 0 if zero, 1 if non-zero

// Reads status in the lower address range (0xB01000x0)
_BGL_GiStatus BGLGiGetStatusPrivileged( void );

// Set all "previous" barrier and notification timestamps to a specified value
void BGLGiSetAllTimestamps( Bit64 v );


////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Interrupt High-Level Interface
////////////////////////////////////////////////////////////////////////////////////////////////////

// Barriers
//  The global interrupts can be used to create barriers.  In this case, the sticky AND mode
//  is always used, so a single global interrupt suffices.  When a barrier is created, an
//  interrupt service routine (ISR) can be specified.  If so, then the ISR is called when
//  the barrier completes.  In general, this feature will not be used.  Rather, the barrier
//  will be a blocking call or a split transaction with periodic polling for completion.
//
//  Note that the global interrupt is explicitly specified in order to avoid a global
//  operation when creating a barrier.  That is, if the system were to choose a global interrupt
//  and return some sort of handle, then coordination between all nodes would be required.
//  Clearly, this can be implemented with this SPI, if desired.

// Creates a barrier using sticky AND mode.  Returns an error if the global interrupt is
// already assigned. If barrierISR is NULL, then the barrier interrupt will be masked and
// polling or blocking is required.
void BGLGiCreateBarrier(
        _BGL_GiHandle channel,            // Global interrupt to use (2 and 3 are reserved for OS)
        int participating,                // Non-zero if participating; 0 otherwise
        void          (*barrierISR()) );  // Optional ISR to call on completion.  NULL if none.

// Traditional blocking barrier call.  Enables the barrier and returns when it has
// completed.  The barrier should not be created with an ISR!
void BGLGiBarrier( _BGL_GiHandle channel );

// Enters a barrier (sets uptree state), and returns immediately.  This starts a split
// transaction.
void BGLGiEnableBarrier( _BGL_GiHandle channel );

// Polls a barrier and returns _BGL_GI_COMPLETE if barrier has completed, _BGL_GI_PENDING if
// not, or an error if the barrier has not been enabled.  Note that the local hardware interrupt
// is polled.
int BGLGiPollBarrier( _BGL_GiHandle channel );

// Enables the associated ISR by un-masking the interrupt.  Returns an error if there is no
// ISR associated with the barrier, or if the barrier has not been created.  Note that this
// call may effect the BIC, to be decided.
int BGLGiEnableBarrierISR( _BGL_GiHandle channel );

// Disables the associated ISR by masking the interrupt.  Returns an error if the barrier has
// not been created.  Note that this call may effect the BIC, to be decided.
int BGLGiDisableBarrierISR( _BGL_GiHandle channel );

// Frees a barrier so that its associated global interrupt can be re-used.  Basically
// dis-associates the global interrupt with any ISR and masks the local hardware interrupt.
// Returns a warning if the barrier has been enabled and has not been cleared.
int BGLGiGFreeBarrier( _BGL_GiHandle channel );

// Clears the local hardware interrupt caused by a barrier or notification completion.
// Since the local interrupt flag is what the polling function looks at, subsequent calls
// to BGLGiPollBarrier will return an error, and subsequent calls to BGLGiPollNotification
// will return _BGL_GI_PENDING.  Note that this call may effect the BIC, to be decided.
void BGLGiClearInterrupt( _BGL_GiHandle channel );


// Notifications
//  The global interrupts can be used to create notifications, which are basically remote
//  interrupts.  In this case, the sticky OR mode is always used, so a single global interrupt
//  suffices.  When a notification is created, an interrupt service routine (ISR) can be
//  specified.  If so, then the ISR is called when a notification arrives.  Alternatively, the
//  notification can be polled for.
//
//  Note that the global interrupt is explicitly specified in order to avoid a global
//  operation when creating a notification.  That is, if the system were to choose a global
//  interrupt and return some sort of handle, then coordination between all nodes would be
//  required.  Clearly, this can be implemented with this SPI, if desired.

// Creates a notification using sticky OR mode.  Returns a handle or -1 if no global interrupts
// are available.  If notificationISR is NULL, then the interrupt will be masked and polling
// is required.
void BGLGiCreateNotification(
        _BGL_GiHandle notification,            // Global interrupt to use (2 and 3 are reserved for OS)
        int participating,                     // Non-zero if participating; 0 otherwise
        void          (*notificationISR()) );  // Optional ISR to call on completion.  NULL if none.

// Activates a notification.
void BGLGiSendNotification( _BGL_GiHandle channel );

// Polls a notification and returns _BGL_GI_COMPLETE if it has arrived, or _BGL_GI_PENDING if not.
int BGLGiPollNotification( _BGL_GiHandle channel );

// Blocks until the specified notification arrives.
void BGLGiWaitForNotification( _BGL_GiHandle channel );

// Enables the associated ISR by un-masking the interrupt.  Returns an error if there is no
// ISR associated with the notification, or if the notification has not been created.
int BGLGiEnableNotificationISR( _BGL_GiHandle notification );

// Disables the associated ISR by masking the interrupt.  Returns an error if the notification
// has not been created.
int BGLGiDisableNotificationISR( _BGL_GiHandle notification );

// Frees a notification so that its associated global interrupt can be re-used.  Basically
// dis-associates the global interrupt with any ISR and masks the local hardware interrupt.
int BGLGiGFreeNotification( _BGL_GiHandle notification );

// Optimizes barrier timestamps...
// The tree must be initialized and idle.
int BGLGiOptimizeTimestamps( int rt_delta, int vt );

// Gets the status and pretty prints it.
void BGLGiShowStatus(int rank);

__END_DECLS

#endif // __ASSEMBLY__

#endif // Add nothing below this line.
