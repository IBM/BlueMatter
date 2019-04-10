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
 ////////////////////////////////////////////////////////////////////////////////////////
//
//
// This File contains the definition and document of the Blue Light Reliable UDP
// library (BlRudp).
//
// The BlRudp library implements the BlRudp protocol.  This protocol is designed
// to provide high bandwidth utilization of either a 100Mb or 1000Mb link between
// two points.  It is designed to run on top of UDP.
//
// For a complete description of this protocol please see:
//           "Blue Light Reliable UDP Network Protocol_e.doc"
//
//
//

//


#include "hostcfg.h"

#include "blrudp_memory.h"
#include "bltimemgr.h"

#include "stddef.h"

#ifndef BLHPK
    #include <stdio.h>
    #include <memory.h>

#if defined(WIN32) || defined(CYGWIN)
    #include <winsock.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif
#endif

//
#if HAVE_STDINT_H
    #include <stdint.h>
#elif USE_GDB_STDINT_H
    #include "gdb_stdint.h"
#endif

#if HAVE_STRINGS_H
    #include <strings.h>
#endif

#if HAVE_UNISTD_H
    #include <unistd.h>
#endif

#if HAVE_ARPA_INET_H
    #include <arpa/inet.h>
#endif

#if HAVE_SYS_IOCTL_H
    #include <sys/ioctl.h>
#endif

#if HAVE_NET_IF_H
    #include <net/if.h>
#endif

#include <time.h>
#if 0
#include <errno.h>
#endif


#include "blrudp.h"
#include "blmap.h"
#include "BlErrors.h"
#include "blrudppkt.h"
#include "blrudp_st.h"
#include "blrudp_state.h"
#include "blrudppkt_help.h"
#include "blrudp_pktpool.h"
#include "blrudp_common.h"
#include "blrudp_memory.h"
#include "blrudp_io.h"

#include "blrudpwork.h"


#include "qlink.h"
#include "dbgtimelog.h"
#include "dbgPrint.h"



#include "bldefs.h"




//
// State/Event virutal table.  One entrypoint per.. event.
//



#define DEFAULT_PORT 3000

//
// Forward (private declarations).
//
int BlrudpInitializeSockets();
void BlrudpCleanupSockets();

int BlrudpCompareSaps(void *pKey1, void *pKey2);
int BlrudpCompareConnectSaps(void *pKey1, void *pKey2);
int BlrudpLocalSap(BLRUDP_MGR *pBlrudp,
                   BLRUDP_LOCAL_SAP_ITEM **ppSap,
                   unsigned long ulIpAddr,
                   unsigned short nPort);

void BlrudpRunRecv(BLRUDP_MGR *pBlrudp);
int BlrudpRunRecvFromSap(BLRUDP_MGR *pBlrudp,
                         BLRUDP_LOCAL_SAP_ITEM *pSap);
void BlrudpRunXmitWork(BLRUDP_MGR *pBlrudp);
void BlrudpRunXmitDoneWork(BLRUDP_MGR *pBlrudp);

void BlrudpRunRecvWork(BLRUDP_MGR *pBlrudp);


int BlrudpProcessOptions(BLRUDP_CONTEXT_OPTIONS *pOptions,
                         int nNumOptions,
                         BLRUDP_OPTIONS pOptionList[]);
int BlrudpRetrieveOptions(BLRUDP_CONTEXT_OPTIONS *pOptions,
                         int nNumOptions,
                         BLRUDP_OPTIONS pOptionList[]);


void BlrudpActCtxClear(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpActCtxInitTimers(BLRUDP_MGR *pBlrudp,
                                  BLRUDP_ACTIVE_CONTEXT *pContext);


//
// Callback protottypes....
//

void BlrudpXmitWorkCallback(struct T_BLWORK_MGR *pBlWork,
                            struct T_BLWORK_ITEM *pWorkItem,
                            void *pUserData);
void BlrudpRecvWorkCallback(struct T_BLWORK_MGR *pBlWork,
                            struct T_BLWORK_ITEM *pWorkItem,
                            void *pUserData);
void BlrudpXmitDoneWorkCallback(struct T_BLWORK_MGR *pBlWork UNUSED,
                                struct T_BLWORK_ITEM *pWorkItem UNUSED,
                                void *pUserData);

void BlrudpWTimerCallback(BLTIME_MGR *pTimeMgr,
                          struct T_BLTIME_TIMER *pTimer,
                          void *pUserData);
void BlrudpCTimerCallback(BLTIME_MGR *pTimeMgr,
                          struct T_BLTIME_TIMER *pTimer,
                          void *pUserData);
void BlrudpCTimeoutCallback(BLTIME_MGR *pTimeMgr,
                          struct T_BLTIME_TIMER *pTimer,
                          void *pUserData);
void BlrudpRTimerCallback(BLTIME_MGR *pTimeMgr,
                          struct T_BLTIME_TIMER *pTimer,
                          void *pUserData);
void BlrudpSleep(BLRUDP_MGR *pBlrudp UNUSED);


//
// Reference counter for socket initialization.
//
static unsigned long g_nSocketsInitialized = 0;
static _QLink g_listBlrudpMgrs = {NULL, NULL};      // list of blrudp managers.


static uint32_t g_ulContextId = 0;
////////////////////////////////////////////////////////////////////////
int BlrudpNew(BLRUDP_HANDLE *phBlrudp,
              int nNumOptions,
              BLRUDP_OPTIONS pOptions[])
//
// Create a new Blue Light RUDP manager.
//
// inputs:
//     *phBlrudp -- pointer to where to stuff the handle for the instance to this
//                  library.
//     nNumOptions -- number of paramters that we wish to override the default values for.
//     pOptions -- pointer to list of parameters.
// outputs
//   returns BLERR_SUCCESS if successful,
//            error codes TBD..
//    pHandle Pointer to where to stuff the Handle to the newly created Blrudp library.
////////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_MGR *pBlrudp = NULL;
    int nRet;


    pBlrudp = (BLRUDP_MGR *)BlrAllocMem(sizeof(*pBlrudp));

    if (pBlrudp == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);         // oops, out of memory.

    BlrudpInitializeSockets();

    memset(pBlrudp, 0, sizeof(*pBlrudp));            // initialize the memory

    nRet = gethostname(pBlrudp->szHostName, sizeof(pBlrudp->szHostName));
    ASSERT(nRet == 0);


    // defaults section.
    pBlrudp->DefOptions.ulWindowSize    = 0x10000L;                           // default window size.
    //pBlrudp->DefOptions.ulWindowSize  = 0x10000*10L;                           // default window size.
    pBlrudp->DefOptions.ulMaxRetryCount = 5*1000/BLRUDP_DEFAULT_WTIMER_TIMEOUT; // default retry count.
    pBlrudp->DefOptions.ulXmitRate      = 0;                                  // default transmit rate (timedel in ms).
    pBlrudp->DefOptions.ulXmitBurstSize = 0;                                  // default transmit burst size in packets..(unlimited)
    pBlrudp->DefOptions.ulSrcIpAddr     = 0;
    pBlrudp->DefOptions.nsSrcPort       = DEFAULT_PORT;                       // default source port
    pBlrudp->DefOptions.ulRndTripDelay  = BLRUDP_DEFAULT_WTIMER_TIMEOUT;      // starting round trip delay time.
    pBlrudp->DefOptions.ulMTL           = BLRUDP_DEFAULT_ZOMBIE_TIMEOUT;      // default max time to live in MS.
    pBlrudp->DefOptions.ulMtuSize       = 0;                                  // use device default MTU....
    pBlrudp->DefOptions.bInOrderDelivery = TRUE;                              // inorder deliver.
    pBlrudp->DefOptions.bDedicatedBuffers = TRUE;                             // dedicated buffers per connection.
    pBlrudp->DefOptions.bUseRawSockets   = FALSE;                             // use raw sockets if available.
    //pBlrudp->DefOptions.bChecksum       = TRUE;                               // checksum all packets.
    pBlrudp->DefOptions.bChecksum       = FALSE;                               // checksum all packets.
    pBlrudp->DefOptions.bAppFlowCtl     = FALSE;                              // Application level flow control.
    pBlrudp->DefOptions.bFastNak        = FALSE;                              // Fast nak mode.


    QInit(&pBlrudp->listPassCtxs);            // initialize the link lists.
    QInit(&pBlrudp->listActCtxs);

    //
    // initialize sap related information...
    QInit(&pBlrudp->listLocalSaps);
    pBlrudp->ulNumLocalSaps = 0;
    #if !defined(BLHPK)
    FD_ZERO(&pBlrudp->fdsetRead);
    FD_ZERO(&pBlrudp->fdsetWrite);
    #endif

    QInit(&pBlrudp->listLocalNICs);

    // is the master list initialized,
    if (g_listBlrudpMgrs.next == NULL)      // no
        QInit(& g_listBlrudpMgrs);          // initialize it.

    EnQ(&pBlrudp->link,&g_listBlrudpMgrs);  // add us into the master list.

    //
    // TBD.. process the parameter list here....
    // paramter list overrides the defaults....
    nRet = BlrudpProcessOptions(&pBlrudp->DefOptions,         // *pOptions,
                                nNumOptions,                  // nNumOptions,
                                pOptions);                    // pOptionList[])

    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);




    //
    // Create a timeing manager
    //
    pBlrudp->pTimeMgr = BltwNew();
    if (pBlrudp->pTimeMgr == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);


    pBlrudp->pRecvWorkMgr = BlrWorkNew();
    if (pBlrudp->pRecvWorkMgr  == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);

    pBlrudp->pXmitDoneWorkMgr = BlrWorkNew();
    if (pBlrudp->pXmitDoneWorkMgr  == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);
    //
    // Maps for local saps to passive context
    //
    pBlrudp->mapPassCtxs = BlMapNew(BlrudpCompareSaps,    // pfnCompare
                                          NULL,                 // pfnDelKeyAndItem
                                          NULL);                // pUserData
    if (pBlrudp->mapPassCtxs == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);         // oops, out of memory.

    //
    // Map of sap pairs to active contexts.
    //
    pBlrudp->mapActCtxs = BlMapNew(BlrudpCompareConnectSaps,  // pfnCompare
                                          NULL,                     // pfnDelKeyAndItem
                                          NULL);                    // pUserData
    if (pBlrudp->mapActCtxs == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);         // oops, out of memory.


    //
    // map of all local saps (both passive and active).
    //
    pBlrudp->mapLocalSaps = BlMapNew(BlrudpCompareSaps,        // pfnCompare
                                     NULL,                     // pfnDelKeyAndItem
                                     NULL);                    // pUserData
    if (pBlrudp->mapLocalSaps == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);         // oops, out of memory.




    *phBlrudp = (BLRUDP_HANDLE)pBlrudp;       // return an opaque handle


    return(BLERR_SUCCESS);
Fail:
    if (pBlrudp)                // ditch the memory if we allocated any.
        BlrudpDelete(pBlrudp);
    return(nErr);

}




