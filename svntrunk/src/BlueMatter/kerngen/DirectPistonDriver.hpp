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
 #ifndef __DIRECT_PISTON_MDVM__
#define __DIRECT_PISTON_MDVM__

template< class DynVarMgrIF >
class DirectPistonMDVM : public MDVM_BaseIF
  {
    int mSimTick;
    int mIntegratorLevel;
    double mEnergy;

    DynVarMgrIF &mDynVarMgrIF;
public:
    DirectPistonMDVM( DynVarMgrIF &aDynVarMgrIF ) : mDynVarMgrIF( aDynVarMgrIF )
    {}

    inline
    void
    ReportThermostatPosition( int aThermCount, int aThermSiteCount, double &aPos )
    {
      mDynVarMgrIF.SetPistonThermostatPosition( aThermSiteCount, aPos );
    }

    inline
    void
    ReportThermostatVelocity( int aThermCount, int aThermSiteCount, double &aVel )
    {
      mDynVarMgrIF.SetPistonThermostatVelocity( aThermSiteCount, aVel );
    }

    inline
    void
    ReportThermostatForce( int aThermCount, int aThermSiteCount, double &aForce )
    {
      mDynVarMgrIF.SetPistonThermostatForce( aThermSiteCount, aForce );
    }

    inline
    void
    ReportThermostatMass( int aThermCount, int aThermSiteCount, double &aMass )
    {
      mDynVarMgrIF.SetPistonThermostatMass( aThermSiteCount, aMass );
    }

    inline
    double
    GetThermostatPosition( int aThermCount, int aThermSiteCount )
    {
      return mDynVarMgrIF.GetPistonThermostatPosition( aThermSiteCount );
    }

    inline
    double
    GetThermostatVelocity( int aThermCount, int aThermSiteCount )
    {
      return mDynVarMgrIF.GetPistonThermostatVelocity( aThermSiteCount );
    }

    inline
    double
    GetThermostatForce( int aThermCount, int aThermSiteCount )
    {
      return mDynVarMgrIF.GetPistonThermostatForce( aThermSiteCount );
    }

    inline
    double
    GetThermostatMass( int aThermCount, int aThermSiteCount )
    {
      return mDynVarMgrIF.GetPistonThermostatMass( aThermSiteCount );
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
      double PistonMass = mDynVarMgrIF.GetPistonMass();
// did not work - volume is zero - WHY???      double Volume = mDynVarMgrIF.mVolume.mX;
      double Volume = RTG.mBirthBoundingBoxDimensionVector.mX;
      double Kappa = 64e-6 / SciConst::Atm_IU; // compressibility of TIP3P water
      double TimeSqr = PistonMass * Volume * Kappa;
      double TimeConst = sqrt( TimeSqr );
      return 30 * TimeConst;
    }

    inline
    void
    ReportDkT0( int aThermSiteCount, double aDKT )
    {
      mDynVarMgrIF.SetPistonDKT( aThermSiteCount , aDKT);
    }

    inline
    void
    ReportDkT1( int aThermSiteCount, double aDKT )
    {
        return;
    }

    inline
    int
    GetFragmentConstraintCount()
    {
      return 0;
    }

    inline
    int
    GetFragmentSiteCount()
    {
        return 1;
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
      mDynVarMgrIF.SetPistonKineticEnergy( aKineticEnergy );
    }


    inline
    double
    GetFragmentKineticEnergy()
    {
      return mDynVarMgrIF.GetPistonKineticEnergy();
    }

    inline
    double
    EvaluateFragmentKineticEnergy()
    {
      double vel = GetVelocity( 0 ).mX;
      double fragKE = 0.5 * GetMass( 0 ) * vel * vel;

      return fragKE;
    }

    inline
    void
    ReportFragmentCOMKineticEnergy( double &aKineticEnergy )
    {
      mDynVarMgrIF.SetPistonKineticEnergy( aKineticEnergy );
    }

    inline
    double
    GetFragmentCOMKineticEnergy()
    {
      return mDynVarMgrIF.GetPistonKineticEnergy();
    }

    inline
    double
    EvaluateFragmentCOMKineticEnergy()
    {
      double    COMVelocity = GetCenterOfMassVelocity().mX;
      double COMMass     = GetCenterOfMassMass();

      double COMKE = 0.5 * COMMass * COMVelocity * COMVelocity;
      return COMKE;
    }

    inline
    XYZ
    GetCenterOfMassVelocity()
      {
      XYZ rc;
      rc = mDynVarMgrIF.GetVolumeVelocity();
      return( rc );
      }

    inline
    XYZ
    GetVelocity( int aSiteIndex )
      {
      XYZ rc;
      rc = mDynVarMgrIF.GetVolumeVelocity();
      return( rc );
      }

    inline
    void
    ReportVelocity( int aSiteIndex, XYZ aVelocity )
      {
      mDynVarMgrIF.mVolumeVelocity = aVelocity ;
      return;
      }

    inline
    double
    GetMass( int aSiteIndex )
      {
      double rc = mDynVarMgrIF.GetPistonMass();
      return( rc );
      }

    inline
    double
    GetCenterOfMassMass()
      {
      double rc = mDynVarMgrIF.GetPistonMass();
      return( rc );
      }

    inline
    double
    GetDkt( int aThermCount, int aThermSiteCount )
    {
      return mDynVarMgrIF.GetPistonDKT( aThermSiteCount );
    }

    inline
    int
    GetFragmentDimension()
    {
        return 1;
    }

    inline
    int
    GetNumberOfThermostats()
    {
        return 1;
    }

   inline
   void
   ReportEnergy( double aEnergy )
   {
       mEnergy = aEnergy;
       return;
   }

   inline
   double
   GetEnergy()
   {
       return mEnergy;
   }

   inline
   double
   GetGaussRandomNumber()
   {
     return NormalDeviateIF();
   }
};
#endif
