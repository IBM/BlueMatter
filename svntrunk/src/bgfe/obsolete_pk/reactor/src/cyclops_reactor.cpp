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
 * Project:         pK
 *
 * Module:          mpi_reactor.cpp
 *
 * Purpose:         active packet driver based on mpi
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         020201 BGF Createed from reactor.cpp ... 010297 BGF Created.
 *
 ***************************************************************************/

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#include "buffer.cpp"

#ifndef PKTRACE_REACTOR
#define PKTRACE_REACTOR (0)
#endif


#ifndef PKFXLOG_REACTOR
#define PKFXLOG_REACTOR (0)
#endif

MPI_Comm REACTIVE_MESSAGE_COMM_WORLD;

BufferPtr      *AllBuf     ;    // A pointer to an array of pointers.
MPI_Request    *AllRequests;
MPI_Status     *AllStatus  ;


#ifndef PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT
#define PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT (64)  // 64 * (say)32KB == 2MB commited.
#endif

#define TOTAL_DATAGRAMS_IN_FLIGHT \
        (PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT * Platform::Topology::GetAddressSpaceCount())

//-----------------------------------------------------------------------------
// This constructor is called during the init of the
// pk. It can be assumed to be running in all tasks.

void
ReactorInitialize()
  {
  BegLogLine( PKFXLOG_REACTOR )
    << "ReactorInitialize() in Task ["
    << Platform::Topology::GetAddressSpaceId() << "]"
    << EndLogLine;

  for( int bpi = 0; bpi < FREE_BUFFER_POOL_SIZE; bpi++ )   //Initialise buffer pool
    FreeBufferPool[bpi] = NULL;
  FreeBufferPoolCount    = 0;
  FreeBufferPoolLockWord = 0;

  AllBuf = new BufferPtr[TOTAL_DATAGRAMS_IN_FLIGHT ];  //Initialise an array of buffers
  assert( AllBuf != NULL );                             // based on the maximum number of
                                                        // packets being sent at once.
  for( int i = 0; i < (TOTAL_DATAGRAMS_IN_FLIGHT); i++ )
    {
    AllBuf[i] = (BufferPtr) Platform::Reactor::GetBuffer();
    }
  AllRequests = new MPI_Request [ TOTAL_DATAGRAMS_IN_FLIGHT ];  //
  AllStatus   = new MPI_Status  [ TOTAL_DATAGRAMS_IN_FLIGHT ];  //

  MPI_Group REACTIVE_MESSAGE_GROUP;

  MPI_Comm_group( MPI_COMM_WORLD, &REACTIVE_MESSAGE_GROUP); //Make REACTIVE_MESSAGE_GROUP
                                                                   //a group with all processes.

  MPI_Comm_create( MPI_COMM_WORLD,
                          REACTIVE_MESSAGE_GROUP,        //Create a new communicator called
                         &REACTIVE_MESSAGE_COMM_WORLD);  // REACTIVE_MESSAGE_COMM_WORLD that includes
                                                         // all processes in REACTIVE_MESSAGE_GROUP.
  int t;
  for( t = 0; t<TOTAL_DATAGRAMS_IN_FLIGHT;t++)
    {
    int msgrc;

    msgrc = MPI_Irecv( (void *) AllBuf[t],             //Post non-blocking receive for
                              sizeof( Buffer ),               // each request.
                              MPI_CHAR,
                              MPI_ANY_SOURCE,
                              MPI_ANY_TAG,
                              REACTIVE_MESSAGE_COMM_WORLD,
                             &AllRequests[t]);
    }

  for( ; t < TOTAL_DATAGRAMS_IN_FLIGHT; t++ )                 //Initialise requests to NULL
    {
    AllRequests[t] = MPI_REQUEST_NULL;
    }

  //NEED: might want to ponder this here even in MPI world.
  MPI_Barrier( REACTIVE_MESSAGE_COMM_WORLD );         //Wait for all processes to get here.


///////////  PhysicalThreadManager::GetInterface()->Start( this, NULL ); //Kick off PhysicalThreadManager
  }

//-----------------------------------------------------------------------------

int
Platform::Reactor::
Trigger( int             TaskNo,
         Platform::Reactor::FunctionPointerType    FxPtr,
         void           *Msg,
         unsigned        MsgLen)
  {
  assert( MsgLen <= Platform::Reactor::PAYLOAD_SIZE );

  int rc;

  assert( MsgLen <= sizeof( Buffer ) );

  BegLogLine( PKFXLOG_REACTOR )
        <<  "Trigger() Before Send ActiveMsg To Task "
        <<  TaskNo
        <<  " Reactor @"
        <<  (unsigned) FxPtr
        <<  " TMsg Len "
        <<  MsgLen
        <<  " Msg data: "
        <<  (void *) (*((int*)Msg))
        <<  EndLogLine;

//PkActiveMsgSubSys::GetIFPtr()->TraceTrigger.HitOE( PKTRACE_REACTOR,
//                                                   ACT,
//                                                   PkActiveMsgSubSys::GetIFPtr()->TraceName,
//                                                   PkActiveMsgSubSys::GetIFPtr()->TracePPid,
//                                                   TraceTrigger);
//

 rc = svcSendTaggedMessage(&TaskNo,                 //MESSAGE_ROUTE *route,
                           FxPtr                    //MESSAGE_PREFIX *prefix,
                           MESSAGE_DATA *tag,
                           MESSAGE_DATA values[],
                           unsigned cnt)



  mpirc = MPI_Send(Msg,
                   MsgLen,
                   MPI_CHAR,                              // Send data and function pointer to
                   TaskNo,                                // specified task.
                   (int) FxPtr,
                   REACTIVE_MESSAGE_COMM_WORLD );

  assert( mpirc == 0 );

  BegLogLine( PKFXLOG_REACTOR )
        <<  "Trigger() After Send ActiveMsg To Task "
        <<  Platform::Topology::GetAddressSpaceId()
        <<  " Reactor @"
        <<  (unsigned) FxPtr
        <<  " TMsg Len "
        <<  MsgLen
        <<  " Msg data: "
        <<  (void *) (*((int*)Msg))
        << EndLogLine;

  if( mpirc != 0 )
    {
    MPI_Abort( REACTIVE_MESSAGE_COMM_WORLD,
                      mpirc );
    }

  return( mpirc );
  }

