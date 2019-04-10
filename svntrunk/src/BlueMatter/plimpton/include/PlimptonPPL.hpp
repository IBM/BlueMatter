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
 

THIS CODE IS RETIRED



#ifndef _PLIMPTONPPL_HPP_
#define _PLIMPTONPPL_HPP_


//************************************************************************
// Something new.

/////////////////BGF Obsolete #include <BlueMatter/PlimptonEnergy.hpp>

#ifndef PKFXLOG_DOMAIN
#define PKFXLOG_DOMAIN 0
#endif

#ifndef PKFXLOG_CONFIG
#define PKFXLOG_CONFIG 0
#endif

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/random.hpp>

#if !defined(CYCLOPS)
#include <pk/setpri.hpp>
#endif

#include <pk/queue_set.hpp>
#include <PhasedPipeline/PPL_SystemIF.hpp>
#include <PhasedPipeline/PPL_ProtocolDriver.hpp>

// The result is expected to be a different type.

template<class tOperand>
class Plimpton_InputType
  {
  public:
    int        mRowFlag;
    int        mColFlag;
    int        mHomeVoxelId;
    int        mResultId;
    tOperand   mOperand;
  };

template<class tResult>
class Plimpton_OutputType
  {
  public:
    int      mResultId;
    tResult  mResult;
  };

