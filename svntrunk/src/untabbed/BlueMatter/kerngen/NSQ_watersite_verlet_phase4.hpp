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
 
#ifndef __NSQ_WATERSITE_VERLET_VERLET_PHASE4_HPP__
#define __NSQ_WATERSITE_VERLET_PHASE4_HPP__

#ifndef PKFXLOG_VECTOR_VERLET_COUNT
#define PKFXLOG_VECTOR_VERLET_COUNT (0)
#endif
#ifndef PKFXLOG_WATERSITE
#define PKFXLOG_WATERSITE ( 0 ) 
#endif

#ifndef PKFXLOG_PLIMPTONLOOPS
#define PKFXLOG_PLIMPTONLOOPS ( 0 )
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

#ifndef BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
#define BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT 0
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

#ifndef FORCE_ATOM_ATOM_FRAGS
#define FORCE_ATOM_ATOM_FRAGS ( 0 )
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
      mNonMeatStat.Observe(mMeatStartTime-mEntryTime) ;
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
#define RELEASE_LOCK 20

template <class DynVarMgrIF>
        class FragmentPartialResultsIF
          {
          public:

            int          mSimTick;
            int          mFragmentOrdinal;
            DynVarMgrIF *mDynVarMgrIF;

            #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
            int          mNullForceInteractionCount;
            #endif

            FragmentPartialResultsIF( int          aSimTick,
                                      DynVarMgrIF *aDynVarMgrIF ,
                                      int          aFragmentOrdinal )
              {
              mSimTick          = aSimTick;
              mDynVarMgrIF      = aDynVarMgrIF;
              mFragmentOrdinal = aFragmentOrdinal;

              #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
                mNullForceInteractionCount = 0;
              #endif
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
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                mDynVarMgrIF->AddForceForRealSpaceSite( aAbsSiteId, aForce );
              #else
                mDynVarMgrIF->AddForceForSite( aAbsSiteId, aForce );

//                 BegLogLine( mSimTick == 0 )
//                   << "SiteId: " << aAbsSiteId
//                   << " F: " << aForce
//                   << EndLogLine;

              #endif
              
              #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
                if( aForce == XYZ::ZERO_VALUE() )
                  mNullForceInteractionCount++;
              #endif
              }
            inline
            XYZ
            GetForce( SiteId aAbsSiteId, int aShellId ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                XYZ rc = mDynVarMgrIF->GetForceForRealSpaceSite( aAbsSiteId );
              #else
                XYZ rc = mDynVarMgrIF->GetForceForSite( aAbsSiteId );


              #endif
                
                return rc ; 
              }
            inline
            void
            GetForce( SiteId aAbsSiteId, int aShellId, XYZ& aForce ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                mDynVarMgrIF->GetForceForRealSpaceSite( aAbsSiteId, aForce );
              #else
                mDynVarMgrIF->GetForceForSite( aAbsSiteId , aForce );


              #endif
                
              }
            inline
            void
            GetForce( SiteId aAbsSiteId, int aShellId, double& aForceX, double& aForceY, double& aForceZ ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                mDynVarMgrIF->GetForceForRealSpaceSite( aAbsSiteId, aForceX, aForceY, aForceZ );
              #else
                mDynVarMgrIF->GetForceForSite( aAbsSiteId , aForceX, aForceY, aForceZ );


              #endif
                
              }
              
//            inline double* AddressForce(SiteId aAbsSiteId )
//            {
//             #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
//                return mDynVarMgrIF->AddressForceForRealSpaceSite( aAbsSiteId );
//              #else
//                return mDynVarMgrIF->AddressForceForSite( aAbsSiteId );
//              #endif
//            }
            
            inline
            double
            ForceX( SiteId aAbsSiteId, int aShellId ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                return mDynVarMgrIF->ForceXForRealSpaceSite( aAbsSiteId );
              #else
                return mDynVarMgrIF->ForceXForSite( aAbsSiteId );
              #endif
              
              }
            inline
            double
            ForceY( SiteId aAbsSiteId, int aShellId ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                return mDynVarMgrIF->ForceYForRealSpaceSite( aAbsSiteId );
              #else
                return mDynVarMgrIF->ForceYForSite( aAbsSiteId );
              #endif
              
              }
            inline
            double
            ForceZ( SiteId aAbsSiteId, int aShellId ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                return mDynVarMgrIF->ForceZForRealSpaceSite( aAbsSiteId );
              #else
                return mDynVarMgrIF->ForceZForSite( aAbsSiteId );
              #endif
              
              }
            inline
            void
            SetForce( SiteId aAbsSiteId, int aShellId, XYZ aForce ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                mDynVarMgrIF->SetForceForRealSpaceSite( aAbsSiteId, aForce ) ; 
              #else
                mDynVarMgrIF->SetForceForSite( aAbsSiteId, aForce );


              #endif
                
              }
            void
            SetForce( SiteId aAbsSiteId, int aShellId, double aForceX, double aForceY, double aForceZ ) 
              {
              #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
                mDynVarMgrIF->SetForceForRealSpaceSite( aAbsSiteId, aForceX, aForceY, aForceZ ) ; 
              #else
                mDynVarMgrIF->SetForceForSite( aAbsSiteId, aForceX, aForceY, aForceZ );


              #endif
                
              }


          };

