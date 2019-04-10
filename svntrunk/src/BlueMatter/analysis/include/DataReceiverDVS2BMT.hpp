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
 #ifndef DATARECEIVERDVS2BMT_HPP
#define DATARECEIVERDVS2BMT_HPP

#include <ostream>
#include <fstream>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>



#define STEADY_SHOT

using namespace std;



class DataReceiverDVS2BMT : public DataReceiver
{
public:
  ofstream mStream;
  FILE *mFile;
  char mBMTFileName[256];
  char mPDBFileName[256];
  int mNNonWaterAtoms;
  int mNWaters;
  int mDoAppend;
  int mStartIndex;
  int mEndIndex;
  int mNoWaters;
  usXYZ mImage;
  usXYZ *mpShortPos;


   void setParams(char *BMTFileName, char *PDBFileName, int NNonWaterAtoms, int NWaters, int DoAppend=0, int NoWaters=0, int nXImages = 1, int nYImages = 1, int nZImages = 1)
    {      
      mpShortPos = NULL;
      strcpy(mBMTFileName, BMTFileName);
      strcpy(mPDBFileName, PDBFileName);
      mNNonWaterAtoms = NNonWaterAtoms;
      mNWaters = NWaters;
      mDoAppend = DoAppend;
      mNoWaters = NoWaters;
      mStartIndex = 0;
      mEndIndex = NNonWaterAtoms + 3 * NWaters  - 1;
      if (NoWaters)
	mEndIndex = NNonWaterAtoms  - 1;
      mImage.mX = (short) nXImages;
      mImage.mY = (short) nYImages;
      mImage.mZ = (short) nZImages;
    }

private:
    unsigned numberofoutertimesteps;
    int numberofoutertimestepsknown;
    unsigned currentstep;
    int currentstepknown;
    int rtpknown;
    ED_ControlPressure pressure;
    int pressureknown;
    unsigned velocityresamplerandomseed;
    unsigned velocityresamplerandomseedknown;
    unsigned velocityresampleperiodinots;
    unsigned velocityresampleperiodinotsknown;
    double   velocityresampletargettemperature;
    unsigned velocityresampletargettemperatureknown;
    unsigned snapshotperiodinots;
    unsigned snapshotperiodinotsknown;
    unsigned shiftv;
    unsigned shiftp;
    XYZ vshift;
    XYZ pshift;
    double xmin, ymin, zmin, xmax, ymax, zmax;
    int boxknown;
    double resampleenergyloss;
    int resampleenergylossknown;
    ArrayGrowable<ED_DynamicVariablesSite> sites;
    int waterstart;
    int randomwaterbox;
    double randomboxsize;

public:
    ED_InformationRTP rtp;
    DataReceiverDVS2BMT()
    {
        numberofoutertimesteps = 0;
        numberofoutertimestepsknown = 0;
        currentstep = 0;
        currentstepknown = 0;
        rtpknown = 0;
        pressureknown = 0;
        velocityresamplerandomseedknown = 0;
        velocityresampleperiodinots = 0;
        velocityresampleperiodinotsknown = 0;
        velocityresampletargettemperature = 0;
        velocityresampletargettemperatureknown = 0;
        snapshotperiodinots = 0;
        snapshotperiodinotsknown = 0;
        resampleenergyloss = 0;
        resampleenergylossknown = 0;
        boxknown = 0;
        waterstart = 0;
        randomwaterbox = 0;
        randomboxsize = 0;

        pressure.mFullOuterTimeStep = 0;
        pressure.mVirial = XYZ::ZERO_VALUE();
        pressure.mVolumeVelocity = XYZ::ZERO_VALUE();
        pressure.mVolumeAcceleration = XYZ::ZERO_VALUE();
        pressure.mInternalPressure = XYZ::ZERO_VALUE();

        vshift = XYZ::ZERO_VALUE();
        pshift = XYZ::ZERO_VALUE();

        rtp.mCoulombMethod = 0;
        rtp.mInnerTimeStepInPicoSeconds = 0;
        rtp.mNumberOfInnerTimeSteps = 0;
        rtp.mOuterTimeStepInPicoSeconds = 0;
        rtp.mNumberOfOuterTimeSteps = 0;
        rtp.mNumberOfRESPALevels = 0;
        rtp.mPressureControlPistonMass = 0;
        rtp.mPressureControlTarget = 0;
        rtp.mVelocityResampleTargetTemperature = 0;
        rtp.mVelocityResamplePeriodInOTS = 0;
        rtp.mEnsemble = 0;
        rtp.mSnapshotPeriodInOTS = 0;
        rtp.mVelocityResampleRandomSeed = 0;
        rtp.mSourceID = 0;
        rtp.mExecutableID = 0;
        rtp.mNumberOfTwoChainThermostats = 0;
        rtp.mJobID = 0;
        rtp.mDynamicBoundingBoxMin = XYZ::ZERO_VALUE();
        rtp.mDynamicBoundingBoxMax = XYZ::ZERO_VALUE();
    }



