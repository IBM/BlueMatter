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
 #ifndef SIMULATIONPARSER_H
#define SIMULATIONPARSER_H

// #include <assert.h>
#include <pk/fxlogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
// #include <BlueMatter/PacketStructures.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
#include <BlueMatter/MapSimple.hpp>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationState.hpp>
#include <BlueMatter/Frame.hpp>
#include <BlueMatter/TailFile.hpp>

#define DO_RECEIVERS_0(action)          \
{                                       \
    for (int i=0; i<mNReceivers; i++)   \
        mDR[i]->action();               \
}

#define DO_RECEIVERS_1(action, p)       \
{                                       \
    for (int i=0; i<mNReceivers; i++)   \
        mDR[i]->action(p);              \
}

class SimulationParser
{
    // map of pending frames, id'd by timestep
    char mFileName[256];
    Tail mTailFile;
    int mDone;
    int mNReceivers;
    int mDoSwap;
    int mEndPending;
    int mCompleteFramesPending;

    ControlState *mControlState;
    DynamicVariablesState *mDynamicVariablesState;
    EnergyState *mEnergyState;
    InformationState *mInformationState;
    SimulationState *mSimulationState;
    UDFState *mUDFState;
    RTPValuesState *mRTPValuesState;

    enum {MAX_RECEIVERS_ATTACHED = 8};

    DataReceiver *mDR[MAX_RECEIVERS_ATTACHED];
    double mStdUDFError[UDF_Binding::UDF_CODE_COUNT];
    double mUDFMean[UDF_Binding::UDF_CODE_COUNT];
    double mUDFValue1[UDF_Binding::UDF_CODE_COUNT];
    double mUDFValue2[UDF_Binding::UDF_CODE_COUNT];
    int mUDFLogCount[UDF_Binding::UDF_CODE_COUNT];
    int mNUDFsLogging;
    double mStdPositionError;
    double mStdVelocityError;
    double mEnergyFigTolerance;
    double mPositionFigTolerance;
    double mVelocityFigTolerance;
    double mEnergyFigError;
    double mPositionFigError;
    double mVelocityFigError;
    int mDiffState;
    int mPositionLogCount;
    int mVelocityLogCount;


public:
    MapSimple<Frame> mFrame;
    enum {REPORT_ABSOLUTE=0x1, REPORT_RELATIVE=0x2, REPORT_SIGFIGS=0x4, REPORT_HEADER=0x8, REPORT_VALUES=0x10,
          REPORT_SITES=0x20, REPORT_SUMMARY=0x1000000, REPORT_SUMSUMMARY=0x2000000, REPORT_NOTHING=0x80000000, REPORT_LOGGING=0x00ffffff};

    enum {SIGFIGMATCH=100};

    int mDebugMode;
    int mDebugStartTimeStep;
    int mTraceOn;
    int mSkewWarningNumber;
    int mQuiet;
    int mMSDKnown;
    int mRTCKnown;
    int mConstraintsKnown;
    int mRTPKnown;
    int mPressureStatus;
    int mRandomStateStatus;
    int mBoundingBoxStatus;
    unsigned mCurrentFullTimeStep;
    int mCurrentFullTimeStepKnown;
    int mUseTail;

    ED_InformationMolecularSystemDefinition mMolecularSystemDefinition;
    ED_InformationRunTimeConfiguration mRunTimeConfiguration;
    ED_InformationConstraints mConstraints;
    ED_InformationRTP mRTP;

    SimulationParser(char *fname, float timeout = 120, int wait = 1, int quiet=0, int debug=0, int debugstep=-1, int skewwarning=242, int seekahead=0);

    void setTolerances(double aEnergyFigTolerance=12.5, double aPositionFigTolerance=12.5, double aVelocityFigTolerance=12.5)
    {
        mEnergyFigTolerance = aEnergyFigTolerance;
        mPositionFigTolerance = aPositionFigTolerance;
        mVelocityFigTolerance = aVelocityFigTolerance;
    }

    // should be a friend
    static int packetsEqual(char *p1, char *p2);

    void getPacket(char *p=NULL);

    void getByte(char &c);

    void skip(int nskip);

    void pushPacket(char *p);

    void getFrame(Frame **ppF);

    // this should be a friend
    int framesEqual(Frame *pF1, Frame *pF2, int report);

    void reportSummary();

    int reportSumSummary();

    int checkSummaryEnergyTolerance();

    int checkSummaryPositionVelocityTolerance();

    void close()
    {
        if (mUseTail)
            mTailFile.Close();
    }

    ~SimulationParser()
    {
        close();
    }

    int OK()
    {
        if (mUseTail)
            return mTailFile.OK();
        return 1;
    }

