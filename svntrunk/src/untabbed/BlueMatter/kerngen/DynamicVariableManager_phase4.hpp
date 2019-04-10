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

#ifndef __DYNAMIC_VARIABLE_MANAGER_HPP__
#define __DYNAMIC_VARIABLE_MANAGER_HPP__

#ifndef PKFXLOG_CHECK_SITE_DISP
#define PKFXLOG_CHECK_SITE_DISP ( 0 )
#endif

#ifndef PKFXLOG_ROLLING_REG_DEBUG
#define PKFXLOG_ROLLING_REG_DEBUG ( 1 )
#endif

#ifndef PKFXLOG_CHASEHANG
#define PKFXLOG_CHASEHANG ( 0 )
#endif

#ifndef PKFXLOG_EXPDYNVAR
#define PKFXLOG_EXPDYNVAR ( 0 )
#endif

#ifndef PKFXLOG_SETUP
#define PKFXLOG_SETUP ( Platform::Topology::GetAddressSpaceId() == 0 )
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

//#define OUT_OF_THIN_AIR_FUDGE_FACTOR 1

#ifdef PK_BLADE_SPI
#ifndef MANAGE_SPACIAL_DECOMPOSE_ALLTOALLV
#define MANAGE_SPACIAL_DECOMPOSE_ALLTOALLV
#endif
#endif
//#define MANAGE_SPACIAL_DECOMPOSE_ALLTOALLV

static void c1_printf( int, XYZ, char*, int );
static void c1_printf( int, char*, int );

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

// #include <BlueMatter/NeighborList.hpp>
#include <BlueMatter/orbnode.hpp>
#include <BlueMatter/InteractionModel.hpp>

#include <rptree.hpp>

// arayshu - Below 2 lines were removed from ORBManagerIF.hpp
// and moved here so that they can be accessed by
// GenerateOnBoundaryAndMidpointTableForVoxelsVersion2Phase5( )
double Extents[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);
XYZ    FragCenters[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);

int MyRankAligned MEMORY_ALIGN(5);

#define NOT_NEAREST_NEIGBOR -9999

struct FragmentInRangePair
  {
  XYZ            mFragmentCenter;
  double         mExtent;
  FragId         mFragId;

#ifdef PK_PHASE5_PROTOTYPE
  int            mRankOrdinal;
#endif

  unsigned short mVoxelOrdinal;
  unsigned short mMoleculeId;
  };

typedef Platform::Clock::TimeValue TimeValue;
typedef unsigned int ExclusionMask_T;

#define EXCLUSION_MASK_NOT_DEFINED     0xFFFFFFFF

#define MAX_NEAREST_NEIGHBORS 100
#define VOXEL_IN_P3ME_BOX_ONLY  2

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

struct FragmentRep
  {
  FragId mFragId;
  short  mOffset;
  } MEMORY_ALIGN( 5 );

#define ROUND32( a ) (( ((unsigned)a)+31 ) & ~0x1f)
#define ROUND8( a ) (( ((unsigned)a)+7 ) & ~0x7)

static inline double NearestInteger(const double x);
#include <BlueMatter/IntegratorStateManagerIF.hpp>
#include <BlueMatter/InteractionStateManagerIF.hpp>

 struct tVerletSiteInfo
{
//    FragId fragid;   // fragment id
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


#ifdef PK_PHASE5_SPLIT_COMMS
#include <BlueMatter/KSpaceCommManagerIF.hpp>
typedef KspaceCommManagerIF KSpaceCommMIF;
#else 
class KspaceDummy {};
typedef KspaceDummy KSpaceCommMIF;
#endif


typedef unsigned short NodeId;

#ifdef PK_PHASE5_PROTOTYPE
struct NodePair
  {
  NodeId mNodeIdA;
  NodeId mNodeIdB;
  };

typedef float LBWeightType;

struct NodePairRecord
  {
  NodePair        mNodePair;
      // float           mWeight;
  LBWeightType mWeight;
#ifndef PK_PHASE5_SKIP_OPT_SORT
  unsigned short             mOptCount;
#endif
#if 0
  HashTableEntry* mOptionsHashTableEntry;
  short    mAssignmentNodeOptionsCount;
  NodeId   mAssignmentNodeOptions[ 1 ];
#endif
  };

struct SparseSkinHelper
  {
//  int             mOptCount;
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

inline void memcpy_xyz( XYZ* aTrg, XYZ* aSrc, int aCount )
  {
  for( int i=0; i < aCount; i++ )
    {
    aTrg[ i ] = aSrc[ i ];
    }
  }

#ifdef PK_PKT_THROBBER
  #include <spi/PktThrobberIF.hpp>
  
  #ifdef PK_PHASE5_SPLIT_COMMS
    #include <BlueMatter/MsgThrobberIF.hpp>
  #endif
#else
  #include <BlueMatter/MsgThrobberIF.hpp>
#endif

//#include <BlueMatter/ellipsoidal_collectives_spi.hpp>

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
      // printf( "errno: %d\n", errno );
      if( errno == EAGAIN )
        continue;
      else
        PLATFORM_ABORT( " FAILED TO WRITE SCO3 FILE " );
      }

    total += write_rc;
    }
  }

static int
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
    // printf("X axis guides nominal voxel size SimX %f / ProcX %d = Nominal %f\n", sX, pX, NominalVoxelSize );
    }
  else if( s2pY < s2pX && s2pY < s2pZ )  // Y is min aspect ratio
    {
    NominalVoxelSize = s2pY;
    // printf("Y axis guides nominal voxel size SimY %f / ProcY %d = Nominal %f\n", sY, pY, NominalVoxelSize );
    }
  else                                   // Z is min aspect ratio
    {
    NominalVoxelSize = s2pZ;
    // printf("Z axis guides nominal voxel size SimZ %f / ProcZ %d = Nominal %f\n", sZ, pZ, NominalVoxelSize );
    }

  double NomVoxMeshX = sX / NominalVoxelSize;
  double NomVoxMeshY = sY / NominalVoxelSize;
  double NomVoxMeshZ = sZ / NominalVoxelSize;

  // printf("Nominal Voxel Mesh Dimensions X %f   Y %f   Z %f\n",  NomVoxMeshX, NomVoxMeshY, NomVoxMeshZ );

  // Round to nearest even

  int VoxMeshX = NomVoxMeshX + 1;
  int VoxMeshY = NomVoxMeshY + 1;
  int VoxMeshZ = NomVoxMeshZ + 1;

  VoxMeshX = ( VoxMeshX / 2 ) * 2;
  VoxMeshY = ( VoxMeshY / 2 ) * 2;
  VoxMeshZ = ( VoxMeshZ / 2 ) * 2;

  //printf("Minimum voxel mesh dimensions X %d   Y %d   Z %d\n", VoxMeshX, VoxMeshY, VoxMeshZ );
  //printf("Maximum voxel size            X %f   Y %f   Z %f\n", sX / VoxMeshX, sY / VoxMeshY, sZ / VoxMeshZ );
  int TotalVoxels = VoxMeshX * VoxMeshY * VoxMeshZ;
  int TotalProcs = pX * pY * pZ;
  double TotalVoxelsPerProc =  (1.0 * TotalVoxels) / TotalProcs;
  // printf("Total Voxels %d  Total Procs %d  Voxels / Proc %f\n", TotalVoxels, TotalProcs, TotalVoxelsPerProc );

  double factor;
  for( factor = 1.0; ; factor += 0.1 )
    {
    NomVoxMeshX = sX / NominalVoxelSize;
    NomVoxMeshY = sY / NominalVoxelSize;
    NomVoxMeshZ = sZ / NominalVoxelSize;

    // printf("Vox mesh scale factor %f\n", factor );

    NomVoxMeshX *= factor ;
    NomVoxMeshY *= factor ;
    NomVoxMeshZ *= factor ;

    // printf("Nominal Voxel Mesh Dimensions X %f   Y %f   Z %f\n",  NomVoxMeshX, NomVoxMeshY, NomVoxMeshZ );

    // Round to nearest even

    VoxMeshX = NomVoxMeshX + 1;
    VoxMeshY = NomVoxMeshY + 1;
    VoxMeshZ = NomVoxMeshZ + 1;

    VoxMeshX = ( VoxMeshX / 2 ) * 2;
    VoxMeshY = ( VoxMeshY / 2 ) * 2;
    VoxMeshZ = ( VoxMeshZ / 2 ) * 2;


    // printf("Minimum voxel mesh dimensions X %d   Y %d   Z %d\n", VoxMeshX, VoxMeshY, VoxMeshZ );
    // printf("Maximum voxel size            X %f   Y %f   Z %f\n", sX / VoxMeshX, sY / VoxMeshY, sZ / VoxMeshZ );
    TotalVoxels = VoxMeshX * VoxMeshY * VoxMeshZ;
    TotalProcs = pX * pY * pZ;
    TotalVoxelsPerProc =  (1.0 * TotalVoxels) / TotalProcs;
    // printf("Total Voxels %d  Total Procs %d  Voxels / Proc %f\n", TotalVoxels, TotalProcs, TotalVoxelsPerProc );

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

  // printf("********************\n");

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
#if 0
  DenseSmallUnsignedIntArray()
    {
    assert( BITS == 1 || BITS == 2 || BITS == 4 );
    mCount = 0;
    mMemory  = NULL;
    }
#endif

  void Init(void)
    {
    assert( BITS == 1 || BITS == 2 || BITS == 4 );
    mCount = 0;
    mMemory  = NULL;
    }

  public:
// This was causing the TDynamicVariableManager to have a destructor, which was moving it from 'common' to '.data',
// which was bad for big molecules
#if 0
  ~DenseSmallUnsignedIntArray()
    {
    if( mMemory != NULL )
      {
      assert( mCount != 0 );
//      delete [] mMemory;
      mMemory = NULL;
      }
    }
#endif

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
//      Platform::Heap::Free(mMemory) ;
      mMemory = NULL;
      }

    mRequiredWords =  ((aCount * BITS) + BITS_IN_WORD - 1) / BITS_IN_WORD ;

    mMemory = new unsigned int [ mRequiredWords ];
//    mMemory = (unsigned int *) Platform::Heap::Allocate(mRequiredWords*sizeof(int)) ;

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

// #ifdef PK_PHASE5_SPLIT_COMMS
//   static TraceClient mGlobalizeBondedPositionsStart;
//   static TraceClient mGlobalizeBondedPositionsFinis;
// #endif

//   static TraceClient mIntegerAllReduceStart ;
//   static TraceClient mIntegerAllReduceFinis ;

  static TraceClient mFloatingPointAllReduceStart;
  static TraceClient mFloatingPointAllReduceFinis;

  } ;

static const float dk_ImageTable[5] = { 2.0, 1.0, 0.0, -1.0, -2.0 } ;