template<class tOperand, class tResult, class PairwiseOp, class ReduceOp, int MaxOperands>
class Plimpton_PairwiseEngine
    : public FoxBase_DomainPipelineProcess
  {
  public:
    typedef Plimpton_InputType<tOperand>   InputType;
    typedef Plimpton_OutputType<tResult>   OutputType;
    class CreateMsgType
      {
      public:
        CreateMsgType(){}
        CreateMsgType ( int      aConnectorCount,
                        int      aPlimptonizorCount,
                        unsigned aInitialOuterTimeStep,
                        unsigned aNumberOfSimTicksPerOuterTimeStep )
          {
          mConnectorCount                   = aConnectorCount;
          mPlimptonizorCount                = aPlimptonizorCount;
          mInitialOuterTimeStep             = aInitialOuterTimeStep;
          mNumberOfSimTicksPerOuterTimeStep = aNumberOfSimTicksPerOuterTimeStep;
          }
        int      mConnectorCount;
        int      mPlimptonizorCount;
        unsigned mInitialOuterTimeStep;
        unsigned mNumberOfSimTicksPerOuterTimeStep;
      };

    int                    mOrdinal;
    int                    mConnectorCount;
    unsigned               mPlimptonizorCount;
    unsigned               mNumberOfSimTicksPerOuterTimeStep;

    // NOTE: OuterTimeStep is kept track of by incrementing with each PPL phase.
    // This assumes that the PlimptonPPL infrastructure is only used once per
    // outer timestep.
    // NEED: to be able to adust this when starting from non-zero OTS.

    unsigned mCurrentOuterTimeStep;  // keep track of phases to know which sim tick it is.
    unsigned mCurrentSimTick;        // CurrentOuterTimeStep * aNumberOfSimTicksPerOuterTimeStep


    class RowResults
      {
      public:
        tOperand                        mRowOperand;
        OutputType                      mOutputItem;
        int                             mHomeVoxelId;
        int                             mLastColumnProcess;
      };

//SLIME
//NEED: having the energy accumulators here and then
//NEED: using a special (-1) ResultId to send them home
//NEED: isn't a great solution. Should be a special channel for energy.
    double mChargeEnergyPartialSum;
    double mLJEnergyPartialSum;

    RowResults mRowResults[ MaxOperands ];
    int        RowResultNext;

    tOperand  mColOperands[ MaxOperands ];
    int       ColOperandNext;

    int OutputItemIndex;
    Plimpton_PairwiseEngine( int aOrdinal, CreateMsgType CMsg )
      {
      BegLogLine( 1 )

        << " Plimpton_Engine "
        << " Constructor called : Ordinal "
        << aOrdinal
        << EndLogLine;

      mOrdinal                          = aOrdinal;
      mConnectorCount                   = CMsg.mConnectorCount;
      mPlimptonizorCount                = CMsg.mPlimptonizorCount;
      mCurrentOuterTimeStep             = CMsg.mInitialOuterTimeStep;
      mNumberOfSimTicksPerOuterTimeStep = CMsg.mNumberOfSimTicksPerOuterTimeStep;
      mCurrentSimTick                   =   CMsg.mInitialOuterTimeStep
                                          * CMsg.mNumberOfSimTicksPerOuterTimeStep;

      ColOperandNext = 0;
      RowResultNext = 0;

      }

    ~Plimpton_PairwiseEngine( )
      {
      BegLogLine( PKFXLOG_DOMAIN )
        << " Plimpton_FeatureHypoMgr "
        << " Destructor called : Ordinal "
        << mOrdinal
        << EndLogLine;
      }

    PPLUSERIF_RC SetOpContext( short &UserBits, GlobjectReference Dummy )
      {
      ColOperandNext          = 0;
      RowResultNext           = 0;
      mChargeEnergyPartialSum = 0.0;
      mLJEnergyPartialSum     = 0.0;

      return( PPLUSERIF_RC_Accept );
      }

    PPLUSERIF_RC AddInputItem( InputType &aInputItem )
      {
      // Currently no processing is done in this routine.
      // Clearly, on adding on or both types of operand,
      // it would be possible to do some work.
      // Probably it would be best to only do work
      // when a Row operands is received against those Col operands
      // all ready in.  Then, on finalize processing, pick up
      // the remaining operations.

      if( aInputItem.mRowFlag )
        {
        assert( RowResultNext < MaxOperands );
        mRowResults[ RowResultNext ].mHomeVoxelId = aInputItem.mHomeVoxelId;
        mRowResults[ RowResultNext ].mRowOperand  = aInputItem.mOperand;
        mRowResults[ RowResultNext ].mOutputItem.mResult.mForcePartialSum.Zero(); // Should be reset or some such
        mRowResults[ RowResultNext ].mOutputItem.mResultId = aInputItem.mResultId;
        RowResultNext++;
        }


      if( aInputItem.mColFlag )
        {
        assert( ColOperandNext < MaxOperands );
        mColOperands[ ColOperandNext ] = aInputItem.mOperand;
        ColOperandNext++;
        }

      return( PPLUSERIF_RC_Accept );
      }

    PPLUSERIF_RC FinalizeProcessing()
      {
      // Currently ALL processing done here
      BegLogLine(0)
        << "PartProd_Pairwise "
        << " RowResults " << RowResultNext
        << " ColOperands " << ColOperandNext
        << EndLogLine;

      for( int r = 0; r < RowResultNext; r++ )
        {
        for( int c = 0; c < ColOperandNext; c++ )
          {
          tResult result;
          double  lje = 0.0;
          double  che = 0.0;
          PairwiseOp::Op( mOrdinal,
                          mCurrentSimTick,
                          result,
                          mRowResults[ r ].mRowOperand,
                          mColOperands[ c ],
                          lje,
                          che
                          );

          //Accumulate the energy.
          //While pair computation is done twice, energy is only accumulated once.
          if( mRowResults[ r ].mRowOperand.mAbsoluteSiteId < mColOperands[ c ].mAbsoluteSiteId  )
            {
            //NEED: the following doesn't work distributed VOXELS!!!!!!!!!!
            BegLogLine(0)
              << " LRFORCE "
              << " AbsId Row " <<  mRowResults[ r ].mRowOperand.mAbsoluteSiteId
              << " AbsId Col " <<  mColOperands[ c ].mAbsoluteSiteId
              << " lje " << lje
              << " che " << che
              << EndLogLine;

            mChargeEnergyPartialSum += che;
            mLJEnergyPartialSum     += lje;

            //NEED: get the slime out now that energy is reported via RDG system.
            //SLIME: PEA[ PWEngine_Ordinal ].mLennardJonesEnergy += lje;
            //SLIME: PEA[ PWEngine_Ordinal ].mChargeEnergy       += che;

            }


          ReduceOp::Op( mRowResults[ r ].mOutputItem.mResult,
                        mRowResults[ r ].mOutputItem.mResult,
                        result);

          }
        }

      // Local energy accumulators are done, so write out packets to
      // Raw Datgram Output

      // NEED: the sense of which SimTick or time step is a little wacked
      // since we only count PPL phases and assume that they match OTS clicks.

      double EwaldDummy = 0.0;

      ED_Emit_Energy_Nonbonded( mCurrentOuterTimeStep,  // this is developed by counting phases.
                                mOrdinal,
                                mPlimptonizorCount,
                                mChargeEnergyPartialSum,
                                mLJEnergyPartialSum,
                                EwaldDummy );


      OutputItemIndex = RowResultNext - 1;
      if( RowResultNext )
        return( PPLUSERIF_RC_AcceptWithOutputReady );
      else
        return( PPLUSERIF_RC_Accept ); // unclear whether this is a valid code path.
      }

    PPL_ORDINAL
    GetOutputItemDest()
      {
//SLIME      PPL_ORDINAL Rc = mRowResults[ OutputItemIndex ].mHomeVoxelId;
//SLIME      return( Rc );
      return(0);
      }

    PPLUSERIF_RC GetOutputItem( OutputType &aOutputItem )
      {
      if( OutputItemIndex >= 0 )
        {
        aOutputItem = mRowResults[ OutputItemIndex ].mOutputItem;
        OutputItemIndex--;
        return( PPLUSERIF_RC_AcceptWithOutputReady );
        }
      else
        {
        //SLIME: put the energy into the output item XYZ struct.
        aOutputItem.mResult.mForcePartialSum.mX = mChargeEnergyPartialSum;
        aOutputItem.mResult.mForcePartialSum.mY = mLJEnergyPartialSum;

        //SLIME: set the special result id that shows energy coming in.
        aOutputItem.mResultId = -1;

        // Increment for next expected OTS.
        mCurrentOuterTimeStep++;
        mCurrentSimTick = mCurrentOuterTimeStep * mNumberOfSimTicksPerOuterTimeStep;

        return( PPLUSERIF_RC_Accept );
        }
      }
  };

