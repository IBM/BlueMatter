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
 #ifdef BLHPK
///////////////////////////////////////////////////////////////////////////////////
//
// Bluelight Reliable UDP io routines: for the high performance kernel...
//
// This file
//
////////////////////////////////////////////////////////////////////////

#include "hostcfg.h"



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

#if  HAVE_HPK_TYPES_H
#include <linux/types.h>
#endif

#if HAVE_LINUX_IN_H
	#include "linux/in.h"
#endif


#ifdef BLHPK
	#undef PACKED                   // ket blhpk define this
	#include <config.h>
	#include <linux/ip.h>
	#include <linux/udp.h>
	#include <sys/blhpk_types.h>
	#include <../drivers/eth_ip.h>
	#include <../drivers/405gp_emac3.h>
#endif

#include "BlErrors.h"
#include "blrudp_io.h"

#include "dbgPrint.h"
#include "blrudp_memory.h"
#include "bldefs.h"



/////////////////////////////////////////////////////////////////////
//
// We will have to  do sockets for more than one if we ever have 
// more than one ethernet device....
//
// I.e. there can be multiple sockets, one associated with each port.
// but they all must agree on the ethernet address.....
//
#define THE_SOCKET_HANDLE	101		// there can only be one socket for now....

////////////////////////////////////////////////////////////////////////
//
// Control structure maintaining information about our 
// socket control structure.
//
typedef struct {
    uint32_t ulIpAddr;      				// ip address of this socket...
    uint16_t nPort;         				// 
    int bIsSocketOpen;        				// true if the socket is open.
	BLRUDP_LOCAL_SAP_ITEM *pLocalSap;		// local sap item associated with this socket.
} IOHPK_SOCKET_MGR;

IOHPK_SOCKET_MGR g_TheSocketMgr = {
    0,          // ulIpAddr;      // ip address of this socket...
    0,          // nPort;         // 
    0,          // bIsSocketOpen;        
	NULL,		// pLocalSap
};

///////////////////////////////////////////////////////////////////
int BlrIoDgramRecvReady(SOCKET hSocket)
//
//  Polls our socket for receive I/O availability
//
// inputs:
//    hSocket -- socket handle to check.
// outputs:
//   returns -- TRUE if receive data is ready, FALSE if not.
//
{
	int bRcvReady;
	if (g_TheSocketMgr.pLocalSap == NULL)
		FAIL;
	bRcvReady = BlrudpLocalSapRecvdDataReady(g_TheSocketMgr.pLocalSap);
	
	return(bRcvReady);			
Fail:
	return(FALSE);	
}

//////////////////////////////////////////////////////////////////////////
int BlrIoDgramXmitReady(SOCKET hSocket)
//
//  Polls our socket for transmit I/O availability
//
// inputs:
//     hSocket -- socket to check for data gram ready.
// 
// outputs:
//     TRUE if transmit data is ready, FALSE if not.
//
{
	if (hSocket != THE_SOCKET_HANDLE)
	    return(FALSE);

	// TBD.. define this
	return(EthIP_TX_Ready());			
}
///////////////////////////////////////////////////////////////////
int BlrIoGetLastSocketError()
//
// Retrieve the last socket error.
//
// This is one of those operations that is different between
// Windows and Eunics.
//
// inputs:
//    inputs:
// outputs:
//  returns -- last socket error.
//
{
	return(0);
}


////////////////////////////////////////////////////////
int BlrIoGetMtuFromAddr(unsigned long ulIpAddr UNUSED,
                        unsigned int *pMtuSize,
						unsigned int *pPayloadSize)
//
// Retrieve the MTU size associated with this local socket and
// IP address.
// 
// inputs:
//    hSocket -- open socket to use for IOCTL calls.
//    ulIpAddr -- IP address to use to search for the NIC
//                we want to grab this info from.
//    pMtuSize -- place to stuff the MTU size.
//    pPayloadSize -- place to stuff the max payload size...
//
// outputs:
//    returns -- BLERR_SUCCESS if successful.
//    *pMtuSize -- MTU size of device.
//    *pPayloadSize -- maximum payload size (MTU - IP and UDP headers).
//
//
{
	*pMtuSize = ENET_MAX_MTU;	    // allow 20 for the udp header
	
	//
	// NOTE: if the HPK code every starts using options in its transmission
	// fields then we need to change this...
	*pPayloadSize = ENET_MAX_MTU - sizeof(struct iphdr) - sizeof(struct udphdr);

	//
	// NOTE: we also need maximum payload size, subtracting off
	// the UDP and IP headers....
	//
	return(BLERR_SUCCESS);
}


/////////////////////////////////////////////////////////////////
SOCKET BlrIoCreateSocket(BLRUDP_LOCAL_SAP_ITEM *pLocalSap,
						 unsigned long ulIpAddr,
                         unsigned short nPort,
                         unsigned short *pActPortNo);
