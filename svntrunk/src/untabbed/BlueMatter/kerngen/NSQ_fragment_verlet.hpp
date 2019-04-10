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
 * Purpose:         Manage loop driven NSQ Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         001001 BGF Created.
 *
 ***************************************************************************/

#ifndef __NSQ_FRAG_VERLET_HPP__
#define __NSQ_FRAG_VERLET_HPP__

#ifndef PKFXLOG_PLIMPTONLOOPS
#define PKFXLOG_PLIMPTONLOOPS ( 0 )
#endif

#ifndef PKFXLOG_NSQ_GUARD_ZONE_TUNE
#define PKFXLOG_NSQ_GUARD_ZONE_TUNE ( 1 )
#endif

#ifndef PKFXLOG_NSQ_INIT
#define PKFXLOG_NSQ_INIT      ( 0 )
#endif

#ifndef PKFXLOG_NSQTARGETFRAG
#define PKFXLOG_NSQTARGETFRAG 0
#endif

#ifndef PKFXLOG_TRIGGERSETTIMEFORFRAGMENT
#define PKFXLOG_TRIGGERSETTIMEFORFRAGMENT ( 0 )
#endif

#ifndef PKFXLOG_NSQSOURCEFRAG
#define PKFXLOG_NSQSOURCEFRAG 0
#endif

#ifndef PKFXLOG_NSQSOURCEFRAG1
#define PKFXLOG_NSQSOURCEFRAG1 ( 0 )
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

#ifdef NSQ_SINGLE_COMPUTE
#define NSC 1
#else
#define NSC 0
#endif

#define MAX_REAL_SPACE_TIME_STEP_TIME    9999999999.0

static TraceClient NSQ_RealSpace_Meat_Start;
static TraceClient NSQ_RealSpace_Meat_Finis;


// #include <BlueMatter/IFP.hpp>

#include <BlueMatter/UDF_BindingWrapper.hpp>

#include <BlueMatter/ExternalDatagram.hpp>

//******************************************************

//****************************************************************
static TraceClient NsqOuterFragmentLoopPoints;

#include <BlueMatter/NeighborList.hpp>

static NeighbourList NSQ_NeighbourList;

struct RealSpaceSignalSet
  {
  unsigned char mResetSignal;
  unsigned char mUpdateFragmentVerletListSignal;
  unsigned char mUpdateFragmentAssignmentListSignal;
  unsigned char mTuneGuardZoneSignal;
  };

