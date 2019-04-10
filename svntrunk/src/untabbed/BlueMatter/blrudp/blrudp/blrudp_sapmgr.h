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
 #ifndef _BLRUDP_SAP_MGR_H_
#define _BLRUDP_SAP_MGR_H_
////////////////////////////////////////////////////////////
//
// This file contains the functions used to manage the SAP and NIC
// items in the bluelight RUDP system.
//

#include "qlink.h"
#include "blmap.h"

#include "blrudppkt.h"
//#include "blrudp_io.h"

#if defined(WIN32) || defined(CYGWIN)
    #include <winsock.h>
    typedef int socklen_t;
#else
  #ifndef SOCKET
    #define SOCKET int
    #endif
#endif

struct T_BLWORK_MGR;


//
// Sap definition structure.
//
typedef struct {
    unsigned long ulIpAddr;     // IP address in network order 
    uint16_t nPort;             // port number in native order.
} BLRUDP_SAP_ADDR;


///////////////////////////////////////////////////////////
// NIC -- network interface controller information.
//
typedef struct {
    _QLink  link;                      //
    uint32_t ulRefCount;               // reference counter... delete when done...
    uint32_t ulIpAddr;                 // ip Address associed with the NIC.
  
    unsigned uMtuSize;                 // MTU size associaed with the NIC.
  unsigned uPayloadSize;			   // size of the payload associated with this packet..
    int32_t lTotalNicPkts;             // total credit of NIC packets.
    int32_t lPktsInPool;               // number of packets in the pool.
    _QLink  listRecvPktPool;           // receive packet pool info.

    #ifdef BLHPK                       // kernel implementation only
                                       // the interrupt routine places data into this queue
                                       // of packets...
    _QLink  listRecvPkts;              // list of received packets...
    #endif
} BLRUDP_NIC_ITEM;

//////////////////////////////////////////////////////////////
// SAP control structure.  There is one of these created
// for each local socket we create.  These are 
// reference counted and destroyed when no more
// connection contexts are using them.
//
//
typedef struct {
    _QLink  link;                          //
    unsigned long       ulRefCount;        // reference counter
    BLRUDP_SAP_ADDR     Sap;               // sap information.
    SOCKET              hSocket;           // socket handle associated with this
    unsigned int        uMtuSize;          // mtu size for socket device.
  unsigned int        uPayloadSize;	   // maximum payload size...
    struct T_BLWORK_MGR *pXmitWorkMgr;     // Transmit Work queue manager.
    BLRUDP_NIC_ITEM     *pLocalNic;        // local NIC associated with this SAP...
    #ifdef BLHPK
    _QLink  listRecvPkts;                  // list of received packets... for this sap only.
    #endif
} BLRUDP_LOCAL_SAP_ITEM;    


struct T_BLRUDP_MGR;

//////////////////////////////////////////////////////////////
BLRUDP_NIC_ITEM *BlrudpNicItemNew(struct T_BLRUDP_MGR*pBlrudp,
                                 uint32_t ulIpAddr);
//
//
// Create a new Nic Item.
//  
// inputs:
//    pBlrudp-- pointer to Blrudp Manger (current home for the
//              list of NICS.
//    ulIpAddr -- IP address of nic to create.  
// 
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void BlrudpNicItemRelease(BLRUDP_NIC_ITEM *pNicItem);
//
// Release a NIC item created by BlrudpNicItemNew...
// destroy when the reference count hits zero...
//
// inputs:
//     pNicItem -- nic item to destroy.
// outputs:
//     none.
//
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void BlrudpNicItemDelete(BLRUDP_NIC_ITEM *pNicItem);
//
// Destroy a NIC item created by BlrudpNicItemNew...
//
// inputs:
//     pNicItem -- nic item to destroy.
// outputs:
//     none.
//
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
BLRUDP_NIC_ITEM *BlrudpFindNicItem(struct T_BLRUDP_MGR*pBlrudp,
                                   unsigned long  ulIpAddr);
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
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
int BlrudpAddNicRecvPkts(BLRUDP_NIC_ITEM *pLocalNic,
                         uint32_t ulNumPkts);
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
//////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
void BlrudpRemoveNicRecvPkts(BLRUDP_NIC_ITEM *pLocalNic,
                             uint32_t ulNumPkts);
