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
 #ifndef __INTEGRATOR_STATE_MANAGER_IF_HPP__
#define __INTEGRATOR_STATE_MANAGER_IF_HPP__

#define VERLET_LIST_VIOLATION         1
#define FRAGMENT_ASSIGNMENT_VIOLATION 2
#define FRAGMENT_BOUNDING_BOX_VIOLATION 4

#if !defined(PKFXLOG_ISMIF)
#define PKFXLOG_ISMIF (0)
#endif

//#ifndef DETERMINISTIC_REDUCTION
//#define DETERMINISTIC_REDUCTION (1)
//#endif

// quadfloat_integrate adds a (52-bit) 'double' to a (104-bit) 'quad', giving a
// 104-bit result.
// The 'volatile' qualifiers are important, to prevent the optimiser from coalescing 
// floating-point adds and subtracts. They must run as coded to get the extended precision
// right. See, for example, 'Priest 1992', or the 'qd' package in http://crd.lbl.gov/~dhbailey/mpdist/
static void quadfloat_integrate_seq(
    double& new_hi,
    double& new_lo,
    double v0,
    double v1, 
    double v2)
{
  
  double sum_hi = v0 + v1 ;
  volatile double v_sum_hi = sum_hi ; 
  volatile double chaff_1 = v_sum_hi - v0 ;
  double chaff = chaff_1 - v1 ;
  double sum_lo = v2 - chaff ;

  double s_hi = sum_hi + sum_lo ;
  volatile double v_s_hi = s_hi ;
  volatile double s_chaff_1 = v_s_hi - sum_hi ;
  double s_chaff = s_chaff_1 - sum_lo ;
  new_hi = s_hi ;
  new_lo = -s_chaff ;
}
static void quadfloat_integrate(
    double& new_hi,
    double& new_lo,
    double old_hi,
    double old_lo,
    double delta)
{
  double abs_old_hi=fabs(old_hi) ;
  double abs_old_lo=fabs(old_lo) ;
  double abs_delta=fabs(delta) ;
  if( abs_delta > abs_old_hi )
    {
    quadfloat_integrate_seq(new_hi,new_lo,delta,old_hi,old_lo) ;
    }
  else if( abs_delta > abs_old_lo)
    {
    quadfloat_integrate_seq(new_hi,new_lo,old_hi,delta,old_lo) ;
    }
  else 
    {
    quadfloat_integrate_seq(new_hi,new_lo,old_hi,old_lo,delta) ;
    }
       
}

//#include <BlueMatter/math.hpp>
extern unsigned SimTick;

//extern double dk1 ; // == 1.0 but the compiler does not know this

#if 0
class PositionPacket
  {
public:
  FragId  mFragId;
  VoxelIndex mVoxelIndex;
 // we want packets to be int multiples and mFragId is a short
  unsigned short  mP;
  // mPosition is deprecated; move to hiding the internal structure.
#if defined(ISM_PACKET_ORDINAL)
  double mPositionOrdinal[ 3 ] ; // This gets oversubscripted
  void SetPosition(unsigned int index, XYZ aPosition)
  {
        mPositionOrdinal[3*index+0] = aPosition.mX ;
        mPositionOrdinal[3*index+1] = aPosition.mY ;
        mPositionOrdinal[3*index+2] = aPosition.mZ ;
  }
  void GetPosition(unsigned int index, XYZ& aPosition)
  {
        aPosition.mX = mPositionOrdinal[3*index+0] ;
        aPosition.mY = mPositionOrdinal[3*index+1] ;
        aPosition.mZ = mPositionOrdinal[3*index+2] ;
  }
#else
  XYZ mPosition[ 1 ]; // This gets oversubscripted
  void SetPosition(unsigned int index, XYZ aPosition)
  {
        mPosition[index] = aPosition ;
  }
  void GetPosition(unsigned int index, XYZ& aPosition)
  {
        aPosition = mPosition[index] ;
  }
#endif

  };

class ForcePacket
  {
public:
  FragId mFragId;
  unsigned short mCulled;
#ifdef CHECKSUM_FORCES_IN_REDUCE
  double checksum;
#endif
  // mForce is deprecated; move to hiding the internal structure
#if defined(ISM_PACKET_ORDINAL)
  double mForceOrdinal[ 3 ] ;
  void ZeroForce(unsigned int index)
  {
        mForceOrdinal[3*index+0] = 0.0 ;
        mForceOrdinal[3*index+1] = 0.0 ;
        mForceOrdinal[3*index+2] = 0.0 ;

  }
  void SetForce(unsigned int index, XYZ aForce)
  {
        mForceOrdinal[3*index+0] = aForce.mX ;
        mForceOrdinal[3*index+1] = aForce.mY ;
        mForceOrdinal[3*index+2] = aForce.mZ ;
  }
  void GetForce(unsigned int index, XYZ& aForce)
  {
        aForce.mX = mForceOrdinal[3*index+0] ;
        aForce.mY = mForceOrdinal[3*index+1] ;
        aForce.mZ = mForceOrdinal[3*index+2] ;
  }
#else
  XYZ mForce[ 1 ]; // This gets oversubscripted
  void ZeroForce(unsigned int index)
  {
        mForce[index].mX = 0.0 ;
        mForce[index].mY = 0.0 ;
        mForce[index].mZ = 0.0 ;
  }
  void SetForce(unsigned int index, XYZ aForce)
  {
        mForce[index] = aForce ;
  }
  void GetForce(unsigned int index, XYZ& aForce)
  {
        aForce = mForce[index] ;
  }

#endif
  } ;

#endif

/*
 *  INTEGRATOR STATE
 */
  class IntegratorFragmentState
    {
    public:
    XYZ mPosition[ MAX_SITES_IN_FRAGMENT ];
    XYZ mVelocity[ MAX_SITES_IN_FRAGMENT ];
    XYZ mForce[ MAX_SITES_IN_FRAGMENT ];

    // Needed for detecting verlet list violation
    XYZ mFragmentVerletMonitoringPosition[ MAX_SITES_IN_FRAGMENT ];
    XYZ mFragmentAssignmentMonitoringPosition[ MAX_SITES_IN_FRAGMENT ];

    XYZ mPositionImageTracker[ MAX_SITES_IN_FRAGMENT ] MEMORY_ALIGN( 5 );        
        
    VoxelIndex mVoxelIndex;
        
#ifdef PK_PHASE5_SPLIT_COMMS
    double padding0[ 4 ];    
    XYZ mForceCore1[ MAX_SITES_IN_FRAGMENT ] MEMORY_ALIGN( 5 );
#if !defined(BASIC_REDUCTION)
    XYZ mForceLowerBits[MAX_SITES_IN_FRAGMENT] MEMORY_ALIGN( 5 );
#endif
    double padding1[ 4 ];    
#endif

    void ZeroPositionTracker()
      {
      for( int i=0; i<MAX_SITES_IN_FRAGMENT; i++ )
        {
        mPositionImageTracker[ i ].Zero();
        }
      }

    void
    Invalidate()
      {
      mVoxelIndex = Math::Infinity;
      for( int i=0; i<MAX_SITES_IN_FRAGMENT; i++ )
        {
        mPosition[ i ].Invalidate();
        mFragmentVerletMonitoringPosition[ i ].Invalidate();
        mVelocity[ i ].Invalidate();
        }
      }

    inline
    unsigned int
    IsInvalid()
      {
      return mPosition[ 0 ].IsInvalid();
      }

    } MEMORY_ALIGN( 5 );

  struct IntegratorStateTableRecord
    {
    IntegratorFragmentState     mIntegratorFragmentState;
    } MEMORY_ALIGN( 5 );

