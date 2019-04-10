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
 ///////////////////////////////////////////////////////////////
// Blue light timeing mgr
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
// BltwGetTimeout -- which will retrieve the timeout value that is the earliest
// timeing delta that we have to service.
//
//
//
//
//

// All time values are in milliseconds.
//
// TBD.. We can probably optimize this such that we don't have to scan
// the timer wheel if there is nothing in it...
//


#include "hostcfg.h"


#if HAVE_MEMORY_H
	#include <memory.h>
#endif

#if HAVE_TIME_H
	#include <time.h>
#endif

#include "blrudp_memory.h"

#include "bltimemgr.h"


#include "dbgPrint.h"
#include "dbgtimelog.h"

#include "bldefs.h"


#define BLTW_BININTERVAL    250     // 250 millisecond bin interval

#ifdef BLHPK
//
// A bit kludgey, but including the file that defines this causes other name and
// include colisions...
//
extern unsigned long long Get_TimeBase( void );
#endif


void BltwInsertTimer(_QLink *pList, 
                     BLTIME_TIMER *pTimer);
BLTIME_MS BltwTimerNow(void);

///////////////////////////////////////////////////////////////////////////
BLTIME_MGR *BltwNew()
//
// Create a new timeing wheel.
// 
// inputs:
//    none.
// outputs:
//    returns -- new timeing wheel created.
//
//////////////////////////////////////////////////////////////////////////////
{
    BLTIME_MGR *pTimeMgr = NULL;
    _QLink *pList;    
    unsigned int n;

    pTimeMgr = BlrAllocMem(sizeof(*pTimeMgr));
    if (pTimeMgr == NULL)     // oops, out of memory.
        FAIL;

    memset(pTimeMgr, 0, sizeof(*pTimeMgr));
    
    for (n = 0, pList = pTimeMgr->Wheel; 
         n < SIZEOF_ARRAY(pTimeMgr->Wheel); 
         n++, pList++)
    {
        QInit(pList);
    }

    pTimeMgr->llCurrTime = BltwTimerNow();      // grab the current time and save it off.
    pTimeMgr->ulTimeSlot = (unsigned long)
        ((pTimeMgr->llCurrTime / BLTW_BININTERVAL) % SIZEOF_ARRAY(pTimeMgr->Wheel));

	ASSERT(pTimeMgr->ulTimeSlot < BLTWHEEL_MAX_ENTRIES);
	

    return(pTimeMgr);
Fail:
    if (pTimeMgr) 
        BlrFreeMem(pTimeMgr);
        
    return(NULL);
}


////////////////////////////////////////////////////////////////////////////
void BltwDelete(BLTIME_MGR *pTimeMgr)
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
{
    _QLink *pList;    
    unsigned int n;

    if (pTimeMgr == NULL)
        FAIL;

    
    //
    // remove each element in the queue so they know they
    // have been removed.
    for (n = 0, pList = pTimeMgr->Wheel; 
         n < SIZEOF_ARRAY(pTimeMgr->Wheel); 
         n++, pList++)
    {
        while (QisFull(pList))
            DeQ(pList->next);
    }

    BlrFreeMem(pTimeMgr);      // dump the wheel.
    return;
Fail:
    return;

}

/////////////////////////////////////////////////////////////////////////
BLTIME_TIMER *BltwTimerNew(BLTIME_MGR *pTimeMgr, 
                           PFN_TIME_WHEEL_CALLBACK pfnCallback,
                           void *pUserData)
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
{

    BLTIME_TIMER *pTimer = NULL;

    pTimer = BlrAllocMem(sizeof(*pTimer));
    if (pTimer == NULL)     // oops, no memory.
        FAIL;               // punt.....


    // initialze the little bugger.    
    BltwTimerInit(pTimeMgr,      // *pTimeMgr,                     
                  pTimer,          // *pTimer,                         
                  pfnCallback,     // pfnCallback,          
                  pUserData);      // *pUserData)                              


    
    return(pTimer);
Fail:
    if (pTimer)
        BlrFreeMem(pTimer);
    return(NULL);
}