template<int SITE_COUNT, int FRAGMENT_COUNT, int RESPA_LEVEL_COUNT>
class TDynamicVariableManager : public MDVM_BaseIF, public TDynamicVariableManagerTracer
  {
  public:

  char*                          mVoxelsInBoxTable MEMORY_ALIGN(5);

  int*                           mVoxelsInBox;
  int                            mVoxelsInBoxSize;

  int*                           mRanksInBox;
  int                            mRanksInBoxSize;

  // FragId                         mFragmentsInVoxelsInP3MEBox[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);
  FragId*                        mFragmentsInVoxelsInP3MEBox MEMORY_ALIGN(5);
  int                            mFragmentsInVoxelsInP3MEBoxSize;


  // FragmentRep                    mSiteIdToFragmentRepMap[ SITE_COUNT ] MEMORY_ALIGN(5);
  FragmentRep*                   mSiteIdToFragmentRepMap MEMORY_ALIGN(5);

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

  #ifdef PK_PHASE5_SPLIT_COMMS
  int* mNodesForBonded;
  int  mNodesForBondedCount;   

  InteractionStateManagerIF       mBondedInteractionStateManagerIF MEMORY_ALIGN(5);
  KspaceInteractionStateManagerIF mKspaceInteractionStateManagerIF MEMORY_ALIGN(5);
  #endif

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
#ifdef MANAGE_SPACIAL_DECOMPOSE_ALLTOALLV
  char* mSendBufferSD MEMORY_ALIGN( 5 );
  char* mRecvBufferSD MEMORY_ALIGN( 5 );
#else
  char** mSendBufferSD MEMORY_ALIGN( 5 );
  char** mRecvBufferSD MEMORY_ALIGN( 5 );
#endif

#ifdef PK_MPI_ALL
  MPI_Request* mAllRequestsSD;
  MPI_Status*  mAllStatusSD ;
#endif

#ifdef PK_PARALLEL
  int* mSDSendCounts;
  int* mSDSendOffsets;
  int* mSDRecvCounts;
  int* mSDRecvOffsets;
  int* mSDSendCountsOffsets;
  int* mSDRecvCountsOffsets;
  int* mSDSendCountsCounts;
  int* mSDRecvCountsCounts;
#endif

  /**
   * Map FragmentId to AddressSpaceId. This is developed by the master node and distributed to the others
   * All nodes then use this list to develop the mask
   **/
  // unsigned int  mPartitionList[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);
  // unsigned char mFragmentInRange[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);

  // unsigned int  mSiteHome[ SITE_COUNT ] MEMORY_ALIGN(5);
  // unsigned int  mSiteIdToFragmentMap[ SITE_COUNT ] MEMORY_ALIGN(5);

  // This is needed for phase 1.5
  // Will leave it here in case we need to merge
  // TimeValue mTimeToRunNsqInnerLoop[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);'
  TimeValue*   mTimeToRunNsqInnerLoop MEMORY_ALIGN(5);

  int    mAddressSpaceId ;

  double mCurrentVerletGuardZone;
  double mCurrentAssignmentGuardZone;

  double mLocalDim_x;
  double mLocalDim_y;
  double mLocalDim_z;

  int mMyP_x, mMyP_y, mMyP_z;
  int mProcs_x, mProcs_y, mProcs_z;

#ifdef PK_PHASE4
  int*                 mRemovedFrags;
  int                  mRemovedFragsAllocated;
  int                  mNumberOfVoxelsInRange;

  int                  mVoxelsInSkinHoodCount;

  int*                 mVoxelsInRange;
  int                  mMaxVoxelsInRange;
  int*                 mVoxelIndexToVoxelOrdinalMap;

  FragmentInRangePair*  mLocalFragments MEMORY_ALIGN(5);

  ORBNode*             mRecBisTree;

  unsigned int   mVoxelInteractionTypeTableSize;

#ifdef PK_PHASE5_PROTOTYPE
  DenseSmallUnsignedIntArray<1> mVoxelInteractionTypeTable;
#else
  DenseSmallUnsignedIntArray<2> mVoxelInteractionTypeTable;
#endif

  int*   mNodeIdToNodeOrdinalMap MEMORY_ALIGN( 5 );

  unsigned int mVoxDimX;
  unsigned int mVoxDimY;
  unsigned int mVoxDimZ;

  double mRadiusOfSphere;

  #ifdef PK_PKT_THROBBER
      #ifdef PK_PHASE5_PROTOTYPE
        NeighborhoodCommDriver< PktThrobberIF, FRAGMENT_COUNT, REAL_SPACE_PKT_COMM_DRIVER > mNeighborhoodCommDriver MEMORY_ALIGN( 5 );
      #else
        NeighborhoodCommDriver< PktThrobberIF, FRAGMENT_COUNT, V4_NEIGHBORHOOD_PKT_COMM_DRIVER > mNeighborhoodCommDriver MEMORY_ALIGN( 5 );
      #endif

      #ifdef PK_PHASE5_SPLIT_COMMS
        NeighborhoodCommDriver< MsgThrobberIF, FRAGMENT_COUNT, BONDED_MSG_COMM_DRIVER > mNeighborhoodCommBondedDriver MEMORY_ALIGN( 5 );
      #endif
  #else
      NeighborhoodCommDriver< MsgThrobberIF, FRAGMENT_COUNT, V4_NEIGHBORHOOD_MSG_COMM_DRIVER > mNeighborhoodCommDriver MEMORY_ALIGN( 5 );
  #endif
#endif

#ifdef PK_PHASE5_PROTOTYPE
    int*  mNodeIdToRecvNodeOrdinalMap;
    unsigned char** mAssignmentNodeTable;
#endif

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

  // FragmentRecord mLocalFragmentTable[ FRAGMENT_COUNT ] MEMORY_ALIGN(5);
  FragmentRecord* mLocalFragmentTable MEMORY_ALIGN(5);

  int             mLocalFragmentIndexNext;

public:

  // THESE ARE HERE TEMPORARILY TO ONLY HOLD THE INITIAL STATE
//   XYZ  mPosit    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
//   XYZ  mPositTracker    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
//   XYZ  mVelocity [ SITE_COUNT ] MEMORY_ALIGN( 5 );
   XYZ* mPosit        MEMORY_ALIGN( 5 );
   XYZ* mPositTracker MEMORY_ALIGN( 5 );
   XYZ* mVelocity     MEMORY_ALIGN( 5 );

#ifdef DEBUG_P4
  XYZ  mForce    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
  XYZ  mReductionTemp    [ SITE_COUNT ] MEMORY_ALIGN( 5 );
#endif

  XYZ  mImageVectorTable[ 125 ] MEMORY_ALIGN( 5 );

  unsigned int mImageMap[ 125 ] MEMORY_ALIGN( 5 ) ;

  int*  mNearestNeighbors MEMORY_ALIGN( 5 );
  int  mNumberOfNearestNeighbors;
  int  mPreNumberOfNearestNeighbors;

  int* mRankToNearestNeighborMap;


  int CheckeredTemplate[ 2 ][ 2 ][ 2 ];

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
  InitVoxelsInBoxTable( int* aVoxelsInBox, int aVoxelsInBoxSize )
  {
  int VoxelSpace = mVoxDimX * mVoxDimY * mVoxDimZ;

  if( mVoxelsInBoxTable != NULL )
    free( mVoxelsInBoxTable );

  mVoxelsInBoxTable = (char *) malloc( sizeof(char) * VoxelSpace );
  if( mVoxelsInBoxTable == NULL )
    PLATFORM_ABORT("mVoxelsInBoxTable == NULL");

//   int MaxVoxelCount = VoxelSpace * OUT_OF_THIN_AIR_FUDGE_FACTOR;
//   int* VoxelsInBox = (int *) malloc( sizeof(int) * MaxVoxelCount );

//   if( VoxelsInBox == NULL )
//     PLATFORM_ABORT("VoxelsInBox == NULL");

//   BegLogLine( 0 )
//     << "vmin: " << vmin
//     << " vmax: " << vmax
//     << EndLogLine;

//   double vminTemp[3];
//   vminTemp[ 0 ] = vmin.mX;
//   vminTemp[ 1 ] = vmin.mY;
//   vminTemp[ 2 ] = vmin.mZ;

//   double vmaxTemp[3];
//   vmaxTemp[ 0 ] = vmax.mX;
//   vmaxTemp[ 1 ] = vmax.mY;
//   vmaxTemp[ 2 ] = vmax.mZ;

//   int VoxelCount;
//   mRecBisTree->VoxelsInBox( vminTemp, vmaxTemp, VoxelsInBox, VoxelCount, MaxVoxelCount );

  for(int i=0; i < VoxelSpace; i++ )
    mVoxelsInBoxTable[ i ]=0;

  assert( mVoxelsInRange != NULL );

  assert( aVoxelsInBoxSize >= 0 && aVoxelsInBoxSize <= VoxelSpace );

  for( int i=0; i < aVoxelsInBoxSize; i++ )
    {
    int VoxId = aVoxelsInBox[ i ];
    VoxelIndex VoxIndex = mRecBisTree->VoxelIdToVoxelIndex( VoxId );

    assert( VoxIndex >= 0 && VoxIndex < VoxelSpace );
    mVoxelsInBoxTable[ VoxIndex ] = 1;

    int VoxIdFound = 0;
    for( int j=0; j < mNumberOfVoxelsInRange; j++ )
      {
      if( VoxId == mVoxelsInRange[ j ] )
        {
        VoxIdFound = 1;
        // break;
        }
      }

    if( !VoxIdFound )
      {
      // Set the Voxel in P3ME box ONLY bit
      // VoxelIndex VoxIndex = mRecBisTree->VoxelIdToVoxelIndex( VoxId );
      mVoxelsInBoxTable[ VoxIndex ] |= VOXEL_IN_P3ME_BOX_ONLY;
      }
    }
  
  mNeighborhoodCommDriver.SetVoxelsInBoxTable( mVoxelsInBoxTable );
  }

  void
  SendPositionToP1_5()
    {
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

        BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
          << "Connection established: about to write pos and vel"
          << EndLogLine;

        // Are the positions, velocities and force correct?
        write_to_file( sockfd, (char *) mPosit, sizeof( XYZ ) * SITE_COUNT );

        write_to_file( sockfd, (char *) mVelocity, sizeof( XYZ ) * SITE_COUNT );

        BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
          << "Done writing pos and vel"
          << EndLogLine;
      }
    }

  void
  SendVelocityToP1_5( int aSimTick )
    {
    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Entering SendVelocityToP1_5"
      << EndLogLine;

    // 1. Reduce all the positions and forces to node 0
    for( int i=0; i<SITE_COUNT; i++ )
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

    int ReduceSize = SITE_COUNT * (sizeof(XYZ)/sizeof(double));

    MPI_Allreduce( mVelocity, mReductionTemp, ReduceSize, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
    memcpy( mVelocity, mReductionTemp, sizeof(XYZ) * SITE_COUNT );

    if( mAddressSpaceId == 0 )
      {
      BegLogLine( 1 )
        << "About to Send Velocities from P4"
        << EndLogLine;

      write_to_file( sockfd, (char *) mVelocity, sizeof( XYZ ) * SITE_COUNT );
      }

    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Leaving SendVelocityToP1_5"
      << EndLogLine;
    }

  void
  SendPosVelP1_5( int aSimTick )
    {
#ifdef DEBUG_P4
    BegLogLine( PKFXLOG_ROLLING_REG_DEBUG )
      << aSimTick    
      << " Entering SendPosVelP1_5"
      << EndLogLine;

    // 1. Reduce all the positions and forces to node 0
    for( int i=0; i<SITE_COUNT; i++ )
      {
      mPosit[ i ].Zero();
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
        mPosit[ sId ] = mIntegratorStateManagerIF.GetPosition( fragId, offset );
        mVelocity[ sId ] = mIntegratorStateManagerIF.GetVelocity( fragId, offset );
        }
      }

    int ReduceSize = SITE_COUNT * (sizeof(XYZ)/sizeof(double));

    MPI_Allreduce( mPosit, mReductionTemp, ReduceSize, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
    memcpy( mPosit, mReductionTemp, sizeof(XYZ) * SITE_COUNT );

    MPI_Allreduce( mVelocity, mReductionTemp, ReduceSize, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
    memcpy( mVelocity, mReductionTemp, sizeof(XYZ) * SITE_COUNT );

    // 2. Send all the positions, velocities and forces to node 0
    if( mAddressSpaceId == 0 )
      {
      assert( sockfd >= 0 );

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
#ifdef DEBUG_P4
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

    MPI_Allreduce( mForce, mReductionTemp, ReduceSize, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
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
  inline
  int
  IsVoxelIndexInP3MEBoxRange( int aVoxIndex )
    {
    assert( aVoxIndex >= 0 && aVoxIndex < mMaxVoxelsInRange );
    return mVoxelsInBoxTable[ aVoxIndex ];
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
//   inline
//   void
//   BroadcastPartitionMap()
//   {
// #ifdef PK_PARALLEL
//   Platform::Collective::Broadcast( (char *) mPartitionList, FRAGMENT_COUNT*sizeof(int), 0  );
// #endif
//   }

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
#ifdef PK_PARALLEL

  // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
  BegLogLine( 0 )
    << aSimTick
    << " Entering ReduceForces"
    << EndLogLine;

  mReduceForcesStart.HitOE( PKTRACE_REDUCEFORCES,
                            "ReduceForces",
                            mAddressSpaceId,
                            mReduceForcesStart );

  mIntegratorStateManagerIF.ZeroForces();

  BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << "Got here" << EndLogLine;
  
  #ifdef PK_PHASE5_SPLIT_COMMS
    mNeighborhoodCommBondedDriver.Reduce( aSimTick );
  #endif

  BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << "Got here" << EndLogLine;

  mNeighborhoodCommDriver.Reduce( aSimTick );

  BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << "Got here" << EndLogLine;

  mReduceForcesFinis.HitOE(PKTRACE_REDUCEFORCES,
                           "ReduceForces",
                           mAddressSpaceId,
                           mReduceForcesFinis );

  // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
  BegLogLine( 0 )
    << aSimTick
    << " Leaving ReduceForces"
    << EndLogLine;
#endif
  return;
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
  SuperWammyGlobalAllReduceWhichWillBeChangedSoonForMigration()
    {
    static XYZ velTemp [ SITE_COUNT ];
    static XYZ posTemp [ SITE_COUNT ];

    for( int i=0; i<SITE_COUNT; i++ )
      {
      mVelocity[ i ].Zero();
      mPosit[ i ].Zero();
      mPositTracker[ i ].Zero();
      }

    for( int i=0; i < mIntegratorStateManagerIF.GetNumberOfLocalFragments(); i++ )
      {
      FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( i );

      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset=0; offset < SiteCount; offset++ )
        {
        SiteId sId = FirstSiteId + offset;

        assert( sId >=0 && sId < SITE_COUNT );
        mVelocity[ sId ] = mIntegratorStateManagerIF.GetVelocity( fragId, offset );
        mPosit[ sId ] = mIntegratorStateManagerIF.GetPosition( fragId, offset );
        mPositTracker[ sId ] = mIntegratorStateManagerIF.GetPositionTracker( fragId, offset );
        }
      }

#ifdef PK_MPI_ALL
    int numberOfDoublesInPackage = SITE_COUNT * (sizeof(XYZ)/sizeof(double));
    MPI_Allreduce( mVelocity , velTemp, numberOfDoublesInPackage, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
    memcpy(mVelocity, velTemp, numberOfDoublesInPackage*sizeof(double));

    MPI_Allreduce( mPosit , posTemp, numberOfDoublesInPackage, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
    memcpy( mPosit, posTemp, numberOfDoublesInPackage*sizeof(double) );

    MPI_Allreduce( mPositTracker , posTemp, numberOfDoublesInPackage, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
    memcpy( mPositTracker, posTemp, numberOfDoublesInPackage*sizeof(double) );
#endif

#ifdef PK_BLADE_SPI
    int ReduceSize = SITE_COUNT * sizeof( XYZ );
    GlobalTreeReduce( (unsigned long *)mVelocity, (unsigned long *)velTemp, ReduceSize, _BGL_TREE_OPCODE_ADD );
    memcpy( mVelocity, velTemp, ReduceSize );

    GlobalTreeReduce( (unsigned long *)mPosit, (unsigned long *)posTemp, ReduceSize, _BGL_TREE_OPCODE_ADD );
    memcpy( mPosit, posTemp, ReduceSize );

    GlobalTreeReduce( (unsigned long *)mPositTracker, (unsigned long *)posTemp, ReduceSize, _BGL_TREE_OPCODE_ADD );
    memcpy( mPositTracker, posTemp, ReduceSize );
#endif
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

#ifdef PK_MPI_ALL
  inline
  void
  GlobalizeAllVelocities( int aSimTick )
    {
    // Reduce Velocities
    ZeroNonLocalVelocities();
    int numberOfDoublesInPackage = SITE_COUNT * (sizeof(XYZ)/sizeof(double));

    static XYZ velTemp [ SITE_COUNT ];
    MPI_Allreduce( mVelocity , velTemp, numberOfDoublesInPackage, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
    memcpy(mVelocity, velTemp, numberOfDoublesInPackage*sizeof(double));
    }

  inline
  void
  GlobalizeAllPosition( int aSimTick )
    {
    // Assume phase 1.5 or phase 2
    // First OR reduce, then broadcast
    static XYZ positionsTemp [ SITE_COUNT ];
    int numberOfDoublesInPackage = SITE_COUNT * (sizeof(XYZ)/sizeof(double));
    ZeroNonLocalPositions();

    BegLogLine( PKFXLOG_GLOBALIZEPOSITIONS )
      << " GlobalizePosition "
      << EndLogLine;

    #ifdef PK_BLADE
//       mIntegerAllReduceStart.HitOE( PKTRACE_ALL_REDUCE_SUM,
//                                     "mIntegerAllReduce_Cntl",
//                                     mAddressSpaceId,
//                                     mIntegerAllReduceStart );

      Platform::Collective::AllReduce_Sum( (char *) mPosit, sizeof( mPosit ) );

//       mIntegerAllReduceFinis.HitOE( PKTRACE_ALL_REDUCE_SUM,
//                                     "mIntegerAllReduce_Cntl",
//                                     mAddressSpaceId,
//                                     mIntegerAllReduceFinis );
    #else // not BLADE ... MPI
//        mIntegerAllReduceStart.HitOE( PKTRACE_ALL_REDUCE_SUM,
//                                      "mIntegerAllReduce_Cntl",
//                                      mAddressSpaceId,
//                                      mIntegerAllReduceStart );

       // BGF Change sig so we might get to use tree on bgl system software AND avoid overhead of FP reduce verus int
       // NOTE: that means twice as many elements to reduce.
       MPI_Allreduce( mPosit, positionsTemp, 2 * numberOfDoublesInPackage, MPI_INT, MPI_SUM, Platform::Topology::cart_comm );

//        mIntegerAllReduceFinis.HitOE( PKTRACE_ALL_REDUCE_SUM,
//                                      "mIntegerAllReduce_Cntl",
//                                      mAddressSpaceId,
//                                      mIntegerAllReduceFinis );

       memcpy(mPosit, positionsTemp, numberOfDoublesInPackage*sizeof(double));
     #endif
    }
#endif

  inline
  void
  GlobalizePositions ( int aSimTick, int aRespaLevel, int aAssignmentViolation )
  {
  #ifdef PK_PARALLEL

  // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
  BegLogLine( 0 )
    << aSimTick
    << " Entering GlobalizePositions"
    << EndLogLine;

  mGlobalizePositionsStart.HitOE( PKTRACE_GLOBALIZEPOSITIONS,
                                  "GlobalizePositions",
                                  mAddressSpaceId,
                                  mGlobalizePositionsStart );

    #ifdef PK_PHASE5_SPLIT_COMMS
//     mGlobalizeBondedPositionsStart.HitOE( PKTRACE_GLOBALIZE_BONDED_POSITIONS,
// 					   "GlobalizeBondedPositions",
// 					   mAddressSpaceId,
// 					   mGlobalizeBondedPositionsStart );

     mNeighborhoodCommBondedDriver.Broadcast( aSimTick,
                aAssignmentViolation, 1 );
     
//      mGlobalizeBondedPositionsFinis.HitOE( PKTRACE_GLOBALIZE_BONDED_POSITIONS,
// 					   "GlobalizeBondedPositions",
// 					   mAddressSpaceId,
// 					   mGlobalizeBondedPositionsFinis );
    #endif

    mNeighborhoodCommDriver.Broadcast( aSimTick,
                                       aAssignmentViolation );

    mGlobalizePositionsFinis.HitOE( PKTRACE_GLOBALIZEPOSITIONS,
                                    "GlobalizePositions",
                                    mAddressSpaceId,
                                    mGlobalizePositionsFinis );

    // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
    BegLogLine( 0 )
      << aSimTick
      << " Leaving GlobalizePositions"
      << EndLogLine;

  #endif
  return;
  }

#if 0
VoxelIndex
GetVoxelIndex( XYZ* aPos, int aCount, XYZ& aInverseBox, iXYZ& aVoxDim )
  {
  assert( aCount >= 1 );
  VoxelIndex voxInd = 0;

  XYZ LowerLeft = aPos[ 0 ];
  double bX = LowerLeft.mX ;
  double bY = LowerLeft.mY ;
  double bZ = LowerLeft.mZ ;
  double cX = bX ;
  double cY = bY ;
  double cZ = bZ ;

  for( int i=1; i < aCount; i++ )
    {
    XYZ AtomLoc = aPos[ i ];
    double dX = AtomLoc.mX ;
    double dY = AtomLoc.mY ;
    double dZ = AtomLoc.mZ ;
    bX = min2(bX,dX) ;
    bY = min2(bY,dY) ;
    bZ = min2(bZ,dZ) ;
    cX = max2(cX,dX) ;
    cY = max2(cY,dY) ;
    cZ = max2(cZ,dZ) ;
    }
  
  XYZ mid;
  
  mid.mX = 0.5 * ( bX + cX );
  mid.mY = 0.5 * ( bY + cY );
  mid.mZ = 0.5 * ( bZ + cZ );

  voxInd = mRecBisTree->GetVoxelIndex( mid );

#if 0  

//  // Map to a unit box
//  double xBoxFrac = (midX * aInverseBox.mX) - 0.5 ;
//  double yBoxFrac = (midY * aInverseBox.mY) - 0.5 ;
//  double zBoxFrac = (midZ * aInverseBox.mZ) - 0.5 ;
//
//  // Image. This will give numbers in -0.5 <= A < 0.5 . I think .
//  double xImFrac = xBoxFrac - NearestInteger( xBoxFrac );
//  double yImFrac = yBoxFrac - NearestInteger( yBoxFrac );
//  double zImFrac = zBoxFrac - NearestInteger( zBoxFrac );
// 20051205 Note from Alex; above not working. NearestInteger rounds down,
// so we were getting -0.5 < A <= 0.5
//
//   if( xImFrac == 0.5 )
//       xImFrac = 0.4999999;

//   if( yImFrac == 0.5 )
//       yImFrac = 0.4999999;

//   if( zImFrac == 0.5 )
//       zImFrac = 0.4999999;

//  // Scale to voxmesh
//  double xVFrac = ( xImFrac + 0.5 ) * aVoxDim.mX ;
//  double yVFrac = ( yImFrac + 0.5 ) * aVoxDim.mY ;
//  double zVFrac = ( zImFrac + 0.5 ) * aVoxDim.mZ ;
//
//  // Convert to integer
//  int xV = xVFrac ;
//  int yV = yVFrac ;
//  int zV = zVFrac ;
//
//  voxInd = ( aVoxDim.mY * xV + yV ) * aVoxDim.mZ + zV;

   // Map to unit box
   double xBoxFrac = midX * aInverseBox.mX ;
   double yBoxFrac = midY * aInverseBox.mY ;
   double zBoxFrac = midZ * aInverseBox.mZ ;

//     // Keep the fractional part. 0.0 <= Frac < 1.0
//     double xImFrac = xBoxFrac - floor(xBoxFrac) ;
//     double yImFrac = yBoxFrac - floor(yBoxFrac) ;
//     double zImFrac = zBoxFrac - floor(zBoxFrac) ;

   // Find a reasonably good 'floor' for each of them
   // (we are expecting '0', but could be anything)
   double xFloor = int(xBoxFrac+(1024.0*1024.0)) - (1024*1024) ;
   double yFloor = int(yBoxFrac+(1024.0*1024.0)) - (1024*1024) ;
   double zFloor = int(zBoxFrac+(1024.0*1024.0)) - (1024*1024) ;

   // Keep the fractional part. 0.0 <= Frac < 1.0
   double xImFrac = xBoxFrac - xFloor ;
   double yImFrac = yBoxFrac - yFloor ;
   double zImFrac = zBoxFrac - zFloor ;

   // Scale to voxmesh
   double xVFrac = xImFrac * aVoxDim.mX ;
   double yVFrac = yImFrac * aVoxDim.mY ;
   double zVFrac = zImFrac * aVoxDim.mZ ;

   // Convert to integer
   int xV = xVFrac ;
   int yV = yVFrac ;
   int zV = zVFrac ;

   // Check that things are in range
   assert(xV >= 0 && xV < aVoxDim.mX ) ;
   assert(yV >= 0 && yV < aVoxDim.mY ) ;
   assert(zV >= 0 && zV < aVoxDim.mZ ) ;

   voxInd = ( aVoxDim.mY * xV + yV ) * aVoxDim.mZ + zV;
   assert (voxInd >= 0 && voxInd < aVoxDim.mX*aVoxDim.mY*aVoxDim.mZ) ;
#endif
  return voxInd;
  }
#endif

// VoxelIndex
// GetVoxelIndex( XYZ* aPos, int aCount, XYZ& aInverseBox, iXYZ& aVoxDim )
//   {
//   assert( aCount >= 1 );
//   VoxelIndex voxInd = 0;

//   XYZ LowerLeft = aPos[ 0 ];
//   double bX = LowerLeft.mX ;
//   double bY = LowerLeft.mY ;
//   double bZ = LowerLeft.mZ ;
//   double cX = bX ;
//   double cY = bY ;
//   double cZ = bZ ;

//   for( int i=1; i < aCount; i++ )
//     {
//     XYZ AtomLoc = aPos[ i ];
//     double dX = AtomLoc.mX ;
//     double dY = AtomLoc.mY ;
//     double dZ = AtomLoc.mZ ;
//     bX = min2(bX,dX) ;
//     bY = min2(bY,dY) ;
//     bZ = min2(bZ,dZ) ;
//     cX = max2(cX,dX) ;
//     cY = max2(cY,dY) ;
//     cZ = max2(cZ,dZ) ;
//     }

//   double midX = 0.5 * ( bX + cX );
//   double midY = 0.5 * ( bY + cY );
//   double midZ = 0.5 * ( bZ + cZ );

//   // Map to a unit box
//   double xBoxFrac = (midX * aInverseBox.mX) - 0.5 ;
//   double yBoxFrac = (midY * aInverseBox.mY) - 0.5 ;
//   double zBoxFrac = (midZ * aInverseBox.mZ) - 0.5 ;

//   // Image. This will give numbers in -0.5 <= A < 0.5 . I think . <== FAILED
//   double xImFrac = xBoxFrac - NearestInteger( xBoxFrac );
//   double yImFrac = yBoxFrac - NearestInteger( yBoxFrac );
//   double zImFrac = zBoxFrac - NearestInteger( zBoxFrac );

//   // Scale to voxmesh
//   double xVFrac = ( xImFrac + 0.5 ) * aVoxDim.mX ;
//   double yVFrac = ( yImFrac + 0.5 ) * aVoxDim.mY ;
//   double zVFrac = ( zImFrac + 0.5 ) * aVoxDim.mZ ;

//   // Convert to integer
//   int xV = xVFrac ;
//   int yV = yVFrac ;
//   int zV = zVFrac ;

//   if( !( xV >= 0 && xV < aVoxDim.mX )
//       || !( yV >= 0 && yV < aVoxDim.mY )
//       || !( zV >= 0 && zV < aVoxDim.mZ ) )
//     {
//     BegLogLine( 1 )
//       << "xV= " << xV
//       << " yV= " << yV
//       << " zV= " << zV
//       << " aVoxDim= " << aVoxDim
//       << " xVFrac= " << xVFrac
//       << " yVFrac= " << yVFrac
//       << " zVFrac= " << zVFrac
//       << " xImFrac= " << xImFrac
//       << " yImFrac= " << yImFrac
//       << " zImFrac= " << zImFrac
//       << " xBoxFrac= " << xBoxFrac
//       << " yBoxFrac= " << yBoxFrac
//       << " zBoxFrac= " << zBoxFrac
//       << " midX= " << midX
//       << " midY= " << midY
//       << " midZ= " << midZ
//       << EndLogLine;
//     }

//   voxInd = ( aVoxDim.mY * xV + yV ) * aVoxDim.mZ + zV;

//   assert( voxInd >= 0 && voxInd < (mVoxDimX * mVoxDimY * mVoxDimZ) );

//   return voxInd;
//   }
#if 0
  inline
  void
  GenerateVoxelIndeciesForPositionsInNeighborhood( int aSimTick )
    {
    iXYZ VoxelDim;
    VoxelDim.mX = mVoxDimX;
    VoxelDim.mY = mVoxDimY;
    VoxelDim.mZ = mVoxDimZ;

    for(int i=0; i < mInteractionStateManagerIF.mInteractionShortListSize; i++ )
      {
      FragId fragmentId   = mInteractionStateManagerIF.mInteractionShortList[ i ];

      XYZ* PosPtr     = mInteractionStateManagerIF.GetPositionPtr( fragmentId, 0 );
      int SiteCount   = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragmentId );


      VoxelIndex VoxIndex1 = GetVoxelIndex( PosPtr,
                                            SiteCount,
                                            mDynamicBoxInverseDimensionVector,
                                            VoxelDim );      

      mInteractionStateManagerIF.SetVoxelIndex( fragmentId, VoxIndex1 );
      }
    }
#endif

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
ManageSpacialDecomposition( int aSimTick )
  {
#ifdef PK_PARALLEL
#if (defined( PK_MPI_ALL ) && defined ( PK_PHASE4 ))

  mFragmentMigrationStart.HitOE( PKTRACE_FRAGMENT_MIGRATION,
                                 "FragmentMigration",
                                 mAddressSpaceId,
                                 mFragmentMigrationStart );

  BegLogLine( 0 )
    << "Entering ManageSpacialDecomposition(" << aSimTick << ")"
    << EndLogLine;

  // Upper bound to number of nearest neighbors

  char* CurMarker[ MAX_NEAREST_NEIGHBORS ];
  int   CurSize[ MAX_NEAREST_NEIGHBORS ];
  for (int i=0; i < mNumberOfNearestNeighbors; i++)
    {
    CurSize[ i ] = 0;
    CurMarker[ i ] = mSendBufferSD[ i ];
    }

  int SentFragmentCount=0;
  int ReceivedFragmentCount=0;

  int RemoveCount = 0;
  int RemoveList[1024];

  int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

  for( int fragmentIndex = 0; fragmentIndex  < NumLocalFrags; fragmentIndex++ )
    {
    FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( fragmentIndex );

    XYZ cog;
    // GetImagedFragCenter( fragId, cog );

    // XYZ &tempCog = mIntegratorStateManagerIF.GetTagAtomPosition( fragId );
    XYZ tempCog = mIntegratorStateManagerIF.GetFragmentPosition( fragId );
    NearestImageInPeriodicVolume( mCenterOfBoundingBox , tempCog, cog );

    ORBNode * leaf = mRecBisTree->NodeFromSimCoord( cog.mX, cog.mY, cog.mZ );
    assert( leaf );

    // Update voxel of a local fragment
    mIntegratorStateManagerIF.SetVoxelIndex( fragId, mRecBisTree->GetVoxelIndex( cog ) );

    // mVoxelIndexForFragment[ fragmentIndex ] = mRecBisTree->VoxelIndex( cog );

    int RankInLeaf = leaf->rank;

//     BegLogLine( aSimTick == 5091015 )
//       << "MIG " << fragId << " " << RankInLeaf
//       << EndLogLine;

    if( RankInLeaf != mAddressSpaceId )
      {
//       if( SentFragmentCount >= mRemovedFragsAllocated )
//         PLATFORM_ABORT( "SentFragmentCount >= mRemovedFragsAllocated" );

//       mRemovedFrags[ SentFragmentCount ] = fragId;

      SentFragmentCount++;
      int RankOfFragment       = RankInLeaf;
      int NearestNeighborIndex = GetNearestNeighborIndexForRank( RankOfFragment );

      if( NearestNeighborIndex == NOT_NEAREST_NEIGBOR )
        PLATFORM_ABORT( "NearestNeighborIndex == NOT_NEAREST_NEIGBOR" );

      // mPartitionList[ fragmentIndex ] = RankOfFragment;
      /////////////mIntegratorStateManagerIF.RemoveFragment( fragId );

      // We just removed a fragment from our local fragment list.
      /// NumLocalFrags--;

      SiteId firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      char* CurPtr = CurMarker[ NearestNeighborIndex ];
      int CurCount  = CurSize[ NearestNeighborIndex ];

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

      ////////printf("%4d: Migrating Frag %d from task %d to task %d.  %d sites\n", mAddressSpaceId, fragId, mAddressSpaceId, RankOfFragment, numberOfSites );

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

      /////mIntegratorStateManagerIF.RemoveFragment( fragId );

      if( RemoveCount > 1024 )
        PLATFORM_ABORT(" if( RemoveCount > 1024 ) ");
      RemoveList[ RemoveCount ] = fragId;
      RemoveCount++;

      ////mIntegratorStateManagerIF.Invalidate( fragId );

      assert( CurCount <= mSizeOfSDBuffers );

      CurSize[ NearestNeighborIndex ] = CurCount;
      CurMarker[ NearestNeighborIndex ] = CurPtr;
      }
    }

#if 1
  // Now remove the fragments that will be shipped out
  for(int r = 0; r < RemoveCount; r++)
    {
    mIntegratorStateManagerIF.RemoveFragment( RemoveList[ r ] );
    }
#endif

  // Post receive buffers
  for( int r=0; r < mNumberOfNearestNeighbors; r++ )
    {
    int msgrc;

    msgrc = MPI_Irecv( (void *) mRecvBufferSD[ r ],      // Post non-blocking receive for
                       mSizeOfSDBuffers,                 // each request.
                       MPI_CHAR,
                       mNearestNeighbors[ r ],
                       MPI_ANY_TAG,
                       Platform::Topology::cart_comm,// MPI_COMM_WORLD,
                       & mAllRequestsSD[ r ] );

    assert( msgrc == MPI_SUCCESS );
    }

  // Post send buffers
  for( int r=0; r < mNumberOfNearestNeighbors; r++ )
    {
    int msgrc;

    msgrc = MPI_Isend( (void *) mSendBufferSD[ r ],  // Post non-blocking receive for
                       CurSize[ r ],                 // each request.
                       MPI_CHAR,
                       mNearestNeighbors[ r ],
                       98,
                       Platform::Topology::cart_comm,// MPI_COMM_WORLD,
                       & mAllRequestsSD[ r + mNumberOfNearestNeighbors ] );

    assert( msgrc == MPI_SUCCESS );
    }

  // Waitall
  MPI_Waitall( 2*mNumberOfNearestNeighbors, mAllRequestsSD, mAllStatusSD );

  // Manage the incoming data
  for( int r=0; r < mNumberOfNearestNeighbors; r++ )
    {
    MPI_Status curStatus = mAllStatusSD[ r ];

    int count = 0;
    MPI_STATUS_COUNT( curStatus, count);

    char* CurBuff =  mRecvBufferSD[ r ];

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

      // ???? mPartitionList[ fragId ] = mAddressSpaceId;

      // int firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( FragId );
      int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

      for( int offset = 0; offset < numberOfSites; offset++ )
        {
        XYZ * xyz_ptr = (XYZ *) CurBuff;
        // mPosit[ siteId ] = *xyz_ptr;
        // memcpy( &mPosit[ siteId ], xyz_ptr, sizeof( XYZ ) );
        memcpy( &mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mPosition[ offset ],
                xyz_ptr,
                sizeof( XYZ ));

        assert( mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mPosition[ offset ].IsReasonable() );

        CurBuff += sizeof( XYZ );
        count -= sizeof( XYZ );
        assert( count > 0 );

        xyz_ptr = (XYZ *) CurBuff;
        // memcpy( &mVelocity[ siteId ], xyz_ptr, sizeof( XYZ ) );
        memcpy( &mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mVelocity[ offset ],
                xyz_ptr,
                sizeof( XYZ ));

        assert( mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mVelocity[ offset ].IsReasonable() );

        CurBuff += sizeof( XYZ );
        count -= sizeof( XYZ );
        assert( count > 0 );

        xyz_ptr = (XYZ *) CurBuff;
        // memcpy( &mVelocity[ siteId ], xyz_ptr, sizeof( XYZ ) );
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
      // mVoxelIndexForFragment[ FragId ] = mRecBisTree->VoxelIndex( FragId );
      mIntegratorStateManagerIF.UpdateVoxelIndex( fragId );

      #ifndef NDEBUG
      // XYZ &tempCog = mIntegratorStateManagerIF.GetTagAtomPosition( fragId );
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
#endif
#endif

  BegLogLine( 0 )
    << "Leaving ManageSpacialDecomposition(" << aSimTick << ")"
    << EndLogLine;

  return;
  }

void
ManageSpacialDecompositionAllToAll( int aSimTick )
  {
#ifdef PK_PARALLEL
#if defined ( PK_PHASE4 )

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

#ifdef PK_BLADE_SPI
    mSDRecvCounts[ NRank ]  = 1;
#endif
    }

//   // char* CurMarker[ MAX_NEAREST_NEIGHBORS ];
//   int   CurSize[ MAX_NEAREST_NEIGHBORS ];
//   for (int i=0; i < mNumberOfNearestNeighbors; i++)
//     {
//     mSDSendCounts[ i ] = 0;
//     // CurMarker[ i ] = mSendBufferSD[ i ];
//     }

  int SentFragmentCount=0;
  int ReceivedFragmentCount=0;

  int RemoveCount = 0;
  int RemoveList[1024];

  int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

//   BegLogLine( aSimTick == 10 )
//     << aSimTick
//     << " GetNumberOfLocalFrags: " << NumLocalFrags
//     << EndLogLine;

  for( int fragmentIndex = 0; fragmentIndex  < NumLocalFrags; fragmentIndex ++ )
    {
    FragId fragId = mIntegratorStateManagerIF.GetNthFragmentId( fragmentIndex );    

    XYZ cog;
    // GetImagedFragCenter( fragId, cog );

    // XYZ &tempCog = mIntegratorStateManagerIF.GetTagAtomPosition( fragId );
    XYZ tempCog = mIntegratorStateManagerIF.GetFragmentPosition( fragId );
    NearestImageInPeriodicVolume( mCenterOfBoundingBox , tempCog, cog );

    ORBNode * leaf = mRecBisTree->NodeFromSimCoord( cog.mX, cog.mY, cog.mZ );
    assert( leaf );
    

    // Update voxel of a local fragment
    mIntegratorStateManagerIF.SetVoxelIndex( fragId, mRecBisTree->GetVoxelIndex( cog ) );

    // mVoxelIndexForFragment[ fragmentIndex ] = mRecBisTree->VoxelIndex( cog );

    int RankInLeaf = leaf->rank;

//     BegLogLine( (fragId == 2087) && (aSimTick == 10) )
//       << aSimTick
//       << " RankInLeaf: " << RankInLeaf
//       << " cog: " << cog
//       << EndLogLine;

    if( RankInLeaf != mAddressSpaceId )
      {
      SentFragmentCount++;
      int RankOfFragment       = RankInLeaf;
      int NearestNeighborIndex = GetNearestNeighborIndexForRank( RankOfFragment );

      assert( NearestNeighborIndex != NOT_NEAREST_NEIGBOR );

      // mPartitionList[ fragmentIndex ] = RankOfFragment;
      /////////////mIntegratorStateManagerIF.RemoveFragment( fragId );

      // We just removed a fragment from our local fragment list.
      // NumLocalFrags--;

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

      ////////printf("%4d: Migrating Frag %d from task %d to task %d.  %d sites\n", mAddressSpaceId, fragId, mAddressSpaceId, RankOfFragment, numberOfSites );

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

      // mIntegratorStateManagerIF.RemoveFragment( fragId );

      if( RemoveCount > 1024 )
        PLATFORM_ABORT(" if( RemoveCount > 1024 ) ");
      RemoveList[ RemoveCount ] = fragId;
      RemoveCount++;

      ////mIntegratorStateManagerIF.Invalidate( fragId );

      assert( CurCount <= mSizeOfSDBuffers );

      mSDSendCounts[ RankInLeaf ] = CurCount;
      // CurMarker[ NearestNeighborIndex ] = CurPtr;
      }
    }

  for(int r = 0; r < RemoveCount; r++)
    {
    mIntegratorStateManagerIF.RemoveFragment( RemoveList[ r ] );
    }

  // MPI_Barrier( MPI_COMM_WORLD );

#ifdef PK_MPI_ALL
    Platform::Collective::Alltoallv( mSDSendCounts,
                                     mSDSendCountsCounts,
                                     mSDSendCountsOffsets,
                                     sizeof( int ),
                                     mSDRecvCounts,
                                     mSDRecvCountsCounts,
                                     mSDRecvCountsOffsets,
                                     sizeof( int ) );
#endif

#ifdef PK_BLADE_SPI
    // If nothing to say, still need to send a packet
    for( int i=0; i<mNumberOfNearestNeighbors; i++ )
      {
      int NRank = mNearestNeighbors[ i ];
      if( mSDSendCounts[ NRank ] == 0 )
        mSDSendCounts[ NRank ] = 1;
      }
#endif
//   MPI_Alltoallv( mSDSendCounts,
//                  mSDSendCountsCounts,
//                  mSDSendCountsOffsets,
//                  MPI_INT,
//                  mSDRecvCounts,
//                  mSDRecvCountsCounts,
//                  mSDRecvCountsOffsets,
//                  MPI_INT, MPI_COMM_WORLD );

  Platform::Collective::Alltoallv( mSendBufferSD,
                                  mSDSendCounts,
                                  mSDSendOffsets,
                                  sizeof( char ),
                                  mRecvBufferSD,
                                  mSDRecvCounts,
                                  mSDRecvOffsets,
                                  sizeof( char ) );
//   MPI_Alltoallv( mSendBufferSD,
//                  mSDSendCounts,
//                  mSDSendOffsets,
//                  MPI_CHAR,
//                  mRecvBufferSD,
//                  mSDRecvCounts,
//                  mSDRecvOffsets,
//                  MPI_CHAR, MPI_COMM_WORLD );

  // Manage the incoming data
  for( int r=0; r < mNumberOfNearestNeighbors; r++ )
    {
    int NRank = mNearestNeighbors[ r ];
    int count = mSDRecvCounts[ NRank ];

    // Signal that the node had nothing to say
#ifdef PK_BLADE_SPI
    if( count == 1 )
      continue;
#endif

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
        // mPosit[ siteId ] = *xyz_ptr;
        // memcpy( &mPosit[ siteId ], xyz_ptr, sizeof( XYZ ) );
//         mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ]
//          .mIntegratorFragmentState.SetPosition(offset, *xyz_ptr) ;

        memcpy( &mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mPosition[ offset ],
                xyz_ptr,
                sizeof( XYZ ));

//         assert(mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ]
//               .mIntegratorFragmentState.GetPosition(offset)
//               .IsReasonable() ) ;

        CurBuff += sizeof( XYZ );
        count -= sizeof( XYZ );
        assert( count > 0 );

        xyz_ptr = (XYZ *) CurBuff;
        // memcpy( &mVelocity[ siteId ], xyz_ptr, sizeof( XYZ ) );
        memcpy( &mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mVelocity[ offset ],
                xyz_ptr,
                sizeof( XYZ ));

        assert( mIntegratorStateManagerIF.mIntegratorStateTable[ fragId ].mIntegratorFragmentState.mVelocity[ offset ].IsReasonable() );

        CurBuff += sizeof( XYZ );
        count -= sizeof( XYZ );
        assert( count > 0 );

        xyz_ptr = (XYZ *) CurBuff;
        // memcpy( &mVelocity[ siteId ], xyz_ptr, sizeof( XYZ ) );
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
      // mVoxelIndexForFragment[ FragId ] = mRecBisTree->VoxelIndex( FragId );
      mIntegratorStateManagerIF.UpdateVoxelIndex( fragId );

      #ifndef NDEBUG
      // XYZ &tempCog = mIntegratorStateManagerIF.GetTagAtomPosition( fragId );
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
#endif
#endif

  BegLogLine( 0 )
    << "Leaving ManageSpacialDecomposition(" << aSimTick << ")"
    << EndLogLine;

  return;
  }

  int
  CheckSiteDisplacementViolation()
  {
// #if 0
//   #ifdef PK_PARALLEL
//     #ifdef PK_BLADE
//       double  isViolation[ 256 ];
//       double  isViolationTemp[256];

//       isViolation[ 0 ] = (double) CheckSiteDisplacement();
//       Platform::Collective::FP_AllReduce_Sum( isViolation, isViolationTemp, 256 );
//       isViolation[0] = isViolationTemp[0];
//       return (int) isViolation[ 0 ];
//     #else
//       int isViolationTemp = CheckSiteDisplacement();
//       int isViolationTemp1;

//       mGuardZoneViolationSignalReduceStart.HitOE( PKTRACE_GUARDZONE_VIOLATION_REDUCTION,
//                                                   "mGuardZoneViolationSignalReduce",
//                                                   mAddressSpaceId,
//                                                   mGuardZoneViolationSignalReduceStart );

//       // Reduce doing a bitwise OR. There are 2 signals in the violation
//       // 1. Bit 0: Verlet violation
//       // 2. Bit 1: Assignment violation
//       MPI_Allreduce( &isViolationTemp, &isViolationTemp1, 1, MPI_INT, MPI_BOR, MPI_COMM_WORLD );

//       mGuardZoneViolationSignalReduceFinis.HitOE( PKTRACE_GUARDZONE_VIOLATION_REDUCTION,
//                                                   "mGuardZoneViolationSignalReduce",
//                                                   mAddressSpaceId,
//                                                   mGuardZoneViolationSignalReduceFinis );

//       return isViolationTemp1;
//     #endif
//   #else
//      return CheckSiteDisplacement();
//   #endif
// #endif

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

// TDynamicVariableManager() { }

// THINK about having this take the SimTick too.

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

// void PrecomputeNeighborListBoundingBoxData( int all=0 )
//   {

//   mFatCompPrecomputeStart.HitOE( PKTRACE_FATCOMP,
//                                  "mFatCompPrecompute",
//                                  mAddressSpaceId,
//                                  mFatCompPrecomputeStart );

//   // We can run the algorithm either by precomputing the in-range fragments,
//   // or the way we used to run it by iterating through all the fragments and deciding
//   // about 'range' just before we call the IFP each time. It is faster to precompute,
//   // but we never throw away any code ...
//   NSQ_NeighbourList.Setup( ::DynVarMgrIF.mDynamicBoxDimensionVector,
//                            ::DynVarMgrIF.mDynamicBoxInverseDimensionVector ) ;

//   unsigned int IrreducibleFragmentCount = GetIrreducibleFragmentCount() ;
//   double GuardZone = GetCurrentGuardZone();

//   for (int pSourceI = 0; pSourceI < IrreducibleFragmentCount; pSourceI++)
//     {
//     if( !all )
//         continue;

//     // int SourceFragmentId = IrreducibleFragmentOperandIF::GetIrreducibleFragmentId(mClientState.mDynVarMgrIF,pSourceI);
//     int SourceFragmentId = pSourceI;
//     int SourceTagSiteIndex = MSD_IF::GetTagAtomIndex( SourceFragmentId );

//     double SourceFragmentExtent = MSD_IF::GetFragmentExtent( SourceFragmentId );

//     int FragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceFragmentId );
//     //XYZ LowerLeft = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,0) ;
//     SiteId base   = GetNthIrreducibleFragmentFirstSiteIndex( SourceFragmentId );
//     XYZ LowerLeft = GetPositionFromFullTable( base );
//     XYZ UpperRight = LowerLeft ;
//     {
//     double bX = LowerLeft.mX ;
//     double bY = LowerLeft.mY ;
//     double bZ = LowerLeft.mZ ;
//     double cX = bX ;
//     double cY = bY ;
//     double cZ = bZ ;

//     // Restructure this loop to 'nickle-and-dime' it, because in the initial implementation it represents replicated calculation on all
//     // nodes. (Another day we will be more selective, and only have each node calculate for fragments it is interested in)
//     for (int pIndexI = 1 ; pIndexI < FragmentSiteCount; pIndexI += 1 )
//       {
//       // XYZ AtomLoc = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,pIndexI) ;
//       XYZ AtomLoc = GetPositionFromFullTable( base+pIndexI ) ;

//       double dX = AtomLoc.mX ;
//       double dY = AtomLoc.mY ;
//       double dZ = AtomLoc.mZ ;
//       bX = min(bX,dX) ;
//       bY = min(bY,dY) ;
//       bZ = min(bZ,dZ) ;
//       cX = max(cX,dX) ;
//       cY = max(cY,dY) ;
//       cZ = max(cZ,dZ) ;
//       } /* endfor */

//     LowerLeft.mX = bX ;
//     LowerLeft.mY = bY ;
//     LowerLeft.mZ = bZ ;
//     UpperRight.mX = cX ;
//     UpperRight.mY = cY ;
//     UpperRight.mZ = cZ ;
//     }

//     // Need to expand the bounding box to become verlet list safe.
//     LowerLeft.mX -= ( 0.5 * GuardZone );
//     LowerLeft.mY -= ( 0.5 * GuardZone );
//     LowerLeft.mZ -= ( 0.5 * GuardZone );
//     UpperRight.mX += ( 0.5 * GuardZone );
//     UpperRight.mY += ( 0.5 * GuardZone );
//     UpperRight.mZ += ( 0.5 * GuardZone );

//     XYZ MidXYZ = 0.5*(LowerLeft+UpperRight) ;
//     double e2max = DBL_MIN ; // Cook the 'max distance' so it is never zero, and the sqrt(0) special case is not needed
//     for (int qIndexI = 0 ; qIndexI < FragmentSiteCount; qIndexI += 1 )
//       {
//       // XYZ AtomLoc = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,qIndexI) ;
//       XYZ AtomLoc = GetPositionFromFullTable( base+qIndexI ) ;
//       double e2 = ( AtomLoc - MidXYZ ).LengthSquared() ;
//       e2max = max( e2max, e2 ) ;
//       } /* endfor */

//     BegLogLine( PKFXLOG_NSQSOURCEFRAG )
//       << "NSQ Fragment " << pSourceI
//       << " bounds " << LowerLeft << UpperRight
//       << " boundsize " << ( UpperRight - LowerLeft )
//       << " centre " << MidXYZ
//       << " extent " << sqrt( e2max ) << 0.5 * ( UpperRight - LowerLeft )
//       << " tagext " << SourceFragmentExtent
//       << GetPositionFromFullTable( base+SourceTagSiteIndex )
//       << EndLogLine ;

//     NSQ_NeighbourList.SetXYZE( pSourceI,
//                                MidXYZ,
//                                e2max,
//                                0.5 * ( UpperRight - LowerLeft ) ) ;
//     }

//   NSQ_NeighbourList.CompleteXYZE( IrreducibleFragmentCount ) ;

//   mFatCompPrecomputeFinis.HitOE( PKTRACE_FATCOMP,
//                                  "mFatCompPrecompute",
//                                  mAddressSpaceId,
//                                  mFatCompPrecomputeFinis );
//   }

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

#ifdef PK_PHASE4
int IsIntegratorFragmentLocal( FragId aFragId )
  {
  unsigned int rc = !mIntegratorStateManagerIF.IsInvalid( aFragId );
  return rc;
  }

// void UpdateVoxelIdsForFragment()
//   {
//   // THIS IS HERE FOR DEBUGGING ONLY
//   // PrecomputeNeighborListBoundingBoxData();

//   for( int i=0; i<FRAGMENT_COUNT; i++)
//     {
//     if( !IsFragmentLocal( i ) )
//       {
//       mVoxelIdForFragment[ i ] = -1;
//       continue;
//       }

//     mVoxelIdForFragment[ i ] = GetVoxelIndexForFragment( i );
//     }
//   }

#ifdef PK_PHASE5_PROTOTYPE
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
    
      if(  CheckeredTemplate [ x & 0x1 ] [ y & 0x1 ][ z & 0x1 ] )             
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

#define PK_PHASE5_DEBUG
#if 1
NodePairRecord* mNodePairRecords;
int             mNodePairRecordsCount;
int             mNodePairRecordsAllocatedCount;

struct AssignedWeightLink
  {
  AssignedWeightLink*  mNextLink;
  NodePairRecord       mNodePairRecord;  
  // AssignedWeightLink*  mPrevLink;
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

NodeAssignmentRecord_T* mFullSkinNodeAssignmentTable;
NodeAssignmentRecord_T* mSparseSkinNodeAssignmentTable;
char* mLoadBalanceHeap;
int   mLoadBalanceHeapSize;
int   mLoadBalanceHeapIndex;

int*    A2ARecvCount;
int*    A2ARecvOffset;
FragId* RecvFragmentBuffer;

NodeId** mFullSkinTable;
int      mNumberOfNodesOnFullSkin;

int*     mSparseSkinsCounts;
NodeId** mSparseSkins;

int*  mSparseSkinSendNodes;
int      mSparseSkinSendNodesCount;

#define BUFFER_CHUNK_SIZE ( 10 * 1024 * 1024 )
char**  mMemoryBuffer;
int     mMemoryBufferIndex;
int     mMemoryBufferCount;

char*   mCurrentBuffer;
int     mCurrentBufferIndex;

#define NON_EXISTANCE_VAL -1
#define DIM_H 3

#define EXPAND_BLOCK (10 * 1024 * 1024)

#if 0
void GetProcVector( int* aPointA, int* aPointB, 
                    int* aMachineDim, int* aVectorAB )
  {
  for( int i=0; i < DIM_H; i++ )
    {
    int low = min( aPointA[ i ], aPointB[ i ] );
    int high = max( aPointA[ i ], aPointB[ i ] );
    
    int dist  = high - low;
    int dist2 = ( low + aMachineDim[ i ] ) - high;
    
    if( dist < dist2 )
      aVectorAB[ i ] = dist;
    else
      aVectorAB[ i ] = dist2;
    }  
  }

iXYZ* mTempOptSignature;

static int CompareIntXYZ(const void *elem1, const void *elem2)
  {
  iXYZ* e1 = (iXYZ *) elem1;
  iXYZ* e2 = (iXYZ *) elem2;

  if ( *e1.mX < *e2.mX )
      return -1;
  else if( *e1.mX == *e2.mX )
      if ( *e1.mY < *e2.mY )
          return -1;
      else if( *e1.mY == *e2.mY )
          if ( *e1.mZ < *e2.mZ )
              return -1;
          else if( *e1.mZ == *e2.mZ )
              return 0;
          else
              return 1;
      else
          return 1;
  else
      return 1;
  }

struct HashTableKey
  {
  iXYZ*           mNodeOptKey;
  int             mNodeOptKeyCount;      
  };

struct HashTableEntry
  {
  HashTableKey    mKey;
  HashTableEntry* mNextEntry;
  };
  
struct HashTableRecord
  {
  HashTableEntry* mFirstEntry;
  HashTableEntry* mLastEntry;
  };

static int CompareTableKey(const void* elem1, const void *elem2)
  {
  HashTableKey* e1 = (HashTableKey *) elem1;
  HashTableKey* e2 = (HashTableKey *) elem2;
  
  if( e1.mNodeOptKeyCount < e2.mNodeOptKeyCount )
    return -1;
  else if( e1.mNodeOptKeyCount == e2.mNodeOptKeyCount )
    {
    for( int i=0; i < e1.mNodeOptKeyCount; i ++ )
      {
      if( e1.mNodeOptKey[ i ] != e2.mNodeOptKey[ i ] )
        return -1;
      }

    return 0;
    }
  else 
    return 1;
  }

class NodeOptionsHashTable
  {
  public:      
      
  int             mHashTableSize;
  HashTableRecord* mHashTable;

  void 
  Init(int aSize)
    {
    mHashTableSize = aSize + 1; // Hashcode is the number of node options

    mHashTable = (HashTableRecord *) malloc( sizeof( HashTableRecord ) * mHashTableSize );
    if( mHashTable == NULL )
      PLATFORM_ABORT( "mHashTable == NULL" );

    for( int i=0; i<mHashTableSize; i++ )
      {
      mHashTable[ i ].mFirstEntry = NULL;
      mHashTable[ i ].mLastEntry = NULL;
      }
    }

   HashTableEntry* 
   Exists( HashTableRecord* aRecordPtr, iXYZ* aKey, int aKeyCount )
     {
     HashTableEntry CurEntry = aRecordPtr->mFirstEntry;
     HashTableEntry ToReturn = NULL;
     
     HashTableKey ThisKey;
     ThisKey.mNodeOptKey      = aKey;
     ThisKey.mNodeOptKeyCount = aKeyCount;
     
     while( CurEntry != NULL )
       {
       if( CompareHashKey( CurEntry->mKey, ThisKey ) == 0 )
         return CurEntry;
       
       CurEntry = CurEntry->mNextEntry;
       }

     return ToReturn;
     }

   // Adds the entry if one is missing, returns the pointer to the entry      
   HashTableEntry* 
   AddEntry( iXYZ* aKey, int aKeyCount )
     {
     assert( aKey != NULL );
     
     assert( aKeyCount > 0 && aKeyCount < mHashTableSize ); 
     
     HashTableRecord* RecordForCount = & mHashTable[ aKeyCount ];
     
     // Search for the record.
     Platform::Algorithm::Qsort( aKey, aKeyCount, sizeof( iXYZ ), CompareIntXYZ );
     
     HashTableEntry* KeyEntry = Exists( aKey, aKeyCount );
     if( KeyEntry == NULL )
       {
       // Insert at the end of the linked list     
       if( RecordForCount->mFirstEntry == NULL )
         {
         RecordForCount->mFirstEntry = (HashTableEntry *) malloc( sizeof( HashTableEntry ) );
         if( RecordForCount->mFirstEntry == NULL )
           PLATFORM_ABORT( "RecordForCount->mFirstEntry" );
         
         RecordForCount->mLastEntry = RecordForCount->mFirstEntry;
         }
       else
         {
         RecordForCount->mLastEntry->mNextEntry = (HashTableEntry *) malloc( sizeof( HashTableEntry ) );
         if( RecordForCount->mLastEntry->mNextEntry == NULL )
           PLATFORM_ABORT( "RecordForCount->mLastEntry->mNextEntry" );
         
         RecordForCount->mLastEntry = RecordForCount->mLastEntry->mNextEntry;
         }
       
       RecordForCount->mLastEntry->mNextEntry = NULL;
       RecordForCount->mLastEntry->mKey.mNodeOptKeyCount = aKeyCount;
       
       RecordForCount->mLastEntry->mKey.mNodeOptKey = (iXYZ *) malloc( sizeof( iXYZ ) * aKeyCount );
       if( RecordForCount->mLastEntry->mKey.mNodeOptKey == NULL )
         PLATFORM_ABORT( "RecordForCount->mLastEntry->mKey.mNodeOptKey = NULL" );
       
       memcpy( RecordForCount->mLastEntry->mKey.mNodeOptKey, aKey, aKeyCount * sizeof( iXYZ ) );       

       KeyEntry = RecordForCount->mLastEntry;
       }
     
     return KeyEntry;
     }
      
  void 
  Finalize()
    {
    for( int i = 0; i < mHashTableSize; i++ )
      {
      HashTableEntry* CurEntry = mHashTable[ i ].mFirstEntry;
      HashTableEntry* PrevEntry = NULL;
      while( CurEntry != NULL )
        {
        if( CurEntry->mKey.mNodeOptKey != NULL )
          free( CurEntry->mKey.mNodeOptKey );

        
        HashTableEntry* TmpCurEntry = CurEntry;
        CurEntry = CurEntry->mNextEntry;
        TmpCurEntry->mNextEntry = PrevEntry;
        PrevEntry = TmpCurEntry;
        }

      CurEntry = mHashTable[ i ].mLastEntry;
      
      while( CurEntry != NULL )
        {
        HashTableEntry* NextEntry = CurEntry->mNextEntry;
        free( CurEntry );
        CurEntry = NextEntry;
        }
      }
    
    free( mHashTable );
    mHashTable = NULL;
    }
  };

NodeOptionsHashTable mNodeOptionsHashTable;

HashTableEntry* GetOptionsHashTableEntry( NodeId aNodeIdI, NodeId aNodeIdJ, NodeId* aIntersectionList, int aIntersectionListCount )
  {
  int NodeCoordsI[ DIM_H ];
  int NodeCoordsJ[ DIM_H ];
  
  Platform::Topology::GetCoordsFromRank( aNodeIdI, 
                                         &NodeCoordsI[ 0 ],
                                         &NodeCoordsI[ 1 ],
                                         &NodeCoordsI[ 2 ] );

  Platform::Topology::GetCoordsFromRank( aNodeIdJ, 
                                         &NodeCoordsJ[ 0 ],
                                         &NodeCoordsJ[ 1 ],
                                         &NodeCoordsJ[ 2 ] );
  int MachineDim[ DIM_H ];
  MachineDim[ 0 ] = mProcs_x;
  MachineDim[ 1 ] = mProcs_y;
  MachineDim[ 2 ] = mProcs_z;
  
  int IJVec[ DIM_H ];

  GetProcVector( NodeCoordsI, NodeCoordsJ,
                 MachineDim, IJVec );  
  
  for( int i = 0; i < aIntersectionListCount; i++ )
    {
    NodeId OptNode = aIntersectionList[ i ];
    
    int OptNodeCoords[ 3 ];
    Platform::Topology::GetCoordsFromRank( OptNode,
                                           & OptNodeCoords[ 0 ],
                                           & OptNodeCoords[ 1 ],
                                           & OptNodeCoords[ 2 ] );
    
    int IOptVec[ 3 ];
    GetProcVector( NodeCoordsI, OptNodeCoords, MachineDim, IOptVec );

    mTempOptSignature[ i ].mX = IOptVec[ 0 ] + IJVec[ 0 ];
    mTempOptSignature[ i ].mY = IOptVec[ 1 ] + IJVec[ 1 ];
    mTempOptSignature[ i ].mZ = IOptVec[ 2 ] + IJVec[ 2 ];
    }
  
  // Check the database of signatures to see if this signature is around
  // if not insert 
  HastTableEntry* HashEntry = mNodeOptionsHashTable.AddEntry( mTempOptSignature,
                                                              aIntersectionListCount );

  if( HashEntry == NULL )
    PLATFORM_ABORT( "HashEntry == NULL" );
  
  return HashEntry;
  }
#endif

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

template< class T >
void* ExpandArray( T** aArray, int& aArrayCount, int aExpandCount )
  {
  void *Memory;
  
  int NewCount = aArrayCount + aExpandCount;
  int NewSize  = sizeof( T ) * NewCount;
  
  BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
    << "NewCount: " << NewCount
    << " NewSize: " << NewSize
    << " aExpandCount: " << aExpandCount
    << EndLogLine;

  Memory = malloc( NewSize );
  if( Memory == NULL )
    {
    return Memory;
    // PLATFORM_ABORT( "Memory == NULL" );
    }

  if( aArray != NULL )
    {
    memcpy( Memory, *aArray, aArrayCount * sizeof( T ) );
    free( *aArray );
    }

  *aArray = (T *) Memory;
  aArrayCount = NewCount;
  return Memory;
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
  unsigned long LocalFragmentCount = mIntegratorStateManagerIF.GetNumberOfLocalFragments();
  Platform::Collective::GlobalMax( &LocalFragmentCount, 
                                   &LocalFragmentCountMax );

  BegLogLine( 1 )
    << "LocalFragmentCountMax= " << LocalFragmentCountMax
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
  
  // Every node send their local fragments to node 0
  // A2ASendCount[ 0 ] = LocalFragmentCount;
  
  FragId* LocalFragments = mIntegratorStateManagerIF.GetLocalFragmentPtr();
  
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
  
#if 0
  for( int i=0; i<MachineSize; i++ )
    {        
    int Count = A2ARecvCount[ i ];
    int Offset = A2ARecvOffset[ i ];
    
    for( int j=0; j<Count; j++ )
      {
      BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
        << "FRAGS " << i << " " << RecvFragmentBuffer[ Offset + j ]
        << EndLogLine;
      }
    }
#endif

  BegLogLine( 1 )
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
  
//   mLoadBalanceHeapIndex = 0;      
//   mLoadBalanceHeapSize = 100 * 1024 * 1024;
//   mLoadBalanceHeap = (char *) malloc( mLoadBalanceHeapSize );
//   if( mLoadBalanceHeap == NULL )
//     PLATFORM_ABORT( "mLoadBalanceHeap == NULL" );    
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
  
  double RadiusSquared = mRadiusOfSphere * mRadiusOfSphere;
  double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
  double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
  
  double Cutoff = SwitchLowerCutoff + SwitchDelta;
  
//   int  NodesOnSkinCountI = 0;
//   int* NodesOnSkinI  = (int *) malloc( sizeof( int ) * MachineSize );
//   if( NodesOnSkinI == NULL )
//     PLATFORM_ABORT( "NodesOnSkinI == NULL" );
  
//   int  NodesOnSkinCountJ = 0;
//   int* NodesOnSkinJ  = (int *) malloc( sizeof( int ) * MachineSize );
//   if( NodesOnSkinJ == NULL )
//     PLATFORM_ABORT( "NodesOnSkinJ == NULL" );
  
  int IntersectionListCount = 0;
  NodeId* IntersectionList = (NodeId *) malloc( sizeof( NodeId ) * MachineSize );
  if( IntersectionList == NULL )
    PLATFORM_ABORT( "IntersectionList == NULL" );    
  
  double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;
  
  double InRangeCutoff = SwitchUpperCutoff  ;
  
  double GuardZone = GetCurrentVerletGuardZone();
  
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
                                        mRadiusOfSphere + P5_EXP,
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
                                          mRadiusOfSphere + P5_EXP,
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
      
//       mRecBisTree->RanksInNeighborhoodSkin( xI, yI, zI,
//                                             mRadiusOfSphere,
//                                             NodesOnSkinI,
//                                             NodesOnSkinCountI,
//                                             MachineSize );
      
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
            // if( 1 )
          {
          // 
//           mRecBisTree->RanksInNeighborhoodSkin( xJ, yJ, zJ,
//                                                 mRadiusOfSphere,
//                                                 NodesOnSkinJ,
//                                                 NodesOnSkinCountJ,
//                                                 MachineSize );
          
          // Intersect the skins and get the intersection.
          IntersectSortedList< NodeId >( mFullSkinTable[ NodeI ], mNumberOfNodesOnFullSkin,
                                         mFullSkinTable[ NodeJ ], mNumberOfNodesOnFullSkin,
                                        IntersectionList, IntersectionListCount );
                    
          BegLogLine( (Platform::Topology::GetAddressSpaceId() == 0 ) && ( NodeI == 348 && NodeJ == 485 ))
          // BegLogLine( IntersectionListCount == 1 )
             << "Pair: { " << NodeI << " , " << NodeJ << " } is accounted for" 
             << " IntersectionListCount: " << IntersectionListCount
             << EndLogLine;

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
                            
              double ExtentA = Extents[ fragIdI ];
              double ExtentB = Extents[ fragIdJ ];
              XYZ & FragCenterA = FragCenters[ fragIdI ];
              XYZ & FragCenterB = FragCenters[ fragIdJ ];
              
              XYZ FragCenterBImg;
              NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
              
              double thA= (GuardRange+ExtentA+ExtentB) ;
              double dsqThreshold = FragCenterA.DistanceSquared(FragCenterBImg) - (thA*thA) ;
              
//               BegLogLine( ( Platform::Topology::GetAddressSpaceId() == 1 ) &&
//                           ( fragIdI == 851 && fragIdJ == 1428 ) ||
//                           ( fragIdJ == 851 && fragIdI == 1428 ) )
//                   << "FragIdI: " << fragIdI
//                   << " FragIdJ: " << fragIdJ
//                   << " ExtentA: " << ExtentA
//                   << " ExtentB: " << ExtentB
//                   << " thA: " << thA
//                   << " dsqThreshold: " << dsqThreshold
//                   << " DistanceSquared(): " << FragCenterA.DistanceSquared(FragCenterBImg)
//                   << EndLogLine;
              
              if( dsqThreshold < 0.0 )
                {
                // This pair counts
                LBWeightType ModWeight = (LBWeightType) InteractionModel::GetWeight( fragIdI, fragIdJ, this );

                // ModWeight could be zero even in here, due to exclusions
#if 0
                if( ModWeight <= 0.0 )
                  {
                  // If we get here, then frag centers are within range.
                  // No weight should be equal or less the zero here.
                  BegLogLine( 1 )
                    << "fragIdI: " << fragIdI
                    << " fragIdJ: " << fragIdJ
                    << " thA: " << thA
                    << " dsqThreshold: " << dsqThreshold
                    << " ExtentA: " << ExtentA
                    << " ExtentB: " << ExtentB
                    << " FragCenterA: " << FragCenterA
                    << " FragCenterBImg: " << FragCenterBImg
                    << " ModWeight: " << ModWeight
                    << EndLogLine;

                  InteractionModel::GetWeight( fragIdI, fragIdJ, this, 1 );

                  PLATFORM_ABORT("ERROR: ModWeight <= 0.0");
                  }
#endif

                LBWeightType WeightForFragPair = ModWeight;
                if( WeightForFragPair < 0.0 )
                  {
                  BegLogLine( 1 )
                    << "FragIdI: " << fragIdI
                    << " FragIdJ: " << fragIdJ
                    << " WeightForFragPair: " << WeightForFragPair
                    << " ModWeight: " << ModWeight
                    // << " OutsideRangeCount: " << OutsideRangeCount
                    << " InteractionModel::GetWeight( fragIdI, fragIdJ, this ): " << InteractionModel::GetWeight( fragIdI, fragIdJ, this )
                    << EndLogLine;
                  
                  PLATFORM_ABORT( "WeightForFragPair < 0.0" );
                  }                  
                NodeIJWeight += WeightForFragPair;
                }
              }
            }
        
          // if( NodeIJWeight > 0.0 )
            {
            TotalWeightCount++;
            
            if( NodeIJWeight > MaxWeight )
              MaxWeight = NodeIJWeight;

            if( NodeIJWeight < MinWeight )
              MinWeight = NodeIJWeight;

            AvgWeight += NodeIJWeight;
            
            // There might not be any interactions from the pair of nodes at the moment,
            // But the pair still has to get assigned. Need to give it some imaginary weight. 
            // NEED to come back here and think some more about the best way to handle this
            // Putting a weight of zero, might screw up the load balancing system.
            // if( NodeIJWeight <= 0.0 )
            //   NodeIJWeight = MinWeight;
            
            // BegLogLine( ( NodeI == 146 && NodeJ == 242 ) || ( NodeJ == 146 && NodeI == 242 ) )
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

            // NodePairRecord* nodePairRec = (NodePairRecord *) & mCurrentBuffer[ mCurrentBufferIndex ];

            AssignedWeightPtr->mNodePairRecord.mNodePair.mNodeIdA = NodeI;
            AssignedWeightPtr->mNodePairRecord.mNodePair.mNodeIdB = NodeJ;
            AssignedWeightPtr->mNodePairRecord.mWeight = NodeIJWeight;            

#ifndef PK_PHASE5_SKIP_OPT_SORT
            AssignedWeightPtr->mNodePairRecord.mOptCount = IntersectionListCount;
#endif
            mCurrentBufferIndex += sizeof( AssignedWeightLink );

            // AssignedWeightPtr->mAssignedWeightPtr = nodePairRec;
            AssignedWeightPtr->mNextLink          = NULL;
            // AssignedWeightPtr->mPrevLink          = NULL;
            
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
              // AssignedWeightPtr->mPrevLink = mFullSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast;
              
              mFullSkinNodeAssignmentTable[ AssignedNode ].mAssignedWeightsLast = AssignedWeightPtr; 
              }
            
            mNodePairRecordsCount ++;
            }
          }
        }
      }

    BegLogLine( 1 )
      << "Finished the FullSkinNodesAssignemtTableSetup"
      << EndLogLine;

#if 1
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
      BegLogLine( 1 )
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
    // }
    
    free( A2ASendCount );
    free( A2ASendOffset );
    free( IntersectionList );

    // free( A2ARecvCount );
    // free( A2ARecvOffset );

    // free( RecvFragmentBuffer );

//     free( NodesOnSkinI );
//     free( NodesOnSkinJ );

    BegLogLine( 1 )
      << "Before the barrier"
      << EndLogLine;

    Platform::Control::Barrier();

    BegLogLine( 1 )
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
    BegLogLine( 1 )
      << "aNewNode: " << aNewNode
      << " already exists in sparse skin list. " 
      << " aSparseSkinCount: " << aSparseSkinCount
      << EndLogLine;
    
    // Don't need to add duplicates
    return;
    // PLATFORM_ABORT( "ERROR: About to add a duplicate entry." );
    }

//   BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
//     << "Adding " << aNewNode << " to skin for: " << aSparseSkinNodeId 
//     << EndLogLine;    

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
    BegLogLine( 1 )
      << aListName << "[ " << i << " ]: "
      << aList[ i ]
      << EndLogLine;
    }
  }

NodeId* mMatchesForSparseListA;
int     mMatchesForSparseListACount;

NodeId* mMatchesForSparseListB;
int     mMatchesForSparseListBCount;

double*    mNodesScoreList;
int*       mNodeIndexWithSameScoreList;
int        mNodeIndexWithSameScoreListCount;

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
//       BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
//         << "MATCH found for { " << aNodeA << " " << aNodeB << " } " << aOptNodesCount
//         << EndLogLine;

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
        // break;
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
//     int RandomNodeIndex = (int) floor( aOptNodesCount * (rand()/(RAND_MAX + 1.0)) );
//     assert( RandomNodeIndex >= 0 && RandomNodeIndex < aOptNodesCount );

    int BestIndex = PickBestNodeForAdditionToSparseSkin( aOptNodes, aOptNodesCount );
    
    NodeId BestNode = aOptNodes[ BestIndex ];
    
    AddNodeInSortedOrder( BestNode, aNodeA, aSparseSkinA, aSparseSkinACount );
    AddNodeInSortedOrder( BestNode, aNodeB, aSparseSkinB, aSparseSkinBCount );

    // Add the weight to the score.
    // mNodesScoreList[ BestNode ] += aWeightAB;
    mNodesScoreList[ BestNode ] += 1.0;
    }

#if 1
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
    // mNodesScoreList[ BestNode ] += aWeightAB;
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
      // mNodesScoreList[ BestNode ] += aWeightAB;
      mNodesScoreList[ BestNode ] += 1.0;
      }
    else
      {
      int BestIndex = PickBestNodeForAdditionToSparseSkin( mMatchesForSparseListA, mMatchesForSparseListACount );
          
      NodeId BestNode = mMatchesForSparseListA[ BestIndex ];

      AddNodeInSortedOrder( BestNode, aNodeB, aSparseSkinB, aSparseSkinBCount );
      // Add the weight to the score.
      // mNodesScoreList[ BestNode ] += aWeightAB;
      mNodesScoreList[ BestNode ] += 1.0;
      }
    }
#endif
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

      // SparseSkinOptList[ SparseSkinOptListCount ].mOptCount = IntersectionListCount;
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
  
//   int* OptDistHistogram = (int *) malloc( sizeof(int) * MachineSize );
//   if( OptDistHistogram == NULL )
//     PLATFORM_ABORT( "OptDistHistogram == NULL" );

  for(int i = 0; i < MachineSize; i++ )
    {
    mNodesScoreList[ i ] = 0.0;
    mSparseSkinsCounts[ i ] = 0;
    // OptDistHistogram[ i ] = 0;

    int HalfTheNumberOfNodes = 0.6 * mNumberOfNodesOnFullSkin;
    mSparseSkins[ i ] = (NodeId *) malloc( sizeof( NodeId ) * HalfTheNumberOfNodes );
    if( mSparseSkins[ i ] == NULL )
      PLATFORM_ABORT( "mSparseSkins[ i ] == NULL" );
    } 

  double RadiusSquared = mRadiusOfSphere * mRadiusOfSphere;

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
    

      // LBWeightType WeightAB = NodePairRecPtr->mWeight;
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
        BegLogLine( 1 )
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
        BegLogLine( 1 )
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

#if 0
  for( int i = 0; i < MachineSize; i++ )
    {
    int Count = mSparseSkinsCounts[ i ];
    
    for( int j=0; j < Count; j++ )
      BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
        << "SPARSESKINS " 
        << i << " " 
        << Count << " " 
        << mSparseSkins[ i ][ j ] 
        << EndLogLine;
    }
#endif

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
      
//       BegLogLine( (Platform::Topology::GetAddressSpaceId() == 0 ) && ( NodeA == 348 && NodeB == 485 ))
//           << "NodeA: " << NodeA
//           << " NodeB: " << NodeB
//           << " IntersectionListCount: " << IntersectionListCount
//           << " AssignedNode: " << AssignedNode 
//           << EndLogLine;
      
      mSparseSkinNodeAssignmentTable[ AssignedNode ].mTotalWeight += PairWeight;
      
      if( mSparseSkinNodeAssignmentTable[ AssignedNode ].mTotalWeight < 0.0 )
        {
        BegLogLine( 1 )
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

#if 1
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
      BegLogLine( 1 )
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

//    BegLogLine(( Platform::Topology::GetAddressSpaceId() == 485 ) && ( mSparseSkinSendNodes[ i ] == 491 ) )
    BegLogLine( 0 )
      << "mSparseSkinSendNodes[ i ]= " << mSparseSkinSendNodes[ i ]
      << EndLogLine;
    }

  free( IntersectionList );
  for(int i = 0; i < MachineSize; i++ )
    {
    free( mFullSkinTable[ i ] );
    }

  free( mFullSkinTable );
#ifndef PK_PHASE5_SKIP_OPT_SORT
  free( SparseSkinOptList );
#endif
  }

void CreateLoadBalancedNodeAssignmentTable( )
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

//   if( Platform::Topology::GetAddressSpaceId() == 0 )
//     {
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

//    Platform::Algorithm::Qsort( WeightsToSort, MachineSize, sizeof( WeightSortHelper ), CompareWeightSortHelper );
//    Platform::Algorithm::RsortDescending( WeightsToSort, MachineSize, sizeof( WeightSortHelper ), 0, sizeof(LBWeightType) );
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

    // int TopHalfOfNodes = MachineSize / 2;
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

      if( TotalWeight <= 0.0 )
        PLATFORM_ABORT( "ERROR: TotalWeight <= 0.0" );

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


        // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
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

          // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
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

            //BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
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
            // Merit = (1.0 / NodePairRecPtr->mWeight) + NodeWeightDiff;
            
            Merit = (1.0 / NodePairRecPtr->mWeight) + NodeWeightDiff;
            // Merit = ( 1.0 / NodePairRecPtr->mWeight );            
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
        // break;
        PLATFORM_ABORT( "ERROR: BestPairLink == NULL" );
        }

      // Remove BestPairLink from the examining node's assigned weight list
      if( BestPairLink == NodeRecordPtr->mAssignedWeightsFirst )
        {
        // This is the first link
        NodeRecordPtr->mAssignedWeightsFirst = NodeRecordPtr->mAssignedWeightsFirst->mNextLink;
        //NodeRecordPtr->mAssignedWeightsFirst->mPrevLink = NULL;
        }
      else if( BestPairLink == NodeRecordPtr->mAssignedWeightsLast )
        {
        // Search to find the last to last link
        AssignedWeightLink* CurLink = NodeRecordPtr->mAssignedWeightsFirst;        
        while( CurLink->mNextLink !=  NodeRecordPtr->mAssignedWeightsLast )
            CurLink = CurLink->mNextLink;

        //NodeRecordPtr->mAssignedWeightsLast = NodeRecordPtr->mAssignedWeightsLast->mPrevLink;
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

        // if( BestPairLink->mPrevLink != NULL )        
        // BestPairLink->mPrevLink->mNextLink = BestPairLink->mNextLink;

//         if( BestPairLink->mNextLink != NULL )
//           BestPairLink->mNextLink->mPrevLink = BestPairLink->mPrevLink;
        }
    
      // Insert BestPairLink at the end the BestRank weight assigned list
      if( !( BestRank >= 0 && BestRank < MachineSize ) )
        PLATFORM_ABORT( "( BestRank >= 0 && BestRank < MachineSize )");

      // BestPairLink->mPrevLink = mNodeAssignmentTable[ BestRank ].mAssignedWeightsLast;
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
    

#if 0
   int tmpA[2];
   tmpA[ 0 ] = ExamRank;
   tmpA[ 1 ] = BestRank;

   for( int i=0; i < 2 ; i++ )
    {    
    AssignedWeightLink* AssignedWeightList = mSparseSkinNodeAssignmentTable[ tmpA[ i ] ].mAssignedWeightsFirst;
    float TotalWeight = 0.0;
    while( AssignedWeightList != NULL )
      {
      TotalWeight += AssignedWeightList->mNodePairRecord.mWeight;
      AssignedWeightList = AssignedWeightList->mNextLink;
      }
    
    if( fabs( TotalWeight - mSparseSkinNodeAssignmentTable[ tmpA [ i ] ].mTotalWeight )  > 10E-10 )
      {
      BegLogLine( 1 )
        << "i: " << i
        << "tmpA[i]: " << tmpA[i]
        << " TotalWeight: " << TotalWeight
        << " mSparseSkinNodeAssignmentTable[ i ].mTotalWeight: " 
        << mSparseSkinNodeAssignmentTable[ tmpA[i] ].mTotalWeight
        << EndLogLine;

      PLATFORM_ABORT( "ERROR: Inconsistent Weights" );
      }
    }
#endif
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

    BegLogLine( 1 )
      << "MyMinAssignment->mTotalWeight: " << MyMinAssignment->mTotalWeight
      << " MyMinAssignment->mAssignedPairsCount: " << MyMinAssignment->mAssignedPairsCount
      << EndLogLine;    
    
    BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      << "Final WeightDiff: " << MinWeightDiff
      << EndLogLine;

    Platform::Control::Barrier();

#if 1
    double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
    double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
    
    double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;
    
    double InRangeCutoff = SwitchUpperCutoff  ;
    
    double GuardZone = GetCurrentVerletGuardZone();
    
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

  double PossibleRange        = 2 * mRadiusOfSphere; ///SwitchLowerCutoff + SwitchDelta + mCurrentAssignmentGuardZone;
  double PossibleRangeSquared = PossibleRange * PossibleRange;

  /*******************************************************
   * Build the voxel interaction table
   ******************************************************/
//   int  NodesOnSkinCountI = 0;
//   int* NodesOnSkinI  = (int *) malloc( sizeof( int ) * MachineSize );
//   if( NodesOnSkinI == NULL )
//     PLATFORM_ABORT( "NodesOnSkinI == NULL" );

//   mRecBisTree->RanksInNeighborhoodSkin( 0, 0, 0,
//                                         mRadiusOfSphere,
//                                         NodesOnSkinI,
//                                         NodesOnSkinCountI,
//                                         MachineSize );  

  ORBNode * leafJ = mRecBisTree->NodeFromProcCoord( 0, 0, 0 );
  assert( leafJ );
  double LowJ[3];
  double HighJ[3];
  leafJ->Low( LowJ );
  leafJ->High( HighJ );
  leafJ->VoxelsInExternalBox( LowJ, HighJ, VoxelsJ, VoxelsCountJ, NumberOfVoxels );
  
  // (Number of nodes in skin) x ( Number of Voxels per node)
  mVoxelsInSkinHoodCount = mNumberOfNodesOnFullSkin * VoxelsCountJ; 
  mNumberOfVoxelsInRange = mVoxelsInSkinHoodCount;
      
  BegLogLine( PKFXLOG_SETUP )
    << " mNumberOfNodesOnFullSkin= " << mNumberOfNodesOnFullSkin
    << " VoxelsCountJ= " << VoxelsCountJ
    << "VoxelsInSkinHoodCount= " << mVoxelsInSkinHoodCount
    << EndLogLine;

  for( int j=0; j < NumberOfVoxels; j++)
    {
    mVoxelIndexToVoxelOrdinalMap[ j ] = -1;
    }

//   for( int i=0; i < VoxelsInSkinHoodCount; i++ )
//     {
//     int VoxelId = VoxelsInSkinHood[ i ];

//     VoxelIndex VoxelIndex1 = mRecBisTree->VoxelIdToVoxelIndex( VoxelId );

//     assert( VoxelIndex1 >= 0 && VoxelIndex1 < mMaxVoxelsInRange );

//     mVoxelIndexToVoxelOrdinalMap[ VoxelIndex1 ] = i;
//     }

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
         BegLogLine( 1 )
           << "NodeA: " << NodeA
           << " NodeB: " << NodeB
           << " NodeAOrdinal: " << NodeAOrdinal
           << " NodeBOrdinal: " << NodeBOrdinal
           << EndLogLine;            
     
    mAssignmentNodeTable[ NodeAOrdinal ][ NodeBOrdinal ] = 1;
    mAssignmentNodeTable[ NodeBOrdinal ][ NodeAOrdinal ] = 1;

#if 1
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
#endif
    }

