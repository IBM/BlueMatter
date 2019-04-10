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

#ifndef __IFP__HPP__
#define __IFP__HPP__

#include <BlueMatter/IFPHelper.hpp>

template<class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int SHELL_COUNT, int CONTROL_FLAGS>
class IFP
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
        static inline void Execute( NBVM_T&     aNbvm,
                                    unsigned int&      aPairInteractionMask,
                                    int&      aSourceId,
                                    int&      aTargetId,
                                    XYZ&      aOldForce,
                                    XYZ&      aNewForce,
                                    double&   aOldEnergy,
                                    double&   aNewEnergy,
                                    XYZ&      aOldVirial,
                                    XYZ&      aNewVirial,
                                    int&      aNumberOfShells,
                                    int&      aFullTimeStep,
                                    int&      aCurrentSwitchType
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

                            aOldForce = aNbvm.GetForce( NBVM_T::SiteA );
                            aOldEnergy = aNbvm.GetEnergy( ORDINAL );

                            if( UDF_Bound::UDF::ReportsVirial )
                                aOldVirial = aNbvm.GetVirial( ORDINAL );

                            if( aCurrentSwitchType != IFPHelper0::SWITCH_FULLY_ON )
                                {
                                    // If respa shells are disabled then we're doing the outer switch.
                                    // Make sure that the force is put on the last respa level
                                    if( !UDF_Bound::UDF::RespaShellsEnabled )
                                        aNbvm.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ aNumberOfShells-1 ] );

                                    // If we're doing the respa correction udf, then we are in an outer timestep
                                    // situation therefore we need to switch in the energy.

                                    if( aFullTimeStep && (aCurrentSwitchType == IFPHelper0::SWITCH_ZONE_1) )
                                        SwitchFunctionEnergyAdapter::Execute( aNbvm );
                                    else
                                        SwitchFunctionForceAdapter::Execute( aNbvm );
                                }

                            aNewForce = aNbvm.GetForce( NBVM_T::SiteA );
                            aNewEnergy = aNbvm.GetEnergy( ORDINAL );

                            if( UDF_Bound::UDF::ReportsVirial )
                                aNewVirial = aNbvm.GetVirial( ORDINAL );
                        }
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
        << "IrreducibleFragmentPair(): STARTING "
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

      // #if MSDDEF_DoPressureControl
      //   XYZ EwaldFragmentVirial;
      //   XYZ NonbondVirial;
      //   EwaldFragmentVirial.Zero();
      //   NonbondVirial.Zero();
      // #endif

      // NEED: Should create an array of these suckers and fill out the distances
      //  for the whole interaction as a vector computation.

#ifdef IFP_STATIC_STORAGE_FOR_VECTOR
#define IFP_VECTOR_STORAGE_CLASS static
#else
#define IFP_VECTOR_STORAGE_CLASS
#endif

      IFP_VECTOR_STORAGE_CLASS NBVMX nbvm;
      nbvm.SetSimTick( aSimTick );

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

          unsigned PairInteractionMask;

          // We expect to check this array for NULL as a template arg.
          if( PairInteractionMaskArray == NULL )
            {
            // when there is no persistant state, assume all UDFs compiled in will fire
            PairInteractionMask =
              ( UDF0_Bound::UDF::Code == -1 ? 0 : 1<<0 ) |
              ( UDF1_Bound::UDF::Code == -1 ? 0 : 1<<1 ) |
              ( UDF2_Bound::UDF::Code == -1 ? 0 : 1<<2 ) ;
            }
          else
            {
            PairInteractionMask = PairInteractionMaskArray[ PairInteractionMaskIndex ++ ];
            }

          if( PairInteractionMask == 0 )
             continue;

#if BLUEMATTER_STATS0
          TotalPairOpsCounter.Increment();
