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
 #ifndef __MSG_THROBBER_IF__
#define __MSG_THROBBER_IF__

#ifndef PKFXLOG_MSG_THROBBER
//#define PKFXLOG_MSG_THROBBER ( Platform::Topology::GetAddressSpaceId() == 0 )
#define PKFXLOG_MSG_THROBBER ( 0 )
#endif

static TraceClient BcastAlltoallvStart;
static TraceClient BcastAlltoallvFinis;

static TraceClient ReduceAlltoallvStart;
static TraceClient ReduceAlltoallvFinis;

static TraceClient MsgBcastStart;
static TraceClient MsgBcastFinis;

static TraceClient MsgReduceStart;
static TraceClient MsgReduceFinis;


class MsgThrobberIF
  {
    struct FragDataRef
      {
      XYZ * mPositPtr;
      XYZ * mForcePtr;
      int   mSiteCount;
      int   mFragLocalIndex;
      };
      
    struct PositionMover
      {
      int        mFragId;
      XYZ*       mPosPtr;
      };
      
    FragDataRef*   mFragDataRef;
 
    int* mNodesToSendTo;
    int  mNodesToSendToCount;

    int* mNodesToRecvFrom;
    int  mNodesToRecvFromCount;        

    int  mFullBufferSize;         

    int  mFullBufferReduceSize;         
      
    int             mFragmentsInPartition;      
    
    // Upper bound on local fragmet buffer size
    int             mFragmentBufferCount;

    // Upper bound on the send and recv buffers
    int             mBufferSize;
      
    int*            mSendBuff;
    int*            mRecvBuff;
    int             mRecvSendBuffSize;

    int*          mSendCountsForBcast;
    int*          mSendOffsetsForBcast;
    int*          mRecvCountsForBcast;
    int*          mRecvOffsetsForBcast;

    int*          mSendCountsForReduce;
    int*          mSendOffsetsForReduce;
    int*          mRecvCountsForReduce;
    int*          mRecvOffsetsForReduce;

    // Needed to determine when to cull
    int           mPostAssignmentViolation;  

    int*            mFragmentsToSendPerNodeCounts;
    PositionMover** mFragmentsToSendPerNode;
    int*            mWereAnyFragmentsCulled;

    PositionMover*  mFullFragmentsToSend;
    int             mLocalFragmentsCount;

    int            mFragmentsInNeighborhoodCount;  
    int*           mFragmentsInNeighborhood;
      
    int            mUseNodeCountsFromLastCall;
    int            mCommRecookTS;  
      
  public:
    MsgThrobberIF() {};
      
    inline int DoesCulling();  

    inline void ReductionExecute( int aSimTick );
    inline void BroadcastExecute( int aSimTick, int aAssignmentViolation );

    inline int GetSiteCount( int aFragId );
    inline int GetFragsInHoodCount();
    inline int GetFragsInHood( int aFragOrd );

    inline XYZ* GetFragPositPtrLocal( int aFragId );
    inline XYZ* GetFragPositPtrInHood( int aFragId );


    inline XYZ* GetFragForcePtr( int aFragId );
    inline int GetFragIdInHood( int aFragIter );
    inline void SetGlobalFragInfo( int aFragId, int aSiteCount );
    inline void SetFragAsLocal( int aFragId );
    inline void ClearLocalFrags();
    inline void Pack();

    inline void SetupBufferFromFragList( int*    aBuffer, 
                                  int&    aBufferSize,
                                  int     aAllocatedBufferSize,
                                  PositionMover* aFragList,
                                  int            aFragListCount,
                                  int&           aReduceSize );
      
    inline void
    Init( int  aDimX, int aDimY, int aDimZ,
          int* aNodesToSendTo,
          int  aNodesToSendToCount,
          int  aFragmentsInPartition,
          int  aMaxLocalFragments,
          int  aMaxFragmentsInNeighborhood = -1 );

    inline void CompressFragmentList( PositionMover*  aFragsToMove,
                                      int&  aFragsToMoveCount );
      
    inline int GetStartOfPositions( int aFragmentCount );
          
  };

int
MsgThrobberIF::DoesCulling()
  {
  return 1;
  }

int
MsgThrobberIF::GetStartOfPositions( int aFragmentCount )
  {
  int BytesOfFragIds = ( aFragmentCount + 1) * sizeof( FragId );
  int NextDoubleAlignedByteOffsetFromFragIds = ROUND8( BytesOfFragIds );
  int StartOfPositions = NextDoubleAlignedByteOffsetFromFragIds / sizeof( int );

  return StartOfPositions;
  }

