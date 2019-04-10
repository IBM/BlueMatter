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
 #ifndef ORBSIMPLE_HPP
#define ORBSIMPLE_HPP

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/DataReceiverSiteLoader.hpp>
#include <BlueMatter/SimpleVoxBox.hpp>

using namespace std;
// orb is array of bb's with implicit index
// 

class FragmentSimple
{
public:
    XYZ *mPos;
    double *mSiteCharge;
    double mNetCharge;
    int mType;
    int mParentMoleculeID;
    int mLocalID;
    int mIsWater;
    int mStartingIndex;
    int mCount;
    int mSiteIDs[4];  // needed for exclusion check
    XYZ mCenter;
    XYZ mDipoleVector;
    double mDipoleMoment;
    double mRadius;
    double mCOGRadius;
    double mBoxRadius;

    FragmentSimple()
    {
        mStartingIndex = 0;
        mCount = 0;
        mType = 0;
        mParentMoleculeID = 0;
        mLocalID = 0;
        mIsWater = 0;
        mPos = NULL;
        mSiteCharge = NULL;
        mNetCharge = 0;
        mRadius = 0;
    }

    FragmentSimple(int aCurrent, int aNext, int aType, int aMolID, int aFragID, int watType)
    {
        mStartingIndex = aCurrent;
        mCount = aNext-aCurrent;
        mType = aType;
        mParentMoleculeID = aMolID;
        mLocalID = aFragID;
        mIsWater = (mType == watType ? 1 : 0);
        mNetCharge = 0;
        mPos = NULL;
        mSiteCharge = NULL;
        mRadius = 0;
    }

    void AssignPositions(XYZ *pos)
    {
        mPos = &pos[mStartingIndex];
    }

    void FindCharges(double *charge)
    {
        mSiteCharge = &charge[mStartingIndex];
        mNetCharge = 0;
        for (int i=0; i<mCount; i++)
            mNetCharge += mSiteCharge[i];
    }

    void FindDipole()
    {
        // echarge*1Angs = 4.803 D
        // So, this answer is in Debye since charge does not include charge conversion
        mDipoleVector = XYZ::ZERO_VALUE();
        double meancharge = mNetCharge/mCount;
        if (mCount > 1)
            for (int i=0; i<mCount; i++)
                mDipoleVector += (mSiteCharge[i]-meancharge)*mPos[i]*4.803;
        mDipoleMoment = mDipoleVector.Length();
    }

    void FindGeometry()
    {
        XYZ v[4];
        double l[4];
        double rsq;
        double tmp;

        switch(mCount) {
            case 1: 
                mCenter = *mPos;
                mRadius = 0;
                break;
            case 2:
                {
                    XYZ d = mPos[1]-mPos[0];
                    mRadius = 0.5*d.Length();
                    mCenter = 0.5*(mPos[0] + mPos[1]);
                    break;
                }
            case 3:
                {
                    v[0] = mPos[1]-mPos[0];
                    v[1] = mPos[2]-mPos[1];
                    v[2] = mPos[0]-mPos[2];
                    l[0] = v[0].Length();
                    l[1] = v[1].Length();
                    l[2] = v[2].Length();
                    int mx = 0;
                    if (l[1]>l[mx])
                        mx = 1;
                    if (l[2]>l[mx])
                        mx = 2;
                    mRadius = l[mx]/2;
                    int k = (mx+1)%3;
                    mCenter = 0.5*(mPos[mx]+mPos[k]);
                    int other = (mx+2)%3;
                    double otherRadius = (mPos[other]-mCenter).Length();
                    if (otherRadius <= mRadius)
                        break;
                    mCenter = (mPos[0]+mPos[1]+mPos[2])/3.0;
                    rsq = (mPos[0]-mCenter).LengthSquared();
                    tmp = (mPos[1]-mCenter).LengthSquared();
                    if (tmp>rsq)
                        rsq = tmp;
                    tmp = (mPos[2]-mCenter).LengthSquared();
                    if (tmp>rsq)
                        rsq = tmp;
                    mRadius = sqrt(rsq);
                    break;
                }

            case 4:
                {
                    mCenter = 0.25*(mPos[0]+mPos[1]+mPos[2]+mPos[3]);
                    rsq = (mPos[0]-mCenter).LengthSquared();
                    tmp = (mPos[1]-mCenter).LengthSquared();
                    if (tmp>rsq)
                        rsq = tmp;
                    tmp = (mPos[2]-mCenter).LengthSquared();
                    if (tmp>rsq)
                        rsq = tmp;
                    tmp = (mPos[3]-mCenter).LengthSquared();
                    if (tmp>rsq)
                        rsq = tmp;
                    mRadius = sqrt(rsq);
                    break;
                }

            default:
                printf("bad fragment size\n");
                exit(-1);
                break;
        }

        mCOGRadius = 0;
        mBoxRadius = 0;
        if (mCount > 1) {
            XYZ cog = XYZ::ZERO_VALUE();
            for (int i=0; i<mCount; i++)
                cog += mPos[i];
            cog = cog/mCount;
            BoundingBox bb;
            bb.Init(mPos[0]);
            for (int i=1; i<mCount; i++)
                bb.Update(mPos[i]);
            XYZ cbox = bb.GetCenter();
            for (int i=0; i<mCount; i++) {
                double tmp = (mPos[i]-cog).LengthSquared();
                if (tmp > mCOGRadius)
                    mCOGRadius = tmp;
                tmp = (mPos[i]-cbox).LengthSquared();
                if (tmp > mBoxRadius)
                    mBoxRadius = tmp;
            }
            mCOGRadius = sqrt(mCOGRadius);
            mBoxRadius = sqrt(mBoxRadius);
            mCenter = cbox;  // set the frag center to box center
        }
        return;
    }
};

