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
 ///////////////////////////////////////////////////////////////////
// Bluelight rudp Buffer queue management....
//
///////////////////////////////////////////////////////////////////

#include "hostcfg.h"


#include "blrudp_memory.h"
#include "BlErrors.h"
#include "blrudppkt.h"
#include "blrudp_pktpool.h"
#include "blrudp_sapmgr.h"

#include "dbgPrint.h"

#include "bldefs.h"

int BlrudpAllocPktPool(_QLink *listPkts,
                       unsigned long ulNumPkts,
                       unsigned short usSizePkt);
int BlrudpIoPktMoveVectorToPool(_QLink *pPktPool,
                                 BLRUDP_PKT_VECTOR     *pVector);
int BlrudpIoPktMoveVectorToNicPool(BLRUDP_NIC_ITEM      *pLocalNic,
                                   BLRUDP_PKT_VECTOR    *pVector);
int BlrudpIoPktMoveToNicPool(BLRUDP_NIC_ITEM *pLocalNic,
                              _QLink *pPktQueue);


//////////////////////////////////////////////////////
//
// TBD.. do separate send and receive packet sizes...
//
//

///////////////////////////////////////////////////////////////////
int BlrudpAllocPktPool(_QLink *listPkts,
                       unsigned long ulNumPkts,
                       unsigned short usSizePkt)
//
// inputs:
//    pBlrudp -- pointer to the Blrudp instance
//    listPkts -- packet pool list to all packets to..
//    ulNumPkts -- number of packets to allocate.
//    ulSizePkt -- size of each packet....
// outputs:
//    returns -- BLRUDP_SUCCESS if OK.
//               or BLRUDP_NO_MEMORY if allocations failed.
//
{
    unsigned long n;

    for (n = 0; n < ulNumPkts; n++)
    {
        BLRUDP_IO_PKT *pPkt;
        pPkt = BlrudpPktNew(usSizePkt);
        if (pPkt == NULL)
            FAIL;
        EnQ(&pPkt->link, listPkts);
    }
    return(BLERR_SUCCESS);
Fail:
    return(BLERR_NO_MEMORY);
}
////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT_POOL *BlrudpIoPktPoolNew(BLRUDP_IO_PKT_POOL **ppSysPool,
                                       BLRUDP_NIC_ITEM  *pLocalNic,
                                       unsigned long ulContextId,
                                       unsigned long ulNumXmitPkts,
                                       unsigned long ulNumRcvPkts,
                                       unsigned short usSizePkt)
//
// Create and initialize a Blue light Reliable udp packet buffer
// control structure.  This structure contains the linked
// lists that hold allocated packet buffers for either the main
// packet pool or each active context created with
// its own packet pool.
//
// inputs:
//    ppSysPool -- pointer to common system pool area.
//    pLocalNic -- network interface controller associated
//            with this packet pool......
//    ulContextId -- context identifier used for debug...
//    ulNumXmitPkts -- number of xmit packets to allocate.
//    ulNumRcvPkts -- number of receive packets to allocate.
//    usSizePkt -- size of each packet....
// ouputs:
//    returns -- pointer to a newly allocated and initialized
//               Packet Control structure.
//
{
    BLRUDP_IO_PKT_POOL *pPktPool = NULL;
    int nRet;

    ASSERT(usSizePkt > (sizeof(BLRUDP_IO_PKT)*2));
    if (usSizePkt <= (sizeof(BLRUDP_IO_PKT)*2))
        FAIL;               // just punt


    pPktPool = BlrAllocMem(sizeof(*pPktPool));
    if (pPktPool == NULL)
        FAIL;

    pPktPool->ulContextId = ulContextId;
    pPktPool->ulRefCount = 1;
    pPktPool->ulSizePkt = usSizePkt;
    //
    // initialize our structures.
    QInit(&pPktPool->listSendBuffers);
    //QInit(&pPktPool->listRecvBuffers);
    QInit(&pPktPool->listCtrlBuffers);

    pPktPool->lTotalRcvPkts = 0;
    pPktPool->lRcvPkts = 0;
    pPktPool->ulSendPkts = 0;
                            

    pPktPool->ppSysPool = ppSysPool;        // save this value....

    ASSERT(pLocalNic);                           // we must have a NIC pool...
    pPktPool->pLocalNic = pLocalNic;

    nRet = BlrudpAllocPktPool(&pPktPool->listSendBuffers,   // listPkts
                              ulNumXmitPkts,                // ulNumPkts
                              usSizePkt);                   // ulSizePkt
    if (nRet != BLERR_SUCCESS)
        FAIL;
    pPktPool->ulSendPkts = ulNumXmitPkts;
    
    // leave some room for receive control pkts..
    //
    ulNumRcvPkts += 2;
    nRet = BlrudpAddNicRecvPkts(pPktPool->pLocalNic,
                                ulNumRcvPkts);
    if (nRet != BLERR_SUCCESS)
        FAIL;

        
    pPktPool->lTotalRcvPkts = ulNumRcvPkts;       
    pPktPool->lRcvPkts = ulNumRcvPkts;            // pkts...

    nRet = BlrudpAllocPktPool(&pPktPool->listCtrlBuffers,   // listPkts
                              BLRUDP_MAX_CTRL_POOL,         // ulNumPkts
                              BLRUDP_SIZE_CTRL_PKT);        // ulSizePkt
    if (nRet != BLERR_SUCCESS)
        FAIL;


    return(pPktPool);
Fail:
    if (pPktPool)
        BlrudpIoPktPoolDelete(pPktPool);
    return(NULL);
}

