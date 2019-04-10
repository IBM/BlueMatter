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
 * Module:          pthread_aix4.cpp
 *
 * Purpose:         Wrap physical thread context (pthread) in C++ class.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         041196 BGF Created.
 *                  270797 RSG created .cpp to hold implementation
 *
 ***************************************************************************/
//#include <pthread.h> //moved to header
#include <pk/pthread_aix4.hpp>
#include <sched.h>
#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_PHYSICAL_THREAD
#define PKFXLOG_PHYSICAL_THREAD 0
#endif

#if 0 // TO BE ADDED WITH REST OF MACHINE INDEPENDANCE LAYER
#include "threaditerator.hpp"
#endif

#include <pk/exception.hpp>

#include <iostream.h>
#include <stdlib.h>
#include <unistd.h>

class MainThreadContext : public PhysicalThreadContext
  {
  private:
  public:
    MainThreadContext();
    void* PhysicalThreadMain(void*);
  };

MainThreadContext::
MainThreadContext()
  {}

void*
MainThreadContext::
PhysicalThreadMain(void* arg)
  {
  return arg;
  }

struct PhysicalThreadContext_i
  {
  // data
  pthread_t d_tid;
  pthread_attr_t d_attr;
  int d_ordinal;
  void* d_arg;
  // methods
  ~PhysicalThreadContext_i()
    {
    pthread_attr_destroy(&d_attr);
    }
  };

PhysicalThreadContext::PhysicalThreadContext()
  {
  d_this = new PhysicalThreadContext_i;
  assert(d_this != NULL);
  d_this->d_tid = (pthread_t) NULL;
  }

PhysicalThreadContext::
~PhysicalThreadContext()
  {
  delete d_this;
  }

void
PhysicalThreadContext::
PhysicalThreadYield()
  {
  //pthread_yield();
  sched_yield();
  }

int
PhysicalThreadContext::
Join( PhysicalThreadContext* ptc,
      void**                 Status)
  {
  BegLogLine( PKFXLOG_PHYSICAL_THREAD )
    << "PhysicalThreadContext::Join() Begin : "
    << ptc
    << EndLogLine;

  pthread_join( ptc->d_this->d_tid, Status);

  BegLogLine( PKFXLOG_PHYSICAL_THREAD )
    << "PhysicalThreadContext::Join() End : "
    << ptc
    << EndLogLine;
  return(0);
  }

void
PhysicalThreadContext::
exit( unsigned rc )
  {
  BegLogLine( PKFXLOG_PHYSICAL_THREAD )
    << "PhysicalThreadContext::exit() Rc : "
    << rc
    << EndLogLine;
  pthread_exit( (void *) rc );
  }

void
PhysicalThreadContext::
sleep(int seconds)
  {
  // call the system sleep function
  ::sleep(seconds);
  }

void
PhysicalThreadContext::
usleep(int microseconds)
  {
  // call the system usleep function
  ::usleep(microseconds);
  }


void* PhysicalThreadContext::PhysicalThreadWrapper( void* v_ptc )
  {
  void *v_rc;
  PhysicalThreadContext* ptc;
  ptc = (PhysicalThreadContext*)v_ptc;


  BegLogLine( PKFXLOG_PHYSICAL_THREAD )
    << "PhysicalThreadWrapper: Thread starting."
    << " Arg "
    << hex
    << ptc->d_this->d_arg
    << EndLogLine;

  try
    {
      PkASSERT( ptc != NULL );

      int rc = PhysicalThreadManager::GetInterface()->registerThread(ptc);
      PkASSERT(rc == 0);

      PhysicalThreadIdentifier newID;
      BegLogLine( PKFXLOG_PHYSICAL_THREAD )
        << "PhysicalThreadWrapper: ThreadIdentifier "
        << newID << " about to call PhysicalThreadMain"
        << EndLogLine;

      v_rc = ptc->PhysicalThreadMain( ptc->d_this->d_arg );

      BegLogLine( PKFXLOG_PHYSICAL_THREAD )
        << "PhysicalThreadWrapper: Thread ending."
        << " Rc "
        << v_rc
        << EndLogLine;
    }
  catch(PkException& gfe)
    {
      PhysicalThreadIdentifier newID;

      BegLogLine( PKFXLOG_PHYSICAL_THREAD )
        << "PhysicalThreadWrapper: Exception Caught for ThreadIdentifier "
        << newID << "\n"
        << gfe
        << EndLogLine;


        cerr << "PhysicalThreadWrapper: Exception Caught for ThreadIdentifier "
             << newID << "\n"
             << gfe
             << endl;

        PkRETHROW(gfe);
    }

  return(v_rc);
  }

