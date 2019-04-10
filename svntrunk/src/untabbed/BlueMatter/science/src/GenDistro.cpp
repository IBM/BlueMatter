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
    cerr << argv[0] << " FName.bmt frag.txt masses.txt -skip N -length N -align comp" << endl;
    cerr << "Input bmt and alignment component" << endl;
    exit(-1);
}

// bmt2histo   sppdchol.bmt  out.txt fragspec.txt bond.txt
int main(int argc, char **argv)
{
    SimpleTrajectory st;

    XYZ box;

    int nskip = 0;
    int nlength = 1000000000;
    double binwidth = 0.6;

    if (argc < 3)
  Usage(argv);

    char *bmtfilename = argv[1];
    char *fragfilename = argv[2];
    char *massfilename = argv[3];
    char *aligncompname = NULL;

    cerr << "About to open " << bmtfilename << endl;
    BMTReader bmt(bmtfilename);
    cerr << "BMT open" << endl;

    // AK: changed access
    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;

    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX;
    box.mY = myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY;
    box.mZ = myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ;
    //box.mX = bmt.m_InitialBox.mMaxBox.mX - bmt.m_InitialBox.mMinBox.mX;
    //box.mY = bmt.m_InitialBox.mMaxBox.mY - bmt.m_InitialBox.mMinBox.mY;
    //box.mZ = bmt.m_InitialBox.mMaxBox.mZ - bmt.m_InitialBox.mMinBox.mZ;

    if (fabs(box.mX) < 1 || fabs(box.mY) < 1 || fabs(box.mZ) < 1) {
  cerr << "Box from bmt is missing or too small: " << myInitBox.mMinBox << " " << myInitBox.mMaxBox  << endl;
  exit(-1);
    }

    cerr << "Box from BMT: " << myInitBox.mMinBox << " " << myInitBox.mMaxBox  << endl;
    cerr << "Net box: " << box << endl;

    double maxd = box.mX;
    if (maxd < box.mY)
  maxd = box.mY;
    if (maxd < box.mZ)
  maxd = box.mZ;

    double histowidth = maxd*1.1;
    int nbins = histowidth/binwidth;

    cerr << "BinWidth " << binwidth << endl;
    cerr << "NBins " << nbins << endl;
    cerr << "BinBoxWidth " << histowidth << endl;

    int argn=4;
    while (argn < argc) {
  if (!strcmp(argv[argn], "-skip")) {
      nskip = atoi(argv[++argn]);
  } else if (!strcmp(argv[argn], "-length")) {
      nlength = atoi(argv[++argn]);
  } else if (!strcmp(argv[argn], "-align")) {
      aligncompname = argv[++argn];
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

    cerr << "Opening frags file " << fragfilename << endl;
    FragmentListTable dbl(fragfilename);

    cerr << "Opening mass file " << massfilename << endl;
    AtomValues m(massfilename);

    // use first fragment list by default
    FragmentList *flrmsd = &dbl.m_FragmentLists[0];
    if (aligncompname)
  flrmsd = &dbl.GetFragmentList(aligncompname);
    Fragment &frmsd = flrmsd->m_Fragments[0];

    cerr << "Assigning positions" << endl;

    dbl.AssignPositions(bmt.getPositions());

    cerr << "Assigning masses" << endl;
    dbl.AssignMasses(m);

    // Initialize the alignment object using the requested fragment
    // Alignment needs to know the full number of atoms so it can allocate an internal buffer
    tAlign<double> al(frmsd, bmt.getNAtoms(), box);
    vector<Histogram *> vh;

    dbl.Print();

    FragmentList &chol = dbl.GetFragmentList("CHOL");
    Fragment &rhod = dbl.GetFragmentList("RHOD").m_Fragments[0];

    // init each histogram in order of frag lists
    for (vector<FragmentList>::iterator fl = dbl.m_FragmentLists.begin();
   fl != dbl.m_FragmentLists.end();
   fl++)
    {
  char buff[1024];
  sprintf(buff, "histo_%s", fl->m_Name);
  Histogram *h = new Histogram;
  h->Init(buff, histowidth, nbins);
  vh.push_back(h);
    }

    int readerr = 0;
    int n = 0;

    if (aligncompname) {
  cerr << "Aligning on " << aligncompname << endl;
    }

    cerr << "Reading " << nlength << " frames" << endl;
    while (!readerr && n < nlength) {

  if (aligncompname) {
      al.DefineTransform(&frmsd.m_Positions[frmsd.m_Start]);
  }

  dbl.FindCenterOfMasses();

  for (int i=0; i<dbl.m_FragmentLists.size(); i++) {
      FragmentList &fl = dbl.m_FragmentLists[i];
      for (int j=0; j<fl.m_Fragments.size(); j++) {
    // al.ApplyTransform(fl.m_Fragments[j]);
    // vh[i]->AddNearestImageAligned(fl.m_Fragments[j], al);

      }
  }

  for (int i=0; i<chol.m_Fragments.size(); i++) {
      XYZ d = Imaging::NearestImageDisplacement(rhod.m_CenterOfMass, chol.m_Fragments[i].m_CenterOfMass, box);
      cout << d.mX << " " << d.mY << " " << d.mZ << endl;
  }
      

  readerr = bmt.ReadFrame();
  n++;
    }

    cerr << "dump histos" << endl;

    // for (int i=0; i<vh.size(); i++)
//	vh[i]->DumpDX(1, 1, 1, dbl.m_FragmentLists[i].GetTotalAtomCount(), n);

    cerr << "all histos dumped" << endl;

    return 0;
}
