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
#include <BlueMatter/SurfaceDistance.hpp>


using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt frag.txt mass.txt centralFrag "
                    << "otherFrag cutoff nskip" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{

    XYZ box;


    if (argc < 8)
        Usage(argv);

    cout << "# ";
    for (int i=0;i<argc;i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    char *bmtfilename = argv[1];
    char *fragfilename = argv[2];
    char *massfilename = argv[3];
    char *centralfrag = argv[4];
    char *otherfrag = argv[5];
    double cutoff = atof(argv[6]);
    int nskip = atoi(argv[7]);

    BMTReader bmt(bmtfilename);
    int nlength = bmt.getNFrames() - nskip;


    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX;
    box.mY = myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY;
    box.mZ = myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ;

    //box.mX = bmt.m_InitialBox.mMaxBox.mX - bmt.m_InitialBox.mMinBox.mX;
    //box.mY = bmt.m_InitialBox.mMaxBox.mY - bmt.m_InitialBox.mMinBox.mY;
    //box.mZ = bmt.m_InitialBox.mMaxBox.mZ - bmt.m_InitialBox.mMinBox.mZ;

    if (fabs(box.mX) < 1 || fabs(box.mY) < 1 || fabs(box.mZ) < 1) 
        {
        cerr << "Box from bmt is missing or too small: " << myInitBox.mMinBox << " " << myInitBox.mMaxBox  << endl;
        exit(-1);
        }

    

    if (bmt.ReadFrame(nskip)) 
        {
        cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
        exit(-1);
        }

    FragmentListTable flt(fragfilename);
    FragmentList &central = flt.GetFragmentList(centralfrag);
    FragmentList &other = flt.GetFragmentList(otherfrag);
    AtomValues m(massfilename);


    flt.AssignMasses(m);
    flt.AssignPositions(bmt.getPositions());

    vector<XYZ> central_coor;
    vector<XYZ> other_coor;

    int readerr = 0;
    int n = 0;

    while (!readerr && n < nlength) 
        {
        
        flt.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        flt.Translate(com);
        flt.FindCenterOfMasses();
        XYZ other_com = other.m_CenterOfMass;
        //cout << "n = " << n << endl;
        //cout << "other_com before = " << other_com << endl;
        other_com = Imaging::NearestImagePosition(central.m_CenterOfMass,
                                                  other_com, box);
        //cout << "other_com after = " << other_com << endl;
        //cout << "central com after = " << central.m_CenterOfMass << endl;
        other_coor.push_back(other_com);
        
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
        n++;
        }

    // build the polygon describing the consensus surface of the central frag
    SurfaceDistance sd(36,-40.0, 40.0, 40);
    sd.AddPoints(central_coor);
    //sd.Print();

    if (bmt.ReadFrame(nskip))
        {
        cerr << "Error rewinding to step " << nskip << " after first pass"
             << " through bmt file" << endl;
        exit(-1);
        }

    readerr = 0;
    n = 0;

    int inside;
    cout << "#Frame\tDist\tIn/Out" << endl;
    for (int i=0; i<nlength; i++)
        {
        double dist = sd.Distance(other_coor[i]);
        if (dist < cutoff)
            {
            inside = 1;            
            }
        else
            {
            inside = 0;
            }
        cout << i << "\t" << dist << "\t" << inside << endl;

        readerr = bmt.ReadFrame();
        }

    
    return 0;
}
