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
 
#include <assert.h>
#include <fcntl.h>
// #include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std ;
#include <BlueMatter/XYZ.hpp>
#define BUFFSIZE 2048

#define BEGINTRAJ "TRAJECTORYFORMAT"
#define ATOM "ATOM"

//                               -fff.fff-fff.fff-fff.fff
// 123456789012345678901234567890123456789012345678901234567890
// ATOM      1 HH31 ACE     1      11.996   2.667  -7.715
using namespace std;

#define MAXATOMS 20000
#define PDBPREFIX 30
#define EMPTYLINE "                                                                   "

struct XYZShort {
    unsigned short mX;
    unsigned short mY;
    unsigned short mZ;
};

ofstream mStream;
FILE *mFile;
char mBMTFileName[256];
char mPDBFileName[256];
int mNNonWaterAtoms;
int mNWaters;
int mNPDBAtoms;
XYZShort mpShortPos[MAXATOMS];
XYZ mpPos[MAXATOMS];


void setParams(char *BMTFileName, char *PDBFileName, int NNonWaterAtoms, int NWaters)
{
    strcpy(mBMTFileName, BMTFileName);
    strcpy(mPDBFileName, PDBFileName);
    mNNonWaterAtoms = NNonWaterAtoms;
    mNWaters = NWaters;
    // cout << mNNonWaterAtoms << " " << mNWaters << endl;
}

void init()
{
    char buff[1024];

    mStream.open(mBMTFileName);
    mStream << "BMT" << endl;
    mStream << "VERSION 1.00" << endl;
    mStream << "STRUCTURE " << mPDBFileName << endl;
    mStream << "BONDS NULL" << endl;
    mStream << "NUMNONWATERATOMS " << mNNonWaterAtoms << endl;
    mStream << "NUMWATERS " << mNWaters << endl;
    mStream << "TRAJECTORYFORMAT 16-bit RESCALE RECENTER" << endl;
    mStream.close();
    // close text mode and switch to binary, appending at end
    mFile = fopen(mBMTFileName, "ab");
}


void findBox(XYZ *pos, int NSites, XYZ &origin, XYZ &span, double &MaxSpan)
{
    XYZ LL, UR;
    XYZ *ps = pos;

    LL.mX = LL.mY = LL.mZ = 1.0E20;
    UR.mX = UR.mY = UR.mZ = -1.0E20;

    for (int i=0; i<NSites; i++) {
        XYZ s = *ps;
        if (s.mX < LL.mX) LL.mX = s.mX;
        if (s.mX > UR.mX) UR.mX = s.mX;
        if (s.mY < LL.mY) LL.mY = s.mY;
        if (s.mY > UR.mY) UR.mY = s.mY;
        if (s.mZ < LL.mZ) LL.mZ = s.mZ;
        if (s.mZ > UR.mZ) UR.mZ = s.mZ;
        ps++;
    }
    origin = LL;
    span = UR - LL;
    MaxSpan = span.mX;
    if (span.mY > MaxSpan) MaxSpan = span.mY;
    if (span.mZ > MaxSpan) MaxSpan = span.mZ;
}

void simplifySites(XYZ *pos, XYZShort *psp, int NSites, XYZ &origin, double factor)
{
    XYZ *ps = pos;

    for (int i=0; i<NSites; i++) {
        XYZ p = *ps;
        p -= origin;
        psp->mX = p.mX*factor;
        psp->mY = p.mY*factor;
        psp->mZ = p.mZ*factor;
        ps++;
        psp++;
    }
}

void sites()
{
    XYZ origin, span;
    double maxspan;
    findBox(mpPos, mNPDBAtoms, origin, span, maxspan);

    double factor = 65535/maxspan;
    simplifySites(mpPos, mpShortPos, mNPDBAtoms, origin, factor);

    float recipfactor = maxspan/65535;
    float x,y,z;
    x = origin.mX;
    y = origin.mY;
    z = origin.mZ;
    fwrite(&x, sizeof(float), 1, mFile);
    fwrite(&y, sizeof(float), 1, mFile);
    fwrite(&z, sizeof(float), 1, mFile);
    fwrite(&recipfactor, sizeof(float), 1, mFile);

    XYZShort *psp = mpShortPos;
    for (int i=0; i<mNPDBAtoms; i++) {
        fwrite(psp, sizeof(unsigned short), 3, mFile);
        psp++;
    }
}

void final()
{
    fclose(mFile);
}



int
main(int argc, char **argv, char **envp)
{
    char buff[BUFFSIZE];

    if (argc < 2) {
        cout << "pdb2bmt PDBFileName BMTFileName" << endl;
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    assert(f);
    fgets(buff, BUFFSIZE, f);

    // seek to first atom
    // grab atom coord
    // seek to line containing END
    // but first skip over first frame
    int firstframe = 1;

    while (!feof(f) && !ferror(f)) {
        int n = 0;
        // seek to first atom
        // cout << "seeking atom" << endl;
        while (!feof(f) && !ferror(f)) {
            if (!strncmp(buff, ATOM, strlen(ATOM)))
                break;
            fgets(buff, BUFFSIZE, f);
        }
        // now loop over atoms until comment
        // cout << "loop to comment" << endl;
        while (!feof(f) && !ferror(f)) {
            if (!strncmp(buff, ATOM, strlen(ATOM))) {
                if (3 != sscanf(buff, "%*30c%lf%lf%lf", &mpPos[n].mX, &mpPos[n].mY, &mpPos[n].mZ)) {
                    cerr << "Error getting atom from line " << buff << endl;
                    assert(1);
                }
                n++;
            }
            fgets(buff, BUFFSIZE, f);
            if (!strncmp(buff, "COMM", 4))
                break;
        }

        // first frame may be native structure without cations so skip it
        if (n > 0 && firstframe) {
            firstframe = 0;
            // cout << "firstframe, n " << firstframe << " " << n << endl;
            continue;
        }

        if (!mNPDBAtoms) {
            mNPDBAtoms = n;
            setParams(argv[2], argv[1], mNPDBAtoms, 0);
            init();
        }
        if (mNPDBAtoms != n) {
            cerr << "Number of atoms in frame is not the expected value, skipping: " << n << " " << mNPDBAtoms << endl;
            continue;
            // fclose(f);
            // assert(1);
        }
        // cout << "sites" << endl;
        sites();
    }
    fclose(f);
    final();
}
