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
 * Module:          Broadcast.hpp
 *
 * Purpose:         Data element broadcast with operation.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         032900 BGF Created.
 *
 ***************************************************************************/

#ifndef _BROADCAST_HPP_
#define _BROADCAST_HPP_

#include "MemoryOps.hpp"
#include <pk/memfence.hpp>

#ifndef PKFXLOG_BROADCAST
#define PKFXLOG_BROADCAST ( 0 )
#endif


template<class Element, class BroadcastOp, class Group>
class Broadcast
  {
  public:

    class BroadcastActorFx_Packet
      {
      public:
        // Might want to shake of the type in the following statement...
        Broadcast<Element, BroadcastOp, Group> *mBroadcastContext;
        int                               mSeqNo;
        Element                           mElement;
        unsigned                          mSource;
      };

    class BroadcastAnchor
       {
       public:
         int        mRootLogicalNodeId;    // for debugging... shouldn't be need in the long run
         int        mChildLogicalNodeId;   // Address space number of parent - now set to root by everyone ('cept root.)
//         int        mCurrentWriteCount;      // Dynamic - curret write count on buffer to be ready to forward
//         int        mRequiredWriteCount;       // Number of writes required for a buffer to be ready
         int        mLockVar;               // Used to lock this structure during access.

         BroadcastActorFx_Packet mBroadcastPacket;

         BroadcastAnchor( Broadcast<Element, BroadcastOp, Group> *aBroadcastContext )
           {
           mRootLogicalNodeId    = -1;
           mChildLogicalNodeId  = -1;
//           mCurrentWriteCount    = 0;
//           mRequiredWriteCount   = 0;
           mLockVar              = 0;

           mBroadcastPacket.mBroadcastContext = aBroadcastContext;
           mBroadcastPacket.mSeqNo                               = -1;
           mBroadcastPacket.mSource =  PhysicalTopology::AddressSpaceSelf();
           BroadcastOp::Identity( mBroadcastPacket.mElement );
           }
       };

    // When declared, this is the only member that is instantiated
    // on construction.  The body of the anchor is instaniated on
    // Attach().
    BroadcastAnchor  *mAnchor;
    AtomicInteger     mAtomicConnectorCount;

    Broadcast()
      {
      mAnchor               = NULL;
      mAtomicConnectorCount.AtomicSet( 0 );
      }

    void
    Attach( int             aRootLogicalNodeId,
            const Group&    aGroup,
            int             aLocalReaderCount = 1 ) // How many threads to read.
      {
      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::Attach: starting "
        << " Anchor@ " << this
        << " Root "
        << aRootLogicalNodeId
        << " TotalReaders "
        << aGroup.size()
        << EndLogLine;
      int i;
      for( i = 0; i < aGroup.size(); i++ )
        {
        BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::Attach: Group Is: "
        << " Anchor@ " << this
        << "Group Index " << i
        << " NodeId "
        << aGroup[i]
        << " This NodeId is : "
        << PhysicalTopology::AddressSpaceSelf()
        << EndLogLine;
        }

      assert( aGroup.IsAMember( aRootLogicalNodeId ) );
      assert( aGroup.IsAMember( PhysicalTopology::AddressSpaceSelf() ) );

      assert( mAnchor == NULL ); //THREAD Assume (ha) that only one thread will attach.

      //NEED a class lock to make this safe if multiple threads do attach.

      BroadcastAnchor *newAnchor = new BroadcastAnchor( this );
      assert( newAnchor != NULL );

      for( i = 0; i < aGroup.size(); i++ )
        {
        if( aGroup[i] == PhysicalTopology::AddressSpaceSelf() )
           break;
        }
      assert( i < aGroup.size() );  // Executing task address space MUST be a member

      //This is where Howard Ho's stuff should be called.

      // Figure out parent and child of this context - this is a ring broadcast topology
      newAnchor->mRootLogicalNodeId    = aRootLogicalNodeId;
      newAnchor->mChildLogicalNodeId           = aGroup[ (i + 1) % aGroup.size() ];
      int ParentLogicalNodeId  = aGroup[ ((i+aGroup.size()) - 1) % aGroup.size() ];

      // Root and Leaf are relative to the tree concept of broadcasts and broadcasts
      int RootFlag   = PhysicalTopology::AddressSpaceSelf() == aRootLogicalNodeId;
      int LeafFlag   = ParentLogicalNodeId                  == aRootLogicalNodeId;  // pointing to root in a ring is being a leaf

      if( LeafFlag )
        newAnchor->mChildLogicalNodeId = -1;

      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::Attach:  "
        << " Anchor@ " << this
        << " Setting mAnchor = newAnchor "
        << EndLogLine;

      ExportFence() ;    // Make sure the data goes out before control value
      mAnchor = newAnchor; //NEED to check sync... might need clear lock to issue isync inst.

      if( ! RootFlag )
        {
        BegLogLine( PKFXLOG_BROADCAST )
          << "Broadcast::Attach: NOT ROOT "
          << " Anchor@ " << this
          << " Doing AtomicAdd to Node "
          << ParentLogicalNodeId
          << EndLogLine;
        mAtomicConnectorCount.AtomicAdd( ParentLogicalNodeId, 1 );
        }


      if( LeafFlag )
        {
        //  Tree Leaves in a broadcast doesn't have to wait here.
        BegLogLine( PKFXLOG_BROADCAST )
          << "Broadcast::Attach: LEAF "
          << " Anchor@ " << this
          << " No Children, so no wait for connectors. "
          << EndLogLine;
        mAtomicConnectorCount.AtomicAdd( 1 );
        }
      else
        {
        // Wait for upstream neighbors to signal ready.
        if( mAtomicConnectorCount.SafeFetch() != 1 )
          {
          BegLogLine( PKFXLOG_BROADCAST )
            << "Broadcast::Attach: PARENT "
            << " Anchor@ " << this
            << " Root is " << aRootLogicalNodeId
            << " starting to wait for connectors "
            << EndLogLine;
          while( mAtomicConnectorCount.SafeFetch() != 1 )
             sched_yield();
          BegLogLine( PKFXLOG_BROADCAST )
            << "Broadcast::Attach: PARENT "
            << " Anchor@ " << this
            << " Root is " << aRootLogicalNodeId
            << " finished wait for connectors "
            << EndLogLine;
          }
        }

      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::Attach: done "
        << " Anchor@ " << this
        << " Root "
        << aRootLogicalNodeId
        << " Child "
        << newAnchor->mChildLogicalNodeId
        << " Parent "
        << ParentLogicalNodeId
        << " TotalReaders "
        << aGroup.size()
        << EndLogLine;
      }

    bool AmRoot()
      {
      // could save a flag in broadcast anchor state to avoid this compare.
      int Rc = ( mAnchor->mRootLogicalNodeId == PhysicalTopology::AddressSpaceSelf() );
      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::AmRoot() "
        << " Anchor@ " << this
        << " Returning: " << Rc
        << EndLogLine;
      return( Rc );
      }

    static int
    BroadcastActorFx(
#ifndef BLUEGENE
                      unsigned src,
                      unsigned len,
#endif
                      void *data )
      {
      /// can't do len with BG without paying. assert( len == sizeof( BroadcastActorFx_Packet ) );
      BroadcastActorFx_Packet *rp = (BroadcastActorFx_Packet *) data;

#ifdef BLUEGENE
      unsigned src = rp->mSource;
#endif

      Broadcast<Element,BroadcastOp,Group> *r = rp->mBroadcastContext;

      assert( r->mAnchor != NULL ); // Target anchor MUST be setup by now - this is a major system error.

      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::BroadcastActorFx: starting "
        << " src " << src
        << " Anchor@ " << (void *) r
        << " CurrSeqNo " << r->mAnchor->mBroadcastPacket.mSeqNo
        << " PacketSeqNo " << rp->mSeqNo
        << " child " << r->mAnchor->mChildLogicalNodeId
        << EndLogLine;

      YieldLock( r->mAnchor->mLockVar );

      // Since pull increments seq no, we should always be looking at a match here.

      r->mAnchor->mBroadcastPacket.mSeqNo++;

      assert( r->mAnchor->mBroadcastPacket.mSeqNo == rp->mSeqNo );

      BroadcastOp::Op( r->mAnchor->mBroadcastPacket.mElement, rp->mElement );

      ExportFence() ;    // Make sure the data goes out before control value
      if( r->mAnchor->mChildLogicalNodeId != -1 )
        r->Trigger();

      UnYieldLock( r->mAnchor->mLockVar );

      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::BroadcastActorFx: ending "
        << " src " << src
        << " Anchor@ " << (void *) r
        << " CurrSeqNo " << r->mAnchor->mBroadcastPacket.mSeqNo
        << " PacketSeqNo " << rp->mSeqNo
        << EndLogLine;
      return(0);
      }

    void Trigger()
      {
      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::Trigger(): BroadcastActorFx in  "
        << " ChildLogicalNodeId "
        << mAnchor->mChildLogicalNodeId
        << " Anchor@ " << this
        << EndLogLine;
      PkActiveMsgSubSys::Trigger(
                                   mAnchor->mChildLogicalNodeId,
                                   BroadcastActorFx,
                                   (void *) &mAnchor->mBroadcastPacket,
                                   sizeof( BroadcastActorFx_Packet ) );
      return;
      }


    void
    Push( Element &aElement, int aSeqNo )
      {
      assert( mAnchor != NULL );  // must have set up

      ImportFence() ;    // Make sure the data goes out before control value
      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::Push: Starting "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mBroadcastPacket.mSeqNo " << mAnchor->mBroadcastPacket.mSeqNo
        << " Child " << mAnchor->mChildLogicalNodeId
        << EndLogLine;

      assert( mAnchor->mBroadcastPacket.mSeqNo + 1 == aSeqNo ); // now, must be ok on seqnos

      assert( mAtomicConnectorCount.SafeFetch() == 1 ); // shouldn't get out of attach without this

      // Write local copy, then call Trigger() to move through bcast network.
      mAnchor->mBroadcastPacket.mElement = aElement;
      mAnchor->mBroadcastPacket.mSeqNo   = aSeqNo;

      ExportFence() ;    // Make sure the data goes out before control value
      if( mAnchor->mChildLogicalNodeId != -1 )
        Trigger();

      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::Push: Ending "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mBroadcastPacket.mSeqNo " << mAnchor->mBroadcastPacket.mSeqNo
        << " Child " << mAnchor->mChildLogicalNodeId
        << EndLogLine;

      }

    void WaitPull( Element &aElement, int aSeqNo )
      {
      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::WaitPull: Started "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mBroadcastPacket.mSeqNo " << mAnchor->mBroadcastPacket.mSeqNo
        << EndLogLine;
      if( ! PollPull(aElement, aSeqNo ) )
        {
        BegLogLine( PKFXLOG_BROADCAST )
          << "Broadcast::WaitPull: Waiting "
          << " Anchor@ " << this
          << " aSeqNo " << aSeqNo
          << " mBroadcastPacket.mSeqNo " << mAnchor->mBroadcastPacket.mSeqNo
          << EndLogLine;
        while( ! PollPull( aElement, aSeqNo ) )
          sched_yield();
        }
      BegLogLine( PKFXLOG_BROADCAST )
        << "Broadcast::WaitPull: Finished "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mBroadcastPacket.mSeqNo " << mAnchor->mBroadcastPacket.mSeqNo
        << EndLogLine;
      }

    int PollPull( Element &aElement, int aSeqNo )
      {
      static int LastSeqNo = 0xFFFFFFFF;

      assert( mAnchor != NULL );

      // Assert requires a local copy lest seq no change between tests.
      int LocalCopy_PacketSeqNo = mAnchor->mBroadcastPacket.mSeqNo;
      assert(   (LocalCopy_PacketSeqNo == aSeqNo)
             || (LocalCopy_PacketSeqNo == (aSeqNo - 1)) );

      // Check if the requirements of SeqNo and Writes are met to return data.
      if( mAnchor->mBroadcastPacket.mSeqNo == aSeqNo  )
        {
        ImportFence() ;    // Make sure the data goes out before control value
        aElement = mAnchor->mBroadcastPacket.mElement;

        ExportFence() ;    // Make sure the data goes out before control value
        BegLogLine( PKFXLOG_BROADCAST )
          << "Broadcast::PollPull: Succeeded. "
          << " Anchor@ " << this
          << " aSeqNo " << aSeqNo
          << " mSeqNo " << mAnchor->mBroadcastPacket.mSeqNo
          << EndLogLine;
        return(1); // Succeeded
        }

      if( LastSeqNo != aSeqNo )
        {
        LastSeqNo = aSeqNo;
        BegLogLine( PKFXLOG_BROADCAST )
          << "Broadcast::PollPull: Failed. "
          << " Anchor@ " << this
          << " aSeqNo " << aSeqNo
          << " mBroadcastPacket.mSeqNo " << mAnchor->mBroadcastPacket.mSeqNo
          << EndLogLine;
        }

      return(0);   // Failed
      }


  };



#endif
