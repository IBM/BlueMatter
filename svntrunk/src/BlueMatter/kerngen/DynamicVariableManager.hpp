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

#ifndef __DYNAMIC_VAIRABLE_MANAGER_HPP__
#define __DYNAMIC_VAIRABLE_MANAGER_HPP__

#ifndef PKFXLOG_ROLLING_REG_DEBUG
#define PKFXLOG_ROLLING_REG_DEBUG ( 0 )
#endif

#ifndef PKFXLOG_CHECK_SITE_DISP
#define PKFXLOG_CHECK_SITE_DISP ( 0 )
#endif

#ifndef PKFXLOG_CHASEHANG
#define PKFXLOG_CHASEHANG ( 0 )
#endif

#ifndef PKFXLOG_SETUP
#define PKFXLOG_SETUP ( Platform::Topology::GetAddressSpaceId() == 0 )
#endif

#ifndef PKFXLOG_VERLET_LIST_DEBUG
#define PKFXLOG_VERLET_LIST_DEBUG ( 0 )
#endif

#ifndef PKFXLOG_EXPDYNVAR
#define PKFXLOG_EXPDYNVAR ( 0 )
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

#include <BlueMatter/ExternalDatagram.hpp>
#include <pk/platform.hpp>
#include <BlueMatter/MSD_IF.hpp>
#include <pk/pktrace.hpp>   // Needed by the pk tracing instrumentation
#include <BlueMatter/Tag.hpp>
#include <BlueMatter/DirectMDVM.hpp>

inline
void
NearestImageInPeriodicVolume(const XYZ &PositionA, const XYZ &PositionB, XYZ &Nearest);

#if ( MSDDEF_NOSE_HOOVER && MSDDEF_DoPressureControl )
#include <BlueMatter/DirectPistonMDVM.hpp>
#endif

class KspaceDummy {};
typedef KspaceDummy KSpaceCommMIF;

struct tVerletSiteInfo
{
    FragId fragid;   // fragment id
    int    siteind;  // site offset in fragment
    int    siteused; // this is for p3me caching, set in AssignCharge, checked in InterpolateFields
    double charge;   // charge of the site
};


#define NOT_NEAREST_NEIGBOR -9999

//////////////////////////////////////////////////////////////////////////
struct BitValue
{
  int mValue;
};

typedef Platform::Clock::TimeValue TimeValue;


struct Neighbor
{
  int mX;
  int mY;
  int mZ;
  int mRank;
};

typedef unsigned int ExclusionMask_T;
#define EXCLUSION_MASK_NOT_DEFINED     0xFFFFFFFF
#define PAIR_NOT_FOUND                 ( -1 )

struct ORBVoxelInput
  {
  XYZ    mPoint;
  double mWeight;
  };

struct FragmentVerletPair
  {
  unsigned short    mTargetFragmentId;
  unsigned short    mSourceFragmentId;
  unsigned short    mFragmentImageVectorIndex; // 0 -> 125 range
  unsigned short    mNotActiveCounter;
  ExclusionMask_T   mExclusionMask;
  };


struct FragWeight
  {
  double         mWeight;
  unsigned short mTargetId;
  unsigned short mSourceId;
  unsigned int   mChecksum;
  };


static int CompareFragWeight(const void *elem1, const void *elem2)
  {
  FragWeight* A = ( FragWeight *) elem1;
  FragWeight* B = ( FragWeight *) elem2;
  if (A->mTargetId < B->mTargetId )return -1;
  else if (A->mTargetId == B->mTargetId)
    {
    if (A->mSourceId < B->mSourceId )return -1;
    else if ( A->mSourceId == B->mSourceId )
      { return 0; }
    else return 1;
    }
  else return 1;
  }

inline int ComparePair( unsigned short a, unsigned short b,
                        unsigned short x, unsigned short y )
    {
    if( a < x )
      return -1;
    else if( a == x )
      {
      if( b < y )
        return -1;
      else if( b == y )
        return 0;
      else
        return 1;
      }
    else
      return 1;
    }

struct InRangeNodePair
  {
  unsigned short mNodeIndexA;
  unsigned short mNodeIndexB;
  };

