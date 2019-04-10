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
 * Module:          NSQEngine
 *
 * Purpose:         Manage NSQ interactions as 3:3 'water-water' + site-site
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         050822 TJCW created from NSQ_fragment_verlet_phase4
 *                  051118 TJCW refreshed from NSQ_fragment_verlet_phase4
 * 
 * This NSQ produces an atom-atom Verlet list, and drives an IFP 
 * which iterates through the atom-atom list.
 * As an optimisation, because typically most of the interactions are
 * '3:3 no-exclusion' (i.e. 'water-water', plus whatever of the protein
 * is handled as 3-atom fragments with no exclusions), whatever of the
 * frag-frag Verlet list fits this category can be split off and handled
 * as its own Verlet list. The optimisation is supposed to be the reduction 
 * of memory footprint, 
 *
 ***************************************************************************/
#ifndef __NSQ_WATERSITE_VERLET_PHASE5_HPP__
#define __NSQ_WATERSITE_VERLET_PHASE5_HPP__

#ifndef PKFXLOG_VECTOR_VERLET_COUNT
#define PKFXLOG_VECTOR_VERLET_COUNT ( 0 )
#endif

#ifndef PKFXLOG_NSQ_DEBUG
#define PKFXLOG_NSQ_DEBUG ( 0 ) 
#endif

#ifndef PKFXLOG_WATERSITE
#define PKFXLOG_WATERSITE ( 0 ) 
#endif

#ifndef PKFXLOG_NSQ_GUARD_ZONE_TUNE
#define PKFXLOG_NSQ_GUARD_ZONE_TUNE ( 1 )
#endif

#ifndef PKFXLOG_NSQ_INIT
#define PKFXLOG_NSQ_INIT      ( 0 )
#endif

#ifndef PKFXLOG_TRIGGERSETTIMEFORFRAGMENT
#define PKFXLOG_TRIGGERSETTIMEFORFRAGMENT ( 0 )
#endif

#ifndef PKFXLOG_NSQSOURCEFRAG_SUMMARY
#define PKFXLOG_NSQSOURCEFRAG_SUMMARY 0
#endif

#ifndef PKFXLOG_NSQSOURCEFRAG_SUMMARY1
#define PKFXLOG_NSQSOURCEFRAG_SUMMARY1 ( 0 )
#endif

#ifndef PKFXLOG_NSQ_VERLET_LIST_DEBUG
#define PKFXLOG_NSQ_VERLET_LIST_DEBUG ( 0 )
#endif

#ifndef PKFXLOG_NSQ_VIRIAL
#define PKFXLOG_NSQ_VIRIAL 0
#endif

#ifndef BLUEMATTER_STATS1
#define BLUEMATTER_STATS1 0
#endif

#if BLUEMATTER_STATS1
#define BLUEMATTER_STATS0 1
#endif

#ifndef BLUEMATTER_STATS0
#define BLUEMATTER_STATS0 0
#endif

#ifndef PKFXLOG_NSQ_SEND
#define PKFXLOG_NSQ_SEND ( 0 )
#endif

#ifndef PKFXLOG_NSQ_DISTANCE_SQRD
#define PKFXLOG_NSQ_DISTANCE_SQRD  ( 0 )
#endif

#ifndef PKFXLOG_NSQLOOP_EMIT
#define PKFXLOG_NSQLOOP_EMIT       ( 0 )
#endif

#ifndef PKFXLOG_VERLET_LIST_DEBUG
#define PKFXLOG_VERLET_LIST_DEBUG ( 0 )
#endif

#ifndef PKFXLOG_SORT_VERLET
#define PKFXLOG_SORT_VERLET ( 0 ) 
#endif

#ifndef PKFXLOG_VECTOR_VERLET
#define PKFXLOG_VECTOR_VERLET ( 0 ) 
#endif

#ifndef PKFXLOG_VECTOR_VERLET1
#define PKFXLOG_VECTOR_VERLET1 ( 0 ) 
#endif

#ifndef PKFXLOG_VECTOR_VERLET_SETUP
#define PKFXLOG_VECTOR_VERLET_SETUP ( 0 ) 
#endif

#ifndef FORCE_ATOM_ATOM_FRAGS
#define FORCE_ATOM_ATOM_FRAGS ( 1 )
#endif

#ifndef PKFXLOG_NSQ_INFO
#define PKFXLOG_NSQ_INFO (0) 
#endif

#define MAX_REAL_SPACE_TIME_STEP_TIME    9999999999.0

static TraceClient NSQ_RealSpace_Meat_Start;
static TraceClient NSQ_RealSpace_Meat_Finis;

static TraceClient FatCompPrecomputeStart;
static TraceClient FatCompPrecomputeFinis;

static TraceClient FatCompVerletListGenStart;
static TraceClient FatCompVerletListGenFinis;

static TraceClient FatCompProcessPairStart;
static TraceClient FatCompProcessPairFinis;

static TraceClient VoxInteractionTableIterStart;
static TraceClient VoxInteractionTableIterFinis;

#include <pk/platform.hpp>
#include <pk/pkstatistic.hpp>

#define NSQ_KEEP_STATISTICS

class NSQStatistic
{
public:
#if defined(NSQ_KEEP_STATISTICS)
   int mEntryTime ;
   int mMeatStartTime ;
   int mMeatEndTime ;
   Statistic mNonMeatStat ;
   Statistic mMeatStat ;
   void Entry(void)
   {
         mEntryTime = Platform::Clock::Oscillator() ;
   }

   void Exit(void)
      {
      // mNonMeatStat.Observe(mMeatStartTime-mEntryTime) ;
      mMeatStat.Observe(mMeatEndTime-mMeatStartTime) ;
      }
    
   void MeatStart(void)
      {
      mMeatStartTime = Platform::Clock::Oscillator() ;
      }

   void MeatEnd(void)
      {
      mMeatEndTime = Platform::Clock::Oscillator() ;
      }
#else
   void Entry(void) { }
   void Exit(void) { }
   void MeatStart(void) { }
   void MeatEnd(void) { }
#endif
} ;
static NSQStatistic NSQStat ;
// Platform::Statistic RealSpaceTime ;

#include <BlueMatter/UDF_BindingWrapper.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
//******************************************************

#define KSPACE_DONE_LOCK 22

//****************************************************************
typedef unsigned int FragIdPairKey;

inline
FragIdPairKey
MakeFragIdPairKey( FragId aT, FragId aS )
  {
  assert( aT < aS );
  assert( (aS & 0x0000FFFF)  == aS ); // Must fit in a shot - when we want more frags, make key must return long long
  assert( (aT & 0x0000FFFF)  == aT ); // Must fit in a shot - when we want more frags, make key must return long long

  FragIdPairKey rc = (aT << 16) | aS;
  return( rc ) ;
  }

inline
void
GetFragIdFromPairKey( FragIdPairKey aK, FragId &aT, FragId &aS )
  {
  aT = aK >> 16;
  aS = aK & 0x0000FFFF;
  assert( aK == MakeFragIdPairKey( aT, aS ) );
  return;
  }

inline  FragIdPairKey
GetFragIdPairKeyMax()
  {
  return( 0xFFFEFFFF );
  }

struct FragmentVerletPair
  {
  FragIdPairKey     mFragIdPairKey;
  unsigned int mExclusionMask            : 25 ;
  unsigned int mFragmentImageVectorIndex :  7 ;
  };

struct FragAssignmentInfo
  {
  unsigned int      mFragmentDatabaseIndex;
  };

struct RealSpaceSignalSet
  {
  unsigned char mResetSignal;
  unsigned char mUpdateFragmentVerletListSignal;
  unsigned char mUpdateFragmentAssignmentListSignal;
  };

static InteractionStateManagerIF TempISM MEMORY_ALIGN(5);
template <class VerletListElement>
class VerletList
{
public:
  VerletListElement * mVerletList ;
  unsigned int mCount ;
  unsigned int mLength ;
 
  unsigned int GetCount(void) const 
  {
    return mCount ;
  } 

  void Dump( int aSimTick )
  {
    for( int i = 0; i < mCount; i++ )
      {
	BegLogLine( aSimTick == 0 )
	  << "VLS[ " << i << " ]: " 
	  << "{ " << mVerletList[ i ].mElA << " "
	  << mVerletList[ i ].mElB << " }"
	  << EndLogLine;
      }
  }

  unsigned int GetElA(unsigned int aIndex) const
  {
    assert(aIndex < mCount ) ; 
    return mVerletList[aIndex].mElA ;
  } 
				
  unsigned int GetElB(unsigned int aIndex) const
  {
    assert(aIndex < mCount ) ; 
    return mVerletList[aIndex].mElB ;
  } 
				
  unsigned int GetImageVectorTableIndex(unsigned int aIndex) const
  {
    assert(aIndex < mCount) ;
    assert(mVerletList[aIndex].mImageVectorTableIndex < 125 ) ;
    return mVerletList[aIndex].mImageVectorTableIndex ;
  }
				
  float GetImageMultiplierX(unsigned int aIndex) const
  {
    assert(aIndex < mCount) ;
    return mVerletList[aIndex].mImageMultiplierX ;
  }
				
  float GetImageMultiplierY(unsigned int aIndex) const
  {
    assert(aIndex < mCount) ;
    return mVerletList[aIndex].mImageMultiplierY ;
  }
				
  float GetImageMultiplierZ(unsigned int aIndex) const
  {
    assert(aIndex < mCount) ;
    return mVerletList[aIndex].mImageMultiplierZ ;
  }
								
  unsigned int GetFragA( unsigned int aIndex ) const
  {
    assert ( aIndex < mCount ) ;
    return mVerletList[aIndex].mElA >> 4 ;
  }	
			    
  unsigned int GetOffsetA( unsigned int aIndex ) const 
  {
    assert ( aIndex < mCount ) ;
    return mVerletList[aIndex].mElA & 0x0000000f ;
  }	
			    
  unsigned int GetFragB( unsigned int aIndex ) const
  {
    assert ( aIndex < mCount ) ;
    return mVerletList[aIndex].mElB >> 4 ;
  }	
			    
  unsigned int GetOffsetB( unsigned int aIndex ) const
  {
    assert ( aIndex < mCount ) ;
    return mVerletList[aIndex].mElB & 0x0000000f ;
  }	
			    
				
  void Init(void)
  {
    mCount = 0 ; 
    mLength = 0 ;
    SetLength( 50000 );
  } 
				
  void Term(void)
  {
    if ( mLength > 0 ) free(mVerletList) ;
    mCount = 0 ; 
    mLength = 0 ; 
  }

