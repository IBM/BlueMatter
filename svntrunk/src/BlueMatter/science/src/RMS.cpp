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
#include <BlueMatter/SimpleTrajectory.hpp>
#include <BlueMatter/Align.hpp> 


using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt frag.txt masses.txt alignFrag alignFrame nskip" << endl;
    cerr << "Input bmt and alignment component" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{

    if (argc != 7)
        {
        Usage(argv);
        }

    cout << "# ";
    for (int i=0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    char *bmtfilename = argv[1];
    char *fragfilename = argv[2];
    char *massfilename = argv[3];
    char *alignfragname = argv[4];
    int alignframe = atoi(argv[5]);
    int nskip = atoi(argv[6]);
    int nlength = 1000000000;

    BMTReader bmt1(bmtfilename);
    BMTReader bmt(bmtfilename);

    SimpleTrajectory st;
    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;
    float actualstart = st.m_StartTime+nskip*st.m_TimeStep;

    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box = myInitBox.GetDimensionVector();

    FragmentListTable flt(fragfilename);
    AtomValues m(massfilename);

    // computing a time series aligning on a fragmentlist only makes sense 
    // if there's only one fragment in the list
    FragmentList &flrmsd = flt.GetFragmentList(alignfragname);
    Fragment &frmsd = flrmsd.m_Fragments[0];

    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);
    tAlign<double> al(frmsd, bmt.getNAtoms(), box);

    for (int i=0; i<nskip; i++)
        {
        if (bmt.ReadFrame()) 
            {
            cerr << "Error reading " << i << " frame during skip" << endl;
            exit(-1);
            } 
        } 

    for (int i=0; i<alignframe; i++)
        {
        if (bmt1.ReadFrame())
            {
            cerr << "Error reading to find align frame at frame " << i << endl;
            }
        }
    flt.AssignPositions(bmt1.getPositions());
    al.UpdateRefPositions(&frmsd.m_Positions[frmsd.m_Start]);
    flt.AssignPositions(bmt.getPositions());

    int readerr = 0;
    int n = 0;

    cout << "# Time   RMS" << endl;
    while (!readerr && n < nlength)
        {
        flt.FindCenterOfMasses();
        flt.Translate(-frmsd.m_CenterOfMass);
        al.DefineTransform(&frmsd.m_Positions[frmsd.m_Start]);
        al.ApplyTransform(frmsd);
        double rms = al.FindRMSD(frmsd);
        cout << n << "  " << rms << endl;

        readerr = bmt.ReadFrame();
        n++;
        }


    return 0;
}

