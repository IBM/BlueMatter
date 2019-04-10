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
 #ifndef DATARECEIVERSIMPLELOGGER_HPP
#define DATARECEIVERSIMPLELOGGER_HPP

#include <assert.h>
#include <fcntl.h>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
#include <BlueMatter/bootstrap.hpp>

#include <vector>

#define NFORM " %#14.12g"
#define SFORM " %14s"

class DataReceiverSimpleLogger : public DataReceiver
{
public:
    int mUDFColumn[UDF_Binding::UDF_CODE_COUNT];
    int mPressureColumn;
    int mUDFsKnown;
    double mEwaldSum;
    double mLeknerSum;
    double mCQ;
    double mCQSumSq;
    double mCQSum;
    double mCQMean;
    double mCQVar;
    int mLogCount;
    int mDelayStats;
    int mOutputPeriod;
    int mBootStrap;
    ArrayGrowable<double> mCQList;
    char mHeaderBuffer[4096];
    char mValuesBuffer[4096];
    int mSaveAllSnapshots;
    char mSnapshotNameStem[512];

    double ConservedQuantityRMSD()
    {
        if (mLogCount > mDelayStats) {
            double invN = 1.0/(mLogCount - mDelayStats);
            mCQMean = mCQSum * invN;
            mCQVar = mCQSumSq * invN - mCQMean * mCQMean;
	    // Gotta clamp due to roundoff error
	    if (mCQVar < 0)
		mCQVar = 0;
            return sqrt(mCQVar);
        }
        return 0;
    }

    inline int InHeader(const int code)
    {
        switch(code) {
        case UDF_Binding::KineticEnergy_Code:
        case UDF_Binding::WaterRigid3SiteShake_Code:
        case UDF_Binding::WaterRigid3SiteRattle_Code:
        case UDF_Binding::WaterSPCShake_Code:
        case UDF_Binding::WaterSPCRattle_Code:
        case UDF_Binding::WaterTIP3PShake_Code:
        case UDF_Binding::WaterTIP3PRattle_Code:
            return 0;
            break;
        default: return 1;
        }
        return 0;
    }

    inline static int IncludeValue(const int code)
    {
        switch(code) {
        case UDF_Binding::KineticEnergy_Code:
        case UDF_Binding::WaterRigid3SiteShake_Code:
        case UDF_Binding::WaterRigid3SiteRattle_Code:
        case UDF_Binding::WaterSPCShake_Code:
        case UDF_Binding::WaterSPCRattle_Code:
        case UDF_Binding::WaterTIP3PShake_Code:
        case UDF_Binding::WaterTIP3PRattle_Code:
            return 0;
            break;
        default: return 1;
        }
        return 0;
    }

protected:

  int SaveAllSnapshots() const {return mSaveAllSnapshots;}

public:
    DataReceiverSimpleLogger(int DelayStats = 0, int OutputPeriod = 1, int BootStrap = 0, int AllSnapshots = 0)
    {
        mUDFsKnown = 0;
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
    }

    void SetSnapshotName(char *sname)
    {
	strcpy(mSnapshotNameStem, sname);
    }

