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
 #ifndef _BLRUDP_H_
#define _BLRUDP_H_
/////////////////////////////////////////////////////////////////////////////////////////
//
//
// This File contains the definition and document of the Blue Light Reliable UDP
// library (BlRudp).
//
// The BlRudp library implements the BlRudp protocol.  This protocol is designed
// to provide high bandwidth utilization of either a 100Mb or 1000Mb link between
// two points.  It is designed to run on top of UDP.
//
// For a complete description of this protocol please see:
//           "Blue Light Reliable UDP Network Protocol_e.doc"
//
//
// Functions defined in this module:
//
//      BlrudpNew
//      BlrudpDelete
//      BlrudpSetOptions
//      BlrudpGetOptions
//      BlrudpSetOptionLong
//      BlrudpGetOptionLong
//      BlrudpGetPollList
//      BlrudpGetSelectSet
//      BlrudpRun
//      BlrudpListen
//      BlrudpAccept
//      BlrudpReject
//      BlrudpUnlisten
//      BlrudpConnect
//      BlrudpDisconnect
//      BlrudpGetContextState
//      BlrudpGetContextAddr
//      BlrudpGetContextAddrStr
//      BlrudpGetStatus
//      BlrudpReqDiagInfo
//      BlrudpGetDiagInfo
//      BlrudpRecvFromContext
//      BlrudpRecvBufferFromContext
//      BlrudpReleaseRecvBuffer
//      BlrudpSendTo
//      BlrudpBufferSendToContext
//      BlrudpGetSendBuffer
//      BlrudpGetMaxPayloadLen
//      BlrudpGetMaxBufferSize
//      BlrudpGetDataSize
//      BlrudpGetDataPointer
//      BlrudpShutdown
//




//



// Parameter Type ID's.
// These ID's are use to override default paramters for an entire instance of
// the library or just an individual connection.
//
//

#include "hostcfg.h"

#if defined(WIN32) || defined(CYGWIN)
#include <winsock.h>
#endif

#if HAVE_STDINT_H
    #include <stdint.h>
#elif USE_GDB_STDINT_H
    #include "gdb_stdint.h"
#endif

#if HAVE_SYS_SOCKET_H
        #include <sys/socket.h>
#endif

#if  HAVE_SYS_POLL_H
        #include <sys/poll.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#if HAVE_SYS_SOCKET_H
        #include <sys/socket.h>
#endif
#if HAVE_SYS_POLL_H
        #include <sys/poll.h>
#endif

#if HAVE_SYS_TIME_H
        #include <sys/time.h>
#endif

typedef void *BLRUDP_HANDLE;            // generic handle used in the BlRudp library to
                                        // refer to opaque objects.

typedef void *BLRUDP_HCONTEXT;           // handle to an opaque context object.

typedef void *BLRUDP_HIOBUFFER;         // handle to an io buffer.

//
// Make sure we have defined these types....
// but don't reqire the entire stdint.h or gdb_stdint.h files.
#if ((!defined(_STDINT_H)) && (!defined(_LINUX_TYPES_H)) && (!defined (_H_INTTYPES)) )
    #ifdef WIN32
        typedef __int64  int64_t;
        typedef unsigned __int64     uint64_t;
    #else
        typedef long long  int64_t;
        typedef unsigned long long   uint64_t;
    #endif
#endif

////////////////////////////////////////////////////////////////
// published diagnostic information...
//
typedef struct {
    int         bDiagValid;         // diagnostic information valid.
    uint64_t    llNumRetrans;       // total number of retransmissions.
    uint64_t    llDataXmit;         // amount of data transmitted
    uint64_t    llDataRecv;         // amount of data received.
} BLRUDP_DIAG;

/////////////////////////////////////////////////////////////////////////////////////////
// Callback function prototypes.
//


////////////////////////////////////////////////////////////////////////////////
typedef void (* PFN_BLRUDP_CONTEXT_RECV)(BLRUDP_HCONTEXT hContext,
                                         void *pUserData);
