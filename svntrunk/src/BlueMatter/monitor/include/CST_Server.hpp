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
 //#include <rdgpacket.hpp>
#include <strstream.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>

#include "CST.hpp"

#include <stdio.h>
#include <iostream>
#include <map>

class DatagramConnectionState
  {
  public:

    typedef struct { enum { Unknown = 0, Connected , Terminating, Disconnected }; } Status;

    unsigned int mId;
    unsigned int mStatus;
    AbsSeqNo_int NextSeqNoExpected;
    AbsSeqNo_int NextRecvMetadataAckSeqNo;
    AbsSeqNo_int NextSendMetadataAckSeqNo;

    DatagramConnectionState()
      {
      mId                        = -1;
      mStatus                    = Status::Unknown;
      NextSeqNoExpected          =  0 ;
      NextRecvMetadataAckSeqNo   =  1 ;
      NextSendMetadataAckSeqNo   =  1 ;
      }
  };

class IpAddrPort
  {
  public:

    IpAddrPort( unsigned aAddr = 0xFFFFFFFF, unsigned aPort = 0xFFFFFFFF )
      {
      Init( aAddr, aPort );
      }

    void
    Init( unsigned aAddr, unsigned aPort )
      {
      mAddr = aAddr;
      mPort = aPort;
      }

    unsigned mAddr;
    unsigned mPort;

  };

struct LessIpAddrPort
  {
  bool
  operator()(const IpAddrPort iapA, const IpAddrPort iapB ) const
    {
    int rc = 0;
    if( iapA.mAddr < iapB.mAddr )
      rc = 1;
    else if( (iapA.mAddr == iapB.mAddr) && (iapA.mPort < iapB.mPort) )
      rc = 1;
    return( rc );
    }
  };


typedef std::map < IpAddrPort, DatagramConnectionState, LessIpAddrPort> T_IpAddrPortToConIdMap;

T_IpAddrPortToConIdMap IpAddrPortToConIdMap;


#define CST_FATAL_ERROR(s)  cerr << s << endl, exit( -1 );

#include "../../../bgfe/pk/fxlogger/include/fxlogger.hpp"

void error(char *msg)
  {
  perror(msg);
  exit(-1);
  }

