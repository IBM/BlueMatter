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
 #ifndef __SPARSE_SKIN_MANAGER_IF__
#define __SPARSE_SKIN_MANAGER_IF__

#ifndef PKFXLOG_SPARSE_SKIN
#define PKFXLOG_SPARSE_SKIN ( 0 )
#endif

#include <rptree.hpp>

template<int BITS>
class DenseSmallUnsignedIntArray
  {
  public:
  unsigned int *mMemory;

  private :
  unsigned int  mCount;
  unsigned int  mRequiredWords;

  enum { BITS_IN_WORD = (sizeof( unsigned int ) * 8) };
  enum { ELEMENT_MASK = ((0x01 << BITS) - 1 ) };

  public:

  void Init(void)
    {
    assert( BITS == 1 || BITS == 2 || BITS == 4 );
    mCount = 0;
    mMemory  = NULL;
    }

  public:

  void
  SetCount( unsigned int aCount )
    {
    BegLogLine(PKFXLOG_SETUP)
      << "DenseSmallUnsignedIntArray::SetCount() :"
      << " Setting to "
      << aCount
      << " From "
      << mCount
      << EndLogLine;

    if( mMemory != NULL )
      {
      assert( mCount != 0 );
      delete [] mMemory;
      mMemory = NULL;
      }

    mRequiredWords =  ((aCount * BITS) + BITS_IN_WORD - 1) / BITS_IN_WORD ;

    mMemory = new unsigned int [ mRequiredWords ];

    if( mMemory == NULL )
      PLATFORM_ABORT(" Failed to allocate small int array.");

    mCount = aCount;

    for( int i = 0; i < mRequiredWords; i++ )
      mMemory[i] = 0;
    }

  inline
  unsigned int
  GetElement( unsigned int aIndex )
    {
    assert( aIndex < mCount );
    int FirstBitOffset = aIndex * BITS;
    int WordIndex      = FirstBitOffset / BITS_IN_WORD;
    int Shift          = FirstBitOffset % BITS_IN_WORD;

    assert( WordIndex >=0 && WordIndex < mRequiredWords );
    unsigned int rc = ( mMemory[ WordIndex ] >> Shift ) & ELEMENT_MASK;
    return( rc );
    }

  inline
  void
  SetElement( unsigned int aIndex, unsigned int aValue )
    {
    assert( aIndex < mCount );
    assert( (aValue & ELEMENT_MASK) == aValue );
    int FirstBitOffset    = aIndex * BITS;
    int WordIndex         = FirstBitOffset / BITS_IN_WORD;
    int Shift             = FirstBitOffset % BITS_IN_WORD;
    unsigned int ORValue  = aValue << Shift;

    assert( WordIndex >= 0 && WordIndex < mRequiredWords );
    mMemory[ WordIndex ] |= ORValue;
    assert( aValue == GetElement( aIndex ) );
    }
  };

typedef unsigned short NodeId;

struct NodePair
  {
  NodeId mNodeIdA;
  NodeId mNodeIdB;
  };

typedef float LBWeightType;

struct NodePairRecord
  {
  NodePair        mNodePair;
  LBWeightType mWeight;
#ifndef PK_PHASE5_SKIP_OPT_SORT
  unsigned short             mOptCount;
#endif
  };

struct SparseSkinHelper
  {
  NodePairRecord* mNodePairRecPtr;
  };

#ifndef PK_PHASE5_SKIP_OPT_SORT
static int CompareSparseSkinOptList(const void *elem1, const void *elem2)
  {
  SparseSkinHelper* e1 = (SparseSkinHelper *) elem1;
  SparseSkinHelper* e2 = (SparseSkinHelper *) elem2;
  
  if( e1->mNodePairRecPtr->mOptCount < e2->mNodePairRecPtr->mOptCount )
    return -1;
  else if( e1->mNodePairRecPtr->mOptCount == e2->mNodePairRecPtr->mOptCount )
    return 0;
  else
    return 1;
  }
#endif

static int CompareInt1(const void *elem1, const void *elem2)
  {
  int* e1 = (int *) elem1;
  int* e2 = (int *) elem2;
  if ( *e1 < *e2 )
      return -1;
  else if( *e1 == *e2 )
      return 0;
  else
      return 1;
  }

static int CompareNodeId(const void *elem1, const void *elem2)
  {
  NodeId* e1 = (NodeId *) elem1;
  NodeId* e2 = (NodeId *) elem2;
  if ( *e1 < *e2 )
      return -1;
  else if( *e1 == *e2 )
      return 0;
  else
      return 1;
  }

struct AssignedWeightLink
  {
  AssignedWeightLink*  mNextLink;
  NodePairRecord       mNodePairRecord;  
  };

class NodeWeightList;

struct NodeAssignmentRecord
  {
  NodeId              mNodeId;
  LBWeightType               mTotalWeight;
  AssignedWeightLink* mAssignedWeightsFirst;
  AssignedWeightLink* mAssignedWeightsLast;
  NodeWeightList*     mNodeWeightLink;
  };

struct NodeWeightList
  {
  NodeAssignmentRecord*  mNodeAssignmentTablePtr;      
  NodeWeightList*        mNext;
  NodeWeightList*        mPrev;
  };

typedef NodeAssignmentRecord NodeAssignmentRecord_T;

struct WeightSortHelper
  {
  LBWeightType                   mWeight;
  NodeAssignmentRecord_T* mNodeAssignmentRec; 
  };

static int CompareWeightSortHelper(const void *elem1, const void *elem2)
  {
  WeightSortHelper* e1 = (WeightSortHelper *) elem1;
  WeightSortHelper* e2 = (WeightSortHelper *) elem2;

  if ( e1->mWeight > e2->mWeight )
      return -1;
  else if( e1->mWeight == e2->mWeight )
      return 0;
  else
      return 1;
  }

struct FinalAssignment
  {
  NodePair* mAssignedPairs;
  int       mAssignedPairsCount;
  int       mAssignedPairsCountAllocated;
  LBWeightType     mTotalWeight;
  };

class SparseSkinManagerIF
{
public:

   SparseSkinManagerIF() {}
  ~SparseSkinManagerIF() {}

  int*            mVoxelIndexToVoxelOrdinalMap;
  int*            mNodeIdToRecvNodeOrdinalMap;
  unsigned char** mAssignmentNodeTable;

  unsigned int    mVoxelInteractionTypeTableSize;

  DenseSmallUnsignedIntArray<1> mVoxelInteractionTypeTable;

  int                  mVoxelsInSkinHoodCount;

  NodeId* mMatchesForSparseListA;
  int     mMatchesForSparseListACount;

  NodeId* mMatchesForSparseListB;
  int     mMatchesForSparseListBCount;

  double*    mNodesScoreList;
  int*       mNodeIndexWithSameScoreList;
  int        mNodeIndexWithSameScoreListCount;

  int             mNodePairRecordsCount;

  int             mMaxVoxelsInRange;
  
  NodeAssignmentRecord_T* mFullSkinNodeAssignmentTable;
  NodeAssignmentRecord_T* mSparseSkinNodeAssignmentTable;

  int*    A2ARecvCount;
  int*    A2ARecvOffset;
  FragId* RecvFragmentBuffer;

  NodeId** mFullSkinTable;
  int      mNumberOfNodesOnFullSkin;

  int*     mSparseSkinsCounts;
  NodeId** mSparseSkins;

  int*     mSparseSkinSendNodes;
  int      mSparseSkinSendNodesCount;

#define BUFFER_CHUNK_SIZE ( 10 * 1024 * 1024 )
  char**  mMemoryBuffer;
  int     mMemoryBufferIndex;
  int     mMemoryBufferCount;

  char*   mCurrentBuffer;
  int     mCurrentBufferIndex;

#define NON_EXISTANCE_VAL -1
#define EXPAND_BLOCK (10 * 1024 * 1024)

  double*                    mExtents;
  XYZ*                       mFragCenters;
  IntegratorStateManagerIF*  mIntegratorStateManagerIF;
  ORBNode*                   mRecBisTree;
  double*                    mRadiusOfSphere;
  double*                    mGuardZone;

  int                        mCheckeredTemplate[ 2 ][ 2 ][ 2 ];

  unsigned int               mVoxDimX;
  unsigned int               mVoxDimY;
  unsigned int               mVoxDimZ;

  template<class T>
  void IntersectSortedList( T* aList1, int aList1Count,
			    T* aList2, int aList2Count,
			    NodeId* aOutput, int& aOutputCount )
  {
    // 3 cases
    // 1. List1[ i ] == List2[ j ] ==> goes on the output list, i++, j++
    // 2. List1[ i ]  < List2[ j ] ==> i++
    // 3. List1[ i ]  > List2[ j ] ==> j++

    int Counter1 = 0;
    int Counter2 = 0;
    aOutputCount = 0;
    
    int IntersectionCount = 0;
    int OneIntersection = 0;
    
    while( ( Counter1 < aList1Count ) && ( Counter2 < aList2Count ) )
      {
	T Elem1 = aList1[ Counter1 ];
	T Elem2 = aList2[ Counter2 ];
	
	assert( Elem1 >= 0 && Elem1 < Platform::Topology::GetAddressSpaceCount() );
	assert( Elem2 >= 0 && Elem2 < Platform::Topology::GetAddressSpaceCount() );
	
	if( Elem1 == Elem2 )
	  {
	    // Only put in odd nodes 
	    int x,y,z;
	    Platform::Topology::GetCoordsFromRank( Elem1, 
						   &x,
						   &y,
						   &z );          
	    
	    if( mCheckeredTemplate [ x & 0x1 ] [ y & 0x1 ][ z & 0x1 ] )             
	      {
		aOutput[ aOutputCount ] = Elem1;
		aOutputCount ++;
	      }
	    
	    OneIntersection = Elem1;
	    IntersectionCount++;
	    Counter1++;
	    Counter2++;
	  }
	else if( Elem1 < Elem2 )
	  {
	    Counter1++;
	  }
	else
	  {
	    Counter2++;
	  }
      }
    
    // If we had intersections, but everything was knocked out, put at least one node in
    if( (aOutputCount == 0) && (IntersectionCount > 0) )
      {
	aOutput[ aOutputCount ] = OneIntersection;
	aOutputCount ++;    
	BegLogLine( 1 )
	  << "Didn't really want to add a node, but was forced to" 
	  << EndLogLine;
      }  
  }
  
  void ExpandFinalAssignedPairList( FinalAssignment* aFinalAssingment )
  {
    void* Memory;
    int NewCount = 2 * aFinalAssingment->mAssignedPairsCountAllocated;
    int NewSize  = NewCount * sizeof( NodePair );
  
    Memory = malloc( NewSize );
    if( Memory == NULL )
      PLATFORM_ABORT( "Memory == NULL" );
  
    if( aFinalAssingment->mAssignedPairs != NULL )
      {
	memcpy( Memory, aFinalAssingment->mAssignedPairs, aFinalAssingment->mAssignedPairsCount * sizeof( NodePair ) );
	free( aFinalAssingment->mAssignedPairs );
      }

    aFinalAssingment->mAssignedPairs = (NodePair *) Memory;
    aFinalAssingment->mAssignedPairsCountAllocated = NewCount;
  }

  // Side effect method. mCurrentBuffer and mCurrentBufferIndex are effected
  void GetNextMemoryBuffer()
  {
    if( (mMemoryBufferIndex + 1) > mMemoryBufferCount )
      PLATFORM_ABORT( "(mMemoryBufferIndex + 1 ) > mMemoryBufferCount" );

    if( (mMemoryBufferIndex + 1 ) == mMemoryBufferCount )
      {
	// expand the memory buffer 
	int ExpandCount = 1;
	int NewCount = mMemoryBufferCount + ExpandCount;
	char** Memory = (char **) malloc( sizeof(char *) * NewCount );
	if( Memory == NULL )
	  PLATFORM_ABORT( "Memory == NULL" );
    
	memcpy( Memory, mMemoryBuffer, mMemoryBufferCount * sizeof( char * ));
	free( mMemoryBuffer );
	mMemoryBuffer = Memory;
    
	for( int i = mMemoryBufferCount; i < NewCount; i++ )
	  {
	    mMemoryBuffer[ i ] = (char *) malloc( sizeof(char) * BUFFER_CHUNK_SIZE );
	    if( mMemoryBuffer[ i ] == NULL )
	      PLATFORM_ABORT( "mMemoryBuffer[ i ] == NULL" );
	  }

	mMemoryBufferCount = NewCount;
      }

    mMemoryBufferIndex++;
    mCurrentBuffer = mMemoryBuffer[ mMemoryBufferIndex ];
    mCurrentBufferIndex = 0;
  }

