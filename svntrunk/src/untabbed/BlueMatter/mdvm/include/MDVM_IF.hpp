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


#ifndef PKFXLOG_MDVM_INTERNALS
#define PKFXLOG_MDVM_INTERNALS
#endif

#ifndef MDVM_IF_HPP
#define MDVM_IF_HPP

#define SITE_REG_COUNT   8
#define DIST_ARRAY_SIZE  (SITE_REG_COUNT * (SITE_REG_COUNT - 1)) / 2
#define INVALID_SITEID   -1


#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/MDVM_BaseIF.hpp>

#include <BlueMatter/UDF_Binding.hpp>

#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/PeriodicImage.hpp>

// This is the new MDVM state manager
class MDVM : public MDVM_BaseIF
{
public:

  int validationCheck;
  int invalidationMasks[ SITE_REG_COUNT ];
  double distArray[ DIST_ARRAY_SIZE ];

  int hit;
  int miss;

  double energyByUdfCode[ UDF_Binding::UDF_MAX_CODE + 1 ];

  MDVM() {}
  ~MDVM() {}


public:

    // yuzh: Let's see if this helps ...
    inline
    XYZ
    GetDynamicBoxDimensionVector()
      {
      return DynVarMgrIF.mDynamicBoxDimensionVector;
      }
    
    inline
    XYZ
    GetDynamicBoxInverseDimensionVector()
      {
      return DynVarMgrIF.mDynamicBoxInverseDimensionVector;
      }
    
    // This class contains the state managed per site
    class SiteReg
      {
      public:
      XYZ mPosition;
      XYZ mForce;
      SiteId mSiteOffset;
      };
    
private:
    int     mSiteRegMap [ SITE_REG_COUNT ];
    SiteReg mSiteRegSet [ SITE_REG_COUNT ];

  // Last energy reported by a MDVM UDF
  double mEnergy;

  // Time bookkeepping
  int mSimTick;
  int mTimeStep;

  XYZ mVirial;
  double mEwaldAlpha;
  
public:
  inline
  int
  GetIndex( const int regi, const int regj )
    {
      return ( regi * (regi - 1) )/2 + regj;
    }

  // Turns the ith bit in the int mask to zero.
  inline
  void
  TurnOffBit ( int& mask, const int i )
    {
      // Create a temp such as 11101111 where the 0 is in the ith place
      // AND it with the mask
      int temp = ~0; // create the 1111...1111
      temp ^= (1 << i);
      mask &= temp;
      return;
    }

  inline
  void
  CreateInvalidationMasks()
    {
      int r = 0;
      int i, j, index, mask;

      // Each iteration of the outer loop creates
      // the mask for rth register.
      while ( r < SITE_REG_COUNT )
        {
          // The mask starts out with all ones.
          // The invalid registers get turned off with a 0;
          mask = ~0;
          j = 0;
          while ( j < r )
            {
              index = GetIndex(r, j);
              TurnOffBit( mask, index );
              j++;
            }

          i = r + 1;
          while ( i < SITE_REG_COUNT )
            {

              index = GetIndex(i, r);
              TurnOffBit( mask, index );
              i++;
            }

          invalidationMasks[r] = mask;
          r++;
        }

      return;
    }


