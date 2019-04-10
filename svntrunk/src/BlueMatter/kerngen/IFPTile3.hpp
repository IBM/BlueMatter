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

#ifndef __IFP_TILE3__HPP__
#define __IFP_TILE3__HPP__

#include <BlueMatter/IFPHelper.hpp>

template<class NsqUdfGroup, class SwitchFunctionEnergyAdapter, class SwitchFunctionForceAdapter, int SHELL_COUNT, int CONTROL_FLAGS>
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
        // Execute with no exclusions
        static inline void ExecuteN( NBVM_T&         aNbvm,
                                    int&            aSourceId,
                                    int&            aTargetId,
                                    XYZ&            aTotalForce,
                                    double&         aNewEnergy,
                                    XYZ&            aTotalVirial
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
                        }

                    aTotalForce += aNbvm.GetForce( NBVM_T::SiteA );
                    aNewEnergy = aNbvm.GetEnergy( ORDINAL );

                    if( UDF_Bound::UDF::ReportsVirial )
                        aTotalVirial += aNbvm.GetVirial( ORDINAL );
        }
        // Execute with the possibility of exclusions
        static inline void Execute( NBVM_T&         aNbvm,
                                    unsigned int&   aPairInteractionMask,
                                    int&            aSourceId,
                                    int&            aTargetId,
                                    XYZ&            aTotalForce,
                                    double&         aNewEnergy,
                                    XYZ&            aTotalVirial
                                    )
        {
            if ( (aPairInteractionMask >> ORDINAL)  & 0x1 )
                {
                   ExecuteN(aNbvm,aSourceId,aTargetId,aTotalForce,aNewEnergy,aTotalVirial) ;
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
    ExecuteX( int           aSimTick,
             GlobalInputIF &aGlobalInputParametersIF,
             TargetGroupIF aTargetFragmentIF,
             SourceGroupIF aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int     aReportForceOnSourceFlag,
             const void  * aPersistentStatePtr)
    {

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

          // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
          XYZ SiteImageB;
          NearestImageInPeriodicVolume( SiteA, *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );

          // The following is probably a very expensive way to find out the AbsSiteId's
          // - these could be carried in with the Src/TrgFragmentIF
          int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
          int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );


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


          XYZ TotalForce;
          TotalForce.Zero();
          XYZ TotalVirial;
          TotalVirial.Zero();

          double NewEnergy0;
          ExecuteWrapper<0, UDF0_Bound, NBVMX>::Execute( nbvm,
                                                         PairInteractionMask,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         TotalForce,
                                                         NewEnergy0,
                                                         TotalVirial
                                                         );
          double NewEnergy1;
          ExecuteWrapper<1, UDF1_Bound, NBVMX>::Execute( nbvm,
                                                         PairInteractionMask,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         TotalForce,
                                                         NewEnergy1,
                                                         TotalVirial
                                                         );

          double NewEnergy2;
          ExecuteWrapper<2, UDF2_Bound, NBVMX>::Execute( nbvm,
                                                         PairInteractionMask,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         TotalForce,
                                                         NewEnergy2,
                                                         TotalVirial
                                                         );

          if( aReportForceOnSourceFlag || SourceIsTargetFlag )
            {
                // OldForce is the positive force
                // aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, nbvm.GetForce( NBVMX::SiteB ) );
                // aSourceResultIF.AddForce( SourceAbsSiteId, 0, nbvm.GetForce( NBVMX::SiteA ) );
                aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );
                aSourceResultIF.AddForce( SourceAbsSiteId, 0,  TotalForce );


                if( UDF0_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 0, NewEnergy0 );
                if( UDF1_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 1, NewEnergy1 );
                if( UDF2_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 2, NewEnergy2 );

              #if MSDDEF_DoPressureControl
              {
                  aGlobalResultIF.AddVirial( 0, TotalVirial );
              }
              #endif
            }
          else
            {
              // ALWAYS apply force on Target
              // aTargetResultIF.AddForce( TargetAbsSiteId, 0,   nbvm.GetForce( NBVMX::SiteB ) );
              // aTargetResultIF.AddForce( TargetAbsSiteId, 0, -NewForce );
                aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );

              // and only half the energy
              if( UDF0_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 0, NewEnergy0 * 0.5);
              if( UDF1_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 1, NewEnergy1 * 0.5);
              if( UDF2_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 2, NewEnergy2 * 0.5);

              #if MSDDEF_DoPressureControl
              {
                aGlobalResultIF.AddVirial( 0, TotalVirial * 0.5);
              }
              #endif
            }


            #if MSDDEF_DoPressureControl
            {
              if( aReportForceOnSourceFlag )
                {
                    // XYZ Force = nbvm.GetForce( NBVMX::SiteA );
                    aGlobalResultIF.AddVirial( 0, PairwiseProduct( TotalForce, Vector ) );
                }
              else
                {
                  // XYZ Force = nbvm.GetForce( NBVMX::SiteA );
                    aGlobalResultIF.AddVirial( 0, PairwiseProduct( TotalForce, Vector ) * 0.5 );
                }
            }
          #endif

          }
        }
    }

    // 'Execute' with no exclusions
    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    Block11ExecuteN( XYZ& SiteA,
              TargetGroupIF aTargetFragmentIF,
              int TargetIrreducibleFragmentId,
              int SourceAbsSiteId,
              int TargetFragmentSiteIndex,
              NBVMX nbvm,
                     const int     aReportForceOnSourceFlag,
              FragmentResultIF     &aTargetResultIF,
              FragmentResultIF     &aSourceResultIF,
              GlobalResultIF       &aGlobalResultIF
              )
    {
          // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
          XYZ SiteImageB;
          NearestImageInPeriodicVolume( SiteA, *aTargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );

          // The following is probably a very expensive way to find out the AbsSiteId's
          // - these could be carried in with the Src/TrgFragmentIF
          int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );


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
            << " TargetAbsSiteId " <<  TargetAbsSiteId
            << EndLogLine;


          XYZ TotalForce;
          TotalForce.Zero();
          XYZ TotalVirial;
          TotalVirial.Zero();

          double NewEnergy0;
          ExecuteWrapper<0, UDF0_Bound, NBVMX>::ExecuteN( nbvm,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         TotalForce,
                                                         NewEnergy0,
                                                         TotalVirial
                                                         );
          double NewEnergy1;
          ExecuteWrapper<1, UDF1_Bound, NBVMX>::ExecuteN( nbvm,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         TotalForce,
                                                         NewEnergy1,
                                                         TotalVirial
                                                         );

          double NewEnergy2;
          ExecuteWrapper<2, UDF2_Bound, NBVMX>::ExecuteN( nbvm,
                                                         SourceAbsSiteId,
                                                         TargetAbsSiteId,
                                                         TotalForce,
                                                         NewEnergy2,
                                                         TotalVirial
                                                         );

          if( aReportForceOnSourceFlag )
            {
                // OldForce is the positive force
                // aTargetResultIF.AddForce( TargetAbsSiteId, currentShell, nbvm.GetForce( NBVMX::SiteB ) );
                // aSourceResultIF.AddForce( SourceAbsSiteId, 0, nbvm.GetForce( NBVMX::SiteA ) );
                aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );
                aSourceResultIF.AddForce( SourceAbsSiteId, 0,  TotalForce );


                if( UDF0_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 0, NewEnergy0 );
                if( UDF1_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 1, NewEnergy1 );
                if( UDF2_Bound::UDF::ReportsEnergy )
                    aGlobalResultIF.AddEnergy( 2, NewEnergy2 );

              #if MSDDEF_DoPressureControl
              {
                  aGlobalResultIF.AddVirial( 0, TotalVirial );
              }
              #endif
            }
          else
            {
              // ALWAYS apply force on Target
              // aTargetResultIF.AddForce( TargetAbsSiteId, 0,   nbvm.GetForce( NBVMX::SiteB ) );
              // aTargetResultIF.AddForce( TargetAbsSiteId, 0, -NewForce );
                aTargetResultIF.AddForce( TargetAbsSiteId, 0, -TotalForce );

              // and only half the energy
              if( UDF0_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 0, NewEnergy0 * 0.5);
              if( UDF1_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 1, NewEnergy1 * 0.5);
              if( UDF2_Bound::UDF::ReportsEnergy )
                aGlobalResultIF.AddEnergy( 2, NewEnergy2 * 0.5);

              #if MSDDEF_DoPressureControl
              {
                aGlobalResultIF.AddVirial( 0, TotalVirial * 0.5);
              }
              #endif
            }


            #if MSDDEF_DoPressureControl
            {
              if( aReportForceOnSourceFlag )
                {
                    // XYZ Force = nbvm.GetForce( NBVMX::SiteA );
                    aGlobalResultIF.AddVirial( 0, PairwiseProduct( TotalForce, Vector ) );
                }
              else
                {
                  // XYZ Force = nbvm.GetForce( NBVMX::SiteA );
                    aGlobalResultIF.AddVirial( 0, PairwiseProduct( TotalForce, Vector ) * 0.5 );
                }
            }
          #endif

    }

    // 'Execute' with no exclusions
    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    ExecuteN( int           aSimTick,
              GlobalInputIF &aGlobalInputParametersIF,
              TargetGroupIF aTargetFragmentIF,
              SourceGroupIF aSourceFragmentIF,
              FragmentResultIF     &aTargetResultIF,
              FragmentResultIF     &aSourceResultIF,
              GlobalResultIF       &aGlobalResultIF,
              const int     aReportForceOnSourceFlag)
    {

      int TargetIrreducibleFragmentId = aTargetFragmentIF.GetIrreducibleFragmentId();
      int SourceIrreducibleFragmentId = aSourceFragmentIF.GetIrreducibleFragmentId();

      // Create a flag to be used to recognize when source and target fragments are the
      // same and thus we can do 1/2 the force term computations by applying the results
      // of each computation to both sits sites.

      // The fragments should be different; within a fragment we should have an exclusion list
      assert ( SourceIrreducibleFragmentId != TargetIrreducibleFragmentId ) ;

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "IrreducibleFragmentPair(): STARTING "
        << " IrreducibleFragmentId (targ/src) " << TargetIrreducibleFragmentId
        << " / " << SourceIrreducibleFragmentId
        << EndLogLine;

      int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
      int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

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


      NBVMX nbvmU;
      NBVMX nbvmV;
      NBVMX nbvmW;
      nbvmU.SetSimTick( aSimTick );
      nbvmV.SetSimTick( aSimTick );
      nbvmW.SetSimTick( aSimTick );
      // Make sure that the force is put on the last respa level
      nbvmU.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );
      nbvmV.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );
      nbvmW.SetSwitchFunctionRadii( RTG.mSwitchFunctionRadii[ 0 ] );


      for(int SourceFragmentSiteIndex = 0;
          SourceFragmentSiteIndex < SourceFragmentSiteCount;
          SourceFragmentSiteIndex++ )
        {
        BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

        XYZ SiteA = *aSourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex );
        int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );

        // Scan the Target Fragment since that should be more local.
        int TargetFragmentSiteIndex ;
        for(TargetFragmentSiteIndex = 0 ;
            TargetFragmentSiteIndex <= TargetFragmentSiteCount-3;
            TargetFragmentSiteIndex += 3 )
          {
          BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

          Block11ExecuteN<GlobalInputIF, SourceGroupIF, TargetGroupIF, FragmentResultIF, GlobalResultIF>(
           SiteA,
           aTargetFragmentIF,
           TargetIrreducibleFragmentId,
           TargetFragmentSiteIndex,
           SourceAbsSiteId,
           nbvmU,
           aReportForceOnSourceFlag,
           aTargetResultIF,
           aSourceResultIF,
           aGlobalResultIF
          ) ;
          Block11ExecuteN<GlobalInputIF, SourceGroupIF, TargetGroupIF, FragmentResultIF, GlobalResultIF>(
           SiteA,
           aTargetFragmentIF,
           TargetIrreducibleFragmentId,
           TargetFragmentSiteIndex+1,
           SourceAbsSiteId,
           nbvmV,
           aReportForceOnSourceFlag,
           aTargetResultIF,
           aSourceResultIF,
           aGlobalResultIF
          ) ;
          Block11ExecuteN<GlobalInputIF, SourceGroupIF, TargetGroupIF, FragmentResultIF, GlobalResultIF>(
           SiteA,
           aTargetFragmentIF,
           TargetIrreducibleFragmentId,
           TargetFragmentSiteIndex+2,
           SourceAbsSiteId,
           nbvmW,
           aReportForceOnSourceFlag,
           aTargetResultIF,
           aSourceResultIF,
           aGlobalResultIF
          ) ;
          } /* endfor */
        for( ;
            TargetFragmentSiteIndex < TargetFragmentSiteCount;
            TargetFragmentSiteIndex++ )
          {
          BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

          Block11ExecuteN<GlobalInputIF, SourceGroupIF, TargetGroupIF, FragmentResultIF, GlobalResultIF>(
           SiteA,
           aTargetFragmentIF,
           TargetIrreducibleFragmentId,
           TargetFragmentSiteIndex,
           SourceAbsSiteId,
           nbvmU,
           aReportForceOnSourceFlag,
           aTargetResultIF,
           aSourceResultIF,
           aGlobalResultIF
          ) ;
          } /* endfor */
        } /* endfor */
      } /* ExecuteN */

    template<class GlobalInputIF, class SourceGroupIF, class TargetGroupIF, class FragmentResultIF, class GlobalResultIF>
    static
    inline
    void
    Execute( int                  aSimTick,
             GlobalInputIF        &aGlobalInputParametersIF,
             TargetGroupIF        aTargetFragmentIF,
             SourceGroupIF        aSourceFragmentIF,
             FragmentResultIF     &aTargetResultIF,
             FragmentResultIF     &aSourceResultIF,
             GlobalResultIF       &aGlobalResultIF,
             const int            aReportForceOnSourceFlag,
             const void  *        aPersistentStatePtr
             )
    {
       if (aPersistentStatePtr == NULL)
       {
	    ExecuteN <GlobalInputIF,  SourceGroupIF, TargetGroupIF,  FragmentResultIF, GlobalResultIF>
                  (aSimTick,
                   aGlobalInputParametersIF,
                   aTargetFragmentIF,
                   aSourceFragmentIF,
                   aTargetResultIF,
                   aSourceResultIF,
                   aGlobalResultIF,
                   aReportForceOnSourceFlag
          ) ;
       }
       else
       { 
	   ExecuteX <GlobalInputIF,  SourceGroupIF, TargetGroupIF,  FragmentResultIF, GlobalResultIF>
                  (aSimTick,
                   aGlobalInputParametersIF,
                   aTargetFragmentIF,
                   aSourceFragmentIF,
                   aTargetResultIF,
                   aSourceResultIF,
                   aGlobalResultIF,
                   aReportForceOnSourceFlag,
                   aPersistentStatePtr
          ) ;
       } /* endif */
    }
};

#endif
