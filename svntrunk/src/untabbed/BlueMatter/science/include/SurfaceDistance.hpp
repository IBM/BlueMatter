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
 #ifndef SURFACEDISTANCE_HPP
#define SURFACEDISTANCE_HPP

#include <vector>
#include <algorithm>
#include <BlueMatter/XYZ.hpp>

template <class type>
class tPolar
{
public:
    type m_R;
    type m_Theta;
    type m_X;
    type m_Y;

    tPolar()
    {
  m_R = 0.0001;
  m_Theta = 0;
  m_X = m_R;
  m_Y = 0.0;
    }

    tPolar(type r, type theta)
    {
  m_R = r;
  m_Theta = theta;
  m_X = r*cos(m_Theta);
  m_Y = r*sin(m_Theta);
    }

    tPolar(const tPolar &f)
    {
  m_R = f.m_R;
  m_Theta = f.m_Theta;
  m_X = f.m_X;
  m_Y = f.m_Y;
    }

    tPolar &operator=(const tPolar &f)
    {
  m_R = f.m_R;
  m_Theta = f.m_Theta;
  m_X = f.m_X;
  m_Y = f.m_Y;
  return *this;
    }

    bool operator==(const tPolar &f) const
    {
  return m_R == f.m_R && m_Theta == f.m_Theta;
    }

    bool operator<(const tPolar &f) const
    {
  return m_Theta < f.m_Theta;
    }

    static double PI() {
  return 3.141592653589793238462;
    }

    tPolar(const tXYZ<type> &p)
    {
  m_R = sqrt(p.mX*p.mX + p.mY*p.mY);
  m_Theta = atan2(p.mY, p.mX);
  if (m_Theta < 0)
      m_Theta += 2*PI();
  m_X = p.mX;
  m_Y = p.mY;
    }
};

typedef tPolar<double> Polar;
typedef tPolar<float> fPolar;

template <class type>
class PolarComp
{
public:
    bool operator()(const tPolar<type> &a, const tPolar<type> &b) const
    {
  return a.m_R < b.m_R;
    }
};

template <class type>
class tSurfaceDistance
{
public:
    int m_NPerimPoints;
    tPolar<type> **m_PerimPoints;
    tXYZ<type> m_Center;
    type m_ZMin;
    type m_ZMax;
    type m_ThetaStep;
    int m_NSlabs;
    type m_SlabStep;
    type *m_Area;
    type m_RMin;

    void Clear()
    {
  if (m_Area)
      delete [] m_Area;
  m_Area = new type[m_NSlabs];
  if (m_PerimPoints)
      delete [] m_PerimPoints;
  m_PerimPoints = new tPolar<type>*[m_NSlabs];
  for (int i=0; i<m_NSlabs; i++) {
      m_PerimPoints[i] = new tPolar<type>[m_NPerimPoints];
      for (int j=0; j<m_NPerimPoints; j++)
    m_PerimPoints[i][j] = tPolar<type>(m_RMin, j*m_ThetaStep);  // initialize perimeter to nice circle to avoid degen
  }
    }

    tSurfaceDistance(int NPerim, type ZMin, type ZMax, int NSlabs=1, type rmin=1.0)
    {
  m_NPerimPoints = NPerim;
  m_NSlabs = NSlabs;
  m_ThetaStep = 2*Polar::PI()/m_NPerimPoints;
  m_ZMin = ZMin;
  m_ZMax = ZMax;
  m_SlabStep = (ZMax-ZMin)/NSlabs;
  m_Area = NULL;
  m_PerimPoints = NULL;
  m_RMin = rmin;

  Clear();
    }

    int SlabNum(const type z) const
    {
  int n = (z-m_ZMin)/m_SlabStep;
  if (n<0 || n>=m_NSlabs)
      n = -1;
  return n;
    }

    type SlabZ(const int i) const
    {
  return m_ZMin + (i+0.5)*m_SlabStep;
    }

    // theta assumed to be 0->2pi
    int WedgeNum(const type t) const
    {
  int sn = t/m_ThetaStep;
  if (sn >= m_NPerimPoints)
      sn = 0;
  return sn;
    }

    void AddPoint(const tXYZ<type> &p)
    {
  int sn = SlabNum(p.mZ);
  if (sn < 0)
      return;
  tPolar<type> pol(p);
  int wn = WedgeNum(pol.m_Theta);
  tPolar<type> &pmax = m_PerimPoints[sn][wn];
  if (pol.m_R > pmax.m_R)
      pmax = pol;
    }

