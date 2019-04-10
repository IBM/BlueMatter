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
 * Project:         BlueMatter
 *
 * Module:          DynamicVariableManager
 *
 * Purpose:         Manage loop communicated variables
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         001001 BGF Created.
 *
 ***************************************************************************/

#ifndef __DYNAMIC_VARIABLE_MANAGER_PHASE5_HPP__
#define __DYNAMIC_VARIABLE_MANAGER_PHASE5_HPP__

#ifndef PKFXLOG_CHECK_SITE_DISP
#define PKFXLOG_CHECK_SITE_DISP ( 0 )
#endif

#ifndef PKFXLOG_ROLLING_REG_DEBUG
#define PKFXLOG_ROLLING_REG_DEBUG ( 0 )
#endif

#ifndef PKFXLOG_CHASEHANG
#define PKFXLOG_CHASEHANG ( 0 )
#endif

#ifndef PKFXLOG_EXPDYNVAR
#define PKFXLOG_EXPDYNVAR ( 0 )
#endif

#ifndef PKFXLOG_SETUP
#define PKFXLOG_SETUP ( 0 )
#endif

#ifndef PKFXLOG_LOCALSITEMGR
#define PKFXLOG_LOCALSITEMGR ( 0 )
#endif

#ifndef PKFXLOG_CHECKMASK
#define PKFXLOG_CHECKMASK  ( 0 )
#endif

#ifndef PKFXLOG_GLOBALIZE_POSITIONS
#define PKFXLOG_GLOBALIZE_POSITIONS ( 0 )
#endif

#ifndef PKFXLOG_GLOBALIZEPOSITIONS
#define PKFXLOG_GLOBALIZEPOSITIONS  ( 0 )
#endif

#ifndef PKFXLOG_LOCALFRAGMENTS
#define PKFXLOG_LOCALFRAGMENTS ( 0 )
#endif

#ifndef PKFXLOG_REDUCE_FRAGMENT_TIME
#define PKFXLOG_REDUCE_FRAGMENT_TIME ( 0 )
#endif

#ifndef PKFXLOG_REDISTRIBUTION
#define PKFXLOG_REDISTRIBUTION ( 0 )
#endif

#ifndef PKFXLOG_DUMP_LOCAL_VELOCITY
#define PKFXLOG_DUMP_LOCAL_VELOCITY ( 0 )
#endif

#ifndef PKFXLOG_DUMP_VELOCITY
#define PKFXLOG_DUMP_VELOCITY ( 0 )
#endif

#ifndef PKFXLOG_DUMP_FORCE
#define PKFXLOG_DUMP_FORCE ( 0 )
#endif

#ifndef PKFXLOG_DUMP_POSITIONS
#define PKFXLOG_DUMP_POSITIONS ( 0 )
#endif

#ifndef PKFXLOG_FORCEREDUCTION
#define PKFXLOG_FORCEREDUCTION ( 0 )
#endif

#ifndef PKFXLOG_GEOMETRIC_EXTENTS
#define PKFXLOG_GEOMETRIC_EXTENTS ( 0 )
#endif

#ifndef PKFXLOG_PARTITIONLIST
#define PKFXLOG_PARTITIONLIST     ( 0 )
#endif

#ifndef PKFXLOG_SPATIALDECOMP
#define PKFXLOG_SPATIALDECOMP     ( 0 )
#endif

#ifndef PKFXLOG_ALLOCATEBLOCK
#define PKFXLOG_ALLOCATEBLOCK     ( 0 )
#endif

#ifndef PKFXLOG_COLLECTIVE_FP_REDUCE
#define PKFXLOG_COLLECTIVE_FP_REDUCE ( 0 )
#endif

#ifndef PKFXLOG_IMAGE_RANGE
#define PKFXLOG_IMAGE_RANGE ( 0 )
#endif

#ifndef PKFXLOG_LOADBALANCE
#define PKFXLOG_LOADBALANCE ( 0 )
#endif

#ifndef PKFXLOG_SPARSE_SKIN_NODE_COUNT
#define PKFXLOG_SPARSE_SKIN_NODE_COUNT ( 0 )
#endif

extern unsigned SimTick;

#include <BlueMatter/ExternalDatagram.hpp>
#include <pk/platform.hpp>
#include <BlueMatter/MSD_IF.hpp>
#include <pk/pktrace.hpp>   // Needed by the pk tracing instrumentation
#include <BlueMatter/Tag.hpp>

#include <BlueMatter/DirectMDVM.hpp>

#if ( MSDDEF_NOSE_HOOVER && MSDDEF_DoPressureControl )
#include <BlueMatter/DirectPistonMDVM.hpp>
#endif

#include <BlueMatter/orbnode.hpp>
#include <BlueMatter/IntegratorStateManagerIF.hpp>
#include <BlueMatter/InteractionStateManagerIF.hpp>
#include <BlueMatter/SparseSkinManagerIF.hpp>

#define NOT_NEAREST_NEIGBOR -9999

struct FragmentInRangePair
  {
  XYZ            mFragmentCenter;
  double         mExtent;
  FragId         mFragId;

  int            mRankOrdinal;

  unsigned short mVoxelOrdinal;
  unsigned short mMoleculeId;
  };

typedef Platform::Clock::TimeValue TimeValue;
typedef unsigned int ExclusionMask_T;

struct ORBVoxelInput
  {
  XYZ    mPoint;
  double mWeight;
  };

enum
  {
  NODES_COULD_NOT_MIDPOINT = 0,
  NODES_ON_BOUNDARY = 1,
  NODES_WITHIN_CUTOFF_COULD_MIDPOINT = 2
  };

#define  VOXEL_COULD_NOT_MIDPOINT             0
#define  VOXEL_ON_BOUNDARY                    1
#define  VOXEL_WITHIN_CUTOFF_COULD_MIDPOINT   2

#define PATIENCE_COUNT                        5

#define ROUND32( a ) (( ((unsigned)a)+31 ) & ~0x1f)
#define ROUND8( a ) (( ((unsigned)a)+7 ) & ~0x7)

static inline double NearestInteger(const double x);

 struct tVerletSiteInfo
{
    int    fragid;   // fragment id , but without leaving a hole in the structure
    int    siteind;  // site offset in fragment
    int    siteused; // this is for p3me caching, set in AssignCharge, checked in InterpolateFields
    double charge;   // charge of the site

  bool operator<( const tVerletSiteInfo& atVerletSiteInfo) const
  {
    if( fragid < atVerletSiteInfo.fragid )
      {
	return 1;
      }
    else if ( fragid == atVerletSiteInfo.fragid )
      {
	if( siteind < atVerletSiteInfo.siteind )
	  {
	    return 1;	   
	  }
	
	return 0;
      }	

    return 0;      
  }
};

static int qsortComparetVerletSiteInfo(const void * aVerletSiteInfo0, const void * aVerletSiteInfo1)
{
	tVerletSiteInfo* siteInfo0 = (tVerletSiteInfo*) aVerletSiteInfo0 ; 
	tVerletSiteInfo* siteInfo1 = (tVerletSiteInfo*) aVerletSiteInfo1 ; 
	int qa = siteInfo1->fragid - siteInfo0->fragid ;
	int qb = siteInfo1->siteind - siteInfo0->siteind ;
	return ( qa != 0 ) ? qa : qb ;
}

#include <BlueMatter/KSpaceCommManagerIF.hpp>
typedef KspaceCommManagerIF KSpaceCommMIF;

inline void memcpy_xyz( XYZ* aTrg, XYZ* aSrc, int aCount )
  {
  for( int i=0; i < aCount; i++ )
    {
    aTrg[ i ] = aSrc[ i ];
    }
  }

#include <spi/PktThrobberIF.hpp>
#include <BlueMatter/MsgThrobberIF.hpp>

static char* CommDriverNames[] =
{ "RealSpacePktCommDriver",
  "V4NeighborhoodPktCommDriver",
  "BondedMsgCommDriver",
  "V4NeighborhoodMsgCommDriver"
};

enum 
{
    REAL_SPACE_PKT_COMM_DRIVER,
    V4_NEIGHBORHOOD_PKT_COMM_DRIVER,
    BONDED_MSG_COMM_DRIVER,
    V4_NEIGHBORHOOD_MSG_COMM_DRIVER
};

#include <BlueMatter/NeighborhoodCommDriver.hpp>

static inline void write_to_file( int sock, char * buff, int len )
  {
  for( int total = 0; total < len; )
    {
    int write_rc = write(   sock,
                            (((char *) buff) + total ),
                            len - total );
    if( write_rc < 0 )
      {
      if( errno == EAGAIN )
        continue;
      else
        PLATFORM_ABORT( " FAILED TO WRITE SCO3 FILE " );
      }

    total += write_rc;
    }
  }

class TDynamicVariableManagerTracer
  {
public:
  static TraceClient mGuardZoneViolationSignalReduceStart;
  static TraceClient mGuardZoneViolationSignalReduceFinis;

  static TraceClient mFragmentMigrationStart;
  static TraceClient mFragmentMigrationFinis;

  static TraceClient mReduceForcesStart ;
  static TraceClient mReduceForcesFinis ;

  static TraceClient mGlobalizePositionsStart ;
  static TraceClient mGlobalizePositionsFinis ;

  static TraceClient mFloatingPointAllReduceStart;
  static TraceClient mFloatingPointAllReduceFinis;

  } ;

static const float dk_ImageTable[5] = { 2.0, 1.0, 0.0, -1.0, -2.0 } ;

template<int SITE_COUNT, int FRAGMENT_COUNT, int RESPA_LEVEL_COUNT>
class TDynamicVariableManager : public MDVM_BaseIF, public TDynamicVariableManagerTracer
  {
  public:

  SparseSkinManagerIF            mSparseSkinManagerIF MEMORY_ALIGN(5);

  FragmentRep*                   mSiteIdToFragmentRepMap MEMORY_ALIGN(5);

  double                         mExtents[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);
  XYZ                            mFragCenters[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);

  int
  GetVoxelMeshDimensions( int     aTargetVoxelsPerProc,
                          double  MinTargetVoxelDim,
                          double  sX, double sY, double sZ,
                          int     pX, int    pY, int    pZ,
                          unsigned int &mX,
                          unsigned int &mY,
                          unsigned int &mZ )
    {
    int    pVol = pX * pY * pZ;
    double sVol = sX * sY * sZ;

    // The first step should be to realine the simulation space in the processor space.
    // For now, we'll just map SimX to ProcX, etc.

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "GetVoxelMeshDimensions():"
      << " TargetVoxelsPerProc " <<  aTargetVoxelsPerProc
      << " MinTargetVoxDim "     << MinTargetVoxelDim
      << " SimXYZ "   << sX << " " << sY << " " << sZ
      << " ProcXYZ "  << pX << " " << pY << " " << pZ
      << EndLogLine;

    double s2pX = sX / pX;
    double s2pY = sY / pY;
    double s2pZ = sZ / pZ;
    double NominalVoxelSize = 0.0;

    // Figure out which demension will guide tha mapping

    if( s2pX < s2pY && s2pX < s2pZ )
      {
      NominalVoxelSize = s2pX;
      }
    else if( s2pY < s2pX && s2pY < s2pZ )  // Y is min aspect ratio
      {
      NominalVoxelSize = s2pY;
      }
    else                                   // Z is min aspect ratio
      {
      NominalVoxelSize = s2pZ;
      }

    double NomVoxMeshX = sX / NominalVoxelSize;
    double NomVoxMeshY = sY / NominalVoxelSize;
    double NomVoxMeshZ = sZ / NominalVoxelSize;

    // Round to nearest even

    int VoxMeshX = NomVoxMeshX + 1;
    int VoxMeshY = NomVoxMeshY + 1;
    int VoxMeshZ = NomVoxMeshZ + 1;

    VoxMeshX = ( VoxMeshX / 2 ) * 2;
    VoxMeshY = ( VoxMeshY / 2 ) * 2;
    VoxMeshZ = ( VoxMeshZ / 2 ) * 2;

    int TotalVoxels = VoxMeshX * VoxMeshY * VoxMeshZ;
    int TotalProcs = pX * pY * pZ;
    double TotalVoxelsPerProc =  (1.0 * TotalVoxels) / TotalProcs;

    double factor;
    for( factor = 1.0; ; factor += 0.1 )
      {
      NomVoxMeshX = sX / NominalVoxelSize;
      NomVoxMeshY = sY / NominalVoxelSize;
      NomVoxMeshZ = sZ / NominalVoxelSize;

      NomVoxMeshX *= factor ;
      NomVoxMeshY *= factor ;
      NomVoxMeshZ *= factor ;

      // Round to nearest even

      VoxMeshX = NomVoxMeshX + 1;
      VoxMeshY = NomVoxMeshY + 1;
      VoxMeshZ = NomVoxMeshZ + 1;

      VoxMeshX = ( VoxMeshX / 2 ) * 2;
      VoxMeshY = ( VoxMeshY / 2 ) * 2;
      VoxMeshZ = ( VoxMeshZ / 2 ) * 2;

      TotalVoxels = VoxMeshX * VoxMeshY * VoxMeshZ;
      TotalProcs = pX * pY * pZ;
      TotalVoxelsPerProc =  (1.0 * TotalVoxels) / TotalProcs;

      if( (sX / VoxMeshX) < MinTargetVoxelDim || (sY / VoxMeshY) < MinTargetVoxelDim || (sZ / VoxMeshZ) < MinTargetVoxelDim )
        {
        BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
          << "GetVoxelMeshDimensions():"
          << " Stopped refining - Reached min voxel dim target of "
          <<  MinTargetVoxelDim
          << " VoxDimX " << sX/VoxMeshX
          << " VoxDimY " << sY/VoxMeshY
          << " VoxDimZ " << sZ/VoxMeshZ
          << EndLogLine;
        break;
        }

      if( TotalVoxelsPerProc >= aTargetVoxelsPerProc )
        {
        BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
          << "GetVoxelMeshDimensions():"
          << " Stopped refining - Reached target voxels per proc of "
          << aTargetVoxelsPerProc
          << " with TotalVoxelsPerProc of "
          << TotalVoxelsPerProc
          << EndLogLine;

        break;
        }
      }

    mX = VoxMeshX;
    mY = VoxMeshY;
    mZ = VoxMeshZ;

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "GetVoxelMeshDimensions():"
      << " Selected Voxel Mesh of "
      << mX << " "
      << mY << " "
      << mZ << " "
      << EndLogLine;

    return(0);
    }

  inline
  FragmentRep &
  GetFragmentRepForSiteId( SiteId aSiteId )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    FragmentRep & rc = mSiteIdToFragmentRepMap[ aSiteId ];
    return ( rc );
    }

  inline
  FragId &
  GetFragIdForSiteId( SiteId aSiteId )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    FragId & rc = mSiteIdToFragmentRepMap[ aSiteId ].mFragId;
    return ( rc );
    }

  IntegratorStateManagerIF   mIntegratorStateManagerIF MEMORY_ALIGN(5);
  InteractionStateManagerIF  mInteractionStateManagerIF MEMORY_ALIGN(5);

  int* mNodesForBonded;
  int  mNodesForBondedCount;   

  InteractionStateManagerIF       mBondedInteractionStateManagerIF MEMORY_ALIGN(5);
  KspaceInteractionStateManagerIF mKspaceInteractionStateManagerIF MEMORY_ALIGN(5);

  // These are the whole system values
  XYZ      mVirial[ RESPA_LEVEL_COUNT ] MEMORY_ALIGN(5);
  XYZ      mInternalPressure ;
  double   mExternalPressure ;
  double   mVolumeMass ;
  XYZ      mVolume ;
  XYZ      mVolumeImpulse ;
  XYZ      mOldVolume ;
  XYZ      mOldVolumeVelocity ;
  XYZ      mVolumeVelocity ;
  XYZ      mVolumeRatio ;
  XYZ      mVolumeVelocityRatio ;
  double   mCenterOfMassKineticEnergy ;
  XYZ      mBirthBoxAbsoluteRatio ;
  XYZ      mDynamicBoxDimensionVector ;
  XYZ      mDynamicBoxInverseDimensionVector ; // currentinversesize
  XYZ      mCenterOfBoundingBox;

  double   mPotentialEnergy;

