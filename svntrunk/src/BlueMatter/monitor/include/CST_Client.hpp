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
 #ifndef __CST_Client__
#define __CST_Client__

#include <iostream>
#include <strstream>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>

#include <assert.h>

#include <BlueMatter/CST.hpp>

#define CST_FATAL_ERROR(s)  std::cerr << s << std::endl, exit( -1 );

#include <pk/fxlogger.hpp>

class CST_Client
  {
    int                 mSocketFd;
    struct sockaddr_in  mServerInetAddr;

    int                 mAbsoluteAckedSeqNo;

    struct BufferPacket
      {
      unsigned        mTrueLen;
      CST_Packet      mCST_Packet;
      };

    int           mTotalPacketBufferCount;
    BufferPacket *mBufferPool;

    AbsSeqNo_int mAbsSeqNo_NextToPost;
    AbsSeqNo_int mAbsSeqNo_LastAcked;
    AbsSeqNo_int mAbsSeqNo_NextToSend;

    AbsSeqNo_int mAbsSeqNo_NextMetadataPacketToSend;
    AbsSeqNo_int mAbsSeqNo_LastMetadataPacketReceived;

    enum {
         READY_TO_SEND = 0x01<<00 ,
         READY_TO_ACK  = 0x01<<01 ,
         READY_TO_POST = 0x01<<02 ,
         OPENED        = 0x01<<03
         };

    enum {
         NOT_READY_TO_SEND = 0 ,
         NOT_READY_TO_ACK  = 0 ,
         NOT_READY_TO_POST = 0 ,
         CLOSED
         };


    unsigned
    GetCurrentState()
      {
      // states:: READY_TO_SEND, READY_TO_ACK, READY_TO_POST
      unsigned StateRc = 0;
      if( mAbsSeqNo_NextToSend <  mAbsSeqNo_NextToPost )
        StateRc |= READY_TO_SEND;
      if( mAbsSeqNo_LastAcked  <  (mAbsSeqNo_NextToSend - 1) )
        StateRc |= READY_TO_ACK;
      if( mAbsSeqNo_NextToPost <= (mAbsSeqNo_LastAcked + mTotalPacketBufferCount) )
        StateRc |= READY_TO_POST;
      return( StateRc );
      }

    enum t_EventType {
         EVENT_RECV_GoBackN,
         EVENT_RECV_Ack,
         EVENT_API_Post,
         EVENT_API_Push,
         EVENT_API_Sync
         } ;

    struct CST_EVENT
      {

      t_EventType mType;

      int mDataLen;

      union t_Data
        {
        char          * mByteArray;
        CST_Packet    * mCST_Packet;
        } mData ;
      };


  public:

    void
    Init( int aServerIpAddr, unsigned aServerUdpPort, unsigned aBufferCount )
      {

      mSocketFd = socket(AF_INET, SOCK_DGRAM, 0);

      if( mSocketFd < 0 )
        perror("socket");

      //
      // Now set it up for non blocking read and writes.
      //
      int ulValue = 1;
      int is = ioctl( mSocketFd,                // socket descriptor
                      FIONBIO,             // cmd, Enable or disable non-blocking mode
                     &ulValue);            // *argp);
      if( is < 0 )
        perror("ioctl(): setting non-blocking");

      // Set up the udp addr/port of the CST server
      mServerInetAddr.sin_family        = AF_INET;
// THE FOLLOWING LINE MAY NOT BE RIGHT
      mServerInetAddr.sin_addr.s_addr   = aServerIpAddr ;
      mServerInetAddr.sin_port          = htons( aServerUdpPort );

      mAbsSeqNo_NextToPost =  0 ; // next to post at 0 seqno
      mAbsSeqNo_NextToSend =  0 ; // not ready to send
      mAbsSeqNo_LastAcked  = -1 ; // not ready to ack

      mTotalPacketBufferCount = aBufferCount;
      mBufferPool             = new BufferPacket[ mTotalPacketBufferCount ];
      assert( mBufferPool != NULL );

      mAbsSeqNo_NextMetadataPacketToSend    = 1 ;
      mAbsSeqNo_LastMetadataPacketReceived  = 1 ;

      }

    int
    RecvFrom( void * aBuf, int * aTrueLen )
      {
      unsigned long int fromlen = sizeof( struct sockaddr_in );
      int rc = 0;
      int recvfrom_rc = recvfrom( mSocketFd,
                                  (char *) aBuf ,
                                  CST_MAXLEN_PACKET ,
                                  0,
                                  (struct sockaddr *) & mServerInetAddr,
                                  &fromlen);
      if( recvfrom_rc < 0 )
        {
        if( errno != EAGAIN )
          perror("CST Client polling recvfrom : ");
        *aTrueLen = 0;
        }
      else
        {
        *aTrueLen = recvfrom_rc;
        rc = 1;
        }
      return( rc );
      }


    int
    SendTo( void * aBuf, int aLen )
      {

      int sendto_rc  = sendto( mSocketFd,
                               aBuf,
                               aLen,
                               0,
                              (struct sockaddr *) & mServerInetAddr,
                               sizeof( mServerInetAddr ) );
      if( sendto_rc < 0 )
        perror("Sendto ()");
      return(1);
      }


    void
    tPost( CST_EVENT & aEvent )
      {
      assert( GetCurrentState() & READY_TO_POST );
      int bufind = mAbsSeqNo_NextToPost % mTotalPacketBufferCount;
      // Set SeqNo - note: since it is positive, this is a data packet.
      mBufferPool[ bufind ].mCST_Packet.mHeader.mAbsSeqNo = mAbsSeqNo_NextToPost ;
      mBufferPool[ bufind ].mTrueLen = aEvent.mDataLen + CST_MAXLEN_HEADER;
      memcpy( mBufferPool[ bufind ].mCST_Packet.mPayload.mByteArray,
              aEvent.mData.mByteArray,
              aEvent.mDataLen );

      mAbsSeqNo_NextToPost++;

      BegLogLine(1)
           << "CST_Client::tPost() "
           << " Posting at bufind "
           << bufind
           << "  AbsSeqNo "
           << mBufferPool[ bufind ].mCST_Packet.mHeader.mAbsSeqNo
           << EndLogLine;

      return;
      }

    void
    tSendAckRequest()
      {
      CST_Packet cstpkt;

      cstpkt.mHeader.mAbsSeqNo = mAbsSeqNo_NextMetadataPacketToSend * -1 ;

      mAbsSeqNo_NextMetadataPacketToSend++;

      cstpkt.mPayload.mMetadata.mType =
           CST_Packet::t_Payload::t_Metadata::CST_METADATA_PACKET_TYPE_ACKREQ ;

      cstpkt.mPayload.mMetadata.mField.mAckReq.mLastSentSeqNo = mAbsSeqNo_NextToSend - 1;

      int TrueLen = CST_Packet::t_Payload::t_Metadata::t_Field::mAckReqLen;

      BegLogLine(1) << "CST_Client::SendAckReq() Metadata Packet AbsSeqNo "
           << cstpkt.mHeader.mAbsSeqNo
           << " Request Ack on LastSentSeqNo: "
           << cstpkt.mPayload.mMetadata.mField.mAckReq.mLastSentSeqNo
           << " ( Ack Packet Len "
           << TrueLen
           << " ) "
           << EndLogLine;

      SendTo( & cstpkt, TrueLen );

      return;
      }

    void
    tSendData()
      {
      BegLogLine(1)
        << "CST_Client::tSendData() "
        << " mAbsSeqNo_NextToSend "
        << mAbsSeqNo_NextToSend
        << " mAbsSeqNo_NextToPost "
        << mAbsSeqNo_NextToPost
        << EndLogLine;

      assert( mAbsSeqNo_NextToSend < mAbsSeqNo_NextToPost  );

      int bufind = mAbsSeqNo_NextToSend % mTotalPacketBufferCount;

      mBufferPool[ bufind ].mCST_Packet.mHeader.mAbsSeqNo = mAbsSeqNo_NextToSend ;

      SendTo( & (mBufferPool[ bufind ].mCST_Packet),
                 mBufferPool[ bufind ].mTrueLen      );

      BegLogLine(1)
        << "CST_Client::tSendData() "
        << "SendAbsSeqNo "
        <<  mBufferPool[ bufind ].mCST_Packet.mHeader.mAbsSeqNo
        << " Packet TrueLen "
        << mBufferPool[ bufind ].mTrueLen
        << EndLogLine;

      mAbsSeqNo_NextToSend++;

      return;
      }


    int
    IsValidMetadataPacket( CST_EVENT & aEvent )
      {
      int rc = 1;  // true
      if( (aEvent.mData.mCST_Packet->mHeader.mAbsSeqNo * -1) < mAbsSeqNo_LastMetadataPacketReceived )
        return 0;  // false
      mAbsSeqNo_LastMetadataPacketReceived = (aEvent.mData.mCST_Packet->mHeader.mAbsSeqNo * -1);
      return( rc );
      }

    int
    tGoBackN( CST_EVENT & aEvent )
      {
      if( IsValidMetadataPacket( aEvent ) )
        {
        // check if this is a request to go back further than the last SeqNo already acked.
        if( aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mGoBackN.mGoBackToSeqNo < mAbsSeqNo_LastAcked )
          CST_FATAL_ERROR( "GoBackN packet going back to SeqNo less than already acked SeqNo " );

        //
        if( aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mGoBackN.mGoBackToSeqNo >= mAbsSeqNo_NextToSend )
          CST_FATAL_ERROR( "GoBackN packet going back to SeqNo greater than last sent SeqNo " );

        BegLogLine(1)
          << "CST_Client::GoBackN() "
          << " Setting NextToSend back to "
          <<  aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mGoBackN.mGoBackToSeqNo
          << " from "
          << mAbsSeqNo_NextToSend
          << EndLogLine;

        mAbsSeqNo_NextToSend = aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mGoBackN.mGoBackToSeqNo;
        }
      else
        {
        BegLogLine(1)
          << "CST_Client::GoBackN() "
          << " Metadata packet was not valid - skipping "
          << EndLogLine;
        }
      return( 0 );
      }

    int
    tAck( CST_EVENT & aEvent )
      {
      if( IsValidMetadataPacket( aEvent ) )
        {
        // check if this is a request to go back further than the last SeqNo already acked.
        if( aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mAck.mAckedSeqNo < mAbsSeqNo_LastAcked )
          CST_FATAL_ERROR( "Ack packet acking SeqNo less than Last Acked SeqNo " );

        //
        if( aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mAck.mAckedSeqNo >= mAbsSeqNo_NextToSend )
          CST_FATAL_ERROR( "Ack packet acking SeqNo greater than last sent SeqNo " );

        BegLogLine(1) << "CST_Client::Ack() "
             << " Setting LastAcked to "
             << aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mAck.mAckedSeqNo
             << " from "
             << mAbsSeqNo_LastAcked
             << EndLogLine;

        mAbsSeqNo_LastAcked = aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mAck.mAckedSeqNo;
        }
      return( 0 );
      }

    int
    tAssertRedundantAck( CST_EVENT & aEvent )
      {
      if( IsValidMetadataPacket( aEvent ) )
        {
        // check if this is a request to go back further than the last SeqNo already acked.
        if( aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mAck.mAckedSeqNo != mAbsSeqNo_LastAcked )
          CST_FATAL_ERROR( "Redundant Ack packet acking SeqNo less than Last Acked SeqNo " );

        //
        if( aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mAck.mAckedSeqNo >= mAbsSeqNo_NextToSend )
          CST_FATAL_ERROR( "Ack packet acking SeqNo greater than last sent SeqNo " );

        BegLogLine(1) << "CST_Client::tAssertRedundantAck() "
             << " Packet Acks "
             << aEvent.mData.mCST_Packet->mPayload.mMetadata.mField.mAck.mAckedSeqNo
             << " which is already "
             << mAbsSeqNo_LastAcked
             << EndLogLine;
        }
      return( 0 );
      }

    int
    DoTransport( CST_EVENT aEvent )
      {
      int rc = 0;  // 0 is succeed
      unsigned int CurrentState = GetCurrentState();
      switch( CurrentState )
        {
        case       READY_TO_SEND |     READY_TO_ACK |     READY_TO_POST :
          {
          switch( aEvent.mType )
            {
            case EVENT_RECV_GoBackN :
              {
              rc = tGoBackN( aEvent );
              break;
              }
            case EVENT_RECV_Ack :
              {
              rc = tAck( aEvent );
              break;
              }
            case EVENT_API_Post :
              {
              tPost( aEvent );
              break;
              }
            case EVENT_API_Push :
              {
              tSendData();
              break;
              }
            case EVENT_API_Sync :
              {
              tSendAckRequest();
              break;
              }
            default:
              {
              break;
              }
            };
          break;
          }
        case   NOT_READY_TO_SEND |     READY_TO_ACK |     READY_TO_POST :
          {
          switch( aEvent.mType )
            {
            case EVENT_RECV_GoBackN :
              {
              rc = tGoBackN( aEvent );
              break;
              }
            case EVENT_RECV_Ack :
              {
              rc = tAck( aEvent );
              break;
              }
            case EVENT_API_Post :
              {
              tPost( aEvent );
              break;
              }
            case EVENT_API_Push :
              {
              ///tSendData();
              rc = -1;
              break;
              }
            case EVENT_API_Sync :
              {
              tSendAckRequest();
              break;
              }
            default:
              {
              break;
              }
            };
          break;
          }
        case       READY_TO_SEND | NOT_READY_TO_ACK |     READY_TO_POST :
          {
          switch( aEvent.mType )
            {
            case EVENT_RECV_GoBackN :
              {
              if( IsValidMetadataPacket( aEvent ) )
                CST_FATAL_ERROR( "GoBackN packet received but not ready to ack " );
              break;
              }
            case EVENT_RECV_Ack :
              {
              tAssertRedundantAck( aEvent );
              break;
              }
            case EVENT_API_Post :
              {
              tPost( aEvent );
              break;
              }
            case EVENT_API_Push :
              {
              tSendData();
              break;
              }
            case EVENT_API_Sync :
              {
              tSendAckRequest();
              break;
              }
            default:
              {
              break;
              }
            };
          break;
          }
        case   NOT_READY_TO_SEND | NOT_READY_TO_ACK |     READY_TO_POST :
          {
          switch( aEvent.mType )
            {
            case EVENT_RECV_GoBackN :
              {
              if( IsValidMetadataPacket( aEvent ) )
                CST_FATAL_ERROR( "GoBackN packet received but not ready to ack " );
              break;
              }
            case EVENT_RECV_Ack :
              {
              tAssertRedundantAck( aEvent );
              break;
              }
            case EVENT_API_Post :
              {
              tPost( aEvent );
              break;
              }
            case EVENT_API_Push :
              {
              //tSendData();
              rc = -1;
              break;
              }
            case EVENT_API_Sync :
              {
              //tSendAckRequest();
              break;
              }
            default:
              {
              break;
              }
            };
          break;
          }
        case       READY_TO_SEND |     READY_TO_ACK | NOT_READY_TO_POST :
          {
          switch( aEvent.mType )
            {
            case EVENT_RECV_GoBackN :
              {
              rc = tGoBackN( aEvent );
              break;
              }
            case EVENT_RECV_Ack :
              {
              rc = tAck( aEvent );
              break;
              }
            case EVENT_API_Post :
              {
              //tPost( aEvent );
              rc = -1;
              break;
              }
            case EVENT_API_Push :
              {
              tSendData();
              break;
              }
            case EVENT_API_Sync :
              {
              tSendAckRequest();
              break;
              }
            default:
              {
              break;
              }
            };
          break;
          }
        case   NOT_READY_TO_SEND |     READY_TO_ACK | NOT_READY_TO_POST :
          {
          switch( aEvent.mType )
            {
            case EVENT_RECV_GoBackN :
              {
              rc = tGoBackN( aEvent );
              break;
              }
            case EVENT_RECV_Ack :
              {
              rc = tAck( aEvent );
              break;
              }
            case EVENT_API_Post :
              {
              //tPost( aEvent );
              rc = -1 ;
              break;
              }
            case EVENT_API_Push :
              {
              //tSendData();
              rc = -1;
              break;
              }
            case EVENT_API_Sync :
              {
              tSendAckRequest();
              break;
              }
            default:
              {
              break;
              }
            };
          break;
          }
        case       READY_TO_SEND | NOT_READY_TO_ACK | NOT_READY_TO_POST :
          {
          switch( aEvent.mType )
            {
            case EVENT_RECV_GoBackN :
              {
              rc = tGoBackN( aEvent );
              break;
              }
            case EVENT_RECV_Ack :
              {
              rc = tAck( aEvent );
              break;
              }
            case EVENT_API_Post :
              {
              ////// A post occured when buffers are filled but nothing has been sent
              ////// The api user didn't sync before filling buffer
              ////// Won't be able to take this packet - but will drive the buffer out
              ////tSendData();
              ////tSendAckRequest();

              //tPost( aEvent );
              rc = -1;
              break;
              }
            case EVENT_API_Push :
              {
              tSendData();
              break;
              }
            case EVENT_API_Sync :
              {
              //tSendAckRequest();
              break;
              }
            default:
              {
              break;
              }
            };
          break;
          }

        case   NOT_READY_TO_SEND | NOT_READY_TO_ACK | NOT_READY_TO_POST :
          {
          CST_FATAL_ERROR( "Not ready to send, ack or post??? " );
          break;
          }

       default:
         {
         CST_FATAL_ERROR( "State value not recognized" );
         break;
         }
        };
      return( rc );
      }


    int
    PollAsyncEvent()
      {
      int rc = 0;
      CST_Packet cstpkt;
      int        truelen;

      if( RecvFrom( &cstpkt, &truelen ) )
        {

        CST_EVENT AsyncEvent;

        if( cstpkt.mHeader.mAbsSeqNo < 0 )
          {
          switch( cstpkt.mPayload.mMetadata.mType )
            {
            case CST_Packet::t_Payload::t_Metadata::CST_METADATA_PACKET_TYPE_ACK     :
              AsyncEvent.mType   = EVENT_RECV_Ack ;
              BegLogLine(1) << "CST_Client::PollAsyncEvent() "
                   << " Received an ACK packet event "
                   << " Metadata AbsSeqNo "
                   << cstpkt.mHeader.mAbsSeqNo
                   << EndLogLine;
              break;
            case CST_Packet::t_Payload::t_Metadata::CST_METADATA_PACKET_TYPE_GOBACKN :
              AsyncEvent.mType   = EVENT_RECV_GoBackN ;
              BegLogLine(1) << "CST_Client::PollAsyncEvent() "
                   << " Received an GoBackN packet event "
                   << " Metadata AbsSeqNo "
                   << cstpkt.mHeader.mAbsSeqNo
                   << EndLogLine;
              break;
            case CST_Packet::t_Payload::t_Metadata::CST_METADATA_PACKET_TYPE_ACKREQ  :
            default :
              BegLogLine(1) << "CST_Client::PollAsyncEvent() "
                   << " Received an ACK packet event "
                   << " Metadata AbsSeqNo "
                   << cstpkt.mHeader.mAbsSeqNo
                   << EndLogLine;
              assert( 0 ); // don't deal or don't work.
              break;
            };
          }
        else
          {
          CST_FATAL_ERROR( "Clent received non-metadata packet - not allowed yet." );
          }

        AsyncEvent.mDataLen          = truelen;
        AsyncEvent.mData.mCST_Packet = &cstpkt;

        rc = DoTransport( AsyncEvent );
        }
      return( rc );
      }

    int
    Close()
      {
      return(0);
      }

    int
    Post( char *aBuffer, int aLen )
      {
      CST_EVENT ApiEvent;
      ApiEvent.mType            = EVENT_API_Post;
      ApiEvent.mData.mByteArray = aBuffer;
      ApiEvent.mDataLen         = aLen;

      int rc = DoTransport( ApiEvent );

      return( rc );
      }


    int
    Push()
      {
      CST_EVENT ApiEvent;
      ApiEvent.mType            = EVENT_API_Push;
      ApiEvent.mData.mByteArray = NULL;
      ApiEvent.mDataLen         = 0;

      int rc = DoTransport( ApiEvent );

      PollAsyncEvent();

      return( rc );
      }


    int
    Sync()
      {
      int rc = 0;
      if( GetCurrentState() & READY_TO_ACK )  // Don't sync if there are no acks pending
        {
        CST_EVENT ApiEvent;
        ApiEvent.mType            = EVENT_API_Sync;
        ApiEvent.mData.mByteArray = NULL;
        ApiEvent.mDataLen         = 0;

        rc = DoTransport( ApiEvent );
        }

      return( rc );
      }

    int
    BlockingSync()
      {
      int rc = 0;
      PollAsyncEvent();
      while( GetCurrentState() & READY_TO_ACK )
        {
        rc = Sync();
if( GetCurrentState() & READY_TO_ACK )
  sleep(1);
        PollAsyncEvent();
        }
      return( rc );
      }


  };

#endif
