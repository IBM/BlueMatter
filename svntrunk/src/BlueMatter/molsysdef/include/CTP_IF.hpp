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
 #ifndef _CTP_IF_HPP_
#define _CTP_IF_HPP_

class CTP_IF
  {
  public:
  /**********************************************************************
   * Here lie the CTP field accessors
   **********************************************************************/
  static
  inline
  double
  GetGuardZoneWidth()
    {
    // This is meant to return the width of the cutoff guard zone.
#if CTPDEF_DoPressureControl
    return( 0.0 );
#else
    return( 0.5 );
#endif
    }

  static
  inline
  int
  GetSimpleRespaRatio()
    {
    return CTP::SimpleRespaRatio;
    }

  static
  inline
  int
  GetVelocityResampleRandomSeed()
    {
    return CTP::VelocityResampleRandomSeed;
    }

#if CTPDEF_ReplicaExchange
  static
  inline
  int
  GetSwapPeriodOTS()
    {
    return CTP::SwapPeriodOTS;
    }

  static
  inline
  int
  GetReplicaExchangeRandomSeed()
    {
    return CTP::ReplicaExchangeRandomSeed;
    }

  static
  inline
  int
  GetNumOfTemperatureAttempts()
    {
    return CTP::NumOfTemperatureAttempts;
    }
#endif

  static
  inline
  BoundingBox
  GetBoundingBox()
    {
    return CTP::BoundingBoxDef;
    }

  static
  inline
  int
  GetDoOuter()
    {
    return CTP::DoOuter;
    }

  static
  inline
  int*
  GetSimTicksAtRespaLevel()
    {
    return (int *) CTP::SimTicksAtRespaLevel;
    }

  static
  inline
  double
  GetNoseHooverMassFactor()
    {
    return CTP::NoseHooverMassFactor;  
    }

  static
  inline
  int
  GetCtpId()
    {
    return CTP::CtpId;  
    }

  static
  inline
  int
  GetImplId()
    {
    return CTP::ImplId;  
    }

  static
  inline
  int
  GetPltId()
    {
    return CTP::PltId;  
    }

  static
  inline
  void
  CheckMagicNumber()
    {
    int magicNumber = 0xFACEB0B6;
    if( CTP::MagicNumber != magicNumber )
      PLATFORM_ABORT("ERROR:: Magic numbers do not match");
    }

  static
  inline
  int
  GetNumberOfNHRespaLevels()
    {
    return CTP::NumberOfNoseHooverRespaLevels;  
    }

  static
  inline
  int
  GetNumberOfRespaLevels()
    {
    return CTP::NumberOfRespaLevels;
    }

  static
  inline
  double
  GetOuterTimeStepInPicoSeconds()
    {
    return CTP::OuterTimeStepInPicoSeconds;
    }

  static
  inline
  double
  GetInnerTimeStepInPicoSeconds()
    {
    return CTP::InnerTimeStepInPicoSeconds;
    }

  static
  inline
  double
  GetSwitchLowerCutoffForShell( const int aShellId )
    {
    return CTP::SwitchLowerCutoffForShell[ aShellId ];
    }

  static
  inline
  double
  GetSwitchDeltaForShell( const int aShellId )
    {
    return CTP::SwitchDeltaForShell[ aShellId ];
    }

  static
  inline
  int
  GetRespaLevelOfFirstShell()
  {
  return CTP::RespaLevelOfFirstShell;
  }

static
inline
int
GetNumberOfEnergyReportingUDFs()
  {
  return CTP::NumberOfEnergyReportingUDFs;
  }

static
inline
int
GetShakeMaxIterations()
  {
  return CTP::ShakeMaxIterations;
  }

static
inline
int
GetRattleMaxIterations()
  {
  return CTP::RattleMaxIterations;
  }

static
inline
double
GetShakeTolerance()
  {
  return CTP::ShakeTolerance;
  }

static
inline
double
GetRattleTolerance()
  {
  return CTP::RattleTolerance;
  }

static
inline
int
GetNumberOfInnerTimeSteps()
  {
      return CTP::NumberOfInnerTimeSteps;
  }

static
inline
int
GetNumberOfOuterTimeSteps()
  {
  return CTP::NumberOfOuterTimeSteps;
  }


static
inline
int
DoNonBondedAtRespaLevel( int aRespaLevel )
  {
  assert( aRespaLevel >= 0 && aRespaLevel < NUMBER_OF_RESPA_LEVELS );
  return CTP::DoNonBondedAtRespaLevel[ aRespaLevel ];
  }

static
inline
int
DoKSpaceAtRespaLevel( int aRespaLevel )
  {
  assert( aRespaLevel >= 0 && aRespaLevel < NUMBER_OF_RESPA_LEVELS );
  return CTP::DoKSpaceAtRespaLevel[ aRespaLevel ];
  }

static
inline
double
GetPressureControlTarget()
  {
  return CTP::PressureControlTarget;
  }

static
inline
double
GetPressureControlPistonInitialVelocity()
  {
  return CTP::PressureControlPistonInitialVelocity;
  }

static
inline
double
GetPressureControlPistonMass()
  {
  return CTP::PressureControlPistonMass;
  }

static
inline
int
GetNumberOfSimTicks()
  {
  return CTP::GetNumberOfSimTicks();
  }

static
inline
double
GetVelocityResampleTargetTemperature()
  {
  return CTP::VelocityResampleTargetTemperature;
  }

static
inline
int
GetSnapshotPeriodInOTS()
  {
  return CTP::SnapshotPeriodInOTS;
  }

static
inline
int
GetVelocityResamplePeriodInOTS()
  {
  return CTP::VelocityResamplePeriodInOTS;
  }

static
inline
int*
GetRespaLevelByUDFCode()
  {
  return (int *)CTP::RespaLevelByUDFCode;
  }

static
inline
double
GetEwaldAlpha()
  {
  return CTP::EwaldAlpha;
  }

static
inline
double
GetConvertedInnerTimeStep()
  {
  return CTP::ConvertedInnerTimeStep;
  }

static
inline
double
GetLeknerEpsilon()
  {
  return CTP::LeknerEpsilon;
  }

static
inline
double
GetExernalPressure()
  {
  return CTP::ExternalPressure;
  }

static
inline
int
GetEmitEnergyTimeStepModulo()
  {
  return CTP::EmitEnergyTimeStepModulo;
  }

static
inline
int
GetEwaldKmax()
  {
  return CTP::EwaldKmax;
  }

static
inline
int
GetP3MEchargeAssignment()
  {
  return CTP::P3MEchargeAssignment;
  }

static
inline
int
GetP3MEdiffOrder()
  {
  return CTP::P3MEdiffOrder;
  }

static
inline
int
GetSimTicksPerTimeStep()
  {
  return CTP::SimTicksPerTimeStep;
  }

static
inline
double*
GetIntegratorLevelConvertedTimeDelta()
  {
  return (double *) CTP::IntegratorLevelConvertedTimeDelta;
  }

/**********************************************************************/
};
#endif