  void BuildNodePairWeightTable()
  {
    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "sizeof( NodePairRecord ): " << sizeof( NodePairRecord )
      << " sizeof( AssignedWeightLink ): " << sizeof( AssignedWeightLink )
      << EndLogLine;
      
    int MachineSize = Platform::Topology::GetAddressSpaceCount();
  
    // Figure out the fragments on each node of our Recv list.
    // Send this nodes local fragment list to all the nodes on this node's skin.
    // Expect to recv from the RecvList of nodes
    int* A2ASendCount = (int *) malloc( sizeof(int) * MachineSize );
    if( A2ASendCount == NULL )
      PLATFORM_ABORT( "A2ASendCount == NULL" );

    int* A2ASendOffset = (int *) malloc( sizeof(int) * MachineSize );
    if( A2ASendOffset == NULL )
      PLATFORM_ABORT( "A2ASendOffset == NULL" );

    A2ARecvCount = (int *) malloc( sizeof(int) * MachineSize );
    if( A2ARecvCount == NULL )
      PLATFORM_ABORT( "A2ARecvCount == NULL" );

    A2ARecvOffset = (int *) malloc( sizeof(int) * MachineSize );
    if( A2ARecvOffset == NULL )
      PLATFORM_ABORT( "A2ARecvOffset == NULL" );
  
    unsigned long LocalFragmentCountMax = 0;
    unsigned long LocalFragmentCount = mIntegratorStateManagerIF->GetNumberOfLocalFragments();

    Platform::Collective::GlobalMax( &LocalFragmentCount, 
				     &LocalFragmentCountMax );

    BegLogLine( PKFXLOG_SPARSE_SKIN )
      << "LocalFragmentCountMax: " << LocalFragmentCountMax
      << EndLogLine;

    RecvFragmentBuffer = (FragId *) malloc( sizeof( FragId ) * LocalFragmentCountMax * MachineSize );
    if( RecvFragmentBuffer == NULL )
      PLATFORM_ABORT( "RecvFragmentBuffer == NULL" );
  
    for( int i=0; i<MachineSize; i++ )
      {
	A2ASendCount[ i ] = ( (LocalFragmentCount == 0) || (LocalFragmentCount == 1) ) ? 2 : LocalFragmentCount;
	A2ASendOffset[ i ] = 0;
	A2ARecvCount[ i ] =  1;
	A2ARecvOffset[ i ] = i * LocalFragmentCountMax; 
      }  
  
    FragId* LocalFragments = mIntegratorStateManagerIF->GetLocalFragmentPtr();
  
    if( LocalFragmentCount == 0 ) 
      {
	LocalFragments = (FragId *) malloc ( sizeof( FragId ) * 2 );
	if( LocalFragments == NULL )
	  PLATFORM_ABORT( "LocalFragments == NULL" );
    
	LocalFragments[ 0 ] = NON_EXISTANCE_VAL;
	LocalFragments[ 1 ] = NON_EXISTANCE_VAL;
      }
    else if( LocalFragmentCount == 1 ) 
      {
	FragId TheOneFragId = LocalFragments[ 0 ];
	LocalFragments = (FragId *) malloc ( sizeof( FragId ) * 2 );
	if( LocalFragments == NULL )
	  PLATFORM_ABORT( "LocalFragments == NULL" );
    
	LocalFragments[ 0 ] = TheOneFragId;
	LocalFragments[ 1 ] = NON_EXISTANCE_VAL;
      }

    BegLogLine( 0 )
      << "BuildNodePairWeightTable: About to run the Alltoallv: " 
      << "LocalFragmentCount: " << LocalFragmentCount
      << " LocalFragmentCountMax: " << LocalFragmentCountMax
      << EndLogLine;
  
    Platform::Collective::Alltoallv( LocalFragments,
				     A2ASendCount,
				     A2ASendOffset,
				     sizeof( FragId ),
				     RecvFragmentBuffer,
				     A2ARecvCount,
				     A2ARecvOffset,
				     sizeof( FragId ), 1, 1, 0 );
  
    BegLogLine( PKFXLOG_SPARSE_SKIN )
      << "BuildNodePairWeightTable: Done with the Alltoallv: " 
      << EndLogLine;

    LBWeightType MaxWeight = 0;
    LBWeightType MinWeight = 9999;
    LBWeightType AvgWeight = 0;
    int    TotalWeightCount = 0;
    
    mFullSkinNodeAssignmentTable = (NodeAssignmentRecord_T *) malloc( sizeof( NodeAssignmentRecord_T ) * MachineSize );
    if( mFullSkinNodeAssignmentTable == NULL )
      PLATFORM_ABORT( "mFullSkinNodeAssignmentTable == NULL" );
  
    for( int i=0; i<MachineSize; i++ )
      {
	mFullSkinNodeAssignmentTable[ i ].mNodeId               = i;
	mFullSkinNodeAssignmentTable[ i ].mTotalWeight          = 0.0;
	mFullSkinNodeAssignmentTable[ i ].mAssignedWeightsFirst = NULL;
	mFullSkinNodeAssignmentTable[ i ].mAssignedWeightsLast  = NULL;
	mFullSkinNodeAssignmentTable[ i ].mNodeWeightLink       = NULL;
      }        
  
    mNodePairRecordsCount=0;
  
    mMemoryBufferCount = 10;
    mMemoryBuffer      = (char **) malloc( sizeof( char* ) * mMemoryBufferCount );
    if( mMemoryBuffer == NULL )
      PLATFORM_ABORT( "mMemoryBuffer == NULL" );
  
    for( int i = 0; i < mMemoryBufferCount; i++ )
      {
	mMemoryBuffer[ i ] = (char *) malloc( sizeof(char) * BUFFER_CHUNK_SIZE );
	if( mMemoryBuffer[ i ] == NULL )
	  PLATFORM_ABORT( "mMemoryBuffer[ i ] == NULL" );
      }

    mCurrentBuffer = mMemoryBuffer[ 0 ];
    mCurrentBufferIndex = 0;
    mMemoryBufferIndex = 0;
  
    double RadiusSquared = (*mRadiusOfSphere) * (*mRadiusOfSphere);
    double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
    double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
  
    double Cutoff = SwitchLowerCutoff + SwitchDelta;
    
    int IntersectionListCount = 0;
    NodeId* IntersectionList = (NodeId *) malloc( sizeof( NodeId ) * MachineSize );
    if( IntersectionList == NULL )
      PLATFORM_ABORT( "IntersectionList == NULL" );    
  
    double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;
  
    double InRangeCutoff = SwitchUpperCutoff  ;
  
    double GuardZone = *mGuardZone;
  
    double GuardRange = InRangeCutoff + GuardZone;
    double GuardRangeSqr = GuardRange * GuardRange;
  
    // Store away the full skins for each node. 
    // This data structure gets freed after we create the sparse skins
  
    // Figure out the size of the skin lists for each node. 
    // They are the same with the even orb.
    int NumberOfNodesOnFullSkin = 0;

    int* TempIntArray = (int *) malloc( sizeof( int ) * MachineSize );
    if( TempIntArray == NULL )
      PLATFORM_ABORT( "TempIntArray == NULL" );    

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "BuildNodePairWeightTable: About to get the nodes on full skin: " 
      << EndLogLine;

    int P5_EXP = 0.0;

    mRecBisTree->RanksInNeighborhoodSkin( 0, 0, 0,
					  (*mRadiusOfSphere) + P5_EXP,
					  TempIntArray,      // This is only used here as a memory holder
					  mNumberOfNodesOnFullSkin, 
					  MachineSize );  

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "COMM_INFO mNumberOfNodesOnFullSkin: " << mNumberOfNodesOnFullSkin
      << EndLogLine;

    mFullSkinTable = (NodeId **) malloc( sizeof( NodeId * ) * MachineSize );
    if( mFullSkinTable == NULL )
      PLATFORM_ABORT( "mFullSkinTable == NULL" );
    
    for( int i=0; i < MachineSize; i++ )
      {
	mFullSkinTable[ i ] = (NodeId *) malloc( sizeof( NodeId ) * mNumberOfNodesOnFullSkin );
	if( mFullSkinTable[ i ] == NULL )
	  PLATFORM_ABORT( "FullSkinTable[ i ] == NULL" );    
      }

    for( int NodeI=0; NodeI < MachineSize; NodeI++ )
      {    
	int xI, yI, zI;
	Platform::Topology::GetCoordsFromRank( NodeI, &xI, &yI, &zI );
	ORBNode* HomeZoneI = mRecBisTree->NodeFromProcCoord( xI, yI, zI );
    
	int NodeCount = 0;
	mRecBisTree->RanksInNeighborhoodSkin( xI, yI, zI,
					      (*mRadiusOfSphere) + P5_EXP,
					      TempIntArray,
					      NodeCount,
					      MachineSize );

	// Make sure all the skins are the same size
	assert( mNumberOfNodesOnFullSkin == NodeCount );

	for( int j = 0; j < NodeCount; j++ )
	  {      
	    mFullSkinTable[ NodeI ][ j ] = TempIntArray[ j ];
	  }
      }
  
    free( TempIntArray );
    TempIntArray = NULL;

    for( int NodeI=0; NodeI < MachineSize; NodeI++ )
      {    
	int xI, yI, zI;
	Platform::Topology::GetCoordsFromRank( NodeI, &xI, &yI, &zI );
	ORBNode* HomeZoneI = mRecBisTree->NodeFromProcCoord( xI, yI, zI );
            
	int FragBuffOffsetI = A2ARecvOffset[ NodeI ];
	int FragBuffCountI = A2ARecvCount[ NodeI ];

	BegLogLine( PKFXLOG_SETUP )
	  << "Top of outer loop: NodeI: " << NodeI 
	  << " FragBuffCountI: " << FragBuffCountI
	  << " FragBuffOffsetI: " << FragBuffOffsetI
	  << EndLogLine;

	if(!( FragBuffCountI >= 0 && FragBuffCountI < NUMBER_OF_FRAGMENTS ))
	  {
	    BegLogLine( 1 )
	      << "NodeI: " << NodeI
	      << " FragBuffCountI: " << FragBuffCountI
	      << " FragBuffOffsetI: " << FragBuffOffsetI
	      << EndLogLine;
	    PLATFORM_ABORT( "FragBuffCountI >= 0 && FragBuffCountI < NUMBER_OF_FRAGMENTS" );
	  }
      
	FragId* FragmentsOfRankI = &RecvFragmentBuffer[ FragBuffOffsetI ];

	for( int NodeJ=NodeI; NodeJ < MachineSize; NodeJ++ )
	  {
	    double deltas[ 3 ];
	    int xJ, yJ, zJ;
	    Platform::Topology::GetCoordsFromRank( NodeJ, &xJ, &yJ, &zJ );
	    ORBNode* HomeZoneJ = mRecBisTree->NodeFromProcCoord( xJ, yJ, zJ );
	    HomeZoneI->NodeMinExternalDeltas( HomeZoneJ, deltas );
        
	    double DistanceSquared = deltas[0]*deltas[0] + deltas[1]*deltas[1] + deltas[2]*deltas[2];
        
	    // If node min > 2R, then there's no need to consider the pair
	    if( DistanceSquared <= 4 * RadiusSquared )
	      {          
		// Intersect the skins and get the intersection.
		IntersectSortedList< NodeId >( mFullSkinTable[ NodeI ], mNumberOfNodesOnFullSkin,
					       mFullSkinTable[ NodeJ ], mNumberOfNodesOnFullSkin,
					       IntersectionList, IntersectionListCount );

		if( IntersectionListCount == 0 )
		  continue;
        
		// Figure out the weight
		int FragBuffOffsetJ = A2ARecvOffset[ NodeJ ];
		int FragBuffCountJ  = A2ARecvCount [ NodeJ ];

		if(!( FragBuffCountI >= 0 && FragBuffCountI < NUMBER_OF_FRAGMENTS ))
		  {
		    PLATFORM_ABORT( "FragBuffCountI >= 0 && FragBuffCountI < NUMBER_OF_FRAGMENTS" );
		  }

		if(!( FragBuffCountJ >= 0 && FragBuffCountJ < NUMBER_OF_FRAGMENTS ))
		  {
		    PLATFORM_ABORT( "FragBuffCountJ >= 0 && FragBuffCountJ < NUMBER_OF_FRAGMENTS" );
		  }

		assert( FragBuffCountJ >= 0 && FragBuffCountJ < NUMBER_OF_FRAGMENTS );
          
		FragId* FragmentsOfRankJ = &RecvFragmentBuffer[ FragBuffOffsetJ ];
          
		LBWeightType NodeIJWeight = 0.0;

		for( int fragOrdI=0; fragOrdI < FragBuffCountI; fragOrdI++ )
		  {
		    FragId fragIdI = FragmentsOfRankI[ fragOrdI ];
            
		    if( fragIdI == (FragId) NON_EXISTANCE_VAL )
		      continue;
            
		    for( int fragOrdJ = ( NodeI == NodeJ ) ? fragOrdI+1: 0; 
			 fragOrdJ < FragBuffCountJ; 
			 fragOrdJ++ )
		      {
			FragId fragIdJ = FragmentsOfRankJ[ fragOrdJ ];
              
			if( fragIdJ == (FragId) NON_EXISTANCE_VAL )
			  continue;

			if( !( fragIdI >= 0 && fragIdI < NUMBER_OF_FRAGMENTS ) ||
			    !( fragIdJ >= 0 && fragIdJ < NUMBER_OF_FRAGMENTS ) )
			  {
			    BegLogLine( 1 )
			      << "fragIdI: " << fragIdI
			      << " fragIdJ: " << fragIdJ
			      << " fragOrdI: " << fragOrdI
			      << " fragOrdJ: " << fragOrdJ
			      << " FragBuffCountI: " << FragBuffCountI
			      << " FragBuffCountJ: " << FragBuffCountJ
			      << " FragBuffOffsetI: " << FragBuffOffsetI
			      << " FragBuffOffsetJ: " << FragBuffOffsetJ
			      << " IntersectionListCount: " << IntersectionListCount 
			      << " NodeI: " << NodeI
			      << " NodeJ: " << NodeJ
			      << EndLogLine;

			    PLATFORM_ABORT( "ERROR: Fragment out of range" );
			  }
                            
			double ExtentA = mExtents[ fragIdI ];
			double ExtentB = mExtents[ fragIdJ ];
			XYZ & FragCenterA = mFragCenters[ fragIdI ];
			XYZ & FragCenterB = mFragCenters[ fragIdJ ];
              
			XYZ FragCenterBImg;
			NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
              
			double thA= (GuardRange+ExtentA+ExtentB) ;
			double dsqThreshold = FragCenterA.DistanceSquared(FragCenterBImg) - (thA*thA) ;
                            
			if( dsqThreshold < 0.0 )
			  {
			    // This pair counts
			    // LBWeightType ModWeight = (LBWeightType) InteractionModel::GetWeight( fragIdI, fragIdJ, this );
			    LBWeightType ModWeight = 1.0;
			    LBWeightType WeightForFragPair = ModWeight;
			    if( WeightForFragPair < 0.0 )
			      {
				BegLogLine( 1 )
				  << "FragIdI: " << fragIdI
				  << " FragIdJ: " << fragIdJ
				  << " WeightForFragPair: " << WeightForFragPair
				  << " ModWeight: " << ModWeight
				  << EndLogLine;
                  
				PLATFORM_ABORT( "WeightForFragPair < 0.0" );
			      }                  
			    NodeIJWeight += WeightForFragPair;
			  }
		      }
		  }
        
		{
		  TotalWeightCount++;
            
		  if( NodeIJWeight > MaxWeight )
		    MaxWeight = NodeIJWeight;

		  if( NodeIJWeight < MinWeight )
		    MinWeight = NodeIJWeight;

		  AvgWeight += NodeIJWeight;
            
		  BegLogLine( 0 )
		    << "Pair: { " << NodeI << " , " << NodeJ << " } is accounted for" 
		    << EndLogLine;
                        
		  // First assignment goes here.
		  int Seed = 0;
		  if( NodeI < NodeJ )
		    Seed = NodeI * MachineSize + NodeJ;
		  else
		    Seed = NodeJ * MachineSize + NodeI;
            
		  srand( Seed );
		  int RandomNodeIndex = (int) floor( IntersectionListCount * (rand()/(RAND_MAX + 1.0)) );
            
		  BegLogLine( 0 )
		    << "SP IntersectionListCount= " << IntersectionListCount
		    << EndLogLine;
            
		  if( !( RandomNodeIndex >= 0 && RandomNodeIndex < IntersectionListCount ) )
		    {
		      BegLogLine( 1 )
			<< "RandomNodeIndex: " << RandomNodeIndex
			<< " IntersectionListCount: " << IntersectionListCount
			<< " Seed: " << Seed
			<< " NodeI: " << NodeI
			<< " NodeJ: " << NodeJ
			<< " NodeIJWeight: " << NodeIJWeight
			<< " RadiusSquared: " << RadiusSquared
			<< EndLogLine;
              
		      PLATFORM_ABORT( "!( RandomNodeIndex >= 0 && RandomNodeIndex < IntersectionListCount )" );
		    }
              
		  int AssignedNode = IntersectionList[ RandomNodeIndex ];
		  if( !( AssignedNode >= 0 && AssignedNode < MachineSize ) )
		    PLATFORM_ABORT( "( AssignedNode >= 0 && AssignedNode < MachineSize )" );
            
		  if(( mCurrentBufferIndex + sizeof( AssignedWeightLink )) >= BUFFER_CHUNK_SIZE ) 
		    {
		      GetNextMemoryBuffer();
		    }

		  AssignedWeightLink* AssignedWeightPtr = (AssignedWeightLink *) & mCurrentBuffer[ mCurrentBufferIndex ];

		  AssignedWeightPtr->mNodePairRecord.mNodePair.mNodeIdA = NodeI;
		  AssignedWeightPtr->mNodePairRecord.mNodePair.mNodeIdB = NodeJ;
		  AssignedWeightPtr->mNodePairRecord.mWeight = NodeIJWeight;            

#ifndef PK_PHASE5_SKIP_OPT_SORT
		  AssignedWeightPtr->mNodePairRecord.mOptCount = IntersectionListCount;
#endif
		  mCurrentBufferIndex += sizeof( AssignedWeightLink );

		  AssignedWeightPtr->mNextLink          = NULL;
            
		  mFullSkinNodeAssignmentTable[ AssignedNode ].mTotalWeight += NodeIJWeight;
            
		  // Add the new weight to the assigned weight link list for this node
		  if( mFullSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsFirst == NULL )
		    {
		      mFullSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsFirst = AssignedWeightPtr;
		      mFullSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast  = AssignedWeightPtr;
		    }
		  else
		    {
		      mFullSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast->mNextLink = AssignedWeightPtr;
		      mFullSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast = AssignedWeightPtr; 
		    }
            
		  mNodePairRecordsCount ++;
		}
	      }
	  }
      }

    BegLogLine( PKFXLOG_SPARSE_SKIN )
      << "Finished the FullSkinNodesAssignemtTableSetup"
      << EndLogLine;

#ifndef NDEBUG
    for( int i = 0; i < MachineSize; i++ )
      {    
	AssignedWeightLink* AssignedWeightList = mFullSkinNodeAssignmentTable[ i ].mAssignedWeightsFirst;
	float TotalWeight = 0.0;
	while( AssignedWeightList != NULL )
	  {
	    TotalWeight += AssignedWeightList->mNodePairRecord.mWeight;
	    AssignedWeightList = AssignedWeightList->mNextLink;
	  }
    
	if( fabs( TotalWeight - mFullSkinNodeAssignmentTable[ i ].mTotalWeight )  > 10E-10 )
	  {
	    BegLogLine( PKFXLOG_SPARSE_SKIN )
	      << "i: " << i
	      << " TotalWeight: " << TotalWeight
	      << " mFullSkinNodeAssignmentTable[ i ].mTotalWeight: " 
	      << mFullSkinNodeAssignmentTable[ i ].mTotalWeight
	      << EndLogLine;

	    PLATFORM_ABORT( "ERROR: Inconsistent Weights" );
	  }
      }
#endif

    AvgWeight /= TotalWeightCount;

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "MinWeight= " << MinWeight
      << " MaxWeight= " << MaxWeight
      << " TotalWeights= " << TotalWeightCount
      << " AvgWeight= " << AvgWeight
      << EndLogLine;
    
    free( A2ASendCount );
    free( A2ASendOffset );
    free( IntersectionList );

    BegLogLine( PKFXLOG_SPARSE_SKIN )
      << "Before the barrier"
      << EndLogLine;

    Platform::Control::Barrier();

    BegLogLine( PKFXLOG_SPARSE_SKIN )
      << "After the barrier"
      << EndLogLine;
  }