  // This variety of 'setlength' is only to be used 
  // when there is no data in the list ...				
  void SetLength(unsigned int aLength)
  {
    unsigned int bLength = aLength * 5 + 10000 ; 
    if ( aLength > mLength )
      {
	BegLogLine( PKFXLOG_VERLET_LIST_DEBUG )
	  << "SetLength extending a-a list to length " << bLength
	  << " old length was " << mLength
	  << " Platform::Thread::GetId() = " << Platform::Thread::GetId()
	  << EndLogLine ;
                                  
#ifdef PK_BGL
	if( Platform::Thread::GetId() == 1 )
	  {
	    char* Abort = NULL;
	    Abort[ 0 ] = 0;
	  }
#endif

	if ( mLength > 0 ) 
	  {
	    //                                    free(mVerletList) ;
	    Platform::Heap::Free(mVerletList) ;
	  } 
			          
	//                    void * Memory = malloc( bLength * sizeof(VerletListElement) ) ;
	void * Memory = Platform::Heap::Allocate(bLength * sizeof(VerletListElement)) ;
                                  
	if( Memory == NULL )
	  PLATFORM_ABORT( "ERROR:: Failed to malloc memory for mVerletList " );
                                  
	mVerletList = (VerletListElement *) Memory ;
	BegLogLine(PKFXLOG_VECTOR_VERLET_SETUP) 
	  << "SetLength from " << mLength
	  << " to " << bLength
	  << " list at " << Memory
	  << EndLogLine ;
	mLength = bLength ;
	mCount = 0 ; 
      }
  }
				
  void SetCount(unsigned int aCount )
  {
    if ( aCount > mLength )
      {
	SetLength(aCount) ; 
      }
    mCount = aCount ;
  }
} ;
			
class SiteVerletListElement
{
public: 
  unsigned int mElA ;
  unsigned int mElB ;
#if defined(IFP_IMAGE_IN_VERLET)	
  unsigned int mImageVectorTableIndex ;
  float mImageMultiplierX ;
  float mImageMultiplierY ;
  float mImageMultiplierZ ;
#endif	

} ;
						
class SiteVerletList: public VerletList<SiteVerletListElement> 
{
public: 
  template<class DynVarMgrIF>
  void SetSitePair(unsigned int aIndex,
		   unsigned int aFragA,
		   unsigned int aOffsetA,
		   unsigned int aFragB,
		   unsigned int aOffsetB,
		   unsigned int aImageVectorTableIndex,
		   DynVarMgrIF * aDynVarMgrIF
		   ) 
  {
    assert ( aIndex < mCount ) ;
    assert(aImageVectorTableIndex < 125) ; // 'we know' that it should be ...
    BegLogLine(PKFXLOG_VECTOR_VERLET)
      << "VerletList::SetSitePair aIndex=" << aIndex
      << " aFragA=" << aFragA
      << " aOffsetA=" << aOffsetA
      << " aFragB=" << aFragB
      << " aOffsetB=" << aOffsetB
      << " aImVX=" << aImageVectorTableIndex
      << EndLogLine ;
			    	
    mVerletList[aIndex].mElA = ( (aFragA << 4 ) & 0xfffffff0 ) 
      | ( (aOffsetA & 0x0000000f ) ) ;
    mVerletList[aIndex].mElB = ( (aFragB << 4 ) & 0xfffffff0 ) 
      | ( (aOffsetB & 0x0000000f ) ) ;
#if defined(IFP_IMAGE_IN_VERLET)    	                         
    mVerletList[aIndex].mImageVectorTableIndex = aImageVectorTableIndex ;
					
    mVerletList[aIndex].mImageMultiplierX = aDynVarMgrIF->GetImageMultiplierX(aImageVectorTableIndex) ;
    mVerletList[aIndex].mImageMultiplierY = aDynVarMgrIF->GetImageMultiplierY(aImageVectorTableIndex) ;
    mVerletList[aIndex].mImageMultiplierZ = aDynVarMgrIF->GetImageMultiplierZ(aImageVectorTableIndex) ;
#endif		
  }							 
				
} ; 
			
class FVerletListElement
{
public:
  unsigned int mElA ;
  unsigned int mElB ;	
  unsigned int mImageVectorTableIndex ;
} ;
			
class FVerletList: public VerletList<FVerletListElement>
{
public: 
  template<class DynVarMgrIF>
  void SetFragPair(unsigned int aIndex
		   , unsigned int aElA
		   , unsigned int aElB 
		   , unsigned int aImageVectorTableIndex
		   , DynVarMgrIF * aDynVarMgrIF
		   )
  {
    assert(aIndex < mCount) ;
    assert(aImageVectorTableIndex < 125) ; // 'we know' that it should be ...
    BegLogLine(PKFXLOG_VECTOR_VERLET)
      << "VerletList::SetFragPair aIndex=" << aIndex
      << " aElA=" << aElA
      << " aElB=" << aElB
      << " aImVX=" << aImageVectorTableIndex
      << EndLogLine ;
    mVerletList[aIndex].mElA = aElA ;
    mVerletList[aIndex].mElB = aElB ;
    mVerletList[aIndex].mImageVectorTableIndex = aImageVectorTableIndex ;
  }
			
				
} ; 
template < class DynVarMgrIF >
class IrreducibleFragmentOperandIF
{
public:

  int          mSimTick;
  int          mFragmentOrdinal;
  DynVarMgrIF *mDynVarMgrIF;


  IrreducibleFragmentOperandIF( int          aSimTick,
				DynVarMgrIF *aDynVarMgrIF ,
				int          aFragmentOrdinal )
  {
    mSimTick          = aSimTick;
    mDynVarMgrIF      = aDynVarMgrIF;
    mFragmentOrdinal = aFragmentOrdinal;
  }

  inline int
  GetIrreducibleFragmentId()
  {
    int rc =  mFragmentOrdinal;
    return( rc );
  }

  static inline int
  GetIrreducibleFragmentId(DynVarMgrIF *aDynVarMgrIF, int aFragmentOrdinal )
  {
    int rc = aFragmentOrdinal;
    return( rc );
  }


  // OK OK... avoided a copy, but at the cost of exposing a pointer into otherwise encapsulated Dynamic Variable Manager
  //NEED TO THINK ABOUT THIS...
  inline
  XYZ *             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
  GetPositionPtr( int aIndex )
  {
    XYZ * rc   =  mDynVarMgrIF->mInteractionStateManagerIF.GetPositionPtr( mFragmentOrdinal, aIndex ) ;
    return( rc );
  }

  inline
  XYZ
  GetPosition( int aIndex )
  {
    XYZ rc   =  mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( mFragmentOrdinal, aIndex ) ;
    return( rc );
  }


  inline
  void
  GetPosition( int aIndex, XYZ& aPosition)
  {
    mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( mFragmentOrdinal, aIndex, aPosition ) ;
  }
                                                                                
  inline
  void
  GetPosition( int aIndex, double& aX, double& aY, double& aZ)
  {
    mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( mFragmentOrdinal, aIndex, aX, aY, aZ ) ;
  }
              
            
  inline
  double
  PositionX( int aIndex)
  {
    return mDynVarMgrIF->mInteractionStateManagerIF.PositionX( mFragmentOrdinal, aIndex ) ;
  }
  inline
  double
  PositionY( int aIndex)
  {
    return mDynVarMgrIF->mInteractionStateManagerIF.PositionY( mFragmentOrdinal, aIndex ) ;
  }
  inline
  double
  PositionZ( int aIndex)
  {
    return mDynVarMgrIF->mInteractionStateManagerIF.PositionZ( mFragmentOrdinal, aIndex ) ;
  }
                                                                                

  static inline
  XYZ *             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
  GetPositionPtr( DynVarMgrIF *aDynVarMgrIF, int aFragmentOrdinal, int aIndex )
  {
    assert( 0 );
    XYZ * rc   =  aDynVarMgrIF->GetPositionPtr( aFragmentOrdinal, aIndex ) ;
    return( rc );
  }

  static inline
  XYZ
  GetPosition( DynVarMgrIF *aDynVarMgrIF, int aFragmentOrdinal, int aIndex )
  {
    assert( 0 );
    XYZ rc   =  aDynVarMgrIF->GetPosition( aFragmentOrdinal, aIndex ) ;
    return( rc );
  }

  inline
  XYZ
  GetCenterOfMassPosition()
  {
    return mDynVarMgrIF->GetCenterOfMassPosition( mFragmentOrdinal );
  }
};

template <class DynVarMgrIF>
class FragmentPartialResultsIF
{
public:

  int          mSimTick;
  int          mFragmentOrdinal;
  DynVarMgrIF *mDynVarMgrIF;

  FragmentPartialResultsIF( int          aSimTick,
			    DynVarMgrIF *aDynVarMgrIF )
  {
    mSimTick          = aSimTick;
    mDynVarMgrIF      = aDynVarMgrIF;
  }

  FragmentPartialResultsIF( int          aSimTick,
			    DynVarMgrIF *aDynVarMgrIF,
			    int aFragmentOrdinal )
  {
    mSimTick          = aSimTick;
    mDynVarMgrIF      = aDynVarMgrIF;
    mFragmentOrdinal = aFragmentOrdinal;
  }

  void
  Prime( int aFragmentOrdinal )
  {
    mFragmentOrdinal = aFragmentOrdinal;

  }

  void
  DisplayForce( int aAbsSiteId )
  {
    mDynVarMgrIF->DisplayForceForSite( aAbsSiteId );
  }

  inline
  void
  AddForce( SiteId aAbsSiteId, int aShellId, const XYZ & aForce )
  {
    mDynVarMgrIF->AddForceForSite( aAbsSiteId, aForce );
  }

  inline
  XYZ
  GetForce( SiteId aAbsSiteId, int aShellId ) 
  {
    XYZ rc = mDynVarMgrIF->GetForceForSite( aAbsSiteId );
    return rc ; 
  }

  inline
  void
  GetForce( SiteId aAbsSiteId, int aShellId, XYZ& aForce ) 
  {
    mDynVarMgrIF->GetForceForSite( aAbsSiteId , aForce );
  }

  inline
  void
  GetForce( SiteId aAbsSiteId, int aShellId, double& aForceX, double& aForceY, double& aForceZ ) 
  {
    mDynVarMgrIF->GetForceForSite( aAbsSiteId , aForceX, aForceY, aForceZ );                
  }
                          
  inline
  double
  ForceX( SiteId aAbsSiteId, int aShellId ) 
  {
    return mDynVarMgrIF->ForceXForSite( aAbsSiteId );
  }

  inline
  double
  ForceY( SiteId aAbsSiteId, int aShellId ) 
  {
    return mDynVarMgrIF->ForceYForSite( aAbsSiteId );
  }
  inline
  double
  ForceZ( SiteId aAbsSiteId, int aShellId ) 
  {
    return mDynVarMgrIF->ForceZForSite( aAbsSiteId );
  }
  inline
  void
  SetForce( SiteId aAbsSiteId, int aShellId, XYZ aForce ) 
  {
    mDynVarMgrIF->SetForceForSite( aAbsSiteId, aForce );
  }
  void
  SetForce( SiteId aAbsSiteId, int aShellId, double aForceX, double aForceY, double aForceZ ) 
  {
    mDynVarMgrIF->SetForceForSite( aAbsSiteId, aForceX, aForceY, aForceZ );
  }
};

template <class DynVarMgrIF>
class FragmentPartialResultsIFCore0
{
public:
  int          mSimTick;
  int          mFragmentOrdinal;
  DynVarMgrIF *mDynVarMgrIF;

  FragmentPartialResultsIFCore0( int          aSimTick,
				 DynVarMgrIF *aDynVarMgrIF )
  {
    mSimTick          = aSimTick;
    mDynVarMgrIF      = aDynVarMgrIF;
  }

