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
 ///////////////////////////////////////////////////////////////////////////////////
//
// Bluelight Reliable UDP io routines.
//
////////////////////////////////////////////////////////////////////////

#include "hostcfg.h"

#include <stdio.h>

#if defined(WIN32) || defined(CYGWIN)
    #include <winsock.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
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

#include <string.h>
#include <errno.h>

#include "BlErrors.h"
#include "blrudp_io.h"

#include "dbgPrint.h"
#include "dbgtimelog.h"
#include "blrudp_memory.h"
#include "bldefs.h"


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
    int nHighestDescriptor=hSocket+1;  // socket descriptor for compatability with berklee sockets.
    fd_set readSet;
    struct timeval nWait = {0L, 0L};

    int nRet = 0 ;

    FD_ZERO(&readSet);
    FD_SET(hSocket, &readSet);

    nRet = select(nHighestDescriptor,     // socket descriptor, for compat. with berklee sockets.
                  &readSet,               // readfds
                  NULL,                   // writefds
                  NULL,                   // exceptfds,
                  &nWait);                // timeout
    if (nRet < 0)
    {
        int nErr;
        nErr = BlrIoGetLastSocketError();
        // TBD.. debug diagnostic message.
        FAIL;
    }


    return(nRet > 0);
Fail:
    return(FALSE);      // nothing here.
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
    int nHighestDescriptor=hSocket+1;  // socket descriptor for compatability with berklee sockets.
    fd_set writeSet;
    struct timeval nWait = {0L,0L};

    int nRet = 0 ;

    FD_ZERO(&writeSet);
    FD_SET(hSocket, &writeSet);


    nRet = select(nHighestDescriptor,     // socket descriptor, for compat. with berklee sockets.
                  NULL,                   // readfds
                  &writeSet,              // writefds
                  NULL,                   // exceptfds,
                  &nWait);                // timeout
    if (nRet < 0)
    {
        int nErr;
        nErr = BlrIoGetLastSocketError();
        // TBD.. debug diagnostic message.
        FAIL;
    }

    return(nRet > 0);
Fail:
    return(FALSE);      // nothing here.
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
    int nErr;

#if defined(WIN32) || defined(CYGWIN)
    nErr = WSAGetLastError();
#else
    nErr = errno;
#endif

    // TBD.. do the unix version of this

    return(nErr);
}

//////////////////////////////////////////////////////////////////////
void BlrIoSetSocketAddr(struct sockaddr_in *pSockAddr,
                         short nFamily,
                         unsigned short nPort,
                         unsigned long ulAddr)
//
// Set the specified socket address to the parameters specified.
//
// this handles differences between operating systems.
//
//
// inputs:
//  pSockAddr -- pointer to socket address to set.
//  nFamily -- Address family (must be AF_INET).
//  nPort -- IP port.
//  ulAddr -- IP address.
//
// return:
//    *pSockAddr -- socket address set.
//
{
    pSockAddr->sin_family = nFamily;
    pSockAddr->sin_port = htons(nPort); // port in network order
    pSockAddr->sin_addr.s_addr = ulAddr;
    memset(pSockAddr->sin_zero, 0, sizeof(pSockAddr->sin_zero));


}

/////////////////////////////////////////////////////////////////////////
void BlrIoGetSocketAddr(struct sockaddr_in *pSockAddr,
                         short *pnFamily,
                         unsigned short *pnPort,
                         unsigned long *pulAddr)
//
// Given A socket address structure extract the paramters.
//
// this handles differences between operating systems.
//
//
// inputs:
//  pSockAddr -- pointer to socket address extract.
//  pnFamily -- pointer to Address family
//  pnPort   -- pointer to IP port.
//  pulAddr  -- pointer to IP address.
// outputs:
//  returns -- none.
//  *pnFamily -- pointer to Address family
//  *pnPort   -- pointer to IP port.
//  *pulAddr  -- pointer to IP address.
//
//
{
    if (pnFamily)
        *pnFamily = pSockAddr->sin_family;
    if (pnPort)
        *pnPort = pSockAddr->sin_port;
    if (pulAddr)
    {
    #if defined(WIN32) || defined(CYGWIN)
        *pulAddr = pSockAddr->sin_addr.S_un.S_addr;
    #else
        *pulAddr = pSockAddr->sin_addr.s_addr;
    #endif
    }
}

