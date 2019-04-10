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
 #ifndef __BMM_RDG_MPI_SendToNodeZero_hpp__
#define __BMM_RDG_MPI_SendToNodeZero_hpp__


#ifndef PKFXLOG_BMM_RDG
#define PKFXLOG_BMM_RDG  0
#endif

#ifndef BMM_RDG_SplitFileSize
#define BMM_RDG_SplitFileSize (1024)
#endif

#ifndef BMM_RDG_SplitFileSizeInBytes
#define BMM_RDG_SplitFileSizeInBytes  ((BMM_RDG_SplitFileSize) * (1024*1024))
#endif

#ifndef BMM_RDG_NodeBufferSize
#define BMM_RDG_NodeBufferSize ( 12 )  // 6MB is good for ~100k sites -- probspec can adjust
#endif

#ifndef BMM_RDG_NodeBufferSizeInBytes
#define BMM_RDG_NodeBufferSizeInBytes ( BMM_RDG_NodeBufferSize * 1024 * 1024 )
#endif

// #ifndef BMM_RDG_NodeZeroBufferCount
// #define BMM_RDG_NodeZeroBufferCount ( 2 ) // 2 at least double buffers
// #endif

#include <spi/BGL_spi_wrapper.hpp>

#ifdef PK_RAIDFS
char * BMM_RDG_FileBaseName = "/raid/hsmprod/RawDatagramOutput";
#else
#ifdef BMM_RDG_Split
static char * BMM_RDG_FileBaseName = "RDG";
#else
static char * BMM_RDG_FileBaseName = "RawDatagramOutput";
#endif
#endif

static int BMM_RDG_Fd = -1;

static unsigned int BMM_RDG_Extension = 0;

static int
BMM_RDG_OpenFile( char *PathName )
  {

  BMM_RDG_Fd = open(  PathName,
                      O_WRONLY  | O_TRUNC | O_CREAT,  ////// | O_LARGEFILE,
                      0666 );

  if( BMM_RDG_Fd < 0 )
    PLATFORM_ABORT( "Failed to open rdg file");

  return(0);
  }

static int
BMM_RDG_OpenOrReopenRDGFile( int NoClose = 0 )
  {
  if( ! NoClose )
    {
    #ifdef BMM_RDG_Split
      fsync( BMM_RDG_Fd );
      int close_rc =  close( BMM_RDG_Fd );
      if( close_rc != 0 )
        PLATFORM_ABORT("BMM_RDG_SwitchToNextFile(): faild on close.");
    #else
      PLATFORM_ABORT("BMM_RDG_SwitchToNextFile(): call to close/open but  BMM_RDG_Split not defined");
    #endif
    }

  char PathName[4096];

  if( strlen( BMM_RDG_FileBaseName ) > sizeof( PathName ) - 10 )
    PLATFORM_ABORT("RDG file base name too long" );

  #ifdef BMM_RDG_Split
    sprintf( PathName,
             "%s.%04d.%04d.%04d.%04d.srdg",
             BMM_RDG_FileBaseName,
             BMM_RDG_Extension,
             Platform::Topology::GetAddressSpaceId(),
             Platform::Topology::GetAddressSpaceCount(),
             BMM_RDG_SplitFileSize
             );
    BMM_RDG_Extension++;
  #else
    sprintf( PathName, "%s", BMM_RDG_FileBaseName );
  #endif

  BegLogLine(1)
    << "BMM_RDG_OpenOrReopenRDGFile "
    << "Opening file "
    << ">" << PathName << "<"
    << EndLogLine

  int rc = BMM_RDG_OpenFile( PathName );

  return( rc );
  }

static unsigned int BMM_RDG_TotalBytesWrittenToFile = 0;

static void BMM_RDG_WriteRecord( int aRdgFd, char * aBuffer, int aLen );

