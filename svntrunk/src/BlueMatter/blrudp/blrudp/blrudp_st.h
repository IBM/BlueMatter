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
 #ifndef _BLRUDP_ST_
#define _BLRUDP_ST_

////////////////////////////////////////////////////////////////////////////////////
// This file contains the internal structures shared by the blue light
// reliable udp modules.
//

#if defined(WIN32) || defined(CYGWIN)
    #include <winsock.h>
    typedef int socklen_t;
#else
//    #include <sys/socket.h>
//    #include <netinet/in.h>
#endif

#include "blrudp.h"
#include "blmap.h"
#include "blrudppkt.h"
#include "bltimemgr.h"
#include "blrudpwork.h"
#include "blrudp_io.h"
#include "blrudp_sapmgr.h"
#include "blrudp_pktpool.h"


//
// NOTE: we may want to do a big endian version of this.
// but for now these are internal signatures.
//
#define FOUR_CHAR_CODE(x1,x2,x3,x4) (((unsigned long) ((x1) & 0x000000FF)) << 24) \
                                   | (((unsigned long) ((x2) & 0x0000FF00)) << 8) \
                                                                   | (((unsigned long) ((x3) & 0x00FF0000)) >> 8) \
                                                                   | (((unsigned long) ((x4) & 0xFF000000)) >> 24)



// TBD.. remove the short debug timeout...
//
//#define BLRUDP_DEFAULT_ZOMBIE_TIMEOUT 2*60*1000      // 2 minute zombie timeout
#define BLRUDP_DEFAULT_ZOMBIE_TIMEOUT 20*1000          // shortened for debug.

#define BLRUDP_DEFAULT_WTIMER_TIMEOUT 200              // initial wtimer timeout
#define BLRUDP_MAX_WTIMER_TIMEOUT 4*1000               // 10 second max wtimer timeout

#define BLRUDP_MIN_ZOMBIE_TIMEOUT 10*1000               // minimum 10 secnod timeout.

#define BLRUDP_NUM_LISTEN_PKTS  2                      // number of listening packets per
                                                       // listening socket....

#define BLRUDP_NUM_CONNECT_PKTS 4                      // number of connect packets
                                                       // when we start a connection, prior
                                                       // to negoiation.

#define BLRUDP_FIRST_RCV_TIMEOUT 30*1000                // 30 seconds FIRST receive state timeout.
                                                        // may want this to be an option...

#define BLRUDP_DEFAULT_CTIMEOUT 20*1000                 // 20 second CTimeout value.
//
// BLHPK has a lot of printf's going on for now.
// in debug mode... turn up the timeout to be tolerant of it...
//
#if (defined(_DEBUG) /*&& defined(BLHPK) */)
        #define BLRUDP_MIN_EST_WTIMER     5*10                    // minimum Established state wtimer ... 10 milliseconds...
#else
        #define BLRUDP_MIN_EST_WTIMER     10                    // minimum Established state wtimer ... 10 milliseconds...
#endif



#define BLRUDP_SYNC_HANDSHAKE_TIMER 10*1000             // synchronization handshake timer.. 10 seconds.
                                                        // if we don't get a response in this time, the other
                                                        // end is declaried dead....
                                                        //
                                                        // May want to eventually calculate this as a multiple of
                                                        // the avg round trip delay....


#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
// Blue light Rudp control structures.
//
// For now, they are private to this module, we may in the future
// put them in their own header file.  NOTE: These should NOT be put into
// the blrudp.h file.....
//


//////////////////////////////////////////////////////////////////////////
// Blue light rudp manager structure.

typedef enum {
    BLRUDP_CTYPE_UNDEFINED = 0,          // undefined context.
    BLRUDP_CTYPE_PASSIVE = FOUR_CHAR_CODE('p', 'a','s','s'),    // listen, passive context.
    BLRUDP_CTYPE_ACTIVE =  FOUR_CHAR_CODE('a','c','t','v'),   // connect, active context.
    BLRUDP_CTYPE_COMMA_DUMMY
} BLRUDP_HCONTEXT_TYPE;

