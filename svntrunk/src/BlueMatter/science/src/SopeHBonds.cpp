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
// #include "/fs/dx_basic/BinaryIO/Histogram.hpp"
#include <BlueMatter/HBond.hpp>

using namespace std ;

// finds all possible hbonds between lipids, within each leaflet
// outputs:
// step uplow donID accID xlatX xlatY xlatZ nbonds energysum 

int main(int argc, char **argv)
{
    fXYZ box;
    box.mX = 22.8807f*2;
    box.mY = 22.8807f*2;
    box.mZ = 31.9384f*2;

    if (argc < 5) {
	cout << "SopeHBonds donaccname bmtname outstem t0 dt" << endl;
	exit(-1);
    }

    char *donaccname = argv[1];
    char *bmtname = argv[2];
    char *outstem = argv[3];
    float t0 = atof(argv[4]);
    float dt = atof(argv[5]);

    BMTReader bmt(bmtname);
    if (bmt.ReadFrame()) {
	cerr << "Error reading first bmt frame" << endl;
	exit(-1);
    }

    // load in full set of donors and acceptors
    fDonorAcceptors da(donaccname);
    // da.SetIDs();

    da.AssignPositions(bmt.getPositions());

    // now split them into upper and lower halves based on z
    fDonorAcceptors upper;
    fDonorAcceptors lower;
    da.SplitUpperLower(upper, lower);

    // build n^2 tables for each
    upper.InitImages(box);
    lower.InitImages(box);

    int readerr = 0;
    int n = 0;

    char don[512];
    char bond[512];

    sprintf(don, "%s_don.txt", outstem);
    sprintf(bond, "%s_bond.txt", outstem);

    FILE *fdon = fopen(don, "w");
    FILE *fbond = fopen(bond, "w");

    while (!readerr && n < 100000000) {
	float t = t0 + dt*n;

	lower.DumpA(box, fdon);
	upper.DumpA(box, fdon);
	lower.DumpHBonds(t, box, fbond, 0);
	upper.DumpHBonds(t, box, fbond, 1);

	readerr = bmt.ReadFrame();
	n++;
    }
    fclose(fdon);
    fclose(fbond);

    return 0;
}