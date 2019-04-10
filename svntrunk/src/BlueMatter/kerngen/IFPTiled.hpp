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

/*
 * This version has the loops split out and refactored
 * to take conditionals out of loops in preparation for tiling
 */

#ifndef __IFP_TILED__HPP__
#define __IFP_TILED__HPP__

#ifndef PKFXLOG_IFPAUDIT
#define PKFXLOG_IFPAUDIT ( 0 )
#endif

#ifndef PKFXLOG_PAIRS_IN_RANGE
#define PKFXLOG_PAIRS_IN_RANGE ( 0 )
#endif

#include <BlueMatter/IFPHelper.hpp>

// We can scale by multiplying here, rather than a conditional branch
static const double k_ExclusionScale[2] = { 0.0, 1.0 } ;

template<class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int CONTROL_FLAGS>
class IFPTiled
{
public:
  typedef typename NsqUdfGroup::UDF0_Bound UDF0_Bound;
  typedef typename NsqUdfGroup::UDF1_Bound UDF1_Bound;
  typedef typename NsqUdfGroup::UDF2_Bound UDF2_Bound;

  typedef IFPHelper< NsqUdfGroup > IFPHelper0;

  typedef NsqUdfGroup NsqUdfGroup0;

  enum {
    k_TileSize = 3
    , k_TileArea = k_TileSize * k_TileSize
    , k_VectorPadding = 20
    , k_FullTileThreshold = 6 // For tiles at least this full, we evaluate all 3x3 slots
    , k_OutRangeThreshold = k_TileArea - k_FullTileThreshold // conversely,for tiles emptier than this, we compress
  };

  template<int ORDINAL, class UDF_Bound, class NBVM_T>
  class ExecuteWrapper
  {
  public:
    static inline void ExecuteWithExclusions( NBVM_T&         aNbvm,
					      int&            aSourceId,
					      int&            aTargetId,
					      XYZ&            aForce,
					      double&         aNewEnergy,
					      XYZ&            aVirial
					      )
    {
#pragma execution_frequency( very_high )
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
	  aNewEnergy = aNbvm.GetEnergy(  );

	  if( UDF_Bound::UDF::ReportsVirial )
	    aVirial = aNbvm.GetVirial( );
	}

    }


    static inline void ExecuteWithExclusionsWithoutSwitchScalar( NBVM_T&         aNbvm,
								 int            aSourceId,
								 int            aTargetId,
								 double&     aE,
								 double&     adEdr,
								 double&             aScalarVirial
								 )
    {
#pragma execution_frequency( very_high )
      aNbvm.SetCurrentUdfOrdinal( ORDINAL );
      if( UDF_Bound::UDF::Code != -1 )
	{
	  IFPHelper0::DriveOne<UDF_Bound::UDF::NsqType, UDF_Bound, NBVM_T>::Execute( aNbvm, aSourceId, aTargetId );
	  double ExclusionScale = 1.0;
	  aE = aNbvm.GetEnergy() * ExclusionScale;
	  adEdr = aNbvm.GetdEdr() * ExclusionScale;
	  if ( UDF_Bound::UDF::ReportsVirial )
	    {
	      aScalarVirial = aNbvm.GetAdditionalScalarVirial() * ExclusionScale ;
	    }
	}
    }


    static inline void Execute( NBVM_T&         aNbvm,
				int&            aSourceId,
				int&            aTargetId,
				XYZ&            aForce,
				double&         aNewEnergy,
				XYZ&            aVirial
				)
    {
#pragma execution_frequency( very_high )
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
	  aNewEnergy = aNbvm.GetEnergy(  );

	  if( UDF_Bound::UDF::ReportsVirial )
	    aVirial = aNbvm.GetVirial( );
	}
    }
    static inline void ExecuteWithoutSwitch( NBVM_T&         aNbvm,
					     int&            aSourceId,
					     int&            aTargetId,
					     XYZ&            aForce,
					     double&         aNewEnergy,
					     XYZ&            aVirial
					     )
    {
#pragma execution_frequency( very_high )
      aNbvm.SetCurrentUdfOrdinal( ORDINAL );
      if( UDF_Bound::UDF::Code != -1 )
	{
	  IFPHelper0::DriveOne<UDF_Bound::UDF::NsqType, UDF_Bound, NBVM_T>::Execute( aNbvm, aSourceId, aTargetId );


	  aForce = aNbvm.GetForce( NBVM_T::SiteA );
	  aNewEnergy = aNbvm.GetEnergy(  );

	  if( UDF_Bound::UDF::ReportsVirial )
	    aVirial = aNbvm.GetVirial(  );
	}
    }

    static inline void ExecuteWithoutSwitchScalar( NBVM_T&         aNbvm,
						   int&            aSourceId,
						   int&            aTargetId,
						   double&     aE,
						   double&     adEdr,
						   double&             aScalarVirial
						   )
    {
#pragma execution_frequency( very_high )
      aNbvm.SetCurrentUdfOrdinal( ORDINAL );
      if( UDF_Bound::UDF::Code != -1 )
	{
	  IFPHelper0::DriveOne<UDF_Bound::UDF::NsqType, UDF_Bound, NBVM_T>::Execute( aNbvm, aSourceId, aTargetId );

	  aE = aNbvm.GetEnergy() ;
	  adEdr = aNbvm.GetdEdr() ;
	  if ( UDF_Bound::UDF::ReportsVirial)
	    {
	      aScalarVirial = aNbvm.GetAdditionalScalarVirial() ;
	    }
	}
    }

    static inline void ExecuteWithoutSwitchScalarDetail(
							int SourceAbsSiteId,
							int TargetAbsSiteId,
							double DistanceSquared,
							double ReciprocalDistance,
							double& aEnergyUnswitched,
							double& adEdrUnswitched,
							double& aExtraVirialUnswitched,
							int aSimTick
							) {
#pragma execution_frequency( very_high )
      NBVM_T nbvm ;
      nbvm.SetSimTick( aSimTick );
      nbvm.Prime(DistanceSquared,ReciprocalDistance );

      BegLogLine(PKFXLOG_IPARTPAIR)
	<< "WorkThread(): "
	<< " NBVMX DistanceAB " << DistanceSquared*ReciprocalDistance
	<< " SourceAbsSiteId " <<  SourceAbsSiteId
	<< " TargetAbsSiteId " <<  TargetAbsSiteId
	<< EndLogLine;

      double EnergyUnswitched = 0.0 ;
      double dEdrUnswitched = 0.0 ;
      double ExtraVirialUnswitched = 0.0 ;
      ExecuteWithoutSwitchScalar( nbvm,
				  SourceAbsSiteId,
				  TargetAbsSiteId,
				  EnergyUnswitched,
				  dEdrUnswitched,
				  ExtraVirialUnswitched
				  );
      aEnergyUnswitched =  EnergyUnswitched ;
      adEdrUnswitched =  dEdrUnswitched ;
      aExtraVirialUnswitched = ExtraVirialUnswitched ;
      nbvm.TearDown() ;


    }

    static inline void ExecuteWithExclusionsWithoutSwitchScalarDetail( int SourceAbsSiteId,
								       int TargetAbsSiteId,
								       double DistanceSquared,
								       double ReciprocalDistance,
								       double& aEnergyUnswitched,
								       double& adEdrUnswitched,
								       double& aExtraVirialUnswitched,
								       int aSimTick
								       )
    {
#pragma execution_frequency( very_high )
      NBVM_T nbvm ;
      nbvm.SetSimTick( aSimTick );
      nbvm.Prime(DistanceSquared,ReciprocalDistance );

      BegLogLine(PKFXLOG_IPARTPAIR)
	<< "WorkThread(): "
	<< " NBVMX DistanceAB " << DistanceSquared*ReciprocalDistance
	<< " SourceAbsSiteId " <<  SourceAbsSiteId
	<< " TargetAbsSiteId " <<  TargetAbsSiteId
	<< EndLogLine;

      double EnergyUnswitched = 0.0 ;
      double dEdrUnswitched = 0.0 ;
      double ExtraVirialUnswitched = 0.0 ;
      ExecuteWithExclusionsWithoutSwitchScalar( nbvm,
						SourceAbsSiteId,
						TargetAbsSiteId,
						EnergyUnswitched,
						dEdrUnswitched,
						ExtraVirialUnswitched
						);

      aEnergyUnswitched =  EnergyUnswitched ;
      adEdrUnswitched =  dEdrUnswitched ;
      aExtraVirialUnswitched = ExtraVirialUnswitched ;

      nbvm.TearDown() ;
    }

    static inline void ExecuteSwitchOnlyUnconditional( NBVM_T&         aNbvm
						       )
    {
#pragma execution_frequency( very_high )

      /****************************************************************/
      // Switch on the energies according to ForceSwitched = Force * S - UnitVector * Energy * dSdr

      // If we're doing the respa correction udf, then we are in an outer timestep
      // situation therefore we need to switch in the energy.
      SwitchFunctionEnergyAdapter::Execute( aNbvm );

    }
  };

  class AtomPair
  {
  public:
    unsigned int SourceIndex ;
    unsigned int TargetIndex ;
  } ;

  class AtomPairList
  {
  public:
    double DistanceSquared[k_TileArea+k_VectorPadding] ;
    double ReciprocalDistance[k_TileArea+k_VectorPadding] ;
    AtomPair mPair[k_TileArea] ;
  } ;


  static
  void
  Init()
#if defined(IFP_SEPARATE_EXECUTE)
    ;
#else
  {
    IFPHelper0::Init();
  }
