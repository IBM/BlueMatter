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
 // ***********************************************************************
// File: Table.hpp
// Author: arayshu / cwo 
// Date: August 15, 2006 
// Namespace: inmemdb
// Class: Table 
// Description: Container that is represents a table --
//              satisfies Container 'concept' for templating
//              (same as QueryResult.hpp)
// ***********************************************************************
#ifndef __INMEMDB_TABLE_HPP__
#define __INMEMDB_TABLE_HPP__

#define  RECORD_ACTIVE     ( 0xffffeeee )
#define  RECORD_NON_ACTIVE ( 0xeeeeffff )

#include <map>
#include <stack>
#include <set>
#include <algorithm>
#include <spi/tree_reduce.hpp>
#include <spi/tree_reduce.cpp>
#include <inmemdb/utils.hpp>
#include <inmemdb/RecordBufferManager.hpp>
#include <inmemdb/RandomNodeList.hpp>

#ifdef FLOW_CONTROL
#include <inmemdb/RecordCommunicationManager_flow_control.hpp>
#else
#include <inmemdb/RecordCommunicationManager.hpp>
#endif

#include <inmemdb/types.hpp>
#include <inmemdb/NamedBarrier.hpp>
#include <inmemdb/NamedAllReduce.hpp>

TraceClient SortStart;
TraceClient SortFinis;

TraceClient HistogramStart;
TraceClient HistogramFinis;

TraceClient RedistributionStart;
TraceClient RedistributionFinis;

TraceClient RedistributionInsertStart;
TraceClient RedistributionInsertFinis;

TraceClient RedistributionCommStart;
TraceClient RedistributionCommFinis;

#ifndef TRACE_SORT 
#define TRACE_SORT ( 1 )
#endif

#define BIN_COUNT_FACTOR               ( 30 )
#define TARGET_RECORDS_PER_NODE_FACTOR ( 1.0 )

#ifndef TABLE_LOGGING
#define TABLE_LOGGING ( 0 )
#endif

#ifndef TABLE_SORT_LOGGING
#define TABLE_SORT_LOGGING ( 1 )
#endif

#ifndef TABLE_REDISTRIBUTION_LOGGING
#define TABLE_REDISTRIBUTION_LOGGING ( 1 )
#endif

struct HistogramBin
  {
  unsigned long mCount;
  };

namespace inmemdb
{    
    template< class TypeInfo >
class Table
  {
  public:
    typedef Table<TypeInfo>                  TableT;  

    typedef typename TypeInfo::Key            Key;
    typedef typename TypeInfo::Tuple          Tuple;
      
//    typedef Key*                              KeyValType;
    typedef Tuple*                            TupleValType;


    typedef typename TypeInfo::KeyCompare     KeyCompare;
    typedef TypeInfo                          TypeInfoT;
      
        
    struct PureRecord
      {          
      Key     mKey;
          // Tuple   mTuple;
      };

   class Record
   {
   public:
    Key     mKey;
    // Tuple   mTuple;
    Record* mNext;
    
    typedef Key   KeyType; 
    typedef Tuple TupleType; 
    
    Record () {}    
    
    bool operator<(const Record& aRec ) const
      {
      // KeyCompare kc;
      return mKey < aRec.mKey;
      }
    
    bool operator<=(const Record& aRec ) const
        {
        return mKey <= aRec.mKey;
        }
    
    void
    Init( Key aKey )
        {
        mNext  = NULL;
        mKey   = aKey;
        // mTuple = aTuple;
        }

    void
    Init( Key aKey, Tuple aTuple )
        {
        mNext  = NULL;
        mKey   = aKey;
        // mTuple = aTuple;
        }
    
      void
      SetKey( Key aKey )
        {
        mKey = aKey;
        }
      
      void
      SetActive()
        {
        //mNext = (Record *) RECORD_ACTIVE;        
        }

      void
      SetNonActive()
        {
        // mNext = (Record *) RECORD_NON_ACTIVE;
        
        #ifndef NDEBUG
        StrongAssertLogLine( sizeof( Key ) > sizeof( unsigned int ))
          << "Table::Record::SetNonActive():: Error:: "
          << " sizeof( Key ): " << sizeof( Key )
          << " sizeof( unsigned int ): " << sizeof( unsigned int )
          << EndLogLine;
        #endif

        unsigned int * KeyPtr = (unsigned int *) &mKey;
        KeyPtr[ 0 ] = RECORD_NON_ACTIVE;
        }

      int
      IsActive()
        {
        // return ( ((unsigned) mNext) == (unsigned) RECORD_ACTIVE );
        return !( IsNonActive() );
        }

      int
      IsNonActive()
        {
        // return ( ((unsigned) mNext) == (unsigned) RECORD_NON_ACTIVE );
        unsigned int * KeyPtr = (unsigned int *) &mKey;
        return ( KeyPtr[ 0 ] == (unsigned int) RECORD_NON_ACTIVE );
        }
      };

    typedef Record* RecordPtr;

   class RecordPtrCompare
     {
     public:
     bool 
     operator()( const RecordPtr &k1, const RecordPtr& k2 ) const
       {
       KeyCompare kc;
       return kc( k1->mKey, k2->mKey );
       }
     };

   class RecordCompare
     {
     public:
     bool 
     operator()( const Record &k1, const Record& k2 ) const
       {
       KeyCompare kc;
       return kc( k1.mKey, k2.mKey );
       }
     };

    typedef RecordPtr KeyValueType;

   // typedef std::set < KeyValueType, RecordPtrCompare, inmemdb::BGL_Alloc< KeyValueType > > TableType;

   typedef RecordBufferManager< Record, RecordCompare > TableType;   

   // typedef std::pair< KeyValType, TupleValType >         KeyTuplePair;
   // typedef inmemdb::BGL_Alloc< KeyTuplePair >             KeyTupleAlloc;
   //   typedef std::map< KeyValType, TupleValType, KeyCompare, KeyTupleAlloc >  TableType;
       
    union KeyRepAsLongLong
      {
      unsigned long long  mVal;
      char                mChars[ 8 ];
      };

   class HistogramBinAssignment
     {
     public:
     int           mNodeId;
   
     Record*       mFirstRec;
     Record*       mLastRec;
  
     HistogramBinAssignment() 
       {
       mNodeId = -1;
   
       mFirstRec = NULL;
       mLastRec  = NULL;
       }

     void
     AddRecord( Record * aRec )
       {
       if( mFirstRec == NULL )
         {
         mFirstRec = aRec;
         mLastRec = aRec;
         }
       else
         {
         mLastRec->mNext = aRec;
         mLastRec = aRec;
         }

       mLastRec->mNext = NULL;
       }
     };

  struct CheckSortPacket
    {
    GPID mGPID;
    int  mKeyInPacketIsLargestFromNode;
    Key  mKey;
    };

  int CheckSortPacketsSent;    
  int CheckSortPacketsRecv;    
 
  inline
  static
  int 
  CheckSortCompareFx( void* arg )
    {
    CheckSortPacket* Pkt = (CheckSortPacket *) arg;
    
    GPID gpid = Pkt->mGPID;
    
    TableT* Table = GlobalParallelObject::Client::GetObjectForGPID<TableT>( gpid );

    Key InPacket = Pkt->mKey;

    unsigned long SourceNodeId;
    unsigned long SourceCoreId;
    pkFiberControlBlockT* Context = NULL;
    PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );       

    if( Pkt->mKeyInPacketIsLargestFromNode )
      {
      // Compare with the smallest key on this node
      Key SmallestKey = Table->mRecordBufferManager.begin()->mKey;
          
      StrongAssertLogLine( (PkNodeGetId()-1) == SourceNodeId )
        << "CheckSortCompareFx: ERROR:: "
        << " PkNodeGetId(): " << PkNodeGetId()
        << " SourceNodeId: " << SourceNodeId
        << EndLogLine;
      

      StrongAssertLogLine( InPacket < SmallestKey )
        << "CheckSortCompareFx: ERROR:: "
        << " Smallest key on this node is not larger then the largest "
        << " key on the smaller node id "
        << EndLogLine;
      }
    else
      {
      // Compare with the largest key on this node
      // Compare with the smallest key on this node
      Key LargestKey = (Table->mRecordBufferManager.end()-1)->mKey;

      StrongAssertLogLine( (PkNodeGetId()+1) == SourceNodeId )
        << "CheckSortCompareFx: ERROR:: "
        << " PkNodeGetId(): " << PkNodeGetId()
        << " SourceNodeId: " << SourceNodeId
        << EndLogLine;

      StrongAssertLogLine( LargestKey < InPacket )
        << "CheckSortCompareFx: ERROR:: "
        << " Largest key on this node is not smaller then the smallest "
        << " key on the larger node id "
        << EndLogLine;
      }

