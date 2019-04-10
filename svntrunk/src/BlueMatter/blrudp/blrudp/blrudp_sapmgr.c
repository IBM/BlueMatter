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
 ////////////////////////////////////////////////////////////
//
// This file contains the functions used to manage the SAP and NIC
// items in the bluelight RUDP system.
//


#include "hostcfg.h"

#if HAVE_MEMORY_H
    #include <memory.h>
#endif

#include "BlErrors.h"
#include "blrudp_sapmgr.h"
#include "blrudp_memory.h"
#include "blrudp_st.h"

#ifdef BLHPK
	#undef PACKED                   // ket blhpk define this
	#include <sys/blhpk_types.h>
	#include <sys/ppc_asm.h>
#endif

#include "bldefs.h"

//
// Define critical section macros for BLHPK.
//
#ifdef BLHPK
	#define BLRUDP_DEFINE_CRITICAL_SECTION(n) uint32_t n
	#define BLRUDP_BEGIN_CRITICAL_SECTION(n) n = BeginCriticalSection()
	#define BLRUDP_END_CRITICAL_SECTION(n) EndCriticalSection(n)

#else
	#define BLRUDP_DEFINE_CRITICAL_SECTION(n) 
	#define BLRUDP_BEGIN_CRITICAL_SECTION(n) 
	#define BLRUDP_END_CRITICAL_SECTION(n) 
#endif



//////////////////////////////////////////////////////////////
BLRUDP_NIC_ITEM *BlrudpNicItemNew(struct T_BLRUDP_MGR*pBlrudp,
                                  uint32_t ulIpAddr)
//
//
// Create a new Nic Item.
//  
// inputs:
//    pBlrudp-- pointer to Blrudp Manger (current home for the
//              list of NICS.
//    ulIpAddr -- IP address of nic to create.  
// 
{
    BLRUDP_NIC_ITEM *pNicItem = NULL;
	BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);

    pNicItem =  BlrAllocMem(sizeof(*pNicItem));
    if (pNicItem == NULL)
        FAIL;

    memset(pNicItem,0, sizeof(*pNicItem));

    pNicItem->ulIpAddr = ulIpAddr;
    QInit(&pNicItem->listRecvPktPool);


    #ifdef BLHPK
    QInit(&pNicItem->listRecvPkts);
    #endif

	BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
    EnQ(&pNicItem->link, &pBlrudp->listLocalNICs);
	BLRUDP_END_CRITICAL_SECTION(ulCrit);
	
    return(pNicItem);       // return the new item.
Fail:
    if (pNicItem)
        BlrudpNicItemDelete(pNicItem);
    return(NULL);
}