class IntegratorStateManagerIF
  {
  public:
 struct ThisPtrArgs
   {
   IntegratorStateManagerIF* mThisPtr;
   };

  IntegratorStateTableRecord     mIntegratorStateTable[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN( 5 );

  // This list containts the local fragments
  FragId                         mIntegratorShortList[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN( 5 );
  int                            mIntegratorShortListSize;

  ORBNode*                       mRecBisTree;
      
  FragmentRep* mSiteIdToFragmentRepMap;

// IntegratorStateManagerIF() {}

  inline
  void
  ChecksumPositions( double aGoldenChecksum )
    {
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
    double PositionCheckSumD = ChecksumPositionsD();

    if( PositionCheckSumD != aGoldenChecksum )
      {
      BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
        << "SimTick: " << SimTick
        << " aGoldenChecksum : " << aGoldenChecksum
        << " PositionCheckSumD: " << PositionCheckSumD
        << " ERROR:: Position checksum failed"
        << EndLogLine;

      PLATFORM_ABORT( "ERROR:: Position checksum failed" );
      }
#endif
    }

  inline
  void
  AssignVoxelIndex()
    {
    for( int f = 0; f < mIntegratorShortListSize; f++)
      {
      FragId fragId = mIntegratorShortList[ f ];
      VoxelIndex voxIndex = GetVoxelIndexForFragment( fragId );
      SetVoxelIndex( fragId, voxIndex );
      }
    }

  inline
  void
  ImageAllPositionsIntoCentralCell( int aSimTick, XYZ& aCenterOfCentralCell )
    {
    for( int f = 0; f < mIntegratorShortListSize; f++)
      {
      FragId fragId = mIntegratorShortList[ f ];

      int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      SiteId firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int TagAtomIndex = MSD_IF::GetTagAtomIndex( fragId );

      XYZ TagAtomPos = GetPosition( fragId, TagAtomIndex );
      XYZ TagAtomPosImaged;
      NearestImageInPeriodicVolume( aCenterOfCentralCell, TagAtomPos, TagAtomPosImaged );
      SetPosition( fragId, TagAtomIndex, TagAtomPosImaged );

      AddToPositionTracker( fragId, TagAtomIndex, ( TagAtomPos - TagAtomPosImaged ));

      for( int offset = 0; offset < numSites; offset++ )
        {
        if( offset != TagAtomIndex )
          {
          XYZ Pos = GetPosition( fragId, offset );
          XYZ PosImaged;
          NearestImageInPeriodicVolume( TagAtomPosImaged, Pos, PosImaged );
          SetPosition( fragId, offset, PosImaged );
          AddToPositionTracker( fragId, offset, ( Pos - PosImaged ));
          }
        }
      }
    }

  inline
  unsigned int
  ChecksumPositions()
    {
     unsigned int checksum = 0;

      BegLogLine( 0 )
      << "ChecksumPositions():: mIntegratorShortListSize= " << mIntegratorShortListSize
      << " &checksum= " << (void*) &checksum
      << EndLogLine;

    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fId = mIntegratorShortList[ i ];

      int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fId );

      assert( numSites >= 1 && numSites <= 4 );

      for( int offset = 0; offset < numSites; offset++ )
        {
        XYZ Pos = GetPosition( fId, offset );
        unsigned int* PosPtr = (unsigned int*) &Pos;

        for( int j=0; j<( sizeof(XYZ) / sizeof( unsigned int )); j++ )
          {
          checksum += PosPtr[ j ];
          }
        }
      }

    return checksum;
    }

  inline
  double
  ChecksumPositionsD()
    {
    double checksum = 0.0;

    BegLogLine( 0 )
      << "ChecksumPositions():: mIntegratorShortListSize= " << mIntegratorShortListSize
      << " &checksum= " << (void*) &checksum
      << EndLogLine;

    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fId = mIntegratorShortList[ i ];

      int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fId );

      assert( numSites >= 1 && numSites <= 4 );

      for( int offset = 0; offset < numSites; offset++ )
        {
        XYZ Pos = GetPosition( fId, offset );

        checksum += ( Pos.mX + Pos.mY + Pos.mZ );
        // unsigned int* PosPtr = (unsigned int*) &Pos;
//         BegLogLine( 1 )
//           << "ChecksumPosition(): "
//           << " Pos: " << Pos
//           << EndLogLine;

//         for( int j=0; j<( sizeof(XYZ) / sizeof( unsigned int )); j++ )
//           {
//           BegLogLine( 1 )
//             << "ChecksumPosition(): "
//             << " PosPtr[ " << j << " ]= " << PosPtr[ j ]
//             << EndLogLine;

//           checksum += PosPtr[ j ];
//           }
        }
      }

    return checksum;
    }

  inline
  double
  ChecksumVelocities()
    {
    double checksum = 0;

    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fId = mIntegratorShortList[ i ];

      int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fId );

      for( int offset = 0; offset < numSites; offset++ )
        {
        XYZ Vel = GetVelocity( fId, offset );
        checksum += ( Vel.mX + Vel.mY + Vel.mZ );
        }
      }

    return checksum;
    }

  inline
  void
  Dumpelocity( int aSimTick )
    {
    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];

      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset = 0; offset < SiteCount; offset++ )
        {
        BegLogLine( 1 )
          << aSimTick << " Velocity " << (FirstSiteId+offset ) << " " << GetVelocity( fragId, offset )
          << EndLogLine;
        }
      }
    }

  inline
  void
  DumpPositions( int aSimTick )
    {
    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];

      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset = 0; offset < SiteCount; offset++ )
        {
        BegLogLine( (aSimTick == 10) && (( fragId == 1758 ) || (fragId == 2087 )))
          << aSimTick << " Position " << (FirstSiteId+offset ) << " " << GetPosition( fragId, offset )
          << EndLogLine;
        }
      }
    }

  inline
  void
  DumpForces( int aSimTick )
    {
    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];

      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset = 0; offset < SiteCount; offset++ )
        {
        BegLogLine( aSimTick == 5091000 )
          << aSimTick << " GraForce " << (FirstSiteId+offset ) << " " << GetForce( fragId, offset )
          << EndLogLine;
        }
      }
    }

  inline
  void
  Invalidate( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.Invalidate();
    }

  inline
  unsigned int
  IsInvalid( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    unsigned int rc = mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.IsInvalid();
    return ( rc );
    }

  inline
  void
  ZeroForce( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    for( int offset=0; offset < MAX_SITES_IN_FRAGMENT; offset++ )
       {
          mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForce[ offset ].Zero();
       }
    }

#ifdef PK_PHASE5_PROTOTYPE
  inline
  void
  ZeroForceCore1( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    for( int offset=0; offset < MAX_SITES_IN_FRAGMENT; offset++ )
       {
          mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ offset ].Zero();
#if !defined(BASIC_REDUCTION)
          mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ offset ].Zero();
#endif
       }  
    }
