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
 * This version is relatively simple tiling
 */

#ifndef __IFP_SIMPLETILE__HPP__
#define __IFP_SIMPLETILE__HPP__

#include <BlueMatter/IFPHelper.hpp>

template<class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int SHELL_COUNT, int CONTROL_FLAGS>
class IFPSimpleTile
{
public:
    typedef typename NsqUdfGroup::UDF0_Bound UDF0_Bound;
    typedef typename NsqUdfGroup::UDF1_Bound UDF1_Bound;
    typedef typename NsqUdfGroup::UDF2_Bound UDF2_Bound;

    typedef IFPHelper< NsqUdfGroup > IFPHelper0;

    typedef NsqUdfGroup NsqUdfGroup0;

    enum {
        NUM_SHELLS = SHELL_COUNT
        , k_TileSize = 3
        , k_TileArea = k_TileSize * k_TileSize
        , k_VectorPadding = 20
         };

    template<int ORDINAL, class UDF_Bound, class NBVM_T>
    class ExecuteWrapper
    {
    public:
        static inline void ExecuteWithExclusions( NBVM_T&         aNbvm,
                                    unsigned int&   aPairInteractionMask,
                                    int&            aSourceId,
                                    int&            aTargetId,
                                    XYZ&            aForce,
                                    double&         aNewEnergy,
                                    XYZ&            aVirial
                                    )
        {
			aNewEnergy = 0.0 ;
            if ( (aPairInteractionMask >> ORDINAL)  & 0x1 )
                {
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
        }


    static inline void Execute( NBVM_T&         aNbvm,
                                    int&            aSourceId,
                                    int&            aTargetId,
                                    XYZ&            aForce,
                                    double&         aNewEnergy,
                                    XYZ&            aVirial
                                    )
        {

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
                                    double&		aScalarVirial
                                    )
        {

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
   static inline void ExecuteSwitchOnlyUnconditional( NBVM_T&         aNbvm
                                      )
        {

  
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
        XYZ Vector[k_TileArea] ;
        double DistanceSquared[k_TileArea+k_VectorPadding] ;
        double ReciprocalDistance[k_TileArea+k_VectorPadding] ;
        AtomPair mPair[k_TileArea] ;
    } ;


    static
    inline
    void
    Init()
     {
       IFPHelper0::Init();
     }


    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    ExecuteWithExclusions( int           aSimTick,
             GlobalInputIF &aGlobalInputParametersIF,
             TargetGroupIF aTargetFragmentIF,
             SourceGroupIF aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int     aReportForceOnSourceFlag,
             const void  * aPersistentStatePtr,
             const XYZ & FragmentImageVector)
    {
      // This used to be an arguement but seems unused by caller so make local.
      // NEED: change the name so this doesn't look like an argument
      //  XYZ aInterFragmentForce;
      //      aInterFragmentForce.Zero();
      //      aVirial.Zero();
      ////////////////////      aVirialIF.Zero();
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
        << " PersistentStatePointer " << (void *) aPersistentStatePtr
        << EndLogLine;

      int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
      int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

      char *  PairInteractionMaskArray = (char *) aPersistentStatePtr;

      // Reset PairInteractionMaskIndex and then re-run those loops.
      int PairInteractionMaskIndex = 0;

      // Need to find nearest image based on COM.  Save the translation vector and apply to all sites in target
      XYZ SourceFragmentCOM = aSourceFragmentIF.GetCenterOfMassPosition();
      XYZ TargetFragmentCOM = aTargetFragmentIF.GetCenterOfMassPosition();
      XYZ TargetFragmentCOMImage;

      NearestImageInPeriodicVolume( SourceFragmentCOM,
                                    TargetFragmentCOM,
                                    TargetFragmentCOMImage);

      XYZ    SourceTargetImageCOMVector = TargetFragmentCOMImage - SourceFragmentCOM;


      // NEED: Should create an array of these suckers and fill out the distances
      //  for the whole interaction as a vector computation.

      NBVMX nbvm;
      nbvm.SetSimTick( aSimTick );
      // Make sure that the force is put on the last respa level
      nbvm.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );


      for(int SourceFragmentSiteIndex = 0;
          SourceFragmentSiteIndex < SourceFragmentSiteCount;
          SourceFragmentSiteIndex++ )
        {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        XYZ SiteA = *aSourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex );

        // nbvm.SetPositionPtr( NBVMX::SiteA,  & SiteA );

        // Scan the Target Fragment since that should be more local.
        for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1 : 0 );
            TargetFragmentSiteIndex < TargetFragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
          BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

                        // 'PairInteractionMask' knocks out those atom pairs which are handled by the bonded force evaluator
          unsigned int PairInteractionMask = CreatePairInteractionMask<UDF0_Bound::UDF::Code,UDF1_Bound::UDF::Code,UDF2_Bound::UDF::Code>
                                            (PairInteractionMaskArray,PairInteractionMaskIndex) ;
                  PairInteractionMaskIndex += 1 ;

          if( PairInteractionMask != 0 )
                  {

                  TotalPairOpsCounter.Increment();

                  // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
                  XYZ SiteImageB;
                  NearestImageInPeriodicVolume( SiteA, *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );


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

                  double DistanceSquared = SiteImageB.DistanceSquared( SiteA );

                  // These are the switch parameters for the shell at the CURRENT respa level
                  double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
                  double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

                  //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
                  double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;

                  if( DistanceSquared <  ( OffRegion * OffRegion ) )
                    {

                          XYZ Vector = SiteA - SiteImageB ;
                          double vlsq = Vector.LengthSquared() ;
                          double vrl = 1.0/sqrt(vlsq) ;
                          nbvm.Prime( Vector, vlsq, vrl );

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
                          ExecuteWrapper<0, UDF0_Bound, NBVMX>::ExecuteWithExclusions( nbvm,
                                                                         PairInteractionMask,
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
                          ExecuteWrapper<1, UDF1_Bound, NBVMX>::ExecuteWithExclusions( nbvm,
                                                                         PairInteractionMask,
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
                          ExecuteWrapper<2, UDF2_Bound, NBVMX>::ExecuteWithExclusions( nbvm,
                                                                         PairInteractionMask,
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

                          if( aReportForceOnSourceFlag || SourceIsTargetFlag )
                            {
                              aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );
                              aSourceResultIF.AddForce( SourceAbsSiteId, 0,  TotalForce );

                          aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, NewEnergy0 );
                          aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, NewEnergy1 );
                          aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, NewEnergy2 );

                           aGlobalResultIF.AddVirial<k_DoPressureControl>( 0, TotalVirial );
                        
						BegLogLine(PKFXLOG_ENERGYTRACE)
							<< "EnergyTrace "
							<< ' ' << SourceAbsSiteId
							<< ' ' << TargetAbsSiteId
							<< ' ' << NewEnergy0
							<< ' ' << NewEnergy1
							<< ' ' << NewEnergy2
							<< EndLogLine ;
							if ( k_DoPressureControl ) 
							{
							BegLogLine(PKFXLOG_VIRIALTRACE)
								<< "VirialTrace "
								<< ' ' << SourceAbsSiteId
								<< ' ' << TargetAbsSiteId
						        << ' ' << PairwiseProduct( TotalForce, Vector )
                      							* ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
								<< ' ' << TotalVirial 
							    << EndLogLine ;
							} 
				
                            }
                          else
                            {
                              // ALWAYS apply force on Target
                      aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );

                              // and only half the energy

                          aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, NewEnergy0 * 0.5);
                          aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, NewEnergy1 * 0.5);
                          aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, NewEnergy2 * 0.5);

