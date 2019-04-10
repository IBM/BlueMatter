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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          pkactor.C
 *
 * Purpose:         File contains cheasy unit test.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/

// All to one band width test.

#ifndef TEST_PAYLOAD_LOW
#define TEST_PAYLOAD_LOW (1)
#endif

#ifndef TEST_PAYLOAD_HIGH
#define TEST_PAYLOAD_HIGH (240)
#endif

#ifndef TEST_DATA_VOLUME
#define TEST_DATA_VOLUME (1*1024*1024)
#endif

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#define MAX_SENDERS 512

extern "C" {
extern void * PkMain( int, char**, char** );
};

volatile int Count[ MAX_SENDERS ];
volatile int SendSize = 2;           // Send a minimum of 2 ints [node,count]
volatile int SentSize[ MAX_SENDERS ];

int ActorFx( void *pkt )
  {
  int * data = (int *) pkt;

  Count[ data[ 0 ] ]++; // Record a count of how much data has been received

  // Note looking for this less than is important if packets come out of order.
  if( SentSize[ data[ 0 ] ] < data[ 1 ] )
    {
    //cerr << "From node " << data[0] << " SentSize from " << SentSize[ data[0] ] << " to " << data[1] << endl;
    SentSize[ data[ 0 ] ] = data[ 1 ] ;           // Record the size of packet that we received
    }
  return(0);
  }

int SetSendSize( void *pkt )
  {
  int * data = (int *) pkt;
  SendSize = *data;           // Record the size of packet that we received
  //cerr << "SetSendSize - SendSize now " << SendSize << endl;
  return(0);
  }

void * remote_driver_thread( void * )
  {
  int data[1024];
  data[0] = Platform::Topology::GetAddressSpaceId() ;
  for(;;)
    {
    //if( data[1] != SendSize )
    //  cerr << " SendSize Now " << SendSize << endl;
    data[1] = SendSize;
    Platform::Reactor::Trigger( 0, ActorFx, data, sizeof(int) * SendSize );
    }

  return( NULL );
  }

int remote_driver_thread_starter( void *something )
  {
  Platform::Thread::Create( remote_driver_thread, NULL, 0 );
  return( 0 );
  }

void * PkMain(int argc, char **argv, char **envp)
  {

  int AddressSpaceCount = Platform::Topology::GetAddressSpaceCount() ;

  for( int i = 1 ; i < Platform::Topology::GetAddressSpaceCount() ; i++ )
    {
    Platform::Reactor::Trigger( i, remote_driver_thread_starter, NULL, 0 );
    }

  for(int phase = 0; phase < 50; phase++ )
    {
    int LocalCount[ MAX_SENDERS ];

    // Wait for remote sender thread to start sending packets in which
    // will reset the SentSize from it's initial 0 value.
    for( int i = 1; i < Platform::Topology::GetAddressSpaceCount() ; i++ )
      {
      while( SentSize[ i ]  != SendSize )
        {
        Platform::Thread::Yield();
        }
      }

    // Let things stabalize.

    BegLogLine (1)
      << "Begin receiving " << SendSize << " word packets " 
  << EndLogLine ;

    sleep(1);
    for( int i = 0; i < Platform::Topology::GetAddressSpaceCount() ; i++ )
      {
      LocalCount[ i ] = 0;
      }

    int GrandTotal = 0;
    int Seconds = 0;
    for( int s = 0; s < 10 ; s++ )
      {
      sleep(1);
      Seconds++;

      for( int i = 0; i < Platform::Topology::GetAddressSpaceCount() ; i++ )
        {
        LocalCount[ i ] = Count[ i ];
        Count[ i ] = 0;
        }
  BegLogLine(1) 
    <<  "Sec " <<  s << " : " 
    << EndLogLine ;

      int Total = 0;
      for( int i = 0; i < Platform::Topology::GetAddressSpaceCount() ; i++ )
        {
  BegLogLine(1) 
    <<  LocalCount[ i ] << " " 
    << EndLogLine ;
        Total += LocalCount[ i ] ;
        }
      GrandTotal += Total;
  BegLogLine(1) 
    <<  " : Total Packets "  << Total  << " : Total Bytes / Sec  "  << Total * SendSize * 4 
    << EndLogLine ;
      }
    BegLogLine(1) 
   << " Average for " << SendSize * 4 << " Byte Payload "
         << " : Packets " << GrandTotal / Seconds
         << " : Bytes / Sec  " << (GrandTotal * SendSize * 4.0) / Seconds 
     << EndLogLine ;

    // Inform senders to switch to next size payload.
    SendSize++;
    for( int i = 1; i < Platform::Topology::GetAddressSpaceCount() ; i++ )
      Platform::Reactor::Trigger( i, SetSendSize, (void *) &SendSize, sizeof( SendSize ) );

    }

  }





