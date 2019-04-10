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
 #ifndef __P2__HPP__
#define __P2__HPP__

#if !defined(LEAVE_SYMS_EXTERN) 
unsigned SimTick;
#endif

#ifndef PKFXLOG_MANAGE_SPACIAL_DECOMP
#define PKFXLOG_MANAGE_SPACIAL_DECOMP ( 0 )
#endif

#ifndef PKFXLOG_CHECKSITEDISPLACEMENTVIOLATION
#define PKFXLOG_CHECKSITEDISPLACEMENTVIOLATION ( 0 )
#endif

#ifndef PKFXLOG_RS_SIGNAL_SET
#define PKFXLOG_RS_SIGNAL_SET ( 0 )
#endif

#ifndef PKFXLOG_RESPA_STEPS
#define PKFXLOG_RESPA_STEPS ( 0 )
#endif

//ME: enables REMD trace data.
#ifndef PKFXLOG_REPLICA_EXCHANGE
#define PKFXLOG_REPLICA_EXCHANGE ( 0 )
#endif

#ifndef PKFXLOG_VERLET_ASSIGN_RECOOK_SYNC
#define PKFXLOG_VERLET_ASSIGN_RECOOK_SYNC ( 0 )
#endif

#ifndef PKFXLOG_REDUCE_FORCES
#define PKFXLOG_REDUCE_FORCES ( 0 )
#endif

#ifndef PKFXLOG_CORRUPT_FFT_HEADERS
#define PKFXLOG_CORRUPT_FFT_HEADERS ( 0 )
#endif

#ifndef PKFXLOG_BROADCAST
#define PKFXLOG_BROADCAST ( 0 )
#endif

#ifndef PKFXLOG_DOTRANSPORT
#define PKFXLOG_DOTRANSPORT ( 0 )
#endif

#ifndef PKFXLOG_VELOCITY_RESAMPLE
#define PKFXLOG_VELOCITY_RESAMPLE ( 0 )
#endif

#ifndef PKFXLOG_INTEGRATOR_TRACE
#define PKFXLOG_INTEGRATOR_TRACE ( 0 )
#endif

#ifndef PKFXLOG_FRAGMENT_STATUS
#define PKFXLOG_FRAGMENT_STATUS  ( 0 )
#endif

#ifndef PKFXLOG_DUMPPARTITIONLIST
#define PKFXLOG_DUMPPARTITIONLIST ( 0 )
#endif

#ifndef PKFXLOG_VOXELMANAGER_ENTRY
#define PKFXLOG_VOXELMANAGER_ENTRY ( 0 )
#endif

#ifndef PKFXLOG_RUN_PARAMS
#define PKFXLOG_RUN_PARAMS ( 1 )
#endif

#ifndef PKFXLOG_DONESETUP
#define PKFXLOG_DONESETUP   ( 0 )
#endif

#ifndef PKFXLOG_VOXELTHREAD_BARRIER
#define PKFXLOG_VOXELTHREAD_BARRIER  ( 0 )
#endif

#ifndef PKFXLOG_VOXELRESPA
#define PKFXLOG_VOXELRESPA           ( 0 )
#endif

#ifndef PKFXLOG_VOXELREALSPACE
#define PKFXLOG_VOXELREALSPACE       ( 0 )
#endif

#ifndef PKFXLOG_VOXELKSPACE
#define PKFXLOG_VOXELKSPACE          ( 0 )
#endif

#ifndef PKFXLOG_TOPOFMAINLOOP
#define PKFXLOG_TOPOFMAINLOOP        ( 0 )
#endif

#ifndef PKFXLOG_PKMAIN
#define PKFXLOG_PKMAIN               ( 0 )
#endif

#ifndef PKTRACE_SIMTICK_INIT_PHASE
#define PKTRACE_SIMTICK_INIT_PHASE ( 30 ) // trace the first X timesteps
#endif

#ifndef PKTRACE_SIMTICK_START
#define PKTRACE_SIMTICK_START      ( RTG.GetNumberOfSimTicks() - 200 )
#endif
#ifndef PKTRACE_SIMTICK_END
#define PKTRACE_SIMTICK_END        ( RTG.GetNumberOfSimTicks() )
#endif

#ifndef PKTRACE_SIMTICK_PERIOD
#define PKTRACE_SIMTICK_PERIOD     ( 1 )
#endif

#ifndef PKTRACE_SIMTICK_COUNT
#define PKTRACE_SIMTICK_COUNT      ( 1 )
#endif

#if MSDDEF_ReplicaExchange
#include <BlueMatter/ReplicaExchange.hpp>
#endif

static PNBE_TYPE::ClientIF  RealSpaceClient;

static RealSpaceSignalSet RealSpaceClientSignalSet;

#ifdef MSDDEF_EWALD
static EwaldKSpaceAnchor<DynamicVariableManager>::ClientIF KSpaceClient;
#define KSpaceEnabled
#endif

#ifdef MSDDEF_P3ME
static P3MEKSpaceAnchor<DynamicVariableManager, P3ME_PLAN_IF, KSpaceCommMIF >::ClientIF KSpaceClient;
#define KSpaceEnabled
#endif

#include <pk/pktrace.hpp>

// ME
#ifdef PERFORMANCE_COUNTERS
#include "/bgwhome1/sextonjc/bglusr/include/apc.h"
//#include "/bgl/bglusr/include/apc.h"
extern "C"
{
  void ApcInit();

  void ApcInitWithRank(int rank);

  void ApcFinalize();

  void ApcStart(int set);

  void ApcStop(int set);
}
#endif

#include <BlueMatter/Tag.hpp>

#define DVS_SUBSTRING "SubPartId"

static int DoneFlag;

#ifdef PK_BGL
#if defined(KSpaceEnabled)
static
void
P3ME_GetNodeIDList( const XYZ& vmin, const XYZ& vmax, int idlist[], int& idnum )
{
  KSpaceClient.GetNodeIDList( vmin, vmax, idlist, idnum );
}
#endif
#endif

