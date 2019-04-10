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
//      File: blade/spi/BGLGi.c
//
//   Purpose: Implement Blade's Global Interrupt System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Matthias A. Blumrich (blumrich@us.ibm.com)
//            Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: This SPI is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the Tree.
//
#include <BonB/blade_on_blrts.h>
#include <stdlib.h> // for exit
#include <time.h>
#include <string.h>  // for memcpy
#include <BonB/BGL_GlobalIntSPI.h>

// Keeps track of automatic channel allocation.
Bit32 __BGL_Gi_ChannelAllocation = 0;

// The cycles to wait between barriers for a previous barrier to complete.
Bit64 __BGL_Gi_BarrierWait = _BGL_GI_MAX_RT_LATENCY;

// Keeps track of the last time a barrier was completed or created.
Bit64 __BGL_Gi_PreviousBarrierCycle[4] ALIGN_L1_CACHE;

// Global for status reads to simplify inlines
volatile Bit32 __BGL_Gi_FlatStatus;
volatile _BGL_GiStatus *__BGL_Gi_Status = (_BGL_GiStatus *)&__BGL_Gi_FlatStatus;

// Macro to convert a channel argument to a bit position for masking and selecting.
#define __BGL_Gi_ChannelMask( channel )  (0x8 >> ((channel) >> 4))

// Macro to check if channel argument is 0x00, 0x10, 0x20, or 0x30
#define __BGL_Gi_VerifyChannelArgument  \
   if( channel & 0xffffffcf ) { \
      GiSPI_TRC( GiSPI_TRC_SYS,("(E) "FN_NAME": Invalid channel (0x%.8x) specified.\n", channel )); \
   }

// Macro to check if user mode access is enabled
#define __BGL_Gi_VerifyUserModePermission  \
{ \
   _BGL_GiStatus status = BGLGiGetStatus(); \
   if( status.readMode ) { \
      BGLGiSetReadModeNormal(); \
      status = BGLGiGetStatus(); \
      BGLGiSetReadModeDiagnostic(); \
   } \
   if( !(status.userEnables & (0x8 >> (channel >> 4))) ) { \
      GiSPI_TRC( GiSPI_TRC_SYS,("(E) "FN_NAME": User-mode access violation (channel=0x%.8x).\n", channel )); \
   } \
}

/*---------------------------------------------------------------------------*
 *     Global Interrupt Low-Level Interface                                  *
 *---------------------------------------------------------------------------*/

//////////////////////////
// User-level Functions
//  The following set of functions can be called in user (unprivileged) mode.  They always access
//  the hardware through the upper address image (0xB11000x0).

// Return global interrupt hardware status.
inline _BGL_GiStatus BGLGiGetStatus( void )
{
   __BGL_Gi_FlatStatus = in32( GI_USR_STATUS );
   return( *__BGL_Gi_Status );
}

// Configure specified channel(s) for sticky logical OR mode.  The channel specified must allow user-level control
// or the call fails.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableStickyOr"
_SPI_GI_INLINE
void BGLGiEnableStickyOr( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   __BGL_Gi_VerifyUserModePermission
#endif

   *( (Bit32 *)(GI_USR_CH0_STICKY_TYPE | channel) ) = _BGL_GI_STICKY_OR;
}

// Configure specified channel(s) for sticky logical AND mode.  The channel specified must allow user-level control
// or the call fails.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableStickyAnd"
_SPI_GI_INLINE
void BGLGiEnableStickyAnd( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   __BGL_Gi_VerifyUserModePermission
#endif

   *( (Bit32 *)(GI_USR_CH0_STICKY_TYPE | channel) ) = _BGL_GI_STICKY_AND;
}

// Activate uptree channel(s) in the previously-configured sticky mode (OR or AND).  The channel specified must
// allow user-level control or the call fails.
#undef  FN_NAME
#define FN_NAME "BGLGiStartStickyInt"
_SPI_GI_INLINE
void BGLGiStartStickyInt( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   __BGL_Gi_VerifyUserModePermission
#endif

   *( (Bit32 *)(GI_USR_CH0_STICKY_CTRL | channel) ) = 1;
}