    Table->CheckSortPacketsRecv++;
    return NULL;
    }

  private:      

    TableType                                           mRecordBufferManager;
    int*                                                mRecordsToRecvForNode;  
    int*                                                mRecordsToSendForNode;

    const char*                                         mTableName;
    // TableType*                                       mTableContents;

    int                                                 mBinExpansionFactor;
    double                                              mTargetRecordsPerNodeFactor;

    inmemdb::BGL_Alloc< Tuple >                                      mTupleAlloc;
    inmemdb::BGL_Alloc< Record >                                     mRecordAlloc;
      
    // TupleCommunicationManager<Key, Tuple, TableType> mComm;
    RecordCommunicationManager<Record, Key, Tuple, TableType>  mComm;

    NamedBarrier                                        mMyBarrier;
    NamedAllReduce<unsigned int, NAMED_ALLREDUCE_SUM>   mMyAllReduce;
    NamedAllReduce<long long, NAMED_ALLREDUCE_SUM>      mMyAllReduceLongLong;

    // Number of nodes that would create this table
    int                                                 mNodeCount;
    GPID                                                mMyGPID;

    // Data structures needed for the sort  
    HistogramBin*                                       mLocalHistogram;
    HistogramBin*                                       mGlobalHistogram;
    HistogramBinAssignment*                             mHistogramBinAssignment;
    Record**                                            mRecordsForNode;  
    int                                                 mBinCount;
    RandomNodeList                                      mRandomNodeList;
    unsigned long long *                                mCountPerNode;

      
    class SortDistributionFunction
      {
      HistogramBinAssignment*      mBinAssignment;
      int                mBinCount;
      KeyRepAsLongLong   mMinVal;
      KeyRepAsLongLong   mMaxVal;
      unsigned long long mDelta;
      int                mCopyOffset;
      int                mSizeOfKeyToHistogram;
      int                mRecAssignedToThisNodeCount;  

    public:
      ~SortDistributionFunction()
         {
         }

      SortDistributionFunction( HistogramBinAssignment* aBinAssignment,
                                int aBinCount,
                                Key &aMinKey,
                                Key &aMaxKey,
                                int aRecAssignedToThisNodeCount )
        {
        mBinAssignment  = aBinAssignment;
        mBinCount       = aBinCount;
        
        mRecAssignedToThisNodeCount = aRecAssignedToThisNodeCount;

        mSizeOfKeyToHistogram = sizeof( Key );        
        if( mSizeOfKeyToHistogram > sizeof( unsigned long long) )
          mSizeOfKeyToHistogram = sizeof( unsigned long long);
        
        mCopyOffset = sizeof( unsigned long long) - mSizeOfKeyToHistogram;

        StrongAssertLogLine( mCopyOffset >= 0 && mCopyOffset < sizeof( unsigned long long) )
          << "ERROR:: " 
          << " sizeof( Key ): " << sizeof( Key )
          << " mCopyOffset: " << mCopyOffset
          << " mSizeOfKeyToHistogram: " << mSizeOfKeyToHistogram
          << EndLogLine;

        mMinVal.mVal = 0;
        memcpy( &( mMinVal.mChars[ mCopyOffset ] ), &aMinKey, mSizeOfKeyToHistogram );
        
        mMaxVal.mVal = 0;
        memcpy( &( mMaxVal.mChars[ mCopyOffset ] ), &aMaxKey, mSizeOfKeyToHistogram );

        mDelta = ( mMaxVal.mVal - mMinVal.mVal ) / static_cast<unsigned long long>( mBinCount - 1 );
        }
      
      unsigned int 
      GetAddressSpaceForKey( const Key& aKey )
        {
        KeyRepAsLongLong KeyRep;
        KeyRep.mVal = 0;
        
        memcpy( &KeyRep.mChars[ mCopyOffset ], &aKey, mSizeOfKeyToHistogram );
        
        int BinIndex = (KeyRep.mVal - mMinVal.mVal) / mDelta;
        BinIndex = BinIndex < 0 ? 0 : BinIndex;
        BinIndex = BinIndex > (mBinCount - 1) ? (mBinCount - 1) : BinIndex;
        
        int NodeId = mBinAssignment[ BinIndex ].mNodeId;
        
        return NodeId;
        }
          
      int
      GetTotalRecordsToReceiveCount( )
        {
        return mRecAssignedToThisNodeCount;
        }

    };
      
    //*************************************************************************
    // Private Constructor 
    //*************************************************************************
    Table (const char* aTableName, int aNodeCount) :
        mTableName(aTableName), mNodeCount(aNodeCount) 
        {}
 
    void 
    Init(const char* aTableName, 
         int aNodeCount,
         int aBinExpansionFactor=BIN_COUNT_FACTOR,
         double aTargetRecordsPerNodeFactor=TARGET_RECORDS_PER_NODE_FACTOR,
         int aRecordCount = DEFAULT_RECORD_COUNT
         )
      {
      mTableName                  = aTableName;
      mNodeCount                  = aNodeCount;
      mBinExpansionFactor         = aBinExpansionFactor;
      mTargetRecordsPerNodeFactor = aTargetRecordsPerNodeFactor;

//       inmemdb::BGL_Alloc< TableType > alloc;
      
//       mTableContents = (TableType *) alloc.allocate( 1 );
//       alloc.construct( mTableContents, TableType() );
      
      
      mRecordsToRecvForNode = (int *) PkHeapAllocate( aNodeCount * sizeof( int ) );
      mRecordsToSendForNode = (int *) PkHeapAllocate( aNodeCount * sizeof( int ) );

      mCountPerNode = (unsigned long long *) PkHeapAllocate( sizeof( unsigned long long ) * aNodeCount );

      mComm.Init( aNodeCount );           

      mRecordBufferManager.Init( mComm.GetMaxNumberOfRecordsInPacket(), aNodeCount, aRecordCount );

      unsigned int RecvTableGPID = (unsigned int) mComm.GetGPID();
      
      char GlobalBarrierName[ 32 ];
      sprintf( GlobalBarrierName, "GBN%d", RecvTableGPID);

      char GlobalReduceName[ 32 ];
      sprintf( GlobalReduceName, "GRN%d", RecvTableGPID);

      char GlobalReduceNameLL[ 32 ];
      sprintf( GlobalReduceNameLL, "GRLLN%d", RecvTableGPID);

      BegLogLine( TABLE_LOGGING )
        << "Table::Init():: About to call mMyBarrierInit()  "
        << " GlobalBarrierName: " << GlobalBarrierName
        << " GlobalReduceName: " << GlobalReduceName
        << " aNodeCount: " << aNodeCount
        << " RecvTableGPID: " << RecvTableGPID
        << " mMyBarrierPtr: " << (void *) & mMyBarrier
        << EndLogLine;

      mMyBarrier.Init(  GlobalBarrierName, aNodeCount );

      mBinCount = PkNodeGetCount() * mBinExpansionFactor;
      mLocalHistogram = (HistogramBin *) PkHeapAllocate( sizeof( HistogramBin ) * mBinCount );      
      mGlobalHistogram = (HistogramBin *) PkHeapAllocate( sizeof( HistogramBin ) * mBinCount );      
      mHistogramBinAssignment = (HistogramBinAssignment *) PkHeapAllocate( sizeof( HistogramBinAssignment ) * mBinCount );

      for( int i=0; i < mBinCount; i++ )
        {
        mHistogramBinAssignment[ i ].mNodeId             = -1;
        mHistogramBinAssignment[ i ].mFirstRec           = NULL;
        mHistogramBinAssignment[ i ].mLastRec            = NULL;
        }

      mRecordsForNode = (Record **) PkHeapAllocate( aNodeCount * sizeof( Record* ) );
      for( int i=0; i < aNodeCount; i++ )
        {
        mRecordsForNode[ i ] = NULL;
        }

      mRandomNodeList.Init( aNodeCount );
      mMyAllReduce.Init( GlobalReduceName,aNodeCount );

      mMyAllReduceLongLong.Init( GlobalReduceNameLL, aNodeCount );

      int DummyRoot;
      GlobalParallelObject::Client::RegisterGlobalParallelObject< TableT >( this,
                                                                            "TABAB",
                                                                            mNodeCount,
                                                                            mMyGPID,
                                                                            DummyRoot );

      }