//
// A connection context contains two saps.  The local sap and
// the remote sap.
//
typedef struct {
    BLRUDP_SAP_ADDR LocalSap;
    BLRUDP_SAP_ADDR RemoteSap;
} BLRUDP_CONNECTION_SAPS;



typedef struct T_BLRUDP_CONTEXT {
    _QLink  link;                          // required header for linked list (must be first)
    BLRUDP_HCONTEXT_TYPE nContextType;     // signature.....
} BLRUDP_CONTEXT;

///////////////////////////////////////////////////////////
// This is the options structure used for global options as well
// as connection specific options.
//
typedef struct {
    uint32_t  ulWindowSize;        // default window size.
    uint32_t  ulMaxRetryCount;        // default retry count.
    uint32_t  ulXmitRate;          // default transmit rate (timedel in ms).
    uint32_t  ulXmitBurstSize;     // default transmit burst size in packets..
    uint32_t  ulSrcIpAddr;         // default source IP address.
    uint16_t  nsSrcPort;           // default source port
    uint32_t  ulRndTripDelay;      // starting round trip delay time.
    uint32_t  ulMTL;               // default max time to live in MS.
    uint32_t  ulMtuSize;           // default MTU size
    uint8_t   bInOrderDelivery;    // inorder deliver.
    uint8_t   bDedicatedBuffers;   // dedicated buffers per connection.
    uint8_t   bUseRawSockets;      // use raw sockets if available.
    uint8_t   bChecksum;           // checksum all packets.
    uint8_t   bAppFlowCtl;         // Application level flow control.
    uint8_t   bFastNak;            // fast nak mode.
    uint8_t   pad[2];              // pad it out to long word boundary....
} BLRUDP_CONTEXT_OPTIONS;

////////////////////////////////////////////////////////////
// These correspond to the TCTRL message
//
typedef struct {
    uint32_t  ulWindowSize;        // default window size.
    uint32_t  ulMtuSize;           // remote MTU size...
    uint8_t   bChecksum;           // checksum
    uint8_t   pad[3];              // pad it out to long word boundary...
} BLRUDP_REMOTE_OPTIONS;

/////////////////////////////////////////////////////////////
// diagnostic information.  Stored for the local and remote
// hosts...
//
typedef struct {
    int         bDiagValid;         // diagnostic information valid.
    uint64_t    llNumRetrans;       // total number of retransmissions.
    uint64_t    llDataXmit;         // amount of data transmitted
    uint64_t    llDataRecv;         // amount of data received.
} BLRUDP_DIAG_INFO;

//////////////////////////////////////////////////////////////
// main manager control structure....
//
typedef struct T_BLRUDP_MGR {
        _QLink link;                                            // link header for a master list of these things.


    _QLink  listPassCtxs;               // list of passive contexts
    _QLink  listActCtxs;                // list of active contexts associated with this.
    _QLink  listLocalSaps;              // list of local saps for retrieving
                                        // incoming data.
    uint32_t ulNumLocalSaps;            // total number of local saps...
    _QLink  listLocalNICs;              // list of Local NICS.  BLRUDP_LIST_ITEMS.

    #if !defined(BLHPK)
        fd_set   fdsetRead;                  // current file descriptor sets for read and write.
        fd_set   fdsetWrite;                 // select calls...
    #endif
    /////////////////////////////////////////////////////////////
    // associative maps....
    BLMAP_HANDLE mapPassCtxs;
    BLMAP_HANDLE mapActCtxs;
    BLMAP_HANDLE mapLocalSaps;          // map of the local sap

    BLTIME_MGR *pTimeMgr;               // timeing manager used to manage timeouts.
                                        //

    BLWORK_MGR *pRecvWorkMgr;           // Receive Work queue manager.
    BLWORK_MGR *pXmitDoneWorkMgr;           // Receive Work queue manager.

    BLRUDP_IO_PKT_POOL *pPktPool;

    // Default value section.
    BLRUDP_CONTEXT_OPTIONS DefOptions;

    char szHostName[50];                // host name of this host...

        unsigned long ulRunNum;                         // run number, increments every time we
                                                                                // do a run...

} BLRUDP_MGR;