static void *
VoxelManagerThread()
{
#pragma execution_frequency( very_low )
  BegLogLine( PKFXLOG_SETUP )
    << "Entering VoxelManagerThread()"
    << EndLogLine;

  Platform::Control::Barrier();
  int DebugNode = Platform::Topology::GetAddressSpaceId();

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  Platform::Collective::Broadcast( (char *) &RTG, sizeof( RTG ), 0 );

#ifdef KSpaceEnabled
  KSpace_NBE.Init();
  KSpaceClient = KSpace_NBE.GetClientIF();
#endif

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  DynVarMgrIF.Init();

#if MSDDEF_ReplicaExchange
  if( RTG.mVelocityResampleTargetTemperature == 0 )
    PLATFORM_ABORT("Running REMD without a set target temperature");

  int myLocalRank = -1;
  int commSize = -1;

  MPI_Comm_rank(Platform::Topology::cart_comm, &myLocalRank);
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);


  // ME: Mapping info  myGlobalRank %d to  myReplicaID %d
  int myReplicaID = Platform::Topology::mSubPartitionId;
  int myGlobalRank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &myGlobalRank);

  BegLogLine( PKFXLOG_REPLICA_EXCHANGE)
    << " myGlobalRank " << myGlobalRank
    << " myReplicaID " << myReplicaID
    << EndLogLine;
  // ME: -------------------------------------------------

  int mNumReplica = Platform::Topology::mSubPartitionCount;

  int NumberOfNodesInReplica = Platform::Topology::GetAddressSpaceCount();

  double *mPotentialEnergyRecv = new double[commSize];  // recv buffer
  double *mPotentialEnergySend = new double[commSize];   // send buffer
  double *mPotentialEnergy = new double[ mNumReplica ];   // temperature-arra

  double *mTemperatureRecv = new double[commSize];  // recv buffer
  double *mTemperatureSend = new double[commSize];   // send buffer
  double *mTemperature = new double[ mNumReplica ];   // temperature-array
  assert( mPotentialEnergyRecv );
  assert( mPotentialEnergySend );
  assert( mPotentialEnergy );

  assert( mTemperatureRecv );
  assert( mTemperatureSend );
  assert( mTemperature);

  for(int i=0;  i<commSize; i++)
    {
      mTemperatureSend[ i ] = 0.0;
    }

  if( myLocalRank == 0 )
    {
      int index = Platform::Topology::mSubPartitionId * NumberOfNodesInReplica;
      assert( index >= 0 && index < commSize );
      mTemperatureSend[ index ] = RTG.mVelocityResampleTargetTemperature;
    }

  MPI_Allreduce( mTemperatureSend, mTemperatureRecv, commSize, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  int counter = 0;
  for(int i=0; i<commSize; i=i+NumberOfNodesInReplica )
    {
      assert( counter >= 0 && counter < mNumReplica );
      mTemperature[ counter ] = mTemperatureRecv[i];
      counter++;
    }

  assert( counter == mNumReplica );

  delete[] mTemperatureRecv;
  delete[] mTemperatureSend;

  // sanity-test:  temp should be from lower to higher
  int status;
  int nIter = mNumReplica-1;

  // ME: sort the temp in decreasing order
  int *mIndex = new int [ mNumReplica ];

  for(int s=0; s<mNumReplica; s++){ mIndex[s]=s; }

  while(nIter)
    {
      status=0;

      for(int s=0; s<nIter; s++)
        {
          if(mTemperature[s+1]<mTemperature[s])
            {
              int tempIndex = mIndex[s+1];
              int temp = mTemperature[s+1];

              mTemperature[s+1] = mTemperature[s];
              mIndex[s+1]=mIndex[s];

              mTemperature[s] = temp;
              mIndex[s]=tempIndex;

              status = s;
            }
        }
      nIter = status;
    }
  // ME: This is for debugging purpose only...
  // ME: check if all the processors have identical Temperature arrays
  /*
    int* mTemperatureArraySend = new int[commSize*mNumReplica];
    int* mTemperatureArrayRecv = new int[commSize*mNumReplica];

    for(int i=0; i<commSize*mNumReplica; i++)
    {
    mTemperatureArraySend[i]=0.;
    mTemperatureArrayRecv[i]=0.;
    }

    int tempCounter = 0;
    int myReplica  = Platform::Topology::mSubPartitionId;

    for(int i=myReplica*mNumReplica; i< myReplica*(mNumReplica+1); i++)
    {
    mTemperatureArraySend[i] = mTemperature[tempCounter];
    tempCounter++;
    }

    MPI_Allreduce( mTemperatureArraySend, mTemperatureArrayRecv,8*mNumReplica*commSize, MPI_BYTE, MPI_BOR, MPI_COMM_WORLD);

    for(int i=0; i<commSize*mNumReplica; i++)
    {
    assert(mTemperatureArrayRecv[i]<= E-08);
    }

    delete[] mTemperatureArraySend;
    delete[] mTemperatureArrayRecv;
  */


#if Stochastic
  REMD_Stochastic *mRemdStandard = new REMD_Stochastic( myGlobalRank,
                                                        mTemperature,
                                                        (unsigned*)mIndex,
                                                        (unsigned&)mNumReplica,
                                                        (unsigned&)RTG.mSwapPeriodOTS,
                                                        (unsigned&)Platform::Topology::mSubPartitionId,
                                                        (unsigned&)RTG.mReplicaExchangeRandomSeed,
                                                        (unsigned&)RTG.mNumOfTemperatureAttempts);
#else
  unsigned zero = 0;
  REMD_Standard *mRemdStandard = new REMD_Standard(myGlobalRank,
                                                   mTemperature,
						   (unsigned*)mIndex,
						   (unsigned&)mNumReplica,
						   (unsigned&)RTG.mSwapPeriodOTS,
						   (unsigned&)Platform::Topology::mSubPartitionId,
						   (unsigned&)RTG.mReplicaExchangeRandomSeed,
						   zero);
#endif

  delete [] mIndex;

#endif

  int RespaLevel = MSD_IF::GetNumberOfRespaLevels() - 1;

  unsigned TimeStep = RTG.mStartingTimeStep; // If respa is being used, this is the outer most timestep or lowest frequency iterator

  // If respa is being used, this is the inner most timestep or highest frequency iterator
  SimTick = TimeStep * MSD_IF::GetSimTicksPerTimeStep();
  unsigned LastSimTickOfTimeStepFlag = 1; // We begin as if initial conditions are the end of the last, possibly respa loop iter.

#ifdef KSpaceEnabled
  {
    /**
     *    Connect EwaldKspaceEngine
     */

    BegLogLine( PKFXLOG_SETUP )
      << "p2.hpp:: About to call KSpaceClient.Connect"
      << EndLogLine;

    KSpaceClient.Connect(& DynVarMgrIF,
			 SimTick,
			 MSD_IF::GetSimTicksAtRespaLevel()[ RespaLevel ],
			 RespaLevel );
  }
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
  ORBManagerIF.Init( & DynVarMgrIF );

  ORBNode* root = ORBManagerIF.GetOrbRoot();

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  DynVarMgrIF.InitPostOrbState( root );
#endif

#ifdef KSpaceEnabled
  KSpaceClient.InitEwaldSelfEnergy();

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: Done with KSpaceClient.InitEwaldSelfEnergy()"
    << EndLogLine;
#endif


#ifndef BM_SPATIAL_DECOMPOSITION
  DynVarMgrIF.GlobalizePositionsVelocities( 0, 0 ); // the zeros are probably not what we want for a restart in the long run

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: Done with DynVarMgrIF.GlobalizePositionsVelocities()"
    << EndLogLine;
#endif

#if MSDDEF_DoShakeRattle
  {
    UDF_State::Set_Shake( MSD_IF::GetShakeMaxIterations(), MSD_IF::GetShakeTolerance() );
    UDF_State::Set_Rattle( MSD_IF::GetRattleMaxIterations(), MSD_IF::GetRattleTolerance() );
  }
#endif

  //*********************************************************************
  // Declare TraceClient on the stack so their constructor is called
  //*********************************************************************

  // ME:  TraceClint for the REMD measurements
#ifndef EveryNTimeSteps
#define EveryNTimeSteps 20
#endif

  TraceClient EveryNTimeStepsStart;
  TraceClient EveryNTimeStepsFinis;

  TraceClient RemdSwappingStepStart;
  TraceClient RemdSwappingStepFinis;

  TraceClient RemdAllreduceEnergyStart;
  TraceClient RemdAllreduceEnergyFinis;

  TraceClient RemdCopyEnergyStart;
  TraceClient RemdCopyEnergyFinis;

  TraceClient GuardZoneViolationReductionStart;
  TraceClient GuardZoneViolationReductionFinis;

  TraceClient RealSpaceStart;
  TraceClient RealSpaceFinis;

  TraceClient BondedStart;
  TraceClient BondedFinis;

  TraceClient ShakeStart;
  TraceClient ShakeFinis;

  TraceClient RattleStart;
  TraceClient RattleFinis;

  TraceClient SimtickLoopStart;
  TraceClient SimtickLoopFinis;

  TraceClient RealSpaceAddForcesStart;
  TraceClient RealSpaceAddForcesFinis;

  TraceClient RealSpaceDualCoreStart;
  TraceClient RealSpaceDualCoreFinis;
  //*********************************************************************

  BegLogLine( PKFXLOG_VOXELTHREAD_BARRIER )
    << "VoxelThread Exiting barrier "
    << EndLogLine;

#if ( MSDDEF_DoNSQCheck )
  {
    MDVM dvsCheckMdvm;
    dvsCheckMdvm.Init();
    dvsCheckMdvm.SetSiteForRegMap( 0, 0 );
    dvsCheckMdvm.SetSiteForRegMap( 1, 1 );

    for( int fragmentId1 = 0; fragmentId1 < DynVarMgrIF.GetIrreducibleFragmentCount(); fragmentId1++ )
      {
	int firstSiteId = DynVarMgrIF.GetNthIrreducibleFragmentFirstSiteIndex( fragmentId1 );
	for( int siteId = firstSiteId; siteId < DynVarMgrIF.GetNthIrreducibleFragmentSiteCount( fragmentId1 ); siteId++ )
	  {
	    for( int fragmentId2 = 0; fragmentId2 < DynVarMgrIF.GetIrreducibleFragmentCount(); fragmentId2++ )
	      {
		int firstSiteId1 = DynVarMgrIF.GetNthIrreducibleFragmentFirstSiteIndex( fragmentId2 );
		for( int siteId1 = firstSiteId1; siteId1 < DynVarMgrIF.GetNthIrreducibleFragmentSiteCount( fragmentId2 ); siteId1++ )
		  {
		    if(siteId != siteId1)
		      {
			dvsCheckMdvm.SetPositionRegister( MDVM::SiteA, DynVarMgrIF.GetPosition( siteId ));
			dvsCheckMdvm.SetPositionRegister( MDVM::SiteB, DynVarMgrIF.GetPosition( siteId1 ));
			UDF_Binding::UDF_NSQCheckSitesInBox_Execute( dvsCheckMdvm );
		      }
		  }
	      }
	  }
      }
  }
#endif

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: About to MSD_TYPED_STD_*Driver.Init()"
    << EndLogLine;

  // Keep the local fragments information in the MSD_IF for now.
  MSD_IF::Init();
  MSD_TYPED_STD_BondedForceDriver.Init( DynVarMgrIF );

  EnergyManagerIF.Init( UDF_Binding::UDF_CODE_COUNT, BMM_RDG_FlushPeriod );

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

#ifdef MOLECULE_DRIVER
  MSD_TYPED_STD_MolDriver.Init( DynVarMgrIF );
#endif

#if MSDDEF_DoShakeRattle
  MSD_TYPED_STD_GroupInitDriver.Init( DynVarMgrIF );
  MSD_TYPED_STD_GroupRattleDriver.Init( DynVarMgrIF );
  MSD_TYPED_STD_GroupShakeDriver.Init( DynVarMgrIF );
#endif

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: Done with MSD_TYPED_STD_*Driver.Init()"
    << EndLogLine;

#if( MSDDEF_DoShakeRattle && MSDDEF_DoWaterInit )
  {
    // This call updates positions!!!
    MSD_TYPED_STD_GroupInitDriver.Execute(DynVarMgrIF, SimTick, RespaLevel );
#if ( defined( PK_PHASE1_5 ) )
    DynVarMgrIF.GlobalizePositions( SimTick, RespaLevel );
#elif ( defined( BM_SPATIAL_DECOMPOSITION ) )
      
    // CWO/Alex 8/2/06 - fix bug here -- ensures that node fragment ownership
    //                   data structures are consistent after Rattle
    DynVarMgrIF.ManageSpacialDecompositionAllToAll( SimTick ); 
    MSD_TYPED_STD_BondedForceDriver.UpdateLocalSitesParams( DynVarMgrIF );
#if MSDDEF_DoShakeRattle
    MSD_TYPED_STD_GroupRattleDriver.UpdateLocalSitesParams( DynVarMgrIF );
    MSD_TYPED_STD_GroupShakeDriver.UpdateLocalSitesParams( DynVarMgrIF );
#endif
     
    DynVarMgrIF.GlobalizePositions( SimTick, RespaLevel, 1 );
#endif
  }
#endif

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: About to ZeroForceRegisters()"
    << EndLogLine;

  DynVarMgrIF.ZeroForceRegisters( 0, MSD_IF::GetNumberOfRespaLevels() - 1 );

#if MSDDEF_DoPressureControl
  {
    MSD_TYPED_LFD_UpdateCenterOfMassPositionVelocityDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );
  }
#endif

  //*********************************************************************

  //******************************************************************
  //****   Set up for dynamics.
  //****   Since we have velocities, we need forces to enter the
  //****   main dynamics loop.  So, drive all relevant force generating
  //****   infrastructures to develop their force partial sums based
  //****   on current positions.
  //****   Note that the RespaLevel the force will be computed at
  //****   must be used in the call so that the forces will be placed
  //****   in the correct accumulator (and thus integrated with the
  //****   correct timestep for that level.  -1 drops force term out.)
  //******************************************************************

  int MyAddressSpaceId = Platform::Topology::GetAddressSpaceId();

#if MSDDEF_NOSE_HOOVER
  NHC::Init( MSD_IF::GetConvertedInnerTimeStep() );

#if MSDDEF_DoPressureControl
  {
    DirectPistonMDVM< DynamicVariableManager > P_MDVM( DynVarMgrIF );
    UDF_Binding::UDF_NoseHooverInit_Execute( P_MDVM );

    if( !NoseHooverRestart )
      UDF_Binding::UDF_NoseHooverInitThermostatPosVel_Execute( P_MDVM );
  }
#endif

  MSD_TYPED_LFD_NoseHooverInitDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

  if( !NoseHooverRestart )
    MSD_TYPED_LFD_NoseHooverInitThermostatPosVelDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );
#endif

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: About to MSD_TYPED_LFD_UpdateKineticEnergyDriver::Execute()"
    << EndLogLine;

  MSD_TYPED_LFD_UpdateKineticEnergyDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

#if ( MSDDEF_NOSE_HOOVER && MSDDEF_DoPressureControl )
  DirectPistonMDVM< DynamicVariableManager > P1_MDVM( DynVarMgrIF );
  UDF_Binding::UDF_NoseHooverEnergy_Execute( P1_MDVM );

  if( MyAddressSpaceId == 0 )
    EmitEnergy( TimeStep, UDF_Binding::NoseHooverEnergy_Code, P1_MDVM.GetEnergy(), 1 );
#endif

  // This is for the virial which comes from the dvs.
  // If it's set in the dvs then use it and stump on GlobalVirial;

  if( MyAddressSpaceId == 0 )
    {
      // Calculate and emit Constraint info
      int NumberOfConstraints         = MSD_IF::GetNumberOfConstraints();
      int NumberOfAtoms               = MSD_IF::GetNumberOfSites();

      // Should be done in probspec
      int NumberOfDegreesOfFreedom    = 3 * NumberOfAtoms - NumberOfConstraints;

      ED_Emit_InformationConstraints( TimeStep, NumberOfConstraints, NumberOfDegreesOfFreedom, 0 );
    }

  /**
   *    Connect PlimptonEngine
   **/
  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: About to PNBE.Init()"
    << EndLogLine;

  PNBE.Init();
  RealSpaceClient = PNBE.GetClientIF();

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: About to RealSpaceClient.Connect()"
    << EndLogLine;

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  RealSpaceClient.Connect( & DynVarMgrIF,
#ifdef BM_SPATIAL_DECOMPOSITION
                           & ORBManagerIF,
#endif
                           RealSpaceClientSignalSet,
                           SimTick,
                           RespaLevel );

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  BegLogLine( PKFXLOG_SETUP )
    << "VoxelManagerThread():: About to GlobalizePosition"
    << EndLogLine;

#if ( defined( PK_PHASE1_5 ) )
  DynVarMgrIF.GlobalizePositions( SimTick, RespaLevel );
#elif ( defined( BM_SPATIAL_DECOMPOSITION ) )
  DynVarMgrIF.GlobalizePositions( SimTick, RespaLevel, 1 );
#endif

#ifdef ROLLING_POINT_REGRESSION
#ifdef BM_SPATIAL_DECOMPOSITION
  DynVarMgrIF.SendPosVelToP1_5( SimTick, 1 );
  Platform::Control::Barrier();
#elif defined( PK_PHASE1_5 )
  if( Platform::Topology::GetAddressSpaceId() == 0 )
    DynVarMgrIF.ReceivePosVelFromP5( SimTick );
  
  int ReduceSize1 = NUMBER_OF_SITES * ( sizeof( XYZ ) / sizeof( double ) );
  
  MPI_Bcast( DynVarMgrIF.mPosit, ReduceSize1, MPI_DOUBLE, 0, Platform::Topology::cart_comm );
  
  MPI_Bcast( DynVarMgrIF.mVelocity, ReduceSize1, MPI_DOUBLE, 0, Platform::Topology::cart_comm );
  Platform::Control::Barrier();
#endif
#endif

#ifdef KSpaceEnabled
  {
    //     KSpaceEvaluationStart.HitOE( PKTRACE_P2_KSPACE_CONTROL,
    //                                  "P2_KSpace_Control",
    //                                  MyAddressSpaceId,
    //                                  KSpaceEvaluationStart );

    BegLogLine( PKFXLOG_VOXELKSPACE )
      << SimTick
      << " VoxelThread Done Sending "
      << EndLogLine;

    KSpaceClient.Send( SimTick, RespaLevel, 1 );

    BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

    BegLogLine( PKFXLOG_SETUP )
      << SimTick
      << " p2.hpp:: Done with KSpaceClient.Send()"
      << EndLogLine;

    //     KSpaceEvaluationFinis.HitOE( PKTRACE_P2_KSPACE_CONTROL,
    //                                  "P2_KSpace_Control",
    //                                  MyAddressSpaceId,
    //                                  KSpaceEvaluationFinis );
  }