//
// Create a new socket and return the handle to the caller.
//
// inputs:
//    pLocalSap -- pointer to the local SAP  object associated with this socket...
//    IpAddr -- ip address in network order.
//    nPort -- port number.
//             the port number zero is a special case and is used
//             to create a port number dynamically.
//             This allows the bind function to assign the port number
//    pActPortNo -- place to stuff the actual port number assigned.
//             equal to nPort if nPort was NOT zero.
// outputs:
//    hSocket -- socket handle or INVALID_SOCKET if we had a problem.
//
////////////////////////////////////////////////////////////////////////////
{
    if (g_TheSocketMgr.bIsSocketOpen)
    {
        dbgPrintf("Hey, the silly socket is already open...\n");
        FAIL;
    }
    
    if (nPort == 0)     // leave this unsupported for now..
        FAIL;
	
    g_TheSocketMgr.ulIpAddr = ulIpAddr;
    g_TheSocketMgr.nPort = nPort;
	g_TheSocketMgr.pLocalSap = pLocalSap;
    //

	return(THE_SOCKET_HANDLE);
Fail:
    return(INVALID_SOCKET);
}

//////////////////////////////////////////////////////////////////////////
void BlrIoCloseSocket(SOCKET hSocket)
//
// Close the specified socket...
//
// inputs:
//    hSocket -- socket to close.
// outputs:
//    none.
//
///////////////////////////////////////////////////////////////////////
{
    if (hSocket != THE_SOCKET_HANDLE)
        FAIL;
        
    g_TheSocketMgr.bIsSocketOpen = FALSE;
	g_TheSocketMgr.pLocalSap = NULL;
	return;
Fail:
    return;    
}



//////////////////////////////////////////////////////////////////////////
int BlrIoRecvPkt(BLRUDP_LOCAL_SAP_ITEM *pLocalSap,
                 BLRUDP_IO_PKT **ppPkt,
                 BLRUDP_SAP_ADDR *pSapAddr)
//
// Receive a packt from a local Sap
//
// inputs:
//    pLocalSap -- local sap to receive packet from
//    pPkt -- packet to receive data into.
//            if (pPkt == NULL) then just receive the packet and
//            throw away the data...
//    pSapAddr -- place to stuff the sap address...
// outputs:
//    returns -- BLERR_SUCCESS if successful.
//               error codes TBD...
//////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_IO_PKT *pRecvPkt = NULL;


	ASSERT(pLocalSap);										// be defensive..
	if (pLocalSap == NULL)
		FAILERR(nErr, BLERR_BLRUDP_SOCKET_RCV_ERR);         // punt.....
		

	pRecvPkt = BlrudpLocalGetSapRecvdPkt(g_TheSocketMgr.pLocalSap);
	
	if (pRecvPkt == NULL)
		FAILERR(nErr, BLERR_BLRUDP_SOCKET_RCV_ERR);         // punt.....
	
	



	if (pSapAddr)
	{
		pSapAddr->ulIpAddr  = pRecvPkt->ulSrcAddr; 
		pSapAddr->nPort     = pRecvPkt->nSrcPort;    
	}
    #ifdef _DEBUG 
    {
        BLRUDP_PKT_HDR *pPktHdr;
        pPktHdr = BlrudpGetPktPayload(pRecvPkt);
        #if TRACE_PROTOCOL
        dbgPrintf("<<<(usCmd(%lx), usDlen(%lx), ulSync(%lx), ulSreq(%lx), ulRseq(%lx)\r\n",
                  ntohs(pPktHdr->usCmd),
                  ntohs(pPktHdr->usDlen),
                  ntohl(pPktHdr->ulSync),
                  ntohl(pPktHdr->ulSseq),
                  ntohl(pPktHdr->ulRseq));
        #endif
    }
    #endif
    
	if (ppPkt)
    {
        *ppPkt = pRecvPkt;                          // return it to the caller.
    }
    else
    {
		// throw away the packet...
		BlrudpPutNicFreeRecvPkt(pLocalSap->pLocalNic, pRecvPkt);     // return the packet..
		pRecvPkt = NULL;
    }
    return(BLERR_SUCCESS);
Fail:
    if (pRecvPkt)
        BlrudpPutNicFreeRecvPkt(pLocalSap->pLocalNic, pRecvPkt);     // return the packet..
    return(nErr);
}

//////////////////////////////////////////////////////////////////////////
int BlrIoSendPkt(BLRUDP_LOCAL_SAP_ITEM *pLocalSap,
                 const BLRUDP_SAP_ADDR *pSapAddr,
                 BLRUDP_IO_PKT *pPkt)