  inline
  void Init()
    {
      //      SetSiteRegMap(0,1,2,3);
      int i=0;
      int z=0;

      mEwaldAlpha = MSD_IF::GetEwaldAlpha() * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
      mEnergy = 0;

      ZeroEnergyAccumulators();

      for (i=0; i <  SITE_REG_COUNT; i++)
        {
          mSiteRegSet[ i ].mSiteOffset = 0;
          mSiteRegSet[ i ].mPosition.Zero();

          mSiteRegSet[ i ].mForce.Zero();          
        }

      validationCheck = 0;

      int length = DIST_ARRAY_SIZE;

      for (int j = 0; j < length; j++)
        {
          distArray[j] = -1;
        }

      // Create the invalidation Masks
      CreateInvalidationMasks();

      hit = miss = 0;
    }

inline
void ZeroEnergyAccumulators()
  {
  for (int i = 0; i <= UDF_Binding::UDF_MAX_CODE; i++)
    {
    energyByUdfCode[i] = 0.0;
    }
  }

inline
void ReportUdfEnergy( const int udfCode, const double energy)
{
  energyByUdfCode[ udfCode ] += energy;
}

inline
void
SetSimTick( int simTick )
{
  mSimTick = simTick;
}

inline
void SetTimeStep( int timeStep )
{
  mTimeStep = timeStep;
}

inline
XYZ &
GetPosition( const int aUserSite )
{
  int SiteRegSetIndex  = mSiteRegMap[ aUserSite ];
  XYZ & Position = mSiteRegSet[ SiteRegSetIndex ].mPosition;
  return( Position );
}

inline
void
SetVelocityRegister( int reg, XYZ velocity )
{
 assert(0);
}

inline
void
ReportVelocity( int reg, XYZ velocity )
{
 assert(0);
}

/// THIS SHOULD B$E CHANGED TO ACTUALLY SET THE FORCE AND NOT JUST ADD TO IT
inline
void
ReportForce( const int aUserSite, const XYZ& aForce )
{
  XYZ bForce = aForce ;
  int SiteRegSetIndex  = mSiteRegMap[ aUserSite ];
  mSiteRegSet[ SiteRegSetIndex ].mForce += bForce;
  return;
}

inline
MDVM::SiteReg*
AccessSiteReg( const int aRegisterIndex )
{
  return &(mSiteRegSet[ aRegisterIndex ]);
}

inline
XYZ &
AccessForceRegister( const int aRegisterIndex )
{
  return( mSiteRegSet[ aRegisterIndex ].mForce );
}

inline
XYZ &
GetForce( const int aRegisterIndex )
{
  XYZ & rc = mSiteRegSet[ aRegisterIndex ].mForce;
  
  return ( rc );
}

inline
XYZ
GetForceRegister( const int aRegisterIndex )
{
  return( mSiteRegSet[ aRegisterIndex ].mForce );
}

inline
void
SetForceRegister( const int aRegisterIndex, const XYZ force)
{
   mSiteRegSet[ aRegisterIndex ].mForce = force ;
}

inline
XYZ &
AccessPositionRegister( const int aRegisterIndex )
{
  return( mSiteRegSet[ aRegisterIndex ].mPosition );
}

inline
XYZ
GetPositionRegister( const int aRegisterIndex )
{
  return( mSiteRegSet[ aRegisterIndex ].mPosition );
}

inline
void
SetPositionRegister( const int aRegisterIndex, const XYZ position )
{
  mSiteRegSet[ aRegisterIndex ].mPosition = position;
}

inline
SiteId &
AccessSiteOffsetRegister( const int aRegisterIndex )
{
  return( mSiteRegSet[ aRegisterIndex ].mSiteOffset );
}

inline
int 
GetSimTick()
  {
  return mSimTick;
  }

inline
SiteId
GetSiteOffsetRegister( const int aRegisterIndex )
{
  return( mSiteRegSet[ aRegisterIndex ].mSiteOffset );
}

inline
void
SetSiteIdInRegister( const int aRegisterIndex, const SiteId aSiteId)
{
  mSiteRegSet[ aRegisterIndex ].mSiteOffset = aSiteId;
}

inline
void
ReportVirial( XYZ aVirial )
  {
  mVirial = aVirial;
  }

inline
XYZ
GetVirial()
{
  return( mVirial );
}

inline
void
ReportEnergy( double aEnergy )
{
  mEnergy = aEnergy;
}

inline
double
GetEwaldAlpha() 
{
  return mEwaldAlpha;  
}


inline
double
GetEnergy()
{
  return( mEnergy );
}

inline
void
SetEnergy( double aEnergy )
{
  mEnergy = aEnergy;
}

inline
double &
AccessEnergy()
{
  return( mEnergy );
}

inline
void
SetSiteRegMap( int aUserSiteInReg0,
               int aUserSiteInReg1,
               int aUserSiteInReg2,
               int aUserSiteInReg3 )
{
  mSiteRegMap[ 0 ] = aUserSiteInReg0;
  mSiteRegMap[ 1 ] = aUserSiteInReg1;
  mSiteRegMap[ 2 ] = aUserSiteInReg2;
  mSiteRegMap[ 3 ] = aUserSiteInReg3;
}

inline
void
SetSiteForRegMap( const int site, const int aUserSiteInReg )
{
  mSiteRegMap[ site ] = aUserSiteInReg;
}

inline
double
GetImageDistanceSquared( const int aUserSite0, const int aUserSite1 )
{
  XYZ pA = GetPosition( aUserSite0 );
  XYZ pB = GetPosition( aUserSite1 );
  XYZ pImageB;
  NearestImageInPeriodicVolume( pA, pB, pImageB );
  XYZ vAB = pImageB - pA;

  return vAB.LengthSquared();
}

inline
double
GetDistanceSquared( const int aUserSite0, const int aUserSite1 )
{
  XYZ pA = GetPosition( aUserSite0 );
  XYZ pB = GetPosition( aUserSite1 );
  XYZ pImageB;
  //NearestImageInPeriodicVolume( pA, pB, pImageB );
  XYZ vAB = pB - pA;

  return vAB.LengthSquared();
}

inline
int IsCached( const int index )
{
  int validBit = (validationCheck >> index) & ( 0x1 );

  return ( validBit );
}

inline
double
GetCacheDistance( const int index )
{
  double distance = distArray[ index ];
  return ( distance );
}

inline
void
SetCacheDistance( const int index, const double dist )
{
  distArray[ index ] = dist;

  // there is a valid caching at the indexed location
  validationCheck |= (0x1 << index);
  return;
}

inline
double
GetDistance2( const int aUserSite0, const int aUserSite1 )
{
  // Caching but keeping a full matrix
  // Problems: A branch to order the registers.
  // Benefits: Access is fast and cheap.
  int regi = mSiteRegMap[aUserSite0];
  int regj = mSiteRegMap[aUserSite1];

  assert( regi != regj );

  if ( regi < regj )
    {
      // register i always has to be greater then register j
      int temp = regj;
      regj = regi;
      regi = temp;
    }


  // All the helper functions assume that the bigger register is
  // the first argument

  int index = GetIndex ( regi, regj );

  if ( IsCached( index ) )
    {
      hit++;
      return GetCacheDistance( index );
    }

  miss++;

  XYZ Vector01  = GetVector( aUserSite0, aUserSite1 );
  double distance01 = Vector01.Length();

  SetCacheDistance( index , distance01 );

  return( distance01 );
}

inline
double
GetDistance( const int aUserSite0, const int aUserSite1 )
{
  return GetDistance2( aUserSite0, aUserSite1 );
}

inline
void
ClearDistanceArrayForRegister ( const int reg )
{
  int invalidRegs = invalidationMasks[ reg ];
  validationCheck &= invalidRegs;
  return;
}

inline
void
InvalidateDistanceForRegister( const int reg )
{
  ClearDistanceArrayForRegister( reg );
}

inline
XYZ
GetVector( int aUserSite0, int aUserSite1 )
{
  // For now, just compute, but should do caching soon.
  XYZ Position0 = GetPosition( aUserSite0 );
  XYZ Position1 = GetPosition( aUserSite1 );

  XYZ vector01 = Position0 - Position1;

  return( vector01 );
}

inline
void
ExportEnergy( unsigned aOuterTimeStep )
{
  double KineticEnergy = 0.0;

  for(int udfCode=0; udfCode <= UDF_Binding::UDF_MAX_CODE; udfCode++)
    {
    if( MSD_IF::IsUDFInUse( MSD_IF::LISTBASEDDRIVER, udfCode ) )
      {
      ED_Emit_UDFd1( aOuterTimeStep, udfCode, 1u, energyByUdfCode[ udfCode ]);
      }
    }
}

inline
int
GetSiteId(int aUserSite0 )
{
  assert(0);
  return 0;
}

inline
XYZ &
GetVelocity(int aUserSite0 )
{
  assert(0);
  // Just to get it to compiler without warnings
  return  mSiteRegSet[ 0 ].mPosition;
}

inline
double
GetDeltaT()
{
  assert(0);
  return 0.0;
}

inline
double
GetHalfInverseMass( int aUserSite0 )
{
  assert(0);
  return 0.0;
}

};

