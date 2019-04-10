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
 #ifndef __KSPACE_COMM_MANAGER_IF_HPP__
#define __KSPACE_COMM_MANAGER_IF_HPP__

#ifndef SORT_SITE_LIST_WITH_QSORT
#define SORT_SITE_LIST_WITH_QSORT (1)
#endif

#include <rptree.hpp>

class KspaceInteractionStateManagerIF 
  {
  public:

      tVerletSiteInfo* mKspaceVerletList                 MEMORY_ALIGN( 5 );
      int              mKspaceVerletListCount            ;//MEMORY_ALIGN( 5 ); 
      int              mKspaceVerletListAllocatedCount   ;//MEMORY_ALIGN( 5 );
    
  FragmentRep*     mSiteIdToFragmentRepMap           MEMORY_ALIGN( 5 );

      int           mNumberOfSites                       ;//MEMORY_ALIGN( 5 );     
      XYZ**         mSiteIdToPosForcePtrMap              ;//MEMORY_ALIGN( 5 );
 
  void Init( int aNumberOfSites, 
             FragmentRep* aSiteIdToFragmentRepMap,
             int          aNumberOfAtomPerNode )
    {
    mSiteIdToFragmentRepMap = aSiteIdToFragmentRepMap;
    mNumberOfSites = aNumberOfSites;
    
    mSiteIdToPosForcePtrMap = (XYZ **) malloc( sizeof( XYZ *) * mNumberOfSites + CACHE_ISOLATION_PADDING_SIZE );
    if( mSiteIdToPosForcePtrMap == NULL )
      PLATFORM_ABORT( "mSiteIdToPosForcePtrMap == NULL" );
    
    for( int i = 0; i < mNumberOfSites; i++ )
      mSiteIdToPosForcePtrMap[ i ] = NULL;

    mKspaceVerletListAllocatedCount = aNumberOfAtomPerNode;
    
    mKspaceVerletListCount = 0;
    mKspaceVerletList = (tVerletSiteInfo *) malloc( sizeof( tVerletSiteInfo ) * 
                                                    mKspaceVerletListAllocatedCount 
                                                    + CACHE_ISOLATION_PADDING_SIZE );
    if( mKspaceVerletList == NULL )
      PLATFORM_ABORT( "mKspaceVerletList == NULL" );
    }

    void SortSiteList()
    {
#if SORT_SITE_LIST_WITH_QSORT
//    	Platform::Algorithm::Qsort(mKspaceVerletList,mKspaceVerletListCount,sizeof(tVerletSiteInfo),qsortComparetVerletSiteInfo);
//    	Platform::Algorithm::Rsort(mKspaceVerletList,mKspaceVerletListCount,sizeof(tVerletSiteInfo),0,2*sizeof(int));
//      RadixSort(mKspaceVerletList,mKspaceVerletListCount,2*sizeof(int)) ;
      RadixSort<tVerletSiteInfo,unsigned int,2,0>(mKspaceVerletList,mKspaceVerletListCount) ;
#else    	
      std::sort( mKspaceVerletList, mKspaceVerletList + mKspaceVerletListCount );
#endif      
    }
      
  void 
  AddSiteToList( int aSiteId )
    {
    if( mKspaceVerletListCount >= mKspaceVerletListAllocatedCount )
      ExpandVerletList( & mKspaceVerletList, 
                        mKspaceVerletListCount, 
                        mKspaceVerletListAllocatedCount );
    
    assert( aSiteId >= 0 && aSiteId < mNumberOfSites );
    int fragId = mSiteIdToFragmentRepMap[ aSiteId ].mFragId;
    int offset = mSiteIdToFragmentRepMap[ aSiteId ].mOffset;
    
    mKspaceVerletList[ mKspaceVerletListCount ].fragid = fragId;
    mKspaceVerletList[ mKspaceVerletListCount ].siteind = offset;
    mKspaceVerletList[ mKspaceVerletListCount ].charge = MSD_IF::GetSiteInfo( aSiteId ).charge;
    mKspaceVerletListCount++;
    }

  tVerletSiteInfo*
  GetKspaceVerletList()
    {
    return mKspaceVerletList;
    }

  int
  GetKspaceVerletListCount()
    {
    return mKspaceVerletListCount;
    }

  void
  ExpandVerletList( tVerletSiteInfo** aList, int aLen, int &aAllocatedCount )
    {
#ifdef PK_BGL
    if( Platform::Thread::GetId() == 1 )
      {
          BegLogLine( 1 )
              << "ERROR: Ohhh NOOOOOO"
              << EndLogLine;          

      char* Abort = NULL;
      Abort[ 0 ] = 0;
      }
#endif

    void * Memory;
    
    int NewCount = aAllocatedCount + 1024;
    int NewSize  = NewCount * sizeof( tVerletSiteInfo ) + CACHE_ISOLATION_PADDING_SIZE;
    
    Memory = malloc( NewSize );
    
    if( Memory == NULL )
      PLATFORM_ABORT( "Memory == NULL" );
    
    if( *aList != NULL )
      {
      memcpy( Memory, *aList, aLen * sizeof( tVerletSiteInfo ) );
      free( *aList );
      }
    
    *aList = (tVerletSiteInfo *) Memory;
    aAllocatedCount = NewCount;
    }
      
  XYZ
  GetPositionAndClearForForce( int aSiteId )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    XYZ rc = *(mSiteIdToPosForcePtrMap[ aSiteId ]);    
    mSiteIdToPosForcePtrMap[ aSiteId ]->Zero();
    return rc;
    }

  XYZ
  GetPositionAndClearForForce( int aFragId, int aOrdinal )
    {
    int siteId =  MSD_IF::GetIrreducibleFragmentFirstSiteId( aFragId ) + aOrdinal;
    assert( siteId >= 0 && siteId < NUMBER_OF_SITES );

    XYZ rc = *(mSiteIdToPosForcePtrMap[ siteId ]);
    mSiteIdToPosForcePtrMap[ siteId ]->Zero();
    return rc;
    }

  void
  ClearForForce( int aFragId, int aOrdinal )
    {
    int siteId =  MSD_IF::GetIrreducibleFragmentFirstSiteId( aFragId ) + aOrdinal;
    assert( siteId >= 0 && siteId < NUMBER_OF_SITES );

    mSiteIdToPosForcePtrMap[ siteId ]->Zero();
    }

  XYZ 
  GetPosition( int aSiteId )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    XYZ rc = *(mSiteIdToPosForcePtrMap[ aSiteId ]);
    return rc;
    }

  XYZ 
  GetPosition( int aFragId, int aOrdinal )
    {
    int siteId =  MSD_IF::GetIrreducibleFragmentFirstSiteId( aFragId ) + aOrdinal;

    assert( siteId >= 0 && siteId < NUMBER_OF_SITES );

    XYZ rc = *(mSiteIdToPosForcePtrMap[ siteId ]);
    return rc;
    }
      
  void 
  SetPositionPtr( int aSiteId, XYZ* aPosPtr )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    mSiteIdToPosForcePtrMap[ aSiteId ] = aPosPtr;
    }
      
  XYZ&
  GetForce( int aSiteId )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    XYZ & rc = *(mSiteIdToPosForcePtrMap[ aSiteId ]);
    return rc;
    }
      
  void
  AddForce( int aSiteId, XYZ& aForce )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );
    *(mSiteIdToPosForcePtrMap[ aSiteId ]) += aForce;
    }

  void
  AddForce( int aFragId, int aOrdinal, XYZ& aForce )
    {
    int siteId =  MSD_IF::GetIrreducibleFragmentFirstSiteId( aFragId ) + aOrdinal;

    assert( siteId >= 0 && siteId < NUMBER_OF_SITES );
    *(mSiteIdToPosForcePtrMap[ siteId ]) += aForce;
    }
  };

