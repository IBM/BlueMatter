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
 #ifndef __NAMED_ALL_REDUCE_HPP__
#define __NAMED_ALL_REDUCE_HPP__

#include <Pk/API.hpp>
#include <inmemdb/Semaphore.hpp>
#include <inmemdb/GlobalParallelObjectManager.hpp>
#include <inmemdb/NamedBarrier.hpp>

#ifndef NAMED_ALLREDUCE_LOGGING
#define NAMED_ALLREDUCE_LOGGING ( 1 )
#endif

enum 
 {
 NAMED_ALLREDUCE_SUM,
 NAMED_ALLREDUCE_AND,
 NAMED_ALLREDUCE_OR,
 NAMED_ALLREDUCE_XOR,
 };

template <class ReduceType, int NAMED_ALLREDUCE_OPERATION >
class NamedAllReduce
  {
  struct PreAllReduceDataPacket
    {
    GPID mGPID;
    int  mTotalCount;
    int  mOffset;
    int  mCount;
    int  mIsLastPacket; 
    ReduceType mData[ 1 ];
    };

  struct PostAllReduceDataPacket
    {
    GPID mGPID;
    int  mOffset;
    int  mCount;
    ReduceType mData[ 1 ];
    };

  GPID mMyGlobalParallelId; 
  int  mRoot;
  int  mMaxDataCount;    
  int  mNodeCount; 
  
//      unsigned* debug_ptr;
      
  Semaphore mSemaphore;      

  #define MAX_NAME_LENGTH 256
  char mName[ MAX_NAME_LENGTH ];

  ReduceType* mCurData;       
  int         mTotalCount;
  int         mAmountReceivedBack;

  // Needed by the server  
  int          mDoneSetupForIncomingData;
  ReduceType*  mReductionBuffer;
  int          mTotalDataReceived;  
      
  int * mNodeIdsInUse;
  int   mNodeIdsInUseCount;

  int * mNodeIdsInUseForFiber;
  int   mNodeIdsInUseCountForFiber;

  // Needed by server fiber;
  struct SendResultsBackFiberArgs
    {
    NamedAllReduce* mThis;    
    };

  ReduceType*  mReductionBufferForFiber;
  int          mReduceCount;    
  SendResultsBackFiberArgs mFiberArgs;

  NamedBarrier mMyBarrier;    

  static
  int
  SetReducedDataFx( void* arg )
    {
    PostAllReduceDataPacket * Pkt = (PostAllReduceDataPacket *) arg;

    GPID gpid = Pkt->mGPID;
    
    NamedAllReduce* MyAllReduce = GlobalParallelObject::Client::GetObjectForGPID<NamedAllReduce>( gpid );

    int Offset = Pkt->mOffset;
    int Count  = Pkt->mCount;
    for( int i = 0; i < Count; i++ )
      {
      MyAllReduce->mCurData[ Offset + i ] = Pkt->mData[ i ];
      }
    
    MyAllReduce->mAmountReceivedBack += Count;

    if( MyAllReduce->mAmountReceivedBack == MyAllReduce->mTotalCount )
      {
      BegLogLine( NAMED_ALLREDUCE_LOGGING )
        << "NamedAllReduce::SetReduceDataFx:: calling semaphore Up() "
        << EndLogLine;
      
      MyAllReduce->mSemaphore.Up();
      }
    
    return ( NULL );
    }

  void
  ClearFiberState()
    {
    AssertLogLine( mReductionBufferForFiber != NULL )
      << "ERROR::  mReductionBufferForFiber should not be NULL"
      << EndLogLine;

    BegLogLine( NAMED_ALLREDUCE_LOGGING )
      << "NamedAllReduce::ClearFiberState:: "
      << " mReductionBufferForFiber: " << (void *) mReductionBufferForFiber
      << EndLogLine;
    
    PkHeapFree( mReductionBufferForFiber );
    mReductionBufferForFiber = NULL;
    }

  static
  void  
  SendResultsBackFiber( void *args )
    {
    SendResultsBackFiberArgs * Args = (SendResultsBackFiberArgs *) args;
    NamedAllReduce* MyAllReduce = Args->mThis;

    int TotalCount = MyAllReduce->mReduceCount;

    int SizeLeftForData = PK_ACTOR_MAX_PACKET_PAYLOAD_SIZE - 
      (sizeof( PostAllReduceDataPacket ) - sizeof( ReduceType ));

    int MaxPostReduceDataInPacket = SizeLeftForData / sizeof( ReduceType );
    
    int NodeCount = MyAllReduce->mNodeIdsInUseCountForFiber;

    BegLogLine( NAMED_ALLREDUCE_LOGGING )
      << "NamedAllReduce::SendResultsBackFiber:: "
      << " MyAllReduce: " << (void *) MyAllReduce
      << " NodeCount: " << NodeCount
      << " MaxPostReduceDataInPacket: " << MaxPostReduceDataInPacket
      << " TotalCount: " << TotalCount
      << " SizeLeftForData: " << SizeLeftForData
      << EndLogLine;
    
    
    // Make sure that our node id is last.
    int MyNodeId = PkNodeGetId();
    
    for( int i=0;i<NodeCount; i++ )
      {
  if( MyAllReduce->mNodeIdsInUseForFiber[ i ] == MyNodeId )
    {
      int tmpNodeId = MyAllReduce->mNodeIdsInUseForFiber[ i ];
      MyAllReduce->mNodeIdsInUseForFiber[ i ] = MyAllReduce->mNodeIdsInUseForFiber[ NodeCount - 1 ];
      MyAllReduce->mNodeIdsInUseForFiber[ NodeCount - 1 ] = tmpNodeId;
      break;
    }
      }
    
    for( int i = 0; i < NodeCount; i++ )
      {
      int           NodeId    = MyAllReduce->mNodeIdsInUseForFiber[ i ];
      
      int Offset = 0;
      while( Offset < TotalCount )
        {
        int CountToSend = 0;
        
        if( ( Offset + MaxPostReduceDataInPacket ) <= TotalCount )
          {
          CountToSend = MaxPostReduceDataInPacket;
          }
        else
          {
          // Last packet
          CountToSend = ( TotalCount - Offset );
          }      
        
        int PacketSize = sizeof( PostAllReduceDataPacket ) + (CountToSend-1) * sizeof( ReduceType );

        PostAllReduceDataPacket * Pkt = (PostAllReduceDataPacket *)
          PkActorReservePacket( NodeId,
                                0,
                                SetReducedDataFx,
                                PacketSize );
        
        Pkt->mGPID          = MyAllReduce->mMyGlobalParallelId;
        Pkt->mOffset        = Offset;
        Pkt->mCount         = CountToSend;
        
        BegLogLine( NAMED_ALLREDUCE_LOGGING )
          << "NamedAllReduce::SendResultsBackFiber:: "
          << " MyAllReduce: " << (void *) MyAllReduce
          << " CountToSend: " << CountToSend
          << " Offset: " << Offset
          << " PacketSize: " << PacketSize
          << " gpid: " << MyAllReduce->mMyGlobalParallelId
          << " NodeId: " << NodeId
          << " MyAllReduce->mReductionBufferForFiber: " << (void *) MyAllReduce->mReductionBufferForFiber
          << EndLogLine;

        memcpy( Pkt->mData, &MyAllReduce->mReductionBufferForFiber[ Offset ], sizeof( ReduceType ) * CountToSend );
        
        
        Offset += CountToSend;
        
        PkActorPacketDispatch( Pkt );
        
        PkFiberYield();
        }
      }
    
    MyAllReduce->ClearFiberState();
    }

  static
  int
  AllReduceFx( void* args )
    {
    PreAllReduceDataPacket* Pkt = (PreAllReduceDataPacket *) args;
    
    GPID gpid = Pkt->mGPID;
    
    NamedAllReduce* MyAllReduce = GlobalParallelObject::Client::GetObjectForGPID<NamedAllReduce>( gpid );
    
    int TotalCount = Pkt->mTotalCount;

    if( !MyAllReduce->mDoneSetupForIncomingData )
      {      
      MyAllReduce->mReductionBuffer = (ReduceType *) PkHeapAllocate( sizeof( ReduceType) * TotalCount );
      
      BegLogLine( NAMED_ALLREDUCE_LOGGING )
        << "NamedAllReduce::AllReduceFx:: "
        << " TotalCount: " << TotalCount
        << " sizeof( ReduceType) * TotalCount: " << (sizeof( ReduceType) * TotalCount)
        << " MyAllReduce->mReductionBuffer: " << (void *)  MyAllReduce->mReductionBuffer
        << EndLogLine;

      for( int i=0; i < TotalCount; i++ )
        MyAllReduce->mReductionBuffer[ i ] = 0;
      
      MyAllReduce->mDoneSetupForIncomingData = 1;
      MyAllReduce->mTotalDataReceived = 0;
      //gDebugReduceId++;
      }
    
    int Offset = Pkt->mOffset;
    int Count  = Pkt->mCount;
    
    unsigned long SourceNodeId;
    unsigned long SourceCoreId;
    
    PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, NULL );

    BegLogLine( NAMED_ALLREDUCE_LOGGING )
      << "NamedAllReduce::AllReduceFx:: "
      << " SenderNodeId: " << SourceNodeId
      << " SourceCoreId: " << SourceCoreId
      << " Offset: " << Offset
      << " Count: " << Count
      << " gpid: " << gpid
      << " TotalCount: " << TotalCount
      << " (sizeof( ReduceType) * TotalCount ): " << (sizeof( ReduceType) * TotalCount )
      << EndLogLine;
    
//     BegLogLine( PkNodeGetId() == 0 && MyAllReduce->debug_ptr != NULL )
//       << "*MyAllReduce->debug_ptr: " << (void *) *MyAllReduce->debug_ptr 
//       << EndLogLine;

    for( int i = 0; i < Count; i++ )
      {
      int Index = Offset + i;

      AssertLogLine( Index >= 0 && Index < (sizeof( ReduceType) * TotalCount ))
        << "NamedAllReduce::AllReduceFx:: "
        << " Index: " << Index
        << " (sizeof( ReduceType) * TotalCount ): " << (sizeof( ReduceType) * TotalCount )
        << " TotalCount: " << TotalCount
        << EndLogLine;

//       BegLogLine( NAMED_ALLREDUCE_LOGGING )
//         << "NamedAllReduce::AllReduceFx:: "
//         << " MyAllReduce->mReductionBuffer[ " << Index << "]: " 
//         << (void *) & MyAllReduce->mReductionBuffer[ Index ]        
//         << " MyAllReduce->debug_ptr: " << (void *) MyAllReduce->debug_ptr 
//         << EndLogLine;

      if( NAMED_ALLREDUCE_OPERATION == NAMED_ALLREDUCE_SUM )
        {
        MyAllReduce->mReductionBuffer[ Index ] += Pkt->mData[ i ];
        }
      else if( NAMED_ALLREDUCE_OPERATION == NAMED_ALLREDUCE_AND )
        {
        MyAllReduce->mReductionBuffer[ Index ] &= Pkt->mData[ i ];
        }
      else if( NAMED_ALLREDUCE_OPERATION == NAMED_ALLREDUCE_OR )
        {
        MyAllReduce->mReductionBuffer[ Index ] |= Pkt->mData[ i ];
        }
      else if( NAMED_ALLREDUCE_OPERATION == NAMED_ALLREDUCE_XOR )
        {
        MyAllReduce->mReductionBuffer[ Index ] ^= Pkt->mData[ i ];
        }
      else
        {
        StrongAssertLogLine( 0 )
          << "AllReduceFx:: Error:: operation: " << NAMED_ALLREDUCE_OPERATION
          << " not recognized"
          << EndLogLine;
        }
      }
    
//     BegLogLine( PkNodeGetId() == 0 && MyAllReduce->debug_ptr != NULL )
//       << "*MyAllReduce->debug_ptr: " << (void *) *MyAllReduce->debug_ptr 
//       << EndLogLine;

    MyAllReduce->mTotalDataReceived += Count;

//     BegLogLine( PkNodeGetId() == 0 && MyAllReduce->debug_ptr != NULL )
//       << "*MyAllReduce->debug_ptr: " << (void *) *MyAllReduce->debug_ptr 
//       << EndLogLine;
    
    if( Pkt->mIsLastPacket )
      {
      int NodeId  = SourceNodeId;
      
      BegLogLine( NAMED_ALLREDUCE_LOGGING )
        << "NamedAllReduce::AllReduceFx:: "
        << " NodeId: " << NodeId
        << " MyAllReduce->mNodeIdsInUseCount: " << MyAllReduce->mNodeIdsInUseCount
        // << " gDebugReduceId: " << gDebugReduceId
        << EndLogLine;

      MyAllReduce->mNodeIdsInUse[ MyAllReduce->mNodeIdsInUseCount ] = NodeId;
      MyAllReduce->mNodeIdsInUseCount++;
      }

//     BegLogLine( PkNodeGetId() == 0 && MyAllReduce->debug_ptr != NULL )
//       << "*MyAllReduce->debug_ptr: " << (void *) *MyAllReduce->debug_ptr 
//       << EndLogLine;

    if( MyAllReduce->mTotalDataReceived == (MyAllReduce->mNodeCount * Pkt->mTotalCount ))
      {
      MyAllReduce->mReduceCount = Pkt->mTotalCount;

//        BegLogLine( NAMED_ALLREDUCE_LOGGING )
// //          << "NamedAllReduce::AllReduceFx:: "
// //          << " MyAllReduce: " << MyAllReduce
// //          << " MyAllReduce->mTotalDataReceived: " << MyAllReduce->mTotalDataReceived
// //          << " MyAllReduce->mNodeCount * Pkt->mTotalCount: " << MyAllReduce->mNodeCount * Pkt->mTotalCount
// //          << " MyAllReduce->mNodeCount: " << MyAllReduce->mNodeCount
// //          << " Pkt->mTotalCount: " << Pkt->mTotalCount
//          << " gDebugReduceId: " << gDebugReduceId
//          << EndLogLine;

      AssertLogLine( MyAllReduce->mReductionBufferForFiber == NULL )
        << "ERROR::  reduction buffer for the fiber should be null"
        << " MyAllReduce->mReductionBufferForFiber: " << (void *) MyAllReduce->mReductionBufferForFiber
        // << " gDebugReduceId: " << gDebugReduceId
        << EndLogLine;
      
      MyAllReduce->mReductionBufferForFiber = (ReduceType *) PkHeapAllocate( sizeof( ReduceType) * Pkt->mTotalCount );
      
      memcpy( MyAllReduce->mReductionBufferForFiber,
              MyAllReduce->mReductionBuffer,
              sizeof( ReduceType ) * Pkt->mTotalCount );
      
      AssertLogLine( MyAllReduce->mReductionBuffer != NULL )
        << "ERROR:: Reduction buffer should not be NULL"
        << EndLogLine;
      
      PkHeapFree( MyAllReduce->mReductionBuffer );
      MyAllReduce->mReductionBuffer = NULL;

      MyAllReduce->mDoneSetupForIncomingData = 0;

      MyAllReduce->mFiberArgs.mThis = MyAllReduce;

      memcpy( MyAllReduce->mNodeIdsInUseForFiber,
              MyAllReduce->mNodeIdsInUse,
              (sizeof( int ) * MyAllReduce->mNodeIdsInUseCount) );

      MyAllReduce->mNodeIdsInUseCountForFiber = MyAllReduce->mNodeIdsInUseCount;
      
      BegLogLine( NAMED_ALLREDUCE_LOGGING )
        << "NamedAllReduce::AllReduceFx:: "
        << " MyAllReduce: " << MyAllReduce
        << " MyAllReduce->mNodeIdsInUseForFiber: " << (void *) MyAllReduce->mNodeIdsInUseForFiber
        << " MyAllReduce->mNodeIdsInUseCount: " << MyAllReduce->mNodeIdsInUseCount
        << " MyAllReduce->mReduceCount: " << MyAllReduce->mReduceCount
        << " MyAllReduce->mTotalDataReceived: " << MyAllReduce->mTotalDataReceived
        << " MyAllReduce->mNodeCount * Pkt->mTotalCount: " << MyAllReduce->mNodeCount * Pkt->mTotalCount
        << " MyAllReduce->mNodeCount: " << MyAllReduce->mNodeCount
        << " Pkt->mTotalCount: " << Pkt->mTotalCount
        << EndLogLine;

      MyAllReduce->mNodeIdsInUseCount = 0;

      // Received all the data
      PkFiberCreate( 128*1024, 
                     SendResultsBackFiber,
                     (void *) &MyAllReduce->mFiberArgs );
      }

    return ( NULL );
    }
              
  public:
