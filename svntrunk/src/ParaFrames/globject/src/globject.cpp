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
#include <Pk/API.hpp>
#include <Pk/globject.hpp>

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

GCM_TypeFreeBase::
GCM_TypeFreeBase()
  {
  ClassTypeNumber    = 0xFFFFFFFF;  // until assigned by a derived class.
  ClassObjectSize    = 0xFFFFFFFF;  // until assigned by a typed derived class.

  TableSize               = 1024;
                                                            //Initialises array that will
  TableNextFreeIndex      = 0;                              //reference all the enrolled
  // Table                   = new TableElement[ TableSize ];  //objects of thi type.
  ////pkNew( &Table, TableSize );
  Table = (TableElement *) PkHeapAllocate( TableSize );

  }

// NEED: Id's should be hashed so that we can have lots
//       right now we just burn id so we can use modulo
// NEED: Manage a fixed number of Ids for objects that are in cache.
unsigned
GCM_TypeFreeBase::
GetNextId()
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
    /////pkNew( &Table, TableSize );
    Table = (TableElement*) PkHeapAllocate( TableSize );

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

  //////ref.HomeMemory = Platform::Topology::GetAddressSpaceId();      //Set the memory space and table key
  ref.HomeMemory = PkNodeGetId();      //Set the memory space and table key
  ref.TableKey   = GetNextId();         //for the new GlobjectReference.

  BegLogLine( PKFXLOG_GLOBJECT )
      << "Array position " << (ref.TableKey % TableSize)
      << " Dummied thread Number [BGL]"
      << EndLogLine;

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
  ////assert( ref.HomeMemory == Platform::Topology::GetAddressSpaceId() );           //by freeing up the slot that references
  AssertLogLine( ref.HomeMemory == PkNodeGetId() )           //by freeing up the slot that references
    << "TypeFreeExpel(): "
    << "ref.HomeMemory " << ref.HomeMemory
    << EndLogLine;

  FreeId( ref.TableKey );                             //it.

  Unlock();

  return( 0 );
  }

void*
GCM_TypeFreeBase::
GetObjectPointer( GlobjectReference ref ) //Returns a pointer the
  {                                                               //object referenced by
  BegLogLine( PKFXLOG_GLOBJECT )                                  //GlobjectReference.
    << "In GCM_TypeFreeBase::GetObjectPointer ref.TableKey"
    << (int) ref.TableKey
    << EndLogLine;

  return( Table[ref.TableKey % TableSize ].HomeAddress );
  }

int
GCM_TypeFreeBase::
TypeFreeFetchBackEnd( void *TriggerMsg )   //of the enroled globject
  {                                                              //and sends a copy of the
  TypeFreeFetchTriggerMsg *Msg = (TypeFreeFetchTriggerMsg *) TriggerMsg;

  unsigned SrcProcess = Msg->Source;
  unsigned Len        = Msg->Len;

  int mpirc = 0;

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

// need a long message


  assert( mpirc == 0 );

  BegLogLine( PKFXLOG_GLOBJECT )
    << "...TypeFreeFetchBackEnd Exited"
    << EndLogLine;

  return(0);
  }

int GCM_TypeFreeBase::TypeFreeFetch( GlobjectReference ref, char *ObjBuf )
  {
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

  ////if( ref.HomeMemory == Platform::Topology::GetAddressSpaceId() )      //If the global object is stored in
  // need to have a single word talk about node/cores
  if( ref.HomeMemory == PkNodeGetId() )      //If the global object is stored in
    {                                       //this locality make a copy in the
    BegLogLine( PKFXLOG_GLOBJECT )          //object buffer
          << " TypeFreeFetch "
          << " Doing memcpy() "
          << EndLogLine;

    memcpy( ObjBuf,
            Table[ ref.TableKey % TableSize ].HomeAddress,
            GetClassObjectSize() );
    }
  else                                      //If global object is in another
    {                                       //locality so we need to get it.
    // Could look in a cache.  Could be hoky: keep the last one here.
    // This caching is potentially horribly dangerous
    // - think about invalidates when the origional is deleted!!!!
    // Probably should verify before using cache and use only for big-uns.

    // See if a packet is bolted up - if not, get one if mState allows
    pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

    pkFullPacketBuffer *ToBeDispatched = mHead;

    mHead = pkActorPacketPoolAllocate( fascb );

    if( mHead == NULL ) // out of packet buffers
       {
       static WarnOnceOnOutOfPacketBuffers = 1;
       BegLogLine(WarnOnceOnOutOfPacketBuffers)
        << "ChannelWriter::LockAndLoad():"
        << " this@ "    << (void*)this
        << " ********* WARNING OUT OF PACKET BUFFERS *********"
        << " mSlotNo "  << mSlotNo
        << " mSeqNo "   << mSeqNo
        << " mAddr "    << (void*) mAddr
        << EndLogLine;
       WarnOnceOnOutOfPacketBuffers = 0;
       return( 0 );
       }

    TypeFreeFetchTriggerMsg Msg;
    Msg.ref        = ref;            //Put GlobjectReference into message.

    Msg.Source     = PkNodeGetId();
    Msg.Len        = sizeof(TypeFreeFetchTriggerMsg);

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


// need a long message receive -- for now we'll just do a 1 packet receive

    BegLogLine( PKFXLOG_GLOBJECT )
          << " TypeFreeFetch "
          << " Recv Finish "
          << EndLogLine;


    int TrueLen;

    assert( TrueLen == GetClassObjectSize() );    //Make sure that what we got back is
    }                                             //of expected size.

  Unlock();

  return( 0 );
  }



