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
 #ifndef _BLRUDP_COMMON_H_
#define _BLRUDP_COMMON_H_

//////////////////////////////////////////////////////////////////////////////////////
// Bluelight Reliable Udp common helper functions.
//
//
#include "blrudppkt.h"
#include "blrudp_st.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////
int BlrudpSendFirstPkt(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Send off the first packet to the SAP associated with this context.
//
// inputs:
//     pContext -- context to process event
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
int BlrudpSendResetPkt(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Send a reset packet to the other end of this connection.
//
// A reset packet is a packet with the END bit set AND it
// has a r-seq number matching the sSeq number received
// by this connection when it received the packet causing
// this to happen...
/////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
int BlrudpSendCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_IO_PKT *pPkt);
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
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
int BlrudpSendDataPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_IO_PKT *pPkt);
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
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
void  BlrudpSetInitialSeqNumbers(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Set the initial sequence numbers for this active context.
//
// inputs:
//   pContext -- context to set the numbers for.
// outputs:
//   pContext -- sequence numbers set in the context.
//
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void BlrudpSetActiveState(BLRUDP_ACTIVE_CONTEXT *pContext,
                          BLRUDP_ACT_STATE nState,
                          int nBlError);
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
/////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void BlrudpSetZombieState(BLRUDP_ACTIVE_CONTEXT *pContext,
                          int nBlError);
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
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
int BlrudpUserState(BLRUDP_ACT_STATE nState);
//
// Map the internal state to a user state.
//
// inputs:
//    nState -- internal state to map.
// outputs:
//    returns -- user state this corresponds to.
//
////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void BlrudpActCtxClearCallbacks(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Clear the callbacks associated with an active context.
//
// inputs:
//    pContext - context to clear the callback pointers in.
// outputs:
//   none.
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void BlrudpPassiveRecvPkt(BLRUDP_PASSIVE_CONTEXT *pPassContext, 
                          BLRUDP_ACTIVE_CONTEXT *pExistingActContext,
                          BLRUDP_IO_PKT *pRecvPkt,
                          unsigned long ulIpAddr,
                          unsigned short nPort );
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
//////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
void BlrudpActiveRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext, 
                          BLRUDP_IO_PKT *pRecvPkt );
