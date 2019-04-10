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
 * Module:          MDVM_UVP
 *
 * Purpose:         Molecular Dynamics Virtual Machine - Registers
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010815 AR Created from design.
 *
 ***************************************************************************/

#ifndef PKFXLOG_MDVM_INTERNALS
#define PKFXLOG_MDVM_INTERNALS
#endif

#ifndef MDVM__UVP_IF_HPP
#define MDVM__UVP_IF_HPP

#define SITE_REG_COUNT   8
#define INVALID_SITEID   -1

#define DISTANCE_OPTION_2

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/MDVM_BaseIF.hpp>
#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
//#include <BlueMatter/RTPFile.hpp>
//#include <BlueMatter/MSD_IF.hpp>

// This is the new MDVM Update Velocities Positions state manager
class MDVM_UVP : public MDVM_BaseIF
{
public:
  enum { SiteA = 0, SiteB = 1, SiteC = 2, SiteD = 3 };

  MDVM_UVP() {}
  ~MDVM_UVP() {}

public:
  // This class contains the state managed per site
  class SiteReg
  {
  public:
    XYZ mPosition;
    XYZ mVelocity;
    SiteId mSiteId;
  };

private:
  //  int     mSiteRegMap [ SITE_REG_COUNT ];
  SiteReg mSiteRegSet [ SITE_REG_COUNT ];
    
  // Time bookkeepping
  int mSimTick;
  double mInnerTimeStep;

    tXYZ<double> zero;

public:
  void Init()
    {
      int i=0;
      for (i=0; i <  SITE_REG_COUNT; i++)
        {
          mSiteRegSet[i].mSiteId = 0;
          mSiteRegSet[i].mPosition.Zero();
          mSiteRegSet[i].mVelocity.Zero();
        }

      mInnerTimeStep = MSD_IF::GetConvertedInnerTimeStep();
      zero.mX = 0.0;
      zero.mY = 0.0;
      zero.mZ = 0.0;
      //tXYZ<double> rc = {0.0,0.0,0.0};
    }

inline
void
SetSimTick( int simTick )
{
  mSimTick = simTick;
}

inline
void
SetSiteRegMap( int a, int b, int c, int d) 
    {
  
    }

inline
void
SetSiteIdInRegister( int reg, int siteId )
{
  mSiteRegSet[ reg ].mSiteId = siteId;
}

inline
void
SetPositionRegister( int reg, XYZ position )
{
  mSiteRegSet[ reg ].mPosition = position;
}

inline 
XYZ&
AccessForceRegister( int reg )
  {
  return zero;
  }

inline
void
SetForceRegister( int reg, XYZ val)
  {
  }



inline
void
ReportPosition(int siteOrd, XYZ position) 
  {  
  SetPositionRegister( siteOrd, position );
  }

inline
void
SetVelocityRegister( int reg, XYZ velocity )
{
  mSiteRegSet[ reg ].mVelocity = velocity;
}

inline
void
ReportVelocity( int reg, XYZ velocity )
{
  mSiteRegSet[ reg ].mVelocity = velocity;
}

inline
void
SetInnerTimeStep( int timeStep )
{
  mInnerTimeStep = timeStep;
}

inline
XYZ &
GetPosition( const int aUserSite )
  {
  //  int SiteRegSetIndex  = mSiteRegMap[ aUserSite ];
  XYZ & Position = mSiteRegSet[ aUserSite ].mPosition;
  return( Position );
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
XYZ
GetVirial()
{
  assert( 0 );  
  return( XYZ::ZERO_VALUE() );
}

inline
XYZ &
GetVelocity(int aUserSite0 )
{
  XYZ & vel = mSiteRegSet[ aUserSite0 ].mVelocity ;
  return vel;
}

inline
SiteId &
GetSiteId(int aUserSite0 )
{
  SiteId & siteId = mSiteRegSet[ aUserSite0 ].mSiteId ;
  return siteId;
}

inline 
double
GetEnergy()
{
    return 0.0;
}

inline
double
GetDeltaT()
{
  return mInnerTimeStep;
}

inline
double
GetHalfInverseMass( int aUserSite0 )
{
  SiteId absId = mSiteRegSet[ aUserSite0 ].mSiteId;
  return MSD_IF::GetHalfInverseMass( absId );
}



// inline
// void
// SetVirial(double virial)
// {
//   mVirial = virial;
// }

// inline
// void
// SetDHDLambda(double lambda)
// {
//   mDHDLambda = lambda;
// }

};
#endif