#endif

  static
  void*
  InitActorFx(void *args)
    {
    ThisPtrArgs* targs = (ThisPtrArgs *) args;
    IntegratorStateManagerIF *ThisPtr = targs->mThisPtr;

    ThisPtr->mIntegratorShortListSize=0;

//     for( int i=0; i < NUMBER_OF_FRAGMENTS; i++ )
//       {
//       ThisPtr->mIntegratorStateTable[ i ].mIntegratorFragmentState = (IntegratorFragmentState *) malloc( sizeof( IntegratorFragmentState ) ) ;
//       if( ThisPtr->mIntegratorStateTable[ i ].mIntegratorFragmentState == NULL )
//         {
//         PLATFORM_ABORT("mIntegratorStateTable[ i ].mIntegratorFragmentState == NULL");
//         }
//       }

    return NULL;
    }

  inline
  void
  Init( FragmentRep* aSiteIdToFragmentRepMap )
    {

    mSiteIdToFragmentRepMap = aSiteIdToFragmentRepMap;

    ThisPtrArgs targs;
    targs.mThisPtr = this;
    InitActorFx( &targs );

    for( int i=0; i < NUMBER_OF_FRAGMENTS; i++ )
      {
      mIntegratorStateTable[ i ].mIntegratorFragmentState.Invalidate();
      mIntegratorStateTable[ i ].mIntegratorFragmentState.ZeroPositionTracker();
      ZeroForce( i );
      }
    }

  inline
  void
  Reset()
    {
    ZeroForces();
    mIntegratorShortListSize=0;
    for( int i=0; i < NUMBER_OF_FRAGMENTS; i++ )
      {
      mIntegratorStateTable[ i ].mIntegratorFragmentState.Invalidate();
      }
    }

  inline
  void
  AddFragment( FragId aFragId )
    {
    if( mIntegratorShortListSize < 0 || mIntegratorShortListSize >= (NUMBER_OF_FRAGMENTS - 1 ) )
      PLATFORM_ABORT("No room in short list for new fragment.");

    ZeroForce( aFragId );

    mIntegratorShortList[ mIntegratorShortListSize ] = aFragId;
    mIntegratorShortListSize++;
    }

  inline
  void
  AreFragsConserved()
   {
#ifdef DEBUG_P4
   unsigned int count = 0;
   for( int f = 0; f < mIntegratorShortListSize; f++)
     {
     FragId fragId = mIntegratorShortList[ f ];
     count += fragId;
     }

   int TotalCount = 0;

   MPI_Allreduce( &count, &TotalCount, 1, MPI_INT, MPI_SUM, Platform::Topology::cart_comm );

   assert( TotalCount == ( NUMBER_OF_FRAGMENTS * (NUMBER_OF_FRAGMENTS-1) ) / 2.0 );
#endif
   }

  inline
  void
  RemoveFragment( FragId aFragId )
    {
    assert( mIntegratorShortListSize > 0 && mIntegratorShortListSize < NUMBER_OF_FRAGMENTS );
    int Found      = 0;
    int FragIndex = 0;

    for( FragIndex = 0; FragIndex < mIntegratorShortListSize; FragIndex++)
      {
      if( mIntegratorShortList[ FragIndex ] == aFragId )
        {
        Found = 1;
        break;
        }
      }

    if( ! Found )
      PLATFORM_ABORT("Fail to find fragment for removal.");

    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.Invalidate();

    // Copy everything over by one
    for( int j = FragIndex ; j < (mIntegratorShortListSize-1); j++ )
      {
      mIntegratorShortList[ j ] = mIntegratorShortList[ j + 1 ];
      }

    mIntegratorShortListSize--;
    assert( mIntegratorShortListSize >= 0 );
    }

///////  inline
///////  void
///////  RemoveFragment( FragId aFragId )
///////    {
///////    for( int i=0; i<mIntegratorShortListSize; i++)
///////      {
///////      if( mIntegratorShortList[ i ] == aFragId )
///////        {
///////        // Copy everything over by one
///////        for( int j=i; j < (mIntegratorShortListSize-1); j++ )
///////          {
///////          mIntegratorShortList[ j ] = mIntegratorShortList[ j+1 ];
///////          }
///////        }
///////      }
///////
///////    mIntegratorShortListSize--;
///////    assert( mIntegratorShortListSize >= 0 );
///////    }

  void
  EmitState( int aSimTick )
    {
    for( int f = 0; f < mIntegratorShortListSize; f++)
      {
      FragId fragId = mIntegratorShortList[ f ];

      int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      SiteId firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );

      for( int offset = 0; offset < numSites; offset++ )
        {
        SiteId s = firstSiteId + offset;
        SiteId externalSiteId = MSD_IF::GetExternalSiteIdForInternalSiteId( s );

        BegLogLine( 0 )
          << aSimTick
          << " GetPositionTracker( " << fragId << " , " << offset << " )= "
          <<   GetPositionTracker( fragId, offset )
          << EndLogLine;

        XYZ Pos = GetPosition( fragId, offset ) + GetPositionTracker( fragId, offset );

        ED_Emit_DynamicVariablesSite( aSimTick,
                                      Platform::Topology::GetAddressSpaceId(),  // using 0 for voxel id
                                      externalSiteId,
                                      Pos,
                                      GetVelocity( fragId, offset ) );
        }
      }
    }

  void
  SetORB( ORBNode* aORB )
    {
    mRecBisTree = aORB;
    }

  inline
  void
  UpdateVoxelIndex( FragId aFragId )
    {
    VoxelIndex voxelIndex = GetVoxelIndexForFragment( aFragId );
    SetVoxelIndex( aFragId, voxelIndex );
    }

   XYZ
   GetFragmentPosition( FragId aFragId )
     {
     int NumberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );

     XYZ LowerLeft = GetPosition( aFragId, 0 );
     XYZ UpperRight;
     double bX = LowerLeft.mX ;
     double bY = LowerLeft.mY ;
     double bZ = LowerLeft.mZ ;
     double cX = bX ;
     double cY = bY ;
     double cZ = bZ ;

     for( int i=1; i < NumberOfSites; i++ )
       {
       XYZ AtomLoc = GetPosition( aFragId, i );
       double dX = AtomLoc.mX ;
       double dY = AtomLoc.mY ;
       double dZ = AtomLoc.mZ ;
       bX = min(bX,dX) ;
       bY = min(bY,dY) ;
       bZ = min(bZ,dZ) ;
       cX = max(cX,dX) ;
       cY = max(cY,dY) ;
       cZ = max(cZ,dZ) ;
       }

     LowerLeft.mX = bX ;
     LowerLeft.mY = bY ;
     LowerLeft.mZ = bZ ;
     UpperRight.mX = cX ;
     UpperRight.mY = cY ;
     UpperRight.mZ = cZ ;

     XYZ MidXYZ = 0.5 * ( LowerLeft + UpperRight );

     return MidXYZ;
     }

//   XYZ& GetTagAtomPosition( FragId aFragId )
//     {
//     assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
//     int offset = MSD_IF::GetTagAtomIndex( aFragId );

//     // XYZ pos = NSQ_NeighbourList.GetFragmentCentre( aFragId );
//     XYZ& pos = GetPosition( aFragId, offset );

//     return pos;
//     }

  VoxelIndex GetVoxelIndexForFragment( int aFragId )
    {
    // XYZ TagAtomPos        = GetTagAtomPosition( aFragId );
    XYZ FragmentPosition     = GetFragmentPosition( aFragId );
    VoxelIndex voxelIndex = mRecBisTree->GetVoxelIndex( FragmentPosition );
    return voxelIndex;
    }

//   inline
//   void
//   UpdateVoxelIndexesForFragment()
//     {
//     for( int i=0; i < mIntegratorShortListSize; i++ )
//       {
//       FragId fragId = mIntegratorShortList[ i ];
//       VoxelIndex NewVoxelIndex = GetVoxelIndexForFragment( fragId );

