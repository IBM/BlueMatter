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
 #ifndef __THREAD_IF__
#define __THREAD_IF__
#include <pk/platform.hpp>
#include <BlueMatter/MDVM_IF.hpp>
#include <BlueMatter/MDVM_UVP_IF.hpp>
#include <BlueMatter/MSD_IF.hpp>

//#include <pk/setpri.hpp>   // Needed by SchedTuneSelf()
#include <pk/pktrace.hpp>  // Needed by the pk tracing instrumentation
//#include <sys/processor.h> // Needed for processor binding
//#include <sys/thread.h>    // Needed by thread_self() to get the thread identifier for bindprocessor.

#ifndef GET_FINALIZE_TIME
#define GET_FINALIZE_TIME 0
#endif

#ifndef GET_INIT_TIME
#define GET_INIT_TIME 0
#endif

#ifndef GET_INTRAMOLECULAR_MDVM_RUN_TIME
#define GET_INTRAMOLECULAR_MDVM_RUN_TIME 0
#endif

#ifndef PKTRACE_MDVM_CNTL
#define PKTRACE_MDVM_CNTL (0)
#endif

#define CPU_COUNT 4

/********************************************************************
 *     Global Declaration of the IntraMolecularVM State
 *******************************************************************/


struct ThreadInput {
  Instruction*       mInstrs;
  int                mInstrSize;
  int                mSimTick;
};

MDVM                       ForceMdvmForThread[ THREAD_NUM ];
MDVM_UVP                   UpdateMdvmForThread[ THREAD_NUM ];

ThreadInput                Tinput[ THREAD_NUM ];
int                        ThreadIds[ THREAD_NUM ];
ThreadBarrier              StartMDVMBarrier;
ThreadBarrier              FinishMDVMBarrier;
TraceClient                MDVM_Start;
TraceClient                MDVM_Finis;
TraceClient                MDVM_Supervisor_Thread_Start;
TraceClient                MDVM_Supervisor_Thread_Finis;
DynamicVariableManager*    LocalSiteMgr;
int                        loadSiteCount[ THREAD_NUM ];

Instruction*               ForceInstrs[ MAX_RESPA_LEVEL ][ THREAD_NUM ];
int                        ForceInstrsSizes[MAX_RESPA_LEVEL][ THREAD_NUM ];

Instruction*               UpdatePositionShakeInstrs[ THREAD_NUM ];
int                        UpdatePositionShakeInstrsSizes[ THREAD_NUM ];

Instruction*               UpdateVelocityRattleInstrs[ THREAD_NUM ];
int                        UpdateVelocityRattleInstrsSizes[ THREAD_NUM ];

Instruction*               WaterInitInstrs[ THREAD_NUM ];
int                        WaterInitInstrsSizes[ THREAD_NUM ];


#ifdef ADD_FORCES_IN_ORDER
extern PartialForceUnit*          partialForceList[ THREAD_NUM ];
#endif

#if 0
extern "C" {
int trcon(int);
int trcoff(int);
};
#endif

/*********************************************************************/

void *IntraMolecularVMWrapper(void *in)
{
  int threadId = *(int *)in;

  //  cerr << "ThreadId: " << threadId  << endl;

  //int pthreadId = pthread_self();

  //cerr << "PThreadProcId: " <<  pthreadId  << endl;

  TraceClient MDVM_Thread_Start;
  TraceClient MDVM_Thread_Finis;

  int sWait = 0;
  int fWait = 0;

  //bindprocessor(BINDTHREAD, thread_self(), threadId % CPU_COUNT);

  int SimTick;
  int index;

  while(1)
    {
      StartMDVMBarrier.Wait( sWait++ );
      MDVM_Thread_Start.HitOE( PKTRACE_MDVM_CNTL,
                               "MDVM_Cntl",
                               threadId,
                               MDVM_Thread_Start );

      SimTick =   Tinput[ threadId ].mSimTick;

      //      index = ((SimTick+threadId)%(THREAD_NUM - 1)) + 1;
      index = threadId;

      ForceMdvmForThread[ index ].SetSimTick( SimTick );

      //trcon( 0 );
      IntraMolecularVM( Tinput[ index ].mInstrs,
                         Tinput[ index ].mInstrSize,
                         *LocalSiteMgr,
                         ForceMdvmForThread[ threadId ],
                         SimTick
// NEED: THIS IS UGLY. Needs changing...  .AR.
#ifdef ADD_FORCES_IN_ORDER
                         , partialForceList[ threadId ]
#endif
                         );

      MDVM_Thread_Finis.HitOE( PKTRACE_MDVM_CNTL,
                               "MDVM_Cntl",
                               threadId,
                               MDVM_Thread_Finis );

      FinishMDVMBarrier.Wait( fWait++ );
    }
}