/*********************************************************/

#if 0
  double Cutoff = SwitchLowerCutoff + SwitchDelta;
  double TotalWeight = 0.0;
  
  for( int i=0; i<RecvNodeCount; i++ )
    {
    int RankI = NodesToRecvFrom[ i ];

    int FragBuffOffsetI = A2ARecvOffset[ RankI ];
    int FragBuffCountI = A2ARecvCount[ RankI ];

    FragId* FragmentsOfRankI = &RecvFragmentBuffer[ FragBuffOffsetI ];

    for( int j=i; j<RecvNodeCount; j++ )
      {      
      if( ! mAssignmentNodeTable[ i ][ j ] )
        continue;
      else
        {        
        int RankJ = NodesToRecvFrom[ j ];
        
        int FragBuffOffsetJ = A2ARecvOffset[ RankJ ];
        int FragBuffCountJ = A2ARecvCount[ RankJ ];
        FragId* FragmentsOfRankJ = &RecvFragmentBuffer[ FragBuffOffsetJ ];

        double NodeIJWeight = 0.0;

        for( int fragOrdI=0; fragOrdI < FragBuffCountI; fragOrdI++ )
          {                    
          FragId fragIdI = FragmentsOfRankI[ fragOrdI ];
          
          if( fragIdI == (FragId) NON_EXISTANCE_VAL )
            continue;
            
          for( int fragOrdJ= ( RankI == RankJ ) ? fragOrdI+1: 0; 
               fragOrdJ < FragBuffCountJ; 
               fragOrdJ++ )
            {
            FragId fragIdJ = FragmentsOfRankJ[ fragOrdJ ];
            
            if( fragIdJ == (FragId) NON_EXISTANCE_VAL )
              continue;

            double ExtentA = Extents[ fragIdI ];
            double ExtentB = Extents[ fragIdJ ];
            XYZ & FragCenterA = FragCenters[ fragIdI ];
            XYZ & FragCenterB = FragCenters[ fragIdJ ];
                        
            XYZ FragCenterBImg;
            NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
            
            double thA= (GuardRange+ExtentA+ExtentB) ;
            double dsqThreshold = FragCenterA.DistanceSquared(FragCenterBImg) - (thA*thA) ;
            
            //BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
            BegLogLine( 0 )
              << "FragIdI: " << fragIdI
              << " FragIdJ: " << fragIdJ
              << " ExtentA: " << ExtentA
              << " ExtentB: " << ExtentB
              << " thA: " << thA
              << " dsqThreshold: " << dsqThreshold
              << EndLogLine;
            
//               BegLogLine( ( Platform::Topology::GetAddressSpaceId() == 1 ) &&
//                           ( fragIdI == 851 && fragIdJ == 1428 ) ||
//                           ( fragIdJ == 851 && fragIdI == 1428 ) )
//                   << "FragIdI: " << fragIdI
//                   << " FragIdJ: " << fragIdJ
//                   << " ExtentA: " << ExtentA
//                   << " ExtentB: " << ExtentB
//                   << " thA: " << thA
//                   << " dsqThreshold: " << dsqThreshold
//                   << EndLogLine;

            if( dsqThreshold < 0.0 )
              {
              // This pair counts              
//               double ModWeight = InteractionModel::GetWeight( fragIdI, fragIdJ, this );
//               TotalWeight += ModWeight;

//               BegLogLine( Platform::Topology::GetAddressSpaceId() == 1 )
//                 << "MODEL: " << fragIdI << " " << fragIdJ << " " << ModWeight
//                 << EndLogLine;

                // This pair counts
                double ModWeight = InteractionModel::GetWeight( fragIdI, fragIdJ, this );

#if 0
                int at1 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragIdI );
                int at2 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragIdJ );
                int FirstSiteId1   = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragIdI );
                int FirstSiteId2   = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragIdJ );

                int OutsideRangeCount = 0;
                for( int ii=FirstSiteId1; ii < (FirstSiteId1+at1); ii++ )
                  {
                  for( int jj=FirstSiteId2; jj < (FirstSiteId2+at2); jj++ )
                    {
                    XYZ SiteA = mPosit[ ii ];
                    XYZ SiteB = mPosit[ jj ];

                    double DistABSqr = NearestSquareDistanceInPeriodicVolume( SiteA, SiteB ) ;

//                     if( ( ii == 2093 && jj == 3503 ) || ( jj == 2093 && ii == 3503 ) )
//                       {
//                       BegLogLine( Platform::Topology::GetAddressSpaceId() == 1 )
//                         << "DistABSqr: " << DistABSqr
//                         << " GuardRangeSqr: " << GuardRangeSqr
//                         << " " << ii << " " << jj << " found here"
//                         << EndLogLine;
//                       }
                    
                    if( DistABSqr >= GuardRangeSqr )
                      {
                      OutsideRangeCount++;
                      }
                    else
                      {
//                       if( ! MSD_IF::AreSitesInExcIncList( ii, jj ) )
//                         {
//                         int siteId1 = ii;
//                         int siteId2 = jj;
                        
//                         if( ii > jj )
//                           {
//                           siteId1 = jj;
//                           siteId2 = ii;
//                           }
                                                
//                         BegLogLine( Platform::Topology::GetAddressSpaceId() == 1 )
//                           << "MODELSITE: " << siteId1 << " " << siteId2 
//                           << EndLogLine;
//                        }
                      }
                    }
                  }
#endif                
                  {
                  // double FinWeight = (ModWeight-OutsideRangeCount);
                  double FinWeight = ModWeight;
                  
//                    int f1 = fragIdI;
//                    int f2 = fragIdJ;
                  
//                    if( f1 > f2 )
//                      {
//                      f2 = f1; 
//                      f1 = fragIdJ; 
//                      }
                   
//                   BegLogLine( Platform::Topology::GetAddressSpaceId() == 1 )
//                     << "MODEL: " << f1 << " " << f2 << " weight=" << FinWeight
//                     << EndLogLine;

                  TotalWeight += FinWeight;
                  }
              }
            }
          }
        }
      }
    }
  
  BegLogLine( 1 )
    << " " << Platform::Topology::GetAddressSpaceId()
    << " TotalWeight= " << TotalWeight << " "
    << EndLogLine;
