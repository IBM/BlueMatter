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
 * Module:          shmem_iv00
 *
 * Purpose:         Installation verification for shared memory queue.
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

//#include <pk/shmem_queue.hpp>
//#include <pk/datagram_queue.hpp>
#include "dgq.hpp"  
#include <pk/setpri.hpp>

#define SLOTS 15

typedef struct
  {
  unsigned Sender;
  } QMessage;

// this should be made an Atomic class entity.
QueueReaderDescriptor ReaderDes; // = 0;
int ReaderConnected = 0;

void *WriterFx( void *arg )
  {
  DatagramQueueWriter<QMessage> QWriter;
  // If QWriter is bigger than the stack size, trouble!!!
  QMessage *qmsg;
  unsigned cnt = 0;

  // wait for reader descriptor to become valid.
  while( ! ReaderDes.Initialized() )
    pthread_yield();

  QWriter.Bind( ReaderDes, 2*1024 );

  for(;;)
    {
      //    while( !( qmsg = QWriter.ReserveMsgBuf() ) )
      //pthread_yield();
    BegLogLine(1) << "WriterFx Waiting" << EndLogLine;
    qmsg = QWriter.BlockingReserveMsgBuf();
    assert( qmsg != NULL );
    qmsg->Sender = cnt++;
    QWriter.PushMsgBuf();
    }
  return( NULL);
  }

unsigned SunkMessages;

void *ReaderFx( void *arg )
  {
  DatagramQueueReader<QMessage> QReader;
  QMessage *qmsg;
  SunkMessages = 0;
  unsigned CachedSunkMessages = 0 ;
  ReaderDes = QReader.Connect();
  ReaderConnected = 1;
  for(;;)
    {
    BegLogLine(1) << "ReaderFx Waiting" << EndLogLine;
    while( ! ( qmsg = QReader.GetNextItem() ) )
      pthread_yield();
    CachedSunkMessages = qmsg->Sender;
    ////cerr << "Received\n" ;
    QReader.ReleaseItem();
    if ( 0 == ( CachedSunkMessages % 1024 ) )
      {
      SunkMessages = CachedSunkMessages ;
      }
    }
  return( NULL );
  }


#include <pk/rpc.hpp>

class MyArgs
  {
  public:
    QueueReaderDescriptor ReaderDes;
#if 0
    MyArgs( QueueReaderDescriptor aReaderDes )
      {
      ReaderDes = aReaderDes;
      }
#endif
  };

class MyResults
  {
  public:
  };

MyResults MyFunction( MyArgs &Args )
  {
  MyResults Rc;
  pthread_t    WriterTid;
  pthread_attr_t    attr;
  pthread_attr_init( &(attr) );
  pthread_attr_setdetachstate( &(attr), PTHREAD_CREATE_UNDETACHED);

  ReaderDes = Args.ReaderDes;

  pthread_create( &WriterTid,
                  &attr,
                  WriterFx,
                  (void *) 0 );
  BegLogLine(1) << "MyFunction Finished" << EndLogLine;
  return( Rc );
  }

#include <pk/setpri.hpp>

int
PkMain(int argc, char **argv, char** envp)
  {

  pthread_t   ReaderTid, WriterTid;
  pthread_attr_t    attr;
  void             *arg;

  pthread_attr_init( &(attr) );
  pthread_attr_setdetachstate( &(attr), PTHREAD_CREATE_UNDETACHED);

  pthread_create( &ReaderTid,
                  &attr,
                  ReaderFx,
                  (void *) 0 );

  // Spin until the reader connects.
  while( !ReaderConnected )
    pthread_yield();

  MyResults Res;
  MyArgs    Args;
  Args.ReaderDes = ReaderDes;
  PkRpc<MyArgs,MyResults>::Execute( 0, MyFunction, Args, Res );


//  assert( SchedTuneSelf() == 0 );

  unsigned FinishedFlag = 0;

  Monitor m( &SunkMessages );
  m.SetFinishedFlagPtr( &FinishedFlag );

  m.Start();

  while( ! FinishedFlag )
    usleep( 1 );

  fflush(stdout);
  return(0);
  }

