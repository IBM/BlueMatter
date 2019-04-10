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
 #ifndef FRAME_HPP
#define FRAME_HPP

#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
// AK : Added Include
#include <pk/platform.hpp>
#include <BlueMatter/SimulationState.hpp> // required for EnergyState etc.
#include <pk/fxlogger.hpp> // required for including UDF_Binding 
#include <BlueMatter/UDF_Binding.hpp>


class UDF
{
private:
    unsigned mNExpected;
    unsigned mNReceived;
    int mStatus;
    double mSum;
    enum {MAXPAYLOAD=0x7fffffff};  // MAXINT-1
    enum {PAYLOADTOLERANCE=20000}; // This should be good to 16K nodes
				   // ideally, maxpayload should be power of 2 if node count is also
    
public:    
    UDF()
    {
        mNExpected = 0;
        mNReceived = 0;
        mStatus = ArrivalStatus::UNKNOWN;
        mSum = 0;
    }
    inline void setNExpected(int n)
    {    
	if (n)
	    mNExpected = n;
	else
	    mNExpected = MAXPAYLOAD;
    }
    inline int isExpected() { return (mStatus & ArrivalStatus::EXPECTED); }
    inline int isComplete() { return (mStatus & ArrivalStatus::RECEIVED || mStatus & ArrivalStatus::NOTEXPECTED); }
    inline int isActive()   { return (mStatus & (ArrivalStatus::ARRIVING | ArrivalStatus::RECEIVED)); }
    inline double getSum()  { return mSum; }
    inline unsigned getCurrentPayload() { return mNReceived; }
    inline void checkReceived()
    {
        if (mNReceived >= mNExpected || (mNExpected - mNReceived) < PAYLOADTOLERANCE)
                mStatus |= ArrivalStatus::RECEIVED;
    }
	
    inline void addValue(double value, unsigned payload)
    {
        mSum += value;
        mNReceived += payload;
        mStatus |= ArrivalStatus::ARRIVING;
	checkReceived();
	// cerr << "UDF addValue status payload recd " << mStatus << " " << payload << " " << mNReceived << endl;
    }
};


class Frame
{
private:
    EnergyState *mEnergyState;
    DynamicVariablesState *mDynamicVariablesState;
    UDFState *mUDFState;
    
public:
    UDF mUDFs[UDF_Binding::UDF_CODE_COUNT];
    int mNUDFsExpected;
    int mUDFStatus;
    unsigned mOuterTimeStep;
    int mShortTimeStep;
    // unsigned mFullTimeStep;
    tEnergyInfo mEnergyInfo;
    double mEwaldSum;
    double mPistonMass;
    double mExternalPressure;
    int mComplete;
    int mNSites;
    int mSitesStatus;
    int mPressureStatus;
    int mRandomStateStatus;
    int mBoundingBoxStatus;
    int mConstraintStatus;
    int mTwoChainFragmentThermostatStatus;
    int mOneChainThermostatStatus;
    int mNumberOfTwoChainFragmentThermostatsExpected;
    int mRTPStatus;
    int mNumberDegreesOfFreedom;
    int mTimeStepFirm;
    int mUDFCountKnown;
    ED_InformationRTP mInformationRTP;
    ED_ControlPressure mControlPressure;
    ED_DynamicVariablesRandomState mDynamicVariablesRandomState;
    ED_DynamicVariablesBoundingBox mDynamicVariablesBoundingBox;
    ArrayGrowable<tSiteData> mSiteData;
    ED_DynamicVariablesOneChainThermostat mOneChainThermo;
    ArrayGrowable<ED_DynamicVariablesTwoChainFragmentThermostat> mTwoChainFragmentThermostatList;
    Frame(int Step = 0)
    {
        // cout << "New Frame " << Step << endl;
        mOuterTimeStep = Step;
	mShortTimeStep = Step & 0xff;
	mTimeStepFirm = 0;
        mSitesStatus = ArrivalStatus::UNKNOWN;
        mPressureStatus = ArrivalStatus::UNKNOWN;
        mRandomStateStatus = ArrivalStatus::UNKNOWN;
        mBoundingBoxStatus = ArrivalStatus::UNKNOWN;
        mTwoChainFragmentThermostatStatus = ArrivalStatus::UNKNOWN;
        mOneChainThermostatStatus = ArrivalStatus::UNKNOWN;
        mComplete = 0;
        mEnergyState = EnergyStateDefault::getInstance();
        mDynamicVariablesState = DynamicVariablesStateDefault::getInstance();
        mUDFState = UDFStateDefault::getInstance();
        memset(&mEnergyInfo, 0, sizeof(tEnergyInfo));
        mUDFStatus = ArrivalStatus::UNKNOWN;
        mRTPStatus = ArrivalStatus::UNKNOWN;
        mNUDFsExpected = 0;
        mNSites = 0;
        mConstraintStatus = ArrivalStatus::UNKNOWN;
        mNumberDegreesOfFreedom = 0;
	mUDFCountKnown = 0;
    }