//
// Notification to the client that there are packets waiting to be received
// in the receive queue for the context hContext.
//
// inputs:
//    hContext -- context that has data to receive.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//    none.
///////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
typedef void (* PFN_BLRUDP_CONTEXT_CHANGED)(BLRUDP_HCONTEXT hContext,
                                            int nOldState,
                                            int nNewState,
                                            int nBlError,
                                            void *pUserData);
//
// Callback indicating the state of the indicated context has changed.
//
// inputs:
//    nOldState -- the previous state of the context.
//    nNewState -- the new state of the context.
//    nBlError  -- bluelight error condition.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//    none.
//
// NOTES:
//    Most all state transitions will report Blerror == BLERR_SUCCESS however,
//    some transitions to the CLOSED state are due to error conditions and
//    these are reported by the nBlError variable.
//
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
typedef void (* PFN_BLRUDP_LISTEN_CONNECT_REQ)(BLRUDP_HCONTEXT hListenContext,
                                               BLRUDP_HCONTEXT hNewContext,
                                               unsigned long nAddr,
                                               unsigned short nPort,
                                               void *pUserData);
//
// Callback indicating the listening port has an incomming connection
// reqest that the client application has to either accept or reject.
//
// The callback either calls BlrudpAccept or BlrudpReject
// to accept or reject the conversation.
//
// If neither accept or reject is called on the context during this call
// then Reject is assumed.
//
// inputs:
//    hContext      -- listening context
//    hNewContext   -- new incomming context.
//    nAddr         -- remote address of the incomming connection.
//    nRemotePort   -- remote port of the incomming connection.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//              A BLRUDP_CONTEXT_CHANGED callback will be called
//              when a new Active context is created and placed into
//              the ...CONNECTING state.
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
typedef void (* PFN_BLRUDP_SEND_BUFFER_DONE)(BLRUDP_HCONTEXT hContext,
                                             unsigned long ulSendBufferID,
                                             void *pUserData);
//
// Callback indicating that a buffer sent by BlrudpSend...
// has been sent and acknowledged.  The buffer has actually been returned
// to the free pool of send buffers.
//
// inputs:
//    hContext -- context this buffer corresponds to.
//    ulSendBufferID -- identifier passed into the BlrudpSend.. function.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//    none.
//
/////////////////////////////////////////////////////////////////////////


enum {


    BLRUDP_WINDOW_SIZE = 0,         //  The maximum amount of data that
                                    // a receiver can receive without sending an acknowledgement
                                    // size is in bytes, rounded up to the nearest MTU size.


    BLRUDP_RETRY_COUNT = 1,         // Maximum number of retransmissions allowed before a
                                    // connection is declared dead.

    BLRUDP_TRANSMIT_RATE = 2,       // The maximum rate between bursts of packets that data can be
                                    //    transmitted before having to wait.  This is used for rate control.
                                    //    Defaults to unlimited.

    BLRUDP_BURST_SIZE = 3,          // Maximum number of packets that can be sent in a single burst
                                    //  if the Valid only if the transmit rate is set.

    BLRUDP_DEFAULT_SRC_PORT = 4,        // Default Source port to use for all connections.

    BLRUDP_SOURCE_IP_ADDR = 5,          // Default source IP address, or possible name of the NIC card
                                        // or NIC number indicating which NIC card should be used.
                                        // Not sure of the best way to do this one.

    BLRUDP_INITIAL_ROUND_TRIP_DELAY = 6, // Initial estimated round trip delay when starting the connection.
                                         // Initial timeout values will be selected by this.
                                         // After the connection is running, it will be actually measured.

    BLRUDP_MAXIMUM_TIME_TO_LIVE = 7,    // Maximum time a packet can live on the network.
                                        // Determines how long a given connection context stays in
                                        // the zombie state.