#ifndef NOT_TIME_REVERSIBLE
  double   mExpVelVolRatio ; // recycled for velocity/volume updates
#endif

  int            mNumberOfRealSpaceCalls ;
  int            mSampleFragmentTime ; 

  int mMaxSitesInFragment;
  double mMaxExtent;

  int mSizeOfSDBuffers;
  char* mSendBufferSD MEMORY_ALIGN( 5 );
  char* mRecvBufferSD MEMORY_ALIGN( 5 );

  int* mSDSendCounts;
  int* mSDSendOffsets;
  int* mSDRecvCounts;
  int* mSDRecvOffsets;
  int* mSDSendCountsOffsets;
  int* mSDRecvCountsOffsets;
  int* mSDSendCountsCounts;
  int* mSDRecvCountsCounts;

  /**
   * Map FragmentId to AddressSpaceId. This is developed by the master node and distributed to the others
   * All nodes then use this list to develop the mask
   **/
  // This is needed for phase 1.5
  // Will leave it here in case we need to merge
  TimeValue*   mTimeToRunNsqInnerLoop MEMORY_ALIGN(5);

  int    mAddressSpaceId ;

  double mCurrentVerletGuardZone;
  double mCurrentAssignmentGuardZone;

  double mLocalDim_x;
  double mLocalDim_y;
  double mLocalDim_z;

  int mMyP_x, mMyP_y, mMyP_z;
  int mProcs_x, mProcs_y, mProcs_z;

  int*                  mRemovedFrags;
  int                   mRemovedFragsAllocated;

  FragmentInRangePair*  mLocalFragments MEMORY_ALIGN(5);

  ORBNode*              mRecBisTree;
 
  int*                  mNodeIdToNodeOrdinalMap MEMORY_ALIGN( 5 );

  unsigned int mVoxDimX;
  unsigned int mVoxDimY;
  unsigned int mVoxDimZ;

  double mRadiusOfSphere;

  NeighborhoodCommDriver< PktThrobberIF, FRAGMENT_COUNT, REAL_SPACE_PKT_COMM_DRIVER > mNeighborhoodCommDriver MEMORY_ALIGN( 5 );

  NeighborhoodCommDriver< MsgThrobberIF, FRAGMENT_COUNT, BONDED_MSG_COMM_DRIVER > mNeighborhoodCommBondedDriver MEMORY_ALIGN( 5 );

#if MSDDEF_DoPressureControl
  XYZ mCenterOfMassForce[ RESPA_LEVEL_COUNT ][ FRAGMENT_COUNT ] MEMORY_ALIGN( 5 );

