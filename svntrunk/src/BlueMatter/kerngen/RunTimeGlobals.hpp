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
 #ifndef _RUNTIMEGLOBALS_HPP_
#define _RUNTIMEGLOBALS_HPP_

// This structure used to contain information
// calculated once at start up.

// It *might* be used to conatain dynamicaly upated stuff
// but that is probably a bad idea.

// If you add something that is dynamicaly updated,
// think about padding the structure to prevent
// false sharing on SMP machines.

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/math.hpp>

template<class RTG_PARAMS>
class RunTimeGlobals
  {
  public:

      XYZ           mBirthBoundingBoxDimensionVector;
      XYZ           mStartingBoundingBoxDimensionVector;
      // BoundingBox   mBirthBoundingBox;
      iXYZ          mBoxIndexOrder;  // order of simulation space indices, from largest dim to smallest

      int           mNumberOfSimTicks;
      int           mNumberOfOuterTimeSteps;
      int           mSnapshotPeriodInOTS;
      int           mEmitEnergyTimeStepModulo;

      double        mPressureControlPistonMass;
      double        mPressureControlPistonInitialVelocity;
      int           mVelocityResamplePeriodInOTS;
      int           mVelocityResampleRandomSeed;
      double        mVelocityResampleTargetTemperature;
      double        mPressureControlTarget;      
      
      unsigned      mStartingTimeStep;
      double        mGlobalResampleEnergyLoss;

     //NEED: To fill this array out based on RTP file inputs.
      SwitchFunctionRadii mSwitchFunctionRadii[ 1 ] ; // class defined in math.hpp
      SwitchFunctionRadii mSwitchOffRadius;
      SwitchFunctionRadii mSwitchOuterMostShell;
      
      // Replica Exchange parameters
      int           mSwapPeriodOTS;
      int           mReplicaExchangeRandomSeed;

      int           mNumOfTemperatureAttempts;

     #if MSDDEF_NOSE_HOOVER
      // Nose-Hoover parameters
      double mDKT[ RTG_PARAMS::NUM_OF_FRAGMENTS ]
                 [ RTG_PARAMS::NUM_OF_THERMOSTATS ] // 
                 [ RTG_PARAMS::NUM_OF_SITES_PER_THERMOSTAT ];
     #endif
 void Init() ;
 int GetNumberOfSimTicks() ;
};


#endif
