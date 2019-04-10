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
 #include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <signal.h>

using namespace std;

void sig_pipe( int signum )
  {
  printf("Got SIGPIPE\n");
  fflush(stdout);
  signal( SIGPIPE, sig_pipe );
  }

int RegressionClientSocket;
int RegressionServerSocket;

#define BUFFER_SIZE 32 * 1024 * 1024

char Buffer[ BUFFER_SIZE ]; 

#define SOCKET_ERROR (-1)

void error_exit(char *str) { fprintf(stderr,"\n%s\n",str); fprintf(stderr,"errno %d\n ", errno); fflush(stderr); exit(-1); }

static inline void write_to_socket( int sock, char * buff, int len )
  {
  for( int total = 0; total < len; )
    {
    int write_rc = write(   sock,
                            (((char *) buff) + total ),
                            len - total );
    if( write_rc < 0 )
      {
      // printf( "errno: %d\n", errno );
      if( errno == EAGAIN )
        continue;
      else
        error_exit( "ERROR:  FAILED TO WRITE TO SOCKET" );
      }

    total += write_rc;
    }
  }

int
main(int argc, char **argv, char ** envp)
  {
  signal( SIGPIPE, sig_pipe );

  if( argc != 3 )
    {
    printf("usage: %s [ port regression client ] [ port regression server ] \n", argv[0] );
    error_exit( " bad command line params " );
    }
  
  int                     RegressionClient_ServerSocket;
  struct sockaddr_in      RegressionClient_Address;
  
  int                     RegressionServer_ServerSocket;
  struct sockaddr_in      RegressionServer_Address;
  
  int    nAddressSize   = sizeof(struct sockaddr_in);
  
  /*****************************************************
   *  Setup the viewer -> relay connection
   ****************************************************/
  RegressionClient_ServerSocket = socket( AF_INET, SOCK_STREAM, 0 );
  
  if( RegressionClient_ServerSocket == SOCKET_ERROR )
    error_exit( " socket system call failed " );
 
  int BufferSize = 10 * 1024 * 1024;

  if( setsockopt( RegressionClient_ServerSocket, SOL_SOCKET, SO_RCVBUF, &BufferSize, sizeof( int ) ) == -1 )
    error_exit( "Failed to set RCVBUF size" ); 
 
  RegressionClient_Address.sin_addr.s_addr = INADDR_ANY;
  RegressionClient_Address.sin_port        = htons( atoi( argv[1] ) );
  RegressionClient_Address.sin_family      = AF_INET;
  
  printf("About to bind RegressionClient_ServerSocket\n");
  
  while(1)
    {
    int bind_rc = bind( RegressionClient_ServerSocket, (struct sockaddr*) &RegressionClient_Address, sizeof( RegressionClient_Address ) );
    if( bind_rc == 0 )
      break;
    if( bind_rc == SOCKET_ERROR && errno != 98 )
      error_exit("Could not connect to host");
    sleep(1);
    }
  
  printf("About to getsockname on RegressionClient_ServerSocket\n");

  getsockname( RegressionClient_ServerSocket, (struct sockaddr *) &RegressionClient_Address, (socklen_t *)&nAddressSize);
  
  printf("About to listen on RegressionClient_ServerSocket\n");
  
  int listen_rc = listen( RegressionClient_ServerSocket, 3);
  
  if( listen_rc == SOCKET_ERROR)
    error_exit("Could not listen");
  
  printf("About to accept on RegressionClient_ServerSocket\n");
    
  RegressionClientSocket = accept( RegressionClient_ServerSocket,(struct sockaddr*)& RegressionClient_Address, (socklen_t *)&nAddressSize);
  
  printf("Accepted connection to RegressionClient_ServerSocket\n");
    
  // Setup the RegressionServer
  RegressionServer_ServerSocket = socket( AF_INET, SOCK_STREAM, 0 );
  
  if( RegressionServer_ServerSocket == SOCKET_ERROR )
    error_exit( " socket system call failed " );
 
  if( setsockopt( RegressionServer_ServerSocket, SOL_SOCKET, SO_SNDBUF, &BufferSize, sizeof( int ) ) == -1 )
    error_exit( "Failed to set SNDBUF size" );
 
  RegressionServer_Address.sin_addr.s_addr = INADDR_ANY;
  RegressionServer_Address.sin_port        = htons( atoi( argv[2] ) );
  RegressionServer_Address.sin_family      = AF_INET;
  
  printf("About to bind RegressionServer_ServerSocket\n");
  
  while(1)
    {
    int bind_rc = bind( RegressionServer_ServerSocket, (struct sockaddr*) &RegressionServer_Address, sizeof( RegressionServer_Address ) );
    if( bind_rc == 0 )
      break;
    if( bind_rc == SOCKET_ERROR && errno != 98 )
      error_exit("Could not connect to host");
    sleep(1);
    }
  
  printf("About to getsockname on RegressionServer_ServerSocket\n");

  getsockname( RegressionServer_ServerSocket, (struct sockaddr *) &RegressionServer_Address, (socklen_t *)&nAddressSize);
  
  printf("About to listen on RegressionServer_ServerSocket\n");
  
  listen_rc = listen( RegressionServer_ServerSocket, 3);
  
  if( listen_rc == SOCKET_ERROR)
    error_exit("Could not listen");
  
  printf("About to accept on RegressionServer_ServerSocket\n");
    
  RegressionServerSocket = accept( RegressionServer_ServerSocket,(struct sockaddr*)& RegressionServer_Address, (socklen_t *)&nAddressSize);
  
  printf("Accepted connection to RegressionServer_ServerSocket\n");  

  long long total_bytes_read = 0;

  long long next_write = 1 * 1024 * 1024;

  while( 1 )
    {
    int data_read = read( RegressionClientSocket, Buffer, BUFFER_SIZE );
    
    if( data_read < 0 )
      error_exit( "ERROR:: read returned a negative length" );
    else
      {
      write_to_socket( RegressionServerSocket, Buffer, data_read );
      }

    total_bytes_read += data_read;
    if( total_bytes_read > next_write )
      {
      printf( "Total bytes read: %lld\n", total_bytes_read );
      next_write += 1 * 1024 * 1024;	
      }
    }
  }