#endif

  // **********************************************************************
  // Log lines putting out run information from Node 0
  // **********************************************************************
  
  if ( Platform::Topology::GetAddressSpaceId() == 0 )
    {
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "SourceId: "
        << MSD_IF::GetSourceId()
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "ExecutableId: "
        << EXECUTABLEID
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "JobId: "
        << Platform::JobInfo::JobId()
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "NumberOfOuterTimeSteps: "
        << RTG.mNumberOfOuterTimeSteps
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "InnerTimeStepInPicoSeconds: "
        << MSD_IF::GetInnerTimeStepInPicoSeconds()
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "OuterTimeStepInPicoSeconds: "
        << MSD_IF::GetOuterTimeStepInPicoSeconds()
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "SnapshotPeriodInOuterTimeSteps: "
        << RTG.mSnapshotPeriodInOTS
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "VelocityResampleTargetTemperature: "
        << RTG.mVelocityResampleTargetTemperature
        << EndLogLine;
      BegLogLine( PKFXLOG_RUN_PARAMS )
        << "VelocityResamplePeriodInOuterTimeSteps: "
        << RTG.mVelocityResamplePeriodInOTS
        << EndLogLine;
    }

  BegLogLine( PKFXLOG_SETUP )
    << "VoxelManagerThread():: About to start ts=0 point energy calculation"
    << EndLogLine;

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  MSD_TYPED_STD_BondedForceDriver.Execute(DynVarMgrIF, SimTick, RespaLevel );

#ifdef MOLECULE_DRIVER
  MSD_TYPED_STD_MolDriver.Execute( DynVarMgrIF, SimTick, RespaLevel );
#endif

  BegLogLine( PKFXLOG_SETUP )
    << "p2.hpp:: Done with BondedForceDriver.Execute()"
    << EndLogLine;

#ifndef MSDDEF_NO_LRF
  {
    RealSpaceStart.HitOE( PKTRACE_REAL_SPACE,
                          "RealSpaceTotal",
                          MyAddressSpaceId,
                          RealSpaceStart );

    BegLogLine( PKFXLOG_VOXELREALSPACE ) << "VoxelThread Done Sending " << EndLogLine;

    DynVarMgrIF.incRealSpaceSend();

    RealSpaceClientSignalSet.mResetSignal = 1;
    RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal = 1;

    BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

#ifdef BM_SPATIAL_DECOMPOSITION
    RealSpaceClientSignalSet.mUpdateFragmentAssignmentListSignal = 1;
    DynVarMgrIF.UpdateDisplacementForFragmentAssignmentMonitoring();
    DynVarMgrIF.UpdateDisplacementForFragmentVerletListMonitoring();
#else
    DynVarMgrIF.UpdateDisplacementForFragmentVerletListMonitoring();
#endif

    RealSpaceClient.Send( SimTick, RealSpaceClientSignalSet );

    BegLogLine( PKFXLOG_SETUP )
      << "p2.hpp:: Done with RealSpaceClient.Send()"
      << EndLogLine;

    RealSpaceFinis.HitOE( PKTRACE_REAL_SPACE,
                          "RealSpaceTotal",
                          MyAddressSpaceId,
                          RealSpaceFinis );
  }
#endif

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  //******************************************************************
  //***** Reduce Forces. This is effectively a barrier.
  //******************************************************************

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

#if defined ( PK_PHASE1_5 )
  DynVarMgrIF.ReduceForces( SimTick, RespaLevel );
#elif defined ( BM_SPATIAL_DECOMPOSITION )
  DynVarMgrIF.ReduceForces( SimTick, RespaLevel );
#endif

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

#ifdef KSpaceEnabled
  KSpaceClient.Wait( SimTick );
#endif

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;


#if MSDDEF_DoPressureControl
  for(int rl=0; rl <= RespaLevel; rl++)
    MSD_TYPED_LFD_AtomToFragmentVirialCorrectionDriver::Execute( DynVarMgrIF, SimTick, rl );

  DynVarMgrIF.ReduceVirial( SimTick, RespaLevel );
  MSD_TYPED_LFD_UpdateCenterOfMassForceDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );
  MSD_TYPED_LFD_UpdateFragmentKineticEnergyDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

#ifdef PK_PHASE1_5
  DynVarMgrIF.ReduceCenterOfMassKineticEnergy( SimTick, RespaLevel );
#endif
#endif

  unsigned FrameContentsMask = 0;

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  if( MyAddressSpaceId == 0 )
    {
      if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
	ED_Emit_InformationUDFCount( TimeStep, MSD_IF::GetNumberOfEnergyReportingUDFs() + 0 );
      else
	ED_Emit_InformationUDFCount( TimeStep, 0 );

#if( MSDDEF_DoTemperatureControl )
      FrameContentsMask |= FrameContents::RandomState;
      NormalDeviate::state currentState = NormalDeviateIF.getState();
      ED_Emit_DynamicVariablesRandomState_Helper( TimeStep, currentState );
#endif


    }

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  BegLogLine( PKFXLOG_TOPOFMAINLOOP ) << "VoxelThread Before ExportDynamicVariables" << EndLogLine;
  DynVarMgrIF.ExportDynamicVariables( TimeStep );
  BegLogLine( PKFXLOG_TOPOFMAINLOOP ) << "VoxelThread After ExportDynamicVariables" << EndLogLine;

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;


  FrameContentsMask |= FrameContents::Constraints;

  //******************************************************************
  //****                   Initial virial and piston force
  //******************************************************************

#if MSDDEF_DoPressureControl
  {
    // Here convert from atm to internal units
    DynVarMgrIF.mExternalPressure = RTG.mPressureControlTarget * SciConst::Atm_IU;

    // For now, each volume dimension is a scalar equal to the bounding box volume
    XYZ bbox = DynVarMgrIF.mDynamicBoxDimensionVector;
    DynVarMgrIF.mVolume.mX = DynVarMgrIF.mVolume.mY = DynVarMgrIF.mVolume.mZ  =
      bbox.mX * bbox.mY * bbox.mZ;

    // VolumeVelocity should come either from RTP, dvs, or randomization call
    // I guess RTP variable should be mPressureControlPistonVelocity
    // May need random seed in rtp file
    // Need separate temperature for piston velocity randomization?
    DynVarMgrIF.mVolumeVelocity.mX = DynVarMgrIF.mVolumeVelocity.mY = DynVarMgrIF.mVolumeVelocity.mZ =
      RTG.mPressureControlPistonInitialVelocity;

    DynVarMgrIF.mVolumeVelocityRatio.mX = DynVarMgrIF.mVolumeVelocityRatio.mY = DynVarMgrIF.mVolumeVelocityRatio.mZ =
      DynVarMgrIF.mVolumeVelocity.mX / DynVarMgrIF.mVolume.mX;

    DynVarMgrIF.mVolumeMass = RTG.mPressureControlPistonMass;
    FrameContentsMask |= FrameContents::Pressure;
  }
#endif

  if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
    FrameContentsMask |= FrameContents::UDFs;

  FrameContentsMask |= FrameContents::Sites;
  FrameContentsMask |= FrameContents::RTP;
  FrameContentsMask |= FrameContents::BirthBox;

  if( MyAddressSpaceId == 0)
    {
      BoundingBox bb;
      bb.mMinBox = -1.0 * DynVarMgrIF.mDynamicBoxDimensionVector / 2 ;
      bb.mMaxBox = bb.mMinBox + DynVarMgrIF.mDynamicBoxDimensionVector;

      ED_Emit_InformationRTP(TimeStep,
			     0,
			     MSD_IF::GetInnerTimeStepInPicoSeconds(),
			     MSD_IF::GetNumberOfInnerTimeSteps(),
			     MSD_IF::GetOuterTimeStepInPicoSeconds(),
			     (RTG.mNumberOfOuterTimeSteps + RTG.mStartingTimeStep) - TimeStep, // The numberOfTimeSteps left
			     MSD_IF::GetNumberOfRespaLevels(),
			     RTG.mPressureControlPistonMass,
			     RTG.mPressureControlTarget,
			     RTG.mVelocityResampleTargetTemperature,
			     RTG.mVelocityResamplePeriodInOTS,
			     0,
			     RTG.mSnapshotPeriodInOTS,
			     0,
			     MSD_IF::GetSourceId(),
			     EXECUTABLEID,
			     NUMBER_OF_FRAGMENTS * MSDDEF_NOSE_HOOVER,
			     Platform::JobInfo::JobId(), // should this be from RTG?
			     bb.mMinBox,
			     bb.mMaxBox );

      ED_Emit_DynamicVariablesBoundingBox( TimeStep, XYZ::ZERO_VALUE(), RTG.mBirthBoundingBoxDimensionVector );
    }

#if MSDDEF_DoPressureControl
  {
    DirectMDVM< DynamicVariableManager > D_MDVM( DynVarMgrIF );
    D_MDVM.Setup( SimTick, RespaLevel );

    UDF_Binding::UDF_UpdateVirialForce_Execute( D_MDVM );

    if( MyAddressSpaceId == 0 )
      ED_Emit_ControlPressure( TimeStep,
                               DynVarMgrIF.mVirial[ 0 ],
                               DynVarMgrIF.mVolume,
                               DynVarMgrIF.mVolumeVelocity,
                               XYZ::ZERO_VALUE(),
                               DynVarMgrIF.mInternalPressure / SciConst::Atm_IU );
  }
#endif


  // Make sure initial resampleenergyloss value (0) is output
  // NOTE: We have to learn to control this more accurately.
#if ( MSDDEF_DoVelocityResample )
  {
    if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
      {
        EmitEnergy( TimeStep, UDF_Binding::ResampleEnergyLoss_Code, RTG.mGlobalResampleEnergyLoss, 0 );
      }
  }
#endif

  if( MyAddressSpaceId == 0 )
    {
      ED_Emit_InformationFrameContents( TimeStep, FrameContentsMask  );
      ED_Emit_ControlSyncId( eInvariantMagicNumber, TimeStep, eVariableMagicNumber, eInvariantMagicNumber, 0 );
    }

  //*************** Setup SimTick loop  *****************************
  // SimTicks are the highest frequency counter in the integrator.
  // Adding 1 to SimTick moves the simulation ahead 1 InnerTimeStep
  //******************************************************************

  int NumberOfSimTicksToDo = RTG.GetNumberOfSimTicks() + SimTick;
#ifdef PERFORMANCE_COUNTERS
  //ME
  ApcInit();
#endif
  //NEED: shouldn't this block come out?????  If we are setting this above to outermost respa level, why this loop?
  while( ( SimTick % MSD_IF::GetSimTicksAtRespaLevel()[ RespaLevel ] ) != 0 )
    {
      RespaLevel--;
      assert( RespaLevel >= 0 );
    }

  BegLogLine( PKFXLOG_INTEGRATOR_TRACE )
    << "Major Time Step Loop Initialization: "
    << " SimTick "    << SimTick
    << " NumberOfSimTicksToDo " << NumberOfSimTicksToDo
    << " TimeStep "   << TimeStep
    << " RespaLevel " << RespaLevel
    << EndLogLine;

  BegLogLine( PKFXLOG_TOPOFMAINLOOP )
    << "Voxel:: About to BMM_RDG_Flush()"
    << EndLogLine;

  BegLogLine(0)
    << "Got Here"
    << EndLogLine;

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  // Flush out any packets up to now to give folks a warm and fuzzy that things are running.
  BMM_RDG_Flush( TimeStep );

  BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

  pkTraceServer::FlushAllBothBuffers();

  Platform::Control::Barrier();
  Platform::Control::Barrier();

  int FirstTimeInLoop = 1;

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
  double PositionCheckSumTD = DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositionsD();
