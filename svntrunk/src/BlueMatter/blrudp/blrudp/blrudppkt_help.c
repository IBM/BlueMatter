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
 


/////////////////////////////////////////////////////////////
//
// This file contains packet helper functions.  They are used
// to encode and decode blrudp packets.
//
//

#include "hostcfg.h"

#if HAVE_LINUX_IN_H
    #include "linux/in.h"
#endif

#include "blrudppkt.h"
#include "blrudp_st.h"
#include "blrudppkt_help.h"
#include "blrudp_pktstatus.h"
#include "dbgPrint.h"

void BlrudpFillPktHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_IO_PKT *pPkt,
                      BLRUDP_PKT_HDR *pPktHdr,
                      uint16_t usCmd);

void BlrudpChecksumPkt(BLRUDP_PKT_HDR *pPkt);
void BlrudpFillCtrlHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_IO_CTRL_PKT *pCtrlPkt);
void BlrudpFillCmdAndTCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                              BLRUDP_IO_PKT *pPkt,
                              uint16_t usCmd);


//////////////////////////////////////////////////////////////
void BlrudpDumpPktHex(BLRUDP_IO_PKT *pPkt UNUSED)
//
// dump the entire packet contents out the debug port in hex..
//
// inputs:
//    pPkt -- pointer to packet to dump.
// outputs:
//    dumped packet...
//
{
#ifdef _DEBUG
    // dump the packet payload out the debug port.
    BLRUDP_PKT_HDR *pPktHdr;
    int nLen;
    unsigned char *pData;
    int nLine;

    pPktHdr = BlrudpGetPktPayload(pPkt);

    nLen = ntohs(pPktHdr->usDlen);
    dbgPrintf("PktDump (len = %d)\r\n", nLen);

    pData = (unsigned char *)pPktHdr;
    nLine= 0;
    while (nLen >= 16)
    {
        dbgPrintf("%2d: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
                ++nLine,
                pData[0],
                pData[1],
                pData[2],
                pData[3],
                pData[4],
                pData[5],
                pData[6],
                pData[7],
                pData[8],
                pData[9],
                pData[10],
                pData[11],
                pData[12],
                pData[13],
                pData[14],
                pData[15]);
        pData += 16;
        nLen -= 16;
    }
    if (nLen)
    {
        while(nLen > 4)
        {
           dbgPrintf("%02x %02x %02x %02x \r\n",
                pData[00],
                pData[01],
                pData[02],
                pData[03]);
            pData += 4;
            nLen -= 4;
        }
        while(nLen > 0)
        {
           dbgPrintf("%02x ", pData[0]);
           pData++;
           nLen--;
        }
        dbgPrintf("\r\n");

    }
    

#endif
}

////////////////////////////////////////////////////////////////
void BlrudpFillPktHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_IO_PKT *pPkt,
                      BLRUDP_PKT_HDR *pPktHdr,
                      uint16_t usCmd)
//
// fill out the packet header with the information from the context.
//
// inputs:
//    pContext -- context to construct the packet for.

//    pPkt -- pointer to packet to fill header info out.
//    usCmd -- command value to insert into the header.
// outputs:
//    none.
//
{
    //
    // The method for picking the initial sequence number is done BEFORE this is called.
    //
    

    pPkt->pContext = &pContext->Hdr;      // owning context....
    if (usCmd == BLRUDP_CMD_TYPE_DATA)      // // for data packets this should already be assigned...
    {
        ASSERT(pPkt->ulSseq == pContext->XmitSeq.ulSseq);       
    }
    else
        pPkt->ulSseq = pContext->XmitSeq.ulSseq;        // all other packets it is dynamic...
    pPkt->ulFlags = 0L;

    // merge the command and flags
    usCmd = (usCmd & BLRUDP_CMD_TYPE) | (pContext->usXmitFlags & (~BLRUDP_CMD_TYPE));
    if (pContext->Options.bChecksum)
        usCmd |= BLRUDP_CMD_CHECKSUM;    

    pPktHdr->usCmd = htons(usCmd);      // network byte order..

    if (usCmd & BLRUDP_CMD_SREQ)
        pPktHdr->ulSync = htonl(pContext->ulXmitSync+1);        // sreq sends the next Sync number...
    else
        pPktHdr->ulSync = htonl(pContext->ulXmitSync);
        
    pPktHdr->ulSseq = htonl(pPkt->ulSseq);
    pPktHdr->ulRseq = htonl(pContext->XmitSeq.ulRseq); // receive acknowlege. and delivered...
    pPktHdr->ulCheck = 0;


}

