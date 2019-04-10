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
 //////////////////////////////////////////////////////////
// 
// This set of procedures handles packet status information
// used for acks and naks utilized by the Bluelight Reliable
// UDP protocol.
//
//


#include "hostcfg.h"

#if HAVE_MEMORY_H
#include <memory.h>
#endif

#if HAVE_LINUX_IN_H
	#include "linux/in.h"
#endif


#include "BlErrors.h"
#include "blrudp_memory.h"


#include "blrudp_pktstatus.h"

#include "dbgPrint.h"
#include "bldefs.h"

///////////////////////////////////////////////////////////////////////////
int BlrudpPktStatusAlloc(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                         uint32_t nNumRecvPkts)
//
// Allocate the packet status data area.
//
// inputs:
//    pPktStatus -- packet status to initialize and allocate.
//    nNumRecvPkts -- total number of receive packets to allocate this for.
// outputs:
//    returns -- BLERR_SUCCESS if successful.
//    
{
    int nErr = BLERR_INTERNAL_ERR;

    pPktStatus->ulBaseSeq = 0;
    pPktStatus->ulEndSeq = 0;
    pPktStatus->ulBaseIndex = 0;

    if (nNumRecvPkts)
    {
        uint32_t nSize;

        pPktStatus->ulNumEntries = (((nNumRecvPkts+31)/32) + 2);
        nSize = sizeof(uint32_t) * pPktStatus->ulNumEntries;
        pPktStatus->pvectRecv = 
            (uint32_t *)BlrAllocMem(nSize);
        if (pPktStatus->pvectRecv == NULL)
            FAILERR(nErr, BLERR_NO_MEMORY);
        memset(pPktStatus->pvectRecv, 
               0,
               nSize);                       
    }
    else
    {
        pPktStatus->pvectRecv = NULL;
        pPktStatus->ulNumEntries = 0;
    }

    return(BLERR_SUCCESS);
Fail:
    BlrudpPktStatusFree(pPktStatus);
    return(nErr);
}

////////////////////////////////////////////////////////////////////////
void BlrudpPktStatusFree(BLRUDP_RECV_PKT_STATUS *pPktStatus)
//
// Free the data associated with this packet status structure.
//
// inputs:
//    pPktStatus -- packet status to free contents of.
// outputs:
//    none.
//
{
    if (pPktStatus->pvectRecv)
    {
        BlrFreeMem(pPktStatus->pvectRecv);
        pPktStatus->pvectRecv = NULL;
    }

}

/////////////////////////////////////////////////////////////////////////
void BlrudpPktStatusSetBaseSeq(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                               uint32_t ulBaseSeq)
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
{
    uint32_t ulNewIndex;
    uint32_t ulBaseIndex;
    ASSERT(pPktStatus->pvectRecv);
    
    ulNewIndex = (ulBaseSeq>>5) % pPktStatus->ulNumEntries;

    ASSERT(pPktStatus->ulBaseIndex == ((pPktStatus->ulBaseSeq>>5) % pPktStatus->ulNumEntries));
    ulBaseIndex = pPktStatus->ulBaseIndex;


    //
    // Clear out 32 bit chunks of the status.
    //
    // this is allocated slightly oversize so we don't have to clear a bit at a time...
    //

    while (ulBaseIndex != ulNewIndex)
    {
        pPktStatus->pvectRecv[ulBaseIndex] = 0;
        ulBaseIndex++;
        if (ulBaseIndex >= pPktStatus->ulNumEntries)        // wrap it around...
            ulBaseIndex = 0;
    }
    pPktStatus->ulBaseSeq = ulBaseSeq;      // save off the information.
    pPktStatus->ulBaseIndex = ulNewIndex;


}
/////////////////////////////////////////////////////////////////////////
void BlrudpPktStatusSetEndSeq(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                              uint32_t ulEndSeq)
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
{
    pPktStatus->ulEndSeq = ulEndSeq;        // remember this...
                                            // do we have processing do do???
}

//////////////////////////////////////////////////////////////////////
void BlrudpPktStatusSetRecvd(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                             uint32_t ulSeq)
//
// Set the status for the sequence number specified to indicate that it
// has been received.
//
// inputs:
//    pPktStatus -- packet status to set the base sequence number for.
//    ulSeq -- sequence number to set.
// outputs:
//    none.
{
    uint32_t ulIndex;
    int nBit;


    ulIndex = (ulSeq>>5) % pPktStatus->ulNumEntries;
    nBit = ulSeq & 0x1f;
    pPktStatus->pvectRecv[ulIndex] |= (1<<nBit);        // ding the bit in question...


}


