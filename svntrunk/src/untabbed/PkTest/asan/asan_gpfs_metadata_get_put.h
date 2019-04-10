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
 #ifndef __ASAN_GPFS_METADATA_GET_PUT_H__
#define __ASAN_GPFS_METADATA_GET_PUT_H__

struct GpfsMetadataGetPacketRequest
{
  int BlockIndex;
  int OffsetOfDataInBlock;
  int Length;
  char* ReceiveBuffer;
};

struct GpfsMetadataGetPacketResponse
{
  char                   Length;
  char*                  ReceiveBuffer;
  pkFiberControlBlockT * FCB;
  char                   Data[ 0 ];
};

struct GpfsMetadataPutPacketRequest
  {
  int BlockIndex;
  int OffsetOfDataInBlock;    
  int Length;  
  char Data[0];
  };

struct GpfsMetadataPutPacketResponse
  {
  pkFiberControlBlockT * FCB;
  };


#define MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET ( _BGL_TORUS_PKT_MAX_PAYLOAD - sizeof( GpfsMetadataGetPacketResponse ) )

#define MAX_DATA_BYTES_IN_GPFS_METADATA_REQUEST_PUT_PACKET ( _BGL_TORUS_PKT_MAX_PAYLOAD - sizeof( GpfsMetadataPutPacketRequest ) )

int ProcessGpfsMetadataGetResponseFx( void* arg)
  {
    GpfsMetadataGetPacketResponse* Pkt = (GpfsMetadataGetPacketResponse *) arg;
    BegLogLine(PKFXLOG_ASAN_GET_PUT) 
      << "ProcessGpfsMetadataGetResponseFx Pkt->ReceiveBuffer=0x" << (void *)(Pkt->ReceiveBuffer)
      << " Pkt->Data=0x" << (void *)(Pkt->Data)
      << " Pkt->Length=" << (int) (Pkt->Length)
      << EndLogLine ;
      
    int * DataAsInt = (int *) &(Pkt->Data) ;
    BegLogLine(PKFXLOG_ASAN_GET_PUT)
      << "Data 0x" << (void *) (DataAsInt[0])
                   << (void *) (DataAsInt[1])
                   << (void *) (DataAsInt[2])
                   << (void *) (DataAsInt[3])
                   << (void *) (DataAsInt[4])
                   << (void *) (DataAsInt[5])
                   << (void *) (DataAsInt[6])
                   << (void *) (DataAsInt[7])
      << EndLogLine ;
    
    
    memcpy( Pkt->ReceiveBuffer, Pkt->Data, Pkt->Length );
    
    pkFiberUnblock( Pkt->FCB );

    return 0;
  }

int ProcessGpfsMetadataPutResponseFx( void* arg)
  {
    GpfsMetadataPutPacketResponse* Pkt = (GpfsMetadataPutPacketResponse *) arg;
    
    BegLogLine(PKFXLOG_ASAN_GET_PUT) 
      << "ProcessGpfsMetadataPutResponseFx "
      << " Pkt=0x" << (void *)Pkt
      << EndLogLine ;
      
    pkFiberUnblock( Pkt->FCB );

    return 0;
  }