static void
BMM_RDG_Write( int aRdgFd, char * aBuffer, int aLen )
  {
  if( aLen == 0 )
    return;

  #ifdef BMM_RDG_Split
    // First, code doesn't support writing more than max file size in one record.
    if( aLen > BMM_RDG_SplitFileSizeInBytes )
      PLATFORM_ABORT(" Code path not implemented to handle single write > BMM_RDG_SplitFileSizeInBytes" );

    if( ( BMM_RDG_TotalBytesWrittenToFile + aLen ) > BMM_RDG_SplitFileSizeInBytes )
      {
      int BytesToWriteBeforeClosingFile = BMM_RDG_SplitFileSizeInBytes - BMM_RDG_TotalBytesWrittenToFile;

      BMM_RDG_WriteRecord( aRdgFd, aBuffer, BytesToWriteBeforeClosingFile );

      aBuffer += BytesToWriteBeforeClosingFile;
      aLen    -= BytesToWriteBeforeClosingFile;
      BMM_RDG_OpenOrReopenRDGFile();
      BMM_RDG_TotalBytesWrittenToFile = 0;
      }
  #endif

  if( aLen == 0 )
    return;

  BMM_RDG_WriteRecord( aRdgFd, aBuffer, aLen );
  BMM_RDG_TotalBytesWrittenToFile += aLen;

  }


static TraceClient RdgIODelayStart;
static TraceClient RdgIODelayFinis;


static void
BMM_RDG_WriteRecord( int aRdgFd, char * aBuffer, int aLen )
  {

  BegLogLine( 0 & PKFXLOG_BMM_RDG )
    << "BMM_RDG_Write"
    << " Src Buf @"
    << (void *) aBuffer
    << " Len "
    << aLen
    << " First 4 words: "
    << (void *) (((int *)aBuffer)[0])
    << " "
    << (void *) (((int *)aBuffer)[1])
    << " "
    << (void *) (((int *)aBuffer)[2])
    << " "
    << (void *) (((int *)aBuffer)[3])
    << EndLogLine;

  if( aLen == 0 )
    return;

  int ZeroWriteCount    = 0;
  int IOProblemFlag     = 0;
  int BytesLeft         = aLen;
  int BytesLeftOffset   = 0;

  for(int TryCount = 0; 1 ; TryCount++ )
    {
    int WriteRc = ::write( aRdgFd, &(aBuffer[ BytesLeftOffset ]), BytesLeft );

    BegLogLine( PKFXLOG_BMM_RDG )
      << "BMM_RDG_WriteRecord() called ::write "
      << " TryCount "
      << TryCount
      << " BytesLeft "
      << BytesLeft
      << " of "
      << aLen
      << " WriteRc "
      << WriteRc
      << EndLogLine;


    if( WriteRc < 0 )
      PLATFORM_ABORT("BMM_RDG_Write() ::write() failed");

    if( WriteRc == BytesLeft )
      break;

    // From here, we did a partial write and need to deal with the fallout

    BytesLeft       -= WriteRc;
    BytesLeftOffset += WriteRc;

    // Back off -- try to give the fs a chance to catch it's breath.
    // Note on the first fail retry is immediate (sleep is 0)
    if( ZeroWriteCount )
      sleep( ZeroWriteCount < 10 ? ZeroWriteCount : 10 );

    if( WriteRc == 0 )
      ZeroWriteCount++;
    else
      ZeroWriteCount = 0;

    if( ZeroWriteCount == 10 )
      {
      IOProblemFlag = 1;
      BegLogLine(1)
        << "Trouble writing RDG.  Blocked waiting for IO to succeed"
        << EndLogLine;
      RdgIODelayStart.HitOE( PKTRACE_RDG_IO,
                             "RdgIODelay",
                             0,
                             RdgIODelayStart );
      }

    }

  if( IOProblemFlag )
    {
    RdgIODelayFinis.HitOE( PKTRACE_RDG_IO,
                           "RdgIODelay",
                           0,
                           RdgIODelayFinis );
    BegLogLine(1)
      << "Trouble writing RDG CLEARED."
      << EndLogLine;
    }
  return;
  }

static int                BlueMatterMonitor_Fd     = -1;
static int                BlueMatterMonitor_Socket = 0;

static int  BMM_RDG_LocalBufferIndex = 0;

