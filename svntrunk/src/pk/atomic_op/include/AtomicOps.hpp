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
 #ifndef _MEMORY_OPS_HPP_
#define _MEMORY_OPS_HPP_

#ifndef PKFXLOG_MEMORYOPS
#define PKFXLOG_MEMORYOPS (0)
#endif

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

// This should really be "template <class thingy> class AtomicThing { ....
// and then figure out if it is an interger (which might benefit from machine atomicity.)

class AtomicInteger
  {
  private:

    Platform::Memory::AtomicInteger mAtomicInteger;

  public:

  int AtomicAdd( int aIncVal) // could overload operator+() if you want
    {
    int Rc = Platform::Memory::AtomicAdd( mAtomicInteger, aIncVal );
    return( Rc );
    }

  int AtomicSet( int aVal) // could overload operator+() if you want
    {
    int Rc = mAtomicInteger = aVal;  //NEED ATOMIC OP
    return( Rc );
    }

  int SafeFetch()
    {
    int Rc = mAtomicInteger;
    return( Rc );
    }

// BGL bringup doesn't support remote invocation
#if !defined( PK_BLADE )
  class AtomicAdd_ActorPacket
    {
    public:
      int            mSource;       // for DEBUGGING can be taken out
      int            mValueToAdd;
      AtomicInteger* mAtomicInteger;
    };

  static int
  AtomicAdd_ActorFx( void *Data )
    {
    AtomicAdd_ActorPacket *aaap = (AtomicAdd_ActorPacket *)Data;
    BegLogLine(PKFXLOG_MEMORYOPS)
      << "AtomicAdd_ActorFx "
      << " Src " << aaap->mSource
      << " AtomicInt@ 0x" << (void *) &(aaap->mAtomicInteger->mAtomicInteger)
      << " " << aaap->mAtomicInteger->mAtomicInteger
      << " + " << aaap->mValueToAdd
      << EndLogLine;
    aaap->mAtomicInteger->AtomicAdd( aaap->mValueToAdd );
    return(0);
    }

  void  // remote version does not return the new value
  AtomicAdd( int aLogicalNodeId, int aIncVal )
    {
    BegLogLine(PKFXLOG_MEMORYOPS)
      << "AtomicAdd "
      << " Dst " << aLogicalNodeId
      << " AtomicInt@ 0x" << this
      << " " << mAtomicInteger
      << " + " << aIncVal
      << EndLogLine;
    if( aLogicalNodeId == Platform::Topology::GetAddressSpaceId() )
      {
      AtomicAdd( aIncVal );
      }
    else
      {
      AtomicAdd_ActorPacket aaap;
      aaap.mSource = Platform::Topology::GetAddressSpaceId(); // for DEBUGGING
      aaap.mAtomicInteger = this;
      aaap.mValueToAdd    = aIncVal;
      Platform::Reactor::Trigger(  aLogicalNodeId,
                                   AtomicAdd_ActorFx,
                                   (void *) & aaap,
                                   sizeof( aaap ) );
      }
    }
#endif
  };
#endif