template<class DynVarMgrIF>
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

            // work around ERRATA
            ~IrreducibleFragmentOperandIF()
            {
              mSimTick = 0 ; 
              mFragmentOrdinal = 0 ;
              mDynVarMgrIF = NULL ;
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
              // int base   = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
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
              
//            inline double* AddressPosition(int aIndex) 
//            {
//            	return mDynVarMgrIF->mInteractionStateManagerIF.AddressPosition(mFragmentOrdinal, aIndex ) ;
//            }
            
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
  
//	double* AddressElAPosition(unsigned int aIndex) const
//	{
//		assert(aIndex < mCount) ;
//		return mVerletList[aIndex].mElAPosition ;
//	}
//	
//	double* AddressElBPosition(unsigned int aIndex) const
//	{
//		assert(aIndex < mCount) ;
//		return mVerletList[aIndex].mElBPosition ;
//	}
//	
//	double* AddressElAForce(unsigned int aIndex) const
//	{
//		assert(aIndex < mCount) ;
//		return mVerletList[aIndex].mElAPosition ;
//	}
//	
//	double* AddressElBForce(unsigned int aIndex) const
//	{
//		assert(aIndex < mCount) ;
//		return mVerletList[aIndex].mElBPosition ;
//	}
  
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
  } 
  
  void Term(void)
  {
    if ( mLength > 0 ) free(mVerletList) ;
    mCount = 0 ; 
    mLength = 0 ; 
  }
  
  void SetLength(unsigned int aLength)
  {
    if ( aLength > mLength )
    {
      void * Memory = malloc(aLength*sizeof(VerletListElement)) ;

          if( Memory == NULL )
              PLATFORM_ABORT( "ERROR:: Failed to malloc memory for mVerletList " );

          if ( mLength > 0 ) 
          {
             memcpy(Memory,mVerletList,mLength*sizeof(VerletListElement)) ;
             free(mVerletList) ;
          } 
          mVerletList = (VerletListElement *) Memory ;
    BegLogLine(PKFXLOG_VECTOR_VERLET) 
      << "SetLength from " << mLength
      << " to " << aLength
      << " list at " << Memory
      << EndLogLine ;
    mLength = aLength ;
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
  
  void dcache_store(void) const
  {
#if defined( DO_REAL_SPACE_ON_SECOND_CORE_VNM )

     // Push out the Verlet list
       rts_dcache_store( GET_BEGIN_PTR_ALIGNED( mVerletList ),
                           GET_END_PTR_ALIGNED( mVerletList + mCount) );

       // and push out the address and count
       rts_dcache_store( GET_BEGIN_PTR_ALIGNED( this ),
                         GET_END_PTR_ALIGNED( this + 1) );

#endif            
    
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
//	double* mElAPosition ;
//	double* mElBPosition ;
//	double* mElAForce ;
//	double* mElBForce ;
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
//        IrreducibleFragmentOperandIF<DynVarMgrIF>  FragmentOperandIFA( -1, aDynVarMgrIF, aFragA );
//        IrreducibleFragmentOperandIF<DynVarMgrIF>  FragmentOperandIFB( -1, aDynVarMgrIF, aFragB );
//		mVerletList[aIndex].mElAPosition = FragmentOperandIFA.AddressPosition(aOffsetA) ;
//		mVerletList[aIndex].mElBPosition = FragmentOperandIFB.AddressPosition(aOffsetB) ;
//		
//        FragmentPartialResultsIF<DynVarMgrIF>      FragmentPartialResultsIFA ( -1, aDynVarMgrIF, aFragA );
//        FragmentPartialResultsIF<DynVarMgrIF>      FragmentPartialResultsIFB ( -1, aDynVarMgrIF, aFragB );
//		
//		mVerletList[aIndex].mElAForce = FragmentPartialResultsIFA.AddressForce(aOffsetA) ;
//		mVerletList[aIndex].mElBForce = FragmentPartialResultsIFB.AddressForce(aOffsetB) ;
    
    
    }							 
  
} ; 
// typedef class VerletList<SiteVerletListElement> SiteVerletList ;

class FVerletListElement
{
public:
    unsigned int mElA ;
    unsigned int mElB ;	
    unsigned int mImageVectorTableIndex ;
//	float mImageMultiplierX ;
//	float mImageMultiplierY ;
//	float mImageMultiplierZ ;
//	double* mElAPosition ;
//	double* mElBPosition ;
//	double* mElAForce ;
//	double* mElBForce ;
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
//		mVerletList[aIndex].mImageMultiplierX = aDynVarMgrIF->GetImageMultiplierX(aImageVectorTableIndex) ;
//		mVerletList[aIndex].mImageMultiplierY = aDynVarMgrIF->GetImageMultiplierY(aImageVectorTableIndex) ;
//		mVerletList[aIndex].mImageMultiplierZ = aDynVarMgrIF->GetImageMultiplierZ(aImageVectorTableIndex) ;
  }

  
} ; 
// typedef class  VerletList<FVerletListElement> FVerletList ;

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
  #if 0 ///  Putting excl mask in bit field drops 1/3 off DB entry size
    ExclusionMask_T   mExclusionMask;
    unsigned short    mFragmentImageVectorIndex; // 0 -> 125 range
  #else
    unsigned int mExclusionMask            : 25 ;
    unsigned int mFragmentImageVectorIndex :  7 ;
  #endif
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
  unsigned char mTuneGuardZoneSignal;
  };

static InteractionStateManagerIF TempISM  MEMORY_ALIGN(5);

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