typedef struct {
    BLRUDP_CONTEXT  Hdr;                               // common context header.
    BLRUDP_MGR      *pBlrudp;
    BLRUDP_LOCAL_SAP_ITEM *pLocalSap;                        // local sap associated with this
    PFN_BLRUDP_LISTEN_CONNECT_REQ pfnListenConnectReq; // pointer to connect request callback.
    BLRUDP_CONTEXT_OPTIONS Options;
    void            *pUserData;                     // pointer to user data for callbcack
    BLRUDP_IO_PKT   *pRcvPkt;                       // pointer to receive packet area.

    BLRUDP_IO_PKT_POOL *pPktPool;

} BLRUDP_PASSIVE_CONTEXT;



//
// Timer structures for controlling Active context timers.
//
typedef struct {
    BLTIME_TIMER WTimer;            // The WTIMER is the timer that guards against the
                                    // loss of a packet with the SREQ bit set.  Whenever a
                                    // packet with the SREQ bit set is sent, the
                                    // transmitter increments the saved_sync value by one
                                    // and places it into the sync field of the packet.
                                    // The context sending the packet also starts the WTIMER
                                    // loading it with the smoothed round-trip time estimate.  The
                                    // WTIMER is the amount of time the transmitter will wait for
                                    // the arrival of the control packet requested with the SEQ bit.
                                    // If a control packet arrives at the transmitter before the
                                    // WTIMER expires, the value in the echo field is compared with
                                    // the saved_sync value.  If they are equal, the WTIMER is
                                    // stopped.  If the WTIMER expires, the context starts a
                                    // synchronizing handshake.

    BLTIME_TIMER CTimer;            // An association is also terminated if no packets are received
                                    // at one of the contexts for some period of time. The
                                    // connection timer CTIMER is used to allow BLRUDP to recover
                                    // from system and network failure by measuring inactivity. The
                                    // CTIMER is enabled when the association first becomes active.
                                    // The context counts the number of packets that have arrived
                                    // during the CTIMER interval. If the packet count is greater
                                    // than zero when the CTIMER expires, the CTIMER is restarted.
                                    // If the packet count is zero, the CTIMER is restarted, and
                                    // the context initiates a synchronizing handshake to verify
                                    // that the other  endpoint is still alive. If the
                                    // synchronizing handshake fails, the context is aborted.

    BLTIME_TIMER CTimeout;          // The CTIMEOUT timer limits the amount of time a synchronizing
                                    // handshake can continue before the context aborts the
                                    // association.
                                    // The CTIMEOUT timer is also used when a context goes into the
                                    // zombie state after sending a packet with the END bit set.
                                    // The CTIMEOUT timer can be disabled by setting the CTIMEOUT
                                    // interval to zero, but if this is done, the initial value of
                                    // the retry_count for the synchronizing handshake must not also be zero.

    BLTIME_TIMER RTimer;            // The RTIMER is the rate control timer used to govern the
                                    // frequency of sending bursts of data. Its use is described in
                                    // Section "Rate Control."

} BLRUDP_ACTIVE_TIMERS;


typedef enum {
    BLRUDP_CTX_STATE_CLOSED     = 0,
    BLRUDP_CTX_STATE_FIRST_SENT = 1,
    BLRUDP_CTX_STATE_FIRST_RECV = 2,
    BLRUDP_CTX_STATE_ESTABLISHED = 3,
    BLRUDP_CTX_STATE_CLOSING    = 4,
    BLRUDP_CTX_STATE_ZOMBIE     = 5,
    BLRUDP_CTX_STATE_MAXSTATE   = 6
} BLRUDP_ACT_STATE;

//
// Reasons for ending up in the zombie state.
//
typedef enum {
    BLRUDP_ZOMBIE_UNKNOWN = 0,              // don't know how we got hhere.
    BLRUDP_ZOMBIE_DISCONNECT = 1,           // manual disconnect
    BLRUDP_ZOMBIE_TIMEOUT = 2,              // retry timeout.
    BLRUDP_ZOMBIE_REJECT = 3,               // connection rejected...
    BLRUDP_ZOMBIE_FIRST_RCV_TIMEOUT = 4     // first receive timeout
} BLRUDP_ZOMBIE_SRC;


