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
#include <BlueMatter/JRand.hpp>


using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt frag.txt mass.txt centralFrag "
                    << " min_z max_z dist_min dist_max nbins nsamples "
                    << " skip" << endl;
    exit(-1);
}

double get_coor(const double dim, jrand &r)
{
double val = (r.fget()-0.5)*dim;
return val;
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    XYZ box;

    if (argc < 12)
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
    double min_z = atof(argv[5]);
    double max_z = atof(argv[6]);
    double dist_min = atof(argv[7]);
    double dist_max = atof(argv[8]);
    int nbins = atoi(argv[9]);
    int nsamples = atoi(argv[10]);
    int nskip = atoi(argv[11]);

    

    BMTReader bmt(bmtfilename);
    int nlength = 1000000;

    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX;
    box.mY = myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY;
    box.mZ = myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ;

    if (fabs(box.mX) < 1 || fabs(box.mY) < 1 || fabs(box.mZ) < 1) 
        {
        cerr << "Box from bmt is missing or too small: " << myInitBox.mMinBox << " " << myInitBox.mMaxBox  << endl;
        exit(-1);
        }

    double binwidth = (dist_max - dist_min) / (double)nbins;

    for (int i=0; i<nskip; i++) 
        {
        if (bmt.ReadFrame()) 
            {
            cerr << "Error reading " << i << " frame during skip" << endl;
            exit(-1);
            }
        }

    if (bmt.ReadFrame()) 
        {
        cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
        exit(-1);
        }

    FragmentListTable flt(fragfilename);
    FragmentList &central = flt.GetFragmentList(centralfrag);
    AtomValues m(massfilename);


    flt.AssignMasses(m);
    flt.AssignPositions(bmt.getPositions());

    cout << "# Initializing histograms and coordinate vectors" << endl;
    double *hist= new double[nbins];
    for (int j = 0; j<nbins; j++)
        {
        hist[j] = 0.0;
        }

    cout << "# Beginning to read the BMT file " << endl;
    int readerr = 0;
    int n = 0;
    vector<XYZ> central_coor;

    while (!readerr && n < nlength) 
        {
        
        central.FindCenterOfMasses();
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
        n++;
        }

    cout << "# Building the polygon" << endl;
    // build the polygon describing the consensus surface of the central frag
    SurfaceDistance sd(36,-40.0, 40.0, 40);
    sd.AddPoints(central_coor);


    jrand rx = jrand();
    jrand ry = jrand();
    jrand rz = jrand();

    double z_range = max_z - min_z;

    XYZ point;

    for (int i=0; i<nsamples; i++)
        {
        point.mX = get_coor(box.mX, rx);
        point.mY = get_coor(box.mY, ry);
        double z = get_coor(1.0, rz);
        point.mZ = (z + 0.5)*z_range + min_z;
        double dist = sd.Distance(point);
        int bin = (int)((dist-dist_min)/binwidth);
        if ( (bin >= 0) && (bin < nbins) )
            {
            hist[bin] += 1.0;
            }
        /*
        if (i % 1000000 == 0)
            {
            cout << "# Finished " << i << " points" << endl;
            for (int j=0; j<nbins; j++)
                {
                double d = dist_min + (j+0.5)*binwidth;
                double val = hist[j] / (double)(i+1.0);
                cout << d << "\t" << val << endl;
                }
            cout << endl;
            }
        */
        }

    cout << "#Dist\tProb" << endl;
    for (int i=0; i<nbins; i++)
        {
        double d = dist_min + (i+0.5)*binwidth;
        double val = hist[i] / (double)nsamples;
        cout << d << "\t" << val << endl;
        }
    
    return 0;
}
