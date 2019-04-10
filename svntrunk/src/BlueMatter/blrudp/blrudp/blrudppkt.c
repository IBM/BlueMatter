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
 ///////////////////////////////////////////////////////////////////////////
//
// This file contains functions used to support
// constructing packets to send using the BLRUDP protocol.
//
// Packets are constructed in network byte order, ready to 
// transmit.  If we find ourselves doing a LOT of flipping
// data then record data redundantly in the header, next to the
// link data to allow rapid scanning without doing silly 
// byte flips.
//
//



#include "hostcfg.h"

#if HAVE_LINUX_IN_H
	#include "linux/in.h"
#endif

#include "blrudppkt.h"
#include "blrudp_st.h"
#include "qlink.h"
#include "blrudp_memory.h"
#include "dbgPrint.h"

#include "bldefs.h"

/////////////////////////////////////////////////////////////////////////////
unsigned short BlrudpXsumPkt(void *pData, int len) 
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
{
    unsigned int sum = 0;
    unsigned short oddbyte = 0;
    unsigned short answer;
    unsigned short* ptr = (unsigned short* )pData;
    /*
    * Algorithm: use a 32-bit accumulator (sum) and add sequential
    * 16-bit words into it; at the end, add the two halves of sum together
    * to fold back the carries. Return the one’s complement of this.
    */
    while (len > 1) 
    {
        sum += ntohs(*ptr++);       // Think we have to compensate for native byte order here.
        len -= 2;
    }
    /* Mop up an odd byte, if necessary. */
    if (len == 1) 
    {
        oddbyte = *(unsigned char*)ptr;
        sum += oddbyte;
    }
    
    /* Add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xFFFF);     /* Add high-16 to low-16 */
    sum += (sum >> 16);                     /* Add carry             */
    answer = ~sum;                          /* One’s complement      */

    /* Return the answer in knotwork byte order*/
    return(htons(answer));      
}

//////////////////////////////////////////////////////////////////////
BLRUDP_PKT_HDR *BlrudpGetPktPayload(BLRUDP_IO_PKT *pPkt)
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
{
    //
    // For now this is a simple offset into the iopkt 
    // It is abstracted to make it easier to change the
    // underlying buffering structure.
    //
    return((BLRUDP_PKT_HDR *)pPkt->pPayload);
}


//////////////////////////////////////////////////////////////////////
void *BlrudpGetPktBuffer(BLRUDP_IO_PKT *pPkt)
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
{
    //
    // For now this is a simple offset into the iopkt 
    // It is abstracted to make it easier to change the
    // underlying buffering structure.
    //
    return(pPkt->pPayload);  // currently there is no difference here...
}

/////////////////////////////////////////////////////////////////
uint16_t BlrudpGetDataPayloadLen(BLRUDP_IO_PKT *pPkt)
//
// Retrieve the length of the data payload of this packet..
// inputs:
//    pPkt-- packet to get the length of...
// outputs:
//    returns the data payload length
//
/////////////////////////////////////////////////////////////////
{
    BLRUDP_PKT_HDR *pPktHdr;
    uint16_t usDataLen;

    pPktHdr = BlrudpGetPktPayload(pPkt);

    ASSERT(pPkt->usPktLen >= offsetof(BLRUDP_IO_DATA_PKT, Payload));
    if (pPkt->usPktLen >= offsetof(BLRUDP_IO_DATA_PKT, Payload))
        usDataLen = pPkt->usPktLen - offsetof(BLRUDP_IO_DATA_PKT, Payload);
    else
        usDataLen  = 0;
    return(usDataLen);
}                                      

/////////////////////////////////////////////////////////////////
void *BlrudpGetDataPayload(BLRUDP_IO_PKT *pPkt)
//
// Retrieve a pointer to the data payload section of this packet..
//
// inputs:
//     pPkt -- packet to return the data payload of...
// outputs:
//     returns -- pointer to the data payload seciton...
//
//////////////////////////////////////////////////////////////
{
    
    BLRUDP_IO_DATA_PKT *pDataPkt;
    pDataPkt = (BLRUDP_IO_DATA_PKT *)BlrudpGetPktPayload(pPkt);

    return(&pDataPkt->Payload);

}
/////////////////////////////////////////////////////////////////
uint16_t BlrudpGetMaxDataPayloadLen(BLRUDP_IO_PKT *pPkt)
//
// Retrieve the Max length of the data payload of this packet..
// inputs:
//    pPkt-- packet to get the length of...
// outputs:
//    returns the data payload length
//
/////////////////////////////////////////////////////////////////
{
    return(pPkt->usPayloadLen - offsetof(BLRUDP_IO_DATA_PKT, Payload));
}




/////////////////////////////////////////////////////////////
BLRUDP_IO_PKT *BlrudpPktNew(uint16_t usBufferLen)
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
{
    BLRUDP_IO_PKT *pPkt = NULL;

    ASSERT(usBufferLen > 0);
    if (usBufferLen == 0)
        FAIL;

    pPkt = (BLRUDP_IO_PKT *)BlrAllocMem(sizeof(*pPkt));
    if (pPkt == NULL)
        FAIL;

    pPkt->link.next = pPkt->link.prev = NULL;
    pPkt->pContext = NULL;        
    pPkt->ulSseq = 0;              
    pPkt->usBufferLen = usBufferLen;
    pPkt->usPayloadLen = usBufferLen - BLRUDP_PKT_UDP_HDR_LEN;
    pPkt->ulFlags = 0L;
    pPkt->ulSendBufferID = 0L;
    pPkt->pPayload = BlrAllocAlignedMem(usBufferLen);
    if (pPkt->pPayload == NULL)
        FAIL;

    return(pPkt);
Fail:
    if (pPkt)
        BlrudpPktDelete(pPkt);
    return(NULL);

}

/////////////////////////////////////////////////////////////////
void BlrudpPktDelete(BLRUDP_IO_PKT *pPkt)
//
// Free a packet allocated by BlrudpPktNew
//
// inputs:
//    pPkt -- packet to free.
// outputs:
//    none.
/////////////////////////////////////////////////////////////////
{
    ASSERT(pPkt->link.next == NULL);       // hey, somebody has this packet queued...
    if (pPkt->link.next)
        DeQ(&pPkt->link);           // remove it, but this really was an error.

    BlrFreeAlignedMem(pPkt);        // dump the packet...
}