#endif
  
#endif

  free( mSparseSkinNodeAssignmentTable );
  mSparseSkinNodeAssignmentTable = NULL;

  //free( mLoadBalanceHeap );
  //mLoadBalanceHeap = NULL;
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
#endif

struct NodeRep
  {
  unsigned short mNodeId;
  unsigned short mX;
  unsigned short mY;
  unsigned short mZ;
  };

#if 0
void GetSparseSkinInNeighborhood( int  aRank, 
                                  int* aNodesOnSkin, 
                                  int  aNodesOnSkinCount, 
                                  int* aNodesOnSparseSkin, 
                                  int& aNodesOnSparseSkinCount )
  {
  int minX, minY, minZ =  999999;
  int maxX, maxY, maxZ = -999999;
  
  for( int i = 0; i < aNodesOnSkinCount; i++ )
    {
    int Rank = aNodesOnSkin[ i ];
    
    int xMy, yMy, zMy;
    Platform::Topology::GetCoordsFromRank( Rank, &xMy, &yMy, &zMy );

    minX = min( minX, xMy );
    minY = min( minY, yMy );
    minZ = min( minZ, zMy );
    maxX = max( maxX, xMy );
    maxY = max( maxY, yMy );
    maxZ = max( maxZ, zMy );
    }

  int dimX = maxX - minX;
  int dimY = maxY - minY;
  int dimZ = maxZ - minZ;

  t3D<unsigned char> ProcMesh;
  ProcMesh.Init( mProc_x, mProc_y, mProc_z );
  ProcMesh.Zero();

  NodeRep** PlaneList =  (NodeRep **) malloc( sizeof(NodeRep *) * dimX );
  if( PlaneList == NULL )
    PLATFORM_ABORT( "PlaneList == NULL" );
    
  NodeRep* NodesOnSkin = (NodeRep *) malloc( aNodesOnSkinCount * sizeof( NodeRep ));
  if( NodesOnSkin == NULL )
    PLATFORM_ABORT( "NodesOnSkin == NULL" );  

  // Put the nodes on a mesh
  for( int i = 0; i < aNodesOnSkinCount; i++ )
    {
    int Rank = aNodesOnSkin[ i ];
    
    int xMy, yMy, zMy;
    Platform::Topology::GetCoordsFromRank( Rank, &xMy, &yMy, &zMy );
    
    ProcMesh( xMy, yMy, zMy ) = 1;
    NodesOnSkin[ i ].mNodeId = Rank;
    NodesOnSkin[ i ].mX = xMy;
    NodesOnSkin[ i ].mY = yMy;
    NodesOnSkin[ i ].mZ = zMy;
    }

  for( int i = 0; i < aNodesOnSkinCount; i++ )
    {
    int NodeX = NodesOnSkin[ i ].mX;
    int NodeY = NodesOnSkin[ i ].mY;
    int NodeZ = NodesOnSkin[ i ].mZ;       
    }
  }
