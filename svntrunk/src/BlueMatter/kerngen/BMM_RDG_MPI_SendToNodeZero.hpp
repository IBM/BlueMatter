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
#define PKFXLOG_BMM_RDG  ( 0 )
#endif

#ifndef BMM_RDG_SplitFileSize
#define BMM_RDG_SplitFileSize (1024)
#endif

#ifndef BMM_RDG_SplitFileSizeInBytes
#define BMM_RDG_SplitFileSizeInBytes  ((BMM_RDG_SplitFileSize) * (1024*1024))
#endif


#ifndef BMM_RDG_NodeBufferSizeInBytes
#define BMM_RDG_NodeBufferSizeInBytes ( 8 * 1024 * 1024 )  // 6MB is about enough for 100k sites on one node.
#endif

#ifndef BMM_RDG_MaxMpiTransferSize
#define BMM_RDG_MaxMpiTransferSize    ( 32 * 1024 ) // Max bytes per mpi transfer to node zero
#endif

#ifndef BMM_RDG_NodeZeroBufferCount
#define BMM_RDG_NodeZeroBufferCount   ( 32 ) // Number of transfer sized buffers posted
#endif

#define BMM_RDG_Split
#ifdef BMM_RDG_Split
static char * BMM_RDG_FileBaseName = "RDG";
#else
static char * BMM_RDG_FileBaseName = "RawDatagramOutput";
#endif

extern int BMM_RDG_Fd ;
extern unsigned int BMM_RDG_Extension ;
#if !defined(LEAVE_SYMS_EXTERN)
int BMM_RDG_Fd = -1;

unsigned int BMM_RDG_Extension = 0;
#endif

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


  #if MSDDEF_ReplicaExchange

    sprintf( PathName,
             "%s.%04d.%04d.%04d.%04d.%04d.srdg",
             BMM_RDG_FileBaseName,
             Platform::Topology::mSubPartitionId,
             BMM_RDG_Extension,
             Platform::Topology::GetAddressSpaceId(),
             Platform::Topology::GetAddressSpaceCount(),
             BMM_RDG_SplitFileSize
             );

    #else
    sprintf( PathName,
             "%s.%04d.%04d.%04d.%04d.srdg",
             BMM_RDG_FileBaseName,
             BMM_RDG_Extension,
             Platform::Topology::GetAddressSpaceId(),
             Platform::Topology::GetAddressSpaceCount(),
             BMM_RDG_SplitFileSize
             );

    #endif

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


extern unsigned int BMM_RDG_TotalBytesWrittenToFile ;
#if !defined(LEAVE_SYMS_EXTERN)
unsigned int BMM_RDG_TotalBytesWrittenToFile = 0;
#endif


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

static TraceClient RdgWriteStart;
static TraceClient RdgWriteFinis;


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
    RdgWriteStart.HitOE( PKTRACE_RDG_IO,
                         "RdgWrite",
                         0,
                         RdgWriteStart );
    
    int WriteRc = ::write( aRdgFd, &(aBuffer[ BytesLeftOffset ]), BytesLeft );
    
    RdgWriteFinis.HitOE( PKTRACE_RDG_IO,
                         "RdgWrite",
                         0,
                         RdgWriteFinis );

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
    BegLogLine( 1 )
      << "Trouble writing RDG CLEARED."
      << EndLogLine;
    }
  return;
  }

static int                BlueMatterMonitor_Fd     = -1;
static int                BlueMatterMonitor_Socket = 0;

static int    BMM_RDG_LocalBufferIndex = 0 ;
static char * BMM_RDG_LocalBuffer      = NULL ;

static Platform::Mutex::MutexWord BMM_RDG_BufferLock;
static Platform::Mutex::MutexWord* BMM_RDG_BufferLockPtr;

static TraceClient DoTransportStart;
static TraceClient DoTransportFinis;


  typedef char *        BMM_RDG_BufferPtrType;
static  BMM_RDG_BufferPtrType BMM_RDG_AllBufPtrs  [ BMM_RDG_NodeZeroBufferCount ];

#ifdef PK_MPI_ALL
static  MPI_Request           BMM_RDG_AllRequests [ BMM_RDG_NodeZeroBufferCount ];
static  MPI_Status            BMM_RDG_AllStatus   [ BMM_RDG_NodeZeroBufferCount ];