    BLRUDP_IN_ORDER_DELIVERY =  8,      // When set to true, this client requires in order deliver,
                                        // false, then packets may be delivered out of order.

    BLRUDP_MTU_SIZE = 9,               // Maximum MTU size to use.  This will probably only be useful
                                        // if we can support Jumbo packets and which to tune the protocol
                                        // by using smaller packet sizes.

    BLRUDP_DEDICATED_BUFFERS = 10,      // If true then each connection context gets its own
                                        // transmit and receive buffers to work with.  If false,
                                        // then they share a common buffer pool.

    BLRUDP_USE_RAW_SOCKETS = 11,        // If true then the RlRudp library will use the RAW sockets
                                        // interface, where it fills out the UDP header.
                                        // This can further reduce the data copying that is required.

    BLRUDP_CHECKSUM = 12,               // Checksum the packets.  This is a redundant checksum on
                                        // all the packet traffic which is used to validate the contents
                                        // and make sure that the underlying protocol stack did
                                        // not screw the data up.

    BLRUDP_APPLEVEL_FLOW_CNTRL = 13,    // if this value is non zero then we are using application level
                                        // flow control.  The receive sequence number does not advance
                                        // until the application receives and releases the data.
    COMMA_DUMMY
};


//
// context states.
//
enum {
    BLRUDP_STATE_CLOSED = 0,            // state of this context is closed.
    BLRUDP_STATE_LISTENING = 1,         // state of this context is listening.
    BLRUDP_STATE_CONNECTING = 2 ,       // context is in the process of connecting to the remote end.
    BLRUDP_STATE_CONNECT = 3,           // context is connected, sends are receives are valid.
    BLRUDP_STATE_CLOSING = 4,           // the context is closing down, wont' accept any more writes,
                                        // but you can still read.
    BLRUD_MAX_STATE = 5
};


//
// Paramter passing structure.
//
// An array of these are used to override default paramters for Blue Light RUDP functions.
//
typedef struct {
    int nOptionId;                       // Option identifer
    union {
        long lValue;                    // Option value integer value.
        unsigned long ulValue;
        char *pszValue;                 // Option string pointer value.
    } v;
} BLRUDP_OPTIONS;


////////////////////////////////////////////////////////////////////////
int BlrudpNew(BLRUDP_HANDLE *phBlrudp,
              int nNumOptions,
              BLRUDP_OPTIONS pOptions[]);
//
// Create a new Blue Light RUDP manager.
//
// inputs:
//     *phBlrudp -- pointer to where to stuff the handle for the instance to this
//                  library.
//     nNumOptions -- number of paramters that we wish to override the default values for.
//     pOptions -- pointer to list of parameters.
//
// outputs
//   returns BLERR_SUCCESS if successful,
//            error codes TBD..
//    pHandle Pointer to where to stuff the Handle to the newly created Blrudp library.
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
int BlrudpDelete(BLRUDP_HANDLE hBlrudp);
//
// Delete the Blue Light RUDP manager created by BlRudpNew.
//
// inputs:
//      hBlrudp -- handle to the instance to delete.
//
// outputs:
//    return BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void BlrudpShutdown();
//
// Call this when the program shuts down.  This will go through all the outstanding
// Blrudp objets and attempt close any outstanding socket handles associated
// with it.
//
// inputs:
//    none.
// outputs:
//    none.
///////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
int BlrudpSetOptions(BLRUDP_HANDLE hBlrudp,
                     int nNumOptions,
                     BLRUDP_OPTIONS pOptions[]);
//
// set the connection options associated with this UDP manager.
// Options apply to all subsequent connections managed by this manager.
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nNumOptions  -- Number of paramters in the parameter list.
//      pOptions     -- Paramter list containing the parameters the client
//                     application wishes to override.
// outputs:
//  returns BLERR_SUCCESS if successful,
//           error codes TBD..
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
int BlrudpGetOptions(BLRUDP_HANDLE hBlrudp,
                     int nNumOptions,
                     BLRUDP_OPTIONS pOptions[]);