///////////////////////////////////////////////////////////////
void BlrudpNicItemRelease(BLRUDP_NIC_ITEM *pNicItem)
//
// Release a NIC item created by BlrudpNicItemNew...
// destroy when the reference count hits zero...
//
// inputs:
//     pNicItem -- nic item to destroy.
// outputs:
//     none.
//
{
    ASSERT(pNicItem);
    ASSERT(pNicItem->ulRefCount);       // should have dumped it by now...
    if (pNicItem->ulRefCount)
        pNicItem->ulRefCount--;

    //
    // refcount all the way down, dump it.
    //
    if (pNicItem->ulRefCount == 0)
    {
        BlrudpNicItemDelete(pNicItem);
        BlrFreeMem(pNicItem);
    }

}
///////////////////////////////////////////////////////////////
void BlrudpNicItemDelete(BLRUDP_NIC_ITEM *pNicItem)
//
// Destroy a NIC item created by BlrudpNicItemNew...
//
// inputs:
//     pNicItem -- nic item to destroy.
// outputs:
//     none.
//
{
	BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
    ASSERT(pNicItem);

    #if BLHPK
        // remove from interrupt handler...
        // this will prevent stuff from going in here
        // when interrupts are flying...
    #endif

	
	BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
    if (pNicItem->link.next)
		DeQ(&pNicItem->link);
	BLRUDP_END_CRITICAL_SECTION(ulCrit);

    // if there is stuff in here.. somebody did not clean up....
    ASSERT(QisEmpty(&pNicItem->listRecvPktPool));

    
	// Actually at this point the interrupts should not be active, but 
	// just in case, make sure..
	//
    // empty it anyway...., once this goes away.
    // the memory is lost.....
    //
    while (QisFull(&pNicItem->listRecvPktPool))
    {
        _QLink *pData;
		BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
		BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
        pData = QBegin(&pNicItem->listRecvPktPool);
		if (pData == QEnd(&pNicItem->listRecvPktPool))
		{
			BLRUDP_END_CRITICAL_SECTION(ulCrit);
			break;
		}
        DeQ(pData);                 // unlink
		BLRUDP_END_CRITICAL_SECTION(ulCrit);

        BlrFreeMem(pData);          // and dump the data.
    }


	//
	// NOT so sure if this is always going to be empty when
	// we shut down the queue...
	#if (defined(_DEBUG) && defined(BLHPK))
	{
		BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
		BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
		ASSERT(QisEmpty(&pNicItem->listRecvPkts));
		BLRUDP_END_CRITICAL_SECTION(ulCrit);
	}		
	#endif

    //
    // empty it anyway...., once this goes away.
    // the memory is lost.....
    //
	//
	// Version of this loop with critical sections....
    #ifdef BLHPK
	    while (TRUE)
	    {
            _QLink *pData;
		    BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
		    
		    BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
            pData = QBegin(&pNicItem->listRecvPkts);
		    if (pData == QEnd(&pNicItem->listRecvPkts))
		    {
			    BLRUDP_END_CRITICAL_SECTION(ulCrit);
			    break;
		    }
            DeQ(pData);                 // unlink
		    BLRUDP_END_CRITICAL_SECTION(ulCrit);
            BlrFreeMem(pData);          // and dump the data.
	    }
    #endif
    
    
}


//////////////////////////////////////////////////////////////////
BLRUDP_NIC_ITEM *BlrudpFindNicItem(struct T_BLRUDP_MGR*pBlrudp,
                                   unsigned long  ulIpAddr)
//
// find the NIC item associated with this NIC....
//
// inputs:
//    pBlrudp -- pointer to the Blrudp instance
//    IpAddr -- ip address in network order.
// outputs:
//    returns -- pointer to found NIC item if there is one.
//               or NULL if there is not...
//
{
    BLRUDP_NIC_ITEM *pNicItem;
    BLRUDP_NIC_ITEM *pFoundItem = NULL;
    for (pNicItem =  (BLRUDP_NIC_ITEM *)QBegin(&pBlrudp->listLocalNICs);
         (&pNicItem->link != QEnd(&pBlrudp->listLocalNICs));
         pNicItem = (BLRUDP_NIC_ITEM *)pNicItem->link.next)
    {
        if (pNicItem->ulIpAddr == ulIpAddr)
        {
            pFoundItem = pNicItem;
            break;
        }
    }
    return(pFoundItem);
}
//////////////////////////////////////////////////////////////
int BlrudpAddNicRecvPkts(BLRUDP_NIC_ITEM *pLocalNic,
                            uint32_t ulNumPkts)
//
// Add the specified number of receive packets to the NIC...
// All receive packets will be ulMtuSize.
//
// This allocation works as a credit debit scheme.  The
// packet pool asks for the number of packets it cares about
// to be allocated.  When it is done, it requests the
// packets to be removed....
//
// inputs:
//    pLocalNic -- local NIC to allocate new packets into.
//    ulNumPkts -- number of packets to add.
// outputs:
//    returns -- BLERR_SUCCESS if successful.
// 
{
    int nErr = BLERR_INTERNAL_ERR;
    uint32_t n;
    uint32_t ulNumAllocated = 0;
    uint16_t usSizePkt;


    ASSERT(pLocalNic->uMtuSize);
    if (pLocalNic->uMtuSize == 0)
        FAILERR(nErr, BLERR_PARAM_ERR);
    
    usSizePkt = pLocalNic->uMtuSize;    // use MTU size for all recv packets...



    //////////////////////////////////////////////////////////////////////
    // TBD.. Separate the concept of the packet 
    // container and the memory it controls...
    //
    // The packet payload is the only thing that needs to be alligned...
    //
    for (n = 0; n < ulNumPkts; n++)
    {
        BLRUDP_IO_PKT *pPkt;
		BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
		
        pPkt = BlrudpPktNew(usSizePkt);
        if (pPkt == NULL)
            FAILERR(nErr, BLERR_NO_MEMORY);
        //
        // TBD.. critical section....

		BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
		EnQ(&pPkt->link, &pLocalNic->listRecvPktPool);
		
        pLocalNic->lTotalNicPkts++;           // total credit of NIC packets.
        pLocalNic->lPktsInPool++;             // number of packets in the pool.
		BLRUDP_END_CRITICAL_SECTION(ulCrit);
        //////////////////////////////////////////////
        // keep track of how far we got....
        ulNumAllocated++;       
    }

    return(BLERR_SUCCESS);

Fail:
    BlrudpRemoveNicRecvPkts(pLocalNic, ulNumAllocated);        // undo what we did...
    return(nErr);
}