void
MsgThrobberIF::Init( int  aDimX, int aDimY, int aDimZ,
                     int* aNodesToSendTo,
                     int  aNodesToSendToCount,
                     int  aFragmentsInPartition,
                     int  aMaxLocalFragments,
                     int  aMaxFragmentsInNeighborhood )
  {

  mFragmentsInPartition = aFragmentsInPartition;

  mNodesToSendTo = aNodesToSendTo;
  mNodesToSendToCount = aNodesToSendToCount;   

  mNodesToRecvFrom = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
  if( mNodesToRecvFrom == NULL )
    PLATFORM_ABORT( "mNodesToRecvFrom == NULL" );

  // Alltoall to get the receive node list
  int* NodesToRecvFromSendBuff = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
  if( NodesToRecvFromSendBuff == NULL )
    PLATFORM_ABORT( "NodesToRecvFromSendBuff == NULL" );

  int* NodesToRecvFromRecvBuff = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
  if( NodesToRecvFromSendBuff == NULL )
    PLATFORM_ABORT( "NodesToRecvFromRecvBuff == NULL" );

  int* NodesCounts = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
  if( NodesCounts == NULL )
    PLATFORM_ABORT( "NodesCounts == NULL" );

  int* NodesOffsets = (int *) malloc( sizeof( int ) * Platform::Topology::GetAddressSpaceCount() );
  if( NodesOffsets == NULL )
    PLATFORM_ABORT( "NodesOffsets == NULL" );
  
  for( int i=0; i < Platform::Topology::GetAddressSpaceCount(); i++ )
    {
    NodesCounts[ i ] = 1;
    NodesOffsets[ i ] = i;
    
    NodesToRecvFromRecvBuff[ i ] = 0;
    NodesToRecvFromSendBuff[ i ] = 0;
    }

  for( int i=0; i < mNodesToSendToCount; i++ )
    {
    int Rank = mNodesToSendTo[ i ];
    NodesToRecvFromSendBuff[ Rank ] = 1;
    }

  Platform::Collective::Alltoallv( NodesToRecvFromSendBuff, NodesCounts, NodesOffsets, sizeof( int ),
                                   NodesToRecvFromRecvBuff, NodesCounts, NodesOffsets, sizeof( int ), 0, 0, 0 );

  mNodesToRecvFromCount = 0;
  for( int i=0; i < Platform::Topology::GetAddressSpaceCount(); i++ )
    {
    if( NodesToRecvFromRecvBuff[ i ] )
      {
      mNodesToRecvFrom[ mNodesToRecvFromCount ] = i;
      mNodesToRecvFromCount++;
      }
    }
  

  BegLogLine( PKFXLOG_MSG_THROBBER )
    << " mNodesToRecvFromCount: " << mNodesToRecvFromCount
    << " mNodesToSendToCount: " << mNodesToSendToCount
    << EndLogLine;

  free( NodesCounts );
  free( NodesOffsets );
  free( NodesToRecvFromSendBuff );
  free( NodesToRecvFromRecvBuff );

  // Allocate message buffers
  int SafetyFactor = 5;
  if( Platform::Topology::GetAddressSpaceCount() > aFragmentsInPartition )
    {
    SafetyFactor = 10;
    }

  mFragmentBufferCount = 2.0 * pow( 1.0 * Platform::Topology::GetAddressSpaceCount(), 1.0/3.0 )
      * SafetyFactor * aMaxLocalFragments;
  
  assert( mFragmentBufferCount != 0 );
  mFragmentBufferCount = ROUND32( mFragmentBufferCount );

  // NumberOfSites + fragId + VoxelId
  mBufferSize = ( ( SafetyFactor * aMaxLocalFragments )
                  * (( (sizeof( XYZ ) * MAX_SITES_IN_FRAGMENT ) + sizeof( FragId )) ) ) / sizeof( int );

  assert( mBufferSize != 0 );
  
  // This is needed for the proper alignment in the alltoallv
  mBufferSize = ROUND32( mBufferSize );  

  mFragDataRef = (FragDataRef *) malloc( sizeof( FragDataRef ) * aFragmentsInPartition );  
  if( mFragDataRef == NULL )
    PLATFORM_ABORT("mFragDataRef == NULL");

  mFragmentsInNeighborhood = (int * ) malloc( sizeof( int ) * aFragmentsInPartition );
  if( mFragmentsInNeighborhood == NULL )
    PLATFORM_ABORT("mFragmentsInNeighborhood == NULL");

  int MaxRecvSendSize = (int) max( mNodesToSendToCount, mNodesToRecvFromCount+1 );
  
  mRecvSendBuffSize = MaxRecvSendSize * mBufferSize; 

  mRecvBuff = ( int * ) malloc( mRecvSendBuffSize * sizeof( int ) );  
  if( mRecvBuff == NULL )
    {
    PLATFORM_ABORT("mRecvBuff == NULL");                
    } 

  mSendBuff = ( int * ) malloc( mRecvSendBuffSize * sizeof( int ) );

  if( mSendBuff == NULL )
    {
    PLATFORM_ABORT("mSendBuff == NULL");                
    }

  
  // Allocate the Bcast alltoallv structurs
  mSendCountsForBcast = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mSendCountsForBcast == NULL )
    PLATFORM_ABORT( "mSendCountsForBcast == NULL" );

  mSendOffsetsForBcast = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mSendOffsetsForBcast == NULL )
    PLATFORM_ABORT( "mSendOffsetsForBcast == NULL" );

  mRecvCountsForBcast = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mRecvCountsForBcast == NULL )
    PLATFORM_ABORT( "mRecvCountsForBcast == NULL" );

  mRecvOffsetsForBcast = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mRecvOffsetsForBcast == NULL )
    PLATFORM_ABORT( "mRecvOffsetsForBcast == NULL" );


  // Allocate the Reduce alltoallv structurs
  mSendCountsForReduce = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mSendCountsForReduce == NULL )
    PLATFORM_ABORT( "mSendCountsForReduce == NULL" );

  mSendOffsetsForReduce = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mSendOffsetsForReduce == NULL )
    PLATFORM_ABORT( "mSendOffsetsForReduce == NULL" );

  mRecvCountsForReduce = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mRecvCountsForReduce == NULL )
    PLATFORM_ABORT( "mRecvCountsForReduce == NULL" );

  mRecvOffsetsForReduce = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
  if( mRecvOffsetsForReduce == NULL )
    PLATFORM_ABORT( "mRecvOffsetsForReduce == NULL" );

  for( int i=0; i<Platform::Topology::GetAddressSpaceCount(); i++ )
    {
    mSendCountsForBcast[ i ] = 0;
    mSendOffsetsForBcast[ i ] = 0;
    mRecvCountsForBcast[ i ] = 0;
    mRecvOffsetsForBcast[ i ] = 0;

    mSendCountsForReduce[ i ] = 0;
    mSendOffsetsForReduce[ i ] = 0;
    mRecvCountsForReduce[ i ] = 0;
    mRecvOffsetsForReduce[ i ] = 0;
    }

  for( int r = 0; r < mNodesToSendToCount; r++ )
    {
    int SendRank = mNodesToSendTo[ r ];
    mRecvOffsetsForReduce[ SendRank ] = r * mBufferSize;
    }

  for( int r = 0; r < mNodesToRecvFromCount; r++ )
    {
    int RecvRank = mNodesToRecvFrom[ r ];
    mRecvOffsetsForBcast[ RecvRank ] = r * mBufferSize;
    mSendOffsetsForReduce[ RecvRank ] = r * mBufferSize;
    }

  mPostAssignmentViolation = 0;

  mFragmentsToSendPerNodeCounts = ( int * ) malloc( (mNodesToSendToCount) * sizeof( int ) );
  if( mFragmentsToSendPerNodeCounts == NULL )
    {
    PLATFORM_ABORT("mFragmentsToSendPerNodeCounts == NULL");    
    }

  mFragmentsToSendPerNode = (PositionMover **) malloc( (mNodesToSendToCount) * sizeof( PositionMover* ) );
  if( mFragmentsToSendPerNode == NULL )
    {
    PLATFORM_ABORT("mFragmentsToSendPerNode == NULL");
    }

  for( int i = 0; i < mNodesToSendToCount; i++ )
    {
    mFragmentsToSendPerNode[ i ] = (PositionMover *) malloc( sizeof( PositionMover ) * mFragmentBufferCount );
    if( mFragmentsToSendPerNode[ i ] == NULL )
      PLATFORM_ABORT("mFragmentsToSendPerNode[ i ] == NULL");
    }

  mWereAnyFragmentsCulled = (int *) malloc( sizeof( int ) * mNodesToSendToCount );
  if( mWereAnyFragmentsCulled == NULL )
    PLATFORM_ABORT( "mWereAnyFragmentsCulled == NULL" );

  mFullFragmentsToSend = (PositionMover *) malloc( sizeof( PositionMover ) * mFragmentBufferCount );
  if( mFullFragmentsToSend == NULL )
    PLATFORM_ABORT( "mFullFragmentsToSend == NULL" );
  
  mLocalFragmentsCount = 0;
  mFragmentsInNeighborhoodCount = 0;
  mFullBufferReduceSize = 0;

  return;
  }

