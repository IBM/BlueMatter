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
 #ifndef __INTERACTION_STATE_MANAGER_IF_HPP__
#define __INTERACTION_STATE_MANAGER_IF_HPP__

#ifndef PKFXLOG_IFPAUDIT
#define  PKFXLOG_IFPAUDIT (0) 
#endif

#ifndef PKFXLOG_DUALCORE
#define PKFXLOG_DUALCORE (0) 
#endif

#ifndef USE_QSORT
#define USE_QSORT (0)
#endif

#include <stdlib.h>
#include <rptree.hpp>

#if 1
#include <builtins.h>
#define fsel(a, x, y) __fsel((a),(x),(y))
#else
#define fsel(a, x, y) ( (a) >= 0.0 ? (x) : (y) )
#endif

static inline double max2(double A, double B)
  {
  return fsel(A-B,A,B) ;
  }

static inline double max3(double A, double B, double C)
  {
  return max2(max2(A,B),C) ;
  }

static inline double min2(double A, double B)
  {
  return fsel(A-B,B,A) ;
  }

static inline double min3(double A, double B, double C)
  {
  return min2(min2(A,B),C) ;
  }

//#include <BlueMatter/qsort.h>

// XYZ DebugPositions[ NUMBER_OF_FRAGMENTS ][ MAX_SITES_IN_FRAGMENT ];

#define CULLED_VAL 999999.0

static int CompareFragId( const void* elem1, const void* elem2 )
  {
  FragId* fId1 = (FragId *) elem1;
  FragId* fId2 = (FragId *) elem2;
  
  if( *fId1 < *fId2 )
    return -1;
  else if( *fId1 == *fId2 )
    return 0;
  else
    return 1;
  }

struct EnergyHolder
{
double mEnergy MEMORY_ALIGN(5);
double mPADDING[ 4 ]; 
};


/* 
 *  INTERACTION STATE
 */  
