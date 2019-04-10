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
//#include <sys/stat.h>
//#include <errno.h>
//#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>


#define MAX_LIPID 10000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile frag1 frag2"
                    << " dist resample skip " 
         << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    XYZ box;

    int nleaflets = 1;


    if (argc != 11)
        Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);
    BMTReader bmt2(argv[1]);
    char *fragmentFileName = argv[2];
    char *frag1name= argv[3];
    char *frag2name = argv[4];
    float box_x = atof(argv[5]);
    float box_y = atof(argv[6]);
    float box_z = atof(argv[7]);
    float min_dist = atof(argv[8]);
    int nresample = atoi(argv[9]);
    int nskip = atoi(argv[10]);

    box.mX = box_x;
    box.mY = box_y;
    box.mZ = box_z;

    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;


    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
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

    FragmentListTable flt(fragmentFileName);
    FragmentList &frag1 = flt.GetFragmentList(frag1name);
    FragmentList &frag2 = flt.GetFragmentList(frag2name);

    flt.AssignPositions(bmt.getPositions());


    // first loop through the trajectory -- build the protein surface
    int readerr = 0;
    int nframe = 0;
    XYZ atom1, atom2;
    while (!readerr && nframe < 100000000)
        {
        int count = 0;
        int skip = nframe % nresample;
        //cout << nframe << "  " << nresample << "  " << skip
        //     << endl;
        if (!skip)
            {
            for (vector<Fragment>::iterator f  = frag1.m_Fragments.begin();
                                             f != frag1.m_Fragments.end();
                                             f++)
                {
                for (int i = 0; i< f->m_Count; i++)
                    {
                    atom1 = f->m_Positions[f->m_Start + i];
                    for (vector<Fragment>::iterator f2 = frag2.m_Fragments.begin();
                                                     f2!= frag2.m_Fragments.end();
                                                     f2++)
                        {
                        for (int j = 0; j < f2->m_Count; j++)
                            {
                            atom2 = f2->m_Positions[f2->m_Start + j];
                            double dist = Imaging::NearestImageDistance(atom1,
                                                                        atom2,
                                                                        box);
                            if (dist <= min_dist)
                                {
                                //int first = f->m_Start + i + 1; // pdb index
                                //int second = f2->m_Start + j + 1;
                                count++;
                                }
                            }
                        }
                    }

                }
            cout << nframe << "    " << count << endl;
            }

        readerr = bmt.ReadFrame(); 
        nframe++;
        }

    return 0;
}
