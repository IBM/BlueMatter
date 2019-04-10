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

#define HAND_XYZ 0
#define HAND_DVM 0
#define HAND_SQRT 0

/////////THIS VERSION DOES NOT CALL IFP... IFP HAS BEEN INLINED INTO THIS CODE
/////////***** THE FOLLOWING HAND CHANGES TO THE MSD WERE REQUIRED TO MAKE THIS WORK

////// XYZ  *StartingVelocities;
////// XYZ  *StartingPositions;
////// int NoseHooverRestart;
////// #include <BlueMatter/DVSFile.hpp>
////// ////#include <BlueMatter/IFPSimple.hpp>
////// //#include <BlueMatter/IFPTiled.hpp>
////// #include <BlueMatter/NSQ_site_verlet.hpp>
////// //#include <BlueMatter/NSQ_fragment_verlet.hpp>
////// ////typedef IFPSimple< NsqUdfGroup,
////// ////                         UDF_SwitchFunction,
////// ////                         UDF_SwitchFunctionForce,
////// ////                         NUMBER_OF_SHELLS,
////// ////                      1
////// ////                       > IFP_TYPE;
//////
////// typedef NSQAnchor<
//////                   DynamicVariableManager,
//////                   NsqUdfGroup,  /////////IFP_TYPE
//////                   UDF_SwitchFunction,
//////                   UDF_SwitchFunctionForce,
//////                   NUMBER_OF_SHELLS
//////                   > PNBE_TYPE;
//////
//////
//////


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
#define NSQ_NO_SINGLE_COMPUTE 1
#else
#define NSQ_NO_SINGLE_COMPUTE 0
#endif


#ifndef PKFXLOG_PAIRS_IN_RANGE
#define PKFXLOG_PAIRS_IN_RANGE ( 0 )
#endif

#define MAX_REAL_SPACE_TIME_STEP_TIME    9999999999.0

TraceClient NSQ_RealSpace_Meat_Start;
TraceClient NSQ_RealSpace_Meat_Finis;

#include <BlueMatter/UDF_BindingWrapper.hpp>

#include <BlueMatter/ExternalDatagram.hpp>

//******************************************************
//****************************************************************
TraceClient NsqOuterFragmentLoopPoints;

#include <BlueMatter/NeighborList.hpp>

#include <BlueMatter/IFPHelper.hpp>

NeighbourList NSQ_NeighbourList;

template<class DynVarMgrIF,
         class NsqUdfGroup,
         class SwitchFunctionEnergyAdapter,
         class SwitchFunctionForceAdapter,
         int   SHELL_COUNT >
