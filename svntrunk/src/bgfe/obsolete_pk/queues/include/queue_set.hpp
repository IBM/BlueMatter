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
 * Module:          queue_set
 *
 * Purpose:         Set support for single-reader-single-writer queues.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         220498 BGF Created : from dgramach, srswq*, etc.
 *
 ***************************************************************************/

#ifndef __QUEUE_SET_HPP__
#define __QUEUE_SET_HPP__

#ifndef PKFXLOG_CHANNEL_SET
#define PKFXLOG_CHANNEL_SET 0
#endif

#ifndef PKTRACE_ACTOR_COMM
#define PKTRACE_ACTOR_COMM 0
#endif

#ifndef PKFXLOG_QUEUESET_CONFIG
#define PKFXLOG_QUEUESET_CONFIG 0
#endif

#include <pk/rpc.hpp>

#ifndef SHMEM_QUEUE
#include <pk/datagram_queue.hpp>
#else
//#include <pk/shmem_queue.hpp>
#endif

#define DEFAULT_CHAN_DEPTH (1024)

// This puppy should have a constructor that actually returns
// an object which can be used to connect to it!  Returning
// just an address and having the caller put it into some structure
// is not good enough!
class QueueReaderSetDescriptor
  {
  public:
    int      CommResource;
    unsigned QueueReaderSetPtr;
  };


// TRY TO DEFINE MAX SLOTS BASED ON PACKET PAYLOAD, BUT IF PACKETS LARGE, LIMIT SLOTS
//NEED: This is to avoid blowing stacks... would be good to handle this more cleanly.
//NEED: the 32 is sort of sizeof( PhasedPipeline ) header... not great.
//NEED: bound to not more than 1024 ... again, not a great limitation.
#if defined(CYCLOPS)
//NEED: larger initiation message for plimptoniser setup.
#define ChannalReaderRefArr_MaxSlots 8
#else
// Note: 64 below is the size of the pipeline driver header
#define ChannalReaderRefArr_MaxSlots                        \
(                                                           \
  (((Platform::Reactor::PAYLOAD_SIZE - 64)                  \
    / sizeof(QueueReaderSetDescriptor)) < 1024 ) ?          \
   ((Platform::Reactor::PAYLOAD_SIZE - 64)                  \
    / sizeof(QueueReaderSetDescriptor)) : 1024              \
)
#endif

class QueueReaderRefArray
  {
  public:
    int                   ActiveCount;
    unsigned              RefTypeCode;
//NEED: to think of a better way to do this... but we've got to make
// this fit into a packet during PhasedPipeline setup!!!!
    QueueReaderSetDescriptor ReaderRef[ ChannalReaderRefArr_MaxSlots ];
  };