class CSTServer
  {
  public:
    int                sock;

    CSTServer( int aPort = 0 )
      {
      int                n;
      unsigned long      length;
      struct sockaddr_in server;

      sock = socket(AF_INET, SOCK_DGRAM, 0);

      if( sock < 0 )
        error("Opening socket");

      length = sizeof( server );
      bzero( &server, length );

      int              namelength;
      struct  hostent  hostent_copy;               /* Storage for hostent data.  */
      struct  hostent *hostentptr;                 /* Pointer to hostent data.   */
      static  char     hostname[256];
      int     nRet;


      /*
       * Get the local host name.
       */
      nRet = gethostname( hostname, sizeof hostname );
      if( nRet )
        error ("gethostname failed");

      BegLogLine(1)
        << "gethostbyname() returned >"
        << hostname
        << "<"
        << EndLogLine;

      /*
       * Get pointer to network data structure for local host.
       */
      hostentptr = gethostbyname( hostname );
      if( hostentptr == NULL)
        error( "gethostbyname");

      /*
       * Copy hostent data to safe storage.
       */
      hostent_copy = *hostentptr;

      BegLogLine(1)
        << "hostent_copy.h_addr "
        << (void *) hostent_copy.h_addr
        << EndLogLine;

      int LocalIpAddr = * ((unsigned long *) hostent_copy.h_addr);

      BegLogLine(1)
        << "LocalIpAddr "
        << (void *) LocalIpAddr
        << EndLogLine;

      ////  if( argc <= 3 )  ... don't ask, just find out
      LocalIpAddr = INADDR_ANY;

      server.sin_family      = AF_INET;
      server.sin_addr.s_addr = LocalIpAddr;     // INADDR_ANY;
      server.sin_port        = aPort;

      int bind_rc = bind( sock,
                         (struct sockaddr *) & server,
                          length);
      if( bind_rc < 0 )
        error("binding");


      // check what we really got for addr and port
      int gsn = getsockname(   sock,                                // socket
                            (  struct sockaddr *) & server,         // name
                             & length);                             // name length
      if( gsn != 0 )
            error( "getsockname for port");

      int port = ntohs( server.sin_port );      // pick off the actual port asigned.

      if( port != aPort )
        fprintf( stderr, " Could not assign port asked for - asked for %d (%08X) got %d (%08X)\n", aPort, aPort, port, port );

      //
      // permit sending of broadcast messages.
      //
      int nValue = 1;
      int sso = setsockopt(sock,                // socket
                           SOL_SOCKET,          // level
                           SO_BROADCAST,        // option name
                           (char *) &nValue,    // *pOptionValue
                            sizeof(nValue));     // nOptionLen

      if( sso != 0 )
        error("setsockopt for broadcast messages");

      ////////
      //////// Now set it up for non blocking read and writes.
      ////////
      //////int ulValue = 1;
      //////int is = ioctl( sock,                // socket descriptor
      //////                FIONBIO,                // cmd, Enable or disable non-blocking mode
      //////               &ulValue);              // *argp);
      //////if( is < 0 )
      //////    perror("ioctlsocket(): setting non-blocking");
      //////

      BegLogLine(1)
        << "server.aaddr "
        << server.sin_addr.s_addr
        << " server.port "
        << server.sin_port
        << EndLogLine;
      }

    int
    GetPacket( char ** aPacket, int * aPacketLen )
      {
      CST_Packet CstRecvPkt ;
      int        LostDataPacketFlag = 0;
      unsigned   TotalConnectionsMade = 0;
      unsigned   TotalConnectedClients = 0;


      unsigned long      fromlen;
      struct sockaddr_in from;

      fromlen = sizeof( struct sockaddr_in );

      while( 1 )
        {
        int n = recvfrom( sock,
                         &CstRecvPkt ,
                          sizeof ( CstRecvPkt ),
                          0,
                          (struct sockaddr *) & from,
                          &fromlen );

        if( (n < CST_MAXLEN_HEADER) || (n > CST_MAXLEN_PACKET) )
          {
          BegLogLine(1)
               << "CST_Client : recvfrom got "
               << n
               << " bytes, errno is "
               << errno
               << "  Expected from "
               << CST_MAXLEN_HEADER
               << " to "
               << CST_MAXLEN_PACKET
               << EndLogLine;
          exit( -1 );
          }

        BegLogLine(0)
             << "CST_Server - RECEIVED PACKET "
             << "Src Addr "
             << from.sin_addr.s_addr
             << " Src Port "
             << from.sin_port
             << "Pkt.mHeader.mAbsSeqNo "
             << CstRecvPkt.mHeader.mAbsSeqNo
             << " Recv Bytes "
             << n
             << " errno "
             << errno
             << EndLogLine;

        DatagramConnectionState &dcs =  IpAddrPortToConIdMap[ IpAddrPort( from.sin_addr.s_addr, from.sin_port) ];

        // Check if this is a new connection
        if( dcs.mStatus == DatagramConnectionState::Status::Unknown  )
          {
          dcs.mId = TotalConnectionsMade;

          TotalConnectionsMade ++ ;
          TotalConnectedClients ++ ;

          dcs.mStatus = DatagramConnectionState::Status::Connected;

          BegLogLine(1)
            << "CST_Server ConId " << &dcs << " "
            << "NEW CONNECTION "
            << "ID " << & dcs
            << " TotalConnectionsMade "
            << TotalConnectionsMade
            << " TotalConnectedClients "
            << TotalConnectedClients
            << EndLogLine;
          }


        // Header AbsSeqNo's >= 0 mean a data packet
        if( CstRecvPkt.mHeader.mAbsSeqNo >= 0 )
          {

          BegLogLine(0)
            << "CST_Server ConId " << &dcs << " "
            << " Got DATA packet"
            << " Pkt.mHeader.mAbsSeqNo "
            <<  CstRecvPkt.mHeader.mAbsSeqNo
            << " Expected AbsSeqNo "
            << dcs.NextSeqNoExpected
            << EndLogLine;

          if( CstRecvPkt.mHeader.mAbsSeqNo == dcs.NextSeqNoExpected )
            {
            dcs.NextSeqNoExpected++;

            BegLogLine(0)
              << "CST_Server ConId " << &dcs << " "
              << "Data packet expected received "
              << "SeqNo "
              << CstRecvPkt.mHeader.mAbsSeqNo
              << " First word "
              << (*((int *) CstRecvPkt.mPayload.mByteArray))
              << EndLogLine ;

            *aPacket    = CstRecvPkt.mPayload.mByteArray ;
            *aPacketLen = n - sizeof( CST_Header ) ;
            return(0);

            ////int write_rc = write( 1, CstRecvPkt.mPayload.mByteArray, n - sizeof( CST_Header ) );
            ////fsync( 1 );
            ////if( write_rc != (n - sizeof( CST_Header ) ) )
            ////  {
            ////  BegLogLine(1)
            ////    << "CST_Server ConId " << &dcs << " "
            ////    << " FAILED TO WRITE DATA RECEIVED TO STDOUT "
            ////    << EndLogLine;
            ////  exit( -1 );
            ////  }
            ////

            }
          else if( CstRecvPkt.mHeader.mAbsSeqNo > dcs.NextSeqNoExpected )
            {

            BegLogLine(1)
              << "CST_Server ConId " << &dcs << " "
              << "Data packet ahead of expected received "
              << "SeqNo "
              << CstRecvPkt.mHeader.mAbsSeqNo
              << " Expected SeqNo "
              <<  dcs.NextSeqNoExpected
              << EndLogLine ;

            LostDataPacketFlag = 1;
            }
          else
            {
            BegLogLine(1)
              << "CST_Server ConId " << &dcs << " "
              << "Data packet behind expected received "
              << "SeqNo "
              << CstRecvPkt.mHeader.mAbsSeqNo
              << " Expected SeqNo "
              <<  dcs.NextSeqNoExpected
              << EndLogLine ;

            continue;
            }
          }
        else
          {

          BegLogLine(1)
            << "CST_Server ConId " << &dcs << " "
            << " Got METADATA packet "
            << " Pkt.mHeader.mAbsSeqNo "
            <<  CstRecvPkt.mHeader.mAbsSeqNo
            << " Expected AbsSeqNo "
            << dcs.NextRecvMetadataAckSeqNo
            << EndLogLine;

          // if this metadata packet is newer than the last one we got, return
          if( (CstRecvPkt.mHeader.mAbsSeqNo * -1) < dcs.NextRecvMetadataAckSeqNo )
            {
            BegLogLine(1)
                 << "CST_Server ConId " << &dcs << " "
                 << "Got duplicate or old metadata packet "
                 << CstRecvPkt.mHeader.mAbsSeqNo
                 << EndLogLine;
            continue;
            }

          BegLogLine(1)
               << "CST_Server ConId " << &dcs << " "
               << " Got valid metadata packet - SeqNo "
               << CstRecvPkt.mHeader.mAbsSeqNo
               << EndLogLine;

          dcs.NextRecvMetadataAckSeqNo++;

          if( CstRecvPkt.mPayload.mMetadata.mType
              ==
              CST_Packet::t_Payload::t_Metadata::CST_METADATA_PACKET_TYPE_ACKREQ )
            {

            CST_Packet AckPkt;

            AckPkt.mHeader.mAbsSeqNo           = -1 * dcs.NextSendMetadataAckSeqNo;
            dcs.NextSendMetadataAckSeqNo++;

            if( dcs.NextSeqNoExpected < CstRecvPkt.mPayload.mMetadata.mField.mAckReq.mLastSentSeqNo )
              {
              AckPkt.mPayload.mMetadata.mType = CST_Packet::t_Payload::t_Metadata::CST_METADATA_PACKET_TYPE_GOBACKN ;
              AckPkt.mPayload.mMetadata.mField.mGoBackN.mGoBackToSeqNo = dcs.NextSeqNoExpected  ;
              }
            else
              {
              AckPkt.mPayload.mMetadata.mType = CST_Packet::t_Payload::t_Metadata::CST_METADATA_PACKET_TYPE_ACK ;
              AckPkt.mPayload.mMetadata.mField.mAck.mAckedSeqNo = dcs.NextSeqNoExpected - 1 ;
              }

            BegLogLine(1)
                 << "CST_Server ConId " << &dcs << " "
                 << " Valid AckReq Packet received "
                 << " LostDataPacketFlag "
                 << LostDataPacketFlag
                 << " Acking Data SeqNo of "
                 << AckPkt.mPayload.mMetadata.mField.mAck.mAckedSeqNo
                 << " Output metadata packet SeqNo "
                 << AckPkt.mHeader.mAbsSeqNo
                 << EndLogLine;

            LostDataPacketFlag = 0;

            n = sendto( sock,
                       &AckPkt,
                        sizeof( CST_Packet ),
                        0,
                       (struct sockaddr *) & from,
                        fromlen);
            if( n < 0 )
              perror("sendto");
            }
          else
            {
            BegLogLine(1)
                 << "CST_Server ConId " << &dcs << " "
                 << " Metadata packet type not recognized. Type is "
                 << CstRecvPkt.mPayload.mMetadata.mType
                 << EndLogLine;
            }
          }
        }
      }

  };