// MDVM for 'vectorised 2-site', which deals in E and dE/dr 
class MDVM_BOND_2 : public MDVM_BaseIF
{
  public: 
  
  double mEnergy ; 
  double mdEdr ;
  double mDistance ;
  double mRecipDistance ;
  inline void Init(void) 
  {
  } 
  inline void Prime(double aDistance, double aRecipDistance )
  {
    mDistance = aDistance ;
    mRecipDistance = aRecipDistance ;
  }
  
  
  inline double GetLength ( void ) 
  {
    return mDistance ; 
  } 
  inline double GetRecipLength ( void ) 
  {
    return mRecipDistance ; 
  } 
  inline double GetEnergy( void )
  {
    return mEnergy ; 
  }
  inline double GetdEdr ( void )
  { 
    return mdEdr ; 
  } 
    inline void   ReportEnergy( double aEnergy )
    {
      mEnergy = aEnergy ;
    }
    inline void   ReportdEdr( double adEdr )
    {
      mdEdr = adEdr ;
    }
  
} ; 
template<int BondCount>
class MDVM_BOND_N: public MDVM_BaseIF
{
  public:
  double mEnergy ;
  double mDistance[BondCount] ;
  double mRecipDistance[BondCount] ;
  XYZ mVector[BondCount] ;
  XYZ mForce[BondCount+1] ;
  
  inline void Init(void)
  { 
  } 
  double GetEnergy ( void )
  {
    return mEnergy ;
  }
  double GetLengthN ( unsigned int index ) 
  {
    return mDistance[index] ; 
  } 
  double GetRecipLengthN ( unsigned int index ) 
  {
    return mRecipDistance[index] ; 
  }
  XYZ GetVectorN( unsigned int index )
  {
    return mVector[index] ; 
  } 
  XYZ& GetForce( unsigned int index )
  {
    return mForce[index] ; 
  }
  
  void ReportEnergy(double aEnergy) 
  {
    mEnergy = aEnergy ; 
  }
  void SetVector(unsigned int index, XYZ aVector, double aLength, double aRecipLength)
  {
    mVector[index] = aVector ;
    mDistance[index] = aLength ;
    mRecipDistance[index] = aRecipLength ;
  }
  void ReportForce(unsigned int index, XYZ aForce)
  {
    mForce[index] = aForce ;
  }
} ;
#endif