#endif

  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1,class FragmentResultIF2, class GlobalResultIF>
  static
  inline
  void
  ExecuteWithExclusions( int           aSimTick,
			 GlobalInputIF &aGlobalInputParametersIF,
			 TargetGroupIF aTargetFragmentIF,
			 SourceGroupIF aSourceFragmentIF,
			 FragmentResultIF1     &aTargetResultIF,
			 FragmentResultIF2     &aSourceResultIF,
			 GlobalResultIF       &aGlobalResultIF,
			 const int     aReportForceOnSourceFlag,
			 ExclusionMask_T * aExclusionMaskPtr,
			 const XYZ & FragmentImageVector )
  {
#pragma execution_frequency( very_high )
    int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
    int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

    // Create a flag to be used to recognize when source and target fragments are the
    // same and thus we can do 1/2 the force term computations by applying the results
    // of each computation to both sits sites.

    int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair::ExecuteWithExclusions(): STARTING "
      << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
      << " / " << SourceIrreducibleFragmentId
      << " aExclusionMaskPtr " << (void *) aExclusionMaskPtr
      << EndLogLine;

    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
    int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

    double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
    double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
    double SwitchUpperCutoff = SwitchLowerCutoff + SwitchDelta;

    double SwitchUpperCutoffSrq = SwitchUpperCutoff * SwitchUpperCutoff;

    ExclusionMask_T ExclMask = *aExclusionMaskPtr;

    static int count1=0;

    for(int SourceFragmentSiteIndex = 0;
	SourceFragmentSiteIndex < SourceFragmentSiteCount;
	SourceFragmentSiteIndex++ )
      {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        XYZ SiteA = aSourceFragmentIF.GetPosition( SourceFragmentSiteIndex );

        XYZ SiteAImage = SiteA-FragmentImageVector ;


        // Scan the Target Fragment since that should be more local.
        for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1 : 0 );
            TargetFragmentSiteIndex < TargetFragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
	    BegLogLine( PKFXLOG_IPARTPAIR ) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

	    // 'PairInteractionMask' knocks out those atom pairs which are handled by the bonded force evaluator
	    unsigned AreSiteInExclusionList = ( ExclMask & 0x00000001 );
	    ExclMask =  ExclMask >> 1;

	    // BegLogLine( count1 < 100 )
	    BegLogLine( 0 )
	      << "IFPTiled:: "
	      << " S=" << SourceFragmentSiteIndex
	      << " T=" << TargetFragmentSiteIndex
	      << " I=" << AreSiteInExclusionList
	      << EndLogLine;


	    BegLogLine( 0 )
	      << "IFPTiled::ExclusionSend:: "
	      << " AreSiteInExclusionList=" << AreSiteInExclusionList
	      << " ExclMask=" << (void *) ExclMask
	      << " OriginalMask=" << (void *) *aExclusionMaskPtr
	      << EndLogLine;

	    if ( AreSiteInExclusionList )
	      continue;

	    TotalPairOpsCounter.Increment();

	    // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
	    XYZ SiteB = aTargetFragmentIF.GetPosition( TargetFragmentSiteIndex );
	    XYZ Vector = SiteAImage - SiteB ;

	    BegLogLine( PKFXLOG_IPARTPAIR )
              << "IFPTiled: "
              << " SiteA "
              << SiteA
              << " aTargetFragmentIF.GetPosition( TargetFragmentSiteIndex )= "
              << aTargetFragmentIF.GetPosition( TargetFragmentSiteIndex )
              << " SiteAImage "
              << SiteAImage
              << " Vector "
              << Vector
              << EndLogLine;

	    double DistanceSquared = Vector.LengthSquared() ;
	    double ReciprocalDistance = 1.0/sqrt(DistanceSquared) ;

	    if( DistanceSquared > SwitchUpperCutoffSrq )
	      {
		continue;
	      }

	    // The following is probably a very expensive way to find out the AbsSiteId's
	    // - these could be carried in with the Src/TrgFragmentIF
	    int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
	    int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

	    BegLogLine( 0 )
	      << "ExclIn " << SourceAbsSiteId
	      << " " << TargetAbsSiteId
	      << " " << SourceFragmentSiteIndex
	      << " " << TargetFragmentSiteIndex
	      << " " << SourceIrreducibleFragmentId
	      << " " << TargetIrreducibleFragmentId
	      << EndLogLine;

	    LoopResult PairdEdr0 ;
	    LoopResult PairdEdr1 ;
	    LoopResult PairdEdr2 ;
	    LoopResult PairEnergy0 ;
	    LoopResult PairEnergy1 ;
	    LoopResult PairEnergy2 ;
	    LoopResult PairExtraScalarVirial0 ;
	    LoopResult PairExtraScalarVirial1 ;
	    LoopResult PairExtraScalarVirial2 ;

	    // Evaluate the unswitched forces
	    if( UDF0_Bound::UDF::Code != -1 )
	      {
		ExecuteWrapper<0, UDF0_Bound, NBVMX_SCALAR>::ExecuteWithExclusionsWithoutSwitchScalarDetail(
													    SourceAbsSiteId,
													    TargetAbsSiteId,
													    DistanceSquared,
													    ReciprocalDistance,
													    PairEnergy0.mValue,
													    PairdEdr0.mValue,
													    PairExtraScalarVirial0.mValue,
													    aSimTick
													    ) ;
	      } /* endif UDF Code */

	    if( UDF1_Bound::UDF::Code != -1 )
	      {
		ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithExclusionsWithoutSwitchScalarDetail(
													    SourceAbsSiteId,
													    TargetAbsSiteId,
													    DistanceSquared,
													    ReciprocalDistance,
													    PairEnergy1.mValue,
													    PairdEdr1.mValue,
													    PairExtraScalarVirial1.mValue,
													    aSimTick
													    ) ;
	      } /* endif UDF Code */

	    if( UDF2_Bound::UDF::Code != -1 )
	      {
		ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithExclusionsWithoutSwitchScalarDetail(
													    SourceAbsSiteId,
													    TargetAbsSiteId,
													    DistanceSquared,
													    ReciprocalDistance,
													    PairEnergy2.mValue,
													    PairdEdr2.mValue,
													    PairExtraScalarVirial2.mValue,
													    aSimTick
													    ) ;
	      } /* endif UDF Code */

	    LoopResult PairdEdr;
	    LoopResult PairExtraScalarVirial ;
	    PairdEdr.mValue =
              ( ( UDF0_Bound::UDF::Code != -1 ) ? PairdEdr0.mValue : 0.0 )
              +( ( UDF1_Bound::UDF::Code != -1 ) ? PairdEdr1.mValue : 0.0 )
              +( ( UDF2_Bound::UDF::Code != -1 ) ? PairdEdr2.mValue : 0.0 ) ;
	    if ( k_DoPressureControl )
	      {
		PairExtraScalarVirial.mValue =
		  ( ( UDF0_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial0.mValue : 0.0 )
		  +( ( UDF1_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial1.mValue : 0.0 )
		  +( ( UDF2_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial2.mValue : 0.0 ) ;
	      }
	    NBVM_SWITCHEVALUATOR switchev ;
	    switchev.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );

	    switchev.Prime(DistanceSquared,ReciprocalDistance  );
	    ExecuteWrapper<2, UDF2_Bound, NBVM_SWITCHEVALUATOR>::ExecuteSwitchOnlyUnconditional( switchev);
	    double S = switchev.GetSwitchS() ;
	    double pdSdr = switchev.GetSwitchdSdR() ;

	    double EnergyUnswitched0 = PairEnergy0.mValue ;
	    double EnergyUnswitched1 = PairEnergy1.mValue ;
	    double EnergyUnswitched2 = PairEnergy2.mValue ;

	    double EnergyUnswitched =
              ( UDF0_Bound::UDF::ReportsEnergy ? EnergyUnswitched0 : 0.0 )
              + ( UDF1_Bound::UDF::ReportsEnergy ? EnergyUnswitched1 : 0.0 )
              + ( UDF2_Bound::UDF::ReportsEnergy ? EnergyUnswitched2 : 0.0 ) ;

	    double dEdrUnswitched = PairdEdr.mValue ;
	    double ExtraScalarVirialUnswitched = PairExtraScalarVirial.mValue ;

	    double dEdrSwitched = dEdrUnswitched * S + EnergyUnswitched * pdSdr ;
	    double EnergySwitched0 = EnergyUnswitched0 * S ;
	    double EnergySwitched1 = EnergyUnswitched1 * S ;
	    double EnergySwitched2 = EnergyUnswitched2 * S ;
	    double ExtraScalarVirialSwitched =ExtraScalarVirialUnswitched * S ;


	    PairdEdr.mValue = dEdrSwitched;
	    PairExtraScalarVirial.mValue = ExtraScalarVirialSwitched ;


	    // Scatter out the vector, so the total atom forces can be calculated
	    XYZ VirialSum ;
	    double ExtraScalarVirial ;
	    if (k_DoPressureControl)
	      {
		VirialSum.Zero() ;
		ExtraScalarVirial = 0.0 ;
	      }
	    XYZ dVecAB = Vector ;

	    double dVecABMagR = ReciprocalDistance;
	    XYZ aForcePartial = dVecAB * (-PairdEdr.mValue*dVecABMagR);
	    aSourceResultIF.AddForce(
				     MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex)
				     , 0
				     , aForcePartial
				     ) ;
	    aTargetResultIF.AddForce(
				     MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex)
				     , 0
				     , -aForcePartial
				     ) ;

	    BegLogLine(PKFXLOG_IPARTPAIR)
              << "PairdEdr " << PairdEdr.mValue
              << "Force on source " << aForcePartial
              << "Force on target " << -aForcePartial
              << EndLogLine;

	    BegLogLine(PKFXLOG_ENERGYTRACE)
              << "EnergyTrace "
              << ' ' << SourceAbsSiteId
              << ' ' << TargetAbsSiteId
              << ' ' << (UDF0_Bound::UDF::ReportsEnergy ? EnergySwitched0 : 0.0 )
              << ' ' << (UDF1_Bound::UDF::ReportsEnergy ? EnergySwitched1 : 0.0 )
              << ' ' << (UDF2_Bound::UDF::ReportsEnergy ? EnergySwitched2 : 0.0 )
              << EndLogLine ;

	    if (k_DoPressureControl)
	      {
		VirialSum += PairwiseProduct( aForcePartial, dVecAB ) ;
		ExtraScalarVirial += PairExtraScalarVirial.mValue ;
		BegLogLine(PKFXLOG_VIRIALTRACE)
		  << "VirialTrace "
		  << ' ' << SourceAbsSiteId
		  << ' ' << TargetAbsSiteId
		  << ' ' << PairwiseProduct( aForcePartial, dVecAB )
		  << ' ' << SetToScalar(PairExtraScalarVirial.mValue * (1.0/3.0) )
		  << EndLogLine ;

	      }
	    aGlobalResultIF.AddVirial<k_DoPressureControl>( (VirialSum + SetToScalar(ExtraScalarVirial * (1.0/3.0) ) ) * ( aReportForceOnSourceFlag ? 1.0 : 0.5  ) ) ;

	    aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, EnergySwitched0 );
	    aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, EnergySwitched1 );
	    aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, EnergySwitched2 );
          }
      }

  } /* IFPSimple::ExecuteWithExclusions */

  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
  static
  inline
  void
  ExecuteWithoutExclusionsCountingBothTriangle( int           aSimTick,
						GlobalInputIF &aGlobalInputParametersIF,
						TargetGroupIF aTargetFragmentIF,
						SourceGroupIF aSourceFragmentIF,
						FragmentResultIF1     &aTargetResultIF,
						FragmentResultIF2     &aSourceResultIF,
						GlobalResultIF       &aGlobalResultIF,
						const int     aReportForceOnSourceFlag,
						const XYZ & FragmentImageVector)
  {
    assert(0) ;         // If we're doing 'Trinagle', it should be fragment against itself, and there should be an exclusion list
  } /* IFPSimple::ExecuteWithoutExclusionsCountingBothTriangle */


  static void PaddableVectorReciprocalSquareRoot(
						 double * Target,
						 const double * Source,
						 unsigned int Count
						 ) {
    assert ( Count <= 9 ) ;  // we are handling 3x3 tiles ...

    double x0 = Source[0];
    double x1 = Source[1];
    double x2 = Source[2];
    double x3 = Source[3];
    double x4 = Source[4];
    double x5 = Source[5];
    double x6 = Source[6];
    double x7 = Source[7];
    double x8 = Source[8];

    double r0 = 1.0/sqrt(x0);
    double r1 = 1.0/sqrt(x1);
    double r2 = 1.0/sqrt(x2);
    double r3 = 1.0/sqrt(x3);
    double r4 = 1.0/sqrt(x4);
    double r5 = 1.0/sqrt(x5);
    double r6 = 1.0/sqrt(x6);
    double r7 = 1.0/sqrt(x7);
    double r8 = 1.0/sqrt(x8);

    Target[0] = r0 ;
    Target[1] = r1 ;
    Target[2] = r2 ;
    Target[3] = r3 ;
    Target[4] = r4 ;
    Target[5] = r5 ;
    Target[6] = r6 ;
    Target[7] = r7 ;
    Target[8] = r8 ;

  }

  class LoopResult
  {
  public:
    double mValue ;
  } ;

  static inline int mymin(int a, int b)
  {
    int difference = a-b ;
    int mask_1 = difference >> 31 ; // 0 if a>=b, 0xffffffff if a < b
    int mask_2 = mask_1 ^ 0xffffffff ; // reverse of mask_1
    int result = ( a & mask_1 ) | ( b & mask_2 ) ; // pick the one we want
    return result ;
  }

  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
  static
  inline
  void
  ExecuteWithoutExclusionsCountingBothTile( int           aSimTick,
					    GlobalInputIF &aGlobalInputParametersIF,
					    TargetGroupIF aTargetFragmentIF,
					    SourceGroupIF aSourceFragmentIF,
					    FragmentResultIF1     &aTargetResultIF,
					    FragmentResultIF2     &aSourceResultIF,
					    GlobalResultIF       &aGlobalResultIF,
					    const int     aReportForceOnSourceFlag,
					    const XYZ& FragmentImageVector,
					    // const XYZ & SourceTargetImageCOMVector,
					    int SourceFragmentTileBase,
					    int TargetFragmentTileBase,
					    int SourceFragmentSiteCount,
					    int TargetFragmentSiteCount
					    )
  {
    // This used to be an arguement but seems unused by caller so make local.
    // NEED: change the name so this doesn't look like an argument
#pragma execution_frequency( very_high )
    int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
    int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

    // Create a flag to be used to recognize when source and target fragments are the
    // same and thus we can do 1/2 the force term computations by applying the results
    // of each computation to both sits sites.

    int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair::ExecuteWithoutExclusionsCountingBothTile(): STARTING "
      << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
      << " / " << SourceIrreducibleFragmentId
      << " Tile " << SourceFragmentTileBase
      << " / " << TargetFragmentTileBase
      << EndLogLine;

    unsigned int silim ;
    unsigned int tilim ;

    // These are the switch parameters for the shell at the CURRENT respa level
    double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
    double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

    double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;
    double OffRegionSquared = OffRegion*OffRegion ;
    double OnRegionSquared = SwitchLowerCutoffOuterShell *SwitchLowerCutoffOuterShell ;

    // 'DistanceSquared' are all positive.
    // For positive numbers, IEE 'double' values collate the same way
    // as 64-bit integers with the same bit pattern. (Strange but true, or
    // just possibly by design)
    // For purposes of looking at whether an atom pair is in switch, or in
    // range, it issufficient to look at the top 32 bits.
    int iOffRegionSquared = *(int *)(&OffRegionSquared) ;
    int iOnRegionSquared = *(int *)(&OnRegionSquared) ;

    // 'AnyInSwitch' gets set >=0 if anything is in the soft-switch region
    int AnyInSwitch ;
    // Run the tile for square-distances
    AtomPairList PairList ;
    unsigned int PairListCount = 0 ;
    XYZ ForcePartial[k_TileSize][k_TileSize] ;
    //        double dEdrPartial[k_TileSize][k_TileSize] ;
    double EnergyPartial0[k_TileSize][k_TileSize] ;
    double EnergyPartial1[k_TileSize][k_TileSize] ;
    double EnergyPartial2[k_TileSize][k_TileSize] ;

    XYZ SiteImagesA[k_TileSize] ;
    XYZ SitesB[k_TileSize] ;


    assert ( 3 == k_TileSize ) ;
    // Special-case a full size tile, it usually is ...
    //      if ( k_TileSize == silim && k_TileSize == tilim )
    // express 'full tile' with a single branch, looking positively :-)
    if (  (signed int) ( ( (SourceFragmentSiteCount-k_TileSize-SourceFragmentTileBase)
			   |       (TargetFragmentSiteCount-k_TileSize-TargetFragmentTileBase)
			   ) ) >= 0
	  )
      {

	SiteImagesA[0]=aSourceFragmentIF.GetPosition( SourceFragmentTileBase+0 ) - FragmentImageVector ;
	SiteImagesA[1]=aSourceFragmentIF.GetPosition( SourceFragmentTileBase+1 ) - FragmentImageVector ;
	SiteImagesA[2]=aSourceFragmentIF.GetPosition( SourceFragmentTileBase+2 ) - FragmentImageVector ;


	SitesB[0] = aTargetFragmentIF.GetPosition( TargetFragmentTileBase+0 ) ;
	SitesB[1] = aTargetFragmentIF.GetPosition( TargetFragmentTileBase+1 ) ;
	SitesB[2] = aTargetFragmentIF.GetPosition( TargetFragmentTileBase+2 ) ;

	BegLogLine(PKFXLOG_IPARTPAIR)
          << "Full tile sources " << SiteImagesA[0] << SiteImagesA[1] << SiteImagesA[2]
          << " targets " << SitesB[0] << SitesB[1] << SitesB[2]
          << EndLogLine ;

	double dsq0 = (SiteImagesA[0]-SitesB[0]).LengthSquared() ;
	double dsq1 = (SiteImagesA[0]-SitesB[1]).LengthSquared() ;
	double dsq2 = (SiteImagesA[0]-SitesB[2]).LengthSquared() ;
	double dsq3 = (SiteImagesA[1]-SitesB[0]).LengthSquared() ;
	double dsq4 = (SiteImagesA[1]-SitesB[1]).LengthSquared() ;
	double dsq5 = (SiteImagesA[1]-SitesB[2]).LengthSquared() ;
	double dsq6 = (SiteImagesA[2]-SitesB[0]).LengthSquared() ;
	double dsq7 = (SiteImagesA[2]-SitesB[1]).LengthSquared() ;
	double dsq8 = (SiteImagesA[2]-SitesB[2]).LengthSquared() ;

	PairList.DistanceSquared[0] = dsq0 ;
	PairList.DistanceSquared[1] = dsq1 ;
	PairList.DistanceSquared[2] = dsq2 ;
	PairList.DistanceSquared[3] = dsq3 ;
	PairList.DistanceSquared[4] = dsq4 ;
	PairList.DistanceSquared[5] = dsq5 ;
	PairList.DistanceSquared[6] = dsq6 ;
	PairList.DistanceSquared[7] = dsq7 ;
	PairList.DistanceSquared[8] = dsq8 ;

	BegLogLine (PKFXLOG_IPARTPAIR)
	  << "Tile square distances"
	  << ' ' << PairList.DistanceSquared[0]
	  << ' ' << PairList.DistanceSquared[1]
	  << ' ' << PairList.DistanceSquared[2]
	  << ' ' << PairList.DistanceSquared[3]
	  << ' ' << PairList.DistanceSquared[4]
	  << ' ' << PairList.DistanceSquared[5]
	  << ' ' << PairList.DistanceSquared[6]
	  << ' ' << PairList.DistanceSquared[7]
	  << ' ' << PairList.DistanceSquared[8]
	  << EndLogLine ;

	double rd0 = 1.0/sqrt(dsq0) ;
	double rd1 = 1.0/sqrt(dsq1) ;
	double rd2 = 1.0/sqrt(dsq2) ;
	double rd3 = 1.0/sqrt(dsq3) ;
	double rd4 = 1.0/sqrt(dsq4) ;
	double rd5 = 1.0/sqrt(dsq5) ;
	double rd6 = 1.0/sqrt(dsq6) ;
	double rd7 = 1.0/sqrt(dsq7) ;
	double rd8 = 1.0/sqrt(dsq8) ;

	PairList.ReciprocalDistance[0] = rd0 ;
	PairList.ReciprocalDistance[1] = rd1 ;
	PairList.ReciprocalDistance[2] = rd2 ;
	PairList.ReciprocalDistance[3] = rd3 ;
	PairList.ReciprocalDistance[4] = rd4 ;
	PairList.ReciprocalDistance[5] = rd5 ;
	PairList.ReciprocalDistance[6] = rd6 ;
	PairList.ReciprocalDistance[7] = rd7 ;
	PairList.ReciprocalDistance[8] = rd8 ;

	BegLogLine (PKFXLOG_IPARTPAIR)
	  << "Tile reciprocal distances"
	  << ' ' << PairList.ReciprocalDistance[0]
	  << ' ' << PairList.ReciprocalDistance[1]
	  << ' ' << PairList.ReciprocalDistance[2]
	  << ' ' << PairList.ReciprocalDistance[3]
	  << ' ' << PairList.ReciprocalDistance[4]
	  << ' ' << PairList.ReciprocalDistance[5]
	  << ' ' << PairList.ReciprocalDistance[6]
	  << ' ' << PairList.ReciprocalDistance[7]
	  << ' ' << PairList.ReciprocalDistance[8]
	  << EndLogLine ;


	// The integer arithmetic unit was looking a  little left out. So ...

	// 'DistanceSquared' are all positive.
	// For positive numbers, IEE 'double' values collate the same way
	// as 64-bit integers with the same bit pattern. (Strange but true, or
	// just possibly by design)
	// For purposes of looking at whether an atom pair is in switch, or in
	// range, it issufficient to look at the top 32 bits.
	int idsq0 = *(int *)(PairList.DistanceSquared+0) ;
	int idsq1 = *(int *)(PairList.DistanceSquared+1) ;
	int idsq2 = *(int *)(PairList.DistanceSquared+2) ;
	int idsq3 = *(int *)(PairList.DistanceSquared+3) ;
	int idsq4 = *(int *)(PairList.DistanceSquared+4) ;
	int idsq5 = *(int *)(PairList.DistanceSquared+5) ;
	int idsq6 = *(int *)(PairList.DistanceSquared+6) ;
	int idsq7 = *(int *)(PairList.DistanceSquared+7) ;
	int idsq8 = *(int *)(PairList.DistanceSquared+8) ;

	BegLogLine (PKFXLOG_IPARTPAIR)
	  << "Tile idsq"
	  << ' ' << idsq0
	  << ' ' << idsq1
	  << ' ' << idsq2
	  << ' ' << idsq3
	  << ' ' << idsq4
	  << ' ' << idsq5
	  << ' ' << idsq6
	  << ' ' << idsq7
	  << ' ' << idsq8
	  << EndLogLine ;


	// No need to set up source/target indexes if we are doing the whole tile
	int pos_needs_switch_0 = idsq0 - iOnRegionSquared ;
	int pos_needs_switch_1 = idsq1 - iOnRegionSquared ;
	int pos_needs_switch_2 = idsq2 - iOnRegionSquared ;
	int pos_needs_switch_3 = idsq3 - iOnRegionSquared ;
	int pos_needs_switch_4 = idsq4 - iOnRegionSquared ;
	int pos_needs_switch_5 = idsq5 - iOnRegionSquared ;
	int pos_needs_switch_6 = idsq6 - iOnRegionSquared ;
	int pos_needs_switch_7 = idsq7 - iOnRegionSquared ;
	int pos_needs_switch_8 = idsq8 - iOnRegionSquared ;

	int pos_needs_switch =  pos_needs_switch_0
	  & pos_needs_switch_1
	  &pos_needs_switch_2
	  & pos_needs_switch_3
	  & pos_needs_switch_4
	  & pos_needs_switch_5
	  & pos_needs_switch_6
	  & pos_needs_switch_7
	  & pos_needs_switch_8 ;
	BegLogLine (PKFXLOG_IPARTPAIR)
	  << "Tile pos_needs_switch"
	  << ' ' << pos_needs_switch_0
	  << ' ' << pos_needs_switch_1
	  << ' ' << pos_needs_switch_2
	  << ' ' << pos_needs_switch_3
	  << ' ' << pos_needs_switch_4
	  << ' ' << pos_needs_switch_5
	  << ' ' << pos_needs_switch_6
	  << ' ' << pos_needs_switch_7
	  << ' ' << pos_needs_switch_8
	  << EndLogLine ;

	AnyInSwitch =  pos_needs_switch ;

	int pos_in_range_0 = iOffRegionSquared - idsq0 ;
	int pos_in_range_1 = iOffRegionSquared - idsq1 ;
	int pos_in_range_2 = iOffRegionSquared - idsq2 ;
	int pos_in_range_3 = iOffRegionSquared - idsq3 ;
	int pos_in_range_4 = iOffRegionSquared - idsq4 ;
	int pos_in_range_5 = iOffRegionSquared - idsq5 ;
	int pos_in_range_6 = iOffRegionSquared - idsq6 ;
	int pos_in_range_7 = iOffRegionSquared - idsq7 ;
	int pos_in_range_8 = iOffRegionSquared - idsq8 ;

	int out_range_0 = ((unsigned int)pos_in_range_0) >> 31 ;
	int out_range_1 = ((unsigned int)pos_in_range_1) >> 31 ;
	int out_range_2 = ((unsigned int)pos_in_range_2) >> 31 ;
	int out_range_3 = ((unsigned int)pos_in_range_3) >> 31 ;
	int out_range_4 = ((unsigned int)pos_in_range_4) >> 31 ;
	int out_range_5 = ((unsigned int)pos_in_range_5) >> 31 ;
	int out_range_6 = ((unsigned int)pos_in_range_6) >> 31 ;
	int out_range_7 = ((unsigned int)pos_in_range_7) >> 31 ;
	int out_range_8 = ((unsigned int)pos_in_range_8) >> 31 ;

	BegLogLine (PKFXLOG_IPARTPAIR)
	  << "Tile pos_in_range"
	  << ' ' << pos_in_range_0
	  << ' ' << pos_in_range_1
	  << ' ' << pos_in_range_2
	  << ' ' << pos_in_range_3
	  << ' ' << pos_in_range_4
	  << ' ' << pos_in_range_5
	  << ' ' << pos_in_range_6
	  << ' ' << pos_in_range_7
	  << ' ' << pos_in_range_8
	  << EndLogLine ;

	int out_range_count = out_range_0
	  + out_range_1
	  + out_range_2
	  + out_range_3
	  + out_range_4
	  + out_range_5
	  + out_range_6
	  + out_range_7
	  + out_range_8 ;

	BegLogLine(PKFXLOG_IPARTPAIR)
	  << "out_range_count " << out_range_count
	  << " pos_needs_switch " << pos_needs_switch
	  << " AnyInSwitch" << AnyInSwitch
	  << EndLogLine ;

	// With a following wind, we will now find we have enough work to do to run the whole tile
	if ( out_range_count <= k_OutRangeThreshold )
	  {
	    // Yes, we can run the whole tile.
	    LoopResult PairdEdr0[k_TileArea] ;
	    LoopResult PairdEdr1[k_TileArea] ;
	    LoopResult PairdEdr2[k_TileArea] ;
	    LoopResult PairEnergy0[k_TileArea] ;
	    LoopResult PairEnergy1[k_TileArea] ;
	    LoopResult PairEnergy2[k_TileArea] ;
	    LoopResult PairExtraScalarVirial0[k_TileArea] ;
	    LoopResult PairExtraScalarVirial1[k_TileArea] ;
	    LoopResult PairExtraScalarVirial2[k_TileArea] ;
	    int SourceAbsSiteIds[k_TileArea] ;
	    int TargetAbsSiteIds[k_TileArea] ;
	    int SourceAbsSiteId0 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+0);
	    int SourceAbsSiteId1 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+1);
	    int SourceAbsSiteId2 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+2);
	    int TargetAbsSiteId0 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+0);
	    int TargetAbsSiteId1 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+1);
	    int TargetAbsSiteId2 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+2);
	    SourceAbsSiteIds[0] = SourceAbsSiteId0 ;
	    SourceAbsSiteIds[1] = SourceAbsSiteId0 ;
	    SourceAbsSiteIds[2] = SourceAbsSiteId0 ;
	    SourceAbsSiteIds[3] = SourceAbsSiteId1 ;
	    SourceAbsSiteIds[4] = SourceAbsSiteId1 ;
	    SourceAbsSiteIds[5] = SourceAbsSiteId1 ;
	    SourceAbsSiteIds[6] = SourceAbsSiteId2 ;
	    SourceAbsSiteIds[7] = SourceAbsSiteId2 ;
	    SourceAbsSiteIds[8] = SourceAbsSiteId2 ;
	    TargetAbsSiteIds[0] = TargetAbsSiteId0 ;
	    TargetAbsSiteIds[1] = TargetAbsSiteId1 ;
	    TargetAbsSiteIds[2] = TargetAbsSiteId2 ;
	    TargetAbsSiteIds[3] = TargetAbsSiteId0 ;
	    TargetAbsSiteIds[4] = TargetAbsSiteId1 ;
	    TargetAbsSiteIds[5] = TargetAbsSiteId2 ;
	    TargetAbsSiteIds[6] = TargetAbsSiteId0 ;
	    TargetAbsSiteIds[7] = TargetAbsSiteId1 ;
	    TargetAbsSiteIds[8] = TargetAbsSiteId2 ;

	    // Evaluate the unswitched forces

	    for ( unsigned int PairListIndex = 0 ; PairListIndex < k_TileArea ; PairListIndex += 1)
	      {
#pragma execution_frequency(very_high)
		if( UDF0_Bound::UDF::Code != -1 ) {
		  ExecuteWrapper<0, UDF0_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
												SourceAbsSiteIds[PairListIndex],
												TargetAbsSiteIds[PairListIndex],
												PairList.DistanceSquared[PairListIndex],
												PairList.ReciprocalDistance[PairListIndex],
												PairEnergy0[PairListIndex].mValue,
												PairdEdr0[PairListIndex].mValue,
												PairExtraScalarVirial0[PairListIndex].mValue,
												aSimTick
												) ;
		}
		if( UDF1_Bound::UDF::Code != -1 ) {
		  ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
												SourceAbsSiteIds[PairListIndex],
												TargetAbsSiteIds[PairListIndex],
												PairList.DistanceSquared[PairListIndex],
												PairList.ReciprocalDistance[PairListIndex],
												PairEnergy1[PairListIndex].mValue,
												PairdEdr1[PairListIndex].mValue,
												PairExtraScalarVirial1[PairListIndex].mValue,
												aSimTick
												) ;
		}
		if( UDF1_Bound::UDF::Code != -1 ) {

		  ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
												SourceAbsSiteIds[PairListIndex],
												TargetAbsSiteIds[PairListIndex],
												PairList.DistanceSquared[PairListIndex],
												PairList.ReciprocalDistance[PairListIndex],
												PairEnergy2[PairListIndex].mValue,
												PairdEdr2[PairListIndex].mValue,
												PairExtraScalarVirial2[PairListIndex].mValue,
												aSimTick
												) ;

		}

	      } /* endfor PairListIndex */
	    LoopResult PairdEdr[k_TileArea];
	    LoopResult PairExtraScalarVirial[k_TileArea] ;
	    { // Add up the partial dE/dr values
	      for ( unsigned int PairListIndex = 0 ; PairListIndex < k_TileArea ; PairListIndex += 1)
		{
		  PairdEdr[PairListIndex].mValue =
		    ( ( UDF0_Bound::UDF::Code != -1 ) ? PairdEdr0[PairListIndex].mValue : 0.0 )
		    +( ( UDF1_Bound::UDF::Code != -1 ) ? PairdEdr1[PairListIndex].mValue : 0.0 )
		    +( ( UDF2_Bound::UDF::Code != -1 ) ? PairdEdr2[PairListIndex].mValue : 0.0 ) ;
		  if ( k_DoPressureControl )
		    {

		      PairExtraScalarVirial[PairListIndex].mValue =
			( ( UDF0_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial0[PairListIndex].mValue : 0.0 )
			+( ( UDF1_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial1[PairListIndex].mValue : 0.0 )
			+ ( ( UDF2_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial2[PairListIndex].mValue : 0.0 ) ;
		    }
		}
	    }
	    // We rather hope not to need the switch, but just in case we do here is the code
	    if (pos_needs_switch >= 0 )
	      {
		// Something is in the switch zone; need to do the whole calculation
		for ( unsigned int px0 = 0 ; px0 < k_TileArea ; px0 += 1)
		  {
#pragma execution_frequency(very_high)
		    NBVM_SWITCHEVALUATOR switchev ;
		    switchev.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );

		    switchev.Prime(PairList.DistanceSquared[px0]
				   ,PairList.ReciprocalDistance[px0]
				   );
		    ExecuteWrapper<2, UDF2_Bound, NBVM_SWITCHEVALUATOR>::ExecuteSwitchOnlyUnconditional( switchev);



		    double S = switchev.GetSwitchS() ;
		    double pdSdr = switchev.GetSwitchdSdR() ;

		    double EnergyUnswitched0 = PairEnergy0[px0].mValue ;
		    double EnergyUnswitched1 = PairEnergy1[px0].mValue ;
		    double EnergyUnswitched2 = PairEnergy2[px0].mValue ;

		    double EnergyUnswitched =
		      ( UDF0_Bound::UDF::ReportsEnergy ? EnergyUnswitched0 : 0.0 )
		      + ( UDF1_Bound::UDF::ReportsEnergy ? EnergyUnswitched1 : 0.0 )
		      + ( UDF2_Bound::UDF::ReportsEnergy ? EnergyUnswitched2 : 0.0 ) ;
		    //                                  double EnergyUnswitched = EnergyUnswitched0 + EnergyUnswitched1 + EnergyUnswitched2 ;

		    double dEdrUnswitched = PairdEdr[px0].mValue ;
		    double ExtraScalarVirialUnswitched = PairExtraScalarVirial[px0].mValue ;

		    double dEdrSwitched = dEdrUnswitched * S + EnergyUnswitched * pdSdr ;
		    double EnergySwitched0 = EnergyUnswitched0 * S ;
		    double EnergySwitched1 = EnergyUnswitched1 * S ;
		    double EnergySwitched2 = EnergyUnswitched2 * S ;             \
										   double ExtraScalarVirialSwitched =ExtraScalarVirialUnswitched * S ;


		    PairdEdr[px0].mValue = dEdrSwitched;
		    if ( UDF0_Bound::UDF::ReportsEnergy ) {
		      PairEnergy0[px0].mValue = EnergySwitched0  ;
		    }
		    if ( UDF1_Bound::UDF::ReportsEnergy ) {
		      PairEnergy1[px0].mValue = EnergySwitched1  ;
		    }
		    if ( UDF2_Bound::UDF::ReportsEnergy ) {
		      PairEnergy2[px0].mValue = EnergySwitched2  ;
		    }
		    PairExtraScalarVirial[px0].mValue = ExtraScalarVirialSwitched ;

		  } /* endfor px0 */

	      } /* endif AnyInSwitch > 0.0 */
	    // Scatter out the vector, so the total atom forces can be calculated
	    XYZ VirialSum ;
	    double ExtraScalarVirial ;
	    if (k_DoPressureControl)
	      {
		VirialSum.Zero() ;
		ExtraScalarVirial = 0.0 ;
	      }
	    {
	      unsigned int px1 = 0 ;
	      for (unsigned int si3=0;si3<k_TileSize;si3+=1)
		{
		  for (int ti3=0;ti3<k_TileSize;ti3+=1)
		    {
		      XYZ dVecAB = SiteImagesA[si3] - SitesB[ti3] ;
		      double dVecABMagR = PairList.ReciprocalDistance[px1];
		      XYZ aForcePartial = dVecAB * (-PairdEdr[px1].mValue*dVecABMagR);

		      BegLogLine(PKFXLOG_IPARTPAIR)
			<< "PairdEdr " << PairdEdr[px1].mValue
			<< EndLogLine ;

		      BegLogLine(PKFXLOG_ENERGYTRACE)
			<< "EnergyTrace "
			<< ' ' << SourceAbsSiteIds[px1]
			<< ' ' << TargetAbsSiteIds[px1]
			<< ' ' << (UDF0_Bound::UDF::ReportsEnergy ? PairEnergy0[px1].mValue : 0.0 )
			<< ' ' << (UDF1_Bound::UDF::ReportsEnergy ?PairEnergy1[px1].mValue : 0.0 )
			<< ' ' << (UDF2_Bound::UDF::ReportsEnergy ?PairEnergy2[px1].mValue : 0.0 )
			<< EndLogLine ;

		      ForcePartial  [ si3 ][ ti3 ] = aForcePartial;

		      if ( UDF0_Bound::UDF::ReportsEnergy ) {
			EnergyPartial0[ si3 ][ ti3 ] = PairEnergy0[px1].mValue ;
		      }
		      if ( UDF1_Bound::UDF::ReportsEnergy ) {
			EnergyPartial1[ si3 ][ ti3 ] = PairEnergy1[px1].mValue ;
		      }
		      if ( UDF2_Bound::UDF::ReportsEnergy ) {
			EnergyPartial2[ si3 ][ ti3 ] = PairEnergy2[px1].mValue ;
		      }
		      if (k_DoPressureControl)
			{
			  VirialSum += PairwiseProduct( aForcePartial, dVecAB ) ;
			  ExtraScalarVirial += PairExtraScalarVirial[px1].mValue ;
			  BegLogLine(PKFXLOG_VIRIALTRACE)
			    << "VirialTrace "
			    << ' ' << SourceAbsSiteIds[px1]
			    << ' ' << TargetAbsSiteIds[px1]
			    << ' ' << PairwiseProduct( aForcePartial, dVecAB )
			    << ' ' << SetToScalar(PairExtraScalarVirial[px1].mValue * (1.0/3.0) )
			    << EndLogLine ;

			}

		      px1 += 1 ;

		    } /* endfor ti3 */
		} /* endfor si3 */
	    }
	    aGlobalResultIF.AddVirial<k_DoPressureControl>( (VirialSum + SetToScalar(ExtraScalarVirial * (1.0/3.0) ) )
							    * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
							    ) ;

	    double Energy0 = 0.0 ;
	    double Energy1 = 0.0 ;
	    double Energy2 = 0.0 ;
	    XYZ TFOS[k_TileSize] ;
	    XYZ TFOT[k_TileSize] ;
	    // Compute the total forces on the atoms.
	    // If the tile was smaller than 3x3, we will add some zeros. But it is
	    // better this way, to get loops with constant iteration counts
	    for ( int a=0;a<k_TileSize;a+=1 )
	      {
		XYZ TotalForceOnSource ;
		XYZ TotalForceOnTarget ;
		TotalForceOnSource=ForcePartial[a][0] ;
		TotalForceOnTarget = ForcePartial[0][a] ;
		if ( UDF0_Bound::UDF::ReportsEnergy ) {
		  Energy0 += EnergyPartial0[a][0] ;
		}
		if ( UDF1_Bound::UDF::ReportsEnergy ) {
		  Energy1 += EnergyPartial1[a][0] ;
		}
		if ( UDF2_Bound::UDF::ReportsEnergy ) {
		  Energy2 += EnergyPartial2[a][0] ;
		}
		for ( int b=1;b<k_TileSize;b+=1)
		  {
		    TotalForceOnSource += ForcePartial[a][b] ;
		    TotalForceOnTarget += ForcePartial[b][a] ;
		    if ( UDF0_Bound::UDF::ReportsEnergy ) {
		      Energy0 += EnergyPartial0[a][b] ;
		    }
		    if ( UDF0_Bound::UDF::ReportsEnergy ) {
		      Energy1 += EnergyPartial1[a][b] ;
		    }
		    if ( UDF0_Bound::UDF::ReportsEnergy ) {
		      Energy2 += EnergyPartial2[a][b] ;
		    }
		  }
		TFOS[a] = TotalForceOnSource ;
		TFOT[a] = TotalForceOnTarget ;
	      }
	    aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, Energy0 );
	    aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, Energy1 );
	    aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, Energy2 );
	    int SourceSiteId0 = MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+0) ;
	    int SourceSiteId1 = MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+1) ;
	    int SourceSiteId2 = MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+2) ;
	    int TargetSiteId0 = MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+0) ;
	    int TargetSiteId1 = MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+1) ;
	    int TargetSiteId2 = MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+2) ;
	    aSourceResultIF.AddForce(SourceSiteId0,0,TFOS[0]) ;
	    aSourceResultIF.AddForce(SourceSiteId1,0,TFOS[1]) ;
	    aSourceResultIF.AddForce(SourceSiteId2,0,TFOS[2]) ;
	    aTargetResultIF.AddForce(TargetSiteId0,0,-TFOT[0]) ;
	    aTargetResultIF.AddForce(TargetSiteId1,0,-TFOT[1]) ;
	    aTargetResultIF.AddForce(TargetSiteId2,0,-TFOT[2]) ;

	    return ; // exit point for 'full tile' processing

	  } else {
	    // No, enough of the tile is out of range that we should compress and use the slow way


	    {
	      // Compress the evaluations so that only inrange ones are done

	      int ir0 = 1-out_range_0 ;
	      int ir1 = 1-out_range_1 ;
	      int ir2 = 1-out_range_2 ;
	      int ir3 = 1-out_range_3 ;
	      int ir4 = 1-out_range_4 ;
	      int ir5 = 1-out_range_5 ;
	      int ir6 = 1-out_range_6 ;
	      int ir7 = 1-out_range_7 ;
	      int ir8 = 1-out_range_8 ;

	      PairList.DistanceSquared[0] = dsq0 ;         PairList.ReciprocalDistance[0] = rd0 ;  PairList.mPair[0].SourceIndex = 0 ;  PairList.mPair[0].TargetIndex = 0 ; int q  = ir0 ;
	      PairList.DistanceSquared[q] = dsq1 ;         PairList.ReciprocalDistance[q] = rd1 ;  PairList.mPair[q].SourceIndex = 0 ;  PairList.mPair[q].TargetIndex = 1 ; q += ir1 ;
	      PairList.DistanceSquared[q] = dsq2 ;         PairList.ReciprocalDistance[q] = rd2 ;  PairList.mPair[q].SourceIndex = 0 ;  PairList.mPair[q].TargetIndex = 2 ; q += ir2 ;
	      PairList.DistanceSquared[q] = dsq3 ;         PairList.ReciprocalDistance[q] = rd3 ;  PairList.mPair[q].SourceIndex = 1 ;  PairList.mPair[q].TargetIndex = 0 ; q += ir3 ;
	      PairList.DistanceSquared[q] = dsq4 ;         PairList.ReciprocalDistance[q] = rd4 ;  PairList.mPair[q].SourceIndex = 1 ;  PairList.mPair[q].TargetIndex = 1 ; q += ir4 ;
	      PairList.DistanceSquared[q] = dsq5 ;         PairList.ReciprocalDistance[q] = rd5 ;  PairList.mPair[q].SourceIndex = 1 ;  PairList.mPair[q].TargetIndex = 2 ; q += ir5 ;
	      PairList.DistanceSquared[q] = dsq6 ;         PairList.ReciprocalDistance[q] = rd6 ;  PairList.mPair[q].SourceIndex = 2 ;  PairList.mPair[q].TargetIndex = 0 ; q += ir6 ;
	      PairList.DistanceSquared[q] = dsq7 ;         PairList.ReciprocalDistance[q] = rd7 ;  PairList.mPair[q].SourceIndex = 2 ;  PairList.mPair[q].TargetIndex = 1 ; q += ir7 ;
	      PairList.DistanceSquared[q] = dsq8 ;         PairList.ReciprocalDistance[q] = rd8 ;  PairList.mPair[q].SourceIndex = 2 ;  PairList.mPair[q].TargetIndex = 2 ; q += ir8 ;

	      PairListCount = q ;

	      // Clear out the partial forces
	      for (int si2=0;si2<k_TileSize;si2+=1)
		{
		  for (int ti2=0;ti2<k_TileSize;ti2+=1)
		    {
		      ForcePartial[si2][ti2].Zero();
		      EnergyPartial0[si2][ti2] = 0.0 ;
		      EnergyPartial1[si2][ti2] = 0.0 ;
		      EnergyPartial2[si2][ti2] = 0.0 ;
		    }
		}


	    }

	  }

	silim = k_TileSize ;
	tilim = k_TileSize ;

      }
    else
      {
	silim = mymin(k_TileSize,SourceFragmentSiteCount-SourceFragmentTileBase);
	tilim = mymin(k_TileSize,TargetFragmentSiteCount-TargetFragmentTileBase) ;

	// Partial tile, do it the slow but general way
	// Pick up values for SA0,1,2 and SB0,1,2 which are ...
	//   'natural' when inside the partial tile
	//   'not going to cause a problem' when outside the tile
	//   'branch-free'

	XYZ SA0 =  aSourceFragmentIF.GetPosition( SourceFragmentTileBase+0 ) ;
	int sa1_index= ( silim > 1 ) ? 1 : 0 ;
	int sa2_index= ( silim > 2 ) ? 2 : 0 ;
	XYZ SA1 = aSourceFragmentIF.GetPosition( SourceFragmentTileBase+sa1_index) ;
	XYZ SA2 = aSourceFragmentIF.GetPosition( SourceFragmentTileBase+sa2_index) ;

	SiteImagesA[0] = SA0 - FragmentImageVector ;
	SiteImagesA[1] = SA1 - FragmentImageVector ;
	SiteImagesA[2] = SA2 - FragmentImageVector ;

	XYZ SB0 = aTargetFragmentIF.GetPosition( TargetFragmentTileBase+0 ) ;
	int sb1_index= ( tilim > 1 ) ? 1 : 0 ;
	int sb2_index= ( tilim > 2 ) ? 2 : 0 ;
	XYZ SB1 = aTargetFragmentIF.GetPosition( TargetFragmentTileBase+sb1_index ) ;
	XYZ SB2 = aTargetFragmentIF.GetPosition( TargetFragmentTileBase+sb2_index ) ;

	SitesB[0] = SB0 ;
	SitesB[1] = SB1 ;
	SitesB[2] = SB2 ;

	// Run the whole tile, figuring which pairs are to have forces evaluated
	double dAnyInSwitch = -1 ;
	for (int si2=0;si2<k_TileSize;si2+=1)
	  {
	    unsigned int si_in_range = si2 - silim ;
	    for (int ti2=0;ti2<k_TileSize;ti2+=1)
	      {
		unsigned int ti_in_range = ti2 - tilim ;
		unsigned int in_range = ( si_in_range & ti_in_range ) >> 31 ;
		XYZ Vector = SiteImagesA[si2] - SitesB[ti2] ;
		double DistanceSquared = Vector.LengthSquared() ;
		PairList.mPair[PairListCount].SourceIndex = si2 ;
		PairList.mPair[PairListCount].TargetIndex = ti2 ;
		PairList.DistanceSquared[PairListCount] = DistanceSquared ;
		double InRegion = OffRegionSquared - DistanceSquared ;
		double PairlistIndexAdd = fsel(InRegion,1.0,0.0) ;
		int iPairlistIndexAdd = PairlistIndexAdd ;
		int IndexAdd = iPairlistIndexAdd & in_range ;
		PairListCount += IndexAdd ;
		/*
		 * If some atom pairs are outside the upper cutoff, and the rest are inside the lower cutoff,
		 * this will result in full switch calculations being done; which is inefficient, but rare.
		 * The alternative would be to take another spin through the vector every time; probably
		 * less efficient on average.
		 */
		dAnyInSwitch = fsel(DistanceSquared-OnRegionSquared,1.0,dAnyInSwitch) ;
		BegLogLine( PKFXLOG_IPARTPAIR )
		  << "IFPTiled: "
		  << " SiteImageA=" << SiteImagesA[si2]
		  << " SiteB=" << SitesB[ti2]
		  << " Vector=" << Vector
		  << EndLogLine;


		BegLogLine(PKFXLOG_IPARTPAIR)
		  << "SquareDistances"
		  << " SourceFragmentSiteIndex=" << si2
		  << " TargetFragmentSiteIndex=" << ti2
		  << " DistanceSquared=" << DistanceSquared
		  << " InRegion=" << InRegion
		  << " in_range=" << in_range
		  << " PairlistIndexAdd=" << PairlistIndexAdd
		  << " IndexAdd=" << IndexAdd
		  << " PairListCount=" << PairListCount
		  << " dAnyInSwitch=" << dAnyInSwitch
		  << EndLogLine;

		ForcePartial[si2][ti2].Zero();
		EnergyPartial0[si2][ti2] = 0.0 ;
		EnergyPartial1[si2][ti2] = 0.0 ;
		EnergyPartial2[si2][ti2] = 0.0 ;
	      }
	  }
	PaddableVectorReciprocalSquareRoot(PairList.ReciprocalDistance,PairList.DistanceSquared,PairListCount) ;
	AnyInSwitch = ( dAnyInSwitch > 0.0 ) ? 0 : -1 ;
      }


    // See if we have any work to do; it is possible that everything was outside range
    if ( PairListCount > 0 )
      {
	LoopResult PairdEdr0[k_TileArea] ;
	LoopResult PairdEdr1[k_TileArea] ;
	LoopResult PairdEdr2[k_TileArea] ;
	LoopResult PairEnergy0[k_TileArea] ;
	LoopResult PairEnergy1[k_TileArea] ;
	LoopResult PairEnergy2[k_TileArea] ;
	LoopResult PairExtraScalarVirial0[k_TileArea] ;
	LoopResult PairExtraScalarVirial1[k_TileArea] ;
	LoopResult PairExtraScalarVirial2[k_TileArea] ;
	int SourceAbsSiteIds[k_TileArea] ;
	int TargetAbsSiteIds[k_TileArea] ;

	// Set up the atoms
	for ( unsigned int px2 = 0 ; px2 < PairListCount ; px2 += 1)
	  {
	    unsigned int SourceIndex = PairList.mPair[px2].SourceIndex ;
	    unsigned int TargetIndex = PairList.mPair[px2].TargetIndex ;

	    int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+SourceIndex);
	    int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+TargetIndex);

	    SourceAbsSiteIds[px2] = SourceAbsSiteId ;
	    TargetAbsSiteIds[px2] = TargetAbsSiteId ;
	  }
	// Evaluate the unswitched forces

	for ( unsigned int PairListIndex = 0 ; PairListIndex < PairListCount ; PairListIndex += 1)
	  {
            // UDF 0
	    if( UDF0_Bound::UDF::Code != -1 ) {
	      ExecuteWrapper<0, UDF0_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
											    SourceAbsSiteIds[PairListIndex],
											    TargetAbsSiteIds[PairListIndex],
											    PairList.DistanceSquared[PairListIndex],
											    PairList.ReciprocalDistance[PairListIndex],
											    PairEnergy0[PairListIndex].mValue,
											    PairdEdr0[PairListIndex].mValue,
											    PairExtraScalarVirial0[PairListIndex].mValue,
											    aSimTick
											    ) ;

	    } /* endif UDF Code */

            // UDF 1
	    if( UDF1_Bound::UDF::Code != -1 ) {
	      ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
											    SourceAbsSiteIds[PairListIndex],
											    TargetAbsSiteIds[PairListIndex],
											    PairList.DistanceSquared[PairListIndex],
											    PairList.ReciprocalDistance[PairListIndex],
											    PairEnergy1[PairListIndex].mValue,
											    PairdEdr1[PairListIndex].mValue,
											    PairExtraScalarVirial1[PairListIndex].mValue,
											    aSimTick
											    ) ;
	    } /* endif UDF Code */

            // UDF 2
	    if( UDF2_Bound::UDF::Code != -1 ) {
	      ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
											    SourceAbsSiteIds[PairListIndex],
											    TargetAbsSiteIds[PairListIndex],
											    PairList.DistanceSquared[PairListIndex],
											    PairList.ReciprocalDistance[PairListIndex],
											    PairEnergy2[PairListIndex].mValue,
											    PairdEdr2[PairListIndex].mValue,
											    PairExtraScalarVirial2[PairListIndex].mValue,
											    aSimTick
											    ) ;
	    } /* endif UDF Code */

	  } /* endfor PairListIndex */

	LoopResult PairdEdr[k_TileArea];
	LoopResult PairExtraScalarVirial[k_TileArea] ;
	{ // Add up the partial dE/dr values
	  for ( unsigned int PairListIndex = 0 ; PairListIndex < PairListCount ; PairListIndex += 1)
	    {
	      PairdEdr[PairListIndex].mValue =
		( ( UDF0_Bound::UDF::Code != -1 ) ? PairdEdr0[PairListIndex].mValue : 0.0 )
		+( ( UDF1_Bound::UDF::Code != -1 ) ? PairdEdr1[PairListIndex].mValue : 0.0 )
		+( ( UDF2_Bound::UDF::Code != -1 ) ? PairdEdr2[PairListIndex].mValue : 0.0 ) ;
	      if ( k_DoPressureControl )
		{

		  PairExtraScalarVirial[PairListIndex].mValue =
		    ( ( UDF0_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial0[PairListIndex].mValue : 0.0 )
		    +( ( UDF1_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial1[PairListIndex].mValue : 0.0 )
		    + ( ( UDF2_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial2[PairListIndex].mValue : 0.0 ) ;
		}
	    }
	}


	// If anything is in the switch zone, evaluate and apply the switches
	if (AnyInSwitch >= 0 )
	  {
	    // Something is in the switch zone; need to do the whole calculation
	    for ( unsigned int px0 = 0 ; px0 < PairListCount ; px0 += 1)
	      {
		NBVM_SWITCHEVALUATOR switchev ;
		switchev.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );

		switchev.Prime( PairList.DistanceSquared[px0],
			        PairList.ReciprocalDistance[px0] );

		ExecuteWrapper<2, UDF2_Bound, NBVM_SWITCHEVALUATOR>::ExecuteSwitchOnlyUnconditional( switchev);



		double S = switchev.GetSwitchS() ;
		double pdSdr = switchev.GetSwitchdSdR() ;

		double EnergyUnswitched0 = PairEnergy0[px0].mValue ;
		double EnergyUnswitched1 = PairEnergy1[px0].mValue ;
		double EnergyUnswitched2 = PairEnergy2[px0].mValue ;

		double EnergyUnswitched =
		  ( UDF0_Bound::UDF::ReportsEnergy ? EnergyUnswitched0 : 0.0 )
		  + ( UDF1_Bound::UDF::ReportsEnergy ? EnergyUnswitched1 : 0.0 )
		  + ( UDF2_Bound::UDF::ReportsEnergy ? EnergyUnswitched2 : 0.0 ) ;

		double dEdrUnswitched = PairdEdr[px0].mValue ;
		double ExtraScalarVirialUnswitched = PairExtraScalarVirial[px0].mValue ;

		double dEdrSwitched = dEdrUnswitched * S + EnergyUnswitched * pdSdr ;
		double EnergySwitched0 = EnergyUnswitched0 * S ;
		double EnergySwitched1 = EnergyUnswitched1 * S ;
		double EnergySwitched2 = EnergyUnswitched2 * S ;             \
									       double ExtraScalarVirialSwitched =ExtraScalarVirialUnswitched * S ;


		PairdEdr[px0].mValue = dEdrSwitched;
		if ( UDF0_Bound::UDF::ReportsEnergy ) {
		  PairEnergy0[px0].mValue = EnergySwitched0  ;
		}
		if ( UDF1_Bound::UDF::ReportsEnergy ) {
		  PairEnergy1[px0].mValue = EnergySwitched1  ;
		}
		if ( UDF2_Bound::UDF::ReportsEnergy ) {
		  PairEnergy2[px0].mValue = EnergySwitched2  ;
		}
		PairExtraScalarVirial[px0].mValue = ExtraScalarVirialSwitched ;

	      } /* endfor px0 */

	  } /* endif AnyInSwitch >= 0 */

	// Scatter out the vector, so the total atom forces can be calculated
	XYZ VirialSum ;
	double ExtraScalarVirial ;
	if (k_DoPressureControl)
	  {
	    VirialSum.Zero() ;
	    ExtraScalarVirial = 0.0 ;
	  }
	{
	  for ( unsigned int px1=0; px1<PairListCount;px1 += 1)
	    {
	      unsigned int si3 = PairList.mPair[px1].SourceIndex ;
	      unsigned int ti3 = PairList.mPair[px1].TargetIndex ;
	      XYZ dVecAB = SiteImagesA[si3] - SitesB[ti3] ;

	      double dVecABMagR = PairList.ReciprocalDistance[px1];
	      XYZ aForcePartial = dVecAB * (-PairdEdr[px1].mValue*dVecABMagR);

	      BegLogLine(PKFXLOG_IPARTPAIR)
		<< "PairdEdr " << PairdEdr[px1].mValue
		<< EndLogLine ;

	      BegLogLine(PKFXLOG_ENERGYTRACE)
		<< "EnergyTrace "
		<< ' ' << SourceAbsSiteIds[px1]
		<< ' ' << TargetAbsSiteIds[px1]
		<< ' ' << (UDF0_Bound::UDF::ReportsEnergy ? PairEnergy0[px1].mValue : 0.0 )
		<< ' ' << (UDF1_Bound::UDF::ReportsEnergy ? PairEnergy1[px1].mValue : 0.0 )
		<< ' ' << (UDF2_Bound::UDF::ReportsEnergy ? PairEnergy2[px1].mValue : 0.0 )
		<< EndLogLine ;

	      ForcePartial  [ si3 ][ ti3 ] = aForcePartial;

	      if ( UDF0_Bound::UDF::ReportsEnergy ) {
		EnergyPartial0[ si3 ][ ti3 ] = PairEnergy0[px1].mValue ;
	      }
	      if ( UDF1_Bound::UDF::ReportsEnergy ) {
		EnergyPartial1[ si3 ][ ti3 ] = PairEnergy1[px1].mValue ;
	      }
	      if ( UDF2_Bound::UDF::ReportsEnergy ) {
		EnergyPartial2[ si3 ][ ti3 ] = PairEnergy2[px1].mValue ;
	      }
	      if (k_DoPressureControl)
		{
		  VirialSum += PairwiseProduct( aForcePartial, dVecAB ) ;
		  ExtraScalarVirial += PairExtraScalarVirial[px1].mValue ;
		  BegLogLine(PKFXLOG_VIRIALTRACE)
		    << "VirialTrace "
		    << ' ' << SourceAbsSiteIds[px1]
		    << ' ' << TargetAbsSiteIds[px1]
		    << ' ' << PairwiseProduct( aForcePartial, dVecAB )
		    << ' ' << SetToScalar(PairExtraScalarVirial[px1].mValue * (1.0/3.0) )
		    << EndLogLine ;

		}
	    } /* endfor px1 */
	} /* endif full tile evaluation */
	aGlobalResultIF.AddVirial<k_DoPressureControl>( (VirialSum + SetToScalar(ExtraScalarVirial * (1.0/3.0) ) )
							* ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
							) ;
	{
	  double Energy0 = 0.0 ;
	  double Energy1 = 0.0 ;
	  double Energy2 = 0.0 ;
	  XYZ TFOS[k_TileSize] ;
	  XYZ TFOT[k_TileSize] ;
	  // Compute the total forces on the atoms.
	  // If the tile was smaller than 3x3, we will add some zeros. But it is
	  // better this way, to get loops with constant iteration counts
	  for ( int a=0;a<k_TileSize;a+=1 )
	    {
	      XYZ TotalForceOnSource ;
	      XYZ TotalForceOnTarget ;
	      TotalForceOnSource=ForcePartial[a][0] ;
	      TotalForceOnTarget = ForcePartial[0][a] ;
	      if ( UDF0_Bound::UDF::ReportsEnergy ) {
		Energy0 += EnergyPartial0[a][0] ;
	      }
	      if ( UDF1_Bound::UDF::ReportsEnergy ) {
		Energy1 += EnergyPartial1[a][0] ;
	      }
	      if ( UDF2_Bound::UDF::ReportsEnergy ) {
		Energy2 += EnergyPartial2[a][0] ;
	      }
	      for ( int b=1;b<k_TileSize;b+=1)
		{
		  TotalForceOnSource += ForcePartial[a][b] ;
		  TotalForceOnTarget += ForcePartial[b][a] ;
		  if ( UDF0_Bound::UDF::ReportsEnergy ) {
		    Energy0 += EnergyPartial0[a][b] ;
		  }
		  if ( UDF0_Bound::UDF::ReportsEnergy ) {
		    Energy1 += EnergyPartial1[a][b] ;
		  }
		  if ( UDF0_Bound::UDF::ReportsEnergy ) {
		    Energy2 += EnergyPartial2[a][b] ;
		  }
		}
	      TFOS[a] = TotalForceOnSource ;
	      TFOT[a] = TotalForceOnTarget ;
	    }
	  aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, Energy0 );
	  aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, Energy1 );
	  aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, Energy2 );


	  BegLogLine(PKFXLOG_IFPAUDIT)
	    << "Force on source " << TFOS[0]
	    << TFOS[1]
	    << TFOS[2]
	    << EndLogLine;

	  BegLogLine(PKFXLOG_IFPAUDIT)
	    << "Force on target " <<- TFOT[0]
	    << -TFOT[1]
	    << -TFOT[2]
	    << EndLogLine;

	  BegLogLine(PKFXLOG_IFPAUDIT) 
	    << "silim=" << silim
	    << "tilim=" << tilim
	    << EndLogLine ;
	  {
	    /* Partial tile, more care needed in writeback */
	    aSourceResultIF.AddForce(
				     MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+0)
				     , 0
				     , TFOS[0]
				     ) ;
	    if ( silim > 1 )
	      {
		aSourceResultIF.AddForce(
					 MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+1)
					 , 0
					 , TFOS[1]
					 ) ;
		if ( silim > 2 )
		  {
		    aSourceResultIF.AddForce(
					     MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentTileBase+2)
					     , 0
					     , TFOS[2]
					     ) ;
		  }
	      }
	    aTargetResultIF.AddForce(
				     MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+0)
				     , 0
				     , -TFOT[0]
				     ) ;
	    if ( tilim > 1 )
	      {
		aTargetResultIF.AddForce(
					 MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+1)
					 , 0
					 , -TFOT[1]
					 ) ;
		if ( tilim > 2 )
		  {
		    aTargetResultIF.AddForce(
					     MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentTileBase+2)
					     , 0
					     , -TFOT[2]
					     ) ;
		  }
	      }
	  } /* endif full tile */

	  BegLogLine(PKFXLOG_IPARTPAIR)
	    << "Force on source " << TFOS[0]
	    << TFOS[1]
	    << TFOS[2]
	    << EndLogLine;

	  BegLogLine(PKFXLOG_IPARTPAIR)
	    << "Force on target " <<- TFOT[0]
	    << -TFOT[1]
	    << -TFOT[2]
	    << EndLogLine;
	}


      } /* endif PairListCount */



  } /* IFPSimple::ExecuteWithoutExclusionsCountingBothTile */

  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
  static
  inline
  void
  ExecuteWithoutExclusionsCountingBothRectangle( int           aSimTick,
						 GlobalInputIF &aGlobalInputParametersIF,
						 TargetGroupIF aTargetFragmentIF,
						 SourceGroupIF aSourceFragmentIF,
						 FragmentResultIF1     &aTargetResultIF,
						 FragmentResultIF2     &aSourceResultIF,
						 GlobalResultIF       &aGlobalResultIF,
						 const int     aReportForceOnSourceFlag,
						 const XYZ & FragmentImageVector)
  {
#pragma execution_frequency( very_high )
    // This used to be an arguement but seems unused by caller so make local.
    // NEED: change the name so this doesn't look like an argument
    int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
    int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

    // Create a flag to be used to recognize when source and target fragments are the
    // same and thus we can do 1/2 the force term computations by applying the results
    // of each computation to both sits sites.

    int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
    int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair::ExecuteWithoutExclusionsCountingBothRectangle(): STARTING "
      << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
      << " / " << SourceIrreducibleFragmentId
      << " Site count (targ/src) " << TargetFragmentSiteCount << " / " << SourceFragmentSiteCount
      << EndLogLine;

    // NEED: Should create an array of these suckers and fill out the distances
    //  for the whole interaction as a vector computation.

    // Run as 'do' loops because we know we're going to go round at least once; should help the optimiser
    int SourceFragmentTileBase = 0 ;
    do
      {

	int TargetFragmentTileBase = 0 ;
	do
	  {

	    ExecuteWithoutExclusionsCountingBothTile
	      <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF1,FragmentResultIF2,GlobalResultIF>
	      (aSimTick
	       ,aGlobalInputParametersIF
	       ,aTargetFragmentIF
	       ,aSourceFragmentIF
	       ,aTargetResultIF
	       ,aSourceResultIF
	       ,aGlobalResultIF
	       ,aReportForceOnSourceFlag
	       ,FragmentImageVector
	       // ,SourceTargetImageCOMVector
	       ,SourceFragmentTileBase
	       ,TargetFragmentTileBase
	       ,SourceFragmentSiteCount
	       ,TargetFragmentSiteCount
	       ) ;

	    TargetFragmentTileBase += k_TileSize ;
	  } while ( TargetFragmentTileBase < TargetFragmentSiteCount ); /* enddo */
	SourceFragmentTileBase += k_TileSize ;
      } while ( SourceFragmentTileBase < SourceFragmentSiteCount ); /* enddo */
  } /* IFPSimple::ExecuteWithoutExclusionsCountingBothRectangle */

  // We only get here for rectangular slices; 'same' fragment would get to triangle
  // evaluator instead
  // Actually, we don't expect this to be run at all; it is the path where
  // all forces are computed twice, once as F(A,B) and again ad F(B,A)
  // so the path is not well tested. Might be safer to throw an assertion ...
  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1,class FragmentResultIF2, class GlobalResultIF>
  static
  inline
  void
  ExecuteWithoutExclusionsCountingTarget( int           aSimTick,
					  GlobalInputIF &aGlobalInputParametersIF,
					  TargetGroupIF aTargetFragmentIF,
					  SourceGroupIF aSourceFragmentIF,
					  FragmentResultIF1     &aTargetResultIF,
					  FragmentResultIF2     &aSourceResultIF,
					  GlobalResultIF       &aGlobalResultIF,
					  const int     aReportForceOnSourceFlag,
					  const XYZ & FragmentImageVector)
  {
#pragma execution_frequency(very_high)
    int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
    int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair::ExecuteWithoutExclusionsCountingTarget(): STARTING "
      << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
      << " / " << SourceIrreducibleFragmentId
      << EndLogLine;

    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
    int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

    for(int SourceFragmentSiteIndex = 0;
	SourceFragmentSiteIndex < SourceFragmentSiteCount;
	SourceFragmentSiteIndex++ )
      {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        XYZ SiteA = aSourceFragmentIF.GetPosition( SourceFragmentSiteIndex );

        XYZ SiteAImage = SiteA-FragmentImageVector ;


        // Scan the Target Fragment since that should be more local.
        for(int TargetFragmentSiteIndex =  0 ;
            TargetFragmentSiteIndex < TargetFragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
	    BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

	    // 'PairInteractionMask' knocks out those atom pairs which are handled by the bonded force evaluator
	    TotalPairOpsCounter.Increment();

	    // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
	    XYZ SiteB = aTargetFragmentIF.GetPosition( TargetFragmentSiteIndex );
	    XYZ Vector = SiteAImage - SiteB ;

	    BegLogLine( PKFXLOG_IPARTPAIR )
              << "IFPTiled: "
              << " SiteA "
              << SiteA
              << " aTargetFragmentIF.GetPosition( TargetFragmentSiteIndex )= "
              << aTargetFragmentIF.GetPosition( TargetFragmentSiteIndex )
              << " SiteAImage "
              << SiteAImage
              << " Vector "
              << Vector
              << EndLogLine;

	    // The following is probably a very expensive way to find out the AbsSiteId's
	    // - these could be carried in with the Src/TrgFragmentIF
	    int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
	    int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );
	    double DistanceSquared = Vector.LengthSquared() ;
	    double ReciprocalDistance = 1.0/sqrt(DistanceSquared) ;

	    LoopResult PairdEdr0 ;
	    LoopResult PairdEdr1 ;
	    LoopResult PairdEdr2 ;
            LoopResult PairEnergy0 ;
            LoopResult PairEnergy1 ;
            LoopResult PairEnergy2 ;
            LoopResult PairExtraScalarVirial0 ;
            LoopResult PairExtraScalarVirial1 ;
            LoopResult PairExtraScalarVirial2 ;
	    // Evaluate the unswitched forces
	    if( UDF0_Bound::UDF::Code != -1 ) {
	      ExecuteWrapper<0, UDF0_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
											    SourceAbsSiteId,
											    TargetAbsSiteId,
											    DistanceSquared,
											    ReciprocalDistance,
											    PairEnergy0.mValue,
											    PairdEdr0.mValue,
											    PairExtraScalarVirial0.mValue,
											    aSimTick
											    ) ;
	    } /* endif UDF Code */
	    if( UDF1_Bound::UDF::Code != -1 ) {
	      ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
											    SourceAbsSiteId,
											    TargetAbsSiteId,
											    DistanceSquared,
											    ReciprocalDistance,
											    PairEnergy1.mValue,
											    PairdEdr1.mValue,
											    PairExtraScalarVirial1.mValue,
											    aSimTick
											    ) ;
	    } /* endif UDF Code */
	    if( UDF2_Bound::UDF::Code != -1 ) {
	      ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
											    SourceAbsSiteId,
											    TargetAbsSiteId,
											    DistanceSquared,
											    ReciprocalDistance,
											    PairEnergy2.mValue,
											    PairdEdr2.mValue,
											    PairExtraScalarVirial2.mValue,
											    aSimTick
											    ) ;
            } /* endif UDF Code */
            LoopResult PairdEdr;
            LoopResult PairExtraScalarVirial ;
	    PairdEdr.mValue =
	      ( ( UDF0_Bound::UDF::Code != -1 ) ? PairdEdr0.mValue : 0.0 )
	      +( ( UDF1_Bound::UDF::Code != -1 ) ? PairdEdr1.mValue : 0.0 )
	      +( ( UDF2_Bound::UDF::Code != -1 ) ? PairdEdr2.mValue : 0.0 ) ;
	    if ( k_DoPressureControl )
	      {
		PairExtraScalarVirial.mValue =
		  ( ( UDF0_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial0.mValue : 0.0 )
		  +( ( UDF1_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial1.mValue : 0.0 )
		  +( ( UDF2_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial2.mValue : 0.0 ) ;
	      }
            NBVM_SWITCHEVALUATOR switchev ;
            switchev.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );

            switchev.Prime(DistanceSquared,ReciprocalDistance  );
            ExecuteWrapper<2, UDF2_Bound, NBVM_SWITCHEVALUATOR>::ExecuteSwitchOnlyUnconditional( switchev);
            double S = switchev.GetSwitchS() ;
            double pdSdr = switchev.GetSwitchdSdR() ;

            double EnergyUnswitched0 = PairEnergy0.mValue ;
            double EnergyUnswitched1 = PairEnergy1.mValue ;
            double EnergyUnswitched2 = PairEnergy2.mValue ;

            double EnergyUnswitched =
	      ( UDF0_Bound::UDF::ReportsEnergy ? EnergyUnswitched0 : 0.0 )
	      + ( UDF1_Bound::UDF::ReportsEnergy ? EnergyUnswitched1 : 0.0 )
	      + ( UDF2_Bound::UDF::ReportsEnergy ? EnergyUnswitched2 : 0.0 ) ;

            double dEdrUnswitched = PairdEdr.mValue ;
            double ExtraScalarVirialUnswitched = PairExtraScalarVirial.mValue ;

            double dEdrSwitched = dEdrUnswitched * S + EnergyUnswitched * pdSdr ;
	    double EnergySwitched0 = EnergyUnswitched0 * S ;
	    double EnergySwitched1 = EnergyUnswitched1 * S ;
	    double EnergySwitched2 = EnergyUnswitched2 * S ;
	    double ExtraScalarVirialSwitched =ExtraScalarVirialUnswitched * S ;


	    PairdEdr.mValue = dEdrSwitched;
            PairExtraScalarVirial.mValue = ExtraScalarVirialSwitched ;


            // Scatter out the vector, so the total atom forces can be calculated
            XYZ VirialSum ;
            double ExtraScalarVirial ;
            if (k_DoPressureControl)
	      {
                VirialSum.Zero() ;
                ExtraScalarVirial = 0.0 ;
	      }
	    XYZ dVecAB = Vector ;

	    double dVecABMagR = ReciprocalDistance;
	    XYZ aForcePartial = dVecAB * (-PairdEdr.mValue*dVecABMagR);
	    aTargetResultIF.AddForce(
				     MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex)
				     , 0
				     , -aForcePartial
				     ) ;

	    BegLogLine(PKFXLOG_IPARTPAIR)
	      << "PairdEdr " << PairdEdr.mValue
	      << "Force on target " << -aForcePartial
	      << EndLogLine;

	    BegLogLine(PKFXLOG_ENERGYTRACE)
	      << "EnergyTrace "
	      << ' ' << SourceAbsSiteId
	      << ' ' << TargetAbsSiteId
	      << ' ' << (UDF0_Bound::UDF::ReportsEnergy ? EnergySwitched0*0.5 : 0.0 )
	      << ' ' << (UDF1_Bound::UDF::ReportsEnergy ? EnergySwitched1*0.5 : 0.0 )
	      << ' ' << (UDF2_Bound::UDF::ReportsEnergy ? EnergySwitched2*0.5 : 0.0 )
	      << EndLogLine ;

            if (k_DoPressureControl)
	      {
                VirialSum += PairwiseProduct( aForcePartial, dVecAB ) ;
                ExtraScalarVirial += PairExtraScalarVirial.mValue ;
		BegLogLine(PKFXLOG_VIRIALTRACE)
		  << "VirialTrace "
		  << ' ' << SourceAbsSiteId
		  << ' ' << TargetAbsSiteId
		  << ' ' << PairwiseProduct( aForcePartial, dVecAB )*0.5
		  << ' ' << SetToScalar(PairExtraScalarVirial.mValue * (1.0/3.0) * 0.5 )
		  << EndLogLine ;

	      }
            aGlobalResultIF.AddVirial<k_DoPressureControl>( (VirialSum + SetToScalar(ExtraScalarVirial * (1.0/3.0) * 0.5 ) )
							    ) ;

            aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, EnergySwitched0*0.5 );
            aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, EnergySwitched1*0.5 );
            aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, EnergySwitched2*0.5 );
          }
      }
  } /* IFPSimple::ExecuteWithoutExclusionsCountingTarget */

  /* Display the state related to the fragment pair */
  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
  static
  void
  Display( int           aSimTick,
	   GlobalInputIF &aGlobalInputParametersIF,
	   TargetGroupIF aTargetFragmentIF,
	   SourceGroupIF aSourceFragmentIF,
	   FragmentResultIF1     &aTargetResultIF,
	   FragmentResultIF2     &aSourceResultIF,
	   GlobalResultIF       &aGlobalResultIF,
	   const int     aReportForceOnSourceFlag,
	   ExclusionMask_T  *   aExclusionMaskPtr,
	   const XYZ & FragmentImageVector )
  {
    BegLogLine(PKFXLOG_IFP_DISPLAY)
      << "IFP::Display starting, exclustion mask=" <<  aExclusionMaskPtr
      << EndLogLine ;
    int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
    int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();
    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
    int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

    for(int SourceFragmentSiteIndex = 0;
	SourceFragmentSiteIndex < SourceFragmentSiteCount;
	SourceFragmentSiteIndex++ )
      {
	BegLogLine(PKFXLOG_IFP_DISPLAY)
	  << "Source fragment " <<SourceIrreducibleFragmentId
	  << " source site " << SourceFragmentSiteIndex
	  << EndLogLine ;
	aSourceResultIF.DisplayForce(
				     MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex)
				     ) ;
  
      }
    for(int TargetFragmentSiteIndex =  0 ;
	TargetFragmentSiteIndex < TargetFragmentSiteCount;
	TargetFragmentSiteIndex++ )
      {
	BegLogLine(PKFXLOG_IFP_DISPLAY)
	  << "Target fragment " <<TargetIrreducibleFragmentId
	  << " target site " << TargetFragmentSiteIndex
	  << EndLogLine ;
	aTargetResultIF.DisplayForce(
				     MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex)
				     ) ;
      }
  }

  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
  static
  void
  Execute( int           aSimTick,
	   GlobalInputIF &aGlobalInputParametersIF,
	   TargetGroupIF aTargetFragmentIF,
	   SourceGroupIF aSourceFragmentIF,
	   FragmentResultIF1     &aTargetResultIF,
	   FragmentResultIF2     &aSourceResultIF,
	   GlobalResultIF       &aGlobalResultIF,
	   const int     aReportForceOnSourceFlag,
	   ExclusionMask_T  *   aExclusionMaskPtr,
	   const XYZ & FragmentImageVector )
