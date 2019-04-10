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
 * Module:          srswq.hpp
 *
 * Purpose:         Single-Reader Single-writer queues, queue sets, etc.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         170697 BGF Created from file srswach.hpp.
 *
 ***************************************************************************/

THIS WORKS BUT ONE SHOULD USE DATAGRAM QUEUES


#ifndef __XSRSWQ_HPP__
#define __XSRSWQ_HPP__

#define DEFAULT_CHAN_DEPTH (1024)

#define SRS_SLEEP_COUNT 4096

#include <pk/fxlogger.hpp>
#include <pk/pktrace.hpp>
#include <strings.h>    //for bzero

#ifdef NO_SLEEPS
#define USLEEP(x) pthread_yield()
#else
#define USLEEP(x) usleep(x)
#endif

#ifndef PKFXLOG_SRSWQ
#define PKFXLOG_SRSWQ 0
#endif

// VERIFY SEQUENCE HAS BEEN TAKEN OUT - IT IS EXPECTED
// THAT UNIT TESTS WILL VERIFY QUEUE FUNCTION

#include <pk/memfence.hpp>

// Fences take the place of 'volatile', for weakly-ordered memory architectures
//#define SMP_SHARED volatile
#define SMP_SHARED

#define CACHE_PADDING(X) int X[8]

static
unsigned RealDepth(unsigned aDepth)
  {
  // Round up all queue depths to the next higher power of 2
  // because then we can do some arithmetic by bit-masking
  // rather then by division/modulus.
  unsigned int rd = 1 ;
  for (int x = 0 ; x < 32 ; x += 1)
    {
    if (rd >= aDepth)
      return rd ;
    rd <<= 1 ;
    } /* endfor */
  // You could get here if someone tried to configure a queue with
  // more than 2**31 elements; but this is not sensible for other
  // reasons, so a 'firewall' trap is OK.
  assert(0) ;
  return 1024 ;
  }


class SharedArea
  {
  public:
    unsigned  mTypeOfItem;
    unsigned  mSizeOfItem;
    unsigned  mDepth;       // 'Init' will force this to a power of 2

    char     *mBuffer;

    CACHE_PADDING(shared_pad_1);

    unsigned  mPutCnt;

    CACHE_PADDING(shared_pad_2);

    unsigned  mGotCnt;

    CACHE_PADDING(shared_pad_3);

  public:
    void
    CheckIndexes() const
      {
#if SAFETY_ON
      int x = mPutCnt; // This line seems to make the 'this' pointer valid.
      assert( mPutCnt <= (mGotCnt + mDepth) );
#endif
      }

    SharedArea( unsigned aSizeOfItem,
                unsigned aDepth = 1,
                unsigned aTypeOfItem = -1 )
      {
      mSizeOfItem = aSizeOfItem;
      mDepth      = aDepth;
      mTypeOfItem = aTypeOfItem;
      // Allocate forcing double word allinement.
      mBuffer     =
        (char *) new double[ ((mDepth * Platform::Memory::Align(mSizeOfItem)) / sizeof(double)) + 1 ];
      }
    unsigned Depth() { return( mDepth ); }

    void
    GotCnt(unsigned Value)
      {
      CheckIndexes();
      mGotCnt = Value; return;
      }

    unsigned
    GotCnt() const
      {
      CheckIndexes();
      return( mGotCnt );
      }

    void
    PutCnt(unsigned Value)
      {
      CheckIndexes();
      ExportFence() ;    // Make sure the data goes out before control value
      mPutCnt = Value;
      return;
      }

    unsigned
    PutCnt() const
      {
      CheckIndexes();
      ImportFence();
      return( mPutCnt );
      }
  };


//typedef unsigned QueueReaderDescriptor;

class QueueReaderDescriptor
  {
  public:
    int TaskNo;
    unsigned DatagramQueueReaderAddress;
    QueueReaderDescriptor()
      {
      TaskNo = -1;
      DatagramQueueReaderAddress = NULL;
      }
    Initialized() volatile
      {
      return( (TaskNo != -1) && (DatagramQueueReaderAddress != NULL) );
      }
  };