#endif
#endif

  int IterationCount = 0;

  Platform::Profiling::TurnOn() ;
  while( SimTick < NumberOfSimTicksToDo )
    {
#pragma execution_frequency( very_low )
      // ME: including Blake's codes
      if( IterationCount <= PKTRACE_SIMTICK_INIT_PHASE )
        {
          if( IterationCount == PKTRACE_SIMTICK_INIT_PHASE )
            pkTraceServer::FlushAllBothBuffers();
        }
      else if( IterationCount >= PKTRACE_SIMTICK_START && IterationCount < PKTRACE_SIMTICK_END )
        {
          int WindowStart = (IterationCount / PKTRACE_SIMTICK_PERIOD) * PKTRACE_SIMTICK_PERIOD;
          int WindowEnd   = WindowStart + PKTRACE_SIMTICK_COUNT;
          if( ! (IterationCount >= WindowStart && IterationCount < WindowEnd ) )
            pkTraceServer::Reset(); /*just to make cvs diff come out right*/
          if( IterationCount == WindowEnd )
            pkTraceServer::FlushAllBothBuffers();
        }
      else
        {
          pkTraceServer::Reset(); /*just to make cvs diff come out right*/
        }


      BegLogLine( PKFXLOG_TOPOFMAINLOOP )
	<< "TOP OF INTEGRATOR LOOP: MyAddressSpaceId " << MyAddressSpaceId
	<< " SimTick " << SimTick
	<< " TimeStep "     << TimeStep
	<< " RespaLevel " << RespaLevel
	<< EndLogLine;

      // ME
#ifdef PERFORMANCE_COUNTERS
      if(SimTick==100) ApcStart(1);
#endif

      SimtickLoopStart.HitOE( PKTRACE_TIMESTEP,
			      "TimeStep",
			      MyAddressSpaceId,
			      SimtickLoopStart );

      //******************************************************************
      //****                   Reset velocities if it's time
      //****  I'm assuming that we reset velocities AFTER reporting
      //****  energies, but I could easily be wrong.  Need to settle
      //****  this as we consider how to construct quasi-energy
      //****  invariants for constant temperature ensembles.
      //******************************************************************

#ifdef DEBUG_POTENTIAL_ENERGY
      double TotalPotentialEnergy = 0.0;
      double MyPotentialEnergy = DynVarMgrIF.GetPotentialEnergy();

      MPI_Allreduce( &MyPotentialEnergy, &TotalPotentialEnergy, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      doube acceptRatio;
      int myReplica = Platform::Topology::mSubPartitionId;

      (*mRemdStandard).getAcceptRatio(acceptRatio, myReplica);

      BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
        << SimTick
        << " TotalPotentialEnergy: " << TotalPotentialEnergy
        << " Acceptance Ratio : " << acceptRatio
        << EndLogLine;
#endif

#if (  MSDDEF_DoTemperatureControl || MSDDEF_ReplicaExchange )
      {
#if !(MSDDEF_NOSE_HOOVER)
	// Replica swap period has to be commensurate with the velocity resample period.
	assert( RTG.mSwapPeriodOTS % RTG.mVelocityResamplePeriodInOTS == 0 );


#if  MSDDEF_ReplicaExchange
	if( SimTick % RTG.mSwapPeriodOTS == 0 )
	  {
	    // #if MSDDEF_Stochastic
	    for (int i=0; i<commSize; i++)
	      {
		mPotentialEnergySend[i] = 0.0;
	      }

	    // ME: REMD-PACKET_EMIT

	    if( MyAddressSpaceId == 0 )
	      {
		ED_Emit_ControlReplicaExchangePeriodInOTS( SimTick,  RTG.mSwapPeriodOTS );
	      }

	    int index = Platform::Topology::mSubPartitionId * NumberOfNodesInReplica;
	    assert( index >= 0 && index < commSize );

	    mPotentialEnergySend[ index ] = DynVarMgrIF.GetPotentialEnergy();

	    RemdAllreduceEnergyStart.HitOE( PKTRACE_REMD_ALLREDUCE_ENERGIES,
					    " remd_allreduce_energies ",
					    MyAddressSpaceId,
					    RemdAllreduceEnergyStart);

	    MPI_Allreduce( mPotentialEnergySend, mPotentialEnergyRecv, commSize, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	    RemdAllreduceEnergyFinis.HitOE( PKTRACE_REMD_ALLREDUCE_ENERGIES,
					    " remd_allreduce_energies ",
					    MyAddressSpaceId,
					    RemdAllreduceEnergyFinis);

	    int counter = 0;

	    RemdCopyEnergyStart.HitOE( PKTRACE_REMD_COPY_ENERGIES,
				       " remd_copy_energies ",
				       MyAddressSpaceId,
				       RemdCopyEnergyStart);

	    for(int i=0; i<commSize; i=i+NumberOfNodesInReplica )
	      {
		assert( counter >= 0 && counter < mNumReplica );
		mPotentialEnergy[ counter ] = mPotentialEnergyRecv[i];
		counter++;
	      }

	    RemdCopyEnergyFinis.HitOE( PKTRACE_REMD_COPY_ENERGIES,
				       " remd_copy_energies ",
				       MyAddressSpaceId,
				       RemdCopyEnergyFinis);

	    // Packet-format
	    // ID1:   index of this replica
	    // T1:    temperature this replica had before entering remdSwapping
	    // E1:    potential energy of this replica

	    assert(counter == mNumReplica);

	    unsigned aID1 = (unsigned)Platform::Topology::mSubPartitionId;
	    double   aE1  = mPotentialEnergy[aID1];

	    RTG.mVelocityResampleTargetTemperature = (*mRemdStandard).getTemperature(aID1);

	    double aT1  = (*mRemdStandard).getTemperature(aID1);
	    // ME: ID2Temp = index of this replica's current temperature in the temperature array`
	    unsigned aID2Temp;  (*mRemdStandard).getReplica2Temperature(aID1,aID2Temp) ;

	    RemdSwappingStepStart.HitOE( PKTRACE_REMD_SWAPPING_STEP,
					 " remd_swapping_step",
					 MyAddressSpaceId,
					 RemdSwappingStepStart);

	    (*mRemdStandard).remdSwapping(mPotentialEnergy);

	    RemdSwappingStepFinis.HitOE( PKTRACE_REMD_SWAPPING_STEP,
					 " remd_swapping_step",
					 MyAddressSpaceId,
					 RemdSwappingStepFinis);

	    RTG.mVelocityResampleTargetTemperature = (*mRemdStandard).getTemperature(aID1);

	    //  T2= current temperature of this replica (after swapping, i.e. temperature(ID2))
	    double aT2  = (*mRemdStandard).getTemperature(aID1);

	    // SwapAttempts = unsigned number of times this T1 was the lower temperature in a swap
	    unsigned aSwapAttemps = (*mRemdStandard).getSwapAttempt(aID2Temp);

	    // SwapAccepts = unsigned number of times the swap was accepted when this T1 was the lower temperature in a swap
	    unsigned aSwapAccepts = (*mRemdStandard).getSwapAccept(aID2Temp);

	    if( MyAddressSpaceId == 0 )
	      {
		ED_Emit_ControlReplicaExchangeAttempt(SimTick, aID1, aID2Temp, aT1, aT2, aE1,
						      aSwapAttemps, aSwapAccepts);
	      }

#endif

	    if( ( MSDDEF_DoTemperatureControl && ( TimeStep % RTG.mVelocityResamplePeriodInOTS == 0 ))
		|| ( MSDDEF_ReplicaExchange && ( (SimTick % RTG.mSwapPeriodOTS) == 0 )))
	      {

#ifndef BM_SPATIAL_DECOMPOSITION

		double KineticEnergyBefore = 0;
		// This needs to be a UDF driven by the direct driver!
		for(int s=0; s < DynVarMgrIF.GetNumberOfSites() ; s++)
		  {
		    //NEED: a frag should be able to carry its own random state!!!
		    XYZ NewVelocity =   RandomVelocity( MSD_IF::GetSiteInfo( s ).mass,
							RTG.mVelocityResampleTargetTemperature );

		    // Need to reorder these along the rotated box for regression independent of machine shape
		    NewVelocity.ReOrder(RTG.mBoxIndexOrder);

		    BegLogLine( PKFXLOG_VELOCITY_RESAMPLE )
		      << " TimeStep=" << TimeStep
		      << " RTG.mRTG.mVelocityResampleTargetTemperature=" << RTG.mVelocityResampleTargetTemperature
		      << " NewVelocity[ " << s << "]=" << NewVelocity
		      << EndLogLine;

#if defined( PK_PHASE1_5 )
		    if( DynVarMgrIF.IsSiteLocal( s ) )
		      {
#endif
			KineticEnergyBefore += 0.5 * MSD_IF::GetSiteInfo(s).mass * DynVarMgrIF.GetVelocity(s) * DynVarMgrIF.GetVelocity(s);
			DynVarMgrIF.SetVelocity(s, NewVelocity );
#if defined( PK_PHASE1_5 )
		      }
#endif
		  }

#if( MSDDEF_DoShakeRattle )
		{
		  MSD_TYPED_STD_GroupRattleDriver.Execute( DynVarMgrIF, SimTick, RespaLevel );
		}
#endif

		double KineticEnergyAfter = 0;

		for(int s=0; s < DynVarMgrIF.GetNumberOfSites() ; s++)
		  {
#if defined( PK_PHASE1_5 )
		    if( DynVarMgrIF.IsSiteLocal( s ) )
#endif
		      KineticEnergyAfter += 0.5 * MSD_IF::GetSiteInfo(s).mass * DynVarMgrIF.GetVelocity(s) * DynVarMgrIF.GetVelocity(s);
		  }

		RTG.mGlobalResampleEnergyLoss += KineticEnergyBefore - KineticEnergyAfter;
#else

		double KineticEnergyBefore = 0;

		for(int s=0; s < DynVarMgrIF.GetNumberOfSites() ; s++)
		  {
		    XYZ NewVel =   RandomVelocity( MSD_IF::GetSiteInfo( s ).mass,
						   RTG.mVelocityResampleTargetTemperature );

		    // Need to reorder these along the rotated box for regression independent of machine shape
		    NewVel.ReOrder(RTG.mBoxIndexOrder);

		    if( DynVarMgrIF.IsSiteLocal( s ) )
		      {
			FragmentRep & fRep = DynVarMgrIF.GetFragmentRepForSiteId( s );
			XYZ Vel = DynVarMgrIF.mIntegratorStateManagerIF.GetVelocity( fRep.mFragId, fRep.mOffset );

			KineticEnergyBefore += ( 0.5 * MSD_IF::GetSiteInfo(s).mass * Vel * Vel );

			// NEED: a frag should be able to carry it's rand seed!!!

			DynVarMgrIF.mIntegratorStateManagerIF.SetVelocity( fRep.mFragId, fRep.mOffset, NewVel );
		      }
		  }

#if( MSDDEF_DoShakeRattle )
		{
		  MSD_TYPED_STD_GroupRattleDriver.Execute( DynVarMgrIF, SimTick, RespaLevel );
		}
#endif

		double KineticEnergyAfter = 0;
		for( int i=0; i < DynVarMgrIF.mIntegratorStateManagerIF.GetNumberOfLocalFragments(); i++ )
		  {
		    FragId fragId = DynVarMgrIF.mIntegratorStateManagerIF.GetNthFragmentId( i );

		    SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
		    int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );

		    for( int offset=0; offset < SiteCount; offset++ )
		      {
			SiteId s = FirstSiteId + offset;

			XYZ Vel = DynVarMgrIF.mIntegratorStateManagerIF.GetVelocity( fragId, offset );

			KineticEnergyAfter += (0.5 * MSD_IF::GetSiteInfo(s).mass * Vel * Vel);
		      }
		  }

		RTG.mGlobalResampleEnergyLoss += KineticEnergyBefore - KineticEnergyAfter;
#endif

#if( MSDDEF_DoPressureControl )
                {
		  //  The following are for piston resampling  SSC
		  MSD_TYPED_LFD_UpdateCenterOfMassVelocityDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );
		  MSD_TYPED_LFD_UpdateFragmentKineticEnergyDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );
#if ( defined ( PK_PHASE1_5 ) || defined ( BM_SPATIAL_DECOMPOSITION ) )
                  DynVarMgrIF.ReduceCenterOfMassKineticEnergy( SimTick, RespaLevel );
#endif

		  //  The following will accomodate particle resampling specific to pressure control  SSC
		  DirectMDVM< DynamicVariableManager > D_MDVM( DynVarMgrIF );
		  D_MDVM.Setup( SimTick, RespaLevel );
		  UDF_Binding::UDF_UpdateVirialForce_Execute( D_MDVM );

		  double PistonKineticEnergyBefore = 0.5 * RTG.mPressureControlPistonMass * DynVarMgrIF.mVolumeVelocity.mX * DynVarMgrIF.mVolumeVelocity.mX;

		  // Resampling the piston velocity
		  //****   Code comming from Maria and Yuriy
		  // VT_Q(T) -> VT_Qnew(T+dT/2)
		  XYZ newPistonVelocity = RandomVelocity( RTG.mPressureControlPistonMass,
							  RTG.mVelocityResampleTargetTemperature );

		  DynVarMgrIF.mVolumeVelocity.mX = DynVarMgrIF.mVolumeVelocity.mY = DynVarMgrIF.mVolumeVelocity.mZ
		    = newPistonVelocity.mX;

		  double PistonKineticEnergyAfter = 0.5 * RTG.mPressureControlPistonMass * newPistonVelocity.mX * newPistonVelocity.mX;

		  // Only node 0 contributes to the piston resample energy loss
		  if ( MyAddressSpaceId == 0 )
		    RTG.mGlobalResampleEnergyLoss += PistonKineticEnergyBefore - PistonKineticEnergyAfter;

		  DynVarMgrIF.mVolumeVelocityRatio.mX = DynVarMgrIF.mVolumeVelocityRatio.mY = DynVarMgrIF.mVolumeVelocityRatio.mZ
		    = DynVarMgrIF.mVolumeVelocity.mX / DynVarMgrIF.mVolume.mX;
                }
#endif

#ifdef ROLLING_POINT_REGRESSION
#ifdef BM_SPATIAL_DECOMPOSITION
		DynVarMgrIF.SendVelocityToP1_5( SimTick );
		Platform::Control::Barrier();
#elif defined( PK_PHASE1_5 )
		if( Platform::Topology::GetAddressSpaceId() == 0 )
		  DynVarMgrIF.ReceiveVelocityFromP5( SimTick );

		int ReduceSize1 = NUMBER_OF_SITES * ( sizeof( XYZ ) / sizeof( double ) );

		MPI_Bcast( DynVarMgrIF.mVelocity, ReduceSize1, MPI_DOUBLE, 0, Platform::Topology::cart_comm );
		Platform::Control::Barrier();
#endif
#endif
              }
#else
	    // Nose-Hoover Temperature control
	    //****   Code comming from Maria
	    //     (-2). do a half-step Nose-Hoover dynamics for VT_i based on eqs (7,9),
	    //      possibly with internal respa decomposition for stability
	    //           VT_i(T)  -> VT_i(T+dT/2)
	    //     (-1). update a full step the thermostat "positions"
	    //           QT_i(T+dT) += VT_i(T+dT/2}*dT
	    //     (0). do a half-step update of the vcom from temperature control
	    //          vcom(T+dT/2) = exp[-VT_i(T+dT/2)*dT/2]*vcom(T)
	    //          vrel_i_a(T+dT/2) = exp[-VT2_i(T+dT/2)*dT/2]*vrel_i_a(T)
	    //*****
	    MSD_TYPED_LFD_NoseHooverIntegrationDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

#if ( MSDDEF_DoPressureControl )
	    {
	      //****   Code comming from Maria
	      //           VT_Q(T) -> VT_Q(T+dT/2)
	      //           QT_Q(T+dT) += VT+Q(T+dT/2)*dT
	      //           VQ(T+dT/2) = exp[-VT_Q(T+dT/2)*dT/2]*VQ(T)
	      DirectPistonMDVM< DynamicVariableManager > P_MDVM( DynVarMgrIF );
	      UDF_Binding::UDF_UpdateFragmentKEForNoseHoover_Execute( P_MDVM );
	      UDF_Binding::UDF_NoseHooverIntegration_Execute( P_MDVM );
	    }
#endif
#endif
	  }
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

	//******************************************************************
	//****                   Update Piston Velocity and Position
	//******************************************************************
#if MSDDEF_DoPressureControl
	{
	  DirectMDVM< DynamicVariableManager > D_MDVM( DynVarMgrIF );
	  D_MDVM.Setup( SimTick, RespaLevel );

	  UDF_Binding::UDF_UpdateVolumeHalfStep_Execute( D_MDVM );
#ifndef NOT_TIME_REVERSIBLE
	  // update volume later, in two half-steps
#else
	  UDF_Binding::UDF_UpdateVolumePosition_Execute( D_MDVM );
#endif
	}
#endif

	//******************************************************************
	//****                   Velocity Half Step
	//******************************************************************

#if MSDDEF_DoPressureControl
#ifndef NOT_TIME_REVERSIBLE
        DynVarMgrIF.UpdateExpVolumeVelocityRatioHalfStepNPT();
#endif
#endif

	MSD_TYPED_LFD_UpdateVelocityFirstHalfStepDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

#ifdef ROLLING_POINT_REGRESSION
        // Here lies the p1.5 vs p5 regression trick.
#ifdef BM_SPATIAL_DECOMPOSITION
        DynVarMgrIF.SendPosVelToP1_5( SimTick, 0 );
        Platform::Control::Barrier();
#elif defined( PK_PHASE1_5 )

        for( int i=0; i < NUMBER_OF_SITES; i++ )
          {
	    if( !DynVarMgrIF.IsSiteLocal( i ) )
	      {
		DynVarMgrIF.mVelocity[i].Zero();
	      }
          }

        int ReduceSize = NUMBER_OF_SITES * ( sizeof( XYZ ) / sizeof( double ) );

        MPI_Allreduce( DynVarMgrIF.mVelocity, DynVarMgrIF.mVelocityTemp, ReduceSize, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
        memcpy( DynVarMgrIF.mVelocity, DynVarMgrIF.mVelocityTemp, sizeof( XYZ ) * NUMBER_OF_SITES );

        if( Platform::Topology::GetAddressSpaceId() == 0 )
          DynVarMgrIF.ReceiveAndRegressPosVelFromP5( SimTick );

        MPI_Bcast( DynVarMgrIF.mVelocity, ReduceSize, MPI_DOUBLE, 0, Platform::Topology::cart_comm );
        MPI_Bcast( DynVarMgrIF.mPosit, ReduceSize, MPI_DOUBLE, 0, Platform::Topology::cart_comm );
        Platform::Control::Barrier();
#endif
#endif

	DynVarMgrIF.ZeroForceRegisters( SimTick, RespaLevel );

#if MSDDEF_ReplicaExchange
	DynVarMgrIF.ZeroPotentialEnergy();
#endif

	//******************************************************************
	//****                       SHAKE
	//******************************************************************
#if (MSDDEF_DoShakeRattle )
	{
	  MSD_TYPED_STD_GroupShakeDriver.Execute( DynVarMgrIF, SimTick, RespaLevel );
	}
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

	//******************************************************************
	//****    [if TIME_REVERSIBLE] Update Piston  Position Half Step
	//******************************************************************
#if MSDDEF_DoPressureControl
#ifndef NOT_TIME_REVERSIBLE
        {
	  // note to myself: "VolumeVelocityRatio" is not used in time reversible algorithm, so do not
	  // need to update it here, even though otherwise would have to
	  DirectMDVM< DynamicVariableManager > D_MDVM( DynVarMgrIF );
	  D_MDVM.Setup( SimTick, RespaLevel );
	  UDF_Binding::UDF_UpdateVolumePosition_Execute( D_MDVM );
        }
#else
        // did it before, full step
#endif
#endif
#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

	//******************************************************************
	//****                   Update Positions
	//******************************************************************
	BegLogLine(PKFXLOG_RESPA_STEPS)
	  << "RESPA VoxId " << MyAddressSpaceId
	  << " SimTick "         << SimTick
	  << " TimeStep "   << TimeStep
	  << " Current Respa Level "  << RespaLevel
	  << " Calling UpdatePositions() "
	  << EndLogLine;

	// If we are in Pressure Control mode the below call also takes care of
	// Updating the COM position
#if MSDDEF_DoPressureControl
#ifndef NOT_TIME_REVERSIBLE
        // this is a common factor used in scaling fragment COM positions, so update it here.
        DynVarMgrIF.UpdateExpVolumeVelocityRatioFullStepNPT();
#endif
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

	MSD_TYPED_LFD_UpdatePositionDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

	//DynVarMgrIF.mIntegratorStateManagerIF.DumpPositions( SimTick );

	//******************************************************************
	//****     [if TIME_REVERSIBLE] Update Piston  Position Half Step
	//******************************************************************
#if MSDDEF_DoPressureControl
#ifndef NOT_TIME_REVERSIBLE
        // note to myself: "VolumeVelocityRatio" is not used in time reversible algorithm, so do not
        // need to update it here, even though otherwise would have to
        {
	  DirectMDVM< DynamicVariableManager > D_MDVM( DynVarMgrIF );
	  D_MDVM.Setup( SimTick, RespaLevel );
	  UDF_Binding::UDF_UpdateVolumePosition_Execute( D_MDVM );
        }
#else
        // nothing here
#endif
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        PositionCheckSumTD = DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositionsD();
#endif
#endif

	//******************************************************************
	//***** Check Verlet list violation
	//******************************************************************
	GuardZoneViolationReductionStart.HitOE( ( PKTRACE_GUARDZONE_VIOLATION_REDUCTION ),
						"GuardZoneViolationReductionStart",
						Platform::Topology::GetAddressSpaceId(),
						GuardZoneViolationReductionStart );

	//******************************************************************
	//* Check if the position of an atom went outside the 1.5 bound box
	//* Only the tag atoms of frag centers get imaged
	//******************************************************************
	int SiteDisplacementViolations = DynVarMgrIF.CheckSiteDisplacementViolation();

	BegLogLine( 0 )
	  << SimTick
	  << " SiteDisplacementViolations: " << SiteDisplacementViolations
	  << EndLogLine;

#ifdef ROLLING_POINT_REGRESSION
#ifdef PK_PHASE1_5
	SiteDisplacementViolations = 1;
#endif
#endif

	//******************************************************************
	//* Reduce signals over the entire machine
	//* 1. Verlet guard zone violation                       Bit 0
	//* 2. Communication assignment guard zone violation     Bit 1
	//* 3. Positions outsite the 1.5 bounding box violation  Bit 2
	//* 4. Orb recooking (not hooked in yet)                 Bit 3
	//******************************************************************

	unsigned long SignalSet = 0;
	SignalSet = SiteDisplacementViolations;

	// for uni runs
	unsigned GuardZoneViolation = SiteDisplacementViolations;

#ifdef PK_PARALLEL
	unsigned long TempSignalSet = 0;

	Platform::Collective::GlobalOR( &SignalSet, &TempSignalSet );

	GuardZoneViolation = TempSignalSet & 3;

	BegLogLine( PKFXLOG_CHECKSITEDISPLACEMENTVIOLATION )
	  << "VoxelManagerThread:: "
	  << SimTick
	  << " CheckSiteDisplacementViolation() = " << TempSignalSet
	  << EndLogLine;


	BegLogLine( PKFXLOG_VERLET_ASSIGN_RECOOK_SYNC )
          << SimTick
          << "GuardZoneViolation : " << GuardZoneViolation
          << EndLogLine;

	unsigned VerletListViolation = GuardZoneViolation & 0x1;

#ifdef BM_SPATIAL_DECOMPOSITION
        int AssignmentViolation = (( GuardZoneViolation >> 1 ) & 0x1);

        static int AssignmentCount = 1;
        unsigned BoundingBoxViolation = ( TempSignalSet >> 2 ) & 0x1;
        if( BoundingBoxViolation )
          {
	    DynVarMgrIF.ImageAllPositionsIntoCentralCell( SimTick );

	    GuardZoneViolation = 1;
	    RealSpaceClientSignalSet.mResetSignal = 1;
	    AssignmentViolation = 1;

	    BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
	      << SimTick
	      << " BoundingBoxViolation= " << BoundingBoxViolation
	      << EndLogLine;
          }

#ifdef FORCE_MIGRATE_BEFORE_SNAPSHOT
	if( (TimeStep % RTG.mSnapshotPeriodInOTS == 0 ) || (SimTick >= NumberOfSimTicksToDo ) ) // or if it's the last time around.
	  {
            GuardZoneViolation = 1;
            AssignmentViolation = 1;
	  }
#endif

#endif
#endif

	GuardZoneViolationReductionFinis.HitOE( ( PKTRACE_GUARDZONE_VIOLATION_REDUCTION ),
						"GuardZoneViolationReductionFinis",
						Platform::Topology::GetAddressSpaceId(),
						GuardZoneViolationReductionFinis );


#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif

	//******************************************************************
	//***** Only manage the spatial decomposion if there's a violation
	//*****
	//******************************************************************

#if 0
	// Routine collects statistice on how migration is handling diffusion.
	DynVarMgrIF.mIntegratorStateManagerIF.CollectMigrationStatistics( AssignmentViolation, DynVarMgrIF.mCenterOfBoundingBox );
#endif

	if( AssignmentViolation )
	  {
	    AssignmentCount = 0;


	    BegLogLine( PKFXLOG_MANAGE_SPACIAL_DECOMP )
	      << SimTick
	      << " Managing Spacial Decomp"
	      << EndLogLine;
	    DynVarMgrIF.ManageSpacialDecompositionAllToAll( SimTick );

	    // Might make sense to go back to the old way of looking at the entire list
	    MSD_TYPED_STD_BondedForceDriver.UpdateLocalSitesParams( DynVarMgrIF );

#if MSDDEF_DoShakeRattle
	    MSD_TYPED_STD_GroupRattleDriver.UpdateLocalSitesParams( DynVarMgrIF );
	    MSD_TYPED_STD_GroupShakeDriver.UpdateLocalSitesParams( DynVarMgrIF );
#endif

#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	    PositionCheckSumTD = DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositionsD();
#endif
	  }
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif
	// DynVarMgrIF.DumpPositions( SimTick );

	//******************************************************************
	//***** Globalize positions. This is effectively a barrier.
	//******************************************************************
	BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

	EmitTimeStamp(SimTick, Tag::TimeStep, 1 );

	//******************************************************************
	//***** Increment SimTick, the integrator's highest frequency counter.
	//******************************************************************
	++SimTick;

	if( SimTick % EveryNTimeSteps == 0 && MyAddressSpaceId == 0 )
	  {
	    EveryNTimeStepsStart.HitOE( 1,
					"EveryNTimeStepsStart",
					MyAddressSpaceId,
					EveryNTimeStepsStart );
	    EveryNTimeStepsFinis.HitOE( 1,
					"EveryNTimeStepsFinis",
					MyAddressSpaceId,
					EveryNTimeStepsFinis );
	  }

	// Reset respa level
	RespaLevel = MSD_IF::GetNumberOfRespaLevels() - 1;  // Change to Zero based index.
	while( ( SimTick % MSD_IF::GetSimTicksAtRespaLevel()[ RespaLevel ] ) != 0 )
	  {
	    RespaLevel--;
	    assert( RespaLevel >= 0 );
	  }

	// If on an OuterMostTimeStep, increment TimeStep counter
	if( RespaLevel == MSD_IF::GetNumberOfRespaLevels() - 1 )
	  {
	    TimeStep ++;
	    LastSimTickOfTimeStepFlag = 1;
	  }
	else
	  {
	    LastSimTickOfTimeStepFlag = 0;
	  }

	if( LastSimTickOfTimeStepFlag )
	  BMM_RDG_Flush( TimeStep );

	BegLogLine( PKFXLOG_VOXELRESPA )
	  << "X*X*X*X SimTick++ X*X*X*X MyAddressSpaceId " << MyAddressSpaceId
	  << " SimTick "               << SimTick
	  << " TimeStep "              << TimeStep
	  << " LastSimTickOfTimeStepFlag " <<   LastSimTickOfTimeStepFlag
	  << " RespaLevel "            << RespaLevel
	  << EndLogLine;

	BegLogLine(PKFXLOG_RESPA_STEPS)
	  << "VoxId " << MyAddressSpaceId
	  << " SimTick "         << SimTick
	  << " TimeStep "        << TimeStep
	  << " Current Respa Level "  << RespaLevel
	  << " Zeroing Force and Energy Accumulators  "
	  << EndLogLine;

#if MSDDEF_DoPressureControl
	{
	  // prepare for global virial to accumulate based on new forces
	  DynVarMgrIF.ZeroVirial();
	}
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif

#ifdef CHECKSUM_INTERECTION_POSITIONS
        DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions( PositionCheckInteractionContext );
#endif
#endif
	//******************************************************************
	//****    Compute Forces Depending On Respa Level
	//******************************************************************
	// If we are monitoring site displacements in order to update verlet lists, do it here.

#if defined(KSpaceEnabled)
#ifdef PK_PHASE5_SPLIT_COMMS
        KSpaceClient.Send( SimTick, 0, 0 );
#endif
#endif

#if ( defined( PK_PHASE1_5 ) )
	DynVarMgrIF.GlobalizePositions( SimTick, RespaLevel );
#elif ( defined ( BM_SPATIAL_DECOMPOSITION ) )
	DynVarMgrIF.GlobalizePositions( SimTick, RespaLevel, AssignmentViolation );
#endif

	BegLogLine( PKFXLOG_CHASEHANG ) << "Chasing Hang" << EndLogLine;

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTERECTION_POSITIONS
        double PositionCheckInteractionContext = DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions();
#endif
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

	//******************************************************************
	//***** Load Balancing
	//******************************************************************
#ifdef PK_PARALLEL
#if !defined( BM_SPATIAL_DECOMPOSITION )
        // For the first half of the tuning steps, just zero -- would like a mid-stream sample.
        if( DynVarMgrIF.mNumberOfRealSpaceCalls == ( (1 + TIMESTEPS_TO_TUNE) / 2) )
          {
	    DynVarMgrIF.ZeroFragmentTimeCounters();
          }

        if( DynVarMgrIF.mNumberOfRealSpaceCalls == TIMESTEPS_TO_TUNE )
          {
#if defined( PK_PHASE1_5 )
            DynVarMgrIF.GlobalizeDynamicStateAfterLoadBalancing( SimTick, RespaLevel );
#endif
	    DynVarMgrIF.ReduceFragmentTime();
	    DynVarMgrIF.RedistributeFragmentsBasedOnTime();

	    DynVarMgrIF.mSampleFragmentTime=0;

	    RealSpaceClientSignalSet.mResetSignal                    = 1;
	    RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal = 1;
	    RealSpaceClientSignalSet.mTuneGuardZoneSignal            = 1;

	    // Update all the SiteTupleDrivers to use the new fragment mapping
	    MSD_TYPED_STD_BondedForceDriver.UpdateLocalSitesParams( DynVarMgrIF );

#if MSDDEF_DoShakeRattle
            MSD_TYPED_STD_GroupRattleDriver.UpdateLocalSitesParams( DynVarMgrIF );
            MSD_TYPED_STD_GroupShakeDriver.UpdateLocalSitesParams( DynVarMgrIF );
#endif
          }
#endif
#endif


#ifdef PK_PHASE5_SPLIT_COMMS
#ifdef KSpaceEnabled
        {
	  // KSpace always goes on the last respa level
	  if( MSD_IF::DoKSpaceAtRespaLevel( RespaLevel ) )
	    {
	      BegLogLine( PKFXLOG_VOXELKSPACE )
		<< SimTick
		<< " VoxelThread before KSpace" << EndLogLine;

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	      DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
	      unsigned KspaceSignal = ( GuardZoneViolation || AssignmentViolation || RealSpaceClientSignalSet.mResetSignal );
#else
	      unsigned KspaceSignal = ( GuardZoneViolation || RealSpaceClientSignalSet.mResetSignal );
#endif

	      BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << " Chasing Hang" << EndLogLine;
	    }
        }
#endif
#endif

	int handle = -1;
#ifndef MSDDEF_NO_LRF
        {
	  RealSpaceStart.HitOE( PKTRACE_REAL_SPACE,
				"RealSpaceTotal",
				MyAddressSpaceId,
				RealSpaceStart );

	  BegLogLine( PKFXLOG_VOXELREALSPACE  ) << "VoxelThread about to Send() " << EndLogLine;

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	  DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif

#ifdef CHECKSUM_INTERECTION_POSITIONS
	  DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions( PositionCheckInteractionContext );
#endif
#endif

	  DynVarMgrIF.incRealSpaceSend();

	  RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal     = GuardZoneViolation & 0x1;
#ifdef BM_SPATIAL_DECOMPOSITION

          RealSpaceClientSignalSet.mUpdateFragmentAssignmentListSignal = AssignmentViolation;
#endif


	  BegLogLine( PKFXLOG_RS_SIGNAL_SET )
	    << SimTick
	    << " RealSpaceClientSignalSet.mResetSignal= "
	    << RealSpaceClientSignalSet.mResetSignal
	    << " RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal= "
	    << RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal
	    << " RealSpaceClientSignalSet.mUpdateFragmentAssignmentListSignal= "
	    << RealSpaceClientSignalSet.mUpdateFragmentAssignmentListSignal
	    << EndLogLine;

#ifdef BM_SPATIAL_DECOMPOSITION
	  if( RealSpaceClientSignalSet.mUpdateFragmentAssignmentListSignal ||
	      RealSpaceClientSignalSet.mResetSignal )
	    {
	      BegLogLine( 0 )
		<< "p2.hpp: About to update displacement lists for verlet and assignment "
		<< EndLogLine;

	      DynVarMgrIF.UpdateDisplacementForFragmentAssignmentMonitoring();
	      DynVarMgrIF.UpdateDisplacementForFragmentVerletListMonitoring();
	    }

	  if( ( RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal &&
		!RealSpaceClientSignalSet.mUpdateFragmentAssignmentListSignal )
	      || RealSpaceClientSignalSet.mResetSignal )
	    {
	      DynVarMgrIF.UpdateDisplacementForFragmentVerletListMonitoring();
	    }
#else
	  if( RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal || RealSpaceClientSignalSet.mResetSignal )
	    {
	      DynVarMgrIF.UpdateDisplacementForFragmentVerletListMonitoring();
	    }
#endif

	  RealSpaceClient.Send( SimTick, RealSpaceClientSignalSet );

	  BegLogLine( 0 )
	    << SimTick
	    << " Done with real space send"
	    << EndLogLine;

	  RealSpaceFinis.HitOE( PKTRACE_REAL_SPACE,
				"RealSpaceTotal",
				MyAddressSpaceId,
				RealSpaceFinis );
        }
#endif

	MSD_TYPED_STD_BondedForceDriver.Execute(DynVarMgrIF, SimTick, RespaLevel );

#ifdef MOLECULE_DRIVER
        MSD_TYPED_STD_MolDriver.Execute( DynVarMgrIF, SimTick, RespaLevel );
#endif

#ifndef PK_PHASE5_SPLIT_COMMS
#ifdef KSpaceEnabled
        {
	  // KSpace always goes on the last respa level
	  if( MSD_IF::DoKSpaceAtRespaLevel( RespaLevel ) )
	    {
	      //           KSpaceEvaluationStart.HitOE( PKTRACE_P2_KSPACE_CONTROL,
	      //                                        "P2_KSpace_Control",
	      //                                        MyAddressSpaceId,
	      //                                        KSpaceEvaluationStart );

	      BegLogLine( PKFXLOG_VOXELKSPACE )
		<< SimTick
		<< " VoxelThread before KSpace" << EndLogLine;

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	      DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
	      unsigned KspaceSignal = ( GuardZoneViolation || AssignmentViolation || RealSpaceClientSignalSet.mResetSignal );
#else
	      unsigned KspaceSignal = ( GuardZoneViolation || RealSpaceClientSignalSet.mResetSignal );
#endif

	      BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << " Chasing Hang" << EndLogLine;

	      KSpaceClient.Send( SimTick, RespaLevel, KspaceSignal );

	      BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << " Chasing Hang" << EndLogLine;

	      BegLogLine( PKFXLOG_VOXELKSPACE )
		<< SimTick
		<< " VoxelThread Done Sending, Waiting " << EndLogLine;

	      KSpaceClient.Wait( SimTick );

	      //           KSpaceEvaluationFinis.HitOE( PKTRACE_P2_KSPACE_CONTROL,
	      //                                        "P2_KSpace_Control",
	      //                                        MyAddressSpaceId,
	      //                                        KSpaceEvaluationFinis );
	    }
        }
#endif
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
	DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif

#ifdef CHECKSUM_INTERECTION_POSITIONS
	DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions( PositionCheckInteractionContext );
#endif
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTERECTION_POSITIONS
	DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions( PositionCheckInteractionContext );
#endif
#endif

	// Reset RealSpace signal
	RealSpaceClientSignalSet.mResetSignal = 0;
	RealSpaceClientSignalSet.mUpdateFragmentVerletListSignal = 0;
	RealSpaceClientSignalSet.mUpdateFragmentAssignmentListSignal = 0;

	//******************************************************************
	//***** Reduce Forces. This is effectively a barrier.
	//******************************************************************
	BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << " Chasing Hang" << EndLogLine;

#if defined( PK_PHASE1_5 )
	DynVarMgrIF.ReduceForces( SimTick, RespaLevel );
#elif defined( BM_SPATIAL_DECOMPOSITION )
	DynVarMgrIF.ReduceForces( SimTick, RespaLevel );
#endif
	BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << "Chasing Hang" << EndLogLine;

#ifdef PK_PHASE5_SPLIT_COMMS
#ifdef KSpaceEnabled
	KSpaceClient.Wait( SimTick );
#endif
#endif
	BegLogLine( PKFXLOG_CHASEHANG ) << SimTick << "Chasing Hang" << EndLogLine;

#if MSDDEF_DoPressureControl
	for(int rl=0; rl <= RespaLevel; rl++)
	  MSD_TYPED_LFD_AtomToFragmentVirialCorrectionDriver::Execute( DynVarMgrIF, SimTick, rl );

	DynVarMgrIF.ReduceVirial( SimTick, RespaLevel );

	// Need to Update COM Force
	MSD_TYPED_LFD_UpdateCenterOfMassForceDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );
#endif


#ifdef ROLLING_POINT_REGRESSION
        // Here lies the p1.5 vs p4 regression trick.
#ifdef BM_SPATIAL_DECOMPOSITION
        DynVarMgrIF.SendForceP1_5( SimTick );
        Platform::Control::Barrier();
#elif defined( PK_PHASE1_5 )
        if( Platform::Topology::GetAddressSpaceId() == 0 )
          DynVarMgrIF.ReceiveAndRegressForceFromP5( SimTick );

        MPI_Bcast( DynVarMgrIF.mForce, ReduceSize, MPI_DOUBLE, 0, Platform::Topology::cart_comm );

        Platform::Control::Barrier();
#endif
#endif


	//******************************************************************
	//****                   Update Volume Velocity
	//******************************************************************
#ifndef NOT_TIME_REVERSIBLE
	// do not need this
#else
#if ( MSDDEF_DoPressureControl )
        {
	  DirectMDVM< DynamicVariableManager > D_MDVM( DynVarMgrIF );
	  D_MDVM.Setup( SimTick, RespaLevel );
	  UDF_Binding::UDF_UpdateVolumeSecondHalfStep_Execute( D_MDVM );
        }
#endif
#endif

	//******************************************************************
	//****                   Velocity Half Step
	//******************************************************************

	BegLogLine(PKFXLOG_RESPA_STEPS)
	  << "RESPA VoxId " << MyAddressSpaceId
	  << " TS "         << TimeStep
	  << " Current Respa Level "  << RespaLevel
	  << " Calling UpdateVelocityHalfStep "
	  << EndLogLine;

#if MSDDEF_DoPressureControl
#ifndef NOT_TIME_REVERSIBLE
        DynVarMgrIF.UpdateExpVolumeVelocityRatioHalfStepNPT();
#endif
#endif

#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTERECTION_POSITIONS
        DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions( PositionCheckInteractionContext );
#endif
#endif

	MSD_TYPED_LFD_UpdateVelocitySecondHalfStepDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

	//******************************************************************
	//****                       RATTLE
	//******************************************************************
#ifdef BM_SPATIAL_DECOMPOSITION
#ifdef CHECKSUM_INTEGRATOR_POSITIONS
        DynVarMgrIF.mIntegratorStateManagerIF.ChecksumPositions( PositionChecksumTD );
#endif
#endif

#if( MSDDEF_DoShakeRattle )
	{
	  MSD_TYPED_STD_GroupRattleDriver.Execute( DynVarMgrIF, SimTick, RespaLevel );
	}
#endif

	//******************************************************************
	//****          Update COM velocities, positions, and KE
	//******************************************************************
	unsigned int contentMask = 0;

	//NEED: to figure out why this call is made twice within the main loop
#if MSDDEF_DoPressureControl
	{
	  contentMask |= FrameContents::Pressure;

	  /// Need to Update COM Velocity
	  /// Need to Update FragmentKineticEnergy
	  MSD_TYPED_LFD_UpdateCenterOfMassVelocityAndFragmentKineticEnergyDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );
#if ( defined ( PK_PHASE1_5 ) )
	  DynVarMgrIF.ReduceCenterOfMassKineticEnergy( SimTick, RespaLevel );
#endif
	}
