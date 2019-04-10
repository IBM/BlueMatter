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
 #ifndef DATARECEIVERLIPIDLOGGER_HPP
#define DATARECEIVERLIPIDLOGGER_HPP

#include <assert.h>
#include <fcntl.h>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/DataReceiverSimpleLogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
#include <BlueMatter/bootstrap.hpp>

#include <BlueMatter/db2/molecularsystem.hpp>

#include <vector>
#include <iostream>

#define NFORM " %#14.12g"
#define SFORM " %14s"
#define MAX_HIST_SIZE 200

class DataReceiverLipidLogger : public DataReceiverSimpleLogger
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
  std::vector< std::vector<int> > mHyds;
  std::vector<int> mPhos;   //PCGL: P  OP3  OP4  OP1   OP2 
  std::vector<int> mChol;   //PCGL: N  C5  C6 C7   C8 H5A H5B  H6A   H6B   H6C  H7A   H7B  H7C H8A   H8B H8C  C4   H4A  H4B
  std::vector<int> mGly;    // PCGL: C3   H3A  H3B   C2   H2A  C1   H1A H1B
  std::vector<int> mCarb;   // PCGL: O1 O2 OLEO: C1 O1 
  std::vector<int> mCH2;   // OLEO: C2-C8 H1[AB]-H8[AB] C11-C17 H11-17[AB]
  std::vector<int> mOle;   // OLEO C9 C10 H9A H10A
  std::vector<int> mCH3;   // OLEO C18 H18[ABC]
  std::vector<int> mH2O;  // HOH
  std::vector<int> mH2O_O;  // oxygens for HOH.  Used for water reimaging
  std::vector< std::vector<int> > mAtch;
  std::vector< std::vector<int> > mChainHyds;

  int mNZSlabs;
  double mSlabSpacing;

  std::vector<int> mPhosZCnts;
  std::vector<int> mCholZCnts;
  std::vector<int> mGlyZCnts;
  std::vector<int> mCarbZCnts;
  std::vector<int> mCH2ZCnts;
  std::vector<int> mOleZCnts;
  std::vector<int> mCH3ZCnts;
  std::vector<int> mH2OZCnts;

  FILE *mLipOut;

  FILE *CHVecsPF[20];

   virtual void init()
    {
    }



