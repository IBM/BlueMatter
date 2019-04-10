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
 //////////////////////////////////////////////////////////////////////////////////////
// Bluelight Reliable Udp common helper functions.
//
//

#include "hostcfg.h"

#if HAVE_MEMORY_H
#include <memory.h>
#endif

#if HAVE_STDINT_H
    #include <stdint.h>
#elif USE_GDB_STDINT_H
    #include "gdb_stdint.h"
#endif

#if HAVE_LINUX_IN_H
    #include "linux/in.h"
#endif

#include "BlErrors.h"
#include "blrudp_common.h"
#include "blrudp_state.h"
#include "blrudppkt_help.h"
#include "blrudp_pktpool.h"
#include "blrudp_pktstatus.h"
#include "blrudp_io.h"
#include "blrudp_memory.h"
#include "dbgtimelog.h"
#include "dbgPrint.h"

#include "bldefs.h"


//
// First Recv....
BLRUDP_EVENT_VTABLE StateFirstSentEventTbl = {
     BlrudpStFirstSentXmit,        // pfnXmitWork
     BlrudpStFirstSentRecv,        // pfnRecvPacket
     BlrudpStFirstSentWTimer,      // pfnWTimer
     BlrudpStFirstSentCTimer,      // pfnCTimer
     BlrudpStFirstSentCTimeout,    // pfnCTimeout
     BlrudpStFirstSentRTimer       // pfnRTimer
};
BLRUDP_EVENT_VTABLE StateFirstRecvEventTbl = {
     BlrudpStFirstRecvXmit,        // pfnXmitWork
     BlrudpStFirstRecvRecv,        // pfnRecvPacket
     BlrudpStFirstRecvWTimer,      // pfnWTimer
     BlrudpStFirstRecvCTimer,      // pfnCTimer
     BlrudpStFirstRecvCTimeout,    // pfnCTimeout
     BlrudpStFirstRecvRTimer       // pfnRTimer
};
BLRUDP_EVENT_VTABLE StateEstabEventTbl = {
     BlrudpStEstabXmit,        // pfnXmitWork
     BlrudpStEstabRecv,            // pfnRecvPacket
     BlrudpStEstabWTimer,          // pfnWTimer
     BlrudpStEstabCTimer,          // pfnCTimer
     BlrudpStEstabCTimeout,        // pfnCTimeout
     BlrudpStEstabRTimer           // pfnRTimer
};
BLRUDP_EVENT_VTABLE StateClosingEventTbl = {
     BlrudpStClosingXmit,        // pfnXmitWork
     BlrudpStClosingRecv,          // BlrudpStFirstSentWTimer
     BlrudpStClosingWTimer,        // pfnWTimer
     BlrudpStClosingCTimer,        // pfnCTimer
     BlrudpStClosingCTimeout,      // pfnCTimeout
     BlrudpStClosingRTimer         // pfnRTimer
};
BLRUDP_EVENT_VTABLE StateZombieEventTbl = {
     BlrudpStZombieXmit,        // pfnXmitWork
     BlrudpStZombieRecv,           // pfnRecvPacket
     BlrudpStZombieWTimer,         // pfnWTimer
     BlrudpStZombieCTimer,         // pfnCTimer
     BlrudpStZombieCTimeout,       // pfnCTimeout
     BlrudpStZombieRTimer          // pfnRTimer
};

// static table matching the active states with their state VTables

BLRUDP_EVENT_VTABLE *ActiveStateList[BLRUDP_CTX_STATE_MAXSTATE] = {
    NULL,                                   // BLRUDP_CTX_STATE_CLOSED     = 0,     
    &StateFirstSentEventTbl,                // BLRUDP_CTX_STATE_FIRST_SENT = 1,     
    &StateFirstRecvEventTbl,                // BLRUDP_CTX_STATE_FIRST_RECV = 2,     
    &StateEstabEventTbl,                    // BLRUDP_CTX_STATE_ESTABLISHED = 3,    
    &StateClosingEventTbl,                  // BLRUDP_CTX_STATE_CLOSING    = 4,     
    &StateZombieEventTbl                    // BLRUDP_CTX_STATE_ZOMBIE     = 5
};                                        


/////////////////////////////////////////////////////////////////
int BlrudpSendResetPkt(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Send a reset packet to the other end of this connection.
//
// A reset packet is a packet with the END bit set AND it
// has a r-seq number matching the sSeq number received
// by this connection when it received the packet causing
// this to happen...
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = NULL;
    int nRet;
    pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);
    ASSERT(pPkt);                   // there is a problem here if on the first
    if (pPkt == NULL)               // time we don't get a packet.
        FAIL;

    
    //
    // fill out the reset packet.
    BlrudpFillResetPkt(pContext, pPkt, pContext->ulXmtResetSeq);   
    nRet = BlrudpSendCtrlPkt(pContext,        // pContext
                             pPkt);           // pPkt

    if (nRet != BLERR_SUCCESS)
        FAIL;
    return(BLERR_SUCCESS);

Fail:
    return(nErr);
}


////////////////////////////////////////////////////////////////
int BlrudpSendFirstPkt(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Send off the first packet to the SAP associated with this context.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pPkt = NULL;
    int nRet;
#if TEST_HALF_OPEN
    if (pContext->nRetryCount > pContext->Options.ulMaxRetryCount)
    {
        BlrudpSetZombieState(pContext,                          // pContext
                             BLERR_BLRUDP_CONNECT_TIMEOUT);     // nBlError
        return(BLERR_SUCCESS);
    }
#endif

    //
    // First time in here, the retry count is zero indicating
    // we have not built a packet yet....
    //
    // dbgPrintf("BlrudpSendFirstPkt\n");


    pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);
    ASSERT(pPkt);                   // there is a problem here if on the first
    if (pPkt == NULL)               // time we don't get a packet.
        FAIL;


    BlrudpFillFirstPkt(pContext, pPkt);        

    if (pContext->nRetryCount == 0)     // first time, start the CTimeout...
    {                                   // how long we will try to connect...
        pContext->ulWTimerFactor = 1;
        // TBD.. get this from an options parameter...
        
        BltwTimerAdd(pContext->pBlrudp->pTimeMgr,       // pTimeMgr
                     &pContext->Timers.CTimeout,        // pTimer
                     BLRUDP_DEFAULT_CTIMEOUT);          // retry timeout.
        pContext->nRetryCount++;    
    }

    //
    // attempt to send the packet...
    //
    nRet = BlrudpSendCtrlPkt(pContext,          // pContext
                             pPkt);              // pPkt
    if (nRet == BLERR_SUCCESS)
    {
        pContext->nRetryCount++;        
    }    

    return(nRet);       // return the result of the actual send...
Fail:
    //
    // TBD....
    //
    // Transition us to the zombie state in error.
    //
    BlrudpSetActiveState(pContext,                      // pContext
                         BLRUDP_CTX_STATE_ZOMBIE,       // nState
                         nErr);                         // nBlError

    return(nErr);
}

//////////////////////////////////////////////////////////////////////////
int BlrudpSendCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_IO_PKT *pPkt)
//
// Send a packet to the remote endpoint indicated remote sap in the 
// active context pContext.
//
// If we cannot send, then set the retry timer for another go around.
// on the next run...  The caller will have to regenerate the packet...
//
//
// The primary difference between control packet and data packet sends.
// is that data packet sends get put on the Sent queue, Control
// packets get returned to the packet pool.
//
// inputs:
//    pContext -- context containing address to send this to..
//    pPkt -- packet to send..
//
// outputs:
//    returns -- BLERR_SUCCESS if packet is successfully sent.
//               
//
{
    int nErr = BLERR_INTERNAL_ERR;
    uint16_t usSReq = 0;
    // 
    // Check to see if we can send...
    //
    if (BlrIoDgramXmitReady(pContext->pLocalSap->hSocket))
    {
        int nRet;
        BLRUDP_PKT_HDR *pPktHdr;

        pPktHdr = BlrudpGetPktPayload(pPkt);

        usSReq = ntohs(pPktHdr->usCmd) & BLRUDP_CMD_SREQ;

        nRet = BlrIoSendPkt(pContext->pLocalSap,                // hSocket
                            &pContext->SapPair.RemoteSap,       // pSapAddr,
                            pPkt);                              // pPkt);
        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, nRet);
    

        // remember we sent this...
        pContext->XmitSeq.ulLastAllocSeqSent = pContext->XmitSeq.ulAllocSeq;        
        if (usSReq)
        {
            BlrudpSentSreq(pContext);       // signal that we just sent a packet
                                            // with an sreq...
        }

        BlrudpIoPktPutCtrlPkt(pContext->pPktPool,                // return it to the packet pool...
                         pPkt);

    }
    else
    {
        FAILERR(nErr, BLERR_BLRUDP_SEND_BUSY);
    }
    return(BLERR_SUCCESS);
Fail:
    BlrudpIoPktPutCtrlPkt(pContext->pPktPool,       // return it to the packet pool...
                     pPkt);
    pContext->usXmitFlags |= usSReq;           // if we failed and were about
                                               // to transmit a SReq, then reset it
                                               // to transmit on the next one...
    //
    // Reschedule send work for this...
    //
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
    return(nErr);
}