// Deactivate uptree channel(s) in the previously-configured sticky mode (OR or AND), and clear the sticky mode
// interrupt(s).  Note that the uptree channel will usually be deactivated already as a result of the downtree
// interrupt returning.  The channel specified must allow user-level control or the call fails.
#undef  FN_NAME
#define FN_NAME "BGLGiClearStickyInt"
_SPI_GI_INLINE
void BGLGiClearStickyInt( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   __BGL_Gi_VerifyUserModePermission
#endif

   *( (Bit32 *)(GI_USR_CH0_STICKY_CTRL | channel) ) = 0;
}

// Configure channel for direct mode, and force the uptree state (0 or 1).  The channel specified must allow
// user-mode control or the call fails.
#undef  FN_NAME
#define FN_NAME "BGLGiSetDirectState"
_SPI_GI_INLINE
void BGLGiSetDirectState( int channel, int state )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x, %d ) called\n", channel, state ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   __BGL_Gi_VerifyUserModePermission
   // State argument 0 or 1?
   if( (state != 0) && (state != 1) ) {
      GiSPI_TRC( GiSPI_TRC_SYS,("(E) "FN_NAME": Invalid state (%d) specified.\n", state ));
   }
#endif

   *( (Bit32 *)(GI_USR_CH0_DIRECT_CTRL | channel) ) = state;
}


//////////////////////////
// System-level Functions
//  The following set of functions can only be called in system (privileged) mode.  They always access
//  the hardware through the lower address image (0xB01000x0).

// Reads status in the lower address range (0xB01000x0)
inline _BGL_GiStatus BGLGiGetStatusPrivileged( void )
{
   __BGL_Gi_FlatStatus = in32( GI_SYS_STATUS );
   return( *__BGL_Gi_Status );
}

// Reset channel to initial state.
#undef  FN_NAME
#define FN_NAME "BGLGiResetChannel"
_SPI_GI_INLINE
void BGLGiResetChannel( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   *( (Bit32 *)GI_SYS_RESET ) = __BGL_Gi_ChannelMask(channel);
}

// Set the read mode to normal or diagnostic.
#undef  FN_NAME
#define FN_NAME "BGLGiSetReadModeNormal"
_SPI_GI_INLINE
void BGLGiSetReadModeNormal( void )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"() called\n" ));
#endif

   *( (Bit32 *)GI_SYS_SET_READ_MODE ) = ( _BGL_GI_READMODE_NRML >> 30 );
}

#undef  FN_NAME
#define FN_NAME "BGLGiSetReadModeDiagnostic"
_SPI_GI_INLINE
void BGLGiSetReadModeDiagnostic( void )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"() called\n" ));
#endif

   *( (Bit32 *)GI_SYS_SET_READ_MODE ) = ( _BGL_GI_READMODE_DIAG >> 30 );
}

// Enable user-level (upper address image) access to the specified channel.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableUserAccess"
_SPI_GI_INLINE
void BGLGiEnableUserAccess( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   *( (Bit32 *)GI_SYS_PERMIT_USR ) = __BGL_Gi_ChannelMask(channel);
}

// Disable user-level (upper address image) access to the specified channel.
#undef  FN_NAME
#define FN_NAME "BGLGiDisableUserAccess"
_SPI_GI_INLINE
void BGLGiDisableUserAccess( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   *( (Bit32 *)GI_SYS_PERMIT_USR ) = ~__BGL_Gi_ChannelMask(channel);
}

// Configure specified channel for sticky logical OR mode.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableStickyOrPrivileged"
_SPI_GI_INLINE
void BGLGiEnableStickyOrPrivileged( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   *( (Bit32 *)(GI_SYS_CH0_STICKY_TYPE | channel) ) = _BGL_GI_STICKY_OR;
}

// Configure specified channel for sticky logical OR mode.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableStickyAndPrivileged"
_SPI_GI_INLINE
void BGLGiEnableStickyAndPrivileged( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   *( (Bit32 *)(GI_SYS_CH0_STICKY_TYPE | channel) ) = _BGL_GI_STICKY_AND;
}

// Activate uptree channel in the previously-configured sticky mode (OR or AND).
#undef  FN_NAME
#define FN_NAME "BGLGiStartStickyIntPrivileged"
_SPI_GI_INLINE
void BGLGiStartStickyIntPrivileged( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   *( (Bit32 *)(GI_SYS_CH0_STICKY_CTRL | channel) ) = 1;
}