int MsgThrobberIF::GetFragIdInHood( int aFragIter )
  {
  assert( aFragIter >=0 && aFragIter < mFragmentsInNeighborhoodCount );
  return mFragmentsInNeighborhood[ aFragIter ];
  }

XYZ* MsgThrobberIF::GetFragPositPtrLocal( int aFragIter )
  {
  assert( mFragDataRef );
  assert( aFragIter >= 0 && aFragIter < NUMBER_OF_FRAGMENTS );
  int FragLocalIndex      = mFragDataRef[ aFragIter ].mFragLocalIndex;
  XYZ* PositPtr = mFullFragmentsToSend[ FragLocalIndex ].mPosPtr;
  assert( PositPtr );
  return( PositPtr );  
  }

XYZ* MsgThrobberIF::GetFragPositPtrInHood( int aFragIter )
  {
  assert( mFragDataRef );
  assert( aFragIter >= 0 && aFragIter < NUMBER_OF_FRAGMENTS );
  XYZ * PositPtr      = mFragDataRef[ aFragIter ].mPositPtr;
  assert( PositPtr );
  return( PositPtr );  
  }

XYZ* MsgThrobberIF::GetFragForcePtr( int aFragIter )
  {
  assert( mFragDataRef );
  assert( aFragIter >= 0 && aFragIter < NUMBER_OF_FRAGMENTS );
  XYZ * ForcePtr      = mFragDataRef[ aFragIter ].mForcePtr;
  assert( ForcePtr );
  return( ForcePtr );
  }