    int done()
    {
        // Can't rely on frames being deleted since sites come out erratically
        if (mFrame.numItems() == 0 && mEndPending)
            mDone = 1;

        // check that receivers are happy.  This could be more active, but DR's don't have handle to sp.
        for (int i=0; i<mNReceivers; i++) {
            if (mDR[i]->done())
                mDone = 1;
        }

        // if (mEndPending)
        //     mDone = 1;
        return mDone;
    }

    int synchStream();

    void addReceiver(DataReceiver *dr)
    {
        assert(mNReceivers < MAX_RECEIVERS_ATTACHED);
        mDR[mNReceivers++] = dr;
    }

    void theEnd()
    {
        mEndPending = 1;
    }

    // FirstDone, AllDone, Never, Timeout
    void setTerminationMode() {}

    void checkFrameHealth()
    {
        int nmem = mFrame.numItems();
        if (mFrame.overstuffed()) {
            cerr << endl;
            cerr << "Too many frames in memory...  Aborting." << endl;
            exit(-1);
        }
        if (nmem > mSkewWarningNumber && !mQuiet) {
            cerr << endl;
            cerr << "Possible packet inconsistency - frames in memory = " << nmem << endl;
            if (mMSDKnown)
                cerr << "MSD Known - expecting " << mMolecularSystemDefinition.mSiteCount << " sites." << endl;
            else
                cerr << "MSD not known - don't know how many sites to expect yet.";
            cerr << "Details of frames in memory:" << endl;
            for (int i=0; i<nmem; i++) {
                Frame *f;
                int key;
                mFrame.getItemAndKey(i, f, key);
                if (f) {
                    cerr << "Index, ShortTimeStep, TimeStep, NSites, NUDFsExpected, NUDFsReceived, AllUDFsArrived: "
                         << i << " " << key << " " << f->mOuterTimeStep << " " << f->mSiteData.numSet() << " "
                         << f->mNUDFsExpected << " "
                         << f->numUDFsReceived() << " "
                         << (int)((f->mUDFStatus & ArrivalStatus::RECEIVED) != 0) << endl;
                } else {
                    cerr << "Couldn't get pointer to frame." << endl;
                }
            }
            cerr << endl;
        }
    }

    void setMolecularSystemDefinition(ED_InformationMolecularSystemDefinition &msd)
    {
        mMolecularSystemDefinition = msd;
        if (!mMSDKnown) {
            mMSDKnown = 1;
            for (int i=0; i<mFrame.numItems(); i++)
                mFrame.getOrdinal(i)->newMolecularSystemDefinition(this);
        }

    }

    void setRunTimeConfiguration(ED_InformationRunTimeConfiguration &rtc)
    {
        mRunTimeConfiguration = rtc;
        if (!mRTCKnown) {
            mRTCKnown = 1;
            for (int i=0; i<mFrame.numItems(); i++)
                mFrame.getOrdinal(i)->newRunTimeConfiguration(this);
        }

    }

    void setConstraints(ED_InformationConstraints &constraints)
    {
        mConstraints = constraints;
        if (!mConstraintsKnown) {
            mConstraintsKnown = 1;
            for (int i=0; i<mFrame.numItems(); i++)
                mFrame.getOrdinal(i)->setConstraints(this);
        }

    }



    void setRTP(ED_InformationRTP &rtp)
    {
        mRTP = rtp;
#if 0
        if (!mRTPKnown)
            for (int i=0; i<mFrame.numItems(); i++)
                mFrame[i].setRTP(this);
#endif
        mRTPKnown = 1;
        DO_RECEIVERS_1(informationRTP, rtp);
    }

    void setControlPressure(ED_ControlPressure &pressure)
    {
        DO_RECEIVERS_1(controlPressure, pressure);
    }

    void setControlTimeStamp(ED_ControlTimeStamp &stamp)
    {
        DO_RECEIVERS_1(controlTimeStamp, stamp);
    }

    void setControlReplicaExchangePeriodInOTS(ED_ControlReplicaExchangePeriodInOTS &period)
    {
        DO_RECEIVERS_1(controlReplicaExchangePeriodInOTS, period);
    }

    void setControlReplicaExchangeAttempt(ED_ControlReplicaExchangeAttempt &attempt)
    {
        DO_RECEIVERS_1(controlReplicaExchangeAttempt, attempt);
    }

