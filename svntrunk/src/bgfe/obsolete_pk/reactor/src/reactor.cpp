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
 * Module:          pkreactor.cpp
 *
 * Purpose:         Methods of .hpp file.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/
#include <pk/reactor.hpp>
#include <pk/mpi.hpp>

#ifndef PKTRACE_REACTOR
#define PKTRACE_REACTOR (0)
#endif

static PkActiveMsgSubSys *SingletonPtr = NULL;

// declaired static in header.
PkActiveMsgSubSys*
PkActiveMsgSubSys::GetIFPtr() //Returns a poointer to the PkActiveMsgSubSys IF.
  {                           //If it hasn't been created yet it calls the constructor.
  static int LockInt = 0;

  if( SingletonPtr == NULL )
    {
    YieldLock( LockInt );

    if( SingletonPtr == NULL )
      {
      BegLogLine( PKFXLOG_PKREACTOR )
        << " Constructing new PkActiveMsgSubSys"
        << EndLogLine;

      SingletonPtr = new PkActiveMsgSubSys();
      assert( SingletonPtr != NULL );

      // Set up for trace... maybe these couple of lines should live elsewhere.
      SingletonPtr->TracePPid = getppid();
      sprintf( SingletonPtr->TraceName, "PkReactor:%03d", MpiSubSys::GetTaskNo() );
      }

    UnYieldLock( LockInt );
    }

  return( SingletonPtr );
  }

#ifndef MPI //#ifndef MPI

PkActiveMsgSubSys::
PkActiveMsgSubSys()
  {
  ReactorTableNext = ReactorTable;     //Point index to start of table.
  BegLogLine( PKFXLOG_PKREACTOR )
    << "PkActiveMsgSubSys: constructed"
    << EndLogLine;
  for( int bpi = 0; bpi < FREE_BUFFER_POOL_SIZE; bpi++ )
    FreeBufferPool[bpi] = NULL;
  FreeBufferPoolCount    = 0;                   //Initialise the buffer pool
  FreeBufferPoolLockWord = 0;

  }
//-----------------------------------------------------------------------------
int
PkActiveMsgSubSys::
Init()               //Start the PkActiveMsgSubSys but dont return a pointer to it.
  {
  PkActiveMsgSubSys* TempPtr = PkActiveMsgSubSys::GetIFPtr();
  assert( TempPtr != NULL);
  return 0;
  }
//-----------------------------------------------------------------------------
int
PkActiveMsgSubSys::
IsInit()               //Start the PkActiveMsgSubSys but dont return a pointer to it.
  {
  if( PkActiveMsgSubSys::GetIFPtr() )
    return( 1 );
  else
    return 0;
  }
//-----------------------------------------------------------------------------
int
PkActiveMsgSubSys::
Trigger( int           TaskNo,  //Target task for triggered function.
         PkActorFxPtr  FxPtr,   //Function that is to be triggered.
         void         *Msg,     //Data to be sent.
         unsigned      MsgLen)  //Size of data to be sent.
  {

  PkActiveMsgSubSys::GetIFPtr()->TraceTrigger.HitOE( PKTRACE_REACTOR,
                                                     PkActiveMsgSubSys::GetIFPtr()->TraceName,
                                                     PkActiveMsgSubSys::GetIFPtr()->TracePPid,
                                                     TraceTrigger);
  assert( MsgLen <= PKREACTOR_DATAGRAM_SIZE);  //Data must fit in the datagram.
  BufferPtr Buf = PkActiveMsgSubSys::GetIFPtr()->GetBuffer();  //Get a bufferfro the pool.
  memcpy( Buf, Msg, MsgLen );                                  //Put the data in the buffer.

#if BLUEGENE
  int ReactorRc = (FxPtr)( Buf );      //Call the function with buffered data.
#else
  int ReactorRc = (FxPtr)( 0, MsgLen, Buf );      //Call the function with buffered data.
#endif

  switch( ReactorRc )
    {
    case 0 :
      // Actor is finished with buffer - too bad we didn't know ... now we have to delete.

      PkActiveMsgSubSys::GetIFPtr()->FreeBuffer( Buf );     //Free up buffer if the triggered function
      break;                                                // is completed.
    case 1 :
      // Actor has kept the buffer.                         //If not don't.
      break;
    default:
      assert( 0 );
    };

  return(0);
  }