#if defined(IFP_SEPARATE_EXECUTE)
    ;
#else
  {
    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair::Execute(): STARTING "
      << " aReportForceOnSourceFlag=" << aReportForceOnSourceFlag
      << " aExclusionMaskPtr=" << (void *) aExclusionMaskPtr
      << " FragmentImageVector=" << FragmentImageVector
      << EndLogLine;

#if defined(IFP_ALWAYS_REPORTS_FOS)
    assert ( aReportForceOnSourceFlag ) ;
#endif
    if ( NULL == aExclusionMaskPtr )
      {
	int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
	int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();
	int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

	if ( SourceIsTargetFlag )
	  {
            assert(0);
	  }
	else
	  {
            ExecuteWithoutExclusionsCountingBothRectangle
              <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF1,FragmentResultIF2,GlobalResultIF>
              (aSimTick
               ,aGlobalInputParametersIF
               ,aTargetFragmentIF
               ,aSourceFragmentIF
               ,aTargetResultIF
               ,aSourceResultIF
               ,aGlobalResultIF
#if defined(IFP_ALWAYS_REPORTS_FOS)
               ,1
#else
               ,aReportForceOnSourceFlag
#endif
               ,FragmentImageVector
               ) ;
	  }
      }
    else
      {
	ExecuteWithExclusions<GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF1,FragmentResultIF2,GlobalResultIF>
	  ( aSimTick,
	    aGlobalInputParametersIF,
	    aTargetFragmentIF,
	    aSourceFragmentIF,
	    aTargetResultIF,
	    aSourceResultIF,
	    aGlobalResultIF,
#if defined(IFP_ALWAYS_REPORTS_FOS)
	    1,
#else
	    aReportForceOnSourceFlag,
#endif
	    aExclusionMaskPtr,
	    FragmentImageVector);
      }
  }
