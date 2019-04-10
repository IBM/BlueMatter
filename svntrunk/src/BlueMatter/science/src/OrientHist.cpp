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
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/DXHistogram.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>

#define MAX_LIPID 99

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile segmentName " 
                    << "atom1 atom2 skip finalhist" << endl;
    cerr << "Input bmt containing rhod system" << endl;
    exit(-1);
}

// bmt2histo   sppdchol.bmt  out.txt fragspec.txt bond.txt
int main(int argc, char **argv)
{
    SimpleTrajectory st;

    //fXYZ box;
    //box.mX = 27.5*2;
    //box.mY = 38.5*2;
    //box.mZ = 51.3225*2;

    int nleaflets = 1;


    if (argc != 9)
	Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);

    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;


    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *segmentName = argv[4];
    int atom1_index = atoi(argv[5]);
    int atom2_index = atoi(argv[6]);
    int nskip = atoi(argv[7]);
    char *finalFileName = argv[8];

    // open the final histogram file for writing
    ofstream finalFile(finalFileName, ios::out);

    cout << "# ";
    finalFile << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        finalFile << argv[i] << " ";
        }
    cout << endl;

    float actualstart = st.m_StartTime+nskip*st.m_TimeStep;

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

    //fFragmentListTable flt("/u/agrossf/projects/rhodopsin/rhod_frags_dry.txt");
    //fMasses m("/u/agrossf/projects/rhodopsin/rhod_mass_dry.txt");
    fFragmentListTable flt(fragmentFileName);
    fAtomValues m(massFileName);

    fFragmentList sdpc = flt.GetFragmentList(segmentName);
    //int sdpc_nindex = 54;
    //int sdpc_pindex = 73;
    //int sdpe_nindex = 54;
    //int sdpe_pindex = 65;

    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);

    sdpc.AssignPositions(bmt.getPositions());
    sdpc.AssignMasses(m);


    flt.FindCenterOfMasses();
    sdpc.FindCenterOfMasses();

    //fFragmentList upper("upper"), lower("lower");
    //sdpc.SplitUpperLower(upper,lower);

    int readerr = 0;
    int nframe = 0;
    int i = 0;
    fXYZ n, p, v;
    int const nbins = 50;
    float const minval = -1.0;
    float const maxval = 1.0;
    float binwidth = (maxval - minval)/(float)nbins;
    float zhist[100],normhist[100]; 
    int bin;
    int count = 0;

    for (i=0;i<nbins;i++)
        {
        zhist[i] = 0.0;
        }

    int nfrag = 0;
    float binval;
    while (!readerr && nframe < 100000000) 
        {
        for (vector<fFragment>::iterator f =  sdpc.m_Fragments.begin();
                                         f != sdpc.m_Fragments.end(); f++)
            {
            n = f->m_Positions[f->m_Start + atom1_index];
            p = f->m_Positions[f->m_Start + atom2_index];
            v = n - p;
            v *= v.ReciprocalLength();

            bin = (int)((v[2] - minval)/binwidth);
            //cout << "z = " << v[2] << " bin = " << bin << endl;
            zhist[bin]+=1.0;
            count++;
            }

        if (nframe % 100 == 0)
            {
            cout << "# Read " << count << " points so far" << endl; 
            cout << "#Cos(N-P  z)\tProb" << endl;
        
            // normalize histogram and print
            for (i=0;i<nbins;i++)
                {
                normhist[i] = zhist[i]/(float)count;
                binval = ((float)i+0.5) * binwidth + minval;
                cout << binval << "\t" << normhist[i] << endl;
                }
            cout << endl;
            }

	    readerr = bmt.ReadFrame();
	    nframe++;
        }

    // write final histogram to finalFile rather than stdout
    finalFile << "# Read " << count << " total points" << endl; 
    finalFile << "#Cos(N-P  z)\tProb" << endl;

    // normalize histogram and print
    for (i=0;i<nbins;i++)
        {
        zhist[i] /= (float)count;
        binval = ((float)i+0.5) * binwidth + minval;
        finalFile << binval << "\t" << zhist[i] << endl;
        }


    return 0;
}