static  MPI_Comm BMM_RDG_MESSAGE_COMM_WORLD;
#endif

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

  if( Platform::Topology::GetAddressSpaceId() == 0 )
    {
    // Open the file without closing a previous file.
    BMM_RDG_OpenOrReopenRDGFile( 1 );

    if( BMM_RDG_Fd < 0 )
      PLATFORM_ABORT( "Failed to open rdg file");
    }

  // Note, local buffer is used for both parallel and non parallel imps

  BMM_RDG_LocalBuffer = (char *) malloc( BMM_RDG_NodeBufferSizeInBytes );
  if( BMM_RDG_LocalBuffer == NULL )
    PLATFORM_ABORT(" Failed to allocate BMM_RDG LocalBuffer of size BMM_RDG_NodeBufferSizeInBytes " );

  BMM_RDG_LocalBufferIndex = 0;


  #ifdef PK_MPI_ALL
    MPI_Group BMM_RDG_MESSAGE_GROUP;

    MPI_Comm_group( Platform::Topology::cart_comm, &BMM_RDG_MESSAGE_GROUP); //Make REACTIVE_MESSAGE_GROUP
                                                                       //a group with all processes.
    MPI_Comm_create(  Platform::Topology::cart_comm,
                      BMM_RDG_MESSAGE_GROUP,        //Create a new communicator called
                      &BMM_RDG_MESSAGE_COMM_WORLD);  // REACTIVE_MESSAGE_COMM_WORLD that includes


    for( int n = 0; n < BMM_RDG_NodeZeroBufferCount; n++ )
      {
      BMM_RDG_AllBufPtrs[ n ] = (char *) malloc( BMM_RDG_MaxMpiTransferSize );
      if( BMM_RDG_AllBufPtrs[ n ] == NULL )
        PLATFORM_ABORT(" Failed to allocate BMM_RDG mpi receive buffer of size BMM_RDG_MaxMpiTransferSize" );
      }

    for( int n = 0; n < BMM_RDG_NodeZeroBufferCount; n++ )
      {
      BMM_RDG_AllRequests[ n ] = MPI_REQUEST_NULL;
      }

    // Post all receive buffers, including the local buffer
    for( int t = 0;
             t <  BMM_RDG_NodeZeroBufferCount ;
             t ++ )
      {
      int msgrc;

      msgrc = MPI_Irecv( (void *) BMM_RDG_AllBufPtrs[ t ],    //Post non-blocking receive for
                         BMM_RDG_MaxMpiTransferSize,
                         MPI_CHAR,
                         MPI_ANY_SOURCE,
                         MPI_ANY_TAG,
                         BMM_RDG_MESSAGE_COMM_WORLD,
                        &BMM_RDG_AllRequests[ t ]);

      if( msgrc != 0 )
        PLATFORM_ABORT(" MPI RC != 0 ON RDG BUFFER SEND " );
      }
  #endif // #endif PK_MPI_ALL

#ifdef PK_BGL
    #ifdef PK_BLADE_SPI
    // 24 is a safe lock for a blade lockbox
      BMM_RDG_BufferLock = 24;
      BMM_RDG_BufferLockPtr = &BMM_RDG_BufferLock;
    #else
      BMM_RDG_BufferLockPtr = rts_allocate_mutex();

      assert( BMM_RDG_BufferLockPtr );

      BegLogLine( 0 )
        << "BMM_RDG_BufferLockPtr: " << (void *) BMM_RDG_BufferLockPtr
        << " BGL_Mutex_Is_Locked( ptr ): " << BGL_Mutex_Is_Locked( BMM_RDG_BufferLockPtr )
        << EndLogLine;

    #endif
#else
      BMM_RDG_BufferLock = 0;
      BMM_RDG_BufferLockPtr = &BMM_RDG_BufferLock;
#endif

  // Platform::Mutex::Unlock( BMM_RDG_BufferLockPtr );
  return( 0 );
  }

#ifndef BMM_RDG_FlushPeriod
#define BMM_RDG_FlushPeriod 100
#endif

// Don't allow more than 128 ts between flush's to avoid massive skew in rdg file
#if BMM_RDG_FlushPeriod > 127
#define BMM_RDG_FlushPeriod 127
#endif