  FragmentPartialResultsIFCore0( int          aSimTick,
				 DynVarMgrIF *aDynVarMgrIF,
				 int aFragmentOrdinal 
				 )
  {
    mSimTick          = aSimTick;
    mDynVarMgrIF      = aDynVarMgrIF;
    mFragmentOrdinal = aFragmentOrdinal;
  }

  void
  Prime( int aFragmentOrdinal )
  {
    mFragmentOrdinal = aFragmentOrdinal;
  }

  void
  DisplayForce( int aAbsSiteId )
  {
    mDynVarMgrIF->DisplayForceForSite( aAbsSiteId );
  }

  inline
  void
  AddForce( SiteId aAbsSiteId, int aShellId, const XYZ & aForce )
  {
    mDynVarMgrIF->AddForceForSite( aAbsSiteId, aForce );
  }
  // TODO: tjcw axtra get and set methods ?
  inline
  double
  ForceX( SiteId aAbsSiteId, int aShellId ) 
  {
    return mDynVarMgrIF->ForceXForSite( aAbsSiteId );
              
  }
  inline
  double
  ForceY( SiteId aAbsSiteId, int aShellId ) 
  {
    return mDynVarMgrIF->ForceYForSite( aAbsSiteId );
              
  }
  inline
  double
  ForceZ( SiteId aAbsSiteId, int aShellId ) 
  {
    return mDynVarMgrIF->ForceZForSite( aAbsSiteId );
              
  }
              
  void
  SetForce( SiteId aAbsSiteId, int aShellId, double aForceX, double aForceY, double aForceZ ) 
  {
    mDynVarMgrIF->SetForceForSite( aAbsSiteId, aForceX, aForceY, aForceZ );
                
  }
};

template<class DynVarMgrIF, class IFP, class ORBManIF >
class NSQEngine
{
  typedef IFP IFP0;

public:

  //********* FROM NB ****************/

  struct GuardZoneReporting
  {
    unsigned int mSimTick;
    double       mCurrentVerletGuardZone;
    double       mDescendingSearchDelta;
    double       mOptRealSpaceTimeStep;
    double       mOptGuardZone;
    int          mOptTSCount;
    double       mGuardZoneTuningRealSpaceTimeSum;
    double       mGuardZoneTuningTimeStepsPerGuardZone;
    double       mRealSpacePerTS;
  };

  class ClientState
  {
  public:
    DynVarMgrIF *       mDynVarMgrIF;
    int                 mSimTick;

    int                 mReportForceOnSourceFlag;

    double              mOptGuardZone;
    double              mOptRealSpaceTimeStep;

    double              mDescendingSearchDelta;
    double              mGuardZoneTuningRealSpaceTimeSum;
    double              mTargetTmpGuardZone;

    int                 mOptTSCount;
    int                 mGuardZoneTuningTimeStepsPerGuardZone;
    int                 mFirstTime;
    int                 mCollectTimingData;

    GuardZoneReporting  mGuardZoneReporting[ 100 ];
    int                 mReportingIndex;
    int                 mReportOnce;

    FragmentInRangePair  mLocalFragments[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);
    FragId               mFragmentsInNeighborhood[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);
    int                  mFragmentsInNeighborhoodCount;
  };

  class SharedIntraCoreState
  {
  public:
    int      mStartOfOtherCoreInActiveList;
    int      mInvalidateActiveList;
    int      mPADDING[ 7 ];
  };


  // Every client port must allocate one of these structures.
  // It is returned as a handle by either Connect or send.
  // Calling wait shows that DynamicVariables are ready to be updated.
  class Client
  {
  public:
    class VerletStatistics
    {
    public:
#if defined(NSQ_KEEP_STATISTICS)
      enum {
	kBuckets =  ORBManIF::kInteractionKinds ,
	kTimesteps = 1024
      } ;
      int mQ[32] ;
      int mCurrentTimestep ;
      void Reset(void) 
      { 
	for ( int x=0;x<kBuckets;x+=1)
	  {
	    mQ[x]=0 ;
	  }
	mCurrentTimestep = 0 ;
      } 
      void Collect( 
		   FragmentVerletPair* FragmentVerletDB ,
		   int StartIndex,
		   int Length) 
      { 
	for ( int ActiveIndex = StartIndex;
	      ActiveIndex < Length;
	      ActiveIndex++ )
	  {

	    FragId SourceI;
	    FragId TargetI;
	    GetFragIdFromPairKey( FragmentVerletDB[ ActiveIndex ].mFragIdPairKey, TargetI, SourceI );
                  
	    unsigned int Bucket = ORBManIF::ModelInteractionKind(TargetI,SourceI) ;
	    mQ[Bucket] += 1 ;
	  } // endfor
      } 
      void Display( void ) 
      { 
	mCurrentTimestep += 1 ;
	if ( mCurrentTimestep == kTimesteps )
	  {
	    for ( int qr=0; qr<kBuckets; qr+=1 )
	      {
		BegLogLine(1)
		  << "Verlet list q[" << qr
		  << "] = " << mQ[qr]
		  << EndLogLine ;
	      }
	  }
      } 
#else
      void Reset(void) { } 
      void Collect( FragmentVerletPair* FragmentVerletDB, int StartIndex, int Length ) { } 
      void Display( void ) { } 
#endif        	
    } ;

    ClientState mClientState;

    SharedIntraCoreState mIntraCoreState MEMORY_ALIGN(5);

    /*************************************************************
     *  Below could move into mClientState
     *************************************************************/
    int                 mActiveVerletListCount;
    int*                mActiveVerletList;
    int                 mActiveVerletListAllocatedCount;
    double*             mActiveVerletListWeights;

    int                 mAssignmentListCount;
    FragAssignmentInfo* mAssignmentList;
    int                 mAssignmentListAllocatedCount;

    FragmentVerletPair* mFragmentVerletDBNext;
    FragmentVerletPair* mFragmentVerletDBPrev;

    int                 mNextDbIndex;

    FVerletList mN33VerletList ; // The 'Nonbond 3:3' frag verlet list as a special case
    SiteVerletList mSiteVerletList ; // The 'remaining site pairs' verlet list

    int mDBCacheMiss;
    int mDBDeletedEntries;
    int mDBCacheAccesses;

    int mAllocatedFragmentVerletDBCount;
    int mAddressSpaceId;

    ORBManIF * mORBManagerIF;
    /*************************************************************/

    void Reset( int aSimTick, RealSpaceSignalSet& aSignalSet )
    {
      // RESET the Verlet List Database --- just put the end terminator mark on the 0 element.
      assert( mAllocatedFragmentVerletDBCount > 0 );
      mFragmentVerletDBNext[ 0 ].mFragIdPairKey = GetFragIdPairKeyMax();
      mFragmentVerletDBPrev[ 0 ].mFragIdPairKey = GetFragIdPairKeyMax();
      aSignalSet.mUpdateFragmentVerletListSignal     = 1;
      aSignalSet.mUpdateFragmentAssignmentListSignal = 1;
      aSignalSet.mResetSignal = 0;
    }

    void ShutTimingDataCollection()
    {
      mClientState.mCollectTimingData = 0;
    }

#if 0
    void DisplayVerletStatistics(void)
    {
      static int InvocationCount = 0 ;
      if ( 0 == InvocationCount )
	{
	  int q[32] ;
	  for ( int qx=0; qx <= 31 ; qx += 1 )
	    {
	      q[qx]=0 ;
	    }
	  for ( int ActiveIndex = mStartIndex;
		ActiveIndex < mLength;
		ActiveIndex++ )
	    {
	      int ActivePairIndex = mActiveVerletList[ ActiveIndex ];
	      assert( ActivePairIndex >= 0 && ActivePairIndex < mAllocatedFragmentVerletDBCount );

	      FragId SourceI;
	      FragId TargetI;
	      GetFragIdFromPairKey( mFragmentVerletDBNext[ ActivePairIndex ].mFragIdPairKey, TargetI, SourceI );
	      int mlid1 = MSD_IF::GetMoleculeId( TargetI );
	      int mlid2 = MSD_IF::GetMoleculeId( SourceI );
	      int at1 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
	      int at2 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );

	      int same_mol = ( mlid1 == mlid2 ) ;
	      if (   at1 >= 1 && at1 <= 4
		     && at2 >= 1 && at2 <= 4
		     ) {
		q[same_mol * 16 + ( ( at1-1 ) * 4 ) + (at2-1)] += 1 ;
	      }
	    } // endfor

	  for ( int qr=0; qr<=31; qr+=1 )
	    {
	      BegLogLine(1)
		<< "Verlet list q[" << qr
		<< "] = " << q[qr]
		<< EndLogLine ;
	    }
	}
      InvocationCount += 1 ;

    }

#endif

    void
    Connect( DynVarMgrIF          * aDynVarMgrIF,   //Type for this should come in as a template interface
	     ORBManIF             * aORBManagerIF,
	     RealSpaceSignalSet&    aSignalSet,
	     unsigned               aSimTick,
	     int                    aIntegratorLevel )
    {
      assert( aIntegratorLevel == 0 );

      IFP0::Init();

      mClientState.mDynVarMgrIF              = aDynVarMgrIF;

      mClientState.mSimTick                  = aSimTick;

      aSignalSet.mUpdateFragmentVerletListSignal      = 1;
      aSignalSet.mUpdateFragmentAssignmentListSignal  = 1;
      aSignalSet.mResetSignal                         = 1;

      mClientState.mFirstTime                   = 0;
      mClientState.mCollectTimingData           = 1;

      mClientState.mOptGuardZone             = aDynVarMgrIF->GetCurrentVerletGuardZone();
      mClientState.mOptRealSpaceTimeStep     = MAX_REAL_SPACE_TIME_STEP_TIME;
      mClientState.mOptTSCount               = 0;
      mClientState.mDescendingSearchDelta    = 0.1;
      mClientState.mGuardZoneTuningTimeStepsPerGuardZone = 1;

      mClientState.mGuardZoneTuningRealSpaceTimeSum =  ( MAX_REAL_SPACE_TIME_STEP_TIME / Platform::Topology::GetAddressSpaceCount() ) - 1.0;

      mClientState.mReportingIndex                = 0;
      mClientState.mReportOnce                    = 1;

      mFragmentVerletDBNext           = NULL;
      mFragmentVerletDBPrev           = NULL;
      mAllocatedFragmentVerletDBCount = 0;

      mAddressSpaceId = Platform::Topology::GetAddressSpaceId();

      mActiveVerletList               = NULL;
      mActiveVerletListWeights        = NULL;
      mActiveVerletListCount          = 0;
      mActiveVerletListAllocatedCount = 0;

      mAssignmentListCount            = 0;
      mAssignmentList                 = NULL;
      mAssignmentListAllocatedCount   = 0;


      mORBManagerIF = aORBManagerIF; 

      // Since we need a terminator in the DB we need to do the first expand here and put the term in.
      ExpandFragmentVerletDB();
      ExpandAssignmentList();
      ExpandActiveVerletList();

      mFragmentVerletDBNext[ 0 ].mFragIdPairKey = GetFragIdPairKeyMax();
      mFragmentVerletDBPrev[ 0 ].mFragIdPairKey = GetFragIdPairKeyMax();
          
      mN33VerletList.Init() ; 
      mSiteVerletList.Init() ;

      BegLogLine( 0 )
	<< "NSQ::Connect() about to exit"
	<< EndLogLine;	  
    }

