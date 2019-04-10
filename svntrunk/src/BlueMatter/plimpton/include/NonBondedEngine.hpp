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



/***************************************************************************
 * Project:         BlueMatter
 *
 * Module:          NonBondedEngine
 *
 * Purpose:         Manage active message driven NonBonded Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010522 BGF Created from design.
 *
 ***************************************************************************/

// This file contains active message infrastructure and nonbonded engine
// thread code.

#ifndef _NONBONDEDENGINE_HPP_
#define _NONBONDEDENGINE_HPP_

#include <pk/platform.hpp>


class PlimptonNonBondedEngine
  {
  public:



    // This structure is allocated to be indexed by the FragmentInstanceId
    // The number of possible fragment ids is known at startup and doesn't change during the run.
    // Declaring it 'unsigned short' limits a single engine to handling 64k fragments.
    // This array is read only by NB worker threads

    unsigned short * mFragmentInstanceMap;

    // These arrays are filled out per phase, as Fragments are completely arrived.
    // The number of arrivals has to do with the size and runtime distribution of the fragements in the system.
    // There arrays can be dynamically sized for where fragment migration won't happen.
    // These arrays can never be of greater bounds than the FragmentInstanceMap above.
    // These arrays are read only to NB worker threads

    // Think about padding to prevent false sharing here.
    int              mSourceFragmentCurrent;  // Index of the last fragment to complete.
    int              mSourceFragmentNext;
    unsigned short * mSourceFragmentArrived;

    // Think about padding to prevent false sharing here.
    Platform::Mutex::MutexWord      mTargetAssignmentMutex;
    int                             mTargetFragmentAssignedCount;  // Next entry to be picked up by worker thread.
    int                             mTargetFragmentArrivedCount;  // Index of the last fragment to complete.
    int                             mTargetFragmentExpectedCount;
    int                             mTargetFragmentNext;
    unsigned short                * mTargetFragmentArrived;
    // Think about padding to prevent false sharing here.


    // This array is dynamically sized based on the number of packets which will
    // arrive - both source and target.  This is directly related to the number of sites
    // which will arrive and the size of the packets.
    // This array can never be greater than the number of sites in the system.
    // This array is read only to the NB worker threads.
    int         OperandPacketPointerHeapNext;
    InputItem * OperandPacketPointerHeap;

    NonbondedFramworkAddressSpaceManager( int aMaxFragmentInstanceId,
                                          int aMaxSiteInstances         );
      {

      mFragmentInstanceMap = new unsigned short[ aMaxFragmentInstanceId ];
      assert( mFragmentInstanceMap != NULL );
      bzero( mFragmentInstanceMap, sizeof(unsigned short) * aMaxFragmentInstanceId  );

      // NEED: Have overallocated here... should sense the size of this array based on dynamic counts!
      mSourceFragmentArrived = new unsigned short[ aMaxFragmentInstanceId ];
      assert( mSourceFragmentArrived != NULL );
      bzero( mSourceFragmentArrived, sizeof(unsigned short) * aMaxFragmentInstanceId  );
      mSourceFragementNext = 1;

      // NEED: Have overallocated here... should sense the size of this array based on dynamic counts!
      mTargetFragmentArrived = new unsigned short[ aMaxFragmentInstanceId ];
      assert( mTargetFragmentArrived != NULL );
      bzero( mTargetFragmentArrived, sizeof(unsigned short) * aMaxFragmentInstanceId  );
      mTargetFragmentNext = 1;

      OperandPacketPointerHeapNext = 1;
      OperandPacketPointerHeap     = new XYZ[ aMaxSiteInstances ];
      assert( OperandPacketPointerHeap != NULL );
      bzero( OperandPacketPointerHeap, sizeof( InputItem * ) * aMaxSiteInstances );

      }

    // Initialization packet - must arrive from every node on ones row and column
    class InitializationActorPacket
      {
      public:
        unsigned mTimeStep;
        unsigned mSourceTask;
        unsigned mFragmentCount;
      };

    static int
    NonBondedEngineInitializationPacket( void * pkt )
      {
      InitializationActorPacket * ap = (InitializationActorPacket *) pkt;

      ExpectedFragmentCount += ap->mFragmentCount;

      InitializationPacketCount++;

      }

    //
    class OperandActorPacket
      {
      public:
        class Header
          {
          public:
            unsigned  1 :    mSinglePacketFragment;  // Fragment dynamic data fits in a single packet payload.
            unsigned  1 :    mSourceFlag;   // Source data only provides one of the pair operands
            unsigned  1 :    mTargetFlag;   // Target data is both an operand AND a flag to return the force.
            unsigned  8 :    mFragmentPacketNumber; // Which packet is the one arriving.  Total expected is deterministic.
            unsigned 16 :    mSourceTask;   // Where to return the forces.  Could be brought over in a setup phase.
            int              mFragmentInstanceId;      // fragment instance id - used to fish in msd file.
          };
        XYZ mPosition[ Platform::Reactor::PAYLOAD_SIZE - sizeof( Header ) ];  // site posit over-subscripted array of site dyanamic data
      };

    static
    int
    NonBondedOperandActorFunction( void * pkt )
      {
      OperandActorPacket *ap = ( OperandActorPacket *) pkt;

      int FragmentPacketHeapBaseIndex =  mFragmentInstanceMap[ mFragmentInstanceId ];

      if( FragmentPacketHeapBaseIndex == 0 )
        {
        //NEED: If there are to be multiple reactor engines, we need a lock here.  Check platform.

        // Figure out which batch of packet pointers will be used for this fragment.
        // Make sure to set up FragmentPacketBaseIndex here.
        }

      // Calculate where the incoming data should live.
      int FragmentPacketHeapIndex = FragmentPacketHeapBaseIndex = ap->Header.mFragmentPacketNumber;

      FragmentPacketHeap[ FragmentPacketHeapIndex ] = ap;

      //NEED: get multi-packet fragment infrastructure in.
      //NEED: For now, only allow single packet fragments which means
      //NEED: a site limit of (payload_size - sizeof(header)) / 24 ... potential about 4 to 12
      assert( ap->Header.mSinglePacketFlag );

      if( 1 ) //NEED: this needs to be if( last packet of fragment )
        {

        if( ap->Header.mSourceFlag )
          {
          //NEED: the following assumes a single reactor
          mSourceFragmentArrived[ mSourceFragmentArrivedNext ] = FragmentPacketHeapBaseIndex;
          mSourceFragmentArrivedNext++;
          }

        if( ap->Header.mTargetFlag )
          {
          //NEED: the following assumes a single reactor
          mTargetFragmentArrived[ mTargetFragmentArrivedNext ] = FragmentPacketHeapBaseIndex;
          mTargetFragmentArrivedNext++;
          }
        }

      return( Platform::Reactor::RETURN_DETATCH_BUFFER );
      }


    static void
    WorkThread( void * arg )
      {
      int CurrentTargetIndex = -1;
      while( 1 )
        {
        // Wait for a complete target fragment to appear to be ready to be worked on.
        while( mTargetFragmentNextToAssign < mTargetFragmentLastToArrive )
          Platform::Thread::Yield();

        // Lock mutex in case multiple WorkerThreads are running - if fail, go arround.
        if( ! Platform::Mutex::TryLock(  mTargetAssignmentMutex ) )
          continue;

        CurrentTargetIndex = mTargetFragmentNextToAssign;

        mTargetFragmentNextToAssign++;

        Platform::Mutex::Unlock( mTargetAssignmentMutex );

        for( SourceIndex = 0; SourceIndex < mSourceFragmentTotal; SourceIndex++ )
          {
          // Wait for a complete target fragment to appear to be ready to be worked on.
          while( mSourceFragmentNextToAssign < mSourceFragmentLastToArrive )
            Platform::Thread::Yield();

          DoFragmentInteraction( CurrentTargetIndex, SourceIndex );
          }

        DeliverResults( CurrentTargetIndex );



      double EwaldDummy = 0.0;

      ED_Emit_Energy_Nonbonded( mCurrentOuterTimeStep,  // this is developed by counting phases.
                                mOrdinal,
                                mPlimptonizorCount,
                                mChargeEnergyPartialSum,
                                mLJEnergyPartialSum,
                                EwaldDummy );


        }
      };

    class OutputType
      {
      public:
        class Header
          {
          public:
          unsigned  8 : mFragmentPacketNumber;
          int           mFragmentInstanceId;
          }
        XYZ mPosition[ Platform::Reactor::PAYLOAD_SIZE - sizeof( Header ) ];
      };



    PPLUSERIF_RC
    FinalizeProcessing()
      {
      // Currently ALL processing done here
      BegLogLine(0)
        << "PartProd_Pairwise "
        << " TargetResults " << TargetResultNext
        << " SourceOperands " << SourceOperandNext
        << EndLogLine;

      for( int r = 0; r < TargetResultNext; r++ )
        {
        for( int c = 0; c < SourceOperandNext; c++ )
          {
          tResult result;
          double  lje = 0.0;
          double  che = 0.0;
          PairwiseOp::Op( mOrdinal,
                          mCurrentSimTick,
                          result,
                          mTargetResults[ r ].mTargetOperand,
                          mSourceOperands[ c ],
                          lje,
                          che
                          );

          //Accumulate the energy.
          //While pair computation is done twice, energy is only accumulated once.
          if( mTargetResults[ r ].mTargetOperand.mAbsoluteSiteId < mSourceOperands[ c ].mAbsoluteSiteId  )
            {
            //NEED: the following doesn't work distributed VOXELS!!!!!!!!!!
            BegLogLine(0)
              << " LRFORCE "
              << " AbsId Target " <<  mTargetResults[ r ].mTargetOperand.mAbsoluteSiteId
              << " AbsId Source " <<  mSourceOperands[ c ].mAbsoluteSiteId
              << " lje " << lje
              << " che " << che
              << EndLogLine;

            mChargeEnergyPartialSum += che;
            mLJEnergyPartialSum     += lje;

            }

          ReduceOp::Op( mTargetResults[ r ].mOutputItem.mResult,
                        mTargetResults[ r ].mOutputItem.mResult,
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


      OutputItemIndex = TargetResultNext - 1;
      if( TargetResultNext )
        return( PPLUSERIF_RC_AcceptWithOutputReady );
      else
        return( PPLUSERIF_RC_Accept ); // unclear whether this is a valid code path.
      }

  };


#endif
