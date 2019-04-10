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
 * Module:          AllReduce
 *
 * Purpose:         Active message based theta all reduce (BroadCast based)
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         Sept 20, 2002 BGF Created.
 *
 ***************************************************************************/

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/AtomicOps.hpp>

template< class tElement, class Op >
class ThetaAllReduceBC
  {
  public:

  struct tActorPacket
    {
    struct tHeader
      {
      tElement *       mElementSubArrayPointer;
      unsigned short   mElementCount;            // This could be avoided if all callers have common partition
      unsigned short   mSourceNode;             // This is really for debugging - in case partitions are not correct
      };
    enum {            eMaxElementPayloadCount = ( Platform::Reactor::BROADCAST_PAYLOAD_SIZE - sizeof( tHeader ) ) / sizeof( tElement ) };
    tHeader           mHeader;
    tElement          mElementSubArray[ eMaxElementPayloadCount ];
    };

  static AtomicInteger ReceivedCount;

  static
  int
  ActorFx( void * pkt )
    {
    tActorPacket *ap = (tActorPacket *) pkt;

    BegLogLine(0)
      << "ThetaAllReduceBC::ActorFx(): "
      << " Src "
      << ap->mHeader.mSourceNode
      << " PayloadElementCount "
      << ap->mHeader.mElementCount
      << " Put data "
      << (void *) ap->mHeader.mElementSubArrayPointer
      << EndLogLine;

    if( ap->mHeader.mElementCount == 0 )
      {

      Platform::Memory::ExportFence();

      ReceivedCount.AtomicAdd( 1 );
      BegLogLine(1)
        << "ThetaAllReduceBC::ActorFx(): "
        << "Received Terminal Packet, ReceivedCount now "
        << ReceivedCount.SafeFetch()
        << EndLogLine;
      }
    else
      {
      for( int i = 0; i < ap->mHeader.mElementCount; i++ )
        {
        Op::Execute( ap->mHeader.mElementSubArrayPointer[ i ], ap->mElementSubArray[i] );
        }
      }

    BegLogLine(0)
      << "ThetaAllReduceBC::ActorFx(): "
      << " Done, ReceivedCount "
      << ReceivedCount.SafeFetch()
      << " Src "
      << ap->mHeader.mSourceNode
      << " PayloadElementCount "
      << ap->mHeader.mElementCount
      << " Put data "
      << (void *) ap->mHeader.mElementSubArrayPointer
      << EndLogLine;

    return(0);
    }

  void
  Do( int aCount, tElement aSourceElementArray[], tElement aTargetElementArray[] )
    {
    int MyNode    = Platform::Topology::GetAddressSpaceId();
    int NodeCount = Platform::Topology::GetAddressSpaceCount();

    // prep to keep stats on how many and how full the packets are
    int PacketCount[ 1 + tActorPacket::eMaxElementPayloadCount ];
    for( int i = 0; i <= tActorPacket::eMaxElementPayloadCount ; i++ )
      PacketCount[ i ] = 0;

    BegLogLine(1)
      << "ThetaAllReduceBC::Do(): "
      << " Begin wait for my turn - ReceivedCount is "
      << ReceivedCount.SafeFetch()
      << EndLogLine;

    while( ReceivedCount.SafeFetch() != MyNode )
      {
      Platform::Thread::Yield();
      }

    BegLogLine(1)
      << "ThetaAllReduceBC::Do(): "
      << " Done wait for my turn - ReceivedCount is "
      << ReceivedCount.SafeFetch()
      << EndLogLine;

    tActorPacket ap;
    ap.mHeader.mSourceNode   = MyNode;

    int Loaded = 0;

    for( int pi = 0; pi < aCount; pi++ )
      {
      // Check if the currenet element is non-identity
      if( ! Op::IsIdentity( aSourceElementArray[ pi ] ) )
        {
        // If this is the first element in the packet, set the target address
        if ( Loaded == 0 )
          ap.mHeader.mElementSubArrayPointer = &( aTargetElementArray[ pi ] );

        // load element into packet
        ap.mElementSubArray[ Loaded ] = aSourceElementArray[ pi ];
        Loaded++;
        }

      if( Loaded
          &&
          (
            Op::IsIdentity( aSourceElementArray[ pi ] )
            ||
            Loaded == tActorPacket::eMaxElementPayloadCount
            ||
            (pi + 1) == aCount
          )
        )
        {
        // true size of packet is the addresss of the last element plus the sizeof an element minus that base address of the packet.
        // It is always a question about whether this works with structure packing
        unsigned ap_true_size = sizeof( tActorPacket::tHeader ) + Loaded * sizeof( tElement ) ;

        PacketCount[ Loaded ]++;

        ap.mHeader.mElementCount = Loaded;

        BegLogLine(0)
          << "ThetaAllReduceBC::Do(): "
          << " Calling trigger for Start Element "
          << pi - Loaded + 1
          << " Loaded "
          << Loaded
          << "true size "
          << ap_true_size
          << " Count in size "
          << PacketCount[ Loaded ]
          << EndLogLine;

        Loaded = 0;

        Platform::Reactor::Trigger( ActorFx,
                                    (void *) &ap,
                                    ap_true_size );
        }
      }

    // send terminal packet - a packet with no contents
    ap.mHeader.mElementSubArrayPointer = NULL;
    ap.mHeader.mElementCount           = Loaded;
    Platform::Reactor::Trigger( ActorFx,
                                (void *) &ap,
                                sizeof( tActorPacket::tHeader ) );

    BegLogLine(1)
      << "ThetaAllReduceBC::Do(): "
      << " Begin wait "
      << " ReceivedCount is "
      << ReceivedCount.SafeFetch()
      << EndLogLine;

    int LastReceivedCount = 0;
    int CheckReceivedCount;
    while( (CheckReceivedCount = ReceivedCount.SafeFetch()) < NodeCount )
      {
      if( CheckReceivedCount > LastReceivedCount )
        {
        LastReceivedCount = CheckReceivedCount;
        BegLogLine(1)
          << "ThetaAllReduceBC::Do(): "
          << " Waiting for all terminals "
          << " ReceivedCount is now "
          << ReceivedCount.SafeFetch()
          << EndLogLine;
        }
      // Should us a Platform::Mutex here
      Platform::Thread::Yield();
      }

    BegLogLine(1)
      << "ThetaAllReduceBC::Do(): "
      << " Done wait "
      << " ReceivedCount is now "
      << ReceivedCount.SafeFetch()
      << " NodeCount "
      << NodeCount
      << EndLogLine;

    int TotalElementsSent = 0;
    int TotalPacketsSent  = 0;
    for( int i = 0; i <=  tActorPacket::eMaxElementPayloadCount ; i++ )
      {
      if( PacketCount[ i ] )
        {
        BegLogLine(0)
          << "ThetaAllReduceBC::Do(): STATS "
          << " Sent "
          << PacketCount[ i ]
          << " Packets with "
          << i
          << " Elements or "
          << PacketCount[ i ]  * i  * sizeof( tElement )
          << " Bytes "
          << EndLogLine;
        }
      TotalElementsSent   += PacketCount[ i ] * i ;
      TotalPacketsSent    += PacketCount[ i ];
      }

    BegLogLine(1)
      << "ThetaAllReduceBC::Do(): STATS "
      << " TotalElements "
      << aCount
      << " TotalElementsSent "
      << TotalElementsSent
      << " TotalBytesSent "
      << TotalElementsSent * sizeof( tElement )
      << " TotalPacketsSent "
      << TotalPacketsSent
      << " Average Bytes Per Packet "
      << TotalElementsSent * sizeof( tElement) / TotalPacketsSent
      << " Fraction Sent "
      << ( 1.0 * TotalElementsSent )  / ( 1.0 * aCount )
      << EndLogLine;

    // THIS IS A RACE COND - OTHER NODES MAY START NEXT SEND BEFORE THIS IS DONE - VERY BAD
    ReceivedCount.AtomicSet( 0 );
    }

  };

