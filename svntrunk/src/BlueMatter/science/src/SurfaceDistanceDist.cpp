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
                    << "min max  nbins skip" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    XYZ box;


    if (argc < 9)
        Usage(argv);

    cerr << "# ";
    for (int i=0;i<argc;i++)
        {
        cerr << argv[i] << " ";
        }
    cerr << endl;

    char *bmtfilename = argv[1];
    char *fragfilename = argv[2];
    char *massfilename = argv[3];
    char *centralfrag = argv[4];
    double dist_min = atof(argv[5]);
    double dist_max = atof(argv[6]);
    int nbins = atoi(argv[7]);
    int nskip = atoi(argv[8]);

    BMTReader bmt(bmtfilename);
    int nlength = 1000000;


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

    //flt.Print();

    cerr << "# Initializing histograms and coordinate vectors" << endl;
    vector< vector<XYZ> > coordinates; 
    vector< double *> histograms;
    vector<XYZ> central_coor;

    // this is going to end up allocating an extra histogram and coordinate
    // vector, because I'm not excluding the central fragment, but doing
    // so makes the bookkeeping simpler and the memory use is trivial, so
    // who cares...
    for (int i=0; i<flt.m_FragmentLists.size(); i++)
        {
        double *h = new double[nbins];
        for (int j = 0; j<nbins; j++)
            {
            h[j] = 0.0;
            }
        histograms.push_back(h);
    
        vector<XYZ> *v = new vector<XYZ>;
        coordinates.push_back(*v);
        }
    //central.Print();


    cerr << "# Beginning to read the BMT file " << endl;
    int readerr = 0;
    int n = 0;

    while (!readerr && n < nlength) 
        {
        
        //central.FindCenterOfMasses();
        flt.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        flt.Translate(com);
        flt.MapIntoBoxByAtom(box);
        
        for (int i=0; i<flt.m_FragmentLists.size(); i++) 
            {
            FragmentList &fl = flt.m_FragmentLists[i];
            for (int j=0; j<fl.m_Fragments.size(); j++) 
                {
                Fragment &f = fl.m_Fragments[j];
                for (int k=0; k < f.m_Count; k++)
                    {
                    int index = f.m_Start + k;
                    XYZ p =f.m_Positions[index];
                    coordinates[i].push_back(p);
                    }
                }
            }

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

    cerr << "# Building the polygon" << endl;
    // build the polygon describing the consensus surface of the central frag
    SurfaceDistance sd(36,-40.0, 40.0, 40);
    sd.AddPoints(central_coor);
    //sd.Print();


    // loop over the other fragments, build a vector of distances
    for (int i=0; i<flt.m_FragmentLists.size(); i++)
        {
        FragmentList &fl = flt.m_FragmentLists[i];
        cerr << "# computing distances for fragment " << fl.m_Name << endl;
        double *hist = histograms[i];
        vector<XYZ> &coor = coordinates[i];
        // loop over the coordinates and compute the distance to the surface
        for (int j=0; j<coor.size(); j++) 
            {
            double dist = sd.Distance(coor[j]);
            int bin = (int)((dist-dist_min)/binwidth);
            //cout << j << "\t" << bin << "\t" << dist << endl;
            // ignore any data outside our predefined boundaries
            if ( (bin >= 0) && (bin < nbins) )
                {
                hist[bin] += 1.0;
                }
            }
        }

    
    // loop over the fragments and normalize and write out the histograms
    for (int i=0; i<flt.m_FragmentLists.size(); i++) 
        {
        FragmentList &fl = flt.m_FragmentLists[i];
        double *hist = histograms[i];
        char filename[1024];
        sprintf(filename, "%s.dat", fl.m_Name);
        ofstream outfile(filename, ios::out);
        // normalize by number of frames and number of atoms / fragment
        // assuming all fragments in a fragmentlist have the same number of
        // atoms
        int num_atoms_per = fl.m_Fragments[0].m_Count;
        double norm = (double)(n * num_atoms_per);
        outfile << "# nframes = " << n << endl;
        outfile << "# natoms = " << num_atoms_per << endl;
        outfile << "#Dist\tNumberDensity" << endl;
        for (int k=0; k<nbins; k++)
            {
            double z = dist_min + (k+0.5)*binwidth;
            double val = hist[k] / norm;
            outfile << z << "\t" << val << endl;
            }
        outfile.close();
        }


    return 0;
}
