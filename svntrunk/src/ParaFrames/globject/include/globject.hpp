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
 * Module:          globject.hpp
 *
 * Purpose:         Provide a global objects with a "pull" mechanism.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/
/*
   Overview

   The PK "globject" or global object system provides a simple method to
   make flat memory structures available throughout the PK program scope.

   The requirement for globject arises from the fact that the main communications
   within the PK system is built using single-reader/single-writer queues
   of fixed sized items.  In order to add variable sized objects to
   the queue item structure, we need a fixed size reference (which is
   queued) to a object instance (which is not queued).  Obviously the
   globject system can be used in other ways, for example globjects can
   contain other globject references.

   The globject system depends upon PK active messages for communication
   of requests.  It also makes uses of a long message protocol, for example
   MPI point-to-point messaging, during the return phase.

   The globject system is constructed in two layers.  The upper layer is
   a C++ template class providing an user interface.  Internally, a
   type free layer is used.

   Global objects are enrolled, expelled, and fetched using the
   C++ template typed interface class.  For each C++ class to be manipulated
   using the globject system, a declaration is required.  A macro is provided
   to the globject user which encapsulates this declaration requiring the
   user of the globject system to provide only the C++ class type to be handled.
   The macro declares a static global.  This global is a singleton pointer
   to a type free globject structure - a "GCM_TypeFreeBase".

   The GlobjectReference returned when an object is enrolled is valid
   anywhere in the scope of PK.  Furthermore, an equality operator exists
   between global objects.  The allows for the globject system user to
   do caching where needed.  To do this, the user makes sure to send a
   new GlobjectReference when it is the intention to cause a refresh of
   the cached copy of the object.  In the case where the origional object
   is modified but remains physically in place, the old GlobjectReference
   should be expelled and and the object re-enrolled.  The user must
   be very careful to manage the synchronization issues involved.

   During coding, the user of the globject system will code all enrolls,
   expels, and fetches as static methods on a templatized GCM class.
   For example:
     UserType *aUserTypeInstancePtr = new UserType(x,y,z);
     GlobjectReference UserTypeGloRef =
                     GCM<UserType>::Enroll( aUserTypeInstancePtr );
   would construct a new object instance of class UserType and enroll that
   instance in the globject system.  The returned GlobjectReference is a
   handle that can be used to fetch a copy of this object from any where
   within the scope of PK.

   The following:
     UserType aUserType = GCM<UserType>::Fetch( GloRef );
   will copy the enrolled instance of UserType into a new instance aUserType.

   Lastly, when the program is finished with the enrolled instance,
   it should expel the instance as follows:
     GCM<UserType>::Expel( GloRef );
   This has the effect of invalidating the GlobjectReference and thus,
   the user must make sure not to fetch that reference in the future.

   The global object system is currently stateless.  Synchronization is also
   left to the completely to the user although violations will sometimes
   be caught.

   Internally, each static method called by the user on the GCM<>
   class recovers the type of object being handled.  This type is effectively
   expressed by the address of the singleton pointer of the type-free area.
   The C++ templates enable compile time checking of Enrolls and Expels - the
   signatures of these methods mean they can't be called with the wrong type.
   The use of the address of the singleton point means that a fetch (possibly
   from one address space to another) can be run time type checked when a
   flag is set causing the system to used typed GlobjectReferences.



 */
#ifndef _INCLUDED_GLOBJECT_HPP_
#define _INCLUDED_GLOBJECT_HPP_

// This should eventually come out to be defined by
// the user of the system.
#define RUNTIME_TYPE_CHECKING

////#include <pk/platform.hpp>
////#include <pk/fxlogger.hpp>
#include <Pk/API.hpp>

#ifndef PKFXLOG_GLOBJECT
#define PKFXLOG_GLOBJECT (0)
#endif

// This and the following MPI dependant code provides a short circuit
// when MPI is not being used.  This ought to be built into the MpiSubSys
// stuff.  OR: Check out what POK is doing with c++ MPI.

// We would like the reference to fit in one word.

class GlobjectReference
  {
  public:
    unsigned HomeMemory     : 8 ; // 2**HomeMemory = MAX memory spaces
    unsigned TableKey       : 24; // 2 ** TableKey =
                                  //    MAX  objects per class per memory space.
#ifdef RUNTIME_TYPE_CHECKING
    unsigned                TypeNumber; // this could be a pointer to the GCM
                                        // but we don't want to become dependant
                                        // on that information.
#if 0 // can't have a constructor AND use this in unions.
    GlobjectReference()
      {
      Invalidate();
      }
#endif

    int IsInvalid()
      {
      GlobjectReference x;
      return( *this == x );
      }

#endif
    bool operator==( const GlobjectReference &Other )
      {
      int iseq = 1;
#ifdef RUNTIME_TYPE_CHECKING
      iseq = TypeNumber == Other.TypeNumber;
#endif
      iseq = iseq && (HomeMemory == Other.HomeMemory) && (TableKey == Other.TableKey );

      return( iseq );
      }

    bool operator!=( const GlobjectReference &Other )
      {
      return( ! ( *this == Other ) );
      }

    void
    Invalidate()
      {
      for( int i = 0; i < sizeof( GlobjectReference ); i++ )
        ((char *)this)[i] = 0xFF;
      }
  };

class TableElement;

