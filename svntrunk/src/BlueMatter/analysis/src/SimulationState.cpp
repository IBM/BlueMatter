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
#include <iostream>
using namespace std ;

#include <BlueMatter/SimulationState.hpp>
#include <BlueMatter/SimulationParser.hpp>


//=================================================================================================


static void inline DefaultMsg(char *s, ED_Header &Hdr) {
    cerr << "Warning: No packet handler installed for: " << s << " type " << Hdr.mType << endl;
};


//=================================================================================================


void (*SimulationStateOutput::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

SimulationStateOutput SimulationStateOutput::mSimulationState;


//=================================================================================================


void (*SimulationStateTimeStamper::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

SimulationStateTimeStamper SimulationStateTimeStamper::mSimulationState;


void SimulationStateTimeStamper::Control(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        switch (Hdr.mType) {
            case ED_CONTROL_TYPES::TheEnd: {
                sp->theEnd();
                break;
            }
            case ED_CONTROL_TYPES::TimeStamp: {
                ED_ControlTimeStamp stamp;
                stamp.mHeader = Hdr;
                ED_OpenPacket_ControlTimeStamp(p, stamp);
                sp->setControlTimeStamp(stamp);
                break;
            }
            default: {
                // cout << "Cntrl:Default - Step, Type: " << Hdr.mShortTimeStep << " " << Hdr.mType << endl << flush;
                break;
            }
        }
    }


void SimulationStateTimeStamper::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
    }


//=================================================================================================

void (*SimulationStateDefault::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

SimulationStateDefault   SimulationStateDefault::mSimulationState;

void SimulationStateDefault::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    DefaultMsg("SimState", Hdr);
}

void SimulationStateDefault::Control(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    sp->getControlState()->process(Hdr, p, sp);
}

void SimulationStateDefault::DynamicVariables(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    sp->getDynamicVariablesState()->process(Hdr, p, sp);
}

void SimulationStateDefault::Energy(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    sp->getEnergyState()->process(Hdr, p, sp);
}

void SimulationStateDefault::Information(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    sp->getInformationState()->process(Hdr, p, sp);
}

void SimulationStateDefault::UDF(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    sp->getUDFState()->process(Hdr, p, sp);
}

void SimulationStateDefault::RTPValues(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    sp->getRTPValuesState()->process(Hdr, p, sp);
}

//=================================================================================================


void (*ControlStateDefault::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

ControlStateDefault ControlStateDefault::mControlState;

void ControlStateDefault::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    DefaultMsg("ControlState", Hdr);
}

void ControlStateDefault::SyncId(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_ControlSyncId sid;
    sid.mHeader = Hdr;
    ED_OpenPacket_ControlSyncId(p, sid);
    sp->setCurrentFullStep(sid.mFullOuterTimeStep);
}

void ControlStateDefault::TheEnd(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_ControlTheEnd te;
    te.mHeader = Hdr;
    ED_OpenPacket_ControlTheEnd(p, te);
    sp->setCurrentFullStep(te.mFullOuterTimeStep);
    sp->theEnd();
}

void ControlStateDefault::Pressure(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_ControlPressure press;
    press.mHeader = Hdr;
    ED_OpenPacket_ControlPressure(p, press);
    sp->setControlPressure(press);
    sp->mFrame[Hdr.mShortTimeStep].newControlPressure(press, sp);
}

void ControlStateDefault::TimeStamp(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_ControlTimeStamp stamp;
    stamp.mHeader = Hdr;
    ED_OpenPacket_ControlTimeStamp(p, stamp);
    sp->setControlTimeStamp(stamp);
    // sp->mFrame[Hdr.mShortTimeStep].newControlTimeStamp(stamp, sp);
}

void ControlStateDefault::ReplicaExchangePeriodInOTS(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_ControlReplicaExchangePeriodInOTS reperiod;
    reperiod.mHeader = Hdr;
    ED_OpenPacket_ControlReplicaExchangePeriodInOTS(p, reperiod);
    sp->setControlReplicaExchangePeriodInOTS(reperiod);
}

void ControlStateDefault::ReplicaExchangeAttempt(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_ControlReplicaExchangeAttempt reattempt;
    reattempt.mHeader = Hdr;
    ED_OpenPacket_ControlReplicaExchangeAttempt(p, reattempt);
    sp->setControlReplicaExchangeAttempt(reattempt);
}

//=================================================================================================


void (*DynamicVariablesStateDefault::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

DynamicVariablesStateDefault DynamicVariablesStateDefault::mDynamicVariablesState;


void DynamicVariablesStateDefault::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    DefaultMsg("DynamicVariables", Hdr);
}

void DynamicVariablesStateDefault::Site(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_DynamicVariablesSite dvs;
    dvs.mHeader = Hdr;
    ED_OpenPacket_DynamicVariablesSite(p, dvs);
    sp->mFrame[Hdr.mShortTimeStep].newSite(dvs, sp);
    sp->doSitePacket(dvs);
}

void DynamicVariablesStateDefault::Force(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_DynamicVariablesForce dvs;
    dvs.mHeader = Hdr;
    ED_OpenPacket_DynamicVariablesForce(p, dvs);
    // sp->mFrame[Hdr.mShortTimeStep].newForce(dvs, sp);
    sp->doForcePacket(dvs);
}

void DynamicVariablesStateDefault::TwoChainFragmentThermostat(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_DynamicVariablesTwoChainFragmentThermostat dvs;
    dvs.mHeader = Hdr;
    ED_OpenPacket_DynamicVariablesTwoChainFragmentThermostat(p, dvs);
    sp->mFrame[Hdr.mShortTimeStep].newTwoChainFragmentThermostat(dvs, sp);
    sp->doTwoChainFragmentThermostatPacket(dvs);
}

void DynamicVariablesStateDefault::OneChainThermostat(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_DynamicVariablesOneChainThermostat dvs;
    dvs.mHeader = Hdr;
    ED_OpenPacket_DynamicVariablesOneChainThermostat(p, dvs);
    sp->mFrame[Hdr.mShortTimeStep].newOneChainThermostat(dvs, sp);
    sp->doOneChainThermostatPacket(dvs);
}

void DynamicVariablesStateDefault::RandomState(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_DynamicVariablesRandomState dvs;
    dvs.mHeader = Hdr;
    ED_OpenPacket_DynamicVariablesRandomState(p, dvs);
    sp->mFrame[Hdr.mShortTimeStep].newDynamicVariablesRandomState(dvs, sp);
    sp->doRandomState(dvs);
}

void DynamicVariablesStateDefault::BoundingBox(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_DynamicVariablesBoundingBox dvs;
    dvs.mHeader = Hdr;
    ED_OpenPacket_DynamicVariablesBoundingBox(p, dvs);
    sp->mFrame[Hdr.mShortTimeStep].newDynamicVariablesBoundingBox(dvs, sp);
    sp->doBoundingBox(dvs);
}

//=================================================================================================


void (*EnergyStateDefault::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

EnergyStateDefault EnergyStateDefault::mEnergyState;


void EnergyStateDefault::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    DefaultMsg("EnergyState", Hdr);
}

void EnergyStateDefault::Bonded(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    cerr << "Deprecated packet: Bonded" << endl << flush;
    assert(0);
//    ED_EnergyBonded eb;
//    eb.mHeader = Hdr;
//    ED_OpenPacket_EnergyBonded(p, eb);
//    sp->mFrame[Hdr.mShortTimeStep].newFragmentEnergy(eb, sp);
}

void EnergyStateDefault::Nonbonded(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    cerr << "Deprecated packet: Nonbonded" << endl << flush;
    assert(0);
//    ED_EnergyNonBonded enb;
//    enb.mHeader = Hdr;
//    ED_OpenPacket_EnergyNonBonded(p, enb);
//    sp->mFrame[Hdr.mShortTimeStep].newNonbondedEnergy(enb, sp);
}

//=================================================================================================


void (*InformationStateDefault::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

InformationStateDefault InformationStateDefault::mInformationState;

void InformationStateDefault::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    DefaultMsg("InformationState", Hdr);
}

void InformationStateDefault::MolecularSystemDefinition(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_InformationMolecularSystemDefinition msd;
    msd.mHeader = Hdr;
    ED_OpenPacket_InformationMolecularSystemDefinition(p, msd);
    sp->setMolecularSystemDefinition(msd);
}

void InformationStateDefault::RunTimeConfiguration(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_InformationRunTimeConfiguration rtc;
    rtc.mHeader = Hdr;
    ED_OpenPacket_InformationRunTimeConfiguration(p, rtc);
    sp->setRunTimeConfiguration(rtc);
}

void InformationStateDefault::UDFCount(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_InformationUDFCount udfc;
    udfc.mHeader = Hdr;
    ED_OpenPacket_InformationUDFCount(p, udfc);
    sp->mFrame[Hdr.mShortTimeStep].setUDFCount(udfc, sp);
}

void InformationStateDefault::FrameContents(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_InformationFrameContents fc;
    fc.mHeader = Hdr;
    ED_OpenPacket_InformationFrameContents(p, fc);
    sp->mFrame[Hdr.mShortTimeStep].setFrameContents(fc, sp);
}

void InformationStateDefault::Constraints(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_InformationConstraints c;
    c.mHeader = Hdr;
    ED_OpenPacket_InformationConstraints(p, c);
    sp->setConstraints(c);
    sp->mFrame[Hdr.mShortTimeStep].setConstraints(sp);
}

void InformationStateDefault::RTP(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_InformationRTP rtp;
    rtp.mHeader = Hdr;
    ED_OpenPacket_InformationRTP(p, rtp);
    sp->setRTP(rtp);
    sp->mFrame[Hdr.mShortTimeStep].setRTP(rtp, sp);
}


//=================================================================================================


void (*UDFStateDefault::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

UDFStateDefault UDFStateDefault::mUDFState;

void UDFStateDefault::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    DefaultMsg("UDFState", Hdr);
}

void UDFStateDefault::d1(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_UDFd1 udf;
    udf.mHeader = Hdr;
    ED_OpenPacket_UDFd1(p, udf);
    // cerr << "Assigning udf to frame with short ts " << Hdr.mShortTimeStep << endl;
    sp->doUDFd1(udf);
    sp->mFrame[Hdr.mShortTimeStep].newUDFd1(udf, sp);
}

void UDFStateDefault::d2(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_UDFd2 udf;
    udf.mHeader = Hdr;
    ED_OpenPacket_UDFd2(p, udf);
}


//=================================================================================================


void (*RTPValuesStateDefault::mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);

RTPValuesStateDefault RTPValuesStateDefault::mRTPValuesState;

void RTPValuesStateDefault::Default(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    DefaultMsg("RTPValuesState", Hdr);
}

void RTPValuesStateDefault::VelocityResampleTargetTemperature(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_RTPValuesVelocityResampleTargetTemperature vrtt;
    vrtt.mHeader = Hdr;
    ED_OpenPacket_RTPValuesVelocityResampleTargetTemperature(p, vrtt);
    sp->velocityResampleTargetTemperature(vrtt);
}

void RTPValuesStateDefault::NumberOfOuterTimeSteps(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_RTPValuesNumberOfOuterTimeSteps vrtt;
    vrtt.mHeader = Hdr;
    ED_OpenPacket_RTPValuesNumberOfOuterTimeSteps(p, vrtt);
    sp->numberOfOuterTimeSteps(vrtt);
}

void RTPValuesStateDefault::SnapshotPeriodInOTS(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_RTPValuesSnapshotPeriodInOTS vrtt;
    vrtt.mHeader = Hdr;
    ED_OpenPacket_RTPValuesSnapshotPeriodInOTS(p, vrtt);
    sp->snapshotPeriodInOTS(vrtt);
}

void RTPValuesStateDefault::EmitEnergyTimeStepModulo(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_RTPValuesEmitEnergyTimeStepModulo vrtt;
    vrtt.mHeader = Hdr;
    ED_OpenPacket_RTPValuesEmitEnergyTimeStepModulo(p, vrtt);
    sp->emitEnergyTimeStepModulo(vrtt);
}

void RTPValuesStateDefault::VelocityResampleRandomSeed(ED_Header &Hdr, char *p, SimulationParser *sp)
{
    ED_RTPValuesVelocityResampleRandomSeed vrtt;
    vrtt.mHeader = Hdr;
    ED_OpenPacket_RTPValuesVelocityResampleRandomSeed(p, vrtt);
    sp->velocityResampleRandomSeed(vrtt);
}

//=================================================================================================
