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
 * Module:          IFPHelper
 *
 * Purpose:         Manage pairwise interactions between irreducible partition pairs
 *                  during NonBonded Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010522 BGF Created from design.
 *
 ***************************************************************************/

// This file contains active message infrastructure and nonbonded engine
// thread code.

// Currently, this code does not have an initialization phase.  This leads to
// some extra data in operand packets and also makes it impossible to
// dynamically allocate some of the structures.  These are allocated based
// on a compilte time limit.  THIS CAN ALL BE CHANGED AS WE GO ALONG.

#ifndef _IRREDUCIBLE_PARTITION_PAIR_HPP_
#define _IRREDUCIBLE_PARTITION_PAIR_HPP_

#ifndef PKFXLOG_NBVM
#define PKFXLOG_NBVM 0
#endif

#ifndef PKFXLOG_IFPVECTOR
#define PKFXLOG_IFPVECTOR 0
#endif


#ifndef PKFXLOG_SITEFORCES_NB
#define PKFXLOG_SITEFORCES_NB 0
#endif

#ifndef PKFXLOG_IPARTPAIR
#define PKFXLOG_IPARTPAIR 0
#endif

#ifndef PKFXLOG_VIRIALTRACE
#define PKFXLOG_VIRIALTRACE 0
#endif

#ifndef PKFXLOG_ENERGYTRACE
// #define PKFXLOG_ENERGYTRACE (SimTick == 11)
#define PKFXLOG_ENERGYTRACE ( 0 )
#endif

#ifndef PKFXLOG_IFP_DRIVE_ONE
#define PKFXLOG_IFP_DRIVE_ONE 0
#endif

#ifndef PKFXLOG_REGRESS_UDF_RESULTS
#define PKFXLOG_REGRESS_UDF_RESULTS ( 0 )
#endif

#ifndef PKFXLOG_IFP_DRIVE_ONE_ANSWER
#define PKFXLOG_IFP_DRIVE_ONE_ANSWER ( 0 )
#endif

#ifndef PKFXLOG_REPORTSTATICCOUNTERS
#define PKFXLOG_REPORTSTATICCOUNTERS   ( 0 )
#endif

#ifndef PKFXLOG_PAIRINTERACTIONMASKARRAY_MAP
#define PKFXLOG_PAIRINTERACTIONMASKARRAY_MAP  ( 0 )
#endif

#ifndef BLUEMATTER_STATS0
#define BLUEMATTER_STATS0 0
#endif


#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/NBVM_IF.hpp>

typedef NBVM<3> NBVMX;
typedef NBVM_SWON<3> NBVMX_SWON;
typedef NBVM_SCALAR<3> NBVMX_SCALAR;

#if defined(PERFORMANCE_EXCLUSION_SCALE_PREP)
typedef EXCLUSION_NBVM<3> EXCLUSION_NBVMX;
#endif

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#include <BlueMatter/IFP_Binder.hpp>

static const double ExclusionMultiplier[2] = { 0.0, 1.0 } ;

class OpCounter
  {
  public:
    unsigned int CurrentValue ;
    inline void Clear(void) ;
    inline unsigned int Get(void) ;
    inline void Increment(void);
  } ;

#if BLUEMATTER_STATS0

void
OpCounter::
Clear(void)
  {
  CurrentValue = 0 ;
  }

inline
unsigned int
OpCounter::
Get(void)
  {
  return CurrentValue ;
  }

inline
void
OpCounter::
Increment(void)
  {
  CurrentValue += 1 ;
  }
#else
void         OpCounter::Clear(void)     { }
unsigned int OpCounter::Get(void)       { return 0 ; }
void         OpCounter::Increment(void) { }

#endif //BLUEMATTER_STATS0

OpCounter TotalPairOpsCounter;
OpCounter TotalPairOpsOutsideSiteSiteCutoff;
OpCounter ZeroForceResultOpsCounter;


#if defined(PK_GCC)
  #define k_DoPressureControl MSDDEF_DoPressureControl
#else
  enum
    {
    k_DoPressureControl = MSDDEF_DoPressureControl
    } ;
#endif

