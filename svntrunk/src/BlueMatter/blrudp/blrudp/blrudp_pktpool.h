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
 #ifndef _BLRUDP_PKTPOOL_H_
#define _BLRUDP_PKTPOOL_H_

///////////////////////////////////////////////////////////////////
// Bluelight rudp Buffer queue management....
//
///////////////////////////////////////////////////////////////////


#include "blrudppkt.h"
#include "blrudp_sapmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////
// Reference counted packet pool created on demand and
// managed by the blue light manager
//


enum {
    BLRUDP_MAX_CTRL_POOL = 4,       // number of send buffers in the send packet 
    BLRUDP_SIZE_CTRL_PKT = 0x100    // pool.. and their max size...
};
typedef struct T_BLRUDP_IO_PKT_POOL {
    uint32_t ulContextId;           // context id to save for debug 
                                    // purposes
    uint32_t ulRefCount;            // reference counter
    uint32_t ulSizePkt;             // payload size of the packet..
    _QLink listSendBuffers;         // list of send buffers 
    //_QLink listRecvBuffers;       // list of receive buffers.
    _QLink listCtrlBuffers;         // list of control send buffers.
                                    // smaller and independant of the
                                    // regular send buffers.

    int32_t lTotalRcvPkts;          // Total packets credited to the 
    int32_t lRcvPkts;               // receive packets associated with this pool
                                    // remaining in the NIC packet queue.

    uint32_t ulSendPkts;            // count of the send packets available.
    struct T_BLRUDP_IO_PKT_POOL **ppSysPool;    // pointer to where the common
                                                // system pool is stored.
    BLRUDP_NIC_ITEM *pLocalNic;     // NIC item associated with this packet pool
                                    // receive packets and receive packet size
                                    // is coordinated with this...
} BLRUDP_IO_PKT_POOL;


//
// structure containing a vector or sent or received packets
// This is utilized as a ring buffer to keep track of transmitted packets
// until they are acknowledged and received packets until they are 
// the receive window advances...  Once in the list they are
// owned by the list until removed and put into the free packet pool.
//
typedef struct T_BLRUDP_PKT_VECTOR {
    uint32_t ulNumEntries;          // total number of entries in the buffer.    
    uint32_t ulBaseSeq;             // base sequence number
    uint32_t ulBaseIndex;           // base index.... (index where the base sequence number resides...)
    uint32_t ulEndSeq;              // end sequence number...      
                                    // for a sender, this is the highest send sequence
                                    // number in the vector.

                                    // for a receiver this is the highest in sequence
                                    // receive number...
    BLRUDP_IO_PKT **pvectPkts;      // vector of packets.
} BLRUDP_PKT_VECTOR;                // packet state information.


////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT_POOL *BlrudpIoPktPoolNew(BLRUDP_IO_PKT_POOL **ppSysPool,
                                       BLRUDP_NIC_ITEM  *pLocalNic,
                                       unsigned long ulContextId,
                                       unsigned long ulNumXmitPkts,
                                       unsigned long ulNumRcvPkts,
                                       unsigned short usSizePkt);
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
/////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveToSendPool(BLRUDP_IO_PKT_POOL *pPktPool,
                               _QLink *pPktQueue);
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
////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveToCtrlPool(BLRUDP_IO_PKT_POOL *pPktPool,
                               _QLink *pPktQueue);
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

//////////////////////////////////////////////////////////////
void BlrudpIoPktMoveToRecvPool(BLRUDP_IO_PKT_POOL *pPktPool,
                               _QLink *pPktQueue);
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

////////////////////////////////////////////////////////////////////
int BlrudpIoPktMoveToPktPool(_QLink *pPktPool,
                              _QLink *pPktQueue);
//
// Move the packets from the packet queue to the receive packet
// pool.
//
// inputs:
//    pPktPool -- pointer to pkt pool queue to transfer to.
//    pPktQueue --- pointer to packet queue move the contents of.
// outputs:
//    returns -- number of packets returned to the pool.
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
void BlrudpIoPktQueueDelete(_QLink *pPktQueue);
//
// Delete the memory associated with a queue of packets.
//
// inputs:
//    pBlrudp -- pointer to the Blrudp instance
//    pPktQueue --- pointer to packet queue to delete.
// outputs:
//    none.
//