struct PhysicalThreadManager_i
  {
  // classes
  class Lock
    {
    private:
      // data
      pthread_mutex_t& d_mutex;
    public:
      inline Lock(pthread_mutex_t& mutex) : d_mutex(mutex)
        {
        pthread_mutex_lock(&d_mutex);
        }
      inline ~Lock()
        {
        pthread_mutex_unlock(&d_mutex);
        }
    };

  // enums
  enum {MAX_PHYSICAL_THREADS = 512};

  // data
  static PhysicalThreadManager* s_instance;
  PhysicalThreadContext*        d_context[MAX_PHYSICAL_THREADS];
  int                           d_nextThreadNumber;
  pthread_mutex_t               d_registerMutex; // monitor lock for register
  pthread_key_t                 d_currentThreadContext;

  // methods
  PhysicalThreadManager_i() : d_nextThreadNumber(0)
    {
    pthread_mutex_init(&d_registerMutex, NULL);
    int rc = pthread_key_create(&d_currentThreadContext, NULL);
    assert(rc == 0);
    }

  ~PhysicalThreadManager_i()
    {
    pthread_mutex_destroy(&d_registerMutex);
    pthread_key_delete(d_currentThreadContext);
    }
  };

PhysicalThreadManager* PhysicalThreadManager_i:: s_instance = NULL;

PhysicalThreadManager::
PhysicalThreadManager()
  {
  // we are assuming that this is only called from the main thread
  d_this = new PhysicalThreadManager_i;
  assert(d_this != NULL);

  PhysicalThreadContext* mainContext = new MainThreadContext;
  assert(mainContext != NULL);

  registerThread(mainContext);
  }

PhysicalThreadManager::
~PhysicalThreadManager()
  {
  delete d_this->d_context[0]; // delete the main context
  d_this->d_context[0] = NULL;
  delete d_this;
  }

PhysicalThreadManager*   // return type
PhysicalThreadManager::  // scope
GetInterface()           // method name
                         // inheritance
  {
  // this next bit should be made thread-safe through use of pthread_once
  if( PhysicalThreadManager_i::s_instance == NULL )
    {
    PhysicalThreadManager_i::s_instance = new PhysicalThreadManager();
    }
  return( PhysicalThreadManager_i::s_instance );
  }

int
PhysicalThreadManager::
registerThread(PhysicalThreadContext* newContext)
  {
  // acquire the monitor lock
  PhysicalThreadManager_i::Lock myLock(d_this->d_registerMutex);

  // get the thread ordinal for this new thread
  assert(d_this->d_nextThreadNumber <
         PhysicalThreadManager_i::MAX_PHYSICAL_THREADS);
  newContext->d_this->d_ordinal = d_this->d_nextThreadNumber;
  d_this->d_context[d_this->d_nextThreadNumber] = newContext;
  ++d_this->d_nextThreadNumber;

  // set the thread-specific memory to point at the thread context
  int rc = pthread_setspecific(d_this->d_currentThreadContext,
                               newContext);
  return (rc);
  }

int
PhysicalThreadManager::
Start( PhysicalThreadContext* ptc,
       void*                  arg )
  {
  int rc;
  BegLogLine( PKFXLOG_PHYSICAL_THREAD )
    << "PhysicalThreadManager::Start() Before pthread_create() : "
    << ptc
    << " arg "
    << arg
    << EndLogLine;

  assert( ptc != NULL );

  ptc->d_this->d_arg = arg;
  pthread_attr_init( &(ptc->d_this->d_attr) );

  // NEED: to code this system to automaticly reap zombie threads -
  // for now, create detached.
  pthread_attr_setdetachstate( &(ptc->d_this->d_attr),
                               PTHREAD_CREATE_DETACHED);

#ifdef SET_PTHREAD_STACK_MULTIPLIER
  // NEED: This should be something that can be done to the ptc before starting.
  pthread_attr_setstacksize( &(ptc->d_this->d_attr),
       SET_PTHREAD_STACK_MULTIPLIER * PTHREAD_STACK_MIN );
#endif
  /* Chris Ward wants to be able to copy a Hursley feature database index
   * onto stack. This arranges that enough space is allocated.
   * It may be appropriate to back this out when Bob's database comes in.
   */
  pthread_attr_setstacksize(
    &(ptc->d_this->d_attr),
    (16001*16) + 65536
  ) ;


  rc = pthread_create( &(ptc->d_this->d_tid),
                       &(ptc->d_this->d_attr),
                       PhysicalThreadContext::PhysicalThreadWrapper,
                       (void *) ptc);

  BegLogLine( PKFXLOG_PHYSICAL_THREAD )
    << "PhysicalThreadManager::Start() After pthread_create() : "
    << " Rc "
    << rc
    << EndLogLine;

  return(0);
  }

const
PhysicalThreadContext*
PhysicalThreadManager::
getCurrentContext() const
  {
  return (PhysicalThreadContext*)(pthread_getspecific(d_this->d_currentThreadContext));
  }

#if 0 // TO BE ADDED WITH REST OF MACHINE INDEPENDANCE LAYER
int PhysicalThreadManager::allThreadsDo(ThreadIterator& iterator)
{
  PhysicalThreadManager_i::Lock myLock(d_this->d_registerMutex);
  int success = 1;

  // all except main thread
  for (int i = 1; i < d_this->d_nextThreadNumber; i++)
    {
      success = iterator.applyTo(d_this->d_context[i]);
        if (!success)
          {
            break;
          }
    }
  return success;
}
#endif

int PhysicalThreadIdentifier::asInt() const
{
  return d_context->d_this->d_ordinal;
}

ostream& operator<<(ostream& os, const PhysicalThreadIdentifier& id)
{
  os << id.asInt();
  return os;
}