    void setCurrentFullStep(unsigned FullStep)
    {
        mCurrentFullTimeStep = FullStep;
        // If first time to know of full timestep, update all frames in memory
        if (mDebugMode && mCurrentFullTimeStep >= mDebugStartTimeStep)
            mTraceOn = 1;
        if (!mCurrentFullTimeStepKnown) {
            mCurrentFullTimeStepKnown = 1;
            for (int i=0; i<mFrame.numItems(); i++) {
                int key;
                Frame *f = NULL;
                mFrame.getItemAndKey(i, f, key);
                if (mTraceOn)
                    cerr << "setCurrentFullStep, got frame: FullStep and key " << f << " " << mCurrentFullTimeStep << " " << key << endl;
                if (f) {
                    f->mOuterTimeStep = f->FullStepFromOuter(mCurrentFullTimeStep, key);
                    f->mTimeStepFirm = 1;
                    if (mTraceOn)
                        cerr << "---assigned full time step as " << f->mOuterTimeStep << endl;
                    f->checkNewFrame(this);
                }
            }
        }
        checkFrameHealth();
    }

    Frame *getCompleteFrame();

    void deleteCompleteFrame();

    Frame *getNextFrame();

    void deleteNextFrame();

    void init()
        DO_RECEIVERS_0(init);

    // This is where it all happens.
    // Loop for packets and call receivers at appropriate times
    // Should return reason for ending
    void run();

    void doPacket(Packet *p)
        DO_RECEIVERS_1(packet, p);

    void doFrame(Frame *f) {
        mCompleteFramesPending++;
	if (mTraceOn)
	    cerr << "about to dorec in doFrame with completeframespending incremented to " << mCompleteFramesPending << endl;
        DO_RECEIVERS_1(frame, f);
        // cout << "about to delete in doframe" << endl;
        // mFrame.deleteItem(f->mOuterTimeStep);
    }

    // void doEnergy(Frame *f) {
    //    DO_RECEIVERS_1(energy, f);
    // }

    void doSitePacket(ED_DynamicVariablesSite sdv) {
        DO_RECEIVERS_1(newSitePacket, sdv);
    }

    void doForcePacket(ED_DynamicVariablesForce sdv) {
        DO_RECEIVERS_1(newForcePacket, sdv);
    }

    void doTwoChainFragmentThermostatPacket(ED_DynamicVariablesTwoChainFragmentThermostat sdv) {
        DO_RECEIVERS_1(newTwoChainFragmentThermostatPacket, sdv);
    }

    void doOneChainThermostatPacket(ED_DynamicVariablesOneChainThermostat sdv) {
        DO_RECEIVERS_1(newOneChainThermostatPacket, sdv);
    }

    void doRandomState(ED_DynamicVariablesRandomState sdv) {
        DO_RECEIVERS_1(newDynamicVariablesRandomState, sdv);
    }

    void doUDFd1(ED_UDFd1 udf) {
        DO_RECEIVERS_1(newUDFd1, udf);
    }

    void doBoundingBox(ED_DynamicVariablesBoundingBox sdv) {
        DO_RECEIVERS_1(newDynamicVariablesBoundingBox, sdv);
    }

    void doSites(Frame *f)
        DO_RECEIVERS_1(sites, f);

    void doUDFs(Frame *f)
        DO_RECEIVERS_1(udfs, f);

    void velocityResampleTargetTemperature(ED_RTPValuesVelocityResampleTargetTemperature &vrtt)
        DO_RECEIVERS_1(velocityResampleTargetTemperature, vrtt);

    void numberOfOuterTimeSteps(ED_RTPValuesNumberOfOuterTimeSteps &vrtt)
        DO_RECEIVERS_1(numberOfOuterTimeSteps, vrtt);

    void snapshotPeriodInOTS(ED_RTPValuesSnapshotPeriodInOTS &vrtt)
        DO_RECEIVERS_1(snapshotPeriodInOTS, vrtt);

    void emitEnergyTimeStepModulo(ED_RTPValuesEmitEnergyTimeStepModulo &vrtt)
        DO_RECEIVERS_1(emitEnergyTimeStepModulo, vrtt);

    void velocityResampleRandomSeed(ED_RTPValuesVelocityResampleRandomSeed &vrtt)
        DO_RECEIVERS_1(velocityResampleRandomSeed, vrtt);

    void doLogInfo(Frame *f)
        DO_RECEIVERS_1(logInfo, f);

    void final(int status = 1)
        DO_RECEIVERS_1(final, status);

    void sync()
        DO_RECEIVERS_0(sync);

    void dumpPackets();

    void dumpTimes();

    // Do later - binary search for specified time step based on magic packet
    void seekTimeStep(int n) {}

    // Do later - seek forward to next frame based on magic packet
    void seekNextTimeStep() {}

    ControlState *getControlState()
    { return mControlState; }

    DynamicVariablesState *getDynamicVariablesState()
    { return mDynamicVariablesState; }

    EnergyState *getEnergyState()
    { return mEnergyState; }

    InformationState *getInformationState()
    { return mInformationState; }

    UDFState *getUDFState()
    { return mUDFState; }

    RTPValuesState *getRTPValuesState()
    { return mRTPValuesState; }

};


#endif
