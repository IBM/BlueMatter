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
 
typedef unsigned int SeqNoType;

enum { RECEIVE_WINDOW_SIZE = 10 };

char BCAST_ADDR[]      = "224.0.0.1";
int OUR_PORT           = 8887;

enum { OUR_PACKET_PAYLOAD = 750 };

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

typedef double ReactorPacketAlignmentType;

struct UdpReactorPacket
  {
  struct tHeader
    {
    Platform::Reactor::FunctionPointerType    mFxPtr;
    SeqNoType                                 mSeqNo;
    } ;
  enum { PAYLOAD_ARRAY_COUNT = ( OUR_PACKET_PAYLOAD - sizeof( tHeader ) + ( sizeof(  ReactorPacketAlignmentType ) - 1 ) ) };
  enum { PAYLOAD_SIZE = PAYLOAD_ARRAY_COUNT /  (sizeof(ReactorPacketAlignmentType) ) };
  tHeader Header;
  ReactorPacketAlignmentType mPayload[ PAYLOAD_ARRAY_COUNT ];
  };

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
      << "ReactorThread Bind() failed. rc = "
      << bindrc
      << " errno "
      << errno
      << EndLogLine;
    PLATFORM_ABORT( "ReactorThread() Bind failed" );
    }

  int                     n;
  socklen_t       len;

  n = 1024 * 1024;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

  // Prime windowing
  int RecvWindow[ RECEIVE_WINDOW_SIZE ];
  for( int i = 0; i < RECEIVE_WINDOW_SIZE; i++ )
    RecvWindow[ i ] = i - RECEIVE_WINDOW_SIZE;

  struct sockaddr     cliaddr;
  len = sizeof( cliaddr );

  unsigned int HighestSeqNo = 0;

  // NEED TO HOOK IN UNDERSTANDING ABOUT PACKET SIZE
  UdpReactorPacket * urppkt = (UdpReactorPacket *) Platform::Reactor::GetBuffer();

  for ( ; ; )
    {
    n = recvfrom(sockfd, (void *) urppkt, sizeof( UdpReactorPacket ), 0, &cliaddr, &len);

    if( urppkt->Header.mSeqNo == HighestSeqNo + 1 )
      BegLogLine(1) << "Out of order - got " << urppkt->Header.mSeqNo << " Expected " << HighestSeqNo << EndLogLine;

    HighestSeqNo++;

    int windex = urppkt->Header.mSeqNo % RECEIVE_WINDOW_SIZE;

    if( RecvWindow[ windex ] == urppkt->Header.mSeqNo )
      BegLogLine(1) << "Duplicated " << urppkt->Header.mSeqNo  << EndLogLine;

    if( RecvWindow[ windex ] != urppkt->Header.mSeqNo - RECEIVE_WINDOW_SIZE )
      BegLogLine(1) << "Lost " << urppkt->Header.mSeqNo - RECEIVE_WINDOW_SIZE  << EndLogLine;

    RecvWindow[ windex ] = urppkt->Header.mSeqNo;

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


    BegLogLine(1)
      << "Server " << Platform::Topology::GetAddressSpaceId()
      << " Recieved #"
      << urppkt->Header.mSeqNo
      << EndLogLine;

    }
  }


int              bcast_sockfd;
struct sockaddr *bcast_servaddr;
socklen_t        bcast_servlen;

void
reactorInitialize()
  {
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
  }


unsigned int BroadcastSenderSeqNo = 0;

int
Trigger( Platform::Reactor::FunctionPointerType    aFxPtr,
          void                                     *aMsg,
          unsigned                                 aMsgLen)
  {

  UdpReactorPacket urppkt;
///  bzero( &urppkt, sizeof( urppkt ) );

  // THIS MUST BE MADE THREAD SAFE
  urppkt.Header.mSeqNo = BroadcastSenderSeqNo;
  BroadcastSenderSeqNo++;

  urppkt.Header.mFxPtr = aFxPtr;

  assert( aMsgLen < UdpReactorPacket::PAYLOAD_SIZE );

  memcpy( (void *)(urppkt.mPayload), aMsg, aMsgLen );

  int urplen = sizeof( urppkt.Header ) + aMsgLen;

  BegLogLine( 1 )
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

  sendto(bcast_sockfd, &urppkt, sizeof( urppkt ), 0, bcast_servaddr, bcast_servlen);

  }


int print_something( void *something )
  {
  BegLogLine(1)
    << "print_something(): "
    << (char *) something
    << EndLogLine;
  return(0);
  }


int
PkMain(int argc, char **argv, char **)
  {
  reactorInitialize();

  int AddressSpaceCount        =  Platform::Topology::GetAddressSpaceCount();
  for( int i = 0; i < AddressSpaceCount; i++ )
    {
    ThreadCreate( i, ReactorThread, 0, NULL );
    }

  sleep( 5 );  // Avoids poorly the race cond


  char *something = "** protocol ";
  char sendCounter[50];
  int counter=0;


  for (int k=0; k <10; k++)
    {
    sleep(1);
    BegLogLine( 0 )
      << "iv00:  "
      << " Send Trigger... "
      << EndLogLine;

   sprintf(sendCounter, "%s %d **", something, counter++);
   Trigger(
           print_something,
           &sendCounter,   // this should proably be a pointer to the wrapper
           1 + strlen( sendCounter ) );
    sleep(1);
    }

  exit(0);
  }


