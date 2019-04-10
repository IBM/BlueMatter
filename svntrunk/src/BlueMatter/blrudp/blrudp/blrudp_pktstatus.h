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
 #ifndef _BLRUDP_PKTSTATUS_H_
#define _BLRUDP_PKTSTATUS_H_
//////////////////////////////////////////////////////////
// 
// This set of procedures handles packet status information
// used for acks and naks utilized by the Bluelight Reliable
// UDP protocol.
//
//

#ifdef __cplusplus
extern "C" {
#endif

#include "blrudp_st.h"

///////////////////////////////////////////////////////////////////////////
int BlrudpPktStatusAlloc(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                         uint32_t nNumRecvPkts);
//
// Allocate the packet status data area.
//
// inputs:
//    pPktStatus -- packet status to initialize and allocate.
//    nNumRecvPkts -- total number of receive packets to allocate this for.
// outputs:
//    returns -- BLERR_SUCCESS if successful.
//    
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
void BlrudpPktStatusFree(BLRUDP_RECV_PKT_STATUS *pPktStatus);
//
// Free the data associated with this packet status structure.
//
// inputs:
//    pPktStatus -- packet status to free contents of.
// outputs:
//    none.
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void BlrudpPktStatusSetBaseSeq(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                               uint32_t ulBaseSeq);
//
// Set/advance the base sequence number for the packet status...
//
// This is done when the system advances the rSeq number it transmits
// to the remote computer.
//
// As the sequence number is advanced, previous bits are cleared
// in clumps of 32.
//
// inputs:
//    pPktStatus -- packet status to set the base sequence number for.
//    ulBaseSeq -- new base sequence number to set.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void BlrudpPktStatusSetEndSeq(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                              uint32_t ulEndSeq);
//
// Set/advance the end sequence number for the packet status...
//
// This is done when the system advances the sSeq number it has received
// from the remote computer.
//
// inputs:
//    pPktStatus -- packet status to set the base sequence number for.
//    ulBaseSeq -- new base sequence number to set.
// outputs:
//    none.
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void BlrudpPktStatusSetRecvd(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                             uint32_t ulSeq);
//
// Set the status for the sequence number specified to indicate that it
// has been received.
//
// inputs:
//    pPktStatus -- packet status to set the base sequence number for.
//    ulSeq -- sequence number to set.
// outputs:
//    none.
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
int BlrudpPktStatusFillErrPayload(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                                  uint32_t ulStartSeq,
                                  uint32_t ulEndSeq,
                                  BLRUDP_IO_ECNTL_PAYLOAD *pPayload,
                                  uint32_t ulMaxErrors);
//
// Fill the error payload from the packet status area.
// 
// inputs:
//    pPktStatus -- packet status to fill thepayload from.
//    ulStartSeq -- start sequence number to start counting Nak packets.
//    ulEndSeq -- ending sequence number to stop counting Nak packets.
//                one past the last packet we should consider.
//    pPayload -- pointer to payload area to fill.
//    ulMaxErrors -- maximum number of err payload items to add to the
//                   payload.
// outputs:
//    returns -- number of error packets actually added to the payload.
//

#ifdef __cplusplus
};
#endif


#endif