// Deactivate uptree channel in the previously-configured sticky mode (OR or AND), and clear the sticky mode
// interrupt.  Note that the uptree channel will usually be deactivated already as a result of the downtree
// interrupt returning.
#undef  FN_NAME
#define FN_NAME "BGLGiClearStickyIntPrivileged"
_SPI_GI_INLINE
void BGLGiClearStickyIntPrivileged( int channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   *( (Bit32 *)(GI_SYS_CH0_STICKY_CTRL | channel) ) = 0;
}

// Configure channel for direct mode, and force the uptree state (0 or 1).
#undef  FN_NAME
#define FN_NAME "BGLGiSetDirectStatePrivileged"
_SPI_GI_INLINE
void BGLGiSetDirectStatePrivileged( int channel, int state )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_SYS,("(I) "FN_NAME"( 0x%x, %d ) called\n", channel, state ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   // State argument 0 or 1?
   if( (state != 0) && (state != 1) ) {
      GiSPI_TRC( GiSPI_TRC_SYS,("(E) "FN_NAME": Invalid state (%d) specified.\n", state ));
   }
#endif

   *( (Bit32 *)(GI_SYS_CH0_DIRECT_CTRL | channel) ) = state;
}

/*---------------------------------------------------------------------------*
 *     Global Interrupt High-Level Interface                                 *
 *---------------------------------------------------------------------------*/

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

// Set all "previous" barrier and notification timestamps to a specified value
void BGLGiSetAllTimestamps( Bit64 v )
{
   dcache_invalidate_line( __BGL_Gi_PreviousBarrierCycle );
   __BGL_Gi_PreviousBarrierCycle[0] = v;
   __BGL_Gi_PreviousBarrierCycle[1] = v;
   __BGL_Gi_PreviousBarrierCycle[2] = v;
   __BGL_Gi_PreviousBarrierCycle[3] = v;
   dcache_store_line( __BGL_Gi_PreviousBarrierCycle );
}

// Creates a barrier using sticky AND mode.  The channel must have been previously allocated,
// but need not be configured.  If barrierISR is NULL, then the barrier interrupt will be masked
// and polling or blocking is required.
#undef  FN_NAME
#define FN_NAME "BGLGiCreateBarrier"
void BGLGiCreateBarrier( _BGL_GiHandle channel, int participating, void (*barrierISR()) )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x, %d, 0x%d ) called\n", channel, participating, (int *)*barrierISR ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   // Synchronize all nodes using pre-defined barrier.  Configure for sticky AND in case this
   // is the first time here and the global barrier has not yet been created!
   BGLGiEnableStickyAndPrivileged( _BGL_GI_BARRIER_ALL_NODES );

   BGLGiBarrier( _BGL_GI_BARRIER_ALL_NODES );

   // Non-participating nodes force the channel low.  Others, high.
   if( !participating ) {
      BGLGiSetDirectStatePrivileged( channel, 0 );
   } else {
      BGLGiSetDirectStatePrivileged( channel, 1 );

      // Enable sticky AND mode
      BGLGiEnableStickyAndPrivileged( channel );
   }

   // Re-enable sticky AND on global barrier in case that is the barrier being created and it was
   // put in direct mode above!
   BGLGiEnableStickyAndPrivileged( _BGL_GI_BARRIER_ALL_NODES );

   // Synchronize all nodes again.
   BGLGiBarrier( _BGL_GI_BARRIER_ALL_NODES );

   // Clear the sticky bit, just to be nice...
   BGLGiClearStickyIntPrivileged( channel );

   // Timestamp
   __BGL_Gi_PreviousBarrierCycle[ channel >> 4 ] = GetTimeBase();
   dcache_store_line( __BGL_Gi_PreviousBarrierCycle );

   // TO DO: ISR CALL SETUP CODE HERE
}

