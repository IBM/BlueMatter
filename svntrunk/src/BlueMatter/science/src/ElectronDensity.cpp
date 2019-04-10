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
#include <BlueMatter/Align.hpp>

using namespace std ;
 
void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt frag.txt masses.txt charges.txt " 
                    << "atomic_numbers.txt centerfrag outputdir nbins skip" 
                    << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    SimpleTrajectory st;

    XYZ box;

    int nlength = 1000000000;

    if (argc < 10)
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
    char *centerfrag = argv[6];
    char *outputdirname = argv[7];
    int nbins = atoi(argv[8]);
    int nskip = atoi(argv[9]);

    BMTReader bmt(bmtfilename);

    // AK: changed access
    st.m_StartTime = bmt.getInitialSimulationTimeInNanoSeconds();
    st.m_TimeStep = bmt.getNetTimeStepInPicoSeconds()/1000.0;

    BoundingBox myInitBox=bmt.getInitialBox();
    box.mX = fabs(myInitBox.mMaxBox.mX - myInitBox.mMinBox.mX);
    box.mY = fabs(myInitBox.mMaxBox.mY - myInitBox.mMinBox.mY);
    box.mZ = fabs(myInitBox.mMaxBox.mZ - myInitBox.mMinBox.mZ);

    if (fabs(box.mX) < 1 || fabs(box.mY) < 1 || fabs(box.mZ) < 1) 
        {
        cerr << "Box from bmt is missing or too small: " << myInitBox.mMinBox << " " << myInitBox.mMaxBox  << endl;
        exit(-1);
        }


    double binwidth = box.mZ / (double)nbins;
    double min_z = -0.5*box.mZ;
    double max_z = 0.5*box.mZ;

    float actualstart = st.m_StartTime+nskip*st.m_TimeStep;

    if (nskip > 0) 
        {
        cerr << "# Skipping first " << nskip << " frames" << endl;
        cerr << "# Starting at " << actualstart << " nanoseconds" << endl;
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
    FragmentList &center = dbl.GetFragmentList(centerfrag);
    AtomValues m(massfilename);
    AtomValues c(chargefilename);
    iAtomValues an(atomicnumberfilename);
    dbl.AssignPositions(bmt.getPositions());
    dbl.AssignMasses(m);
    dbl.AssignAtomicNumbers(an);
    dbl.AssignCharges(c);

    vector<double *> vh;

    // init a histogram for each FragmentList
    for (int i=0; i<dbl.m_FragmentLists.size(); i++) 
        {
        FragmentList &dl = dbl.m_FragmentLists[i];
        double *h = new double[nbins];
        for (int k=0; k<nbins; k++)
            {
            h[k] = 0.0;
            }
        vh.push_back(h);
        }

    int readerr = 0;
    int n = 0;

    while (!readerr && n < nlength) 
        {
        dbl.MapIntoBox(box);
        dbl.FindCenterOfMasses();
        for (int i=0; i<dbl.m_FragmentLists.size(); i++) 
            {
            FragmentList &dl = dbl.m_FragmentLists[i];
            for (int j=0; j<dl.m_Fragments.size(); j++) 
                {
                Fragment &f = dl.m_Fragments[j];
                f.Translate(-center.m_CenterOfMass);
                f.AssignElectronDensity(vh[i], nbins, min_z, max_z,
                                        binwidth);
                // put it back, in case atoms in this fragment are specified
                // more than once
                f.Translate(center.m_CenterOfMass);
                }
            }

        readerr = bmt.ReadFrame();
        n++;
        }

    // loop over the fragments and normalize and write out the histograms
    for (int i=0; i<dbl.m_FragmentLists.size(); i++) 
        {
        FragmentList &dl = dbl.m_FragmentLists[i];
        double *hist = vh[i];
        char filename[1024];
        sprintf(filename, "%s/%s.dat", outputdirname, dl.m_Name);
        ofstream outfile(filename, ios::out);
        outfile << "#Z\tElectrons" << endl;
        double norm = (double)n * box.mX * box.mY * binwidth;
        cout << "# norm = " << norm << endl;
        cout << "# binwidth = " << binwidth << endl;
        cout << "# box = " << box << endl;
        cout << "# n = " << n << endl;
        for (int k=0; k<nbins; k++)
            {
            double z = min_z + (k+0.5)*binwidth;
            double val = hist[k] / norm;
            outfile << z << "\t" << val << "\t" << hist[k] << endl;
            }
        outfile.close();
        }



    return 0;
}