////////////////////////////////////////////////////////////////
void BlrudpRefreshPktHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                         BLRUDP_IO_PKT *pPkt)
//
// Refresh the packet header with current Sync and Echo and SREQ fields.
// 
// inputs:
//     pContext -- context of the packet to refresh.
//     pPkt -- packet to refresh.
// outputs:
//    none.
{
    BLRUDP_PKT_HDR *pPktHdr;
    uint16_t usCmd;

    pPktHdr = BlrudpGetPktPayload(pPkt);

    //
    // Redo the transmit flags...  (NOTE: may want to not disturbe the
    // RCLOSE and WCLOSE flags here.  Not sure of the effect of this....
    //
    usCmd = (pContext->usXmitFlags & (~BLRUDP_CMD_TYPE)) | ntohs(pPktHdr->usCmd);
    pPktHdr->usCmd = htons(usCmd);      
    pPktHdr->ulSync = htonl(pContext->ulXmitSync);
    pPktHdr->ulRseq = htonl(pContext->XmitSeq.ulRseq); // receive acknowlege. and delivered...
    pPktHdr->ulCheck = 0;

    BlrudpChecksumPkt(pPktHdr);

}


////////////////////////////////////////////////////////////////////////////////////
void BlrudpFillDataPktHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                          BLRUDP_IO_PKT *pPkt)
//
// Fill out the data packet header..  The data payload is already filled out
// here and the data length field is set...
//
// inputs:
//     pContext -- context of the packet to refresh.
//     pPkt -- packet to refresh.
// outputs:
//    none.
//
{
    BLRUDP_PKT_HDR *pPktHdr;
    pPktHdr = BlrudpGetPktPayload(pPkt);

    BlrudpFillPktHdr(pContext,                     // *pContext, 
                     pPkt,                         // *pPkt,             
                     pPktHdr,                      // *pPktHdr,         
                     BLRUDP_CMD_TYPE_DATA);        // usCmd)                  

    ASSERT(pPkt->usPktLen <= pContext->pLocalSap->uMtuSize);  // hey

    if (pPkt->usPktLen > pContext->pLocalSap->uMtuSize)       // make sure we don't do something stupid...
        pPkt->usPktLen = pContext->pLocalSap->uMtuSize;

    pPktHdr->usDlen = htons(pPkt->usPktLen);
    BlrudpChecksumPkt(pPktHdr);
}

/////////////////////////////////////////////////////////////////////
void BlrudpFillCmdAndTCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                              BLRUDP_IO_PKT *pPkt,
                              uint16_t usCmd)
