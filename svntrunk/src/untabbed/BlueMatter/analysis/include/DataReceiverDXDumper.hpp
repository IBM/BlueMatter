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
 #ifndef DATARECEIVERDXDUMPER_HPP
#define DATARECEIVERDXDUMPER_HPP

#include <ostream>
#include <fstream>
#include <vector>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/DataReceiverTrajectoryDumper.hpp>
#include <BlueMatter/XYZ.hpp>

using namespace std;

class DataReceiverDXDumper : public DataReceiverTrajectoryDumper
{
public:
    fXYZ *m_Positions;
    int m_FirstTime;
    int m_OutFrame;
    int m_NFramesToBuffer;
    float *m_HBEList;


    virtual void init()
    {
  m_Positions = NULL;
  m_FirstTime = 1;
  m_OutFrame = 0;
  m_NFramesToBuffer = 200;
  m_HBEList = NULL;
    }

    virtual void final()
    {
  if (m_Positions)
      delete [] m_Positions;
  if (m_HBEList)
      delete [] m_HBEList;
    }


    float TranslationDistance(float L, float x)
    {
  float a = fabs(x);
  if (a < L/2)
      return 0;
  int n = (a-L/2)/L + 1;
  return (x > 0) ? n*L : -n*L;
    }

    // returns nonzero if no translation
    int TranslationVector(float L, float H, fXYZ &p, fXYZ &v)
    {
  v.mX = TranslationDistance(L, p.mX);
  v.mY = TranslationDistance(L, p.mY);
  v.mZ = TranslationDistance(H, p.mZ);
  return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
    }

    int InRange(float L, float H, fXYZ p, fXYZ t)
    {
  float x;
  if (fabs(p.mX-t.mX)-L/2 > L*0.05)
      return 0;
  if (fabs(p.mY-t.mY)-L/2 > L*0.05)
      return 0;
  if (fabs(p.mZ-t.mZ)-H/2 > H*0.05)
      return 0;
  return 1;
    }

    // returns nonzero if no change in translation
    int TranslationVector(float L, float H, fXYZ &p, fXYZ &v, fXYZ &tvec)
    {

  int inrange = InRange(L, H, p, tvec);

  v.mX = TranslationDistance(L, p.mX);
  v.mY = TranslationDistance(L, p.mY);
  v.mZ = TranslationDistance(H, p.mZ);
  return (inrange || (v.mX == 0 && v.mY == 0 && v.mZ == 0));
    }

    // returns nonzero if no translation
    int TranslationVector(fXYZ &r, float L, fXYZ &p, fXYZ &v)
    {
  v.mX = TranslationDistance(L, p.mX-r.mX);
  v.mY = TranslationDistance(L, p.mY-r.mY);
  v.mZ = TranslationDistance(L, p.mZ-r.mZ);
  return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
    }

    void TranslateIntoBox(float L, float H, fXYZ *p, int k, int count, int tag)
    {
  fXYZ v;
  if (TranslationVector(L, H, p[k+tag], v))
      return;
  for (int i=k; i<k+count; i++) {
      p[i].mX -= v.mX;
      p[i].mY -= v.mY;
      p[i].mZ -= v.mZ;
  }
    }

    void TranslateIntoBox(float L, float H, fXYZ *p, int k, int count, int tag, fXYZ &tvec)
    {
  fXYZ v;
  // if it's not out of range using the old tvec, just reuese it.
  int shifted = TranslationVector(L, H, p[k+tag], v, tvec);
  for (int i=k; i<k+count; i++) {
      p[i].mX -= v.mX;
      p[i].mY -= v.mY;
      p[i].mZ -= v.mZ;
  }
  if (shifted)
      tvec = v;
    }


#define STARTION 256
#define ENDION 258
#define NUMWATERS 1660
#define NUMNONWATERATOMS 259
#define NUMATOMS NUMNONWATERATOMS+3*NUMWATERS
#define BOXWIDTH 38.0