///////////////////////////////////////////////////////////////////////////
int BlrudpSendDataPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                       BLRUDP_IO_PKT *pPkt)
//
// Send a packet to the remote endpoint indicated remote sap in the 
// active context pContext.
//
//
// inputs:
//    pContext -- context containing address to send this to..
//    pPkt -- packet to send
//
// outputs:
//    returns -- BLERR_SUCCESS if packet is successfully sent.
//
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_PKT_HDR *pPktHdr;
    uint16_t usSReq = 0;

    pPktHdr = BlrudpGetPktPayload(pPkt);
    
    ASSERT((ntohs(pPktHdr->usCmd) & BLRUDP_CMD_TYPE) == BLRUDP_CMD_TYPE_DATA);     // hey....

    // 
    // Check to see if we can send...
    //
    if (BlrIoDgramXmitReady(pContext->pLocalSap->hSocket))
    {
        int nRet;
        BLRUDP_PKT_HDR *pPktHdr;

        //
        // Check to see if we are requesting a sync response...
        pPktHdr = BlrudpGetPktPayload(pPkt);
        usSReq = ntohs(pPktHdr->usCmd) & BLRUDP_CMD_SREQ;


        nRet = BlrIoSendPkt(pContext->pLocalSap,                // hSocket
                            &pContext->SapPair.RemoteSap,       // pSapAddr,
                            pPkt);                              // pPkt);
        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, nRet);


        if (usSReq)
        {
            BlrudpSentSreq(pContext);       // signal that we just sent a packet
                                            // with an sreq...
        }

        // NOTE: We DON't put the packets back on the send list BECAUSE we have to retain
        // these packets in our retransmit queue..

    }
    else
    {
        FAILERR(nErr, BLERR_BLRUDP_SEND_BUSY);
        ASSERT(0);          // should not happen, don't send if we are not clear to send...
    }
    return(BLERR_SUCCESS);
Fail:

    pContext->usXmitFlags |= usSReq;           // if we failed and were about
                                               // to transmit a SReq, then reset it
                                               // to transmit on the next one...
    //
    // Reschedule send work for this...
    //
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
    return(nErr);
}