////////////////////////////////////////////////////////////////
void BlrudpRemoveNicRecvPkts(BLRUDP_NIC_ITEM *pLocalNic,
                            uint32_t ulNumPkts)
//
// Remove the specified number of receive packets to the NIC...
//
// inputs:
//    pLocalNic -- local NIC to allocate new packets into.
//    ulNumPkts -- number of packets to add.
// outputs:
//    returns -- BLERR_SUCCESS if successful.
// 
{
    _QLink *pPktQueue = &pLocalNic->listRecvPktPool;
    while (ulNumPkts) 
    {
        BLRUDP_IO_PKT *pPkt;
		BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
        
		// Total Nic pkts should never go negative....
        ASSERT(pLocalNic->lTotalNicPkts);
        if (pLocalNic->lTotalNicPkts > 0)
            pLocalNic->lTotalNicPkts--;           // total credit of NIC packets.
        // validate our counters against the actual queue...        
		
		BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
		ASSERT( ((pLocalNic->lPktsInPool > 0) && (QisFull(pPktQueue))) ||
				((pLocalNic->lPktsInPool <= 0) && (QisEmpty(pPktQueue))) );
        // this number can go negative if there are packets out in the other parts of the system.
        pLocalNic->lPktsInPool--;  

        
		// packets may be out in other queues and not yet returned here...
        if (QisFull(pPktQueue))
        {
            pPkt =  (BLRUDP_IO_PKT *)QBegin(pPktQueue);
            DeQ(&pPkt->link);          
			BLRUDP_END_CRITICAL_SECTION(ulCrit);     
            BlrudpPktDelete(pPkt);
        }
		else
        {
			BLRUDP_END_CRITICAL_SECTION(ulCrit);     
        }
        //

        ulNumPkts--;        // count them down.
    }
    return;
}

////////////////////////////////////////////////////////////////
void BlrudpPutNicFreeRecvPkt(BLRUDP_NIC_ITEM *pLocalNic,
                         BLRUDP_IO_PKT *pPkt)
//
// return a packet to the NIC free receive packet list.
// 
// inputs:
//    pLocalNic -- local NIC to put packets into.
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
// WARNING!!!!WARNING!!!!!WARNING
// Don't EVER call this at interrupt time.  This will
// call a free memory routine under certain circumstances.
//
{
    // TBD.. critical section..
	BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
	
	BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
    ASSERT(pLocalNic->lPktsInPool <= pLocalNic->lTotalNicPkts);
    ////////////////////////////////////////////////////////////////////

    // If this is true then this means that the packet pool credit was
    // reduced while there were packets still in the system and
    // now they are being returned to the pool.  They need to be
    // deleted now..
    pLocalNic->lPktsInPool++;
    if (pLocalNic->lPktsInPool <= pLocalNic->lTotalNicPkts)
    {
        pPkt->ulDestAddr     = 0L;      // clear this out.. so we don't do stupid
        pPkt->nDestPort      = 0;       // things with OLD data...
        pPkt->ulSrcAddr      = 0L;
        pPkt->nSrcPort       = 0;
        EnQ(&pPkt->link, &pLocalNic->listRecvPktPool);
		BLRUDP_END_CRITICAL_SECTION(ulCrit);
    }
    else
    {
		BLRUDP_END_CRITICAL_SECTION(ulCrit);
        BlrudpPktDelete(pPkt);
    }
}

