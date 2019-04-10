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
 *                  This version takes a Verlet list of 3:3's (e.g. water-water)
 *                  and a site-site Verlet list
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         2005/09/05 TJCW Created from IFPSimple
 *
 ***************************************************************************/

#ifndef __IFP_SIMPLE__HPP__
#define __IFP_SIMPLE__HPP__

#ifndef PKFXLOG_IFPAUDIT
#define PKFXLOG_IFPAUDIT ( 0 )
#endif

#ifndef PKFXLOG_PAIRS_IN_RANGE
#define PKFXLOG_PAIRS_IN_RANGE ( 0 )
#endif

#ifndef PKFXLOG_VECTOR_VERLET
#define PKFXLOG_VECTOR_VERLET ( 0 ) 
#endif

#include <BlueMatter/IFPHelper.hpp>


template<class NsqUdfGroup,
         class SwitchFunctionEnergyAdapter,
         class SwitchFunctionForceAdapter,
         int   SHELL_COUNT,
         int   CONTROL_FLAGS>
class IFPWaterSite
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
         };

    class AtomPairList
    {
        public:
        double DistanceSquared[k_TileArea] ;
        double ReciprocalDistance[k_TileArea] ;
    } ;
    
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


    static
    void
    Init() ;

    template<class GlobalInputIF,
             class DynVarMgrIF,
             class FragmentResultIF,
             class GlobalResultIF>
    static
    void
    ExecuteN33(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
                   unsigned int       aFragSource,
                   unsigned int       aFragTarget,
                   unsigned int       aImageVectorTableIndex
    ) 
    {
            // This used to be an arguement but seems unused by caller so make local.
      // NEED: change the name so this doesn't look like an argument
      //  XYZ aInterFragmentForce;
      //      aInterFragmentForce.Zero();
      //      aVirial.Zero();
      ////////////////////      aVirialIF.Zero();
#pragma execution_frequency( very_high )
      IrreducibleFragmentOperandIF<DynVarMgrIF>  TargetFragmentOperandIF( aSimTick, aDynVarMgrIF, aFragTarget );
      FragmentResultIF     TargetPartialResultsIF( aSimTick, aDynVarMgrIF, aFragTarget );
      IrreducibleFragmentOperandIF<DynVarMgrIF>  SourceFragmentOperandIF( aSimTick, aDynVarMgrIF, aFragSource );
      FragmentResultIF      SourcePartialResultsIF( aSimTick, aDynVarMgrIF, aFragSource );

//    XYZ FragmentImageVector = aDynVarMgrIF -> mImageVectorTable[ aImageVectorTableIndex ] ;
      XYZ FragmentImageVector = aDynVarMgrIF -> GetImageVector ( aImageVectorTableIndex ) ; 
      int TargetIrreducibleFragmentId = TargetFragmentOperandIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = SourceFragmentOperandIF.GetIrreducibleFragmentId();


      BegLogLine(PKFXLOG_IPARTPAIR)
        << "IrreducibleFragmentPair::ExecuteWithoutExclusionsCountingBothTile(): STARTING "
        << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
        << " / " << SourceIrreducibleFragmentId
        << EndLogLine;


      // These are the switch parameters for the shell at the CURRENT respa level
      double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
      double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

      //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
      double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;
      double OffRegionSquared = OffRegion*OffRegion ;
      double OnRegionSquared = SwitchLowerCutoffOuterShell *SwitchLowerCutoffOuterShell ;

                // 'DistanceSquared' are all positive.
                // For positive numbers, IEEE 'double' values collate the same way
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

      SiteImagesA[0]=SourceFragmentOperandIF.GetPosition( 0 ) - FragmentImageVector ;
      SiteImagesA[1]=SourceFragmentOperandIF.GetPosition( 1 ) - FragmentImageVector ;
      SiteImagesA[2]=SourceFragmentOperandIF.GetPosition( 2 ) - FragmentImageVector ;

      SitesB[0] = TargetFragmentOperandIF.GetPosition( 0 ) ;
      SitesB[1] = TargetFragmentOperandIF.GetPosition( 1 ) ;
      SitesB[2] = TargetFragmentOperandIF.GetPosition( 2 ) ;

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

//               BegLogLine(PKFXLOG_IPARTPAIR)
//               << "inrange_count " << inrange_count
//               << " AnyInSwitch" << (((unsigned int) AnyInSwitch) >> 31 )
//               << EndLogLine ;

// With a following wind, we will now find we have enough work to do to run the whole tile
                    // Yes, we can run the whole tile.
       double PairdEdr0[k_TileArea] ;
       double PairdEdr1[k_TileArea] ;
       double PairdEdr2[k_TileArea] ;
       double PairEnergy0[k_TileArea] ;
       double PairEnergy1[k_TileArea] ;
       double PairEnergy2[k_TileArea] ;
       double PairExtraScalarVirial0[k_TileArea] ;
       double PairExtraScalarVirial1[k_TileArea] ;
       double PairExtraScalarVirial2[k_TileArea] ;
       int SourceAbsSiteIds[k_TileArea] ;
       int TargetAbsSiteIds[k_TileArea] ;
       int SourceAbsSiteId0 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, 0);
       int SourceAbsSiteId1 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, 1);
       int SourceAbsSiteId2 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, 2);
       int TargetAbsSiteId0 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, 0);
       int TargetAbsSiteId1 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, 1);
       int TargetAbsSiteId2 =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, 2);
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
                                                                                                         PairEnergy0[PairListIndex] ,
                                                                                                         PairdEdr0[PairListIndex] ,
                                                                                                         PairExtraScalarVirial0[PairListIndex] ,
                                                                                                         aSimTick
                                                                                                         ) ;
                        }
                     if( UDF1_Bound::UDF::Code != -1 ) {
                           ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                                                                                                         SourceAbsSiteIds[PairListIndex],
                                                                                                         TargetAbsSiteIds[PairListIndex],
                                                                                                         PairList.DistanceSquared[PairListIndex],
                                                                                                         PairList.ReciprocalDistance[PairListIndex],
                                                                                                         PairEnergy1[PairListIndex] ,
                                                                                                         PairdEdr1[PairListIndex] ,
                                                                                                         PairExtraScalarVirial1[PairListIndex] ,
                                                                                                         aSimTick
                                                                                                         ) ;
                        }
                     if( UDF1_Bound::UDF::Code != -1 ) {

                           ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                                                                                                         SourceAbsSiteIds[PairListIndex],
                                                                                                         TargetAbsSiteIds[PairListIndex],
                                                                                                         PairList.DistanceSquared[PairListIndex],
                                                                                                         PairList.ReciprocalDistance[PairListIndex],
                                                                                                         PairEnergy2[PairListIndex] ,
                                                                                                         PairdEdr2[PairListIndex] ,
                                                                                                         PairExtraScalarVirial2[PairListIndex] ,
                                                                                                         aSimTick
                                                                                                         ) ;

                        }

                     } /* endfor PairListIndex */
       double PairdEdr[k_TileArea];
       double PairExtraScalarVirial[k_TileArea] ;
       { // Add up the partial dE/dr values
         for ( unsigned int PairListIndex = 0 ; PairListIndex < k_TileArea ; PairListIndex += 1)
           {
             PairdEdr[PairListIndex]  =
               ( ( UDF0_Bound::UDF::Code != -1 ) ? PairdEdr0[PairListIndex]  : 0.0 )
               +( ( UDF1_Bound::UDF::Code != -1 ) ? PairdEdr1[PairListIndex]  : 0.0 )
               +( ( UDF2_Bound::UDF::Code != -1 ) ? PairdEdr2[PairListIndex]  : 0.0 ) ;
             if ( k_DoPressureControl )
               {

                 PairExtraScalarVirial[PairListIndex]  =
                   ( ( UDF0_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial0[PairListIndex]  : 0.0 )
                   +( ( UDF1_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial1[PairListIndex]  : 0.0 )
                   + ( ( UDF2_Bound::UDF::Code != -1 ) ? PairExtraScalarVirial2[PairListIndex]  : 0.0 ) ;
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

                           double EnergyUnswitched0 = PairEnergy0[px0]  ;
                           double EnergyUnswitched1 = PairEnergy1[px0]  ;
                           double EnergyUnswitched2 = PairEnergy2[px0]  ;

                           double EnergyUnswitched =
                             ( UDF0_Bound::UDF::ReportsEnergy ? EnergyUnswitched0 : 0.0 )
                             + ( UDF1_Bound::UDF::ReportsEnergy ? EnergyUnswitched1 : 0.0 )
                             + ( UDF2_Bound::UDF::ReportsEnergy ? EnergyUnswitched2 : 0.0 ) ;

                           double dEdrUnswitched = PairdEdr[px0]  ;
                           double ExtraScalarVirialUnswitched = PairExtraScalarVirial[px0]  ;

                           double dEdrSwitched = dEdrUnswitched * S + EnergyUnswitched * pdSdr ;
                           double EnergySwitched0 = EnergyUnswitched0 * S ;
                           double EnergySwitched1 = EnergyUnswitched1 * S ;
                           double EnergySwitched2 = EnergyUnswitched2 * S ;             \
                           double ExtraScalarVirialSwitched =ExtraScalarVirialUnswitched * S ;


                           PairdEdr[px0]  = dEdrSwitched;
                           if ( UDF0_Bound::UDF::ReportsEnergy ) {
                             PairEnergy0[px0]  = EnergySwitched0  ;
                           }
                           if ( UDF1_Bound::UDF::ReportsEnergy ) {
                             PairEnergy1[px0]  = EnergySwitched1  ;
                           }
                           if ( UDF2_Bound::UDF::ReportsEnergy ) {
                             PairEnergy2[px0]  = EnergySwitched2  ;
                           }
                           PairExtraScalarVirial[px0]  = ExtraScalarVirialSwitched ;

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
#pragma unroll(k_TileSize)          
         for (unsigned int si3=0;si3<k_TileSize;si3+=1)
           {
#pragma unroll(k_TileSize)            	
             for (int ti3=0;ti3<k_TileSize;ti3+=1)
               {
                 XYZ dVecAB = SiteImagesA[si3] - SitesB[ti3] ;
                 double dVecABMagR = PairList.ReciprocalDistance[px1];
                 //                                      XYZ unitVector = dVecAB*dVecABMagR;
                 //                                                 XYZ aForcePartial = unitVector * (-PairdEdr[px1] );
                 XYZ aForcePartial = dVecAB * (-PairdEdr[px1] *dVecABMagR);

                 BegLogLine(PKFXLOG_IPARTPAIR)
                   << "PairdEdr " << PairdEdr[px1] 
                   //                         << "unitVector" << unitVector
                   //                         << "aForcePartial " << aForcePartial
                   << EndLogLine ;

                 BegLogLine(PKFXLOG_ENERGYTRACE)
                   << "EnergyTrace "
                   << ' ' << SourceAbsSiteIds[px1]
                   << ' ' << TargetAbsSiteIds[px1]
                   << ' ' << (UDF0_Bound::UDF::ReportsEnergy ? PairEnergy0[px1]  : 0.0 )
                   << ' ' << (UDF1_Bound::UDF::ReportsEnergy ?PairEnergy1[px1]  : 0.0 )
                   << ' ' << (UDF2_Bound::UDF::ReportsEnergy ?PairEnergy2[px1]  : 0.0 )
                   << EndLogLine ;

                 ForcePartial  [ si3 ][ ti3 ] = aForcePartial;
                 //                                              dEdrPartial[ si3 ][ ti3 ] = PairdEdr[px1]  ;
                 if ( UDF0_Bound::UDF::ReportsEnergy ) {
                   EnergyPartial0[ si3 ][ ti3 ] = PairEnergy0[px1]  ;
                 }
                 if ( UDF1_Bound::UDF::ReportsEnergy ) {
                   EnergyPartial1[ si3 ][ ti3 ] = PairEnergy1[px1]  ;
                 }
                 if ( UDF2_Bound::UDF::ReportsEnergy ) {
                   EnergyPartial2[ si3 ][ ti3 ] = PairEnergy2[px1]  ;
                 }
                 if (k_DoPressureControl)
                   {
                     VirialSum += PairwiseProduct( aForcePartial, dVecAB ) ;
                     ExtraScalarVirial += PairExtraScalarVirial[px1]  ;
                     BegLogLine(PKFXLOG_VIRIALTRACE)
                       << "VirialTrace "
                       << ' ' << SourceAbsSiteIds[px1]
                       << ' ' << TargetAbsSiteIds[px1]
                       << ' ' << PairwiseProduct( aForcePartial, dVecAB )
                       << ' ' << SetToScalar(PairExtraScalarVirial[px1]  * (1.0/3.0) )
                       << EndLogLine ;

                   }

                 px1 += 1 ;

               } /* endfor ti3 */
           } /* endfor si3 */
       }
       aGlobalResultIF.AddVirial<k_DoPressureControl>( 0
                                                       , (VirialSum + SetToScalar(ExtraScalarVirial * (1.0/3.0) ) )
                                                       ) ;

       double Energy0 = 0.0 ;
       double Energy1 = 0.0 ;
       double Energy2 = 0.0 ;
       XYZ TFOS[k_TileSize] ;
       XYZ TFOT[k_TileSize] ;
       // Compute the total forces on the atoms.
       // If the tile was smaller than 3x3, we will add some zeros. But it is
       // better this way, to get loops with constant iteration counts
#pragma unroll(k_TileSize)       
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
#pragma unroll(k_TileSize)           
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
       int SourceSiteId0 = MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, 0) ;
       int SourceSiteId1 = MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, 1) ;
       int SourceSiteId2 = MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, 2) ;
       int TargetSiteId0 = MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, 0) ;
       int TargetSiteId1 = MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, 1) ;
       int TargetSiteId2 = MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, 2) ;
       SourcePartialResultsIF.AddForce(SourceSiteId0,0,TFOS[0]) ;
       SourcePartialResultsIF.AddForce(SourceSiteId1,0,TFOS[1]) ;
       SourcePartialResultsIF.AddForce(SourceSiteId2,0,TFOS[2]) ;
       TargetPartialResultsIF.AddForce(TargetSiteId0,0,-TFOT[0]) ;
       TargetPartialResultsIF.AddForce(TargetSiteId1,0,-TFOT[1]) ;
       TargetPartialResultsIF.AddForce(TargetSiteId2,0,-TFOT[2]) ;

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
             
    BegLogLine(PKFXLOG_VECTOR_VERLET)
      << "Water writeback (" << SourceIrreducibleFragmentId
      << ", 0 -> " << SourceSiteId0
      << ", 1 -> " << SourceSiteId1
      << ", 2 -> " << SourceSiteId2
      << ") (" << TargetIrreducibleFragmentId
      << ", 0 -> " << TargetSiteId0
      << ", 1 -> " << TargetSiteId1
      << ", 2 -> " << TargetSiteId2
          << ")"
      << EndLogLine ;



      
    }
    
    static
    double 
    PlusReduce(
      const double * aValues,
      unsigned int aCount
    ) 
    {
      if ( aCount > 0 )
      {
        double result = aValues[0] ;
        for ( unsigned int x=1;x<aCount;x+=1)
        {
          result += aValues[x] ;
        }
        return result ;
      } else {
        return 0.0 ;
      }
    }
    
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class GlobalResultIF>
    static
    void
    ExecuteSite(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   unsigned int       aFragSource,
                   unsigned int       aOffsetSource,
                   unsigned int       aFragTarget,
                   unsigned int       aOffsetTarget,
                   unsigned int       aImageVectorTableIndex,
                   XYZ&               aForce,
                   XYZ&               aVirial,
                   double&            aEnergy0,
                   double&            aEnergy1,
                   double&            aEnergy2
    ) 
    {
        IrreducibleFragmentOperandIF<DynVarMgrIF>  TargetFragmentOperandIF( aSimTick, aDynVarMgrIF, aFragTarget );
        IrreducibleFragmentOperandIF<DynVarMgrIF>  SourceFragmentOperandIF( aSimTick, aDynVarMgrIF, aFragSource );

//    	XYZ ImageVector = aDynVarMgrIF -> mImageVectorTable[ aImageVectorTableIndex ] ;
//        XYZ ImageVector = aDynVarMgrIF -> GetImageVector ( aImageVectorTableIndex ) ; 
//        XYZ SiteA = SourceFragmentOperandIF.GetPosition( aOffsetSource );
//        XYZ SiteB = TargetFragmentOperandIF.GetPosition( aOffsetTarget );
        XYZ ImageVector ;
        XYZ SiteA ;
        XYZ SiteB ;
        
        aDynVarMgrIF -> GetImageVector ( ImageVector, aImageVectorTableIndex ) ; 
        SourceFragmentOperandIF.GetPosition( aOffsetSource, SiteA );
        TargetFragmentOperandIF.GetPosition( aOffsetTarget, SiteB ) ;
        XYZ SiteAImage = SiteA-ImageVector ;
        XYZ Vector = SiteAImage - SiteB ;
        double DistanceSquared = Vector.LengthSquared() ;
        double ReciprocalDistance = 1.0/sqrt(DistanceSquared) ;
        int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragSource, aOffsetSource );
        int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragTarget, aOffsetTarget );
        BegLogLine( PKFXLOG_IPARTPAIR )
              << "IFPWaterSite::ExecuteSite: "
              << " SiteA "
              << SiteA
              << " SiteB "
              << SiteB
              << " SiteAImage "
              << SiteAImage
              << " Vector "
              << Vector
              << EndLogLine;
       double PairdEdr0 = 0.0  ;
       double PairdEdr1 = 0.0  ;
       double PairdEdr2 = 0.0  ;
       double PairEnergy0 = 0.0  ;
       double PairEnergy1 = 0.0  ;
       double PairEnergy2 = 0.0  ;
       double PairExtraScalarVirial0 = 0.0  ;
       double PairExtraScalarVirial1 = 0.0  ;
       double PairExtraScalarVirial2 = 0.0  ;
       if( UDF0_Bound::UDF::Code != -1 ) {
           ExecuteWrapper<0, UDF0_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                     SourceAbsSiteId,
                     TargetAbsSiteId,
                     DistanceSquared,
                     ReciprocalDistance,
                     PairEnergy0,
                     PairdEdr0,
                     PairExtraScalarVirial0,
                     aSimTick
           ) ;
        }
       if( UDF1_Bound::UDF::Code != -1 ) {
           ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                     SourceAbsSiteId,
                     TargetAbsSiteId,
                     DistanceSquared,
                     ReciprocalDistance,
                     PairEnergy1,
                     PairdEdr1,
                     PairExtraScalarVirial1,
                     aSimTick
           ) ;
        }
       if( UDF1_Bound::UDF::Code != -1 ) {

           ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                     SourceAbsSiteId,
                     TargetAbsSiteId,
                     DistanceSquared,
                     ReciprocalDistance,
                     PairEnergy2, 
                     PairdEdr2,
                     PairExtraScalarVirial2,
                     aSimTick
           ) ;

        }
        
      double PairdEdr = PairdEdr0 + PairdEdr1 + PairdEdr2 ;
      double PairExtraScalarVirial = PairExtraScalarVirial0 
                                   + PairExtraScalarVirial1 
                                   + PairExtraScalarVirial2 ;
               
      NBVM_SWITCHEVALUATOR switchev ;
      switchev.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );
      switchev.Prime(DistanceSquared,ReciprocalDistance);
      ExecuteWrapper<2, UDF2_Bound, NBVM_SWITCHEVALUATOR>::ExecuteSwitchOnlyUnconditional(switchev);
      double S = switchev.GetSwitchS() ;
      double pdSdr = switchev.GetSwitchdSdR() ;
      double EnergyUnswitched = PairEnergy0 + PairEnergy1 + PairEnergy2 ;
      double dEdrSwitched = PairdEdr * S + EnergyUnswitched * pdSdr ;
      double EnergySwitched0 = PairEnergy0 * S ;
      double EnergySwitched1 = PairEnergy1 * S ;
      double EnergySwitched2 = PairEnergy2 * S ;             
      double ExtraScalarVirialSwitched =PairExtraScalarVirial * S ;
      aForce = Vector * (-dEdrSwitched*ReciprocalDistance) ;
      aVirial = PairwiseProduct( aForce, Vector ) + SetToScalar(ExtraScalarVirialSwitched * (1.0/3.0) );
      
      if (  UDF0_Bound::UDF::ReportsEnergy )
      {
        aEnergy0 = EnergySwitched0 ;
      }                             
                                 
      if (  UDF1_Bound::UDF::ReportsEnergy )
      {
        aEnergy1 = EnergySwitched1 ;
      }                             
                                 
      if (  UDF2_Bound::UDF::ReportsEnergy )
      {
        aEnergy2 = EnergySwitched2 ;
      }                             
                                 

    BegLogLine(PKFXLOG_IPARTPAIR)
        << " DistanceSquared=" << DistanceSquared
        << " ReciprocalDistance=" << ReciprocalDistance
        << " dE/dr-unswiched=" <<PairdEdr
        << " Energy-unswitched=" << EnergyUnswitched
        << " Switch=" << S
        << " dS/dr=" << pdSdr
        << " TotalForce " << aForce
        << " TotalVirial " << aVirial
        << " NewEnergy0 " << EnergySwitched0
        << " NewEnergy1 " << EnergySwitched1
        << " NewEnergy2 " << EnergySwitched2
        << EndLogLine;
  
    BegLogLine(PKFXLOG_ENERGYTRACE)
        << "EnergyTrace "
        << ' ' << SourceAbsSiteId
        << ' ' << TargetAbsSiteId
        << ' ' << EnergySwitched0
        << ' ' << EnergySwitched1
        << ' ' << EnergySwitched2
        << EndLogLine ;
      
     // Workarounds for ERRATA 
     ImageVector.Zero()  ;
     SiteA.Zero() ;
     SiteB.Zero() ;
     SiteAImage.Zero() ;
     Vector.Zero() ;
         
    }
    
    template<class DynVarMgrIF>
    static
    void
    WriteBackSite(
                   int                aSimTick,
                   DynVarMgrIF      * aDynVarMgrIF,
                   unsigned int       aFragTarget,
                   unsigned int       aOffsetTarget,
                   unsigned int       aFragSource,
                   unsigned int       aOffsetSource,
                   double             aForceX,
                   double             aForceY,
                   double             aForceZ
//                 const XYZ&         aForce
    ) 
    {
        FragmentPartialResultsIF<DynVarMgrIF>      TargetPartialResultsIF ( aSimTick, aDynVarMgrIF, aFragTarget );
        FragmentPartialResultsIF<DynVarMgrIF>      SourcePartialResultsIF ( aSimTick, aDynVarMgrIF, aFragSource );

        int TargetSiteId = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragTarget, aOffsetTarget) ;
        int SourceSiteId = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragSource, aOffsetSource) ;
  
    BegLogLine(PKFXLOG_VECTOR_VERLET)
      << "WriteBackSite (" << aFragTarget 
      << "," << aOffsetTarget
      << " -> " << TargetSiteId
      << ") (" << aFragSource
      << "," << aOffsetSource
      << " -> " << SourceSiteId
      << ")"
      << EndLogLine ;
      
    XYZ TargetForce = TargetPartialResultsIF.GetForce(TargetSiteId,0) ;
    XYZ SourceForce = SourcePartialResultsIF.GetForce(SourceSiteId,0) ;
    
    TargetForce.mX += aForceX ;
    TargetForce.mY += aForceY ; 
    TargetForce.mZ += aForceZ ; 
    
    SourceForce.mX -= aForceX ;
    SourceForce.mY -= aForceY ; 
    SourceForce.mZ -= aForceZ ;
    
    TargetPartialResultsIF.SetForce(TargetSiteId,0,TargetForce) ;
    SourcePartialResultsIF.SetForce(SourceSiteId,0,SourceForce) ;
    