//////////////////////////////////////////////////////////////////
void  BlrudpSetInitialSeqNumbers(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Set the initial sequence numbers for this active context.
//
// inputs:
//   pContext -- context to set the numbers for.
// outputs:
//   pContext -- sequence numbers set in the context.
//
{
    //
    // For now, a simple increment..
    // 
    // Later, we may want to select them at random
    // if the context is not being re-used....
    //
    pContext->XmitSeq.ulSseq = pContext->XmitSeq.ulSseq + 0x1000+51;      

    //
    // What ever we received from the other end, is not valid,
    // so set it to what we are sending here.  It is what we
    // are expecting back...
    pContext->RecvSeq.ulRseq = pContext->XmitSeq.ulSseq;        

}

////////////////////////////////////////////////////////////
void BlrudpSetZombieState(BLRUDP_ACTIVE_CONTEXT *pContext,
                          int nBlError)
//
// Set the active context into the zombie state and record
// the reason why we entered the state.
//
// The reason controls what to do on a Wtimer, which happens
// when the UDP stack is busy and we could not send the
// packet that caused us to enter the zombie state and we have
// to resend it..
//
// inputs:
//    pContext -- context to set the state in.
//    nZombieSrc -- source of entering the zombie state.
//    nBlError -- error code to report to the client application
//                as the reason for this state change...
//
{
    pContext->usXmitFlags |= BLRUDP_CMD_END;                // indicate that we are at the end...
    BlrudpSetActiveState(pContext,                          // pContext
                         BLRUDP_CTX_STATE_ZOMBIE,           // nState
                         nBlError);                         // blnerr
}



/////////////////////////////////////////////////////////////////////
void BlrudpSetActiveState(BLRUDP_ACTIVE_CONTEXT *pContext,
                          BLRUDP_ACT_STATE nState,
                          int nBlError)
//
// This sets the active state to the state indicated.
//
// inputs:
//    pContext -- context to set the state in.
//    nState -- state to set the context to.
//    nBlError -- error code to report to the client application
//                as the reason for this state change...
// outputs:
//    none.
//
{
    int nOldUsrState;
    int nNewUsrState;
    void *pUserData;
    PFN_BLRUDP_CONTEXT_CHANGED pfnContextChanged;

    ASSERT(nState < BLRUDP_CTX_STATE_MAXSTATE);
    if (nState >= BLRUDP_CTX_STATE_MAXSTATE)
        return;

    //dbgPrintf("State(%s->%s)\r\n",
    //          BlrudpStateString(pContext->nState),
    //          BlrudpStateString(nState));

    nOldUsrState = BlrudpUserState(pContext->nState);
    nNewUsrState = BlrudpUserState(nState);
    //
    // zero out retry count on state transition.
    //
    pContext->nRetryCount = 0;
    pContext->ulWorkFlags = 0;
    pContext->nState = nState;
    pContext->pStateTbl = ActiveStateList[nState];  // pick up the state Vtable.

    pfnContextChanged = pContext->pfnContextChanged;
    pUserData = pContext->pUserData;

    //
    // When we go into the zombie state, we set the zombie timeout.... Always
    //
    if (nState == BLRUDP_CTX_STATE_ZOMBIE)
    {
        BLTIME_MS fTimeout;

        BlrudpActCtxClearTimers(pContext);                          // changing state, clears all active timers...
        BlrudpActCtxClearCallbacks(pContext);                       // clear the callbacks..
                                                                    // zombie states only happen once....
        if (pContext->Options.ulMTL > BLRUDP_MIN_ZOMBIE_TIMEOUT)
            fTimeout = pContext->Options.ulMTL;
        else
            fTimeout = BLRUDP_MIN_ZOMBIE_TIMEOUT;
        BltwTimerAdd(pContext->pBlrudp->pTimeMgr,           // pTimeMgr
                     &pContext->Timers.CTimeout,            // pTimer
                     BLRUDP_MIN_ZOMBIE_TIMEOUT);            // 2 minute timeout.
                

    }

    // inform the user of this change????
    if ((pfnContextChanged)  && (nNewUsrState != nOldUsrState))
    {
        (*pfnContextChanged)(pContext,                    // pContext
                             nOldUsrState,                // nOldState,
                             nNewUsrState,                // nNewState,
                             nBlError,                    // nBlError,
                             pUserData);                  // pUserData
    }




}
////////////////////////////////////////////////////////////////////////////////
int BlrudpUserState(BLRUDP_ACT_STATE nState)
//
// Map the internal state to a user state.
//
// inputs:
//    nState -- internal state to map.
// outputs:
//    returns -- user state this corresponds to.
//
{
    typedef struct {
        int nUserState;
        BLRUDP_ACT_STATE nActState;
    } STATE_MAP;

    int nUserState = BLRUDP_STATE_CLOSED;
    static STATE_MAP StateMap[BLRUDP_CTX_STATE_MAXSTATE] = {
        {BLRUDP_STATE_CLOSED,            BLRUDP_CTX_STATE_CLOSED     },
        {BLRUDP_STATE_CONNECTING,        BLRUDP_CTX_STATE_FIRST_SENT },
        {BLRUDP_STATE_CONNECTING,        BLRUDP_CTX_STATE_FIRST_RECV },
        {BLRUDP_STATE_CONNECT,           BLRUDP_CTX_STATE_ESTABLISHED},
        {BLRUDP_STATE_CLOSING,           BLRUDP_CTX_STATE_CLOSING    },
        {BLRUDP_STATE_CLOSED,            BLRUDP_CTX_STATE_ZOMBIE     }
    };
    ASSERT(nState < BLRUDP_CTX_STATE_MAXSTATE);
    if (nState >= BLRUDP_CTX_STATE_MAXSTATE)
        FAIL;

    ASSERT(StateMap[nState].nActState == nState);   // in debug, make sure we notice the mapping is out...

    nUserState = StateMap[nState].nUserState;       // grab the return value.

    return(nUserState);
Fail:
    return(nUserState);
}


///////////////////////////////////////////////////////////////////
void BlrudpActCtxClearCallbacks(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Clear the callbacks associated with an active context.
//
// inputs:
//    pContext - context to clear the callback pointers in.
// outputs:
//   none.
//
{
    pContext->pfnContextChanged = NULL;
    pContext->pfnContextRecv = NULL;
    pContext->pfnSendBufferDone = NULL;
    pContext->pUserData = NULL;

}


///////////////////////////////////////////////////////////
void BlrudpPassiveRecvPkt(BLRUDP_PASSIVE_CONTEXT *pPassContext, 
                          BLRUDP_ACTIVE_CONTEXT *pExistingActContext,
                          BLRUDP_IO_PKT *pRecvPkt,
                          unsigned long ulIpAddr,
                          unsigned short nPort )
//
// process a receive packet by a passive context.
//
// inputs:
//    pContext -- passive context to process this for.
//    pExistingActContext -- if NOT NULL, then use this existing
//                           active context rather than
//                           creating a new one...
//
//    pRecvPkt -- received packet..
//    ulIpAddr -- ip address of remote connection.
//    nPort -- port of the remote connection.
// outputs:
//    none.
{
    uint16_t usCmd;
    BLRUDP_PKT_HDR *pPktHdr;
    
    pPktHdr = BlrudpGetPktPayload(pRecvPkt);
     
    // dbgPrintf("BlrudpPassiveRecvPkt(%x)\n", pPktHdr->usCmd);

    
    ASSERT(pPassContext->pfnListenConnectReq);
    if (pPassContext->pfnListenConnectReq == NULL)
        FAIL;
    usCmd = ntohs(pPktHdr->usCmd);

    if (usCmd & BLRUDP_CMD_CHECKSUM)
    {
        unsigned long ulChecksum;
        ulChecksum = BlrudpXsumPkt(pPktHdr,
                                   ntohs(pPktHdr->usDlen));
        if (ulChecksum != 0)    // checksum NG, drop it...
            FAIL;               // ignore the packet...
    }

    //
    // valid messages in the listen state is the FIRST packet
    // and really nothing else....
    //
    usCmd = (usCmd & BLRUDP_CMD_TYPE);
    if (usCmd == BLRUDP_CMD_TYPE_FIRST)     
    {
        BLRUDP_ACTIVE_CONTEXT *pActContext;
        BLRUDP_IO_TCTRL_PKT *pFirstPkt;

        pFirstPkt = (BLRUDP_IO_TCTRL_PKT *)pPktHdr;

        if (pExistingActContext == NULL)
        {
            pActContext = BlrudpActCtxNew(pPassContext->pBlrudp);
            if (pActContext == NULL)
            {
                //out of memory.  Respond with error message to the
                // other side...
                FAIL;
            }
            pActContext->pLocalSap = pPassContext->pLocalSap;     // save this
            pActContext->pLocalSap->ulRefCount++;                 // 
            //
            // Remember the pair of addresses.
            //
            pActContext->SapPair.LocalSap.ulIpAddr   = pPassContext->pLocalSap->Sap.ulIpAddr;
            pActContext->SapPair.LocalSap.nPort      = pPassContext->pLocalSap->Sap.nPort;
            pActContext->SapPair.RemoteSap.ulIpAddr  = ulIpAddr;
            pActContext->SapPair.RemoteSap.nPort     = nPort;
        }
        else
        {
            pActContext = pExistingActContext;
            //
            // if all is well, these don't change...
            //
            ASSERT(pActContext->SapPair.LocalSap.ulIpAddr   ==  pPassContext->pLocalSap->Sap.ulIpAddr);
            ASSERT(pActContext->SapPair.LocalSap.nPort      ==  pPassContext->pLocalSap->Sap.nPort);
            ASSERT(pActContext->SapPair.RemoteSap.ulIpAddr  ==  ulIpAddr);
            ASSERT(pActContext->SapPair.RemoteSap.nPort     ==  nPort);
        }

        //
        // Override the options with the options of the listening context.
        memcpy(&pActContext->Options,               // dest
               &pPassContext->Options,              // src
               sizeof(pActContext->Options));       // len

        //
        // Need to record the Receive sequence numbers now...
        // First packet we take these numbers as is, without range checks... 
        //

        // First packet, the rseq is not valid.  once we connnect it should
        // reflect our xmit seq...
        pActContext->RecvSeq.ulSseq = ntohl(pPktHdr->ulSseq);
        pActContext->RecvSeq.ulRseq = pActContext->XmitSeq.ulSseq;      // zero out this delta.
        pActContext->RecvSeq.ulNextInSeq = pActContext->RecvSeq.ulSseq;


        //
        // Save off initial synchronization values...
        pActContext->ulRecvCtrlSync     = ntohl(pFirstPkt->Ctrl.IoPkt.ulSync);
        pActContext->RecvSeq.ulAllocSeq = ntohl(pFirstPkt->Ctrl.ulAllocSeq);        

        BlrudpProcessRemoteOptions(pActContext,
                                   pFirstPkt);



        //
        // pick up the transmitters Sseq as our Rseq so it 
        // recognizes it as a response..
        //
        pActContext->XmitSeq.ulRseq = pActContext->RecvSeq.ulSseq;

        pActContext->ulRecvCtrlSync     = ntohl(pFirstPkt->Ctrl.IoPkt.ulSync);
        pActContext->ulRcvdEcho         = ntohl(pFirstPkt->Ctrl.ulEcho);
        pActContext->RecvSeq.ulAllocSeq = pActContext->RecvSeq.ulSseq;

        //
        // Call the listener callback function...
        // this should either accept or reject the connection.....
        //
        (*pPassContext->pfnListenConnectReq)(pPassContext,      // hListenContext
                                             pActContext,       // hNewContext
                                             ulIpAddr,          // nAddr
                                             nPort,             // nPort
                                             pPassContext->pUserData);
        //
        // was accept or reject called on the packet...
        // 
        if (!pActContext->bAccRejCalled)   
        {
            BlrudpReject(pActContext);  // call reject ourselves...
        }
    }


    return;
Fail:
    return;
}
//////////////////////////////////////////////////////////////////
void BlrudpProcessRemoteOptions(BLRUDP_ACTIVE_CONTEXT *pContext,
                                BLRUDP_IO_TCTRL_PKT *pTCtrlPkt)
//
// Process the remote options from a Tctrl packet and place it into the
// remote options section of the context.
//
// inputs:
//    pContext -- context to process this against.
//    pTctrlPkt -- control packet to parse.
//
// outputs:
//    none. -- all inforamtion is placed into the pcontext structure.
//
{
    uint8_t *pPayload;
    uint8_t *pEndPayload;
    int n;
    int nNumParams;
    BLRUDP_IO_TCTRL_PAYLOAD *pTctrl;

    //
    // process the remote options payload and record it in the
    // active context..
    //
    pPayload = (uint8_t *)pTCtrlPkt->ParamPayload;
    pEndPayload = ((uint8_t *)pTCtrlPkt) + ntohs(pTCtrlPkt->Ctrl.IoPkt.usDlen);
    nNumParams = ntohs(pTCtrlPkt->nParams);
    for (n = 0; n < nNumParams; n++)
    {
        pTctrl = (BLRUDP_IO_TCTRL_PAYLOAD *)pPayload;
        switch (pTctrl->nKey)
        {
            case BLRUDP_TCTRL_WINDOWSIZE:
            {
                uint32_t *pulValue;
                
                ASSERT(pTctrl->nLen == sizeof(uint32_t));
                if (pTctrl->nLen != sizeof(uint32_t))
                    FAIL;
                pulValue = (uint32_t*)pTctrl->szData;
                pContext->RemoteOptions.ulWindowSize = ntohl(*pulValue);
                break;
            }
            case BLRUDP_TCTRL_MTUSIZE:
            {
                uint32_t *pulValue;
                ASSERT(pTctrl->nLen == sizeof(uint32_t));
                if (pTctrl->nLen != sizeof(uint32_t))
                    FAIL;
                pulValue = (uint32_t*)pTctrl->szData;
                ASSERT(*pulValue);                    // should not be zero...

                pContext->RemoteOptions.ulMtuSize = ntohl(*pulValue);
                break;
            }
            case BLRUDP_TCTRL_CHECKSUM:
            {
                uint16_t *pusValue;
                ASSERT(pTctrl->nLen == sizeof(uint16_t));
                if (pTctrl->nLen != sizeof(uint16_t))
                    FAIL;
                pusValue = (uint16_t *)pTctrl->szData;
                pContext->RemoteOptions.bChecksum = (ntohl(*pusValue) != 0);
                break;
            }
        }
        // point to the next location.
        pPayload += pTctrl->nLen + offsetof(BLRUDP_IO_TCTRL_PAYLOAD, szData);    // next location.
        ASSERT(pPayload <= pEndPayload);
        if (pPayload > pEndPayload)         // end of the line, but for the WRONG
            break;                          // reason....
    }
    return;
Fail:
    return;
}

/////////////////////////////////////////////////////////////////////////
void BlrudpSetRecvWindow(BLRUDP_ACTIVE_CONTEXT *pContext, 
                         uint32_t ulRecvWindow)
//
// Set the size of the receive window and any state variables associated with it.
//
// inputs:
//    pContext -- pointer to context this pertains to.
//    ulRecvWindow -- number of packets in the receive window..
//
// outputs:
//    none.
//
{
    pContext->ulNumRecvWinPkts = ulRecvWindow;
    pContext->XmitSeq.ulAllocSeq = pContext->RecvSeq.ulSseq + pContext->ulNumRecvWinPkts;
    pContext->XmitSeq.ulLastAllocSeqSent = pContext->XmitSeq.ulAllocSeq;

    //
    // Select a size that will allow a certain amount of parallelism
    // in the protocol.....
    //
    if (ulRecvWindow < 4)
        pContext->ulAllocRefreshInterval = ulRecvWindow;    // punt, the window is so small we have to wait anyway...
    else
    if (ulRecvWindow < 40)
        pContext->ulAllocRefreshInterval = ulRecvWindow/2;
    else
        pContext->ulAllocRefreshInterval = ulRecvWindow/4;
}
/////////////////////////////////////////////////////////////////////////
void BlrudpActiveRecvAcceptPkt(BLRUDP_ACTIVE_CONTEXT *pContext, 
                               BLRUDP_IO_PKT *pRecvPkt )
//
// Receive and process the accept packet..
//
// We decode the packet, look at the window and MTU size and
// reallocate our packet buffer accordingly.
//
// inputs:
//    pContext -- active context to process this for.
//    pRecvPkt -- received packet..
// outputs:
//    none.
//
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_PKT_HDR *pPktHdr;
    BLRUDP_IO_TCTRL_PKT *pTCtrlPkt;
    unsigned long ulNumPkts;
    int16_t usCmd;
    
    // dbgPrintf("BlrudpActiveRecvAcceptPkt\n");

    pPktHdr = BlrudpGetPktPayload(pRecvPkt);
#if TEST_HALF_OPEN              // don't process this for testing...
    if (pContext->XmitSeq.ulSseq == 0x33)
        return;
#endif

    usCmd = (ntohs(pPktHdr->usCmd) & BLRUDP_CMD_TYPE);
    ASSERT(usCmd == BLRUDP_CMD_TYPE_TCTRL);
    if (usCmd != BLRUDP_CMD_TYPE_TCTRL)         // hey???
        FAIL;                                   // that's not roight.

    pTCtrlPkt = (BLRUDP_IO_TCTRL_PKT *)pPktHdr;

    //
    // Need to record the Receive sequence numbers now... 
    //
    pContext->RecvSeq.ulSseq = ntohl(pPktHdr->ulSseq);
    pContext->RecvSeq.ulRseq = ntohl(pPktHdr->ulRseq);
    pContext->RecvSeq.ulNextInSeq = pContext->RecvSeq.ulSseq;
    //
    // Take the remote options and place them into the RemoteOptions
    // section of this context.
    BlrudpProcessRemoteOptions(pContext, pTCtrlPkt);

    //
    // combine the remote and the local options in this state
    // and perform some rudimentary evaluation...
    //
    nRet = BlrudpCombineRemoteOptions(pContext);
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    //
    // Reallocate the packet pool based on the negoiated options.

    //
    // allocate based on the mtu size etc....
    //
    ulNumPkts = (pContext->Options.ulWindowSize+(pContext->Options.ulMtuSize-1)) / 
        pContext->Options.ulMtuSize;


    //
    // compute

    if (pContext->pCurrRcvPkt)                         // we are taking responsibility 
    {                                                   // for this packet...
        BlrudpIoPktPutRecvPkt(pContext->pPktPool,
                         pContext->pCurrRcvPkt);
        pContext->pCurrRcvPkt = NULL;
    }
    //
    // After the init the packet recv packet is toast... 
    // all receive packets returned to the buffer....

    //
    // initialize the balance of the context.
    nRet = BlrudpActCtxInit(pContext,           // pContext             
                            ulNumPkts,          // ulNumXmtPkts         
                            ulNumPkts,          // ulNumRcvPkts         
                            TRUE);              // bAllocVect           
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);                               

    BlrudpSetRecvWindow(pContext, ulNumPkts);
    

    // set the transmit vector state...
    pContext->XmitPktVector.ulBaseSeq = pContext->XmitSeq.ulSseq;      // start here...
    pContext->XmitPktVector.ulEndSeq = pContext->XmitSeq.ulSseq;       

    // set the receive vector state.  No packets in the vector...
    pContext->RecvPktVector.ulBaseSeq = pContext->RecvSeq.ulSseq;      // start here...
    pContext->RecvPktVector.ulEndSeq  = pContext->RecvSeq.ulSseq;       


    //
    // control packet received with the correct sequence
    // numbers, we are done...
    //
    BlrudpSetActiveState(pContext,                  // pContext
                         BLRUDP_CTX_STATE_ESTABLISHED,   // nState
                         BLERR_SUCCESS);              // blnerr

    // 
    // the proper response is a CTRL packet...
    //
    BlrudpStAddCtrlMsgToWork(pContext);
    return;
Fail:
    //
    // this just sends the END flag, does not tell us why.....
    //
    BlrudpSetActiveState(pContext,                  // pContext
                         BLRUDP_CTX_STATE_ZOMBIE,   // nState       
                         nErr);                     // blnerr
    BlrudpStAddCtrlMsgToWork(pContext);
    return;    
}