  void AddNodeInSortedOrder( NodeId  aNewNode,
			     NodeId  aSparseSkinNodeId,
			     NodeId* aSparseSkin, 
			     int&    aSparseSkinCount )
  {
    assert( aNewNode >= 0 && aNewNode < Platform::Topology::GetSize() );
    assert( aSparseSkinCount < mNumberOfNodesOnFullSkin );

    // This is not most optimal. Mostly to test initial function.
    // Might be left in, if it's good enough for startup
    int found = 0;
    for( int i = 0; i < aSparseSkinCount; i++ )
      {
	if( aSparseSkin[ i ] == aNewNode )
	  {
	    found = 1;
	    break;
	  }
      }

    if( found )
      {
        BegLogLine( PKFXLOG_SPARSE_SKIN )
	  << "aNewNode: " << aNewNode
	  << " already exists in sparse skin list. " 
	  << " aSparseSkinCount: " << aSparseSkinCount
	  << EndLogLine;
    
	// Don't need to add duplicates
	return;
      }

    aSparseSkin[ aSparseSkinCount ] = aNewNode;
    aSparseSkinCount++;
    Platform::Algorithm::Qsort( aSparseSkin, aSparseSkinCount, sizeof( NodeId ), CompareNodeId );
  }

  void PrintList( NodeId* aList, 
		  int     aListCount,
		  char*   aListName )
  {
    for( int i = 0; i < aListCount; i++ )
      {
	BegLogLine( PKFXLOG_SPARSE_SKIN )
	  << aListName << "[ " << i << " ]: "
	  << aList[ i ]
	  << EndLogLine;
      }
  }

  // This method returns the "best" index which represents
  // a node id to be added to the sparse skin.
  int PickBestNodeForAdditionToSparseSkin( NodeId* aNodeList, 
					   int     aNodeListCount )
  {
    // Loop down looking for the nodes with smallest score
    double MinScore = 999999999.0;
    
    for( int i = 0; i < aNodeListCount; i++ )
      {
	NodeId CurNodeId = aNodeList[ i ];
	assert( CurNodeId >= 0 && CurNodeId < Platform::Topology::GetSize() );

	if( mNodesScoreList[ CurNodeId ] < MinScore )
	  {
	    MinScore = mNodesScoreList[ CurNodeId ];
	    mNodeIndexWithSameScoreListCount = 0;
	    mNodeIndexWithSameScoreList[ mNodeIndexWithSameScoreListCount ] = i;
	    mNodeIndexWithSameScoreListCount++;
	  }
	else if( mNodesScoreList[ CurNodeId ] == MinScore )
	  {
	    mNodeIndexWithSameScoreList[ mNodeIndexWithSameScoreListCount ] = i;
	    mNodeIndexWithSameScoreListCount++;      
	  }
      }

    if( mNodeIndexWithSameScoreListCount == 0 )
      PLATFORM_ABORT( "ERROR: mNodeIndexWithSameScoreListCount == 0" );
  
    // If we get here, then every node on the node list was already choosen.
    // Just choose one at random.
    int RandomIndex = (int) floor( mNodeIndexWithSameScoreListCount * ( rand() / ( RAND_MAX + 1.0 ) ) );
    assert( RandomIndex >= 0 && RandomIndex < mNodeIndexWithSameScoreListCount );

    int Index = mNodeIndexWithSameScoreList[ RandomIndex ];
    return Index;
  }

