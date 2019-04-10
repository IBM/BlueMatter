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
 // SimpleTrajectory.h: interface for the SimpleTrajectory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLETRAJECTORY_H__4B156D3E_174D_42E2_9658_D4F1256611FB__INCLUDED_)
#define AFX_SIMPLETRAJECTORY_H__4B156D3E_174D_42E2_9658_D4F1256611FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include <cmath>
#include <fcntl.h>
#include <cstdio>
#ifdef WIN32
#include <io.h>
#define SWAP_IN
#define SWAP_OUT
#endif
#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>

#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/Swap.hpp>
#include <BlueMatter/Topology.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/ExternalDatagram.hpp> // for tSiteData
#include <BlueMatter/XYZColumn.hpp>
#include <BlueMatter/NoesyList.hpp>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/JRand.hpp>

using namespace std ;

// list of trajectories from same run
// stores a vector of xyzcolumns, so
// stores a single fixed step bmt of many lipids
// "items" refers to lipids
class SimpleTrajectory  
{
public:
    // start can be greater than end if timestep < 0
    float m_StartTime;
    float m_EndTime;

    // but min is always less than max
    float m_MinTime;
    float m_MaxTime;

    float m_TimeStep;
    int   m_NSteps;
    bool m_Reverse;

    vector<XYZColumn> m_Pos;
    int getNItems() {
  return m_Pos.size();
    }

    // call after loading the columns so it knows npts
    void init(float start, float step) {
  m_StartTime = start;
  m_TimeStep = step;
  m_NSteps = m_Pos[0].getNPts();
  m_EndTime = m_StartTime + (m_NSteps-1)*m_TimeStep;
  m_Reverse = (m_TimeStep < 0);
  if (!m_Reverse) {
      m_MinTime = m_StartTime;
      m_MaxTime = m_EndTime;
  } else {
      m_MinTime = m_EndTime;
      m_MaxTime = m_StartTime;
  }
    }

    bool hasTime(float t) {
  if (!m_Reverse)
      return (t >= m_StartTime && t <= m_EndTime);
        return (t >= m_EndTime && t <= m_StartTime);
    }

    bool getPosition(int nitem, float t, XYZ &p, float maxerr = 1.0) {
  int n = (t-m_StartTime)/m_TimeStep+0.5;
  if (n < 0)
      n=0;
  if (n>=m_NSteps)
      n=m_NSteps-1;
  p = m_Pos[nitem].m_Data[n];
  float actualtime = m_StartTime + n*m_TimeStep;
  return ((fabs(actualtime-t)<fabs(maxerr*m_TimeStep)));
    }

    bool getIndex(float t, int &index, float maxerr = 1.0) {
  index = (t-m_StartTime)/m_TimeStep+0.5;
  if (index < 0)
      index=0;
  if (index>=m_NSteps)
      index=m_NSteps-1;
  float actualtime = m_StartTime + index*m_TimeStep;
  return ((fabs(actualtime-t)<fabs(maxerr*m_TimeStep)));
    }


    SimpleTrajectory()
    {
    }

    virtual ~SimpleTrajectory()
    {

    }
};


// want to load individual SimpTrajLip, and accum them into lipidset

// list of different trajectories
class TrajectoryLipidSet
{
public:
    vector<SimpleTrajectory *> m_Trajectory;

    float m_StartTime;
    float m_EndTime;
    int m_NTraj;
    int m_NSteps;
    int m_NItems;
    int m_NLeaflets;
    int m_NLower;
    int m_NUpper;

    TrajectoryLipidSet() {
  m_NTraj = 0;
  m_StartTime = 1.0E+20f;
  m_EndTime = -1.0E+20f;
  m_NSteps = 0;
  m_NItems = 0;
  m_NLeaflets = 0;
  m_NLower = 0;
  m_NUpper = 0;
    }

    int getNTrajectories() {
  return m_Trajectory.size();
    }

    // assumes t has been init
    void addTrajectory(SimpleTrajectory &t) {
  m_Trajectory.push_back(&t);
  m_NTraj++;
  if (m_StartTime > t.m_MinTime)
      m_StartTime = t.m_MinTime;
  if (m_EndTime < t.m_MaxTime)
      m_EndTime = t.m_MaxTime;
  m_NSteps += t.m_NSteps;
    }

