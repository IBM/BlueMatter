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
 * Module:          spi_reactor.cpp
 *
 * Purpose:         Implementation of 'reactor' for BG/L torus on its SPI
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         20020625 tjcw Created.
 *
 ***************************************************************************/

#ifndef PKTRACE_REACTOR
#define PKTRACE_REACTOR (1)
#endif

#ifndef PKTRACE_REACTOR_POLL
#define PKTRACE_REACTOR_POLL (0)
#endif

#include <pk/platform.hpp>

// #include <standalone.h>
#include <bgltoruspacket.h>

#include "buffer.cpp"

class BGLTorus
{
   public:
   static BGLTorusDevice torus ;
} ;

BGLTorusDevice BGLTorus::torus ;

static void TraceTorusStatus(const _BGL_TorusFifoStatus& Status)
{
  BegLogLine(PKTRACE_REACTOR)
  << "TraceTorusStatus r0=" << (unsigned int)Status.r0
  << " r1=" << (unsigned int)Status.r1
  << " r2=" << (unsigned int)Status.r2
  << " r3=" << (unsigned int)Status.r3
  << " r4=" << (unsigned int)Status.r4
  << " r5=" << (unsigned int)Status.r5
  << " rH=" << (unsigned int)Status.rH
  << " i0=" << (unsigned int)Status.i0
  << " i1=" << (unsigned int)Status.i1
  << " i2=" << (unsigned int)Status.i2
  << " iH=" << (unsigned int)Status.iH
  << EndLogLine ;
}
#if 0
static void ShowTorusStatus(void) 
{
  _BGL_TorusFifoStatus Status0 ;
  _BGL_TorusFifoStatus Status1 ;
  
  BGLTorusGetStatus0(&Status0) ;
  BGLTorusGetStatus1(&Status1) ;
  
  BegLogLine(PKTRACE_REACTOR)
    << "ShowTorusStatus"
      << EndLogLine ;

  TraceTorusStatus(Status0) ;
  TraceTorusStatus(Status1) ;
  
  BegLogLine(PKTRACE_REACTOR)
    << "ShowTorusStatus return"
      << EndLogLine ;
  
  
}
#endif
static inline void ShowTorusStatus(void) { } 

void
ReactorInitialize()
  {
 ////// ReactorTableNext = ReactorTable;     //Point index to start of table.
  BegLogLine( PKFXLOG_PKREACTOR )
    << "PkActiveMsgSubSys: constructed"
    << EndLogLine;
  for( int bpi = 0; bpi < FREE_BUFFER_POOL_SIZE; bpi++ )
    FreeBufferPool[bpi] = NULL;
  FreeBufferPoolCount    = 0;                   //Initialise the buffer pool
  FreeBufferPoolLockWord = 0;

  // Torus initialisation
// 20050218 tjcw no hardware initialisation; assume blrts does it before 'main'
//  BGLTorus_Initialize (0, 0, 0, 1, 1, 1);       // Set up the hardware
//
  BGLTorusDevice_Init (&BGLTorus::torus, BGLTorusDevice_All, (void *)0 ) ;  // Allocate all FIFOs to this handle
  }

//-----------------------------------------------------------------------------

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
  BegLogLine(PKTRACE_REACTOR)
    << "Platform::Reactor::Trigger MsgLen="
    << MsgLen
    << "Msg=0x" << Msg
    << " Platform::Reactor::PAYLOAD_SIZE="
    << Platform::Reactor::PAYLOAD_SIZE
    << EndLogLine ;

#if defined(PK_ONE_CORE)
  (void) Drain() ;
#endif  
  assert( MsgLen <= Platform::Reactor::PAYLOAD_SIZE );                   //Data must fit in the datagram.
  assert ( 0 == (unsigned int) Msg % BGL_QUAD_ALIGNSIZE ) ;              //Data must be quadword-aligned.

  BGLTorusPacketHeader PacketHeader ;

  BGLTorusPacketHeader_Init(&PacketHeader
                          , 0,0,0              // We have a 1x1x1 torus at present
                          , FxPtr
                          , 0
                           ) ;

  // 0-16 bytes: put '0' in packet header
  // 17-48 bytes: put '1' in packet header
  // and so on
  PacketHeader.hh.size = (MsgLen+(BGL_QUAD_ALIGNSIZE-1)) / BGL_CHUNK_ALIGNSIZE ;

//  // Put a hint bit on, so the torus does something with the self-addressed packet
//  PacketHeader.hh.hintXPlus = 1 ; 

  PacketHeader.sh.fcn = (BGLTorusPacketHandler) FxPtr ;

  BGLDeviceStatus SendResult =
          BGLTorusDevice_sendH (
              &BGLTorus::torus
            , &PacketHeader
            , (BGLQuad *) Msg
            ) ;
  ShowTorusStatus() ;            

  // To start with, we expect the hardware to take the packet correctly
  assert(BGLDeviceStatus_OK == SendResult) ;

  BegLogLine(PKTRACE_REACTOR)
        << "Platform::Reactor::Trigger message sent"
        << EndLogLine ;

  return(0);
}


