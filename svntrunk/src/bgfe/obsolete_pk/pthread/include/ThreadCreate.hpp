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
 #ifndef _THREAD_CREATE_HPP_
#define _THREAD_CREATE_HPP_

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_PKTHREADCREATE
#define PKFXLOG_PKTHREADCREATE ( 0 )
#endif

typedef void * (*T_ThreadFxPtr)( void* );

// Flags will allow sync start, best effort,

//THIS CLASS NEEDS TO HAVE A TEMPLATE WRAPPER TO PREVENT PEOPLE FROM DOING
//EVIL THINGS BY PASSING A VALUE AS AN THREAD ARG ADDRESS

class ThreadCreate
  {
  public:
    int Status;

    struct ThreadCreateBackEndFx_Packet
      {
      struct T_Header
        {
        //NEED: to add variables to hold the other args for Platform::Thread::Create()
        int           ArgSize;
        unsigned      Source;  // For DEBUGGING... can be taken out... probably shouldn't be though.
        T_ThreadFxPtr ThreadFxPtr;
        } Header;
#if defined(CYCLOPS)
        double Arg [ (Platform::Reactor::PAYLOAD_SIZE - sizeof( T_Header ) - 4 + sizeof(double)-1) / sizeof(double) ] ;
#else
      char  Arg[ Platform::Reactor::PAYLOAD_SIZE - sizeof( T_Header ) - 4  ]; // NEED: subtracted 4 cause of compiler padding.
#endif
      };

    enum{ PAYLOAD_SIZE = ( Platform::Reactor::PAYLOAD_SIZE - sizeof( ThreadCreateBackEndFx_Packet::Header ) - 4 ) };


    static void *
    RemoteThreadStart( void * new_packet )
      {
      BegLogLine( PKFXLOG_PKTHREADCREATE )
        << " RemoteThreadStart() Running "
        << EndLogLine;
      //THIS IS THE NEW THREAD RUNNING
      //This wrapper is here to be able to delete the copy of the
      //args we made.  The actor function below also used the args to start this thread.

      ThreadCreateBackEndFx_Packet *satp = (ThreadCreateBackEndFx_Packet *) new_packet;

      unsigned src = satp->Header.Source;
      BegLogLine( PKFXLOG_PKTHREADCREATE ) << "RemoteThreadStart: Thread running, Source " << src
        << " ThreadFxPtr " << satp->Header.ThreadFxPtr
        << " Arg " << satp->Arg
        << EndLogLine;

      //Here, finally call the user level thread function.
      void *rc = (*(satp->Header.ThreadFxPtr))( satp->Arg );

      delete( (char *) new_packet ); // cast back to char becase of how this memory was new'd below.

      //NEED: to mark the context free in other ways... though we don't start and stop threads right now

      return( rc );
      };

     // This is the active message handler

    static int
    ThreadCreateBackEndFx( void *packet )
      {
      //THIS IS AN ACTIVE PACKET HANDLER
      BegLogLine( PKFXLOG_PKTHREADCREATE ) << "ThreadCreateBackEndFx " << EndLogLine;

      ThreadCreateBackEndFx_Packet *satp = (ThreadCreateBackEndFx_Packet *) packet;

      unsigned src = satp->Header.Source;
      BegLogLine( PKFXLOG_PKTHREADCREATE ) << "ThreadCreateBackEndFx: Source " << src << EndLogLine;

      // Make a copy of the incoming packet... alternitively, could detatch the reactor packet?
      char* tempPtr;
      pkNew( &tempPtr, sizeof( ThreadCreateBackEndFx_Packet ) );
      
      ThreadCreateBackEndFx_Packet *new_packet = (ThreadCreateBackEndFx_Packet *) tempPtr;
      // new char[ sizeof( ThreadCreateBackEndFx_Packet ) ];

      if( new_packet == NULL )
        PLATFORM_ABORT(" Failed to get backing store for arguement" );

      memcpy( new_packet, packet, sizeof(  ThreadCreateBackEndFx_Packet ) ); // NEED: this might fetch from beyond packet

      //NEED: to have the rest of the args from Platform::Thread::Create here...
      //Start the wrapper function above which will call the user thread fx, then delete new_packet.

      Platform::Thread::Create( RemoteThreadStart, (void *) new_packet );

      //Return to active message handler - currently not keeping packet.
      BegLogLine( PKFXLOG_PKTHREADCREATE )
         << "ThreadCreateBackEndFx: Source "
         << src
         << " Returning "
         << EndLogLine
      return(0);
      }

//     ThreadCreate( Platform::Topology::Group1 *GroupIFPtr,
//                   T_ThreadFxPtr  ThreadFx,         // What function to run
//                   int            CMsgLen,          // Length of argument data
//                   void          *CMsg,             // Argument data pointer
//                   int            ProcessorAssignment = -1, // Bind to a processor: -1
//                   int            StackSize = -1,           // Stack size in bytes: -1 is default
//                   int            Flags  = 0     )  // Operation flags - eg: sync.
//       {
//       // NOT IMPLEMENTED YET - BUT DON'T PUSH ME, IT AIN'T THAT HARD.
//       PLATFORM_ABORT(" ThreadCreate for groups not implemented ");
//       }

///NEED NEED : TO ADD THE PROCESSOR AFFINITY ARG.


    ThreadCreate( int            AddressSpaceId, // Where to start (-1 means start here)
                  T_ThreadFxPtr  ThreadFx,       // What function to run
                  int            CMsgLen,         // Length of argument data
                  void          *CMsg,            // Argument data pointer

                  int            ProcessorAssignment = -1, // Bind to a processor: -1
                  int            StackSize = -1,           // Stack size in bytes: -1 is default
                  int            Flags  = 0     )// Operation flags - eg: sync.
      {
      //THIS IS CALLED BY THE CLIENT AN RUNS ON THE CALLER'S STACK
      int LocalAddressSpaceFlag = 0;

      // Checkif AddressSpaceId is a flag for local. (-1)
      // For static creates, -1 must be specified so we don't touch
      // active message comms before they are setup.  This is all good
      // THIS IS ACTUALLY NO GOOD because we can't stop app threads from
      // using comms - we'll need to book keep static thread starts
      // and do them after active message subsystem comes up.

      if( AddressSpaceId == -1 )    // Local
        LocalAddressSpaceFlag = 1;
      else if( AddressSpaceId == Platform::Topology::GetAddressSpaceId() )
        LocalAddressSpaceFlag = 1;  // Local
      else
        LocalAddressSpaceFlag = 0;  // Not local


      ThreadCreateBackEndFx_Packet satp;
      satp.Header.ArgSize          = CMsgLen;
      satp.Header.ThreadFxPtr      = ThreadFx;
      satp.Header.Source           = Platform::Topology::GetAddressSpaceId();


      if( CMsg != NULL )
        {
        if( CMsgLen <= 0 )
          PLATFORM_ABORT( "CMsg != NULL but CMsgLen <= 0 ");
        if( CMsgLen > PAYLOAD_SIZE )  // arg doesn't fit - use globjet, send handle
          PLATFORM_ABORT( "CMsgLen > PAYLOAD_SIZE ");
        memcpy( satp.Arg, CMsg, CMsgLen );
        }


      // Branch around Active Message subsystem for local stuff.
      if( LocalAddressSpaceFlag )
        {
        // NEED: to assert that total packet size is not bigger than a network packet.
        int Rc = ThreadCreateBackEndFx((void *) &satp);
        }
      else
        {
        Platform::Reactor::Trigger( AddressSpaceId,
                                    ThreadCreateBackEndFx,
                                    (void *) &satp,
                                    sizeof( ThreadCreateBackEndFx_Packet ) );
        // should wait for sycronous starts here
        }
      Status = 0;
      }
  };

#endif