////////////////////////////////////////////////////////////////////////////////////
int BlrudpDelete(BLRUDP_HANDLE hBlrudp)
//
// Delete the Blue Light RUDP manager created by BlRudpNew.
//
// inputs:
//      hBlrudp -- handle to the instance to delete.
//
// outputs:
//    return BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_MGR *pBlrudp = NULL;
    _QLink *pList;

    if (hBlrudp == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    pBlrudp = (BLRUDP_MGR *)hBlrudp;      // recover our pointer from the opaque type.



    //
    // Remove all active contexts.
    //
    pList = &pBlrudp->listActCtxs;
    while (QisFull(pList))
    {
        BLRUDP_ACTIVE_CONTEXT *pContext =  (BLRUDP_ACTIVE_CONTEXT *)QBegin(pList);
        // dispose of the context
        BlrudpActCtxDelete(pBlrudp,
                                   pContext);
    }

    pList = &pBlrudp->listPassCtxs;
    while (QisFull(pList))
    {
        BLRUDP_PASSIVE_CONTEXT *pContext =  (BLRUDP_PASSIVE_CONTEXT *)QBegin(pList);
        // dispose of the context
        BlrudpPassCtxDelete(pBlrudp,
                                   pContext);
    }


    if (pBlrudp->pRecvWorkMgr)                  // delete the work manager.
    {
        BlrWorkDelete(pBlrudp->pRecvWorkMgr);
        pBlrudp->pRecvWorkMgr = NULL;
    }

    if (pBlrudp->pXmitDoneWorkMgr)                  // delete the work manager.
    {
        BlrWorkDelete(pBlrudp->pXmitDoneWorkMgr);
        pBlrudp->pXmitDoneWorkMgr = NULL;
    }

    if (pBlrudp->pTimeMgr)                      // delete the timer manager.
    {
        BltwDelete(pBlrudp->pTimeMgr);
        pBlrudp->pTimeMgr = NULL;
    }

    //
    // dispose of the maps.
    if (pBlrudp->mapPassCtxs)
    {
        BlMapDelete(pBlrudp->mapPassCtxs);
        pBlrudp->mapPassCtxs = NULL;
    }

    if (pBlrudp->mapActCtxs)
    {
        BlMapDelete(pBlrudp->mapActCtxs);
        pBlrudp->mapActCtxs = NULL;
    }


    if (pBlrudp->mapLocalSaps)
    {
        BlMapDelete(pBlrudp->mapLocalSaps);
        pBlrudp->mapLocalSaps = NULL;
    }


    if (pBlrudp->pPktPool)
        BlrudpIoPktPoolDelete(pBlrudp->pPktPool);

    //
    // removal active context and passive context should
    // have removed these...
    ASSERT(QisEmpty(&pBlrudp->listLocalSaps));
    //
    // Delete anyway...
    //
    while (QisFull(&pBlrudp->listLocalSaps))
    {
        BLRUDP_LOCAL_SAP_ITEM *pLocalSap=  (BLRUDP_LOCAL_SAP_ITEM *)QBegin(&pBlrudp->listLocalSaps);
        // dispose of the context
        BlrudpLocalSapDelete(pBlrudp, pLocalSap);
    }

    //
    // Removal of the local saps should have taken care of this..
    //
    ASSERT(QisEmpty(&pBlrudp->listLocalNICs));
    //
    // delete anyway...
    //
    while (QisFull(&pBlrudp->listLocalNICs))
    {
        BLRUDP_NIC_ITEM *pNicItem =  (BLRUDP_NIC_ITEM *)QBegin(&pBlrudp->listLocalNICs);
        // dispose of the context
        BlrudpNicItemDelete(pNicItem);
    }

    BlrudpCleanupSockets();

    DeQ(&pBlrudp->link);            // remove us us into the master list.


    BlrFreeMem(pBlrudp);          // commite suicide.


    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

//////////////////////////////////////////////////////////////////////
int BlrudpRetrieveOptions(BLRUDP_CONTEXT_OPTIONS *pOptions,
                         int nNumOptions,
                         BLRUDP_OPTIONS pOptionList[])
//
// Retrieve the option list.  This structure appears in the manager, active and
// passive contexts...
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pOptions    -- pointer to the option structure to stuff with override
//                   options.
//    nNumOptions -- number of options in the pOptionsList.
//    pOptionsList -- array of options values.
//  outputs:
//     returns -- BLERR_SUCCESS if successful.
{
    int n;
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_OPTIONS *pOpt;

    //
    // if there is no option list then just return success immediatly..
    //
    if ((nNumOptions == 0) || ((nNumOptions == 0) && (pOptionList == NULL)))
        return(BLERR_SUCCESS);

    if (pOptionList == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // TBD.. do some kind of sanity check on this....
    //
    for (n = 0, pOpt = pOptionList;
         n < nNumOptions;
         n++, pOpt++)
    {
        switch(pOpt->nOptionId)
        {
            case BLRUDP_WINDOW_SIZE:
            {
                pOpt->v.ulValue = pOptions->ulWindowSize    ;
                break;
            }
            case BLRUDP_RETRY_COUNT:
            {
                pOpt->v.ulValue = pOptions->ulMaxRetryCount ;
                break;
            }
            case BLRUDP_TRANSMIT_RATE:
            {
                pOpt->v.ulValue = pOptions->ulXmitRate  ;
                break;
            }
            case BLRUDP_BURST_SIZE:
            {
                pOpt->v.ulValue = pOptions->ulXmitBurstSize ;
                break;
            }
            case BLRUDP_DEFAULT_SRC_PORT:
            {
                ///BGF (uint16_t)pOpt->v.ulValue = pOptions->nsSrcPort        ;
                pOpt->v.ulValue = pOptions->nsSrcPort        ;
                break;
            }
            case BLRUDP_SOURCE_IP_ADDR:
            {
                //
                // may want to take this off a string instead....
                //
                pOpt->v.ulValue = pOptions->ulSrcIpAddr      ;
                break;
            }
            case BLRUDP_INITIAL_ROUND_TRIP_DELAY:
            {
                pOpt->v.ulValue = pOptions->ulRndTripDelay   ;
                break;
            }
            case BLRUDP_MAXIMUM_TIME_TO_LIVE:
            {
                pOpt->v.ulValue = pOptions->ulMTL;
                break;
            }
            case BLRUDP_IN_ORDER_DELIVERY:
            {
                pOpt->v.ulValue =  (pOptions->bInOrderDelivery != 0);
                break;
            }
            case BLRUDP_MTU_SIZE:
            {
                pOpt->v.ulValue = pOptions->ulMtuSize ;
                break;
            }
            case BLRUDP_DEDICATED_BUFFERS:
            {
                pOpt->v.ulValue = (pOptions->bDedicatedBuffers != 0);
                break;
            }
            case BLRUDP_USE_RAW_SOCKETS:
            {
                pOpt->v.ulValue = (pOptions->bUseRawSockets != 0);
                break;
            }
            case BLRUDP_CHECKSUM:
            {
                pOpt->v.ulValue = (pOptions->bChecksum != 0);
                break;
            }
            case BLRUDP_APPLEVEL_FLOW_CNTRL:
            {
                pOpt->v.ulValue = (pOptions->bAppFlowCtl != 0);
                break;
            }
            default:
            {
                FAILERR(nErr, BLERR_PARAM_ERR);
                break;
            }
        }       // switch...
    }    // for...



    return(BLERR_SUCCESS);
Fail:
    return(nErr);

}


//////////////////////////////////////////////////////////////////////////////////
int BlrudpProcessOptions(BLRUDP_CONTEXT_OPTIONS *pOptions,
                         int nNumOptions,
                         BLRUDP_OPTIONS pOptionList[])
//
// Process the option list and load/override the data into the pOptions
// structure.  This structure appears in the manager, active and
// passive contexts...
//
// inputs:
//    pOptions    -- pointer to the option structure to stuff with override
//                   options.
//    nNumOptions -- number of options in the pOptionsList.
//    pOptionsList -- array of options values.
//  outputs:
//     returns -- BLERR_SUCCESS if successful.
//
{
    int n;
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_OPTIONS *pOpt = pOptionList;

    //
    // if there is no option list then just return success immediatly..
    //
    if ((nNumOptions == 0) || ((nNumOptions == 0) && (pOptionList == NULL)))
        return(BLERR_SUCCESS);

    if (pOptionList == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // TBD.. do some kind of sanity check on this....
    //
    for (n = 0, pOpt = pOptionList;
         n < nNumOptions;
         n++, pOpt++)
    {
        switch(pOpt->nOptionId)
        {
            case BLRUDP_WINDOW_SIZE:
            {
                if (pOpt->v.ulValue)            // zero means use the default size...
                    pOptions->ulWindowSize    = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_RETRY_COUNT:
            {
                pOptions->ulMaxRetryCount = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_TRANSMIT_RATE:
            {
                pOptions->ulXmitRate  = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_BURST_SIZE:
            {
                pOptions->ulXmitBurstSize = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_DEFAULT_SRC_PORT:
            {
                pOptions->nsSrcPort        = (uint16_t)pOpt->v.ulValue;
                break;
            }
            case BLRUDP_SOURCE_IP_ADDR:
            {
                //
                // may want to take this off a string instead....
                //
                pOptions->ulSrcIpAddr      = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_INITIAL_ROUND_TRIP_DELAY:
            {
                pOptions->ulRndTripDelay   = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_MAXIMUM_TIME_TO_LIVE:
            {
                pOptions->ulMTL            = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_IN_ORDER_DELIVERY:
            {
                pOptions->bInOrderDelivery = (pOpt->v.ulValue != 0);
                break;
            }
            case BLRUDP_MTU_SIZE:
            {
                pOptions->ulMtuSize = pOpt->v.ulValue;
                break;
            }
            case BLRUDP_DEDICATED_BUFFERS:
            {
                pOptions->bDedicatedBuffers = (pOpt->v.ulValue != 0);
                break;
            }
            case BLRUDP_USE_RAW_SOCKETS:
            {
                pOptions->bUseRawSockets = (pOpt->v.ulValue != 0);
                break;
            }
            case BLRUDP_CHECKSUM:
            {
                pOptions->bChecksum = (pOpt->v.ulValue != 0);
                break;
            }
            case BLRUDP_APPLEVEL_FLOW_CNTRL:
            {
                pOptions->bAppFlowCtl = (pOpt->v.ulValue != 0);
                break;
            }
            default:
            {
                FAILERR(nErr, BLERR_PARAM_ERR);
                break;
            }
        }       // switch...
    }    // for...



    return(BLERR_SUCCESS);
Fail:
    return(nErr);

}

//////////////////////////////////////////////////////////////////////////////
void BlrudpShutdown()
//
// Call this when the program shuts down.  This will go through all the outstanding
// Blrudp objets and attempt close any outstanding socket handles associated
// with it.
//
// inputs:
//    none.
// outputs:
//    none.
///////////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_MGR *pBlrudp = NULL;
    //
    //
    // surgically close down the sockets rather than attempting to deallocate
    // everything.
    //
    if (g_listBlrudpMgrs.next == NULL)      // uninitialized, nothing to do...
        return;
    while (QisFull(&g_listBlrudpMgrs))
    {
        pBlrudp = (BLRUDP_MGR *)QBegin(&g_listBlrudpMgrs);
        BlrudpDelete(pBlrudp);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
int BlrudpSetOptions(BLRUDP_HANDLE hBlrudp,
                     int nNumOptions,
                     BLRUDP_OPTIONS pOptions[])
//
// set the connection options associated with this UDP manager.
// Options apply to all subsequent connections managed by this manager.
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nNumOptions  -- Number of paramters in the parameter list.
//      pOptions     -- Paramter list containing the parameters the client
//                     application wishes to override.
// outputs:
//  returns BLERR_SUCCESS if successful,
//           error codes TBD..
//////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_MGR *pBlrudp = NULL;
    BLRUDP_CONTEXT_OPTIONS Options;

    if (hBlrudp == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    pBlrudp = (BLRUDP_MGR *)hBlrudp;      // recover our pointer from the opaque type.

    memcpy(&Options, &pBlrudp->DefOptions, sizeof(Options));
    nRet = BlrudpProcessOptions(&pBlrudp->DefOptions,           // *pOptions,
                                nNumOptions,                    // nNumOptions,
                                pOptions);                      // pOptionList[])
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);

    //
    // bitwise copy back...
    //
    memcpy(&pBlrudp->DefOptions, &Options, sizeof(Options));

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

///////////////////////////////////////////////////////////////////
int BlrudpGetOptions(BLRUDP_HANDLE hBlrudp,
                     int nNumOptions,
                     BLRUDP_OPTIONS pOptions[])
//
// Retrieve the current option values.
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nNumOptions  -- Number of paramters in the parameter list.
//      pOptions     -- Paramter list containing the parameters the client
//                     application wishes to query.
// outputs:
//  returns BLERR_SUCCESS if successful,
//           error codes TBD..
//
//  NOTE:
//     Pointer values returned belong to the Blrudp library.
//     DON'T MODIFY THEM.
//////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_MGR *pBlrudp = NULL;

    if (hBlrudp == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    pBlrudp = (BLRUDP_MGR *)hBlrudp;      // recover our pointer from the opaque type.

    nRet = BlrudpRetrieveOptions(&pBlrudp->DefOptions,         // *pOptions,
                                nNumOptions,                  // nNumOptions,
                                pOptions);                    // pOptionList[])

    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    return(BLERR_SUCCESS);
Fail:
    return(nErr);

}

//////////////////////////////////////////////////////////////////////
int BlrudpSetOptionLong(BLRUDP_HANDLE hBlrudp,
                        int nOptionId,
                        long lValue)
//
// Set a single option (long values only).
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nOptionID   -- identifier of the option to return.
//      lValue     -- Value to set.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_MGR *pBlrudp = NULL;
    BLRUDP_OPTIONS Option;

    if (hBlrudp == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);
    pBlrudp = (BLRUDP_MGR *)hBlrudp;      // recover our pointer from the opaque type.

    //
    // setup to to an array of 1.
    //
    Option.nOptionId = nOptionId;
    Option.v.lValue = lValue;
    nRet = BlrudpProcessOptions(&pBlrudp->DefOptions,           // *pOptions,
                                1,                              // nNumOptions,
                                &Option);                       // pOptionList[])
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

//////////////////////////////////////////////////////////////////////
int BlrudpGetOptionLong(BLRUDP_HANDLE hBlrudp UNUSED,
                        int nOptionId UNUSED,
                        long *plValue UNUSED)
//
// Retrieve a single option (long values only).
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nOptionId   -- identifier of the option to return.
//      plValue     -- place to stuff the option value.
// outputs:
//      returns BLERR_SUCCESS if successful.
//      *plValue -- Value returned by the option.
//
////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_MGR *pBlrudp = NULL;
    BLRUDP_OPTIONS Option;

    if (hBlrudp == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);
    pBlrudp = (BLRUDP_MGR *)hBlrudp;      // recover our pointer from the opaque type.

    //
    // setup to to an array of 1.
    //
    Option.nOptionId = nOptionId;
    nRet = BlrudpRetrieveOptions(&pBlrudp->DefOptions,           // *pOptions,
                                1,                              // nNumOptions,
                                &Option);                       // pOptionList[])
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);

    *plValue = Option.v.lValue;

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}


#if ( (!defined(BLHPK)) && (!defined(WIN32)) )

//////////////////////////////////////////////////////////////////////
int BlrudpGetPollList(BLRUDP_HANDLE hBlrudp,
                      unsigned int nMaxPollDescriptors,
                      unsigned int *pnNumPollDescriptors,
                      struct pollfd *pPollFd,
                      struct timeval * pTimeout)

//
// Return a list of pollfd structures containing and the timeout value to
// use if the calling function  wishes to put the application to sleep until
// something interesting happens.  Also returns the
// time the application wishes to wake up for a timeout.
//
// this allows the client application to go to sleep until something
// of interest in this library or something else the client is interested
// in pops up.
//
// inputs:
//      hBlrudp -- instance to the instance to apply these options to.
//      nMaxPollDescriptors -- maximum number of poll descriptors that there is
//                               space to return.
//      pnNumPollDescriptors -- pointer to where to stuffnumber of poll
//                              descriptors actually in the list.
//      pPollFd --  pointer to an array of pollfd structs where we can stuff
//                  the list.
//                  if (pPollFd == NULL, then just stuff the number of maximum descriptors
//                      we would have retruned...)
//     pTimeout -- pointer where to stuff the current timeout value.
//
// outputs:
//      return -- BLERR_SUCCESS if successful.
//            BLERR_STRUCT_TOO_SMALL if pPollfd area is too small to contain the
//                                   number of poll structs.
//                                   pnNumPollDescriptors -- contains the number
//                                   that would be returned if there was enough space.
//      *pnNumPollDescriptors -- number of poll fd structs in the list.
//     *pTimeout -- stuff the current timeout value.
//
//
{
    int nErr = BLERR_INTERNAL_ERR;

    BLRUDP_MGR *pBlrudp = (BLRUDP_MGR *)hBlrudp;      // recover our pointer from the opaque type.

    if ((pBlrudp == NULL) || (pnNumPollDescriptors == NULL))
        FAILERR(nErr, BLERR_PARAM_ERR);

    *pnNumPollDescriptors = pBlrudp->ulNumLocalSaps;
    if (pPollFd)
    {
        BLRUDP_LOCAL_SAP_ITEM *pSap;
        struct pollfd *pFd = pPollFd;
        if (nMaxPollDescriptors < pBlrudp->ulNumLocalSaps)
            FAILERR(nErr, BLERR_STRUCT_TOO_SMALL);
        memset(pPollFd, 0, sizeof(*pPollFd) * nMaxPollDescriptors);        // start out clear

        //
        // Create a poll descriptor for EACH local sap we have...
        //
        for (pSap = (BLRUDP_LOCAL_SAP_ITEM *)QBegin(&pBlrudp->listLocalSaps);
             &pSap->link != QEnd(&pBlrudp->listLocalSaps);
             pSap = (BLRUDP_LOCAL_SAP_ITEM *)pSap->link.next)
        {
            pFd->fd = pSap->hSocket;        // save the file descriptor...
            pFd->events = POLLIN | POLLPRI; // there is data to read, or urgent data to read..
            if (BlrWorkNumItems(pSap->pXmitWorkMgr))
                pFd->events |= POLLOUT;     // Writing now will not block */
            pFd++;      // next....
        }
    }
    if (pTimeout)
    {
        BltwGetTimeout(pBlrudp->pTimeMgr,        // pTimerMgr
                       pTimeout,                 // pTimeValue
                       0);                       // nMaxcheck
    }
    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}
#endif

#ifndef BLHPK
////////////////////////////////////////////////////////////////////////////////
int BlrudpGetSelectSet(BLRUDP_HANDLE hBlrudp,
                       int *pnFds,
                       fd_set * pReadFds,
                       fd_set * pWriteFds,
                       fd_set * pExceptFds,
                       struct timeval * pTimeout)

//
// returns read, write and except File Descriptor sets which can then be used to call
// select and block the application until something interesting happens
// in the protocol.
//
// inputs:
//     hBlrudp     -- instance to the instance to apply these options to.
//     pnFds -- pointer where to stuff the highest numbered descriptor in
//              any of the FDsets.
//     pReadFds -- pointer where to stuff the read file descriptor set.
//     pWriteFds -- pointer where to stuff the write file descriptor set.
//     pExceptFds -- pointer where to stuff the except file descriptor set.
//     pTimeout -- pointer where to stuff the current timeout value.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
//     pnFds -- the highest numbered descriptor in
//                 any of the FDsets.
//     pReadFds -- the read file descriptor set.
//     pWriteFds -- the write file descriptor set.
//     pExceptFds -- the except file descriptor set.
//     pTimeout -- stuff the current timeout value.
//
//////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int hHighFd;
    BLRUDP_MGR *pBlrudp = (BLRUDP_MGR *)hBlrudp;      // recover our pointer from the opaque type.

    if (pBlrudp == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    if (QisEmpty(&pBlrudp->listLocalSaps))
        FAILERR(nErr, BLERR_BLRUDP_NO_CONTEXTS);

    if (pnFds == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);
    //
    // Read selects are already build as the local saps come and go..
    //
    // Write selects are dynamic... so we have to rebuild them.
    //
    BlrudpLocalSapBuildWriteSelects(pBlrudp, &hHighFd);
    ASSERT(hHighFd != INVALID_SOCKET);
    if (hHighFd == INVALID_SOCKET)
        FAILERR(nErr, BLERR_BLRUDP_SOCKET_ERR);


    *pnFds = hHighFd;

    if (pExceptFds)
        FD_ZERO(pExceptFds);


    if (pReadFds)
        *pReadFds = pBlrudp->fdsetRead;
    if (pWriteFds)
        *pWriteFds = pBlrudp->fdsetWrite;

    // Get the minimum timeout value....
    if (pTimeout)
    {
        BltwGetTimeout(pBlrudp->pTimeMgr,  // pTimerMgr
                      pTimeout,            // pTimeValue
                      0);                  // nMaxcheck
    }
    return(BLERR_SUCCESS);
Fail:
    return(nErr);

}
#endif


///////////////////////////////////////////////////////////////////////////////////
int BlrudpRun(BLRUDP_HANDLE hBlrudp,
              int bSleep UNUSED)
//
// Give the RudpManager time to run.
// The manager does all its work of receiving data here.
//
// If the client application has programmed any callbacks, they will be called
// from within this function so we don't have to worry about problems
// re-entrancy and interrupt contexts.
//
// inputs:
//     hBlrudp     -- instance to the instance to apply these options to.
//     bSleep      -- if TRUE the run command will sleep until there is
//                    something interesting to do.  Incomming connection,
//                    timeout, receive data, send buffer done etc...
// outputs:
//     returns -- BLERR_SUCCESS if successful.
////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_MGR *pBlrudp;

    ASSERT(hBlrudp);

    if (hBlrudp == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    pBlrudp = (BLRUDP_MGR *)hBlrudp;   // recover our pointer from the opaque type.

    pBlrudp->ulRunNum++;                //


    #ifdef BLHPK
    BlrudpNicRun(&pBlrudp->listLocalNICs,       // do the blhpk run thing here...
                 pBlrudp->mapLocalSaps);
    #else
    if (bSleep)
        BlrudpSleep(pBlrudp);       // sleep until something interesting happens....
    #endif

    //
    // 1. Check for receive data.

    BlrudpRunRecv(pBlrudp);     // receive any data.


    BlrudpRunRecvWork(pBlrudp); // give the receiver a shot at it...

    //
    // 2. Check timers...
    BltwTimerRun(pBlrudp->pTimeMgr);     // run the timer...

    BlrudpRunXmitDoneWork(pBlrudp);

    BlrudpRunXmitWork(pBlrudp);

    //
    // do a round robin scheduling,   move the first to the last.
    //
    if (QisFull(&pBlrudp->listLocalSaps))
    {
        _QLink *pSap = QBegin(&pBlrudp->listLocalSaps);
        DeQ(pSap);
        EnQ(pSap,&pBlrudp->listLocalSaps);
    }
    //
    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

#if !defined(BLHPK)                             // high performance kernel does not sleep
/////////////////////////////////////////////////////////////////////////////
void BlrudpSleep(BLRUDP_MGR *pBlrudp UNUSED)
//
// Put the manager to sleep until something of interest happens, such as a
// data receive or send buffer becomes ready.
//
//    pBlrudp -- pointer to the blrudp manager.
// outputs:
//    none.
{
    struct timeval Timeout;
    int bTimeout;
    int hHighFd;
    int nRet;
    fd_set   fdsetRead;                  // current file descriptor sets for read and write.
    fd_set   fdsetWrite;                 // select calls...

    //
    // if we have NO local saps, then this is meaningless, because
    // we would sleep forever...
    //
    // NOTE: We may just want to sleep for a nominal interval here...
    // if this is the case....
    //
    if (QisEmpty(&pBlrudp->listLocalSaps))
        return;

    //
    // don't sleep if we have receive work to do...
    //
    if (BlrWorkNumItems(pBlrudp->pRecvWorkMgr) > 0)
        return;


    //
    // Read selects are already build as the local saps come and go..
    //
    // Write selects are dynamic... so we have to rebuild them.
    //
    BlrudpLocalSapBuildWriteSelects(pBlrudp, &hHighFd);
    ASSERT(hHighFd != INVALID_SOCKET);
    if (hHighFd == INVALID_SOCKET)
        FAIL;

    // Get the minimum timeout value....
    bTimeout = BltwGetTimeout(pBlrudp->pTimeMgr,        //
                              &Timeout,                 // pTimeValue
                              0);                       // nMaxcheck
    //
    // TBD.. WE may want to special case this when we have no timeouts
    // for now, we will wake up once per interval of the timerwheel...
    //


    fdsetRead  = pBlrudp->fdsetRead;
    fdsetWrite = pBlrudp->fdsetWrite;

    #ifdef _DEBUG
        //
        // for debug max out the timeout so we can
        // signal we want to stop here...
        //
        if (Timeout.tv_sec > 2)
            Timeout.tv_sec = 2;
    #endif
    nRet = select(hHighFd+1,
                  &fdsetRead,             // readfds
                  &fdsetWrite,            // writefds
                  NULL,                   // exceptfds,
                  &Timeout);              // timeout
    //ASSERT(nRet != SOCKET_ERROR);
  // SOCKET_ERRor is happens normally when


    return;
Fail:
    return;
}
#endif

/////////////////////////////////////////////////////////////////////////////
void BlrudpRunXmitWork(BLRUDP_MGR *pBlrudp)
//
// Run the transmit work queue as long as we are clear to send...
// or until we run out of work.
//
// inputs:
//    pBlrudp -- pointer to the blrudp manager.
// outputs:
//    none.
{
    // we have one xmit work queue PER local sap...
    BLRUDP_LOCAL_SAP_ITEM *pSap;
    int bXmitReady;
    int bDidWork;
    int nCount = 0;
    //
    // for some stupid reason I don't understand, the windows TCP stack
    // starts dropping packets if we loop more than 5 packets worth of
    // data....
    //
    #ifdef WIN32
        int nLoops = 5;            // do the 1st 5 items of work...
    #else
        int nLoops = 50;            // do the 1st 50 items of work...
    #endif

    //
    // for all our local saps....
    //
    bDidWork = FALSE;
    pSap = (BLRUDP_LOCAL_SAP_ITEM *)QBegin(&pBlrudp->listLocalSaps);
    while (&pSap->link != QEnd(&pBlrudp->listLocalSaps))
    {
        bXmitReady = BlrIoDgramXmitReady(pSap->hSocket);
        if (! bXmitReady)       // this one is not ready, try the next SAP.....
            continue;
        //
        //
        if (BlrWorkNumItems(pSap->pXmitWorkMgr) > 0)
        {
            int bMoreWork = TRUE;
            while ((bMoreWork) && (nLoops > 0) && (bXmitReady))
            {
                bMoreWork = BlrWorkRun(pSap->pXmitWorkMgr);
                nLoops--;           // don't stay here forever...
                nCount++;
                bXmitReady = BlrIoDgramXmitReady(pSap->hSocket);
            }
            bDidWork = TRUE;                    // remember we actually did some work...
        }
        pSap = (BLRUDP_LOCAL_SAP_ITEM *)pSap->link.next;
    }
}

///////////////////////////////////////////////////////////////////////////////
void BlrudpRunXmitDoneWork(BLRUDP_MGR *pBlrudp)
//
// Run the Perform callbacks for all the active contexts that have
// buffers they can free for reuse...
//
//
// inputs:
//    pBlrudp -- pointer to the blrudp manager.
// outputs:
//    none.
//
{
    //
    // run this until there is no more receive work...
    //
    while(TRUE)
    {
        int bMoreWork;
        bMoreWork = BlrWorkRun(pBlrudp->pXmitDoneWorkMgr);
        if (!bMoreWork)
            break;
    }
}


/////////////////////////////////////////////////////////////////////////////
void BlrudpRunRecvWork(BLRUDP_MGR *pBlrudp)
//
// Run the receive work queue until we run out of work.
//
// This typically results in receive callbacks to the client application.
//
// inputs:
//    pBlrudp -- pointer to the blrudp manager.
// outputs:
//    none.
{
    //
    // run this until there is no more receive work...
    //
    uint32_t ulNumWorkItems;

    //
    // don't stay in here forever.
    // the receive work item can put more work
    // back on this and we don't want to keep waking up the
    // dead with this...
    ulNumWorkItems = BlrWorkNumItems(pBlrudp->pRecvWorkMgr);

    while(ulNumWorkItems)
    {
        int bMoreWork;
        bMoreWork = BlrWorkRun(pBlrudp->pRecvWorkMgr);
        if (!bMoreWork)
            break;
        ulNumWorkItems--;
    }

}

/////////////////////////////////////////////////////////////////////////////
void BlrudpRunRecv(BLRUDP_MGR *pBlrudp)
//
// Perform the receive data portion of the BlrudpRun process.
//
// inputs:
//    pBlrudp -- pointer to the blrudp manager.
// outputs:
//    none.
{

    BLRUDP_LOCAL_SAP_ITEM *pSap;

      // pick up the first in the list.
    for (pSap = (BLRUDP_LOCAL_SAP_ITEM *)QBegin(&pBlrudp->listLocalSaps);
         &pSap->link != QEnd(&pBlrudp->listLocalSaps);
         pSap = (BLRUDP_LOCAL_SAP_ITEM *)pSap->link.next)
    {
        int nLoops = 0;
        int nRet;

        //
        // we have data...
        // .. should we drain it???
        //
        while ((BlrIoDgramRecvReady(pSap->hSocket)) && (nLoops < 50))
        {
            nRet = BlrudpRunRecvFromSap(pBlrudp, pSap);
            if (nRet != BLERR_SUCCESS)
                break;
            nLoops++;
        } // while...
    }   // for(...
}

/////////////////////////////////////////////////////////////////////
int BlrudpRunRecvFromSap(BLRUDP_MGR *pBlrudp,
                         BLRUDP_LOCAL_SAP_ITEM *pSap)
//
// Receive data from a specific SAP.
//
// inputs:
//    pBlrudp -- pointer to the blrudp manager.
//    pSap -- sap to receive data from.
// outputs:
//    returns -- BLERR_SUCCESS if we had a successful receive.
//               != BLERR_SUCCESS if we were not able to clear
//               the receive condition....
//
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_SAP_ADDR SapAddr;
    //
    // NOTE, for the IO node implementation, we don't want recvfrom to copy
    // the data but we want to grab a buffer directly from the lower level
    // driver.  This avoids copying, which will be deadly to the thru-put.
    //
    BLRUDP_IO_PKT *pRecvPkt = NULL;
    BLRUDP_CONNECTION_SAPS SapPair;
    BLRUDP_ACTIVE_CONTEXT  *pActCtx = NULL;
    BLRUDP_PASSIVE_CONTEXT *pPassCtx = NULL;

    ////////////////////////////////////////////////////////////////////////
    // receive a Packet from the local sap....
    //
    nRet = BlrIoRecvPkt(pSap,                   // pLocalSap
                        &pRecvPkt,              // **ppPkt
                        &SapAddr);


    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);

    ///////////////////////////////////////////////////////////
    // if we get this far, then we have valid address
    //
    // First check to see if we have any active contexts matching this pair
    // of saps.
    SapPair.LocalSap.ulIpAddr   = pSap->Sap.ulIpAddr;
    SapPair.LocalSap.nPort      = pSap->Sap.nPort;
    SapPair.RemoteSap.ulIpAddr  = SapAddr.ulIpAddr;
    SapPair.RemoteSap.nPort     = SapAddr.nPort;
    pActCtx = BlMapFindItem(pBlrudp->mapActCtxs, &SapPair);
    if (pActCtx)
    {
        // This packet came from the global free pool
        // so debit the active context, such that when
        // we return the packet we do the correct thing.
        nRet = BlrudpIoPktDebitRecvPkt(pActCtx->pPktPool);
        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, nRet);
        //
        // NOTE if we have any failures after this point then we must credit
        // the pktpool...

        ASSERT(pRecvPkt->link.next == NULL);
        BlrudpActiveRecvPkt(pActCtx, pRecvPkt);

    }
    else
    {
        // unsigned int ulIpAddr;
        // ulIpAddr = htonl(SapAddr.ulIpAddr);
        //
        //dbgPrintf("PassiveRecv(%ld.%ld.%ld.%ld)\n",
        //         (ulIpAddr >> 24) & 0xff,
        //         (ulIpAddr >> 16) & 0xff,
        //         (ulIpAddr >>  8) & 0xff,
        //         (ulIpAddr >>  0) & 0xff);
        // no active contexts.  Check to see if we have any passive (listening)
        // contexts assocated with the local sap...
        pPassCtx = BlMapFindItem(pBlrudp->mapPassCtxs, &pSap->Sap);
        ASSERT(pPassCtx);        // HUH????

        if (pPassCtx)
        {
            // This packet came from the global free pool
            // so debit the active context, such that when
            // we return the packet we do the correct thing.
            nRet = BlrudpIoPktDebitRecvPkt(pPassCtx->pPktPool);
            if (nRet != BLERR_SUCCESS)
                FAILERR(nErr, nRet);
            //
            // NOTE if we have any failures after this point then we must credit
            // the pktpool...
            BlrudpPassiveRecvPkt(pPassCtx,                          // *pPassContext,
                                 NULL,                              // *pExistingActContext,
                                 pRecvPkt,                          // *pRecvPkt,
                                 SapAddr.ulIpAddr,                  // ulIpAddr,
                                 SapAddr.nPort);                    // nPort )

            BlrudpIoPktPutRecvPkt(pPassCtx->pPktPool,        // return it to the pool
                             pRecvPkt);
        }
        else
        {
            // nobody picked it up.
            // return it to the NIC...
            //
            BlrudpPutNicFreeRecvPkt(pSap->pLocalNic, pRecvPkt);     // return the packet..
        }

    }

    return(BLERR_SUCCESS);
Fail:
    if (pRecvPkt)
        BlrudpPutNicFreeRecvPkt(pSap->pLocalNic, pRecvPkt);     // return the packet..
    return(nErr);
}
/////////////////////////////////////////////////////////////////////////////
int BlrudpListen(BLRUDP_HANDLE hBlrudp,
                 BLRUDP_HCONTEXT *phContext,
                 unsigned long ulIpAddr,
                 unsigned short nPort,
                 int nNumOptions,
                 BLRUDP_OPTIONS pOptions[],
                 PFN_BLRUDP_LISTEN_CONNECT_REQ pfnListenConnectReq,
                 void *pUserData)
//
// Listen for incoming connections.
//
// This causes the Blrudp manager to listen for incomming connections on
// a designated port.  Nothing in this computer can be listenening
// to this IP address and port combination.
//
// inputs:
//    hBlrudp     -- instance to the instance to apply these options to.
//    phContext   -- pointer to where to stuff the context handle to refer
//                   to this listening context.
//    ulIpAddr -- source IP address in network byte order.
//                (same as returned by inet_addr()..)
//                if zero, use the default ip Address.
//    nPort -- port to listen for incomming connections on.
//             if 0 then use the default port.
//    nNumOptions -- number of paramters that we wish to override the default values for.
//    pOptions -- pointer to list of parameters.
//    pfnListenConnectReq -- callback to call when a connect request comes in.
//    pUserData -- pointer to user data associated with this callback context.
//
//  outputs:
//     returns -- BLERR_SUCCESS if successful.
//     *phContext -- newly created listening context.
////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_PASSIVE_CONTEXT *pContext = NULL;
    BLRUDP_PASSIVE_CONTEXT *pExistingContext = NULL;
    BLRUDP_LOCAL_SAP_ITEM *pLocalSap = NULL;
    BLRUDP_MGR *pBlrudp = (BLRUDP_MGR *)hBlrudp;
    BLRUDP_SAP_ADDR Sap;
    int nRet;

    ASSERT(hBlrudp && phContext);
    if (hBlrudp == NULL)
        FAIL;

    //
    // convert the address to network order.
    //

    ASSERT(pfnListenConnectReq);            // if we don't have one of these
                                            // then there is no way to approve
                                            // or reject the connection.
    if (pfnListenConnectReq == NULL)        // by by..
        FAILERR(nErr, BLERR_PARAM_ERR);

    Sap.ulIpAddr = ulIpAddr;
    Sap.nPort = nPort;
    //
    // check first to see if we have a passive context for this SAP.
    //
    pExistingContext= BlMapFindItem(pBlrudp->mapPassCtxs, &Sap);
    if (pExistingContext)
        FAILERR(nErr, BLERR_BLRUDP_DUP_SAP);



    //
    // Create and initialize a new passive context structure.
    //
    pContext = BlrudpPassCtxNew(pBlrudp);
    if (pContext == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);


    nRet = BlrudpProcessOptions(&pContext->Options,             // *pOptions,
                                nNumOptions,                    // nNumOptions,
                                pOptions);                      // pOptionList[])
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    //
    // Remember our callback.
    //
    pContext->pfnListenConnectReq = pfnListenConnectReq;
    pContext->pUserData = pUserData;


    nRet = BlrudpLocalSap(pBlrudp,
                          &pLocalSap,
                          ulIpAddr,
                          nPort);
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    ASSERT(pLocalSap);
    pContext->pLocalSap = pLocalSap;        // save this

    //
    // Now we have the local sap, and therefore the MTU size..
    // Double check that we have it and allocate a buffer that size
    // associated with the receive packet...
    //

    ASSERT(pContext->pLocalSap->uMtuSize);
    if (pContext->pLocalSap->uMtuSize == 0)         // oops, should not happen.....
        FAIL;

    //
    // we only need a limited number or MTU sized receive packets
    // per listening context.
    //
    pContext->pPktPool =
        BlrudpIoPktPoolNew(&pBlrudp->pPktPool,              // ppSysPool
                           pContext->pLocalSap->pLocalNic,  // pLocalNic
                           0xFFFFFFFFL,                     // ulContextId
                           0,                               // ulNumXmtPkts
                           BLRUDP_NUM_LISTEN_PKTS,          // ulNumRcvPkts
                           (uint16_t)pContext->pLocalSap->uMtuSize);  // ulSizePkt

    if (pContext->pPktPool == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);
    //
    // Now we are ready to insert this into the passive context map.
    // we use the sap as the index and the context as the item.
    //
    nRet = BlMapInsert(pBlrudp->mapPassCtxs,     // pBlMap
                       &pContext->pLocalSap->Sap,        // pKey
                        pContext);                      // pItem
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);



    // all done, put it on the silly list.
    EnQ(&pContext->Hdr.link, &pBlrudp->listPassCtxs);

    *phContext = pContext;              // return the context.
    return(BLERR_SUCCESS);
Fail:
    if (pContext)
        BlrudpPassCtxDelete(pBlrudp,pContext);
    return(nErr);
}

////////////////////////////////////////////////////////////////////////////////
int BlrudpUnlisten(BLRUDP_HCONTEXT hContext)
//
// stop listening for incomming connections for the given listening context.
//
// inputs
//    hContext   -- hContext handle to listening context.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_PASSIVE_CONTEXT *pContext = (BLRUDP_PASSIVE_CONTEXT *)hContext;

    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    BlrudpPassCtxDelete(pContext->pBlrudp,
                               pContext);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

//////////////////////////////////////////////////////////////////////////////
int BlrudpAccept(BLRUDP_HCONTEXT hContext,
                 PFN_BLRUDP_CONTEXT_CHANGED pfnContextChanged,
                 PFN_BLRUDP_CONTEXT_RECV pfnContextRecv,
                 PFN_BLRUDP_SEND_BUFFER_DONE pfnSendBufferDone,
                 void *pUserData)
//
// Accept the incomming connection.
//
// inputs:
//   hContext -- handle to context that we are accepting.
//   pfnContextChanged -- pointer to context changed callback function.
//   pfnContextRecv -- pointer to context receive data callback function.
//   pfnSndBufferdone -- pointer to Send buffer done callback function.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    unsigned long ulNumPkts;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    pContext->bAccRejCalled = TRUE;         // remember we were here.

    // reject is only valid in these two states.
    if ((pContext->nState != BLRUDP_CTX_STATE_CLOSED) &&
        (pContext->nState != BLRUDP_CTX_STATE_ZOMBIE))
    {
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);
    }

    //
    // combine the remote options with the local options.
    //
    nRet = BlrudpCombineRemoteOptions(pContext);
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    //
    // Save off the call back pointers..
    //
    pContext->pfnContextChanged = pfnContextChanged;
    pContext->pfnContextRecv    = pfnContextRecv;
    pContext->pfnSendBufferDone = pfnSendBufferDone;
    pContext->pUserData         = pUserData;
    //
    // allocate based on the mtu size etc....
    //
    ulNumPkts = (pContext->Options.ulWindowSize+(pContext->Options.ulMtuSize-1)) /
        pContext->Options.ulMtuSize;

    //
    // initialize the balance of the context.
    nRet = BlrudpActCtxInit(pContext,         // pContext
                            ulNumPkts,        // ulNumXmtPkts
                            ulNumPkts,        // ulNumRcvPkts
                            TRUE);            // bAllocVect
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);



    //
    // Establish the initial send sequence number.
    //
    BlrudpSetInitialSeqNumbers(pContext);

    // set the transmit vector state...
    pContext->XmitPktVector.ulBaseSeq = pContext->XmitSeq.ulSseq;      // start here...
    pContext->XmitPktVector.ulEndSeq = pContext->XmitSeq.ulSseq;

    // set the receive vector state.  No packets in the vector...
    pContext->RecvPktVector.ulBaseSeq = pContext->RecvSeq.ulSseq;      // start here...
    pContext->RecvPktVector.ulEndSeq  = pContext->RecvSeq.ulSseq;



    // The remote send sequence + ulNumPkts..
    BlrudpSetRecvWindow(pContext,
                        ulNumPkts);

    BlrudpInitSreqState(pContext);

    //
    // This will cause a callback to the client...
    //
    BlrudpSetActiveState(pContext,                      // pContext
                         BLRUDP_CTX_STATE_FIRST_RECV,   // nState
                         BLERR_SUCCESS);                // nBlError



    //
    //
    //
    // we have work to do...
    //
    BlrudpStAddTctrlMsgToWork(pContext);

    return(BLERR_SUCCESS);
Fail:
    //
    // If the state never made it into the closed state
    // then dump it....
    if (pContext->nState == BLRUDP_CTX_STATE_CLOSED)
        BlrudpActCtxDelete(pContext->pBlrudp, pContext);

    return(nErr);
}

//////////////////////////////////////////////////////////////////////////////
int BlrudpReject(BLRUDP_HCONTEXT hContext UNUSED)
//
// Reject the incomming connection.
//
// inputs:
//   hContext -- handle to context that we are accepting.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    pContext->bAccRejCalled = TRUE;     // remember we were here.

    // reject is only valid in these two states.
    if ((pContext->nState != BLRUDP_CTX_STATE_CLOSED) &&
        (pContext->nState != BLRUDP_CTX_STATE_ZOMBIE))
    {
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);
    }

    BlrudpSelectMtuSize(pContext);      // must do the MTU size thing....
    //
    // perform the balance of context initialization
    // here with a minimum packet buffer
    nRet = BlrudpActCtxInit(pContext,                          // pContext
                            BLRUDP_NUM_CONNECT_PKTS,           // ulNumXmtPkts
                            BLRUDP_NUM_CONNECT_PKTS,           // ulNumRcvPkts
                            FALSE);                            // bAllocVect

    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);

    pContext->XmitSeq.ulAllocSeq = pContext->RecvSeq.ulRseq;     // can't do data transmissions

    //
    // place us into the zombie state, so we timeout even if we
    // fail on the error path here...
    //
    // Leave us in th zombie state for a while..
    // to process duplicate FIRST packets and other junk....
    //

    BlrudpSetZombieState(pContext,                  // pContext
                         BLERR_SUCCESS);            // nBlError


    // end flag makes this message a REJECT...
    pContext->usXmitFlags |= BLRUDP_CMD_END;
    // we have work to do...
    BlrudpStAddCtrlMsgToWork(pContext);



    return(BLERR_SUCCESS);
