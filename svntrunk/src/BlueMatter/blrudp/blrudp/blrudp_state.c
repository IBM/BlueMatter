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
// Blue light reliable udp state handling functions.
//
//

#include "hostcfg.h"

#if HAVE_LINUX_IN_H
    #include "linux/in.h"
#endif

#include "BlErrors.h"
#include "blrudp_state.h"
#include "blrudp_common.h"
#include "blrudppkt_help.h"
#include "blrudp_pktpool.h"
#include "blrudp_pktstatus.h"
#include "dbgPrint.h"

#include "bldefs.h"



void BlrudpStGetCtrlParams(BLRUDP_ACTIVE_CONTEXT *pContext,
                               BLRUDP_PKT_HDR *pPktHdr);

void BlrudpStGetPktHdrValues(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_PKT_HDR *pPktHdr);

int BlrudpStSendDiagReq(BLRUDP_ACTIVE_CONTEXT *pContext);
int BlrudpStSendDiagResp(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStCheckRclose(BLRUDP_ACTIVE_CONTEXT *pContext, 
                         int bScheduleWork);

#if TEST_DROPPED_PKTS 
static int g_nDropPktCounter = 0;
#endif

//////////////////////////////////////////////////////////////////////
void BlrudpStFirstSentXmit(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// FirstSentState -- Xmit Work event
//
// This handles the Xmit Work event while in the FirstSent State.
//
// inputs:
//     pContext -- context to Do xmit work for.
// outputs:
//     none.
//
{
    int nRet;
    unsigned long ulWorkDone = 0;           // flags used to clear the work we actually did...

    // TBD.. do something here
    // dbgPrintf("BlrudpStFirstSentXmit\n");


    //
    // Reset takes prescident...
    //
    if (pContext->ulWorkFlags & BLRUDP_WORK_SENDRESET)
    {
        nRet = BlrudpSendResetPkt(pContext);        // attempt to resend...
        if (nRet != BLERR_SUCCESS)
            FAIL;
        ulWorkDone = BLRUDP_WORK_SENDRESET;

    }
    else
    if (pContext->ulWorkFlags & BLRUDP_WORK_SENDFIRST)      // followed by first packet...
    {
        nRet = BlrudpSendFirstPkt(pContext);        // attempt to resend...
        if (nRet != BLERR_SUCCESS)
            FAIL;
        ulWorkDone = BLRUDP_WORK_SENDFIRST;
    }
    else
    {
        ASSERT(0);
        pContext->ulWorkFlags = 0;      // clear this out, so we don't continue forever.
        FAIL;
    }


    //
    // check off the work we completed...
    //    
    pContext->ulWorkFlags &=  ~ulWorkDone;
    if (pContext->ulWorkFlags)          // still more work to do???
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,       // schedule it...
                       &pContext->XmitWorkItem);
    }
    
    return;
Fail:
    if (pContext->ulWorkFlags)          // still more work to do???
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }
}
//////////////////////////////////////////////////////////////////////
void BlrudpStFirstSentRecv(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED,
                           BLRUDP_IO_PKT *pRecvPkt)
//
// FirstSentState -- receive packet event.
//
// This handles the receive packet event while in the FirstSent State.
//
// inputs:
//     pContext -- context to receive the packet on.
// outputs:
//     none.
//
{
    uint16_t usCmd;
    uint16_t usFlags;
    BLRUDP_PKT_HDR *pPktHdr;


    // dbgPrintf("BlrudpStFirstSentRecv\n");

    pPktHdr = BlrudpGetPktPayload(pRecvPkt);

    usCmd = ntohs(pPktHdr->usCmd);
    usFlags = usCmd & (~BLRUDP_CMD_TYPE);       // extract the flags and commands...
    usCmd = usCmd & BLRUDP_CMD_TYPE;



    //
    // first check for an error packet.  
    //
    if (usCmd == BLRUDP_CMD_TYPE_ERROR)
    {
        //
        // for now, only decode the FIRST error and if it is the conection established
        // then abort the connection.
        //
        // dbgPrintf("usCmd == BLRUDP_CMD_TYPE_ERROR\n");
        BlrudpStAddResetMsgToWork(pContext, ntohl(pPktHdr->ulRseq));
        BlrudpStAddFirstMsgToWork(pContext);

    }
    else
    if (pContext->XmitSeq.ulSseq == ntohl(pPktHdr->ulRseq))        // has to match, or punt...
    {                           // this tells us that this is the packet we just sent....

        pContext->XmitSeq.ulRseq = ntohl(pPktHdr->ulSseq);      // pick up the send sequence
                                                                // to echo...


        //
        // If this is NOT a TCTRL packet and it is receive seq number does not match
        // our send seq then drop it on the floor....  it is not the response we are looking for.
        //
        if (usFlags & BLRUDP_CMD_END)       // this is a REJECT....
        {
            // dbgPrintf("Reject Packet detected\n");
            BlrudpSetZombieState(pContext,
                                 BLERR_BLRUDP_CONNECT_REJECT);

        }
        else            // if not a reject then it has to be a TCTRL.
        {
            // Getting this probably means a half open connection with
            // a sequence number that is similar enough to our own..
            if (usCmd != BLRUDP_CMD_TYPE_TCTRL)
            {
                //dbgPrintf("FirstSentRecv: got ctrl msg\n");
                BlrudpStAddResetMsgToWork(pContext, ntohl(pPktHdr->ulSseq));
                // The other end matches our sequence number exactly.
                // must get us off top dead center otherwise
                // this will not work..
                BlrudpSetInitialSeqNumbers(pContext);
                BlrudpStAddFirstMsgToWork(pContext);
            }
            else 
            {
                //
                // TBD.. decode the packet and the paramters
                // the correct response is a CTRL packet...  Same Sequence
                // numbers and Sync++...
                //
                // dbgPrintf("Accept Packet detected\n");


                // extract the packet header values we want to keep as state.
                BlrudpStGetPktHdrValues(pContext, pPktHdr);
                BlrudpStGetCtrlParams(pContext, pPktHdr);
                BlrudpActiveRecvAcceptPkt(pContext, pRecvPkt );
            }
        }
    }


    return;
}

//////////////////////////////////////////////////////////////////////
void BlrudpStFirstSentWTimer(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// FirstSentState -- WTimer event.
//
// This handles the WTimer event while in the FirstSent State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    //dbgPrintf("BlrudpStFirstSentWTimer: %s\n", pContext->pBlrudp->szHostName);

    BlrudpStAddFirstMsgToWork(pContext);        // schedule a resend...

}

//////////////////////////////////////////////////////////////////////
void BlrudpStFirstSentCTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// FirstSentState -- CTimer event.
//
// This handles the CTimer event while in the FirstSent State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStFirstSentCTimer\n");
    // TBD.. do something here

}

////////////////////////////////////////////////////////////////////
void BlrudpStFirstSentCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// FirstSentState -- CTimeout event.
//
// This handles the CTimeout event while in the FirstSent State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStFirstSentCTimeout\n");
    // TBD.. do something here
    //
    // If this puppy goes off it means the other end has stopped responding.
    // just down the connection.
    pContext->usXmitFlags |= BLRUDP_CMD_END;                // indicate that we are at the end...
    BlrudpSetZombieState(pContext,                          // pContext
                         BLERR_BLRUDP_CONNECT_TIMEOUT);     // nBlError

}