#if MSDDEF_NOSE_HOOVER
  double mPistonThermostatPosition[ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
  double mPistonThermostatVelocity[ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
  double mPistonThermostatMass[ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
  double mPistonThermostatForce[ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
  double mDKTPiston[ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
  double mPistonKineticEnergy ;
#endif
#endif

  class FragmentRecord
    {
public:
    XYZ mCenterOfMassPosition ;
    XYZ mCenterOfMassVelocity ;
    double mCenterOfMassMass ;
    double mCenterOfMassHalfInverseMass ;
    double mCenterOfMassKineticEnergy ;

#if MSDDEF_NOSE_HOOVER
    double mThermostatPosition[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mThermostatVelocity[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mThermostatMass[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mThermostatForce[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mFragmentKineticEnergy ;
#endif

    void ZeroState()
      {
      mCenterOfMassPosition.Zero();
      mCenterOfMassVelocity.Zero();
      mCenterOfMassMass = 0.0;
      mCenterOfMassHalfInverseMass = 0.0;
      mCenterOfMassKineticEnergy = 0.0;


#if MSDDEF_NOSE_HOOVER
      mFragmentKineticEnergy = 0.0;

      for(int i=0; i < NHC::NUMBER_OF_THERMOSTATS; i++ )
        {
        for( int j=0; j < NUMBER_OF_SITES_PER_THERMOSTAT; j++ )
          {
          mThermostatPosition[ i ][ j ] = 0.0;
          mThermostatVelocity[ i ][ j ] = 0.0;
          mThermostatMass[ i ][ j ] = 0.0;
          mThermostatForce[ i ][ j ] = 0.0;
          }
        }
#endif
      }
    } MEMORY_ALIGN(5);

  FragmentRecord* mLocalFragmentTable MEMORY_ALIGN(5);

  int             mLocalFragmentIndexNext;

public:

  // THESE ARE HERE TEMPORARILY TO ONLY HOLD THE INITIAL STATE
   XYZ* mPosit        MEMORY_ALIGN( 5 );
   XYZ* mPositTracker MEMORY_ALIGN( 5 );
   XYZ* mVelocity     MEMORY_ALIGN( 5 );

#ifdef ROLLING_POINT_REGRESSION
  XYZ  mForce    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
  XYZ  mReductionTemp    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
#endif

  XYZ  mImageVectorTable[ 125 ] MEMORY_ALIGN( 5 );

  unsigned int mImageMap[ 125 ] MEMORY_ALIGN( 5 ) ;

  int*  mNearestNeighbors MEMORY_ALIGN( 5 );
  int  mNumberOfNearestNeighbors;
  int  mPreNumberOfNearestNeighbors;

  int* mRankToNearestNeighborMap;

  void
  ZeroPotentialEnergy()
    {
    mPotentialEnergy = 0.0;
    }

  void
  AddToPotentialEnergy( int aSimTick, double aEnergy )
    {
    mPotentialEnergy += aEnergy;
    }

  double
  GetPotentialEnergy()
    {
    return mPotentialEnergy;
    }

  void
  SendVelocityToP1_5( int aSimTick )
    {
#ifdef ROLLING_POINT_REGRESSION
    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Entering SendVelocityP1_5"
      << EndLogLine;

    // 1. Reduce all the positions and forces to node 0
    for( int i=0; i < SITE_COUNT; i++ )
      {
      mVelocity[ i ].Zero();
      }

    for( int i=0; i < mIntegratorStateManagerIF.GetNumberOfLocalFragments(); i++ )
      {
      FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( i );
      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < SiteCount; offset++ )
        {
        SiteId sId = FirstSiteId + offset;
        mVelocity[ sId ] = mIntegratorStateManagerIF.GetVelocity( fragId, offset );
        }
      }

    int ReduceSize = SITE_COUNT * ( sizeof(XYZ) / sizeof(double) );

    Platform::Collective::FP_Reduce_Sum( (double *) & (mVelocity[ 0 ]), 
					 (double *) & (mReductionTemp[ 0 ]),
					 ReduceSize );
    
    memcpy( mVelocity, mReductionTemp, sizeof(XYZ) * SITE_COUNT );

    // 2. Send all the positions, velocities and forces to node 0
    if( mAddressSpaceId == 0 )
      {
      assert( sockfd >= 0 );

      write_to_file( sockfd, (char *) mVelocity, sizeof( XYZ ) * SITE_COUNT );
      }

    Platform::Control::Barrier();

    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Leaving SendVelocityP1_5"
      << EndLogLine;
#endif
    }

  void
  SendPosVelToP1_5( int aSimTick, int aConnect=0 )
    {
#ifdef ROLLING_POINT_REGRESSION
    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Entering SendPosVelP1_5"
      << EndLogLine;

    if( aConnect && ( mAddressSpaceId == 0 ) )
      {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	Platform::Comm::mRemoteAddr.sin_family       =  AF_INET;
	Platform::Comm::mRemoteAddr.sin_port         =  Platform::Comm::mRemotePort;
	Platform::Comm::mRemoteAddr.sin_addr.s_addr  =  Platform::Comm::mRemoteHostIP;
	printf( "Platform::Comm::mRemoteAddr.sin_addr.s_addr=%08X Platform::Comm::mRemoteAddr.sin_port=%d \n", Platform::Comm::mRemoteAddr.sin_addr.s_addr, Platform::Comm::mRemoteAddr.sin_port );
	
	while( 1 )
	  {
	    int rc = connect( sockfd, (struct sockaddr *) &Platform::Comm::mRemoteAddr, sizeof(struct sockaddr_in));
	    
	    if (rc < 0)
	      {
		if( errno == ECONNREFUSED || errno == ECONNABORTED || errno == ETIMEDOUT )
		  {
		    sleep( 1 );
		    continue;
		  }
		PLATFORM_ABORT("connect");
	      }
	    else
	      break;
	  }
	
	assert( sockfd >= 0 );
	
	BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
	  << "Connection established: about to write pos and vel"
	  << EndLogLine;	    
      }
    
    // 1. Reduce all the positions and forces to node 0
    for( int i=0; i < SITE_COUNT; i++ )
      {
      mPosit[ i ].Zero();
      mVelocity[ i ].Zero();
      }

    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << "mIntegratorStateManagerIF.GetNumberOfLocalFragments(): "
      << mIntegratorStateManagerIF.GetNumberOfLocalFragments()
      << EndLogLine;

    for( int i=0; i < mIntegratorStateManagerIF.GetNumberOfLocalFragments(); i++ )
      {
      FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( i );
      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < SiteCount; offset++ )
        {
        SiteId sId = FirstSiteId + offset;
        mPosit[ sId ] = mIntegratorStateManagerIF.GetPosition( fragId, offset );
        mVelocity[ sId ] = mIntegratorStateManagerIF.GetVelocity( fragId, offset );
	BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
	  << "From Integrator State: "
	  << " mPosit[ " << sId << " ]: " << mPosit[ sId ]
	  << " mVelocity[ " << sId << " ]: " << mVelocity[ sId ]
	  << EndLogLine;
        }
      }

    int ReduceSize = SITE_COUNT * ( sizeof(XYZ) / sizeof(double) );

    Platform::Collective::FP_Reduce_Sum( (double *) &(mPosit[0]), 
					 (double *) &(mReductionTemp[0]), 
					 ReduceSize );

    memcpy( mPosit, mReductionTemp, sizeof(XYZ) * SITE_COUNT );

    Platform::Collective::FP_Reduce_Sum( (double *) & (mVelocity[ 0 ]), 
					 (double *) & (mReductionTemp[ 0 ]),
					 ReduceSize );
    
    memcpy( mVelocity, mReductionTemp, sizeof(XYZ) * SITE_COUNT );

    // 2. Send all the positions, velocities and forces to node 0
    if( mAddressSpaceId == 0 )
      {
      assert( sockfd >= 0 );
      
      for( int i = 0; i < SITE_COUNT; i++ )
	{
	  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
	    << aSimTick 
	    << " P5 mPosit[ " << i << "]: " << mPosit[ i ]
	    << " P5 mVelocity[ " << i << "]: " << mVelocity[ i ]
	    << EndLogLine;	  
	}

      // Are the positions, velocities and force correct?
      write_to_file( sockfd, (char *) mPosit, sizeof( XYZ ) * SITE_COUNT );

      write_to_file( sockfd, (char *) mVelocity, sizeof( XYZ ) * SITE_COUNT );
      }

    Platform::Control::Barrier();

    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Leaving SendPosVelP1_5"
      << EndLogLine;
#endif
    }

  void
  SendForceP1_5( int aSimTick )
    {
#ifdef ROLLING_POINT_REGRESSION
    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Entering SendForceP1_5"
      << EndLogLine;

    // 1. Reduce all the positions and forces to node 0
    for( int i=0; i<SITE_COUNT; i++ )
      {
      mForce[ i ].Zero();
      }

    for( int i=0; i < mIntegratorStateManagerIF.GetNumberOfLocalFragments(); i++ )
      {
      FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( i );
      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < SiteCount; offset++ )
        {
        SiteId sId = FirstSiteId + offset;
        mForce[ sId ] = mIntegratorStateManagerIF.GetForce( fragId, offset );
        }
      }

    int ReduceSize = SITE_COUNT * (sizeof(XYZ)/sizeof(double));

    Platform::Collective::FP_Reduce_Sum( (double *) & (mForce[ 0 ]), 
					 (double *) & (mReductionTemp[ 0 ]), 
					 ReduceSize );
    
    memcpy( mForce, mReductionTemp, sizeof(XYZ) * SITE_COUNT );

    // 2. Send all the positions, velocities and forces to node 0
    if( mAddressSpaceId == 0 )
      {
      assert( sockfd >= 0 );

      // Are the positions, velocities and force correct?
      write_to_file( sockfd, (char *) mForce, sizeof( XYZ ) * SITE_COUNT );
      }

    Platform::Control::Barrier();

    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Leaving SendForceP1_5"
      << EndLogLine;
#endif
    }

  double GetCurrentVerletGuardZone()
    {
    return mCurrentVerletGuardZone;
    }

  double GetCurrentAssignmentGuardZone()
    {
    return mCurrentAssignmentGuardZone;
    }

  void SetCurrentVerletGuardZone( double aZone )
    {
    mCurrentVerletGuardZone = aZone;
    return;
    }

  void
  DumpNoseHooverState(int aTimeStep)
    {
#if MSDDEF_NOSE_HOOVER_DEBUG
    printf("*******************************************************************************\n");
    printf("NoseHooverDump for ts=%d\n", aTimeStep);


    for(int i=0; i < FRAGMENT_COUNT; i++)
      {
      for( int j=0; j < NUMBER_OF_SITES_PER_THERMOSTAT; j++)
        {
        printf("%d.%d.mThermostatPosition[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 0, j, mLocalFragmentTable[i].mThermostatPosition[ 0 ][ j ]);
        printf("%d.%d.mThermostatPosition[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 1, j, mLocalFragmentTable[i].mThermostatPosition[ 1 ][ j ]);

        printf("%d.%d.mThermostatVelocity[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 0, j, mLocalFragmentTable[i].mThermostatVelocity[ 0 ][ j ]);
        printf("%d.%d.mThermostatVelocity[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 1, j, mLocalFragmentTable[i].mThermostatVelocity[ 1 ][ j ]);

        printf("%d.%d.mThermostatMass[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 0, j, mLocalFragmentTable[i].mThermostatMass[ 0 ][ j ]);
        printf("%d.%d.mThermostatMass[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 1, j, mLocalFragmentTable[i].mThermostatMass[ 1 ][ j ]);

        printf("%d.%d.mThermostatForce[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 0, j, mLocalFragmentTable[i].mThermostatForce[ 0 ][ j ]);
        printf("%d.%d.mThermostatForce[%d][%d][%d]=%f\n", mAddressSpaceId, aTimeStep, i, 1, j, mLocalFragmentTable[i].mThermostatForce[ 1 ][ j ]);
        }
      }

    for( int j=0; j < NUMBER_OF_SITES_PER_THERMOSTAT; j++)
      {
      printf("%d.%d.mPistonThermostatPosition[%d]=%f\n", mAddressSpaceId, aTimeStep, j, mPistonThermostatPosition[j] );
      printf("%d.%d.mPistonThermostatVelocity[%d]=%f\n", mAddressSpaceId, aTimeStep, j, mPistonThermostatVelocity[j] );
      printf("%d.%d.mPistonThermostatMass[%d]=%f\n", mAddressSpaceId, aTimeStep, j, mPistonThermostatMass[j] );
      printf("%d.%d.mPistonThermostatForce[%d]=%f\n", mAddressSpaceId, aTimeStep, j, mPistonThermostatForce[j] );
      }
    printf("*******************************************************************************\n");

    fflush( stdout );
#endif
    }

inline
void
ZeroVirial()
  {
  for(int i=0; i<RESPA_LEVEL_COUNT; i++)
    {
    mVirial[ i ].Zero();
    }
  }

inline
void
AddVirial( int aRespaLevel, XYZ aVirial )
  {
  mVirial[ aRespaLevel ] += aVirial;
  }

inline
XYZ
GetVirial( int aRespaLevel )
  {
  return mVirial[ aRespaLevel ];
  }

double
GetFragmentExtent( int aFragmentIndex )
  {
  double rc;
  rc = MSD_IF::GetFragmentExtent( aFragmentIndex );
  return( rc );
  }

void
GetDynamicFragmentExtentFragmentCenter( FragId   aFragmentIndex,
                                        double*  aExtent,
                                        XYZ*     aXYZ )
  {
  int NumberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragmentIndex );

  XYZ LowerLeft = mInteractionStateManagerIF.GetPosition( aFragmentIndex, 0 );
  XYZ UpperRight;
  double bX = LowerLeft.mX ;
  double bY = LowerLeft.mY ;
  double bZ = LowerLeft.mZ ;
  double cX = bX ;
  double cY = bY ;
  double cZ = bZ ;

  for( int i=1; i < NumberOfSites; i++ )
    {
    XYZ AtomLoc = mInteractionStateManagerIF.GetPosition( aFragmentIndex, i );
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

  *aXYZ = MidXYZ;

  double e2max = DBL_MIN ; // Cook the 'max distance' so it is never zero, and the sqrt(0) special case is not needed
  for (int qIndexI = 0 ; qIndexI < NumberOfSites; qIndexI += 1 )
    {
    XYZ AtomLoc = mInteractionStateManagerIF.GetPosition( aFragmentIndex, qIndexI ) ;
    double e2 = ( AtomLoc - MidXYZ ).LengthSquared() ;
    e2max = max( e2max, e2 ) ;
    } /* endfor */

  // PERF: Return e2max, then do a vector of sqrts

  double rc = sqrt( e2max );
  *aExtent = rc;
  }

void
GetDynamicFragmentCenter( FragId   aFragmentIndex,
                          XYZ*     aXYZ )
  {
  int NumberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragmentIndex );
  double DistanceSquaredFromTagSite = 0.0;

  XYZ LowerLeft = mInteractionStateManagerIF.GetPosition( aFragmentIndex, 0 );
  XYZ UpperRight;
  double bX = LowerLeft.mX ;
  double bY = LowerLeft.mY ;
  double bZ = LowerLeft.mZ ;
  double cX = bX ;
  double cY = bY ;
  double cZ = bZ ;

  for( int i=1; i < NumberOfSites; i++ )
    {
    XYZ AtomLoc = mInteractionStateManagerIF.GetPosition( aFragmentIndex, i );
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

  *aXYZ = MidXYZ;
  }

void
GetDynamicFragmentExtentFragmentCenterVersion2( FragId   aFragmentIndex,
                                                double*  aExtent,
                                                XYZ*     aXYZ )
  {
  int FirstSiteId   = MSD_IF::GetIrreducibleFragmentFirstSiteId( aFragmentIndex );
  int NumberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragmentIndex );
  double DistanceSquaredFromTagSite = 0.0;

  XYZ LowerLeft = GetPositionFromFullTable( FirstSiteId );
  XYZ UpperRight;
  double bX = LowerLeft.mX ;
  double bY = LowerLeft.mY ;
  double bZ = LowerLeft.mZ ;
  double cX = bX ;
  double cY = bY ;
  double cZ = bZ ;

  for( int i=1; i < NumberOfSites; i++ )
    {
    XYZ AtomLoc = GetPositionFromFullTable( FirstSiteId+i );
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

  *aXYZ = MidXYZ;

  double e2max = DBL_MIN ; // Cook the 'max distance' so it is never zero, and the sqrt(0) special case is not needed
  for (int qIndexI = 0 ; qIndexI < NumberOfSites; qIndexI += 1 )
    {
    XYZ AtomLoc = GetPositionFromFullTable( FirstSiteId+qIndexI );
    double e2 = ( AtomLoc - MidXYZ ).LengthSquared() ;
    e2max = max( e2max, e2 ) ;
    } /* endfor */

  double rc = sqrt( e2max );
  *aExtent = rc;
  }

public:

inline void
ReduceVirial( int aSimTick, int aRespaLevel )
  {
  XYZ tmpVirial[ RESPA_LEVEL_COUNT ];
  int numberOfDoublesInPackage =  3*RESPA_LEVEL_COUNT;

  Platform::Collective::FP_Reduce_Sum( (double *)mVirial, (double *)tmpVirial, numberOfDoublesInPackage );

  memcpy( mVirial, tmpVirial, sizeof(double)*numberOfDoublesInPackage );
  }


inline void
ReduceCenterOfMassKineticEnergy( int aSimTick, int aRespaLevel )
  {
  double tmpKE = 0.0;

  Platform::Collective::FP_Reduce_Sum( (double *) &mCenterOfMassKineticEnergy, (double *) &tmpKE, 1 );

  mCenterOfMassKineticEnergy = tmpKE;
  }

inline void ZeroCenterOfMassForceReg()
  {
#if MSDDEF_DoPressureControl
  for(int i=0; i<FRAGMENT_COUNT; i++)
    for (int rl=0; rl < RESPA_LEVEL_COUNT; rl++)
      mCenterOfMassForce[ rl ][i].Zero();
#endif
  }

inline
void
ReduceForces( int aSimTick, int aRespaLevel )
  {
  BegLogLine( 0 )
    << aSimTick
    << " Entering ReduceForces"
    << EndLogLine;

  mReduceForcesStart.HitOE( PKTRACE_REDUCEFORCES,
                            "ReduceForces",
                            mAddressSpaceId,
                            mReduceForcesStart );

  mIntegratorStateManagerIF.ZeroForces();
  
  mNeighborhoodCommBondedDriver.Reduce( aSimTick );

  mNeighborhoodCommDriver.Reduce( aSimTick );

  mReduceForcesFinis.HitOE(PKTRACE_REDUCEFORCES,
                           "ReduceForces",
                           mAddressSpaceId,
                           mReduceForcesFinis );

  BegLogLine( 0 )
    << aSimTick
    << " Leaving ReduceForces"
    << EndLogLine;
  return;
  }

/// This call is meant to be called once in the setup phase
inline
void
GlobalizePositionsVelocities( int aSimTick, int aRespaLevel )
  {
  Platform::Collective::Broadcast( (char *) mVelocity, 3*SITE_COUNT*sizeof(double), 0 );

  Platform::Collective::Broadcast( (char *) mPosit, 3*SITE_COUNT*sizeof(double), 0 );

#if MSDDEF_NOSE_HOOVER
  for(int i=0; i < FRAGMENT_COUNT; i++)
    {
    Platform::Collective::Broadcast( (char *) mLocalFragmentTable[ i ].mThermostatPosition[0], NUMBER_OF_SITES_PER_THERMOSTAT*sizeof(double), 0 );
    Platform::Collective::Broadcast( (char *) mLocalFragmentTable[ i ].mThermostatPosition[1], NUMBER_OF_SITES_PER_THERMOSTAT*sizeof(double), 0 );

    Platform::Collective::Broadcast( (char *) mLocalFragmentTable[ i ].mThermostatVelocity[0], NUMBER_OF_SITES_PER_THERMOSTAT*sizeof(double), 0 );
    Platform::Collective::Broadcast( (char *) mLocalFragmentTable[ i ].mThermostatVelocity[1], NUMBER_OF_SITES_PER_THERMOSTAT*sizeof(double), 0 );
    }

#if MSDDEF_DoPressureControl
  Platform::Collective::Broadcast( (char *) mPistonThermostatPosition, NUMBER_OF_SITES_PER_THERMOSTAT*sizeof(double), 0 );
  Platform::Collective::Broadcast( (char *) mPistonThermostatVelocity, NUMBER_OF_SITES_PER_THERMOSTAT*sizeof(double), 0 );
#endif

#endif
  return;
  }

inline
int unsigned
IsSiteLocal( SiteId aAbsSiteId )
  {
  assert( aAbsSiteId >= 0 && aAbsSiteId < SITE_COUNT );

  FragmentRep & fRep = mSiteIdToFragmentRepMap[ aAbsSiteId ];

  unsigned int rc = !mIntegratorStateManagerIF.IsInvalid( fRep.mFragId );

  return ( rc );
  }

inline
int
CheckSiteDisplacement()
  {
  XYZ UpperLimitOfBox = 1.5 * mDynamicBoxDimensionVector;
  XYZ LowerLimitOfBox = -0.5 * mDynamicBoxDimensionVector;

  int rc = mIntegratorStateManagerIF.CheckSiteDisplacement( mCurrentVerletGuardZone,
                                                            mCurrentAssignmentGuardZone,
                                                            LowerLimitOfBox,
                                                            UpperLimitOfBox );
  return ( rc );
  }


inline
void
ZeroNonLocalPositions()
  {
  for(FragId i=0; i<FRAGMENT_COUNT; i++)
    {
    if( mIntegratorStateManagerIF.IsFragmentLocal( i ) )
      continue;

    int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( i );
    int firstSiteId   = MSD_IF::GetIrreducibleFragmentFirstSiteId( i );

    for( int j=firstSiteId; j < (firstSiteId+numberOfSites); j++ )
      mPosit[ j ].Zero();
    }
  }

inline
void
ZeroNonLocalVelocities()
  {
  for(FragId i=0; i<FRAGMENT_COUNT; i++)
    {
    if( mIntegratorStateManagerIF.IsFragmentLocal( i ) )
      continue;

    int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( i );
    int firstSiteId   = MSD_IF::GetIrreducibleFragmentFirstSiteId( i );

    for( int j=firstSiteId; j < (firstSiteId+numberOfSites); j++ )
      mVelocity[ j ].Zero();
    }
  }

void
UpdateDisplacementForFragmentVerletListMonitoring()
  {
  mIntegratorStateManagerIF.UpdateDisplacementForFragmentVerletListMonitoring();
  return;
  }

void
UpdateDisplacementForFragmentAssignmentMonitoring()
  {
  mIntegratorStateManagerIF.UpdateDisplacementForFragmentAssignmentMonitoring();
  return;
  }

  inline
  void
  SetPositionTracking()
   {
   int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

   for( int fragmentIndex = 0; fragmentIndex  < NumLocalFrags; fragmentIndex ++ )
     {
     FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( fragmentIndex );

     int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
     SiteId firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );

     for( int offset=0; offset < numberOfSites; offset++ )
       {
       SiteId sid = firstSiteId+offset;
       mIntegratorStateManagerIF.SetPositionTracker( fragId, offset, mPositTracker[ sid ] );
       }
     }
   }

  inline
  void
  GlobalizePositions ( int aSimTick, int aRespaLevel, int aAssignmentViolation )
  {
  BegLogLine( 0 )
    << aSimTick
    << " Entering GlobalizePositions"
    << EndLogLine;

  mGlobalizePositionsStart.HitOE( PKTRACE_GLOBALIZEPOSITIONS,
                                  "GlobalizePositions",
                                  mAddressSpaceId,
                                  mGlobalizePositionsStart );

  mNeighborhoodCommBondedDriver.Broadcast( aSimTick,
					   aAssignmentViolation, 1 );
  
  
  mNeighborhoodCommDriver.Broadcast( aSimTick,
				     aAssignmentViolation );
  
  mGlobalizePositionsFinis.HitOE( PKTRACE_GLOBALIZEPOSITIONS,
				  "GlobalizePositions",
				  mAddressSpaceId,
				  mGlobalizePositionsFinis );
  
  BegLogLine( 0 )
    << aSimTick
    << " Leaving GlobalizePositions"
    << EndLogLine;
  return;
  }

int GetNearestNeighborIndexForRank( unsigned aRank )
  {
  assert( aRank < (mProcs_x * mProcs_y * mProcs_z) );
  return mRankToNearestNeighborMap[ aRank ];
  }

void GetImagedFragCenterVersion2( FragId aFragId, XYZ& aImaged )
  {
  XYZ tempCog;
  double dummy;
  GetDynamicFragmentExtentFragmentCenterVersion2( aFragId,
                                                  &dummy,
                                                  &tempCog );

  NearestImageInPeriodicVolume( mCenterOfBoundingBox , tempCog, aImaged );
  }

void GetImagedFragCenter( FragId aFragId, XYZ& aImaged )
  {
  XYZ tempCog;
  double dummy;
  GetDynamicFragmentExtentFragmentCenter( aFragId,
                                          &dummy,
                                          &tempCog );

  NearestImageInPeriodicVolume( mCenterOfBoundingBox , tempCog, aImaged );
  }

  void
  DumpForces( int aSimTick )
    {
    mIntegratorStateManagerIF.DumpForces( aSimTick );
    }

  void
  DumpVelocity( int aSimTick )
    {
    mIntegratorStateManagerIF.DumpVelocity( aSimTick );
    }

  void
  DumpPositions( int aSimTick )
    {
    mIntegratorStateManagerIF.DumpPositions( aSimTick );
    }

void
ManageSpacialDecompositionAllToAll( int aSimTick )
  {
  mFragmentMigrationStart.HitOE( PKTRACE_FRAGMENT_MIGRATION,
                                 "FragmentMigration",
                                 mAddressSpaceId,
                                 mFragmentMigrationStart );

  BegLogLine( 0 )
    << "Entering ManageSpacialDecompositionAllToAll(" << aSimTick << ")"
    << EndLogLine;

  // Upper bound to number of nearest neighbors
  for( int i=0; i<mNumberOfNearestNeighbors; i++ )
    {
    int NRank = mNearestNeighbors[ i ];
    mSDSendCounts[ NRank ]  = 0;
    mSDRecvCounts[ NRank ]  = 1;
    }

  int SentFragmentCount=0;
  int ReceivedFragmentCount=0;

  int RemoveCount = 0;
  int RemoveList[1024];

  int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

  for( int fragmentIndex = 0; fragmentIndex  < NumLocalFrags; fragmentIndex ++ )
    {
    FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( fragmentIndex );    

    XYZ cog;

    XYZ tempCog = mIntegratorStateManagerIF.GetFragmentPosition( fragId );
    NearestImageInPeriodicVolume( mCenterOfBoundingBox , tempCog, cog );

    ORBNode * leaf = mRecBisTree->NodeFromSimCoord( cog.mX, cog.mY, cog.mZ );
    assert( leaf );
    
    // Update voxel of a local fragment
    mIntegratorStateManagerIF.SetVoxelIndex( fragId, mRecBisTree->GetVoxelIndex( cog ) );

    int RankInLeaf = leaf->rank;


    if( RankInLeaf != mAddressSpaceId )
      {
      SentFragmentCount++;
      int RankOfFragment       = RankInLeaf;
      int NearestNeighborIndex = GetNearestNeighborIndexForRank( RankOfFragment );

      assert( NearestNeighborIndex != NOT_NEAREST_NEIGBOR );

      SiteId firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      // This assumes that the offsets and count are in bytes
      char* CurPtr = mSendBufferSD + mSDSendOffsets[ RankInLeaf ] + mSDSendCounts[ RankInLeaf ];
      int CurCount  = mSDSendCounts[ RankInLeaf ];

      assert( CurCount < mSizeOfSDBuffers ) ;

      FragId* FragIdBuff = ( FragId * ) CurPtr;
      memcpy( FragIdBuff, &fragId, sizeof( FragId ));
      CurCount += sizeof( FragId );
      CurPtr += sizeof( FragId );

      BegLogLine( 0 )
        << "NearestNeighborIndex= " << NearestNeighborIndex
        << " firstSiteId= " << firstSiteId
        << " numberOfSites= " << numberOfSites
        << EndLogLine;

      for( int offset = 0; offset < numberOfSites; offset++ )
        {
        assert( CurCount < mSizeOfSDBuffers ) ;

        XYZ * xyz_ptr = (XYZ *) CurPtr;

        BegLogLine( 0 )
          << "xyz_ptr= " << (void *) xyz_ptr
          << EndLogLine;

        XYZ & Pos = mIntegratorStateManagerIF.GetPosition( fragId, offset );

        memcpy( xyz_ptr, &Pos, sizeof( XYZ ) );

        CurCount += sizeof( XYZ );
        CurPtr += sizeof( XYZ );

        BegLogLine( 0 )
          << "CurPtr= " << (void*) CurPtr
          << EndLogLine;

        assert( CurCount < mSizeOfSDBuffers );

        xyz_ptr = (XYZ *) CurPtr;

        XYZ & Vel = mIntegratorStateManagerIF.GetVelocity( fragId, offset );

        memcpy( xyz_ptr, &Vel, sizeof( XYZ ) );
        CurCount += sizeof( XYZ );
        CurPtr += sizeof( XYZ );

        xyz_ptr = (XYZ *) CurPtr;

        XYZ & PosTracker = mIntegratorStateManagerIF.GetPositionTracker( fragId, offset );

        memcpy( xyz_ptr, &PosTracker, sizeof( XYZ ) );
        CurCount += sizeof( XYZ );
        CurPtr += sizeof( XYZ );

        BegLogLine( 0 )
          << "CurPtr= " << (void*) CurPtr
          << EndLogLine;
        }

      if( RemoveCount > 1024 )
        PLATFORM_ABORT(" if( RemoveCount > 1024 ) ");
      RemoveList[ RemoveCount ] = fragId;
      RemoveCount++;

      assert( CurCount <= mSizeOfSDBuffers );

      mSDSendCounts[ RankInLeaf ] = CurCount;
      }
    }

  for(int r = 0; r < RemoveCount; r++)
    {
    mIntegratorStateManagerIF.RemoveFragment( RemoveList[ r ] );
    }

#ifdef PK_MPI_ALL
  
  // Since MPI a2a interface requires a posting of 
  // recieve counts
  Platform::Collective::Alltoallv( mSDSendCounts,
				   mSDSendCountsCounts,
				   mSDSendCountsOffsets,
				   sizeof( int ),
				   mSDRecvCounts,
				   mSDRecvCountsCounts,
				   mSDRecvCountsOffsets,
				   sizeof( int ) );
#endif

    // If nothing to say, still need to send a packet
    for( int i=0; i<mNumberOfNearestNeighbors; i++ )
      {
	int NRank = mNearestNeighbors[ i ];
      if( mSDSendCounts[ NRank ] == 0 )
        mSDSendCounts[ NRank ] = 1;
      }

  Platform::Collective::Alltoallv( mSendBufferSD,
                                  mSDSendCounts,
                                  mSDSendOffsets,
                                  sizeof( char ),
                                  mRecvBufferSD,
                                  mSDRecvCounts,
                                  mSDRecvOffsets,
                                  sizeof( char ) );

  // Manage the incoming data
  for( int r=0; r < mNumberOfNearestNeighbors; r++ )
    {
    int NRank = mNearestNeighbors[ r ];
    int count = mSDRecvCounts[ NRank ];

    // Signal that the node had nothing to say
    if( count == 1 )
      continue;

    char* CurBuff =  (char *)( mRecvBufferSD + mSDRecvOffsets[ NRank ] );

    BegLogLine( 0 )
      << " count=" << count
      << " mSizeOfSDBuffers=" << mSizeOfSDBuffers
      << EndLogLine;

    assert( count <= mSizeOfSDBuffers );

    while( count > 0 )
      {
      ReceivedFragmentCount++;
      FragId fragId;
      FragId* FragIdBuff = ( FragId * ) CurBuff;
      memcpy( (void *) &fragId, (void * ) FragIdBuff, sizeof( FragId ));

      CurBuff += sizeof( FragId );
      count -= sizeof( FragId );
      assert( count > 0 );

      assert( fragId >= 0 && fragId < FRAGMENT_COUNT );

      mIntegratorStateManagerIF.AddFragment( fragId );

      // int firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( FragId );
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset = 0; offset < numberOfSites; offset++ )
        {
        XYZ * xyz_ptr = (XYZ *) CurBuff;
        memcpy( &mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mPosition[ offset ],
                xyz_ptr,
                sizeof( XYZ ));

        CurBuff += sizeof( XYZ );
        count -= sizeof( XYZ );
        assert( count > 0 );

        xyz_ptr = (XYZ *) CurBuff;
        memcpy( &mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mVelocity[ offset ],
                xyz_ptr,
                sizeof( XYZ ));

        assert( mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mVelocity[ offset ].IsReasonable() );

        CurBuff += sizeof( XYZ );
        count -= sizeof( XYZ );
        assert( count > 0 );

        xyz_ptr = (XYZ *) CurBuff;
        memcpy( &mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mPositionImageTracker[ offset ],
                xyz_ptr,
                sizeof( XYZ ));

        CurBuff += sizeof( XYZ );
        count -= sizeof( XYZ );
        assert( count >= 0 );

        }

      // This code has to be here, since GetVoxelIndexForFragment()
      // depends on the incoming position of a new fragment
      // Figure out which voxel id this fragment belongs to on this new node;
      mIntegratorStateManagerIF.UpdateVoxelIndex( fragId );

      #ifndef NDEBUG
        XYZ tempCog = mIntegratorStateManagerIF.GetFragmentPosition( fragId );
        XYZ cog;
        NearestImageInPeriodicVolume( mCenterOfBoundingBox , tempCog, cog );

        ORBNode * leaf = mRecBisTree->NodeFromSimCoord( cog.mX, cog.mY, cog.mZ );
        assert( leaf );
        assert( leaf->rank == mAddressSpaceId );
      #endif
      }
    }

  mFragmentMigrationFinis.HitOE( PKTRACE_FRAGMENT_MIGRATION,
                                 "FragmentMigration",
                                 mAddressSpaceId,
                                 mFragmentMigrationFinis );

  BegLogLine( 0 )
    << "Migration sent: " << SentFragmentCount
    << " recv: " << ReceivedFragmentCount
    << EndLogLine;

  BegLogLine( 0 )
    << "Leaving ManageSpacialDecomposition(" << aSimTick << ")"
    << EndLogLine;

  return;
  }

  int
  CheckSiteDisplacementViolation()
  {
  return CheckSiteDisplacement();
  }

  inline
  void
  ImageAllPositionsIntoCentralCell( int aSimTick )
    {
    mIntegratorStateManagerIF.ImageAllPositionsIntoCentralCell( aSimTick, mCenterOfBoundingBox );
    }

  inline
  void
  ReduceFragmentTime()
  {
  assert( sizeof( mTimeToRunNsqInnerLoop ) == ( sizeof(double) * FRAGMENT_COUNT ) );
  double mTimeToRunNsqInnerLoopTarget[ FRAGMENT_COUNT ];
  double mTimeToRunNsqInnerLoopSource[ FRAGMENT_COUNT ];

  for( int i=0; i < FRAGMENT_COUNT; i++)
    {
    mTimeToRunNsqInnerLoopSource[ i ] = Platform::Clock::ConvertTimeValueToDouble( mTimeToRunNsqInnerLoop[ i ] );
    }

  Platform::Collective::FP_AllReduce_Sum( mTimeToRunNsqInnerLoopSource,
                                          mTimeToRunNsqInnerLoopTarget,
                                          FRAGMENT_COUNT );

  for( int i=0; i < FRAGMENT_COUNT; i++)
    {
    mTimeToRunNsqInnerLoop[ i ] = Platform::Clock::ConvertDoubleToTimeValue( mTimeToRunNsqInnerLoopTarget[ i ] );
    }

  BegLogLine( PKFXLOG_REDUCE_FRAGMENT_TIME )
    << "ReduceFragmentTime "
    << "sizeof(mTimeToRunNsqInnerLoop) =  " << (int) sizeof( mTimeToRunNsqInnerLoop )
    << EndLogLine;
  }

inline
void
AddTimeForFragment( int aFragmentId, TimeValue aTimeValue )
  {
  assert( aFragmentId >= 0 && aFragmentId < FRAGMENT_COUNT );
  mTimeToRunNsqInnerLoop[ aFragmentId ] += aTimeValue;

  return;
  }

inline
double
GetDeltaT()
  {
  /// NOTE: The zero means that the GetDeltaT is used EVERY SimTick and
  /// we therefore need to get the TimeDelta for the lowest respa level
  return ( MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ 0 ] );
  }

inline
XYZ
GetVolumePosition()
  {
  return( mVolume );
  }

inline
void
SetVolumePosition( XYZ aPosition)
  {
  mVolume = aPosition;
  }

inline
double
GetVolumeInverseMass()
  {
  return( 1 / mVolumeMass );
  }

inline
double
GetVolumeHalfInverseMass()
  {
  return( 0.5 / mVolumeMass );
  }

inline
XYZ
GetVolumeRatio()
  {
  return( mVolumeRatio );
  }

inline
void
SetVolumeRatio( XYZ aRatio )
  {
  mVolumeRatio = aRatio;
  return;
  }

inline
XYZ
GetVolumeImpulse()
  {
  return( mVolumeImpulse );
  }

inline
XYZ
GetVolumeVelocity()
  {
  return( mVolumeVelocity );
  }


inline
void
SetVolumeVelocity( XYZ aVelocity )
  {
  mVolumeVelocity = aVelocity;
  return;
  }

inline
XYZ
GetOldVolumeVelocity()
  {
  return( mOldVolumeVelocity );
  }

inline
void
SetOldVolumeVelocity( XYZ aVelocity )
  {
  mOldVolumeVelocity = aVelocity;
  return;
  }

inline
void
SetVolumeVelocityRatio( XYZ aRatio )
  {
  mVolumeVelocityRatio = aRatio;
  return;
  }

#ifndef NOT_TIME_REVERSIBLE
inline
void
SetExpVolumeVelocityRatio( double aRatio )
  {
  mExpVelVolRatio = aRatio;
  return;
  }

inline
double
GetExpVolumeVelocityRatio()
  {
  return mExpVelVolRatio;
  }

inline
void
UpdateExpVolumeVelocityRatioFullStepNPT()
  {
  SetExpVolumeVelocityRatio( exp( mVolumeVelocity.mX*GetDeltaT()/(3.0*mVolume.mX) ) );
  }

inline
void
UpdateExpVolumeVelocityRatioHalfStepNPT()
  {
  SetExpVolumeVelocityRatio( exp( -mVolumeVelocity.mX*0.5*GetDeltaT()/(3.0*mVolume.mX) ) );
  }
#endif

inline
XYZ
GetVolumeInversePosition()
  {
  XYZ InversePosition;
  InversePosition.mX = InversePosition.mY = InversePosition.mZ =
    1/mVolume.mX;
  return( InversePosition );
  }

inline
XYZ
GetVolumeVelocityRatio()
  {
  return( mVolumeVelocityRatio );
  }

inline
void
SetCenterOfMassPosition( int aFragmentId, XYZ aCenterOfMassPosition )
  {
  mLocalFragmentTable[ aFragmentId ].mCenterOfMassPosition = aCenterOfMassPosition;
  }

inline
XYZ
GetCenterOfMassPosition( int aFragmentId )
  {
  XYZ rc = mLocalFragmentTable[ aFragmentId ].mCenterOfMassPosition;
  return( rc );
  }

inline
void
SetCenterOfMassVelocity( int aFragmentId, XYZ aCenterOfMassVelocity )
  {
  mLocalFragmentTable[ aFragmentId ].mCenterOfMassVelocity = aCenterOfMassVelocity;
  }

inline
XYZ
GetCenterOfMassVelocity( int aFragmentId )
  {
  XYZ rc = mLocalFragmentTable[ aFragmentId ].mCenterOfMassVelocity;
  return( rc );
  }

inline
void
SetCenterOfMassForce( int aFragmentId, int aRespaLevel, XYZ aCenterOfMassForce )
  {
  #if MSDDEF_DoPressureControl
  mCenterOfMassForce[ aRespaLevel ][ aFragmentId ] = aCenterOfMassForce;
  #endif
  }

inline
void
AddCenterOfMassForce( int aFragmentId, int aRespaLevel,  XYZ aCenterOfMassForce )
  {
#if MSDDEF_DoPressureControl
  mCenterOfMassForce[ aRespaLevel ][ aFragmentId ] += aCenterOfMassForce;
#endif
  }

inline
XYZ
GetCenterOfMassForce( int aFragmentId, int aRespaLevel )
  {
  XYZ rc;
  #if MSDDEF_DoPressureControl
    assert( aRespaLevel >= 0 && aRespaLevel < RESPA_LEVEL_COUNT );
    rc = mCenterOfMassForce[ aRespaLevel ][ aFragmentId ];
  #endif
  return( rc );
  }

inline
double
GetCenterOfMassMass( int aFragmentId )
  {
  double rc = mLocalFragmentTable[ aFragmentId ].mCenterOfMassMass;
  return( rc );
  }

inline
void
AddCenterOfMassKineticEnergy( double aCenterOfMassKineticEnergy )
  {
  mCenterOfMassKineticEnergy += aCenterOfMassKineticEnergy;
  }

inline
double
GetCenterOfMassHalfInverseMass( int aFragmentId )
  {
  double rc = mLocalFragmentTable[ aFragmentId ].mCenterOfMassHalfInverseMass;
  return( rc );
  }

// Do the one time init of fragment state
inline
void
DoNthIrreducibleFragmentOneTimeInit( FragId aLocalFragmentId )
  {
  SiteId Start = MSD_IF::GetIrreducibleFragmentFirstSiteId( aLocalFragmentId );
  SiteId End   = Start + MSD_IF::GetIrreducibleFragmentMemberSiteCount( aLocalFragmentId );

  double Mass = 0;
  for ( SiteId i=Start; i<End; i++)
    {
    Mass += MSD_IF::GetMass( i );
    }
  mLocalFragmentTable[ aLocalFragmentId ].mCenterOfMassMass            = Mass;
  mLocalFragmentTable[ aLocalFragmentId ].mCenterOfMassHalfInverseMass = 0.5 / Mass;
  }

inline
int
GetNumberOfLocalFragments()
  {
  return mIntegratorStateManagerIF.GetNumberOfLocalFragments();
  }

inline
int
GetIrreducibleFragmentCount()
  {
  // THIS IS AMBIGIOUS - DOES THIS CALL MEAN LOCAL OR GLOBAL FRAG COUNT????  BGF
  int rc = MSD_IF::GetIrreducibleFragmentCount();
  return( rc );
  }

inline
void
InitLoadState()
  {
  mPosit        = (XYZ *) malloc( sizeof( XYZ ) * SITE_COUNT );
  if( mPosit == NULL )
      PLATFORM_ABORT("mPosit == NULL");

  mVelocity     = (XYZ *) malloc( sizeof( XYZ ) * SITE_COUNT );
  if( mVelocity == NULL )
    PLATFORM_ABORT("mVelocity == NULL");
  }

inline
void
LoadDynamicVariables( SiteId aAbsSiteId, XYZ aPosit, XYZ aVelocity )
  {
  assert( aAbsSiteId >= 0 && aAbsSiteId < GetNumberOfSites() );
  SetPosition( aAbsSiteId, aPosit );
  SetVelocity( aAbsSiteId, aVelocity );
  }

inline
void
ZeroTimeValues()
  {
  for(int i=0; i<FRAGMENT_COUNT; i++)
    mTimeToRunNsqInnerLoop[ i ].Zero();
  }

void GenRankForNeighborIndexMap()
  {
  int MachineSize = mProcs_x * mProcs_y * mProcs_z;
  for( int i=0; i < MachineSize; i++ )
    {
    mRankToNearestNeighborMap[ i ] = NOT_NEAREST_NEIGBOR;
    for( int j=0; j < mNumberOfNearestNeighbors; j++ )
      {
      if( mNearestNeighbors[ j ] == i )
        mRankToNearestNeighborMap[ i ] = j;
      }
    }
  }

  static
  void
  CreateExclusionMask( unsigned int aTargetI, unsigned int aSourceI, ExclusionMask_T* aExclusionMask )
  {
  int SourceIsTargetFlag = ( aTargetI == aSourceI );

  int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aSourceI );
  int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aTargetI );

  *aExclusionMask = 0;

  // NOTE:arayshu: Code which reads this mask has to unwind the stack in the opposite order.

  static int count2=0;

  for(int SourceFragmentSiteIndex = SourceFragmentSiteCount-1;
      SourceFragmentSiteIndex >= 0;
      SourceFragmentSiteIndex-- )
    {
    unsigned int SourceAbsSiteId =
      MSD_IF::GetIrreducibleFragmentMemberSiteId( aSourceI, SourceFragmentSiteIndex );

    // Scan the Target Fragment since that should be more local.
    for( int TargetFragmentSiteIndex = (TargetFragmentSiteCount-1);
         TargetFragmentSiteIndex >= (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1: 0 );
         TargetFragmentSiteIndex-- )
      {
      unsigned int TargetAbsSiteId =
        MSD_IF::GetIrreducibleFragmentMemberSiteId( aTargetI, TargetFragmentSiteIndex );

      unsigned IsInExcList = MSD_IF::AreSitesInExcIncList( SourceAbsSiteId, TargetAbsSiteId );

      *aExclusionMask = *aExclusionMask << 1;
      *aExclusionMask = ( *aExclusionMask | ( IsInExcList & 0x1 ) );

      BegLogLine( 0 )
        << "IFPHelper:: "
        << " S=" << SourceFragmentSiteIndex
        << " T=" << TargetFragmentSiteIndex
        << " I=" << IsInExcList
        << EndLogLine;
      }
    }
  }

int IsIntegratorFragmentLocal( FragId aFragId )
  {
  unsigned int rc = !mIntegratorStateManagerIF.IsInvalid( aFragId );
  return rc;
  }

XYZ& 
GetFirstPositionInFragment( int aFragId )
  {
    int base   = GetNthIrreducibleFragmentFirstSiteIndex( aFragId );
    XYZ & pos = mPosit[ base ];
    return pos;
  }

void
SetOrb( ORBNode* aORB )
  {
    mRecBisTree = aORB;
    mInteractionStateManagerIF.SetOrb( aORB );
    
    mBondedInteractionStateManagerIF.SetOrb( aORB );
  }

void
Init()
  {
  BegLogLine( PKFXLOG_SETUP )
    << "Entering DynVarMgr.Init()"
    << EndLogLine;

  ZeroPotentialEnergy();

  mSiteIdToFragmentRepMap     = (FragmentRep *) malloc( sizeof( FragmentRep ) * SITE_COUNT );
  if( mSiteIdToFragmentRepMap == NULL )
    PLATFORM_ABORT("mSiteIdToFragmentRepMap == NULL");

  mTimeToRunNsqInnerLoop = (TimeValue* ) malloc( sizeof( TimeValue ) * FRAGMENT_COUNT );
  if( mTimeToRunNsqInnerLoop == NULL )
    PLATFORM_ABORT("mTimeToRunNsqInnerLoop == NULL");

  mLocalFragments = (FragmentInRangePair *) malloc( sizeof( FragmentInRangePair ) * FRAGMENT_COUNT );
  if( mLocalFragments == NULL )
    PLATFORM_ABORT("mLocalFragments == NULL");

  mLocalFragmentTable = (FragmentRecord *) malloc( sizeof( FragmentRecord ) * FRAGMENT_COUNT );
  if( mLocalFragmentTable == NULL )
    PLATFORM_ABORT("mLocalFragmentTable == NULL");


  mPositTracker = (XYZ *) malloc( sizeof( XYZ ) * SITE_COUNT );
  if( mPositTracker == NULL )
    PLATFORM_ABORT("mPositTracker == NULL");

  mRecBisTree = NULL;

  #if MSDDEF_DoPressureControl
    mCurrentVerletGuardZone = 0.0;
  #else
    mCurrentVerletGuardZone = 1.0;
  #endif

  mCurrentAssignmentGuardZone = 3;

  Platform::Topology::GetDimensions( &mProcs_x, &mProcs_y, &mProcs_z );

  Platform::Topology::GetMyCoords( &mMyP_x, &mMyP_y, &mMyP_z );
  double BoundingBoxDim_x = RTG.mStartingBoundingBoxDimensionVector.mX;
  double BoundingBoxDim_y = RTG.mStartingBoundingBoxDimensionVector.mY;
  double BoundingBoxDim_z = RTG.mStartingBoundingBoxDimensionVector.mZ;

  mLocalDim_x = BoundingBoxDim_x / mProcs_x;
  mLocalDim_y = BoundingBoxDim_y / mProcs_y;
  mLocalDim_z = BoundingBoxDim_z / mProcs_z;

  mAddressSpaceId = Platform::Topology::GetAddressSpaceId();

  mDynamicBoxDimensionVector = RTG.mStartingBoundingBoxDimensionVector;

  mDynamicBoxInverseDimensionVector.mX = 1.0 / mDynamicBoxDimensionVector.mX;
  mDynamicBoxInverseDimensionVector.mY = 1.0 / mDynamicBoxDimensionVector.mY;
  mDynamicBoxInverseDimensionVector.mZ = 1.0 / mDynamicBoxDimensionVector.mZ;

  mBirthBoxAbsoluteRatio.mX = mDynamicBoxInverseDimensionVector.mX * RTG.mBirthBoundingBoxDimensionVector.mX;
  mBirthBoxAbsoluteRatio.mY = mDynamicBoxInverseDimensionVector.mY * RTG.mBirthBoundingBoxDimensionVector.mY;
  mBirthBoxAbsoluteRatio.mZ = mDynamicBoxInverseDimensionVector.mZ * RTG.mBirthBoundingBoxDimensionVector.mZ;

  mCenterOfBoundingBox.mX = BoundingBoxDim_x / 2.0;
  mCenterOfBoundingBox.mY = BoundingBoxDim_y / 2.0;
  mCenterOfBoundingBox.mZ = BoundingBoxDim_z / 2.0;

  for ( FragId i=0; i< GetIrreducibleFragmentCount(); i++ )
    {
    DoNthIrreducibleFragmentOneTimeInit( i );
    }

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): About to setup the mSiteIdToFragmentRepMap"
    << EndLogLine;

  for( FragId fragId=0; fragId < FRAGMENT_COUNT; fragId++ )
    {
    SiteId firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
    int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

    int offset = 0;
    for( SiteId siteId=firstSiteId; siteId < (firstSiteId + numberOfSites); siteId++ )
      {
      mSiteIdToFragmentRepMap[ siteId ].mFragId = fragId;
      mSiteIdToFragmentRepMap[ siteId ].mOffset = offset;

      BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
          << "FragId " << fragId                                                                             
          << " SiteOffsetInFrag " << offset                                                                  
          << " SiteCount " << numberOfSites                                                                  
          << " InternalSiteId " << firstSiteId + offset                                                      
          << " ExternalSiteId " << MSD_IF::GetExternalSiteIdForInternalSiteId( siteId )                     
          << EndLogLine;    

      offset++;
      }
    }

  int TargetVoxelsPerProc = 200;
  double MinTargetVoxelDim = 1.0;

  GetVoxelMeshDimensions( TargetVoxelsPerProc,
                          MinTargetVoxelDim,
                          mDynamicBoxDimensionVector.mX,
                          mDynamicBoxDimensionVector.mY,
                          mDynamicBoxDimensionVector.mZ,
                          mProcs_x,
                          mProcs_y,
                          mProcs_z,
                          mVoxDimX,
                          mVoxDimY,
                          mVoxDimZ );

  if( ( Platform::Topology::mVoxDimx != 0 ) &&
      ( Platform::Topology::mVoxDimy != 0 ) &&
      ( Platform::Topology::mVoxDimz != 0 ) )
    {
    mVoxDimX = Platform::Topology::mVoxDimx;
    mVoxDimY = Platform::Topology::mVoxDimy;
    mVoxDimZ = Platform::Topology::mVoxDimz;
    }

  // Rotate Voxel Dimensions
  iXYZ VoxDims;
  VoxDims.mX = mVoxDimX;
  VoxDims.mY = mVoxDimY;
  VoxDims.mZ = mVoxDimZ;
  VoxDims.ReOrderAbs( RTG.mBoxIndexOrder );
  mVoxDimX = VoxDims.mX;
  mVoxDimY = VoxDims.mY;
  mVoxDimZ = VoxDims.mZ;

  BegLogLine( mAddressSpaceId == 0 )
    << "TargetVoxelsPerProc: " << TargetVoxelsPerProc
    << " MinTargetVoxelDim: " << MinTargetVoxelDim
    << " mDynamicBoxDimensionVector: " << mDynamicBoxDimensionVector
    << " mProcs_x: " << mProcs_x
    << " mProcs_y: " << mProcs_y
    << " mProcs_z: " << mProcs_z
    << " mVoxDimX: " << mVoxDimX
    << " mVoxDimY: " << mVoxDimY
    << " mVoxDimZ: " << mVoxDimZ
    << EndLogLine;

  mInteractionStateManagerIF.Init();

  mBondedInteractionStateManagerIF.Init( );

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): About to figure out the max extent"
    << EndLogLine;

  int maxSites = -1;
  double maxExtent = -1.0;
  for( FragId f=0; f < FRAGMENT_COUNT; f++ )
    {
    int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( f );
    double extent = MSD_IF::GetFragmentExtent( f );

    if( numberOfSites > maxSites )
      maxSites = numberOfSites;

    if( extent > maxExtent )
      maxExtent = extent;
    }

  mMaxSitesInFragment = maxSites;

  mMaxExtent = 1.82;

  double VoxelSpace_x = mDynamicBoxDimensionVector.mX / mVoxDimX;
  double VoxelSpace_y = mDynamicBoxDimensionVector.mY / mVoxDimY;
  double VoxelSpace_z = mDynamicBoxDimensionVector.mZ / mVoxDimZ;

  double VoxelDiagonal = sqrt( VoxelSpace_x * VoxelSpace_x +
                               VoxelSpace_y * VoxelSpace_y +
                               VoxelSpace_z * VoxelSpace_z );

  double MaxOfMaxExtentAndHalfVoxelDiagonal = max( VoxelDiagonal, mMaxExtent );
  double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
  double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );

  mRadiusOfSphere = (( SwitchLowerCutoff + SwitchDelta + mCurrentAssignmentGuardZone ) / 2.0 ) + MaxOfMaxExtentAndHalfVoxelDiagonal;

  // IF YOU CHANGE THE FORMULA FOR mRadiusOfSphere THENE YOU MUST UPDATE THE LOG LINE BELOW
  // IF YOU CHANGE THE FORMULA FOR mRadiusOfSphere THENE YOU MUST UPDATE THE LOG LINE BELOW
  // IF YOU CHANGE THE FORMULA FOR mRadiusOfSphere THENE YOU MUST UPDATE THE LOG LINE BELOW

  BegLogLine( PKFXLOG_SETUP )
    << "Setting mRadiusOfSphere "
    << mRadiusOfSphere
    << " Compried of : 0.5 * ( "
    << " SwitchLowerCutoff "
    << SwitchLowerCutoff
    << " SwitchDelta "
    << SwitchDelta
    << " mCurrentAssignmentGuardZone "
    << mCurrentAssignmentGuardZone
    << " ) + MaxOfMaxExtentAndHalfVoxelDiagonal "
    << MaxOfMaxExtentAndHalfVoxelDiagonal
    << EndLogLine;

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): mRadiusOfSphere " << mRadiusOfSphere
    << EndLogLine;

  #ifdef VOXEL_INTERACTION_TABLE_ENTRY_AS_CHAR
    mVoxelInteractionTypeTable = NULL;
  #endif

  mNearestNeighbors = NULL;
  mSendBufferSD = NULL;
  mRecvBufferSD = NULL;

  mSDSendCounts = NULL;
  mSDSendOffsets = NULL;
  mSDRecvCounts = NULL;
  mSDRecvOffsets = NULL;

  mSDSendCountsOffsets = NULL;
  mSDRecvCountsOffsets = NULL;
  mSDSendCountsCounts = NULL;
  mSDRecvCountsCounts = NULL;

  int MachineSize = mProcs_x * mProcs_y * mProcs_z;
  pkNew( &mRankToNearestNeighborMap, MachineSize );

  mNodeIdToNodeOrdinalMap = (int *) malloc( sizeof(int) * MachineSize );

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): About to create the 125 image vectors "
    << EndLogLine;

  // Init the 125 Image Vectors
  for( int i = -2; i <= 2; i++ )
    {
    for( int j = -2; j <= 2; j++ )
      {
      for( int k = -2; k <= 2; k++ )
        {
        int index = GetIndexForImageMultiplierIn125ImageRange(i,j,k);

        XYZ ImageVector;
        ImageVector.mX = mDynamicBoxDimensionVector.mX * i;
        ImageVector.mY = mDynamicBoxDimensionVector.mY * j;
        ImageVector.mZ = mDynamicBoxDimensionVector.mZ * k;

        // IFP Tiled likes ImaveVectors negative
        assert( index >=0 && index < 125 );
        mImageVectorTable[ index ] = -ImageVector;

        // ... IFPWaterSite is going to use something different
        mImageMap[ index ] = ( ( i + 2 ) << 16  ) | ( ( j + 2 ) << 8 ) | ( k + 2 ) ;
        }
      }
    }


  // Version 2 voxelization results in interactions
  // assigned to nodes 1/2 max Voxel dimension away from
  // the true center.
  int VoxelMeshSize = mVoxDimX * mVoxDimY * mVoxDimZ;

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): About to mNeighborhoodCommDriver"
    << EndLogLine;

  mNeighborhoodCommDriver.Init( &mInteractionStateManagerIF, &mIntegratorStateManagerIF,
                                MAX_SITES_IN_FRAGMENT, VoxelMeshSize );

  mNeighborhoodCommBondedDriver.Init( &mBondedInteractionStateManagerIF, &mIntegratorStateManagerIF,
                                      MAX_SITES_IN_FRAGMENT, VoxelMeshSize );

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): About to mIntegratorStateManagerIF.Init()"
    << EndLogLine;

  mIntegratorStateManagerIF.Init( mSiteIdToFragmentRepMap );

  int executableId = EXECUTABLEID;
  BegLogLine( (Platform::Topology::GetAddressSpaceId() == 0 ) )
    << "SourceId: " << MSD_IF::GetSourceId()
    << " ExecutableId: " << executableId
    << " JobId: " << Platform::JobInfo::JobId()
    << " ProcMeshDim: { " << mProcs_x << " , " << mProcs_y << " , " << mProcs_z << " }"
#ifdef MSDDEF_P3ME
    << " FFTMeshDim: { " << FFT_PLAN::GLOBAL_SIZE_X << " , " << FFT_PLAN::GLOBAL_SIZE_Y << " , " << FFT_PLAN::GLOBAL_SIZE_Z << " }"
#endif
    << " VoxMeshDim: { " << mVoxDimX << " , " << mVoxDimY << " , " << mVoxDimZ << " }"
    << EndLogLine;
  
  for( int fragIdA = 0; fragIdA < FRAGMENT_COUNT; fragIdA++ )
    {
      double ExtentA;
      XYZ FragCenterA;
      
      GetDynamicFragmentExtentFragmentCenterVersion2( fragIdA, &ExtentA, &FragCenterA );
      mFragCenters[ fragIdA ] = FragCenterA;
      mExtents[ fragIdA ] = ExtentA;
    }

  BegLogLine( PKFXLOG_SETUP )
    << "Leaving DynVarMgr.Init(): "
    << EndLogLine;
  }

  XYZ GetImageVector(unsigned int index) const
  {
        XYZ ImageVector ;
        assert ( index < 125 ) ;
        unsigned int ImageMap = mImageMap [ index ] ;
        unsigned int i = ImageMap >> 16 ;
        unsigned int j = ( ImageMap >> 8 ) & 0xff ;
        unsigned int k = ImageMap & 0xff ;
        ImageVector.mX = mDynamicBoxDimensionVector.mX * dk_ImageTable[i] ;
        ImageVector.mY = mDynamicBoxDimensionVector.mY * dk_ImageTable[j] ;
        ImageVector.mZ = mDynamicBoxDimensionVector.mZ * dk_ImageTable[k] ;
        return ImageVector ;
  }

  void GetImageVector(XYZ& aImageVector, unsigned int index) const
  {
    assert ( index < 125 ) ;
    unsigned int ImageMap = mImageMap [ index ] ;
    unsigned int i = ImageMap >> 16 ;
    unsigned int j = ( ImageMap >> 8 ) & 0xff ;
    unsigned int k = ImageMap & 0xff ;
    aImageVector.mX = mDynamicBoxDimensionVector.mX * dk_ImageTable[i] ;
    aImageVector.mY = mDynamicBoxDimensionVector.mY * dk_ImageTable[j] ;
    aImageVector.mZ = mDynamicBoxDimensionVector.mZ * dk_ImageTable[k] ;
  }


  void InitPostOrbState( ORBNode* aORB )
    {
    BegLogLine( PKFXLOG_SETUP )
      << "Entering InitPostOrbState"
      << EndLogLine;

    mRecBisTree = aORB;
    mIntegratorStateManagerIF.SetORB( aORB );
    mInteractionStateManagerIF.SetOrb( aORB );
    mBondedInteractionStateManagerIF.SetOrb( aORB );

    mPreNumberOfNearestNeighbors = mNumberOfNearestNeighbors;

    SpacialDecomposition();

    BegLogLine( PKFXLOG_SETUP )
      << "InitPostOrbState: Done with the SpacialDecomposition()"
      << EndLogLine;

    GenRankForNeighborIndexMap();

    mSparseSkinManagerIF.Init( mExtents, 
			       mFragCenters, 
			       & mIntegratorStateManagerIF,
			       mRecBisTree,
			       & mRadiusOfSphere,
			       & mCurrentVerletGuardZone,
			       mVoxDimX, mVoxDimY, mVoxDimZ );

    mSparseSkinManagerIF.CreateSparseSkinAndAssignement();
    
    int* SparseSkinNodes      = mSparseSkinManagerIF.GetSparseSkinNodes();
    int  SparseSkinNodesCount = mSparseSkinManagerIF.GetSparseSkinNodesCount();

    BegLogLine( PKFXLOG_SPARSE_SKIN_NODE_COUNT )
        << "COMM_INFO SparseSkinNodesCount: " << SparseSkinNodesCount
        << EndLogLine;

    mNeighborhoodCommDriver.ReInit( mRecBisTree,
                                    SparseSkinNodes,
                                    SparseSkinNodesCount );

    int MachineSize = Platform::Topology::GetAddressSpaceCount();
    int xMy, yMy, zMy;
    Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
    
    double BondedForceRadius = 5.0;

    mNodesForBondedCount = 0;
    mNodesForBonded = (int *) malloc( sizeof(int) * Platform::Topology::GetAddressSpaceCount() );
    if( mNodesForBonded == NULL )
      PLATFORM_ABORT( "mNodesForBonded == NULL" );

    mRecBisTree->RanksInNeighborhood( xMy, yMy, zMy,
                                      BondedForceRadius,
                                      mNodesForBonded,
                                      mNodesForBondedCount,
                                      MachineSize );

    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
        << "COMM_INFO mNodesForBondedCount: " << mNodesForBondedCount
        << EndLogLine;

    mNeighborhoodCommBondedDriver.ReInit( mRecBisTree,
                                          mNodesForBonded,
                                          mNodesForBondedCount );

    // This is only for information purposes
    int NodesInHoodCount = 0;
    int* NodesInHood      = (int *) malloc( sizeof( int ) * MachineSize );
    if( NodesInHood == NULL )
    PLATFORM_ABORT( "NodesInHood == NULL" );

    mRecBisTree->RanksInNeighborhood( xMy, yMy, zMy,
                                      mRadiusOfSphere,
                                      NodesInHood,
                                      NodesInHoodCount,
                                      MachineSize );
    
    BegLogLine( PKFXLOG_SPARSE_SKIN_NODE_COUNT )
        << "COMM_INFO Nodes in V4 hood count: " << NodesInHoodCount
        << EndLogLine;

    free( NodesInHood );
    NodesInHood = NULL;        

    BegLogLine( PKFXLOG_SETUP )
      << "InitPostOrbState: About to GlobalizePositions"
      << EndLogLine;

    GlobalizePositions( 0, 0, 1 );

    BegLogLine( PKFXLOG_SETUP )
      << "Leaving InitPostOrbState"
      << EndLogLine;
    }

  int GetIndexForImageMultiplierIn125ImageRange( int x, int y, int z )
    {
        BegLogLine(PKFXLOG_IMAGE_RANGE)
         << "GetIndexForImageMultiplierIn125ImageRange(" << x
         << "," << y
         << "," << z
         << ")"
         << EndLogLine ;
    assert( ( x >= -2 ) && ( x <= 2 ) );
    assert( ( y >= -2 ) && ( y <= 2 ) );
    assert( ( z >= -2 ) && ( z <= 2 ) );

    int i_ind = x + 2;
    int j_ind = y + 2;
    int k_ind = z + 2;
    int index = 25 * i_ind + 5 * j_ind + k_ind;

    return index;
    }

  int CheckBoundingBoxViolation( int aSimTick )
    {
    // This assumes that the Bounding Box Vector is at the origin
    XYZ UpperLimitOfBox = 1.5 * mDynamicBoxDimensionVector;
    XYZ LowerLimitOfBox = -0.5 * mDynamicBoxDimensionVector;

    int rc = mIntegratorStateManagerIF.CheckBoundingBoxViolation( LowerLimitOfBox, UpperLimitOfBox );

    return ( rc );
    }

void InitSpacialDecompositionManagementAllToAll()
  {
  int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

  int MaxNumberOfLocalFrags = 0;
  Platform::Collective::GlobalMax( (unsigned long *) &NumLocalFrags, (unsigned long *) &MaxNumberOfLocalFrags );

  BegLogLine( 0 )
    << "NumLocalFrags: " << NumLocalFrags
    << EndLogLine;

  int SafetyFactor = 5;
  if( Platform::Topology::GetAddressSpaceCount() > FRAGMENT_COUNT )
    {
    SafetyFactor = 10;
    }

  mSizeOfSDBuffers = SafetyFactor * MaxNumberOfLocalFrags *
      ( sizeof( int ) + sizeof( XYZ ) + mMaxSitesInFragment * ( 3 * sizeof( XYZ )) );

  assert( mSizeOfSDBuffers != 0 );

  mSizeOfSDBuffers = ROUND32( mSizeOfSDBuffers );

  BegLogLine( 0 )
    << "mSizeOfSDBuffers=" << mSizeOfSDBuffers
    << "NumLocalFrags=" << NumLocalFrags
    << "mMaxSitesInFragment=" << mMaxSitesInFragment
    << EndLogLine;

  if ( mNearestNeighbors != NULL )
    free( mNearestNeighbors );

  int MaxVoxels = mVoxDimX * mVoxDimY * mVoxDimZ;
  mNearestNeighbors = ( int *) malloc( MaxVoxels * sizeof( int ));
  if( mNearestNeighbors == NULL )
    PLATFORM_ABORT("mNearestNeighbors == NULL");

  double radius = mCurrentAssignmentGuardZone / 2.0; // Just about half the guard zone

  mNumberOfNearestNeighbors=0;
  mRecBisTree->RanksInNeighborhood( mMyP_x, mMyP_y, mMyP_z, radius, mNearestNeighbors, mNumberOfNearestNeighbors, MaxVoxels );

  if( mSendBufferSD != NULL )
    {
    free( mSendBufferSD );
    free( mRecvBufferSD );
    }

  mSendBufferSD = (char *) malloc( mNumberOfNearestNeighbors * mSizeOfSDBuffers * sizeof( char ));
  mRecvBufferSD = (char *) malloc( mNumberOfNearestNeighbors * mSizeOfSDBuffers * sizeof( char ));

  if( mSendBufferSD == NULL )
    PLATFORM_ABORT("mSendBufferSD == NULL");

  if( mRecvBufferSD == NULL )
    PLATFORM_ABORT("mRecvBufferSD == NULL");

  BegLogLine( 0 )
    << "mNumberOfNearestNeighbors=" << mNumberOfNearestNeighbors
    << " mSizeOfSDBuffers=" << mSizeOfSDBuffers
    << EndLogLine;

  if( mSDSendCounts != NULL )
    free( mSDSendCounts );
  mSDSendCounts = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDSendCounts );

  if( mSDSendOffsets != NULL )
    free( mSDSendOffsets );
  mSDSendOffsets = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDSendOffsets );

  if( mSDRecvCounts != NULL )
    free( mSDRecvCounts );
  mSDRecvCounts = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDRecvCounts );

  if( mSDRecvOffsets != NULL )
    free( mSDRecvOffsets );
  mSDRecvOffsets = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDRecvOffsets );

  if( mSDSendCountsOffsets != NULL )
    free( mSDSendCountsOffsets );
  mSDSendCountsOffsets = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDSendCountsOffsets );

  if( mSDRecvCountsOffsets != NULL )
    free( mSDRecvCountsOffsets );
  mSDRecvCountsOffsets = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDRecvCountsOffsets );

  if( mSDRecvCountsCounts != NULL )
    free( mSDRecvCountsCounts );
  mSDRecvCountsCounts = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDRecvCountsCounts );

  if( mSDSendCountsCounts != NULL )
    free( mSDSendCountsCounts );
  mSDSendCountsCounts = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
  assert( mSDSendCountsCounts );

  for( int i=0; i<Platform::Topology::GetSize(); i++ )
    {
    mSDSendCounts[ i ]  = 0;
    mSDSendOffsets[ i ] = 0;
    mSDRecvCounts[ i ]  = 0;
    mSDRecvOffsets[ i ] = 0;

    // Since we can't over post a buffer in MPI_Alltoallv
    mSDSendCountsCounts[ i ]  = 0;
    mSDSendCountsOffsets[ i ] = 0;
    mSDRecvCountsCounts[ i ]  = 0;
    mSDRecvCountsOffsets[ i ] = 0;
    }

  for( int i=0; i < mNumberOfNearestNeighbors; i++ )
    {
    int NRank = mNearestNeighbors[ i ];

    mSDSendOffsets[ NRank ] = mSizeOfSDBuffers * i;
    mSDRecvOffsets[ NRank ] = mSizeOfSDBuffers * i;
    mSDSendCountsOffsets[ NRank ] = NRank;
    mSDRecvCountsOffsets[ NRank ] = NRank;
    mSDSendCountsCounts[ NRank ]  = 1;
    mSDRecvCountsCounts[ NRank ]  = 1;
    }
  }

