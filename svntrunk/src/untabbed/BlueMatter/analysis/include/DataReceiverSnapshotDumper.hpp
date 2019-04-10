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
 #ifndef DATARECEIVERSNAPSHOTDUMPER_HPP
#define DATARECEIVERSNAPSHOTDUMPER_HPP

#include <ostream>
#include <fstream>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>


//   #define STEADY_SHOT

using namespace std;

struct XYZShort{
    unsigned short mX;
    unsigned short mY;
    unsigned short mZ;
};


class DataReceiverSnapshotDumper : public DataReceiver
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
#ifdef STEADY_SHOT
    XYZ FirstOrigin;
    int FirstFrame;
#endif
    XYZShort *mpShortPos;

    void setParams(char *BMTFileName, char *PDBFileName, int NNonWaterAtoms, int NWaters, int DoAppend=0, int NoWaters=0)
    {
  strcpy(mBMTFileName, BMTFileName);
        strcpy(mPDBFileName, PDBFileName);
        mNNonWaterAtoms = NNonWaterAtoms;
        mNWaters = NWaters;
        mDoAppend = DoAppend;
  mNoWaters = NoWaters;
  mStartIndex = 0;
  mEndIndex = NNonWaterAtoms + 3 * NWaters - 1;
  if (NoWaters)
      mEndIndex = NNonWaterAtoms - 1;
    }

    virtual void init()
  {
            char buff[1024];

      mpShortPos = NULL;
            if (!mDoAppend) {
                mStream.open(mBMTFileName);
                if (!mStream.is_open()) {
                    cerr << "SSD output file " << mBMTFileName << " could not be open." << endl;
                    exit(-1);
                }
                mStream << "SSD" << endl;
                mStream << "VERSION 1.00" << endl;
                mStream << "STRUCTURE " << mPDBFileName << endl;
                mStream << "BONDS NULL" << endl;
                mStream << "NUMNONWATERATOMS " << mNNonWaterAtoms << endl;
                mStream << "NUMWATERS " << mNWaters << endl;
    mStream << "NOWATERS " << mNoWaters << endl;
    mStream << "STARTINDEX " << mStartIndex << endl;
    mStream << "ENDINDEX " << mEndIndex << endl;
                mStream << "SNAPSHOTFORMAT nsites* double px,py,pz,vx,vy,vz" << endl;
                mStream.close();
            }

      mFile = fopen(mBMTFileName, "ab");
            if (!mFile) {
                cerr << "BMT output file " << mBMTFileName << " could not be open for binary write/append." << endl;
                exit(-1);
            }
  }

    virtual void final()
    {
  if (fclose(mFile)) {
            cerr << "Error on closing " << mBMTFileName << endl;
        }
            
  if (mpShortPos)
      delete [] mpShortPos;
    }
  void FrameCenter(tSiteData *full_ps, int Start, int End, XYZ &cen)
  {
    tSiteData *ps = &full_ps[Start];

    cen.Zero();
    for (int i=Start; i<=End; i++) {
      XYZ s = ps->mPosition;
      cen = cen + s;
      ps++;
    }
    double N = 1.0 / (Start - End);
    cen = N * cen;
    return; 
  }

    void findBox(tSiteData *full_ps, int Start, int End, XYZ &origin, XYZ &span, double &MaxSpan)
    {
  XYZ LL, UR;
  tSiteData *ps = &full_ps[Start];

  LL.mX = LL.mY = LL.mZ = 1.0E20;
  UR.mX = UR.mY = UR.mZ = -1.0E20;

  for (int i=Start; i<=End; i++) {
      XYZ s = ps->mPosition;
      if (s.mX < LL.mX) LL.mX = s.mX;
      if (s.mX > UR.mX) UR.mX = s.mX;
      if (s.mY < LL.mY) LL.mY = s.mY;
      if (s.mY > UR.mY) UR.mY = s.mY;
      if (s.mZ < LL.mZ) LL.mZ = s.mZ;
      if (s.mZ > UR.mZ) UR.mZ = s.mZ;
      ps++;
  }
  origin = LL;
  span = UR - LL;
  MaxSpan = span.mX;
  if (span.mY > MaxSpan) MaxSpan = span.mY;
  if (span.mZ > MaxSpan) MaxSpan = span.mZ;
    }

    void simplifySites(tSiteData *full_ps, XYZShort *psp, int Start, int End, XYZ &origin, double factor)
    {
  tSiteData *ps = &full_ps[Start];

  for (int i=Start; i<=End; i++) {
      XYZ p = ps->mPosition;
      p -= origin;
      psp->mX = p.mX*factor;
      psp->mY = p.mY*factor;
      psp->mZ = p.mZ*factor;
      ps++;
      psp++;
  }
    }

    virtual void sites(Frame *f)
  {

    
            static char fname[512];
      
            int NumToWrite;
            int NumWritten;

            unsigned CurrentStep = f->mOuterTimeStep;

            tSiteData *ps= f->mSiteData.getArray();
            int NSites = f->mSiteData.getSize();

      XYZ cen;

      //	    CenterFrame(ps,mStartIndex,mEndIndex);

#if 1
            for (int i=0; i<NSites; i++) 
        {
    int site = i+1;
    fwrite(&CurrentStep, sizeof(int), 1, mFile);
    fwrite(&site, sizeof(int), 1, mFile);
    fwrite(&ps->mPosition, sizeof(double), 3, mFile);
    fwrite(&ps->mVelocity, sizeof(double), 3, mFile);
                ps++;
        }
#endif

#if 0
            for (int i=0; i<NSites; i++) 
        {
    XYZ &p = ps->mPosition;
    XYZ &v = ps->mVelocity;
    fprintf(mFile,"%10d %6d %12.8f %12.8f %12.8f %12.8f %12.8 %12.8 /n",
      CurrentStep,i+1,p.mX,p.mY,p.mZ,v.mX,v.mY,v.mZ);
                ps++;
        }
#endif
  }

