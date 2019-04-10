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
 * Module:          pkrnew.hpp
 *
 * Purpose:         Provide remote object "new"ing.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/
#ifndef __PKRNEW_HPP__
#define __PKRNEW_HPP__

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
/////////#include <pk/reactor.hpp>

#ifndef PKFXLOG_PKRNEW
#define PKFXLOG_PKRNEW (0)
#endif


template<class ClassOfStoreObject>
class RemoteStoreMessage
  {
  public:
#ifdef BLUEGENE
    unsigned src;
    unsigned len;
#endif
    ClassOfStoreObject *TargetAddress;
    ClassOfStoreObject  PayLoad;
  };

template<class ClassOfObject>
class RemoteObjectHandle
  {
  public:
    unsigned       Locality;
    ClassOfObject *Address;
  };

template<class ClassOfStoreObject>
class RemoteObject
  {
  private:
    static int
#ifndef BLUEGENE
    StoreObjectBackEnd( unsigned SrcProccess, unsigned Len, void *TriggerMsg )
      {
#else
    StoreObjectBackEnd( void *TriggerMsg )
      {
      RemoteStoreMessage<ClassOfStoreObject> *StoreMsg =
          (RemoteStoreMessage<ClassOfStoreObject> *) TriggerMsg;
      unsigned SrcProccess = StoreMsg->src;
      unsigned Len        = StoreMsg->len;
#endif
      BegLogLine( PKFXLOG_PKRNEW )
        << " RemObjStoreBackend "
        << "Src "
        <<  SrcProccess
        << " Addr @"
        <<  (unsigned) ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->TargetAddress
        << EndLogLine;

      // assuming StoreObjectMsg is has the following format...
      BegLogLine( PKFXLOG_PKRNEW )
        << " RemObjStoreBackend "
        << "Store Payload.Loc "
        <<   ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->PayLoad.Locality
        << " Payload.Address "
        << (unsigned)  ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->PayLoad.Address
        << EndLogLine;

      //Pprintf("StoreObjectBackEnd src %03d, Len %05d TargetAddr %08x\n",
      //  SrcProccess,
      //  Len,
      //  ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->TargetAddress );
      //fflush(stdout);

      // The following assumes that the ClassOfStoreObject *is* RemoteObjectHandle
      //Pprintf("StoreObjectBackEnd PayLoad: locality %03d, Address %08x\n",
      //  ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->PayLoad.Locality,
      //  ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->PayLoad.Address );
      //fflush(stdout);

      // Put the object into place.
      *(((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->TargetAddress) =
        ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->PayLoad;

      // The following assumes that the ClassOfStoreObject *is* RemoteObjectHandle
      //Pprintf("StoreObjectBackEnd Values At  TargetAddr: locality %03d, Address %08x\n",
      //  ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->TargetAddress->Locality,
      //  ((RemoteStoreMessage<ClassOfStoreObject> *)TriggerMsg)->TargetAddress->Address );
      //fflush(stdout);

      return(0);
      }

  public:
    static int
    Store( unsigned Locality, RemoteStoreMessage<ClassOfStoreObject> *StoreObjectMsg )
      {
      BegLogLine( PKFXLOG_PKRNEW )
        << "RemObjStore "
        << "Store To Loc "
        << Locality
        << " Addr @"
        << (unsigned) StoreObjectMsg->TargetAddress
        << EndLogLine;

      // assuming StoreObjectMsg is has the following format...
      BegLogLine( PKFXLOG_PKRNEW )
        << "RemObjStore "
        << "Store Payload.Loc "
        <<   StoreObjectMsg->PayLoad.Locality
        << " Payload.Address "
        << (unsigned)  StoreObjectMsg->PayLoad.Address
        << EndLogLine;

      //Pprintf("Store dest %03d, TargetAddress %08X\n", Locality, StoreObjectMsg->TargetAddress );
      //fflush(stdout);

      // The following assumes that the ClassOfStoreObject *is* RemoteObjectHandle
      //Pprintf("Store PayLoad: locality %03d, Address %08x\n",
      //  StoreObjectMsg->PayLoad.Locality,
      //  StoreObjectMsg->PayLoad.Address );
      //fflush(stdout);

      Platform::Reactor::Trigger( Locality,
               StoreObjectBackEnd,
               (void *) StoreObjectMsg,
               sizeof( *StoreObjectMsg ) );
      return(0);
      }
  };


//LINUX RemoteObjectHandle was here before
//LINUX RemoteStoreMessage was here before

template <class ClassOfObject, class ClassOfConstructorParms>
class RemoteObjectOf
  {
  public:
    RemoteObjectHandle<ClassOfObject> ObjectHandle;

    typedef RemoteStoreMessage< RemoteObjectHandle<ClassOfObject> > ReturnStoreMsgType;

    // This struct is used to repackage constructor
    class TriggerAndRcAddr
      {
      public:
#ifdef BLUEGENE
        unsigned src;
        unsigned len;
#endif
        ReturnStoreMsgType       ReturnStoreMsg;
        char                     ConstructorParms[ sizeof( ClassOfConstructorParms ) ];
        TriggerAndRcAddr()
          {
          }
      };

  public:
    static int
#ifndef BLUEGENE
    NewBackEnd( unsigned SrcProcess, unsigned Len, void *TriggerMsg )
      {
#else
    NewBackEnd( void *TriggerMsg )
      {
      TriggerAndRcAddr *TMsg = (TriggerAndRcAddr *) TriggerMsg;
      unsigned SrcProcess = TMsg->src;
      unsigned Len        = TMsg->len;
#endif

      // Create the object in this locality, storing the pointer into the
      // ReturnStoreMsg payload area.
      ((TriggerAndRcAddr *) TriggerMsg)->ReturnStoreMsg.PayLoad.Locality = Platform::Topology::GetAddressSpaceId();
      ((TriggerAndRcAddr *) TriggerMsg)->ReturnStoreMsg.PayLoad.Address =
         new ClassOfObject( *( (ClassOfConstructorParms *) ( ((TriggerAndRcAddr *)TriggerMsg)->ConstructorParms ) ));

      BegLogLine( PKFXLOG_PKRNEW )
        << " NewBackEnd(): RemoteObject constructed at "
        << "0x" << hex << setw(8)
        << ((TriggerAndRcAddr *) TriggerMsg)->ReturnStoreMsg.PayLoad.Address
        << EndLogLine;

      // Using the part of the Trigger message that is a RemoteObject<Type>::StoreMsg,
      // return information about the object just created.

      RemoteObject< RemoteObjectHandle<ClassOfObject> >::Store( SrcProcess,
                             &( ((TriggerAndRcAddr *) TriggerMsg)->ReturnStoreMsg) );

      return(0);
      }

    ClassOfObject *
    Instantiate( int TaskNo, ClassOfConstructorParms *ConstructorParms )
      {
      TriggerAndRcAddr  Msg;

      memcpy( Msg.ConstructorParms,
             (char *) ConstructorParms,
             sizeof( ClassOfConstructorParms ));

      // Partially fill out the return store object message.
      Msg.ReturnStoreMsg.TargetAddress     = &ObjectHandle;

      Msg.ReturnStoreMsg.PayLoad.Locality = Platform::Topology::GetAddressSpaceId();

      ObjectHandle.Locality = 0;
      ObjectHandle.Address  = 0;

      Platform::Reactor::Trigger( TaskNo,
               NewBackEnd,
               (void *) &Msg,
               sizeof(   Msg  ) );

#if _THREAD_SAFE
      // When threaded, wait.
      //A98: while( ObjectHandle.Address == 0 ) pthread_yield();
      while( ObjectHandle.Address == 0 )/// sched_yield();
        Platform::Thread::Yield();
#else
      // When not threaded, it better be done!!!.
      assert( ObjectHandle.Address != 0 );
#endif

      BegLogLine( PKFXLOG_PKRNEW )
        << " Instantiate() : Target Task "
        << TaskNo
        << " Address of created object "
        << "0x" << hex << setw(8)
        << ObjectHandle.Address
        << EndLogLine;

      return( (ClassOfObject *) ObjectHandle.Address );
      }
  };

#endif
