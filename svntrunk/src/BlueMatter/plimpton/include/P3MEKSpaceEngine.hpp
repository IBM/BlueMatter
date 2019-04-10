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

#ifndef _P3MEKSPACEENGINE_HPP_
#define _P3MEKSPACEENGINE_HPP_

#ifndef PKFXLOG_P3MEKSPACEENGINE
#define PKFXLOG_P3MEKSPACEENGINE ( 0 )
#endif

#ifndef PKFXLOG_RECV_BARRIER
#define PKFXLOG_RECV_BARRIER ( 0 )
#endif

#ifndef PKFXLOG_SENDFORCETONODE
#define PKFXLOG_SENDFORCETONODE ( 0 )
#endif

#ifndef PKFXLOG_ADD_FORCE
#define PKFXLOG_ADD_FORCE ( 0 )
#endif

#ifndef PKFXLOG_ADD_KSPACE_FORCE
#define PKFXLOG_ADD_KSPACE_FORCE ( 0 )
#endif

#ifndef PKFXLOG_P3MESEND
#define PKFXLOG_P3MESEND  ( 0 )
#endif

#ifndef PKFXLOG_P3MESEND_1
#define PKFXLOG_P3MESEND_1  ( 0 )
#endif

#ifndef PKFXLOG_REGRESS_UDF_RESULTS
#define PKFXLOG_REGRESS_UDF_RESULTS ( 0 )
#endif

#ifndef PKFXLOG_P3MEALLOCATE
#define PKFXLOG_P3MEALLOCATE ( 0 )
#endif

#ifndef PKFXLOG_BEFORE_FWD_FFT
#define PKFXLOG_BEFORE_FWD_FFT ( 0 )
#endif

#include <BlueMatter/UDF_Binding.hpp>

#include <BlueMatter/ExternalDatagram.hpp>

#include <BlueMatter/p3me.hpp>

static TraceClient KSpaceTotalStart;
static TraceClient KSpaceTotalFinis;

static TraceClient KSpaceForwardFFTStart;
static TraceClient KSpaceForwardFFTFinis;

static TraceClient KSpaceAssignChargeStart;
static TraceClient KSpaceAssignChargeFinis;

static TraceClient KSpaceBuildFragmentListStart;
static TraceClient KSpaceBuildFragmentListFinis;

static TraceClient KSpaceInterpolateStart;
static TraceClient KSpaceInterpolateFinis;

static TraceClient KSpaceBcastStart;
static TraceClient KSpaceBcastFinis;

static TraceClient KSpaceReduceStart;
static TraceClient KSpaceReduceFinis;

//******************************************************

#ifndef PKFXLOG_FORCE_TERMS
#define PKFXLOG_FORCE_TERMS (0)
#endif

extern void ForceExportFence();
extern void ForceImportFence();