////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetNicFreeRecvPkt(BLRUDP_NIC_ITEM *pLocalNic)
//
// Get a packet from the NIC free receive packet list.
// 
// inputs:
//    pLocalNic -- local NIC to allocate get packets from...
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
{
     BLRUDP_IO_PKT *pPkt =  NULL;
    _QLink *pPktQueue = &pLocalNic->listRecvPktPool;
	BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
    //
    // TBD... critical section....
    //

	BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
    ASSERT( ((pLocalNic->lPktsInPool > 0) && (QisFull(pPktQueue))) || 
            ((pLocalNic->lPktsInPool <= 0) && (QisEmpty(pPktQueue))) );
    ASSERT(pLocalNic->lPktsInPool <= pLocalNic->lTotalNicPkts);
    if (QisFull(pPktQueue))
    {
        ASSERT(pLocalNic->lPktsInPool);        // this should reflect the packets here...
        if (pLocalNic->lPktsInPool)
            pLocalNic->lPktsInPool--;
        pPkt =  (BLRUDP_IO_PKT *)QBegin(pPktQueue);
        DeQ(&pPkt->link);               
    }
    BLRUDP_END_CRITICAL_SECTION(ulCrit);
    
    return(pPkt);

}

#ifdef BLHPK
////////////////////////////////////////////////////////////////
void BlrudpPutNicRecvdPkt(BLRUDP_NIC_ITEM *pLocalNic,
                         BLRUDP_IO_PKT *pPkt)
//
// return a packet to the NIC full received packet list.
// 
// inputs:
//    pLocalNic -- local NIC to put packets into.
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
// WARNING!!!!WARNING!!!!!WARNING
// Don't EVER call this at interrupt time.  This will
// call a free memory routine under certain circumstances.
////////////////////////////////////////////////////////////////
{
	BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);
    //
    // If we call this we MUST already know the src and dest ports...
    //
    ASSERT(pPkt->ulSrcAddr && pPkt->nSrcPort && pPkt->ulDestAddr && pPkt->nDestPort);
	
	BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
    EnQ(&pPkt->link, &pLocalNic->listRecvPkts);
	BLRUDP_END_CRITICAL_SECTION(ulCrit);
}


////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetNicRecvdPkt(BLRUDP_NIC_ITEM *pLocalNic)
//
// Get a packet from the NIC full received packet list.
// 
// inputs:
//    pLocalNic -- local NIC to allocate get packets from...
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////
{
    BLRUDP_IO_PKT *pPkt = NULL;
	BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);

    //
    // Critical section
	BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
    if (QisFull(&pLocalNic->listRecvPkts))
	{
        pPkt =  (BLRUDP_IO_PKT *)QBegin(&pLocalNic->listRecvPkts);
		DeQ(&pPkt->link);
	}
    BLRUDP_END_CRITICAL_SECTION(ulCrit);

    return(pPkt);       // return the result...
}
////////////////////////////////////////////////////////////////////
int BlrudpNicRecvdDataReady(BLRUDP_LOCAL_SAP_ITEM  *pLocalSap)
//
// Look at the SAP receive buffer and return TRUE if there is data in it.
// 
// inputs:
//    pLocalSap -- local sap associated with the receive data.
// outputs:
//    returns -- TRUE if there is data available, FALSE if not.
//
///////////////////////////////////////////////////////////////////
{
	int bDataReady;
	BLRUDP_DEFINE_CRITICAL_SECTION(ulCrit);

    //
    // Critical section
	BLRUDP_BEGIN_CRITICAL_SECTION(ulCrit);
    bDataReady = QisFull(&pLocalSap->pLocalNic->listRecvPkts);
    BLRUDP_END_CRITICAL_SECTION(ulCrit);
	
	return(bDataReady);

}


#endif      // def BLHPK

////////////////////////////////////////////////////////////////
BLRUDP_LOCAL_SAP_ITEM *BlrudpLocalSapNew(struct T_BLRUDP_MGR*pBlrudp,
                                         unsigned long  ulIpAddr,
                                         unsigned short nPort)