////////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveVectorToSendPool(BLRUDP_IO_PKT_POOL *pPktPool,
                                     struct T_BLRUDP_PKT_VECTOR     *pVector)
//
// Move a packet vector to the send pool
//
// inputs:
//    pPktPool -- packet pool to move vector back to.
//    pVector -- pointer to vector to move packets from.
// outputs:
//    none.
//
{
    BlrudpIoPktMoveVectorToPool(&pPktPool->listSendBuffers, pVector);
}
////////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveVectorToRecvPool(BLRUDP_IO_PKT_POOL *pPktPool,
                                     struct T_BLRUDP_PKT_VECTOR     *pVector)
//
// Move a packet vector to the send pool
//
// inputs:
//    pPktPool -- packet pool to move vector back to.
//    pVector -- pointer to vector to move packets from.
// outputs:
//    none.
//
{
    uint32_t ulNumReturned;
    ulNumReturned = BlrudpIoPktMoveVectorToNicPool(pPktPool->pLocalNic, pVector);
    pPktPool->lRcvPkts += ulNumReturned;

    ///////////////////////////////////////////////////////////////////////
    // double check our numbers here...
    //
    ASSERT(pPktPool->lRcvPkts <= pPktPool->lTotalRcvPkts);
    if (pPktPool->lRcvPkts > pPktPool->lTotalRcvPkts)
        pPktPool->lRcvPkts = pPktPool->lTotalRcvPkts;
}

///////////////////////////////////////////////////////////////////////////
int BlrudpIoPktMoveVectorToPool(_QLink *pPktPool,
                                 BLRUDP_PKT_VECTOR     *pVector)
//
// move the contents of the packet vector to the specified packet pool
//
// inputs:
//     pPktPool --- pointer to actual pool queue to add this to.
//     pVector -- pointer to vector of packets...
// outputs:
//    returns -- number of packets returned to the pool..
//
{
    uint32_t n;
    uint32_t nNumPkts;
    uint32_t nNumReturned = 0;

    BLRUDP_IO_PKT **ppPkt;
    ASSERT(pPktPool && pVector);
    if ((pPktPool == NULL) || (pVector == NULL))
        FAIL;
    if (pVector->pvectPkts)
        return(0);

    nNumPkts = pVector->ulNumEntries;
    for (n = 0, ppPkt = pVector->pvectPkts; 
         n < nNumPkts; 
         n++, ppPkt++)
    {
        if (*ppPkt)
        {
            BLRUDP_IO_PKT *pPkt = *ppPkt;
            if (pPkt->link.next)        // if it is on a list, yank it.
                DeQ(&pPkt->link);
            EnQ(&pPkt->link, pPktPool);        // add it to the pool
            *ppPkt = NULL;      
            nNumReturned++;
        }

    }

    return(nNumReturned);
Fail:
    return(nNumReturned);
}
///////////////////////////////////////////////////////////////////////////
int BlrudpIoPktMoveVectorToNicPool(BLRUDP_NIC_ITEM      *pLocalNic,
                                   BLRUDP_PKT_VECTOR    *pVector)
