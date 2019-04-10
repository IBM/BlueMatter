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

#include <pk/platform.hpp>

#include <stdio.h>

//#include <pk/fxlogger.hpp>
#include <pk/monitor.hpp>

#include <pk/datagram_queue.hpp>


#define SLOTS 15

typedef struct
  {
  unsigned Sender;
  } QMessage;

////volatile QueueReaderDescriptor ReaderDes; // = 0;
QueueReaderDescriptor ReaderDes; // = 0;

Platform::Memory::AtomicInteger ReaderDesReadyFlag = 0;

// Used to work with this on stack - now, with trouble on -O3, try here.
//DatagramQueueWriter<QMessage> QWriter;

void *WriterFx( void *arg )
  {
  DatagramQueueWriter<QMessage> QWriter;
  // If QWriter is bigger than the stack size, trouble!!!
  QMessage *qmsg;
  unsigned cnt = 0;

  // wait for reader descriptor to become valid.
//  while( ! ReaderDes.Initialized() )
//    Platform::Thread::Yield();

  while( ReaderDesReadyFlag == 0 )
    Platform::Thread::Yield();

  Platform::Memory::ImportFence();

  QWriter.Bind( ReaderDes, 2*1024 );

  for(;;)
    {
    //while( !( qmsg = QWriter.ReserveMsgBuf() ) )
    //  Platform::Thread::Yield();
      ///////////////////pthread_yield();
    qmsg = QWriter.BlockingReserveMsgBuf();
    assert( qmsg != NULL );
    qmsg->Sender = cnt++;
    QWriter.PushMsgBuf();
    }
  return( NULL);
  }

unsigned SunkMessages;

////  DatagramQueueReader<QMessage> QReader;
void *ReaderFx( void *arg )
  {
  DatagramQueueReader<QMessage> QReader;
  QMessage *qmsg;
  SunkMessages = 0;
  unsigned CachedSunkMessages = 0 ;
  ReaderDes = QReader.Connect();

  Platform::Memory::ExportFence();

  int rc = Platform::Memory::AtomicAdd( ReaderDesReadyFlag, 1 );

  for(;;)
    {
    while( ! ( qmsg = QReader.GetNextItem() ) )
      Platform::Thread::Yield();

    CachedSunkMessages = qmsg->Sender;

    QReader.ReleaseItem();
    if ( 0 == ( CachedSunkMessages % 1024 ) )
      {
      SunkMessages = CachedSunkMessages ;
      }
    }
  return( NULL );
  }

#include <pk/setpri.hpp>

PkMain(int argc, char **argv, char** envp)
  {

  Platform::Thread::Create( ReaderFx );
  Platform::Thread::Create( WriterFx );

  unsigned FinishedFlag = 0;

  Monitor m( &SunkMessages );
  m.SetFinishedFlagPtr( &FinishedFlag );

  m.Start();

  while( ! FinishedFlag )
    sleep( 1 );

  fflush(stdout);
  return(0);
  }