//
// Create a new sap control structure and a preliminary
// initalization of the control structure.
//
// inputs:
//    pBlrudp -- pointer to the Blrudp instance
//    IpAddr -- ip address in network order.
//    nPort -- port number.
// outputs:
//    returns -- pointer to newly allocated SAP
//               NULL if out of memory.
{
    BLRUDP_LOCAL_SAP_ITEM *pLocalSap;
    int nRet;

    pLocalSap =  BlrAllocMem(sizeof(*pLocalSap));
    if (pLocalSap == NULL)
        FAIL;
    memset(pLocalSap, 0, sizeof(*pLocalSap ));

    pLocalSap->link.next = pLocalSap->link.prev = NULL;
    pLocalSap->ulRefCount = 1;
    pLocalSap->Sap.ulIpAddr = ulIpAddr;
    pLocalSap->Sap.nPort = nPort;
    pLocalSap->hSocket = INVALID_SOCKET;

    #ifdef BLHPK
    QInit(&pLocalSap->listRecvPkts);
    #endif

    //////////////////////////////////////////////////////
    // add this to the sap control map.
    //
    nRet = BlMapInsert(pBlrudp->mapLocalSaps,       // pBlMap
                        &pLocalSap->Sap,             // pKey
                        pLocalSap);                  // pItem.
    if (nRet != BLERR_SUCCESS)
        FAIL;
    
    pLocalSap->pXmitWorkMgr = BlrWorkNew();
    if (pLocalSap->pXmitWorkMgr == NULL)
        FAIL;
    
    pLocalSap->pLocalNic = BlrudpFindNicItem(pBlrudp, ulIpAddr);
    if (pLocalSap->pLocalNic == NULL)
        pLocalSap->pLocalNic = BlrudpNicItemNew(pBlrudp, ulIpAddr);
    if (pLocalSap->pLocalNic == NULL)
        FAIL;
    pLocalSap->pLocalNic->ulRefCount++;

    EnQ(&pLocalSap->link, &pBlrudp->listLocalSaps);
    pBlrudp->ulNumLocalSaps++;


    return(pLocalSap);
Fail:
    if (pLocalSap)
        BlrudpLocalSapDelete(pBlrudp, pLocalSap);

    return(NULL);
}
/////////////////////////////////////////////////////
void BlrudpLocalSapDelete(struct T_BLRUDP_MGR   *pBlrudp,
                          BLRUDP_LOCAL_SAP_ITEM *pLocalSap)
//
// Delete the sap control structure.  
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pLocalSap -- sap control structure.
// outputs:
//    none.
//
{
    ASSERT(pLocalSap->ulRefCount == 0);

    BlrIoCloseSocket(pLocalSap->hSocket);
    BlMapRemove(pBlrudp->mapLocalSaps,      // pBlMap
                &pLocalSap->Sap);            // pKey

    //
    
    if (pLocalSap->link.next)
    {
        ASSERT(pBlrudp->ulNumLocalSaps);
        if (pBlrudp->ulNumLocalSaps)
            pBlrudp->ulNumLocalSaps--;
        DeQ(&pLocalSap->link);       // remove it from the link.
    }


    if (pLocalSap->pXmitWorkMgr)
        BlrWorkDelete(pLocalSap->pXmitWorkMgr);

    if (pLocalSap->pLocalNic)
    {
        #ifdef BLHPK
        while (QisFull(&pLocalSap->listRecvPkts))
        {   
            BLRUDP_IO_PKT *pPkt;
            pPkt = (BLRUDP_IO_PKT *)QBegin(&pLocalSap->listRecvPkts);
            BlrudpPutNicFreeRecvPkt(pLocalSap->pLocalNic,
                                    pPkt);
        }
        #endif
        BlrudpNicItemRelease(pLocalSap->pLocalNic);
    }


    BlrFreeMem(pLocalSap);

}

/////////////////////////////////////////////////////////////
void BlrudpLocalSapRelease(struct T_BLRUDP_MGR* pBlrudp,
                           BLRUDP_LOCAL_SAP_ITEM *pLocalSap)
