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
 #include <memory.h>
#include <cstdio>
using namespace std ;

#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/SimulationState.hpp>
#include <BlueMatter/DataReceiverSimpleLogger.hpp>

SimulationParser::SimulationParser(char *fname, float timeout, int wait, int quiet, int debug, int debugstep, int skewwarning, int seekahead)
{
    mDone = 0;
    mUseTail = 0;
    strcpy(mFileName, "");
    if (fname) {
        mTailFile.Open(fname, timeout, wait, seekahead);
        strcpy(mFileName, fname);
        mUseTail = 1;
    }
    mQuiet = quiet;
    mDebugMode = debug;
    mDebugStartTimeStep = debugstep;
    mTraceOn = 0;
    if (mDebugMode && mDebugStartTimeStep < 0)
        mTraceOn = 1;
    mSkewWarningNumber = skewwarning;
    mNReceivers = 0;
    mMSDKnown = 0;
    mRTCKnown = 0;
    mConstraintsKnown = 0;
    mRTPKnown = 0;
    mPressureStatus = ArrivalStatus::UNKNOWN;
    mCurrentFullTimeStep = 0;
    mEndPending = 0;
    mCompleteFramesPending = 0;
    mCurrentFullTimeStepKnown = 0;

    mSimulationState         = SimulationStateDefault::getInstance();
    mControlState            = ControlStateDefault::getInstance();
    mDynamicVariablesState   = DynamicVariablesStateDefault::getInstance();
    mEnergyState             = EnergyStateDefault::getInstance();
    mInformationState        = InformationStateDefault::getInstance();
    mUDFState                = UDFStateDefault::getInstance();
    mRTPValuesState          = RTPValuesStateDefault::getInstance();

    for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
        mStdUDFError[i] = 0;
        mUDFMean[i]     = 0;
        mUDFLogCount[i] = 0;
        mUDFValue1[i] = 0;
        mUDFValue2[i] = 0;
    }
    mStdPositionError = 0;
    mStdVelocityError = 0;
    mPositionLogCount = 0;
    mVelocityLogCount = 0;
    mEnergyFigTolerance = SIGFIGMATCH;
    mPositionFigTolerance = SIGFIGMATCH;
    mVelocityFigTolerance = SIGFIGMATCH;
    mEnergyFigError = SIGFIGMATCH;
    mPositionFigError = SIGFIGMATCH;
    mVelocityFigError = SIGFIGMATCH;
    mDiffState = 0;
    mNUDFsLogging = 0;
}

