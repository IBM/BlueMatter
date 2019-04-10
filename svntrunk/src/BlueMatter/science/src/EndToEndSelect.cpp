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
#include <BlueMatter/SurfaceDistance.hpp>


#define MAX_LIPID 1000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile segmentName "
                    << "atom1 atom2 select_dist_min select_dist_max skip" 
                    << endl;
    exit(-1);
}

int main(int argc, char **argv)
{



    int nleaflets = 1;
    int nlength = 1000000;


    if (argc != 10)
	Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);
    BMTReader bmt2(argv[1]);
    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *segmentName = argv[4];
    int atom1_index = atoi(argv[5]);
    int atom2_index = atoi(argv[6]); 
    float select_dist_min = atof(argv[7]);
    float select_dist_max = atof(argv[8]);
    int nskip = atoi(argv[9]);


    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX;
    box.mY = myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY;
    box.mZ = myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ;

    if (fabs(box.mX) < 1 || fabs(box.mY) < 1 || fabs(box.mZ) < 1)
        {
        cerr << "Box from bmt is missing or too small: " << myInitBox.mMinBox << " " << myInitBox.mMaxBox  << endl;
        exit(-1);
        }

    // Record the arguments in the output file
    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    for (int i = 0; i< nskip; i++)
        {
        if (bmt.ReadFrame())
            {
            cerr << "Error reading frame " << i << "during skip" << endl;
            exit(-1);
            }
        if (bmt2.ReadFrame())
            {
            cerr << "Error reading frame " << i << "during skip" << endl;
            exit(-1);
            }

        }


    if (bmt.ReadFrame()) {
	cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
	exit(-1);
    }
    if (bmt2.ReadFrame()) {
	cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
	exit(-1);
    }

    FragmentListTable flt(fragmentFileName);
    AtomValues m(massFileName);

    FragmentList &selected = flt.GetFragmentList(segmentName);
    FragmentList &central = flt.GetFragmentList("RHOD");

    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);
    flt.FindCenterOfMasses();


    int readerr = 0;
    int nframe = 0;

    vector<XYZ> central_coor;

    while (!readerr && nframe < nlength)
        {
        flt.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        flt.Translate(com);
        flt.MapIntoBoxByAtom(box);

        for (int i=0; i<central.m_Fragments.size(); i++)
            {
            Fragment &f = central.m_Fragments[i];
            for (int k=0; k < f.m_Count; k++)
                {
                int index = f.m_Start + k;
                XYZ p = f.m_Positions[index];
                central_coor.push_back(p);
                }
            }

        readerr = bmt.ReadFrame();
        nframe++;
        }

    // build the polygon describing the consensus surface of the central frag
    SurfaceDistance sd(36,-40.0, 40.0, 40);
    sd.AddPoints(central_coor);


    flt.AssignPositions(bmt2.getPositions());
    readerr = 0;
    nframe = 0;
    XYZ n, p, v;
    Column xvalues; 
    Column yvalues; 
    Column zvalues; 
    int numevents=0;

    int nfrag = 0;
    cout << "# Frame  Distance  XY_Component Z_Component" << endl;
    while (!readerr && nframe < 100000000) 
        {
        //cout << "got here" << endl;
        flt.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        flt.Translate(com);
        flt.MapIntoBox(box);
        for (vector<Fragment>::iterator f =  selected.m_Fragments.begin();
                                        f != selected.m_Fragments.end(); f++)
            {
            float dist = sd.Distance(f->m_CenterOfMass);

            //cout << dist << "  " << nfrag << "  " << endl;
            if ( (dist < select_dist_max) &&
                 (dist >= select_dist_min) )
                    {
                    n = f->m_Positions[f->m_Start + atom1_index];
                    p = f->m_Positions[f->m_Start + atom2_index];
                    v = n - p;
                    dist = v.Length();

                    // negate the z component for lower leaflet
                    if (n[2] < 0)
                        {
                        v[2] = -v[2];
                        }
                    float xy = sqrt(v[0]*v[0] + v[1]*v[1]);
                    cout << nframe << "  ";
                    cout << dist   << "  ";
                    cout << xy     << "  ";
                    cout << v[2]   << endl;
                    }
            }
	    readerr = bmt2.ReadFrame();

	    nframe++;
        }

    return 0;
}
