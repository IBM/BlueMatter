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

#ifndef _EWALDKSPACEENGINE_HPP_
#define _EWALDKSPACEENGINE_HPP_

#ifndef PKFXLOG_EWALDKSPACEENGINE
#define PKFXLOG_EWALDKSPACEENGINE 0
#endif

#ifndef PKFXLOG_P3MESEND
#define PKFXLOG_P3MESEND  ( 0 )
#endif

#include <BlueMatter/UDF_Binding.hpp>

#include <BlueMatter/ExternalDatagram.hpp>
#include <pk/Barrier.hpp>

//******************************************************

#ifndef PKFXLOG_FORCE_TERMS
#define PKFXLOG_FORCE_TERMS (0)
#endif


TraceClient Ewald_KSpace_Start;
TraceClient Ewald_KSpace_Finis;

TraceClient Ewald_Eiks_Start;
TraceClient Ewald_Eiks_Finis;

TraceClient Ewald_Loop_Start;
TraceClient Ewald_Loop_Finis;

TraceClient Ewald_AllReduce_Start;
TraceClient Ewald_AllReduce_Finis;

TraceClient Ewald_LocalReduce_Start;
TraceClient Ewald_LocalReduce_Finis;

TraceClient Ewald_Forces_Start;
TraceClient Ewald_Forces_Finis;

//****************************************************************