Fail:
    //
    // If the state never made it into the closed state
    // then dump it....
    if (pContext->nState == BLRUDP_CTX_STATE_CLOSED)
        BlrudpActCtxDelete(pContext->pBlrudp, pContext);

    return(nErr);
}


///////////////////////////////////////////////////////////////////////////////
void BlrudpActCtxInitTimers(BLRUDP_MGR *pBlrudp,
                                  BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Initialize the timers in an active context..
//
// inputs:
//    pBlrudp -- pointer to blrudp manager.
//    pContext -- context to initialize the timers for..
// outputs:
//    none.
//
{
    BLTIME_MGR *pTimeMgr;
    //
    // Initialize our connect timers.  This associates each
    // timer with their callback function and leaves the timer
    // inactive.
    //
    pTimeMgr = pBlrudp->pTimeMgr;
    BltwTimerInit(pTimeMgr,                       // pTimeMgr
                  &pContext->Timers.WTimer,         // pTimer
                  BlrudpWTimerCallback,             // pfnCallback
                  pContext);                        // user data.
    BltwTimerInit(pTimeMgr,                       // pTimeMgr
                  &pContext->Timers.CTimer,         // pTimer
                  BlrudpCTimerCallback,             // pfnCallback
                  pContext);                        // user data.
    BltwTimerInit(pTimeMgr,                       // pTimeMgr
                  &pContext->Timers.CTimeout,       // pTimer
                  BlrudpCTimeoutCallback,           // pfnCallback
                  pContext);                        // user data.
    BltwTimerInit(pTimeMgr,                       // pTimeMgr
                  &pContext->Timers.RTimer,         // pTimer
                  BlrudpRTimerCallback,             // pfnCallback
                  pContext);                        // user data.

}
///////////////////////////////////////////////////////////////////////////////
int BlrudpConnect(BLRUDP_HANDLE hBlrudp,
                  BLRUDP_HCONTEXT *phContext,
                  unsigned long ulSrcIpAddr,
                  unsigned short nSrcPort,
                  unsigned long ulDestIpAddr,
                  unsigned short nDestPort,
                  int nNumOptions,
                  BLRUDP_OPTIONS pOptions[],
                  PFN_BLRUDP_CONTEXT_CHANGED pfnContextChanged,
                  PFN_BLRUDP_CONTEXT_RECV pfnContextRecv,
                  PFN_BLRUDP_SEND_BUFFER_DONE pfnSendBufferDone,
                  void *pUserData)

//
// Request the Blrudp manager establish a connection with the remote SAP.
//
// inputs:
//    hBlrudp     -- instance to the instance to apply these options to.
//    phContext   -- pointer to where to stuff the new active context.
//    ulIpSrcAddr -- source IP address in network byte order.
//                 (same as returned by inet_addr()..)
//                 if zero, use the default port.
//    nSrcPort -- source port number to use, if zero use the default port.
//    ulIpDestAddr -- destination IP address in network byte order.
//                 (same as returned by inet_addr()..)
//                 if zero, use the default port.
//    nDestPort -- pointer to the port to use.
//    nNumOptions -- number of paramters that we wish to override the default values for.
//    pOptions -- pointer to list of parameters.
//    pfnContextChanged -- pointer to context changed callback function.
//    pfnContextRecv -- pointer to context receive data callback function.
//    pfnSndBufferdone -- pointer to Send buffer done callback function.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//     phContext -- newly created active context (NOTE, this won't
//                  be in the state needed to actually send data
//                  until BlrudpRun has been called a few times.
//
/////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_MGR *pBlrudp = (BLRUDP_MGR *)hBlrudp;
    BLRUDP_ACTIVE_CONTEXT *pContext = NULL;
    BLRUDP_ACTIVE_CONTEXT *pExistingContext = NULL;
    BLRUDP_LOCAL_SAP_ITEM *pLocalSap = NULL;
    BLRUDP_CONNECTION_SAPS SapPair;
    int nRet;

    ASSERT(hBlrudp && phContext);
    if (hBlrudp == NULL)
        FAIL;

    //
    // find the local sap
    nRet = BlrudpLocalSap(pBlrudp,
                          &pLocalSap,
                          ulSrcIpAddr,
                          nSrcPort);
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);

    ASSERT(pLocalSap);
    
    //
    // dynamic port assignment??
    //
    if (nSrcPort == 0)
        nSrcPort = pLocalSap->Sap.nPort;

    //
    // check first to see if we have a active context for this SAP pair.
    //
    SapPair.LocalSap.ulIpAddr   = ulSrcIpAddr;
    SapPair.LocalSap.nPort      = nSrcPort;
    SapPair.RemoteSap.ulIpAddr  = ulDestIpAddr;
    SapPair.RemoteSap.nPort     = nDestPort;

    //
    // Existing context is valid if the context is in the zombi state.
    pExistingContext = BlMapFindItem(pBlrudp->mapActCtxs, &SapPair);
    if (pExistingContext)
    {
        if (pExistingContext->nState != BLRUDP_CTX_STATE_ZOMBIE)
            FAILERR(nErr, BLERR_BLRUDP_DUP_SAP);
        //
        // We have an existing context, clean out the stuff
        // that has to be re-initialized.
        //
        BlrudpActCtxClear(pExistingContext);
        pContext = pExistingContext;        // use this context....
    }
    else
        pContext = BlrudpActCtxNew(pBlrudp);


    if (pContext == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);

    pContext->pfnContextChanged = pfnContextChanged;
    pContext->pfnContextRecv    = pfnContextRecv;
    pContext->pfnSendBufferDone = pfnSendBufferDone;
    pContext->pUserData         = pUserData;

    pContext->SapPair = SapPair;
    pContext->pLocalSap = pLocalSap;        // save this
    pLocalSap = NULL;           // the context owns it now...


    nRet = BlrudpProcessOptions(&pContext->Options,             // *pOptions,
                                nNumOptions,                    // nNumOptions,
                                pOptions);                      // pOptionList[])
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    
    

    //
    // Now we have the local sap, and therefore the MTU size..
    // Double check that we have it and allocate a buffer that size
    // associated with the receive packet...
    //

    ASSERT(pContext->pLocalSap->uMtuSize);
    if (pContext->pLocalSap->uMtuSize == 0)         // oops, should not happen.....
        FAIL;


    BlrudpSelectMtuSize(pContext);
    //
    // pickup the initial wtimer value

    BlrudpInitSreqState(pContext);

    nRet = BlrudpActCtxInit(pContext,                       // pContext
                            BLRUDP_NUM_CONNECT_PKTS,        // ulNumXmtPkts
                            BLRUDP_NUM_CONNECT_PKTS,        // ulNumRcvPkts
                            FALSE);                         // bAllocVect

    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);



    // pick initial sequence numbers here...
    BlrudpSetInitialSeqNumbers(pContext);
    pContext->XmitSeq.ulAllocSeq = 0;                   // actually not a valid alloc yet...


    //
    // Set us to first sent state.  Last thing we do,
    // we cannot have a FAIL after this and correctly recover...
    //
    BlrudpSetActiveState(pContext,
                         BLRUDP_CTX_STATE_FIRST_SENT,
                         BLERR_SUCCESS);

    //
    // Mow prime the pump to get the active context
    // sending its first message.  The work manager will call
    // us with work to do.  The only thing to do in a First sent
    // state is send or resend the first packet...
    //
    BlrudpStAddFirstMsgToWork(pContext);

    *phContext = pContext;      // return the handle
    return(BLERR_SUCCESS);