// Traditional blocking barrier call.  Enables the barrier and returns when it has
// completed.  The barrier should not be created with an ISR!
#undef  FN_NAME
#define FN_NAME "BGLGiBarrier"
void BGLGiBarrier( _BGL_GiHandle channel )
{
   int    state;

#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   BGLGiEnableBarrier( channel );

   // Poll sticky bit interrupt for completion
   do {
      state = BGLGiPollBarrier( channel );
      if( state == _BGL_GI_ERROR ) {
         GiSPI_TRC( GiSPI_TRC_SYS,("(E) "FN_NAME": polling error on channel 0x%x.\n", channel ));
         _BLADE_ABORT( -1 );
      }
   } while( state != _BGL_GI_COMPLETE );

   if( _blTestInt_BLC_Version() > 0 ) {
      // Clear the interrupt
      BGLGiClearStickyIntPrivileged( channel );
   }

   // Timestamp
   __BGL_Gi_PreviousBarrierCycle[ channel >> 4 ] = GetTimeBase();
   dcache_store_line( __BGL_Gi_PreviousBarrierCycle );
}

// Enters a barrier (sets uptree state), and returns immediately.  This starts a split transaction.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableBarrier"
void BGLGiEnableBarrier( _BGL_GiHandle channel )
{
   long long timeout;
   _BGL_GiStatus status;

#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   // Wait for previous barrier to clear
   dcache_invalidate_line( __BGL_Gi_PreviousBarrierCycle );
   timeout = (long long)__BGL_Gi_PreviousBarrierCycle[ channel >> 4 ] + (long long)__BGL_Gi_BarrierWait;
   while( (long long)GetTimeBase() < timeout ) ;

   if( _blTestInt_BLC_Version() == 0 ) {
      // Make sure it has. Use direct mode to poll downtree wire state.
      BGLGiSetDirectStatePrivileged( channel, 1 );
   }

   status = BGLGiGetStatusPrivileged();
   if( !(status.giRecvState & __BGL_Gi_ChannelMask(channel)) ) {
      GiSPI_TRC( GiSPI_TRC_SYS,("(E) "FN_NAME": Previous barrier on channel %.1x has not ended after %d cycles (downtree wire is still low).\n", channel>>4, (int)__BGL_Gi_BarrierWait ));
      _BLADE_ABORT( -1 );
   }

   if( _blTestInt_BLC_Version() == 0 ) {
      // Enable sticky AND mode
      BGLGiEnableStickyAndPrivileged( channel );
   }

   // Clear previous sticky bit and enable the barrier
   BGLGiStartStickyIntPrivileged( channel );
}

// Polls a barrier and returns _BGL_GI_COMPLETE if barrier has completed, _BGL_GI_PENDING if
// not, or an error if the barrier has not been enabled.  Note that the local hardware interrupt
// is polled.
#undef  FN_NAME
#define FN_NAME "BGLGiPollBarrier"
int BGLGiPollBarrier( _BGL_GiHandle channel )
{
   _BGL_GiStatus status;

#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   status = BGLGiGetStatusPrivileged();
   if( status.stickyActive & __BGL_Gi_ChannelMask(channel) )
      return( _BGL_GI_PENDING );
   if( status.stickyInt & __BGL_Gi_ChannelMask(channel) )
      return( _BGL_GI_COMPLETE );
   return( _BGL_GI_ERROR );
}


#if 0
// Enables the associated ISR by un-masking the interrupt.  Returns an error if there is no
// ISR associated with the barrier, or if the barrier has not been created.  Note that this
// call may effect the BIC, to be decided.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableBarrierISR"
int BGLGiEnableBarrierISR( _BGL_GiHandle channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

   GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": Not implemented\n" ));
   _BLADE_ABORT( -1 );
   return( -1 );
}
#endif


#if 0
// Disables the associated ISR by masking the interrupt.  Returns an error if the barrier has
// not been created.  Note that this call may effect the BIC, to be decided.
#undef  FN_NAME
#define FN_NAME "BGLGiDisableBarrierISR"
int BGLGiDisableBarrierISR( _BGL_GiHandle channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

   GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": Not implemented\n" ));
   _BLADE_ABORT( -1 );
   return( -1 );
}
#endif


#if 0
// Frees a barrier so that its associated global interrupt can be re-used.  Basically
// dis-associates the global interrupt with any ISR and masks the local hardware interrupt.
// Returns a warning if the barrier has been enabled and has not been cleared.
#undef  FN_NAME
#define FN_NAME "BGLGiFreeBarrier"
int BGLGiGFreeBarrier( _BGL_GiHandle channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

   GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": Not implemented\n" ));
   _BLADE_ABORT( -1 );
   return( -1 );
}
#endif