                          aGlobalResultIF.AddVirial<k_DoPressureControl>( 0, TotalVirial * 0.5);
						BegLogLine(PKFXLOG_ENERGYTRACE)
							<< "EnergyTrace "
							<< ' ' << SourceAbsSiteId
							<< ' ' << TargetAbsSiteId
							<< ' ' << NewEnergy0
							<< ' ' << NewEnergy1
							<< ' ' << NewEnergy2
							<< EndLogLine ;
							if ( k_DoPressureControl ) 
							{
							BegLogLine(PKFXLOG_VIRIALTRACE)
								<< "VirialTrace "
								<< ' ' << SourceAbsSiteId
								<< ' ' << TargetAbsSiteId
						        << ' ' << PairwiseProduct( TotalForce, Vector )
                      							* ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
								<< ' ' << TotalVirial * 0.5
							    << EndLogLine ;
							} 
                           }

                          /////////////////////////// NEW FRAGMENT BASED CODE

                      aGlobalResultIF.AddVirial<k_DoPressureControl>( 0,
                        PairwiseProduct( TotalForce, Vector )
                      * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
                      ) ;

                    } else {
                       TotalPairOpsOutsideSiteSiteCutoff.Increment();

                    } /* endif distance cutoff */

                  } /* endif pairinteractionmask */
          } /* endfor targetfragmentsiteindex */
        } /* endfor sourcefragmentsiteindex */
    } /* IFPSimple::ExecuteWithExclusions */

    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    ExecuteWithoutExclusionsCountingBothTriangle( int           aSimTick,
             GlobalInputIF &aGlobalInputParametersIF,
             TargetGroupIF aTargetFragmentIF,
             SourceGroupIF aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int     aReportForceOnSourceFlag,
             const XYZ & FragmentImageVector)
    {
      // This used to be an arguement but seems unused by caller so make local.
      // NEED: change the name so this doesn't look like an argument
      //  XYZ aInterFragmentForce;
      //      aInterFragmentForce.Zero();
      //      aVirial.Zero();
      ////////////////////      aVirialIF.Zero();
      int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

      assert(SourceIrreducibleFragmentId == TargetIrreducibleFragmentId ) ;

      int IrreducibleFragmentId = TargetIrreducibleFragmentId ;
      // Create a flag to be used to recognize when source and target fragments are the
      // same and thus we can do 1/2 the force term computations by applying the results
      // of each computation to both sits sites.

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "IrreducibleFragmentPair::ExecuteWithoutExclusionsCountingBothTriangle(): STARTING "
        << " IrreducibleFragmentId (targ == src) " << IrreducibleFragmentId
        << EndLogLine;

      int FragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( IrreducibleFragmentId );



      // NEED: Should create an array of these suckers and fill out the distances
      //  for the whole interaction as a vector computation.

      NBVMX nbvm;
      nbvm.SetSimTick( aSimTick );
      // Make sure that the force is put on the last respa level
      nbvm.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );


      for(int SourceFragmentSiteIndex = 0;
          SourceFragmentSiteIndex < FragmentSiteCount;
          SourceFragmentSiteIndex++ )
        {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        XYZ SiteA = *aSourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex );
        XYZ SiteImageA = SiteA + FragmentImageVector ;

        // nbvm.SetPositionPtr( NBVMX::SiteA,  & SiteA );

        // Scan the Target Fragment since that should be more local.
        for(int TargetFragmentSiteIndex = SourceFragmentSiteIndex + 1 ;
            TargetFragmentSiteIndex < FragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
          BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

          TotalPairOpsCounter.Increment();

          // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
          XYZ SiteImageB;
          NearestImageInPeriodicVolume( SiteA, *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );


          BegLogLine( PKFXLOG_IPARTPAIR )
              << "IFPSimple: "
              << " SiteA "
              << SiteA
              << " *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex )= "
              << *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex )
              << " SiteImageB "
             << SiteImageB
