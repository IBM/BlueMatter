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
 #include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <rts.h>

#include <BonB/blade_on_blrts.h>
#include <pk/platform.hpp>
//#include <pk/fxlogger.hpp>

void BGLTorusSendPacket( 
    int            aCoreId,
    _BGL_TorusPkt *aPkt,
    int            aTruePayloadSize,
    int            aDestRank );

int BGLTorusTryToSendPacket( 
    int aCoreId,
    _BGL_TorusPkt *aPkt,
    int            aTruePayloadSize,
    int            aDestRank );

int BGLTorusTryToSendPacketBack( int aCoreId, _BGL_TorusPkt *aPkt );

int BGLTorusRecvPacket( 
    int            aCoreId,
    char* aRecvBuffer, 
    int   aStrideSize );

int BGL_TreeRawSendPacket(
    int            vc,       // Virtual channel (0 or 1)
    _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
    void           *pyld );  // Source address of payload (16-byte aligned)

_BGL_TreeHwHdr *BGL_TreeMakeCollectiveHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int irq,                     // irq.  If combine, must be the same for all callers.
                           int opcode,                  // ALU opcode (000 for ordinary packets)
                           int opsize,                  // ALU operand size
                           int tag,                     // software tag. If combine, must be the same for all callers.
                           int csumMode );              // injection checksum mode

int BGL_TreeRawReceivePacket(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld );   // Payload buffer (16-byte aligned)

void BGL_TreeGetStatus( int vc, _BGL_TreeFifoStatus *stat );

void InitTestHeaders();