//         int                 mUpdateFragmentVerletListSignal;
//         int                 mUpdateNeighborhoodViolationSignal;

        int                 mReportForceOnSourceFlag;

        double              mOptGuardZone;
        double              mOptRealSpaceTimeStep;

        double              mDescendingSearchDelta;
        double              mGuardZoneTuningRealSpaceTimeSum;
        double              mTargetTmpGuardZone;

        int                 mOptTSCount;
        int                 mGuardZoneTuningTimeStepsPerGuardZone;
      // int                 mTuneGuardZone;
        int                 mFirstTime;
        int                 mCollectTimingData;          


        GuardZoneReporting  mGuardZoneReporting[ 100 ];
        int                 mReportingIndex;
        int                 mReportOnce;
          
        FragmentInRangePair  mLocalFragments[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);  
        FragId               mFragmentsInNeighborhood[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);  
        int                  mFragmentsInNeighborhoodCount;   
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


      /*************************************************************
       *  Below could move into mClientState
       *************************************************************/

        int                 mStartIndex;
        int                 mLength;
          
        int                 mDualCoreRealSpaceMode;
        int                 mActiveVerletListCount;
        int*                mActiveVerletList;
        int                 mActiveVerletListAllocatedCount;
        double*             mActiveVerletListWeights;

        int                 mAssignmentListCount;
        FragAssignmentInfo* mAssignmentList;
        int                 mAssignmentListAllocatedCount;

        FragmentVerletPair* mFragmentVerletDBNext;
        FragmentVerletPair* mFragmentVerletDBPrev;
        
        FVerletList mN33VerletList ; // The 'Nonbond 3:3' frag verlet list as a special case
        SiteVerletList mSiteVerletList ; // The 'remaining site pairs' verlet list

        int mDBCacheMiss;
        int mDBDeletedEntries;
        int mDBCacheAccesses;

        int mAllocatedFragmentVerletDBCount;
        int mAddressSpaceId;

        ORBManIF* mORBManagerIF;
      VerletStatistics mVerletStatistics ;
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
          mVerletStatistics.Reset() ;
          }

        void ShutTimingDataCollection()
          {
          mClientState.mCollectTimingData = 0;
          }

        void SetTuneGuardZoneSignal( int signal )
          {
          #if !MSDDEF_DoPressureControl
            mClientState.mTuneGuardZone = signal;
          #endif
          }
      

        void ReportGuardZone()
          {
#if 0
          if( ( mClientState.mReportingIndex != 0 ) && mClientState.mReportOnce && !mClientState.mTuneGuardZone )
            {
            mClientState.mReportOnce=0;

            if( Platform::Topology::GetAddressSpaceId() == 0 )
              {
              for( int i=1; i < mClientState.mReportingIndex; i++ )
                {
                BegLogLine( PKFXLOG_NSQ_GUARD_ZONE_TUNE )
                  << " Auto Tuning the guard zone. "
                  << " SimTick= "                              << mClientState.mGuardZoneReporting[ i ].mSimTick
                  << " CurrentGuardZone= "                     << mClientState.mGuardZoneReporting[ i-1 ].mCurrentVerletGuardZone
                  << " DescendingSearchDelta= "                << mClientState.mGuardZoneReporting[ i ].mDescendingSearchDelta
                  << " OptRealSpaceTimeStep= "                 << mClientState.mGuardZoneReporting[ i ].mOptRealSpaceTimeStep
                  << " OptGuardZone= "                         << mClientState.mGuardZoneReporting[ i ].mOptGuardZone
                  << " OptTSCount= "                           << mClientState.mGuardZoneReporting[ i ].mOptTSCount
                  << " GuardZoneTuningRealSpaceTimeSum= "      << mClientState.mGuardZoneReporting[ i ].mGuardZoneTuningRealSpaceTimeSum
                  << " GuardZoneTuningTimeStepsPerGuardZone= " << mClientState.mGuardZoneReporting[ i ].mGuardZoneTuningTimeStepsPerGuardZone
                  << " RealSpacePerTS= "                       << mClientState.mGuardZoneReporting[ i ].mRealSpacePerTS
                  << EndLogLine;
                }
              }
            }
#endif
          }

        void TuneGuardZone( int aSimTick )
          {
#if 0
          if( mClientState.mTuneGuardZone )
            {
            double GuardZone           = mClientState.mDynVarMgrIF->GetCurrentVerletGuardZone();

             #ifdef PK_PARALLEL
               Platform::Collective::FP_AllReduce_Sum( &mClientState.mGuardZoneTuningRealSpaceTimeSum, &mClientState.mTargetTmpGuardZone, 1 );
               mClientState.mGuardZoneTuningRealSpaceTimeSum = mClientState.mTargetTmpGuardZone;
             #endif

            double RealSpacePerTS = mClientState.mGuardZoneTuningRealSpaceTimeSum / mClientState.mGuardZoneTuningTimeStepsPerGuardZone;

            if( RealSpacePerTS < mClientState.mOptRealSpaceTimeStep )
              {
              mClientState.mOptGuardZone = GuardZone;
              mClientState.mOptTSCount   = mClientState.mGuardZoneTuningTimeStepsPerGuardZone;
              mClientState.mOptRealSpaceTimeStep = RealSpacePerTS;
              }

            if( ( GuardZone - mClientState.mDescendingSearchDelta ) >= -1.0*EPS )
              {
              mClientState.mDynVarMgrIF->SetCurrentVerletGuardZone( GuardZone - mClientState.mDescendingSearchDelta );
              }
            else
              {
              mClientState.mTuneGuardZone = 0;
              mClientState.mDynVarMgrIF->SetCurrentVerletGuardZone( mClientState.mOptGuardZone );
              }

            if( Platform::Topology::GetAddressSpaceId() == 0 )
              {
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mSimTick = aSimTick;
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mCurrentVerletGuardZone = mClientState.mDynVarMgrIF->GetCurrentVerletGuardZone();
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mDescendingSearchDelta = mClientState.mDescendingSearchDelta;
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mOptRealSpaceTimeStep = mClientState.mOptRealSpaceTimeStep;
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mOptGuardZone = mClientState.mOptGuardZone;
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mOptTSCount = mClientState.mOptTSCount;
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mGuardZoneTuningRealSpaceTimeSum = mClientState.mGuardZoneTuningRealSpaceTimeSum;
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mGuardZoneTuningTimeStepsPerGuardZone = mClientState.mGuardZoneTuningTimeStepsPerGuardZone;
              mClientState.mGuardZoneReporting[ mClientState.mReportingIndex ].mRealSpacePerTS = RealSpacePerTS;

              mClientState.mReportingIndex++;
              }

            mClientState.mGuardZoneTuningRealSpaceTimeSum      = 0.0;
            mClientState.mGuardZoneTuningTimeStepsPerGuardZone = 0;
            }
#endif
          }
     

