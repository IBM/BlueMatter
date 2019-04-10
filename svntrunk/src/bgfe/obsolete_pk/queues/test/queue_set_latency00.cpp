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
 * Module:          srswq.C
 *
 * Purpose:         Unit test for single reader single writer memory queues.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         300697 BGF Created.
 *
 ***************************************************************************/
#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#include <pk/monitor.hpp>

#include <pk/queue_set.hpp>
#include <pk/setpri.hpp>     // for schedtune

#define SLOTS 15

typedef struct
  {
  unsigned Sender;
  } QMessage;

// Declare the ends of the channel in global space.
#ifndef CACHE_PADDING
#define CACHE_PADDING(X) int X[8]
#endif

CACHE_PADDING(aaa);
QueueReaderSetDescriptor    TheAddress;
CACHE_PADDING(bbb);
QueueReaderRefArray       TheArray;
CACHE_PADDING(ccc);
QueueReaderSet<QMessage> RecverQueueSet;
CACHE_PADDING(ddd);
QueueWriterSet<QMessage> SenderQueueSet;
CACHE_PADDING(eee);

void *SourceFx( void *arg )
  {
  QMessage *qmsg;
  unsigned cnt = 0;
  for(;;)
    {
    for( int i = 0;
         !( qmsg = SenderQueueSet.Reserve( 0 ) );
         i++ )
      {
      if( i < 1024 )
        Platform::Thread::Yield();
      else
        usleep(100000);
      }
    qmsg->Sender = cnt++;
    SenderQueueSet.Push();
    }
  return( NULL);
  }

unsigned SunkMessages;

void *SinkFx( void *arg )
  {
  unsigned CachedSunkMessages = 0;
  SunkMessages = 0;
  QMessage *qmsg;
  for(;;)
    {
    for( int i = 0;
         !(qmsg = RecverQueueSet.PollNextItem());
         i++ )
      {
      if( i < 1024 )
        Platform::Thread::Yield();
      else
        usleep(100000);
      }
    SunkMessages = CachedSunkMessages =  qmsg->Sender;
    RecverQueueSet.ReleaseItem();
    }
  return( NULL );
  }

PkMain( int argc, char **argv, char **envp )
  {
  RecverQueueSet.Init( 1,          // Number of connectors.
                         "Sender" );

  TheAddress.QueueReaderSetPtr = (unsigned) RecverQueueSet.GetAddress();
  TheAddress.CommResource      = Platform::Topology::GetAddressSpaceId(); ////MpiSubSys::GetTaskNo();

  TheArray.ReaderRef[0] = TheAddress;
  TheArray.ActiveCount  = 1;

  SenderQueueSet.Init(   "Recver",
                         0,          // The connection being used.
                         TheArray,
                         1024 * 1024 );

  Platform::Thread::Create( SinkFx, NULL, 0 );
  Platform::Thread::Create( SourceFx, NULL, 1 );

  unsigned FinishedFlag = 0;

  Monitor m( &SunkMessages );
  m.SetFinishedFlagPtr( &FinishedFlag );

  m.Start();

  while( ! FinishedFlag )
    sleep( 1 );

  return(0);
  }