    void MapHairpinIntoBox(fXYZ *proteinsites, fXYZ *watersites)
    {
  // 109 119 133 155
  // 64 85 203 189
  fXYZ shift;
  shift.mX = (proteinsites[64].mX+proteinsites[85].mX+proteinsites[203].mX+proteinsites[189].mX)/4;
  shift.mY = (proteinsites[64].mY+proteinsites[85].mY+proteinsites[203].mY+proteinsites[189].mY)/4;
  shift.mZ = (proteinsites[64].mZ+proteinsites[85].mZ+proteinsites[203].mZ+proteinsites[189].mZ)/4;
  fXYZ *p = proteinsites;
  for (int i=0; i<NUMNONWATERATOMS; i++, p++) {
      p->mX -= shift.mX;
      p->mY -= shift.mY;
      p->mZ -= shift.mZ;
  }

  p = watersites;
  for (int i=0; i<NUMWATERS*3; i++, p++) {
      p->mX -= shift.mX;
      p->mY -= shift.mY;
      p->mZ -= shift.mZ;
  }

  for (int i=0; i<NUMWATERS; i++)
      TranslateIntoBox(BOXWIDTH, BOXWIDTH, watersites, 3*i, 3, 1);
  for (int i=STARTION; i<=ENDION; i++)
      TranslateIntoBox(BOXWIDTH, BOXWIDTH, proteinsites, i, 1, 0);

    }

    // assumes H is N+1 and O is C+1
    struct HBResidue {
  int ID;
  int N;
  int C;
    };

    // fill in the array
    // if native output -e, if nonnative output +e.
    void findHBonds(float *hbe)
    {
#define DSSP_FACTOR (332.0 * 0.42 * 0.2)
#define NHBONDS 14
#define MAXOH 5.0

  int nat,hat,cat,oat;
  static HBResidue res[] =
  {//  resnum, n, c
      {41, 7, 12},
      {42, 14, 27},
      {43, 29, 51},
      {44, 53, 65},
      {45, 67, 86},
      {46, 88, 98},
      {47, 100, 110},
      {48, 112, 120},
      {49, 122, 134},
      {50, 136, 156},
      {51, 158, 170},
      {52, 172, 190},
      {53, 192, 204},
      {54, 206, 220},
      {55, 222, 234},
      {56, 236, 249}
  };

  fXYZ *ph, *po, *pn, *pc, vn, v, dv, rotax;
  fXYZ *pv;
  float doh, don, dch, dcn;
  float theta, phi;
  float bondthickness;
  float maxbonddistance = 4.5;
  float maxenergy = -1.0;

  int nres = sizeof(res)/sizeof(HBResidue);

  for (int i=0; i<nres; i++) {
      int n = res[i].N;
      int h = n+1;
      pn = &m_Positions[n-1];
      ph = &m_Positions[h-1];
      for (int j=0; j<nres; j++) {
    int npair = nres*i+j;
    if (abs(i-j)<3) {
        hbe[npair] = 0;
        continue;
    }
    int c = res[j].C;
    int o = c+1;
    pc = &m_Positions[c-1];
    po = &m_Positions[o-1];

    dv = (*po)-(*ph);
    doh = dv.Length();
    if (doh > maxbonddistance) {
        hbe[npair] = 0;
        continue;
    }
    if (doh<1.0E-10)
        doh = 1.0E-10;
    vn = dv/doh;

    // don = Magnitude(po - pn);
    // dch = Magnitude(pc - ph);
    // dcn = Magnitude(pc - pn);
    don = po->Distance(*pn);
    dch = pc->Distance(*ph);
    dcn = pc->Distance(*pn);

    int native = 0;
    if (i<j) {
        if (48-res[i].ID == res[j].ID-49)
      native = 1;
    } else {
        if (48-res[j].ID == res[i].ID-49)
      native = 1;
    }
    float e = DSSP_FACTOR * (1/don + 1/dch - 1/doh - 1/dcn);
    int isbond = (e < maxenergy) ? 1 : 0;
    if (!isbond) {
        hbe[npair] = 0;
    } else if (native) {
        hbe[npair] = e;
    } else {
        hbe[npair] = -e;
        // bondthickness = -e/10 + 0.05;
    }
      }
  }
  // for (int i=0; i<256; i++)
  //    cout << i << " " << hbe[i] << endl;
    }