#endif

  template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
  static
  void
  ExecuteInDifferentMolecules( int           aSimTick,
			       GlobalInputIF &aGlobalInputParametersIF,
			       TargetGroupIF aTargetFragmentIF,
			       SourceGroupIF aSourceFragmentIF,
			       FragmentResultIF1     &aTargetResultIF,
			       FragmentResultIF2     &aSourceResultIF,
			       GlobalResultIF       &aGlobalResultIF,
			       const int     aReportForceOnSourceFlag,
			       ExclusionMask_T  *   aExclusionMaskPtr,
			       const XYZ & FragmentImageVector )
#if defined(IFP_SEPARATE_EXECUTE)
    ;
#else
  {
    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair::ExecuteInDifferentMolecules(): STARTING "
      << " aReportForceOnSourceFlag=" << aReportForceOnSourceFlag
      << " aExclusionMaskPtr=" << (void *) aExclusionMaskPtr
      << " FragmentImageVector=" << FragmentImageVector
      << EndLogLine;

    assert (  NULL == aExclusionMaskPtr ) ;
#if defined(IFP_ALWAYS_REPORTS_FOS)
    assert ( aReportForceOnSourceFlag ) ;
#endif

    ExecuteWithoutExclusionsCountingBothRectangle
      <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF1,FragmentResultIF2,GlobalResultIF>
      (aSimTick
       ,aGlobalInputParametersIF
       ,aTargetFragmentIF
       ,aSourceFragmentIF
       ,aTargetResultIF
       ,aSourceResultIF
       ,aGlobalResultIF
#if defined(IFP_ALWAYS_REPORTS_FOS)
       ,1
#else
       ,aReportForceOnSourceFlag
#endif
       ,FragmentImageVector
       ) ;
  }