//
// Retrieve the current option values.
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nNumOptions  -- Number of paramters in the parameter list.
//      pOptions     -- Paramter list containing the parameters the client
//                     application wishes to query.
// outputs:
//  returns BLERR_SUCCESS if successful,
//           error codes TBD..
//
//  NOTE:
//     Pointer values returned belong to the Blrudp library.
//     DON'T MODIFY THEM.
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
int BlrudpSetOptionLong(BLRUDP_HANDLE hBlrudp,
                        int nOptionID,
                        long lValue);
//
// Set a single option (long values only).
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nOptionID   -- identifier of the option to return.
//      lValue     -- Value to set.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
int BlrudpGetOptionLong(BLRUDP_HANDLE hBlrudp,
                        int nOptionId,
                        long *plValue);
//
// Retrieve a single option (long values only).
//
// inputs:
//      hBlrudp     -- instance to the instance to apply these options to.
//      nOptionId   -- identifier of the option to return.
//      plValue     -- place to stuff the option value.
// outputs:
//      returns BLERR_SUCCESS if successful.
//      *plValue -- Value returned by the option.
//
////////////////////////////////////////////////////////////////////////


#if (!defined(BLHPK)) && (!defined(WIN32))

//////////////////////////////////////////////////////////////////////
int BlrudpGetPollList(BLRUDP_HANDLE hBlrudp,
                      unsigned int nMaxPollDescriptors,
                      unsigned int *pnNumPollDescriptors,
                      struct pollfd *pPollFd,
                      struct timeval * pTimeout);

//
// Return a list of pollfd structures containing and the timeout value to
// use if the calling function  wishes to put the application to sleep until
// something interesting happens.  Also returns the
// time the application wishes to wake up for a timeout.
//
// this allows the client application to go to sleep until something
// of interest in this library or something else the client is interested
// in pops up.
//
// inputs:
//      hBlrudp -- instance to the instance to apply these options to.
//      nMaxPollDescriptors -- maximum number of poll descriptors that there is
//                               space to return.
//      pnNumPollDescriptors -- pointer to where to stuffnumber of poll
//                              descriptors actually in the list.
//      pPollFd --  pointer to an array of pollfd structs where we can stuff
//                  the list.
//                  if (pPollFd == NULL, then just stuff the number of maximum descriptors
//                      we would have retruned...)
//     pTimeout -- pointer where to stuff the current timeout value.
//
// outputs:
//      return -- BLERR_SUCCESS if successful.
//            BLERR_STRUCT_TOO_SMALL if pPollfd area is too small to contain the
//                                   number of poll structs.
//                                   pnNumPollDescriptors -- contains the number
//                                   that would be returned if there was enough space.
//      *pnNumPollDescriptors -- number of poll fd structs in the list.
//     *pTimeout -- stuff the current timeout value.
//
//
//////////////////////////////////////////////////////////////////////////////////
#endif

#if (!defined(BLHPK))

////////////////////////////////////////////////////////////////////////////////
int BlrudpGetSelectSet(BLRUDP_HANDLE hBlrudp,
                       int *pnFds,
                       fd_set * pReadFds,
                       fd_set * pWriteFds,
                       fd_set * pExceptFds,
                       struct timeval * pTimeout);

//
// returns read, write and except File Descriptor sets which can then be used to call
// select and block the application until something interesting happens
// in the protocol.
//
// inputs:
//     hBlrudp     -- instance to the instance to apply these options to.
//     pnFds -- pointer where to stuff the highest numbered descriptor in
//              any of the FDsets.
//     pReadFds -- pointer where to stuff the read file descriptor set.
//     pWriteFds -- pointer where to stuff the write file descriptor set.
//     pExceptFds -- pointer where to stuff the except file descriptor set.
//     pTimeout -- pointer where to stuff the current timeout value.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
//     pnFds -- the highest numbered descriptor in
//                 any of the FDsets.
//     pReadFds -- the read file descriptor set.
//     pWriteFds -- the write file descriptor set.
//     pExceptFds -- the except file descriptor set.
//     pTimeout -- stuff the current timeout value.
//
//////////////////////////////////////////////////////////////////////////////////
#endif