  void ExpandSparseList( double aWeightAB,
			 NodeId aNodeA, NodeId aNodeB,
			 NodeId* aOptNodes, int aOptNodesCount,
			 NodeId* aSparseSkinA, int& aSparseSkinACount,
			 NodeId* aSparseSkinB, int& aSparseSkinBCount )
  {
 
    int OptCounter = 0;
    int SparseSkinACounter = 0;
    int SparseSkinBCounter = 0;

    mMatchesForSparseListACount = 0;
    mMatchesForSparseListBCount = 0;

    while(    ( OptCounter < aOptNodesCount )
	      && ( SparseSkinACounter < aSparseSkinACount )
	      && ( SparseSkinBCounter < aSparseSkinBCount ) )
      {
    
	assert( OptCounter >= 0 && OptCounter < aOptNodesCount );
	assert( SparseSkinACounter >= 0 && SparseSkinACounter < aSparseSkinACount );
	assert( SparseSkinBCounter >= 0 && SparseSkinBCounter < aSparseSkinBCount );

	NodeId OptNode = aOptNodes[ OptCounter ];
	assert( OptNode >= 0 && OptNode < Platform::Topology::GetSize() );

	NodeId SparseSkinNodeA = aSparseSkinA[ SparseSkinACounter ];
	assert( SparseSkinNodeA >= 0 && SparseSkinNodeA < Platform::Topology::GetSize() );

	NodeId SparseSkinNodeB = aSparseSkinB[ SparseSkinBCounter ];
	assert( SparseSkinNodeB >= 0 && SparseSkinNodeB < Platform::Topology::GetSize() );

	if( OptNode == SparseSkinNodeA )
	  {
	    mMatchesForSparseListA[ mMatchesForSparseListACount ] = OptNode;
	    mMatchesForSparseListACount++;
	  }

	if( OptNode == SparseSkinNodeB )
	  {
	    mMatchesForSparseListB[ mMatchesForSparseListBCount ] = OptNode;
	    mMatchesForSparseListBCount++;
	  }
    
	if( ( OptNode == SparseSkinNodeA ) && ( OptNode == SparseSkinNodeB ) )
	  {
	    // There's a clear match. No need to expand anything. Done!
	    return;
	  }
	else if(( OptNode < SparseSkinNodeA ) && ( OptNode < SparseSkinNodeB ) )
	  {
	    // Both sparse list nodes are larger then opt node
	    // Get a larger opt node
	    OptCounter++;
	  }
	else if( ( OptNode > SparseSkinNodeA ) && ( OptNode == SparseSkinNodeB ))
	  {
	    // We have a lock on sparse list B, try to get a larger sparse node A
	    SparseSkinACounter++;
	  }
	else if( ( OptNode == SparseSkinNodeA ) && ( OptNode > SparseSkinNodeB ))
	  {
	    // We have a lock on sparse list A, try to get a larger sparse node B
	    SparseSkinBCounter++;
	  }
	else if( ( OptNode > SparseSkinNodeA ) && ( OptNode > SparseSkinNodeB ))
	  {
	    // Both sparse lists are smaller, get a larger value for both lists      
	    SparseSkinACounter++;
	    SparseSkinBCounter++;
	  }
	else if( ( OptNode < SparseSkinNodeA ) && ( OptNode >= SparseSkinNodeB ))
	  {
	    // We need to get a larger option to ever match A, 
	    // No chance for B if we don't try to get a larger value
	    OptCounter++;
	    SparseSkinBCounter++;      
	  }
	else if( ( OptNode >= SparseSkinNodeA ) && ( OptNode < SparseSkinNodeB ))
	  {
	    // We need to get a larger option to ever match B, 
	    // No chance for A if we don't try to get a larger value
	    OptCounter++;
	    SparseSkinACounter++;
	  }
	else
	  {
	    PLATFORM_ABORT( "ERROR:: Reached an unreachable case");
	  }
      }
  
    // No match found. Doh... Need to choose a node so that it satisfies the intersection
    // This is where reading of the tarrot cards would help... 

    // It could be, due to the different sizes of lists that we never got a 
    // chance to find the match of an option node in either A or B sparse
    // skin lists. In this case search A first to find a match, if that
    // comes up with nothing, then search B.
    if( ( mMatchesForSparseListACount == 0 ) && ( mMatchesForSparseListBCount == 0 ) )
      {
	int indO = 0;
	int indA = 0;
	while( ( indA < aSparseSkinACount ) && 
	       ( indO < aOptNodesCount ) )
	  {
	    if( aSparseSkinA[ indA ] == aOptNodes[ indO ] )
	      {
		mMatchesForSparseListA[ mMatchesForSparseListACount ] = aOptNodes[ indO ];
		mMatchesForSparseListACount++;
		indA++;
		indO++;
	      }
	    else if( aSparseSkinA[ indA ] < aOptNodes[ indO ] )
	      indA++;
	    else
	      indO++;
	  }

	if( mMatchesForSparseListACount == 0 )
	  {
	    indO = 0;
	    int indB = 0;
	    while( ( indB < aSparseSkinBCount ) && 
		   ( indO < aOptNodesCount ) )
	      {
		if( aSparseSkinB[ indB ] == aOptNodes[ indO ] )
		  {
		    mMatchesForSparseListB[ mMatchesForSparseListBCount ] = aOptNodes[ indO ];
		    mMatchesForSparseListBCount++;
		    indB++;
		    indO++;
		  }
		else if( aSparseSkinB[ indB ] < aOptNodes[ indO ] )
		  indB++;
		else
		  indO++;
	      }
	  }
      }
 
    // In the end, our goal is to have as least number of nodes as possible. We think
    // that adding only to the list that doesn't have the node is a good idea.
    // If we get stuck with both nodes having absolutely no match with the opts, that's tough. 
    // Pick some node out of the opts and add to both. 
    srand( aOptNodes[ 0 ] );
    if( ( mMatchesForSparseListACount == 0 ) && ( mMatchesForSparseListBCount == 0 ) )
      {
	// Opts matched neither, add first to both
	// Add a random node from aOptNodes.
	int BestIndex = PickBestNodeForAdditionToSparseSkin( aOptNodes, aOptNodesCount );
    
	NodeId BestNode = aOptNodes[ BestIndex ];
    
	AddNodeInSortedOrder( BestNode, aNodeA, aSparseSkinA, aSparseSkinACount );
	AddNodeInSortedOrder( BestNode, aNodeB, aSparseSkinB, aSparseSkinBCount );

	// Add the weight to the score.
	// mNodesScoreList[ BestNode ] += aWeightAB;
	mNodesScoreList[ BestNode ] += 1.0;
      }
    else if( mMatchesForSparseListACount == 0 )
      {
	// Add B's match to A
	int BestIndex = PickBestNodeForAdditionToSparseSkin( mMatchesForSparseListB, mMatchesForSparseListBCount );
    
	NodeId BestNode = mMatchesForSparseListB[ BestIndex ];

	AddNodeInSortedOrder( BestNode, aNodeA, aSparseSkinA, aSparseSkinACount );

	// Add the weight to the score.
	mNodesScoreList[ BestNode ] += 1.0;
	// mNodesScoreList[ BestNode ] += aWeightAB;
      }
    else if( mMatchesForSparseListBCount == 0 )
      {
	// Add A's match to B
	int BestIndex = PickBestNodeForAdditionToSparseSkin( mMatchesForSparseListA, mMatchesForSparseListACount );
    
	NodeId BestNode = mMatchesForSparseListA[ BestIndex ];

	AddNodeInSortedOrder( BestNode, aNodeB, aSparseSkinB, aSparseSkinBCount );
	// Add the weight to the score.
	mNodesScoreList[ BestNode ] += 1.0;        
      }
    else
      {
	// There was a match for both, but not the same match. Need to pick one.
    
	// If A has less entries give a random B entry to A, else give to B
	if( aSparseSkinACount < aSparseSkinBCount )
	  {
	    int BestIndex = PickBestNodeForAdditionToSparseSkin( mMatchesForSparseListB, mMatchesForSparseListBCount );
      
	    NodeId BestNode = mMatchesForSparseListB[ BestIndex ];

	    AddNodeInSortedOrder( BestNode, aNodeA, aSparseSkinA, aSparseSkinACount );
	    // Add the weight to the score.
	    mNodesScoreList[ BestNode ] += 1.0;
	  }
	else
	  {
	    int BestIndex = PickBestNodeForAdditionToSparseSkin( mMatchesForSparseListA, mMatchesForSparseListACount );
          
	    NodeId BestNode = mMatchesForSparseListA[ BestIndex ];

	    AddNodeInSortedOrder( BestNode, aNodeB, aSparseSkinB, aSparseSkinBCount );
	    // Add the weight to the score.
	    mNodesScoreList[ BestNode ] += 1.0;
	  }
      }
  }