template<class ItemType>
class DatagramQueueReader
  {
  public:
    CACHE_PADDING(reader_pad);
    SharedArea *mSharedArea;
    unsigned    mDepthMask;
    unsigned    mSynchMask;

    unsigned    mReaders_StatusWord;

    unsigned    mReaders_CachedPutCnt;
    unsigned    mReaders_CachedGotCnt;
    unsigned    mFlushedValue_Readers_CachedGotCnt;

    TraceClient BeforeBlockingPullReadMessageBuffer ;
    TraceClient AfterBlockingPullReadMessageBuffer ;
    TraceClient BeforeSleepInBlockingPullReadMessageBuffer ;
    TraceClient AfterSleepInBlockingPullReadMessageBuffer ;

  public:
    DatagramQueueReader()
      {
      Reset();
      }

    QueueReaderDescriptor
    Connect()
      {
      QueueReaderDescriptor Rc;
      Rc.TaskNo = 0;
      Rc.DatagramQueueReaderAddress = (unsigned) this ;
      return( Rc );
      };

    void
    Bind( SharedArea *aSharedArea )
      {
      mSharedArea = aSharedArea;
      mDepthMask  = mSharedArea->Depth()  - 1 ;
      mSynchMask = ( mSharedArea->Depth() - 1 ) >> 2 ;
      mReaders_StatusWord = 0;
      }
    Reset()
      {
      mSharedArea = NULL;
      mDepthMask  = 0;
      mSynchMask  = 0;

      mReaders_CachedPutCnt = 0;
      mReaders_CachedGotCnt = 0;
      mReaders_StatusWord |= 0x01;
      return(0);
      }

    int
    Suspend()
      {
      // The following assert catches duplicate suspends.
      assert( ! ( mReaders_StatusWord & 0x02) );
      mReaders_StatusWord |= 0x02;
      return(0);
      }

    int
    Resume()
      {
      // The following assert catches resumes without suspends.
      // For now, we allow multiple resumes.  As a technicality, this is wrong.
      //      assert( Readers_StatusWord & 0x02 );
      mReaders_StatusWord &= (0xFFFFFFFF ^ 0x02);
      return(0);
      }

    ItemType *
    MsgBufPtr()
      {
      mSharedArea->CheckIndexes();
      unsigned BufferIndex = mReaders_CachedGotCnt & mDepthMask;
      // Calculate the address of the Item using sizeof( ItemType ).
      // Note: if ItemType was force to const power of 2, << could be used.
      ItemType *ItemPtr =
        (ItemType *) (mSharedArea->mBuffer + (BufferIndex * Platform::Memory::Align(sizeof(ItemType))));
        BegLogLine(1)
        << "DatagramQueueReader::MsgBufPtr BufferIndex="
        << BufferIndex
        << " mBuffer=" << (void *)mSharedArea->mBuffer
        << " result=" << (void *) ItemPtr
        << EndLogLine ;
      //////cerr << " Getting from " << ItemPtr << endl ;
      return( ItemPtr );
      }


    ItemType *
    GetNextItem()
      {
      ItemType *ItemPtr = NULL;

      if( ! mReaders_StatusWord )
        {
        mSharedArea->CheckIndexes();

        if( mReaders_CachedGotCnt == mReaders_CachedPutCnt )
          {
          mReaders_CachedPutCnt = mSharedArea->PutCnt();
          }

        if( mReaders_CachedPutCnt > mReaders_CachedGotCnt )
          ItemPtr = MsgBufPtr();
        else
          FlushReleasedReadMessageBuffers();

        mSharedArea->CheckIndexes();
        }

      return( ItemPtr );
      }

    ReleaseItem()
      {
      mSharedArea->CheckIndexes();
      // This is not yet seen by the receiver... a PushFlush would be required.
      ++mReaders_CachedGotCnt;

      // ... however, every 1K, flush out to shared memory... but don't desched.
      if ( ( mReaders_CachedGotCnt & mSynchMask ) == 0 )
        {
        FlushReleasedReadMessageBuffers();
        }
      return(0);
      }

private:  // these don't seem to be used externally in the new world
    void
    ReleaseFlushReadMessageBuffer()
      {
      ++mReaders_CachedPutCnt;
      FlushReleasedReadMessageBuffers();
      }

    void FlushReleasedReadMessageBuffers()
      {
      if( mFlushedValue_Readers_CachedGotCnt != mReaders_CachedGotCnt )
        {
        mFlushedValue_Readers_CachedGotCnt = mReaders_CachedGotCnt;
        mSharedArea->GotCnt( mReaders_CachedGotCnt );
        }
      }


  };