#endif

} ;/* IFPTiled */

#if defined(IFP_SEPARATE_EXECUTE) && defined(IFP_COMPILE_EXECUTE)
template <class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int CONTROL_FLAGS>
void IFPTiled<NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter,CONTROL_FLAGS>::Init()
{
  IFPHelper0::Init();
}

template <class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int CONTROL_FLAGS>
template <
  class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
void
IFPTiled<NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter, CONTROL_FLAGS>::Execute( int           aSimTick,
														     GlobalInputIF &aGlobalInputParametersIF,
														     TargetGroupIF aTargetFragmentIF,
														     SourceGroupIF aSourceFragmentIF,
														     FragmentResultIF1     &aTargetResultIF,
														     FragmentResultIF2     &aSourceResultIF,
														     GlobalResultIF       &aGlobalResultIF,
														     const int     aReportForceOnSourceFlag,
														     ExclusionMask_T  *   aExclusionMaskPtr,
														     const XYZ & FragmentImageVector )
{
  BegLogLine(PKFXLOG_IPARTPAIR)
    << "IrreducibleFragmentPair::Execute(): STARTING "
    << " aReportForceOnSourceFlag=" << aReportForceOnSourceFlag
    << " aExclusionMaskPtr=" << (void *) aExclusionMaskPtr
    << " FragmentImageVector=" << FragmentImageVector
    << EndLogLine;

#if defined(IFP_ALWAYS_REPORTS_FOS)
  assert ( aReportForceOnSourceFlag ) ;
#endif
  if ( NULL == aExclusionMaskPtr )
    {
      int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();
      int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

      if ( SourceIsTargetFlag )
	{
	  assert(0);
	}
      else
	{
	  ExecuteWithoutExclusionsCountingBothRectangle
	    <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF1,FragmentResultIF2,GlobalResultIF>
	    (aSimTick
	     ,aGlobalInputParametersIF
	     ,aTargetFragmentIF
	     ,aSourceFragmentIF
	     ,aTargetResultIF
	     ,aSourceResultIF
	     ,aGlobalResultIF
#if defined(IFP_ALWAYS_REPORTS_FOS)
	     ,1
#else
	     ,aReportForceOnSourceFlag
#endif
	     ,FragmentImageVector
	     ) ;

	}
    }
  else
    {
      ExecuteWithExclusions<GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF1,FragmentResultIF2,GlobalResultIF>
	( aSimTick,
	  aGlobalInputParametersIF,
	  aTargetFragmentIF,
	  aSourceFragmentIF,
	  aTargetResultIF,
	  aSourceResultIF,
	  aGlobalResultIF,
#if defined(IFP_ALWAYS_REPORTS_FOS)
	  1,
#else
	  aReportForceOnSourceFlag,
#endif
	  aExclusionMaskPtr,
	  FragmentImageVector);
    }
}