///////////////////////////////////////////////////////////////////////////////////
int BlrudpRun(BLRUDP_HANDLE hBlrudp,
              int bSleep);
//
// Give the RudpManager time to run.
// The manager does all its work of receiving data here.
//
// If the client application has programmed any callbacks, they will be called
// from within this function so we don't have to worry about problems
// re-entrancy and interrupt contexts.
//
// inputs:
//     hBlrudp     -- instance to the instance to apply these options to.
//     bSleep      -- if TRUE the run command will sleep until there is
//                    something interesting to do.  Incomming connection,
//                    timeout, receive data, send buffer done etc...
// outputs:
//     returns -- BLERR_SUCCESS if successful.
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
int BlrudpListen(BLRUDP_HANDLE hBlrudp,
                 BLRUDP_HCONTEXT *phContext,
                 unsigned long ulIpAddr,
                 unsigned short nPort,
                 int nNumOptions,
                 BLRUDP_OPTIONS pOptions[],
                 PFN_BLRUDP_LISTEN_CONNECT_REQ pfnListenConnectReq,
                 void *pUserData);
//
// Listen for incoming connections.
//
// This causes the Blrudp manager to listen for incomming connections on
// a designated port.  Nothing in this computer can be listenening
// to this IP address and port combination.
//
// inputs:
//    hBlrudp     -- instance to the instance to apply these options to.
//    phContext   -- pointer to where to stuff the context handle to refer
//                   to this listening context.
//    ulIpAddr -- source IP address in network byte order.
//                (same as returned by inet_addr()..)
//                if zero, use the default ip Address.
//    nPort -- port to listen for incomming connections on.
//             if 0 then use the default port.
//    nNumOptions -- number of paramters that we wish to override the default values for.
//    pOptions -- pointer to list of parameters.
//    pfnListenConnectReq -- callback to call when a connect request comes in.
//    pUserData -- pointer to user data associated with this callback context.
//
//  outputs:
//     returns -- BLERR_SUCCESS if successful.
//     *phContext -- newly created listening context.
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int BlrudpAccept(BLRUDP_HCONTEXT hContext,
                 PFN_BLRUDP_CONTEXT_CHANGED pfnContextChanged,
                 PFN_BLRUDP_CONTEXT_RECV pfnContextRecv,
                 PFN_BLRUDP_SEND_BUFFER_DONE pfnSendBufferDone,
                 void *pUserData);
//
// Accept the incomming connection.
//
// inputs:
//   hContext -- handle to context that we are accepting.
//   pfnContextChanged -- pointer to context changed callback function.
//   pfnContextRecv -- pointer to context receive data callback function.
//   pfnSndBufferdone -- pointer to Send buffer done callback function.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int BlrudpReject(BLRUDP_HCONTEXT hContext);
//
// Reject the incomming connection.
//
// inputs:
//   hContext -- handle to context that we are accepting.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
int BlrudpUnlisten(BLRUDP_HCONTEXT hContext);
//
// stop listening for incomming connections for the given listening context.
//
// inputs
//    hContext   -- hContext handle to listening context.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
int BlrudpConnect(BLRUDP_HANDLE hBlrudp,
                  BLRUDP_HCONTEXT *phContext,
                  unsigned long ulIpSrcAddr,
                  unsigned short nSrcPort,
                  unsigned long ulIpDestAddr,
                  unsigned short nDestPort,
                  int nNumOptions,
                  BLRUDP_OPTIONS pOptions[],
                  PFN_BLRUDP_CONTEXT_CHANGED pfnContextChanged,
                  PFN_BLRUDP_CONTEXT_RECV pfnContextRecv,
                  PFN_BLRUDP_SEND_BUFFER_DONE pfnSendBufferDone,
                  void *pUserData);