Fail:
    if (pLocalSap)                                      // dangling local sap
        BlrudpLocalSapRelease(pBlrudp, pLocalSap);      // ditch it.
        
    if ((pContext) && (pExistingContext == NULL))
        BlrudpActCtxDelete(pBlrudp,pContext);
    return(nErr);
}

//////////////////////////////////////////////////////////////////////
int BlrudpDisconnect(BLRUDP_HCONTEXT hContext,
                     int bImmediate)
//
// disconnect from a connected context.
//
// inputs:
//    hContext -- Context to disconnect from.
//    bImmediate -- disconnect immediatly....
// outputs:
//     returns -- BLERR_SUCCESS if successful.
////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;

    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;


    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    if (bImmediate)
    {
        //
        // proceed immediatly to zombie with wtimer = 0.
        // zombie wtimer == send an end bit...  oneshot....
        BlrudpSetZombieState(pContext,                  // pContext
                             BLERR_SUCCESS);            // nBlError


        // end flag kills the connection....
        pContext->usXmitFlags |= BLRUDP_CMD_END;
        // we have work to do...
        BlrudpStAddCtrlMsgToWork(pContext);

    }
    else
    {
        // Graceful close, goes directly into the closing state...

        BlrudpSetActiveState(pContext,
                             BLRUDP_CTX_STATE_CLOSING,
                             BLERR_SUCCESS);

        //
        // schedule xmit work to do for closing...
        //
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }
    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