inline
void
SpacialDecomposition()
  {
  BegLogLine( 0 )
    << "Entered the SpacialDecomposition::"
    << EndLogLine;

  mIntegratorStateManagerIF.Reset();

  for( FragId i=0; i < FRAGMENT_COUNT; i++)
    {
    XYZ cog;
    GetImagedFragCenterVersion2( i, cog );

    BegLogLine( 0 )
      << "cog: " << cog
      << EndLogLine;

    ORBNode * leaf = mRecBisTree->NodeFromSimCoord( cog.mX, cog.mY, cog.mZ );

    assert( leaf );

    if( leaf->rank == mAddressSpaceId )
      {

      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( i );
      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( i );

      mIntegratorStateManagerIF.AddFragment( i );

      for( int offset=0; offset < SiteCount; offset++ )
        {
        SiteId CurSite = FirstSiteId + offset;
        XYZ & Pos = mPosit[ CurSite ];
        XYZ & Vel = mVelocity[ CurSite ];

        mIntegratorStateManagerIF.SetPosition( i, offset, Pos );
        mIntegratorStateManagerIF.SetVelocity( i, offset, Vel );
        }
      }
    }

  mIntegratorStateManagerIF.ImageAllPositionsIntoCentralCell( 0, mCenterOfBoundingBox );
  mIntegratorStateManagerIF.AssignVoxelIndex();

  int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

  InitSpacialDecompositionManagementAllToAll();

  return;
  }

  inline
  void
  incRealSpaceSend()
  {
  mNumberOfRealSpaceCalls++;
  }