static void P3ME_GetNodeIDList( const XYZ& vmin, const XYZ& vmax, int idlist[], int& idnum );

class KspaceCommManagerIF
{
public:
    IntegratorStateManagerIF*        mIntegratorStateManagerIF MEMORY_ALIGN( 5 );
    
    XYZ                              mCenterOfCentralCell      MEMORY_ALIGN( 5 );

    struct MessageElement
      {
      int mSiteId;
      XYZ mForceOrPos;
      }; 

    struct ActiveNodeHelper
      {
      int            mDestNodeId;
      _BGL_TorusPkt  mBGL_TorusPkt;
      };

    int*                  mNodeIdTemp ;//MEMORY_ALIGN( 5 );
    
    int*                            mActiveNodes ;//MEMORY_ALIGN( 5 );
    int                             mActivePacketCount; // MEMORY_ALIGN( 5 );
    ActiveNodeHelper*              mActivePackets ;//MEMORY_ALIGN( 5 );
    
    int                             mNumberOfPacketsSent; // MEMORY_ALIGN( 5 );

    struct KSpacePacketPayload
      {
      int               mElementCount;
      MessageElement    mElements[ 1 ];
      };

    int                 mRecvBufferSize ; // MEMORY_ALIGN( 5 );
    _BGL_TorusPkt*      mRecvBuffer ; // MEMORY_ALIGN( 5 );