      struct RecursiveBisectionElement
        {
        unsigned int mStartIndex;
        unsigned int mLen;
        unsigned long long mTotalWeight;
        };

      struct RecursiveBisectionList
        {
        RecursiveBisectionElement* mList;
        unsigned int               mCount;    
        };

      int
      PartitionKeysBasedOnHistogramRecursiveBisection( HistogramBin*             aHistogram, 
                                                       HistogramBinAssignment*   aHistogramBinAssignment,
                                                       int                       aBinCount, 
                                                       int                       aNodeCount,
                                                       unsigned long long        aNumRecords )
        {
        BegLogLine( TABLE_SORT_LOGGING )
          << "HistogramRecursiveBisection: "  
          << " Entering "
          << EndLogLine;

        // Partition the histogram into aNodeCount equal parts
        int StartIndex                      = 0;
        int EndIndex                        = aBinCount-1;
        unsigned long long TotalToBisect    = aNumRecords;

        RecursiveBisectionList EvenBisection;
        RecursiveBisectionList OddBisection;

        EvenBisection.mList  = (RecursiveBisectionElement *) PkHeapAllocate( sizeof( RecursiveBisectionElement ) * aNodeCount );
        EvenBisection.mCount = 0;

        OddBisection.mList  = (RecursiveBisectionElement *) PkHeapAllocate( sizeof( RecursiveBisectionElement ) * aNodeCount );
        OddBisection.mCount = 0;

        // Figure out the levels of recursion
        unsigned int LevelCount = 0;
        unsigned int NodeCount = 1;

        BegLogLine( TABLE_SORT_LOGGING )
          << "HistogramRecursiveBisection: "  
          << " NodeCount: " << NodeCount
          << " aNodeCount: " << aNodeCount
          << EndLogLine;

        unsigned int nc = aNodeCount;
        while( NodeCount < nc )
          {
          NodeCount = NodeCount << 1;
          LevelCount++;
          }

        BegLogLine( TABLE_SORT_LOGGING )
          << "HistogramRecursiveBisection: "  
          << " LevelCount: " << LevelCount
          << " NodeCount: " << NodeCount
          << EndLogLine;

        RecursiveBisectionList* ReadBisection = &EvenBisection;
        RecursiveBisectionList* WriteBisection = &OddBisection;

        ReadBisection->mList[ 0 ].mStartIndex  = 0;
        ReadBisection->mList[ 0 ].mLen         = aBinCount;
        ReadBisection->mList[ 0 ].mTotalWeight = aNumRecords;
        ReadBisection->mCount = 1;


        for( int level=0; level < LevelCount; level++ )
          {
          WriteBisection->mCount = 0;
          
          for( int i=0; i < ReadBisection->mCount; i++ )
            {
            int                StartIndex  = ReadBisection->mList[ i ].mStartIndex;
            int                Len         = ReadBisection->mList[ i ].mLen;
            
            StrongAssertLogLine( Len > 1 )
              << "PartitionKeysBasedOnHistogramRecursiveBisection:: Error:: "
              << " Can not partition bin: " << StartIndex
              << " Len: " << Len
              << " i: " << i
              << " ReadBisection->mCount: " << ReadBisection->mCount
              << " aBinCount: " << aBinCount
              << " level: "      << level 
              << " LevelCount: " << LevelCount
              << EndLogLine;
            
            unsigned long long TotalWeight = ReadBisection->mList[ i ].mTotalWeight;
            
            unsigned long long HalfWeight = TotalWeight >> 1;
            unsigned long long Count = 0;
            
            // Find the bisection
            for( int b=StartIndex; b < (StartIndex+Len); b++ )
              {
              // Count += aHistogram[ b ].mCount;
              
              if( Count + aHistogram[ b ].mCount >= HalfWeight )
                {
                int StartIndex1 = StartIndex;
                //int Len1        = b - StartIndex + 1;
                int Len1        = b - StartIndex;

                int StartIndex2 = StartIndex1 + Len1;
                int Len2        = Len - Len1;

                int WriteIndex = WriteBisection->mCount; 
                WriteBisection->mList[ WriteIndex ].mStartIndex  = StartIndex1;
                WriteBisection->mList[ WriteIndex ].mLen         = Len1;
                WriteBisection->mList[ WriteIndex ].mTotalWeight = Count;
                WriteBisection->mCount++;

                WriteIndex = WriteBisection->mCount; 
                WriteBisection->mList[ WriteIndex ].mStartIndex  = StartIndex2;
                WriteBisection->mList[ WriteIndex ].mLen         = Len2;
                WriteBisection->mList[ WriteIndex ].mTotalWeight = TotalWeight - Count;
                WriteBisection->mCount++;

                break;
                }
              
              Count += aHistogram[ b ].mCount;
              }
            }
          
          // Swap the read and the write count;
          RecursiveBisectionList* TmpPtr = ReadBisection;
          ReadBisection                  = WriteBisection;
          WriteBisection                 = TmpPtr;
          }
        
        StrongAssertLogLine( ReadBisection->mCount == aNodeCount )
          << "PartitionKeysBasedOnHistogramRecursiveBisection:: Error:: "
          << " ReadBisection->mCount: " << ReadBisection->mCount
          << " aNodeCount: " << aNodeCount
          << EndLogLine;
        
        BegLogLine( TABLE_SORT_LOGGING )
          << "HistogramRecursiveBisection: "  
          << " Bisection is finished"
          << EndLogLine;

        // Create the assignment and setup the bined linked lists
        unsigned long long RecAssignedToMeCount = 0;
        unsigned int RecCapacity = mRecordBufferManager.available_capacity();

        for( int nodeId = 0; nodeId < ReadBisection->mCount; nodeId++ )
          {
          unsigned int StartIndex  = ReadBisection->mList[ nodeId ].mStartIndex;
          unsigned int Len         = ReadBisection->mList[ nodeId ].mLen;
          unsigned long long TotalWeight = ReadBisection->mList[ nodeId ].mTotalWeight;
          
          StrongAssertLogLine( Len > 0 )
            << "PartitionKeysBasedOnHistogramRecursiveBisection:: Error:: "
            << " nodeId: " << nodeId
            << " Len: " << Len
            << " StartIndex: " << StartIndex
            << " ReadBisection->mCount: " << ReadBisection->mCount
            << EndLogLine;
          
          if( PkNodeGetId() == 0 )
            {
            if( TotalWeight > RecCapacity )
              {
              BegLogLine( 1 )
                << " TotalWeight: " << TotalWeight
                << " RecCapacity: " << RecCapacity
                << " nodeId: " << nodeId
                << " Len: " << Len
                << " StartIndex: " << StartIndex
                << " ReadBisection->mCount: " << ReadBisection->mCount
                << EndLogLine;

              for( int bin = StartIndex; bin < (StartIndex+Len); bin++ )
                {
                BegLogLine( 1 )
                  << " bin: " << bin
                  << " aHistogram[ " << bin << " ].mCount: "
                  << aHistogram[ bin ].mCount
                  << EndLogLine;
                }
                
              StrongAssertLogLine( TotalWeight <= RecCapacity )
                << "PartitionKeysBasedOnHistogramRecursiveBisection:: Error:: "
                << " assigned record allocation is greater then capacity. "
                << " capacity: " << RecCapacity
                << " TotalWeight: " << TotalWeight
                << " nodeId: " << nodeId
                << EndLogLine;
              
              }
            }

          Record* LastRec = NULL;
          for( int bin = StartIndex; bin < (StartIndex+Len); bin++ )
            {
            aHistogramBinAssignment[ bin ].mNodeId = nodeId;

            if( aHistogramBinAssignment[ bin ].mFirstRec == NULL )
              continue;
            
            if( mRecordsForNode[ nodeId ] == NULL )
              mRecordsForNode[ nodeId ] = aHistogramBinAssignment[ bin ].mFirstRec;
            else
              LastRec->mNext = aHistogramBinAssignment[ bin ].mFirstRec;
            
            LastRec = aHistogramBinAssignment[ bin ].mLastRec;
            
            AssertLogLine( LastRec->mNext == NULL )
              << "ERROR:: " << (void *) LastRec->mNext
              << " nodeId: " << nodeId
              << " bin: " << bin
              << " mGlobalHistogram[ " << bin << " ].mCount: " 
              <<   mGlobalHistogram[ bin ].mCount
              << " mLocalHistogram[ " << bin << " ].mCount: " 
              <<   mLocalHistogram[ bin ].mCount
              << EndLogLine;          
            }
          
          if( nodeId == PkNodeGetId() )
            {
            RecAssignedToMeCount = TotalWeight;
            }
          }
        
        PkHeapFree( EvenBisection.mList );
        PkHeapFree( OddBisection.mList );

        BegLogLine( TABLE_SORT_LOGGING )
          << "HistogramRecursiveBisection: "  
          << " Leaving "
          << EndLogLine;

        return RecAssignedToMeCount;
        }

