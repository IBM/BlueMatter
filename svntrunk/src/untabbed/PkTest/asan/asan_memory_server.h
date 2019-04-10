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
 #ifndef __ASAN_MEMORY_SERVER__
#define __ASAN_MEMORY_SERVER__

#include <BonB/BGL_PartitionSPI.h>
#include <rts.h>
#include <Pk/API.hpp>
#include <Pk/Compatibility.hpp>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>

#include "gpfs2cn_protocol.h"

#define DEBUG_LOG ( 1 )

#define SOCKET_ERROR (-1)
// States
enum 
{
  MEMORY_SERVER_STATE_START,
  MEMORY_SERVER_STATE_CONNECTED,
  MEMORY_SERVER_STATE_DISCONNECTED
};

static void error_exit(char *str) 
{ 
  fprintf(stderr,"\n%s\n",str); 
  fprintf(stderr,"errno %d\n ", errno); 
  fflush(stderr); 
  exit(-1); 
}

static unsigned int readAll( int sockd, char* Buf, unsigned int len )
{
  unsigned long long bytesRead = 0;
  while( bytesRead < len )
    {
      int rlen = read( sockd, &Buf[ bytesRead ], (len-bytesRead) );

      StrongAssertLogLine( rlen >= 0 )
  << "Error on read from socket"
  << " errno: " << errno
  << EndLogLine;
  
      BegLogLine( 1 )
  << "readAll:: "
  << " sockd: " << sockd
  << " &Buf[ " << bytesRead << " ]: " << (void *) &Buf[ bytesRead ]
  << " bytesRead: " << bytesRead
  << " len: " << len
  << " rlen: " << rlen
  << EndLogLine;

      bytesRead += rlen;
    }
  
  return bytesRead;
}

static unsigned int writeAll( int sockd, char* Buf, unsigned int len )
{
  unsigned int bytesWritten = 0;
  while( bytesWritten < len )
    {
      int wlen = write( sockd, &Buf[ bytesWritten ], (len-bytesWritten) );

      StrongAssertLogLine( wlen >= 0 )
  << "Error on write from socket"
  << " errno: " << errno
  << EndLogLine;
    
      BegLogLine( 1 )
  << "writeAll:: "
  << " sockd: " << sockd
  << " &Buf[ " << bytesWritten << " ]: " << (void *) &Buf[ bytesWritten ]
  << " bytesWritten: " << bytesWritten
  << " len: " << len
  << " wlen: " << wlen
  << EndLogLine;

      bytesWritten += wlen;
    }

  return bytesWritten;
}

class ASAN_BufferManager
{
  static char * Buffer;
  static int    BufferSize;
  
 public:
  static void AllocateBuffer( int MBCount );
  static char* GetBufferPtr();
  static int GetBufferSize();
  
};

class ASAN_MemoryServer
{
  int    CurrentState;
  char * BufferPtr;

  int    sockfd;
  int    AssignedPort;


  in_addr_t GetIONodeIP();  
  void SendDisconnectionAck( int sockD );
  void SendConnectionAck( int sockD );
  void ConnectAndAck( int sockD, int port, in_addr_t IONodeIP );
  void AcceptConnection(int sockD );
  
  // This happens once for now
  in_addr_t GenerateDiskPortFile();
  void Init( );
  void Connect( int sockD, int port, in_addr_t IONodeIP );
  void SetBufferPtr( char * buff );
 public:
  ASAN_MemoryServer() {}

  void Run(); 
};

#endif
