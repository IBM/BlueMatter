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



using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile fragmentName " 
                    << "finalhist minval maxval nbins skip " << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;




    if (argc != 10)
        Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);

    // AK: Modified access
    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;


    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *segmentName = argv[4];
    char *finalFileName = argv[5];
    float minval = atof(argv[6]);
    float maxval = atof(argv[7]);
    int nbins = atoi(argv[8]);
    int nskip = atoi(argv[9]);

    float binwidth = (maxval - minval)/(float)nbins;

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

    FragmentListTable dbl(fragmentFileName);
    AtomValues m(massFileName);

    FragmentList &selected = dbl.GetFragmentList(segmentName);
    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);
    dbl.FindCenterOfMasses();

    int readerr = 0;
    int nframe = 0;
    int i = 0;
    float *zhist = new float[nbins];
    float *normhist = new float[nbins];
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
        for (vector<Fragment>::iterator f =  selected.m_Fragments.begin();
                                         f != selected.m_Fragments.end(); f++)
            {
            f->FindPrincipalAxes();
            XYZ axis = f->m_PrincipalAxes[0];
            
            // probably unnecessary
            axis = axis / axis.Length();
            float order = fabs(1.5*axis[2]*axis[2] - 0.5);

            bin = (int)((order - minval)/binwidth);
            zhist[bin]+=1.0;
            count++;
            }

        if (nframe % 100 == 0)
            {
            cout << "# Read " << count << " points so far" << endl; 
            cout << "#OrderParameter\tProb" << endl;
        
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
    finalFile << "#OrderParameter\tProb" << endl;

    // normalize histogram and print
    for (i=0;i<nbins;i++)
        {
        zhist[i] /= (float)count;
        binval = ((float)i+0.5) * binwidth + minval;
        finalFile << binval << "\t" << zhist[i] << endl;
        }


    return 0;
}