int ProcessGpfsMetadataPutRequestFx( void* arg )
  {
    GpfsMetadataPutPacketRequest* Pkt = (GpfsMetadataPutPacketRequest *) arg;

    int Length              = Pkt->Length;
    int BlockIndex          = Pkt->BlockIndex;
    int OffsetOfDataInBlock = Pkt->OffsetOfDataInBlock;    
    
    char* StartOfMetadataBlock = GetGpfsMetadata( BlockIndex );
    if( StartOfMetadataBlock == NULL )
      StartOfMetadataBlock = ReserveForGpfsMetadata( BlockIndex );
    
    char* RequestedDataPtr = & StartOfMetadataBlock[ OffsetOfDataInBlock ];

    BegLogLine(PKFXLOG_ASAN_GET_PUT) 
      << "ProcessGpfsMetadataGetResponseFx Length=" << Length
      << " BlockIndex=" << BlockIndex
      << " OffsetOfDataInBlock=" << OffsetOfDataInBlock
      << " StartOfMetadataBlock=0x" << (void *)StartOfMetadataBlock
      << " RequestedDataPtr=0x" << (void *)RequestedDataPtr
      << EndLogLine ;
      
    int * DataAsInt = (int *) &(Pkt->Data) ;
    BegLogLine(PKFXLOG_ASAN_GET_PUT)
      << "Data 0x" << (void *) (DataAsInt[0])
                   << (void *) (DataAsInt[1])
                   << (void *) (DataAsInt[2])
                   << (void *) (DataAsInt[3])
                   << (void *) (DataAsInt[4])
                   << (void *) (DataAsInt[5])
                   << (void *) (DataAsInt[6])
                   << (void *) (DataAsInt[7])
      << EndLogLine ;
    
    memcpy( RequestedDataPtr, Pkt->Data, Length );
    
    GpfsMetadataPutPacketResponse* ResPkt = (GpfsMetadataPutPacketResponse *)
      PkActorReserveResponsePacket( ProcessGpfsMetadataPutResponseFx,
            sizeof( GpfsMetadataPutPacketResponse ) );
        
    unsigned long SourceNodeId;
    unsigned long SourceCoreId;
    pkFiberControlBlockT* Context = NULL;
    PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );   

    BegLogLine(PKFXLOG_ASAN_GET_PUT) 
      << "ProcessGpfsMetadataGetResponseFx Replying to SourceNodeId=" << SourceNodeId
      << " SourceCoreId=" << SourceCoreId
      << EndLogLine ;
    
    ResPkt->FCB           = Context;

    PkActorPacketDispatch( ResPkt );
    
    return 0;    
  }

int ProcessGpfsMetadataGetRequestFx( void* arg )
  {
    GpfsMetadataGetPacketRequest* Pkt = (GpfsMetadataGetPacketRequest *) arg;

    int Length              = Pkt->Length;
    int BlockIndex          = Pkt->BlockIndex;
    int OffsetOfDataInBlock = Pkt->OffsetOfDataInBlock;    
    char* ReceiveBuffer     = Pkt->ReceiveBuffer;
    
    char* StartOfMetadataBlock = GetGpfsMetadata( BlockIndex );
    if( StartOfMetadataBlock == NULL )
      StartOfMetadataBlock = ReserveForGpfsMetadata( BlockIndex );
    
    char* RequestedDataPtr = & StartOfMetadataBlock[ OffsetOfDataInBlock ];
    
    int SizeToSend = sizeof( GpfsMetadataGetPacketResponse ) + Length;
    
    GpfsMetadataGetPacketResponse* ResPkt = (GpfsMetadataGetPacketResponse *)
      PkActorReserveResponsePacket( ProcessGpfsMetadataGetResponseFx,
            SizeToSend );
    
    BegLogLine(PKFXLOG_ASAN_GET_PUT) 
      << "ProcessGpfsMetadataGetRequestFx Length=" << Length
      << " BlockIndex=" << BlockIndex
      << " OffsetOfDataInBlock=" << OffsetOfDataInBlock
      << " ReceiveBuffer=0x" << (void *)ReceiveBuffer
      << " StartOfMetadataBlock=0x" << (void *)StartOfMetadataBlock
      << " RequestedDataPtr=0x" << (void *)RequestedDataPtr
      << " SizeToSend=" << SizeToSend
      << " ResPkt=0x" << (void *)ResPkt
      << EndLogLine ;
      
    int * DataAsInt = (int *) RequestedDataPtr ;
    BegLogLine(PKFXLOG_ASAN_GET_PUT)
      << "Data 0x" << (void *) (DataAsInt[0])
                   << (void *) (DataAsInt[1])
                   << (void *) (DataAsInt[2])
                   << (void *) (DataAsInt[3])
                   << (void *) (DataAsInt[4])
                   << (void *) (DataAsInt[5])
                   << (void *) (DataAsInt[6])
                   << (void *) (DataAsInt[7])
      << EndLogLine ;
    
    memcpy( ResPkt->Data, RequestedDataPtr, Length );
    
    ResPkt->ReceiveBuffer = ReceiveBuffer;
    ResPkt->Length        = Length;
    
    unsigned long SourceNodeId;
    unsigned long SourceCoreId;
    pkFiberControlBlockT* Context = NULL;
    PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );   
    
    ResPkt->FCB           = Context;

    PkActorPacketDispatch( ResPkt );
    
    return 0;    
  }