      /**************************************************************
       * Creates an assignemnt of bins to nodes
       * Return: number of records assigned to this node
       *************************************************************/
      int
      PartitionKeysBasedOnHistogram( HistogramBin*             aHistogram, 
                                     HistogramBinAssignment*   aHistogramBinAssignment,
                                     int                       aBinCount, 
                                     int                       aNodeCount,
                                     unsigned long long        aNumRecords )
        {
        unsigned long long RecPerNode = aNumRecords / aNodeCount;
        unsigned long long TargetRecPerNode = mTargetRecordsPerNodeFactor * RecPerNode;

        unsigned long long RecAssignedToMeCount = 0;

        int EmptyNodeCount = 0;
        int IsSomeNodeOverFlown = 0;
        int FirstTime     = 1;
        int LastIteration = 0;

        int MinEmptyNodeCount       = aNodeCount;
        double MinTargetRecPerNode  = -1.0;

        int TryCount    = 0;
        int MaxTryCount = 50;

        int LastDecision = -1;

        double Delta = 0.1;

        int UpFactor = 0;
        int DownFactor = 0;

        unsigned int RecCapacity = mRecordBufferManager.available_capacity();

        while( !LastIteration )
          {
          if( !FirstTime && !LastIteration )
            {
            if( UpFactor )
              {
              // Increase target rec per node 
              // to allow for other nodes to take over the load              

              if( LastDecision == 0 )
                {
                // Last decision was to increase
                // then we want to decrease by half of the increase amount
                Delta /= 2.0;
                }

              mTargetRecordsPerNodeFactor += Delta;
              LastDecision = 1;
              }
            else if( DownFactor )
              {
              // Decrease target rec per node 
              // Some nodes have nothing to do
                            
              if( LastDecision == 1 )
                {
                // Last decision was to increase
                // then we want to decrease by half of the increase amount
                Delta /= 2.0;
                }

              mTargetRecordsPerNodeFactor -= Delta;
              LastDecision = 0;
              }
            else
              {
              // Assignement was successful
              break;
              }
            }
          
          // Time to go...
          if( TryCount == MaxTryCount )
            {
            mTargetRecordsPerNodeFactor = MinTargetRecPerNode;
            StrongAssertLogLine( MinTargetRecPerNode != -1 )
              << "PartitionKeysBasedOnHistogram::ERROR:: "
              << " Could not find a viable assignment after "
              << MaxTryCount << " iterations "
              << EndLogLine;
            
            LastIteration = 1;
            }
          
          TargetRecPerNode = mTargetRecordsPerNodeFactor * RecPerNode;          
            
          TryCount++;

          // BegLogLine( PkNodeGetId() == 0 )
          BegLogLine( 0 )
            << "TryCount: " << TryCount
            << " LastIteration: " << LastIteration
            << " TargetRecPerNode: " << TargetRecPerNode
            << " mTargetRecordsPerNodeFactor: " << mTargetRecordsPerNodeFactor
            << " LastIteration: " << LastIteration
            << " Delta: " << Delta
            << " EmptyNodeCount: " << EmptyNodeCount
            << " IsSomeNodeOverFlown: " << IsSomeNodeOverFlown
            << " MinEmptyNodeCount: " << MinEmptyNodeCount
            << " MinTargetRecPerNode: " << MinTargetRecPerNode
            << " RecCapacity: " << RecCapacity
            << " UpFactor: " << UpFactor
            << " DownFactor: " << DownFactor
            << EndLogLine;

          // Create the assignement 
          int CurrentNodeId = 0;
          unsigned long long CurrentRecCount = 0;                    
          for( int i=0; i < aBinCount; i++ )
            {
            if(( aHistogram[ i ].mCount + CurrentRecCount ) <= TargetRecPerNode )
              {
              aHistogramBinAssignment[ i ].mNodeId = CurrentNodeId;
              CurrentRecCount += aHistogram[ i ].mCount;
              }
            else
              {
              // Give it to the next node
              CurrentNodeId++;
              
              if( CurrentNodeId >= aNodeCount )
                CurrentNodeId = aNodeCount-1;
              
              aHistogramBinAssignment[ i ].mNodeId = CurrentNodeId;
              CurrentRecCount = aHistogram[ i ].mCount;
              }
            }
                    
          // See what that did to the load on each node
          for( int i=0; i < aNodeCount; i++ )
            {
            mCountPerNode[ i ] = 0;
            }
          
          for( int i=0; i < aBinCount; i++ )
            {
            int NodeId = aHistogramBinAssignment[ i ].mNodeId;
            
            StrongAssertLogLine( NodeId >= 0 && NodeId < PkNodeGetCount() )
              << "ERROR:: NodeId: " << NodeId
              << EndLogLine;
            
            mCountPerNode[ NodeId ] += aHistogram[ i ].mCount;
            }
          
          IsSomeNodeOverFlown = 0;
          EmptyNodeCount = 0;
          UpFactor = 0;
          DownFactor = 0;
          int EverythingFits = 1;
          for( int i=0; i < aNodeCount; i++ )
            {
            if( mCountPerNode[ i ] == 0 )
              EmptyNodeCount++;
            else if( mCountPerNode[ i ] > 0.95 * RecCapacity )
              {
              // IsSomeNodeOverFlown = 1;
              if( i == aNodeCount-1 )
                {
                // If the last bin is over filled then increase the factor.
                UpFactor = 1;
                }
              else
                {
                // If bins other then the last bin are overfilled then decrease the factor.
                DownFactor = 1;
                }
              EverythingFits = 0;
              }
            }

          // Only store if it's a legal(all the records fit) configuration
          if( EverythingFits && ( EmptyNodeCount < MinEmptyNodeCount ))
            {
            MinEmptyNodeCount = EmptyNodeCount;
            MinTargetRecPerNode = mTargetRecordsPerNodeFactor;
            }

          if( EmptyNodeCount && !UpFactor )
            DownFactor = 1;

          FirstTime = 0;
          }

        // String the records for a node according to assignment
        Record* LastRec = NULL;
        int PrevNodeId = -1;
        for( int i=0; i < aBinCount; i++ )
          {
          int NodeId = aHistogramBinAssignment[ i ].mNodeId;
            
          // Don't string empty bins
          if( aHistogramBinAssignment[ i ].mFirstRec == NULL )
              continue;
            
          if( NodeId != PrevNodeId )
            {
            AssertLogLine( NodeId >= 0 && NodeId < PkNodeGetCount() )
              << "ERROR:: NodeId: " << NodeId
              << EndLogLine;
              
            mRecordsForNode[ NodeId ] = aHistogramBinAssignment[ i ].mFirstRec;
            }
          else
            {            
            LastRec->mNext = aHistogramBinAssignment[ i ].mFirstRec;
            }
            
          PrevNodeId = NodeId;
          LastRec = aHistogramBinAssignment[ i ].mLastRec;
            
          AssertLogLine( LastRec->mNext == NULL )
            << "ERROR:: " << (void *) LastRec->mNext
            << " NodeId: " << NodeId
            << " i: " << i
            << " mGlobalHistogram[ " << i << " ].mCount: " 
            <<   mGlobalHistogram[ i ].mCount
            << " mLocalHistogram[ " << i << " ].mCount: " 
            <<   mLocalHistogram[ i ].mCount
            << EndLogLine;          
          }
                    
        for( int i=0; i < aBinCount; i++ )
          {
          int NodeId = aHistogramBinAssignment[ i ].mNodeId;
          if( NodeId == PkNodeGetId() )
            {
            RecAssignedToMeCount += aHistogram[ i ].mCount;
            }
          }

        if( PkNodeGetId() == 0 )
          {
          for( int i=0; i < aNodeCount; i++ )
            {
            BegLogLine( 0 )
              << "mCountPerNode[ " << i << " ]: " 
              << mCountPerNode[ i ]
              << EndLogLine;
            }
          }
        
#if 0          
          unsigned long long MinCount = 999999999;
          int MinNode  = -1;
          unsigned long long MaxCount = 0;
          int MaxNode  = -1;
          for( int i=0; i < aNodeCount; i++ )
            {
            if( MinCount > mCountPerNode[ i ] )
              {
              MinCount = mCountPerNode[ i ];
              MinNode  = i;
              }
            
            if( MaxCount < mCountPerNode[ i ] )
              {
              MaxCount = mCountPerNode[ i ];
              MaxNode  = i;
              }
            }

        BegLogLine( PkNodeGetId() == 0 )
          << " aNumRecords: " << aNumRecords
          << " MinCount: " << MinCount
          << " MinNode: " << MinNode
          << " MaxCount: " << MaxCount
          << " MaxNode: " << MaxNode
          << EndLogLine;

#endif

        BegLogLine( 0 )        
          << " RecPerNode: " << RecPerNode
          << " TargetRecPerNode: " << TargetRecPerNode
          << " mTargetRecordsPerNodeFactor: " << mTargetRecordsPerNodeFactor
          << EndLogLine;

        // PkHeapFree( CountPerNode );    

        StrongAssertLogLine( RecAssignedToMeCount == mCountPerNode[ PkNodeGetId() ] )
          << "Table::PartitionKeysBasedOnHistogram::ERROR:: "
          << " RecAssignedToMeCount: " << RecAssignedToMeCount
          << " mCountPerNode[ PkNodeGetId() ]: " << mCountPerNode[ PkNodeGetId() ]
          << EndLogLine;        
        
        return RecAssignedToMeCount;
        }
      