enum {
    BLRUDP_WORK_SENDCTRL     = (1<<0),       // send ctrl message.
    BLRUDP_WORK_SENDTCTRL    = (1<<1),       // send TCTRL message.
    BLRUDP_WORK_SENDERR      = (1<<2),       // send error message....
    BLRUDP_WORK_SENDRESET    = (1<<3),       // send a reset message.
    BLRUDP_WORK_SENDFIRST    = (1<<4),       // send a first packet..
    BLRUDP_WORK_SENDDIAGREQ  = (1<<5),       // send diagnostic Req
    BLRUDP_WORK_SENDDIAGRESP = (1<<6)        // send diagnostic response

};

typedef struct {
    uint32_t ulSseq;                            // highest send Sequence number
    uint32_t ulRseq;                            // hightet receive sequence number.
    uint32_t ulAllocSeq;            //  Current allocation value being sent...
                                    //  Describes the current receive window for the
                                    //  local end. (number of packets we are willing to
                                    //  accept...
    uint32_t ulLastAllocSeqSent;    // last alloc successfully sent.
                                    // Xmit only information... not meaningful for transmit...
    uint32_t ulNextInSeq;           // Next in sequence number to be received...
                                    // receive only information.
    uint16_t usFlags;               // flags of the last packet received..
                                    // ... receive side only for now...
} BLRUDP_SEQ_STATE;


//
// receive packet buffer to keep track of packets received
// for purpose of naking packets....
//
typedef struct {
    uint32_t ulNumEntries;           // number of 32 bit entries in the status vector...
    uint32_t ulBaseSeq;              // base sequence number
    uint32_t ulBaseIndex;            // index into the sequence containing the bit
                                     // for the base sequence number....
    uint32_t ulEndSeq;               // end sequence number...
    uint32_t *pvectRecv;             // bits indicating packets received.
} BLRUDP_RECV_PKT_STATUS;

//////////////////////////////////////////////////////////////////
// Sync request state variables.
//
// This information about the round trip delay computation from
// the XTP specification:
// Implementors may use whatever smoothing functions they wish. The following is from
// Van Jacobson. For each observation of the round-trip time rtt,
//
//  SRTT is the smoothed round trip time, and RTTV is the round-trip time variance. The
//  value loaded into WTIMER is:
//         rtt = (now - saved_time);
//         SRTT = SRTT + ((rtt - SRTT) / 8);
//         RTTV = RTTV + ((abs(rtt - SRTT) - RTTV) / 4);
//
//  WTIMER SRTT + 2*RTTV,
//

typedef struct {
    uint32_t    ulXmitSync;                 // transmit sync when the sreq we sent when starting the wtimer.
    BLTIME_MS   llXmitSyncTime;             // time the above xmit sync was sent.
                                            // NOTE: if this is zero, then the timer is not active...
    BLTIME_MS   llSRTT;                     // smoothed round trip time (initialize to initial estimate of delay).
    BLTIME_MS   llRTTV;                     // round trip time variance. (init to zero).
    int         bSyncHandshake;             // true if sync handshake is in progress..
    //
    // These two fields are needed because in watching the performance of this protcol in a many to one test we found
    // that we could timeout on a sync, start a synchronous handshake and then, receive the original
    // response we were expecting when we timed out.  These two fields remember the sync/echo value
    // that started the handshake and if that arrives, it will use it to calculate the
    // response time..
    uint32_t    ulHandshakeSync;            // sync we missed when we started the handshake timer.
    BLTIME_MS   llHandshakeSyncTime;        // time the sync was sent that was missed that started the handshake timer.

} BLRUDP_SREQ_STATE;