class FragmentSimpleList
{
public:
    vector<FragmentSimple> mFragments;
    vector<int> mExternalToInternalSiteMap;
    int mNFragments;
    int mNAtoms;
    XYZ *mPos;
    double *mCharge;
    enum {BUFFSIZE=4096};
    char mBuff[BUFFSIZE];
    int mFragmentTypeCount[4];
    int mNWaters;
    double mFragmentMinRadius[4];
    double mFragmentMaxRadius[4];
    double mFragmentMeanRadius[4];
    double mFragmentSDRadius[4];
    int mNTotalExcluded;
    int mMaxDelta;
    double mMeanDelta;
    double mSigmaDelta;
    enum {NDELTAHISTO=6};
    int mDeltaHisto[NDELTAHISTO];
    int mDeltaValue[NDELTAHISTO];

    class ExclusionList {
    public:
        enum {MAXEXCLUSION=32};
        int mExcluded[MAXEXCLUSION];
        int mNExcluded;

        int Excluded(int b) {
            for (int i=0; i<mNExcluded; i++)
                if (mExcluded[i] == b)
                    return 1;
            return 0;
        }

        void AddExclusion(int b) {
            if (mNExcluded >= MAXEXCLUSION) {
                printf("too many exclusions\n");
                exit(-1);
            }
            mExcluded[mNExcluded++] = b;
        }
    };

    ExclusionList *mExclusionList;

    void FindExclusionStats()
    {
        double sum=0;
        double sumsq=0;
        mNTotalExcluded=0;
        mMaxDelta = 0;

        this->mDeltaValue[0] = 4;
        mDeltaValue[1] = 8;
        mDeltaValue[2] = 16;
        mDeltaValue[3] = 32;
        mDeltaValue[4] = 1024;
        mDeltaValue[5] = 999999;

        for (int i=0; i<NDELTAHISTO; i++)
            mDeltaHisto[i] = 0;

        for (int i=0; i<mNAtoms; i++) {
            for (int j=0; j<mExclusionList[i].mNExcluded; j++) {
                mNTotalExcluded++;
                int delta = abs(mExclusionList[i].mExcluded[j]-i);
                sum += delta;
                sumsq += delta*delta;
                if (mMaxDelta < delta)
                    mMaxDelta = delta;
                for (int d=0; d<NDELTAHISTO; d++) {
                    if (delta < mDeltaValue[d]) {
                        this->mDeltaHisto[d]++;
                        break;
                    }
                }
            }
        }
        mMeanDelta = sum/mNTotalExcluded;
        mSigmaDelta = sqrt(sumsq/mNTotalExcluded - mMeanDelta*mMeanDelta);

    }

    int IsExcluded(int a, int b)
    {
        if (a < b) {
            printf("exclusion check on sites with bad order\n");
            exit(-1);
        }
        return mExclusionList[a].Excluded(b);
    }

    void AssignPositions(XYZ *pos)
    {
        for (vector<FragmentSimple>::iterator fs = mFragments.begin(); fs != mFragments.end(); fs++)
            fs->AssignPositions(pos);
    }

    void FindGeometry()
    {
        for (vector<FragmentSimple>::iterator fs = mFragments.begin(); fs != mFragments.end(); fs++)
            fs->FindGeometry();
    }

    void FindCharges()
    {
        for (vector<FragmentSimple>::iterator fs = mFragments.begin(); fs != mFragments.end(); fs++)
            fs->FindCharges(mCharge);
    }

    void FindDipoles()
    {
        for (vector<FragmentSimple>::iterator fs = mFragments.begin(); fs != mFragments.end(); fs++)
            fs->FindDipole();
    }

    void FindStats()
    {
        double sumsq[4];
        double sum[4];
        for (int i=0; i<4; i++) {
            mFragmentTypeCount[i] = 0;
            mNWaters = 0;
            mFragmentMinRadius[i] = 100000;
            mFragmentMaxRadius[i] = -1000000;
            sumsq[i] = 0;
            sum[i] = 0;
        }
        for (vector<FragmentSimple>::iterator fs = mFragments.begin(); fs != mFragments.end(); fs++) {
            int n = fs->mCount-1;
            double r = fs->mBoxRadius;
            mFragmentTypeCount[n]++;
            if (fs->mIsWater)
                mNWaters++;
            if (r < mFragmentMinRadius[n])
                mFragmentMinRadius[n] = r;
            if (r > mFragmentMaxRadius[n])
                mFragmentMaxRadius[n] = r;
            sumsq[n] += r*r;
            sum[n] += r;
        }
        for (int i=0; i<4; i++) {
            if (mFragmentTypeCount[i] == 0)
                mFragmentTypeCount[i] = 1;
            int n = mFragmentTypeCount[i];
            mFragmentMeanRadius[i] = sum[i]/n;
            mFragmentSDRadius[i] = sqrt(sumsq[i]/n-mFragmentMeanRadius[i]*mFragmentMeanRadius[i]);
        }
    }