      int
      GetGlobalHistogramAssignment(Key aMinKey, Key aMaxKey)
        {
        HistogramStart.HitOE( TRACE_SORT,
                              "Histrogram",
                              PkNodeGetId(),
                              HistogramStart );
        
        TableType::iterator contents_itr = mRecordBufferManager.begin();
        TableType::iterator contents_end = mRecordBufferManager.end();
        
        int SizeOfKeyToHistogram = sizeof( Key );
        if( SizeOfKeyToHistogram > sizeof( unsigned long long) )
          SizeOfKeyToHistogram = sizeof( unsigned long long);
        
        int CopyOffset = sizeof( unsigned long long) - SizeOfKeyToHistogram;
        StrongAssertLogLine( CopyOffset >= 0 && CopyOffset < sizeof( unsigned long long) )
          << "ERROR:: " 
          << " sizeof( Key ): " << sizeof( Key )
          << " CopyOffset: " << CopyOffset
          << " SizeOfKeyToHistogram: " << SizeOfKeyToHistogram
          << EndLogLine;
        
        KeyRepAsLongLong MinVal;
        MinVal.mVal = 0;
        memcpy( &(MinVal.mChars[ CopyOffset ]), &aMinKey, SizeOfKeyToHistogram );

        KeyRepAsLongLong MaxVal;
        MaxVal.mVal = 0;
        memcpy( &(MaxVal.mChars[ CopyOffset ]), &aMaxKey, SizeOfKeyToHistogram );

        double Delta = 1.0 * (MaxVal.mVal - MinVal.mVal) / static_cast<double>( mBinCount );

        BegLogLine( TABLE_SORT_LOGGING )
          << "GetGlobalHistogram:: "
          << " MinVal.mVal: " << MinVal.mVal
          << " MaxVal.mVal: " << MaxVal.mVal
          << " Delta: " << Delta
          << EndLogLine;

        for( int i=0; i < mBinCount; i++ )
          {
          mLocalHistogram[ i ].mCount = 0;
          }

        for (; contents_itr != contents_end; ++contents_itr)
          {
          Key CurKey = contents_itr->mKey;
          
          KeyRepAsLongLong KeyRep;
          KeyRep.mVal = 0;

          memcpy( &KeyRep.mChars[ CopyOffset ], &CurKey, SizeOfKeyToHistogram );
          
          int BinIndex = (int) floor( 1.0 * (KeyRep.mVal - MinVal.mVal) / Delta );
          
//           if( BinIndex < 0 ) 
//             {
//             BegLogLine( 1 )
//               << "KeyRep.mChars[ " << 0 << " ]: "  << (unsigned int) KeyRep.mChars[ 0 ]
//               << " KeyRep.mChars[ " << 1 << " ]: " << (unsigned int) KeyRep.mChars[ 1 ]
//               << " KeyRep.mChars[ " << 2 << " ]: " << (unsigned int) KeyRep.mChars[ 2 ]
//               << " KeyRep.mChars[ " << 3 << " ]: " << (unsigned int) KeyRep.mChars[ 3 ]
//               << " KeyRep.mChars[ " << 4 << " ]: " << (unsigned int) KeyRep.mChars[ 4 ]
//               << " KeyRep.mChars[ " << 5 << " ]: " << (unsigned int) KeyRep.mChars[ 5 ]
//               << " KeyRep.mChars[ " << 6 << " ]: " << (unsigned int) KeyRep.mChars[ 6 ]
//               << " KeyRep.mChars[ " << 7 << " ]: " << (unsigned int) KeyRep.mChars[ 7 ]
//               << EndLogLine;
//             _exit( -1 );
//             }          

          BinIndex = BinIndex < 0 ? 0 : BinIndex;
          BinIndex = BinIndex > (mBinCount - 1) ? (mBinCount - 1) : BinIndex;

          mLocalHistogram[ BinIndex ].mCount++;
          
          Record * RecPtr = contents_itr.itr;

          mHistogramBinAssignment[ BinIndex ].AddRecord( RecPtr );

          AssertLogLine( mHistogramBinAssignment[ BinIndex ].mLastRec->mNext == NULL )
            << "Table::ERROR::GetGlobalHistogramAssignment:: " 
            << " mHistogramBinAssignment[ " << BinIndex << " ].mLastRec->mNext: "
            << (void *) mHistogramBinAssignment[ BinIndex ].mLastRec       
            << " mLocalHistogram[ " << BinIndex << " ].mCount: " 
            << mLocalHistogram[ BinIndex ].mCount            
            << EndLogLine;
          }
        
        // All Reduce the histogram
        int SizeOfHistograms = sizeof( HistogramBin ) * mBinCount;
        int NumberOfInts = SizeOfHistograms / sizeof( unsigned int );

        for( int i=0; i < mBinCount; i++ )
          {
          BegLogLine( TABLE_LOGGING )
            // BegLogLine( 1 )
            << "GetGlobalHistogram: Before Reduce "
            << " mLocalHistogram[ " << i << " ].mCount: " 
            << mLocalHistogram[ i ].mCount
            << EndLogLine;
          }
        
        memcpy( mGlobalHistogram, mLocalHistogram, sizeof( unsigned int ) * NumberOfInts );
        
        BegLogLine( TABLE_SORT_LOGGING )
        // BegLogLine( 1 )
          << "GetGlobalHistogram: Before AllReduce  "
          << " NumberOfInts: " << NumberOfInts
          << EndLogLine;

  // GlobalTreeReduce( (unsigned long *) mGlobalHistogram, (unsigned long *) mGlobalHistogram, sizeof( unsigned int ) * NumberOfInts, _BGL_TREE_OPCODE_ADD );
  mMyAllReduce.Execute( (unsigned int * ) mGlobalHistogram, NumberOfInts );
        
        BegLogLine( TABLE_SORT_LOGGING )
          << "GetGlobalHistogram: After AllReduce  "
          << " NumberOfInts: " << NumberOfInts
          << EndLogLine;

        for( int i=0; i < mBinCount; i++ )
          {
          BegLogLine( TABLE_LOGGING )
          //BegLogLine( PkNodeGetId() == 0 )
            << "GetGlobalHistogram: After Reduce "
            << " mGlobalHistogram[ " << i << " ].mCount: " 
            << mGlobalHistogram[ i ].mCount
            << EndLogLine;
          }

        /***************************************************************/

        unsigned long long NumRows = 0;
        for( int i=0; i < mBinCount; i++ )
          {
          NumRows += mGlobalHistogram[ i ].mCount;
          }

        int RecAssignedToThisNodeCount
            = PartitionKeysBasedOnHistogram( mGlobalHistogram, 
            // = PartitionKeysBasedOnHistogramRecursiveBisection( mGlobalHistogram,
                                                             mHistogramBinAssignment,
                                                             mBinCount, 
                                                             mNodeCount, 
                                                             NumRows );
        
        HistogramFinis.HitOE( TRACE_SORT,
                              "Histrogram",
                              PkNodeGetId(),
                              HistogramFinis );
        
        return RecAssignedToThisNodeCount;
        }

