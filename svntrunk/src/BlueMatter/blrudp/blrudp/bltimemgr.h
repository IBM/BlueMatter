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
 #ifndef _BLTIME_MGR_H_
#define _BLTIME_MGR_H_

///////////////////////////////////////////////////////////////
// Blue light timeing wheel
//
//
// This file contains the timeing mgr functions for
// bluelight.
//
// The maximum time that this timeing mgr can timeout is 2 minutes.
// 
// To create a timing mgr call BltwNew and destroy it
// when done by calling BltwDelete.
//
// The timemgr does not take responsiblity for deleting any of the timers
// it controls.  That is the responability of the function that queues them up.
//
//
// In order for the timer functions to work BltwRun needs to be periodically called.
//
// If you wish to do a blocking io with a timeout then you can call
// 
// BltwGetTimeout -- which will retrieve the timeout value that is the earliest
// timeing delta that we have to service.
//
//
// This should provide adequate resolution for communication protocols.
//

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <time.h>

#if defined(WIN32) || defined(CYGWIN)
    #include <wtypes.h>
    #include <winsock.h>        // for timeval definition
#endif

/* Standard types.  */
#if HAVE_STDINT_H
    #include <stdint.h>
#elif USE_GDB_STDINT_H
    #include "gdb_stdint.h"
#endif

#if  HAVE_HPK_TYPES_H
#include <linux/types.h>
#endif


#if HAVE_SYS_TIME_H
    #include <sys/time.h>
#endif

#if HAVE_TIME_H
	#include <time.h>
#endif

#include "qlink.h"

typedef uint64_t BLTIME_MS;         // bluelight time in milliseconds.
//
// NOTE: we may want to make this an opaque structure.
//
enum {
    BLTWHEEL_MAX_ENTRIES = 256          // powers of 2, save some cycles on wrap.
};

typedef struct {
    BLTIME_MS llCurrTime;                       // current time of the timing wheel.
    unsigned long ulTimeSlot;              // current time slot for the current time.
    _QLink Wheel[BLTWHEEL_MAX_ENTRIES];
} BLTIME_MGR;


struct T_BLTIME_TIMER;


/////////////////////////////////////////////////////////////////////////////
typedef void (* PFN_TIME_WHEEL_CALLBACK)(BLTIME_MGR *pTimeMgr,
                                         struct T_BLTIME_TIMER *pTimer,
                                         void *pUserData);
//
// Timing wheel callback.
//
// inputs:
//    pTimeMgr -- pointer to timewheel that called this callback function.
//    pTimer -- pointer to timer that caused this callback.
//    pUserDAta -- user data passed in when the timer was initailized.
// outputs:
//    none.
//
//  NOTE: The pTimer is removed from the wheel when this function is called.
//
/////////////////////////////////////////////////////////////////////////////


typedef struct T_BLTIME_TIMER {
    _QLink link;                            // required link header to use this on a linked list.
                                            // MUST BE FIRST in STRUCT
    BLTIME_MS llTimeToCall;                 // time to call the timer.
    PFN_TIME_WHEEL_CALLBACK pfnCallback;    // pointer to function to call for timer
    void *pUserData;                        // pUserData -- pointer to user data to use.
} BLTIME_TIMER;




///////////////////////////////////////////////////////////////////////////
BLTIME_MGR *BltwNew();

//
// Create a new timeing wheel.
// 
// inputs:
//    none.
// outputs:
//    returns -- new timeing wheel created.
//
//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
void BltwDelete(BLTIME_MGR *pTimeMgr);
//
// Delete a timing wheel create with BltwNew...
//
// this will remove all the timer elements from the queue and zero out
// the links in the timer elemeents to the calling functions can see
// that they are no longer in the timing wheel.
//
// inputs:
//    pTimeMgr -- timewheel to delete.
// outputs:
//    none.
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
BLTIME_TIMER *BltwTimerNew(BLTIME_MGR *pTimeMgr, 
                          PFN_TIME_WHEEL_CALLBACK pfnCallback,
                          void *pUserData);