    int SourceIDFromPairID(int n)
    {
        switch(n) {
            case 0:
                return 0;
                break;
            case 1:
            case 2:
                return 1;
                break;
            case 3:
            case 4:
            case 5:
            case 6:
                return 2;
                break;
            default:
                return 3;
                break;
        }
        return 3;
    }

// /*NONE*/ { 3 ,  MSD::WATER, 0 , 3, 1.1 * 0.9572 }
// };
// const int MSD::IrreduciblePartitionTypeListSize = 8055;

// // {start_site , ip_type, molecule_id, fragments_index_in_molecule}
// const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] =
// {
// {0, 0, 0, 0},
// {4, 1, 0, 1},
// {6, 2, 0, 2},
// :
// {43216, 8054, 7552, 0},
// {43219, 8054, 7553, 0}
// };
#define FRAGMAPSTRING "const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] ="
    void InitFromMSD(char *fname)
    {
        int watid = -1;
        FILE *f = fopen(fname, "ra");
        if (!f) {
            // cerr << "error reading " << fname << endl;
            exit(-1);
        }
        printf("Opened MSD %s\n", fname);
        while (!feof(f)) {
            fgets(mBuff, BUFFSIZE, f);
            if (1 == sscanf(mBuff, "const int MSD::IrreduciblePartitionTypeListSize = %d", &watid))
                break;
        }
        if (watid < 0) {
            printf("water id not found in msd\n");
            exit(-1);
        }
        watid--;
        printf("Water ID %d\n", watid);
        while (!feof(f)) {
            fgets(mBuff, BUFFSIZE, f);
            if (!strncmp(FRAGMAPSTRING, mBuff, strlen(FRAGMAPSTRING)))
                break;
        }
        if (strncmp(FRAGMAPSTRING, mBuff, strlen(FRAGMAPSTRING))) {
            printf("frag map string not found in msd\n");
            exit(-1);
        }
        FragmentSimple fsnext;
        FragmentSimple fscurr;
        int nfrags=0;

        // skip to first entry
        while (4 != (fscanf(f, "{%d, %d, %d, %d},\n", &fscurr.mStartingIndex, &fscurr.mType, &fscurr.mParentMoleculeID, &fscurr.mLocalID)))
            ;

        while (4 == (fscanf(f, "{%d, %d, %d, %d},\n", &fsnext.mStartingIndex, &fsnext.mType, &fsnext.mParentMoleculeID, &fsnext.mLocalID))) {
            mFragments.push_back(FragmentSimple(fscurr.mStartingIndex, fsnext.mStartingIndex, fscurr.mType, fscurr.mParentMoleculeID, fscurr.mLocalID, watid));
            fscurr = fsnext;
            nfrags++;
        }

        // assume it failed reading last one, and shove in a final water.
        mFragments.push_back(FragmentSimple(fscurr.mStartingIndex, fscurr.mStartingIndex+3, fscurr.mType, fscurr.mParentMoleculeID, fscurr.mLocalID, watid));
        printf("NFrags %d\n", mFragments.size());

// const SiteId MSD::ExternalToInternalSiteIdMap[] =
// {
// 0,
// 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,

#define SITEMAPSTRING "const SiteId MSD::ExternalToInternalSiteIdMap[] ="
        while (!feof(f)) {
            fgets(mBuff, BUFFSIZE, f);
            if (!strncmp(SITEMAPSTRING, mBuff, strlen(SITEMAPSTRING)))
                break;
        }
        if (strncmp(SITEMAPSTRING, mBuff, strlen(SITEMAPSTRING))) {
            printf("site map string not found in msd\n");
            exit(-1);
        }
        fgets(mBuff, BUFFSIZE, f);
        int s;
        while (1 == fscanf(f, "%d", &s)) {
            mExternalToInternalSiteMap.push_back(s);
            fscanf(f, ",");
        }
        // if (mExternalToInternalSiteMap.size() != mNAtoms) {
        //     printf("atom count wrong in site map\n");
        //     exit(-1);
        // }
        int count = mExternalToInternalSiteMap.size();

        mNAtoms = count;

        printf("NAtoms %d\n", mNAtoms);

        // load coulomb table
        // load site type and do lookup as loading

        mCharge = new double[mNAtoms];
        mExclusionList = new ExclusionList[mNAtoms];
        enum {MAXCHARGETYPE=1000};
        double ChargeType[MAXCHARGETYPE];

        for (int i=0; i<mNAtoms; i++)
            mExclusionList[i].mNExcluded = 0;

// const UDF_Binding::NSQCoulomb_Params MSD::ChargeNsqParams[] =
// {
// { (.189000*ChargeConversion) },
// { (-.900000E-01*ChargeConversion) },
#define COULOMBTABLESTRING "const UDF_Binding::NSQCoulomb_Params MSD::ChargeNsqParams[] ="
        while (!feof(f)) {
            fgets(mBuff, BUFFSIZE, f);
            if (!strncmp(COULOMBTABLESTRING, mBuff, strlen(COULOMBTABLESTRING)))
                break;
        }
        if (strncmp(COULOMBTABLESTRING, mBuff, strlen(COULOMBTABLESTRING))) {
            printf("site map string not found in msd\n");
            exit(-1);
        }
        fgets(mBuff, BUFFSIZE, f);
        // double q;
        int nq = 0;
        fgets(mBuff, BUFFSIZE, f);
        while (strstr(mBuff, "ChargeConversion")) {
            char *s1, *s2;
            if (s1 = strstr(mBuff, "(")) {
                s1++;
            } else {
                s1 = strstr(mBuff, "{ ");
                s1 += 2;
            }
            s2 = strstr(mBuff, "*");
            *s2 = '\0';
            ChargeType[nq++] = atof(s1);
            fgets(mBuff, BUFFSIZE, f);
        }

        printf("NChargeTypes %d\n", nq);

// const int MSD::NsqSiteIDtoSiteTypeMap[] =
// {
// 103,
// 29,
// 29,

#define SITETYPESTRING "const int MSD::NsqSiteIDtoSiteTypeMap[] ="
        while (!feof(f)) {
            fgets(mBuff, BUFFSIZE, f);
            if (!strncmp(SITETYPESTRING, mBuff, strlen(SITETYPESTRING)))
                break;
        }
        if (strncmp(SITETYPESTRING, mBuff, strlen(SITETYPESTRING))) {
            printf("site map string not found in msd\n");
            exit(-1);
        }
        fgets(mBuff, BUFFSIZE, f);
        int st;
        int na = 0;
        while (1 == fscanf(f, "%d", &st)) {
            mCharge[na++] = ChargeType[st];
            fscanf(f, ",");
        }

        printf("NCharges %d\n", na);
        if (na != mNAtoms) {
            printf("NAtoms != NCharges\n");
            exit(-1);
        }

        // load exclusion list and convert

// const SiteId MSD::ExclusionList1_2_3_4[] =
// {
// 0 , 1,
// 0 , 2,
// 0 , 3,

#define EXCLUSIONLISTSTRING "const SiteId MSD::ExclusionList1_2_3_4[] ="
        while (!feof(f)) {
            fgets(mBuff, BUFFSIZE, f);
            if (!strncmp(EXCLUSIONLISTSTRING, mBuff, strlen(EXCLUSIONLISTSTRING)))
                break;
        }
        if (strncmp(EXCLUSIONLISTSTRING, mBuff, strlen(EXCLUSIONLISTSTRING))) {
            printf("exclusion list not found in msd\n");
            exit(-1);
        }
        fgets(mBuff, BUFFSIZE, f);
        int a, b;
        int nexc = 0;
        while (2 == fscanf(f, "%d , %d", &a, &b)) {
            if (a > b) {
                int tmp = a;
                a = b;
                b = tmp;
            }
            mExclusionList[a].AddExclusion(b);
            fscanf(f, ",");
            nexc++;
        }

        this->FindExclusionStats();

        printf("NExclusions %d\n", this->mNTotalExcluded);
        printf("MeanDelta %lg\n", this->mMeanDelta);
        printf("SigmaDelta %lg\n", this->mSigmaDelta);
        printf("MaxDelta %d\n", this->mMaxDelta);
        for (int i=0; i<NDELTAHISTO; i++)
            printf("NExcludedBelow %d : %d\n", mDeltaValue[i], mDeltaHisto[i]);
        printf("FractionExcluded %lg\n", double(mNTotalExcluded)/(mNAtoms*double(mNAtoms-1.0)*0.5));

        // FindStats();

        fclose(f);
    }