//
// Fill a TCtrl packet with the options contents 
// and sequence numbers from the pContext and stuff the indicated command
// into the header.
//
// Fill out the contents of a first packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with TCTRL packet
//                 information.
// outputs:
//    output none.
{
    BLRUDP_IO_TCTRL_PAYLOAD *pTctrl;
    uint8_t *pPayload;
    uint32_t *pulValue;
    uint16_t *pusValue;
    BLRUDP_IO_TCTRL_PKT *pTCtrlPkt;

    ASSERT(pContext && pPkt);

    if ((pContext == NULL) || (pPkt == NULL))      // should not happen...
        return;                                         // but punt anyway.

    pContext->usXmitFlags |= BLRUDP_CMD_SREQ;
    pTCtrlPkt = (BLRUDP_IO_TCTRL_PKT *)BlrudpGetPktPayload(pPkt);

    //
    // TBD.. some of this will be migrated into separate functions as we find
    // commonality between packets...
    //
    BlrudpFillPktHdr(pContext, 
                     pPkt,
                     &pTCtrlPkt->Ctrl.IoPkt,
                     usCmd);


    //
    // What should this number be initially, we have not negoiated the window size with
    // the other side yet...
    //
    BlrudpFillCtrlHdr(pContext, &pTCtrlPkt->Ctrl);

    pTCtrlPkt->nParams = htons(3);      // 3 parameters....

    pPayload = (uint8_t *)pTCtrlPkt->ParamPayload;

    pTctrl = (BLRUDP_IO_TCTRL_PAYLOAD *)pPayload;
    pTctrl->nKey = BLRUDP_TCTRL_WINDOWSIZE;
    pTctrl->nLen = sizeof(uint32_t);
    pulValue = (uint32_t *)pTctrl->szData;
    *pulValue = htonl(pContext->Options.ulWindowSize);

    //
    // next variable length payload section.
    //
    ASSERT(pContext->Options.ulMtuSize);            // hey, this should NOT be zero....
    pPayload += pTctrl->nLen + offsetof(BLRUDP_IO_TCTRL_PAYLOAD, szData);    // next location.
    pTctrl = (BLRUDP_IO_TCTRL_PAYLOAD *)pPayload;
    pTctrl->nKey = BLRUDP_TCTRL_MTUSIZE;
    pTctrl->nLen = sizeof(uint32_t);                       
    pulValue = (uint32_t *)pTctrl->szData;
    *pulValue = htonl(pContext->Options.ulMtuSize);
    
    pPayload += pTctrl->nLen + offsetof(BLRUDP_IO_TCTRL_PAYLOAD, szData);    // next location.
    pTctrl = (BLRUDP_IO_TCTRL_PAYLOAD *)pPayload;
    pTctrl->nKey = BLRUDP_TCTRL_CHECKSUM;
    pTctrl->nLen = sizeof(uint16_t);                       
    pusValue = (uint16_t *)pTctrl->szData;
    *pusValue = htons(pContext->Options.bChecksum);
    pPayload += pTctrl->nLen + offsetof(BLRUDP_IO_TCTRL_PAYLOAD, szData);    // next location.

    pPkt->usPktLen =  (uint16_t)(pPayload - (uint8_t *)pTCtrlPkt);
    pTCtrlPkt->Ctrl.IoPkt.usDlen = htons(pPkt->usPktLen);

    BlrudpChecksumPkt(&pTCtrlPkt->Ctrl.IoPkt);
}

//////////////////////////////////////////////////////////////////
void BlrudpFillTCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                              BLRUDP_IO_PKT *pPkt)
//
// Fill a TCtrl packet with the options contents 
// and sequence numbers from the pContext
//
// Fill out the contents of a first packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pTctrlPkt -- pointeer to buffer to fill out with TCTRL packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////
{
    BlrudpFillCmdAndTCtrlPkt(pContext,                      // pContect
                             pPkt,                     // pTctrlPkt
                             BLRUDP_CMD_TYPE_TCTRL);        // uscmd
}



/////////////////////////////////////////////////////////////
void BlrudpFillFirstPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt)
//
// Fill out the contents of a first packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with first packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////
{
    BlrudpFillCmdAndTCtrlPkt(pContext,                      // pContect
                             pPkt,                          // pPkt
                             BLRUDP_CMD_TYPE_FIRST);        // uscmd


}

////////////////////////////////////////////////////////////////////
void BlrudpFillCtrlHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_IO_CTRL_PKT *pCtrlPkt)
//
// Fill the control header with the perinant informatino.
// inputs:
//    pContext -- context to use to fill it out.
//    pCtrlPkt -- packet to fill out....
// outputs:
//    none.
{

    pCtrlPkt->ulAllocSeq = htonl(pContext->XmitSeq.ulAllocSeq);    
    pCtrlPkt->ulEcho = htonl(pContext->ulRecvSync);

}


/////////////////////////////////////////////////////////////
void BlrudpFillCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt)
//
// Fill out the contents of a CTRL packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with control packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////
{
    BLRUDP_IO_CTRL_PKT *pCtrlPkt;

    ASSERT(pContext && pPkt);

    if ((pContext == NULL) || (pPkt == NULL))      // should not happen...
        return;                                         // but punt anyway.

    pCtrlPkt = (BLRUDP_IO_CTRL_PKT *)BlrudpGetPktPayload(pPkt);

    BlrudpFillPktHdr(pContext, 
                     pPkt, 
                     &pCtrlPkt->IoPkt, 
                     BLRUDP_CMD_TYPE_CTRL);
    BlrudpFillCtrlHdr(pContext, pCtrlPkt);

    pPkt->usPktLen = sizeof(*pCtrlPkt);
    pCtrlPkt->IoPkt.usDlen = htons(pPkt->usPktLen);

    BlrudpChecksumPkt(&pCtrlPkt->IoPkt);


}

