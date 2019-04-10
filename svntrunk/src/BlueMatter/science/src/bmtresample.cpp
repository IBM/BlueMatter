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
    cerr << argv[0] << " FName.bmt NewFName.bmt resample skip" 
                    << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    if (argc != 5) Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);
    BMTWriter bmtw(argv[2], bmt);

    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;

    int resample = atoi(argv[3]);
    int nskip = atoi(argv[4]);

    // Record the arguments in the output file
    cerr << "# ";
    for (int i = 0; i<argc; i++)
        {
        cerr << argv[i] << " ";
        }
    cerr << endl;
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

    int readerr = 0;
    int nframe = 0;

    while (!readerr && nframe < 100000000) 
        {
        if (nframe % resample == 0)
            {
            bmtw.WriteFrame();
            }
        readerr = bmt.ReadFrame();
        nframe++;
        }
    bmtw.Close();
}