static Platform::Mutex::MutexWord BMM_RDG_BufferLock;// = 0;

static TraceClient DoTransportStart;
static TraceClient DoTransportFinis;


typedef char *        BMM_RDG_BufferPtrType;
static BMM_RDG_BufferPtrType BMM_RDG_AllBufPtrs;

static int *IOSendCounts;
static int *IOSendOffsets;
static int *IORecvCounts;
static int *IORecvOffsets;

static int
BMM_RDG_Init( char * aPathName = NULL )
  {
  #ifdef RDG_TO_DEVNULL
    return 0;
  #endif

  BegLogLine( PKFXLOG_BMM_RDG )
    << "BMM_RDG_Init() Entered "
    << EndLogLine;

  // This allows passing a pointer to the path name in, probably an argv pointer.
  if( aPathName != NULL )
    BMM_RDG_FileBaseName = aPathName;

  BMM_RDG_AllBufPtrs = (char *) malloc( BMM_RDG_NodeBufferSizeInBytes );

  BMM_RDG_LocalBufferIndex = 0;

  if( Platform::Topology::GetAddressSpaceId() == 0 )
    {
    // Open the file without closing a previous file.
    BMM_RDG_OpenOrReopenRDGFile( 1 );
    
    if( BMM_RDG_Fd < 0 )
      PLATFORM_ABORT( "Failed to open rdg file");    
    }

  int Size = Platform::Topology::GetSize();
  IOSendCounts = (int *) malloc( Size * sizeof(int) );
  assert( IOSendCounts );

  IOSendOffsets = (int *) malloc( Size * sizeof(int) );
  assert( IOSendOffsets );

  IORecvCounts = (int *) malloc( Size * sizeof(int) );
  assert( IORecvCounts );

  IORecvOffsets = (int *) malloc( Size * sizeof(int) );
  assert( IORecvOffsets );

  for( int i=0; i<Size; i++ )
    {
    IOSendOffsets[ i ] = 0;
    IORecvOffsets[ i ] = 0;
    IORecvCounts[ i ] = 0;
    IOSendCounts[ i ] = 0;
    }
  
  return( 0 );
  }

#ifndef BMM_RDG_FlushPeriod
#define BMM_RDG_FlushPeriod 100
#endif

// Don't allow more than 128 ts between flush's to avoid massive skew in rdg file
#if BMM_RDG_FlushPeriod > 128
#define BMM_RDG_FlushPeriod 128
#endif

#define RDG_PACKET_ADDITIONAL_SIZE (sizeof( unsigned char ) + sizeof(int))
#define RDG_DATA_PACKET_SIZE (sizeof( _BGL_TorusPktPayload ) - RDG_PACKET_ADDITIONAL_SIZE)

struct IO_RDG_Packet
  {
  _BGL_TorusPktHdr mPacketHeader;
  int              mNumPacketsToBeSentFromThisNode; // This is set on a key packet
  unsigned char    mDataLength;    
  char             mRdgData [ RDG_DATA_PACKET_SIZE ];
  } ALIGN_QUADWORD;

static IO_RDG_Packet RecvBuffer[ 6 ];