// This should be identical to the one above, except that we assert about aReportForceOnSourceFlag and aExclusionMaskPtr
template <class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int CONTROL_FLAGS>
template <
  class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF1, class FragmentResultIF2, class GlobalResultIF>
void
IFPTiled<NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter, CONTROL_FLAGS>::ExecuteInDifferentMolecules( int           aSimTick,
																	 GlobalInputIF &aGlobalInputParametersIF,
																	 TargetGroupIF aTargetFragmentIF,
																	 SourceGroupIF aSourceFragmentIF,
																	 FragmentResultIF1     &aTargetResultIF,
																	 FragmentResultIF2     &aSourceResultIF,
																	 GlobalResultIF       &aGlobalResultIF,
																	 const int     aReportForceOnSourceFlag,
																	 ExclusionMask_T  *   aExclusionMaskPtr,
																	 const XYZ & FragmentImageVector )
{
  BegLogLine(PKFXLOG_IPARTPAIR)
    << "IrreducibleFragmentPair::ExecuteInDifferentMolecules(): STARTING "
    << " aReportForceOnSourceFlag=" << aReportForceOnSourceFlag
    << " aExclusionMaskPtr=" << (void *) aExclusionMaskPtr
    << " FragmentImageVector=" << FragmentImageVector
    << EndLogLine;

  ExecuteWithoutExclusionsCountingBothRectangle
    <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF1,FragmentResultIF2,GlobalResultIF>
    (aSimTick
     ,aGlobalInputParametersIF
     ,aTargetFragmentIF
     ,aSourceFragmentIF
     ,aTargetResultIF
     ,aSourceResultIF
     ,aGlobalResultIF
#if defined(IFP_ALWAYS_REPORTS_FOS)
     ,1
#else
     ,aReportForceOnSourceFlag
#endif
     ,FragmentImageVector
     ) ;
}

