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
 * Module:          globject.cpp
 *
 * Purpose:         Implementation of global object system methods.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/
#include <pk/platform.hpp>
#include <pk/globject.hpp>

#define BLUEGENE // this mostly indicates that the active message system does not provide source/len info to actor handlers


#ifndef PKFXLOG_GLOBJECT
#define PKFXLOG_GLOBJECT (0)
#endif


class TableElement      //This holds the information
  {                     //required to find an enrolled globject.
  public:
    void         *HomeAddress;
    unsigned      TableKey;    // This will be needed to expand the table.

    TableElement()
      {
      HomeAddress = NULL;
      }

    TableElement&  operator = (const TableElement& source)
      {
      if (this == &source)
         return( *this );

      HomeAddress = source.HomeAddress;
      TableKey    = source.TableKey   ;

      return( *this );
      }
  };





GCM_TypeFreeBase::GCM_TypeFreeBase()
  {
  ClassTypeNumber    = 0xFFFFFFFF;  // until assigned by a derived class.
  ClassObjectSize    = 0xFFFFFFFF;  // until assigned by a typed derived class.

  TableSize               = 1024;
                                                            //Initialises array that will
  TableNextFreeIndex      = 0;                              //reference all the enrolled
  // Table                   = new TableElement[ TableSize ];  //objects of thi type.
  pkNew( &Table, TableSize );
#if defined(PK_BLADE)
  TableAccessLock.mVar = 0;
#else
  #ifdef PK_BGL
    BGL_Mutex* TempMutex = rts_allocate_mutex();
    memcpy( &TableAccessLock, TempMutex, sizeof( BGL_Mutex ) );
  #else
    TableAccessLock = 0;
  #endif
#endif

  }

// NEED: Id's should be hashed so that we can have lots
//       right now we just burn id so we can use modulo
// NEED: Manage a fixed number of Ids for objects that are in cache.
unsigned GCM_TypeFreeBase::GetNextId()
  {
  unsigned int TableIndex = 0xFFFFFFFF;
                                                         //Finds a free slot in the Table.
  for( int i = TableNextFreeIndex; i < TableSize; i++ )
    {
    TableIndex = i % TableSize ;

    if( Table[ TableIndex ].HomeAddress == NULL )
      break;
    }

  // The following assert means that the whole table is full.
  // Right now we have a fixed size table, so max no of globjects.
  // A cheap bad solution would be to double the size of the table
  // and rehash (%) everything.  Then, run the above loop again to find a
  // Free slot.

  if ( TableIndex == 0xFFFFFFFF )
    {
    BegLogLine( PKFXLOG_GLOBJECT )
        << "Starting rehash....."
        << EndLogLine;

    TableElement* OldTable = Table;
    int           OldTableSize = TableSize ;
    int loop;

    TableSize = TableSize * 2;
    // Table = new TableElement[TableSize];
    pkNew( &Table, TableSize );

    for (loop = 0; loop < OldTableSize; loop++)
      {
      Table[loop] = OldTable[loop];
      }

    delete OldTable;

    // Repeat loop
    for( int i = TableNextFreeIndex; i < TableSize; i++ )
      {
      TableIndex = i % TableSize ;

      if( Table[ TableIndex ].HomeAddress == NULL )
        break;
      }
    BegLogLine( PKFXLOG_GLOBJECT )
      << "....finishing rehash"
      << EndLogLine;

    }

  assert (TableIndex != 0xFFFFFFFF);

  // To get here a Free gap has been found
  TableNextFreeIndex++;

  if( TableIndex > TableSize * 0.75 )
    {
    BegLogLine( PKFXLOG_GLOBJECT )
      << "WARNING: Global Object Table filling"
      << EndLogLine;
    }

  // should bounds check TableNextFreeIndex as less that 2**24
  BegLogLine( PKFXLOG_GLOBJECT )
    << "GCM_TypeFreeBase::GetNextId returning "
    << TableNextFreeIndex
    << EndLogLine;

  return( TableNextFreeIndex - 1);

  }