///////////////////////////////////////////////////////////////
void BlrudpStFirstSentRTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// FirstSentState -- RTimer event.
//
// This handles the CTimer event while in the FirstSent State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStFirstSentRTimer\n");
    // TBD.. do something here

}


//////////////////////////////////////////////////////////////////////
void BlrudpStFirstRecvXmit(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// FirstRecevState -- Xmit Work event
//
// This handles the Xmit Work event while in the First Received State.
//
// inputs:
//     pContext -- context to Do xmit work for.
// outputs:
//     none.
//
{
    BLRUDP_IO_PKT *pPkt;
    int nRet;
    int bRetryTimer = FALSE;
    unsigned long ulWorkDone = 0;           // flags used to clear the work we actually did...
                                        // that is if the work went through....

    // dbgPrintf("BlrudpStFirstRecvXmit\n");

    //
    // Get a packet to send from the free pool.....
    //
    pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);     
    ASSERT(pPkt);        // HUH????                                                               

    if (pPkt == NULL)
        FAIL;                   // should not of happened...
    //
    // Work in the first receive state is to send the Error or TCTRL packet...
    //
    if (pContext->ulWorkFlags & BLRUDP_WORK_SENDERR)
    {
        BlrudpFillErrorPkt(pContext,                // pContext, 
                           pPkt,                    // pPkt,             
                           pContext->lXmtErrCode,   // ulErrCode,              
                           0);                      // ulErrValue)             
        ulWorkDone = BLRUDP_WORK_SENDERR;

    }
    else
    if (pContext->ulWorkFlags & BLRUDP_WORK_SENDTCTRL)
    {
        BlrudpFillTCtrlPkt(pContext,        // Fill out the Tctrl packet     
                           pPkt);           //                               
        ulWorkDone = BLRUDP_WORK_SENDTCTRL;
        bRetryTimer = TRUE;
    }
    else
    {
        ASSERT(0);                      // huh????
        pContext->ulWorkFlags = 0;      // clear this out, so we don't continue forever.
        FAIL;
    }

    //
    // Send it back to the receiver                                                  
    //
    nRet = BlrudpSendCtrlPkt(pContext,                       // pContext
                              (BLRUDP_IO_PKT *)pPkt);        // pPkt
    ASSERT(nRet == BLERR_SUCCESS);
    if (nRet == BLERR_BLRUDP_SEND_BUSY)     // could not send
    {                                       // add a work item, try again later.
        FAIL;
    }

    pContext->ulWorkFlags &=  ~ulWorkDone;
    if (pContext->ulWorkFlags)          // still more work to do???
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }

    return;
Fail:
    if (pContext->ulWorkFlags)          // still more work to do???
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }
    return;
    
}

///////////////////////////////////////////////////////////////
void BlrudpStFirstRecvRecv(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_IO_PKT *pRecvPkt)

//
//
// FirstRecvState -- Receive event.
//
// This handles the Receive event while in the FirstRecv State.
//
// inputs:
//     pContext -- context to process event
//     pRecvPkt -- packet received.
// outputs:
//     none.
//
{
    uint32_t ulSseq;
    uint16_t usCmd;
    uint16_t usFlags;
    BLRUDP_PKT_HDR *pPktHdr;

    // dbgPrintf("BlrudpStFirstRecvRecv\n");

    pPktHdr = BlrudpGetPktPayload(pRecvPkt);

    usCmd = ntohs(pPktHdr->usCmd);
    usFlags = usCmd & (~BLRUDP_CMD_TYPE);       // extract the flags and commands...
    usCmd = usCmd & BLRUDP_CMD_TYPE;

    ulSseq = ntohl(pPktHdr->ulSseq);


    if (usCmd == BLRUDP_CMD_TYPE_FIRST)
    {
        //unsigned long ulIpAddr;
        //ulIpAddr = htonl(pContext->SapPair.LocalSap.ulIpAddr);
        //dbgPrintf("BLRUDP_CMD_TYPE_FIRST(%ld.%ld.%ld.%ld)\n",
        //         (ulIpAddr >> 24) & 0xff,
        //         (ulIpAddr >> 16) & 0xff,
        //         (ulIpAddr >>  8) & 0xff,
        //         (ulIpAddr >>  0) & 0xff);
        if (ulSseq == pContext->RecvSeq.ulSseq)          // same as the last one 
        {                                                // we received...
            // dbgPrintf("Duplicate FIRST\n");
            BlrudpStAddTctrlMsgToWork(pContext);
        }
        else
        {
            // dbgPrintf("new First (send error)\n");
            BlrudpStAddErrorMsgToWork(pContext,BLRUDP_WORK_SENDERR);
        }
    }
    else
    if (usCmd == BLRUDP_CMD_TYPE_DATA)
    {
        int bValidSeq;
        //
        // have to checdk the range of numbers here....
        //
        bValidSeq = BlrudpIsSseqValid(pContext,
                                      pPktHdr);

        if (bValidSeq)
        {
            // if this happens,then we missed the first control sequence...
            // signal we need another one....
            pContext->usXmitFlags |= BLRUDP_CMD_SREQ;               // response required
            pContext->ulWorkFlags |= BLRUDP_WORK_SENDTCTRL;          // next packet out is a control packet...
            BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                           &pContext->XmitWorkItem);


            pContext->ulRecvCtrlSync = ntohl(pPktHdr->ulSync);          // make sure we will accept the next ctrl info...

            // cancle the timeout...
            BltwTimerRemove(pContext->pBlrudp->pTimeMgr,       // pTimeMgr
                           &pContext->Timers.CTimeout);          // pTimer

            BlrudpSetActiveState(pContext,                  // pContext
                                 BLRUDP_CTX_STATE_ESTABLISHED,   // nState
                                 BLERR_SUCCESS);              // blnerr

            // if this was a data packet we also receive it...
            if ((usCmd == BLRUDP_CMD_TYPE_DATA) && (pContext->pStateTbl))
            {
                (*pContext->pStateTbl->pfnRecvPacket)(pContext,
                                                      pRecvPkt);
            }
        }
    }
    else
    {
        //
        // is it the expected number...  Must be exact...
        // 
        if (ulSseq != pContext->XmitSeq.ulRseq)     // no
            FAIL;                                   // punt...
        if (usFlags & BLRUDP_CMD_END)
        {                                           // remote end giving up???
            //
            // End flag..... and a matching sequence number
            // means the other end gave up...
            //
                // dbgPrintf("Connection abort received...\n");
                BlrudpSetZombieState(pContext,
                                     BLERR_BLRUDP_CONNECT_ABORT);

        }
        else                        // not an END..
        {                           // proceed with the connection...

            // control will place us into the established state.
            ASSERT(usCmd == BLRUDP_CMD_TYPE_CTRL);  
            if (usCmd != BLRUDP_CMD_TYPE_CTRL)
                FAIL;


            // extract the packet header values we want to keep as state.
            BlrudpStGetPktHdrValues(pContext, pPktHdr);
            BlrudpStGetCtrlParams(pContext, pPktHdr);

            // control packet received with the correct sequence
            // numbers, we are done...
            // cancle the timeout...
            BltwTimerRemove(pContext->pBlrudp->pTimeMgr,       // pTimeMgr
                         &pContext->Timers.CTimeout);          // pTimer
            BlrudpSetActiveState(pContext,                  // pContext
                                 BLRUDP_CTX_STATE_ESTABLISHED,   // nState
                                 BLERR_SUCCESS);              // blnerr

        }
    }

    //    else
    //      Drop the packet on the floor...
    // all other packet types, ignore for now....

    return;