class InteractionStateManagerIF
  {        
  public:
  struct InteractionStateTableRecord
    {
    double*  mForces;
    double*  mPositions;

#ifdef DO_REAL_SPACE_ON_SECOND_CORE
    double*  mForcesCoreOne;
#endif

    VoxelIndex mVoxelIndex;    
        
    int mIsFragUsed; // for culling

    double* AddressPosition(unsigned int index)
    {
      assert( mPositions != NULL );
      return mPositions+3*index ;
    } 
    
    void SetPosition( unsigned int index, XYZ aPosition )
      {  	
      assert( mPositions != NULL );

      mPositions[ 3 * index + 0 ] = aPosition.mX ;
      mPositions[ 3 * index + 1 ] = aPosition.mY ;
      mPositions[ 3 * index + 2 ] = aPosition.mZ ;
      }

    void GetPosition( unsigned int index, XYZ& aPosition )
      {  	
      assert( mPositions != NULL );

      aPosition.mX = mPositions[ 3 * index + 0 ] ;
      aPosition.mY = mPositions[ 3 * index + 1 ] ;
      aPosition.mZ = mPositions[ 3 * index + 2 ] ;
      }

    void SetUseFragment( int aUseFragment )
      {  	
      mIsFragUsed = aUseFragment;
      }

    int GetUseFragment()
      {  	
      return mIsFragUsed;
      }
      
    void GetPosition( unsigned int index, double& aX, double& aY, double& aZ )
      {  	
      assert( mPositions != NULL );

      aX = mPositions[ 3 * index + 0 ] ;
      aY = mPositions[ 3 * index + 1 ] ;
      aZ = mPositions[ 3 * index + 2 ] ;
      }

    double PositionX( unsigned int index )
      {  	
      assert( mPositions != NULL );
      return mPositions[ 3 * index + 0 ] ;
      }

    double PositionY( unsigned int index )
      {  	
      assert( mPositions != NULL );
      return mPositions[ 3 * index + 1 ] ;
      }

    double PositionZ( unsigned int index )
      {  	
      assert( mPositions != NULL );
      return mPositions[ 3 * index + 2 ] ;
      }


    XYZ* GetPositionPtr( unsigned int index )
      {  	
      assert( mPositions != NULL );

      return (XYZ *) &mPositions[ 3 * index ];
      }

    double* AddressForce(unsigned int index) 
    {
//    	assert( mForces != NULL );
      return mForces+3*index ;
    } 
    
    void SetForce( unsigned int index, XYZ aForce )
      {  	
      assert( mForces != NULL );

      mForces[ 3 * index + 0 ] = aForce.mX ;
      mForces[ 3 * index + 1 ] = aForce.mY ;
      mForces[ 3 * index + 2 ] = aForce.mZ ;      
      }

    void SetForce( unsigned int index, double aForceX, double aForceY, double aForceZ )
      {  	
      assert( mForces != NULL );

      mForces[ 3 * index + 0 ] = aForceX ;
      mForces[ 3 * index + 1 ] = aForceY ;
      mForces[ 3 * index + 2 ] = aForceZ ;
      }

    void GetForce( unsigned int index, XYZ& aForce )
      {  	
      assert( mForces != NULL );

      aForce.mX = mForces[ 3 * index + 0 ] ;
      aForce.mY = mForces[ 3 * index + 1 ] ;
      aForce.mZ = mForces[ 3 * index + 2 ] ;
      }
      
    void GetForce( unsigned int index, double& aForceX, double& aForceY, double& aForceZ )
      {  	
      assert( mForces != NULL );

      aForceX = mForces[ 3 * index + 0 ] ;
      aForceY = mForces[ 3 * index + 1 ] ;
      aForceZ = mForces[ 3 * index + 2 ] ;
      }
      
    double ForceX( unsigned int index )
      {  	
      assert( mForces != NULL );

      return mForces[ 3 * index + 0 ] ;
      }
      
    double ForceY( unsigned int index )
      {  	
      assert( mForces != NULL );

      return mForces[ 3 * index + 1 ] ;
      }
      
    double ForceZ( unsigned int index )
      {  	
      assert( mForces != NULL );

      return mForces[ 3 * index + 2 ] ;
      }

#ifdef DO_REAL_SPACE_ON_SECOND_CORE
    void SetForceCoreOne( unsigned int index, XYZ aForce )
      {  	
      assert( mForcesCoreOne != NULL );

      mForcesCoreOne[ 3 * index + 0 ] = aForce.mX ;
      mForcesCoreOne[ 3 * index + 1 ] = aForce.mY ;
      mForcesCoreOne[ 3 * index + 2 ] = aForce.mZ ;      
      }

    void SetForceCoreOne( unsigned int index, double aForceX, double aForceY, double aForceZ )
      {  	
      assert( mForcesCoreOne != NULL );

      mForcesCoreOne[ 3 * index + 0 ] = aForceX ;
      mForcesCoreOne[ 3 * index + 1 ] = aForceY ;
      mForcesCoreOne[ 3 * index + 2 ] = aForceZ ;
      }

    void GetForceCoreOne( unsigned int index, XYZ& aForce )
      {  	
      assert( mForcesCoreOne != NULL );

      aForce.mX = mForcesCoreOne[ 3 * index + 0 ] ;
      aForce.mY = mForcesCoreOne[ 3 * index + 1 ] ;
      aForce.mZ = mForcesCoreOne[ 3 * index + 2 ] ;
      }
      
    void GetForceCoreOne( unsigned int index, double& aForceX, double& aForceY, double& aForceZ )
      {  	
      assert( mForcesCoreOne != NULL );

      aForceX = mForcesCoreOne[ 3 * index + 0 ] ;
      aForceY = mForcesCoreOne[ 3 * index + 1 ] ;
      aForceZ = mForcesCoreOne[ 3 * index + 2 ] ;
      }
      
    double ForceCoreOneX( unsigned int index )
      {  	
      assert( mForcesCoreOne != NULL );

      return mForcesCoreOne[ 3 * index + 0 ] ;
      }
      
    double ForceCoreOneY( unsigned int index )
      {  	
      assert( mForcesCoreOne != NULL );

      return mForcesCoreOne[ 3 * index + 1 ] ;
      }
      
    double ForceCoreOneZ( unsigned int index )
      {  	
      assert( mForcesCoreOne != NULL );

      return mForcesCoreOne[ 3 * index + 2 ] ;
      }

    void AddInCoreOneForce( unsigned int index )
      {
      double NewForceX = ForceX( index ) + ForceCoreOneX( index );
      double NewForceY = ForceY( index ) + ForceCoreOneY( index );
      double NewForceZ = ForceZ( index ) + ForceCoreOneZ( index );
      
      SetForce( index, NewForceX, NewForceY, NewForceZ );
      SetForceCoreOne( index, 0.0, 0.0, 0.0 );
      }
#endif      
    } MEMORY_ALIGN( 5 );
  
  InteractionStateTableRecord    mInteractionStateTable[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN( 5 );
  
  // This list containts the fragments in the elliprical sphere
  FragId                         mInteractionShortList[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN( 5 );
  int                            mInteractionShortListSize MEMORY_ALIGN( 5 );
  // unsigned short                 mRealSpaceCulling[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN( 5 );    

  XYZ                            mCulledForce;

  #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
    double                          mCoreOneForces[ 3 * NUMBER_OF_SITES ] MEMORY_ALIGN( 5 );    
    int                             mCoreOneForcesCount;      


    #define REAL_SPACE_ENERGY_COUNT 3

/// This number should come from a macro in platform
    #define NUMBER_OF_PROCESSORS 2
    int                              mRealSpaceEnergyUDF[ REAL_SPACE_ENERGY_COUNT ] MEMORY_ALIGN( 5 );

    #ifdef DUAL_CORE_REAL_SPACE  
      EnergyHolder                   mRealSpaceEnergy[ NUMBER_OF_PROCESSORS ][ REAL_SPACE_ENERGY_COUNT ] MEMORY_ALIGN( 5 );
    #else
      EnergyHolder                   mRealSpaceEnergy[ REAL_SPACE_ENERGY_COUNT ] MEMORY_ALIGN( 5 );
    #endif
  #endif

    #define KSPACE_SPACE_ENERGY_COUNT 2
    double                         padding[4];
    int                            mKspaceSpaceEnergyUDF[ KSPACE_SPACE_ENERGY_COUNT ] MEMORY_ALIGN( 5 );
    EnergyHolder                   mKspaceSpaceEnergy[ KSPACE_SPACE_ENERGY_COUNT ] MEMORY_ALIGN( 5 );      
    double                         padding0[4];
      
   ORBNode*  mRecBisTree;

      // InteractionStateManagerIF() {}

#if (defined( PK_BGL ) && defined( DO_REAL_SPACE_ON_SECOND_CORE ) )
 inline
 void
 AddInCoreOneForce( FragId aFragId, int aNumSites )
   {
   assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
   
   InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
   
   for( int offset=0; offset < aNumSites; offset ++ )
     {
     rec.AddInCoreOneForce( offset );
     }
   }

 inline 
 void
 AddInCoreOneForces( int aSimTick )
   {   
   // NOTE: Cache coherancy happends in the caller's scope.
   // This is due to variate behaviour from running real space on Core0 and Core1      

   for( int i=0; i<mInteractionShortListSize; i++ )
     { 
     FragId fragId = mInteractionShortList[ i ];
     assert( fragId >= 0 && fragId < NUMBER_OF_FRAGMENTS );
     // SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );

     int NumSites       = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
     assert( NumSites >= 1 && NumSites <= 4 );

     // mInteractionStateTable[ fragId ].mForcePacket->mCulled =  mRealSpaceCulling[ fragId ];
     
     AddInCoreOneForce( fragId, NumSites );
     
#if 0
     for( int offset=0; offset < NumSites; offset ++ )
       {
       SiteId CurSiteId = FirstSiteId + offset;

       BegLogLine(PKFXLOG_DUALCORE)
         << "AddInCoreOneForces for ("
         << fragId
         << "," << offset
         << "," << CurSiteId
         << ") [" << mCoreOneForces[ 3*CurSiteId +0 ]
         << "," << mCoreOneForces[ 3*CurSiteId +1 ]
         << "," << mCoreOneForces[ 3*CurSiteId +2 ]
         << "]+[" << ForceX(fragId, offset)
         << "," << ForceY(fragId, offset)
         << "," << ForceZ(fragId, offset)
         << "]" 
         << EndLogLine ;
         
       BegLogLine( 0 )
         << aSimTick
         << " mCoreOneForces[ " << CurSiteId << " ]"
         << mCoreOneForces[ 3*CurSiteId +0 ]
         << " " << mCoreOneForces[ 3*CurSiteId +1 ]
         << " " << mCoreOneForces[ 3*CurSiteId +2 ]
         << EndLogLine;
       

       AddInCoreOneForce( fragId, NumSites, mCoreOneForces[ 3*CurSiteId +0 ],  mCoreOneForces[ 3*CurSiteId +1 ], mCoreOneForces[ 3*CurSiteId +2 ]);       

       BegLogLine(PKFXLOG_DUALCORE)
         << "AddInCoreOneForces resultant ("
         << fragId
         << "," << offset
         << "," << CurSiteId
         << ") [" << ForceX(fragId, offset)
         << "," << ForceY(fragId, offset)
         << "," << ForceZ(fragId, offset)
         << "]" 
         << EndLogLine ;
       
       mCoreOneForces[ 3*CurSiteId+0 ] = 0.0;
       mCoreOneForces[ 3*CurSiteId+1 ] = 0.0;
       mCoreOneForces[ 3*CurSiteId+2 ] = 0.0;
       }
#endif
     }
   }

 inline
 void
 SetEnergyHolderUDFNum( int aOrdinal, int aUDFNum )
   {
   assert( aOrdinal >= 0 && aOrdinal < REAL_SPACE_ENERGY_COUNT );
   mRealSpaceEnergyUDF[ aOrdinal ] = aUDFNum;   
   }

 inline
 int
 GetEnergyHolderUDFNum( int aOrdinal )
   {
   assert( aOrdinal >= 0 && aOrdinal < REAL_SPACE_ENERGY_COUNT );
   return mRealSpaceEnergyUDF[ aOrdinal ];
   }
  
 inline 
 double
 GetRealSpaceEnergy( int aOrdinal )
   {
   assert( aOrdinal >= 0 && aOrdinal < REAL_SPACE_ENERGY_COUNT );

   #ifdef DUAL_CORE_REAL_SPACE     
     double Energy = 0.0;
     for( int i=0; i<NUMBER_OF_PROCESSORS; i++ )
       { 
       rts_dcache_invalidate( GET_BEGIN_PTR_ALIGNED( &mRealSpaceEnergy[ i ][ aOrdinal ] ),
                              GET_END_PTR_ALIGNED( &mRealSpaceEnergy[ i ][ aOrdinal ] + 1 ) );

       Energy += mRealSpaceEnergy[ i ][ aOrdinal ].mEnergy;
       }

     return Energy;
   #else
     return mRealSpaceEnergy[ aOrdinal ].mEnergy;     
   #endif
   }

 inline 
 void 
 SetRealSpaceEnergy( int aOrdinal, double aEnergy )
   {
   assert( aOrdinal >= 0 && aOrdinal < REAL_SPACE_ENERGY_COUNT );

   #ifdef DUAL_CORE_REAL_SPACE
     int core = Platform::Thread::GetId();

     assert( core >= 0 && core < NUMBER_OF_PROCESSORS );
     mRealSpaceEnergy[ core ][ aOrdinal ].mEnergy = aEnergy;

     rts_dcache_store( GET_BEGIN_PTR_ALIGNED( &mRealSpaceEnergy[ core ][ aOrdinal ] ),
                       GET_END_PTR_ALIGNED( &mRealSpaceEnergy[ core ][ aOrdinal ] + 1 ) );
   #else
     mRealSpaceEnergy[ aOrdinal ].mEnergy = aEnergy;
   #endif
   }

 inline
 void
 ReportRealSpaceEnergies( int aSimTick )
   {   
   // NOTE: Cache coherancy happends in the caller's scope.
   // This is due to variate behaviour from running real space on Core0 and Core1      

   #ifdef DUAL_CORE_REAL_SPACE
     rts_dcache_invalidate( GET_BEGIN_PTR_ALIGNED( mRealSpaceEnergy ),
                            GET_END_PTR_ALIGNED( mRealSpaceEnergy + 
                                                 REAL_SPACE_ENERGY_COUNT * NUMBER_OF_PROCESSORS ) );
   #endif
   
   for( int i=0; i<REAL_SPACE_ENERGY_COUNT; i++ )
     {
     if( mRealSpaceEnergyUDF[ i ] != -1 )
       {
       double Energy = 0.0;
       
       #ifdef DUAL_CORE_REAL_SPACE
         for( int j=0; j<NUMBER_OF_PROCESSORS; j++ )
           {
           Energy += mRealSpaceEnergy[ j ][ i ].mEnergy;  
           mRealSpaceEnergy[ j ][ i ].mEnergy = 0.0;
           }
       #else
         Energy = mRealSpaceEnergy[ i ].mEnergy;
         mRealSpaceEnergy[ i ].mEnergy = 0.0;
       #endif
       
       EmitEnergy( aSimTick, mRealSpaceEnergyUDF[ i ], Energy, 0 );
       }
     }

   #ifdef DUAL_CORE_REAL_SPACE
     rts_dcache_store( GET_BEGIN_PTR_ALIGNED( mRealSpaceEnergy ),
                       GET_END_PTR_ALIGNED( mRealSpaceEnergy + 
                                            REAL_SPACE_ENERGY_COUNT * NUMBER_OF_PROCESSORS ) );
   #endif
   }

  inline
  void
  AddForceOnCoreOne( FragId aFragId, int aOffset, XYZ& aForce )
    {
    assert( aForce.IsReasonable(__FILE__, __LINE__) );

    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif
      
    assert( rec.mForcesCoreOne != NULL );

    XYZ OldForce; 
    rec.GetForceCoreOne( aOffset, OldForce );

    XYZ NewForce = OldForce + aForce;
    rec.SetForceCoreOne( aOffset, NewForce.mX, NewForce.mY, NewForce.mZ );
    
    BegLogLine(PKFXLOG_IFPAUDIT)
      << "AddForce aFragId=" << aFragId
      << " aOffset=" << aOffset
      << " aForce=" << aForce
      << " updated forcesum=" << NewForce
      << EndLogLine ;    

//     mCoreOneForces[ 3*aSiteId + 0 ] += aForce.mX ;
//     mCoreOneForces[ 3*aSiteId + 1 ] += aForce.mY ;
//     mCoreOneForces[ 3*aSiteId + 2 ] += aForce.mZ ;

//     BegLogLine(PKFXLOG_DUALCORE)
//       << "AddForceOnCoreOne (" << aSiteId
//       << ") --> [" << mCoreOneForces[ 3*aSiteId + 0 ]
//       << "," << mCoreOneForces[ 3*aSiteId + 1 ]
//       << "," << mCoreOneForces[ 3*aSiteId + 2 ]
//       << "]"
//       << EndLogLine ;
    }

    void SetForceOnCoreOne( unsigned int aFragId, int aOffset, double aForceX, double aForceY, double aForceZ )
      {
      assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
      InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
      
      #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
      #endif
      
      assert( rec.mForcesCoreOne != NULL );
        
      rec.SetForceCoreOne( aOffset, aForceX, aForceY, aForceZ );
 
//       mCoreOneForces[ 3 * index + 0 ] = aForceX ;
//       mCoreOneForces[ 3 * index + 1 ] = aForceY ;
//       mCoreOneForces[ 3 * index + 2 ] = aForceZ ;

//     BegLogLine(PKFXLOG_DUALCORE)
//       << "SetForceOnCoreOne (" << index
//       << ") --> [" << mCoreOneForces[ 3*index + 0 ]
//       << "," << mCoreOneForces[ 3*index+ 1 ]
//       << "," << mCoreOneForces[ 3*index + 2 ]
//       << "]"
//       << EndLogLine ;
      }

    void GetForceOnCoreOne( unsigned int aFragId, int aOffset, XYZ& aForce )
      {
      assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
      InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
      
      #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
      #endif
      
      assert( rec.mForcesCoreOne != NULL );
        
      rec.GetForceCoreOne( aOffset, aForce );
      
//       aForce.mX = mCoreOneForces[ 3 * index + 0 ] ;
//       aForce.mY = mCoreOneForces[ 3 * index + 1 ] ;
//       aForce.mZ = mCoreOneForces[ 3 * index + 2 ] ;
      }
      
    void GetForceOnCoreOne( unsigned int aFragId, int aOffset, double& aForceX, double& aForceY, double& aForceZ )
      {  	
      assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
      InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
      
      #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
      #endif
      
      assert( rec.mForcesCoreOne != NULL );
        
      rec.GetForceCoreOne( aOffset, aForceX, aForceY, aForceZ );
 
//       aForceX = mCoreOneForces[ 3 * index + 0 ] ;
//       aForceY = mCoreOneForces[ 3 * index + 1 ] ;
//       aForceZ = mCoreOneForces[ 3 * index + 2 ] ;
      }
      
    double ForceXOnCoreOne( unsigned int aFragId, int aOffset )
      {  	
      assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
      InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
      
      #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
      #endif
      
      assert( rec.mForcesCoreOne != NULL );
        
      return rec.ForceCoreOneX( aOffset );
      }

    double ForceYOnCoreOne( unsigned int aFragId, int aOffset )
      {  	
      assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
      InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
      
      #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
      #endif
      
      assert( rec.mForcesCoreOne != NULL );
        
      return rec.ForceCoreOneY( aOffset );
      }

    double ForceZOnCoreOne( unsigned int aFragId, int aOffset )
      {  	
      assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
      InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
      
      #ifndef NDEBUG
      int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
      assert( aOffset >= 0 && aOffset < nsites );
      #endif
      
      assert( rec.mForcesCoreOne != NULL );
        
      return rec.ForceCoreOneZ( aOffset );
      }      
#endif
     
  void 
  ChecksumPositions( double aGoldenChecksum )
    {
    #ifdef CHECKSUM_INTERECTION_POSITIONS 
    double PositionCheckInteractionContext1 = DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions();
    if( PositionCheckInteractionContext1 != aGoldenChecksum )
      {
          BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
        << "SimTick: " << SimTick
        << " aGoldenChecksum: " << aGoldenChecksum
        << " PositionCheckInteractionContext1: " << PositionCheckInteractionContext1
        << " ERROR:: Position checksum failed"
        << EndLogLine;
      
     PLATFORM_ABORT( "ERROR:: Position checksum failed" );      
      }
    #endif
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

  double
  ChecksumPositions()
    {
    double checksum = 0.0;

    for( int i=0; i< NUMBER_OF_FRAGMENTS; i++ )
      {
      if( mInteractionStateTable[ i ].mPositions != NULL )
        {
        FragId fId = i;
        int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fId );
        
        assert( numSites >= 1 && numSites <= 4 );
        
        for( int offset = 0; offset < numSites; offset++ )
          {
//          XYZ Pos = GetPosition( fId, offset );
          XYZ Pos ;
          GetPosition( fId, offset, Pos );
          checksum += ( Pos.mX + Pos.mY + Pos.mZ );
          }
        }
      }
#if 0    
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      FragId fId = GetNthFragmentInNeighborhood( i );
      
      int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fId );

      assert( numSites >= 1 && numSites <= 4 );

      for( int offset = 0; offset < numSites; offset++ )
        {
        XYZ Pos = GetPosition( fId, offset );
        
        checksum += ( Pos.mX + Pos.mY + Pos.mZ );
        }

      }

//     long long mask = 0x1;
//     return (double) (((long long) checksum) | mask );
#endif

     return checksum;
    }    

  inline
  int
  IsFragmentInRange( FragId aFragId )
    {
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      if( mInteractionShortList[ i ] == aFragId )
        return 1;
      }
    return 0;
    }

  inline
  void
  EliminateFragmentInNeighborhood( FragId aFragId )
    {
    // Can't cull in the packet layer yet
#ifndef PK_BLADE_SPI
     SetForce( aFragId, 0, mCulledForce );
#endif
    }
      
  inline
  void
  DumpPositions( int aSimTick )
    {
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      FragId fragId = mInteractionShortList[ i ];
      
      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      
      for( int offset = 0; offset < SiteCount; offset++ )
        {
      XYZ Pos ;

        GetPosition(fragId, offset, Pos) ;
        BegLogLine( 1 )
          << aSimTick << " Position " << (FirstSiteId+offset ) 
          << " " << Pos
          << EndLogLine;
//        BegLogLine( 1 )
//          << aSimTick << " Position " << (FirstSiteId+offset ) << " " << GetPosition( fragId, offset )
//          << EndLogLine;
        }
      }
    }    

  inline
  void
  DumpForces( int aSimTick )
    {
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      FragId fragId = mInteractionShortList[ i ];
      
      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      
      for( int offset = 0; offset < SiteCount; offset++ )
        {
        BegLogLine( aSimTick == 5091000 )
          << aSimTick << " RacForce " << fragId << " " << offset << " " << GetForce( fragId, offset ) 
          << EndLogLine;
        }
      }
    }

  void 
  SortFragList()
    {   
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      int MinI = i ;
      FragId MinFragId = mInteractionShortList[ i ];
      for( int j=i; j<mInteractionShortListSize; j++ )
        {
        if( MinFragId >= mInteractionShortList[ j ] )
          {
          MinI = j;
          MinFragId = mInteractionShortList[ j ];
          }
        }

      FragId temp = mInteractionShortList[ MinI ];
      mInteractionShortList[ MinI ] = mInteractionShortList[ i ];
      mInteractionShortList[ i ] = temp;      
      }
    }

   inline
   void
   SortFragmentNeighborList()
     {
#if USE_QSORT      
       Platform::Algorithm::Qsort( mInteractionShortList, mInteractionShortListSize, sizeof( FragId ), CompareFragId );
#else       
//       Platform::Algorithm::Rsort( mInteractionShortList, mInteractionShortListSize, sizeof( FragId ) ) ;
       RadixSort<FragId,FragId,1,0>( mInteractionShortList, mInteractionShortListSize ) ;
#endif	   
     }
  inline
  void
  SortFragmentNeighborList( FragId* aList, int aSize )
    {
#if USE_QSORT      
    Platform::Algorithm::Qsort( aList, aSize, sizeof( FragId ), CompareFragId );
#else       
//    Platform::Algorithm::Rsort( aList, aSize, sizeof( FragId ) );
    RadixSort<FragId,FragId,1,0>( aList, aSize );
#endif     
    }

  inline
  int
  GetFragmentNeighborListSize()
    {
    return mInteractionShortListSize;
    }

  inline
  FragId
  GetNthFragmentInNeighborhood( int n )
    {
    assert( n >= 0 && n < mInteractionShortListSize );
    
    return mInteractionShortList[ n ];
    }  

  void
  SetOrb( ORBNode* aORB )
    {
    mRecBisTree = aORB;    
    }
 
  inline
  void
  Init()      
    {            
    mInteractionShortListSize = 0;
    
    mCulledForce.Set( CULLED_VAL, CULLED_VAL, CULLED_VAL );

  #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
    mCoreOneForcesCount = 0;
    for( int i=0; i<REAL_SPACE_ENERGY_COUNT; i++ )
      {
      mRealSpaceEnergyUDF[ i ] = -1;

       #ifdef DUAL_CORE_REAL_SPACE
         for( int j=0; j<NUMBER_OF_PROCESSORS; j++ )
           mRealSpaceEnergy[ j ][ i ].mEnergy = 0.0;
       #else
         mRealSpaceEnergy[ i ].mEnergy = 0.0;
       #endif
      }

    for( int i=0; i<NUMBER_OF_SITES; i++ )
      {
      mCoreOneForces[ 3 * i + 0 ]  = 0.0;
      mCoreOneForces[ 3 * i + 1 ]  = 0.0;
      mCoreOneForces[ 3 * i + 2 ]  = 0.0;
      }
  #endif

#ifdef PK_PHASE5_SPLIT_COMMS    
    mKspaceSpaceEnergy[ 0 ].mEnergy = 0.0;
    mKspaceSpaceEnergy[ 1 ].mEnergy = 0.0;
#endif
    }

  void    
  SetKspaceEnergy( int aOrdinal, int aUdfCode, double aEnergy )
    {
    assert( aOrdinal >= 0 && aOrdinal < KSPACE_SPACE_ENERGY_COUNT );
    mKspaceSpaceEnergyUDF[ aOrdinal ] = aUdfCode;
    mKspaceSpaceEnergy[ aOrdinal ].mEnergy = aEnergy;
    }
      
  double
  GetKspaceEnergy( int aOrdinal )
    {
    assert( aOrdinal >= 0 && aOrdinal < KSPACE_SPACE_ENERGY_COUNT );
    return mKspaceSpaceEnergy[ aOrdinal ].mEnergy;
    }

  void
  ReportKspaceEnergy( int aSimTick )
   {
   for( int i = 0; i < KSPACE_SPACE_ENERGY_COUNT; i++)
     {
     EmitEnergy( aSimTick, mKspaceSpaceEnergyUDF[i], mKspaceSpaceEnergy[i].mEnergy, 0 );
     mKspaceSpaceEnergy[i].mEnergy = 0.0;
     }
   }

  inline
  void
  ResetState()
    {
      #ifdef CHECKSUM_INTERECTION_POSITIONS
      for( int i=0; i<NUMBER_OF_FRAGMENTS; i++ )
        {
        mInteractionStateTable[ i ].mForces = NULL;
        mInteractionStateTable[ i ].mPositions = NULL;
        }
      #endif
    mInteractionShortListSize = 0;

#ifdef DO_REAL_SPACE_ON_SECOND_CORE
    mCoreOneForcesCount = 0;
#endif
    }

  inline    
  void
  SetPositions( FragId aFragId, XYZ* aPositions )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    // BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
    BegLogLine( 0 )
      << SimTick
      << " "
      << aFragId
      << " " << (void *) aPositions
      << " " << (void *) mInteractionStateTable[ aFragId ].mPositions
      << EndLogLine;

    mInteractionStateTable[ aFragId ].mPositions = (double *) aPositions;
    }

  inline    
  void
  SetForces( FragId aFragId, XYZ* aForces )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );    

    BegLogLine( 0 )
      << SimTick
      << " "
      << aFragId
      << " " << (void *) aForces
      << " " << (void *) mInteractionStateTable[ aFragId ].mForces
      << EndLogLine;

    mInteractionStateTable[ aFragId ].mForces = (double *) aForces;