    static char *HeaderStringFromUDFCode( int code )
    {
        switch (code) {
        case UDF_Binding::DefaultForce_Code:              return "Default";         break;
        case UDF_Binding::StdHarmonicBondForce_Code:      return "StdHarmBond";     break;
        case UDF_Binding::StdHarmonicAngleForce_Code:     return "StdHarmAngle";    break;
        case UDF_Binding::SwopeTorsionForce_Code:         return "SwopeTorsion";    break;
        case UDF_Binding::UreyBradleyForce_Code:          return "UreyBradley";     break;
        case UDF_Binding::OPLSTorsionForce_Code:          return "OPLSTorsion";     break;
        case UDF_Binding::OPLSImproperForce_Code:         return "OPLSImproper";    break;
        case UDF_Binding::ImproperDihedralForce_Code:     return "ImpDihedral";     break;
        case UDF_Binding::NSQLennardJones_Code:           return "NSQLJ";           break;
        case UDF_Binding::LennardJonesForce_Code:         return "LJ";              break;
        case UDF_Binding::NSQCoulomb_Code:                return "NSQCoulomb";      break;
        case UDF_Binding::StdChargeForce_Code:            return "Coulomb";         break;
        case UDF_Binding::NSQEwaldRealSpace_Code:         return "NSQEwaldR";       break;
        case UDF_Binding::EwaldRealSpaceForce_Code:       return "EwaldReal";       break;
        case UDF_Binding::EwaldKSpaceForce_Code:          return "EwaldK";          break;
        case UDF_Binding::EwaldSelfEnergyForce_Code:      return "EwaldSelf";       break;
        case UDF_Binding::NSQEwaldCorrection_Code:        return "NSQEwaldCorr";    break;
        case UDF_Binding::NSQEwaldRespaCorrection_Code:   return "NSQEwaldRespaCorr";break;
        case UDF_Binding::EwaldCorrectionForce_Code:      return "EwaldCorr";       break;
        case UDF_Binding::TIP3PForce_Code:                return "TIP3P";           break;
        case UDF_Binding::SPCForce_Code:                  return "SPC";             break;
        case UDF_Binding::KineticEnergy_Code:             return "KE";              break;
        case UDF_Binding::Coulomb14_Code:                 return "Coulomb14";       break;
        case UDF_Binding::COMKineticEnergy_Code:          return "COMKE";           break;
        case UDF_Binding::ResampleEnergyLoss_Code:        return "ELoss";           break;
        case UDF_Binding::LennardJones14Force_Code:       return "LJ14";            break;
        case UDF_Binding::OPLSLennardJones14Force_Code:   return "OPLSLJ14";        break;
        case UDF_Binding::NSQOPLSLennardJones_Code:       return "NSQOPLSLJ";       break;
        case UDF_Binding::NSQLekner_Code:                 return "NSQLekner";       break;
        case UDF_Binding::NSQLeknerCorrection_Code:       return "NSQLekCorr";      break;
        case UDF_Binding::LeknerSelfEnergy_Code:          return "LekSelf";         break;
        case UDF_Binding::NoseHooverEnergy_Code:          return "NoseHooverE";     break;
        case UDF_Binding::COMRestraint_Code:              return "COMRestraint";    break;
        default:                                          return "UnknownUDF";      break;
        }
    }

    void ReportTotalEnergy( Frame *f )
    {
        tEnergyInfo *ei = &f->mEnergyInfo;
        char buff[1024];
        char buff2[128];
        sprintf(buff, "%7d %#14.12g %#14.12g %#14.12g %#14.12g %#14.12g %#14.12g %#14.12g",
            ei->mEnergySumAccumulator.mFullTimeStep,
            ei->mEnergySums.mTotalE,
            ei->mEnergySums.mIntraE,
            ei->mEnergySums.mInterE,
            ei->mEnergySumAccumulator.mKineticEnergy,
            ei->mEnergySums.mTemp,
            ei->mEnergySums.mConservedQuantity,
            ConservedQuantityRMSD());

        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (f->mUDFs[i].isComplete() && IncludeValue(i)) {
                sprintf(buff2, NFORM, f->mUDFs[i].getSum());
                strcat(buff, buff2);
                if (i == UDF_Binding::EwaldKSpaceForce_Code) {
                    sprintf(buff2, NFORM, mEwaldSum);
                    strcat(buff, buff2);
                }
                if (i == UDF_Binding::NSQLekner_Code) {
                    sprintf(buff2, NFORM, mLeknerSum);
                    strcat(buff, buff2);
                }
            }
        }

        if (f->mPressureStatus & ArrivalStatus::RECEIVED) {
            sprintf(buff2, " %#14.12g %#14.12g %#14.12g %#14.12g",
                f->mEnergyInfo.mPressureInfo.mPressure,
                f->mEnergyInfo.mPressureInfo.mVirial,
                f->mEnergyInfo.mPressureInfo.mVolume,
                f->mEnergyInfo.mPressureInfo.mVolumeVelocity);
            strcat(buff, buff2);
        }