  public:

    //*************************************************************************
    // Constructor / Destructor
    //*************************************************************************
    template< class Allocator >
    static Table< TypeInfo >* instance(const char* aTableName, 
                                       int aNodeCount,
                                       int aBinExpansionFactor=BIN_COUNT_FACTOR,
                                       double aTargetRecordsPerNodeFactor=TARGET_RECORDS_PER_NODE_FACTOR,
                                       int aRecordsPerNode=DEFAULT_RECORD_COUNT )
      {
      Allocator alloc;
      Table< TypeInfo > *new_ptr = (Table< TypeInfo >*)alloc.allocate( 1 );

      // alloc.construct(new_ptr, Table< TypeInfo >( aTableName, aNodeCount ));
      new_ptr->Init( aTableName, 
                     aNodeCount, 
                     aBinExpansionFactor,
                     aTargetRecordsPerNodeFactor,
                     aRecordsPerNode );

      return (new_ptr);
      }

//     template< class Allocator >
//     static Table< TypeInfo >* instance(const char* aTableName, TableType* aTableContents)
//       {
//       Allocator alloc;
//       Table< TypeInfo > *new_ptr = (Table< TypeInfo >*)alloc.allocate( 1 );
//       alloc.construct(new_ptr, Table< TypeInfo >(aTableName, aTableContents));

//       return (new_ptr);
//       }

    ~Table() {} //handle deallocation elsewhere
        
    //*************************************************************************
    // This _should_ be a fully-compliant STL iterator
    // (along with Table being a fully-compliant STL container)
    // But for now, just the basics
    //*************************************************************************
    class iterator {

      private:

      public:
        typename TableType::iterator map_itr; //make this a friend to exterior class??

        //Copied, dont know if necessary?
        //typedef bidirectional_iterator_tag iterator_category;
        typedef KeyValueType value_type;
        typedef value_type & reference;
        typedef value_type * pointer;

        iterator(typename TableType::iterator new_itr) 
          {
          map_itr = new_itr;
          }

        reference operator*() const   
          {
          return (reference) (*map_itr);
          }
        
        pointer operator->()
          {
          return(&map_itr);
          }
        
        iterator& operator++() 
          {
          ++map_itr;
          return (*this);
          }
        
        iterator& operator--() 
          {
          --map_itr;
          return (*this);
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
          return (map_itr == eq_itr.map_itr);
          }

        bool operator!=(const iterator &eq_itr) const 
          {
          return (!(map_itr == eq_itr.map_itr));
          }
      };
    
    //*************************************************************************
    
    //*************************************************************************
    // Table specific  
    //*************************************************************************

//     //*************************************************************************
//     // Reserve a space for the key pointer pair
//     //*************************************************************************
//     Tuple* reserveTuple(const Key& aKey)
//       {
//       Tuple* NewTuplePtr = (Tuple *) mTupleAlloc.allocate( 1 );
      
//       if( NewTuplePtr == NULL )
//         {
//         // Allocation failed
//         return NULL;
//         }
//       else if( !(mTableContents->insert( std::make_pair( aKey, NewTuplePtr ))).second )
//         {
//         // Insertion failed
//         return NULL;
//         }

//       return NewTuplePtr;
//       }

    //*************************************************************************
    //*************************************************************************
    bool insert(const Key& aKey, const Tuple& aTuple)
      {
      Record* rc = mRecordBufferManager.insert( aKey, aTuple );

      return ( rc != NULL);
      }

    bool insert( Record* aRec )
      {
      Record* rc = mRecordBufferManager.insert( aRec );

      return ( rc != NULL );
      }

    //*************************************************************************
    //*************************************************************************
//     bool insert(const Key& aKey, const Tuple* aTuplePtr )
//       {
      
//       return ( mTableContents->insert( std::make_pair( aKey, aTuplePtr )) ).second;
//       }

    //*************************************************************************
    //*************************************************************************
    typename Table< TypeInfo >::iterator getTuple(const Key& aKey)
      {
      Record Rec;
      Rec.SetKey( aKey );

      TableType::iterator map_itr = this->mRecordBufferManager.find( &Rec );
      Table< TypeInfo >::iterator new_itr(map_itr);

      return new_itr;
      } //end getTuple(Key)

    //*************************************************************************
    //*************************************************************************
    void removeTuple(const Key& aKey )
      {
      BegLogLine( 1 )
          << "Table::removeTuple:: WARNING Deleting the record ptr is not implemented!!!"
          << EndLogLine;

      Record Rec;
      Rec.SetKey( aKey );

      mRecordBufferManager.erase( Rec );
      } //end removeTuple()

    void removeTuple(typename Table< TypeInfo >::iterator aKeyItr)
      {
      BegLogLine( 0 )
          << "Table::removeTuple:: WARNING Deleting the record ptr is not implemented!!!"
          << EndLogLine;

      mRecordBufferManager.erase( *aKeyItr.map_itr );
      } //end removeTuple()


    //*************************************************************************
    // General Container
    //*************************************************************************

    //*************************************************************************
    //*************************************************************************
    typename Table< TypeInfo >::iterator begin()
      {
      typename TableType::iterator new_itr = mRecordBufferManager.begin();
      return Table< TypeInfo >::iterator( new_itr );
      } //end begin()  


    //*************************************************************************
    //*************************************************************************
    typename Table< TypeInfo >::iterator end()
      {
      typename TableType::iterator new_itr = mRecordBufferManager.end();
      return Table< TypeInfo >::iterator(new_itr);
      } //end begin()
  
    //*************************************************************************
    //*************************************************************************
    int rowcount()
      {
      return mRecordBufferManager.size();
      } //end count()

    //*************************************************************************
    //*************************************************************************
    void clear()
      {
      mRecordBufferManager.clear();
      }

    //*************************************************************************
    //*************************************************************************
    void print(char * Label = "", int ToPrint=1 )
      {
      if( ToPrint )
        {
        BegLogLine( 1 )
          << "Printing Table: " << Label << " " 
          << this->mTableName 
          << " row count: " << rowcount()
          << EndLogLine;
        
        std::cout << "( ";
        for( int i = 0; i < TypeInfo::AttributeCount; ++i)
          {
          int AttributeTypeIndex = TypeInfo::AttributeDescriptor[ i ].mAttributeTypeIndex;
          AssertLogLine( ( AttributeTypeIndex >= 0 ) && ( AttributeTypeIndex < MAX_ATTRIBUTE_TYPE_INDEX ))
            << EndLogLine;
          
          const char* AttributeTypeString = TYPES::TypeNameStrings[ AttributeTypeIndex ];
          
          std::cout << TypeInfo::AttributeDescriptor[ i ].mAttributeName 
                    << " [" << AttributeTypeString << "]";
          if (i != (TypeInfo::AttributeCount-1)) { std::cout << " , "; }
          }
        std::cout << " )" << std::endl;

        TableType::iterator contents_itr = mRecordBufferManager.begin();
        TableType::iterator contents_end = mRecordBufferManager.end();
        
        for (; contents_itr != contents_end; ++contents_itr)
          {
          BegLogLine( 1 )
            << "Key: "
            << contents_itr->mKey
//             << " Tuple: " 
//             << contents_itr->mTuple
            << EndLogLine;
          }
        }
      } //end print()
      