Fail:
    return;
}

///////////////////////////////////////////////////////////////
void BlrudpStFirstRecvWTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// FirstRecvState -- WTimer event.
//
// This handles the WTimer event while in the FirstRecv State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    //dbgPrintf("BlrudpStFirstRecvWTimer: %s\n", pContext->pBlrudp->szHostName);

    //
    // resend the tctrl message.... we did not see a response here.
    // could be that the remote end did not get it, or
    // we did not receive it...
    //
    BlrudpStAddTctrlMsgToWork(pContext);

}

///////////////////////////////////////////////////////////////
void BlrudpStFirstRecvCTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// FirstRecvState -- CTimer event.
//
// This handles the CTimer event while in the FirstRecv State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStFirstRecvCTimer\n");
    // TBD.. do something here

}

///////////////////////////////////////////////////////////////
void BlrudpStFirstRecvCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext)
//
//
// FirstRecvState -- CTimeout event.
//
// This handles the CTimeout event while in the FirstRecv State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    //
    // We received a first packet, but no follow up CTRL packet
    // from the remote computer.  Assume either the remote computer
    // went away without completing the transatction, or this message
    // was spurious...
    //
    // dbgPrintf("BlrudpStFirstRecvCTimeout\n");


    //
    // If this puppy goes off it means the other end has stopped responding.
    // just down the connection.
    pContext->usXmitFlags |= BLRUDP_CMD_END;                // indicate that we are at the end...
    BlrudpSetZombieState(pContext,                          // pContext
                         BLERR_BLRUDP_CONNECT_TIMEOUT);     // nBlError

}

///////////////////////////////////////////////////////////////
void BlrudpStFirstRecvRTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// FirstRecvState -- RTimer event.
//
// This handles the RTimer event while in the FirstRecv State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStFirstRecvRTimer\n");
    // TBD.. do something here

}