#endif

          // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
          XYZ SiteImageB;
          NearestImageInPeriodicVolume( SiteA, *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );

          // The following is probably a very expensive way to find out the AbsSiteId's - these could be carried in with the Src/TrgFragmentIF
          int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
          int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

          double DistanceSquared = SiteImageB.DistanceSquared( SiteA );

          // These are the switch parameters for the shell at the CURRENT respa level
          double SwitchLowerCutoffOuterShell = aGlobalInputParametersIF.mSwitchLowerCutoff;
          double SwitchDeltaOuterShell = aGlobalInputParametersIF.mSwitchDelta;

          //          double SwitchUpperCutoff = aGlobalInputParametersIF.mSwitchLowerCutoff + aGlobalInputParametersIF.mSwitchDelta ;
          double OffRegion = SwitchLowerCutoffOuterShell + SwitchDeltaOuterShell;

          if( DistanceSquared >  ( OffRegion * OffRegion ) )
            {
              #if BLUEMATTER_STATS0
                TotalPairOpsOutsideSiteSiteCutoff.Increment()
              #endif
              continue;
            }

          XYZ Vector = SiteA - SiteImageB ;
          nbvm.Prime( Vector );

          // If we get here we definetely are in one of the regions in the shell
          double distanceAB = nbvm.GetDistance( NBVMX::SiteA, NBVMX::SiteB );

          int currentShell      = 0;
          int currentSwitchType = IFPHelper0::SWITCH_FULLY_ON;
          int numberOfShells    = aGlobalInputParametersIF.mNumberOfShells;

          int FullTimeStep  = ( numberOfShells == SHELL_COUNT );

          for( int shellId=0; shellId < numberOfShells; shellId++)
            {
              double switchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( shellId );
              double switchDelta = MSD_IF::GetSwitchDeltaForShell( shellId );

              if( distanceAB <= switchLowerCutoff )
                {
                  // We are in an ON region no need for the switch funtion
                  nbvm.SetSwitchFunctionRadii( RTG.mSwitchOffRadius );
                  currentSwitchType = IFPHelper0::SWITCH_FULLY_ON;
                  currentShell = shellId;
                  break;
                }
              else if( distanceAB < (switchLowerCutoff+switchDelta) )
                {
                  // We are in a switch region. 1 Switch vs. 2 Switches
                  nbvm.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ shellId ] );
                  if( shellId == (numberOfShells-1) )
                    currentSwitchType = IFPHelper0::SWITCH_ZONE_1;
                  else
                    currentSwitchType = IFPHelper0::SWITCH_ZONE_2;

                  // In this case the currentRespaLevel is the respaLevel for the Off Switch (the decreasing switch)
                  currentShell = shellId;
                  break;
                }
            }

          BegLogLine(PKFXLOG_IPARTPAIR)
            << "WorkThread(): " << " NBVMX:: " << " Posit A " << SiteA << " Posit B "
            << SiteImageB << " DistanceAB " << nbvm.GetDistance( NBVMX::SiteA, NBVMX::SiteB )
            << EndLogLine;

          BegLogLine(PKFXLOG_IPARTPAIR)
            << "*** ABOUT NSQ SITE PAIR *** "
            << " SourceAbsSiteId " <<  SourceAbsSiteId
            << " TargetAbsSiteId " <<  TargetAbsSiteId
            << EndLogLine;


          XYZ OldForce0;
          XYZ NewForce0;
          XYZ OldVirial0;
          XYZ NewVirial0;
          double OldEnergy0;
          double NewEnergy0;

          ExecuteWrapper<0, UDF0_Bound, NBVMX>::Execute( nbvm,
                                                         PairInteractionMask,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         OldForce0,
                                                         NewForce0,
                                                         OldEnergy0,
                                                         NewEnergy0,
                                                         OldVirial0,
                                                         NewVirial0,
                                                         numberOfShells,
                                                         FullTimeStep,
                                                         currentSwitchType );
          XYZ OldForce1;
          XYZ NewForce1;
          XYZ OldVirial1;
          XYZ NewVirial1;
          double OldEnergy1;
          double NewEnergy1;

          ExecuteWrapper<1, UDF1_Bound, NBVMX>::Execute( nbvm,
                                                         PairInteractionMask,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         OldForce1,
                                                         NewForce1,
                                                         OldEnergy1,
                                                         NewEnergy1,
                                                         OldVirial1,
                                                         NewVirial1,
                                                         numberOfShells,
                                                         FullTimeStep,
                                                         currentSwitchType );
          XYZ OldForce2;
          XYZ NewForce2;
          XYZ OldVirial2;
          XYZ NewVirial2;
          double OldEnergy2;
          double NewEnergy2;

          ExecuteWrapper<2, UDF2_Bound, NBVMX>::Execute( nbvm,
                                                         PairInteractionMask,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         OldForce2,
                                                         NewForce2,
                                                         OldEnergy2,
                                                         NewEnergy2,
                                                         OldVirial2,
                                                         NewVirial2,
                                                         numberOfShells,
                                                         FullTimeStep,
                                                         currentSwitchType );



