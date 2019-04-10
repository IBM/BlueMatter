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
 #include <strstream.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>

#include "CST.hpp"

void error(char *msg)
  {
  perror(msg);
  exit(-1);
  }


class CST_Client
  {
  public:

  };


int main(int argc, char **argv)
  {
  int sock, n;
  struct sockaddr_in server, from;
  struct hostent *hp;
  char buffer[256];

  enum { DataLen = 1024 * 1024 };
  char Data[ DataLen ];

  if( argc < 3  || argc > 4 )
    {
    printf("Usage: server port\n");
    exit(1);
    }

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  if( sock < 0 )
    perror("socket");

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


  server.sin_family = AF_INET;

  hp = gethostbyname( argv[1] );

  if( hp == 0 )
    error("Unknown host");

  bcopy( (char *)hp->h_addr,
         (char *)&server.sin_addr,
          hp->h_length);

  server.sin_port = htons( atoi( argv[2] ) );

  unsigned long int length = sizeof( struct sockaddr_in );

  if( argc == 3 )
    {
    printf("Please enter the message: ");
    bzero( buffer, 256 );
    fgets( buffer, 255, stdin);
    }
  else
    {
    strncpy( buffer, argv[3], 255 );
    buffer[ strlen( buffer ) + 1 ] = NULL;
    buffer[ strlen( buffer )     ] = '\n';
    }

  int ServerSideConfirmedSeqNo = 0;
  int NextSeqNoToSend          = 0;
  int TotalPacketsToSend       = 1000;

  while( 1 )
    {
    if( TotalPacketsToSend == (NextSeqNoToSend - 1 ))
      break;

    if( NextSeqNoToSend < TotalPacketsToSend )
      {
      CST_Packet cstp;
      cstp.Header.SeqNo    = NextSeqNoToSend++;
      cstp.Header.Metadata = 0;
      if( ( NextSeqNoToSend % 10) == 0 )
        cstp.Header.RequestStateUpdate = 1;
      else
        cstp.Header.RequestStateUpdate = 0;

      sprintf( cstp.Payload, "%d", cstp.Header.SeqNo  );

printf(">%s<\n", cstp.Payload );

      n = sendto( sock,
                  (void *) & cstp,  // buffer,
                  strlen(buffer) + CST_MAXLEN_HEADER,
                  0,
                  (struct sockaddr *) & server,
                  length);

      if( n < 0 )
        error("Sendto");
      }

    unsigned long int fromlen = sizeof( struct sockaddr_in );

    n = recvfrom(sock,
                 buffer,
                 sizeof( buffer ),
                 0,
                 (struct sockaddr *) & from,
                 &fromlen);

    if( n < 0 )
      {
      if( errno != EAGAIN )
        perror("recvfrom");
      }

    CST_Packet *cstpkt = (CST_Packet *) buffer;
    if( cstpkt->Header.Metadata )
      {
      CST_AckPacket *ap = (CST_AckPacket *) buffer;
      // If the received seqno advances us
      if( ap->AckedSeqNo > ServerSideConfirmedSeqNo )
        {
        printf("Server side has acked %d\n", ap->AckedSeqNo );
        fflush(stdout);
        ServerSideConfirmedSeqNo = ap->AckedSeqNo ;
        }
      }
    }
  }