    void LoadPositionsTxt(char *fname)
    {
        mPos = new XYZ[150000];
        FILE *f = fopen(fname, "ra");
        if (!f) {
            printf("Failed to load positions file %s\n", fname);
            exit(-1);
        }
        printf("Loading positions from %s\n", fname);
        mNAtoms = 0;
        int insite = mExternalToInternalSiteMap[mNAtoms];
        while (3 == fscanf(f, "%lf %lf %lf", &mPos[insite].mX, &mPos[insite].mY, &mPos[insite].mZ)) {
            mNAtoms++;
            insite = mExternalToInternalSiteMap[mNAtoms];
        }
        fclose(f);
        if (mNAtoms < 1) {
            printf("failed to load atoms\n");
            exit(-1);
        }
        printf("NPositions %d\n", mNAtoms);
        AssignPositions(mPos);
    }

    void LoadPositionsDVS(char *fname, XYZ &range)
    {
  SimulationParser sp = SimulationParser(fname, 0, 0);
  DataReceiverSiteLoader drdvs;
  sp.addReceiver(&drdvs);
        printf("Loading positions from %s\n", fname);
  sp.init();
  sp.run();
  sp.final();

  mNAtoms = drdvs.NSites;

        if (mNAtoms < 1) {
            printf("failed to load atoms\n");
            exit(-1);
        }

  XYZ box = drdvs.Box.GetDimensionVector();
  if (box.mX > 1.0E-6)
    range = box;

  mPos = new XYZ[mNAtoms];

        for (int i=0; i<mNAtoms; i++) {
    int insite = mExternalToInternalSiteMap[i];
    mPos[insite] = drdvs.Sites[i].mPosition;
        }

        printf("NPositions %d\n", mNAtoms);
        AssignPositions(mPos);
    }
};

class OrbSimple
{
public:
    int mNCells;
    enum {NGrid=50};
    enum {NMaxCells = 65536};
    int mGrid[NGrid][NGrid][NGrid];
    BoundingBox mCells[NMaxCells];
    BoundingBox mSystemBox;
    XYZ mSystemRange;
    enum {NHISTOBINS = 200};
    int mAttractiveCounts[NHISTOBINS];
    int mRepulsiveCounts[NHISTOBINS];
    int mNeutralCounts[NHISTOBINS];
    int mNAttractive;
    int mNRepulsive;
    int mNNeutral;
    int mNInteractions;
    bool mUseOrb;

    void InitBox(double l, double w, double h)
    {
        mNCells = 0;
        mSystemRange.mX = l;
        mSystemRange.mY = w;
        mSystemRange.mZ = h;
        mSystemBox.mMinBox = XYZ::ZERO_VALUE();
        mSystemBox.mMaxBox = mSystemRange;
        mUseOrb = false;
    }

    void Init(char *fname)
    {
        mUseOrb = true;
        FILE *f = fopen(fname, "r");
        if (!f) {
            // cerr << "error reading " << fname << endl;
            exit(-1);
        }
        printf("Loading ORB %s\n", fname);
        for (int j=0; j<NHISTOBINS; j++)
            mAttractiveCounts[j] = mRepulsiveCounts[j] = mNeutralCounts[j] = 0;
        mNAttractive = mNRepulsive = mNNeutral = 0;
        mNInteractions = 0;

        mNCells = 0;
        int d0, d1, d2;
        double w;
        while (10 == fscanf(f, "%lf %lf %lf %lf %lf %lf %d %d %d %lf",
            &mCells[mNCells].mMinBox.mX, &mCells[mNCells].mMinBox.mY,&mCells[mNCells].mMinBox.mZ, 
            &mCells[mNCells].mMaxBox.mX, &mCells[mNCells].mMaxBox.mY,&mCells[mNCells].mMaxBox.mZ, 
            &d0, &d1, &d2, &w))
        {
            mNCells++;
        }
        fclose(f);

        mSystemBox.Init(mCells[0].mMinBox);
        for (int c=0; c<mNCells; c++) {
            mSystemBox.Update(mCells[c].mMinBox);
            mSystemBox.Update(mCells[c].mMaxBox);
        }

        for (int i=0; i<NGrid; i++)
            for (int j=0; j<NGrid; j++)
                for (int k=0; k<NGrid; k++)
                    mGrid[i][j][k] = -1;

        mSystemRange = mSystemBox.GetDimensionVector();

        printf("NOrbCells %d\n", mNCells);
        printf("BoxExtent %lf %lf %lf\n", mSystemRange.mX, mSystemRange.mY, mSystemRange.mZ);

        for (int c=0; c<mNCells; c++)
        {
            // printf("cell %d\n", c);
            XYZ p;
            for (int i=0; i<NGrid; i++)
            {
                p.mX = mSystemBox.mMinBox.mX + mSystemRange.mX*double(i)/NGrid;
                for (int j=0; j<NGrid; j++) {
                    p.mY = mSystemBox.mMinBox.mY + mSystemRange.mY*double(j)/NGrid;
                    int hit = 0;
                    for (int k=0; k<NGrid; k++)
                    {
                        p.mZ = mSystemBox.mMinBox.mZ + mSystemRange.mZ*double(k)/NGrid;
                        if (mCells[c].ContainsLeft(p)) {
                            mGrid[i][j][k] = c;
                            hit = 1;
                        } else {
                            if (hit)
                               break;
                        }
                    }
                }
            }
        }

        for (int i=0; i<NGrid; i++)
            for (int j=0; j<NGrid; j++)
                for (int k=0; k<NGrid; k++)
                    if (mGrid[i][j][k] == -1)
                        printf("Bad grid point %d %d %d\n", i, j, k);


    }