#if BLUEMATTER_STATS0
          if( nbvm.GetForce( NBVMX::SiteB ).mX == 0.0 )
            if( nbvm.GetForce( NBVMX::SiteB ).mY == 0.0 )
              if( nbvm.GetForce( NBVMX::SiteB ).mZ == 0.0 )
                ZeroForceResultOpsCounter.Increment() ;
#endif

          if( aReportForceOnSourceFlag || SourceIsTargetFlag )
            {
              // OldForce is the positive force
              // aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, nbvm.GetForce( NBVMX::SiteB ) );
              // aSourceResultIF.AddForce( SourceAbsSiteId, currentShell, nbvm.GetForce( NBVMX::SiteA ) );
              if ( UDF0_Bound::UDF::Code != -1 )
                {
                  if( UDF0_Bound::UDF::RespaShellsEnabled )
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, -NewForce0 );
                  else
                    aTargetResultIF.AddForceIntoLastRespaBin( TargetAbsSiteId,  -NewForce0 );

                  if( UDF0_Bound::UDF::RespaShellsEnabled )
                    aSourceResultIF.AddForce( SourceAbsSiteId, currentShell,  NewForce0 );
                  else
                    aSourceResultIF.AddForceIntoLastRespaBin( SourceAbsSiteId, NewForce0 );
                }

              if ( UDF1_Bound::UDF::Code != -1 )
                {
                  if( UDF1_Bound::UDF::RespaShellsEnabled )
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, -NewForce1 );
                  else
                    aTargetResultIF.AddForceIntoLastRespaBin( TargetAbsSiteId, -NewForce1 );

                  if( UDF1_Bound::UDF::RespaShellsEnabled )
                    aSourceResultIF.AddForce( SourceAbsSiteId, currentShell,  NewForce1 );
                  else
                    aSourceResultIF.AddForceIntoLastRespaBin( SourceAbsSiteId, NewForce1 );
                }

              if ( UDF2_Bound::UDF::Code != -1 )
                {
                  if( UDF2_Bound::UDF::RespaShellsEnabled )
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, -NewForce2 );
                  else
                    aTargetResultIF.AddForceIntoLastRespaBin( TargetAbsSiteId, -NewForce2 );

                  if( UDF2_Bound::UDF::RespaShellsEnabled )
                    aSourceResultIF.AddForce( SourceAbsSiteId, currentShell,  NewForce2 );
                  else
                    aSourceResultIF.AddForceIntoLastRespaBin( SourceAbsSiteId, NewForce2 );
                }

              if( UDF0_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 0, NewEnergy0 );
              if( UDF1_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 1, NewEnergy1 );
              if( UDF2_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 2, NewEnergy2 );

              if( currentSwitchType == IFPHelper0::SWITCH_ZONE_2)
                {
                  if( UDF0_Bound::UDF::RespaShellsEnabled  && ( UDF0_Bound::UDF::Code != -1 ))
                    {
                    XYZ complementForce0 = OldForce0 - NewForce0;
                    aSourceResultIF.AddForce( SourceAbsSiteId, currentShell+1,  complementForce0 );
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell+1, -complementForce0 );
                    }

                  if( UDF1_Bound::UDF::RespaShellsEnabled  && ( UDF1_Bound::UDF::Code != -1 ))
                    {
                    XYZ complementForce1 = OldForce1 - NewForce1;
                    aSourceResultIF.AddForce( SourceAbsSiteId, currentShell+1,  complementForce1 );
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell+1, -complementForce1 );
                    }

                  if( UDF2_Bound::UDF::RespaShellsEnabled  && ( UDF2_Bound::UDF::Code != -1 ))
                    {
                    XYZ complementForce2 = OldForce2 - NewForce2;
                    aSourceResultIF.AddForce( SourceAbsSiteId, currentShell+1,  complementForce2 );
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell+1, -complementForce2 );
                    }