///////////////////////////////////////////////////////////
void BlrudpActiveRecvEcntlPkt(BLRUDP_ACTIVE_CONTEXT *pContext, 
                              BLRUDP_IO_PKT *pRecvPkt UNUSED,
                              BLRUDP_PKT_HDR *pPktHdr)
//
// Receive and process an error control packet...
//
// inputs:
//    pContext -- pointer to process the packet for.
//    pRecvPkt -- pointer to packet to process.
//    pPktHdr -- pointer to packet header of the error packet 
//               to process.
//
// outputs:
//    none.
//
{
    BLRUDP_IO_ECNTL_PKT *pEcntlPkt;
    BLRUDP_IO_ECNTL_PAYLOAD *pPayload;
    unsigned int n;
    int bScheduleWork = FALSE;
    
    ASSERT((ntohs(pPktHdr->usCmd) & BLRUDP_CMD_TYPE) == BLRUDP_CMD_TYPE_ECNTL);

    pEcntlPkt = (BLRUDP_IO_ECNTL_PKT *)pPktHdr;     // decode as an error packet...
    pPayload = pEcntlPkt->nEcntlPayload;        // point to the payload....

    
    for (n = ntohs(pEcntlPkt->usEnctls); n > 0; n--)
    {
        uint32_t ulNakMask;
        uint32_t ulSseq;
        uint32_t ulMask;

        ulNakMask = ntohl(pPayload->ulNakMask);
        ulSseq = ntohl(pPayload->ulBaseSeq);
        ASSERT( (ulSseq & 0x1f) == 0);

        // shift all the bit positions.
        for (ulMask = 1; ulMask != 0; ulMask <<= 1)
        {
            if (ulMask & ulNakMask)     // a 1 indicates a Nak...
            {
                BLRUDP_IO_PKT *pPkt;
                pPkt = BlrudpGetVectorsPkt(&pContext->XmitPktVector,
                                           ulSseq);
                //ASSERT(pPkt); 
                if ((pPkt) && (pPkt->ulSseq == ulSseq))     // this would be an OLD nak...
                {
                    //ASSERT(pPkt->ulSseq == ulSseq);
                    if (pPkt->link.next == NULL)        // add to retransmit queue
                        EnQ(&pPkt->link, &pContext->listRexmitPkts);
                    bScheduleWork = TRUE;
                        
                }
            }
            ulSseq++;       // next sequence number....
        }

        pPayload++;     // next payload
    }
    if (bScheduleWork)
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }

}
/////////////////////////////////////////////////////////////////////////////
void BlrudpActiveRecvDiagResp(BLRUDP_ACTIVE_CONTEXT *pContext, 
                              BLRUDP_IO_PKT *pRecvPkt UNUSED,
                              BLRUDP_PKT_HDR *pPktHdr)
//
// Receive and process an Diagnostic response packet...
//
// inputs:
//    pContext -- pointer to process the packet for.
//    pRecvPkt -- pointer to packet to process.
//    pPktHdr -- pointer to packet header of the Diagnostic packet 
//               to process.
//
// outputs:
//    none.
//
{
    BLRUDP_IO_DIAGRESP_PKT *pDiagPkt;
    BLRUDP_IO_DIAGRESP_PAYLOAD *pDiagPayload;
    uint32_t n;
    uint32_t usNumResp;
    uint8_t *pPayload;
    uint8_t *pEndPayload;
    
    ASSERT((ntohs(pPktHdr->usCmd) & BLRUDP_CMD_TYPE) == BLRUDP_CMD_TYPE_DRESP);

    pDiagPkt = (BLRUDP_IO_DIAGRESP_PKT *)pPktHdr;     // decode as an diag packet
    pPayload = (uint8_t *)pDiagPkt->DiagPayload;
    pEndPayload = ((uint8_t *)pDiagPkt) + ntohs(pDiagPkt->Ctrl.IoPkt.usDlen);
    usNumResp = ntohs(pDiagPkt->usNumResp);


    for (n = usNumResp; n > 0; n--)
    {
        pDiagPayload = (BLRUDP_IO_DIAGRESP_PAYLOAD *)pPayload;
        switch (pDiagPayload->usCode)
        {
            case BLRUDP_DIAG_NUMRETRANS:
            {
                uint64_t *pllValue;
                
                ASSERT(pDiagPayload->usLen== sizeof(uint64_t));
                if (pDiagPayload->usLen != sizeof(uint64_t))
                    FAIL;
                pllValue = (uint64_t*)pDiagPayload->Value;
                pContext->RemoteDiag.llNumRetrans = ntoh64(*pllValue);
                break;
            }
            case BLRUDP_DIAG_DATAXMIT:
            {
                uint64_t *pllValue;
                ASSERT(pDiagPayload->usLen == sizeof(uint64_t));
                if (pDiagPayload->usLen != sizeof(uint64_t))
                    FAIL;
                pllValue = (uint64_t*)pDiagPayload->Value;

                pContext->RemoteDiag.llDataXmit = ntoh64(*pllValue);
                break;
            }
            case BLRUDP_DIAG_DATARECV:
            {
                uint64_t *pllValue;
                ASSERT(pDiagPayload->usLen == sizeof(uint64_t));
                if (pDiagPayload->usLen != sizeof(uint64_t))
                    FAIL;
                pllValue = (uint64_t*)pDiagPayload->Value;
                pContext->RemoteDiag.llDataRecv  = ntoh64(*pllValue);
                break;
            }
        }
        // point to the next location.
        pPayload += pDiagPayload->usLen + offsetof(BLRUDP_IO_DIAGRESP_PAYLOAD , Value);
        ASSERT(pPayload <= pEndPayload);
        if (pPayload > pEndPayload)         // end of the line, but for the WRONG
            break;                          // reason....
    }
    return;
Fail:
    return;
}
///////////////////////////////////////////////////////////
void BlrudpActiveRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext, 
                          BLRUDP_IO_PKT *pRecvPkt )
