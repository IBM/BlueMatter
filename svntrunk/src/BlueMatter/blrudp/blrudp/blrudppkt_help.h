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
 #ifndef _BLRUDP_PKT_HELP_H
#define _BLRUDP_PKT_HELP_H
/////////////////////////////////////////////////////////////
//
// This file contains packet helper functions.  They are used
// to encode and decode blrudp packets.
//
//

#include "blrudppkt.h"
#include "blrudp_st.h"

#ifdef __cplusplus
extern "C" {
#endif




/////////////////////////////////////////////////////////////
void BlrudpFillFirstPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt);
//
// Fill out the contents of a first packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with first packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////
void BlrudpFillCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                       BLRUDP_IO_PKT *pPkt);
//
// Fill out the contents of a CTRL packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with first packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
void BlrudpFillTCtrlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                         BLRUDP_IO_PKT *pPkt);
//
// Fill a TCtrl packet with the options contents 
// and sequence numbers from the pContext
//
// Fill out the contents of a first packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt  -- pointeer to buffer to fill out with TCTRL packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void BlrudpFillEcntlPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt);
//
// Fill out the contents of a ECNTL (error control) packet 
// and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with control packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void BlrudpFillErrorPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt,
                        int32_t lErrCode,
                        int32_t lErrValue);
//
// Fill an error packet with a single error code.....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pErrPkt -- poitner to buffer to fill out with error packet
//                 information.
//    lErrCode -- error code to stuff.
//    lErrValue -- value to stuff with it....
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void BlrudpFillDiagReq(BLRUDP_ACTIVE_CONTEXT *pContext,
                       BLRUDP_IO_PKT *pPkt);
//
// Fill out the contents of a DIAGREQ packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with first packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void BlrudpFillDiagResp(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_IO_PKT *pPkt);
//
// Fill out the contents of a RESPONSE packet and prepare for sending....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with first packet
//                 information.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
uint16_t BlrudpGetPktCmd(BLRUDP_PKT_HDR *pPkt);
//
// Pick off and encode the packet command without the flags....
//
// inputs:
//    pPkt -- packet to decode this from.
// outputs:
//    returns -- command in host byte order.
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void BlrudpFillResetPkt(BLRUDP_ACTIVE_CONTEXT *pContext,
                        BLRUDP_IO_PKT *pPkt,
                        uint32_t ulSseq);
//
// Fill out the contents of a Reset packet and prepare for sending....
//
// A reset packet is a control packet with an overriden rSeq and
// the END bit set.  This causes a half open context on the other
// end to abort....
//
// inputs:
//    pContext -- context to construct the packet for.
//    pPkt -- pointeer to buffer to fill out with control packet
//                 information.
//    ulSseq -- send sequence number to use override.
// outputs:
//    output none.
/////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
void BlrudpFillDataPktHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                          BLRUDP_IO_PKT *pPkt);
//
// Fill out the data packet header..  The data payload is already filled out
// here and the data length field is set...
//
// inputs:
//     pContext -- context of the packet to refresh.
//     pPkt -- packet to refresh.
// outputs:
//    none.
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void BlrudpRefreshPktHdr(BLRUDP_ACTIVE_CONTEXT *pContext,
                         BLRUDP_IO_PKT *pPkt);
//
// Refresh the packet header with current Sync and Echo and SREQ fields.
// 
// inputs:
//     pContext -- context of the packet to refresh.
//     pPkt -- packet to refresh.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void BlrudpDumpPktHex(BLRUDP_IO_PKT *pPkt);
//
// dump the entire packet contents out the debug port in hex..
//
// inputs:
//    pPkt -- pointer to packet to dump.
// outputs:
//    dumped packet...
//
/////////////////////////////////////////////////////////////



#if (BYTE_ORDER == BIG_ENDIAN)
    #define hton64(x) (x)
    #define ntoh64(x) (x)
#else
//////////////////////////////////////////////////////////////////
uint64_t hton64 (uint64_t lValue );
//
// 64 bit byte flip function.
// inputs:
//    lValue -- value to flip.
// outputs:
//    lValue -- flipped value if the machine is little endian.
//
///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////    
uint64_t ntoh64(uint64_t lValue );
//
// 64 bit byte flip function.
// inputs:
//    lValue -- value to flip.
// outputs:
//    lValue -- flipped value if the machine is little endian.
//
///////////////////////////////////////////////////////////////
#define ntoh64(x) hton64((x))
#endif


#ifdef __cplusplus
};
#endif

#endif