#endif

	//******************************************************************
	//****  Report Energy for on ticks that do outermost respa level
	//******************************************************************
	if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
	  contentMask |= FrameContents::UDFs;

#if ( MSDDEF_DoPressureControl )
	{
	  DirectMDVM< DynamicVariableManager > D_MDVM( DynVarMgrIF );
	  D_MDVM.Setup( SimTick, RespaLevel );

	  UDF_Binding::UDF_UpdateVirialForce_Execute( D_MDVM );

	  UDF_Binding::UDF_UpdateVolumeHalfStep_Execute( D_MDVM );

	  DynVarMgrIF.SetVolumeVelocityRatio( DynVarMgrIF.GetVolumeVelocity() / DynVarMgrIF.GetVolumePosition().mX );

	  // Emit the pressure info prior to updating the volume info
	  // Convert pressure back to atm here
	}
#endif

#if MSDDEF_NOSE_HOOVER
	//****   Code comming from Maria
	//     (-2). do a half-step Nose-Hoover dynamics for VT_i based on eqs (7,9),
	//      possibly with internal respa decomposition for stability
	//           VT_i(T)  -> VT_i(T+dT/2)
	//     (-1). update a full step the thermostat "positions"
	//           QT_i(T+dT) += VT_i(T+dT/2}*dT
	//     (0). do a half-step update of the vcom from temperature control
	//          vcom(T+dT/2) = exp[-VT_i(T+dT/2)*dT/2]*vcom(T)
	//          vrel_i_a(T+dT/2) = exp[-VT2_i(T+dT/2)*dT/2]*vrel_i_a(T)
	//*****
	MSD_TYPED_LFD_NoseHooverIntegrationDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