#ifdef DO_REAL_SPACE_ON_SECOND_CORE
    int NumberOfSites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );

    BegLogLine( 0 )
      << SimTick
      << " "
      << aFragId
      << " " << (void *) & mCoreOneForces[ mCoreOneForcesCount ]
      << " " << (void *) mInteractionStateTable[ aFragId ].mForcesCoreOne
      << EndLogLine;
    
    assert( mCoreOneForcesCount >= 0 && mCoreOneForcesCount < (3 * NUMBER_OF_SITES) );
    mInteractionStateTable[ aFragId ].mForcesCoreOne = & mCoreOneForces[ mCoreOneForcesCount ];
    
    mCoreOneForcesCount += (NumberOfSites * 3);
#endif
    }
  
  inline
  void
  ZeroForce( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    for( int offset=0; offset < nsites; offset++)
      {
      mInteractionStateTable[ aFragId ].SetForce( offset, XYZ::ZERO_VALUE() );

#ifdef DO_REAL_SPACE_ON_SECOND_CORE
      mInteractionStateTable[ aFragId ].SetForceCoreOne( offset, XYZ::ZERO_VALUE() );
#endif
      }
    }

  inline
  void
  AddFragment( FragId aFragId,
               XYZ* aPositions,
               XYZ* aForces )
    {
    SetPositions( aFragId, aPositions );
    
    SetForces( aFragId, aForces );
    
    SetUseFragment( aFragId, 0 );
      
    // Forces get added during the interaction phase
    // ZeroForce( aFragId );

    assert( mInteractionShortListSize >= 0 && 
            mInteractionShortListSize < NUMBER_OF_FRAGMENTS );
    
    mInteractionShortList[ mInteractionShortListSize ] = aFragId;        
    mInteractionShortListSize++;    
    }

  inline
  VoxelIndex
  GetVoxelIndex( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
        
    VoxelIndex rc = rec.mVoxelIndex;
    
    return( rc );
    }

  inline
  void
  SetVoxelIndex( FragId     aFragId,
                 VoxelIndex aVoxelIndex )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    mInteractionStateTable[ aFragId ].mVoxelIndex = aVoxelIndex;    
    }

