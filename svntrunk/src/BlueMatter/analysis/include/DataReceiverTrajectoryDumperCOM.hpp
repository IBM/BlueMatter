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
 #ifndef DATARECEIVERTRAJECTORYDUMPERCOM_HPP
#define DATARECEIVERTRAJECTORYDUMPERCOM_HPP

#ifdef AK_ONLY_FOR_DEBUG_INCLUDE_LATER

#include <ostream>
#include <fstream>
#include <vector>
#include <BlueMatter/DataReceiverTrajectoryDumper.hpp>


#define COM_STEADY_SHOT

using namespace std;

class DataReceiverTrajectoryDumperCOM : public DataReceiverTrajectoryDumper
{
public:

    double *masses;


    void setParams(char *BMTFileName, char *PDBFileName, int NNonWaterAtoms, int NWaters, char *connectivity, double *mass, int DoAppend=0, int NoWaters=0,int SimulationStart=0, int Reverse=0, int nXImages = 1, int nYImages = 1, int nZImages = 1)
    {      
      strcpy(mBMT.BMTFileName, BMTFileName);
      strcpy(mBMT.PDBFileName, PDBFileName);
      mBMT.m_MolecularSystem.mNNonWaterAtoms = NNonWaterAtoms;
      mBMT.m_MolecularSystem.mNWaters = NWaters;
      mBMT.m_DoAppend = DoAppend;
      mBMT.mNoWaters = NoWaters;
      mBMT.mStartAtomIndex = 0;
      mBMT.mEndAtomIndex = NNonWaterAtoms + 3 * NWaters  - 1;
      if (NoWaters)
	mBMT.mEndAtomIndex = NNonWaterAtoms  - 1;
      mImage.mX = (short) nXImages;
      mImage.mY = (short) nYImages;
      mImage.mZ = (short) nZImages;
      strcpy(mConnectivity, connectivity);
      addFragments();
      masses = mass;
      mSimulationStart = SimulationStart;
      mReverse = Reverse;
    }


  void FrameCenter(tSiteData *full_ps, int Start, int End, XYZ & BoxWidth, XYZ & cen)
  {
    tSiteData *ps = &full_ps[Start];
    double m = 0;
    XYZ offset;
    cen.Zero();
    for (int i=Start; i<=End; i++) {
      XYZ s0 = ps->mPosition;
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
		  cen = cen + masses[i] * s;
		  m += masses[i];
		}
	    }
	}

      ps++;
    }
    ps = &full_ps[Start];
    double M = 1.0 / m;
    cen = M * cen;
    return; 
  }
void MapIntoBox(tSiteData *ps, XYZ &box, XYZ &cen)
{
    int count = mFragmentList.size();
    int index = 0;
    for (int i=0; i<count; i++) 
      {
	for(int j = index; j < index + mFragmentList[i];j++)
	  { ps[j].mPosition -= cen; }
        TranslateIntoBox(ps, box, index, mFragmentList[i]);
	index += mFragmentList[i];
      }
}
  
    virtual void sites(Frame *f)
	{
            static char fname[512];
	    static int firsttime = 1;
	    
            int NumToWrite;
            int NumWritten;

            unsigned CurrentStep = f->mOuterTimeStep;

            tSiteData *ps= f->mSiteData.getArray();
            int NSites = f->mSiteData.getSize();

	    XYZ min,max,width;
	    min = f->mInformationRTP.mDynamicBoundingBoxMin;
	    max = f->mInformationRTP.mDynamicBoundingBoxMax;
	    width = max - min;

	    if (firsttime) {
		firsttime = 0;

		mStream << "INITIALBOX " << min << " " << max << endl;
                mStream << "TRAJECTORYFORMAT 16-bit RESCALE RECENTER" << endl;
                mStream.close();
		// close text mode and switch to binary, appending at end
		mFile = fopen(mBMTFileName, "ab");
		if (!mFile) { 
		    cerr << "BMT output file " << mBMTFileName << " could not be open for binary write/append." << endl;
		    exit(-1);
		}
	    }
#ifdef COM_STEADY_SHOT
     	    XYZ cen;
	    FrameCenter(ps,mStartIndex,mEndIndex,width,cen);
	    if (mDoMapping)
	      MapIntoBox(ps, width, cen);
#else
	    if (mDoMapping)
	      MapIntoBox(ps, width);
#endif	
    
	    XYZ origin, span;
	    double maxspan;
	    findBox(ps, mStartIndex, mEndIndex, width, origin, span, maxspan);
	    int NImageSites = NSites * mImage.mX * mImage.mY * mImage.mZ;

	    if (!mpShortPos)
		mpShortPos = new XYZShort[NImageSites];

	    double factor = 65535/maxspan;
	    simplifySites(ps, mpShortPos, mStartIndex, mEndIndex, width,  origin, factor);

	    float recipfactor = maxspan/65535;
	    float x,y,z;
#ifdef COM_STEADY_SHOT
	    FrameCenter(ps,mStartIndex,mEndIndex,width,cen);

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
	}
};

#endif
#endif