//****************************************************************
template<class DynVarMgrIF,class P3ME_PLAN, class P3MECommIF>
class P3MEKSpaceEngine
  {
  public:
    typedef void *VoidPtr;

    //********* FROM NB ****************/
    class ClientState
      {
      public:
          DynVarMgrIF *mDynVarMgrIF;// MEMORY_ALIGN(5);
          int          mSimTick;//     MEMORY_ALIGN(5);
          unsigned     mSimTicksPerTimeStep; //MEMORY_ALIGN(5);
          int          mIntegratorLevel; //MEMORY_ALIGN(5);

          int          mRealSpaceKSpaceSeqNo; //MEMORY_ALIGN(5);
          int          mVerletSiteListAllocatedCount; //MEMORY_ALIGN(5);
          
          // double mPadding0[ 4 ];
          P3MECommIF   mCommIF; // MEMORY_ALIGN(5);
          //        double mPadding1[ 4 ];
      };

    // Every client port must allocated one of these structures.
    // It is returned as a handle by either Connect or send.
    // It is passed to calls to wait.
    // Calling wait shows that DynamaicVariables are ready to be updated.
    class Client
      {
      public:

          ClientState mClientState; // MEMORY_ALIGN(5);
          
          //---------------------------------------------------------------------------------//
          //-             VERLET LIST FOR P3ME SITES                                        -//
          //-                                                                               -//
          
          tVerletSiteInfo *VerletSiteList;
          int VerletSiteListLen;

          double mPadding1[ 4 ];
          
          int mHandle MEMORY_ALIGN( 5 );

          double mPadding2[ 4 ];
          //---------------------------------------------------------------------------------//

        void 
        ExpandVerletList( tVerletSiteInfo** aList, int aLen, int &aAllocatedCount )
          {
          #ifdef PK_BGL
          if( Platform::Thread::GetId() == 1 )
            {
            char* Abort = NULL;
            Abort[ 0 ] = 0;
            }
          #endif

          void * Memory;
          
          int NewCount = aAllocatedCount + 1024;
          int NewSize  = NewCount * sizeof( tVerletSiteInfo );
          
          Memory = malloc( NewSize );
          
          if( Memory == NULL )
            PLATFORM_ABORT( "Memory == NULL" );

          if( *aList != NULL )
            {
            memcpy( Memory, *aList, aLen * sizeof( tVerletSiteInfo ) );
            free( *aList );
            }
          *aList = (tVerletSiteInfo *) Memory;
          aAllocatedCount = NewCount;
          }

        void
        Connect( //NEED: think about whether to allow the volume or machine grid to be passed in here.
                 DynVarMgrIF          * aDynVarMgrIF,   //Type for this should come in as a template interface
                 unsigned               aSimTick,
                 unsigned               aSimTicksPerTimeStep, // assumes Plimptonize only happens at OTS
                 int                    aIntegratorLevel  )
          {
          	BegLogLine(PKFXLOG_SETUP)
          	  << "P3MEKSpaceEngine::Connect" 
          	  << EndLogLine ;
          mClientState.mDynVarMgrIF              = aDynVarMgrIF;
          mClientState.mSimTick                  = aSimTick;
          mClientState.mSimTicksPerTimeStep      = aSimTicksPerTimeStep;
          mClientState.mIntegratorLevel          = aIntegratorLevel;
          mClientState.mRealSpaceKSpaceSeqNo     = 0;          

#ifdef PK_PHASE5_SPLIT_COMMS
          mClientState.mCommIF.Init( 
              & mClientState.mDynVarMgrIF->mIntegratorStateManagerIF,
              & mClientState.mDynVarMgrIF->mKspaceInteractionStateManagerIF,
              mClientState.mDynVarMgrIF->mCenterOfBoundingBox,
              mClientState.mDynVarMgrIF->mSiteIdToFragmentRepMap,
              NUMBER_OF_SITES,
              mClientState.mDynVarMgrIF->mDynamicBoxDimensionVector );
#else
          // This size will get expanded as needed
          mClientState.mVerletSiteListAllocatedCount = 1024;

          VerletSiteList = new tVerletSiteInfo[ mClientState.mVerletSiteListAllocatedCount ]; //######  need to reduce this!!!
          if( VerletSiteList == NULL )
            {
            PLATFORM_ABORT("VerletSiteList == NULL");
            }
#endif

          P3ME< IrreducibleFragmentOperandIF, 
                FragmentPartialResultsIF, 
                P3ME_PLAN::FFT_FWD_TYPE, 
                P3ME_PLAN::FFT_REV_TYPE 
              >::Init();

          
          P3ME< IrreducibleFragmentOperandIF, 
                FragmentPartialResultsIF, 
                P3ME_PLAN::FFT_FWD_TYPE, 
                P3ME_PLAN::FFT_REV_TYPE 
              >::ParticleMesh::InitVoxelAddExtent();
          
#ifdef PK_PHASE5_SPLIT_COMMS
          rts_dcache_evict_normal();
#endif
          }

        inline
        void
        InitEwaldSelfEnergy()
          {
          P3ME< IrreducibleFragmentOperandIF, 
                FragmentPartialResultsIF, 
                P3ME_PLAN::FFT_FWD_TYPE, 
                P3ME_PLAN::FFT_REV_TYPE 
              >::InitEwaldSelfEnergy();          
          }

        inline
        void
        GetLocalPaddedMeshDimensions( XYZ& vmin, XYZ& vmax )
          {          
          P3ME< IrreducibleFragmentOperandIF, 
                FragmentPartialResultsIF, 
                P3ME_PLAN::FFT_FWD_TYPE, 
                P3ME_PLAN::FFT_REV_TYPE 
              >::ParticleMesh::GetLocalPaddedMeshDimensions( vmin, vmax );
          }

        inline
        void
        GetNodeIDList( const XYZ& vmin, const XYZ& vmax, int idlist[], int& idnum )
          {          
          P3ME< IrreducibleFragmentOperandIF, 
                FragmentPartialResultsIF, 
                P3ME_PLAN::FFT_FWD_TYPE, 
                P3ME_PLAN::FFT_REV_TYPE 
              >::ParticleMesh::GetNodeIDList( vmin, vmax, idlist, idnum );
          }

        inline
        XYZ
        GetVoxelAddExtent()
          {          
              return P3ME< IrreducibleFragmentOperandIF, 
                  FragmentPartialResultsIF, 
                  P3ME_PLAN::FFT_FWD_TYPE, 
                  P3ME_PLAN::FFT_REV_TYPE 
                  >::ParticleMesh::GetVoxelAddExtent();
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
              int rc = mFragmentOrdinal ;
              return( rc );
              }


// OK OK... avoided a copy, but at the cost of exposing a pointer into otherwise encapsulated Dynamic Variable Manager
//NEED TO THINK ABOUT THIS...
            inline
            XYZ             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
            GetPosition( int aIndex )
              {
              int base   = MSD_IF::GetIrreducibleFragmentFirstSiteId( mFragmentOrdinal );              
              #ifdef BM_SPATIAL_DECOMPOSITION
                XYZ rc   =  mDynVarMgrIF->mKspaceInteractionStateManagerIF.GetPosition( mFragmentOrdinal, aIndex );
              #else
                XYZ rc   = *( mDynVarMgrIF->GetPositionPtr( base + aIndex ) );
              #endif
              return( rc );
              }

#ifdef PK_PHASE5_SPLIT_COMMS
            inline
            XYZ
            GetPositionAndClearForForce( int aIndex )
              {
              XYZ rc   =  mDynVarMgrIF->mKspaceInteractionStateManagerIF.GetPositionAndClearForForce( mFragmentOrdinal, aIndex );
              return( rc );
              }

            inline
            XYZ
            ClearForForce( int aIndex )
              {
              XYZ rc   =  mDynVarMgrIF->mKspaceInteractionStateManagerIF.ClearForForce( mFragmentOrdinal, aIndex );
              return( rc );
              }
#endif
              
            inline
            double             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
            GetCharge( int aIndex )
              {
              int base   = MSD_IF::GetIrreducibleFragmentFirstSiteId( mFragmentOrdinal );
              double rc  = MSD_IF::GetSiteInfo( base + aIndex ).charge;
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


            FragmentPartialResultsIF(        int          aSimTick,
                                           int          aIntegratorLevel,
                                           DynVarMgrIF *aDynVarMgrIF ,
                                           int          aFragmentOrdinal )
              {
              mSimTick          = aSimTick;
              mIntegratorLevel  = aIntegratorLevel;
              mDynVarMgrIF      = aDynVarMgrIF;
              mFragmentOrdinal = aFragmentOrdinal;
              }

            inline
            void
            AddForce( int aIndex, XYZ& aForce )
              {
              #ifdef BM_SPATIAL_DECOMPOSITION
                mDynVarMgrIF->mKspaceInteractionStateManagerIF.AddForce( mFragmentOrdinal, aIndex, aForce );
              #else
                int base   = mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( mFragmentOrdinal );
                mDynVarMgrIF->AddForce( base + aIndex, mSimTick, mIntegratorLevel, aForce );                
              #endif
              }

          };

        class GlobalPartialResultsIF
          {
          public:

            double mTotalEwaldKSpaceEnergy;
            double mTotalEwaldSelfEnergy;

            static inline const GlobalPartialResultsIF ZERO_VALUE()
            {
                    GlobalPartialResultsIF Rc;
                    Rc.Zero();
                    return ( Rc );
            }


            void
            Zero()
              {
              mTotalEwaldKSpaceEnergy     = 0.0 ;
              mTotalEwaldSelfEnergy     = 0.0 ;
              }

            inline
            void
            AddEnergy( int aUDF_Code, double aEnergy )
              {
              switch( aUDF_Code )
                {
                case UDF_Binding::EwaldKSpaceForce_Code :
                  {
                    mTotalEwaldKSpaceEnergy += aEnergy;
                    break;
                  }
                case UDF_Binding::EwaldSelfEnergyForce_Code :
                  {
                    mTotalEwaldSelfEnergy += aEnergy;
                    break;
                  }
                default :
                  assert(0);
                };
              }
          };
        
       int HasFFTHeaderChecksumChanged()
        {
          return   P3ME< IrreducibleFragmentOperandIF,
            FragmentPartialResultsIF,
            P3ME_PLAN::FFT_FWD_TYPE, 
            P3ME_PLAN::FFT_REV_TYPE 
            >::HasFFTHeaderChecksumChanged();
        } 

       
       //-- note: coord is modified here!!!
       inline void MapToCentralCell( XYZ& coord )
       {
           {
               const double box_1 = mClientState.mDynVarMgrIF -> mDynamicBoxInverseDimensionVector.mX;
               const double box = mClientState.mDynVarMgrIF -> mDynamicBoxDimensionVector.mX;
               coord.mX -= floor(coord.mX * box_1) * box;
           }
           {
               const double box_1 = mClientState.mDynVarMgrIF -> mDynamicBoxInverseDimensionVector.mY;
               const double box = mClientState.mDynVarMgrIF -> mDynamicBoxDimensionVector.mY;
               coord.mY -= floor(coord.mY * box_1) * box;
           }
           {
               const double box_1 = mClientState.mDynVarMgrIF -> mDynamicBoxInverseDimensionVector.mZ;
               const double box = mClientState.mDynVarMgrIF -> mDynamicBoxDimensionVector.mZ;
               coord.mZ -= floor(coord.mZ * box_1) * box;
           }
       }

       inline int SiteRelevant( const XYZ& coord_in_cc, const XYZ& vmin, const XYZ& vmax ) const
       {
           const iXYZ& wrp = P3ME< IrreducibleFragmentOperandIF,
               FragmentPartialResultsIF,
               P3ME_PLAN::FFT_FWD_TYPE, 
               P3ME_PLAN::FFT_REV_TYPE 
               >::ParticleMesh::wrapped_dim;

           return 
               ( wrp.mX || 
                 ( coord_in_cc.mX >= vmin.mX && coord_in_cc.mX <= vmax.mX ) )
               &&
               ( wrp.mY ||
                 ( coord_in_cc.mY >= vmin.mY && coord_in_cc.mY <= vmax.mY ) )
               &&
               ( wrp.mZ || 
                 ( coord_in_cc.mZ >= vmin.mZ && coord_in_cc.mZ <= vmax.mZ ) );
       }


      void 
      RefreshVerletSiteList( int aSimTick )
      {
	static FragId SourceList[NUMBER_OF_FRAGMENTS];   //############# allocate it here for now; 
	int SizeInSourceList = 0;
	for( int Source = 0; Source < mClientState.mDynVarMgrIF ->GetIrreducibleFragmentCount(); Source++ )
	  {
	    IrreducibleFragmentOperandIF SourceFragmentOperandIF (
								  aSimTick,
								  mClientState.mIntegratorLevel,
								  mClientState.mDynVarMgrIF,
								  Source
								  );
             
	    {
	      SourceList[SizeInSourceList++] = Source;
	    }
	  }

	//-- scan the incoming fragment list to select sites

	XYZ vmin, vmax;
	GetLocalPaddedMeshDimensions( vmin, vmax );
	const double halfguardzone = 0.5 * mClientState.mDynVarMgrIF->mCurrentVerletGuardZone;

	vmin.mX -= halfguardzone;
	vmin.mY -= halfguardzone;
	vmin.mZ -= halfguardzone;
	vmax.mX += halfguardzone;
	vmax.mY += halfguardzone;
	vmax.mZ += halfguardzone;
           
	XYZ shift;
	shift.mX = -vmin.mX;
	shift.mY = -vmin.mY;
	shift.mZ = -vmin.mZ;

	vmin.mX += shift.mX; vmin.mY += shift.mY; vmin.mZ += shift.mZ;
	vmax.mX += shift.mX; vmax.mY += shift.mY; vmax.mZ += shift.mZ;

	VerletSiteListLen = 0;
           
	for( int ind = 0; ind < SizeInSourceList; ind++ )
	  {
	    FragId fragid = SourceList[ ind ];
               
	    IrreducibleFragmentOperandIF SourceFragmentIF (
							   aSimTick,
							   mClientState.mIntegratorLevel,
							   mClientState.mDynVarMgrIF,
							   fragid
							   );
               
	    int SourceIrreducibleFragmentId = SourceFragmentIF.GetIrreducibleFragmentId();
	    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
               
	    for( int SourceFragmentSiteIndex = 0;
		 SourceFragmentSiteIndex < SourceFragmentSiteCount;
		 SourceFragmentSiteIndex++ )
	      {
		XYZ coord = SourceFragmentIF.GetPosition( SourceFragmentSiteIndex );

		coord.mX += shift.mX;
		coord.mY += shift.mY;
		coord.mZ += shift.mZ;

		MapToCentralCell( coord );                   

		if( SiteRelevant( coord, vmin, vmax ) )
		  {
		    if( VerletSiteListLen >= mClientState.mVerletSiteListAllocatedCount )
		      ExpandVerletList( &VerletSiteList, VerletSiteListLen, mClientState.mVerletSiteListAllocatedCount );
                     
		    VerletSiteList[VerletSiteListLen].fragid = fragid;
		    VerletSiteList[VerletSiteListLen].siteind = SourceFragmentSiteIndex;
		    VerletSiteList[VerletSiteListLen].charge = SourceFragmentIF.GetCharge( SourceFragmentSiteIndex );
		    VerletSiteListLen++;
		  }
	      }
	  }
      }
  
      struct SendActorArgs
        {
        int                                                   mSimTick;    
        typename P3MEKSpaceEngine<DynVarMgrIF, P3ME_PLAN, P3MECommIF>::Client* mThis;
        };

      static 
      void* 
      SendActor( void * args )
        {
#pragma execution_frequency( very_low )
        SendActorArgs* saa = (SendActorArgs *) args;
        BegLogLine(PKFXLOG_P3MESEND)
          << "SendActor mThis=" << saa->mThis 
          << EndLogLine ; 
        saa->mThis->Send( saa->mSimTick, 0, 0 );
	return NULL;
        }

      void
      Send( int aSimTick, int aRespaLevel, int aVerletListRecookSignal )
        {              
#pragma execution_frequency( very_low )
#ifdef PK_PHASE5_SPLIT_COMMS
#ifndef PHASE5_RUN_KSPACE_ON_CORE0
          if( Platform::Thread::GetId() == 0 )
            {
            SendActorArgs saa;
            saa.mSimTick = aSimTick;
            saa.mThis    = this;
            rts_dcache_evict_normal();
            
            Platform::Control::Barrier();

            BegLogLine(PKFXLOG_P3MESEND)
              << "About to start core 1, saa.mThis=" << saa.mThis 
              << EndLogLine ;
            mHandle = Platform::Coprocessor::Start( SendActor, &saa, sizeof( SendActorArgs ) );
            return;
            }              
#endif          
          BegLogLine( PKFXLOG_P3MESEND | PKFXLOG_CHASEHANG ) << SimTick << " Got here" << EndLogLine;

          KSpaceTotalStart.HitOE( PKTRACE_KSPACE_TOTAL,
                                  "KSpaceTotal",
                                  Platform::Topology::GetAddressSpaceId(),
                                  KSpaceTotalStart );

          KSpaceBcastStart.HitOE( PKTRACE_KSPACE_BCAST,
                                  "KSpaceBcast",
                                  Platform::Topology::GetAddressSpaceId(),
                                  KSpaceBcastStart );         
          
          BegLogLine( PKFXLOG_P3MESEND | PKFXLOG_CHASEHANG ) << SimTick << " Got here" << EndLogLine;

          {
          unsigned long dummy3;
          unsigned long dummy4;
          Platform::Collective::GlobalMax( &dummy3, &dummy4 );
          }

          BegLogLine( PKFXLOG_P3MESEND | PKFXLOG_CHASEHANG ) << SimTick << " Got here" << EndLogLine;

          mClientState.mCommIF.BroadcastExecute( aSimTick );
          
          BegLogLine( PKFXLOG_P3MESEND | PKFXLOG_CHASEHANG ) << SimTick << " Got here" << EndLogLine;

          KSpaceBcastFinis.HitOE( PKTRACE_KSPACE_BCAST,
                                  "KSpaceBcast",
                                  Platform::Topology::GetAddressSpaceId(),
                                  KSpaceBcastFinis );
#endif

          BegLogLine( 0 )
            << "Entering P3ME_Send MSD_IF::GetNumberOfEnergyReportingUDFs(): " << MSD_IF::GetNumberOfEnergyReportingUDFs()
            << EndLogLine;
          
          static GlobalPartialResultsIF GlobalPartialResult = GlobalPartialResultsIF::ZERO_VALUE();

#pragma execution_frequency( very_low )
          BegLogLine( PKFXLOG_P3MESEND_1 )

            << "Send(): "
            << " SimTick "
            << aSimTick
            << EndLogLine;

          if( aSimTick % MSD::SimpleRespaRatio == 0 )
          {
          mClientState.mIntegratorLevel = aRespaLevel;
          mClientState.mSimTicksPerTimeStep = MSD_IF::GetSimTicksAtRespaLevel()[ aRespaLevel ];

          GlobalPartialResult.Zero();

          //////////////////////////////////////////////////////////////////////////////
          //
          // P3ME method. P3ME shares the same real-space implementation with Ewald,
          // Only the k-space energy is being replaced by the following code
          //
          //////////////////////////////////////////////////////////////////////////////

          //-----------------------------------------------
          // K-space part of P3ME,
          // The k-space energy is currently being added to the
          // total energy as Ewald
          //-----------------------------------------------

          P3ME< IrreducibleFragmentOperandIF,
                FragmentPartialResultsIF,
                P3ME_PLAN::FFT_FWD_TYPE, 
                P3ME_PLAN::FFT_REV_TYPE 
              >::ParticleMesh::ZeroGridCharge();

          // need to do it every time if volume changes
          P3ME< IrreducibleFragmentOperandIF,
              FragmentPartialResultsIF,
              P3ME_PLAN::FFT_FWD_TYPE, 
              P3ME_PLAN::FFT_REV_TYPE 
              >::ParticleMesh::InitVoxelAddExtent();


          BegLogLine( PKFXLOG_P3MESEND )
            << "After P3ME<IrreducibleFragmentOperandIF,FragmentPartialResultsIF,P3ME_PLAN::FFT_TYPE>::ParticleMesh::ZeroGridCharge();"
            << EndLogLine;

          /*----------------------------------------------------------------------------------*/
          /*--                       build a Verlet list of relevant sites                  --*/
          /*----------------------------------------------------------------------------------*/

          KSpaceBuildFragmentListStart.HitOE( PKTRACE_KSPACE_BUILD_FRAGMENT_LIST,
                                              "KSpaceBuildFragmentList",
                                              Platform::Topology::GetAddressSpaceId(),
                                              KSpaceBuildFragmentListStart );         
          
                    
#ifdef PK_PHASE5_SPLIT_COMMS
          VerletSiteList    = mClientState.mDynVarMgrIF->mKspaceInteractionStateManagerIF.GetKspaceVerletList();
          VerletSiteListLen = mClientState.mDynVarMgrIF->mKspaceInteractionStateManagerIF.GetKspaceVerletListCount();
#else
          if( aVerletListRecookSignal )
            RefreshVerletSiteList( aSimTick );
#endif     

          KSpaceBuildFragmentListFinis.HitOE( PKTRACE_KSPACE_BUILD_FRAGMENT_LIST,
                                              "KSpaceBuildFragmentList",
                                              Platform::Topology::GetAddressSpaceId(),
                                              KSpaceBuildFragmentListFinis );         
          
          /*-----------------------------------------------------------------------------------*/
          /*--             process sites in verlet list to assign charges to the grid        --*/
          /*-----------------------------------------------------------------------------------*/


          // EmitTimeStamp(aSimTick, Tag::KSpaceChargeAssignment_Begin, 1 );
          KSpaceAssignChargeStart.HitOE( PKTRACE_KSPACE_ASSIGN_CHARGE,
                                         "KSpaceAssignCharge",
                                         Platform::Topology::GetAddressSpaceId(),
                                         KSpaceAssignChargeStart );

          P3ME< IrreducibleFragmentOperandIF,
              FragmentPartialResultsIF,
              P3ME_PLAN::FFT_FWD_TYPE, 
              P3ME_PLAN::FFT_REV_TYPE 
              >::ParticleMesh::ResetCache();

          BegLogLine( PKFXLOG_P3MEALLOCATE )
            << "AssignCharge VerletSiteListLen=" << VerletSiteListLen
#ifndef PK_PHASE5_SPLIT_COMMS          	
            << " mVerletSiteListAllocatedCount=" << mClientState.mVerletSiteListAllocatedCount
#endif              
            << EndLogLine;
            
          for( int ind = 0; ind < VerletSiteListLen; ind++ )
          {
#ifndef PK_PHASE5_SPLIT_COMMS          	
// There is only an 'mVerletList' if we aren't splitting the comms
              assert( ind >= 0 && ind < mClientState.mVerletSiteListAllocatedCount );
#endif              
              FragId fragid = VerletSiteList[ind].fragid;
              
              IrreducibleFragmentOperandIF SourceFragmentIF (
                  aSimTick,
                  mClientState.mIntegratorLevel,
                  mClientState.mDynVarMgrIF,
                  fragid
                  );
              
              int siteind = VerletSiteList[ind].siteind;
//              XYZ coord = *SourceFragmentIF.GetPositionPtr( siteind );
              XYZ coord = SourceFragmentIF.GetPosition( siteind );
              const double charge = VerletSiteList[ind].charge;

              const int usd = P3ME< IrreducibleFragmentOperandIF,
                  FragmentPartialResultsIF,
                  P3ME_PLAN::FFT_FWD_TYPE, 
                  P3ME_PLAN::FFT_REV_TYPE 
                  >::ParticleMesh::AssignCharge( coord,
						 charge,
						 MSD_IF::GetExternalSiteIdForInternalSiteId(MSD_IF::GetIrreducibleFragmentFirstSiteId(fragid) +
											    siteind),
						 aSimTick );

              VerletSiteList[ ind ].siteused = usd;
          }
          
          KSpaceAssignChargeFinis.HitOE( PKTRACE_KSPACE_ASSIGN_CHARGE,
                                         "KSpaceAssignCharge",
                                         Platform::Topology::GetAddressSpaceId(),
                                         KSpaceAssignChargeFinis );

          // EmitTimeStamp(aSimTick, Tag::KSpaceChargeAssignment_End, 1 );

#if PKFXLOG_BEFORE_FWD_FFT
	  typedef P3ME< IrreducibleFragmentOperandIF,
	    FragmentPartialResultsIF, 
	    P3ME_PLAN::FFT_FWD_TYPE, 
	    P3ME_PLAN::FFT_REV_TYPE 
	    > P3meImpl;

	  for (int ix = 0; ix < P3meImpl::GridVoxel::iLocalGridSize.mX; ++ix)
	    {
	      int ixabs = ix + P3meImpl::GridVoxel::iLeftCorner.mX;
	      for (int iy = 0; iy < P3meImpl::GridVoxel::iLocalGridSize.mY; ++iy)
		{
		  int iyabs = iy + P3meImpl::GridVoxel::iLeftCorner.mY;
		  for (int iz = 0; iz < P3meImpl::GridVoxel::iLocalGridSize.mZ; ++iz)
		    {
		      int izabs = iz + P3meImpl::GridVoxel::iLeftCorner.mZ;
		      BegLogLine(PKFXLOG_BEFORE_FWD_FFT)
			<< FormatString("BEFORE_FWD_FFT %6d ") << aSimTick
			<< FormatString("ix iy iz: %6d ")      << ixabs
			<< FormatString("%6d ")                << iyabs
			<< FormatString("%6d ")                << izabs
			<< FormatString(" rs_elt: ")           << P3meImpl::FFT_FWD_IF->GetRealSpaceElement( ix, iy, iz )
			<< EndLogLine;
		    }
		}
	    }
#endif

          /*-------------------------------------------------------------------------------*/
          /*--                                forward FFT                                --*/
          /*-------------------------------------------------------------------------------*/

          BegLogLine( PKFXLOG_P3MESEND )
            << "Before P3ME<IrreducibleFragmentOperandIF,FragmentPartialResultsIF,P3ME_PLAN::FFT_TYPE>::FFT::ForwardFFT();"
            << EndLogLine;

          KSpaceForwardFFTStart.HitOE( PKTRACE_KSPACE_FFT_FWD,
                                       "KSpaceForwardFFT",
                                       Platform::Topology::GetAddressSpaceId(),
                                       KSpaceForwardFFTStart );         
 
          
          // TimeValue StartKSpaceFFTTime = Platform::Clock::GetTimeValue();
          //EmitTimeStamp( aSimTick, Tag::KSpaceForwardFFT_Begin, 1 );
          // FFT
          P3ME< IrreducibleFragmentOperandIF,
                FragmentPartialResultsIF,
                P3ME_PLAN::FFT_FWD_TYPE,
                P3ME_PLAN::FFT_REV_TYPE
              >::RunForwardFFT();


//           if( Platform::Topology::GetAddressSpaceId() == 0 )
//             {
//               TimeValue FinishKSpaceFFTTime = Platform::Clock::GetTimeValue();
//               TimeValue DifferenceFFT = FinishKSpaceFFTTime - StartKSpaceFFTTime;
//               ED_Emit_ControlTimeStamp( aSimTick, Tag::KSpaceForwardFFT, 1u, 
//                                         DifferenceFFT.Seconds, DifferenceFFT.Nanoseconds );          
//             }

          // EmitTimeStamp( aSimTick, Tag::KSpaceForwardFFT_End, 1 );
          KSpaceForwardFFTFinis.HitOE( PKTRACE_KSPACE_FFT_FWD,
                                       "KSpaceForwardFFT",
                                       Platform::Topology::GetAddressSpaceId(),
                                       KSPaceForwardFFTFinis );          

          BegLogLine( PKFXLOG_P3MESEND )
            << "Before P3ME<IrreducibleFragmentOperandIF,FragmentPartialResultsIF,P3ME_PLAN::FFT_TYPE>::ParticleMesh::FindFields();"
            << EndLogLine;

          /*--------------------------------------------------------------------------*/
          /*-- do necessary convolutions in K space and inverse FFT transforms here --*/
          /*--------------------------------------------------------------------------*/
          
          P3ME< IrreducibleFragmentOperandIF,
                FragmentPartialResultsIF,
                P3ME_PLAN::FFT_FWD_TYPE, 
                P3ME_PLAN::FFT_REV_TYPE 
              >::ParticleMesh::FindFields( mClientState.mDynVarMgrIF->mVirial[ aRespaLevel ], aSimTick );
          
//            FindFieldsFinis.HitOE( PKTRACE_FIND_FIELDS,
//                                   "P3ME_FindFields_Control",
//                                   Platform::Topology::GetAddressSpaceId(),
//                                   FindFieldsFinis );


          BegLogLine( PKFXLOG_P3MESEND )
            << "After P3ME<IrreducibleFragmentOperandIF,FragmentPartialResultsIF,P3ME_PLAN::FFT_TYPE>::ParticleMesh::FindFields();"
            << EndLogLine;

          double P3MEKSpaceEnergy = 0.0;

          // TimeValue StartKSpaceInterpolationTime = Platform::Clock::GetTimeValue();
          //EmitTimeStamp( aSimTick, Tag::KSpaceInterpolation_Begin, 1 );

          KSpaceInterpolateStart.HitOE( PKTRACE_KSPACE_INTERPOLATE,
                                        "KSpaceInterpolate",
                                        Platform::Topology::GetAddressSpaceId(),
                                        KSpaceInterpolateStart );

          /*---------------------------------------------------------------------*/
          /*--           calculate forces and energy on particles              --*/
          /*---------------------------------------------------------------------*/

         for( int ind = 0; ind < VerletSiteListLen; ind++ )
          {
#pragma execution_frequency( very_high )
              
              if( VerletSiteList[ind].siteused == 0 )
                {
#ifdef PK_PHASE5_SPLIT_COMMS
                mClientState.mCommIF.mKspaceInteractionStateManagerIFPtr->ClearForForce( VerletSiteList[ ind ].fragid,
                                                                                         VerletSiteList[ ind ].siteind );
#endif                
                continue;
                }

              FragId fragid = VerletSiteList[ ind ].fragid;
              
              IrreducibleFragmentOperandIF SourceFragmentIF (
                  aSimTick,
                  mClientState.mIntegratorLevel,
                  mClientState.mDynVarMgrIF,
                  fragid
                  );
              
              FragmentPartialResultsIF SourcePartialResultsIF(
                  aSimTick,
                  mClientState.mIntegratorLevel,
                  mClientState.mDynVarMgrIF,
                  fragid
                  );
              
              int siteind = VerletSiteList[ind].siteind;
              
#ifdef PK_PHASE5_SPLIT_COMMS
              XYZ coord = SourceFragmentIF.GetPositionAndClearForForce( siteind );
#else
              XYZ coord = SourceFragmentIF.GetPosition( siteind );
#endif
              // XYZ coord = *SourceFragmentIF.GetPositionPtr( siteind );
              const double charge = VerletSiteList[ind].charge;
              XYZ SiteForce;
              double SiteEnergy;
              P3ME< IrreducibleFragmentOperandIF,
                  FragmentPartialResultsIF,
                  P3ME_PLAN::FFT_FWD_TYPE, 
                  P3ME_PLAN::FFT_REV_TYPE 
                  >::ParticleMesh::InterpolateFields( coord,
						      charge,
						      SiteForce,
						      SiteEnergy,
						      MSD_IF::GetExternalSiteIdForInternalSiteId(MSD_IF::GetIrreducibleFragmentFirstSiteId(fragid) + siteind),
						      aSimTick );
#if 0
              BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
	        << FormatString("UDFREG % 6d ") << aSimTick
                << FormatString(" % 2d ")      << UDF_Binding::EwaldKSpaceForce_Code
                << FormatString(" S0 %5d ")    
		<< MSD_IF::GetExternalSiteIdForInternalSiteId(MSD_IF::GetIrreducibleFragmentFirstSiteId(fragid) + siteind) << " "
                << SiteEnergy << " "
                << "F0 " << SiteForce
                << EndLogLine;
#endif
              P3MEKSpaceEnergy += SiteEnergy;
              SiteForce *= MSD::SimpleRespaRatio;
              SourcePartialResultsIF.AddForce( siteind, SiteForce );              
          }

//           if( Platform::Topology::GetAddressSpaceId() == 0 )
//             {
//             TimeValue FinalKSpaceInterpolationTime = Platform::Clock::GetTimeValue();
//             TimeValue DifferenceInterpolation = FinalKSpaceInterpolationTime - StartKSpaceInterpolationTime;
//             ED_Emit_ControlTimeStamp( aSimTick, Tag::KSpaceInterpolation, 1u, 
//                                       DifferenceInterpolation.Seconds, DifferenceInterpolation.Nanoseconds );          
//             }

         KSpaceInterpolateFinis.HitOE( PKTRACE_KSPACE_INTERPOLATE,
                                       "KSpaceInterpolate",
                                       Platform::Topology::GetAddressSpaceId(),
                                       KSpaceInterpolateFinis );

          // EmitTimeStamp( aSimTick, Tag::KSpaceInterpolation_End, 1 );
          GlobalPartialResult.AddEnergy( UDF_Binding::EwaldKSpaceForce_Code, P3MEKSpaceEnergy );

#ifdef PK_PHASE5_SPLIT_COMMS
          KSpaceReduceStart.HitOE( PKTRACE_KSPACE_REDUCE,
                                   "KSpaceReduce",
                                   Platform::Topology::GetAddressSpaceId(),
                                   KSpaceReduceStart );         
          
          BegLogLine( PKFXLOG_P3MESEND | PKFXLOG_CHASEHANG ) << SimTick << "Got here" << EndLogLine;

          {
          unsigned long dummy1;
          unsigned long dummy2;
          Platform::Collective::GlobalMax( &dummy1, &dummy2 );
          }

          mClientState.mCommIF.ReduceExecute( aSimTick );

          BegLogLine( PKFXLOG_P3MESEND | PKFXLOG_CHASEHANG ) << SimTick << " Got here" << EndLogLine;
          
          KSpaceReduceFinis.HitOE( PKTRACE_KSPACE_REDUCE,
                                  "KSpaceReduce",
                                   Platform::Topology::GetAddressSpaceId(),
                                   KSpaceReduceFinis );         
#endif

          // now done inside Convolute(aVirial) in K-space
          //
          ///RTG.mGlobalVirial.mX += P3MEKSpaceEnergy;
          ///RTG.mGlobalVirial.mY += P3MEKSpaceEnergy;
          ///RTG.mGlobalVirial.mZ += P3MEKSpaceEnergy;


          /*-----------------------------------------------------------------------*/
          /*--                      Self Energy part                             --*/
          /*-----------------------------------------------------------------------*/

          double EwaldSelfEnergy = 
              ( MSD_IF::GetEwaldAlpha() * mClientState.mDynVarMgrIF->mBirthBoxAbsoluteRatio.mX ) * 
              P3ME<IrreducibleFragmentOperandIF,
              FragmentPartialResultsIF,
              P3ME_PLAN::FFT_FWD_TYPE,
              P3ME_PLAN::FFT_REV_TYPE>::EwaldSelfEnergyPrefactor;
          
          GlobalPartialResult.AddEnergy( UDF_Binding::EwaldSelfEnergyForce_Code, EwaldSelfEnergy );
          XYZ ewaldSelfEnergyVirial;
          double selfenergycontrib = EwaldSelfEnergy*(1./3.);
          
          selfenergycontrib *= MSD::SimpleRespaRatio;
          
          ewaldSelfEnergyVirial.mX = selfenergycontrib;
          ewaldSelfEnergyVirial.mY = selfenergycontrib;
          ewaldSelfEnergyVirial.mZ = selfenergycontrib;
          mClientState.mDynVarMgrIF->AddVirial( aRespaLevel, ewaldSelfEnergyVirial );
          
          //
          // end of K-space P3ME
          //----------------------------------------------

// #ifdef PK_PHASE2

// #ifdef PK_MPI_ALL
//           // Reduce the KSpaceForces, then add the results to the overall force.
//           mClientState.mDynVarMgrIF->ReduceKSpaceForces( aSimTick, aRespaLevel );
//           mClientState.mDynVarMgrIF->AddKSpaceTotalForces( aSimTick, aRespaLevel );
//           mClientState.mDynVarMgrIF->ZeroKSpaceForce();
          
// #elif defined (PK_BGL)
//           Platform::Memory::ExportFence();
//           ForceImportFence();
          
//           // Need this barrier, since we want to make sure 
//           // that the second core has invalidated it's cache
//           // before it start adding forces received from 
//           // other processors.
//           Platform::Control::Barrier();
          
//           SendNonLocalKSpaceForces( aSimTick, aRespaLevel );
// #endif
          
//           Platform::Control::Barrier();
          
// #if defined ( PK_BLADE )
//           // #define HACK_SIZE 64

//           double totalSent;
//           totalSent = 0.0;
             
//           double sentFromThisNode;
//           sentFromThisNode = 1.0 * KSpaceForcesSent.mVar;
          
//           BegLogLine( PKFXLOG_RECV_BARRIER )
//             << "P3ME::Send() before "
//             << " KSpaceForcesSent=" << KSpaceForcesSent.mVar
//             << " sentFromThisNode[0]="  << sentFromThisNode
//             << " totalSent=" << totalSent
//             << EndLogLine;
          
//           // This should be a reduction on ints
//           // Platform::Collective::FP_AllReduce_Sum((double *) &sentFromThisNode[0], (double *) &totalSent[0], HACK_SIZE);
//           totalSent = tr_AllReduce1( sentFromThisNode , 0 );
          
//           BegLogLine( PKFXLOG_RECV_BARRIER )
//             << "P3ME::Send() after "
//             << " sentFromThisNode=" << sentFromThisNode
//             << " totalSent=" << totalSent
//             << EndLogLine;

//           double totalRecv;
//           totalRecv = 0.0;
          
//           while( 1 )
//             {
//               Platform::Memory::ImportFence((void *)&KSpaceForcesRecv, sizeof(AtomicNative<int>));  
//               double tmpRcv;
//               tmpRcv = 1.0 * KSpaceForcesRecv.mVar;
//                 // This should be a reduction on ints
//               // Platform::Collective::FP_AllReduce_Sum(tmpRcv, totalRecv, HACK_SIZE );
//               totalRecv = tr_AllReduce1( tmpRcv , 0 );
//               if( fabs( totalRecv - totalSent ) < 1 )
//                 break;
//             }

//           BegLogLine( PKFXLOG_RECV_BARRIER )
//             << "P3ME::Send() "
//             << " recvThisNode=" << KSpaceForcesRecv.mVar
//             << " totalRecv=" << totalRecv
//             << EndLogLine;
          
//           Platform::Memory::ImportFence();
//           ForceExportFence();
// #endif
          
// #endif

          } // if SimpleRespa

          unsigned OTS = aSimTick;

          // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
          unsigned TimeStep = aSimTick / mClientState.mSimTicksPerTimeStep;

          // Check if this particular SimTick is the new TimeStep.
          if( TimeStep * mClientState.mSimTicksPerTimeStep == aSimTick )
            {
             BegLogLine( PKFXLOG_P3MESEND )
                 << "PlimptonLoops::Send() "
                 << " Emitting RDG Energy packets "
                 << " SimTick "  << aSimTick
                 << " TimeStep " << TimeStep
                 << EndLogLine;

                #if MSDDEF_ReplicaExchange
                  double Energy = GlobalPartialResult.mTotalEwaldKSpaceEnergy +
                      GlobalPartialResult.mTotalEwaldSelfEnergy ;
                  
                  mClientState.mDynVarMgrIF->AddToPotentialEnergy( aSimTick, Energy );
                #endif

            
             if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 ) 
               {
#ifdef FFT_IN_ONE_TASK_ONLY 
                 EmitEnergy( TimeStep, UDF_Binding::EwaldKSpaceForce_Code, GlobalPartialResult.mTotalEwaldKSpaceEnergy, 1 );
                 EmitEnergy( TimeStep, UDF_Binding::EwaldSelfEnergyForce_Code, GlobalPartialResult.mTotalEwaldSelfEnergy, 1 );
#else
#ifdef PK_PHASE5_SPLIT_COMMS
                 mClientState.mDynVarMgrIF->mInteractionStateManagerIF.SetKspaceEnergy( 0,
                                                                                        UDF_Binding::EwaldKSpaceForce_Code,
                                                                                        GlobalPartialResult.mTotalEwaldKSpaceEnergy );
                 
                 mClientState.mDynVarMgrIF->mInteractionStateManagerIF.SetKspaceEnergy( 1,
                                                                                        UDF_Binding::EwaldSelfEnergyForce_Code,
                                                                                        GlobalPartialResult.mTotalEwaldSelfEnergy );
#else                 
                 EmitEnergy( TimeStep, UDF_Binding::EwaldKSpaceForce_Code, GlobalPartialResult.mTotalEwaldKSpaceEnergy, 0 );
                 EmitEnergy( TimeStep, UDF_Binding::EwaldSelfEnergyForce_Code, GlobalPartialResult.mTotalEwaldSelfEnergy, 0 );
#endif
#endif
               }
            }
          
          // EmitTimeStamp( aSimTick, Tag::KSpace_End, 1 );

#ifdef PK_PHASE5_SPLIT_COMMS
          rts_dcache_evict_normal();
#endif

          BegLogLine( 0 )
            << "Leaving P3ME_Send MSD_IF::GetNumberOfEnergyReportingUDFs(): " << MSD_IF::GetNumberOfEnergyReportingUDFs()
            << EndLogLine;
          
          BegLogLine( PKFXLOG_P3MESEND | PKFXLOG_CHASEHANG )
            << aSimTick 
            << " PlimptonLoops::Send() "
            << "Finished..."
            << EndLogLine;

          KSpaceTotalFinis.HitOE( PKTRACE_KSPACE_TOTAL,
                                  "KSpaceTotal",
                                  Platform::Topology::GetAddressSpaceId(),
                                  KSpaceTotalFinis );
          }
          
    void
    Wait( int aSimTick )
      {
      // Loops better be done, no?
#ifdef PK_PHASE5_SPLIT_COMMS
#ifndef PHASE5_RUN_KSPACE_ON_CORE0
      Platform::Coprocessor::Join( mHandle );
#endif

      // This integer division will leave off remainder if this isn't an outermost timestep SimTick.
      unsigned TimeStep = aSimTick / mClientState.mSimTicksPerTimeStep;
      if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
        {
           mClientState.mDynVarMgrIF->mInteractionStateManagerIF.ReportKspaceEnergy( aSimTick );
        }

      mClientState.mDynVarMgrIF->mIntegratorStateManagerIF.AddInCore1Forces( aSimTick );
#endif
      return ;
      }
  
      };
  };


