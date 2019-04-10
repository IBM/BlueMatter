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
 * Module:          NBVM
 *
 * Purpose:         NonBonded Virtual Machine interface.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010522 BGF Created from design.
 *
 ***************************************************************************/

// This file contains active message infrastructure and nonbonded engine
// thread code.

// Currently, this code does not have an initialization phase.  This leads to
// some extra data in operand packets and also makes it impossible to
// dynamically allocate some of the structures.  These are allocated based
// on a compilte time limit.  THIS CAN ALL BE CHANGED AS WE GO ALONG.

#ifndef _PLIMPNONBONDEDVIRTUALMACHINE_HPP_
#define _PLIMPNONBONDEDVIRTUALMACHINE_HPP_

#ifndef PKFXLOG_NBVM_PRIME
#define PKFXLOG_NBVM_PRIME 0
#endif

#ifndef PKFXLOG_NBVM
#define PKFXLOG_NBVM 0
#endif

#ifndef PKFXLOG_PRESSURE_CONTROL
#define PKFXLOG_PRESSURE_CONTROL 0
#endif

#ifndef PKFXLOG_REGRESS_UDF_RESULTS
#define PKFXLOG_REGRESS_UDF_RESULTS 0
#endif


#include <BlueMatter/MDVM_BaseIF.hpp>
#include <BlueMatter/XYZ.hpp>
#include <pk/fxlogger.hpp>


//*****************************************************************************

