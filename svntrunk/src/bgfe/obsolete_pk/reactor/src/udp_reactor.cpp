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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          mpi_reactor.cpp
 *
 * Purpose:         active packet driver based on mpi
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         020201 BGF Createed from reactor.cpp ... 010297 BGF Created.
 *
 ***************************************************************************/

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#include "buffer.cpp"

#ifndef PKTRACE_REACTOR
#define PKTRACE_REACTOR (0)
#endif


#ifndef PKFXLOG_REACTOR
#define PKFXLOG_REACTOR (0)
#endif

//***************************************



char BCAST_ADDR[]      = "224.0.0.1";
int OUR_PORT           = 8887;


#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <stream.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pk/platform.hpp>
#include <pk/ThreadCreate.hpp>


// It's a little backwards that this file gets this info from the platform header file.
enum { PK_UDP_PACKET_PAYLOAD_SIZE = Platform::Reactor::BROADCAST_PAYLOAD_SIZE + 32 };

typedef double ReactorPacketAlignmentType;

// Note: we use MPI Task id's for now, but would could/should map back from UDP's knowledge of the sender's IP address.


typedef unsigned short MpiTaskId;
typedef unsigned short SeqNoType;


// Receive window needs to be a power of 2
const SeqNoType RECEIVE_WINDOW_MASK = 0x000F ;
const SeqNoType RECEIVE_WINDOW_SIZE = RECEIVE_WINDOW_MASK + 1 ;

struct UdpReactorPacket
  {
  struct tHeader
    {
    Platform::Reactor::FunctionPointerType    mFxPtr;
    SeqNoType                                 mSeqNo;
    MpiTaskId                                 mSourceTaskId;
    } ;
  enum { PAYLOAD_SIZE = ( PK_UDP_PACKET_PAYLOAD_SIZE - sizeof( tHeader )  ) };
  enum { PAYLOAD_ARRAY_COUNT = PAYLOAD_SIZE /  (sizeof(ReactorPacketAlignmentType) ) };
  tHeader Header;
  ReactorPacketAlignmentType mPayload[ PAYLOAD_ARRAY_COUNT ];
  };


Platform::Mutex::MutexWord UdpReactorUp;

void *
ReactorThread( void * )
  {
  BegLogLine(1) << "Server running on " << Platform::Topology::GetAddressSpaceId() << EndLogLine;
  int                     sockfd;
  struct sockaddr_in      servaddr;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(OUR_PORT);

  int bindrc = bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  if( bindrc < 0 )
    {
    BegLogLine(1)
      << "ReactorThread bind() failed. rc = "
      << bindrc
      << " errno "
      << errno
      << EndLogLine;
    PLATFORM_ABORT( "ReactorThread() Bind failed" );
    }

  int             n;
  socklen_t       len;

  n = 1024 * 1024;
  int setsockoptrc = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));
  if( setsockoptrc != 0 )
    {
    BegLogLine(1)
      << "ReactorThread setsockopt() to set rcv buffers failed. rc = "
      << setsockoptrc
      << " Buffer request "
      << n
      << "kb "
      << " errno "
      << errno
      << EndLogLine;
    PLATFORM_ABORT( "ReactorThread() setsockopt failed" );
    }

  enum { MaxUdpNodes = 256 };
  SeqNoType HighestSeqNo[ MaxUdpNodes ];
  for( int i = 0; i < MaxUdpNodes; i++ )
    HighestSeqNo[ i ] = 0;

  // Prime windowing
  SeqNoType RecvWindow[ MaxUdpNodes ][ RECEIVE_WINDOW_SIZE ];
  for( int n = 0; n < MaxUdpNodes; n++ )
    for( int i = 0; i < RECEIVE_WINDOW_SIZE; i++ )
      RecvWindow[ n ][ i ] = i ;

  struct sockaddr     cliaddr;
  len = sizeof( cliaddr );

  // NEED TO HOOK IN UNDERSTANDING ABOUT PACKET SIZE
  UdpReactorPacket * urppkt = (UdpReactorPacket *) Platform::Reactor::GetBuffer();

  // Unlocking this mutex signals that we are ready to run
  Platform::Mutex::Unlock( UdpReactorUp );

  for ( ; ; )
    {
    n = recvfrom(sockfd, (void *) urppkt, sizeof( UdpReactorPacket ), 0, &cliaddr, &len);

    int PacketSource       = urppkt->Header.mSourceTaskId;
    SeqNoType PacketSeqNo  =  urppkt->Header.mSeqNo;

    if( PacketSeqNo != HighestSeqNo[ PacketSource ]  )
      {

      BegLogLine(0)
        << "OUT OF ORDER - got "
        << PacketSource
        << ":"
        << PacketSeqNo
        << " Expected "
        << PacketSource
        << ":"
        <<  HighestSeqNo[ PacketSource ]
        << EndLogLine;
      /// PLATFORM_ABORT( "udp_reactor::ReactorThread(): LOST BROADCAST PACKET " )
      }

    if( PacketSeqNo >= HighestSeqNo[ PacketSource ]  )
      HighestSeqNo[ PacketSource ] = PacketSeqNo + 1;

    int windex = PacketSeqNo & RECEIVE_WINDOW_MASK;

    if( RecvWindow[ PacketSource ][ windex ] != PacketSeqNo  )
      {
      BegLogLine(1)
        << "LOST A PACKET "
        << " windex "
        << windex
        << " PacketSource "
        << PacketSource
        << " RecvWindow[S][w] "
        <<  RecvWindow[ PacketSource ][ windex ]
        << " PacketSeqNo "
        << PacketSeqNo
        << " WINDOW IS "
        << RECEIVE_WINDOW_SIZE
        << EndLogLine;
      PLATFORM_ABORT( "udp_reactor::ReactorThread(): LOST BROADCAST PACKET " )
      }

    RecvWindow[ PacketSource ][ windex ] = PacketSeqNo + RECEIVE_WINDOW_SIZE;

    int ReactorRc = urppkt->Header.mFxPtr( (void *) urppkt->mPayload );

    switch( ReactorRc )
      {
      case 0 :
        break;
      case 1 :
        // Replace packet buffer since the last one was kept by the application
        urppkt = (UdpReactorPacket *) Platform::Reactor::GetBuffer();
        break;
      default:
        assert( 0 );
      };


    BegLogLine(0)
      << "Server " << Platform::Topology::GetAddressSpaceId()
      << " Recieved task:seqno "
      << PacketSource
      << ":"
      << PacketSeqNo
      << EndLogLine;

    }
  }


