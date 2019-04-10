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
 #ifndef IMAGING_HPP
#define IMAGING_HPP

#include <vector>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/Topology.hpp> // for MoleculeRun
#include <BlueMatter/ExternalDatagram.hpp> // for tSiteData

using namespace std;

template <class type>
class tImaging
{
public:

    // Hausdorff distance between two sets of points
    // should be static
    // no imaging
    static type HausdorffDistance(const tXYZ<type> *p, const int np, const tXYZ<type> *q, const int nq) 
    {
  type max = -1.0E+20;

  for (int i=0; i<nq; i++) {
      type min = 1.0E+20;
      for (int j=0; j<np; j++) {
    d = p[j].DistanceSquared(q[i]);
    if (d < min)
        min = d;
      }
      if (max < min)
    max = min;
  }
  return max;
    }

    // Linear translation vector of 1D point
    static type TranslationDistance(const type L, const type x)
    {
  type a = fabs(x);
  if (a < L/2) return 0;
  int n = (a-L/2)/L + 1;
  return (x > 0) ? n*L : -n*L;
    }

    // Linear translation vector of 1D point with respect to the origin
    static type TranslationDistanceAK(const type L, const type x)
    {
  type a = fabs(x);
  if (a < L/2) return 0;

  if(a<L && x>0) return L;	// this will move x-L
  if(a<L && x<=0) return (-L); // this will move x+L


  type ret = 0;
        double rem = fmod(a , L);
        if(x > 0 ) {
    ret = rem;
                if(rem > L/2)  ret -= L;
    return (x - ret);
        }
  // ie., x <= 0
  ret = -rem;
        if(rem > L/2)  ret += L;
  return (x - ret); // this is the bug fix : (x+ret) was here earlier
    }

    // integer number of lattice steps away for 1D point
    static int LatticeDistance(const type L, const type x)
    {
  type a = fabs(x);
  if (a < L/2)
      return 0;
  int n = (a-L/2)/L + 1;
  return (x > 0) ? n : -n;
    }


    // 3D translation vector of point in cubic box centered at 0
    // returns nonzero if no translation
    static int TranslationVector(const type L, const tXYZ<type> &p, const tXYZ<type> &v)
    {
  v.mX = TranslationDistance(L, p.mX);
  v.mY = TranslationDistance(L, p.mY);
  v.mZ = TranslationDistance(L, p.mZ);
  return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
    }


    // 3D translation vector of point in arbitrary box centered at origin
    // returns nonzero if no translation
    static int TranslationVector(const tXYZ<type> &r, const tXYZ<type> &box, tXYZ<type> &v)
    {
  v.mX = TranslationDistance(box.mX, r.mX);
  v.mY = TranslationDistance(box.mY, r.mY);
  v.mZ = TranslationDistance(box.mZ, r.mZ);
  return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
    }

    // 3D translation vector of point in arbitrary box centered at origin
    // returns nonzero if no translation
    // also returns integer translation vector
    static int TranslationVectorAndLattice(const tXYZ<type> &r, const tXYZ<type> &box, tXYZ<type> &v, iXYZ &lat)
    {
  v.mX = TranslationDistance(box.mX, r.mX);
  v.mY = TranslationDistance(box.mY, r.mY);
  v.mZ = TranslationDistance(box.mZ, r.mZ);
  lat.mX = LatticeDistance(box.mX, r.mX);
  lat.mY = LatticeDistance(box.mY, r.mY);
  lat.mZ = LatticeDistance(box.mZ, r.mZ);
  return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
    }


    // Translate run of points into cubic box centered at origin using tag atom
    static void TranslateIntoBox(const type L, tXYZ<type> *p, const int k, const int count, const int tag)
    {
  tXYZ<type> v;
  if (TranslationVector(L, p[k+tag], v))
      return;
  for (int i=k; i<k+count; i++)
      p[i] -= v;
    }

    // Translate run of site data positions into box centered at origin, using middle of run as tag
    static void TranslateIntoBox(tSiteData *s, const tXYZ<type> &box, const int k, const int count)
    {
  // int tag = k + count/2;
  int tag = k;
  tXYZ<type> v;
  if (TranslationVector(s[tag].mPosition, box, v))
      return;
  for (int i=k; i<k+count; i++)
      s[i].mPosition -= v;
    }