#if ( MSDDEF_DoPressureControl )
        //****   Code comming from Maria
        //           VT_Q(T) -> VT_Q(T+dT/2)
        //           QT_Q(T+dT) += VT+Q(T+dT/2)*dT
        //           VQ(T+dT/2) = exp[-VT_Q(T+dT/2)*dT/2]*VQ(T)
        DirectPistonMDVM< DynamicVariableManager > P_MDVM( DynVarMgrIF );
        UDF_Binding::UDF_UpdateFragmentKEForNoseHoover_Execute( P_MDVM );
        UDF_Binding::UDF_NoseHooverIntegration_Execute( P_MDVM );
#endif
#endif

	MSD_TYPED_LFD_UpdateKineticEnergyDriver::Execute( DynVarMgrIF, SimTick, RespaLevel );

#if ( MSDDEF_NOSE_HOOVER && MSDDEF_DoPressureControl )
	DirectPistonMDVM< DynamicVariableManager > P1_MDVM( DynVarMgrIF );
	UDF_Binding::UDF_NoseHooverEnergy_Execute( P1_MDVM );

	if( MyAddressSpaceId == 0 )
	  EmitEnergy( TimeStep, UDF_Binding::NoseHooverEnergy_Code, P1_MDVM.GetEnergy(), 1 );