inline
XYZ&
GetPositionForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return mIntegratorStateManagerIF.GetPosition( fReg.mFragId, fReg.mOffset );
  }

inline
XYZ
GetInteractionPositionForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  XYZ rc = mBondedInteractionStateManagerIF.GetPosition( fReg.mFragId, fReg.mOffset );
  mBondedInteractionStateManagerIF.SetUseFragment( fReg.mFragId, 1 );
  return rc;
  }

inline
void
SetPositionForSite( SiteId aSiteId, XYZ& aPos )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return mIntegratorStateManagerIF.SetPosition( fReg.mFragId, fReg.mOffset, aPos );
  }

inline
void
SetVelocityForSite( SiteId aSiteId, XYZ& aVel )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return mIntegratorStateManagerIF.SetVelocity( fReg.mFragId, fReg.mOffset, aVel );
  }

inline
XYZ&
GetVelocityForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return mIntegratorStateManagerIF.GetVelocity( fReg.mFragId, fReg.mOffset );
  }

inline
void
DisplayForceForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  BegLogLine( 1 )
        << "Force on site " << aSiteId
        << " is " << mInteractionStateManagerIF.GetForce( fReg.mFragId, fReg.mOffset)
        << EndLogLine  ;
  }

inline
void
AddForceForSite( SiteId aSiteId, const XYZ& aForce )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  XYZ fb = mBondedInteractionStateManagerIF.GetForce( fReg.mFragId, fReg.mOffset );

  mBondedInteractionStateManagerIF.AddForce( fReg.mFragId, fReg.mOffset, (XYZ&) aForce );
  }