////////////////////////////////////////////////////////
int BlrIoGetMtuFromAddr(unsigned long ulIpAddr,
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
    int nErr = BLERR_INTERNAL_ERR;
    char *pBuffer = NULL;
    SOCKET hSocket = INVALID_SOCKET;

    //
    // NOTE: we also need maximum payload size, subtracting off
    // the UDP and IP headers....
    //

    ASSERT(pMtuSize);
    if (pMtuSize ==NULL)
        FAIL;                   // just punt....

    #if !defined(WIN32) && !defined(CYGWIN)
    {
        struct ifconf IfConf;
        struct ifreq *pIfReq;
        struct ifreq *pIfReqEnd;
        int nRet;
        long SysConfigIoctl;


        hSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (hSocket == INVALID_SOCKET)
            FAIL;


        #if defined(AIX)
            //
            // AIX defines a slightly different IOCTL for this
            // function and cannot be queried with a zero length buffer
            // therefore, assume a worst case of 200 adapters and types
            // and use that....
            //
            SysConfigIoctl = OSIOCGIFCONF;
            IfConf.ifc_len = 200*sizeof(struct ifreq);
        #else
            SysConfigIoctl = SIOCGIFCONF;
            // for this call
            // first call gets the length....
            IfConf.ifc_len = 0;
            IfConf.ifc_buf = NULL;
            //IfConf.ifc_buf = (void *)pIfReq;
            nRet = ioctl(hSocket,       // socket descriptor
                         SysConfigIoctl,
                         &IfConf);
            if ((nRet != 0) || (IfConf.ifc_len == 0))
                FAILERR(nErr, BLERR_NOT_FOUND);
        #endif


        //
        // allocate enought memory for our structure.
        pBuffer = BlrAllocMem(IfConf.ifc_len);
        if (pBuffer == NULL)
            FAILERR(nErr, BLERR_NO_MEMORY);
        //
        // Second call, call and get the configuration
        // data
        IfConf.ifc_buf = pBuffer;
        nRet = ioctl(hSocket,       // socket descriptor
                     SysConfigIoctl,   // get configuration.
                     &IfConf);
        if ((nRet != 0) || (IfConf.ifc_len == 0))
            FAILERR(nErr, BLERR_NOT_FOUND);

        //
        // compute the end of the buffer to check in the loop
        //
        pIfReqEnd = (struct ifreq *)(pBuffer + IfConf.ifc_len);
        for (pIfReq = (struct ifreq *)pBuffer;
             pIfReq < pIfReqEnd; pIfReq++)
        {
            if (pIfReq->ifr_addr.sa_family == AF_INET)
            {
                struct sockaddr_in *pSockAddr;
                pSockAddr = (struct sockaddr_in *)&pIfReq->ifr_addr;

                if (pSockAddr->sin_addr.s_addr == ulIpAddr)
                {
                    struct ifreq ifr;
                    memcpy(ifr.ifr_name, pIfReq->ifr_name, sizeof(ifr.ifr_name));
                    nRet = ioctl(hSocket,       // socket descriptor
                                SIOCGIFMTU,     // cmd, Get interface MTU
                                &ifr);          // *argp);
                    if (nRet != 0)
                        FAILERR(nErr, BLERR_NOT_FOUND);
                    *pMtuSize = ifr.ifr_mtu;
                    //
                    // TBD.. perform a better calculation of this
                    // value....
                    *pPayloadSize = ifr.ifr_mtu - BLRUDP_PKT_UDP_HDR_LEN;
                    break;
                }
            }
        }

    }
    #else
        //
        // If we ever care about this, we are going to have to track the MTU size down the
        // Windows way....  Until then, just punt.
        *pMtuSize = 1500;      // for windows, just do a stupid constant...
        *pPayloadSize = *pMtuSize - BLRUDP_PKT_UDP_HDR_LEN;
    #endif

    if (pBuffer)
        BlrFreeMem(pBuffer);

    if (hSocket != INVALID_SOCKET)
        closesocket(hSocket);


    return(BLERR_SUCCESS);
Fail:
    if (pBuffer)
        BlrFreeMem(pBuffer);
    if (hSocket != INVALID_SOCKET)
        closesocket(hSocket);
    if (pMtuSize)
        *pMtuSize = 1500;      // for windows, just do a stupid constant...
    return(nErr);
}


