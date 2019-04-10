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

#include <BlueMatter/DynamicVariableManager_phase5.hpp>

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

  void
  Init( DynVarMgrIF* aDynVarMgrIF )
  {
    BegLogLine( PKFXLOG_SETUP )
      << "Entering ORBManagerIF::Init(): "
      << EndLogLine;

    mAddressSpaceId = Platform::Topology::GetAddressSpaceId();

    mDynVarMgrIF = aDynVarMgrIF;
    mOrbVerletListSize = 0;

    mRecBisTree = NULL;
      
    int VoxDimX = mDynVarMgrIF->mVoxDimX;
    int VoxDimY = mDynVarMgrIF->mVoxDimY;
    int VoxDimZ = mDynVarMgrIF->mVoxDimZ;

    mVoxelSpaceSize = VoxDimX * VoxDimY * VoxDimZ;
         
    CreateFirstOrb( 0 );      
  }

  ORBNode* 
  GetOrbRoot()
  {
    assert( mRecBisTree != NULL );
    return mRecBisTree;
  }

  int 
  IsSiteHere( XYZ aSite )
  {
    int rc = ( ( aSite.mX >= myLow[ 0 ] && aSite.mX < myHigh[ 0 ] ) &&
	       ( aSite.mY >= myLow[ 1 ] && aSite.mY < myHigh[ 1 ] ) &&
	       ( aSite.mZ >= myLow[ 2 ] && aSite.mZ < myHigh[ 2 ] ) );

    return rc;
  }

  void 
  SetMyOrbBox()
  {
    int xMy, yMy, zMy;
    Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
    ORBNode* MyNode = mRecBisTree->NodeFromProcCoord( xMy, yMy, zMy );
    MyNode->Low(mMyLow);
    MyNode->High(mMyHigh);
  }

  void 
  InitORB()
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

    int ProcsX, ProcsY, ProcsZ; 
    Platform::Topology::GetDimensions( &ProcsX, &ProcsY, &ProcsZ );
    mRecBisTree->SetOrbDims(mDynVarMgrIF->mDynamicBoxDimensionVector.mX,
			    mDynVarMgrIF->mDynamicBoxDimensionVector.mY,
			    mDynVarMgrIF->mDynamicBoxDimensionVector.mZ,
			    mDynVarMgrIF->mVoxDimX,
			    mDynVarMgrIF->mVoxDimY,
			    mDynVarMgrIF->mVoxDimZ,
			    ProcsX, ProcsY, ProcsZ );
  }

  void 
  ClosestMidpoint(XYZ a, XYZ b, XYZ aCenter, XYZ &c)
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
  enum 
    { 
      kInteractionKinds = 32 
    } ;

  static 
  unsigned int 
  ModelInteractionKind( int TargetI, int SourceI)
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

    InitORB();

#if 0
#if !( defined( EVEN_ORB ) || defined( PK_PHASE5_PROTOTYPE ))
    BegLogLine( PKFXLOG_SETUP )
      << "ORB Tuning Parameters "
      << Platform::LoadBalance::GetTune(0)
      << " " << Platform::LoadBalance::GetTune(1)
      << EndLogLine ;

    double VoxDimX = mDynVarMgrIF->mDynamicBoxDimensionVector.mX / mDynVarMgrIF->mVoxDimX;
    double VoxDimY = mDynVarMgrIF->mDynamicBoxDimensionVector.mY / mDynVarMgrIF->mVoxDimY;
    double VoxDimZ = mDynVarMgrIF->mDynamicBoxDimensionVector.mZ / mDynVarMgrIF->mVoxDimZ;
            
    double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
    double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
      
    // Try to feed the right interactions into the ORB weighting      
    double VerletGuardZone = mDynVarMgrIF->GetCurrentVerletGuardZone() ; 
    double Cutoff = SwitchLowerCutoff + SwitchDelta + VerletGuardZone ;      

    // Feed the orb a midpoints of voxel points of an intracting pair plus model weight      
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
	            	            
	      XYZ FragCenterBImg;
	      NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
	            
	      double thA= (Cutoff+ExtentA+ExtentB) ;
	            
	      double dsqThreshold = FragCenterA.DistanceSquared(FragCenterBImg) - (thA*thA) ;
	            
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
	            
	    XYZ FragCenterBImg;
	    NearestImageInPeriodicVolume( FragCenterA, FragCenterB, FragCenterBImg );
	            
	    double thA= (Cutoff+ExtentA+ExtentB) ;
	    double dsqThreshold = FragCenterA.DistanceSquared(FragCenterBImg) - (thA*thA) ;
		            
	    FragmentIndices[i] = fragIdB ; 
	    unsigned int iThreshold = * (unsigned int *) (&dsqThreshold) ;
	    i += (iThreshold >> 31) ;
	    // Workaround 'errata' (performance problem)
	    FragCenterB.Zero() ;
	    FragCenterBImg.Zero() ;

	  }
	BegLogLine(PKFXLOG_ORB_LOAD && (Platform::Topology::GetAddressSpaceId() == 0) )
	  << "CreateFirstORB fragIdA=" << fragIdA
	  << " i=" << i
	  << EndLogLine ;
	{ 
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
    mRecBisTree->SubdivideEvenly( 0 );

    BegLogLine( PKFXLOG_SETUP )
      << "About to DeriveAllLoadZones()"
      << EndLogLine;

    int xMy, yMy, zMy;
    Platform::Topology::GetCoordsFromRank( Platform::Topology::GetAddressSpaceId(), &xMy, &yMy, &zMy );
    mRecBisTree->DeriveLoadZones( xMy, yMy, zMy );

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
