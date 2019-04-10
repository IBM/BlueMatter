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
    cerr << argv[0] << " FName.bmt frag.txt masses.txt alignFrag window_width nskip" << endl;
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
    int win_width = atoi(argv[5]);
    int nskip = atoi(argv[6]);
    int nlength = 1000000000;

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

    // build the vector of coordinate frames
    vector<XYZ *> pos_list;
    for (int i=0;i<win_width; i++)
        {
        XYZ *v = new XYZ[frmsd.m_Count];
        pos_list.push_back(v);
        }

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

    // seed the coordinate lists
    for (int i=0; i<win_width; i++)
        {
        if (bmt.ReadFrame())
            {
            cerr << "Error reading to fill frame list at frame " << i << endl;
            }
        else
            {
            //pos_list[i] = frmsd.m_Positions[frmsd.m_Start];
            flt.FindCenterOfMasses();
            flt.Translate(-frmsd.m_CenterOfMass);
            for (int j = 0; j< frmsd.m_Count; j++)
                {
                pos_list[i][j] = frmsd.m_Positions[frmsd.m_Start+j];
                }
            }
        }

    float **rms_vals = new float*[win_width];
    for (int i=0; i<win_width; i++)
        {
        float *f = new float[win_width];
        rms_vals[i] = f;
        }

    // seed the rms lists
    for (int i=0; i<win_width; i++)
        {
        al.UpdateRefPositions(pos_list[i]);
        for (int j=0; j<win_width; j++)
            {
            for (int k=0; k<frmsd.m_Count;k++)
                {
                frmsd.m_Positions[frmsd.m_Start+k] = pos_list[j][k];
                }
            al.DefineTransform(&frmsd.m_Positions[frmsd.m_Start]);
            al.ApplyTransform(frmsd);
            double rms = al.FindRMSD(frmsd);
            rms_vals[i][j] = rms;
            }
        }

    flt.AssignPositions(bmt.getPositions());

    int readerr = 0;
    int n = 0;
    

    cout << "# Time   RMS" << endl;
    while (!readerr && n < nlength)
        {
        flt.FindCenterOfMasses();
        flt.Translate(-frmsd.m_CenterOfMass);
        int index = n % win_width;
        for (int i=0; i<win_width; i++)
            {
            al.UpdateRefPositions(pos_list[i]);
            al.DefineTransform(&frmsd.m_Positions[frmsd.m_Start]);
            al.ApplyTransform(frmsd);
            double rms = al.FindRMSD(frmsd);
            rms_vals[index][i] = rms;
            //rms_ave += (float)rms;
            }
        
        // compute all-to-all average rms
        int num_vals = 0;
        float rms_ave = 0.0;
        for (int i=0; i<win_width; i++)
            {
            int j = (i + index) % win_width;
            for (int k=i; k<win_width; k++)
                {
                rms_ave += rms_vals[j][k];
                num_vals++;
                }
            }
        rms_ave /= (float)num_vals;

        cout << n << "  " << rms_ave << endl;

        // update the window pos_list by overwriting the oldest entry
        // this is the n % window_width one
        for (int i=0;i<frmsd.m_Count;i++)
            {
            pos_list[index][i] = frmsd.m_Positions[frmsd.m_Start+i];
            }


        readerr = bmt.ReadFrame();
        n++;
        }


    return 0;
}