inline double* AddressForceForSite(SiteId aSiteId )
{
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return        mInteractionStateManagerIF.AddressForce(fReg.mFragId, fReg.mOffset );
}

inline
XYZ
GetForceForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  XYZ fb = mInteractionStateManagerIF.GetForce( fReg.mFragId, fReg.mOffset );

  return fb ;
  }

inline
void
GetForceForSite( SiteId aSiteId, XYZ& aForce )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  mInteractionStateManagerIF.GetForce( fReg.mFragId, fReg.mOffset, aForce );

  }

inline
void
GetForceForSite( SiteId aSiteId, double& aForceX, double& aForceY, double& aForceZ )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  mInteractionStateManagerIF.GetForce( fReg.mFragId, fReg.mOffset, aForceX, aForceY, aForceZ );

  }

inline
double
ForceXForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return mInteractionStateManagerIF.ForceX( fReg.mFragId, fReg.mOffset );

  }

inline
double
ForceYForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return mInteractionStateManagerIF.ForceY( fReg.mFragId, fReg.mOffset );

  }

inline
double
ForceZForSite( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  return mInteractionStateManagerIF.ForceZ( fReg.mFragId, fReg.mOffset );

  }


inline
void
SetForceForSite( SiteId aSiteId, XYZ aForce )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  mInteractionStateManagerIF.SetForce( fReg.mFragId, fReg.mOffset, aForce );
  }