template<class DynVarMgrIF, class P3ME_PLAN, class P3MECommIF>
class P3MEKSpaceAnchor
  {

    /*
     * The anchor IS the Server.
     */
  private:

    class P3MEKSpaceEngine<DynVarMgrIF, P3ME_PLAN, P3MECommIF> mServer;

    /*
     *
     */
  public:

    void
    Init()
      {
      P3MEKSpaceEngine< DynVarMgrIF, P3ME_PLAN, P3MECommIF > mServer;
      }


    // Create a compiler recongnized instance of the Client Interface - should be zero byts.
    typedef class P3MEKSpaceEngine<DynVarMgrIF, P3ME_PLAN, P3MECommIF>::Client ClientIF;

    ClientIF &
    GetClientIF()
      {
      P3MEKSpaceEngine<DynVarMgrIF, P3ME_PLAN, P3MECommIF>::Client * cr;
      // = new  P3MEKSpaceEngine<DynVarMgrIF, P3ME_PLAN>::Client();
      pkNew( &cr, 1, __FILE__, __LINE__ );

      return( * cr  );
      }
  };

#if defined(P3ME_COMPILE)
template void P3ME<
 
   P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::IrreducibleFragmentOperandIF
   ,P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::FragmentPartialResultsIF
   ,P3ME_FFT_FWD_TO_USE
   ,P3ME_FFT_REV_TO_USE
   