//
// Request the Blrudp manager establish a connection with the remote SAP.
//
// inputs:
//    hBlrudp     -- instance to the instance to apply these options to.
//    phContext   -- pointer to where to stuff the new active context.
//    ulIpSrcAddr -- source IP address in network byte order.
//                 (same as returned by inet_addr()..)
//                 if zero, use the default port.
//    nSrcPort -- source port number to use, if zero use the default port.
//    ulIpDestAddr -- destination IP address in network byte order.
//                 (same as returned by inet_addr()..)
//                 if zero, use the default port.
//    nDestPort -- pointer to the port to use.
//    nNumOptions -- number of paramters that we wish to override the default values for.
//    pOptions -- pointer to list of parameters.
//    pfnContextChanged -- pointer to context changed callback function.
//    pfnContextRecv -- pointer to context receive data callback function.
//    pfnSndBufferdone -- pointer to Send buffer done callback function.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//     phContext -- newly created active context (NOTE, this won't
//                  be in the state needed to actually send data
//                  until BlrudpRun has been called a few times.
//
/////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
int BlrudpDisconnect(BLRUDP_HCONTEXT hContext,
                     int bImmediate);
//
// disconnect from a connected context.
//
// inputs:
//    hContext -- Context to disconnect from.
//    bImmediate -- disconnect immediatly....
// outputs:
//     returns -- BLERR_SUCCESS if successful.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
int BlrudpGetContextState(BLRUDP_HCONTEXT hContext);
//
// Return the current state of the context.
//
// inptus:
//    hContext -- context we are inetersted in.
// outputs:
//    returns -- the current state of the context.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
unsigned long BlrudpGetContextId(BLRUDP_HCONTEXT hContext);
//
// return an internal context identifier.  This is an identifier
// which increments monotonically each time a new context is created.
// It is used to help identify each context for debugging purposeds.
//
// inputs:
//    hContex -- the context handle we are interested in.
// outputs:
//    returns -- the context Identifier.
//
/////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
int BlrudpGetContextAddr(BLRUDP_HCONTEXT hContext,
                         unsigned long *pulSrcAddr,
                         unsigned short *pnSrcPort,
                         unsigned long *pulDestAddr,
                         unsigned short *pnDestPort);
//
// Retrieve the address and port number assocaited with this
// device.
//
// The context passed can be either an active or passive context.
// If a passive context is passed in the pulDestAddr and pulDestPort will
// be returned as zero...
//
//
// The address and port number are returned in the native byte order.
//
// inputs:
//    hContext -- context containing the address to return.
//    pulSrcAddr -- pointer to where to stuff the source adderss.
//    pnSrcPort -- pointer to where to stuff the source port number.
//    pulDestAddr -- pointer to where to stuff the destination adderss.
//    pnDestPort -- pointer to where to stuff the destination port number.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//     *pulSrcAddr -- Address of port in native byte order.
//     *pnSrcPort -- Port number.
//     *pulDestAddr -- Address of port in native byte order.
//     *pnDestPort -- Port number.
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
int BlrudpGetStatus(BLRUDP_HCONTEXT hContext,
                    int  *pnState,
                    int  *pnRecvCount,
                    int  *pnSendCount);