extern "C" { void trcon(int) ; void trcoff(int) ; } ;

//-----------------------------------------------------------------------------
void*
ReactorThreadMain( void *arg )     //This goes round and round and round...........
  {
  int mpirc;
  int ReactorRc;


  BegLogLine( PKFXLOG_REACTOR )
        << " PhysicalThread ... Running ..."
      << "ActiveMsg Processor: "
        << EndLogLine;


  // Continuous loop
  for(int d = 0; (1) ; d++)
    {
    unsigned msgrc;
    MPI_Status Status;                     //Always do this
    int ReqIndex;

#if 1  // Do not trust MPIs wait mechanism to be consisant with PK

    BegLogLine( PKFXLOG_REACTOR )
      << "ActiveMsg Processor: "
      << " Testany()/usleep() for incomming "
      << EndLogLine;
    int Flag = 0;
//NEED to encapsulate this function as a yield function.
    for( int PollCount = 0; Flag == 0; PollCount++ )   //Loop until a request is complete
      {
      mpirc = MPI_Testany(TOTAL_DATAGRAMS_IN_FLIGHT, AllRequests, &ReqIndex, &Flag, &Status);
      assert( mpirc == 0);
      if( ! Flag )
        {
        if( PollCount < (64 * 1024) )  // the reactor should be the last to sleep
          Platform::Thread::Yield();
        else
          usleep( 10000 );             // and sleep less long.
        }
      }

#else // Trust mpi to deschedule reactor during wait.

    BegLogLine( PKFXLOG_REACTOR )
      << "ActiveMsg Processor: "
      << " Waiting for incomming "
      << EndLogLine;
    mpirc = MPI_Waitany( TOTAL_DATAGRAMS_IN_FLIGHT, AllRequests,
                         &ReqIndex, &Status );
    assert( mpirc == 0 );

#endif

    int Len;

    MPI_Get_count( &Status, MPI_CHAR, &Len );//Use Status retuurned above to get message length.

    BegLogLine( PKFXLOG_REACTOR )
           << "ActiveMsg Processor: Got one! "
           << " Before call Fx @"
           <<  MPI_STATUS_TAG( Status )
           << " Src "
           <<  MPI_STATUS_SOURCE( Status )
           <<  " Len "
           <<  Len
           <<  " Data "
           <<  AllBuf[ReqIndex]
           << EndLogLine;

    //////TraceExecute.HitOE( PKTRACE_REACTOR,ACT,TraceName,TracePPid,TraceExecute);

#if 0 // Code to do stateful active messages.  Not really used, so out for now.
    if( (unsigned)(MPI_STATUS_TAG( Status )) >= (unsigned) &(ReactorTable[0])  //This checks to see if this
        &&                                                     //is a pointer to a ReactorTable
        (unsigned)(MPI_STATUS_TAG( Status )) < (unsigned) ReactorTableNext)    //entry of a function pointer
      {
      ReactorTableEntry *RTEP = (ReactorTableEntry *) (MPI_STATUS_TAG( Status ));
      ReactorRc = ((PkReactorFxPtr)(RTEP->RFxPtr))
                              ( RTEP->State,                   //If so call function as defined
                                AllBuf[ReqIndex] );
      }
    else
#endif
      {
      // THIS IS THE POINT WHERE ACTIVE PACKET FUNCTIONS ARE EXECUTED
      ReactorRc = ((Platform::Reactor::FunctionPointerType)((MPI_STATUS_TAG( Status ))))( AllBuf[ReqIndex] );
      }

    switch( ReactorRc )
      {
      case 0 :
        break;
      case 1 :
        AllBuf[ReqIndex] = Platform::Reactor::GetBuffer();       //buffer so we make a new one.
        break;
      default:
        assert( 0 );
      };

    BegLogLine( PKFXLOG_REACTOR )
      << "ActiveMsg Processor: Did one! "
      << "After call Fx() @"
      <<  (MPI_STATUS_TAG( Status ))
      << " Src "
      <<  MPI_STATUS_SOURCE( Status )
      <<  " Len "
      <<  Len
      <<  " Data  "
      <<  AllBuf[ReqIndex]
      << EndLogLine;


    msgrc = MPI_Irecv( (void *) AllBuf[ReqIndex],      //Post a non-blocking receive on the
                              sizeof( Buffer ),               //buffer we just used.
                              MPI_CHAR,
                              MPI_ANY_SOURCE,
                              MPI_ANY_TAG,
                              REACTIVE_MESSAGE_COMM_WORLD,
                             &AllRequests[ReqIndex] );

    assert( msgrc == 0 );

    if( msgrc != 0 )
      {
      //Pprintf("Reactive Message Subsystem: MPI_Irecv failed \n");
      MPI_Abort( MPI_COMM_WORLD, -1);
      }
    }

  MPI_Finalize();
  _exit(0);
  return( NULL ); // Keep compiler quiet
  }