//
// process a receive packet by a Active context.
//
// inputs:
//    pContext -- active context to process this for.
//    pRecvPkt -- received packet..
// outputs:
//    none.
{
    uint16_t usCmd;

    BLRUDP_PKT_HDR *pPktHdr;

    pPktHdr = BlrudpGetPktPayload(pRecvPkt);
    

        
    // dbgPrintf("<%d> BlrudpActiveRecvPkt(%x,%ld, %ld, %ld)\n", 
    //         pContext->ulContextId,
    //         htons(pPktHdr->usCmd),
    //         htonl(pPktHdr->ulSseq),
    //         htonl(pPktHdr->ulRseq),
    //         htonl(pPktHdr->ulSync));

    ASSERT(pContext);
    if (pContext == NULL)
        return;

    usCmd = ntohs(pPktHdr->usCmd);

    // do we need to check the check sum??????
    if (usCmd & BLRUDP_CMD_CHECKSUM)
    {
        unsigned long ulChecksum;
        ulChecksum = BlrudpXsumPkt(pPktHdr,
                                   ntohs(pPktHdr->usDlen));
        if (ulChecksum != 0)    // checksum NG, drop it...
            FAIL;               // ignore the packet...
    }

    pContext->pCurrRcvPkt = pRecvPkt;
    ASSERT(pRecvPkt->link.next == NULL);
    if (pContext->pStateTbl)
        (*pContext->pStateTbl->pfnRecvPacket)(pContext,
                                              pRecvPkt);

    //
    // if no one took responsibility for the packet
    // return it to the free pool...
    //
    if (pContext->pCurrRcvPkt)
    {
        BlrudpIoPktPutRecvPkt(pContext->pPktPool,      // put the packet back into the pool...
                         pContext->pCurrRcvPkt);    
        pContext->pCurrRcvPkt = NULL;
    }
    return;
Fail:
    BlrudpIoPktPutRecvPkt(pContext->pPktPool,      // put the packet back into the pool...
                     pRecvPkt);    
    return;
}

//////////////////////////////////////////////////////////////////
int BlrudpCombineRemoteOptions(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// combine the local and remote options.
//
// inputs:
//    pContext -- context to combine the options in.
// outputs::
//   returns -- BLERR_SUCCESS if successful.
//
{
    int nErr = BLERR_INTERNAL_ERR;

    //
    // possibly overriden by remote options below...
    //
    BlrudpSelectMtuSize(pContext);


    //
    // Merge the remote options with our options...
    //
    if (pContext->RemoteOptions.ulWindowSize)
    {
        pContext->Options.ulWindowSize = min(pContext->Options.ulWindowSize,
                                             pContext->RemoteOptions.ulWindowSize);
    }
    //
    // if the otherend wants it off, then we will do it also.
    //
    if (pContext->RemoteOptions.bChecksum != (uint8_t)-1)
    {
        if (!pContext->RemoteOptions.bChecksum)
            pContext->Options.bChecksum = FALSE;
    }

    if (pContext->RemoteOptions.ulMtuSize)
    {
        pContext->Options.ulMtuSize = min(pContext->Options.ulMtuSize,
                                          pContext->RemoteOptions.ulWindowSize);
    }
    //
    // Calculate the number of packets we need based on the MTU size
    // and window size.
    //
    ASSERT((pContext->RemoteOptions.ulWindowSize) && 
           (pContext->RemoteOptions.ulMtuSize >= 1024));
    if ((pContext->RemoteOptions.ulWindowSize <= 0) || 
        (pContext->RemoteOptions.ulMtuSize < 1024))
        FAILERR(nErr, BLERR_BLRUDP_INVALID_OPTIONS);

    return(BLERR_SUCCESS);

Fail:
    return(nErr);
}    

///////////////////////////////////////////////////////////////////////////////
int BlrudpActCtxInit(BLRUDP_ACTIVE_CONTEXT *pContext,
                            unsigned long ulNumXmtPkts,
                            unsigned long ulNumRcvPkts,
                            int bAllocVectors)
//
// Perform the balance of the active context initialization which is common
// to all places where we create an active context..
//
// This allocates a new packet pool, initializes the timers
// and inserts this into the various lists....
//
//
// inputs:
//    pContext -- pointer to context to handle common initialization.
//    ulNumXmtPkts -- number of xmit packets to allocate in packet pool.
//    ulNumRcvPkts -- number of receive packets to allocate in packet pool.
//    bAllocVectors -- set to true if we should allocate the packet vectors
//                     to manage active transmissions...
//
//
// outputs:
//    returns -- BLERR_SUCCESS if successful.
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    BLRUDP_MGR *pBlrudp = pContext->pBlrudp;


    ASSERT(pContext->Options.ulMtuSize);                // at this point this should NEVER be zero....


    // make sure we don't have any pool in here....
    BlrudpActCtxDumpPktPool(pContext);
    //
    //
    ASSERT(pContext->pLocalSap);
    pContext->pPktPool = 
        BlrudpIoPktPoolNew(&pBlrudp->pPktPool,             // pBlrudp
                           pContext->pLocalSap->pLocalNic, // pLocalNic
                           pContext->ulContextId,           // ulContextId
                           ulNumXmtPkts,                   // ulNumXmtPkts
                           ulNumRcvPkts,                   // ulNumRcvPkts
                           (uint16_t)pContext->Options.ulMtuSize);  // usSizePkt

    if (pContext->pPktPool == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);

    if (bAllocVectors)          // allocate the vectors to manage an active state if called for...
    {
        nRet = BlrudpPktVectorsAlloc(pContext,
                                     ulNumXmtPkts,
                                     ulNumRcvPkts);
        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, nRet);
    }
    pContext->usXmitFlags = 0;      // make sure we start without the END CLOSE or WCLOSE
                                    // flags set...

    memset(&pContext->LocalDiag, 0, sizeof(pContext->LocalDiag));
    memset(&pContext->RemoteDiag, 0, sizeof(pContext->RemoteDiag));

    pContext->LocalDiag.bDiagValid = TRUE;          // local diag is valid
                                                    // starts at zero...

    /////////////////////////////////////////////////////////////////////////
    //
    // put us in the list of active contexts to service.
    // only if we are not already in the list...
    //
    if (pContext->Hdr.link.next == NULL)
    {
        //
        // Now we are ready to insert this into the active context map.
        // we use the sap as the index and the context as the item.
        //
        nRet = BlMapInsert(pBlrudp->mapActCtxs,     // pBlMap
                           &pContext->SapPair,      // pKey
                            pContext);              // pItem

        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, nRet);                               

        EnQ(&pContext->Hdr.link, &pBlrudp->listActCtxs);
    }

    return(BLERR_SUCCESS);

Fail:
    return(nErr);
}