//============================================================

//************************************************************************

class Plimpton_Result
  {
  public:
    XYZ mForcePartialSum;
  };


class Plimpton_ReduceOp
  {
  public:
    static
    void
    IdentityValue(XYZ &Rc)
      {
      Rc.Zero();
      }
    static
    void
    Op( Plimpton_Result &Rc, Plimpton_Result &A, Plimpton_Result &B )
      {
      Rc.mForcePartialSum = A.mForcePartialSum + B.mForcePartialSum;
      }
  };

class Plimpton_Operand
  {
  public:
    int mAbsoluteSiteId;
    int SiteSpecTableIndex; // debatable whether to send this or get it from a table indexed by AbsSiteId
    XYZ mPosit;
    // It is probable that this structure can contain only a single identifying int and a position.
  };

class Plimpton_PairwiseOp
  {
  public:
    void
    static
    Op( int              &PWEngine_Ordinal,
        unsigned         &aCurrentSimTick,
        Plimpton_Result  &aResult,
        Plimpton_Operand &aOpRow,
        Plimpton_Operand &aOpCol,
        double           &lje,
        double           &che
        )
      {
      // Must zero() ... just in case we don't make it into the following block.
      aResult.mForcePartialSum.Zero();

      if( aOpRow.mAbsoluteSiteId != aOpCol.mAbsoluteSiteId )
        {
        XYZ fa, fb;

        UpdateInterMolecularForces( aCurrentSimTick,
                                    AccessMolSysDef().SiteSpecTable[ aOpRow.SiteSpecTableIndex ],
                                    AccessMolSysDef().SiteSpecTable[ aOpCol.SiteSpecTableIndex ],
                                    aOpRow.mPosit,
                                    aOpCol.mPosit,
                                    fa,
                                    fb,
                                    lje,
                                    che );

#ifdef NUMERICAL_STABILITY_CHECK  // Stability check - change the order of operands and see if all is ok
        XYZ xfa, xfb;
        double xlje, xche;
        UpdateInterMolecularForces( aCurrentSimTick,
                                    AccessMolSysDef().SiteSpecTable[ aOpCol.SiteSpecTableIndex ],
                                    AccessMolSysDef().SiteSpecTable[ aOpRow.SiteSpecTableIndex ],
                                    aOpCol.mPosit,
                                    aOpRow.mPosit,
                                    xfb,
                                    xfa,
                                    xlje,
                                    xche );
        assert( xfa == fa );
        assert( xfb == fb );
        assert( xlje == lje );
        assert( xche == che );

#endif
        aResult.mForcePartialSum  = fa;

        BegLogLine(0)
          << "Plimpton_PairwiseOp "
          << " AbsId Row " << aOpRow.mAbsoluteSiteId
          << " AbsId Col " << aOpCol.mAbsoluteSiteId
          << " PRow " << aOpRow.mPosit
          << " PCol " << aOpCol.mPosit
          << " FRow " << fa
          << EndLogLine;
        }
      }
  };


