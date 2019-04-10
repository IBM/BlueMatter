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
 #ifndef _BLRUDPWORK_H_
#define _BLRUDPWORK_H_


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
// A given context schedules work by calling ...AddWork and 
// removes itself from the work queue by calling ...RemoveWork.
// Adding work puts the work at end of the work queue.
//
// Work can be rescheduled to the end of the work queue by calling 
// ...RescheduleWork.
//
//
//

#include <stddef.h>


#include "qlink.h"


#ifdef __cplusplus
extern "C" {
#endif


// forward delcarations...
struct T_BLWORK_MGR;
struct T_BLWORK_ITEM;

/////////////////////////////////////////////////////////////////////////////
typedef void (* PFN_WORK_CALLBACK)(struct T_BLWORK_MGR *pBlWork,
                                 struct T_BLWORK_ITEM *pWorkItem,
                                 void *pUserData);
//
// Work callback.
//
// inputs:
//    pBlWork -- pointer to the work manager this belongs to.
//    pWorkItem -- the work item this callback corresponds to.
//    pUserDAta -- user data passed in when the work item was initalized
// outputs:
//    none.
//
// WORK is removed from the work queue when this is called.  It can be
// rescheduled by callling ..AddWork...
//
//
/////////////////////////////////////////////////////////////////////////////

typedef struct T_BLWORK_ITEM {
    _QLink link;
    PFN_WORK_CALLBACK pfnCallback;      // pointer to work callback.
    void *pUserData;                    // 
} BLWORK_ITEM;

typedef struct T_BLWORK_MGR {
    _QLink WorkQueue;
    unsigned long ulNumWorkItems;
} BLWORK_MGR;

///////////////////////////////////////////////////////////////////////////
BLWORK_MGR *BlrWorkNew();

//
// Create a new work queue manager.
// 
// inputs:
//    none.
// outputs:
//    returns -- new timeing wheel created.
//
//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
void BlrWorkDelete(BLWORK_MGR *pBlWork);
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


////////////////////////////////////////////////////////////////////////////
int BlrWorkRun(BLWORK_MGR *pBlWork);
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


////////////////////////////////////////////////////////////////////////
void BlrWorkItemInit(BLWORK_ITEM *pWorkItem,
                    PFN_WORK_CALLBACK pfnCallback,
                    void *pUserData);
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

///////////////////////////////////////////////////////////////////////
void BlrWorkAddItem(BLWORK_MGR *pBlWork,
                    BLWORK_ITEM *pWorkItem);
//
// Add a work item to the end of the work queue.
//
// inputs:
//    pBlWork -- work manager that this work item is for.
//    pWorkItem -- work item to remove.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void BlrWorkRemoveItem(BLWORK_MGR *pBlWork,
                      BLWORK_ITEM *pWorkItem);
//
// Remove a given work item from the queue of work items.
//
// inputs:
//    pBlWork -- work manager that this work item is for.
//    pWorkItem -- work item to remove.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
unsigned long BlrWorkNumItems(BLWORK_MGR *pBlWork);
//
// Total number of items in the work queue.
//
// inputs:
//    pBlWork -- work manager to count the items for.
// outputs:
//    returns -- total number of work items in this queeu.
//
//////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
};
#endif


#endif