#endif

	// Things to do on outermost timestep only!
	// Note, we may not export dynamic vars and/or energies on every
	// outer most timestep - or even on the same number of timesteps.

#if MSDDEF_DoPressureControl
	if ( MyAddressSpaceId == 0 && ( SimTick % MSD::SimTicksPerTimeStep == 0 ) )
	  {
	    ED_Emit_ControlPressure( TimeStep,
				     DynVarMgrIF.mVirial[ 0 ],
				     DynVarMgrIF.mVolume,
				     DynVarMgrIF.mVolumeVelocity,
				     XYZ::ZERO_VALUE(),
				     DynVarMgrIF.mInternalPressure / SciConst::Atm_IU );
	  }
#endif

#ifdef DUMP_SNAPSHOT_EVERY_SIMTICK
	DynVarMgrIF.ExportDynamicVariables( SimTick );
#endif

	if( LastSimTickOfTimeStepFlag )
	  {
	    if( (TimeStep % RTG.mSnapshotPeriodInOTS == 0 ) || (SimTick >= NumberOfSimTicksToDo ) ) // or if it's the last time around.
	      {
		BegLogLine(PKFXLOG_RESPA_STEPS)
		  << "RESPA VoxId "      << MyAddressSpaceId
		  << " SimTick "         << SimTick
		  << " TimeStep "             << TimeStep
		  << " Pushing snapshot data into External Datagram stream "
		  << EndLogLine;

		// If we're get here then tell the packet server to expect sites.
		contentMask |= FrameContents::Sites;

#ifndef DUMP_SNAPSHOT_EVERY_SIMTICK
		DynVarMgrIF.ExportDynamicVariables( TimeStep );
#endif

		if( MyAddressSpaceId == 0 )
		  {
		    contentMask |= FrameContents::RandomState;

		    NormalDeviate::state currentState = NormalDeviateIF.getState();
		    ED_Emit_DynamicVariablesRandomState_Helper( TimeStep, currentState );

		    contentMask |= FrameContents::RTP;
		    contentMask |= FrameContents::BirthBox;

		    BoundingBox DynamicBoundingBox;
		    DynamicBoundingBox.mMinBox = - 0.5 * DynVarMgrIF.mDynamicBoxDimensionVector;
		    DynamicBoundingBox.mMaxBox = DynamicBoundingBox.mMinBox + DynVarMgrIF.mDynamicBoxDimensionVector;

		    ED_Emit_InformationRTP(TimeStep,
					   0,
					   MSD_IF::GetInnerTimeStepInPicoSeconds(),
					   MSD_IF::GetNumberOfInnerTimeSteps(),
					   MSD_IF::GetOuterTimeStepInPicoSeconds(),
					   (RTG.mNumberOfOuterTimeSteps + RTG.mStartingTimeStep) - TimeStep, // The numberOfTimeSteps left
					   MSD_IF::GetNumberOfRespaLevels(),
					   RTG.mPressureControlPistonMass,
					   RTG.mPressureControlTarget,
					   RTG.mVelocityResampleTargetTemperature,
					   RTG.mVelocityResamplePeriodInOTS,
					   0,
					   RTG.mSnapshotPeriodInOTS,
					   0,
					   MSD_IF::GetSourceId(),
					   EXECUTABLEID,
					   NUMBER_OF_FRAGMENTS * MSDDEF_NOSE_HOOVER,
					   Platform::JobInfo::JobId(), // should this be from RTG?
					   DynamicBoundingBox.mMinBox,
					   DynamicBoundingBox.mMaxBox );

		    ED_Emit_DynamicVariablesBoundingBox( TimeStep, XYZ::ZERO_VALUE(), RTG.mBirthBoundingBoxDimensionVector );
		  }
	      }

	    if( LastSimTickOfTimeStepFlag )
	      {
#if ( MSDDEF_DoVelocityResample )
		{
		  if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
		    {
		      EmitEnergy( TimeStep, UDF_Binding::ResampleEnergyLoss_Code, RTG.mGlobalResampleEnergyLoss, 0 );
		    }
		}
#endif

		if( MyAddressSpaceId == 0 )
		  {
		    ED_Emit_InformationFrameContents( TimeStep, contentMask );
		    ED_Emit_ControlSyncId( eInvariantMagicNumber, TimeStep, eVariableMagicNumber, eInvariantMagicNumber, 0 );
		  }
	      }
	  }

	BegLogLine( 0 )
	  << "p2 MSD_IF::GetNumberOfEnergyReportingUDFs(): " << MSD_IF::GetNumberOfEnergyReportingUDFs()
	  << EndLogLine;

	if( LastSimTickOfTimeStepFlag )
	  {
	    if( MyAddressSpaceId == 0 )
	      {
		if( TimeStep % RTG.mEmitEnergyTimeStepModulo == 0 )
		  ED_Emit_InformationUDFCount( TimeStep, MSD_IF::GetNumberOfEnergyReportingUDFs() + 0 ); //
		else
		  ED_Emit_InformationUDFCount( TimeStep, 0 ); // was 3
	      }
	  }

	SimtickLoopFinis.HitOE( PKTRACE_TIMESTEP,
				"TimeStep",
				MyAddressSpaceId,
				SimtickLoopFinis );

