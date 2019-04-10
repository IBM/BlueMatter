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
 #ifndef _BLRUDP_IO_H_
#define _BLRUDP_IO_H_

///////////////////////////////////////////////////////////////////////////////////
//
// Bluelight Reliable UDP io routines.
//
////////////////////////////////////////////////////////////////////////

// some trace stuff.  // TBD consolidate this in one place later.
//
#define TRACE_PROTOCOL 0

//
// Turn this flag to disable the UDP checksum in linux..  DON'T leave this on..
//
#define DISABLE_CHECKSUM 0

#if defined(WIN32) || defined(CYGWIN)
    #include <winsock.h>
    typedef int socklen_t;
#else
#ifndef BLHPK
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif    

    #define closesocket close
    #define ioctlsocket ioctl

#endif

//
// These are found in winsock, but no in unix. make sure we have these....
#ifndef INVALID_SOCKET
    #define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
    #define SOCKET_ERROR   -1
#endif



#ifdef __cplusplus
extern "C" {
#endif

#include "blrudp_sapmgr.h"
#include "blrudppkt.h"


///////////////////////////////////////////////////////////////////
int BlrIoDgramRecvReady(SOCKET hSocket);
//
//  Polls our socket for receive I/O availability
//
// inputs:
//    hSocket -- socket handle to check.
// outputs:
//   returns -- TRUE if receive data is ready, FALSE if not.
//
///////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
int BlrIoDgramXmitReady(SOCKET hSocket);
//
//  Polls our socket for Transmit I/O availability
//
// inputs:
//     hSocket -- socket to check for data gram ready.
// 
// outputs:
//     TRUE if transmit data is ready, FALSE if not.
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
int BlrIoGetLastSocketError(); 
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
///////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
int BlrIoGetMtuFromAddr(unsigned long ulIpAddr,
                        unsigned int *pMtuSize,
                        unsigned int *pPayloadSize);
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


////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
void BlrIoCloseSocket(SOCKET hSocket);
//
// Close the specified socket...
//
// inputs:
//    hSocket -- socket to close.
// outputs:
//    none.
//
///////////////////////////////////////////////////////////////////////

#ifdef OBSOLETE
///////////////////////////////////////////////////////////////////////////
int BlrIoGetNextRecvPktAddr(SOCKET hSocket,
                            BLRUDP_SAP_ADDR *pSocketSap);
//
// Retrieve the address of the next packet we are about to receive...
//
// inputs:
//    hSocket -- socket to receive packet from.
//    *pSocketSap -- pointer to where to stuff the sap information.
//    
// outputs:
//    return -- BLERR_SUCCESS if successful.
//    *pSocketSap->ulIpAddr -- ip address from packet in network byte order.
//    *pSocketSap->nPort -- port number of the packet..
//
////////////////////////////////////////////////////////////////////////////
#endif

//////////////////////////////////////////////////////////////////////////
int BlrIoRecvPkt(BLRUDP_LOCAL_SAP_ITEM *pLocalSap,
                 BLRUDP_IO_PKT **pPkt,
                 BLRUDP_SAP_ADDR *pSapAddr);
//
// Receive a packt from a local Sap.  The caller is responsible
// for returning the packet to the free pool of packets...
//
// inputs:
//    pLocalSap -- local sap to receive packet from
//    pPkt -- place to stuff the packet that is received.
//            if (pPkt == NULL) then just receive the packet and
//            throw away the data...
//    pSapAddr -- place to stuff the sap address...
// outputs:
//    returns -- BLERR_SUCCESS if successful.
//               error codes TBD...
//    *pPkt -- packet received from the local sap...
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
int BlrIoSendPkt(BLRUDP_LOCAL_SAP_ITEM *pLocalSap,
                 const BLRUDP_SAP_ADDR *pSapAddr,
                 BLRUDP_IO_PKT *pPkt);
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

#ifdef __cplusplus
};
#endif

#endif
