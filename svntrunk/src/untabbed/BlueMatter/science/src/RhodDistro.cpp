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
#include <BlueMatter/Align.hpp>

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt -skip N" << endl;
    cerr << "Input bmt containing rhod system" << endl;
    exit(-1);
}

// bmt2histo   sppdchol.bmt  out.txt fragspec.txt bond.txt
int main(int argc, char **argv)
{
    SimpleTrajectory st;

    fXYZ box;
    box.mX = 27.5*2;
    box.mY = 38.5*2;
    box.mZ = 51.3225*2;

    int nleaflets = 1;

    int nskip = 0;

    if (argc < 2)
  Usage(argv);

    char *bmtname = argv[1];

    cerr << "About to open " << argv[1] << endl;
    BMTReader bmt(argv[1]);
    cerr << "BMT open" << endl;

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
    cerr << "Start is " << actualstart << endl;

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

    cerr << "About to read frame" << endl;
    if (bmt.ReadFrame()) {
  cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
  exit(-1);
    }

    cerr << "Opening frags file" << endl;
    //fFragmentListTable flt("../rhod_frags_dry.txt");
    fFragmentListTable flt("../rhod_frags_dry_fine.txt");
    cerr << "Opening mass file" << endl;
    fAtomValues m("../rhod_mass_dry.txt");

    cerr << "defining fragment lists" << endl;
    fFragmentList rhod = flt.GetFragmentList("RHOD");
    fFragmentList chol = flt.GetFragmentList("CHOL");
    fFragmentList steae = flt.GetFragmentList("STEAE");
    fFragmentList steac = flt.GetFragmentList("STEAC");
    fFragmentList pegl = flt.GetFragmentList("PEGL");
    fFragmentList dhae = flt.GetFragmentList("DHAE");
    fFragmentList dhac = flt.GetFragmentList("DHAC");
    fFragmentList pcgl = flt.GetFragmentList("PCGL");
    fFragmentList palm = flt.GetFragmentList("PALM");
    fFragmentList sod = flt.GetFragmentList("SOD");
    fFragmentList cla = flt.GetFragmentList("CLA");

    cerr << "Assigning positions" << endl;

    flt.AssignPositions(bmt.getPositions());

    cerr << "Assigning masses" << endl;
    flt.AssignMasses(m);

#if 1
    rhod.AssignPositions(bmt.getPositions());
    chol.AssignPositions(bmt.getPositions());
    steae.AssignPositions(bmt.getPositions());
    steac.AssignPositions(bmt.getPositions());
    pegl.AssignPositions(bmt.getPositions());
    dhae.AssignPositions(bmt.getPositions());
    dhac.AssignPositions(bmt.getPositions());
    pcgl.AssignPositions(bmt.getPositions());
    palm.AssignPositions(bmt.getPositions());
    sod.AssignPositions(bmt.getPositions());
    cla.AssignPositions(bmt.getPositions());

    rhod.AssignMasses(m);
    chol.AssignMasses(m);
    steae.AssignMasses(m);
    steac.AssignMasses(m);
    pegl.AssignMasses(m);
    dhae.AssignMasses(m);
    dhac.AssignMasses(m);
    pcgl.AssignMasses(m);
    palm.AssignMasses(m);
    sod.AssignMasses(m);
    cla.AssignMasses(m);
#endif


    fFragment &r = rhod.m_Fragments[0];
    tAlign<float> al(r);
#if 0
    int er = 0;
    while (!er) {
  cerr << al.FindRMSD(r) << endl;
  er = bmt.ReadFrame();
    }
    exit(-1);
#endif
    cerr << "finding table center of masses" << endl;
    flt.FindCenterOfMasses();

    cerr << "finding rhod center of mass" << endl;
    rhod.FindCenterOfMasses();
    // chol.FindCenterOfMasses();

    cerr << "starting histogram" << endl;
    fHistogram *hrhod = new fHistogram;
    hrhod->Init("rhod_rhod_al", 120.0, 200);
    cerr << "just initd rhod histo" << endl;
    fHistogram *hchol = new fHistogram;
    hchol->Init("rhod_chol_al", 120.0, 200);
    fHistogram *hsteae = new fHistogram;
    hsteae->Init("rhod_steae_al", 120.0, 200);
    fHistogram *hsteac = new fHistogram;
    hsteac->Init("rhod_steac_al", 120.0, 200);
    fHistogram *hpegl = new fHistogram;
    hpegl->Init("rhod_pegl_al", 120.0, 200);
    cerr << "pegl" << endl;
    fHistogram *hdhae = new fHistogram;
    hdhae->Init("rhod_dhae_al", 120.0, 200);
    fHistogram *hdhac = new fHistogram;
    hdhac->Init("rhod_dhac_al", 120.0, 200);

    fHistogram *hpcgl = new fHistogram;
    hpcgl->Init("rhod_pcgl_al", 120.0, 200);
    cerr << "pcgl" << endl;

    fHistogram *hpalm = new fHistogram;
    hpalm->Init("rhod_palm_al", 120.0, 200);
    cerr << "palm" << endl;
    fHistogram *hsod = new fHistogram;
    hsod->Init("rhod_sod_al", 120.0, 200);
    cerr << "sod" << endl;
    fHistogram *hcla = new fHistogram;
    cerr << "just newd cla_al" << endl;
    hcla->Init("rhod_cla", 120.0, 200);

    // cerr << "initd all histos" << endl;
    // exit(-1);

    // fXYZ &palm = bmt.getPositions()[5512];
    int readerr = 0;
    int n = 0;
    while (!readerr && n < 100000000) {

  // fXYZ v = palm-rhod.m_CenterOfMass;
        // cout << v.mX << " " << v.mY << " " << v.mZ << endl;
  // define transform based on rhod, then immediately apply it so center of mass is appropriate
  al.DefineTransform(&rhod.m_Fragments[0].m_Positions[rhod.m_Fragments[0].m_Start]);
  al.ApplyTransform(rhod.m_Fragments[0]);

  rhod.FindCenterOfMasses();

#if 0
  for (vector<fFragment>::iterator f = chol.m_Fragments.begin(); f != chol.m_Fragments.end(); f++) {
      fXYZ v = fImaging::NearestImageDisplacement(rhod.m_CenterOfMass, f->m_CenterOfMass, box);
      hchol->AddPoint(v);
      cout << v.mX << " " << v.mY << " " << v.mZ << endl;
  }
#endif	
  if (n==0)
      cerr << "about to add rhod frag" << endl;
  for (vector<fFragment>::iterator f = rhod.m_Fragments.begin(); f != rhod.m_Fragments.end(); f++) {
      // al.ApplyTransform(*f);
      hrhod->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  if (n==0)
      cerr << "about to add chol frag" << endl;
  for (vector<fFragment>::iterator f = chol.m_Fragments.begin(); f != chol.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hchol->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = steae.m_Fragments.begin(); f != steae.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hsteae->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = steac.m_Fragments.begin(); f != steac.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hsteac->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = pegl.m_Fragments.begin(); f != pegl.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hpegl->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = dhae.m_Fragments.begin(); f != dhae.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hdhae->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = dhac.m_Fragments.begin(); f != dhac.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hdhac->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = pcgl.m_Fragments.begin(); f != pcgl.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hpcgl->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = palm.m_Fragments.begin(); f != palm.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hpalm->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = sod.m_Fragments.begin(); f != sod.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hsod->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  for (vector<fFragment>::iterator f = cla.m_Fragments.begin(); f != cla.m_Fragments.end(); f++) {
      al.ApplyTransform(*f);
      hcla->AddFragmentAtomsNearestImageDisplacement(rhod.m_CenterOfMass, *f, box);
  }
  if (n==0)
      cerr << "just added all frags" << endl;

  readerr = bmt.ReadFrame();
  // cerr << "reading frame" << endl;

  // rhod.FindCenterOfMasses();
  // chol.FindCenterOfMasses();
  n++;
    }

    cerr << "about to dump rhod histo" << endl;
    hrhod->DumpDX(1, 1, rhod.GetTotalAtomCount(), 1, n);
    cerr << "about to dump chol histo" << endl;
    hchol->DumpDX(1, 1, chol.GetTotalAtomCount(), 1, n);
    hsteae->DumpDX(1, 1, steae.GetTotalAtomCount(), 1, n);
    hsteac->DumpDX(1, 1, steac.GetTotalAtomCount(), 1, n);
    hpegl->DumpDX(1, 1, pegl.GetTotalAtomCount(), 1, n);
    hdhae->DumpDX(1, 1, dhae.GetTotalAtomCount(), 1, n);
    hdhac->DumpDX(1, 1, dhac.GetTotalAtomCount(), 1, n);
    hpcgl->DumpDX(1, 1, pcgl.GetTotalAtomCount(), 1, n);
    hpalm->DumpDX(1, 1, palm.GetTotalAtomCount(), 1, n);
    hsod->DumpDX(1, 1, sod.GetTotalAtomCount(), 1, n);
    hcla->DumpDX(1, 1, cla.GetTotalAtomCount(), 1, n);

    cerr << "just dumped all histos" << endl;

    return 0;
}
