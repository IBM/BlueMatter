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
 #ifndef ALIGN_HPP
#define ALIGN_HPP

#include <vector>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/LSQ.hpp>
#include <BlueMatter/Topology.hpp>

using namespace std ;

template <class T>
class tAlign
{
public:
    Matrix m_Rotation;
    Matrix m_RInv;
    Vector m_Displacement;
    int m_NAtoms;
    int m_NHeavyAtoms;
    int m_NSystemAtoms;
    tXYZ<T> *m_LocalPos;
    tXYZ<T> m_CurrentCOMInBox;
    tXYZ<T> m_OrigCOM;
    tXYZ<T> m_Box;

    Vector *m_RefPositions;
    Vector *m_Buffer;
    T *m_Masses;

    tAlign(tXYZ<T> *p, int count, int systemcount, tXYZ<T> &box)
    {
  m_NAtoms = count;
  m_RefPositions = new Vector[m_NAtoms];
  m_Buffer = new Vector[m_NAtoms];
  m_NSystemAtoms = systemcount;
  m_LocalPos = new tXYZ<T>[m_NSystemAtoms];
  m_Masses = NULL;
  m_NHeavyAtoms = 0;
  m_Box = box;

  m_OrigCOM = tXYZ<T>::ZERO_VALUE();
  for (int i=0; i<m_NAtoms; i++) {
      m_OrigCOM += p[i];
      for (int j=0; j<3; j++)
    m_RefPositions[i][j] = p[i][j];
  }
  m_OrigCOM /= m_NAtoms;
    }

    tAlign(tXYZ<T> *p, T *m, int count, int systemcount, tXYZ<T> &box)
    {
  m_NAtoms = count;
  m_RefPositions = new Vector[m_NAtoms];
  m_Buffer = new Vector[m_NAtoms];
  m_NSystemAtoms = systemcount;
  m_LocalPos = new tXYZ<T>[m_NSystemAtoms];
  m_Masses = m;
  m_Box = box;

  m_NHeavyAtoms = 0;
  m_OrigCOM = tXYZ<T>::ZERO_VALUE();
  double HeavyMass = 0;
  for (int i=0; i<m_NAtoms; i++) {
      if (m_Masses[i] < 2)
    continue;
      m_OrigCOM += m_Masses[i]*p[i];
      HeavyMass += m_Masses[i];
      for (int j=0; j<3; j++)
    m_RefPositions[m_NHeavyAtoms][j] = p[i][j];
      m_NHeavyAtoms++;
  }
  m_OrigCOM /= HeavyMass;
    }

    // Initialize the class using a reference fragment
    // Also, allocate space for coordinate buffers used later to align arbitrary fragments
    // This initialization finds the original com of the ref fragment
    //   and stores its initial positions, which remain untouched
    tAlign(tFragment<T> &f, int systemcount, tXYZ<T> &box, bool heavy = true)
    {
  // cerr << "align " << f.m_Name << " " << f.m_Start << " " << f.m_Count << " " << (void *)f.m_Positions << endl;
  XYZ *p = &f.m_Positions[f.m_Start];
  m_NAtoms = f.m_Count;
  m_RefPositions = new Vector[m_NAtoms];
  m_Buffer = new Vector[m_NAtoms];
  m_NSystemAtoms = systemcount;
  m_LocalPos = new tXYZ<T>[m_NSystemAtoms];
  m_Box = box;
  m_OrigCOM = tXYZ<T>::ZERO_VALUE();
  double HeavyMass = 0;
  if (heavy) {
      m_Masses = &f.m_Masses[f.m_Start];

      m_NHeavyAtoms = 0;
      for (int i=0; i<m_NAtoms; i++) {
    if (m_Masses[i] < 2)
        continue;
    m_OrigCOM += m_Masses[i]*p[i];
    HeavyMass += m_Masses[i];
    for (int j=0; j<3; j++)
        m_RefPositions[m_NHeavyAtoms][j] = p[i][j];
    m_NHeavyAtoms++;
      }
  } else {
      m_Masses = NULL;
      for (int i=0; i<m_NAtoms; i++) {
    HeavyMass += m_Masses[i];
    m_OrigCOM += m_Masses[i]*p[i];
    for (int j=0; j<3; j++)
        m_RefPositions[i][j] = p[i][j];
      }
  }
  m_OrigCOM = m_OrigCOM/HeavyMass;
    }

    void UpdateRefPositions(tXYZ<T> *p, bool heavy= true)
    {
    if (heavy) {
        int nheavy = 0;
        for (int i=0; i<m_NAtoms; i++) {
            if (m_Masses[i] < 2)
                continue;
            for (int j=0; j<3; j++)
                m_RefPositions[nheavy][j] = p[i][j];
            nheavy++;
            }
        }
    else {
        for (int i=0; i<m_NAtoms; i++) {
            for (int j=0; j<3; j++)
                m_RefPositions[i][j] = p[i][j];
            }
        }
    }