class GCM_TypeFreeBase
  {
  public:
    GCM_TypeFreeBase();
  private:
    unsigned      ClassTypeNumber;
    unsigned      ClassObjectSize;

    unsigned      TableSize;
    unsigned      TableNextFreeIndex;
    TableElement *Table;


    // NOTE: note static - will be 'newed' with the create of the typefree base.
//?    Platform::Lock::LockWord TableAccessLock ; // Linker will put this in zero-filled memory

    void
    Lock()
      {
//?      Platform::Mutex::YieldLock( &TableAccessLock );
      }

    void
    Unlock()
      {
//?      Platform::Mutex::Unlock( &TableAccessLock );
      }


  public:

    static void
    Create( GCM_TypeFreeBase ** NewSingletonPtr,
            unsigned            PtrToFx_GetFreeBasePtr,
            unsigned            Size )
      {
      // The following static class member is used to serialize
      // access to this creator function these preventing race
      // conditions around SingletonPtr.  Note however that
      // all template globject types serialize on this lock when
      // they create their TypeFreeBase object.

      // static int CreateFxLock = 0;
//?      static Platform::Lock::LockWord CreateFxLock ; // Linker will put this in zero-filled memory

//?      Platform::Mutex::YieldLock( &CreateFxLock );

      if( *NewSingletonPtr == NULL )
        {
        BegLogLine( PKFXLOG_GLOBJECT )
            << " Constructing new Globject GCM "
            << EndLogLine;

        // *NewSingletonPtr = new GCM_TypeFreeBase();
        ////pkNew( NewSingletonPtr, 1 );
        *NewSingletonPtr = (GCM_TypeFreeBase*) PkHeapAllocate( sizeof( GCM_TypeFreeBase ) );

        assert( *NewSingletonPtr != NULL );

        (*NewSingletonPtr)->SetClassTypeNumber( PtrToFx_GetFreeBasePtr);
        (*NewSingletonPtr)->SetClassObjectSize( Size );
        }

//?      Platform::Mutex::Unlock( &CreateFxLock );
      }

    unsigned GetNextId();

    int FreeId( unsigned Id );

    void SetClassTypeNumber( unsigned aClassTypeNumber );

    unsigned GetClassTypeNumber( );

    void SetClassObjectSize( unsigned aClassObjectSize );
    unsigned GetClassObjectSize( );

    GlobjectReference TypeFreeEnroll( void* aGlobject );

    int TypeFreeExpel( GlobjectReference ref );

    void *GetObjectPointer( GlobjectReference ref );

    typedef struct
      {
      unsigned          Source;
      unsigned          Len;
      GlobjectReference ref;
      GCM_TypeFreeBase * (*IFPtrFx)();
      unsigned          ReturnMsgType;
      } TypeFreeFetchTriggerMsg;

    static int
    TypeFreeFetchBackEnd(void *Msg );

    int TypeFreeFetch( GlobjectReference ref, char *ObjBuf );

};

// This template class contains a set of static methods.

// Globalized Class Manager
template<class ObjType>        //This is the user interface and is templatized
class GCM                      //for each user class that uses the Global object system.
  {                            //Each class accesses the internal type free global object
  private:                     //structure through a singleton pointer.
    // It is important to remember that this static function
    // returns a pointer to a GCM but which can also be used
    // as a pointer to a GCM_TypeFreeBase.

    static inline  GCM_TypeFreeBase* GetFreeBasePtr();

    static GCM_TypeFreeBase *SingletonPtr ;

  public:

    // Enroll a user managed memory area in globalizing system.
    // static GlobjectReference Enroll( ObjType* aGlobject );
//A98    //static inline  GlobjectReference GCM<ObjType>::Enroll( ObjType* aGlobject );
    static inline  GlobjectReference Enroll( ObjType* aGlobject );

    static inline  int Expel( GlobjectReference ref );

#if MAYBE
    // Import an object either for persistant or volatile existance.
    static inline  int Import( ObjectType *aGlobject, StorageClass ) { }
#endif

    // Get a copy of this object - storage class unknown - but ref must be
    // valid at source.
    static inline  ObjType Fetch( GlobjectReference ref );

  };


// ********* This macro needs a better discription **************

#define GLOBJECT_TYPE_DCL(TYPE)              \
template <class TYPE > GCM_TypeFreeBase *GCM< TYPE >::SingletonPtr = NULL  //A98 required NULL def.


//  ********  FUNCTION BODIES FOR TEMPLATE METHODS *****************
template<class ObjType>
GCM_TypeFreeBase*       // Return
GCM<ObjType>::          // Class
GetFreeBasePtr()        // Method
  {
  if( SingletonPtr == NULL )
    {
    GCM_TypeFreeBase::Create( & SingletonPtr,
                              (unsigned) GCM<ObjType>::GetFreeBasePtr,
                              sizeof( ObjType ) );
    assert( SingletonPtr != NULL );
    }
  return( SingletonPtr );
  }

template <class ObjType>
GlobjectReference            // Return
GCM<ObjType>::               // Class
Enroll( ObjType* aGlobject ) // Method
  {
  GlobjectReference ref = GCM<ObjType>::GetFreeBasePtr()->
                                           TypeFreeEnroll( (void *) aGlobject );
  return( ref );
  }

template <class ObjType>
int                              // Return
GCM<ObjType>::                   // Class
Expel( GlobjectReference ref )  // Method
  {
  int rc = GCM<ObjType>::GetFreeBasePtr()->TypeFreeExpel( ref );
  return( rc );
  }

// Get a copy of this object - storage class unknown - but
// ref must be valid at source.
template <class ObjType>
ObjType                           // Return
GCM<ObjType>::                    // Class
Fetch( GlobjectReference ref )    // Method
  {
  char ObjBuf[ sizeof(ObjType) ];

  int rc = GCM<ObjType>::GetFreeBasePtr()->TypeFreeFetch( ref, ObjBuf );
  assert( rc == 0 );

  return( *((ObjType *)ObjBuf) );
  }


#endif