void SendGpfsMetadataGetRequest( int       NodeId,
         int       BlockIndex,
         int       OffsetOfDataInBlock,
         char*     RecvBuffer,
         int       Length )
{
  // Send a request one packet at a time

  int NumberOfFullPackets = Length / MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET;

  int BytesInLastPacket = Length % MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET;
  
  for( int Pck = 0; Pck < NumberOfFullPackets; Pck++ )
    {
      GpfsMetadataGetPacketRequest* Pkt = (GpfsMetadataGetPacketRequest *) 
  PkActorReservePacket( NodeId,
            1,
            ProcessGpfsMetadataGetRequestFx,
            sizeof( GpfsMetadataGetPacketRequest ) );

      Pkt->BlockIndex = BlockIndex;

      Pkt->OffsetOfDataInBlock = OffsetOfDataInBlock + 
  Pck * MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET;

      Pkt->Length = MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET;
      Pkt->ReceiveBuffer = RecvBuffer + (Pck * MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET);
  
      PkActorPacketDispatch( Pkt );
      
      PkFiberBlock();
    }  

  if( BytesInLastPacket )
    {
      GpfsMetadataGetPacketRequest* Pkt = (GpfsMetadataGetPacketRequest *) 
  PkActorReservePacket( NodeId,
            1,
            ProcessGpfsMetadataGetRequestFx,
            sizeof( GpfsMetadataGetPacketRequest ) );
      
      Pkt->BlockIndex = BlockIndex;
      
      Pkt->OffsetOfDataInBlock = OffsetOfDataInBlock + 
  NumberOfFullPackets * MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET;

      Pkt->Length = BytesInLastPacket;
      Pkt->ReceiveBuffer = RecvBuffer + 
  ( NumberOfFullPackets * MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET );
  
      PkActorPacketDispatch( Pkt );
      
      PkFiberBlock();      
    }
}

void SendGpfsMetadataPutRequest( int       NodeId,
         int       BlockIndex,
         int       OffsetOfDataInBlock,
         char*     DataBuffer,
         int       Length )
{
  // Send a request one packet at a time

  int NumberOfFullPackets = Length / MAX_DATA_BYTES_IN_GPFS_METADATA_REQUEST_PUT_PACKET;

  int BytesInLastPacket = Length % MAX_DATA_BYTES_IN_GPFS_METADATA_REQUEST_PUT_PACKET;
  
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "SendGpfsMetadataPutRequest NodeId " << NodeId
        << " BlockIndex " << BlockIndex
        << " OffsetOfDataInBlock " << OffsetOfDataInBlock
        << " DataBuffer 0x" << DataBuffer
        << " Length " << Length
        << EndLogLine ;
        
  for( int Pck = 0; Pck < NumberOfFullPackets; Pck++ )
    {
      int SizeToSend = sizeof( GpfsMetadataPutPacketRequest ) + MAX_DATA_BYTES_IN_GPFS_METADATA_REQUEST_PUT_PACKET;
      
      GpfsMetadataPutPacketRequest* Pkt = (GpfsMetadataPutPacketRequest *) 
  PkActorReservePacket( NodeId,
            1,
            ProcessGpfsMetadataPutRequestFx,
            SizeToSend );
      
      Pkt->BlockIndex = BlockIndex;
      
      int index = Pck * MAX_DATA_BYTES_IN_GPFS_METADATA_REQUEST_PUT_PACKET;

      Pkt->OffsetOfDataInBlock = OffsetOfDataInBlock + index;

      Pkt->Length = MAX_DATA_BYTES_IN_GPFS_METADATA_REQUEST_PUT_PACKET;
  
      int * DataAsInt = (int *) (&DataBuffer[ index ]) ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "SendGpfsMetadataPutRequest Length " << MAX_DATA_BYTES_IN_GPFS_METADATA_REQUEST_PUT_PACKET
        << " Data 0x" << (void *) (DataAsInt[0])
                      << (void *) (DataAsInt[1])
                      << (void *) (DataAsInt[2])
                      << (void *) (DataAsInt[3])
                      << (void *) (DataAsInt[4])
                      << (void *) (DataAsInt[5])
                      << (void *) (DataAsInt[6])
                      << (void *) (DataAsInt[7])
        << EndLogLine ;
    
      memcpy( Pkt->Data, &DataBuffer[ index ], Pkt->Length );
      
      PkActorPacketDispatch( Pkt );
      
      PkFiberBlock();
    }  

  if( BytesInLastPacket )
    {      
      int SizeToSend = sizeof( GpfsMetadataPutPacketRequest ) + BytesInLastPacket;

      GpfsMetadataPutPacketRequest* Pkt = (GpfsMetadataPutPacketRequest *) 
  PkActorReservePacket( NodeId,
            1,
            ProcessGpfsMetadataPutRequestFx,
            SizeToSend );
      
      Pkt->BlockIndex = BlockIndex;
      
      int index = NumberOfFullPackets * MAX_DATA_BYTES_IN_GPFS_METADATA_RESPONSE_GET_PACKET;

      Pkt->OffsetOfDataInBlock = OffsetOfDataInBlock + index;

      Pkt->Length = BytesInLastPacket;
  
      int * DataAsInt = (int *) (&DataBuffer[ index ]) ;
      BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "SendGpfsMetadataPutRequest Length " << BytesInLastPacket
        << " Data 0x" << (void *) (DataAsInt[0])
                     << (void *) (DataAsInt[1])
                     << (void *) (DataAsInt[2])
                     << (void *) (DataAsInt[3])
                     << (void *) (DataAsInt[4])
                     << (void *) (DataAsInt[5])
                     << (void *) (DataAsInt[6])
                     << (void *) (DataAsInt[7])
        << EndLogLine ;
      memcpy( Pkt->Data, &DataBuffer[ index ], Pkt->Length );

      PkActorPacketDispatch( Pkt );
      
      PkFiberBlock();
    }
}

