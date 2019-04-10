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
 #ifndef __DIRECT_MDVM__
#define __DIRECT_MDVM__

//NEED: The following MDVM which reaches directly into the DynVarMgr probably needs it's own file.

template< class DynVarMgrIF >
class DirectMDVM : public MDVM_BaseIF
  {
  public:
    int mSimTick;
    int mIntegratorLevel;
    int mCurrentFragmentId;
    int mCurrentFragmentFirstSiteIndex;
    int mCurrentFragmentSiteCount;

    int mNonsharedSelect;

    enum { mNonsharedMax = 10 };
    struct
      {
      double mEnergy;
      } mNonshared[ mNonsharedMax ];

    // should this be const?
    DynVarMgrIF &mDynVarMgrIF;

    DirectMDVM( DynVarMgrIF &aDynVarMgrIF ) : mDynVarMgrIF( aDynVarMgrIF )
      {
      }
    void
    Setup(int aSimTick, int aIntegratorLevel )
      {
      mNonsharedSelect               = -1;
      mSimTick    = aSimTick;
      mIntegratorLevel = aIntegratorLevel;
      mCurrentFragmentId             = -1;
      mCurrentFragmentFirstSiteIndex = -1;
      mCurrentFragmentSiteCount      = -1;
      for( int i = 0; i < mNonsharedMax; i++ )
        {
        mNonshared[ i ].mEnergy = 0.0;
        }
      }

    int
    GetNumberOfFragments()
      {
      #ifdef BM_SPATIAL_DECOMPOSITION
        return mDynVarMgrIF.mIntegratorStateManagerIF.GetNumberOfLocalFragments();
      #else
        return mDynVarMgrIF.GetIrreducibleFragmentCount();
      #endif
      }

    void
    SetNonsharedSelect( int aNonsharedSelect )
      {
          mNonsharedSelect = aNonsharedSelect;
      }

    inline
    void
    SetFragmentId( int aCurrentFragmentId )
      {
      #ifdef BM_SPATIAL_DECOMPOSITION
        FragId fragId = mDynVarMgrIF.mIntegratorStateManagerIF.GetNthFragmentId( aCurrentFragmentId );
        mCurrentFragmentId             = fragId;
         // We cache these values
        mCurrentFragmentFirstSiteIndex = MSD_IF::GetIrreducibleFragmentFirstSiteId( fragId );
        mCurrentFragmentSiteCount      = MSD_IF::GetIrreducibleFragmentMemberSiteCount( fragId );
      #else
        mCurrentFragmentId             = aCurrentFragmentId;
        // We cache these values
        mCurrentFragmentFirstSiteIndex = MSD_IF::GetIrreducibleFragmentFirstSiteId( aCurrentFragmentId );
        mCurrentFragmentSiteCount      = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aCurrentFragmentId );
      #endif
      }

    inline
    int
    GetFragmentSiteCount()
      {
      assert( mCurrentFragmentSiteCount != -1 );
      return( mCurrentFragmentSiteCount );
      }

    inline
    double
    GetHalfInverseMass( int aSiteIndex )
      {
      double rc = MSD_IF::GetHalfInverseMass( mCurrentFragmentFirstSiteIndex + aSiteIndex );
      return( rc );
      }

    inline
    double
    GetMass( int aSiteIndex )
      {
      double rc = MSD_IF::GetMass( mCurrentFragmentFirstSiteIndex + aSiteIndex );
      return( rc );
      }

    inline
    int
    GetSimTick()
      {
      return( mSimTick );
      }

    inline
    XYZ
    GetVirialSum()
    {
      XYZ virial;
      virial.Zero();
      for( int il = 0; il <= mIntegratorLevel; il++ )
        {
          virial += mDynVarMgrIF.GetVirial( il );
        }

      return virial;
    }

    inline
    XYZ
    GetVirialImpulse()
    {
      XYZ virialImpulse;
      virialImpulse.Zero();
      for( int il = 0; il <= mIntegratorLevel; il++ )
        {
          virialImpulse = virialImpulse +  mDynVarMgrIF.GetVirial( il )
            * MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ il ];
        }

      return virialImpulse;
    }


