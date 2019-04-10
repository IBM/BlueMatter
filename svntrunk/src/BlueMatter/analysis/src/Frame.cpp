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
 #include <iostream>
using namespace std ;
#include <memory.h>

#include <BlueMatter/SimulationState.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/math.hpp>

void Frame::checkNewFrame(SimulationParser *sp)
{
    if (sp->mTraceOn) {
        if (!sp->mCurrentFullTimeStepKnown)
	    cerr << "checkNewFram Timestep not known mOuterTimeStep mCurrentFullTimeStep " << " " << mOuterTimeStep << " " << sp->mCurrentFullTimeStep << endl;
	else
	    cerr << "checkNewFram Timestep known Firm, mOuterTimeStep, mCurrentFullTimeStep " << " " << mTimeStepFirm << " " << mOuterTimeStep << " " << sp->mCurrentFullTimeStep << endl;
    }
    if (!sp->mCurrentFullTimeStepKnown)
        return;
    if (!mTimeStepFirm) {
        if (sp->mTraceOn)
            cerr << "---CheckNewFrame SettingFirmTimeStep: currentfull and ots input: " << sp->mCurrentFullTimeStep << " " << mOuterTimeStep << endl;
        mOuterTimeStep = FullStepFromOuter(sp->mCurrentFullTimeStep, mOuterTimeStep);
        if (sp->mTraceOn)
            cerr << "---Assigned Firm outertimestep: " << mOuterTimeStep << endl;
        mTimeStepFirm = 1;
    }
    if (sp->mTraceOn) {
        cerr << "---checkNewFram mOuterTimeStep mCurrentFullTimeStep " << " " << mOuterTimeStep << " " << sp->mCurrentFullTimeStep << endl;
	cerr << "---nudfs received, allreceived, udfstatus " << numUDFsReceived() << " " << allUDFsReceived() << " " << mUDFStatus << endl;
        cerr << "---ts msdknown constknown rtpstat presstat twochnstat onechanstat numtwochainsexp twochainssize twochainsgaps " << mOuterTimeStep << " " << sp->mMSDKnown << " " << sp->mConstraintsKnown << " " <<
                 mRTPStatus << " " << mPressureStatus << " " << mTwoChainFragmentThermostatStatus << " " << mOneChainThermostatStatus <<  " " << mNumberOfTwoChainFragmentThermostatsExpected << " " << mTwoChainFragmentThermostatList.getSize() << " " << mTwoChainFragmentThermostatList.hasGaps() << endl;
    }

    // short circuit status evaluation if this isn't the leading frame
    if (!sp->mFrame.isHead(mShortTimeStep)) {
	if (sp->mTraceOn)
	    cerr << "---Frame is not head" << endl;
	return;
    }

    // Try to output log info, though may not be ready to output snapshot
    // Must know MSD since nsites is required for temperature calculation
    // Also need piston state
    if (sp->mMSDKnown && sp->mConstraintsKnown &&
	mUDFCountKnown &&
        !(mUDFStatus & ArrivalStatus::NOTEXPECTED || mUDFStatus & ArrivalStatus::SENT) && allUDFsReceived() &&
        (mRTPStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED)) &&
        (mPressureStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED)) )
    {
        // This uses sp->RTP for values constant during run
        mNSites = sp->mMolecularSystemDefinition.mSiteCount;
        mNumberDegreesOfFreedom = sp->mConstraints.mDegreesOfFreedomCount;
        mPistonMass = sp->mRTP.mPressureControlPistonMass;
        mExternalPressure = sp->mRTP.mPressureControlTarget;

        // Now have everything needed to log energy info, without regard for presence of sites
	// only log if udfs were really expected in this frame
	if (mUDFStatus & ArrivalStatus::EXPECTED)
	    sp->doLogInfo(this);
        mUDFStatus |= ArrivalStatus::RECEIVED;
        mUDFStatus |= ArrivalStatus::SENT;
    }

    // Set status of thermostat arrival state
    if (mNumberOfTwoChainFragmentThermostatsExpected > 0 &&
        mNumberOfTwoChainFragmentThermostatsExpected <= mTwoChainFragmentThermostatList.getSize() &&
	!mTwoChainFragmentThermostatList.hasGaps())
        mTwoChainFragmentThermostatStatus |= ArrivalStatus::RECEIVED;

    // hardcode here the slaving of thermos to sites
    if (mSitesStatus & ArrivalStatus::NOTEXPECTED || !mNumberOfTwoChainFragmentThermostatsExpected)
	mTwoChainFragmentThermostatStatus |= ArrivalStatus::NOTEXPECTED;

    if (sp->mTraceOn) {
        cerr << "---SECONDPHASE checkNewFram mOuterTimeStep mCurrentFullTimeStep " << " " << mOuterTimeStep << " " << sp->mCurrentFullTimeStep << endl;
	cerr << "---nudfs received, allreceived, udfstatus " << numUDFsReceived() << " " << allUDFsReceived() << " " << mUDFStatus << endl;
        cerr << "---ts msdknown constknown rtpstat presstat twochnstat onechanstat " << mOuterTimeStep << " " << sp->mMSDKnown << " " << sp->mConstraintsKnown << " " <<
                 mRTPStatus << " " << mPressureStatus << " " << mTwoChainFragmentThermostatStatus << " " << mOneChainThermostatStatus << endl;
	cerr << "---sitestatus " << mSitesStatus << endl;
    }

    // Now output full restart state when we know we have everything expected
    // Snapshot now requires udf info for energy loss, so gotta have all udfs for restart
    if ((mSitesStatus & ArrivalStatus::RECEIVED) && !(mSitesStatus & ArrivalStatus::SENT) &&
        (mRTPStatus      & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))       &&
        (mPressureStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))       &&
	(mRandomStateStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))    &&
	(mBoundingBoxStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))    &&
	mUDFCountKnown &&
        (mUDFStatus      & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))       &&
        (mTwoChainFragmentThermostatStatus      & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))
       )
    {
        // hardcode here the slaving of one chain thermos to pressure and two chainthermos
	// wait if all are here except onechainthermo
        if (mPressureStatus & ArrivalStatus::RECEIVED &&
            mTwoChainFragmentThermostatStatus & ArrivalStatus::RECEIVED &&
            !(mOneChainThermostatStatus & ArrivalStatus::RECEIVED)) {
            if (sp->mTraceOn)
		cerr << "---Pressure and Thermo are blocking frame completion" << endl;
        } else {
           // Have sites for this frame, so notify
	   if (sp->mTraceOn)
	       cerr << "---Sending sites" << endl; 
           sp->doSites(this);
           mSitesStatus |= ArrivalStatus::SENT;
        }
    }

    if ((mSitesStatus    & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::SENT))           &&
        (mRTPStatus      & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))       &&
        (mPressureStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))       &&
	(mRandomStateStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))    &&
	(mBoundingBoxStatus & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))    &&
	mUDFCountKnown &&
        (mUDFStatus      & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::SENT))       &&
        (mTwoChainFragmentThermostatStatus      & (ArrivalStatus::NOTEXPECTED | ArrivalStatus::RECEIVED))
       )
    {
           sp->checkFrameHealth();
           // Have energy info and site info determined, so notify
	   if (mComplete)
	       return; 
           mComplete = 1;
           if (sp->mTraceOn) {
               cerr << "---setting frame complete " << endl << flush;
               cerr << "---Frame calling doFrame, frames in mem:  " << sp->mFrame.numItems() << endl;
	    }
           sp->doFrame(this);
    }
}