//
// Retrieve status about a given connection context.
//
// inputs:
//    hContext -- Context to disconnect from.
//    pnState -- pointer of where to stuff the state of the context.
//    pnRecvCount -- pointer of where to stuff the number of receive buffers
//                    waiting to be received.
//    pnSendCount -- pointer of where to stuff the number of send buffers
//                    available for sending data.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//     *pnState -- the state of the context.
//     *pnRecvCount -- the number of receive buffers
//                    waiting to be received.
//     *pnSendCount -- the number of send buffers
//                    available for sending data.
//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int BlrudpReqDiagInfo(BLRUDP_HCONTEXT hContext);
//
// Request diagnostic information from the remote context.
//
// This causes a low priority diagnostic information message to be
// sent to the remote end.  The client will be notified of the return
// message via callback.
//
// inputs:
//    hContext -- Context to disconnect from.
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int BlrudpGetDiagInfo(BLRUDP_HCONTEXT hContext,
                      int bRemote,
                      BLRUDP_DIAG *pDiag);
//
// Retrieve the current diagnostic information.
//
// this retrieves the local diagnostic information or the
// last received remote diagnostic information from the remote host.
//
// inputs:
//    hContext -- Context to disconnect from.
//    bRemote -- if TRUE, we are requesting the remote context information.
//               if FALSE, we are requesting the local context information.
//    pDiag -- poniter to where to stuff the diagnostic information.
// outputs:
//    returns - BLERR_SUCCESS if successful.
//    *pDiag -- diagnostic information...
//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int BlrudpRecvFromContext(BLRUDP_HCONTEXT hContext,
                          unsigned char *pData,
                          int nBufferLen,
                          int *pnDataLen);
//
// Receive data from a specific context copying the data into the
// buffer provided by the client.
//
// Call this with a NULL pData pointer and a pnDataLen and
// this will return the length of the data in the buffer.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      pData      -- pointer to where to stuff the data.
//      nBufferLen -- maximum length of the buffer, should the MTU size.
//      pnDataLen -- pointer of where to stuff the actual data packet length.
//
// outputs:
//      returns BLERR_SUCCESS if successful.
//              BLERR_BLRUDP_NO_RECVDATA when there is no more data to receive.
//      *pData -- data filled out.
//      *pnDataLen -- length of the received data packet.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
int BlrudpRecvBufferFromContext(BLRUDP_HCONTEXT hContext,
                                BLRUDP_HIOBUFFER *phIoBuffer,
                                unsigned char **ppData,
                                int *pnDataLen);
//
// Receive an entire receive buffer from any context.
//
// This allows the caller to receive the data without having to Blrudp
// manager do a copy operation.  It meerely returns the buffer.  When
// doing this kind of receive, the caller MUST release the buffer when
// it is done with it.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      phIoBuffer -- pointer to where to stuff the io buffer handle
//      *pData      -- pointer to where to stuff the buffer pointer.
//      pnDataLen -- pointer of where to stuff the actual data packet length.
//
// outputs:
//      returns BLERR_SUCCESS if successful.
//      *phIoBuffer -- io buffer handle.
//      **pData -- pointer ot data buffer.
//      *pnDataLen -- length of the received data packet.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
int BlrudpReleaseRecvBuffer(BLRUDP_HIOBUFFER hIoBuffer);
//
// Release a buffer received with either BlrudpRecvBufferFromContext
//
// After this call the Blrudp manager is free to re-use this buffer.  This
// is the signal that is used to signal the other end that the program
// is done using the buffer.
//
// inputs:
//    hIoBuffer -- buffer handle.
// outputs:
//      returns BLERR_SUCCESS if successful.
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
int BlrudpSendToContext(BLRUDP_HCONTEXT hContext,
                        const unsigned char *pData,
                        unsigned int nLen,
                        unsigned long ulSendBufferID);

//
//
// Send data to a connected context.  This actually copies the data
// to a free send buffer.  Note: there has to be a free send buffer to
// copy the data to for this to succeed.  Buffers may be exausted because
// the receiving end has not acknowledged the date in the send pipe.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      *pData      -- pointer to where to stuff the buffer pointer.
//      nLen -- lenght of the data to send.
//      ulSendBufferID -- identification used to identify this
//                        send buffer when it has been transmmitted
//                        and acknowleged.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
int BlrudpBufferSendToContext(BLRUDP_HCONTEXT hContext,
                       BLRUDP_HIOBUFFER hIoBuffer,
                       unsigned int nLen,
                       unsigned long ulSendBufferID);