#ifdef DO_REAL_SPACE_ON_SECOND_CORE_VNM
        void
        UpdateDynVarMgrIFOnSlaveCore( DynVarMgrIF          * aDynVarMgrIF )
          {
          mClientState.mDynVarMgrIF = aDynVarMgrIF;
          // Copy DynVarMgr flat structures from core0
          
          // Need to save away InteractionStateManager. Do NOT want this state overwriten
          memcpy( &TempISM, &mClientState.mDynVarMgrIF->mInteractionStateManagerIF, sizeof( TempISM ) );
          
          DynVarMgrIF* OtherDynVarMgr = (DynVarMgrIF *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF);
          
          memcpy( mClientState.mDynVarMgrIF, OtherDynVarMgr, sizeof(DynVarMgrIF) );

          // Copy back the saved Temp state of the InteractionStateManagerIF
          memcpy( &mClientState.mDynVarMgrIF->mInteractionStateManagerIF, &TempISM, sizeof( TempISM ) );
          
          mClientState.mDynVarMgrIF->mVoxelIndexToVoxelOrdinalMap = 
              (int *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mVoxelIndexToVoxelOrdinalMap);

          mClientState.mDynVarMgrIF->mVoxelsInBoxTable = 
              (char *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mVoxelsInBoxTable );

          mClientState.mDynVarMgrIF->mVoxelInteractionTypeTable.mMemory = 
              (unsigned int *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mVoxelInteractionTypeTable.mMemory );

          mClientState.mDynVarMgrIF->mFragmentsInVoxelsInP3MEBox = 
              (FragId *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mFragmentsInVoxelsInP3MEBox );

          mClientState.mDynVarMgrIF->mSiteIdToFragmentRepMap = 
              (FragmentRep *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mSiteIdToFragmentRepMap );

          mClientState.mDynVarMgrIF->mTimeToRunNsqInnerLoop = 
              (TimeValue *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mTimeToRunNsqInnerLoop );

          mClientState.mDynVarMgrIF->mLocalFragments = 
              (FragmentInRangePair *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mLocalFragments );

//           mClientState.mDynVarMgrIF->mLocalFragmentTable = 
//               (FragmentRecord *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mLocalFragmentTable );

          mClientState.mDynVarMgrIF->mPosit = 
              (XYZ *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mPosit );

          mClientState.mDynVarMgrIF->mPositTracker = 
              (XYZ *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mPositTracker );

          mClientState.mDynVarMgrIF->mVelocity = 
              (XYZ *) (0x50000000 + (unsigned)mClientState.mDynVarMgrIF->mVelocity );
          // Update the pointers needed by this core1
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

        #if ( defined( DO_REAL_SPACE_ON_SECOND_CORE ) || defined( DO_REAL_SPACE_ON_SECOND_CORE_VNM ) )
          if( IFP0::NsqUdfGroup0::UDF0_Bound::UDF::ReportsEnergy )
            mClientState.mDynVarMgrIF->mInteractionStateManagerIF.
                SetEnergyHolderUDFNum( 0, IFP0::NsqUdfGroup0::UDF0_Bound::UDF::Code );

          if( IFP0::NsqUdfGroup0::UDF1_Bound::UDF::ReportsEnergy )
            mClientState.mDynVarMgrIF->mInteractionStateManagerIF.
                SetEnergyHolderUDFNum( 1, IFP0::NsqUdfGroup0::UDF1_Bound::UDF::Code );

          if( IFP0::NsqUdfGroup0::UDF2_Bound::UDF::ReportsEnergy )
            mClientState.mDynVarMgrIF->mInteractionStateManagerIF.
                SetEnergyHolderUDFNum( 2, IFP0::NsqUdfGroup0::UDF2_Bound::UDF::Code );          
        #endif

          mClientState.mSimTick                  = aSimTick;

          aSignalSet.mUpdateFragmentVerletListSignal      = 1;
          aSignalSet.mUpdateFragmentAssignmentListSignal  = 1;
          aSignalSet.mResetSignal                         = 1;
          aSignalSet.mTuneGuardZoneSignal                 = 1;

          mClientState.mFirstTime                   = 0;
          mClientState.mCollectTimingData           = 1;

          mClientState.mOptGuardZone             = aDynVarMgrIF->GetCurrentVerletGuardZone();
          mClientState.mOptRealSpaceTimeStep     = MAX_REAL_SPACE_TIME_STEP_TIME;
          mClientState.mOptTSCount               = 0;
          mClientState.mDescendingSearchDelta    = 0.1;
          // mClientState.mTuneGuardZone            = 0;
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


          mORBManagerIF = aORBManagerIF; // mClientState.mDynVarMgrIF->GetORBManagerIF();

// #ifdef DO_REAL_SPACE_ON_SECOND_CORE_VNM
//           mORBManagerIF->mLocalFragWeight = NULL;
//           mORBManagerIF->mCountPerNode = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
//           assert( mORBManagerIF->mCountPerNode );
//           for( int i=0; i<Platform::Topology::GetSize(); i++ )
//             mORBManagerIF->mCountPerNode[ i ] = 0;
// #endif
          // Since we need a terminator in the DB we need to do the first expand here and put the term in.
          ExpandFragmentVerletDB();
          mFragmentVerletDBNext[ 0 ].mFragIdPairKey = GetFragIdPairKeyMax();
          mFragmentVerletDBPrev[ 0 ].mFragIdPairKey = GetFragIdPairKeyMax();

          #if ( defined( DO_REAL_SPACE_ON_SECOND_CORE_VNM ) && defined( PK_BLADE_SPI ) )
            _blLockBoxUnLock( KSPACE_DONE_LOCK );
            mDualCoreRealSpaceMode = 0;
          #endif
          
          mN33VerletList.Init() ; 
          mSiteVerletList.Init() ;
          }

        void
        ExpandFragmentVerletDB()
          {
          void * Memory;

          int NewCount =  5 * ( mAllocatedFragmentVerletDBCount + (128 * 1024) );

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
          void * Memory;

          int NewCount = 5 * ( mAssignmentListAllocatedCount + ( 64 * 1024 ) );
          
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
          void * Memory;
          void * Memory1;

          int NewCount = 5 * ( mActiveVerletListAllocatedCount + (32 * 1024) );
          
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

          BegLogLine(0)
            << "ExpandActiveVerletList()"
            << "Expand entries from "
            << mActiveVerletListAllocatedCount
            << " to "
            << NewCount
            << EndLogLine;

          mActiveVerletListAllocatedCount = NewCount;
          }

          int
          GetClosestMidpointIndex( int aStartIndex, int aLen, double aTargetWeight )
            {
            // assert( aStartIndex >= 0 && aStartIndex < aLen );
            
            if( aLen == 1 )
              {
              return aStartIndex;
              }

            int MidIndex = (aLen / 2 )+ aStartIndex;

            if( aTargetWeight > mActiveVerletListWeights[ MidIndex ] )
              {
              return GetClosestMidpointIndex( aStartIndex, aLen / 2, aTargetWeight );
              }
            else
              {
              return GetClosestMidpointIndex( aStartIndex + (aLen/2), aLen / 2, aTargetWeight );
              }
            }

        // The 'Send' method here will only be called when



        class FragmentPartialResultsDummyIF
          {
          public:

            int          mSimTick;
            int          mFragmentOrdinal;
            DynVarMgrIF *mDynVarMgrIF;

            #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
            int          mNullForceInteractionCount;
            #endif

            FragmentPartialResultsDummyIF( int          aSimTick,
                                      DynVarMgrIF *aDynVarMgrIF ,
                                      int          aFragmentOrdinal )
              {
              mSimTick          = aSimTick;
              mDynVarMgrIF      = aDynVarMgrIF;
              mFragmentOrdinal = aFragmentOrdinal;
              }

            inline
            void
            AddForce( SiteId aAbsSiteId, int aShellId, const XYZ & aForce )
              {
                return;
              }
          };

        typedef FragmentPartialResultsIF<DynVarMgrIF> FragmentPartialResults_T;

        template <int HalfEnergy>
          class GlobalPartialResultsIF
          {
          public:

            double mEnergyByOrdinal[ IFP0::NsqUdfGroup0::ActiveUDFCount ];
            XYZ    mVirialByShell[ IFP0::NUM_SHELLS ];

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
              for( int i=0; i < IFP0::NUM_SHELLS; i++ )
                {
                mVirialByShell[ i ].Zero();
                }
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
              if( ReportsEnergy )
                {
                assert( aOrdinal >= 0 && aOrdinal < IFP0::NsqUdfGroup0::ActiveUDFCount );

                mEnergyByOrdinal[ aOrdinal ] += HalfEnergy ? 0.5 * aEnergy : aEnergy;
                }
              }

            inline
            void
            AddVirial(int aShellId, XYZ aVirial)
              {
              assert( aShellId >= 0 && aShellId < IFP0::NUM_SHELLS );
              mVirialByShell[ aShellId ] += aVirial;
              }

            template <int ReportsVirial>
            inline
            void
            AddVirial(int aShellId, XYZ aVirial)
              {
              if (ReportsVirial)
                {
                assert( aShellId >= 0 && aShellId < IFP0::NUM_SHELLS );
                mVirialByShell[ aShellId ] += aVirial;
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
          void CreateWaterSiteVerletLists(double GuardRangeSqr)
          {
            BegLogLine(PKFXLOG_WATERSITE)
              << "CreateWaterSiteVerletLists()" 
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
                  } else {
                    if ( 0 == ExclusionMask )
                    {
                      // 3:3 in same mol, but no exclusions
                      N33Count += 1 ;
                    } else {
                      // 3:3 in same mol with exclusions, e.g. adjacent frags
                      SiteSiteCount += 3*3 ;
                    } 
                  } 
               } else {
                  // Not a 3:3
                  SiteSiteCount += NumberOfSitesA * NumberOfSitesB ;
               }
              
            } 
            BegLogLine(PKFXLOG_WATERSITE)
               "Nonbond 3:3 count=" << N33Count 
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
               int NumberOfSitesA = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
               int NumberOfSitesB = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
               int mlid1 = MSD_IF::GetMoleculeId( TargetI );
               int mlid2 = MSD_IF::GetMoleculeId( SourceI );
               ExclusionMask_T ExclusionMask = mFragmentVerletDBNext[ ActivePairIndex ].mExclusionMask;