template<class ItemType>
class QueueReaderSet
  {
  public:
    char                                *mName;
    int                                  mConnectCount;
    DatagramQueueReader<ItemType>       *mDgQReaderArray;
    unsigned                             mCurrentIndex;

   public:
    QueueReaderSet()
      {
      BegLogLine( PKFXLOG_QUEUESET_CONFIG )
        << " QueueReaderSet::Constructor "
        << EndLogLine;
      mName = NULL;
      mConnectCount = -1;
      mDgQReaderArray = NULL;
      }

    unsigned
    GetAddress()
      {
      BegLogLine( PKFXLOG_QUEUESET_CONFIG )
        << " QueueReaderSet::GetAddress "
        << (void *) this
        << EndLogLine;
      return( (unsigned) this );
      }

  public:
    int Init( int aConnectCount = 1024, char *aName = NULL )
      {
      if( aName == NULL )
        mName = "NoChanName";
      else
        mName         = aName;
      BegLogLine( PKFXLOG_QUEUESET_CONFIG )
        << mName
        << " QueueReaderSet::Init "
        << (void *) this
        << EndLogLine;
      mConnectCount = aConnectCount;
      mCurrentIndex = 0;
      mDgQReaderArray  = new DatagramQueueReader<ItemType>[mConnectCount];
      assert( mDgQReaderArray != NULL );
      // Clear the receiver structures.
      for(int i=0; i<mConnectCount; i++ )
        mDgQReaderArray[i].Reset();
      return(0);
      }

    class QueueReaderSetConnectMessage
      {
      public:
      QueueReaderSetDescriptor ReaderSetDes;
      int PortNo;
      };

    static
    QueueReaderDescriptor
    Connect( QueueReaderSetDescriptor ReaderSetDes, int PortNo )
      {
      QueueReaderSetConnectMessage ConnMsg;
      ConnMsg.ReaderSetDes = ReaderSetDes;
      ConnMsg.PortNo       = PortNo;
      QueueReaderDescriptor Rc;
      PkRpc< QueueReaderSetConnectMessage,
             QueueReaderDescriptor >::Execute
               ( ReaderSetDes.CommResource,
                 BackEnd_Connect,
                 ConnMsg,
                 Rc ); // This gets filled in with actors comm address.
      return(Rc);
      }

    static
    QueueReaderDescriptor
    BackEnd_Connect( QueueReaderSetConnectMessage &ConnMsg )
      {
      QueueReaderDescriptor ReaderDes =
        ((QueueReaderSet<ItemType> *)(ConnMsg.ReaderSetDes.QueueReaderSetPtr))->
           mDgQReaderArray[ ConnMsg.PortNo ].Connect();
      BegLogLine( PKFXLOG_QUEUESET_CONFIG )
        << " QueueReaderSet::Connect "
        << " QueueReaderSetPtr "
        << (void *)  ConnMsg.ReaderSetDes.QueueReaderSetPtr
        << " PortNo "
        << ConnMsg.PortNo
        << " Connected at "
        << LogHexDump4( &ReaderDes )
        << EndLogLine;
      return( ReaderDes );
      }

    //Passthru functions to Reader of at mCurrentIndex
    int ReleaseItem()
      {
      int rc = mDgQReaderArray[ mCurrentIndex ].ReleaseItem();
      return( rc );
      }

    int Suspend()
      {
      //Should perhaps return live channel count for app dead lock detect.
      int rc = mDgQReaderArray[ mCurrentIndex ].Suspend();
      return( rc );
      }

    int ResumeAll()
      {
      int rc = 0;
      for( int Index = 0; Index < mConnectCount; Index++ )
        rc += mDgQReaderArray[ Index ].Resume();
      return( rc );
      }

    TraceClient TS_DgQueueRcvrSet_BeforeWait ;
    TraceClient TS_DgQueueRcvrSet_AfterWait ;

//NEED: to change names to POLL and WAIT

    ItemType *
    PollNextItem()
      {
      assert( mCurrentIndex >= 0 && mCurrentIndex < mConnectCount );
      ItemType * ItemPointer =
        mDgQReaderArray[ mCurrentIndex ].GetNextItem();

      if( ItemPointer == NULL && mConnectCount > 1 )  // Didn't get one... start scanning
        {
        //THIS WAS A PROBLEM IN HURSLEY CODE BUT WAS NOT ON THE PHASED_PIPELINE CODE PATH
        // When there are more than one queue to look at,
        // snapshot the current index and then scan the
        // array. I think this could be better.
        unsigned StartScanIndex = mCurrentIndex;
        mCurrentIndex++;
        if( mCurrentIndex == mConnectCount )
          mCurrentIndex = 0;

        while( mCurrentIndex != StartScanIndex )
          {
//NEED: to figure out if this fence is needed.
Platform::Memory::ExportFence();

          ItemPointer = mDgQReaderArray[ mCurrentIndex ].GetNextItem();

          if( ItemPointer != NULL )
            break; // GOT ONE!!

          // Increment and wrap mCurrentIndex
          mCurrentIndex++;
          if( mCurrentIndex == mConnectCount )
            mCurrentIndex = 0;
          }
        }
      return( ItemPointer );
      }

    ItemType *
    GetNextItem()
      {
      // Optimisticly try to get another element from the previously
      // active channel.
      // NEED to think about whether it would be better to move
      // on after some count (100) so as to provide fairness.
      ItemType * ItemPointer =
        mDgQReaderArray[ mCurrentIndex ].GetNextItem();

      if( ItemPointer == NULL )  // Didn't get one... start scanning
        {
        ////TS_DgQueueRcvrSet_BeforeWait.HitOE(PKTRACE_ACTOR_COMM, DgQueueRcvrSet_BeforeWait, mName, (int) this, BeforeWait);
        // Set up to do a blocking scan...
//NEED: To make the heart of this function a loop around the PollNextItem()
        unsigned StartScanIndex = mCurrentIndex;
        unsigned ScanCount = 0;
        while( ItemPointer == NULL )
          {
          mCurrentIndex++;

          if( mCurrentIndex == mConnectCount )
            mCurrentIndex = 0;
  // Need to figure out if this is needed.
  Platform::Memory::ExportFence();
          ItemPointer = mDgQReaderArray[ mCurrentIndex ].GetNextItem();

          if( mCurrentIndex == StartScanIndex )
            {
#if defined(CYCLOPS)
// On CYCLOPS, there are no virtual threads. But if there were, this would probably be the right thing to do.
            Platform::Thread::Yield() ;
#else
            if( ScanCount < 64 )
               Platform::Thread::Yield() ;
            else
               usleep( 100000 );
#endif
            }
          }
        ////TS_DgQueueRcvrSet_AfterWait.HitOE(PKTRACE_ACTOR_COMM, DgQueueRcvrSet_AfterWait, mName, (int) this, AfterWait);
        }
      return( ItemPointer );
      }

#if 0 //moved to datagram_queuereader class  // this doesn't go here... where does it go??
    static int
    DeliverDatagram( unsigned src, unsigned len, void *msg )
      {
      QueueDatagram<ItemType> *QDg =
           (QueueDatagram<ItemType> *)msg;
      BegLogLine( PKFXLOG_DGQUEUE_PROTOCOL )
        << " QueueReaderSet::DeliverDatagram: "
        << " Begun "
        << " bytes " << QDg->SizeOf()
        << " port "  << QDg->Port
        << " SeqNo " << QDg->SeqNo
        << " ItemCount " << QDg->ItemCount
        << EndLogLine;
      int Size = QDg->SizeOf();

      // cast the unsigned pointer value in datagram to proper pointer.
      QueueReaderSet<ItemType> *DgQReceiveSet =
        (QueueReaderSet<ItemType> *) QDg->QueueReaderSetPtr;

      if( DgQReceiveSet->mDgQReaderArray[ QDg->Port ].SendSideTask == -1 )
        DgQReceiveSet->mDgQReaderArray[ QDg->Port ].SendSideTask = src;
      else
        assert( DgQReceiveSet->mDgQReaderArray[ QDg->Port ].SendSideTask == src );


      Platform::Memory::ExportFence();   // putting the pointer in place makes dg available - fence data our first.
      assert( DgQReceiveSet->mDgQReaderArray[ QDg->Port ].DgPtr[ (QDg->SeqNo - 1) % DG_PER_CIRCUIT ] == NULL );

      DgQReceiveSet->mDgQReaderArray[ QDg->Port ].DgPtr[ (QDg->SeqNo - 1) % DG_PER_CIRCUIT ] = QDg;
      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " QueueReaderSet::DeliverDatagram: "
        << DgQReceiveSet->mDgQReaderArray[ QDg->Port ].mName
        << " Ended "
        << EndLogLine;
      return(PKREACTOR_DETATCH_BUFFER);
      }
#endif

  };