//-----------------------------------------------------------------------------
int
PkActiveMsgSubSys::
Trigger( ReactorHandle Handle,      //This uses a ReactorHandle to get an RFxPtr
         void         *Msg,         // and State from the ReactorTable. It then
         unsigned      MsgLen)      // triggers the reactor function with the
  {                                 // State & data in a similar manner to above.

  assert( MsgLen <= PKREACTOR_DATAGRAM_SIZE);
  assert( Handle.Task == 0 );
#if 0
  PkReactorFxPtr RFxPtr = PkActiveMsgSubSys::GetIFPtr()->ReactorTable[ Handle.Index ].RFxPtr;
  char           *State = PkActiveMsgSubSys::GetIFPtr()->ReactorTable[ Handle.Index ].State;
#endif
  PkReactorFxPtr RFxPtr = Handle.ReactorTableEntryPtr->RFxPtr;
  char           *State = Handle.ReactorTableEntryPtr->State;
  assert( RFxPtr != NULL );
  BufferPtr Buf = PkActiveMsgSubSys::GetIFPtr()->GetBuffer();
  memcpy( Buf, Msg, MsgLen );

#if BLUEGENE
  int ReactorRc = (RFxPtr)( State, Buf );
#else
  int ReactorRc = (RFxPtr)( State, 0, MsgLen, Buf );
#endif
  switch( ReactorRc )
    {
    case 0 :
      // Actor is finished with buffer - too bad we didn't know ... now we have to delete.
      PkActiveMsgSubSys::GetIFPtr()->FreeBuffer( Buf );
      break;
    case 1 :
      // Actor has kept the buffer.
      break;
    default:
      assert( 0 );
    };

  return(0);
  }
//-----------------------------------------------------------------------------
ReactorHandle
PkActiveMsgSubSys::
RegisterReactor( char           *State,    //Enrole a new Reactor in the reactor table.
                 PkReactorFxPtr  RFxPtr )
  {
  ReactorHandle Handle;
  // WARNING: RACE COND FOR MULTITHREADED ACCESS :: ReactorTable needs protecting
  Handle.ReactorTableEntryPtr =
       PkActiveMsgSubSys::GetIFPtr()->ReactorTableNext++;
  Handle.Task  = 0;
  Handle.ReactorTableEntryPtr->RFxPtr = RFxPtr;
  Handle.ReactorTableEntryPtr->State  = State;
  return( Handle );
  }
//-----------------------------------------------------------------------------
void* PkActiveMsgSubSys::PhysicalThreadMain( void *arg )
  {
  assert(0);                    //Don't want to be here if not using mpi
  return(NULL);
  }
//-----------------------------------------------------------------------------

//#############################################################################
#else  // MPI turned on

#ifndef PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT
#define PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT (64)  // 64 * (say)32KB == 2MB commited.
#endif

#define TOTAL_DATAGRAMS_IN_FLIGHT (PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT * MpiSubSys::TaskCnt)

//-----------------------------------------------------------------------------
// This constructor is called during the init of the
// pk. It can be assumed to be running in all tasks.