#ifndef NOT_TIME_REVERSIBLE
    inline
    double
    GetExpVolumeVelocityRatio()
      {
      return mDynVarMgrIF.mExpVelVolRatio;
      }
#endif


    inline
    XYZ
    GetVolumeInversePosition()
      {
      XYZ InversePosition;
      InversePosition.mX = InversePosition.mY = InversePosition.mZ =
        1 / mDynVarMgrIF.mVolume.mX;
      return( InversePosition );
      }

    inline
    void
    ZeroCenterOfMassForce()
    {
     for(int respaLevel=0; respaLevel <= mIntegratorLevel; respaLevel++ )
       {
        mDynVarMgrIF.SetCenterOfMassForce(mCurrentFragmentId, respaLevel, XYZ::ZERO_VALUE());
       }
    }

    inline
    void
    AddForceToCenterOfMassForce( int aSiteIndex )
      {
      for(int respaLevel=0; respaLevel <= mIntegratorLevel; respaLevel++ )
        {
        #ifdef BM_SPATIAL_DECOMPOSITION
          XYZ & f = mDynVarMgrIF.mIntegratorStateManagerIF.GetForce( mCurrentFragmentId, aSiteIndex  );
        #else
          XYZ f = mDynVarMgrIF.GetForce(  mCurrentFragmentFirstSiteIndex + aSiteIndex, mSimTick, respaLevel );
        #endif
        mDynVarMgrIF.AddCenterOfMassForce( mCurrentFragmentId, respaLevel, f );
        }
      }
      
    inline
    XYZ&
    GetForce( int aSiteIndex )
      {
      #ifdef BM_SPATIAL_DECOMPOSITION      
        XYZ & f = mDynVarMgrIF.mIntegratorStateManagerIF.GetForce(  mCurrentFragmentId, aSiteIndex );
      #else
        XYZ & f = mDynVarMgrIF.GetForce(  mCurrentFragmentFirstSiteIndex + aSiteIndex, mSimTick, mIntegratorLevel );
      #endif
      return( f );
      }

    inline
    XYZ&
    GetPosition( int aFragmentSiteIndex )
      {      
      #ifdef BM_SPATIAL_DECOMPOSITION
        XYZ & Position = mDynVarMgrIF.mIntegratorStateManagerIF.GetPosition( mCurrentFragmentId, aFragmentSiteIndex );
      #else
        XYZ & Position = mDynVarMgrIF.GetPosition( mCurrentFragmentFirstSiteIndex + aFragmentSiteIndex );
      #endif

      return( Position );
      }

    inline
    void
    ReportPosition( int aFragmentSiteIndex, XYZ aPosition )
      {
      #ifdef BM_SPATIAL_DECOMPOSITION
        mDynVarMgrIF.mIntegratorStateManagerIF.SetPosition( mCurrentFragmentId,
                                                            aFragmentSiteIndex,
                                                            aPosition );
      #else
        mDynVarMgrIF.SetPosition( mCurrentFragmentFirstSiteIndex + aFragmentSiteIndex,
                                  aPosition );
      #endif
      return;
      }

    inline
    void
    ReportCenterOfMassPosition( XYZ aCenterOfMassPosition )
      {
      mDynVarMgrIF.SetCenterOfMassPosition( mCurrentFragmentId, aCenterOfMassPosition );
      return;
      }

    inline
    XYZ&
    GetVelocity( int aSiteIndex )
      {
      assert( mCurrentFragmentFirstSiteIndex != -1 );

      #ifdef BM_SPATIAL_DECOMPOSITION
        XYZ & Velocity = mDynVarMgrIF.mIntegratorStateManagerIF.GetVelocity( mCurrentFragmentId, aSiteIndex );
      #else
        XYZ & Velocity = mDynVarMgrIF.GetVelocity( mCurrentFragmentFirstSiteIndex + aSiteIndex );
      #endif

      return( Velocity );
      }

    inline
    double
    GetMass()
    {
    assert( mCurrentFragmentFirstSiteIndex != -1 );
    double mass = MSD_IF::GetSiteInfo( mCurrentFragmentFirstSiteIndex + aSiteIndex ).mass;
    return( mass );
    }

    inline
    void
    ReportCenterOfMassVelocity( XYZ aCenterOfMassVelocity )
      {
      mDynVarMgrIF.SetCenterOfMassVelocity( mCurrentFragmentId, aCenterOfMassVelocity );
      return;
      }


    inline
    void
    ReportCenterOfMassKineticEnergy( double aCenterOfMassKineticEnergy )
      {
      mDynVarMgrIF.AddCenterOfMassKineticEnergy( aCenterOfMassKineticEnergy );
      return;
      }

    inline
    XYZ
    GetCenterOfMassVelocity()
      {
      XYZ rc;
      rc = mDynVarMgrIF.GetCenterOfMassVelocity( mCurrentFragmentId );
      return( rc );
      }

    inline
    void
    ReportVelocity( int aSiteIndex, XYZ& aVelocity )
      {
      #ifdef BM_SPATIAL_DECOMPOSITION
        mDynVarMgrIF.mIntegratorStateManagerIF.SetVelocity( mCurrentFragmentId,  
                                                            aSiteIndex,
                                                            aVelocity );
      #else
        mDynVarMgrIF.SetVelocity( mCurrentFragmentFirstSiteIndex + aSiteIndex,
                                  aVelocity );
      #endif
      return;
      }

    inline
    XYZ
    GetDisplacement( int aSiteIndex )
      {
      #ifdef BM_SPATIAL_DECOMPOSITION
        XYZ d =  mDynVarMgrIF.mIntegratorStateManagerIF.GetVelocity( mCurrentFragmentId, aSiteIndex )
            * MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ 0 ];
      #else
        XYZ d =  mDynVarMgrIF.GetVelocity(  mCurrentFragmentFirstSiteIndex +  aSiteIndex )
            * MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ 0 ];
      #endif
      return( d );
      }

    inline
    XYZ
    GetImpulse( int aSiteIndex )
      {
      XYZ f;
      f.Zero();
      for( int il = 0; il <= mIntegratorLevel; il++ )
        {
        #ifdef BM_SPATIAL_DECOMPOSITION
          f = f +   mDynVarMgrIF.mIntegratorStateManagerIF.GetForce(  mCurrentFragmentId, aSiteIndex )
              * MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ il ];
        #else       
          f = f +   mDynVarMgrIF.GetForce(  mCurrentFragmentFirstSiteIndex + aSiteIndex, mSimTick, il )
              * MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ il ];
        #endif
        }
      return( f );
      }

    inline
    XYZ
    GetCenterOfMassImpulse()
      {
      XYZ f;
      f.Zero();
      for( int il = 0; il <= mIntegratorLevel; il++ )
        {
        f = f + mDynVarMgrIF.GetCenterOfMassForce( mCurrentFragmentId, il )
            * MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ il ];
        }
      return( f );
      }



    inline
    void
    SetCenterOfMassKineticEnergy( double NewKE )
      {
      //  Note this adds to the global KE
      mDynVarMgrIF.mCenterOfMassKineticEnergy = NewKE;
      }


    inline
    double
    GetCenterOfMassMass()
      {
      double Mass;
      Mass = mDynVarMgrIF.GetCenterOfMassMass( mCurrentFragmentId );
      return( Mass );
      }

    inline
    double
    GetCenterOfMassHalfInverseMass()
      {
      double HalfInverseMass;
      HalfInverseMass = mDynVarMgrIF.GetCenterOfMassHalfInverseMass( mCurrentFragmentId );
      return( HalfInverseMass );
      }

    inline
    double
    GetCenterOfMassKineticEnergy()
      {
      return( mDynVarMgrIF.mCenterOfMassKineticEnergy );
      }

    inline
    void
    ZeroForce( int aSiteIndex )
      {
      #ifndef BM_SPATIAL_DECOMPOSITION
        for( int il = 0; il <= mIntegratorLevel; il++ )
          {
          mDynVarMgrIF.SetForce(  mCurrentFragmentFirstSiteIndex + aSiteIndex, mSimTick, il, XYZ::ZERO_VALUE() );
          }
      #endif
      return;
      }

    inline
    void
    ReportEnergy( double aEnergy )
      {
      assert( mNonsharedSelect >= 0 && mNonsharedSelect < mNonsharedMax );
      mNonshared[ mNonsharedSelect ].mEnergy += aEnergy;
      }

    inline
    double
    GetEnergy( int i )
      {
      return( mNonshared[ i ].mEnergy );
      }

    inline
    void
    ZeroEnergy( int i )
      {
          mNonshared[ i ].mEnergy = 0.0;

          SetCenterOfMassKineticEnergy( 0.0 );
      }


    inline
    double
    GetDeltaT()
      {
      /// NOTE: The zero means that the GetDeltaT is used EVERY SimTick and
      /// we therefore need to get the TimeDelta for the lowest respa level
      return ( MSD_IF::GetIntegratorLevelConvertedTimeDelta()[ 0 ] );
      }

    inline
    double
    GetNoseHooverMassFactor()
    {
        return MSD_IF::GetNoseHooverMassFactor();
    }

    inline
    int
    GetNumberOfNHRespaLevels()
    {
        return MSD_IF::GetNumberOfNHRespaLevels();
    }


     inline
     double
     GetProperTimeConstant()
     {
       /// JUST A HACK - for now
       // return 0.01 / SciConst::StdTimeConversion;
       return 100 * GetDeltaT();
     }

    inline
    double
    GetPressureRTP()
      {
      return( mDynVarMgrIF.mExternalPressure );
      }


    inline
    void
    ReportInternalPressure( const XYZ Pressure )
      {
      mDynVarMgrIF.mInternalPressure = Pressure;
      }


    inline
    XYZ
    GetVolumeImpulse()
      {
      return( mDynVarMgrIF.mVolumeImpulse );
      }

    inline
    void
    ReportVolumeImpulse( XYZ aImpulse )
      {
      mDynVarMgrIF.mVolumeImpulse = aImpulse;
      return;
      }


    inline
    XYZ
    GetVolumeVelocity()
      {
      return( mDynVarMgrIF.mVolumeVelocity );
      }


    inline
    void
    ReportVolumeVelocity( XYZ aVelocity )
      {
      mDynVarMgrIF.mVolumeVelocity = aVelocity;
      return;
      }


    inline
    double
    GetVolumeHalfInverseMass()
      {
      return( 0.5 / mDynVarMgrIF.mVolumeMass );
      }


    inline
    XYZ
    GetOldVolumeVelocity()
      {
      return( mDynVarMgrIF.mOldVolumeVelocity );
      }

    inline
    void
    ReportOldVolumeVelocity( XYZ aVelocity )
      {
      mDynVarMgrIF.mOldVolumeVelocity = aVelocity;
      return;
      }


    inline
    XYZ
    GetVolumePosition()
      {
      return( mDynVarMgrIF.mVolume );
      }

    inline
    void
    ReportVolumePosition( XYZ aPosition)
      {
      mDynVarMgrIF.mVolume = aPosition;
      }


    inline
    XYZ&
    GetDynamicBoxDimensionVector()
    {
      return mDynVarMgrIF.mDynamicBoxDimensionVector;
    }

    inline
    XYZ&
    GetBirthBoxDimensionVector()
    {
      return RTG.mBirthBoundingBoxDimensionVector;
    }

    inline
    void
    ReportBirthBoxAbsoluteRatio( XYZ ratio )
    {
      mDynVarMgrIF.mBirthBoxAbsoluteRatio = ratio;
    }

    inline
    void
    ReportDynamicBoxDimensionVector( XYZ aNewDBDV )
    {
      mDynVarMgrIF.mDynamicBoxDimensionVector = aNewDBDV;
    }

    inline
    void
    ReportDynamicBoxInverseDimensionVector( XYZ aNewDBDV )
    {
      mDynVarMgrIF.mDynamicBoxInverseDimensionVector = aNewDBDV;
    }

    inline
    void
    ReportVolumeRatio( XYZ aRatio )
      {
      mDynVarMgrIF.mVolumeRatio = aRatio;
      return;
      }

    inline
    void
    ReportCenterOfMassForce( XYZ aCenterOfMassForce )
      {
      mDynVarMgrIF.SetCenterOfMassForce( mCurrentFragmentId, aCenterOfMassForce );
      return;
      }

    inline
    XYZ
    GetCenterOfMassForce()
      {
      XYZ rc;
      rc = mDynVarMgrIF.GetCenterOfMassForce( mCurrentFragmentId, mIntegratorLevel );
      return( rc );
      }


    inline
    XYZ
    GetVolumeRatio()
      {
      return( mDynVarMgrIF.mVolumeRatio );
      }

    inline
    XYZ
    GetVolumeVelocityRatio()
      {
      return( mDynVarMgrIF.mVolumeVelocityRatio );
      }

    inline
    void
    ReportVolumeVelocityRatio( XYZ aRatio )
      {
      mDynVarMgrIF.mVolumeVelocityRatio = aRatio;
      return;
      }


    inline
    XYZ
    GetCenterOfMassPosition()
      {
      XYZ rc;
      rc = mDynVarMgrIF.GetCenterOfMassPosition( mCurrentFragmentId );
      return( rc );
      }

     inline
     void
     SetCenterOfMassForce( XYZ aCenterOfMassForce )
       {
       mDynVarMgrIF.SetCenterOfMassForce( mCurrentFragmentId, mIntegratorLevel, aCenterOfMassForce );
       return;
       }

    inline
    void
    ReportVirial( XYZ aVirial )
    {
      mDynVarMgrIF.AddVirial( mIntegratorLevel, aVirial );
      return;
    }

    /********************************************
     *  Nose-Hoover methods
     ********************************************/