/////////////////////////////////////////////////////////////
void BlrudpFillResetPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt,
                        uint32_t ulSseq)
//
// Fill out the contents of a Reset packet and prepare for sending....
//
// A reset packet is a control packet with an overriden rSeq and
// the END bit set.  This causes a half open context on the other
// end to abort....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with control packet
//                 information.
//    ulSseq -- send sequence number to use override.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////
{
    BLRUDP_IO_CTRL_PKT *pCtrlPkt;

    ASSERT(pContext && pPkt);

    if ((pContext == NULL) || (pPkt == NULL))      // should not happen...
        return;                                         // but punt anyway.

    pCtrlPkt = (BLRUDP_IO_CTRL_PKT *)BlrudpGetPktPayload(pPkt);

    BlrudpFillPktHdr(pContext, 
                     pPkt, 
                     &pCtrlPkt->IoPkt, 
                     BLRUDP_CMD_TYPE_CTRL);

    BlrudpFillCtrlHdr(pContext, pCtrlPkt);

    //
    // override the sequence and flags...
    //
    pCtrlPkt->IoPkt.ulSseq = htonl(ulSseq);                  
    pCtrlPkt->IoPkt.usCmd |= htons(BLRUDP_CMD_END);

    pPkt->usPktLen = sizeof(*pCtrlPkt);
    pCtrlPkt->IoPkt.usDlen = htons(pPkt->usPktLen);

    BlrudpChecksumPkt(&pCtrlPkt->IoPkt);
}




///////////////////////////////////////////////////////////////////////////////////
void BlrudpChecksumPkt(BLRUDP_PKT_HDR *pPkt)
//
// Optionally perform the packet checksum.
//
// inputs:
//   pPkt = packet to perform the checksum in.
// outputs:
//    none.
{
    
    if (ntohs(pPkt->usCmd) & BLRUDP_CMD_CHECKSUM)
    {
        pPkt->ulCheck = BlrudpXsumPkt(pPkt,
                                      ntohs(pPkt->usDlen));
    
        //
        // Make absolutly sure the checksum is good going out...
        #ifdef _DEBUG
        {
            uint32_t ulCheck;
            ulCheck = BlrudpXsumPkt(pPkt,
                                    ntohs(pPkt->usDlen));
            if (ulCheck != 0)
            {
                dbgPrintf("Sending Checksum bad, usDlen = %lx\r\n", ntohs(pPkt->usDlen));
            }
        }
        #endif
    }


}

////////////////////////////////////////////////////////////////////////////////
void BlrudpFillErrorPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt,
                        int32_t lErrCode,
                        int32_t lErrValue)
//
// Fill an error packet with a single error code.....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- poitner to buffer to fill out with error packet
//                 information.
//    lErrCode -- error code to stuff.
//    lErrValue -- value to stuff with it....
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////
{
    BLRUDP_IO_ERROR_PKT *pErrPkt;

    ASSERT(pContext && pPkt);

    if ((pContext == NULL) || (pPkt == NULL))      // should not happen...
        return;                                         // but punt anyway.

    pErrPkt = (BLRUDP_IO_ERROR_PKT *)BlrudpGetPktPayload(pPkt);

    BlrudpFillPktHdr(pContext, 
                     pPkt, 
                     &pErrPkt->Ctrl.IoPkt, 
                     BLRUDP_CMD_TYPE_ERROR);
    BlrudpFillCtrlHdr(pContext, &pErrPkt->Ctrl);

    pErrPkt->usNumErr = 1;
    pErrPkt->ErrorPayload[0].lErrCode = lErrCode;
    pErrPkt->ErrorPayload[0].lErrValue = lErrValue;

    pPkt->usPktLen = sizeof(BLRUDP_IO_ERROR_PKT );
    pErrPkt->Ctrl.IoPkt.usDlen = htons(pPkt->usPktLen);

    BlrudpChecksumPkt(&pErrPkt->Ctrl.IoPkt);

}

