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
 #ifndef __NEIGHBORHOOD_COMM_DRIVER__
#define __NEIGHBORHOOD_COMM_DRIVER__

#ifndef PKFXLOG_NEIGHBORHOOD_INFO
#define PKFXLOG_NEIGHBORHOOD_INFO ( 0 )
#endif

class CommHelper
  {
public:
  inline static int GetNumberOfSitesForFragment( int aFragId )
    {
    return MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    }
  };

// static TraceClient gThrobberBcastStart;
// static TraceClient gThrobberBcastFinis;

// static TraceClient gThrobberReduceStart;
// static TraceClient gThrobberReduceFinis;

static TraceClient gBroadcastStart;
static TraceClient gBroadcastFinis;

static TraceClient gReduceStart;
static TraceClient gReduceFinis;

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
class NeighborhoodCommDriver
  {
  ORBNode*                    mTree;

  ThrobberIF                  mThrobberIF;    

  IntegratorStateManagerIF*   mIntegratorStateManagerIF;
  InteractionStateManagerIF*  mInteractionStateManagerIF;

  int*          mNodesToSendTo;
  int           mNodesToSendToCount;

  int*          mNodesInRadius;
  int           mNodesInRadiusCount;

  char*         mIsNodeInSphere;
  int           mVoxelMeshSize;


  int           mAssignmentRecookTS;
  int           mPostAssignment;    

  int                mFragsInRangeCount;
  FragId             mFragsInRange[ FRAGMENT_COUNT ];    

  int* mVoxelIndexToVoxelOrdinalMap;
  int  mVoxelsInRange;
  int  mTotalNumberOfVoxels;
  
  int* mSparseSkinNodes;    
  int  mSparseSkinNodesCount;    

public:
  void Init( InteractionStateManagerIF* aInteractionStateManagerIF,
             IntegratorStateManagerIF*  aIntegratorStateManagerIF,
             int                        aMaxSitesPerFragment,
             int                        aVoxelIdForFragment );

  void RecalculateNodesInRange( double aRadius,
                                int*   aRanksInP3MEMeshSpace,
                                int    aRanksInP3MEMeshSpaceSize );

   void ReInit( ORBNode* aTree,
                int*     aNodesToSendTo, 
                int      aNodesToSendToCount );

  void ReInit( double   aCutoff, 
               ORBNode* aTree,
               int*     aRanksInP3MEMeshSpace,
               int      aRanksInP3MEMeshSpaceSize,
               int*     aSparseSkinNodes = NULL, 
               int      aSparseSkinNodesCount = 0 );

  void Broadcast( int aSimTick, int aAssignmentViolation, int aBondedInteraction = 0 );
  void Reduce( int aSimTick );
      
  int* GetNodesInNeighborhoodRadius( );
  int  GetNodesInNeighborhoodRadiusCount( );
  
  void SetVoxelIndexToVoxelOrdinalMap( int* aVoxelIndexToVoxelOrdinalMap,
                                       int  aVoxelsInRange,
                                       int  aTotalNumberOfVoxels );      
};

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
int* NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::
GetNodesInNeighborhoodRadius( )
  {
  return mNodesInRadius;
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
int NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::
GetNodesInNeighborhoodRadiusCount( )
  {
  return mNodesInRadiusCount;
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
void NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::
RecalculateNodesInRange( double aRadius,
                         int*   aRanksInP3MEMeshSpace,
                         int    aRanksInP3MEMeshSpaceSize )
  {
  BegLogLine( 0 )
    << "Entering RecalculateNodesInRange()"
    << EndLogLine;
  
  int procs_x, procs_y, procs_z;
  
  Platform::Topology::GetDimensions( &procs_x, &procs_y, &procs_z );
  
  int myP_x, myP_y, myP_z;
  Platform::Topology::GetMyCoords( &myP_x, &myP_y, &myP_z );
  int MachineSize = procs_x * procs_y * procs_z;
  
  int NodesInRadiusCount = 0;
  int* NodesInRadius = (int *) malloc( sizeof( int ) * MachineSize );
  if( NodesInRadius == NULL )
    PLATFORM_ABORT( "NodesInRadius == NULL" );

  int* IsNodeInWholeSphere = (int *) malloc( sizeof( int ) * MachineSize );
  if( IsNodeInWholeSphere == NULL )
    PLATFORM_ABORT( "IsNodeInWholeSphere == NULL" );
  
  mTree->RanksInNeighborhood( myP_x, myP_y, myP_z,
                              aRadius,
                              NodesInRadius, 
                              NodesInRadiusCount, 
                              mVoxelMeshSize );
  

  mNodesInRadius      = mSparseSkinNodes;
  mNodesInRadiusCount = mSparseSkinNodesCount;

  BegLogLine( 0 )
      << "mNodesInRadiusCount: "
      << mNodesInRadiusCount 
      << EndLogLine;

  assert( mNodesInRadiusCount>=0 && mNodesInRadiusCount <= MachineSize );

  for( int i=0; i < MachineSize; i++ )
    {
    mIsNodeInSphere[ i ] = 0;
    IsNodeInWholeSphere[ i ] = 0;
    }

  for( int i=0; i < mNodesInRadiusCount; i++ )
    {
    int Rank = mNodesInRadius[ i ];
    mNodesToSendTo[ i ] = Rank ;
    mIsNodeInSphere[ Rank ] = 1;
    }
  mNodesToSendToCount = mNodesInRadiusCount;

  int NodesToSendToWholeSphereCount = 0;
  for( int i=0; i < NodesInRadiusCount; i++ )
    {
    int Rank = NodesInRadius[ i ];
    IsNodeInWholeSphere[ Rank ] = 1;
    }
  NodesToSendToWholeSphereCount = NodesInRadiusCount;

  // Add the nodes to send to
  for( int i=0; i < aRanksInP3MEMeshSpaceSize; i++ )
    {
    int Rank = aRanksInP3MEMeshSpace[ i ];

    // If the node is not already present in the sphere
    // add it to the send list
    if( !mIsNodeInSphere[ Rank ] )
      {
      mNodesToSendTo[ mNodesToSendToCount ] = Rank;
      mNodesToSendToCount++;
      }

    if( !IsNodeInWholeSphere[ Rank ] )
      {
      NodesToSendToWholeSphereCount++;
      }
    }

  BegLogLine( PKFXLOG_NEIGHBORHOOD_INFO )
    << " mNodesInSkinRadiusCount= " << mNodesInRadiusCount
    << " mNodesInRadiusCount= " << NodesInRadiusCount
    << " aRanksInP3MEMeshSpaceSize= " << aRanksInP3MEMeshSpaceSize
    << " mNodesToSendToCount= " << mNodesToSendToCount
    << " NodesToSendToWholeSphereCount= " << NodesToSendToWholeSphereCount
    << EndLogLine;
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
void NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::ReInit( ORBNode* aTree,
                                                                             int* aNodesToSendTo, 
                                                                             int  aNodesToSendToCount )
  {
  mTree = aTree;

  mNodesToSendTo      = aNodesToSendTo;
  mNodesToSendToCount = aNodesToSendToCount;
  
  int NumberOfLocalFrags = mIntegratorStateManagerIF->GetNumberOfLocalFragments();
  int MaxNumberOfLocalFrags = 0;

  Platform::Collective::GlobalMax( (unsigned long *) &NumberOfLocalFrags, (unsigned long *) &MaxNumberOfLocalFrags );

  int MachineDimX, MachineDimY, MachineDimZ;
  Platform::Topology::GetDimensions( &MachineDimX, &MachineDimY, &MachineDimZ );

  mThrobberIF.Init( MachineDimX, MachineDimY, MachineDimZ,
                    mNodesToSendTo,
                    mNodesToSendToCount,                  
                    FRAGMENT_COUNT,        // Total number of fragments in the system (used for ids)
                    MaxNumberOfLocalFrags,
                    FRAGMENT_COUNT / 3 );   // SHOULD CALC BASED ON VOLUME OF NEIGHBORHOOD: Max number of fragments to be received for neighborhood

  for( int f = 0; f < FRAGMENT_COUNT; f++ )
    {
    mThrobberIF.SetGlobalFragInfo( f, CommHelper::GetNumberOfSitesForFragment( f ) );
    }

  BegLogLine( PKFXLOG_NEIGHBORHOOD_INFO )
    << "NeighborhoodCommDriver::ReInit:: Done with state setup"
    << EndLogLine;  
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
void NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::ReInit( double aCutoff,
                                                                 ORBNode* aTree,
                                                                 int* aRanksInP3MEMeshSpace,
                                                                 int  aRanksInP3MEMeshSpaceSize,
                                                                 int* aSparseSkinNodes, 
                                                                 int  aSparseSkinNodesCount )
  {
  BegLogLine( 0 )
    << "NeighborhoodCommDriver::ReInit:: Entering state setup"
    << EndLogLine;

  mTree = aTree;

  mSparseSkinNodes      = aSparseSkinNodes;
  mSparseSkinNodesCount = aSparseSkinNodesCount;

  RecalculateNodesInRange( aCutoff,
                           aRanksInP3MEMeshSpace,
                           aRanksInP3MEMeshSpaceSize );

  int MachineDimX, MachineDimY, MachineDimZ;
  Platform::Topology::GetDimensions( &MachineDimX, &MachineDimY, &MachineDimZ );

  int NumberOfLocalFrags = mIntegratorStateManagerIF->GetNumberOfLocalFragments();
  int MaxNumberOfLocalFrags = 0;

  Platform::Collective::GlobalMax( (unsigned long *) &NumberOfLocalFrags, (unsigned long *) &MaxNumberOfLocalFrags );

  mThrobberIF.Init( MachineDimX, MachineDimY, MachineDimZ,
                    mNodesToSendTo,
                    mNodesToSendToCount,                  
                    FRAGMENT_COUNT,        // Total number of fragments in the system (used for ids)
                    MaxNumberOfLocalFrags,
                    FRAGMENT_COUNT / 3 );   // SHOULD CALC BASED ON VOLUME OF NEIGHBORHOOD: Max number of fragments to be received for neighborhood

  for( int f = 0; f < FRAGMENT_COUNT; f++ )
    {
    mThrobberIF.SetGlobalFragInfo( f, CommHelper::GetNumberOfSitesForFragment( f ) );
    }

  BegLogLine( PKFXLOG_NEIGHBORHOOD_INFO )
    << "NeighborhoodCommDriver::ReInit:: Done with state setup"
    << EndLogLine;
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
void NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::Init( InteractionStateManagerIF* aInteractionStateManagerIF,
                                                               IntegratorStateManagerIF*  aIntegratorStateManagerIF,
                                                               int  aMaxSitesPerFragment,
                                                               int  aVoxelMeshSize )
  {
  mIntegratorStateManagerIF  = aIntegratorStateManagerIF;
  mInteractionStateManagerIF = aInteractionStateManagerIF;

  // Check your 8 vertices with the other 8 vertices.
  int procs_x, procs_y, procs_z;
  int myP_x, myP_y, myP_z;

  Platform::Topology::GetDimensions( &procs_x, &procs_y, &procs_z );

  BegLogLine( 0 )
    << "procs_x=" << procs_x
    << " procs_y=" << procs_y
    << " procs_z=" << procs_z
    << EndLogLine;

  Platform::Topology::GetMyCoords( &myP_x, &myP_y, &myP_z );

  BegLogLine( 0 )
    << "myP_x: " << myP_x
    << " myP_y: " << myP_y
    << " myP_z: " << myP_z
    << EndLogLine;

  int my_rank = Platform::Topology::MakeRankFromCoords( myP_x, myP_y, myP_z );

  assert ( my_rank == Platform::Topology::GetAddressSpaceId() );

  int MachineSize = procs_x * procs_y * procs_z;

  mVoxelMeshSize = aVoxelMeshSize;

  mNodesInRadius = ( int * ) malloc( sizeof( int ) * mVoxelMeshSize );
  if( mNodesInRadius == NULL )
    PLATFORM_ABORT("mNodesInRadius == NULL");

  mNodesToSendTo = ( int * ) malloc( MachineSize * sizeof(int));
  if( mNodesToSendTo == NULL )
    PLATFORM_ABORT("mNodesToSendTo == NULL");

  mIsNodeInSphere = ( char * ) malloc( MachineSize * sizeof(char));
  if( mIsNodeInSphere == NULL )
    PLATFORM_ABORT("mIsNodeInSphere == NULL");

  mNodesInRadiusCount = 0;
  mNodesToSendToCount = 0;

  mPostAssignment = 0;
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
void NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::Broadcast( int aSimTick, int aAssignmentViolation,
                                                                                int aBondedInteraction )
  {
  char TraceBcastString[ 128 ];
  strcpy( TraceBcastString, CommDriverNames[ DriverDesc ] );
  strcat( TraceBcastString, "Broadcast");

  gBroadcastStart.HitOE( PKTRACE_NEIGHBORHOOD_COMM_DRIVER_BROADCAST,
                         TraceBcastString,
                         Platform::Topology::GetAddressSpaceId(),
                         gBroadcastStart );
  
  BegLogLine( 0 )
    << aSimTick
    << " Broadcast:: Entering..."
    << EndLogLine;
  
  mAssignmentRecookTS = aAssignmentViolation;

  mInteractionStateManagerIF->ResetState();

  if( aAssignmentViolation )
    {
    mThrobberIF.ClearLocalFrags();
    for( int i=0; i < mIntegratorStateManagerIF->GetNumberOfLocalFragments(); i++ )
      {
      FragId fragmentId = mIntegratorStateManagerIF->GetNthFragmentId( i );

      assert( fragmentId >= 0 && fragmentId < FRAGMENT_COUNT );
      mThrobberIF.SetFragAsLocal( fragmentId );
      }
    
    mThrobberIF.Pack();
    }

  for( int i=0; i < mIntegratorStateManagerIF->GetNumberOfLocalFragments(); i++ )
    {
    FragId fragmentId = mIntegratorStateManagerIF->GetNthFragmentId( i );
    XYZ* FragPosPtr = mThrobberIF.GetFragPositPtrLocal( fragmentId );

    XYZ* Pos0ptr = &(mIntegratorStateManagerIF->mIntegratorStateTable[ fragmentId ].mIntegratorFragmentState.mPosition[ 0 ]);

    assert( fragmentId >= 0 && fragmentId < FRAGMENT_COUNT );
    int SiteCount   = CommHelper::GetNumberOfSitesForFragment( fragmentId );

    memcpy_xyz( FragPosPtr, Pos0ptr, SiteCount );
    }

  BegLogLine( 0 )
      << "Broadcast:: Got to before ThrobberBcast"
      << EndLogLine;

  Platform::Control::Barrier();

//   gThrobberBcastStart.HitOE( PKTRACE_THROBBER_BCAST,
//                               "ThrobberBcastStart",
//                               Platform::Topology::GetAddressSpaceId(),
//                               gThrobberBcastStart );

  mThrobberIF.BroadcastExecute( aSimTick, aAssignmentViolation );

//   gThrobberBcastFinis.HitOE( PKTRACE_THROBBER_BCAST,
//                              "ThrobberBcastFinis",
//                              Platform::Topology::GetAddressSpaceId(),
//                              gThrobberBcastFinis );

  BegLogLine( 0 )
      << "Broadcast:: Got to after ThrobberBcast"
      << EndLogLine;

  // Zero all the forces in the throbber
  int FragsInHoodCount = mThrobberIF.GetFragsInHoodCount();

  for(int f = 0; f < FragsInHoodCount; f++ )
    {
    int fragId = mThrobberIF.GetFragIdInHood( f );
    
    if( fragId == -1 )
      continue;
    
    int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );    
    XYZ * FragForces = mThrobberIF.GetFragForcePtr( fragId );      
    
    for( int i=0; i<SiteCount; i++ )
      {
      FragForces[ i ].Zero();
      }
    }

  if( aBondedInteraction )
    {
    for(int f = 0; f < FragsInHoodCount; f++ )
      {
      int fragId = mThrobberIF.GetFragIdInHood( f );
      XYZ * PosPtr  = mThrobberIF.GetFragPositPtrInHood( fragId );
      XYZ * FragForces = mThrobberIF.GetFragForcePtr( fragId );      

      mInteractionStateManagerIF->AddFragment( fragId, PosPtr, FragForces );
      }
    }

  if( aAssignmentViolation && !aBondedInteraction )
    {
    mFragsInRangeCount = 0;

    int FragsInHoodCount = mThrobberIF.GetFragsInHoodCount();
    for(int f = 0; f < FragsInHoodCount; f++ )
      {
      int fragId = mThrobberIF.GetFragIdInHood( f );
      
      if( fragId == -1 )
        continue;

      XYZ * PosPtr  = mThrobberIF.GetFragPositPtrInHood( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      VoxelIndex VoxIndex1 = mInteractionStateManagerIF->GetVoxelIndex( PosPtr,
                                                                        SiteCount );

      XYZ * FragForces = mThrobberIF.GetFragForcePtr( fragId );      

      mInteractionStateManagerIF->SetVoxelIndex( fragId, VoxIndex1 );

      mInteractionStateManagerIF->AddFragment( fragId, PosPtr, FragForces );

      // Need this here because of a need to have a force allocated 
      // for "culling" 
      // NOTE: The downside of doing this is that we have extra fragments
      // on assignment recook time step AND we don't need it for the packet
      // layer
      mFragsInRange[ mFragsInRangeCount ] = fragId;
      mFragsInRangeCount++;
      mInteractionStateManagerIF->SetUseFragment( fragId, 1 );
      }
    }
  else
    {
    for(int f = 0; f < mFragsInRangeCount; f++ )
      {
      FragId fragId            = mFragsInRange[ f ];
      
      XYZ * FragPosits = mThrobberIF.GetFragPositPtrInHood( fragId );
      XYZ * FragForces = mThrobberIF.GetFragForcePtr( fragId );
      
      mInteractionStateManagerIF->AddFragment( fragId, FragPosits, FragForces );
      }
    }

  mPostAssignment = aAssignmentViolation;

  gBroadcastFinis.HitOE( PKTRACE_NEIGHBORHOOD_COMM_DRIVER_BROADCAST,
                         TraceBcastString,
                         Platform::Topology::GetAddressSpaceId(),
                         gBroadcastFinis );

  BegLogLine( 0 )
    << aSimTick
    << " Broadcast:: Leaving..."
    << EndLogLine;
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
void NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::SetVoxelIndexToVoxelOrdinalMap( int* aVoxelIndexToVoxelOrdinalMap,
                                                                                         int  aVoxelsInRange,
                                                                                         int  aTotalNumberOfVoxels )
  {
  mVoxelIndexToVoxelOrdinalMap = aVoxelIndexToVoxelOrdinalMap;
  mVoxelsInRange               = aVoxelsInRange;
  mTotalNumberOfVoxels         = aTotalNumberOfVoxels;
  }

template<class ThrobberIF, int FRAGMENT_COUNT, int DriverDesc>
void NeighborhoodCommDriver<ThrobberIF, FRAGMENT_COUNT, DriverDesc>::Reduce( int aSimTick )
  {
  BegLogLine( 0 )
    << aSimTick
    << " Reduce:: Entering..."
    << EndLogLine;

  char TraceReduceString[ 128 ];
  strcpy( TraceReduceString, CommDriverNames[ DriverDesc ] );
  strcat( TraceReduceString, "Reduce");

  gReduceStart.HitOE( PKTRACE_NEIGHBORHOOD_COMM_DRIVER_REDUCE,
                      TraceReduceString,
                      Platform::Topology::GetAddressSpaceId(),
                      gReduceStart );

  if( mAssignmentRecookTS && mThrobberIF.DoesCulling() )
    mInteractionStateManagerIF->CullUnusedFragments();

  Platform::Control::Barrier();
  
//   gThrobberReduceStart.HitOE( PKTRACE_THROBBER_REDUCE,
//                               "ThrobberReduceStart",
//                               Platform::Topology::GetAddressSpaceId(),
//                               gThrobberReduceStart );

  mThrobberIF.ReductionExecute( aSimTick );

//   gThrobberReduceFinis.HitOE( PKTRACE_THROBBER_REDUCE,
//                               "ThrobberReduceFinis",
//                               Platform::Topology::GetAddressSpaceId(),
//                               gThrobberReduceFinis );

  BegLogLine( 0 )
    << "Reduce:: Got to after Reduce"
    << EndLogLine;

  for( int i=0; i<mIntegratorStateManagerIF->GetNumberOfLocalFragments(); i++ )
    {
    FragId fragmentId = mIntegratorStateManagerIF->GetNthFragmentId( i );

    XYZ * FragForces = mThrobberIF.GetFragForcePtr( fragmentId );
    int   SiteCount  = CommHelper::GetNumberOfSitesForFragment( fragmentId );

    for( int offset=0; offset<SiteCount; offset++ )
      {
      mIntegratorStateManagerIF->AddForce( fragmentId, offset, FragForces[ offset ] );
      }
    }

  BegLogLine( 0 )
    << aSimTick
    << " Reduce:: Leaving..."
    << EndLogLine;

  gReduceFinis.HitOE( PKTRACE_NEIGHBORHOOD_COMM_DRIVER_REDUCE,
                      TraceReduceString,
                      Platform::Topology::GetAddressSpaceId(),
                      gReduceFinis );
  }
#endif
