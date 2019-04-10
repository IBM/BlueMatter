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
 /*
 *  Compute the mean squared displacement curve for a group of molecules.
 *  Diffusion is computed only in the xy plane, but changing the code to
 *  do 3d diffusion is trivial
 *  
 *  Frank may have done this better at some point, but he didn't document 
 *  a damn thing and I can't find it.
 *
 *  Note: the imaging is kind of tricky -- I assume that the imaging in the 
 *  first snap shot is intuitive (eg, all lipids in a leaflet are in the same
 *  image), and then I simply remove all image translations after that.
 */

#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <BlueMatter/Topology.hpp>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>


using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massfile centersegment segmentName" 
                    << " outdir max_delta skip " << endl;
    exit(-1);
}

int main(int argc, char **argv)
{


    if (argc != 9)
        {
        Usage(argv);
        }

    char *bmtname = argv[1];
    char *fragmentFileName = argv[2];
    char *massFile = argv[3];
    char *centersegment = argv[4];
    char *segmentName = argv[5];
    char *outdir = argv[6];
    int max_delta = atoi(argv[7]);
    int nskip = atoi(argv[8]);

    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    BMTReader bmt(bmtname);
    int total_frames = bmt.getNFrames();
    int nframes = total_frames - nskip - 1;
    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box = myInitBox.GetDimensionVector();
    // cheesy fix in case the signs of the dimensions get flipped
    box.mX = fabs(box.mX);
    box.mY = fabs(box.mY);
    box.mZ = fabs(box.mZ);

    // make the output directory
    if (mkdir(outdir, 0777) != 0)
        {
        if (errno != EEXIST)
            {
            cerr << "Error making directory " << outdir << ": " <<
                    strerror(errno) << endl;
            }
        }


    if (bmt.ReadFrame(nskip))
        {
      cerr << "Error reading first bmt frame after skip " << nskip << " frames" << endl;
        exit(-1);
        }

    FragmentListTable flt(fragmentFileName);
    FragmentList &center = flt.GetFragmentList(centersegment);
    FragmentList &fraglist = flt.GetFragmentList(segmentName);
    flt.AssignPositions(bmt.getPositions());
    AtomValues m(massFile);
    flt.AssignMasses(m);
    flt.AssignPositions(bmt.getPositions());

    int num_molecules = fraglist.m_Fragments.size();
    int num_center_frags = center.m_Fragments.size();
    int total_num_atoms = bmt.determineNEntries();

    vector<double> disp;
    disp.resize(max_delta);
    //vector<int> num_counts;
    //num_counts.resize(max_delta);

    vector< vector<XYZ> > coor;
    coor.resize(num_molecules);
    for (int i=0; i<num_molecules; i++)
        {
        coor[i].resize(nframes);
        }
    vector<XYZ> center_prev;
    center_prev.resize(num_center_frags);

    cout << "# num molecules = " << num_molecules << endl;

    int readerr = 0;
    int nframe = 0;
    int i = 0;
    int nconf = 0;
    bool first = true;

    int cur = 0;
    disp.assign(max_delta,0.0);

    while (!readerr && nframe < nframes) 
        {
        // To deal with the center of mass shifting radically 
        // due to reimaging, we shift so the central domain is 
        // in the middle, then remap, then recenter
        center.FindCenterOfMasses(); 
        if (first)
            {
            for (int i = 0; i<num_center_frags; i++)
                {
                center_prev[i] = center.m_Fragments[i].m_CenterOfMass;
                }
            }
        else
            {
            for (int i = 0; i<num_center_frags; i++)
                {
                XYZ foo = Imaging::NearestImagePosition(center_prev[i],
                                     center.m_Fragments[i].m_CenterOfMass,
                                     box);
                XYZ diff = foo - center.m_Fragments[i].m_CenterOfMass;
                center.m_Fragments[i].Translate(diff);
                }
            }

        flt.FindCenterOfMasses();
        XYZ com = -center.m_CenterOfMass;
        TranslateAtoms(flt.m_Positions, total_num_atoms,com);
        fraglist.MapIntoBox(box);
        fraglist.FindCenterOfMasses(); 

        if (first)
            {
            for (int molnum = 0; molnum< num_molecules; molnum++)
                {
                coor[molnum][0] = 
                    fraglist.m_Fragments[molnum].m_CenterOfMass;
                }
            }
        else
            {
            for (int molnum = 0; molnum< num_molecules; molnum++)
                {
                coor[molnum][cur] = Imaging::NearestImagePosition(
                                coor[molnum][cur-1],
                                fraglist.m_Fragments[molnum].m_CenterOfMass,
                                box);
                }
            }
        cur++;
        first = false;

        readerr = bmt.ReadFrame();
        if (readerr)
            {
            cerr << "Error reading bmt, molecule = " << i << "\t"
                 << "frame = " << nframe << endl;
            exit(-1);
            }
        nframe++;
        }

    // Don't forget to rezero disp!
    for (int molnum=0; molnum < num_molecules; molnum++)
        {
        disp.assign(max_delta,0.0);
        // compute displacement function for this molecule
        for (int delta = 0; delta < max_delta; delta++)
            {
            for (int k=0; k < nframes -delta; k++)
                {
                //double dist = coor[k].DistanceSquared(coor[k+delta]);
                XYZ diff = coor[molnum][k] - coor[molnum][k+delta];
                double dist = diff[0]*diff[0] + diff[1]*diff[1];
                disp[delta] += dist;        
                }
            disp[delta] /= (double) (nframes - delta);
            }

        // write out the results
        char filename[1024];
        sprintf(filename,"%s/mol_%d.dat",outdir,molnum);
        ofstream outFile(filename);


        outFile << "#Delta\tDisp^2" << endl;
        for (int delta=0; delta < max_delta; delta++)
            {
            outFile << delta << "\t" << disp[delta] << endl;
            }
        outFile.close();

        }

}
