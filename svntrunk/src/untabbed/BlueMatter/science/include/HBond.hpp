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
 #ifndef __HBOND_HPP__
#define __HBOND_HPP__

#include <BlueMatter/XYZ.hpp>
#include "Imaging.hpp"

template <class T>
class tDonorAcceptor
{
public:
    int m_ID;
    int m_AIndex;
    int m_BIndex[6];
    int m_NB;
    bool m_IsDonor;

    tXYZ<T> *m_pA;
    tXYZ<T> *m_pB[6];

    // Both donor and acceptor have single A and multiple B atoms

    tDonorAcceptor(int a, int *b, int n, bool donor)
    {
  m_AIndex = a;
  m_NB = n;
  for (int i=0; i<m_NB; i++)
      m_BIndex[i] = b[i];
  m_pA = NULL;
  m_pB = NULL;
  m_IsDonor = donor;
    }

    tDonorAcceptor(int *intvals, int nvals, bool isdon)
    {
  assert(nvals > 0);
  m_AIndex = intvals[0];
  for (int i=1; i<nvals; i++)
      m_BIndex[i-1] = intvals[i];
  m_IsDonor = isdon;
  m_NB = nvals-1;
  m_ID = 0;
    }

    tDonorAcceptor(tDonorAcceptor &da, int offset)
    {
  m_AIndex = da.m_AIndex+offset;
  m_NB = da.m_NB;
  m_pA = NULL;
  m_IsDonor = da.m_IsDonor;
  for (int i=0; i<m_NB; i++) {
      m_BIndex[i] = da.m_BIndex[i]+offset;
      m_pB[i] = NULL;
  }
    }


    void AssignPositions(tXYZ<T> *p)
    {
  m_pA = &p[m_AIndex];
  for (int i=0; i<m_NB; i++)
      m_pB[i] = &p[m_BIndex[i]];
    }

    T Separation(tDonorAcceptor<T> &Acc, tXYZ<T> &box, int &i, int &j, int &k)
    {
  T s;
  return s;
    }

    int NBonds(tDonorAcceptor<T> &Acc, tXYZ<T> &box, int &i, int &j, int &k)
    {
  return 0;
    }

};

typedef tDonorAcceptor<float> fDonorAcceptor;
typedef tDonorAcceptor<double> DonorAcceptor;


template <class T>
class tHBondRecord
{
public:
    tDonorAcceptor<T> *m_pDonor;
    tDonorAcceptor<T> *m_pAcceptor;
    int m_NBonds;
    int m_Image[3];
    float m_TotalEnergy;
    T m_Separation;
};

typedef tHBondRecord<float> fHBondRecord;
typedef tHBondRecord<double> HBondRecord;


template <class T>
class tDonorAcceptors
{
public:
    vector<tDonorAcceptor<T> > m_Don;
    vector<tDonorAcceptor<T> > m_Acc;
    iXYZ **m_ImageTable;

    char m_Name[512];

    // find current point in dons and accs
    // repeat each list separately
    void BuildRepeats(int dstart, int astart, int repeatcount, int length)
    {
  int j;
  int nremaining = repeatcount-1;
  int ndons = m_Don.size();
  // build don repeats with attempt to maintain order
        for (j=0; j<nremaining; j++) {
          for (int i=dstart; i<ndons; i++) {
    m_Don.push_back(tDonorAcceptor<T>(m_Don[i], length*(j+1)));
      }
  }
  int naccs = m_Acc.size();
  // build don repeats with attempt to maintain order
        for (j=0; j<nremaining; j++) {
          for (int i=astart; i<naccs; i++) {
    m_Acc.push_back(tDonorAcceptor<T>(m_Acc[i], length*(j+1)));
      }
  }
    }

    tDonorAcceptors()
    {
    }

    // given string of integers, return array of n
    // assumes strtok is primed
    int ParseIntegers(int *vals, int maxvals)
    {
  // char buff[2048];
  // strcpy(buff, str);
  // char *s = strtok(buff, " \t\n");
  char *s = strtok(NULL, " \t\n");
  int n = 0;
  while (s && strlen(s) && n < maxvals) {
      int i = atoi(s);
      // Atom ID's are 1-based!!  REMOVE OFFSET HERE ONLY
      vals[n] = i-1;
      n++;
      s = strtok(NULL, " \t\n");
  }
  return n;
    }

#if 0
DON NH 55 57 58 59
ACC PO 65 66 67 68 69
LENGTH 131
REPEAT 72
#endif

