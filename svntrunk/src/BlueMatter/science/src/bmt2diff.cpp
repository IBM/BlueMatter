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

// bmt2histo   sppdchol.bmt  out.txt fragspec.txt bond.txt
int main(int argc, char **argv)
{
    SimpleTrajectory st;

    if (argc < 3) {
	cerr << argv[0] << " FName.bmt OutName" << endl;
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

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);

    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;
    char *outname = argv[2];

#if 0
    for (int i=0; i<nskip; i++) {
	if (bmt.ReadFrame()) {
	    cerr << "Error reading " << i << " frame during skip" << endl;
	    exit(-1);
	}
    }
#endif

    if (bmt.ReadFrame()) {
	cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
	exit(-1);
    }

    FILE *fout = fopen(outname, "wb");
    fwrite(&st.m_StartTime, sizeof(float), 1, fout);  // write 4
    fwrite(&st.m_TimeStep, sizeof(float), 1, fout);   // write 4

    // out file is 8 + (72+2)*3*4*n  bytes

    FragmentListTable dbl("sope_dry_frags.txt");
    FragmentList &fl = dbl.GetFragmentList("SOPE");
    AtomValues m("sope_dry_masses.txt");
    
    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);

    // first need com's to partition leaflets
    fl.FindCenterOfMasses();
    FragmentList upper("upper"), lower("lower");
    fl.SplitUpperLower(lower, upper);

    // now init starting data for each leaflet separately
    lower.FindCenterOfMasses();
    Fragment &f = lower.m_Fragments[0];
    upper.FindCenterOfMasses();

    int readerr = 0;
    int nwritten;
    int n = 0;
    while (!readerr && n < 100000000) {
	XYZ lowerdcom = lower.m_CenterOfMass;
	XYZ upperdcom = upper.m_CenterOfMass;

	nwritten = fwrite(&lowerdcom, sizeof(float), 3, fout);
	assert(nwritten == 3);
	nwritten = fwrite(&upperdcom, sizeof(float), 3, fout);
	assert(nwritten == 3);

	for (vector<Fragment>::iterator f=lower.m_Fragments.begin(); f != lower.m_Fragments.end(); f++) {
	    XYZ d = f->m_CenterOfMass - lowerdcom;
	    nwritten = fwrite(&d, sizeof(float), 3, fout);
	    assert(nwritten == 3);
	}

	for (vector<Fragment>::iterator f=upper.m_Fragments.begin(); f != upper.m_Fragments.end(); f++) {
	    XYZ d = f->m_CenterOfMass - upperdcom;
	    nwritten = fwrite(&d, sizeof(float), 3, fout);
	    assert(nwritten == 3);
	}

	readerr = bmt.ReadFrame();
	lower.FindCenterOfMasses();
	upper.FindCenterOfMasses();

	n++;
    }
    fclose(fout);
    return 0;
}