template<class DynVarMgrIF>
class EwaldKSpaceEngine
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
    private:
        
        int ContiguousKIndex;
        int KStorageIndex;
        int iKx2, iKxy2;
        double birth_box_abs_rat_sqr;

    public:
        
        ClientState mClientState;
        
        inline
        void
        Connect( //NEED: think about whether to allow the volume or machine grid to be passed in here.
            DynVarMgrIF          * aDynVarMgrIF,   //Type for this should come in as a template interface
            unsigned               aSimTick,
            unsigned               aSimTicksPerTimeStep, // assumes Plimptonize only happens at OTS
            int                    aIntegratorLevel  )
            {
            mClientState.mDynVarMgrIF              = aDynVarMgrIF;
            mClientState.mSimTick                  = aSimTick;
            mClientState.mSimTicksPerTimeStep = aSimTicksPerTimeStep;
            mClientState.mIntegratorLevel          = aIntegratorLevel;
            
            EwaldIF::Init();
            }
        
        
        inline void ProcLegitK()
        {
            ContiguousKIndex++;
            KStorageIndex++;
            
            // IF THE BUFFER IS NOT FULL - continue, otherwise - REDUCE IT ALL AND START OVER WITH RESET CONTIGUOUSKINDEX

            if( ( KStorageIndex < EwaldIF::KStorageSize ) && ( ContiguousKIndex < EwaldIF::ContiguousKIndMax ) )
                return;
            
            const int inddiff = ContiguousKIndex - KStorageIndex;
            
            Ewald_LocalReduce_Start.HitOE( PKTRACE_EWALD_ALLREDUCE,
                                           "Ewald_LocalReduce",
                                           Platform::Topology::GetAddressSpaceId(),
                                           Ewald_LocalReduce_Start );
            
            ///// DO REDUCTION OF STRUCTURE FACTORS HERE
            
            for( int StorInd = 0; StorInd < KStorageIndex; StorInd++ )
            {
                const Complex * const Eikxyz_kind = EwaldIF::Eikxyz( StorInd );
#ifdef PK_PARALLEL
                const int egsf_ind = StorInd;
#else
                const int egsf_ind = inddiff + StorInd;
#endif
                EwaldIF::EwaldGlobalStructureFactor[ egsf_ind ] = 0;
                for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                {
                    EwaldIF::EwaldGlobalStructureFactor[ egsf_ind ].re += Eikxyz_kind[ ContiguousSiteIndex ].re;
                    EwaldIF::EwaldGlobalStructureFactor[ egsf_ind ].im += Eikxyz_kind[ ContiguousSiteIndex ].im;
                }
            }
            
            Ewald_LocalReduce_Finis.HitOE( PKTRACE_EWALD_ALLREDUCE,
                                           "Ewald_LocalReduce",
                                           Platform::Topology::GetAddressSpaceId(),
                                           Ewald_LocalReduce_Finis );
            
#ifdef PK_PARALLEL
//            EmitTimeStamp( aSimTick, Tag::EwaldAllReduce_Begin, 1 );
            Ewald_AllReduce_Start.HitOE( PKTRACE_EWALD_ALLREDUCE,
                                         "Ewald_AllReduce",
                                         Platform::Topology::GetAddressSpaceId(),
                                         Ewald_AllReduce_Start );
            
            Platform::Collective::FP_AllReduce_Sum( (double *)EwaldIF::EwaldGlobalStructureFactor, (double *)&EwaldIF::ReducedEwaldGlobalStructureFactor[inddiff], 2*KStorageIndex );
            
            Ewald_AllReduce_Finis.HitOE( PKTRACE_EWALD_ALLREDUCE,
                                         "Ewald_AllReduce",
                                         Platform::Topology::GetAddressSpaceId(),
                                         Ewald_AllReduce_Finis );
//            EmitTimeStamp( aSimTick, Tag::EwaldAllReduce_End, 1 );
#endif
            
            Ewald_Forces_Start.HitOE( PKTRACE_EWALD_ALLREDUCE,
                                      "Ewald_Forces",
                                      Platform::Topology::GetAddressSpaceId(),
                                      Ewald_Forces_Start );
            
            //---- the following is a replacement for GetKSpaceForce(...)
            
            for( int StorInd = 0; StorInd < KStorageIndex; StorInd++ )
            {
                const int contkind = inddiff + StorInd;
                
#if MSDDEF_DoPressureControl
                const double sclbdX = EwaldIF::LambDampK[ contkind ].mX * birth_box_abs_rat_sqr;
                const double sclbdY = EwaldIF::LambDampK[ contkind ].mY * birth_box_abs_rat_sqr;
                const double sclbdZ = EwaldIF::LambDampK[ contkind ].mZ * birth_box_abs_rat_sqr;
#else
                const double sclbdX = EwaldIF::LambDampK[ contkind ].mX;
                const double sclbdY = EwaldIF::LambDampK[ contkind ].mY;
                const double sclbdZ = EwaldIF::LambDampK[ contkind ].mZ;
#endif
                const double gsre = EwaldIF::ReducedEwaldGlobalStructureFactor[ contkind ].re;
                const double gsim = EwaldIF::ReducedEwaldGlobalStructureFactor[ contkind ].im;
                
                const Complex * const Eikxyz_kind = EwaldIF::Eikxyz( StorInd );

                for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                {    
                    const double sdre = Eikxyz_kind[ ContiguousSiteIndex ].re;
                    const double sdim = Eikxyz_kind[ ContiguousSiteIndex ].im;
                    const double fact = gsre * sdim - gsim * sdre;
                    
                    EwaldIF::SiteData[ ContiguousSiteIndex ].Force.mX -= fact * sclbdX;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].Force.mY -= fact * sclbdY;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].Force.mZ -= fact * sclbdZ;
                }
            }
            
            Ewald_Forces_Finis.HitOE( PKTRACE_EWALD_ALLREDUCE,
                                      "Ewald_Forces",
                                      Platform::Topology::GetAddressSpaceId(),
                                      Ewald_Forces_Finis );
            
            KStorageIndex = 0;
        }
        



        inline void forkz()
        {
            Complex * const Eikxyz_kind = EwaldIF::Eikxyz( KStorageIndex );

            for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
            {
                Eikxyz_kind[ ContiguousSiteIndex ].re = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.re;
                Eikxyz_kind[ ContiguousSiteIndex ].im = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.im;
            }

            int PrevStorInd = KStorageIndex;

            if( iKxy2 > 0 )  // exclude kx=ky=kz=0 case
            {
                ProcLegitK();
            }

            for( int kz = 1; ; kz++ )
            {
                int iKxyz2 = iKxy2 + kz * kz;
                if( iKxyz2 > EwaldIF::Kmax2 )
                    break;

                const Complex * const Eikxyz_prevkind = EwaldIF::Eikxyz( PrevStorInd );
                Complex * const Eikxyz_kind = EwaldIF::Eikxyz( KStorageIndex );

                for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                {
                    const double rea = Eikxyz_prevkind[ ContiguousSiteIndex ].re;
                    const double ima = Eikxyz_prevkind[ ContiguousSiteIndex ].im;
                    const double reb = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikz1.re;
                    const double imb = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikz1.im;
                    const double re = rea * reb - ima * imb;
                    const double im = rea * imb + reb * ima;
                    Eikxyz_kind[ ContiguousSiteIndex ].re = re;
                    Eikxyz_kind[ ContiguousSiteIndex ].im = im;
                }
                
                PrevStorInd = KStorageIndex;
                ProcLegitK();
            }
        }
        
        inline void forky()
        {              
            for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
            {
                EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.re = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re;
                EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.im = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im;
            }
            
            // ky = 0
            
            iKxy2 = iKx2;
            forkz();
            
            // ky > 0
            
            for( int ky = 1; ; ky++ )
            {
                iKxy2 = iKx2 + ky * ky;
                if( iKxy2 > EwaldIF::Kmax2 )
                    break;
                
                for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                {
                    const double rea = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.re;
                    const double ima = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.im;
                    const double reb =  EwaldIF::SiteData[ ContiguousSiteIndex ].Eiky1.re;
                    const double imb =  EwaldIF::SiteData[ ContiguousSiteIndex ].Eiky1.im;
                    const double re = rea * reb - ima * imb;
                    const double im = rea * imb + reb * ima;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.re = re;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.im = im;
                }
                
                forkz();
            }
            
            // ky = -1
            
            iKxy2 = iKx2 + 1;
            if( iKxy2 <= EwaldIF::Kmax2 )
            {
                for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                {
                    const double rea = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re;
                    const double ima = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im;
                    const double reb =  EwaldIF::SiteData[ ContiguousSiteIndex ].Eiky1.re;
                    const double imb = - EwaldIF::SiteData[ ContiguousSiteIndex ].Eiky1.im;
                    const double re = rea * reb - ima * imb;
                    const double im = rea * imb + reb * ima;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.re = re;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.im = im;
                }
                
                forkz();
                
                // ky < -1
                
                for( int ky = -2; ; ky-- )
                {
                    iKxy2 = iKx2 + ky * ky;
                    if( iKxy2 > EwaldIF::Kmax2 )
                        break;
                    
                    for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                    {
                        const double rea = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.re;
                        const double ima = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.im;
                        const double reb =  EwaldIF::SiteData[ ContiguousSiteIndex ].Eiky1.re;
                        const double imb = - EwaldIF::SiteData[ ContiguousSiteIndex ].Eiky1.im;
                        const double re = rea * reb - ima * imb;
                        const double im = rea * imb + reb * ima;
                        EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.re = re;
                        EwaldIF::SiteData[ ContiguousSiteIndex ].Eikxy.im = im;
                    }
                    
                    forkz();
                }
            }
        }
        
        inline void Send( int aSimTick, int aRespaLevel, int aDummy )
        {
            EmitTimeStamp( aSimTick, Tag::KSpace_Begin, 1 );
            BegLogLine(PKFXLOG_P3MESEND)
                << "Send(): "
                << " SimTick "
                << aSimTick
                << EndLogLine;
            
            Ewald_KSpace_Start.HitOE( PKTRACE_EWALD_KSPACE,
                                      "Ewald_KSpace",
                                      Platform::Topology::GetAddressSpaceId(),
                                      Ewald_KSpace_Start );
            
            mClientState.mIntegratorLevel = aRespaLevel;
            mClientState.mSimTicksPerTimeStep = MSD_IF::GetSimTicksAtRespaLevel()[ aRespaLevel ];
            
            EmitTimeStamp( aSimTick, Tag::EwaldEiks_Begin, 1 );
            Ewald_Eiks_Start.HitOE( PKTRACE_EWALD_EIKS,
                                    "Ewald_Eiks",
                                    Platform::Topology::GetAddressSpaceId(),
                                    Ewald_Eiks_Start );
            
            EwaldIF::PrecomputeEiks();
            
            Ewald_Eiks_Finis.HitOE( PKTRACE_EWALD_EIKS,
                                    "Ewald_Eiks",
                                    Platform::Topology::GetAddressSpaceId(),
                                    Ewald_Eiks_Finis );
            EmitTimeStamp( aSimTick, Tag::EwaldEiks_End, 1 );
            
            EmitTimeStamp( aSimTick, Tag::EwaldLoop_Begin, 1 );
            Ewald_Loop_Start.HitOE( PKTRACE_EWALD_LOOP,
                                    "Ewald_Loop",
                                    Platform::Topology::GetAddressSpaceId(),
                                    Ewald_Loop_Start );
            
            for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
            {
                EwaldIF::SiteData[ ContiguousSiteIndex ].Force.Zero();
            }

#if MSDDEF_DoPressureControl
            birth_box_abs_rat_sqr = mClientState.mDynVarMgrIF->mBirthBoxAbsoluteRatio.mX * mClientState.mDynVarMgrIF->mBirthBoxAbsoluteRatio.mX;
#endif

            //--- main "loop" starts
            
            ContiguousKIndex = 0;
            KStorageIndex = 0;
            
            for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
            {
                EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re = EwaldIF::SiteData[ ContiguousSiteIndex ].Charge;
                EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im = 0;
            }
            
            // kx = 0
            
            iKx2 = 0;
            forky();
            
            // kx > 0
            
            for( int kx = 1; ; kx++ )
            {
                iKx2 = kx * kx;
                if( iKx2 > EwaldIF::Kmax2 )
                    break;
                
                for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                {
                    const double rea = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re;
                    const double ima = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im;
                    const double reb = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikx1.re;
                    const double imb = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikx1.im;
                    const double re = rea * reb - ima * imb;
                    const double im = rea * imb + reb * ima;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re = re;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im = im;
                }
                
                forky();    
            }
            
            // kx = -1
            
            iKx2 = 1;
            
            for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
            {
                const double charge = EwaldIF::SiteData[ ContiguousSiteIndex ].Charge;
                EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re = charge * EwaldIF::SiteData[ ContiguousSiteIndex ].Eikx1.re;
                EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im = - charge * EwaldIF::SiteData[ ContiguousSiteIndex ].Eikx1.im;
            }
            
            forky();
            
            // kx < -1
            
            for( int kx = 2; ; kx++ )
            {
                iKx2 = kx * kx;
                if( iKx2 > EwaldIF::Kmax2 )
                    break;
                
                for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
                {
                    const double rea = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re;
                    const double ima = EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im;
                    const double reb = EwaldIF::SiteData[ ContiguousSiteIndex ].Eikx1.re;
                    const double imb = - EwaldIF::SiteData[ ContiguousSiteIndex ].Eikx1.im;
                    const double re = rea * reb - ima * imb;
                    const double im = rea * imb + reb * ima;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.re = re;
                    EwaldIF::SiteData[ ContiguousSiteIndex ].qEikx.im = im;
                }
                
                forky();    
            }
            
            //--- main "loop" finishes
            
            for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < EwaldIF::NumLocalSites; ContiguousSiteIndex++ )
            {
                const int AbsSiteId = EwaldIF::SiteData[ ContiguousSiteIndex ].AbsSiteId;
                const XYZ NewForce = 
                    mClientState.mDynVarMgrIF->GetForce( AbsSiteId, aSimTick, mClientState.mIntegratorLevel ) 
                    + EwaldIF::SiteData[ ContiguousSiteIndex ].Force;
                mClientState.mDynVarMgrIF->SetForce( AbsSiteId, aSimTick, mClientState.mIntegratorLevel, NewForce );
            }
            
            Ewald_Loop_Finis.HitOE( PKTRACE_EWALD_LOOP,
                                    "Ewald_Loop",
                                    Platform::Topology::GetAddressSpaceId(),
                                    Ewald_Loop_Finis );
            
            EmitTimeStamp( aSimTick, Tag::EwaldLoop_End, 1 );
            
            
            // do energy calculation, k-space distributed over nodes
            
            double LocalKSpaceEnergy = 0;
            for( int kind = EwaldIF::KIndLocalStart; kind < EwaldIF::KIndLocalFinish; kind++ )
            {
                EwaldIF::GetKSpaceEnergy( LocalKSpaceEnergy, kind );
            }
            
            XYZ LocalVirial;
            LocalVirial.mX = LocalVirial.mY = LocalVirial.mZ = (1./3.) * LocalKSpaceEnergy;
            mClientState.mDynVarMgrIF->AddVirial( aRespaLevel, LocalVirial );
            
            //
            // Self Energy part
            //
            double EwaldSelfEnergy = mClientState.mDynVarMgrIF->mBirthBoxAbsoluteRatio.mX * EwaldIF::EwaldSelfEnergyPrefactor;
            {
                XYZ ewaldSelfEnergyVirial;
                ewaldSelfEnergyVirial.mX = ewaldSelfEnergyVirial.mY = ewaldSelfEnergyVirial.mZ = EwaldSelfEnergy * (1./3.);
                mClientState.mDynVarMgrIF->AddVirial( aRespaLevel, ewaldSelfEnergyVirial );
            }
            
            
            unsigned OTS = aSimTick;
            
          // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
            unsigned TimeStep = aSimTick / mClientState.mSimTicksPerTimeStep;
            
            // Check if this particular SimTick is the new TimeStep.
            if( TimeStep * mClientState.mSimTicksPerTimeStep == aSimTick )
            {
                BegLogLine( PKFXLOG_P3MESEND)
                    << "PlimptonLoops::Send() "
                    << " Emitting RDG Energy packets "
                    << " SimTick "  << aSimTick
                    << " TimeStep " << TimeStep
                    << EndLogLine;
                
                if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 ) 
                {
                    EmitEnergy( TimeStep, UDF_Binding::EwaldKSpaceForce_Code, LocalKSpaceEnergy, 0 );
                    EmitEnergy( TimeStep, UDF_Binding::EwaldSelfEnergyForce_Code, EwaldSelfEnergy, 0 );
                }
            }
            
            
            Ewald_KSpace_Finis.HitOE( PKTRACE_EWALD_KSPACE,
                                      "Ewald_KSpace",
                                      Platform::Topology::GetAddressSpaceId(),
                                      Ewald_KSpace_Finis );
            EmitTimeStamp( aSimTick, Tag::KSpace_End, 1 );
                        
        }
        
        inline void
        Wait( int aSimTick )
        {
            // Loops better be done, no?
            return ;
        }
        
    };
};


template<class DynVarMgrIF>
class EwaldKSpaceAnchor
  {

    /*
     * The anchor IS the Server.
     */
  private:

    class EwaldKSpaceEngine<DynVarMgrIF> mServer;

    /*
     *
     */
  public:

    void
    inline
    Init()
      {
      EwaldKSpaceEngine<DynVarMgrIF> mServer;
      }


    // Create a compiler recongnized instance of the Client Interface - should be zero byts.
    typedef class EwaldKSpaceEngine<DynVarMgrIF>::Client ClientIF;

    inline
    ClientIF &
    GetClientIF()
      {
      EwaldKSpaceEngine<DynVarMgrIF>::Client * cr;
        // = new  EwaldKSpaceEngine<DynVarMgrIF>::Client();
      pkNew( &cr, 1 );

      return( * cr  );
      }
  };



#endif