template<int UDF_COUNT>
class NBVM : public MDVM_BaseIF
  {
    public:
      enum { NBVM_SITE_REG_COUNT = 2 };
      enum { SiteA = 0, SiteB = 1, SiteC = -1, SiteD = -1 };

      enum
      {
          kForceFullyOn = 0 ,
          kSuppressVectorReporting = 0
      } ;

      // This class contains the state managed per site
      class SiteReg
      {
          public:
          XYZ      mForce;
      };

      XYZ    mCachedVectorAB;
      double mCachedSquareDistance;
      double mCachedReciprocalDistance;
      //    double mCachedDistance;

    // SwitchFunctionRadii & mSwitchFunctionRadii;
    SwitchFunction        mSF;

#if defined(PERFORMANCE_RECORD_A_ONLY)
    SiteReg mSiteRegA ;
#else
    SiteReg mSiteRegSet [ NBVM_SITE_REG_COUNT ];
#endif

    // Last energy reported by a MDVM UDF

// Use of mCurrentUdfOrdinal was inhibiting optimisation
    //#if !defined(PERFORMANCE_VECTOR_NBVM)
    int mCurrentUdfOrdinal;
// #else
    double mLastEnergy ;
// #endif
    // Last rate-of-change-of-energy-with-distance reported by a UDF
    // The intent is that if energy increases as the atom-atom distance increases, dEdr will be positive
    double mdEdr;

    double mEnergy[ UDF_COUNT ];
    XYZ mVirial[ UDF_COUNT ];

    SiteId mSiteIds[ 2 ];

#if defined(PERFORMANCE_VECTOR_NBVM)
    XYZ mLastVirial ;
#endif

    XYZ mVVirial ;

    // Results of switch evaluation
    double SwitchS ;
    double SwitchdSdR ;


#if !defined(PERFORMANCE_NO_NBVM_SIMTICK)
    int mSimTick;
#endif

//     inline
//     void
//     GetSwitchFunctionRadii( SwitchFunctionRadii&  aResult )
//     {
//       aResult = mSwitchFunctionRadii;
//     }

    inline
    SiteId
    GetSiteId( int index )
      {
      assert( index >= 0 && index < 2 );
      return mSiteIds[ index ];
      }

    inline
    void
    SetSiteId( int index, SiteId aSiteId )
      {
      assert( index >= 0 && index < 2 );
      mSiteIds[ index ] = aSiteId;
      }

    inline
    void
    SetSwitchFunctionRadii( SwitchFunctionRadii &aSwitchFunctionRadii )
    {
      // mSwitchFunctionRadii = aSwitchFunctionRadii;
        mSF.Set( aSwitchFunctionRadii );
    }

    inline
    void
    EvaluateSwitch(double aDistance, double &aS, double &aPdSdR)
    {
      mSF.Evaluate(aDistance, aS, aPdSdR);
    }

    inline
    double
    GetEwaldAlpha()
      {
      return MSD_IF::GetEwaldAlpha() * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
      }

    inline
    void
    Prime(const XYZ& aVectorAB)
      {

      for(int i = 0; i < UDF_COUNT; i++ )
        {
#if defined(PERFORMANCE_VECTOR_NBVM)
        mVirial[ i ].Zero();
#endif
        mEnergy[ i ] = 0.0;
        }

#if defined(PERFORMANCE_VECTOR_NBVM)
            mLastVirial.Zero() ;
#endif
#if defined(PERFORMANCE_RECORD_A_ONLY)
      mSiteRegA.mForce.Zero() ;
#else
      mSiteRegSet[0].mForce.Zero();
      mSiteRegSet[1].mForce.Zero();
#endif

      mVVirial.Zero() ;

      mCachedVectorAB = aVectorAB;

      mCachedSquareDistance = aVectorAB.LengthSquared() ;
      mCachedReciprocalDistance = 1.0/sqrt(mCachedSquareDistance) ;
//      mCachedDistance = mCachedReciprocalDistance * mCachedSquareDistance ;

      }

    inline
    void
    Prime(const XYZ& aVectorAB, double aLengthSquared)
      {
      for(int i = 0; i < UDF_COUNT; i++ )
        {
#if defined(PERFORMANCE_VECTOR_NBVM)
        mVirial[ i ].Zero();
#endif
        mEnergy[ i ] = 0.0;
        }

#if defined(PERFORMANCE_VECTOR_NBVM)
            mLastVirial.Zero() ;
#endif

#if defined(PERFORMANCE_RECORD_A_ONLY)
      mSiteRegA.mForce.Zero() ;
#else
      mSiteRegSet[0].mForce.Zero();
      mSiteRegSet[1].mForce.Zero();
#endif
      mVVirial.Zero() ;

      mCachedVectorAB = aVectorAB;

      mCachedSquareDistance = aLengthSquared ;
      mCachedReciprocalDistance = 1.0/sqrt(mCachedSquareDistance) ;
//      mCachedDistance = mCachedReciprocalDistance * mCachedSquareDistance ;

      }

// This one for optimisable situations where we precompute the lengths
    inline
    void
    Prime(const XYZ& aVectorAB, double aLengthSquared, double aReciprocalLength)
      {

      // Check that the passed-in reciprocal length is believable
      assert(is_almost_zero(aLengthSquared*aReciprocalLength*aReciprocalLength - 1.0)) ;

      for(int i = 0; i < UDF_COUNT; i++ )
        {
#if defined(PERFORMANCE_VECTOR_NBVM)
        mVirial[ i ].Zero();
#endif
        mEnergy[ i ] = 0.0;
        }

#if defined(PERFORMANCE_VECTOR_NBVM)
            mLastVirial.Zero() ;
#endif

#if defined(PERFORMANCE_RECORD_A_ONLY)
      mSiteRegA.mForce.Zero() ;
#else
      mSiteRegSet[0].mForce.Zero();
      mSiteRegSet[1].mForce.Zero();
#endif
      mVVirial.Zero() ;

      mCachedVectorAB = aVectorAB;

      mCachedSquareDistance = aLengthSquared ;
      mCachedReciprocalDistance = aReciprocalLength ;
//      mCachedDistance = mCachedReciprocalDistance * mCachedSquareDistance ;

      }
    // This one for optimisable situations where we precompute the lengths
    inline
    void
    Prime(double aLengthSquared, double aReciprocalLength)
      {

      // Check that the passed-in reciprocal length is believable
      assert(is_almost_zero(aLengthSquared*aReciprocalLength*aReciprocalLength - 1.0)) ;


      mCachedSquareDistance = aLengthSquared ;
      mCachedReciprocalDistance = aReciprocalLength ;
//      mCachedDistance = mCachedReciprocalDistance * mCachedSquareDistance ;

      }

    // This one for optimisable situations where we precompute the lengths
    // but where we are carrying around the length on its own, not the
    // square length or the reciprocal length. 
    // Only for 'switch' NBVMs !
    inline
    void
    PrimeLength(double aLength)
      {

      mCachedSquareDistance = aLength ;
      mCachedReciprocalDistance = 1.0 ;

      }


    inline
    void
    SetSimTick( unsigned int aSimTick )
      {
#if !defined(PERFORMANCE_NO_NBVM_SIMTICK)
      mSimTick = aSimTick;
#endif
      return;
      }

//  inline
//  void
//  SetPositionPtr( const int aRegisterIndex, XYZ * aPositRef )
//    {
//    mSiteRegSet[ aRegisterIndex ].mPosition = aPositRef;
//    return;
//    }

// UDF ordinals were interfering with optimisation, make sure noone uses them
    //#if !defined(PERFORMANCE_VECTOR_NBVM)
    inline
    void
    SetCurrentUdfOrdinal( int aOrdinal )
      {
#if !defined(PERFORMANCE_VECTOR_NBVM)
      mCurrentUdfOrdinal = aOrdinal;
#endif
      return;
      }

    inline
    const int
    GetCurrentUdfOrdinal()
      {
      return( mCurrentUdfOrdinal );
      }
// #else
//     inline
//     void
//     SetCurrentUdfOrdinal(int aOrdinal)
//       {
//       }
// #endif
//#ifdef MSDDEF_LEKNER
    inline
    double
    GetLeknerEpsilon()
      {
//    return mLeknerEpsilon;
      return  MSD_IF::GetLeknerEpsilon();
      }
//#endif
    inline
    const unsigned int
    GetSimTick()
      {
#if !defined(PERFORMANCE_NO_NBVM_SIMTICK)
      return( mSimTick );
#else
      return (unsigned int) -1 ;
#endif
      }

//  inline
//  const XYZ &
//  GetPosition( const int aUserSite ) const
//    {
//    const XYZ & Position = *( mSiteRegSet[ aUserSite ].mPosition );
//    return( Position );
//    }

    inline
    void
    ReportForce( const int aUserSite, const XYZ & aForce )
      {
#if defined(PERFORMANCE_RECORD_A_ONLY)
      if (aUserSite == SiteA)
      {
         mSiteRegA.mForce += aForce ;
      } /* endif */
#else
      mSiteRegSet[ aUserSite ].mForce = aForce;
#endif
      return;
      }

    inline
    const XYZ
    GetForce( const int aRegisterIndex )
      {
#if defined(PERFORMANCE_RECORD_A_ONLY)
      if (aRegisterIndex == SiteA)
      {
         return mSiteRegA.mForce ;
      }
      else
      {
         return ( - mSiteRegA.mForce ) ;
      } /* endif */
#else
      return( mSiteRegSet[ aRegisterIndex ].mForce );
#endif
      }

    inline
    void
    ReportEnergy( double aEnergy )
      {
         BegLogLine ( PKFXLOG_NBVM )
           << "NBVM::ReportEnergy(" << aEnergy << ")"
           << EndLogLine ;
      //NEED: this handled as an ASSIGNMENT is not indicated by the call 'ReportEnergy' it is a choice of the VM
     //  Use of mCurrentUdfOrdinal was inhibiting optimisaton

#if !defined(PERFORMANCE_VECTOR_NBVM)
     mEnergy[ mCurrentUdfOrdinal ] = aEnergy;
#else
     mLastEnergy = aEnergy ;
#endif
      }
   inline
    void
    ReportdEdr( double adEdr )
      {
         BegLogLine ( PKFXLOG_NBVM )
           << "NBVM::ReportdEdr(" << adEdr << ")"
           << EndLogLine ;
         mdEdr = adEdr ;
         double dVecABMagR = GetReciprocalDistance( SiteA, SiteB ) ;
         XYZ dVecAB = GetVector( SiteA, SiteB ) ;
         XYZ fab = dVecAB * dVecABMagR * (-adEdr);

         ReportForce( SiteA ,   fab ) ;
         ReportForce( SiteB , - fab ) ;
      }

    inline
    double
    GetdEdr( void )
      {
         return mdEdr ;
      }

    inline
    void
    ReportEnergy( double aEnergy , int aUdfOrdinal)
      {
         BegLogLine ( PKFXLOG_NBVM )
           << "NBVM::ReportEnergy(" << aEnergy
           << "," << aUdfOrdinal
           << ")"
           << EndLogLine ;
      // #if defined(PERFORMANCE_VECTOR_NBVM)
      mEnergy[ aUdfOrdinal ] = aEnergy;
      // #endif
      }

    inline
    const double
    GetEnergy(const int aUdfOrdinal )
      {
      return( mEnergy[ aUdfOrdinal ] );
      }
    inline
    const double
    GetEnergy(void)
      {
#if defined(PERFORMANCE_VECTOR_NBVM)
      return( mLastEnergy );
#else
      return mEnergy[ mCurrentUdfOrdinal ] ;
#endif
      }

    inline
    void
    ReportSwitch(double S, double dSdR)
    {
        SwitchS = S ;
        SwitchdSdR = dSdR ;
    }

    inline
    double
    GetSwitchS(void)
    {
       return SwitchS ;
    }

    inline
    double
    GetSwitchdSdR(void)
    {
        return SwitchdSdR ;
    }

    inline
    void
    ReportVVirial( const XYZ& aVVirial )
      {
      #if MSDDEF_DoPressureControl
      mVVirial += aVVirial;
      #endif
      }

    inline
    void
    ReportVirial( XYZ aVirial )
      {
      #if MSDDEF_DoPressureControl
#if !defined(PERFORMANCE_VECTOR_NBVM)
      mVirial[ mCurrentUdfOrdinal ] = aVirial;
#else
          mLastVirial = aVirial ;
#endif
      #endif
      }


    inline
    const XYZ
    GetVirial()
      {
      #if MSDDEF_DoPressureControl
#if !defined(PERFORMANCE_VECTOR_NBVM)
      return( mVirial[ mCurrentUdfOrdinal ] );
#else
      return mLastVirial ;
#endif
      #else
      XYZ zero;
      zero.Zero();
      return(zero);
      #endif
      }

    inline
    const XYZ
    GetVirial( int aUdfOrdinal )
      {
      #if MSDDEF_DoPressureControl
#if !defined(PERFORMANCE_VECTOR_NBVM)

      return( mVirial[ aUdfOrdinal ] );
#else
      return mLastVirial ;
#endif
      #else
      XYZ zero;
      zero.Zero();
      return(zero);
      #endif
      }

    inline
    const XYZ
    GetVVirial( int aUdfOrdinal )
      {
      #if MSDDEF_DoPressureControl
      return( mVVirial );
      #else
      return(0.0);
      #endif
      }

    inline
    const XYZ
    GetVector( int aUserSite0, int aUserSite1 )
      {
      BegLogLine(PKFXLOG_NBVM) << "NBVM::GetVector(): " << " RegA " << aUserSite0 << " RegB " << aUserSite1 << EndLogLine

      // Hopefully the compiler is inlining this, so there isn't actually a conditional here
      assert ( SiteA == aUserSite0 ) ;
      assert ( SiteB == aUserSite1 ) ;
      return mCachedVectorAB ;

//    if( aUserSite0 == SiteA )
//      {
//      return( mCachedVectorAB );
//      }
//    else
//      {
//      XYZ Position0 = GetPosition( aUserSite0 );
//      XYZ Position1 = GetPosition( aUserSite1 );
//
//      XYZ vector01 = Position0 - Position1;
//
//      return( vector01 );
//      }
      }

    inline
    const double
    GetDistance( int aUserSite0, int aUserSite1 )
      {
        BegLogLine(PKFXLOG_NBVM) << "NBVM::GetDistance(): " << " RegA " << aUserSite0 << " RegB " << aUserSite1 << EndLogLine;
//      return( mCachedDistance );
      return( mCachedSquareDistance * mCachedReciprocalDistance );
      }

    inline
    const double
    GetReciprocalDistance( int aUserSite0, int aUserSite1 )
      {
      // For now, just compute, but should do caching soon.

      BegLogLine(PKFXLOG_NBVM)
        << "NBVM::GetDistance(): "
        << " RegA " << aUserSite0
        << " RegB " << aUserSite1
        << EndLogLine

      return( mCachedReciprocalDistance );
      }

    inline
    const double
    GetDistanceSquared( int aUserSite0, int aUserSite1 )
      {
      BegLogLine(PKFXLOG_NBVM) << "NBVM::GetDistanceSquared(): " << " RegA " << aUserSite0 << " RegB " << aUserSite1 << EndLogLine
//      return( mCachedDistance * mCachedDistance );
        return mCachedSquareDistance ;
      }

    inline
    XYZ
    GetDynamicBoxDimensionVector()
       {
       BegLogLine(PKFXLOG_NBVM) << "NBVM::GetDynamicBoxVector(): " <<  EndLogLine ;
       return DynVarMgrIF.mDynamicBoxDimensionVector;
       }

    inline
    XYZ
    GetDynamicBoxInverseDimensionVector()
       {
       BegLogLine(PKFXLOG_NBVM)
          << "NBVM::GetDynamicBoxInverseVector(): "
          <<  EndLogLine ;
       return DynVarMgrIF.mDynamicBoxInverseDimensionVector;
       }

    inline
    void
    ApplySwitch(void)
    {
               BegLogLine(PKFXLOG_NBVM)
                          << "NBVM::ApplySwitch(): "
                          <<  EndLogLine ;

           XYZ force = GetForce( SiteA );
                   XYZ virial = GetVirial();
                   double energy = GetEnergy();
                   double S = GetSwitchS() ;
                   double pdSdR = GetSwitchdSdR() ;


                   XYZ dVecAB = GetVector( SiteA, SiteB );
                   double dVecABMagR = GetReciprocalDistance( SiteA, SiteB );
                   XYZ unitVector = dVecAB*dVecABMagR;

                   XYZ ForceSwitched = force*S - unitVector*energy*pdSdR;
                   double EnergySwitched = energy * S;
                   XYZ    VirialSwitched = virial * S;

                   ReportEnergy( EnergySwitched );
                   ReportVirial( VirialSwitched );

                   ReportForce( SiteA,   ForceSwitched );
                   ReportForce( SiteB, - ForceSwitched );

    }


 };