///////////////////////////////////////////////////////////////////
uint16_t BlrudpGetPktCmd(BLRUDP_PKT_HDR *pPkt)
//
// Pick off and encode the packet command without the flags....
//
// inputs:
//    pPkt -- packet to decode this from.
// outputs:
//    returns -- command in host byte order.
//
{
    int16_t usCmd;
    usCmd = ntohs(pPkt->usCmd);
    return(usCmd & BLRUDP_CMD_TYPE);
}
/////////////////////////////////////////////////////////////
void BlrudpFillEcntlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt)
//
// Fill out the contents of a ECTRL (error control) packet 
// and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with control packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////
{
    BLRUDP_IO_ECNTL_PKT *pEnctlPkt;
    BLRUDP_IO_ECNTL_PAYLOAD *pPayload;
    BLRUDP_RECV_PKT_STATUS *pStatus;
    uint32_t ulMaxErrors;
    uint32_t ulSseq;
    uint32_t ulEndSeq;
    uint32_t ulNumErrs;
    uint32_t ulDlen;


    ASSERT(pContext && pPkt);

    if ((pContext == NULL) || (pPkt == NULL))      // should not happen...
        return;                                         // but punt anyway.

    pEnctlPkt = (BLRUDP_IO_ECNTL_PKT *)BlrudpGetPktPayload(pPkt);

    pStatus = &pContext->RecvPktStatus; // pickup a shorthand pointer.

    BlrudpFillPktHdr(pContext, 
                     pPkt, 
                     &pEnctlPkt->Ctrl.IoPkt, 
                     BLRUDP_CMD_TYPE_ECNTL);
    BlrudpFillCtrlHdr(pContext, &pEnctlPkt->Ctrl);

    pPayload = pEnctlPkt->nEcntlPayload;        // point to the payload....

    //
    // compute the max number of errors we can have in this payload.
    //
    ulMaxErrors = (pPkt->usPayloadLen - offsetof(BLRUDP_IO_ECNTL_PKT, nEcntlPayload)) /
                    sizeof(*pPayload);

    //
    // The sequence RecvSeq.ulNextInSeq to ulSseq contain packets 
    // we are missing.  
    //

    ASSERT(pStatus->ulEndSeq == pContext->RecvSeq.ulSseq);

    //
    // should not be doing this if this is the case....
    ASSERT(pContext->RecvSeq.ulNextInSeq != pContext->RecvSeq.ulSseq);

    ulSseq = pContext->RecvSeq.ulNextInSeq;
    ulEndSeq = pContext->RecvSeq.ulSseq;

    ulNumErrs = BlrudpPktStatusFillErrPayload(&pContext->RecvPktStatus,      // *pPktStatus, 
                                              ulSseq,                        // ulStartSeq,                
                                              ulEndSeq,                      // ulEndSeq,                  
                                              pPayload,                      // *pPayload,  
                                              ulMaxErrors);                  // ulMaxErrors);              



    pEnctlPkt->usEnctls = htons( (uint16_t)ulNumErrs);
    ulDlen = offsetof(BLRUDP_IO_ECNTL_PKT, nEcntlPayload)+(sizeof(*pPayload) * ulNumErrs);

    pPkt->usPktLen = (uint16_t)ulDlen;
    pEnctlPkt->Ctrl.IoPkt.usDlen = htons(pPkt->usPktLen);

    BlrudpChecksumPkt(&pEnctlPkt->Ctrl.IoPkt);

}

/////////////////////////////////////////////////////////////
void BlrudpFillDiagReq(BLRUDP_ACTIVE_CONTEXT *pContext,
                       BLRUDP_IO_PKT *pPkt)
//
// Fill out the contents of a DIAGREQ packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with first packet
//                 information.
// outputs:
//    output none.
{
    BLRUDP_IO_DIAGREQ_PKT *pDiagPkt;
    uint16_t usNumReq;

    ASSERT(pContext && pPkt);

    if ((pContext == NULL) || (pPkt == NULL))      // should not happen...
        return;                                         // but punt anyway.

    pDiagPkt = (BLRUDP_IO_DIAGREQ_PKT *)BlrudpGetPktPayload(pPkt);

    BlrudpFillPktHdr(pContext, 
                     pPkt, 
                     &pDiagPkt->Ctrl.IoPkt, 
                     BLRUDP_CMD_TYPE_DREQ);
    BlrudpFillCtrlHdr(pContext, &pDiagPkt->Ctrl);

    usNumReq = 0;                          // no explicit items requested...
    pPkt->usPktLen = (sizeof(pDiagPkt->usCode) * usNumReq) + 
                      offsetof(BLRUDP_IO_DIAGREQ_PKT, usCode);
    pDiagPkt->usNumReq = htons(usNumReq);
    pDiagPkt->Ctrl.IoPkt.usDlen = htons(pPkt->usPktLen);

    BlrudpChecksumPkt(&pDiagPkt->Ctrl.IoPkt);
}