//		XYZ aForce ;
//		aForce.mX = aForceX ; 
//		aForce.mY = aForceY ; 
//		aForce.mZ = aForceZ ;
//        TargetPartialResultsIF.AddForce(TargetSiteId,0,aForce) ;             
//        BegLogLine(PKFXLOG_VECTOR_VERLET)
//          << "Target writeback complete"
//          << EndLogLine ;
//        SourcePartialResultsIF.AddForce(SourceSiteId,0,-aForce) ;
//        BegLogLine(PKFXLOG_VECTOR_VERLET)
//          << "Source writeback complete"
//          << EndLogLine ;
      
    // Stop ERRATA from biting ...
    TargetForce.mX = 0.0 ;
    TargetForce.mY = 0.0 ;
    TargetForce.mZ = 0.0 ;
    SourceForce.mX = 0.0 ;
    SourceForce.mY = 0.0 ;
    SourceForce.mZ = 0.0 ;
        TargetForce.Zero() ;
        SourceForce.Zero() ;
      
      
        
    }
    
    enum {
      k_BlockSize = 256 ,
#if defined(TUNE_440D)
      k_FPUPipe = 10 ,
#else
        k_FPUPipe = 5 ,
#endif    	
      k_PaddedBlockSizeIn = k_BlockSize + 2*k_FPUPipe ,
      k_PaddedBlockSizeOut = k_BlockSize + k_FPUPipe
    } ;
    
    template<class DynVarMgrIF>
    static
    void
    VectorAndDistance(
              int                aSimTick,
              DynVarMgrIF      * aDynVarMgrIF,
              unsigned int       aFragSource,
              unsigned int       aOffsetSource,
              unsigned int       aFragTarget,
              unsigned int       aOffsetTarget,
#if defined(IFP_IMAGE_IN_VERLET)              
              unsigned int       aImageVectorTableIndex,
              float aImageTableMultiplerX,
              float aImageTableMultiplerY,
              float aImageTableMultiplerZ,
#endif              
//              double* aAddressSource,
//              double* aAddressTarget,
              
              double& VectorX,
              double& VectorY,
              double& VectorZ,
              double& SquareDistance
    ) 
    {
        
//      double ivX, ivY, ivZ ;
//        double saX, saY, saZ ;
//        double sbX, sbY, sbZ ;
        
        
        // Fetch the atom locations and the periodic image shift
        IrreducibleFragmentOperandIF<DynVarMgrIF>  TargetFragmentOperandIF( aSimTick, aDynVarMgrIF, aFragTarget );
        IrreducibleFragmentOperandIF<DynVarMgrIF>  SourceFragmentOperandIF( aSimTick, aDynVarMgrIF, aFragSource );

        double saX = SourceFragmentOperandIF.PositionX(aOffsetSource) ;
        double saY = SourceFragmentOperandIF.PositionY(aOffsetSource) ;
        double saZ = SourceFragmentOperandIF.PositionZ(aOffsetSource) ;

        double sbX = TargetFragmentOperandIF.PositionX(aOffsetTarget) ;
        double sbY = TargetFragmentOperandIF.PositionY(aOffsetTarget) ;
        double sbZ = TargetFragmentOperandIF.PositionZ(aOffsetTarget) ;
        

#if defined(IFP_IMAGE_IN_VERLET)
//      aDynVarMgrIF -> GetImageVector ( ivX, ivY, ivZ, aImageVectorTableIndex ) ; 
        double ivX = aImageTableMultiplerX * aDynVarMgrIF -> mDynamicBoxDimensionVector.mX ;
        double ivY = aImageTableMultiplerY * aDynVarMgrIF -> mDynamicBoxDimensionVector.mY ;
        double ivZ = aImageTableMultiplerZ * aDynVarMgrIF -> mDynamicBoxDimensionVector.mZ ;
        
        double ivX0 = aDynVarMgrIF -> GetImageVectorX ( aImageVectorTableIndex ) ; 
        double ivY0 = aDynVarMgrIF -> GetImageVectorY ( aImageVectorTableIndex ) ; 
        double ivZ0 = aDynVarMgrIF -> GetImageVectorZ ( aImageVectorTableIndex ) ;
         
        BegLogLine(0)
          << "VectorAndDistance " << ivX0 << " " << ivX 
          << " " << ivY0 << " " << ivY 
          << " " << ivZ0 << " " << ivZ 
          << EndLogLine ;
          
        assert(is_almost_zero(ivX-ivX0));   
        assert(is_almost_zero(ivY-ivY0));   
        assert(is_almost_zero(ivZ-ivZ0));   
          
          
        
//        double sa0X = aAddressSource[0] ;
//        double sa0Y = aAddressSource[1] ;
//        double sa0Z = aAddressSource[2] ;
//        
//        BegLogLine(1)
//          << "VectorAndDistance sa "
//          << sa0X << " " << saX << " " 
//          << sa0Y << " " << saY << " " 
//          << sa0Z << " " << saZ 
//          << EndLogLine ;
//          
//        assert(is_almost_zero(sa0X-saX)) ;
//        assert(is_almost_zero(sa0Y-saY)) ;
//        assert(is_almost_zero(sa0Z-saZ)) ;
        
        
//        double sb0X = aAddressTarget[0] ;
//        double sb0Y = aAddressTarget[1] ;
//        double sb0Z = aAddressTarget[2] ;
//        
//        BegLogLine(1)
//          << "VectorAndDistance sb "
//          << sb0X << " " << sbX << " " 
//          << sb0Y << " " << sbY << " " 
//          << sb0Z << " " << sbZ 
//          << EndLogLine ;
//        
//        
//        assert(is_almost_zero(sb0X-sbX)) ;
//        assert(is_almost_zero(sb0Y-sbY)) ;
//        assert(is_almost_zero(sb0Z-sbZ)) ;
        
        
//        SourceFragmentOperandIF.GetPosition( aOffsetSource, saX, saY, saZ );
//        TargetFragmentOperandIF.GetPosition( aOffsetTarget, sbX, sbY, sbZ ) ;
        
        // Do the imaging and work out the the vector
        double saiX = saX - ivX ; 
        double saiY = saY - ivY ; 
        double saiZ = saZ - ivZ ; 
        
        double vX = sbX - saiX ; 
        double vY = sbY - saiY ; 
        double vZ = sbZ - saiZ ;
#else

  double vX = ::NearestVectorInFullyPeriodicLine(sbX, saX, aDynVarMgrIF->mDynamicBoxDimensionVector.mX, aDynVarMgrIF->mDynamicBoxInverseDimensionVector.mX ) ;
  double vY = ::NearestVectorInFullyPeriodicLine(sbY, saY, aDynVarMgrIF->mDynamicBoxDimensionVector.mY, aDynVarMgrIF->mDynamicBoxInverseDimensionVector.mY ) ;
  double vZ = ::NearestVectorInFullyPeriodicLine(sbZ, saZ, aDynVarMgrIF->mDynamicBoxDimensionVector.mZ, aDynVarMgrIF->mDynamicBoxInverseDimensionVector.mZ ) ;

#endif         
        
        VectorX = vX ; 
        VectorY = vY ; 
        VectorZ = vZ ; 
        SquareDistance = vX*vX + vY*vY + vZ*vZ ;
        
        BegLogLine(PKFXLOG_VECTOR_VERLET)
          << "VectorAndDistance [" << vX
          << "," << vY 
          << "," << vZ 
          << "] " << SquareDistance
          << EndLogLine ;
      
    }
    
   