//
// move the contents of the packet vector to the specified packet pool
//
// inputs:
//     pLocalNic -- pointer to local NIC to add this to...
//     pVector -- pointer to vector of packets...
// outputs:
//    returns -- number of packets returned to the pool..
//
{
    uint32_t n;
    uint32_t nNumPkts;
    uint32_t nNumReturned = 0;

    BLRUDP_IO_PKT **ppPkt;
    ASSERT(pLocalNic && pVector);
    if ((pLocalNic == NULL) || (pVector == NULL))
        FAIL;
    if (pVector->pvectPkts)
        return(0);

    nNumPkts = pVector->ulNumEntries;
    for (n = 0, ppPkt = pVector->pvectPkts; 
         n < nNumPkts; 
         n++, ppPkt++)
    {
        if (*ppPkt)
        {
            BlrudpPutNicFreeRecvPkt(pLocalNic,
                                *ppPkt);
            *ppPkt = NULL;      
            nNumReturned++;
        }

    }

    return(nNumReturned);
Fail:
    return(nNumReturned);
}
////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveToSendPool(BLRUDP_IO_PKT_POOL *pPktPool,
                               _QLink *pPktQueue)
//
// Move the packets from the packet queue to the send packet
// pool.
//
// inputs:
//    pPktPool -- pointer to pkt control to transfer
//    pPktQueue --- pointer to packet queue move the contents of.
// outputs:
//    none.
//    
{
    BlrudpIoPktMoveToPktPool(&pPktPool->listSendBuffers, pPktQueue);
}

////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveToCtrlPool(BLRUDP_IO_PKT_POOL *pPktPool,
                               _QLink *pPktQueue)
//
// Move the packets from the packet queue to the send packet
// pool.
//
// inputs:
//    pPktPool -- pointer to pkt control to transfer
//    pPktQueue --- pointer to packet queue move the contents of.
// outputs:
//    none.
//    
{
    BlrudpIoPktMoveToPktPool(&pPktPool->listCtrlBuffers, pPktQueue);
}

//////////////////////////////////////////////////////////////
void BlrudpIoPktMoveToRecvPool(BLRUDP_IO_PKT_POOL *pPktPool,
                               _QLink *pPktQueue)
//
// Move the packets from the packet queue to the receive packet
// pool.
//
// inputs:
//    pPktPool -- pointer to pkt control to transfer
//    pPktQueue --- pointer to packet queue move the contents of.
// outputs:
//    none.
//    
{
    uint32_t ulNumReturned;

    ulNumReturned = BlrudpIoPktMoveToNicPool(pPktPool->pLocalNic, pPktQueue);
    pPktPool->lRcvPkts += ulNumReturned;

    //
    // double check our numbers here...
    //
    ASSERT(pPktPool->lRcvPkts <= pPktPool->lTotalRcvPkts);
    if (pPktPool->lRcvPkts > pPktPool->lTotalRcvPkts)
        pPktPool->lRcvPkts = pPktPool->lTotalRcvPkts;
}

//////////////////////////////////////////////////////////////
int BlrudpIoPktMoveToPktPool(_QLink *pPktPool,
                              _QLink *pPktQueue)
//
// Move the packets from the packet queue to the receive packet
// pool.
//
// inputs:
//    pPktPool -- pointer to pkt pool queue to transfer to.
//    pPktQueue --- pointer to packet queue move the contents of.
// outputs:
//    returns -- number of packets returned to the pool.
//    
{
    uint32_t ulNumPkts = 0;
    while (QisFull(pPktQueue))
    {
        BLRUDP_IO_PKT *pPkt =  (BLRUDP_IO_PKT *)QBegin(pPktQueue);
        DeQ(&pPkt->link);
        EnQ(&pPkt->link, pPktPool);
        ulNumPkts++;        // count em''
    }
    return(ulNumPkts);
}
//////////////////////////////////////////////////////////////
int BlrudpIoPktMoveToNicPool(BLRUDP_NIC_ITEM *pLocalNic,
                              _QLink *pPktQueue)
