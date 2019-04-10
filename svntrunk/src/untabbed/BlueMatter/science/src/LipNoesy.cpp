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
#include <BlueMatter/SimpleTrajectory.hpp>
#include <BlueMatter/Column.hpp>
#include <BlueMatter/NoesyList.hpp>

using namespace std ;

// LipNoesy 1.bmt 2.bmt 3.bmt
// load lip_frags.txt and noesylist.h

void Usage(char **argv)
{
    cerr << argv[0] << " -ij 1 2 -coherence 1.0 -nboot 200 -mass massfile.txt -groups g.txt -fragments f.txt f1.bmt f2.bmt" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{

    if (argc < 2) {
  Usage(argv);
  exit(-1);
    }

    int I = 0;
    int J = 0;
    int nboot = 100;
    float tcoherence = 0.1;
    int nfiles = 0;
    char *fnames[100];
    char *NoesyGroupFile;
    char *FragmentListFile;
    char *FragmentMassFile;

    // LipNoesy -ij 1 2 -coherence 1.0 -nboot 200 f1.bmt f2.bmt 

    int argn=1;
    while (argn < argc) {
  if (!strcmp(argv[argn], "-ij")) {
      I = atoi(argv[++argn]);
      J = atoi(argv[++argn]);
  } else if (!strcmp(argv[argn], "-nboot")) {
      nboot = atoi(argv[++argn]);
  } else if (!strcmp(argv[argn], "-groups")) {
      NoesyGroupFile = argv[++argn];
  } else if (!strcmp(argv[argn], "-fragments")) {
      FragmentListFile = argv[++argn];
  } else if (!strcmp(argv[argn], "-mass")) {
      FragmentMassFile = argv[++argn];
  } else if (!strcmp(argv[argn], "-coherence")) {
      tcoherence = atof(argv[++argn]);
  } else if (argv[argn][0] != '-') {
      fnames[nfiles++] = argv[argn];
  } else {
      cerr << "arg " << argv[argn] << " not recognized" << endl;
      Usage(argv);
  }
  ++argn;
    }

    // check correct box
    XYZ box;
    box.mX = 22.8807f*2;
    box.mY = 22.8807f*2;
    box.mZ = 31.9384f*2;

    NoesyList nl(NoesyGroupFile);
    FragmentListTable dbl(FragmentListFile);
    tAtomValues<double> m = tAtomValues<double>(FragmentMassFile);

    FragmentList lipids = dbl.GetFragmentList("SOPE");
 
    TrajectoryLipidSet tls;
    
    // load each bmt and extract the subset of ids
    // all bmt's are assumed to be same system, but for different time period, with gaps possible
    for (int nf = 0; nf<nfiles; nf++)
  tls.loadNoesy(fnames[nf], nl, lipids, dbl, m);

    Column corr;

    Column serr;

    Column times;

    cout << "doing noesy " << I << " " << J << endl;
    cout << "nids " << nl.m_NIds[I] << " " << nl.m_NIds[J] << endl;

    tls.findNoesyCorr(corr, serr, I, J, nl, times, box, tcoherence, nboot);

    cout << "NOESY OUTPUT " << I << " " << J << endl;
    int npts = corr.m_NPts;

    for (int k=0; k<npts; k++) {
  cout << times.m_Data[k] << " " << corr.m_Data[k] << " " << serr.m_Data[k] << endl; 
    }

    cout << endl;

    return 0;
}



#if 0

    // calc. only half matrix
    int ncorrs = (nl.m_NGroups*nl.m_NGroups + nl.m_NGroups)/2;

    cout << "ncorrs " << ncorrs << endl;

    Column *corr = new Column[ncorrs];

    Column *serr = new Column[ncorrs];

    Column *times = new Column[ncorrs];

    int ncorrstocalc = (maxi*maxi+maxi)/2;


    for (int i=0; i<maxi; i++) {
  for (int j=0; j<=i; j++) {
      // loop over id's in each group and find sum of corrs for each. store in array
      // each ij involves the sum of a large number of corrs over both group members and frags
      int corri = i*nl.m_NGroups+j;
      cout << "doing noesy " << i << " " << j << " " << corri << endl;
      cout << "nids " << nl.m_NIds[i] << " " << nl.m_NIds[j] << endl;

      tls.findNoesyCorr(corr[corri], serr[corri], i, j, nl, times[corri], box, tcoherence);
      // tls.findNoesyCorrStraight(corr[corri], i, j, nl, dt, box);
      cout << "ZZZ NOESY OUTPUT " << i << j << endl;
      int npts = corr[corri].m_NPts;

      for (int k=0; k<npts; k++) {
    cout << "ZZZ " << times[corri].m_Data[k] << " " << corr[corri].m_Data[k] << " " << serr[corri].m_Data[k] << endl; 
      }
      cout << endl;
  }
    }

    int npts = corr[0].m_Data.size();

    for (i=0; i<npts; i++) {
  cout << times[0].m_Data[i] << " ";
  for (int j=0; j<ncorrstocalc; j++) {
      cout << corr[j].m_Data[i] << " " << serr[j].m_Data[i] << " "; 
  }
  cout << endl;
    }

#endif