////////////////////////////////////////////////////////////////////
void BlrudpIoPktPoolDelete(BLRUDP_IO_PKT_POOL *pPktPool);
//
// Delete the allocated packet pool structure.  This also
// deletes any memory associated with any packets left in its queues.
//
// inputs:
//    pPktPool -- pointer to pkt control to delete.
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void BlrudpIoPktPoolRelease(BLRUDP_IO_PKT_POOL *pPktPool);
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
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpIoPktGetSendPkt(BLRUDP_IO_PKT_POOL *pPktPool);
//
// Retrieve a send packet from the packet pool
//
// inputs:
//    pPktPool -- packet pool to retrieve packet from.
// outputs:
//    returns -- pointer to from pkt pool, or zero if
//               packets are exausted.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
uint32_t BlrudpIoPktGetSendPktCount(BLRUDP_IO_PKT_POOL *pPktPool);
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


//////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpIoPktGetCtrlPkt(BLRUDP_IO_PKT_POOL *pPktPool);
//
// Retrieve a Ctrl packet from the packet pool
//
// inputs:
//    pPktPool -- packet pool to retrieve packet from.
// outputs:
//    returns -- pointer to from pkt pool, or zero if
//               packets are exausted.
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpIoPktGetRecvPkt(BLRUDP_IO_PKT_POOL *pPktPool);
//
// Retrieve a receive packet from the packet pool
//
// inputs:
//    pPktPool -- packet pool to retrieve packet from.
// outputs:
//    returns -- pointer to from pkt pool, or zero if
//               packets are exausted.
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
int BlrudpIoPktDebitRecvPkt(BLRUDP_IO_PKT_POOL *pPktPool);
//
// reduce the credit of receive packets associated with this
// packet pool by 1.
// 
// inputs:
//    pPktPool -- packetpool to reduce debit.
// outputs:
//    returns BLERR_SUCCESS if successful.
//            BLERR_BLRUDP_NO_PKTS -- if no more packets are available
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void BlrudpIoPktPutRecvPkt(BLRUDP_IO_PKT_POOL *pPktPool, 
                      BLRUDP_IO_PKT *pPkt );
//
// Return a packet to the receive packet pool.
//
// inputs:
//    pPktPool -- packet pool to return packet to.
//    pPkt -- packet to return to the pool.
// outputs:
//    returns -- none
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void BlrudpIoPktPutSendPkt(BLRUDP_IO_PKT_POOL *pPktPool, 
                      BLRUDP_IO_PKT *pPkt );
//
// Return a packet to the send packet pool.
//
// inputs:
//    pPktPool -- packet pool to return packet to.
//    pPkt -- packet to return to the pool.
// outputs:
//    returns -- none
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void BlrudpIoPktPutCtrlPkt(BLRUDP_IO_PKT_POOL *pPktPool, 
                      BLRUDP_IO_PKT *pPkt );
//
// Return a packet to the Ctrl packet pool.
//
// inputs:
//    pPktPool -- packet pool to return packet to.
//    pPkt -- packet to return to the pool.
// outputs:
//    returns -- none
//
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveVectorToSendPool(BLRUDP_IO_PKT_POOL *pPktPool,
                                     struct T_BLRUDP_PKT_VECTOR *pVector);
//
// Move a packet vector to the send pool
//
// inputs:
//    pPktPool -- packet pool to move vector back to.
//    pVector -- pointer to vector to move packets from.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BlrudpIoPktMoveVectorToRecvPool(BLRUDP_IO_PKT_POOL *pPktPool,
                                     struct T_BLRUDP_PKT_VECTOR *pVector);
//
// Move a packet vector to the send pool
//
// inputs:
//    pPktPool -- packet pool to move vector back to.
//    pVector -- pointer to vector to move packets from.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif


#endif
