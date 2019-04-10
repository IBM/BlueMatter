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
 #ifndef __INTERACTION_STATE_MANAGER_IF_HPP__
#define __INTERACTION_STATE_MANAGER_IF_HPP__

// #include <BlueMatter/qsort.h>

static int CompareFragId( const void* elem1, const void* elem2 )
  {
  FragId* fId1 = (FragId *) elem1;
  FragId* fId2 = (FragId *) elem2;
  
  if( *fId1 < *fId2 )
    return -1;
  else if( *fId1 == *fId2 )
    return 0;
  else
    return 1;
  }

struct EnergyHolder
{
double mEnergy;
int    mUDFNum;
};


/* 
 *  INTERACTION STATE
 */  
class InteractionStateManagerIF
  {        
  public:
  struct ThisPtrArgs
    {
    InteractionStateManagerIF* mThisPtr;
    int                        mUtil;
    };

  struct InteractionStateTableRecord
    {
    XYZ*       mForces;
    XYZ*       mPositions;
    VoxelIndex mVoxelIndex;
    } MEMORY_ALIGN( 5 );
  
  InteractionStateTableRecord    mInteractionStateTable[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN( 5 );
  
  // This list containts the fragments in the elliprical sphere
  FragId                         mInteractionShortList[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN( 5 );
  int                            mInteractionShortListSize MEMORY_ALIGN( 5 );
  int                            mInteractionShortListSizeOnCore MEMORY_ALIGN( 5 );

  char*                          mLocalPositPacketBuffer                  MEMORY_ALIGN( 5 );
  int                            mLocalPositPacketBufferSize;
      
  char*                          mLocalForcePacketBuffer                  MEMORY_ALIGN( 5 );
  int                            mLocalForcePacketBufferSize;
  int                            mLocalForcePacketBufferCurrentSize;

  #define REAL_SPACE_ENERGY_COUNT 3
  EnergyHolder                   mRealSpaceEnergy[ REAL_SPACE_ENERGY_COUNT ] MEMORY_ALIGN( 5 );

  XYZ mLocBuff[ 5 ] MEMORY_ALIGN( 5 );
  
#if (defined( PK_BGL ) && defined( DO_REAL_SPACE_ON_SECOND_CORE_VNM ) )

 static
 void*
 AddInRealSpaceForcesActorFx( void* args )
   {
   ThisPtrArgs* zargs = (ThisPtrArgs *) args;
   InteractionStateManagerIF* ThisPtrMe    = zargs->mThisPtr;

   // NOTE: Cache coherancy happends in the caller's scope.
   // This is due to variate behaviour from running real space on Core0 and Core1      
   InteractionStateManagerIF* OtherThisPtr = (InteractionStateManagerIF*) (0x50000000 + (unsigned)ThisPtrMe);

   XYZ* MyForcesOnOtherCore = (XYZ *) ( 0x50000000 + (unsigned) OtherThisPtr->mLocalForcePacketBuffer );

   int CurForceOffset = 0;

   // for( int i=0; i < ThisPtrMe->mInteractionShortListSizeOnCore; i++ )
   // for( int i=ThisPtrMe->mInteractionShortListSizeOnCore; i < ThisPtrMe->mInteractionShortListSize; i++ )
   for( int i=0; i < ThisPtrMe->mInteractionShortListSizeOnCore; i++ )
     {
     FragId fragId = ThisPtrMe->mInteractionShortList[ i ];
     assert( fragId >= 0 && fragId < NUMBER_OF_FRAGMENTS );

     int NumSites       = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
     assert( NumSites >= 1 && NumSites <= 4 );
     
     // XYZ* ForcePacketOther = (XYZ *) ( 0x50000000 + (unsigned) OtherThisPtr->mInteractionStateTable[ fragId ].mForces );
     XYZ* ForcePacketOther = ( &MyForcesOnOtherCore[ CurForceOffset ] );
     
     CurForceOffset += NumSites;

     // This might cause a reference off the end of the 'force packet', but should still be in mapped memory, and we will not use the 'extra' stuff
     double Force0mX = ForcePacketOther[ 0 ].mX ;
     double Force0mY = ForcePacketOther[ 0 ].mY ;
     double Force0mZ = ForcePacketOther[ 0 ].mZ ;
     
     double Force1mX = ForcePacketOther[ 1 ].mX ;
     double Force1mY = ForcePacketOther[ 1 ].mY ;
     double Force1mZ = ForcePacketOther[ 1 ].mZ ;
     
     double Force2mX = ForcePacketOther[ 2 ].mX ;
     double Force2mY = ForcePacketOther[ 2 ].mY ;
     double Force2mZ = ForcePacketOther[ 2 ].mZ ;
     
     double Force3mX = ForcePacketOther[ 3 ].mX ;
     double Force3mY = ForcePacketOther[ 3 ].mY ;
     double Force3mZ = ForcePacketOther[ 3 ].mZ ;
     
     switch ( NumSites ) 
     {
  case 1: 
           ThisPtrMe->AddForce( fragId, 0, Force0mX, Force0mY, Force0mZ ) ;
           break ; 	
  case 2: 
           ThisPtrMe->AddForce( fragId, 0, Force0mX, Force0mY, Force0mZ ) ;
           ThisPtrMe->AddForce( fragId, 1, Force1mX, Force1mY, Force1mZ ) ;
           break ; 	
  case 3: 
           ThisPtrMe->AddForce( fragId, 0, Force0mX, Force0mY, Force0mZ ) ;
           ThisPtrMe->AddForce( fragId, 1, Force1mX, Force1mY, Force1mZ ) ;
           ThisPtrMe->AddForce( fragId, 2, Force2mX, Force2mY, Force2mZ ) ;
           break ; 	
  case 4: 
           ThisPtrMe->AddForce( fragId, 0, Force0mX, Force0mY, Force0mZ ) ;
           ThisPtrMe->AddForce( fragId, 1, Force1mX, Force1mY, Force1mZ ) ;
           ThisPtrMe->AddForce( fragId, 2, Force2mX, Force2mY, Force2mZ ) ;
           ThisPtrMe->AddForce( fragId, 3, Force3mX, Force3mY, Force3mZ ) ;
           break ; 	
        default: assert(0) ; // Only handles frags with 1,2,3, or 4 sites
     } // end switch(NumSites) 
     
     
//     XYZ Force0; Force0.mX=Force0mX ; Force0.mY=Force0mY ; Force0.mZ=Force0mZ ;
//     XYZ Force1; Force1.mX=Force1mX ; Force1.mY=Force1mY ; Force1.mZ=Force1mZ ;
//     XYZ Force2; Force2.mX=Force2mX ; Force2.mY=Force2mY ; Force2.mZ=Force2mZ ;
//     XYZ Force3; Force3.mX=Force3mX ; Force3.mY=Force3mY ; Force3.mZ=Force3mZ ;
//     
////     XYZ Force0 = ForcePacketOther[ 0 ] ; 
////     XYZ Force1 = ForcePacketOther[ 1 ] ; 
////     XYZ Force2 = ForcePacketOther[ 2 ] ; 
////     XYZ Force3 = ForcePacketOther[ 3 ] ; 
//     
//     switch ( NumSites ) 
//     {
//	case 1: 
//           ThisPtrMe->AddForce( fragId, 0, Force0 );
//           break ; 	
//	case 2: 
//           ThisPtrMe->AddForce( fragId, 0, Force0 );
//           ThisPtrMe->AddForce( fragId, 1, Force1 );
//           break ; 	
//	case 3: 
//           ThisPtrMe->AddForce( fragId, 0, Force0 );
//           ThisPtrMe->AddForce( fragId, 1, Force1 );
//           ThisPtrMe->AddForce( fragId, 2, Force2 );
//           break ; 	
//	case 4: 
//           ThisPtrMe->AddForce( fragId, 0, Force0 );
//           ThisPtrMe->AddForce( fragId, 1, Force1 );
//           ThisPtrMe->AddForce( fragId, 2, Force2 );
//           ThisPtrMe->AddForce( fragId, 3, Force3 );
//           break ; 	
//        default: assert(0) ; // Only handles frags with 1,2,3, or 4 sites
//     } // end switch(NumSites) 
//     for( int offset=0; offset < NumSites; offset++ )
//       {
//       ThisPtrMe->AddForce( fragId, offset, ForcePacketOther[ offset ] );
//       }
     }
   return NULL;
   }

 inline
 void 
 FlushCore1()
   {       
   vnm_co_start( FlushCacheActor, NULL, 0 );
   // rts_dcache_evict_normal();
   FlushCacheActor( NULL );
   vnm_co_join();
   }

  static
  void* FlushEnergyOnCoreActor( void* args )
    {
    ThisPtrArgs* zargs = (ThisPtrArgs *) args;
    InteractionStateManagerIF* ThisPtrMe    = zargs->mThisPtr;

    rts_dcache_store_invalidate( GET_BEGIN_PTR_ALIGNED( ThisPtrMe->mRealSpaceEnergy ),
                                 GET_END_PTR_ALIGNED( ThisPtrMe->mRealSpaceEnergy + REAL_SPACE_ENERGY_COUNT ) );
    return NULL;
    }

  inline
  void 
  FlushEnergyOnCore()
    {
   ThisPtrArgs zargs;
   zargs.mThisPtr = this;

    vnm_co_start( FlushEnergyOnCoreActor, &zargs, sizeof( zargs ) );
    // rts_dcache_evict_normal();
    FlushEnergyOnCoreActor( &zargs );
    vnm_co_join();
    }

  static
  void* FlushOtherCoreForceActor( void* args )
    {
    ThisPtrArgs* zargs = (ThisPtrArgs *) args;
    InteractionStateManagerIF* ThisPtrMe    = zargs->mThisPtr;

    rts_dcache_store_invalidate( GET_BEGIN_PTR_ALIGNED( ThisPtrMe->mLocalForcePacketBuffer ),
                                 GET_END_PTR_ALIGNED( ThisPtrMe->mLocalForcePacketBuffer + 
                                                      ThisPtrMe->mLocalForcePacketBufferCurrentSize) );
    return NULL;
    }

 inline
 void
 FlushOtherCoreForces()
   {
   ThisPtrArgs zargs;
   zargs.mThisPtr = this;

   vnm_co_start( FlushOtherCoreForceActor, &zargs, sizeof( zargs ) );
   FlushOtherCoreForceActor( &zargs );
   vnm_co_join();
   }

 inline 
 void
 AddInRealSpaceForces( int aSimTick )
   {   
   FlushOtherCoreForces();

   ThisPtrArgs zargs;
   zargs.mThisPtr = this;
   vnm_co_start( AddInRealSpaceForcesActorFx, &zargs, sizeof( zargs ));
   AddInRealSpaceForcesActorFx( &zargs );
   vnm_co_join();
   return;
   }

  static
  void*
  GetVoxelIndecies(void* args)
    {
    ThisPtrArgs* targs = (ThisPtrArgs*) args;
    InteractionStateManagerIF* ThisPtr = targs->mThisPtr;
    InteractionStateManagerIF* ThisPtrOthr = (InteractionStateManagerIF *) ( 0x50000000 + (unsigned) ThisPtr );

    for(int i=0; i<ThisPtr->mInteractionShortListSize; i++ )
      {
      FragId fragmentId = ThisPtr->mInteractionShortList[ i ];
      ThisPtr->SetVoxelIndex( fragmentId, 
                              ThisPtrOthr->mInteractionStateTable[ fragmentId ].mVoxelIndex );
      }
    return NULL;
    }
 
  void
  SendVoxelIdsToCore1() 
    {
    ThisPtrArgs targs;
    targs.mThisPtr = this;
    rts_dcache_evict_normal();
    vnm_co_start( GetVoxelIndecies, &targs, sizeof( targs ) );
    vnm_co_join();
    }

  struct CopyActorArgs
    {
    InteractionStateManagerIF* mThisPtr;
    int                        mFragCount;
    };
      
 static
 void*
 CopyNeighborhoodStateActor( void *arg )
   {
   CopyActorArgs* zargs = (CopyActorArgs *) arg;
   InteractionStateManagerIF* ThisPtrMe    = zargs->mThisPtr;
   InteractionStateManagerIF* ThisPtrOther = (InteractionStateManagerIF*) ( 0x50000000 + (unsigned) ThisPtrMe );
   int OtherNodeFragCount = zargs->mFragCount;

   ThisPtrMe->mInteractionShortListSizeOnCore = ThisPtrMe->mInteractionShortListSize;
   int CurPositOffset = 0;
   ThisPtrMe->mLocalForcePacketBufferCurrentSize = 0;

   BegLogLine( 0 )
     << "ThisPtrMe->mInteractionShortListSize: " << ThisPtrMe->mInteractionShortListSize
     << " OtherNodeFragCount: " << OtherNodeFragCount
     << EndLogLine;
   
   for( int i=0; i<OtherNodeFragCount; i++ )
     {
     FragId fragId = ThisPtrOther->mInteractionShortList[ i ];
     int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
     
     // Copy the position packet into local buffer
     XYZ* PositionPacketOther = 
       (XYZ *) ( 0x50000000 + (unsigned) ThisPtrOther->mInteractionStateTable[ fragId ].mPositions );
     
     assert( CurPositOffset >= 0 && CurPositOffset < ThisPtrMe->mLocalPositPacketBufferSize );
     XYZ* PositionPacketMe = (XYZ *) &ThisPtrMe->mLocalPositPacketBuffer[ CurPositOffset ];

     int SizeOfPositPacket =  nsites * sizeof( XYZ );
     int SizeOfForcePacket =  nsites * sizeof( XYZ );

     double Pos0X = PositionPacketOther[ 0 ].mX;
     double Pos0Y = PositionPacketOther[ 0 ].mY;
     double Pos0Z = PositionPacketOther[ 0 ].mZ;

     double Pos1X = PositionPacketOther[ 1 ].mX;
     double Pos1Y = PositionPacketOther[ 1 ].mY;
     double Pos1Z = PositionPacketOther[ 1 ].mZ;

     double Pos2X = PositionPacketOther[ 2 ].mX;
     double Pos2Y = PositionPacketOther[ 2 ].mY;
     double Pos2Z = PositionPacketOther[ 2 ].mZ;

     double Pos3X = PositionPacketOther[ 3 ].mX;
     double Pos3Y = PositionPacketOther[ 3 ].mY;
     double Pos3Z = PositionPacketOther[ 3 ].mZ;

     switch( nsites )
       {
       case 1:
           PositionPacketMe[ 0 ].mX = Pos0X;
           PositionPacketMe[ 0 ].mY = Pos0Y;
           PositionPacketMe[ 0 ].mZ = Pos0Z;
           break;
       case 2:
           PositionPacketMe[ 0 ].mX = Pos0X;
           PositionPacketMe[ 0 ].mY = Pos0Y;
           PositionPacketMe[ 0 ].mZ = Pos0Z;

           PositionPacketMe[ 1 ].mX = Pos1X;
           PositionPacketMe[ 1 ].mY = Pos1Y;
           PositionPacketMe[ 1 ].mZ = Pos1Z;
           break;
       case 3:
           PositionPacketMe[ 0 ].mX = Pos0X;
           PositionPacketMe[ 0 ].mY = Pos0Y;
           PositionPacketMe[ 0 ].mZ = Pos0Z;

           PositionPacketMe[ 1 ].mX = Pos1X;
           PositionPacketMe[ 1 ].mY = Pos1Y;
           PositionPacketMe[ 1 ].mZ = Pos1Z;

           PositionPacketMe[ 2 ].mX = Pos2X;
           PositionPacketMe[ 2 ].mY = Pos2Y;
           PositionPacketMe[ 2 ].mZ = Pos2Z;
           break;
       case 4:
           PositionPacketMe[ 0 ].mX = Pos0X;
           PositionPacketMe[ 0 ].mY = Pos0Y;
           PositionPacketMe[ 0 ].mZ = Pos0Z;

           PositionPacketMe[ 1 ].mX = Pos1X;
           PositionPacketMe[ 1 ].mY = Pos1Y;
           PositionPacketMe[ 1 ].mZ = Pos1Z;

           PositionPacketMe[ 2 ].mX = Pos2X;
           PositionPacketMe[ 2 ].mY = Pos2Y;
           PositionPacketMe[ 2 ].mZ = Pos2Z;

           PositionPacketMe[ 3 ].mX = Pos3X;
           PositionPacketMe[ 3 ].mY = Pos3Y;
           PositionPacketMe[ 3 ].mZ = Pos3Z;
           break;
       default:
           assert(0);
       }

//      memcpy( PositionPacketMe,
//              PositionPacketOther,
//              SizeOfPositPacket );

     assert( ThisPtrMe->mLocalForcePacketBufferCurrentSize >= 0 && ThisPtrMe->mLocalForcePacketBufferCurrentSize < ThisPtrMe->mLocalForcePacketBufferSize );
     XYZ* ForcePacketMe = (XYZ *) &ThisPtrMe->mLocalForcePacketBuffer[ ThisPtrMe->mLocalForcePacketBufferCurrentSize ];
     
     ThisPtrMe->AddFragment( fragId, PositionPacketMe, ForcePacketMe );       
     
     CurPositOffset += SizeOfPositPacket;
     ThisPtrMe->mLocalForcePacketBufferCurrentSize += SizeOfForcePacket;
     }

   return NULL;
   }

  static
  void* FlushCacheActor( void* args )
    {
    rts_dcache_evict_normal();
    return NULL;
    }

  inline
  void
  CopyNeigborhoodState()
    {
    FlushCore1();
    
    // Get other nodes LocalFragmentCount
    int OtherNodesFragmentCount = (int) *( (int *) (0x50000000 + (unsigned) &mInteractionShortListSize ) );
    CopyActorArgs zargs0;
    zargs0.mThisPtr = this;
    zargs0.mFragCount   = OtherNodesFragmentCount;

    CopyActorArgs zargs1;
    zargs1.mThisPtr = this;
    zargs1.mFragCount   = mInteractionShortListSize;

    vnm_co_start( CopyNeighborhoodStateActor, &zargs1, sizeof( CopyActorArgs ) );
    CopyNeighborhoodStateActor( &zargs0 );
    vnm_co_join();
    }

 inline
 void
 SetEnergyHolderUDFNum( int aOrdinal, int aUDFNum )
   {
   assert( aOrdinal >= 0 && aOrdinal < REAL_SPACE_ENERGY_COUNT );
   mRealSpaceEnergy[ aOrdinal ].mUDFNum = aUDFNum;   
   }
  
 inline 
 void 
 SetRealSpaceEnergy( int aOrdinal, double aEnergy )
   {
   assert( aOrdinal >= 0 && aOrdinal < REAL_SPACE_ENERGY_COUNT );
   mRealSpaceEnergy[ aOrdinal ].mEnergy = aEnergy;
   }

 inline
 void
 ReportRealSpaceEnergies( int aSimTick )
   {   
   // NOTE: Cache coherancy happends in the caller's scope.
   // This is due to variate behaviour from running real space on Core0 and Core1      
   EnergyHolder* OtherCoreEnergyHolder = (EnergyHolder*) (0x50000000 + (unsigned)mRealSpaceEnergy);

   FlushEnergyOnCore();
   
   for( int i=0; i<REAL_SPACE_ENERGY_COUNT; i++ )
     {
     if( OtherCoreEnergyHolder[ i ].mUDFNum != -1 )
       {
       #ifdef DUAL_CORE_REAL_SPACE    
         double Energy = mRealSpaceEnergy[ i ].mEnergy + OtherCoreEnergyHolder[ i ].mEnergy;
       #else
         double Energy = OtherCoreEnergyHolder[ i ].mEnergy;
       #endif

       EmitEnergy( aSimTick, OtherCoreEnergyHolder[ i ].mUDFNum, Energy, 0 );
//          EmitEnergy( aSimTick, 
//                      OtherCoreEnergyHolder[ i ].mUDFNum, 
//                      mRealSpaceEnergy[ i ].mEnergy, 
//                      0 );
         
       }
     }
   }
#endif
     
  void 
  ChecksumPositions( double aGoldenChecksum )
    {
    #ifdef CHECKSUM_INTERECTION_POSITIONS 
    double PositionCheckInteractionContext1 = DynVarMgrIF.mInteractionStateManagerIF.ChecksumPositions();
    if( PositionCheckInteractionContext1 != aGoldenChecksum )
      {
      BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
        << "SimTick: " << SimTick
        << " aGoldenChecksum: " << aGoldenChecksum
        << " PositionCheckInteractionContext1: " << PositionCheckInteractionContext1
        << " ERROR:: Position checksum failed"
        << EndLogLine;
      
     PLATFORM_ABORT( "ERROR:: Position checksum failed" );      
      }
    #endif
    }
 
  double
  ChecksumPositions()
    {
    double checksum = 0.0;

    for( int i=0; i< NUMBER_OF_FRAGMENTS; i++ )
      {
      if( mInteractionStateTable[ i ].mPositions != NULL )
        {
        int numSites    = MSD_IF::GetIrreducibleFragmentMemberSiteCount( i );
        
        assert( numSites >= 1 && numSites <= 4 );
        
        for( int offset = 0; offset < numSites; offset++ )
          {
          XYZ Pos = GetPosition( i, offset );
          checksum += ( Pos.mX + Pos.mY + Pos.mZ );
          }
        }
      }

     return checksum;
    }    

  inline
  int
  IsFragmentInRange( FragId aFragId )
    {
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      if( mInteractionShortList[ i ] == aFragId )
        return 1;
      }
    return 0;
    }

  inline
  void
  EliminateFragmentInNeighborhood( int n )
    {
#if 0
    assert( n>=0 && n < mInteractionShortListSize );
    FragId fragId = mInteractionShortList[ n ];
    assert( fragId >= 0 && fragId < NUMBER_OF_FRAGMENTS );
    #ifdef DO_REAL_SPACE_ON_SECOND_CORE
      mRealSpaceCulling[ fragId ] = 1;
    #else
      mInteractionStateTable[ fragId ].mForcePacket->mCulled = 1;
    #endif
#endif
  }
    

  static
  void* DumpPositionActor( void * args )
    {
    ThisPtrArgs* zargs = (ThisPtrArgs *) args;
    InteractionStateManagerIF* ThisPtrMe    = zargs->mThisPtr;
    int                        lSimTick     = zargs->mUtil;

    ThisPtrMe->DumpPositions( lSimTick );

    return NULL;
    }
 
  inline
  void
  DumpPositionBothCores( int aSimTick )
    {
    ThisPtrArgs zargs;
    zargs.mThisPtr = this;
    zargs.mUtil    = aSimTick;

    vnm_co_start( DumpPositionActor, &zargs,  sizeof( zargs ) );
    DumpPositionActor( &zargs );
    vnm_co_join();
    }

  inline
  void
  DumpPositions( int aSimTick )
    {
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      FragId fragId = mInteractionShortList[ i ];
      
      SiteId FirstSiteId = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      
      for( int offset = 0; offset < SiteCount; offset++ )
        {
        BegLogLine( aSimTick == 0 )
          << aSimTick << " Position " << (FirstSiteId+offset ) << " " << GetPosition( fragId, offset )
          << EndLogLine;
        }
      }
    }    

  inline
  void
  DumpForces( int aSimTick )
    {
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      FragId fragId = mInteractionShortList[ i ];
      
      int SiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      
      for( int offset = 0; offset < SiteCount; offset++ )
        {
        BegLogLine( aSimTick == 0 )
          << aSimTick << " RacForce " << fragId << " " << offset << " " << GetForce( fragId, offset ) 
          << EndLogLine;
        }
      }
    }    


  void 
  SortFragList()
    {   
    for( int i=0; i<mInteractionShortListSize; i++ )
      {
      int MinI = i ;
      FragId MinFragId = mInteractionShortList[ i ];
      for( int j=i; j<mInteractionShortListSize; j++ )
        {
        if( MinFragId >= mInteractionShortList[ j ] )
          {
          MinI = j;
          MinFragId = mInteractionShortList[ j ];
          }
        }

      FragId temp = mInteractionShortList[ MinI ];
      mInteractionShortList[ MinI ] = mInteractionShortList[ i ];
      mInteractionShortList[ i ] = temp;      
      }
    }

  inline
  void
  SortFragmentNeighborList( FragId* aList, int aSize )
    {
    Platform::Algorithm::Qsort( aList, aSize, sizeof( FragId ), CompareFragId, 1 );
    }

  inline
  int
  GetFragmentNeighborListSize()
    {
    return mInteractionShortListSize;
    }

  inline
  FragId
  GetNthFragmentInNeighborhood( int n )
    {
    assert( n >= 0 && n < mInteractionShortListSize );
    
    return mInteractionShortList[ n ];
    }  

  static 
  void*
  InitPositionForceBuffers( void* arg )
    {
    ThisPtrArgs* zargs = (ThisPtrArgs *) arg;
    InteractionStateManagerIF* ThisPtrMe    = zargs->mThisPtr;

    ThisPtrMe->mInteractionShortListSize = 0;    
    for( int i=0; i<REAL_SPACE_ENERGY_COUNT; i++ )
      {
      ThisPtrMe->mRealSpaceEnergy[ i ].mUDFNum = -1;
      ThisPtrMe->mRealSpaceEnergy[ i ].mEnergy = -1.0;
      }

    // ThisPtrMe->mLocalPositPacketBufferSize = ( sizeof( XYZ ) * NUMBER_OF_SITES ) + ( NUMBER_OF_FRAGMENTS * sizeof( PositionPacket ));
    ThisPtrMe->mLocalPositPacketBufferSize = ( sizeof( XYZ ) * NUMBER_OF_SITES ) / 2.0 ;
    ThisPtrMe->mLocalPositPacketBuffer  = (char *) malloc( ThisPtrMe->mLocalPositPacketBufferSize );
    assert( ThisPtrMe->mLocalPositPacketBuffer );

    // ThisPtrMe->mLocalForcePacketBufferSize = ( sizeof( XYZ ) * NUMBER_OF_SITES ) + ( NUMBER_OF_FRAGMENTS * sizeof( ForcePacket ) );
    ThisPtrMe->mLocalForcePacketBufferSize = ( sizeof( XYZ ) * NUMBER_OF_SITES ) / 2.0 ;
    ThisPtrMe->mLocalForcePacketBuffer  = (char *) malloc( ThisPtrMe->mLocalForcePacketBufferSize );
    assert( ThisPtrMe->mLocalForcePacketBuffer );

    BegLogLine( 0 )
      << "ThisPtrMe->mLocalPositPacketBufferSize: " << ThisPtrMe->mLocalPositPacketBufferSize
      << " ThisPtrMe->mLocalForcePacketBufferSize: " << ThisPtrMe->mLocalForcePacketBufferSize
      << " ThisPtrMe->mLocalPositPacketBuffer: " << ThisPtrMe->mLocalPositPacketBuffer
      << " ThisPtrMe->mLocalForcePacketBuffer: " << ThisPtrMe->mLocalForcePacketBuffer
      << " ThisPtrMe: " << ThisPtrMe
      << EndLogLine;

    return NULL;    
    }

  inline
  void
  Init()
    {
    mInteractionShortListSize = 0;    

    ThisPtrArgs zargs;
    zargs.mThisPtr = this;

    InitPositionForceBuffers( &zargs );

    vnm_co_start( InitPositionForceBuffers, &zargs, sizeof( ThisPtrArgs ) );
    vnm_co_join();

    for( int i=0; i<REAL_SPACE_ENERGY_COUNT; i++ )
      {
      mRealSpaceEnergy[ i ].mEnergy = 0.0; 
      }    
    }
 
      
  static
  void* ResetStateActor( void * args )
    {
    ThisPtrArgs* zargs = (ThisPtrArgs *) args;
    InteractionStateManagerIF* ThisPtrMe    = zargs->mThisPtr;
 
    ThisPtrMe->ResetState();

    return NULL;
    }
 
  inline
  void
  ResetStateOnBothCores()
    {
    ThisPtrArgs zargs;
    zargs.mThisPtr = this;

    vnm_co_start( ResetStateActor, &zargs,  sizeof( zargs ) );
    ResetStateActor( &zargs );
    vnm_co_join();
    }

  inline
  void
  ResetState()
    {
    #ifdef CHECKSUM_INTERECTION_POSITIONS
    for( int i = 0; i < NUMBER_OF_FRAGMENTS; i++ )
      {
      mInteractionStateTable[ i ].mForces    = NULL;
      mInteractionStateTable[ i ].mPositions = NULL;
      }
    #endif

    mInteractionShortListSize = 0;
    }

  inline    
  void
  SetPositions( FragId aFragId, XYZ* aPositions )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    mInteractionStateTable[ aFragId ].mPositions = aPositions;
    }

  inline    
  void
  SetForces( FragId aFragId, XYZ* aForces )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );    
    mInteractionStateTable[ aFragId ].mForces = aForces;