////////////////////////////////////////////////////////////////////////
SOCKET BlrIoCreateSocket(BLRUDP_LOCAL_SAP_ITEM *pLocalSap UNUSED,
                         unsigned long ulIpAddr,
                         unsigned short nPort,
                         unsigned short *pActPortNo)
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
    SOCKET hSocket = INVALID_SOCKET;
    struct sockaddr_in SocketAddr;
    int nValue;
    unsigned long ulValue;
    int nRet;
    socklen_t nLen;


    //
    // dynamic port assignment, then we must have
    // a dynamic port return value.
    //
    if ((nPort == 0) && (pActPortNo == NULL))
        FAIL;

    BlrIoSetSocketAddr(&SocketAddr,              // rSocketAddr
                        AF_INET,                  // nFamily
                        nPort,                    // nPort
                        ulIpAddr);                // ulAddr;


    // create a new datagram socket.
    hSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (hSocket == INVALID_SOCKET)
        FAIL;


    // Without this call, bind may fail if we crash without closing the
    // socket.  The system keeps the socket in use for 30 minutes or more
    // so this is a REAL nusance during debug....
    // NOTE:  There may be an issue of stealing the bound port
    //        from another process when we do this.  Therefore it may be
    //        prudent to do this only in a debug build.
    nValue = 1;

    nRet = setsockopt(hSocket,                  // s
                      SOL_SOCKET,               // level
                      SO_REUSEADDR,             // optname
                      (char *)&nValue,          // optval
                      sizeof(nValue) );         // optlen
    ASSERT(nRet == 0);


    #if DISABLE_CHECKSUM
    nRet = setsockopt(hSocket,                  // s
                      SOL_SOCKET,               // level
                      SO_NO_CHECK,             // optname
                      (char *)&nValue,          // optval
                      sizeof(nValue) );         // optlen
    ASSERT(nRet == 0);
    #endif
    //
    // bind the socket to a specific port/socket pair on the NIC
    //

    nRet = bind(hSocket,                                // socket
                (struct sockaddr *)&SocketAddr,         // name
                sizeof(struct sockaddr_in));            // name length
    if (nRet == SOCKET_ERROR)
        FAIL;

    // dynamic binding??
    nLen = sizeof(struct sockaddr_in);
    if ((nPort == 0) && (pActPortNo))
    {
        nRet = getsockname(hSocket,                             // socket
                           (struct sockaddr *)&SocketAddr,      // name
                           &nLen);                              // name length
        if (nRet != 0)
            FAIL;
        *pActPortNo = ntohs(SocketAddr.sin_port);      // pick off the actual port asigned.
    }
    //
    // permit sending of broadcast messages.
    //
    nValue = 1;
    nRet = setsockopt(hSocket,             // socket
                      SOL_SOCKET,          // level
                      SO_BROADCAST,        // option name
                      (char *) &nValue,    // *pOptionValue
                      sizeof(nValue));     // nOptionLen

    if (nRet == SOCKET_ERROR)
        FAIL;

    //
    // Now set it up for non blocking read and writes.
    //
    ulValue = 0;
    nRet = ioctlsocket (hSocket,                // socket descriptor
                        FIONBIO,                // cmd, Enable or disable non-blocking mode
                        &ulValue);              // *argp);
    if (nRet == SOCKET_ERROR)
        FAIL;



    return(hSocket);
Fail:
    if (hSocket != INVALID_SOCKET)
        closesocket(hSocket);
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
    if (hSocket != INVALID_SOCKET)
        closesocket(hSocket);
}


#ifdef OBSOLETE
///////////////////////////////////////////////////////////////////////////
int BlrIoGetNextRecvPktAddr(SOCKET hSocket,
                            BLRUDP_SAP_ADDR *pSapAddr)