    void redistribute_flow_control( int aRecAssignedToThisNodeCount )
      {
      RedistributionStart.HitOE( TRACE_SORT,
                                 "Redistribution",
                                 PkNodeGetId(),
                                 RedistributionStart );

      BegLogLine( TABLE_REDISTRIBUTION_LOGGING )
        << "Table::redistribute_plan:: Entering... "
        << EndLogLine;

      /**************************************************************
       * Move data - All to all of data according to plan
       *************************************************************/     
      int TotalRecordsToRecvCount = aRecAssignedToThisNodeCount;
      

      mComm.PrimeState( TotalRecordsToRecvCount,
                        &mRecordBufferManager,
                        mRecordsForNode );

      unsigned int BarrierBuffer[ 1 ];
      mMyAllReduce.Execute( BarrierBuffer, 1 );
      
      // PkCo_BarrierWithYield();

      BegLogLine( TABLE_REDISTRIBUTION_LOGGING )
        << "Table::redistribute:: "
        << " TotalRecordsToRecvCount: " << TotalRecordsToRecvCount
        << EndLogLine;

      RedistributionCommStart.HitOE( TRACE_SORT,
                                     "RedistributionComm",
                                     PkNodeGetId(),
                                     RedistributionCommStart );

      mComm.Send( & mRandomNodeList );
      
      mComm.Wait();      

      RedistributionCommFinis.HitOE( TRACE_SORT,
                                     "RedistributionComm",
                                     PkNodeGetId(),
                                     RedistributionCommFinis );

      StrongAssertLogLine( TotalRecordsToRecvCount == mComm.GetReceivedRecordCount() )
        << " ERROR:: Table::redistribute_plan:: did not receive the expected number of records"
        << " TotalRecordsToRecvCount: " << TotalRecordsToRecvCount
        << " mComm.GetReceivedRecordCount(): " << mComm.GetReceivedRecordCount()
        << EndLogLine;

      BegLogLine( TABLE_REDISTRIBUTION_LOGGING )
        << "Table::redistribute_plan:: Done with communication, about to compress "
        << EndLogLine;

      // Take out all the non active records, left due to reusing 
      // the same buffer for sending and receiving
      mRecordBufferManager.compress();

      BegLogLine( TABLE_REDISTRIBUTION_LOGGING )
        << "Table::redistribute_plan:: Leaving... "
        << EndLogLine;

      RedistributionFinis.HitOE( TRACE_SORT,
                                 "Redistribution",
                                 PkNodeGetId(),
                                 RedistributionFinis );
      }
      
    //*************************************************************************
    //*************************************************************************
      // template< class DistributionFunctor >
      // void redistribute_plan(DistributionFunctor & aDistributionFunc )
    void redistribute_plan( int aRecAssignedToThisNodeCount )
      {
      RedistributionStart.HitOE( TRACE_SORT,
                                 "Redistribution",
                                 PkNodeGetId(),
                                 RedistributionStart );

      BegLogLine( TABLE_LOGGING )
        << "Table::redistribute_plan:: Entering... "
        << EndLogLine;
      
      /**************************************************************
       * Setup local receive buffers
       *************************************************************/      
      int TotalRecordsToRecvCount = aRecAssignedToThisNodeCount;

#if 0
      /**************************************************************
       * Plan data movement
       *************************************************************/      
      // All-to-all of record to send counts
      for( int i=0; i < mNodeCount; i++ )
        mRecordsToSentForNode[ i ] = 0;
      
      for( int i=0; i < mBinCount; i++ )
        {
        int NodeId = mGlobalHistogram[ i ].mNodeId;
        int Count  = mLocalHistogram[ i ].mCount;

        mRecordsToSendForNode[ NodeId ] += Count;
        }      

      AllToAll.Execute( mRecordsToSentForNode, 
                        mRecordsToRecvForNode );

      // Create the receive buffer pointers
      mRecvBuffers[ 0 ] = RecvBuffer;
      for( int i = 1; i < mNodeCount; i++ )
        {
        mRecvBuffers[ i ] = mRecvBuffers[ i-1 ] +  mRecordsToRecvForNode[ i-1 ];
        }

      // Create syncronous data movement plan
#endif     

      /**************************************************************
       * Move data - All to all of data according to plan
       *************************************************************/     
      BegLogLine( TABLE_REDISTRIBUTION_LOGGING )
        << "Table::redistribute:: "
        << " TotalRecordsToRecvCount: " << TotalRecordsToRecvCount
        << EndLogLine;

      mComm.PrimeState( TotalRecordsToRecvCount,
                        &mRecordBufferManager );

      // mMyBarrier.Execute();
//      PkCo_BarrierWithYield();
      PkCo_Barrier();

      RedistributionCommStart.HitOE( TRACE_SORT,
                                     "Redistribution",
                                     PkNodeGetId(),
                                     RedistributionCommStart );

      BegLogLine( TABLE_REDISTRIBUTION_LOGGING )
        << "Table::redistribute:: "
        << " mRandomNodeList.IsEmpty(): " << mRandomNodeList.IsEmpty()
        << EndLogLine;   

      while( !mRandomNodeList.IsEmpty() )
        {
        RandomNodeLink* RandomNode = mRandomNodeList.GetNext();
        int NodeId = RandomNode->mNodeId;
                
        Record * CurRec = mRecordsForNode[ NodeId ];
        
        // Might be nothing to send to a node
        if( CurRec == NULL )
          {
          mRandomNodeList.Remove( RandomNode );
          continue;
          }

        AssertLogLine( CurRec != NULL )
          << "Table::redistribute:: "
          << " CurRec: " << (void *) CurRec
          << " NodeId: " << NodeId
          << " RandomNode: " << (void *) RandomNode
          << EndLogLine;

//         AssertLogLine( ( (unsigned) CurRec != RECORD_ACTIVE ) &&
//                        ( (unsigned) CurRec != RECORD_NON_ACTIVE ) )
//                          << "redistribute_plan:: ERROR:: "
//                          << " CurRec: " << (void *) CurRec
//                          << EndLogLine;

        // CurRec should either be NULL or some real pointer value
        // Not one of the values below.
        AssertLogLine( !CurRec->IsNonActive() || !CurRec->IsActive() )
          << "redistribute_plan:: ERROR:: "
          << " CurRec->mNext: " << (void *) CurRec->mNext
          << EndLogLine;

        int SentCount = mComm.Send( NodeId, CurRec );

        // Remove the elements, advance the counter.
        for( int i=0; i<SentCount; i++ )
          {
          AssertLogLine( CurRec != NULL )
            << "Table::redistribute:: "
            << " i: " << i
            << " SentCount: " << SentCount
            << EndLogLine;

          BegLogLine( 0 )
            << "Table::redistribute:: "
            << " CurRec: " << (void *) CurRec
            << " CurRec->mNext: " << (void *) CurRec->mNext
            << " i: " << i
            << " SentCount: " << SentCount
            << EndLogLine;


          Record* TmpPtr = CurRec;
          CurRec = CurRec->mNext;

          mRecordBufferManager.AddToFreePool( TmpPtr );
          TmpPtr->SetNonActive();
          }

        if( CurRec == NULL )
          mRandomNodeList.Remove( RandomNode );
        else
          {
          // CurRec should either be NULL or some real pointer value
          // Not one of the values below.
//           AssertLogLine( ( (unsigned) CurRec != RECORD_ACTIVE ) &&
//                          ( (unsigned) CurRec != RECORD_NON_ACTIVE ) )
//                            << "redistribute_plan:: ERROR:: "
//                            << " CurRec: " << (void *) CurRec
//                            << EndLogLine;

            
          AssertLogLine( !CurRec->IsNonActive() || !CurRec->IsActive() )
            << "redistribute_plan:: ERROR:: "
            << " CurRec: " << (void *) CurRec
            << " CurRec->mNext: " << (void *) CurRec->mNext
            << EndLogLine;
          }

        mRecordsForNode[ NodeId ] = CurRec;
        }
          
      mComm.Flush();

      RedistributionCommFinis.HitOE( TRACE_SORT,
                                     "Redistribution",
                                     PkNodeGetId(),
                                     RedistributionCommFinis );
      
      mComm.Wait();


      /**************************************************************
       * Insert all the data into the set, result is in sorted order
       *************************************************************/
      StrongAssertLogLine( TotalRecordsToRecvCount == mComm.GetReceivedRecordCount() )
        << " ERROR:: Table::redistribute_plan:: did not receive the expected number of records"
        << " TotalRecordsToRecvCount: " << TotalRecordsToRecvCount
        << " mComm.GetReceivedRecordCount(): " << mComm.GetReceivedRecordCount()
        << EndLogLine;

      // Take out all the non active records, left due to reusing 
      // the same buffer for sending and receiving
      mRecordBufferManager.compress();             

#if 0
      RedistributionInsertStart.HitOE( TRACE_SORT,
                                       "Redistribution",
                                       PkNodeGetId(),
                                       RedistributionInsertStart );

      int     RecBufferCount = mRecordBufferManager.GetRecordBufferCount(); 
      Record* RecBuffer      = mRecordBufferManager.GetRecordBufferStart(); 

      BegLogLine( 1 )
        << "redistribute: "
        << " RecBufferCount: " << RecBufferCount
        << EndLogLine;

      int ActiveRecords = 0;
      for( int i = 0; i < RecBufferCount; i++ )
        {
        Record* Rec = &RecBuffer[ i ];
        if( Rec->IsActive() )
          {
          insert( Rec );
          ActiveRecords++;
          }
        }

      BegLogLine( TABLE_REDISTRIBUTION_LOGGING )
        << "Table::redistribution_plan:: "
        << " RecBufferCount: " << RecBufferCount
        << " ActiveRecords: " << ActiveRecords
        << " RecBuffer: " << (void *) RecBuffer
        << EndLogLine;

      RedistributionInsertFinis.HitOE( TRACE_SORT,
                                       "Redistribution",
                                       PkNodeGetId(),
                                       RedistributionInsertFinis );
#endif
     
      BegLogLine( TABLE_LOGGING )
        << "Table::redistribute_plan:: Leaving... "
        << EndLogLine;

      RedistributionFinis.HitOE( TRACE_SORT,
                                 "Redistribution",
                                 PkNodeGetId(),
                                 RedistributionFinis );
      }