/////////////////////////////////////////////////////////////
void BlrudpFillDiagResp(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt)
//
// Fill out the contents of a RESPONSE packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with first packet
//                 information.
// outputs:
//    output none.
{
    BLRUDP_IO_DIAGRESP_PKT *pDiagPkt;
    BLRUDP_IO_DIAGRESP_PAYLOAD *pDiagPayload;
    uint8_t *pPayload;
    uint16_t usNumResp;
    uint64_t *pllValue;
    ASSERT(pContext && pPkt);

    if ((pContext == NULL) || (pPkt == NULL))      // should not happen...
        return;                                         // but punt anyway.

    pDiagPkt = (BLRUDP_IO_DIAGRESP_PKT *)BlrudpGetPktPayload(pPkt);

    BlrudpFillPktHdr(pContext, 
                     pPkt, 
                     &pDiagPkt->Ctrl.IoPkt, 
                     BLRUDP_CMD_TYPE_DRESP);
    BlrudpFillCtrlHdr(pContext, &pDiagPkt->Ctrl);

    usNumResp = 0;                          

    pPayload = (uint8_t *)&pDiagPkt->DiagPayload;

    pDiagPayload = (BLRUDP_IO_DIAGRESP_PAYLOAD *)pPayload;
    pDiagPayload->usCode = BLRUDP_DIAG_NUMRETRANS;
    pDiagPayload->usLen = sizeof(uint64_t);                       
    pllValue = (uint64_t *)pDiagPayload->Value;
    *pllValue = hton64(pContext->LocalDiag.llNumRetrans);
    usNumResp++;

    // next location.
    pPayload += pDiagPayload->usLen + offsetof(BLRUDP_IO_DIAGRESP_PAYLOAD, Value);    
    pDiagPayload = (BLRUDP_IO_DIAGRESP_PAYLOAD *)pPayload;
    pDiagPayload->usCode = BLRUDP_DIAG_DATAXMIT;
    pDiagPayload->usLen = sizeof(uint64_t);                       
    pllValue = (uint64_t *)pDiagPayload->Value;
    *pllValue = hton64(pContext->LocalDiag.llDataXmit);
    usNumResp++;

    //next location...
    pPayload += pDiagPayload->usLen + offsetof(BLRUDP_IO_DIAGRESP_PAYLOAD, Value);    
    pDiagPayload = (BLRUDP_IO_DIAGRESP_PAYLOAD *)pPayload;
    pDiagPayload->usCode = BLRUDP_DIAG_DATARECV;
    pDiagPayload->usLen = sizeof(uint64_t);                       
    pllValue = (uint64_t *)pDiagPayload->Value;
    *pllValue = hton64(pContext->LocalDiag.llDataRecv);
    usNumResp++;

    pPayload += pDiagPayload->usLen + offsetof(BLRUDP_IO_DIAGRESP_PAYLOAD, Value);    
    
    pPkt->usPktLen = (uint16_t)(pPayload - (uint8_t *)pDiagPkt);
    pDiagPkt->usNumResp = htons(usNumResp);          
    pDiagPkt->Ctrl.IoPkt.usDlen = htons(pPkt->usPktLen);

    BlrudpChecksumPkt(&pDiagPkt->Ctrl.IoPkt);


}



#if (BYTE_ORDER == LITTLE_ENDIAN)

//////////////////////////////////////////////////////////////////
uint64_t hton64 (uint64_t lValue )
//
// 64 bit byte flip function.
// inputs:
//    lValue -- value to flip.
// outputs:
//    lValue -- flipped value if the machine is little endian.
//
///////////////////////////////////////////////////////////////
{
    uint8_t *pSrc;
    uint8_t *pDest;
    uint64_t lDest;
    int n;

    pSrc =  (uint8_t *)&lValue;
    pDest = ((uint8_t *)&lDest) + (sizeof(uint64_t)-1);

    for (n = sizeof(uint64_t); n > 0; n--)
        *pDest-- = *pSrc++;

    return(lDest);
}
#endif

