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
 * Module:          IPartPair
 *
 * Purpose:         Manage pairwise interactions between irreducible partition pairs
 *                  during NonBonded Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010522 BGF Created from design.
 *
 ***************************************************************************/

#ifndef __IFP_SIMPLE__HPP__
#define __IFP_SIMPLE__HPP__

#ifndef PKFXLOG_PAIRS_IN_RANGE
#define PKFXLOG_PAIRS_IN_RANGE ( 0 )
#endif

#include <BlueMatter/IFPHelper.hpp>

template<class NsqUdfGroup,
         class SwitchFunctionEnergyAdapter,
         class SwitchFunctionForceAdapter,
         int   SHELL_COUNT,
         int   CONTROL_FLAGS>
class IFPSimple
  {
  public:
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
            aForce = aNbvm.GetForce( NBVM_T::SiteA );
            aNewEnergy = aNbvm.GetEnergy();

            if( UDF_Bound::UDF::ReportsVirial )
              aVirial = aNbvm.GetVirial( ORDINAL );
            }
          }
      };

    static
    inline
    void
    Init()
      {
      BegLogLine(1)
        << "IFPSimple::Init() "
        << " initializing "
        << EndLogLine;

      IFPHelper0::Init();

      }


    template<class GlobalInputIF,
             class SourceGroupIF,
             class TargetGroupIF,
             class FragmentResultIF,
             class GlobalResultIF>
    static
    inline
    void
    Execute(       int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   TargetGroupIF      aTargetFragmentIF,
                   SourceGroupIF      aSourceFragmentIF,
                   FragmentResultIF & aTargetResultIF,
                   FragmentResultIF & aSourceResultIF,
                   GlobalResultIF   & aGlobalResultIF,
             const int                aReportForceOnSourceFlag,
                   ExclusionMask_T  * aExclusionMaskPtr,
             const XYZ              & FragmentImageVector)
      {
      // These are the switch parameters for the shell at the CURRENT respa level
      double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
      double SwitchDeltaOuterShell       = aGlobalInputParametersIF.mSwitchDelta;

      double OffRegion       = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;
      double OffRegionSquared = OffRegion * OffRegion;

      int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

      // Create a flag to be used to recognize when source and target fragments are the
      // same and thus we can do 1/2 the force term computations by applying the results
      // of each computation to both sits sites.

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "IrreducibleFragmentPair(): STARTING "
        << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
        << " / " << SourceIrreducibleFragmentId
        // << " PersistentStatePointer " << (void *) aPersistentStatePtr
        << EndLogLine;

      int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
      int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

      NBVMX nbvm;
      nbvm.SetSimTick( aSimTick );

      // Make sure that the force is put on the last respa level
      nbvm.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );

      ExclusionMask_T ExclMask;
      if( aExclusionMaskPtr != NULL )
        ExclMask = *aExclusionMaskPtr;
      else
        ExclMask = 0;          

      int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

      for(int SourceFragmentSiteIndex = 0;
          SourceFragmentSiteIndex < SourceFragmentSiteCount;
          SourceFragmentSiteIndex++ )
        {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        XYZ SiteA = *aSourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex );

        // Scan the Target Fragment since that should be more local.
        for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1 : 0 );
            TargetFragmentSiteIndex < TargetFragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
          BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

          unsigned AreSiteInExclusionList = ( ExclMask & 0x00000001 );
          ExclMask =  ExclMask >> 1;

          if ( AreSiteInExclusionList )
            continue;

          TotalPairOpsCounter.Increment();

          XYZ SiteB = *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex );
          XYZ SiteImageB = SiteB + FragmentImageVector;

          BegLogLine( PKFXLOG_IPARTPAIR )
            << "IFPSimple: "
            << " SiteA "
            << SiteA
            << " *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex )= "
            << *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex )
            << " SiteImageB "
            << SiteImageB
            << EndLogLine;

          // The following is probably a very expensive way to find out the AbsSiteId's
          // - these could be carried in with the Src/TrgFragmentIF
          int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
          int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

          XYZ    Vector            = SiteA - SiteImageB ;
          double DistanceSquared   = Vector.LengthSquared() ;

          BegLogLine( TargetAbsSiteId == 226 && SourceAbsSiteId == 1474 && aSimTick == 0 )
            << "About to cull out site pair( " << TargetAbsSiteId << " , " << SourceAbsSiteId << " )"
            << " SiteA: " << SiteA
            << " SiteB: " << SiteA
            << " FragmentImageVector: " << FragmentImageVector
            << " DistanceSquared: " << DistanceSquared  
            << " OffRegionSquared: " << OffRegionSquared
            << EndLogLine;

          ///////double DistanceSquared = SiteImageB.DistanceSquared( SiteA );

          if( DistanceSquared < OffRegionSquared )
            {

            BegLogLine( TargetAbsSiteId == 0 && SourceAbsSiteId == 2683 )
              << "IFPSimple:: Accepted site pair( " << TargetAbsSiteId << " , " << SourceAbsSiteId << " )"
              << EndLogLine;
            
            double InverseDistance = 1.0/sqrt(DistanceSquared) ;
            nbvm.Prime( Vector, DistanceSquared, InverseDistance );

            BegLogLine( PKFXLOG_PAIRS_IN_RANGE && (aSimTick==1) )
              << "IFPTiled:: "
              << SourceAbsSiteId << " "
              << TargetAbsSiteId
              << EndLogLine;


            BegLogLine(PKFXLOG_IPARTPAIR)
              << "WorkThread(): " << " NBVMX:: " << " Posit A " << SiteA << " Posit B "
              << SiteImageB << " DistanceAB " << nbvm.GetDistance( NBVMX::SiteA, NBVMX::SiteB )
              << EndLogLine;

            BegLogLine(PKFXLOG_IPARTPAIR)
              << "*** ABOUT NSQ SITE PAIR *** "
              << " SourceAbsSiteId " <<  SourceAbsSiteId
              << " TargetAbsSiteId " <<  TargetAbsSiteId
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
            XYZ TotalForce = Force0 + Force1 + Force2 ;
            XYZ TotalVirial = Virial0 + Virial1 + Virial2 ;

            BegLogLine(PKFXLOG_IPARTPAIR)
              << "TotalForce " << TotalForce
              << " TotalVirial " << TotalVirial
              << " NewEnergy0 " << NewEnergy0
              << " NewEnergy1 " << NewEnergy1
              << " NewEnergy2 " << NewEnergy2
              << EndLogLine;

            BegLogLine(PKFXLOG_ENERGYTRACE)
              << "EnergyTrace "
              << ' ' << SourceAbsSiteId
              << ' ' << TargetAbsSiteId
              << ' ' << NewEnergy0
              << ' ' << NewEnergy1
              << ' ' << NewEnergy2
              << EndLogLine ;

            if( aReportForceOnSourceFlag || SourceIsTargetFlag )
              {
              aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );
              aSourceResultIF.AddForce( SourceAbsSiteId, 0,  TotalForce );
              //BGF g++
              #if defined(PK_GCC)
                if( UDF0_Bound::UDF::ReportsEnergy )
                  aGlobalResultIF.AddEnergy( 0, NewEnergy0 );
                if( UDF1_Bound::UDF::ReportsEnergy )
                  aGlobalResultIF.AddEnergy( 1, NewEnergy1 );
                if( UDF2_Bound::UDF::ReportsEnergy )
                  aGlobalResultIF.AddEnergy( 2, NewEnergy2 );

                if( k_DoPressureControl )
                  aGlobalResultIF.AddVirial( 0, TotalVirial );
              #else
                aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, NewEnergy0 );
                aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, NewEnergy1 );
                aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, NewEnergy2 );
                aGlobalResultIF.AddVirial<k_DoPressureControl>( 0, TotalVirial );
              #endif
              //BGF g++
              if( k_DoPressureControl )
                {
                BegLogLine(PKFXLOG_VIRIALTRACE)
                  << "VirialTrace "
                  << ' ' << SourceAbsSiteId
                  << ' ' << TargetAbsSiteId
                  << ' ' <<  PairwiseProduct( TotalForce, Vector ) * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
                  << ' ' << TotalVirial
                  << EndLogLine ;
                }
              }
            else
              {
              // ALWAYS apply force on Target
              aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );

              // and only half the energy

              #if defined(PK_GCC)
                if( UDF0_Bound::UDF::ReportsEnergy )
                  aGlobalResultIF.AddEnergy( 0, NewEnergy0 * 0.5);
                if( UDF1_Bound::UDF::ReportsEnergy )
                  aGlobalResultIF.AddEnergy( 1, NewEnergy1 * 0.5);
                if( UDF2_Bound::UDF::ReportsEnergy )
                  aGlobalResultIF.AddEnergy( 2, NewEnergy2 * 0.5);

                if( k_DoPressureControl )
                  aGlobalResultIF.AddVirial( 0, TotalVirial * 0.5);
              #else
                aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, NewEnergy0 * 0.5);
                aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, NewEnergy1 * 0.5);
                aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, NewEnergy2 * 0.5);

                aGlobalResultIF.AddVirial<k_DoPressureControl>( 0, TotalVirial * 0.5);
              #endif

              if( k_DoPressureControl )
                {
                BegLogLine(PKFXLOG_VIRIALTRACE)
                  << "VirialTrace "
                  << ' ' << SourceAbsSiteId
                  << ' ' << TargetAbsSiteId
                  << ' ' <<  PairwiseProduct( TotalForce, Vector )
                  * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
                  << ' ' << TotalVirial *0.5
                  << EndLogLine ;
                }
              }

            /////////////////////////// NEW FRAGMENT BASED CODE

            if( k_DoPressureControl )
              {
              #if defined(PK_GCC)
                aGlobalResultIF.AddVirial( 0,
                                           PairwiseProduct( TotalForce, Vector )
                                           * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
                                         ) ;
              #else
                aGlobalResultIF.AddVirial<k_DoPressureControl>( 0,
                                                              PairwiseProduct( TotalForce, Vector )
                                                              * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
                                                              ) ;
              #endif
              }
            }
          else
            {
            TotalPairOpsOutsideSiteSiteCutoff.Increment();
            } /* endif distance cutoff */
          } /* endif pairinteractionmask */
        } /* endfor targetfragmentsiteindex */
      } /* endfor sourcefragmentsiteindex */

  }; /* IFPSimple */

#endif