template<class ItemType>
class DatagramQueueWriter
  {
  private:
    CACHE_PADDING(writer_pad);
    SharedArea *mSharedArea;

    unsigned    mSynchMask ;   // so that we can do bit-masking for this
    unsigned    mDepthMask ;  // and this

    unsigned    mWriters_CachedPutCnt;
    unsigned    mWriters_CachedGotCnt;
    TraceClient BeforeBlockingReserveWriteMessageBuffer ;
    TraceClient AfterBlockingReserveWriteMessageBuffer ;
    TraceClient BeforeSleepInBlockingReserveWriteMessageBuffer ;
    TraceClient AfterSleepInBlockingReserveWriteMessageBuffer ;

  public:
    DatagramQueueWriter()
      {
      Reset();
      }

    void
    Reset()
      {
      mSharedArea = NULL;
      mSynchMask  = 0 ;   // so that we can do bit-masking for this
      mDepthMask  = 0;  // and this

      mWriters_CachedPutCnt = 0;
      mWriters_CachedGotCnt = 0;
      }

    Bind( QueueReaderDescriptor aReaderDes,
          int                   aDepth = DEFAULT_CHAN_DEPTH )
      {
      unsigned int realDepth = RealDepth(aDepth) ;
      mSynchMask = ( realDepth - 1 ) >> 2 ;
      mDepthMask = realDepth - 1 ;

      mSharedArea = new SharedArea( sizeof( ItemType ), realDepth );
      assert( mSharedArea );

      ((DatagramQueueReader<ItemType> *)(aReaderDes.DatagramQueueReaderAddress))->Bind( mSharedArea );

      return( 0 );
      }

    ItemType *
    MsgBufPtr()
      {
      mSharedArea->CheckIndexes();
      unsigned BufferIndex = mWriters_CachedPutCnt & mDepthMask;
      // Calculate the address of the Item using sizeof( ItemType ).
      // Note: if ItemType was force to const power of 2, << could be used.
      ItemType *ItemPtr =
        (ItemType *) (mSharedArea->mBuffer + (BufferIndex * Platform::Memory::Align(sizeof(ItemType))));
        BegLogLine(1)
        << "DatagramQueueWriter::MsgBufPtr BufferIndex="
        << BufferIndex
        << " mBuffer=" << (void *)mSharedArea->mBuffer
        << " result=" << (void *) ItemPtr
        << EndLogLine ;
      /////cerr << " Putting to " << ItemPtr << endl ;
      return( ItemPtr );
      }

    ItemType *
    ReserveMsgBuf()
      {
      mSharedArea->CheckIndexes();
      ItemType *RetMessageBuf = NULL;

      // If needs be, fetch for new queue control values.
      // Leave got count alone until we need actually need
      // a new value to write into queue.
      if( (mWriters_CachedPutCnt - mWriters_CachedGotCnt) == mSharedArea->Depth() )
        {
        mWriters_CachedGotCnt = mSharedArea->GotCnt();
        }

      if( (mWriters_CachedPutCnt - mWriters_CachedGotCnt) < mSharedArea->Depth() )
        RetMessageBuf = MsgBufPtr();
      else
        FlushMsgBuf();

      return( RetMessageBuf );
      }

    ItemType *
    BlockingReserveMsgBuf()
      {
      ItemType *Item;
      int       PollCount = 0;
      while( NULL == ( Item = ReserveMsgBuf() ) )
        {
        PollCount++;
        if( PollCount < 64 )
          pthread_yield();
        else
          usleep( 100000 );
        }
      return( Item );
      }

    void
    PushMsgBuf()
      {
      mSharedArea->CheckIndexes();
      // This is not yet seen by the receiver... a PushFlush would be required.
      ++mWriters_CachedPutCnt;
      // Flush buffer every so often, based on a bit mask.
      if( 0 == (mWriters_CachedPutCnt & mSynchMask) )
          FlushMsgBuf();
      }

    void
    PushFlushMsgBuf()
      {
      mSharedArea->CheckIndexes();
      ++mWriters_CachedPutCnt;
      FlushMsgBuf();
      }

    void FlushMsgBuf()
      {
      mSharedArea->CheckIndexes();
      mSharedArea->PutCnt( mWriters_CachedPutCnt );
      }

  };