template<class ItemType>
class QueueWriterSet
  {
  private:
    char                                     *mName;
    DatagramQueueWriter<ItemType>            *QueueArray;
    unsigned                                  QueueCount;

    enum { NotInitialized = -1,
           NotReserved    = -2,
           AllReserved    = -3 } State;

    int                                  CurrentState;

    ItemType                         *CurrentItem;

    TraceClient TS_ActChanSet_BeforePushFlush ;
    TraceClient TS_ActChanSet_AfterPushFlush ;

  private:

    //Allow us to init one message, then set all to the same values.
    int SetAll( unsigned elem )
      {
      assert( CurrentState == AllReserved );
      for( int c = 0; c < QueueCount; c++ )
        if( c != elem )
          {
             BegLogLine(1)
               << "QueueWriterSet::SetAll "
               << c
               << ' ' << (void *) QueueArray[c].MsgBufPtr()
               << ' ' << (void *) QueueArray[ elem ].MsgBufPtr()
               << EndLogLine ;

          *(QueueArray[c].MsgBufPtr()) =
              *(QueueArray[ elem ].MsgBufPtr());
          }
      return(0);
      }


  public:
    QueueWriterSet()
      { mName = "Not Initialized"; CurrentState = NotInitialized; };

    int GetQueueCount() { return( QueueCount ); }

    // This method ought to be overloaded to take a single ref as well as a ref-arr.
    int Init( char *Name,
              unsigned PortNo,
              QueueReaderRefArray RefArr,
              int aQueueDepth = DEFAULT_CHAN_DEPTH )
      {
      mName = Name;
      BegLogLine( PKFXLOG_QUEUESET_CONFIG )
        << mName
        << " QueueWriterSet::Constuctor "
        << " PortNo "
        << PortNo
        << EndLogLine;
      assert( CurrentState == NotInitialized );
      CurrentState = NotReserved;
      QueueCount = RefArr.ActiveCount;
      QueueArray = new DatagramQueueWriter<ItemType> [ QueueCount ];
      assert( QueueArray != NULL );
      for( int c = 0; c < QueueCount; c++ )
        {
        // What follows results in the allocation of send buffers and
        // control blocks which are connect to the comm substrait.
////////QueueArray[c] =
////////  DatagramQueueWriter<ItemType>::Create( PortNo,
////////                                       RefArr.ReaderRef[c],
////////                                       aQueueDepth );
////////assert( QueueArray[c] != NULL );

        BegLogLine( PKFXLOG_QUEUESET_CONFIG )
          << mName
          << " QueueWriterSet::Init "
          << " Connecting c "
          << c
          << EndLogLine;

        // Call a static on the queue reader to get a connection
        QueueReaderDescriptor ReaderDes =
          QueueReaderSet<ItemType>::Connect( RefArr.ReaderRef[c], PortNo );

        BegLogLine( PKFXLOG_QUEUESET_CONFIG )
          << mName
          << " QueueWriterSet::Init "
          << " ReaderDes "
          << LogHexDump2( & ReaderDes )
          << EndLogLine;

        // Call Bind on the individual queue writer.
        QueueArray[ c ].Bind( ReaderDes, aQueueDepth );
        }
      return(0);
      }

    unsigned GetQueueSetMemberCount()
      {
      return( QueueCount );
      }

    ItemType *
    operator->()
      {
      assert( CurrentItem != NULL );
      return( CurrentItem );
      }

    ItemType *
    GetCurrentItemPtr()
      {
      assert( CurrentItem != NULL );
      return( CurrentItem );
      }

    int SetCurrentItem( ItemType &Elem )
      {
      assert( CurrentItem != NULL );
      *CurrentItem = Elem;
      return(0);
      }

    ItemType *
    ReserveAll()
      {
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "ReserveAll(): Begin" << EndLogLine;
      assert( CurrentState == NotReserved );
      CurrentState = AllReserved;
      for( int c = 0; c < QueueCount; c++ )
        QueueArray[c].BlockingReserveMsgBuf();
      CurrentItem = QueueArray[0].MsgBufPtr();
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "ReserveAll(): End" << EndLogLine;
      return(CurrentItem);
      }

    int PushAll()
      {
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "PushAll(): Begin" << EndLogLine;
      assert( CurrentState == AllReserved );
      SetAll(0);
      for( int c = 0; c < QueueCount; c++ )
        QueueArray[c].PushMsgBuf();
      CurrentItem = NULL;
      CurrentState = NotReserved;
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "PushAll(): End" << EndLogLine;
      return(0);
      }

    int FlushAll()
      {
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "FlushAll(): Begin" << EndLogLine;
      // this is used after a push where the code wasn't sure to flush or not.
      assert( CurrentState == NotReserved );
//      TS_ActChanSet_BeforePushFlush.HitOE(PKTRACE_ACTOR_COMM, ActChanSet_PushFlush, QueueArray[0]->Name, (int) this, BeforePushFlush) ;
      for( int c = 0; c < QueueCount; c++ )
        QueueArray[c].FlushMsgBuf();
//      TS_ActChanSet_AfterPushFlush.HitOE(PKTRACE_ACTOR_COMM, ActChanSet_PushFlush, QueueArray[0]->Name, (int) this, AfterPushFlush) ;
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "FlushAll(): End" << EndLogLine;
      return(0);
      }


    int PushFlushAll()
      {
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "PushFlushAll(): Begin" << EndLogLine;
      assert( CurrentState == AllReserved );
      SetAll(0);
      PushAll();
      FlushAll();
      BegLogLine( PKFXLOG_CHANNEL_SET ) << "PushFlushAll(): End" << EndLogLine;
      return(0);
      }

    ItemType *
    Reserve( int c )
      {
//      BegLogLine( PKFXLOG_CHANNEL_SET ) << "Reserve(): Begin  Num: " << c << EndLogLine;
      assert( c >= 0 && c < QueueCount );
      assert( CurrentState == NotReserved );
      QueueArray[c].BlockingReserveMsgBuf();
      CurrentItem = QueueArray[c].MsgBufPtr();
      CurrentState = c;
//      BegLogLine( PKFXLOG_CHANNEL_SET ) << "Reserve(): End  Num: " << c << EndLogLine;
      return( CurrentItem );
      }

    int Push()
      {
//      BegLogLine( PKFXLOG_CHANNEL_SET ) << "Push(): Begin  Num: " << CurrentState << EndLogLine;
      assert( CurrentState >= 0 && CurrentState < QueueCount );
      QueueArray[ CurrentState ].PushMsgBuf();
      CurrentItem = NULL;
      CurrentState = NotReserved;
//      BegLogLine( PKFXLOG_CHANNEL_SET ) << "Push(): End  Num: " << CurrentState << EndLogLine;
      return(0);
      }

    int PushFlush()
      {
//      BegLogLine( PKFXLOG_CHANNEL_SET ) << "PushFlush(): Begin  Num: " << CurrentState << EndLogLine;
      assert( CurrentState >= 0 && CurrentState < QueueCount );
      QueueArray[ CurrentState ].PushFlushMsgBuf();
      CurrentItem = NULL;
      CurrentState = NotReserved;
//      BegLogLine( PKFXLOG_CHANNEL_SET ) << "PushFlush(): End  Num: " << CurrentState << EndLogLine;
      return(0);
      }

  };



#endif