////////////////////////////////////////////////////////////////////////////////
void BlrudpSelectMtuSize(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Select the mtu size from either the default local sap or the
// options specified by the user.
//
// inputs:
//     pcontext -- active context to select the MTU size for.
// outputs:
//     none.
//
{
    //
    // pick up the smaller of the MTU size options, the local sap or the options selected
    // by the user.
    ASSERT(pContext->pLocalSap);
    if (pContext->pLocalSap)
    {
        if (pContext->Options.ulMtuSize)
        {
            pContext->Options.ulMtuSize = min(pContext->Options.ulMtuSize, 
                                              pContext->pLocalSap->uMtuSize);
        }
        else
        {
            pContext->Options.ulMtuSize = pContext->pLocalSap->uMtuSize;
        }
    }

}


//////////////////////////////////////////////////////////////////
void BlrudpActCtxDumpPktPool(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// dump the packet pool associated with this active context.
//
// inputs:
//    pContext -- context to dump pool from.
// outputs:
//    none.
//
{
    //
    // put these packets back into the send pool..
    //
    if (pContext->pPktPool)
    {
        BlrudpActCtxReclaimPkts(pContext);
        BlrudpIoPktPoolRelease(pContext->pPktPool);
        pContext->pPktPool = NULL;
    }


}

///////////////////////////////////////////////////////////////////
void BlrudpActCtxReclaimPkts(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Reclaim the packets from send and to send pkts into
// the packet pool.
//
// inputs:
//    pContext -- context to reclaim packets for.
// outputs:
//    none.
{
    #if 0
    //
    // RB.. 10/2/01.. Not sure if this is valid.  packets
    // on the reXmit queue are also on the XmitPktVector. so
    // they get put back onto the send queue below....
    //
    BlrudpIoPktMoveToSendPool(pContext->pPktPool, 
                              &pContext->listRexmitPkts);
    #endif
    BlrudpIoPktMoveToSendPool(pContext->pPktPool, 
                              &pContext->listClientSendPkts);
    pContext->ulClientSendPktCount = 0;

    BlrudpIoPktMoveVectorToSendPool(pContext->pPktPool, 
                                    &pContext->XmitPktVector);

    BlrudpIoPktMoveVectorToRecvPool(pContext->pPktPool, 
                                    &pContext->RecvPktVector);
}

/////////////////////////////////////////////////////////////////////////////
void BlrudpStAddErrorMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext,
                              int32_t nErrCode)
//
// Set the context state to send an error and schedule the work.
//
// inputs:
//     pCotnext --context to add the error message to.
//     nErrCode -- error code to send.
// outputs:
//     none.
// 
{
    // NOTE: currently there is only one place to stuff this message
    // so the last call to this prior to the work getting executed
    // will be the one sent.
    pContext->ulWorkFlags |= BLRUDP_WORK_SENDERR;
    pContext->lXmtErrCode = nErrCode;
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
}


////////////////////////////////////////////////////////////////////////
void BlrudpStAddTctrlMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Add a Tctrl message to the work to do..
//
//
// inputs:
//     pCotnext --context to add the error message to.
// outputs:
//     none.
// 
{
    pContext->ulWorkFlags |= BLRUDP_WORK_SENDTCTRL;
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
}

////////////////////////////////////////////////////////////////////////
void BlrudpStAddCtrlMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Add a Tctrl message to the work to do..
//
//
// inputs:
//     pCotnext --context to add the error message to.
// outputs:
//     none.
// 
{
    pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
}

////////////////////////////////////////////////////////////////////////
void BlrudpStAddFirstMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Add a First message to the work to do..
//
//
// inputs:
//     pContext --context to add the work message to.
// outputs:
//     none.
// 
//
{
    pContext->ulWorkFlags |= BLRUDP_WORK_SENDFIRST;
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
}

////////////////////////////////////////////////////////////////////////
void BlrudpStAddResetMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext,
                               uint32_t ulSseq)
//
// Add a reset message to the work to do..
//
// inputs:
//     pContext -- context to add the work message to.
//     ulSseq -- sequence number to use in message.
// outputs:
//     none.
// 
{
    pContext->ulWorkFlags |= BLRUDP_WORK_SENDRESET;
    pContext->ulXmtResetSeq = ulSseq;                   
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
}
//////////////////////////////////////////////////////////////////
void BlrudpActCtxClearTimers(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Clear the timers from this context.
//
// inputs:
//    pcontext -- context to clear the timers from.
// outputs:
//    none.
//
{
    BLTIME_MGR *pTimeMgr = pContext->pBlrudp->pTimeMgr;
    if (pTimeMgr == NULL)         // time manager not yet created
        return;                     // punt.
    //
    // Unhook any timers, if we have any.
    //
    BltwTimerRemove(pTimeMgr,                      // pTimeMgr
                  &pContext->Timers.WTimer);       // pTimer


    BltwTimerRemove(pTimeMgr,                      // pTimeMgr
                  &pContext->Timers.CTimer);       // pTimer
    BltwTimerRemove(pTimeMgr,                      // pTimeMgr
                  &pContext->Timers.CTimeout);     // pTimer
    BltwTimerRemove(pTimeMgr,                      // pTimeMgr
                  &pContext->Timers.RTimer);       // pTimer
}

//////////////////////////////////////////////////////////////////////////
int BlrudpIsSseqValid(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_PKT_HDR *pPktHdr)
//
// Check to see if the Send sequence number in the packet is valid
// for this session.
//
// inputs:
//    pContext -- pContext to check this against
//    pPktHdr -- packet header containing the sequence number to check.
// outputs:
//    returns -- TRUE if the sequence number is valid. FALSE if not.
//
////////////////////////////////////////////////////////////////////////
{
    uint16_t usCmd;
    uint32_t ulSseq;
    uint32_t ulAllocDelta;
    uint32_t ulSseqDelta;
    int bValid = FALSE;

    usCmd = ntohs(pPktHdr->usCmd) & BLRUDP_CMD_TYPE;
    ulSseq = ntohl(pPktHdr->ulSseq);

    // ulSseq   Sending sequence number for this packet (increments only for data packets).
    //          for Data packets this is the sequence number of the packet
    //          being transmitted.  For control packets, this is the number
    //          of the next data packet to be transmitted...

    // ulRseq   Receiving sequence number.  Highest in-sequence 
    //          number + 1 received by this SAP.

    // valid sequence numbers to receive for a data packet...
    //
    // RecvSeq.ulSeq >= XmitSeq.ulRseq && RecvSeq.ulSeq < XmitSeq.ulRseq+RecvSeq.ulAlloc
    //
    // valid sequence number for the control packet (NOT a data packet)...
    // 
    // RecvSeq.ulSeq >= XmitSeq.ulRseq && RecvSeq.ulSeq <= XmitSeq.ulRseq+RecvSeq.ulAlloc
    //                                                   ^
    //
    // Control packets are allowed to be equal....
    //

    // calculate the window size as a packet delta.
    ulAllocDelta = pContext->XmitSeq.ulAllocSeq - pContext->XmitSeq.ulRseq;
    ulSseqDelta = ulSseq - pContext->XmitSeq.ulRseq;

    if (usCmd == BLRUDP_CMD_TYPE_DATA)
    {
        if (ulSseqDelta < ulAllocDelta)
            bValid = TRUE;
    }
    else
    {
        if (ulSseqDelta <= ulAllocDelta)
            bValid = TRUE;
    }
    return(bValid);
}

////////////////////////////////////////////////////////////////////////
int BlrudpPktVectorAvail(BLRUDP_PKT_VECTOR *pVector)
//
// Check to see if there is room in the packet vector..
//
// inputs:
//     pVector -- pointer to the packet vector.
// outputs:
//     returns -- TRUE if there is room, FALSE if there is no room.
// 
{
    unsigned long ulSeqDelta;

    ulSeqDelta = pVector->ulEndSeq - pVector->ulBaseSeq;

    ASSERT(ulSeqDelta <= pVector->ulNumEntries);
    if (ulSeqDelta >= pVector->ulNumEntries)
        return(0);
    else
        return(pVector->ulNumEntries - ulSeqDelta);
}

////////////////////////////////////////////////////////////////////////
int BlrudpPktVectorsPktsToSend(BLRUDP_ACTIVE_CONTEXT *pContext,
                               BLRUDP_PKT_VECTOR     *pVector)

//
// check to see if we have packets to send from the xmit buffer.
//
// inputs:
//     pContext -- context associated with this vector.
//     pVector -- pointer to the packet vector.
// outputs:
//     returns -- returns the number of packets in the vector 
//                left to send...
//
{
    uint32_t ulSeqDelta;

    ulSeqDelta = pVector->ulEndSeq - pContext->XmitSeq.ulSseq;
    ASSERT( (ulSeqDelta & 0x80000000) == 0);
    if (ulSeqDelta & 0x80000000L)            // just in case....
        ulSeqDelta = 0;

    return(ulSeqDelta);
}

////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetVectorsPkt(BLRUDP_PKT_VECTOR     *pVector,
                                   uint32_t              ulSeq)
//
// Get the vector associated with the sequence number requested.
//
// inputs:
//     pContext -- context associated with this vector.
//     pVector -- pointer to the packet vector.
//     ulSeq -- sequence number to retrieve.
// 
// outputs:
//    returns -- pointer to packet at that sequence number OR
//               NULL if there is no packet at that sequence number.
//
//
{
    uint32_t  ulIndex;

    ulIndex = BlrudpPktVectorIndex(pVector, ulSeq);

    return(pVector->pvectPkts[ulIndex]);

}


///////////////////////////////////////////////////////////////////////
uint32_t BlrudpPktVectorIndex(BLRUDP_PKT_VECTOR     *pVector,
                              uint32_t              ulSeq)
//
// Calculate the vector index associated with this vector.
//
// inputs:
//     pVector -- pointer to the packet vector.
//     pPkt -- packet to add to the vector...
//     ulSeq -- sequence number to calulate the vector for.
// outputs:
//     returns -- the vector index...
//
// NOTE: may want to make this inline code....
{
    uint32_t ulIndex;

    ulIndex = (pVector->ulBaseIndex + (ulSeq - pVector->ulBaseSeq)) % 
                      pVector->ulNumEntries;

    return(ulIndex);
}


//////////////////////////////////////////////////////////////////////
void BlrudpPktVectorsPush(BLRUDP_ACTIVE_CONTEXT *pContext,
                          BLRUDP_PKT_VECTOR     *pVector,
                          BLRUDP_IO_PKT         *pPkt)
//                        
//
//  Append a packet to the end of this vector list.
//
// inputs:
//     pContext -- context associated with this vector.
//     pVector -- pointer to the packet vector.
//     pPkt -- packet to add to the vector...
// outputs:
//     none
//
{
    uint32_t ulIndex;
    ASSERT(pContext && pVector && pPkt && pVector->pvectPkts);
    if ((pContext == NULL) || 
        (pVector == NULL)  || 
        (pPkt == NULL)     || 
        (pVector->pvectPkts == NULL))
    {
        FAIL;
    }

    //
    // Hey, there is NO room here, this should not have been called...
    //
    ASSERT( (pVector->ulEndSeq - pVector->ulBaseSeq) < pVector->ulNumEntries);
    if ((pVector->ulEndSeq - pVector->ulBaseSeq) >= pVector->ulNumEntries)
        FAIL;

    ulIndex = BlrudpPktVectorIndex(pVector, pVector->ulEndSeq);

    ASSERT(pPkt->link.next == NULL);        // should not really be on a queue at this point...

    ASSERT(pVector->pvectPkts[ulIndex] == NULL);        //should not be anything here.

    pVector->pvectPkts[ulIndex] = pPkt;     // store the packet pointer.
    pPkt->ulSseq = pVector->ulEndSeq;       // save a quick version of this...

    pVector->ulEndSeq++;                    // next sequence number.

    return;
Fail:
    return;    
}

////////////////////////////////////////////////////////////////////////////////
int BlrudpPktVectorsAlloc(BLRUDP_ACTIVE_CONTEXT *pContext,
                          uint32_t nNumXmitPkts,
                          uint32_t nNumRecvPkts)
//
//
// allocate memory for packet vectors adequate to contain the number
// of packets indicated.
// 
// inputs:
//   pContext -- pointer to context to do alloc for
//   nNumXmitPkts   -- number of xmit packets to account for.
//   nNumRecvPkts   -- number of receive packets to account for.
// outputs:
//    returns -- BLERR_SUCCESS if successful.
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;

    
    
    BlrudpPktVectorsFree(pContext);         // make sure we don't leak...



    //
    // Do transmit packets...
    //
    pContext->XmitPktVector.ulBaseSeq = 0;
    pContext->XmitPktVector.ulEndSeq = 0;
    pContext->XmitPktVector.ulBaseIndex = 0;
    if (nNumXmitPkts)
    {
        size_t nSize;
        nSize = nNumXmitPkts * sizeof(BLRUDP_IO_PKT *);
        pContext->XmitPktVector.ulNumEntries = nNumXmitPkts;
        pContext->XmitPktVector.pvectPkts = 
            (BLRUDP_IO_PKT **)BlrAllocMem(nSize);
        memset(pContext->XmitPktVector.pvectPkts, 
               0,
               nSize);                       
        if (pContext->XmitPktVector.pvectPkts == NULL)
            FAILERR(nErr, BLERR_NO_MEMORY);
    }

    
    //
    // Do receive packets...
    //
    pContext->RecvPktVector.ulBaseSeq = 0;
    pContext->RecvPktVector.ulEndSeq = 0;
    pContext->RecvPktVector.ulBaseIndex = 0;



    if (nNumRecvPkts)
    {
        size_t nSize;
        nSize = nNumRecvPkts * sizeof(BLRUDP_IO_PKT *);
        pContext->RecvPktVector.ulNumEntries = nNumRecvPkts;
        pContext->RecvPktVector.pvectPkts = 
            (BLRUDP_IO_PKT **)BlrAllocMem(nSize);
        if (pContext->RecvPktVector.pvectPkts == NULL)
            FAILERR(nErr, BLERR_NO_MEMORY);
        memset(pContext->RecvPktVector.pvectPkts, 
               0,
               nSize);                       

    }
    else
    {
        pContext->RecvPktVector.ulNumEntries = 0;
        pContext->RecvPktVector.pvectPkts = NULL;
    }

    nRet =  BlrudpPktStatusAlloc(&pContext->RecvPktStatus, nNumRecvPkts);
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);


    //
    // Perform the initialization....    
    BlrudpPktStatusSetBaseSeq(&pContext->RecvPktStatus,      // synchronize our receive status
                              pContext->XmitSeq.ulRseq);     // vector

    BlrudpPktStatusSetEndSeq(&pContext->RecvPktStatus,      // synchronize our receive status
                             pContext->XmitSeq.ulRseq);
    return(BLERR_SUCCESS);