  void GetSparseSkinInNeighborhood()
  {
    int MachineSize = Platform::Topology::GetAddressSpaceCount();
  
    int IntersectionListCount = 0;
    NodeId* IntersectionList = (NodeId *) malloc( sizeof( NodeId ) * MachineSize );
    if( IntersectionList == NULL )
      PLATFORM_ABORT( "IntersectionList == NULL" );

#ifndef PK_PHASE5_SKIP_OPT_SORT
    int SparseSkinOptListCount = 0;

    BegLogLine( PKFXLOG_SETUP )
      << "sizeof(SparseSkinHelper) * mNodePairRecordsCount: " << sizeof(SparseSkinHelper) * mNodePairRecordsCount
      << EndLogLine;

    SparseSkinHelper* SparseSkinOptList = (SparseSkinHelper *) malloc ( sizeof(SparseSkinHelper) * mNodePairRecordsCount );
    if( SparseSkinOptList == NULL )
      {
	BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
	  << "sizeof(SparseSkinHelper) * mNodePairRecordsCount: " << sizeof(SparseSkinHelper) * mNodePairRecordsCount
	  << " mNodePairRecordsCount: " << mNodePairRecordsCount
	  << " sizeof(SparseSkinHelper): " << sizeof(SparseSkinHelper)
	  << EndLogLine;
	PLATFORM_ABORT( "SparseSkinOptList == NULL" );
      }

    BegLogLine( PKFXLOG_SETUP )
      << "Got memory! sweet! "
      << EndLogLine;

    for( int i = 0; i < MachineSize; i++ )
      {
	AssignedWeightLink* AssignedWeightLinkPtr = mFullSkinNodeAssignmentTable[ i ].mAssignedWeightsFirst;
    
	while( AssignedWeightLinkPtr != NULL )
	  {
	    NodePairRecord* NodePairRecPtr = & AssignedWeightLinkPtr->mNodePairRecord;

	    int NodeA  = NodePairRecPtr->mNodePair.mNodeIdA;
	    int NodeB  = NodePairRecPtr->mNodePair.mNodeIdB;
      
	    IntersectSortedList< NodeId >( mFullSkinTable[ NodeA ], mNumberOfNodesOnFullSkin,
					   mFullSkinTable[ NodeB ], mNumberOfNodesOnFullSkin,
					   IntersectionList, IntersectionListCount );
      
	    assert( SparseSkinOptListCount >= 0 && SparseSkinOptListCount < mNodePairRecordsCount );

	    SparseSkinOptList[ SparseSkinOptListCount ].mNodePairRecPtr = NodePairRecPtr;
	    SparseSkinOptListCount++;

	    AssignedWeightLinkPtr = AssignedWeightLinkPtr->mNextLink;
	  }
      }

    assert( SparseSkinOptListCount == mNodePairRecordsCount );
  
    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "GetSparseSkinInNeighborhood: About to qsort "
      << EndLogLine;

    Platform::Algorithm::Qsort( SparseSkinOptList,
				SparseSkinOptListCount,
				sizeof( SparseSkinHelper ),
				CompareSparseSkinOptList );

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "GetSparseSkinInNeighborhood: Done with the qsort "
      << EndLogLine;
#endif
  
    mSparseSkins = (NodeId **) malloc( sizeof( NodeId *) * MachineSize );
    if( mSparseSkins == NULL )
      PLATFORM_ABORT( "mSparseSkins == NULL" );

    mSparseSkinsCounts = (int *) malloc( sizeof( int ) * MachineSize );
    if( mSparseSkinsCounts == NULL )
      PLATFORM_ABORT( "mSparseSkinsCounts == NULL" );

    mMatchesForSparseListA = (NodeId *) malloc( sizeof( NodeId ) * MachineSize );
    if( mMatchesForSparseListA == NULL )
      PLATFORM_ABORT( "mMatchesForSparseListA == NULL" );

    mMatchesForSparseListB = (NodeId *) malloc( sizeof( NodeId ) * MachineSize );
    if( mMatchesForSparseListB == NULL )
      PLATFORM_ABORT( "mMatchesForSparseListB == NULL" );
    
    mNodesScoreList = (double *) malloc( sizeof( double ) * MachineSize );
    if( mNodesScoreList == NULL )
      PLATFORM_ABORT( "mNodesScoreList == NULL" );  

    mNodeIndexWithSameScoreList = (int *) malloc( sizeof( int ) * MachineSize );
    if( mNodeIndexWithSameScoreList == NULL )
      PLATFORM_ABORT( "mNodeIndexWithSameScoreList == NULL" );  

    mMatchesForSparseListACount = 0;
    mMatchesForSparseListBCount = 0;
  
    for(int i = 0; i < MachineSize; i++ )
      {
	mNodesScoreList[ i ] = 0.0;
	mSparseSkinsCounts[ i ] = 0;

	int HalfTheNumberOfNodes = 0.6 * mNumberOfNodesOnFullSkin;
	mSparseSkins[ i ] = (NodeId *) malloc( sizeof( NodeId ) * HalfTheNumberOfNodes );
	if( mSparseSkins[ i ] == NULL )
	  PLATFORM_ABORT( "mSparseSkins[ i ] == NULL" );
      } 

    double RadiusSquared = (*mRadiusOfSphere) * (*mRadiusOfSphere);

#ifndef PK_PHASE5_SKIP_OPT_SORT
    for( int i = 0; i < SparseSkinOptListCount; i++ )
      {
	// For each set of options decide which option makes it on both lists.
	// Search the sparse sets for both nodes to see if he option is already there    

	NodePairRecord* NodePairRecPtr = SparseSkinOptList[ i ].mNodePairRecPtr;
	int NodeA = NodePairRecPtr->mNodePair.mNodeIdA;
	int NodeB = NodePairRecPtr->mNodePair.mNodeIdB;       
#else
	for( int NodeA=0; NodeA < MachineSize; NodeA++ )
	  {   
	    int xI, yI, zI;
	    Platform::Topology::GetCoordsFromRank( NodeA, &xI, &yI, &zI );
	    ORBNode* HomeZoneI = mRecBisTree->NodeFromProcCoord( xI, yI, zI );

	    for( int NodeB=NodeA; NodeB < MachineSize; NodeB++ )
	      {
		double deltas[ 3 ];
		int xJ, yJ, zJ;
		Platform::Topology::GetCoordsFromRank( NodeB, &xJ, &yJ, &zJ );
		ORBNode* HomeZoneJ = mRecBisTree->NodeFromProcCoord( xJ, yJ, zJ );
		HomeZoneI->NodeMinExternalDeltas( HomeZoneJ, deltas );
      
		double DistanceSquared = deltas[0]*deltas[0] + deltas[1]*deltas[1] + deltas[2]*deltas[2];
      
		// If node min > 2R, then there's no need to consider the pair
		if( DistanceSquared <= 4 * RadiusSquared )
		  {
#endif

		    IntersectSortedList< NodeId >( mFullSkinTable[ NodeA ], mNumberOfNodesOnFullSkin,
						   mFullSkinTable[ NodeB ], mNumberOfNodesOnFullSkin,
						   IntersectionList, IntersectionListCount );

		    if( IntersectionListCount == 0 )
		      continue;

		    LBWeightType WeightAB  = 1;

		    assert( NodeA >= 0 && NodeA < MachineSize );        
		    assert( NodeB >= 0 && NodeB < MachineSize );

    
		    NodeId* SparseSkinA      = mSparseSkins[ NodeA ];    
		    NodeId* SparseSkinB      = mSparseSkins[ NodeB ];

		    ExpandSparseList( WeightAB, NodeA, NodeB,
				      IntersectionList, IntersectionListCount,
				      SparseSkinA, mSparseSkinsCounts[ NodeA ],
				      SparseSkinB, mSparseSkinsCounts[ NodeB ] );

#if 0
		    for( int j = 0; j < (mSparseSkinsCounts[ NodeA ]-1); j++ )
		      {
			if( SparseSkinA[ j ] > SparseSkinA[ j+1 ] )
			  {
			    BegLogLine( PKFXLOG_SPARSE_SKIN )
			      << "NodeA: " << NodeA
			      << " NodeB: " << NodeB
			      << " mSparseSkinsCounts[ NodeA ]: " << mSparseSkinsCounts[ NodeA ]
			      << " i: " << i
			      << " j: " << j
			      << " SparseSkinA[ j ]: " << SparseSkinA[ j ]
			      << " SparseSkinA[ j+1 ]: " << SparseSkinA[ j+1 ]
			      << " IntersectionListCount: " << IntersectionListCount
			      << EndLogLine;

			    PLATFORM_ABORT( "ERROR: SkinA list is not sorted." );
			  }
		      }

		    for( int j = 0; j < (mSparseSkinsCounts[ NodeB ]-1); j++ )
		      {
			if( SparseSkinB[ j ] > SparseSkinB[ j+1 ] )
			  {
			    BegLogLine( PKFXLOG_SPARSE_SKIN )
			      << "NodeA: " << NodeA
			      << " NodeB: " << NodeB

			      << " mSparseSkinsCounts[ NodeB ]: " << mSparseSkinsCounts[ NodeB ]
			      << " i: " << i
			      << " j: " << j
			      << " SparseSkinB[ j ]: " << SparseSkinB[ j ]
			      << " SparseSkinB[ j+1 ]: " << SparseSkinB[ j+1 ]
			      << " IntersectionListCount: " << IntersectionListCount
			      << EndLogLine;

			    PLATFORM_ABORT( "ERROR: Options list is not sorted." );
			  }
		      }
#endif

#ifndef PK_PHASE5_SKIP_OPT_SORT
		  }
#else
	      }
	  }
      }
#endif

    // Clear the initial assignmed
    mSparseSkinNodeAssignmentTable = (NodeAssignmentRecord_T *) malloc( sizeof( NodeAssignmentRecord_T ) * MachineSize );
    if( mSparseSkinNodeAssignmentTable == NULL )
      PLATFORM_ABORT( "mSparseSkinNodeAssignmentTable == NULL" );

    for( int i = 0; i < MachineSize; i++ )
      {
	mSparseSkinNodeAssignmentTable[ i ].mNodeId               = i;
	mSparseSkinNodeAssignmentTable[ i ].mTotalWeight          = 0.0;
	mSparseSkinNodeAssignmentTable[ i ].mAssignedWeightsFirst = NULL;
	mSparseSkinNodeAssignmentTable[ i ].mAssignedWeightsLast  = NULL;
	mSparseSkinNodeAssignmentTable[ i ].mNodeWeightLink       = NULL;
      }  

    // Go over the FullSkinNodeAssignmentTable and take out the links
    // to assign to the SparseSkinNodeAssignmentTable.
    for( int i = 0; i < MachineSize; i++ )
      {
	AssignedWeightLink* CurLink = mFullSkinNodeAssignmentTable[ i ].mAssignedWeightsFirst;
	// Loop over all the links, assigning to the SparseSkinNodeAssignmentTable.
	while( CurLink != NULL )
	  {
	    AssignedWeightLink* NextLink = CurLink->mNextLink; 

	    LBWeightType PairWeight = CurLink->mNodePairRecord.mWeight;
      
	    int NodeA = CurLink->mNodePairRecord.mNodePair.mNodeIdA;
	    int NodeB = CurLink->mNodePairRecord.mNodePair.mNodeIdB;
      
	    NodeId* SparseSkinA      = mSparseSkins[ NodeA ];
	    int     SparseSkinCountA = mSparseSkinsCounts[ NodeA ];
      
	    NodeId* SparseSkinB      = mSparseSkins[ NodeB ];
	    int     SparseSkinCountB = mSparseSkinsCounts[ NodeB ];
      
	    assert( NodeA >= 0 && NodeA < MachineSize );
	    assert( NodeB >= 0 && NodeB < MachineSize );
      
	    IntersectSortedList< NodeId >( SparseSkinA, SparseSkinCountA,
					   SparseSkinB, SparseSkinCountB,
					   IntersectionList, IntersectionListCount );
      
	    if( IntersectionListCount <= 0 )
	      PLATFORM_ABORT( "IntersectionListCount <= 0" );
      
	    if( IntersectionListCount > mNumberOfNodesOnFullSkin )
	      PLATFORM_ABORT( "IntersectionListCount > mNumberOfNodesOnFullSkin" );      
      
	    int Seed = 0;
	    if( NodeA < NodeB )
	      Seed = NodeA * MachineSize + NodeB;
	    else
	      Seed = NodeB * MachineSize + NodeA;
      
	    srand( Seed );
	    int RandomNodeIndex = (int) floor( IntersectionListCount * (rand()/(RAND_MAX + 1.0)) );
      
	    int AssignedNode = IntersectionList[ RandomNodeIndex ];
	    if( !( AssignedNode >= 0 && AssignedNode < MachineSize ) )
	      PLATFORM_ABORT( "( AssignedNode >= 0 && AssignedNode < MachineSize )" );
            
	    mSparseSkinNodeAssignmentTable[ AssignedNode ].mTotalWeight += PairWeight;
      
	    if( mSparseSkinNodeAssignmentTable[ AssignedNode ].mTotalWeight < 0.0 )
	      {
		BegLogLine( PKFXLOG_SPARSE_SKIN )
		  << "i: " << i
		  << " PairWeight: " << PairWeight
		  << " AssignedNode: " << AssignedNode
		  << " mNodeAssignmentTable[ AssignedNode ].mTotalWeight: " 
		  << mSparseSkinNodeAssignmentTable[ AssignedNode ].mTotalWeight
		  << EndLogLine;
		PLATFORM_ABORT( "mSparseSkinNodeAssignmentTable[ AssignedNode ].mTotalWeight < 0.0" );
	      }

	    // Add to the end of the list
	    CurLink->mNextLink = NULL;

	    if( mSparseSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsFirst == NULL )
	      {
		mSparseSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsFirst = CurLink;
		mSparseSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast = mSparseSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsFirst;        
	      }    
	    else 
	      {
		mSparseSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast->mNextLink = CurLink;
		mSparseSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast = CurLink;
	      }
      
	    CurLink = NextLink;
	  }
      }

    free( mFullSkinNodeAssignmentTable );
    mFullSkinNodeAssignmentTable = NULL;

#ifndef NDEBUG
    // Clip all the unused links.
    for( int i = 0; i < MachineSize; i++ )
      {    
	AssignedWeightLink* AssignedWeightList = mSparseSkinNodeAssignmentTable[ i ].mAssignedWeightsFirst;
	float TotalWeight = 0.0;
	while( AssignedWeightList != NULL )
	  {
	    TotalWeight += AssignedWeightList->mNodePairRecord.mWeight;
	    AssignedWeightList = AssignedWeightList->mNextLink;
	  }
    
	if( fabs( TotalWeight - mSparseSkinNodeAssignmentTable[ i ].mTotalWeight )  > 10E-10 )
	  {
	    BegLogLine( PKFXLOG_SPARSE_SKIN )
	      << "i: " << i
	      << " TotalWeight: " << TotalWeight
	      << " mSparseSkinNodeAssignmentTable[ i ].mTotalWeight: " 
	      << mSparseSkinNodeAssignmentTable[ i ].mTotalWeight
	      << EndLogLine;

	    PLATFORM_ABORT( "ERROR: Inconsistent Weights" );
	  }
      }
#endif

    int MyRank = Platform::Topology::GetAddressSpaceId();  

    mSparseSkinSendNodesCount = mSparseSkinsCounts[ MyRank ];

    // CWO 8-4-06 - For Phase 5, SpareSkinSendNodes must hold the count given by mSpareSkinsCounts[MyRank]
    //              As Well As 'this' node -- so we must leave space in the malloc 
    mSparseSkinSendNodes      = (int *) malloc( sizeof( int ) * (mSparseSkinSendNodesCount + 1) );
    if( mSparseSkinSendNodes == NULL )
      PLATFORM_ABORT("mSparseSkinSendNodes == NULL");
  
    for( int i = 0; i < mSparseSkinSendNodesCount; i++ )
      {
	mSparseSkinSendNodes[ i ] = mSparseSkins[ MyRank ][ i ];

	BegLogLine( 0 )
	  << "mSparseSkinSendNodes[ i ]= " << mSparseSkinSendNodes[ i ]
	  << EndLogLine;
      }

    mSparseSkinSendNodes[ mSparseSkinSendNodesCount ] = Platform::Topology::GetAddressSpaceId();
    mSparseSkinSendNodesCount++;

    free( IntersectionList );
    for(int i = 0; i < MachineSize; i++ )
      {
	free( mFullSkinTable[ i ] );
      }

    free( mFullSkinTable );
    mFullSkinTable = NULL;

    free( mMatchesForSparseListA );
    mMatchesForSparseListA = NULL;

    free( mMatchesForSparseListB );
    mMatchesForSparseListB = NULL;

    free( mNodesScoreList );
    mNodesScoreList = NULL;

    free( mNodeIndexWithSameScoreList );
    mNodeIndexWithSameScoreList = NULL;

#ifndef PK_PHASE5_SKIP_OPT_SORT
    free( SparseSkinOptList );
    SparseSkinOptList = NULL;
#endif
  }

