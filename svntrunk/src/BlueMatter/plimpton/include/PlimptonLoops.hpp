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
 




THIS CODE IS DEAD : replaced by NSQ.hpp







/***************************************************************************
 * Project:         BlueMatter
 *
 * Module:          NonBondedEngine
 *
 * Purpose:         Manage loop driven NonBonded Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         001001 BGF Created.
 *
 ***************************************************************************/

#ifndef _PLIMPTONLOOPS_HPP_
#define _PLIMPTONLOOPS_HPP_

#ifndef PKFXLOG_PLIMPTONLOOPS
#define PKFXLOG_PLIMPTONLOOPS 0
#endif

#include <BlueMatter/NBVM_IF.hpp>

#include <BlueMatter/WaterFragmentPair.hpp>
#include <BlueMatter/IrreducibleFragmentPair.hpp>

#include <BlueMatter/UDF_Binding.hpp>

////////////#include <BlueMatter/RTPFile.hpp>

#include <BlueMatter/ExternalDatagram.hpp>

#include <BlueMatter/p3me.hpp>

//******************************************************

#ifndef PKFXLOG_FORCE_TERMS
#define PKFXLOG_FORCE_TERMS (0)
#endif


//****************************************************************

template<class DynVarMgrIF, class NBMV>
class PlimptonNonBondedEngine
  {
  public:
    typedef void *VoidPtr;

    //********* FROM NB ****************/

    class ClientState
      {
      public:
        DynVarMgrIF *mDynVarMgrIF;
        int          mSimTick;
        unsigned     mSimTicksPerTimeStep;
        int          mIntegratorLevel;
        VoidPtr     *mPersistantStatePointerArray;
      };


    // Every client port must allocated one of these structures.
    // It is returned as a handle by either Connect or send.
    // It is passed to calls to wait.
    // Calling wait shows that DynamaicVariables are ready to be updated.
    class Client
      {
      public:

        ClientState mClientState;

        void
        Connect( //NEED: think about whether to allow the volume or machine grid to be passed in here.
                 Grid2D                 aGrid2D,
                 DynVarMgrIF          * aDynVarMgrIF,   //Type for this should come in as a template interface
                 unsigned               aSimTick,
                 unsigned               aSimTicksPerTimeStep, // assumes Plimptonize only happens at OTS
                 int                    aIntegratorLevel )
          {
          mClientState.mDynVarMgrIF              = aDynVarMgrIF;
          mClientState.mSimTick                  = aSimTick;
          mClientState.mSimTicksPerTimeStep = aSimTicksPerTimeStep;
          mClientState.mIntegratorLevel          = aIntegratorLevel;

          int NumberOfIrreducibleParitionPairCombinations =
                         aDynVarMgrIF->GetIrreducibleFragmentCount()
                         *
                         aDynVarMgrIF->GetIrreducibleFragmentCount();

          mClientState.mPersistantStatePointerArray =
            new VoidPtr[  NumberOfIrreducibleParitionPairCombinations  ];

          assert( mClientState.mPersistantStatePointerArray != NULL );

          for(int i = 0; i < NumberOfIrreducibleParitionPairCombinations; i++ )
            mClientState.mPersistantStatePointerArray[ i ] = NULL;

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


#ifdef CHECK_COMMASSFORCE
            XYZ CheckCOMMassForce;
#endif



            FragmentPartialResultsIF(        int          aSimTick,
                                           int          aIntegratorLevel,
                                           DynVarMgrIF *aDynVarMgrIF ,
                                           int          aFragmentOrdinal )
              {
              mSimTick          = aSimTick;
              mIntegratorLevel  = aIntegratorLevel;
              mDynVarMgrIF      = aDynVarMgrIF;
              mFragmentOrdinal = aFragmentOrdinal;
#ifdef CHECK_COMMASSFORCE
              CheckCOMMassForce.Zero();
#endif
              }

            inline
            void
            AddForce( int aIndex, XYZ aForce )
              {
              int base   = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
              XYZ NewForce = mDynVarMgrIF->GetForce( base + aIndex, mSimTick, mIntegratorLevel  ) + aForce;
              mDynVarMgrIF->SetForce( base + aIndex, mSimTick, mIntegratorLevel, NewForce );

#ifdef CHECK_COMMASSFORCE
              CheckCOMMassForce += aForce;
#endif
              }

            inline
            XYZ
            GetForce( int aIndex )
              {
              int base   = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
              XYZ Force = mDynVarMgrIF->GetForce( base + aIndex, mSimTick, mIntegratorLevel  );
              return Force;
              }

            inline
            void
            AddCenterOfMassForce( XYZ aForce)
              {
              mDynVarMgrIF->AddCenterOfMassForce( mFragmentOrdinal, aForce );
              }


          };

        class GlobalPartialResultsIF
          {
          public:

            //NOTE:: Most of the below fields are not used.
            double mTotalLennardJonesEnergy;
            double mTotalLennardJonesDHDLamda;
            XYZ    mTotalLennardJonesVirial;
            double mTotalChargeEnergy;
            double mTotalEwaldRealSpaceEnergy;
            double mTotalEwaldCorrectionEnergy;
            double mTotalChargeDHDLamda;
            XYZ    mTotalChargeVirial;

            double mEnergyByUdfCode[ UDF_Binding::UDF_MAX_CODE + 1 ];

            void
            Zero()
              {
              mTotalLennardJonesEnergy    = 0.0 ;
              mTotalLennardJonesDHDLamda  = 0.0 ;
              mTotalLennardJonesVirial.Zero();
              mTotalChargeEnergy          = 0.0 ;
              mTotalEwaldRealSpaceEnergy  = 0.0 ;
              mTotalEwaldCorrectionEnergy  = 0.0 ;
              mTotalChargeDHDLamda        = 0.0 ;
              mTotalChargeVirial.Zero();

              for (int udfCode = 0; udfCode <= UDF_Binding::UDF_MAX_CODE; udfCode++)
                {
                  mEnergyByUdfCode[ udfCode ] = 0.0;
                }
              }

            inline
            void
            AddEnergy( int aUDF_Code, double aEnergy )
              {
                assert(aUDF_Code > 0 && aUDF_Code <= UDF_Binding::UDF_MAX_CODE);
                mEnergyByUdfCode[ aUDF_Code ] += aEnergy;
#if 0
              switch( aUDF_Code )
                {
                case UDF_Binding::StdChargeForce_Code :
                case UDF_Binding::NSQCoulomb_Code:
                  {
                  mTotalChargeEnergy += aEnergy;
                  break;
                  }
                case UDF_Binding::LennardJonesForce_Code :
                case UDF_Binding::NSQLennardJones_Code:
                  {
                  mTotalLennardJonesEnergy += aEnergy;
                  break;
                  }
                case UDF_Binding::NSQEwaldRealSpace_Code:
                  {
                    mTotalEwaldRealSpaceEnergy += aEnergy;
                    break;
                  }
                case UDF_Binding::NSQEwaldCorrection_Code:
                  {
                    mTotalEwaldCorrectionEnergy += aEnergy;
                    break;
                  }
                default :
                  assert(0);
                };
#endif
              }
          };


        void
        Send( int aSimTick )
          {
          BegLogLine(1)
            << "Send(): "
            << " SimTick "
            << aSimTick
            << EndLogLine;


          GlobalPartialResultsIF GlobalPartialResult;

          GlobalPartialResult.Zero();

          for( int TargetI = 0; TargetI < mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount(); TargetI++ )
            {

            IrreducibleFragmentOperandIF TargetFragmentOperandIF( aSimTick,
                                                           mClientState.mIntegratorLevel,
                                                           mClientState.mDynVarMgrIF,
                                                           TargetI );

            FragmentPartialResultsIF TargetPartialResultsIF( aSimTick,
                                                           mClientState.mIntegratorLevel,
                                                           mClientState.mDynVarMgrIF,
                                                           TargetI );

            for( int SourceI = 0; SourceI < mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount(); SourceI++ )
              {
              IrreducibleFragmentOperandIF SourceFragmentOperandIF( aSimTick,
                                                             mClientState.mIntegratorLevel,
                                                             mClientState.mDynVarMgrIF,
                                                             SourceI );

              // #define WATERPAIRTEST
#ifdef WATERPAIRTEST
              // This is the function we need... but how to get it in a generic way.
              // Should be possible without specificaly saying 'water'... but how?

              if( MSD_IF::GetIrreducibleFragmentWaterStatus( TargetI ) == MSD::WATER
                  && MSD_IF::GetIrreducibleFragmentWaterStatus( SourceI ) == MSD::WATER )
                  {
                      WaterFragmentPair( aSimTick,
                                         SourceFragmentOperandIF,
                                         TargetFragmentOperandIF,
                                         TargetPartialResultsIF,
                                         GlobalPartialResult
                                         );
                  }
              else
                  {

#endif
                      // do the standard, site by site fragment pair method.

                      XYZ LocalVirialContribution;
                      XYZ InterFragmentForce;

                      IrreducibleFragmentPair( aSimTick,
                                               SourceFragmentOperandIF,
                                               TargetFragmentOperandIF,
                                               TargetPartialResultsIF,
                                               GlobalPartialResult,
                                               LocalVirialContribution,
                                               InterFragmentForce,
                                               &( mClientState.mPersistantStatePointerArray[ TargetI * mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount() + SourceI ] ) );


#ifdef WATERPAIRTEST
                }
#endif

#ifdef CHECK_COMMASSFORCE
           cerr << " CheckCOMMassForce " << TargetPartialResultsIF.CheckCOMMassForce
                << " Recored CheckCOMMassForce " << TargetPartialResultsIF.mDynVarMgrIF->GetCenterOfMassForce( TargetI )
                << " Difference " << TargetPartialResultsIF.CheckCOMMassForce  - TargetPartialResultsIF.mDynVarMgrIF->GetCenterOfMassForce( TargetI )
                << endl;
#endif
              }

            }
          unsigned OTS = aSimTick;

          // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
          unsigned TimeStep = aSimTick / mClientState.mSimTicksPerTimeStep;

          // Check if this particular SimTick is the new TimeStep.
          if( TimeStep * mClientState.mSimTicksPerTimeStep == aSimTick )
            {
            BegLogLine(1)
              << "PlimptonLoops::Send() "
              << " Emitting RDG Energy packets "
              << " SimTick "  << aSimTick
              << " TimeStep " << TimeStep
              << EndLogLine;

#if 0
            ED_Emit_UDFd1( TimeStep, UDF_Binding::LennardJonesForce_Code, 1u, GlobalPartialResult.mTotalLennardJonesEnergy );
            switch( MSD_IF::Access().LongRangeForceMethod )
              {
              case PreMSD::STDCOULOMB:
              case PreMSD::FRC:
                  //              cerr << "ChargeEnergy: " << GlobalPartialResult.mTotalChargeEnergy << endl;
                ED_Emit_UDFd1( TimeStep, UDF_Binding::StdChargeForce_Code,    1u, GlobalPartialResult.mTotalChargeEnergy       );
                break;

              case PreMSD::EWALD:
              case PreMSD::P3ME:
                  //cerr << "RealSpaceEnergy: " << GlobalPartialResult.mTotalEwaldRealSpaceEnergy << endl;
                  ED_Emit_UDFd1( TimeStep, UDF_Binding::EwaldRealSpaceForce_Code,    1u, GlobalPartialResult.mTotalEwaldRealSpaceEnergy );
                  ED_Emit_UDFd1( TimeStep, UDF_Binding::EwaldCorrectionForce_Code,    1u, GlobalPartialResult.mTotalEwaldCorrectionEnergy );
                break;

              default:
                assert(0);
              }

#endif
            for(int udfCode=0; udfCode <= UDF_Binding::UDF_MAX_CODE; udfCode++)
              {
                if( MSD_IF::IsUDFInUse( MSD_IF::NSQDRIVER, udfCode ) )
                  {
                    ED_Emit_UDFd1( TimeStep, udfCode, 1u, GlobalPartialResult.mEnergyByUdfCode[ udfCode ]);
                  }
              }


            if( MSD_IF::Access().DoFreeEnergy )
              {
              double TotalDHDLamda =   GlobalPartialResult.mTotalLennardJonesDHDLamda
                                     + GlobalPartialResult.mTotalChargeDHDLamda;
              ED_Emit_EnergyFree( TimeStep, TotalDHDLamda );
              }

            if( MSD_IF::Access().DoPressureControl )
              {
              XYZ TotalVirial =   GlobalPartialResult.mTotalLennardJonesVirial
                                + GlobalPartialResult.mTotalChargeVirial;
              // ControlPressure packet now has everything in it...
              ///////////////////////  ED_Emit_ControlPressure( TimeStep, TotalVirial );
              }
            }
          }


        void
        Wait( int aSimTick )
          {
          // Loops better be done, no?
          return ;
          }

      };
  };