Fail:
    BlrudpPktVectorsFree(pContext);    
    return(nErr);

}

///////////////////////////////////////////////////////////////////////////////////
void BlrudpPktVectorsFree(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Free the packet vectors associated with the Bluelight Rudp manager.
//
// inputs:
//   pContext -- pointer to context to do alloc for
//   nNumXmitPkts   -- number of xmit packets to account for.
//   nNumRecvPkts   -- number of receive packets to account for.
// outputs:
//    returns -- none
//
{
    if (pContext->XmitPktVector.pvectPkts)
    {
        BlrFreeMem(pContext->XmitPktVector.pvectPkts);
        pContext->XmitPktVector.pvectPkts = NULL;
    }
    if (pContext->RecvPktVector.pvectPkts)
    {
        BlrFreeMem(pContext->RecvPktVector.pvectPkts);
        pContext->RecvPktVector.pvectPkts = NULL;
    }
    BlrudpPktStatusFree(&pContext->RecvPktStatus);
    

}


/////////////////////////////////////////////////////////////
void BlrudpUpdateReceiverRSeq(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Update the receiver RSEQ number...
//
// inputs:
//    pContext -- pointer to context to update.
// outputs:
//    none.
//
{
    BLRUDP_PKT_VECTOR      *pVector;
    uint32_t ulIndex;
    int bScheduleWork = FALSE;
    
    pVector = &pContext->RecvPktVector;     // use a shorthandl
    
    ASSERT(pContext->XmitSeq.ulRseq == pVector->ulBaseSeq);
    ASSERT(pContext->RecvSeq.ulNextInSeq == pVector->ulEndSeq);
    
    ulIndex = pVector->ulBaseIndex;

    while (pVector->ulBaseSeq != pVector->ulEndSeq)
    {
        BLRUDP_IO_PKT *pPkt = pVector->pvectPkts[ulIndex];
        ASSERT(pPkt);       // should NEVER have a gap...

        // if we have a gap or it is NOT delivered...
        if ((pPkt == NULL) || (! (pPkt->ulFlags &  BLRUDP_PKT_STATUS_DELIVERED)))
            break;
        
        ASSERT(pVector->ulBaseSeq == pPkt->ulSseq);
        pVector->pvectPkts[ulIndex] = NULL;
        BlrudpIoPktPutRecvPkt(pContext->pPktPool, pPkt);

        pVector->ulBaseSeq++;
        ulIndex++;
        bScheduleWork = TRUE;
        if (ulIndex >= pVector->ulNumEntries)
            ulIndex = 0;

    }
    pContext->XmitSeq.ulRseq = pVector->ulBaseSeq;
    pVector->ulBaseIndex = ulIndex; // reset the base index...


    // Now we can increase the allocation...
    pContext->XmitSeq.ulAllocSeq = pContext->XmitSeq.ulRseq + pContext->ulNumRecvWinPkts;          // set the initial allocation....



    if (bScheduleWork)
    {
        //////////////////////////////////////////////////////////
        // If we are not expecting any more packets, then we need
        // to notify the sender that we have advanced...
        //
        // this is the traffic that advances the end to end ack
        //
        if (pContext->RecvSeq.usFlags & BLRUDP_CMD_EOM)
        {
            //
            // TBD...we may be able to dispense with forcing a ctrl response...
            // Need to think this through...
            //
            pContext->usXmitFlags |= BLRUDP_CMD_SREQ;               
            pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;          // schecdule the work...
            BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                           &pContext->XmitWorkItem);
        }
        else
        {
            //
            // Check to see if we have advanced the allocation past a certain point.  If so then
            // we schedule work anyway.... even without the EOM, but we don't request an SREQ...
            // since if this is missed we will be getting another SREQ from the other end at some point...
            //
            //
            uint32_t ulAllocDelta;
            ulAllocDelta = (pContext->XmitSeq.ulAllocSeq - pContext->XmitSeq.ulLastAllocSeqSent);
            ASSERT((ulAllocDelta & ((int64_t)1L<<63)) == 0);           // should not happen...
            if (ulAllocDelta >= pContext->ulAllocRefreshInterval)
            {
                pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;          // schecdule the work...
                BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                               &pContext->XmitWorkItem);
            }
        }

    }
    //
    // Reflect this in the bit fields indicating what we can
    // ack and nak....
    BlrudpPktStatusSetBaseSeq(&pContext->RecvPktStatus, 
                              pContext->XmitSeq.ulRseq);
  
}

/////////////////////////////////////////////////////////////////////////
void BlrudpPutRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext, BLRUDP_IO_PKT *pPkt)
//
// Put a receive packet into the receive packet list for the
// client to pick-up.
//
// inputs:
//    pContext -- pointer to context.
//    pPkt -- pointer to packet to put.
// outputs:
//    none 
//
{
    pContext->ulRecvPktCount++;
    EnQ(&pPkt->link,&pContext->listRecvPkts);
}

/////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Retrieve a packet from the list of received packets..
// 
// inputs:
//    pContext -- pointer to context to update.
// outputs:
//    returns -- pointer to packet retrieved, or 0 if no packets..
//
{
    BLRUDP_IO_PKT *pPkt = NULL;

    if (QisFull(&pContext->listRecvPkts))
    {
        pPkt = (BLRUDP_IO_PKT *)QBegin(&pContext->listRecvPkts);
        ASSERT(pContext->ulRecvPktCount); 
        if (pContext->ulRecvPktCount)
            pContext->ulRecvPktCount--;
        DeQ(&pPkt->link);
    }
    return(pPkt);
}

/////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpPeekRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Get the top most packet from the list of received packets..
// but leave it on the queue.
//
// 
// inputs:
//    pContext -- pointer to context to update.
// outputs:
//    returns -- pointer to packet retrieved, or 0 if no packets..
//
/////////////////////////////////////////////////////////////////////////
{
    BLRUDP_IO_PKT *pPkt = NULL;

    if (QisFull(&pContext->listRecvPkts))
        pPkt = (BLRUDP_IO_PKT *)QBegin(&pContext->listRecvPkts);
    return(pPkt);

}