//////////////////////////////////////////////////////////////////////
void BltwTimerDelete(BLTIME_MGR *pTimeMgr UNUSED,
                     BLTIME_TIMER *pTimer)
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
{
    BlrFreeMem(pTimer);      // ditch the memory.
}

////////////////////////////////////////////////////////////////////////
void BltwTimerInit(BLTIME_MGR *pTimeMgr UNUSED,
                   BLTIME_TIMER *pTimer,
                   PFN_TIME_WHEEL_CALLBACK pfnCallback,
                   void *pUserData)
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
{
    
    memset(pTimer, 0, sizeof(*pTimer));

    pTimer->pfnCallback = pfnCallback;
    pTimer->pUserData = pUserData;
    pTimer->llTimeToCall = 0;


}


/////////////////////////////////////////////////////////////////////////
void BltwTimerAdd(BLTIME_MGR *pTimeMgr,
                  BLTIME_TIMER *pTimer,
                  BLTIME_MS llTimeout)
//
// Add the timer to the timeing wheel to timeout ftimeout seconds
// from now.
//
// A timer can be active in only one timing wheel at a time.
//
// inputs:
//    pTimeMgr -- timeing wheel to put this timer into.
//    pTimer -- pointer to timer to add.
//    llTimeout -- timeout value (delta from the current time).
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////////////////
{
    ASSERT(pTimeMgr && pTimer);

    if ((pTimeMgr == NULL) && (pTimer == NULL))
        FAIL;

    BltwTimerRemove(pTimeMgr, pTimer);      // make sure we don't double queue it...


    if (llTimeout == 0)
    {
        //
        // zero based timeouts go in the current timerslot.
        //
		_QLink *pCurrTimer = &pTimeMgr->Wheel[pTimeMgr->ulTimeSlot];
		
		ASSERT(pTimeMgr->ulTimeSlot < BLTWHEEL_MAX_ENTRIES);
        
		pTimer->llTimeToCall = BltwTimerNow();        // right now...

        BltwInsertTimer(pCurrTimer,  pTimer);
        //EnQ(&pTimer->link, pCurrTimer);             // queue it into the list.
    }
    else
    {   // figure out the timeing slot we need to be in.
        BLTIME_MS llTimeDelta;
        BLTIME_MS llTimeToCall;
        BLTIME_MS llNow;

        unsigned long ulTimeSlot;
        llNow = BltwTimerNow();
        llTimeToCall = llNow+llTimeout;        
        if (llTimeToCall < pTimeMgr->llCurrTime)        // time went backwards????
            llTimeToCall = pTimeMgr->llCurrTime;        // tough, handle it.

        llTimeDelta = llTimeToCall - pTimeMgr->llCurrTime;

        llTimeDelta = llTimeDelta / BLTW_BININTERVAL;   // number of bins.
        
        if (llTimeDelta >= SIZEOF_ARRAY(pTimeMgr->Wheel))   // max it out.
        {
            llTimeDelta = SIZEOF_ARRAY(pTimeMgr->Wheel)-1;
            llTimeToCall = pTimeMgr->llCurrTime + 
                           ((SIZEOF_ARRAY(pTimeMgr->Wheel)-1) * BLTW_BININTERVAL);
        }
        pTimer->llTimeToCall = llTimeToCall;

        ulTimeSlot = (unsigned long)((llTimeToCall / BLTW_BININTERVAL) % 
                                      SIZEOF_ARRAY(pTimeMgr->Wheel));
		ASSERT(pTimeMgr->ulTimeSlot < BLTWHEEL_MAX_ENTRIES);
		
        BltwInsertTimer( &pTimeMgr->Wheel[ulTimeSlot], pTimer);
    }

    return;
Fail:
    return;
}

