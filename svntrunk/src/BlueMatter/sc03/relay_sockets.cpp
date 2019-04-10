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

int mSocket_v;
int mSocket_b;
#define SOCKET_ERROR (-1)

void error_exit(char *str) { fprintf(stderr,"\n%s\n",str); fprintf(stderr,"errno %d\n ", errno); fflush(stderr); exit(-1); }

int
main(int argc, char **argv, char ** envp)
  {
  signal( SIGPIPE, sig_pipe );

  if( argc != 3 )
    {
    printf("usage: %s [port viewer] [port bgl] \n", argv[0] );
    error_exit( " bad command line params " );
    }

  char LastFileName[1024];
  LastFileName[ 0 ] = '\0';

  for(int ConnectCount = 0; 1;  ConnectCount++ )
    {
    printf("Starting connection %d\n", ConnectCount );

    int                     vServerSocket;
    struct sockaddr_in      vAddress;

    int                     bServerSocket;
    struct sockaddr_in      bAddress;
   
    int    nAddressSize   = sizeof(struct sockaddr_in);


    /*****************************************************
     *  Setup the viewer -> relay connection
     ****************************************************/
    vServerSocket = socket( AF_INET, SOCK_STREAM, 0 );

    if( vServerSocket == SOCKET_ERROR )
      error_exit( " socket system call failed " );

    vAddress.sin_addr.s_addr = INADDR_ANY;
    vAddress.sin_port        = htons( atoi( argv[1] ) );
    vAddress.sin_family      = AF_INET;

    printf("About to bind vServerSocket\n");

    while(1)
      {
      int bind_rc = bind( vServerSocket, (struct sockaddr*) &vAddress, sizeof( vAddress ) );
      if( bind_rc == 0 )
              break;
      if( bind_rc == SOCKET_ERROR && errno != 98 )
        error_exit("Could not connect to host");
      sleep(1);
      }

    printf("About to getsockname on vServerSocket\n");

    getsockname( vServerSocket, (struct sockaddr *) &vAddress, (socklen_t *)&nAddressSize);

    printf("About to listen on vServerSocket\n");

    int listen_rc = listen( vServerSocket, 3);

    if( listen_rc == SOCKET_ERROR)
      error_exit("Could not listen");

    printf("About to accept on vServerSocket\n");

    mSocket_v = accept( vServerSocket,(struct sockaddr*)&vAddress, (socklen_t *)&nAddressSize);

    printf("Accepted connection to viewer\n");

    /*****************************************************
     *  Setup the BG/L -> relay connection
     ****************************************************/
    bServerSocket = socket( AF_INET, SOCK_STREAM, 0 );

    if( bServerSocket == SOCKET_ERROR )
      error_exit( " socket system call failed " );

    bAddress.sin_addr.s_addr = INADDR_ANY;
    bAddress.sin_port        = htons( atoi( argv[2] ) );
    bAddress.sin_family      = AF_INET;

    printf("About to bind on bServerSocket\n");

    while(1)
      {
      int bind_rc = bind( bServerSocket, (struct sockaddr*) &bAddress, sizeof( bAddress ) );
      if( bind_rc == 0 )
              break;
      if( bind_rc == SOCKET_ERROR && errno != 98 )
        error_exit("Could not connect to host");
      sleep(1);
      }

    printf("About to getsockname on bServerSocket\n");

    getsockname( bServerSocket, (struct sockaddr *) &bAddress, (socklen_t *)&nAddressSize);

    printf("About to listen on bServerSocket\n");

    int listen_rc1 = listen( bServerSocket, 3);

    if( listen_rc1 == SOCKET_ERROR)
      error_exit("Could not listen");

    printf("About to accept on bServerSocket\n");

    mSocket_b = accept( bServerSocket,(struct sockaddr*)&bAddress, (socklen_t *)&nAddressSize);

    printf("Accepted connection to BG/L\n");
    /****************************************************/
    
    
    int RecycleSocketFlag = 0;
    for( int FileIndex = 0; ! RecycleSocketFlag ; FileIndex ++ )
      {

      int hdr[2];
      int DataFrameSize = -1;

      //  HERE WE READ TWO INTS OF HEADER
      //  First is string "MDS\0" as 4-byte int
      //  Second is Length of data block, in bytes, including 24-byte header
      int data_hdr_read = read( mSocket_b, hdr, sizeof( hdr ) );
      if( data_hdr_read < sizeof( hdr ) )
        {
          usleep(10000);
        }
      else
        {
          DataFrameSize = ntohl( hdr[1] );
        }
      
      char * Buffer = NULL;
      if( Buffer == NULL )
        {
        Buffer = new char[ DataFrameSize ];

        if( Buffer == NULL )
          error_exit(" Could not allocate data frame buffer" );
        }

      unsigned int * UIBuffer = (unsigned int * ) Buffer;

      int TotalDataSent = 0;

      UIBuffer[0] = (unsigned int)hdr[0];
      UIBuffer[1] = (unsigned int)hdr[1];
      int firsttime = 1;
      
      // The following assumes the data frame size is the same for all frames
      for( int FrameIndex = 0; ! RecycleSocketFlag ; FrameIndex++ )
        {
          //         while( 1 ) // While we haven't gotten the frame correctly from the file
          //           {
          int TotalDataRead = 0;

	  // need to skip over the header info that is already read on the first time step
	  if (firsttime)
	    {
	      TotalDataRead = 2*sizeof(hdr[0]);
	      firsttime = 0;
	    }
	    
          while( TotalDataRead < DataFrameSize )
            {
              int data_read = read(    mSocket_b,
                                       & (Buffer[TotalDataRead]) ,
                                       DataFrameSize - TotalDataRead );
              
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

	  //printf("ScannedCheckSum: %d ExpectedCheckSum: %d\n", ScannedCheckSum, ExpectedCheckSum );
	  //printf( "DataFrameSize: %d\n", DataFrameSize );

           if( ScannedCheckSum == ExpectedCheckSum )
             {
               // Good frame - put checksum value back and move on
               UIBuffer[ CheckSumFieldIndex ] = RawCheckSum;
             }
           else
             {              
	       printf("BAD FRAME READ FrameIndex %d", FrameIndex );
	       printf(" ScannedCheckSum: %d ExpectedCheckSum: %d", ScannedCheckSum, ExpectedCheckSum );
	       printf(" DataFrameSize: %d\n", DataFrameSize );
               // Do not set the bad frame to the viewer
               // continue;
             }
          
          // int lseek_rc = lseek( DataFD, FrameIndex * DataFrameSize, SEEK_SET );
          // if( lseek_rc != (FrameIndex * DataFrameSize) )
          //   error_exit(" failed to lseek back to beginning of frame on re-read" );          
          //  } // end of while -- we have a good frame
          
          //         int TypeFieldIndex = 2;
          //         unsigned int FrameType = ntohl( UIBuffer[ TypeFieldIndex ] );
          
          //         if( FrameType == 99 ) // signal to change files
          //           {
          //           printf("File change request came in. Closing >%s<\n", DataFile );
          //           break;
          //           }          
          
          // Send the data to the viewer
          for( int DataSent = 0; DataSent < DataFrameSize ;  )
            {
              int send_rc = send( mSocket_v, & ( Buffer[ DataSent ] ) , DataFrameSize - DataSent, 0);
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
              printf("RELAY Frame %d\n", FrameIndex );
              // fflush(stdout);
            }
        }
      }

    printf("Broken connection %d -- recycling connection \n", ConnectCount );
    close(mSocket_b);
    close(mSocket_v);
    }
  }