#if MSDDEF_NOSE_HOOVER
    inline
    void
    ReportThermostatPosition( int aThermCount, int aThermSiteCount, double &aPos )
    {
      mDynVarMgrIF.SetThermostatPosition( mCurrentFragmentId, aThermCount, aThermSiteCount, aPos );
    }

    inline
    void
    ReportThermostatVelocity( int aThermCount, int aThermSiteCount, double &aVel )
    {
      mDynVarMgrIF.SetThermostatVelocity( mCurrentFragmentId, aThermCount, aThermSiteCount, aVel );
    }

    inline
    void
    ReportThermostatForce( int aThermCount, int aThermSiteCount, double &aForce )
      {
      mDynVarMgrIF.SetThermostatForce( mCurrentFragmentId, aThermCount, aThermSiteCount, aForce );
      }

    inline
    void
    ReportThermostatMass( int aThermCount, int aThermSiteCount, double &aMass )
      {
      mDynVarMgrIF.SetThermostatMass( mCurrentFragmentId, aThermCount, aThermSiteCount, aMass );
      }

    inline
    double
    GetThermostatPosition( int aThermCount, int aThermSiteCount )
      {
      return mDynVarMgrIF.GetThermostatPosition( mCurrentFragmentId, aThermCount, aThermSiteCount );
      }
      
    inline
    double
    GetThermostatVelocity( int aThermCount, int aThermSiteCount )
      {
      return mDynVarMgrIF.GetThermostatVelocity( mCurrentFragmentId, aThermCount, aThermSiteCount );
      }
      
    inline
    double
    GetThermostatForce( int aThermCount, int aThermSiteCount )
      {
      return mDynVarMgrIF.GetThermostatForce( mCurrentFragmentId, aThermCount, aThermSiteCount );
      }

    inline
    double
    GetThermostatMass( int aThermCount, int aThermSiteCount )
      {
      return mDynVarMgrIF.GetThermostatMass( mCurrentFragmentId, aThermCount, aThermSiteCount );
      }

    inline
    void
    ReportDkT0( int aThermSiteCount, double aDKT )
      {
      assert( mCurrentFragmentId < NUMBER_OF_FRAGMENTS );
      assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );
      
      RTG.mDKT[ mCurrentFragmentId ][ 0 ][ aThermSiteCount ] = aDKT;
      }
      
    inline
    void
    ReportDkT1( int aThermSiteCount, double aDKT )
      {
      assert( mCurrentFragmentId < NUMBER_OF_FRAGMENTS );
      assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );

      RTG.mDKT[ mCurrentFragmentId ][ 1 ][ aThermSiteCount ] = aDKT;
      }

    inline
    int
    GetFragmentConstraintCount()
      {
      return MSD_IF::GetFragmentConstraintCount( mCurrentFragmentId );
      }

    inline
    int
    GetNumberOfSitesPerThermostat()
    {
      return NUMBER_OF_SITES_PER_THERMOSTAT;
    }

    inline
    double
    GetVelocityResampleTargetTemperature()
    {
      return RTG.mVelocityResampleTargetTemperature;
    }

    inline
    void
    ReportFragmentKineticEnergy( double &aKineticEnergy )
    {
      mDynVarMgrIF.SetFragmentKineticEnergy( mCurrentFragmentId, aKineticEnergy );
    }

    inline
    double
    GetFragmentKineticEnergy()
    {
      return mDynVarMgrIF.GetFragmentKineticEnergy( mCurrentFragmentId );
    }

    inline
    double
    EvaluateFragmentKineticEnergy()
    {
      double fragKE = 0.0;

      for( int SiteIndex = 0; SiteIndex < GetFragmentSiteCount(); SiteIndex++ )
      {
        XYZ vel = GetVelocity( SiteIndex );
        fragKE += 0.5 * GetMass( SiteIndex ) * vel * vel;
      }

      return fragKE;
    }

    inline
    void
    ReportFragmentCOMKineticEnergy( double &aKineticEnergy )
    {
      mDynVarMgrIF.SetFragmentCOMKineticEnergy( mCurrentFragmentId, aKineticEnergy );
    }

    inline
    double
    GetFragmentCOMKineticEnergy()
    {
      return mDynVarMgrIF.GetFragmentCOMKineticEnergy( mCurrentFragmentId );
    }


    inline
    double
    EvaluateFragmentCOMKineticEnergy()
    {
      XYZ    COMVelocity = GetCenterOfMassVelocity();
      double COMMass     = GetCenterOfMassMass();

      double COMKE = 0.5 * COMMass * COMVelocity * COMVelocity;
      return COMKE;
    }

    inline
    double
    GetDkt( int aThermCount, int aThermSiteCount )
    {
      assert( mCurrentFragmentId < NUMBER_OF_FRAGMENTS );
      assert( aThermCount < NHC::NUMBER_OF_THERMOSTATS );
      assert( aThermSiteCount < NUMBER_OF_SITES_PER_THERMOSTAT  );

      return RTG.mDKT[ mCurrentFragmentId ][ aThermCount ][ aThermSiteCount ];
    }

    inline
    int
    GetNumberOfThermostats()
    {
        int degOfFreedom = 3 * GetFragmentSiteCount() - 3 - GetFragmentConstraintCount();
        if( degOfFreedom == 0 )
          return 1;
        else
          return 2;
    }

    inline
    int
    GetFragmentDimension()
    {
        return 3;
    }

   inline
   double
   GetGaussRandomNumber()
   {
     return NormalDeviateIF();
   }

#endif
  };


#endif