void SimulationParser::getPacket(char *packet)
{
    char Record[1024];
    char *ptr = Record;
    ED_Header Hdr;
    int ok;

    if (!mUseTail) {
        cerr << "Trying to do getPacket with no TailFile assigned." << endl;
        exit(-1);
    }

#if 0
    // cerr << "get packet completeframespending " << mCompleteFramesPending << endl;;
    while (mCompleteFramesPending) {
        // cerr << "getPacket kill frames" << endl;
        deleteCompleteFrame();
        mCompleteFramesPending--;
    }
#endif
    // If there is a complete frame pending, and if it is the next one destined to go, delete it.
    if (mCompleteFramesPending) {
        Frame *f;
	// check head frame and see if it is complete
        while ( f=getNextFrame() ) {
	    // As front frame is deleted, new ones must be checked
	    f->checkNewFrame(this);
            if (!f->mComplete) {
		if (mTraceOn)
		    cerr << "Frame is not complete in getPacket" << endl;
                break;
	    }
            if (mTraceOn)
                cerr << "deleting next frame in getpacket with CompleteFramesPending " << mCompleteFramesPending << endl;
            deleteNextFrame();
            mCompleteFramesPending--;
        }
    }

    if (done()) {
	// cerr << "done in getPacket" << endl;
        return;
    }

    ok = mTailFile.Read( &(Record[0]), sizeof( ED_Header ) );
    int *checkval = (int *)Record;

    // keep retrying if the header is 0 (which must be read error)
    int maxtry = 1000;
    int ntry = 0;
    while (((*checkval == 0)  && ok) && (ntry < maxtry)) {
	usleep(5000);
	// cerr << "ReReading header 0 " << ntry << endl;
	ok = mTailFile.Read( &(Record[0]), sizeof( ED_Header ), 1 );
	ntry++;
    }

    ntry = 0;
    if (ok) {
        BM_PacketPrimitive::Unpack(ptr, Hdr);
        ok = mTailFile.Read( (void *) &Record[ 0 ],
            Packet::getPayloadSize(Hdr) );
#if 0
	while (!ok && (ntry < maxtry)) {
	    usleep(5000);
	    ok = mTailFile.Read( (void *) &Record[ 0 ],
		Packet::getPayloadSize(Hdr), 1 );
	    ntry++;
	}
#endif
    }

    if (!ok) {
        mDone = 1;
        if (mDebugMode)
            cerr << "not ok after TailFile read in getpacket" << ok << endl;
        return;
    }

    // if (mTraceOn)
    //    cerr << " In getPacket - header " << Hdr.mClass << " " << Hdr.mType << " " << Hdr.mShortTimeStep << endl;

    // Allow caller to get the actual packet bytes if desired
    if (packet) {
        memcpy(packet, (void *)&Hdr, sizeof(ED_Header));
        memcpy(&packet[sizeof(ED_Header)], Record, Hdr.mLen);
        // cout << " In getPacket - " << Packet::getClass(packet) << " " << Packet::getType(packet) << endl;
    }

    mSimulationState->process(Hdr, Record, this);

}

void SimulationParser::pushPacket(char *packet)
{
    char *ptr = packet;
    ED_Header Hdr;
    int ok;

    if (getNextFrame()) {
        if (mTraceOn)
            cout << "deleting next frame in getpacket with CompleteFramesPending " << mCompleteFramesPending << endl;
        deleteNextFrame();
        mCompleteFramesPending--;
    }

    if (done())
        return;

    BM_PacketPrimitive::Unpack(ptr, Hdr);

    mSimulationState->process(Hdr, ptr, this);

}

Frame *SimulationParser::getCompleteFrame()
{
    if (mTraceOn)
        cerr << "getCompleteFrame NumItems: " << mFrame.numItems() << endl;
    Frame *f = mFrame.getHead();
    if (f && mTraceOn) {
	cerr << "Frame, ots, complete, sitedatasize, hasgaps: " << " " << f->mOuterTimeStep << " " << f->mComplete << " "  << f->mSiteData.getSize() << " " << f->mSiteData.hasGaps() << endl;
    }
    if (f && f->mComplete)
	return f;
    else
	return NULL;
}

#if 0
void SimulationParser::deleteCompleteFrame()
{
    Frame *pF = getCompleteFrame();
    if (pF) {
        if (mTraceOn)
            cerr << "deleting in deletecompleteframe" << pF->mOuterTimeStep << endl;
        mFrame.deleteItem((pF->mOuterTimeStep & 0xff));
    }
}
#endif

// get head frame whether or not it is complete yet
Frame *SimulationParser::getNextFrame()
{
    Frame *f = mFrame.getHead();
    if (f && mTraceOn) {
	cerr << "getnextFrame, ots, complete, SiteSize, HasGaps: " <<  f->mOuterTimeStep << " " << f->mComplete;
	cerr << " " << f->mSiteData.getSize() << " " << f->mSiteData.hasGaps() << endl;
    }
    if (mTraceOn && !f) 
	cerr << "getnextFrame sees no head frame, frame count is " << mFrame.numItems() << endl;
    return f;
}

void SimulationParser::deleteNextFrame()
{
    Frame *pF = getNextFrame();
    if (pF) {
        if (mTraceOn)
            cerr << "deleteNextFrame" << endl;
        mFrame.deleteItem((pF->mOuterTimeStep & 0xff));
    }
}

void SimulationParser::getFrame(Frame **ppF)
{
    // If just acquired a frame, need to let getPacket clear it up
    if (!done())
        getPacket();
    while (!done()  && !getCompleteFrame())
        getPacket();
    *ppF = getCompleteFrame();
}