int
GCM_TypeFreeBase::
FreeId( unsigned Id )
  {
  unsigned TableIndex = Id % TableSize;                 //Frees a slot in the table.
  assert( Table[ TableIndex ].HomeAddress != NULL );
  Table[ TableIndex ].HomeAddress = NULL;
  return(0);
  }

void
GCM_TypeFreeBase::
SetClassTypeNumber( unsigned aClassTypeNumber )
  {
  assert( ClassTypeNumber == 0xFFFFFFFF );       //Sets the class type for this GCM.
  ClassTypeNumber = aClassTypeNumber;
  }

unsigned
GCM_TypeFreeBase::
GetClassTypeNumber( )
  {
  assert( ClassTypeNumber != 0xFFFFFFFF );   //Return the class type number
  return( ClassTypeNumber );
  }

void
GCM_TypeFreeBase::
SetClassObjectSize( unsigned aClassObjectSize )
  {
  assert( ClassObjectSize == 0xFFFFFFFF );
  ClassObjectSize = aClassObjectSize;
  }

unsigned GCM_TypeFreeBase::GetClassObjectSize( )
  {
  assert( ClassObjectSize != 0xFFFFFFFF );
  return( ClassObjectSize );
  }

GlobjectReference
GCM_TypeFreeBase::
TypeFreeEnroll( void* aGlobject )
  {
  GlobjectReference ref;

  ///YieldLockOp(YieldLockOpCode_LOCK);
  Lock();

#ifdef RUNTIME_TYPE_CHECKING
  ref.TypeNumber = GetClassTypeNumber();
  BegLogLine( PKFXLOG_GLOBJECT )
      << "In TypeFreeEnroll TypeNumber " << GetClassTypeNumber()
      << EndLogLine;

#endif
  ref.HomeMemory = Platform::Topology::GetAddressSpaceId();      //Set the memory space and table key
  ref.TableKey = GetNextId();         //for the new GlobjectReference.
#if defined(PK_BLADE)
  BegLogLine( PKFXLOG_GLOBJECT )
      << "Array position " << (ref.TableKey % TableSize)
      << " Dummied thread Number [BGL]"
      << EndLogLine;
#elif defined( PK_PTHREADS )

  BegLogLine( PKFXLOG_GLOBJECT )
      << "Array position " << (ref.TableKey % TableSize)
      << " Thread Number [" << pthread_self() << "]"
      << EndLogLine;
#endif

  Table[ref.TableKey % TableSize ].HomeAddress = aGlobject;  //Put the address of the Globject
                                                             //into the table under the appropriate
  ////YieldLockOp(YieldLockOpCode_UNLOCK);                       //key.
  Unlock();

  return( ref );
  }

int
GCM_TypeFreeBase::
TypeFreeExpel( GlobjectReference ref )
  {
  Lock();

#ifdef RUNTIME_TYPE_CHECKING
  assert( ref.TypeNumber == GetClassTypeNumber() );
#endif
  // The following assret demands that expels occur
  // in the Home Memory space.                        //Remove a Globject from the table
  assert( ref.HomeMemory == Platform::Topology::GetAddressSpaceId() );           //by freeing up the slot that references
  FreeId( ref.TableKey );                             //it.

  Unlock();

  return( 0 );
  }

void* GCM_TypeFreeBase::GetObjectPointer( GlobjectReference ref ) //Returns a pointer the
  {                                                               //object referenced by
  BegLogLine( PKFXLOG_GLOBJECT )                                  //GlobjectReference.
    << "In GCM_TypeFreeBase::GetObjectPointer ref.TableKey"
    << (int) ref.TableKey
    << EndLogLine;

  return( Table[ref.TableKey % TableSize ].HomeAddress );
  }

#ifdef PK_MPI_ALL // really #ifdef PKACTOR

// NEED: The following actor NEEDs to detach as a thread before doing blocking sends
// NEED: The following actor NEEDs to detach as a thread before doing blocking sends
// NEED: The following actor NEEDs to detach as a thread before doing blocking sends