////////////////////////////////////////////////////////////////////////
void BltwTimerRemove(BLTIME_MGR *pTimeMgr UNUSED,
                     BLTIME_TIMER *pTimer UNUSED)
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
{
    
    // if it is in  a linked list, pull it.
    if (pTimer->link.next)
        DeQ(&pTimer->link);

}

////////////////////////////////////////////////////////////////
void BltwInsertTimer(_QLink *pList, 
                     BLTIME_TIMER *pTimer)
//
// Insert the timer into the priority queue.
//
// inputs:
//    pList -- list to insert the timer into.
//    pTimer -- pointer to timer to insert
// outputs:
//    none.
//
//
{
    BLTIME_TIMER *pT;

    pT = (BLTIME_TIMER *)pList->next;       // pick up the first in the list.
    while (pT->link.next != pList)       // while the link does not do an un-natural act.
    {
        if (pTimer->llTimeToCall <= pT->llTimeToCall)
        {
            EnQ(&pTimer->link,&pT->link);
            break;
        }
        pT = (BLTIME_TIMER *)pT->link.next;       // pick up the first in the list.

    }
    if (pT->link.next == pList)          // insert at the end of the list.
        EnQ(&pTimer->link, pList);


}
                     
/////////////////////////////////////////////////////////////////////////
void BltwTimerRun(BLTIME_MGR *pTimeMgr)
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
{
    unsigned long ulTimeSlot;
    unsigned long ulIdx;
    BLTIME_MS llCurrTime;
    _QLink listExpiredTimers;

    llCurrTime = BltwTimerNow();
    ulTimeSlot = (unsigned long)((llCurrTime / BLTW_BININTERVAL) % 
                                 SIZEOF_ARRAY(pTimeMgr->Wheel));
    ulIdx = pTimeMgr->ulTimeSlot;       // starting slot.

	ASSERT(pTimeMgr->ulTimeSlot < BLTWHEEL_MAX_ENTRIES);

    pTimeMgr->llCurrTime = llCurrTime;
    pTimeMgr->ulTimeSlot = ulTimeSlot;      // update the target slot now.
    QInit(&listExpiredTimers);

    //
    // Loop throught all the expired timers.
    // break terminates the loop.
    while (TRUE)
    {
        _QLink *pEntry = &pTimeMgr->Wheel[ulIdx];
        

        //
        // Transfer these to the expired timers list
        // so we can't lock up into a infinite loop where
        // zero timers keep being added to the list.
        //
        if (ulIdx != ulTimeSlot)        // do all the slots.
        {
            while (QisFull(pEntry))
            {
                BLTIME_TIMER *pTimer = (BLTIME_TIMER *)pEntry->next;
                DeQ(&pTimer->link);
                EnQ(&pTimer->link, &listExpiredTimers);
                ASSERT(llCurrTime >= pTimer->llTimeToCall);
            }
        }
        else
        {               // only the ones in the top of the priority list.
            BLTIME_TIMER *pT;
            pT = (BLTIME_TIMER *)QBegin(pEntry);       // pick up the first in the list.
            while (pT != (BLTIME_TIMER *)QEnd(pEntry)) // while the link does not do an un-natural act.
            {
                BLTIME_TIMER *pNext;

                if (pT->llTimeToCall > llCurrTime)        // end of the line.
                    break;
                pNext = (BLTIME_TIMER *)pT->link.next;      // pick up the next before we mess
                DeQ(&pT->link);                             // with the queue.
                EnQ(&pT->link, &listExpiredTimers);
                pT = pNext;       // very nice, next...
            }
        }

        if (ulIdx == ulTimeSlot)
            break;                      //end of the line....
        ulIdx = ++ulIdx % SIZEOF_ARRAY(pTimeMgr->Wheel);
    };
    while (QisFull(&listExpiredTimers))
    {
        BLTIME_TIMER *pTimer = (BLTIME_TIMER *)listExpiredTimers.next;
        DeQ(&pTimer->link);
        if (pTimer->pfnCallback) 
        {
            (*pTimer->pfnCallback)(pTimeMgr, 
                                   pTimer,
                                   pTimer->pUserData);
        }
    }

	ASSERT(pTimeMgr->ulTimeSlot < BLTWHEEL_MAX_ENTRIES);

}