#if 0
                  if( UDF0_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 0, OldEnergy0 - NewEnergy0 );
                  if( UDF1_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 1, OldEnergy1 - NewEnergy1 );
                  if( UDF2_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 2, OldEnergy2 - NewEnergy2 );
#endif
                }

              #if MSDDEF_DoPressureControl
              {
                // if( UDF0_Bound::UDF::ReportsVirial )
                //   virialSum += nbvm.GetVirial( 0 );
                // if( UDF1_Bound::UDF::ReportsVirial )
                //   virialSum += nbvm.GetVirial( 1 );
                // if( UDF2_Bound::UDF::ReportsVirial )
                //   virialSum += nbvm.GetVirial( 2 );

                if(  UDF0_Bound::UDF::ReportsVirial )
                  {
                    if( UDF0_Bound::UDF::RespaShellsEnabled )
                      aGlobalResultIF.AddVirial( currentShell, NewVirial0 );
                    else
                      aGlobalResultIF.AddVirialLastRespaBin( NewVirial0 );
                  }

                if(  UDF1_Bound::UDF::ReportsVirial )
                  {
                    if( UDF1_Bound::UDF::RespaShellsEnabled )
                      aGlobalResultIF.AddVirial( currentShell, NewVirial1 );
                    else
                      aGlobalResultIF.AddVirialLastRespaBin( NewVirial1 );
                  }

                if(  UDF2_Bound::UDF::ReportsVirial )
                  {
                    if( UDF2_Bound::UDF::RespaShellsEnabled )
                      aGlobalResultIF.AddVirial( currentShell, NewVirial2 );
                    else
                      aGlobalResultIF.AddVirialLastRespaBin( NewVirial2 );
                  }

                if( currentSwitchType == IFPHelper0::SWITCH_ZONE_2 )
                  {
                    if( UDF0_Bound::UDF::ReportsVirial && UDF0_Bound::UDF::RespaShellsEnabled )
                      aGlobalResultIF.AddVirial( currentShell+1, (OldVirial0 - NewVirial0) );

                    if( UDF1_Bound::UDF::ReportsVirial && UDF1_Bound::UDF::RespaShellsEnabled )
                      aGlobalResultIF.AddVirial( currentShell+1, (OldVirial1 - NewVirial1) );

                    if( UDF2_Bound::UDF::ReportsVirial && UDF2_Bound::UDF::RespaShellsEnabled )
                      aGlobalResultIF.AddVirial( currentShell+1, (OldVirial2 - NewVirial2) );
                  }
              }
              #endif
            }
          else
            {
              // ALWAYS apply force on Target
              // aTargetResultIF.AddForce( TargetAbsSiteId, currentShell,   nbvm.GetForce( NBVMX::SiteB ) );
              // aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, -NewForce );
              if ( UDF0_Bound::UDF::Code != -1 )
                {
                  if( UDF0_Bound::UDF::RespaShellsEnabled )
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, -NewForce0 );
                  else
                    aTargetResultIF.AddForceIntoLastRespaBin( TargetAbsSiteId,  -NewForce0 );
                }

              if ( UDF1_Bound::UDF::Code != -1 )
                {
                  if( UDF1_Bound::UDF::RespaShellsEnabled )
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, -NewForce1 );
                  else
                    aTargetResultIF.AddForceIntoLastRespaBin( TargetAbsSiteId, -NewForce1 );
                }

              if ( UDF2_Bound::UDF::Code != -1 )
                {
                  if( UDF2_Bound::UDF::RespaShellsEnabled )
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, -NewForce2 );
                  else
                    aTargetResultIF.AddForceIntoLastRespaBin( TargetAbsSiteId, -NewForce2 );
                }

              // and only half the energy
              if( UDF0_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 0, NewEnergy0 * 0.5);
              if( UDF1_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 1, NewEnergy1 * 0.5);
              if( UDF2_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 2, NewEnergy2 * 0.5);


              if( currentSwitchType == IFPHelper0::SWITCH_ZONE_2 )
                {
                  if( UDF0_Bound::UDF::RespaShellsEnabled  && ( UDF0_Bound::UDF::Code != -1 ))
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell+1, -OldForce0 + NewForce0 );
                  if( UDF1_Bound::UDF::RespaShellsEnabled  && ( UDF1_Bound::UDF::Code != -1 ))
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell+1, -OldForce1 + NewForce1 );
                  if( UDF2_Bound::UDF::RespaShellsEnabled  && ( UDF2_Bound::UDF::Code != -1 ))
                    aTargetResultIF.AddForce( TargetAbsSiteId, currentShell+1, -OldForce2 + NewForce2 );

                  if( UDF0_Bound::UDF::ReportsEnergy )
                      aGlobalResultIF.AddEnergy( 0, (OldEnergy0 - NewEnergy0) * 0.5);
                  if( UDF1_Bound::UDF::ReportsEnergy )
                      aGlobalResultIF.AddEnergy( 1, (OldEnergy1 - NewEnergy1) * 0.5);
                  if( UDF2_Bound::UDF::ReportsEnergy )
                      aGlobalResultIF.AddEnergy( 2, (OldEnergy2 - NewEnergy2) * 0.5);
                }

              #if MSDDEF_DoPressureControl
                {
                  // if( UDF0_Bound::UDF::ReportsVirial )
                  //   EwaldFragmentVirial += nbvm.GetVirial( 0 ) * 0.5;
                  // if( UDF1_Bound::UDF::ReportsVirial )
                  //   EwaldFragmentVirial += nbvm.GetVirial( 1 ) * 0.5;
                  // if( UDF2_Bound::UDF::ReportsVirial )
                  //   EwaldFragmentVirial += nbvm.GetVirial( 2 ) * 0.5;

                  // XYZ virialSum;
                  // virialSum.Zero();

                  // if( UDF0_Bound::UDF::ReportsVirial )
                  //   virialSum += nbvm.GetVirial( 0 ) * 0.5;
                  // if( UDF1_Bound::UDF::ReportsVirial )
                  //   virialSum += nbvm.GetVirial( 1 ) * 0.5;
                  // if( UDF2_Bound::UDF::ReportsVirial )
                  //   virialSum += nbvm.GetVirial( 2 ) * 0.5;

                  if(  UDF0_Bound::UDF::ReportsVirial )
                    {
                      if( UDF0_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, NewVirial0 * 0.5);
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( NewVirial0 * 0.5 );
                    }

                  if(  UDF1_Bound::UDF::ReportsVirial )
                    {
                      if( UDF1_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, NewVirial1 * 0.5 );
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( NewVirial1 * 0.5 );
                    }

                  if(  UDF2_Bound::UDF::ReportsVirial )
                    {
                      if( UDF2_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, NewVirial2 * 0.5);
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( NewVirial2 * 0.5 );
                    }

                  if( currentSwitchType == IFPHelper0::SWITCH_ZONE_2 )
                    {
                      if( UDF0_Bound::UDF::ReportsVirial && UDF0_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell+1, (OldVirial0 - NewVirial0) * 0.5);

                      if( UDF1_Bound::UDF::ReportsVirial && UDF1_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell+1, (OldVirial1 - NewVirial1) * 0.5);

                      if( UDF2_Bound::UDF::ReportsVirial && UDF2_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell+1, (OldVirial2 - NewVirial2) * 0.5);
                    }

                  // aGlobalResultIF.AddVirial( currentShell, virialSum );
                  // if( currentSwitchType == IFPHelper0::SWITCH_ZONE_2 )
                  //   aGlobalResultIF.AddVirial( currentShell+1, (OldVirial*0.5  - virialSum) );
                }