    // Translate run of site data positions into box centered at origin, using the first atom as tag
    static void TranslateIntoBox(tXYZ<type> *s, const tXYZ<type> &boxw, const tXYZ<type> &origin, const int k, const int count)
    {
  // int tag = k + count/2;
  int tag = k;
  tXYZ<type> v;
  if (TranslationVector(s[tag], boxw, v))
      return;
  for (int i=k; i<k+count; i++)
      s[i] -= v - origin;
    }

    // Translate single vector into box
    static void TranslateIntoBox(const tXYZ<type> &v, const tXYZ<type> &box, tXYZ<type> &inbox)
    {
  tXYZ<type> tv;
  inbox = v;
  if (TranslationVector(v, box, tv))
      return;
  inbox -= tv;
    }


    // Translate fragments into box using site list and fragment list, based on middle atom of fragment as tag
    static void MapIntoBox(tSiteData *ps, const tXYZ<type> &box, const vector<int> &fragmentlist)
    {
  int count = fragmentlist.size();
  int index = 0;
  for (int i=0; i<count; i++) {
      TranslateIntoBox(ps, box, index, fragmentlist[i]);
      index += fragmentlist[i];
  }
    }

    // Translate fragments into box using site list and fragment list, based on the first atom of fragment as tag
    static void MapIntoBox(tXYZ<type> *ps, const tXYZ<type> &boxw, const tXYZ<type> origin, vector<MoleculeRun> &molrun)
    {
  int index = 0;
  vector<MoleculeRun>::iterator it=molrun.begin();
  for(;it!=molrun.end();it++) {
    for(int i=0;i<it->m_Count;i++ ) { // for each molecule
          TranslateIntoBox(ps, boxw, origin, index, it->m_Size);
          index += it->m_Size;
    }
    
  }
    }

    // This maps a point into a box centered at origin, then lifts it up for orb use
    static tXYZ<type> MapIntoBlueMatterBox(const tXYZ<type> p, const tXYZ<type> &box)
    {
        tXYZ<type> v;
        tXYZ<type> rv = p;
        if (!TranslationVector(rv, box, v))
        {
            rv -= v;
        }
        if (rv.mX < 0)
            rv.mX += box.mX;
        if (rv.mY < 0)
            rv.mY += box.mY;
        if (rv.mZ < 0)
            rv.mZ += box.mZ;
        return rv;
    }


    // this gives the displacement from a to the nearest image of b
    static tXYZ<type> NearestImageDisplacement(const tXYZ<type> &a, const tXYZ<type> &b, const tXYZ<type> &box)
    {
  tXYZ<type> rel = b - a;
  tXYZ<type> xlat;
  if (TranslationVector(rel, box, xlat))
      return rel;
  else
      return rel - xlat;
    }

    static type NearestImageDistance(const tXYZ<type> &a, const tXYZ<type> &b, const tXYZ<type> &box)
        {
    tXYZ<type> rel = b - a;
    tXYZ<type> xlat;
    if (!TranslationVector(rel, box, xlat))
        rel =  rel - xlat;
    return rel.Length();
    }

    // this gives the nearest image of b to a
    static tXYZ<type> NearestImagePosition(const tXYZ<type> &a, const tXYZ<type> &b, const tXYZ<type> &box)
    {
  tXYZ<type> rel = b - a;
  tXYZ<type> xlat;
  if (TranslationVector(rel, box, xlat))
      return b;
  else
      return b - xlat;
    }


    static iXYZ NearestLattice(const tXYZ<type> *a, const tXYZ<type> *b, const tXYZ<type> &box)
    {
  iXYZ delta;

  tXYZ<type> v = *b-(*a);
  delta.mX = LatticeDistance(box.mX, v.mX);
  delta.mY = LatticeDistance(box.mY, v.mY);
  delta.mZ = LatticeDistance(box.mZ, v.mZ);

  return delta;
    }
};

typedef tImaging<double> Imaging;
typedef tImaging<float> fImaging;


#endif