/////////////////////////////////////////////////////////////////////////
int BltwGetTimeout(BLTIME_MGR *pTimeMgr,
                    struct timeval *pTimeValue,
                    unsigned int nMaxCheck)
//
//
// Retrieve a timeout value that can be used in poll and select 
//
// inputs:
//    pTimeMgr -- timeing wheel to get timeout for.
//    pTimeValue -- pointer to time value to fill out.
//    nMax       -- maximum number of timerwheel slots to check.
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
{
    BLTIME_TIMER *pTimer = NULL;
    unsigned long ulTimeSlot;
    unsigned long ulIdx;
    BLTIME_MS llTimeout = 0;
    BLTIME_MS llCurrTime;
    int bRet = TRUE;
    _QLink *pCurrTimer;


    ASSERT(pTimeMgr && pTimeValue);
    if ((pTimeMgr == NULL) && (pTimeValue == NULL))
        FAIL;

    //
    // check the entire wheel???
    if ((nMaxCheck == 0) || (nMaxCheck > SIZEOF_ARRAY(pTimeMgr->Wheel)))
        nMaxCheck = SIZEOF_ARRAY(pTimeMgr->Wheel);


    //
    // The efficiences should not be too bad here.  If there is nothing
    // to do then we will waste a little time scanning the entire wheel.
    // If there is actually something to do then we will stop earlier.
    // If the caller is concerned, then the search can be limited
    // to a narrower time frame.
    //
    // At least until we come up with a better way....
    //

	ASSERT(pTimeMgr->ulTimeSlot < BLTWHEEL_MAX_ENTRIES);

    llCurrTime = BltwTimerNow();

    ulTimeSlot = pTimeMgr->ulTimeSlot;
    pCurrTimer = &pTimeMgr->Wheel[ulTimeSlot];
    for (ulIdx = 0; ulIdx < nMaxCheck; ulIdx++)
    {
        if (QisFull(pCurrTimer))                   // found one about to expire.
        {
            pTimer = (BLTIME_TIMER *)QBegin(pCurrTimer);
            break;
        }
        ulTimeSlot++; 
        pCurrTimer++;

        // time to wrap???
        if (ulTimeSlot >= SIZEOF_ARRAY(pTimeMgr->Wheel))
        {
            ulTimeSlot = 0;
            pCurrTimer = pTimeMgr->Wheel;
        }
    }
    if (pTimer)         // found one???
    {
		if (pTimer->llTimeToCall > llCurrTime) 
			llTimeout = pTimer->llTimeToCall - llCurrTime;
		else
			llTimeout = 0;
        bRet = TRUE;
    }
    else
    {
        BLTIME_MS llRemainder;
        llRemainder = (llTimeout % BLTW_BININTERVAL);
        // did we check the entire wheel???
        if (nMaxCheck == SIZEOF_ARRAY(pTimeMgr->Wheel)) // yes, 
        {
            bRet = FALSE;           // no timeout active....
            llTimeout = ((BLTIME_MS)nMaxCheck*BLTW_BININTERVAL);
        }
        else        
        {           
            bRet = TRUE;
            // did not check the entire wheel, max timeout is the next bin.
            llTimeout = ((BLTIME_MS)nMaxCheck*BLTW_BININTERVAL);

        }
        //
        // Less the interval we are currently into the current slot.
        // There has to be a way of doing this without so many multiply and divides.
        //
        if (llTimeout > llRemainder)
            llTimeout -= llRemainder;
    }
	pTimeValue->tv_sec = (unsigned long)(llTimeout / 1000);                    
	pTimeValue->tv_usec = (unsigned long)(llTimeout % 1000);  

    return(bRet);
Fail:
    if (pTimeValue)
        memset(pTimeValue, 0, sizeof(*pTimeValue));
    return(FALSE);

}