//
// Remove the specified number of receive packets to the NIC...
//
// inputs:
//    pLocalNic -- local NIC to allocate new packets into.
//    ulNumPkts -- number of packets to add.
// outputs:
//    none.
// 
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
void BlrudpPutNicFreeRecvPkt(BLRUDP_NIC_ITEM *pLocalNic,
                         BLRUDP_IO_PKT *pPkt);
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
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetNicFreeRecvPkt(BLRUDP_NIC_ITEM *pLocalNic);
//
// Get a packet from the NIC free receive packet list.
// 
// inputs:
//    pLocalNic -- local NIC to allocate get packets from...
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
BLRUDP_LOCAL_SAP_ITEM *BlrudpLocalSapNew(struct T_BLRUDP_MGR*pBlrudp,
                                         unsigned long  ulIpAddr,
                                         unsigned short nPort);
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
//////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void BlrudpLocalSapDelete(struct T_BLRUDP_MGR*pBlrudp,
                          BLRUDP_LOCAL_SAP_ITEM *pLocalSap);
//
// Delete the sap control structure.  
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pLocalSap -- sap control structure.
// outputs:
//    none.
//
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void BlrudpLocalSapRelease(struct T_BLRUDP_MGR*pBlrudp,
                          BLRUDP_LOCAL_SAP_ITEM *pLocalSap);
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
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
int BlrudpLocalSapSetMtuSize(BLRUDP_LOCAL_SAP_ITEM *pLocalSap);
//
// Set the MTU size associated with this SAP. 
//
// inputs:
//    pLocalSap -- pointer to the local sap to set the mtusize for.
// outputs:
//    returns -- BLERR_SUCCESS if successful..
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void BlrudpLocalSapBuildReadSelects(struct T_BLRUDP_MGR* pBlrudp);
//
// Build the Read select and poll strcutures for ALL of the local
// saps in the local sap list..
//
// inputs:
//    pBlrudp -- pointer to blrudp structure to build list for.
// outputs:
//    none.
//
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void BlrudpLocalSapBuildWriteSelects(struct T_BLRUDP_MGR* pBlrudp,
                                     SOCKET *phHighestSocketFd);
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
///////////////////////////////////////////////////////////////////

#ifdef BLHPK
////////////////////////////////////////////////////////////////
void BlrudpPutNicRecvdPkt(BLRUDP_NIC_ITEM *pLocalNic,
                         BLRUDP_IO_PKT *pPkt);
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


////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetNicRecvdPkt(BLRUDP_NIC_ITEM *pLocalNic);
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

//////////////////////////////////////////////////////////////////////////
void BlrudpNicRun(_QLink *listLocalNICs,
                  BLMAP_HANDLE mapLocalSaps);
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
//    listLocalNICs -- the complete list of the local NICs to service.
//    mapLocalSaps -- map of the local saps to look up the addresses in.
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
int BlrudpNicRecvdDataReady(BLRUDP_LOCAL_SAP_ITEM  *pLocalSap);
//
// Look at the SAP receive buffer and return TRUE if there is data in it.
// 
// inputs:
//    pLocalSap -- local sap associated with the receive data.
// outputs:
//    returns -- TRUE if there is data available, FALSE if not.
//
///////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
void BlrudpLocalSapPutRecvdPkt(BLRUDP_LOCAL_SAP_ITEM *pLocalSap,
                         BLRUDP_IO_PKT *pPkt);
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


////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpLocalGetSapRecvdPkt(BLRUDP_LOCAL_SAP_ITEM  *pLocalSap);
//
// Get a packet from the localSap received packet list.
// 
// inputs:
//    pLocalSap -- local sap associated with the receive data.
//    pPkt -- packet to return to the list.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
int BlrudpLocalSapRecvdDataReady(BLRUDP_LOCAL_SAP_ITEM  *pLocalSap);
//
// Look at the SAP receive buffer and return TRUE if there is data in it.
// 
// inputs:
//    pLocalSap -- local sap associated with the receive data.
// outputs:
//    returns -- TRUE if there is data available, FALSE if not.
//
///////////////////////////////////////////////////////////////////
#endif



#endif

