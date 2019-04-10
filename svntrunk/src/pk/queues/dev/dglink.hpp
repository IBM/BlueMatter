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
 * Module:          datagram_Link
 *
 * Purpose:         Datagram supported single-reader-single-writer Links.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         170297 BGF Created.
 *
 ***************************************************************************/

#ifndef NO_LINK_DEBUG
#define LINK_DEBUG
#endif

#ifndef __DATAGRAM_Link_HPP__
#define __DATAGRAM_Link_HPP__

#ifndef PKFXLOG_DGLink
#define PKFXLOG_DGLink (0)
#else
#define PKFXLOG_DGLink_PROTOCOL (1)
#endif

#ifndef PKFXLOG_DGLink_PROTOCOL
#define PKFXLOG_DGLink_PROTOCOL (0)
#endif

#ifndef PKTRACE_CHAN_COMM
#define PKTRACE_CHAN_COMM 0
#endif

#ifndef PKTRACE_ACTOR_COMM
#define PKTRACE_ACTOR_COMM 0
#endif

#include <pk/fxlogger.hpp>

#include <pk/memfence.hpp> // SMP memory fencing ops.

#ifndef DATAGRAM_SIZE
#define DATAGRAM_SIZE PKREACTOR_DATAGRAM_SIZE
#endif

static char *NoChanName = "NoChanName";

//*****************************************************************************

template<class Payload> class LinkDatagram;
template<class Payload> class LinkReader;
template<class Payload> class LinkWriter;

// This is what actually flies.
template<class Payload>
class LinkDatagram
  {
  public:
    LinkReader<Payload>    *mLinkReaderAddress; 
    unsigned                mSeqNo;
    Payload                 mPayload;

#ifdef LINK_DEBUG
    unsigned             mSourceTask;
    LinkWriter<Payload> *mLinkWriterAddress;
    unsigned             mDatagramLen;
#endif
  };

// Methods and data to keep track of a receive port.
template<class Payload>
class LinkReader
  {
  public:

    LinkDatagram<Payload> *mLinkDatagramPtr;
    unsigned               mExpectedSeqNo;

#ifdef LINK_DEBUG
    int                    mExpectedSourceTask;
    LinkWriter<Payload>*   mExpectedSourceAddress;
    int                    mExpectedSize;
#endif

    LinkReader()
      {
      Reset();
      }

    int Reset()
      {
      BegLogLine(PKFXLOG_DGLink_PROTOCOL)
        << " LinkReader::Reset "
        << " this "
        << (void *) this
        << " TaskNo "
        <<  MpiSubSys::GetTaskNo()
        << EndLogLine;

      mExpectedSeqNo         = 0;
      mLinkDatagramPtr       = NULL;

#ifdef LINK_DEBUG
      mExpectedSourceTask    = -1;
      mExpectedSourceAddress = NULL;
#endif
      return(0);
      }

    static int
    DeliverLinkDatagram(  void *msg )
      {
      LinkDatagram<Payload> *LinkDatagramPtr =
           (LinkDatagram<Payload> *)msg;

      LinkReader   *LinkReaderPtr = LinkDatagramPtr->mLinkReaderAddress;

      BegLogLine( PKFXLOG_DGLink_PROTOCOL )                                             
        << " LinkReader::DeliverLinkDatagram: "
        << " LDg->LinkReaderAddress @ " << LinkDatagramPtr->mLinkReaderAddress
        << " LDg->SeqNo "               << LinkDatagramPtr->mSeqNo
        << " LR->mExpectedSeqNo "       << LinkReaderPtr->mExpectedSeqNo
        << " LR->mLinkDatagramPtr "     << LinkReaderPtr->mLinkDatagramPtr
#ifdef LINK_DEBUG
// be nice to put the debugging fields in here.
#endif
        << EndLogLine;

      // Assert that the SeqNo is exactly what is expected.
      assert( LinkDatagramPtr->mSeqNo == LinkReaderPtr->mExpectedSeqNo );

      // assert that another packet isn't still in the pointer.
      assert( LinkReaderPtr->mLinkDatagramPtr == NULL );

      LinkReaderPtr->mLinkDatagramPtr =  LinkDatagramPtr;

      ExportFence();   //NEED: is this needed here: putting the pointer in place makes dg available - fence data our first.

      BegLogLine(PKFXLOG_DGLink_PROTOCOL)
        << " LinkReaderSet::DeliverDatagram: "
        << " Ended "
        << EndLogLine;
      return(PKREACTOR_DETATCH_BUFFER);
      }

    int
    Poll( unsigned aSeqNo )
      {
      int rc = 0;
      if( mLinkDatagramPtr != NULL )
        {
        if( aSeqNo == mLinkDatagramPtr->mSeqNo )
          {
          assert( aSeqNo == mExpectedSeqNo );  // backcheck that this is so.
          rc = 1;
          }
        }
      return( rc );
      }

    Payload &
    AccessPayload( unsigned aSeqNo )
      {
      assert( Poll( aSeqNo ) );
      return( mLinkDatagramPtr->mPayload );
      }

    void
    FreePayload( unsigned SeqNo )
      {
      PkActiveMsgSubSys::GetIFPtr()->FreeBuffer( (void *) mLinkDatagramPtr );
      mLinkDatagramPtr = NULL;
      mExpectedSeqNo++;
      //NEED: do we need a memory sync here??
      }

  };