struct FragmentInRangePair
  {
  XYZ            mFragmentCenter;
  double         mExtent;
  unsigned short mFragId;
  unsigned short mVoxelOrdinal;
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

#define VOXELS_MIDPOINT_NOT_HERE              0
#define VOXELS_MIDPOINT_HERE                  1
#define VOXELS_WITHIN_CUTOFF                  2
#define PATIENCE_COUNT     5

template<int SITE_COUNT, int FRAGMENT_COUNT, int RESPA_LEVEL_COUNT>
class TDynamicVariableManager : public MDVM_BaseIF
  {
  public:

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


#ifndef NOT_TIME_REVERSIBLE
  double   mExpVelVolRatio ; // recycled for velocity/volume updates
#endif
  TraceClient mFragmentMigrationStart;
  TraceClient mFragmentMigrationFinis;

  TraceClient mReduceForcesStart ;
  TraceClient mReduceForcesFinis ;

  TraceClient mGlobalizePositionsStart ;
  TraceClient mGlobalizePositionsFinis ;

//   TraceClient mIntegerAllReduceStart ;
//   TraceClient mIntegerAllReduceFinis ;

  TraceClient mFatCompPrecomputeStart;
  TraceClient mFatCompPrecomputeFinis;

  TraceClient mFatCompVerletListGenStart;
  TraceClient mFatCompVerletListGenFinis;

  TraceClient mFatCompProcessPairStart;
  TraceClient mFatCompProcessPairFinis;

  TraceClient mFloatingPointAllReduceStart;
  TraceClient mFloatingPointAllReduceFinis;

  TraceClient mDebugP5CommStart;
  TraceClient mDebugP5CommFinis;

  int            mNumberOfRealSpaceCalls ;
  int            mSampleFragmentTime ;

  int mMaxSitesInFragment;
  double mMaxExtent;

  int mSizeOfSDBuffers;

  char** mSendBufferSD MEMORY_ALIGN( 5 );
  char** mRecvBufferSD MEMORY_ALIGN( 5 );

#ifdef PK_MPI_ALL
  MPI_Request* mAllRequestsSD;
  MPI_Status*  mAllStatusSD ;
#endif

  /**
   * Map FragmentId to AddressSpaceId. This is developed by the master node and distributed to the others
   * All nodes then use this list to develop the mask
   **/
  unsigned int mPartitionList[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);
  unsigned char mFragmentInRange[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);
  FragmentInRangePair mLocalFragments[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);
  int          mLocalFragmentsSize;

  unsigned int mSiteHome[ SITE_COUNT ] MEMORY_ALIGN(5);
  unsigned int mSiteIdToFragmentMap[ SITE_COUNT ] MEMORY_ALIGN(5);
  TimeValue mTimeToRunNsqInnerLoop[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);

  int    mAddressSpaceId ;

  double mCurrentVerletGuardZone;
  double mPrevGuardZone;

  double mLocalDim_x;
  double mLocalDim_y;
  double mLocalDim_z;

  int mMyP_x, mMyP_y, mMyP_z;
  int mProcs_x, mProcs_y, mProcs_z;

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

    XYZ mImagedFragmentPosition;

#if MSDDEF_NOSE_HOOVER
    double mThermostatPosition[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mThermostatVelocity[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mThermostatMass[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mThermostatForce[ NHC::NUMBER_OF_THERMOSTATS ][ NUMBER_OF_SITES_PER_THERMOSTAT ] MEMORY_ALIGN(5);
    double mFragmentKineticEnergy ;
#endif

#ifdef USE_DYNAMIC_EXTENTS
    double mExtent  ;
#endif

#ifdef DVM_TRACK_EXTENT
    // These vars will be used to do fragment fragment cut-off based on a dynamicaly monitored geometric extent.
    double mGeometricExtent ;
    XYZ    mCenterOfGeometry ;
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

  FragmentRecord mLocalFragmentTable[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);

  int            mLocalFragmentIndexNext;

  XYZ     mPositStart[ SITE_COUNT ] MEMORY_ALIGN(5);
  double  isViolation[ 256 ] MEMORY_ALIGN(5);
  double  isViolationTemp[256] MEMORY_ALIGN(5);

public:

  double mFragTimes      [ FRAGMENT_COUNT ] MEMORY_ALIGN( 5 );
  XYZ mPosit    [ SITE_COUNT ] ;// MEMORY_ALIGN( 5 );
  XYZ mVelocity [ SITE_COUNT ] MEMORY_ALIGN( 5 );
  XYZ mForce    [ RESPA_LEVEL_COUNT ] [ SITE_COUNT ] MEMORY_ALIGN( 5 );

#ifdef ROLLING_POINT_REGRESSION
  XYZ mPositP5    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
  XYZ mVelocityP5 [ SITE_COUNT ] MEMORY_ALIGN( 5 );
  XYZ mForceP5    [ SITE_COUNT ] MEMORY_ALIGN( 5 );

  XYZ mVelocityTemp    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
    
  int mConnectionEstablished;
    
  int                     mServerSocket;
  struct sockaddr_in      mAddress;
  int                     mSocket;      
#endif      

  XYZ   mImageVectorTable[ 125 ] MEMORY_ALIGN( 5 );

  int*  mNearestNeighbors MEMORY_ALIGN( 5 );
  int   mNumberOfNearestNeighbors;
  int   mPreNumberOfNearestNeighbors;

  int*  mRankToNearestNeighborMap;

  inline
  unsigned int
  GetFragIdForSiteId( SiteId aSiteId )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    unsigned int rc = mSiteIdToFragmentMap[ aSiteId ];
    return ( rc );        
    }    


  double GetCurrentVerletGuardZone()
    {
    return mCurrentVerletGuardZone;
    }

  void SetCurrentVerletGuardZone( double aZone )
    {
    mCurrentVerletGuardZone = aZone;
    return;
    }

  void SetMyOrbBox()
    {
    int xMy, yMy, zMy;
    Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
    ORBNode* MyNode = mRecBisTree->NodeFromProcCoord( xMy, yMy, zMy );
    myLow = MyNode->low;
    myHigh = MyNode->high;
    }

  void
  DumpNoseHooverState(int aTimeStep)
    {
#if 0
#if MSDDEF_NOSE_HOOVER
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
#endif
    }

#define SOCKET_ERROR (-1)
void error_exit(char *str) { fprintf(stderr,"\n%s\n",str); fprintf(stderr,"errno %d\n ", errno); fflush(stderr); exit(-1); }

void
ReceivePosVelFromP5( int aSimTick )
  {  
#ifdef ROLLING_POINT_REGRESSION
  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Entering ReceivePositionsFromP5"
    << EndLogLine;

  if( mAddressSpaceId == 0 )
    {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ( &Platform::Comm::mRemoteAddr, sizeof( Platform::Comm::mRemoteAddr ) );
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
        
    mConnectionEstablished=1;
    
    int TotalDataRead = 0;
    int DataToRead = sizeof( mPosit );
    while( TotalDataRead < DataToRead )
      {
      int data_read = read( sockfd, &(((char *) mPosit)[ TotalDataRead ]), DataToRead - TotalDataRead );
      TotalDataRead += data_read;
      }

    TotalDataRead = 0;
    DataToRead = sizeof( mVelocity );
    while( TotalDataRead < DataToRead )
      {
      int data_read = read( sockfd, &(((char *) mVelocity)[ TotalDataRead ]), DataToRead - TotalDataRead );
      TotalDataRead += data_read;
      }
    }

  for( int i = 0; i < SITE_COUNT; i++ )
    {
      BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
	<< aSimTick 
	<< " mPosit[ " << i << "]: " << mPosit[ i ]
	<< " mVelocity[ " << i << "]: " << mVelocity[ i ]
	<< EndLogLine;
	
    }

  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Leaving ReceivePositionsFromP5"
    << EndLogLine;
#endif
  }

void
ReceiveAndRegressPosVelFromP5( int aSimTick )
  {
#ifdef ROLLING_POINT_REGRESSION
  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Entering ReceiveAndRegressPosVelFromP5"
    << EndLogLine;

  assert( mConnectionEstablished );
  assert( sockfd != -1 );

  mDebugP5CommStart.HitOE( 1,
                           "mDebugP5CommStart",
                           mAddressSpaceId,
                           mDebugP5CommStart );
  
   
  int TotalDataRead = 0;
  int DataToRead = sizeof( mPositP5 );
  while( TotalDataRead < DataToRead )
    {
    int data_read = read( sockfd, &(((char *) mPositP5)[ TotalDataRead ]), DataToRead - TotalDataRead );
    TotalDataRead += data_read;
    }

  TotalDataRead = 0;
  DataToRead = sizeof( mVelocityP5 );
  while( TotalDataRead < DataToRead )
    {
    int data_read = read( sockfd, &(((char *) mVelocityP5)[ TotalDataRead ]), DataToRead - TotalDataRead );
    TotalDataRead += data_read;
    }


  for( int i = 0; i < SITE_COUNT; i++ )
    {
      BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
	<< aSimTick 
	<< " mPositP5[ " << i << "]: " << mPositP5[ i ]
	<< " mVelocityP5[ " << i << "]: " << mVelocityP5[ i ]
	<< EndLogLine;
	
    }

  mDebugP5CommFinis.HitOE( 1,
                           "mDebugP5CommFinis",
                           mAddressSpaceId,
                           mDebugP5CommFinis );

  // double tolerance = 10E-14;
  // double toleranceF = 10E-11;

  static double MaxToleranceP = 0;
  double toleranceP       = ( MaxToleranceP > 10E-11 ) ? 10E-11 : MaxToleranceP;

  static double MaxToleranceV = 0;
  double toleranceV       = ( MaxToleranceV > 10E-11 ) ? 10E-11 : MaxToleranceV;

  int FirstDiffP = 0;
  for( int i=0; i < SITE_COUNT; i++ )
    {    
    // Need to image both Posit and PositP5
    XYZ ImagedPosit;
    XYZ ImagedPositP5;
    NearestImageInPeriodicVolume( mCenterOfBoundingBox, mPosit[ i ], ImagedPosit );
    NearestImageInPeriodicVolume( mCenterOfBoundingBox, mPositP5[ i ], ImagedPositP5 );

    double diff_X = fabs( ImagedPositP5.mX - ImagedPosit.mX );

    if( diff_X > toleranceP )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Positions DO NOT agree "
        << " mPosit[ " << i << " ]= " << ImagedPosit
        << " mPositP5[ " << i << " ]= " << ImagedPositP5
        << " diff_X: " << FormatString("%16.14f") << diff_X
        << " tol: " << FormatString("%16.14f") << toleranceP
        << EndLogLine;
      FirstDiffP = 1;
      }      

    if( diff_X > MaxToleranceP )
        MaxToleranceP = diff_X;

    double diff_Y = fabs( ImagedPositP5.mY - ImagedPosit.mY );
    if( diff_Y > toleranceP )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Positions DO NOT agree "
        << " mPosit[ " << i << " ]= " << ImagedPosit
        << " mPositP5[ " << i << " ]= " << ImagedPositP5
        << " diff_Y: " << FormatString("%16.14f") << diff_Y
        << " tol: " << FormatString("%16.14f") << toleranceP
        << EndLogLine;
      FirstDiffP = 1;
      }

    if( diff_Y > MaxToleranceP )
        MaxToleranceP = diff_Y;

    double diff_Z = fabs( ImagedPositP5.mZ - ImagedPosit.mZ );
    if( diff_Z > toleranceP )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Positions DO NOT agree "
        << " mPosit[ " << i << " ]= " << ImagedPosit
        << " mPositP5[ " << i << " ]= " << ImagedPositP5
        << " diff_Z: " << FormatString("%16.14f") << diff_Z
        << " tol: " << FormatString("%16.14f") << toleranceP
        << EndLogLine;
      FirstDiffP = 1;
      }

    if( diff_Z > MaxToleranceP )
        MaxToleranceP = diff_Z;
    
//     if( FirstDiffP )
//       break;
    
    // Overwrite P1.5 values with P5 values
    mPosit[i] = mPositP5[i];
    }

  int FirstDiffV = 0;
  for( int i=0; i < SITE_COUNT; i++ )
    {
    double diff_X = fabs( mVelocityP5[ i ].mX - mVelocity[ i ].mX );
    if( diff_X > toleranceV )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Velocities DO NOT agree "
        << " mVelocity[ " << i << " ]= " << mVelocity[i]
        << " mVelocityP5[ " << i << " ]= " << mVelocityP5[i]
        << " diff_X: " << FormatString("%16.14f") << diff_X
        << " tol: " << FormatString("%16.14f") << toleranceV
        << EndLogLine;
      FirstDiffV = 1;
      }

    if( diff_X > MaxToleranceV )
        MaxToleranceV = diff_X;

    double diff_Y = fabs( mVelocityP5[ i ].mY - mVelocity[ i ].mY );
    if( diff_Y > toleranceV )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Velocities DO NOT agree "
        << " mVelocity[ " << i << " ]= " << mVelocity[i]
        << " mVelocityP5[ " << i << " ]= " << mVelocityP5[i]
        << " diff_Y: " << FormatString("%16.14f") << diff_Y
        << " tol: " << FormatString("%16.14f") << toleranceV
        << EndLogLine;
      FirstDiffV = 1;
      }

    if( diff_Y > MaxToleranceV )
        MaxToleranceV = diff_Y;

    double diff_Z = fabs( mVelocityP5[ i ].mZ - mVelocity[ i ].mZ );
    if( diff_Z > toleranceV )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Velocities DO NOT agree "
        << " mVelocity[ " << i << " ]= " << mVelocity[i]
        << " mVelocityP5[ " << i << " ]= " << mVelocityP5[i]
        << " diff_Z: " << FormatString("%16.14f") << diff_Z
        << " tol: " << FormatString("%16.14f") << toleranceV
        << EndLogLine;
      FirstDiffV = 1;
      }
    
    if( diff_Z > MaxToleranceV )
        MaxToleranceV = diff_Z;

//     if( FirstDiffV )
//       break;

    // Overwrite P1.5 values with P5 values
    mVelocity[i] = mVelocityP5[i];
    }


    if( FirstDiffP || FirstDiffV )
      {  
      // PLATFORM_ABORT("P1_5 version found significant diffs with P5 version of BlueMatter.");      
      }

  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Leaving ReceiveAndRegressPosVelFromP5"
    << EndLogLine;
  return;
#endif
  }