//
// Release the sap control structure.  This decrements
// the reference count and if it reaches zero it closes the
// socket and dumps the control structure.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pLocalSap -- sap control structure.
// outputs:
//    none.
//
{
    ASSERT(pLocalSap->ulRefCount);       // should have dumped it by now...
    if (pLocalSap->ulRefCount)
        pLocalSap->ulRefCount--;

    ///////////////////////////////////////////////////////////////////
    //
    // refcount all the way down, dump it.
    //
    if (pLocalSap->ulRefCount == 0)
    {
        BlrudpLocalSapDelete(pBlrudp, pLocalSap);
        BlrudpLocalSapBuildReadSelects(pBlrudp);
    }

}

//////////////////////////////////////////////////////////////////////
int BlrudpLocalSapSetMtuSize(BLRUDP_LOCAL_SAP_ITEM *pLocalSap)
//
// Set the MTU size associated with this SAP. 
//
// inputs:
//    pLocalSap -- pointer to the local sap to set the mtusize for.
// outputs:
//    returns -- BLERR_SUCCESS if successful..
//
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    //
    // if we don't have a MTU size yet.  then try to get one.
    //
    if (pLocalSap->pLocalNic->uMtuSize == 0)
    {
        nRet = BlrIoGetMtuFromAddr(pLocalSap->Sap.ulIpAddr,
                                    &pLocalSap->pLocalNic->uMtuSize,
									&pLocalSap->pLocalNic->uPayloadSize);
        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, BLERR_BLRUDP_SOCKET_ERR);
    }
    pLocalSap->uMtuSize = pLocalSap->pLocalNic->uMtuSize;
    pLocalSap->uPayloadSize = pLocalSap->pLocalNic->uPayloadSize;

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}


//////////////////////////////////////////////////////////////
void BlrudpLocalSapBuildReadSelects(struct T_BLRUDP_MGR* pBlrudp)
//
// Build the select and poll strcutures for ALL of the local
// saps in the local sap list..
//
// inputs:
//    pBlrudp -- pointer to blrudp structure to build list for.
// outputs:
//    none.
//
//
{
#if defined(BLHPK)
    return;
#else
    BLRUDP_LOCAL_SAP_ITEM *pSap;
    // 
    // as each local sap builds a read set..
    // write sets are only built for those saps that
    // have outstanding writes...
    //
    FD_ZERO(&pBlrudp->fdsetRead);                  

    for (pSap = (BLRUDP_LOCAL_SAP_ITEM *)QBegin(&pBlrudp->listLocalSaps);
         &pSap->link != QEnd(&pBlrudp->listLocalSaps);
         pSap = (BLRUDP_LOCAL_SAP_ITEM *)pSap->link.next)
    {
        FD_SET(pSap->hSocket, &pBlrudp->fdsetRead);
        
    }
#endif
}    
//////////////////////////////////////////////////////////////
void BlrudpLocalSapBuildWriteSelects(struct T_BLRUDP_MGR* pBlrudp,
                                     SOCKET *phHighestSocketFd)
//
// Build the select and poll strcutures for ALL of the local
// saps in the local sap list..
//
// inputs:
//    pBlrudp -- pointer to blrudp structure to build list for.
//    phHighestSocketFd -- pointer to where to stuff the highest
//                         socket descriptor.
//
// outputs:
//    *phHighestSocketFd -- highest socket descriptor found...
//
//
{
#if defined(BLHPK)
    return;
#else
    BLRUDP_LOCAL_SAP_ITEM *pSap;
    SOCKET hHighFd = INVALID_SOCKET;
    // 
    // as each local sap builds a read set..
    // write sets are only built for those saps that
    // have outstanding writes...
    //
    FD_ZERO(&pBlrudp->fdsetWrite);                  

    for (pSap = (BLRUDP_LOCAL_SAP_ITEM *)QBegin(&pBlrudp->listLocalSaps);
         &pSap->link != QEnd(&pBlrudp->listLocalSaps);
         pSap = (BLRUDP_LOCAL_SAP_ITEM *)pSap->link.next)
    {
        if ((hHighFd < pSap->hSocket) || (hHighFd == INVALID_SOCKET))
            hHighFd = pSap->hSocket;
        // TBD.. only if we have outstanding writes..
        if (BlrWorkNumItems(pSap->pXmitWorkMgr))
        {
            FD_SET(pSap->hSocket, &pBlrudp->fdsetWrite);
        }
    }
    if (phHighestSocketFd)
        *phHighestSocketFd = hHighFd;

#endif
}    