    void
    ExpandFragmentVerletDB()
    {
#ifdef PK_BGL
      if( Platform::Thread::GetId() == 1 )
	{
	  char* Abort = NULL;
	  Abort[ 0 ] = 0;
	}
#endif

      void * Memory;

      int NewCount =  8 * ( mAllocatedFragmentVerletDBCount + (128 * 1024) );

      int NewSize = NewCount * sizeof(FragmentVerletPair);

      Memory = malloc( NewSize );
      if( Memory == NULL )
	PLATFORM_ABORT( "ERROR:: Not enough memory for fragment verlet list: mFragmentVerletDB" );

      if( mFragmentVerletDBNext != NULL )
	{
	  memcpy( Memory, mFragmentVerletDBNext, (mAllocatedFragmentVerletDBCount * sizeof(FragmentVerletPair)) );
	  free( mFragmentVerletDBNext );
	}
      mFragmentVerletDBNext = ( FragmentVerletPair * ) Memory;

      Memory = malloc( NewSize );
      if( Memory == NULL )
	PLATFORM_ABORT( "ERROR:: Not enough memory for fragment verlet list: mFragmentVerletDB" );

      if( mFragmentVerletDBPrev != NULL )
	{
	  memcpy( Memory, mFragmentVerletDBPrev, (mAllocatedFragmentVerletDBCount * sizeof(FragmentVerletPair)) );
	  free( mFragmentVerletDBPrev );
	}
      mFragmentVerletDBPrev = ( FragmentVerletPair * ) Memory;

      BegLogLine(0)
	<< "ExpandFragmentVerletDB() "
	<< " Expand entries from "
	<<  mAllocatedFragmentVerletDBCount
	<< " to "
	<<  NewCount
	<< " sizeof( DB entry ) "
	<< sizeof( FragmentVerletPair )
	<< " size of whole db (bytes) "
	<< NewCount
	<< EndLogLine;

      mAllocatedFragmentVerletDBCount = NewCount ;
    }

    // Assigned list is a subset of pairs in the DB
    void
    ExpandAssignmentList()
    {
#ifdef PK_BGL
      if( Platform::Thread::GetId() == 1 )
	{
	  char* Abort = NULL;
	  Abort[ 0 ] = 0;
	}
#endif

      void * Memory;

      int NewCount = 8 * ( mAssignmentListAllocatedCount + ( 64 * 1024 ) );

      int NewSize = NewCount * sizeof( FragAssignmentInfo );

      Memory = malloc( NewSize );

      if( Memory == NULL )
	PLATFORM_ABORT( "ERROR:: Failed to malloc memory for mAssignmentList " );

      if( mAssignmentList != NULL );
      {
	memcpy( Memory, mAssignmentList, ( mAssignmentListAllocatedCount * sizeof( FragAssignmentInfo )) );
	free( mAssignmentList );
      }

      mAssignmentList = ( FragAssignmentInfo * ) Memory;

      BegLogLine( 0 )
	<< "ExpandAssignmentList() "
	<< "Expand entries from "
	<< mAssignmentListAllocatedCount
	<< " to "
	<< NewCount
	<< EndLogLine;

      mAssignmentListAllocatedCount = NewCount;
    }

    //
    void
    ExpandActiveVerletList()
    {
      BegLogLine( PKFXLOG_VERLET_LIST_DEBUG )
	<< "Entering: ExpandActiveVerletList: mActiveVerletListAllocatedCount: " << mActiveVerletListAllocatedCount
	<< EndLogLine;

#ifdef PK_BGL
      if( Platform::Thread::GetId() == 1 )
	{
	  char* Abort = NULL;
	  Abort[ 0 ] = 0;
	}
#endif

      void * Memory;
      void * Memory1;

      int NewCount = 12 * ( mActiveVerletListAllocatedCount + (32 * 1024) );

      int NewSize = NewCount * sizeof( int );

      int NewSize1 = NewCount * sizeof( double );

      Memory = malloc( NewSize );
      if( Memory == NULL )
	PLATFORM_ABORT( "ERROR:: Failed to malloc memory for mActiveVerletList " );

      Memory1 = malloc( NewSize1 );
      if( Memory1 == NULL )
	PLATFORM_ABORT( "ERROR:: Failed to malloc memory for mActiveVerletList " );

      if( mActiveVerletList != NULL )
	{
	  memcpy( Memory, mActiveVerletList, (mActiveVerletListAllocatedCount * sizeof( int )) );
	  free( mActiveVerletList );
	}

      if( mActiveVerletListWeights != NULL )
	{
	  memcpy( Memory1, mActiveVerletListWeights, (mActiveVerletListAllocatedCount * sizeof( double )) );
	  free( mActiveVerletListWeights );
	}

      mActiveVerletList = ( int * ) Memory;
      mActiveVerletListWeights = ( double * ) Memory1;

      BegLogLine( PKFXLOG_VERLET_LIST_DEBUG )
	<< "ExpandActiveVerletList()"
	<< "Expand entries from "
	<< mActiveVerletListAllocatedCount
	<< " to "
	<< NewCount
	<< EndLogLine;

      mActiveVerletListAllocatedCount = NewCount;
    }

    //****************************************************************
    typedef unsigned int FragIdPairKey;
			
    class FragVerletListElement
    {
    public:
      FragIdPairKey mFragPair ;
    } ;
			
    class FragVerletList
    {
    public: 
      FragVerletListElement * mVerletList ;
      unsigned int mCount ;
      unsigned int mLength ;
    } ;

    typedef FragmentPartialResultsIF<DynVarMgrIF> FragmentPartialResults_T;
    
    template <int HalfEnergy>
    class GlobalPartialResultsIF
    {
    public:
      
      double mEnergyByOrdinal[ IFP0::NsqUdfGroup0::ActiveUDFCount ];
      XYZ    mVirial;

      inline
      void
      ZeroEnergy()
      {
	for( int i=0; i < IFP0::NsqUdfGroup0::ActiveUDFCount; i++ )
	  {
	    mEnergyByOrdinal[ i ] = 0.0;
	  }
      }

      inline
      void
      ZeroVirial()
      {
	mVirial.Zero();
      }

      inline
      void
      Zero()
      {
	ZeroEnergy();
	ZeroVirial();
      }

      inline
      void
      AddEnergy( int aOrdinal, double aEnergy )
      {
	assert( aOrdinal >= 0 && aOrdinal < IFP0::NsqUdfGroup0::ActiveUDFCount );

	mEnergyByOrdinal[ aOrdinal ] += HalfEnergy ? 0.5 * aEnergy : aEnergy;
      }

      template <int ReportsEnergy>
      inline
      void
      AddEnergy( int aOrdinal, double aEnergy )
      {
	BegLogLine(PKFXLOG_NSQ_INFO)
	  << "AddEnergy<" << ReportsEnergy
	  << "> (" << aOrdinal
	  << "," << aEnergy
	  << ")"
	  << EndLogLine ;
	if( ReportsEnergy )
	  {
	    assert( aOrdinal >= 0 && aOrdinal < IFP0::NsqUdfGroup0::ActiveUDFCount );
	    
	    mEnergyByOrdinal[ aOrdinal ] += HalfEnergy ? 0.5 * aEnergy : aEnergy;
	  }
      }

      inline
      void
      AddVirial( XYZ aVirial )
      {
	mVirial += aVirial;
      }

      template <int ReportsVirial>
      inline
      void
      AddVirial( XYZ aVirial )
      {
	if (ReportsVirial)
	  {
	    mVirial += aVirial;
	  }
      }

      inline
      double
      GetEnergy( int aShellId )
      {
	return mEnergyByOrdinal[ aShellId ];
      }
    };

    class GlobalInputParametersIF
    {
    public:
      double                  mSwitchLowerCutoff;
      double                  mSwitchDelta;

      inline
      void
      Init( )
      {
	mSwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
	mSwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
      }
    };
    void SortActiveVerletList(void)
    {
      // Verlet lists of length 0 or 1 do not need sorting, and we
      // do not need the compiler's 'optimal special cases' ...
      if ( mActiveVerletListCount > 1 )
	{
	  unsigned int BucketMap[ORBManIF::kInteractionKinds] ;
	  unsigned int KindList[mActiveVerletListCount] ;
	  unsigned int ScratchVerletList[mActiveVerletListCount] ;
	  // Work out how many Verlet elements of each kind there are
	  for  ( int a=0;a<ORBManIF::kInteractionKinds;a+=1) 
	    {
	      BucketMap[a]=0 ;
	    }
	  for (int i=0;i<mActiveVerletListCount;i+=1)
	    {
	      int ActivePairIndex = mActiveVerletList[ i ];
	      assert( ActivePairIndex >= 0 && ActivePairIndex < mAllocatedFragmentVerletDBCount );
	
	      FragId SourceI;
	      FragId TargetI;
	
	      GetFragIdFromPairKey( mFragmentVerletDBNext[ ActivePairIndex ].mFragIdPairKey, TargetI, SourceI );
	      unsigned int Bucket = mORBManagerIF->ModelInteractionKind( TargetI, SourceI );
	      KindList[i] = Bucket ;
	      BucketMap[Bucket] += 1;               	
	    }
	  // Work out where to start the Verlet list elements of each kind
	  unsigned int BucketStart = 0 ;
	  for (int b=0;b<ORBManIF::kInteractionKinds;b+=1)
	    {
	      unsigned int NextBucketStart = BucketMap[b]+BucketStart ;
	      BucketMap[b] = BucketStart ;
	      BucketStart = NextBucketStart ;
	    }
	  // Generate the sorted Verlet list
	  for ( int j=0;j<mActiveVerletListCount;j+=1)
	    {
	      BegLogLine(PKFXLOG_SORT_VERLET)
		"Re-bucketing " << j
				<< " (bucket " << KindList[j]
				<< ") --> slot "
				<< BucketMap[KindList[j]] 
				<< EndLogLine ;
	      unsigned int Bucket = KindList[j] ;
	      ScratchVerletList[BucketMap[Bucket]] = mActiveVerletList[j] ;
	      BucketMap[Bucket] += 1 ;
	    }
	  // Copy it back to the real Verlet list
	  for ( int k=0;k<mActiveVerletListCount;k+=1)
	    {
	      BegLogLine(PKFXLOG_SORT_VERLET)
		<< "mActiveVerletList[" << k 
		<< "] was " << mActiveVerletList[k]
		<< " being changed to " << ScratchVerletList[k]
		<< EndLogLine ;
	      mActiveVerletList[k] = ScratchVerletList[k] ;
	    }
	}
    } 
          