void
ReceiveAndRegressForceFromP5( int aSimTick )
  {
#ifdef ROLLING_POINT_REGRESSION
  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Entering ReceiveAndRegressForceFromP5"
    << EndLogLine;

  assert( mConnectionEstablished );
  assert( sockfd != -1 );

  mDebugP5CommStart.HitOE( 1,
                           "mDebugP5CommStart",
                           mAddressSpaceId,
                           mDebugP5CommStart );
     
  int TotalDataRead = 0;
  int DataToRead = sizeof( mForceP5 );
  while( TotalDataRead < DataToRead )
    {
    int data_read = read( sockfd, &(((char *) mForceP5)[ TotalDataRead ]), DataToRead - TotalDataRead );
    TotalDataRead += data_read;
    }    

  mDebugP5CommFinis.HitOE( 1,
                           "mDebugP5CommFinis",
                           mAddressSpaceId,
                           mDebugP5CommFinis );

  static double MaxToleranceF = 0;
  double toleranceF       = ( MaxToleranceF > 10E-11 ) ? 10E-11 : MaxToleranceF;

  int FirstDiffF = 0;
  for( int i=0; i < SITE_COUNT; i++ )
    {
    double diff_X = fabs( mForceP5[ i ].mX - mForce[0][ i ].mX );
    if( diff_X > toleranceF )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Force DO NOT agree "
        << " mForce[ " << i << " ]= " << mForce[0][i]
        << " mForceP5[ " << i << " ]= " << mForceP5[i]
        << " diff_X: " << FormatString("%16.14f") << diff_X
        << " tol: " << FormatString("%16.14f") << toleranceF
        << EndLogLine;
      FirstDiffF = 1;
      }

    if( diff_X > MaxToleranceF )
        MaxToleranceF = diff_X;

    double diff_Y = fabs( mForceP5[ i ].mY - mForce[0][ i ].mY );
    if( diff_Y > toleranceF )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Force DO NOT agree "
        << " mForce[ " << i << " ]= " << mForce[0][i]
        << " mForceP5[ " << i << " ]= " << mForceP5[i]
        << " diff_Y: " << FormatString("%16.14f") << diff_Y
        << " tol: " << FormatString("%16.14f") << toleranceF
        << EndLogLine;
      FirstDiffF = 1;
      }

    if( diff_Y > MaxToleranceF )
        MaxToleranceF = diff_Y;

    double diff_Z = fabs( mForceP5[ i ].mZ - mForce[0][ i ].mZ );
    if( diff_Z > toleranceF )
      {
      BegLogLine( 1 )
        << aSimTick
        << " Force DO NOT agree "
        << " mForce[ " << i << " ]= " << mForce[0][i]
        << " mForceP5[ " << i << " ]= " << mForceP5[i]
        << " diff_Z: " << FormatString("%16.14f") << diff_Z
        << " tol: " << FormatString("%16.14f") << toleranceF
        << EndLogLine;
      FirstDiffF = 1;
      }
    
    if( diff_Z > MaxToleranceF )
        MaxToleranceF = diff_Z;

    // Overwrite P1.5 values with P5 values
    mForce[0][i] = mForceP5[i];
    }

    if( FirstDiffF )
      {  
      // PLATFORM_ABORT("P1_5 version found significant diffs with P5 version of BlueMatter.");      
      }

  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Leaving ReceiveAndRegressForceFromP5"
    << EndLogLine;

  return;
#endif
  }

void
ReceiveVelocityFromP5( int aSimTick )
  {
#ifdef ROLLING_POINT_REGRESSION
  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Entering ReceiveVelocityFromP5"
    << EndLogLine;

  assert( mConnectionEstablished );
  assert( sockfd != -1 );  
   
  int TotalDataRead = 0;
  int DataToRead = sizeof( mVelocity );
  while( TotalDataRead < DataToRead )
    {
    int data_read = read( sockfd, &(((char *) mVelocity)[ TotalDataRead ]), DataToRead - TotalDataRead );
    TotalDataRead += data_read;
    }

  BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
    << aSimTick    
    << " Leaving ReceiveVelocityFromP5"
    << EndLogLine;
  return;
#endif
  }


int GetNodeForFragment( int aFragId )
  {
  assert( aFragId >= 0 && aFragId < FRAGMENT_COUNT );

  return mPartitionList[ aFragId ];
  }

int GetNumberOfLocalFragments()
  {
  int count=0;
  for( int i=0; i<FRAGMENT_COUNT; i++ )
    {
    if( mPartitionList[ i ] == mAddressSpaceId )
      count++;
    }
  return count;
  }

int GetNumberOfNodesInRange()
  {
  return mNumberOfNodesInRange;
  }

  void
  DumpForces( int aSimTick )
  {
  for( int i=0; i<SITE_COUNT; i++)
    for( int rl = 0; rl < RESPA_LEVEL_COUNT; rl++)
      BegLogLine( 1 )
        << aSimTick << " Force " << i << " " << mForce[ rl ][ i ]
        << EndLogLine;
  }

void
DumpVelocity( int aSimTick )
  {
  int numberLocal = 0;
  for( int i=0; i<FRAGMENT_COUNT; i++ )
    {
    if( mPartitionList[ i ] == mAddressSpaceId )
      numberLocal++;    
    }
  
  BegLogLine( 0 )
    << "NumberOfLocalFrags " << numberLocal
    << EndLogLine;
  
  int siteid = 2968;
  BegLogLine( 0 )
    << "mSiteHome[ " << siteid << " ] " << mSiteHome[ siteid ] 
    << EndLogLine;

  FragId ff = mSiteIdToFragmentMap[ siteid ];

  assert ( IsSiteLocal( siteid ) == IsFragmentLocal( ff ) );

  for( int i=0; i<FRAGMENT_COUNT; i++ )
    {
    int FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( i );
    int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( i );
    
    if( IsFragmentLocal( i ) )
      {
      if( i == ff )
        BegLogLine( 0 )
          << "FirstSiteId " << FirstSiteId
          << " SiteCount " << SiteCount
          << EndLogLine;
      
      for( int j=0; j<SiteCount; j++ )
        {
        SiteId siteId = ( FirstSiteId+j );

        assert( IsSiteLocal( siteId ) );

        BegLogLine( 1 )
          << aSimTick << " Velocity " << siteId << " " << mVelocity[ siteId ]
          << EndLogLine;        
        }
      }    
    }

  for( int i=0; i<SITE_COUNT; i++)
    {        
    if( IsSiteLocal( i ) )
      BegLogLine( 0 )
        << aSimTick << " Velocity " << i << " " << mVelocity[ i ]
        << EndLogLine;
    }
  }

void
DumpPositions( int aSimTick )
  {
  for( int i=0; i<SITE_COUNT; i++)
    {
    if( IsSiteLocal( i ) )
      BegLogLine( 1 )
        << aSimTick << " Position " << i << " " << GetPosition( i )
        << EndLogLine;
    } 
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
#ifdef USE_DYNAMIC_EXTENTS
  rc = mLocalFragmentTable[ aFragmentIndex ].mExtent;
#else
  rc = MSD_IF::GetFragmentExtent( aFragmentIndex );
#endif
  return( rc );
  }

void
GetDynamicFragmentExtentFragmentCenter( int     aFragmentIndex,
                                        double* aExtent,
                                        XYZ*     aXYZ )
  {
  XYZ TagAtomPosition = GetTagAtomPosition( aFragmentIndex );
  int FirstSiteId   = MSD_IF::GetIrreducibleFragmentFirstSiteId( aFragmentIndex );
  int NumberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragmentIndex );
  double DistanceSquaredFromTagSite = 0.0;

  XYZ LowerLeft = GetPosition( FirstSiteId );
  XYZ UpperRight;
  double bX = LowerLeft.mX ;
  double bY = LowerLeft.mY ;
  double bZ = LowerLeft.mZ ;
  double cX = bX ;
  double cY = bY ;
  double cZ = bZ ;

  for( int SiteId=(FirstSiteId+1); SiteId < (FirstSiteId + NumberOfSites); SiteId++ )
    {
    XYZ AtomLoc = GetPosition( SiteId );
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
    XYZ AtomLoc = GetPosition( FirstSiteId+qIndexI ) ;
    double e2 = ( AtomLoc - MidXYZ ).LengthSquared() ;
    e2max = max( e2max, e2 ) ;
    } /* endfor */

  double rc = sqrt( e2max );
  *aExtent = rc;
  }

public:

  inline
  void
  BroadcastPartitionMap()
  {
#ifdef PK_PARALLEL
  Platform::Collective::Broadcast( (char *) mPartitionList, FRAGMENT_COUNT*sizeof(int), 0  );
#endif
  }

inline void
ReduceVirial( int aSimTick, int aRespaLevel )
  {
#ifdef PK_PARALLEL
  XYZ tmpVirial[ RESPA_LEVEL_COUNT ];
  int numberOfDoublesInPackage =  3*RESPA_LEVEL_COUNT;

  Platform::Collective::FP_AllReduce_Sum( (double *)mVirial, (double *)tmpVirial, numberOfDoublesInPackage );

  memcpy( mVirial, tmpVirial, sizeof(double)*numberOfDoublesInPackage );
#endif
  }


inline void
ReduceCenterOfMassKineticEnergy( int aSimTick, int aRespaLevel )
  {
#ifdef PK_PARALLEL
  double tmpKE = 0.0;

  Platform::Collective::FP_AllReduce_Sum( (double *) &mCenterOfMassKineticEnergy, (double *) &tmpKE, 1 );

  mCenterOfMassKineticEnergy = tmpKE;
#endif
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
#ifdef DUMP_FORCES
  char buff[64];
  FILE* flPtr = fopen("forces.out","a+");
  for( int i=0; i < SITE_COUNT; i++ )
    for( int rl=0; rl <= aRespaLevel; rl++ )
      {
      XYZ force = mForce[ rl ][ i ];
      sprintf( buff,"%d %d %d %f %f %f\n", aSimTick, i, rl, force.mX, force.mY, force.mZ );
      fputs( buff, flPtr );
      }
  fclose( flPtr );
#endif

#ifdef PK_PARALLEL

  mReduceForcesStart.HitOE( PKTRACE_REDUCEFORCES,
                            "ReduceForces_Cntl",
                            mAddressSpaceId,
                            mReduceForcesStart );

  static XYZ ForceTemp    [ RESPA_LEVEL_COUNT ] [ SITE_COUNT ];

  int numberOfDoublesInPackage = RESPA_LEVEL_COUNT*SITE_COUNT * (sizeof(XYZ)/sizeof(double));

  EmitTimeStamp(aSimTick, Tag::AllReduce_Begin, 1 );

  for( int i=0; i < numberOfDoublesInPackage; i++)
    {
    BegLogLine( PKFXLOG_COLLECTIVE_FP_REDUCE )
      << "mForce[ " << i << " ] = "
      << ((double *) mForce)[ i ]
      << EndLogLine;
    }

//   mFloatingPointAllReduceStart.HitOE( PKTRACE_ALL_REDUCE_SUM,
// 				      "mFloatingPointAllReduce_Cntl",
// 				      mAddressSpaceId,
// 				      mFloatingPointAllReduceStart );
  
  Platform::Collective::FP_AllReduce_Sum( (double *) mForce, (double *)ForceTemp, numberOfDoublesInPackage );

//    mFloatingPointAllReduceFinis.HitOE( PKTRACE_ALL_REDUCE_SUM,
//                                        "mFloatingPointAllReduce_Cntl",
//                                        mAddressSpaceId,
//                                        mFloatingPointAllReduceFinis );

  for( int i=0; i < numberOfDoublesInPackage; i++)
    {
    BegLogLine( PKFXLOG_COLLECTIVE_FP_REDUCE )
      << "ForceTemp[ " << i << " ] = "
      << ((double *) ForceTemp)[ i ]
      << EndLogLine;
    }

  memcpy( mForce, ForceTemp, numberOfDoublesInPackage * sizeof( double ) );

  EmitTimeStamp(aSimTick, Tag::AllReduce_End, 1);

  mReduceForcesFinis.HitOE(PKTRACE_REDUCEFORCES,
                           "ReduceForces_Cntl",
                           mAddressSpaceId,
                           mReduceForcesFinis );
#endif
  return;
  }

  inline
  void
  AddForce( int                aAbsoluteSiteId,
            int                 aTimeStep,
            int                 aIntegratorLevel,
            const XYZ          & aForce                 )
  {
  //NEED: might need to wait for force register to be ready here.
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  assert( aIntegratorLevel >= 0 && aIntegratorLevel < NUMBER_OF_RESPA_LEVELS );

  mForce [ aIntegratorLevel ] [ aAbsoluteSiteId ] += aForce;
  }