#endif

void GenerateOnBoundaryAndMidpointTableForVoxelsVersion2Phase5( )
  {
  int MachineSize = Platform::Topology::GetAddressSpaceCount();
  int xMy, yMy, zMy;
  Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
  ORBNode* MyNode = mRecBisTree->NodeFromProcCoord( xMy, yMy, zMy );
  assert( MyNode );

  // Get the skin of nodes in sphere.
  int  NodesInHoodCount  = 0;
  int* NodesInHood      = (int *) malloc( sizeof( int ) * MachineSize );
  if( NodesInHood == NULL )
    PLATFORM_ABORT( "NodesInHood == NULL" );

  mRecBisTree->RanksInNeighborhood( xMy, yMy, zMy,
                                    mRadiusOfSphere,
                                    NodesInHood,
                                    NodesInHoodCount,
                                    MachineSize );

  int  RecvFromNodesCount  = 0;
  int* RecvFromNodes       = (int *) malloc( sizeof( int ) * MachineSize );
  if( RecvFromNodes == NULL )
    PLATFORM_ABORT( "RecvFromNodes == NULL" );

  int** SkinsOfRecvFromNodes       = (int **) malloc( sizeof( int* ) * NodesInHoodCount );
  if( SkinsOfRecvFromNodes == NULL )
    PLATFORM_ABORT( "SkinsOfRecvFromNodes == NULL" );

  for( int i=0; i<NodesInHoodCount; i++ )
    {
    SkinsOfRecvFromNodes[ i ] = (int *) malloc( sizeof( int ) * MachineSize );

    if( SkinsOfRecvFromNodes[ i ] == NULL )
      PLATFORM_ABORT( "SkinsOfRecvFromNodes[ i ] == NULL" );
    }

  int* SkinsOfRecvFromNodesCounts   = (int *) malloc( sizeof( int ) * NodesInHoodCount );
  if( SkinsOfRecvFromNodesCounts == NULL )
    PLATFORM_ABORT( "SkinsOfRecvFromNodesCounts == NULL" );

  int* NodeIdToRecvNodeOrdinalMap  = (int *) malloc( sizeof( int ) * MachineSize );
  if( NodeIdToRecvNodeOrdinalMap == NULL )
    PLATFORM_ABORT( "NodeIdToRecvNodeOrdinalMap == NULL" );

  int  NodesOnRecvFromNodeSkinCount = 0;
  int* NodesOnRecvFromNodeSkin  = (int *) malloc( sizeof( int ) * MachineSize );
  if( NodesOnRecvFromNodeSkin == NULL )
    PLATFORM_ABORT( "NodesOnRecvFromNodeSkin == NULL" );

  for( int i=0; i<MachineSize; i++ )
    {
    NodeIdToRecvNodeOrdinalMap[ i ] = -1;
    }

  for( int i=0; i<NodesInHoodCount; i++ )
    {
    int Rank = NodesInHood[ i ];
    assert( Rank >= 0 && Rank < MachineSize );

    int xMy1, yMy1, zMy1;
    Platform::Topology::GetCoordsFromRank( Rank, &xMy1, &yMy1, &zMy1 );

    assert( RecvFromNodesCount >= 0 && RecvFromNodesCount < NodesInHoodCount );

    mRecBisTree->RanksInNeighborhoodSkin( xMy1, yMy1, zMy1,
                                          mRadiusOfSphere,
                                          NodesOnRecvFromNodeSkin,
                                          NodesOnRecvFromNodeSkinCount,
                                          MachineSize );

    for( int j=0; j < NodesOnRecvFromNodeSkinCount; j++ )
      {
      if( NodesOnRecvFromNodeSkin[ j ] == Platform::Topology::GetAddressSpaceId() )
        {
        assert( NodesOnRecvFromNodeSkinCount >= 0 && NodesOnRecvFromNodeSkinCount < MachineSize );

        memcpy( SkinsOfRecvFromNodes[ RecvFromNodesCount ], NodesOnRecvFromNodeSkin, NodesOnRecvFromNodeSkinCount*sizeof(int) );
        SkinsOfRecvFromNodesCounts[ RecvFromNodesCount ] = NodesOnRecvFromNodeSkinCount;

        NodeIdToRecvNodeOrdinalMap[ Rank ] = RecvFromNodesCount;

        BegLogLine( 0 )
          << "RecvFromNodes[ " << RecvFromNodesCount << " ]: " << Rank
          << EndLogLine;

        RecvFromNodes[ RecvFromNodesCount ++ ] = Rank;
        break;
        }
      }
    }

  // Here we have everyones skins lets intersect and come up with the assignment node
  // Create a 2D table of nodes where each entry is the assigned node

  int ** AssignmentNodeTable = (int **) malloc( sizeof(int *) * RecvFromNodesCount );
  if( AssignmentNodeTable == NULL )
    PLATFORM_ABORT( "AssignmentNodeTable == NULL" );

  for( int i=0; i<RecvFromNodesCount; i++ )
    {
    AssignmentNodeTable[ i ] = (int *) malloc( sizeof(int) * RecvFromNodesCount );

    if( AssignmentNodeTable[ i ] == NULL )
      PLATFORM_ABORT( "AssignmentNodeTable[i] == NULL" );

    for( int j=0; j<RecvFromNodesCount; j++ )
      AssignmentNodeTable[ i ][ j ] = -1;
    }

  // Belt and suspenders, RanksInNeighborhoodSkin might return a sorted list
  for( int i=0; i<RecvFromNodesCount; i++ )
    {
    int RankI = RecvFromNodes[ i ];

    int* SkinOfRecvFromNodeI = SkinsOfRecvFromNodes[ i ];
    int  CountI              = SkinsOfRecvFromNodesCounts[ i ];

    Platform::Algorithm::Qsort( SkinOfRecvFromNodeI, CountI, sizeof( int ), CompareInt1 );

    for( int j=0; j < CountI; j++ )
      {
      // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      BegLogLine( 0 )
        << "SkinOfRecvFromNodeI[ " << RankI << " ][ " << j << " ]= " << SkinOfRecvFromNodeI[ j ]
        << " CountI=" << CountI
        << EndLogLine;
      }
    }

  int IntersectionListCount = 0;

  int* IntersectionList = (int *) malloc( sizeof( int ) * MachineSize );
  if( IntersectionList == NULL )
    PLATFORM_ABORT( "IntersectionList == NULL" );

  for( int i=0; i<RecvFromNodesCount; i++ )
    {
    int RankI = RecvFromNodes[ i ];

    int* SkinOfRecvFromNodeI = SkinsOfRecvFromNodes[ i ];
    int  CountI              = SkinsOfRecvFromNodesCounts[ i ];

    for( int j=i; j<RecvFromNodesCount; j++ )
      {
      int RankJ = RecvFromNodes[ j ];

      int* SkinOfRecvFromNodeJ = SkinsOfRecvFromNodes[ j ];
      int  CountJ              = SkinsOfRecvFromNodesCounts[ j ];

      // For a given pair of nodes { RankI, RankJ } figure out the intersection
      // of their skin nodes and randomly pick one of those nodes for assignment
      IntersectSortedList< int >( SkinOfRecvFromNodeI, CountI,
                                  SkinOfRecvFromNodeJ, CountJ,
                                  IntersectionList, IntersectionListCount );

      int Seed = 0;
      if( RankI < RankJ )
        Seed = RankI * MachineSize + RankJ;
      else
        Seed = RankJ * MachineSize + RankI;

      srand( Seed );
      int RandomNodeIndex = (int) floor( IntersectionListCount * (rand()/(RAND_MAX + 1.0)) );

      BegLogLine( 0 )
        << "SP IntersectionListCount= " << IntersectionListCount
        << EndLogLine;

      assert( RandomNodeIndex >= 0 && RandomNodeIndex < IntersectionListCount );
      int IsRandomNodeMine = ( IntersectionList[ RandomNodeIndex ] == Platform::Topology::GetAddressSpaceId() );

      assert( mAssignmentNodeTable[ i ][ j ] == -1 );
      assert( mAssignmentNodeTable[ j ][ i ] == -1 );

      AssignmentNodeTable[ i ][ j ] = IsRandomNodeMine;
      AssignmentNodeTable[ j ][ i ] = IsRandomNodeMine;
      }
    }
  
  // Now we can evaluate the load on this node using the current model
  
  // Figure out the fragments on each node of our Recv list.
  // Send this nodes local fragment list to all the nodes on this node's skin.
  // Expect to recv from the RecvList of nodes
  int* A2ASendCount = (int *) malloc( sizeof(int) * MachineSize );
  if( A2ASendCount == NULL )
    PLATFORM_ABORT( "A2ASendCount == NULL" );

  int* A2ASendOffset = (int *) malloc( sizeof(int) * MachineSize );
  if( A2ASendOffset == NULL )
    PLATFORM_ABORT( "A2ASendOffset == NULL" );

  int* A2ARecvCount = (int *) malloc( sizeof(int) * MachineSize );
  if( A2ARecvCount == NULL )
    PLATFORM_ABORT( "A2ARecvCount == NULL" );

  int* A2ARecvOffset = (int *) malloc( sizeof(int) * MachineSize );
  if( A2ARecvOffset == NULL )
    PLATFORM_ABORT( "A2ARecvOffset == NULL" );
  
  unsigned long LocalFragmentCountMax = 0;
  unsigned long LocalFragmentCount = mIntegratorStateManagerIF.GetNumberOfLocalFragments();
  Platform::Collective::GlobalMax( &LocalFragmentCount, 
                                   &LocalFragmentCountMax );

  FragId* RecvFragmentBuffer = (FragId *) malloc( sizeof( FragId ) * LocalFragmentCountMax * RecvFromNodesCount );
  if( RecvFragmentBuffer == NULL )
    PLATFORM_ABORT( "RecvFragmentBuffer == NULL" );
  
  for( int i=0; i<MachineSize; i++ )
    {
    A2ASendCount[ i ] = 0;
    A2ASendOffset[ i ] = 0;
    A2ARecvCount[ i ] = 0;
    A2ARecvOffset[ i ] = 0;
    }
  
  for( int i=0; i<RecvFromNodesCount; i++ )
    {
    int RankI = RecvFromNodes[ i ];
    A2ARecvCount[ RankI ] = 1;
    A2ARecvOffset[ RankI ] = i*LocalFragmentCountMax;
    }
  
  mRecBisTree->RanksInNeighborhoodSkin( xMy, yMy, zMy,
                                        mRadiusOfSphere,
                                        NodesOnRecvFromNodeSkin,
                                        NodesOnRecvFromNodeSkinCount,
                                        MachineSize );  

  for( int i=0; i<NodesOnRecvFromNodeSkinCount; i++ )
    {
    int Rank = NodesOnRecvFromNodeSkin[ i ];
    A2ASendCount[ Rank ] = LocalFragmentCount;
    }

  FragId* LocalFragments = mIntegratorStateManagerIF.GetLocalFragmentPtr();

  Platform::Collective::Alltoallv( LocalFragments,
                                   A2ASendCount,
                                   A2ASendOffset,
                                   sizeof( FragId ),
                                   RecvFragmentBuffer,
                                   A2ARecvCount,
                                   A2ARecvOffset,
                                   sizeof( FragId ), 0, 1 );  

  double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
  double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
  
  double Cutoff = SwitchLowerCutoff + SwitchDelta;
  
  double TotalWeight = 0.0;

  // Now that we have all the fragments, we are ready to evaluate the load on this node
  for( int i=0; i<RecvFromNodesCount; i++ )
    {
    int RankI = RecvFromNodes[ i ];

    int FragBuffOffsetI = A2ARecvOffset[ RankI ];
    int FragBuffCountI = A2ARecvCount[ RankI ];

    FragId* FragmentsOfRankI = &RecvFragmentBuffer[ FragBuffOffsetI ];
    
    for( int j=i; j<RecvFromNodesCount; j++ )
      {
      int RankJ = RecvFromNodes[ j ];
      
      // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
      BegLogLine( 0 )
        << "{ RankI, RankJ }: { " << RankI << " , " << RankJ << " }"
        << " mAssignmentNodeTable[ " << i << " ][ " << j << " ]: " << AssignmentNodeTable[ i ][ j ]
        << " RecvFromNodesCount: " << RecvFromNodesCount
        << EndLogLine;

      if( !AssignmentNodeTable[ i ][ j ] )
        {
        // If the pair RankI, RankJ is not assigned here
        // no need to account for it's weight
        continue;
        }
      else
        {
        int FragBuffOffsetJ = A2ARecvOffset[ RankJ ];
        int FragBuffCountJ = A2ARecvCount[ RankJ ];
        
        FragId* FragmentsOfRankJ = &RecvFragmentBuffer[ FragBuffOffsetJ ];
        
        // Figure out the weight from evaluating fragment pairs from RankI, RankJ
        //BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
        BegLogLine( 0 )
          << "{ RankI, RankJ }: { " << RankI << " , " << RankJ << " }"
          << " { FragBuffOffsetI, FragBuffOffsetI }: { " << FragBuffOffsetI << " , " << FragBuffOffsetJ << " }"
          << " { FragBuffCountI, FragBuffCountJ }: { " << FragBuffCountI << " , " << FragBuffCountJ << " }"
          << EndLogLine;
        
        for( int fragOrdI=0; fragOrdI < FragBuffCountI; fragOrdI++ )
          {                    
          FragId fragIdI = FragmentsOfRankI[ fragOrdI ];
          
          for( int fragOrdJ=0; fragOrdJ < FragBuffCountJ; fragOrdJ++ )
            {
            FragId fragIdJ = FragmentsOfRankJ[ fragOrdJ ];
            
            double ExtentA = Extents[ fragIdI ];
            double ExtentB = Extents[ fragIdJ ];
            XYZ & FragCenterA = FragCenters[ fragIdI ];
            XYZ & FragCenterB = FragCenters[ fragIdJ ];
                        
            XYZ FragCenterBImg;
            NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
            
            double thA= ( Cutoff + ExtentA + ExtentB ) ;
            double dsqThreshold = FragCenterA.DistanceSquared( FragCenterBImg ) - ( thA * thA ) ;
            
            //BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
            BegLogLine( 0 )
              << "FragIdI: " << fragIdI
              << " FragIdJ: " << fragIdJ
              << " ExtentA: " << ExtentA
              << " ExtentB: " << ExtentB
              << " thA: " << thA
              << " dsqThreshold: " << dsqThreshold
              << EndLogLine;
            
            if( dsqThreshold < 0.0 )
              {
              // This pair counts
              double ModWeight = InteractionModel::GetWeight( fragIdI, fragIdJ, this );
              TotalWeight += ModWeight;
              }
            }
          }
        }
      }
    }

  BegLogLine( 1 )
    << " " << Platform::Topology::GetAddressSpaceId()
    << " TotalWeight= " << TotalWeight << " "
    << EndLogLine;

  free( NodesInHood );
  free( RecvFromNodes );

  for( int i=0; i<NodesInHoodCount; i++ )
    {
    free( SkinsOfRecvFromNodes[ i ] );
    }

  free( SkinsOfRecvFromNodes );
  free( IntersectionList );

  free( A2ASendCount );
  free( A2ASendOffset );
  free( A2ARecvCount );
  free( A2ARecvOffset );
  free( RecvFragmentBuffer );
  }