//#if defined(ISM_PACKET_ORDINAL)
// This one generally causes the compiler to make better code
  inline double* AddressPosition(FragId aFragId, int aOffset)
  {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif  
    
    return rec.AddressPosition( aOffset );
    
  }
  
  inline 
  void
  GetPosition (  FragId aFragId, int aOffset, XYZ & aPosition)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif  
    
    rec.GetPosition( aOffset, aPosition );
    }

  inline 
  void
  SetUseFragment( FragId aFragId, int aUseFragment )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
        
    rec.SetUseFragment( aUseFragment );
    }

  inline 
  int
  GetUseFragment( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    return rec.GetUseFragment( );
    }
    
  inline 
  void
  GetPosition (  FragId aFragId, int aOffset, double& aX, double& aY, double& aZ)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif  
    
    rec.GetPosition( aOffset, aX, aY, aZ );
    }

  inline 
  double
  PositionX (  FragId aFragId, int aOffset)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif  
    
    return rec.PositionX( aOffset );
    }

  inline 
  double
  PositionY (  FragId aFragId, int aOffset)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif  
    
    return rec.PositionY( aOffset );
    }

  inline 
  double
  PositionZ (  FragId aFragId, int aOffset)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif  
    
    return rec.PositionZ( aOffset );
    }

  inline 
  XYZ*
  GetPositionPtr(  FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif  
    
    return rec.GetPositionPtr( aOffset );
    }

  inline 
  double* 
  AddressForce (  FragId aFragId, int aOffset)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    return rec.AddressForce( aOffset);
    }
    
  inline 
  void
  GetForce (  FragId aFragId, int aOffset, XYZ & aForce)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    rec.GetForce( aOffset, aForce);
    }

  inline 
  void
  GetForce (  FragId aFragId, int aOffset, double& aForceX, double& aForceY, double& aForceZ)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    rec.GetForce( aOffset, aForceX, aForceY, aForceZ);
    }

  inline 
  double
  ForceX (  FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    return rec.ForceX( aOffset );
    }

  inline 
  double
  ForceY (  FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    return rec.ForceY( aOffset );
    }

  inline 
  double
  ForceZ (  FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    return rec.ForceZ( aOffset );
    }

  inline 
  void
  SetForce (  FragId aFragId, int aOffset, XYZ & aForce)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    rec.SetForce( aOffset, aForce);
    BegLogLine(PKFXLOG_DUALCORE)
      << "SetForce (" << aFragId
      << "," << aOffset
      << ") --> " << aForce
      << EndLogLine ;
    }

  inline 
  void
  SetForce (  FragId aFragId, int aOffset, double aForceX, double aForceY, double aForceZ)
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    rec.SetForce( aOffset, aForceX, aForceY, aForceZ );
    BegLogLine(PKFXLOG_DUALCORE)
      << "SetForce (" << aFragId
      << "," << aOffset
      << ") --> [" << aForceX
      << "," << aForceY
      << "," << aForceZ
      << "]"
      << EndLogLine ;
    }

