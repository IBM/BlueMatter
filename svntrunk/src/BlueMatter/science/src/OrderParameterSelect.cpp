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
#include <BlueMatter/DXHistogram.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>
#include <BlueMatter/Bonds.hpp>
#include <BlueMatter/SurfaceDistance.hpp>


#define MAX_LIPID 10000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile centralFrag"
                    << " segmentName BondFile select_dist_min select_dist_max"
                    << " skip" 
         << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    XYZ box;

    int nleaflets = 1;


    if (argc != 10)
        Usage(argv);

    char *bmtname = argv[1];

    BMTReader bmt(argv[1]);
    BMTReader bmt2(argv[1]);
    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *centralFrag = argv[4];
    char *segmentName = argv[5];
    char *bondFileName = argv[6];
    float select_dist_min = atof(argv[7]);
    float select_dist_max = atof(argv[8]);
    int nskip = atoi(argv[9]);

    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX;
    box.mY = myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY;
    box.mZ = myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ;

    //box.mX = bmt.m_InitialBox.mMaxBox.mX - bmt.m_InitialBox.mMinBox.mX;
    //box.mY = bmt.m_InitialBox.mMaxBox.mY - bmt.m_InitialBox.mMinBox.mY;
    //box.mZ = bmt.m_InitialBox.mMaxBox.mZ - bmt.m_InitialBox.mMinBox.mZ;

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

    FragmentListTable dbl(fragmentFileName);
    AtomValues m(massFileName);

    FragmentList &selected = dbl.GetFragmentList(segmentName);
    FragmentList &central= dbl.GetFragmentList(centralFrag);

    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);

    vector<XYZ> central_coor;

    // first loop through the trajectory -- build the protein surface
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
    
    // read the bondFile containing the carbons and hydrogens we want to
    // use for the order parameter calculation
    OrderBondList obl(bondFileName, &selected);


    // reset for second pass through the trajectory -- compute the order
    // parameter as a function of distance
    readerr = 0;
    nframe = 0;

    for (int i = 0; i< nskip; i++)
        {
        if (bmt2.ReadFrame())
            {
            cerr << "Error reading frame " << i << "during second skip" << endl;
            exit(-1);
            }
        }

    dbl.AssignPositions(bmt2.getPositions());
    XYZ c, h, v;
    int c_index, h_index;
    int nlipcount = 0;


    while (!readerr && nframe < 100000000) 
        {
        dbl.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        dbl.Translate(com);
        dbl.MapIntoBox(box);
        for (vector<Fragment>::iterator 
            f =  obl.m_Fragments->m_Fragments.begin();
            f != obl.m_Fragments->m_Fragments.end(); f++)
            {
            float dist = sd.Distance(f->m_CenterOfMass);

            if ( (dist < select_dist_max) &&
                 (dist >= select_dist_min) )
                {
                nlipcount++;
                for (vector<OrderBond>::iterator bond  = obl.m_bonds.begin();
                                           bond != obl.m_bonds.end(); bond++)
                    {
                    while (bond->SetIndices(&c_index, &h_index))
                        {
                        c = f->m_Positions[f->m_Start + c_index];
                        h = f->m_Positions[f->m_Start + h_index];
                        v = c - h;
                        v *= v.ReciprocalLength();
                        bond->AddValue(v[2]);
                        }
                    }
                }
            }
        readerr = bmt2.ReadFrame(); 
        selected.FindCenterOfMasses();
        nframe++;
        }

    cout << "# Read " << nframe << " frames" << endl; 
    cout << "# Counted " << nlipcount << " lipids" << endl;
    cout << "#Carbon\tOrderParameter\tFluctuations" << endl;

    for (vector<OrderBond>::iterator bond  = obl.m_bonds.begin();
                                     bond != obl.m_bonds.end(); bond++)
        {
        double op = bond->GetOP();
        double op_fluc = bond->GetOPFluc();
        cout << bond->m_Name << "\t" << op << "\t" << op_fluc << endl;
        }

    return 0;
}