void GetAsanGpfsMetadata( int       MasterNode, 
        int       BlockIndex,
        int       OffsetOfDataInBlock,
        char*     RecvBuffer,
        int       Length )
{
   int myNode = PkNodeGetId() ;
    BegLogLine(PKFXLOG_ASAN_GET_PUT)
      << " GetAsanGpfsMetadata MasterNode=" << MasterNode
      << " myNode=" << myNode
      << " BlockIndex=" << BlockIndex
      << " OffsetOfDataInBlock=" << OffsetOfDataInBlock
      << " RecvBuffer=0x" << (void *)RecvBuffer
      << " Length=" << Length
      << EndLogLine ;
  if( MasterNode == myNode )
    {
      return;
    }
  else
    {
      SendGpfsMetadataGetRequest( MasterNode,
          BlockIndex,
          OffsetOfDataInBlock,
          RecvBuffer,
          Length );
    }
   int * DataAsInt = (int *) RecvBuffer ;
   BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "Length " << Length
        << " Data 0x" << (void *) (DataAsInt[0])
                     << (void *) (DataAsInt[1])
                     << (void *) (DataAsInt[2])
                     << (void *) (DataAsInt[3])
                     << (void *) (DataAsInt[4])
                     << (void *) (DataAsInt[5])
                     << (void *) (DataAsInt[6])
                     << (void *) (DataAsInt[7])
        << EndLogLine ;
}

void PutAsanGpfsMetadata( int       MasterNode, 
        int       BlockIndex,
        int       OffsetOfDataInBlock,
        char*     DataBuffer,
        int       Length )
{
   int myNode = PkNodeGetId() ;
    BegLogLine(PKFXLOG_ASAN_GET_PUT)
      << " PutAsanGpfsMetadata MasterNode=" << MasterNode
      << " myNode=" << myNode
      << " BlockIndex=" << BlockIndex
      << " OffsetOfDataInBlock=" << OffsetOfDataInBlock
      << " DataBuffer=0x" << (void *)DataBuffer
      << " Length=" << Length
      << EndLogLine ;
   int * DataAsInt = (int *) DataBuffer ;
   BegLogLine(PKFXLOG_ASAN_GET_PUT)
        << "Length " << Length
        << " Data 0x" << (void *) (DataAsInt[0])
                     << (void *) (DataAsInt[1])
                     << (void *) (DataAsInt[2])
                     << (void *) (DataAsInt[3])
                     << (void *) (DataAsInt[4])
                     << (void *) (DataAsInt[5])
                     << (void *) (DataAsInt[6])
                     << (void *) (DataAsInt[7])
        << EndLogLine ;
  if( MasterNode == myNode )
    {
      return;
    }
  else
    {
      SendGpfsMetadataPutRequest( MasterNode,
          BlockIndex,
          OffsetOfDataInBlock,
          DataBuffer,
          Length );
    }   
}
#endif