    _BGL_TreeHwHdr      mTreeHdr ; // MEMORY_ALIGN( 5 );    
    _BGL_TreeHwHdr      mTreeHdrDummy ;// MEMORY_ALIGN( 5 );    
    _BGL_TreePayload    mTreeSendPayload ; // MEMORY_ALIGN( 5 );
    _BGL_TreePayload    mTreeRecvPayload ; // MEMORY_ALIGN( 5 );

    KspaceInteractionStateManagerIF* mKspaceInteractionStateManagerIFPtr ;//MEMORY_ALIGN( 5 );

    int                 mTotalRecvPacketCount ; // MEMORY_ALIGN( 5 );

    int mNumberOfSites; // MEMORY_ALIGN( 5 );
    int mMachineSize; //  MEMORY_ALIGN( 5 );

    int mCoreId ; //MEMORY_ALIGN( 5 );

#define CURRENT_PAYLOAD_SIZE (( KSpacePayloadPtr->mElementCount == 0 ) ? (sizeof( KSpacePacketPayload )) : \
                              (((KSpacePayloadPtr->mElementCount-1) * sizeof( MessageElement )) + sizeof( KSpacePacketPayload ) ))

    void Init( IntegratorStateManagerIF*        aIntegratorStateManagerIF, 
               KspaceInteractionStateManagerIF* aKspaceInteractionStateManagerIFPtr,
               XYZ                              aCenterOfCentralCell,
               FragmentRep*                     aSiteIdToFragmentRepMap,
               int                              aNumberOfSites,
               XYZ                              aBoundingBoxDim )
      {
      // mCoreId = Platform::Thread::GetId();
      mCoreId = 1;

      mKspaceInteractionStateManagerIFPtr = aKspaceInteractionStateManagerIFPtr;
      mIntegratorStateManagerIF = aIntegratorStateManagerIF;
      mNumberOfSites             = aNumberOfSites;

      mCenterOfCentralCell = aCenterOfCentralCell;
      
      mMachineSize = Platform::Topology::GetAddressSpaceCount();
      mNodeIdTemp = (int *) malloc( sizeof( int ) * mMachineSize + CACHE_ISOLATION_PADDING_SIZE );
      if( mNodeIdTemp == NULL )
        PLATFORM_ABORT( "mNodeIdTemp == NULL" );

      mActiveNodes = (int *) malloc( sizeof( int ) * mMachineSize + CACHE_ISOLATION_PADDING_SIZE );
      if( mActiveNodes == NULL )
        PLATFORM_ABORT( "mNodeIdTemp == NULL" );

      mActivePacketCount = 0;
      mActivePackets = (ActiveNodeHelper *) malloc( sizeof( ActiveNodeHelper ) * mMachineSize + CACHE_ISOLATION_PADDING_SIZE);
       if( mActivePackets == NULL )
         PLATFORM_ABORT( "mNodeIdTemp == NULL" );

      for( int i = 0; i < mMachineSize; i++ )
        {
        mActiveNodes[ i ] = -1;
        }
      
      int procsX, procsY, procsZ;
      Platform::Topology::GetDimensions( &procsX, &procsY, &procsZ );
      
      XYZ LocalDim;
      LocalDim.mX = ( aBoundingBoxDim.mX / procsX ) + 4.0;
      LocalDim.mY = ( aBoundingBoxDim.mY / procsY ) + 4.0;
      LocalDim.mZ = ( aBoundingBoxDim.mZ / procsZ ) + 4.0;
      
      double LocalVol = LocalDim.mX * LocalDim.mY * LocalDim.mZ;
      double BoxVol   = aBoundingBoxDim.mX * aBoundingBoxDim.mY * aBoundingBoxDim.mZ;
      
      double ratio = LocalVol / BoxVol;
      
      int NumberOfAtomPerNode = 5 * (int) ( NUMBER_OF_SITES * ratio );
      
      mRecvBufferSize = sizeof( _BGL_TorusPkt ) * (int ) ( NumberOfAtomPerNode );
      mRecvBuffer = (_BGL_TorusPkt *) malloc( mRecvBufferSize + CACHE_ISOLATION_PADDING_SIZE );
      if( mRecvBuffer == NULL )
        PLATFORM_ABORT( "mRecvBuffer == NULL" );

      BGL_TreeMakeCollectiveHdr( &mTreeHdr,
                                1,
                                0,
                                _BGL_TREE_OPCODE_ADD, // Add
                                _BGL_TREE_OPSIZE_BIT32,
                                999,
                                _BGL_TREE_CSUM_NONE );

      mKspaceInteractionStateManagerIFPtr->Init( aNumberOfSites, 
                                                 aSiteIdToFragmentRepMap,
                                                 NumberOfAtomPerNode );
      }

    int AddToPacket( int aNodeId, int aSiteId, XYZ & aPos )
      {
      assert( aNodeId >= 0 && aNodeId < mMachineSize );
      int PacketIndex = mActiveNodes[ aNodeId ];
      if( PacketIndex == -1 )
        {
        // Starting a new node
        assert( mActivePacketCount >= 0 && mActivePacketCount < mMachineSize );
//         mActivePackets[ mActivePacketCount ] = (ActiveNodeHelper *) malloc( sizeof( ActiveNodeHelper ) );

//         if( mActivePackets[ mActivePacketCount ] == NULL )
//           PLATFORM_ABORT( "mActivePackets[ mActivePacketCount ] == NULL" );

        mActivePackets[ mActivePacketCount ].mDestNodeId = aNodeId;
        KSpacePacketPayload* KSpacePayloadPtr = (KSpacePacketPayload *) & mActivePackets[ mActivePacketCount ].mBGL_TorusPkt.payload;
        KSpacePayloadPtr->mElementCount = 0;

        mActiveNodes[ aNodeId ] = mActivePacketCount;
        PacketIndex = mActiveNodes[ aNodeId ];
        mActivePacketCount++;

//         BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
//           << "mActivePacketCount: " << mActivePacketCount 
//           << EndLogLine;
        }
      
      assert( PacketIndex >= 0 && PacketIndex < mMachineSize );
      _BGL_TorusPkt* PacketToFill = &mActivePackets[ PacketIndex ].mBGL_TorusPkt;

      KSpacePacketPayload* KSpacePayloadPtr = (KSpacePacketPayload *) & PacketToFill->payload;

      if( (CURRENT_PAYLOAD_SIZE + sizeof( MessageElement )) >
           sizeof( _BGL_TorusPktPayload ) )
        {
        // Send out the current packet and fill in this packet
        if( aNodeId == Platform::Topology::GetAddressSpaceId() )
          {
          PacketToFill->hdr.swh0.arg0 = aNodeId;

          memcpy( &mRecvBuffer[ mTotalRecvPacketCount ], PacketToFill, sizeof( _BGL_TorusPkt ) );
          mTotalRecvPacketCount++;
          mNumberOfPacketsSent++;
          KSpacePayloadPtr->mElementCount = 0;
          }        
        else if( BGLTorusTryToSendPacket( mCoreId, PacketToFill, CURRENT_PAYLOAD_SIZE, aNodeId ) )
          {
          KSpacePayloadPtr->mElementCount = 0;
          mNumberOfPacketsSent++;
          }
        else
          {
          // The packet was not sent, try to send it again
          return 0;
          }
        }
      
      KSpacePayloadPtr->mElements[ KSpacePayloadPtr->mElementCount ].mSiteId     = aSiteId;
      KSpacePayloadPtr->mElements[ KSpacePayloadPtr->mElementCount ].mForceOrPos = aPos;
      KSpacePayloadPtr->mElementCount++;
      return 1;
      }
    
    void ExpandRecvBuffer( int aCurrentBufferCount )
      {
#ifdef PK_BGL
    if( Platform::Thread::GetId() == 1 )
      {
      BegLogLine( 1 )
        << "ERROR: "
        << EndLogLine;

      char* Abort = NULL;
      Abort[ 0 ] = 0;
      }
#endif

      void * Memory;
      
      int NewCount = aCurrentBufferCount + 200;
      int NewSize = NewCount * sizeof( _BGL_TorusPkt ) + CACHE_ISOLATION_PADDING_SIZE;
      
      Memory = malloc( NewSize );
      if( Memory == NULL )
        PLATFORM_ABORT("Memory == NULL");
      
      memcpy( Memory, mRecvBuffer, aCurrentBufferCount * sizeof( _BGL_TorusPkt ) );
      free( mRecvBuffer );
      mRecvBuffer = (_BGL_TorusPkt *) Memory;
      mRecvBufferSize = NewSize;
      }

    void BroadcastExecute( int aSimTick )
      {
      //BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
      BegLogLine( 0 )
        << SimTick
        << " Entering BroadcastExecute"
        << EndLogLine;

      mNumberOfPacketsSent = 0;
      mTotalRecvPacketCount = 0;

      mKspaceInteractionStateManagerIFPtr->mKspaceVerletListCount = 0;
 
      int NumberOfKspaceNodesForSite = 0;
      int RecvDone = 0;
      int SendDone = 0;      
      
      int CurPacketToSend = 0;
      
      int CurFragIndex = 0;
      int GotSiteToAdd = 0;
      
      int CurSiteId = -1;
      int CurNodeId = -1;
      int CurSiteIndex = 0;
      int CurNodeIndex = 0;

      int GlobalSendPkts = 0;
      int GlobalRecvPkts = 0;

      _BGL_TreeFifoStatus stat;

      int DoReduce = 1;

      // Recv is done when every one on the network sent and received their packets
      while( ! RecvDone )
        {
        for( ; CurFragIndex < mIntegratorStateManagerIF->GetNumberOfLocalFragments();  )
          {
          FragId fId = mIntegratorStateManagerIF->GetNthFragmentId( CurFragIndex );
          int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fId );
          int FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fId );
          
          for( ; CurSiteIndex < SiteCount; )
            {          
            CurSiteId = FirstSiteId + CurSiteIndex;
            
            XYZ & Pos = mIntegratorStateManagerIF->GetPosition( fId, CurSiteIndex );
            XYZ CurPosImg;
            
            NearestImageInPeriodicVolume( mCenterOfCentralCell, Pos, CurPosImg );
            
            P3ME_GetNodeIDList( CurPosImg, CurPosImg, mNodeIdTemp, NumberOfKspaceNodesForSite );
            
            for( ; CurNodeIndex < NumberOfKspaceNodesForSite;  )
              {
              CurNodeId = mNodeIdTemp[ CurNodeIndex ];
              
              // Add this site to this nodes packet
              if( ! AddToPacket( CurNodeId, CurSiteId, CurPosImg ) )
                {
                // Could not add the packet
                GotSiteToAdd = 1;
                
                goto GoToTarget_PollOnTorusRecvFifo;
                }
              
              CurNodeIndex++;
              }
            
            CurNodeIndex = 0;
            CurSiteIndex++;
            }
          
          CurSiteIndex = 0;
          CurFragIndex++;
          }
        
        // Go over all the remaining active packets and send them out.
        // while( ( CurPacketToSend < mMachineSize ) && mActivePackets[ CurPacketToSend ] != NULL )
  while( CurPacketToSend < mActivePacketCount ) 
          {
          _BGL_TorusPkt* PacketToFill = & mActivePackets[ CurPacketToSend ].mBGL_TorusPkt;
          int DestNode                =   mActivePackets[ CurPacketToSend ].mDestNodeId;

          KSpacePacketPayload* KSpacePayloadPtr = (KSpacePacketPayload *) & PacketToFill->payload;
          
          if( KSpacePayloadPtr->mElementCount != 0 )
            {
            if( DestNode == Platform::Topology::GetAddressSpaceId() )
              {
              PacketToFill->hdr.swh0.arg0 = DestNode;
              
              memcpy( &mRecvBuffer[ mTotalRecvPacketCount ], PacketToFill, sizeof( _BGL_TorusPkt ) );

              KSpacePayloadPtr->mElementCount = 0;
              mTotalRecvPacketCount++;
              mNumberOfPacketsSent++;
              }                
            else if( BGLTorusTryToSendPacket( mCoreId, PacketToFill, CURRENT_PAYLOAD_SIZE, DestNode ) )
              {
              KSpacePayloadPtr->mElementCount = 0;
              mNumberOfPacketsSent++;
              }
            else
              {
              // failed to send, try receiving
              goto GoToTarget_PollOnTorusRecvFifo;
              }
            }
          CurPacketToSend++;
          }
        
        SendDone = 1;
        // Here we're ready to all reduce the number of global packets sent        
        
        if( DoReduce )
          {
          int* TreeInfo = (int *) &mTreeSendPayload;
          TreeInfo[ 0 ] = mNumberOfPacketsSent;
          TreeInfo[ 1 ] = mTotalRecvPacketCount;
          
           BegLogLine( 0 )
             << "About to BGL_TreeRawSendPacket "
             << "mNumberOfPacketsSent: " << mNumberOfPacketsSent
             << "mTotalRecvPacketCount: " << mTotalRecvPacketCount
             << EndLogLine;
          
          BGL_TreeRawSendPacket( 1, &mTreeHdr, &mTreeSendPayload );

          BegLogLine( 0 )
            << "Done BGL_TreeRawSendPacket"
            << EndLogLine;
          }

        GoToTarget_PollOnTorusRecvFifo:
        // See if you can stuff 6 more packets into the buffer
        if( (mTotalRecvPacketCount + 6) * sizeof( _BGL_TorusPkt ) > mRecvBufferSize )
          ExpandRecvBuffer( mTotalRecvPacketCount );
        
        mTotalRecvPacketCount += BGLTorusRecvPacket( mCoreId, (char *) & mRecvBuffer[ mTotalRecvPacketCount ], sizeof( _BGL_TorusPkt ) );
        
        if( SendDone )
          {
          BGL_TreeGetStatus( 1, &stat );
          
          BegLogLine( 0 )
            << "stat.RecHdrCount: " << stat.RecHdrCount
            << " stat.RecPyldCount: " << stat.RecPyldCount
            << EndLogLine;

          if( (stat.RecHdrCount != 0) && (stat.RecPyldCount >= 0x10 ) )
            {
            // We got something
            BGL_TreeRawReceivePacket( 1, &mTreeHdrDummy, &mTreeRecvPayload );
            int* TreeResult = (int *) &mTreeRecvPayload;
            GlobalSendPkts = TreeResult[ 0 ];
            GlobalRecvPkts = TreeResult[ 1 ];
            
            BegLogLine( 0 )
              << "GlobalSendPkts: " << GlobalSendPkts
              << " GlobalRecvPkts: " << GlobalRecvPkts
              << EndLogLine;
            
            if( GlobalRecvPkts == GlobalSendPkts )
              RecvDone = 1;
            
            DoReduce = 1;
            }
          else
            {
            // Nothing came on the tree, wait for it, do not send
            DoReduce = 0;
            }
          }
        }
    
      BegLogLine( 0 )
        << "Done with the bcast loop, about to hoo'm up"
        << EndLogLine;

      // Rundown the list of packets and hook them up.
      for(int RecvIndex = 0; RecvIndex < mTotalRecvPacketCount; RecvIndex++ )
        {
        _BGL_TorusPkt* CurPkt = & mRecvBuffer[ RecvIndex ];
        KSpacePacketPayload* KSpacePayloadPtr = (KSpacePacketPayload *) & CurPkt->payload;
        
//         int OldSourceRank = CurPkt->hdr.swh0.arg0;
//         int OldDestRank   = CurPkt->hdr.swh1.arg;
        
//         BegLogLine( 1 )
//           << "OldSourceRank1: "  << OldSourceRank
//           << " OldDestRank1: "  << OldDestRank
//           << EndLogLine;

        int Count = KSpacePayloadPtr->mElementCount;
        for( int ElemIndex = 0; ElemIndex < Count; ElemIndex++ )
          {
          int siteId =   KSpacePayloadPtr->mElements[ ElemIndex ].mSiteId;
          XYZ * PosPtr  = & KSpacePayloadPtr->mElements[ ElemIndex ].mForceOrPos;
          
          mKspaceInteractionStateManagerIFPtr->SetPositionPtr( siteId, PosPtr );

          mKspaceInteractionStateManagerIFPtr->AddSiteToList( siteId );
          }
        }

      // Need this to be deterministic
      mKspaceInteractionStateManagerIFPtr->SortSiteList();

      // BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
      BegLogLine( 0 )
        << SimTick
        << " Leaving BroadcastExecute"
        << EndLogLine;
      }

    void ReduceExecute( int aSimTick )
      {
      // BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
      BegLogLine( 0 )
        << SimTick
        << " Entering ReduceExecute"
        << EndLogLine;

      // Reverse the direction in the packets and send them all back.
      // Expect to get back the same number of packets as we've sent
      int RecvDone = 0;
      int SendDone = 0;

      int CurrentPacketIndex = 0;
      
      int TotalPacketsRecvOnReduce = 0;

      // There could be at most 6 packets read from fifos
      _BGL_TorusPkt ReduceRecvBuffer[ 6 ];

      BegLogLine( 0 )
        << "Entering ReduceExecute..."
        << EndLogLine;

      mIntegratorStateManagerIF->ZeroForcesCore1();

      RecvDone = ( TotalPacketsRecvOnReduce == mNumberOfPacketsSent );
      SendDone = ( CurrentPacketIndex == mTotalRecvPacketCount );

      while( !RecvDone || !SendDone ) 
        {
        while( !SendDone )
          {
          // Reverse direction.
          _BGL_TorusPkt* CurPkt = & mRecvBuffer[ CurrentPacketIndex ];

          // The old source rank is the new dest rank, test for self-sends
          int OldSourceRank = CurPkt->hdr.swh0.arg0;
//           int OldDestRank   = CurPkt->hdr.swh1.arg;

          if( OldSourceRank == Platform::Topology::GetAddressSpaceId() )
            {
            KSpacePacketPayload* KSpacePayloadPtr = (KSpacePacketPayload *) & CurPkt->payload;
            
            int Count = KSpacePayloadPtr->mElementCount;
            for( int ElemIndex = 0; ElemIndex < Count; ElemIndex++ )
              {
              int siteId     = KSpacePayloadPtr->mElements[ ElemIndex ].mSiteId;
              XYZ & Force = KSpacePayloadPtr->mElements[ ElemIndex ].mForceOrPos;
              
              mIntegratorStateManagerIF->AddForceCore1( siteId, Force );
              }
            
            TotalPacketsRecvOnReduce++;
            }
          else if( !BGLTorusTryToSendPacketBack( mCoreId, CurPkt ) )
            {
            // Do some torus recv fifo polling
            break;
            }

          CurrentPacketIndex++;
          SendDone = (CurrentPacketIndex == mTotalRecvPacketCount);
          }

        if( !RecvDone )
          {
          int RecvPacketCount = BGLTorusRecvPacket( mCoreId, (char *) & ReduceRecvBuffer, sizeof( _BGL_TorusPkt ) );
          
          TotalPacketsRecvOnReduce += RecvPacketCount;
          
          for( int i = 0; i < RecvPacketCount; i++ )
            {
            _BGL_TorusPkt* CurPkt = & ReduceRecvBuffer[ i ];
            KSpacePacketPayload* KSpacePayloadPtr = (KSpacePacketPayload *) & CurPkt->payload;
            
            int Count = KSpacePayloadPtr->mElementCount;
            for( int ElemIndex = 0; ElemIndex < Count; ElemIndex++ )
              {
              int siteId     = KSpacePayloadPtr->mElements[ ElemIndex ].mSiteId;
              XYZ & Force = KSpacePayloadPtr->mElements[ ElemIndex ].mForceOrPos;
              
              mIntegratorStateManagerIF->AddForceCore1( siteId, Force );
              }
            }

          BegLogLine( 0 )
              << "TotalPacketsRecvOnReduce: " << TotalPacketsRecvOnReduce
              << " mNumberOfPacketsSent: " << mNumberOfPacketsSent
              << EndLogLine;
          
          // We need to get back whatever we sent
          RecvDone = ( TotalPacketsRecvOnReduce == mNumberOfPacketsSent );
          }
        }

      BegLogLine( 0 )
        << "Done ReduceExecute..."
        << EndLogLine;

      // BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
      BegLogLine( 0 )
        << SimTick
        << " Leaving ReduceExecute"
        << EndLogLine;
      }
};
#endif