/////////////////////////////////////////////////////////////////
BLTIME_MS BltwTimerRawTime(void)
//
// Get the raw timer information from the system.  System time 
// in in the finest grain that the system returns.
//
// inputs:
//    none.
// outputs:
//    returns -- the current raw time.
//
{
    BLTIME_MS llRawTime = 0;

#if defined(WIN32) || defined(CYGWIN)
    LARGE_INTEGER nPerfCounter;

    //
    // At 1Ghz clock this performance counter is good for 586 years.
    // I won't be around when this times out.  Plus, windows is not
    // really a target for this.
    //
    BOOL bRet;
    bRet = QueryPerformanceCounter(&nPerfCounter);

    if (bRet)
        llRawTime = nPerfCounter.QuadPart;

#elif defined(BLHPK)

	llRawTime = Get_TimeBase();  // get current time in ticks


#else
    struct timeval tp;
	//
	// ADD blhpk code here...
	//
    gettimeofday(&tp,NULL);
    llRawTime = ((BLTIME_MS)tp.tv_sec*1000+(BLTIME_MS)tp.tv_usec);

#endif

    return(llRawTime);
}
////////////////////////////////////////////////////////////////////
BLTIME_MS BltwTimerNow(void)
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
{
    BLTIME_MS llNow = 0;

#if defined(WIN32) || defined(CYGWIN)

    LARGE_INTEGER nPerfCounter;
    LARGE_INTEGER nFreqency;

    //
    // At 1Ghz clock this performance counter is good for 586 years.
    // I won't be around when this times out.  Plus, windows is not
    // really a target for this.
    //
    BOOL bRet;
    bRet = QueryPerformanceCounter(&nPerfCounter);
    if (bRet)
        bRet = QueryPerformanceFrequency(&nFreqency);

    

    if (bRet)
        llNow = (nPerfCounter.QuadPart/(nFreqency.QuadPart/1000));

#elif defined(BLHPK)
	uint64_t tb;

	tb = Get_TimeBase();  // get current time in ticks

	// convert it to milliseconds
	llNow = (tb/(TB_TICKS_PER_SEC/1000));

#else
    struct timeval tp;
	//
	// ADD blhpk code here...
	//
    gettimeofday(&tp,NULL);
    llNow = ((BLTIME_MS)tp.tv_sec*1000+(BLTIME_MS)tp.tv_usec/1000);

#endif


// we don't have breakpoints in the kernel yet, so this
// is not much use.. Make sure it does not screw things up...
#if 0
#ifndef BLHPK
    #ifdef _DEBUG 
    {
        //
        // For debugging purposes each time we breakpoint, we will end up
        // having ALL the timers expire at once.  Since this is probably NOT 
        // desirable, suppress any time deltas that are greater than 5 seconds.
        //
        static BLTIME_MS llLastTime = 0;            // last time we got.
        static BLTIME_MS llLastNow = 0;             // last now we returned.
        static BLTIME_MS llDelta;

        llDelta = llNow - llLastTime;
        llLastTime = llNow;     // remember this...
        if ((llDelta) < 3000)
            llNow = llLastNow + llDelta;   // add in the delta
        else
            llNow = llLastNow;              // just return the last time.

        llLastNow = llNow;
    }
    #endif
#endif	
#endif

   return(llNow);

}


////////////////////////////////////////////////////////////////////////////////
int BltwIsTimerActive(BLTIME_TIMER *pTimer)
//
// Check to see if the timer is active...
//
// inputs:
//    pTimer -- timer to check.
// outputs:
//   returns -- TRUE if the timer active.
//              FALSE if not.
//
{
    if (pTimer->link.next)
        return(TRUE);
    else
        return(FALSE);

}