//======================================================================
#if 0
template<class ItemType>
class SRSW_Queue
  {

    SRSW_Queue()
      {
      }

    unsigned RealDepth(unsigned aDepth)
      {
      // Round up all queue depths to the next higher power of 2
      // because then we can do some arithmetic by bit-masking
      // rather then by division/modulus.
      unsigned int rd = 1 ;
      for (int x = 0 ; x < 32 ; x += 1)
        {
        if (rd >= aDepth)
          return rd ;
        rd <<= 1 ;
        } /* endfor */
      // You could get here if someone tried to configure a queue with
      // more than 2**31 elements; but this is not sensible for other
      // reasons, so a 'firewall' trap is OK.
      assert(0) ;
      return 1024 ;
      }

    // Init is called from the Put side.
    Init(char *aName, int aIndex, unsigned aDepth) SMP_SHARED
      {
      Writers_StatusWord = Readers_StatusWord = 1;

      unsigned int realDepth = RealDepth(aDepth) ;
      SynchMask = ( realDepth - 1 ) >> 2 ;
      DepthMask = realDepth - 1 ;

      BegLogLine( PKFXLOG_SRSWQ )
        << " SRSWQ:"
        << " Name: "
        << aName
        << " Depth "
        << Depth
        << " Depth base 2 "
        << realDepth
        << " Item size "
        << sizeof(ItemType)
        << EndLogLine;

      strncpy( (char *)((void *)Name), aName, sizeof( Name ) );

//    Index = aIndex;
      Writers_CachedPutCnt = Readers_CachedPutCnt = mSharedArea->PutCnt() = 0;
      Writers_CachedGotCnt = Readers_CachedGotCnt = mSharedArea->GotCnt() = 0;
//    Depth  = Depth  = Depth  = realDepth;
      Depth = realDepth;

      QueueItemArray = new QueueItem[ realDepth ];
      assert( QueueItemArray != NULL );

      for( int i = 0; i < realDepth; i++ )
        QueueItemArray[i].Init();

      Writers_StatusWord = Readers_StatusWord = 0;

      return(0);
      }


    int
    Uninit()
      {
      assert( ! ( Readers_StatusWord & 0x01) );

      Readers_StatusWord |= 0x01;
      Writers_StatusWord |= 0x01;

      delete QueueItemArray;

      return(0);
      }



// QUEUE READER METHODS

  };

#endif


#endif  // Attic below this line.

#if 0
    ItemType *
    BlockingPullReadMessageBuffer()
      {
      ItemType *CurrentMessageBuf;
      TraceBit Tracing = TraceEnabled(Development, ModuleTrace::SRSW_Queue) ;

      CurrentMessageBuf = PullReadMessageBuffer() ;

      if (NULL == CurrentMessageBuf)
        {
        BeforeBlockingPullReadMessageBuffer.HitOE
            ( Tracing, SRSW_Queue,Name, (int) this,
              BeforeBlockingPullReadMessageBuffer ) ;

        int WaitCount = 0;
        int SleepCount = 0;
        do
          {
          if( WaitCount )
            {
            if( WaitCount < SRS_SLEEP_COUNT )
              pthread_yield();
            else
              {
              if( SleepCount == 0 )
                {
                BeforeSleepInBlockingPullReadMessageBuffer.HitOE
                    ( Tracing, SRSW_Queue,Name, (int) this,
                      BeforeSleepInBlockingPullReadMessageBuffer ) ;
                }
              SleepCount++;
              USLEEP( 100000 );
              }
            }
          WaitCount++;
          } while ( NULL == (CurrentMessageBuf = PullReadMessageBuffer()) ); /* enddo */

        if( SleepCount )
          {
          AfterSleepInBlockingPullReadMessageBuffer.HitOE
              ( Tracing, SRSW_Queue,Name, (int) this,
                AfterSleepInBlockingPullReadMessageBuffer );
          }
        AfterBlockingPullReadMessageBuffer.HitOE
            ( Tracing, SRSW_Queue,Name, (int) this,
              AfterBlockingPullReadMessageBuffer );
        } /* endif */


      return( CurrentMessageBuf );
      }
#endif

#if 0
    ItemType *
    BlockingReserveWriteMessageBuffer()
      {
      ItemType *CurrentMessageBuf;
      TraceBit Tracing = TraceEnabled(Development, ModuleTrace::SRSW_Queue) ;

      CurrentMessageBuf = ReserveWriteMessageBuffer() ;
      if( CurrentMessageBuf == NULL )
        {
        BeforeBlockingReserveWriteMessageBuffer.HitOE
            ( 0, SRSW_Queue,Name, (int) this,
              BeforeBlockingReserveWriteMessageBuffer );

        int WaitCount = 0;
        int SleepCount = 0;
        do
          {
          if( WaitCount )
            {
            if( WaitCount < SRS_SLEEP_COUNT )
              pthread_yield();
            else
              {
              if( SleepCount == 0 );
                {
                BeforeSleepInBlockingReserveWriteMessageBuffer.HitOE
                    ( 1, SRSW_Queue,Name, (int) this,
                      BeforeSleepInBlockingReserveWriteMessageBuffer );
                }
              SleepCount++;
              USLEEP( 100000 );
              }
            }
          WaitCount++;
          } while ( ( CurrentMessageBuf = ReserveWriteMessageBuffer()) == NULL ); /* enddo */

        if( SleepCount )
          {
          AfterSleepInBlockingReserveWriteMessageBuffer.HitOE
              ( 1, SRSW_Queue,Name, (int) this,
                AfterSleepInBlockingReserveWriteMessageBuffer );
          }
        AfterBlockingReserveWriteMessageBuffer.HitOE
            ( 0, SRSW_Queue,Name, (int) this,
              AfterBlockingReserveWriteMessageBuffer ) ;
        } /* endif */

      return( CurrentMessageBuf );
      }
#endif

