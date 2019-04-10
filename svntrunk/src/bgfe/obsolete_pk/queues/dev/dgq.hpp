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
 * Module:          datagram_queue
 *
 * Purpose:         Datagram supported single-reader-single-writer queues.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         170297 BGF Created.
 *
 ***************************************************************************/

#ifndef __DATAGRAM_QUEUE_HPP__
#define __DATAGRAM_QUEUE_HPP__

#ifndef PKFXLOG_DGQUEUE
#define PKFXLOG_DGQUEUE (0)
#else
#define PKFXLOG_DGQUEUE_PROTOCOL (1)
#endif

#ifndef PKFXLOG_DGQUEUE_PROTOCOL
#define PKFXLOG_DGQUEUE_PROTOCOL (0)
#endif

#ifndef PKTRACE_CHAN_COMM
#define PKTRACE_CHAN_COMM 0
#endif

#ifndef PKTRACE_ACTOR_COMM
#define PKTRACE_ACTOR_COMM 0
#endif

#include <pk/fxlogger.hpp>
#include <pk/pktrace.hpp>

#include <pk/new.hpp>

#include <pk/rpc.hpp>      // remote procedure capability
//#include <pk/pthread.hpp>  // physical thread class

#include <pk/memfence.hpp> // SMP memory fencing ops.

//#include "globject.hpp"

#ifndef DATAGRAM_SIZE
#define DATAGRAM_SIZE PKREACTOR_DATAGRAM_SIZE
#endif

static char *NoChanName = "NoChanName";

#ifndef DG_PER_CIRCUIT
#define DG_PER_CIRCUIT (4)
#endif

#define DEFAULT_CHAN_DEPTH (1024)

//*****************************************************************************

class QueueReaderDescriptor
  {
  public:
    int      TaskNo;
    unsigned DatagramQueueReaderAddress;
    QueueReaderDescriptor()
      {
      TaskNo = -1;
      DatagramQueueReaderAddress = (unsigned) NULL;
      }
    
    int Initialized() volatile
      {
      return( (TaskNo != -1) && (DatagramQueueReaderAddress != (unsigned) NULL) );
      }
  };

// This is what actually flies.
template<class ItemType>
class QueueDatagram
  {
  public:
#ifdef BLUEGENE
    unsigned    src;                         // not needed except for debugging
    unsigned    len;                         // not needed except for debugging
#endif
    unsigned    DatagramQueueReaderAddress;  // probably ought to be a pointer, but could be other coding
    unsigned   *PointerToAckTargetArea;      // should be held by queue reader
    unsigned    SeqNo;                       // should be many fewer bits
    unsigned    ItemCount;                   // should be many fewer bits
    ItemType    ItemArray[1];                // to be over subscripted.

  //Datagram header should be easily packed into 8 bytes or less.

  QueueDatagram()
    {
    SeqNo = 0xFFFFFFFF;
    ItemCount = 0;
    }

  int SizeOf()
    {
    int Size = sizeof( QueueDatagram<ItemType> ) + ((ItemCount - 1) * sizeof( ItemType ));
    return( Size );
    }

  int MaxItemCount()
    {
    int mec = DATAGRAM_SIZE - sizeof( QueueDatagram<ItemType> );
    mec = mec / sizeof( QueueDatagram<ItemType> ) + 1;
    return( mec );
    }

  ItemType *
  GetItemPtr( unsigned ItemIndex )
    {
    assert( ItemIndex < ItemCount );
    ItemType *ItemPtr = &(ItemArray[ ItemIndex ]);
    return( ItemPtr );
    }

  };