inline
void
BMM_RDG_Flush( int aTimeStep )
  {
  #ifdef RDG_TO_DEVNULL
    return;
  #endif

  if( aTimeStep % BMM_RDG_FlushPeriod != 0 )
    return ;

  DoTransportStart.HitOE( PKTRACE_RDG_IO,
                          "RdgFlush",
                          0,
                          DoTransportStart );

  EnergyManagerIF.ReduceAndEmitEnergies( aTimeStep );

  BegLogLine( PKFXLOG_BMM_RDG )
    << "BMM_RDG_Flush() Entered "
    << " TimeStep "
    << aTimeStep
    << EndLogLine;

  int mpirc = 0;

  Platform::Mutex::YieldLock( BMM_RDG_BufferLockPtr );

  int PostedReceiveBuffers = 0;

  if( Platform::Topology::GetAddressSpaceId() != 0 )
    {
    #ifdef PK_MPI_ALL

      int ExtraMessagesToSend =  0;

      BegLogLine( PKFXLOG_BMM_RDG )
        << "Sending LocalNodeBuffer to zero. "
        << "Len "
        << " BMM_RDG_LocalBufferIndex "
        << BMM_RDG_LocalBufferIndex
        << EndLogLine;


      // Need to check if we're going to use more than one transfer to node 0
      int NextSendIndex = 0;
      while( (BMM_RDG_LocalBufferIndex - NextSendIndex) > BMM_RDG_MaxMpiTransferSize )
        {
        // scan for an end index so we can transfer whole packets
        int NextSendLen = 0 ;
        while( 1 )
          {
          // length of an RDG packet is in first byte
          int RdgPktLen = BMM_RDG_LocalBuffer[ NextSendIndex + NextSendLen ];

          // if next packet would go beyond a single transfer buffer, break
          if( ( NextSendLen + RdgPktLen ) > BMM_RDG_MaxMpiTransferSize )
            break;

          NextSendLen += RdgPktLen;
          }

        mpirc = MPI_Send( & ( BMM_RDG_LocalBuffer[ NextSendIndex ] ),
                              NextSendLen,
                              MPI_CHAR,                                // Send data and function pointer to
                              0,                                       // specified task.
                              0, // mpi type == 0 means that this is not the 'key' message from this task
                              BMM_RDG_MESSAGE_COMM_WORLD );

        if( mpirc != 0 )
          PLATFORM_ABORT(" MPI RC != 0 ON RDG BUFFER SEND " );

        // advance the start point for the next send.
        NextSendIndex       += NextSendLen ;
        ExtraMessagesToSend ++ ;
        }

      // Send the key message - tag is positive and is the number of other transfer units sent above
      mpirc = MPI_Send( & ( BMM_RDG_LocalBuffer[ NextSendIndex ] ),
                        BMM_RDG_LocalBufferIndex - NextSendIndex,
                        MPI_CHAR,                              // Send data and function pointer to
                        0,                                     // specified task.
                        1 + ExtraMessagesToSend,     // this is the key packet - tell how many packets will be sent from this node
                        BMM_RDG_MESSAGE_COMM_WORLD );

      BMM_RDG_LocalBufferIndex = 0;

      if( mpirc != 0 )
        PLATFORM_ABORT(" MPI RC != 0 ON RDG BUFFER SEND " );
    #endif
    }
  else // NODE ZERO (O) ACTIVIES HANDLED IN THIS BLOCK
    {
    // Write node zero's data
    BegLogLine( PKFXLOG_BMM_RDG )
      << "Writing node zero data to file"
      << " Len "
      <<  BMM_RDG_LocalBufferIndex
      << EndLogLine;

    // Node 0's data will go out with the first required write
    /////BMM_RDG_Write( BMM_RDG_Fd, BMM_RDG_AllBufPtrs[ 0 ], BMM_RDG_LocalBufferIndex );
    /////BMM_RDG_LocalBufferIndex = 0;

    #ifdef PK_MPI_ALL
      int KeyMessagesReceived = 0;
      int KeyMessagesExpected = Platform::Topology::GetAddressSpaceCount() - 1;

      int TotalMessagesExpected = KeyMessagesExpected;
      int TotalMessagesReceived = 0;

      // loop until receiving at least one message (key) from each node and then until total messsage count is achieved
      while(    KeyMessagesReceived   < KeyMessagesExpected
             || TotalMessagesReceived < TotalMessagesExpected )
        {
        BegLogLine( PKFXLOG_BMM_RDG )
          << "Top of receive loop "
          << "  KeyMessagesReceived "
          <<  KeyMessagesReceived
          << " KeyMessagesExpected "
          << KeyMessagesExpected
          << " TotalMessagesReceived "
          << TotalMessagesReceived
          << " TotalMessagesExpected "
          << TotalMessagesExpected
          << EndLogLine;

        int ReqIndex = 0;

        MPI_Status Status;

        BegLogLine( PKFXLOG_BMM_RDG )
          << "Calling MPI_Waitany "
          << EndLogLine;

        mpirc = MPI_Waitany(  BMM_RDG_NodeZeroBufferCount, // note - we pass whole array of requests even though some of them may not be outstanding
                              BMM_RDG_AllRequests,
                             &ReqIndex,
                             &Status );

        if( mpirc != 0 )
          PLATFORM_ABORT(" MPI RC != 0 ON WAITANY " );

        if( ReqIndex == MPI_UNDEFINED )
          PLATFORM_ABORT("MPI_Waitany returned ReqIndex == MPI_UNDEFINED");

        BMM_RDG_AllRequests[ ReqIndex ] = MPI_REQUEST_NULL;

        int MsgLen;

        MPI_Get_count( &Status, MPI_CHAR, &MsgLen );  //Use Status retuurned above to get message length.


        BegLogLine( PKFXLOG_BMM_RDG )
          << "Return from MPI_Waitany "
          << " ReqIndex "
          << ReqIndex
          << " MsgLen "
          << MsgLen
          << " Status.MPI_TAG "
          << Status.MPI_TAG
          << EndLogLine;

        if( Status.MPI_TAG > 0 )
          {
          int ExtraMessageCount  = Status.MPI_TAG - 1 ;
          TotalMessagesExpected += ExtraMessageCount ;
          KeyMessagesReceived ++ ;
          }

        TotalMessagesReceived ++ ;

        // If the incomming data would overflow the buffer, do a write to clear space
        if( (BMM_RDG_LocalBufferIndex + MsgLen) >= BMM_RDG_NodeBufferSizeInBytes )
          {
          BMM_RDG_Write( BMM_RDG_Fd,
                         BMM_RDG_LocalBuffer,
                         BMM_RDG_LocalBufferIndex );
          BMM_RDG_LocalBufferIndex = 0;
          }

        memcpy( & (BMM_RDG_LocalBuffer[ BMM_RDG_LocalBufferIndex ] ),
                BMM_RDG_AllBufPtrs[ ReqIndex ],
                MsgLen );

        BMM_RDG_LocalBufferIndex += MsgLen ;


        BegLogLine( PKFXLOG_BMM_RDG )
          << "Reposting buffer "
          << " ReqIndex "
          << ReqIndex
          << EndLogLine;

        mpirc = MPI_Irecv( (void *) BMM_RDG_AllBufPtrs[ ReqIndex ],  //Post a non-blocking receive on the
                           BMM_RDG_MaxMpiTransferSize,        //buffer we just used.
                           MPI_CHAR,
                           MPI_ANY_SOURCE,
                           MPI_ANY_TAG,
                           BMM_RDG_MESSAGE_COMM_WORLD,
                          &BMM_RDG_AllRequests[ ReqIndex ] );


        if( mpirc != 0 )
          PLATFORM_ABORT(" Repost of BMM_RDG receive buffer failed ");

        }

     // Write out what ever has been accumulated into the local buffer
     if( BMM_RDG_LocalBufferIndex )
       {
       BMM_RDG_Write( BMM_RDG_Fd,
                      BMM_RDG_LocalBuffer,
                      BMM_RDG_LocalBufferIndex );
       BMM_RDG_LocalBufferIndex = 0;
       }
    #else
     if( BMM_RDG_LocalBufferIndex )
       {
       BMM_RDG_Write( BMM_RDG_Fd,
                      BMM_RDG_LocalBuffer,
                      BMM_RDG_LocalBufferIndex );
       BMM_RDG_LocalBufferIndex = 0;
       }
    #endif  // #endif PK_MPI_ALL block

    // Node 0 fsyncs the file to get some smoothness in io stream
    // Mainly for steaming apps.
    //fsync( BMM_RDG_Fd );
    }

  DoTransportFinis.HitOE( PKTRACE_RDG_IO,
                          "RdgFlush",
                          0,
                          DoTransportFinis );

  // Since this all introduces a lot of skew, barrier here
  BegLogLine( PKFXLOG_BMM_RDG )
    << "Entering BMM_RDG Write Barrier"
    << " PostedReceiveBuffers "
    << PostedReceiveBuffers
    << EndLogLine;


  Platform::Control::Barrier();

  BegLogLine( PKFXLOG_BMM_RDG )
    << "Exiting BMM_RDG Write Barrier"
    << EndLogLine;

  Platform::Mutex::Unlock( BMM_RDG_BufferLockPtr );
  }

inline
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

  // BegLogLine( 0 & PKFXLOG_BMM_RDG )
  BegLogLine( 0 )
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


//   BegLogLine( 0 )
//     << "BMM_RDG_BufferLockPtr: " << (void *) BMM_RDG_BufferLockPtr
//     << " BGL_Mutex_Is_Locked( ptr ): " << BGL_Mutex_Is_Locked( BMM_RDG_BufferLockPtr )
//     << EndLogLine;

  Platform::Mutex::YieldLock( BMM_RDG_BufferLockPtr );

  // Have to abort if buffer is full because there is no way to trigger node zero to do io phase.
  if( (BMM_RDG_LocalBufferIndex + aLen) >= BMM_RDG_NodeBufferSizeInBytes )
    PLATFORM_ABORT(" No room for new datagram in buffer - define larger buffer ");

  memcpy( &( BMM_RDG_LocalBuffer[ BMM_RDG_LocalBufferIndex ] ), aBuffer, aLen );

  BMM_RDG_LocalBufferIndex += aLen;

  Platform::Mutex::Unlock( BMM_RDG_BufferLockPtr );

  BegLogLine( 0 )
    << "Done SendDatagramToMonitor" 
    << EndLogLine;
  }


#endif