static inline
void
BMM_RDG_Flush( int aTimeStep )
  {
  #ifdef RDG_TO_DEVNULL
    return;
  #endif

  if( aTimeStep % BMM_RDG_FlushPeriod != 0 )
    return ;

  Platform::Control::Barrier();

  DoTransportStart.HitOE( PKTRACE_RDG_IO,
                          "RdgFlush",
                          0,
                          DoTransportStart );

  EnergyManagerIF.ReduceAndEmitEnergies( aTimeStep );

  int MyRank = Platform::Topology::GetAddressSpaceId();
  int Size   = Platform::Topology::GetSize();    

  // Flush buffer on node zero.
  if( MyRank == 0 )
    {
    BMM_RDG_Write( BMM_RDG_Fd, BMM_RDG_AllBufPtrs, BMM_RDG_LocalBufferIndex );
    BMM_RDG_LocalBufferIndex = 0;
    
    // Receive from all p-1 nodes
    // Node 0 expects to get P-1 key packets. 
    // Each key packet has the count of packets to receive from that node.
    
    // Iterate till all the key packets came in.    
    int ExpectedKeyPacketCount           = Platform::Topology::GetAddressSpaceCount() - 1;
    int RecvedKeyCount                   = 0;
    int TotalPacketsToRecvInKeyPktsCount = 0;
    int TotalPacketsRecved               = 0;

    while( 1 )
      {
      // If there's no room for at least 6 more packets 
      // (which is the max number of packets we can receive at one time)
      // then send flush the current buffer
      int PacketCount = BGLTorusRecvPacket( Platform::Thread::GetId(), (char *) RecvBuffer, sizeof( IO_RDG_Packet ) );
      for( int i=0; i < PacketCount; i++ )
        {
        IO_RDG_Packet* RdgPktPtr = &RecvBuffer[ i ];
        TotalPacketsRecved ++;
        
        if( RdgPktPtr->mNumPacketsToBeSentFromThisNode != -1 )
          {
          TotalPacketsToRecvInKeyPktsCount += RdgPktPtr->mNumPacketsToBeSentFromThisNode;
          RecvedKeyCount++;
          }
        
        int SpaceLeftInBuffer = BMM_RDG_NodeBufferSizeInBytes - BMM_RDG_LocalBufferIndex;
        
        if( SpaceLeftInBuffer < RdgPktPtr->mDataLength )
          {
          BMM_RDG_Write( BMM_RDG_Fd, BMM_RDG_AllBufPtrs, BMM_RDG_LocalBufferIndex );
          BMM_RDG_LocalBufferIndex = 0;
          }

        memcpy( &BMM_RDG_AllBufPtrs[ BMM_RDG_LocalBufferIndex ],
                RdgPktPtr->mRdgData,
                RdgPktPtr->mDataLength );

        BMM_RDG_LocalBufferIndex += RdgPktPtr->mDataLength;
        }

      if(( RecvedKeyCount == ExpectedKeyPacketCount )
         && ( TotalPacketsToRecvInKeyPktsCount == TotalPacketsRecved ))
        {        
        break;
        }
      }
    
    if( BMM_RDG_LocalBufferIndex )
      {
      BMM_RDG_Write( BMM_RDG_Fd, BMM_RDG_AllBufPtrs, BMM_RDG_LocalBufferIndex );
      BMM_RDG_LocalBufferIndex = 0;
      }    
    }
  else
    {
    // Figure out how many packets this node has to send to Node0 
    // Send that count in a key packet        
    char* DataToSend = BMM_RDG_AllBufPtrs;
    
    int PacketCount = 0;

    IO_RDG_Packet RdgPkt;
    RdgPkt.mDataLength = 0;

    while( DataToSend != &BMM_RDG_AllBufPtrs[ BMM_RDG_LocalBufferIndex ] )
      {
      ED_Header* Hdr = (ED_Header *) DataToSend;
      if( ( Hdr->mLen + RdgPkt.mDataLength ) > RDG_DATA_PACKET_SIZE )
        {        
        int TruePayloadSize = RDG_PACKET_ADDITIONAL_SIZE + RdgPkt.mDataLength;
        RdgPkt.mNumPacketsToBeSentFromThisNode = -1;
        BGLTorusSendPacket( Platform::Thread::GetId(), (_BGL_TorusPkt *) &RdgPkt, TruePayloadSize, 0 );

        RdgPkt.mDataLength = 0;
        PacketCount++;
        }
        
      memcpy( &RdgPkt.mRdgData[ RdgPkt.mDataLength ], DataToSend, Hdr->mLen );
      RdgPkt.mDataLength += Hdr->mLen;
      DataToSend += Hdr->mLen;
      }
    
    // Send the filled packet, this is also the key packet
    // If there's nothing to send we at least send one packet.
    PacketCount++;
    RdgPkt.mNumPacketsToBeSentFromThisNode = PacketCount;
    int TruePayloadSize = RDG_PACKET_ADDITIONAL_SIZE + RdgPkt.mDataLength;
    BGLTorusSendPacket( Platform::Thread::GetId(), (_BGL_TorusPkt *) &RdgPkt, TruePayloadSize, 0 ); 
    
    BMM_RDG_LocalBufferIndex = 0;
    }
  
#if 0
  // Get Data from each node 1 buffers at a time  
  for( int i=1; i<Size; i++ )
    {    
    if( MyRank == i )
      {    
      IOSendCounts[ 0 ]  = BMM_RDG_LocalBufferIndex ? BMM_RDG_LocalBufferIndex : 1;
      BMM_RDG_LocalBufferIndex = 0;
      }

    // Only node zero expect to recv from node i
    if( MyRank == 0 )
      IORecvCounts[ i ]  = 1;

    Platform::Collective::Alltoallv( BMM_RDG_AllBufPtrs[ 0 ],
                                     IOSendCounts,
                                     IOSendOffsets,
                                     sizeof( char ),
                                     BMM_RDG_AllBufPtrs[ 1 ],
                                     IORecvCounts,
                                     IORecvOffsets,
                                     sizeof( char ) );
    if( MyRank == 0 )
      {
      if( IORecvCounts[ i ] > 1 )
        BMM_RDG_Write( BMM_RDG_Fd, BMM_RDG_AllBufPtrs[ 1 ], IORecvCounts[ i ] );
      
      // Don't need to hear from that node no mo'
      IORecvCounts[ i ] = 0;
      }
    
    if( MyRank == i )
      {
      // Node i doesn't need to sent nothing to nobody no mo'
      IOSendCounts[ 0 ]  = 0;
      }
    }
#endif
  DoTransportFinis.HitOE( PKTRACE_RDG_IO,
                          "RdgFlush",
                          0,
                          DoTransportFinis );
  
  Platform::Control::Barrier();
  
  BegLogLine( PKFXLOG_BMM_RDG )
      << "Exiting BMM_RDG Write Barrier"
      << EndLogLine;
  
  Platform::Mutex::Unlock( &BMM_RDG_BufferLock );
  }