//////////////////////////////////////////////////////////////////////
void BlrudpStEstabXmit(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// Established state-- Xmit Work event
//
// This handles the Xmit Work event while in the Established State.
//
// inputs:
//     pContext -- context to Do xmit work for.
// outputs:
//     none.
//
{
    unsigned long ulWorkDone = 0;           // flags used to clear the work we actually did...
    // TBD.. do something here
    // dbgPrintf("BlrudpStEstabXmit\n");

    //
    // TBD.. Send as many packets as we can while we are still
    // clear to send....
    // 
    // May be more pratical to place that in a function that
    // calls this or a subroutine that returns whether there is more work to do...
    //


    // 
    // check to see if we should set the RClose bit....
    BlrudpStCheckRclose(pContext,       // pContext
                        FALSE);         // bScheduleWork
    
    if (pContext->ulWorkFlags & BLRUDP_WORK_SENDERR)
    {
        BLRUDP_IO_PKT *pPkt;
        int nRet;

        pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);
        if (pPkt == NULL)               // time we don't get a packet.
            FAIL;

        BlrudpFillErrorPkt(pContext,                // pContext, 
                           pPkt,                    // pPkt,             
                           pContext->lXmtErrCode,   // ulErrCode,              
                           0);                      // ulErrValue)             
        //
        // attempt to send the packet...
        //
        nRet = BlrudpSendCtrlPkt(pContext,          // pContext
                                 pPkt);              // pPkt
        ASSERT(nRet == BLERR_SUCCESS);              // hey, how did this happen...
        if (nRet != BLERR_SUCCESS)
            FAIL;

        ulWorkDone = BLRUDP_WORK_SENDERR;

    }
    else
    if (pContext->ulWorkFlags & BLRUDP_WORK_SENDCTRL)
    {
        BLRUDP_IO_PKT *pPkt;
        int nRet;
        uint16_t usSreq;
        uint32_t ulNumDataPktsToSend;

        //
        // Send/Resend a control packet made up from the current state..
        //
        pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);
        ASSERT(pPkt);                   // there is a problem here if on the first
        if (pPkt == NULL)               // time we don't get a packet.
            FAIL;


        //
        // if there is no more data in the pipe, the notify the other end
        // of that fact with the EOM bit...
        ulNumDataPktsToSend = BlrudpPktVectorsPktsToSend(pContext, &pContext->XmitPktVector);
        if (( ulNumDataPktsToSend == 0) && (QisEmpty(&pContext->listRexmitPkts)))
            pContext->usXmitFlags |=  BLRUDP_CMD_EOM;
        else
            pContext->usXmitFlags &=  (~BLRUDP_CMD_EOM);    // otherwise make sure it is clear...

        // are we expecting a response....
        usSreq = pContext->usXmitFlags & BLRUDP_CMD_SREQ;       

        // if the NextInSeq != to the next highest expected packet.. then we have a gap...
        if (pContext->RecvSeq.ulNextInSeq != pContext->RecvSeq.ulSseq)
        {
             // dbgPrintf("<%ld> Sending ECNTL(%lx, %lx, %lx) .\n", 
             //           pContext->ulContextId, 
             //           pPkt->ulSseq, 
             //           pContext->ulXmitSync,
             //           pContext->XmitSeq.ulRseq);
            
            BlrudpFillEcntlPkt(pContext, pPkt);        // make the next packet a control packet.
        }
        else
        {
            // dbgPrintf("<%ld> Sending CNTL(%d, %x, %x) .\n", 
            //           pContext->ulContextId, 
            //           pPkt->ulSseq, 
            //           pContext->ulXmitSync,
            //           pContext->XmitSeq.ulAllocSeq);
            if ((pContext->nState == BLRUDP_CTX_STATE_CLOSING) && 
                ((pContext->usXmitFlags & BLRUDP_CMD_WCLOSE) == 0))
            {
                uint32_t ulNumDataPktsToSend;
                // CLOSING state.  If we have no more packets to send then...
                // set the WCLOSE bit in the packet..
                ulNumDataPktsToSend = BlrudpPktVectorsPktsToSend(pContext, &pContext->XmitPktVector);
                if (ulNumDataPktsToSend == 0)
                    pContext->usXmitFlags |=  (BLRUDP_CMD_WCLOSE | BLRUDP_CMD_SREQ);
            }
            else
            {
                BlrudpFillCtrlPkt(pContext, pPkt);        // make the next packet a control packet.
            }
        }
        

        //
        // attempt to send the packet...
        //
        nRet = BlrudpSendCtrlPkt(pContext,          // pContext
                                 pPkt);              // pPkt
        ASSERT(nRet == BLERR_SUCCESS);              // hey, how did this happen...
        if (nRet != BLERR_SUCCESS)
            FAIL;

        //
        // clear the send ctrl flag....
        //
        ulWorkDone = BLRUDP_WORK_SENDCTRL;


    }
    else
    if (pContext->ulWorkFlags &  BLRUDP_WORK_SENDDIAGREQ)
    {
        int nRet;
        nRet = BlrudpStSendDiagReq(pContext);
        ASSERT(nRet == BLERR_SUCCESS);              // hey, how did this happen...
        if (nRet != BLERR_SUCCESS)
            FAIL;
        //
        // clear the send ctrl flag....
        //
        ulWorkDone = BLRUDP_WORK_SENDDIAGREQ;
    }
    else
    if (pContext->ulWorkFlags &  BLRUDP_WORK_SENDDIAGRESP)
    {
        int nRet;
        nRet = BlrudpStSendDiagResp(pContext);
        ASSERT(nRet == BLERR_SUCCESS);              // hey, how did this happen...
        if (nRet != BLERR_SUCCESS)
            FAIL;
        ulWorkDone = BLRUDP_WORK_SENDDIAGRESP;
    }
    else
    {
        uint16_t usSreq;
        uint32_t ulNumDataPktsToSend;

        ulNumDataPktsToSend = BlrudpPktVectorsPktsToSend(pContext, &pContext->XmitPktVector);

        // are we expecting a response....
        usSreq = pContext->usXmitFlags & BLRUDP_CMD_SREQ;       


        //
        // here is where we transmit the  packets..
        // First perform retransmissions.  
        // If there are no retransmissions then send data .  
        // If this is the last bit of data, send a packet with
        // the SREQ bit set...
        //
        if (QisFull(&pContext->listRexmitPkts))
        {
            // hey, we gots re-transmissions...
            BLRUDP_IO_PKT *pPkt;
            int nRet;

            // dbgPrintf("Retransmit\n");

            //
            // Grab the packet to retransmit...
            //
            pPkt = (BLRUDP_IO_PKT *)QBegin(&pContext->listRexmitPkts);
            DeQ(&pPkt->link);

            /////////////////////////////////////////////////////////////////////////
            // *   If no packets in the send vector set the SREQ bit to generate
            //     a response and the EOM to notify the other end we have no more
            //     to say...
            if (( ulNumDataPktsToSend == 0) && (QisEmpty(&pContext->listRexmitPkts)))
                pContext->usXmitFlags |= (BLRUDP_CMD_SREQ | BLRUDP_CMD_EOM);
            else
                pContext->usXmitFlags &= (~BLRUDP_CMD_EOM); // we do have more so clear the EOM bit...


            if ((pContext->nState == BLRUDP_CTX_STATE_CLOSING) && (ulNumDataPktsToSend == 0))
                pContext->usXmitFlags |=  (BLRUDP_CMD_WCLOSE | BLRUDP_CMD_SREQ);


            // Replace sync, rSeq and flags field
            // *  Redo the checksum.
            BlrudpRefreshPktHdr(pContext, pPkt);

            // *  Attempt to transmit.
            nRet = BlrudpSendDataPkt(pContext, pPkt);
            ASSERT(nRet == BLERR_SUCCESS);              // hey, how did this happen...
            if (nRet != BLERR_SUCCESS)
                FAIL;
            pContext->LocalDiag.llNumRetrans++;        // count number of retransmissions...
        }
        else
        if (ulNumDataPktsToSend  > 0)
        {
            BLRUDP_IO_PKT *pPkt;
            int nRet;

            // dbgPrintf("Data Transmit....\n");

            // *.  Get the packet from the vector...
            pPkt = BlrudpGetVectorsPkt(&pContext->XmitPktVector,    // *pVector,  
                                       pContext->XmitSeq.ulSseq);   // ulSeq)                              
            // *.  Fill out the header and checksum the packet...

            if (ulNumDataPktsToSend <= 1)                       // last one out gets the SREQ....
            {
                pContext->usXmitFlags |= (BLRUDP_CMD_SREQ | BLRUDP_CMD_EOM);
                if (pContext->nState == BLRUDP_CTX_STATE_CLOSING) 
                    pContext->usXmitFlags |=  (BLRUDP_CMD_WCLOSE | BLRUDP_CMD_SREQ);
            }                
            else
                pContext->usXmitFlags &= (~BLRUDP_CMD_EOM); // we do have more so clear the EOM bit...
                

            BlrudpFillDataPktHdr(pContext, pPkt);

            // *.  Attempt to transmit...
            #if TEST_DROPPED_PKTS               // test code for dropped packets....
                g_nDropPktCounter++;
                if (g_nDropPktCounter < TEST_DROPPED_PKTS_INTERVAL)
                    nRet = BlrudpSendDataPkt(pContext, pPkt);
                else
                {
                    g_nDropPktCounter = 0;
                    nRet = BLERR_SUCCESS;
                }
            #else
                nRet = BlrudpSendDataPkt(pContext, pPkt);       // regular code....
            #endif


            ASSERT(nRet == BLERR_SUCCESS);              // hey, how did this happen...
            if (nRet != BLERR_SUCCESS)
                FAIL;

            // *.  If success, increment the ulSseq number....
            pContext->LocalDiag.llDataXmit += pPkt->usPktLen;       // keep track of packets sent...

            pContext->XmitSeq.ulSseq++;

        }   
        else
        {
            //
            // We transitioned with nothing to do here, so 
            // schedule the next work send a ctrl message...
            //
            if ((pContext->nState == BLRUDP_CTX_STATE_CLOSING) && 
                ((pContext->usXmitFlags & BLRUDP_CMD_WCLOSE) == 0))
            {
                pContext->usXmitFlags |=  (BLRUDP_CMD_WCLOSE | BLRUDP_CMD_SREQ);

                pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;
                ASSERT(ulWorkDone == 0);
                ulWorkDone &= (~BLRUDP_WORK_SENDCTRL);      
            }

        }

    }

    //
    // Clear any flags inditating the work we did, if any...
    //
    pContext->ulWorkFlags &= (~ulWorkDone);

    //
    // schedule work of there is more to do....
    if ((pContext->ulWorkFlags) || (QisFull(&pContext->listRexmitPkts)) || 
        (BlrudpPktVectorsPktsToSend(pContext, &pContext->XmitPktVector)))
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,       
                       &pContext->XmitWorkItem);
    }
    return;
Fail:
    //
    // If we failed for some reason, then there is definitly more work to do....
    //
    if ((pContext->ulWorkFlags) || (QisFull(&pContext->listRexmitPkts)) || 
        (BlrudpPktVectorsPktsToSend(pContext, &pContext->XmitPktVector)))
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);

    }

}

///////////////////////////////////////////////////////////////
void BlrudpStEstabRecv(BLRUDP_ACTIVE_CONTEXT *pContext,
                       BLRUDP_IO_PKT *pRecvPkt)