    virtual void sites(Frame *f)
    {

  int NumToWrite;
  int NumWritten;

  unsigned CurrentStep = f->mOuterTimeStep;

  tSiteData *ps= f->mSiteData.getArray();
  int NSites = f->mSiteData.getSize();

  if (m_FirstTime) {
      mBMT.m_File = fopen(mBMT.m_FileName, "r+b");
      if (!mBMT.m_File)
    mBMT.m_File = fopen(mBMT.m_FileName, "w");
  } else {
      mBMT.m_File = fopen(mBMT.m_FileName, "r+b");
  }
  while (!mBMT.m_File) {
      // cerr << "BMT output file " << mBMT.m_FileName << " could not be open for binary write/append." << endl;
      // exit(-1);
      usleep(10000);
      mBMT.m_File = fopen(mBMT.m_FileName, "r+b");
  }

  int FrameSize = NSites*3*sizeof(float)+16*16*sizeof(float)+2*sizeof(int);
  int res = fseek(mBMT.m_File, FrameSize*m_OutFrame, SEEK_SET);
  // cout << "just seek to " << FrameSize*m_OutFrame << " return " << res << " outframe " << m_OutFrame << endl;

  if (m_FirstTime) {
      m_Positions = new fXYZ[NSites];
      if (NSites == 5239)
    m_HBEList = new float[16*16];
  }

  for (int i=0; i<NSites; i++) {
      m_Positions[i].mX = ps[i].mPosition.mX;
      m_Positions[i].mY = ps[i].mPosition.mY;
      m_Positions[i].mZ = ps[i].mPosition.mZ;
  }

  if (NSites == 5239) {
      MapHairpinIntoBox(&m_Positions[0], &m_Positions[259]);
      findHBonds(m_HBEList);
  }

  // pos
  // 256 bond energies
  // timestep
  // file index

  fXYZ *psp = m_Positions;
  NumToWrite = sizeof(fXYZ);
  for (int i=0; i< NSites; i++) {
      NumWritten = fwrite((void *)psp, NumToWrite, 1, mBMT.m_File);
      // assert(NumWritten == NumToWrite);
      psp++;
  }
  NumToWrite = sizeof(float);
  float *pf = m_HBEList;
  for (int i=0; i<16*16; i++) {
      NumWritten = fwrite((void *)pf, NumToWrite, 1, mBMT.m_File);
      // assert(NumWritten == NumToWrite);
      pf++;
  }
  NumToWrite = sizeof(int);
  NumWritten = fwrite((void *)&CurrentStep, NumToWrite, 1, mBMT.m_File);
  // assert(NumWritten == NumToWrite);
  NumWritten = fwrite((void *)m_OutFrame, NumToWrite, 1, mBMT.m_File);
  // assert(NumWritten == NumToWrite);
  fclose(mBMT.m_File);

  FILE *fint = fopen("FrameIndex.bin", "w");
  while (!fint) {
      usleep(10000);
      fint = fopen("FrameIndex.bin", "w");
  }
  NumToWrite = sizeof(char);
  char oldframe = (char)((m_OutFrame-1+m_NFramesToBuffer)%m_NFramesToBuffer);
  if (m_FirstTime)
      oldframe = (char)-1;
  NumWritten = fwrite((void *)&oldframe, NumToWrite, 1, fint);
  while (NumWritten != NumToWrite) {
      NumWritten = fwrite((void *)&oldframe, NumToWrite, 1, fint);
  }
  // assert(NumWritten == NumToWrite);
  fclose(fint);

  // cout << "frameout " << CurrentStep << " " << m_OutFrame << " " << int(oldframe) << endl;

  m_FirstTime = 0;
  m_OutFrame = (m_OutFrame+1)%m_NFramesToBuffer;
    }
};

#endif
