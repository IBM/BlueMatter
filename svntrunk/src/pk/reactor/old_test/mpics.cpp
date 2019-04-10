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
 //#include        "unp.h"

enum { RECEIVE_WINDOW_SIZE = 10 };

char BCAST_ADDR[]      = "224.0.0.1";
int OUR_PORT           = 8887;

enum { OUR_PACKET_PAYLOAD = 750 };
struct IntPacket
  {
  int SeqNo;
  enum { PayloadCount =  OUR_PACKET_PAYLOAD / sizeof( int )  - 1 };
  int Array[ PayloadCount ];
  };


static void     recvfrom_alarm(int);

#define MAXLINE 80

//#include <assert.h>
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

#define SA      struct sockaddr

class UdpRecvStats
  {
  public:
    unsigned int Received   ;
    unsigned int Duplicated ;
    unsigned int OutOfOrder ;
    unsigned int Lost       ;
    unsigned int DataError  ;

    unsigned int PrevReceived;

    UdpRecvStats()
      {
      Received = 0;
      PrevReceived = 0;
      Zero();
      }

    void Zero()
      {
      DataError  = 0;
      Duplicated = 0;
      OutOfOrder = 0;
      Lost       = 0;
      }

  static
  void *
  ReportThread( void * arg )
    {
    UdpRecvStats *urs = (UdpRecvStats *) arg;
    for( int tu = 0; ; tu++ )
      {
      sleep(1);
      BegLogLine(1)
        << "UdpStats Node "
        << Platform::Topology::GetAddressSpaceId()
        << " tu "
        << tu
        << " Recv "
        << urs->Received
        << " Dup "
        << urs->Duplicated
        << " out "
        << urs->OutOfOrder
        << " lost "
        << urs->Lost
        << " Data Error "
        << urs->DataError
        << " Pkts/s "
        <<  (urs->Received - urs->PrevReceived - urs->Duplicated - urs->Lost )
        << " KB/s "
        << ( (urs->Received - urs->PrevReceived - urs->Duplicated - urs->Lost ) * OUR_PACKET_PAYLOAD) / 1024
        << EndLogLine;
      urs->PrevReceived = urs->Received;
      urs->Zero();
      }
    }

  };

void *
ServerThread( void * )
  {
  BegLogLine(1) << "Server running on " << Platform::Topology::GetAddressSpaceId() << EndLogLine;
  int                     sockfd;
  struct sockaddr_in      servaddr;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(OUR_PORT);

  int bindrc = bind(sockfd, (SA *) &servaddr, sizeof(servaddr));
  if( bindrc < 0 )
    {
    BegLogLine(1)
      << "ServerThread Bind() failed. rc = "
      << bindrc
      << " errno "
      << errno
      << EndLogLine;
    PLATFORM_ABORT( "ServerThread() Bind failed" );
    }

  int                     n;
  socklen_t       len;

  n = 1024 * 1024;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

  IntPacket ipkt;
  UdpRecvStats MyStats;
  Platform::Thread::Create( UdpRecvStats::ReportThread, (void *) &MyStats );

  // Prime windowing
  int RecvWindow[ RECEIVE_WINDOW_SIZE ];
  for( int i = 0; i < RECEIVE_WINDOW_SIZE; i++ )
    RecvWindow[ i ] = i - RECEIVE_WINDOW_SIZE;

  struct sockaddr     cliaddr;
  len = sizeof( cliaddr );

  unsigned int HighestSeqNo = 0;

  for ( ; ; )
    {
    n = recvfrom(sockfd, &ipkt, sizeof( ipkt ), 0, &cliaddr, &len);

    if( ipkt.SeqNo > HighestSeqNo )
      HighestSeqNo = ipkt.SeqNo;

    if( ipkt.SeqNo != HighestSeqNo )
      MyStats.OutOfOrder++;

    int windex = ipkt.SeqNo % RECEIVE_WINDOW_SIZE;

    if( RecvWindow[ windex ] == ipkt.SeqNo )
      MyStats.Duplicated++;

    if( RecvWindow[ windex ] != ipkt.SeqNo - RECEIVE_WINDOW_SIZE )
      MyStats.Lost++;

    RecvWindow[ windex ] = ipkt.SeqNo;

    for(int p = 0; p < IntPacket::PayloadCount; p++ )
      {
      if( ipkt.Array[p] != ipkt.SeqNo + p )
        MyStats.DataError++;
      }


    BegLogLine(0)
      << "Server " << Platform::Topology::GetAddressSpaceId()
      << " Recieved #"
      << ipkt.SeqNo
      << " Expected "
      << MyStats.Received
      << EndLogLine;

    MyStats.Received++;

    }
  }


void
dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
  {
  int                     n;
  const int               on = 1;
  socklen_t               len;
  struct sockaddr *preply_addr;

  int PktSendRate = 1;

  preply_addr = (sockaddr *) malloc(servlen);

  setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

  IntPacket ipkt;
  bzero( &ipkt, sizeof( ipkt ) );
  ipkt.SeqNo = 0; //????

  // blow a gig
  for(int i = 0; i < 1024*1024 / OUR_PACKET_PAYLOAD ; i++ ) // send a million packests
    {
    BegLogLine(0)
      << "Client Sending SeqNo "
      << ipkt.SeqNo
      << EndLogLine;

    for(int p = 0; p < IntPacket::PayloadCount; p++ )
      ipkt.Array[p] = ipkt.SeqNo + p;

    sendto(sockfd, &ipkt, sizeof( ipkt ), 0, pservaddr, servlen);
    ipkt.SeqNo++;
    if(0) ///   ( PktSendRate / 10 + 1 ) < 1000000 )
      {
      usleep( 1000000 / ( PktSendRate / 10 + 1) );
      PktSendRate *= 2;
      }
    }
  }

static void
recvfrom_alarm(int signo)
  {
  return;         /* just interrupt the recvfrom() */
  }

int
PkMain(int argc, char **argv, char **)
  {
  int                     sockfd;
  struct sockaddr_in      servaddr;

  int AddressSpaceCount        =  Platform::Topology::GetAddressSpaceCount();
  for( int i = 0; i < AddressSpaceCount; i++ )
    {
    ThreadCreate( i, ServerThread, 0, NULL );
    }

  sleep( 5 );  // Avoids poorly the race cond

  //if (argc != 2)
  //  {
  //  cerr << "usage: udpcli01 <IPaddress>" << endl;
  //  exit( -1 );
  //  }

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(OUR_PORT);

  ////int rc = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  int rc = inet_pton(AF_INET,  BCAST_ADDR, &servaddr.sin_addr);
  assert( rc > 0 );

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

  exit(0);
  }


