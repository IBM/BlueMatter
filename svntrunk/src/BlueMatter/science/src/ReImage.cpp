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

#define MAX_LIPID 1000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt NewFName.bmt fragmentFile massFile "
                    << "skip" 
                    << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    if (argc != 6) Usage(argv);

    char *bmtname = argv[1];
    char *outbmtname = argv[2];

    char *fragmentFileName = argv[3];
    char *massFileName = argv[4];
    int nskip = atoi(argv[5]);

    // Record the arguments in the output file
    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    BMTReader bmt(bmtname);

    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = fabs(myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX);
    box.mY = fabs(myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY);
    box.mZ = fabs(myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ);

    for (int i = 0; i< nskip; i++)
        {
        if (bmt.ReadFrame())
            {
            cerr << "Error reading frame " << i << "during skip" << endl;
            exit(-1);
            }
        }


    if (bmt.ReadFrame()) {
        cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
        exit(-1);
    }
    BMTWriter bmtw(outbmtname, bmt);

    FragmentListTable dbl(fragmentFileName);
    AtomValues m(massFileName);

    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);

    FragmentList &rhod = dbl.GetFragmentList("RHOD");
    FragmentList &sdpc = dbl.GetFragmentList("SDPC");
    FragmentList &sdpe = dbl.GetFragmentList("SDPE");


    int readerr = 0;
    int nframe = 0;

    while (!readerr && nframe < 100000000) 
        {
        rhod.FindCenterOfMasses();

        dbl.Translate(-rhod.m_CenterOfMass);
        dbl.FindCenterOfMasses(); 
        dbl.MapIntoBox(box);

        sdpc.FindCenterOfMasses();
        sdpe.FindCenterOfMasses();

        XYZ lip_com = (sdpc.m_Mass*sdpc.m_CenterOfMass + 
                        sdpe.m_Mass*sdpe.m_CenterOfMass) / 
                         (sdpc.m_Mass + sdpe.m_Mass);

        dbl.Translate(-lip_com);
        dbl.FindCenterOfMasses();
        dbl.MapIntoBox(box);

        bmtw.WriteFrame();
        readerr = bmt.ReadFrame();
        nframe++;
        }
    bmtw.Close();
}