//
// Move the packets from the packet queue to the Nicpacket
// pool.
//
// inputs:
//    pLocalNic -- local Nic to transfer packets to...
//    pPktQueue --- pointer to packet queue move the contents of.
// outputs:
//    returns -- number of packets returned to the pool.
//    
{
    uint32_t ulNumPkts = 0;
    while (QisFull(pPktQueue))
    {
        BLRUDP_IO_PKT *pPkt =  (BLRUDP_IO_PKT *)QBegin(pPktQueue);
        DeQ(&pPkt->link);
        BlrudpPutNicFreeRecvPkt(pLocalNic,
                            pPkt);
        ulNumPkts++;        // count em''
    }
    return(ulNumPkts);
}

////////////////////////////////////////////////////////////////////
void BlrudpIoPktQueueDelete(_QLink *pPktQueue)
//
// Delete the memory associated with a queue of packets.
//
// inputs:
//    pBlrudp -- pointer to the Blrudp instance
//    pPktQueue --- pointer to packet queue to delete.
// outputs:
//    none.
//
{

    //
    // dump the contents of each list.
    //
    while (QisFull(pPktQueue))
    {
        BLRUDP_IO_PKT *pPkt =  (BLRUDP_IO_PKT *)QBegin(pPktQueue);
        DeQ(&pPkt->link);               
        BlrudpPktDelete(pPkt);
    }
}

////////////////////////////////////////////////////////////////////
void BlrudpIoPktPoolDelete(BLRUDP_IO_PKT_POOL *pPktPool)
//
// Delete the allocated packet pool structure.  This also
// deletes any memory associated with any packets left in its queues.
//
// inputs:
//    pPktPool -- pointer to pkt control to delete.
// outputs:
//    none.
//
{
    ASSERT(pPktPool);
    if (pPktPool == NULL)       // hey.. don't do that.
        return;


    //
    // dump the contents of each list.
    //
    BlrudpIoPktQueueDelete(&pPktPool->listSendBuffers);
    BlrudpIoPktQueueDelete(&pPktPool->listCtrlBuffers);

    //
    // undo the credit of NIC packets from the receive buffer.
    //
    BlrudpRemoveNicRecvPkts(pPktPool->pLocalNic, 
                            pPktPool->lTotalRcvPkts);
            


    //
    // If this is the managers packet pool, then clean it out.
    //
    if ((pPktPool->ppSysPool) && (*(pPktPool->ppSysPool) == pPktPool))
        *(pPktPool->ppSysPool) = pPktPool = NULL;
                
    BlrFreeMem(pPktPool);

}

//////////////////////////////////////////////////////////////////////
void BlrudpIoPktPoolRelease(BLRUDP_IO_PKT_POOL *pPktPool)
//
// Release the refrence counted packet control structure.  
//
// when the reference count reaches zero then the packet pool
// is deleted.
//
//
// inputs:
//    pPktPool -- pointer to pkt control to delete.
// outputs:
//    none.
//
{
    ASSERT(pPktPool);
    if (pPktPool == NULL)       // punt...
        FAIL;

    if (pPktPool->ulRefCount > 0)    
        pPktPool->ulRefCount--;

    if (pPktPool->ulRefCount == 0)
        BlrudpIoPktPoolDelete(pPktPool);

    return;
Fail:
    return;
}

////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpIoPktGetSendPkt(BLRUDP_IO_PKT_POOL *pPktPool)
//
// Retrieve a send packet from the packet pool
//
// inputs:
//    pPktPool -- packet pool to retrieve packet from.
// outputs:
//    returns -- pointer to from pkt pool, or zero if
//               packets are exausted.
//
{   
    BLRUDP_IO_PKT *pPkt = NULL;
    if (QisFull(&pPktPool->listSendBuffers))
    {
        ASSERT(pPktPool->ulSendPkts);
        if (pPktPool->ulSendPkts)
            pPktPool->ulSendPkts--;
        pPkt = (BLRUDP_IO_PKT *)QBegin(&pPktPool->listSendBuffers);
        DeQ(&pPkt->link);
    }
    else
    {
        ASSERT(pPktPool->ulSendPkts == 0);
    }
    return(pPkt);

}