#if 0

#include <Pk/API.hpp>

#include <Pk/FASched.Init.cpp>

#if 1
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <Pk/XYZ.hpp>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <rts.h>
#include <BonB/blade_on_blrts.h>

//#include "BGLTorusAppSupport.c"
////#include <pk/fxlogger.hpp>
////#include <pk/platform.hpp>

#include <BonB/BonB_CoRoutine.h>

#endif

//#include <builtins.h>

#ifndef FXLOG_VERBOSE
#define FXLOG_VERBOSE (0)
#endif

#ifndef PK_COROUTINE
#define PK_COROUTINE (0)
#endif

#ifndef PK_BGL_ACTOR
#define PK_BGL_ACTOR (0)
#endif

#ifndef PK_TORUS_BARRIER
#define PK_TORUS_BARRIER (0)
#endif

#ifndef PK_SHMEM
#define PK_SHMEM (0)
#endif

#ifndef TOY_APP
#define TOY_APP (0)
#endif

unsigned TaskId    = 0xFFFFFFFF;
unsigned TaskCount = 0xFFFFFFFF;

int aCore = 0; // change when we go dual core


template<class ItemType>
class GetItemIndirectAsync
  {
  public:

    struct
    ResponsePacket
      {
      struct Header
        {
        unsigned int   mFlagBitAddr;  // Which addr of bit to set on return... bit number in lower 3 bits
        ItemType*      mDataAddr;
        } mHeader;
      ItemType         mData;
      };

    struct
    RequestPacket
      {
      ItemType**             mBasePtr;
      unsigned int           mIndex;
      typename ResponsePacket::Header mResponseHeader;
      };

    inline
    static
    int
    RequestFx( void *arg )
      {
      RequestPacket * ReqPkt = (RequestPacket *) arg;

      ResponsePacket * ResPkt =
            (ResponsePacket *)
            PkActorReserveResponsePacket( ResponseFx,
                                          sizeof( ResponsePacket ) );

      assert( ResPkt );

      ItemType * LocalArray = *(ReqPkt->mBasePtr);

      BegLogLine( PK_SHMEM )
        << "RequestFx() " // << (void*) RequestFx<ItemType>
        << " BasePtr "      << (void*) ReqPkt->mBasePtr
        << " Index "        << ReqPkt->mIndex
        << " LoadAddr "     << (void*) &(LocalArray[ ReqPkt->mIndex ])
        << " sizeof(Item) " << sizeof( ItemType )
        << EndLogLine;

      assert( LocalArray );

      ResPkt->mData   = LocalArray[ ReqPkt->mIndex ];
      ResPkt->mHeader = ReqPkt->mResponseHeader;

      PkActorPacketDispatch( ResPkt );

      return(NULL);
      }

    static
    inline
    void
    SetFlagBit(unsigned int aFlagBitAddr )
      {
      int    flag_bit  = 0x0000001F & aFlagBitAddr;  // pick the bit in word index
      int *  flag_addr = (int*) ( (aFlagBitAddr >> 3) & (~0x03) ); // slide down to have the word addr
      int    flag_mask = 0x01 << flag_bit;
      *flag_addr |= flag_mask;
      }

    inline
    static
    int
    ResponseFx( void *arg )
      {
      ResponsePacket * ResPkt = (ResponsePacket *) arg;

      // Move the data item into place
      *ResPkt->mHeader.mDataAddr = ResPkt->mData;

      // set the bit showing the op is completed
      SetFlagBit( ResPkt->mHeader.mFlagBitAddr );

      BegLogLine( PK_SHMEM )
        << "ResponseFx() " // << (void*) RequestFx<ItemType>
        << " sizeof(Item) " << sizeof( ItemType )
        << " Item "         << (ResPkt->mData)
        << " FlagBitAddr "  << (void*) ResPkt->mHeader.mFlagBitAddr
        << " Bit "          << (void*) (0x0000001F & ( ResPkt->mHeader.mFlagBitAddr ))
        << " Addr "         << (void*) ( ResPkt->mHeader.mFlagBitAddr >> 5 )
        << EndLogLine;
      return(NULL);
      }

    static
    void
    Fetch( ItemType*        aDataAddr,
           unsigned  int   *aFlagAddr,
           unsigned  int    aFlagBit,
           unsigned  int    aPE,
           ItemType**       aBasePtr,
           unsigned  int    aIndex)
      {
      assert( aFlagBit >= 0 && aFlagBit < 32 );
      assert( ((unsigned)aFlagAddr & 0x00000003) == NULL );  // must be 4 byte aligned
      assert( ((unsigned)aFlagAddr & 0xE0000000) == NULL );

      // addr is a word aligned addr ... lower two bits should be 0
      unsigned int FlagBitAddr = (((unsigned)aFlagAddr)<<3) + aFlagBit;

      BegLogLine( PK_SHMEM )
        << "ShmemLoadFromBase() "
        << " aTarget "              << (void*) aDataAddr
        << " FlagBitAddr  "         << (void*) FlagBitAddr
        << " Addr  "                << (void*) aFlagAddr
        << " Bit "                  << (void*) aFlagBit
        << " aPE "                  << aPE
        << " aBasePtr "             << (void*) aBasePtr
        << " aIndex "               << aIndex
        << EndLogLine;


      if( aPE == TaskId )
        {
        *aDataAddr = (*aBasePtr)[ aIndex ];
        SetFlagBit( FlagBitAddr );
        BegLogLine( PK_SHMEM )
          << "ShmemLoadFromBase() "
          << " Local PE return"
          << " value " << *aDataAddr
          << EndLogLine;
        }
      else
        {
        // Send request
        RequestPacket * ReqPkt =
              (RequestPacket *) PkActorReservePacket( aPE,
                                                      0,
                                                      RequestFx,
                                                      sizeof( RequestPacket ) );

        ReqPkt->mBasePtr                     = aBasePtr;
        ReqPkt->mIndex                       = aIndex;
        ReqPkt->mResponseHeader.mFlagBitAddr = FlagBitAddr;
        ReqPkt->mResponseHeader.mDataAddr    = aDataAddr;

        PkActorPacketDispatch( ReqPkt );

        BegLogLine( PK_SHMEM )
          << "ShmemLoadFromBase() "
          << " remote node returning "
          << EndLogLine;
        }
      }
  };