int GCM_TypeFreeBase::TypeFreeFetchBackEnd(
#ifndef BLUEGENE
                                            unsigned SrcProcess, //This function will be
                                            unsigned Len,        //triggerd in the locality
#endif
                                            void *TriggerMsg )   //of the enroled globject
  {                                                              //and sends a copy of the
  TypeFreeFetchTriggerMsg *Msg = (TypeFreeFetchTriggerMsg *) TriggerMsg;

#ifdef BLUEGENE
  unsigned SrcProcess = Msg->Source;
  unsigned Len        = Msg->Len;
#endif

  int mpirc = 0;

#ifdef MPISUBSYS_TRACE
  static PkTraceStreamDcl MpiSubSysSendBefore;
  static PkTraceStreamDcl MpiSubSysSendAfter;
#endif

  BegLogLine( PKFXLOG_GLOBJECT )
    << " TypeFreeFetchBackEnd Entered..."
    << EndLogLine;

  assert( Len == sizeof( TypeFreeFetchTriggerMsg ) );

  BegLogLine( PKFXLOG_GLOBJECT )
    << "Address of Msg " << Msg
    << EndLogLine;

  BegLogLine( PKFXLOG_GLOBJECT )
    << "GCM_TypeFreeBase::TypeFreeFetchBackEnd Msg->ref.TableKey "
    << (int) Msg->ref.TableKey
    << EndLogLine;

  void *ObjAddr = Msg->IFPtrFx()->GetObjectPointer(Msg->ref);
                                                              //Use IFPtrFx to get a pointer
  int   ObjSize = Msg->IFPtrFx()->GetClassObjectSize();       //to GCM_TypeFreeBase IF. Use this
                                                              //pointer to get Object Address &
  BegLogLine( PKFXLOG_GLOBJECT )                              //size.
    << " TypeFreeFetchBackEnd "
    << " Send To: "
    << " Addr " << (unsigned) ObjAddr
    << " Size " << ObjSize
    << " Dest " << SrcProcess
    << " Type " << Msg->ReturnMsgType
    << EndLogLine;

#ifndef MPISUBSYS_TRACE

  mpirc = MPI_Send(  ObjAddr,           //Send the requested object back
                     ObjSize,                  //to the source process.
                     MPI_CHAR,
                     SrcProcess,
                     Msg->ReturnMsgType,
                     MPI_COMM_WORLD );

#else
  mpirc = MPI_Send(  ObjAddr,
                     ObjSize,
                     MPI_CHAR,
                     SrcProcess,
                     Msg->ReturnMsgType,
                     MPI_COMM_WORLD,
                     &MpiSubSysSendBefore,
                     &MpiSubSysSendAfter,
                     "MPISendGlbObj");
#endif

  assert( mpirc == 0 );

  BegLogLine( PKFXLOG_GLOBJECT )
    << "...TypeFreeFetchBackEnd Exited"
    << EndLogLine;

  return(0);
  }

#endif //MPI defined.

