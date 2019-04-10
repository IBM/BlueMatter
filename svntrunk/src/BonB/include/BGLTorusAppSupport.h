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
 #ifndef __BONB_BGL_TORUS_APP_SUPPORT_H__
#define __BONB_BGL_TORUS_APP_SUPPORT_H__

__BEGIN_DECLS
#if defined(BONB_APPSUPPORT_EXTERN)
#define STATIC extern
#else
#define STATIC static
#endif

STATIC void _ts_GetStatus0( _BGL_TorusFifoStatus *stat );
STATIC void _ts_GetStatus1( _BGL_TorusFifoStatus *stat );
STATIC void _ts_GetStatusN_nosync( _BGL_TorusFifoStatus *aHardwareStatusRegister, _BGL_TorusFifoStatus *stat );
STATIC void _ts_GetStatusBoth( _BGL_TorusFifoStatus *stat0, _BGL_TorusFifoStatus *stat1 );
STATIC void _ts_MakeSendHintsF0( _BGL_TorusPktHdr *hdr );
STATIC _BGL_TorusPktHdr *BGLTorusMakeHdrApp(
                     _BGL_TorusPktHdr *hdr, // Filled in on return
                     int dest_x,            // destination coordinates
                     int dest_y,
                     int dest_z,
                     Bit32 arg0,            // first  argument
                     Bit32 arg1,            // second argument
                     int payload_bytes );    // bytes in payload, I convert to chunks
STATIC _BGL_TorusPktHdr *BGLTorusMakeHdrAppDeposit(
    _BGL_TorusPktHdr *hdr, // Filled in on return
    Bit32 arg0,            // first  argument
    Bit32 arg1,            // second argument
    int payload_bytes );    // bytes in payload, I convert to chunks
STATIC _BGL_TorusPktHdr *BGLTorusMakeHdrAppChooseRecvFifo(
    _BGL_TorusPktHdr *hdr, // Filled in on return
    int dest_x,            // destination coordinates
    int dest_y,
    int dest_z,
    Bit32 arg0,            // first  argument
    Bit32 arg1,            // second argument
    int payload_bytes,     // bytes in payload, I convert to chunks
    int RecvFifoPicker );   // either { 0, 1 } picks either fifos0 or fifos1 to recv

// Choose a Low Priority Fifo: NOTE BLAKE IS FORCING X+ and X- etc, INTO SAME FIFO!!! (that's bad)
STATIC _BGL_TorusFifo *_ts_CheckClearToSendOnLink0( _BGL_TorusFifoStatus *stat, int fifo_number, int chunks );
STATIC _BGL_TorusFifo *_ts_CheckClearToSendOnLink( _BGL_TorusFifoStatus *stat, int fifo_number, int chunks );
STATIC _BGL_TorusFifo *_ts_GetNextFifoCheckClearToSendOnLink0( _BGL_TorusFifoStatus *stat, int chunks );
STATIC _BGL_TorusFifo *_ts_GetNextFifoCheckClearToSendOnLink1( _BGL_TorusFifoStatus *stat, int chunks );
STATIC _BGL_TorusFifo *_ts_CheckClearToSendOnLink1( _BGL_TorusFifoStatus *stat, int fifo_number, int chunks );
extern _BGL_TorusPktHdr expect0;
extern _BGL_TorusPktHdr expect1;

extern _BGL_TorusPktHdr mask;
STATIC void InitHeader( _BGL_TorusPktHdr* hdr_check, int aCoreId );
STATIC void BGLTorusInjectPacketImageApp(
        _BGL_TorusFifo *fifo,  // Injection FIFO to use (Must have space available!)
        _BGL_TorusPkt  *pkt );  // Address of ready-to-go packet

STATIC void BGLTorusInjectAppDeposit(
        _BGL_TorusFifo   *fifo,          // Injection FIFO to use (Must have space available!)
        _BGL_TorusPktHdr *hdr,           // Packet header we edit here
        _BGL_TorusPktPayload    *payload,       // Packet Payload
        unsigned         hint_bit,       // one of _BGL_TORUS_PKT_HINT_{X|Y|Z}{P|M}
        int              dest_hard_processor_id,
        int              dest_x,         // destination coordinates
        int              dest_y,         //
        int              dest_z);         //

STATIC int PullHeader( _BGL_TorusFifo *fifo, void *addr );

STATIC Bit8 PullFromRecvTorusFifo( Bit8 *stat, _BGL_TorusFifo *fifo, void *addr );

// Put: move entire packet from fifo to buf
STATIC int _ts_AppReceiver_Put( Bit8 *stat, _BGL_TorusFifo *fifo, void *addr );

STATIC int _ts_PollF0_OnePassReceive_Put( _BGL_TorusFifoStatus *statF0, void *base_addr, int stride );

// same as above but with c/F0/F1/*m
STATIC int _ts_PollF1_OnePassReceive_Put( _BGL_TorusFifoStatus *statF1, void *base_addr, int stride );
__END_DECLS
STATIC void _ts_AppReceiver_Drain( _BGL_TorusFifo *fifo, int ChunksInFifo, void * drainBuffer ) ;

// We can choose whether to bind with AppSupport at compile time or link time
#if !defined(BONB_APPSUPPORT_EXTERN)
#include <BonB/BGLTorusAppSupport.c>
#endif

#endif
