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
 * Module:          WATERNBVM
 *
 * Purpose:         NonBonded Virtual Machine interface.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010820 TJCW Specialised from general NBVM.
 *
 ***************************************************************************/

// This file contains active message infrastructure and nonbonded engine
// thread code.

// Currently, this code does not have an initialization phase.  This leads to
// some extra data in operand packets and also makes it impossible to
// dynamically allocate some of the structures.  These are allocated based
// on a compilte time limit.  THIS CAN ALL BE CHANGED AS WE GO ALONG.

#ifndef _PLIMPWATERNONBONDEDVIRTUALMACHINE_HPP_
#define _PLIMPWATERNONBONDEDVIRTUALMACHINE_HPP_

#ifndef PKFXLOG_WATERNBVM
#define PKFXLOG_WATERNBVM 0
#endif

#include <BlueMatter/XYZ.hpp>
#include <pk/fxlogger.hpp>


//*****************************************************************************

static const XYZ XYZ_ZERO = { 0.0, 0.0, 0.0 } ;

class WATERNBVM : public MDVM_BaseIF
  {
  public:
    enum { WATERNBVM_SITE_REG_COUNT = 2
          ,WATERNBVM_PAIR_REG_COUNT = 1
    };
    enum { SiteA = 0, SiteB = 1, SiteC = -1, SiteD = -1 };

  private:
    // This class contains the state managed per site
    // (Try it by only managing the force on each site)
    class SiteReg
      {
      public:
//      XYZ    * mPosition;
        XYZ      mForce;
//      SiteReg()
//        {
//        mPosition = NULL;
//        mForce.Zero();
//        }
      };

    // This class contains the state managed per pairwise interaction
    // (We're going to do charge-charge for every atom in water, which implies reciprocal square roots,
    //  so do this rather than the reciprocal which would be sufficient for L-J)
    class PairReg
    {
    public:
      XYZ VectorAB ;
      double SquareDistance ;
      double RecipSquareDistance ;
      double RecipDistance ;

    } ;


    SiteReg mSiteRegSet [ WATERNBVM_SITE_REG_COUNT ];
    PairReg mPairReg ;

    // Last energy reported by a MDVM UDF
    double mEnergy;

    int mSimTick;

  public:

    WATERNBVM(const XYZ& aVectorAB, double aSquareDistance, double aRecipSquareDistance, double aRecipDistance )
      {
         mPairReg.VectorAB = aVectorAB ;
         mPairReg.SquareDistance = aSquareDistance ;
         mPairReg.RecipSquareDistance = aRecipSquareDistance ;
         mPairReg.RecipDistance = aRecipDistance ;
         mEnergy = 0.0;
      }

    inline
    const unsigned int
    GetSimTick()
      {
      return( mSimTick );
      }

    inline
    void
    SetSimTick( unsigned int aSimTick )
      {
      mSimTick = aSimTick;
      return;
      }

    inline
    const XYZ &
    GetPosition( const int aUserSite ) const
      {
         assert(0) ; // Shouldn't do this for nonbond water ...
         return( XYZ_ZERO );
      }

    inline
    void
    SetForce( const int aUserSite, XYZ aForce )
      {
      mSiteRegSet[ aUserSite ].mForce = aForce;
      return;
      }

    inline
    XYZ &
    GetForce( const int aRegisterIndex )
      {
      return( mSiteRegSet[ aRegisterIndex ].mForce );
      }

    inline
    void
    SetPositionPtr( const int aRegisterIndex, XYZ * aPositRef )
      {
         assert(0) ;
      return;
      }

    inline
    void
    SetEnergy( double aEnergy )
      {
      mEnergy = aEnergy;
      }

    inline
    double
    GetEnergy()
      {
      return( mEnergy );
      }


    inline
    XYZ
    GetVector( int aUserSite0, int aUserSite1 )
      {
      // For now, just compute, but should do caching soon.

      BegLogLine(PKFXLOG_WATERNBVM)
        << "WATERNBVM::GetVector(): "
        << " RegA " << aUserSite0
        << " RegB " << aUserSite1
        << EndLogLine

         assert(aUserSite0 == SiteA ) ;
         assert(aUserSite1 == SiteB ) ;
         return mPairReg.VectorAB ;
      }

    inline
    double
    GetDistance( int aUserSite0, int aUserSite1 )
      {
         assert(0) ;
         return 0.0 ;
      // For now, just compute, but should do caching soon.

      BegLogLine(PKFXLOG_WATERNBVM)
        << "WATERNBVM::GetDistance(): "
        << " RegA " << aUserSite0
        << " RegB " << aUserSite1
        << EndLogLine

         assert(aUserSite0 == SiteA ) ;
         assert(aUserSite1 == SiteB ) ;
         return mPairReg.SquareDistance * mPairReg.RecipDistance ;
      }

    inline
    double
    GetDistanceSquared( int aUserSite0, int aUserSite1 )
      {
      // For now, just compute, but should do caching soon.

      BegLogLine(PKFXLOG_WATERNBVM)
        << "WATERNBVM::GetDistanceSquared(): "
        << " RegA " << aUserSite0
        << " RegB " << aUserSite1
        << EndLogLine

         assert(aUserSite0 == SiteA ) ;
         assert(aUserSite1 == SiteB ) ;
         return mPairReg.SquareDistance ;
      }

    inline
    double
    GetRecipDistanceSquared( int aUserSite0, int aUserSite1 )
      {
      // For now, just compute, but should do caching soon.

      BegLogLine(PKFXLOG_WATERNBVM)
        << "WATERNBVM::GetDistanceSquared(): "
        << " RegA " << aUserSite0
        << " RegB " << aUserSite1
        << EndLogLine

         assert(aUserSite0 == SiteA ) ;
         assert(aUserSite1 == SiteB ) ;
         return mPairReg.RecipSquareDistance ;
      }

    inline
    int
    GetSiteId(int aUserSite0 )
    {
      assert(0);
      return 0;
    }

    inline const
    XYZ &
    GetVelocity(int aUserSite0 )
    {
      assert(0);
      return XYZ_ZERO;
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

   inline
   void
   SetVelocity( int reg, XYZ velocity )
    {
      assert(0);
    }
};

#endif