    //  All these setXXX guys assume the command line settings have happened first, and any duplicate setting
    //  was caused by values read from the snapshot file

    //  1)  Set all terms to 0 in rtp and in local variables
    //  2)  Set all known states to unknown
    //  3)  Parse command options and assign local variables accordingly
    //  4)  Load dvs and assign sites, rtp, energyloss, etc.
    //  5)  In final() call, overwrite rtp values by values set on command line

    void setInformationRTP(ED_InformationRTP &artp)
    {
        if (!rtpknown) {
            // this stomps on existing rtp settings
            // but stores final settings in local variables
            // in final(), the final rtp values are re-inserted
            rtp = artp;
            if (rtp.mDynamicBoundingBoxMin != XYZ::ZERO_VALUE() || rtp.mDynamicBoundingBoxMax != XYZ::ZERO_VALUE())
                setBoundingBox(rtp.mDynamicBoundingBoxMin.mX, rtp.mDynamicBoundingBoxMin.mY, rtp.mDynamicBoundingBoxMin.mZ,
                               rtp.mDynamicBoundingBoxMax.mX, rtp.mDynamicBoundingBoxMax.mY, rtp.mDynamicBoundingBoxMax.mZ);
            if (rtp.mVelocityResampleRandomSeed)
                setVelocityResampleRandomSeed(rtp.mVelocityResampleRandomSeed);
            if (rtp.mVelocityResamplePeriodInOTS)
                setVelocityResamplePeriod(rtp.mVelocityResamplePeriodInOTS);
            if (rtp.mVelocityResampleTargetTemperature)
                setTemp(rtp.mVelocityResampleTargetTemperature);
            if (rtp.mSnapshotPeriodInOTS)
                setSnapshotPeriod(rtp.mSnapshotPeriodInOTS);
            if (rtp.mNumberOfOuterTimeSteps)
                setNSteps(rtp.mNumberOfOuterTimeSteps);
            rtpknown = 1;
        } else {
            cerr << "Duplicate rtp packet ignored." << endl;
        }
    }

    void setWaterStart(int n) { waterstart = n; }

    void setRandomWaterBox(int n) { randomwaterbox = n; }

    void setRandomBoxSize(double x) { randomboxsize = x; }

    void setVShift(XYZ v)
    {
        vshift = v;
        shiftv = 1;
    }

    void setPShift(XYZ v)
    {
        pshift = v;
        shiftp = 1;
    }

    void setControlPressure(ED_ControlPressure &apressure)
    {
        if (!pressureknown) {
            pressure = apressure;
            pressureknown = 1;
        } else {
            cerr << "Duplicate pressure packet ignored." << endl;
        }
    }

    void setBoundingBox(double x0, double y0, double z0, double x1, double y1, double z1)
    {
        if (!boxknown) {
            xmin = x0;
            ymin = y0;
            zmin = z0;
            xmax = x1;
            ymax = y1;
            zmax = z1;
            boxknown = 1;
        } else {
            cerr << "Snapshot bounding box setting ignored - using command-line value." << endl;
        }
    }

    void setVelocityResampleRandomSeed(unsigned seed)
    {
        if (!velocityresamplerandomseedknown) {
            velocityresamplerandomseed = seed;
            velocityresamplerandomseedknown = 1;
        } else {
            cerr << "Snapshot VelocityResampleRandomSeed " << seed << " ignored - using " << velocityresamplerandomseed << endl;
        }
    }