// This is a 'vectorised' recoprocal square root. 440d does best when 
// doing recoprocal square roots in groups of 10; the incoming data buffer and
// the results buffer are padded to facilitate it
#if 1
    static
    void
    ReciprocalDistances(
      double SquareDistance[k_PaddedBlockSizeIn],
      double ReciprocalDistance[k_PaddedBlockSizeOut],
      int Length
    ) {
      for ( unsigned int w = 0 ; w < Length ; w += 1)
    {
      ReciprocalDistance[w] = 1.0/sqrt(SquareDistance[w]) ;
      BegLogLine(PKFXLOG_VECTOR_VERLET)
        << "rsqrt(" << SquareDistance[w]
        << ") --> " << ReciprocalDistance[w]
        << EndLogLine ;
    }
      
    }
#else    
    static
    void
    ReciprocalDistances(
      double SquareDistance[k_PaddedBlockSizeIn],
      double ReciprocalDistance[k_PaddedBlockSizeOut],
      int Length
    ) {
#pragma disjoint(SquareDistance,ReciprocalDistance) 
        // Pad the incoming vector becuase we may do extra roots and loads
        for ( unsigned int px=0;px<2*k_FPUPipe;px+=1)
        {
          SquareDistance[Length+px] = 1.0 ;
        }
        double x0 = SquareDistance[0] ;
        double x1 = SquareDistance[1] ;
        double x2 = SquareDistance[2] ;
        double x3 = SquareDistance[3] ;
        double x4 = SquareDistance[4] ;
#if defined(TUNE_440D)
        double x5 = SquareDistance[5] ;
        double x6 = SquareDistance[6] ;
        double x7 = SquareDistance[7] ;
        double x8 = SquareDistance[8] ;
        double x9 = SquareDistance[9] ;
#endif  
      unsigned int Base = 0 ; 
      while ( Base < Length+k_FPUPipe )
      {
        double r0 = 1.0/sqrt(x0) ; 
        double r1 = 1.0/sqrt(x1) ; 
        double r2 = 1.0/sqrt(x2) ; 
        double r3 = 1.0/sqrt(x3) ; 
        double r4 = 1.0/sqrt(x4) ; 
#if defined(TUNE_440D)
        double r5 = 1.0/sqrt(x5) ; 
        double r6 = 1.0/sqrt(x6) ; 
        double r7 = 1.0/sqrt(x7) ; 
        double r8 = 1.0/sqrt(x8) ; 
        double r9 = 1.0/sqrt(x9) ; 
#endif
            x0 = SquareDistance[Base+k_FPUPipe+0] ;
            x1 = SquareDistance[Base+k_FPUPipe+1] ;
            x2 = SquareDistance[Base+k_FPUPipe+2] ;
            x3 = SquareDistance[Base+k_FPUPipe+3] ;
            x4 = SquareDistance[Base+k_FPUPipe+4] ;
#if defined(TUNE_440D)            
            x5 = SquareDistance[Base+k_FPUPipe+5] ;
            x6 = SquareDistance[Base+k_FPUPipe+6] ;
            x7 = SquareDistance[Base+k_FPUPipe+7] ;
            x8 = SquareDistance[Base+k_FPUPipe+8] ;
            x9 = SquareDistance[Base+k_FPUPipe+9] ;
#endif            
            ReciprocalDistance[Base+0] = r0 ; 
            ReciprocalDistance[Base+1] = r1 ; 
            ReciprocalDistance[Base+2] = r2 ; 
            ReciprocalDistance[Base+3] = r3 ; 
            ReciprocalDistance[Base+4] = r4 ; 
#if defined(TUNE_440D)            
            ReciprocalDistance[Base+5] = r5 ; 
            ReciprocalDistance[Base+6] = r6 ; 
            ReciprocalDistance[Base+7] = r7 ; 
            ReciprocalDistance[Base+8] = r8 ; 
            ReciprocalDistance[Base+9] = r9 ; 
#endif
            Base += k_FPUPipe ;            
      }
    } 
#endif
    static
    void
    ReciprocalDistancesIndexed(
      double SquareDistance[k_PaddedBlockSizeIn],
      double ReciprocalDistance[k_PaddedBlockSizeOut],
      unsigned short IndexVector[k_PaddedBlockSizeIn],
      int IndexVectorLength
    ) {
        // Pad the index vector incoming vector becuase we may do extra roots and loads
        // The consequence is that we will repeat reciprocal roots of element 0,
        // but this is not destructive.
        for ( unsigned int px=0;px<2*k_FPUPipe;px+=1)
        {
          IndexVector[IndexVectorLength+px] = 0 ;
        }
        int v0 = IndexVector[0] ;
        int v1 = IndexVector[1] ;
        int v2 = IndexVector[2] ;
        int v3 = IndexVector[3] ;
        int v4 = IndexVector[4] ;
        double x0 = SquareDistance[v0] ;
        double x1 = SquareDistance[v1] ;
        double x2 = SquareDistance[v2] ;
        double x3 = SquareDistance[v3] ;
        double x4 = SquareDistance[v4] ;
#if defined(TUNE_440D)
        int v5 = IndexVector[5] ;
        int v6 = IndexVector[6] ;
        int v7 = IndexVector[7] ;
        int v8 = IndexVector[8] ;
        int v9 = IndexVector[9] ;
        double x5 = SquareDistance[v5] ;
        double x6 = SquareDistance[v6] ;
        double x7 = SquareDistance[v7] ;
        double x8 = SquareDistance[v8] ;
        double x9 = SquareDistance[v9] ;
#endif  
      unsigned int Base = 0 ; 
      while ( Base < IndexVectorLength+k_FPUPipe )
      {
        double r0 = 1.0/sqrt(x0) ; 
        double r1 = 1.0/sqrt(x1) ; 
        double r2 = 1.0/sqrt(x2) ; 
        double r3 = 1.0/sqrt(x3) ; 
        double r4 = 1.0/sqrt(x4) ; 
#if defined(TUNE_440D)
        double r5 = 1.0/sqrt(x5) ; 
        double r6 = 1.0/sqrt(x6) ; 
        double r7 = 1.0/sqrt(x7) ; 
        double r8 = 1.0/sqrt(x8) ; 
        double r9 = 1.0/sqrt(x9) ; 
#endif
            int sv0 = v0 ; 
            int sv1 = v1 ; 
            int sv2 = v2 ; 
            int sv3 = v3 ; 
            int sv4 = v4 ; 
            v0 = IndexVector[Base+k_FPUPipe+0] ;
            v1 = IndexVector[Base+k_FPUPipe+1] ;
            v2 = IndexVector[Base+k_FPUPipe+2] ;
            v3 = IndexVector[Base+k_FPUPipe+3] ;
            v4 = IndexVector[Base+k_FPUPipe+4] ;
            x0 = SquareDistance[v0] ;
            x1 = SquareDistance[v1] ;
            x2 = SquareDistance[v2] ;
            x3 = SquareDistance[v3];
            x4 = SquareDistance[v4] ;
#if defined(TUNE_440D)            
            int sv5 = v5 ; 
            int sv6 = v6 ; 
            int sv7 = v7 ; 
            int sv8 = v8 ; 
            int sv9 = v9 ; 
            v5 = IndexVector[Base+k_FPUPipe+5] ;
            v6 = IndexVector[Base+k_FPUPipe+6] ;
            v7 = IndexVector[Base+k_FPUPipe+7] ;
            v8 = IndexVector[Base+k_FPUPipe+8] ;
            v9 = IndexVector[Base+k_FPUPipe+9] ;
            x5 = SquareDistance[v5] ;
            x6 = SquareDistance[v6] ;
            x7 = SquareDistance[v7] ;
            x8 = SquareDistance[v8] ;
            x9 = SquareDistance[v9] ;
#endif            
            ReciprocalDistance[sv0] = r0 ; 
            ReciprocalDistance[sv1] = r1 ; 
            ReciprocalDistance[sv2] = r2 ; 
            ReciprocalDistance[sv3] = r3 ; 
            ReciprocalDistance[sv4] = r4 ; 
#if defined(TUNE_440D)            
            ReciprocalDistance[sv5] = r5 ; 
            ReciprocalDistance[sv6] = r6 ; 
            ReciprocalDistance[sv7] = r7 ; 
            ReciprocalDistance[sv8] = r8 ; 
            ReciprocalDistance[sv9] = r9 ; 
#endif
            Base += k_FPUPipe ;            
      }
      
      
#if defined(PKFXLOG_VECTOR_VERLET)	    
      for ( unsigned int w0 = 0 ; w0 < IndexVectorLength ; w0 += 1)
    {
      unsigned int w = IndexVector[w0] ;
      BegLogLine(PKFXLOG_VECTOR_VERLET)
        << "rsqrt(" << SquareDistance[w]
        << ") --> " << ReciprocalDistance[w]
        << EndLogLine ;
    } 
#endif
#if !defined(NDEBUG)
    for ( unsigned int ww0=0 ; ww0 < IndexVectorLength ; ww0 += 1)
    { 
      unsigned int ww = IndexVector[ww0] ;
      assert(
        is_almost_zero(
          ReciprocalDistance[ww]
         *ReciprocalDistance[ww]
         *SquareDistance[ww]
         -1.0
        )
      ) ;
    }
#endif		
      

    }