    int GetCell(const XYZ &p)
    {
        if (!mUseOrb)
            return 0;

        int i = (p.mX-mSystemBox.mMinBox.mX)/mSystemRange.mX*NGrid;
        int j = (p.mY-mSystemBox.mMinBox.mY)/mSystemRange.mY*NGrid;
        int k = (p.mZ-mSystemBox.mMinBox.mZ)/mSystemRange.mZ*NGrid;

        int fail = 0;
        if (i >= NGrid)
            fail=1;
        if (j >= NGrid)
            fail=1;
        if (k >= NGrid)
            fail=1;
        if (i < 0)
            fail=1;
        if (j < 0)
            fail=1;
        if (k < 0)
            fail=1;
        if (fail) {
            printf("Error gridding point %lf %lf %lf\n", p.mX, p.mY, p.mZ);
            exit(-1);
        }
        int c = mGrid[i][j][k];
        if (mCells[c].ContainsLeft(p))
            return c;

        for (int n=0; n<mNCells; n++)
            if (mCells[n].ContainsLeft(p))
                return n;
        printf("Error finding point %lf %lf %lf\n", p.mX, p.mY, p.mZ);
        // return 0;
        exit(-1);
        return -1;
    }

    void GetPairFraction(const FragmentSimple *a, const FragmentSimple *b, const double lowercutoffsq, const double uppercutoffsq, int &ndirect, int &nout)
    {

        ndirect = 0;
        nout = 0;
        for (int i=0; i<a->mCount; i++)
            for (int j=0; j<b->mCount; j++) {
                XYZ r = Imaging::NearestImageDisplacement(a->mPos[i], b->mPos[j], mSystemRange);
                double rsq = r.LengthSquared();
                if (rsq < lowercutoffsq)
                    ndirect++;
                if (rsq > uppercutoffsq)
                    nout++;
            }
    }

    int PairType(const FragmentSimple *a, const FragmentSimple *b)
    {
        int n1 = a->mCount;
        int n2 = b->mCount;
        if (n1 < n2) {
            int tmp = n1;
            n1 = n2;
            n2 = tmp;
        }
        switch(n1) {
            case 1:
                return 0;
            case 2:
                if (n2 == 1)
                    return 1;
                else
                    return 2;
            case 3:
                switch(n2) {
                    case 1: return 3;
                    case 2: return 4;
                    case 3:
                        if (a->mParentMoleculeID == b->mParentMoleculeID)
                            return 5;  // This is intra (slower)
                        else
                            return 6;  // This is inter (faster)
                }
            case 4:
                return 6+n2;
        }
        printf("pair type error\n");
        exit(-1);
  return -1;
    }

    double GetCost(int pairtype)
    {
        switch(pairtype) {
            case 0:
                return 1.048728;
            case 1:
                return 1.608737;
            case 2:
                return 2.728592;
            case 3:
                return 2.044509;
            case 4:
                return 3.670676;
            case 5:
                return 4.448243;
            case 6:
                return 3.462162;
            case 7:
                return 2.949303;
            case 8:
                return 4.426865;
            case 9:
                return 6.332759;
            case 10:
                return 5.655414;
        }
        printf("error in pair cost for %d\n", pairtype);
        exit(-1);
        return -1;
    }

    // if (mOrb.GetInteraction(&(*f1), &(*f2), mSwitchLowerCutoff, mSwitchUpperCutoff, mGuardZone, interactiontype, ndirect, nswitch, nout,
    //      pairtype, cellid, midpt, nwaters)) {


    // interaction of fragments with known radii and no assumption of separation
    int GetInteraction(const FragmentSimple *a, const FragmentSimple *b, const double lowercutoff, const double uppercutoff, const double guardzone,
                       const double rdmax, int &interactiontype, int &ndirect, int &nswitch, int &nout,
                       int &pairtype, int &cellid, XYZ &midpt, int &nwaters, double &separation, int &nexclusion)
    {
        XYZ r = Imaging::NearestImageDisplacement(a->mCenter, b->mCenter, mSystemRange);
        double radsum = a->mRadius + b->mRadius;
        double rgz = uppercutoff + guardzone;
        double mindist = (rgz + radsum);
        double mindistsq = mindist*mindist;
        double rsq = r.LengthSquared();
        nexclusion = 0;
        if (rsq >= (rdmax+5)*(rdmax+5))
            return 0;

        // build sitepair histo in here
        for (int i=0; i<a->mCount; i++) {
            double qa = a->mSiteCharge[i];
            for (int j=0; j<b->mCount; j++) {
                double qb = b->mSiteCharge[j];
                double chargeprod = qa*qb;
                XYZ rab = Imaging::NearestImageDisplacement(a->mPos[i], b->mPos[j], mSystemRange);
                double d = rab.Length();
                int bin = d/rdmax*NHISTOBINS;
                if (bin >= NHISTOBINS)
                    continue;
                if (chargeprod>0)
                    mRepulsiveCounts[bin]++;
                else if (chargeprod < 0)
                    mAttractiveCounts[bin]++;
                else
                    mNeutralCounts[bin]++;
            }
        }

        // if gets here, it is to be counted for radial distro stats only

        separation = sqrt(rsq);
        if (rsq > mindistsq)
            return 1;

        // if gets here, the pairs are in verlet list, but may still be out of range

        int npairs = a->mCount*b->mCount;
        midpt = Imaging::MapIntoBlueMatterBox((a->mCenter+0.5*r), mSystemRange);
        cellid = GetCell(midpt);
        nwaters = a->mIsWater + b->mIsWater;
        pairtype = PairType(a, b);

        // check if all outside cutoff

        mindist = uppercutoff + radsum;
        mindistsq = mindist*mindist;
        if (rsq >= mindistsq) {
            interactiontype = 2;
            ndirect = 0;
            nswitch = 0;
            nout = npairs;
            return 2;
        }

        // check if all inside inner cutoff

        mindist = lowercutoff - radsum;
        mindistsq = mindist*mindist;
        if (rsq <= mindistsq) {
            interactiontype = 0;
            ndirect = npairs;
            nswitch = 0;
            nout = 0;
            if (a->mParentMoleculeID == b->mParentMoleculeID)
                nexclusion = npairs;
            return 3;
        }

        // general case with some possibly switched
        double lcsq = lowercutoff*lowercutoff;
        double ucsq = uppercutoff*uppercutoff;
        GetPairFraction(a, b, lowercutoff*lowercutoff, uppercutoff*uppercutoff, ndirect, nout);
        nswitch = npairs - ndirect - nout;
        interactiontype = 1;
        if (a->mParentMoleculeID == b->mParentMoleculeID)
            nexclusion = npairs - nout;
        return 4;
    }

};

