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
 //////////////////////////////////////////////////////////////////////////
//
// This module manages the blue light reliable work queue.
//
// this is a round robin queue which schedules work on behalf of each
// of the active contexts.  
//
//
// Typically Work for a given queue comes in two flavors.
// 1.  Transmit work.  A given context has something to send to the
//     other end of a connection.
// 2.  Receive callbacks.  Data have been received for a given
//     context and the data receive callbacks need to be called.
//
// However, the Work queue manager has no knowledge of this.  It just manages
// the work queues.
//
// A given context schedules work by calling ...AddWorkItem and 
// removes itself from the work queue by calling ...RemoveWorkItem.
// Adding work puts the work at end of the work queue.
//
// Work can be rescheduled to the end of the work queue by calling 
// ...RescheduleWork.
//
//
//


#include "hostcfg.h"

#include "blrudp_memory.h"

#include "blrudpwork.h"

#include "dbgPrint.h"

#include "bldefs.h"


///////////////////////////////////////////////////////////////////////////
BLWORK_MGR *BlrWorkNew()

//
// Create a new work queue manager.
// 
// inputs:
//    none.
// outputs:
//    returns -- new timeing wheel created.
//
//////////////////////////////////////////////////////////////////////////////
{
    BLWORK_MGR *pBlWork = NULL;


    pBlWork  = BlrAllocMem(sizeof(*pBlWork ));
    if (pBlWork  == NULL)     // oops, out of memory.
        FAIL;


    QInit(&pBlWork->WorkQueue);     // initialize the queue.

    pBlWork->ulNumWorkItems = 0;
    return(pBlWork);
Fail:
    if (pBlWork)
        BlrWorkDelete(pBlWork);
    return(NULL);
}


////////////////////////////////////////////////////////////////////////////
void BlrWorkDelete(BLWORK_MGR *pBlWork)
//
// Delete a Work queue manager Created with BlrWorkNew...
//
// this also dequeues all work items in the work queue.
//
// inputs:
//    pBlWork -- work manager to delete.
// outputs:
//    none.
////////////////////////////////////////////////////////////////////////////
{
    ASSERT(pBlWork);
    if (pBlWork == NULL)
        return;         // punt

    while (QisFull(&pBlWork->WorkQueue))    // dump the queue.
        DeQ(QBegin(&pBlWork->WorkQueue));

    BlrFreeMem(pBlWork);            // by by...
}


////////////////////////////////////////////////////////////////////////////
int BlrWorkRun(BLWORK_MGR *pBlWork)
//
// Run the work manager.  This will fire off the top most callback
// in the work queue removing it from the work queue.
//
// inputs:
//    pBlwork -- pointer to the blue light work queue to fire off work
//               for.
// outputs:
//    return -- TRUE if there is more work to do.
//              FALSE if the queue is now empty...
//
////////////////////////////////////////////////////////////////////////
{
    ASSERT(pBlWork);
    if (pBlWork == NULL)
        FAIL;

    if (QisFull(&pBlWork->WorkQueue))    
    {
        BLWORK_ITEM *pWorkItem = (BLWORK_ITEM *)QBegin(&pBlWork->WorkQueue);
        ASSERT(pBlWork->ulNumWorkItems);
        if (pBlWork->ulNumWorkItems)
            pBlWork->ulNumWorkItems--;

        DeQ(&pWorkItem->link);                

        if (pWorkItem->pfnCallback)
        {
            (*pWorkItem->pfnCallback)(pBlWork,
                                      pWorkItem,
                                      pWorkItem->pUserData);
        }
    }
        
    if (QisFull(&pBlWork->WorkQueue))
        return(TRUE);                   // more work to do....     
    else
        return(FALSE);
Fail:
    return(FALSE);
}


////////////////////////////////////////////////////////////////////////
void BlrWorkItemInit(BLWORK_ITEM *pWorkItem,
                    PFN_WORK_CALLBACK pfnCallback,
                    void *pUserData)
//
// Initialize a given work item.  
//
// inputs:
//    pBlWork -- work manager that this work item is for.
//    pWorkItem -- work item to initialize.
//    pfnCallback -- callback associated with this work item.
//    pUserData -- user data to associate with this work item.
// outputs:
//    none.
//
/////////////////////////////////////////////////////////////////////////
{
    ASSERT(pWorkItem);
    if (pWorkItem == NULL)
        FAIL;
    ASSERT(pfnCallback);

    pWorkItem->link.next = pWorkItem->link.prev = NULL;     // initialize unlinked.
    pWorkItem->pfnCallback = pfnCallback;      
    pWorkItem->pUserData = pUserData;          


    return;
Fail:
    return;
}

///////////////////////////////////////////////////////////////////////
void BlrWorkAddItem(BLWORK_MGR *pBlWork,
                   BLWORK_ITEM *pWorkItem)
//
// Add a work item to the end of the work queue.
//
// inputs:
//    pBlWork -- work manager that this work item is for.
//    pWorkItem -- work item to remove.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////////
{
    ASSERT(pBlWork && pWorkItem);
    if ((pBlWork == NULL) || (pWorkItem == NULL))
        FAIL;

#if 1
    //
    // alternate stragegy..  NOTE: we must NEVER queue
    // the work item into a different queue for this to work correctly..
    //

    if (pWorkItem->link.next == NULL)       // queue only if it is not already in the queue...
    {
        EnQ(&pWorkItem->link, &pBlWork->WorkQueue);     // queue it up.
        pBlWork->ulNumWorkItems++;
    }

#else
    // if it is in a queue, remove it.
    if (pWorkItem->link.next)
    {
        DeQ(&pWorkItem->link);
        ASSERT(pBlWork->ulNumWorkItems);
        if (pBlWork->ulNumWorkItems)
            pBlWork->ulNumWorkItems--;
    }

    EnQ(&pWorkItem->link, &pBlWork->WorkQueue);     // queue it up.
    pBlWork->ulNumWorkItems++;
#endif

    return;
Fail:
    return;
}

///////////////////////////////////////////////////////////////////////
void BlrWorkRemoveItem(BLWORK_MGR *pBlWork,
                      BLWORK_ITEM *pWorkItem)
//
// Remove a given work item from the queue of work items.
//
// inputs:
//    pBlWork -- work manager that this work item is for.
//    pWorkItem -- work item to remove.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////////
{
    // if it is in a queue, remove it.
    if (pWorkItem->link.next)
    {
        ASSERT(pBlWork->ulNumWorkItems);
        if (pBlWork->ulNumWorkItems)
            pBlWork->ulNumWorkItems--;
        DeQ(&pWorkItem->link);
    }

}

//////////////////////////////////////////////////////////////////////////
unsigned long BlrWorkNumItems(BLWORK_MGR *pBlWork)
//
// Total number of items in the work queue.
//
// inputs:
//    pBlWork -- work manager to count the items for.
// outputs:
//    returns -- total number of work items in this queeu.
//
{
    ASSERT( (pBlWork->ulNumWorkItems && QisFull(&pBlWork->WorkQueue))   || 
           (!pBlWork->ulNumWorkItems && QisEmpty(&pBlWork->WorkQueue)));

    return(pBlWork->ulNumWorkItems);

}