class MDVM_ManagerIF
{
private:


  int                        mStartReady;
  int                        mFinishReady;

  inline
  void
  SetUpThreads()
    {

      int i = 1;
      while ( i < THREAD_NUM )
        {
          ForceMdvmForThread[ i ].Init();
          UpdateMdvmForThread[ i ].Init();
          ThreadIds[ i ] = i;

          Platform::Thread::Create( IntraMolecularVMWrapper,
                                    (void *)( &(ThreadIds[ i ]) ) );
          i++;
        }
      //  SchedTuneSelf();

    }


  inline
  void
  SetUpDataForThreads( Instruction **instrs,
                       int* instrsSize,
                       int simTick )
    {


      int i = 0;
      while ( i < THREAD_NUM )
        {
          Tinput[ i ].mInstrs       = instrs[ i ];
          Tinput[ i ].mInstrSize    = instrsSize[ i ];
          Tinput[ i ].mSimTick      = simTick;
          i++;
        }
    }


  inline
  void
  RunThreads( const int respaLevel, const int simTick )
    {

      SetUpDataForThreads( ForceInstrs[ respaLevel ],
                           ForceInstrsSizes[ respaLevel ],
                           simTick );


      MDVM_Start.HitOE( PKTRACE_MDVM_CNTL,
                                "MDVM_Cntl",
                                0,
                                MDVM_Start );

      StartMDVMBarrier.Wait( mStartReady++ );

    }

#ifdef ADD_FORCES_IN_ORDER
  inline
  void
  AddForces( int aSimTick )
    {
      int curLoadSiteCount;
      PartialForceUnit* pfu = NULL;

      for(int i=0; i < THREAD_NUM; i++)
        {
          curLoadSiteCount = loadSiteCount[i];

          for (int j=0; j < curLoadSiteCount; j++)
            {
              pfu = &partialForceList[i][j];

              for (int z=0; z < MSD_IF::Access().NumberOfRespaLevels; z++)
                {
                  LocalSiteMgr->AddForce( pfu->mSiteId, aSimTick, z, pfu->mPartialForce[z] );
                }
            }
        }
    }
#endif
public:
  enum  { DO_FORCE = 0,
          DO_UPDATE_POSITION_SHAKE = 1,
          DO_UPDATE_VELOCITY_RATTLE = 2,
          DO_WATER_INIT = 3
  };