void SimulationParser::run()
{
    while(!done())
    {
        getPacket();

    }
}

void SimulationParser::dumpPackets()
{
    mSimulationState = SimulationStateOutput::getInstance();
}

void SimulationParser::dumpTimes()
{
    mSimulationState = SimulationStateTimeStamper::getInstance();
}

void SimulationParser::getByte(char &c)
{
    int ok;
    ok = mTailFile.Read( &c, 1 );
    if (!ok) {
	cerr << "error in getByte during synchStream" << endl;
	exit(-1);
    }
}

void SimulationParser::skip(int nskip)
{
    char buff[1000];
    int ntoskip = nskip;
    while (ntoskip > 0) {
	int ntry = ntoskip;
	if (ntry > 1000)
	    ntry = 1000;
	int ok = mTailFile.Read(buff, ntry);
	if (!ok) {
	    cerr << "error reading tailfile " << ntry << " bytes during skip" << endl;
	    cerr << "have " << ntoskip << " remaining while trying to skip " << nskip << endl;
	    exit(-1);
	}
	ntoskip -= ntry;
    }
}

int SimulationParser::synchStream()
{
    // read a byte at a time until a valid synch packet appears - no seeking
    // invariant, timestep, variable, invariant, checksum
    // so look for invariant/8/invariant/4
    // invariable is abcdefed
    // variable is 01010001
    // search for invariant
    // read 8
    // confirm next invariant
    // read 4

    const int key = 0xabcdefed;
    char buf[4];
    int &val = (int &)buf; 

    // prime the buffer
    getByte(buf[0]);
    getByte(buf[1]);
    getByte(buf[2]);
    getByte(buf[3]);

    int gotit = 0;

    while (!gotit) {
	// seek for key one byte at a time
	while (val != key) {
	    buf[0] = buf[1];
	    buf[1] = buf[2];
	    buf[2] = buf[3];
	    getByte(buf[3]);
	}

	// found key, so skip over 8 bytes
	for (int i=0; i<8; i++)
	    getByte(buf[0]);
	
	// confirm next is key
	for (int i=0; i<4; i++)
	    getByte(buf[i]);
	
	gotit = (val == key);
    }
    // skip over checksum
    if (gotit) {
	for (int i=0; i<4; i++)
	    getByte(buf[0]);
    }
    return gotit;
}

int SimulationParser::checkSummaryEnergyTolerance()
{
    int diff = 0;
    for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
        if (mUDFLogCount[i]) {
            double sigma = sqrt(mStdUDFError[i]/mUDFLogCount[i]);
            if (sigma == 0)
                continue;
            diff |= 1;
            double mean = fabs(mUDFMean[i]/mUDFLogCount[i]);
            if (mean < sigma)
                mean = sigma;
            double figdiff = (log(mean) - log(sigma)) / log(10.0);
            double figsigma = -log(sigma)/log(10.0);
            if (figsigma > figdiff)
                figdiff = figsigma;
            if (figdiff < mEnergyFigTolerance) {
                diff |= 2;
            }
        }
    }
    return diff;
}

int SimulationParser::checkSummaryPositionVelocityTolerance()
{
    int diff = 0;
    if (mPositionLogCount > 0) {
        double dpos = sqrt(mStdPositionError/mPositionLogCount);
        double dvel = sqrt(mStdVelocityError/mVelocityLogCount);
        if (dpos > 0 || dvel > 0)
            diff = 1;
        if (dpos > 1.0E-7 || dvel > 1.0E-7)
            diff |= 2;
    }
    return diff;
}