    // noesy list has groups containing ids
    // fragment list has the fragments pointed to
    // this loads all in group order, so Ng1*NFrag, Ng2*NFrag, etc.
    void loadNoesy(char *fname, NoesyList &nl, FragmentList &lipids, FragmentListTable &all, AtomValues &m)
    {
  BMTReader bmt(fname);

  // this will hold a trajectory section for a group of protons
  SimpleTrajectory *st = new SimpleTrajectory();

  int nlips = lipids.m_Fragments.size();

  for (int i=0; i<nl.m_NTotalIds; i++) {
      nl.m_StartingIndexAllFragments.push_back(nl.m_StartingIndex[i]*nlips);
      nl.m_NIdsAllFragments.push_back(nl.m_NIds[i]*nlips);
  }

  nl.m_NTotalIdsAllFragments = nl.m_NTotalIds*nlips;

  for (int i=0; i<nl.m_NTotalIdsAllFragments; i++) {
      XYZColumn *c = new XYZColumn;
      st->m_Pos.push_back(*c);
  }

  bmt.ReadFrame();
  lipids.AssignPositions(bmt.getPositions());
  all.AssignPositions(bmt.getPositions());
  all.AssignMasses(m);

  int maxload = 10000000;
  int nframe = 0;
  while(nframe < maxload) {
      all.FindCenterOfMasses();
      for (int i=0; i<nl.m_NTotalIds; i++) {
        for (int j=0; j<nlips; j++) {
        // here the original id's are mapped into the actual fragments
        int InputIndex = i*nlips+j;
        int AtomIndex = lipids.m_Fragments[j].m_Start+nl.m_IdsAsList[i];
        st->m_Pos[InputIndex].add(bmt.getPositions()[AtomIndex]-all.m_CenterOfMass);
        // here should update the bounding box of the trajectory
        // so it knows the bounding box of all its consituent positions over time
    }
      }
      if (bmt.ReadFrame())
    break;
      nframe++;
  }

  for (int i=0; i<st->getNItems(); i++)
      st->m_Pos[i].findBoundingSphere();

  st->init(bmt.getInitialSimulationTimeInNanoSeconds(), bmt.m_NetTimeStepInPicoSeconds/1000.0);
  addTrajectory(*st);
    }

    inline double Y20(double x)
    {
  const double f = sqrt(5.0/16.0/3.14159265358979);
  return f*(3*x*x-1);
    }

    // for all members of each group, find corr and sum
    // corr involves L(rij(0))/rij(0)^3 * L(rij(t))/rij(t)^3
    // divide by NLip for ens. average.
    // do not include same proton against itself
    void findNoesyCorrStraight(Column &corr, int i, int j, NoesyList &nl, float dt, XYZ &box, float maxerr=1.5)
    {
  float delta;
  float maxrange = m_EndTime - m_StartTime;
  int ncorrtimes = maxrange/dt;
  bool starting;
  int nsource = nl.m_NIdsAllFragments[i];
  int ndest = nl.m_NIdsAllFragments[j];
  // each source/dest pair adds to corr.
  double sum = 0;
  int count = 0;

  // first fill cols and init to 0 to prepare for summation
  for (int n=0; n<ncorrtimes; n++)
      corr.addValue(0);

  // now need to loop over source/dest pairs and find corr of each, using box imaging.
  // add each corr plot to corr.

  for (int p=0; p<nsource; p++) {
      cout << "i j p last sum count" << i << " " << j << " " << p << " " << sum << " " << count << endl;
      for (int q=0; q<ndest; q++) {
    int s = nl.m_StartingIndexAllFragments[i]+p;
    int d = nl.m_StartingIndexAllFragments[j]+q;

    // skip self-self
    if (s==d)
        continue;

    // would be better to do a miss based on individ sub-traj pairings
    bool totalmiss = true;
    for (int traji = 0; traji<m_NTraj && totalmiss; traji++) {
        for (int trajj = 0; trajj<m_NTraj && totalmiss; trajj++) {
      XYZ sep = tImaging<double>::NearestImageDisplacement(m_Trajectory[traji]->m_Pos[s].m_Center, m_Trajectory[trajj]->m_Pos[d].m_Center, box);
      double rsum = m_Trajectory[traji]->m_Pos[s].m_Radius + m_Trajectory[trajj]->m_Pos[d].m_Radius;
      if (sep.Length() - rsum < 5) {
          totalmiss = false;
      }
        }
    }

    if (totalmiss) {
        cout << "... skipped source, dest " << s << " " << d << endl;
        continue;
    }

    // build stats for larger and larger deltas
    int index = 0;
    bool toofar = false;
    for (delta = dt; delta <= maxrange && !toofar; delta += dt) {
        count = 0;
        sum = 0;

        int tleft, ileft, tright, iright;
        // bump along the left index and grab data whenever there is right data delta away
        // run through all value pairs and gather stats for that value of delta
        starting = true;
        while (getNextDelta(delta, ileft, tleft, iright, tright, maxerr, starting)) {
      starting = false;
      count++;

      SimpleTrajectory *stl = m_Trajectory[tleft];
      SimpleTrajectory *str = m_Trajectory[tright];

      XYZ s0 = stl->m_Pos[s].m_Data[ileft];
      XYZ d0 = stl->m_Pos[d].m_Data[ileft];
      XYZ sT = str->m_Pos[s].m_Data[iright];
      XYZ dT = str->m_Pos[d].m_Data[iright];

      XYZ r0 = tImaging<double>::NearestImageDisplacement(s0, d0, box);
      XYZ rT = tImaging<double>::NearestImageDisplacement(sT, dT, box);

      double R0 = r0.Length();
      double R0sq = R0*R0;
      double R0cube = R0sq*R0;
      double costheta0 = r0.mZ/R0;

      double RT = rT.Length();
      double RTsq = RT*RT;
      double RTcube = RTsq*RT;
      double costhetaT = rT.mZ/RT;
      sum += Y20(costheta0)/R0cube*Y20(costhetaT)/RTcube;

        }
        if (count > 0)
          corr.bumpValue(index, sum/count*4.0/5.0/72.0);  // HACK!!! this is lipid count here for ens avg.
        index++;
        // cout << "added p q s d i j " << p << " " << q << " " << s << " " << d << " " << i << " " << j << endl;
    }
      }
  }
    }