#if 0
    mInteractionStateTable[ aFragId ].mForcePacket->mCulled = 0;
    #ifdef DO_REAL_SPACE_ON_SECOND_CORE
      mRealSpaceCulling[ aFragId ] = 0;
    #endif
#endif
    }
  
  inline
  void
  ZeroForce( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    for( int offset=0; offset < nsites; offset++)
      {
      assert( mInteractionStateTable[ aFragId ].mForces != NULL );
      mInteractionStateTable[ aFragId ].mForces[ offset ].Zero();
      }
    }

  inline
  void
  SetVoxelIndex( FragId     aFragId,
                 VoxelIndex aVoxelIndex )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    mInteractionStateTable[ aFragId ].mVoxelIndex = aVoxelIndex;    
    }

  inline
  void
  AddFragment( FragId      aFragId, 
               VoxelIndex  aVoxelIndex,
               XYZ*        aPositions,
               XYZ*        aForces )
    {
    SetPositions( aFragId, aPositions );
    SetForces( aFragId, aForces );
    SetVoxelIndex( aFragId, aVoxelIndex );
    
    // Forces get added during the interaction phase
    ZeroForce( aFragId );

    assert( mInteractionShortListSize >= 0 && 
            mInteractionShortListSize < NUMBER_OF_FRAGMENTS );
    
    mInteractionShortList[ mInteractionShortListSize ] = aFragId;    
    mInteractionShortListSize++;    
    }

  inline
  void
  AddFragment( FragId      aFragId, 
               XYZ*        aPositions,
               XYZ*        aForces )
    {
    SetPositions( aFragId, aPositions );
    SetForces( aFragId, aForces );
    
    // Forces get added during the interaction phase
    ZeroForce( aFragId );

    assert( mInteractionShortListSize >= 0 && 
            mInteractionShortListSize < NUMBER_OF_FRAGMENTS );
    
    mInteractionShortList[ mInteractionShortListSize ] = aFragId;    
    mInteractionShortListSize++;    
    }

  inline
  VoxelIndex
  GetVoxelIndex( FragId aFragId )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
        
    VoxelIndex rc = rec.mVoxelIndex;
    
    return( rc );
    }

  inline
  XYZ *
  GetPositionPtr( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  

    assert( rec.mPositions != NULL );
    
    XYZ * rc = & (rec.mPositions[ aOffset ]);
    
    return( rc );
    }

  inline
  XYZ&
  GetPosition( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    // assert( rec.mPositions != NULL );
    if( rec.mPositions == NULL )
      {
      BegLogLine( 1 )
          << "ERROR: aFragId: " 
          << aFragId
          << " aOffset: " << aOffset
          << EndLogLine;
      
      assert( 0 );
      }

    XYZ & rc = rec.mPositions[ aOffset ];
    
    return( rc );
    }

  inline
  XYZ&
  GetForce( FragId aFragId, int aOffset )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
    
    assert( rec.mForces != NULL );

    XYZ & rc = rec.mForces[ aOffset ];
    
    return( rc );
    }

  inline
  short 
  GetCulled( FragId aFragId )
    {
#if 0
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
        
    assert( rec.mForcePacket != NULL );

    short rc = rec.mForcePacket->mCulled;
#endif

    short rc = 0;
    return( rc );
    }
  
  inline
  void
  AddForce( FragId aFragId, int aOffset, XYZ aForce )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    assert( aForce.IsReasonable(__FILE__, __LINE__) );

    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
      
    assert( rec.mForces != NULL );

    rec.mForces[ aOffset ] += aForce;      
    }  
        
  inline
  void
  AddForce( FragId aFragId, int aOffset, double aForcemX, double aForcemY, double aForcemZ )
    {
    assert( aFragId >= 0 && aFragId < NUMBER_OF_FRAGMENTS );
    
    #ifndef NDEBUG
    XYZ aForce ; aForce.mX = aForcemX ; aForce.mY = aForcemY ; aForce.mZ = aForcemZ ;
    assert( aForce.IsReasonable(__FILE__, __LINE__) );
    #endif

    InteractionStateTableRecord & rec = mInteractionStateTable[ aFragId ];  
    
    #ifndef NDEBUG
    int nsites = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aFragId );
    assert( aOffset >= 0 && aOffset < nsites );  
    #endif  
      
    assert( rec.mForces != NULL );

  rec.mForces [ aOffset ].mX += aForcemX ; 
  rec.mForces [ aOffset ].mY += aForcemY ; 
  rec.mForces [ aOffset ].mZ += aForcemZ ; 
  
    }      
  
  
  #ifdef DO_REAL_SPACE_ON_SECOND_CORE  
  inline
  void
  AddForceForRealSpace(  SiteId aSiteId, XYZ aForce )
    {
    assert( aSiteId >= 0 && aSiteId < NUMBER_OF_SITES );    
    assert( aForce.IsReasonable(__FILE__, __LINE__) );
    mRealSpaceForces[ aSiteId ] += aForce;
    }      
  #endif
  
  inline
  void
  ZeroForces()
    {
    for( int i=0; i < mInteractionShortListSize; i++ )
      {
      FragId FragmentId = mInteractionShortList[ i ];
      ZeroForce( FragmentId );
      }
    }
  };
#endif