    ~Frame()
    {
        mSiteData.free();
	mTwoChainFragmentThermostatList.free();
    }

    static unsigned FullStepFromOuter(unsigned CurrentFull, tOuterTimeStep NextOuter)
    {
        // Create new step by masking off lower bits and shoving in outer time step
        // unsigned NewFull = (CurrentFull & (((unsigned) -1) << 8*sizeof(tOuterTimeStep))) | NextOuter;
        // If the outer time step seems to have wrapped, bump it up by the full range
        // if (abs((int)CurrentFull - (int)NewFull) >= (1 << 8*sizeof(tOuterTimeStep)))
        //     NewFull += (1 << sizeof(tOuterTimeStep));
	// cerr << "Full From Outer: " << CurrentFull << " " << NextOuter << endl;
        unsigned NewFull = (CurrentFull & 0xffffff00) + (NextOuter & 0xff);
	if (NewFull + 128 < CurrentFull)
	    NewFull += 256;
	if (CurrentFull + 128 < NewFull)
	    NewFull -= 256;
	// cerr << "NewFull is " << NewFull << endl;
        return NewFull;
    }

    inline DynamicVariablesState *getDynamicVariablesState() { return mDynamicVariablesState; }
    inline EnergyState *getEnergyState() { return mEnergyState; }
    inline UDFState *getUDFState() { return mUDFState; }
    void newSite(ED_DynamicVariablesSite &sdv, SimulationParser *sp);
    void newMolecularSystemDefinition(SimulationParser *sp);
    void newRunTimeConfiguration(SimulationParser *sp);
    void setConstraints(SimulationParser *sp);
    void newUDFd1(ED_UDFd1 &udf, SimulationParser *sp);
    void newControlPressure(ED_ControlPressure &cp, SimulationParser *sp);
    void newDynamicVariablesRandomState(ED_DynamicVariablesRandomState &cp, SimulationParser *sp);
    void newDynamicVariablesBoundingBox(ED_DynamicVariablesBoundingBox &cp, SimulationParser *sp);
    void newTwoChainFragmentThermostat(ED_DynamicVariablesTwoChainFragmentThermostat &t, SimulationParser *sp);
    void newOneChainThermostat(ED_DynamicVariablesOneChainThermostat &t, SimulationParser *sp);
    void setRTP(ED_InformationRTP &cp, SimulationParser *sp);
    void setUDFCount(ED_InformationUDFCount &udfc, SimulationParser *sp);
    void setFrameContents(ED_InformationFrameContents &fc, SimulationParser *sp);
    void checkNewFrame(SimulationParser *sp);

    int numUDFsReceived()
    {
        if (mUDFStatus == ArrivalStatus::UNKNOWN)
            return 0;
        int fullcount = 0;
        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (mUDFs[i].isComplete())
                fullcount++;
        }
        return fullcount;
    }

    int allUDFsReceived()
    {
	if (!mUDFCountKnown)
	    return 0;
        if ((mUDFStatus & ArrivalStatus::NOTEXPECTED))
            return 1;
        if (!(mUDFStatus & ArrivalStatus::EXPECTED))
            return 0;
	int nrec = numUDFsReceived();
	// if (nrec > mNUDFsExpected)
	//    cerr << "WARNING - too many udf's received.  Expected " << mNUDFsExpected << " and got " << nrec << endl;
        return (nrec >= mNUDFsExpected);
    }

};

#endif