    // for all members of each group, find corr and sum
    // corr involves L(rij(0))/rij(0)^3 * L(rij(t))/rij(t)^3
    // divide by NLip for ens. average.
    // do not include same proton against itself
    void findNoesyCorr(Column &corr, Column &serr, int i, int j, NoesyList &nl, Column &times, XYZ &box, float tcoherence, int nboot, float maxerr=1.5)
    {
  float delta;
  float maxrange = m_EndTime - m_StartTime;
  int nsource = nl.m_NIdsAllFragments[i];
  int ndest = nl.m_NIdsAllFragments[j];

  // now need to loop over source/dest pairs and find corr of each, using box imaging.
  // add each corr plot to corr.

  jrand sr(nsource);
  jrand dr(ndest);
  jrand junit;

  // build stats for larger and larger deltas
  Column sum;
  for (delta = 0; delta <= maxrange; ) {
      sum.flush();
          int ncoherence = (maxrange-delta)/tcoherence;
      if (ncoherence < 1)
    ncoherence = 1;
      int nsamples = nsource*ndest*ncoherence;

      cout << "i j nsource ndest ncoherence nsamples" << i << " " << j << " " << nsource << 
        " " << ndest << " " << ncoherence << " " << nsamples << endl;


      for (int b=0; b<nboot; b++) {
    double localsum = 0;
    for (int nsample=0; nsample<nsamples; ) {
        XYZ r0, rT;
        int s = sr.get() + nl.m_StartingIndexAllFragments[i];
        int d = dr.get() + nl.m_StartingIndexAllFragments[j];

        // skip self-self
        if (s==d)
      continue;

        // starting point has to be delta away from end
        float t0 = junit.fget()*(maxrange-delta);
        // repeat until don't fall into gap
        while (!getDeltaPair(s, d, t0, delta, r0, rT, box))
            t0 = junit.fget()*(maxrange-delta);

        double R0 = r0.Length();
        double R0sq = R0*R0;
        double R0cube = R0sq*R0;
        double costheta0 = r0.mZ/R0;

        double RT = rT.Length();
        double RTsq = RT*RT;
        double RTcube = RTsq*RT;
        double costhetaT = rT.mZ/RT;
        localsum += Y20(costheta0)/R0cube*Y20(costhetaT)/RTcube;
        nsample++;
    }
    double mean = localsum/ncoherence*4.0/5.0/72.0; // lipid count here HACK !!!!!
    sum.addValue(mean);  
    cout << i << " " << j << " " << delta << " " << b << " " << mean << endl;
      }
      sum.doStats();
      corr.addValue(sum.m_Mean);
      serr.addValue(sum.m_Sigma);
      cout << "i j dt mean sigma " << i << " " << j << " " << delta << " " << sum.m_Mean << " " << sum.m_Sigma << endl;
      times.addValue(delta);
      if (delta < 1)
    delta += 0.2;
      else if (delta < 5.0)
    delta += 0.5;
      else
    delta += 1.0;
  }
    }


