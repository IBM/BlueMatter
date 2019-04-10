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
 //#include <rdgpacket.hpp>
#include <strstream.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>

#define BGL_TREE_PACKET_PAYLOAD 256

struct OutgoingUDPPacket
{
  int mAck;
};

#define UDP_PACKET_PAYLOAD_SIZE 1472

struct UdpPacket
{
  int   mLen;
  int   mSeqNum;
  char  mData[ UDP_PACKET_PAYLOAD_SIZE ];
};

void error(char *msg)
  {
  perror(msg);
  exit(-1);
  }

int main(int argc, char *argv[])
  {
  int sock, n;
  unsigned long length;
  unsigned long fromlen;
  struct sockaddr_in server;
  struct sockaddr_in from;
  char buf[2048];

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  if( sock < 0 )
    error("Opening socket");

  length = sizeof( server );
  bzero( &server, length );

  int port;
  if( argc == 2 )
    port        = htons( atoi( argv[1] ) );
  else
    port        = htons( 0 );


////////////
  int     namelength;
  struct  hostent hostent_copy;               /* Storage for hostent data.  */
  struct  hostent *hostentptr;                 /* Pointer to hostent data.   */
  static  char hostname[256];
  int     nRet;


  /*
   * Get the local host name.
   */
  nRet = gethostname( hostname, sizeof hostname );
  if( nRet )
    error ("gethostname failed");

  cerr << "gethostbyname() returned >" << hostname << "<" << endl;

  /*
   * Get pointer to network data structure for local host.
   */
  hostentptr = gethostbyname( hostname );
  if( hostentptr == NULL)
    error( "gethostbyname");

  /*
   * Copy hostent data to safe storage.
   */
  hostent_copy = *hostentptr;

  cerr << "hostent_copy.h_addr " << (void *) hostent_copy.h_addr << endl;

  int LocalIpAddr = * ((unsigned long *) hostent_copy.h_addr);
  int SaveIpAddr = LocalIpAddr;

  cerr << "LocalIpAddr " << (void*) LocalIpAddr << endl;

  if( argc <= 3 )
    LocalIpAddr = INADDR_ANY;

////////////

  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = LocalIpAddr;     // INADDR_ANY;
  server.sin_port        = htons( port );

  int bind_rc = bind( sock,
                     (struct sockaddr *) & server,
                      length);
  if( bind_rc < 0 )
    error("binding");


  // check what we really got for addr and port
  int gsn = getsockname(   sock,                                // socket
                        (  struct sockaddr *) & server,         // name
                         & length);                             // name length
  if( gsn != 0 )
        error( "getsockname for port");
  port = ntohs( server.sin_port );      // pick off the actual port asigned.

  //
  // permit sending of broadcast messages.
  //
  int nValue = 1;
  int sso = setsockopt(sock,                // socket
                       SOL_SOCKET,          // level
                       SO_BROADCAST,        // option name
                       (char *) &nValue,    // *pOptionValue
                        sizeof(nValue));     // nOptionLen

  if( sso != 0 )
    error("setsockopt for broadcast messages");

  //
  // Now set it up for non blocking read and writes.
  //
  int ulValue = 1;
  int is = ioctl( sock,                // socket descriptor
                  FIONBIO,                // cmd, Enable or disable non-blocking mode
                 &ulValue);              // *argp);
  if( is < 0 )
      perror("ioctlsocket(): setting non-blocking");


  fromlen = sizeof( struct sockaddr_in );

  cerr << "server IpAddr IpPort " << (void *) SaveIpAddr << " " << (void *) server.sin_port << endl;

  //fprintf(stderr,"Server IpAddr IpPort : %08X %08X \n", server.sin_addr.s_addr,  server.sin_port);
  //fflush(stderr);

  // SIMPLE 
  // Get a packet put it into rdg file. And we're done
  // Open an RDG file
  int open_perms = 0660 ;
  int Address = open( "RawDatagramOutput" ,
                      O_CREAT | O_TRUNC | O_WRONLY | O_APPEND,  // O_APPEND Required to ensure proper writes by threaded apps
                      open_perms );
  

  int expectedSeqNum = 0;

  int pollcnt = 0;
  while( 1 )
    {
    n = recvfrom(sock,
                 buf,
                 2048,
                 0,
                 (struct sockaddr *) & from,
                 &fromlen);

    if( n < 0 )
      {
      if( errno != EAGAIN )
        perror("recvfrom");
      else
        pollcnt++;
      if( pollcnt == 100 )
        sched_yield();
      else
        usleep(100000); // sleep off a timeslice
      }
    else
      {
      if( n%4 != 0 )
        printf("ERROR:: Can't send something to BGL that is not a multiple of 4 bytes...");
      
      UdpPacket* incPkt = ( UdpPacket * ) buf;

      //printf("pollcnt %d\n", pollcnt );
      pollcnt = 0;
      ///printf("Got %d bytes from addr %08X port %08X >%s<, curSeqNum=%d gotSeqNum=%d \n",
      //       n, from.sin_addr.s_addr, from.sin_port, buf, expectedSeqNum, incPkt->mSeqNum );
      //fflush(stdout);

      
      if(( incPkt->mSeqNum == expectedSeqNum ) || ( incPkt->mSeqNum == expectedSeqNum-1 ))
        {
          OutgoingUDPPacket AckPkt;
          AckPkt.mAck = incPkt->mSeqNum;      
          
          int SendN = sendto( sock,
                              (char *) & AckPkt,
                              sizeof( OutgoingUDPPacket ),
                              0,
                              (struct sockaddr *) & from,
                              fromlen);   
          if( SendN < 0 )
            perror("sendto");                
        }

      // Ignore duplicate packets
      if( incPkt->mSeqNum == expectedSeqNum ) 
        {
          // Write the received buffer to a file.
          int writen = ::write( Address, incPkt->mData, incPkt->mLen );                
                             
          expectedSeqNum++;          
        }
      }
    }
  }
