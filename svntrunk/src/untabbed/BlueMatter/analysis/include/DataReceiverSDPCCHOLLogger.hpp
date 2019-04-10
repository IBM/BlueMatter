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
 #ifndef DATARECEIVERSOPELOGGER_HPP
#define DATARECEIVERSOPELOGGER_HPP

#include <assert.h>
#include <fcntl.h>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/DataReceiverSimpleLogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
#include <BlueMatter/bootstrap.hpp>

#include <BlueMatter/db2/molecularsystem.hpp>
#include <BlueMatter/DataReceiverMomentumMonitor.hpp>

#include <vector>
#include <iostream>

#define NFORM " %#14.12g"
#define SFORM " %14s"
#define MAX_HIST_SIZE 200

#define REIMAGEWATERS 1
#define CENTERONPHOSPHATES 1
class DataReceiverSDPCCHOLLogger : public DataReceiverSimpleLogger
{
private:

  char db_name[256];
  int sysId;
  double mXYscale;
  double mVave;
  double mdzave;
  int mNobs;
  int mWriteSelectedSnapshot;
  int mSelectedStep;

  db2::MolecularSystem *aMolSys;
  std::vector< std::vector<int> > mSHyds;
  std::vector< std::vector<int> > mDHyds;
  std::vector<int> mPhos;   // PCGL: P  OP3  OP4  OP1   OP2 
  std::vector<int> mPC;     // PCGL: N  C[4-8] H[5-8][ABC]  H[4-5]5[AB] 
  std::vector<int> mGly;    // PCGL: C3   H3[AB]   C2   H2  C1   H1[AB]
  std::vector<int> mCarb;   // PEGL: O1 O2 STEA: C1 O1 OLEO : C1 O1 
  std::vector<int> mSCH2;   // STEA: C2- C17 H2-17[AB]
  std::vector<int> mSCH3;   // STEA C18 H18[ABC]
  std::vector<int> mDCH2;   // DHA: C[2-3] C6 C9 C12 C15 C18 C21 H6[AB]  H9[AB]  H12[AB]  H15[AB]  H18[AB]  H21[AB] 
  std::vector<int> mOle;    // DHA: [HC][4,5,7,8,10,11,13,14,16,17,19,20] 
  std::vector<int> mDCH3;   // DHA C22 H22[ABC]
  std::vector<int> mChol18;   // CHOL [CH]18[ ABC]
  std::vector<int> mH2O;    // HOH
  std::vector<int> mH2O_O;  // oxygens for HOH.  Used for water reimaging
  std::vector< std::vector<int> > mAtch;
  std::vector< std::vector<int> > mSChainHyds;
  std::vector< std::vector<int> > mDChainHyds;

  int mNZSlabs;
  double mSlabSpacing;

  std::vector<int> mPhosZCnts;
  std::vector<int> mPCZCnts;
  std::vector<int> mGlyZCnts;
  std::vector<int> mCarbZCnts;
  std::vector<int> mSCH2ZCnts;
  std::vector<int> mDCH2ZCnts;
  std::vector<int> mOleZCnts;
  std::vector<int> mSCH3ZCnts;
  std::vector<int> mDCH3ZCnts;
  std::vector<int> mChol18ZCnts;
  std::vector<int> mH2OZCnts;

  std::vector<db2::Site> mSite;

  double mSysMass;
  XYZ mCOM;
  XYZ mCOMVel;

  FILE *mDLipOut;
  FILE *mSLipOut;

  FILE *SCHVecsPF[20];
  FILE *DCHVecsPF[24];