//
// Create a new timer and initizlie the callback and user data fields.
//
// inputs:
//    pTimeMgr -- timer wheel used to allocate this memory.
//    pfnCallback -- callback function to call when this timer expires.
//    pUserData -- user data to pass back to the function.
//
// outputs:
//    returns -- pointer to a newly allocated timer....
//
////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void BltwTimerDelete(BLTIME_MGR *pTimeMgr,
                     BLTIME_TIMER *pTimer);
//
// Delete a timer allocated by BltwTimerNew.
//
// inputs:
//    pTimeMgr -- timer wheel used to delete.
//    pTimer -- pointer to timer to delete.
// outputs:
//    none.
// 
/////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BltwTimerInit(BLTIME_MGR *pTimeMgr,
                   BLTIME_TIMER *pTimer,
                   PFN_TIME_WHEEL_CALLBACK pfnCallback,
                   void *pUserData);
//
// Initialize a timer structure.  Use this function when you
// wish to make a timer part of a larger structure but don't want
// to use dynamic allocation.
//
// If you use this kind of initialization make SURE you call
// BltwTimerRemove before disposing of the containing structure.
//
// inputs:
//    pTimeMgr -- timer wheel used to initalize this.
//    pTimer -- pointer to timer to inialize.
//    pfnCallback -- callback function to call when this timer expires.
//    pUserData -- user data to pass back to the function.
// outputs:
//
// outputs:
//    none.
// 
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
void BltwTimerAdd(BLTIME_MGR *pTimeMgr,
                  BLTIME_TIMER *pTimer,
                  BLTIME_MS fTimeout);
//
// Add the timer to the timeing wheel to timeout ftimeout seconds
// from now.
//
// A timer can be active in only one timing wheel at a time.
//
// inputs:
//    pTimeMgr -- timeing wheel to put this timer into.
//    pTimer -- pointer to timer to add.
//    fTimeout -- timeout value (delta from the current time).
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BltwTimerRemove(BLTIME_MGR *pTimeMgr,
                     BLTIME_TIMER *pTimer);
//
// Remove a timer from a timeing wheel.
//
// inputs:
//    pTimeMgr -- timeing wheel to remove this from
//    pTimer -- pointer to timer to remove.
// outputs:
//    none.
//
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void BltwTimerRun(BLTIME_MGR *pTimeMgr);
//
// Check the current time and callback any timer callbacks
// that have expired.
//
// inputs:
//    pTimeMgr -- timeing wheel to get timeout for.
//
// outputs:
//    none.
//
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
int BltwGetTimeout(BLTIME_MGR *pTimeMgr,
                   struct timeval *pTimeValue,
                   unsigned int nMaxCheck);
//
//
// Retrieve a timeout value that can be used in poll and select 
//
// inputs:
//    pTimeMgr -- timeing wheel to get timeout for.
//    pTimeValue -- pointer to time value to fill out.
//    nMaxCheck  -- maximum number of timerwheel slots to check.
//                  0 indicates check the entire wheel.
//           
// ouputs:
//    returns -- TRUE if we have a timeout value at all,
//               FALSE we checked the entire list and it is empty.
//    *pTimeValue -- time value sturcture filled out.
//
// NOTE:
//    Using small values for nMaxCheck limits
//    the time spent attempting to find a minimum
//    timeout value at the expense of waking the process up
//    when there is perhaps nothing actually to service.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
int BltwIsTimerActive(BLTIME_TIMER *pTimer);
//
// Check to see if the timer is active...
//
// inputs:
//    pTimer -- timer to check.
// outputs:
//   returns -- TRUE if the timer active.
//              FALSE if not.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BLTIME_MS BltwTimerRawTime(void);
//
// Get the raw timer information from the system.  System time 
// in in the finest grain that the system returns.
//
// inputs:
//    none.
// outputs:
//    returns -- the current raw time.
//
///////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
BLTIME_MS BltwTimerNow(void);
//
// 
// retrieve the current time in milliseconds
//
// inputs:
//     none.
// outputs:
//     returns the current time in milliseconds
//             since time began.
//
//
////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif


#endif