    void setVelocityResamplePeriod(unsigned period)
    {
        if (!velocityresampleperiodinotsknown) {
            velocityresampleperiodinots = period;
            velocityresampleperiodinotsknown = 1;
        } else {
            cerr << "Snapshot VelocityResamplePeriod " << period << " ignored - using " << velocityresampleperiodinots << endl;
        }
    }

    void setSnapshotPeriod(unsigned period)
    {
        if (!snapshotperiodinotsknown) {
            snapshotperiodinots = period;
            snapshotperiodinotsknown = 1;
        } else {
            cerr << "Snapshot SnapshotPeriodInOTS " << period << " ignored - using " << snapshotperiodinots << endl;
        }
    }

    void setTemp(double T)
    {
        if (velocityresampletargettemperatureknown) {
            if (T != velocityresampletargettemperature) {
                cerr << "Snapshot file Temp " << T << " ignored - using " << velocityresampletargettemperature << endl;
            }
            return;
        }
        velocityresampletargettemperature = T;
        velocityresampletargettemperatureknown = 1;
    }

    void setCurrentStep(unsigned N)
    {
        if (currentstepknown) {
            if (N != currentstep) {
                cerr << "Snapshot file CurrentStep " << N << " ignored - using " << currentstep << endl;
            }
            return;
        }
        currentstep = N;
        currentstepknown = 1;
    }

    void setNSteps(unsigned N)
    {
        if (numberofoutertimestepsknown) {
            if (N != numberofoutertimesteps) {
                cerr << "dvs file NSteps " << N << " ignored - using " << numberofoutertimesteps << endl;
            }
            return;
        }
        numberofoutertimesteps = N;
        numberofoutertimestepsknown = 1;
    }

    void reset()
    {
        cerr << "resetting to zero:  temp, nsteps, currentstep, resampleperiod, resampletemperature, snapshotperiod, energyloss" << endl;
        numberofoutertimestepsknown = 1;
        currentstepknown = 1;
        velocityresampleperiodinotsknown = 1;
        velocityresampletargettemperatureknown = 1;
        snapshotperiodinotsknown = 1;
        resampleenergylossknown = 1;
    }

    void resetAll()
    {
        reset();
        xmin = 0;
        ymin = 0;
        zmin = 0;
        xmax = 0;
        ymax = 0;
        zmax = 0;
        cerr << "resetting to zero:  bounding box" << endl;
        boxknown = 1;
    }

    virtual void velocityResampleTargetTemperature(ED_RTPValuesVelocityResampleTargetTemperature &t)
    {
        setTemp(t.mValue);
    }

    virtual void velocityResampleRandomSeed(ED_RTPValuesVelocityResampleRandomSeed &t)
    {
    }

    virtual void numberOfOuterTimeSteps(ED_RTPValuesNumberOfOuterTimeSteps &t)
    {
        setNSteps(t.mValue);
    }

    virtual void snapshotPeriodInOTS(ED_RTPValuesSnapshotPeriodInOTS &t)
    {
    }

    virtual void informationRTP(ED_InformationRTP &rtp)
    {
        setInformationRTP(rtp);
    }

    virtual void controlPressure(ED_ControlPressure &pressure)
    {
        setControlPressure(pressure);
    }


    virtual void init()
	{
            char buff[1024];
	    int ImageFactor = mImage.mX * mImage.mY * mImage.mZ;
	    // printf("DVS2BMT::init called\n");
	    
	    int n = mEndIndex + 1 ;
	    sites.resize(n);

	    mpShortPos = NULL;
            if (!mDoAppend) {
                mStream.open(mBMTFileName);
                if (!mStream.is_open()) {
                    cerr << "BMT output file " << mBMTFileName << " could not be open." << endl;
                    exit(-1);
                }
                mStream << "BMT" << endl;
                mStream << "VERSION 1.00" << endl;
                mStream << "STRUCTURE " << mPDBFileName << endl;
                mStream << "BONDS NULL" << endl;
                mStream << "NUMNONWATERATOMS " << mNNonWaterAtoms * ImageFactor << endl;
                mStream << "NUMWATERS " << mNWaters * ImageFactor<< endl;
		mStream << "NOWATERS " << mNoWaters << endl;
		mStream << "STARTINDEX " << mStartIndex << endl;
		mStream << "ENDINDEX " << (mEndIndex + 1) * ImageFactor - 1 << endl;
                mStream << "TRAJECTORYFORMAT 16-bit CENTERED IMAGED " << mImage.mX << " " << mImage.mY << " " << mImage.mZ << endl;
                mStream.close();
            }
            // close text mode and switch to binary, appending at end
	    mFile = fopen(mBMTFileName, "ab");
            if (!mFile) { 
                cerr << "BMT output file " << mBMTFileName << " could not be open for binary write/append." << endl;
                exit(-1);
            }
	
	}