  void
  Init( double*                    aExtents, 
	XYZ*                       aFragCenters,
	IntegratorStateManagerIF*  aIntegratorStateManagerIF,
	ORBNode*                   aRecBisTree,
	double*                    aRadiusOfSphere,
	double*                    aGuardZone,
	unsigned int               aVoxDimX,
	unsigned int               aVoxDimY,
	unsigned int               aVoxDimZ )
  {
    mExtents                  = aExtents;
    mFragCenters              = aFragCenters;
    mIntegratorStateManagerIF = aIntegratorStateManagerIF;
    mRecBisTree               = aRecBisTree;
    mRadiusOfSphere           = aRadiusOfSphere;
    mGuardZone                = aGuardZone;

    mNodeIdToRecvNodeOrdinalMap = NULL;
    mAssignmentNodeTable        = NULL;


    // This chooses which nodes to exclude from the sparse skin
    mCheckeredTemplate[0][0][0] = 1;
    mCheckeredTemplate[0][0][1] = 1;
    mCheckeredTemplate[0][1][0] = 1;
    mCheckeredTemplate[0][1][1] = 1;
    mCheckeredTemplate[1][0][0] = 1;
    mCheckeredTemplate[1][0][1] = 1;
    mCheckeredTemplate[1][1][0] = 1;
    mCheckeredTemplate[1][1][1] = 1;
    
    mVoxDimX = aVoxDimX;
    mVoxDimY = aVoxDimY;
    mVoxDimZ = aVoxDimZ;
    
    mMaxVoxelsInRange = mVoxDimX * mVoxDimY * mVoxDimZ;
    
    mVoxelIndexToVoxelOrdinalMap = (int *) malloc( sizeof(int) * mMaxVoxelsInRange );
    if( mVoxelIndexToVoxelOrdinalMap == NULL )
      PLATFORM_ABORT( "mVoxelIndexToVoxelOrdinalMap == NULL" );
  }

  int* 
  GetSparseSkinNodes()
  {
    return mSparseSkinSendNodes;
  }

  int 
  GetSparseSkinNodesCount()
  {
    return mSparseSkinSendNodesCount;
  }