    // Go throught the 'active Verlet list' to build the water and atom-atom
    // Verlet lists
    void CreateWaterSiteVerletLists( int aSimTick, double GuardRangeSqr)
    {
      BegLogLine( 0 )
	<< "Entering CreateWaterSiteVerletLists()" 
	<< EndLogLine ;

      // Count how long each list needs to be.
      // Generate a 'high' estimate of how long the site-site list needs to be,
      // a somewhat-smaller piece of it will be filled in due to exclusions
      unsigned int N33Count = 0 ; 
      unsigned int SiteSiteCount = 0 ; 
      for ( unsigned int a = 0 ; a < mActiveVerletListCount ; a += 1)
	{
	  int ActivePairIndex = mActiveVerletList[ a ];
	  assert( ActivePairIndex >= 0 && ActivePairIndex < mAllocatedFragmentVerletDBCount );
	  FragId SourceI;
	  FragId TargetI;
              
	  GetFragIdFromPairKey( mFragmentVerletDBNext[ ActivePairIndex ].mFragIdPairKey, TargetI, SourceI );
              
	  BegLogLine( 0 )
	    << aSimTick
	    << " FragPair " << TargetI << " " << SourceI
	    << EndLogLine;
              
	  int NumberOfSitesA = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
	  int NumberOfSitesB = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
	  int mlid1 = MSD_IF::GetMoleculeId( TargetI );
	  int mlid2 = MSD_IF::GetMoleculeId( SourceI );
	  ExclusionMask_T ExclusionMask = mFragmentVerletDBNext[ ActivePairIndex ].mExclusionMask;
              
	  if ( ( 0 == FORCE_ATOM_ATOM_FRAGS ) && ( 3 == NumberOfSitesA 
						   && 3 == NumberOfSitesB ) )
	    {   
	      if ( mlid1 != mlid2 )
		{ 
                  // 'water-water' (or possibly 3:3 in protein-water etc.)
                  N33Count += 1 ;
		} 
	      else 
		{
                  if ( 0 == ExclusionMask )
                    {
		      // 3:3 in same mol, but no exclusions
		      N33Count += 1 ;
                    } 
                  else 
                    {
		      // 3:3 in same mol with exclusions, e.g. adjacent frags
		      SiteSiteCount += 3*3 ;
                    } 
		} 
	    } 
	  else 
	    {
	      // Not a 3:3
	      SiteSiteCount += NumberOfSitesA * NumberOfSitesB ;
	    }              
	} 
          	
      BegLogLine(PKFXLOG_WATERSITE)
	<< "Nonbond 3:3 count=" << N33Count 
	<< " Remaining site:site count=" << SiteSiteCount 
	<< EndLogLine ;

      // Set up the Verlet lists
      mN33VerletList.SetCount(N33Count) ; 
      mSiteVerletList.SetCount(SiteSiteCount) ;
            
      unsigned int N33Index = 0 ; 
      unsigned int SiteSiteIndex = 0 ; 
            
      for ( unsigned int b=0 ; b<mActiveVerletListCount; b += 1)
	{
	  int ActivePairIndex = mActiveVerletList[ b ];
	  assert( ActivePairIndex >= 0 && ActivePairIndex < mAllocatedFragmentVerletDBCount );
	  FragId SourceI;
	  FragId TargetI;
              
	  GetFragIdFromPairKey( mFragmentVerletDBNext[ ActivePairIndex ].mFragIdPairKey, TargetI, SourceI );
              
	  BegLogLine( 0 )
	    << aSimTick
	    << " FragPair " << TargetI << " " << SourceI
	    << EndLogLine;
              
	  int NumberOfSitesA = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
	  int NumberOfSitesB = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
	  int mlid1 = MSD_IF::GetMoleculeId( TargetI );
	  int mlid2 = MSD_IF::GetMoleculeId( SourceI );
	  ExclusionMask_T ExclusionMask = mFragmentVerletDBNext[ ActivePairIndex ].mExclusionMask;
	  int FragmentImageVectorIndex = mFragmentVerletDBNext[ ActivePairIndex ].mFragmentImageVectorIndex;
              
	  XYZ ImageVector = mClientState.mDynVarMgrIF->mImageVectorTable[ FragmentImageVectorIndex ] ;
	  if ( ( 0 == FORCE_ATOM_ATOM_FRAGS ) && ( 3 == NumberOfSitesA 
						   && 3 == NumberOfSitesB ) )
	    {   
	      if ( mlid1 != mlid2 )
		{ 
                  // 'water-water' (or possibly 3:3 in protein-water etc.
                  mN33VerletList.SetFragPair(N33Index,SourceI,TargetI,FragmentImageVectorIndex,mClientState.mDynVarMgrIF) ;
                  N33Index += 1 ;
		} 
	      else 
		{
                  if ( 0 == ExclusionMask )
                    {
		      // 3:3 in same mol, but no exclusions
		      mN33VerletList.SetFragPair(N33Index,SourceI,TargetI,FragmentImageVectorIndex,mClientState.mDynVarMgrIF) ;
		      N33Index += 1 ;
                    } 
                  else 
                    {
		      // 3:3 in same mol with exclusions, e.g. adjacent frags
		      for ( unsigned int xA = 0 ; xA < 3 ; xA += 1)
			{
			  XYZ PosA = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( SourceI, xA );
                      
			  for ( unsigned int xB = 0 ; xB < 3 ; xB += 1 )
			    {
			      XYZ PosB = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( TargetI, xB );
			      double DistABSqr = NearestSquareDistanceInPeriodicVolume(PosA, PosB) ;
                        
			      unsigned int ExclusionBit = 1 << (3*xA + xB) ;
			      if ( (0 == (ExclusionMask & ExclusionBit)) && (DistABSqr<GuardRangeSqr) ) 
				{                          
				  mSiteVerletList.SetSitePair<DynVarMgrIF>(
									   SiteSiteIndex,
									   SourceI,xA,
									   TargetI,xB,
									   FragmentImageVectorIndex,
									   mClientState.mDynVarMgrIF
									   ) ;
				  SiteSiteIndex += 1 ;
				} 
			    } 
			}
                    } 
		} 
	    } 
	  else 
	    {
	      // Not a 3:3
	      if ( mlid1 != mlid2 || 0 == ExclusionMask )
		{
                  // But no exclusions
		  BegLogLine( 0 )
		    << aSimTick
                    << " About to SetSitePair " << TargetI << " " << SourceI
                    << EndLogLine;

                  for ( unsigned int xA = 0 ; xA < NumberOfSitesA ; xA += 1)
                    {
		      XYZ PosA = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( SourceI, xA );
		      for ( unsigned int xB = 0 ; xB < NumberOfSitesB ; xB += 1 )
			{
			  XYZ PosB = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( TargetI, xB );
			  double DistABSqr = NearestSquareDistanceInPeriodicVolume(PosA, PosB) ;

			  BegLogLine( 0 )
			    << aSimTick
			    << " About to SetSitePair " << TargetI << " " << SourceI
			    << " DistABSqr: " << DistABSqr
			    << " GuardRangeSqr: " << GuardRangeSqr
			    << " xA: " << xA
			    << " xB: " << xB
			    << " NumberOfSitesA: " << NumberOfSitesA
			    << " NumberOfSitesB: " << NumberOfSitesB
			    << " PosA: " << PosA
			    << " PosB: " << PosB
			    << EndLogLine;                      

			  if ( DistABSqr < GuardRangeSqr )
			    {
			      BegLogLine( 0 )
				<< aSimTick
				<< " About to SetSitePair " << TargetI << " " << SourceI
				<< EndLogLine;
                        
			      mSiteVerletList.SetSitePair<DynVarMgrIF>(	           	  	  	       
								       SiteSiteIndex,
								       SourceI,xA,
								       TargetI,xB,
								       FragmentImageVectorIndex,
								       mClientState.mDynVarMgrIF
								       ) ;
			      SiteSiteIndex += 1 ;
			    }
			} 
                    }
		}
	      else 
		{
                  // with exclusions
                  for ( unsigned int xA = 0 ; xA < NumberOfSitesA ; xA += 1)
                    {
		      XYZ PosA = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( SourceI, xA );
		      for ( unsigned int xB = 0 ; xB < NumberOfSitesB ; xB += 1 )
			{
			  XYZ PosB = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( TargetI, xB );
			  double DistABSqr = NearestSquareDistanceInPeriodicVolume(PosA, PosB) ;
			  unsigned int ExclusionBit = 1 << (NumberOfSitesB*xA + xB) ;
			  if ( (0 == (ExclusionMask & ExclusionBit)) && (DistABSqr<GuardRangeSqr) ) 
			    {                        
			      mSiteVerletList.SetSitePair<DynVarMgrIF>(
								       SiteSiteIndex,
								       SourceI,xA,
								       TargetI,xB,
								       FragmentImageVectorIndex,
								       mClientState.mDynVarMgrIF
								       ) ;
			      SiteSiteIndex += 1 ;
			    } 
			} 
                    }
		}
	    }
	}
          	
      // Set up the accurate count, having accounted for exclusions and out-of-guardzone
      mSiteVerletList.SetCount(SiteSiteIndex) ;
      assert(N33Index == N33Count) ; 
            
      BegLogLine(PKFXLOG_VECTOR_VERLET_COUNT)
	<< "SiteSiteIndex= " << SiteSiteIndex
	<< " " << N33Index << " " 
	<< EndLogLine ;
          	
      unsigned int ll = mSiteVerletList.GetCount() ;

      BegLogLine( 0 ) 
	<< "CreateWaterSiteVerletLists ll=" << ll
	<< EndLogLine;
            
      for( unsigned int v = 0; v < ll; v += 1 )
	{
	  BegLogLine(PKFXLOG_VECTOR_VERLET1)
	    // BegLogLine( 0 )
	    << "CreateWaterSiteVerletLists v=" << v
	    << " FragA= " << mSiteVerletList.GetFragA(v)
	    << " OffsetA= " << mSiteVerletList.GetOffsetA(v)
	    << " FragB= " << mSiteVerletList.GetFragB(v)
	    << " OffsetB= " << mSiteVerletList.GetOffsetB(v)
	    //    	        << " ImVX=" << mSiteVerletList.GetImageVectorTableIndex(v)
	    << EndLogLine;
              
	  int SiteIdA = MSD_IF::GetIrreducibleFragmentFirstSiteId( mSiteVerletList.GetFragA(v) )
	    + mSiteVerletList.GetOffsetA(v);
              
	  int SiteIdB = MSD_IF::GetIrreducibleFragmentFirstSiteId( mSiteVerletList.GetFragB( v ) )
	    + mSiteVerletList.GetOffsetB( v );
              
	  BegLogLine( 0 )
	    << aSimTick
	    << " FragA= " << mSiteVerletList.GetFragA(v)
	    << " OffsetA= " << mSiteVerletList.GetOffsetA(v)
	    << " FragB= " << mSiteVerletList.GetFragB(v)
	    << " OffsetB= " << mSiteVerletList.GetOffsetB(v)
	    << EndLogLine;
              
	  int Tmp1 = SiteIdA;
	  int Tmp2 = SiteIdB;
              
	  if( SiteIdA > SiteIdB )
	    {
	      Tmp1 = SiteIdB;
	      Tmp2 = SiteIdA;
	    }              
	}
            
      BegLogLine( 0 )
	<< "Leaving CreateWaterSiteVerletLists()" 
	<< EndLogLine ;
    }
          
    void
    CreateFragmentVerletList( int aSimTick )
    {
      BegLogLine( 0 )
	<< "Entering: CreateFragmentVerletList() "
	<< EndLogLine;

      mActiveVerletListCount = 0;

      double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
      double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );

      double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;

      double BoxTrace = mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mX +
	mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mY +
	mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mZ  ;

      double InRangeCutoff = ( SwitchUpperCutoff < BoxTrace ) ? SwitchUpperCutoff : BoxTrace ;

      double GuardZone = mClientState.mDynVarMgrIF->GetCurrentVerletGuardZone();

      double GuardRange = InRangeCutoff + GuardZone;
      double GuardRangeSqr = GuardRange * GuardRange;

      // Create a list of all centers and extents of fragments in neighborhood
      int FragmentNeighborhoodCount = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetFragmentNeighborListSize();

      BegLogLine( 0 )
	<< aSimTick
	<< " Got Here: "
	<< EndLogLine;

      for( int i = 0; i < FragmentNeighborhoodCount; i++ )
	{
	  FragId LocalFrag  = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetNthFragmentInNeighborhood( i );

	  mClientState.mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenter(
									    LocalFrag,
									    &mClientState.mLocalFragments[ LocalFrag ].mExtent,
									    &mClientState.mLocalFragments[ LocalFrag ].mFragmentCenter );

#ifndef NDEBUG
	  if( mClientState.mLocalFragments[ LocalFrag ].mExtent > mClientState.mDynVarMgrIF->mMaxExtent )
	    {
	      BegLogLine( 1 )
		<< "LocalFrag: " << LocalFrag
		<< " Extent: " << mClientState.mLocalFragments[ LocalFrag ].mExtent
		<< " MaxExtent: " << mClientState.mDynVarMgrIF->mMaxExtent
		<< EndLogLine;
	      PLATFORM_ABORT( "FragmentExtent > mMaxExtent" );
	    }
#endif
	}

      BegLogLine( 0 )
	<< aSimTick
	<< " Got Here: "
	<< EndLogLine;

      double Cutoff = InRangeCutoff + GuardZone;

      for( int i=0; i < mAssignmentListCount; i++ )
	{
	  // Get the pair, check the distance. If the pair is with in cutoff
	  unsigned int FragmentDatabaseIndex = mAssignmentList[ i ].mFragmentDatabaseIndex;

	  assert( FragmentDatabaseIndex >= 0 && FragmentDatabaseIndex < mAllocatedFragmentVerletDBCount );

	  FragId FragIdA;
	  FragId FragIdB;

	  GetFragIdFromPairKey( mFragmentVerletDBNext[ FragmentDatabaseIndex ].mFragIdPairKey,
				FragIdA,
				FragIdB );

	  assert( FragIdA >= 0 && FragIdA < NUMBER_OF_FRAGMENTS );
	  assert( FragIdB >= 0 && FragIdB < NUMBER_OF_FRAGMENTS );

	  BegLogLine( 0 )
	    << aSimTick
	    << " FragPair " << FragIdA << " " << FragIdB
	    << EndLogLine;

	  unsigned char FragmentImageVectorIndex = mFragmentVerletDBNext[ FragmentDatabaseIndex ].mFragmentImageVectorIndex;

	  assert( FragmentImageVectorIndex >=0 && FragmentImageVectorIndex < 125 );
	  XYZ*  FragmentImageVector      = & mClientState.mDynVarMgrIF->mImageVectorTable[ FragmentImageVectorIndex ];

	  double DistanceCutoff = Cutoff
	    + mClientState.mLocalFragments[ FragIdA ].mExtent
	    + mClientState.mLocalFragments[ FragIdB ].mExtent;

	  double DistanceCutoffSqr = DistanceCutoff * DistanceCutoff;

	  XYZ ImageVector = *FragmentImageVector;

	  XYZ SiteTagAImage  = mClientState.mLocalFragments[ FragIdA ].mFragmentCenter + ImageVector; // FragmentImageVector is negative

	  XYZ & FragmentCenterB = mClientState.mLocalFragments[ FragIdB ].mFragmentCenter;

	  ///*****  DEBUG LINES THAT GO HERE MOVED TO BOTTOM OF FILE

	  double FragCenterDistanceSquared = FragmentCenterB.DistanceSquared( SiteTagAImage );

	  if(  FragCenterDistanceSquared > DistanceCutoffSqr )
	    continue;

	  // This is based on insturmenting two systems for minimum reject distance between frag centers.
	  if(  FragCenterDistanceSquared < GuardRangeSqr )
	    goto GoToTarget_IsInRange_AddToActiveList;

	  { // This block is indented because the goto can see the local vars bypassed.

	    int FirstSiteIdA   = MSD_IF::GetIrreducibleFragmentFirstSiteId( FragIdA );
	    int NumberOfSitesA = MSD_IF::GetIrreducibleFragmentMemberSiteCount( FragIdA );

	    int FirstSiteIdB   = MSD_IF::GetIrreducibleFragmentFirstSiteId( FragIdB );
	    int NumberOfSitesB = MSD_IF::GetIrreducibleFragmentMemberSiteCount( FragIdB );

	    for( int offsetA=0; offsetA < NumberOfSitesA; offsetA++ )
	      {
		for( int offsetB=0; offsetB < NumberOfSitesB; offsetB++ )
		  {
                    XYZ PosA = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( FragIdA, offsetA );
                    XYZ PosB = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( FragIdB, offsetB );

                    XYZ PosAImaged   = PosA + ImageVector;
                    double DistABSqr = PosAImaged.DistanceSquared( PosB );

                    if( DistABSqr < GuardRangeSqr )
                      {
			goto GoToTarget_IsInRange_AddToActiveList;
                      }
		  }
	      }
	  } // end block for goto shielding

	  // HERE IS THE GOTO TARGET !!!!     To be in, a goto must have been executed
	  BegLogLine( 0 )
	    << aSimTick
	    << " FragPair " << FragIdA << " " << FragIdB
	    << EndLogLine;

	  continue;
	GoToTarget_IsInRange_AddToActiveList:


	  if( mActiveVerletListCount >= mActiveVerletListAllocatedCount )
	    {
	      BegLogLine( 0 )
		<< aSimTick
		<< " Got Here: "
		<< EndLogLine;
                
	      ExpandActiveVerletList();
	    }

	  BegLogLine( 0 )
	    << aSimTick
	    << " FragPair " << FragIdA << " " << FragIdB
	    << EndLogLine;

	  mActiveVerletList[ mActiveVerletListCount ] = FragmentDatabaseIndex;
	  mActiveVerletListCount++;
	}

      BegLogLine( 0 )
	<< aSimTick
	<< " mActiveVerletListCount: " << mActiveVerletListCount
	<< EndLogLine;

      CreateWaterSiteVerletLists( aSimTick, GuardRangeSqr) ;
#if defined(NSQ_SORT_VERLET_BY_KIND)
      // Sort the active verlet list by 'kind', so (e.g.) all the water-waters come together
      SortActiveVerletList() ;
#endif            