public:
  DataReceiverLipidLogger(int DelayStats = 0, int OutputPeriod = 1, int BootStrap = 0, int AllSnapshots = 0, char *db_name = NULL, int sysid = -1, double xyscale = 1.0, int selected_step = 0) : DataReceiverSimpleLogger(DelayStats, OutputPeriod, BootStrap, AllSnapshots)
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

	mXYscale = xyscale;
	//	mCarbs = aMolSys->SelectedSites("component_name = 'OLEO' and element_symbol ='C'");
	char cmd[256],cmdbuf[1000];  
	for( i = 3; i<=17;i++)
	  {
	    sprintf(cmd,"component_name = 'OLEO' and (atom_tag ='H%dA' or atom_tag = 'H%dB')",i,i);
	    mHyds.push_back(aMolSys->SelectedSites(cmd));
	  }

	//  // Collect phosphates
	cmdbuf[0] = '\0';
	sprintf(cmd,"component_name = 'PCGL' and (atom_tag ='P' or atom_tag = 'OP1' or atom_tag = 'OP2' or atom_tag = 'OP3' or atom_tag = 'OP4')");
	strcat(cmdbuf,cmd);
	mPhos = aMolSys->SelectedSites(cmdbuf);

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
	mChol = aMolSys->SelectedSites(cmdbuf);

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

	// Collect carbonyls
	cmdbuf[0] = '\0';
	sprintf(cmd,"( ( component_name = 'PCGL' and (atom_tag ='O1' or atom_tag = 'O2') ) or  ");
	strcat(cmdbuf,cmd);
	sprintf(cmd," ( component_name = 'OLEO' and (atom_tag ='C1' or atom_tag = 'O1') ) )  ");
	strcat(cmdbuf,cmd);
	mCarb = aMolSys->SelectedSites(cmdbuf);


	// Collect CH2s 
	cmdbuf[0] = '\0';
	sprintf(cmd,"component_name = 'OLEO' and ( atom_tag = 'C' ");
	strcat(cmdbuf,cmd);
	for(i=2; i<=8;i++)
	  {
	    sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB'  ",i,i,i);
	    strcat(cmdbuf,cmd);
	  }
	for(i=11; i<=17;i++)
	  {
	    sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA' or atom_tag = 'H%dB'  ",i,i,i);
	    strcat(cmdbuf,cmd);
	  }
	strcat(cmdbuf,")");
	mCH2 = aMolSys->SelectedSites(cmdbuf);

	// Collect Olefins
	cmdbuf[0] = '\0';
	sprintf(cmd,"component_name = 'OLEO' and ( atom_tag = 'C' ");
	strcat(cmdbuf,cmd);
	for(i=9; i<=10;i++)
	  {
	    sprintf(cmd," or atom_tag = 'C%d' or atom_tag = 'H%dA'   ",i,i);
	    strcat(cmdbuf,cmd);
	  }
	strcat(cmdbuf,")");
	mOle = aMolSys->SelectedSites(cmdbuf);

	// Collect Methyls
	cmdbuf[0] = '\0';
	sprintf(cmd,"component_name = 'OLEO' and ( atom_tag = 'C18' or atom_tag = 'H18A' or atom_tag = 'H18B' or atom_tag = 'H18C' )  ");
	strcat(cmdbuf,cmd);
	mCH3 = aMolSys->SelectedSites(cmdbuf);

	// Collect Waters
	cmdbuf[0] = '\0';
	sprintf(cmd,"component_name = 'HOH'");
	mH2O = aMolSys->SelectedSites(cmd);

	// Collect Water oxygens
	cmdbuf[0] = '\0';
	sprintf(cmd,"(component_name = 'HOH' and element_symbol = 'O')");
	mH2O_O = aMolSys->SelectedSites(cmd);


	printf("Now Collecting Chain Hyds\n");
	// Collect ChainCarbs 
	for(i=2; i<=18;i++)
	  {
	    sprintf(cmdbuf,"component_name = 'OLEO' and ");
	    sprintf(cmd,"(atom_tag = 'H%dA' or atom_tag = 'H%dB' or atom_tag = 'H%dC')",i,i,i);
	    strcat(cmdbuf,cmd);	    
	    std::vector< int > hyds = aMolSys->SelectedSites(cmdbuf);
	    //	    printf("Carbon # %d nHyds = %d\n",i,hyds.size());
	    mChainHyds.push_back( hyds );
	  }

	const std::vector< std::vector<int>  > &atch = aMolSys->bonds().asAdjList();
	mAtch = atch;
	mLipOut = fopen("LipidOrderParam.dat","w");

	for(i=0;i<20;i++)
	  { CHVecsPF[i] = NULL; }
	for(i=2;i<=18;i++)
	  { 
	    char bufS[256];
	    sprintf(bufS,"CHvec_%d.dat",i);
	    CHVecsPF[i] = fopen(bufS,"w"); 
	  }

	mNZSlabs = MAX_HIST_SIZE;
	mSlabSpacing = 0.5;

	mPhosZCnts.reserve(MAX_HIST_SIZE+1);
	mCholZCnts.reserve(MAX_HIST_SIZE+1);
	mGlyZCnts.reserve(MAX_HIST_SIZE+1);
	mCarbZCnts.reserve(MAX_HIST_SIZE+1);
	mCH2ZCnts.reserve(MAX_HIST_SIZE+1);
	mCH3ZCnts.reserve(MAX_HIST_SIZE+1);
	mOleZCnts.reserve(MAX_HIST_SIZE+1);
	mH2OZCnts.reserve(MAX_HIST_SIZE+1);
	for (i=0;i<=mNZSlabs;i++)
	  {
	    mPhosZCnts[i] = 0;
	    mCholZCnts[i] = 0;
	    mGlyZCnts[i] = 0;
	    mCarbZCnts[i] = 0;
	    mCH2ZCnts[i] = 0;
	    mOleZCnts[i] = 0;
	    mCH3ZCnts[i] = 0;
	    mH2OZCnts[i] = 0;
	  }
	
	mVave = mdzave = 0.0;
	mNobs = 0;


	//	printf("Finished Initialization\n"); fflush(stdout);
	//	int oleo_idx = db2::ChemicalComponentList::instance()->indexOfComponent("OLEO");
	//	assert(oleo_idx != -1);

	//  for (std::vector<int>::const_iterator iter = Carbs.begin();