template<class DynVarMgrIF, class NBMV>
class PlimptonAnchor
  {

    /*
     * The anchor IS the Server.
     */
  private:

    class PlimptonNonBondedEngine<DynVarMgrIF,NBMV> mServer;

    /*
     *
     */
  public:

    void
    Init()
      {
      //NEED: THIS SHOULD BE SOMEWHERE ELSE

      PlimptonNonBondedEngine<DynVarMgrIF,NBMV> mServer;
#if 0
      BegLogLine(1)
        << "PlimptonNonBondedEngine<>:: "
        << " sizeof( connect pkt hdr ) "
        << sizeof( PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::ConnectActorPacket)
        << " sizeof( operand pkt hdr ) "
        << sizeof( PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::OperandActorPacket::Header )
        << " sizeof( result  pkt hdr) "
        << sizeof( PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::ResultsActorPacket::Header )
        << EndLogLine;
#endif
      BegLogLine(1)
        << "PlimptonAnchor::Init(): "
        << " ServerPtr "
/////////        << (void *) ServerPtr
        << EndLogLine;

#if 0
      //NEED: thread create to learn about ActorMulticast().
      for( int p = 0; p < Platform::Topology::GetAddressSpaceCount(); p++ )
        {
        ThreadCreate( p,
                      PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Server::WorkThread,
                      sizeof( ServerPtr ),
                      & ServerPtr );
        }
#endif
      }


    // Create a compiler recongnized instance of the Client Interface - should be zero byts.
    typedef class PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Client ClientIF;

    ClientIF &
    GetClientIF()
      {
      PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Client * cr
        = new  PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Client();

//////////      cr->mClientState.mServerRef = & mServer;

      return( * cr  );
      }
  };



#endif
