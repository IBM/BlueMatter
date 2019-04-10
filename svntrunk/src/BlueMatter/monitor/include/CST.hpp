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
 #ifndef __CST_HPP___
#define __CST_HPP___

// Client synchcronous transport layer

typedef long long AbsSeqNo_int;

// These should be derived from 'sizeof()s' on the structs
#define CST_MAXLEN_HEADER  (8)
#define CST_MAXLEN_PAYLOAD (240)
#define CST_MAXLEN_PACKET  ( CST_MAXLEN_HEADER + CST_MAXLEN_PAYLOAD )

// If the SeqNo value is positive, this is a data packet.
// If negative, metadata.
struct CST_Header
  {
  AbsSeqNo_int    mAbsSeqNo ;
  } ;

enum { MetadataPacketType_Ack     = 0x01<<00,
       MetadataPacketType_GoBackN = 0x01<<01,
       MetadataPacketType_AckReq  = 0x01<<02 };

// Metadata packets may be lost.
// Each metadata packet totaly supercedes those sent earlier.

struct CST_Packet
  {
  CST_Header mHeader;
  union t_Payload
    {
    char   mByteArray[ CST_MAXLEN_PAYLOAD ];
    struct t_Metadata
      {
      enum {
             CST_METADATA_PACKET_TYPE_UNDEFINED = 0,
             CST_METADATA_PACKET_TYPE_ACK       = 1,
             CST_METADATA_PACKET_TYPE_GOBACKN   = 2,
             CST_METADATA_PACKET_TYPE_ACKREQ    = 3,
             CST_METADATA_PACKET_TYPE_EOT       = 4   // End Of Transmission
           };
      typedef int MetadataTypeType;
      MetadataTypeType       mType;
      union t_Field
        {
        struct
          {
          AbsSeqNo_int mAckedSeqNo;
          } mAck ;
        struct
          {
          AbsSeqNo_int mGoBackToSeqNo;
          } mGoBackN ;
        struct t_AckReq
          {
          AbsSeqNo_int mLastSentSeqNo;
          } mAckReq ;
        struct t_EotReq
          {
          AbsSeqNo_int mLastSentSeqNo;
          } mEotReq ;
        enum { mAckReqLen = sizeof( CST_Header ) + sizeof( MetadataTypeType ) + sizeof( t_AckReq ) };
        } mField ;
      } mMetadata ;
    } mPayload ;
  };


#endif
