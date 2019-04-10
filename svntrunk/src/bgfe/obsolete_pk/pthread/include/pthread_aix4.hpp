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
 * Module:          pthread_aix4.hpp
 *
 * Purpose:         Wrap physical thread context (pthread) in C++ class.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         041196 BGF Created.
 *
 ***************************************************************************/



THIS FILE IS RETIRED



#ifndef INCLUDE_PTHREAD_AIX4_HPP_
#define INCLUDE_PTHREAD_AIX4_HPP_

extern "C" {
#include <pthread.h>
}
#include <stdlib.h>
#include <assert.h>

class PhysicalThreadManager;
class ostream;
class PhysicalThreadIdentifier;

class PhysicalThreadContext_i;

// abstract base class
class PhysicalThreadContext
  {
    friend class PhysicalThreadManager;
    friend class PhysicalThreadIdentifier;
  private:
    // data
    PhysicalThreadContext_i* d_this;
    // methods
    static void* PhysicalThreadWrapper( void* ); // pure virtual
                                                 // providing runnable
  public:
                    PhysicalThreadContext();
    virtual        ~PhysicalThreadContext();
    virtual  void*  PhysicalThreadMain(void*) = 0;
    static   void   PhysicalThreadYield();
    static   int    Join( PhysicalThreadContext*, void**);
    static   void   exit( unsigned );
    static   void   sleep( int ); // sleep interval in seconds
    static   void   usleep( int ); // sleep interval in microseconds
  };

class ThreadIterator;

class PhysicalThreadManager_i;

class PhysicalThreadManager
  {
    friend class PhysicalThreadManager_i;
    friend class PhysicalThreadContext;
  private:
    // data
    PhysicalThreadManager_i* d_this;
    // methods
    PhysicalThreadManager();
    int registerThread(PhysicalThreadContext*); // this method has a monitor lock

  public:
    ~PhysicalThreadManager();
    static PhysicalThreadManager* GetInterface();
    const  PhysicalThreadContext* getCurrentContext() const;
    int    Start( PhysicalThreadContext* ptc, void* arg );
    int    allThreadsDo(ThreadIterator&);
  };


class PhysicalThreadIdentifier
  {
    friend ostream& operator<<(ostream&, const PhysicalThreadIdentifier&);
  private:
    const PhysicalThreadContext* d_context;
  public:
    // the default constructor will be initialized with the identifier
    // of the current thread
    inline PhysicalThreadIdentifier();
    inline PhysicalThreadIdentifier(const PhysicalThreadContext*);
    inline PhysicalThreadIdentifier(const PhysicalThreadIdentifier&);
    inline PhysicalThreadIdentifier& operator=(const PhysicalThreadIdentifier&);
    int asInt() const;
  };

/******************************************************************************
*
******************************************************************************/
PhysicalThreadIdentifier::
PhysicalThreadIdentifier()
  {
  d_context =
    PhysicalThreadManager::GetInterface()->getCurrentContext();
  assert(d_context != NULL);
  }

/******************************************************************************
*
******************************************************************************/
PhysicalThreadIdentifier::
PhysicalThreadIdentifier(const PhysicalThreadContext* ptc
                        ) : d_context(ptc)
  {
  assert(d_context != NULL);
  }

/******************************************************************************
*
******************************************************************************/
PhysicalThreadIdentifier::
PhysicalThreadIdentifier(const PhysicalThreadIdentifier& proto
                        ) : d_context(proto.d_context)
  {
  }

/******************************************************************************
*
******************************************************************************/
PhysicalThreadIdentifier&
PhysicalThreadIdentifier::
operator=(const PhysicalThreadIdentifier& other)
  {
  d_context = other.d_context;
  return *this;
  }

/******************************************************************************
*
******************************************************************************/
ostream& operator<<(ostream&, const PhysicalThreadIdentifier&);

#endif