PkActiveMsgSubSys::
PkActiveMsgSubSys()
  {

  BegLogLine( PKFXLOG_PKREACTOR )
    << "PkActiveMsgSubSys: constructed"
    << EndLogLine;

  BegLogLine( PKFXLOG_PKREACTOR )
    << "PkActiveMsgSubSys::PkActiveMsgSubSys( ) in Task ["
    << MpiSubSys::TaskNo << "]"
    << EndLogLine;

  for( int bpi = 0; bpi < FREE_BUFFER_POOL_SIZE; bpi++ )   //Initialise buffer pool
    FreeBufferPool[bpi] = NULL;
  FreeBufferPoolCount    = 0;
  FreeBufferPoolLockWord = 0;

  ReactorTableNext = ReactorTable;              //Point to start of reactor table.

  AllBuf = new BufferPtr[TOTAL_DATAGRAMS_IN_FLIGHT ];  //Initialise an array of buffers
  assert( AllBuf != NULL );                             // based on the maximum number of
                                                        // packets being sent at once.
  for( int i = 0; i < (TOTAL_DATAGRAMS_IN_FLIGHT); i++ )
    {
    ///////AllBuf[i] = new Buffer;
    AllBuf[i] = GetBuffer();
    }
  AllRequests = new MPI_Request [ TOTAL_DATAGRAMS_IN_FLIGHT ];  //
  AllStatus   = new MPI_Status  [ TOTAL_DATAGRAMS_IN_FLIGHT ];  //

  MPI_Group REACTIVE_MESSAGE_GROUP;

  MpiSubSys::Comm_group( MPI_COMM_WORLD, &REACTIVE_MESSAGE_GROUP); //Make REACTIVE_MESSAGE_GROUP
                                                                   //a group with all processes.

  MpiSubSys::Comm_create( MPI_COMM_WORLD,
                          REACTIVE_MESSAGE_GROUP,        //Create a new communicator called
                         &REACTIVE_MESSAGE_COMM_WORLD);  // REACTIVE_MESSAGE_COMM_WORLD that includes
                                                         // all processes in REACTIVE_MESSAGE_GROUP.
  int t;
  for( t = 0; t<TOTAL_DATAGRAMS_IN_FLIGHT;t++)
    {
    int msgrc;

    msgrc = MpiSubSys::Irecv( (void *) AllBuf[t],             //Post non-blocking receive for
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


  MpiSubSys::Barrier( REACTIVE_MESSAGE_COMM_WORLD );         //Wait for all processes to get here.

  // this line make thes (not) called function debugable
  if( (rand() + 0x000fffff) == 0 )
    PhysicalThreadMain( NULL );

  PhysicalThreadManager::GetInterface()->Start( this, NULL ); //Kick off PhysicalThreadManager
  }

//-----------------------------------------------------------------------------
int
PkActiveMsgSubSys::
Init()
  {
  PkActiveMsgSubSys* TempPtr = PkActiveMsgSubSys::GetIFPtr(); //Start PkActiveMsgSubSys but don't
  assert( TempPtr != NULL);                                   // return IF pointer.
  return(0);
  }

//-----------------------------------------------------------------------------
int
PkActiveMsgSubSys::
IsInit()               //Start the PkActiveMsgSubSys but dont return a pointer to it.
  {
  //NEED to issue a sync perhaps.
  if( PkActiveMsgSubSys::GetIFPtr() )
    return( 1 );
  else
    return 0;
  }
//-----------------------------------------------------------------------------
ReactorHandle
PkActiveMsgSubSys::                        //Enrole new reactor in ReactorTable
RegisterReactor( char           *State,
                 PkReactorFxPtr  RFxPtr )
  {
  ReactorHandle Handle;
  // WARNING: RACE COND FOR MULTITHREADED ACCESS :: ReactorTable needs protecting
  Handle.ReactorTableEntryPtr =
       PkActiveMsgSubSys::GetIFPtr()->ReactorTableNext++;
  Handle.Task  = 0;
  Handle.ReactorTableEntryPtr->RFxPtr = RFxPtr;
  Handle.ReactorTableEntryPtr->State  = State;
  return( Handle );
  }
//-----------------------------------------------------------------------------
int
PkActiveMsgSubSys::
Trigger( ReactorHandle Handle,
         void         *Msg,           //extract Task & PkActorFxPtr from table
         unsigned      MsgLen)        //and call Trigger with these.
  {
  // Wait for active message sub system to come up before allowing trigger.
  //NEED this could be a problem a static thread attempts a trigger from
  // the main thread and blocks here before the actor system is brought up.
  while( ! PkActiveMsgSubSys::IsInit() )
    {
    //NEED: to check here that this is not the thread destined to become the
    //      active message processor, namely main in posix.
    usleep(100);
    }

  assert( MsgLen < PKREACTOR_DATAGRAM_SIZE);
  assert( Handle.Task == 0 );
  int ReactorRc = Trigger( Handle.Task,
                           (PkActorFxPtr) Handle.ReactorTableEntryPtr,
                           Msg,
                           MsgLen);
  assert( ReactorRc == 0 );
  return(0);
  }
//-----------------------------------------------------------------------------

int
PkActiveMsgSubSys::
Trigger( int             TaskNo,
         PkActorFxPtr    FxPtr,
         void           *Msg,
         unsigned        MsgLen)
  {
  assert( MsgLen <= PKREACTOR_DATAGRAM_SIZE);

  PkActiveMsgSubSys* IFPtr = PkActiveMsgSubSys::GetIFPtr();   //Get pointer to PkActiveMsgSubSys IF
  assert (IFPtr != NULL);

  int mpirc;

  assert( MsgLen <= sizeof( Buffer ) );

  BegLogLine( PKFXLOG_PKREACTOR )
        <<  "Trigger() Before Send ActiveMsg To Task "
        <<  TaskNo
        <<  " Reactor @"
        <<  (unsigned) FxPtr
        <<  " TMsg Len "
        <<  MsgLen
        <<  " Msg data: "
        <<  (void *) (*((int*)Msg))
        <<  EndLogLine;

  PkActiveMsgSubSys::GetIFPtr()->TraceTrigger.HitOE( PKTRACE_REACTOR,
                                                     PkActiveMsgSubSys::GetIFPtr()->TraceName,
                                                     PkActiveMsgSubSys::GetIFPtr()->TracePPid,
                                                     TraceTrigger);

  mpirc = MpiSubSys::Send(Msg,
                   MsgLen,
                   MPI_CHAR,                              // Send data and function pointer to
                   TaskNo,                                // specified task.
                   (int) FxPtr,
                   IFPtr->REACTIVE_MESSAGE_COMM_WORLD );

  assert( mpirc == 0 );

  BegLogLine( PKFXLOG_PKREACTOR )
        <<  "Trigger() After Send ActiveMsg To Task "
        <<  TaskNo
        <<  " Reactor @"
        <<  (unsigned) FxPtr
        <<  " TMsg Len "
        <<  MsgLen
        <<  " Msg data: "
        <<  (void *) (*((int*)Msg))
        << EndLogLine;

  if( mpirc != 0 )
    {
    MpiSubSys::Abort( IFPtr->REACTIVE_MESSAGE_COMM_WORLD,
                      mpirc );
    }

  return( mpirc );
  }

extern "C" { void trcon(int) ; void trcoff(int) ; } ;
//-----------------------------------------------------------------------------
void*
PkActiveMsgSubSys::
PhysicalThreadMain( void *arg )     //This goes round and round and round...........
  {
  int mpirc;
  int ReactorRc;

  BegLogLine( PKFXLOG_PKREACTOR )
        << " PhysicalThread ... Running ..."
      << "ActiveMsg Processor: "
        << EndLogLine;

  // Continuous loop
  for(int d = 0;;d++)
  {


// NOTE: with this macro set, we will be doing a polling Testany loop!!!!!

#if 1 //def MPISUBSYS_TEST_WAIT

    unsigned msgrc;
    MPI_Status Status;                     //Always do this
    int ReqIndex;

#if 0
    BegLogLine( PKFXLOG_PKREACTOR )
      << "ActiveMsg Processor: "
      << " Waiting for incomming "
      << EndLogLine;
    mpirc = MpiSubSys::Waitany( TOTAL_DATAGRAMS_IN_FLIGHT, AllRequests,
                         &ReqIndex, &Status );
    assert( mpirc == 0 );
#else

    BegLogLine( PKFXLOG_PKREACTOR )
      << "ActiveMsg Processor: "
      << " Testany()/usleep() for incomming "
      << EndLogLine;
    int Flag = 0;
    for( int PollCount = 0; Flag == 0; PollCount++ )   //Loop until a request is complete
      {
      mpirc = MpiSubSys::Testany(TOTAL_DATAGRAMS_IN_FLIGHT, AllRequests, &ReqIndex, &Flag, &Status);
      assert( mpirc == 0);
      if( ! Flag )
        {
        if( PollCount < (64 * 1024) )  // the reactor should be the last to sleep
          pthread_yield();
        else
          usleep( 10000 );             // and sleep less long.
        }
      }
#endif

    int Len;

    MpiSubSys::Get_count( &Status, MPI_CHAR, &Len );//Use Status retuurned above to get message length.

    BegLogLine( PKFXLOG_PKREACTOR )
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

    TraceExecute.HitOE( PKTRACE_REACTOR,TraceName,TracePPid,TraceExecute);

    if( (unsigned)(MPI_STATUS_TAG( Status )) >= (unsigned) &(ReactorTable[0])  //This checks to see if this
        &&                                                     //is a pointer to a ReactorTable
        (unsigned)(MPI_STATUS_TAG( Status )) < (unsigned) ReactorTableNext)    //entry of a function pointer
      {
      ReactorTableEntry *RTEP = (ReactorTableEntry *) (MPI_STATUS_TAG( Status ));
#if BLUEGENE
      ReactorRc = ((PkReactorFxPtr)(RTEP->RFxPtr))
                              ( RTEP->State,                   //If so call function as defined
                                AllBuf[ReqIndex] );
#else
      ReactorRc = ((PkReactorFxPtr)(RTEP->RFxPtr))
                              ( RTEP->State,                   //If so call function as defined
                                Status.source,                 //in ReactorTableEntry
                                Len,
                                AllBuf[ReqIndex] );
#endif
      }
    else
      {
#if BLUEGENE
      ReactorRc = ((PkActorFxPtr)((MPI_STATUS_TAG( Status ))))( AllBuf[ReqIndex] ); //we can call it directly.
#else
      ReactorRc = ((PkActorFxPtr)((MPI_STATUS_TAG( Status ))))( Status.source,            //If not (MPI_STATUS_TAG( Status )) is a
                                                      Len,                //pointer to a function, so
                                                      AllBuf[ReqIndex] ); //we can call it directly.
#endif
      }

    switch( ReactorRc )
      {
      case 0 :
        break;
      case 1 :
//////        AllBuf[ReqIndex] = new Buffer;  //The function still needs the old
        AllBuf[ReqIndex] = GetBuffer();       //buffer so we make a new one.
        break;
      default:
        assert( 0 );
      };

    BegLogLine( PKFXLOG_PKREACTOR )
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


    msgrc = MpiSubSys::Irecv( (void *) AllBuf[ReqIndex],      //Post a non-blocking receive on the
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
      MpiSubSys::Abort( MPI_COMM_WORLD, -1);
      }

#else // NOT MPISUBSYS_TEST_WAIT
                                                                   //All this is compiled out...
    MPI_Status *Status   = new MPI_Status[TOTAL_DATAGRAMS_IN_FLIGHT];
    int        *ReqIndex = new int[TOTAL_DATAGRAMS_IN_FLIGHT];
    int         Flag      = 0;
    int         OutCount;

    BegLogLine( PKFXLOG_PKREACTOR )
          <<  "Begin Waitsome()"
          << EndLogLine;

    mpirc = MpiSubSys::Waitsome( TOTAL_DATAGRAMS_IN_FLIGHT, AllRequests,&OutCount,
                         ReqIndex, Status );

    BegLogLine( PKFXLOG_PKREACTOR )
      <<  "Done  Waitsome()"
      << EndLogLine;

    assert( mpirc == 0 );

    BegLogLine( PKFXLOG_PKREACTOR )
      <<  "OutCount [" << OutCount << "]"
      << EndLogLine;

    unsigned msgrc;
    for (int loop2 = 0; loop2 < OutCount; loop2++)
      {
      BegLogLine( PKFXLOG_PKREACTOR )
          <<  "Flag"
          << Flag
          << EndLogLine;

      int Len;

      MpiSubSys::Get_count( &Status[loop2], MPI_CHAR, &Len );


      BegLogLine( PKFXLOG_PKREACTOR )
        << "ActiveMsg Processor: "
        << "Exec Begin PhysReactor @"
        <<  Status[loop2].tag
        << " TMsg From Node "
        <<  Status[loop2].source
        <<  " Len "
        <<  Len
        <<  " Data "
        <<  (void *) (*(int *)(AllBuf[ReqIndex[loop2]]))
        << EndLogLine;

      if( (unsigned)Status[loop2].tag >= (unsigned) &(ReactorTable[0])
          &&
          (unsigned)Status[loop2].tag < (unsigned) ReactorTableNext)
        {
        BegLogLine( PKFXLOG_PKREACTOR )
          << "ActiveMsg Processor: "
          << "Exec PhysReactor @"
          <<  Status[loop2].tag
          << " REACTOR "
          << EndLogLine;
        ReactorTableEntry *RTEP = (ReactorTableEntry *) Status[loop2].tag;
#if BLUEGENE
        ReactorRc = ((PkReactorFxPtr)(RTEP->RFxPtr))
                                ( RTEP->State,
                                  AllBuf[ReqIndex[loop2]] );
#else
        ReactorRc = ((PkReactorFxPtr)(RTEP->RFxPtr))
                                ( RTEP->State,
                                  Status[loop2].source,
                                  Len,
                                  AllBuf[ReqIndex[loop2]] );
#endif
        }
      else
        {
        BegLogLine( PKFXLOG_PKREACTOR )
          << "ActiveMsg Processor: "
          << "Exec PhysReactor @"
          <<  Status[loop2].tag
          << " ACTOR "
          << EndLogLine;

#if BLUEGENE
        ReactorRc = ((PkActorFxPtr)(Status[loop2].tag))( AllBuf[ReqIndex[loop2]] );
#else
        ReactorRc = ((PkActorFxPtr)(Status[loop2].tag))( Status[loop2].source,
                                                         Len,
                                                         AllBuf[ReqIndex[loop2]] );
#endif
        }

      switch( ReactorRc )
        {
        case 0 :
          break;
        case 1 :
/////          AllBuf[ReqIndex[loop2]] = new Buffer;
          AllBuf[ReqIndex[loop2]] = GetBuffer();
          break;
        default:
          assert( 0 );
        };


      BegLogLine( PKFXLOG_PKREACTOR )
          << "ActiveMsg Processor: "
        << "Exec End PhysReactor @"
        <<  Status[loop2].tag
        << " TMsg From Node "
        <<  Status[loop2].source
        <<  " Len "
        <<  Len
        <<  " Data  "
        <<  (void *) (*(int *)(AllBuf[ReqIndex[loop2]]))
        << EndLogLine;

      msgrc = MpiSubSys::Irecv( (void *) AllBuf[ReqIndex[loop2]],
                                 sizeof( Buffer ),
                                 MPI_CHAR,
                                 MPI_ANY_SOURCE,
                                 MPI_ANY_TAG,
                                 REACTIVE_MESSAGE_COMM_WORLD,
                                 &AllRequests[ReqIndex[loop2]] );
      assert( msgrc == 0 );

      if( msgrc != 0 )
        {
        BegLogLine( PKFXLOG_PKREACTOR )
          << "ActiveMsg Processor: "
          << " MPI Irecv failed, aborting program "
          << EndLogLine;
        MpiSubSys::Abort( MPI_COMM_WORLD, -1);
        }
      }
#endif

    }

  MpiSubSys::Finalize();
  _exit(0);
  return( NULL ); // Keep compiler quiet
  }

#endif //MPI