class NSQEngine
  {
  public:
    //********* FROM NB ****************/

    struct FragmentVerletPair
      {
      unsigned short    mTargetFragmentId;
      unsigned short    mSourceFragmentId;
      XYZ               mFragmentImageVector;
      ExclusionMask_T   mExclusionMask;
      };


    struct tSiteVerletListEntry
      {
      int mTargetAbsSiteId;
      int mSourceAbsSiteId;
      XYZ mFragmentImageVector;
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
        DynVarMgrIF *          mDynVarMgrIF;
        int                    mSimTick;
        int                    mIntegratorLevel;
        int                    mCalculateVerletListSize;
        int                    mVerletListSize;
        int                    mUpdateFragmentVerletList;

        int                    mAllocatedFragmentVerletListSize;
        FragmentVerletPair *   mFragmentVerletList;

        int                    mAllocatedSiteVerletListSize;
        tSiteVerletListEntry * mSiteVerletList;
        int                    mSiteVerletListSize;


        double                 mOptGuardZone;
        double                 mOptRealSpaceTimeStep;

        double                 mDescendingSearchDelta;
        double                 mGuardZoneTuningRealSpaceTimeSum;
        double                 mTargetTmpGuardZone;

        int                    mOptTSCount;
        int                    mGuardZoneTuningTimeStepsPerGuardZone;
        int                    mTuneGuardZone;

        GuardZoneReporting     mGuardZoneReporting[ 100 ];
        int                    mReportingIndex;
        int                    mReportOnce;

        int                    mUpdateFragVerletListFirstTime;

        FragmentVerletPair *   mLastFragmentVerletList;
        int                    mLastFragmentVerletIndex;
      };


    // Every client port must allocate one of these structures.
    // It is returned as a handle by either Connect or send.
    // Calling wait shows that DynamicVariables are ready to be updated.
    class Client
      {
      public:

        ClientState mClientState;

        void SetTuneGuardZoneSignal( int signal )
          {
          #if !MSDDEF_DoPressureControl
            mClientState.mTuneGuardZone = signal;
          #endif
          }

        void SetUpdateFragmentListSignal( int signal )
          {
          // Only set the signal if the signal is not already set ON
          // The idea is that if the signal is already set, we do not want to unset it.
          // We only unset the signal when we're done with updating the verlet list.

          if( mClientState.mUpdateFragmentVerletList == 0 )
            mClientState.mUpdateFragmentVerletList  = signal;
          }

        void
        Connect( DynVarMgrIF          * aDynVarMgrIF,   //Type for this should come in as a template interface
                 unsigned               aSimTick,
                 int                    aIntegratorLevel )
          {
          mClientState.mDynVarMgrIF              = aDynVarMgrIF;
          mClientState.mSimTick                  = aSimTick;
          mClientState.mIntegratorLevel          = aIntegratorLevel;
          mClientState.mFragmentVerletList       = NULL;
          mClientState.mCalculateVerletListSize  = 1;
          mClientState.mVerletListSize           = 0;
          mClientState.mUpdateFragmentVerletList = 1;

          mClientState.mSiteVerletListSize       = 0;

          mClientState.mOptGuardZone             = aDynVarMgrIF->GetCurrentVerletGuardZone();
          mClientState.mOptRealSpaceTimeStep     = MAX_REAL_SPACE_TIME_STEP_TIME;
          mClientState.mOptTSCount               = 0;
          mClientState.mDescendingSearchDelta    = 0.1;
          mClientState.mTuneGuardZone            = 0;
          mClientState.mGuardZoneTuningTimeStepsPerGuardZone = 1;

          mClientState.mGuardZoneTuningRealSpaceTimeSum =  ( MAX_REAL_SPACE_TIME_STEP_TIME / Platform::Topology::GetAddressSpaceCount() ) - 1.0;

          mClientState.mReportingIndex                = 0;
          mClientState.mReportOnce                    = 1;
          mClientState.mUpdateFragVerletListFirstTime = 1;

          mClientState.mLastFragmentVerletList  = NULL;
          mClientState.mLastFragmentVerletIndex = 0;

          }

        typedef typename NsqUdfGroup::UDF0_Bound UDF0_Bound;
        typedef typename NsqUdfGroup::UDF1_Bound UDF1_Bound;
        typedef typename NsqUdfGroup::UDF2_Bound UDF2_Bound;

        typedef IFPHelper< NsqUdfGroup > IFPHelper0;

        typedef NsqUdfGroup NsqUdfGroup0;

        enum { NUM_SHELLS = SHELL_COUNT };

        template<int ORDINAL, class UDF_Bound, class NBVM_T>
        class ExecuteWrapper
          {
          public:
            static
            inline
            void
            Execute( NBVM_T&         aNbvm,
                     int&            aSourceId,
                     int&            aTargetId,
                     XYZ&            aForce,
                     double&         aNewEnergy,
                     XYZ&            aVirial
                     )
              {
              aNewEnergy = 0.0 ;
              aNbvm.SetCurrentUdfOrdinal( ORDINAL );
              if( UDF_Bound::UDF::Code != -1 )
                {
                IFPHelper0::DriveOne<UDF_Bound::UDF::NsqType, UDF_Bound, NBVM_T>::Execute( aNbvm, aSourceId, aTargetId );

                /****************************************************************/
                // Switch on the energies according to ForceSwitched = Force * S - UnitVector * Energy * dSdr

                // If we're doing the respa correction udf, then we are in an outer timestep
                // situation therefore we need to switch in the energy.
                SwitchFunctionEnergyAdapter::Execute( aNbvm );

                aForce     = aNbvm.GetForce( NBVM_T::SiteA );
                aNewEnergy = aNbvm.GetEnergy();

                if( UDF_Bound::UDF::ReportsVirial )
                  aVirial = aNbvm.GetVirial( ORDINAL );
                }
              }
          };


        inline
        XYZ *             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
        GetPositionPtr( int aFragmentOrdinal, int aIndex )
          {
          int base   = mClientState.mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( aFragmentOrdinal );
          XYZ * rc   = mClientState.mDynVarMgrIF->GetPositionPtr( base + aIndex ) ;
          return( rc );
          }

        void
        Send( int aSimTick, int aRespaLevel )
          {
          EmitTimeStamp( aSimTick, Tag::RealSpace_Begin, 1 );

          BegLogLine( PKFXLOG_NSQ_SEND )
            << "Send(): "
            << " SimTick "
            << aSimTick
            << EndLogLine;

           ////mClientState.mDynVarMgrIF->incRealSpaceSend();
           ////if( mClientState.mDynVarMgrIF->mNumberOfRealSpaceCalls == TIMESTEPS_TO_TUNE )
           ////  mClientState.mDynVarMgrIF->mSampleFragmentTime=0;


          BegLogLine(0)
            <<"Send(): "
            << "mSampleFragmentTime flag "
            << mClientState.mDynVarMgrIF->mSampleFragmentTime
            << " mNumberOfRealSpaceCalls "
            << mClientState.mDynVarMgrIF->mNumberOfRealSpaceCalls
            << " of "
            << TIMESTEPS_TO_TUNE
            << EndLogLine;


          assert( NUM_SHELLS == 1 );
          const int SHELL_INDEX_IS_ZERO = 0;

          // intend to move these to an init
          double mSwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( SHELL_INDEX_IS_ZERO );
          double mSwitchDelta       = MSD_IF::GetSwitchDeltaForShell( SHELL_INDEX_IS_ZERO  );
          double mSwitchUpperCutoff   = mSwitchLowerCutoff + mSwitchDelta ;
          // These are the switch parameters for the shell at the CURRENT respa level
          double mOffRegion                   = mSwitchLowerCutoff + mSwitchDelta;
          double mOffRegionSquared            = mOffRegion * mOffRegion;

          // Where should this really be??????
          IFPHelper0::Init();

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
            IFPHelper0::ClearStaticCounters();
          #endif

          int count                          = 0;
          int TotalCount                     = 0;
          int TotalNullForceInteractionCount = 0;
          int NumLocalFragment               = 0;
          int VLCount                        = 0;

          TimeValue RealSpaceStartTime = Platform::Clock::GetTimeValue();

          if( (mClientState.mReportingIndex != 0) && mClientState.mReportOnce && !mClientState.mTuneGuardZone )
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
          if( mClientState.mUpdateFragmentVerletList )
            {

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

//              if( Platform::Topology::GetAddressSpaceId == 0 )
//                {
//                BegLogLine( PKFXLOG_NSQ_GUARD_ZONE_TUNE )
//                  << "****** Final Guard Zone ******"
//                  << " Optimal Guard Zone: " << mClientState.mOptGuardZone
//                  << " NumberOfTimeSteps: " << mClientState.mOptTSCount
//                  << EndLogLine;
//                }
                }

              if( Platform::Topology::GetAddressSpaceId() == 0 )
                {
//              BegLogLine( PKFXLOG_NSQ_GUARD_ZONE_TUNE )
//                << " Auto Tuning the guard zone. "
//                << " SimTick=" << aSimTick
//                << " CurrentGuardZone=" << mClientState.mDynVarMgrIF->GetCurrentGuardZone()
//                << " DescendingSearchDelta=" << mClientState.mDescendingSearchDelta
//                << " OptRealSpaceTimeStep="<< mClientState.mOptRealSpaceTimeStep
//                << " OptGuardZone=" << mClientState.mOptGuardZone
//                << " OptTSCount=" << mClientState.mOptTSCount
//                << " GuardZoneTuningRealSpaceTimeSum=" << mClientState.mGuardZoneTuningRealSpaceTimeSum
//                << " GuardZoneTuningTimeStepsPerGuardZone=" << mClientState.mGuardZoneTuningTimeStepsPerGuardZone
//                << " RealSpacePerTS=" << RealSpacePerTS
//                << EndLogLine;

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
              } // End of tune guard zone block.

            // We can run the algorithm either by precomputing the in-range fragments,
            // or the way we used to run it by iterating through all the fragments and deciding
            // about 'range' just before we call the IFP each time. It is faster to precompute,
            // but we never throw away any code ...

            NSQ_NeighbourList.Setup( ::DynVarMgrIF.mDynamicBoxDimensionVector,
                                     ::DynVarMgrIF.mDynamicBoxInverseDimensionVector ) ;

            unsigned int IrreducibleFragmentCount = mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount() ;

            // WHAT THE HELL IS THIS BoxTrace stuff????
            double BoxTrace =
                ::DynVarMgrIF.mDynamicBoxDimensionVector.mX
              + ::DynVarMgrIF.mDynamicBoxDimensionVector.mY
              + ::DynVarMgrIF.mDynamicBoxDimensionVector.mZ  ;

            double InRangeCutoff = ( mSwitchUpperCutoff < BoxTrace ) ? mSwitchUpperCutoff : BoxTrace ;

            double GuardZone = mClientState.mDynVarMgrIF->GetCurrentVerletGuardZone();

            unsigned int FragmentPairIndex = 0;
            unsigned int SitePairIndex = 0;
            int CulledOutCounter = 0;

            // Need to update the starting positions so that guard zone tracking would
            // occur from here.
            // GUARD ZONE TRACKING SHOULD BE DONE AT UPDATE POSITS AND A SIGNAL SHOULD BE PROPOGATED ON VIOLATION
            mClientState.mDynVarMgrIF->UpdateDisplacementStartPosit();

            for (int pSourceI = 0; pSourceI < IrreducibleFragmentCount; pSourceI++)
              {

              int SourceFragmentId = mClientState.mDynVarMgrIF->GetNthIrreducibleFragmentId( pSourceI );

              int SourceTagSiteIndex = MSD_IF::GetTagAtomIndex( SourceFragmentId );

              int FragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceFragmentId );
              XYZ LowerLeft = * GetPositionPtr( pSourceI, 0) ;
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
                  XYZ AtomLoc = * GetPositionPtr( pSourceI, pIndexI) ;
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
                XYZ AtomLoc = * GetPositionPtr(pSourceI,qIndexI) ;
                double e2 = ( AtomLoc - MidXYZ ).LengthSquared() ;
                e2max = max( e2max, e2 ) ;
                } /* endfor */

              BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                << "NSQ Fragment " << pSourceI
                << " bounds " << LowerLeft << UpperRight
                << " boundsize " << ( UpperRight - LowerLeft )
                << " centre " << MidXYZ
                << " extent " << sqrt( e2max ) << 0.5 * ( UpperRight - LowerLeft )
                ///////<< " tagext " << SourceFragmentExtent << * GetPositionPtr(pSourceI,SourceTagSiteIndex)
                << EndLogLine ;

              NSQ_NeighbourList.SetXYZE( pSourceI,
                                         MidXYZ,
                                         e2max,
                                         0.5 * ( UpperRight - LowerLeft ) ) ;
              }

            NSQ_NeighbourList.CompleteXYZE( IrreducibleFragmentCount ) ;

            BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
              << "NSQ_frag::Send() IrreducibleFragmentCount=" << IrreducibleFragmentCount
              << EndLogLine;


            // Neighbor list is now generated.
            // This loop creates the verlet entrys for each interacting pair of frags
            // on the neighbor list.
            while( 1 )
              {
              BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
                << "NSQ_frag::Send() mClientState.mUpdateFragmentVerletList=" << mClientState.mUpdateFragmentVerletList
                << " mClientState.mVerletListSize=" << mClientState.mVerletListSize
                << " mClientState.mCalculateVerletListSize=" << mClientState.mCalculateVerletListSize
                << EndLogLine;

              // One way or another, we are finished with the O(N) work, and we start on the O(N*N) work
              for( int TargetI = 0; TargetI < IrreducibleFragmentCount; TargetI++ )
                {
                #pragma execution_frequency(very_high)
                TotalCount += TargetI;

                // If not 'Local' the fragment's interactions are not handled on this node.
                if( !mClientState.mDynVarMgrIF->IsFragmentLocal( TargetI ) )
                  continue;

                NumLocalFragment++;

                double TargetFragmentExtent  = NSQ_NeighbourList.GetFragmentExtent(TargetI) ;
                XYZ TargetFragmentCorner     = NSQ_NeighbourList.GetCorner(TargetI) ;

                double AtomDistanceThreshold = InRangeCutoff + TargetFragmentExtent +  GuardZone;

                XYZ AtomDistanceThresholdBox = TargetFragmentCorner.scalarAdd( InRangeCutoff ) ;
                XYZ TPos                     = NSQ_NeighbourList.GetFragmentCentre(TargetI) ;


                int SourceI = TargetI;

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
                  mClientState.mVerletListSize += NeighbourListSize;
                  mClientState.mSiteVerletListSize += 16 * NeighbourListSize;
                  }
                else
                  {
                  // At this point we have a created mClientState.mFragmentVerletList
                  // so let's fill it.

                  double TargetFragmentExtent = NSQ_NeighbourList.GetFragmentExtent( TargetI );

                  XYZ SiteTagB = NSQ_NeighbourList.GetFragmentCentre( TargetI );

                  VLCount += NeighbourListSize;

                  for ( int NeighbourListIndex = 0 ; NeighbourListIndex < NeighbourListSize ; NeighbourListIndex += 1 )
                    {
                    int nSourceI = NSQ_NeighbourList.Get( NeighbourListIndex );

                    XYZ SiteTagA = NSQ_NeighbourList.GetFragmentCentre( nSourceI );
                    XYZ SiteTagBImage ;
                    NearestImageInPeriodicVolume( SiteTagA, SiteTagB, SiteTagBImage ) ;
                    XYZ FragmentImageVector = SiteTagBImage - SiteTagB ;

                    // Must do each interaction between source frag and target frag once and only once
                    int SourceIsTargetFlag = TargetI == nSourceI;

                    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( nSourceI );
                    int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );

                    for(int SourceFragmentSiteIndex = 0;
                        SourceFragmentSiteIndex < SourceFragmentSiteCount;
                        SourceFragmentSiteIndex++ )
                      {
                      BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

                      // Scan the Target Fragment since that should be more local.
                      for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1 : 0 );
                          TargetFragmentSiteIndex < TargetFragmentSiteCount;
                          TargetFragmentSiteIndex++ )
                        {
                        BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

                        // The following is probably a very expensive way to find out the AbsSiteId's
                        // - these could be carried in with the Src/TrgFragmentOperandIF
                        int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( nSourceI, SourceFragmentSiteIndex );
                        int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetI,  TargetFragmentSiteIndex );

                        if( MSD_IF::AreSitesInExcIncList( SourceAbsSiteId, TargetAbsSiteId ) )
                          continue;

                        TotalPairOpsCounter.Increment();

                        // this should have been on the exclusion list... but it seems site identity ops are not excl
                        if( SourceAbsSiteId == TargetAbsSiteId )
                          continue;

                        mClientState.mSiteVerletList[ SitePairIndex ].mTargetAbsSiteId     = TargetAbsSiteId;
                        mClientState.mSiteVerletList[ SitePairIndex ].mSourceAbsSiteId     = SourceAbsSiteId;
                        mClientState.mSiteVerletList[ SitePairIndex ].mFragmentImageVector = FragmentImageVector;
                        SitePairIndex++;
                        if( SitePairIndex >= mClientState.mAllocatedSiteVerletListSize )
                           PLATFORM_ABORT( "SitePairIndex overran allocation" );
                        #ifdef PK_PARALLEL
#if 0
                          if( mClientState.mDynVarMgrIF->mSampleFragmentTime )
                            {
                            Platform::Clock::TimeValue tv;
                            tv.Seconds = 1;
                            mClientState.mDynVarMgrIF->AddTimeForFragment( TargetI, tv );
                            }
#endif
                        #endif

                        }
                      }
                    }
                  }
                } /* Closes the for( Target I ) */


                BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
                  << "NSQ_frag::Send() FragmentPairIndex " << FragmentPairIndex
                  << " SitePairIndex " << SitePairIndex
                  << EndLogLine;

                if( mClientState.mCalculateVerletListSize )
                  {
                  // Alloc the site verlet list. The 16 is based on a frag size of 4 yielding up 16 site-site interactions.
                  mClientState.mAllocatedSiteVerletListSize =  2 * 16 * mClientState.mVerletListSize ;
                  pkNew( &mClientState.mSiteVerletList, mClientState.mAllocatedSiteVerletListSize, __FILE__, __LINE__ );

                  if( mClientState.mSiteVerletList == NULL )
                    PLATFORM_ABORT( "ERROR:: Not enough memory for site verlet lists" );

                   mClientState.mCalculateVerletListSize = 0;
                  }
                else
                  {
                  // This breaks us out of the while(1) loop. In the case when we just obtained
                  // FragmentVerletListSize we want to iterate againt to actually fill the list;
                  mClientState.mVerletListSize     = FragmentPairIndex;
                  mClientState.mSiteVerletListSize = SitePairIndex;
                  break;
                  }
                } // end while(1)

              mClientState.mUpdateFragmentVerletList = 0;
              mClientState.mUpdateFragVerletListFirstTime = 0;

              BegLogLine( 0 )
                << " NumLocalFragment=" << NumLocalFragment
                << " mClientState.mVerletListSize=" <<  mClientState.mVerletListSize
                << EndLogLine;
              } /* Closes the if( UpdateVetletLists ) */


          BegLogLine( 0 )
            << " VLCount= " << VLCount
            << EndLogLine;

          /************************************************************************************
           * arayshu: Now that we created the fragment verlet list above loop over the pair
           * arayshu: culling out the elements which made it into the list due to the
           * arayshu: guard zone.
           ***********************************************************************************/

          NSQ_RealSpace_Meat_Start.HitOE( PKTRACE_REAL_SPACE_MEAT,
                                          "NSQ_RealSpace_Meat_Start",
                                          0,
                                          NSQ_RealSpace_Meat_Start );

          BegLogLine( PKFXLOG_NSQ_VERLET_LIST_DEBUG )
            << "NSQ::Send() mClientState.mVerletListSize = " << mClientState.mVerletListSize
            << EndLogLine;


          //// SHOULD BE THIS BUT THEN LATER CAN"T USE CONST SUBSCRIPTS double GrandTotalEnergyOnUdf[ NsqUdfGroup0::ActiveUDFCount ];
          double GrandTotalEnergyOnUdf[ 3 ];   // 3 IS THE NUMBER OF POSSIBLE ACTIVE UDFs - MAGIC NUMBER SHOULD COME FROM NsqUdfGroup
          for( int i = 0; i < NsqUdfGroup0::ActiveUDFCount; i++ )
            GrandTotalEnergyOnUdf[ i ] = 0.0;

          XYZ  GrandTotalVirial;
          GrandTotalVirial.Zero();

          NBVMX nbvm;
          nbvm.SetSimTick( aSimTick );

          // Make sure that the force is put on the last respa level
          nbvm.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );

#define VECTOR_LEN 20
          struct tVectorPrime
            {
            XYZ    mVector;
            double mDistanceSquared;
            double mInverseDistance;
            };

          #ifdef PK_PARALLEL
            int FirstTargetAbsSiteId = mClientState.mSiteVerletList[ 0 ].mTargetAbsSiteId;
            int CurrentFragId        = mClientState.mDynVarMgrIF->mSiteIdToFragmentMap[ FirstTargetAbsSiteId ];
            StartValue = Platform::Clock::GetTimeValue();
          #endif

          for(int OuterSitePairIndex = 0;
                  OuterSitePairIndex <  ( mClientState.mSiteVerletListSize + VECTOR_LEN - 1 );
                  OuterSitePairIndex += VECTOR_LEN )
            {
            tVectorPrime VectorPrime[ VECTOR_LEN ];
            int          VectorLiveCount = 0;

if( mClientState.mDynVarMgrIF->mSampleFragmentTime )
  {
  FinisValue      = Platform::Clock::GetTimeValue();
  mClientState.mDynVarMgrIF->AddTimeForFragment( CurrentFragId,  FinisValue - StartValue );
  }


            ////// BegLogLine(1) << "OuterSitePairIndex " << OuterSitePairIndex << EndLogLine;

            for( int i = 0; i < VECTOR_LEN; i++ )
              {
              if( (OuterSitePairIndex + i ) <  mClientState.mSiteVerletListSize )
                {
                int SourceAbsSiteId = mClientState.mSiteVerletList[ OuterSitePairIndex + i ].mSourceAbsSiteId;
                int TargetAbsSiteId = mClientState.mSiteVerletList[ OuterSitePairIndex + i ].mTargetAbsSiteId;

#if HAND_DVM
                XYZ SiteA =  mClientState.mDynVarMgrIF->mPosit[ SourceAbsSiteId ];
                XYZ SiteB =  mClientState.mDynVarMgrIF->mPosit[ TargetAbsSiteId ];
#else
                XYZ SiteA      = * mClientState.mDynVarMgrIF->GetPositionPtr( SourceAbsSiteId ) ;
                XYZ SiteB      = * mClientState.mDynVarMgrIF->GetPositionPtr( TargetAbsSiteId ) ;
#endif

#if HAND_XYZ
                XYZ SiteImageB;
                SiteImageB.mX = SiteB.mX + mClientState.mSiteVerletList[ OuterSitePairIndex + i ].mFragmentImageVector.mX;
                SiteImageB.mY = SiteB.mY + mClientState.mSiteVerletList[ OuterSitePairIndex + i ].mFragmentImageVector.mY;
                SiteImageB.mZ = SiteB.mZ + mClientState.mSiteVerletList[ OuterSitePairIndex + i ].mFragmentImageVector.mZ;

                VectorPrime[ i ].mVector.mX         =  SiteA.mX - SiteImageB.mX ;
                VectorPrime[ i ].mVector.mY         =  SiteA.mY - SiteImageB.mY ;
                VectorPrime[ i ].mVector.mZ         =  SiteA.mZ - SiteImageB.mZ ;
#else
                XYZ SiteImageB = SiteB + mClientState.mSiteVerletList[ OuterSitePairIndex + i ].mFragmentImageVector;
                VectorPrime[ i ].mVector         =  SiteA - SiteImageB ;
#endif

#if HAND_XYZ
                VectorPrime[ i ].mDistanceSquared  =   VectorPrime[ i ].mVector.mX * VectorPrime[ i ].mVector.mX
                                                     + VectorPrime[ i ].mVector.mY * VectorPrime[ i ].mVector.mY
                                                     + VectorPrime[ i ].mVector.mZ * VectorPrime[ i ].mVector.mZ ;
#else
                VectorPrime[ i ].mDistanceSquared = VectorPrime[ i ].mVector.LengthSquared() ;
#endif

                VectorLiveCount++;
                }
              else
                {
                // avoid divide by zero
                VectorPrime[ i ].mDistanceSquared = 1.0;
                }
              }

#if HAND_SQRT
            double x0 = VectorPrime[ 0 ].mDistanceSquared  ;
            double x1 = VectorPrime[ 1 ].mDistanceSquared  ;
            double x2 = VectorPrime[ 2 ].mDistanceSquared  ;
            double x3 = VectorPrime[ 3 ].mDistanceSquared  ;
            double x4 = VectorPrime[ 4 ].mDistanceSquared  ;
            double x5 = VectorPrime[ 5 ].mDistanceSquared  ;
            double x6 = VectorPrime[ 6 ].mDistanceSquared  ;
            double x7 = VectorPrime[ 7 ].mDistanceSquared  ;
            double x8 = VectorPrime[ 8 ].mDistanceSquared  ;

            double x9 = VectorPrime[ 9 ].mDistanceSquared  ;
            double x10 = VectorPrime[ 10 ].mDistanceSquared  ;
            double x11 = VectorPrime[ 11 ].mDistanceSquared  ;
            double x12 = VectorPrime[ 12 ].mDistanceSquared  ;
            double x13 = VectorPrime[ 13 ].mDistanceSquared  ;
            double x14 = VectorPrime[ 14 ].mDistanceSquared  ;
            double x15 = VectorPrime[ 15 ].mDistanceSquared  ;
            double x16 = VectorPrime[ 16 ].mDistanceSquared  ;
            double x17 = VectorPrime[ 17 ].mDistanceSquared  ;
            double x18 = VectorPrime[ 18 ].mDistanceSquared  ;
            double x19 = VectorPrime[ 19 ].mDistanceSquared  ;

            double r0 = 1.0/sqrt(x0);
            double r1 = 1.0/sqrt(x1);
            double r2 = 1.0/sqrt(x2);
            double r3 = 1.0/sqrt(x3);
            double r4 = 1.0/sqrt(x4);
            double r5 = 1.0/sqrt(x5);
            double r6 = 1.0/sqrt(x6);
            double r7 = 1.0/sqrt(x7);
            double r8 = 1.0/sqrt(x8);

            double r9 = 1.0/sqrt(x9);
            double r10 = 1.0/sqrt(x10);
            double r11 = 1.0/sqrt(x11);
            double r12 = 1.0/sqrt(x12);
            double r13 = 1.0/sqrt(x13);
            double r14 = 1.0/sqrt(x14);
            double r15 = 1.0/sqrt(x15);
            double r16 = 1.0/sqrt(x16);
            double r17 = 1.0/sqrt(x17);
            double r18 = 1.0/sqrt(x18);
            double r19 = 1.0/sqrt(x19);

            VectorPrime[ 0 ].mInverseDistance = r0 ;
            VectorPrime[ 1 ].mInverseDistance = r1 ;
            VectorPrime[ 2 ].mInverseDistance = r2 ;
            VectorPrime[ 3 ].mInverseDistance = r3 ;
            VectorPrime[ 4 ].mInverseDistance = r4 ;
            VectorPrime[ 5 ].mInverseDistance = r5 ;
            VectorPrime[ 6 ].mInverseDistance = r6 ;
            VectorPrime[ 7 ].mInverseDistance = r7 ;
            VectorPrime[ 8 ].mInverseDistance = r8 ;

            VectorPrime[ 9 ].mInverseDistance  = r9 ;
            VectorPrime[ 10 ].mInverseDistance = r10 ;
            VectorPrime[ 11 ].mInverseDistance = r11 ;
            VectorPrime[ 12 ].mInverseDistance = r12 ;
            VectorPrime[ 13 ].mInverseDistance = r13 ;
            VectorPrime[ 14 ].mInverseDistance = r14 ;
            VectorPrime[ 15 ].mInverseDistance = r15 ;
            VectorPrime[ 16 ].mInverseDistance = r16 ;
            VectorPrime[ 17 ].mInverseDistance = r17 ;
            VectorPrime[ 18 ].mInverseDistance = r18 ;
            VectorPrime[ 19 ].mInverseDistance = r19 ;
#else
            // Can the compiler see the vector itself
            for(int vi = 0; vi < VECTOR_LEN; vi++ )
              {
              VectorPrime[ vi ].mInverseDistance = 1.0 / sqrt( VectorPrime[ vi ].mDistanceSquared );
              }

#endif


if( mClientState.mDynVarMgrIF->mSampleFragmentTime )
  {
  StartValue      = Platform::Clock::GetTimeValue();
   }
          for(int LiveOne = 0;
                  LiveOne <  VectorLiveCount ;
                  LiveOne++ )
            {

            //////BegLogLine(1) <<" LiveOne " << LiveOne << " OuterSitePairIndex " << OuterSitePairIndex << EndLogLine;

            TotalPairOpsCounter.Increment();
            int SourceAbsSiteId = mClientState.mSiteVerletList[ OuterSitePairIndex + LiveOne ].mSourceAbsSiteId;
            int TargetAbsSiteId = mClientState.mSiteVerletList[ OuterSitePairIndex + LiveOne ].mTargetAbsSiteId;

            #ifdef PK_PARALLEL
#if 1
            if( mClientState.mDynVarMgrIF->mSampleFragmentTime )
              {
              int ThisFragId  = mClientState.mDynVarMgrIF->mSiteIdToFragmentMap[ TargetAbsSiteId ];
              if( ThisFragId != CurrentFragId  )
                {
                FinisValue      = Platform::Clock::GetTimeValue();
                mClientState.mDynVarMgrIF->AddTimeForFragment( mClientState.mDynVarMgrIF->mSiteIdToFragmentMap[ TargetAbsSiteId ],
                                                             FinisValue - StartValue );
                StartValue    = FinisValue;
                CurrentFragId = ThisFragId;
                }
              }
#endif
            #endif

            if(  VectorPrime[ LiveOne ].mDistanceSquared < mOffRegionSquared )
              {
              nbvm.Prime( VectorPrime[ LiveOne ].mVector,
                          VectorPrime[ LiveOne ].mDistanceSquared,
                          VectorPrime[ LiveOne ].mInverseDistance );

              BegLogLine( PKFXLOG_PAIRS_IN_RANGE && (aSimTick==1) )
                << "Interacting site pair: "
                << " SimTick "
                << aSimTick
                << " SAbsId "
                << SourceAbsSiteId
                << " TAbsId "
                << TargetAbsSiteId
                << EndLogLine;


              double NewEnergy0;
              XYZ Force0 ;
              XYZ Virial0 ;
              Force0.Zero() ;
              Virial0.Zero() ;
              ExecuteWrapper<0, UDF0_Bound, NBVMX>::Execute( nbvm,
                                                             SourceAbsSiteId,
                                                             TargetAbsSiteId,
                                                             Force0,
                                                             NewEnergy0,
                                                             Virial0
                                                             );
              double NewEnergy1;
              XYZ Force1 ;
              XYZ Virial1 ;
              Force1.Zero() ;
              Virial1.Zero() ;
              ExecuteWrapper<1, UDF1_Bound, NBVMX>::Execute( nbvm,
                                                             SourceAbsSiteId,
                                                             TargetAbsSiteId,
                                                             Force1,
                                                             NewEnergy1,
                                                             Virial1
                                                             );

              double NewEnergy2;
              XYZ Force2;
              XYZ Virial2;
              Force2.Zero() ;
              Virial2.Zero() ;
              ExecuteWrapper<2, UDF2_Bound, NBVMX>::Execute( nbvm,
                                                             SourceAbsSiteId,
                                                             TargetAbsSiteId,
                                                             Force2,
                                                             NewEnergy2,
                                                             Virial2
                                                             );
#if HAND_XYZ
              XYZ TotalForce;
                  TotalForce.mX  = Force0.mX  + Force1.mX  + Force2.mX ;
                  TotalForce.mY  = Force0.mY  + Force1.mY  + Force2.mY ;
                  TotalForce.mZ  = Force0.mZ  + Force1.mZ  + Force2.mZ ;


              XYZ TotalVirial;
                  TotalVirial.mX = Virial0.mX + Virial1.mX + Virial2.mX ;
                  TotalVirial.mY = Virial0.mY + Virial1.mY + Virial2.mY ;
                  TotalVirial.mZ = Virial0.mZ + Virial1.mZ + Virial2.mZ ;
#else
              XYZ TotalForce  = Force0  + Force1  + Force2 ;
              XYZ TotalVirial = Virial0 + Virial1 + Virial2 ;
#endif

  #if 0
              BegLogLine( aSimTick == 0 )
                << "SiteNBPair "
                << " SimTick "
                << aSimTick
                << " SId "
                << SourceAbsSiteId
                << " TId "
                << TargetAbsSiteId
                << " TF "
                << TotalForce
                << " TE "
                << TotalVirial
                << " SPos "
                << SiteA
                << " TPos "
                << SiteB
                << EndLogLine;
  #endif



#if HAND_DVM

  #if HAND_XYZ
              mClientState.mDynVarMgrIF->mForce[ 0 ][ SourceAbsSiteId ].mX += TotalForce.mX ;
              mClientState.mDynVarMgrIF->mForce[ 0 ][ SourceAbsSiteId ].mY += TotalForce.mY ;
              mClientState.mDynVarMgrIF->mForce[ 0 ][ SourceAbsSiteId ].mZ += TotalForce.mZ ;

              mClientState.mDynVarMgrIF->mForce[ 0 ][ TargetAbsSiteId ].mX -= TotalForce.mX ;
              mClientState.mDynVarMgrIF->mForce[ 0 ][ TargetAbsSiteId ].mY -= TotalForce.mY ;
              mClientState.mDynVarMgrIF->mForce[ 0 ][ TargetAbsSiteId ].mZ -= TotalForce.mZ ;
  #else
              mClientState.mDynVarMgrIF->mForce[ 0 ][ SourceAbsSiteId ] += TotalForce ;
              mClientState.mDynVarMgrIF->mForce[ 0 ][ TargetAbsSiteId ] -= TotalForce ;
  #endif

#else

  #if HAND_XYZ
              XYZ OppositeTotalForce;
                  OppositeTotalForce.mX = -TotalForce.mX;
                  OppositeTotalForce.mY = -TotalForce.mY;
                  OppositeTotalForce.mZ = -TotalForce.mZ;
  #else
              XYZ OppositeTotalForce = -TotalForce;
  #endif
              mClientState.mDynVarMgrIF->AddForce( SourceAbsSiteId, aSimTick, 0,          TotalForce );
              mClientState.mDynVarMgrIF->AddForce( TargetAbsSiteId, aSimTick, 0,  OppositeTotalForce );

#endif


              GrandTotalEnergyOnUdf[ 0 ] += NewEnergy0;
              GrandTotalEnergyOnUdf[ 1 ] += NewEnergy1;
              GrandTotalEnergyOnUdf[ 2 ] += NewEnergy2;


              // Don't understand how to code the virial reporting.
              //GlobalPartialResultAxB.AddVirial<k_DoPressureControl>( 0, TotalVirial );
              if( k_DoPressureControl )
                {
                //GlobalPartialResultAxB.AddVirial<k_DoPressureControl>( 0,
                //                                               PairwiseProduct( TotalForce, Vector ) ) ;
                }
              }
            else
              {
              TotalPairOpsOutsideSiteSiteCutoff.Increment();
              }

            }
            }

          NSQ_RealSpace_Meat_Finis.HitOE( PKTRACE_REAL_SPACE_MEAT,
                                          "NSQ_RealSpace_Meat_Finis",
                                          0,
                                          NSQ_RealSpace_Meat_Finis );


          if( mClientState.mTuneGuardZone )
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
            IFPHelper0::ReportStaticCounters();
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
              if( NsqUdfGroup0::UDF0_Bound::UDF::ReportsEnergy )
                EmitEnergy( TimeStep, NsqUdfGroup0::UDF0_Bound::UDF::Code, GrandTotalEnergyOnUdf[ 0 ], 0 );

              if( NsqUdfGroup0::UDF1_Bound::UDF::ReportsEnergy )
                EmitEnergy( TimeStep, NsqUdfGroup0::UDF1_Bound::UDF::Code, GrandTotalEnergyOnUdf[ 1 ], 0 );

              if( NsqUdfGroup0::UDF2_Bound::UDF::ReportsEnergy )
                EmitEnergy( TimeStep, NsqUdfGroup0::UDF2_Bound::UDF::Code, GrandTotalEnergyOnUdf[ 2 ], 0 );

              }
            }

          #if MSDDEF_DoPressureControl
            {
              int RespaLevelOfFirstShell = MSD_IF::GetRespaLevelOfFirstShell();
              for(int shellId=0; shellId< NUM_SHELLS; shellId++ )
                {
                mClientState.mDynVarMgrIF->AddVirial( RespaLevelOfFirstShell+shellId,
                                                      GlobalPartialResult.mVirialByShell[ shellId ] );
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


template<class DynVarMgrIF,
         class NsqUdfGroup,
         class SwitchFunctionEnergyAdapter,
         class SwitchFunctionForceAdapter,
         int   SHELL_COUNT >
class NSQAnchor
  {

    /*
     * The anchor IS the Server.
     */
  private:

    class NSQEngine< DynVarMgrIF, NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter, SHELL_COUNT > mServer;

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
    typedef class NSQEngine< DynVarMgrIF, NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter, SHELL_COUNT >::Client ClientIF;

    ClientIF &
    GetClientIF()
      {
      NSQEngine< DynVarMgrIF, NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter, SHELL_COUNT >::Client * cr;
      pkNew( &cr, 1, __FILE__, __LINE__ );

      return( * cr  );
      }
  };

#endif
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
#if 0 // GRAVE YARD
              // If there is an active verlet list, move it to the end of the buffer in
              // prep for the scan and reuse of the entries for the image info.
              if( ! mClientState.mUpdateFragVerletListFirstTime )
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
#endif // END GRAVE YARD
