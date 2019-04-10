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
 
#ifndef __IFP_SPLIT__HPP__
#define __IFP_SPLIT__HPP__

#include <BlueMatter/IFPHelper.hpp>

#undef PKFXLOG_IPARTPAIR
#define PKFXLOG_IPARTPAIR 1

template<class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int SHELL_COUNT, int CONTROL_FLAGS>
class IFPSplit
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
        static inline void ExecuteWithExclusions( NBVM_T&         aNbvm,
                                    unsigned int&   aPairInteractionMask,
                                    int&            aSourceId,
                                    int&            aTargetId,
                                    XYZ&            aForce,
                                    double&         aNewEnergy,
                                    XYZ&            aVirial
                                    )
        {

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
                        aNewEnergy = aNbvm.GetEnergy( ORDINAL );
    
                        if( UDF_Bound::UDF::ReportsVirial )
                            aVirial = aNbvm.GetVirial( ORDINAL );
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
                aNewEnergy = aNbvm.GetEnergy( ORDINAL );
    
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
              
            } else {
               TotalPairOpsOutsideSiteSiteCutoff.Increment(); 
        
            } /* endif distance cutoff */
              
          } /* endfor targetfragmentsiteindex */
        } /* endfor sourcefragmentsiteindex */
    } /* IFPSimple::ExecuteWithoutExclusionsCountingBothTriangle */
    
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
        for(int TargetFragmentSiteIndex = 0;
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
              
            } else {
               TotalPairOpsOutsideSiteSiteCutoff.Increment(); 
        
            } /* endif distance cutoff */
              
          } /* endfor targetfragmentsiteindex */
        } /* endfor sourcefragmentsiteindex */
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