>
::InKSpace
::Init(void) ;

template
void P3ME<
 P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::IrreducibleFragmentOperandIF
   ,P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::FragmentPartialResultsIF
   ,P3ME_FFT_FWD_TO_USE
   ,P3ME_FFT_REV_TO_USE
   
>
::InKSpace
::Convolute( XYZ& aVirial, const int aSimTick ) ;

template
void P3ME<
 P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::IrreducibleFragmentOperandIF
   ,P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::FragmentPartialResultsIF
   ,P3ME_FFT_FWD_TO_USE
   ,P3ME_FFT_REV_TO_USE
   
>
::InKSpace
::ConvoluteKx() ;
template
void P3ME<
  P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::IrreducibleFragmentOperandIF
   ,P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::FragmentPartialResultsIF
   ,P3ME_FFT_FWD_TO_USE
   ,P3ME_FFT_REV_TO_USE
   
>
::InKSpace
::ConvoluteKy() ;

template
void P3ME<
 P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::IrreducibleFragmentOperandIF
   ,P3MEKSpaceEngine<TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1>,P3ME_PLAN_IF,KSpaceCommMIF>::Client::FragmentPartialResultsIF
   ,P3ME_FFT_FWD_TO_USE
   ,P3ME_FFT_REV_TO_USE
   
>
::InKSpace
::ConvoluteKz() ;
#endif


#endif