struct
LoaderFiberArgs
  {
  pkFiberControlBlockT*   FCB; // not required but convenient
  unsigned long long     mInitialValue;
  unsigned int           mIterations;
  unsigned int           mNumberOfOutstandingLoads;
  unsigned int           mLocalArrayCount;
  unsigned long long **  mBigArrayAnchorAddr;
  };

#define MAX_OUTSTANDING_LOADS (32)

void
LoaderFiber( void * args )
  {
  int a;
  int b;
  BegLogLine( TOY_APP )
    << "LoaderFiber() Entered " << " @a " << (void*) &a << " @b " << (void*) &b << EndLogLine;
  LoaderFiberArgs * LTA = (LoaderFiberArgs *) args;
  assert( LTA );
  unsigned long long *BigArray = *LTA->mBigArrayAnchorAddr;
  BegLogLine( TOY_APP )
    << "LoaderFiber() "
    << " arg "             << (void*) args
    << " InitValue "       << LTA->mInitialValue
    << " Iterations "      << LTA->mIterations
    << " NumberOfOutstandingLoads " << LTA->mNumberOfOutstandingLoads
    << " LocalArrayCount " << LTA->mLocalArrayCount
    << " BigArrayAnchorAddr " << (void*) LTA->mBigArrayAnchorAddr
    << " BigArray "           << (void*) BigArray
    << EndLogLine;

  unsigned long long Check[ MAX_OUTSTANDING_LOADS ];
  unsigned long long Value[ MAX_OUTSTANDING_LOADS ];

  int NumberOfOutstandingLoads = LTA->mNumberOfOutstandingLoads;
  assert( NumberOfOutstandingLoads > 0 );
  assert( NumberOfOutstandingLoads <= MAX_OUTSTANDING_LOADS );

  // Prime the main loop.  Set up connurrent load contexts.
  unsigned int Flags = 0;
  for(int i = 0; i < NumberOfOutstandingLoads; i++ )
    {
    Flags   |= 1<<i;
    Value[i] = BigArray[i];
    BegLogLine( TOY_APP )
      << "LoaderFiber() "
      << " Initials "             << i
      << " Flags "      << (void*) Flags
      << " Value " << Value[i]
      << EndLogLine;
    }


  int Dispatched = 0;
  int Retired    = 0 - NumberOfOutstandingLoads ;  // loop below will use retire code path to do initial issues

  //for( int l = 0; l < LTA->mIterations; l++ )
    BegLogLine( TOY_APP )
      << "LoaderFiber() "
      << " Retired " << Retired
      << " Issued " << Dispatched
      << " LTA->mIterations " <<  LTA->mIterations
      << EndLogLine;

  // Move the unsigned into a signed var otherwise while loop isn't entered.
  int  Iterations = LTA->mIterations;
  unsigned int LocalArrayCount = LTA->mLocalArrayCount;
  unsigned long long ** BigArrayAnchorAddr = LTA->mBigArrayAnchorAddr;

  while(Retired < Iterations )
    {
    // Effectively block this coroutine on flags being set - this could be done better with coroutine infrastructure
    //int i = 32;
    ///while( (index = __cntlz4( Flags )) == 32 )

    PkFiberYield();

    unsigned int SnapShot = Flags;  // probably not required ... if no call services the network...
    Flags = 0;

    int Bit = 0;
    int BitMask = 1;

    // Handle all data flaged in snapshot
    while( SnapShot != 0 )
      {
      // Get here, we got a bit ... should be cntlz

      while( ! ( SnapShot & BitMask ) )
        {
        Bit++;
        BitMask <<= 1;
        }

      Check[Bit] ^= Value[Bit];
      unsigned int Node  = Value[Bit] / LocalArrayCount;
      unsigned int Index = Value[Bit] % LocalArrayCount;

      BegLogLine( TOY_APP )
        << "LoaderFiber() "
        << " RETIRED "
        << " Flags "    << (void*) Flags
        << " SnapShot " << (void*) SnapShot
        << " i "        << Bit
        << " Retired "  << Retired
        << " Value "    << Value[Bit]
        << " Issued "   << Dispatched
        << " Node "     << Node
        << " Index "    << Index
        << " Iters "    << Iterations
        << " BitMask "  << BitMask
        << EndLogLine;


      // Turn off the flag in both the SnapShot and the Flags
      SnapShot ^= BitMask;
      //////// now set to 0 after snapshot is taken above Flags    ^= BitMask;

      Retired++;

      if( Dispatched < Iterations )
        {
        GetItemIndirectAsync<unsigned long long>::Fetch( &(Value[Bit]),           // address to store return value
                                                &Flags,            // address of flag word
                                                 Bit,               // bit number is context
                                                 Node,            // Fetch target node
                                                 BigArrayAnchorAddr,  // Fetch target indirect base addr
                                                 Index );         // Fetch target index into array pointed to by base
        Dispatched++;
        BegLogLine( TOY_APP )
          << "LoaderFiber() "
          << " ISSUED "
          << " Flags "    << (void*) Flags
          << " SnapShot " << (void*) SnapShot
          << " i "        << Bit
          << " Retired "  << Retired
          << " Value "    << Value[Bit]
          << " Issued "   << Dispatched
          << " Node "     << Node
          << " Index "    << Index
          << " Iters "    << Iterations
          << " BitMask "  << BitMask
          << EndLogLine;
        }
      }
    }


  BegLogLine( TOY_APP )
    << "LoaderFiber() Exiting "
    << " Retired " << Retired
    << " Iters "    << LTA->mIterations
    << EndLogLine;
  }
#endif