//       mIntegratorStateTable[ fragId ].mIntegratorFragmentState->mVoxelIndex = NewVoxelIndex;
//       }
//     }

  inline
  int
  CheckBoundingBoxViolation( XYZ& aLowerBoundingBox,
                             XYZ& aUpperBoundingBox )
    {
    for( int i=0; i < mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < numberOfSites; offset++ )
        {
        XYZ & Pos      = GetPosition( fragId, offset );

        unsigned bX = (( Pos.mX > aUpperBoundingBox.mX ) || ( Pos.mX < aLowerBoundingBox.mX ));
        unsigned bY = (( Pos.mY > aUpperBoundingBox.mY ) || ( Pos.mY < aLowerBoundingBox.mY ));
        unsigned bZ = (( Pos.mZ > aUpperBoundingBox.mZ ) || ( Pos.mZ < aLowerBoundingBox.mZ ));

        if ( bX || bY || bZ )
          {
          BegLogLine( 1 )
            << "VIOLATOR Pos: " << Pos
            << " from FragId: " << fragId
            << " offset: " << offset
            << " aLowerBoundingBox: " << aLowerBoundingBox
            << " aUpperBoundingBox: " << aUpperBoundingBox
            << EndLogLine;

          return 1;
          }
        }
      }

    return 0;
    }

  int
  IsFragmentLocal( FragId aFragId )
    {
    // WARNING!!! This method should almost NEVER be called. Only for temporary support
    // of the previous version.
    // Therefore this implementation is SEVERELY inefficient
    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      if( mIntegratorShortList[ i ] == aFragId )
        return 1;
      }
    return 0;
    }

  inline
  void
  UpdateDisplacementForFragmentVerletListMonitoring()
    {
    for( int i=0; i < mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < numberOfSites; offset++ )
        {
        SetFragmentVerletMonitoringPosition( fragId, offset, GetPosition( fragId, offset ) );
        }
      }
    }

  inline
  void
  UpdateDisplacementForFragmentAssignmentMonitoring()
    {
    for( int i=0; i < mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < numberOfSites; offset++ )
        {
        SetFragmentAssignmentMonitoringPosition( fragId, offset, GetPosition( fragId, offset ) );
        }
      }
    }