///////////////////////////////////////////////////////////////////////
uint32_t BlrudpPktStatusIndex(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                              uint32_t              ulSeq)
//
// Calculate the vector index associated with this status.
//
// inputs:
//     pPktStatus -- packet status to set the base sequence number for.
//     pPkt -- packet to add to the vector...
//     ulSeq -- sequence number to calulate the vector for.
// outputs:
//     returns -- the vector index...
//
// NOTE: may want to make this inline code....
{
    uint32_t ulIndex;

    //
    // this one is a straight modulous..
    ulIndex = (ulSeq>>5) % pPktStatus->ulNumEntries;

    return(ulIndex);
}

////////////////////////////////////////////////////////////////////
int BlrudpPktStatusFillErrPayload(BLRUDP_RECV_PKT_STATUS *pPktStatus,
                                  uint32_t ulStartSeq,
                                  uint32_t ulEndSeq,
                                  BLRUDP_IO_ECNTL_PAYLOAD *pPayload,
                                  uint32_t ulMaxErrs)
//
// Fill the error payload from the packet status area.
// 
// inputs:
//    pPktStatus -- packet status to fill thepayload from.
//    ulStartSeq -- start sequence number to start counting Nak packets.
//    ulEndSeq -- ending sequence number to stop counting Nak packets.
//                one past the last packet we should consider.
//    pPayload -- pointer to payload area to fill.
//    ulMaxErrs -- maximum number of err payload items to add to the
//                   payload.
// outputs:
//    returns -- number of error packets actually added to the payload.
//
{
    uint32_t ulIndex;
    uint32_t ulEndIndex;
    uint32_t ulSeq;
    uint32_t ulNumErrs;

    ulSeq = ulStartSeq;

    ulIndex = BlrudpPktStatusIndex(pPktStatus,
                                   ulSeq);
    ulEndIndex = BlrudpPktStatusIndex(pPktStatus,
                                   ulEndSeq);

    ulNumErrs = 0;


    if (ulIndex == ulEndIndex)      // first and last the same is a special case...
    {
        uint32_t ulAckMask;
        ulAckMask = ((1<<(ulSeq & 0x1f))-1);    // do the leading bits.
        ulAckMask |= (~((1<<(ulEndSeq & 0x1f))-1)); // add trailing bits.
        pPayload->ulBaseSeq = htonl(ulSeq & (~0x1f));   
        pPayload->ulNakMask = htonl(~(pPktStatus->pvectRecv[ulIndex] | ulAckMask));

        ulNumErrs++;
        pPayload++;
    }
    else
    {
        uint32_t ulAckMask;
        // compute a mask for the bits that are already acked.
        // this applies to the first set of bits only...
        ulAckMask = ((1<<(ulSeq & 0x1f))-1);
        while (ulIndex != ulEndIndex)
        {   
            pPayload->ulBaseSeq = htonl(ulSeq & (~0x1f));   
            pPayload->ulNakMask = htonl(~(pPktStatus->pvectRecv[ulIndex] | ulAckMask));

            ulNumErrs++;
            ulIndex++;
            pPayload++;
            ulAckMask = 0;      // after the first one we don't need this...
            ulSeq = (ulSeq & (~0x1f)) + 32; // next  sequence number...
            if (ulIndex >= pPktStatus->ulNumEntries)        // did we wrap???
                ulIndex = 0;                                            // yes.. start again..
            if (ulNumErrs >= ulMaxErrs)     // make sure we don't overflow the packet...
                break;
        }
        //
        // do we have some final clean up to doo.....
        if (ulEndSeq & (0x1f) && (ulNumErrs < ulMaxErrs))      // last packets...
        {
            ulAckMask = (~((1<<(ulEndSeq & 0x1f))-1));
            pPayload->ulBaseSeq = htonl(ulSeq & (~0x1f));   
            pPayload->ulNakMask = htonl(~(pPktStatus->pvectRecv[ulIndex] | ulAckMask));

            pPayload++;
            ulNumErrs++;

        }
    }
    return(ulNumErrs);
}