inline
void
SetForceForSite( SiteId aSiteId, double aForceX, double aForceY, double aForceZ )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  mInteractionStateManagerIF.SetForce( fReg.mFragId, fReg.mOffset, aForceX, aForceY, aForceZ );
  }

inline
XYZ&
GetPositionFromFullTable( SiteId aAbsoluteSiteId )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  XYZ & rc = mPosit[ aAbsoluteSiteId ];

  assert( !rc.IsInvalid() );

  return( rc );
 }

inline double PositionXFromFullTable(SiteId aAbsoluteSiteId )
{
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  return mPosit[ aAbsoluteSiteId ].mX ;	
}

inline double PositionYFromFullTable(SiteId aAbsoluteSiteId )
{
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  return mPosit[ aAbsoluteSiteId ].mY ;	
}

inline double PositionZFromFullTable(SiteId aAbsoluteSiteId )
{
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  return mPosit[ aAbsoluteSiteId ].mZ ;	
}

inline
void
SetPosition( SiteId aAbsoluteSiteId, XYZ aPosition )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  mPosit[ aAbsoluteSiteId ] = aPosition;
  }

inline
void
SetVelocity( SiteId aAbsoluteSiteId, XYZ aVelocity )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  mVelocity[ aAbsoluteSiteId ] = aVelocity;
  }