// This should cause the compiler to instantiate exactly the kind of IFPTiled that we need.
// We're going to need 'Init', 'Execute', and 'ExecuteInDifferentMolecules' functions.
template
void
IFPTiled<
  UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
  ,UDF_SwitchFunction
  ,UDF_SwitchFunctionForce
  ,1
  >::Init() ;

template
void
IFPTiled<
  UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
  ,UDF_SwitchFunction
  ,UDF_SwitchFunctionForce
  ,1
  >::Execute
<
  NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalInputParametersIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF

  ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
	     ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
				 ,UDF_LennardJonesForce_Bound
				 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
	     ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalPartialResultsIF<0>

>
(
 int
 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalInputParametersIF&

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&

 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&

 //           GlobalResultIF       &aGlobalResultIF,
 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalPartialResultsIF<0>&

 ,    const int     aReportForceOnSourceFlag
 ,    ExclusionMask_T  *   aExclusionMaskPtr
 ,    const XYZ & FragmentImageVector ) ;
template
void
IFPTiled<
  UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
  ,UDF_SwitchFunction
  ,UDF_SwitchFunctionForce
  ,1
  >::Execute
<
  NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalInputParametersIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF    
     ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF    

				  ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
					     ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
								 ,UDF_LennardJonesForce_Bound
								 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
					     ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalPartialResultsIF<0>