// Clears the local hardware interrupt caused by a barrier or notification completion.
// Since the local interrupt flag is what the polling function looks at, subsequent calls
// to BGLGiPollBarrier will return an error, and subsequent calls to BGLGiPollNotification
// will return _BGL_GI_PENDING.  Note that this call may effect the BIC, to be decided.
#undef  FN_NAME
#define FN_NAME "BGLGiClearInterrupt"
void BGLGiClearInterrupt( _BGL_GiHandle channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   // Clear the hardware interrupt at the GI device
   BGLGiClearStickyIntPrivileged( channel );
}


// Notifications
//  The global interrupts can be used to create notifications, which are basically remote
//  interrupts.  In this case, the sticky OR mode is always used, but 2 global interrupts are
//  required because every notification must be followed by a dedicated barrier.
//  When a notification is created, an interrupt service routine (ISR) can be
//  specified.  If so, then the ISR is called when a notification arrives.  Alternatively, the
//  notification can be polled for.
//
//  Note that the global interrupt is explicitly specified in order to avoid a global
//  operation when creating a notification.  That is, if the system were to choose a global
//  interrupt and return some sort of handle, then coordination between all nodes would be
//  required.  Clearly, this can be implemented with this SPI, if desired.
//
//  Creates a notification using sticky OR mode.  Two channels are used: the specified channel and
//  the one immediately following in numerically increasing order.  Both of these channels must have
//  been previously allocated, but need not be configured.  If notificationISR is NULL, then the
//  interrupt will be masked and polling is required.
//
//  A spurious interrupt may occur during the creation of a notification.  Therefore, interrupts
//  should be disabled when calling this function.
#undef  FN_NAME
#define FN_NAME "BGLGiCreateNotification"
void BGLGiCreateNotification( _BGL_GiHandle channel, int participating, void (*notificationISR()) )
{

#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x, %d, 0x%d ) called\n", channel, participating, (int *)*notificationISR ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   if( ( channel + 0x10 ) > 0x30 ) {
      GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": channel 0x%x cannot be used because channel 0x%x is invalid.\n", channel, channel + 0x10 ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Disarm (and clear the channel interrupt)
   BGLGiClearStickyIntPrivileged( channel );

   // Configure the channel for global OR
   BGLGiEnableStickyOrPrivileged( channel );

   // NOTE: It is possible to receive a spurious interrupt here if the downtree wire is high!

#if 0
   // Create the barrier associated with this notification.  BGLGiCreateBarrier will also barrier
   // twice, seperated by the roundtrip tree latency.
   BGLGiCreateBarrier( channel + 0x10, 1, NULL );
#endif

   // Reset the previous notification timestamp now that all nodes are synchronized.
   __BGL_Gi_PreviousBarrierCycle[ channel >> 4 ] = GetTimeBase();
   dcache_store_line( __BGL_Gi_PreviousBarrierCycle );

   // Because the barrier creation delayed by the roundtrip latency, it is now safe to clear the channel
   // interrupt, incase one was incorrectly received
   BGLGiClearStickyIntPrivileged( channel );

   // TO DO: ISR CALL SETUP CODE HERE
}

// Activates a notification.
#undef  FN_NAME
#define FN_NAME "BGLGiSendNotification"
void BGLGiSendNotification( _BGL_GiHandle channel )
{
   Bit64 timeout;
   _BGL_GiStatus status;

#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
#endif

   // Wait for previous notification to clear
   dcache_invalidate_line( __BGL_Gi_PreviousBarrierCycle );
   timeout = __BGL_Gi_PreviousBarrierCycle[ channel >> 4 ] + __BGL_Gi_BarrierWait;
   while( GetTimeBase() < timeout ) ;

   if( _blTestInt_BLC_Version() == 0 ) {
      // Use direct mode to poll downtree wire state.
      BGLGiSetDirectStatePrivileged( channel, 1 );
   }

   // Make sure it has.
   status = BGLGiGetStatusPrivileged();
   if( status.giRecvState & __BGL_Gi_ChannelMask(channel) ) {
      GiSPI_TRC( GiSPI_TRC_SYS,("(E) "FN_NAME": Previous notification has not ended after %d cycles.\n", (int)__BGL_Gi_BarrierWait ));
      _BLADE_ABORT( -1 );
   }

   if( _blTestInt_BLC_Version() == 0 ) {
      // Return to sticky OR mode
      BGLGiEnableStickyOrPrivileged( channel );
   }

   // Clear previous sticky bit and enable the notification
   BGLGiStartStickyIntPrivileged( channel );
}

