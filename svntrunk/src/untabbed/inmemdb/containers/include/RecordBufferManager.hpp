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
 #ifndef __INCLUDE_RECORD_BUFFER_MANAGER_HPP__
#define __INCLUDE_RECORD_BUFFER_MANAGER_HPP__

#include <Pk/API.hpp>
#include <algorithm>

#ifndef RECORD_BUFFER_LOGGING
#define RECORD_BUFFER_LOGGING ( 0 )
#endif

#define DEFAULT_RECORD_COUNT ( 10000 )

template< class Record_T, class RecordCompare >
class RecordBufferManager
  {
  // typedef std::vector<Record_T> BufferType;    
  typedef Record_T* BufferType;


  typedef RecordCompare MyRecCompare;
      
  BufferType            mRecordBuffer;
  int                   mRecordBufferCount;
  int                   mRecordBufferAllocatedCount;

  typedef Record_T* Record_T_Ptr;

  // typedef std::stack<Record_T_Ptr, std::deque< Record_T_Ptr, inmemdb::BGL_Alloc< Record_T_Ptr > > > StackType;

  Record_T *     mFreeMemoryStackFirst;
  Record_T *     mFreeMemoryStackLast;

  unsigned int            mReservedRecordCount;
  unsigned int            mMaxRecordsInPacket;
  unsigned int            mOccupancyCount;
  unsigned int            mNodeCount;    

  void
  ExpandRecordBuffer()
    {
    StrongAssertLogLine( 0 )
      << EndLogLine;

    void * Memory;
    
    int NewCount = mRecordBufferAllocatedCount + DEFAULT_RECORD_COUNT;
    
    int NewSize = NewCount * sizeof( Record_T );
    
    Memory = PkHeapAllocate( NewSize );
    
    if( mRecordBuffer != NULL )
      {
      memcpy( Memory, mRecordBuffer, mRecordBufferAllocatedCount * sizeof( Record_T ));
      PkHeapFree( mRecordBuffer );
      }
    
    mRecordBuffer = (Record_T *) Memory;
    mRecordBufferAllocatedCount = NewCount;
    }    

  public:

   class iterator
    {
      private:
        //typedef bidirectional_iterator_tag iterator_category;
        typedef Record_T value_type;
        typedef value_type & reference;
        typedef value_type * pointer;

        typedef random_access_iterator_tag iterator_category;

      public:
        typedef BufferType IterType;
        IterType itr; 

        iterator(IterType new_itr) 
          {
          itr = new_itr;
          }

        reference operator*() const   
          {
          return (reference) (*itr);
          }
        
        pointer operator->()
          {
          return itr;
          }
        
        iterator& operator++() 
          {
          ++itr;
          return (*this);
          }
        
        iterator& operator--() 
          {
          --itr;
          return (*this);
          }

        iterator operator-(int n) 
           {
           iterator rc = this->itr - n;
           return( rc );
           }

        iterator operator+(int n) 
           {
           iterator rc = this->itr + n;
           return( rc );
           }

        iterator operator++(int) 
          {
          iterator prev_val = *this;
          ++*this;
          return (prev_val);
          }

        iterator operator--(int) 
          {
          iterator prev_val = *this;
          --*this;
          return (prev_val);
          }

        bool operator==(const iterator &eq_itr) const 
          {
          return (itr == eq_itr.itr);
          }

        bool operator!=(const iterator &eq_itr) const 
          {
          return (!(itr == eq_itr.itr));
          }        

        bool operator<(const iterator &eq_itr) const 
          {
          return (itr < eq_itr.itr);
          }        

        bool operator>(const iterator &eq_itr) const 
          {
          return (itr > eq_itr.itr);
          }        

        bool operator<=(const iterator &eq_itr) const 
          {
          return (itr <= eq_itr.itr);
          }        

        bool operator>=(const iterator &eq_itr) const 
          {
          return (itr >= eq_itr.itr);
          }        
    };

  iterator begin()
    {
    return iterator( &mRecordBuffer[ 0 ] );
    }

  iterator end()
    {
    return iterator( &mRecordBuffer[ mRecordBufferCount ] );
    }

  bool empty() const
    {
    return (mRecordBufferCount == 0);
    }

  void clear() 
    {
    mRecordBufferCount = 0;
    }
  
  int size() const
    {
    return mRecordBufferCount;
    }

  // The total allocated space
  unsigned int capacity() const
    {
    return mRecordBufferAllocatedCount;
    }

  // Available space to put records in
  // Do not put records into the initially reserved area
  // Algorithm assumes that there's a file 
  unsigned int available_capacity() const
    {
    AssertLogLine( mReservedRecordCount >= (mMaxRecordsInPacket * mNodeCount) )
      << "available_capacity::Error:: "
      << " mReservedRecordCount: " << mReservedRecordCount
      << " mMaxRecordsInPacket: " << mMaxRecordsInPacket
      << " mNodeCount: " << mNodeCount
      << EndLogLine;

    return mRecordBufferAllocatedCount - mReservedRecordCount;
    }

  void reserve(int aCount ) 
    {
    if( mRecordBuffer != NULL )
      {
      PkHeapFree( mRecordBuffer );
      mRecordBuffer = NULL;
      }
    
    int RecordBufferAllocatedSize = sizeof( Record_T ) * aCount;
    mRecordBuffer = (Record_T *) PkHeapAllocate( RecordBufferAllocatedSize );
    
    StrongAssertLogLine( mRecordBuffer != NULL )
      << "Table::reserve:: Not enough memory for buffer "
      << EndLogLine;

    mRecordBufferAllocatedCount = aCount;

    BegLogLine( RECORD_BUFFER_LOGGING )
      << "RecordBufferManager::Init:: "
      << " RecordBufferAllocatedSize: " << RecordBufferAllocatedSize
      << " mRecordBufferAllocatedCount: " << mRecordBufferAllocatedCount
      << " mRecordBuffer: " << (void *) mRecordBuffer
      << EndLogLine;
    }
 
  unsigned int
  occupancy()
    {
    return mOccupancyCount;
    }

  unsigned int
  GetReasonableRecordReservation()
    {
    unsigned int Capacity  = capacity();
    unsigned int Occupancy = occupancy();

    AssertLogLine( mReservedRecordCount + Occupancy <= Capacity )
      << "GetReasonableRecordReservation:: ERROR:: "
      << " mReservedRecordCount: " << mReservedRecordCount
      << " Occupancy: " << Occupancy
      << " Capacity: " << Capacity
      << EndLogLine
      
    unsigned int FreeRecordCount = Capacity - mReservedRecordCount - Occupancy;
        
    unsigned int ReasonableReservedAmount = (FreeRecordCount >> 1);
    
#if 0
    unsigned int rc = ( ReasonableReservedAmount == 0 ) ? 1 : ReasonableReservedAmount;
    mReservedRecordCount += rc;
#else
    // For now allocate 
    unsigned int rc = mMaxRecordsInPacket;
#endif

    return rc;
    }

  void
  ReleaseReservedRecords( unsigned int aRecordCount )
   {
   mReservedRecordCount -= aRecordCount;
   }

  void push_back( Record_T& aRec )
    {
    AssertLogLine( size() < capacity() )
      << "push_back:: Error:: "
      << " size(): " << size()
      << " capacity(): " << capacity()
      << EndLogLine;

    memcpy( mRecordBuffer[ mRecordBufferCount ] , &aRec, sizeof( Record_T ));
    // mRecordBuffer[ mRecordBufferCount ].SetActive();
    mRecordBufferCount++;
    }

   Record_T* insert( const typename Record_T::KeyType&   aRecKey )
    {
    mOccupancyCount++;
    
    Record_T* rc = NULL;

    if( ! IsFreePoolEmpty() )
      {
      rc = GetFromFreePool();
      rc->Init( aRecKey );
      }
    else
      {
      AssertLogLine( size() < capacity() )
        << "insert:: Error:: "
        << " size(): " << size()
        << " capacity(): " << capacity()
        << EndLogLine;
      
      rc = end().itr;
      
      StrongAssertLogLine( rc != NULL )
  << "ERROR:: In insert(): Not Enough Space for: "
  << " size(): " << size()
  << " capacity(): " << capacity()
  << " Key: " << aRecKey
  << EndLogLine;

      rc->Init( aRecKey );
      mRecordBufferCount++;
      }

    // rc->SetActive();

    return rc;
    }

   Record_T* insert( const typename Record_T::KeyType&   aRecKey, 
                     const typename Record_T::TupleType& aRecTuple )
    {
    mOccupancyCount++;
    
    Record_T* rc = NULL;

    if( ! IsFreePoolEmpty() )
      {
      rc = GetFromFreePool();
      rc->Init( aRecKey, aRecTuple );
      }
    else
      {
      AssertLogLine( size() < capacity() )
        << "insert:: Error:: "
        << " size(): " << size()
        << " capacity(): " << capacity()
        << EndLogLine;
      
      rc = end().itr;
      
      StrongAssertLogLine( rc != NULL )
  << "ERROR:: In insert(): Not Enough Space for: "
  << " size(): " << size()
  << " capacity(): " << capacity()
  << " Key: " << aRecKey
  << EndLogLine;

      rc->Init( aRecKey, aRecTuple );
      mRecordBufferCount++;
      }

    // rc->SetActive();

    return rc;
    }

#if 0
  Record_T* insert( Record_T* aRecPtr, Record_T& aRec )
    {
    AssertLogLine( aRecPtr != NULL )
      << "RecordBufferManager::insert:: Error:: "
      << " aRecPtr: " << (void *) aRecPtr
      EndLogLine;
    
    memcpy( aRecPtr, &aRec, sizeof( Record_T ) );

    // aRecPtr->SetActive();
    return aRecPtr;
    }
      
  Record_T* insert( Record_T& aRec )
    {
    if( ! IsFreePoolEmpty() )
      {
      Record_T* RecPtr = GetFromFreePool();
      return insert( RecPtr, aRec );
      }
    else
      {
      return push_back( aRec );
      }
    }

  Record_T* insert( Record_T* aRec )
    {
    return insert( *aRec );    
    }
#endif
    
  iterator find( const Record_T& aKey )
    {
    return std::find( begin(),
                      end(),
                      aKey );
    }
  
  void erase ( const Record_T& aKey )
    {
    iterator iter = find( aKey );
    
    if( iter != end() )
      {
      iter->SetNonActive();
      compress();
      }
    }

  void sort()
    {
    std::sort( begin().itr, end().itr );
    }

  void compress()
    {
    iterator iter_begin  = begin();
    iterator iter_end    = end();
    
    iterator EmptySlot    = begin();
    iterator ActiveSlot = end();
    
    ActiveSlot--;

    int TotalCount = 0;

    while( 1 )
      {
      // Find the next empty slot
      while( EmptySlot <= ActiveSlot )
        {
        if( EmptySlot->IsNonActive() )
          break;
        
        EmptySlot++;
        TotalCount++;
        }
      
      // Start from back find the next active slot
      // Move the element over
      while( ActiveSlot >= EmptySlot )
        {
        if( ActiveSlot->IsActive() )
          break;
        
        ActiveSlot--;
        }
      
      // if EmptySearch goes past Active search halt
      if( EmptySlot < ActiveSlot )
        {
        memcpy( (char *) EmptySlot.itr, 
                (char *) ActiveSlot.itr, 
                sizeof( Record_T ) );

        ActiveSlot->SetNonActive();
        EmptySlot->SetActive();

        EmptySlot++;
        ActiveSlot--;
        TotalCount++;
        }
      else
        break;
      }
    
    // Decrease the number of elements in the buffer
    // by the number of empty slots filled in
    mRecordBufferCount = TotalCount;

    #ifndef NDEBUG
    int DebugCount = 0;
    iterator iter = iter_begin;
    while( iter != iter_end )
      {
      if( iter < end() )
        {
        StrongAssertLogLine( iter->IsActive() )
          << "compress::ERROR:: Should not find gaps after compression"
          << " iter->mNext: " << (void *) iter->mNext
          << EndLogLine;
        }
      else if( iter >= end() && iter < iter_end )
        {
        StrongAssertLogLine( iter->IsNonActive() )
          << "compress::ERROR:: Should not find gaps after compression"
          << " iter->mNext: " << (void *) iter->mNext
          << EndLogLine;      
        }
      
      if( iter->IsActive() )
        {
        DebugCount++;
        }
      
      iter++;
      }

    StrongAssertLogLine( DebugCount == mRecordBufferCount )
      << "compress::ERROR:: "
      << " DebugCount: " << DebugCount
      << " mRecordBufferCount: " << mRecordBufferCount
      << EndLogLine;
    #endif
    }

  RecordBufferManager() 
    {
    }

  void    
  Init( int aMaxNumberOfRecordsInPacket, int aNodeCount, int aRecordCount = DEFAULT_RECORD_COUNT )
    {
    mRecordBufferCount = 0;
    mRecordBufferAllocatedCount = 0;
    mRecordBuffer = NULL;

    mOccupancyCount     = 0;
    mMaxRecordsInPacket = aMaxNumberOfRecordsInPacket;
    
    mNodeCount = aNodeCount;

    // Initially there's at LEAST room for one packet full from each node
    mReservedRecordCount = mMaxRecordsInPacket * aNodeCount;    

    reserve( aRecordCount );

//     inmemdb::BGL_Alloc< StackType > alloc;
//     mFreeMemoryStack = (StackType *) alloc.allocate( 1 );
//     alloc.construct( mFreeMemoryStack, StackType() );
    mFreeMemoryStackFirst = NULL;
    mFreeMemoryStackLast = NULL;
    }

#if 0      
  Record_T*
  AddRecord( const typename Record_T::RecKeyType& aKey, const typename Record_T::RecTupleType& aTuple )
    { 
    Record_T* RecPtr = NULL;
    if( !mFreeMemoryStack->empty() )
      {
      RecPtr = GetFromFreePool();
      }
    else
      {
      if( mRecordBufferCount + 1 >= mRecordBufferAllocatedCount )
        {
        // ExpandRecordBuffer();
        StrongAssertLogLine( 1 )
          << "RecordBufferManager::AddRecord:: Error:: Buffer is not large enough "
          << EndLogLine;
        }
      
      // memcpy( &mRecordBuffer[ mRecordBufferCount ], aRecord, sizeof( Record ) );
      RecPtr = &mRecordBuffer[ mRecordBufferCount ];
      mRecordBufferCount++;
      }
    
    RecPtr->Init( aKey, aTuple );
    
    return RecPtr;
    }
#endif
      
#if 0      
  void
  AddRecord( Record_T* aRecord )
    {
    if( mRecordBufferCount + 1 >= mRecordBufferAllocatedCount )
      ExpandRecordBuffer();
    
    memcpy( &mRecordBuffer[ mRecordBufferCount ], aRecord, sizeof( Record_T ) );
    mRecordBufferCount++;
    }
#endif
      
  Record_T* 
  GetRecordBufferStart()
    {
    return mRecordBuffer;
    }
      
  int 
  GetRecordBufferCount()
    {
    return mRecordBufferCount;
    }
      
  bool
  IsFreePoolEmpty( )
   {
   return( mFreeMemoryStackFirst == NULL );
   }

  void
  AddToFreePool( Record_T* aRecord )
    {    
    AssertLogLine( mOccupancyCount > 0 )
      << "AddToFreePool:: Error:: "
      << " mOccupancyCount: " << mOccupancyCount
      << EndLogLine;

    // mFreeMemoryStack->push( aRecord );
    if( mFreeMemoryStackFirst == NULL )
      {
      mFreeMemoryStackFirst = aRecord;
      mFreeMemoryStackLast = mFreeMemoryStackFirst;
      mFreeMemoryStackLast->mNext = NULL;
      }
    else
      {
      mFreeMemoryStackLast->mNext = aRecord;
      mFreeMemoryStackLast = mFreeMemoryStackLast->mNext;
      mFreeMemoryStackLast->mNext = NULL;
      }
    
    mOccupancyCount--;
    }

  Record_T*
  GetFromFreePool()
    {
//     Record_T* rc = mFreeMemoryStack->top();
//     mFreeMemoryStack->pop();
    AssertLogLine( !IsFreePoolEmpty() )
      << "GetFromFreePool:: Error:: Free pool is empty "
      << EndLogLine;

    Record_T * rc = mFreeMemoryStackFirst;

    // Pop off the stack
    mFreeMemoryStackFirst = mFreeMemoryStackFirst->mNext;

    return rc;
    }
  };
#endif
