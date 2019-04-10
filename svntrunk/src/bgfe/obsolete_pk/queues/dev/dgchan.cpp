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
 * Project:         pk
 *
 * Module:          
 *
 * Purpose:      
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         300697 BGF Created.
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

#ifdef LINUX
#define PTHREAD_CREATE_UNDETACHED PTHREAD_CREATE_JOINABLE
#endif

#include <iostream.h>

#include <pk/fxlogger.hpp>
#include <pk/monitor.hpp>

#include "dglink.hpp"
#include "dgchan.hpp"

#include <pk/setpri.hpp>

#define SLOTS 15

#include <pk/setpri.hpp>

class BigLoad
  {
  public:
    int x;
    int k[20];
  };


ChannelSender<BigLoad>    SendCh;
ChannelReceiver<BigLoad>  RecvCh;
unsigned SunkMessages = 0;

void *SendFx( void *arg )
  {
  for( int i = 0; 1 ; i++ )
    {
    SendCh.LW.AccessPayload( i ).x = i ;
    SendCh.LW.Push( i );
    while( ! SendCh.LR.Poll( i ) )
      pthread_yield();
 //   assert( SendCh.LR.AccessPayload( i ) == i );
    SendCh.LR.FreePayload( i );
    }
  return( NULL);
  }

void *RecvFx( void *arg )
  {
  for( int i = 0; 1 ; i++ )
    {
    while( ! RecvCh.LR.Poll( i ) )
      pthread_yield();
    assert( RecvCh.LR.AccessPayload( i ).x == i );
    RecvCh.LR.FreePayload( i );
 //   RecvCh.LW.AccessPayload( i ) = i ;
    RecvCh.LW.Push( i );
    SunkMessages++;
    }
  return( NULL);
  }

int
PkMain(int argc, char **argv, char** envp)
  {
  BegLogLine(1)
    << "sizeof( SendCh ) "  << sizeof( SendCh )
    << " sizeof( RecvCh ) " << sizeof( RecvCh )
    << EndLogLine;


  SendCh.LW.Bind( 0, &RecvCh.LR );
  RecvCh.LW.Bind( 0, &SendCh.LR );

  pthread_t         SendTid, RecvTid;
  pthread_attr_t    attr;
  void             *arg;

  pthread_attr_init( &(attr) );
  pthread_attr_setdetachstate( &(attr), PTHREAD_CREATE_UNDETACHED);

  pthread_create( &SendTid,
                  &attr,
                  SendFx,
                  (void *) 0 );

  pthread_create( &RecvTid,
                  &attr,
                  RecvFx,
                  (void *) 0 );


  unsigned FinishedFlag = 0;

  Monitor m( &SunkMessages );
  m.SetFinishedFlagPtr( &FinishedFlag );

  m.Start();

  while( ! FinishedFlag )
    sleep( 1 );

  fflush(stdout);
  return(0);
  }