#ifdef BLHPK
////////////////////////////////////////////////////////////////
void BlrudpLocalSapPutRecvdPkt(BLRUDP_LOCAL_SAP_ITEM *pLocalSap,
                         BLRUDP_IO_PKT *pPkt)
//
// return a packet to the Local Sap full received packet list.
// 
// inputs:
//    pLocalNic -- local NIC to put packets into.
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
// WARNING!!!!WARNING!!!!!WARNING
// Don't EVER call this at interrupt time.  This will
// call a free memory routine under certain circumstances.
////////////////////////////////////////////////////////////////
{
    EnQ(&pPkt->link, &pLocalSap->listRecvPkts);
}


////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpLocalGetSapRecvdPkt(BLRUDP_LOCAL_SAP_ITEM  *pLocalSap)
//
// Get a packet from the localSap received packet list.
// 
// inputs:
//    pLocalNic -- local NIC to allocate get packets from...
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////
{
    BLRUDP_IO_PKT *pPkt = NULL;

    if (QisFull(&pLocalSap->listRecvPkts))
	{
        pPkt =  (BLRUDP_IO_PKT *)QBegin(&pLocalSap->listRecvPkts);
		DeQ(&pPkt->link);
	}
    // end critical section.

    return(pPkt);       // return the result...
}

//////////////////////////////////////////////////////////////////////////
void BlrudpNicRun(_QLink *plistLocalNICs,
                  BLMAP_HANDLE mapLocalSaps)
//
// 
// Perform run time maintenance for the local NICS.
//
// only need to call this for the bluelight high performance kernel.
//
// this process looks at each NIC and checks its 
// Received Packet queue.  It then distributes the received packets
// to each of the local SAP queues...  At this point the BlrIo routines 
// can handle the interface...
//
// inputs:
//    plistLocalNICs -- the complete list of the local NICs to service.
//    mapLocalSaps -- map of the local saps to look up the addresses in.
// outputs:
//    none.
//
{
    BLRUDP_NIC_ITEM *pLocalNic;
    
	for (pLocalNic = (BLRUDP_NIC_ITEM *)QBegin(plistLocalNICs);
         pLocalNic != (BLRUDP_NIC_ITEM *)QEnd(plistLocalNICs);
         pLocalNic = (BLRUDP_NIC_ITEM *)pLocalNic->link.next)
    {
        BLRUDP_IO_PKT *pPkt;
        while (TRUE)
        {
            BLRUDP_LOCAL_SAP_ITEM *pLocalSap = NULL;
            BLRUDP_SAP_ADDR Sap;
            pPkt = BlrudpGetNicRecvdPkt(pLocalNic);
            if (pPkt == NULL)
                break;
            
			Sap.ulIpAddr = pPkt->ulDestAddr;
            Sap.nPort    = pPkt->nDestPort;
            pLocalSap    = BlMapFindItem(mapLocalSaps, &Sap);

            if (pLocalSap == NULL)                      // not found, just return the packet.
                BlrudpPutNicFreeRecvPkt(pLocalNic,   pPkt);
            else
			{
                BlrudpLocalSapPutRecvdPkt(pLocalSap, pPkt);
			}
        }


    }

}
////////////////////////////////////////////////////////////////////
int BlrudpLocalSapRecvdDataReady(BLRUDP_LOCAL_SAP_ITEM  *pLocalSap)
//
// Look at the SAP receive buffer and return TRUE if there is data in it.
// 
// inputs:
//    pLocalSap -- local sap associated with the receive data.
// outputs:
//    returns -- TRUE if there is data available, FALSE if not.
//
///////////////////////////////////////////////////////////////////
{
	return(QisFull(&pLocalSap->listRecvPkts));
}

#endif