#else

void GenerateOnBoundaryAndMidpointTableForVoxelsVersion2()
  {
  BegLogLine( PKFXLOG_SETUP )
    << "Entering GenerateOnBoundaryAndMidpointTableForVoxelsVersion2()"
    << EndLogLine;

  #ifdef VOXEL_INTERACTION_TABLE_ENTRY_AS_CHAR
    // Interate over the voxels in range and create a voxel table
    // This table is indexed by VoxelOrdinal
    if( mVoxelInteractionTypeTable != NULL )
      free( mVoxelInteractionTypeTable );
  #endif

  int xMy, yMy, zMy;
  Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
  ORBNode* MyNode = mRecBisTree->NodeFromProcCoord( xMy, yMy, zMy );

  MyNode->VoxelsInNeighborhood( mRadiusOfSphere, mVoxelsInRange, mNumberOfVoxelsInRange,  mMaxVoxelsInRange );

  BegLogLine( PKFXLOG_SETUP )
    << "mNumberOfVoxelsInRange= " << mNumberOfVoxelsInRange
    << EndLogLine;

  for( int j=0; j < mMaxVoxelsInRange; j++)
    {
    mVoxelIndexToVoxelOrdinalMap[ j ] = -1;
    }

  for( int i=0; i < mNumberOfVoxelsInRange; i++ )
    {
    int VoxelId = mVoxelsInRange[ i ];

    VoxelIndex VoxelIndex1 = mRecBisTree->VoxelIdToVoxelIndex( VoxelId );

    assert( VoxelIndex1 >= 0 && VoxelIndex1 < mMaxVoxelsInRange );

    mVoxelIndexToVoxelOrdinalMap[ VoxelIndex1 ] = i;
    }

  mNeighborhoodCommDriver.SetVoxelIndexToVoxelOrdinalMap( mVoxelIndexToVoxelOrdinalMap, 
                                                          mNumberOfVoxelsInRange, 
                                                          mMaxVoxelsInRange );

  BegLogLine( PKFXLOG_SETUP )
    << "GenVoxTable(): About to allocate the n^2 voxel table"
    << EndLogLine;

  mVoxelInteractionTypeTableSize = mNumberOfVoxelsInRange * mNumberOfVoxelsInRange;

  #ifdef VOXEL_INTERACTION_TABLE_ENTRY_AS_CHAR
    mVoxelInteractionTypeTable = (char *) malloc( mVoxelInteractionTypeTableSize * sizeof(char));
    if( mVoxelInteractionTypeTable == NULL )
      PLATFORM_ABORT("mVoxelInteractionTypeTable == NULL" );
  #else
    mVoxelInteractionTypeTable.SetCount( mVoxelInteractionTypeTableSize );
  #endif

  //double SureRange = 2 * mRadiusOfSphere;
  //double SureRangeSquared = SureRange * SureRange;

  double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
  double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );

  double SureRange        = SwitchLowerCutoff ;
  double SureRangeSquared = SureRange * SureRange;

  // This is the range particles might diffuse into ranged during the life of the communication list
  double PossibleRange        = 2 * mRadiusOfSphere; ///SwitchLowerCutoff + SwitchDelta + mCurrentAssignmentGuardZone;
  double PossibleRangeSquared = PossibleRange * PossibleRange  ;

  BegLogLine( PKFXLOG_SETUP )
    << "About to gen VoxelInteractionTypeTable "
    << "LowerCutorr "
    << MSD_IF::GetSwitchLowerCutoffForShell( 0 )
    << " RadiusOfSphere "
    << mRadiusOfSphere
    << " SwitchLowerCutoff "
    << SwitchLowerCutoff
    << " SwitchDelta "
    << SwitchDelta
    << " PossibleRange "
    << PossibleRange
    << " SureRange "
    << SureRange
    << EndLogLine;

  assert( SureRange <=  2 * mRadiusOfSphere );