inline
void
AddKSpaceTotalForces( int aSimTick, int aRespaLevel )
  {
  for(int i=0; i < SITE_COUNT; i++)
    {
    AddForce( i, aSimTick, aRespaLevel, mKSpaceForce[ i ] );
    }
  }

/// This call is meant to be called once in the setup phase
inline
void
GlobalizePositionsVelocities( int aSimTick, int aRespaLevel )
  {
#ifdef PK_PARALLEL
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

#endif
  return;
  }

inline
unsigned
IsSiteLocal( int aAbsSiteId )
  {
  assert( aAbsSiteId >= 0 && aAbsSiteId < SITE_COUNT );

  unsigned rc = ( mSiteHome[ aAbsSiteId ] == mAddressSpaceId );
  return ( rc );
  }

int
CheckSiteDisplacement()
  {
  int rc = 0;

  double HalfGuardZone = mCurrentVerletGuardZone / 2.0;

  double HalfGuardZoneSquared = HalfGuardZone * HalfGuardZone;

  for( int s = 0; s < SITE_COUNT; s++ )
    {
    if ( !IsSiteLocal( s ) )
      continue;

    double DistanceSquared = mPositStart[ s ].DistanceSquared( GetPosition( s ) );
    if( DistanceSquared >= HalfGuardZoneSquared )
      {
      rc = 1;
      break;
      }
    }
  return( rc );
  }

  void
  DumpLocalForces( int aSimTick )
    {
    for( int i=0; i<SITE_COUNT; i++)
      if ( IsSiteLocal ( i )  )
        for( int rl = 0; rl < RESPA_LEVEL_COUNT; rl++)
          BegLogLine( PKFXLOG_DUMP_FORCE )
            << aSimTick << " Force[" << rl << "][" << i << "]=" << mForce[ rl ][ i ]
            << EndLogLine;
    }

  void
  DumpLocalVelocity( int aSimTick )
    {
    for( int i=0; i<SITE_COUNT; i++)
      if ( IsSiteLocal ( i )  )
        BegLogLine( PKFXLOG_DUMP_LOCAL_VELOCITY )
          << aSimTick << " Velocity[ " << i << " ]=" << mVelocity[ i ]
          << EndLogLine;
    }

  void
  DumpLocalPositions( int aSimTick )
    {
    for( int i=0; i<SITE_COUNT; i++)
      if ( IsSiteLocal ( i )  )
        BegLogLine( PKFXLOG_DUMP_POSITIONS )
          << aSimTick << " Positions[" << i << "]=" << GetPosition( i )
          << EndLogLine;
    }


inline
void
ZeroNonLocalPositions()
  {
  for(int i=0; i<SITE_COUNT; i++)
    {
    if( IsSiteLocal( i ) )
      continue;

    mPosit[ i ].Zero();
    }
  }

inline
void
InvalidateNonLocalPositions()
  {
  for(int i=0; i<SITE_COUNT; i++)
    {
    if( IsSiteLocal( i ) )
      continue;

    mPosit[ i ].Invalidate();
    }
  }

inline
void
ZeroNonLocalVelocities()
  {
  for(int i=0; i<SITE_COUNT; i++)
    {
    if( IsSiteLocal( i ) )
      continue;

    mVelocity[ i ].Zero();
    }
  }

void
UpdateDisplacementForFragmentVerletListMonitoring()
//UpdateDisplacementStartPosit()
  {
  for( int s = 0; s < SITE_COUNT; s++ )
    {
    if( IsSiteLocal( s ) )
      mPositStart[ s ] = GetPosition( s );
    }

  return;
  }