// this should go somewhere else
inline
void
DistanceOutOfBoundingBox( double &aDistOut, XYZ &NearHomePos, XYZ &BoxHigh, XYZ &BoxLow )
  {
  double dx, dy, dz;

  if( NearHomePos.mX > BoxHigh.mX )
    dx = NearHomePos.mX - BoxHigh.mX;
  else if( NearHomePos.mX < BoxLow.mX )
    dx = BoxLow.mX - NearHomePos.mX;
  else
    dx = 0.0;

  if( NearHomePos.mY > BoxHigh.mY )
    dy = NearHomePos.mY - BoxHigh.mY;
  else if( NearHomePos.mY < BoxLow.mY )
    dy = BoxLow.mY - NearHomePos.mY;
  else
    dy = 0.0;

  if( NearHomePos.mZ > BoxHigh.mZ )
    dz = NearHomePos.mZ - BoxHigh.mZ;
  else if( NearHomePos.mZ < BoxLow.mZ )
    dz = BoxLow.mZ - NearHomePos.mZ;
  else
    dz = 0.0;

  aDistOut = sqrt( dx*dx + dy*dy + dz*dz );

  }


  inline
  int
  CheckSiteDisplacement( double aVerletListGuardZone,
                         double aFragmentAssignmentGuardZone,
                         XYZ&   aLowerBoundingBox,
                         XYZ&   aUpperBoundingBox )
    {

#if 0
int myP_x, myP_y, myP_z;
Platform::Topology::GetMyCoords( &myP_x, &myP_y, &myP_z );
ORBNode* MyLeaf = mRecBisTree->NodeFromProcCoord( myP_x, myP_y, myP_z );
assert( MyLeaf );
XYZ HomeCenter;
HomeCenter.mX = (MyLeaf->low[0] + MyLeaf->high[0]) / 2;
HomeCenter.mY = (MyLeaf->low[1] + MyLeaf->high[1]) / 2;
HomeCenter.mZ = (MyLeaf->low[2] + MyLeaf->high[2]) / 2;

XYZ BoxHigh;
BoxHigh.mX = MyLeaf->high[0];
BoxHigh.mY = MyLeaf->high[1];
BoxHigh.mZ = MyLeaf->high[2];

XYZ BoxLow;
BoxLow.mX = MyLeaf->low[0];
BoxLow.mY = MyLeaf->low[1];
BoxLow.mZ = MyLeaf->low[2];

int SignalFragId = -1;
#define DEF_SignalFragId
#endif

    int ViolationBits = 0;

    double HalfVerletGuardZone =  aVerletListGuardZone / 2.0;
    double HalfVerletGuardZoneSquared = HalfVerletGuardZone * HalfVerletGuardZone;

    double HalfAssignmentGuardZone =  aFragmentAssignmentGuardZone / 2.0;
    double HalfAssignmentGuardZoneSquared = HalfAssignmentGuardZone * HalfAssignmentGuardZone;

    unsigned SkipVerletCheck = 0;
    unsigned SkipAssignmentCheck = 0;

    for( int i=0; i < mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < numberOfSites; offset++ )
        {
        XYZ & Pos            = GetPosition( fragId, offset );

#if 0
        XYZ ap1 = GetFragmentPosition( fragId );
        XYZ AveragePosition1;
        NearestImageInPeriodicVolume( HomeCenter , ap1, AveragePosition1  );
        
        double AveragePositionDistanceOutOfHomeZone1;
        
        DistanceOutOfBoundingBox( AveragePositionDistanceOutOfHomeZone1, AveragePosition1, BoxHigh, BoxLow );
        
        BegLogLine( SimTick == 5096288 && ( fragId == 4399 || fragId == 5963 ))
          << SimTick 
          << " fragId: " << fragId
          << " numberOfSites: " << numberOfSites
          << " offset: " << offset
          << " Pos: " << Pos
          << " BoxLow: " << BoxLow
          << " BoxHigh: " << BoxHigh
          << " FragPos: " << ap1
          << " AveragePosition: " << AveragePosition1
          << " AveragePositionDistanceOutOfHomeZone: " << AveragePositionDistanceOutOfHomeZone1
          << " VoxelIndex: " << mRecBisTree->GetVoxelIndex( ap1 )
          << " extent: " << Pos.Distance( ap1 )
          << EndLogLine;
        
        fflush( stdout );
#endif

        if( !SkipVerletCheck )
          {
          XYZ & VerletPos      = GetFragmentVerletMonitoringPosition( fragId, offset );
          double VerletDistanceSquared = Pos.DistanceSquared( VerletPos );
          if( VerletDistanceSquared >= HalfVerletGuardZoneSquared )
            {
            ViolationBits |= VERLET_LIST_VIOLATION;
            }
          }

        if( !SkipAssignmentCheck )
          {
#if 0 // try out AP based frag migration as p4 design would suggest - looks for center out of box

      XYZ ap = GetFragmentPosition( fragId );
      XYZ AveragePosition;
      NearestImageInPeriodicVolume( HomeCenter , ap, AveragePosition  );

      double AveragePositionDistanceOutOfHomeZone;

      DistanceOutOfBoundingBox( AveragePositionDistanceOutOfHomeZone, AveragePosition, BoxHigh, BoxLow );

      double AveragePositionDistanceOutOfHomeZoneSquared =
             AveragePositionDistanceOutOfHomeZone * AveragePositionDistanceOutOfHomeZone;

      if( AveragePositionDistanceOutOfHomeZoneSquared >= HalfAssignmentGuardZoneSquared )
        {
        ViolationBits |= FRAGMENT_ASSIGNMENT_VIOLATION;
  #ifdef DEF_SignalFragId
        SignalFragId = fragId;
  #endif
        BegLogLine( 0 )
            << SimTick
            << " " << (void *) ViolationBits
            << " " << HalfAssignmentGuardZoneSquared
            << " " << AveragePosition
            << " " << HomeCenter
            << " " << AveragePositionDistanceOutOfHomeZone
            << " " << AveragePositionDistanceOutOfHomeZoneSquared
            << " " << BoxLow
            << " " << BoxHigh
            << EndLogLine;
        }

#endif // scans for atom displacement to 1/2 comm

          XYZ & AssignmentPos  = GetFragmentAssignmentMonitoringPosition( fragId, offset );
          double AssignmentDistanceSquared = Pos.DistanceSquared( AssignmentPos );
          if( AssignmentDistanceSquared >= HalfAssignmentGuardZoneSquared )
            {
            ViolationBits |= FRAGMENT_ASSIGNMENT_VIOLATION;
            }
          }

        unsigned bX = (( Pos.mX > aUpperBoundingBox.mX ) || ( Pos.mX < aLowerBoundingBox.mX ));
        unsigned bY = (( Pos.mY > aUpperBoundingBox.mY ) || ( Pos.mY < aLowerBoundingBox.mY ));
        unsigned bZ = (( Pos.mZ > aUpperBoundingBox.mZ ) || ( Pos.mZ < aLowerBoundingBox.mZ ));

        if ( bX || bY || bZ )
          {
          ViolationBits |= FRAGMENT_BOUNDING_BOX_VIOLATION;
          ////////////////////////////return ViolationBits;
          }
        }


      SkipVerletCheck = ViolationBits & VERLET_LIST_VIOLATION;
      SkipAssignmentCheck = ViolationBits & FRAGMENT_ASSIGNMENT_VIOLATION;
      }

    return ( ViolationBits );
    }

  void
  CollectMigrationStatistics( int AssignmentViolationFlag, XYZ CenterOfBoundingBox )
   {
    ///////////////////////////////////////////////////////////////////////////////////////////
    #if 0

    static double         LeftBackFCOGMaxDistOut  [ NUMBER_OF_FRAGMENTS ];
    static double         LeftBackAtomMaxDistOut  [ NUMBER_OF_FRAGMENTS ];
    static double         LeftBackMaxExtent       [ NUMBER_OF_FRAGMENTS ];
    static unsigned short LeftBackCount           [ NUMBER_OF_FRAGMENTS ];
    static unsigned short LeftBackTimeSteps       [ NUMBER_OF_FRAGMENTS ];

    static unsigned char  MigrationCandidateFlags [ NUMBER_OF_FRAGMENTS ];
    static unsigned char  MigrationCandidateIndex [ NUMBER_OF_FRAGMENTS ];
    static unsigned int   LastSimTickWasAssignmentViolationFlag;

    static unsigned int   NumberOfRecooks;

    static char LeftBackIfMigratedNow[ NUMBER_OF_FRAGMENTS ];

    enum { MaxRecordedLeftBackCount = 20 };
    double HistogramBinSize  = 0.1;
    enum { HistogramBinCount = 40 };

    struct IntSums
      {
      unsigned int    MigrationHoldBackHist[ MaxRecordedLeftBackCount + 1 ];
      unsigned int    ReleasedOnHist       [ MaxRecordedLeftBackCount + 1 ];
      unsigned int    FCOGDistOutHist[ HistogramBinCount + 1 ];
      unsigned int    AtomDistOutHist[ HistogramBinCount + 1 ];
      unsigned int    FragExtentHist [ HistogramBinCount + 1 ];
      unsigned int    HeldBackAtShortListIndexHist[ HistogramBinCount + 1 ];
      };

    struct FpMaxs
      {
      double          MaxAtomDistanceOut ;
      double          MaxFCOGDistanceOut ;
      double          MaxFragmentExtent  ;
      double          MaxHoldBackCount ;
      };


    static IntSums Sums;
    static FpMaxs  Maxs;

    static unsigned int    ReportCount;

    #ifndef MIGRATION_REPORT_MODULO
    #define MIGRATION_REPORT_MODULO (1000)
    #endif

    double WARN_FCOG_DOUT = 1.5;
    double WARN_ATOM_DOUT = 2.5;
    double WARN_FEXT_DOUT = 1.6;  // this is for frag 637 in lys

    // collect info on migration
    // note: posits have been updated once since the actual leave back... so no posit data here
    if( LastSimTickWasAssignmentViolationFlag )
      {
      // go through the current list and find out what was left behind
      for( int i=0; i < mIntegratorShortListSize; i++ )
        {
        FragId fragId = mIntegratorShortList[ i ];
        // This array of flags has a one set for every frag that was supposed to migrate
        if( MigrationCandidateFlags[ fragId ] )
          {
          // Turn off flag so later scan to see what did migrate doesn't hit it
          MigrationCandidateFlags[ fragId ] = 0;

          // increment the count of leave backs kept on a per frag basis
          LeftBackCount[ fragId ]++;

          if( LeftBackCount[ fragId ] >= MaxRecordedLeftBackCount )
            PLATFORM_ABORT(" way too many migration holdbacks " );

          // use that count to increment the a histogram bin
          Sums.MigrationHoldBackHist[ LeftBackCount[ fragId ] ]++;

          if(  MigrationCandidateIndex[ fragId ] < HistogramBinCount )
            Sums.HeldBackAtShortListIndexHist[ MigrationCandidateIndex[ fragId ] ]++;

          if(  LeftBackCount[ fragId ] > Maxs.MaxHoldBackCount
            || LeftBackFCOGMaxDistOut[ fragId ] > WARN_FCOG_DOUT
            || LeftBackAtomMaxDistOut[ fragId ] > WARN_ATOM_DOUT )
            {
            Maxs.MaxHoldBackCount = LeftBackCount[ fragId ];

            // Report on a new hold back level
            printf("E_NODE_NEW_MAX_HOLD_BACK_COUNT %d %d %d %f %f %f\n",
                   SimTick - 1, fragId,
                   LeftBackCount[ fragId ],
                   LeftBackFCOGMaxDistOut[ fragId ],
                   LeftBackMaxExtent[ fragId ],
                   LeftBackAtomMaxDistOut[ fragId ] );
            }
          }
        else
          {
          // look for a case where a frag is not a candidate for migration AND has a positive LeftBackCount
          // this means the frag wandered out of the homezone and then back without ever being migrated
          if( LeftBackCount[ fragId ] != 0 )
            {
            Sums.ReleasedOnHist[ LeftBackCount[ fragId ] ]++;

            if(    LeftBackCount[ fragId ] > 3
                || LeftBackFCOGMaxDistOut[ fragId ] > WARN_FCOG_DOUT
                || LeftBackAtomMaxDistOut[ fragId ] > WARN_ATOM_DOUT )
              {
              // Report on a new hold back level
              printf("E_NODE_FRAG_RELEASE %d %d %d %f %f %f\n",
                     SimTick - 1, fragId,
                     LeftBackCount[ fragId ],
                     LeftBackFCOGMaxDistOut[ fragId ],
                     LeftBackMaxExtent[ fragId ],
                     LeftBackAtomMaxDistOut[ fragId ] );
              }
            LeftBackFCOGMaxDistOut[ fragId ] = 0;
            LeftBackMaxExtent[ fragId ]      = 0;
            LeftBackAtomMaxDistOut[ fragId ] = 0;
            LeftBackCount[ fragId ] = 0;
            }
          }
        }
      // Now scan to see what did leave.  This could/should be done by keeping a list of last ts shortlist
      for( int fragId = 0; fragId < NUMBER_OF_FRAGMENTS; fragId++ )
        {
        if( MigrationCandidateFlags[ fragId ] )
          {
          MigrationCandidateFlags[ fragId ] = 0;

          Sums.ReleasedOnHist[ LeftBackCount[ fragId ] ]++;

          if( LeftBackCount[ fragId ] == 0 ) // 0 leave backs recorded in the migratioin hold back hist
            Sums.MigrationHoldBackHist[ 0 ]++;

          if(    LeftBackCount[ fragId ] > 3
              || LeftBackFCOGMaxDistOut[ fragId ] > WARN_FCOG_DOUT
              || LeftBackAtomMaxDistOut[ fragId ] > WARN_ATOM_DOUT )
            {
            // Report on a new hold back level
            printf("E_NODE_FRAG_RELEASE %d %d %d %f %f %f\n",
                   SimTick - 1, fragId,
                   LeftBackCount[ fragId ],
                   LeftBackFCOGMaxDistOut[ fragId ],
                   LeftBackMaxExtent[ fragId ],
                   LeftBackAtomMaxDistOut[ fragId ] );
            }

          LeftBackFCOGMaxDistOut[ fragId ] = 0;
          LeftBackMaxExtent[ fragId ]      = 0;
          LeftBackAtomMaxDistOut[ fragId ] = 0;
          LeftBackCount[ fragId ] = 0;
          }
        }
      }

    static int ReportTick;
    if( ReportTick == 0 )
      ReportTick = SimTick;

    // This block periodically writes out the counters.
    if( SimTick == ReportTick )
      {
      static int ReportCount = 0;

      // Node 0 does writes for reduced statistical values
      if( Platform::Topology::GetAddressSpaceId() == 0 )
        {
        printf("H_INFO %d %d %d %d\n",
                SimTick, ReportCount, ReportCount *  MIGRATION_REPORT_MODULO, NumberOfRecooks );
        }

      ReportCount++;
      ReportTick += MIGRATION_REPORT_MODULO;

      // Integer sum reduction
      IntSums ISB;
      MPI_Reduce( &Sums,
                  &ISB,
                  sizeof(IntSums)/sizeof(unsigned int),
                  MPI_UNSIGNED,
                  MPI_SUM,
                  0,
                  Platform::Topology::cart_comm );

      // Floating point max reduction
      FpMaxs FMB;
      MPI_Reduce( &Maxs,
                  &FMB,
                  sizeof(FpMaxs)/sizeof(double),
                  MPI_DOUBLE,
                  MPI_MAX,
                  0,
                  Platform::Topology::cart_comm );

#if 1
      // Node 0 does writes for reduced statistical values
      if( Platform::Topology::GetAddressSpaceId() == 0 )
        {
        // put out the total counts of leavebacks in bins
        unsigned *B = ISB.MigrationHoldBackHist;
        printf("H_MIGHB %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u\n",
               SimTick ,
               B[ 0],B[ 1], B[ 2], B[ 3], B[ 4], B[ 5], B[ 6], B[ 7], B[ 8], B[ 9],
               B[10],B[11], B[12], B[13], B[14], B[15], B[16], B[17], B[18], B[19], B[20] );

        B = ISB.ReleasedOnHist;
        printf("H_RELON %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u\n",
               SimTick ,
               B[ 0],B[ 1], B[ 2], B[ 3], B[ 4], B[ 5], B[ 6], B[ 7], B[ 8], B[ 9],
               B[10],B[11], B[12], B[13], B[14], B[15], B[16], B[17], B[18], B[19], B[20] );

        B = ISB.FCOGDistOutHist;
        printf("H_FDOUT %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u\n",
               SimTick ,
               B[ 0],B[ 1], B[ 2], B[ 3], B[ 4], B[ 5], B[ 6], B[ 7], B[ 8], B[ 9],
               B[10],B[11], B[12], B[13], B[14], B[15], B[16], B[17], B[18], B[19],
               B[20],B[21], B[22], B[23], B[24], B[25], B[26], B[27], B[28], B[29],
               B[30],B[31], B[32], B[33], B[34], B[35], B[36], B[37], B[38], B[39],
               B[40] );

        B = ISB.AtomDistOutHist;
        printf("H_ADOUT %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u\n",
               SimTick ,
               B[ 0],B[ 1], B[ 2], B[ 3], B[ 4], B[ 5], B[ 6], B[ 7], B[ 8], B[ 9],
               B[10],B[11], B[12], B[13], B[14], B[15], B[16], B[17], B[18], B[19],
               B[20],B[21], B[22], B[23], B[24], B[25], B[26], B[27], B[28], B[29],
               B[30],B[31], B[32], B[33], B[34], B[35], B[36], B[37], B[38], B[39],
               B[40] );

        B = ISB.FragExtentHist;
        printf("H_FEXT %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u\n",
               SimTick ,
               B[ 0],B[ 1], B[ 2], B[ 3], B[ 4], B[ 5], B[ 6], B[ 7], B[ 8], B[ 9],
               B[10],B[11], B[12], B[13], B[14], B[15], B[16], B[17], B[18], B[19],
               B[20],B[21], B[22], B[23], B[24], B[25], B[26], B[27], B[28], B[29],
               B[30],B[31], B[32], B[33], B[34], B[35], B[36], B[37], B[38], B[39],
               B[40] );

        B = ISB.HeldBackAtShortListIndexHist;
        printf("H_HBIND %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u %u %u %u %u   %u\n",
               SimTick ,
               B[ 0],B[ 1], B[ 2], B[ 3], B[ 4], B[ 5], B[ 6], B[ 7], B[ 8], B[ 9],
               B[10],B[11], B[12], B[13], B[14], B[15], B[16], B[17], B[18], B[19],
               B[20],B[21], B[22], B[23], B[24], B[25], B[26], B[27], B[28], B[29],
               B[30],B[31], B[32], B[33], B[34], B[35], B[36], B[37], B[38], B[39],
               B[40] );

        printf("H_FP_MAXS %d  %f %f %f %f\n",
                SimTick , FMB.MaxHoldBackCount, FMB.MaxAtomDistanceOut, FMB.MaxFCOGDistanceOut, FMB.MaxFragmentExtent );

        }

      // Zero histograms
      bzero( (void *) &Sums, sizeof( IntSums ) );

#endif

      }

    int myP_x, myP_y, myP_z;
    Platform::Topology::GetMyCoords( &myP_x, &myP_y, &myP_z );
    ORBNode* MyLeaf = mRecBisTree->NodeFromProcCoord( myP_x, myP_y, myP_z );
    assert( MyLeaf );
    XYZ HomeCenter;
    HomeCenter.mX = (MyLeaf->low[0] + MyLeaf->high[0]) / 2;
    HomeCenter.mY = (MyLeaf->low[1] + MyLeaf->high[1]) / 2;
    HomeCenter.mZ = (MyLeaf->low[2] + MyLeaf->high[2]) / 2;

    XYZ BoxHigh;
    BoxHigh.mX = MyLeaf->high[0];
    BoxHigh.mY = MyLeaf->high[1];
    BoxHigh.mZ = MyLeaf->high[2];

    XYZ BoxLow;
    BoxLow.mX = MyLeaf->low[0];
    BoxLow.mY = MyLeaf->low[1];
    BoxLow.mZ = MyLeaf->low[2];

    for( int i=0; i < mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      // Create an imaged FragementCenter ( Average Position, as used by the rest of the program )
      XYZ ap = GetFragmentPosition( fragId );
      XYZ FragmentCenter;
      NearestImageInPeriodicVolume( HomeCenter , ap, FragmentCenter  );

      double FragmentCenterDistanceOutOfHomeZone;

      DistanceOutOfBoundingBox( FragmentCenterDistanceOutOfHomeZone, FragmentCenter, BoxHigh, BoxLow );

      // If there is a global recook, we need to look at migration hold back stats
      // This function is called after the migration code has had a chance
      if( AssignmentViolationFlag )
        {
        // Do the migration candidate check with the method used in the actual migration function
        XYZ cog;
        XYZ tempCog = GetFragmentPosition( fragId );
        NearestImageInPeriodicVolume( CenterOfBoundingBox , tempCog, cog );

        ORBNode * leaf = mRecBisTree->Node( cog.mX, cog.mY, cog.mZ );
        assert( leaf );

        if( leaf->rank != Platform::Topology::GetAddressSpaceId() )
          {
          MigrationCandidateFlags[ fragId ] = 1;
          MigrationCandidateIndex[ fragId ] = i;  // record the shortlist bin that this frag is in
          }
        }

      // Scan the atoms in frag for max extent, image them and look for violators
      XYZ NearHomePos[ 10 ];
      double ExtentFromFragmentCenter  = 0.0;
      for( int offset=0; offset < numberOfSites; offset++ )
        {
        XYZ & Pos            = GetPosition( fragId, offset );

        NearestImageInPeriodicVolume( HomeCenter , Pos, NearHomePos[ offset ]  );

        // Max extent from average position (what the code is using for COG)
        double eap = FragmentCenter.Distance( NearHomePos[ offset ] );
        if( eap > ExtentFromFragmentCenter )
          {
          ExtentFromFragmentCenter = eap;
          }

        double AtomDistanceOutOfHomeZone;
        DistanceOutOfBoundingBox( AtomDistanceOutOfHomeZone, NearHomePos[ offset ], BoxHigh, BoxLow );

        // Print out site violators beyond a limit
        if( AtomDistanceOutOfHomeZone > WARN_ATOM_DOUT )
          {
          printf("E_AOUTHZ %d %d %d %d %f\n", SimTick, fragId, offset, i, AtomDistanceOutOfHomeZone );
          }

        // Add values to histograms
        if( AtomDistanceOutOfHomeZone > 0.0 )
          {
          int bin = AtomDistanceOutOfHomeZone / HistogramBinSize ;
          if( bin < HistogramBinCount )
            {
            Sums.AtomDistOutHist[ bin ]++;
            }
          else
            {
            Sums.AtomDistOutHist[ HistogramBinCount ]++;
            printf("E_HUGE_ATOM_DIST_OUT %d %d %d %d %f\n", SimTick, fragId, offset, i, AtomDistanceOutOfHomeZone );
            }
          }

        if( AtomDistanceOutOfHomeZone > Maxs.MaxAtomDistanceOut )
          Maxs.MaxAtomDistanceOut = AtomDistanceOutOfHomeZone;

        if( AtomDistanceOutOfHomeZone > LeftBackAtomMaxDistOut[ fragId ] )
          LeftBackAtomMaxDistOut[ fragId ] = AtomDistanceOutOfHomeZone;

        }  // end of loop over sites in frag

      // Add values to histograms
      int bin = ExtentFromFragmentCenter / HistogramBinSize ;
      if( bin < 0 ) bin = 0;
      if( bin < HistogramBinCount )
        {
        Sums.FragExtentHist[ bin ]++;
        }
      else
        {
        Sums.FragExtentHist[ HistogramBinCount ]++;
        printf("E_HUGE_EXTENT %d %d %d %f %f\n",
               SimTick, fragId, i, ExtentFromFragmentCenter, FragmentCenterDistanceOutOfHomeZone );
        }

      if( ExtentFromFragmentCenter > Maxs.MaxFragmentExtent )
        {
        Maxs.MaxFragmentExtent = ExtentFromFragmentCenter;
        }

      if( ExtentFromFragmentCenter > LeftBackMaxExtent[ fragId ] )
        {
        LeftBackMaxExtent[ fragId ] = ExtentFromFragmentCenter;
        }

      // Check if either fragment center method is out of bounding box
      if( ExtentFromFragmentCenter  > WARN_FEXT_DOUT )
        {
        printf("E_FEXT_OUT %d %d %f %f\n",
               SimTick, fragId, ExtentFromFragmentCenter,FragmentCenterDistanceOutOfHomeZone);
        }

      // Add values to histograms
      if( FragmentCenterDistanceOutOfHomeZone > 0.0 )
        {
        bin = FragmentCenterDistanceOutOfHomeZone / HistogramBinSize ;
        if(  bin < HistogramBinCount )
          {
          Sums.FCOGDistOutHist[ bin ]++;
          }
        else
          {
          Sums.FCOGDistOutHist[ HistogramBinCount ]++;
          printf("E_HUGE_FCOG_DIST_OUT %d %d %d %f %f\n",
                 SimTick, fragId, i, FragmentCenterDistanceOutOfHomeZone, ExtentFromFragmentCenter );
          }
        }

      if( FragmentCenterDistanceOutOfHomeZone > Maxs.MaxFCOGDistanceOut )
        {
        Maxs.MaxFCOGDistanceOut =  FragmentCenterDistanceOutOfHomeZone;
        }

      if( FragmentCenterDistanceOutOfHomeZone > LeftBackFCOGMaxDistOut[ fragId ] )
        {
        LeftBackFCOGMaxDistOut[ fragId ] =  FragmentCenterDistanceOutOfHomeZone;
        }

      // Check if either fragment center method is out of bounding box
      if( FragmentCenterDistanceOutOfHomeZone   > WARN_FCOG_DOUT )
        {
        printf("E_FCOG_OUT %d %d %f %f\n",
               SimTick, fragId,
               FragmentCenterDistanceOutOfHomeZone, ExtentFromFragmentCenter );
        }
      }

    // Will need to know on next ts that last ts was a recook ts
    LastSimTickWasAssignmentViolationFlag = AssignmentViolationFlag;
    if( AssignmentViolationFlag )
      NumberOfRecooks++;

    BegLogLine( 0 )
        << SimTick
        << " ViolationBits: " << ViolationBits
        << EndLogLine;
    
    return ( ViolationBits );
#endif
    ///////////////////////////////////////////////////////////////////////////////////////////
    }