void Frame::newSite(ED_DynamicVariablesSite &sdv, SimulationParser *sp)
{
    tSiteData sd;
    sd.mPosition = sdv.mPosition;
    sd.mVelocity = sdv.mVelocity;
    mSiteData.set(sdv.mSiteId, sd);
    if (mSitesStatus & ArrivalStatus::NOTEXPECTED)
        cerr << "Sites arriving at frame " << mOuterTimeStep << " when NOT EXPECTED.  Results corrupt." << endl;
    mSitesStatus |= ArrivalStatus::ARRIVING;

    if (sp->mTraceOn)
        cerr << "Site, size, msdsize, hasgaps, ots " << sdv.mSiteId << " " <<  mSiteData.getSize() << " " << sp->mMolecularSystemDefinition.mSiteCount << " " << mSiteData.hasGaps() << " " << mOuterTimeStep << endl;
    if (sp->mMSDKnown && !mSiteData.hasGaps() && (sp->mMolecularSystemDefinition.mSiteCount <= mSiteData.getSize()))
    {
        // mOuterTimeStep = sdv.mOuterTimeStep;
        mSitesStatus |= ArrivalStatus::RECEIVED;
        if (sp->mTraceOn)
            cerr << "---Sites all received, checking new frame since no gaps and NSites= " << mSiteData.getSize() << endl;
        checkNewFrame(sp);
    }
}