inline
void
GlobalizeDynamicStateAfterLoadBalancing ( int aSimTick, int aRespaLevel )
  {
  #ifdef PK_PARALLEL
    // Globalize Velocities
    // Assume phase 1.5 or phase 2
    // First OR reduce, then broadcast
    int numberOfDoublesInPackage = SITE_COUNT * (sizeof(XYZ)/sizeof(double));
    ZeroNonLocalVelocities();

    #ifdef PK_BLADE
     Platform::Collective::AllReduce_Sum( (char *) mVelocity, sizeof( mVelocity ) );
    #else
      static XYZ velTemp [ SITE_COUNT ];
      MPI_Allreduce( mVelocity , velTemp, numberOfDoublesInPackage, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      memcpy(mVelocity, velTemp, numberOfDoublesInPackage*sizeof(double));
    #endif

    // Globalize CenterOfMassForce
    #if MSDDEF_DoPressureControl
      for( int j=0; j < FRAGMENT_COUNT; j++ )
        {
        if( !IsFragmentLocal( j ) )
          {
          for( int i=0; i < RESPA_LEVEL_COUNT; i++ )
            mCenterOfMassForce[ i ][ j ].Zero() ;
          }
        }

      #ifdef PK_BLADE
        Platform::Collective::AllReduce_Sum( (char *) mCenterOfMassForce, sizeof( mCenterOfMassForce ) );
      #else
        static XYZ mCenterOfMassForceTemp [ RESPA_LEVEL_COUNT ][ FRAGMENT_COUNT ];

        int nd1 = sizeof( mCenterOfMassForce ) / sizeof(double);

        MPI_Allreduce( mCenterOfMassForce, mCenterOfMassForceTemp, nd1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        memcpy( mCenterOfMassForce, mCenterOfMassForceTemp, nd1*sizeof(double));
      #endif
    #endif

    #if ( MSDDEF_DoPressureControl || MSDDEF_NOSE_HOOVER )
      for( int j=0; j < FRAGMENT_COUNT; j++ )
        {
        if( !IsFragmentLocal( j ) )
          {
          mLocalFragmentTable[ j ].ZeroState();
          }
        }

      #ifdef PK_BLADE
        Platform::Collective::AllReduce_Sum( (char *) mLocalFragmentTable, sizeof( mLocalFragmentTable ) );
      #else
        static FragmentRecord mLocalFragmentTableTemp[ FRAGMENT_COUNT ];
        int nd2 = sizeof( mLocalFragmentTable ) / sizeof(double);

        MPI_Allreduce( mLocalFragmentTable, mLocalFragmentTableTemp, nd2, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        memcpy( mLocalFragmentTable, mLocalFragmentTableTemp, nd2*sizeof(double));
      #endif
    #endif

    #if MSDDEF_NOSE_HOOVER
      for( int j=0; j < FRAGMENT_COUNT; j++ )
        {
        if( !IsFragmentLocal( j ) )
          {
          for( int k=0; k < NUMBER_OF_SITES_PER_THERMOSTAT; k++ )
            {
            RTG.mDKT[j][0][k] = 0.0;
            RTG.mDKT[j][1][k] = 0.0;
            }
          }
        }

      #ifdef PK_BLADE
        Platform::Collective::AllReduce_Sum( (char *) RTG.mDKT, sizeof( RTG.mDKT ) );
      #else
        static  double DKT_temp[ NUMBER_OF_FRAGMENTS ][ 2 ][ NUMBER_OF_SITES_PER_THERMOSTAT ];

        int nd3 = sizeof( RTG.mDKT ) / sizeof(double);

        MPI_Allreduce( RTG.mDKT, DKT_temp, nd3, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        memcpy( RTG.mDKT, DKT_temp, nd3*sizeof(double) );
      #endif
    #endif // NOSE_HOOVER
  #endif
  return;
  }

  inline
  void
  GlobalizePositions ( int aSimTick, int aRespaLevel )
  {
  #ifdef PK_PARALLEL
    mGlobalizePositionsStart.HitOE( PKTRACE_GLOBALIZEPOSITIONS,
                                    "GlobalizePositions_Cntl",
                                    mAddressSpaceId,
                                    mGlobalizePositionsStart );

    // Assume phase 1.5 or phase 2
    // First OR reduce, then broadcast
    static XYZ positionsTemp [ SITE_COUNT ];
    int numberOfDoublesInPackage = SITE_COUNT * (sizeof(XYZ)/sizeof(double));
    ZeroNonLocalPositions();
    
    BegLogLine( PKFXLOG_GLOBALIZEPOSITIONS )
      << " GlobalizePosition "
      << EndLogLine;


    //         mIntegerAllReduceStart.HitOE( PKTRACE_ALL_REDUCE_SUM,
    //                                       "mIntegerAllReduce_Cntl",
    //                                       mAddressSpaceId,
    //                                       mIntegerAllReduceStart );
      
    // BGF Change sig so we might get to use tree on bgl system software 
    // AND avoid overhead of FP reduce verus int
    // NOTE: that means twice as many elements to reduce.
    
    Platform::Collective::FP_AllReduce_Sum( (double *) &(mPosit[ 0 ]), 
					    (double *) &(positionsTemp[ 0 ]), 
					    numberOfDoublesInPackage );
      
    //         mIntegerAllReduceFinis.HitOE( PKTRACE_ALL_REDUCE_SUM,
    //                                       "mIntegerAllReduce_Cntl",
    //                                       mAddressSpaceId,
    //                                       mIntegerAllReduceFinis );
      
    memcpy(mPosit, positionsTemp, numberOfDoublesInPackage*sizeof(double));

    mGlobalizePositionsFinis.HitOE( PKTRACE_GLOBALIZEPOSITIONS,
				    "GlobalizePositions_Cntl",
				    mAddressSpaceId,
				    mGlobalizePositionsFinis );
      
      
    DumpLocalPositions( aSimTick );

  #endif
  return;
  }

inline
void
GetProcCoordsForFragment( int aFragId, int* aX, int* aY, int* aZ )
  {
  XYZ tempCog = GetCenterOfGeometry( aFragId );

  double BoundingBoxDim_x = RTG.mStartingBoundingBoxDimensionVector.mX;
  double BoundingBoxDim_y = RTG.mStartingBoundingBoxDimensionVector.mY;
  double BoundingBoxDim_z = RTG.mStartingBoundingBoxDimensionVector.mZ;

  XYZ CenterOfBoundingBox;
  CenterOfBoundingBox.mX = BoundingBoxDim_x / 2.0;
  CenterOfBoundingBox.mY = BoundingBoxDim_y / 2.0;
  CenterOfBoundingBox.mZ = BoundingBoxDim_z / 2.0;

  int procs_x = mProcs_x;
  int procs_y = mProcs_y;
  int procs_z = mProcs_z;

  double LocalDim_x = mLocalDim_x;
  double LocalDim_y = mLocalDim_y;
  double LocalDim_z = mLocalDim_z;

  XYZ cog;
  NearestImageInPeriodicVolume( CenterOfBoundingBox , tempCog, cog );

  *aX = (int) ( floor( cog.mX / LocalDim_x ) ) % procs_x;
  *aY = (int) ( floor( cog.mY / LocalDim_y ) ) % procs_y;
  *aZ = (int) ( floor( cog.mZ / LocalDim_z ) ) % procs_z;
  }

int GetNearestNeighborIndexForRank( unsigned aRank )
  {
  assert( aRank < (mProcs_x * mProcs_y * mProcs_z) );
  return mRankToNearestNeighborMap[ aRank ];
  }

inline
XYZ
GetCenterOfGeometry( int aFragmentId )
  {
  XYZ cog;
  cog.Zero();

  int firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( aFragmentId );
  int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragmentId );

  for( int siteId=firstSiteId; siteId < (firstSiteId + numberOfSites); siteId++ )
    {
    cog += GetPosition( siteId );
    }

  cog = cog / numberOfSites;

  return cog;
  }

void GetImagedFragCenter( int aFragId, XYZ& aImaged )
  {
  // XYZ tempCog = GetCenterOfGeometry( aFragId );
  XYZ tempCog = GetTagAtomPosition( aFragId );

  NearestImageInPeriodicVolume( mCenterOfBoundingBox , tempCog, aImaged );
  }

int GetVoxelIndexForFragment( int aFragId )
  {
  XYZ TagAtomPos = GetTagAtomPosition( aFragId );
  int VoxelIndex =  GetVoxelIndexForPosition( TagAtomPos );
  return VoxelIndex;
  }

int GetVoxelIndexForPosition( XYZ aPos )
  {
  double Pos[ 3 ];
  Pos[ 0 ] = aPos.mX;
  Pos[ 1 ] = aPos.mY;
  Pos[ 2 ] = aPos.mZ;

  int VoxelIndex = mRecBisTree->VoxelIndex( Pos );
  return VoxelIndex;
  }

int GetVoxelIdForPosition( XYZ aPos )
  {
  double Pos[ 3 ];
  Pos[ 0 ] = aPos.mX;
  Pos[ 1 ] = aPos.mY;
  Pos[ 2 ] = aPos.mZ;

  int VoxelId = mRecBisTree->VoxelId( Pos );
  return VoxelId;
  }

  int
  CheckSiteDisplacementViolation()
  {
  BegLogLine( PKFXLOG_CHECK_SITE_DISP )
    << "CheckSiteDisplacementViolation():: "
    << " localViolation = " << (int) isViolation
    << EndLogLine;

    return CheckSiteDisplacement();
  }

  inline
  void
  ReduceFragmentTime()
  {
  #ifdef PK_PARALLEL
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
  #endif
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
void
AddTimeForFragment( int aFragmentId, double aTimeValue )
  {
  assert( aFragmentId >= 0 && aFragmentId < FRAGMENT_COUNT );

  mFragTimes[ aFragmentId ] += aTimeValue;

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
DoNthIrreducibleFragmentOneTimeInit( int aLocalFragmentId )
  {
  int Start = GetNthIrreducibleFragmentFirstSiteIndex( aLocalFragmentId );
  int End   = Start + GetNthIrreducibleFragmentSiteCount( aLocalFragmentId );
  double Mass = 0;
  for (int i=Start; i<End; i++)
    {
    Mass += MSD_IF::GetMass( i );
    }
  mLocalFragmentTable[ aLocalFragmentId ].mCenterOfMassMass            = Mass;
  mLocalFragmentTable[ aLocalFragmentId ].mCenterOfMassHalfInverseMass = 0.5 / Mass;
  }

inline
int
GetIrreducibleFragmentCount()
  {
  // THIS IS AMBIGIOUS - DOES THIS CALL MEAN LOCAL OR GLOBAL FRAG COUNT????  BGF
  int rc = MSD_IF::GetIrreducibleFragmentCount();
  return( rc );
  }

// Returns the absolute fragment number of the Nth fragment locally managed.
inline
int
GetNthIrreducibleFragmentId( int aIndex )
  {
  int rc = aIndex;
  return( rc );
  }

// Returns the absolute irreducible partition element id of the nth one manage here.
inline
int
GetNthIrreducibleFragmentSiteCount( int aLocalFragmentId )
  {
  int AbsFragmentId = GetNthIrreducibleFragmentId(  aLocalFragmentId );
  int rc = MSD_IF::GetIrreducibleFragmentMemberSiteCount( AbsFragmentId );
  return( rc );
  }

// Get the absolute site id of first site in locally managed irreducible fragment.
inline
int
GetNthIrreducibleFragmentFirstSiteIndex( int aLocalFragmentIndex )
  {
  int AbsFragmentId = GetNthIrreducibleFragmentId( aLocalFragmentIndex );
  int rc = MSD_IF::GetIrreducibleFragmentMemberSiteId( AbsFragmentId, 0 );
  return( rc );
  }

inline
int
GetNthIrreducibleFragmentNthSiteIndex( int aLocalFragmentIndex, int aSiteIndex )
  {
  int AbsFragmentId = GetNthIrreducibleFragmentId(  aLocalFragmentIndex );
  int rc = MSD_IF::GetIrreducibleFragmentMemberSiteId( AbsFragmentId, aSiteIndex );
  return( rc );
  }


inline
int
AreSitesWithinExtent( int aFragmentNumber )
  {
  int numberOfSitesInFragment = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragmentNumber );

  if( numberOfSitesInFragment == 1 )
    return 1;

  int tagAtomIndex        = MSD_IF::GetTagAtomIndex( aFragmentNumber );
  double maxExtent        = MSD_IF::GetFragmentExtent( aFragmentNumber );
  int tagAtomAbsSiteId    = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragmentNumber, tagAtomIndex );

  XYZ tagAtomPosition     = GetPosition( tagAtomAbsSiteId );

  for( int i=0; i < numberOfSitesInFragment; i++)
    {
    int absSiteId = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragmentNumber, i );
    XYZ sourcePosition = GetPosition( absSiteId );

    double distance = tagAtomPosition.Distance( sourcePosition );
    if( distance > maxExtent )
      return 0;
    }
  return 1;
  }

TDynamicVariableManager() { }

// THINK about having this take the SimTick too.

inline 
void 
InitLoadState()
  {
  return;
  }

inline
void
LoadDynamicVariables( int aAbsSiteId, XYZ aPosit, XYZ aVelocity )
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

void PrecomputeNeighborListBoundingBoxData( int all=0 )
  {

  mFatCompPrecomputeStart.HitOE( PKTRACE_FATCOMP,
                                 "mFatCompPrecompute",
                                 mAddressSpaceId,
                                 mFatCompPrecomputeStart );

  // We can run the algorithm either by precomputing the in-range fragments,
  // or the way we used to run it by iterating through all the fragments and deciding
  // about 'range' just before we call the IFP each time. It is faster to precompute,
  // but we never throw away any code ...
  NSQ_NeighbourList.Setup( ::DynVarMgrIF.mDynamicBoxDimensionVector,
                           ::DynVarMgrIF.mDynamicBoxInverseDimensionVector ) ;

  unsigned int IrreducibleFragmentCount = GetIrreducibleFragmentCount() ;
  double GuardZone = GetCurrentVerletGuardZone();

  for (int pSourceI = 0; pSourceI < IrreducibleFragmentCount; pSourceI++)
    {

    if( !all && !mInteractionTreeCreationMode )
      if( !IsFragmentInRange( pSourceI ) )
        continue;

    // int SourceFragmentId = IrreducibleFragmentOperandIF::GetIrreducibleFragmentId(mClientState.mDynVarMgrIF,pSourceI);
    int SourceFragmentId = pSourceI;
    int SourceTagSiteIndex = MSD_IF::GetTagAtomIndex( SourceFragmentId );

    double SourceFragmentExtent = MSD_IF::GetFragmentExtent( SourceFragmentId );

    int FragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceFragmentId );
    //XYZ LowerLeft = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,0) ;
    int base   = GetNthIrreducibleFragmentFirstSiteIndex( SourceFragmentId );
    XYZ LowerLeft = GetPosition( base );
    XYZ UpperRight = LowerLeft ;
    {
    double bX = LowerLeft.mX ;
    double bY = LowerLeft.mY ;
    double bZ = LowerLeft.mZ ;
    double cX = bX ;
    double cY = bY ;
    double cZ = bZ ;

    // Restructure this loop to 'nickle-and-dime' it, because in the initial implementation it represents replicated calculation on all
    // nodes. (Another day we will be more selective, and only have each node calculate for fragments it is interested in)
    for (int pIndexI = 1 ; pIndexI < FragmentSiteCount; pIndexI += 1 )
      {
      // XYZ AtomLoc = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,pIndexI) ;
      XYZ AtomLoc = GetPosition( base+pIndexI ) ;

      double dX = AtomLoc.mX ;
      double dY = AtomLoc.mY ;
      double dZ = AtomLoc.mZ ;
      bX = min(bX,dX) ;
      bY = min(bY,dY) ;
      bZ = min(bZ,dZ) ;
      cX = max(cX,dX) ;
      cY = max(cY,dY) ;
      cZ = max(cZ,dZ) ;
      } /* endfor */

    LowerLeft.mX = bX ;
    LowerLeft.mY = bY ;
    LowerLeft.mZ = bZ ;
    UpperRight.mX = cX ;
    UpperRight.mY = cY ;
    UpperRight.mZ = cZ ;
    }

    // Need to expand the bounding box to become verlet list safe.
    LowerLeft.mX -= ( 0.5 * GuardZone );
    LowerLeft.mY -= ( 0.5 * GuardZone );
    LowerLeft.mZ -= ( 0.5 * GuardZone );
    UpperRight.mX += ( 0.5 * GuardZone );
    UpperRight.mY += ( 0.5 * GuardZone );
    UpperRight.mZ += ( 0.5 * GuardZone );

    XYZ MidXYZ = 0.5*(LowerLeft+UpperRight) ;
    double e2max = DBL_MIN ; // Cook the 'max distance' so it is never zero, and the sqrt(0) special case is not needed
    for (int qIndexI = 0 ; qIndexI < FragmentSiteCount; qIndexI += 1 )
      {
      // XYZ AtomLoc = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,qIndexI) ;
      XYZ AtomLoc = GetPosition( base+qIndexI ) ;
      double e2 = ( AtomLoc - MidXYZ ).LengthSquared() ;
      e2max = max( e2max, e2 ) ;
      } /* endfor */

    BegLogLine( PKFXLOG_NSQSOURCEFRAG )
      << "NSQ Fragment " << pSourceI
      << " bounds " << LowerLeft << UpperRight
      << " boundsize " << ( UpperRight - LowerLeft )
      << " centre " << MidXYZ
      << " extent " << sqrt( e2max ) << 0.5 * ( UpperRight - LowerLeft )
      << " tagext " << SourceFragmentExtent
      << GetPosition( base+SourceTagSiteIndex )
      << EndLogLine ;

    NSQ_NeighbourList.SetXYZE( pSourceI,
                               MidXYZ,
                               e2max,
                               0.5 * ( UpperRight - LowerLeft ) ) ;
    }

  NSQ_NeighbourList.CompleteXYZE( IrreducibleFragmentCount ) ;

  mFatCompPrecomputeFinis.HitOE( PKTRACE_FATCOMP,
                                 "mFatCompPrecompute",
                                 mAddressSpaceId,
                                 mFatCompPrecomputeFinis );
  }

  void InitInteractionTree()
    {
    if( mRecBisTree != NULL )
      {
      delete mRecBisTree;
      mRecBisTree = NULL;
      }

    mRecBisTree = new ORBNode();
    if( mRecBisTree == NULL )
      {
      PLATFORM_ABORT("mRecBisTree == NULL");
      }

    int d = 3;
    int levels = 0;
    int t;
    int NumbrOfProcs = Platform::Topology::GetSize();
    do
      {
      levels++;
      t = 1 << (levels*d);
      }while (NumbrOfProcs > t);
    int base = (levels-1)*d;
    while(d > 0)
      {
      t = 1 << (base + d);
      if(NumbrOfProcs == t) break;
      d--;
      }
    assert(d>0);


    int N=levels;
    mRecBisTree->id = -levels;
    mRecBisTree->dims = d;

    mRecBisTree->low[ 0 ] = 0.0;
    mRecBisTree->low[ 1 ] = 0.0;
    mRecBisTree->low[ 2 ] = 0.0;

    mRecBisTree->voxnum[ 0 ] = mVoxDimX;
    mRecBisTree->voxnum[ 1 ] = mVoxDimY;
    mRecBisTree->voxnum[ 2 ] = mVoxDimZ;

    mRecBisTree->high[ 0 ] = mDynamicBoxDimensionVector.mX;
    mRecBisTree->high[ 1 ] = mDynamicBoxDimensionVector.mY;
    mRecBisTree->high[ 2 ] = mDynamicBoxDimensionVector.mZ;
    }

double GetWeight( int aTargetFragId, int aSourceFragId, ExclusionMask_T aExclMask )
  {
  assert( aTargetFragId >= 0 && aTargetFragId < FRAGMENT_COUNT );
  assert( aSourceFragId >= 0 && aSourceFragId < FRAGMENT_COUNT );

  double weight = 0;

  int TargetSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aTargetFragId );
  int SourceSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aSourceFragId );

  int SourceIsTargetFlag = ( aTargetFragId == aSourceFragId );

  int ExcludedCount = 0;

  double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
  double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );

  double SwitchOff = SwitchLowerCutoff + SwitchDelta;
  double SwitchOffSqrd = SwitchOff * SwitchOff;

  ExclusionMask_T LocalMask = aExclMask;

  int InteractionWithinCutoffCounter = 0;
  for(int SourceFragmentSiteIndex = SourceSiteCount-1;
      SourceFragmentSiteIndex >= 0;
      SourceFragmentSiteIndex-- )
    {

    unsigned int SourceAbsSiteId =
      MSD_IF::GetIrreducibleFragmentMemberSiteId( aSourceFragId, SourceFragmentSiteIndex );

    // Scan the Target Fragment since that should be more local.
    for( int TargetFragmentSiteIndex = (TargetSiteCount-1);
         TargetFragmentSiteIndex >= (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1: 0 );
         TargetFragmentSiteIndex-- )
      {
      ExcludedCount += ( LocalMask & 0x01 );
      LocalMask = LocalMask >> 1;

      unsigned int TargetAbsSiteId =
        MSD_IF::GetIrreducibleFragmentMemberSiteId( aTargetFragId, TargetFragmentSiteIndex );

      XYZ SourcePos = GetPosition( SourceAbsSiteId );
      XYZ TargetPos = GetPosition( TargetAbsSiteId );

      XYZ SourceImaged;
      NearestImageInPeriodicVolume( TargetPos , SourcePos, SourceImaged );

      if ( SourceImaged.DistanceSquared( TargetPos ) <= SwitchOffSqrd )
        {
        InteractionWithinCutoffCounter++;
        }
      }
    }

  int WeightIndex = InteractionWithinCutoffCounter - ExcludedCount;

  assert( WeightIndex < 17 );

  weight = mTimesPerWeight[ WeightIndex ];

  assert( weight >= 0 );

  return weight;
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

  XYZ& GetFirstPositionInFragment( int aFragId )
    {
    int base   = GetNthIrreducibleFragmentFirstSiteIndex( aFragId );
    XYZ & pos = mPosit[ base ];
    return pos;
    }