typedef struct {
    BLRUDP_CONTEXT         Hdr;
    int                    bAccRejCalled;       // connection accept or reject called....
    uint32_t               ulContextId;         // internal context identifer for debugging...

    BLRUDP_MGR             *pBlrudp;

    BLRUDP_CONNECTION_SAPS SapPair;
    BLRUDP_LOCAL_SAP_ITEM  *pLocalSap;          // local sap associated with this

    BLRUDP_ACT_STATE       nState;              // current state of active context.
    struct T_BLRUDP_EVENT_VTABLE *pStateTbl;    // current state table.

    BLRUDP_ACTIVE_TIMERS   Timers;              // structure containing timers to use.

    //
    // Section of work information.
    //
    BLWORK_ITEM             RecvWorkItem;       // Data received, call backs pending.

    BLWORK_ITEM             XmitDoneWorkItem;   // transmission buffer released callbacks pending.

    BLWORK_ITEM             XmitWorkItem;       // some transmit work to do...
                                                // false if not...

    unsigned long           ulWorkFlags;        // flags indicating work to do..
                                                // cleared on EVERY state transition.

    int32_t                 lXmtErrCode;        // error code to send when BLRUDP_WORK_SENDERR is set...
    uint32_t                ulXmtResetSeq;      // sequence number of reset to send...
                                                // used in connection establishment.

    unsigned int           nRetryCount;         // current retry count.

    BLTIME_MS              llWTimerTimeout;     // wtimer timeout value.
    uint32_t               ulWTimerFactor;      // timer factor to multiply wtimer by...

    BLRUDP_IO_PKT_POOL     *pPktPool;           // packet pool for this context..

    _QLink                 listRexmitPkts;      // list of data packet to retransmit
    _QLink                 listRecvPkts;        // list of packets received and ready to pick up.
                                                // For in order reception this list contains only
                                                // in order packets.  The remaining packets are in the
                                                // ReceivePktVector...
                                                //
                                                // For out-of-order reception
                                                // this list contains potentially out of order packets..
                                                //
    uint32_t               ulRecvPktCount;      // count of the pkts in the listRecvPkts queue.


    BLRUDP_PKT_VECTOR      XmitPktVector;       // vector of xmit packets.
    _QLink                 listClientSendPkts;  // list of the packet buffer the client has
                                                // requested via BlrudpGetSendBuffer.
    uint32_t               ulClientSendPktCount;// count of packets in the Client send packets queue

    BLRUDP_PKT_VECTOR      RecvPktVector;       // vector or receive packets..
    _QLink                 listClientRecvPkts;  // list of the receive packets the client has...
    uint32_t               ulClientRecvPktCount;// count of the packets in the listClientRecvPkts list.

    BLRUDP_RECV_PKT_STATUS RecvPktStatus;

    BLRUDP_IO_PKT          *pCurrRcvPkt;        // current packet being received.
                                                // if the context takes responsibility
                                                // for this packet, then clear this field..
    BLRUDP_CONTEXT_OPTIONS Options;
    BLRUDP_REMOTE_OPTIONS  RemoteOptions;

    PFN_BLRUDP_CONTEXT_CHANGED pfnContextChanged;
    PFN_BLRUDP_CONTEXT_RECV pfnContextRecv;
    PFN_BLRUDP_SEND_BUFFER_DONE pfnSendBufferDone;
    void                    *pUserData;

    uint32_t                ulNumRecvWinPkts;           // total number of packets in the receive
                                                        // windows.
    uint32_t                ulAllocRefreshInterval;     // number of packets between
                                                        // the last allocation we sent and
                                                        // the time we will send a new allocation
                                                        // via a control package.  This is usually
                                                        // set to 1/2 or 1/4 of the receive window.
                                                        // but for small windows it will be set to the
                                                        // size of the entire window..



    //
    // State variables
    //
    BLRUDP_SEQ_STATE   XmitSeq;     // tranmitter sequence information.
                                    // XmitSeq.ulSseq -- currently transmitting Sseq number, which
                                    //  is the sequence number of a data packet if the transmitted
                                    //  data is a data packet and the sequence number of the NEXT data
                                    //  packet if the packet is a control packet.
                                    // XmitSeq.ulRseq -- currently transmitting Rseq number.  The
                                    //  highest sequential data received and delivered to the target
                                    //  application.
                                    //
    BLRUDP_SEQ_STATE   RecvSeq;     // receive sequence information
                                    //  RecvSeq.ulSseq -- the highest received data packet....
                                    //  RecvSeq.ulRseq -- the highest received Rseq data...

    BLRUDP_SREQ_STATE  XmitSreqSt;  // Xmit Srequest state...

    BLRUDP_DIAG_INFO   LocalDiag;   // local diagnostic information (to send to remote host).
    BLRUDP_DIAG_INFO   RemoteDiag;  // diagnostic information from the remote host...

    uint32_t ulXmitSync;            //  current Xmit sync value...

    uint32_t ulKkseq;                           //  Last sequence number retransmitted

    uint32_t ulKseqSync;                        //  Last sync value retransmitted
    uint32_t ulSavedSync;           //  Last sync value sent
    uint32_t ulSavedSyncSeq;        //  Sequence number of last sync value sent

    uint32_t ulRecvCtrlSync;        //  highest sync value received in a control packet.
    uint32_t ulRecvSync;            //  highest receive sync value...
    uint32_t ulRcvdEcho;            //  Highest echo value received

    uint32_t ulCTimerSync;          //  sync value when we set the CTimer



    uint16_t usXmitFlags;           //  current xmit flags.

} BLRUDP_ACTIVE_CONTEXT;

