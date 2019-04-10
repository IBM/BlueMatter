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
#include <BlueMatter/SurfaceDistance.hpp>


#define MAXBINS 1000


using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile " 
                    << "CentralFragment WaterFragment threshold zmin zmax nskip"
                    << endl;
    exit(-1);
}

int main(int argc, char **argv)
{




    if (argc != 10)
        Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);
    BMTReader bmt2(argv[1]);



    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *centralfragmentName = argv[4];
    char *waterfragmentName = argv[5];
    float threshold = atof(argv[6]);
    float zmin = atof(argv[7]);
    float zmax = atof(argv[8]);
    int nskip = atoi(argv[9]);

    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX;
    box.mY = myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY;
    box.mZ = myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ;

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
        }

    if (bmt.ReadFrame()) {
        cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
        exit(-1);
    }

    FragmentListTable dbl(fragmentFileName);
    AtomValues m(massFileName);

    FragmentList &water = dbl.GetFragmentList(waterfragmentName);
    FragmentList &central = dbl.GetFragmentList(centralfragmentName);

    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);
    dbl.FindCenterOfMasses();

    vector<XYZ> central_coor;

    // first pass through the file -- built the protein surface
    int readerr = 0;
    int nframe = 0;
    while (!readerr && nframe < 100000000)
        {
        dbl.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        dbl.Translate(com);
        dbl.MapIntoBoxByAtom(box);

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

    // build the protein surface
    SurfaceDistance sd(36,-40.0, 40.0, 40);
    sd.AddPoints(central_coor);

    
    // reset for second pass through the trajectory
    readerr = 0;
    nframe = 0;
    for (int i = 0; i< nskip; i++)
        {
        if (bmt2.ReadFrame())
            {
            cerr << "Error reading frame " << i << "during skip" << endl;
            exit(-1);
            }
        }

    if (bmt2.ReadFrame()) {
        cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
        exit(-1);
    }



    dbl.AssignPositions(bmt2.getPositions());
    cout << "#Frame\tNumWaters" << endl;
    while (!readerr && nframe < 100000000) 
        {
        int count = 0;
        dbl.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        dbl.Translate(com);
        dbl.MapIntoBox(box);
        for (vector<Fragment>::iterator f =  water.m_Fragments.begin();
                                        f != water.m_Fragments.end(); 
                                        f++)
            {
            XYZ pos = f->m_CenterOfMass;
            if ( (pos.mZ > zmin) && (pos.mZ < zmax) )
                {
                double dist = sd.Distance(f->m_CenterOfMass);
                if (dist < threshold)
                    {
                    count++;
                    }
                }
            }
        cout << nframe << "\t" << count << endl;
        readerr = bmt2.ReadFrame();
        nframe++;
        }
    return 0;
}