void
Init()
  {
  #if MSDDEF_DoPressureControl
    mCurrentVerletGuardZone = 0.0;
  #else
    mCurrentVerletGuardZone = 1.0;
  #endif

  Platform::Topology::GetDimensions( &mProcs_x, &mProcs_y, &mProcs_z );

  Platform::Topology::GetMyCoords( &mMyP_x, &mMyP_y, &mMyP_z );
  double BoundingBoxDim_x = RTG.mStartingBoundingBoxDimensionVector.mX;
  double BoundingBoxDim_y = RTG.mStartingBoundingBoxDimensionVector.mY;
  double BoundingBoxDim_z = RTG.mStartingBoundingBoxDimensionVector.mZ;

  mLocalDim_x = BoundingBoxDim_x / mProcs_x;
  mLocalDim_y = BoundingBoxDim_y / mProcs_y;
  mLocalDim_z = BoundingBoxDim_z / mProcs_z;

  mPrevGuardZone = mCurrentVerletGuardZone;

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

  for (int i=0; i< GetIrreducibleFragmentCount(); i++ )
    {
    DoNthIrreducibleFragmentOneTimeInit( i );
    }

  for( int fragId=0; fragId < FRAGMENT_COUNT; fragId++ )
    {
    int firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
    int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

    for( int siteId=firstSiteId; siteId < (firstSiteId + numberOfSites); siteId++ )
      mSiteIdToFragmentMap[ siteId ] = fragId;
    }

  PartitionFragmentsForStaticDistribution();

  // Need to store the prev guard zone to look for a changing guardzone
  // When we use the elliptical broadcast.
  ZeroTimeValues();
  mNumberOfRealSpaceCalls = 0;
  mSampleFragmentTime = 1;
  }

  int GetIndexForImageMultiplierIn125ImageRange( int x, int y, int z )
    {
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
    XYZ LimitOfBox = mDynamicBoxDimensionVector + mCenterOfBoundingBox;

    for( int i=0; i < SITE_COUNT; i++ )
      {
      if( !IsSiteLocal( i ) )
        continue;

      if(( mPosit[ i ].mX > LimitOfBox.mX ) || ( mPosit[ i ].mX < -LimitOfBox.mX ))
        {
        return 1;
        }

      if(( mPosit[ i ].mY > LimitOfBox.mY ) || ( mPosit[ i ].mY < -LimitOfBox.mY ))
        {
        return 1;
        }

      if(( mPosit[ i ].mZ > LimitOfBox.mZ ) || ( mPosit[ i ].mZ < -LimitOfBox.mZ ))
        {
        return 1;
        }
      }
    return 0;
    }

unsigned int GetHomeForSite( unsigned int aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  return mSiteHome[ aSiteId ];
  }

