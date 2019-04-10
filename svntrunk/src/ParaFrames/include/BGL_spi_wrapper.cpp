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
 #include <spi/BGL_spi_wrapper.hpp>
#include <BonB/BGLTorusAppSupport.c>

#if 0
_BGL_TorusPktHdr expect0;
_BGL_TorusPktHdr expect1;
_BGL_TorusPktHdr mask;
#endif 

void BGLTorusSendPacket( 
    int            aCoreId,
    _BGL_TorusPkt *aPkt,
    int            aTruePayloadSize,
    int            aDestRank )
  {
  _BGL_TorusFifoStatus FifoStatus;

  _BGL_TorusFifo * NextFifoToSend = NULL;
  int FifoSelect = 0;
  while( NextFifoToSend == NULL )
    {
    if( aCoreId == 0 )
      {
      _ts_GetStatus0( & FifoStatus );    
      NextFifoToSend = _ts_CheckClearToSendOnLink0( & FifoStatus, FifoSelect, 8 );    
      }
    else
      {
      _ts_GetStatus1( & FifoStatus );    
      NextFifoToSend = _ts_CheckClearToSendOnLink1( & FifoStatus, FifoSelect, 8 );    
      }

    FifoSelect++;
    if( FifoSelect == 3 )
      FifoSelect = 0;
    }
  
  int x,y,z;
  Platform::Topology::GetCoordsFromRank( aDestRank, &x, &y, &z );

  BGLTorusMakeHdrAppChooseRecvFifo( & aPkt->hdr,
                                    x,
                                    y,
                                    z,
                                    Platform::Topology::GetAddressSpaceId(), // arg 0
                                    aDestRank,                               // arg 1
                                    aTruePayloadSize,                        // payload in bytes
                                    aCoreId );
  
  BGLTorusInjectPacketImageApp( NextFifoToSend, ( _BGL_TorusPkt* ) aPkt );
  }

#define TORUS_SEND_FIFO_COUNT 3

int BGLTorusTryToSendPacket( 
    int            aCoreId,
    _BGL_TorusPkt *aPkt,
    int            aTruePayloadSize,
    int            aDestRank )
  {
  _BGL_TorusFifoStatus FifoStatus;
  _BGL_TorusFifo * NextFifoToSend = NULL;
  int FifoSelect = 0;
  
  int try_count = 0;

  int chunks = ((aTruePayloadSize + 15) >> 5);

  assert( aCoreId == 1 );

  while( try_count < TORUS_SEND_FIFO_COUNT )
    {
    if( aCoreId == 0 )
      _ts_GetStatus0( & FifoStatus );
    else
      _ts_GetStatus1( & FifoStatus );

    if( aCoreId == 0 ) NextFifoToSend = _ts_CheckClearToSendOnLink0( & FifoStatus, FifoSelect, chunks   );
    else               NextFifoToSend = _ts_CheckClearToSendOnLink1( & FifoStatus, FifoSelect, chunks   );

    if( NextFifoToSend != NULL )
      break;
  
    FifoSelect++;
    if( FifoSelect == TORUS_SEND_FIFO_COUNT )
      FifoSelect = 0;
    
    try_count++;
    }
  
  if( NextFifoToSend == NULL )
    return 0;

  int x,y,z;
  Platform::Topology::GetCoordsFromRank( aDestRank, &x, &y, &z );

  BGLTorusMakeHdrAppChooseRecvFifo( & aPkt->hdr,
                                    x,
                                    y,
                                    z,
                                    Platform::Topology::GetAddressSpaceId(), // arg 0
                                    aDestRank,                               // arg 1
                                    aTruePayloadSize,                        // payload in bytes
                                    aCoreId );
  
  BGLTorusInjectPacketImageApp( NextFifoToSend, ( _BGL_TorusPkt* ) aPkt );
  return 1;
  }

