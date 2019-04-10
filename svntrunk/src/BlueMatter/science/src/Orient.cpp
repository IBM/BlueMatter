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

#define MAX_LIPID 1000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile segmentName "
                    << "atom1 atom2" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    fXYZ box;
    box.mX = 27.5*2;
    box.mY = 38.5*2;
    box.mZ = 51.3225*2;

    int nleaflets = 1;

    int nskip = 0;

    if (argc != 7)
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

    // Record the arguments in the output file
    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;
    float actualstart = st.m_StartTime+nskip*st.m_TimeStep;



    if (bmt.ReadFrame()) {
	cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
	exit(-1);
    }

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
    fXYZ n, p, v;
    Column zvalues[MAX_LIPID]; 
    Column xvalues[MAX_LIPID]; 

    int nfrag = 0;
    while (!readerr && nframe < 100000000) 
        {
        nfrag=0;
        for (vector<fFragment>::iterator f =  sdpc.m_Fragments.begin();
                                         f != sdpc.m_Fragments.end(); f++)
            {
            n = f->m_Positions[f->m_Start + atom1_index];
            p = f->m_Positions[f->m_Start + atom2_index];
            v = n - p;
            v *= v.ReciprocalLength();
            xvalues[nfrag].addValue((double)v[0]);
            zvalues[nfrag].addValue((double)v[2]);
            nfrag++;
            }
	    readerr = bmt.ReadFrame();

	    nframe++;
        if (nframe % 100 == 0)
            {
            // Compute running average
            }
        }

    cout << "# Read " << nframe << " frames" << endl; 

    cout << "# lipid\t<cos(N-P z)>\tFluc\t<cos(N-P x)>\tFluc" << endl;

    double total_xmean = 0.0;
    double total_xsigma = 0.0;
    double total_zmean = 0.0;
    double total_zsigma = 0.0;
    for (int i=0; i<nfrag; i++)
        {
        xvalues[i].doStats();
        zvalues[i].doStats();
        cout << i << "\t" 
             << zvalues[i].m_Mean << "\t" << zvalues[i].m_Sigma 
             << "\t" << xvalues[i].m_Mean << "\t" << xvalues[i].m_Sigma 
             << endl;
        total_xmean += xvalues[i].m_Mean;
        total_xsigma += xvalues[i].m_Sigma;
        // take absolute value so upper and lower leaflets don't 
        // cancel each other!
        total_zmean += fabs(zvalues[i].m_Mean);
        total_zsigma += zvalues[i].m_Sigma;
        }

    total_xmean /= (double)nfrag;
    total_xsigma /= (double)nfrag;
    total_zmean /= (double)nfrag;
    total_zsigma /= (double)nfrag;
    cout << "# Final average " 
         << total_zmean << "\t" << total_zsigma << "\t"
         << total_xmean << "\t" << total_xsigma 
         << endl;
    return 0;
}