void InitSpacialDecompositionManagement()
  {
  int NumLocalFrags=0;
  for( int f=0; f<FRAGMENT_COUNT;f++)
    {
    if( IsFragmentLocal( f ) )
      NumLocalFrags++;
    }

  BegLogLine(0)
    << "NumLocalFrags: " << NumLocalFrags
    << EndLogLine;

    // NumberOfLocalFragments * ( fragment_id + imaged_tag_atom + positions_for_frag + velocities_for_frag )
  mSizeOfSDBuffers = ( NumLocalFrags ) * ( sizeof( int ) + sizeof( XYZ ) + mMaxSitesInFragment * ( 2 * sizeof( XYZ )) );

  BegLogLine( 0 )
    << "mSizeOfSDBuffers=" << mSizeOfSDBuffers
    << "NumLocalFrags=" << NumLocalFrags
    << "mMaxSitesInFragment=" << mMaxSitesInFragment
    << EndLogLine;


  mNumberOfNearestNeighbors=0;

  if ( mNearestNeighbors != NULL )
    free( mNearestNeighbors );

  int MaxVoxels = mVoxDimX * mVoxDimY * mVoxDimZ;
  mNearestNeighbors = ( int *) malloc( MaxVoxels * sizeof( int ));
  if( mNearestNeighbors == NULL )
    PLATFORM_ABORT("mNearestNeighbors == NULL");

  double radius = 0.5; // Just about half the guard zone

  mPreNumberOfNearestNeighbors = mNumberOfNearestNeighbors;

  mRecBisTree->RanksInNeighborhood( mMyP_x, mMyP_y, mMyP_z, radius, mNearestNeighbors, mNumberOfNearestNeighbors, MaxVoxels );

  if( mSendBufferSD != NULL )
    {
    for( int i=0; i < mPreNumberOfNearestNeighbors; i++ )
      {
      if( mSendBufferSD[ i ] != NULL )
        free( mSendBufferSD[ i ] );

      if( mRecvBufferSD[ i ] != NULL )
        free( mRecvBufferSD[ i ] );
      }

    free( mSendBufferSD );
    free( mRecvBufferSD );
    }

  mSendBufferSD = (char **) malloc( mNumberOfNearestNeighbors * sizeof( char * ));
  mRecvBufferSD = (char **) malloc( mNumberOfNearestNeighbors * sizeof( char * ));

  if( mSendBufferSD == NULL )
    {
    PLATFORM_ABORT("mSendBufferSD == NULL");
    }

  if( mRecvBufferSD == NULL )
    {
    PLATFORM_ABORT("mRecvBufferSD == NULL");
    }


  BegLogLine( 0 )
    << "mNumberOfNearestNeighbors=" << mNumberOfNearestNeighbors
    << " mSizeOfSDBuffers=" << mSizeOfSDBuffers
    << EndLogLine;

  for( int i=0; i < mNumberOfNearestNeighbors; i++ )
    {
    mSendBufferSD[ i ] = (char* ) malloc( mSizeOfSDBuffers );
    if( mSendBufferSD[ i ] == NULL )
      {
      PLATFORM_ABORT("mSendBufferSD[i] == NULL");
      }

    mRecvBufferSD[ i ] = (char* ) malloc( mSizeOfSDBuffers );
    if( mRecvBufferSD[ i ] == NULL )
      {
      PLATFORM_ABORT("mRecvBufferSD[i] == NULL");
      }
    }

  if ( mAllRequestsSD != NULL )
    delete [] mAllRequestsSD;

  if ( mAllStatusSD != NULL )
    delete [] mAllStatusSD;

  mAllRequestsSD = new MPI_Request [ 2 * mNumberOfNearestNeighbors ];
  if( mAllRequestsSD == NULL )
    {
    PLATFORM_ABORT("mAllRequestsSD == NULL");
    }

  mAllStatusSD   = new MPI_Status  [ 2 * mNumberOfNearestNeighbors ];
  if( mAllStatusSD == NULL )
    {
    PLATFORM_ABORT("mAllStatusSD == NULL");
    }
  }

inline
void
InitLocalSitesMask()
  {
  for(int fragmentIndex = 0; fragmentIndex < FRAGMENT_COUNT; fragmentIndex++)
    {
    BegLogLine( PKFXLOG_CHECKMASK )
      << " AddressSpaceId=" << mAddressSpaceId
      << " mPartitionList[ " << fragmentIndex << " ]=" << mPartitionList[ fragmentIndex ]
      << EndLogLine;

    int firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragmentIndex );
    int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragmentIndex );
    for( int siteId=firstSiteId; siteId < (firstSiteId + numberOfSites); siteId++ )
      {
      assert( siteId < SITE_COUNT );
      mSiteHome[ siteId ] = mPartitionList[ fragmentIndex ];
      }
    }
  }

inline
unsigned
IsFragmentLocal( int aFragmentId )
  {
  assert( aFragmentId >= 0 && aFragmentId < FRAGMENT_COUNT );

  unsigned rc = ( mPartitionList[ aFragmentId ] == mAddressSpaceId );

  return ( rc );
  }

inline
unsigned
IsFragmentInRange( int aFragmentId )
  {
  assert( aFragmentId >= 0 && aFragmentId < FRAGMENT_COUNT );

  unsigned rc = mFragmentInRange[ aFragmentId ];

  return ( rc );
  }

inline
void
DistributeFragmentPartitionsStatic( int aTotalInteractionsDone, int* aForceIterationsPerFragment )
  {
  int numberOfAddressSpaces = Platform::Topology::GetAddressSpaceCount();

  if( numberOfAddressSpaces == 1 )
    {
    for( int fragmentIndex=0; fragmentIndex < FRAGMENT_COUNT; fragmentIndex++ )
      mPartitionList[ fragmentIndex ] = 0;
    }
  else
    {
    // int optimalNumberPerAddressSpace = ceil((double) ( aTotalInteractionsDone / numberOfAddressSpaces ));
    int optimalNumberPerAddressSpace = (int) ( ( aTotalInteractionsDone + numberOfAddressSpaces -1) / numberOfAddressSpaces ) ;

    int* workLoad;
    pkNew( &workLoad, numberOfAddressSpaces, __FILE__, __LINE__ );

    assert( workLoad != NULL );

    for( int i=0; i < numberOfAddressSpaces; i++ )
      workLoad[ i ] = 0;

    for( int fragmentIndex=0; fragmentIndex < FRAGMENT_COUNT; fragmentIndex++ )
      {
      int currentWorkLoad = aForceIterationsPerFragment[ fragmentIndex ];
      int addressSpaceToFill = Comparator::Min(workLoad, numberOfAddressSpaces);

      workLoad[ addressSpaceToFill ] += currentWorkLoad;
      mPartitionList[ fragmentIndex ] = addressSpaceToFill;
      }
    }
  }

inline
void
SpacialDecomposition()
  {
  int x = 0;
  int y = 0;
  int z = 0;

  BegLogLine(0)
    << "Entered the SpacialDecomposition::"
    << EndLogLine;

  for( int i=0; i < FRAGMENT_COUNT; i++)
    {
    XYZ cog;
    GetImagedFragCenter( i, cog );

    ORBNode * leaf = mRecBisTree->Node( cog.mX, cog.mY, cog.mZ );

    assert( leaf );

    mPartitionList[ i ] = leaf->rank;

    BegLogLine( 0 )
      << "mPartitionList[ " << i <<" ]="
      << mPartitionList[ i ]
      << EndLogLine;
    }

  InitLocalSitesMask();
  InitSpacialDecompositionManagement();

  return;
  }

  void
  ZeroFragmentTimeCounters()
    {
    for (int i=0; i < FRAGMENT_COUNT; i++)
      mTimeToRunNsqInnerLoop[ i ].Zero();
    }


  inline
  void
  RedistributeFragmentsBasedOnTime()
    {
#ifdef PK_PARALLEL
    int numberOfAddressSpaces = Platform::Topology::GetAddressSpaceCount();

    if( numberOfAddressSpaces == 1 )
      return;

    TimeValue* workLoad;
    pkNew( &workLoad, numberOfAddressSpaces, __FILE__, __LINE__ );

    for( int i=0; i < numberOfAddressSpaces; i++ )
      {
      workLoad[ i ].Zero();
      }

    for (int i=0; i < FRAGMENT_COUNT; i++)
      {
      BegLogLine( PKFXLOG_REDISTRIBUTION )
        << "mTimeToRunNsqInnerLoop[ " << i << " ]="
        << mTimeToRunNsqInnerLoop[ i ].Seconds << "." << mTimeToRunNsqInnerLoop[ i ].Nanoseconds
        << " Current Assignment "
        <<  mPartitionList[ i ]
        << EndLogLine;
      }

    for (int i=0; i < FRAGMENT_COUNT; i++)
      {
      for( int i=0; i < numberOfAddressSpaces; i++ )
        {
        BegLogLine( PKFXLOG_REDISTRIBUTION )
          << "workLoad[ " <<  i << " ]= " << workLoad[ i ].Seconds << "." << "" << workLoad[ i ].Nanoseconds
          << EndLogLine;
        }

      int fragmentMaxIndex = Comparator::Max< TimeValue >( mTimeToRunNsqInnerLoop, FRAGMENT_COUNT ) ;
      int addressSpaceToFill = Comparator::Min< TimeValue >( workLoad, numberOfAddressSpaces );

      BegLogLine(PKFXLOG_REDISTRIBUTION)
        << "RedistributeFragmentBasedOnTime "
        << " FragmentMaxIndex: " << fragmentMaxIndex
        << " AddressSpaceToFill: " << addressSpaceToFill
        << EndLogLine;

      assert( addressSpaceToFill >=0 && addressSpaceToFill < numberOfAddressSpaces );
      assert( fragmentMaxIndex >=0 && fragmentMaxIndex < FRAGMENT_COUNT );

      workLoad[ addressSpaceToFill ] += mTimeToRunNsqInnerLoop[ fragmentMaxIndex ];

      mTimeToRunNsqInnerLoop[ fragmentMaxIndex ].Zero();

      mPartitionList[ fragmentMaxIndex ] = addressSpaceToFill;
      }

    for(int i=0; i < FRAGMENT_COUNT; i++)
      {
      BegLogLine(PKFXLOG_REDISTRIBUTION)
        << "mPartitionList[ " << i << " ]= "
        << mPartitionList[ i ]
        << EndLogLine;
      }

    for( int i=0; i < numberOfAddressSpaces; i++ )
      {
      BegLogLine(PKFXLOG_REDISTRIBUTION)
        << "workLoad[ " <<  i << " ]= " << workLoad[ i ].Seconds << "." << "" << workLoad[ i ].Nanoseconds
        << EndLogLine;
      }

    for (int i=0; i < FRAGMENT_COUNT; i++)
      {
      BegLogLine(PKFXLOG_REDISTRIBUTION)
        << "mTimeToRunNsqInnerLoop[ " << i << " ]= "
        << mTimeToRunNsqInnerLoop[ i ].Seconds << "." << mTimeToRunNsqInnerLoop[ i ].Nanoseconds
        << EndLogLine;
      }

    InitLocalSitesMask();
#endif
    return;
    }

  inline
  void
  GlobalizePartitionMap()
  {
#ifdef PK_PARALLEL
  for(int i=0; i < FRAGMENT_COUNT; i++)
    {
    BegLogLine(PKFXLOG_REDISTRIBUTION)
      << "Before Broadcast: mPartitionList[ " << i << " ]= "
      << mPartitionList[ i ]
      << EndLogLine;
    }

  BroadcastPartitionMap();

  InitLocalSitesMask();

  for(int i=0; i < FRAGMENT_COUNT; i++)
    {
    BegLogLine(PKFXLOG_REDISTRIBUTION)
      << "After Broadcast: mPartitionList[ " << i << " ]= "
      << mPartitionList[ i ]
      << EndLogLine;
    }
#endif
  return;
  }

  inline
  void
  incRealSpaceSend()
  {
  mNumberOfRealSpaceCalls++;
  }