////////////////////////////////////////////////////////////////////////////////
int BlrudpGetContextState(BLRUDP_HCONTEXT hContext UNUSED)
//
// Return the current state of the context.
//
// inptus:
//    hContext -- context we are inetersted in.
// outputs:
//    returns -- the current state of the context.
//
///////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;
    if (pContext == NULL)
        FAIL;


    return(BlrudpUserState(pContext->nState));
Fail:
    return(BLRUDP_STATE_CLOSED);
}

///////////////////////////////////////////////////////////////////////////
unsigned long BlrudpGetContextId(BLRUDP_HCONTEXT hContext)
//
// return an internal context identifier.  This is an identifier
// which increments monotonically each time a new context is created.
// It is used to help identify each context for debugging purposeds.
//
// inputs:
//    hContex -- the context handle we are interested in.
// outputs:
//    returns -- the context Identifier.
//
/////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;
    if (pContext == NULL)
        FAIL;


    return(pContext->ulContextId);
Fail:
    return(0);
}
////////////////////////////////////////////////////////////////
int BlrudpGetContextAddr(BLRUDP_HCONTEXT hContext,
                         unsigned long *pulSrcAddr,
                         unsigned short *pnSrcPort,
                         unsigned long *pulDestAddr,
                         unsigned short *pnDestPort)
//
// Retrieve the address and port number assocaited with this
// device.
//
// The context passed can be either an active or passive context.
// If a passive context is passed in the pulDestAddr and pulDestPort will
// be returned as zero...
//
//
// The address and port number are returned in the native byte order.
//
// inputs:
//    hContext -- context containing the address to return.
//    pulSrcAddr -- pointer to where to stuff the source adderss.
//    pnSrcPort -- pointer to where to stuff the source port number.
//    pulDestAddr -- pointer to where to stuff the destination adderss.
//    pnDestPort -- pointer to where to stuff the destination port number.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//     *pulSrcAddr -- Address of port in native byte order.
//     *pnSrcPort -- Port number.
//     *pulDestAddr -- Address of port in native byte order.
//     *pnDestPort -- Port number.
//
///////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_CONTEXT *pContext = (BLRUDP_CONTEXT *)hContext;
    BLRUDP_ACTIVE_CONTEXT *pActiveCtx;
    BLRUDP_PASSIVE_CONTEXT *pPassiveCtx;

    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // ok, what flavor.
    //
    if (pContext->nContextType == BLRUDP_CTYPE_ACTIVE)
    {
        pActiveCtx = (BLRUDP_ACTIVE_CONTEXT *)pContext;
        if (pulSrcAddr)
            *pulSrcAddr = pActiveCtx->SapPair.LocalSap.ulIpAddr;
        if (pnSrcPort)
            *pnSrcPort = pActiveCtx->SapPair.LocalSap.nPort;
        if (pulDestAddr)
            *pulDestAddr = pActiveCtx->SapPair.RemoteSap.ulIpAddr;
        if (pnDestPort)
            *pnDestPort = pActiveCtx->SapPair.RemoteSap.nPort;
    }
    else
    if (pContext->nContextType == BLRUDP_CTYPE_PASSIVE)
    {
        pPassiveCtx = (BLRUDP_PASSIVE_CONTEXT *)pContext;
        if (pulSrcAddr)
            *pulSrcAddr = pPassiveCtx->pLocalSap->Sap.ulIpAddr;
        if (pnSrcPort)
            *pnSrcPort = pPassiveCtx->pLocalSap->Sap.nPort;
        if (pulDestAddr)
            *pulDestAddr = 0;
        if (pnDestPort)
            *pnDestPort = 0;
    
    }
    else
        FAILERR(nErr, BLERR_INVALID_HANDLE);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}



///////////////////////////////////////////////////////////////////////
int BlrudpGetStatus(BLRUDP_HCONTEXT hContext UNUSED,
                    int  *pnState UNUSED,
                    int  *pnRecvCount UNUSED,
                    int  *pnSendCount UNUSED)
//
// Retrieve status about a given connection context.
//
// inputs:
//    hContext -- Context to disconnect from.
//    pnState -- pointer of where to stuff the state of the context.
//    pnRecvCount -- pointer of where to stuff the number of receive buffers
//                    waiting to be received.
//    pnSendCount -- pointer of where to stuff the number of send buffers
//                    available for sending data.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//     *pnState -- the state of the context.
//     *pnRecvCount -- the number of receive buffers
//                    waiting to be received.
//     *pnSendCount -- the number of send buffers
//                    available for sending data.
//
////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;
    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    if (pnState)
        *pnState = BlrudpUserState(pContext->nState);
    if (pnRecvCount)
        *pnRecvCount = BlrudpGetRecvPktCount(pContext);
    if (pnSendCount)
    {
        uint32_t ulCount;
        //
        // We take the minimum of the total number of send buffers available
        // for this context and the amount of space in the send vector
        //
        uint32_t ulSendVectSpace = BlrudpPktVectorAvail(&pContext->XmitPktVector);
        ulCount = BlrudpIoPktGetSendPktCount(pContext->pPktPool) +
                       pContext->ulClientSendPktCount;

        // these should be equal, unless we are not activly connected
        ASSERT((ulSendVectSpace == ulCount) || (pContext->nState < BLRUDP_CTX_STATE_ESTABLISHED));
        if (ulSendVectSpace < ulCount)
            ulCount = ulSendVectSpace;
        *pnSendCount = ulCount;
    }

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