    // This defines the current transform, using the positions of the current reference fragment
    // First the com is found for the ref fragment and it is translated into the box with
    // the orig com.  Thus the bulk of the diffusion is removed via imaging rather than minimization.
    // Once it is in the box near the orig. configuration, the transformation is calculated using lsq.
    void DefineTransform(tXYZ<T> *p)
    {
  int N;
  double HeavyMass = 0;
  tXYZ<T> com = tXYZ<T>::ZERO_VALUE();
  if (m_Masses) {
      int h = 0;
      for (int i=0; i<m_NAtoms; i++) {
    if (m_Masses[i] < 2)
        continue;
    HeavyMass += m_Masses[i];
    com += m_Masses[i]*p[i];
    for (int j=0; j<3; j++)
        m_Buffer[h][j] = p[i][j];
    h++;
      }
      N = m_NHeavyAtoms;
  } else {
      for (int i=0; i<m_NAtoms; i++) {
    com += m_Masses[i]*p[i];
    HeavyMass += m_Masses[i];
    for (int j=0; j<3; j++)
        m_Buffer[i][j] = p[i][j];
      }
      N = m_NAtoms;
  }

  com = com/HeavyMass;
  tXYZ<T> dv = com-m_OrigCOM;
  tXYZ<T> trans;
  tImaging<T>::TranslationVector(dv, m_Box, trans);
  m_CurrentCOMInBox = com-trans;
  // cerr << "currentcom and trans " << m_CurrentCOMInBox << " " << trans << " " << com << " " << m_OrigCOM << endl;
  for (int i=0; i<N; i++)
      for (int j=0; j<3; j++)
    m_Buffer[i][j] -= trans[j];
      
        lstSqAlign(m_RefPositions, m_Buffer, N, m_Rotation, m_Displacement);
  for (int i=0; i<3; i++)
      for (int j=0; j<3; j++)
    m_RInv[i][j] = m_Rotation[j][i];
  // cerr << "definetrans " << (void *)p <<  " " << N << " " << m_Displacement << endl;
    }

    // This transforms an arbitrary fragment
    // First it maps it into the box near the currentcom of the ref frag (in the box)
    // then it applies the lsq transform
    void ApplyTransform(tFragment<T> &f)
    {
  // cerr << "applyxform to " << f.m_Name << " " << f.m_Start << " " << f.m_Count << " " << (void *)f.m_Positions << " " << m_Displacement << endl;
  tXYZ<T> trans;
  tXYZ<T> dv = f.m_CenterOfMass-m_CurrentCOMInBox;
  tImaging<T>::TranslationVector(dv, m_Box, trans);
  for (int i=0; i<f.m_Count; i++) {
      Vector v;
      tXYZ<T> w = f.m_Positions[f.m_Start+i] - trans;
      v[0] = w[0];
      v[1] = w[1];
      v[2] = w[2];
      v = v - m_Displacement;
      v = m_RInv / v;
      m_LocalPos[i][0] = v[0];
      m_LocalPos[i][1] = v[1];
      m_LocalPos[i][2] = v[2];
  }
    }

#if 0
    void ApplyTransform(fFragmentList &fl)
    {
  for (vector<fFragment>::iterator i = fl.m_Fragments.begin(); i != fl.m_Fragments.end(); i++) {
      ApplyTransform(*i);
  }
    }
#endif

    // this finds rmsd with respect to ref points, with or without mass.
    // NOTE - this should also do heavy lifting with imaging
    double FindRMSD(tXYZ<T> *p)
    {
  DefineTransform(p);
  int N;
  Vector v;
  double rmsd = 0;
  if (m_Masses) {
      int h = 0;
      for (int i=0; i<m_NAtoms; i++) {
    if (m_Masses[i] < 2)
        continue;
    for (int j=0; j<3; j++)
        v[j] = p[i][j];
    v = v - m_Displacement;
    v = m_RInv / v;
    v = v - m_RefPositions[h];
    rmsd += v/v;
    h++;
      }
      N = m_NHeavyAtoms;
  } else {
      for (int i=0; i<m_NAtoms; i++) {
    for (int j=0; j<3; j++)
        v[j] = p[i][j];
    v = v - m_Displacement;
    v = m_RInv / v;
    v = v - m_RefPositions[i];
    rmsd += v/v;
      }
      N = m_NAtoms;
  }
  return sqrt(rmsd/N);
    }

    double FindRMSD(tFragment<T> &f)
    {
  return FindRMSD(&f.m_Positions[f.m_Start]);
    }

};

#endif
