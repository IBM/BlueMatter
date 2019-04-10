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
 #ifndef DATARECEIVERQUADRATICCONS_HPP
#define DATARECEIVERQUADRATICCONS_HPP

#include <assert.h>
#include <fcntl.h>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
#include <BlueMatter/DataReceiverSimpleLogger.hpp>
#include <BlueMatter/bootstrap.hpp>

#include <vector>

class DataReceiverQuadraticCons : public DataReceiverSimpleLogger
{
public:

    vector<double> mKEs;
    vector<double> mCQs;
    double mTimeStep;
    int mFirstOne;

    DataReceiverQuadraticCons(int DelayStats = 0, int OutputPeriod = 1, int BootStrap = 0, int AllSnapshots = 0)
    {
        mUDFsKnown = 1;  // simple hack to stop output of top line
        mPressureColumn = 0;
        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++)
            mUDFColumn[i] = 0;
        mLogCount = 0;
        mCQSum = 0;
        mCQSumSq = 0;
        mDelayStats = DelayStats;
  mOutputPeriod = OutputPeriod;
  mHeaderBuffer[0] = '\0';
  mValuesBuffer[0] = '\0';
  mBootStrap = BootStrap;
  mSaveAllSnapshots = AllSnapshots;
  strcpy(mSnapshotNameStem, "Snapshot");
  mFirstOne = 0;
    }

    virtual void sites(Frame *f)
    {
        DONEXT_1(sites, f);
    }

    virtual void logInfo(Frame *f)
    {
        doLogStats(f);
  mTimeStep = f->mInformationRTP.mOuterTimeStepInPicoSeconds;
  mCQs.push_back(f->mEnergyInfo.mEnergySums.mConservedQuantity);
  mKEs.push_back(f->mEnergyInfo.mEnergySumAccumulator.mKineticEnergy);

  #if 0
  printf("%d %d %lf %lf %lf\n",
  f->mOuterTimeStep,
  f->mInformationRTP.mNumberOfOuterTimeSteps,
  f->mInformationRTP.mOuterTimeStepInPicoSeconds,
  f->mEnergyInfo.mEnergySumAccumulator.mKineticEnergy,
  f->mEnergyInfo.mEnergySums.mConservedQuantity
  );
  #endif
        DONEXT_1(logInfo, f);
    }

    virtual void final(int status=1)
    {
  DONEXT_0(final);
    }
    
};

#endif