//***************************************************************************************

template<class Payload>
class LinkWriter
  {
  private:

    int                           mDestTaskNo;
    LinkDatagram<Payload>         mLinkDatagramBuffer;

  public:

    int Bind( int aDestTaskNo, LinkReader<Payload> *aLinkReaderAddress )
      {
      BegLogLine(PKFXLOG_DGLink_PROTOCOL)
        << " LinkWriter::Bind "
        << " this "
        << (void *) this
        << " DestTaskNo "
        << aDestTaskNo
        << " aLinkReaderAddress "
        << (void *) aLinkReaderAddress
        << " LinkDatagramSize "
        << sizeof( LinkDatagram<Payload> )
        << EndLogLine;

      mDestTaskNo                            = aDestTaskNo;
      mLinkDatagramBuffer.mLinkReaderAddress = aLinkReaderAddress;

      return(0);
      }

    LinkWriter()
      {
      BegLogLine(PKFXLOG_DGLink_PROTOCOL)
        << " LinkWriter::Constructor "
        << (void *) this
        << EndLogLine;
      assert( sizeof( LinkDatagram<Payload> ) < DATAGRAM_SIZE );
      mDestTaskNo                              = -1;
      mLinkDatagramBuffer.mLinkReaderAddress   = NULL;
      mLinkDatagramBuffer.mSeqNo               = 0;

#ifdef BLUEGENE
      mLinkDatagramBuffer.mSourceTask        = MpiSubSys::GetTaskNo();
      mLinkDatagramBuffer.mDatagramLen       = sizeof( mLinkDatagramBuffer );
      mLinkDatagramBuffer.mLinkWriterAddress = this;
#endif

      }


    Payload &
    AccessPayload( int aSeqNo )
      {
      assert( mLinkDatagramBuffer.mSeqNo == aSeqNo );
      return( mLinkDatagramBuffer.mPayload );
      }

    void
    Push( int aSeqNo )
      {
      BegLogLine(PKFXLOG_DGLink_PROTOCOL)
        << " FlushMsgBuf "
        << " SeqNo "
        << mLinkDatagramBuffer.mSeqNo
        << EndLogLine;

      assert( mLinkDatagramBuffer.mSeqNo == aSeqNo );

///   if( mDestTaskNo == MpiSubSys::GetTaskNo() )
///     {
///     LinkReader<Payload>::DeliverLinkDatagram( (void *) & mLinkDatagramBuffer );
///     }
///   else
        {
        PkActiveMsgSubSys::Trigger( mDestTaskNo,
                                    LinkReader<Payload>::DeliverLinkDatagram,
                                    (void *) &mLinkDatagramBuffer,
                                    sizeof( mLinkDatagramBuffer )           );
        }

      mLinkDatagramBuffer.mSeqNo++;

      }

  };


#endif