#if defined(IFP_PARALLEL_DISTANCES)    
     template<class GlobalInputIF,
             class DynVarMgrIF,
             class GlobalResultIF>
    static
    void
    VectorsAndDistances(
              int aSimTick,
              GlobalInputIF & aGlobalInputParametersIF,
              DynVarMgrIF      * aDynVarMgrIF,
              const SiteVerletList& aSiteVerletList,
              int FirstSite,
              int Length,
              double VectorX[k_BlockSize],
              double VectorY[k_BlockSize],
              double VectorZ[k_BlockSize],
              double SquareDistance[k_PaddedBlockSizeIn],
              unsigned short IndexBelowUpper[k_PaddedBlockSizeIn],
              unsigned int& BelowThresholdCount,
              double UpperSquareDistanceThreshold
              
              
    ) 
    { 
      unsigned int rxh = 0 ; 
      
      // Hand-unroll x2, so we can put in the 'compute-parallel'
      
    
      for ( unsigned int w = 0 ; w < Length-1 ; w += 2)
    {
      unsigned int v0=FirstSite+w ;
      double SqDistance0 ;
      double vx0, vx1 ; 
      double vy0, vy1 ; 
      double vz0, vz1 ; 
      VectorAndDistance<DynVarMgrIF>(
        aSimTick,
              aDynVarMgrIF,
              aSiteVerletList.GetFragA(v0),aSiteVerletList.GetOffsetA(v0),
              aSiteVerletList.GetFragB(v0),aSiteVerletList.GetOffsetB(v0),
#if defined(IFP_IMAGE_IN_VERLET)              
              aSiteVerletList.GetImageVectorTableIndex(v0),
              aSiteVerletList.GetImageMultiplierX(v0),
              aSiteVerletList.GetImageMultiplierY(v0),
              aSiteVerletList.GetImageMultiplierZ(v0),
#endif              
              vx0,
              vy0,
              vz0,
              SqDistance0
      ) ;
            VectorX[rxh]=vx0 ,
            VectorY[rxh]=vy0 ,
            VectorZ[rxh]=vz0 ,
            SquareDistance[rxh]=SqDistance0 ;
      
      union {
        double aDouble ;
        unsigned int aUInt ;
      } UpperThreshold0, UpperThreshold1 ;
      
      IndexBelowUpper[rxh]=w ;
      
#if defined(FORCE_FULL_IFP_UPPER)
      // Do not compress the lists
      rxh += 1 ;
#else			
            double UThreshold0 = SqDistance0-UpperSquareDistanceThreshold ;
      UpperThreshold0.aDouble = UThreshold0 ;
      unsigned int uppercriterion0=UpperThreshold0.aUInt ;
      unsigned int below_upper0 = uppercriterion0 >> 31 ; 
      rxh += below_upper0 ;
#endif			
      unsigned int v1=FirstSite+w+1 ;
      double SqDistance1 ;
      VectorAndDistance<DynVarMgrIF>(
        aSimTick,
              aDynVarMgrIF,
              aSiteVerletList.GetFragA(v1),aSiteVerletList.GetOffsetA(v1),
              aSiteVerletList.GetFragB(v1),aSiteVerletList.GetOffsetB(v1),
#if defined(IFP_IMAGE_IN_VERLET)              
              aSiteVerletList.GetImageVectorTableIndex(v1),
              aSiteVerletList.GetImageMultiplierX(v1),
              aSiteVerletList.GetImageMultiplierY(v1),
              aSiteVerletList.GetImageMultiplierZ(v1),
#endif              
              vx1,
              vy1,
              vz1,
              SqDistance1
      ) ;
            VectorX[rxh]=vx1;
            VectorY[rxh]=vy1;
            VectorZ[rxh]=vz1;
            SquareDistance[rxh]=SqDistance1 ;
      
      IndexBelowUpper[rxh]=w+1 ;
      
#if defined(FORCE_FULL_IFP_UPPER)
      // Do not compress the lists
      rxh += 1 ;
#else			
            double UThreshold1 = SqDistance1-UpperSquareDistanceThreshold ;
      UpperThreshold1.aDouble = UThreshold1 ;
      unsigned int uppercriterion1=UpperThreshold1.aUInt ;
      unsigned int below_upper1 = uppercriterion1 >> 31 ; 
      rxh += below_upper1 ;
#endif			
//// Mark up for compiler SLP algorithm
//            compute_parallel(vx0, vx1) ; 
//            compute_parallel(vy0, vy1) ; 
//            compute_parallel(vz0, vz1) ; 
//            compute_parallel(SqDistance0, SqDistance1) ; 
//            compute_parallel(UThreshold0, UThreshold1) ;
    }
    if ( ( Length % 2) == 1 )
    {
      unsigned int v=FirstSite+Length-1 ;
      double SqDistance ;
      VectorAndDistance<DynVarMgrIF>(
        aSimTick,
              aDynVarMgrIF,
              aSiteVerletList.GetFragA(v),aSiteVerletList.GetOffsetA(v),
              aSiteVerletList.GetFragB(v),aSiteVerletList.GetOffsetB(v),
#if defined(IFP_IMAGE_IN_VERLET)              
              aSiteVerletList.GetImageVectorTableIndex(v),
              aSiteVerletList.GetImageMultiplierX(v),
              aSiteVerletList.GetImageMultiplierY(v),
              aSiteVerletList.GetImageMultiplierZ(v),
#endif              
//              aSiteVerletList.AddressElAPosition(v),
//              aSiteVerletList.AddressElBPosition(v),
              VectorX[rxh],
              VectorY[rxh],
              VectorZ[rxh],
              SqDistance
      ) ;
            SquareDistance[rxh]=SqDistance ;
      
      union {
        double aDouble ;
        unsigned int aUInt ;
      } UpperThreshold ;
      
      IndexBelowUpper[rxh]=Length-1 ;
      
#if defined(FORCE_FULL_IFP_UPPER)
      // Do not compress the lists
      rxh += 1 ;
#else			
            double UThreshold = SqDistance-UpperSquareDistanceThreshold ;
      UpperThreshold.aDouble = UThreshold ;
      unsigned int uppercriterion=UpperThreshold.aUInt ;
      unsigned int below_upper = uppercriterion >> 31 ; 
      rxh += below_upper ;
#endif			
    } 
      BelowThresholdCount = rxh ;
    } 
#else    
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class GlobalResultIF>
    static
    void
    VectorsAndDistances(
              int aSimTick,
              GlobalInputIF & aGlobalInputParametersIF,
              DynVarMgrIF      * aDynVarMgrIF,
              const SiteVerletList& aSiteVerletList,
              int FirstSite,
              int Length,
              double VectorX[k_BlockSize],
              double VectorY[k_BlockSize],
              double VectorZ[k_BlockSize],
              double SquareDistance[k_PaddedBlockSizeIn],
              unsigned short IndexBelowUpper[k_PaddedBlockSizeIn],
              unsigned int& BelowThresholdCount,
              double UpperSquareDistanceThreshold
//              unsigned short InSwitchRegionVector[k_BlockSize],
//              unsigned int& InSwitchCount,
//              double LowerSquareDistanceThreshold
//              ,
//              double ReciprocalDistance[k_PaddedBlockSizeOut]
              
              
    ) 
    { 
//    	double UpperThresholdVector[k_BlockSize] ;
//    	double LowerThresholdVector[k_BlockSize] ;
      unsigned int rxh = 0 ; 
//		unsigned int rxl = 0 ;
      
      
      for ( unsigned int w = 0 ; w < Length ; w += 1)
    {
      unsigned int v=FirstSite+w ;
      double SqDistance ;
      VectorAndDistance<DynVarMgrIF>(
        aSimTick,
              aDynVarMgrIF,
              aSiteVerletList.GetFragA(v),aSiteVerletList.GetOffsetA(v),
              aSiteVerletList.GetFragB(v),aSiteVerletList.GetOffsetB(v),
#if defined(IFP_IMAGE_IN_VERLET)              
              aSiteVerletList.GetImageVectorTableIndex(v),
              aSiteVerletList.GetImageMultiplierX(v),
              aSiteVerletList.GetImageMultiplierY(v),
              aSiteVerletList.GetImageMultiplierZ(v),
#endif              
//              aSiteVerletList.AddressElAPosition(v),
//              aSiteVerletList.AddressElBPosition(v),
              VectorX[rxh],
              VectorY[rxh],
              VectorZ[rxh],
              SqDistance
      ) ;
//			SquareDistance[w]=SqDistance ;
//			ReciprocalDistance[w]=1.0/sqrt(SqDistance) ;
            SquareDistance[rxh]=SqDistance ;
      
      union {
        double aDouble ;
        unsigned int aUInt ;
      } UpperThreshold,LowerThreshold ;
      
      IndexBelowUpper[rxh]=w ;
//			InSwitchRegionVector[rxl]=w ;
      
#if defined(FORCE_FULL_IFP_UPPER)
      // Do not compress the lists
      rxh += 1 ;
//	 		rxl += 1 ;
#else			
            double UThreshold = SqDistance-UpperSquareDistanceThreshold ;
      UpperThreshold.aDouble = UThreshold ;
//			LowerThreshold.aDouble = LowerSquareDistanceThreshold-SqDistance ;
      unsigned int uppercriterion=UpperThreshold.aUInt ;
//			unsigned int lowercriterion=LowerThreshold.aUInt ;
      unsigned int below_upper = uppercriterion >> 31 ; 
//			unsigned int above_lower = lowercriterion >> 31 ;
      rxh += below_upper ;
//			rxl += (above_lower & below_upper) ;
#endif			
//	    	BegLogLine(PKFXLOG_VECTOR_VERLET)
//	     	  << "SelectBelowThreshold " << w
//	    	  << " " << SquareDistance[w]
//	    	  << " " << UpperSquareDistanceThreshold
//	    	  << " " << below_upper
//	    	  << " " << rxh
//	    	  << EndLogLine ;
//	    	BegLogLine(PKFXLOG_VECTOR_VERLET)
//	     	  << "SelectAboveThreshold " << w
//	    	  << " " << SquareDistance[w]
//	    	  << " " << LowerSquareDistanceThreshold
//	    	  << " " << (above_lower & below_upper)
//	    	  << " " << rxl
//	    	  << EndLogLine ;
//			
//			UpperThresholdVector[w]= SqDistance-UpperSquareDistanceThreshold ;
//			LowerThresholdVector[w]= LowerSquareDistanceThreshold-SqDistance ;
    }
//    	for ( unsigned int ww=0;ww<Length;ww+=1)
//    	{
//    		IndexBelowUpper[rxh]=ww;
//    		unsigned int uppercriterion=*(int *)(&(UpperThresholdVector[ww])) ;
//    		// 'criterion' will have its top bit set if the 'double' was negative
//    		unsigned int below_upper = uppercriterion >> 31 ; 
//    		rxh += below_upper ;
//	    	BegLogLine(PKFXLOG_VECTOR_VERLET)
//	     	  << "SelectBelowThreshold " << ww
//	    	  << " " << SquareDistance[ww]
//	    	  << " " << UpperSquareDistanceThreshold
//	    	  << " " << below_upper
//	    	  << " " << rxh
//	    	  << EndLogLine ;
//	    	InSwitchRegionVector[rxl]=ww;
//    		unsigned int lowercriterion=*(int *)(&(LowerThresholdVector[ww])) ;
//    		// 'criterion' will have its top bit set if the 'double' was negative
//    		unsigned int above_lower = lowercriterion >> 31 ; 
//    		rxl += (above_lower & below_upper) ;
//	    	BegLogLine(PKFXLOG_VECTOR_VERLET)
//	     	  << "SelectAboveThreshold " << ww
//	    	  << " " << SquareDistance[ww]
//	    	  << " " << LowerSquareDistanceThreshold
//	    	  << " " << (above_lower & below_upper)
//	    	  << " " << rxl
//	    	  << EndLogLine ;
//	    	  
//    	}
      BelowThresholdCount = rxh ;
//    	InSwitchCount = rxl ;
    } 
#endif    
    
    static
    void
    EnergyAndSlope_Unswitched(
              int                aSimTick,
              unsigned int       aFragSource,
              unsigned int       aOffsetSource,
              unsigned int       aFragTarget,
              unsigned int       aOffsetTarget,
              double DistanceSquared,
              double& aReciprocalDistance,
              
              int&    aSourceAbsSiteId,
              int&    aTargetAbsSiteId,
              double& adEdr,
              double& aEnergy0,
              double& aEnergy1,
              double& aEnergy2,
              double& aExtraScalarVirial 
              )
    {
       int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragSource, aOffsetSource );
       int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragTarget, aOffsetTarget );
       double PairdEdr0 = 0.0  ;
       double PairdEdr1 = 0.0  ;
       double PairdEdr2 = 0.0  ;
       double PairEnergy0 = 0.0  ;
       double PairEnergy1 = 0.0  ;
       double PairEnergy2 = 0.0  ;
       double PairExtraScalarVirial0 = 0.0  ;
       double PairExtraScalarVirial1 = 0.0  ;
       double PairExtraScalarVirial2 = 0.0  ;
       
       double ReciprocalDistance=1.0/sqrt(DistanceSquared) ; 
       aReciprocalDistance=ReciprocalDistance ; 
       // Evaluate the forces
       if( UDF0_Bound::UDF::Code != -1 ) {
           ExecuteWrapper<0, UDF0_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                     SourceAbsSiteId,
                     TargetAbsSiteId,
                     DistanceSquared,
                     ReciprocalDistance,
                     PairEnergy0,
                     PairdEdr0,
                     PairExtraScalarVirial0,
                     aSimTick
           ) ;
        }
       if( UDF1_Bound::UDF::Code != -1 ) {
           ExecuteWrapper<1, UDF1_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                     SourceAbsSiteId,
                     TargetAbsSiteId,
                     DistanceSquared,
                     ReciprocalDistance,
                     PairEnergy1,
                     PairdEdr1,
                     PairExtraScalarVirial1,
                     aSimTick
           ) ;
        }
       if( UDF1_Bound::UDF::Code != -1 ) {

           ExecuteWrapper<2, UDF2_Bound, NBVMX_SCALAR>::ExecuteWithoutSwitchScalarDetail(
                     SourceAbsSiteId,
                     TargetAbsSiteId,
                     DistanceSquared,
                     ReciprocalDistance,
                     PairEnergy2, 
                     PairdEdr2,
                     PairExtraScalarVirial2,
                     aSimTick
           ) ;

        }
        
      double PairdEdr = PairdEdr0 + PairdEdr1 + PairdEdr2 ;
      double PairExtraScalarVirial = PairExtraScalarVirial0 
                                   + PairExtraScalarVirial1 
                                   + PairExtraScalarVirial2 ;
       
      aSourceAbsSiteId = SourceAbsSiteId ;
      aTargetAbsSiteId = TargetAbsSiteId ;      
      adEdr = PairdEdr ;
      aEnergy0 = PairEnergy0 ; 
      aEnergy1 = PairEnergy1 ; 
      aEnergy2 = PairEnergy2 ; 
      aExtraScalarVirial = PairExtraScalarVirial ;

    }              
    
    static void
    Switch(
      double DistanceSquared,
      double ReciprocalDistance,
      
      double& aSwitch,
      double& adSdr
    ) {
            // Do the 'switch'        
      NBVM_SWITCHEVALUATOR switchev ;
      switchev.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );
      switchev.Prime(DistanceSquared,ReciprocalDistance);
      ExecuteWrapper<2, UDF2_Bound, NBVM_SWITCHEVALUATOR>::ExecuteSwitchOnlyUnconditional(switchev);
      double S = switchev.GetSwitchS() ;
      double pdSdr = switchev.GetSwitchdSdR() ;
      
      aSwitch=S ;
      adSdr = pdSdr ;
    } 
    
    static void
    Switch(
      double Distance,
      double& aSwitch,
      double& adSdr
    ) {
            // Do the 'switch'        
      NBVM_SWITCHEVALUATOR switchev ;
      switchev.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );
      switchev.PrimeLength(Distance);
      ExecuteWrapper<2, UDF2_Bound, NBVM_SWITCHEVALUATOR>::ExecuteSwitchOnlyUnconditional(switchev);
      double S = switchev.GetSwitchS() ;
      double pdSdr = switchev.GetSwitchdSdR() ;
      
      aSwitch=S ;
      adSdr = pdSdr ;
    } 
    
