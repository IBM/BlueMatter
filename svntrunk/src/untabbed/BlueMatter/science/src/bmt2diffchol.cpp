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
#include <BlueMatter/Column.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt OutName -skip N" << endl;
    cerr << "Input bmt containing items in two leaflets" << endl;
    cerr << "Output xyz of lower then upper leaflets, after removing leaflet COM drift, imaged to first arrangement" << endl;
    cerr << "-skip causes it to skip first N frames" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    if (argc < 3)
  Usage(argv);

    fXYZ box;
    box.mX = box.mY = 52.96;
    box.mZ = 63.7028;

    int nleaflets = 2;

    int nskip = 0;

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);

    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;
    char *outname = argv[2];

    int argn=3;
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

    FILE *fout = fopen(outname, "wb");
    fwrite(&actualstart, sizeof(float), 1, fout);  // write 4
    fwrite(&st.m_TimeStep, sizeof(float), 1, fout);   // write 4


    fFragmentListTable flt("/fs/lipchol2/sdpcchol_frags.txt");
    fAtomValues m("/fs/lipchol2/sdpcchol_masses.txt");

    flt.deleteFragmentList("TIP3");
    
    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);

    fFragmentListTable upperall, lowerall;
    flt.SplitUpperLower(lowerall, upperall);

    // now init starting data for each leaflet separately
    lowerall.FindCenterOfMasses();
    upperall.FindCenterOfMasses();

    lowerall.SetPreviousPositions();
    upperall.SetPreviousPositions();

    // fFragmentList &lower = lowerall.GetFragmentList("CHOL");
    // fFragmentList &upper = upperall.GetFragmentList("CHOL");

    fFragmentList &lower = lowerall.GetFragmentList("LIPID");
    fFragmentList &upper = upperall.GetFragmentList("LIPID");


    lower.FindCenterOfMasses();
    upper.FindCenterOfMasses();

    lower.SetPreviousPositions();
    upper.SetPreviousPositions();

    int nlower = lower.m_Fragments.size();
    int nupper = upper.m_Fragments.size();

    // output nleaflets, which is number of com's that prefix each list of items
    fwrite(&nleaflets, sizeof(int), 1, fout);  // write 4
    fwrite(&nlower, sizeof(int), 1, fout);  // write 4
    fwrite(&nupper, sizeof(int), 1, fout);   // write 4

    // out file is 20 + (ntot+2)*3*4*n  bytes

    cout << "NLower, NUpper: " << nlower << " " << nupper << endl;

    int readerr = 0;
    int nwritten;
    int n = 0;
    while (!readerr && n < 100000000) {

  lower.ImageToNearPrevious(box);
  upper.ImageToNearPrevious(box);
  lowerall.ImageToNearPrevious(box);
  upperall.ImageToNearPrevious(box);

  fXYZ lowerdcom = lowerall.FindAggregateCenterOfMass();
  fXYZ upperdcom = upperall.FindAggregateCenterOfMass();

  nwritten = fwrite(&lowerdcom, sizeof(float), 3, fout);
  assert(nwritten == 3);
  nwritten = fwrite(&upperdcom, sizeof(float), 3, fout);
  assert(nwritten == 3);

  bool first = true;
  for (vector<fFragment>::iterator f=lower.m_Fragments.begin(); f != lower.m_Fragments.end(); f++) {
      fXYZ d = f->m_CenterOfMass - lowerdcom;
      //if (first)
    // cout << f->m_CenterOfMass << " " << lowerdcom << " " << d << endl;
      first = false;
      nwritten = fwrite(&d, sizeof(float), 3, fout);
      assert(nwritten == 3);
  }

  first = true;
  for (vector<fFragment>::iterator f=upper.m_Fragments.begin(); f != upper.m_Fragments.end(); f++) {
      fXYZ d = f->m_CenterOfMass - upperdcom;
      //if (first)
    // cout << f->m_CenterOfMass << " " << upperdcom << " " << d << endl;
      first = false;
      nwritten = fwrite(&d, sizeof(float), 3, fout);
      assert(nwritten == 3);
  }

  lower.SetPreviousPositions();
  upper.SetPreviousPositions();
  lowerall.SetPreviousPositions();
  upperall.SetPreviousPositions();

  readerr = bmt.ReadFrame();
  
  lower.FindCenterOfMasses();
  upper.FindCenterOfMasses();
  lowerall.FindCenterOfMasses();
  upperall.FindCenterOfMasses();

  n++;
    }
    fclose(fout);
    return 0;
}