        strcpy(mValuesBuffer, buff);

        cout << buff << endl << flush;


        int BadData = 0;

        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (!finite(f->mUDFs[i].getSum())) {
	      BadData = 1;
            }
        }

        if (f->mPressureStatus & ArrivalStatus::RECEIVED) {
	    if (
		!finite(f->mEnergyInfo.mPressureInfo.mPressure) ||
		!finite(f->mEnergyInfo.mPressureInfo.mVirial) ||
		!finite(f->mEnergyInfo.mPressureInfo.mVolume) ||
		!finite(f->mEnergyInfo.mPressureInfo.mVolumeVelocity)
	       )
		   BadData = 1;
        }

	if (BadData) {
            cerr << "Bad data reading stream in DataReceiverSimpleLogger" << endl;
	    mDone = 1;
	    mFinalStatus = 1;
	}
    };
    
    virtual void sites(Frame *f)
    {
        char fname[512];
        char p[MAXPACKEDPACKETSIZE];

        unsigned CurrentStep = f->mOuterTimeStep;

        if (mSaveAllSnapshots) {
	    //sprintf(fname, "Snapshot%08d.dvs", CurrentStep);
	    sprintf(fname, "%s%08d.dvs", mSnapshotNameStem, CurrentStep);
	} else {
	    //strcpy(fname, "Snapshot.dvs");
	    sprintf(fname, "%s.dvs", mSnapshotNameStem);
	    // If file exists, delete it
	    // This is a simple way to delete the last snapshot
	    if (access(fname, 0) != -1) {
		unlink(fname);
	    }
	}
        
        int fd = open(fname, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        assert(fd >= 0);

        int NumToWrite = ED_ControlSyncId::PackedSize;

        ED_FillPacket_ControlSyncId( p, eInvariantMagicNumber, CurrentStep, eVariableMagicNumber, eInvariantMagicNumber, 0 );
        int NumWritten = write(fd, p, NumToWrite);
        assert(NumWritten == NumToWrite);

        if (f->mRTPStatus & ArrivalStatus::RECEIVED) {
            ED_FillPacket_InformationRTP(p,
                CurrentStep,
                f->mInformationRTP.mCoulombMethod,
                f->mInformationRTP.mInnerTimeStepInPicoSeconds,
                f->mInformationRTP.mNumberOfInnerTimeSteps,
                f->mInformationRTP.mOuterTimeStepInPicoSeconds,
                f->mInformationRTP.mNumberOfOuterTimeSteps,
                f->mInformationRTP.mNumberOfRESPALevels,
                f->mInformationRTP.mPressureControlPistonMass,
                f->mInformationRTP.mPressureControlTarget,
                f->mInformationRTP.mVelocityResampleTargetTemperature,
                f->mInformationRTP.mVelocityResamplePeriodInOTS,
                f->mInformationRTP.mEnsemble,
                f->mInformationRTP.mSnapshotPeriodInOTS,
                f->mInformationRTP.mVelocityResampleRandomSeed,
                f->mInformationRTP.mSourceID,
                f->mInformationRTP.mExecutableID,
                f->mInformationRTP.mNumberOfTwoChainThermostats,
                f->mInformationRTP.mJobID,
                f->mInformationRTP.mDynamicBoundingBoxMin,
                f->mInformationRTP.mDynamicBoundingBoxMax
            );
                NumToWrite = ED_InformationRTP::PackedSize;
                NumWritten = write(fd, p, NumToWrite);
                assert(NumWritten == NumToWrite);
        }

        if (f->mPressureStatus & ArrivalStatus::RECEIVED) {
            ED_FillPacket_ControlPressure(p,
                CurrentStep,
                f->mControlPressure.mVirial,
                f->mControlPressure.mVolumePosition,
                f->mControlPressure.mVolumeVelocity,
                f->mControlPressure.mVolumeAcceleration,
                f->mControlPressure.mInternalPressure
            );
                NumToWrite = ED_ControlPressure::PackedSize;
                NumWritten = write(fd, p, NumToWrite);
                assert(NumWritten == NumToWrite);
        }

        if (f->mRandomStateStatus & ArrivalStatus::RECEIVED) {
            ED_FillPacket_DynamicVariablesRandomState(p, f->mDynamicVariablesRandomState);
	    NumToWrite = ED_DynamicVariablesRandomState::PackedSize;
	    NumWritten = write(fd, p, NumToWrite);
	    assert(NumWritten == NumToWrite);
        }

        if (f->mBoundingBoxStatus & ArrivalStatus::RECEIVED) {
            ED_FillPacket_DynamicVariablesBoundingBox(p, f->mDynamicVariablesBoundingBox);
	    NumToWrite = ED_DynamicVariablesBoundingBox::PackedSize;
	    NumWritten = write(fd, p, NumToWrite);
	    assert(NumWritten == NumToWrite);
        }

	ED_FillPacket_UDFd1(p,
	    CurrentStep,
	    UDF_Binding::ResampleEnergyLoss_Code,
	    1,
	    f->mUDFs[UDF_Binding::ResampleEnergyLoss_Code].getSum()
	);
	NumToWrite = ED_UDFd1::PackedSize;
	NumWritten = write(fd, p, NumToWrite);
	assert(NumWritten == NumToWrite);

        
	int NThermos = f->mTwoChainFragmentThermostatList.getSize();
	if (NThermos) {
	    ED_DynamicVariablesTwoChainFragmentThermostat *t = f->mTwoChainFragmentThermostatList.getArray();

	    NumToWrite = ED_DynamicVariablesTwoChainFragmentThermostat::PackedSize;        
	    for (int i=0; i<NThermos; i++) {
		ED_FillPacket_DynamicVariablesTwoChainFragmentThermostat(p, CurrentStep, t->mFragmentNumber, t->mVoxelId, t->mNumberOfSitesPerChain,
		    t->mPosition00,
		    t->mPosition01,
		    t->mPosition02,
		    t->mPosition03,
		    t->mPosition04,
		    t->mVelocity00,
		    t->mVelocity01,
		    t->mVelocity02,
		    t->mVelocity03,
		    t->mVelocity04,
		    t->mPosition10,
		    t->mPosition11,
		    t->mPosition12,
		    t->mPosition13,
		    t->mPosition14,
		    t->mVelocity10,
		    t->mVelocity11,
		    t->mVelocity12,
		    t->mVelocity13,
		    t->mVelocity14
		);
		NumWritten = write(fd, p, NumToWrite);
		assert(NumWritten == NumToWrite);
		t++;
	    }
	    // if have thermos and pressure, must have piston thermo
            if (f->mPressureStatus & ArrivalStatus::RECEIVED) {
	        NumToWrite = ED_DynamicVariablesOneChainThermostat::PackedSize;        
		ED_DynamicVariablesOneChainThermostat *t = &f->mOneChainThermo;
		ED_FillPacket_DynamicVariablesOneChainThermostat(p, CurrentStep, t->mVoxelId, t->mNumberOfSitesPerChain,
		    t->mPosition0,
		    t->mPosition1,
		    t->mPosition2,
		    t->mPosition3,
		    t->mPosition4,
		    t->mVelocity0,
		    t->mVelocity1,
		    t->mVelocity2,
		    t->mVelocity3,
		    t->mVelocity4
		);
		NumWritten = write(fd, p, NumToWrite);
		assert(NumWritten == NumToWrite);
	    }
	}

        tSiteData *ps= f->mSiteData.getArray();
        int NSites = f->mSiteData.getSize();

        NumToWrite = ED_DynamicVariablesSite::PackedSize;        
        for (int i=0; i<NSites; i++) {
            // Using 0 for voxel id 
            ED_FillPacket_DynamicVariablesSite(p, CurrentStep, 0, i, ps->mPosition, ps->mVelocity);
            NumWritten = write(fd, p, NumToWrite);
            assert(NumWritten == NumToWrite);
            ps++;
        }
        close(fd);
        
        // If file exists, delete it
        // if (access(fname, 0) != -1)
        //     unlink(fname);

        DONEXT_1(sites, f);
    }


    void SetUDFColumns(Frame *f)
    {
        char buff[512];
        char buff2[128];
        sprintf(buff, "%7s %14s %14s %14s %14s %14s %14s %14s",
                "Step", "TotalE", "IntraE", "InterE", "KE", "Temp", "ConsQuant", "CQRMSDEst");

        int col = 1;

        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (InHeader(i))
                if (f->mUDFs[i].isComplete())
                    mUDFColumn[i] = col++;

        }

        mPressureColumn = col;

        int UnknownCount = 0;
        for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
            if (f->mUDFs[i].isComplete() && InHeader(i)) {
                sprintf(buff2, SFORM, HeaderStringFromUDFCode(i));
                if (strstr(buff2, "UnknownUDF"))
                    sprintf(buff2, "    UnknownUDF%d", UnknownCount++);
                strcat(buff, buff2);
                if (i == UDF_Binding::EwaldKSpaceForce_Code) {
                    sprintf(buff2, SFORM, "EwaldSum");
                    strcat(buff, buff2);
                }
                if (i == UDF_Binding::NSQLekner_Code) {
                    sprintf(buff2, SFORM, "LeknerSum");
                    strcat(buff, buff2);
                }
            }
        }

       if (f->mPressureStatus & ArrivalStatus::RECEIVED) {
            sprintf(buff2, "%15s%15s%15s%15s", "IntPressure", "Virial", "Volume", "VolVeloc");
            strcat(buff, buff2);
        }

       strcpy(mHeaderBuffer, buff);

       cout << buff << endl << flush;

    }

    
    void doLogStats(Frame *f)
    {
        if (!mUDFsKnown) {
            mUDFsKnown = 1;
            SetUDFColumns(f);
        }

        tEnergySumAccumulator *pesa = &f->mEnergyInfo.mEnergySumAccumulator;
        f->mEnergyInfo.mEnergySums.mIntraE = 0;
        f->mEnergyInfo.mEnergySums.mInterE = 0;
        mEwaldSum = 0;
	f->mEwaldSum = 0;
        mLeknerSum = 0;

        for (int code = 0; code < UDF_Binding::NUMENTRIES; code++) {

            if (f->mUDFs[code].isComplete()) {
                switch(code) {

                case UDF_Binding::StdHarmonicBondForce_Code:
                case UDF_Binding::StdHarmonicAngleForce_Code:
                case UDF_Binding::SwopeTorsionForce_Code:
                case UDF_Binding::UreyBradleyForce_Code:
                case UDF_Binding::OPLSTorsionForce_Code:
                case UDF_Binding::OPLSImproperForce_Code:
                case UDF_Binding::ImproperDihedralForce_Code:
                case UDF_Binding::TIP3PForce_Code:
                case UDF_Binding::SPCForce_Code:
                    f->mEnergyInfo.mEnergySums.mIntraE += f->mUDFs[code].getSum();
                    break;

                case UDF_Binding::LennardJonesForce_Code:
                case UDF_Binding::NSQLennardJones_Code:
                case UDF_Binding::StdChargeForce_Code:
                case UDF_Binding::Coulomb14_Code:
                case UDF_Binding::LennardJones14Force_Code:
                case UDF_Binding::OPLSLennardJones14Force_Code:
                case UDF_Binding::NSQOPLSLennardJones_Code:
                case UDF_Binding::NSQCoulomb_Code:
                case UDF_Binding::COMRestraint_Code:
                    f->mEnergyInfo.mEnergySums.mInterE += f->mUDFs[code].getSum();
                    break;

                case UDF_Binding::EwaldRealSpaceForce_Code:
                case UDF_Binding::NSQEwaldRealSpace_Code:
                case UDF_Binding::EwaldKSpaceForce_Code:
                case UDF_Binding::EwaldSelfEnergyForce_Code:
                case UDF_Binding::EwaldCorrectionForce_Code:
                case UDF_Binding::NSQEwaldRespaCorrection_Code:
                case UDF_Binding::NSQEwaldCorrection_Code:
                    mEwaldSum += f->mUDFs[code].getSum();
                    break;

                case UDF_Binding::NSQLeknerCorrection_Code:
                case UDF_Binding::NSQLekner_Code:
                case UDF_Binding::LeknerSelfEnergy_Code:
                    mLeknerSum += f->mUDFs[code].getSum();
                    break;

                case  UDF_Binding::KineticEnergy_Code:
                    pesa->mKineticEnergy = f->mUDFs[code].getSum();
                    break;

                case UDF_Binding::COMKineticEnergy_Code:
                    pesa->mCOMKineticEnergy = f->mUDFs[code].getSum();
                    break;

                case UDF_Binding::ResampleEnergyLoss_Code:
                    pesa->mResampleEnergyLoss = f->mUDFs[code].getSum();
                    break;

                case UDF_Binding::NoseHooverEnergy_Code:
                    pesa->mNoseHooverEnergy = f->mUDFs[code].getSum();
                    break;

                default:
                    if (f->mUDFs[code].getSum() != 0) {
                        cerr << "Unknown UDF with nonzero value:  Code, Value = " << code << " " << f->mUDFs[code].getSum() << endl;
                    }
                }
            }
        }

        f->mEnergyInfo.mEnergySums.mInterE += mEwaldSum;

	f->mEwaldSum = mEwaldSum;

        f->mEnergyInfo.mEnergySums.mInterE += mLeknerSum;

        f->mEnergyInfo.mEnergySums.mTotalE =
            f->mEnergyInfo.mEnergySums.mInterE + f->mEnergyInfo.mEnergySums.mIntraE + pesa->mKineticEnergy;
        
        // currently subtract 3 from number of degrees of freedom, assuming stationary center of mass.
        // thus need more than one site.
        // Later this will include number of constraints in rtp file, or direct from probspec
        if (f->mNSites > 1)
            f->mEnergyInfo.mEnergySums.mTemp = pesa->mKineticEnergy * 2.0 / ( f->mNumberDegreesOfFreedom )/SciConst::KBoltzmann_IU;
        else
            f->mEnergyInfo.mEnergySums.mTemp = 0;
        
        pesa->mFullTimeStep = f->mOuterTimeStep;
        
        // ReportTotalEnergy(f);

        if (f->mEnergyInfo.mPressureInfo.mDoPressure) {
	    pesa->mPistonKineticEnergy =
                0.5 * f->mPistonMass * f->mEnergyInfo.mPressureInfo.mVolumeVelocity * f->mEnergyInfo.mPressureInfo.mVolumeVelocity;
                // ( SciConst::Avagadro / SciConst::kcal2joule / 1.0E17 );
	    pesa->mPressureEnergy =
		f->mEnergyInfo.mPressureInfo.mVolume * (f->mExternalPressure) * SciConst::Atm_IU;
            f->mEnergyInfo.mEnergySums.mConservedQuantity = 
		  f->mEnergyInfo.mEnergySums.mInterE + 
                  f->mEnergyInfo.mEnergySums.mIntraE +
		  pesa->mKineticEnergy +
		  pesa->mPressureEnergy +
		  pesa->mPistonKineticEnergy +
		  pesa->mResampleEnergyLoss +
		  pesa->mNoseHooverEnergy;
	} else {
	    f->mEnergyInfo.mEnergySums.mConservedQuantity =
		f->mEnergyInfo.mEnergySums.mTotalE +
		pesa->mResampleEnergyLoss +
		pesa->mNoseHooverEnergy;
	}
#if 0
	cout << "PistKE " << f->mEnergyInfo.mEnergySumAccumulator.mPistonKineticEnergy << " PEnergy " <<
	f->mEnergyInfo.mEnergySumAccumulator.mPressureEnergy << " COMKE " << f->mEnergyInfo.mEnergySumAccumulator.mCOMKineticEnergy <<
	" EnergyLoss "  << f->mEnergyInfo.mEnergySumAccumulator.mResampleEnergyLoss <<  
            " ExtPress " << f->mExternalPressure << " VolVel " << f->mEnergyInfo.mPressureInfo.mVolumeVelocity << endl;
#endif
        mCQList.set(mLogCount, f->mEnergyInfo.mEnergySums.mConservedQuantity); 
        mLogCount++;
        if (mLogCount > mDelayStats) {
            mCQSum   += f->mEnergyInfo.mEnergySums.mConservedQuantity;
            mCQSumSq += f->mEnergyInfo.mEnergySums.mConservedQuantity * f->mEnergyInfo.mEnergySums.mConservedQuantity;
        }


        
    }

    virtual void logInfo(Frame *f)
    {
        doLogStats(f);
	if (mOutputPeriod < 2 || mLogCount % mOutputPeriod == 1)
            ReportTotalEnergy(f);
        DONEXT_1(logInfo, f);
    }

    virtual void newTwoChainFragmentThermostatPacket(ED_DynamicVariablesTwoChainFragmentThermostat &t)
    {
        DONEXT_1(newTwoChainFragmentThermostatPacket, t);
    }

    virtual void newOneChainThermostatPacket(ED_DynamicVariablesOneChainThermostat &t)
    {
        DONEXT_1(newOneChainThermostatPacket, t);
    }        

    virtual void final(int status=1)
    {
        double deltaSumSq    = 0;
        double CQVarVarSumSq = 0;
        double CQVarSumSq    = 0;
        double CQVarVar      = 0;
	double CQRMSDRMSD    = 0;

	mCQSum   = 0;
	mCQSumSq = 0;
	mCQVar   = 0;

        if (mLogCount > mDelayStats) {
	    int i;
            double invN = 1.0/(mLogCount - mDelayStats);
	    for (i=mDelayStats; i<mLogCount; i++) {
		mCQSum += mCQList[i];
	    }
	    mCQMean = mCQSum * invN;
	    for (i=mDelayStats; i<mLogCount; i++) {
		double delta = mCQList[i] - mCQMean;
		mCQSumSq += delta*delta;
	    }
	    mCQVar = mCQSumSq * invN;

            if (mBootStrap) {
	        // bootstrap estimation of uncertainty in ConsQRMSD
	        std::vector<double> cqData;
	        cqData.reserve(mLogCount - mDelayStats);
	        for (i=mDelayStats; i<mLogCount; i++) {
	            cqData.push_back(mCQList[i]);
	        }
	        Bootstrap bootstrap(cqData);
	        const std::vector<double> synthData = bootstrap.eval(1000);
	        double cqrmsd = sqrt(mCQVar);
	        double varSum = 0;
	        for (std::vector<double>::const_iterator iter = synthData.begin();
		     iter != synthData.end();
		     ++iter)
	          {
		    double delta = *iter - cqrmsd;
		    varSum = varSum + delta*delta;
	          }
	        CQRMSDRMSD = sqrt(varSum/synthData.size());
	    } else {
	        CQRMSDRMSD = -1;
	    }
        }

        const char fname[] = "Run.final";
        if (access(fname, 0) != -1)
            unlink(fname);
        
        FILE *file = fopen(fname, "w");
        assert( file != NULL );
        
        int k = 0;
        for (int i=mDelayStats; i<mLogCount; i++) {
            k++;

            double delta = mCQList[i] - mCQMean;
            double deltasq = delta * delta - mCQVar;
            deltaSumSq += deltasq * deltasq;
        }

        if (k < 1)
	    CQVarVar = 0;
	else
	    CQVarVar = deltaSumSq / k;

        fprintf(file, "%s %15s %15s %15s\n", mHeaderBuffer, "ConsQRMSD", "CQRMSDRMSD", "FinalStatus");

        fprintf(file, "%s %#15.13g %#15.13g %15d\n", mValuesBuffer, sqrt(mCQVar), CQRMSDRMSD, status);

        fclose(file);

	DONEXT_0(final);
    }
    
};

#endif