//
// Retrieve the address of the next packet we are about to receive...
//
// inputs:
//    hSocket -- socket to receive packet from.
//    *pSapAddr -- pointer to where to stuff the sap information.
//
// outputs:
//    return -- BLERR_SUCCESS if successful.
//    *pSapAddr->ulIpAddr -- ip address from packet in network byte order.
//    *pSapAddr->nPort -- port number of the packet..
//
//
{
    int nErr = BLERR_INTERNAL_ERR;
    int nRcvLen;
    struct sockaddr_in RcvFromSocket;
    socklen_t nRcvFromSocketLen = sizeof(RcvFromSocket);

    ASSERT(pSapAddr);

    //
    // First cut strategy.... Attempt to peek at a message,
    // with a zero length buffer, from this we extract the
    // source of the packet.  Then we can grab the correct
    // buffer from the buffer pool and have it fill it in.
    //
    //
    nRcvLen = recvfrom(hSocket,                       // socket
                       NULL,                          // *buf
                       0,                             // len
                       MSG_PEEK,                      // flags
                       (struct sockaddr *)&RcvFromSocket, // *from
                       &nRcvFromSocketLen);           // *fromlen
    #ifdef WIN32
        if (nRcvLen == SOCKET_ERROR)
        {
            //
            // Windows returns a special error code with
            // if the message buffer was too short.
            // Linux does not...  AIX.. Who knows???
            int nRet;
            nRet = BlrIoGetLastSocketError();

            if (nRet !=  WSAEMSGSIZE)
            {
                if (nRet == WSAECONNRESET)       // see if we can clear this condition.
                {
                    //
                    // for some reason, we can get this error when
                    // a transmission is sent to this NIC, but not on a socket
                    // that anyone is listening to.
                    // Select returns that there is something to receive.
                    // we go to receive it, and we perpetually get this error.
                    // Calling receive without the PEEK clears the condition....
                    //
                    recvfrom(hSocket,                       // socket
                             NULL,                          // *buf
                             0,                             // len
                             0,                             // flags
                             (struct sockaddr *)&RcvFromSocket, // *from
                             &nRcvFromSocketLen);           // *fromlen
                }
                FAILERR(nErr, BLERR_BLRUDP_SOCKET_RCV_ERR);       // punt.....
            }
        }
    #else
        if (nRcvLen == SOCKET_ERROR)
            FAILERR(nErr, BLERR_BLRUDP_SOCKET_RCV_ERR);       // punt.....
    #endif

    pSapAddr->ulIpAddr  = RcvFromSocket.sin_addr.s_addr;
    pSapAddr->nPort     = ntohs(RcvFromSocket.sin_port);


    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}
