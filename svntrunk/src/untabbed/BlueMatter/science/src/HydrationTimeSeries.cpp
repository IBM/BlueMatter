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
#include <BlueMatter/SimpleTrajectory.hpp>


#define MAX_LIPID 10000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile frag1 frag2"
                    << " atomCut comCut resample skip " 
         << endl;
    exit(-1);
}

int main(int argc, char **argv)
{
    if (argc != 10)
        Usage(argv);

    char *bmtname = argv[1];

    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *frag1name= argv[4];
    char *frag2name = argv[5];
    double atomCut = atof(argv[6]);
    double comCut = atof(argv[7]);
    int nresample = atoi(argv[8]);
    int nskip = atoi(argv[9]);

    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    BMTReader bmt(argv[1]);
    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box = myInitBox.GetDimensionVector();
    // cheesy fix in case the signs of the dimensions get flipped
    box.mX = fabs(box.mX);
    box.mY = fabs(box.mY);
    box.mZ = fabs(box.mZ);

    int total_frames = bmt.getNFrames();
    if (bmt.ReadFrame(nskip)) 
        {
      cerr << "Error reading bmt frame to skip of " 
             << nskip << " frames" << endl;
      exit(-1);
        }

    FragmentListTable flt(fragmentFileName);
    AtomValues m(massFileName);
    FragmentList &frag1 = flt.GetFragmentList(frag1name);
    FragmentList &frag2 = flt.GetFragmentList(frag2name);

    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);

    // we assume that frag1 is really 1 Fragment
    Fragment &solute = frag1.m_Fragments[0];

    int readerr = 0;
    int nframe = 0;
    while (!readerr && nframe < total_frames)
        {
        if (nframe % nresample == 0)
            {
            int count = 0;
            flt.FindCenterOfMasses();
            //solute.FindCenterOfMass();
            for (vector<Fragment>::iterator f  = frag2.m_Fragments.begin();
                                            f != frag2.m_Fragments.end();
                                            f++)
                {
                double distance = Imaging::NearestImageDistance(
                            solute.m_CenterOfMass,
                            f->m_CenterOfMass,
                            box);
                //cout << "sol = " << solute.m_CenterOfMass << endl;
                //cout << "wat = " << f->m_CenterOfMass << endl; 
                //cout << endl;
                if (distance < comCut)
                    {
                    XYZ water = f->m_CenterOfMass;
                    double min_dist = solute.MinDistance(water, box);
                    if (min_dist < atomCut)
                        {
                        count++;
                        }
                    }
                }
            cout << nframe << "    " << count << endl;
            }

        readerr = bmt.ReadFrame(); 
        nframe++;
        }

    return 0;
}
