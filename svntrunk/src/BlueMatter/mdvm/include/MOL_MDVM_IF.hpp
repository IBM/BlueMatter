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
 * Project:         BlueMatter
 *
 * Module:          MDVM
 *
 * Purpose:         Molecular Dynamics Virtual Machine - Registers
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010522 BGF Created from design.
 *
 ***************************************************************************/

// NOTE:  This file cannot have knowledge of the direct Binding info in the new world order.
// Such knowledge is brought in during template instantiation in the MSD file.

#ifndef MOL_MDVM_IF_HPP
#define MOL_MDVM_IF_HPP

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/MDVM_BaseIF.hpp>

#define NUM_MOL 2

// This is the new MDVM state manager
class MOL_MDVM : public MDVM_BaseIF
{
  double mEnergy;
  SiteId mMolFirstSiteId[ NUM_MOL ];
  int    mMolSiteCount[ NUM_MOL ];
  XYZ    mPos0[ NUM_MOL ];;

public:
  MOL_MDVM() {}
  ~MOL_MDVM() {}

  void
  Prime( int aMolFirstSiteId0, int aMolSiteCount0, 
         int aMolFirstSiteId1, int aMolSiteCount1 )
    {
    mMolFirstSiteId[ 0 ] = aMolFirstSiteId0;
    mMolSiteCount[ 0 ]   = aMolSiteCount0;
    mMolFirstSiteId[ 1 ] = aMolFirstSiteId1;
    mMolSiteCount[ 1 ]   = aMolSiteCount1;
    mEnergy         = 0.0;

    for( int i=0; i<NUM_MOL; i++ )
      {
      FragId fId = DynVarMgrIF.mSiteIdToFragmentRepMap[ mMolFirstSiteId[ i ] ].mFragId;
      
      if( !DynVarMgrIF.mInteractionStateManagerIF.IsFragmentInRange( fId ) )
        {
        PLATFORM_ABORT( "COM Restrain fragment is NOT in range" );
        }
      
      mPos0[ i ] = DynVarMgrIF.GetInteractionPositionForSite( mMolFirstSiteId[ i ] );
      }
    }

  int
  GetMoleculeSiteCount( int aMolOrd )
    {
    assert( aMolOrd >= 0 && aMolOrd < NUM_MOL );
    return mMolSiteCount[ aMolOrd ];
    }

    
  double 
  GetMass( int aMolOrd, int aIndex )
    {
    assert( aMolOrd >= 0 && aMolOrd < NUM_MOL );
    SiteId s = mMolFirstSiteId[ aMolOrd ] + aIndex;
    return MSD_IF::GetMass( s );
    }
    
  XYZ 
  GetPosition( int aMolOrd, int aIndex )
    {
    assert( aMolOrd >= 0 && aMolOrd < NUM_MOL );
    SiteId s = mMolFirstSiteId[ aMolOrd ] + aIndex;

    assert( s >=0 && s < NUMBER_OF_SITES );
    
    FragId fId = DynVarMgrIF.mSiteIdToFragmentRepMap[ s ].mFragId;

    if( !DynVarMgrIF.mInteractionStateManagerIF.IsFragmentInRange( fId ) )
      {
      PLATFORM_ABORT( "COM Restrain fragment is NOT in range" );
      }

    XYZ & Pos = DynVarMgrIF.GetInteractionPositionForSite( s );

    XYZ PosImaged;
    NearestImageInPeriodicVolume( mPos0[ aMolOrd ], Pos, PosImaged );

    BegLogLine( 0 )
        << "aIndex: " << aIndex
        << " mMolFirstSiteId: " << mMolFirstSiteId[ aMolOrd ]
        << " s: " << s
        << " pos: " << Pos
        << EndLogLine;

    return PosImaged;
    }
    
  XYZ 
  GetDimensionVector()
    {
    return DynVarMgrIF.mDynamicBoxDimensionVector;
    }

  void
  ReportForce( int aMolOrd, int aIndex, XYZ& aForce )
    {
    SiteId s = mMolFirstSiteId[ aMolOrd ] + aIndex;
    return DynVarMgrIF.AddForceForSite( s, aForce );    
    }

  void
  ReportEnergy( double aEnergy )
    {
    mEnergy = aEnergy;
    }
    
  double  
  GetEnergy()
    {
    return mEnergy;
    }
};
#endif