void Frame::setUDFCount(ED_InformationUDFCount &udfc, SimulationParser *sp)
{
    mUDFCountKnown = 1;
    if (mUDFStatus & ArrivalStatus::NOTEXPECTED)
	mNUDFsExpected = 0;
    else {
	mNUDFsExpected = udfc.mCount;
    }
    if (sp->mTraceOn) {
        cerr << "Frame mOuterTimeStep, SetUDFCount " << mOuterTimeStep << " " << udfc.mCount << endl;
        cerr << "---UDFCount KNOWN" << endl;
	cerr << "---checking new frame..." << endl;
    }
    checkNewFrame(sp);
}

void Frame::setFrameContents(ED_InformationFrameContents &fc, SimulationParser *sp)
{
    if (FrameContents::Sites & fc.mContents) {
        mSitesStatus |= ArrivalStatus::EXPECTED;
        // For now, thermostats are tied to sites
        mTwoChainFragmentThermostatStatus |= ArrivalStatus::EXPECTED;
    } else {
        mSitesStatus |= ArrivalStatus::NOTEXPECTED;
        mTwoChainFragmentThermostatStatus |= ArrivalStatus::NOTEXPECTED;
    }

    if (FrameContents::Pressure & fc.mContents) {
        mPressureStatus |= ArrivalStatus::EXPECTED;
        sp->mPressureStatus |= ArrivalStatus::EXPECTED;
    } else {
        mPressureStatus |= ArrivalStatus::NOTEXPECTED;
        sp->mPressureStatus |= ArrivalStatus::NOTEXPECTED;
    }

    if (FrameContents::RandomState & fc.mContents) {
        mRandomStateStatus |= ArrivalStatus::EXPECTED;
        sp->mRandomStateStatus |= ArrivalStatus::EXPECTED;
    } else {
        mRandomStateStatus |= ArrivalStatus::NOTEXPECTED;
        sp->mRandomStateStatus |= ArrivalStatus::NOTEXPECTED;
    }

    if (FrameContents::BirthBox & fc.mContents) {
        mBoundingBoxStatus |= ArrivalStatus::EXPECTED;
        sp->mBoundingBoxStatus |= ArrivalStatus::EXPECTED;
    } else {
        mBoundingBoxStatus |= ArrivalStatus::NOTEXPECTED;
        sp->mBoundingBoxStatus |= ArrivalStatus::NOTEXPECTED;
    }

    if (FrameContents::Constraints & fc.mContents)
        mConstraintStatus |= ArrivalStatus::EXPECTED;
    else
        mConstraintStatus |= ArrivalStatus::NOTEXPECTED;

    if (FrameContents::UDFs & fc.mContents)
        mUDFStatus |= ArrivalStatus::EXPECTED;
    else {
        mUDFStatus |= ArrivalStatus::NOTEXPECTED;
	mNUDFsExpected = 0;
    }

    if (FrameContents::RTP & fc.mContents)
        mRTPStatus |= ArrivalStatus::EXPECTED;
    else
        mRTPStatus |= ArrivalStatus::NOTEXPECTED;

    if (sp->mTraceOn)
        cerr << "SetFrameContents ots sitestat udfstat pstat cstat rtpstat " << mOuterTimeStep << " " << mSitesStatus << " " << mUDFStatus << " " << mPressureStatus << " " << mConstraintStatus << " " << mRTPStatus << " " << endl << flush;
    checkNewFrame(sp);
}