// This one for compatibility  
  inline
  XYZ
  GetPosition( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    XYZ rc ;
    rec.GetPosition(aOffset,rc);
       
    return( rc );
    }

// Another 'compatibility' one    
  inline
  XYZ
  GetForce( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    XYZ rc ;
    rec.GetForce(aOffset, rc) ;
    
    return( rc );
    }
// #else
//   inline
//   XYZ *
//   GetPositionPtr( FragId aFragId, int aOffset )
//     {
//     assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
//     InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
//     #ifndef NDEBUG
//     int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
//     assert( aOffset >= 0 && aOffset < nsites );  
//     #endif  

//     assert( rec.mPositionPacket != NULL );
    
//     XYZ * rc = & (rec.mPositionPacket->mPosition[ aOffset ]);
    
//     return( rc );
//     }

//   inline
//   XYZ&
//   GetPosition( FragId aFragId, int aOffset )
//     {
//     assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
//     InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
//     #ifndef NDEBUG
//     int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
//     assert( aOffset >= 0 && aOffset < nsites );  
//     #endif  
    
//     assert( rec.mPositionPacket != NULL );

//     XYZ & rc = rec.mPositionPacket->mPosition[ aOffset ];
       
//     return( rc );
//     }
    
//   inline
//   void
//   GetPosition( FragId aFragId, int aOffset, XYZ& aPosition )
//     {
//     assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
//     InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
//     #ifndef NDEBUG
//     int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
//     assert( aOffset >= 0 && aOffset < nsites );  
//     #endif  
    