typedef Plimpton_PairwiseEngine<Plimpton_Operand,
                                Plimpton_Result,
                                Plimpton_PairwiseOp,
                                Plimpton_ReduceOp,
#if defined(CYCLOPS)
                                256
#else
                                (64*1024)           // number of posits that a plimptonizer can handle.
#endif

                                > XYZ_Plimpton;

//********************************************************************************
//********************************************************************************

    // the following would be plimptonized...
    // 1) Run through particle list creating a block of localy homed particles
    // 2) bcast in row and column the positions of atoms(functional groups) homed here
    // 3) Receive from all others in row and column
    // 4) Compute forces components ( now this node has for components for all atoms in row and column )
    // 5) bcast force components on row and column
    // 6) Receive bcast from all others in row and column of force components
    // ... should be ready to update velocity halfstep
    // 500 * 500 atom pairs in 180 x 180 and 50000 charges
    // Here we have to wait for elements to come in.
    // It is an issue to know when all are in.
    // Could use seqnos or non-bypassing messages... eg queues.
    // How light can queus be?????

  void
  Plimptonize(int VoxelFragmentCount,
              FragmentControlBlock *VoxelFragmentList,
              int aControlBlock,
              unsigned aCurrentSimTick,
              unsigned aSimTicksPerOuterTimeStep, // assumes Plimptonize only happens at OTS
              int      aIntegratorLevel )
  {
  // THIS ONLY WORKS WITH ALL ATOMS IN ONE ADDRESS SPACE
  static int Entry1 = 1;
#ifdef PK_MPI
  // The following will result in each processor receiving the entire set of
  // site positions but only computing 1/TaskCnt of the Force Partial Sums.
  // Communications could be reduced by figuring out a rectangle of processes given
  // the TaskCnt.  But, this should be ok for a while.
  static unsigned Rows   = 1 ;
  static unsigned Cols   = Platform::Topology::GetAddressSpaceCount() * Platform::Thread::GetCpusPerAddressSpaceCount();
#else
  static unsigned Rows   = 2;  // these numbers have to map to the number of nodes to be used by plimpton.
  static unsigned Cols   = 1 + ((Platform::Thread::GetCpusPerAddressSpaceCount() - 1) / Rows) ;
#endif
  static unsigned NumCells = Rows * Cols;
  static int Phase = 0;
  static int PhaseMask = 0x0F;

  BegLogLine(1)
    << "PlimptonPPL "
    << " Rows "              << Rows
    << " Cols "              << Cols
    << " NumCells "          << NumCells
    << " AddressSpaces "     << Platform::Topology::GetAddressSpaceCount()
    << " Cpus/AddressSpace " << Platform::Thread::GetCpusPerAddressSpaceCount()
    << EndLogLine;

  ED_Emit_Information_RunTimeConfiguration( 1u,          // Since the plimptonizer only works with 1 voxel right now
                                            NumCells ); // Total number of Nonbonded engines in use.

  ////////////////////SLIME: PEA_ActiveCount = NumCells;
  //////////////////
  ////////////////////NEED: to think about this zeroing of energy accumulators
  ////////////////////SLIME: for( int p = 0; p < PEA_ActiveCount; p++ )
  //////////////////  {
  //////////////////  PEA[0].mLennardJonesEnergy = 0.0;
  //////////////////  PEA[0].mChargeEnergy = 0.0;
  //////////////////  }
  //////////////////

  static QueueWriterSet< PhasedPipelineItem< XYZ_Plimpton::InputType >  >  PipelineEntrance;
  static QueueReaderSet< PhasedPipelineItem< XYZ_Plimpton::OutputType >   > OutputQueueSet;

  if( Entry1 )
    {
    Entry1=0;
       OutputQueueSet.Init( NumCells, "Plimpton_Engine" );

       QueueReaderRefArray OutputQueue_RefArr;

       OutputQueue_RefArr.ActiveCount = 1;
       OutputQueue_RefArr.ReaderRef[0].CommResource = 0;
       OutputQueue_RefArr.ReaderRef[0].QueueReaderSetPtr =
           (unsigned) OutputQueueSet.GetAddress();


       //------------------------------------------------------------------------
       // BRING UP PROC Plimpton_Driver

       BegLogLine(PKFXLOG_CONFIG) << " Starting Plimpton Driver. " << EndLogLine;

       QueueReaderRefArray Plimpton_Driver_RefArr;

       Plimpton_Driver_RefArr.ActiveCount = Rows * Cols;
       for( int r = 0; r < Rows; r++ )
         {
         for( int c = 0; c < Cols; c++ )
           {
           int t = c * Rows + r;

           Plimpton_Driver_RefArr.ReaderRef[t] =
             PhasedPipelineDriver< XYZ_Plimpton>::CreateProcess
               (
               t,  // which mpi proc
               PhasedPipelineDriver< XYZ_Plimpton>::CreateMsgType
                 (
                 0,                        // Stage Number
                 t,                        // ordinal in stage
                 1,                        // number of connectors to allocate ports for
                 "XHM",                    // Name of stage
                 OutputQueue_RefArr,       // output channel ref array.
                 PhaseMask,                     // Phase mask... number of phases allowed on pipeline
                 XYZ_Plimpton::CreateMsgType( 1,            // Connector Count
                                              Rows * Cols,  // Number of elements in plimptonizers
                                              aCurrentSimTick / aSimTicksPerOuterTimeStep,
                                              aSimTicksPerOuterTimeStep )

                 )
               );
           }
         }

#if (!defined(MPI)) && (!defined(CYCLOPS))
      sleep(1);
      // The following causes aix to cease using dynamic priority adjustments
      // which acheives the round robin scheduling we require.
      if( SchedTuneSelf() != 0 )
        {
        BegLogLine(1) << " SchedTuneSelf() Failed " << EndLogLine;
#if !defined(NO_SETPRI)
        assert(0) ;
#endif
#if 0
          fprintf(stderr, "WARNING: /var/setpri Failed! \n \
          logon as root and do the following:\n \
          xlc ../foxpk/setpri.c -o /var/setpri \n \
          chmod u+s /var/setpri \n" );
          fflush(stderr);
  // The following ugly bit prevents tests from running with
  // setpri not in place.
  #ifndef NO_SETPRI
  fprintf(stderr,"\n\nPLEASE PUT SETPRI IN PLACE OR SET: -D NO_SETPRI AT COMPILE\n\n");
  fprintf(stderr,"\n\nPLEASE PUT SETPRI IN PLACE OR SET: -D NO_SETPRI AT COMPILE\n\n");
  fprintf(stderr,"\n\nPLEASE PUT SETPRI IN PLACE OR SET: -D NO_SETPRI AT COMPILE\n\n");
  fflush(stderr);
  sleep(3);
  exit(-1);
  #endif
#endif
        }
      else
        {
        BegLogLine(1) << " SchedTuneSelf() Succeeded " << EndLogLine;
        }

#endif




       PipelineEntrance.Init( "CTL", 0, Plimpton_Driver_RefArr, 1024 );

       BegLogLine(1)
         << "Done with Plimpton inits "
         << EndLogLine;
    }

  PhasedPipelineItem< XYZ_Plimpton::InputType> PipelineInputMsg;
  PhasedPipelineItem< XYZ_Plimpton::OutputType> *PipelineOutputMsg;


  BegLogLine(1)
    << "About to push init message"
    << EndLogLine;


  // Initialize the pipeline.
  PipelineEntrance.ReserveAll();
  PipelineInputMsg.Header.Phase               = Phase;
  PipelineInputMsg.Header.FirstFlag           = 1;
  PipelineInputMsg.Header.FlushFlag           = 0;
  PipelineInputMsg.Header.UserBits            = 0;
  PipelineEntrance.SetCurrentItem( PipelineInputMsg );
  PipelineEntrance.PushAll();


  BegLogLine(1)
    << "Done inits, about to begin pushing operands"
    << EndLogLine;


  // Scan the local site table for resident sites
  // and send to appropriate plimptonizers
  PipelineInputMsg.Header.Phase               = Phase;
  PipelineInputMsg.Header.FirstFlag           = 0;
  PipelineInputMsg.Header.FlushFlag           = 0;
  PipelineInputMsg.Header.UserBits            = 0;

  int WaitForPhase = Phase;
  int TotalSiteCount = 0;

  // for each fragment in this voxel...
  for( int vfA = 0; vfA < VoxelFragmentCount; vfA++)
    {
    // for each site in that fragment
    for( int vfAi = 0;
             vfAi < VoxelFragmentList[ vfA ].AccessFragSpec().NumberInternalSites;
             vfAi++ )
      {
      TotalSiteCount++;
      int AbsoluteSiteId =  VoxelFragmentList[ vfA ].GetAbsoluteSiteId( vfAi );

      // Load this site into a Plimptonizer payload and prepare to ship.
      XYZ_Plimpton::InputType     payload;

      payload.mHomeVoxelId                = 0;
      payload.mResultId                   = AbsoluteSiteId;
      payload.mOperand.mPosit             = VoxelFragmentList[ vfA ].AccessPosition( vfAi );
      payload.mOperand.SiteSpecTableIndex = VoxelFragmentList[ vfA ].GetSiteSpecTableIndex( vfAi );
      payload.mOperand.mAbsoluteSiteId    = AbsoluteSiteId;


      // Send to app pliptonizer engine
      for( int x = 0; x < Rows; x ++ )
        {
        for( int y = 0; y < Cols; y++ )
          {
          // On the way back, we'll not be using the fragment control blocks, but rather absolute id.
          // We could of course just use the actual address of the LocalSiteTable entry.
          payload.mRowFlag                    = (((AbsoluteSiteId / Cols) % Rows) == x) ? 1 : 0;
          payload.mColFlag                    = ((AbsoluteSiteId % Cols) == y) ? 1 : 0;
          if( payload.mRowFlag || payload.mColFlag )
            {
            PipelineEntrance.Reserve( x + y * Rows );
            PipelineInputMsg.FMLSeq.Middle.PayloadItem.Flatten( payload );
            PipelineEntrance.SetCurrentItem( PipelineInputMsg );
            PipelineEntrance.Push();
            }
          }
        }
      }
    }


  BegLogLine(1)
    << "Done pushing operands, pushing flush."
    << " TotalSiteCount "
    << TotalSiteCount
    << EndLogLine;


  // Put other stuff into message - almost all of which could move into PerXActContext.
  Phase = (Phase + 1) & PhaseMask;
  PipelineInputMsg.Header.Phase     = Phase;
  PipelineInputMsg.Header.FirstFlag = 1;
  PipelineInputMsg.Header.FlushFlag = 1;
  PipelineInputMsg.Header.UserBits  = 0;

  PipelineEntrance.ReserveAll();
  PipelineEntrance.SetCurrentItem( PipelineInputMsg );
  PipelineEntrance.PushFlushAll();

  Phase = (Phase + 1) & PhaseMask;

  BegLogLine(1)
    << "PlimptonizerCNTL "
    << "Done pushing flush, waiting for results. "
    << EndLogLine;

  int  TotalResultsCount = 0; // we'll want to see this go to total site count * row count

  int FlushCount = 0;
  while( FlushCount < (Rows*Cols) )
    {
    while ( NULL == (PipelineOutputMsg = OutputQueueSet.PollNextItem() ) )
#if defined(CYCLOPS)
      Platform::Thread::Yield() ;
#else
      usleep(10000)
#endif
      ;

    BegLogLine( 0 )
      << "PlimptonizerCNTL "
      << " Got PipelineOutputMsg "
      << " Phase "     << PipelineOutputMsg->Header.Phase
      << " FirstFlag " << PipelineOutputMsg->Header.FirstFlag
      << " FlushFlag " << PipelineOutputMsg->Header.FlushFlag
      << EndLogLine;

    if(  PipelineOutputMsg->Header.FlushFlag )
      {
      OutputQueueSet.ReleaseItem();  // release the flush packet
      OutputQueueSet.Suspend();      // turn off that queue
      FlushCount++;                  // bump flush count
      }
    else if(  PipelineOutputMsg->Header.FirstFlag )
      {
      // We don't do anything with the first packet
      OutputQueueSet.ReleaseItem();
      }
    else if( !  PipelineOutputMsg->Header.FirstFlag )
      {
      TotalResultsCount++;
      XYZ result =  PipelineOutputMsg->FMLSeq.Middle.PayloadItem.AsTyped().mResult.mForcePartialSum;
      int AbsoluteSiteId =  PipelineOutputMsg->FMLSeq.Middle.PayloadItem.AsTyped().mResultId;

      //NEED: to take out the slime now that energy is reported via RDG.
      //SLIME: we're bringing back the energy in the result structure
      if( AbsoluteSiteId == -1 )
        {
        //SLIME: put the energy partial sums where they go
        //////////////////// PEA[0].mChargeEnergy            += result.mX;
        /////////////////// PEA[0].mLennardJonesEnergy      += result.mY;
        }
      else //SLIME: the path for the real force partial sums
        {
        //NEED: this shouldn't directly reach into LocalSiteTable... put that in a class!

        LocalSiteTable::AccessForce( AbsoluteSiteId, aCurrentSimTick, aIntegratorLevel ) =
          LocalSiteTable::AccessForce( AbsoluteSiteId, aCurrentSimTick, aIntegratorLevel ) + result;
        }

      BegLogLine( 0 )
        << " Plimptonizer:: Force Component for: "
        << AbsoluteSiteId
        << " Force Received "
        << result
        << " Force In Table: "
        << LocalSiteTable::AccessForce( AbsoluteSiteId, aCurrentSimTick, aIntegratorLevel )
        << EndLogLine;

      OutputQueueSet.ReleaseItem();
      }
    else
      assert( 0 );  // don't know what would be going on here.


    } // End while pipeline gurges up an Item.

  BegLogLine(1)
    << "PlimptonizerCNTL "
    << "Done waiting for results."
    << EndLogLine;


  OutputQueueSet.ResumeAll();

  BegLogLine( 0 )
    << "Plimptonizer "
    << " TotalResultsCount "
    << TotalResultsCount
    << " Expected "
    << TotalSiteCount * Rows
    << EndLogLine;


  }

#endif
