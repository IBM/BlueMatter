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

#define MAX_LIPID 1000

using namespace std;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile segmentName "
                    << "segment2name outDir skip" 
                    << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;


    if (argc != 8)
        Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);

    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;

    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *segmentName = argv[4];
    char *segment2Name = argv[5];
    char *outDir = argv[6];
    int nskip = atoi(argv[7]);

    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box = myInitBox.GetDimensionVector();
    // Record the arguments in the output file
    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;
    double actualstart = st.m_StartTime+nskip*st.m_TimeStep;

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
    AtomValues m(massFileName);

    FragmentList &orient = flt.GetFragmentList(segmentName);
    FragmentList &selected = flt.GetFragmentList(segment2Name);

    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);


    flt.FindCenterOfMasses();


    int readerr = 0;
    int nframe = 0;
    Column xvalues[MAX_LIPID]; 
    Column yvalues[MAX_LIPID]; 
    Column zvalues[MAX_LIPID]; 

    int nfrag = 0;
    while (!readerr && nframe < 100000000) 
        {
        nfrag=0;
        flt.FindCenterOfMasses();
        for (vector<Fragment>::iterator f =  selected.m_Fragments.begin();
                                        f != selected.m_Fragments.end(); f++)
            {
            XYZ vec;
            vec = Imaging::NearestImageDisplacement(orient.m_CenterOfMass,
                                                     f->m_CenterOfMass,
                                                     box);
            xvalues[nfrag].addValue((double)vec[0]);
            yvalues[nfrag].addValue((double)vec[1]);
            zvalues[nfrag].addValue((double)vec[2]);
            nfrag++;
            }
	    readerr = bmt.ReadFrame();

	    nframe++;
        }

    cerr << "# Writing output to files in " << outDir << endl;

    for (int i = 0; i<nfrag; i++)
        {
        // open the output file
        char filename[1024];
        sprintf(filename,"%s/%d.dat",outDir,i);
        ofstream outFile(filename);

        // get the statistics and put them at the top of the file
        xvalues[i].doStats();
        yvalues[i].doStats();
        zvalues[i].doStats();
        
        outFile << "# x " << xvalues[i].m_Min << " "
                          << xvalues[i].m_Max << " "
                          << xvalues[i].m_Mean << " "
                          << xvalues[i].m_Sigma << endl;

        outFile << "# y " << yvalues[i].m_Min << " "
                          << yvalues[i].m_Max << " "
                          << yvalues[i].m_Mean << " "
                          << yvalues[i].m_Sigma << endl;

        outFile << "# z " << zvalues[i].m_Min << " "
                          << zvalues[i].m_Max << " "
                          << zvalues[i].m_Mean << " "
                          << zvalues[i].m_Sigma << endl;

        outFile << "# Time" << "\t" << "X" << "\t" << "Y" << "\t" 
                                    << "Z" << endl;
        for (int j = 0; j < xvalues[i].m_NPts; j++)
            {
            double time = actualstart + st.m_TimeStep*j;
            outFile << time << "\t" << xvalues[i].m_Data[j]
                         << "\t" << yvalues[i].m_Data[j]
                         << "\t" << zvalues[i].m_Data[j]
                         << endl;
            }
        outFile.close();
        cerr << "#Finished writing "<< filename << endl;
        }


    return 0;
}