static inline
void
BMM_RDG_BlockingSync( int aTimeStep )
  {
  #ifdef RDG_TO_DEVNULL
    return;
  #endif
  BegLogLine( PKFXLOG_BMM_RDG )
    << "BMM_RDG_BlockingSync() Entered "
    << EndLogLine;
  BMM_RDG_Flush( aTimeStep );
  }

inline
void
SendDatagramToMonitor(int fd, char *aBuffer, int aLen )
  {
  #ifdef RDG_TO_DEVNULL
    return;
  #endif

  if( aLen >= 256 )
    PLATFORM_ABORT("SendDatagramToMonitor() : aLen > 256 ");

  BegLogLine( 0 & PKFXLOG_BMM_RDG )
    << "SendDatagramToMonitor"
    << " LocalBufferIndex "
    << BMM_RDG_LocalBufferIndex
    << " Src Buf @"
    << (void *) aBuffer
    << " Len "
    << aLen
    << " First 4 words: "
    << (void *) (((int *)aBuffer)[0])
    << " "
    << (void *) (((int *)aBuffer)[1])
    << " "
    << (void *) (((int *)aBuffer)[2])
    << " "
    << (void *) (((int *)aBuffer)[3])
    << EndLogLine;

  Platform::Mutex::YieldLock( &BMM_RDG_BufferLock );

  // Have to abort if buffer is full because there is no way to trigger node zero to do io phase.
  if( (BMM_RDG_LocalBufferIndex + aLen) >= BMM_RDG_NodeBufferSizeInBytes )
    PLATFORM_ABORT(" No room for new datagram in buffer - define larger buffer ");

  memcpy( &( BMM_RDG_AllBufPtrs[ BMM_RDG_LocalBufferIndex ] ), aBuffer, aLen );

  BMM_RDG_LocalBufferIndex += aLen;

  Platform::Mutex::Unlock( &BMM_RDG_BufferLock );
  }


#endif