    void AddPoints(vector<tXYZ<type> > &vp)
    {
  for (vector<tXYZ<type> >::iterator i = vp.begin(); i != vp.end(); i++)
      AddPoint(*i);
    }

#if 0
    // go through all points
    // for any point in range:
    //   find 2d angle and dist from center
    // sort by angle
    // build polygon
    void Slab(vector<tXYZ<type> > &points, const tXYZ<type> &center, const type zmin, const type zmax, int nintervals=1)
    {
  m_ZMin = zmin;
  m_ZMax = zmax;
  m_Center = center;
  double pi = atan(1.0)*4;

  vector<tPolar<type> > blob;
  for (vector<tXYZ<type> >::iterator p = points.begin(); p != points.end(); p++)
  {
      if (p->mZ >= zmin && p->mZ < zmax) {
    tPolar<type> *pol = new tPolar<type>(*p - m_Center);
    blob.push_back(*pol);
      }
  }

  sort(blob.begin(), blob.end()); // theta sort

  vector<tPolar<type> >::iterator first;
  vector<tPolar<type> >::iterator last = blob.begin();
  tPolar<type> ref;
  for (int i=0; i<m_NPerimPoints; i++) {
      first = last;
      ref.m_Theta = (i+1)*m_ThetaStep;  
      last = upper_bound(first, blob.end(), ref); // theta sort
      // cerr << "upper bound returns theta " << last->m_Theta << " " << ref.m_Theta << endl;
      if (i == m_NPerimPoints-1)
    last = blob.end();
      // cerr << i << " reftheta, first, last theta " << ref.m_Theta << " " << first->m_Theta << " " << last->m_Theta << endl;
      vector<tPolar<type> >::iterator mx = max_element(first, last, PolarComp<type>());
      // cerr << "max element " << mx->m_Theta << " " << mx->m_R << endl;
      m_PerimPoints[0][i] = *mx;
  }
    }
#endif

    void FindArea()
    {
  for (int j=0; j<m_NSlabs; j++) {
      type z = SlabZ(j);
      type a = 0;
      for (int i=0; i<m_NPerimPoints; i++) {
    int h = (i-1)%m_NPerimPoints;
    int k = (i+1)%m_NPerimPoints;
    a += m_PerimPoints[j][i].m_X*(m_PerimPoints[j][h].m_Y - m_PerimPoints[j][k].m_Y);
      }
      m_Area[j] = fabs(a);
  }
    }

    void Print()
    {
  FindArea();
  cerr << endl;
  cerr << "SurfacePolygon with " << m_NPerimPoints << " points (and chords)" << endl;
  cerr << "Center " << m_Center << " z = " << m_ZMin << " -> " << m_ZMax << endl;
  cerr << "Areas:" << endl;
  for (int j=0; j<m_NSlabs; j++) {
      type z = SlabZ(j);
      cerr << z << " " << m_Area[j] << endl;
  }
  cerr << "Perimiter:" << endl;
  for (int j=0; j<m_NSlabs; j++) {
      type z = SlabZ(j);
      for (int i=0; i<m_NPerimPoints; i++) {
    cerr << m_PerimPoints[j][i].m_X << " " << m_PerimPoints[j][i].m_Y << " " << z << " " <<
        m_PerimPoints[j][i].m_R << " " << m_PerimPoints[j][i].m_Theta << " " << m_Area[j] << endl;
      }
  }
  cerr << endl;
    }

    void DXDump(char *fname)
    {
  FILE *f = fopen(fname, "a");
  if (!f) {
      cerr << "Error opening dx surface file " << fname << endl;
      exit(-1);
  }
  FindArea();
  for (int j=0; j<m_NSlabs; j++) {
      type z = SlabZ(j);
      for (int i=0; i<m_NPerimPoints; i++) {
    fprintf(f, "%f %f %f %f\n", m_PerimPoints[j][i].m_X, m_PerimPoints[j][i].m_Y, z, m_Area[j]);
      }
  }
  fclose(f);
    }

    // given arbitrary polar point, find signed distance from surface
    //   first find which sector
    //   then find which pair
    //   then location of intersection
    type Distance(tPolar<type> &p, int slabnum=0)
    {
  if (slabnum < 0 || slabnum >= m_NSlabs)
      return p.m_R;
  int sector = p.m_Theta/m_ThetaStep;
  int ccw = (sector+1)%m_NPerimPoints;
  int cw = (sector-1)%m_NPerimPoints;
  tPolar<type> &a = m_PerimPoints[slabnum][sector];
  int other = ccw;

  // assume point is between sector pt and next one ccw
  if (a.m_Theta > p.m_Theta)
      other = cw;
  tPolar<type> &b = m_PerimPoints[slabnum][other];

  double invdenom = 1.0/((b.m_Y-a.m_Y)*(p.m_X-0) - (b.m_X-a.m_X)*(p.m_Y-0));
  double ua = ((b.m_X-a.m_X)*(0-a.m_Y) - (b.m_Y-a.m_Y)*(0-a.m_X))*invdenom;

  double xnew = 0 + ua*(p.m_X-0);
  double ynew = 0 + ua*(p.m_Y-0);
  double rnew = sqrt(xnew*xnew + ynew*ynew);

  return p.m_R-rnew;
    }

    type Distance(tXYZ<type> &p)
    {
  tPolar<type> point(p);
  return Distance(point, SlabNum(p.mZ));
    }

#if 0
    // go through all points
    // for any point in z-range:
    //   find 2d angle and dist from center
    //   find distance from intersected chord
    void Measure(vector<tXYZ<type> > &points, vector<type> &d)
    {
  for (vector<tXYZ<type> >::iterator p = points.begin(); p != points.end(); p++)
  {
      if (p->mZ >= m_ZMin && p->mZ < m_ZMax) {
    tPolar<type> point(*p - m_Center);
    d.push_back(Distance(point));
      }
  }
    }
#endif
};

typedef tSurfaceDistance<double> SurfaceDistance;
typedef tSurfaceDistance<float> fSurfaceDistance;

#endif