      BegLogLine( 0 )
	<< "Leaving: CreateFragmentVerletList() "
	<< EndLogLine;
    }

    void
    CreateFragmentAssignment( int aSimTick )
    {
      BegLogLine( 0 )
	<< "Entering: CreateFragmentAssignment() "
	<< EndLogLine;

      unsigned int IrreducibleFragmentCount = mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount() ;

      double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
      double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );

      double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;

      double BoxTrace = mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mX +
	mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mY +
	mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mZ  ;

      double InRangeCutoff = ( SwitchUpperCutoff < BoxTrace ) ? SwitchUpperCutoff : BoxTrace ;

      double GuardZone = mClientState.mDynVarMgrIF->GetCurrentVerletGuardZone();
      double AssignmentZone = mClientState.mDynVarMgrIF->GetCurrentAssignmentGuardZone();

      unsigned int FragmentPairIndex     = 0;

      mAssignmentListCount               = 0;

      mDBCacheMiss                       = 0;
      mDBDeletedEntries                  = 0;
      mDBCacheAccesses                   = 0;

      // Swap PrevDB with DB
      FragmentVerletPair * TempPtr       = mFragmentVerletDBPrev;
      mFragmentVerletDBPrev              = mFragmentVerletDBNext;
      mFragmentVerletDBNext              = TempPtr;

      double GuardRange                  = InRangeCutoff + GuardZone;
      double GuardRangeSqr               = GuardRange * GuardRange;

      double AssignedRange                  = InRangeCutoff + AssignmentZone;
      double AssignedRangeSqr               = AssignedRange * AssignedRange;

      int NumberOfPairsAssigned          = 0;

      FatCompVerletListGenStart.HitOE( PKTRACE_REAL_SPACE_FAT,
				       "RealSpaceFat",
				       mAddressSpaceId,
				       FatCompVerletListGenStart );

      int LocalFragmentsCount    = 0;

      mClientState.mFragmentsInNeighborhoodCount = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.mInteractionShortListSize;

      memcpy( mClientState.mFragmentsInNeighborhood,
	      mClientState.mDynVarMgrIF->mInteractionStateManagerIF.mInteractionShortList,
	      mClientState.mFragmentsInNeighborhoodCount * sizeof( FragId ) );


      // Sort local structure.
      mClientState.mDynVarMgrIF->mInteractionStateManagerIF.SortFragmentNeighborList( mClientState.mFragmentsInNeighborhood,
										      mClientState.mFragmentsInNeighborhoodCount );

      int FragmentNeighborhoodCount = mClientState.mFragmentsInNeighborhoodCount;
      for( int i = 0; i < FragmentNeighborhoodCount; i++ )
	{
	  FragId LocalFrag  = mClientState.mFragmentsInNeighborhood[ i ];
              
	  XYZ centerDummy;
              
	  mClientState.mDynVarMgrIF->GetImagedFragCenter(
							 LocalFrag,
							 centerDummy );

	  ORBNode* RankForFragLeaf = mClientState.mDynVarMgrIF->mRecBisTree->NodeFromSimCoord( centerDummy.mX, centerDummy.mY, centerDummy.mZ );
              
	  assert( RankForFragLeaf );
              
	  int RankForFrag = RankForFragLeaf->rank;
              
	  BegLogLine( 0 )
	    << "REG " << LocalFrag << " " << RankForFrag << " " << centerDummy
	    << EndLogLine;

	  assert( RankForFrag >= 0 && RankForFrag < Platform::Topology::GetAddressSpaceCount() );

	  int RankOrdinal = mClientState.mDynVarMgrIF->mSparseSkinManagerIF.mNodeIdToRecvNodeOrdinalMap[ RankForFrag ];

	  BegLogLine( PKFXLOG_NSQ_DEBUG )
	    << " LocalFrag: " << LocalFrag
	    << " RankForFrag: " << RankForFrag
	    << " RankOrdinal: " << RankOrdinal 
	    << EndLogLine;

	  if( RankOrdinal != -1 )
	    {
	      int VoxelIndex1 = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetVoxelIndex( LocalFrag );

	      assert( VoxelIndex1 >= 0 && VoxelIndex1 < mClientState.mDynVarMgrIF->mSparseSkinManagerIF.mMaxVoxelsInRange );
                
	      int VoxelOrdinal = mClientState.mDynVarMgrIF->mSparseSkinManagerIF.mVoxelIndexToVoxelOrdinalMap[ VoxelIndex1 ];
                
	      assert( VoxelOrdinal >=0 && VoxelOrdinal < mClientState.mDynVarMgrIF->mSparseSkinManagerIF.mVoxelsInSkinHoodCount );

	      BegLogLine( 0 )
		<< "REG " << LocalFrag << " " << RankForFrag 
		<<  " " << VoxelOrdinal << " " << RankOrdinal
		<< EndLogLine;

	      BegLogLine( PKFXLOG_NSQ_DEBUG )
		<< "LocalFragmentsCount: " << LocalFragmentsCount 
		<< " LocalFrag: " << LocalFrag 
		<< " RankOrdinal: " << RankOrdinal
		<< " VoxelIndex1: " << VoxelIndex1
		<< " VoxelOrdinal:" << VoxelOrdinal		
		<< EndLogLine;
	      
	      mClientState.mLocalFragments[ LocalFragmentsCount ].mFragId      = LocalFrag;
	      mClientState.mLocalFragments[ LocalFragmentsCount ].mRankOrdinal = RankOrdinal;
	      mClientState.mLocalFragments[ LocalFragmentsCount ].mVoxelOrdinal = VoxelOrdinal;
	      mClientState.mLocalFragments[ LocalFragmentsCount ].mMoleculeId  = MSD_IF::GetMoleculeId( LocalFrag );

	      LocalFragmentsCount++;
	    }
	}
      ////// END THIS BLOCK PROBABLY SHOULD NOT BE HERE //////


      BegLogLine( 0 )
	<< "CreateFragmentAssignment(): "
	<< " SimTick "                    << aSimTick
	<< " LocalFragmentsCount = "       << LocalFragmentsCount
	<< " FragmentNeighborhoodCount = " << FragmentNeighborhoodCount
	<< EndLogLine;

      /// assert( MAX_SITES_IN_FRAGMENT <= 4 );

      // Begin combinatorial scan of all pairs of local fragments.

      mNextDbIndex = 0;
      int PrevDbIndex = 0;

      int InteractionTotal = 0;
      int InteractionType[4];
      InteractionType[0] = 0;
      InteractionType[1] = 0;
      InteractionType[2] = 0;
      InteractionType[3] = 0;

      int DB_Evicted                   = 0;
      int DB_Inserted                  = 0;
      int Culled_InRangeAssigned       = 0;
      int Culled_InRangeNotAssigned    = 0;
      int Culled_NotInRangeNotAssigned = 0;
      int DistanceCheckPassed = 0;

      for( int fragA = 0; fragA < LocalFragmentsCount; fragA++ )
	{
	  FragId FragIdA         = mClientState.mLocalFragments[ fragA ].mFragId;
	  int    MoleculeIdA     = mClientState.mLocalFragments[ fragA ].mMoleculeId;

	  int RankOrdinalA       = mClientState.mLocalFragments[ fragA ].mRankOrdinal;

	  int    VoxelOrdinalA   = mClientState.mLocalFragments[ fragA ].mVoxelOrdinal;
	  int    VoxelBaseOffset = VoxelOrdinalA * mClientState.mDynVarMgrIF->mSparseSkinManagerIF.mVoxelsInSkinHoodCount;

	  // ATTENTION: fragA+1 assumes that self fragments are always excluded
	  for( int fragB = fragA+1; fragB < LocalFragmentsCount; fragB++ )
	    {
	      FragId FragIdB         = mClientState.mLocalFragments[ fragB ].mFragId;
	      // Get the node type index.

	      int RankOrdinalB     = mClientState.mLocalFragments[ fragB ].mRankOrdinal;

	      unsigned char VoxelInteractionType = mClientState.mDynVarMgrIF->mSparseSkinManagerIF.mAssignmentNodeTable[ RankOrdinalA ][ RankOrdinalB ];

	      BegLogLine( PKFXLOG_NSQ_DEBUG )
		<< "RankOrdinalA: " << RankOrdinalA
		<< " RankOrdinalB: " << RankOrdinalB
		<< " VoxelInteractionType: " << VoxelInteractionType
		<< EndLogLine;


	      BegLogLine( 0 )
		<< "REG " << FragIdA << " " << FragIdB
		<<  " " << VoxelInteractionType
		<< EndLogLine;

	      if( VoxelInteractionType == 0 )
		{
                  // Note that if this pair of frags is in the db ( because they came from diff voxels ) it will be left out.
                  Culled_NotInRangeNotAssigned++;
                  continue;
		}

	      int  VoxelOrdinalB = mClientState.mLocalFragments[ fragB ].mVoxelOrdinal;
                
	      unsigned int VoxelInteractionIndex1 = VoxelBaseOffset + VoxelOrdinalB;
	      unsigned char VoxelInteractionType1 =
		mClientState.mDynVarMgrIF->mSparseSkinManagerIF.mVoxelInteractionTypeTable.GetElement( VoxelInteractionIndex1 );

	      BegLogLine( PKFXLOG_NSQ_DEBUG )
		<< "REG " << FragIdA << " " << FragIdB
		<<  " " << VoxelOrdinalA << " " << VoxelOrdinalB << " " << VoxelInteractionType1
		<<  " " << RankOrdinalA << " " << RankOrdinalB
		<< EndLogLine;

	      if( VoxelInteractionType1 == 0 )
		{
                  continue;
		}

	      DistanceCheckPassed++;

	      unsigned char AssignedInteractionBit = VoxelInteractionType & 0x01;
	      unsigned char RangeBit               = VoxelInteractionType & 0x02;

	      InteractionTotal ++ ;
	      assert( VoxelInteractionType >= 0 && VoxelInteractionType < 4 );
	      InteractionType[ VoxelInteractionType ] ++ ;
                
#ifdef NSQ_MINIMIZE_PAIR_DB
	      // This block prevents trying to keep around nearby but not assigned DB entries - more imaging and exlc comps.
	      if( ( ! AssignedInteractionBit  ) && ( RangeBit ) )
		{
		  // Note that if this pair of frags is in the db ( because they came from diff voxels ) it will be left out.
		  Culled_InRangeNotAssigned++;
		  continue;
		}
#endif

	      int    MoleculeIdB   = mClientState.mLocalFragments[ fragB ].mMoleculeId;

	      BegLogLine( 0 )
		<< "NSQ::CreateFragmentAssignment:: "
		<< " FragPair= { " << FragIdA << " , " << FragIdB << " }"
		<< EndLogLine;

	      assert( FragIdA < FragIdB );

	      FragIdPairKey NewFragIdPairKey = MakeFragIdPairKey( FragIdA, FragIdB ); // A is "target", B is "source
	      BegLogLine( 0 )
		<< "REG1 " << FragIdA << " " << FragIdB
		<< EndLogLine;

	      // Scan the old DB, skipping over entries no longer relevant until finding the
	      // current pair or the slot it should go in.
	      // NOTE: Last entry will have the FragIdPairKeyMax  eg  0xFFFEFFFF

	      assert( PrevDbIndex >=0 && PrevDbIndex < mAllocatedFragmentVerletDBCount );
	      while( NewFragIdPairKey > mFragmentVerletDBPrev[ PrevDbIndex ].mFragIdPairKey )
		{
                  DB_Evicted++;
                  PrevDbIndex ++ ;
                  assert( PrevDbIndex >=0 && PrevDbIndex <= mAllocatedFragmentVerletDBCount );
		}

	      // If the entry is found, we're going to keep it because the
	      // AssignmentBit and/or the RangeBit is set.

	      int DBIndexForAssignmentList = -1;

	      assert( PrevDbIndex >=0 && PrevDbIndex < mAllocatedFragmentVerletDBCount );
	      if(  NewFragIdPairKey == mFragmentVerletDBPrev[ PrevDbIndex ].mFragIdPairKey )
		{
                  if( mNextDbIndex  + 1 >= mAllocatedFragmentVerletDBCount )
                    ExpandFragmentVerletDB();

                  assert( mNextDbIndex >=0 && mNextDbIndex < mAllocatedFragmentVerletDBCount );
                  mFragmentVerletDBNext[ mNextDbIndex ] = mFragmentVerletDBPrev[ PrevDbIndex ];

                  DBIndexForAssignmentList = mNextDbIndex;  // Save the index to use to add to AssignmentList if AssignmenBit is on.

                  mNextDbIndex ++ ;
                  PrevDbIndex ++ ;
		}
	      else // pair wasn't found in old DB -- add only if Assigned now.
		{
                  if( ! AssignedInteractionBit )
                    {
		      // Don't do anything if AssignmentInterationBit is not set - don't add to DB until actual assignment happens.
		      Culled_InRangeNotAssigned++;
                    }
                  else
                    {
		      Culled_InRangeAssigned++;
		      DB_Inserted++;
		      // Add this pair to the DB as it is being assigned for the first recent time and does not exist in the DB.

		      XYZ SiteTagBImageMultipliers ;

		      XYZ FragCenterA;
		      XYZ FragCenterB;

		      mClientState.mDynVarMgrIF->GetDynamicFragmentCenter( FragIdA, &FragCenterA );
		      mClientState.mDynVarMgrIF->GetDynamicFragmentCenter( FragIdB, &FragCenterB );

		      NearestImageInPeriodicVolumeMultiplier( FragCenterB, FragCenterA, SiteTagBImageMultipliers );

		      int ImageVectorTableIndex = mClientState.mDynVarMgrIF->GetIndexForImageMultiplierIn125ImageRange( SiteTagBImageMultipliers.mX,
															SiteTagBImageMultipliers.mY,
															SiteTagBImageMultipliers.mZ );

#ifndef MAX_FRAGID_DELTA_TO_HAVE_EXCLUSIONS
#define MAX_FRAGID_DELTA_TO_HAVE_EXCLUSIONS 999999999
#endif

		      ExclusionMask_T ExclMask = 0;
		      if(    ( MoleculeIdA == MoleculeIdB )
			     && ( abs( FragIdA - FragIdB ) <= MAX_FRAGID_DELTA_TO_HAVE_EXCLUSIONS ) )
			{
			  mClientState.mDynVarMgrIF->CreateExclusionMask( FragIdA, FragIdB, &ExclMask );
			}

		      if( mNextDbIndex  + 1 >= mAllocatedFragmentVerletDBCount )
			ExpandFragmentVerletDB();

		      assert( mNextDbIndex >=0 && mNextDbIndex < mAllocatedFragmentVerletDBCount );
		      mFragmentVerletDBNext[ mNextDbIndex ].mFragIdPairKey           = NewFragIdPairKey;

		      mFragmentVerletDBNext[ mNextDbIndex ].mExclusionMask           = ExclMask;
		      assert( mFragmentVerletDBNext[ mNextDbIndex ].mExclusionMask  == ExclMask ); // ensure enough bits in DB field

		      mFragmentVerletDBNext[ mNextDbIndex ].mFragmentImageVectorIndex = ImageVectorTableIndex;

		      DBIndexForAssignmentList = mNextDbIndex;  // Save the index to use to add to AssignmentList if AssignmenBit is on.

		      mNextDbIndex++;
                    }
		}

	      // Put any entries on the assigned interaction list
	      if( AssignedInteractionBit )
		{
                  if( mAssignmentListCount + 1 >= mAssignmentListAllocatedCount )
                    ExpandAssignmentList();

                  // Should check RangeBit in here and put on separate lists for switch, non switch
                  assert( mAssignmentListCount >=0 && mAssignmentListCount < mAssignmentListAllocatedCount );

                  BegLogLine( 0 )
                    << "REG2 " << FragIdA << " " << FragIdB
                    << EndLogLine;
                  
                  mAssignmentList[ mAssignmentListCount ].mFragmentDatabaseIndex = DBIndexForAssignmentList;
                  mAssignmentListCount ++ ;
		}
	    }
	}

      FatCompVerletListGenFinis.HitOE( PKTRACE_REAL_SPACE_FAT,
				       "RealSpaceFat",
				       mAddressSpaceId,
				       FatCompVerletListGenFinis );

      BegLogLine( 0 )
	<< "Assignment: "
	<< " SimTick "
	<< SimTick
	<< " DBEntryCount "
	<< mNextDbIndex
	<< " AssignmentListCount "
	<< mAssignmentListCount
	<< " HoodFragCnt "
	<< FragmentNeighborhoodCount
	<< " LocFragCnt "
	<< LocalFragmentsCount
	<< " InteractionTotal "
	<< InteractionTotal
	<< " IType[0] " <<  InteractionType[0]
	<< " IType[1] " <<  InteractionType[1]
	<< " IType[2] " <<  InteractionType[2]
	<< " IType[3] " <<  InteractionType[3]
	<< EndLogLine;

      BegLogLine( 0 )
	<< "Assignment: "
	<< " SimTick "
	<< SimTick
	<< " DBEntryCount "
	<< mNextDbIndex
	<< " AssignmentListCount "
	<< mAssignmentListCount
	<< " HoodFragCnt "
	<< FragmentNeighborhoodCount
	<< " LocFragCnt "
	<< LocalFragmentsCount
	<< " InteractionTotal "
	<< InteractionTotal
	<< " DistanceCheckPassed " 
	<< DistanceCheckPassed
	<< " DB_Evicted " <<  DB_Evicted
	<< " DB_Inserted " <<  DB_Inserted
	<< " Culled_InRangeAssigned " <<  Culled_InRangeAssigned
	<< " Culled_InRangeNotAssigned " <<  Culled_InRangeNotAssigned
	<< " Culled_NotInRangeNotAssigned " <<  Culled_NotInRangeNotAssigned
	<< EndLogLine;

      // Put int the terminator for when this is the Prev - terminator is essential to above function.
      assert( mNextDbIndex >=0 && mNextDbIndex < mAllocatedFragmentVerletDBCount );
      mFragmentVerletDBNext[ mNextDbIndex ].mFragIdPairKey = GetFragIdPairKeyMax();

      BegLogLine( 0 )
	<< "Leaving: CreateFragmentAssignment() "
	<< EndLogLine;
    }


    template<class FragPartResultsIF, class GlobPartResultIF>
    void RunActiveList( int aSimTick,
			int aStartIndex,
			int aLen,
			GlobPartResultIF& aGlobalPartialResultAxB )
    {
      BegLogLine( 0 )
	<< "RunActiveList aSimTick=" << aSimTick
	<< " aStartIndex=" << aStartIndex
	<< " aLen=" << aLen
	<< EndLogLine ;
            	
      GlobalInputParametersIF GlobalInputParameters;
      GlobalInputParameters.Init();

      NSQ_RealSpace_Meat_Start.HitOE( PKTRACE_REAL_SPACE_MEAT,
				      "RealSpaceMeat",
				      Platform::Topology::GetAddressSpaceId(),
				      NSQ_RealSpace_Meat_Start );

      NSQStat.MeatStart() ; 

      unsigned int ll = mSiteVerletList.GetCount() ;

      IFP0::ExecuteSiteVerletList<GlobalInputParametersIF,DynVarMgrIF,FragPartResultsIF,GlobPartResultIF>(aSimTick,
													  GlobalInputParameters,
													  mClientState.mDynVarMgrIF,
													  aGlobalPartialResultAxB,
													  mSiteVerletList) ; 

      if ( 0 == FORCE_ATOM_ATOM_FRAGS )
	{
	  IFP0::ExecuteN33VerletList<GlobalInputParametersIF,DynVarMgrIF,FragPartResultsIF,GlobPartResultIF>(aSimTick,
													     GlobalInputParameters,
													     mClientState.mDynVarMgrIF,
													     aGlobalPartialResultAxB,
													     mN33VerletList) ;
	}
                        
      NSQStat.MeatEnd() ; 
            
      NSQ_RealSpace_Meat_Finis.HitOE( PKTRACE_REAL_SPACE_MEAT,
				      "RealSpaceMeat",
				      Platform::Topology::GetAddressSpaceId(),
				      NSQ_RealSpace_Meat_Finis );
    }
            

    void
    Send( int                 aSimTick,
	  RealSpaceSignalSet& aSignalSet )
      __attribute__((noinline)) ;

    void
    Wait( int aSimTick )
    {
      // Loops better be done, no?
      return ;
    }
  };
};
template<class DynVarMgrIF, class IFP, class ORBManIF >
void
NSQEngine<DynVarMgrIF,IFP,ORBManIF>::Client::Send( 
						  int                 aSimTick, 
						  RealSpaceSignalSet& aSignalSet )
  __attribute__((noinline)) 
{
#pragma execution_frequency(very_low)
    
  EmitTimeStamp( aSimTick, Tag::RealSpace_Begin, 1 );
    
  BegLogLine( 0 )
    << "Send(): "
    << " SimTick "
    << aSimTick
    << EndLogLine;
    
  NSQStat.Entry() ;
  GlobalPartialResultsIF<0> GlobalPartialResultAxB;
  GlobalPartialResultAxB.Zero();
    
#if BLUEMATTER_STATS0
  IFP0::IFPHelper0::ClearStaticCounters();
#endif
    
  if( aSignalSet.mResetSignal )
    {
      Reset( aSimTick, aSignalSet );
    }
    
  /*
   * Create the fragment verlet list.
   */
  if( aSignalSet.mUpdateFragmentAssignmentListSignal )
    {
      CreateFragmentAssignment( aSimTick );
      aSignalSet.mUpdateFragmentAssignmentListSignal  = 0;
      aSignalSet.mUpdateFragmentVerletListSignal      = 1;
    }

  if( aSignalSet.mUpdateFragmentVerletListSignal )
    {
      CreateFragmentVerletList( aSimTick );
      aSignalSet.mUpdateFragmentVerletListSignal = 0;
    }

  int StartIndex = 0;
  int Length     = mActiveVerletListCount;

  BegLogLine( 0 )
    << "aSimTick: " << aSimTick
    << " mActiveVerletList= " << mActiveVerletList
    << " mActiveVerletListCount= " << mActiveVerletListCount
    << " mFragmentVerletDBNext= " << mFragmentVerletDBNext
    << " StartIndex= " << StartIndex
    << " Length= " << Length
    << EndLogLine;
	              	   
  RunActiveList<FragmentPartialResultsIF<DynVarMgrIF> >( aSimTick, StartIndex, Length,
							 GlobalPartialResultAxB );
	   
  BegLogLine(PKFXLOG_NSQ_INFO) 
    << "NSQEngine::Client::Send energies "
    << GlobalPartialResultAxB.GetEnergy( 0 )
    << " " << GlobalPartialResultAxB.GetEnergy( 1 )
    << " " << GlobalPartialResultAxB.GetEnergy( 2 )
    << EndLogLine ;
             
  BegLogLine( 0 )
    << "Chasing a hang..."
    << EndLogLine;

  // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
  unsigned TimeStep = aSimTick / MSD::SimTicksPerTimeStep;
  EmitTimeStamp(aSimTick, Tag::RealSpace_End, 1 );

#if MSDDEF_ReplicaExchange
  double Energy =
    IFP0::NsqUdfGroup0::UDF0_Bound::UDF::ReportsEnergy ? GlobalPartialResultAxB.GetEnergy( 0 ) : 0 +
    IFP0::NsqUdfGroup0::UDF1_Bound::UDF::ReportsEnergy ? GlobalPartialResultAxB.GetEnergy( 1 ) : 0 +
    IFP0::NsqUdfGroup0::UDF2_Bound::UDF::ReportsEnergy ? GlobalPartialResultAxB.GetEnergy( 2 ) : 0;

  mClientState.mDynVarMgrIF->AddToPotentialEnergy( aSimTick, Energy );
#endif

  // Check if this particular SimTick is the new TimeStep.
  if( TimeStep * MSD::SimTicksPerTimeStep == aSimTick )
    {
      BegLogLine( PKFXLOG_NSQLOOP_EMIT )
	<< "NSQLoops::Send() "
	<< " Emitting RDG Energy packets "
	<< " SimTick " << aSimTick
	<< " TimeStep " << TimeStep
	<< EndLogLine;

      if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
	{
	  if( IFP0::NsqUdfGroup0::UDF0_Bound::UDF::ReportsEnergy )
	    {
	      double Energy =  GlobalPartialResultAxB.GetEnergy( 0 );
	      EmitEnergy( TimeStep, IFP0::NsqUdfGroup0::UDF0_Bound::UDF::Code, Energy, 0 );
	    }

	  if( IFP0::NsqUdfGroup0::UDF1_Bound::UDF::ReportsEnergy )
	    {
	      double Energy =  GlobalPartialResultAxB.GetEnergy( 1 );
	      EmitEnergy( TimeStep, IFP0::NsqUdfGroup0::UDF1_Bound::UDF::Code, Energy, 0 );
	    }

	  if( IFP0::NsqUdfGroup0::UDF2_Bound::UDF::ReportsEnergy )
	    {
	      double Energy =  GlobalPartialResultAxB.GetEnergy( 2 );
	      EmitEnergy( TimeStep, IFP0::NsqUdfGroup0::UDF2_Bound::UDF::Code, Energy, 0 );
	    }
	}
    }

#if MSDDEF_DoPressureControl
  mClientState.mDynVarMgrIF->AddVirial( GlobalPartialResult.mVirial );
#endif
#if defined(NSQ_KEEP_STATISTICS)
  // DisplayVerletStatistics() ;
#endif
  NSQStat.Exit() ;

  BegLogLine( 0 )
    << "Chasing a hang..."
    << EndLogLine;

}

template<class DynVarMgrIF, class IFP, class ORBManIF >
class NSQAnchor
{
  /*
   * The anchor IS the Server.
   */
private:

  class NSQEngine< DynVarMgrIF, IFP, ORBManIF > mServer;

  /*
   *
   */
public:

  void
  Init()
  {
    //NEED: THIS SHOULD BE SOMEWHERE ELSE
    BegLogLine( PKFXLOG_NSQ_INIT )
      << "NSQAnchor::Init(): "
      << EndLogLine;
  }

  typedef class NSQEngine< DynVarMgrIF, IFP, ORBManIF >::Client ClientIF;

  ClientIF &
  GetClientIF()
  {
    NSQEngine< DynVarMgrIF, IFP, ORBManIF >::Client * cr;
    pkNew( &cr, 1, __FILE__, __LINE__ );
    return( * cr  );
  }
};

#endif
              