    tDonorAcceptors(char *fname)
    {
  // read file
  // build don/acc lists
  FILE *f = fopen(fname, "r");
  assert(f);
  char buff[1024];
  int index = 0;
  // char s[1024];
  int runlength = 0;
  int drepeatstart = 0;
  int arepeatstart = 0;
  int intvals[10];
  int maxvals = 10;
  // load donacc lists one by one until repeat
  // then repeat all lists since last repeat start
  // then reset repeat start to next list
  while (fgets(buff, 1024, f)) {
      char *cmd = strtok(buff, " \t\n");
      if (!strcmp(cmd, "REPEAT")) {
    // this updates the index to after the repeatrun
    int c = atoi(strtok(NULL, " \t\n"));
    BuildRepeats(drepeatstart, arepeatstart, c, runlength);
    drepeatstart = m_Don.size();
    arepeatstart = m_Acc.size();
    runlength = 0;
    continue;
      } else if (!strcmp(cmd, "LENGTH")) {
    runlength = atoi(strtok(NULL, " \t\n"));
      } else if (!strcmp(cmd, "DON")) {
    strtok(NULL, " \t\n");  // skip name
    int n=ParseIntegers(intvals, maxvals);
    m_Don.push_back(tDonorAcceptor<T>(intvals, n, true));
      } else if (!strcmp(cmd, "ACC")) {
    strtok(NULL, " \t\n");  // skip name
    int n=ParseIntegers(intvals, maxvals);
    m_Acc.push_back(tDonorAcceptor<T>(intvals, n, false));
      }
  }
    }

    void AssignPositions(tXYZ<T> *p)
    {
  for (vector<tDonorAcceptor<T> >::iterator da=m_Don.begin(); da != m_Don.end(); da++)
      da->AssignPositions(p);
  for (vector<tDonorAcceptor<T> >::iterator da=m_Acc.begin(); da != m_Acc.end(); da++)
      da->AssignPositions(p);
    }

    void SetIDs()
    {
  int id=0;
  for (vector<tDonorAcceptor<T> >::iterator da=m_Don.begin(); da != m_Don.end(); da++)
      da->m_ID = id++;
  id = 0;
  for (vector<tDonorAcceptor<T> >::iterator da=m_Acc.begin(); da != m_Acc.end(); da++)
      da->m_ID = id++;
    }

    void InitImages(tXYZ<T> box)
    {
        // iXYZ **m_ImageTable;
  // build initial nsq table and give initial image vects.
  // also assign id's in a way consistent with each table

  int ndons = m_Don.size();
  int nacc = m_Acc.size();

  m_ImageTable = new iXYZ*[ndons];
  for (int i=0; i<ndons; i++)
      m_ImageTable[i] = new iXYZ[nacc];
  for (int i=0; i<ndons; i++) {
      m_Don[i].m_ID = i;
      for (int j=0; j<nacc; j++) {
    m_Acc[j].m_ID = j;
    m_ImageTable[i][j] = tImaging<T>::NearestLattice(m_Don[i].m_pA, m_Acc[j].m_pA, box);
      }
  }
    }

    void SplitUpperLower(tDonorAcceptors<T> &lower, tDonorAcceptors<T> &upper)
    {
  float zsum=0;
  for (int i=0; i<m_Don.size(); i++)
      zsum += m_Don[i].m_pA->mZ;
  for (int i=0; i<m_Acc.size(); i++)
      zsum += m_Acc[i].m_pA->mZ;
  zsum /= m_Don.size() + m_Acc.size();
  for (int i=0; i<m_Don.size(); i++) {
      if (m_Don[i].m_pA->mZ < zsum)
    lower.m_Don.push_back(m_Don[i]);
      else
    upper.m_Don.push_back(m_Don[i]);
  }
  for (int i=0; i<m_Acc.size(); i++) {
      if (m_Acc[i].m_pA->mZ < zsum)
    lower.m_Acc.push_back(m_Acc[i]);
      else
    upper.m_Acc.push_back(m_Acc[i]);
  }
    }

    void FindHBonds(vector<tHBondRecord<T> > &hbr, tXYZ<T> &box)
    {
    }