//                << " SiteImageA=" << SiteImageA
//                << " SiteB=" << *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex )
//                << " Vector=" << Vector
              << EndLogLine;


          // The following is probably a very expensive way to find out the AbsSiteId's
          // - these could be carried in with the Src/TrgFragmentIF
          int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
          int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

          double DistanceSquared = SiteImageB.DistanceSquared( SiteA );

          // These are the switch parameters for the shell at the CURRENT respa level
          double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
          double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

          //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
          double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;

          if( DistanceSquared <  ( OffRegion * OffRegion ) )
            {

                  XYZ Vector = SiteA - SiteImageB ;
                  double vlsq = Vector.LengthSquared() ;
                  double vrl = 1.0/sqrt(vlsq) ;
                  nbvm.Prime( Vector, vlsq, vrl );

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

              aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );
              aSourceResultIF.AddForce( SourceAbsSiteId, 0,  TotalForce );

              aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, NewEnergy0 );
              aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, NewEnergy1 );
              aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, NewEnergy2 );

              aGlobalResultIF.AddVirial<k_DoPressureControl>( 0, TotalVirial );

                  /////////////////////////// NEW FRAGMENT BASED CODE

              aGlobalResultIF.AddVirial<k_DoPressureControl>( 0,
                PairwiseProduct( TotalForce, Vector )
              * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
              ) ;
              
						BegLogLine(PKFXLOG_ENERGYTRACE)
							<< "EnergyTrace "
							<< ' ' << SourceAbsSiteId
							<< ' ' << TargetAbsSiteId
							<< ' ' << NewEnergy0
							<< ' ' << NewEnergy1
							<< ' ' << NewEnergy2
							<< EndLogLine ;
						if ( k_DoPressureControl )
						{
						BegLogLine(PKFXLOG_VIRIALTRACE)
							<< "VirialTrace "
							<< ' ' << SourceAbsSiteId
							<< ' ' << TargetAbsSiteId
							<< ' ' <<  PairwiseProduct( TotalForce, Vector )
								              * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
							<< ' ' << TotalVirial 
							<< EndLogLine ;
						}

            } else {
               TotalPairOpsOutsideSiteSiteCutoff.Increment();

            } /* endif distance cutoff */

          } /* endfor targetfragmentsiteindex */
        } /* endfor sourcefragmentsiteindex */
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

    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    ExecuteWithoutExclusionsCountingBothTile( int           aSimTick,
             GlobalInputIF &aGlobalInputParametersIF,
             TargetGroupIF aTargetFragmentIF,
             SourceGroupIF aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int     aReportForceOnSourceFlag,
             const XYZ& FragmentImageVector,
             const XYZ & SourceTargetImageCOMVector,
                     int SourceFragmentTileBase,
                     int TargetFragmentTileBase,
                     int SourceFragmentSiteCount,
                     int TargetFragmentSiteCount
             )
    {
      // This used to be an arguement but seems unused by caller so make local.
      // NEED: change the name so this doesn't look like an argument
      //  XYZ aInterFragmentForce;
      //      aInterFragmentForce.Zero();
      //      aVirial.Zero();
      ////////////////////      aVirialIF.Zero();
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

      unsigned int silim = min(k_TileSize,SourceFragmentSiteCount-SourceFragmentTileBase);
      unsigned int tilim = min(k_TileSize,TargetFragmentSiteCount-TargetFragmentTileBase) ;
//          unsigned int SourceFragmentTileLimit = min(SourceFragmentTileBase+k_TileSize,SourceFragmentSiteCount) ;
//          unsigned int TargetFragmentTileLimit = min(TargetFragmentTileBase+k_TileSize,TargetFragmentSiteCount) ;

      // These are the switch parameters for the shell at the CURRENT respa level
      double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
      double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

      //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
      double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;
      double OffRegionSquared = OffRegion*OffRegion ;
      double OnRegionSquared = SwitchLowerCutoffOuterShell *SwitchLowerCutoffOuterShell ;
	  double AnyInSwitch = -1.0 ;
          // Run the tile for square-distances
          AtomPairList PairList ;
          unsigned int PairListCount = 0 ;
      XYZ ForcePartial[k_TileSize][k_TileSize] ;
      double EnergyPartial0[k_TileSize][k_TileSize] ;
      double EnergyPartial1[k_TileSize][k_TileSize] ;
      double EnergyPartial2[k_TileSize][k_TileSize] ;
      
      assert ( 3 == k_TileSize ) ;
      XYZ SA0 =  *aSourceFragmentIF.GetPositionPtr( SourceFragmentTileBase+0 ) ;
      XYZ SA1 ;
      if ( silim > 1 )
      {
      	 SA1 =  *aSourceFragmentIF.GetPositionPtr( SourceFragmentTileBase+1 ) ;
      } else {
      	SA1 = FragmentImageVector; 
      }
      XYZ SA2 ;
      if ( silim > 2 )
      {
      	 SA2 =  *aSourceFragmentIF.GetPositionPtr( SourceFragmentTileBase+2 ) ;
      } else {
      	SA2 = FragmentImageVector ;
      }
      
      
      XYZ SiteImagesA[k_TileSize] ;
      
      SiteImagesA[0] = SA0 - FragmentImageVector ; 
      SiteImagesA[1] = SA1 - FragmentImageVector ; 
      SiteImagesA[2] = SA2 - FragmentImageVector ;
      
      XYZ SB0 = *aTargetFragmentIF.GetPositionPtr( TargetFragmentTileBase+0 ) ;
      XYZ SB1 ;
      if ( tilim > 1 )
      {
      	SB1 = *aTargetFragmentIF.GetPositionPtr( TargetFragmentTileBase+1 ) ;
      } else {
      	SB1.Zero() ;
      }
      XYZ SB2 ;
      if ( tilim > 2 )
      {
      	SB2 = *aTargetFragmentIF.GetPositionPtr( TargetFragmentTileBase+2 ) ;
      } else {
      	SB2.Zero() ;
      }
      
      XYZ SitesB[k_TileSize] ;
      SitesB[0] = SB0 ;
      SitesB[1] = SB1 ;
      SitesB[2] = SB2 ;
      
      // Run the whole tile, figuring which pairs are to have forces evaluated
      for (int si2=0;si2<k_TileSize;si2+=1)
      {
      	unsigned int si_in_range = si2 - silim ;
      	for (int ti2=0;ti2<k_TileSize;ti2+=1)
      	{
      		unsigned int ti_in_range = ti2 - tilim ;
      		unsigned int in_range = ( si_in_range & ti_in_range ) >> 31 ;
      		XYZ Vector = SiteImagesA[si2] - SitesB[ti2] ; 
      		double DistanceSquared = Vector.LengthSquared() ;  
      		PairList.Vector[PairListCount] = Vector ;
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
              AnyInSwitch = fsel(DistanceSquared-OnRegionSquared,1.0,AnyInSwitch) ;
              BegLogLine( PKFXLOG_IPARTPAIR )
                << "IFPSimpleTile: "
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
                << " AnyInSwitch=" << AnyInSwitch
                << EndLogLine;

      		ForcePartial[si2][ti2].Zero();
      		EnergyPartial0[si2][ti2] = 0.0 ; 
      		EnergyPartial1[si2][ti2] = 0.0 ; 
      		EnergyPartial2[si2][ti2] = 0.0 ;
        }
      }

      // See if we have any work to do; it is possible that everything was outside range
      if ( PairListCount > 0 )
      {
                PaddableVectorReciprocalSquareRoot(PairList.ReciprocalDistance,PairList.DistanceSquared,PairListCount) ;
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
                {   // UDF 0
	                for ( unsigned int PairListIndex = 0 ; PairListIndex < PairListCount ; PairListIndex += 1)
	                {
#pragma execution_frequency(very_high)	                	    
	                        NBVMX_SCALAR nbvm ;
	                        nbvm.SetSimTick( aSimTick );
	                        nbvm.Prime(PairList.DistanceSquared[PairListIndex]
	                                                 ,PairList.ReciprocalDistance[PairListIndex]
	                        );
			                int SourceAbsSiteId =  SourceAbsSiteIds[PairListIndex];
			                int TargetAbsSiteId =  TargetAbsSiteIds[PairListIndex];
	
	                        BegLogLine(PKFXLOG_IPARTPAIR)
	                          << "WorkThread(): "
	                          << " NBVMX DistanceAB " << nbvm.GetDistance( NBVMX::SiteA, NBVMX::SiteB )
	                          << " SourceAbsSiteId " <<  SourceAbsSiteId
	                          << " TargetAbsSiteId " <<  TargetAbsSiteId
	                          << EndLogLine;
	
	                        double EnergyUnswitched = 0.0 ;
	                        double dEdrUnswitched = 0.0 ;
	                        double ExtraVirialUnswitched = 0.0 ;
	                        ExecuteWrapper<0, UDF0_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalar( nbvm,
	                                                                       SourceAbsSiteId,
	                                                                       TargetAbsSiteId,
	                                                                       EnergyUnswitched,
	                                                                       dEdrUnswitched,
	                                                                       ExtraVirialUnswitched
	                                                                     );
	                        PairdEdr0[PairListIndex].mValue = dEdrUnswitched;
	                        PairEnergy0[PairListIndex].mValue = EnergyUnswitched ;
	                        PairExtraScalarVirial0[PairListIndex].mValue = ExtraVirialUnswitched ;
	                        
	                } /* endfor PairListIndex */
                }
                { // UDF 1
	                for ( unsigned int PairListIndex = 0 ; PairListIndex < PairListCount ; PairListIndex += 1)
	                {
#pragma execution_frequency(very_high)	                	    
	                	    
	                        NBVMX_SCALAR nbvm ;
	                        nbvm.SetSimTick( aSimTick );
	                        nbvm.Prime(PairList.DistanceSquared[PairListIndex]
	                                                 ,PairList.ReciprocalDistance[PairListIndex]
	                        );
			                int SourceAbsSiteId =  SourceAbsSiteIds[PairListIndex];
			                int TargetAbsSiteId =  TargetAbsSiteIds[PairListIndex];
	
	                        BegLogLine(PKFXLOG_IPARTPAIR)
	                          << "WorkThread(): "
	                          << " NBVMX DistanceAB " << nbvm.GetDistance( NBVMX::SiteA, NBVMX::SiteB )
	                          << " SourceAbsSiteId " <<  SourceAbsSiteId
	                          << " TargetAbsSiteId " <<  TargetAbsSiteId
	                          << EndLogLine;
	
	                        double EnergyUnswitched = 0.0 ;
	                        double dEdrUnswitched = 0.0 ;
	                        double ExtraVirialUnswitched = 0.0 ;
	                        ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalar( nbvm,
	                                                                       SourceAbsSiteId,
	                                                                       TargetAbsSiteId,
	                                                                       EnergyUnswitched,
	                                                                       dEdrUnswitched,
	                                                                       ExtraVirialUnswitched
	                                                                     );
	                        PairdEdr1[PairListIndex].mValue = dEdrUnswitched;
	                        PairEnergy1[PairListIndex].mValue = EnergyUnswitched ;
	                        PairExtraScalarVirial1[PairListIndex].mValue = ExtraVirialUnswitched ;
	                } /* endfor PairListIndex */
                }
                { // UDF 2
	                for ( unsigned int PairListIndex = 0 ; PairListIndex < PairListCount ; PairListIndex += 1)
	                {
#pragma execution_frequency(very_high)	                	    
	                	    
	                        NBVMX_SCALAR nbvm ;
	                        nbvm.SetSimTick( aSimTick );
	                        nbvm.Prime(PairList.DistanceSquared[PairListIndex]
	                                                 ,PairList.ReciprocalDistance[PairListIndex]
	                        );
			                int SourceAbsSiteId =  SourceAbsSiteIds[PairListIndex];
			                int TargetAbsSiteId =  TargetAbsSiteIds[PairListIndex];
	
	                        BegLogLine(PKFXLOG_IPARTPAIR)
	                          << "WorkThread(): "
	                          << " NBVMX DistanceAB " << nbvm.GetDistance( NBVMX::SiteA, NBVMX::SiteB )
	                          << " SourceAbsSiteId " <<  SourceAbsSiteId
	                          << " TargetAbsSiteId " <<  TargetAbsSiteId
	                          << EndLogLine;
	
	                        double EnergyUnswitched = 0.0 ;
	                        double dEdrUnswitched = 0.0 ;
	                        double ExtraVirialUnswitched = 0.0 ;
	                        ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalar( nbvm,
	                                                                       SourceAbsSiteId,
	                                                                       TargetAbsSiteId,
	                                                                       EnergyUnswitched,
	                                                                       dEdrUnswitched,
	                                                                       ExtraVirialUnswitched
	                                                                     );
	                        PairdEdr2[PairListIndex].mValue = dEdrUnswitched;
	                        PairEnergy2[PairListIndex].mValue = EnergyUnswitched ;
	                        PairExtraScalarVirial2[PairListIndex].mValue = ExtraVirialUnswitched ;
	                        
	                } /* endfor PairListIndex */
                }
                
                LoopResult PairdEdr[k_TileArea];
                LoopResult PairExtraScalarVirial[k_TileArea] ;
                { // Add up the partial dE/dr values
                	for ( unsigned int PairListIndex = 0 ; PairListIndex < PairListCount ; PairListIndex += 1)
                	{
                		PairdEdr[PairListIndex].mValue = 
                		     PairdEdr0[PairListIndex].mValue
                		  + PairdEdr1[PairListIndex].mValue
                		  + PairdEdr2[PairListIndex].mValue ;
                		PairExtraScalarVirial[PairListIndex].mValue =
                		     PairExtraScalarVirial0[PairListIndex].mValue
                		  + PairExtraScalarVirial1[PairListIndex].mValue
                		  + PairExtraScalarVirial2[PairListIndex].mValue ;
                		
                	}               	
                }
                

				// If anything is in the switch zone, evaluate and apply the switches
                if (AnyInSwitch > 0.0 )
                {
                	// Something is in the switch zone; need to do the whole calculation
	                for ( unsigned int px0 = 0 ; px0 < PairListCount ; px0 += 1)
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
	                	    double EnergyUnswitched = EnergyUnswitched0 + EnergyUnswitched1 + EnergyUnswitched2 ;
	                	    
	                	    double dEdrUnswitched = PairdEdr[px0].mValue ;
	                	    double ExtraScalarVirialUnswitched = PairExtraScalarVirial[px0].mValue ;
	                	 
			                double dEdrSwitched = dEdrUnswitched * S + EnergyUnswitched * pdSdr ;
                	        double EnergySwitched0 = EnergyUnswitched0 * S ;               
                	        double EnergySwitched1 = EnergyUnswitched1 * S ;               
                	        double EnergySwitched2 = EnergyUnswitched2 * S ;             \
                	        double ExtraScalarVirialSwitched =ExtraScalarVirialUnswitched * S ;  
	                	                            
                        
	 	                	PairdEdr[px0].mValue = dEdrSwitched;
	                        PairEnergy0[px0].mValue = EnergySwitched0  ;
	                        PairEnergy1[px0].mValue = EnergySwitched1  ;
	                        PairEnergy2[px0].mValue = EnergySwitched2  ;
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
                for ( unsigned int px1=0; px1<PairListCount;px1 += 1)
                {
                        unsigned int si3 = PairList.mPair[px1].SourceIndex ;
                        unsigned int ti3 = PairList.mPair[px1].TargetIndex ;
					   XYZ dVecAB = PairList.Vector[px1];
					   double dVecABMagR = PairList.ReciprocalDistance[px1];
					   XYZ unitVector = dVecAB*dVecABMagR;
					   XYZ aForcePartial = unitVector * (-PairdEdr[px1].mValue);
					   
						BegLogLine(PKFXLOG_IPARTPAIR)
                         << "PairdEdr " << PairdEdr[px1].mValue
                         << "unitVector" << unitVector
                         << "aForcePartial " << aForcePartial
                         << EndLogLine ;
                        
						BegLogLine(PKFXLOG_ENERGYTRACE)
							<< "EnergyTrace "
							<< ' ' << SourceAbsSiteIds[px1]
							<< ' ' << TargetAbsSiteIds[px1]
							<< ' ' << PairEnergy0[px1].mValue
							<< ' ' << PairEnergy1[px1].mValue
							<< ' ' << PairEnergy2[px1].mValue
							<< EndLogLine ;
							
                        ForcePartial  [ si3 ][ ti3 ] = aForcePartial;
                        EnergyPartial0[ si3 ][ ti3 ] = PairEnergy0[px1].mValue ;
                        EnergyPartial1[ si3 ][ ti3 ] = PairEnergy1[px1].mValue ;
                        EnergyPartial2[ si3 ][ ti3 ] = PairEnergy2[px1].mValue ;
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
                aGlobalResultIF.AddVirial<k_DoPressureControl>( 0
                		, (VirialSum + SetToScalar(ExtraScalarVirial * (1.0/3.0) ) ) 
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
                        Energy0 += EnergyPartial0[a][0] ;
                        Energy1 += EnergyPartial1[a][0] ;
                        Energy2 += EnergyPartial2[a][0] ;
                        for ( int b=1;b<k_TileSize;b+=1)
                        {
                                TotalForceOnSource += ForcePartial[a][b] ;
                                TotalForceOnTarget += ForcePartial[b][a] ;
                                Energy0 += EnergyPartial0[a][b] ;
                                Energy1 += EnergyPartial1[a][b] ;
                                Energy2 += EnergyPartial2[a][b] ;
                        }
                        TFOS[a] = TotalForceOnSource ;
                        TFOT[a] = TotalForceOnTarget ;
                    }
                    aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, Energy0 );
                    aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, Energy1 );
                    aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, Energy2 );

                    
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

    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    ExecuteWithoutExclusionsCountingBothRectangle( int           aSimTick,
             GlobalInputIF &aGlobalInputParametersIF,
             TargetGroupIF aTargetFragmentIF,
             SourceGroupIF aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int     aReportForceOnSourceFlag,
             const XYZ & FragmentImageVector)
    {
      // This used to be an arguement but seems unused by caller so make local.
      // NEED: change the name so this doesn't look like an argument
      //  XYZ aInterFragmentForce;
      //      aInterFragmentForce.Zero();
      //      aVirial.Zero();
      ////////////////////      aVirialIF.Zero();
      int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

      // Create a flag to be used to recognize when source and target fragments are the
      // same and thus we can do 1/2 the force term computations by applying the results
      // of each computation to both sits sites.

      int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "IrreducibleFragmentPair::ExecuteWithoutExclusionsCountingBothRectangle(): STARTING "
        << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
        << " / " << SourceIrreducibleFragmentId
        << EndLogLine;

      int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
      int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

      // Need to find nearest image based on COM.  Save the translation vector and apply to all sites in target
      XYZ SourceFragmentCOM = aSourceFragmentIF.GetCenterOfMassPosition();
      XYZ TargetFragmentCOM = aTargetFragmentIF.GetCenterOfMassPosition();
      XYZ TargetFragmentCOMImage;

      NearestImageInPeriodicVolume( SourceFragmentCOM,
                                    TargetFragmentCOM,
                                    TargetFragmentCOMImage);

      XYZ    SourceTargetImageCOMVector = TargetFragmentCOMImage - SourceFragmentCOM;


      // NEED: Should create an array of these suckers and fill out the distances
      //  for the whole interaction as a vector computation.
          for ( int SourceFragmentTileBase = 0 ;
                                SourceFragmentTileBase < SourceFragmentSiteCount ;
                                SourceFragmentTileBase += k_TileSize )
                {
                        for ( int TargetFragmentTileBase = 0 ;
                                      TargetFragmentTileBase < TargetFragmentSiteCount ;
                                      TargetFragmentTileBase += k_TileSize )
                                {
                                ExecuteWithoutExclusionsCountingBothTile
                                 <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF,GlobalResultIF>
                                 (aSimTick
                                 ,aGlobalInputParametersIF
                                 ,aTargetFragmentIF
                                 ,aSourceFragmentIF
                                 ,aTargetResultIF
                                 ,aSourceResultIF
                                 ,aGlobalResultIF
                                 ,aReportForceOnSourceFlag
                                 ,FragmentImageVector
                                 ,SourceTargetImageCOMVector
                                 ,SourceFragmentTileBase
                                 ,TargetFragmentTileBase
                                 ,SourceFragmentSiteCount
                                 ,TargetFragmentSiteCount
                                 ) ;

                                }  /* endfor TargetFragmentSiteBase */
                } /* endfor SourceFragmentTileBase */
    } /* IFPSimple::ExecuteWithoutExclusionsCountingBothRectangle */

    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    ExecuteWithoutExclusionsCountingTarget( int           aSimTick,
             GlobalInputIF &aGlobalInputParametersIF,
             TargetGroupIF aTargetFragmentIF,
             SourceGroupIF aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int     aReportForceOnSourceFlag,
             const XYZ & FragmentImageVector)
    {
      // This used to be an arguement but seems unused by caller so make local.
      // NEED: change the name so this doesn't look like an argument
      //  XYZ aInterFragmentForce;
      //      aInterFragmentForce.Zero();
      //      aVirial.Zero();
      ////////////////////      aVirialIF.Zero();
      int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

      // Create a flag to be used to recognize when source and target fragments are the
      // same and thus we can do 1/2 the force term computations by applying the results
      // of each computation to both sits sites.

      int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "IrreducibleFragmentPair::ExecuteWithoutExclusionsCountingTarget(): STARTING "
        << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
        << " / " << SourceIrreducibleFragmentId
        << EndLogLine;

      int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
      int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );


      // Need to find nearest image based on COM.  Save the translation vector and apply to all sites in target
      XYZ SourceFragmentCOM = aSourceFragmentIF.GetCenterOfMassPosition();
      XYZ TargetFragmentCOM = aTargetFragmentIF.GetCenterOfMassPosition();
      XYZ TargetFragmentCOMImage;

      NearestImageInPeriodicVolume( SourceFragmentCOM,
                                    TargetFragmentCOM,
                                    TargetFragmentCOMImage);

      XYZ    SourceTargetImageCOMVector = TargetFragmentCOMImage - SourceFragmentCOM;


      // NEED: Should create an array of these suckers and fill out the distances
      //  for the whole interaction as a vector computation.

      NBVMX nbvm;
      nbvm.SetSimTick( aSimTick );
      // Make sure that the force is put on the last respa level
      nbvm.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );


      for(int SourceFragmentSiteIndex = 0;
          SourceFragmentSiteIndex < SourceFragmentSiteCount;
          SourceFragmentSiteIndex++ )
        {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        XYZ SiteA = *aSourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex );

        // nbvm.SetPositionPtr( NBVMX::SiteA,  & SiteA );

        // Scan the Target Fragment since that should be more local.
        for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1 : 0 );
            TargetFragmentSiteIndex < TargetFragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
          BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

          TotalPairOpsCounter.Increment();

          // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
          XYZ SiteImageB;
          NearestImageInPeriodicVolume( SiteA, *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );


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

          double DistanceSquared = SiteImageB.DistanceSquared( SiteA );

          // These are the switch parameters for the shell at the CURRENT respa level
          double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
          double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

          //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
          double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;

          if( DistanceSquared <  ( OffRegion * OffRegion ) )
            {

                  XYZ Vector = SiteA - SiteImageB ;
                  double vlsq = Vector.LengthSquared() ;
                  double vrl = 1.0/sqrt(vlsq) ;
                  nbvm.Prime( Vector, vlsq, vrl );

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

              // ALWAYS apply force on Target
              aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );

              // and only half the energy

              aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, NewEnergy0 * 0.5);
              aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, NewEnergy1 * 0.5);
              aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, NewEnergy2 * 0.5);

              aGlobalResultIF.AddVirial<k_DoPressureControl>( 0, TotalVirial * 0.5);

                  /////////////////////////// NEW FRAGMENT BASED CODE

              aGlobalResultIF.AddVirial<k_DoPressureControl>( 0,
                PairwiseProduct( TotalForce, Vector )
              * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
              ) ;

						BegLogLine(PKFXLOG_ENERGYTRACE)
							<< "EnergyTrace "
							<< ' ' << SourceAbsSiteId
							<< ' ' << TargetAbsSiteId
							<< ' ' << NewEnergy0
							<< ' ' << NewEnergy1
							<< ' ' << NewEnergy2
							<< EndLogLine ;
		      if ( k_DoPressureControl )
		      {
		      			BegLogLine(PKFXLOG_VIRIALTRACE)
								<< "VirialTrace "
								<< ' ' << SourceAbsSiteId
								<< ' ' << TargetAbsSiteId
								<< ' ' << PairwiseProduct( TotalForce, Vector )
 									             * ( aReportForceOnSourceFlag ? 1.0 : 0.5  )
								<< ' ' << TotalVirial * 0.5 
								<< EndLogLine ;
		      	
		      }
            } else {
               TotalPairOpsOutsideSiteSiteCutoff.Increment();

            } /* endif distance cutoff */

          } /* endfor targetfragmentsiteindex */
        } /* endfor sourcefragmentsiteindex */
    } /* IFPSimple::ExecuteWithoutExclusionsCountingTarget */

    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    Execute( int           aSimTick,
             GlobalInputIF &aGlobalInputParametersIF,
             TargetGroupIF aTargetFragmentIF,
             SourceGroupIF aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int     aReportForceOnSourceFlag,
             const void  * aPersistentStatePtr,
             const XYZ & FragmentImageVector)
    {
         BegLogLine(PKFXLOG_IPARTPAIR)
           << "IrreducibleFragmentPair::Execute(): STARTING "
           << " aReportForceOnSourceFlag=" << aReportForceOnSourceFlag
           << " aPersistentStatePtr=" << (unsigned int) aPersistentStatePtr
           << " FragmentImageVector=" << FragmentImageVector
           << EndLogLine;

        if (NULL == aPersistentStatePtr )
        {
            int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
            int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();
                int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

                if ( aReportForceOnSourceFlag || SourceIsTargetFlag )
                {
                        if ( SourceIsTargetFlag )
                        {
                                ExecuteWithoutExclusionsCountingBothTriangle
                                 <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF,GlobalResultIF>
                                 (aSimTick
                                 ,aGlobalInputParametersIF
                                 ,aTargetFragmentIF
                                 ,aSourceFragmentIF
                                 ,aTargetResultIF
                                 ,aSourceResultIF
                                 ,aGlobalResultIF
                                 ,aReportForceOnSourceFlag
                                 ,FragmentImageVector
                                 ) ;
                        } else {
                                ExecuteWithoutExclusionsCountingBothRectangle
                                 <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF,GlobalResultIF>
                                 (aSimTick
                                 ,aGlobalInputParametersIF
                                 ,aTargetFragmentIF
                                 ,aSourceFragmentIF
                                 ,aTargetResultIF
                                 ,aSourceResultIF
                                 ,aGlobalResultIF
                                 ,aReportForceOnSourceFlag
                                 ,FragmentImageVector
                                 ) ;

                        }
                } else {
                        ExecuteWithoutExclusionsCountingTarget
                         <GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF,GlobalResultIF>
                         (aSimTick
                         ,aGlobalInputParametersIF
                         ,aTargetFragmentIF
                         ,aSourceFragmentIF
                         ,aTargetResultIF
                         ,aSourceResultIF
                         ,aGlobalResultIF
                         ,aReportForceOnSourceFlag
                         ,FragmentImageVector
                         ) ;
                }
        } else {
                ExecuteWithExclusions<GlobalInputIF,SourceGroupIF,TargetGroupIF,FragmentResultIF,GlobalResultIF>
                 (aSimTick
                 ,aGlobalInputParametersIF
                 ,aTargetFragmentIF
                 ,aSourceFragmentIF
                 ,aTargetResultIF
                 ,aSourceResultIF
                 ,aGlobalResultIF
                 ,aReportForceOnSourceFlag
                 ,aPersistentStatePtr
                 ,FragmentImageVector
                ) ;
        }
    }

} /* IFPSimple */
;

#endif
