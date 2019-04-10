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
 * Module:          smp_reactor.cpp
 *
 * Purpose:         Methods of .hpp file.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/

#ifndef PKTRACE_REACTOR
#define PKTRACE_REACTOR (0)
#endif

#include <pk/platform.hpp>

#include "buffer.cpp"

void
InitReactorSystem()
  {
 ////// ReactorTableNext = ReactorTable;     //Point index to start of table.
  BegLogLine( PKFXLOG_PKREACTOR )
    << "PkActiveMsgSubSys: constructed"
    << EndLogLine;
  for( int bpi = 0; bpi < FREE_BUFFER_POOL_SIZE; bpi++ )
    FreeBufferPool[bpi] = NULL;
  FreeBufferPoolCount    = 0;                   //Initialise the buffer pool
#if defined(PK_BLADE)
  FreeBufferPoolLockWord.mVar = 0;
#else
  FreeBufferPoolLockWord = 0;
#endif
  }

//-----------------------------------------------------------------------------

// BlueLight bringup doesn't have new/delete memory. Try suppressing active messages.
#if !defined( PK_BLADE )
int
Platform::Reactor::Trigger(
         int                                     TaskNo,  //Target task for triggered function.
         Platform::Reactor::FunctionPointerType  FxPtr,   //Function that is to be triggered.
         void                                   *Msg,     //Data to be sent.
         unsigned                                MsgLen)  //Size of data to be sent.
  {

//PkActiveMsgSubSys::GetIFPtr()->TraceTrigger.HitOE( PKTRACE_REACTOR,
//                                                   ACT,
//                                                   PkActiveMsgSubSys::GetIFPtr()->TraceName,
//                                                   PkActiveMsgSubSys::GetIFPtr()->TracePPid,
//                                                   TraceTrigger);
//
  BegLogLine( PKTRACE_REACTOR )
    << "Platform::Reactor::Trigger MsgLen="
    << MsgLen
    << " Platform::Reactor::PAYLOAD_SIZE="
    << Platform::Reactor::PAYLOAD_SIZE
    << EndLogLine ;

  assert( MsgLen <= Platform::Reactor::PAYLOAD_SIZE );                   //Data must fit in the datagram.

  BufferPtr Buf = (char *) GetBuffer();  //Get a bufferfro the pool.

  memcpy( Buf, Msg, MsgLen );                                  //Put the data in the buffer.

  BegLogLine( PKTRACE_REACTOR ) 
	<< "Platform::Reactor::Trigger Calling "
	<< (void*) FxPtr
	<< " with buffer "
	<< (void*) Buf
	<< EndLogLine ;
  int ReactorRc = (FxPtr)( Buf );      //Call the function with buffered data.

  BegLogLine( PKTRACE_REACTOR ) 
	<< "Platform::Reactor::Trigger returned from function with code "
	<< ReactorRc
	<< EndLogLine ;

  switch( ReactorRc )
    {
    case 0 :
      // Actor is finished with buffer - too bad we didn't know ... now we have to delete.
      FreeBuffer( Buf );     //Free up buffer if the triggered function
      break;                                                // is completed.
    case 1 :
      // Actor has kept the buffer.                         //If not don't.
      break;
    default:
      assert( 0 );
    };

  return(0);
  }
#endif