#if 0
  int NodeOfInterest = 0x0;
  int aa,bb,cc;
  Platform::Topology::GetCoordsFromRank( NodeOfInterest, &aa, &bb, &cc );

  ORBNode* NodeThis = mRecBisTree->NodeFromProcCoord( aa, bb, cc );

  BegLogLine( mAddressSpaceId == NodeOfInterest )
  // BegLogLine( 0 )
    << "FM:: node " << NodeOfInterest << " bounding box: "
    << " " << aa << " " << bb << " " << cc
    <<" { " << NodeThis->low[ 0 ] << " , "
    << NodeThis->low[ 1 ] << " , "
    << NodeThis->low[ 2 ] << " } "
    <<" { " << NodeThis->high[ 0 ] << " , "
    << NodeThis->high[ 1 ] << " , "
    << NodeThis->high[ 2 ] << " } "
    << EndLogLine;

  NodeOfInterest = 0x41;
  Platform::Topology::GetCoordsFromRank( NodeOfInterest, &aa, &bb, &cc );

  NodeThis = mRecBisTree->NodeFromProcCoord( aa, bb, cc );

  BegLogLine( mAddressSpaceId == NodeOfInterest )
  // BegLogLine( 0 )
    << "FM:: node " << NodeOfInterest << " bounding box: "
    << " " << aa << " " << bb << " " << cc
    <<" { " << NodeThis->low[ 0 ] << " , "
    << NodeThis->low[ 1 ] << " , "
    << NodeThis->low[ 2 ] << " } "
    <<" { " << NodeThis->high[ 0 ] << " , "
    << NodeThis->high[ 1 ] << " , "
    << NodeThis->high[ 2 ] << " } "
    << EndLogLine;
#endif

  BegLogLine( PKFXLOG_SETUP )
    << "GenVoxTable(): About to fill the the n^2 voxel table, mVoxelsInRange=" << mVoxelsInRange
    << EndLogLine;

  int VoxelIdTemp1 = mRecBisTree->VoxelIndexToVoxelId( 200654 );
  int VoxelIdTemp2 = mRecBisTree->VoxelIndexToVoxelId( 188817 );

  for( int i=0; i < mNumberOfVoxelsInRange; i++)
    {
    int VoxelIdA = mVoxelsInRange[ i ];
    ////int VoxelIndexA = mRecBisTree->VoxelIdToVoxelIndex( VoxelIdA );

#if 1
    char IsMidpointMineVector[mNumberOfVoxelsInRange] ;
    MyNode->IsMidpointOfVoxelPointsMineVector(IsMidpointMineVector,VoxelIdA,mVoxelsInRange+i,mNumberOfVoxelsInRange-i) ;
#endif
    
    for( int j=i; j < mNumberOfVoxelsInRange; j++)
      {
      int VoxelIdB = mVoxelsInRange[ j ];
#if 0     	


//       #ifdef P4_LOAD_BALANCING_NEW_STUNT
      unsigned IsPointMine = MyNode->IsMidpointOfVoxelPointsMine( VoxelIdA, VoxelIdB );
//       unsigned IsPointMine = MyNode->IsPointInRandomSphereMine( VoxelIdA,
//                                                                 VoxelIdB,
//                                                                 CutoffGuardZone );
//       #else
//         unsigned IsPointMine = MyNode->IsVoxelPointOfVoxelPointsMidpointMine( VoxelIdA, VoxelIdB );
//    
#else
    int IsPointMine=IsMidpointMineVector[j-i] ; 
#endif

      unsigned char VoxelInteractionType = 0;



      // NOTES on the range bit. Range bit means different things based on whether interaction bit is set or not.
      // If the interaction bit is set, then Range bit tells us if the interaction is a sure thing  or not.
      // If the interaction bit is not set, then Range bit tells us how long we preserve a pair in the neighborhood DB.
      // NOTE: one could control ranging for pair preservation differently from sure thing interactions.
      
      if( ((VoxelIdTemp1 == VoxelIdA) && (VoxelIdTemp2 == VoxelIdB) )
          || ((VoxelIdTemp2 == VoxelIdA) && (VoxelIdTemp1 == VoxelIdB) ) )
        {
        BegLogLine( 1 )
          << "VoxelIdA: " << VoxelIdA
          << " VoxelIdB: " << VoxelIdB
          << " IsPointMine: " << IsPointMine          
          << EndLogLine;
        }

      if( IsPointMine )
        {
        double MaxVoxelDistanceSqr = MyNode->MaxVoxelSqrDistance( VoxelIdA, VoxelIdB );

        if( MaxVoxelDistanceSqr < PossibleRangeSquared )
          {
          VoxelInteractionType |= 0x01 ; // Turning this bit on means the pair could be in range and needs to be checked

          if( ((VoxelIdTemp1 == VoxelIdA) && (VoxelIdTemp2 == VoxelIdB) )
              || ((VoxelIdTemp2 == VoxelIdA) && (VoxelIdTemp1 == VoxelIdB) ) )
            {
            BegLogLine( 1 )
              << "VoxelIdA: " << VoxelIdA
              << " VoxelIdB: " << VoxelIdB
              << " IsPointMine: " << IsPointMine
              << " MaxVoxelDistanceSqr: " << MaxVoxelDistanceSqr
              << " PossibleRangeSquared: " << PossibleRangeSquared
              << " VoxelInteractionType: " << VoxelInteractionType
              << EndLogLine;
            }
          }

        if( MaxVoxelDistanceSqr < SureRangeSquared )
          {
          VoxelInteractionType |= 0x02 ; // Turning the bit on here means distnace check isn't required
          }
        }


      // Here, we should ask how far away
      if( 1 /*** Actually, want to check how far away midpoint is from this load zone ***/ )
        {
//TURNING THIS BIT ON CAUSES DB TO BE POPULATED - CURRENTLY SLOWS RECOOK ON HAIRPIN
// NEED FUNCTION TO TELL WHEN MIDPOINT IS NEAR TO BEING MINE
//////      VoxelInteractionType |= 0x02 ;   // Turning this bit on here means that the DB entry will be preserved
        }

//       if( i==181 && j==205 && ( mAddressSpaceId == NodeOfInterest ))
//         {
//         BegLogLine( 0 )
//           << "Looking at (i,j) VoxelIdA: " << VoxelIdA
//           << " VoxelIdB: " << VoxelIdB
//           << " i: " << i
//           << " j: " << j
//           << " VoxelInteractionType: " << VoxelInteractionType
//           << EndLogLine;
//         }

//       if(( VoxelIdA==5246982 && VoxelIdB==6292486 ) ||
//          ( VoxelIdB==5246982 && VoxelIdA==6292486 ) )
//         {
//         BegLogLine( 0 )
//           << "VoxelIdA: " << VoxelIdA
//           << " VoxelIdB: " << VoxelIdB
//           << " i: " << i
//           << " j: " << j
//           << " VoxelInteractionType: " << VoxelInteractionType
//           << EndLogLine;

//         mRecBisTree->ReportVoxel( VoxelIdA );
//         mRecBisTree->ReportVoxel( VoxelIdB );
//         }

//       if( mAddressSpaceId == 27 )
//         {
//         BegLogLine( 1 )
//           << "i: " << i
//           << " j: " << j
//           << " VoxelIdA: " << VoxelIdA
//           << " VoxelIdB: " << VoxelIdB
//           << " VoxelInteractionType: " << VoxelInteractionType
//           << EndLogLine;

//         }

      int VoxelInteractionTypeTableIndexIJ = i * mNumberOfVoxelsInRange + j;
      int VoxelInteractionTypeTableIndexJI = j * mNumberOfVoxelsInRange + i;

      if ( VoxelInteractionType != 0 )
      {
      //////assert( VoxelInteractionTypeTableIndex < mVoxelInteractionTypeTableSize );
      #ifdef VOXEL_INTERACTION_TABLE_ENTRY_AS_CHAR
        mVoxelInteractionTypeTable[ VoxelInteractionTypeTableIndexIJ ] = VoxelInteractionType;
        mVoxelInteractionTypeTable[ VoxelInteractionTypeTableIndexJI ] = VoxelInteractionType;
      #else
        mVoxelInteractionTypeTable.SetElement( VoxelInteractionTypeTableIndexIJ, VoxelInteractionType );
        mVoxelInteractionTypeTable.SetElement( VoxelInteractionTypeTableIndexJI, VoxelInteractionType );
      #endif
      }
      }
    }

  BegLogLine( mAddressSpaceId == 0 )
    << "Leaving GenerateOnBoundaryAndMidpointTableForVoxelsVersion2()"
    << EndLogLine;
  }
#endif

#endif

  XYZ& GetFirstPositionInFragment( int aFragId )
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

#ifdef PK_PHASE5_SPLIT_COMMS
    mBondedInteractionStateManagerIF.SetOrb( aORB );
#endif

    }

void
Init()
  {
  BegLogLine( PKFXLOG_SETUP )
    << "Entering DynVarMgr.Init()"
    << EndLogLine;

  CheckeredTemplate[0][0][0] = 1;
  CheckeredTemplate[0][0][1] = 1;
  CheckeredTemplate[0][1][0] = 1;
  CheckeredTemplate[0][1][1] = 1;
  CheckeredTemplate[1][0][0] = 1;
  CheckeredTemplate[1][0][1] = 1;
  CheckeredTemplate[1][1][0] = 1;
  CheckeredTemplate[1][1][1] = 1;

  ZeroPotentialEnergy();

  mFragmentsInVoxelsInP3MEBox = (FragId *) malloc( sizeof( FragId ) * FRAGMENT_COUNT );
  if( mFragmentsInVoxelsInP3MEBox == NULL )
    PLATFORM_ABORT("mFragmentsInVoxelsInP3MEBox == NULL");

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
  mVoxelsInBox = NULL;
  mRanksInBox = NULL;
  mVoxelsInBoxTable = NULL;

  #if MSDDEF_DoPressureControl
    mCurrentVerletGuardZone = 0.0;
  #else
    mCurrentVerletGuardZone = 1.0;
  #endif

    //mCurrentAssignmentGuardZone = 2.0;
    //mCurrentAssignmentGuardZone = 1.5;
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

//   int SiteIdA = 254;
//   int SiteIdB = 672;

//   BegLogLine( mAddressSpaceId == 0 )
//     << "SiteIdA: " << SiteIdA
//     << " SiteIdB: " << SiteIdB
//     << " FragIdA: " << mSiteIdToFragmentRepMap[ SiteIdA ].mFragId
//     << " FragIdB: " << mSiteIdToFragmentRepMap[ SiteIdB ].mFragId
//     << EndLogLine;

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
#if 1
    iXYZ VoxDims;
    VoxDims.mX = mVoxDimX;
    VoxDims.mY = mVoxDimY;
    VoxDims.mZ = mVoxDimZ;
    VoxDims.ReOrderAbs( RTG.mBoxIndexOrder );
    mVoxDimX = VoxDims.mX;
    mVoxDimY = VoxDims.mY;
    mVoxDimZ = VoxDims.mZ;
#endif
////////////#if defined( PK_BGL )
////////////  int factor = 4;
////////////#else
////////////  int factor = 1;
////////////#endif
////////////
#ifdef P4_LOAD_BALANCING_NEW_STUNT
// int factor = 2;
// mVoxDimX = factor * 8;
// mVoxDimY = factor * 12;
// mVoxDimZ = factor * 16;

//   mVoxDimX = 64;
//   mVoxDimY = 64;
//   mVoxDimZ = 64;

//  mVoxDimX = 16;
//  mVoxDimY = 16;
//  mVoxDimZ = 16;
#endif

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
#ifdef PK_PHASE5_SPLIT_COMMS
    mBondedInteractionStateManagerIF.Init( );
#endif


  mMaxVoxelsInRange = mVoxDimX * mVoxDimY * mVoxDimZ;

  mVoxelsInRange = (int *) malloc( sizeof(int) * mMaxVoxelsInRange );
  if( mVoxelsInRange == NULL )
    PLATFORM_ABORT( "mVoxelsInRange == NULL" );

  mVoxelIndexToVoxelOrdinalMap = (int *) malloc( sizeof(int) * mMaxVoxelsInRange );
  if( mVoxelIndexToVoxelOrdinalMap == NULL )
    PLATFORM_ABORT( "mVoxelIndexToVoxelOrdinalMap == NULL" );

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

  // mMaxExtent = maxExtent;
  mMaxExtent = 1.82;

  double VoxelSpace_x = mDynamicBoxDimensionVector.mX / mVoxDimX;
  double VoxelSpace_y = mDynamicBoxDimensionVector.mY / mVoxDimY;
  double VoxelSpace_z = mDynamicBoxDimensionVector.mZ / mVoxDimZ;

  double VoxelDiagonal = sqrt( VoxelSpace_x * VoxelSpace_x +
                               VoxelSpace_y * VoxelSpace_y +
                               VoxelSpace_z * VoxelSpace_z );

  //         double HalfVoxelDiagonal =  sqrt(  VoxelSpace_x * VoxelSpace_x +
  //                                            VoxelSpace_y * VoxelSpace_y +
  //                                            VoxelSpace_z * VoxelSpace_z );

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

  // mRadiusOfSphere = (( SwitchLowerCutoff + SwitchDelta + mCurrentVerletGuardZone ) / 2.0 ) + MaxOfMaxExtentAndHalfVoxelDiagonal; // + MaxExtent;
  // mRadiusOfSphere = (( SwitchLowerCutoff + SwitchDelta + mCurrentGuardZone ) / 2.0 ) + mMaxExtent;

  // mRadiusOfSphere = 8.0;

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): mRadiusOfSphere " << mRadiusOfSphere
    << EndLogLine;

  #ifdef VOXEL_INTERACTION_TABLE_ENTRY_AS_CHAR
    mVoxelInteractionTypeTable = NULL;
  #endif

  mNearestNeighbors = NULL;
  mSendBufferSD = NULL;
  mRecvBufferSD = NULL;

#ifdef PK_MPI_ALL
  mAllRequestsSD = NULL;
  mAllStatusSD = NULL;
#endif

#ifdef PK_PARALLEL
  mSDSendCounts = NULL;
  mSDSendOffsets = NULL;
  mSDRecvCounts = NULL;
  mSDRecvOffsets = NULL;

  mSDSendCountsOffsets = NULL;
  mSDRecvCountsOffsets = NULL;
  mSDSendCountsCounts = NULL;
  mSDRecvCountsCounts = NULL;
#endif

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

  #ifdef PK_PHASE5_SPLIT_COMMS
  mNeighborhoodCommBondedDriver.Init( &mBondedInteractionStateManagerIF, &mIntegratorStateManagerIF,
                                      MAX_SITES_IN_FRAGMENT, VoxelMeshSize );
  #endif

  BegLogLine( PKFXLOG_SETUP )
    << "DynVarMgr.Init(): About to mIntegratorStateManagerIF.Init()"
    << EndLogLine;

  mIntegratorStateManagerIF.Init( mSiteIdToFragmentRepMap );

  BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
    << "SourceId: " << MSD_IF::GetSourceId()
    << " ProcMeshDim: { " << mProcs_x << " , " << mProcs_y << " , " << mProcs_z << " }"
#ifdef MSDDEF_P3ME
    << " FFTMeshDim: { " << FFT_PLAN::GLOBAL_SIZE_X << " , " << FFT_PLAN::GLOBAL_SIZE_Y << " , " << FFT_PLAN::GLOBAL_SIZE_Z << " }"
#endif
    << " VoxMeshDim: { " << mVoxDimX << " , " << mVoxDimY << " , " << mVoxDimZ << " }"
    << EndLogLine;


#ifdef PK_PHASE5_PROTOTYPE
  mNodeIdToRecvNodeOrdinalMap = NULL;
  mAssignmentNodeTable = NULL;
#endif

//   mRemovedFragsAllocated = 20 * ( FRAGMENT_COUNT / MachineSize );
//   mRemovedFrags = (int *) malloc( sizeof(int) * mRemovedFragsAllocated );
//   if( mRemovedFrags == NULL )
//     PLATFORM_ABORT( "mRemovedFrags == NULL" );

//   int SiteA = 11177;
//   int SiteB = 15316;

//   BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
//     << "sitepair: { " << SiteA << " , " << SiteB << " } "
//     << " fragpair: { " << GetFragIdForSiteId( SiteA ) << " , " << GetFragIdForSiteId( SiteB ) << " } "
//     << " offsets: { " << GetFragmentRepForSiteId( SiteA ).mOffset << " , " << GetFragmentRepForSiteId( SiteB ).mOffset << " } "
//     << EndLogLine;  

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
//      XYZ ImageVector ;
        assert ( index < 125 ) ;
        unsigned int ImageMap = mImageMap [ index ] ;
        unsigned int i = ImageMap >> 16 ;
        unsigned int j = ( ImageMap >> 8 ) & 0xff ;
        unsigned int k = ImageMap & 0xff ;
        aImageVector.mX = mDynamicBoxDimensionVector.mX * dk_ImageTable[i] ;
        aImageVector.mY = mDynamicBoxDimensionVector.mY * dk_ImageTable[j] ;
        aImageVector.mZ = mDynamicBoxDimensionVector.mZ * dk_ImageTable[k] ;
