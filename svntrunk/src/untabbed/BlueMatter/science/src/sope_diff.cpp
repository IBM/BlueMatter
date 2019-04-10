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
#include <BlueMatter/Column.hpp>

using namespace std ;

int main(int argc, char **argv)
{
    if (argc < 2) {
  cerr << argv[0] << " fname.bmt nskip" << endl;
  cerr << "Input bmt containing 72 lipids in two leaflets with box 22.8807x2 by 228807x2 by 31.9384x2" << endl;
  cerr << "Output xyz of lower then upper leaflets, after removing leaflet COM drift, referenced to first position" << endl;
  cerr << "Requires sope_frags.txt and wetsope.masses.dat in local directory" << endl;
  cerr << "nskip will skip over first part of trajectory" << endl;
  exit(-1);
    }

    XYZ box;
    box.mX = 22.8807f*2;
    box.mY = 22.8807f*2;
    box.mZ = 31.9384f*2;

    int nskip = 0;
    if (argc > 2)
  nskip = atoi(argv[2]);

    BMTReader bmt(argv[1]);
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


    FragmentListTable dbl("/fs/lipid5/sope_frags.txt");
    FragmentList &fl = dbl.GetFragmentList("SOPE");
    AtomValues m("/fs/lipid5/wetsope.masses.dat");
    
    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);

    // first need com's to partition leaflets
    fl.FindCenterOfMasses();
    // cout << fl.m_CenterOfMass << endl;
    FragmentList upper("upper"), lower("lower");
    fl.SplitUpperLower(lower, upper);
    // cout << lower.m_Fragments.size() << endl;

    // now init starting data for each leaflet separately
    lower.FindStartingCenterOfMasses(box);
    Fragment &f = lower.m_Fragments[0];
    // cout << f.m_Center << " " << f.m_CenterOfMass << " " << f.m_StartingPosition << " " << f.m_StartingTranslatedPosition << " " << f.m_StartingTranslationVector <<endl;
    upper.FindStartingCenterOfMasses(box);

    // cout << lower.m_CenterOfMass << endl;

    Column dlower[36];
    Column dupper[36];
    Column slower[36];
    Column supper[36];

    int readerr = 0;
    int n = 0;
    while (!readerr && n < 100000000) {
  XYZ lowerdcom = lower.m_CenterOfMass - lower.m_StartingCenterOfMass;
  XYZ upperdcom = upper.m_CenterOfMass - upper.m_StartingCenterOfMass;
  // cout << lowerdcom.mX << " " << lowerdcom.mY << " " << lowerdcom.mZ << " "
  //     << upperdcom.mX << " " << upperdcom.mY << " " << upperdcom.mZ << endl;
  int i=0;
  for (vector<Fragment>::iterator f=lower.m_Fragments.begin(); f != lower.m_Fragments.end(); f++) {
      XYZ d = f->m_CenterOfMass - f->m_StartingTranslationVector - lowerdcom;
      cout << d.mX << " " << d.mY << " " << d.mZ << endl;
      XYZ v = f->GetLocalMotion(d);
      // cout << sqrt(v.mX*v.mX + v.mY*v.mY) << " ";
      double dist = sqrt(v.mX*v.mX + v.mY*v.mY);
      dlower[i].addValue(dist);
      i++;
  }
  i = 0;
  for (vector<Fragment>::iterator f=upper.m_Fragments.begin(); f != upper.m_Fragments.end(); f++) {
      XYZ d = f->m_CenterOfMass - f->m_StartingTranslationVector - upperdcom;
      cout << d.mX << " " << d.mY << " " << d.mZ << endl;
      XYZ v = f->GetLocalMotion(d);
      // cout << sqrt(v.mX*v.mX + v.mY*v.mY) << " ";
      double dist = sqrt(v.mX*v.mX + v.mY*v.mY);
      dupper[i].addValue(dist);
      i++;
  }
  // cout << endl;
  readerr = bmt.ReadFrame();
  lower.FindCenterOfMasses(true);
  upper.FindCenterOfMasses(true);

  n++;
    }
#if 0
    for (int i=0; i<36; i++) {
  dlower[i].smooth(slower[i], 10);
  dupper[i].smooth(supper[i], 10);
    }

    for (int s=0; s<slower[0].m_NPts; s++) {
  for (int i=0; i<36; i++)
      cout << slower[i].m_Data[s] << " ";
  for (int i=0; i<36; i++)
      cout << supper[i].m_Data[s] << " ";
  cout << endl;
    }
#endif
    return 0;
}
