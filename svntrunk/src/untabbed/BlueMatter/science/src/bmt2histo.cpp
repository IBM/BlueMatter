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
 
#include <math.h>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/DXHistogram.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt -skip N" << endl;
    cerr << "Input bmt containing items in two leaflets" << endl;
    cerr << "Output xyz of lower then upper leaflets, after removing leaflet COM drift, imaged to first arrangement" << endl;
    cerr << "-skip causes it to skip first N frames" << endl;
    exit(-1);
}

// bmt2histo   sppdchol.bmt  out.txt fragspec.txt bond.txt
int main(int argc, char **argv)
{
    SimpleTrajectory st;

    fXYZ box;
    box.mX = box.mY = 52.96;
    box.mZ = 63.7028;

    int nleaflets = 2;

    int nskip = 0;

    if (argc < 2)
  Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);

    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;

    int argn=2;
    while (argn < argc) {
  if (!strcmp(argv[argn], "-skip")) {
      nskip = atoi(argv[++argn]);
  } else {
      cerr << "arg " << argv[argn] << " not recognized" << endl;
      Usage(argv);
  }
  ++argn;
    }

    float actualstart = st.m_StartTime+nskip*st.m_TimeStep;

    if (nskip > 0) {
  cerr << "Skipping first " << nskip << " frames" << endl;
  cerr << "Starting at " << actualstart << " nanoseconds" << endl;
    }
    for (int i=0; i<nskip; i++) {
  if (bmt.ReadFrame()) {
      cerr << "Error reading " << i << " frame during skip" << endl;
      exit(-1);
  }
    }

    if (bmt.ReadFrame()) {
  cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
  exit(-1);
    }

    fFragmentListTable fltbody("/fs/lipchol2/cholsub.txt");
    // fFragmentListTable flt("/fs/lipchol2/sdpcchol_frags.txt");
    fFragmentListTable flt("/fs/lipchol2/lipchol_hbond_frag.txt");
    fAtomValues m("/fs/lipchol2/sdpcchol_masses.txt");

    // flt.deleteFragmentList("HOH");
    // flt.deleteFragmentList("TIP3");
    
    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);
    fltbody.AssignPositions(bmt.getPositions());
    fltbody.AssignMasses(m);

    flt.FindCenterOfMasses();
    fltbody.FindCenterOfMasses();

    fFragmentListTable upperbody, lowerbody;
    fltbody.SplitUpperLower(lowerbody, upperbody);

    lowerbody.FindCenterOfMasses();
    upperbody.FindCenterOfMasses();

    int nlower = lowerbody.m_FragmentLists[0].m_Fragments.size();
    int nupper = upperbody.m_FragmentLists[0].m_Fragments.size();

    cout << "NLower, NUpper: " << nlower << " " << nupper << endl;


    // load list of bonds as vectors.
    fFragmentList &flbody = fltbody.GetFragmentList("CHOLSUB");
    fFragmentList &fllowerbody = lowerbody.GetFragmentList("CHOLSUB");
    fFragmentList &flupperbody = upperbody.GetFragmentList("CHOLSUB");
    fBondList bl(flbody, 25, 26);  // load list of vectors  C13C18[]
    fBondList upbl(flbody, 25, 38);  // approx up direction in CHOL
    fBondList bllower(fllowerbody, 25, 26);  // load list of vectors  C13C18[]
    fBondList upbllower(fllowerbody, 25, 38);  // approx up direction in CHOL
    fBondList blupper(flupperbody, 25, 26);  // load list of vectors  C13C18[]
    fBondList upblupper(flupperbody, 25, 38);  // approx up direction in CHOL


    bl.AssignPositions(bmt.getPositions());
    upbl.AssignPositions(bmt.getPositions());
    bllower.AssignPositions(bmt.getPositions());
    upbllower.AssignPositions(bmt.getPositions());
    blupper.AssignPositions(bmt.getPositions());
    upblupper.AssignPositions(bmt.getPositions());



    // FILE **outfiles = new FILE *[flt.m_FragmentLists.size()];
    fHistogram *h = new fHistogram[2*flt.m_FragmentLists.size()];
    for (int i=0; i<flt.m_FragmentLists.size(); i++) {
  char fname[512];
  sprintf(fname, "%s_lower_all_pos", flt.m_FragmentLists[i].m_Name);
  h[2*i].Init(fname, 70.0, 140);
  sprintf(fname, "%s_upper_all_pos", flt.m_FragmentLists[i].m_Name);
  h[2*i+1].Init(fname, 70.0, 140);
  //outfiles[i] = fopen(fname, "w");
  //assert(outfiles[i]);
    }

    int readerr = 0;
    int n = 0;
    while (!readerr && n < 100000000) {
  // flbody.FindGeometricCenters();
  // only need principal axes of chol
  fllowerbody.FindPrincipalAxes();
  flupperbody.FindPrincipalAxes();
  // project bond based on corresponding fragment principal axes
  upbllower.Init();
  bllower.Init(fllowerbody, upbllower);
  upblupper.Init();
  blupper.Init(flupperbody, upblupper);

  for (vector<fBond>::iterator b = bllower.m_Bonds.begin(); b != bllower.m_Bonds.end(); b++) {
      for (int i=0; i<flt.m_FragmentLists.size(); i++) {
    fFragmentList &fl = flt.m_FragmentLists[i];
    for (vector<fFragment>::iterator f=fl.m_Fragments.begin(); f != fl.m_Fragments.end(); f++) {
        for (int k=f->m_Start; k<f->m_Start+f->m_Count; k++) {
      fXYZ v = b->FindNearestXYZDelta(f->m_Positions[k], box);
      h[2*i].AddPoint(v);
        }
    }
      }
  }

  for (vector<fBond>::iterator b = blupper.m_Bonds.begin(); b != blupper.m_Bonds.end(); b++) {
      for (int i=0; i<flt.m_FragmentLists.size(); i++) {
    fFragmentList &fl = flt.m_FragmentLists[i];
    for (vector<fFragment>::iterator f=fl.m_Fragments.begin(); f != fl.m_Fragments.end(); f++) {
        for (int k=f->m_Start; k<f->m_Start+f->m_Count; k++) {
      fXYZ v = b->FindNearestXYZDelta(f->m_Positions[k], box);
      h[2*i+1].AddPoint(v);
        }
    }
      }
  }

  
  readerr = bmt.ReadFrame();
  // for (int skip=0; skip<50; skip++)
  //    readerr = bmt.ReadFrame();

  n++;
    }

    for (int j=0; j<flt.m_FragmentLists.size(); j++) {
  //fclose(outfiles[j]);
  h[2*j  ].DumpDX(fllowerbody.m_Fragments.size(), fllowerbody.m_Fragments[0].m_Count, flt.m_FragmentLists[j].m_Fragments.size(),
      flt.m_FragmentLists[j].m_Fragments[0].m_Count, n);
  h[2*j+1].DumpDX(flupperbody.m_Fragments.size(), flupperbody.m_Fragments[0].m_Count, flt.m_FragmentLists[j].m_Fragments.size(),
      flt.m_FragmentLists[j].m_Fragments[0].m_Count, n);
    }

    return 0;
}