//    static void SelectBelowThreshold(
//      double SquareDistance[k_PaddedBlockSizeIn],
//      double UpperSquareDistanceThreshold,
//      unsigned int Length,
//      unsigned short BelowThresholdIndex[k_PaddedBlockSizeIn],
//      unsigned int& BelowThresholdCount
//    ) {
//    	double UpperThresholdVector[k_BlockSize] ;
//    	for ( unsigned int w=0; w<Length;w+=1)
//    	{
//    		UpperThresholdVector[w]= SquareDistance[w]-UpperSquareDistanceThreshold ;
//        }
//    	unsigned int rxh = 0 ; 
//    	for ( unsigned int v=0;v<Length;v+=1)
//    	{
//    		BelowThresholdIndex[rxh]=v;
//    		unsigned int uppercriterion=*(int *)(&(UpperThresholdVector[v])) ;
//    		// 'criterion' will have its top bit set if the 'double' was negative
//    		unsigned int below_upper = uppercriterion >> 31 ; 
//    		rxh += below_upper ;
//	    	BegLogLine(PKFXLOG_VECTOR_VERLET)
//	     	  << "SelectBelowThreshold " << v
//	    	  << " " << SquareDistance[v]
//	    	  << " " << UpperSquareDistanceThreshold
//	    	  << " " << below_upper
//	    	  << " " << rxh
//	    	  << EndLogLine ;
//    	}
//    	BelowThresholdCount = rxh ;
//    }
    
    static void SelectAboveThreshold(
      double SquareDistance[k_PaddedBlockSizeIn],
      double LowerSquareDistanceThreshold,
      unsigned int IndexInLength,
      unsigned short IndexIn[k_PaddedBlockSizeIn],
      unsigned short AboveThresholdIndexOut[k_PaddedBlockSizeIn],
      unsigned int& AboveThresholdCount
    ) {
      double LowerThresholdVector[k_BlockSize] ;
      for ( unsigned int w0=0; w0<IndexInLength;w0+=1)
      {
        unsigned int w = IndexIn[w0] ;
        LowerThresholdVector[w0]= LowerSquareDistanceThreshold-SquareDistance[w] ;
        }
      unsigned int rxl = 0 ; 
      for ( unsigned int v0=0;v0<IndexInLength;v0+=1)
      {
        unsigned int v=IndexIn[v0] ;
        AboveThresholdIndexOut[rxl]=v;
        unsigned int lowercriterion=*(int *)(&(LowerThresholdVector[v0])) ;
        // 'criterion' will have its top bit set if the 'double' was negative
        unsigned int above_lower = lowercriterion >> 31 ; 
        rxl += above_lower ;
        BegLogLine(PKFXLOG_VECTOR_VERLET)
          << "SelectAboveThreshold " << v
          << " " << SquareDistance[v]
          << " " << LowerSquareDistanceThreshold
          << " " << above_lower
          << " " << rxl
          << EndLogLine ;
      }
      AboveThresholdCount = rxl ;
    }
    
// This picks out from the all-atom-pair list
// (1) Those site pairs which are in the switch region
// (2) Those site pairs which are outside the upper cutoff
// We could do the 'outside upper cutoff' earlier, and skip the LJ/Charge computation
// for them. Job for another day.    
    static void
    AboveThreshold(
      double SquareDistance[k_PaddedBlockSizeIn],
      double LowerSquareDistanceThreshold,
      double UpperSquareDistanceThreshold,
      int Length,
      int AboveLowerThresholdVector[k_BlockSize],
      int AboveUpperThresholdVector[k_BlockSize],
      int& AboveLowerCount,
      int& AboveUpperCount
    ) { 
      double LowerThresholdVector[k_BlockSize] ;
      double UpperThresholdVector[k_BlockSize] ;
      for ( unsigned int w=0; w<Length;w+=1)
      {
        LowerThresholdVector[w]= LowerSquareDistanceThreshold-SquareDistance[w] ;
        UpperThresholdVector[w]= UpperSquareDistanceThreshold-SquareDistance[w] ;
        }
      unsigned int rxl = 0 ;
      unsigned int rxh = 0 ; 
      for ( unsigned int v=0;v<Length;v+=1)
      {
        AboveLowerThresholdVector[rxl]=v;
        AboveUpperThresholdVector[rxh]=v;
        unsigned int lowercriterion=*(int *)(&(LowerThresholdVector[v])) ;
        unsigned int uppercriterion=*(int *)(&(UpperThresholdVector[v])) ;
        // 'criterion' will have its top bit set if the 'double' was negative
        unsigned int above_lower = lowercriterion >> 31 ; 
        unsigned int above_upper = uppercriterion >> 31 ; 
        rxh += above_upper ;
        rxl += (above_lower-above_upper) ;
        BegLogLine(PKFXLOG_VECTOR_VERLET)
          << "AboveThreshold " << v
          << " " << SquareDistance[v]
          << " " << LowerSquareDistanceThreshold
          << " " << UpperSquareDistanceThreshold
          << " " << above_lower
          << " " << above_upper
          << " " << rxl
          << " " << rxh
          << EndLogLine ;
      }
      AboveLowerCount = rxl ;
      AboveUpperCount = rxh ;
    } 
    
// This modifies the 'unswitched' forces and energies
    static void
    SwitchBlock(
      double SquareDistance[k_PaddedBlockSizeIn],
      double ReciprocalDistance[k_PaddedBlockSizeOut],
      double adEdr[k_BlockSize],
      double aEnergy0[k_BlockSize],
      double aEnergy1[k_BlockSize],
      double aEnergy2[k_BlockSize],
      double aExtraScalarVirial[k_BlockSize], 
      int AboveLowerThresholdVector[k_BlockSize],
      int AboveUpperThresholdVector[k_BlockSize],
      int AboveLowerCount,
      int AboveUpperCount
    
    ) {
      double dEdr[k_BlockSize] ;
      double E0_Switched[k_BlockSize] ;
      double E1_Switched[k_BlockSize] ;
      double E2_Switched[k_BlockSize] ;
      double ExtraScalarVirial_Switched[k_BlockSize] ;
      
      for ( unsigned int v=0;v<AboveLowerCount;v+=1)
      {
        double S ;
        double dSdr ;
        int w=AboveLowerThresholdVector[v] ;
        Switch(
         SquareDistance[w],
         ReciprocalDistance[w],
         S,
         dSdr ) ;
         double E0 = aEnergy0[w] ;
         double E1 = aEnergy1[w] ;
         double E2 = aEnergy2[w] ;
         double ESV = aExtraScalarVirial[w] ;
         E0_Switched[v] = E0 * S ;
         E1_Switched[v] = E1 * S ;
         E2_Switched[v] = E2 * S ;
         ExtraScalarVirial_Switched[v] = ESV * S ;
         dEdr[v] = adEdr[w] * S + (E0+E1+E2) * dSdr ;
      }
      for ( unsigned int vv=0;vv<AboveLowerCount;vv+=1)
      {
        int ww=AboveLowerThresholdVector[vv] ;
        aEnergy0[ww]=E0_Switched[vv] ;
        aEnergy1[ww]=E1_Switched[vv] ;
        aEnergy2[ww]=E2_Switched[vv] ;
        aExtraScalarVirial[ww]=ExtraScalarVirial_Switched[vv] ;
        adEdr[ww]=dEdr[vv] ;
      }
      for ( unsigned int vvv=0;vvv<AboveUpperCount ; vvv += 1)
      {
        int www=AboveUpperThresholdVector[vvv] ;
        aEnergy0[www]=0.0 ;
        aEnergy1[www]=0.0 ;
        aEnergy2[www]=0.0 ;
        aExtraScalarVirial[www]=0.0 ;
        adEdr[www]=0.0 ;
        
      }
    }