//  	     iter != Carbs.end();
//  	     ++iter)
//  	  {
//  	    std::cout << "C " << std::setw(8) << "C " << *iter  << std::endl;
//  	  }
//  	for (std::vector<int>::const_iterator iter = Hyds.begin();
//  	     iter != Hyds.end();
//  	     ++iter)
//  	  {
//  	    std::cout << "H " << std::setw(8) << *iter  << std::endl;
//  	  }
 	//	printf("OLEO indx = %d\n",oleo_idx);
//  	int i = 0;
//  	for( std::vector< std::vector<int> >::const_iterator atch_iter = atch.begin();
//  	     atch_iter != atch.end();
//  	     i++, atch_iter++)
//  	{
//  	  int natch =  (*atch_iter).size();
//  	  printf("Natch[%d] = %d\n",i,natch);
	  
//  	}

    }
  ~DataReceiverLipidLogger()
  {
    fclose(mLipOut);
    for(int i = 2;i<=18;i++)
      fclose(CHVecsPF[i]);
  }

    virtual void sites(Frame *f)
    {
        char fname[512];
        char p[MAXPACKEDPACKETSIZE];
	XYZ min,max;
	double aspectZperX;

        unsigned CurrentStep = f->mOuterTimeStep;

	if(mWriteSelectedSnapshot && (CurrentStep != mSelectedStep))
           return;

        if (SaveAllSnapshots()) {
	    sprintf(fname, "LipSnapshot%08d.dvs", CurrentStep);
	} else {
	    strcpy(fname, "LipSnapshot.dvs");
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
	    
	    min = f->mInformationRTP.mDynamicBoundingBoxMin;
	    max = f->mInformationRTP.mDynamicBoundingBoxMax;

	    aspectZperX = (max.mZ - min.mZ) / (max.mX - min.mX);

	    min.mX *= mXYscale;
	    min.mY *= mXYscale;
	    max.mX *= mXYscale;
	    max.mY *= mXYscale;
	    
	    double dx = max.mX - min.mX;
	    double dy = max.mY - min.mY;
	    double dz = max.mZ - min.mZ;

	    mVave += dx*dy*dz;
	    mdzave += dz;
	    mNobs++;
	    
	    min.mX = -0.5*dx;	max.mX = 0.5*dx;
	    min.mY = -0.5*dy;	max.mY = 0.5*dy;
	    min.mZ = -0.5*dz;	max.mZ = 0.5*dz;
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
	
        tSiteData *ps= f->mSiteData.getArray();
        int NSites = f->mSiteData.getSize();
	std::vector< XYZ > coords(NSites);
	
        NumToWrite = ED_DynamicVariablesSite::PackedSize;        
        for (int i=0; i<NSites; i++) 
	  {
	    // Using 0 for voxel id 	    
	    coords[i]  = ps->mPosition;
	    coords[i].mX *= mXYscale;
	    coords[i].mY *= mXYscale;
            ps++;
	  }

        if (f->mRTPStatus & ArrivalStatus::RECEIVED) 
	  {
	    ReimageWaters(coords,mH2O_O,min,max);
	  }

	ps= f->mSiteData.getArray();
        for (int i=0; i<NSites; i++) 
	  {
	    // Using 0 for voxel id 	    
            ED_FillPacket_DynamicVariablesSite(p, CurrentStep, 0, i, coords[i], ps->mVelocity);
            NumWritten = write(fd, p, NumToWrite);
            assert(NumWritten == NumToWrite);
            ps++;
	  }
	
	UpdateCHVecs(coords);
	UpdateScd(coords);

	UpdateHist(coords,mPhos,mPhosZCnts,min,max);
	UpdateHist(coords,mChol,mCholZCnts,min,max);
	UpdateHist(coords,mGly,mGlyZCnts,min,max);
	UpdateHist(coords,mCarb,mCarbZCnts,min,max);
	UpdateHist(coords,mCH2,mCH2ZCnts,min,max);
	UpdateHist(coords,mCH3,mCH3ZCnts,min,max);
	UpdateHist(coords,mOle,mOleZCnts,min,max);
	UpdateHist(coords,mH2O,mH2OZCnts,min,max);


        close(fd);
        
        // If file exists, delete it
        // if (access(fname, 0) != -1)
        //     unlink(fname);


	OutputHist("Phos_Hist.dat",mPhosZCnts,min,max);
	OutputHist("Chol_Hist.dat",mCholZCnts,min,max);
	OutputHist("Gly_Hist.dat",mGlyZCnts,min,max);
	OutputHist("Carb_Hist.dat",mCarbZCnts,min,max);
	OutputHist("CH2_Hist.dat",mCH2ZCnts,min,max);
	OutputHist("Ole_Hist.dat",mOleZCnts,min,max);
	OutputHist("CH3_Hist.dat",mCH3ZCnts,min,max);
	OutputHist("H2O_Hist.dat",mH2OZCnts,min,max);

        DONEXT_1(sites, f);
    }

  void UpdateCHVecs(std::vector< XYZ> &crds)
  {
    for (int i = 2; i<=18; i++)
      {
	for(std::vector<int>::const_iterator iter = mChainHyds[i-2].begin();
	    iter != mChainHyds[i-2].end();
	    ++iter)
	  {
	    XYZ CHvec = crds[*iter] - crds[mAtch[*iter][0]];
	    double len = CHvec.Length();
	    CHvec = CHvec / len;
	    fprintf(CHVecsPF[i]," %10.7f %10.7f %10.7f    ", CHvec.mX,CHvec.mY,CHvec.mZ);
	  }
	fprintf(CHVecsPF[i],"\n");
      }
  }

  void UpdateScd(std::vector< XYZ> &crds)
  {

    int n = mHyds.size();
    std::vector<double> SCD(n);
    
    fprintf(mLipOut,"   SCD_3_17  ");
    for(int i = 0;i< n;i++)
      { 
	SCD[i] = 0.0; 
	int j = 0;
	for (std::vector<int>::const_iterator iter = mHyds[i].begin();
	     iter != mHyds[i].end();
	     j++,++iter)
	  {
	    XYZ CHvec = crds[*iter] - crds[mAtch[*iter][0]];
	    double len = CHvec.Length();
	    double cosCHZ = CHvec.mZ / len;  // equilvalent to cos(CHvec with (0,0,1))
	    double CHZOrderParam = 1.5*cosCHZ*cosCHZ - 0.5;
	    SCD[i] += CHZOrderParam;
	    if (0)
	      {
		printf("\tC[%f,%f,%f] \tH[%f,%f,%f] \tCH[%f,%f,%f]\n",
		       crds[mAtch[*iter][0]].mX,crds[mAtch[*iter][0]].mY,crds[mAtch[*iter][0]].mZ,
		       crds[*iter].mX,crds[*iter].mY,crds[*iter].mZ,
		       CHvec.mX,CHvec.mY,CHvec.mZ);
		printf("%d: len %f cosCHZ %f  order param = %f\n",i,len,cosCHZ,CHZOrderParam);
	      }
	  }
	SCD[i] /= j;
	fprintf(mLipOut,"\t%f", SCD[i]);
      }
    fprintf(mLipOut,"\n");
    fflush(mLipOut);
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