//
// Send a packet to the destination specified by the sap address.
// 
// inputs:
//    pLocalSap -- local sap to use to send this packet.
//    pSapAddr -- pointer to sap address to use to address this packet.
//    pPkt -- packet to send.
// outputs:
//    returns -- BLERR_SUCCESS if successful.
//               error codes TBD...
//////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRet;
    void *pPktBuff;
    int nLen;

    #ifdef _DEBUG
    {                   
        // in debug only.  Take the time here to validate that we setup the packet header 
        // correctly...
        BLRUDP_PKT_HDR *pPktHdr;
        pPktHdr = BlrudpGetPktPayload(pPkt);
        ASSERT(ntohs(pPktHdr->usDlen) == pPkt->usPktLen);   // these must agree...

        // 
        // display the packet we are sending...
        //
        #if TRACE_PROTOCOL
        dbgPrintf(">>>(usCmd(%lx), usDlen(%lx), ulSync(%lx), ulSreq(%lx), ulRseq(%lx)\r\n",
                  ntohs(pPktHdr->usCmd),
                  ntohs(pPktHdr->usDlen),
                  ntohl(pPktHdr->ulSync),
                  ntohl(pPktHdr->ulSseq),
                  ntohl(pPktHdr->ulRseq));
        #endif

    }
    #endif

    pPktBuff = BlrudpGetPktBuffer(pPkt);

    // Perform long word alignment.
    //
    nLen = (pPkt->usPktLen + (sizeof(uint32_t)-1)) & (~(sizeof(uint32_t)-1));
        
    
    
    nRet =  hpk_sendto( pSapAddr->ulIpAddr,          // rmt_addr, 
                        pSapAddr->nPort,             // rmt_port, 
                        pLocalSap->Sap.nPort,        // src_port, 
                        pPktBuff,                    // *buf, 
                        nLen);             // len )

    if (nRet != 0)
        FAILERR(nErr, BLERR_BLRUDP_SEND_BUSY);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);    
}

///////////////////////////////////////////////////////////////////////
void hpk_recvfrom_isr(Bit32 ulSrcAddr,
                      Bit16 usSrcPort,
					  Bit32 ulDestAddr,
                      Bit16 usDestPort,
                      void *pData,
                      int nLen)
//
// Receive From ISR routine.  This statically linked routine is called from 
// Eth_IP_Pkt (which is the ethernet interrupt service routine).  
//
// It delivers a packet to the upper layers. 
// When this routine returns the packet is reclaimed by the driver for re-use.
// 
// Any udp ports that the kernel needs for its own operation are filtered
// piror to calling this function.
//
// inputs:
//    ulSrcAddr  -- source IP addr.
//    usSrcPort  -- source Port.
//    usDestPort -- destination port.
//    pData -- pointer to the received data.
//    nLen -- length of the data.
// outputs:
//   none.
//////////////////////////////////////////////////////////////////////////
{
	
	BLRUDP_IO_PKT 	*pRecvPkt = NULL;
	BLRUDP_NIC_ITEM *pLocalNic = g_TheSocketMgr.pLocalSap->pLocalNic;
	BLRUDP_PKT_HDR *pPktHdr;
	
	if (ulSrcAddr == 0xFFFFFFFFL)			// ignore broad cast stuff.
		FAIL;

	if (nLen < sizeof(BLRUDP_PKT_HDR))
	{
		dbgPrintf("Received packet too short, can't be Blrudp\r\n");
		FAIL;
	}
	
	pRecvPkt = BlrudpGetNicFreeRecvPkt(pLocalNic);
	if (pRecvPkt == NULL)
	{
		dbgPrintf("hpk_recvfrom_isr: Packets exausted\r\n");
		FAIL;
	}

	// save off the address..
	pRecvPkt->ulSrcAddr     = ulSrcAddr;
	pRecvPkt->nSrcPort      = usSrcPort;
	pRecvPkt->ulDestAddr    = ulDestAddr;
	pRecvPkt->nDestPort     = usDestPort;
	pRecvPkt->usPktLen 		= nLen;               
	
	
	//
	// for now copy the data....
	// TBD.. work out a scheme where we can swap buffers..
	//
	pPktHdr = BlrudpGetPktPayload(pRecvPkt);
	ASSERT(pPktHdr);
	if (pPktHdr == NULL)		// huh??
		FAIL;

	ASSERT(nLen <= pRecvPkt->usBufferLen);			// make sure we don't crap all over memory...
	if (nLen > pRecvPkt->usBufferLen)	
		nLen = pRecvPkt->usBufferLen;			//
		
	memcpy(pPktHdr, pData, nLen);		// copy the data.

	
	
	BlrudpPutNicRecvdPkt(pLocalNic,pRecvPkt);		 // put it on the receive queue....

	return;					// yea, success...
Fail:
    if (pRecvPkt)
        BlrudpPutNicFreeRecvPkt(pLocalNic, pRecvPkt);     // return the packet..
	return;
}


#endif

