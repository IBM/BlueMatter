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
#include <BlueMatter/SurfaceDistance.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " test.bmt test.frag " << endl;
    exit(-1);
}

// bmt2histo   sppdchol.bmt  out.txt fragspec.txt bond.txt
int main(int argc, char **argv)
{
    XYZ box;

    cerr << "loading " << argv[1] << endl;
    BMTReader bmt(argv[1]);

    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX;
    box.mY = myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY;
    box.mZ = myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ;

    //box.mX = bmt.m_InitialBox.mMaxBox.mX - bmt.m_InitialBox.mMinBox.mX;
    //box.mY = bmt.m_InitialBox.mMaxBox.mY - bmt.m_InitialBox.mMinBox.mY;
    //box.mZ = bmt.m_InitialBox.mMaxBox.mZ - bmt.m_InitialBox.mMinBox.mZ;

    cerr << "initial box " << box << endl;

    if (bmt.ReadFrame()) {
  cerr << "bmtreaderror" << endl;
  exit(-1);
    }

    cerr << "loading fragment list table " << argv[2] << endl;

    FragmentListTable dbl(argv[2]);

    // dbl.Print();

    dbl.AssignPositions(bmt.getPositions());

    FragmentList &rhod = dbl.GetFragmentList("RHOD");
    FragmentList &dha = dbl.GetFragmentList("DHA");

    vector<XYZ> allrhod;
    vector<XYZ> alldha;

    for (int i=0; i<200; i++)
  bmt.ReadFrame();

    int rerror = 0;
    while (!rerror) {
  rhod.FindGeometricCenters();
  XYZ &cog = rhod.m_Center;
  for (int j=0; j<rhod.m_Fragments.size(); j++) {
      Fragment frag = rhod.m_Fragments[j];
      for (int i=0; i<frag.m_Count; i++) {
    XYZ f = frag.m_Positions[frag.m_Start+i]-cog;
    // float tmp = f.mX;
    // f.mX = -f.mY;
    // f.mY = tmp;
    allrhod.push_back(f);
      }
  }
  for (int i=0; i<dha.m_Fragments.size(); i++) {
      Fragment frag = dha.m_Fragments[i];
      for (int j=0; j<frag.m_Count; j++) {
    XYZ f = Imaging::NearestImageDisplacement(cog, frag.m_Positions[frag.m_Start+j], box);
    alldha.push_back(f);
      }
  }
  rerror = bmt.ReadFrame();
    }

    SurfaceDistance sd(36, -40, 40.0, 32);
    sd.AddPoints(allrhod);
    sd.Print();

    const int nbins = 140;
    int histo[nbins];
    float rmax = 50;
    float rmin = -20;
    float range = rmax-rmin;
    for (int i=0; i<nbins; i++)
  histo[i] = 0;
    for (int i=0; i<alldha.size(); i++) {
  XYZ f = alldha[i];
  // float tmp = f.mX;
  // f.mX = -f.mY;
  // f.mY = tmp;
  int bin = (sd.Distance(f)-rmin)/range*nbins;
  histo[bin]++;
    }

    for (int i=0; i<nbins; i++) {
  cerr << rmin+i*(range/nbins) << " " << histo[i] << endl;
    }

    return 0;
}