>
(
 int
 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalInputParametersIF&

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&    

 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&    

 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalPartialResultsIF<0>&

 ,    const int     aReportForceOnSourceFlag
 ,    ExclusionMask_T  *   aExclusionMaskPtr
 ,    const XYZ & FragmentImageVector ) ;

template
void
IFPTiled<
  UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
  ,UDF_SwitchFunction
  ,UDF_SwitchFunctionForce
  ,1
  >::ExecuteInDifferentMolecules
<
  NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalInputParametersIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF

  ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
	     ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
				 ,UDF_LennardJonesForce_Bound
				 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
	     ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalPartialResultsIF<0>

>
(
 int
 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalInputParametersIF&

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&

 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&

 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalPartialResultsIF<0>&

 ,    const int     aReportForceOnSourceFlag                 // But we'll assert that this is 'on'
 ,    ExclusionMask_T  *   aExclusionMaskPtr                 // we'll assert that this is NULL-- never any exclusions in different molecules
 ,    const XYZ & FragmentImageVector ) ;

template
void
IFPTiled<
  UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
  ,UDF_SwitchFunction
  ,UDF_SwitchFunctionForce,
  1
  >::ExecuteInDifferentMolecules
<
  NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalInputParametersIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
	    ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::IrreducibleFragmentOperandIF

  ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF   

     ,NSQEngine<
  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
  ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
		      ,UDF_LennardJonesForce_Bound
		      ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
  ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::FragmentPartialResultsIF   

				,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
					   ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
							       ,UDF_LennardJonesForce_Bound
							       ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
					   ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
>::Client::GlobalPartialResultsIF<0>

>
(
 int
 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalInputParametersIF&

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
 ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::IrreducibleFragmentOperandIF

 ,NSQEngine<
 TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&    

 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::FragmentPartialResultsIF&    
 ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
 ,IFPTiled<UdfGroup< UDF_NSQChargeRealSpace_Bound
 ,UDF_LennardJonesForce_Bound
 ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1>
 ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
 >::Client::GlobalPartialResultsIF<0>&

 ,    const int     aReportForceOnSourceFlag                 // But we'll assert that this is 'on'
 ,    ExclusionMask_T  *   aExclusionMaskPtr                 // we'll assert that this is NULL-- never any exclusions in different molecules
 ,    const XYZ & FragmentImageVector ) ;

#endif
;
#endif