//
//
// Established State -- Receive event.
//
// This handles the Receive event while in the Established State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    uint16_t usFlags;
    uint16_t usCmd;
    uint32_t ulSseq;
    BLRUDP_PKT_HDR *pPktHdr;
    int bValidSeq;

    // dbgPrintf("BlrudpStEstabRecv\n");

    pPktHdr = BlrudpGetPktPayload(pRecvPkt);

    usCmd = ntohs(pPktHdr->usCmd);
    usFlags = usCmd & (~BLRUDP_CMD_TYPE);       // extract the flags and commands...
    usCmd = usCmd & BLRUDP_CMD_TYPE;


    //
    // First always gets a response.... even if its sequence numbers
    // don't match...
    // This error message is ignored if received in the established
    // state...
    //
    if (usCmd == BLRUDP_CMD_TYPE_FIRST)
    {
        // dbgPrintf("new First (send error)\n");
        BlrudpStAddErrorMsgToWork(pContext,BLRUDP_WORK_SENDERR);
        return;     // end of the line here.
    }

    bValidSeq = BlrudpIsSseqValid(pContext,
                                  pPktHdr);
    //
    // have to check the range of sequence numbers here....
    //

    if (!bValidSeq)
    {
        //dbgPrintf("Invalid Packet\n");
        return;
    }

    // extract the packet header values we want to keep as state.
    BlrudpStGetPktHdrValues(pContext, pPktHdr);



    //
    // Response requested????
    // set the flag to we send it out.....
    //
    if (usFlags & BLRUDP_CMD_SREQ)
    {               
        pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;          // schecdule the work...
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }

    //
    // This is an ABORT packet... 
    // put the connection out of its missery...
    if (usFlags & BLRUDP_CMD_END)       
    {
        // dbgPrintf("BLRUDP_CMD_END\n");
        int nErrCode;
        if (((usFlags & BLRUDP_CMD_ALL_CLOSED) == BLRUDP_CMD_ALL_CLOSED) && 
            ((pContext->usXmitFlags & BLRUDP_CMD_ALL_CLOSED) == BLRUDP_CMD_ALL_CLOSED))
        {
            nErrCode = BLERR_SUCCESS;
        }
        else
        {
            nErrCode = BLERR_BLRUDP_CONNECT_ABORT;
        }
        BlrudpSetZombieState(pContext,
                             nErrCode);
        return;     // end of the line.

    }

    //
    // if the remote end sends us a WCLOSE flag
    // immediatly transition us to the closing state...
    //
    if ((usFlags & BLRUDP_CMD_WCLOSE) && 
        ((pContext->nState != BLRUDP_CTX_STATE_CLOSING)))
    {
        BlrudpSetActiveState(pContext,                    // pContext
                             BLRUDP_CTX_STATE_CLOSING,    // nState
                             BLERR_SUCCESS);              // blnerr
        
    }



    // TBD.. not an end packet..
    // pick it appart.  Control or data..
    if (usCmd == BLRUDP_CMD_TYPE_DATA)
    {
        uint32_t ulSeqDelta;
        BLRUDP_PKT_VECTOR *pVector;

        //
        // Probably want to turn this into a subroutine...
        //

        // dbgPrintf("<%ld> Data packet(usCmd(%lx), usDlen(%lx), ulSync(%lx), ulSreq(%lx), ulRseq(%lx)\n",
        //           pContext->ulContextId,
        //           ntohs(pPktHdr->usCmd),
        //           ntohs(pPktHdr->usDlen),
        //           ntohl(pPktHdr->ulSync),
        //           ntohl(pPktHdr->ulSseq),
        //           ntohl(pPktHdr->ulRseq));

        pVector = &pContext->RecvPktVector;
        // these really should be the same here....
        ASSERT(pVector->ulBaseSeq == pContext->XmitSeq.ulRseq);


        // 
        // we can be full here, BECAUSE, this packet
        // may be a re-transmission...
        //   .. < ulNumEntries means we have room for another packet.
        //   .. <= ulNumEntries means we are FULL, retransmissions 
        //         only are valid.
        //
        ASSERT( (pVector->ulEndSeq - pVector->ulBaseSeq) <= pVector->ulNumEntries);

        #if 0
        if ((pVector->ulEndSeq - pVector->ulBaseSeq) > pVector->ulNumEntries)
        {
            dbgPrintf("<%d> ulBaseSeq(%lx), ulEndSeq(%lx), ulNumEntries(%lx)\n",
                      pContext->ulContextId,
                      pVector->ulBaseSeq,
                      pVector->ulEndSeq,
                      pVector->ulNumEntries);
        }
        #endif

        pRecvPkt->ulSseq = ulSseq = ntohl(pPktHdr->ulSseq);
        ulSeqDelta = ulSseq - pVector->ulBaseSeq;
        pRecvPkt->usPktLen  = ntohs(pPktHdr->usDlen);   // save the raw packet length...

        if ((ulSeqDelta & 0x80000000L) == 0)
        {
            //
            // an ASSERT here means that the other end sent
            // a sequence number past the allocation.
            ASSERT(ulSeqDelta < pVector->ulNumEntries);
            if (ulSeqDelta < pVector->ulNumEntries)
            {
                uint32_t ulIndex;
                ulIndex = BlrudpPktVectorIndex(pVector, ulSseq);

                // check if this is a duplicate.               
                if (pVector->pvectPkts[ulIndex] == NULL)
                {   
                    // not a duplicate????
                    // check off that we have received this packet....
                    BlrudpPktStatusSetRecvd(&pContext->RecvPktStatus, ulSseq);

                    // indicate the packet is not yet received...
                    pRecvPkt->ulFlags &= (~BLRUDP_PKT_STATUS_DELIVERED);
                
                    //
                    // if we hare receiving out of sequence numbers
                    // then we can put all values on the receive queue.
                    //
                    if ( ! pContext->Options.bInOrderDelivery)
                        BlrudpPutRecvPkt(pContext, pRecvPkt);

                
                    ASSERT(pRecvPkt== pContext->pCurrRcvPkt);
                    pVector->pvectPkts[ulIndex] = pRecvPkt;
                    pContext->pCurrRcvPkt = NULL;       // indicate we have taken responibility
                                                        // for this packet...

                    ASSERT(pContext->RecvSeq.ulNextInSeq == pVector->ulEndSeq);
                    // can we advance the in sequence number....
                    ulIndex = BlrudpPktVectorIndex(pVector, 
                                                   pContext->RecvSeq.ulNextInSeq );
                
                    ASSERT(pRecvPkt->link.next == NULL);
                    //
                    // Attempt to move the next in sequence upto
                    // the top received sequence number or until we see a gap...
                    // 
                    while (pContext->RecvSeq.ulNextInSeq != pContext->RecvSeq.ulSseq)
                    {
                        BLRUDP_IO_PKT *pPkt;
                        pPkt = pVector->pvectPkts[ulIndex]; // extract the packet
                        if (pPkt == NULL)
                            break;

                        //
                        // in-order delivery, then add packets to the receive list
                        // that are in-order...
                        if (pContext->Options.bInOrderDelivery)     // queue only in-order packets...
                            BlrudpPutRecvPkt(pContext, pPkt);
                        pContext->RecvSeq.ulNextInSeq ++;
                        ulIndex++;

                        ASSERT( (pVector->ulEndSeq - pVector->ulBaseSeq) < pVector->ulNumEntries);
                        if (ulIndex >= pVector->ulNumEntries)        // did it wrap..???
                            ulIndex = 0;


                    } 
                    pVector->ulEndSeq = pContext->RecvSeq.ulNextInSeq;

                    pContext->LocalDiag.llDataRecv += ntohs(pPktHdr->usDlen);   // keep incomming telemetry...

                }
                //
                // if we have receive packets            
                // make sure we have work scheduled...   
                //
                if (BlrudpGetRecvPktCount(pContext) > 0)      
                {                                          
                    BlrWorkAddItem(pContext->pBlrudp->pRecvWorkMgr,        // work work manager.
                                   &pContext->RecvWorkItem);               // work item to add.
                }
            }       
        }

        // data packet, receive it and process the numbers...

    }
    else
    {
        // dbgPrintf("<%ld> Control packet(usCmd(%lx), usDlen(%lx), ulSync(%lx), ulSreq(%lx), ulRseq(%lx)\n",
        //           pContext->ulContextId,
        //           ntohs(pPktHdr->usCmd),
        //           ntohs(pPktHdr->usDlen),
        //           ntohl(pPktHdr->ulSync),
        //           ntohl(pPktHdr->ulSseq),
        //           ntohl(pPktHdr->ulRseq));
        // control packet..
        // process and update sequence and allocation....
        BlrudpStGetCtrlParams(pContext, pPktHdr);

        //
        // The next step...
        //
        // 1.  Create code to process the Rseq and remove packets form
        // our transmit vector and callback the client app.
        //
        // 2. Create code Create NAK packets.
        //
        // 3.  Decode and process NAK packets....
        //  

        // Further processing..... what flavor of control.....
        switch(usCmd)
        {
            case BLRUDP_CMD_TYPE_CTRL    :
                // this is handled in the BlrudpStGetCtrlParams....
                break;
            case BLRUDP_CMD_TYPE_TCTRL   :
                // TBD.. We can change dynamic paramters here.
                break;
            case BLRUDP_CMD_TYPE_ECNTL   :
                BlrudpActiveRecvEcntlPkt(pContext,
                                         pRecvPkt,
                                         pPktHdr);
                break;
            case BLRUDP_CMD_TYPE_ERROR   :
                // TBD.. log the error here.
                break;
            case BLRUDP_CMD_TYPE_DREQ    :
                pContext->ulWorkFlags |= BLRUDP_WORK_SENDDIAGRESP;          // schecdule the work...
                BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                               &pContext->XmitWorkItem);
                break;
            case BLRUDP_CMD_TYPE_DRESP   :
                BlrudpActiveRecvDiagResp(pContext,
                                         pRecvPkt,
                                         pPktHdr);
                break;
        }
    }  // else ...TYPE_DATA...

    //
    // special processing for closing state...
    if (pContext->nState == BLRUDP_CTX_STATE_CLOSING) 
    {
        //
        // first, should we set the RCLOSE bit on our side... We send the RCLOSE
        // bit IF we have already received the WCLOSE bit from the otherside
        // AND our we have received all packets...
        BlrudpStCheckRclose(pContext,               // pContxt
                            TRUE);                  // bScheduleWork.
          
        if (((pContext->RecvSeq.usFlags & BLRUDP_CMD_ALL_CLOSED) == BLRUDP_CMD_ALL_CLOSED) && 
            ((pContext->usXmitFlags & BLRUDP_CMD_ALL_CLOSED) == BLRUDP_CMD_ALL_CLOSED))
        {
            BlrudpSetZombieState(pContext,
                                 BLERR_SUCCESS);

            pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;      // pump the END message out...
            BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                           &pContext->XmitWorkItem);
        }
    }


}