template <int UDF0_Code, int UDF1_Code, int UDF2_Code>
static
inline
unsigned int
CreatePairInteractionMask(const char         * PairInteractionMaskArray,
                                unsigned int   PairInteractionMaskIndex)
  {
  unsigned int PairInteractionMask ;
  if( PairInteractionMaskArray == NULL )
    {
    // when there is no persistant state, assume all UDFs compiled in will fire
    PairInteractionMask =
      ( UDF0_Code == -1 ? 0 : (1<<0) ) |
      ( UDF1_Code == -1 ? 0 : (1<<1) ) |
      ( UDF2_Code == -1 ? 0 : (1<<2) ) ;
    }
  else
    {
    PairInteractionMask = PairInteractionMaskArray[ PairInteractionMaskIndex ];
    }
  return PairInteractionMask ;
  }

#ifdef MSDDEF_STDCOULOMB
typedef UDF_NSQCoulomb_Bound UDF_NSQChargeRealSpace_Bound ;
#endif

#if ( defined( MSDDEF_EWALD ) || defined( MSDDEF_P3ME ) )
typedef UDF_NSQEwaldRealSpace_Bound UDF_NSQChargeRealSpace_Bound ;
#endif

template<class NsqUdfGroup>
class IFPHelper
  {
  public:
    typedef typename NsqUdfGroup::UDF0_Bound UDF0_Bound;
    typedef typename NsqUdfGroup::UDF1_Bound UDF1_Bound;
    typedef typename NsqUdfGroup::UDF2_Bound UDF2_Bound;

    // SWITCH_FULLY_ON = no switches to execute
    // SWITCH_ZONE_1   = one switch to execute
    // SWITCH_ZONE_2   = two switches to execute
    enum { SWITCH_FULLY_ON=0, SWITCH_ZONE_1=1, SWITCH_ZONE_2=2 };

    static
    void
    ClearStaticCounters()
      {
      TotalPairOpsCounter.Clear();
      TotalPairOpsOutsideSiteSiteCutoff.Clear();
      ZeroForceResultOpsCounter.Clear();
      }

    static void
    ReportStaticCounters()
      {
      BegLogLine( PKFXLOG_REPORTSTATICCOUNTERS )
        << "IFP Counters "
        << " Total Site Pairs Done "
        << TotalPairOpsCounter.Get()
        << " TotalPairOpsOutsideSiteSiteCutoff "
        << TotalPairOpsOutsideSiteSiteCutoff.Get()
        << " ZeroForceResultOpsCounter "
        <<  ZeroForceResultOpsCounter.Get()
        << EndLogLine;
      }


    template<int NSQ_Type, class UDF_Bound, class NBVMX_CLASS >
    class DriveOne {};

    // This specialization does combining on the fly - used to be NSQ2
    template<class UDF_Bound, class NBVMX_CLASS >
    class DriveOne< 2, UDF_Bound, NBVMX_CLASS >
      {
      public:
        static
        void
        Init()
          {
          }


        inline
        static
        void
        Execute( NBVMX_CLASS &nbvm, int aSourceAbsSiteId, int aTargetAbsSiteId )
          {
          #if PKFXLOG_IPARTPAIR
          int UdfCode = UDF_Bound::UDF::Code;
          int NsqType = UDF_Bound::UDF::NsqType;

          BegLogLine(PKFXLOG_IFP_DRIVE_ONE)
            << "IFP UDF DriveOne Type "
            << UDF_Bound::UDF::NsqType
            << " Code "
            << UDF_Bound::UDF::Code
            << " Name "
            << UDF_Bound::UDF::UdfName
            << " SSiteId "
            << aSourceAbsSiteId
            << " TSiteId "
            << aTargetAbsSiteId
            << nbvm
            << EndLogLine;
          #endif

          A98_TYPENAME UDF_Bound::ParamType SourceP = UDF_Bound::GetParam( aSourceAbsSiteId );
          A98_TYPENAME UDF_Bound::ParamType TargetP = UDF_Bound::GetParam( aTargetAbsSiteId );

          A98_TYPENAME UDF_Bound::ParamType STP;
          UDF_Bound::UDF::Combine( SourceP, TargetP, STP );

#if 0
          nbvm.SetSiteId( 0, aSourceAbsSiteId );
          nbvm.SetSiteId( 1, aTargetAbsSiteId );
#endif
          UDF_Bound::UDF::Execute( nbvm, STP );

          BegLogLine(0 && PKFXLOG_IFP_DRIVE_ONE_ANSWER)
            << "SimTick " << SimTick
            //            << "IFP UDF DriveOne Type "
            //            << UDF_Bound::UDF::NsqType
            << " Code "
            << UDF_Bound::UDF::Code
            //<< " Name "
            //<< UDF_Bound::UDF::UdfName
            << " SSiteId "
            << aSourceAbsSiteId
            << " TSiteId "
            << aTargetAbsSiteId
            << " Energy "
            << FormatString("%16.14f") << nbvm.GetEnergy()
            << EndLogLine;


          BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
            << FormatString("UDFREG % 6d ") << SimTick
            << FormatString(" % 2d ")      << UDF_Bound::UDF::Code
            << FormatString(" S0 %5d ")    << aSourceAbsSiteId
            << FormatString(" S1 %5d ")    << aTargetAbsSiteId
            << nbvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
            << nbvm.GetForce(0) << " "
            << nbvm.GetForce(1)
            << EndLogLine;

          }
      };

    template<class UDF_Bound, class NBVMX_CLASS>
    class DriveOne< 1, UDF_Bound, NBVMX_CLASS>
      {
      public:
        static
        void
        Init()
          {
          AccessParam( 0, 0, 1 );
          }

        static const
        typename UDF_Bound::ParamType &
        AccessParam( SiteId aI, SiteId aJ, int aInitFlag = 0 )
          {
          static int InitDone = 0;
          static A98_TYPENAME UDF_Bound::ParamType CombinedParams[ NUMBER_OF_NSQ_SITE_TYPES ][ NUMBER_OF_NSQ_SITE_TYPES ];

          if( aInitFlag != 0 )
            {
            for( int i = 0; i < NUMBER_OF_NSQ_SITE_TYPES; i++ )
              {
              for( int j = 0; j < NUMBER_OF_NSQ_SITE_TYPES; j++ )
                {
                A98_TYPENAME UDF_Bound::ParamType ip = UDF_Bound::ParamAccessor::GetParamByIndex( i );
                A98_TYPENAME UDF_Bound::ParamType jp = UDF_Bound::ParamAccessor::GetParamByIndex( j );
                A98_TYPENAME UDF_Bound::ParamType ijp;
                UDF_Bound::UDF::Combine( ip, jp, ijp );
                CombinedParams[i][j] =  ijp;
                }
              }
            InitDone = 1;
      return CombinedParams[0][0] ;
            }

          assert( InitDone );
//          UDF_Bound::ParamType rc = CombinedParams[ aI ][ aJ ];
//          return( rc );
           return  CombinedParams[ aI ][ aJ ];
          }

        inline
        static
        void
        Execute( NBVMX_CLASS &nbvm, int aSourceAbsSiteId, int aTargetAbsSiteId )
          {
          #if PKFXLOG_IPARTPAIR
          int UdfCode = UDF_Bound::UDF::Code;
          BegLogLine(PKFXLOG_IFP_DRIVE_ONE)
            << "IFP UDF DriveOne Type "
            << UDF_Bound::UDF::NsqType
            << " Code "
            << UDF_Bound::UDF::Code
            << " Name "
            << UDF_Bound::UDF::UdfName
            << " SSiteId "
            << aSourceAbsSiteId
            << " TSiteId "
            << aTargetAbsSiteId
            << EndLogLine;
          int NsqType = UDF_Bound::UDF::NsqType;

          #endif

          assert( aSourceAbsSiteId >= 0 && aSourceAbsSiteId < MSD::NsqSiteIDtoSiteTypeMapSize );
          assert( aTargetAbsSiteId >= 0 && aTargetAbsSiteId < MSD::NsqSiteIDtoSiteTypeMapSize );

          int SourceParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aSourceAbsSiteId ];
          int TargetParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aTargetAbsSiteId ];

          A98_TYPENAME UDF_Bound::ParamType STP = AccessParam( SourceParamSiteType, TargetParamSiteType );

          #ifndef NDEBUG
            nbvm.SetSiteId( 0, aSourceAbsSiteId );
            nbvm.SetSiteId( 1, aTargetAbsSiteId );
          #endif

          UDF_Bound::UDF::Execute( nbvm, STP );


          BegLogLine(0 && PKFXLOG_IFP_DRIVE_ONE_ANSWER)
            << "SimTick " << SimTick
            //            << "IFP UDF DriveOne Type "
            //            << UDF_Bound::UDF::NsqType
            << " Code "
            << UDF_Bound::UDF::Code
            //<< " Name "
            //<< UDF_Bound::UDF::UdfName
            << " SSiteId "
            << aSourceAbsSiteId
            << " TSiteId "
            << aTargetAbsSiteId
            << " Energy "
            << nbvm.GetEnergy()
            << EndLogLine;


          BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
            << FormatString("UDFREG % 6d ") << SimTick
            << FormatString(" % 2d ")       << UDF_Bound::UDF::Code
            << FormatString(" S0 % 5d ")    << aSourceAbsSiteId
            << FormatString(" S1 % 5d ")    << aTargetAbsSiteId
            << nbvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
            << nbvm.GetForce(0) << " "
            << nbvm.GetForce(1)
            << EndLogLine;

          }
      };

    template<class UDF_Bound, class NBVMX_CLASS>
    class DriveOne< 0, UDF_Bound, NBVMX_CLASS>
      {
      public:
        static
        void
        Init()
          {
          }

        inline
        static
        void
        Execute( NBVMX_CLASS &nbvm, int aSourceAbsSiteId, int aTargetAbsSiteId )
          {
          #if PKFXLOG_IPARTPAIR
          int UdfCode = UDF_Bound::UDF::Code;
          int NsqType = UDF_Bound::UDF::NsqType;

          BegLogLine(PKFXLOG_IFP_DRIVE_ONE)
            << "IFP UDF DriveOne Type "
            << UDF_Bound::UDF::NsqType
            << " Code "
            << UDF_Bound::UDF::Code
            << " Name "
            << UDF_Bound::UDF::UdfName
            << " SSiteId "
            << aSourceAbsSiteId
            << " TSiteId "
            << aTargetAbsSiteId
            << EndLogLine;
          #endif

          static A98_TYPENAME UDF_Bound::ParamType STP;
          ///UDF_Bound::UDF::Combine( SourceP, TargetP, STP );
          // NOTE: this could be registered without a param and we could notice
          // that an NSQ0 was running and not call with a param at all.
          UDF_Bound::UDF::Execute( nbvm, STP );

          BegLogLine(0 && PKFXLOG_IFP_DRIVE_ONE_ANSWER)
            << "SimTick " << SimTick
            //            << "IFP UDF DriveOne Type "
            //            << UDF_Bound::UDF::NsqType
            << " Code "
            << UDF_Bound::UDF::Code
            //<< " Name "
            //<< UDF_Bound::UDF::UdfName
            << " SSiteId "
            << aSourceAbsSiteId
            << " TSiteId "
            << aTargetAbsSiteId
            << " Energy "
            << nbvm.GetEnergy()
            << EndLogLine;

          BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
            << FormatString("UDFREG % 6d ") << SimTick
            << FormatString(" % 2d ")       << UDF_Bound::UDF::Code
            << FormatString(" S0 %5d ")    << aSourceAbsSiteId
            << FormatString(" S1 %5d ")    << aTargetAbsSiteId
            << nbvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
            << nbvm.GetForce(0) << " "
            << nbvm.GetForce(1)
            << EndLogLine;

          }
      };

     static
     void
     Init()
     {
      if( UDF0_Bound::UDF::Code != -1 )
      {
        DriveOne<UDF0_Bound::UDF::NsqType, UDF0_Bound, NBVMX>::Init();
        DriveOne<UDF0_Bound::UDF::NsqType, UDF0_Bound, NBVMX_SCALAR>::Init();
#if defined(PERFORMANCE_EXPLOIT_FULLY_ON)
        DriveOne<UDF0_Bound::UDF::NsqType, UDF0_Bound, NBVMX_SWON>::Init();
#endif
#if defined(PERFORMANCE_EXCLUSION_SCALE_INIT)
        DriveOne<UDF0_Bound::UDF::NsqType, UDF0_Bound, EXCLUSION_NBVMX>::Init();
#endif
      }
      if( UDF1_Bound::UDF::Code != -1 )
      {
        DriveOne<UDF1_Bound::UDF::NsqType, UDF1_Bound, NBVMX>::Init();
        DriveOne<UDF1_Bound::UDF::NsqType, UDF1_Bound, NBVMX_SCALAR>::Init();
#if defined(PERFORMANCE_EXPLOIT_FULLY_ON)
        DriveOne<UDF1_Bound::UDF::NsqType, UDF1_Bound, NBVMX_SWON>::Init();
#endif
#if defined(PERFORMANCE_EXCLUSION_SCALE_INIT)
        DriveOne<UDF1_Bound::UDF::NsqType, UDF1_Bound, EXCLUSION_NBVMX>::Init();
#endif
      }
      if( UDF2_Bound::UDF::Code != -1 )
      {
        DriveOne<UDF2_Bound::UDF::NsqType, UDF2_Bound, NBVMX>::Init();
        DriveOne<UDF2_Bound::UDF::NsqType, UDF2_Bound, NBVMX_SCALAR>::Init();
#if defined(PERFORMANCE_EXPLOIT_FULLY_ON)
        DriveOne<UDF2_Bound::UDF::NsqType, UDF2_Bound, NBVMX_SWON>::Init();
#endif
#if defined(PERFORMANCE_EXCLUSION_SCALE_INIT)
       DriveOne<UDF2_Bound::UDF::NsqType, UDF2_Bound, EXCLUSION_NBVMX>::Init();
#endif
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

    template<class SourceGroupIF, class TargetGroupIF>
    static
    void
    CreatePersistentState(                    SourceGroupIF      aSourceFragmentIF,
                                              TargetGroupIF      aTargetFragmentIF,
                                              ExclusionMask_T ** aPersistentStatePtr )
      {
      int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

      // Create a flag to be used to recognize when source and target fragments are the
      // same and thus we can do 1/2 the force term computations by applying the results
      // of each computation to both sits sites.

      int SourceIsTargetFlag = ( SourceIrreducibleFragmentId == TargetIrreducibleFragmentId );

      BegLogLine(PKFXLOG_IPARTPAIR) << "IrreducibleFragmentPair(): STARTING " << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId << " / " << SourceIrreducibleFragmentId << " PersistentStatePointer "
                                    << *aPersistentStatePtr << EndLogLine;

      int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
      int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

      // Assert that the persistant state pointer is NULL, meaning not set up yet.
      assert( *aPersistentStatePtr == NULL );
      ExclusionMask_T *  PairInteractionMaskArray = (ExclusionMask_T *) *aPersistentStatePtr;
      // For now, we use a byte as a pair interaction mask.
      // We can ad a bit for verlet list cutoff distances easily.

      // NOTE: where Source == Target, the following array could be smaller.
      int     PairInteractionMaskArraySize = TargetFragmentSiteCount * SourceFragmentSiteCount;

      // PairInteractionMaskArray     = new char[ PairInteractionMaskArraySize ];
      pkNew( &PairInteractionMaskArray, PairInteractionMaskArraySize, __FILE__, __LINE__, 0 );

      if( PairInteractionMaskArray == NULL )
        {
        PLATFORM_ABORT(" PairInteractionMaskArray == NULL ");
        }

      // ASSIGN TO PERSISTANT STATE POINTER HELD BY OUTER LAYER
      *aPersistentStatePtr = (ExclusionMask_T *) PairInteractionMaskArray;

      int     PairInteractionMaskIndex     = 0;

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "IrreducibleFragmentPair(): PairInteractionMaskArraySize "
        << PairInteractionMaskArraySize
        << EndLogLine;

      //***** NOTE: THESE LOOPS MUST MUST MUST INDEX EXACTLY THE SAME AS THOSE USED LATER TO READ THE PairInteractionMaskArray.

      for(int SourceFragmentSiteIndex = 0;
              SourceFragmentSiteIndex < SourceFragmentSiteCount;
              SourceFragmentSiteIndex++ )
        {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        int SourceAbsSiteId =
            MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );

        // Scan the Target Fragment since that should be more local.
        for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1: 0 );
            TargetFragmentSiteIndex < TargetFragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
          BegLogLine( 0 ) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

          int TargetAbsSiteId =
            MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

          unsigned PairInteractionMask =
            MSD_IF::GetInteractionCodeMaskNsq( TargetAbsSiteId, SourceAbsSiteId );

          PairInteractionMaskArray[ PairInteractionMaskIndex ++ ] = PairInteractionMask;

          BegLogLine(0)
            << "IrreducibleFragmentPair(): "
            << " Interaction "
            << " Source " << SourceAbsSiteId
            << " Target " << TargetAbsSiteId
            << " PairIndex " << PairInteractionMaskIndex - 1
            << " Mask " << (void *) PairInteractionMask
            << EndLogLine;

          }
        }
      }
  };
#endif