//      return ImageVector ;
  }


  void InitPostOrbState( ORBNode* aORB,
                         XYZ& aVmin,
                         XYZ& aVmax,
                         int* aRanksInP3MEMeshSpace,
                         int  aRanksInP3MEMeshSpaceSize )
    {
    BegLogLine( PKFXLOG_SETUP )
      << "Entering InitPostOrbState"
      << EndLogLine;

    mRecBisTree = aORB;
    mIntegratorStateManagerIF.SetORB( aORB );
    mInteractionStateManagerIF.SetOrb( aORB );
#ifdef PK_PHASE5_SPLIT_COMMS
    mBondedInteractionStateManagerIF.SetOrb( aORB );
#endif

    mPreNumberOfNearestNeighbors = mNumberOfNearestNeighbors;

    DynVarMgrIF.SpacialDecomposition();

    BegLogLine( PKFXLOG_SETUP )
      << "InitPostOrbState: Done with the SpacialDecomposition"
      << EndLogLine;

    // mIntegratorStateManagerIF.UpdateVoxelIndexesForFragment();

    GenRankForNeighborIndexMap();

    int VoxelSpace = mVoxDimX * mVoxDimY * mVoxDimZ;
    int HugeAllocationOfVoxels = VoxelSpace;


#ifdef MSDDEF_P3ME
    if( mRanksInBox != NULL )
      free ( mRanksInBox );

    mRanksInBox = (int *) malloc( sizeof(int) * HugeAllocationOfVoxels );
    if( mRanksInBox == NULL )
      PLATFORM_ABORT("mRanksInBox == NULL");

    if( mVoxelsInBox != NULL )
      free ( mVoxelsInBox );

    mVoxelsInBox = (int *) malloc( sizeof(int) * HugeAllocationOfVoxels );
    if( mVoxelsInBox == NULL )
      PLATFORM_ABORT("mVoxelsInBox == NULL");


    double FudgeFactor = mMaxExtent + mCurrentAssignmentGuardZone / 2.0;

    double vminTemp[3];
    vminTemp[ 0 ] = aVmin.mX - FudgeFactor;
    vminTemp[ 1 ] = aVmin.mY - FudgeFactor;
    vminTemp[ 2 ] = aVmin.mZ - FudgeFactor;

   double vmaxTemp[3];
    vmaxTemp[ 0 ] = aVmax.mX + FudgeFactor;
    vmaxTemp[ 1 ] = aVmax.mY + FudgeFactor;
    vmaxTemp[ 2 ] = aVmax.mZ + FudgeFactor;

    // BegLogLine( PKFXLOG_LOADBALANCE && (Platform::Topology::GetAddressSpaceId() == 0 ) )
    BegLogLine( 0 )
      << " vminTemp: { " << vminTemp[0] << " , " << vminTemp[1] << " , " << vminTemp[2] << " }"
      << " vmaxTemp: { " << vmaxTemp[0] << " , " << vmaxTemp[1] << " , " << vmaxTemp[2] << " }"
      << " aVmin: " << aVmin
      << " aVmax: " << aVmax
      << " FudgeFactor: " << FudgeFactor
      << EndLogLine;

    BegLogLine( PKFXLOG_SETUP )
      << "InitPostOrbState: About to call mRecBisTree->RanksInLoadBox"
      << EndLogLine;

    int xMy1, yMy1, zMy1;
    Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy1, &yMy1, &zMy1 );
    ORBNode *nd = mRecBisTree->NodeFromProcCoord( xMy1, yMy1, zMy1 );
    if( !nd->nLoadVox )
      PLATFORM_ABORT("!nd->nLoadVox");

    mRecBisTree->RanksInLoadBox( vminTemp, vmaxTemp,
                                 mVoxelsInBox, mVoxelsInBoxSize, HugeAllocationOfVoxels,
                                 mRanksInBox, mRanksInBoxSize, HugeAllocationOfVoxels );

    if( !nd->nLoadVox )
      PLATFORM_ABORT("!nd->nLoadVox");

    mVoxelsInBoxSize = 0;
    FudgeFactor = mCurrentVerletGuardZone / 2.0 + 4.0;

    vminTemp[ 0 ] = aVmin.mX - FudgeFactor;
    vminTemp[ 1 ] = aVmin.mY - FudgeFactor;
    vminTemp[ 2 ] = aVmin.mZ - FudgeFactor;


    vmaxTemp[ 0 ] = aVmax.mX + FudgeFactor;
    vmaxTemp[ 1 ] = aVmax.mY + FudgeFactor;
    vmaxTemp[ 2 ] = aVmax.mZ + FudgeFactor;
    mRecBisTree->VoxelsInExternalBox( vminTemp, vmaxTemp,
                                      mVoxelsInBox, mVoxelsInBoxSize, HugeAllocationOfVoxels );

    if( !nd->nLoadVox )
      PLATFORM_ABORT("!nd->nLoadVox");


    BegLogLine( PKFXLOG_SETUP )
      << "InitPostOrbState: About to mNeighborhoodCommDriver.ReInit"
      << EndLogLine;

#else
    mRanksInBox = NULL;
    mRanksInBoxSize = 0;
#endif

#ifdef PK_PHASE5_PROTOTYPE
    CreateLoadBalancedNodeAssignmentTable();

#ifdef PK_PHASE5_SPLIT_COMMS
    
    mSparseSkinSendNodes[ mSparseSkinSendNodesCount ] = Platform::Topology::GetAddressSpaceId();
    mSparseSkinSendNodesCount++;

    BegLogLine( 1 )
        << "COMM_INFO mSparseSkinSendNodesCount: " << mSparseSkinSendNodesCount
        << EndLogLine;

    mNeighborhoodCommDriver.ReInit( mRecBisTree,
                                    mSparseSkinSendNodes,
                                    mSparseSkinSendNodesCount );

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
    
    BegLogLine( 1 )
        << "COMM_INFO Nodes in V4 hood count: " << NodesInHoodCount
        << EndLogLine;

    free( NodesInHood );
    NodesInHood = NULL;        

#else
    mNeighborhoodCommDriver.ReInit( mRadiusOfSphere,
                                    mRecBisTree,
                                    aRanksInP3MEMeshSpace,
                                    aRanksInP3MEMeshSpaceSize,
                                    mSparseSkinSendNodes, 
                                    mSparseSkinSendNodesCount );
#endif
#else
    GenerateOnBoundaryAndMidpointTableForVoxelsVersion2();

    mNeighborhoodCommDriver.ReInit( mRadiusOfSphere,
                                    mRecBisTree,
                                    aRanksInP3MEMeshSpace,
                                    aRanksInP3MEMeshSpaceSize );
#endif


    BegLogLine( PKFXLOG_SETUP )
      << "InitPostOrbState: About to InitVoxelsInBoxTable"
      << EndLogLine;

    InitVoxelsInBoxTable( mVoxelsInBox, mVoxelsInBoxSize );

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

// unsigned int GetHomeForSite( unsigned int aSiteId )
//   {
//   assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

//   return mSiteHome[ aSiteId ];
//   }

#ifdef MANAGE_SPACIAL_DECOMPOSE_ALLTOALLV
void InitSpacialDecompositionManagementAllToAll()
  {
  int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

  int MaxNumberOfLocalFrags = 0;
//  MPI_Allreduce( &NumLocalFrags, &MaxNumberOfLocalFrags, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD );
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
#else
void InitSpacialDecompositionManagement()
  {
  int NumLocalFrags = mIntegratorStateManagerIF.GetNumberOfLocalFragments();

  BegLogLine( 0 )
    << "NumLocalFrags: " << NumLocalFrags
    << EndLogLine;

    // NumberOfLocalFragments * ( fragment_id + imaged_tag_atom + positions_for_frag + velocities_for_frag )
  // mSizeOfSDBuffers = ( NumLocalFrags ) * ( sizeof( int ) + sizeof( XYZ ) + mMaxSitesInFragment * ( 2 * sizeof( XYZ )) );

  // WARNING:: This is a WASTE of memory, multiple messages will fix that

  int MaxNumberOfLocalFrags = 0;
  // MPI_Allreduce( &NumLocalFrags, &MaxNumberOfLocalFrags, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD );
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

  double radius = mCurrentAssignmentGuardZone / 2.0; // Just about half the guard zone

  mPreNumberOfNearestNeighbors = mNumberOfNearestNeighbors;

  mRecBisTree->RanksInNeighborhood( mMyP_x, mMyP_y, mMyP_z, radius, mNearestNeighbors, mNumberOfNearestNeighbors, MaxVoxels );
  //       ORBNode* MyLeaf = mRecBisTree->Node( mMyP_x, mMyP_y, mMyP_z );
  //       assert( MyLeaf );
  //       MyLeaf->RanksInLoadZoneNeighborhood( radius, mNearestNeighbors, mNumberOfNearestNeighbors, MaxVoxels );

//   if( mNumberOfNearestNeighbors >= MAX_NEAREST_NEIGHBORS )
//       PLATFORM_ABORT("mNumberOfNearestNeighbors >= 100");

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
#endif
// inline
// void
// InitLocalSitesMask()
//   {
//   for(int fragmentIndex = 0; fragmentIndex < FRAGMENT_COUNT; fragmentIndex++)
//     {
//     BegLogLine( PKFXLOG_CHECKMASK )
//       << " AddressSpaceId=" << mAddressSpaceId
//       << " mPartitionList[ " << fragmentIndex << " ]=" << mPartitionList[ fragmentIndex ]
//       << EndLogLine;

//     int firstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragmentIndex );
//     int numberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragmentIndex );
//     for( int siteId=firstSiteId; siteId < (firstSiteId + numberOfSites); siteId++ )
//       {
//       assert( siteId < SITE_COUNT );
//       mSiteHome[ siteId ] = mPartitionList[ fragmentIndex ];
//       }
//     }
//   }

// inline
// unsigned
// IsFragmentLocal( int aFragmentId )
//   {
//   assert( aFragmentId >= 0 && aFragmentId < FRAGMENT_COUNT );

//   unsigned rc = ( mPartitionList[ aFragmentId ] == mAddressSpaceId );

//   return ( rc );
//   }

// inline
// unsigned
// IsFragmentInRange( int aFragmentId )
//   {
//   assert( aFragmentId >= 0 && aFragmentId < FRAGMENT_COUNT );

//   unsigned rc = mFragmentInRange[ aFragmentId ];

//   return ( rc );
//   }

// VoxelIndex GetVoxelIndexForFragment( FragId aFragId )
//   {
//   XYZ TagAtomPos        = GetTagAtomPosition( aFragId );
//   VoxelIndex voxelIndex = mRecBisTree->GetVoxelIndex( TagAtomPos );
//   return voxelIndex;
//   }

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

    // BegLogLine( mAddressSpaceId == 0 )
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

  // assert( NumLocalFrags != 0 );

  // InitLocalSitesMask();
#ifdef MANAGE_SPACIAL_DECOMPOSE_ALLTOALLV
  InitSpacialDecompositionManagementAllToAll();
#else
  InitSpacialDecompositionManagement();
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

#ifdef PK_PHASE5_SPLIT_COMMS
  XYZ rc = mBondedInteractionStateManagerIF.GetPosition( fReg.mFragId, fReg.mOffset );
  mBondedInteractionStateManagerIF.SetUseFragment( fReg.mFragId, 1 );
#else  
  XYZ rc = mInteractionStateManagerIF.GetPosition( fReg.mFragId, fReg.mOffset );
#endif
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

#ifdef DO_REAL_SPACE_ON_SECOND_CORE
inline
void
AddForceForSiteOnCoreOne( SiteId aSiteId, const XYZ& aForce )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  mInteractionStateManagerIF.AddForceOnCoreOne( fReg.mFragId, fReg.mOffset, (XYZ&) aForce );
  }
inline
double
ForceXForSiteOnCoreOne( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  return mInteractionStateManagerIF.ForceXOnCoreOne( fReg.mFragId, fReg.mOffset );
  }
inline
double
ForceYForSiteOnCoreOne( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  return mInteractionStateManagerIF.ForceYOnCoreOne( fReg.mFragId, fReg.mOffset );
  }
inline
double
ForceZForSiteOnCoreOne( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  return mInteractionStateManagerIF.ForceZOnCoreOne( fReg.mFragId, fReg.mOffset );
  }
  inline
XYZ
GetForceForSiteOnCoreOne( SiteId aSiteId )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  XYZ fb = mInteractionStateManagerIF.GetForceOnCoreOne( fReg.mFragId, fReg.mOffset );

  return fb ;
//   BegLogLine( SimTick == 0 )
//     << "SiteId: " << aSiteId
//     << " F: " << aForce
//     << EndLogLine;
  }

inline
void
GetForceForSiteOnCoreOne( SiteId aSiteId, XYZ& aForce )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  mInteractionStateManagerIF.GetForceOnCoreOne( fReg.mFragId, fReg.mOffset, aForce );
  }

inline
void
GetForceForSiteOnCoreOne( SiteId aSiteId, double& aForceX, double& aForceY, double& aForceZ )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  mInteractionStateManagerIF.GetForceOnCoreOne( fReg.mFragId, fReg.mOffset, aForceX, aForceY, aForceZ );
  }

inline
void
SetForceForSiteOnCoreOne( SiteId aSiteId, double afX, double afY, double afZ )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];  

  mInteractionStateManagerIF.SetForceOnCoreOne( fReg.mFragId, fReg.mOffset, afX, afY, afZ );
  }
#endif

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

#ifdef PK_PHASE5_SPLIT_COMMS    
  XYZ fb = mBondedInteractionStateManagerIF.GetForce( fReg.mFragId, fReg.mOffset );

  mBondedInteractionStateManagerIF.AddForce( fReg.mFragId, fReg.mOffset, (XYZ&) aForce );
#else
  XYZ fb = mInteractionStateManagerIF.GetForce( fReg.mFragId, fReg.mOffset );

  mInteractionStateManagerIF.AddForce( fReg.mFragId, fReg.mOffset, (XYZ&) aForce );
#endif
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
//   BegLogLine( SimTick == 0 )
//     << "SiteId: " << aSiteId
//     << " F: " << aForce
//     << EndLogLine;
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

//   BegLogLine( SimTick == 0 )
//     << "SiteId: " << aSiteId
//     << " F: " << aForce
//     << EndLogLine;
  }

inline
void
SetForceForSite( SiteId aSiteId, double aForceX, double aForceY, double aForceZ )
  {
  assert( aSiteId >= 0 && aSiteId < SITE_COUNT );

  FragmentRep & fReg = mSiteIdToFragmentRepMap[ aSiteId ];

  mInteractionStateManagerIF.SetForce( fReg.mFragId, fReg.mOffset, aForceX, aForceY, aForceZ );

//   BegLogLine( SimTick == 0 )
//     << "SiteId: " << aSiteId
//     << " F: " << aForce
//     << EndLogLine;
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
// DON'T MUCH LIKE THIS INTERFACE, BUT IT IS USED BY PLIMPTONLOOPS AT THE MOMENT
// inline
// XYZ *
// GetPositionPtr( int aAbsoluteSiteId )
//   {
//   XYZ *rc = &(mPosit[ aAbsoluteSiteId ]);

//   assert( !rc->IsInvalid() );

//   return( rc );
//   }


inline
void
SetPosition( SiteId aAbsoluteSiteId, XYZ aPosition )
  {
  assert( aAbsoluteSiteId >= 0 && aAbsoluteSiteId < SITE_COUNT );
  mPosit[ aAbsoluteSiteId ] = aPosition;
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
SetVelocity( SiteId aAbsoluteSiteId, XYZ aVelocity )
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

  #ifdef PK_PARALLEL
  BegLogLine( PKFXLOG_EXPDYNVAR ) << "ExportDynamicVariables:: Before Barrier " << EndLogLine;
  Platform::Control::Barrier();
  BegLogLine( PKFXLOG_EXPDYNVAR ) << "ExportDynamicVariables:: After Barrier " << EndLogLine;
  #endif
  DumpNoseHooverState( aOuterTimeStep );
  }

#if (defined(DEMO) && (defined (PK_BGL)) && (defined(PK_MPI_ALL)))
// Can't be anything but BGL MPI

struct xyz
  {
  float x, y, z;
  };

template<int NumberOfSites >
struct SC03File
  {
  int TagStr;                       ////String synch packet message:  MDS\0
  int Len;                          ////packet length  : int
  int Type;                         ////packet type   :  int   /// not byte
  int SiteCount;                    ////number of sites: int
  int TimeStep;                     ////timestep: int
  int Offset;                       ////site offset: int
  xyz Posit[ NumberOfSites ];
  };

inline
void
SC03_HackToPrintSnapshotToSocket( int aOuterTimeStep )
  {
  const int FRAMES_IN_FILE = 1000; // Each file holds 1000 time steps
  const int CHANGE_FILES_CODE = 99;

  static SC03File<SITE_COUNT> *SC03FilePtr = NULL;
  int MyRank = -1;
  static unsigned int FileIndexState  = 0; // File to work with
  static unsigned int TimeStepCounter = 0;
  static int isFirstTime     = 1;

  TimeStepCounter++;

  MPI_Comm_rank( Platform::Topology::cart_comm, &MyRank);

  BegLogLine( 0 )
    << "SC03_HackToPrintSnapshotToFile2() "
    << " aOuterTimeStep=" << aOuterTimeStep
    << " MyRank=" <<  MyRank
    << EndLogLine;

  if( MyRank == 0 )
    {
    // if remote port is 0, just spin and don't connect socket
    // allowing standalone testing and performance estimates
    if (Platform::Comm::mRemotePort == 0)
      {
      // output every tenth only
      // (starts with 1, so deal with it - important to see first output
      if (TimeStepCounter%10 == 1)
        {
        printf("DEMO not connected - step %d\n", TimeStepCounter-1);
        }
      #ifdef PK_PARALLEL
      Platform::Control::Barrier();
      #endif
      return;
      }

    char SC03FileName[ 2048 ];
    if( isFirstTime )
      {
      while( 1 )
        {
        int rc = connect( sockfd, (struct sockaddr *) &Platform::Comm::mRemoteAddr, sizeof(struct sockaddr_in));

        if (rc < 0)
          {
          if( errno == ECONNREFUSED || errno == ECONNABORTED )
            {
            sleep( 1 );
            continue;
            }
          PLATFORM_ABORT("connect");
          }
        else
          break;
        }

      isFirstTime = 0;
      }

    if( SC03FilePtr == NULL )
      {
      SC03FilePtr = new SC03File < SITE_COUNT >;
      }

    strncpy( (char *) &SC03FilePtr->TagStr, "MDS", 4);

    SC03FilePtr->Len       =  sizeof( SC03File<SITE_COUNT> );
    SC03FilePtr->Type      = 1;
    SC03FilePtr->SiteCount = SITE_COUNT;
    SC03FilePtr->TimeStep  = 0;
    SC03FilePtr->Offset    = 0;

    for( int s = 0; s < GetNumberOfSites(); s++)
      {
      int externalIndex = MSD_IF::GetInternalSiteIdForExternalSiteId( s );
      SC03FilePtr->Posit[ s ].x = GetPosition( externalIndex ).mX;
      SC03FilePtr->Posit[ s ].y = GetPosition( externalIndex ).mY;
      SC03FilePtr->Posit[ s ].z = GetPosition( externalIndex ).mZ;
      }

    unsigned int checksum1 = 0;
    for( int j=0; j < SC03FilePtr->Len / 4; j++ )
      {
      checksum1 += *((int *)SC03FilePtr + j );
      }

    SC03FilePtr->TimeStep = checksum1;

    write_to_file( sockfd, (char *) SC03FilePtr, SC03FilePtr->Len );

    // sync();
    // fsync( mSock );  // force data to file system

    printf("SC03 demo:: Just wrote ts=%d\n", aOuterTimeStep );

    }

  #ifdef PK_PARALLEL
  Platform::Control::Barrier();
  #endif
  }

#endif

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

//   TraceClient TDynamicVariableManagerTracer::mIntegerAllReduceStart ;
//   TraceClient TDynamicVariableManagerTracer::mIntegerAllReduceFinis ;

  TraceClient TDynamicVariableManagerTracer::mFloatingPointAllReduceStart;
  TraceClient TDynamicVariableManagerTracer::mFloatingPointAllReduceFinis;
#endif


#endif
