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
 #ifndef _BLRUDP_PKT_H_
#define _BLRUDP_PKT_H_
////////////////////////////////////////////////////////////////////////////////////
//
// This file contains the packet header data structures used to build
// BLRUDP packets.
//
//

#include <hostcfg.h>
//
#if HAVE_STDINT_H
    #include <stdint.h>
#elif USE_GDB_STDINT_H
    #include "gdb_stdint.h"
#endif

#if HAVE_STRINGS_H
    #include <strings.h>
#endif

#if  HAVE_HPK_TYPES_H
#include <linux/types.h>
#endif


#include "qlink.h"


#ifdef __cplusplus
extern "C" {
#endif


#if defined(_MSC_VER)
    //
    // don't leave the alignment to chance... control it...
    #pragma pack(push)
    #pragma pack(2)                     // word boundaries....
#endif

///////////////////////////////////////////////////////////////
// Flags for ulFlags field in the pkt control header.....
enum {
    BLRUDP_PKT_STATUS_DELIVERED = (1<<0)  // packet has been received and delivered
};

enum {
    BLRUDP_PKT_UDP_HDR_LEN = 28          // udp header length + IP overhead...
};
//
// Control structure that contains packet information.
//
typedef struct {
    _QLink  link;                           // link for queing the buffer
    struct T_BLRUDP_CONTEXT *pContext;      // context that owns this packet...
    uint32_t ulSseq;                        // send sequence number in machine order.
    uint32_t ulFlags;                       // status flags...

    uint32_t ulSrcAddr;                     // receive information extracted
    uint16_t nSrcPort;                      // from the packet....
    uint32_t ulDestAddr;
    uint16_t nDestPort;

    uint32_t ulSendBufferID;                // send buffer ID used by the
                                            // sender to identify when a payload
                                            // has actually been delivered.
    uint16_t usBufferLen;                   // packet buffer length.
    uint16_t usPayloadLen;                  // packet payload length..
    uint16_t usPktLen;                      // actual length of this packet..
    void *pPayload;                         // pointer to packet payload
} BLRUDP_IO_PKT  PACKED;


//
// BLRUDP common packet header.. appears on all packets.
//
typedef struct
{
    uint16_t  usCmd;        // Identifies the type of packet this is (data,
                            // control packet, error control packet, traffic control, error packet).
    uint16_t  usDlen;       // Length of the payload section of this packet.
    uint32_t  ulSync;       // Sync field used to synchronize control packets. The sync field
                            // us defined to be incremented by one for each outgoing packet with the SREQ bit set.
                            // The outgoing packet carries the newly incremented  value.

    uint32_t  ulSseq;       // Sending sequence number for this packet (increments only for data packets).
                            // for Data packets this is the sequence number of the packet
                            // being transmitted.  For control packets, this is the number
                            // of the next data packet to be transmitted...

    uint32_t  ulRseq;       // Receiving sequence number.  Highest in-sequence
                            // number + 1 received by this SAP.

    uint32_t  ulCheck;      // Packet checksum (used only if the checksum bit is set).
} BLRUDP_PKT_HDR  PACKED;

//
// command field is divided up a follows:
//
//  ---------------------
//  |xxxx xxxx xxxx xxxx|
//  ---------------------
//   |||| ||        -----
//   |||| ||        \-------- Command Type...
//   |||| ||
//   |||| |\----------------- EOM End of message.
//   |||| \------------------ CHECKSUM
//   ||||
//   |||\-------------------- END (terminate link).
//   ||\--------------------- RCLOSE -- read closed.
//   |\---------------------- WCLOSE -- write closed.
//   \----------------------- SREQ -- sync request.
//


/////////////////////////////////////////////////////////////////
//
// SREQ         When this bit is set the receiver must immediately send
//          either a CNTL or ECNTL packet, depending on whether there
//          are missing packets.  For each packet where this bit is set
//          the Sync field of the header is incremented.
//
// WCLOSE and RCLOSE
//          These bits are the basis for disconnect handshakes carried
//          out by the close state machines (see Section 4.2.7,
//          "Association Termination"). The WCLOSE bit indi-cates
//          that no more data will be written to the outgoing data stream.
//          The RCLOSE bit indicates that all bytes written to the
//          incoming stream have been received with respect to the error
//             control policy in use.
//
// CHECKSUM This indicates that the entire packet is check-summed.
//
// END  This bit indicates that the sending context is being released.
//          It is used on last packet of a closing handshake and also
//          when a connection is being reset.  It also is sent in response
//          to the FIRST packet to reject the connection.
//
// EOM  End of message.  The sender application uses this flag to signal the
//          receiver that this packet is the end of a multi-packet message.
//          I.e. The sender has no more packets to send for now.
//

enum {
    BLRUDP_CMD_TYPE     = (0xFF),
    BLRUDP_CMD_EOM      = (1<<10),
    BLRUDP_CMD_CHECKSUM = (1<<11),
    BLRUDP_CMD_END      = (1<<12),
    BLRUDP_CMD_RCLOSE   = (1<<13),
    BLRUDP_CMD_WCLOSE   = (1<<14),
    BLRUDP_CMD_SREQ     = (1<<15),

    BLRUDP_CMD_ALL_CLOSED = (BLRUDP_CMD_RCLOSE | BLRUDP_CMD_WCLOSE)
};

enum {
    BLRUDP_CMD_TYPE_DATA    = 1,        // odd type, must be data.
    BLRUDP_CMD_TYPE_FIRST   = (2),      // first packet.    // CTRL with FIRST command.
    BLRUDP_CMD_TYPE_CTRL    = (4),      // CTRL packet.
    BLRUDP_CMD_TYPE_TCTRL   = (6),      // TCTRL packet
    BLRUDP_CMD_TYPE_ECNTL   = (8),      // Error control
    BLRUDP_CMD_TYPE_ERROR   = (10),     // error packet
    BLRUDP_CMD_TYPE_DREQ    = (12),     // diag request.
    BLRUDP_CMD_TYPE_DRESP   = (14)      // diag response.
};


//
// The Data packet..
//
typedef struct {
    BLRUDP_PKT_HDR IoPkt;            // packet with queue and common header.
    uint8_t Payload[2];             // packet payload variable length...
} BLRUDP_IO_DATA_PKT  PACKED;

//
// common control packet. This is the first part of ALL control packets.
// for the Common control packet it is the ONLY part....
//
typedef struct {
    BLRUDP_PKT_HDR IoPkt;            // packet with queue and common header.
    uint32_t      ulAllocSeq;          // The current size of the receiving window.
                                    // The receiving context is advertising that it
                                    // has space to receive Alloc.  It is the
                                    // highest sequence number that this SAP has
                                    // space to receive.

    uint32_t      ulEcho;           // The echo field carries the highest sync value
                                    // seen yet in any incoming packets.  The act of
                                    // echoing this monotonically increasing number
                                    // back to its originator gives the originator
                                    // some idea of how synchronized the end points
                                    // are.  See below for the synchronizing
                                    // handshake procedure.
} BLRUDP_IO_CTRL_PKT  PACKED;


//
// Traffic control packet.
// used to convey and negotiate traffic shaping information
// as well as common control information.
//

typedef struct {
    BLRUDP_IO_CTRL_PKT Ctrl;                // control portion of this packet
    uint16_t           nParams;             // number of parmeter blocks
    uint8_t            ParamPayload[2];     // parameter Payload (points to BLRUDP_IO_TCTRL_PAYLOAD
                                            // which is a variable length structure.
} BLRUDP_IO_TCTRL_PKT PACKED;

//
// the paramter payload is encoded as follows:
//
// -------------------------------------
// | key(1) | Len(1) | Data(Len bytes) |
// -------------------------------------
//
//
typedef struct {
    uint8_t nKey;               // paramter key
    uint8_t nLen;               // parameter data (must be even).
    uint8_t szData[2];
} BLRUDP_IO_TCTRL_PAYLOAD PACKED;      // TCTRL payload.

//
// TCTRL key enumerations.
//
enum {
    BLRUDP_TCTRL_WINDOWSIZE = 0,                // window size
    BLRUDP_TCTRL_MTUSIZE    = 1,                // max MTU size on this end...
    BLRUDP_TCTRL_CHECKSUM   = 2,                // use checksum for this session
    BLRUDP_TCTRL_COMMA_DUMMY
};

//
// Error control payload...
//
// This payload is designed to be easy to keep track of with simple
// bitmasks and offsets.  The nBaseSeq number is the base used
// to reference the bitmask.  NOTE: this may be less than the
// usSSeq/usRseq number in the header.  However, this number + the nNakmask
// offset cannot be less than the usSSeq/usRseq number in the header.
// If it is, this should generate an error packet.
//
// A 1 bit in the NAK mask indicates that the packet should be re-transmitted.
// A zero bit means NOTHING to the receiver of this.  The sender is in control.
//
//
// NOTE:  I am not sure about the overhead for this.  This increases the processing
// load for dealing with NAKS for the sender, but decreases it for the receiver.
//
typedef struct {
    uint32_t    ulBaseSeq;            // base sequence number being naked (32 bit aligned).
    uint32_t    ulNakMask;            // mask of sequence numbers being naked.
} BLRUDP_IO_ECNTL_PAYLOAD PACKED;

typedef struct {
    BLRUDP_IO_CTRL_PKT Ctrl;                    // control portion of this packet
    uint16_t           usEnctls;                // number of error controls.
    uint16_t           pad;                     // pad for allignment....
    BLRUDP_IO_ECNTL_PAYLOAD nEcntlPayload[1];   // usEnctls of error control payload
} BLRUDP_IO_ECNTL_PKT PACKED;

//
// The Error packet is used to report pathological conditions
// that are either fatal or require corrective action.
typedef struct {
    int32_t            lErrCode;                // Error Code indicating the error condition.
    int32_t            lErrValue;           // additional error information if applicable
} BLRUDP_IO_ERROR_PAYLOAD PACKED;


typedef struct {
    BLRUDP_IO_CTRL_PKT Ctrl;                        // control portion of this packet
    uint16_t           usNumErr;                    // Total number of errors reported in
                                                    // this packet (usually this will be 1).
    uint16_t           pad;                         // pad for alignment...
    BLRUDP_IO_ERROR_PAYLOAD ErrorPayload[1];        // error payload.
} BLRUDP_IO_ERROR_PKT PACKED;

enum {
    BLRUDP_IO_ERR_NOMEM         = 2,         // no memory error...
    BLUDP_ERR_CONNECTION_ACTIVE = 3          // connection active
};


//
// Diagnostic Request Packet
//
// The diagnostic request packet is used to request various telemetry about how the
// protocol is operating as seen by the host sending the diagnostic packet.  Various
// information is sent, such as current state information, number of buffers available,
// total number of re-transmissions, time the connection has been up, etc.
//
typedef struct {
    BLRUDP_IO_CTRL_PKT Ctrl;        // control portion of this packet
    uint16_t           usNumReq;    // Total number of requests in this packet.
    uint16_t           usCode[1];   // request code for the information
                                    // being requested (NumReq number of these).
} BLRUDP_IO_DIAGREQ_PKT PACKED;

// diagnostic response packet.
//
// The diagnostic packet is used to send various telemetry about how the protocol is
// performing as seen by the host sending the
// diagnostic packet.  Various information is
// sent, such as current state information,
// number of buffers available, total number of
// re-transmissions, time the connection has been up, etc.

// The purpose of the diagnostic packet is to help diagnose protocol problems and tune
// the protocol.
//
typedef struct {
    uint8_t usCode;               // Code identifying the information being sent
    uint8_t usLen;               // Length in bytes of the value section to follow(must be even).
    uint8_t Value[2];            // Value containing the information being requested.
} BLRUDP_IO_DIAGRESP_PAYLOAD PACKED;

typedef struct {
    BLRUDP_IO_CTRL_PKT Ctrl;        // control portion of this packet
    uint16_t           usNumResp;    // Total number of requests/responses in this packet.
    BLRUDP_IO_DIAGRESP_PAYLOAD DiagPayload[1];  // diagnostic payload.
} BLRUDP_IO_DIAGRESP_PKT PACKED;

/////////////////////////////////////////////////////
// diag enumerations..
enum {
    BLRUDP_DIAG_NUMRETRANS,         // total number of retransmissions.
    BLRUDP_DIAG_DATAXMIT,           // amount of data transmitted
    BLRUDP_DIAG_DATARECV            // amount of data received.
};


#if defined(_MSC_VER)
    #pragma pack(pop)
#endif

/////////////////////////////////////////////////////////////////////////////
unsigned short BlrudpXsumPkt(void *pData, int len) ;
//
// Perform checksum on the bluelight RUDP packet....
//
// This is a C function for this checksum. This is the Braden, Borman, and Partridge
// algorithm as related by Stevens in his book, “UNIX network programming.”
//
// inputs:
//   ptr -- pointer to packet to checksum.
//   len -- length to checksum in bytes.
// outputs:
//   returns -- checksum value.
//
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
BLRUDP_PKT_HDR *BlrudpGetPktPayload(BLRUDP_IO_PKT *pPkt);
//
// Given a specified packet return a pointer to the
// packet payload.
//
// inputs:
//    pPkt -- pointer to packet to get the payload of.
// outputs:
//    returns -- pointer to the packet payload.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void *BlrudpGetPktBuffer(BLRUDP_IO_PKT *pPkt);
//
// Given a specified packet return a pointer to the
// packet buffer.    This can be different than the packet payload
// in that it may include the UDP header etc...
//
// inputs:
//    pPkt -- pointer to packet to get the payload of.
// outputs:
//    returns -- pointer to the packet payload.
//
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
void *BlrudpGetDataPayload(BLRUDP_IO_PKT *pPkt);
//
// Retrieve a pointer to the data payload section of this packet..
//
// inputs:
//     pPkt -- packet to return the data payload of...
// outputs:
//     returns -- pointer to the data payload seciton...
//
//////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
uint16_t BlrudpGetDataPayloadLen(BLRUDP_IO_PKT *pPkt);
//
// Retrieve the length of the data payload of this packet..
// inputs:
//    pPkt-- packet to get the length of...
// outputs:
//    returns the data payload length
//
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
uint16_t BlrudpGetMaxDataPayloadLen(BLRUDP_IO_PKT *pPkt);
//
// Retrieve the Max length of the data payload of this packet..
// inputs:
//    pPkt-- packet to get the length of...
// outputs:
//    returns the data payload length
//
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpPktNew(uint16_t usBufferLen);
//
// Create a new packet structure with a payload length of
// usBufferLen.
//
// inputs:
//    usBufferLen -- length of the pkt buffer...
// outputs:
//    returns -- new packet
//
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void BlrudpPktDelete(BLRUDP_IO_PKT *pPkt);
//
// Free a packet allocated by BlrudpPktNew
//
// inputs:
//    pPkt -- packet to free.
// outputs:
//    none.
/////////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

#endif