    void GetShiftVector(XYZ &v, int aNonZero = 0)
    {
        int i = int((float(rand())/RAND_MAX*2 - 1)*randomwaterbox);
        int j = int((float(rand())/RAND_MAX*2 - 1)*randomwaterbox);
        int k = int((float(rand())/RAND_MAX*2 - 1)*randomwaterbox);

        while (aNonZero && (i == 0 || j == 0 || k == 0)) {
            i = int((float(rand())/RAND_MAX*2 - 1)*randomwaterbox);
            j = int((float(rand())/RAND_MAX*2 - 1)*randomwaterbox);
            k = int((float(rand())/RAND_MAX*2 - 1)*randomwaterbox);
        }

        // cerr << i << " " << j << " " << k << endl;
        v.mX = i * randomboxsize;
        v.mY = j * randomboxsize;
        v.mZ = k * randomboxsize;
        // cerr << v.mX << endl;
    }

 
    virtual void newSitePacket(ED_DynamicVariablesSite &dvs)
    {
	// printf("Site %d %f %f %f\n",dvs.mSiteId,dvs.mPosition.mX,dvs.mPosition.mY,dvs.mPosition.mZ);
        sites.set(dvs.mSiteId, dvs);
    }


   virtual void final()
    {

#if 0
      // printf("Final Called\n");  
            static char fname[512];
	    
            int NumToWrite;
            int NumWritten;


            int NSites = sites.getSize();

	    XYZ min,max,width;
	    min.mX = xmin; min.mY = ymin; min.mZ = zmin;
	    max.mX = xmax; max.mY = ymax; max.mZ = zmax;
	    width = max - min;
	    // printf(" width = %f %f %f\n",width.mX,width.mY,width.mZ);
	    XYZ origin, span;
	    double maxspan;
	    findBox(mStartIndex, mEndIndex, width, origin, span, maxspan);
	    int NImageSites = NSites * mImage.mX * mImage.mY * mImage.mZ;

	    if (!mpShortPos)
		mpShortPos = new XYZShort[NImageSites];

	    double factor = 65535/maxspan;
	    simplifySites(mpShortPos, mStartIndex, mEndIndex, width,  origin, factor);

	    float recipfactor = maxspan/65535;
	    float x,y,z;
#ifdef STEADY_SHOT
     	    XYZ cen;
	    FrameCenter(mStartIndex,mEndIndex,width,cen);

	    x = origin.mX - cen.mX;
	    y = origin.mY - cen.mY;
	    z = origin.mZ - cen.mZ;
#else
	    x = origin.mX;
	    y = origin.mY;
	    z = origin.mZ;
#endif
            NumToWrite = sizeof(float);
	    NumWritten = fwrite(&x, sizeof(float), 1, mFile);
            assert(NumWritten == NumToWrite);
	    NumWritten = fwrite(&y, NumToWrite, 1, mFile);
            assert(NumWritten == NumToWrite); 
	    NumWritten = fwrite(&z, NumToWrite, 1, mFile);
            assert(NumWritten == NumToWrite);
	    NumWritten = fwrite(&recipfactor, NumToWrite, 1, mFile);
            assert(NumWritten == NumToWrite);


	    XYZShort *psp = mpShortPos;
            NumToWrite = sizeof(unsigned short);
            for (int i=0; i< NImageSites; i++) {
		NumWritten = fwrite(psp, NumToWrite, 3, mFile);
                assert(NumWritten == NumToWrite);
                psp++;
            }

	if (fclose(mFile)) {
            cerr << "Error on closing " << mBMTFileName << endl;
        }
            
	if (mpShortPos)
	    delete [] mpShortPos;
#endif
    }
  void FrameCenter(int Start, int End, XYZ & BoxWidth, XYZ & cen)
  {

    int n = 0;
    XYZ offset;
    cen.Zero();
    for (int i=Start; i<=End; i++) {
      XYZ s0 = sites[i].mPosition;
      for(short iX = 0; iX < mImage.mX ; iX++)
	{
	  offset.mX = iX * BoxWidth.mX;
	  for(short iY = 0; iY < mImage.mY ; iY++)
	    {
	      offset.mY = iY * BoxWidth.mY;
	      for(short iZ = 0; iZ < mImage.mZ ; iZ++)
		{
		  offset.mZ = iZ * BoxWidth.mZ;
		  XYZ s = s0 + offset; 
		  cen = cen + s;
		  n++;
		}
	    }
	}
    }
    double N = 1.0 / n;
    cen = N * cen;
    return; 
  }