void MsgThrobberIF::SetGlobalFragInfo( int aFragId, int aSiteCount )
  {
  assert( mFragDataRef );
  assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
  mFragDataRef[ aFragId ].mSiteCount = aSiteCount;
  }

int MsgThrobberIF::GetSiteCount( int aFragId )
  {
  assert( mFragDataRef );
  assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
  return mFragDataRef[ aFragId ].mSiteCount;
  }

void MsgThrobberIF::SetupBufferFromFragList( int*    aBuffer, 
                                             int&    aBufferSize,
                                             int     aAllocatedBufferSize,
                                             PositionMover* aFragList,
                                             int            aFragListCount,
                                             int&           aReduceSize )
  {
  
  if( aFragListCount == 0 )
    {
    aBufferSize = 0;
    aReduceSize = 0;
    return;
    }

  FragId* BufferPtr = (FragId *) aBuffer;  

  // First entry is the number of fragments in the message
  BufferPtr[ 0 ] = aFragListCount;
  
  // Figure out the start of the run of position
  
  aBufferSize = GetStartOfPositions( aFragListCount );

  assert( aBufferSize >= 0 && aBufferSize < aAllocatedBufferSize );
  XYZ* PositionStartPtr = (XYZ *) &aBuffer[ aBufferSize ];
  assert( PositionStartPtr );
  
  aReduceSize = 0;

  for( int i=0; i < aFragListCount; i++ )
    {
    FragId fId = aFragList[ i ].mFragId;
    
    assert( fId >= 0 && fId < mFragmentsInPartition );
    int SiteCount = mFragDataRef[ fId ].mSiteCount;
    
    BufferPtr[i+1] = fId;
    aFragList[ i ].mPosPtr = PositionStartPtr;

    PositionStartPtr += SiteCount;
    aReduceSize += ( sizeof( XYZ ) * SiteCount ) / sizeof( int ) ;
    }
  
  aBufferSize += aReduceSize;
  }

void MsgThrobberIF::Pack()
  {
  //
  SetupBufferFromFragList( mSendBuff, mFullBufferSize, mBufferSize,
                           mFullFragmentsToSend, mLocalFragmentsCount, mFullBufferReduceSize );

//   for( int i=0; i < mLocalFragmentsCount; i++ )
//     {
//     FragId fId = mFullFragmentsToSend[ i ].mFragId;
    
//     mFragDataRef[ fId ].mPositPtr = mFullFragmentsToSend[ i ].mPosPtr;
//     }
  }

void MsgThrobberIF::SetFragAsLocal( int aFragId )
  {
  assert( mFullFragmentsToSend );

  assert( aFragId >= 0 && aFragId < mFragmentsInPartition );
  assert( mLocalFragmentsCount >= 0 && mLocalFragmentsCount < mFragmentsInPartition );
  
  mFullFragmentsToSend[ mLocalFragmentsCount ].mFragId = aFragId;
  mFragDataRef[ aFragId ].mFragLocalIndex = mLocalFragmentsCount;
  mLocalFragmentsCount++;
  return;
  }