/////////////////////////////////////////////////////////////////
uint32_t BlrudpGetRecvPktCount(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// retrieve the count of waiting to be received.
//
// inputs:
//    
// inputs:
//    pContext -- pointer to context to update.
// outputs:
//    returns -- count of the packets in the receive queue.
//
{
    return(pContext->ulRecvPktCount);
}


///////////////////////////////////////////////////////////////////
void BlrudpSentSreq(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Anytime a send function SUCCESSFULLY sends a message with
// the SREQ bit set we get called here.
//
// This handles saving the sync, recording the time this was
// sent and starting the wtimer.
//
// inputs:
//    pContext -- context which just sent the packet..
// outputs:
//    none.
{
    // TBD..
    // 1.  Set the wtimer.
    BLTIME_MS llTimeout;

    ASSERT(pContext->ulWTimerFactor > 0);
    if (pContext->ulWTimerFactor == 0)
        pContext->ulWTimerFactor = 1;               // make sure we recover from this...

    //
    // Compute the timeout and max it out.  Don't want a 2 minute
    // wtimer value...
    llTimeout =  pContext->llWTimerTimeout*pContext->ulWTimerFactor;
    if (llTimeout > BLRUDP_MAX_WTIMER_TIMEOUT)
        llTimeout = BLRUDP_MAX_WTIMER_TIMEOUT;

    //
    // Make sure the timer is active, but don't reset it if it
    // already is running....
    //
    if (!BltwIsTimerActive(&pContext->Timers.WTimer))
    {
        BltwTimerAdd(pContext->pBlrudp->pTimeMgr,       // pTimeMgr
                     &pContext->Timers.WTimer,          // pTimer
                     llTimeout);                        // time value.
    }

    // increment to the next sync number.... (this is what we just sent...)
    // all subsequent packets will cary this also....
    pContext->ulXmitSync++;     


    
    // 2.  save the sync.
    // check to see if we are already timing a sync,
    // if so, don't start timing another.  We can sometimes send ctrl's
    // faster than than they can come back...
    if (pContext->XmitSreqSt.llXmitSyncTime == 0L)
    {
        pContext->XmitSreqSt.ulXmitSync = pContext->ulXmitSync;
    
        pContext->XmitSreqSt.llXmitSyncTime = BltwTimerNow();
    }
    //
    // once we put the SReq flag into a packet, we clear the flag
    // that told us to send an SREQ...
    //
    pContext->usXmitFlags &= (~BLRUDP_CMD_SREQ);
}
///////////////////////////////////////////////////////////////////////////
void BlrudpInitSreqState(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// initialize the SreqState variables for this context..
//
// inputs:
//    none
// outputs:
//    pContext -- pContext->XmitSreqSt initialized.
//
//
{
    pContext->XmitSreqSt.ulXmitSync = pContext->ulXmitSync;
    pContext->XmitSreqSt.llXmitSyncTime = 0;        // inactive timer.
    pContext->XmitSreqSt.llSRTT = pContext->Options.ulRndTripDelay;
    pContext->XmitSreqSt.llRTTV = 0;
    pContext->XmitSreqSt.bSyncHandshake = FALSE;
    pContext->XmitSreqSt.ulHandshakeSync = pContext->XmitSreqSt.ulXmitSync;
    pContext->XmitSreqSt.llHandshakeSyncTime = 0;        
    //
    // set the default wtimer value here...
    //
    pContext->llWTimerTimeout = pContext->XmitSreqSt.llSRTT;
    pContext->ulWTimerFactor = 1;

} 

////////////////////////////////////////////////////////////
void BlrudpUpdateSreqState(BLRUDP_ACTIVE_CONTEXT *pContext, 
                           uint32_t ulEcho)
//
// Update the sreq state.  This calculates the round trip delay
// and cancels the wtimer if it sees a match to the sreq packet
// that we sent out and started timing...
//
// inputs:
//    pContext -- context this pertains to.
//    ulEcho -- value of the echo field from the control packet.
// outputs:
//   none.
//
{
    int64_t llRtt;      // signed value for calculations.
    BLTIME_MS llXmitSyncTime;
    uint32_t ulXmitSync;
    uint32_t ulDelta;
    
    //
    // Sync matches the one we were timing, update the round trip timers...
    //
    //if (ulEcho != pContext->XmitSreqSt.ulXmitSync)
    //{
    //dbgPrintf("%s: ulEcho(%lx), ulXmitSync(%lx)\r\n", 
    //         pContext->pBlrudp->szHostName,
    //          ulEcho,
    //          pContext->XmitSreqSt.ulXmitSync);
    //}
    
    llXmitSyncTime = pContext->XmitSreqSt.llXmitSyncTime;       // pick up the starting values
    ulXmitSync = pContext->XmitSreqSt.ulXmitSync;               // to use.
    ////////////////////////////////////////////////////////////////////////
    // if we are in a synchronous handshake, check the value we were
    // expecting first, just in case it arrives REAL LATE...
    //
    if ((pContext->XmitSreqSt.bSyncHandshake)  && 
        (ulEcho == pContext->XmitSreqSt.ulHandshakeSync))
    {
        ASSERT(pContext->XmitSreqSt.llHandshakeSyncTime);
        if (pContext->XmitSreqSt.llHandshakeSyncTime)
        {
            llXmitSyncTime = pContext->XmitSreqSt.llHandshakeSyncTime;      // override the values...
            ulXmitSync = pContext->XmitSreqSt.ulHandshakeSync;
        }
    
    }

    ulDelta = ulEcho - ulXmitSync;              // any echo equal or > than the 
                                                // one the we are expecting
    if ((ulDelta & 0x80000000L) == 0)           // will be taken as the RTT...
    {
        pContext->nRetryCount = 0;
        BlrudpStopSyncHandshake(pContext);
        if (llXmitSyncTime)
        {
            // compute the round trip delay for this one...
            llRtt = BltwTimerNow() - llXmitSyncTime;
            if (llRtt <= 0)
                llRtt = 1;               // 1 millisecond is the minimum resoluition...

            pContext->XmitSreqSt.llXmitSyncTime = 0L;       // clear this out...
            pContext->XmitSreqSt.llHandshakeSyncTime = 0L;

            // remove the timer
            BltwTimerRemove(pContext->pBlrudp->pTimeMgr,       // pTimeMgr
                            &pContext->Timers.WTimer);         // pTimer

        
            ASSERT(llRtt > 0);
            if (llRtt > 0)      // make sure time did not go backwards...
            {
                int64_t llDelta;
                int64_t llSRTT;     // do some signed unsigned magic...
                int64_t llRTTV;

                llSRTT = pContext->XmitSreqSt.llSRTT;
                llRTTV = pContext->XmitSreqSt.llRTTV;

                llSRTT += ((llRtt - llSRTT ) / 8);

                ASSERT(llSRTT >= 0);
                //
                // this should not overflow unless we goof....
                //
                if (llSRTT <= 0)
                    llSRTT = BLRUDP_DEFAULT_WTIMER_TIMEOUT;        // recover anyway...
                
                llDelta = llRtt - llSRTT;
            
                if (llDelta < 0)
                    llDelta *= -1;
                llRTTV += ((llDelta - llRTTV) / 4);
                ASSERT(llRTTV >= 0);

                pContext->XmitSreqSt.llSRTT = llSRTT;
                pContext->XmitSreqSt.llRTTV = llRTTV;


                //
                // compute the current llWtimer value to use...
                //
                pContext->llWTimerTimeout = llSRTT + 2*llRTTV;
            
            }
        }   // if (pContext->XmitSreqSt.llXmitSyncTime...
    }   // if (ulEcho == ....
}

///////////////////////////////////////////////////////////////
void BlrudpStartSyncHandshake(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Start/continue a synchronous handshake.  A sync handshake starts by
// setting the Ctimeout timer and resetting the ulWTimerFactor to 
// 1.
//
// The Ctimeout is how long the sync handshake can be allowed to 
// continue.  If the timer actually expires, then the connection is
// taken down..
//
// Each subsequent wtimer while a sync handshake in
// in progress will double this factor to make the actual
// timeout longer.
//
// inputs:
//    pContext -- context this pertains to.
// outputs:
//   none.
//
{
    if (pContext->XmitSreqSt.bSyncHandshake)
    {
        if (pContext->ulWTimerFactor < 0xFFFF)
        {
            //pContext->ulWTimerFactor <<= 1;     // double the factor...
            pContext->ulWTimerFactor = (pContext->ulWTimerFactor*3)/2;      // 1.5 multiplier.
        }
        ////////////////////////////////////////////////////////////////////
        //
        // remember the sync we were expecting when this handshake started.
        // this way if it comes we use it in our Round trip time culations...
        //
        pContext->XmitSreqSt.ulHandshakeSync = pContext->XmitSreqSt.ulXmitSync;            
        pContext->XmitSreqSt.llHandshakeSyncTime = pContext->XmitSreqSt.llXmitSyncTime; 

        //dbgPrintf("%s: ulRetryCount(%ld), ulWTimerFactor(%ld), ulXmitSync(%lx)\r\n", 
        //     pContext->pBlrudp->szHostName,
        //     pContext->nRetryCount,
        //     pContext->ulWTimerFactor,
        //     pContext->ulXmitSync+1);
    }
    else
    {
        pContext->ulWTimerFactor = 1;
        // TBD.. get this from an options parameter...

        BltwTimerAdd(pContext->pBlrudp->pTimeMgr,       // pTimeMgr
                     &pContext->Timers.CTimeout,        // pTimer
                     BLRUDP_DEFAULT_CTIMEOUT);          // retry timeout.

        pContext->XmitSreqSt.bSyncHandshake = TRUE;     // remember this
    }
}

///////////////////////////////////////////////////////////////
void BlrudpStopSyncHandshake(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// If a sync handshake is in progress this shuts it down.
//
// This resets the ulWTimerFactor to 1 and stops the CTimeout.
//
// inputs:
//
// inputs:
//    pContext -- context this pertains to.
// outputs:
//   none.
//
{
    pContext->ulWTimerFactor = 1;
    BltwTimerRemove(pContext->pBlrudp->pTimeMgr,       // pTimeMgr
                    &pContext->Timers.CTimeout);       // pTimer

    pContext->XmitSreqSt.bSyncHandshake = FALSE;
}