//  NamedAllReduce() {}
//
//  ~NamedAllReduce() 
//     {
//     Finalize();
//     }

  void
  Finalize()
   {
   mMyBarrier.Execute();
   // PkHeapFree( this );
   }

  void
  Init( char* aName, int aNodeCount )
    {
    //gDebugReduceId    = -1 ;

    mNodeCount = aNodeCount;
    
    StrongAssertLogLine( strlen( aName ) < MAX_NAME_LENGTH )
      << "NamedAllReduce::Init:: ERROR:: strlen( aName ) < MAX_NAME_LENGTH: "  
      << " strlen( aName ): " << strlen( aName )
      << " MAX_NAME_LENGTH: " << MAX_NAME_LENGTH
      << EndLogLine;

    strcpy( mName, aName );
    
    mDoneSetupForIncomingData = 0;
    mReductionBuffer          = NULL;
    mReductionBufferForFiber  = NULL;
    mTotalDataReceived        = 0;

    mNodeIdsInUse         = (int *) PkHeapAllocate( sizeof( int ) * aNodeCount );
    mNodeIdsInUseForFiber = (int *) PkHeapAllocate( sizeof( int ) * aNodeCount );

    mNodeIdsInUseCount = 0;

    mSemaphore.Init( 0 );


    GlobalParallelObject::Client::RegisterGlobalParallelObject<NamedAllReduce>( this,
                                                                                mName,
                                                                                mNodeCount,
                                                                                mMyGlobalParallelId,
                                                                                mRoot );
    
    char BarName[ 16 ];
    sprintf( BarName, "BARR%d", mMyGlobalParallelId );
        
    mMyBarrier.Init( BarName, aNodeCount );
    
    StrongAssertLogLine( sizeof( PreAllReduceDataPacket ) <= PK_ACTOR_MAX_PACKET_PAYLOAD_SIZE )
      << "NamedAllReduce::Init:: ERROR::  "
      << " sizeof( PreAllReduceDataPacket ): " << sizeof( PreAllReduceDataPacket )
      << " PK_ACTOR_MAX_PACKET_PAYLOAD_SIZE: " << PK_ACTOR_MAX_PACKET_PAYLOAD_SIZE
      << EndLogLine;

    int SizeLeftForData = PK_ACTOR_MAX_PACKET_PAYLOAD_SIZE - 
      (sizeof(PreAllReduceDataPacket ) - sizeof( ReduceType ));

    mMaxDataCount = SizeLeftForData / sizeof( ReduceType );

    StrongAssertLogLine( mMaxDataCount > 0 )
      << "NamedAllReduce::Init:: ERROR::  "
      << " mMaxDataCount: " << mMaxDataCount
      << EndLogLine;
    }

  void
  Execute(ReduceType* aInData, int aTCount, unsigned* aDebugArg=NULL )
    {    
    BegLogLine( NAMED_ALLREDUCE_LOGGING )
      << "NamedAllReduce::Execute:: Entering... "
      << EndLogLine;

    // debug_ptr = aDebugArg;

    BegLogLine( PkNodeGetId() == 0 && aDebugArg != NULL )
      << "*aDebugArg: " << (void *) *aDebugArg 
      << EndLogLine;

    mCurData  = aInData;
    mTotalCount = aTCount;
    mAmountReceivedBack = 0;

    int Offset = 0;
    int ElementsSent = 0;

    while( Offset < aTCount )
      {      
      int CountToSend = 0;
      
      if( ( Offset + mMaxDataCount ) <= aTCount )
        {
        CountToSend = mMaxDataCount;
        }
      else
        {
        // Last packet
        CountToSend = ( aTCount - Offset );
        }      

      int PacketSize = sizeof( PreAllReduceDataPacket ) + (CountToSend-1) * sizeof( ReduceType );

      BegLogLine( NAMED_ALLREDUCE_LOGGING )
        << "NamedAllReduce::Execute:: "
        << " PacketSize: " << PacketSize
        << " CountToSend: " << CountToSend
        << " mMaxDataCount: " << mMaxDataCount
        << " Offset: " << Offset
        << " aTCount: " << aTCount
        << " mRoot: " << mRoot
        << " PK_ACTOR_MAX_PACKET_PAYLOAD_SIZE: " << PK_ACTOR_MAX_PACKET_PAYLOAD_SIZE
        << EndLogLine;

      BegLogLine( PkNodeGetId() == 0 && aDebugArg != NULL )
        << "*aDebugArg: " << (void *) *aDebugArg 
        << " Offset: " << Offset
        << EndLogLine;

      PreAllReduceDataPacket * Pkt = (PreAllReduceDataPacket *)
        PkActorReservePacket( mRoot,
                              0,
                              AllReduceFx,
                              PacketSize );
      
      Pkt->mGPID          = mMyGlobalParallelId;
      Pkt->mTotalCount    = mTotalCount;
      Pkt->mOffset        = Offset;     
      Pkt->mCount         = CountToSend;

      BegLogLine( PkNodeGetId() == 0 && aDebugArg != NULL )
        << "*aDebugArg: " << (void *) *aDebugArg 
        << " Offset: " << Offset
        << EndLogLine;

      memcpy( Pkt->mData, &aInData[ Offset ], sizeof( ReduceType ) * CountToSend );
      Offset += CountToSend;
      
      BegLogLine( PkNodeGetId() == 0 && aDebugArg != NULL )
        << "*aDebugArg: " << (void *) *aDebugArg 
        << " Offset: " << Offset
        << EndLogLine;      
      
      if( Offset == aTCount )
        Pkt->mIsLastPacket = 1;
      else
        Pkt->mIsLastPacket = 0;
      
      PkActorPacketDispatch( Pkt );
      
      PkFiberYield();

      BegLogLine( PkNodeGetId() == 0 && aDebugArg != NULL )
        << "*aDebugArg: " << (void *) *aDebugArg 
        << " Offset: " << Offset
        << EndLogLine;
      }
    
    BegLogLine( PkNodeGetId() == 0 && aDebugArg != NULL )
      << "*aDebugArg: " << (void *) *aDebugArg 
      << EndLogLine;

    BegLogLine( NAMED_ALLREDUCE_LOGGING )
      << "NamedAllReduce::Execute:: About to wait on a semaphore "
      << EndLogLine;    

    // Wait for the reduced data
    mSemaphore.Down();

    BegLogLine( NAMED_ALLREDUCE_LOGGING )
      << "NamedAllReduce::Execute:: Leaving "
      << EndLogLine;
    
    // Once this is unblocked, we're done with the barrier.
    return;    
    }
      
  char* 
  GetName()
    {
    return (char *) & mName[ 0 ];
    }
  };
#endif