// This modifies the 'unswitched' forces and energies
    static void
    SwitchBlockIndexed(
      double SquareDistance[k_PaddedBlockSizeIn],
      double ReciprocalDistance[k_PaddedBlockSizeOut],
      double adEdr[k_BlockSize],
      double aEnergy0[k_BlockSize],
      double aEnergy1[k_BlockSize],
      double aEnergy2[k_BlockSize],
      double aExtraScalarVirial[k_BlockSize], 
      unsigned short AboveLowerThresholdVector[k_BlockSize],
      int AboveLowerCount
    
    ) {
      double dEdr[k_BlockSize] ;
      double E0_Switched[k_BlockSize] ;
      double E1_Switched[k_BlockSize] ;
      double E2_Switched[k_BlockSize] ;
      double ExtraScalarVirial_Switched[k_BlockSize] ;
      
      for ( unsigned int v=0;v<AboveLowerCount;v+=1)
      {
        double S ;
        double dSdr ;
        int w=AboveLowerThresholdVector[v] ;
        Switch(
         SquareDistance[w],
         ReciprocalDistance[w],
         S,
         dSdr ) ;
         double E0 = aEnergy0[w] ;
         double E1 = aEnergy1[w] ;
         double E2 = aEnergy2[w] ;
         double ESV = aExtraScalarVirial[w] ;
         E0_Switched[v] = E0 * S ;
         E1_Switched[v] = E1 * S ;
         E2_Switched[v] = E2 * S ;
         ExtraScalarVirial_Switched[v] = ESV * S ;
         dEdr[v] = adEdr[w] * S + (E0+E1+E2) * dSdr ;
      }
      for ( unsigned int vv=0;vv<AboveLowerCount;vv+=1)
      {
        int ww=AboveLowerThresholdVector[vv] ;
        aEnergy0[ww]=E0_Switched[vv] ;
        aEnergy1[ww]=E1_Switched[vv] ;
        aEnergy2[ww]=E2_Switched[vv] ;
        aExtraScalarVirial[ww]=ExtraScalarVirial_Switched[vv] ;
        adEdr[ww]=dEdr[vv] ;
      }
    }
    
    static
    void
    SwitchBlockPacked(
    double aAboveLowerThresholdDistances[k_BlockSize],
      double adEdr[k_BlockSize],
      double aEnergy0[k_BlockSize],
      double aEnergy1[k_BlockSize],
      double aEnergy2[k_BlockSize],
      double aExtraScalarVirial[k_BlockSize], 
      unsigned short aAboveLowerThresholdVector[k_BlockSize],
      unsigned int aAboveLowerCount
     ) { 
        for ( unsigned int v=0;v<aAboveLowerCount;v+=1)
      {
        double S ;
        double dSdr ;
        int w=aAboveLowerThresholdVector[v] ;
        Switch(
         aAboveLowerThresholdDistances[v],
         S,
         dSdr ) ;
         double E0 = aEnergy0[w] ;
         double E1 = aEnergy1[w] ;
         double E2 = aEnergy2[w] ;
         double ESV = aExtraScalarVirial[w] ;
         aEnergy0[w] = E0 * S ;
         aEnergy1[w] = E1 * S ;
         aEnergy2[w] = E2 * S ;
         aExtraScalarVirial[w] = ESV * S ;
         adEdr[w] = adEdr[w] * S + (E0+E1+E2) * dSdr ;
      }
      
      
     }
    
    
    static
    void
    EnergyAndSlope(
              int                aSimTick,
              unsigned int       aFragSource,
              unsigned int       aOffsetSource,
              unsigned int       aFragTarget,
              unsigned int       aOffsetTarget,
              double DistanceSquared,
              double ReciprocalDistance,
              
              int&    aSourceAbsSiteId,
              int&    aTargetAbsSiteId,
              double& adEdr,
              double& aEnergy0,
              double& aEnergy1,
              double& aEnergy2,
              double& aExtraScalarVirial 
              
    ) 
    { 
      double dEdr_Unswitched ;
      double Energy0_Unswitched ;
      double Energy1_Unswitched ;
      double Energy2_Unswitched ;
      double ExtraScalarVirial_Unswitched ;
      double S ; 
      double dSdr ;
      EnergyAndSlopeUnswitched(
              aSimTick,
              aFragSource,
              aOffsetSource,
              aFragTarget,
              aOffsetTarget,
              DistanceSquared,
              ReciprocalDistance,
              aSourceAbsSiteId,
              aTargetAbsSiteId,
              dEdr_Unswitched,
              Energy0_Unswitched,
              Energy1_Unswitched,
              Energy2_Unswitched,
              ExtraScalarVirial_Unswitched
      ) ;
      Switch (
       DistanceSquared,
       ReciprocalDistance,
       S,
       dSdr
      ) ;
      

      double EnergyUnswitched = Energy0_Unswitched 
                              + Energy1_Unswitched
                              + Energy2_Unswitched ;    	
      double dEdrSwitched = PairdEdr * S + EnergyUnswitched * dSdr ;
      double EnergySwitched0 = PairEnergy0 * S ;
      double EnergySwitched1 = PairEnergy1 * S ;
      double EnergySwitched2 = PairEnergy2 * S ;             
      double ExtraScalarVirialSwitched =ExtraScalarVirial_Unswitched * S ;
      
      
      // Write back the results for the caller
      adEdr = dEdrSwitched ;
      aEnergy0 = EnergySwitched0 ; 
      aEnergy1 = EnergySwitched1 ; 
      aEnergy2 = EnergySwitched2 ; 
      aExtraScalarVirial = ExtraScalarVirialSwitched ;
      
      BegLogLine(PKFXLOG_VECTOR_VERLET)
        << "EnergyAndSlope " 
        << DistanceSquared
        << " " << dEdrSwitched
        << EndLogLine ; 
      
    }
    
    static
    void
    EnergiesAndSlopes(
       int aSimTick,
       const SiteVerletList& aSiteVerletList,
       int FirstSite,
       int Length,
       double SwitchLowerCutoffOuterShell,
       double SwitchDeltaOuterShell,
       double SquareDistance[k_PaddedBlockSizeIn],
       double ReciprocalDistance[k_PaddedBlockSizeOut],
       
       int SourceSiteID[k_BlockSize],
     int TargetSiteID[k_BlockSize],
        
       double dEdr[k_BlockSize] ,
       double& aEnergy0 ,
       double& aEnergy1 ,
       double& aEnergy2 ,
       double& aExtraScalarVirial
    ) 
    { 
      double TotalEnergy0 = 0.0 ;
      double TotalEnergy1 = 0.0 ; 
      double TotalEnergy2 = 0.0 ;
      double TotalExtraScalarVirial = 0.0 ;
    double Energy0[k_BlockSize] ;
    double Energy1[k_BlockSize] ;
    double Energy2[k_BlockSize] ; 
    double ExtraScalarVirial[k_BlockSize] ;
    // Do the unswitched evaluation
      for ( unsigned int w = 0 ; w < Length ; w += 1)
    {
      unsigned int v=FirstSite+w ;
      EnergyAndSlope_Unswitched(
        aSimTick,
              aSiteVerletList.GetFragA(v),aSiteVerletList.GetOffsetA(v),
              aSiteVerletList.GetFragB(v),aSiteVerletList.GetOffsetB(v),
              SquareDistance[w],
              ReciprocalDistance[w],
              SourceSiteID[w],
              TargetSiteID[w],
              dEdr[w],
              Energy0[w],
              Energy1[w],
            Energy2[w],
            ExtraScalarVirial[w] 
      ) ;
    }
      int AboveLowerThresholdVector[k_BlockSize] ;
      int AboveUpperThresholdVector[k_BlockSize] ;
      int AboveLowerCount ;
      int AboveUpperCount ;
      // These are the switch parameters for the shell at the CURRENT respa level
//      double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
//      double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

      //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
      double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;
      double OffRegionSquared = OffRegion*OffRegion ;
      double OnRegionSquared = SwitchLowerCutoffOuterShell *SwitchLowerCutoffOuterShell ;
    
    
    // Work out what is in the switch region (and what is above upper cutoff)
    AboveThreshold(
      SquareDistance,
      OnRegionSquared,
      OffRegionSquared,
      Length,
      AboveLowerThresholdVector,
      AboveUpperThresholdVector,
      AboveLowerCount,
      AboveUpperCount
    ) ;
    
    // Scale back the forces and energies in the switch (and zero them above)
  SwitchBlock(
      SquareDistance,
      ReciprocalDistance,
      dEdr,
      Energy0,
      Energy1,
      Energy2,
      ExtraScalarVirial, 
      AboveLowerThresholdVector,
      AboveUpperThresholdVector,
      AboveLowerCount,
      AboveUpperCount
     ) ;
    
    
    // Work out total energies and virial
      for ( unsigned int ww = 0 ; ww < Length ; ww += 1)
    {
      TotalEnergy0 += Energy0[ww] ;
      TotalEnergy1 += Energy1[ww] ;
      TotalEnergy2 += Energy2[ww] ;
      TotalExtraScalarVirial += ExtraScalarVirial[ww] ;
    }
    
    
    // Write back for the caller
    aEnergy0 = TotalEnergy0 ;
    aEnergy1 = TotalEnergy1 ;
    aEnergy2 = TotalEnergy2 ;
    aExtraScalarVirial = TotalExtraScalarVirial ;
    
#if PKFXLOG_VECTOR_VERLET
        for ( unsigned int z=0;z<Length;z+=1)
        {
           BegLogLine(PKFXLOG_VECTOR_VERLET)
          << "EnergyAndSlope " 
          << SquareDistance[z]
          << " " << dEdrS[z]
          << EndLogLine ; 
          
        }
#endif		
    }
    
    static
    void
       ConstructInSwitchIndex(
       double SquareDistance[k_PaddedBlockSizeIn],
       unsigned int Length, 
       double LowerSquareDistanceThreshold,
       double ReciprocalDistance[k_PaddedBlockSizeOut],
       unsigned short InSwitchVector[k_BlockSize],
       unsigned int& InSwitchCount,
       double AboveLowerThresholdDistances[k_BlockSize]
    ) {
      unsigned int rxl = 0 ;
      for ( unsigned int w=0;w<Length;w+=1)
      {
        union {
        double aDouble ;
        unsigned int aUInt ;
      } LowerThreshold ;
      
      
      InSwitchVector[rxl]=w ;
      AboveLowerThresholdDistances[rxl]=SquareDistance[w]*ReciprocalDistance[w] ;
      
#if defined(FORCE_FULL_IFP)
      // Do not compress the lists
      rxl += 1 ;
#else			
      LowerThreshold.aDouble = LowerSquareDistanceThreshold-SquareDistance[w] ;
      unsigned int lowercriterion=LowerThreshold.aUInt ;
      unsigned int above_lower = lowercriterion >> 31 ;
      rxl += above_lower;
#endif
        
      }
      InSwitchCount = rxl ;
    }
    
    static
    void
    EnergiesAndSlopesIndexed(
       int aSimTick,
       const SiteVerletList& aSiteVerletList,
       int FirstSite,
       unsigned short IndexVector[k_PaddedBlockSizeIn],
       int IndexVectorLength,
       double SwitchLowerCutoffOuterShell,
       double SwitchDeltaOuterShell,
       double SquareDistance[k_PaddedBlockSizeIn],
       double ReciprocalDistance[k_PaddedBlockSizeOut],
       
       int SourceSiteID[k_BlockSize],
     int TargetSiteID[k_BlockSize],
        
       double dEdr[k_BlockSize] ,
       double& aEnergy0 ,
       double& aEnergy1 ,
       double& aEnergy2 ,
       double& aExtraScalarVirial 
    ) 
    { 
      double TotalEnergy0 = 0.0 ;
      double TotalEnergy1 = 0.0 ; 
      double TotalEnergy2 = 0.0 ;
      double TotalExtraScalarVirial = 0.0 ;
    double Energy0[k_BlockSize] ;
    double Energy1[k_BlockSize] ;
    double Energy2[k_BlockSize] ; 
    double ExtraScalarVirial[k_BlockSize] ;
    double AboveLowerThresholdDistances[k_BlockSize] ;
    
       unsigned short AboveLowerThresholdVector[k_BlockSize] ;
       unsigned int AboveLowerCount ;
       
    // Do the unswitched evaluation
      for ( unsigned int w0 = 0 ; w0 < IndexVectorLength ; w0 += 1)
    {
      unsigned int w = IndexVector[w0] ;
      unsigned int v=FirstSite+w ;
      EnergyAndSlope_Unswitched(
        aSimTick,
              aSiteVerletList.GetFragA(v),aSiteVerletList.GetOffsetA(v),
              aSiteVerletList.GetFragB(v),aSiteVerletList.GetOffsetB(v),
//              SquareDistance[w],
//              ReciprocalDistance[w],
              SquareDistance[w0],
              ReciprocalDistance[w0],
              SourceSiteID[w],
              TargetSiteID[w],
//              dEdr[w],
//              Energy0[w],
//              Energy1[w],
//	          Energy2[w],
//	          ExtraScalarVirial[w] 
              dEdr[w0],
              Energy0[w0],
              Energy1[w0],
            Energy2[w0],
            ExtraScalarVirial[w0] 
      ) ;
    }
      // These are the switch parameters for the shell at the CURRENT respa level
//      double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
//      double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

      //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
      double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;
      double OffRegionSquared = OffRegion*OffRegion ;
      double OnRegionSquared = SwitchLowerCutoffOuterShell *SwitchLowerCutoffOuterShell ;
    
    
// This now done by VectorsAndDistances		
//    // Work out what is in the switch region 
//    SelectAboveThreshold(
//      SquareDistance,
//      OnRegionSquared,
//      IndexVectorLength,
//      IndexVector,
//      AboveLowerThresholdVector,
//      AboveLowerCount
//    ) ;
    
    // Scale back the forces and energies in the switch (and zero them above)
    
    ConstructInSwitchIndex(
       SquareDistance,
       IndexVectorLength,
       OnRegionSquared,
       ReciprocalDistance,
       AboveLowerThresholdVector,
       AboveLowerCount,
       AboveLowerThresholdDistances
    ) ;
  SwitchBlockPacked(
    AboveLowerThresholdDistances,
      dEdr,
      Energy0,
      Energy1,
      Energy2,
      ExtraScalarVirial, 
      AboveLowerThresholdVector,
      AboveLowerCount
     ) ;
    
    
    // Work out total energies and virial
      for ( unsigned int ww0 = 0 ; ww0 < IndexVectorLength ; ww0 += 1)
    {
//			int ww=IndexVector[ww0] ;
// Trace the energies
            BegLogLine(PKFXLOG_ENERGYTRACE)
                   << "EnergyTrace "
                   << ' ' << SourceSiteID[IndexVector[ww0]]
                   << ' ' << TargetSiteID[IndexVector[ww0]]
                   << ' ' << (UDF0_Bound::UDF::ReportsEnergy ? Energy0[ww0]  : 0.0 )
                   << ' ' << (UDF1_Bound::UDF::ReportsEnergy ? Energy1[ww0]  : 0.0 )
                   << ' ' << (UDF2_Bound::UDF::ReportsEnergy ? Energy2[ww0]  : 0.0 )
            
              << EndLogLine ;
// Sum the energies
      TotalEnergy0 += Energy0[ww0] ;
      TotalEnergy1 += Energy1[ww0] ;
      TotalEnergy2 += Energy2[ww0] ;
      TotalExtraScalarVirial += ExtraScalarVirial[ww0] ;
    }
    
    
    // Write back for the caller
    aEnergy0 = TotalEnergy0 ;
    aEnergy1 = TotalEnergy1 ;
    aEnergy2 = TotalEnergy2 ;
    aExtraScalarVirial = TotalExtraScalarVirial ;
    
#if PKFXLOG_VECTOR_VERLET
        for ( unsigned int z=0;z<IndexVectorLength;z+=1)
        {
           BegLogLine(PKFXLOG_VECTOR_VERLET)
          << "EnergyAndSlope " 
          << SquareDistance[IndexVector[z]]
          << " " << dEdr[IndexVector[z]]
          << EndLogLine ; 
          
        }
#endif		
    }
     
    
// Go from scalar to vector, and update force buffers    
    template<class DynVarMgrIF, class FragPartResultIF>
    static
    void
    ForceAndWriteback(
                   int                aSimTick,
                   DynVarMgrIF      * aDynVarMgrIF,
                   unsigned int       aFragSource,
                   unsigned int       aOffsetSource,
                   unsigned int       aFragTarget,
                   unsigned int       aOffsetTarget,
                   double VectorX,
                   double VectorY,
                   double VectorZ,
                   double dEdr,
                   double ReciprocalDistance,
                   int aSourceSiteId,
                   int aTargetSiteId,
                   
                   double& aVirialX,
                   double& aVirialY,
                   double& aVirialZ
                               
    ) 
    { 
      XYZ Vector ; 
      Vector.mX = VectorX ; 
      Vector.mY = VectorY ;
      Vector.mZ = VectorZ ;
      
      XYZ Force = Vector * (-dEdr*ReciprocalDistance) ;
      XYZ Virial = PairwiseProduct( Force, Vector ) ;
      FragPartResultIF      TargetPartialResultsIF ( aSimTick, aDynVarMgrIF, aFragTarget );
      FragPartResultIF      SourcePartialResultsIF ( aSimTick, aDynVarMgrIF, aFragSource );

      int TargetSiteId = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragTarget, aOffsetTarget) ;
      int SourceSiteId = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragSource, aOffsetSource) ;
  
      BegLogLine(PKFXLOG_VECTOR_VERLET)
        << "ForceAndWriteBack "
        << aSourceSiteId 
        << " " << SourceSiteId
        << " " << aTargetSiteId
        << " " << TargetSiteId
        << EndLogLine ;
        
      assert(aSourceSiteId == SourceSiteId) ;
      assert(aTargetSiteId == TargetSiteId) ;
//	  BegLogLine(PKFXLOG_VECTOR_VERLET)
//		<< "WriteBackSite (" << aFragTarget 
//		<< "," << aOffsetTarget
//		<< " -> " << TargetSiteId
//		<< ") (" << aFragSource
//		<< "," << aOffsetSource
//		<< " -> " << SourceSiteId
//		<< ")"
//		<< EndLogLine ;
      
    XYZ TargetForce = TargetPartialResultsIF.GetForce(TargetSiteId,0) ;
      XYZ SourceForce = SourcePartialResultsIF.GetForce(SourceSiteId,0) ;

//	  XYZ TargetForce ;
//	  XYZ SourceForce ;
//	  
//	  TargetPartialResultsIF.GetForce(TargetSiteId,0,TargetForce) ;
//      SourcePartialResultsIF.GetForce(SourceSiteId,0,SourceForce) ;
      
      TargetForce += Force ;
      SourceForce -= Force ;		
    
    TargetPartialResultsIF.SetForce(TargetSiteId,0,TargetForce) ;
    SourcePartialResultsIF.SetForce(SourceSiteId,0,SourceForce) ;
    
    aVirialX = Virial.mX ; 
    aVirialY = Virial.mY ; 
    aVirialZ = Virial.mZ ;
    
    // ERRATA avoidance
    Vector.Zero() ;
    Force.Zero() ;
    Virial.Zero() ;
    TargetForce.Zero() ;
    SourceForce.Zero() ;
    
    } 
    template<class DynVarMgrIF, class FragPartResultIF>
    static
    void
    ForceAndWritebackNoVirial(
                   int                aSimTick,
                   DynVarMgrIF      * aDynVarMgrIF,
                   unsigned int       aFragSource,
                   unsigned int       aOffsetSource,
                   unsigned int       aFragTarget,
                   unsigned int       aOffsetTarget,
//                   double* AddressSource,
//                   double* AddressTarget,
                   double VectorX,
                   double VectorY,
                   double VectorZ,
                   double dEdr,
                   double ReciprocalDistance,
                   int aSourceSiteId,
                   int aTargetSiteId
                               
    ) 
    { 
      
      assert(is_almost_zero((VectorX*VectorX + VectorY*VectorY + VectorZ*VectorZ)*ReciprocalDistance*ReciprocalDistance - 1.0)) ;
      double scaling = -dEdr*ReciprocalDistance ;
      
      double ForceX = VectorX * scaling ; 
      double ForceY = VectorY * scaling ; 
      double ForceZ = VectorZ * scaling ; 
      
      FragPartResultIF      TargetPartialResultsIF ( aSimTick, aDynVarMgrIF, aFragTarget );
      FragPartResultIF      SourcePartialResultsIF ( aSimTick, aDynVarMgrIF, aFragSource );

      int TargetSiteId = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragTarget, aOffsetTarget) ;
      int SourceSiteId = MSD_IF::GetIrreducibleFragmentMemberSiteId( aFragSource, aOffsetSource) ;
  
      BegLogLine(PKFXLOG_VECTOR_VERLET)
        << "ForceAndWriteBack "
        << aSourceSiteId 
        << " " << SourceSiteId
        << " " << aTargetSiteId
        << " " << TargetSiteId
        << EndLogLine ;
        
      assert(aSourceSiteId == SourceSiteId) ;
      assert(aTargetSiteId == TargetSiteId) ;