#ifdef PERFORMANCE_COUNTERS
	// ME
	if(SimTick==199) ApcStop(1);
#endif
	IterationCount++;
	//******************************************************************
      } // End of SimTick loop.
      Platform::Profiling::TurnOff() ;

#ifdef PERFORMANCE_COUNTERS
      ApcFinalize();
#endif
      if( MyAddressSpaceId == 0 )
	{
	  ED_Emit_ControlTheEnd( TimeStep );
	}

      // Flush out that last packets that may be buffered.
      BMM_RDG_BlockingSync( TimeStep );

      pkTraceServer::FlushAllBothBuffers();

      MSD_IF::Finalize();

      DoneFlag += 1 ;
#ifdef PK_PARALLEL
      Platform::Control::Barrier();
#endif

      // If we are in virtual node mode, make the IOP terminate
      BegLogLine(0)
        << "About to leave VoxelManagerThread"
        << EndLogLine ;

      return(NULL);
    }

#if !defined(IFP_NO_MAIN)
  extern "C" {

    extern char *rdgnameinargv;

    char * DvsFileNameArg ;

    static int ProcessArgv( int argc, char **argv, char **envp )
    {
      int argcount = 1 ;
      while( argcount < argc )
	{
	  // ADD COMMAND ARGUMENTS HANDLING HERE AND DON'T FORGET TO UPDATE argcount!!!

	  if( ! strcmp( argv[argcount], "-dvs" ) )
	    {
	      argcount++;
	      if( argc - argcount < 1 )
		{
		  PLATFORM_ABORT("-dvs needs a filename") ;
		}

	      DvsFileNameArg=argv[argcount] ;
	      argcount++;
	    }
	  else if( ! strcmp( argv[argcount], "-voxmesh" ) )
	    {
	      argcount++;

	      if( argc - argcount < 3 )
		PLATFORM_ABORT( "The -voxmesh flag requires three numerical arguments" );

	      int voxDimx = atoi( argv[argcount++] );
	      int voxDimy = atoi( argv[argcount++] );
	      int voxDimz = atoi( argv[argcount++] );

	      if( voxDimx < 0 || voxDimy < 0 || voxDimz < 0 )
		PLATFORM_ABORT( "bad arguments for -voxmesh" );

	      Platform::Topology::mVoxDimx = voxDimx;
	      Platform::Topology::mVoxDimy = voxDimy;
	      Platform::Topology::mVoxDimz = voxDimz;
	    }
	  else
	    {
	      fprintf ( stderr, "Use: progname pk_args -- app_args\n" ) ;
	      fprintf ( stderr, "Unknown app arg (%s)\n", argv[argcount]) ;
	      PLATFORM_ABORT( "Unknown flag on the command line" );
	    }
	}

      return 0;
    }

    // For a call like 'prog -procmesh 8 8 8 -- -dvs dvsfile'
    // things will get here with 'argv' pointing at the '--', and 'argc'
    // adjusted appropriately
    void *
    PkMain(int argc, char** argv, char** envp)
    {
      BegLogLine( PKFXLOG_SETUP )
	<< "Entering PkMain()"
	<< EndLogLine;

#if PKTRACE_TUNE
      pkTraceServer::Init();
      BegLogLine( PKFXLOG_VOXELMANAGER_ENTRY )
	<< "PkMain "
	<< "Done with pkTraceServer::Init"
	<< EndLogLine;
#endif
      Platform::Profiling::TurnOff() ; // Do not want 'startup' in a performance profile
      TraceClient InitialBarrierStart;
      TraceClient InitialBarrierFinis;

      // Try to sync up trace output.
      Platform::Control::Barrier();
      Platform::Control::Barrier();
      for( int b = 0; b < 10; b++ )
	{
	  Platform::Control::Barrier();
	  InitialBarrierStart.HitOE( 1,
				     "InitialBarrier",
				     0,
				     InitialBarrierStart );

	  Platform::Control::Barrier();
	  // sleep( b );

	  InitialBarrierFinis.HitOE( 1,
				     "InitialBarrier",
				     0,
				     InitialBarrierFinis );
	}

      TraceClient CalibrateTraceStart;
      TraceClient CalibrateTraceFinis;

      // Try to sync up trace output.
      Platform::Control::Barrier();
      Platform::Control::Barrier();
      if (   Platform::Topology::GetAddressSpaceId() == 0 )
	{
	  for( int b = 0; b < 1000; b++ )
	    {
	      CalibrateTraceStart.HitOE( 1,
					 "CalibrateTrace",
					 0,
					 CalibrateTraceStart );
	
	      if ( 0 == ( b & 1 ) )
		{
		  long long t = rts_get_timebase();
		  long long tend = t + (700 * 1000);
		  while( t < tend )
		    t = rts_get_timebase();
		} 
	
	      CalibrateTraceFinis.HitOE( 1,
					 "CalibrateTrace",
					 0,
					 CalibrateTraceFinis );
	    }
	}
      ProcessArgv( argc, argv, envp );

      BegLogLine( PKFXLOG_PKMAIN )
	<< argv[ 0 ]
	<< "::PkMain() running. "
	<< "DVS Fn >"
	<< DvsFileNameArg
	<< "< "
	<< EndLogLine;

      BegLogLine( PKFXLOG_SETUP )
	<< "PkMain(): About to BMM_RDG_Init()"
	<< EndLogLine;

      // Initialize the RDG transport system.
      if( rdgnameinargv )
	BMM_RDG_Init( rdgnameinargv );
      else
	BMM_RDG_Init( );

      DoneFlag = 0 ;

      BegLogLine( PKFXLOG_PKMAIN )
	<< " AtomicSet Done "
	<< EndLogLine;

      RTG.Init();

      BegLogLine( PKFXLOG_SETUP )
	<< "PkMain(): About to ReadDVSFile"
	<< EndLogLine;

      // Use filesystem to load DVS files.  Eventually, this will be handled via
      // External Datagram interface.

      pkNew( &StartingPositions, MSD_IF::GetNumberOfSites(), __FILE__, __LINE__ );
      if( StartingPositions == NULL )
	PLATFORM_ABORT( "StartingPositions == NULL" );

      pkNew( &StartingVelocities, MSD_IF::GetNumberOfSites(), __FILE__, __LINE__ );
      if( StartingVelocities == NULL )
	PLATFORM_ABORT( "StartingVelocities == NULL" );

      if( Platform::Topology::GetAddressSpaceId() == 0 )
	{
	  char * DvsFileName = DvsFileNameArg ;
	  char ModFileName[ 2048 ];
	  for( int dd=0; dd < 2048; dd++ )
	    ModFileName[ dd ] = 0;

	  if( Platform::Topology::mSubPartitionCount > 1 )
	    {
	      char PartitionId[ 1024 ];
	      sprintf(PartitionId, "%d", Platform::Topology::mSubPartitionId );
	      int partlen = strlen( PartitionId );
	      int dvssubstringlen = strlen( DVS_SUBSTRING );

	      int OriginalStringSearchStart = 0;
	      int EditedStringCatStart = 0;

	      for( int i = 0; 1; i++ )
		{
		  char * SubPartIdInStr = strstr( &(argv[ 1 ][ OriginalStringSearchStart ]), DVS_SUBSTRING );
		  if( SubPartIdInStr == NULL  )
		    {
		      if( i == 0 )
			{
			  fprintf(stderr, "WARNING: Multiple subpartitions requested. If required, add SubPartId (literary) field to the dvs path/filename. Ex. Rhod.SubPartId.dvs");
			  fflush(stderr);
			}

		      // Append the rest of the sting and break
		      int PathNameLength = strlen( argv[ 1 ] );

		      strncpy( &ModFileName[ EditedStringCatStart ],
			       &argv[ 1 ][ OriginalStringSearchStart ],
			       ( PathNameLength - OriginalStringSearchStart ) );

		      break;
		    }

		  char * StartOfCopy = (&argv[1][ OriginalStringSearchStart ]);
		  int len = SubPartIdInStr - StartOfCopy;
		  strncpy( &ModFileName[ EditedStringCatStart ], StartOfCopy, len );

		  BegLogLine( 1 )
		    << "EditedStringCatStart: " << EditedStringCatStart
		    << " len: " << len
		    << " partlen: " << partlen
		    << " dvssubstringlen: " << dvssubstringlen
		    << " ModFileName: " << ModFileName
		    << EndLogLine;

		  strcpy( &ModFileName[ EditedStringCatStart + len ], PartitionId );
		  EditedStringCatStart += ( len + partlen );
		  OriginalStringSearchStart += ( len + dvssubstringlen );

		  BegLogLine( 1 )
		    << "EditedStringCatStart: " << EditedStringCatStart
		    << " OriginalStringSearchStart: " << OriginalStringSearchStart
		    << " partlen: " << partlen
		    << " dvssubstringlen: " << dvssubstringlen
		    << " ModFileName: " << ModFileName
		    << EndLogLine;
		}

	      DvsFileName = ModFileName;
	    }

	  BegLogLine( 1 )
	    << "DvsFileName: " << DvsFileName
	    << EndLogLine;

	  ReadDVSFile( -1, // A flag to take the first complete set of dynamic vars found in file.
		       MSD_IF::GetNumberOfSites(),
		       DvsFileName,
		       MSD_IF::GetBoundingBox(),
		       DynVarMgrIF );
	}

      assert( MSD_IF::GetNumberOfSites() == DynVarMgrIF.GetNumberOfSites() );

#ifdef PK_PARALLEL
      Platform::Control::Barrier();

      Platform::Collective::Broadcast( (char *) StartingVelocities, DynVarMgrIF.GetNumberOfSites() * sizeof( XYZ ), 0 );

      BegLogLine( PKFXLOG_SETUP )
	<< "PkMain(): About to Broadcast2"
	<< EndLogLine;
      Platform::Collective::Broadcast( (char *) StartingPositions, DynVarMgrIF.GetNumberOfSites() * sizeof( XYZ ), 0 );

      BegLogLine( PKFXLOG_SETUP )
	<< "PkMain(): About to Broadcast3"
	<< EndLogLine;
      Platform::Collective::Broadcast( (char *) &RTG, sizeof( RTG ), 0 );

      BegLogLine( PKFXLOG_SETUP )
	<< "PkMain(): About to Broadcast4"
	<< " sizeof( DynVarMgrIF ): " << sizeof( DynVarMgrIF )
	<< EndLogLine;
      Platform::Collective::Broadcast( (char *) &DynVarMgrIF, sizeof( DynVarMgrIF ), 0 );

      Platform::Collective::Broadcast( (char *) &NormalDeviateIF, sizeof( NormalDeviateIF ), 0 );
#endif

      BegLogLine( PKFXLOG_SETUP )
	<< "PkMain(): About to LoadDynamicVariables"
	<< EndLogLine;

      // These loops MUST have all the sites available in a single voxel.
      DynVarMgrIF.InitLoadState();
      for( int SiteId = 0; SiteId < DynVarMgrIF.GetNumberOfSites(); SiteId++ )
	{
	  DynVarMgrIF.LoadDynamicVariables( SiteId, StartingPositions[ SiteId ], StartingVelocities[ SiteId ] );
	}

      delete [] StartingPositions;
      StartingPositions = NULL;
      delete [] StartingVelocities;
      StartingVelocities = NULL;

      // BGF This is a strange place for this call - but at least it must be after ReadDVSFile to know the starting time step.
      ED_Emit_InformationMolecularSystemDefinition
	(
	 RTG.mStartingTimeStep, 
	 0u,                                      //const unsigned         &aMSDCheckSum,
	 MSD_IF::GetNumberOfSites(),              //const unsigned         &aSiteCount,
	 1u                                       //const unsigned         &aFragmentCount
	 );

      MSD_IF::CheckMagicNumber();

      /****************************** CONFIGURATOR END ******************************/

      /*****************************************************************************
       *                            START YOUR ENGINES!!!
       *****************************************************************************/
      VoxelManagerThread();
      /*****************************************************************************/


      BegLogLine( PKFXLOG_PKMAIN )
	<< " PkMain():: Finito... "
	<< EndLogLine;

      // And there it is...

      return(NULL);
    }
  };
#endif
#endif