    //*************************************************************************
    //*************************************************************************
    void checkSort( unsigned long long aTotalRecordCount )
      {
      CheckSortPacketsSent = 0;    
      CheckSortPacketsRecv = 0;    

      // Makes sure that every one is done with their sort
      // PkCo_BarrierWithYield();
      unsigned int BarrierBuffer[ 1 ];
      mMyAllReduce.Execute( BarrierBuffer, 1 );
      
      TableType::iterator contents_itr = mRecordBufferManager.begin();
      TableType::iterator contents_end = mRecordBufferManager.end();

      // Start with one since we loop over N-1 records
      long long RecordCount = 1;
      
      for (; contents_itr != (contents_end-1); ++contents_itr)
        {
        TableType::iterator Cur = contents_itr;
        TableType::iterator Next = Cur+1;
        
        StrongAssertLogLine( Cur->mKey <= Next->mKey )
          << "Table::checkSort:: "
          << " Cur->mKey: [ " << Cur->mKey.key << " ] "
          << " Next->mKey: [ " << Next->mKey.key << " ] "
          << EndLogLine;
        
        RecordCount++;
        }

      // GlobalTreeReduce64( (long long*) &RecordCount, (long long *) &RecordCount, sizeof( long long ), _BGL_TREE_OPCODE_ADD );

      mMyAllReduceLongLong.Execute( &RecordCount, 1 );
      
      StrongAssertLogLine( aTotalRecordCount == RecordCount )
        << "checkSort:: ERROR::  " 
        << " aTotalRecordCount: " << aTotalRecordCount
        << " RecordCount: " << RecordCount
        << EndLogLine;
      
      int MyNodeId = PkNodeGetId();
      
      int BiggerNodeId = -1;
      int SmallerNodeId = -1;
      if( MyNodeId == 0 )
        {
        BiggerNodeId = MyNodeId+1;
        }
      else if( MyNodeId == (PkNodeGetCount()-1) )
        {
        SmallerNodeId = MyNodeId-1;
        }
      else
        {
        SmallerNodeId = MyNodeId-1;
        BiggerNodeId = MyNodeId+1;
        }
      
      if( SmallerNodeId != -1 )
        {
        CheckSortPacket* Pkt = (CheckSortPacket *) PkActorReservePacket( SmallerNodeId,
                                                                         0,
                                                                         CheckSortCompareFx,
                                                                         sizeof( CheckSortPacket ) );
        TableType::iterator contents_first = mRecordBufferManager.begin();
        
        Pkt->mGPID = mMyGPID;
        Pkt->mKey  = contents_first->mKey;
        Pkt->mKeyInPacketIsLargestFromNode  = 0;
        
        PkActorPacketDispatch( Pkt );
        
        CheckSortPacketsSent++;
        PkFiberYield();
        }
      
      if( BiggerNodeId != -1 )
        {
        CheckSortPacket* Pkt = (CheckSortPacket *) PkActorReservePacket( BiggerNodeId,
                                                                         0,
                                                                         CheckSortCompareFx,
                                                                         sizeof( CheckSortPacket ) );
        TableType::iterator contents_last = mRecordBufferManager.end() - 1;
        
        Pkt->mGPID = mMyGPID;
        Pkt->mKey  = contents_last->mKey;
        Pkt->mKeyInPacketIsLargestFromNode  = 1;
        
        PkActorPacketDispatch( Pkt );
        
        CheckSortPacketsSent++;
        PkFiberYield();
        }

      BegLogLine( 1 )
  << "CheckSort:: Check if we're ready to end the phase" 
  << EndLogLine;

      int PhaseEndBuff[ 2 ];
      while( 1 )
        {
        PhaseEndBuff[ 0 ] = CheckSortPacketsSent;
        PhaseEndBuff[ 1 ] = CheckSortPacketsRecv;
        
        mMyAllReduce.Execute( (unsigned int *) PhaseEndBuff, 2 );
  // GlobalTreeReduce( (unsigned long *) PhaseEndBuff, (unsigned long *) PhaseEndBuff, sizeof( unsigned long ) * 2, _BGL_TREE_OPCODE_ADD );
  
  // mMyAllReduce.Execute( );
  
       if( PhaseEndBuff[ 0 ] == PhaseEndBuff[ 1 ])
         {
         break;
         }
       else
         {
         PkFiberYield();
         }
        }
      
      BegLogLine( PkNodeGetId() == 0 )
          << "CheckSort:: Success! "
          << EndLogLine;
      }

    //*************************************************************************
    //*************************************************************************
    void sort()
      {
      SortStart.HitOE( TRACE_SORT, 
                       "Sort",
                       PkNodeGetId(),
                       SortStart );

      BegLogLine( TABLE_SORT_LOGGING )
        << "Table::sort():: Entering..."
        << EndLogLine;
      
      // Histogram
      Key MinVal = Key::MinValue();
      Key MaxVal = Key::MaxValue();
      int RecAssignedToThisNodeCount = GetGlobalHistogramAssignment( MinVal, MaxVal );

      BegLogLine( TABLE_SORT_LOGGING )
        << "Table::sort():: Done histogram..."
        << EndLogLine;
      
      #ifdef FLOW_CONTROL
        redistribute_flow_control( RecAssignedToThisNodeCount );
      #else
        redistribute_plan( RecAssignedToThisNodeCount );
      #endif
      
      BegLogLine( TABLE_SORT_LOGGING )
        << "Table::sort():: Done redistribution..."
        << EndLogLine;

      mRecordBufferManager.sort();

      BegLogLine( TABLE_SORT_LOGGING )
        << "Table::sort():: Leaving..."
        << EndLogLine;

      SortFinis.HitOE( TRACE_SORT, 
                       "Sort",
                       PkNodeGetId(),
                       SortFinis );
      }

    //*************************************************************************
    //*************************************************************************
    void
    Finalize()
      {
      mMyBarrier.Execute();
      }

    //*************************************************************************
    //*************************************************************************
    template< class DBConnection >
    bool importDatabaseTable(DBConnection& dbConnection, const char *dbTableName)
      {
      std::cerr << "importDatabaseTable() not implemented" << std::endl;
      return false;
      }

      

  };

} //end namespace

#endif