class OrbRun
{
public:
    OrbSimple mOrb;
    FragmentSimpleList mFragments;
    char mMSDName[512];
    char mOrbName[512];
    char mDVSName[512];
    char mOutStem[512];
    int mSourceID;
    int mSystemID;
    int mCTPID;
    int mIMPLID;
    int mPLTID;
    char mShortName[512];
    double mSwitchLowerCutoff;
    double mSwitchUpperCutoff;
    double mGuardZone;
    double mSwitchDelta;
    double mRDMax;

    class InteractionTracker
    {
    public:
        int mNInVerlet;
        int mNOutsideCutoff;
        int mNInSwitch;
        int mNWithinSwitch;
        int mNExclusionCheck;

        InteractionTracker()
        {
            mNInVerlet = 0;
            mNOutsideCutoff = 0;
            mNInSwitch = 0;
            mNWithinSwitch = 0;
            mNExclusionCheck = 0;
        }
    };

    InteractionTracker mFragmentTracker, mSiteTracker;

    OrbRun()
    {
    }

    void Init(char *msdname, char *dvsname, char *outstem, char *shortname, char *orbname, double slc=9.0, double sd=1.0, double gz = 1.0, double l=0, double w=0, double h=0)
    {
        strcpy(mMSDName, msdname);
        strcpy(mOrbName, orbname);
        strcpy(mDVSName, dvsname);
        strcpy(mOutStem, outstem);
        strcpy(mShortName, shortname);

        if (!strlen(mOrbName)) {
            mOrb.InitBox(l, w, h);
        } else {
            mOrb.Init(mOrbName);
        }

        mFragments.InitFromMSD(mMSDName);
        // mFragments.LoadPositionsTxt(mDVSName);
        mFragments.LoadPositionsDVS(mDVSName, mOrb.mSystemRange);
        mFragments.FindGeometry();
        mFragments.FindStats();
        mFragments.FindCharges();
        mFragments.FindDipoles();

        mSwitchLowerCutoff = slc;
        mSwitchDelta = sd;
        mSwitchUpperCutoff = mSwitchLowerCutoff+mSwitchDelta;
        mGuardZone = gz;
        mRDMax = 20;
    }

    void Dump()
    {
        char outname[512];
        sprintf(outname, "%s.frag.txt", mOutStem);
        FILE *f = fopen(outname, "wa");
        fprintf(f, "StartIndex X Y Z Count Radius BoxRadius COGRadius IsWater Charge Dipole\n");
        for (vector<FragmentSimple>::iterator fs = mFragments.mFragments.begin(); fs != mFragments.mFragments.end(); fs++) {
            XYZ fragcent = Imaging::MapIntoBlueMatterBox(fs->mCenter, this->mOrb.mSystemRange);
            fprintf(f, "%d %lf %lf %lf %d %lf %lf %lf %d %lf %lf\n", fs->mStartingIndex, fragcent.mX, fragcent.mY, fragcent.mZ,
                fs->mCount, fs->mRadius, fs->mBoxRadius, fs->mCOGRadius, fs->mIsWater, fs->mNetCharge, fs->mDipoleMoment);
        }
        fclose(f);

        double vol = mOrb.mSystemRange.mX*mOrb.mSystemRange.mY*mOrb.mSystemRange.mZ;
        sprintf(outname, "%s.log", mOutStem);
        f = fopen(outname, "wa");
        fprintf(f, "MSD %s\n", this->mMSDName);
        fprintf(f, "DVS %s\n", this->mDVSName);
        if (strlen(mOrbName))
            fprintf(f, "ORB %s\n", this->mOrbName);
        else
            fprintf(f, "ORB None\n");
        fprintf(f, "SHORTNAME %s\n", mShortName);
        fprintf(f, "BOX %lf %lf %lf\n", this->mOrb.mSystemRange.mX, mOrb.mSystemRange.mY, mOrb.mSystemRange.mZ);
        fprintf(f, "CUTOFFS %lf %lf %lf\n", this->mSwitchLowerCutoff, mSwitchUpperCutoff, mSwitchUpperCutoff+mGuardZone);
        fprintf(f, "NATOMS %d\n", mFragments.mNAtoms);
        fprintf(f, "ATOMDENSITY %lg\n", mFragments.mNAtoms/vol);
        fprintf(f, "NFRAGMENTS %d\n", mFragments.mFragments.size());
        fprintf(f, "NWATERS %d\n", mFragments.mNWaters);
        fprintf(f, "FRAGDENSITY %lf\n", mFragments.mFragments.size()/vol);
        fprintf(f, "FRAGINTERACTIONDENSITY %lf\n", mFragmentTracker.mNInVerlet/vol);
        fprintf(f, "SITEINTERACTIONDENSITY %lf\n", mSiteTracker.mNInVerlet/vol);
        fprintf(f, "InsideSwitch Switching Outside InVerlet Exclusion\n");
        fprintf(f, "FRAGCUTOFFS %d %d %d %d %d\n", mFragmentTracker.mNWithinSwitch, mFragmentTracker.mNInSwitch, mFragmentTracker.mNOutsideCutoff, mFragmentTracker.mNInVerlet, mFragmentTracker.mNExclusionCheck);
        fprintf(f, "SITECUTOFFS %d %d %d %d %d\n", mSiteTracker.mNWithinSwitch, mSiteTracker.mNInSwitch, mSiteTracker.mNOutsideCutoff, mSiteTracker.mNInVerlet, mSiteTracker.mNExclusionCheck);
        // count density meanrad sd min max
        for (int i=0; i<4; i++) {
            fprintf(f, "FRAGSTATS%d %d %lf %lf %lf %lf %lf\n", i+1, mFragments.mFragmentTypeCount[i], mFragments.mFragmentTypeCount[i]/vol,
                mFragments.mFragmentMeanRadius[i], mFragments.mFragmentSDRadius[i], mFragments.mFragmentMinRadius[i], mFragments.mFragmentMaxRadius[i]);
        }

        fprintf(f, "NExclusions %d\n", mFragments.mNTotalExcluded);
        fprintf(f, "MeanDelta %lg\n", mFragments.mMeanDelta);
        fprintf(f, "SigmaDelta %lg\n", mFragments.mSigmaDelta);
        fprintf(f, "MaxDelta %d\n", mFragments.mMaxDelta);
        for (int i=0; i<mFragments.NDELTAHISTO; i++)
            fprintf(f, "NExcludedBelow_%d : %d\n", mFragments.mDeltaValue[i], mFragments.mDeltaHisto[i]);
        fprintf(f, "FractionExcluded %lg\n", double(mFragments.mNTotalExcluded)/(mFragments.mNAtoms*double(mFragments.mNAtoms-1.0)*0.5));



        fclose(f);
    }