#ifdef PK_PHASE5_PROTOTYPE
  inline
  void
  AddForceCore1( int aSiteId,  XYZ& aForce )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );

    FragmentRep & fRep = mSiteIdToFragmentRepMap[ aSiteId ];
    int fragId = fRep.mFragId;
    int offset = fRep.mOffset;
    AddForceCore1( fragId, offset, aForce );
    }

  inline
  void
  AddInCore1Forces( int aSimTick )
    {
    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      
      for( int j = 0; j < nsites; j++ )
        {
        AddForce( fragId, j, GetForceCore1( fragId, j) );
        }
      }
    }

  inline
  void
  AddForceCore1( FragId aFragId, int aOffset, XYZ& aForce )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    // assert( aForce.IsReasonable(__FILE__, __LINE__) );

    #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
    #endif

    BegLogLine(PKFXLOG_ISMIF)
      << "AddForce aFragId=" << aFragId
      << " aOffset=" << aOffset
      << " mIntegratorStateTable=" << mIntegratorStateTable
      << EndLogLine ;

    BegLogLine(PKFXLOG_ISMIF)
      << "AddForce x.x.mForce=" << mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForce
      << EndLogLine ;

#if !defined(BASIC_REDUCTION)
  BegLogLine(PKFXLOG_ISMIF)
        << "Before quadsum mForces[" << aFragId
        << "," << aOffset
        << "] = " << aForce
      << " sum_mForces[] = " << mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ]
      << " sum_mForceLowerBits[] = " << mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ]
      << EndLogLine ;
  
  quadfloat_integrate(
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ].mX,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ].mX,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ].mX,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ].mX,
    aForce.mX
      ) ;
  quadfloat_integrate(
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ].mY,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ].mY,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ].mY,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ].mY,
    aForce.mY
      ) ;
  quadfloat_integrate(
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ].mZ,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ].mZ,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ].mZ,
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ].mZ,
    aForce.mZ
      ) ;
  
  BegLogLine(PKFXLOG_ISMIF)
        << "Before quadsum mForces[" << aFragId
        << "," << aOffset
        << "] = " << aForce
      << " sum_mForces[] = " << mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ]
      << " sum_mForceLowerBits[] = " << mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceLowerBits[ aOffset ]
      << EndLogLine ;
  