    void findBox(int Start, int End,XYZ &BoxWidth , XYZ &origin, XYZ &span, double &MaxSpan)
    {
	XYZ LL, UR, offset, s;

	LL.mX = LL.mY = LL.mZ = 1.0E20;
	UR.mX = UR.mY = UR.mZ = -1.0E20;

	for (int i=Start; i<=End; i++) {
	    XYZ s0 = sites[i].mPosition;
	    for(short iX = 0; iX < mImage.mX ; iX++)
	      {
		offset.mX = iX * BoxWidth.mX;
		for(short iY = 0; iY < mImage.mY ; iY++)
		  {
		    offset.mY = iY * BoxWidth.mY;
		    for(short iZ = 0; iZ < mImage.mZ ; iZ++)
		      {
			offset.mZ = iZ * BoxWidth.mZ;
			s = s0 + offset; 
			if (s.mX < LL.mX) LL.mX = s.mX;
			if (s.mX > UR.mX) UR.mX = s.mX;
			if (s.mY < LL.mY) LL.mY = s.mY;
			if (s.mY > UR.mY) UR.mY = s.mY;
			if (s.mZ < LL.mZ) LL.mZ = s.mZ;
			if (s.mZ > UR.mZ) UR.mZ = s.mZ;
		      }
		  }
	      }
	}
	origin = LL;
	span = UR - LL;
	MaxSpan = span.mX;
	if (span.mY > MaxSpan) MaxSpan = span.mY;
	if (span.mZ > MaxSpan) MaxSpan = span.mZ;
    }

    void simplifySites(usXYZ *psp, int Start, int End, XYZ &BoxWidth, XYZ &origin, double factor)
    {
	XYZ offset;

	

	//  First replicate all of the non water atoms
	for(int iX = 0; iX < mImage.mX ; iX++)
	  {
	    offset.mX = iX * BoxWidth.mX;
	    for(int iY = 0; iY < mImage.mY ; iY++)
	      {
		offset.mY = iY * BoxWidth.mY;
		for(int iZ = 0; iZ < mImage.mZ ; iZ++)
		  {
		    offset.mZ = iZ * BoxWidth.mZ;

		    for (int i=Start; i< mNNonWaterAtoms; i++) 
		      {
			XYZ s0 = sites[i].mPosition;
			XYZ p = s0 + offset; 
			
			p -= origin;
			psp->mX = p.mX*factor;
			psp->mY = p.mY*factor;
			psp->mZ = p.mZ*factor;
			//		      printf("image %d %d %d  simpcoord %d %d %d\n",iX,iY,iZ,psp->mX, psp->mY, psp->mZ);
			psp++;
		      }
		  }
	      }
	  }


	// Now loop through and replicate all of the waters
	for(int iX = 0; iX < mImage.mX ; iX++)
	  {
	    offset.mX = iX * BoxWidth.mX;
	    for(int iY = 0; iY < mImage.mY ; iY++)
	      {
		offset.mY = iY * BoxWidth.mY;
		for(int iZ = 0; iZ < mImage.mZ ; iZ++)
		  {
		    offset.mZ = iZ * BoxWidth.mZ;

		    for (int i=Start + mNNonWaterAtoms; i<= End; i++) 
		      {
			XYZ s0 = sites[i].mPosition;
			XYZ p = s0 + offset; 
			
			p -= origin;
			psp->mX = p.mX*factor;
			psp->mY = p.mY*factor;
			psp->mZ = p.mZ*factor;
			//		      printf("image %d %d %d  simpcoord %d %d %d\n",iX,iY,iZ,psp->mX, psp->mY, psp->mZ);
			psp++;
		      }
		  }
	      }
	  }

    }
  

};

#endif