//////////////////////////////////////////////////////////////////////
uint32_t BlrudpIoPktGetSendPktCount(BLRUDP_IO_PKT_POOL *pPktPool)
//
// Retrieve the number of send buffers available in the packet pool.
//
// 
// inputs:
//    pPktPool -- packet pool to retrieve packet count.
// outputs:
//    returns -- count of packets available in the send packet pool.
//
//////////////////////////////////////////////////////////////////////
{
    return(pPktPool->ulSendPkts);
}

////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpIoPktGetCtrlPkt(BLRUDP_IO_PKT_POOL *pPktPool)
//
// Retrieve a Ctrl packet from the packet pool
//
// inputs:
//    pPktPool -- packet pool to retrieve packet from.
// outputs:
//    returns -- pointer to from pkt pool, or zero if
//               packets are exausted.
//
{   
    BLRUDP_IO_PKT *pPkt = NULL;
    if (QisFull(&pPktPool->listCtrlBuffers))
    {
        pPkt = (BLRUDP_IO_PKT *)QBegin(&pPktPool->listCtrlBuffers);
        DeQ(&pPkt->link);
    }
    return(pPkt);

}
///////////////////////////////////////////////////////////////////////
int BlrudpIoPktDebitRecvPkt(BLRUDP_IO_PKT_POOL *pPktPool)
//
// reduce the credit of receive packets associated with this
// packet pool by 1.
// 
// inputs:
//    pPktPool -- packetpool to reduce debit.
// outputs:
//    returns BLERR_SUCCESS if successful.
//            BLERR_BLRUDP_NO_PKTS -- if no more packets are available
{
    ASSERT(pPktPool->lRcvPkts > 0);    // if this goes low, holler, it may be a problem...

    pPktPool->lRcvPkts--;
    return(BLERR_SUCCESS);            
}
////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpIoPktGetRecvPkt(BLRUDP_IO_PKT_POOL *pPktPool)
//
// Retrieve a receive packet from the packet pool
//
// inputs:
//    pPktPool -- packet pool to retrieve packet from.
// outputs:
//    returns -- pointer to from pkt pool, or zero if
//               packets are exausted.
//
{
    BLRUDP_IO_PKT *pPkt = NULL;

    if (pPktPool->lRcvPkts > 0)
    {
        pPkt = BlrudpGetNicFreeRecvPkt(pPktPool->pLocalNic);
        ASSERT(pPkt);           // hey. this should happen...
        if (pPkt)
            pPktPool->lRcvPkts--;
    }
    return(pPkt);
}
////////////////////////////////////////////////////////////////////////
void BlrudpIoPktPutRecvPkt(BLRUDP_IO_PKT_POOL *pPktPool, 
                      BLRUDP_IO_PKT *pPkt )
//
// Return a packet to the receive packet pool.
//
// inputs:
//    pPktPool -- packet pool to return packet to.
//    pPkt -- packet to return to the pool.
// outputs:
//    returns -- none
//
{
    //////////////////////////////////////////////////////////////////
    // put it back and don't loose it...
    //
    BlrudpPutNicFreeRecvPkt(pPktPool->pLocalNic,
                        pPkt);

    //
    // this is a valid condition, where we allocate get
    // a free packet from the NIC pool and then
    // re-allocate the packet pool....
    if (pPktPool->lRcvPkts < pPktPool->lTotalRcvPkts)
        pPktPool->lRcvPkts++;

}
////////////////////////////////////////////////////////////////////////
void BlrudpIoPktPutSendPkt(BLRUDP_IO_PKT_POOL *pPktPool, 
                      BLRUDP_IO_PKT *pPkt )
//
// Return a packet to the send packet pool.
//
// inputs:
//    pPktPool -- packet pool to return packet to.
//    pPkt -- packet to return to the pool.
// outputs:
//    returns -- none
//
{
    pPktPool->ulSendPkts++;
    EnQ(&pPkt->link, &pPktPool->listSendBuffers);
}
////////////////////////////////////////////////////////////////////////
void BlrudpIoPktPutCtrlPkt(BLRUDP_IO_PKT_POOL *pPktPool, 
                      BLRUDP_IO_PKT *pPkt )
//
// Return a packet to the Ctrl packet pool.
//
// inputs:
//    pPktPool -- packet pool to return packet to.
//    pPkt -- packet to return to the pool.
// outputs:
//    returns -- none
//
{
    EnQ(&pPkt->link, &pPktPool->listCtrlBuffers);
}