template<class DynVarMgrIF, class IFP, class ORBManIF >
class NSQEngine
  {
    typedef IFP IFP0;

  public:
    typedef void *VoidPtr;
    typedef void **Void2Ptr;

    //********* FROM NB ****************/

    struct FragmentVerletPair
    {
      unsigned short    mTargetFragmentId;
      unsigned short    mSourceFragmentId;
      XYZ               mFragmentImageVector;
      ExclusionMask_T   mExclusionMask;
    };

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
        int                 mIntegratorLevel;
        int                 mCalculateVerletListSize;
        int                 mAllocatedFragmentVerletListSize;
        int                 mVerletListSize;
      // int                 mUpdateFragmentVerletList;
        int                 mReportForceOnSourceFlag;
        FragmentVerletPair* mFragmentVerletList;

        double              mOptGuardZone;
        double              mOptRealSpaceTimeStep;

        double              mDescendingSearchDelta;
        double              mGuardZoneTuningRealSpaceTimeSum;
        double              mTargetTmpGuardZone;

        int                 mOptTSCount;
        int                 mGuardZoneTuningTimeStepsPerGuardZone;
      // int                 mTuneGuardZone;

        GuardZoneReporting  mGuardZoneReporting[ 100 ];
        int                 mReportingIndex;
        int                 mReportOnce;

        int                 mUpdateFragVerletListFirstTime;

        FragmentVerletPair* mLastFragmentVerletList;
        int                 mLastFragmentVerletIndex;
      };


    // Every client port must allocate one of these structures.
    // It is returned as a handle by either Connect or send.
    // Calling wait shows that DynamicVariables are ready to be updated.
    class Client
      {
      public:

        ClientState mClientState;

//         void SetTuneGuardZoneSignal( int signal )
//         {
// #if !MSDDEF_DoPressureControl
//           mClientState.mTuneGuardZone = signal;
// #endif
//         }

//         void SetUpdateFragmentListSignal( int signal )
//         {
//           // Only set the signal if the signal is not already set ON
//           // The idea is that if the signal is already set, we do not want to unset it.
//           // We only unset the signal when we're done with updating the verlet list.

//           if( mClientState.mUpdateFragmentVerletList == 0 )
//             mClientState.mUpdateFragmentVerletList  = signal;
//         }

        void
        Connect( //NEED: think about whether to allow the volume or machine grid to be passed in here.
                 DynVarMgrIF          * aDynVarMgrIF,   //Type for this should come in as a template interface
                 RealSpaceSignalSet   & aSignalSet,
                 unsigned               aSimTick,
                 int                    aIntegratorLevel )
          {
          mClientState.mDynVarMgrIF              = aDynVarMgrIF;
          mClientState.mSimTick                  = aSimTick;
          mClientState.mIntegratorLevel          = aIntegratorLevel;
          mClientState.mFragmentVerletList       = NULL;
          mClientState.mCalculateVerletListSize  = 1;
          mClientState.mVerletListSize           = 0;
          aSignalSet.mUpdateFragmentVerletListSignal = 1;

          mClientState.mOptGuardZone             = aDynVarMgrIF->GetCurrentVerletGuardZone();
          mClientState.mOptRealSpaceTimeStep     = MAX_REAL_SPACE_TIME_STEP_TIME;
          mClientState.mOptTSCount               = 0;
          mClientState.mDescendingSearchDelta    = 0.1;
          aSignalSet. mTuneGuardZoneSignal      = 0;
          mClientState.mGuardZoneTuningTimeStepsPerGuardZone = 1;

          mClientState.mGuardZoneTuningRealSpaceTimeSum =  ( MAX_REAL_SPACE_TIME_STEP_TIME / Platform::Topology::GetAddressSpaceCount() ) - 1.0;

          mClientState.mReportingIndex                = 0;
          mClientState.mReportOnce                    = 1;
          mClientState.mUpdateFragVerletListFirstTime = 1;

          mClientState.mLastFragmentVerletList  = NULL;
          mClientState.mLastFragmentVerletIndex = 0;


          if(  NSC == 0 )
            mClientState.mReportForceOnSourceFlag   = 0;
          else
            mClientState.mReportForceOnSourceFlag   = 1;

          //BGF moved here from inside send loop Nov 20 2004
          IFP0::Init();
          }

        // The 'Send' method here will only be called when

        class IrreducibleFragmentOperandIF
          {
          public:

            int          mSimTick;
            int          mFragmentOrdinal;
            DynVarMgrIF *mDynVarMgrIF;
            int          mIntegratorLevel;


            IrreducibleFragmentOperandIF( int          aSimTick,
                                           int          aIntegratorLevel,
                                           DynVarMgrIF *aDynVarMgrIF ,
                                           int          aFragmentOrdinal )
              {
              mSimTick          = aSimTick;
              mIntegratorLevel  = aIntegratorLevel;
              mDynVarMgrIF      = aDynVarMgrIF;
              mFragmentOrdinal = aFragmentOrdinal;
              }

            inline int
            GetIrreducibleFragmentId()
              {
              int rc = mDynVarMgrIF->GetNthIrreducibleFragmentId( mFragmentOrdinal );
              return( rc );
              }

            static inline int
            GetIrreducibleFragmentId(DynVarMgrIF *aDynVarMgrIF, int          aFragmentOrdinal)
              {
              int rc = aDynVarMgrIF->GetNthIrreducibleFragmentId( aFragmentOrdinal );
              return( rc );
              }


            // OK OK... avoided a copy, but at the cost of exposing a pointer into otherwise encapsulated Dynamic Variable Manager
            //NEED TO THINK ABOUT THIS...
            inline
            XYZ *             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
            GetPositionPtr( int aIndex )
              {
              int base   = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
              XYZ * rc   =  mDynVarMgrIF->GetPositionPtr( base + aIndex ) ;
              return( rc );
              }

            inline
            XYZ
            GetPosition( int aIndex )
              {
              int base   = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
              XYZ * rc   =  mDynVarMgrIF->GetPositionPtr( base + aIndex ) ;
              return( *rc );
              }

            inline
            int GetSiteId( int aIndex )
              {
                int base = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
                return  ( base+aIndex );
              }

            static inline
            XYZ *             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
            GetPositionPtr( DynVarMgrIF *aDynVarMgrIF , int          aFragmentOrdinal, int aIndex )
              {
              int base   = aDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( aFragmentOrdinal );
              XYZ * rc   =  aDynVarMgrIF->GetPositionPtr( base + aIndex ) ;
              return( rc );
              }

            inline
            XYZ
            GetCenterOfMassPosition()
              {
              return mDynVarMgrIF->GetCenterOfMassPosition( mFragmentOrdinal );
              }

          };

        class FragmentPartialResultsIF
          {
          public:

            int          mSimTick;
            int          mFragmentOrdinal;
            DynVarMgrIF *mDynVarMgrIF;
            int          mIntegratorLevel;
            int          mRespaLevelOfFirstShell;


            #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
            int          mNullForceInteractionCount;
            #endif

            FragmentPartialResultsIF( int          aSimTick,
                                      int          aIntegratorLevel,
                                      DynVarMgrIF *aDynVarMgrIF ,
                                      int          aFragmentOrdinal )
              {
              mSimTick          = aSimTick;
              mIntegratorLevel  = aIntegratorLevel;
              mDynVarMgrIF      = aDynVarMgrIF;
              mFragmentOrdinal = aFragmentOrdinal;
              mRespaLevelOfFirstShell = MSD_IF::GetRespaLevelOfFirstShell();


              #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
              mNullForceInteractionCount = 0;
              #endif
              }

            inline
            void
            DisplayForce( int aAbsSiteId, int aShellId )
              {
              int respaIndex = mRespaLevelOfFirstShell+aShellId;

              assert( respaIndex >=0 && respaIndex < NUMBER_OF_RESPA_LEVELS );

    BegLogLine(1) 
      << "DisplayForce aAbsSiteId=" << aAbsSiteId
      << " aShellId=" << aShellId
      << " force=" << mDynVarMgrIF->GetForce( aAbsSiteId, mSimTick, respaIndex)
      << EndLogLine ;
      

              }

            inline
            void
            AddForce( int aAbsSiteId, int aShellId, const XYZ & aForce )
              {
              int respaIndex = mRespaLevelOfFirstShell+aShellId;

              assert( respaIndex >=0 && respaIndex < NUMBER_OF_RESPA_LEVELS );

              mDynVarMgrIF->AddForce( aAbsSiteId, mSimTick, respaIndex, aForce );

              #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
              if( aForce == XYZ::ZERO_VALUE() )
                mNullForceInteractionCount++;
              #endif
              }
          };

        class FragmentPartialResultsDummyIF
          {
          public:

            int          mSimTick;
            int          mFragmentOrdinal;
            DynVarMgrIF *mDynVarMgrIF;
            int          mIntegratorLevel;
            int          mRespaLevelOfFirstShell;


            #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
            int          mNullForceInteractionCount;
            #endif

            FragmentPartialResultsDummyIF( int          aSimTick,
                                      int          aIntegratorLevel,
                                      DynVarMgrIF *aDynVarMgrIF ,
                                      int          aFragmentOrdinal )
              {
              mSimTick          = aSimTick;
              mIntegratorLevel  = aIntegratorLevel;
              mDynVarMgrIF      = aDynVarMgrIF;
              mFragmentOrdinal = aFragmentOrdinal;
              mRespaLevelOfFirstShell = MSD_IF::GetRespaLevelOfFirstShell();
              }

            inline
            void
            AddForce( int aAbsSiteId, int aShellId, const XYZ & aForce )
              {
                return;
              }
          };

        typedef FragmentPartialResultsIF FragmentPartialResults_T;

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
                if ( ReportsEnergy )
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

            int                     mRespaLevel;
            int                     mNumberOfShells;
            double                  mSwitchLowerCutoff;
            double                  mSwitchDelta;

            inline
            void
            Init( int aRespaLevel )
              {
                  assert( aRespaLevel >= 0 && aRespaLevel < NUMBER_OF_RESPA_LEVELS );

                  mRespaLevel          = aRespaLevel;
                  mNumberOfShells      = aRespaLevel - MSD_IF::GetRespaLevelOfFirstShell() + 1;
                  assert( mNumberOfShells >= 1 && mNumberOfShells <= IFP0::NUM_SHELLS );

                  mSwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( mNumberOfShells - 1 );
                  mSwitchDelta       = MSD_IF::GetSwitchDeltaForShell( mNumberOfShells - 1 );

              }
          };

      void
      Send( int                 aSimTick, 
            RealSpaceSignalSet& aSignalSet )
        {
#pragma execution_frequency(very_low)
          // TimeValue StartRealSpaceTime = Platform::Clock::GetTimeValue();
        EmitTimeStamp( aSimTick, Tag::RealSpace_Begin, 1 );
        BegLogLine( PKFXLOG_NSQ_SEND )
          << "Send(): "
          << " SimTick "
          << aSimTick
          << EndLogLine;

          // mClientState.mDynVarMgrIF->incRealSpaceSend();
          // if( mClientState.mDynVarMgrIF->mNumberOfRealSpaceCalls == TIMESTEPS_TO_TUNE )
          //   mClientState.mDynVarMgrIF->mSampleFragmentTime=0;
          GlobalPartialResultsIF<0> GlobalPartialResultAxB;
          GlobalPartialResultAxB.Zero();

          GlobalInputParametersIF GlobalInputParameters;
          GlobalInputParameters.Init( 0 );

          //////////BGF move to init of this class to be called once per ts IFP0::Init();

          XYZ NSQ_Driver_VirialContribution;
          NSQ_Driver_VirialContribution.Zero();


          #ifdef USE_DYNAMIC_EXTENTS
            mClientState.mDynVarMgrIF->UpdateDynamicExtents();
          #endif

          TimeValue StartValue;
          TimeValue FinisValue;

          #if BLUEMATTER_STATS0
          int FragmentFragmentInteractionTotal = 0;
          int FragmentFragmentInteraction = 0;
          #endif

          #if BLUEMATTER_STATS0
          IFP0::IFPHelper0::ClearStaticCounters();
          #endif


          int count=0;
          int TotalCount=0;
          int TotalNullForceInteractionCount = 0;
          int NumLocalFragment = 0;

          int VLCount=0;

          TimeValue RealSpaceStartTime = Platform::Clock::GetTimeValue();

          if( (mClientState.mReportingIndex != 0) && mClientState.mReportOnce && !aSignalSet.mTuneGuardZoneSignal )
            {
              // Report the optimal guardzone here.
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

          /*
           * Create the fragment verlet list.
           */
          if( aSignalSet.mUpdateFragmentVerletListSignal )
            {
             if( !mClientState.mUpdateFragVerletListFirstTime )
               {
                 // Memory copy the fragment verlet list to the end of the buffer
                 unsigned int SpaceLeftInBuffer = ( mClientState.mAllocatedFragmentVerletListSize - mClientState.mVerletListSize );

                 int NumberOfChunks    = mClientState.mVerletListSize / SpaceLeftInBuffer;
                 int NumberOfRemaining = mClientState.mVerletListSize % SpaceLeftInBuffer;

                 int LastIndex  = mClientState.mVerletListSize;

                 for( int i=0; i < NumberOfChunks; i++ )
                   {
                     int SourceIndex = ( LastIndex - SpaceLeftInBuffer );
                     int SizeOfBlock = SpaceLeftInBuffer ;
                     int TargetIndex = LastIndex;


                     memcpy( &mClientState.mFragmentVerletList[ TargetIndex ],
                             &mClientState.mFragmentVerletList[ SourceIndex ],
                             SizeOfBlock * sizeof( FragmentVerletPair ) );

                     LastIndex -= SizeOfBlock;
                   }

                 if( NumberOfRemaining != 0 )
                   {
                     memcpy( &mClientState.mFragmentVerletList[ LastIndex ],
                             &mClientState.mFragmentVerletList[ LastIndex - NumberOfRemaining ],
                             NumberOfRemaining * sizeof( FragmentVerletPair ) );
                   }

                 mClientState.mLastFragmentVerletList = & mClientState.mFragmentVerletList[ LastIndex ];
                 mClientState.mLastFragmentVerletIndex = LastIndex;
               }

            if( aSignalSet.mTuneGuardZoneSignal )
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
                  aSignalSet.mTuneGuardZoneSignal = 0;
                  mClientState.mDynVarMgrIF->SetCurrentVerletGuardZone( mClientState.mOptGuardZone );

//                   if( Platform::Topology::GetAddressSpaceId == 0 )
//                     BegLogLine( PKFXLOG_NSQ_GUARD_ZONE_TUNE )
//                       << "****** Final Guard Zone ******"
//                       << " Optimal Guard Zone: " << mClientState.mOptGuardZone
//                       << " NumberOfTimeSteps: " << mClientState.mOptTSCount
//                       << EndLogLine;
                }

              if( Platform::Topology::GetAddressSpaceId() == 0 )
                {
//                   BegLogLine( PKFXLOG_NSQ_GUARD_ZONE_TUNE )
//                     << " Auto Tuning the guard zone. "
//                     << " SimTick=" << aSimTick
//                     << " CurrentGuardZone=" << mClientState.mDynVarMgrIF->GetCurrentGuardZone()
//                     << " DescendingSearchDelta=" << mClientState.mDescendingSearchDelta
//                     << " OptRealSpaceTimeStep="<< mClientState.mOptRealSpaceTimeStep
//                     << " OptGuardZone=" << mClientState.mOptGuardZone
//                     << " OptTSCount=" << mClientState.mOptTSCount
//                     << " GuardZoneTuningRealSpaceTimeSum=" << mClientState.mGuardZoneTuningRealSpaceTimeSum
//                     << " GuardZoneTuningTimeStepsPerGuardZone=" << mClientState.mGuardZoneTuningTimeStepsPerGuardZone
//                     << " RealSpacePerTS=" << RealSpacePerTS
//                     << EndLogLine;

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

            // We can run the algorithm either by precomputing the in-range fragments,
            // or the way we used to run it by iterating through all the fragments and deciding
            // about 'range' just before we call the IFP each time. It is faster to precompute,
            // but we never throw away any code ...
            NSQ_NeighbourList.Setup( ::DynVarMgrIF.mDynamicBoxDimensionVector,
                                     ::DynVarMgrIF.mDynamicBoxInverseDimensionVector ) ;

            unsigned int IrreducibleFragmentCount = mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount() ;

            double SwitchUpperCutoff = GlobalInputParameters.mSwitchLowerCutoff + GlobalInputParameters.mSwitchDelta ;
            double BoxTrace =
                ::DynVarMgrIF.mDynamicBoxDimensionVector.mX
              + ::DynVarMgrIF.mDynamicBoxDimensionVector.mY
              + ::DynVarMgrIF.mDynamicBoxDimensionVector.mZ  ;

            double InRangeCutoff = ( SwitchUpperCutoff < BoxTrace ) ? SwitchUpperCutoff : BoxTrace ;

            // double GuardZone = MSD_IF::GetGuardZoneWidth();
            double GuardZone = mClientState.mDynVarMgrIF->GetCurrentVerletGuardZone();
            double GuardZoneSquared = GuardZone * GuardZone;

            unsigned int FragmentPairIndex = 0;
            int CulledOutCounter = 0;

            // Need to update the starting positions so that guard zone tracking would
            // occur from here.
            // mClientState.mDynVarMgrIF->UpdateDisplacementStartPosit();
            
            for (int pSourceI = 0; pSourceI < IrreducibleFragmentCount; pSourceI++)
              {

              int SourceFragmentId = IrreducibleFragmentOperandIF::GetIrreducibleFragmentId(mClientState.mDynVarMgrIF,pSourceI);
              int SourceTagSiteIndex = MSD_IF::GetTagAtomIndex( SourceFragmentId );

              // Well, actually, we don't use either of these (except to put in the log message); instead we take the time
              // to work out proper minimum answers, it pays in the long run.
              #ifdef USE_DYNAMIC_EXTENTS
               // NOTE: requires access to dynamic extents - or they'd have to be generated on the fly
              double SourceFragmentExtent = mClientState.mDynVarMgrIF->GetFragmentExtent( SourceFragmentId );
              #else
              // NOTE - just uses nice, clean MSD constants.
              double SourceFragmentExtent = MSD_IF::GetFragmentExtent( SourceFragmentId );
              #endif

              int FragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceFragmentId );
              XYZ LowerLeft = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,0) ;
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
                  XYZ AtomLoc = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,pIndexI) ;
                  double dX = AtomLoc.mX ;
                  double dY = AtomLoc.mY ;
                  double dZ = AtomLoc.mZ ;
                  bX = min(bX,dX) ;
                  bY = min(bY,dY) ;
                  bZ = min(bZ,dZ) ;
                  cX = max(cX,dX) ;
                  cY = max(cY,dY) ;
                  cZ = max(cZ,dZ) ;
                  // LowerLeft = minXYZ(LowerLeft,AtomLoc) ;
                  // UpperRight = maxXYZ(UpperRight,AtomLoc) ;
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
                XYZ AtomLoc = *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,qIndexI) ;
                double e2 = ( AtomLoc - MidXYZ ).LengthSquared() ;
                e2max = max( e2max, e2 ) ;
                } /* endfor */

              BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                << "NSQ Fragment " << pSourceI
                << " bounds " << LowerLeft << UpperRight
                << " boundsize " << ( UpperRight - LowerLeft )
                << " centre " << MidXYZ
                << " extent " << sqrt( e2max ) << 0.5 * ( UpperRight - LowerLeft )
                << " tagext " << SourceFragmentExtent << *IrreducibleFragmentOperandIF::GetPositionPtr(mClientState.mDynVarMgrIF,pSourceI,SourceTagSiteIndex)
                << EndLogLine ;

              NSQ_NeighbourList.SetXYZE( pSourceI,
                                         MidXYZ,
                                         e2max,
                                         0.5 * ( UpperRight - LowerLeft ) ) ;
              }

            NSQ_NeighbourList.CompleteXYZE(IrreducibleFragmentCount) ;


            BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
              << "NSQ_frag::Send() IrreducibleFragmentCount=" << IrreducibleFragmentCount
              << EndLogLine;

            while( 1 )
              {
                BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
                  << "NSQ_frag::Send() aSignalSet.mUpdateFragmentVerletListSignal=" << aSignalSet.mUpdateFragmentVerletListSignal
                  << " mClientState.mVerletListSize=" << mClientState.mVerletListSize
                  << " mClientState.mCalculateVerletListSize=" << mClientState.mCalculateVerletListSize
                  << EndLogLine;

                // One way or another, we are finished with the O(N) work, and we start on the O(N*N) work
                for( int TargetI = 0; TargetI < IrreducibleFragmentCount; TargetI++ )
                  {
                  #pragma execution_frequency(very_high)
                  TotalCount += TargetI;

                  if( !mClientState.mDynVarMgrIF->IsFragmentLocal( TargetI ) )
                    continue;

                  NumLocalFragment++;

                  int TargetFragmentId         = TargetI; // TargetFragmentOperandIF.GetIrreducibleFragmentId();
                  int TargetMoleculeId         = MSD_IF::GetMoleculeId( TargetFragmentId );
                  int TargetFragmentTypeId     = MSD_IF::GetFragmentTypeId( TargetFragmentId );
                  int TargetMoleculeTypeId     = MSD_IF::GetMoleculeTypeId( TargetFragmentId );
                  int TargetWaterStatus        = MSD_IF::GetIrreducibleFragmentWaterStatus( TargetFragmentId );

                  double TargetFragmentExtent  = NSQ_NeighbourList.GetFragmentExtent(TargetI) ;
                  XYZ TargetFragmentCorner     = NSQ_NeighbourList.GetCorner(TargetI) ;

                  double AtomDistanceThreshold = InRangeCutoff + TargetFragmentExtent +  GuardZone;

                  XYZ AtomDistanceThresholdBox = TargetFragmentCorner.scalarAdd( InRangeCutoff ) ;
                  XYZ TPos = NSQ_NeighbourList.GetFragmentCentre(TargetI) ;

                  BegLogLine( PKFXLOG_NSQTARGETFRAG )
                    << "NSQ "
                    << " TargFragId "        << TargetFragmentId
                    << " TargFragTypeId "    << TargetFragmentTypeId
                    << " TargFragMolTypeId " << TargetMoleculeTypeId
                    << "NSQ Connect "
                    << " FragmentCount "
                    << IrreducibleFragmentCount
                    << EndLogLine;

                  // int ReportForceOnSourceFlag;
                  int SourceI;
                  if( NSC == 0 )
                    {
                      SourceI = 0;
                    }
                  else
                    {
                      SourceI = TargetI;
                    }

  #if defined(PERFORMANCE_PRECOMPUTE_RANGE_DEBUG)
                  int NeighbourListSize = NSQ_NeighbourList.ProduceAll( TPos,
                                                                        AtomDistanceThresholdBox,
                                                                        AtomDistanceThreshold,
                                                                        SourceI,
                                                                        IrreducibleFragmentCount
                                                                       ) ;
  #else
  #if defined(PERFORMANCE_PRECOMPUTE_RANGE_I)
                  int NeighbourListSize = NSQ_NeighbourList.iProduce( TPos,
                                                                      AtomDistanceThresholdBox,
                                                                      AtomDistanceThreshold,
                                                                      SourceI,
                                                                      IrreducibleFragmentCount
                                                                      ) ;

                  if( (TargetI % 100) == 0 )
                    {
                      BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
                        << "NSQ_frag::Send() NeighbourListSize=" << NeighbourListSize
                        << EndLogLine;
                    }
  #else
                  int NeighbourListSize = NSQ_NeighbourList.Produce( TPos,
                                                                     AtomDistanceThresholdBox,
                                                                     AtomDistanceThreshold,
                                                                     SourceI,
                                                                     IrreducibleFragmentCount
                                                                    ) ;
  #endif
  #endif
                  if( mClientState.mCalculateVerletListSize )
                    {
                      // NOTE: This is only an upper bound since some of the pair would
                      // be culled out later.
                      if ( (TargetI % 100) == 0 )
                        {
                        BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
                          << "NSQ_frag::Send() mClientState.mVerletListSize=" << mClientState.mVerletListSize
                          << " NeighbourListSize=" << NeighbourListSize
                          << EndLogLine;
                        }

                      mClientState.mVerletListSize += NeighbourListSize;
                    }
                  else
                    {
                      // At this point we have a created mClientState.mFragmentVerletList
                      // so let's fill it.

                      double TargetFragmentExtent = NSQ_NeighbourList.GetFragmentExtent( TargetI );

                      XYZ SiteTagB = NSQ_NeighbourList.GetFragmentCentre( TargetI );

                      double radius = InRangeCutoff + GuardZone + TargetFragmentExtent;
                      double wall   = SiteTagB.mZ + radius;

                      VLCount += NeighbourListSize;

                      for ( int NeighbourListIndex = 0 ; NeighbourListIndex < NeighbourListSize ; NeighbourListIndex += 1 )
                        {
                        int nSourceI = NSQ_NeighbourList.Get( NeighbourListIndex );

                        double SourceFragmentExtent = NSQ_NeighbourList.GetFragmentExtent( nSourceI );

                        XYZ SiteTagA = NSQ_NeighbourList.GetFragmentCentre( nSourceI );
                        XYZ SiteTagBImage ;
                        NearestImageInPeriodicVolume( SiteTagA, SiteTagB, SiteTagBImage ) ;
                        XYZ FragmentImageVector = SiteTagBImage - SiteTagB ;

                        ExclusionMask_T* ExclMaskPtr =  & mClientState.mFragmentVerletList[ FragmentPairIndex ].mExclusionMask;
                        mClientState.mDynVarMgrIF->CreateExclusionMask( TargetI,
                                                                        nSourceI,
                                                                        ExclMaskPtr );

                        mClientState.mFragmentVerletList[ FragmentPairIndex ].mTargetFragmentId = TargetI;
                        mClientState.mFragmentVerletList[ FragmentPairIndex ].mSourceFragmentId = nSourceI;
                        mClientState.mFragmentVerletList[ FragmentPairIndex ].mFragmentImageVector = FragmentImageVector;
                        FragmentPairIndex++;

                        if( FragmentPairIndex >= mClientState.mAllocatedFragmentVerletListSize )
                           PLATFORM_ABORT( "FragmentPairIndex overran allocation - note this can be caused by load balancing/timing" );
                        }
                    }
                  } /* Closes the for( Target I ) */


              BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
                << "NSQ_frag::Send() FragmentPairIndex=" << FragmentPairIndex
                << EndLogLine;

              if( mClientState.mCalculateVerletListSize )
                {
                  // Create the fragment verlet list
                  // Double the size of the list due to load balancing
                  mClientState.mAllocatedFragmentVerletListSize = 3 * mClientState.mVerletListSize;
                  pkNew( &mClientState.mFragmentVerletList, mClientState.mAllocatedFragmentVerletListSize, __FILE__, __LINE__ );

                  if( mClientState.mFragmentVerletList == NULL )
                    PLATFORM_ABORT( "ERROR:: Not enough memory for fragment verlet lists" );

                   mClientState.mCalculateVerletListSize = 0;
                }
              else
                {
                  // This breaks us out of the while(1) loop. In the case when we just obtained
                  // FragmentVerletListSize we want to iterate againt to actually fill the list;
                  mClientState.mVerletListSize = FragmentPairIndex;
                  break;
                }
            }
            // mClientState.mUpdateFragmentVerletList = 0;
            aSignalSet.mUpdateFragmentVerletListSignal = 0;
            mClientState.mUpdateFragVerletListFirstTime = 0;

            BegLogLine( 0 )
              << " NumLocalFragment=" << NumLocalFragment
              << " mClientState.mVerletListSize=" <<  mClientState.mVerletListSize
              << EndLogLine;
            } /* Closes the if( UpdateVetletLists ) */


        /************************************************************************************
         * arayshu: Now that we created the fragment verlet list above loop over the pair
         * arayshu: culling out the elements which made it into the list due to the
         * arayshu: guard zone.
         ***********************************************************************************/
          NSQ_RealSpace_Meat_Start.HitOE( PKTRACE_REAL_SPACE_MEAT,
                                          "RealSpaceMeat",
                                          Platform::Topology::GetAddressSpaceId(),
                                          NSQ_RealSpace_Meat_Start );

          BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
            << "NSQ::Send() mClientState.mVerletListSize = " << mClientState.mVerletListSize
            << EndLogLine;

          for ( int FragmentPairIndex = 0 ; FragmentPairIndex < mClientState.mVerletListSize; FragmentPairIndex++ )
            {
#pragma execution_frequency(very_high)
            #ifdef PK_PARALLEL
              StartValue = Platform::Clock::GetTimeValue();
            #endif

            // SourceI = NSQ_NeighbourList.Get(NeighbourListIndex) ;
            int SourceI = mClientState.mFragmentVerletList[ FragmentPairIndex ].mSourceFragmentId;
            int TargetI = mClientState.mFragmentVerletList[ FragmentPairIndex ].mTargetFragmentId;

            // printf( "%5d %5d %5d\n", aSimTick, TargetI, 1SourceI );

            XYZ              *FragmentImageVector  = & mClientState.mFragmentVerletList[ FragmentPairIndex ].mFragmentImageVector;
            ExclusionMask_T  *ExclusionMask        = & mClientState.mFragmentVerletList[ FragmentPairIndex ].mExclusionMask;


            // This is the body for 2 kinds of loop; either 'precomputed' in which case we IFP everything that comes in,
            // or 'not precomputed' in which case we make a determination rather messily on the fly. It would be more legible
            // to split this out into separate loops, but you need some time to make sure you get it right if you want to do
            // this restructuring.
            IrreducibleFragmentOperandIF TargetFragmentOperandIF( aSimTick,
                                                                  mClientState.mIntegratorLevel,
                                                                  mClientState.mDynVarMgrIF,
                                                                  TargetI );

            FragmentPartialResultsIF            TargetPartialResultsIF( aSimTick,
                                                                        mClientState.mIntegratorLevel,
                                                                        mClientState.mDynVarMgrIF,
                                                                        TargetI );

            IrreducibleFragmentOperandIF SourceFragmentOperandIF( aSimTick,
                                                                  mClientState.mIntegratorLevel,
                                                                  mClientState.mDynVarMgrIF,
                                                                  SourceI );


            FragmentPartialResultsIF       SourcePartialResultsIF( aSimTick,
                                                                   mClientState.mIntegratorLevel,
                                                                   mClientState.mDynVarMgrIF,
                                                                   SourceI );

            FragmentPartialResultsDummyIF       SourcePartialResultsIFDummy( aSimTick,
                                                                             mClientState.mIntegratorLevel,
                                                                             mClientState.mDynVarMgrIF,
                                                                             SourceI );


            int TargetFragmentId = TargetI; // TargetFragmentOperandIF.GetIrreducibleFragmentId();
            int TargetMoleculeId = MSD_IF::GetMoleculeId( TargetFragmentId );
            int TargetFragmentTypeId =  MSD_IF::GetFragmentTypeId( TargetFragmentId );
            int TargetMoleculeTypeId =  MSD_IF::GetMoleculeTypeId( TargetFragmentId );

            int SourceFragmentId = SourceFragmentOperandIF.GetIrreducibleFragmentId();
            int SourceMoleculeId = MSD_IF::GetMoleculeId( SourceFragmentId );
            int SourceFragmentTypeId =  MSD_IF::GetFragmentTypeId( SourceFragmentId );
            int SourceMoleculeTypeId =  MSD_IF::GetMoleculeTypeId( SourceFragmentId );

            // Cull out the fragments in the list which are there due to the guard zone


            BegLogLine( PKFXLOG_NSQSOURCEFRAG )
              << "NSQ "
              << " SrcFragId "        << SourceFragmentId
              << " SrcFragTypeId "    << SourceFragmentTypeId
              << " SrcFragMolTypeId " << SourceMoleculeTypeId
              << EndLogLine;

            XYZ IFP_VirialContribution;
            #if !defined(PERFORMANCE_PRECOMPUTE_RANGE)
            // If we don't have a precomputed list, then we will skip the call to IFP for out-of-range fragments.
            // In this case, we need a zero virial for the addition later which is done unconditionally.
            IFP_VirialContribution.Zero() ;
            #endif

#if 1 // test without specialization for no 1:4s - reducing speciallization costs about 15%
            if( SourceMoleculeId != TargetMoleculeId )
              {
              #ifdef EXTENT_CHECK
              if( !mClientState.mDynVarMgrIF->AreSitesWithinExtent(SourceFragmentId) )
                {
                  BegLogLine(1) << "Site outside the extent for source fragment id: " << SourceFragmentId << EndLogLine;
                  PLATFORM_ABORT("Sites outside the extent.");
                }

              if( !mClientState.mDynVarMgrIF->AreSitesWithinExtent(TargetFragmentId) )
                {
                  BegLogLine(1) << "Site outside the extent for target fragment id: " << TargetFragmentId << EndLogLine;
                  PLATFORM_ABORT("Sites outside the extent.");
                }
              #endif

              #if BLUEMATTER_STATS1
                FragmentFragmentInteraction++;
              #endif

              BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                << "NSQ running "
                << " SourceI " << SourceI
                << EndLogLine;

              // Much is known - no 14s, no exclusions - IFP state
              IFP0::Execute( aSimTick,
                             GlobalInputParameters,
                             TargetFragmentOperandIF,
                             SourceFragmentOperandIF,
                             TargetPartialResultsIF,
                             SourcePartialResultsIF,
                             GlobalPartialResultAxB,
                             mClientState.mReportForceOnSourceFlag,
                             NULL,
                             *FragmentImageVector
                             );
              }
            else
#endif // test wihtout code specialization
              {

              #if BLUEMATTER_STATS1
                FragmentFragmentInteraction++;
              #endif

              IFP0::Execute( aSimTick,
                             GlobalInputParameters,
                             TargetFragmentOperandIF,
                             SourceFragmentOperandIF,
                             TargetPartialResultsIF,
                             SourcePartialResultsIF,
                             GlobalPartialResultAxB,
                             mClientState.mReportForceOnSourceFlag,
                             ExclusionMask,
                             *FragmentImageVector
                             );

              }

             #ifdef PK_PARALLEL
             if( mClientState.mDynVarMgrIF->mSampleFragmentTime )
               {
               FinisValue = Platform::Clock::GetTimeValue();
               mClientState.mDynVarMgrIF->AddTimeForFragment( TargetI, FinisValue - StartValue );
               }
             #endif
            } /* Ends the for ( int FragmentPairIndex = 0 ; FragmentPairIndex < mClientState.mVerletListSize; FragmentPairIndex++ ) */

          NSQ_RealSpace_Meat_Finis.HitOE( PKTRACE_REAL_SPACE_MEAT,
                                          "RealSpaceMeat",
                                          Platform::Topology::GetAddressSpaceId(),
                                          NSQ_RealSpace_Meat_Finis );


          if( aSignalSet.mTuneGuardZoneSignal )
            {
              TimeValue RealSpaceTimeFinis = Platform::Clock::GetTimeValue();
              TimeValue TimeDiff = RealSpaceTimeFinis - RealSpaceStartTime;

              double TimeDelta = Platform::Clock::ConvertTimeValueToDouble( TimeDiff );

              // printf("TimeDelta=%.16f\n", TimeDelta);

              mClientState.mGuardZoneTuningRealSpaceTimeSum += TimeDelta;
              mClientState.mGuardZoneTuningTimeStepsPerGuardZone++;
            }


          // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
          unsigned TimeStep = aSimTick / MSD::SimTicksPerTimeStep;

          #if BLUEMATTER_STATS0
          BegLogLine( BLUEMATTER_STATS0 )
            << "NSQ STAT0 "
            << " SimTick "  << aSimTick
            << " TimeStep " << TimeStep
            << " Total Fragment Interaction Count "
            << FragmentFragmentInteractionTotal
            << EndLogLine;
          #endif

          #if BLUEMATTER_STATS_NULL_FORCE_INTERACTION_COUNT
            BegLogLine( BLUEMATTER_STATS1 )
              << "NSQ STAT1 "
              << " SimTick " << aSimTick
              << " TimeStep " << TimeStep
              << " Total "
              << " NullInteractionCount "
              << TotalNullForceInteractionCount
              << EndLogLine;
          #endif

          #if BLUEMATTER_STATS0
          IFP0::IFPHelper0::ReportStaticCounters();
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
            {
              int RespaLevelOfFirstShell = MSD_IF::GetRespaLevelOfFirstShell();
              for(int shellId=0; shellId< IFP0::NUM_SHELLS; shellId++ )
                {
                mClientState.mDynVarMgrIF->AddVirial( RespaLevelOfFirstShell+shellId,
                                                      GlobalPartialResultAxB.mVirialByShell[ shellId ] );
                }
            }
          #endif

          EmitTimeStamp(aSimTick, Tag::RealSpace_End, 1 );
          }

        void
        Wait( int aSimTick )
          {
          // Loops better be done, no?
          return ;
          }

      };
  };


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


    // Create a compiler recongnized instance of the Client Interface - should be zero byts.
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