typedef struct T_BLRUDP_EVENT_VTABLE {
    //
    // NOTE: This vtable only maps those events that are valid across 2 or more states.
    //       otherwise it gets too complicated to maintain these tables.
    //
    #if 0           // keep these here in case I change my mind.
        int  (*pfnConnect)(BLRUDP_ACTIVE_CONTEXT *pContext);
        int  (*pfnAccept)(BLRUDP_ACTIVE_CONTEXT *pContext);
        int  (*pfnReject)(BLRUDP_ACTIVE_CONTEXT *pContext);
        int  (*pfnRecvFrom)(BLRUDP_ACTIVE_CONTEXT *pContext);
        int  (*pfnSendTo)(BLRUDP_ACTIVE_CONTEXT *pContext);
        int  (*pfnReqDiag)(BLRUDP_ACTIVE_CONTEXT *pContext);
    #endif
    //
    void (*pfnXmitWork)(BLRUDP_ACTIVE_CONTEXT *pContext);
    void (*pfnRecvPacket)(BLRUDP_ACTIVE_CONTEXT *pContext, BLRUDP_IO_PKT *pPkt);
    void (*pfnWTimer)(BLRUDP_ACTIVE_CONTEXT *pContext);
    void (*pfnCTimer)(BLRUDP_ACTIVE_CONTEXT *pContext);
    void (*pfnCTimeout)(BLRUDP_ACTIVE_CONTEXT *pContext);
    void (*pfnRTimer)(BLRUDP_ACTIVE_CONTEXT *pContext);
} BLRUDP_EVENT_VTABLE;



/////////////////////////////////////////////////////////////
BLRUDP_ACTIVE_CONTEXT *BlrudpActCtxNew(BLRUDP_MGR *pBlrudp);
//
// Create and initialize a new active context structure.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
// outputs:
//    returns -- pointer to new passive context structure
//               or NULL if out of memory.
//
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void BlrudpActCtxDelete(BLRUDP_MGR *pBlrudp,
                               BLRUDP_ACTIVE_CONTEXT *pContext);
//
// delete and shutdown a passive context.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pContext    -- context to delete and shut down.
// outputs:
//    none.
//
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
BLRUDP_PASSIVE_CONTEXT *BlrudpPassCtxNew(BLRUDP_MGR *pBlrudp);
//
// Create and initialize a new passive context structure.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
// outputs:
//    returns -- pointer to new passive context structure
//               or NULL if out of memory.
//
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void BlrudpPassCtxDelete(BLRUDP_MGR *pBlrudp,
                                BLRUDP_PASSIVE_CONTEXT *pContext);
//
// delete and shutdown a passive context.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    pContext    -- context to delete and shut down.
// outputs:
//    none.
//
/////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

#endif