  inline
  void Init( DynamicVariableManager* lsm, const int respaLevel )
    {
//////timebasestruct_t start, finish;
//////ReadTime(GET_INIT_TIME, start);
      assert (THREAD_NUM > 0);

      int rl;

      LocalSiteMgr = lsm;
      mStartReady = 0;
      mFinishReady = 0;

      //      WorkOrderHelp * workOrder = new WorkOrderHelp[ MSD_IF::GetWorkOrderSize() ];
      //      assert ( workOrder != NULL );

      //      MSD_IF::GetWorkOrder( workOrder );

      /***************************************************************************
       *                  DO the FORCE translation
       **************************************************************************/
      for (int rl = 0; rl <= respaLevel; rl++)
        Translate( ForceInstrs[rl], ForceInstrsSizes[rl], "UpdateForce", rl );

      Translate( WaterInitInstrs, WaterInitInstrsSizes, "WaterInit", 0);
      Translate( UpdatePositionShakeInstrs, UpdatePositionShakeInstrsSizes, "UpdateVelocityFirstHalfStep", 0);
      Translate( UpdateVelocityRattleInstrs, UpdateVelocityRattleInstrsSizes, "UpdateVelocitySecondHalfStep", 0 );

#ifdef ADD_FORCES_IN_ORDER
      for (int i=0; i < THREAD_NUM; i++)
        {
          assert (loadSiteCount[i] > 0);
          // partialForceList[i] = new PartialForceUnit[ loadSiteCount[i] ];
          pkNew( &partialForceList[i], loadSiteCount[i] );
          assert ( partialForceList[i] != NULL );
        }
#endif
      /****************************************************************************/


      ForceMdvmForThread[ 0 ].Init();
      UpdateMdvmForThread[ 0 ].Init();
      // //////////////////////////////  bindprocessor(BINDTHREAD,thread_self (), 0);

      StartMDVMBarrier.Init ( THREAD_NUM );
      FinishMDVMBarrier.Init( THREAD_NUM );

      if ( THREAD_NUM > 1 )
        {
          SetUpThreads();
        }

//////ReadTime(GET_INIT_TIME, finish);
//////PrintTime(GET_INIT_TIME, start, finish, "Time to initialize the IntraMolecularMDVM: ");
    }

  inline
  void
  Run( const int respaLevel, const int simTick, const int flag )
    {
//////timebasestruct_t start, finish;
//////PrintForPerformance( GET_INTRAMOLECULAR_MDVM_RUN_TIME, "SimTick: ", simTick);
//////ReadTime( GET_INTRAMOLECULAR_MDVM_RUN_TIME, start);

      if ( THREAD_NUM > 1 )
        {
          RunThreads( respaLevel, simTick );
        }



//      MDVM_Supervisor_Thread_Start.HitOE( PKTRACE_MDVM_CNTL,
//                                        "MDVM_Cntl",
//                                        0,
//                                        MDVM_Supervisor_Thread_Start );


      // Run the science on the supervisor thread
      int masterId = 0;

      switch (flag)
        {
        case DO_FORCE:
          {
            IntraMolecularVM(ForceInstrs[ respaLevel ][ masterId ],
                             ForceInstrsSizes[ respaLevel ][ masterId ],
                             *LocalSiteMgr,
                             ForceMdvmForThread[ masterId ],
                             simTick
// NEED: THIS IS UGLY. Needs changing...  .AR.
#ifdef ADD_FORCES_IN_ORDER
                             , partialForceList[ masterId ]
#endif
                             );
            break;
          }
        case DO_UPDATE_POSITION_SHAKE:
          {
            UpdateVPVM( UpdatePositionShakeInstrs[ masterId ],
                        UpdatePositionShakeInstrsSizes[ masterId ],
                        *LocalSiteMgr,
                        UpdateMdvmForThread[ masterId ],
                        simTick );
            break;
          }
        case DO_UPDATE_VELOCITY_RATTLE:
          {

            UpdateVPVM( UpdateVelocityRattleInstrs[ masterId ],
                        UpdateVelocityRattleInstrsSizes[ masterId ],
                        *LocalSiteMgr,
                        UpdateMdvmForThread[ masterId ],
                        simTick );
            break;
          }
        case DO_WATER_INIT:
          {

            UpdateVPVM( WaterInitInstrs[ masterId ],
                        WaterInitInstrsSizes[ masterId ],
                        *LocalSiteMgr,
                        UpdateMdvmForThread[ masterId ],
                        simTick );
            break;
          }
        default:
          {
            assert(0);
          }
        }

//       MDVM_Supervisor_Thread_Finis.HitOE( PKTRACE_MDVM_CNTL,
//                                        "MDVM_Cntl",
//                                        0,
//                                        MDVM_Supervisor_Thread_Finis );

//////ReadTime( GET_INTRAMOLECULAR_MDVM_RUN_TIME, finish);
//////PrintTime( GET_INTRAMOLECULAR_MDVM_RUN_TIME, start, finish, "Time for in loop calculation of IntraMolecularMDVM: ");
    }