   virtual void init()
    {
    }



public:
  DataReceiverSDPCCHOLLogger(int DelayStats = 0, int OutputPeriod = 1, int BootStrap = 0, int AllSnapshots = 0, char *db_name = NULL, int sysid = -1, double xyscale = 1.0, int selected_step = 0) : DataReceiverSimpleLogger(DelayStats, OutputPeriod, BootStrap, AllSnapshots)
    {
      int i;
  strcpy(db_name,db_name);
  sysId = sysid;
  assert(sysId > 0);

  if(selected_step > 0)
    {
      mWriteSelectedSnapshot = 1;
      mSelectedStep = selected_step;
    }
  else
    {
      mWriteSelectedSnapshot = 0;
    }

  aMolSys = db2::MolecularSystem::constructFromDB2(sysId,db_name);
  assert(aMolSys != NULL);


  mSite = aMolSys->sites().asVector();

  mXYscale = xyscale;

  char cmd[256],cmdbuf[1000];  
  for( i = 3; i<=17;i++)
    {
      sprintf(cmd,"component_name = 'STEA' and (atom_tag ='H%dA' or atom_tag = 'H%dB')",i,i);
      mSHyds.push_back(aMolSys->SelectedSites(cmd));
    }
  for( i = 3; i<=21;i++)
    {
      sprintf(cmd,"component_name = 'DHA' and (atom_tag = 'H%d' or atom_tag ='H%dA' or atom_tag = 'H%dB')",i,i,i);
      mDHyds.push_back(aMolSys->SelectedSites(cmd));
    }

  //  // Collect phosphates
  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'PCGL' and (atom_tag ='P' or atom_tag = 'OP1' or atom_tag = 'OP2' or atom_tag = 'OP3' or atom_tag = 'OP4')");
  strcat(cmdbuf,cmd);
  mPhos = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("Phos.ids","Phos",mPhos);

  // Collect cholines
  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'PCGL' and (atom_tag ='N' ");
  strcat(cmdbuf,cmd);
  for(i=4; i<=8;i++)
    {
      sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB'  or atom_tag = 'H%dC' ",i,i,i,i);
      strcat(cmdbuf,cmd);
    }
  strcat(cmdbuf,")");
  mPC = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("PC.ids","PC",mPC);

  // Collect glycols
  cmdbuf[0] = '\0'; 
  sprintf(cmd,"component_name = 'PCGL' and ( atom_tag = 'C' ");
  strcat(cmdbuf,cmd);
  for(i=1; i<=3;i++)
    {
      sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB'  ",i,i,i);
      strcat(cmdbuf,cmd);
    }
  strcat(cmdbuf,")");
  mGly = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("Gly.ids","Gly",mGly);

  // Collect carbonyls
  cmdbuf[0] = '\0';
  sprintf(cmd,"( ( component_name = 'PCGL' and (atom_tag ='O1' or atom_tag = 'O2') ) or  ");
  strcat(cmdbuf,cmd);
  sprintf(cmd," ( (component_name = 'DHA' or component_name = 'STEA') and (atom_tag ='C1' or atom_tag = 'O1') ) )  ");
  strcat(cmdbuf,cmd);
  mCarb = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("Carb.ids","Carb",mCarb);


  // Collect CH2s 
  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'DHA' and ( atom_tag = 'C' ");
  strcat(cmdbuf,cmd);
  {
          i = 2;
    sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB'  or atom_tag = 'H%d'  ",i,i,i,i);
    strcat(cmdbuf,cmd);
  }
  for(i=3; i<=21;i+=3)
    {
      sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB'  or atom_tag = 'H%d'  ",i,i,i,i);
      strcat(cmdbuf,cmd);
    }
  strcat(cmdbuf,")");
  mDCH2 = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("DHA_CH2.ids","DHA_CH2",mDCH2);

  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'STEA' and ( atom_tag = 'C' ");
  strcat(cmdbuf,cmd);
  for(i=2; i<=17;i++)
    {
      sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB'  ",i,i,i);
      strcat(cmdbuf,cmd);
    }
  strcat(cmdbuf,")");
  mSCH2 = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("STEA_CH2.ids","STEA_CH2",mSCH2);

  // Collect Olefins
  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'DHA' and ( atom_tag = 'C' ");
  strcat(cmdbuf,cmd);
  for(i=4; i<=19;i+=3)
    {
      sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%d'   ",i,i);
      strcat(cmdbuf,cmd);
      sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%d'   ",i+1,i+1);
      strcat(cmdbuf,cmd);
    }
  strcat(cmdbuf,")");
  mOle = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("Ole.ids","Ole",mOle);

  // Collect Methyls
  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'DHA' and ( atom_tag = 'C22' or atom_tag = 'H22A' or atom_tag = 'H22B' or atom_tag = 'H22C' )  ");
  strcat(cmdbuf,cmd);
  mDCH3 = aMolSys->SelectedSites(cmdbuf);
  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'STEA' and ( atom_tag = 'C18' or atom_tag = 'H18A' or atom_tag = 'H18B' or atom_tag = 'H18C' )  ");
  strcat(cmdbuf,cmd);
  mSCH3 = aMolSys->SelectedSites(cmdbuf);
  OutputIdList("STEA_CH3.ids","STEA_CH3",mSCH3);

  // Collect Waters
  cmdbuf[0] = '\0';
  sprintf(cmd,"component_name = 'HOH'");
  mH2O = aMolSys->SelectedSites(cmd);
  OutputIdList("H2O.ids","H2O",mH2O);

  // Collect Water oxygens
  cmdbuf[0] = '\0';
  sprintf(cmd,"(component_name = 'HOH' and element_symbol = 'O')");
  mH2O_O = aMolSys->SelectedSites(cmd);


  // Collect Chain Hyds
  for(i=2; i<=22;i++)
    {
      sprintf(cmdbuf," component_name = 'DHA'  and ");
      sprintf(cmd,"( atom_tag = 'H%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB' or atom_tag = 'H%dC' )",i,i,i,i);
      strcat(cmdbuf,cmd);	    
      std::vector< int > Dhyds = aMolSys->SelectedSites(cmdbuf);
      //	    printf("Carbon # %d nHyds = %d\n",i,hyds.size());
      mDChainHyds.push_back( Dhyds );
    }

  for(i=2; i<=18;i++)
    {
      sprintf(cmdbuf," component_name = 'STEA'  and ");
      sprintf(cmd,"( atom_tag = 'H%dA' or atom_tag = 'H%dB' or atom_tag = 'H%dC' )",i,i,i);
      strcat(cmdbuf,cmd);	    
      std::vector< int > Shyds = aMolSys->SelectedSites(cmdbuf);
      //	    printf("Carbon # %d nHyds = %d\n",i,hyds.size());
      mSChainHyds.push_back( Shyds );
    }

  // Collect Cholesterol [HC]18
  i=18;
    {
      sprintf(cmdbuf," component_name = 'CHOL'  and ");
      sprintf(cmd,"( atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB' or atom_tag = 'H%dC' )",i,i,i,i);
      strcat(cmdbuf,cmd);	    
      mChol18 = aMolSys->SelectedSites(cmdbuf);
    }

  const std::vector< std::vector<int>  > &atch = aMolSys->bonds().asAdjList();
  mAtch = atch;
  mSLipOut = fopen("STEA_LipidOrderParam.dat","w");
  mDLipOut = fopen("DHA_LipidOrderParam.dat","w");

  for(i=0;i<20;i++)
    { SCHVecsPF[i] = NULL; }
  for(i=2;i<=18;i++)
    { 
      char bufS[256];
      sprintf(bufS,"STEA_CHvec_%d.dat",i);
      SCHVecsPF[i] = fopen(bufS,"w"); 
    }
  for(i=0;i<24;i++)
    { DCHVecsPF[i] = NULL; }
  for(i=2;i<=22;i++)
    { 
      char bufS[256];
      sprintf(bufS,"DHA_CHvec_%d.dat",i);
      DCHVecsPF[i] = fopen(bufS,"w"); 
    }

  mNZSlabs = MAX_HIST_SIZE;
  mSlabSpacing = 0.5;

  mPhosZCnts.reserve(MAX_HIST_SIZE+1);
  mPCZCnts.reserve(MAX_HIST_SIZE+1);
  mGlyZCnts.reserve(MAX_HIST_SIZE+1);
  mCarbZCnts.reserve(MAX_HIST_SIZE+1);
  mSCH2ZCnts.reserve(MAX_HIST_SIZE+1);
  mDCH2ZCnts.reserve(MAX_HIST_SIZE+1);
  mSCH3ZCnts.reserve(MAX_HIST_SIZE+1);
  mDCH3ZCnts.reserve(MAX_HIST_SIZE+1);
  mOleZCnts.reserve(MAX_HIST_SIZE+1);
  mChol18ZCnts.reserve(MAX_HIST_SIZE+1);
  mH2OZCnts.reserve(MAX_HIST_SIZE+1);
  for (i=0;i<=mNZSlabs;i++)
    {
      mPhosZCnts[i] = 0;
      mPCZCnts[i] = 0;
      mGlyZCnts[i] = 0;
      mCarbZCnts[i] = 0;
      mSCH2ZCnts[i] = 0;
      mDCH2ZCnts[i] = 0;
      mOleZCnts[i] = 0;
      mDCH3ZCnts[i] = 0;
      mSCH3ZCnts[i] = 0;
      mChol18ZCnts[i] = 0;
      mH2OZCnts[i] = 0;
    }
  
  mVave = mdzave = 0.0;
  mNobs = 0;


    }
  ~DataReceiverSDPCCHOLLogger()
  {
    fclose(mSLipOut);
    fclose(mDLipOut);
    for(int i = 2;i<=18;i++)
      fclose(SCHVecsPF[i]);
    for(int i = 2;i<=22;i++)
      fclose(DCHVecsPF[i]);
  }

    virtual void sites(Frame *f)
    {
        char fname[512];
        char p[MAXPACKEDPACKETSIZE];
  XYZ min,max;
  double aspectZperX;

        unsigned CurrentStep = f->mOuterTimeStep;
        int NSites = f->mSiteData.getSize();

  if(mWriteSelectedSnapshot && (CurrentStep != mSelectedStep))
           return;

        if (SaveAllSnapshots()) {
      sprintf(fname, "SDPCCHOLLipSnapshot%08d.dvs", CurrentStep);
  } else {
      strcpy(fname, "SDPCCHOLSnapshot.dvs");
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




        if (f->mRTPStatus & ArrivalStatus::RECEIVED) 
    {
      UpdateBoundingBox(f,min,max);
      aspectZperX = (max.mZ - min.mZ) / (max.mX - min.mX);
    }
  else
    {
      mXYscale = 1.0;
    }
      

        if (f->mRTPStatus & ArrivalStatus::RECEIVED) 
    {
      
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
           f->mInformationRTP.mdummy3,
           min,  //f->mInformationRTP.mDynamicBoundingBoxMin,
           max //f->mInformationRTP.mDynamicBoundingBoxMax
           );
      NumToWrite = ED_InformationRTP::PackedSize;
      NumWritten = write(fd, p, NumToWrite);
      assert(NumWritten == NumToWrite);
    }
  
        if (f->mPressureStatus & ArrivalStatus::RECEIVED) 
    {
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
  
  ED_FillPacket_UDFd1(p,
          CurrentStep,
          UDF_Binding::ResampleEnergyLoss_Code,
          1,
          f->mUDFs[UDF_Binding::ResampleEnergyLoss_Code].getSum()
          );
  NumToWrite = ED_UDFd1::PackedSize;
  NumWritten = write(fd, p, NumToWrite);
  assert(NumWritten == NumToWrite);
  

  GetSysCOM(f);
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
  std::vector< XYZ > coords(NSites);

        NumToWrite = ED_DynamicVariablesSite::PackedSize;        
        for (int i=0; i<NSites; i++) 
    {
      coords[i]  = ps->mPosition - mCOM;
      if(mXYscale < 1.0)
        {
    coords[i].mZ /= mXYscale;
        }
      else
        {
    coords[i].mX *= mXYscale;
    coords[i].mY *= mXYscale;
        }
            ps++;
    }


  if(CENTERONPHOSPHATES)
    {
      XYZ cen;
      CenterOfIdList(coords,mPhos,cen);
      for (int i=0; i<NSites; i++) 
        {
    coords[i]  -= cen;
        }
    }

  if(REIMAGEWATERS)
    ReimageWaters(coords,mH2O_O,min,max);


  ps= f->mSiteData.getArray();
        for (int i=0; i<NSites; i++) 
    {
      // Using 0 for voxel id 	    
      XYZ fixVel = ps->mVelocity - mCOMVel;
            ED_FillPacket_DynamicVariablesSite(p, CurrentStep, 0, i, coords[i], fixVel);
            NumWritten = write(fd, p, NumToWrite);
            assert(NumWritten == NumToWrite);
            ps++;
    }
  
  UpdateCHVecs(coords);
  UpdateScd(coords);

  UpdateHist(coords,mPhos,mPhosZCnts,min,max);
  UpdateHist(coords,mPC,mPCZCnts,min,max);
  UpdateHist(coords,mGly,mGlyZCnts,min,max);
  UpdateHist(coords,mCarb,mCarbZCnts,min,max);
  UpdateHist(coords,mSCH2,mSCH2ZCnts,min,max);
  UpdateHist(coords,mDCH2,mDCH2ZCnts,min,max);
  UpdateHist(coords,mSCH3,mSCH3ZCnts,min,max);
  UpdateHist(coords,mDCH3,mDCH3ZCnts,min,max);
  UpdateHist(coords,mOle,mOleZCnts,min,max);
  UpdateHist(coords,mChol18,mChol18ZCnts,min,max);
  UpdateHist(coords,mH2O,mH2OZCnts,min,max);


        close(fd);
        
        // If file exists, delete it
        // if (access(fname, 0) != -1)
        //     unlink(fname);


  OutputHist("Phos_Hist.dat",mPhosZCnts,min,max);
  OutputHist("PC_Hist.dat",mPCZCnts,min,max);
  OutputHist("Gly_Hist.dat",mGlyZCnts,min,max);
  OutputHist("Carb_Hist.dat",mCarbZCnts,min,max);
  OutputHist("STEA_CH2_Hist.dat",mSCH2ZCnts,min,max);
  OutputHist("DHA_CH2_Hist.dat",mDCH2ZCnts,min,max);
  OutputHist("Ole_Hist.dat",mOleZCnts,min,max);
  OutputHist("STEA_CH3_Hist.dat",mSCH3ZCnts,min,max);
  OutputHist("DHA_CH3_Hist.dat",mDCH3ZCnts,min,max);
  OutputHist("CHOL18_Hist.dat",mChol18ZCnts,min,max);
  OutputHist("H2O_Hist.dat",mH2OZCnts,min,max);

        DONEXT_1(sites, f);
    }
  
  void UpdateBoundingBox(Frame *f, XYZ &min, XYZ &max )
  {
    min = f->mInformationRTP.mDynamicBoundingBoxMin;
    max = f->mInformationRTP.mDynamicBoundingBoxMax;
    
    if(mXYscale < 1.0)
      {
  min.mZ /= mXYscale;
  max.mZ /= mXYscale;
      }
    else
      {
  min.mX *= mXYscale;
  min.mY *= mXYscale;
  max.mX *= mXYscale;
  max.mY *= mXYscale;
      }
    double dx = max.mX - min.mX;
    double dy = max.mY - min.mY;
    double dz = max.mZ - min.mZ;
    
    mVave += dx*dy*dz;
    mdzave += dz;
    mNobs++;
    
    min.mX = -0.5*dx;	max.mX = 0.5*dx;
    min.mY = -0.5*dy;	max.mY = 0.5*dy;
    min.mZ = -0.5*dz;	max.mZ = 0.5*dz;
    return ;
  }
  void GetSysCOM(Frame *f )
  {

    mSysMass = 0;
    mCOM.Zero();
    mCOMVel.Zero();
    for (int i = 0; i < f->mSiteData.getSize(); ++i)
      {
  mSysMass += mSite[i].atomicMass();
  mCOM += f->mSiteData[i].mPosition * mSite[i].atomicMass();
  mCOMVel += f->mSiteData[i].mVelocity;
  //	    pTot += f->mSiteData[i].mVelocity * mSite[i].atomicMass();
      }
    
    mCOM = mCOM / mSysMass;
    mCOMVel = mCOMVel / ((double) f->mSiteData.getSize());
    return;
  }

  void UpdateCHVecs(std::vector< XYZ> &crds)
  {
    for (int i = 2; i<=18; i++)
      {
  for(std::vector<int>::const_iterator iter = mSChainHyds[i-2].begin();
      iter != mSChainHyds[i-2].end();
      ++iter)
    {
      XYZ CHvec = crds[*iter] - crds[mAtch[*iter][0]];
      double len = CHvec.Length();
      CHvec = CHvec / len;
      fprintf(SCHVecsPF[i]," %10.7f %10.7f %10.7f    ", CHvec.mX,CHvec.mY,CHvec.mZ);
    }
  fprintf(SCHVecsPF[i],"\n");
      }
    for (int i = 2; i<=22; i++)
      {
  for(std::vector<int>::const_iterator iter = mDChainHyds[i-2].begin();
      iter != mDChainHyds[i-2].end();
      ++iter)
    {
      XYZ CHvec = crds[*iter] - crds[mAtch[*iter][0]];
      double len = CHvec.Length();
      CHvec = CHvec / len;
      fprintf(DCHVecsPF[i]," %10.7f %10.7f %10.7f    ", CHvec.mX,CHvec.mY,CHvec.mZ);
    }
  fprintf(DCHVecsPF[i],"\n");
      }
  }

  void UpdateScd(std::vector< XYZ> &crds)
  {

    int Dn = mDHyds.size();
    std::vector<double> DSCD(Dn);
    int Sn = mSHyds.size();
    std::vector<double> SSCD(Sn);
    
    fprintf(mSLipOut,"   SCD_3_17  ");
    fprintf(mDLipOut,"   SCD_3_21  ");

    for(int i = 0;i< Dn;i++)
      { 
  DSCD[i] = 0.0; 
  int j = 0;
  for (std::vector<int>::const_iterator iter = mDHyds[i].begin();
       iter != mDHyds[i].end();
       j++,++iter)
    {
      XYZ CHvec = crds[*iter] - crds[mAtch[*iter][0]];
      double len = CHvec.Length();
      double cosCHZ = CHvec.mZ / len;  // equilvalent to cos(CHvec with (0,0,1))
      double CHZOrderParam = 1.5*cosCHZ*cosCHZ - 0.5;
      DSCD[i] += CHZOrderParam;
      if (0)
        {
    printf("\tC[%f,%f,%f] \tH[%f,%f,%f] \tCH[%f,%f,%f]\n",
           crds[mAtch[*iter][0]].mX,crds[mAtch[*iter][0]].mY,crds[mAtch[*iter][0]].mZ,
           crds[*iter].mX,crds[*iter].mY,crds[*iter].mZ,
           CHvec.mX,CHvec.mY,CHvec.mZ);
    printf("%d: len %f cosCHZ %f  order param = %f\n",i,len,cosCHZ,CHZOrderParam);
        }
    }
  DSCD[i] /= j;
  fprintf(mDLipOut,"\t%f", DSCD[i]);

      }
    for(int i = 0;i< Sn;i++)
      { 
  SSCD[i] = 0.0; 
  int j = 0;
  for (std::vector<int>::const_iterator iter = mSHyds[i].begin();
       iter != mSHyds[i].end();
       j++,++iter)
    {
      XYZ CHvec = crds[*iter] - crds[mAtch[*iter][0]];
      double len = CHvec.Length();
      double cosCHZ = CHvec.mZ / len;  // equilvalent to cos(CHvec with (0,0,1))
      double CHZOrderParam = 1.5*cosCHZ*cosCHZ - 0.5;
      SSCD[i] += CHZOrderParam;
      if (0)
        {
    printf("\tC[%f,%f,%f] \tH[%f,%f,%f] \tCH[%f,%f,%f]\n",
           crds[mAtch[*iter][0]].mX,crds[mAtch[*iter][0]].mY,crds[mAtch[*iter][0]].mZ,
           crds[*iter].mX,crds[*iter].mY,crds[*iter].mZ,
           CHvec.mX,CHvec.mY,CHvec.mZ);
    printf("%d: len %f cosCHZ %f  order param = %f\n",i,len,cosCHZ,CHZOrderParam);
        }
    }
  SSCD[i] /= j;
  fprintf(mSLipOut,"\t%f", SSCD[i]);
      }


    fprintf(mDLipOut,"\n");
    fflush(mDLipOut);
    fprintf(mSLipOut,"\n");
    fflush(mSLipOut);
  }
  
  void CenterOfIdList(std::vector < XYZ > & crds, std::vector< int > &ids, XYZ & cen)
  {
    int cnt = 0;
    cen.Zero();
    for (std::vector<int>::const_iterator iter = ids.begin();
   iter != ids.end();
   ++iter)
      {
  XYZ & crd = crds[*iter];
  cen += crd;
  cnt++;
      }
    cen = cen / cnt;
    return;
  }

  

  void UpdateHist(std::vector < XYZ > & crds, std::vector< int > &ids, std::vector< int > &hist,XYZ &min, XYZ & max)
  {
    for (std::vector<int>::const_iterator iter = ids.begin();
   iter != ids.end();
   ++iter)
      {
  double dz = max.mZ - min.mZ;
  XYZ & crd = crds[*iter];
  while (( - 0.5 * dz) > crd.mZ) crd.mZ += dz;
  while ( ( 0.5 * dz) < crd.mZ) crd.mZ -= dz;
  double Zoff = 0.5 * mNZSlabs * mSlabSpacing; 
  int idx = (crd.mZ + Zoff) / mSlabSpacing;
  if(idx <0) idx = 0;
  if(idx >= mNZSlabs) idx = mNZSlabs - 1;
  hist[idx]++;
      }

  }

    
  void OutputHist(char *fname,std::vector<int> & his, XYZ &min, XYZ &max )
  {
    FILE *fPF = fopen(fname,"w");
    int i;
    double V = mVave / mNobs;;
    double dz = mdzave / mNobs;;
    double dx = sqrt(V/dz);;
    double dy = dx;



    double slabV = dx*dy*mSlabSpacing;

    double total = 0.0;

    for( i = 0;i< mNZSlabs; i++)
      { total += his[i]; }
    
    fprintf(fPF,"# V\t Vs\t aspect\t Z\t A\t totalN\n");
    fprintf(fPF,"# %10f \t %10f \t %10.4f \t %10.4f \t %10f \t %d\n", V, slabV, dz/dx , dz , dx*dy, (int) total);

    for(i=0;i<mNZSlabs;i++)
      {
  double Z = i*mSlabSpacing - 0.5 * mNZSlabs * mSlabSpacing;
  double ndens = his[i] / total / slabV;
  fprintf(fPF,"%10f %15.7g\n",Z,ndens);
      }
    fprintf(fPF,"\n");

    fclose(fPF);
  }


  void OutputIdList ( char *fname, char * collabel, std::vector<int> & ids)
  {
    int n = ids.size();
    FILE *fPF = fopen(fname,"w");
    fprintf(fPF,"#%s\n",collabel);
    for(int i = 0; i< n; i++)
      {
  fprintf(fPF,"%d\n",ids[i]);
      }
    fclose(fPF);
    return;
  }

  void ReimageWaters(std::vector < XYZ > &crds, std::vector< int > &ids, XYZ &min, XYZ &max)
  {
    double dx = max.mX - min.mX;
    double dy = max.mY - min.mY;
    double dz = max.mZ - min.mZ;

    for (std::vector<int>::const_iterator iter = ids.begin();
   iter != ids.end();
   ++iter)
      {
  
  int flg = 0;
  XYZ & crd = crds[*iter];
  XYZ & crdH1 = crds[*iter + 1];
  XYZ & crdH2 = crds[*iter + 2];
  while ( min.mX > crd.mX){crd.mX += dx; crdH1.mX += dx; crdH2.mX += dx;}
  while ( max.mX < crd.mX){crd.mX -= dx; crdH1.mX -= dx; crdH2.mX -= dx;}
  while ( min.mY > crd.mY){crd.mY += dy; crdH1.mY += dy; crdH2.mY += dy;}
  while ( max.mY < crd.mY){crd.mY -= dy; crdH1.mY -= dy; crdH2.mY -= dy;}
  while ( min.mZ > crd.mZ){crd.mZ += dz; crdH1.mZ += dz; crdH2.mZ += dz;}
  while ( max.mZ < crd.mZ){crd.mZ -= dz; crdH1.mZ -= dz; crdH2.mZ -= dz;}
  crds[*iter] = crd;
  crds[*iter + 1] = crdH1;
  crds[*iter + 2] = crdH2;
      }
  }


};

#endif
