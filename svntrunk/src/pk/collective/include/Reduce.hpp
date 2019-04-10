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
 * Module:          Reduce.hpp
 *
 * Purpose:         Data element reduce with operation.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         032900 BGF Created.
 *
 ***************************************************************************/

#ifndef _REDUCE_HPP_
#define _REDUCE_HPP_

#include <pk/memfence.hpp>
#include "MemoryOps.hpp"

#ifndef PKFXLOG_PKREDUCE
#define PKFXLOG_PKREDUCE          (0)
#endif

template<class Element, class ReduceOp, class Group>
class Reduce
  {
  public:

    class ReduceActorFx_Packet
      {
      public:
        // Might want to shake of the type in the following statement...
        Reduce<Element, ReduceOp, Group> *mReduceContext;
        int                               mSeqNo;
        Element                           mElement;
#ifdef BLUEGENE
        unsigned mSource;
#endif
      };

    class ReduceAnchor
       {
       public:
         int        mRootLogicalNodeId;    // for debugging... shouldn't be need in the long run
         int        mParentLogicalNodeId;   // Address space number of parent - now set to root by everyone ('cept root.)
         int        mCurrentWriteCount;      // Dynamic - curret write count on buffer to be ready to forward
         int        mRequiredWriteCount;       // Number of writes required for a buffer to be ready
         int        mLockVar;               // Used to lock this structure during access.

         ReduceActorFx_Packet mReducePacket;

       ReduceAnchor( Reduce<Element, ReduceOp, Group> *aReduceContext )
         {
         mRootLogicalNodeId    = -1;
         mParentLogicalNodeId  = -1;
         mCurrentWriteCount    = 0;
         mRequiredWriteCount   = 0;
         mLockVar              = 0;

         mReducePacket.mReduceContext = aReduceContext;
         mReducePacket.mSeqNo         = -1;
         mReducePacket.mSource        = PhysicalTopology::AddressSpaceSelf();
         ReduceOp::Identity( mReducePacket.mElement );
         }
       };

    // When declared, this is the only member that is instantiated
    // on construction.  The body of the anchor is instaniated on
    // Attach().
    ReduceAnchor     *mAnchor;
    AtomicInteger     mAtomicConnectorCount;

    Reduce()
      {
      mAnchor               = NULL;
      mAtomicConnectorCount.AtomicSet( 0 );
      }

    void
    Attach( int             aRootLogicalNodeId,
            const Group&    aGroup,
            int             aLocalReaderCount = 1 ) // How many threads to read.
      {
      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::Attach: starting "
        << " Anchor@ " << this
        << " Root "
        << aRootLogicalNodeId
        << " TotalReaders "
        << aGroup.size()
        << EndLogLine;
      int i;
      for( i = 0; i < aGroup.size(); i++ )
        {
        BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::Attach: Group Is: "
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

      ReduceAnchor *newAnchor = new ReduceAnchor( this );
      assert( newAnchor != NULL );

      for( i = 0; i < aGroup.size(); i++ )
        {
        if( aGroup[i] == PhysicalTopology::AddressSpaceSelf() )
           break;
        }
      assert( i < aGroup.size() );  // Executing task address space MUST be a member

      //This is where Howard Ho's stuff should be called.

      // Figure out parent and child of this context - this is a ring reduce topology
      newAnchor->mRootLogicalNodeId    = aRootLogicalNodeId;
      int ChildLogicalNodeId           = aGroup[ (i + 1) % aGroup.size() ];
      newAnchor->mParentLogicalNodeId  = aGroup[ ((i+aGroup.size()) - 1) % aGroup.size() ];

      // Root and Leaf are relative to the tree concept of broadcasts and reduces
      int RootFlag   = PhysicalTopology::AddressSpaceSelf() == aRootLogicalNodeId;
      int LeafFlag   = ChildLogicalNodeId                   == aRootLogicalNodeId;  // pointing to root in a ring is being a leaf


      if( RootFlag )
        {
        newAnchor->mParentLogicalNodeId = aRootLogicalNodeId;  // Reduce root sends contribution to self.
        }

      // Leaves need only the local push, everyone else needs parent + local
      if( LeafFlag )
        newAnchor->mRequiredWriteCount = 1;
      else
        newAnchor->mRequiredWriteCount = 2;

      ExportFence() ;    // Make sure the data goes out before control value
      mAnchor = newAnchor; //NEED to check sync... might need clear lock to issue isync inst.

      // Now that anchor is in place, send read to parent in reduce tree.
      if( ! LeafFlag )
        {
        mAtomicConnectorCount.AtomicAdd( ChildLogicalNodeId, 1 );
        }

      if( RootFlag )
        {
        // Root in a reduce doesn't have to wait here.
        BegLogLine( PKFXLOG_PKREDUCE )
          << "Reduce::Attach: ROOT "
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
          BegLogLine( PKFXLOG_PKREDUCE )
            << "Reduce::Attach: CHILD "
            << " Anchor@ " << this
            << " Root is " << aRootLogicalNodeId
            << " starting to wait for connectors "
            << EndLogLine;
          while( mAtomicConnectorCount.SafeFetch() != 1 )
             sched_yield();
          BegLogLine( PKFXLOG_PKREDUCE )
            << "Reduce::Attach: CHILD "
            << " Anchor@ " << this
            << " Root is " << aRootLogicalNodeId
            << " finished wait for connectors "
            << EndLogLine;
          }
        }

      ImportFence();
      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::Attach: done "
        << " Anchor@ " << this
        << " Root "
        << aRootLogicalNodeId
        << " Child "
        << ChildLogicalNodeId
        << " Parent "
        << newAnchor->mParentLogicalNodeId
        << " TotalReaders "
        << " PktSeqNo "
        << newAnchor->mReducePacket.mSeqNo
        << aGroup.size()
        << EndLogLine;
      }

    bool AmRoot()
      {
      // could save a flag in reduce anchor state to avoid this compare.
      int Rc = ( mAnchor->mRootLogicalNodeId == PhysicalTopology::AddressSpaceSelf() );
      BegLogLine( 0 )   // We poll on this, so we'll turn of most of the time.
        << "Reduce::AmRoot() "
        << " Anchor@ " << this
        << " Returning: " << Rc
        << EndLogLine;
      return( Rc );
      }

    static int
    ReduceActorFx(
#ifndef BLUEGENE
                    unsigned src,
                    unsigned len,
#endif
                    void *data )
      {
      ReduceActorFx_Packet *rp = (ReduceActorFx_Packet *) data;
#ifdef BLUEGENE
      unsigned src = rp->mSource;
#else
      assert( len == sizeof( ReduceActorFx_Packet ) );
#endif

      Reduce<Element,ReduceOp,Group> *r = rp->mReduceContext;

      assert( r->mAnchor != NULL ); // Target anchor MUST be setup by now - this is a major system error.

      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::ReduceActorFx: starting "
        << " src " << src
        << " Anchor@ " << (void *) r
        << " PktSeqNO " << rp->mSeqNo
        << " LocalSeqNo " << r->mAnchor->mReducePacket.mSeqNo
        << EndLogLine;

      r->Push( rp->mElement, rp->mSeqNo );

      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::ReduceActorFx: ending "
        << " src " << src
        << " Anchor@ " << (void *) r
        << " PktSeqNO " << rp->mSeqNo
        << " LocalSeqNo " << r->mAnchor->mReducePacket.mSeqNo
        << EndLogLine;
      return(0);
      }

    void Trigger()
      {
      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::Trigger(): ReduceActorFx in  "
        << mAnchor->mParentLogicalNodeId
        << " Anchor@ " << this
        << " Dest "  << mAnchor->mParentLogicalNodeId
        << " PktSeqNo " << mAnchor->mReducePacket.mSeqNo
        << EndLogLine;
      PkActiveMsgSubSys::Trigger(
                                   mAnchor->mParentLogicalNodeId,
                                   ReduceActorFx,
                                   (void *) &mAnchor->mReducePacket,
                                   sizeof( ReduceActorFx_Packet ) );
      return;
      }


    void
    Push( Element &aElement, int aSeqNo )
      {
      assert( mAnchor != NULL );  // must have set up

      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::Push: Starting "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mReducePacket.mSeqNo " << mAnchor->mReducePacket.mSeqNo
        << EndLogLine;

      assert( mAtomicConnectorCount.SafeFetch() == 1 ); // shouldn't get out of attach without this

      YieldLock( mAnchor->mLockVar );
      ImportFence();

      int LocalCopy_PacketSeqNo = mAnchor->mReducePacket.mSeqNo;
      assert(    LocalCopy_PacketSeqNo     == aSeqNo    // this is a repetitive call for thie seqno
              || LocalCopy_PacketSeqNo + 1 == aSeqNo ); // this is the first call for this seqno

      int LocalCopy_WriteCount = mAnchor->mCurrentWriteCount;
      assert(   LocalCopy_WriteCount >= 0
             && LocalCopy_WriteCount <= mAnchor->mRequiredWriteCount );


      ReduceOp::Op( mAnchor->mReducePacket.mElement, aElement );
      mAnchor->mReducePacket.mSeqNo = aSeqNo;

      mAnchor->mCurrentWriteCount++;

      if( mAnchor->mCurrentWriteCount == mAnchor->mRequiredWriteCount )
        {
        if( ! AmRoot() )
          {
          Trigger();
          mAnchor->mCurrentWriteCount = 0;
          }
        }

      ExportFence() ;    // Make sure the data goes out before control value
      UnYieldLock( mAnchor->mLockVar );

      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::Push: Ending "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mReducePacket.mSeqNo " << mAnchor->mReducePacket.mSeqNo
        << EndLogLine;

      }

    void WaitPull( Element &aElement, int aSeqNo )
      {
      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::WaitPull: Started "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mReducePacket.mSeqNo " << mAnchor->mReducePacket.mSeqNo
        << EndLogLine;
      if( ! PollPull(aElement, aSeqNo ) )
        {
        BegLogLine( PKFXLOG_PKREDUCE )
          << "Reduce::WaitPull: Waiting "
          << " Anchor@ " << this
          << " aSeqNo " << aSeqNo
          << " mReducePacket.mSeqNo " << mAnchor->mReducePacket.mSeqNo
          << EndLogLine;
        while( ! PollPull( aElement, aSeqNo ) )
          sched_yield();
        }
      BegLogLine( PKFXLOG_PKREDUCE )
        << "Reduce::WaitPull: Finished "
        << " Anchor@ " << this
        << " aSeqNo " << aSeqNo
        << " mReducePacket.mSeqNo " << mAnchor->mReducePacket.mSeqNo
        << EndLogLine;
      }

    int PollPull( Element &aElement, int aSeqNo )
      {
      static int LastSeqNo = 0xFFFFFFFF;

      assert( mAnchor != NULL );
      assert( AmRoot() );
      assert(   mAnchor->mCurrentWriteCount >= 0
             && mAnchor->mCurrentWriteCount <= mAnchor->mRequiredWriteCount );
      assert(   mAnchor->mReducePacket.mSeqNo == aSeqNo
             || mAnchor->mReducePacket.mSeqNo == aSeqNo - 1 );


      ImportFence() ;    // Make sure the data goes out before control value

      // Check if the requirements of SeqNo and Writes are met to return data.
      if(     mAnchor->mCurrentWriteCount   == mAnchor->mRequiredWriteCount
          &&  mAnchor->mReducePacket.mSeqNo == aSeqNo  )
        {
        aElement = mAnchor->mReducePacket.mElement;

        mAnchor->mCurrentWriteCount = 0; // done here for root
        ExportFence();

        BegLogLine( PKFXLOG_PKREDUCE )
          << "Reduce::PollPull: Succeeded. "
          << " Anchor@ " << this
          << " aSeqNo " << aSeqNo
          << " mSeqNo " << mAnchor->mReducePacket.mSeqNo
          << EndLogLine;
        return(1); // Succeeded
        }

      if( LastSeqNo != aSeqNo )
        {
        LastSeqNo = aSeqNo;
        BegLogLine( PKFXLOG_PKREDUCE )
          << "Reduce::PollPull: Failed. "
          << " Anchor@ " << this
          << " aSeqNo " << aSeqNo
          << " mReducePacket.mSeqNo " << mAnchor->mReducePacket.mSeqNo
          << EndLogLine;
        }

      return(0);   // Failed
      }


  };



#endif
