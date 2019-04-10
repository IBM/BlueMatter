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
 
//#include <ostream>
//#include <fstream>
//#include <BlueMatter/DataReceiver.hpp>
//#include <BlueMatter/ExternalDatagram.hpp>

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

using namespace std;

int mSocket;
#define SOCKET_ERROR (-1)

void error_exit(char *str) { fprintf(stderr,"\n%s\n",str); fprintf(stderr,"errno %d\n ", errno); fflush(stderr); exit(-1); }

int
main(int argc, char **argv, char ** envp)
  {

  if( argc != 3 )
    error_exit( " usage: relay port filename " );


  char LastFileName[1024];
  LastFileName[ 0 ] = '\0';

  for(int ConnectCount = 0; 1;  ConnectCount++ )
    {
    printf("Starting connection %d\n", ConnectCount );

    int                     hServerSocket;
    struct hostent        * pHostInfo;
    struct sockaddr_in      Address;
    int    nAddressSize   = sizeof(struct sockaddr_in);

    hServerSocket = socket( AF_INET, SOCK_STREAM, 0 );

    if( hServerSocket == SOCKET_ERROR )
      error_exit( " socket system call failed " );

    Address.sin_addr.s_addr = INADDR_ANY;
    Address.sin_port        = htons( atoi( argv[1] ) );
    Address.sin_family      = AF_INET;

    printf("About to bind\n");

    int bind_rc = bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address));

    if( bind_rc == SOCKET_ERROR )
      error_exit("Could not connect to host");

    printf("About to getsockname\n");

    //  getsockname( hServerSocket, (struct sockaddr *) &Address, (unsigned long *)&nAddressSize);
    getsockname( hServerSocket, (struct sockaddr *) &Address, (socklen_t *)&nAddressSize);

    printf("About to listen\n");

    int listen_rc = listen( hServerSocket, 3);

    if( listen_rc == SOCKET_ERROR)
      error_exit("Could not listen");

    printf("About to accept\n");

    //// mSocket = accept( hServerSocket,(struct sockaddr*)&Address, (unsigned long *)&nAddressSize);
    mSocket = accept( hServerSocket,(struct sockaddr*)&Address, (socklen_t *)&nAddressSize);

    printf("Accepted\n");

    char LinkedFile[1024];

    printf("About to open index file >%s<\n", argv[2] );

    int index_fd = -1;

    while( 1 )
      {
      index_fd = open( argv[2], O_RDONLY );
      if( index_fd < 0 )
        sleep(1);//error_exit( " index file open failed ");
      else
        break;
      }

    printf("About to begin main loop\n");


    unsigned char OneByte;

    int index_read = 0;

    printf("Begin read/wait of index file \n");

    while( 1 )
      {
      index_read = read( index_fd, & OneByte, 1 );
      if( index_read < 0 )
        {
        error_exit( " index file read failed " );
        }
      else if( index_read == 1 )
        {
        break;
        }
      usleep(10000);
      }

    close( index_fd );

    printf("Done read/wait for index file - start index is %d \n", OneByte );
    
    char bs_call[256];  
    sprintf( bs_call, "ls -ltr > /dev/null " );
    
    for(;; OneByte++)
      {
      char FinalFileName[1024];

      char DataFileName1[1024];
      sprintf( DataFileName1, "%s.%02X", argv[2], OneByte );

      printf( "Begin wait/open for data file index %d name >%s< \n", OneByte,  DataFileName1 );
      int linked_fd = -1;

      while(1)
        {
          // sync();
         system( bs_call );

        linked_fd = open( DataFileName1, O_RDONLY );

        printf( "Value of linked_fd=%d\n", linked_fd );

        if( linked_fd > 0 )
          {
          strcpy( FinalFileName, DataFileName1 );
          break;
          }

        if( ( linked_fd < 0 ) && ( errno != ENOENT ) )
          {
          error_exit(" linked file open failed " );
          }


        if( ( linked_fd < 0 ) && ( errno != ENOENT ) )
          {
          error_exit(" linked file open failed " );
          }

        usleep( 10000 );

        }

      // It insn't clear that we need to delete the frame files - but if we do...
      if( LastFileName[ 0 ] != '\0')
        {
        printf("Unlinking file=%s\n", LastFileName );  
        int unlink_rc = unlink( LastFileName );
        sync();
        if( unlink_rc != 0 )
          error_exit( " linked file failed to unlink " );
        }

      printf("Done wait/open of >%s<\n", FinalFileName );
      strcpy( LastFileName, FinalFileName );

      char Buffer[32*1024];

      int RecycleSocketFlag = 0;

      int hdr[2];
      int DataFileLen = -1;

      while(1)
        {
        int data_hdr_read = read( linked_fd, hdr, sizeof( hdr ) );
        lseek( linked_fd, 0, SEEK_SET );
        if( data_hdr_read < sizeof( hdr ) )
          {
          usleep(10000);
          }
        else
          {
          DataFileLen = ntohl( hdr[1] );
          break;
          }
        }

      printf("Beginning relay of frame index %s , file len %d \n", FinalFileName, DataFileLen );

      for( int BufferCount = 0, TotalDataSent = 0 ;
           ( TotalDataSent < DataFileLen ) &&  ( ! RecycleSocketFlag ) ;
           BufferCount++ )
        {

        int data_read = read( linked_fd, Buffer, sizeof( Buffer ) );

        if( data_read < 0 )
          error_exit( " data read failed " );

        if( data_read == 0 )  // EOF ... but we are still waiting for data
          continue;

        for( int DataSent = 0; DataSent < data_read;  )
          {
          int send_rc = send( mSocket, & ( Buffer[ DataSent ] ) , data_read - DataSent, 0);
          if( send_rc < 0 )
            {
            if( errno == EAGAIN )
              continue;
            else
              {
              printf("Fatal looking socket send() error (%d) - break out to recycle socket.\n", errno );
              RecycleSocketFlag = 1;
              break;
              }
            }
          DataSent      += send_rc;
          TotalDataSent += send_rc;
          }
        }

      // NOTE: this file was unlinked above.
      printf(" Closing linked file.\n" );
      close( linked_fd );
      }


    if( LastFileName[ 0 ] != '\0')
      {
      int unlink_rc = unlink( LastFileName );
      if( unlink_rc != 0 )
        error_exit( " linked file failed to unlink " );
      }

    printf("Broken connection %d -- recycling connection \n", ConnectCount );
    close(mSocket);
    }

  }