template<int UDF_COUNT, class streamclass>
static streamclass& operator<<(streamclass& os, NBVM<UDF_COUNT> &nbvm)
  {
  os
    << " Tick " << nbvm.GetSimTick()
    << " Vector " << nbvm.GetVector(0,1)
#if !defined(PERFORMANCE_VECTOR_NBVM)
    << " UDF# " << nbvm.GetCurrentUdfOrdinal()
//  << " PA "   << nbvm.GetPosition( 0 )
//  << " PB "   << nbvm.GetPosition( 1 )
    << " cE "   << nbvm.GetEnergy( nbvm.GetCurrentUdfOrdinal() )
#endif
    ;
  return(os);
  }



class NBVM_SWITCHEVALUATOR: public NBVM<1>
{
        public:
        inline
    void
    ApplySwitch(void)
    {
               BegLogLine(PKFXLOG_NBVM)
                          << "NBVM_SWITCHEVALUATOR::ApplySwitch() dummied: "
                          <<  EndLogLine ;
    }
} ;

template<int UDF_COUNT>
class NBVM_SWON : public NBVM<UDF_COUNT>
{
public:
   enum {
           kForceFullyOn = 1
        } ;
} ;

// 'scalar' nonbond UDF does not compute vector forces, and traps if you try GetForce
template<int UDF_COUNT>
class NBVM_SCALAR : public NBVM<UDF_COUNT>
{
public:
    enum {
        kSuppressVectorReporting = 1
    } ;
    double mAdditionalScalarVirial ;
    inline void ReportAdditionalScalarVirial ( double aScalarVirial )
    {
        mAdditionalScalarVirial = aScalarVirial ;
    }
    inline double GetAdditionalScalarVirial()
    {
        return mAdditionalScalarVirial ;
    }
    inline
    void
    ReportdEdr( double adEdr )
      {
         BegLogLine ( PKFXLOG_NBVM )
           << "NBVM_SCALAR::ReportdEdr(" << adEdr << ")"
           << EndLogLine ;
         mdEdr = adEdr ;
      }