#endif
            }


          // if( ! SourceIsTargetFlag )
          // {
          // aInterFragmentForce += nbvm.GetForce( NBVMX::SiteB );
          // }
          /////////////////////////// NEW FRAGMENT BASED CODE
            #if MSDDEF_DoPressureControl
            {
              if( aReportForceOnSourceFlag )
                {
                  // XYZ Force = nbvm.GetForce( NBVMX::SiteA );
                  if ( UDF0_Bound::UDF::Code != -1 )
                    {
                      if( UDF0_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, PairwiseProduct( NewForce0, Vector ) );
                      else
                        aGlobalResultIF.AddVirialLastRespaBin(  PairwiseProduct( NewForce0, Vector ) );
                    }

                  if ( UDF1_Bound::UDF::Code != -1 )
                    {
                      if( UDF1_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, PairwiseProduct( NewForce1, Vector ) );
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( PairwiseProduct( NewForce1, Vector ) );
                    }

                  if ( UDF2_Bound::UDF::Code != -1 )
                    {
                      if( UDF2_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, PairwiseProduct( NewForce2, Vector ) );
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( PairwiseProduct( NewForce2, Vector ) );
                    }

                  if( currentSwitchType == IFPHelper0::SWITCH_ZONE_2 )
                    {
                      if( UDF0_Bound::UDF::RespaShellsEnabled && ( UDF0_Bound::UDF::Code != -1 ))
                        aGlobalResultIF.AddVirial( currentShell+1, PairwiseProduct( OldForce0-NewForce0, Vector ));

                      if( UDF1_Bound::UDF::RespaShellsEnabled && ( UDF1_Bound::UDF::Code != -1 ))
                        aGlobalResultIF.AddVirial( currentShell+1, PairwiseProduct( OldForce1-NewForce1, Vector ));

                      if( UDF2_Bound::UDF::RespaShellsEnabled && ( UDF2_Bound::UDF::Code != -1 ))
                        aGlobalResultIF.AddVirial( currentShell+1, PairwiseProduct( OldForce2-NewForce2, Vector ));
                    }
                }
              else
                {
                  // XYZ Force = nbvm.GetForce( NBVMX::SiteA );
                  if ( UDF0_Bound::UDF::Code != -1 )
                    {
                      if( UDF0_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, PairwiseProduct( NewForce0, Vector ) * 0.5 );
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( PairwiseProduct( NewForce0, Vector ) * 0.5 );
                    }

                  if ( UDF1_Bound::UDF::Code != -1 )
                    {
                      if( UDF1_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, PairwiseProduct( NewForce1, Vector ) * 0.5 );
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( PairwiseProduct( NewForce1, Vector ) * 0.5 );
                    }


                  if ( UDF2_Bound::UDF::Code != -1 )
                    {
                      if( UDF2_Bound::UDF::RespaShellsEnabled )
                        aGlobalResultIF.AddVirial( currentShell, PairwiseProduct( NewForce2, Vector ) * 0.5 );
                      else
                        aGlobalResultIF.AddVirialLastRespaBin( PairwiseProduct( NewForce2, Vector ) * 0.5 );
                    }
                  // aGlobalResultIF.AddVirial( currentShell, ( PairwiseProduct( NewForce, Vector) * 0.5 ) );

                  if( currentSwitchType == IFPHelper0::SWITCH_ZONE_2 )
                    {
                      // aGlobalResultIF.AddVirial( currentShell+1, ( PairwiseProduct( OldForce-NewForce, Vector) * 0.5 ) );
                      if( UDF0_Bound::UDF::RespaShellsEnabled && ( UDF0_Bound::UDF::Code != -1 ))
                        aGlobalResultIF.AddVirial( currentShell+1, PairwiseProduct( OldForce0-NewForce0, Vector ) * 0.5 );

                      if( UDF1_Bound::UDF::RespaShellsEnabled && ( UDF1_Bound::UDF::Code != -1 ))
                        aGlobalResultIF.AddVirial( currentShell+1, PairwiseProduct( OldForce1-NewForce1, Vector ) * 0.5 );

                      if( UDF2_Bound::UDF::RespaShellsEnabled && ( UDF2_Bound::UDF::Code != -1 ))
                        aGlobalResultIF.AddVirial( currentShell+1, PairwiseProduct( OldForce2-NewForce2, Vector ) * 0.5 );
                    }
                }
            }
          #endif

          }
        }
      /////////////////  NEW FRAGMENT BASED CODE /////////////////////////////////

      // if( aReportForceOnSourceFlag )
      //  {
      //  aVirial = NonbondVirial;
      //  }
      //  else
      //  {
      //  aVirial = 0.5 * NonbondVirial;
      //  }

      //  aVirial.mX += EwaldFragmentVirial /3.;
      //  aVirial.mY += EwaldFragmentVirial /3.;
      //  aVirial.mZ += EwaldFragmentVirial /3.;

      /////////////////////////////////////////////////////////////////////////////

      /**************************** OLD MOLECULAR VIRIAL CODE ********************/
      ////////////////double FragmentVirial;
      //////////////if( aReportForceOnSourceFlag )
      ////////////{
      ///////////////////////FragmentVirial = aInterFragmentForce.DotProduct( SourceTargetImageCOMVector ) + EwaldFragmentVirial;
      ////////           }
      ///////else
      //////////// {
      /////////// 1/2 for interfragment virial since count it twice for each pair
      /////////// FragmentVirial = 0.5 * aInterFragmentForce.DotProduct( SourceTargetImageCOMVector ) + EwaldFragmentVirial;
      /////////////}

      // Copy scalar values into the vector virial for now
      ///////////////////////////aVirial.mY = aVirial.mZ = aVirial.mX = FragmentVirial;
      /************************************************************************************/
      // The factors of '0.5' in the 2 calls below are suspicious (we think they may be supposed to be -1.0 and 1.0)
      // Adding to the Center Of Mass Force
      // THE CENTER OF MASS FORCE IS NOW CALCULATED BY THE DIRECT
      // aTargetResultIF.AddCenterOfMassForce( -1.0 * aInterFragmentForce );
      // aTargetResultIF.AddCenterOfMassForce( aInterFragmentForce );

      // if( aReportForceOnSourceFlag )
      //   {
      //   // aSourceResultIF.AddCenterOfMassForce( 1.0 * aInterFragmentForce );
      //   aSourceResultIF.AddCenterOfMassForce( -1.0 * aInterFragmentForce );
      //   }

      // BegLogLine(PKFXLOG_IPARTPAIR)
      //   << "IrreducibleFragmentPair(): FINISHED "
      //   << " IrreducibleFragmentId (targ/src) "
      //   << TargetIrreducibleFragmentId
      //   << " / "
      //   << SourceIrreducibleFragmentId
      //   << " VirialForce "
      //   << aInterFragmentForce
      //   << " NonbondVirial=" << NonbondVirial
      //   << " EwaldFragmentVirial=" << EwaldFragmentVirial
      //   << " aVirial=" << aVirial
      //   << EndLogLine;
      /// }
      /// #endif
    }

};

#endif