int BGLTorusTryToSendPacketBack( int aCoreId, _BGL_TorusPkt *aPkt )
  {
  _BGL_TorusFifoStatus FifoStatus;
  _BGL_TorusFifo * NextFifoToSend = NULL;
  int FifoSelect = 0;
  
  int try_count = 0;

  int chunks = aPkt->hdr.hwh0.Chunks;
  assert( aCoreId == 1 );

  while( try_count < TORUS_SEND_FIFO_COUNT )
    {
    if( aCoreId == 0 )
      _ts_GetStatus0( & FifoStatus );
    else
      _ts_GetStatus1( & FifoStatus );

    if( aCoreId == 0 ) NextFifoToSend = _ts_CheckClearToSendOnLink0( & FifoStatus, FifoSelect, chunks   );
    else               NextFifoToSend = _ts_CheckClearToSendOnLink1( & FifoStatus, FifoSelect, chunks   );
    
    if( NextFifoToSend != NULL )
      break;
  
    FifoSelect++;
    if( FifoSelect == TORUS_SEND_FIFO_COUNT )
      FifoSelect = 0;
    
    try_count++;
    }
  
  if( NextFifoToSend == NULL )
    return 0;

  int OldSourceRank = aPkt->hdr.swh0.arg0;
  int OldDestRank   = aPkt->hdr.swh1.arg;
  
  int NewSourceRank = OldDestRank;
  int NewDestRank   = OldSourceRank;
  
  int x,y,z;
  Platform::Topology::GetCoordsFromRank( NewDestRank, &x, &y, &z );

#if 0
  aPkt->hdr.swh0.arg0 = NewSourceRank;
  aPkt->hdr.swh1.arg  = NewDestRank;
  aPkt->hdr.hwh0.X = x;
  aPkt->hdr.hwh1.Y = y;
  aPkt->hdr.hwh1.Z = z;
#endif

  // int chunks = aPkt->hdr.hwh0.Chunks;
  int pid    = aPkt->hdr.hwh0.Pid;

  BGLTorusMakeHdrAppChooseRecvFifo( & aPkt->hdr,
                                    x,
                                    y,
                                    z,
                                    NewSourceRank, // arg 0
                                    NewDestRank,                               // arg 1
                                    0,                        // payload in bytes
                                    pid );
  
  aPkt->hdr.hwh0.Chunks = chunks;

  BGLTorusInjectPacketImageApp( NextFifoToSend, ( _BGL_TorusPkt* ) aPkt );
  return 1;
  }

int BGLTorusRecvPacket( 
    int aCoreId,
    char* aRecvBuffer, 
    int   aStrideSize )
  {
  _BGL_TorusFifoStatus FifoStatus;  
  
  int PacketCount = 0;
  if( aCoreId == 0 )
    {
    _ts_GetStatus0( & FifoStatus );

    PacketCount = _ts_PollF0_OnePassReceive_Put( & FifoStatus,
                                                 aRecvBuffer,
                                                 aStrideSize );
    }
  else
    {
    _ts_GetStatus1( & FifoStatus );
    PacketCount = _ts_PollF1_OnePassReceive_Put( & FifoStatus,
                                                 aRecvBuffer,
                                                 aStrideSize );
    }
  ////////////////// _ts_GetStatus0( & FifoStatus );
    
  return PacketCount;
  }

int BGL_TreeRawSendPacket(
    int            vc,       // Virtual channel (0 or 1)
    _BGL_TreeHwHdr *hdrHW,   // Previously created hardware header (any type)
    void           *pyld )  // Source address of payload (16-byte aligned)
{
return BGLTreeRawSendPacket( vc, hdrHW, pyld );
}

_BGL_TreeHwHdr *BGL_TreeMakeCollectiveHdr(
                           _BGL_TreeHwHdr *hdr,         // Filled in on return
                           int vt,                      // class (virtual tree)
                           int irq,                     // irq.  If combine, must be the same for all callers.
                           int opcode,                  // ALU opcode (000 for ordinary packets)
                           int opsize,                  // ALU operand size
                           int tag,                     // software tag. If combine, must be the same for all callers.
                           int csumMode )              // injection checksum mode
{
return BGLTreeMakeCollectiveHdr( hdr, vt, irq, opcode, opsize, tag, csumMode );
}

int BGL_TreeRawReceivePacket(
       int            vc,       // Virtual channel (0 or 1)
       _BGL_TreeHwHdr *hdrHW,   // Hardware header buffer
       void           *pyld )   // Payload buffer (16-byte aligned)
{
return BGLTreeRawReceivePacket( vc, hdrHW, pyld );
}

void BGL_TreeGetStatus( int vc, _BGL_TreeFifoStatus *stat )
{
BGLTreeGetStatus( vc, stat );
}

#if 0
void InitTestHeaders()
  {
#if 0
  InitHeader( &expect0, 0 );  
  InitHeader( &expect1, 1 );  

  unsigned int* MaskPtr = (unsigned int *) & mask;
  MaskPtr[ 0 ] = 0xffffffff;
  MaskPtr[ 1 ] = 0xffffffff;
  mask.hwh0.Chunks = 0;
  mask.hwh0.Pid = 0;
  mask.hwh0.X = 0xf8;
  mask.hwh1.Y = 0xf8;
  mask.hwh1.Z = 0xf8;
  mask.hwh1.Resvd0 = 0;
  mask.hwh1.Resvd1 = 0;
#endif
  }
#endif