void Frame::newUDFd1(ED_UDFd1 &udf, SimulationParser *sp)
{
    if (udf.mcount == 1)
        mUDFs[udf.mCode].setNExpected(1);  // backward compatible with old rdg's
    else
        mUDFs[udf.mCode].setNExpected(0); //  expect payload-based udf packets and track sum
    mUDFs[udf.mCode].addValue(udf.mValue, udf.mcount);
    mUDFStatus |= ArrivalStatus::ARRIVING;
    if (sp->mTraceOn)
        cerr << "AddUDF:  outertimestep, udfnum, value, payload, done " << mOuterTimeStep << " " << udf.mCode << " " << udf.mValue << " " << mUDFs[udf.mCode].getCurrentPayload() << " " << mUDFs[udf.mCode].isComplete() << endl;
    checkNewFrame(sp);
}

void Frame::newControlPressure(ED_ControlPressure &cp, SimulationParser *sp)
{
    mEnergyInfo.mPressureInfo.mDoPressure = 1;
    mEnergyInfo.mPressureInfo.mPressure = cp.mInternalPressure.mX;
    mEnergyInfo.mPressureInfo.mVirial = cp.mVirial.mX;
    mEnergyInfo.mPressureInfo.mVolume = cp.mVolumePosition.mX;
    mEnergyInfo.mPressureInfo.mVolumeVelocity = cp.mVolumeVelocity.mX;
    mPressureStatus |= ArrivalStatus::RECEIVED;
    sp->mPressureStatus |= ArrivalStatus::RECEIVED;
    mControlPressure = cp;
    // cerr << "new pressure, checking frame" << endl;
    checkNewFrame(sp);
}

void Frame::newDynamicVariablesRandomState(ED_DynamicVariablesRandomState &cp, SimulationParser *sp)
{
    mRandomStateStatus |= ArrivalStatus::RECEIVED;
    sp->mRandomStateStatus |= ArrivalStatus::RECEIVED;
    mDynamicVariablesRandomState = cp;
    checkNewFrame(sp);
}

void Frame::newDynamicVariablesBoundingBox(ED_DynamicVariablesBoundingBox &cp, SimulationParser *sp)
{
    mBoundingBoxStatus |= ArrivalStatus::RECEIVED;
    sp->mBoundingBoxStatus |= ArrivalStatus::RECEIVED;
    mDynamicVariablesBoundingBox = cp;
    checkNewFrame(sp);
}

void Frame::newTwoChainFragmentThermostat(ED_DynamicVariablesTwoChainFragmentThermostat &t, SimulationParser *sp)
{
    mTwoChainFragmentThermostatList.set(t.mFragmentNumber, t);
    mTwoChainFragmentThermostatStatus |= ArrivalStatus::ARRIVING;
    if (mNumberOfTwoChainFragmentThermostatsExpected >= mTwoChainFragmentThermostatList.getSize() &&
	!mTwoChainFragmentThermostatList.hasGaps()) {
        mTwoChainFragmentThermostatStatus |= ArrivalStatus::RECEIVED;
        checkNewFrame(sp);
    }
}

void Frame::newOneChainThermostat(ED_DynamicVariablesOneChainThermostat &t, SimulationParser *sp)
{
    mOneChainThermo = t;
    mOneChainThermostatStatus |= ArrivalStatus::RECEIVED;
    checkNewFrame(sp);
}

void Frame::setRTP(ED_InformationRTP &cp, SimulationParser *sp)
{
    mInformationRTP = cp;
    mNumberOfTwoChainFragmentThermostatsExpected = cp.mNumberOfTwoChainThermostats;
    mRTPStatus |= ArrivalStatus::RECEIVED;
    if (sp->mTraceOn)
	cerr << "new rtp, checking frame" << endl;
    checkNewFrame(sp);
}

void Frame::newMolecularSystemDefinition(SimulationParser *sp)
{
  // mNSites = sp->mMolecularSystemDefinition.mSiteCount;
  // cout << "New MSD" << endl;
}

void Frame::newRunTimeConfiguration(SimulationParser *sp)
{
    // cout << "New RTC" << endl;
}

void Frame::setConstraints(SimulationParser *sp)
{
    mConstraintStatus |= ArrivalStatus::RECEIVED;
}