    float HBEnergy(tXYZ<T> *pn, tXYZ<T> *ph, tXYZ<T> *pc, tXYZ<T> *po)
    {
#define DSSP_FACTOR (332.0 * 0.42 * 0.2)
        tXYZ<T> dv = *po-(*ph);
        T doh = dv.Length();
  const float maxbonddistance = 5.0;
        if (doh > maxbonddistance)
            return 0;

        T don = po->Distance(*pn);
        T dch = pc->Distance(*ph);
        T dcn = pc->Distance(*pn);

        float e = DSSP_FACTOR * (1/don + 1/dch - 1/doh - 1/dcn);
  return e;
    }

    void DumpHBond(tDonorAcceptor<T> &d, tDonorAcceptor<T> &a, tXYZ<T> &box, float t, FILE *f, int upper)
    {
  iXYZ lat;
  tXYZ<T> xlat;
  tXYZ<T> dvabs = *a.m_pA - *d.m_pA;
  tImaging<T>::TranslationVectorAndLattice(dvabs, box, xlat, lat);
  tXYZ<T> dv = dvabs-xlat;
  const float maxsep = 8.0;
  const float bondenergy = -1.0;
  if (dv.LengthSquared() > maxsep*maxsep)
      return;
  float esum = 0;
  int nbonds = 0;
  tXYZ<T> acc = *a.m_pA - xlat;
  for (int i=0; i<d.m_NB; i++) {
      for (int j=0; j<a.m_NB; j++) {
    tXYZ<T> acc2 = *a.m_pB[j]-xlat;
    float e = HBEnergy(d.m_pA, d.m_pB[i], &acc, &acc2);
    if (e < bondenergy) {
        nbonds++;
        esum += e;
    }
      }
  }
  if (!nbonds)
      return;
  // This assumes id's are bound to table index!!  (by construction in InitImages).
  iXYZ dlat = lat-m_ImageTable[d.m_ID][a.m_ID];

  fprintf(f, "%8.3f %d %d %d %d %f %d %d %d\n", t, upper, d.m_ID, a.m_ID, nbonds, esum, lat.mX, lat.mY, lat.mZ);

#if 0
  if (nbonds > 7) {
      cout << *d.m_pA << " " << d.m_pB[0] << " " << d.m_pB[1] << " " << d.m_pB[2] << endl;
      cout << d.m_AIndex << " " << d.m_BIndex[0] << " " << d.m_BIndex[1] << " " << d.m_BIndex[2] << " " << d.m_BIndex[3] << endl;
      cout << *a.m_pA << " " << a.m_pB[0] << " " << a.m_pB[1] << " " << a.m_pB[2] << " " << a.m_pB[3] << endl;
      cout << a.m_AIndex << " " << a.m_BIndex[0] << " " << a.m_BIndex[1] << " " << a.m_BIndex[2] << " " << a.m_BIndex[3] << endl;
  }
#endif
#if 0
  cout << step*0.005 << " " << uplow << " " << d.m_ID << " " << a.m_ID << " " <<
    nbonds << " " << esum << " " <<
    lat.mX << " " << lat.mY << " " << lat.mZ << " " <<
    // dlat.mX << " " << dlat.mY << " " << dlat.mZ << " " << nbonds << " " << esum << " " <<
    // d.m_pA->mX << " " << d.m_pA->mY << " " << d.m_pA->mZ << " " <<
    // a.m_pA->mX << " " << a.m_pA->mY << " " << a.m_pA->mZ << " " <<
    endl;
#endif

    }

    void DumpHBonds(float t, tXYZ<T> &box, FILE *f, int upper)
    {
  for (vector<tDonorAcceptor<T> >::iterator d=m_Don.begin(); d != m_Don.end(); d++) {
      for (vector<tDonorAcceptor<T> >::iterator a=m_Acc.begin(); a != m_Acc.end(); a++) {
    DumpHBond(*d, *a, box, t, f, upper);
      }
  }
    }

    void DumpA(tXYZ<T> &box, FILE *f)
    {
  for (vector<tDonorAcceptor<T> >::iterator d=m_Acc.begin(); d != m_Acc.end(); d++) {
      fXYZ v;
      tImaging<T>::TranslateIntoBox(*d->m_pA, box, v);
      fprintf(f, "%f %f %f\n", v.mX, v.mY, v.mZ);
  }
    }
};

typedef tDonorAcceptors<float> fDonorAcceptors;
typedef tDonorAcceptors<double> DonorAcceptors;

#endif