  // This call creates the sparse skins and a load balanced
  // Assignment of fragment(node-node) operations to nodes
  void 
  CreateSparseSkinAndAssignement()
  {
#pragma execution_frequency(very_low)
    // Setup the table of weights and rings 
    // Creates an initial pass at the NodeWeightTable
    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "CreateLoadBalancedNodeAssignmentTable: About to BuildNodePairWeightTable() "
      << EndLogLine;

    BuildNodePairWeightTable();

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "CreateLoadBalancedNodeAssignmentTable: About to GetSparseSkinInNeighborhood(); "
      << EndLogLine;
  
    // The result of this call is in
    // mSparseSkinNodes, mSparseSkinNodesCount
    GetSparseSkinInNeighborhood();
  
    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "CreateLoadBalancedNodeAssignmentTable: Done with GetSparseSkinInNeighborhood(); "
      << EndLogLine;

    // Let's see if greedy annealing will work
    // Sort the NodeAssignmentTable
    int MachineSize = Platform::Topology::GetAddressSpaceCount();
    int MyRank = Platform::Topology::GetAddressSpaceId();
  
    WeightSortHelper* WeightsToSort = (WeightSortHelper *) malloc( MachineSize * sizeof( WeightSortHelper ) );
    if( WeightsToSort == NULL )
      PLATFORM_ABORT( "WeightsToSort == NULL" );
    
    for( int i=0; i<MachineSize; i++ )
      {
	WeightsToSort[ i ].mWeight            =   mSparseSkinNodeAssignmentTable[ i ].mTotalWeight;

	if( WeightsToSort[ i ].mWeight < 0.0 )
	  PLATFORM_ABORT( "ERROR: WeightsToSort[ i ].mWeight < 0.0" );
       

	WeightsToSort[ i ].mNodeAssignmentRec = & mSparseSkinNodeAssignmentTable[ i ];

	BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
	  << "Before: NodeId: " << i 
	  << " Weight: " << WeightsToSort[ i ].mWeight
	  << " mAssignedWeightsFirst: " << (void *) WeightsToSort[ i ].mNodeAssignmentRec->mAssignedWeightsFirst
	  << " mAssignedWeightsLast: " <<  (void *) WeightsToSort[ i ].mNodeAssignmentRec->mAssignedWeightsLast
	  << " mNodeWeightLink: " <<  (void *) WeightsToSort[ i ].mNodeAssignmentRec->mNodeWeightLink
	  << EndLogLine;
      }

    // This exploits the proposition that non-negative 'floats' sort the same way as the same bit map 'unsigned int'    
    RadixSortDescending<WeightSortHelper,unsigned int,1,0>( WeightsToSort, MachineSize );
    
    for( int i=0; i<MachineSize; i++ )
      {
	BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
	  << "After Weight: " << WeightsToSort[ i ].mWeight
	  << EndLogLine;
      }

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "Before WeightDiff: " << WeightsToSort[ 0 ].mWeight - WeightsToSort[ MachineSize-1 ].mWeight
      << EndLogLine;

    NodeWeightList* WeightOrderedNodeListFirst = NULL;
    NodeWeightList* WeightOrderedNodeListLast  = NULL;

    // Build the linked list 
    for( int i=0; i<MachineSize; i++ )
      {
	NodeWeightList* Link = (NodeWeightList*) malloc( sizeof( NodeWeightList ) );
	if( Link == NULL )
	  PLATFORM_ABORT( "Link == NULL" );

	Link->mNodeAssignmentTablePtr = WeightsToSort[ i ].mNodeAssignmentRec;
	Link->mNext = NULL;

	Link->mNodeAssignmentTablePtr->mNodeWeightLink = Link;

	if( WeightOrderedNodeListFirst == NULL )
	  {
	    WeightOrderedNodeListFirst = Link;
	    WeightOrderedNodeListLast = Link;
	    WeightOrderedNodeListFirst->mPrev = NULL;
	  }
	else
	  {
	    Link->mPrev = WeightOrderedNodeListLast;
	    WeightOrderedNodeListLast->mNext = Link;
	    WeightOrderedNodeListLast = Link;
	  }
      }

    int TopCoupleOfNodes = 3;
  
    // Start iterating over the WeightOrderedNodeListFirst finding a pair of nodes
    // that needs weight offloading
    int TOTAL_TO_TRY = 15000;

    int PrevBestRank = -1;

    int  HISTORY_COUNT = 4;
    int* LoadGivingHistory = ( int *) malloc( MachineSize * HISTORY_COUNT * sizeof( int ));
    if( LoadGivingHistory == NULL )
      PLATFORM_ABORT( "LoadGivingHistory == NULL" );

    for(int i = 0; i < MachineSize; i++ )
      {
	for(int j = 0; j < HISTORY_COUNT; j++ )
	  {
	    LoadGivingHistory[ i*HISTORY_COUNT + j ] = -1;
	  }
      }

    double MinWeightDiff = 999999999.0;
    
    FinalAssignment* MyMinAssignment = (FinalAssignment *)malloc( sizeof( FinalAssignment ) ); 
    if( MyMinAssignment == NULL )
      PLATFORM_ABORT( "MyAssignment == NULL" );

    MyMinAssignment->mTotalWeight = 0.0;
    MyMinAssignment->mAssignedPairsCount = 0;
    MyMinAssignment->mAssignedPairsCountAllocated = MachineSize;
    MyMinAssignment->mAssignedPairs = (NodePair *) malloc( sizeof( NodePair ) * MyMinAssignment->mAssignedPairsCountAllocated );
    if( MyMinAssignment->mAssignedPairs == NULL )
      PLATFORM_ABORT( "MyMinAssignment->mAssignedPairs == NULL" );

    int IntersectionListCount = 0;
    NodeId* IntersectionList = (NodeId *) malloc( sizeof( NodeId ) * mNumberOfNodesOnFullSkin );
    if( IntersectionList == NULL )
      PLATFORM_ABORT( "IntersectionList == NULL" );

    for( int iter = 0; iter < TOTAL_TO_TRY; iter++ )
      {
	// Randomly choose top couple of nodes
	NodeWeightList* NodeToExamine = WeightOrderedNodeListFirst;
      
	NodeAssignmentRecord_T* NodeRecordPtr = NodeToExamine->mNodeAssignmentTablePtr;
      
	// Pick up the top entry in the list and analyze it    
	LBWeightType               TotalWeight  = NodeRecordPtr->mTotalWeight;

	if( TotalWeight == 0)
	  {
	    BegLogLine( PKFXLOG_SPARSE_SKIN )
	      << "TotalWeight == 0"
	      << EndLogLine;
	  }

	if( TotalWeight < 0.0 )
	  PLATFORM_ABORT( "ERROR: TotalWeight < 0.0" );

	NodeId              ExamRank     = NodeRecordPtr->mNodeId;
	double              BestMerit    = -1.0;
	int                 BestRank     = -1;
	double              BestWeight   = -1.0;
	AssignedWeightLink* BestPairLink = NULL;
      
	BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
	  << iter
	  << " Starting out with Node:  " << NodeRecordPtr->mNodeId
	  << " with Weight: " << NodeRecordPtr->mTotalWeight
	  << EndLogLine;

	AssignedWeightLink* AssignedWeightList = NodeRecordPtr->mAssignedWeightsFirst;
      
	if( AssignedWeightList == NULL )
	  PLATFORM_ABORT( "AssignedWeightList == NULL" );

	while( AssignedWeightList != NULL )
	  {
	    NodePairRecord* NodePairRecPtr = & AssignedWeightList->mNodePairRecord;

	    // There could be weights that are zero due to a lack of 
	    // interacting atoms from a pair of nodes
	    if( NodePairRecPtr->mWeight <= 0.0 )
	      {
		AssignedWeightList = AssignedWeightList->mNextLink;
		continue;
	      }

	    assert( NodePairRecPtr != NULL );
        
	    // This is where we generate the intersection of the sparse skins
	    int NodeIdA = NodePairRecPtr->mNodePair.mNodeIdA;
	    int NodeIdB = NodePairRecPtr->mNodePair.mNodeIdB;

	    IntersectSortedList< NodeId >( mSparseSkins[ NodeIdA ], mSparseSkinsCounts[ NodeIdA ],
					   mSparseSkins[ NodeIdB ], mSparseSkinsCounts[ NodeIdB ],
					   IntersectionList, IntersectionListCount );

	    if( IntersectionListCount == 0 )
	      PLATFORM_ABORT( "IntersectionListCount == 0" );

	    BegLogLine( 0 )
	      << iter
	      << " NodeId:  " << NodeRecordPtr->mNodeId
	      << " for weight: " << NodePairRecPtr->mWeight
	      << " A: " << NodePairRecPtr->mNodePair.mNodeIdA
	      << " B: " << NodePairRecPtr->mNodePair.mNodeIdB
	      << " IntersectionListCount: " << IntersectionListCount
	      << EndLogLine;

	    for( int opt = 0; opt < IntersectionListCount; opt++ )
	      {
		int OptRank = IntersectionList[ opt ];

		BegLogLine( 0 )
		  << iter
		  << " OptRank: " << OptRank
		  << " ExamRank: " << ExamRank
		  << EndLogLine;

		// The current node is not an option
		if( ExamRank == OptRank ) 
		  continue;

		if( !( OptRank >= 0 && OptRank < MachineSize ) )
		  PLATFORM_ABORT( "( OptRank >= 0 && OptRank < MachineSize )");

		double NodeWeightDiff = TotalWeight - mSparseSkinNodeAssignmentTable[ OptRank ].mTotalWeight;          
        
		double Merit = 0.0;
		if( iter < 7000 )
		  {
		    Merit = NodePairRecPtr->mWeight + NodeWeightDiff;

		    BegLogLine( 0 )
		      << iter
		      << " Merit: " << Merit
		      << " NodePairRecPtr->mWeight: " << NodePairRecPtr->mWeight
		      << " NodeWeightDiff: " << NodeWeightDiff
		      << " TotalWeight: " << TotalWeight
		      << " mSparseSkinNodeAssignmentTable[ OptRank ].mTotalWeight: " << mSparseSkinNodeAssignmentTable[ OptRank ].mTotalWeight
		      << " OptRank: " << OptRank
		      << EndLogLine;
		  }
		else
		  {           
		    Merit = (1.0 / NodePairRecPtr->mWeight) + NodeWeightDiff;
		  }          

		if( Merit > BestMerit )
		  {            
		    // If the potentially best node was in out recent history 
		    // lets not choose that node again. This is done to handle
		    // winding up in a thrashing situation
		    if( iter >= 1000 )
		      {
			int NodeFound = 0;
			for( int j = 0; j < HISTORY_COUNT; j++ )
			  {
			    int index = ExamRank * HISTORY_COUNT + j;
			    if( LoadGivingHistory[ index ] == OptRank )
			      {
				NodeFound = 1;
			      }
			  }
              
			if( NodeFound )
			  continue;
		      }
            
		    BestMerit    = Merit;
		    BestRank     = OptRank;
		    BestWeight   = NodePairRecPtr->mWeight;
		    BestPairLink = AssignedWeightList;
		  }
	      }

	    AssignedWeightList = AssignedWeightList->mNextLink;
	  }

	for( int j=0; j<HISTORY_COUNT-1; j++ )
	  {
	    int index = ExamRank * HISTORY_COUNT + j;
	    LoadGivingHistory[ index + 1 ] = LoadGivingHistory[ index ];
	  }
      
	LoadGivingHistory[ ExamRank * HISTORY_COUNT ] = BestRank;

	BegLogLine( 0 )
	  << iter
	  << " BestMerit: " << BestMerit
	  << " BestRank: " << BestRank
	  << " BestWeight: " << BestWeight
	  << " BestPairLink: " << BestPairLink
	  << EndLogLine;
    
	// BestRank should be the rank that we off load work to.
      
	// Adjust the overall weights, adjusting weight lists
	NodeRecordPtr->mTotalWeight -= BestWeight;
      
	if( BestPairLink == NULL )
	  {
	    // reached a wall
	    PLATFORM_ABORT( "ERROR: BestPairLink == NULL" );
	  }

	// Remove BestPairLink from the examining node's assigned weight list
	if( BestPairLink == NodeRecordPtr->mAssignedWeightsFirst )
	  {
	    // This is the first link
	    NodeRecordPtr->mAssignedWeightsFirst = NodeRecordPtr->mAssignedWeightsFirst->mNextLink;
	  }
	else if( BestPairLink == NodeRecordPtr->mAssignedWeightsLast )
	  {
	    // Search to find the last to last link
	    AssignedWeightLink* CurLink = NodeRecordPtr->mAssignedWeightsFirst;        
	    while( CurLink->mNextLink !=  NodeRecordPtr->mAssignedWeightsLast )
	      CurLink = CurLink->mNextLink;

	    NodeRecordPtr->mAssignedWeightsLast = CurLink;
	    NodeRecordPtr->mAssignedWeightsLast->mNextLink = NULL;
	  }
	else
	  {
	    // We're taking out a link in the middle. Need to find the prev link.
	    // This assumes that BestPairLink is in the chain. That's a must.
	    AssignedWeightLink* CurLink = NodeRecordPtr->mAssignedWeightsFirst;        
	    while( CurLink->mNextLink !=  BestPairLink )
	      CurLink = CurLink->mNextLink;        
        
	    CurLink->mNextLink = BestPairLink->mNextLink;

	  }
    
	// Insert BestPairLink at the end the BestRank weight assigned list
	if( !( BestRank >= 0 && BestRank < MachineSize ) )
	  PLATFORM_ABORT( "( BestRank >= 0 && BestRank < MachineSize )");

	BestPairLink->mNextLink = NULL;
      
	// Check for the case where a node has zero work assigned
	if( ( mSparseSkinNodeAssignmentTable[ BestRank ].mAssignedWeightsLast == NULL )
	    || ( mSparseSkinNodeAssignmentTable[ BestRank ].mAssignedWeightsFirst == NULL ) )
	  {
	    mSparseSkinNodeAssignmentTable[ BestRank ].mAssignedWeightsFirst = BestPairLink;
	    mSparseSkinNodeAssignmentTable[ BestRank ].mAssignedWeightsLast  = BestPairLink;
	  }
	else
	  {
	    mSparseSkinNodeAssignmentTable[ BestRank ].mAssignedWeightsLast->mNextLink = BestPairLink;
	    mSparseSkinNodeAssignmentTable[ BestRank ].mAssignedWeightsLast = BestPairLink;
	  }

	assert( BestRank >= 0 && BestRank < MachineSize );
	mSparseSkinNodeAssignmentTable[ BestRank ].mTotalWeight += BestWeight;    
    
	// Weight distribution changed. Readjust the NodeWeight link order

	// Move the "heavy" node DOWN the sorted doubly linked list
	double HeavyNodeWeight = NodeRecordPtr->mTotalWeight;    

	NodeWeightList* NodeToMove = NodeToExamine;
	NodeWeightList* CurNode    = NodeToMove->mNext;

	int cnt = 0;

	while( CurNode != NULL )
	  {
	    if( HeavyNodeWeight > CurNode->mNodeAssignmentTablePtr->mTotalWeight )
	      {
		// We found a place where the heave node has to go.
		if( NodeToMove->mNext == CurNode )
		  {
		    // The heavy node stays where it is.
		    break;
		  }
		else
		  {           
		    // Remove the heavy node
		    if( NodeToMove->mPrev != NULL )
		      NodeToMove->mPrev->mNext = NodeToMove->mNext;
		    else
		      {
			// NodeToMove was the first
			WeightOrderedNodeListFirst = NodeToMove->mNext;
		      }
            
		    if( NodeToMove->mNext != NULL )
		      NodeToMove->mNext->mPrev = NodeToMove->mPrev;
            
		    // Insert into it's new place.
		    // Attach the new link in FRONT of CurNode
		    NodeToMove->mPrev = CurNode->mPrev;
		    NodeToMove->mNext = CurNode;
            
		    if( CurNode->mPrev != NULL )
		      CurNode->mPrev->mNext = NodeToMove;

		    CurNode->mPrev = NodeToMove;
          
		    break;
		  }
	      }
      
	    cnt++;
	    CurNode = CurNode->mNext;
	    if( CurNode == NULL )
	      {
		// If the search turned up nothing, then NodeToMove must be the last node
		// in the double link list

		// Unhook NodeToMove
		if( NodeToMove->mPrev != NULL )
		  NodeToMove->mPrev->mNext = NodeToMove->mNext;
		else
		  WeightOrderedNodeListFirst = NodeToMove->mNext;
          
		if( NodeToMove->mNext != NULL )
		  NodeToMove->mNext->mPrev = NodeToMove->mPrev;

		// Hook it up in a new place
		NodeToMove->mNext = NULL;
		NodeToMove->mPrev = WeightOrderedNodeListLast;

		WeightOrderedNodeListLast->mNext = NodeToMove;
		WeightOrderedNodeListLast = NodeToMove;

		break;
	      }
	  }
      
	if( WeightOrderedNodeListFirst->mNext == NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListFirst->mNext == NULL" );

	if( WeightOrderedNodeListFirst->mPrev != NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListFirst->mPrev != NULL" );

	if( WeightOrderedNodeListLast->mPrev == NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListLast->mPrev == NULL" );

	if( WeightOrderedNodeListLast->mNext != NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListLast->mNext != NULL" );

	// Move the "light" node UP the sorted doubly linked list
	double LightNodeWeight = mSparseSkinNodeAssignmentTable[ BestRank ].mTotalWeight;

	NodeToMove = mSparseSkinNodeAssignmentTable[ BestRank ].mNodeWeightLink;
	CurNode = NodeToMove->mPrev;

	while( CurNode != NULL )
	  {
	    if( LightNodeWeight < CurNode->mNodeAssignmentTablePtr->mTotalWeight )
	      {
		// We found a place where the light node has to go.
		if( NodeToMove->mPrev == CurNode )
		  {
		    // The light node stays where it is.
		    break;
		  }
		else
		  {
		    // Remove the light node
		    if( NodeToMove->mPrev != NULL )
		      NodeToMove->mPrev->mNext = NodeToMove->mNext;
          
		    if( NodeToMove->mNext != NULL )
		      NodeToMove->mNext->mPrev = NodeToMove->mPrev;
		    else
		      {  
			// NodeToMove was the last
			WeightOrderedNodeListLast = NodeToMove->mPrev;
		      }

		    // Insert into it's new place.
		    // Attach the new link BEHIND of CurNode
		    NodeToMove->mPrev = CurNode;
		    NodeToMove->mNext = CurNode->mNext;
          
		    if( CurNode->mNext != NULL )
		      CurNode->mNext->mPrev = NodeToMove;

		    CurNode->mNext = NodeToMove;
          
		    break;
		  }
	      }

	    CurNode = CurNode->mPrev;
	    if( CurNode == NULL )
	      {
		// If the search turned up nothing, then NodeToMove must be the first node
		// in the double link list
          
		// Unhook NodeToMove
		if( NodeToMove->mPrev != NULL )
		  NodeToMove->mPrev->mNext = NodeToMove->mNext;
          
		if( NodeToMove->mNext != NULL )
		  NodeToMove->mNext->mPrev = NodeToMove->mPrev;
		else
		  WeightOrderedNodeListLast = NodeToMove->mPrev;            

		// Hook it up as the first
		NodeToMove->mPrev = NULL;
		NodeToMove->mNext = WeightOrderedNodeListFirst;
		WeightOrderedNodeListFirst->mPrev = NodeToMove;
		WeightOrderedNodeListFirst        = NodeToMove;
		break;
	      }
	  }

	if( WeightOrderedNodeListFirst->mNext == NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListFirst->mNext == NULL" );

	if( WeightOrderedNodeListFirst->mPrev != NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListFirst->mPrev != NULL" );

	if( WeightOrderedNodeListLast->mPrev == NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListLast->mPrev == NULL" );

	if( WeightOrderedNodeListLast->mNext != NULL )
	  PLATFORM_ABORT( "WeightOrderedNodeListLast->mNext != NULL" );
            
	double WeightDiffList   = 
          WeightOrderedNodeListFirst->mNodeAssignmentTablePtr->mTotalWeight - 
          WeightOrderedNodeListLast->mNodeAssignmentTablePtr->mTotalWeight;
      
	if( WeightDiffList < MinWeightDiff )
	  {
	    MinWeightDiff = WeightDiffList;
         
	    // Store away the assignment for this node.         
	    NodeAssignmentRecord_T* CurAssignment = & mSparseSkinNodeAssignmentTable[ MyRank ];
	    AssignedWeightLink* CurLink = CurAssignment->mAssignedWeightsFirst;
         
	    MyMinAssignment->mAssignedPairsCount = 0;
	    MyMinAssignment->mTotalWeight = CurAssignment->mTotalWeight;

	    BegLogLine( 0 )
	      << iter
	      << " MinWeightDiff: " << MinWeightDiff
	      << " TotalWeightOnThisRank: " << MyMinAssignment->mTotalWeight
	      << EndLogLine;

	    double TotalWeightInList = 0.0;
         
	    while( CurLink != NULL )
	      {
		NodePairRecord* AssignedWeight = & CurLink->mNodePairRecord;

		if( MyMinAssignment->mAssignedPairsCount >= MyMinAssignment->mAssignedPairsCountAllocated )
		  ExpandFinalAssignedPairList( MyMinAssignment );
           
		MyMinAssignment->mAssignedPairs[ MyMinAssignment->mAssignedPairsCount ] = AssignedWeight->mNodePair;
		MyMinAssignment->mAssignedPairsCount++;
           
		TotalWeightInList += AssignedWeight->mWeight;

		CurLink = CurLink->mNextLink;
	      }
	  }

	BegLogLine( 0 )
	  << iter
	  << " WeightDiffList: " << WeightDiffList
	  << EndLogLine;      
      }

    BegLogLine( PKFXLOG_SPARSE_SKIN )
      << "MyMinAssignment->mTotalWeight: " << MyMinAssignment->mTotalWeight
      << " MyMinAssignment->mAssignedPairsCount: " << MyMinAssignment->mAssignedPairsCount
      << EndLogLine;    
    
    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "Final WeightDiff: " << MinWeightDiff
      << EndLogLine;

    Platform::Control::Barrier();

    double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
    double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
    
    double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;
    
    double InRangeCutoff = SwitchUpperCutoff  ;
    
    double GuardZone = *mGuardZone;
    
    double GuardRange = InRangeCutoff + GuardZone;
    double GuardRangeSqr = GuardRange * GuardRange;

    // Now we're ready to create the assignment table
    mNodeIdToRecvNodeOrdinalMap  = (int *) malloc( sizeof( int ) * MachineSize );
    if( mNodeIdToRecvNodeOrdinalMap == NULL )
      PLATFORM_ABORT( "mNodeIdToRecvNodeOrdinalMap == NULL" );
    
    for( int i=0; i<MachineSize; i++ )
      {
	mNodeIdToRecvNodeOrdinalMap[ i ] = -1;
      }
    
    int RecvNodeCount = 0;
  
    int* NodesToRecvFrom = (int *) malloc( sizeof( int ) * MachineSize );
    if( NodesToRecvFrom == NULL )
      PLATFORM_ABORT( "NodesToRecvFrom == NULL" );

    // Figure out the number of recv nodes to allocate the assignment table
    for( int i = 0; i < MyMinAssignment->mAssignedPairsCount; i++ )
      {     
	int NodeA = MyMinAssignment->mAssignedPairs[ i ].mNodeIdA;
	int NodeB = MyMinAssignment->mAssignedPairs[ i ].mNodeIdB;
    
	if(!( NodeA >= 0 && NodeA < MachineSize ) )
	  PLATFORM_ABORT( "NodeA >= 0 && NodeA < MachineSize" );

	if(!( NodeB >= 0 && NodeB < MachineSize ) )
	  PLATFORM_ABORT( "NodeB >= 0 && NodeB < MachineSize" );

	if( mNodeIdToRecvNodeOrdinalMap[ NodeA ] == -1 )
	  {
	    mNodeIdToRecvNodeOrdinalMap[ NodeA ] = RecvNodeCount;
	    NodesToRecvFrom[ RecvNodeCount ] = NodeA;
	    RecvNodeCount++;
	  }

	if( mNodeIdToRecvNodeOrdinalMap[ NodeB ] == -1 )
	  {
	    mNodeIdToRecvNodeOrdinalMap[ NodeB ] = RecvNodeCount;
	    NodesToRecvFrom[ RecvNodeCount ] = NodeB;
	    RecvNodeCount++;
	  }
      }

    mAssignmentNodeTable = (unsigned char **) malloc( sizeof(unsigned char *) * RecvNodeCount );
    if( mAssignmentNodeTable == NULL )
      PLATFORM_ABORT( "mAssignmentNodeTable == NULL" );
  
    for( int i=0; i<RecvNodeCount; i++ )
      {
	mAssignmentNodeTable[ i ] = (unsigned char *) malloc( sizeof( unsigned char ) * RecvNodeCount );
    
	if( mAssignmentNodeTable[ i ] == NULL )
	  PLATFORM_ABORT( "mAssignmentNodeTable[ i ] == NULL" );

	for( int j = 0; j < RecvNodeCount; j++ )
	  mAssignmentNodeTable[ i ][ j ] = 0;
      }

    int NumberOfVoxels = mVoxDimX * mVoxDimY * mVoxDimZ;

    int VoxelsCountI = 0;
    int* VoxelsI = (int *) malloc( NumberOfVoxels * sizeof( int ) );
    if( VoxelsI == NULL )
      PLATFORM_ABORT( "VoxelsI == NULL" );

    int VoxelsCountJ = 0;
    int* VoxelsJ = (int *) malloc( NumberOfVoxels * sizeof( int ) );
    if( VoxelsJ == NULL )
      PLATFORM_ABORT( "VoxelsJ == NULL" );

    double PossibleRange        = 2 * (*mRadiusOfSphere); ///SwitchLowerCutoff + SwitchDelta + mCurrentAssignmentGuardZone;
    double PossibleRangeSquared = PossibleRange * PossibleRange;

    /*******************************************************
     * Build the voxel interaction table
     ******************************************************/

    ORBNode * leafJ = mRecBisTree->NodeFromProcCoord( 0, 0, 0 );
    assert( leafJ );
    double LowJ[3];
    double HighJ[3];
    leafJ->Low( LowJ );
    leafJ->High( HighJ );
    leafJ->VoxelsInExternalBox( LowJ, HighJ, VoxelsJ, VoxelsCountJ, NumberOfVoxels );
  
    // (Number of nodes in skin) x ( Number of Voxels per node)
    mVoxelsInSkinHoodCount = mNumberOfNodesOnFullSkin * VoxelsCountJ; 
      
    BegLogLine( PKFXLOG_SETUP )
      << " mNumberOfNodesOnFullSkin= " << mNumberOfNodesOnFullSkin
      << " VoxelsCountJ= " << VoxelsCountJ
      << "VoxelsInSkinHoodCount= " << mVoxelsInSkinHoodCount
      << EndLogLine;

    for( int j=0; j < NumberOfVoxels; j++)
      {
	mVoxelIndexToVoxelOrdinalMap[ j ] = -1;
      }

    BegLogLine( PKFXLOG_SETUP )
      << "GenVoxTable(): About to allocate the n^2 voxel table"
      << EndLogLine;
  
    // This trap will fail if mVoxelInteractionTypeTableSize wants to be larger than MAXINT
    // We will probably overflow memory at a lower value; this is a backstop.  
    if( mVoxelsInSkinHoodCount > 46340 )
      {
	PLATFORM_ABORT("mVoxelInteractionTypeTable overflows memory. Try a smaller voxmesh") ;
      }

    mVoxelInteractionTypeTableSize = mVoxelsInSkinHoodCount * mVoxelsInSkinHoodCount;

    mVoxelInteractionTypeTable.SetCount( mVoxelInteractionTypeTableSize );

    int OrdinalCount = 0;

    for( int i = 0; i < MyMinAssignment->mAssignedPairsCount; i++ )
      {     
	int NodeA = MyMinAssignment->mAssignedPairs[ i ].mNodeIdA;
	int NodeB = MyMinAssignment->mAssignedPairs[ i ].mNodeIdB;
    
	int NodeAOrdinal = mNodeIdToRecvNodeOrdinalMap[ NodeA ];
	int NodeBOrdinal = mNodeIdToRecvNodeOrdinalMap[ NodeB ];
    
	if( NodeAOrdinal < 0 || NodeAOrdinal >= RecvNodeCount )
	  PLATFORM_ABORT( "NodeAOrdinal < 0 || NodeAOrdinal >= RecvNodeCount" );

	if( NodeBOrdinal < 0 || NodeBOrdinal >= RecvNodeCount )
	  PLATFORM_ABORT( "NodeBOrdinal < 0 || NodeBOrdinal >= RecvNodeCount" );
    
	if( ( NodeA == 348 && NodeB == 485 ) || ( NodeB == 348 && NodeA == 485 ) )
	  BegLogLine( PKFXLOG_SPARSE_SKIN )
	    << "NodeA: " << NodeA
	    << " NodeB: " << NodeB
	    << " NodeAOrdinal: " << NodeAOrdinal
	    << " NodeBOrdinal: " << NodeBOrdinal
	    << EndLogLine;            
     
	mAssignmentNodeTable[ NodeAOrdinal ][ NodeBOrdinal ] = 1;
	mAssignmentNodeTable[ NodeBOrdinal ][ NodeAOrdinal ] = 1;

	// Here we build the voxel table.    
	int xI, yI, zI;
	Platform::Topology::GetCoordsFromRank( NodeA, &xI, &yI, &zI );
    
	ORBNode * leafI = mRecBisTree->NodeFromProcCoord( xI, yI, zI );
	assert( leafI );

	double LowI[3];
	double HighI[3];
	leafI->Low( LowI );
	leafI->High( HighI );
	leafI->VoxelsInExternalBox( LowI, HighI, VoxelsI, VoxelsCountI, NumberOfVoxels );

	int xJ, yJ, zJ;
	Platform::Topology::GetCoordsFromRank( NodeB, &xJ, &yJ, &zJ );
	ORBNode * leafJ = mRecBisTree->NodeFromProcCoord( xJ, yJ, zJ );
	assert( leafJ );
	double LowJ[3];
	double HighJ[3];
	leafJ->Low( LowJ );
	leafJ->High( HighJ );
	leafJ->VoxelsInExternalBox( LowJ, HighJ, VoxelsJ, VoxelsCountJ, NumberOfVoxels );

	for( int voxI = 0; voxI < VoxelsCountI; voxI++ )
	  {
	    int VoxelIdI = VoxelsI[ voxI ];
      
	    int VoxelIndexI = mRecBisTree->VoxelIdToVoxelIndex( VoxelIdI );
	    int VoxelOrdinalI = mVoxelIndexToVoxelOrdinalMap[ VoxelIndexI ];
      
	    if( VoxelOrdinalI == -1 )
	      {
		mVoxelIndexToVoxelOrdinalMap[ VoxelIndexI ] = OrdinalCount;
		VoxelOrdinalI = mVoxelIndexToVoxelOrdinalMap[ VoxelIndexI ];
		OrdinalCount++;
	      }

	    for( int voxJ = 0; voxJ < VoxelsCountJ; voxJ++ )
	      {
		int VoxelIdJ = VoxelsJ[ voxJ ];
		int VoxelIndexJ = mRecBisTree->VoxelIdToVoxelIndex( VoxelIdJ );
		int VoxelOrdinalJ = mVoxelIndexToVoxelOrdinalMap[ VoxelIndexJ ];

		if( VoxelOrdinalJ == -1 )
		  {
		    mVoxelIndexToVoxelOrdinalMap[ VoxelIndexJ ] = OrdinalCount;
		    VoxelOrdinalJ = mVoxelIndexToVoxelOrdinalMap[ VoxelIndexJ ];
		    OrdinalCount++;
		  }

		double MaxVoxelDistanceSqr = mRecBisTree->MaxVoxelSqrDistance( VoxelIdI, VoxelIdJ );
        
		unsigned char VoxelInteractionType = 0;

		if( MaxVoxelDistanceSqr < PossibleRangeSquared )
		  {
		    VoxelInteractionType |= 0x01;          
		  }
        
		int VoxelInteractionTypeTableIndexIJ = VoxelOrdinalI * mVoxelsInSkinHoodCount + VoxelOrdinalJ;
		int VoxelInteractionTypeTableIndexJI = VoxelOrdinalJ * mVoxelsInSkinHoodCount + VoxelOrdinalI;
		mVoxelInteractionTypeTable.SetElement( VoxelInteractionTypeTableIndexIJ, VoxelInteractionType );
		mVoxelInteractionTypeTable.SetElement( VoxelInteractionTypeTableIndexJI, VoxelInteractionType );
	      }
	  }
      }

    free( mSparseSkinNodeAssignmentTable );
    mSparseSkinNodeAssignmentTable = NULL;

    for( int i = 0; i < mMemoryBufferCount; i++ )
      {
	free( mMemoryBuffer[ i ] );
      }
    free( mMemoryBuffer );

    free( WeightsToSort );

    free( LoadGivingHistory );

    NodeWeightList* CurLink = WeightOrderedNodeListLast;
    while( CurLink != NULL )
      {
	NodeWeightList* PrevLink = CurLink->mPrev;
	free( CurLink );
	CurLink = PrevLink;
      }

    free( MyMinAssignment->mAssignedPairs );
    free( MyMinAssignment );
    free( IntersectionList );
  
    for( int i=0; i<MachineSize; i++ )
      {
	free( mSparseSkins[ i ] );
	mSparseSkins[i] = NULL;
      }

    free( mSparseSkins );
    mSparseSkins = NULL;

    free( mSparseSkinsCounts );
    mSparseSkinsCounts = NULL;
  }
};
#endif