int SimulationParser::reportSumSummary()
{
    char buff[512];
    char numbuff[32];
    double sum1 = 0;
    double sum2 = 0;
    double PosSigError;
    double VelSigError;

    cout << "# Diff of UDF Sums" << endl;
    for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
        if (mUDFLogCount[i]) {
            sum1 += mUDFValue1[i];
            sum2 += mUDFValue2[i];
            strcat(buff, numbuff);
        }
    }
    double delta = sum1-sum2;
    double ESigError;
    double absdelta = fabs(delta);
    double emax = fabs(sum1);
    if (fabs(sum2) > emax)
        emax = fabs(sum2);
    if (absdelta > 0)
        ESigError = (log(emax)-log(absdelta))/log(10.0);
    else
        ESigError = SIGFIGMATCH;
    sprintf(buff, "# %15E SigFigs = %8.1f", delta, ESigError);
    cout << buff << endl;

    sprintf(buff, "# RMS Position, Velocity = %15E %15E", sqrt(mStdPositionError/mPositionLogCount), sqrt(mStdVelocityError/mVelocityLogCount));
    cout << buff << endl;
    int differ;
    if (ESigError != SIGFIGMATCH)
        differ = 1;
    if (ESigError < mEnergyFigTolerance)
        differ |= 2;
    if (!finite(emax))
        differ |= 4;
    return differ;
}

void SimulationParser::reportSummary()
{
    char buff[512];
    char numbuff[32];

    reportSumSummary();

    cout << "# RMS UDF Deviation" << endl;
    sprintf(buff, "# ");
    for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
        if (mUDFLogCount[i]) {
            sprintf(numbuff, "%15s", DataReceiverSimpleLogger::HeaderStringFromUDFCode(i));
            strcat(buff, numbuff);
        }
    }
    cout << buff << endl;

    sprintf(buff, "# ");
    for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
        if (mUDFLogCount[i]) {
            sprintf(numbuff, "%15E", sqrt(mStdUDFError[i]/mUDFLogCount[i]));
            strcat(buff, numbuff);
        }
    }
    cout << buff << endl;

    sprintf(buff, "# RMS Position, Velocity = %15E %15E", sqrt(mStdPositionError/mPositionLogCount), sqrt(mStdVelocityError/mVelocityLogCount));
    cout << buff << endl;
}