//////////////////////////////////////////////////////////////////////////////////////////////
void BlrudpStCheckRclose(BLRUDP_ACTIVE_CONTEXT *pContext, 
                         int bScheduleWork)
//
// check if we should send the Rclose flag...
//
// inputs:
//    pContext -- context pointer to send.
//    bScheduleWork -- TRUE if we should schedule work here.
//.
// outputs:
//    none.
{
    if ((pContext->RecvSeq.usFlags & BLRUDP_CMD_WCLOSE) &&     // did we receive a WCLOSE??
        (! (pContext->usXmitFlags & BLRUDP_CMD_RCLOSE)) )        // have we sent one???
    {
      
        if (pContext->XmitSeq.ulRseq == pContext->RecvSeq.ulSseq)      // are we caught up
        {
            //
            pContext->usXmitFlags |= (BLRUDP_CMD_RCLOSE |  BLRUDP_CMD_SREQ);
            if (bScheduleWork)
            {
                pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;      // pump the END message out...
                BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                               &pContext->XmitWorkItem);
            }
        }            
    }
}
///////////////////////////////////////////////////////////////
void BlrudpStEstabWTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Established State -- Wtimer event.
//
// This handles the WTimer event while in the Established State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    //dbgPrintf("BlrudpStEstabWTimer: %s: %lx\n", 
    //    pContext->pBlrudp->szHostName,
    //    pContext->llWTimerTimeout);

    // TBD..
    // 1.  If not in a sync handshake, start sync handshake.
    BlrudpStartSyncHandshake(pContext);
    pContext->nRetryCount++;        // keep a count...

    //
    // set the SREQ flag and schedule work....
    pContext->usXmitFlags |= BLRUDP_CMD_SREQ;
    // Actually sending this will set the wtimer...

    //
    // if there is nothing in the output queue, then force a control packet out
    // the door...

    if ((QisEmpty(&pContext->listRexmitPkts)) && 
        ( ! BlrudpPktVectorsPktsToSend(pContext, &pContext->XmitPktVector)))
    {
        pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;
    }
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,       
                   &pContext->XmitWorkItem);
    

}

///////////////////////////////////////////////////////////////
void BlrudpStEstabCTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Established State -- CTimer event.
//
// This handles the CTimer event while in the Established State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStEstabCTimer\n");
    

}

///////////////////////////////////////////////////////////////
void BlrudpStEstabCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Established State -- CTimeout event.
//
// This handles the CTimeout event while in the Established State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    //dbgPrintf("%s: BlrudpStEstabCTimeout, ulRetryCount(%ld), llWtimerfactor(%lx), llWtimeout(%llx)\n", 
    //         pContext->pBlrudp->szHostName,
    //          pContext->nRetryCount,
    //          pContext->ulWTimerFactor,
    //          pContext->llWTimerTimeout*pContext->ulWTimerFactor);
    //

    //
    // If this puppy goes off it means the other end has stopped responding.
    // just down the connection.
    pContext->usXmitFlags |= BLRUDP_CMD_END;                // indicate that we are at the end...
    BlrudpSetZombieState(pContext,                          // pContext
                         BLERR_BLRUDP_CONNECT_TIMEOUT);     // nBlError
}

///////////////////////////////////////////////////////////////
void BlrudpStEstabRTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Established State -- RTimer event.
//
// This handles the RTimer event while in the Established State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStEstabRTimer\n");
    // TBD.. do something here

}


//////////////////////////////////////////////////////////////////////
void BlrudpStClosingXmit(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// Closing State-- Xmit Work event
//
// This handles the Xmit Work event while in the Closing State.
//
// inputs:
//     pContext -- context to Do xmit work for.
// outputs:
//     none.
//
{
    BlrudpStEstabXmit(pContext);
}

///////////////////////////////////////////////////////////////
void BlrudpStClosingRecv(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED,
                           BLRUDP_IO_PKT *pRecvPkt UNUSED)

//
//
// Closing State -- Closing event.
//
// This handles the Receive event while in the Closing State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    BlrudpStEstabRecv(pContext, 
                      pRecvPkt);            // same as estab..
                                            // in the estab we look at the state for
                                            // special case....
}

///////////////////////////////////////////////////////////////
void BlrudpStClosingWTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Closing State -- Wtimer event.
//
// This handles the WTimer event while in the Closing State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    BlrudpStEstabWTimer(pContext);

}