//     assert( rec.mPositionPacket != NULL );

//     aPosition = rec.mPositionPacket->mPosition[ aOffset ];
       
//     }


//   inline
//   XYZ&
//   GetForce( FragId aFragId, int aOffset )
//     {
//     assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
//     InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
//     #ifndef NDEBUG
//     int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
//     assert( aOffset >= 0 && aOffset < nsites );  
//     #endif  
    
//     assert( rec.mForcePacket != NULL );

//     XYZ & rc = rec.mForcePacket->mForce[ aOffset ];
    
//     return( rc );
//     }
// #endif    

//   inline
//   short 
//   GetCulled( FragId aFragId )
//     {
//     assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
//     InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
        
//     assert( rec.mForcePacket != NULL );

//     short rc = rec.mForcePacket->mCulled;
    
//     return( rc );
//     }
  
  inline
  void
  AddForce( FragId aFragId, int aOffset, XYZ aForce )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif
      
    assert( rec.mForces != NULL );

    XYZ OldForce; 
    rec.GetForce( aOffset, OldForce );

    XYZ NewForce = OldForce + aForce;
    rec.SetForce( aOffset, NewForce );
    
    BegLogLine(PKFXLOG_IFPAUDIT)
      << "AddForce aFragId=" << aFragId
      << " aOffset=" << aOffset
      << " aForce=" << aForce
      << " updated forcesum=" << NewForce
      << EndLogLine ;    
    }      
    
  inline
  void
  AddForce( FragId aFragId, int aOffset, double aForceX, double aForceY, double aForceZ )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );
    #endif
      
    assert( rec.mForces != NULL );

    rec.SetForce( aOffset, rec.ForceX(aOffset)+aForceX, rec.ForceY(aOffset)+aForceY,rec.ForceZ(aOffset)+aForceZ );
    
    BegLogLine(PKFXLOG_IFPAUDIT)
      << "AddForce aFragId=" << aFragId
      << " aOffset=" << aOffset
      << EndLogLine ;    
    }      

VoxelIndex
GetVoxelIndex( XYZ* aPos, int aCount )
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

  return mRecBisTree->GetVoxelIndex( mid );
  }

  inline
  void
  CullUnusedFragments()
    {
    for( int i=0; i < mInteractionShortListSize; i++ )
      {
      FragId FragmentId = mInteractionShortList[ i ];
      if( !GetUseFragment( FragmentId ) )
        {
// 	  BegLogLine( SimTick == 0 )
// 	    << " Sender culling:  " 
// 	    << FragmentId
// 	    << EndLogLine;

        SetForce( FragmentId, 0, mCulledForce );
        }
      }
    }
    
  inline
  void
  ZeroForces()
    {
    for( int i=0; i < mInteractionShortListSize; i++ )
      {
      FragId FragmentId = mInteractionShortList[ i ];
      ZeroForce( FragmentId );
      }
    }
  };
#endif