#else    
    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForceCore1[ aOffset ] += aForce;
#endif    
    }
#endif

  inline
  void
  AddForce( int aSiteId,  XYZ aForce )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );

    FragmentRep & fRep = mSiteIdToFragmentRepMap[ aSiteId ];
    int fragId = fRep.mFragId;
    int offset = fRep.mOffset;
    AddForce( fragId, offset, aForce );
    }

  inline
  void
  AddForce( FragId aFragId, int aOffset, XYZ aForce )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    // assert( aForce.IsReasonable(__FILE__, __LINE__) );

    #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
    #endif

    BegLogLine(PKFXLOG_ISMIF)
      << "AddForce aFragId=" << aFragId
      << " aOffset=" << aOffset
      << " mIntegratorStateTable=" << mIntegratorStateTable
      << EndLogLine ;

    BegLogLine(PKFXLOG_ISMIF)
      << "AddForce x.x.mForce=" << mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForce
      << EndLogLine ;

    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForce[ aOffset ] += aForce;
    }

  inline
  void
  SetForce( FragId aFragId, int aOffset, XYZ& aForce )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
    #endif

    mIntegratorStateTable[ aFragId ].mIntegratorFragmentState.mForce[ aOffset ] = aForce;
    }

  inline
  void
  SetForce( int aSiteId, XYZ& aForce )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );

//     #ifndef NDEBUG
//       int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
//       assert( aOffset >= 0 && aOffset < nsites );
//     #endif

    FragmentRep & fRep = mSiteIdToFragmentRepMap[ aSiteId ];
    int fragId = fRep.mFragId;
    int offset = fRep.mOffset;

    mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mForce[ offset ] = aForce;
    }

  inline
  int
  GetNumberOfLocalFragments()
    {
    return mIntegratorShortListSize;
    }

  inline
  FragId*
  GetLocalFragmentPtr()
    {
    return (FragId *) mIntegratorShortList;
    }

  inline
  FragId
  GetNthFragmentId( int n )
    {
    assert( n >= 0 && n < mIntegratorShortListSize );
    return mIntegratorShortList[ n ];
    }

  inline
  XYZ&
  GetPosition( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ & rc = rec.mIntegratorFragmentState.mPosition[ aOffset ];

    assert( rc.IsReasonable() );

    return( rc );
    }

  inline
  XYZ*
  GetPositionPtr( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ* rc = & ( rec.mIntegratorFragmentState.mPosition[ aOffset ] );

    assert( rc->IsReasonable() );

    return( rc );
    }

  inline
  void
  ZeroForces()
    {
    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];

      ZeroForce( fragId );
      }
    }

#ifdef PK_PHASE5_PROTOTYPE
  inline
  void
  ZeroForcesCore1()
    {
    for( int i=0; i<mIntegratorShortListSize; i++ )
      {
      FragId fragId = mIntegratorShortList[ i ];

      ZeroForceCore1( fragId );
      }
    }
#endif

  inline
  XYZ
  GetForce( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ & rc = rec.mIntegratorFragmentState.mForce[ aOffset ];

    assert( rc.IsReasonable() );

    return( rc );
    }


#ifdef PK_PHASE5_PROTOTYPE
  inline
  XYZ&
  GetForceCore1( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ & rc = rec.mIntegratorFragmentState.mForceCore1[ aOffset ];

    assert( rc.IsReasonable() );

    return( rc );
    }
#endif

  inline
  void
  SetPosition( FragId aFragId, int aOffset, XYZ& aPos  )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    assert( aPos.IsReasonable() );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    rec.mIntegratorFragmentState.mPosition[ aOffset ] = aPos;
    }

  inline
  void
  AddToPositionTracker( FragId aFragId, int aOffset, XYZ aPos  )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    assert( aPos.IsReasonable() );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    rec.mIntegratorFragmentState.mPositionImageTracker[ aOffset ] += aPos;
    }

  inline
  XYZ&
  GetPositionTracker( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ & rc = rec.mIntegratorFragmentState.mPositionImageTracker[ aOffset ];

    return( rc );
    }

  inline
  void
  SetPositionTracker( FragId aFragId, int aOffset, XYZ& aPos )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    rec.mIntegratorFragmentState.mPositionImageTracker[ aOffset ] = aPos;
    }

  inline
  void
  SetVelocity( FragId aFragId, int aOffset, XYZ& aVel  )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    assert( aVel.IsReasonable() );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    rec.mIntegratorFragmentState.mVelocity[ aOffset ] = aVel;
    }

  XYZ&
  GetFragmentVerletMonitoringPosition( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ & rc = rec.mIntegratorFragmentState.mFragmentVerletMonitoringPosition[ aOffset ];

    assert( rc.IsReasonable() );

    return( rc );
    }

  XYZ&
  GetFragmentAssignmentMonitoringPosition( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ & rc = rec.mIntegratorFragmentState.mFragmentAssignmentMonitoringPosition[ aOffset ];

    assert( rc.IsReasonable() );

    return( rc );
    }

  inline
  void
  SetFragmentVerletMonitoringPosition( FragId aFragId, int aOffset, XYZ& aPos  )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    assert( aPos.IsReasonable() );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    rec.mIntegratorFragmentState.mFragmentVerletMonitoringPosition[ aOffset ] = aPos;
    }

  inline
  void
  SetFragmentAssignmentMonitoringPosition( FragId aFragId, int aOffset, XYZ& aPos  )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    assert( aPos.IsReasonable() );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    rec.mIntegratorFragmentState.mFragmentAssignmentMonitoringPosition[ aOffset ] = aPos;
    }

  inline
  XYZ&
  GetVelocity( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    assert( aOffset >= 0 && aOffset < MAX_SITES_IN_FRAGMENT );
    XYZ & rc = rec.mIntegratorFragmentState.mVelocity[ aOffset ];

    assert( rc.IsReasonable() );

    return( rc );
    }

  inline
  VoxelIndex
  GetVoxelIndex( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    VoxelIndex rc = rec.mIntegratorFragmentState.mVoxelIndex;

    return ( rc );
    }

  inline
  void
  SetVoxelIndex( FragId aFragId, VoxelIndex aVoxelIndex )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );

    IntegratorStateTableRecord & rec = mIntegratorStateTable[ aFragId ];

    rec.mIntegratorFragmentState.mVoxelIndex = aVoxelIndex;
    }
  };

#endif