///////////////////////////////////////////////////////////////
void BlrudpStClosingCTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Closing State -- CTimer event.
//
// This handles the CTimer event while in the Closing State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    //
    // if this goes of, terminate it.  it is taking WAY too long...
    //
    BlrudpSetZombieState(pContext,
                         BLERR_BLRUDP_CONNECT_ABORT);

    pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;          // schecdule the work...
    BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                   &pContext->XmitWorkItem);
}

///////////////////////////////////////////////////////////////
void BlrudpStClosingCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Closing State -- CTimeout event.
//
// This handles the CTimeout event while in the Closing State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    BlrudpStEstabCTimeout(pContext);

}

///////////////////////////////////////////////////////////////
void BlrudpStClosingRTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Closing State -- RTimer event.
//
// This handles the RTimer event while in the Closing State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    BlrudpStEstabRTimer(pContext);

}

//////////////////////////////////////////////////////////////////////
void BlrudpStZombieXmit(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
// Zombie state -- Xmit Work event
//
// This handles the Xmit Work event while in the Zombie State.
//
// inputs:
//     pContext -- context to Do xmit work for.
// outputs:
//     none.
//
{
    BLRUDP_IO_PKT *pPkt;
    int nRet;
    // dbgPrintf("BlrudpStZombieXmit\n");

    //
    // check to see if we have anythinhg to send or resend...
    //

    //
    // what we re-send depends on how we got here...
    //

    // Send/Resend a control packet made up from the current state..
    //
    pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);
    ASSERT(pPkt);                   // there is a problem here if on the first
    if (pPkt == NULL)               // time we don't get a packet.
    {
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,       // try again later...
                       &pContext->XmitWorkItem);
        return;
    }

    //
    // refill the packet...
    //      
    ASSERT(pContext->usXmitFlags & BLRUDP_CMD_END);         // make sure I notice something is wrong...

    pContext->usXmitFlags |= BLRUDP_CMD_END;            // make sure the END flag is set here...
    BlrudpFillCtrlPkt(pContext, pPkt);        // make the next packet a control packet.

    //
    // attempt to send the again packet...
    //
    nRet = BlrudpSendCtrlPkt(pContext,          // pContext
                             pPkt);              // pPkt
    ASSERT(nRet == BLERR_SUCCESS);              // hey, how did this happen...

    if (nRet == BLERR_BLRUDP_SEND_BUSY)     // could not send
    {                                       // add a work item, try again later.
                
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }

}

///////////////////////////////////////////////////////////////
void BlrudpStZombieRecv(BLRUDP_ACTIVE_CONTEXT *pContext ,
                        BLRUDP_IO_PKT *pRecvPkt )

//
//
// Zombie State -- Receive event.
//
// This handles the Receive event while in the Zombie State.
//
// inputs:
//     pContext -- context to process event
//     pRecvPkt -- received packet.
// outputs:
//     none.
//
{
    uint16_t usCmd;
    uint32_t ulSseq;
    uint32_t ulRseq;
    BLRUDP_PKT_HDR *pPktHdr;

    //dbgPrintf("BlrudpStZombieRecv\n");

    pPktHdr = BlrudpGetPktPayload(pRecvPkt);
    //
    // In the zombie state, anything we get here
    // is a result of the last packet we went not being received.
    //
    // Unless this is a FIRST packet where the sequence number
    // DOES NOT match the last sequence number received.....
    // 
    // In which case this is is treated the same as a passive receive
    // provided that we can find a listening context for it....
    //
    
    usCmd = (ntohs(pPktHdr->usCmd) & BLRUDP_CMD_TYPE);
    ulSseq = ntohl(pPktHdr->ulSseq);
    ulRseq = ntohl(pPktHdr->ulRseq);

    if (usCmd == BLRUDP_CMD_TYPE_FIRST)
    {
        //
        // May be able to find the packet to resend..
        // since it is an error packet, where should it come from...
        //
        if (ulSseq == pContext->RecvSeq.ulSseq)
        {
            BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                           &pContext->XmitWorkItem);
        }
        else
        {
            BLRUDP_PASSIVE_CONTEXT *pPassCtx = NULL;
            // TBD....
            // this is a NEW first packet..
            // so check to see if we have a listening context.
            // if so then process it the same passive context...
            // dbgPrintf("BlrudpStZombieRecv NEW first packet\n");

            //
            // some defensive coding.    These should NEVER be out of sync....
            //
            ASSERT(pContext->pLocalSap->Sap.ulIpAddr == pContext->SapPair.LocalSap.ulIpAddr);
            ASSERT(pContext->pLocalSap->Sap.nPort == pContext->SapPair.LocalSap.nPort);


            //
            // Check to see if we have any passive (listening)
            // contexts assocated with the local sap...
            pPassCtx = BlMapFindItem(pContext->pBlrudp->mapPassCtxs, &pContext->SapPair.LocalSap.ulIpAddr);
            if (pPassCtx)       // yes, indeedee, we have a passive context....
            {
                BlrudpPassiveRecvPkt(pPassCtx,                              // *pPassContext,       
                                     pContext,                              // *pExistingActContext,
                                     pRecvPkt,                              // *pRecvPkt,                    
                                     pContext->SapPair.RemoteSap.ulIpAddr,   // ulIpAddr,                     
                                     pContext->SapPair.RemoteSap.nPort);     // nPort )                      
            }
        }
    }


}



///////////////////////////////////////////////////////////////
void BlrudpStZombieWTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Zombie State -- WTimer event.
//
// This handles the WTimer event while in the Zombie State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStZombieWTimer\n");



}

///////////////////////////////////////////////////////////////
void BlrudpStZombieCTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Zombie State -- CTimer event.
//
// This handles the CTimer event while in the Zombie State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStZombieCTimer\n");


}

///////////////////////////////////////////////////////////////
void BlrudpStZombieCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext)
//
//
// Zombie State -- CTimeout event
//
// This handles the CTimeout event while in the Zombie State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStZombieCTimeout\n");
    //
    // Ctimout. we destroy the context....
    //
    BlrudpActCtxDelete(pContext->pBlrudp,
                              pContext);

}

///////////////////////////////////////////////////////////////
void BlrudpStZombieRTimer(BLRUDP_ACTIVE_CONTEXT *pContext UNUSED)
//
//
// Zombie State -- RTimer event.
//
// This handles the RTimer event while in the Zombie State.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     none.
//
{
    // dbgPrintf("BlrudpStZombieRTimer\n");
    // TBD.. do something here


}





///////////////////////////////////////////////////////////////////////////
void BlrudpStGetCtrlParams(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_PKT_HDR *pPktHdr)
//
// Transfer the packets control paramters into the receivers context information.
//
// This grabs both the allocation and the echo sync value.
//
// inptus:
//    pContext -- context this is is to be transferred to.
//    pPktHdr -- pointer to the control packet payload.
// outputs:
//    none.
{
    uint32_t ulSyncDelta;
    BLRUDP_IO_CTRL_PKT *pCtrlPkt = (BLRUDP_IO_CTRL_PKT *)pPktHdr;
    //
    // first check to see if this is a new or newer ctrl pack by checking its sync number.
    //
    ulSyncDelta = ntohl(pCtrlPkt->IoPkt.ulSync) - pContext->ulRecvCtrlSync;

    if ((ulSyncDelta & (1L<<31)) == 0)    // save off the data if this is newer.. or same
    {
        uint32_t ulAllocDelta;
        pContext->ulRecvCtrlSync = ntohl(pCtrlPkt->IoPkt.ulSync);

        ulAllocDelta = ntohl(pCtrlPkt->ulAllocSeq) - pContext->RecvSeq.ulAllocSeq;

        if ((ulAllocDelta & (1L<<31)) == 0)
            pContext->RecvSeq.ulAllocSeq = ntohl(pCtrlPkt->ulAllocSeq);
    }

    

    // 
    // record the highest echo value received...
    //
    ulSyncDelta = ntohl(pCtrlPkt->ulEcho) - pContext->ulRcvdEcho;
    if ((ulSyncDelta & (1L<<31)) == 0)
    {   
        pContext->ulRcvdEcho = ntohl(pCtrlPkt->ulEcho);

        BlrudpUpdateSreqState(pContext,                     // pContext
                              pContext->ulRcvdEcho);        // ulEcho
    }

}