//             int FirstSiteIdA   = MSD_IF::GetIrreducibleFragmentFirstSiteId( SourceI );
//             int FirstSiteIdB   = MSD_IF::GetIrreducibleFragmentFirstSiteId( TargetI );
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
                  } else {
                    if ( 0 == ExclusionMask )
                    {
                      // 3:3 in same mol, but no exclusions
                       mN33VerletList.SetFragPair(N33Index,SourceI,TargetI,FragmentImageVectorIndex,mClientState.mDynVarMgrIF) ;
                       N33Index += 1 ;
                    } else {
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
               } else {
                  // Not a 3:3
                  if ( mlid1 != mlid2 || 0 == ExclusionMask )
                  {
                    // But no exclusions
                       for ( unsigned int xA = 0 ; xA < NumberOfSitesA ; xA += 1)
                       {
                         XYZ PosA = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( SourceI, xA );
                         for ( unsigned int xB = 0 ; xB < NumberOfSitesB ; xB += 1 )
                         {
                             XYZ PosB = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetPosition( TargetI, xB );
                             double DistABSqr = NearestSquareDistanceInPeriodicVolume(PosA, PosB) ;
                             if ( DistABSqr<GuardRangeSqr )
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
                    
                  } else {
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
               BegLogLine(PKFXLOG_VECTOR_VERLET)
              << "CreateWaterSiteVerletLists ll=" << ll
              << EndLogLine ;
             
            for ( unsigned int v=0;v<ll;v+=1)
            {
               BegLogLine(PKFXLOG_VECTOR_VERLET)
              << "CreateWaterSiteVerletLists v=" << v
              << " FragA=" << mSiteVerletList.GetFragA(v)
              << " OffsetA=" << mSiteVerletList.GetOffsetA(v)
              << " FragB=" << mSiteVerletList.GetFragB(v)
              << " OffsetB=" << mSiteVerletList.GetOffsetB(v)
//    	        << " ImVX=" << mSiteVerletList.GetImageVectorTableIndex(v)
              << EndLogLine ;
              
            }
            
            
          } 
          
          void
          CreateFragmentVerletList( int aSimTick )
            {
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

            for( int i = 0; i < FragmentNeighborhoodCount; i++ )
              {
              FragId LocalFrag  = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetNthFragmentInNeighborhood( i );

              mClientState.mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenter(
                  LocalFrag,
                  &mClientState.mLocalFragments[ LocalFrag ].mExtent,
                  &mClientState.mLocalFragments[ LocalFrag ].mFragmentCenter );

              ///////////////////////// WTF? mClientState.mDynVarMgrIF->mLocalFragments[ LocalFrag ].mExtent *= 1.5;

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

              unsigned char FragmentImageVectorIndex = mFragmentVerletDBNext[ FragmentDatabaseIndex ].mFragmentImageVectorIndex;

              assert( FragmentImageVectorIndex >=0 && FragmentImageVectorIndex < 125 );
              XYZ*  FragmentImageVector      = & mClientState.mDynVarMgrIF->mImageVectorTable[ FragmentImageVectorIndex ];

              double DistanceCutoff = Cutoff
                                    + mClientState.mLocalFragments[ FragIdA ].mExtent
                                    + mClientState.mLocalFragments[ FragIdB ].mExtent;

              double DistanceCutoffSqr = DistanceCutoff * DistanceCutoff;

              XYZ ImageVector = *FragmentImageVector;

              /////****  MOVED THIS BLOCK TO GRAVE YARD AT BOTTOM:  #ifdef CALCULATE_IMAGE_VECTORS_ON_VERLET_LIST_GENERATION

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

//              int FirstSiteIdA   = MSD_IF::GetIrreducibleFragmentFirstSiteId( FragIdA );
                int NumberOfSitesA = MSD_IF::GetIrreducibleFragmentMemberSiteCount( FragIdA );

//              int FirstSiteIdB   = MSD_IF::GetIrreducibleFragmentFirstSiteId( FragIdB );
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
              continue;
              GoToTarget_IsInRange_AddToActiveList:


              if( mActiveVerletListCount >= mActiveVerletListAllocatedCount )
                ExpandActiveVerletList();
              
              mActiveVerletList[ mActiveVerletListCount ] = FragmentDatabaseIndex;
              mActiveVerletListCount++;
              }
              
                        
            mStartIndex = 0;
            mLength     = mActiveVerletListCount;

            CreateWaterSiteVerletLists(GuardRangeSqr) ;
#if defined(NSQ_SORT_VERLET_BY_KIND)
// Sort the active verlet list by 'kind', so (e.g.) all the water-waters come together
            SortActiveVerletList() ;
#endif            

#if defined( DO_REAL_SPACE_ON_SECOND_CORE_VNM )

            rts_dcache_store( GET_BEGIN_PTR_ALIGNED( &mActiveVerletListCount ),
                              GET_END_PTR_ALIGNED( &mActiveVerletListCount + 1) );

            rts_dcache_store( GET_BEGIN_PTR_ALIGNED( &mActiveVerletList ),
                              GET_END_PTR_ALIGNED( &mActiveVerletList + 1 ) );

            rts_dcache_store( GET_BEGIN_PTR_ALIGNED( mActiveVerletList ),
                              GET_END_PTR_ALIGNED( mActiveVerletList + mActiveVerletListCount ) );

            rts_dcache_store( GET_BEGIN_PTR_ALIGNED( &mActiveVerletListCount ),
                              GET_END_PTR_ALIGNED( &mActiveVerletListCount + 1) );

      mN33VerletList.dcache_store() ;
      mSiteVerletList.dcache_store() ;
#endif            


            }
            
          void
          CreateFragmentAssignment( int aSimTick )
            {
            unsigned int IrreducibleFragmentCount = mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount() ;

            double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
            double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );

            double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;

            double BoxTrace = mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mX +
                              mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mY +
                              mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector.mZ  ;

            double InRangeCutoff = ( SwitchUpperCutoff < BoxTrace ) ? SwitchUpperCutoff : BoxTrace ;

            double GuardZone = mClientState.mDynVarMgrIF->GetCurrentVerletGuardZone();

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

            int NumberOfPairsAssigned          = 0;

            FatCompVerletListGenStart.HitOE( PKTRACE_FATCOMP,
                                             "FatCompVerletListGen",
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

              int VoxelIndex1 = mClientState.mDynVarMgrIF->mInteractionStateManagerIF.GetVoxelIndex( LocalFrag );
              
//               if( !( VoxelIndex1 >= 0 && VoxelIndex1 < mClientState.mDynVarMgrIF->mMaxVoxelsInRange ) )
//                 BegLogLine( 1 )
//                   << "VoxelIndex1: " << VoxelIndex1
//                   << " LocalFrag: " << LocalFrag
//                   << " FragmentNeighborhoodCount: " << FragmentNeighborhoodCount
//                   << " i: " << i
//                   << " mClientState.mDynVarMgrIF->mMaxVoxelsInRange: " << mClientState.mDynVarMgrIF->mMaxVoxelsInRange
//                   << EndLogLine;

              assert( VoxelIndex1 >= 0 && VoxelIndex1 < mClientState.mDynVarMgrIF->mMaxVoxelsInRange );

              int VoxelOrdinal = mClientState.mDynVarMgrIF->mVoxelIndexToVoxelOrdinalMap[ VoxelIndex1 ];

              assert( VoxelOrdinal < mClientState.mDynVarMgrIF->mNumberOfVoxelsInRange );

              // If VoxelOrdinal == -1 this fragment is in node range,
              // but NOT in voxel range
              if( VoxelOrdinal != -1 )
                {
#if 0                
                int me = Platform::Topology::GetAddressSpaceId();
                double dummy;
                XYZ centerDummy;
                
                mClientState.mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenter(
                    LocalFrag,
                    &dummy,
                    &centerDummy );

                double Temp[3];                
                Temp[ 0 ] = centerDummy.mX;
                Temp[ 1 ] = centerDummy.mY;
                Temp[ 2 ] = centerDummy.mZ;
                int VoxelIdA = mClientState.mDynVarMgrIF->mRecBisTree->VoxelId( Temp );
                
                BegLogLine(( me == 0x0 || me == 0x41 ) && ( LocalFrag == 2706 ) || ( LocalFrag == 2708 ) )
                  << aSimTick
                  << " LocalFrag: " << LocalFrag
                  << " VoxelOrdinal: " << VoxelOrdinal
                  << " VoxelIndex: " << VoxelIndex1
                  << " VoxelId: " << VoxelIdA
                  << " centerDummy: " << centerDummy
                  << EndLogLine;

#endif                
                assert( LocalFragmentsCount>=0 && LocalFragmentsCount < NUMBER_OF_FRAGMENTS );
                mClientState.mLocalFragments[ LocalFragmentsCount ].mFragId       = LocalFrag;
                mClientState.mLocalFragments[ LocalFragmentsCount ].mVoxelOrdinal = VoxelOrdinal;
                mClientState.mLocalFragments[ LocalFragmentsCount ].mMoleculeId   = MSD_IF::GetMoleculeId( LocalFrag );
                LocalFragmentsCount++;
                }
              else
                {
                // This fragment is not needed by NSQ, check p3me
                if( !mClientState.mDynVarMgrIF->IsVoxelIndexInP3MEBoxRange( VoxelIndex1 ) )
                  {
                  mClientState.mDynVarMgrIF->mInteractionStateManagerIF.EliminateFragmentInNeighborhood( LocalFrag );

                  BegLogLine( 0 )
                    << "NSQ: Eliminating fragment: " << LocalFrag
                    << EndLogLine;
                  }
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

            int NextDbIndex = 0;
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

#if 0
            int NumberAssigned = 0;
            int NumberTotal = 0;
            VoxInteractionTableIterStart.HitOE( PKTRACE_VOX_INTERACTION_TABLE_ITER,
                                                "VoxInteractionTableIter",
                                                mAddressSpaceId,
                                                VoxInteractionTableIterStart );

            long long tNow = rts_get_timebase();

            for( int fragA = 0; fragA < LocalFragmentsCount; fragA++ )
              {
              FragId FragIdA         = mClientState.mLocalFragments[ fragA ].mFragId;
              int    VoxelOrdinalA   = mClientState.mLocalFragments[ fragA ].mVoxelOrdinal;
              int    VoxelBaseOffset = VoxelOrdinalA * mClientState.mDynVarMgrIF->mNumberOfVoxelsInRange;
              int    MoleculeIdA     = mClientState.mLocalFragments[ fragA ].mMoleculeId;

              // ATTENTION: fragA+1 assumes that self fragments are always excluded
              for( int fragB = fragA+1; fragB < LocalFragmentsCount; fragB++ )
                {
                // Get the node type index.
                FragId FragIdB       = mClientState.mLocalFragments[ fragB ].mFragId;

                int    VoxelOrdinalB = mClientState.mLocalFragments[ fragB ].mVoxelOrdinal;

                unsigned int VoxelInteractionIndex = VoxelBaseOffset + VoxelOrdinalB;

                #ifdef VOXEL_INTERACTION_TABLE_ENTRY_AS_CHAR
                  assert( VoxelInteractionIndex < mClientState.mDynVarMgrIF->mVoxelInteractionTypeTableCount );

                  unsigned char VoxelInteractionType = mClientState.mDynVarMgrIF->mVoxelInteractionTypeTable[ VoxelInteractionIndex ];
                #else
                  unsigned char VoxelInteractionType =
                                  mClientState.mDynVarMgrIF->mVoxelInteractionTypeTable.GetElement( VoxelInteractionIndex );
                #endif

//                 InteractionTotal ++ ;
//                 InteractionType[ VoxelInteractionType ] ++ ;

                unsigned char AssignedInteractionBit = VoxelInteractionType & 0x01;
                unsigned char RangeBit               = VoxelInteractionType & 0x02;
                NumberAssigned += AssignedInteractionBit;
                NumberTotal++;
                }
              }

            VoxInteractionTableIterFinis.HitOE( PKTRACE_VOX_INTERACTION_TABLE_ITER,
                                                "VoxInteractionTableIter",
                                                mAddressSpaceId,
                                                VoxInteractionTableIterFinis );

            long long tFinal = rts_get_timebase();
            double Tdelta = 1.0 * (tFinal-tNow) / 700000000.0;

            BegLogLine( 1 )
                << aSimTick 
                << " NumberAssigned: " << NumberAssigned
                << " NumberTotal: " << NumberTotal
                << " Tdelta: " << FormatString("% 16.14f") << Tdelta
                << " Tdelta/NumberAssigned: " << FormatString("% 16.14f") << Tdelta / (1.0*NumberAssigned)
                << " Tdelta/NumberTotal: " << FormatString("% 16.14f") << Tdelta / (1.0 * NumberTotal)
                << EndLogLine;

#endif

            for( int fragA = 0; fragA < LocalFragmentsCount; fragA++ )
              {
              FragId FragIdA         = mClientState.mLocalFragments[ fragA ].mFragId;
              int    VoxelOrdinalA   = mClientState.mLocalFragments[ fragA ].mVoxelOrdinal;
              int    VoxelBaseOffset = VoxelOrdinalA * mClientState.mDynVarMgrIF->mNumberOfVoxelsInRange;
              int    MoleculeIdA     = mClientState.mLocalFragments[ fragA ].mMoleculeId;

              // ATTENTION: fragA+1 assumes that self fragments are always excluded
              for( int fragB = fragA+1; fragB < LocalFragmentsCount; fragB++ )
                {
                // Get the node type index.
                FragId FragIdB       = mClientState.mLocalFragments[ fragB ].mFragId;

                int    VoxelOrdinalB = mClientState.mLocalFragments[ fragB ].mVoxelOrdinal;

                unsigned int VoxelInteractionIndex = VoxelBaseOffset + VoxelOrdinalB;

                #ifdef VOXEL_INTERACTION_TABLE_ENTRY_AS_CHAR
                  assert( VoxelInteractionIndex < mClientState.mDynVarMgrIF->mVoxelInteractionTypeTableCount );

                  unsigned char VoxelInteractionType = mClientState.mDynVarMgrIF->mVoxelInteractionTypeTable[ VoxelInteractionIndex ];
                #else
                  unsigned char VoxelInteractionType =
                                  mClientState.mDynVarMgrIF->mVoxelInteractionTypeTable.GetElement( VoxelInteractionIndex );
                #endif

                InteractionTotal ++ ;
                assert( VoxelInteractionType >= 0 && VoxelInteractionType < 4 );
                InteractionType[ VoxelInteractionType ] ++ ;

                unsigned char AssignedInteractionBit = VoxelInteractionType & 0x01;
                unsigned char RangeBit               = VoxelInteractionType & 0x02;

                if( VoxelInteractionType == 0 )
                  {
                  // Note that if this pair of frags is in the db ( because they came from diff voxels ) it will be left out.
                  Culled_NotInRangeNotAssigned++;
                  continue;
                  }

                #ifdef NSQ_MINIMIZE_PAIR_DB
                  // This block prevents trying to keep around nearby but not assigned DB entries - more imaging and exlc comps.
                  if( ( ! AssignedInteractionBit  ) && ( RangeBit ) )
                    {
                    // Note that if this pair of frags is in the db ( because they came from diff voxels ) it will be left out.
                    Culled_InRangeNotAssigned++;
                    continue;
                    }
                #endif

#if 0
                BegLogLine( aSimTick == 5091001 && FragIdA == 2706 && FragIdB == 2708 && AssignedInteractionBit )
                  << aSimTick
                  << " FragIdA: " << FragIdA
                  << " FragIdB: " << FragIdB
                  << " VoxelOrdinalA: " << VoxelOrdinalA
                  << " VoxelOrdinalB: " << VoxelOrdinalB
                  << " AssignementInteractionBit: " << AssignedInteractionBit  
                  << EndLogLine;
#endif

                int    MoleculeIdB   = mClientState.mLocalFragments[ fragB ].mMoleculeId;

                assert( FragIdA < FragIdB );

                FragIdPairKey NewFragIdPairKey = MakeFragIdPairKey( FragIdA, FragIdB ); // A is "target", B is "source

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
                  if( NextDbIndex  + 1 >= mAllocatedFragmentVerletDBCount )
                    ExpandFragmentVerletDB();

                  assert( NextDbIndex >=0 && NextDbIndex < mAllocatedFragmentVerletDBCount );
                  mFragmentVerletDBNext[ NextDbIndex ] = mFragmentVerletDBPrev[ PrevDbIndex ];

                  DBIndexForAssignmentList = NextDbIndex;  // Save the index to use to add to AssignmentList if AssignmenBit is on.

                  NextDbIndex ++ ;
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

                    if( NextDbIndex  + 1 >= mAllocatedFragmentVerletDBCount )
                      ExpandFragmentVerletDB();

                    assert( NextDbIndex >=0 && NextDbIndex < mAllocatedFragmentVerletDBCount );
                    mFragmentVerletDBNext[ NextDbIndex ].mFragIdPairKey           = NewFragIdPairKey;

                    mFragmentVerletDBNext[ NextDbIndex ].mExclusionMask           = ExclMask;
                    assert( mFragmentVerletDBNext[ NextDbIndex ].mExclusionMask  == ExclMask ); // ensure enough bits in DB field

                    mFragmentVerletDBNext[ NextDbIndex ].mFragmentImageVectorIndex = ImageVectorTableIndex;

                    DBIndexForAssignmentList = NextDbIndex;  // Save the index to use to add to AssignmentList if AssignmenBit is on.

                    NextDbIndex++;
                    }
                  }

                // Put any entries on the assigned interaction list
                if( AssignedInteractionBit )
                  {
                  if( mAssignmentListCount + 1 >= mAssignmentListAllocatedCount )
                    ExpandAssignmentList();

                  // Should check RangeBit in here and put on separate lists for switch, non switch
                  assert( mAssignmentListCount >=0 && mAssignmentListCount < mAssignmentListAllocatedCount );

                  mAssignmentList[ mAssignmentListCount ].mFragmentDatabaseIndex = DBIndexForAssignmentList;
                  mAssignmentListCount ++ ;
                  }

                }
              }

            FatCompVerletListGenFinis.HitOE( PKTRACE_FATCOMP,
                                             "FatCompVerletListGen",
                                             mAddressSpaceId,
                                             FatCompVerletListGenFinis );

            BegLogLine(0)
              << "Assignment: "
              << " SimTick "
              << SimTick
              << " DBEntryCount "
              << NextDbIndex
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

            BegLogLine(0)
              << "Assignment: "
              << " SimTick "
              << SimTick
              << " DBEntryCount "
              << NextDbIndex
              << " AssignmentListCount "
              << mAssignmentListCount
              << " HoodFragCnt "
              << FragmentNeighborhoodCount
              << " LocFragCnt "
              << LocalFragmentsCount
              << " DB_Evicted " <<  DB_Evicted
              << " DB_Inserted " <<  DB_Inserted
              << " Culled_InRangeAssigned " <<  Culled_InRangeAssigned
              << " Culled_InRangeNotAssigned " <<  Culled_InRangeNotAssigned
              << " Culled_NotInRangeNotAssigned " <<  Culled_NotInRangeNotAssigned
              << EndLogLine;

            // Put int the terminator for when this is the Prev - terminator is essential to above function.
            assert( NextDbIndex >=0 && NextDbIndex < mAllocatedFragmentVerletDBCount );
            mFragmentVerletDBNext[ NextDbIndex ].mFragIdPairKey = GetFragIdPairKeyMax();

            #if ( defined( DO_REAL_SPACE_ON_SECOND_CORE_VNM ) && defined( PK_BLADE_SPI ) )
            rts_dcache_evict_normal();
            #endif
            }
            
          void
          Send( int                 aSimTick, 
                RealSpaceSignalSet& aSignalSet )
      __attribute__((noinline)) ;
      
//#if defined(NSQ_NOCOMPILE)
//	;
//#else
//#endif

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

            #if (!defined( DO_REAL_SPACE_ON_SECOND_CORE ) && !defined( DO_REAL_SPACE_ON_SECOND_CORE_VNM ) )
              EmitTimeStamp( aSimTick, Tag::RealSpace_Begin, 1 );
            #endif  
  
            BegLogLine( PKFXLOG_NSQ_SEND )
                << "Send(): "
                << " SimTick "
                << aSimTick
                << EndLogLine;

      NSQStat.Entry() ;
            GlobalPartialResultsIF<0> GlobalPartialResultAxB;
            GlobalPartialResultAxB.Zero();

            GlobalInputParametersIF GlobalInputParameters;
            GlobalInputParameters.Init();

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
            
            NSQ_RealSpace_Meat_Start.HitOE( PKTRACE_REAL_SPACE_MEAT,
                                            "NSQ_RealSpace_Meat_Start",
                                            0,
                                            NSQ_RealSpace_Meat_Start );
 
            BegLogLine( 0 )
              << aSimTick 
              << " mActiveVerletListCount: " << mActiveVerletListCount
              << EndLogLine;

      NSQStat.MeatStart() ; 

      IFP0::ExecuteSiteVerletList(aSimTick,
                               GlobalInputParameters,
                               mClientState.mDynVarMgrIF,
                               GlobalPartialResultAxB,
                               mSiteVerletList) ; 
            if ( 0 == FORCE_ATOM_ATOM_FRAGS )
            {
               IFP0::ExecuteN33VerletList(aSimTick,
                                 GlobalInputParameters,
                                 mClientState.mDynVarMgrIF,
                                 GlobalPartialResultAxB,
                                 mN33VerletList) ;
            }
                        
            NSQStat.MeatEnd() ; 
            
            NSQ_RealSpace_Meat_Finis.HitOE( PKTRACE_REAL_SPACE_MEAT,
                                            "NSQ_RealSpace_Meat_Finis",
                                            0,
                                            NSQ_RealSpace_Meat_Finis );
            
              // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
              unsigned TimeStep = aSimTick / MSD::SimTicksPerTimeStep;
              EmitTimeStamp(aSimTick, Tag::RealSpace_End, 1 );

              // Check if this particular SimTick is the new TimeStep.
              if( TimeStep * MSD::SimTicksPerTimeStep == aSimTick )
                {
                BegLogLine( PKFXLOG_NSQLOOP_EMIT )
                  << "NSQLoops::Send() "
                  << " Emitting RDG Energy packets "
                  << " SimTick " << aSimTick
                  << " TimeStep " << TimeStep
                  << EndLogLine;
                
                #if MSDDEF_ReplicaExchange 
                  double Energy = 
                      IFP0::NsqUdfGroup0::UDF0_Bound::UDF::ReportsEnergy ? GlobalPartialResultAxB.GetEnergy( 0 ) : 0 + 
                      IFP0::NsqUdfGroup0::UDF1_Bound::UDF::ReportsEnergy ? GlobalPartialResultAxB.GetEnergy( 1 ) : 0 + 
                      IFP0::NsqUdfGroup0::UDF2_Bound::UDF::ReportsEnergy ? GlobalPartialResultAxB.GetEnergy( 2 ) : 0; 
                  
                  mClientState.mDynVarMgrIF->AddToPotentialEnergy( aSimTick, Energy );
                #endif

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
              int RespaLevelOfFirstShell = MSD_IF::GetRespaLevelOfFirstShell();
              for(int shellId=0; shellId< IFP0::NUM_SHELLS; shellId++ )
                {
                mClientState.mDynVarMgrIF->AddVirial( RespaLevelOfFirstShell+shellId,
                                                      GlobalPartialResult.mVirialByShell[ shellId ] );
                }
            #endif
            mVerletStatistics.Collect(
              mFragmentVerletDBNext ,
              mStartIndex,
              mLength 
            ) ;
            mVerletStatistics.Display() ;
            NSQStat.Exit() ;
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
