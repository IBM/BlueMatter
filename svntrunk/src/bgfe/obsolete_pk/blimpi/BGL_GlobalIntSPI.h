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
//      File: blade/spi/BGL_GlobalIntSPI.h
//
//   Purpose: Define Blade's Global Interrupt System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Matthias A. Blumrich (blumrich@us.ibm.com)
//            (c) IBM, 2003
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

__BEGIN_DECLS

/*---------------------------------------------------------------------------*
 *     Global Interrupt Low-Level Interface                                  *
 *---------------------------------------------------------------------------*/

// Use these to construct channel vectors to specify multiple channels in one operation.
#define _BGL_GI_CH0   (0x00000008)
#define _BGL_GI_CH1   (0x00000004)
#define _BGL_GI_CH2   (0x00000002)
#define _BGL_GI_CH3   (0x00000001)

// Information returned by status reads.  Note that userEnables returns the uptree state
// in diagnostic read mode.
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


//////////////////////////
// User-level Functions
//  The following set of functions can be called in user (unprivileged) mode.  They always access
//  the hardware through the upper address image (0xB11000x0).

// Configure specified channel(s) for sticky logical OR or AND mode.
// All of the channels specified must allow user-level control or the call fails.
int BGLGiEnableStickyOr( int channelVector );
int BGLGiEnableStickyAnd( int channelVector );

// Sticky mode.  Activate uptree channel(s) in the previously-configured sticky mode (OR or AND).
// All of the channels specified must allow user-level control or the call fails.  This call sets
// the uptree state.
int BGLGiStartStickyInt( int channelVector );

// Sticky mode.  Deactivate uptree channel(s) in the previously-configured sticky mode (OR or AND),
// and clear the sticky mode interrupt(s).  Note that the uptree channel will usually be deactivated
// already as a result of the downtree interrupt returning.
// All of the channels specified must allow user-level control or the call fails.
int BGLGiClearStickyInt( int channelVector );

// Direct mode.  Configure channel(s) for direct mode, and force the uptree state (0 or 1).
// All of the channels specified must allow user-level control or the call fails.
int BGLGiSetChannelState(
        int  channelVector,    // Channels to force to specified state
        int  state );          // 0 if zero, 1 if non-zero

// Return global interrupt hardware status.
int BGLGiGetStatus( _BGL_GiStatus *stat );


//////////////////////////
// System-level Functions
//  The following set of functions can only be called in system (privileged) mode.  They always access
//  the hardware through the lower address image (0xB01000x0).

// Reset channel(s) to initial state.
int BGLGiResetChannel( int channelVector );

// Set the read mode to normal or diagnostic.
int BGLGiSetReadModeNormal();
int BGLGiSetReadModeDiagnostic();

// Enable and disable user-level (upper address image) access to the specified channel(s).
int BGLGiEnableUserAccess( int channelVector );
int BGLGiDisableUserAccess( int channelVector );

// Configure specified channel(s) for sticky logical OR or AND mode.
int BGLGiEnableStickyOrPrivileged( int channelVector );
int BGLGiEnableStickyAndPrivileged( int channelVector );

// Sticky mode.  Activate uptree channel(s) in the previously-configured sticky mode (OR or AND).
// This sets the uptree state.
int BGLGiStartStickyIntPrivileged( int channelVector );

// Sticky mode.  Deactivate uptree channel(s) in the previously-configured sticky mode (OR or AND),
// and clear the sticky mode interrupt(s).  Note that the uptree channel will usually be deactivated
// already as a result of the downtree interrupt returning.
int BGLGiClearStickyIntPrivileged( int channelVector );

// Direct mode.  Configure channel(s) for direct mode, and force the uptree state (0 or 1).
int BGLGiSetChannelStatePrivileged(
        int  channelVector,    // Channels to force to specified state
        int  state );          // 0 if zero, 1 if non-zero

// Reads status in the lower address range (0xB01000x0)
int BGLGiGetStatusPrivileged( _BGL_GiStatus *stat );