int GCM_TypeFreeBase::TypeFreeFetch( GlobjectReference ref, char *ObjBuf )
  {
                                                     //This function retrieves the Globject
#ifdef MPISUBSYS_TRACE                               //referenced by the GlobjectRefrerence
  static PkTraceStreamDcl MpiSubSysIRecvBefore;      //that is passed to it.
  static PkTraceStreamDcl MpiSubSysIRecvAfter;       //This is handled differently depending
  static PkTraceStreamDcl MpiSubSysGetCountBefore;   //on wether the Globject is stored remotely
  static PkTraceStreamDcl MpiSubSysGetCountAfter;    //or localy.

#endif

  Lock();

  BegLogLine( PKFXLOG_GLOBJECT )
        << " TypeFreeFetch Entered "
        << " ref.HomeMemory "
        << (int) ref.HomeMemory
        << " ref.TableKey "
        << (int) ref.TableKey
        << EndLogLine;

#ifdef RUNTIME_TYPE_CHECKING
  BegLogLine( PKFXLOG_GLOBJECT )
        << " TypeFreeFetch RUNTIME_TYPE_CHECKING "
        << " ref.TypeNumber "
        << ref.TypeNumber
        << " GetClassTypeNumber() " << GetClassTypeNumber()
        << EndLogLine;
  if ( ref.TypeNumber != GetClassTypeNumber() )
    {
    BegLogLine( PKFXLOG_GLOBJECT )
        << "FATAL Error in RUNTIME_TYPE_CHECKING "
        << EndLogLine;
    }

  assert( ref.TypeNumber == GetClassTypeNumber() );
#endif

  if( ref.HomeMemory == Platform::Topology::GetAddressSpaceId() )      //If the global object is stored in
    {                                       //this locality make a copy in the
    BegLogLine( PKFXLOG_GLOBJECT )          //object buffer
          << " TypeFreeFetch "
          << " Doing memcpy() "
          << EndLogLine;

    memcpy( ObjBuf,
            Table[ ref.TableKey % TableSize ].HomeAddress,
            GetClassObjectSize() );
    }
#ifndef PK_MPI_ALL
  else
    {
    assert(0);                              //If not MPI the we should have
    }                                       //one memory space only.
#else // MPI and thus PKACTOR is included...

  else                                      //If global object is in another
    {                                       //locality so we need to get it.
    // Could look in a cache.  Could be hoky: keep the last one here.
    // This caching is potentially horribly dangerous
    // - think about invalidates when the origional is deleted!!!!
    // Probably should verify before using cache and use only for big-uns.

    MPI_Status Status;
    int mpirc = 0;

    TypeFreeFetchTriggerMsg Msg;
    Msg.ref        = ref;            //Put GlobjectReference into message.

#ifdef BLUEGENE
    Msg.Source     = Platform::Topology::GetAddressSpaceId();
    Msg.Len        = sizeof(TypeFreeFetchTriggerMsg);
#endif
    // The following should be done via a: GetUniquePort() functi on.

    Msg.ReturnMsgType = (unsigned) ObjBuf;  //Use Obj buffer address as unique
                                            //massage tag.

    // The following is how the type Free back-end function operates sans type.
    Msg.IFPtrFx = (GCM_TypeFreeBase * (*)()) GetClassTypeNumber();        //Will be used to get
                                                                          //a pointer to the
    Platform::Reactor::Trigger( ref.HomeMemory,                           //appropriate GCM IF
                                TypeFreeFetchBackEnd,
                                (void *) &Msg,
                                sizeof(   Msg  ) );   //Trigger the TypeFreeFetchBackEnd
                                                      //process in the locality of stored
    BegLogLine( PKFXLOG_GLOBJECT )                    //Globject. This will send us a copy
          << " TypeFreeFetch "                        //of the required object.
          << " Recv Start "
          << EndLogLine;



#ifndef MPISUBSYS_TRACE

    mpirc = MPI_Recv( ObjBuf,            //Blocking receive to get Object returned
                      GetClassObjectSize(),     //by remote TypeFreeFetchBackEnd process.
                      MPI_CHAR,
                      ref.HomeMemory,
                      Msg.ReturnMsgType, // hopefully this address
                                         // is unique to the process.
                      MPI_COMM_WORLD,
                      &Status);

#else

    mpirc = MPI_Recv( ObjBuf,
                      GetClassObjectSize(),
                      MPI_CHAR,
                      ref.HomeMemory,
                      Msg.ReturnMsgType, // hopefully this address
                                         // is unique to the process.
                      MPI_COMM_WORLD,
                      &Status,
                      &MpiSubSysIRecvBefore,
                      &MpiSubSysIRecvAfter,
                      "MPIGlbObjTypeFreeFetchIRecv");

#endif

    assert( mpirc == 0 );

    BegLogLine( PKFXLOG_GLOBJECT )
          << " TypeFreeFetch "
          << " Recv Finish "
          << EndLogLine;


    int TrueLen;
#ifndef MPISUBSYS_TRACE
    MPI_Get_count( &Status, MPI_CHAR, &TrueLen );
#else
    MPI_Get_count( &Status, MPI_CHAR, &TrueLen,
                          &MpiSubSysGetCountBefore,
                          &MpiSubSysGetCountAfter,
                          "MPIGlbObjTypeFreeFetchGetCount");

#endif

    assert( TrueLen == GetClassObjectSize() );    //Make sure that what we got back is
    }                                             //of expected size.
#endif // MPI

  Unlock();

  return( 0 );
  }