/////////////////////////////////////////////////////////////////////////
void BlrudpStGetPktHdrValues(BLRUDP_ACTIVE_CONTEXT *pContext,
                             BLRUDP_PKT_HDR *pPktHdr)
//
// Extract common packet header values and record them in our
// state..  This is done for all packets received with Valid sequence numbers.
//
// inputs:
//    pContext -- context this is is to be transferred to.
//    pPktHdr -- pointer to the control packet payload.
// outputs:
//    none.
//
{
    uint32_t ulSyncDelta;
    uint32_t ulSreqDelta;
    uint32_t ulSeq;
    uint32_t ulSync;
    uint16_t usCmd;
    uint16_t usFlags;

    usFlags = ntohs(pPktHdr->usCmd) & (~BLRUDP_CMD_TYPE);
    usCmd = ntohs(pPktHdr->usCmd) & BLRUDP_CMD_TYPE;

    //
    // If this is newer or the same than any previously received value
    // then take its seq value.
    // 
    ulSeq = ntohl(pPktHdr->ulSseq);
    ulSync = ntohl(pPktHdr->ulSync);

    // for all control packets, this is 
    // the seq number of the NEXT packet 
    // that will be sent....
    // For data packets this is the seq number of this packet...

    if (usCmd == BLRUDP_CMD_TYPE_DATA)      // make this the next packet we are expecting....
        ulSeq++;

    //
    // repsonse requested????
    // This schedules work, even if received out of order....
    //
    if (ntohs(pPktHdr->usCmd) & BLRUDP_CMD_SREQ)
    {               
        pContext->ulWorkFlags |= BLRUDP_WORK_SENDCTRL;          // schecdule the work...
        BlrWorkAddItem(pContext->pLocalSap->pXmitWorkMgr,
                       &pContext->XmitWorkItem);
    }


    ulSreqDelta = ulSeq - pContext->RecvSeq.ulSseq;

    if ((ulSreqDelta & 0x80000000L) == 0)
    {
        uint32_t ulRseq;
        pContext->RecvSeq.ulSseq = ulSeq;           // next highest packet expected...
        BlrudpPktStatusSetEndSeq(&pContext->RecvPktStatus,      // reflect this in the status...
                                 ulSeq);


        ulRseq = ntohl(pPktHdr->ulRseq);
        ulSyncDelta = ulRseq - pContext->RecvSeq.ulRseq;
        if ((ulSyncDelta & 0x80000000L) == 0)
        {
            pContext->RecvSeq.ulRseq = ulRseq; 
            if (ulSyncDelta)        // it advanced, schedule work...
            {                       // this will eventually cause callbacks...
                BlrWorkAddItem(pContext->pBlrudp->pXmitDoneWorkMgr,
                               &pContext->XmitDoneWorkItem);
            }
        }
    }
    
    //
    // Now update our sync and flag status....
    //
    ulSync = ntohl(pPktHdr->ulSync);
    ulSyncDelta = ulSync - pContext->ulRecvSync;
    if ((ulSyncDelta & 0x80000000L) == 0)
    {
        pContext->ulRecvSync = ulSync;
        pContext->RecvSeq.usFlags = usFlags;        // save the last flags received....
    }


}


/////////////////////////////////////////////////////////////////
const char *BlrudpStateString(int nState)
//
// Map a context state to a context state string.
//
// inputs:
//    nState -- context state to map.
// outputs:
//    returns -- pointer to context state string.
//
{
    const char *pszState;

    switch (nState)
    {
        case BLRUDP_CTX_STATE_CLOSED      :  pszState = "CLOSED"; break;
        case BLRUDP_CTX_STATE_FIRST_SENT  :  pszState = "FIRST_SENT"; break;
        case BLRUDP_CTX_STATE_FIRST_RECV  :  pszState = "FIRST_RECV"; break;
        case BLRUDP_CTX_STATE_ESTABLISHED :  pszState = "ESTABLISHED"; break;
        case BLRUDP_CTX_STATE_CLOSING     :  pszState = "CLOSING"; break;
        case BLRUDP_CTX_STATE_ZOMBIE      :  pszState = "ZOMBIE"; break;
        default: pszState = "????unknown????"; break;
    }
    return(pszState);
}

//////////////////////////////////////////////////////////////////////////////
int BlrudpStSendDiagReq(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Send off a diagnostic request packet...
//
// inputs:
//    pContext -- context this is is to be transferred to.
//    pPktHdr -- pointer to the control packet payload.
// outputs:
//    returns -- BLERR_SUCCESS if successfull.
//
{
    int nErr = BLERR_INTERNAL_ERR;

    BLRUDP_IO_PKT *pPkt;
    int nRet;

    //
    // Send/Resend a control packet made up from the current state..
    //
    pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);
    ASSERT(pPkt);                   // there is a problem here if on the first
    if (pPkt == NULL)               // time we don't get a packet.
        FAILERR(nErr, BLERR_BLRUDP_NO_PKTS);

    // fill the packet out...
    BlrudpFillDiagReq(pContext, pPkt);

    // attempt to send the packet...
    //
    nRet = BlrudpSendCtrlPkt(pContext,          // pContext
                             pPkt);             // pPkt

    return(nRet);
Fail:
    return(nErr);
}
//////////////////////////////////////////////////////////////////////////////
int BlrudpStSendDiagResp(BLRUDP_ACTIVE_CONTEXT *pContext)
//
// Send off a diagnostic request response packet...
//
// inputs:
//    pContext -- context this is is to be transferred to.
//    pPktHdr -- pointer to the control packet payload.
// outputs:
//    returns -- BLERR_SUCCESS if successfull.
//
{
    int nErr = BLERR_INTERNAL_ERR;

    BLRUDP_IO_PKT *pPkt;
    int nRet;

    //
    // Send/Resend a control packet made up from the current state..
    //
    pPkt = BlrudpIoPktGetCtrlPkt(pContext->pPktPool);
    ASSERT(pPkt);                   // there is a problem here if on the first
    if (pPkt == NULL)               // time we don't get a packet.
        FAILERR(nErr, BLERR_BLRUDP_NO_PKTS);

    // fill the packet out...
    BlrudpFillDiagResp(pContext, pPkt);

    // attempt to send the packet...
    //
    nRet = BlrudpSendCtrlPkt(pContext,          // pContext
                             pPkt);              // pPkt

    return(nRet);
Fail:
    return(nErr);
}