// Polls a notification and returns _BGL_GI_COMPLETE if it has arrived, or _BGL_GI_PENDING if not.
#undef  FN_NAME
#define FN_NAME "BGLGiPollNotification"
int BGLGiPollNotification( _BGL_GiHandle channel )
{
   _BGL_GiStatus status;

#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   if( ( channel + 0x10 ) > 0x30 ) {
      GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": channel 0x%x cannot be used because channel 0x%x is invalid.\n", channel, channel + 0x10 ));
      _BLADE_ABORT( -1 );
   }
#endif

   status = BGLGiGetStatusPrivileged();
   if( status.stickyInt & __BGL_Gi_ChannelMask(channel) ) {
      return( _BGL_GI_COMPLETE );
   } else {
      return( _BGL_GI_PENDING );
   }
}

// Blocks until the specified notification arrives.
#undef  FN_NAME
#define FN_NAME "BGLGiWaitForNotification"
void BGLGiWaitForNotification( _BGL_GiHandle channel )
{
   int state;

#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

#ifdef _SPI_GI_CHECK
   __BGL_Gi_VerifyChannelArgument
   if( ( channel + 0x10 ) > 0x30 ) {
      GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": channel 0x%x cannot be used because channel 0x%x is invalid.\n", channel, channel + 0x10 ));
      _BLADE_ABORT( -1 );
   }
#endif

   // Wait for an interrupt to arrive
   do {
      state = BGLGiPollNotification( channel );
   } while( state == _BGL_GI_PENDING );

   // Use associated barrier to synchronize and complete
   BGLGiBarrier( channel + 0x10 );

   // Clear the interrupt
   BGLGiClearStickyIntPrivileged( channel );

   // Timestamp
   __BGL_Gi_PreviousBarrierCycle[ channel >> 4 ] = GetTimeBase();
   dcache_store_line( __BGL_Gi_PreviousBarrierCycle );
}


#if 0
// Enables the associated ISR by un-masking the interrupt.  Returns an error if there is no
// ISR associated with the notification, or if the notification has not been created.
#undef  FN_NAME
#define FN_NAME "BGLGiEnableNotificationISR"
int BGLGiEnableNotificationISR( _BGL_GiHandle channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

   GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": Not implemented\n" ));
   _BLADE_ABORT( -1 );
   return( -1 );
}
#endif


#if 0
// Disables the associated ISR by masking the interrupt.  Returns an error if the notification
// has not been created.
#undef  FN_NAME
#define FN_NAME "BGLGiDisableNotificationISR"
int BGLGiDisableNotificationISR( _BGL_GiHandle channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

   GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": Not implemented\n" ));
   _BLADE_ABORT( -1 );
   return( -1 );
}
#endif


#if 0
// Frees a notification so that its associated global interrupt can be re-used.  Basically
// dis-associates the global interrupt with any ISR and masks the local hardware interrupt.
#undef  FN_NAME
#define FN_NAME "BGLGiFreeNotification"
int BGLGiGFreeNotification( _BGL_GiHandle channel )
{
#ifdef _SPI_GI_CHECK_INFO
   GiSPI_TRC( GiSPI_TRC_USR,("(I) "FN_NAME"( 0x%x ) called\n", channel ));
#endif

   GiSPI_TRC( GiSPI_TRC_USR,("(E) "FN_NAME": Not implemented\n" ));
   _BLADE_ABORT( -1 );
   return( -1 );
}
#endif