int SimulationParser::framesEqual(Frame *pF1, Frame *pF2, int report)
{
    double UDFDiffs[UDF_Binding::UDF_CODE_COUNT];
    double UDFFigDiffs[UDF_Binding::UDF_CODE_COUNT];
    int UDFPresent[UDF_Binding::UDF_CODE_COUNT];
    int NSites = pF1->mSiteData.getSize();
    int NSites2 = pF2->mSiteData.getSize();

    int differ = 0;
    int nan = 0;
    int outoftolerance = 0;
    char buff[512];
    char numbuff[32];

    int NLogging = 0;
    for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
        if (pF1->mUDFs[i].isComplete() || pF2->mUDFs[i].isComplete()) {
            UDFPresent[i] = 1;
            mUDFLogCount[i]++;
            NLogging++;
            mUDFValue1[i] = pF1->mUDFs[i].getSum();
            mUDFValue2[i] = pF2->mUDFs[i].getSum();
        } else {
            UDFPresent[i] = 0;
            mUDFValue1[i] = 0;
            mUDFValue2[i] = 0;
        }
    }

    int NewUDF = (NLogging != mNUDFsLogging);
    mNUDFsLogging = NLogging;


    // Dump header if requested or needed
    if (report & REPORT_HEADER || (report & REPORT_LOGGING && NewUDF)) {
        sprintf(buff, "#%6s", "Step");
        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (mUDFLogCount[i]) {
                sprintf(numbuff, "%15s", DataReceiverSimpleLogger::HeaderStringFromUDFCode(i));
                strcat(buff, numbuff);
            }
        }
        cout << buff << endl;
        cout << "#" << endl;
    }

    int NLines = 0;

    int TimeStep = pF1->mOuterTimeStep;
    if (TimeStep != pF2->mOuterTimeStep) {
        TimeStep = -TimeStep;
    }

    if (report & REPORT_VALUES) {
        sprintf(buff, "%7d", TimeStep);
        NLines++;
    }

    for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
        double v1 = pF1->mUDFs[i].getSum();
        double v2 = pF2->mUDFs[i].getSum();
        mUDFMean[i] += v1;
        double vdiff = fabs(v2 - v1);
        double vmax = fabs(v2)>fabs(v1) ? fabs(v2) : fabs(v1);
        if (!finite(v1) || !finite(v2))
            nan = 1;

        UDFDiffs[i] = vdiff;
        if (vdiff != 0) {
            differ = 1;
            mStdUDFError[i] += vdiff*vdiff;
            double figdiff = (log(vmax) - log(vdiff)) / log(10.0);
            double figval  = -log(vdiff)/log(10.0);
            if (figval > figdiff)
                figdiff = figval;
            UDFFigDiffs[i] = figdiff;
            if (UDFFigDiffs[i] < mEnergyFigTolerance) {
                outoftolerance = 1;
            }
        } else {
            UDFFigDiffs[i] = SIGFIGMATCH;
        }
        if (report & REPORT_VALUES) {
            if (mUDFLogCount[i]) {
                if (UDFPresent[i])
                    sprintf(numbuff, "%15f", v1);
                else
                    sprintf(numbuff, "%15d", 0);
                strcat(buff, numbuff);
            }
        }
    }

    if (report & REPORT_VALUES)
        cout << buff << endl;

    if (report & REPORT_ABSOLUTE) {
        sprintf(buff, "%7d", TimeStep);
        NLines++;

        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (mUDFLogCount[i]) {
                if (UDFDiffs[i] != 0)
                    sprintf(numbuff, "%15E", UDFDiffs[i]);
                else
                    sprintf(numbuff, "%15s", "0");
                strcat(buff, numbuff);
            }
        }
        cout << buff << endl;
    }

    if (report & REPORT_SIGFIGS) {
        NLines++;
        sprintf(buff, "%7d", TimeStep);
        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (mUDFLogCount[i]) {
                if (UDFDiffs[i] != 0)
                    sprintf(numbuff, "%15.1f", UDFFigDiffs[i]);
                else
                    sprintf(numbuff, "%15s", "0");
                strcat(buff, numbuff);
            }
        }
        cout << buff << endl;
    }

    if (report & REPORT_LOGGING && NLines > 1)
        cout << "#" << endl;

    if ( NSites != 0 && NSites2 != 0) {
        if ( NSites == NSites2 ) {
            tSiteData *ps1 = pF1->mSiteData.getArray();
            tSiteData *ps2 = pF2->mSiteData.getArray();
            double dPosSum = 0;
            double dVelSum = 0;
            for (int i=0; i<NSites; i++, ps1++, ps2++) {
                if (!(ps1->mPosition == ps2->mPosition) || !(ps1->mVelocity == ps2->mVelocity)) {
                    differ = 1;
                    double dPos = ps1->mPosition.DistanceSquared(ps2->mPosition);
                    dPosSum += dPos;
                    double dVel = ps1->mVelocity.DistanceSquared(ps2->mVelocity);
                    dVelSum += dVel;
                }
            }
            mStdPositionError += dPosSum/NSites;
            mStdVelocityError += dVelSum/NSites;
            mPositionLogCount++;
            mVelocityLogCount++;
            if (!finite(mStdPositionError) || !finite(mStdVelocityError))
                differ = 1;
            if (report & REPORT_SITES) {
                sprintf(buff, "# Snapshot RMS Position, Velocity = %15E %15E", sqrt(dPosSum/NSites), sqrt(dVelSum/NSites));
                cout << endl;
                cout << buff << endl;
                cout << endl;
            }
        } else if (report & REPORT_SITES) {
            sprintf(buff, "# Frames have different snapshot NSites:  %d and %d", NSites, pF2->mSiteData.getSize());
            cout << buff << endl;
            differ = 1;
        }
    } else if (report & REPORT_SITES) {
        if ( NSites != 0 || NSites2 != 0) {
            cout << "# One frame has snapshot and other does not.  No diff reported." << endl;
        }
    }

    return differ + outoftolerance*2 + nan*4;
}