    void loadTrajectoryFile(char *fname) {
  // load trajectory from binary file
  // open
  // read t0 and ts
  // read all frames
  // set end time
  XYZ v;

#if 0
  if (m_NItems != 0 && m_NItems != nitems) {
      cerr << "Already have " << m_NItems << " loaded and trying to load " << nitems
     << " from " << fname << endl;
      exit(-1);
  }
#endif
  // m_NItems = nitems;

  FILE *f = fopen(fname, "rb");
  int nread;
  float starttime, timestep;
  nread = fread(&starttime, sizeof(float), 1, f);
  assert(nread == 1);
  nread = fread(&timestep, sizeof(float), 1, f);
  assert(nread == 1);
  nread = fread(&m_NLeaflets, sizeof(int), 1, f);
  assert(nread == 1);
  nread = fread(&m_NLower, sizeof(int), 1, f);
  assert(nread == 1);
  nread = fread(&m_NUpper, sizeof(int), 1, f);
  assert(nread == 1);

  m_NItems = m_NLower+m_NUpper;

  SimpleTrajectory *st = new SimpleTrajectory();
  for (int i=0; i<m_NItems; i++) {
      XYZColumn *c = new XYZColumn;
      st->m_Pos.push_back(*c);
  }

  int nframes = 0;
  while (!feof(f)) {
      // skip COM's
      nread = fread(&v, sizeof(float), 3, f);
      if (nread != 3)
    break;
      assert(nread == 3);
      nread = fread(&v, sizeof(float), 3, f);
      assert(nread == 3);
      for (int i=0; i<m_NItems; i++) {
    nread = fread(&v, sizeof(float), 3, f);
    assert(nread == 3);
    st->m_Pos[i].add(v);
      }
      nframes++;
  }
  fclose(f);
  st->init(starttime, timestep);
  addTrajectory(*st);
    }

    // given a lipid, grab a delta_pos from set of traj's
    bool getDelta(int nlip, float t, float dt, XYZ &d) {
  XYZ p1, p2;

  for (int i=0; i<m_NTraj; i++) {
      if (m_Trajectory[i]->hasTime(t)) {
    m_Trajectory[i]->getPosition(nlip, t, p1);
    for (int j=0; j<m_NTraj; j++) {
        if (m_Trajectory[j]->hasTime(t+dt)) {
      m_Trajectory[j]->getPosition(nlip, t+dt, p2);
      d = p2-p1;
      return true;
        }
    }
      }
  }
  return false;
    }

    // given a lipid, grab a delta_pos from set of traj's
    bool getDeltaPair(int nlip1, int nlip2, float t, float dt, XYZ &d1, XYZ &d2, XYZ &box) {
  XYZ p1, p2, q1, q2;

  for (int i=0; i<m_NTraj; i++) {
      if (m_Trajectory[i]->hasTime(t)) {
    m_Trajectory[i]->getPosition(nlip1, t, p1);
    m_Trajectory[i]->getPosition(nlip2, t, q1);
    for (int j=0; j<m_NTraj; j++) {
        if (m_Trajectory[j]->hasTime(t+dt)) {
      m_Trajectory[j]->getPosition(nlip1, t+dt, p2);
      m_Trajectory[j]->getPosition(nlip2, t+dt, q2);
      d1 = tImaging<double>::NearestImageDisplacement(p1, q1, box);
      d2 = tImaging<double>::NearestImageDisplacement(p2, q2, box);
      return true;
        }
    }
      }
  }
  return false;
    }

