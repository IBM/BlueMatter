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
 #ifndef SIMULATIONSTATE_HPP
#define SIMULATIONSTATE_HPP

#include <BlueMatter/ExternalDatagram.hpp>
#include <iostream>
#include <iomanip>
using namespace std ;

//=================================================================================================

class SimulationParser;

class ControlState
{
public:
    virtual void process(ED_Header &Hdr, char *p, SimulationParser *sp) {}
};

class ControlStateDefault : public ControlState
{
private:
    static ControlStateDefault mControlState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void SyncId(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void TheEnd(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Pressure(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void TimeStamp(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void ReplicaExchangePeriodInOTS(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void ReplicaExchangeAttempt(ED_Header &Hdr, char *p, SimulationParser *sp);
public:
    ControlStateDefault()
    {
        for (int i=0; i<256; i++)
            mAction[i] = ControlStateDefault::Default;
        mAction[ED_CONTROL_TYPES::SyncId]  = SyncId;
        mAction[ED_CONTROL_TYPES::TheEnd]  = TheEnd;
        mAction[ED_CONTROL_TYPES::Pressure]= Pressure;
        mAction[ED_CONTROL_TYPES::TimeStamp]= TimeStamp;
        mAction[ED_CONTROL_TYPES::ReplicaExchangePeriodInOTS]= ReplicaExchangePeriodInOTS;
        mAction[ED_CONTROL_TYPES::ReplicaExchangeAttempt]= ReplicaExchangeAttempt;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mType](Hdr, p, sp);
    }
    static ControlState *getInstance() { return &mControlState; }
};


//=================================================================================================


class DynamicVariablesState
{
public:
    virtual void process(ED_Header &Hdr, char *p, SimulationParser *sp) {}
};

class DynamicVariablesStateDefault : public DynamicVariablesState
{
private:
    static DynamicVariablesStateDefault mDynamicVariablesState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Site(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Force(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void TwoChainFragmentThermostat(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void OneChainThermostat(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void RandomState(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void BoundingBox(ED_Header &Hdr, char *p, SimulationParser *sp);
    
public:
    DynamicVariablesStateDefault()
    {
        for (int i=0; i<256; i++)
            mAction[i] = Default;
        mAction[ED_DYNAMICVARIABLES_TYPES::Site]  = Site;
        mAction[ED_DYNAMICVARIABLES_TYPES::Force]  = Force;
        mAction[ED_DYNAMICVARIABLES_TYPES::TwoChainFragmentThermostat]  = TwoChainFragmentThermostat;
        mAction[ED_DYNAMICVARIABLES_TYPES::OneChainThermostat]  = OneChainThermostat;
        mAction[ED_DYNAMICVARIABLES_TYPES::RandomState]  = RandomState;
        mAction[ED_DYNAMICVARIABLES_TYPES::BoundingBox]  = BoundingBox;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mType](Hdr, p, sp);
    }
    static DynamicVariablesState *getInstance() { return &mDynamicVariablesState; }
};


//=================================================================================================

class EnergyState
{
public:
    virtual void process(ED_Header &Hdr, char *p, SimulationParser *sp) {}
};

class EnergyStateDefault : public EnergyState
{
private:
    static EnergyStateDefault mEnergyState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Bonded(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Nonbonded(ED_Header &Hdr, char *p, SimulationParser *sp);
    
public:
    EnergyStateDefault()
    {
        for (int i=0; i<256; i++)
            mAction[i] = Default;
        mAction[ED_ENERGY_TYPES::Bonded]      = Bonded;
        mAction[ED_ENERGY_TYPES::NonBonded]   = Nonbonded;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mType](Hdr, p, sp);
    }
    static EnergyState *getInstance() { return &mEnergyState; }
};


//=================================================================================================


class InformationState
{
public:
    virtual void process(ED_Header &Hdr, char *p, SimulationParser *sp) {}
};

class InformationStateDefault : public InformationState
{
private:
    static InformationStateDefault mInformationState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void MolecularSystemDefinition(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void RunTimeConfiguration(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void UDFCount(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void FrameContents(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Constraints(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void RTP(ED_Header &Hdr, char *p, SimulationParser *sp);
    
public:
    InformationStateDefault()
    {
        for (int i=0; i<256; i++)
            mAction[i] = Default;
        mAction[ED_INFORMATION_TYPES::MolecularSystemDefinition]     = MolecularSystemDefinition;
        mAction[ED_INFORMATION_TYPES::RunTimeConfiguration]          = RunTimeConfiguration;
        mAction[ED_INFORMATION_TYPES::UDFCount]                      = UDFCount;
        mAction[ED_INFORMATION_TYPES::FrameContents]                 = FrameContents;
        mAction[ED_INFORMATION_TYPES::Constraints]                   = Constraints;
        mAction[ED_INFORMATION_TYPES::RTP]                           = RTP;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mType](Hdr, p, sp);
    }
    static InformationState *getInstance() { return &mInformationState; }
};


//=================================================================================================


class UDFState
{
public:
    virtual void process(ED_Header &Hdr, char *p, SimulationParser *sp) {}
};

class UDFStateDefault : public UDFState
{
private:
    static UDFStateDefault mUDFState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void d1(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void d2(ED_Header &Hdr, char *p, SimulationParser *sp);
    
public:
    UDFStateDefault()
    {
        for (int i=0; i<256; i++)
            mAction[i] = Default;
        mAction[ED_UDF_TYPES::d1]     = d1;
        mAction[ED_UDF_TYPES::d2]     = d2;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mType](Hdr, p, sp);
    }
    static UDFState *getInstance() { return &mUDFState; }
};

//=================================================================================================


class RTPValuesState
{
public:
    virtual void process(ED_Header &Hdr, char *p, SimulationParser *sp) {}
};

class RTPValuesStateDefault : public RTPValuesState
{
private:
    static RTPValuesStateDefault mRTPValuesState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void VelocityResampleTargetTemperature(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void NumberOfOuterTimeSteps(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void SnapshotPeriodInOTS(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void EmitEnergyTimeStepModulo(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void VelocityResampleRandomSeed(ED_Header &Hdr, char *p, SimulationParser *sp);
    
public:
    RTPValuesStateDefault()
    {
        for (int i=0; i<256; i++)
            mAction[i] = Default;
        mAction[ED_RTPVALUES_TYPES::VelocityResampleTargetTemperature]     = VelocityResampleTargetTemperature;
        mAction[ED_RTPVALUES_TYPES::NumberOfOuterTimeSteps]                = NumberOfOuterTimeSteps;
        mAction[ED_RTPVALUES_TYPES::SnapshotPeriodInOTS]                   = SnapshotPeriodInOTS;
        mAction[ED_RTPVALUES_TYPES::EmitEnergyTimeStepModulo]              = EmitEnergyTimeStepModulo;
        mAction[ED_RTPVALUES_TYPES::VelocityResampleRandomSeed]            = VelocityResampleRandomSeed;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mType](Hdr, p, sp);
    }
    static RTPValuesState *getInstance() { return &mRTPValuesState; }
};

//=================================================================================================


class SimulationState
{
public:
    virtual void process(ED_Header &Hdr, char *p, SimulationParser *sp) {}
};

class SimulationStateDefault : public SimulationState
{
protected:
    static SimulationStateDefault mSimulationState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Control(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void DynamicVariables(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Energy(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Information(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void UDF(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void RTPValues(ED_Header &Hdr, char *p, SimulationParser *sp);
public:
    SimulationStateDefault()
    {
        for (int i=0; i<256; i++)
            SimulationStateDefault::mAction[i] = Default;
        mAction[ED_CLASS::Control]          = Control;
        mAction[ED_CLASS::DynamicVariables] = DynamicVariables;
        mAction[ED_CLASS::Energy]           = Energy;
        mAction[ED_CLASS::Information]      = Information;
        mAction[ED_CLASS::UDF]              = UDF;
        mAction[ED_CLASS::RTPValues]        = RTPValues;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mClass](Hdr, p, sp);
    }
    static SimulationState *getInstance() { return &mSimulationState; }
};


class SimulationStateTimeStamper : public SimulationState
{
protected:
    static SimulationStateTimeStamper mSimulationState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Control(ED_Header &Hdr, char *p, SimulationParser *sp);
public:
    SimulationStateTimeStamper()
    {
        for (int i=0; i<256; i++)
            SimulationStateTimeStamper::mAction[i] = Default;
        mAction[ED_CLASS::Control]          = Control;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mClass](Hdr, p, sp);
    }
    static SimulationState *getInstance() { return &mSimulationState; }
};



//  This is a version intended for debugging purposes, hence the use of the switch statements
//  which are avoided in the official state classes
class SimulationStateOutput : public SimulationState
{
protected:
    static SimulationStateOutput mSimulationState;
    static void (*mAction[256])(ED_Header &Hdr, char *p, SimulationParser *sp);
    static void Default(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        cout << "Default Packet - Class, Type: " << Hdr.mClass << " " << Hdr.mType << endl << flush;
    }
    static void Control(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        switch (Hdr.mType) {
            case ED_CONTROL_TYPES::SyncId: {
                ED_ControlSyncId sid;
                sid.mHeader = Hdr;
                ED_OpenPacket_ControlSyncId(p, sid);
                // cout << "Cntrl:Sync - FullStep, CheckSum: " << sid->mFullOuterTimeStep << " " << sid->mCheckSum << endl;
                cout << "Cntrl:SyncId " << sid << endl << flush;
                break;
            }
            case ED_CONTROL_TYPES::TheEnd: {
                ED_ControlTheEnd te;
                te.mHeader = Hdr;
                ED_OpenPacket_ControlTheEnd(p, te);
                // cout << "Cntrl:TheEnd - FullStep: " << te->mFullOuterTimeStep << endl;
                cout << "Cntrl:TheEnd " << te << endl << flush;
                break;
            }
            case ED_CONTROL_TYPES::Pressure: {
                ED_ControlPressure press;
                press.mHeader = Hdr;
                ED_OpenPacket_ControlPressure(p, press);
                cout << "Cntrl:Pressure " << press << endl << flush;
                break;
            }
            case ED_CONTROL_TYPES::TimeStamp: {
                ED_ControlTimeStamp stamp;
                stamp.mHeader = Hdr;
                ED_OpenPacket_ControlTimeStamp(p, stamp);
                cout << "Cntrl:TimeStamp " << stamp << endl << flush;
                break;
            }
            case ED_CONTROL_TYPES::ReplicaExchangePeriodInOTS: {
                ED_ControlReplicaExchangePeriodInOTS reperiod;
                reperiod.mHeader = Hdr;
                ED_OpenPacket_ControlReplicaExchangePeriodInOTS(p, reperiod);
                cout << "Cntrl:ReplicaExchangePeriodInOTS " << reperiod << endl << flush;
                break;
            }
            case ED_CONTROL_TYPES::ReplicaExchangeAttempt: {
                ED_ControlReplicaExchangeAttempt reattempt;
                reattempt.mHeader = Hdr;
                ED_OpenPacket_ControlReplicaExchangeAttempt(p, reattempt);
                cout << "Cntrl:ReplicaExchangeAttempt " << reattempt << endl << flush;
                break;
            }
            default: {
                cout << "Cntrl:Default - Step, Type: " << Hdr.mShortTimeStep << " " << Hdr.mType << endl << flush;
                break;
            }
        }
    }
    static void DynamicVariables(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        switch (Hdr.mType) {
            case ED_DYNAMICVARIABLES_TYPES::Site: {
                ED_DynamicVariablesSite sdv;
                sdv.mHeader = Hdr;
                ED_OpenPacket_DynamicVariablesSite(p, sdv);
                // cout << "DynVar:Site - Step, P, V: " << /* sdv->mOuterTimeStep */ 9999 << " " << sdv->mSiteId << " " << sdv->mPosition << " " << sdv->mVelocity << endl;
                cout << "DynVar:Site " << sdv << endl << flush;
                break;
            }
            case ED_DYNAMICVARIABLES_TYPES::Force: {
                ED_DynamicVariablesForce sdv;
                sdv.mHeader = Hdr;
                ED_OpenPacket_DynamicVariablesForce(p, sdv);
                // cout << "DynVar:Force - Step, P, V: " << /* sdv->mOuterTimeStep */ 9999 << " " << sdv->mSiteId << " " << sdv->mPosition << " " << sdv->mVelocity << endl;
                cout << "DynVar:Force " << sdv << endl << flush;
                break;
            }
            case ED_DYNAMICVARIABLES_TYPES::TwoChainFragmentThermostat: {
                ED_DynamicVariablesTwoChainFragmentThermostat sdv;
                sdv.mHeader = Hdr;
                ED_OpenPacket_DynamicVariablesTwoChainFragmentThermostat(p, sdv);
                cout << "DynVar:TwoChainFragThermo " << sdv << endl << flush;
                break;
            }
            case ED_DYNAMICVARIABLES_TYPES::OneChainThermostat: {
                ED_DynamicVariablesOneChainThermostat sdv;
                sdv.mHeader = Hdr;
                ED_OpenPacket_DynamicVariablesOneChainThermostat(p, sdv);
                cout << "DynVar:OneChainThermo " << sdv << endl << flush;
                break;
            }
            case ED_DYNAMICVARIABLES_TYPES::RandomState: {
                ED_DynamicVariablesRandomState sdv;
                sdv.mHeader = Hdr;
                ED_OpenPacket_DynamicVariablesRandomState(p, sdv);
                cout << "DynVar:RandomState " << sdv << endl << flush;
                break;
            }
            case ED_DYNAMICVARIABLES_TYPES::BoundingBox: {
                ED_DynamicVariablesBoundingBox sdv;
                sdv.mHeader = Hdr;
                ED_OpenPacket_DynamicVariablesBoundingBox(p, sdv);
                cout << "DynVar:BirthBoundingBox " << sdv << endl << flush;
                break;
            }
            default: {
                cout << "DynVar:Default - Step, Type: " << Hdr.mShortTimeStep << " " << Hdr.mType << endl << flush;
                break;
            }
        }
    }
    static void Energy(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        switch (Hdr.mType) {
            case ED_ENERGY_TYPES::Bonded: {
                ED_EnergyBonded eb;
                eb.mHeader = Hdr;
                ED_OpenPacket_EnergyBonded(p, eb);
                // cout << "E:Bnded  - Step, ID: " << fe->mOuterTimeStep << " " << fe->mFragmentId << endl;
                cout << "E:Bnded - " << eb << endl << flush;
                break;
            }
            case ED_ENERGY_TYPES::NonBonded: {
                ED_EnergyNonBonded enb;
                enb.mHeader = Hdr;
                ED_OpenPacket_EnergyNonBonded(p, enb);
                // cout << "E:NonBnded - Step, Ordinal: " << nbe->mOuterTimeStep << " " << nbe->mOrdinal << endl;
                cout << "E:NonBnded - "  << enb << endl << flush;
                break;
            }
            default: {
                cout << "E:Default - Step, Type: " << Hdr.mShortTimeStep << " " << Hdr.mType << endl << flush;
                break;
            }
        }
    }
    static void Information(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        switch (Hdr.mType) {
        case ED_INFORMATION_TYPES::MolecularSystemDefinition: {
            ED_InformationMolecularSystemDefinition msd;
            msd.mHeader = Hdr;
            ED_OpenPacket_InformationMolecularSystemDefinition(p, msd);
            // cout << "Info:MSD - FragCount, SiteCount, CheckSum: " << msd->mFragmentCount << " " << msd->mSiteCount << " " << msd->mMSDCheckSum << endl;
            cout << "Info:MSD - " << msd << endl << flush;
            break;
        }
        case ED_INFORMATION_TYPES::RunTimeConfiguration: {
            ED_InformationRunTimeConfiguration rtc;
            rtc.mHeader = Hdr;
            ED_OpenPacket_InformationRunTimeConfiguration(p, rtc);
            // cout << "Info:RunTimeConfig - NonBondedEngineCount, VoxelCount: " << rtf->mNonBondedEngineCount << " " << rtf->mVoxelCount << endl;
            cout << "Info:RunTimeConfig - " << rtc << endl << flush;
            break;
        }
        case ED_INFORMATION_TYPES::UDFCount: {
            ED_InformationUDFCount udfc;
            udfc.mHeader = Hdr;
            ED_OpenPacket_InformationUDFCount(p, udfc);
            cout << "Info:UDFCount - " << udfc << endl << flush;
            break;
        }
        case ED_INFORMATION_TYPES::FrameContents: {
            ED_InformationFrameContents fc;
            fc.mHeader = Hdr;
            ED_OpenPacket_InformationFrameContents(p, fc);
            cout << "Info:FrameContents - " << fc << endl << flush;
            break;
        }
        case ED_INFORMATION_TYPES::Constraints: {
            ED_InformationConstraints c;
            c.mHeader = Hdr;
            ED_OpenPacket_InformationConstraints(p, c);
            cout << "Info:Constraints - " << c << endl << flush;
            break;
        }
        case ED_INFORMATION_TYPES::RTP: {
            ED_InformationRTP rtp;
            rtp.mHeader = Hdr;
            ED_OpenPacket_InformationRTP(p, rtp);
            cout << "Info:RTP - " << rtp << endl << flush;
            break;
        }
        default: {
            cout << "Info:Default - Step, Type: " << Hdr.mShortTimeStep << " " << Hdr.mType << endl << flush;
            break;
        }
        }
    }
    static void UDF(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        switch (Hdr.mType) {
        case ED_UDF_TYPES::d1: {
            ED_UDFd1 udf;
            udf.mHeader = Hdr;
            ED_OpenPacket_UDFd1(p, udf);
            cout << "UDF:d1 - " << udf << endl << flush;
            break;
        }
        case ED_UDF_TYPES::d2: {
            ED_UDFd2 udf;
            udf.mHeader = Hdr;
            ED_OpenPacket_UDFd2(p, udf);
            cout << "UDF:d2 - " << udf << endl << flush;
            break;
        }
        default: {
            cout << "UDF:Default - Step, Type: " << Hdr.mShortTimeStep << " " << Hdr.mType << endl << flush;
            break;
        }
        }
    }
    static void RTPValues(ED_Header &Hdr, char *p, SimulationParser *sp)
    {
        switch (Hdr.mType) {
        case ED_RTPVALUES_TYPES::VelocityResampleTargetTemperature: {
            ED_RTPValuesVelocityResampleTargetTemperature vrtt;
            vrtt.mHeader = Hdr;
            ED_OpenPacket_RTPValuesVelocityResampleTargetTemperature(p, vrtt);
            cout << "RTPValues:VelocityResampleTargetTemperature - " << vrtt << endl << flush;
            break;
        }
        case ED_RTPVALUES_TYPES::NumberOfOuterTimeSteps: {
            ED_RTPValuesNumberOfOuterTimeSteps noots;
            noots.mHeader = Hdr;
            ED_OpenPacket_RTPValuesNumberOfOuterTimeSteps(p, noots);
            cout << "RTPValues:NumberOfOuterTimeSteps - " << noots << endl << flush;
            break;
        }
        case ED_RTPVALUES_TYPES::SnapshotPeriodInOTS: {
            ED_RTPValuesSnapshotPeriodInOTS spio;
            spio.mHeader = Hdr;
            ED_OpenPacket_RTPValuesSnapshotPeriodInOTS(p, spio);
            cout << "RTPValues:SnapshotPeriodInOTS - " << spio << endl << flush;
            break;
        }
        case ED_RTPVALUES_TYPES::EmitEnergyTimeStepModulo: {
            ED_RTPValuesEmitEnergyTimeStepModulo eetsm;
            eetsm.mHeader = Hdr;
            ED_OpenPacket_RTPValuesEmitEnergyTimeStepModulo(p, eetsm);
            cout << "RTPValues:EmitEnergyTimeStepModulo - " << eetsm << endl << flush;
            break;
        }
        case ED_RTPVALUES_TYPES::VelocityResampleRandomSeed: {
            ED_RTPValuesVelocityResampleRandomSeed vrtt;
            vrtt.mHeader = Hdr;
            ED_OpenPacket_RTPValuesVelocityResampleRandomSeed(p, vrtt);
            cout << "RTPValues:VelocityResampleRandomSeed - " << vrtt << endl << flush;
            break;
        }
        default: {
            cout << "RTPValues:Default - Step, Type: " << Hdr.mShortTimeStep << " " << Hdr.mType << endl << flush;
            break;
        }
        }
    }
public:
    SimulationStateOutput()
    {
        for (int i=0; i<256; i++)
            SimulationStateOutput::mAction[i] = Default;
        mAction[ED_CLASS::Control]          = Control;
        mAction[ED_CLASS::DynamicVariables] = DynamicVariables;
        mAction[ED_CLASS::Energy]           = Energy;
        mAction[ED_CLASS::Information]      = Information;
        mAction[ED_CLASS::UDF]              = UDF;
        mAction[ED_CLASS::RTPValues]        = RTPValues;
    }
    void process(ED_Header &Hdr, char *p, SimulationParser *sp) {
        mAction[Hdr.mClass](Hdr, p, sp);
    }
    static SimulationState *getInstance() { return &mSimulationState; }
};


#endif