    // two loops over frags
    void DumpFragmentPairs()
    {
        char outname[512];
        sprintf(outname, "%s.frag_pairs.txt", mOutStem);
        FILE *f = fopen(outname, "wa");
        int FragAttractiveCounts[11][OrbSimple::NHISTOBINS];
        int FragRepulsiveCounts[11][OrbSimple::NHISTOBINS];
        int FragNeutralCounts[11][OrbSimple::NHISTOBINS];

        int DipoleCorrCounts[OrbSimple::NHISTOBINS];
        double DipoleCorr[OrbSimple::NHISTOBINS];
        double DipoleCorrAbs[OrbSimple::NHISTOBINS];

        for (int i=0; i<OrbSimple::NHISTOBINS; i++) {
            DipoleCorrCounts[i] = 0;
            DipoleCorr[i] = 0;
            DipoleCorrAbs[i] = 0;
        }

#if 1
        for (int i=0; i<11; i++) {
            for (int j=0; j<OrbSimple::NHISTOBINS; j++) {
                FragAttractiveCounts[i][j] = 0;
                FragRepulsiveCounts[i][j] = 0;
                FragNeutralCounts[i][j] = 0;
            }
        }
#endif

        fprintf(f, "X Y Z PairType Cost PairCount InteractionType NDirect NSwitch NOut CellID NWaters Separation MolID1 MolID2 Count1 Count2\n");
        int nf1 = 0;
        for (vector<FragmentSimple>::iterator f1 = mFragments.mFragments.begin(); f1 != mFragments.mFragments.end()-1; f1++) {
            int nf2 = 0;
            for (vector<FragmentSimple>::iterator f2 = f1+1; f2 != mFragments.mFragments.end(); f2++) {
                // find dist to nearest image of f2
                // if within cutoff, map mid point into box
                // get orb id of point
                // print x, y, z, pairtype, cost, nsitepairs, fractionwithincutoff, orbid
                // find orb distro of weights and output stats
                // mean, sd, min, max
                // orbid, weight, volume?
                nf2++;

                int pairtype;
                int cellid;
                XYZ midpt;
                int nwaters;
                int interactiontype;
                int ndirect;
                int nswitch;
                int nout;
                double separation;
                int nexclusioncheck;
#if 1
                int rc = mOrb.GetInteraction(&(*f1), &(*f2), mSwitchLowerCutoff, mSwitchUpperCutoff, mGuardZone, mRDMax, interactiontype, ndirect, nswitch, nout,
                        pairtype, cellid, midpt, nwaters, separation, nexclusioncheck);
                if (!rc)
                    continue;

                int bin = separation/mRDMax*OrbSimple::NHISTOBINS;
                if (bin>=OrbSimple::NHISTOBINS)
                    continue;
                double chargeproduct = f1->mNetCharge*f2->mNetCharge;
                if (chargeproduct<0)
                    FragAttractiveCounts[pairtype][bin]++;
                else if (chargeproduct>0)
                    FragRepulsiveCounts[pairtype][bin]++;
                else
                    FragNeutralCounts[pairtype][bin]++;

                if (fabs(f1->mDipoleMoment*f2->mDipoleMoment) > 0.01) {
                    double dprod = f1->mDipoleVector.DotProduct(f2->mDipoleVector)/f1->mDipoleVector.Length()/f2->mDipoleVector.Length();
                    DipoleCorr[bin] += dprod;
                    DipoleCorrAbs[bin] += fabs(dprod);
                    DipoleCorrCounts[bin]++;
                }

                if (rc > 1) {
                    mFragmentTracker.mNInVerlet++;
                    if (f1->mParentMoleculeID == f2->mParentMoleculeID)
                        mFragmentTracker.mNExclusionCheck++;
                    switch(interactiontype) {
                        case 0: this->mFragmentTracker.mNWithinSwitch++; break;
                        case 1: this->mFragmentTracker.mNInSwitch++; break;
                        case 2: this->mFragmentTracker.mNOutsideCutoff++; break;
                        default:
                            printf("Unknown interaction type\n"); exit(-1); break;
                    }
                    mSiteTracker.mNWithinSwitch += ndirect;
                    mSiteTracker.mNInSwitch += nswitch;
                    mSiteTracker.mNOutsideCutoff += nout;
                    mSiteTracker.mNInVerlet += (ndirect + nswitch + nout);
                    mSiteTracker.mNExclusionCheck += nexclusioncheck;

                    fprintf(f, "%lf %lf %lf %d %lf %d %d %d %d %d %d %d %lf %d %d %d %d\n", midpt.mX, midpt.mY, midpt.mZ, pairtype, mOrb.GetCost(pairtype),
                       f1->mCount*f2->mCount, interactiontype, ndirect, nswitch, nout, cellid, nwaters, separation,
           f1->mParentMoleculeID, f2->mParentMoleculeID, f1->mCount, f2->mCount);
                    // fprintf(f, "%lf %lf %lf %lf %d %d %d %d\n", midpt.mX, midpt.mY, midpt.mZ, mOrb.GetCost(pairtype), f1->mParentMoleculeID, f2->mParentMoleculeID, f1->mCount, f2->mCount);
                }
#endif
            }
            nf1++;
        }
        fclose(f);

        char namebuff[512];
        sprintf(namebuff, "%s.site_rdf.dat", mOutStem);
        FILE *frd = fopen(namebuff, "wa");
        fprintf(frd, "r Attractive Repulsive Neutral All\n");
        for (int i=0; i<mOrb.NHISTOBINS; i++) {
            double r = double(i+0.5)/mOrb.NHISTOBINS*mRDMax;
            double v = 4*3.1415926535*r*r*mRDMax/mOrb.NHISTOBINS;
            fprintf(frd, "%lf ", r);
            int all = mOrb.mAttractiveCounts[i]+mOrb.mRepulsiveCounts[i]+mOrb.mNeutralCounts[i];
            fprintf(frd, "%lg %lg %lg %lg", mOrb.mAttractiveCounts[i]/v/mFragments.mNAtoms, mOrb.mRepulsiveCounts[i]/v/mFragments.mNAtoms, mOrb.mNeutralCounts[i]/v/mFragments.mNAtoms, all/v/mFragments.mNAtoms);
            fprintf(frd, "\n");
        }
        fclose(frd);

        sprintf(namebuff, "%s.dipole_corr.dat", mOutStem);
        frd = fopen(namebuff, "wa");
        fprintf(frd, "r DipoleCorr DipoleCorrAbs N\n");
        for (int i=0; i<mOrb.NHISTOBINS; i++) {
            double r = double(i+0.5)/mOrb.NHISTOBINS*mRDMax;
            fprintf(frd, "%lf %lg %lg %d\n", r, DipoleCorr[i]/(DipoleCorrCounts[i]+0.1), DipoleCorrAbs[i]/(DipoleCorrCounts[i]+0.1), DipoleCorrCounts[i]);
        }
        fclose(frd);


        sprintf(namebuff, "%s.frag_rdf_attract.dat", mOutStem);
        frd = fopen(namebuff, "wa");
        fprintf(frd, "r 1_1 2_1 2_2 3_1 3_2 3_3F 3_3S 4_1 4_2 4_3 4_4\n");
        for (int i=0; i<mOrb.NHISTOBINS; i++) {
            double r = double(i+0.5)/mOrb.NHISTOBINS*mRDMax;
            double v = 4*3.1415926535*r*r*mRDMax/mOrb.NHISTOBINS;
            fprintf(frd, "%lf ", r);
            for (int j=0; j<11; j++) {
                fprintf(frd, "%lg ", FragAttractiveCounts[j][i]/v/mFragments.mFragmentTypeCount[mFragments.SourceIDFromPairID(j)]);
            }
            fprintf(frd, "\n");
        }
        fclose(frd);

        sprintf(namebuff, "%s.frag_rdf_repulse.dat", mOutStem);
        frd = fopen(namebuff, "wa");
        fprintf(frd, "r 1_1 2_1 2_2 3_1 3_2 3_3F 3_3S 4_1 4_2 4_3 4_4\n");
        for (int i=0; i<mOrb.NHISTOBINS; i++) {
            double r = double(i+0.5)/mOrb.NHISTOBINS*mRDMax;
            double v = 4*3.1415926535*r*r*mRDMax/mOrb.NHISTOBINS;
            fprintf(frd, "%lf ", r);
            for (int j=0; j<11; j++) {
                fprintf(frd, "%lg ", FragRepulsiveCounts[j][i]/v/mFragments.mFragmentTypeCount[mFragments.SourceIDFromPairID(j)]);
            }
            fprintf(frd, "\n");
        }
        fclose(frd);

        sprintf(namebuff, "%s.frag_rdf_neutral.dat", mOutStem);
        frd = fopen(namebuff, "wa");
        fprintf(frd, "r 1_1 2_1 2_2 3_1 3_2 3_3F 3_3S 4_1 4_2 4_3 4_4\n");
        for (int i=0; i<mOrb.NHISTOBINS; i++) {
            double r = double(i+0.5)/mOrb.NHISTOBINS*mRDMax;
            double v = 4*3.1415926535*r*r*mRDMax/mOrb.NHISTOBINS;
            fprintf(frd, "%lf ", r);
            for (int j=0; j<11; j++) {
                fprintf(frd, "%lg ", FragNeutralCounts[j][i]/v/mFragments.mFragmentTypeCount[mFragments.SourceIDFromPairID(j)]);
            }
            fprintf(frd, "\n");
        }
        fclose(frd);

        sprintf(namebuff, "%s.frag_rdf_all.dat", mOutStem);
        frd = fopen(namebuff, "wa");
        fprintf(frd, "r 1_1 2_1 2_2 3_1 3_2 3_3F 3_3S 4_1 4_2 4_3 4_4\n");
        for (int i=0; i<mOrb.NHISTOBINS; i++) {
            double r = double(i+0.5)/mOrb.NHISTOBINS*mRDMax;
            double v = 4*3.1415926535*r*r*mRDMax/mOrb.NHISTOBINS;
            fprintf(frd, "%lf ", r);
            for (int j=0; j<11; j++) {
                int all = FragNeutralCounts[j][i] + FragRepulsiveCounts[j][i] + FragAttractiveCounts[j][i];
                fprintf(frd, "%lg ", all/v/mFragments.mFragmentTypeCount[mFragments.SourceIDFromPairID(j)]);
            }
            fprintf(frd, "\n");
        }
        fclose(frd);


    }
};

#endif