  inline
  void
  Wait( int aSimTick )
    {
      FinishMDVMBarrier.Wait( mFinishReady++ );

#ifdef ADD_FORCES_IN_ORDER
      AddForces( aSimTick );
#endif

      MDVM_Finis.HitOE( PKTRACE_MDVM_CNTL,
                        "MDVM_Cntl",
                        0,
                        MDVM_Finis );
    }

  inline
  void
  ExportEnergy( const int currentOuterTimeStep )
  {
    if( THREAD_NUM > 1 )
      {
        for (int ei=1; ei < THREAD_NUM; ei++)
          {
            for (int ej=0; ej <= UDF_Binding::UDF_MAX_CODE; ej++)
              {
                ForceMdvmForThread[ 0 ].energyByUdfCode[ ej ] +=  ForceMdvmForThread[ ei ].energyByUdfCode[ ej ];
              }
          }
      }

    double KineticEnergy = 0.0;

    // NEED TO THINK ABOUT WHERE ALL THIS SHOULD LIVE
    // PROBABLY NEED TO CREATE A GLOBAL VALUE REDUCTION CLASS
#if 1
    // Compute kinetic energy for this fragment.
    /////////////////////for( int s=0; s < LocalSiteManager::GetNumberOfSites(); s++)
    for( int s=0; s < LocalSiteMgr->GetNumberOfSites(); s++)
      {
        KineticEnergy +=    0.5
          * MSD_IF::GetSiteInfo(s).mass
          * LocalSiteMgr->GetVelocity(s)
          * LocalSiteMgr->GetVelocity(s);
      }
    ED_Emit_UDFd1( currentOuterTimeStep, UDF_Binding::KineticEnergy_Code , 1, KineticEnergy);

    //BELOW IS A PURE HACK TO BE RESOLVED RESOLVED SOON
    // I REPEAT THIS IS A HACK!!! used to make the packet world happy.

    //    cerr << "Hack present in MDVM_ManagerIF.hpp::ExportEnergy()... needs packet resolvement!!!" << endl;
    ////////////ED_Emit_UDFd1( currentOuterTimeStep, UDF_Binding::StdHarmonicBondForce_Code , 1, 0.0);
    ////////////ED_Emit_UDFd1( currentOuterTimeStep, UDF_Binding::StdHarmonicAngleForce_Code , 1, 0.0);
#endif

    ForceMdvmForThread[ 0 ].ExportEnergy( currentOuterTimeStep );
  }

  inline
  void
  ZeroEnergy()
    {
      ForceMdvmForThread[ 0 ].ZeroEnergyAccumulators();

      if (THREAD_NUM > 1)
        for (int ei = 1; ei < THREAD_NUM; ei++)
          ForceMdvmForThread[ ei ].ZeroEnergyAccumulators();
    }

  inline
  void
  Finalize()
    {
//////timebasestruct_t start, finish;
//////ReadTime(GET_FINALIZE_TIME, start);

      for (int i = 0; i < MAX_RESPA_LEVEL; i++)
        for (int ei=0; ei < THREAD_NUM; ei++)
          if ( ForceInstrs[ i ][ ei ] != NULL )
            delete ForceInstrs[ i ][ ei ];

#ifdef ADD_FORCES_IN_ORDER
      for (int i=0; i < THREAD_NUM; i++)
        {
          delete []  partialForceList[i];
          partialForceList[i] = NULL;
        }

#endif

//////ReadTime(GET_FINALIZE_TIME, finish);
//////PrintTime(GET_FINALIZE_TIME, start, finish, "Time to finilize the IntraMolecularMDVM: ");
    }
};
#endif