//////////////////////////////////////////////////////////////////////////////
int BlrudpReqDiagInfo(BLRUDP_HCONTEXT hContext UNUSED)
//
// Request diagnostic information from the remote context.
//
// This causes a low priority diagnostic information message to be
// sent to the remote end.  The client will be notified of the return
// message via callback.
//
// inputs:
//    hContext -- Context to disconnect from.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);
    if (pContext->nState != BLRUDP_CTX_STATE_ESTABLISHED)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);

    pContext->ulWorkFlags |= BLRUDP_WORK_SENDDIAGREQ;          // schecdule the work...
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

//////////////////////////////////////////////////////////////////////////////
int BlrudpGetDiagInfo(BLRUDP_HCONTEXT hContext,
                      int bRemote,
                      BLRUDP_DIAG *pDiag)
//
// Retrieve the current diagnostic information.
//
// this retrieves the local diagnostic information or the
// last received remote diagnostic information from the remote host.
//
// inputs:
//    hContext -- Context to disconnect from.
//    bRemote -- if TRUE, we are requesting the remote context information.
//               if FALSE, we are requesting the local context information.
//    pDiag -- poniter to where to stuff the diagnostic information.
// outputs:
//    returns - BLERR_SUCCESS if successful.
//    *pDiag -- diagnostic information...
//
////////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;
    BLRUDP_DIAG_INFO *pCtxDiagInfo;

    if ((pContext == NULL) || (pDiag == NULL))
        FAILERR(nErr, BLERR_PARAM_ERR);

    if (pContext->nState == BLRUDP_CTX_STATE_ZOMBIE)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);

    /////////////////////////////////////////////////////////////////
    // What flavor diag to we want...
    //
    if (bRemote)
        pCtxDiagInfo = &pContext->RemoteDiag;
    else
        pCtxDiagInfo = &pContext->LocalDiag;

    pDiag->llNumRetrans = pCtxDiagInfo->llNumRetrans;
    pDiag->llDataXmit = pCtxDiagInfo->llDataXmit;
    pDiag->llDataRecv = pCtxDiagInfo->llDataRecv;

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}


//////////////////////////////////////////////////////////////////////////////
int BlrudpRecvFromContext(BLRUDP_HCONTEXT hContext,
                          unsigned char *pData ,
                          int nBufferLen ,
                          int *pnDataLen )
//
// Receive data from a specific context copying the data into the
// buffer provided by the client.
//
// Call this with a NULL pData pointer and a pnDataLen and
// this will return the length of the data in the buffer.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      pData      -- pointer to where to stuff the data.
//      nBufferLen -- maximum length of the buffer, should the MTU size.
//      pnDataLen -- pointer of where to stuff the actual data packet length.
//
// outputs:
//      returns BLERR_SUCCESS if successful.
//              BLERR_BLRUDP_NO_RECVDATA when there is no more data to receive.
//      *pData -- data filled out.
//      *pnDataLen -- length of the received data packet.
/////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt;
    uint16_t usDataLen;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    ASSERT(pContext && pnDataLen);

    if ((pContext == NULL) || (pnDataLen == NULL))
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // make sure we have the correct flavor of context...
    //
    if (pContext->Hdr.nContextType != BLRUDP_CTYPE_ACTIVE)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_CONTEXT);

    if (pData)
    {
        pPkt = BlrudpGetRecvPkt(pContext);
        if (pPkt == NULL)
            FAILERR(nErr, BLERR_BLRUDP_NO_RECVDATA);

        usDataLen  = BlrudpGetDataPayloadLen(pPkt);
        if (usDataLen > nBufferLen)     // do we have enough room
            FAILERR(nErr, BLERR_BLRUDP_DATA_TOOLONG);   // no,, punt....

        ASSERT(usDataLen > 0);       // huh?? somebody sending us zero length packets.???

        if (usDataLen > 0)      // keep us from doing something really stupid....
        {
            memcpy(pData,
                   BlrudpGetDataPayload(pPkt),
                   usDataLen);
        }
        //
        // Mark the packet as delivered...
        //
        pPkt->ulFlags |= BLRUDP_PKT_STATUS_DELIVERED;
        //
        //
        BlrudpUpdateReceiverRSeq(pContext);
    }
    else
    {
        pPkt = BlrudpPeekRecvPkt(pContext);
        if (pPkt == NULL)
            FAILERR(nErr, BLERR_BLRUDP_NO_RECVDATA);
        usDataLen  = BlrudpGetDataPayloadLen(pPkt);
    }
    *pnDataLen = usDataLen;

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}


///////////////////////////////////////////////////////////////////////////////
int BlrudpRecvBufferFromContext(BLRUDP_HCONTEXT hContext UNUSED,
                                BLRUDP_HIOBUFFER *phIoBuffer,
                                unsigned char **ppData UNUSED,
                                int *pnDataLen UNUSED)
//
// Receive an entire receive buffer from any context.
//
// This allows the caller to receive the data without having to Blrudp
// manager do a copy operation.  It meerely returns the buffer.  When
// doing this kind of receive, the caller MUST release the buffer when
// it is done with it.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      phIoBuffer -- pointer to where to stuff the io buffer handle
//      *pData      -- pointer to where to stuff the buffer pointer.
//      pnDataLen -- pointer of where to stuff the actual data packet length.
//
// outputs:
//      returns BLERR_SUCCESS if successful.
//      *phIoBuffer -- io buffer handle.
//      **pData -- pointer ot data buffer.
//      *pnDataLen -- length of the received data packet.
/////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    ASSERT(pContext && pnDataLen);

    if ((pContext == NULL) || (phIoBuffer == NULL))
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // make sure we have the correct flavor of context...
    //
    if (pContext->Hdr.nContextType != BLRUDP_CTYPE_ACTIVE)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_CONTEXT);

    pPkt = BlrudpGetRecvPkt(pContext);
    if (pPkt == NULL)
        FAILERR(nErr, BLERR_BLRUDP_NO_RECVDATA);

    pPkt->pContext = &pContext->Hdr;        // make sure we remember our context...

    ASSERT(pPkt->link.next == NULL);
    EnQ(&pPkt->link, &pContext->listClientRecvPkts);
    pContext->ulClientSendPktCount++;

    *phIoBuffer = pPkt;     // return the handle...

    if (ppData)
        *ppData = BlrudpGetDataPayload(pPkt);
    if (pnDataLen)
        *pnDataLen = BlrudpGetDataPayloadLen(pPkt);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

///////////////////////////////////////////////////////////////////////////
int BlrudpReleaseRecvBuffer(BLRUDP_HIOBUFFER hIoBuffer)
//
// Release a buffer received with either BlrudpRecvBufferFromContext or
// BlrudpRecvBufferFromAny.
//
// After this call the Blrudp manager is free to re-use this buffer.  This
// is the signal that is used to signal the other end that the program
// is done using the buffer.
//
// inputs:
//    hIoBuffer -- buffer handle.
// outputs:
//      returns BLERR_SUCCESS if successful.
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_ACTIVE_CONTEXT *pContext;
    BLRUDP_IO_PKT *pPkt = (BLRUDP_IO_PKT *)hIoBuffer;

    ASSERT(pPkt);

    if (pPkt == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    pContext = (BLRUDP_ACTIVE_CONTEXT *)pPkt->pContext;
    ASSERT(pContext);
    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // make sure we have the correct flavor of context...
    //
    if (pContext->Hdr.nContextType != BLRUDP_CTYPE_ACTIVE)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_CONTEXT);

    //
    // Mark the packet as delivered...
    //
    pPkt->ulFlags |= BLRUDP_PKT_STATUS_DELIVERED;


    ASSERT(pPkt->link.next);                // remove it from the client list...
    DeQ(&pPkt->link);
    ASSERT(pContext->ulClientSendPktCount);
    if (pContext->ulClientSendPktCount > 0)
        pContext->ulClientSendPktCount--;

    //
    // update the sequence numbers to account for receiving this...
    //
    BlrudpUpdateReceiverRSeq(pContext);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

////////////////////////////////////////////////////////////////////////
int BlrudpSendToContext(BLRUDP_HCONTEXT hContext,
                        const unsigned char *pData,
                        unsigned int nLen,
                        unsigned long ulSendBufferID)

//
//
// Send data to a connected context.  This actually copies the data
// to a free send buffer.  Note: there has to be a free send buffer to
// copy the data to for this to succeed.  Buffers may be exausted because
// the receiving end has not acknowledged the date in the send pipe.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      *pData      -- pointer to where to stuff the buffer pointer.
//      nLen -- lenght of the data to send.
//      ulSendBufferID -- identification used to identify this
//                        send buffer when it has been transmmitted
//                        and acknowleged.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = NULL;
    BLRUDP_IO_DATA_PKT *pDataPkt;

    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    ASSERT(pData && (nLen > 0));

    if ((pContext == NULL) || (pData == NULL) || (nLen <= 0))
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // procedure for sending packets...
    //
    // 1.  Check state.
    // 2.  Check the vector if there is room.
    // 3.  Get a free packet.
    // 4.  Copy the packet payload into the free packet.
    // 5.  Push it onto the vector.
    // 6.  Schedule work...

    if (pContext->nState != BLRUDP_CTX_STATE_ESTABLISHED)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);


    if (! BlrudpPktVectorAvail(&pContext->XmitPktVector))
        FAILERR(nErr, BLERR_BLRUDP_NO_SEND_BUFFERS);

    pPkt = BlrudpIoPktGetSendPkt(pContext->pPktPool);
    ASSERT(pPkt);
    if (pPkt == NULL)
        FAILERR(nErr, BLERR_BLRUDP_NO_SEND_BUFFERS);

    //
    // make sure we can copy all of this data...
    //
    if (nLen > BlrudpGetMaxDataPayloadLen(pPkt))
        FAILERR(nErr, BLERR_BLRUDP_DATA_TOOLONG);

    pDataPkt = (BLRUDP_IO_DATA_PKT *)BlrudpGetPktPayload(pPkt);
    memcpy(pDataPkt->Payload, pData, nLen);

    //
    // save off some packet header information....
    //
    pPkt->pContext = &pContext->Hdr;
    pPkt->usPktLen = (nLen + offsetof(BLRUDP_IO_DATA_PKT, Payload));
    pPkt->ulSendBufferID = ulSendBufferID;

    //
    // Packet is not encoded at this point....
    // only the data portion is copied...
    //

    // push this onto the transmit packet vector.
    BlrudpPktVectorsPush(pContext,                   //pContext,
                         &pContext->XmitPktVector,   //pVector,
                         pPkt);                      //pPkt)

    //
    // make sure we are on the work queue....
    //
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);

    return(BLERR_SUCCESS);
Fail:
    if (pPkt)
        BlrudpIoPktPutSendPkt(pContext->pPktPool, pPkt);
    return(nErr);
}


////////////////////////////////////////////////////////////////////////
int BlrudpBufferSendToContext(BLRUDP_HCONTEXT hContext,
                               BLRUDP_HIOBUFFER hIoBuffer,
                               unsigned int nLen,
                               unsigned long ulSendBufferID)

//
//
// Send a Data buffer to a connected context.  The Blrudp manager
// takes responsibility for this buffer and when done it returns it
// to the free send buffer pool.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      hIoBuffer -- handle of IO buffer to send...
//      nLen -- lenght of the data to send.
//      ulSendBufferID -- identification used to identify this
//                        send buffer when it has been transmmitted
//                        and acknowleged.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = (BLRUDP_IO_PKT *)hIoBuffer;
    BLRUDP_IO_DATA_PKT *pDataPkt;

    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    ASSERT(pPkt && (nLen > 0));

    if ((pContext == NULL) || (pPkt == NULL) || (nLen <= 0))
        FAILERR(nErr, BLERR_PARAM_ERR);

    //
    // this should be a packet that was obtained from this context...
    //
    //
    ASSERT(&pContext->Hdr == pPkt->pContext);

    if (&pContext->Hdr != pPkt->pContext)
        FAILERR(nErr, BLERR_PARAM_ERR);
    //
    // procedure for sending packets...
    //
    // 1.  Check state.
    // 2.  Check the vector if there is room.
    // 3.  Push packet onto the vector.
    // 4.  Schedule work...

    if (pContext->nState != BLRUDP_CTX_STATE_ESTABLISHED)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);


    if (! BlrudpPktVectorAvail(&pContext->XmitPktVector))
        FAILERR(nErr, BLERR_BLRUDP_NO_SEND_BUFFERS);

    pDataPkt = (BLRUDP_IO_DATA_PKT *)BlrudpGetPktPayload(pPkt);

    //
    // save off some packet header information....
    //
    pPkt->pContext = &pContext->Hdr;
    pPkt->usPktLen = (nLen + offsetof(BLRUDP_IO_DATA_PKT, Payload));
    pPkt->ulSendBufferID = ulSendBufferID;
    //
    // Packet is not encoded at this point....
    //

    //
    // Pull it off the client queue...
    //
    ASSERT(pPkt->link.next);
    if (pPkt->link.next)
    {
        DeQ(&pPkt->link);       // remove it from the client send pkt list.
        ASSERT(pContext->ulClientSendPktCount);
        if (pContext->ulClientSendPktCount)
            pContext->ulClientSendPktCount--;
    }
    // push this onto the transmit packet vector.
    BlrudpPktVectorsPush(pContext,                   //pContext,
                         &pContext->XmitPktVector,   //pVector,
                         pPkt);                      //pPkt)

    //
    // make sure we are on the work queue....
    //
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

///////////////////////////////////////////////////////////////////////
int BlrudpGetSendBuffer(BLRUDP_HCONTEXT hContext,
                        BLRUDP_HIOBUFFER *phIoBuffer,
                        unsigned char **ppDataBuffer,
                        unsigned long *pulBufferLen)