inline 
void 
MsgThrobberIF::CompressFragmentList( PositionMover*  aFragsToMove,
                                     int&  aFragsToMoveCount )
  {
  // Iterate over the aFragsToMove list and take out all the 
  // culled fragments with a negative fragment id
  int ValidCounter = 0;
  int ind = 0;

  while( ind < aFragsToMoveCount )
    {
    // Find the next valid fragid and copy it into 
    // the place pointer to by ValidCounter
    int fId = aFragsToMove[ ind ].mFragId;
    
    if( fId != 0xffff )
      {
      aFragsToMove[ ValidCounter ] = aFragsToMove[ ind ];
      ValidCounter++;    
      }
    
    ind++;
    }

  aFragsToMoveCount = ValidCounter;
  }

int MsgThrobberIF::GetFragsInHoodCount()
  {
  return mFragmentsInNeighborhoodCount;
  }

void MsgThrobberIF::ClearLocalFrags()
  {
  mLocalFragmentsCount = 0;    
  }

void MsgThrobberIF::BroadcastExecute( int aSimTick, int aAssignmentViolation )
  {
  MsgBcastStart.HitOE( PKTRACE_MSG_COMM_BCAST,
                       "MsgCommBroadcast",
                       Platform::Topology::GetAddressSpaceId(),
                       MsgBcastStart );

  BegLogLine( PKFXLOG_MSG_THROBBER )
//  BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
    << "MsgThrobberIF::BroadcastExecute:: Entering with SimTick=" << aSimTick
    << " AssignmentViolation: " << aAssignmentViolation
    << EndLogLine;

  // We can assume to have ALL the fragment positions in a buffer some where
  // On AssignmentViolation==1 send all fragments to all out communicating partners  
  int AlltoallvRecvCountOverpost = 0;
  mCommRecookTS = aAssignmentViolation;
  if( aAssignmentViolation )
    {    
    BegLogLine( PKFXLOG_MSG_THROBBER )
      << "MsgThrobberIF::BroadcastExecute:: Entering with SimTick=" << aSimTick
      << " mFullBufferSize: " << mFullBufferSize
      << EndLogLine;

    for( int s = 0; s < mNodesToSendToCount; s++ )
      {      
      int SendRank = mNodesToSendTo[ s ];
      mRecvCountsForReduce[ SendRank ] = mFullBufferReduceSize;
      mSendCountsForBcast[ SendRank ] = mFullBufferSize;
      mSendOffsetsForBcast[ SendRank ] = 0;

        // When there's nothing to send to a node, a signal representing that must be sent
      if( mSendCountsForBcast[ SendRank ] == 0 )
        mSendCountsForBcast[ SendRank ] = -1;

      BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mSendCountsForBcast[" << SendRank
        << "]=" << mSendCountsForBcast[ SendRank ]
        << EndLogLine ;
      // Setup culling meta-data to be filled out during the reduce
      mWereAnyFragmentsCulled[ s ] = 0;
      mFragmentsToSendPerNodeCounts[ s ] = mLocalFragmentsCount;
      memcpy( mFragmentsToSendPerNode[ s ], mFullFragmentsToSend, mLocalFragmentsCount * sizeof( PositionMover ) );
      }
    
    for( int r = 0; r < mNodesToRecvFromCount; r++ )
      {
      int RecvRank = mNodesToRecvFrom[ r ];
      
      mRecvCountsForBcast[ RecvRank ] = 1;
      BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mRecvCountsForBcast[" << RecvRank
        << "]=" << mRecvCountsForBcast[ RecvRank ]
        << EndLogLine ;
      }
    
    AlltoallvRecvCountOverpost = 1;
    }
  else if( mPostAssignmentViolation )
    {
    // Culling happens here
    
    int NumOfUniqueBuffers = 1; // Full buffer is the first unique buffer
    for( int s = 0; s < mNodesToSendToCount; s++ )
      {
      int SendRank = mNodesToSendTo[ s ];
      if( mWereAnyFragmentsCulled[ s ] )
        {
        // Compress the list of fragments, create a send buffer, update Alltoallv counts and offsets
        CompressFragmentList( mFragmentsToSendPerNode[ s ], mFragmentsToSendPerNodeCounts[ s ] );
        
        // The full buffer is in the fi
        int SendOffset = NumOfUniqueBuffers * mBufferSize;
        
        mSendOffsetsForBcast[ SendRank ] = SendOffset;
        
        SetupBufferFromFragList( &mSendBuff[ SendOffset ], mSendCountsForBcast[ SendRank ], mBufferSize,
                                 mFragmentsToSendPerNode[ s ], mFragmentsToSendPerNodeCounts[ s ], mRecvCountsForReduce[ SendRank ] );
        
        // When there's nothing to send to a node, a signal representing that must be sent
        if( mSendCountsForBcast[ SendRank ] == 0 )
          mSendCountsForBcast[ SendRank ] = -1;
        
        BegLogLine( PKFXLOG_MSG_THROBBER )
          << "MsgThrobberIF::BroadcastExecute:: Entering with SimTick=" << aSimTick
          << " mSendCountsForBcast[ " << SendRank << " ]: " << mSendCountsForBcast[ SendRank ]
          << EndLogLine;
        
        NumOfUniqueBuffers++;
        }
      }
    
    AlltoallvRecvCountOverpost = 1;
    }

  // Fill full buffer fragment id data
  FragId* FullBufferFragPtr = (FragId *) mSendBuff;
  FullBufferFragPtr[ 0 ] = mLocalFragmentsCount;
  
  for( int i=0; i<mLocalFragmentsCount; i++ )
    {
    FullBufferFragPtr[ i+1 ] = mFullFragmentsToSend[ i ].mFragId;
    }

  // Copy in positions for culled nodes.
  int NumOfUniqueBuffers = 1; // Full buffer is the first unique buffer
  int CompletelyCulled = 0;
  for( int s = 0; s < mNodesToSendToCount; s++ )
    {
    if( mWereAnyFragmentsCulled[ s ] )
      {      
      int SendOffset = NumOfUniqueBuffers * mBufferSize;

      assert( SendOffset >= 0 && SendOffset < mRecvSendBuffSize );

      FragId* CulledBufferFragPtr = (FragId *) &mSendBuff[ SendOffset ];
      CulledBufferFragPtr[ 0 ] = mFragmentsToSendPerNodeCounts[ s ];
      
#if 0
      if( CulledBufferFragPtr[ 0 ] == 0 )
        {
        CompletelyCulled++;
        int SendRank = mNodesToSendTo[ s ];        
        if( !aAssignmentViolation && ! mPostAssignmentViolation            
            && mSendCountsForBcast[ SendRank ] != 0 )
          PLATFORM_ABORT( "ERROR: MsgThrobber sending an empty message" );
#if 0        
        continue;
#endif
        }
#endif

      for( int i=0; i < mFragmentsToSendPerNodeCounts[ s ]; i++ )
        {
        int fId = mFragmentsToSendPerNode[ s ][ i ].mFragId;
        
        CulledBufferFragPtr[ i+1 ] = (FragId) fId;

        XYZ* PosPtrSrc   = GetFragPositPtrLocal( fId );
        XYZ* PosPtrTrg   = mFragmentsToSendPerNode[ s ][ i ].mPosPtr;
        int SiteCount = GetSiteCount( fId );

        memcpy_xyz( PosPtrTrg, PosPtrSrc, SiteCount );
        }
      
      NumOfUniqueBuffers ++;
      }
    }

#if 0
  int data_volume = 0;
  for( int i=0; i<Platform::Topology::GetAddressSpaceCount(); i++ )
    {
    data_volume += mSendCountsForBcast[i] * sizeof( int );
    }
   
  BegLogLine( PKFXLOG_MSG_THROBBER )
    << "MsgThrobberIF::BroadcastExecute:: Entering with SimTick=" << aSimTick
    << " CompletelyCulled: " << CompletelyCulled
    << " data_volume: " << data_volume
    << " NumOfUniqueBuffers: " << NumOfUniqueBuffers
    << EndLogLine;
#endif
  
  // On PostAssignmentViolation time step create a set of fragment position  
  BcastAlltoallvStart.HitOE( PKTRACE_MSG_COMM_BCAST_ALLTOALLV,
                             "MsgBcastAlltoallv",
                             Platform::Topology::GetAddressSpaceId(),
                             BcastAlltoallvStart );          

  // If this is a simple old time step use the cached recv/send
  // node counts from last call.
  mUseNodeCountsFromLastCall = ! AlltoallvRecvCountOverpost;

  Platform::Collective::Alltoallv( (void*) mSendBuff,
                                   mSendCountsForBcast,
                                   mSendOffsetsForBcast,
                                   sizeof( int ),
                                   (void*) mRecvBuff,
                                   mRecvCountsForBcast,
                                   mRecvOffsetsForBcast,
                                   sizeof( int ), 0,
                                   AlltoallvRecvCountOverpost, 
                                   0, 
                                   mUseNodeCountsFromLastCall,
                                   1 );

  BcastAlltoallvFinis.HitOE( PKTRACE_MSG_COMM_BCAST_ALLTOALLV,
                             "MsgBcastAlltoallv",
                             Platform::Topology::GetAddressSpaceId(),
                             BcastAlltoallvFinis );          

  
  mFragmentsInNeighborhoodCount = 0;
  // Parse out incomming buffers to set up Pos and Force Ptrs
  {
  for( int i=0; i<Platform::Topology::GetAddressSpaceCount(); i++ )
    {
      BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mSendCountsForReduce[" << i
        << "] was " << mSendCountsForReduce[ i ]
          << " addr=0x" << (void *)(mSendCountsForReduce+i)                                          	                          
        << EndLogLine ;
//    mSendCountsForReduce[ i ] = 0;
//    mSendOffsetsForReduce[ i ] = 0;
    }
  }
  for( int r = 0; r < mNodesToRecvFromCount; r++ )
    {
    int RecvRank = mNodesToRecvFrom[ r ];
    assert( RecvRank >= 0 && RecvRank < Platform::Topology::GetAddressSpaceCount() );

    int RecvCount = mRecvCountsForBcast[ RecvRank ];
    BegLogLine( PKFXLOG_MSG_THROBBER )
      << "Throbber mRecvCountsForBcast[" << RecvRank
      << "]=" << mRecvCountsForBcast[ RecvRank ]
      << EndLogLine ;
    if( RecvCount == 0 )
      {
      mSendCountsForReduce[ RecvRank ] = 0;      
    BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mSendCountsForReduce[" << RecvRank
        << "] setting to " << mSendCountsForReduce[ RecvRank ]
        << EndLogLine ;
      continue;
      }
    
    int BuffOffset = mRecvOffsetsForBcast[ RecvRank ];
    
    assert( BuffOffset >= 0 && BuffOffset < mRecvSendBuffSize );
    FragId* FragIdPtr = (FragId *) &mRecvBuff[ BuffOffset ];

    int NumberOfFragments = FragIdPtr[ 0 ];
    
    int StartOfPositions = GetStartOfPositions( NumberOfFragments ) + BuffOffset;
    
    assert( StartOfPositions >= 0 && StartOfPositions < mRecvSendBuffSize );

    XYZ* PosPtr = (XYZ *) &mRecvBuff[ StartOfPositions ];    

    int StartOfForces = mBufferSize * r;
    assert( StartOfForces >= 0 && StartOfForces < mRecvSendBuffSize );

    XYZ* ForcePtr = (XYZ *) &mSendBuff[ StartOfForces ];
    
    mSendCountsForReduce[ RecvRank ] = 0;
    BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mSendCountsForReduce[" << RecvRank
        << "] setting to " << mSendCountsForReduce[ RecvRank ]
        << EndLogLine ;

    for( int i=0; i < NumberOfFragments; i++ )
      {
      FragId fId = FragIdPtr[i+1];
      
      assert( fId >= 0 && fId < mFragmentsInPartition );
      
      mFragmentsInNeighborhood[ mFragmentsInNeighborhoodCount ] = fId;
      mFragmentsInNeighborhoodCount++;
      
      mFragDataRef[ fId ].mPositPtr = PosPtr;
      mFragDataRef[ fId ].mForcePtr = ForcePtr;
      
      int SiteCount = GetSiteCount( fId );
      PosPtr += SiteCount;
      ForcePtr += SiteCount;
      mSendCountsForReduce[ RecvRank ] += ( SiteCount * sizeof( XYZ ) / sizeof( int ) );
    BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mSendCountsForReduce[" << RecvRank
        << "] setting to " << mSendCountsForReduce[ RecvRank ]
        << EndLogLine ;
      }
      BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mSendCountsForReduce[" << RecvRank
        << "]=" << mSendCountsForReduce[ RecvRank ]
        << EndLogLine ;
    }
    
  {
  for( int i=0; i<Platform::Topology::GetAddressSpaceCount(); i++ )
    {

      BegLogLine( PKFXLOG_MSG_THROBBER )
        << "Throbber mSendCountsForReduce[" << i
        << "] is " << mSendCountsForReduce[ i ]
          << " mSendOffsetsForReduce is "	<< mSendOffsetsForReduce[ i ]                           
        << EndLogLine ;
    }
  }
  mPostAssignmentViolation = aAssignmentViolation;

  BegLogLine( PKFXLOG_MSG_THROBBER )
    << "MsgThrobberIF::BroadcastExecute:: Leaving..." 
    << EndLogLine;

  MsgBcastFinis.HitOE( PKTRACE_MSG_COMM_BCAST,
                       "MsgCommBroadcast",
                       Platform::Topology::GetAddressSpaceId(),
                       MsgBcastFinis );
  }

void MsgThrobberIF::ReductionExecute( int aSimTick )
  {
  MsgReduceStart.HitOE( PKTRACE_MSG_COMM_REDUCE,
                       "MsgCommReduce",
                       Platform::Topology::GetAddressSpaceId(),
                       MsgReduceStart );

  BegLogLine( PKFXLOG_MSG_THROBBER )
    << "MsgThrobberIF::ReductionExecute:: Entering..." 
    << EndLogLine;
  if( PKFXLOG_MSG_THROBBER )
    {
    int AddressSpaceCount = Platform::Topology::GetAddressSpaceCount();
    for( int i=0; i<AddressSpaceCount; i++ )
       {
        BegLogLine( PKFXLOG_MSG_THROBBER )
          << "Throbber mSendCountsForReduce[" << i
          << "] is " << mSendCountsForReduce[ i ]
          << EndLogLine ;
       }
    }
  // Believe it or not, but the Alltoallv for the reduction has already been setup
  ReduceAlltoallvStart.HitOE( PKTRACE_MSG_COMM_REDUCE_ALLTOALLV,
                             "MsgReduceAlltoallv",
                             Platform::Topology::GetAddressSpaceId(),
                             ReduceAlltoallvStart );          

  Platform::Collective::Alltoallv( (void*) mSendBuff,
                                   mSendCountsForReduce,
                                   mSendOffsetsForReduce,
                                   sizeof( int ),
                                   (void*) mRecvBuff,
                                   mRecvCountsForReduce,
                                   mRecvOffsetsForReduce,
                                   sizeof( int ), 0, 0, 1,  // Counts are known.
                                   mUseNodeCountsFromLastCall, 2 );

  ReduceAlltoallvFinis.HitOE( PKTRACE_MSG_COMM_REDUCE_ALLTOALLV,
                              "MsgReduceAlltoallv",
                              Platform::Topology::GetAddressSpaceId(),
                              ReduceAlltoallvFinis );          

  // Setup the force accumulation buffer for local fragmets
  XYZ *AccForcePtr = (XYZ *) mSendBuff;
  for( int i=0; i < mLocalFragmentsCount; i++ )
    {
    FragId fId = mFullFragmentsToSend[ i ].mFragId;
    int SiteCount = GetSiteCount( fId );
    
    mFragDataRef[ fId ].mForcePtr = AccForcePtr;

    for( int j=0; j<SiteCount; j++ )
      {
      AccForcePtr[ j ].Zero();
      }
    
    AccForcePtr += SiteCount;
    }
  
  // Parse out all the Forces and add the up
  for( int r = 0; r < mNodesToSendToCount; r++ )
    {
    int RecvRank = mNodesToSendTo[ r ];

    assert( RecvRank >= 0 && RecvRank < Platform::Topology::GetAddressSpaceCount() );

    int BaseOffset = mRecvOffsetsForReduce[ RecvRank ];
    
    XYZ* ForcePtr = (XYZ *) &mRecvBuff[ BaseOffset ];

    int NumberOfFragments = mFragmentsToSendPerNodeCounts[ r ];
    
    for( int i = 0; i < NumberOfFragments; i++ )
      {
      FragId fId = mFragmentsToSendPerNode[ r ][ i ].mFragId;
      int SiteCount = GetSiteCount( fId );

      // Check for culling
      if( ForcePtr[ 0 ].mX == CULLED_VAL )
        {
//         BegLogLine( aSimTick == 0 )
// 	  << " Culling: "
// 	  << " from node: " << RecvRank
// 	  << " fId: " << fId
// 	  << EndLogLine;

        mFragmentsToSendPerNode[ r ][ i ].mFragId = 0xffff;
        mWereAnyFragmentsCulled[ r ] = 1;
        }
      else
        {
        // This one counts
        for( int j=0; j<SiteCount; j++ )
          {
          mFragDataRef[ fId ].mForcePtr[ j ] += ForcePtr[ j ];
          }
        }
      
      ForcePtr += SiteCount;
      }

  BegLogLine( PKFXLOG_MSG_THROBBER )
    << "MsgThrobberIF::ReductionExecute:: Leaving..." 
    << EndLogLine;
    }

  MsgReduceFinis.HitOE( PKTRACE_MSG_COMM_REDUCE,
                        "MsgCommReduce",
                        Platform::Topology::GetAddressSpaceId(),
                        MsgReduceFinis );
  }
#endif