inline
int
PartitionFragmentsForStaticDistribution()
  {
  /*************************************************************************************
   * Analyze the number of force calculations performed for each fragment.
   * Since we are now single calculating the NSQ forces, lower index
   * fragments( depending on the number of sites) are more likely to do less
   * force calculations then the higher index fragments.
   *
   * Let S = { s_0, s_1, ..., s_n } be the set of site counts s_i for fragment i. for i<=n
   * Let Fi be the ith fragment for i<=n
   *
   * Then for Fi the number of NSQ force iterations are:
   *
   *                        s_i * ( s_i + ... + s_n ) for i <= n
   *
   * Based on the observation above we create an array with the total number of NSQ force
   * iterations per fragment, and then partition the fragments to have a close to an equal
   * number of such force iterations.
   *************************************************************************************/


  // Only the master node is allowed in here
  // assert( Platform::Topology::GetAddressSpaceId() == 0 );

  int fragmentCount = MSD_IF::GetIrreducibleFragmentCount();
  BegLogLine( PKFXLOG_IRREDUCIBLEFRAGMENT )
    << " PartitionFragmentsForDistribution() "
    << "fragmentCount=" << fragmentCount
    << EndLogLine;

  int* forceIterationsPerFragment;// = new int[ fragmentCount ];
  pkNew( &forceIterationsPerFragment, fragmentCount, __FILE__, __LINE__ );
  assert( forceIterationsPerFragment != NULL );

  int totalNumberOfForceIterations = 0;

  for( int i = 0; i < fragmentCount; i++ )
    {

    int sitesInFragmentI = MSD_IF::GetIrreducibleFragmentMemberSiteCount( i );
    forceIterationsPerFragment[ i ] = 0;

    for( int j=i; j < fragmentCount; j++ )
      {
      int sitesInFragmentJ = MSD_IF::GetIrreducibleFragmentMemberSiteCount( j );
      forceIterationsPerFragment[ i ] += sitesInFragmentJ;
      }

    forceIterationsPerFragment[ i ] *= sitesInFragmentI;
    totalNumberOfForceIterations += forceIterationsPerFragment[ i ];

    }

  DistributeFragmentPartitionsStatic( totalNumberOfForceIterations, forceIterationsPerFragment );
  InitLocalSitesMask();
  return 0;
  }

/**********************************************************************/
inline
int
AllocateBlock(  int aBaseAbsoluteSiteId,  // This alloc is for absolute sites aBaseAbs to aBaseAbs + aBlockSize.
                int aBlockSize )
  {
  BegLogLine( PKFXLOG_ALLOCATEBLOCK )
    << "DynVarMgr::AllocateBlock() "
    << " aBaseAbsoluteSiteId "
    << aBaseAbsoluteSiteId
    << " aBlockSize "
    << aBlockSize
    << EndLogLine;
  return( aBaseAbsoluteSiteId );
  }


  inline
  void
  DumpAllSiteRecord( int aFlag,
                     char *aContext,
                     int aTimeStep,
                     int aIntegratorLevel)
  {
  for( int site = 0; site < SITE_COUNT; site++ )
    {
    DumpSiteRecord( aFlag, aContext, aTimeStep, aIntegratorLevel, site );
    }
  }


inline
void
DumpSiteRecord( int aFlag,
                char *aContext,
                int aTimeStep,
                int aIntegratorLevel,
                int aIndex )
  {
  assert( this->mAbsoluteToLocalSiteMap[ aIndex ].IsValid() );

  BegLogLine(aFlag)
    << aContext
    << " TimeStep "
    << aTimeStep
    << " SubLevel "
    << aIntegratorLevel
    << " Pos "
    << this->mLocalSiteTable[ aIndex ].mPosit
    << " Force "
    << this->mLocalSiteTable[ aIndex ].mForce[ aIntegratorLevel ]
    << " Vel "
    << this->mLocalSiteTable[ aIndex ].mVelocity
    << EndLogLine;
  }


inline
void
SetForce( int aAbsoluteSiteId,
          int aTimeStep,
          int aIntegratorLevel,
          XYZ aForce )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  assert( aIntegratorLevel >= 0 && aIntegratorLevel < NUMBER_OF_RESPA_LEVELS );
  mForce[ aIntegratorLevel ][ aAbsoluteSiteId ] = aForce;
  }


// inline
// XYZ
// GetForce( int aAbsoluteSiteId,
//           int aTimeStep,
//           int aIntegratorLevel )
//   {
//   assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
//   assert( aIntegratorLevel >= 0 && aIntegratorLevel < NUMBER_OF_RESPA_LEVELS );
//   XYZ rc = mForce[ aIntegratorLevel ][ aAbsoluteSiteId ];
//   return( rc );
//   }

inline
XYZ&
GetForce( int aAbsoluteSiteId,
          int aTimeStep,
          int aIntegratorLevel )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  assert( aIntegratorLevel >= 0 && aIntegratorLevel < NUMBER_OF_RESPA_LEVELS );
  XYZ & rc = mForce[ aIntegratorLevel ][ aAbsoluteSiteId ];
  return( rc );
  }

// inline
// XYZ
// GetPosition( int aAbsoluteSiteId )
//   {
//   assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
//   XYZ rc = mPosit[ aAbsoluteSiteId ];

//   assert( !rc.IsInvalid() );

//   return( rc );
//   }

inline
XYZ&
GetPosition( int aAbsoluteSiteId )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  XYZ & rc = mPosit[ aAbsoluteSiteId ];

  assert( !rc.IsInvalid() );

  return( rc );
  }


// DON'T MUCH LIKE THIS INTERFACE, BUT IT IS USED BY PLIMPTONLOOPS AT THE MOMENT
inline
XYZ *
GetPositionPtr( int aAbsoluteSiteId )
  {
  XYZ *rc = &(mPosit[ aAbsoluteSiteId ]);

  assert( !rc->IsInvalid() );

  return( rc );
  }


inline
void
SetPosition( int aAbsoluteSiteId, XYZ aPosition )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  mPosit[ aAbsoluteSiteId ] = aPosition;
  }

inline
XYZ&
GetVelocity( int aAbsoluteSiteId )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  XYZ & rc = mVelocity[ aAbsoluteSiteId ];
  return( rc );
  }

// inline
// XYZ
// GetVelocity( int aAbsoluteSiteId )
//   {
//   assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
//   XYZ rc = mVelocity[ aAbsoluteSiteId ];
//   return( rc );
//   }

inline
void
SetVelocity( int aAbsoluteSiteId, XYZ aVelocity )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  mVelocity[ aAbsoluteSiteId ] = aVelocity;
  }

inline
void
ExportDynamicVariables( unsigned aOuterTimeStep )
  {
  //#define DYNVARMNGR_NOEXPORTOFSITES
  #ifdef DYNVARMNGR_NOEXPORTOFSITES
    return;
  #endif

  for( int s = 0; s < GetNumberOfSites(); s++)
    {

    int externalSiteId = MSD_IF::GetExternalSiteIdForInternalSiteId( s );

    //#ifdef PK_MPI_ALL
    #ifdef PK_PARALLEL
    if( IsSiteLocal( s ) )
    #endif
      {
      ED_Emit_DynamicVariablesSite( aOuterTimeStep,
                                    mAddressSpaceId,  // using 0 for voxel id
                                    externalSiteId,
                                    GetPosition( s ),
                                    GetVelocity( s )     );
      }
    }

  #if MSDDEF_NOSE_HOOVER
  if( mAddressSpaceId == 0 )
    for( int i=0; i < FRAGMENT_COUNT; i++)
      {
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

  #ifdef PK_PARALLEL
  BegLogLine( PKFXLOG_EXPDYNVAR ) << "ExportDynamicVariables:: Before Barrier " << EndLogLine;
  Platform::Control::Barrier();
  BegLogLine( PKFXLOG_EXPDYNVAR ) << "ExportDynamicVariables:: After Barrier " << EndLogLine;
  #endif
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
  // In order to make good use of the information about the
  // site, we would like to access the converted time from
  // this routine.
  BegLogLine(PKFXLOG_LOCALSITEMGR)
    << "LocalSiteManager::ZeroForceRegisters() BEGIN"
    << " SimTick "     << aSimTick
    << " IntLevel "    << aIntegratorLevel
    << EndLogLine;

  for( int level = aStartLevel; level <= aIntegratorLevel; level++ )
    {
    for(int site = 0; site < GetNumberOfSites(); site ++ )
      {
      mForce[ level ][ site ].Zero();
      }
    }
  BegLogLine(PKFXLOG_LOCALSITEMGR)
    << "LocalSiteManager::ZeroForceRegisters()  END"
    << " SimTick "     << aSimTick
    << " IntLevel "    << aIntegratorLevel
    << EndLogLine;

  ZeroCenterOfMassForceReg();
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

#endif
