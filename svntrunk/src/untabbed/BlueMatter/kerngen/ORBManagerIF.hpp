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
 #ifndef __ORB_MANAGER_IF__
#define __ORB_MANAGER_IF__

#include <BlueMatter/DynamicVariableManager_phase4.hpp>

#ifndef PKFXLOG_INTERACTION_KIND
#define PKFXLOG_INTERACTION_KIND ( 0 ) 
#endif

#ifndef PKFXLOG_ORB_LOAD
#define PKFXLOG_ORB_LOAD (0) 
#endif

#ifdef PK_BGL
typedef long long ORBWeightType;
#else
typedef double ORBWeightType;
#endif

//__________
// These two bits buy you the latest greatest ORB weighting sham - Pitman 4/23/05
#define USE_ORB_TABLE_MODEL  
#define USE_MODEL_WEIGHTS
//___________

// double Extents[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);
// XYZ    FragCenters[ NUMBER_OF_FRAGMENTS ] MEMORY_ALIGN(5);

struct FragWeight
  {
  ORBWeightType         mWeight;
  unsigned short mTargetId;
  unsigned short mSourceId;
  unsigned int   mChecksum;
  };

template< class DynVarMgrIF >
class T_ORBManagerIF
  {
  public:
    ORBNode*            mRecBisTree;
    DynVarMgrIF*        mDynVarMgrIF;


    double             mMyLow[3];
    double             mMyHigh[3];

    int*                mCountPerNode;
    int*                mTempCountPerNode;

    FragWeight*         mTotalFragWeightList;
    FragWeight*         mTempTotalFragWeightList;
    int                 mTotalFragWeightListSize;

    FragWeight*         mLocalFragWeight;
    int                 mLocalFragWeightSize;

    int                 mOrbVerletListSize;
    int                 mCalculateVerletListSize;

    int                 mAddressSpaceId;

    XYZ*                mOrbInputPoints;  
    ORBWeightType*      mOrbInputWeights;
    int                 mVoxelSpaceSize;

//    T_ORBManagerIF() {}

    void
    Init( DynVarMgrIF* aDynVarMgrIF )
      {
      BegLogLine( PKFXLOG_SETUP )
        << "Entering ORBManagerIF::Init(): "
        << EndLogLine;

      mAddressSpaceId = Platform::Topology::GetAddressSpaceId();

      mDynVarMgrIF = aDynVarMgrIF;
      mOrbVerletListSize = 0;

//       mCountPerNode = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
//       if( mCountPerNode == NULL )
//         PLATFORM_ABORT("mCountPerNode == NULL");

//       mTempCountPerNode = (int *) malloc( sizeof(int) * Platform::Topology::GetSize() );
//       if( mTempCountPerNode == NULL )
//         PLATFORM_ABORT("mTempCountPerNode == NULL");

//       for( int i=0; i < Platform::Topology::GetSize(); i++)
//         {
//         mCountPerNode[ i ] = 0;
//         mTempCountPerNode[ i ] = 0;
//         }

      mRecBisTree = NULL;
//       mTotalFragWeightList = NULL;
//       mTempTotalFragWeightList = NULL;

//       mLocalFragWeight = NULL;
      
      int VoxDimX = mDynVarMgrIF->mVoxDimX;
      int VoxDimY = mDynVarMgrIF->mVoxDimY;
      int VoxDimZ = mDynVarMgrIF->mVoxDimZ;

      mVoxelSpaceSize = VoxDimX * VoxDimY * VoxDimZ;
         
      CreateFirstOrb( 0 );
      
//       mOrbInputWeights = (ORBWeightType *) malloc( sizeof( ORBWeightType ) * mVoxelSpaceSize );
//       if( mOrbInputWeights == NULL )
//           PLATFORM_ABORT( "mOrbInputWeights == NULL" );

//       mOrbInputPoints = (XYZ *) malloc( sizeof( XYZ ) * mVoxelSpaceSize );
//       if( mOrbInputPoints == NULL )
//           PLATFORM_ABORT( "mOrbInputPoints == NULL" );

//       double VoxSimDimX = mDynVarMgrIF->mDynamicBoxDimensionVector.mX / VoxDimX;
//       double VoxSimDimY = mDynVarMgrIF->mDynamicBoxDimensionVector.mY / VoxDimY;
//       double VoxSimDimZ = mDynVarMgrIF->mDynamicBoxDimensionVector.mZ / VoxDimZ;

//       for( int i=0; i < mVoxelSpaceSize; i++ )
//         {
//         mOrbInputWeights[ i ] = 0;
//         mOrbInputPoints[ i ].Invalidate();
//         }

//       for( int x=0; x < mDynVarMgrIF->mVoxDimX; x++ )
//         {
//         for( int y=0; y < mDynVarMgrIF->mVoxDimY; y++ )
//           {
//           for( int z=0; z < mDynVarMgrIF->mVoxDimZ; z++ )
//             {
//             double mid[3];
//             mid[ 0 ] = 0.001 + VoxSimDimX * x;
//             mid[ 1 ] = 0.001 + VoxSimDimY * y;
//             mid[ 2 ] = 0.001 + VoxSimDimZ * z;

//             int VoxelIdMid = mRecBisTree->VoxelId( mid );
//             int VoxelIndexMid = mRecBisTree->VoxelIdToVoxelIndex( VoxelIdMid );
//             mRecBisTree->VoxelPoint( VoxelIdMid, mid );

//             assert( VoxelIndexMid >=0 && VoxelIndexMid < mVoxelSpaceSize );

//             mOrbInputPoints[ VoxelIndexMid ].mX = mid[ 0 ];
//             mOrbInputPoints[ VoxelIndexMid ].mY = mid[ 1 ];
//             mOrbInputPoints[ VoxelIndexMid ].mZ = mid[ 2 ];
//             mOrbInputWeights[ VoxelIndexMid ] = 0;
//             }
//           }
//         }
      
//       #ifndef NDEBUG
//       for( int i=0; i < mVoxelSpaceSize; i++ )
//         assert( !mOrbInputPoints[ i ].IsInvalid() );
//       #endif
      
      }

    ORBNode* GetOrbRoot()
      {
      assert( mRecBisTree != NULL );
      return mRecBisTree;
      }

    int IsSiteHere( XYZ aSite )
      {
      int rc = ( ( aSite.mX >= myLow[ 0 ] && aSite.mX < myHigh[ 0 ] ) &&
                 ( aSite.mY >= myLow[ 1 ] && aSite.mY < myHigh[ 1 ] ) &&
                 ( aSite.mZ >= myLow[ 2 ] && aSite.mZ < myHigh[ 2 ] ) );

      return rc;
      }

    void SetMyOrbBox()
      {
      int xMy, yMy, zMy;
      Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
      ORBNode* MyNode = mRecBisTree->NodeFromProcCoord( xMy, yMy, zMy );
      MyNode->Low(mMyLow);
      MyNode->High(mMyHigh);
      }

    void InitORB()
      {
      if( mRecBisTree != NULL )
        {
        delete mRecBisTree;
        mRecBisTree = NULL;
        }

      mRecBisTree = new ORBNode();
      if( mRecBisTree == NULL )
        {
        PLATFORM_ABORT("mRecBisTree == NULL");
        }

//       int d = 3;
//       int levels = 0;
//       int t;
//       int NumbrOfProcs = Platform::Topology::GetSize();
//       do
//         {
//         levels++;
//         t = 1 << ( levels * d );
//         }
//       while ( NumbrOfProcs > t );
//       int base = ( levels - 1 ) * d;

//       while(d > 0)
//         {
//         t = 1 << ( base + d );
//         if( NumbrOfProcs == t ) break;
//         d--;
//         }
//       assert( d > 0 );

//       int N=levels;
//       mRecBisTree->id = -levels;
//       mRecBisTree->dims = d;

//       mRecBisTree->low[ 0 ] = 0.0;
//       mRecBisTree->low[ 1 ] = 0.0;
//       mRecBisTree->low[ 2 ] = 0.0;

//       mRecBisTree->voxnum[ 0 ] = mDynVarMgrIF->mVoxDimX;
//       mRecBisTree->voxnum[ 1 ] = mDynVarMgrIF->mVoxDimY;
//       mRecBisTree->voxnum[ 2 ] = mDynVarMgrIF->mVoxDimZ;

//       mRecBisTree->high[ 0 ] = mDynVarMgrIF->mDynamicBoxDimensionVector.mX;
//       mRecBisTree->high[ 1 ] = mDynVarMgrIF->mDynamicBoxDimensionVector.mY;
//       mRecBisTree->high[ 2 ] = mDynVarMgrIF->mDynamicBoxDimensionVector.mZ;

//       mRecBisTree->reciprocal[ 0 ] = 1.0/mDynVarMgrIF->mDynamicBoxDimensionVector.mX;
//       mRecBisTree->reciprocal[ 1 ] = 1.0/mDynVarMgrIF->mDynamicBoxDimensionVector.mY;
//       mRecBisTree->reciprocal[ 2 ] = 1.0/mDynVarMgrIF->mDynamicBoxDimensionVector.mZ;

//       // Setting proc dimensions so GenerateORB can use actual machine geometry

//       mRecBisTree->SetProcDims( ProcsX, ProcsY, ProcsZ );

      int ProcsX, ProcsY, ProcsZ; 
      Platform::Topology::GetDimensions( &ProcsX, &ProcsY, &ProcsZ );
      // mRecBisTree->SetProcDims( ProcsX, ProcsY, ProcsZ );
      mRecBisTree->SetOrbDims(mDynVarMgrIF->mDynamicBoxDimensionVector.mX,
            mDynVarMgrIF->mDynamicBoxDimensionVector.mY,
            mDynVarMgrIF->mDynamicBoxDimensionVector.mZ,
            mDynVarMgrIF->mVoxDimX,
            mDynVarMgrIF->mVoxDimY,
            mDynVarMgrIF->mVoxDimZ,
            ProcsX, ProcsY, ProcsZ );
      }

    void ClosestMidpoint(XYZ a, XYZ b, XYZ aCenter, XYZ &c)
      {
      XYZ d;
      NearestImageInPeriodicVolume(a,b,d);
      d.mX = (d.mX - a.mX) / 2.0 + a.mX;
      d.mY = (d.mY - a.mY) / 2.0 + a.mY;
      d.mZ = (d.mZ - a.mZ) / 2.0 + a.mZ;
      NearestImageInPeriodicVolume(aCenter,d,c);
      }

  // Return a 'kind' for the interaction, so that (e.g.) NSQ runtime can
  // optimise the Verlet list layout and analyse the effectiveness of the ORB    
  enum { 
    kInteractionKinds = 32 
  } ;
  static unsigned int ModelInteractionKind( int TargetI, int SourceI)
  {
    int mlid1 = MSD_IF::GetMoleculeId( TargetI );
    int mlid2 = MSD_IF::GetMoleculeId( SourceI );
    int at1 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
    int at2 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
    
    assert( at1 >= 1 ) ; 
    assert( at1 <= 4 ) ; 
    assert( at2 >= 1 ) ;
    assert( at2 <= 4 ) ; 
    
    unsigned int result =  ( ( (mlid1 == mlid2 ) << 4 ) & 0x10 )
                         | ( ( (at1 - 1        ) << 2 ) & 0x0c )
                         | (   (at2 - 1        )        & 0x03 ) ;
      BegLogLine(PKFXLOG_INTERACTION_KIND)
        << "InteractionKind mlid1=" << mlid1
        << " mlid2=" << mlid2
        << " at1=" << at1
        << " at2=" << at2
        << " result=" << result
        << EndLogLine ;
      return result ;
                                 
  }
      
  // Figure how we are going to do the load balance modelling
  enum {
    k_OrbTableModel = 
#if defined(USE_TABLE_MODEL)
        1
#else
        0
#endif	
       ,
        k_OrbVVLModel = 
#if defined(ORB_WEIGHT_FRAG)        
        0
#else
        1
#endif   
       ,                 	
        k_OrbVVLAAModel = 
#if defined(ORB_WEIGHT_VVL33)        
        0
#else
        1
#endif                    	
  } ;
  
#if 0
  static int CountSitesNotExcluded(int aTargetI, int aSourceI)
  {

      if ( aTargetI == aSourceI ) return 0 ; 
    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aSourceI );
    int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aTargetI );

      int result = 0 ; 

  
    for(int SourceFragmentSiteIndex = SourceFragmentSiteCount-1;
        SourceFragmentSiteIndex >= 0;
        SourceFragmentSiteIndex-- )
      {
      unsigned int SourceAbsSiteId =
        MSD_IF::GetIrreducibleFragmentMemberSiteId( aSourceI, SourceFragmentSiteIndex );
  
      // Scan the Target Fragment since that should be more local.
      for( int TargetFragmentSiteIndex = (TargetFragmentSiteCount-1);
           TargetFragmentSiteIndex >= 0 ;
           TargetFragmentSiteIndex-- )
        {
        unsigned int TargetAbsSiteId =
          MSD_IF::GetIrreducibleFragmentMemberSiteId( aTargetI, TargetFragmentSiteIndex );
  
        unsigned IsInExcList = MSD_IF::AreSitesInExcIncList( SourceAbsSiteId, TargetAbsSiteId );
        
        if ( 0 == IsInExcList ) 
        {
          result += 1 ;
        }
  
        }
      }
      return result ;
  }
    
    
    // Model the interaction weight for when we are doing 'vector verlet'
    // with all atom-atom
    double AAWeight(int aTargetAbsSiteId, int aSourceAbsSiteId)
    {

#if defined(AA_USE_LOADTUNE)    	
      double BelowLowerSwitchCost = Platform::LoadBalance::GetTune(0) ; // '2.0' would be reasonable
      double InSwitchCost = Platform::LoadBalance::GetTune(1) ; // '3.0' would be reasonable
#else
// Timings from a sample run with optimisations turned off
        double TimeToVectors =  174154.0 ; 
        double TimeTodEdr    =  541766.0 ;
        double TimeToS       =  679800.0 ; 
        double TimeToEnd     = 1108209.0 ;

// Derived timings
        double CostForAboveUpper = TimeToVectors ;
        double CostForBelowLower = TimeToEnd-(TimeToS-TimeTodEdr) ;
        double CostForInSwitch   = TimeToEnd ;
// Values for degree-of-freedom calculation      
        double BelowLowerSwitchCost = CostForBelowLower/CostForAboveUpper ; // 5.57
        double InSwitchCost = CostForInSwitch/CostForAboveUpper ; // 6.36
                
#endif    	
      double AboveUpperSwitchCost = 1.0 ; 
//    	XYZ TargetAtomLoc=mDynVarMgrIF->GetPositionFromFullTable(aTargetAbsSiteId) ;
//    	XYZ SourceAtomLoc=mDynVarMgrIF->GetPositionFromFullTable(aSourceAbsSiteId) ;
      XYZ TargetAtomLoc ;
      XYZ SourceAtomLoc ;
      TargetAtomLoc.mX = mDynVarMgrIF->PositionXFromFullTable(aTargetAbsSiteId) ;
      TargetAtomLoc.mY = mDynVarMgrIF->PositionYFromFullTable(aTargetAbsSiteId) ;
      TargetAtomLoc.mZ = mDynVarMgrIF->PositionZFromFullTable(aTargetAbsSiteId) ;
      SourceAtomLoc.mX = mDynVarMgrIF->PositionXFromFullTable(aSourceAbsSiteId) ;
      SourceAtomLoc.mY = mDynVarMgrIF->PositionYFromFullTable(aSourceAbsSiteId) ;
      SourceAtomLoc.mZ = mDynVarMgrIF->PositionZFromFullTable(aSourceAbsSiteId) ;
      double SquareDistance = NearestSquareDistanceInPeriodicVolume(TargetAtomLoc, SourceAtomLoc) ;

        double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
        double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
      
        double Cutoff = SwitchLowerCutoff + SwitchDelta;
        
        double VerletGuardZone = mDynVarMgrIF -> GetCurrentVerletGuardZone() ; 
        double VerletLimit = Cutoff + VerletGuardZone ;
        
        
//        double weight = ( SquareDistance < SwitchLowerCutoff*SwitchLowerCutoff )
//             ? BelowLowerSwitchCost
//             : ( SquareDistance < Cutoff*Cutoff )
//             ? InSwitchCost
//             : ( SquareDistance < VerletLimit*VerletLimit) 
//             ? AboveUpperSwitchCost 
//             : 0.0 ;
      double weight = fsel(
             (SwitchLowerCutoff*SwitchLowerCutoff-SquareDistance), BelowLowerSwitchCost
           , fsel(
               (Cutoff*Cutoff-SquareDistance), InSwitchCost
             ,  fsel(
                 (VerletLimit*VerletLimit-SquareDistance), AboveUpperSwitchCost, 0.0
                 ) 
               )
             ) ;
      BegLogLine(PKFXLOG_VERLET_LIST_DEBUG)
        << "ORB-estimate "
        << SquareDistance
        << " " << weight
        << " " 
          << EndLogLine ;
      return weight ; 
    }
    
  double ModelInteractionWeight_IFPWaterSite_AA(int TargetI, int SourceI)
  {
    int at1 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
    int at2 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
    int mlid1 = MSD_IF::GetMoleculeId( TargetI );
    int mlid2 = MSD_IF::GetMoleculeId( SourceI );
      int FirstSiteId1   = MSD_IF::GetIrreducibleFragmentFirstSiteId( TargetI );
      int FirstSiteId2   = MSD_IF::GetIrreducibleFragmentFirstSiteId( SourceI );

    double weight = 0.0 ; 
    if ( mlid1 != mlid2 )
    {
        // Frags in different molecules. There will be no exclusions
        for (int x1 = 0;x1 < at1 ; x1 += 1)
        {
          for (int x2 = 0 ; x2 < at2 ; x2 += 1)
          {
            weight += AAWeight(FirstSiteId1+x1,FirstSiteId2+x2) ;
          }
        }
    } else {
        // Frags in same molecule. Need to check for exclusions
        for (int x1 = 0;x1 < at1 ; x1 += 1)
        {
          for (int x2 = 0 ; x2 < at2 ; x2 += 1)
          {
            unsigned IsInExcList = MSD_IF::AreSitesInExcIncList(FirstSiteId1+x1,FirstSiteId2+x2);
            if ( 0 == IsInExcList )
            {
              weight += AAWeight(FirstSiteId1+x1,FirstSiteId2+x2) ;
            } 
          }
        }
        
    }
      return weight ;
  }
  
  double ModelInteractionWeight(int TargetI,int SourceI)
  {
    /// Alex - look here
    double ModWeight;
    int mlid1 = MSD_IF::GetMoleculeId( TargetI );
    int mlid2 = MSD_IF::GetMoleculeId( SourceI );
    int at1 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
    int at2 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
    
    if ( k_OrbVVLModel )
    {
      // 'Vector Verlet' model just looks how much will be processed with the '3:3' optimisation
      // (and at the moment guesses it; really it should take into account exclusions)
      if ( k_OrbVVLAAModel )
      {
//	  		ModWeight = at1*at2 ; 
            ModWeight = ModelInteractionWeight_IFPWaterSite_AA(TargetI,SourceI) ;
//           if ( mlid1 == mlid2 )
//           {
//             ModWeight = CountSitesNotExcluded(TargetI,SourceI) ;
//           } else {
//           	 ModWeight = at1*at2 ; 
//           }
      } else {
        ModWeight = ( 3 == at1 && 3 == at2 ) ? 1.55412 : ( 0.586728 * at1 * at2 ) ;
      }            
    } else {
    int atcount1 = (at1 < at2) ? at1 : at2;
    int atcount2 = (at1 < at2) ? at2 : at1;
    if(mlid1 == mlid2)
      {
        if ( k_OrbTableModel )
        {
        switch(atcount1)
    {
    case 1:  
      switch(atcount2)
        {
        case 1: ModWeight = 1.217803; break; // 1.072657; break;
        case 2: ModWeight = 1.657851; break; // 1.78807; break;
        case 3: ModWeight = 2.092172; break; // 2.285994; break;
        case 4: ModWeight = 2.678360; break; //2.968841; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    case 2:  
      switch(atcount2)
        {
        case 2: ModWeight = 2.678111; break; // 2.768698; break;
        case 3: ModWeight = 3.560940; break;  // 3.729389; break;
        case 4: ModWeight = 4.307348; break; //4.670057; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    case 3:  
      switch(atcount2)
        {
        case 3: ModWeight = 4.448243; break; // 5.2042 ; break;
        case 4: ModWeight = 5.990370; break; // 6.382139 ; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    case 4:  
      switch(atcount2)
        {
        case 4: ModWeight = 5.626951; break; // 7.762542; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    default:
      char msg[256]; 
      sprintf(msg,"Frag atom count out of range: %d\n", atcount1);		  
      PLATFORM_ABORT(msg);		  
    }
        } else {
        ModWeight = 1.28 + 0.406*(atcount1*atcount2);
        }
      }
    else
      {
       if ( k_OrbTableModel )
       {
        switch(atcount1)
    {
    case 1:  
      switch(atcount2)
        {
        case 1: ModWeight = 1.217803; break; // 1.072657; break;
        case 2: ModWeight = 1.657851; break; // 1.78807; break;
        case 3: ModWeight = 2.092172; break; // 2.285994; break;
        case 4: ModWeight = 2.678360; break; //2.968841; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    case 2:  
      switch(atcount2)
        {
        case 2: ModWeight = 2.678111; break; // 2.768698; break;
        case 3: ModWeight = 3.560940; break;  // 3.729389; break;
        case 4: ModWeight = 4.307348; break; //4.670057; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    case 3:  
      switch(atcount2)
        {
        case 3: ModWeight = 3.462162; break; // 3.439775; break;
        case 4: ModWeight = 5.990370; break; // 6.382139 ; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    case 4:  
      switch(atcount2)
        {
        case 4: ModWeight = 5.626951; break; // 7.762542; break;
        default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
          PLATFORM_ABORT(msg);		  
        }
      break;
    default:
      char msg[256]; 
      sprintf(msg,"Frag atom count out of range: %d\n", atcount1);		  
      PLATFORM_ABORT(msg);		  
    }
       } else {
        ModWeight = 1.28 + 0.294*(atcount1*atcount2);
       }
      }
    }
  
    return ModWeight;
  }
#endif	
  
    void CreateFirstOrb_Inline( int aSimTick )
      {
      int VoxelSpaceSize = mDynVarMgrIF->mVoxDimX
          * mDynVarMgrIF->mVoxDimY
          * mDynVarMgrIF->mVoxDimZ;

      BegLogLine( PKFXLOG_SETUP )
        << "Entering ORBManagerIF::CreateFirstOrb(): "
        << " VoxDimX: " << mDynVarMgrIF->mVoxDimX
        << " VoxDimY: " << mDynVarMgrIF->mVoxDimY
        << " VoxDimZ: " << mDynVarMgrIF->mVoxDimZ
        << " VoxelSpaceSize: " << VoxelSpaceSize 
        << EndLogLine;

      BegLogLine( PKFXLOG_SETUP )
        << "ORB Tuning Parameters "
        << Platform::LoadBalance::GetTune(0)
        << " " << Platform::LoadBalance::GetTune(1)
        << EndLogLine ;

      double VoxDimX = mDynVarMgrIF->mDynamicBoxDimensionVector.mX / mDynVarMgrIF->mVoxDimX;
      double VoxDimY = mDynVarMgrIF->mDynamicBoxDimensionVector.mY / mDynVarMgrIF->mVoxDimY;
      double VoxDimZ = mDynVarMgrIF->mDynamicBoxDimensionVector.mZ / mDynVarMgrIF->mVoxDimZ;

      InitORB();
            
      double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
      double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
      
// Try to feed the right interactions into the ORB weighting      
      double VerletGuardZone = mDynVarMgrIF -> GetCurrentVerletGuardZone() ; 
//      double Cutoff = SwitchLowerCutoff + SwitchDelta;
      double Cutoff = SwitchLowerCutoff + SwitchDelta + VerletGuardZone ;
      

      // Feed the orb a midpoints of voxel points of an intracting pair plus model weight      
//      double DistanceThresholds[NUMBER_OF_FRAGMENTS] ;
      int FragmentIndices[NUMBER_OF_FRAGMENTS] ;
      XYZAssign LocalFragCenters[NUMBER_OF_FRAGMENTS] ;
      double LocalExtents[NUMBER_OF_FRAGMENTS] ;
      
      for( int fragIdA = 0; fragIdA < NUMBER_OF_FRAGMENTS; fragIdA++ )
        {
        double ExtentA;
        XYZ FragCenterA;

        mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( fragIdA, &ExtentA, &FragCenterA );
        FragCenters[ fragIdA ] = FragCenterA;
        LocalFragCenters[ fragIdA ] = FragCenterA;
        Extents[ fragIdA ] = ExtentA;
        LocalExtents[ fragIdA ] = ExtentA;
        }


#if !( defined( EVEN_ORB ) || defined( PK_PHASE5_PROTOTYPE ))
//       // One pass for apoa 12 angs
//       int LEN_FOR_APOA = 2549880;      
//       mRecBisTree->SetLen( LEN_FOR_APOA );
//       CollectSize = 0;

// Figure how many items will be in the bis tree
        double dInd = 0.0 ; 
        {
          for( int fragIdA = 0; fragIdA < NUMBER_OF_FRAGMENTS; fragIdA++ )
          {
              double ExtentA = LocalExtents[ fragIdA ];
              XYZAssign FragCenterA = LocalFragCenters[ fragIdA ];
            for( int fragIdB = fragIdA; fragIdB < NUMBER_OF_FRAGMENTS; fragIdB++ )
            {
  #pragma execution_frequency(very_high) 
              double ExtentB = LocalExtents[ fragIdB ];
              XYZAssign FragCenterB = LocalFragCenters[ fragIdB ];
              
  //             mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( fragIdA, &ExtentA, &FragCenterA );
  //             mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( fragIdB, &ExtentB, &FragCenterB );
              
              XYZ FragCenterBImg;
              NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
              
              double thA= (Cutoff+ExtentA+ExtentB) ;
              
              double dsqThreshold = FragCenterA.DistanceSquared(FragCenterBImg) - (thA*thA) ;
              
//	            double DistanceThreshold = FragCenterA.Distance( FragCenterBImg ) + ExtentA + ExtentB - Cutoff;
              dInd += fsel(dsqThreshold, 0.0, 1.0) ;
              // Workaround 'errata' (performance problem)
              FragCenterB.Zero() ;
              FragCenterBImg.Zero() ;
            } 
          }
        }  
        int Ind = dInd ; 
// Set the bis tree to size         
          BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
              << "NumberOfInteractions: " << Ind
              << EndLogLine;
          
          mRecBisTree->SetLen( Ind );
// Fill in the bis tree
        unsigned int Ind2 = 0 ; 
        
        for( int fragIdA = 0; fragIdA < NUMBER_OF_FRAGMENTS; fragIdA++ )
          {
          int i=0 ; 
          double ExtentA = LocalExtents[ fragIdA ];
          XYZAssign FragCenterA = LocalFragCenters[ fragIdA ];
          for( int fragIdB = fragIdA; fragIdB < NUMBER_OF_FRAGMENTS; fragIdB++ )
            {
#pragma execution_frequency(very_high) 
              double ExtentB = LocalExtents[ fragIdB ];
              XYZAssign FragCenterB = LocalFragCenters[ fragIdB ];
              
  //             mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( fragIdA, &ExtentA, &FragCenterA );
  //             mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( fragIdB, &ExtentB, &FragCenterB );
              
              XYZ FragCenterBImg;
              NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
              
            double thA= (Cutoff+ExtentA+ExtentB) ;
            double dsqThreshold = FragCenterA.DistanceSquared(FragCenterBImg) - (thA*thA) ;
                
  //            double DistanceThreshold = FragCenterA.Distance( FragCenterBImg ) + ExtentA + ExtentB - Cutoff;
            FragmentIndices[i] = fragIdB ; 
            unsigned int iThreshold = * (unsigned int *) (&dsqThreshold) ;
            i += (iThreshold >> 31) ;
  //            DistanceThresholds[fragIdB] = dsqThreshold ;
                // Workaround 'errata' (performance problem)
              FragCenterB.Zero() ;
              FragCenterBImg.Zero() ;

            }
            BegLogLine(PKFXLOG_ORB_LOAD && (Platform::Topology::GetAddressSpaceId() == 0) )
              << "CreateFirstORB fragIdA=" << fragIdA
              << " i=" << i
              << EndLogLine ;
            { 
//	           for ( int fragIdB = fragIdA; fragIdB < NUMBER_OF_FRAGMENTS; fragIdB++ )
//	           {
//	            double DistanceThreshold = DistanceThresholds[fragIdB] ;
//	            if( DistanceThreshold < 0.0 )
             double Temp[3];
             Temp[ 0 ] = FragCenterA.mX;
             Temp[ 1 ] = FragCenterA.mY;
             Temp[ 2 ] = FragCenterA.mZ;
             int VoxelIdA = mRecBisTree->VoxelId( Temp );
               for ( int j = 0; j < i; j+=1)
               {
                {
                  int fragIdB =FragmentIndices[j] ;
                  double ModWeight = InteractionModel::GetWeight( fragIdA, fragIdB, mDynVarMgrIF );
                  if (  ModWeight > 0.0 )
                  {
                    // Add to the orb
                    XYZ FragCenterB = LocalFragCenters[ fragIdB ];
                    XYZ FragCenterBImg;
                    NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
    
                    double Temp[3];
                    
                    Temp[ 0 ] = FragCenterBImg.mX;
                    Temp[ 1 ] = FragCenterBImg.mY;
                    Temp[ 2 ] = FragCenterBImg.mZ;
                    int VoxelIdB = mRecBisTree->VoxelId( Temp );
                    
                    double vox_pt[3];
                    int VoxelId = mRecBisTree->GetMidpointOfVoxelPoints( VoxelIdA, VoxelIdB, vox_pt );
                    XYZ midXYZ;
                    midXYZ.mX = vox_pt[ 0 ] ;
                    midXYZ.mY = vox_pt[ 1 ] ;
                    midXYZ.mZ = vox_pt[ 2 ] ;
                    
                      BegLogLine(PKFXLOG_ORB_LOAD && (Platform::Topology::GetAddressSpaceId() == 0) )
                        << "SetSite " << Ind2 
                        << " " << midXYZ
                        << " " << ModWeight
                        << " " << fragIdA
                        << " " << fragIdB 
                        << EndLogLine
                    mRecBisTree->SetSite( Ind2++, midXYZ, ModWeight, fragIdA, fragIdB );
                                  // Workaround 'errata' (performance problem)
                  FragCenterB.Zero() ;
                  FragCenterBImg.Zero() ;
                  }
                }
               }
          }
        }
      // Revise the  ORB size downwards if necessary
      if ( Ind2 < Ind )
      {
        mRecBisTree->SetLen( Ind2 );
      }
      BegLogLine(PKFXLOG_SETUP)
       << "ORB configured size " << Ind
       << " loaded size " << Ind2
       << EndLogLine ; 
         
#endif

      GenerateORB();

      Platform::Control::Barrier();

#if defined(ORB_REPORT)
         if( Platform::Topology::GetAddressSpaceId() == 0 )
           mRecBisTree->ReportLeafData();
#endif           

      BegLogLine( 0 )
        << "Leaving CreateVerletListForOrb()"
        << EndLogLine;
      }

#if 0
    void
    AddToWeight( FragId aTargetI, FragId aSourceI, ORBWeightType aWeight )
      {
      XYZ FragCenterA;
      XYZ FragCenterB;
      double dummy;
      
      mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( aTargetI, &dummy, &FragCenterA );
      mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( aSourceI, &dummy, &FragCenterB );
      
      double Temp[3];
      Temp[ 0 ] = FragCenterA.mX;
      Temp[ 1 ] = FragCenterA.mY;
      Temp[ 2 ] = FragCenterA.mZ;
      int VoxelIdA = mRecBisTree->VoxelId( Temp );
      
      Temp[ 0 ] = FragCenterB.mX;
      Temp[ 1 ] = FragCenterB.mY;
      Temp[ 2 ] = FragCenterB.mZ;
      int VoxelIdB = mRecBisTree->VoxelId( Temp );
      
      BegLogLine( 0 )
          << "VoxelIdA: " << VoxelIdA
          << " VoxelIdB: " << VoxelIdB
          << " FragCenterA: " << FragCenterA
          << " FragCenterB: " << FragCenterB
          << EndLogLine;
      
      double vox_pt[3];
      int VoxelId = mRecBisTree->GetMidpointOfVoxelPoints( VoxelIdA, VoxelIdB, vox_pt );
      
      int VoxelIndexMid = mRecBisTree->VoxelIdToVoxelIndex( VoxelId );
      assert( VoxelIndexMid >= 0 && VoxelIndexMid < mVoxelSpaceSize );

      mOrbInputWeights[ VoxelIndexMid ] += aWeight;      
      }
#endif

#if 0
    void  
    RecookOrbBasedOnTimingDataVersion2()
      {      
      static ORBWeightType* TempWeights = (ORBWeightType *) malloc( sizeof( ORBWeightType ) * mVoxelSpaceSize );
      if( TempWeights == NULL )
        PLATFORM_ABORT("TempWeights == NULL");

      BegLogLine( 0 )
        << "mOrbInputWeights[ 1 ]: " << mOrbInputWeights[ 1 ]
        << EndLogLine;      
      
#ifdef PK_BGL      
      MPI_Allreduce( mOrbInputWeights, TempWeights, mVoxelSpaceSize, MPI_LONG_LONG, MPI_SUM, Platform::Topology::cart_comm );
#else
      MPI_Allreduce( mOrbInputWeights, TempWeights, mVoxelSpaceSize, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm );
#endif      
      memcpy( mOrbInputWeights, TempWeights, sizeof( ORBWeightType ) * mVoxelSpaceSize );

      BegLogLine( 0 )
        << " mOrbInputWeights[ 1 ]: " << mOrbInputWeights[ 1 ]
        << EndLogLine;      

      InitORB();

      mRecBisTree->SetLen( mVoxelSpaceSize );

      for( int j=0; j < mVoxelSpaceSize; j++ )
        {
        XYZ    midXYZ        = mOrbInputPoints[ j ];
        ORBWeightType Weight = mOrbInputWeights[ j ];

        assert( Weight > 0 );
        
        //BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
        BegLogLine( 0 )
          << "j: " << j
          << " midXYZ: " << midXYZ
          << " Weight: " << Weight
          << EndLogLine;

        mRecBisTree->SetSite( j, midXYZ, 1.0*Weight );
        }      

      Platform::Control::Barrier();
      
      GenerateORB();
#if defined(ORB_REPORT)
      if( Platform::Topology::GetAddressSpaceId() == 0 )
           mRecBisTree->ReportLeafData();
#endif           
      
      }
#endif

#if 0
    void
    RecookOrbBasedOnTimingData()
      {
      BegLogLine( PKFXLOG_SETUP )
        << "Entering RecookOrbBasedOnTimingData() "
        << EndLogLine;

        MPI_Allreduce( mCountPerNode, mTempCountPerNode, Platform::Topology::GetSize(), MPI_INT, MPI_SUM, Platform::Topology::cart_comm );
        memcpy( mCountPerNode, mTempCountPerNode, sizeof( int ) * Platform::Topology::GetSize() );

        mTotalFragWeightListSize = 0;
        for( int i=0; i<Platform::Topology::GetSize(); i++ )
          {
          mTotalFragWeightListSize += mCountPerNode[ i ];
          }

        if( mTotalFragWeightList != NULL )
          {
          delete [] mTotalFragWeightList;
          mTotalFragWeightList = NULL;
          }

        mTotalFragWeightList =  new FragWeight[ mTotalFragWeightListSize ];

        if ( mTotalFragWeightList == NULL )
          {
          PLATFORM_ABORT("mTotalFragWeightList == NULL");
          }

        if( mTempTotalFragWeightList != NULL )
          {
          delete [] mTempTotalFragWeightList;
          mTempTotalFragWeightList = NULL;
          }

        mTempTotalFragWeightList = new FragWeight[ mTotalFragWeightListSize ];
        if ( mTempTotalFragWeightList == NULL )
          {
          PLATFORM_ABORT("mTempTotalFragWeightList == NULL");
          }

        for( int i=0; i < mTotalFragWeightListSize; i++ )
          {
          mTotalFragWeightList[ i ].mTargetId = 0;
          mTotalFragWeightList[ i ].mSourceId = 0;         
          mTotalFragWeightList[ i ].mWeight   = 0;
          mTotalFragWeightList[ i ].mChecksum = 0;
          }

        // Calculate base
        int OffsetBase = 0;
        for( int i=0; i < mAddressSpaceId; i++ )
          {
          OffsetBase += mCountPerNode[ i ];
          }

        assert( mLocalFragWeightSize == mCountPerNode[ mAddressSpaceId ] );

        BegLogLine( 0 )
          << "mLocalFragWeightSize: " << mLocalFragWeightSize
          << " OffsetIntoTotalArray: " << OffsetBase
          << EndLogLine;

        for( int i=0; i < mLocalFragWeightSize; i++ )
          {
          int index = OffsetBase + i;
          assert( index < mTotalFragWeightListSize );

          unsigned int * ptr =  (unsigned int *) & mLocalFragWeight[ i ].mWeight;

          mLocalFragWeight[ i ].mChecksum = mLocalFragWeight[ i ].mTargetId +
            mLocalFragWeight[ i ].mSourceId +
            ptr[ 0 ] + ptr[ 1 ] + index;

          memcpy( &mTotalFragWeightList[ index ], &mLocalFragWeight[ i ], sizeof(FragWeight) );

          BegLogLine( 0 )
            << "mTotalFragWeightList[ " << index <<" ].mTargetId " << mTotalFragWeightList[ index ].mTargetId
            << " mTotalFragWeightList[ " << index <<" ].mSourceId " << mTotalFragWeightList[ index ].mSourceId
            << " mTotalFragWeightList[ " << index <<" ].mWeight " << mTotalFragWeightList[ index ].mWeight
            << " mTotalFragWeightList[ " << index <<" ].mChecksum " << mTotalFragWeightList[ index ].mChecksum
            << EndLogLine;
          }

//         int TotalLength = 0;
//         for( int i=0; i<Platform::Topology::GetSize(); i++)
//           {
//           TotalLength += mCountPerNode[ i ];
//           }

//         assert( TotalLength == mTotalFragWeightListSize );

        BegLogLine( PKFXLOG_SETUP )
          << "RecookOrbBasedOnTimingData(): About to reduce the frag timing array"
          << EndLogLine;

        ///////MPI_Allreduce( mTotalFragWeightList, mTempTotalFragWeightList , mTotalFragWeightListSize * sizeof( FragWeight) , MPI_CHAR, MPI_SUM, MPI_COMM_WORLD );

        if(  (((mTotalFragWeightListSize * sizeof( FragWeight)) / sizeof( int )) * sizeof( int )) != (mTotalFragWeightListSize * sizeof( FragWeight)) )
          PLATFORM_ABORT(" STRUCT REDUCED WITH INT TYPE IS NOT A ROUND MULTIPLE OF INTS");

        MPI_Allreduce( mTotalFragWeightList, mTempTotalFragWeightList , (mTotalFragWeightListSize * sizeof( FragWeight)) / sizeof( int ) , MPI_INT, MPI_SUM, Platform::Topology::cart_comm );

        memcpy( mTotalFragWeightList, mTempTotalFragWeightList, mTotalFragWeightListSize * (sizeof(FragWeight)));

        delete [] mTempTotalFragWeightList;
        mTempTotalFragWeightList = NULL;

        InitORB();

        BegLogLine( 0 )
          << "mTotalFragWeightListSize= " << mTotalFragWeightListSize
          << "sizeof(FragWeight)= " << sizeof( FragWeight )
          << EndLogLine;

        int VoxelSpaceSize = mDynVarMgrIF->mVoxDimX *
            mDynVarMgrIF->mVoxDimY *
            mDynVarMgrIF->mVoxDimZ;

        ORBVoxelInput* OrbInput = (ORBVoxelInput *) malloc( VoxelSpaceSize * sizeof(ORBVoxelInput));
        if( OrbInput == NULL )
          PLATFORM_ABORT( "OrbInput == NULL" );

#ifdef VOXEL_COUNT_TO_ORB
        mRecBisTree->SetLen( VoxelSpaceSize );
#else
        mRecBisTree->SetLen( mTotalFragWeightListSize );
#endif
        
        for( int i=0; i < VoxelSpaceSize; i++ )
          {
          OrbInput[ i ].mPoint = XYZ::ZERO_VALUE();
          OrbInput[ i ].mWeight = 0.0;
          }

        double TotalWeight = 0.0;

        double minW=9999999.9;
        double maxW=-9999999.9;
        double Sum=0;

        BegLogLine( PKFXLOG_SETUP )
          << "RecookOrbBasedOnTimingData(): About to loop over the frag timing array"
          << EndLogLine;

        for(int i=0; i < mTotalFragWeightListSize; i++)
          {
          int TargetI = (int) mTotalFragWeightList[ i ].mTargetId;
          int SourceI = (int) mTotalFragWeightList[ i ].mSourceId;
          
           #ifdef PK_BGL
             long long Weight = mTotalFragWeightList[ i ].mWeight;
           #else
             double Weight = mTotalFragWeightList[ i ].mWeight;
           #endif

          unsigned int * ptr =  (unsigned int *) & mTotalFragWeightList[ i ].mWeight;

          unsigned int checksum = TargetI + SourceI + ptr[ 0 ] + ptr[ 1 ] + i;

          if( checksum != mTotalFragWeightList[ i ].mChecksum )
            {
            BegLogLine( 1 )
              << " i " << i
              << " TargetI " << TargetI
              << " SourceI " << SourceI
              << " Weight " << Weight
              << " RecvChecksum " << mTotalFragWeightList[ i ].mChecksum
              << " CalcChecksum " << checksum
              << EndLogLine;

            PLATFORM_ABORT("checksum != mTotalFragWeightList[ i ].mChecksum");
            }

          if( Weight <= 0 )
            {
            BegLogLine( 1 )
              << " i " << i
              << "TargetI " << TargetI
              << " SourceI " << SourceI
              << " Weight " << Weight
              << EndLogLine;

            assert( 0 );
            }

          XYZ FragCenterA;
          XYZ FragCenterB;
          double dummy;

          mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( TargetI, &dummy, &FragCenterA );
          mDynVarMgrIF->GetDynamicFragmentExtentFragmentCenterVersion2( SourceI, &dummy, &FragCenterB );

          double Temp[3];
          Temp[ 0 ] = FragCenterA.mX;
          Temp[ 1 ] = FragCenterA.mY;
          Temp[ 2 ] = FragCenterA.mZ;
          int VoxelIdA = mRecBisTree->VoxelId( Temp );

          Temp[ 0 ] = FragCenterB.mX;
          Temp[ 1 ] = FragCenterB.mY;
          Temp[ 2 ] = FragCenterB.mZ;
          int VoxelIdB = mRecBisTree->VoxelId( Temp );

          BegLogLine( 0 )
            << "VoxelIdA: " << VoxelIdA
            << " VoxelIdB: " << VoxelIdB
            << " FragCenterA: " << FragCenterA
            << " FragCenterB: " << FragCenterB
            << EndLogLine;

          double vox_pt[3];
          int VoxelId = mRecBisTree->GetMidpointOfVoxelPoints( VoxelIdA, VoxelIdB, vox_pt );


#ifdef VOXEL_COUNT_TO_ORB
         int VoxelIndexMid = mRecBisTree->VoxelIdToVoxelIndex( VoxelId );
          mRecBisTree->VoxelPoint( VoxelId, vox_pt );
#endif

          XYZ midXYZ;
          midXYZ.mX = vox_pt[ 0 ] ;
          midXYZ.mY = vox_pt[ 1 ] ;
          midXYZ.mZ = vox_pt[ 2 ] ;

#ifdef VOXEL_COUNT_TO_ORB
         assert( VoxelIndexMid >= 0 && VoxelIndexMid < VoxelSpaceSize );
         OrbInput[ VoxelIndexMid ].mPoint  = midXYZ;
         OrbInput[ VoxelIndexMid ].mWeight += (1.0*Weight);
#endif
          //BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
          BegLogLine( 0 )
            << "ORBINPUT "
            << midXYZ 
            << " "
            << Weight
            << " "
            << EndLogLine;

#ifndef VOXEL_COUNT_TO_ORB
          
#if 0
          if( TargetI == 2706 && SourceI == 2708 )
            {
            BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
              << "TargetI= " << TargetI
              << " SourceI= " << SourceI
              << " midXYZ= " << midXYZ
              << " VoxelIdA= " << VoxelIdA
              << " VoxelIdB= " << VoxelIdB
              << " Weight= " << Weight
              << " FragCenterA: " << FragCenterA
              << " FragCenterB: " << FragCenterB
              << EndLogLine;
            }
#endif

#if 0
    double weight = 1.0*Weight;
          if(Platform::Topology::GetAddressSpaceId() == 0 )
      {
        printf("ORB %6.3f %6.3f %6.3f %8.2f %d %d %d %d\n",midXYZ.mX,midXYZ.mY,midXYZ.mZ,weight,
         MSD_IF::GetMoleculeId( TargetI ),
         MSD_IF::GetMoleculeId( SourceI ),
         MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI ),
         MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI )
         );
      }
#endif


#ifdef USE_MODEL_WEIGHTS

    ////  Alex - look here
    //// Use Frank's 4/22/05 slam dunk model for interaction cost to weight orb input	 
    double ModWeight = ModelInteractionWeight( TargetI, SourceI );	
#else
    double ModWeight = 1.0*Weight;
#endif
          mRecBisTree->SetSite( i, midXYZ, ModWeight, TargetI, SourceI );

#endif
          BegLogLine( 0 )
            << "midXYZ: " << midXYZ
            << EndLogLine;
          }
        
        delete [] mTotalFragWeightList;
        mTotalFragWeightList = NULL;

        BegLogLine( PKFXLOG_SETUP )
          << "RecookOrbBasedOnTimingData(): About to set orb input"
          << EndLogLine;

#ifdef VOXEL_COUNT_TO_ORB
        for( int j=0; j<VoxelSpaceSize; j++ )
          {
          XYZ    midXYZ = OrbInput[ j ].mPoint;
          double Weight = OrbInput[ j ].mWeight;
          
          BegLogLine( 0 )
            << "ORBINPUT "
            << midXYZ 
            << " "
            << Weight
            << " "
            << EndLogLine;
          
          mRecBisTree->SetSite( j, midXYZ, Weight );
          }
#endif        
        // PLATFORM_ABORT("END");

        GenerateORB();

#if defined(ORB_REPORT)
         if( Platform::Topology::GetAddressSpaceId() == 0 )
           mRecBisTree->ReportLeafData();
#endif           

//         int xMy, yMy, zMy;
//         Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
//         ORBNode* MyNode = mRecBisTree->NodeFromProcCoord( xMy, yMy, zMy );
//         MyNode->ReportLocalFrags();
        
        BegLogLine( PKFXLOG_SETUP )
          << "Leaving RecookOrbBasedOnTimingData() "
          << EndLogLine;
      }
#endif

  void CreateFirstOrb( int aSimTick ) 
#if defined(ORB_SEPARATE)
  ;
#else
    {
      CreateFirstOrb_Inline(aSimTick) ;
    }
#endif	

    void GenerateORB()
      {
      // mRecBisTree->SetWorkVolMixingParam(1.0);
#if ( defined( EVEN_ORB ) || defined( PK_PHASE5_PROTOTYPE ))
        mRecBisTree->SubdivideEvenly( 0 );
#else
//      BegLogLine( PKFXLOG_SETUP )
//        << "About to qsort orb input"
//        << EndLogLine;
//
//       qsort( mRecBisTree->SiteArray, mRecBisTree->nsites, sizeof( Site ), CompareX );

      BegLogLine( PKFXLOG_SETUP )
        << "About to subdivide orb input"
        << EndLogLine;

       mRecBisTree->Subdivide( 0 );

      // mRecBisTree->SetWorkVolMixingParam(1.0);
#endif

//       BegLogLine( PKFXLOG_SETUP )
//         << "About to NumberLeaves()"
//         << EndLogLine;

//       mRecBisTree->NumberLeaves( 0 );

//       BegLogLine( PKFXLOG_SETUP )
//         << "About to SetVoxelNums()"
//         << EndLogLine;
      
//       mRecBisTree->SetVoxelNums( 0, 0, 0 );

//       BegLogLine( PKFXLOG_SETUP )
//         << "About to UpdateNodeVerletCount()"
//         << EndLogLine;

//       mRecBisTree->UpdateNodeVerletCount();

//       mRecBisTree->WhipperUp();

      BegLogLine( PKFXLOG_SETUP )
        << "About to DeriveAllLoadZones()"
        << EndLogLine;

      int xMy, yMy, zMy;
      Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
      mRecBisTree->DeriveLoadZones( xMy, yMy, zMy );

      // mRecBisTree->DeriveAllLoadZones();
      ORBNode *nd = mRecBisTree->NodeFromProcCoord( xMy, yMy, zMy );
      if( !nd->nLoadVox )
        PLATFORM_ABORT("!nd->nLoadVox");

      SetMyOrbBox();
      
      BegLogLine( PKFXLOG_SETUP )
        << "Done creating the first orb"
        << EndLogLine;      
      }
  };
  
 
#if defined(ORB_SEPARATE) && defined(ORB_COMPILE)     
  
  template <class DynVarMgrIF>
      void T_ORBManagerIF<DynVarMgrIF>::CreateFirstOrb( int aSimTick )
      {
        CreateFirstOrb_Inline( aSimTick ) ;
      }
      
// This should cause the compiler to instantiate exactly the kind of CreateFirstORB that we need.
template
    void
    T_ORBManagerIF<
      TDynamicVariableManager<
       NUMBER_OF_SITES,
       NUMBER_OF_FRAGMENTS,
       NUMBER_OF_RESPA_LEVELS
      >
    >
    ::CreateFirstOrb
    (
      int aSimTick
    ) ;
#endif
  
#endif