//	  BegLogLine(PKFXLOG_VECTOR_VERLET)
//		<< "WriteBackSite (" << aFragTarget 
//		<< "," << aOffsetTarget
//		<< " -> " << TargetSiteId
//		<< ") (" << aFragSource
//		<< "," << aOffsetSource
//		<< " -> " << SourceSiteId
//		<< ")"
//		<< EndLogLine ;
      
    
//	  double TargetForceX, TargetForceY, TargetForceZ ; 
//	  double SourceForceX, SourceForceY, SourceForceZ ; 
     
    double TargetForceX=TargetPartialResultsIF.ForceX(TargetSiteId,0) ;
    double TargetForceY=TargetPartialResultsIF.ForceY(TargetSiteId,0) ;
    double TargetForceZ=TargetPartialResultsIF.ForceZ(TargetSiteId,0) ;
    
    double SourceForceX=SourcePartialResultsIF.ForceX(SourceSiteId,0) ;
    double SourceForceY=SourcePartialResultsIF.ForceY(SourceSiteId,0) ;
    double SourceForceZ=SourcePartialResultsIF.ForceZ(SourceSiteId,0) ;
    
//	  double TargetForce0X = AddressTarget[0] ;
//	  double TargetForce0Y = AddressTarget[1] ;
//	  double TargetForce0Z = AddressTarget[2] ;
//	 
//	  double SourceForce0X = AddressSource[0] ;
//	  double SourceForce0Y = AddressSource[1] ;
//	  double SourceForce0Z = AddressSource[2] ;
//	  
//	  BegLogLine(0)
//	    << "ForceAndWritebackNoVirial "
//	    << TargetForceX << " " << TargetForce0X << " " 
//	    << TargetForceY << " " << TargetForce0Y << " "
//	    << TargetForceZ << " " << TargetForce0Z << " "
//	    << SourceForceX << " " << SourceForce0X << " "
//	    << SourceForceY << " " << SourceForce0Y << " "
//	    << SourceForceZ << " " << SourceForce0Z
//	    << EndLogLine ;
//	    
//	 
//	  assert(is_almost_zero(TargetForceX-TargetForce0X)) ;
//	  assert(is_almost_zero(TargetForceY-TargetForce0Y)) ;
//	  assert(is_almost_zero(TargetForceZ-TargetForce0Z)) ;
//	  assert(is_almost_zero(SourceForceX-SourceForce0X)) ;
//	  assert(is_almost_zero(SourceForceY-SourceForce0Y)) ;
//	  assert(is_almost_zero(SourceForceZ-SourceForce0Z)) ;
    
//	  TargetPartialResultsIF.GetForce(TargetSiteId,0,TargetForceX, TargetForceY, TargetForceZ) ;
//      SourcePartialResultsIF.GetForce(SourceSiteId,0,SourceForceX, SourceForceY, SourceForceZ) ;


    TargetPartialResultsIF.SetForce(TargetSiteId,0,TargetForceX+ForceX, TargetForceY+ForceY, TargetForceZ+ForceZ) ;
    SourcePartialResultsIF.SetForce(SourceSiteId,0,SourceForceX-ForceX, SourceForceY-ForceY, SourceForceZ-ForceZ) ;
    
//	  AddressTarget[0] = TargetForceX+ForceX ;
//	  AddressTarget[1] = TargetForceY+ForceY ;
//	  AddressTarget[2] = TargetForceZ+ForceZ ;
//	  
//	  AddressSource[0] = SourceForceX-ForceX ;
//	  AddressSource[1] = SourceForceY-ForceY ;
//	  AddressSource[2] = SourceForceZ-ForceZ ;
    
    
    } 
    template<class DynVarMgrIF>
    static
    void
    EvaluateVirial(
                   DynVarMgrIF      * aDynVarMgrIF,
                   double VectorX,
                   double VectorY,
                   double VectorZ,
                   double dEdr,
                   double ReciprocalDistance,
                   
                   double& aVirialX,
                   double& aVirialY,
                   double& aVirialZ
                               
    ) 
    { 
      XYZ Vector ; 
      Vector.mX = VectorX ; 
      Vector.mY = VectorY ;
      Vector.mZ = VectorZ ;
      
      XYZ Force = Vector * (-dEdr*ReciprocalDistance) ;
      XYZ Virial = PairwiseProduct( Force, Vector ) ;
  
      BegLogLine(PKFXLOG_VECTOR_VERLET)
        << "EvaluateVirial "
        << EndLogLine ;
        
    
    aVirialX = Virial.mX ; 
    aVirialY = Virial.mY ; 
    aVirialZ = Virial.mZ ;
    
    // ERRATA avoidance
    Vector.Zero() ;
    Force.Zero() ;
    Virial.Zero() ;
    
    } 
    
    template<class DynVarMgrIF, class FragPartResultIF>
    static
    void
    ForcesAndWriteback(
              int     aSimTick,
              DynVarMgrIF      *      aDynVarMgrIF,
              const SiteVerletList& aSiteVerletList,
              int FirstSite,
              int Length,
              double VectorX[k_BlockSize],
              double VectorY[k_BlockSize],
              double VectorZ[k_BlockSize],
              double dEdr[k_BlockSize] ,
              double ReciprocalDistance[k_BlockSize] ,
              int    SourceSiteID[k_BlockSize] ,
              int    TargetSiteID[k_BlockSize],
              
              XYZ& aVectorVirial 
    ) 
    { 
        double vvX = 0.0 ; 
        double vvY = 0.0 ; 
        double vvZ = 0.0 ; 
      for ( unsigned int w = 0 ; w < Length ; w += 1)
    {
      unsigned int v=FirstSite+w ;
      double VirialX ; 
      double VirialY ; 
      double VirialZ ;
            ForceAndWriteback<DynVarMgrIF,FragPartResultIF>(
                   aSimTick,
                   aDynVarMgrIF,
              aSiteVerletList.GetFragA(v),aSiteVerletList.GetOffsetA(v),
              aSiteVerletList.GetFragB(v),aSiteVerletList.GetOffsetB(v),
//              aSiteVerletList.AddressElAForce(v),
//              aSiteVerletList.AddressElBForce(v),
                   VectorX[w],
                   VectorY[w],
                   VectorZ[w],
                   dEdr[w],
                   ReciprocalDistance[w],
                   SourceSiteID[w],
                   TargetSiteID[w],
                   
                   VirialX,
                   VirialY,
                   VirialZ
            ) ;
            vvX += VirialX ;
            vvY += VirialY ; 
            vvZ += VirialZ ;
    }
    
    aVectorVirial.mX = vvX ;
    aVectorVirial.mY = vvY ; 
    aVectorVirial.mZ = vvZ ;
    } 
    template<class DynVarMgrIF, class FragPartResultIF>
    static
    void
    ForcesAndWritebackIndexed(
              int     aSimTick,
              DynVarMgrIF      *      aDynVarMgrIF,
              const SiteVerletList& aSiteVerletList,
              int FirstSite,
              unsigned short IndexVector[k_BlockSize],
              int IndexVectorLength,
              double VectorX[k_BlockSize],
              double VectorY[k_BlockSize],
              double VectorZ[k_BlockSize],
              double dEdr[k_BlockSize] ,
              double ReciprocalDistance[k_BlockSize] ,
              int    SourceSiteID[k_BlockSize] ,
              int    TargetSiteID[k_BlockSize],
              
              XYZ& aVectorVirial 
    ) 
    { 
      if ( k_DoPressureControl )
      {
        // Do the virial
          double vvX = 0.0 ; 
          double vvY = 0.0 ; 
          double vvZ = 0.0 ; 
        for ( unsigned int w00 = 0 ; w00 < IndexVectorLength ; w00 += 1)
      {
        unsigned int w = IndexVector[w00] ;
        unsigned int v=FirstSite+w ;
        double VirialX ; 
        double VirialY ; 
        double VirialZ ;
              EvaluateVirial<DynVarMgrIF>(
                     aDynVarMgrIF,
                     VectorX[w00],
                     VectorY[w00],
                     VectorZ[w00],
                     dEdr[w00],
                     ReciprocalDistance[w00],
                     
                     VirialX,
                     VirialY,
                     VirialZ
              ) ;
              vvX += VirialX ;
              vvY += VirialY ; 
              vvZ += VirialZ ;
      }
      aVectorVirial.mX = vvX ;
      aVectorVirial.mY = vvY ; 
      aVectorVirial.mZ = vvZ ;
      }
      
      // Do the force
      for ( unsigned int w0 = 0 ; w0 < IndexVectorLength ; w0 += 1)
    {
      unsigned int w = IndexVector[w0] ;
      unsigned int v=FirstSite+w ;
            ForceAndWritebackNoVirial<DynVarMgrIF,FragPartResultIF>(
                   aSimTick,
                   aDynVarMgrIF,
              aSiteVerletList.GetFragA(v),aSiteVerletList.GetOffsetA(v),
              aSiteVerletList.GetFragB(v),aSiteVerletList.GetOffsetB(v),
//              aSiteVerletList.AddressElAForce(v),
//              aSiteVerletList.AddressElBForce(v),
              
                   VectorX[w0],
                   VectorY[w0],
                   VectorZ[w0],
                   dEdr[w0],
                   ReciprocalDistance[w0],
                   SourceSiteID[w],
                   TargetSiteID[w]
            ) ;
    }
    
    } 
    
    
    
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class FragPartResultIF,
             class GlobalResultIF>
    static
    void
  ExecuteSiteVerletBlock(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
           const SiteVerletList& aSiteVerletList,
           unsigned int FirstSite,
           unsigned int Length)  
    {
//    	XYZ ForceBlock[k_BlockSize] ;
//    	XYZ VirialBlock[k_BlockSize] ;
//    	double Energy0Block[k_BlockSize] ;
//    	double Energy1Block[k_BlockSize] ;
//    	double Energy2Block[k_BlockSize] ;

        XYZ VectorVirial ;

        double VectorX[k_BlockSize] ;
        double VectorY[k_BlockSize] ;
        double VectorZ[k_BlockSize] ;
        double SquareDistance[k_PaddedBlockSizeIn] ;
        double ReciprocalDistance[k_PaddedBlockSizeOut] ;
        double dEdr[k_BlockSize] ;
        int SourceSiteID[k_BlockSize] ;
        int TargetSiteID[k_BlockSize] ; 

        double Energy0 ; 
        double Energy1 ;
        double Energy2 ;
        double ExtraScalarVirial ;
        
        BegLogLine(PKFXLOG_VECTOR_VERLET)
          << "ExecuteSiteVerletBlock " << FirstSite 
          << " " << Length
          << EndLogLine ;
      unsigned short IndexBelowUpper[k_PaddedBlockSizeIn] ;
      unsigned short InSwitchRegionVector[k_BlockSize] ;
      double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
      double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;
      double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;
      double OffRegionSquared = OffRegion*OffRegion ;
      double OnRegionSquared = SwitchLowerCutoffOuterShell *SwitchLowerCutoffOuterShell ;
      unsigned int BelowUpperCount ;
      unsigned int InSwitchCount ;

        VectorsAndDistances<GlobalInputIF,DynVarMgrIF,GlobalResultIF>(
              aSimTick,
              aGlobalInputParametersIF,
              aDynVarMgrIF,
              aSiteVerletList,
              FirstSite,
              Length,
              VectorX,
              VectorY,
              VectorZ,
              SquareDistance,
              IndexBelowUpper,
              BelowUpperCount,
              OffRegionSquared
//              ,
//              InSwitchRegionVector,
//              InSwitchCount,
//              OnRegionSquared
//              ,
//              ReciprocalDistance
         ) ;
         

// This now done by VectorsAndDistances
//        // First 'compression' pass --- select the pairs within upper cutoff
//        
//        SelectBelowThreshold(
//              SquareDistance,
//              OffRegionSquared,
//              Length,
//              IndexBelowUpper,
//              BelowUpperCount
//        ) ;
//        
//        // Could do a 'compression' pass here to delete everything above 
//        // upper threshold. Would need to keep around a 'compressed' block-ful of
//        // the site verlet list; not too bad a thing to do.
        
//        ReciprocalDistances(SquareDistance,ReciprocalDistance,Length) ;

//        ReciprocalDistancesIndexed(SquareDistance,ReciprocalDistance,IndexBelowUpper,BelowUpperCount) ;
        
        
        EnergiesAndSlopesIndexed(
           aSimTick,
           aSiteVerletList,
           FirstSite,
           IndexBelowUpper,
           BelowUpperCount,
           SwitchLowerCutoffOuterShell,
           SwitchDeltaOuterShell,
           SquareDistance,
           ReciprocalDistance,
           SourceSiteID,
           TargetSiteID,
           dEdr ,
           Energy0 ,
           Energy1 ,
           Energy2 ,
           ExtraScalarVirial 
//           ,
//           InSwitchRegionVector,
//           InSwitchCount
        ) ;
        
        ForcesAndWritebackIndexed<DynVarMgrIF,FragPartResultIF>(
              aSimTick,
              aDynVarMgrIF,
              aSiteVerletList,
              FirstSite,
              IndexBelowUpper,
              BelowUpperCount,
              VectorX,
              VectorY,
              VectorZ,
              dEdr ,
              ReciprocalDistance ,
              SourceSiteID,
              TargetSiteID,
              
              VectorVirial 
        ) ;

        // Write back virial and energies
        XYZ SummedVirial = VectorVirial + SetToScalar(ExtraScalarVirial*(1.0/3.0) ) ;
    aGlobalResultIF.AddVirial<k_DoPressureControl>( 0, SummedVirial ) ; 
      aGlobalResultIF.AddEnergy<UDF0_Bound::UDF::ReportsEnergy>( 0, Energy0 );
        aGlobalResultIF.AddEnergy<UDF1_Bound::UDF::ReportsEnergy>( 1, Energy1 );
        aGlobalResultIF.AddEnergy<UDF2_Bound::UDF::ReportsEnergy>( 2, Energy2 );
        
        
    }			       
    
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class FragPartResultIF,
             class GlobalResultIF>
    static
    void
  ExecuteSiteVerletBlock_NoInline(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
           const SiteVerletList& aSiteVerletList,
           unsigned int FirstSite,
           unsigned int Length) 
      __attribute__((noinline)) 
      ;
    
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class FragmentResultIF,
             class GlobalResultIF>
    static
    void
    ExecuteN33VerletList(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
                   const FVerletList& aN33VerletList)
      __attribute__((noinline)) 
             ;
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class FragPartResultIF,
             class GlobalResultIF>
    static
    void
  ExecuteSiteVerletListByBlock(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
           const SiteVerletList& aSiteVerletList) 
    {
    unsigned int Limit = aSiteVerletList.GetCount() ;
    unsigned int Blocks = Limit / k_BlockSize ;
    unsigned int Remainder = Limit % k_BlockSize ;
    for ( unsigned int x = 0 ; x < Blocks ; x += 1 )
    {
      ExecuteSiteVerletBlock_NoInline<GlobalInputIF,DynVarMgrIF,FragPartResultIF,GlobalResultIF>
      (
        aSimTick,
              aGlobalInputParametersIF,
              aDynVarMgrIF,
              aGlobalResultIF,
              aSiteVerletList,
              x*k_BlockSize,
              k_BlockSize
      ) ;
    } 
    if ( Remainder > 0 ) 
    {
      ExecuteSiteVerletBlock_NoInline<GlobalInputIF,DynVarMgrIF,FragPartResultIF,GlobalResultIF>
      (
        aSimTick,
              aGlobalInputParametersIF,
              aDynVarMgrIF,
              aGlobalResultIF,
              aSiteVerletList,
              Blocks*k_BlockSize,
              Remainder
      ) ;
      
    }
    }			       
        
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class FragmentResultIF,
             class GlobalResultIF>
    static
    void
  ExecuteSiteVerletList(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
           const SiteVerletList& aSiteVerletList) 
             ;

  }; /* IFPWaterSite */

