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
 #include <strstream>
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

#include "CST_Client.hpp"

#include <pk/fxlogger.hpp>

CST_Client cst;

int main(int argc, char **argv)
  {
  BegLogLine(1)
    << argv[0]
    << " Running .. "
    << EndLogLine;
  int sock, n;
  struct sockaddr_in server, from;
  struct hostent *hp;
  char buffer[256];

  if( argc < 3  || argc > 4 )
    {
    printf("Usage: server port\n");
    exit(1);
    }

  server.sin_family = AF_INET;

  hp = gethostbyname( argv[1] );

  assert( hp != NULL );

  bcopy( (char *)hp->h_addr,
         (char *)&server.sin_addr,
          hp->h_length);

  server.sin_port = htons( atoi( argv[2] ) );


  int TotalToSend = 30;
  int FlushModulo = 10;

  cst.Init( server.sin_addr.s_addr , server.sin_port, FlushModulo * 2 );

  for(int TotalSent = 0 ; TotalSent < TotalToSend ; )
    {
    // Send if possible
    int post_rc =  cst.Post( (char *) (&TotalSent), sizeof( TotalSent ) );

    if( post_rc == 0 )
      {
      TotalSent++;
      }

    if( ( post_rc != 0 ) || ( TotalSent % FlushModulo == 0 ) )
      {
      BegLogLine(1)
        << "main() "
        << "Starting Push phase "
        << EndLogLine;

      cst.Sync();               // send a sync packet
      while( cst.Push() == 0 ); // push a bunch of data packests
      cst.PollAsyncEvent();     // see if we got anything back

      BegLogLine(1)
        << "main() "
        << "Done Push phase "
        << EndLogLine;
      }
    }

  BegLogLine(1)
    << "main() "
    << "Done main loop - do a final sync "
    << EndLogLine;

  cst.BlockingSync();
  cst.Close();

  BegLogLine(1)
    << "main() "
    << "Finished"
    << EndLogLine;

  }