    inline
    void
    ReportForce( const int aUserSite, const XYZ & aForce )
      {
      return;
      }

    inline
    const XYZ
    GetForce( const int aRegisterIndex )
      {
        assert(0) ;
        return( XYZ::ZERO_VALUE() );
      }

      inline void TearDown(void)
      {
        mdEdr = 0.0 ;
        mAdditionalScalarVirial = 0.0 ;
        mCachedSquareDistance = 0.0 ;
        mCachedReciprocalDistance = 0.0 ;
#if defined(PERFORMANCE_VECTOR_NBVM)
        mLastEnergy = 0.0 ;
#endif
      }


} ;




template<int UDF_COUNT>
class EXCLUSION_NBVM : public NBVM<UDF_COUNT>
  {
  public:
  double mExclusionScale ;
    inline
    void
    SetExclusionScale( double aScale )
    {
       mExclusionScale = aScale ;
      BegLogLine ( PKFXLOG_PRESSURE_CONTROL ) << "EXCLUSION_NBVM::SetExclusionScale aScale="
         << aScale
         << EndLogLine ;
    } ;

    inline
    void
    ReportForce( const int aUserSite, const XYZ & aForce )
      {
#if defined(PERFORMANCE_RECORD_A_ONLY)
      if (aUserSite == SiteA)
      {
         mSiteRegA.mForce += mExclusionScale * aForce ;
      } /* endif */
#else
      mSiteRegSet[ aUserSite ].mForce = mExclusionScale * aForce;
#endif
      BegLogLine ( PKFXLOG_PRESSURE_CONTROL ) << "EXCLUSION_NBVM::ReportForce aForce="
         << aForce
         << " mExclusionScale= "
         << mExclusionScale
         << EndLogLine ;
      return;
      }

    inline
    void
    ReportEnergy( double aEnergy )
      {
      //NEED: this handled as an ASSIGNMENT is not indicated by the call 'ReportEnergy' it is a choice of the VM
// Use of mCurrentUdfOrdinal was inhibiting optimisaton
#if !defined(PERFORMANCE_VECTOR_NBVM)
      mEnergy[ mCurrentUdfOrdinal ] = mExclusionScale * aEnergy;
#else
      mLastEnergy = mExclusionScale * aEnergy ;
#endif
      }

    inline
    void
    ReportEnergy( double aEnergy , int aUdfOrdinal)
      {
      // #if defined(PERFORMANCE_VECTOR_NBVM)
      mEnergy[ aUdfOrdinal ] = mExclusionScale * aEnergy;
      // #endif
      }

    inline
    void
    ReportVVirial( const XYZ& aVVirial )
      {
      #if MSDDEF_DoPressureControl
      mVVirial += mExclusionScale * aVVirial;
      BegLogLine ( PKFXLOG_PRESSURE_CONTROL ) << "EXCLUSION_NBVM::ReportVVirial aVVirial="
         << aVVirial
         << " mExclusionScale= "
         << mExclusionScale
         << EndLogLine ;
      #endif
      }

    inline
    void
    ReportVirial( XYZ aVirial )
      {
      #if MSDDEF_DoPressureControl
#if !defined(PERFORMANCE_VECTOR_NBVM)
      mVirial[ mCurrentUdfOrdinal ] += mExclusionScale * aVirial;
#else
          mLatVirial += mExclusionScale * aVirial ;
#endif
      BegLogLine ( PKFXLOG_PRESSURE_CONTROL )
          << "EXCLUSION_NBVM::ReportVirial aVirial="
          << aVirial
          << " mExclusionScale= "
          << mExclusionScale
          << EndLogLine ;
      #endif
      }


 };

#endif
