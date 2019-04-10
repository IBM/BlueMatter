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
 * Module:          NonBondedEngine
 *
 * Purpose:         Manage loop driven NonBonded Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         001001 BGF Created.
 *
 ***************************************************************************/

#ifndef _LEKNERENGINE_HPP_
#define _LEKNERENGINE_HPP_

#ifndef PKFXLOG_LEKNERENGINE
#define PKFXLOG_LEKNERENGINE 0
#endif

#include <BlueMatter/UDF_Binding.hpp>

#include <BlueMatter/ExternalDatagram.hpp>

//******************************************************

#ifndef PKFXLOG_FORCE_TERMS
#define PKFXLOG_FORCE_TERMS (0)
#endif


//****************************************************************

template<class DynVarMgrIF>
class LeknerEngine
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
      };


    // Every client port must allocated one of these structures.
    // It is returned as a handle by either Connect or send.
    // It is passed to calls to wait.
    // Calling wait shows that DynamaicVariables are ready to be updated.
    class Client
      {
      public:

        ClientState mClientState;

        double LeknerSelfEnergyInit; // keeps 1/2 \tilde{\fi} \Sum q_i^2 which then is scaled if volume changes

        inline
        void
        LeknerInit()
          {
              NBVMX aNBVMX;
              UDF_HelperLekner<NBVMX>::OneTimeInit( aNBVMX );
              UDF_HelperLekner<NBVMX>::SelfEnergy( LeknerSelfEnergyInit );

              double sumcharge2 = 0;
              for ( int AbsSiteId = 0;
                    AbsSiteId < mClientState.mDynVarMgrIF->GetNumberOfSites();
                    AbsSiteId++ )
              {
                  double cite_charge = MSD_IF::GetSiteInfo( AbsSiteId ).charge;
                  sumcharge2 += cite_charge * cite_charge;
              }

              LeknerSelfEnergyInit *= 0.5 * sumcharge2;

              ////////// init self energy goes here !!!! ######### $$$$$$$$$$ !!!!!!
          }

        inline
        void
        Connect( //NEED: think about whether to allow the volume or machine grid to be passed in here.
                 DynVarMgrIF          * aDynVarMgrIF,   //Type for this should come in as a template interface
                 unsigned               aSimTick,
                 unsigned               aSimTicksPerTimeStep, // assumes Plimptonize only happens at OTS
                 int                    aIntegratorLevel )
          {
          mClientState.mDynVarMgrIF              = aDynVarMgrIF;
          mClientState.mSimTick                  = aSimTick;
          mClientState.mSimTicksPerTimeStep = aSimTicksPerTimeStep;
          mClientState.mIntegratorLevel          = aIntegratorLevel;

          // calculate self-energy
          LeknerInit();
          }

        // The 'Send' method here will only be called when

        class IrreducibleFragmentOperandIF
          {
          public:

            int          mSimTick;
            int          mFragmentOrdinal;
            DynVarMgrIF *mDynVarMgrIF;
            int          mIntegratorLevel;

            inline
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

            inline FragmentPartialResultsIF(      int          aSimTick,
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

            inline
            void
            AddForce( int aIndex, XYZ& aForce )
              {
              int base   = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
              XYZ NewForce = mDynVarMgrIF->GetForce( base + aIndex, mSimTick, mIntegratorLevel  ) + aForce;
              mDynVarMgrIF->SetForce( base + aIndex, mSimTick, mIntegratorLevel, NewForce );

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

            double mTotalLeknerSelfEnergy;

            inline
            void
            Zero()
              {
              mTotalLeknerSelfEnergy     = 0.0 ;
              }

            inline
            void
            AddEnergy( int aUDF_Code, double aEnergy )
              {
              switch( aUDF_Code )
                {
                case UDF_Binding::LeknerSelfEnergy_Code :
                  {
                    mTotalLeknerSelfEnergy += aEnergy;
                    break;
                  }
                default :
                  assert(0);
                };
              }
          };


       void
       inline
       Send( int aSimTick, int aRespaLevel )
          {
          BegLogLine(1)
            << "Send(): "
            << " SimTick "
            << aSimTick
            << EndLogLine;

          mClientState.mIntegratorLevel = aRespaLevel;
          mClientState.mSimTicksPerTimeStep = MSD_IF::GetSimTicksAtRespaLevel()[ aRespaLevel ];

          GlobalPartialResultsIF GlobalPartialResult;
          GlobalPartialResult.Zero();

          //
          // Self Energy part
          //

          double LeknerSelfEnergy = LeknerSelfEnergyInit * RTG.mBoxSizeAbsoluteRatio.mX;

          //* >>>>>>> 1.3.2.3
          GlobalPartialResult.AddEnergy( UDF_Binding::LeknerSelfEnergy_Code, LeknerSelfEnergy );
//           RTG.mGlobalVirial.mX += LeknerSelfEnergy;
//           RTG.mGlobalVirial.mY += LeknerSelfEnergy;
//           RTG.mGlobalVirial.mZ += LeknerSelfEnergy;
          XYZ leknerSelfEnergyVirial;
          leknerSelfEnergyVirial.mX = LeknerSelfEnergy/3.;
          leknerSelfEnergyVirial.mY = LeknerSelfEnergy/3.;
          leknerSelfEnergyVirial.mZ = LeknerSelfEnergy/3.;

          mClientState.mDynVarMgrIF->AddVirial( aRespaLevel, leknerSelfEnergyVirial );
          unsigned OTS = aSimTick;

          // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
          unsigned TimeStep = aSimTick / mClientState.mSimTicksPerTimeStep;

          // Check if this particular SimTick is the new TimeStep.
          if( TimeStep * mClientState.mSimTicksPerTimeStep == aSimTick )
            {
            BegLogLine(1)
              << " Lekner::Send() "
              << " Emitting RDG Energy packets "
              << " SimTick "  << aSimTick
              << " TimeStep " << TimeStep
              << EndLogLine;

            if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 ) 
              {
              ED_Emit_UDFd1( TimeStep, UDF_Binding::LeknerSelfEnergy_Code, 1u, GlobalPartialResult.mTotalLeknerSelfEnergy );
              }
            }
          }

        void
        inline
        Wait( int aSimTick )
          {
          // Loops better be done, no?
          return ;
          }

      };
  };


template<class DynVarMgrIF>
class LeknerAnchor
  {

    /*
     * The anchor IS the Server.
     */
  private:

    class LeknerEngine<DynVarMgrIF> mServer;

    /*
     *
     */
  public:

    void
    inline
    Init()
      {
      LeknerEngine<DynVarMgrIF> mServer;
      }


    // Create a compiler recongnized instance of the Client Interface - should be zero byts.
    typedef class LeknerEngine<DynVarMgrIF>::Client ClientIF;

    inline
    ClientIF &
    GetClientIF()
      {
      LeknerEngine<DynVarMgrIF>::Client * cr;
        // = new  LeknerEngine<DynVarMgrIF>::Client();
      pkNew( &cn, 1 );

      return( * cr  );
      }
  };



#endif