//
//
// Send a Data buffer to a connected context.  The Blrudp manager
// takes responsibility for this buffer and when done it returns it
// to the free send buffer pool.
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      hIoBuffer -- handle of IO buffer to send...
//      nLen -- lenght of the data to send.
//      ulSendBufferID -- identification used to identify this
//                        send buffer when it has been transmmitted
//                        and acknowleged.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
int BlrudpGetSendBuffer(BLRUDP_HCONTEXT hContext,
                        BLRUDP_HIOBUFFER *phIoBuffer,
                        unsigned char **ppDataBuffer,
                        unsigned long *pulBufferLen);
//
// Request a send buffer from the free buffer pool associated with this context.
// Each buffer will be MTU size large (see BlrudpGetMaxPayloadLen()... call).
// Buffers can also be queried for their data size and actuall size.
//
// These buffers remain under the control of the context.  They are
// automatically freed when the context closes...  Therefore, when a given
// context goes to the closed state, any send buffers the client has
// become invalid...
//
// inputs:
//      hContext  -- Context handle to receive data from.
//      *ppDataBuffer -- to where to stuff the data buffer pointer.
//      pnBufferLen -- pointer to the data buffer length.
//      nLen -- lenght of the data to send.
//
// outputs:
//      returns BLERR_SUCCESS if successful.
//      *phIOBuffer io buffer handle
//      *ppDataBuffer -- data buffer.
//      *pnBufferLen -- buffer length.
//
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
int BlrudpReleaseSendBuffer(BLRUDP_HIOBUFFER hIoBuffer);
//
// Release a send buffer that was obtained by BlrudpGetSendbuffer.
//
// The client calls this when it wishes to give the buffer back to the context
// without sending data..
//
// inputs:
//    hIoBuffer -- buffer handle.
// outputs:
//      returns BLERR_SUCCESS if successful.
//
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
int BlrudpGetMaxPayloadLen(BLRUDP_HCONTEXT hContext,
                           unsigned long *pulMaxPayloadLen);
//
// Retrieve the maxium payload  size assocaied with a given context.
//
// inputs:
//    hContext -- context to retrieve the max payload length
//    pulMTUSize -- pointer of where to stuff the max payload length
// outputs:
//      returns BLERR_SUCCESS if successful.
//    *pulMTUSize -- the MTU size.
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
int BlrudpGetMaxBufferSize(BLRUDP_HIOBUFFER hIoBuffer,
                           unsigned long *pulBufferSize);
//
// Return the maximum size of this data buffer.
//
// inputs:
//    hIOBuffer -- io buffer handle
//    pulBufferSize -- pointer where to stuff the buffer size.
// outputs:
//     returns BLERR_SUCCESS if successful.
//    *pulBufferSize  -- The maximum buffer size.
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
int BlrudpGetDataSize(BLRUDP_HIOBUFFER hIoBuffer,
                      unsigned long *pulDataSize);
//
// Return the size of the data contained in this buffer.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    hIOBuffer -- io buffer handle
//    pulDataSize -- pointer where to stuff the buffer size.
// outputs:
//     returns BLERR_SUCCESS if successful.
//    *pulDataSize  -- The size of the data buffer.
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
int BlrudpGetDataPointer(BLRUDP_HIOBUFFER hIoBuffer,
                         unsigned char **ppDataBuffer);
//
// return the pointer to the data buffer assicagted with this IO buffer.
//
// inputs:
//    hBlrudp     -- instance to the Blrudp instance
//    hIOBuffer -- io buffer handle
//    pulDataSize -- pointer where to stuff the buffer size.
// outputs:
//     returns BLERR_SUCCESS if successful.
//    *pulDataSize  -- The size of the data buffer.
/////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
};
#endif


#endif