//
// Request a send buffer from the free buffer pool associated with this context.
// Each buffer will be MTU size large (see BlrudpGetMaxPayloadLen()... call).
// Buffers can also be queried for their data size and actuall size.
//
// These buffers remain under the control of the context.  They are
// automatically freed when the context closes...  Therefore, when a given
// context goes to the closed state, any send buffers the client has
// become invalid...
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      *ppDataBuffer -- to where to stuff the data buffer pointer.
//      pnBufferLen -- pointer to the data buffer length.
//      nLen -- lenght of the data to send.
//
// outputs:
//      returns BLERR_SUCCESS if successful.
//      *hIOBuffer io buffer handle
//      *ppDataBuffer -- data buffer.
//      *pnBufferLen -- buffer length.
//
//////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = NULL;
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;

    if ((pContext == NULL) || (phIoBuffer == NULL))
        FAILERR(nErr, BLERR_PARAM_ERR);

    if (pContext->nState != BLRUDP_CTX_STATE_ESTABLISHED)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);

    pPkt = BlrudpIoPktGetSendPkt(pContext->pPktPool);
    ASSERT(pPkt);
    if (pPkt == NULL)
        FAILERR(nErr, BLERR_BLRUDP_NO_SEND_BUFFERS);

    // put it on the send pkts list....
    EnQ(&pPkt->link, &pContext->listClientSendPkts);
    pContext->ulClientSendPktCount++;

    pPkt->pContext = &pContext->Hdr;

    *phIoBuffer = pPkt;

    if (ppDataBuffer)
        *ppDataBuffer = BlrudpGetDataPayload(pPkt);
    if (pulBufferLen)
        *pulBufferLen = BlrudpGetMaxDataPayloadLen(pPkt);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}


/////////////////////////////////////////////////////////////////////////////
int BlrudpReleaseSendBuffer(BLRUDP_HIOBUFFER hIoBuffer)
//
// Release a send buffer that was obtained by BlrudpGetSendbuffer.
//
// The client calls this when it wishes to give the buffer back to the context
// without sending data..
//
// inputs:
//    hIoBuffer -- buffer handle.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
//////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_ACTIVE_CONTEXT *pContext = NULL;
    BLRUDP_IO_PKT *pPkt = (BLRUDP_IO_PKT *)hIoBuffer;

    if (pPkt == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    pContext = (BLRUDP_ACTIVE_CONTEXT *)pPkt->pContext;

    if (pContext->nState != BLRUDP_CTX_STATE_ESTABLISHED)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);

    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    if (pContext->nState != BLRUDP_CTX_STATE_ESTABLISHED)
        FAILERR(nErr, BLERR_BLRUDP_INVALID_STATE);

    ASSERT(pPkt->link.next);
    if (pPkt->link.next)
    {
        DeQ(&pPkt->link);       // remove it from the client send pkt list.
        ASSERT(pContext->ulClientSendPktCount);
        if (pContext->ulClientSendPktCount)
            pContext->ulClientSendPktCount--;
    }

    BlrudpIoPktPutSendPkt(pContext->pPktPool, pPkt);    // put it back in the pool....

    return(BLERR_SUCCESS);
Fail:
    if (pPkt)
        BlrudpIoPktPutSendPkt(pContext->pPktPool, pPkt);
    return(nErr);

}

//////////////////////////////////////////////////////////////////////////
int BlrudpGetMaxPayloadLen(BLRUDP_HCONTEXT hContext,
                     unsigned long *pulMaxPayloadLen)
//
// Retrieve the maxium payload  size assocaied with a given context.
//
// inputs:
//    hContext -- context to retrieve the max payload length
//    pulMTUSize -- pointer of where to stuff the max payload length
// outputs:
//      returns BLERR_SUCCESS if successful.
//    *pulMTUSize -- the MTU size.
/////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;

    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)hContext;
    if (pContext == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);


    *pulMaxPayloadLen = pContext->Options.ulMtuSize -
                            offsetof(BLRUDP_IO_DATA_PKT, Payload) -
                            BLRUDP_PKT_UDP_HDR_LEN;

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}


/////////////////////////////////////////////////////////////////////////
int BlrudpGetMaxBufferSize(BLRUDP_HIOBUFFER hIoBuffer,
                           unsigned long *pulBufferSize )
//
// Return the maximum size of this data buffer.
//
// inputs:
//    hIOBuffer -- io buffer handle
//    pulBufferSize -- pointer where to stuff the buffer size.
// outputs:
//     returns BLERR_SUCCESS if successful.
//    *pulBufferSize  -- The maximum buffer size.
/////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = (BLRUDP_IO_PKT *)hIoBuffer;

    ASSERT(pPkt);
    if (pPkt == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    *pulBufferSize = BlrudpGetMaxDataPayloadLen(pPkt);
    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

/////////////////////////////////////////////////////////////////////////
int BlrudpGetDataSize(BLRUDP_HIOBUFFER hIoBuffer,
                      unsigned long *pulDataSize )
//
// Return the size of the data contained in this buffer.
//
// inputs:
//    hIOBuffer -- io buffer handle
//    pulDataSize -- pointer where to stuff the buffer size.
// outputs:
//     returns BLERR_SUCCESS if successful.
//    *pulDataSize  -- The size of the data buffer.
/////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = (BLRUDP_IO_PKT *)hIoBuffer;

    ASSERT(pPkt);
    if (pPkt == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    *pulDataSize  = BlrudpGetDataPayloadLen(pPkt);
    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

/////////////////////////////////////////////////////////////////////////
int BlrudpGetDataPointer(BLRUDP_HIOBUFFER hIoBuffer ,
                         unsigned char **ppDataBuffer)
//
// return the pointer to the data buffer assicagted with this IO buffer.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    hIOBuffer -- io buffer handle
//    pulDataSize -- pointer where to stuff the buffer size.
// outputs:
//     returns BLERR_SUCCESS if successful.
//    *pulDataSize  -- The size of the data buffer.
/////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = (BLRUDP_IO_PKT *)hIoBuffer;

    ASSERT(pPkt);
    if (pPkt == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    *ppDataBuffer = BlrudpGetDataPayload(pPkt);
    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}


/////////////////////////////////////////////////////////////
int BlrudpInitializeSockets()
// Perform socket initialization
//
// This handles any system specific sock system initialization.
// for example, in windows this calls WSAStartup
//
// inptus:
//    none
// outputs:
//    none.
{

    int nRet;
    if (g_nSocketsInitialized == 0)
    {
        #if defined(WIN32) || defined(CYGWIN)
            WSADATA wData;
            nRet = WSAStartup(0x11,     // wVersion requested (1.1) minor | major
                              &wData);
        #else
            nRet = BLERR_SUCCESS;       // no need to do this on non windows systems
        #endif
            if (nRet != 0)
                FAIL;
    }
    g_nSocketsInitialized++;
    return(BLERR_SUCCESS);
Fail:
    return(BLERR_JTAG_SOCKET_FAIL);
}

////////////////////////////////////////////////////////////
void BlrudpCleanupSockets()
// Perform socket cleanup
//
// This handles any system specific sock system cleanup.
// for example, in windows this calls WSAStartup
//
// inputs:
//   none
//
{
    ASSERT(g_nSocketsInitialized > 0);      // somebody called us too many times.
    if (g_nSocketsInitialized == 0) // already done, so but out.
        return;

    g_nSocketsInitialized--;
    if (g_nSocketsInitialized > 0)      // nothing to do yet.
        return;
#if defined(WIN32) || defined(CYGWIN)
    WSACleanup();       // clean up what we started.
#endif

}

/////////////////////////////////////////////////////////////
BLRUDP_PASSIVE_CONTEXT *BlrudpPassCtxNew(BLRUDP_MGR *pBlrudp)
//
// Create and initialize a new passive context structure.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
// outputs:
//    returns -- pointer to new passive context structure
//               or NULL if out of memory.
//
{
    BLRUDP_PASSIVE_CONTEXT *pContext;
    pContext = BlrAllocMem(sizeof(*pContext));
    if (pContext == NULL)
        FAIL;


    memset(pContext, 0, sizeof(*pContext));
    pContext->Hdr.nContextType = BLRUDP_CTYPE_PASSIVE;
    pContext->pBlrudp = pBlrudp;

    memcpy(&pContext->Options,                  // dest
           &pBlrudp->DefOptions,                // src
           sizeof(pContext->Options));          // len


    return(pContext);
Fail:
    return(NULL);
}
////////////////////////////////////////////////////////////
void BlrudpPassCtxDelete(BLRUDP_MGR *pBlrudp,
                                BLRUDP_PASSIVE_CONTEXT *pContext)
//
// delete and shutdown a passive context.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pContext    -- context to delete and shut down.
// outputs:
//    none.
//
{
    if (pContext->Hdr.link.next)
        DeQ(&pContext->Hdr.link);

    //
    // packet pool for incomming conections.
    //
    if (pContext->pPktPool)
        BlrudpIoPktPoolRelease(pContext->pPktPool);

    if (pContext->pLocalSap)
    {
        // remove this context from the map of passive contexts.
        BlMapRemove(pBlrudp->mapPassCtxs,    // pBlMap
                    &pContext->pLocalSap->Sap);      // pKey
        BlrudpLocalSapRelease(pBlrudp, pContext->pLocalSap);
    }


    BlrFreeMem(pContext);
}

/////////////////////////////////////////////////////////////
BLRUDP_ACTIVE_CONTEXT *BlrudpActCtxNew(BLRUDP_MGR *pBlrudp)
//
// Create and initialize a new active context structure.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
// outputs:
//    returns -- pointer to new passive context structure
//               or NULL if out of memory.
//
{
    BLRUDP_ACTIVE_CONTEXT *pContext;
    pContext = BlrAllocMem(sizeof(*pContext));
    if (pContext == NULL)
        FAIL;

    memset(pContext, 0, sizeof(*pContext));

    pContext->ulContextId = g_ulContextId++;

    pContext->nRetryCount = 0;
    pContext->Hdr.nContextType = BLRUDP_CTYPE_ACTIVE;
    pContext->pBlrudp = pBlrudp;

    pContext->RemoteOptions.bChecksum = -1;         // no value sent

    memcpy(&pContext->Options,                  // dest
           &pBlrudp->DefOptions,                // src
           sizeof(pContext->Options));          // len

    QInit(&pContext->listRexmitPkts);       // retransmit list....
    QInit(&pContext->listRecvPkts);         // receive list...
    pContext->ulRecvPktCount = 0;

    QInit(&pContext->listClientSendPkts);
    pContext->ulClientSendPktCount = 0;

    QInit(&pContext->listClientRecvPkts);
    pContext->ulClientRecvPktCount = 0;

    //
    // initailze our work items.
    //
    BlrWorkItemInit(&pContext->RecvWorkItem,
                   BlrudpRecvWorkCallback,
                   pContext);

    BlrWorkItemInit(&pContext->XmitDoneWorkItem,
                   BlrudpXmitDoneWorkCallback,
                   pContext);

    BlrWorkItemInit(&pContext->XmitWorkItem,
                   BlrudpXmitWorkCallback,
                   pContext);

    //
    // This is probably common between accept and reject...
    //
    BlrudpActCtxInitTimers(pBlrudp,
                           pContext);

    //
    // this should result in something that does not repeat itself
    // each time we run...
    //
    pContext->XmitSeq.ulSseq = (uint32_t)BltwTimerRawTime();

    return(pContext);
Fail:
    return(NULL);
}
////////////////////////////////////////////////////////////
void BlrudpActCtxDelete(BLRUDP_MGR *pBlrudp,
                               BLRUDP_ACTIVE_CONTEXT *pContext)
//
// delete and shutdown a passive context.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pContext    -- context to delete and shut down.
// outputs:
//    none.
//
{
    if (pContext->Hdr.link.next)
        DeQ(&pContext->Hdr.link);

    BlrudpActCtxClear(pContext);

    // if we have a local sap, release it.
    if (pContext->pLocalSap)
        BlrudpLocalSapRelease(pBlrudp, pContext->pLocalSap);

    BlMapRemove(pBlrudp->mapActCtxs,     // pBlMap
                &pContext->SapPair);            // pKey

    BlrFreeMem(pContext);
}




//
//
/////////////////////////////////////////////////////////////////
void BlrudpActCtxClear(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// clear the active context so it can be re-used or dumped.
//
// inputs:
//    pContext -- context to dump pool from.
// outputs:
//    none.
//
{
    BlrudpActCtxClearTimers(pContext);

    //
    // move to dump the packet pool. the new one may
    // have different allocation requirements.
    BlrudpActCtxDumpPktPool(pContext);

    //
    // dump any vectors we
    // may have
    BlrudpPktVectorsFree(pContext);


}






////////////////////////////////////////////////////////////////////////
int BlrudpCompareSaps(void *pKey1, void *pKey2)
//
// function to compare saps for Blrudp map function.
//
// inputs:
//    pKey1, pKey2 -- pointer to BLRUDP_SAP_ADDR to compare
// outputs:
//    none.
//
{
    BLRUDP_SAP_ADDR *pSap1 = pKey1;
    BLRUDP_SAP_ADDR *pSap2 = pKey2;
    int64_t ullVal1, ullVal2;           // try to do this efficiently.
    int64_t ullRet;

    ullVal1 = ((pSap1->ulIpAddr << 16) | pSap1->nPort);
    ullVal2 = ((pSap2->ulIpAddr << 16) | pSap2->nPort);


    ullRet = ullVal1 - ullVal2;

    if (ullRet > 0)
        return(1);
    if (ullRet < 0)
        return(-1);
    return(0);

}

////////////////////////////////////////////////////////////////////////
int BlrudpCompareConnectSaps(void *pKey1, void *pKey2)
//
// function to compare connection saps for Blrudp map function.
//
// inputs:
//    pKey1, pKey2 -- pointer to BLRUDP_CONNECTION_SAPS to compare
// outputs:
//    none.
//
{
    BLRUDP_CONNECTION_SAPS *pSap1 = pKey1;
    BLRUDP_CONNECTION_SAPS *pSap2 = pKey2;

    int64_t ullVal1, ullVal2;           // try to do this efficiently.
    int64_t ullRet;

    ullVal1 = (((uint64_t)pSap1->RemoteSap.ulIpAddr << 16) | pSap1->RemoteSap.nPort);
    ullVal2 = (((uint64_t)pSap2->RemoteSap.ulIpAddr << 16) | pSap2->RemoteSap.nPort);

    ullRet = ullVal1 - ullVal2;
    if (ullRet > 0)
        return(1);
    if (ullRet < 0)
        return(-1);

    ullVal1 = (((uint64_t)pSap1->LocalSap.ulIpAddr << 16) | pSap1->LocalSap.nPort);
    ullVal2 = (((uint64_t)pSap2->LocalSap.ulIpAddr << 16) | pSap2->LocalSap.nPort);
    ullRet = ullVal1 - ullVal2;

    if (ullRet > 0)
        return(1);
    if (ullRet < 0)
        return(-1);
    return(0);

}


/////////////////////////////////////////////////////////////////////
int BlrudpLocalSap(BLRUDP_MGR *pBlrudp,
                   BLRUDP_LOCAL_SAP_ITEM **ppSap,
                   unsigned long ulIpAddr,
                   unsigned short nPort)
//
// Get or create a local SAP.
//
// This looks to see if the local sap already exists, if it does it retrieves
// it and increments its refernece counter.
//
// If it does not exist, it attempts to create it and create a socket to
// go with it.
//
// inputs:
//    pBlrudp -- pointer to the Blrudp instance
//    ppSap -- pointer to where to stuff the found sap.
//    ulIpAddr -- ip address in network byte order.
//    nPort -- port number in native byte order.
// outputs:
//    returns -- BLERR_SUCCESS
//               BLERR_NO_MEMORY
//               BLERR_
//    *ppSap -- pointer to local sap, or NULL if we failed to create it.
//
//
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_LOCAL_SAP_ITEM *pLocalSap = NULL;
    BLRUDP_SAP_ADDR Sap;

    Sap.ulIpAddr = ulIpAddr;
    Sap.nPort = nPort;
    pLocalSap = BlMapFindItem(pBlrudp->mapLocalSaps, &Sap);
    if (pLocalSap)
    {
        pLocalSap->ulRefCount++;
    }
    else
    {
        int nRet;
        unsigned short nActPortNo;
        SOCKET hSocket;
        pLocalSap = BlrudpLocalSapNew(pBlrudp,
                                     ulIpAddr,
                                     nPort);
        if (pLocalSap == NULL)
            FAILERR(nErr, BLERR_NO_MEMORY);


        nRet = BlrudpLocalSapSetMtuSize(pLocalSap);
        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, BLERR_BLRUDP_CREATE_SOCKET);

        //
        // attempt to create the socket.
        //
        hSocket = BlrIoCreateSocket(pLocalSap,
                                    ulIpAddr,
                                    nPort,
                                    &nActPortNo);
        if (hSocket == INVALID_SOCKET)
            FAILERR(nErr, BLERR_BLRUDP_CREATE_SOCKET);
        pLocalSap->hSocket = hSocket;
        if (nPort == 0)             // dynamic port assignment??
        {
            pLocalSap->Sap.nPort = nActPortNo;
        }
            

        BlrudpLocalSapBuildReadSelects(pBlrudp);        // rebuld the select and poll list...

    }

    *ppSap = pLocalSap;

    return(BLERR_SUCCESS);
Fail:
    if (pLocalSap)
        BlrudpLocalSapRelease(pBlrudp, pLocalSap);
    return(nErr);
}



//////////////////////////////////////////////////////////////////////
void BlrudpXmitWorkCallback(struct T_BLWORK_MGR *pBlWork UNUSED,
                            struct T_BLWORK_ITEM *pWorkItem UNUSED,
                            void *pUserData)
//
// Transmitter work callback callback.
//
// inputs:
//    pBlWork -- pointer to the work manager this belongs to.
//    pWorkItem -- the work item this callback corresponds to.
//    pUserData -- user data passed in when the work item was initalized
// outputs:
//    none.
//
// WORK is removed from the work queue when this is called.  It can be
// rescheduled by callling ..AddWork...
//
//
/////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)pUserData;
    ASSERT(pContext);
    if (pContext == NULL)
        return;

    if (pContext->pStateTbl)
        (*pContext->pStateTbl->pfnXmitWork)(pContext);
}
//////////////////////////////////////////////////////////////////////
void BlrudpXmitDoneWorkCallback(struct T_BLWORK_MGR *pBlWork UNUSED,
                                struct T_BLWORK_ITEM *pWorkItem UNUSED,
                                void *pUserData)