// Methods and data to keep track of a receive port.
template<class ItemType>
class DatagramQueueReader
  {
  public:
    typedef QueueDatagram<ItemType> *QueueDatagramPtr;
    QueueDatagramPtr              DgPtr[DG_PER_CIRCUIT];
    int                           SelectDg;
    int                           SuspendedFlag;
    unsigned                      ExpectedDgSeqNo;
    int                           SendSideTask;
    int                           CurrentItemIndex;

    DatagramQueueReader()
      {
      Reset();
      }

    QueueReaderDescriptor
    Connect()
      {
      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " DatagramQueueReader::Connect "
        << " this "
        << (void *) this
        << " TaskNo "
        <<  MpiSubSys::GetTaskNo()
        << EndLogLine;
      QueueReaderDescriptor Rc;
      Rc.TaskNo = MpiSubSys::GetTaskNo();
      Rc.DatagramQueueReaderAddress = (unsigned) this;
      assert( SendSideTask == -1 ); //slimey cheepo way to make sure a reset has been done.
      return( Rc );
      };


    int Reset()
      {
      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " DatagramQueueReader::Reset "
        << " this "
        << (void *) this
        << " TaskNo "
        <<  MpiSubSys::GetTaskNo()
        << EndLogLine;
      for( int i = 0; i < DG_PER_CIRCUIT; i++ )
        {
        DgPtr[i]            = NULL;
        }
      SelectDg            = 0;
      ExpectedDgSeqNo     = 0;
      SuspendedFlag       = 0;
      SendSideTask        = -1;
      CurrentItemIndex = 0;
      return(0);
      }

    int Suspend() { SuspendedFlag = 1; return(0); }
    int Resume()  { SuspendedFlag = 0; return(0); }

    ItemType *
    GetNextItem()
      {
      ItemType *ItemPtr = NULL;

      if( DgPtr[SelectDg] != NULL && ! SuspendedFlag )
        ItemPtr = DgPtr[SelectDg]->GetItemPtr( CurrentItemIndex );
      return( ItemPtr );
      }

    class DatagramAckMsg
      {
      public:
#if BLUEGENE
        unsigned src;
        unsigned len;
#endif
        unsigned *PointerToAckTargetArea;
        unsigned AckSeqNo;
      };

    static int
#ifndef BLUEGENE
    DeliverDatagram( unsigned src, unsigned len, void *msg )
      {
      QueueDatagram<ItemType> *QDg =
           (QueueDatagram<ItemType> *)msg;
#else
    DeliverDatagram(  void *msg )
      {
      QueueDatagram<ItemType> *QDg =
           (QueueDatagram<ItemType> *)msg;
      unsigned src = QDg->src;
      unsigned len = QDg->len;
#endif
      BegLogLine( PKFXLOG_DGQUEUE_PROTOCOL )
        << " DatagramQueueReader::DeliverDatagram: "
        << " Begun "
        << " bytes " << QDg->SizeOf()
        << " SeqNo " << QDg->SeqNo
        << " ItemCount " << QDg->ItemCount
        << EndLogLine;
      int Size = QDg->SizeOf();

      // cast the unsigned pointer value in datagram to proper pointer.
      DatagramQueueReader<ItemType> *DgQReaderPtr =
        (DatagramQueueReader<ItemType> *) QDg->DatagramQueueReaderAddress;

      if( DgQReaderPtr->SendSideTask == -1 )
        DgQReaderPtr->SendSideTask = src;
      else
        assert( DgQReaderPtr->SendSideTask == src );


      ExportFence();   // putting the pointer in place makes dg available - fence data our first.
      assert( DgQReaderPtr->DgPtr[ (QDg->SeqNo - 1) % DG_PER_CIRCUIT ] == NULL );

      DgQReaderPtr->DgPtr[ (QDg->SeqNo - 1) % DG_PER_CIRCUIT ] = QDg;

      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " QueueReaderSet::DeliverDatagram: "
        << " Ended "
        << EndLogLine;
      return(PKREACTOR_DETATCH_BUFFER);
      }


    static int
#ifndef BLUEGENE
    QueueDatagram_Ack( unsigned src, unsigned len, void *msg)
      {
      DatagramAckMsg *DgAckMsg = (DatagramAckMsg *)msg;
#else
    QueueDatagram_Ack(  void *msg )
      {
      DatagramAckMsg *DgAckMsg = (DatagramAckMsg *)msg;
      unsigned src = DgAckMsg->src;
      unsigned len = DgAckMsg->len;
#endif

      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " DgQRecver::QueueDatagram_Ack: "
        << " Begun "
        << " assigning Ack of " << DgAckMsg->AckSeqNo
        << " to word at @" << (void *) DgAckMsg->PointerToAckTargetArea
        << EndLogLine;
      *(DgAckMsg->PointerToAckTargetArea) = DgAckMsg->AckSeqNo;
ExportFence();
      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " DgQRecver::QueueDatagram_Ack: "
        << " Ended "
        << " assigning Ack of " << DgAckMsg->AckSeqNo
        << " to word at @" << (void *) DgAckMsg->PointerToAckTargetArea
        << EndLogLine;
      return(0);
      }

    int ReleaseItem()
      {
      CurrentItemIndex++;
      // If this is the last element in the datagram, we need to ack and release the datagram.
      if( CurrentItemIndex == DgPtr[SelectDg]->ItemCount )
        {
        BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
          << " DgQRecver::ReleaseItem: "
          << " Before Trigger QueueDatagram_Ack on "
          << SendSideTask
          << EndLogLine;

        DatagramAckMsg DgAckMsg;
        DgAckMsg.PointerToAckTargetArea = DgPtr[SelectDg]->PointerToAckTargetArea;
        DgAckMsg.AckSeqNo               = DgPtr[SelectDg]->SeqNo;


        if( SendSideTask == MpiSubSys::GetTaskNo() )
          {
          // note: important to set this to NULL before acking
          // this is a bad aspect of prot - my need a fence.
          // This reps a poor portion of current protocol.
          DgPtr[SelectDg] = NULL;
#ifndef BLUEGENE
          QueueDatagram_Ack( SendSideTask, sizeof(DgAckMsg), (void *) &DgAckMsg);
#else
          DgAckMsg.src = SendSideTask;
          DgAckMsg.len = sizeof(DgAckMsg);
          QueueDatagram_Ack( (void *) &DgAckMsg);
#endif
          }
        else
          {
          // Important to free and NULL pointer ... same as above.
          PkActiveMsgSubSys::GetIFPtr()->FreeBuffer( (void *) DgPtr[ SelectDg ] );
          DgPtr[SelectDg] = NULL;
          PkActiveMsgSubSys::Trigger( SendSideTask,
                                      QueueDatagram_Ack,
                                      (void *) &DgAckMsg,
                                      sizeof( DgAckMsg ) );
          }

        BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
          << " DgQRecver::ReleaseItem: "
          << " After Trigger QueueDatagram_Ack on "
          << SendSideTask
          << EndLogLine;
        // NEED: to code this as a return to a datagram pool!
        SelectDg = (SelectDg + 1) % DG_PER_CIRCUIT;

        CurrentItemIndex = 0;
        }
      return(0);
      }

  };

//***************************************************************************************

template<class ItemType>
class DatagramQueueWriter
  {
  private:
    // Sequence number of the next datagram to be sent on this channel.
    class PerDg
      {
      public:
        unsigned                      AckExpected;
        unsigned                      AckTargetArea;
        QueueDatagram<ItemType>   *DgPtr;
        char                          RawDgArea[ DATAGRAM_SIZE ];
        PerDg()
          {
          DgPtr               = (QueueDatagram<ItemType> *) RawDgArea;
          DgPtr->PointerToAckTargetArea = & AckTargetArea;
          }
      };

    int                           DestTaskNo;
    unsigned                      NextDgSeqNo;
    unsigned                      SelectDg;
    PerDg                         Dg[DG_PER_CIRCUIT];

    enum { Uninitialized, ReserveReady, PushReady, FlushReady, DriveReady } State;

    ItemType *CurrentItemBuf;

    char *Name; // ... this really should be a pointer to some name buffer

  public:

    int Bind( QueueReaderDescriptor aReaderDes,
          int                           aDepth = DEFAULT_CHAN_DEPTH )
      {
      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " DatagramQueueWriter::Bind "
        << " this "
        << (void *) this
        << " ReaderDes.DgQReaderAddress "
        << (void *) aReaderDes.DatagramQueueReaderAddress
        << EndLogLine;

      for( int i = 0; i < DG_PER_CIRCUIT; i++ )
        {
        Dg[i].AckExpected           = 0;
        Dg[i].AckTargetArea         = 0;
        Dg[i].DgPtr->SeqNo          = i + 1 ;
        Dg[i].DgPtr->PointerToAckTargetArea = & (Dg[i].AckTargetArea);
        Dg[i].DgPtr->ItemCount   = 0;
        Dg[i].DgPtr->DatagramQueueReaderAddress =
                 aReaderDes.DatagramQueueReaderAddress;

        }
      DestTaskNo                    = aReaderDes.TaskNo;
      SelectDg                      = 0;
      CurrentItemBuf = NULL;
      State = ReserveReady;

      return(0);
      }

    DatagramQueueWriter()
      {
      BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
        << " DatagramQueueWriter::Constructor "
        << (void *) this
        << EndLogLine;
      DestTaskNo          = -1;
      NextDgSeqNo         = 1;
      State               = Uninitialized;
      }

    TraceClient TS_ActChanTo_BeforeReserveMsgBuf ;
    TraceClient TS_ActChanTo_AfterReserveMsgBuf ;

    ItemType*
    BlockingReserveMsgBuf() // This should be called "ReserveItemBuf()"
      {
      assert( State == ReserveReady );

      // When we try to move on to next packet, we must check/wait for
      // it to have b[Deen acked before using it.

      // If we need an ack and ahven't gotten one.
      if( Dg[SelectDg].DgPtr->SeqNo == Dg[SelectDg].AckExpected )
        {
        BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
          << " ReserveMsgBuf "
          << " Beg Wait "
          << " AckTargetArea "
          << Dg[SelectDg].AckTargetArea
          << " @"
          << &(Dg[SelectDg].AckTargetArea)
          << " AckExpected "
          << Dg[SelectDg].AckExpected
          << EndLogLine;
        int PollCount = 0;

        while( Dg[SelectDg].AckTargetArea != Dg[SelectDg].AckExpected )
          {
ImportFence();
          BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL && PollCount == 0 )
            << " ReserveMsgBuf "
            << " Spin Wait waiting "
            << " AckTargetArea "
            << Dg[SelectDg].AckTargetArea
            << " @"
            << &(Dg[SelectDg].AckTargetArea)
            << " AckExpected "
            << Dg[SelectDg].AckExpected
            << EndLogLine;

          PollCount++;
          if( PollCount < 64 )
            {
            //A98:  pthread_yield();
            sched_yield();
            }
          else
            usleep( 100000 );
          }

        BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL && PollCount > 0 )
          << " ReserveMsgBuf "
          << " End Wait "
          << " AckTargetArea "
          << Dg[SelectDg].AckTargetArea
          << " @"
          << &(Dg[SelectDg].AckTargetArea)
          << " AckExpected "
          << Dg[SelectDg].AckExpected
          << EndLogLine;

        Dg[SelectDg].DgPtr->SeqNo = NextDgSeqNo;
        Dg[SelectDg].DgPtr->ItemCount = 0;
        }

      // We're supposed to have notice the end of packet on the push.
      assert( Dg[SelectDg].DgPtr->ItemCount < Dg[SelectDg].DgPtr->MaxItemCount() );

      CurrentItemBuf =
        &(Dg[SelectDg].DgPtr->ItemArray[ Dg[SelectDg].DgPtr->ItemCount ]);

      Dg[SelectDg].DgPtr->ItemCount++;

      State = PushReady;

      return( CurrentItemBuf );
      }

    ItemType *
    MsgBufPtr()
      {
      assert( State == PushReady );
      return( CurrentItemBuf );
      }

    int PushMsgBuf( unsigned Size = sizeof( ItemType ) )
      {
      assert( State == PushReady );
      State = ReserveReady;

      if( Dg[SelectDg].DgPtr->ItemCount == Dg[SelectDg].DgPtr->MaxItemCount() )
        FlushMsgBuf();

      CurrentItemBuf = NULL;

      return(0);
      }

    TraceClient TS_ActChanTo_BeforeFlushMsgBuf ;
    TraceClient TS_ActChanTo_AfterFlushMsgBuf ;

    int FlushMsgBuf()
      {
      assert( State == ReserveReady );

      // Make sure that we haven't sent this Dg AND that there
      // is something to send.
      if( (  Dg[SelectDg].AckExpected
           < Dg[SelectDg].DgPtr->SeqNo )
          &&
          (Dg[SelectDg].DgPtr->ItemCount > 0) )
        {
        Dg[SelectDg].AckExpected    = Dg[SelectDg].DgPtr->SeqNo ;
        Dg[SelectDg].DgPtr->PointerToAckTargetArea = & (Dg[SelectDg].AckTargetArea);

#ifdef BLUEGENE
        Dg[SelectDg].DgPtr->src = MpiSubSys::GetTaskNo();
        Dg[SelectDg].DgPtr->len = Dg[SelectDg].DgPtr->SizeOf( );
#endif

        BegLogLine(PKFXLOG_DGQUEUE_PROTOCOL)
          << " FlushMsgBuf "
          << " SeqNo "
          << Dg[SelectDg].DgPtr->SeqNo
          << " AckArea @"
          << (void *) Dg[SelectDg].DgPtr->PointerToAckTargetArea
          << " NextDgSeqNo "
          << NextDgSeqNo
          << " SelectDg "
          << SelectDg
          << EndLogLine;

//        TS_ActChanTo_BeforeFlushMsgBuf.HitOE(PKTRACE_ACTOR_COMM, ActChanTo_BeforeFlushMsgBuf, Name, (int) this, BeforeFlushMsgBuf);
        if( DestTaskNo == MpiSubSys::GetTaskNo() )
          {
#ifndef BLUEGENE
          DatagramQueueReader<ItemType>::DeliverDatagram
                         ( MpiSubSys::GetTaskNo(),
                           Dg[SelectDg].DgPtr->SizeOf(),
                           (void *) Dg[SelectDg].DgPtr );
#else
          //NEED: should assert that the packet isn't too big here.
          DatagramQueueReader<ItemType>::DeliverDatagram
                         ( (void *) Dg[SelectDg].DgPtr );
#endif
          }
        else
          {
          PkActiveMsgSubSys::Trigger( DestTaskNo,
                                      DatagramQueueReader<ItemType>::DeliverDatagram,
                                      (void *) Dg[SelectDg].DgPtr,
                                      Dg[SelectDg].DgPtr->SizeOf() );
          }
//        TS_ActChanTo_AfterFlushMsgBuf.HitOE(PKTRACE_ACTOR_COMM, ActChanTo_AfterFlushMsgBuf, Name, (int) this, AfterFlushMsgBuf) ;

        NextDgSeqNo++;
        SelectDg = ( SelectDg + 1 ) % DG_PER_CIRCUIT;
        }

      // Figure out which is the next packet to go.
      return(0);
      }

    // This routine is just shorthand.
    int PushFlushMsgBuf( unsigned Size = sizeof( ItemType ) )
      {
      PushMsgBuf( Size );
      FlushMsgBuf();
      return(0);
      }
  };


#endif // __ACTCHAN_HPP__