// The tree must be initialized and idle.
#undef  FN_NAME
#define FN_NAME "BGLGiOptimizeTimestamps"
int BGLGiOptimizeTimestamps( int rt_delta, int vt )
{
   int vc = 0;
   int max_packet[2];
   int my_rank = BGLPartitionGetRank();
   union {
      _BGL_TreeHwHdr hw;
      Bit32          flat;
   } snd_hdr;


   if( rt_delta > 32767 ) {
      printf( "(E) "FN_NAME": roudtrip latency (rt_delta=%d) exceeds 32767.\n", rt_delta );
      return( -1 );
   }

   max_packet[0] = rt_delta;

   // Prepare for MINLOC of rt_deltas, assuming rt_delta <= 32767 and loc <= 131071.
   max_packet[1] = (((unsigned int)(0x7fff-max_packet[0])) << 17) | (((unsigned int)my_rank) & 0x1ffff);

   // Do 32-bit MAX to get maximum overall rt_delta, and node with minimum rt_delta.
   BGLTreeMakeCollectiveHdr( &snd_hdr.hw, vt, 0, _BGL_TREE_OPCODE_MAX, _BGL_TREE_OPSIZE_BIT32, 0, _BGL_TREE_CSUM_NONE );
   BGLTreeSendUnaligned( vc, &snd_hdr.hw, (Bit64 *)max_packet, 8 );
   BGLTreeReceiveUnaligned( vc, &snd_hdr.hw, (Bit64 *)max_packet, 8 );

   printf("(I) Max delta=%d, min=%d at node %d\n", max_packet[0], 32767-((int)((max_packet[1]&0xfffe0000)>>17)), max_packet[1]&0x1ffff );

   // Node with minimum rt_delta (closest to root of GI tree) sets timestamps to maximum roundtrip latency
   // (plus a little fudge factor); others to 0.
   if( my_rank == (max_packet[1] & 0x1ffff) ) {
      //__BGL_Gi_BarrierWait = (Bit64) max_packet[0] + 8;
      __BGL_Gi_BarrierWait = 0;
      printf(" (I) [%d] Set __BGL_Gi_BarrierWait to %Lu.\n", my_rank, __BGL_Gi_BarrierWait );
   } else {
      __BGL_Gi_BarrierWait = 0;
   }

   return( 0 );
}

#undef  FN_NAME
#define FN_NAME "BGLGiShowStatus"
void BGLGiShowStatus(int rank)
{
   _BGL_GiStatus S;
   int my_rank = BGLPartitionGetRealRank();

   if( (rank >= 0) && (my_rank == rank) ) {
      S = BGLGiGetStatusPrivileged();

      printf("(I) [%3d] GI: mode %.1s%.1s%.1s%.1s; stickyInt %.1d%.1d%.1d%.1d; stickyActive %.1d%.1d%.1d%.1d; stickyType %.1s%.1s%.1s%.1s; readMode %.1d; recvState %.1d%.1d%.1d%.1d; userEnables %.1s%.1s%.1s%.1s; sendState %.1d%.1d%.1d%.1d\n", my_rank,
         (S.mode&0x8)==8?"S":"D", (S.mode&0x4)==4?"S":"D", (S.mode&0x2)==2?"S":"D", (S.mode&0x1)==1?"S":"D",
         (S.stickyInt&0x8)==8, (S.stickyInt&0x4)==4, (S.stickyInt&0x2)==2, (S.stickyInt&0x1)==1,
         (S.stickyActive&0x8)==8, (S.stickyActive&0x4)==4, (S.stickyActive&0x2)==2, (S.stickyActive&0x1)==1,
         (S.stickyType&0x8)==8?"A":"O", (S.stickyType&0x4)==4?"A":"O", (S.stickyType&0x2)==2?"A":"O", (S.stickyType&0x1)==1?"A":"O",
         (S.readMode)==1,
         (S.giRecvState&0x8)==8, (S.giRecvState&0x4)==4, (S.giRecvState&0x2)==2, (S.giRecvState&0x1)==1,
         (S.userEnables&0x8)==8?"E":".", (S.userEnables&0x4)==4?"E":".", (S.userEnables&0x2)==2?"E":".", (S.userEnables&0x1)==1?"E":".",
         (S.giSendState&0x8)==8, (S.giSendState&0x4)==4, (S.giSendState&0x2)==2, (S.giSendState&0x1)==1);
#if 0
      printf("(I) [%3d] GI: mode %.4x; stickyInt %.4x; stickyActive %.4x; stickyType %.4x; readMode %.1d; recvState %.4x; userEnables %.4x; sendState %.4x\n", my_rank,
         S.mode, S.stickyInt, S.stickyActive, S.stickyType, S.readMode, S.giRecvState, S.userEnables, S.giSendState);
#endif
   }
}