int              bcast_sockfd;
struct sockaddr *bcast_servaddr;
socklen_t        bcast_servlen;

void
reactorInitialize()
  {
  BegLogLine(1)
    << "UdpReactorInitialize "
    << "Starting "
    << EndLogLine;
  static struct sockaddr_in      servaddr;

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(OUR_PORT);

  int rc = inet_pton(AF_INET,  BCAST_ADDR, &servaddr.sin_addr);
  assert( rc > 0 );

  bcast_sockfd   = socket(AF_INET, SOCK_DGRAM, 0);
  bcast_servaddr = (struct sockaddr *) &servaddr;
  bcast_servlen  = sizeof( servaddr );

  int                     n;
  const int               on = 1;

  setsockopt(bcast_sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

  BegLogLine(1)
    << "Udp bcast packet max size "
    << sizeof( UdpReactorPacket )
    << " UdpReactorPacket::PAYLOAD_ARRAY_COUNT "
    << UdpReactorPacket::PAYLOAD_ARRAY_COUNT
    << " UdpReactorPacket::PAYLOAD_SIZE "
    << UdpReactorPacket::PAYLOAD_SIZE
    << " sizeof( ReactorPacketAlignmentType ) "
    << sizeof( ReactorPacketAlignmentType )
    << EndLogLine;

  BegLogLine(1)
    << "UdpReactorInitialize "
    << "Finished "
    << EndLogLine;
  }


SeqNoType BroadcastSenderSeqNo = 0;

int
Platform::Reactor::Trigger( Platform::Reactor::FunctionPointerType    aFxPtr,
          void                                     *aMsg,
          unsigned                                 aMsgLen)
  {

  UdpReactorPacket urppkt;
///  bzero( &urppkt, sizeof( urppkt ) );

  // THIS MUST BE MADE THREAD SAFE
  urppkt.Header.mSeqNo = BroadcastSenderSeqNo;
  BroadcastSenderSeqNo++;

  urppkt.Header.mSourceTaskId =  Platform::Topology::GetAddressSpaceId();

  urppkt.Header.mFxPtr = aFxPtr;

  if( aMsgLen > Platform::Reactor::BROADCAST_PAYLOAD_SIZE )
    {
    BegLogLine(1)
      <<  "Trigger() Bcast "
      << " aMsgLen to long "
      << aMsgLen
      << " Platform::Reactor::BROADCAST_PAYLOAD_SIZE "
      << Platform::Reactor::BROADCAST_PAYLOAD_SIZE
      << EndLogLine;
    PLATFORM_ABORT( "BCast Trigger aMsgLen > Platform::Reactor::BROADCAST_PAYLOAD_SIZE" );
    }

  memcpy( (void *)(urppkt.mPayload), aMsg, aMsgLen );

  int urplen = sizeof( urppkt.Header ) + aMsgLen;

  BegLogLine( 0 )
        <<  "Trigger() Bcast "
        << "SeqNo "
        << urppkt.Header.mSeqNo
        <<  " Reactor @"
        <<  (void *) urppkt.Header.mFxPtr
        <<  " TMsg Len "
        <<  urplen
        <<  " Msg data: "
        << urppkt.mPayload[0]
        << EndLogLine;

  sendto(bcast_sockfd, &urppkt, urplen, 0, bcast_servaddr, bcast_servlen);

  }

//******************************************


MPI_Comm REACTIVE_MESSAGE_COMM_WORLD;

BufferPtr      *AllBuf     ;    // A pointer to an array of pointers.
MPI_Request    *AllRequests;
MPI_Status     *AllStatus  ;



#ifndef PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT
#define PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT (64)  // 64 * (say)32KB == 2MB commited.
#endif

#define TOTAL_DATAGRAMS_IN_FLIGHT \
        (PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT * Platform::Topology::GetAddressSpaceCount())

//-----------------------------------------------------------------------------
// This constructor is called during the init of the
// pk. It can be assumed to be running in all tasks.

void
ReactorInitialize()
  {
  BegLogLine( PKFXLOG_REACTOR )
    << "ReactorInitialize() in Task ["
    << Platform::Topology::GetAddressSpaceId() << "]"
    << EndLogLine;

  for( int bpi = 0; bpi < FREE_BUFFER_POOL_SIZE; bpi++ )   //Initialise buffer pool
    FreeBufferPool[bpi] = NULL;
  FreeBufferPoolCount    = 0;
  FreeBufferPoolLockWord = 0;

  AllBuf = new BufferPtr[TOTAL_DATAGRAMS_IN_FLIGHT ];  //Initialise an array of buffers
  assert( AllBuf != NULL );                             // based on the maximum number of
                                                        // packets being sent at once.
  for( int i = 0; i < (TOTAL_DATAGRAMS_IN_FLIGHT); i++ )
    {
    AllBuf[i] = (BufferPtr) Platform::Reactor::GetBuffer();
    }
  AllRequests = new MPI_Request [ TOTAL_DATAGRAMS_IN_FLIGHT ];  //
  AllStatus   = new MPI_Status  [ TOTAL_DATAGRAMS_IN_FLIGHT ];  //

  MPI_Group REACTIVE_MESSAGE_GROUP;

  MPI_Comm_group( MPI_COMM_WORLD, &REACTIVE_MESSAGE_GROUP); //Make REACTIVE_MESSAGE_GROUP
                                                                   //a group with all processes.

  MPI_Comm_create( MPI_COMM_WORLD,
                          REACTIVE_MESSAGE_GROUP,        //Create a new communicator called
                         &REACTIVE_MESSAGE_COMM_WORLD);  // REACTIVE_MESSAGE_COMM_WORLD that includes
                                                         // all processes in REACTIVE_MESSAGE_GROUP.
  int t;
  for( t = 0; t<TOTAL_DATAGRAMS_IN_FLIGHT;t++)
    {
    int msgrc;

    msgrc = MPI_Irecv( (void *) AllBuf[t],             //Post non-blocking receive for
                              sizeof( Buffer ),               // each request.
                              MPI_CHAR,
                              MPI_ANY_SOURCE,
                              MPI_ANY_TAG,
                              REACTIVE_MESSAGE_COMM_WORLD,
                             &AllRequests[t]);
    }

  for( ; t < TOTAL_DATAGRAMS_IN_FLIGHT; t++ )                 //Initialise requests to NULL
    {
    AllRequests[t] = MPI_REQUEST_NULL;
    }

  //NEED: might want to ponder this here even in MPI world.
  MPI_Barrier( REACTIVE_MESSAGE_COMM_WORLD );         //Wait for all processes to get here.

  // Do the udp bcast driver now
  reactorInitialize();

  MPI_Barrier( REACTIVE_MESSAGE_COMM_WORLD );         //Wait for all processes to get here.

  }

//-----------------------------------------------------------------------------

int
Platform::Reactor::
Trigger( int             TaskNo,
         Platform::Reactor::FunctionPointerType    FxPtr,
         void           *Msg,
         unsigned        MsgLen)
  {
  assert( MsgLen <= Platform::Reactor::PAYLOAD_SIZE );

  int mpirc;

  assert( MsgLen <= sizeof( Buffer ) );

  BegLogLine( PKFXLOG_REACTOR )
        <<  "Trigger() Before Send ActiveMsg To Task "
        <<  TaskNo
        <<  " Reactor @"
        <<  (unsigned) FxPtr
        <<  " TMsg Len "
        <<  MsgLen
        <<  " Msg data: "
        <<  (void *) (*((int*)Msg))
        <<  EndLogLine;

  mpirc = MPI_Send(Msg,
                   MsgLen,
                   MPI_CHAR,                              // Send data and function pointer to
                   TaskNo,                                // specified task.
                   (int) FxPtr,
                   REACTIVE_MESSAGE_COMM_WORLD );

  assert( mpirc == 0 );

  BegLogLine( PKFXLOG_REACTOR )
        <<  "Trigger() After Send ActiveMsg To Task "
        <<  Platform::Topology::GetAddressSpaceId()
        <<  " Reactor @"
        <<  (unsigned) FxPtr
        <<  " TMsg Len "
        <<  MsgLen
        <<  " Msg data: "
        <<  (void *) (*((int*)Msg))
        << EndLogLine;

  if( mpirc != 0 )
    {
    MPI_Abort( REACTIVE_MESSAGE_COMM_WORLD,
                      mpirc );
    }

  return( mpirc );
  }

extern "C" { void trcon(int) ; void trcoff(int) ; } ;


//-----------------------------------------------------------------------------

///int adummyarg;

void*
ReactorThreadMain( void *arg )     //This goes round and round and round...........
  {


//  if( ! Platform::Mutex::TryLock( UdpReactorUp ) )
//    PLATFORM_ABORT( "Unable to get UdpReactorUp lock" );

///  Platform::Thread::Create( ReactorThread, NULL );
///  ThreadCreate( -1, ReactorThread, 0, NULL );

//  Platform::Mutex::YieldLock( UdpReactorUp );


  int mpirc;
  int ReactorRc;

  MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.

  BegLogLine( PKFXLOG_REACTOR )
        << " PhysicalThread ... Running ..."
        << "ActiveMsg Processor: "
        << EndLogLine;


  // Continuous loop
  for(int d = 0; (1) ; d++)
    {
    unsigned msgrc;
    MPI_Status Status;                     //Always do this
    int ReqIndex;

    BegLogLine( PKFXLOG_REACTOR )
      << "ActiveMsg Processor: "
      << " Testany()/usleep() for incomming "
      << EndLogLine;
    int Flag = 0;

//NEED to encapsulate this function as a yield function.
    for( int PollCount = 0; Flag == 0; PollCount++ )   //Loop until a request is complete
      {
      mpirc = MPI_Testany(TOTAL_DATAGRAMS_IN_FLIGHT, AllRequests, &ReqIndex, &Flag, &Status);
      assert( mpirc == 0);
      if( ! Flag )
        {
        if( PollCount < (64 * 1024) )  // the reactor should be the last to sleep
          Platform::Thread::Yield();
        else
          usleep( 10000 );             // and sleep less long.
        }
      }

    int Len;

    MPI_Get_count( &Status, MPI_CHAR, &Len );//Use Status retuurned above to get message length.

    BegLogLine( PKFXLOG_REACTOR )
           << "ActiveMsg Processor: Got one! "
           << " Before call Fx @"
           <<  MPI_STATUS_TAG( Status )
           << " Src "
           <<  MPI_STATUS_SOURCE( Status )
           <<  " Len "
           <<  Len
           <<  " Data "
           <<  AllBuf[ReqIndex]
           << EndLogLine;


    // THIS IS THE POINT WHERE ACTIVE PACKET FUNCTIONS ARE EXECUTED
    ReactorRc = ((Platform::Reactor::FunctionPointerType)((MPI_STATUS_TAG( Status ))))( AllBuf[ReqIndex] );

    switch( ReactorRc )
      {
      case 0 :
        break;
      case 1 :
        AllBuf[ReqIndex] = Platform::Reactor::GetBuffer();       //buffer so we make a new one.
        break;
      default:
        assert( 0 );
      };

    BegLogLine( PKFXLOG_REACTOR )
      << "ActiveMsg Processor: Did one! "
      << "After call Fx() @"
      <<  (MPI_STATUS_TAG( Status ))
      << " Src "
      <<  MPI_STATUS_SOURCE( Status )
      <<  " Len "
      <<  Len
      <<  " Data  "
      <<  AllBuf[ReqIndex]
      << EndLogLine;


    msgrc = MPI_Irecv( (void *) AllBuf[ReqIndex],      //Post a non-blocking receive on the
                              sizeof( Buffer ),               //buffer we just used.
                              MPI_CHAR,
                              MPI_ANY_SOURCE,
                              MPI_ANY_TAG,
                              REACTIVE_MESSAGE_COMM_WORLD,
                             &AllRequests[ReqIndex] );

    assert( msgrc == 0 );

    if( msgrc != 0 )
      {
      //Pprintf("Reactive Message Subsystem: MPI_Irecv failed \n");
      MPI_Abort( MPI_COMM_WORLD, -1);
      }
    }

  MPI_Finalize();
  _exit(0);
  return( NULL ); // Keep compiler quiet
  }