template <class NsqUdfGroup
        , class SwitchFunctionEnergyAdapter
        , class SwitchFunctionForceAdapter
        , int SHELL_COUNT
        , int CONTROL_FLAGS>
    template<class GlobalInputIF,
             class DynVarMgrIF,
             class FragPartResultIF,
             class GlobalResultIF>
    void
  IFPWaterSite<NsqUdfGroup
             , SwitchFunctionEnergyAdapter
             , SwitchFunctionForceAdapter
             , SHELL_COUNT
             , CONTROL_FLAGS>::ExecuteSiteVerletBlock_NoInline(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
           const SiteVerletList& aSiteVerletList,
           unsigned int FirstSite,
           unsigned int Length) 
      __attribute__((noinline)) 
{
  ExecuteSiteVerletBlock<GlobalInputIF,DynVarMgrIF,FragPartResultIF,GlobalResultIF>
  ( aSimTick
   ,aGlobalInputParametersIF
   ,aDynVarMgrIF
   ,aGlobalResultIF
   ,aSiteVerletList
   ,FirstSite
   ,Length ) ;
}

#if defined(IFP_COMPILE_EXECUTE) || !defined(IFP_SEPARATE_EXECUTE)  
template <class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int SHELL_COUNT, int CONTROL_FLAGS>
  void IFPWaterSite<NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter,SHELL_COUNT,  CONTROL_FLAGS>::Init()
     {
       IFPHelper0::Init();
     }

template <class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int SHELL_COUNT, int CONTROL_FLAGS>
  template <
    class GlobalInputIF, class DynVarMgrIF, class FragPartResultIF, class GlobalResultIF>
    void
    IFPWaterSite<NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter,SHELL_COUNT,  CONTROL_FLAGS>::
      ExecuteN33VerletList
    ( int           aSimTick,
      GlobalInputIF    & aGlobalInputParametersIF,
      DynVarMgrIF      * aDynVarMgrIF,
      GlobalResultIF   & aGlobalResultIF,
      const FVerletList& aN33VerletList
    ) {
    unsigned int Limit = aN33VerletList.GetCount() ;
    BegLogLine(PKFXLOG_VECTOR_VERLET)
      << "IFPWaterSite::ExecuteN33VerletList count=" << Limit
      << EndLogLine ;
    for ( unsigned int x = 0 ; x < Limit ; x += 1)
    {
      BegLogLine(PKFXLOG_VECTOR_VERLET)
            << "IFPWaterSite::ExecuteN33VerletList x=" << x
            << " ElA=" << aN33VerletList.GetElA(x)
            << " ElB=" << aN33VerletList.GetElB(x)
            << " ImVX=" << aN33VerletList.GetImageVectorTableIndex(x)
            << EndLogLine ;
      
      ExecuteN33<GlobalInputIF,DynVarMgrIF,FragPartResultIF,GlobalResultIF>
      (
        aSimTick,
              aGlobalInputParametersIF,
              aDynVarMgrIF, 
              aGlobalResultIF,
              aN33VerletList.GetElA(x),
              aN33VerletList.GetElB(x),
              aN33VerletList.GetImageVectorTableIndex(x) 
      ) ;
    }
  } 
    
template <class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int SHELL_COUNT, int CONTROL_FLAGS>
  template <class GlobalInputIF, class DynVarMgrIF, class FragPartResultIF, class GlobalResultIF>
    void
    IFPWaterSite<NsqUdfGroup, SwitchFunctionEnergyAdapter, SwitchFunctionForceAdapter,SHELL_COUNT,  CONTROL_FLAGS>::
  ExecuteSiteVerletList(
                   int                aSimTick,
                   GlobalInputIF    & aGlobalInputParametersIF,
                   DynVarMgrIF      * aDynVarMgrIF,
                   GlobalResultIF   & aGlobalResultIF,
             const SiteVerletList& aSiteVerletList) 
    {
      ExecuteSiteVerletListByBlock<GlobalInputIF,DynVarMgrIF,FragPartResultIF,GlobalResultIF>
       (
        aSimTick,
              aGlobalInputParametersIF,
              aDynVarMgrIF,
              aGlobalResultIF,
              aSiteVerletList     	 
       ) ;
    }			       
#endif

#if defined(IFP_COMPILE_EXECUTE) && defined(IFP_SEPARATE_EXECUTE)     
// This should cause the compiler to instantiate exactly the kind of IFPWaterSite that we need.
// We're going to need 'Init', 'Execute', and 'ExecuteInDifferentMolecules' functions.
template
    void
    IFPWaterSite<
//    NsqUdfGroup
    UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
//     SwitchFunctionEnergyAdapter
    ,UDF_SwitchFunction
//    SwitchFunctionForceAdapter
    ,UDF_SwitchFunctionForce
//    SHELL_COUNT
    ,1
//    CONTROL_FLAGS
    ,1
    >::Init() ;

template
    void
    IFPWaterSite<
//    NsqUdfGroup
    UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
//     SwitchFunctionEnergyAdapter
    ,UDF_SwitchFunction
//    SwitchFunctionForceAdapter
    ,UDF_SwitchFunctionForce
//    SHELL_COUNT
    ,1
//    CONTROL_FLAGS
    ,1
    >::ExecuteN33VerletList
    <
//    class GlobalInputIF
        NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF
//    , class DynamicVariableManager
      ,  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
      
//    , class   FragPartResultIF
      ,  FragmentPartialResultsIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >

//    , class GlobalResultIF
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>

    >
    (
//           int           aSimTick,
         int
//           GlobalInputIF &aGlobalInputParametersIF,
        ,NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF&
//           DynamicVariableManager &aDynamicVariableManager,
        ,TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> *aDynamicVariableManager
//           GlobalResultIF       &aGlobalResultIF,
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>&

        ,    const FVerletList&
 ) ;

template    
    void
    IFPWaterSite<
//    NsqUdfGroup
    UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
//     SwitchFunctionEnergyAdapter
    ,UDF_SwitchFunction
//    SwitchFunctionForceAdapter
    ,UDF_SwitchFunctionForce
//    SHELL_COUNT
    ,1
//    CONTROL_FLAGS
    ,1
    >::ExecuteSiteVerletList
    <
//    class GlobalInputIF
        NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF
//    , class DynamicVariableManager
      ,  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> 
      
//    , class   FragPartResultIF
      ,  FragmentPartialResultsIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >

//    , class GlobalResultIF
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>

    >
    (
//           int           aSimTick,
         int
//           GlobalInputIF &aGlobalInputParametersIF,
        ,NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF&
//           DynamicVariableManager& aDynamicVariableManager,
        ,TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> * aDynamicVariableManager
//           GlobalResultIF       &aGlobalResultIF,
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>&

        ,    const SiteVerletList&
 ) ;
 

// And the corresponding 'core 0' versions of the execute functions
template
    void
    IFPWaterSite<
//    NsqUdfGroup
    UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
//     SwitchFunctionEnergyAdapter
    ,UDF_SwitchFunction
//    SwitchFunctionForceAdapter
    ,UDF_SwitchFunctionForce
//    SHELL_COUNT
    ,1
//    CONTROL_FLAGS
    ,1
    >::ExecuteN33VerletList
    <
//    class GlobalInputIF
        NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF
//    , class DynamicVariableManager
      ,  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
      
//    , class   FragPartResultIF
      ,  FragmentPartialResultsIFCore0<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >

//    , class GlobalResultIF
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>

    >
    (
//           int           aSimTick,
         int
//           GlobalInputIF &aGlobalInputParametersIF,
        ,NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF&
//           DynamicVariableManager &aDynamicVariableManager,
        ,TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> *aDynamicVariableManager
//           GlobalResultIF       &aGlobalResultIF,
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>&

        ,    const FVerletList&
 ) ;

template    
    void
    IFPWaterSite<
//    NsqUdfGroup
    UdfGroup<UDF_NSQChargeRealSpace_Bound,UDF_LennardJonesForce_Bound,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
//     SwitchFunctionEnergyAdapter
    ,UDF_SwitchFunction
//    SwitchFunctionForceAdapter
    ,UDF_SwitchFunctionForce
//    SHELL_COUNT
    ,1
//    CONTROL_FLAGS
    ,1
    >::ExecuteSiteVerletList
    <
//    class GlobalInputIF
        NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF
//    , class DynamicVariableManager
      ,  TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> 
      
//    , class   FragPartResultIF
      ,  FragmentPartialResultsIFCore0<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >

//    , class GlobalResultIF
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>

    >
    (
//           int           aSimTick,
         int
//           GlobalInputIF &aGlobalInputParametersIF,
        ,NSQEngine<
          TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                           ,UDF_LennardJonesForce_Bound
                           ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >
                  ,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalInputParametersIF&
//           DynamicVariableManager& aDynamicVariableManager,
        ,TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> * aDynamicVariableManager
//           GlobalResultIF       &aGlobalResultIF,
        ,NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>
         ,IFPWaterSite<UdfGroup< UDF_NSQChargeRealSpace_Bound
                            ,UDF_LennardJonesForce_Bound
                            ,IFP_Binder<NULL_IFP_UDF,NULL_IFP_ParamAccessor> >,UDF_SwitchFunction,UDF_SwitchFunctionForce,1,1>
         ,T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> >
         >::Client::GlobalPartialResultsIF<0>&

        ,    const SiteVerletList&
 ) ;
//// 'ExecuteSiteVerletList' to run on core 1, with different output buffers 
// template 
// void IFPWaterSite
//  <UdfGroup
//    <IFP_Binder <UDF_NSQEwaldRealSpace, NSQChargeParamAccessor>
//   , IFP_Binder<UDF_NSQCHARMMLennardJonesForce, NSQLennardJonesParamAccessor>
//   , IFP_Binder<NULL_IFP_UDF, NULL_IFP_ParamAccessor>
//  >, UDF_SwitchFunction, UDF_SwitchFunctionForce, 1, 1>
//  ::ExecuteSiteVerletList
//  <NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1>
//  , IFPWaterSite<
//     UdfGroup<IFP_Binder<UDF_NSQEwaldRealSpace, NSQChargeParamAccessor>
//            , IFP_Binder<UDF_NSQCHARMMLennardJonesForce, NSQLennardJonesParamAccessor>
//            , IFP_Binder<NULL_IFP_UDF, NULL_IFP_ParamAccessor> >
//        , UDF_SwitchFunction, UDF_SwitchFunctionForce, 1, 1
//        >
//
//  , T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1> >
//   >::Client::GlobalInputParametersIF
//
//   , TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1>
//   , NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1>
//      , IFPWaterSite<UdfGroup<IFP_Binder<UDF_NSQEwaldRealSpace, NSQChargeParamAccessor>
//                            , IFP_Binder<UDF_NSQCHARMMLennardJonesForce, NSQLennardJonesParamAccessor>
//                            , IFP_Binder<NULL_IFP_UDF, NULL_IFP_ParamAccessor>
//                            >
//                   , UDF_SwitchFunction
//                   , UDF_SwitchFunctionForce
//                   , 1
//                   , 1>
//      , T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1> >
//       >::Client::GlobalPartialResultsIF<0>
//       >
//
//  (int
//  , NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1>
//      , IFPWaterSite<UdfGroup<IFP_Binder<UDF_NSQEwaldRealSpace, NSQChargeParamAccessor>
//                            , IFP_Binder<UDF_NSQCHARMMLennardJonesForce, NSQLennardJonesParamAccessor>
//                            , IFP_Binder<NULL_IFP_UDF, NULL_IFP_ParamAccessor> >
//                   , UDF_SwitchFunction, UDF_SwitchFunctionForce, 1, 1>
//      , T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1> >
//    >::Client::GlobalInputParametersIF&
//   , TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1>*
//   , NSQEngine<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1>
//             , IFPWaterSite<UdfGroup<IFP_Binder<UDF_NSQEwaldRealSpace, NSQChargeParamAccessor>
//                                   , IFP_Binder<UDF_NSQCHARMMLennardJonesForce, NSQLennardJonesParamAccessor>
//                                   , IFP_Binder<NULL_IFP_UDF, NULL_IFP_ParamAccessor> >
//                          , UDF_SwitchFunction
//                          , UDF_SwitchFunctionForce
//                          , 1
//                          , 1
//                           >
//             , T_ORBManagerIF<TDynamicVariableManager<NUMBER_OF_SITES, NUMBER_OF_FRAGMENTS, 1> >
//              >::Client::GlobalPartialResultsIF<0>&
//              , const SiteVerletList&) ;
 
#endif

#endif
