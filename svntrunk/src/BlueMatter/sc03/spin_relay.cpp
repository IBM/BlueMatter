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

#include <signal.h>

using namespace std;

void sig_pipe( int signum )
  {
  printf("Got SIGPIPE\n");
  fflush(stdout);
  signal( SIGPIPE, sig_pipe );
  }

int mSocket;
#define SOCKET_ERROR (-1)

void error_exit(char *str) { fprintf(stderr,"\n%s\n",str); fprintf(stderr,"errno %d\n ", errno); fflush(stderr); exit(-1); }

int
main(int argc, char **argv, char ** envp)
  {
  signal( SIGPIPE, sig_pipe );

  if( argc != 2  && argc != 3 )
    {
    printf("usage: %s [port] <int fps_value> \n", argv[0] );
    printf("usage: port is required port no of viewer \n", argv[0] );
    printf("usage: -l is a flag which causes prgram to loop on file (File must be SC03_Posit_File.00000000)\n", argv[0] );
    error_exit( " bad command line params " );
    }

  int FramesPerSecond = 5;
  if( argc == 3 )
    {
    FramesPerSecond = atoi( argv[2] );
    printf("Looping on first file.  Frame per second %d\n", FramesPerSecond);
    }


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

    while(1)
      {
      int bind_rc = bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address));
      if( bind_rc == 0 )
              break;
      if( bind_rc == SOCKET_ERROR && errno != 98 )
        error_exit("Could not connect to host");
      sleep(1);
      }

    printf("About to getsockname\n");

    getsockname( hServerSocket, (struct sockaddr *) &Address, (socklen_t *)&nAddressSize);

    printf("About to listen\n");

    int listen_rc = listen( hServerSocket, 3);

    if( listen_rc == SOCKET_ERROR)
      error_exit("Could not listen");

    printf("About to accept\n");

    mSocket = accept( hServerSocket,(struct sockaddr*)&Address, (socklen_t *)&nAddressSize);

    printf("Accepted\n");
    int RecycleSocketFlag = 0;


    for( int FileIndex = 0; ! RecycleSocketFlag ; FileIndex ++ )
      {

      char DataFile[1024];
      sprintf( DataFile, "SC03_Posit_File.%08X", FileIndex );

      printf("About to spin/open index file >%s<\n", DataFile );

      int DataFD = -1;

      while( 1 )
        {
        DataFD = open( DataFile, O_RDONLY );
        if( DataFD < 0 )
          sleep(1); //error_exit( " index file open failed ");
        else
          break;
        }

      printf("Done open of >%d<\n", DataFile );

      int hdr[2];
      int DataFrameSize = -1;

      while(1)
        {
        int data_hdr_read = read( DataFD, hdr, sizeof( hdr ) );
        lseek( DataFD, 0, SEEK_SET );
        if( data_hdr_read < sizeof( hdr ) )
          {
          usleep(10000);
          }
        else
          {
          DataFrameSize = ntohl( hdr[1] );
          break;
          }
        }


      char * Buffer = NULL;
      if( Buffer == NULL )
        {
        Buffer = new char[ DataFrameSize ];

        if( Buffer == NULL )
          error_exit(" Could not allocate data frame buffer" );
        }

      unsigned int * UIBuffer = (unsigned int * ) Buffer;


      printf("Beginning relay of frame index %s , file size %d \n", DataFile, DataFrameSize );

      int TotalDataSent = 0;

      for( int FrameIndex = 0; ! RecycleSocketFlag ; FrameIndex++ )
        {

        while( 1 ) // While we haven't gotten the frame correctly from the file
          {
          int TotalDataRead = 0;
          while( TotalDataRead < DataFrameSize )
            {
            int data_read = read(    DataFD,
                                  & (Buffer[TotalDataRead]) ,
                                     DataFrameSize - TotalDataRead );

            // horrible hack to allow looping for testing
            if( data_read == 0 && argc == 3 )
              {
              lseek( DataFD, 0, SEEK_SET );
              data_read = 0;
              continue;
              }

            if( data_read < 0 )
              error_exit( " data read failed " );

            TotalDataRead += data_read;
            }

          int CheckSumFieldIndex = 4;
          unsigned int ExpectedCheckSum = ntohl( UIBuffer[ CheckSumFieldIndex ] );
          unsigned int RawCheckSum      = UIBuffer[ CheckSumFieldIndex ] ;
          UIBuffer[ CheckSumFieldIndex ]     = 0;
          unsigned int ScannedCheckSum  = 0 ;

          for( int w = 0; w < DataFrameSize / sizeof( UIBuffer[0] ); w++ )
              ScannedCheckSum += ntohl( UIBuffer[ w ] );

          if( ScannedCheckSum == ExpectedCheckSum )
            {
            // Good frame - put checksum value back and move on
            UIBuffer[ CheckSumFieldIndex ] = RawCheckSum;
            break;
            }

          if( argc == 3 )
            error_exit(" got bad frame in file to be looped over - so it would never correct " );

          printf("BAD FRAME READ FrameIndex %d\n", FrameIndex );
          int lseek_rc = lseek( DataFD, FrameIndex * DataFrameSize, SEEK_SET );
          if( lseek_rc != (FrameIndex * DataFrameSize) )
            error_exit(" failed to lseek back to beginning of frame on re-read" );

          } // end of while -- we have a good frame

        int TypeFieldIndex = 2;
        unsigned int FrameType = ntohl( UIBuffer[ TypeFieldIndex ] );

        if( FrameType == 99 ) // signal to change files
          {
          // horrible hack to allow looping for testing
          if( argc == 3 )
            {
            lseek( DataFD, 0, SEEK_SET );
            continue;
            }
          printf("File change request came in. Closing >%s<\n", DataFile );
          break;
          }


        for( int DataSent = 0; DataSent < DataFrameSize ;  )
          {
          int send_rc = send( mSocket, & ( Buffer[ DataSent ] ) , DataFrameSize - DataSent, 0);
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
            printf("Frame index %d completed (read and sent in socket)\n", FrameIndex );
            fflush(stdout);
          }

          if( argc == 3 )
            usleep( 1000000 / FramesPerSecond );

        }

      printf("Exited Frame read loop, closing >%s<\n", DataFile );
      close( DataFD );

      if( argc != 3 )
        unlink( DataFile );

      }

    printf("Broken connection %d -- recycling connection \n", ConnectCount );
    close(mSocket);
    }

  }


