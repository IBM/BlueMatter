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

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt frag.txt masses.txt charges.txt " 
                    << "atomic_numbers.txt centralFrag dbinMin dbinMax "
                    << "dnbins zbinmin zbinmax znbins outdir skip" << endl;
    exit(-1);
}

// bmt2histo   sppdchol.bmt  out.txt fragspec.txt bond.txt
int main(int argc, char **argv)
{
    SimpleTrajectory st;

    XYZ box;

    int nlength = 1000000000;

    if (argc < 15)
        Usage(argv);

    cout << "# ";
    for (int i = 0; i < argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    char *bmtfilename = argv[1];
    char *fragfilename = argv[2];
    char *massfilename = argv[3];
    char *chargefilename = argv[4];
    char *atomicnumberfilename = argv[5];
    char *fragname = argv[6];
    float dhistmin = atof(argv[7]);
    float dhistmax = atof(argv[8]);
    int dnbins = atoi(argv[9]);
    float zhistmin = atof(argv[10]);
    float zhistmax = atof(argv[11]);
    int znbins = atoi(argv[12]);
    char *outdir = argv[13];
    int nskip = atoi(argv[14]);

    BMTReader bmt(bmtfilename);
    BMTReader bmt2(bmtfilename);

    float zbinwidth = (zhistmax - zhistmin) / (float) znbins;
    float dbinwidth = (dhistmax - dhistmin) / (float) dnbins;

    // AK: changed access
    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;

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


    float actualstart = st.m_StartTime+nskip*st.m_TimeStep;

    if (nskip > 0) 
        {
        cerr << "# Skipping first " << nskip << " frames" << endl;
        }
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



    FragmentListTable dbl(fragfilename);
    dbl.Print();
    FragmentList &central = dbl.GetFragmentList(fragname);

    AtomValues m(massfilename);
    AtomValues c(chargefilename);
    iAtomValues an(atomicnumberfilename);
    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);
    dbl.AssignAtomicNumbers(an);
    dbl.AssignCharges(c);

    // First loop through the trajectory -- build the protein surface
    int readerr = 0;
    int n = 0;

    vector<XYZ> central_coor;

    while (!readerr && n < nlength) 
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
        n++;
        if (n % 5 == 0)
            {
            cout << "# Finished " << n << " frames" << endl;
            }
        }
    SurfaceDistance sd(36,-40.0, 40.0, 40);
    sd.AddPoints(central_coor);


    vector<double **> vh;

    // init a histogram for each FragmentList
    for (int i=0; i<dbl.m_FragmentLists.size(); i++) 
        {
        FragmentList &fl = dbl.m_FragmentLists[i];
        // don't make a histogram for the central fragmentlist
        double **h = new double* [dnbins];
        for (int j=0; j<dnbins; j++)
            {
            h[j] = new double[znbins];
            for (int k=0; k<znbins; k++)
                {
                    h[j][k] = 0.0;
                }
            }
        vh.push_back(h);
        }

    // second pass through the trajectory
    for (int i=0; i<nskip; i++) 
        {
        if (bmt2.ReadFrame()) 
            {
            cerr << "Error reading " << i << " frame during skip" << endl;
            exit(-1);
            }
        }

    if (bmt2.ReadFrame()) 
        {
        cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
        exit(-1);
        }
    readerr = 0;
    n = 0;
    dbl.AssignPositions(bmt2.getPositions());
    central.AssignPositions(bmt2.getPositions());

    while (!readerr && n < nlength) 
        {
        dbl.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        dbl.Translate(com);
        dbl.MapIntoBoxByAtom(box);
        for (int i=0; i<dbl.m_FragmentLists.size(); i++) 
            {
            FragmentList &fl = dbl.m_FragmentLists[i];
            double **hist = vh[i];
            for (int j=0; j<fl.m_Fragments.size(); j++) 
                {
                Fragment &f = fl.m_Fragments[j];
                for (int k=0; k<f.m_Count; k++)
                    {
                    int index = f.m_Start+k;
                    double electrons = (double)f.m_AtomicNumbers[index] -
                                    f.m_Charges[index]; 
                    double dist = sd.Distance(f.m_Positions[index]);
                    int dbin = (int)((dist-dhistmin) / dbinwidth);
                    double z = f.m_Positions[index].mZ;
                    int zbin = (int)((z-zhistmin) / zbinwidth);
                    if ( (zbin >=0) && (zbin<znbins) &&
                         (dbin >=0) && (dbin<dnbins))
                        {
                        hist[dbin][zbin] += electrons;
                        }
                    }
                }
    
            }
        readerr = bmt2.ReadFrame();
        n++;
        if (n % 5 == 0)
            {
            cout << "# Finished " << n << " frames" << endl;
            }
        }
    cout << "# Normalizing by electrons per fragment" << endl;
    cout << "# Assuming all fragments in a given fragment list are the same" 
         << endl;
    for (int i=0; i<dbl.m_FragmentLists.size(); i++)
        {
        FragmentList &fl = dbl.m_FragmentLists[i];
        double **hist = vh[i];
        double total_electrons = 0.0;
        Fragment &f = fl.m_Fragments[0];
        for (int k=0; k<f.m_Count; k++)
            {
            int index = f.m_Start + k;
            // don't need to add the partial charges, since they have to 
            // cancel out 
            double electrons = (double)f.m_AtomicNumbers[index];
            total_electrons += electrons;
            }
        double sum = 0.0;
        for (int j=0; j<dnbins; j++)
            {
            for (int k=0; k<znbins; k++)
                {
                hist[j][k] /= total_electrons;
                sum += hist[j][k];
                }
            }
        cout << fl.m_Name << " " << sum << endl;
        }

    cout << "# Writing output" << endl;
    // loop over the fragments and write out the histograms
    for (int i=0; i<dbl.m_FragmentLists.size(); i++) 
        {
        FragmentList &fl = dbl.m_FragmentLists[i];
        double **hist = vh[i];
        char filename[1024];
        sprintf(filename, "%s/%s.dat", outdir, fl.m_Name);
        ofstream outfile(filename, ios::out);
        outfile << "#z\tDist\tDens" << endl;
        for (int j=0; j<dnbins; j++)
            {
            for (int k=0; k<znbins; k++)
                {
                double d = dhistmin + (j+0.5)*dbinwidth;
                double z = zhistmin + (k+0.5)*zbinwidth;
                outfile << z << "\t" << d << "\t" << hist[j][k] << endl;
                }
            outfile << endl;
            }
        outfile.close();
        }


    return 0;
}