    bool getNextPosition(int nitem, float &t, XYZ &p, bool start = false) {
  static int CurrentTraj;
  static int CurrentStep;
  static SimpleTrajectory *CurrentST;

  if (start) {
      CurrentTraj = 0;
      CurrentStep = -1;
      if (getNTrajectories()==0) {
    cerr << "No trajectories loaded" << endl;
    exit(-1);
      }
      CurrentST = m_Trajectory[CurrentTraj];
  }
  
  CurrentStep++;
  int index = CurrentStep;
  if (CurrentST->m_Reverse)
      index = CurrentST->m_NSteps-index-1;

  if (index < CurrentST->m_NSteps) {
      p = CurrentST->m_Pos[nitem].m_Data[index];
      t = CurrentST->m_StartTime + index*CurrentST->m_TimeStep;
      return true;
  }

  // if get to here, the current st has been used up
  CurrentTraj++;
  
  if (CurrentTraj >= getNTrajectories())
      return false;

  CurrentST = m_Trajectory[CurrentTraj];
  if (CurrentST->m_NSteps == 0)
      return false;

  CurrentStep = 0;
  index = CurrentStep;
  if (CurrentST->m_Reverse)
      index = CurrentST->m_NSteps-index;

  p = CurrentST->m_Pos[nitem].m_Data[index];
  t = CurrentST->m_StartTime + index*CurrentST->m_TimeStep;
  return true;
    }

    bool getIndex(float t, int &traj, int &index, float maxerr = 1.5) {
  for (traj=0; traj<getNTrajectories(); traj++) {
      if (m_Trajectory[traj]->hasTime(t)) {
    bool ok = m_Trajectory[traj]->getIndex(t, index, maxerr);
    if (!ok)
        continue;
    return ok;
      }
  }
  return false;
    }

    // for given dt, find next pair of indices that exist
    bool getNextDelta(float dt, int &ileft, int &tleft, int &iright, int &tright, float maxerr = 1.5, bool start = false) {
  static int CurrentTraj;
  static int CurrentStep;
  static SimpleTrajectory *CurrentST;

  if (start) {
      CurrentTraj = 0;
      CurrentStep = -1;
      if (getNTrajectories()==0) {
    cerr << "No trajectories loaded" << endl;
    exit(-1);
      }
      CurrentST = m_Trajectory[CurrentTraj];
  }
  
  // keep stepping along until have one on left and right
  while (1) {
      CurrentStep++;

      // is the left one out of range?
      if (CurrentStep >= CurrentST->m_NSteps) {
    CurrentTraj++;
    if (CurrentTraj >= getNTrajectories())
        return false;
    CurrentST = m_Trajectory[CurrentTraj];
    CurrentStep = -1;
    continue;
      }

      ileft = CurrentStep;
      if (CurrentST->m_Reverse)
    ileft = CurrentST->m_NSteps-CurrentStep-1;

      float timeleft = CurrentST->m_StartTime + ileft*CurrentST->m_TimeStep;

      bool ok = getIndex(timeleft+dt, tright, iright, maxerr);
      if (ok) {
    tleft = CurrentTraj;
    return true;
      }
  }
  return false;
    }



    // for each trajectory, get first pos and its time; get second pos if in range;
    // build stats for all
    // start at min time and go in steps of dt
    void diffusion2d(Column *c, Column &t, float dt, float maxerr = 1.5) {
  float delta;
  float maxrange = m_EndTime - m_StartTime;
  bool starting = true;
  double *sum = new double[m_NItems];
  int count;

  // build stats for larger and larger deltas, for each traj
  for (delta = dt; delta <= maxrange; delta += dt) {
          for (int i=0; i<m_NItems; i++)
    sum[i] = 0;
      count = 0;
      starting = true;

      int tleft, ileft, tright, iright;
      // bump along the left index and grab data whenever there is right data delta away
      // run through all value pairs and gather stats for that value of delta
      while (getNextDelta(delta, ileft, tleft, iright, tright, maxerr, starting)) {
    starting = false;
    count++;
    SimpleTrajectory *stl = m_Trajectory[tleft];
    SimpleTrajectory *str = m_Trajectory[tright];
    for (int i=0; i<m_NItems; i++) {
        XYZ left = stl->m_Pos[i].m_Data[ileft];
        XYZ right = str->m_Pos[i].m_Data[iright];
        XYZ d = right-left;
        sum[i] += d.mX*d.mX + d.mY*d.mY;
// #define TESTING
#ifdef TESTING
        if (i==0) {
      cout << count << " " << tleft << " " << ileft << " "
           << tright << " " << iright << " " << left << " " << right << " " << sum[i]/count << endl;
        }
#endif
    }
      }
      t.addValue(delta);
      for (int i=0; i<m_NItems; i++)
    c[i].addValue(sum[i]/count);
  }
    }
};


#endif // !defined(AFX_SIMPLETRAJECTORY_H__4B156D3E_174D_42E2_9658_D4F1256611FB__INCLUDED_)