static int ReactorPoll(void)
{
   BegLogLine(PKTRACE_REACTOR_POLL)
        << "ReactorPoll entry"
        << EndLogLine ;
   BGLTorusPacket ReceivePacket ;
   BGLDeviceStatus ReceiveResult = BGLTorusDevice_recv(&BGLTorus::torus, &ReceivePacket) ;

   BegLogLine(PKTRACE_REACTOR_POLL)
        << " BGLTorusDevice_recv rc=" << ReceiveResult
        << EndLogLine ;
   if (BGLDeviceStatus_OK == ReceiveResult)
   {

     // With the current interface, need to copy the hardware packet in case
     // the software wants to keep it.

     BufferPtr Buf = (char *) Platform::Reactor::GetBuffer();  //Get a buffer from the pool.
     unsigned int ChunkCount = ReceivePacket.header.hh.size ;     // 0 means 16 bytes of data, 1 means 48, ...
     unsigned int ByteCount = ChunkCount * BGL_CHUNK_ALIGNSIZE + BGL_QUAD_ALIGNSIZE ;
     memcpy( Buf, ReceivePacket.data, ByteCount );                                  //Put the data in the buffer.

     Platform::Reactor::FunctionPointerType  FxPtr = (Platform::Reactor::FunctionPointerType)ReceivePacket.header.sh.fcn ;

     int ReactorRc = (FxPtr)( Buf );      //Call the function with buffered data.

     BegLogLine(PKTRACE_REACTOR)
           << "Platform::Reactor::Trigger returned from function with code "
           << ReactorRc
           << EndLogLine ;

     switch( ReactorRc )
       {
       case 0 :
         // Actor is finished with buffer - too bad we didn't know ... now we have to delete.
         Platform::Reactor::FreeBuffer( Buf );     //Free up buffer if the triggered function
         break;                                                // is completed.
       case 1 :
         // Actor has kept the buffer.                         //If not don't.
         break;
       default:
         assert( 0 );
       };

   } /* endif */

   BegLogLine(PKTRACE_REACTOR_POLL)
        << "ReactorPoll return"
        << EndLogLine ;
   return ReceiveResult ;        
}

int Platform::Reactor::Poll(void)
{
   BegLogLine(PKTRACE_REACTOR_POLL)
        << "Platform::Reactor::Poll entry"
        << EndLogLine ;
   ShowTorusStatus() ;
   int rc = ReactorPoll() ;
   BegLogLine(PKTRACE_REACTOR_POLL)
        << "Platform::Reactor::Poll return "  << rc 
        << EndLogLine ;
  return rc ;
}

int Platform::Reactor::Drain(void)
{
   BegLogLine(PKTRACE_REACTOR_POLL)
        << "Platform::Reactor::Drain entry"
        << EndLogLine ;
  int rc = ReactorPoll() ;
  while ( BGLDeviceStatus_OK == rc )
  {
    rc = ReactorPoll() ;
  }
   BegLogLine(PKTRACE_REACTOR_POLL)
        << "Platform::Reactor::Drain return"
        << EndLogLine ;
  return rc ;

}


#if 0
// Don't think it can be done this way until BGLTorusPacketHandler definition is clarified
void ReactorPacketHandler(
    struct _BGLTorusDevice_tag * TorusDevice
  , struct _BGLTorusPacket_tag * TorusPacket
) {

  // With the current interface, need to copy the hardware packet in case
  // the software wants to keep it.

  BufferPtr Buf = (char *) GetBuffer();  //Get a buffer from the pool.
  unsigned int ChunkCount = TorusPacket -> hh.size ;     // 0 means 16 bytes of data, 1 means 48, ...
  unsigned int ByteCount = ChunkCount * BGL_CHUNK_ALIGNSIZE + BGL_QUAD_ALIGNSIZE ;
  memcpy( Buf, TorusPacket -> data, ByteCount );                                  //Put the data in the buffer.


  BegLogLine(1)
        << "Platform::Reactor::Trigger Calling "
        << (void*) FxPtr
        << " with buffer "
        << (void*) Buf
        << EndLogLine ;
  int ReactorRc = (FxPtr)( Buf );      //Call the function with buffered data.

  BegLogLine(1)
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
  }

void ReactorPoll(void)
{
  BegLogLine(1)
        << "ReactorPoll Calling BGLTorusDevice_poll"
        << EndLogLine ;

   BGLDeviceStatus PollResult =
          BGLTorusDevice_poll  (
                &BGLTorus::torus
               , ReactorPacketHandler
               , 1) ;

  BegLogLine(1)
        << "ReactorPoll back from BGLTorusDevice_poll, rc="
        << PollResult
        << EndLogLine ;

   assert(BGLDeviceStatus_OK == PollResult || BGLDeviceStatus_E_EMPTY == PollResult) ;
}

#endif
