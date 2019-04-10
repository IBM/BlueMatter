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
 #ifndef _RUNTIMEGLOBALSINIT_HPP_
#define _RUNTIMEGLOBALSINIT_HPP_
#include <BlueMatter/RunTimeGlobals.hpp>

template<class RTG_PARAMS>
void RunTimeGlobals<RTG_PARAMS>::Init()
 {

 mBirthBoundingBoxDimensionVector        = ((BoundingBox) MSD_IF::GetBoundingBox()).GetDimensionVector();
 mStartingBoundingBoxDimensionVector        = ((BoundingBox) MSD_IF::GetBoundingBox()).GetDimensionVector();
 // mBirthBoundingBox                       = MSD_IF::GetBoundingBox();

 mNumberOfSimTicks                       = MSD_IF::GetNumberOfSimTicks();
 mNumberOfOuterTimeSteps                 = MSD_IF::GetNumberOfOuterTimeSteps();
 mSnapshotPeriodInOTS                    = MSD_IF::GetSnapshotPeriodInOTS();
 mEmitEnergyTimeStepModulo               = MSD_IF::GetEmitEnergyTimeStepModulo();
 mPressureControlPistonMass              = MSD_IF::GetPressureControlPistonMass();
 mPressureControlPistonInitialVelocity   = MSD_IF::GetPressureControlPistonInitialVelocity();
 mPressureControlTarget                  = MSD_IF::GetPressureControlTarget();      
 mVelocityResamplePeriodInOTS            = MSD_IF::GetVelocityResamplePeriodInOTS();
 mVelocityResampleRandomSeed             = MSD_IF::GetVelocityResampleRandomSeed();
 mVelocityResampleTargetTemperature      = MSD_IF::GetVelocityResampleTargetTemperature();

#if MSDDEF_ReplicaExchange
 mReplicaExchangeRandomSeed              = MSD_IF::GetReplicaExchangeRandomSeed();
 mSwapPeriodOTS                          = MSD_IF::GetSwapPeriodOTS();
 mNumOfTemperatureAttempts               = MSD_IF::GetNumOfTemperatureAttempts();
#endif

 mStartingTimeStep                       = 0u;
 mGlobalResampleEnergyLoss               = 0.0;
 
 int numShells = 1;

 for( int i=0; i < numShells; i++) 
   {
   mSwitchFunctionRadii[ i ].SetRadii( MSD_IF::GetSwitchLowerCutoffForShell( i ),
                                       MSD_IF::GetSwitchLowerCutoffForShell( i ) + MSD_IF::GetSwitchDeltaForShell( i ) );
   }
 
 mSwitchOffRadius.SetRadii(1E100, 1E100 + 1.0);
 mSwitchOuterMostShell.SetRadii( MSD_IF::GetSwitchLowerCutoffForShell( numShells - 1 ),
                                 MSD_IF::GetSwitchLowerCutoffForShell( numShells - 1 ) 
                                 + MSD_IF::GetSwitchDeltaForShell( numShells - 1 ) );
 }


template<class RTG_PARAMS>
int 
RunTimeGlobals<RTG_PARAMS>::GetNumberOfSimTicks() 
  {    
  int rc = MSD_IF::GetSimTicksPerTimeStep() * mNumberOfOuterTimeSteps;
  return ( rc );
  }
#endif
