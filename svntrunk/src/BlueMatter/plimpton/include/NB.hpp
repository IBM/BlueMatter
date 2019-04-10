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
 * Purpose:         Manage active message driven NonBonded Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010522 BGF Created from design.
 *
 ***************************************************************************/
// This file contains active message infrastructure and nonbonded engine
// thread code.

// Currently, this code does not have an initialization phase.  This leads to
// some extra data in operand packets and also makes it impossible to
// dynamically allocate some of the structures.  These are allocated based
// on a compilte time limit.  THIS CAN ALL BE CHANGED AS WE GO ALONG.

#ifndef _PLIMPNONBONDEDENGINE_HPP_
#define _PLIMPNONBONDEDENGINE_HPP_

#ifndef PKFXLOG_NBVM
#define PKFXLOG_NBVM 0
#endif

#ifndef PKFXLOG_SITEFORCES_NB
#define PKFXLOG_SITEFORCES_NB 0
#endif

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/NBVM_IF.hpp>
#include <BlueMatter/IrreducibleFragmentPair.hpp>

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/Grid2D.hpp>
#include <pk/ActorMulticast.hpp>

#include <string.h>  // for bzero()

template<class DynVarMgrIF, class NBMV>
class PlimptonNonBondedEngine
  {
  public:

    /*
     * Forward declare client and server state structures
     */
    class   Server;
    typedef Server * ServerRef;  // For now, this is just a pointer.  But we might keep something else as a ref.

    class   ClientState;
    typedef ClientState * ClientRef;

    enum{ TargetRowFlag = 1, SourceColFlag = 0 };

    enum{ MAX_FRAGMENTS_HANDLED = 0x0000FFFF };  // 64K frags (max unsigned short) worth.

    /*
     * Declare the packet structures.
     */

    class ConnectActorPacket
      {
      public:
        ServerRef          mServerRef  ; // Pointer to the NBE targeted.
        int                mSourceOrTargetFlag;
        int                mSourceTask        ;
        Grid2D::Coordinate mCoordinate        ;
        int                mClientIrreducibleFragmentCount;
        int                mSimTickToStart;
        Grid2D             mGrid2D;
      };


    class OperandActorPacket
      {
      public:
        class Header
          {
          // NOTE: this header could be compressed in all sorts of ways.
          //       Should try to get it under 8 bytes, no?
          public:
            ServerRef mServerRef  ; // Could could could get this out of here... by creating a static singleton.
            ClientRef mClientRef  ;
            int       mIrreducibleFragmentId          ; // fragment instance id - used to fish in msd file.
            unsigned  mSinglePacketFragment :    1 ; // Fragment dynamic data fits in a single packet payload.
            unsigned  mSourceColFlag           :    1 ;
            unsigned  mTargetRowFlag           :    1 ;
            unsigned  mFragmentPacketNumber :    8 ; // Which packet is the one arriving.  Total expected is deterministic.
            unsigned  mSourceTask           :   16 ; // Where to return the forces.  Could be brought over in a Connect phase.
            unsigned  mSimTick;
          };
        Header mHeader;
        enum { NumberOfPositions = ( Platform::Reactor::PAYLOAD_SIZE - sizeof( Header ) ) / sizeof( XYZ ) };
        XYZ    mPositions[ NumberOfPositions ];  // site posit over-subscripted array of site dyanamic data
      };


    class TargetResultsActorPacket
      {
      public:
        class Header
          {
          public:
            ClientRef mClientRef  ;
            unsigned  mSinglePacketFragment :    1 ; // Fragment dynamic data fits in a single packet payload.
            unsigned  mFragmentPacketNumber :    8 ; // Which packet is the one arriving.  Total expected is deterministic.
            int       mIrreducibleFragmentId          ; // fragment instance id - used to fish in msd file.
          };
        Header mHeader;
        enum { NumberOfForces = ( Platform::Reactor::PAYLOAD_SIZE - sizeof( Header ) ) / sizeof( XYZ ) };
        XYZ    mForce[ NumberOfForces ];  // site posit over-subscripted array of site dyanamic data

      };


    // For now, the LocalServer of a fragment will be it's operand packet.
    // This will need to be changed to allow for persistant tables.
    // This will need to be changed to allow for multi packet operands.
    typedef OperandActorPacket * OperandActorPacketPtr;

    class ClientState
      {
      public:

        ServerRef                   mServerRef;

        DynVarMgrIF                *mDynVarMgrIF;   //Type for this should come in as a template interface
        unsigned                    mSimTick;
        unsigned                    mSimTicksPerOuterTimeStep; // assumes Plimptonize only happens at OTS
        int                         mIntegratorLevel;

        Grid2D                      mGrid2D;
        int                         mTargetResultsArrivedCount;
        int                         mTargetResultsExpected;

        int                         mTargetResultsDoneFlag;

        static
        int
        TargetResults_ActorFx( void * pkt )
          {
          TargetResultsActorPacket *ap = ( TargetResultsActorPacket *) pkt;
          BegLogLine(1)
            << "TargeResults_ActorFx(): "
            << " START "
            << " Client@ "
            << (void *) ap->mHeader.mClientRef
            << " IrreducibleFragmentId "
            << ap->mHeader.mIrreducibleFragmentId
            << EndLogLine;

          ClientState &cs = * ap->mHeader.mClientRef;

          int IrreducibleFragmentId = ap->mHeader.mIrreducibleFragmentId;

          int LocalIrreducibleFragmentBaseIndex =
                   cs.mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( IrreducibleFragmentId );

          // NEED: think about the protocol .... zero index is reserved for CenterOfMassForce
          // This is no good, since if we have a single atom fragment, the force is the same as the
          // atom force.
          cs.mDynVarMgrIF->AddCenterOfMassForce( IrreducibleFragmentId, ap->mForce[ 0 ] );

#ifdef CHECK_COMMASSFORCE
          XYZ COMMass;
          COMMass.Zero();
#endif
          for( int s = 0;
                   s < MSD_IF::GetIrreducibleFragmentMemberSiteCount( IrreducibleFragmentId );
                   s++ )
            {
            int LocalSiteIndex =  LocalIrreducibleFragmentBaseIndex + s ;

            BegLogLine(1)
              << "TargetResults_ActorFx(): "
              << " IrrdeducibleFragmentId "
              << IrreducibleFragmentId
              << " Local Site Index "
              << LocalSiteIndex
              << " Force "
              << ap->mForce[ s ]
              << EndLogLine;

            // NOTE - we index s + 1 since protocol reservse index 0 for COM force.
            cs.mDynVarMgrIF->AddForce( LocalSiteIndex, cs.mSimTick, cs.mIntegratorLevel, ap->mForce[ s + 1 ] );

#ifdef CHECK_COMMASSFORCE
            COMMass += ap->mForce[ s + 1 ];
#endif
            }

#ifdef CHECK_COMMASSFORCE
          cerr << " COMMass " << COMMass << " ap->COMMass " << ap->mForce[ 0 ] << " Difference " << COMMass - ap->mForce[0] << endl;
#endif
          cs.mTargetResultsArrivedCount++;

          BegLogLine(1)
            << "TargetResults_ActorFx(): "
            << " FINISH "
            << " Client@ "
            << (void *) ap->mHeader.mClientRef
            << " TargetResultsArrivedCount "
            << cs.mTargetResultsArrivedCount
            << " FragInst "
            << ap->mHeader.mIrreducibleFragmentId
            << EndLogLine;

          BegLogLine(1)
            << "TargetResults_ActorFx(): "
            << " TargetResultsArrivedCount "
            << cs.mTargetResultsArrivedCount
            << " Grid dimension X"
            << cs.mGrid2D.GetXDimension()
            << " Y "
            << cs.mGrid2D.GetYDimension()
            << " F[0] "
            << ap->mForce[ 0 ]
            << EndLogLine;

          if( cs.mTargetResultsArrivedCount == cs.mTargetResultsExpected )
            {
            BegLogLine(1)
              << "TargetResults_ActorFx(): "
              << "LAST RESULT PACKET IN "
              << EndLogLine;

Platform::Memory::ExportFence();
            cs.mTargetResultsDoneFlag = 1;

            }

          return( Platform::Reactor::RETURN_OK );
          }


      };




    typedef OperandActorPacket * OperandActorPacketPointer;

    /*
     * Declare the client and server state structures.
     */
    class Server
      {
      public:
        int                             mSimTick;

        int                             mConnectPacketsReceivedCount;
        int                             mConnectionComplete;

        // For now, we will have a static allocation for incoming fragment packet pointers.
        OperandActorPacketPtr     mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ MAX_FRAGMENTS_HANDLED ];  //NEED: make dynamic.

        Platform::Mutex::MutexWord      mSourceAssignmentMutex;
        unsigned short                  mSourceFragmentArrived[ MAX_FRAGMENTS_HANDLED ]; //NEED: make dynamic
        int                             mSourceFragmentArrivedCount;
        int                             mSourceFragmentTotalCount;

        Platform::Mutex::MutexWord      mTargetAssignmentMutex;
        unsigned short                  mTargetFragmentArrived[ MAX_FRAGMENTS_HANDLED ];
        int                             mTargetFragmentAssignedCount;  // Next entry to be picked up by worker thread.
        int                             mTargetFragmentArrivedCount;  // Index of the last fragment to complete.
        int                             mTargetFragmentTotalCount;

        Grid2D                          mGrid2D;

        Platform::Mutex::MutexWord      mWorkThreadAssignmentMutex;

        void                         ** mPersistantStatePointerArray;


        static
        int
        Connect_ActorFx( void * pkt )
          {
          ConnectActorPacket *ap = (ConnectActorPacket *) pkt;
          Server &LocalServer    =  *(ap->mServerRef);  // Symbolic short hand.

          LocalServer.mSimTick   = ap->mSimTickToStart;

          BegLogLine(1)
            << "Connetc_ActorFx(): "
            << " @ "
            << (void *) Connect_ActorFx
            << " Server.SimTick "
            << LocalServer.mSimTick
            << " SourceTask "
            << ap->mSourceTask
            << EndLogLine;

          // Either it's the first one or it must match.
          // NEED: NEED NEED LOCK FOR MULTI-REACTOR ADDRESS SPACE.
          if( LocalServer.mGrid2D.IsInitialized() )
            assert( LocalServer.mGrid2D  == ap->mGrid2D );
          else
            LocalServer.mGrid2D  = ap->mGrid2D;

          // Check that the local mapping comes up with a consistant AddressSpaceID -> Grid Coord.
          assert( LocalServer.mGrid2D.GetCoordinate( ap->mSourceTask ) == ap->mCoordinate );
          if( ap->mSourceOrTargetFlag == TargetRowFlag )
            LocalServer.mTargetFragmentTotalCount += ap->mClientIrreducibleFragmentCount;
          else
            LocalServer.mSourceFragmentTotalCount += ap->mClientIrreducibleFragmentCount;

          BegLogLine(1)
            << "Connetc_ActorFx(): "
            << " @ "
            << (void *) Connect_ActorFx
            << " Server.SimTick "
            << LocalServer.mSimTick
            << " SourceTask "
            << ap->mSourceTask
            << " Total Targ/Src "
            << LocalServer.mTargetFragmentTotalCount
            << " / "
            << LocalServer.mSourceFragmentTotalCount
            << EndLogLine;

          // We've got everything once this count reaches the right number.
          LocalServer.mConnectPacketsReceivedCount++;

          // Check if we've received all the connexts expected.
          if( LocalServer.mConnectPacketsReceivedCount
               ==
#if 1 // NEED: to complete voxel based connection.
               ( Platform::Topology::GetAddressSpaceId() == 0) ? 2 : 1 )  // FOR NOW, wait for 2 connects localy and 1 on row col neighbors
#else
                 ( LocalServer.mGrid2D.GetXDimension() +
                   LocalServer.mGrid2D.GetYDimension()   )  )
#endif
            {

            // Allocate space for persistant state pointer used by PartitionPair method

            int NumberOfIrreducibleParitionPairCombinations =
                           LocalServer.mTargetFragmentTotalCount
                           *
                           LocalServer.mSourceFragmentTotalCount;

            typedef void *VoidPtr;  // just short hand.

            LocalServer.mPersistantStatePointerArray =
              new VoidPtr[  NumberOfIrreducibleParitionPairCombinations  ];

            assert( LocalServer.mPersistantStatePointerArray != NULL );

            for(int i = 0; i < NumberOfIrreducibleParitionPairCombinations; i++ )
              LocalServer.mPersistantStatePointerArray[ i ] = NULL;


            BegLogLine(1)
              << "Connetc_ActorFx(): FINISHED "
              << " @ "
              << (void *) Connect_ActorFx
              << " Server.SimTick "
              << LocalServer.mSimTick
              << " SourceTask "
              << ap->mSourceTask
              << " Total Targ/Src "
              << LocalServer.mTargetFragmentTotalCount
              << " / "
              << LocalServer.mSourceFragmentTotalCount
              << EndLogLine;

Platform::Memory::ExportFence();
            LocalServer.mConnectionComplete = 1;
            }


          return( Platform::Reactor::RETURN_OK );
          }


        static
        int
        Operand_ActorFx( void * pkt )
          {
          // THIS METHOD ASSUMES A SINGLE REACTOR - NEED FURTHER SYNC FOR MULTI-REACTOR MODE.

          OperandActorPacket *ap          =   ( OperandActorPacket *) pkt;
          Server             &LocalServer = * ( ap->mHeader.mServerRef );  // Symbolic short hand.

          BegLogLine(1)
            << "Operand_ActorFx(): "
            << " @ "
            << (void *)Operand_ActorFx
            << " Server.SimTick "
            << LocalServer.mSimTick
            << " Operand.SimTick "
            << ap->mHeader.mSimTick
            << " Incoming Fragment Id "
            << ap->mHeader.mIrreducibleFragmentId
            << " SourceTask "
            << ap->mHeader.mSourceTask
            << " Op Src/Trg "
            << ap->mHeader.mSourceColFlag
            << ":"
            << ap->mHeader.mTargetRowFlag
            << EndLogLine;


          assert( ap->mHeader.mSimTick == LocalServer.mSimTick );

          int    FragInstId = ap->mHeader.mIrreducibleFragmentId;

          // Make sure not to over index the map.
          assert( FragInstId < PlimptonNonBondedEngine::MAX_FRAGMENTS_HANDLED );

          // The block deals with the case where the ariving packet is targetted at a filled slot.
          // This is possible when a fragment is sent as both a source and target to it's own address space.
          // NEED: Becausse this could also mean bad things, we try to check that it is an acceptable dupilcate.
          // NEED: it would be best to arange not to send duplicates!!!!!!
          int Rc;
          if( NULL != LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ] )
            {

            assert( (
                    ap->mHeader.mIrreducibleFragmentId
                    ==
                    LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ]->mHeader.mIrreducibleFragmentId
                    )
                  &&
                    (
                    ap->mHeader.mSourceTask
                    ==
                    LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ]->mHeader.mSourceTask
                    )
                  );

            // OR in the source/target characturistics of the the duplicate.
            LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ]->mHeader.mSourceColFlag |=
                        ap->mHeader.mSourceColFlag;

            LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ]->mHeader.mTargetRowFlag |=
                        ap->mHeader.mTargetRowFlag;

            Rc = Platform::Reactor::RETURN_OK ;
            }
          else
            {
            // Assign the pointer to the incomming packet to the Local Server.  This memory will be given back
            // later within by the WorkThread.
            LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ] = ap;
            Rc = Platform::Reactor::RETURN_DETATCH_BUFFER ;
            }

          if( ap->mHeader.mSourceColFlag )
            {
            BegLogLine(1)
              << "Operand_ActorFx(): "
              << " SOURCE Frag Arrived "
              << " Server.SimTick "
              << LocalServer.mSimTick
              << " FragInstID "
              << FragInstId
              << " FragArrivedIndex "
              << LocalServer.mSourceFragmentArrivedCount
              << EndLogLine;

            Platform::Mutex::YieldLock( LocalServer.mSourceAssignmentMutex );  // import fence and spinlock

            LocalServer.mSourceFragmentArrived[ LocalServer.mSourceFragmentArrivedCount ] = FragInstId;

            Platform::Memory::ExportFence();  // Send the pointer out.

            LocalServer.mSourceFragmentArrivedCount++;

            Platform::Mutex::Unlock( LocalServer.mSourceAssignmentMutex );

            }

          if( ap->mHeader.mTargetRowFlag )
            {

            BegLogLine(1)
              << "Operand_ActorFx(): "
              << " TARGET Frag Arrived "
              << " Server.SimTick "
              << LocalServer.mSimTick
              << " FragInstID "
              << FragInstId
              << " FragArrivedIndex "
              << LocalServer.mSourceFragmentArrivedCount
              << EndLogLine;

            Platform::Mutex::YieldLock( LocalServer.mTargetAssignmentMutex );  // import fence and spinlock

            LocalServer.mTargetFragmentArrived[ LocalServer.mTargetFragmentArrivedCount ] = FragInstId;

            Platform::Memory::ExportFence();  // Send the pointer out.

            LocalServer.mTargetFragmentArrivedCount++;

            Platform::Mutex::Unlock( LocalServer.mTargetAssignmentMutex );
            }

          BegLogLine(1)
            << "Operand_ActorFx(): "
            << " DONE "
            << " Server.SimTick "
            << LocalServer.mSimTick
            << " FragInstID "
            << FragInstId
            << " SourceTask "
            << ap->mHeader.mSourceTask
            << " LocalServer Src/Trg "
            << LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ]->mHeader.mSourceColFlag
            << ":"
            << LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ FragInstId ]->mHeader.mTargetRowFlag
            << EndLogLine;

          return( Rc );
          }


        class IrreducibleFragmentIF
          {
          // This class wraps the OperandActorPacket and provides interfaces suitable for
          // the PartitionPair() method.
          // THESE COULD JUST BE METHODS ON THE OPERAND PACKET ITSELF UNLESS THERE IS OTHER STATE!!!!!
          public:

            inline
            XYZ
            GetCenterOfMassPosition()
              {
              // THIS ROUTINE SHOULD FIND THE CenterOfMass IN THE MSD
              int FragmentId        = GetIrreducibleFragmentId();
              int FragmentSiteCount = GetIrreducibleFragmentMemberCount();

              double TotalMass = 0.0;
              XYZ    SumPositionTimesMass;
              SumPositionTimesMass.Zero();

              for(int FragmentSiteIndex = 0;
                      FragmentSiteIndex < FragmentSiteCount;
                      FragmentSiteIndex++ )
                {
                int AbsSiteId =
                    MSD_IF::GetIrreducibleFragmentMemberSiteId( FragmentId, FragmentSiteIndex );

                TotalMass += MSD_IF::GetMass( AbsSiteId );

                SumPositionTimesMass  += *(GetPositionPtr( FragmentSiteIndex )) * MSD_IF::GetMass( AbsSiteId );

                }
                XYZ CenterOfMassPosition = SumPositionTimesMass / TotalMass;

              return( CenterOfMassPosition );
              }


            OperandActorPacketPtr mOperandPacketPtr;

            // NB infrastructure methods
            OperandActorPacketPtr
            GetPacketAddress()
              {
              return( mOperandPacketPtr );
              }

            inline int
            GetSimTick()
              {
              int rc = mOperandPacketPtr->mHeader.mSimTick;
              return( rc );
              }

            // PartitionPair required methods:
            IrreducibleFragmentIF( OperandActorPacketPtr aOperandPacketPtr )
              {
              mOperandPacketPtr =  aOperandPacketPtr ;
              }

            inline int
            GetIrreducibleFragmentId()
              {
              int rc = mOperandPacketPtr->mHeader.mIrreducibleFragmentId;
              return( rc );
              }

            inline int
            GetIrreducibleFragmentMemberCount()
              {
              int rc =  MSD_IF::GetIrreducibleFragmentMemberSiteCount( GetIrreducibleFragmentId() );
              return( rc );
              }

            inline
            XYZ *             //NEED: is this a problem?  we copying?????  should we be returning a pointer?
            GetPositionPtr( int aIndex )
              {
              // NEED: Get some bounds checking in here!
              XYZ * rc = & ( mOperandPacketPtr->mPositions[ aIndex ] );
              return( rc );
              }
          };

        class TargetResultsPacketIF
          {
          public:
            TargetResultsActorPacket * mTargetResultsPacket;

            TargetResultsPacketIF( TargetResultsActorPacket * aTargetResultsPacket )
              {
              mTargetResultsPacket = aTargetResultsPacket;
              }

            inline
            void
            AddCenterOfMassForce( XYZ aForce)
              {
              ////mDynVarMgrIF->AddCenterOfMassForce( mFragmentOrdinal, aForce );

              // NEED: THINK: here we reserve the zero index for CenterOfMassForce which is not always needed.
              mTargetResultsPacket->mForce[ 0 ] += aForce;

              }



            // This method allows passing this packet directly to
            inline
            void
            AddForce( int aIndex, XYZ aForce )
              {
              // NEED: THINK: here we reserve the zero index for CenterOfMassForce which is not always needed.
              // THE PROTOCOL IS THAT THE 0 INDEX IS RESERVED FOR THE COM FORCE...
              // NEED: think about this because we don't always need it... could chage
              // the protocol when pressure control is turned off.
              assert( aIndex + 1 < TargetResultsActorPacket::NumberOfForces );
              mTargetResultsPacket->mForce[ aIndex + 1 ] += aForce;
              }

          };

        class GlobalPartialResultsIF
          {
          public:


            double mTotalLennardJonesEnergy;
            double mTotalLennardJonesDHDLamda;
            XYZ    mTotalLennardJonesVirial;
            double mTotalChargeEnergy;
            double mTotalChargeDHDLamda;
            XYZ    mTotalChargeVirial;

            void
            Zero()
              {
              mTotalLennardJonesEnergy    = 0.0 ;
              mTotalLennardJonesDHDLamda  = 0.0 ;
              mTotalLennardJonesVirial.Zero();
              mTotalChargeEnergy          = 0.0 ;
              mTotalChargeDHDLamda        = 0.0 ;
              mTotalChargeVirial.Zero();
              }

            inline
            void
            AddEnergy( int aUDF_Code, double aEnergy )
              {
              switch( aUDF_Code )
                {
                case UDF_Binding::StdChargeForce_Code :
                  {
                  mTotalChargeEnergy += aEnergy;
                  break;
                  }
                case UDF_Binding::LennardJonesForce_Code :
                  {
                  mTotalLennardJonesEnergy += aEnergy;
                  break;
                  }
                default :
                  assert(0);
                };
              }
          };

        static void *
        WorkThread( void * ptr )
          {
          Server &LocalServer = **((Server **)ptr);

          BegLogLine(1)
            << "WorkThread(): "
            << " running. "
            << " Server @ "
            << (void *) &LocalServer
            << EndLogLine;


          // Make sure Connect is done.
          while( LocalServer.mConnectionComplete == 0 )
            Platform::Thread::Yield();

          // This wrapper is handled in a silly way - TargetResultsPacketIF _could_ just inherit the packet struct to add methods.
          TargetResultsActorPacket TargetResultsPacket;
          TargetResultsPacketIF    TargetResultsIF( &TargetResultsPacket );

          ED_Emit_InformationRunTimeConfiguration( 0u,  // zero for now, but needs to be set when this context knows which simtick it is working on.
                                                   1u,          // Since the plimptonizer only works with 1 voxel right now
                                                   1u ); // Total number of Nonbonded engines in use.

          GlobalPartialResultsIF wte;
          wte.Zero();

          int AssignedTargetFragmentArrivedIndex = 0;
          while( 1 )
            {
            BegLogLine(1)<< "WorkThread(): "<< " Top of loop "<< EndLogLine;

            // In case there are multiple worker thread, enter an atomic secontion with lock.

            Platform::Mutex::YieldLock( LocalServer.mWorkThreadAssignmentMutex );

            BegLogLine(1)
              << "WorkThread(): "
              << " Locked to be next to assign.  Waiting for: "
              << LocalServer.mTargetFragmentAssignedCount
              << " Frags Arrived (Source/Target): "
              << LocalServer.mSourceFragmentArrivedCount
              << " / "
              << LocalServer.mTargetFragmentArrivedCount
              << " Frags Expected (Souce/Target): "
              << LocalServer.mSourceFragmentTotalCount
              << " /  "
              << LocalServer.mTargetFragmentTotalCount
              << EndLogLine;

            // Now that this is the chosen thread,
            // wait for a complete target fragment to appear to be ready to be worked on.

            while( LocalServer.mTargetFragmentAssignedCount == LocalServer.mTargetFragmentArrivedCount )
              {
              Platform::Thread::Yield();
              Platform::Memory::ImportFence();
              }

            // Pick up an index into the mTargetFragmentArrived array.
            // NOTE: if we happen to pick up the last one, when were done we free up all source frags
            AssignedTargetFragmentArrivedIndex =
                   LocalServer.mTargetFragmentAssignedCount ;

            LocalServer.mTargetFragmentAssignedCount++ ;

            BegLogLine(1)
              << "WorkThread(): "
              << " Selected "
              << AssignedTargetFragmentArrivedIndex
              << " Unlocking AssignmentMutex "
              << EndLogLine;

            Platform::Mutex::Unlock( LocalServer.mWorkThreadAssignmentMutex );

            BegLogLine(1) << "WorkThread(): " << " Unlocked   LocalServer.mWorkThreadAssignmentMutex" << EndLogLine;

            // Get the index into the AbsloluteFragementInstance map.
            int TargetAssignedFragInstId =
                   LocalServer.mTargetFragmentArrived[ AssignedTargetFragmentArrivedIndex ];

            // Load packet pointer into the inteface class.
            IrreducibleFragmentIF TargetFragmentIF( LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ TargetAssignedFragInstId ] );

            BegLogLine(1)
              << "WorkThread(): "
              << " TargetOperandPacket Selected "
              << (void *) TargetFragmentIF.GetPacketAddress()
              << " FragmentId "
              << TargetAssignedFragInstId
              << " Assigned Target Fragment Arrived Index "
              << LocalServer.mTargetFragmentAssignedCount
              << EndLogLine;

            // Zero results packet.

            //NEED: think about the protocol... 0 index is reserved for COM Force
            TargetResultsPacket.mForce[ 0 ].Zero();
            for( int rs = 0;
                     rs <  TargetFragmentIF.GetIrreducibleFragmentMemberCount();
                     rs++ )
              {
              TargetResultsPacket.mForce[ rs + 1 ].Zero();
              }

            BegLogLine(1)
              << "WorkThread(): "
              << " Before Source Frag Loop "
              << " Source Frags expected: "
              << LocalServer.mSourceFragmentTotalCount
              << " Source Frags arrived: "
              << LocalServer.mSourceFragmentArrivedCount
              << EndLogLine;

            for( int SourceArrivedIndex = 0;
                     SourceArrivedIndex < LocalServer.mSourceFragmentTotalCount;
                     SourceArrivedIndex++ )
              {

              BegLogLine(1)
                << "WorkThread(): "
                << " Check/Wait on SourceArrivedIndex  "
                << SourceArrivedIndex
                << " Current SourceArrivedCount "
                << LocalServer.mSourceFragmentArrivedCount
                << EndLogLine;

              // Wait for a complete source fragment to appear to be ready to be worked on.
              while( SourceArrivedIndex >= LocalServer.mSourceFragmentArrivedCount )
                {
                Platform::Thread::Yield();
                Platform::Memory::ImportFence();
                }

   // Fence in the data indicated by LocalServer.mSourceFragmentArrivedCount
   Platform::Memory::ImportFence();

              // Get the index into the AbsloluteFragementInstance map.
              int SourceAssignedFragInstId =
                     LocalServer.mSourceFragmentArrived[ SourceArrivedIndex ];

              // Load packet pointer into the inteface class.
              IrreducibleFragmentIF
                 SourceFragmentIF( LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ SourceAssignedFragInstId ] );

              BegLogLine(1)
                << "WorkThread(): "
                << "START PROCESSING FRAG INDEXES (src/trg) "
                << SourceArrivedIndex
                << " / "
                << AssignedTargetFragmentArrivedIndex
                << " FragInstId (src/trg) "
                << SourceFragmentIF.GetIrreducibleFragmentId()
                << " / "
                << TargetFragmentIF.GetIrreducibleFragmentId()
                << " SourcePacket @ "
                << (void *) SourceFragmentIF.GetPacketAddress()
                << EndLogLine;

              if( SourceFragmentIF.GetIrreducibleFragmentId() == TargetFragmentIF.GetIrreducibleFragmentId() )
                TRUE;

              XYZ LocalVirialContribution;
              LocalVirialContribution.Zero();
              XYZ InterFragmentForce;


              IrreducibleFragmentPair( SourceFragmentIF.GetSimTick(),
                                       SourceFragmentIF,
                                       TargetFragmentIF,
                                       TargetResultsIF,
                                       wte,
                                       LocalVirialContribution,
                                       InterFragmentForce,
                                      &( LocalServer.mPersistantStatePointerArray[    AssignedTargetFragmentArrivedIndex
                                                                                     * LocalServer.mSourceFragmentTotalCount
                                                                                     + SourceArrivedIndex ] ) );
              GlobalVirial += 0.5 * LocalVirialContribution;                // virial is added in each direction so divide by 2

//////////////SourceFragmentIF.AddCenterOfMassForce( - InterFragmentForce );  // these signs could be backwards
//////////////TargetFragmentIF.AddCenterOfMassForce(   InterFragmentForce );

              TargetResultsIF.AddCenterOfMassForce( InterFragmentForce );

              BegLogLine(1)
                << "WorkThread(): "
                << "FINISH PROCESSING FRAG INDEXES (src/trg) "
                << SourceArrivedIndex
                << " / "
                << AssignedTargetFragmentArrivedIndex
                << " FragInstId (src/trg) "
                << SourceFragmentIF.GetIrreducibleFragmentId()
                << " / "
                << TargetFragmentIF.GetIrreducibleFragmentId()
                << EndLogLine;


              }

            BegLogLine(1) << "WorkThread(): " << " Done with all SourceArrived Loop, Sending results. " << EndLogLine;

            // Here we may want to drop the results off to a reduce infrastructure,
            // but for now, we'll just point-to-point send them home.
            // RESULTS PACKET IS THE SAME AS HELD IN ResultIF - SHOULD CLEAN THIS UP!!!!!!!!
            TargetResultsPacket.mHeader.mClientRef            = TargetFragmentIF.GetPacketAddress()->mHeader.mClientRef;
            TargetResultsPacket.mHeader.mIrreducibleFragmentId  =
                TargetFragmentIF.GetIrreducibleFragmentId();

            Platform::Reactor::Trigger( TargetFragmentIF.GetPacketAddress()->mHeader.mSourceTask,           // Back to where it come frm.
                                        ClientState::TargetResults_ActorFx,
                                      & TargetResultsPacket,
                                        sizeof(TargetResultsPacket) );


            // Release the target fragments data.
            if( ! TargetFragmentIF.GetPacketAddress()->mHeader.mSourceColFlag )  // if this was a target flag only
              {
              BegLogLine(1)
                << "WorkThread(): "
                << " Server @ "
                << (void *) &LocalServer
                << " Freeing Operand for TargetAssignedFragInstId "
                << TargetAssignedFragInstId
                << " Operand @ : "
                << (void *)  LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ TargetAssignedFragInstId ]
                << EndLogLine;

              Platform::Reactor::FreeBuffer(  LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ TargetAssignedFragInstId ] );

              LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ TargetAssignedFragInstId ] = NULL;
              }


            BegLogLine(1)
              << "WorkThread(): "
              << " Server @ "
              << (void *) &LocalServer
              << " Checking for IF this was last Target "
              << " AssiedTargetFragmentArrivedIndex "
              << AssignedTargetFragmentArrivedIndex
              << " Expected "
              << LocalServer.mTargetFragmentTotalCount
              << EndLogLine;

            // If we picked up the last one, and have completed the work, release all SourceFragmnt packets
            if( AssignedTargetFragmentArrivedIndex + 1 == LocalServer.mTargetFragmentTotalCount )
              {
              BegLogLine(1)
                << "WorkThread(): "
                << " Server @ "
                << (void *) &LocalServer
                << " LAST Target fragment "
                << " Freeing source frags "
                << EndLogLine;

              // First, send in the energy!
              BegLogLine(1)
                << " TotLJ "    << wte.mTotalLennardJonesEnergy
                << " TotCh "    << wte.mTotalChargeEnergy
                << EndLogLine;

              // These two avoid compiler warnings - there must be a way to DCL an unsigned literal.
              int zero = 0;
              unsigned one = 1;

              unsigned OTS = TargetFragmentIF.GetPacketAddress()->mHeader.mSimTick;
//////////////ED_Emit_EnergyNonBonded( OTS,
//////////////                         zero,
//////////////                         one,
//////////////                         wte.mTotalChargeEnergy,
//////////////                         wte.mTotalLennardJonesEnergy,
//////////////                         0.0 );
//////////////
              ED_Emit_UDFd1( OTS, UDF_Binding::LennardJonesForce_Code, one, wte.mTotalLennardJonesEnergy );
              ED_Emit_UDFd1( OTS, UDF_Binding::StdChargeForce_Code,    one, wte.mTotalChargeEnergy       );

              wte.Zero();

              if( MSD_IF.Access().DoFreeEnergy )
                {
                double TotalDHDLamda = wte.mTotalLennardJonesDHDLamda + wte.mTotalChargeDHDLamda;
                ED_Emit_EnergyFree( OTS, TotalDHDLamda );
                }

              if( MSD_IF::DoPressureControl )
                {
                XYZ TotalVirial = wte.mTotalLennardJonesVirial + wte.mTotalChargeVirial;
// UNCLEAR HOW THIS IS GOING TO WORK BASED ON WHAT IS IN PLIMPTON LOOPS
/////////////////                ED_Emit_ControlPressure( OTS, TotalVirial );
                }

//NEED: MUST MUST MUST MAKE SURE THAT ALL OTHER THREADS ARE DONE (NOT JUST STARTED) WITH SOURCE PACKETS.

              // Scan through and free source buffers.
              for( int SourceArrivedIndex = 0;
                       SourceArrivedIndex < LocalServer.mSourceFragmentTotalCount;
                       SourceArrivedIndex++ )
                {
                int SourceFragInstId = LocalServer.mSourceFragmentArrived[ SourceArrivedIndex ];

                BegLogLine(1)
                  << "WorkThread(): "
                  << " Server @ "
                  << (void *) &LocalServer
                  << " Freeing Operand for SourceFragInstId "
                  << SourceFragInstId
                  << " Operand @ : "
                  << (void *)  LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ TargetAssignedFragInstId ]
                  << EndLogLine;

                Platform::Reactor::FreeBuffer(  LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ SourceFragInstId ] );
                LocalServer.mAbsoluteIrreducibleFragmentId_To_LocalServerMap[ SourceFragInstId ] = NULL;
                }

              LocalServer.mSourceFragmentArrivedCount = 0;
              LocalServer.mTargetFragmentArrivedCount = 0;
              LocalServer.mTargetFragmentAssignedCount = 0;

              LocalServer.mSimTick++;

              BegLogLine(1)
                << "WorkThread(): "
                << " Server @ "
                << (void *) &LocalServer
                << " Incrementing SimTick to: "
                <<  LocalServer.mSimTick
                << EndLogLine;

              }
            }
          }

      }; // End of class Server

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
                 unsigned               aSimTicksPerOuterTimeStep, // assumes Plimptonize only happens at OTS
                 int                    aIntegratorLevel )
          {

          ConnectActorPacket ap;

          mClientState.mGrid2D      = aGrid2D;
          mClientState.mDynVarMgrIF                = aDynVarMgrIF;
          mClientState.mSimTick                    = aSimTick;
          mClientState.mTargetResultsDoneFlag            = 0;
          mClientState.mSimTicksPerOuterTimeStep   = aSimTicksPerOuterTimeStep;
          mClientState.mIntegratorLevel            = aIntegratorLevel ;

          ap.mServerRef = mClientState.mServerRef;

          ap.mSimTickToStart = aSimTick;

          // These two lines seem to say the same thing but actually fix the map.
          ap.mSourceTask  = Platform::Topology::GetAddressSpaceId();
          ap.mGrid2D      = aGrid2D;
          ap.mCoordinate  = aGrid2D.GetMyCoordinate();

          ap.mClientIrreducibleFragmentCount =  aDynVarMgrIF->GetIrreducibleFragmentCount();

          ap.mSourceOrTargetFlag = TargetRowFlag;  //NEED: this should be an enum... need to map to row v col

          ActorMulticast( aGrid2D.GetRowGroupIF( aGrid2D.MyXCoordinate() ),
                          Server::Connect_ActorFx,
                          & ap,
                          sizeof(ap) );

          ap.mSourceOrTargetFlag = SourceColFlag;

          ActorMulticast( aGrid2D.GetColGroupIF( aGrid2D.MyYCoordinate() ),
                          Server::Connect_ActorFx,
                          & ap,
                          sizeof(ap) );

          // NEED: to think about whether we have to barrier here.  Don't think so, but it could change.

          }

        void
        Send( int aSimTick )
          {
          BegLogLine(1)
            << "Send(): "
            << " SimTick "
            << aSimTick
            << EndLogLine;

          mClientState.mTargetResultsArrivedCount      = 0;
          mClientState.mTargetResultsExpected = mClientState.mGrid2D.GetYDimension() * mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount();
          mClientState.mTargetResultsDoneFlag          = 0;

          for( int f = 0; f < mClientState.mDynVarMgrIF->GetIrreducibleFragmentCount(); f++ )
            {
            int PacketNumber       = 0;
            int SinglePacketFlag   = 1;
            int PacketPositionBase = 0;

            // This is the id that would be correct when using MSD_IF interfaces
            int AbsoluteIrreducibleFragmentId = mClientState.mDynVarMgrIF->GetNthIrreducibleFragmentId( f );

            PlimptonNonBondedEngine::OperandActorPacket ap;
            ap.mHeader.mServerRef                     = mClientState.mServerRef;
            ap.mHeader.mClientRef                     = & mClientState;
            ap.mHeader.mIrreducibleFragmentId = AbsoluteIrreducibleFragmentId;
            ap.mHeader.mFragmentPacketNumber          = PacketNumber;
            ap.mHeader.mSinglePacketFragment          = SinglePacketFlag;
            ap.mHeader.mSourceTask                    = 0;
            ap.mHeader.mSimTick                       = aSimTick;


            //For now, assert that the entire fragment fits into the packet.
            assert(
                    MSD_IF::GetIrreducibleFragmentMemberSiteCount( AbsoluteIrreducibleFragmentId )
                    <
                    OperandActorPacket::NumberOfPositions );

            int FragmentSiteIndexBase = mClientState.mDynVarMgrIF->GetNthIrreducibleFragmentFirstSiteIndex( f );
            int FragmentSiteCount     = mClientState.mDynVarMgrIF->GetNthIrreducibleFragmentSiteCount( f );

            for( int s =  0;
                     s <  FragmentSiteCount;
                     s++ )
              {
              ap.mPositions[ s ] = mClientState.mDynVarMgrIF->GetPosition( FragmentSiteIndexBase + s );
              }

            ap.mHeader.mSourceColFlag            = 1;
            ap.mHeader.mTargetRowFlag            = 0;

            BegLogLine(1)
              << "Send(): "
              << " SimTick "
              << aSimTick
              << " STARTING Multicast Frag Id "
              << ap.mHeader.mIrreducibleFragmentId
              << " NumberOfSites "
              << mClientState.mDynVarMgrIF->GetNthIrreducibleFragmentSiteCount( f )
              << EndLogLine;

            ActorMulticast( mClientState.mGrid2D.GetRowGroupIF( mClientState.mGrid2D.MyXCoordinate() ),
                            Server::Operand_ActorFx,
                            & ap,
                            sizeof(ap) );

            ap.mHeader.mSourceColFlag            = 0;
            ap.mHeader.mTargetRowFlag            = 1;

            ActorMulticast( mClientState.mGrid2D.GetColGroupIF( mClientState.mGrid2D.MyYCoordinate() ),
                            Server::Operand_ActorFx,
                            & ap,
                            sizeof(ap) );

            BegLogLine(1)
              << "Send(): "
              << " SimTick "
              << aSimTick
              << " FINISHED Multicast Frag Id "
              << ap.mHeader.mIrreducibleFragmentId
              << " NumberOfSites "
              << mClientState.mDynVarMgrIF->GetNthIrreducibleFragmentSiteCount( f )
              << EndLogLine;

            }
          }

        void
        Wait( int aSimTick )
          {
          // Here we want to wait for the appropriate number of resutlts to have been returned.
          while( ! mClientState.mTargetResultsDoneFlag )
            {
            BegLogLine(1)
              << "Wait(): "
              << EndLogLine;
            Platform::Thread::Yield();
            sleep(1);
            }
   // Fence in the data indicated by LocalServer.mSourceFragmentArrivedCount
   Platform::Memory::ImportFence();
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

    class PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Server mServer;

    /*
     *
     */
  public:

    void
    Init()
      {
      //NEED: THIS SHOULD BE SOMEWHERE ELSE

      PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Server * ServerPtr = & mServer;

      BegLogLine(1)
        << "PlimptonNonBondedEngine<>:: "
        << " sizeof( connect pkt hdr ) "
        << sizeof( PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::ConnectActorPacket)
        << " sizeof( operand pkt hdr ) "
        << sizeof( PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::OperandActorPacket::Header )
        << " sizeof( result  pkt hdr) "
        << sizeof( PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::TargetResultsActorPacket::Header )
        << EndLogLine;

      BegLogLine(1)
        << "PlimptonAnchor::Init(): "
        << " ServerPtr "
        << (void *) ServerPtr
        << EndLogLine;

      //NEED: thread create to learn about ActorMulticast().
      for( int p = 0; p < Platform::Topology::GetAddressSpaceCount(); p++ )
        {
        ThreadCreate( p,
                      PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Server::WorkThread,
                      sizeof( ServerPtr ),
                      & ServerPtr );
        }
      }


    // Create a compiler recongnized instance of the Client Interface - should be zero byts.
    typedef class PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Client ClientIF;

    ClientIF &
    GetClientIF()
      {
      PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Client * cr
        = new  PlimptonNonBondedEngine<DynVarMgrIF,NBMV>::Client();

      cr->mClientState.mServerRef = & mServer;

      return( * cr  );
      }




  };

#endif