/*---------------------------------------------------------------------------*
 *     Global Interrupt High-Level Interface                                 *
 *---------------------------------------------------------------------------*/

#define _BGL_GI_PENDING    (0x00000000)  // (actual value t.b.d.)
#define _BGL_GI_COMPLETE   (0x00000001)  // (actual value t.b.d.)

typedef Bit32 _BGL_GiHandle;

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
int BGLGiCreateBarrier(
        _BGL_GiHandle barrier,            // Global interrupt to use (2 and 3 are reserved for OS)
        void          (*barrierISR()) );  // Optional ISR to call on completion.  NULL if none.

// Traditional blocking barrier call.  Enables the barrier and returns when it has
// completed.  The barrier should not be created with an ISR!
int BGLGiBarrier( _BGL_GiHandle barrier );

// Enters a barrier (sets uptree state), and returns immediately.  This starts a split
// transaction.
int BGLGiEnableBarrier( _BGL_GiHandle barrier );

// Polls a barrier and returns _BGL_GI_COMPLETE if barrier has completed, _BGL_GI_PENDING if
// not, or an error if the barrier has not been enabled.  Note that the local hardware interrupt
// is polled.
int BGLGiPollBarrier( _BGL_GiHandle barrier );

// Clears the local hardware interrupt caused by a barrier completion.  Since the local
// interrupt flag is what the polling function looks at, subsequent polls after this call
// will return _BGL_GI_PENDING.  Note that this call may effect the BIC, to be decided.
int BGLGiClearBarrier( _BGL_GiHandle barrier );

// Enables the associated ISR by un-masking the interrupt.  Returns an error if there is no
// ISR associated with the barrier, or if the barrier has not been created.  Note that this
// call may effect the BIC, to be decided.
int BGLGiEnableBarrierISR( _BGL_GiHandle barrier );

// Disables the associated ISR by masking the interrupt.  Returns an error if the barrier has
// not been created.  Note that this call may effect the BIC, to be decided.
int BGLGiDisableBarrierISR( _BGL_GiHandle barrier );

// Frees a barrier so that its associated global interrupt can be re-used.  Basically
// dis-associates the global interrupt with any ISR and masks the local hardware interrupt.
// Returns a warning if the barrier has been enabled and has not been cleared.
int BGLGiGFreeBarrier( _BGL_GiHandle barrier );


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
_BGL_GiHandle BGLGiCreateNotification(
        _BGL_GiHandle notification,            // Global interrupt to use (2 and 3 are reserved for OS)
        void          (*notificationISR()) );  // Optional ISR to call on completion.  NULL if none.

// Activates a notification.
int BGLGiSendNotification( _BGL_GiHandle notification );

// Polls a notification and returns _BGL_GI_COMPLETE if it has arrived, _BGL_GI_PENDING if
// not, or an error if the notification has not been created.
int BGLGiPollNotification( _BGL_GiHandle notification );

// Clears the local hardware interrupt caused by a notification arrival.  Since the local
// interrupt flag is what the polling function looks at, subsequent polls after this call
// will return _BGL_GI_PENDING.  Note that this call may effect the BIC, to be decided.
int BGLGiClearNotification( _BGL_GiHandle notification );

// Blocks until the specified notification arrives.
int BGLGiWaitForNotification( _BGL_GiHandle notification );

// Enables the associated ISR by un-masking the interrupt.  Returns an error if there is no
// ISR associated with the notification, or if the notification has not been created.
int BGLGiEnableNotificationISR( _BGL_GiHandle notification );

// Disables the associated ISR by masking the interrupt.  Returns an error if the notification
// has not been created.
int BGLGiDisableNotificationISR( _BGL_GiHandle notification );

// Frees a notification so that its associated global interrupt can be re-used.  Basically
// dis-associates the global interrupt with any ISR and masks the local hardware interrupt.
int BGLGiGFreeNotification( _BGL_GiHandle notification );


__END_DECLS

#endif // Add nothing below this line.
