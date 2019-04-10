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
 #ifndef DATARECEIVER_HPP
#define DATARECEIVER_HPP

// #include <BlueMatter/PacketStructures.hpp>
#include <pk/fxlogger.hpp>
#include <BlueMatter/SimulationState.hpp>
#include <BlueMatter/Frame.hpp>

#define DONEXT_0(foo) {        \
    if (mNextReceiver)         \
        mNextReceiver->foo();  \
    }

#define DONEXT_1(foo, x) {     \
    if (mNextReceiver)         \
        mNextReceiver->foo(x); \
    }


class DataReceiver
{
protected:
    int mDone;
    int mFinalStatus;
    DataReceiver *mNextReceiver;

public:

    DataReceiver()
    {
        mDone = 0;
        mFinalStatus = 0;
        mNextReceiver = NULL;
    }

    virtual void attach(DataReceiver *dr)
    {
        mNextReceiver = dr;
    }

    virtual int getFinalStatus()
    {
        return mFinalStatus;
    }

    // New state, e.g. pos and vel for all sites
    // virtual void newSites(Frame *x)
    // { DONEXT_1(newSites, x) };

    virtual void newSitePacket(ED_DynamicVariablesSite &x)
    { DONEXT_1(newSitePacket, x) };

    virtual void newSiteCount(int x)
    { DONEXT_1(newSiteCount, x) };

    virtual void newForcePacket(ED_DynamicVariablesForce &f)
    { DONEXT_1(newForcePacket, f) }

    virtual void newDynamicVariablesRandomState(ED_DynamicVariablesRandomState &f)
    { DONEXT_1(newDynamicVariablesRandomState, f) }

    virtual void newUDFd1(ED_UDFd1 &f)
    { DONEXT_1(newUDFd1, f) }

    virtual void newDynamicVariablesBoundingBox(ED_DynamicVariablesBoundingBox &f)
    { DONEXT_1(newDynamicVariablesBoundingBox, f) }

    virtual void newTwoChainFragmentThermostatPacket(ED_DynamicVariablesTwoChainFragmentThermostat &t)
    { DONEXT_1(newTwoChainFragmentThermostatPacket, t) }

    virtual void newOneChainThermostatPacket(ED_DynamicVariablesOneChainThermostat &t)
    { DONEXT_1(newOneChainThermostatPacket, t) }

    // New bonded energy
    // virtual void newBondedEnergy(tEnergySumAccumulator &x)
    // { DONEXT_1(newBondedEnergy, x) };

    // New non-bonded energy
    // virtual void newNonBondedEnergy(tEnergySumAccumulator &x)
    // { DONEXT_1(newNonBondedEnergy, x) };

    // Called before running packet grabber
    virtual void init()
    { DONEXT_0(init) };

    // Query done prior to each packet grab
    virtual int done()
    {
        if (mDone)
            return 1;
        if (mNextReceiver)
            return mNextReceiver->done();
        else
            return 0;
    }

    virtual void packet(Packet *p)
    { DONEXT_1(packet, p) }

    virtual void frame(Frame *f)
    { DONEXT_1(frame, f) }

    virtual void sites(Frame *f)
    { DONEXT_1(sites, f) }

    // virtual void energy(Frame *f)
    // { DONEXT_1(energy, f) }

    virtual void udfs(Frame *f)
    { DONEXT_1(udfs, f) }

    virtual void logInfo(Frame *f)
    { DONEXT_1(logInfo, f) }

    // Called after packet grabbing completed
    virtual void final(int status=1)
    { DONEXT_1(final, status) };

    // Called when all info for the "current" time step has been acquired
    // e.g. for synching a logged output of variables that arrive independently
    virtual void sync()
    { DONEXT_0(sync) };

    virtual void velocityResampleTargetTemperature(ED_RTPValuesVelocityResampleTargetTemperature &vrtt)
    { DONEXT_1(velocityResampleTargetTemperature, vrtt) };

    virtual void numberOfOuterTimeSteps(ED_RTPValuesNumberOfOuterTimeSteps &vrtt)
    { DONEXT_1(numberOfOuterTimeSteps, vrtt) };

    virtual void snapshotPeriodInOTS(ED_RTPValuesSnapshotPeriodInOTS &vrtt)
    { DONEXT_1(snapshotPeriodInOTS, vrtt) };

    virtual void emitEnergyTimeStepModulo(ED_RTPValuesEmitEnergyTimeStepModulo &vrtt)
    { DONEXT_1(emitEnergyTimeStepModulo, vrtt) };

    virtual void velocityResampleRandomSeed(ED_RTPValuesVelocityResampleRandomSeed &vrtt)
    { DONEXT_1(velocityResampleRandomSeed, vrtt) };

    virtual void informationRTP(ED_InformationRTP &rtp)
    { DONEXT_1(informationRTP, rtp) };

    virtual void controlPressure(ED_ControlPressure &pressure)
    { DONEXT_1(controlPressure, pressure) };

    virtual void controlTimeStamp(ED_ControlTimeStamp &stamp)
    { DONEXT_1(controlTimeStamp, stamp) };

    virtual void controlReplicaExchangePeriodInOTS(ED_ControlReplicaExchangePeriodInOTS &period)
    { DONEXT_1(controlReplicaExchangePeriodInOTS, period) };

    virtual void controlReplicaExchangeAttempt(ED_ControlReplicaExchangeAttempt &attempt)
    { DONEXT_1(controlReplicaExchangeAttempt, attempt) };

};

#endif