#endif

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
    int nRcvLen;
    struct sockaddr_in RcvFromSocket;
    socklen_t nRcvFromSocketLen = sizeof(RcvFromSocket);
    void *pPktBuff;
    BLRUDP_IO_PKT *pRecvPkt = NULL;

    pRecvPkt = BlrudpGetNicFreeRecvPkt(pLocalSap->pLocalNic);
    if (pRecvPkt == NULL)
        FAILERR(nErr, BLERR_BLRUDP_NO_PKTS);

    if (ppPkt)
    {
        pPktBuff = BlrudpGetPktBuffer(pRecvPkt);
        nRcvLen = pRecvPkt->usPayloadLen;
    }
    else
    {
        pPktBuff = NULL;
        nRcvLen = 0;
    }

    nRcvFromSocketLen = sizeof(RcvFromSocket);
    nRcvLen = recvfrom(pLocalSap->hSocket,                // socket
                       (char *)pPktBuff,                  // *buf
                       nRcvLen,                           // len
                       0,                                 // flags
                       (struct sockaddr *)&RcvFromSocket, // *from
                       &nRcvFromSocketLen);               // *fromlen
    if (nRcvLen == SOCKET_ERROR)
    {
        #ifdef WIN32
            int nRet;
            // if we send and no one is listening, then
            // we get this condition, nRcvLen == SOCKET_ERROR
            // last error == 0... or WSAECONNRESET
            //
            nRet = BlrIoGetLastSocketError();
            ASSERT((nRet == 0) || (nRet == WSAECONNRESET));
            if ((nRet != 0) && (nRet != WSAECONNRESET))
            {
                dbgPrintf("Socket Error %d\n", nRet);
            }
        #else
            // in linux we appear to get these when we are under heavy load and
            // when ctrl-c is pressed.  Since this is handled by the
            // code that calls this, this is OK.
            //ASSERT(0);      // make sure we notice this in linux or else where...
        #endif
        FAILERR(nErr, BLERR_BLRUDP_SOCKET_RCV_ERR);       // punt.....
    }

    if (ppPkt)
    {
        *ppPkt = pRecvPkt;                          // return it to the caller.
        pRecvPkt->usPktLen = nRcvLen;                           // record the length of this received packet...

        pRecvPkt->ulSrcAddr     = RcvFromSocket.sin_addr.s_addr;        // preserve this with the packet.
        pRecvPkt->nSrcPort      = ntohs(RcvFromSocket.sin_port);
        pRecvPkt->ulDestAddr    = pLocalSap->Sap.ulIpAddr;
        pRecvPkt->nDestPort     = pLocalSap->Sap.nPort;
        if (pSapAddr)
        {
            pSapAddr->ulIpAddr  = pRecvPkt->ulSrcAddr;
            pSapAddr->nPort     = pRecvPkt->nSrcPort;
        }
    }
    else
    {
        if (pSapAddr)
        {
            pSapAddr->ulIpAddr  = RcvFromSocket.sin_addr.s_addr;
            pSapAddr->nPort     = ntohs(RcvFromSocket.sin_port);
        }
    }
    #ifdef _DEBUG
    {
        BLRUDP_PKT_HDR *pPktHdr;
        pPktHdr = BlrudpGetPktPayload(pRecvPkt);
        #if TRACE_PROTOCOL
        dbgPrintf("<<<(usCmd(%04lx), usDlen(%03lx), ulSync(%08lx), ulSreq(%08lx), ulRseq(%08lx)\n",
                  ntohs(pPktHdr->usCmd),
                  ntohs(pPktHdr->usDlen),
                  ntohl(pPktHdr->ulSync),
                  ntohl(pPktHdr->ulSseq),
                  ntohl(pPktHdr->ulRseq));
        #endif
    }
    #endif
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
    struct sockaddr_in Addr;
    void *pPktBuff;
    int nLen;

    #ifdef _DEBUG
    {
        // in debug only.  Take the time here to validate that we setup the packet header
        // correctly...
        BLRUDP_PKT_HDR *pPktHdr;
        pPktHdr = BlrudpGetPktPayload(pPkt);
        ASSERT(ntohs(pPktHdr->usDlen) == pPkt->usPktLen);   // these must agree...

        #if TRACE_PROTOCOL
        dbgPrintf(">>>(usCmd(%04lx), usDlen(%03lx), ulSync(%08lx), ulSreq(%08lx), ulRseq(%08lx)\n",
                  ntohs(pPktHdr->usCmd),
                  ntohs(pPktHdr->usDlen),
                  ntohl(pPktHdr->ulSync),
                  ntohl(pPktHdr->ulSseq),
                  ntohl(pPktHdr->ulRseq));
        #endif
    }
    #endif

    pPkt->ulDestAddr     = pSapAddr->ulIpAddr;        // preserve this with the packet.
    pPkt->nDestPort      = pSapAddr->nPort;
    pPkt->ulSrcAddr      = pLocalSap->Sap.ulIpAddr;
    pPkt->nSrcPort       = pLocalSap->Sap.nPort;

    BlrIoSetSocketAddr(&Addr,                                     // rSocketAddr
                       AF_INET,                                   // nFamily
                       pSapAddr->nPort,                           // hPort
                       pSapAddr->ulIpAddr);                       // ulAddr


    pPktBuff = BlrudpGetPktBuffer(pPkt);

    //
    // Perform long word alignment.  The kernel code on the 405 does
    // not appear to like odd length stuff. so pad it out...
    //
    nLen = (pPkt->usPktLen + (sizeof(uint32_t)-1)) & (~(sizeof(uint32_t)-1));

    nRet = sendto(pLocalSap->hSocket,              // s,
                  (char *)pPktBuff,                // const char *buf,
                  nLen,                            // int len,
                  0,                               // int flags,
                  (struct sockaddr *)&Addr,        // const struct sockaddr *to,
                  sizeof(Addr));                   // int tolen
    if (nRet == 0)
        FAILERR(nErr, BLERR_BLRUDP_SEND_BUSY);

    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