//
// process a receive packet by a Active context.
//
// inputs:
//    pContext -- active context to process this for.
//    pRecvPkt -- received packet..
// outputs:
//    none.
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void BlrudpActiveRecvAcceptPkt(BLRUDP_ACTIVE_CONTEXT *pContext, 
                               BLRUDP_IO_PKT *pRecvPkt );
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
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void BlrudpActiveRecvEcntlPkt(BLRUDP_ACTIVE_CONTEXT *pContext, 
                              BLRUDP_IO_PKT *pRecvPkt,
                              BLRUDP_PKT_HDR *pPktHdr);
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
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void BlrudpActiveRecvDiagResp(BLRUDP_ACTIVE_CONTEXT *pContext, 
                              BLRUDP_IO_PKT *pRecvPkt UNUSED,
                              BLRUDP_PKT_HDR *pPktHdr);
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
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
void BlrudpProcessRemoteOptions(BLRUDP_ACTIVE_CONTEXT *pContext,
                                BLRUDP_IO_TCTRL_PKT *pTCtrlPkt);
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
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
int BlrudpCombineRemoteOptions(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// combine the local and remote options.
//
// inputs:
//    pContext -- context to combine the options in.
// outputs::
//   returns -- BLERR_SUCCESS if successful.
//
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
int BlrudpActCtxInit(BLRUDP_ACTIVE_CONTEXT *pContext,
                            unsigned long ulNumXmtPkts,
                            unsigned long ulNumRcvPkts,
                            int bAllocVectors);
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


////////////////////////////////////////////////////////////////////////////////
void BlrudpSelectMtuSize(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Select the mtu size from either the default local sap or the
// options specified by the user.
//
// inputs:
//     pcontext -- active context to select the MTU size for.
// outputs:
//     none.
//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
void BlrudpActCtxDumpPktPool(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// dump the packet pool associated with this active context.
//
// inputs:
//    pContext -- context to dump pool from.
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void BlrudpActCtxReclaimPkts(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Reclaim the packets from send and to send pkts into
// the packet pool.
//
// inputs:
//    pContext -- context to reclaim packets for.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void BlrudpStAddErrorMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext,
                              int32_t nErrCode);
//
// Set the context state to send an error and schedule the work.
//
// inputs:
//     pContext --context to add the error message to.
//     nErrCode -- error code to send.
// outputs:
//     none.
// 

////////////////////////////////////////////////////////////////////////
void BlrudpStAddTctrlMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Add a Tctrl message to the work to do..
//
//
// inputs:
//     pContext --context to add the work message to.
// outputs:
//     none.
// 
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BlrudpStAddCtrlMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Add a Tctrl message to the work to do..
//
//
// inputs:
//     pContext --context to add the work message to.
// outputs:
//     none.
// 
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BlrudpStAddFirstMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext);
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
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BlrudpStAddResetMsgToWork(BLRUDP_ACTIVE_CONTEXT *pContext,
                               uint32_t ulSseq);
//
// Add a reset message to the work to do..
//
// inputs:
//     pContext -- context to add the work message to.
//     ulSseq -- sequence number to use in message.
// outputs:
//     none.
// 
////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
void BlrudpActCtxClearTimers(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Clear the timers from this context.
//
// inputs:
//    pcontext -- context to clear the timers from.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
int BlrudpIsSseqValid(BLRUDP_ACTIVE_CONTEXT *pContext,
                      BLRUDP_PKT_HDR *pPktHdr);
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

////////////////////////////////////////////////////////////////////////
void BlrudpUpdateReceiverRSeq(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Update the receiver RSEQ number...
//
// inputs:
//    pContext -- pointer to context to update.
// outputs:
//    none.
//
////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void BlrudpSetRecvWindow(BLRUDP_ACTIVE_CONTEXT *pContext, 
                         uint32_t ulRecvWindow);
//
// Set the size of the receive window and any state variables associated with it.
//
// inputs:
//    pContext -- pointer to context this pertains to.
//    ulRecvWindow -- number of packets in the receive window..
//
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
int BlrudpPktVectorsAlloc(BLRUDP_ACTIVE_CONTEXT *pContext,
                          uint32_t nNumXmitPkts,
                          uint32_t nNumRecvPkts);
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
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BlrudpPktVectorsFree(BLRUDP_ACTIVE_CONTEXT *pContext);
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
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void BlrudpPktVectorsPush(BLRUDP_ACTIVE_CONTEXT *pContext,
                          BLRUDP_PKT_VECTOR     *pVector,
                          BLRUDP_IO_PKT         *pPkt);
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
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
int BlrudpPktVectorAvail(BLRUDP_PKT_VECTOR *pVector);
//
// Check to see if there is room in the packet vector..
//
// inputs:
//     pVector -- pointer to the packet vector.
// outputs:
//     returns -- TRUE if there is room, FALSE if there is no room.
// 
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
int BlrudpPktVectorsPktsToSend(BLRUDP_ACTIVE_CONTEXT *pContext,
                               BLRUDP_PKT_VECTOR     *pVector);

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
/////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetVectorsPkt(BLRUDP_PKT_VECTOR     *pVector,
                                   uint32_t              ulSeq);
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
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
uint32_t BlrudpPktVectorIndex(BLRUDP_PKT_VECTOR     *pVector,
                              uint32_t              ulSeq);
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
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
void BlrudpPutRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext, BLRUDP_IO_PKT *pPkt);
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
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpGetRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Retrieve a packet from the list of received packets..
//
// 
// inputs:
//    pContext -- pointer to context.
// outputs:
//    returns -- pointer to packet retrieved, or 0 if no packets..
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpPeekRecvPkt(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Get the top most packet from the list of received packets..
// but leave it on the queue.
//
// 
// inputs:
//    pContext -- pointer to context.
// outputs:
//    returns -- pointer to packet retrieved, or 0 if no packets..
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
uint32_t BlrudpGetRecvPktCount(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// retrieve the count of waiting to be received.
//
// inputs:
//    
// inputs:
//    pContext -- pointer to context.
// outputs:
//    returns -- count of the packets in the receive queue.
//
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void BlrudpSentSreq(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// Anytime a send function successfully sends a message with
// the SREQ bit set we get called here.
//
// This handles saving the sync, recording the time this was
// sent and starting the wtimer.
//
// inputs:
//    pContext -- context which just sent the packet..
// outputs:
//    none.
/////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void BlrudpInitSreqState(BLRUDP_ACTIVE_CONTEXT *pContext);
//
// initialize the SreqState variables for this context..
//
// inputs:
//    none
// outputs:
//    pContext -- pContext->XmitSreqSt initialized.
//
//
///////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
void BlrudpUpdateSreqState(BLRUDP_ACTIVE_CONTEXT *pContext, 
                           uint32_t ulEcho);
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
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void BlrudpStartSyncHandshake(BLRUDP_ACTIVE_CONTEXT *pContext);
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
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void BlrudpStopSyncHandshake(BLRUDP_ACTIVE_CONTEXT *pContext);
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
/////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

#endif