#if 0
    virtual void udfs(Frame *f)
        {
            tEnergySumAccumulator *pesa = &f->mEnergyInfo.mEnergySumAccumulator;
            pesa->mBondEnergy = 
                f->mUDFs[ UDF_Binding::StdHarmonicBondForce_Code ].getSum();

            pesa->mAngleEnergy = 
                f->mUDFs[ UDF_Binding::StdHarmonicAngleForce_Code ].getSum();

            pesa->mUBEnergy =
                f->mUDFs[ UDF_Binding::UreyBradleyForce_Code ].getSum();

            pesa->mTorsionEnergy =
                f->mUDFs[ UDF_Binding::SwopeTorsionForce_Code ].getSum() +
                f->mUDFs[ UDF_Binding::OPLSTorsionForce_Code ].getSum();

            pesa->mImproperEnergy =
                f->mUDFs[ UDF_Binding::OPLSImproperForce_Code ].getSum() +
                f->mUDFs[ UDF_Binding::ImproperDihedralForce_Code ].getSum() ;

            pesa->mChargeEnergy =
                f->mUDFs[ UDF_Binding::StdChargeForce_Code ].getSum();

            pesa->mLennardJonesEnergy =
                f->mUDFs[ UDF_Binding::LennardJonesForce_Code ].getSum();

            pesa->mKineticEnergy = 
                f->mUDFs[ UDF_Binding::KineticEnergy_Code ].getSum();

            pesa->mWaterEnergy = 
                f->mUDFs[ UDF_Binding::TIP3PForce_Code ].getSum();

            f->mEnergyInfo.mEnergySums.mIntraE =
                pesa->mBondEnergy +
                pesa->mAngleEnergy +
                pesa->mUBEnergy +
                pesa->mTorsionEnergy +
                pesa->mImproperEnergy +
                pesa->mWaterEnergy;

            f->mEnergyInfo.mEnergySums.mInterE =
                pesa->mChargeEnergy +
                pesa->mLennardJonesEnergy;

            if (f->mNSites > 0)
                f->mEnergyInfo.mEnergySums.mTemp = f->mEnergyInfo.mEnergySumAccumulator.mKineticEnergy/1.5/f->mNSites/SciConst::KBoltzmann_IU;
            else
                f->mEnergyInfo.mEnergySums.mTemp = 0;

            f->mEnergyInfo.mEnergySums.mTotalE = f->mEnergyInfo.mEnergySums.mInterE + f->mEnergyInfo.mEnergySums.mIntraE + pesa->mKineticEnergy;

            pesa->mFullTimeStep = f->mFullTimeStep;

            ReportTotalEnergy(&f->mEnergyInfo);
        }
#endif

};

#endif