inline
void
ExportDynamicVariables( unsigned aOuterTimeStep )
  {
  #ifdef DYNVARMNGR_NOEXPORTOFSITES
  return;
  #endif

  mIntegratorStateManagerIF.EmitState( aOuterTimeStep );

  #if MSDDEF_NOSE_HOOVER
  if( mAddressSpaceId == 0 )
    for( int i=0; i < FRAGMENT_COUNT; i++)
      {
      ED_Emit_DynamicVariablesTwoChainFragmentThermostat( aOuterTimeStep,
                                                          i,
                                                          mAddressSpaceId,
                                                          NUMBER_OF_SITES_PER_THERMOSTAT,
                                                          mLocalFragmentTable[ i ].mThermostatPosition[ 0 ],
                                                          mLocalFragmentTable[ i ].mThermostatVelocity[ 0 ],
                                                          mLocalFragmentTable[ i ].mThermostatPosition[ 1 ],
                                                          mLocalFragmentTable[ i ].mThermostatVelocity[ 1 ]);
      }

  #if MSDDEF_DoPressureControl
  if( mAddressSpaceId == 0 )
    ED_Emit_DynamicVariablesOneChainThermostat( aOuterTimeStep,
                                                mAddressSpaceId,
                                                NUMBER_OF_SITES_PER_THERMOSTAT,
                                                mPistonThermostatPosition,
                                                mPistonThermostatVelocity );
  #endif
  #endif

  BegLogLine( PKFXLOG_EXPDYNVAR ) << "ExportDynamicVariables:: Before Barrier " << EndLogLine;
  Platform::Control::Barrier();
  BegLogLine( PKFXLOG_EXPDYNVAR ) << "ExportDynamicVariables:: After Barrier " << EndLogLine;
  DumpNoseHooverState( aOuterTimeStep );
  }


  inline
  int
  GetNumberOfSites()
  {
  int rc = SITE_COUNT;
  return( rc );
  }

inline
void
ZeroForceRegisters( int    aSimTick,
                    int    aIntegratorLevel,
                    int    aStartLevel = 0 )
  {
  ZeroCenterOfMassForceReg();

  mIntegratorStateManagerIF.ZeroForces();
  }

/********************************************
 *  Nose-Hoover methods
 ********************************************/
#if MSDDEF_NOSE_HOOVER
inline
void
SetThermostatVelocity( int aCurrentFragmentId, int aThermCount, int aThermSiteCount, double &aVel )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
  assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
  mLocalFragmentTable[ aCurrentFragmentId ].mThermostatVelocity[ aThermCount ][ aThermSiteCount ] = aVel;
  }

inline
void
SetThermostatPosition( int aCurrentFragmentId, int aThermCount, int aThermSiteCount, double &aPos )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
  assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
  mLocalFragmentTable[ aCurrentFragmentId ].mThermostatPosition[ aThermCount ][ aThermSiteCount ] = aPos;
  }

inline
void
SetThermostatForce( int aCurrentFragmentId, int aThermCount, int aThermSiteCount, double &aForce )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
  assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
  mLocalFragmentTable[ aCurrentFragmentId ].mThermostatForce[ aThermCount ][ aThermSiteCount ] = aForce;
  }

inline
void
SetThermostatMass( int aCurrentFragmentId, int aThermCount, int aThermSiteCount, double &aMass )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
  assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
  mLocalFragmentTable[ aCurrentFragmentId ].mThermostatMass[ aThermCount ][ aThermSiteCount ] = aMass;
  }


inline
double
GetThermostatVelocity( int aCurrentFragmentId, int aThermCount, int aThermSiteCount )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
  assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
  return mLocalFragmentTable[ aCurrentFragmentId ].mThermostatVelocity[ aThermCount ][ aThermSiteCount ];
  }

inline
double
GetThermostatPosition( int aCurrentFragmentId, int aThermCount, int aThermSiteCount )
    {
    assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
    assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
    assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
    return mLocalFragmentTable[ aCurrentFragmentId ].mThermostatPosition[ aThermCount ][ aThermSiteCount ];
    }

inline
double
GetThermostatForce( int aCurrentFragmentId, int aThermCount, int aThermSiteCount )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
  assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
  return mLocalFragmentTable[ aCurrentFragmentId ].mThermostatForce[ aThermCount ][ aThermSiteCount ];
  }

inline
double
GetThermostatMass( int aCurrentFragmentId, int aThermCount, int aThermSiteCount )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
  assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
  return mLocalFragmentTable[ aCurrentFragmentId ].mThermostatMass[ aThermCount ][ aThermSiteCount ];
  }


inline
void
SetFragmentKineticEnergy( int aCurrentFragmentId, double &aKineticEnergy )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  mLocalFragmentTable[ aCurrentFragmentId ].mFragmentKineticEnergy = aKineticEnergy;
  }

inline
double
GetFragmentKineticEnergy( int aCurrentFragmentId )
  {
  assert( aCurrentFragmentId < NUMBER_OF_FRAGMENTS );
  return mLocalFragmentTable[ aCurrentFragmentId ].mFragmentKineticEnergy;
  }

inline
double
GetFragmentCOMKineticEnergy( int aCurrentFragmentId )
  {
  return mLocalFragmentTable[ aCurrentFragmentId ].mCenterOfMassKineticEnergy;
  }

inline
void
SetFragmentCOMKineticEnergy( int aCurrentFragmentId, double &aEnergy )
  {
  mLocalFragmentTable[ aCurrentFragmentId ].mCenterOfMassKineticEnergy = aEnergy;
  }


#if MSDDEF_DoPressureControl
inline
double
GetPistonDKT( int aCount )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  return mDKTPiston[ aCount ];
  }

inline
double
GetPistonMass()
  {
  return RTG.mPressureControlPistonMass;
  }

inline
double
GetPistonKineticEnergy()
  {
  return mPistonKineticEnergy;
  }

inline
void
SetPistonKineticEnergy( double aEnergy )
  {
  mPistonKineticEnergy = aEnergy;
  return;
  }

inline
void
SetPistonDKT( int aCount, double aDkt )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  mDKTPiston[ aCount ] = aDkt;
  }

inline
double
GetPistonThermostatMass(int aCount )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  return mPistonThermostatMass[ aCount ];
  }

inline
double
GetPistonThermostatForce(int aCount )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  return mPistonThermostatForce[ aCount ];
  }

inline
double
GetPistonThermostatVelocity(int aCount )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  return mPistonThermostatVelocity[ aCount ];
  }

inline
double
GetPistonThermostatPosition(int aCount )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  return mPistonThermostatPosition[ aCount ];
  }


  inline
  void
  SetPistonThermostatMass(int aCount, double aMass )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  mPistonThermostatMass[ aCount ] = aMass;
  }

inline
void
SetPistonThermostatForce(int aCount, double aForce )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  mPistonThermostatForce[ aCount ] = aForce;
  }

inline
void
SetPistonThermostatVelocity(int aCount, double aVel )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  mPistonThermostatVelocity[ aCount ] = aVel;
  }

inline
void
SetPistonThermostatPosition(int aCount, double aPos )
  {
  assert( aCount < NUMBER_OF_SITES_PER_THERMOSTAT );
  mPistonThermostatPosition[ aCount ] = aPos;
  }

#endif
#endif

};

#if !defined(LEAVE_SYMS_EXTERN)
  TraceClient TDynamicVariableManagerTracer::mGuardZoneViolationSignalReduceStart;
  TraceClient TDynamicVariableManagerTracer::mGuardZoneViolationSignalReduceFinis;

  TraceClient TDynamicVariableManagerTracer::mFragmentMigrationStart;
  TraceClient TDynamicVariableManagerTracer::mFragmentMigrationFinis;

  TraceClient TDynamicVariableManagerTracer::mReduceForcesStart ;
  TraceClient TDynamicVariableManagerTracer::mReduceForcesFinis ;

  TraceClient TDynamicVariableManagerTracer::mGlobalizePositionsStart ;
  TraceClient TDynamicVariableManagerTracer::mGlobalizePositionsFinis ;

  TraceClient TDynamicVariableManagerTracer::mFloatingPointAllReduceStart;
  TraceClient TDynamicVariableManagerTracer::mFloatingPointAllReduceFinis;
#endif
#endif