//
// Transmitter Done work callback callback.
//
// inputs:
//    pBlWork -- pointer to the work manager this belongs to.
//    pWorkItem -- the work item this callback corresponds to.
//    pUserData -- user data passed in when the work item was initalized
// outputs:
//    none.
//
// The Send packet vector is examined to determine which
// buffers we can release.  The buffers are released and the client is called back...
//
//
/////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)pUserData;
    BLRUDP_PKT_VECTOR *pVector;
    uint32_t ulIndex;
    uint32_t ulSseq;
    uint32_t ulEndSeq;

    ASSERT(pContext);
    if (pContext == NULL)
        return;


    pVector = &pContext->XmitPktVector;
    ulIndex = pContext->XmitPktVector.ulBaseIndex;  // starting index.

    ulSseq = pContext->XmitPktVector.ulBaseSeq;  // starting location.
    ulEndSeq = pContext->RecvSeq.ulRseq;


    // dbgPrintf("<%ld> BlrudpXmitDoneWorkCallback(ulSeq=%lx, ulEndSeq=%lx)\n",
    //           pContext->ulContextId,
    //           ulSseq,
    //           ulEndSeq);

    //
    // double check our numbers...
    //
    ASSERT( ((ulEndSeq - ulSseq) & 0x80000000L) == 0);

    // do we have any room to move.....
    while (ulSseq != ulEndSeq)
    {
        BLRUDP_IO_PKT *pPkt;

        pPkt =  pVector->pvectPkts[ulIndex];
        ASSERT(pPkt);       // we must have this little bugger.
        if (pPkt)   // make sure we don't crash...
        {
            ASSERT(pPkt->ulSseq == ulSseq);   // this should be the same...

            pVector->pvectPkts[ulIndex] = NULL; // clear this out...
            //
            // call the receive callback if we have one...
            // if we don't??? well...
            if (pContext->pfnSendBufferDone)
            {
                (*pContext->pfnSendBufferDone)(pContext,
                                               pPkt->ulSendBufferID,
                                               pContext->pUserData);
            }

            if (pPkt->link.next)    // remove it from any queue it is in..
                DeQ(&pPkt->link);    // i.e. retransmit queue...

            //
            // return it to the packet pool....
            //
            BlrudpIoPktPutSendPkt(pContext->pPktPool,
                             pPkt);

        }
        ulSseq++;
        ulIndex++;

        if (ulIndex >= pVector->ulNumEntries)   // did we wrap...???
            ulIndex = 0;                        // yes, wrap around...
        pContext->XmitPktVector.ulBaseSeq = ulSseq;     // save this off..
        pContext->XmitPktVector.ulBaseIndex = ulIndex;
    }

    // dbgPrintf("(ulSeq=%lx, ulEndSeq=%lx)\n",
    //           ulSseq,
    //           ulEndSeq);

} // BGF removed semi after method close squiggle

//////////////////////////////////////////////////////////////////////
void BlrudpRecvWorkCallback(struct T_BLWORK_MGR *pBlWork UNUSED,
                            struct T_BLWORK_ITEM *pWorkItem UNUSED,
                            void *pUserData)
//
// Receiver work callback Work callback.
//
// inputs:
//    pBlWork -- pointer to the work manager this belongs to.
//    pWorkItem -- the work item this callback corresponds to.
//    pUserData -- user data passed in when the work item was initalized
// outputs:
//    none.
//
// WORK is removed from the work queue when this is called.  It can be
// rescheduled by callling ..AddWork...
//
//
/////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)pUserData;
    ASSERT(pContext);
    if (pContext == NULL)
        return;

    // dbgPrintf("<%ld> BlrudpRecvWorkCallback\n", pContext->ulContextId);

    //
    // call the receive callback if we have one...
    // if we don't??? well...
    //
    if (pContext->pfnContextRecv)
    {
        (*pContext->pfnContextRecv)(pContext, pContext->pUserData);
        //
        // if we still the callee has not drained the buffer, then schedule
        // more work..
        // Keep nagging the caller until he picks up the data....
        if (BlrudpGetRecvPktCount(pContext) > 0)
        {
            BlrWorkAddItem(pContext->pBlrudp->pRecvWorkMgr,        // work work manager.
                           &pContext->RecvWorkItem);               // work item to add.
        }
    }
}
//////////////////////////////////////////////////////////////////////
void BlrudpWTimerCallback(BLTIME_MGR *pTimeMgr UNUSED,
                          struct T_BLTIME_TIMER *pTimer UNUSED,
                          void *pUserData UNUSED)
//
// WTimer Callback function.  This function is called when the WTimer
// expires.
//
// The WTIMER is the timer that guards against the
// loss of a packet with the SREQ bit set.  Whenever a
// packet with the SREQ bit set is sent, the
// transmitter increments the saved_sync value by one
// and places it into the sync field of the packet.
// The context sending the packet also starts the WTIMER
// loading it with the smoothed round-trip time estimate.  The
// WTIMER is the amount of time the transmitter will wait for
// the arrival of the control packet requested with the SEQ bit.
// If a control packet arrives at the transmitter before the
// WTIMER expires, the value in the echo field is compared with
// the saved_sync value.  If they are equal, the WTIMER is
// stopped.  If the WTIMER expires, the context starts a
// synchronizing handshake.
//
// inputs:
//    pTimeMgr -- pointer to timemgr that called this callback function.
//    pTimer -- pointer to timer that caused this callback.
//    pUserDAta -- user data passed in when the timer was initailized.
// outputs:
//    none.
//
//  NOTE: The pTimer is removed from the mgr when this function is called.
//
/////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)pUserData;
    ASSERT(pContext);
    if (pContext == NULL)
        return;

    if (pContext->pStateTbl)
        (*pContext->pStateTbl->pfnWTimer)(pContext);

}

/////////////////////////////////////////////////////////////////////////////
void BlrudpCTimerCallback(BLTIME_MGR *pTimeMgr UNUSED,
                          struct T_BLTIME_TIMER *pTimer UNUSED,
                          void *pUserData UNUSED)
//
// CTimer Callback function.
//
// An association is also terminated if no packets are received
// at one of the contexts for some period of time. The
// connection timer CTIMER is used to allow BLRUDP to recover
// from system and network failure by measuring inactivity. The
// CTIMER is enabled when the association first becomes active.
// The context counts the number of packets that have arrived
// during the CTIMER interval. If the packet count is greater
// than zero when the CTIMER expires, the CTIMER is restarted.
// If the packet count is zero, the CTIMER is restarted, and
// the context initiates a synchronizing handshake to verify
// that the other  endpoint is still alive. If the
// synchronizing handshake fails, the context is aborted.
//
// inputs:
//    pTimeMgr -- pointer to timemgr that called this callback function.
//    pTimer -- pointer to timer that caused this callback.
//    pUserDAta -- user data passed in when the timer was initailized.
// outputs:
//    none.
//
//  NOTE: The pTimer is removed from the mgrwhen this function is called.
//
/////////////////////////////////////////////////////////////////////////////
{

    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)pUserData;
    ASSERT(pContext);
    if (pContext == NULL)
        return;

    if (pContext->pStateTbl)
        (*pContext->pStateTbl->pfnCTimer)(pContext);

}
/////////////////////////////////////////////////////////////////////////////
void BlrudpCTimeoutCallback(BLTIME_MGR *pTimeMgr UNUSED,
                          struct T_BLTIME_TIMER *pTimer UNUSED,
                          void *pUserData UNUSED)
//
// CTimeout callback function.
//
// The CTIMEOUT timer limits the amount of time a synchronizing
// handshake can continue before the context aborts the
// association.
// The CTIMEOUT timer is also used when a context goes into the
// zombie state after sending a packet with the END bit set.
// The CTIMEOUT timer can be disabled by setting the CTIMEOUT
// interval to zero, but if this is done, the initial value of
// the retry_count for the synchronizing handshake must not also be zero.
//
// inputs:
//    pTimeMgr -- pointer to timemgr that called this callback function.
//    pTimer -- pointer to timer that caused this callback.
//    pUserDAta -- user data passed in when the timer was initailized.
// outputs:
//    none.
//
//  NOTE: The pTimer is removed from the mgr when this function is called.
//
/////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)pUserData;
    ASSERT(pContext);
    if (pContext == NULL)
        return;

    if (pContext->pStateTbl)
        (*pContext->pStateTbl->pfnCTimeout)(pContext);


}

/////////////////////////////////////////////////////////////////////////////
void BlrudpRTimerCallback(BLTIME_MGR *pTimeMgr UNUSED,
                          struct T_BLTIME_TIMER *pTimer UNUSED,
                          void *pUserData UNUSED)
//
// Rtimer callback function.
//
// The RTIMER is the rate control timer used to govern the
// frequency of sending bursts of data. Its use is described in
// Section "Rate Control."
//
// inputs:
//    pTimeMgr -- pointer to timemgr that called this callback function.
//    pTimer -- pointer to timer that caused this callback.
//    pUserDAta -- user data passed in when the timer was initailized.
// outputs:
//    none.
//
//  NOTE: The pTimer is removed from the mgr when this function is called.
//
/////////////////////////////////////////////////////////////////////////////
{
    BLRUDP_ACTIVE_CONTEXT *pContext = (BLRUDP_ACTIVE_CONTEXT *)pUserData;
    ASSERT(pContext);
    if (pContext == NULL)
        return;

    if (pContext->pStateTbl)
        (*pContext->pStateTbl->pfnRTimer)(pContext);
}










