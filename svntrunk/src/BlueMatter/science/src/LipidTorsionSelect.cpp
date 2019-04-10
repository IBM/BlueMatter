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
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <errno.h>
//#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <BlueMatter/BMT.hpp>
//#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
//#include <BlueMatter/DXHistogram.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>
#include <BlueMatter/Torsions.hpp>
#include <BlueMatter/SurfaceDistance.hpp>


#define MAX_LIPID 10000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile massFile centralSegmentName " 
                    << " segmentName TorsionFile isGGT select_min select_max "
                    << " skip" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{


    if (argc != 11)
        {
        Usage(argv);
        }

    char *bmtname = argv[1];
    char *fragmentFileName = argv[2];
    char *massFileName = argv[3];
    char *centralName = argv[4];
    char *segmentName = argv[5];
    char *torsionFileName = argv[6];
    bool isGGT;
    if (atoi(argv[7]))
        {
        isGGT = true;
        }
    else
        {
        isGGT = false;
        }
    float select_dist_min = atof(argv[8]);
    float select_dist_max = atof(argv[9]);
    int nskip = atoi(argv[10]);


    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    BMTReader bmt(argv[1]);
    BMTReader bmt2(argv[1]);

    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box = myInitBox.GetDimensionVector();

    for (int i = 0; i< nskip; i++)
        {
        if (bmt.ReadFrame())
            {
            cerr << "Error reading frame " << i << "during skip" << endl;
            exit(-1);
            }
        }

    if (bmt.ReadFrame()) 
        {
	    cerr << "Error reading first bmt frame after skip of " << nskip << " frames" << endl;
        exit(-1);
        }

    FragmentListTable flt(fragmentFileName);
    AtomValues m(massFileName);
    FragmentList &selected = flt.GetFragmentList(segmentName);
    FragmentList &central= flt.GetFragmentList(centralName);

    flt.AssignPositions(bmt.getPositions());
    flt.AssignMasses(m);

    vector<XYZ> central_coor;

    // first loop through the trajectory -- build the protein surface
    int readerr = 0;
    int nframe = 0;
    while (!readerr && nframe < 100000000)
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

    // build the protein surface
    SurfaceDistance sd(36,-40.0, 40.0, 40);
    sd.AddPoints(central_coor);

    TorsionList tl(torsionFileName,isGGT);

    //map<string, int> conf_hist;
    //ConformationList conf_list(tl.NumTorsions() );
    
    // saturated chains (GGT) done as pairs, polyunsaturated
    // done singly
    ConformationList conf_list;
    if (isGGT)
        {
        conf_list.SetNumTors(tl.NumTorsions()/2 );
        }
    else
        {
        conf_list.SetNumTors(tl.NumTorsions());
        }

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

    int i = 0;
    int nconf = 0;
    flt.AssignPositions(bmt2.getPositions());

    while (!readerr && nframe < 100000000) 
        {
        flt.FindCenterOfMasses();
        XYZ com = -central.m_CenterOfMass;
        flt.Translate(com);
        flt.MapIntoBox(box);
        for (vector<Fragment>::iterator f =selected.m_Fragments.begin();
                                         f!=selected.m_Fragments.end();
                                         f++)
            {
            float dist = sd.Distance(f->m_CenterOfMass);
            if ( (dist < select_dist_max) &&
                 (dist >= select_dist_min) )
                {
                //string conf = tl.GetConformation(*f);
                string conf;
                if (isGGT)
                    {
                    conf = tl.GetPairedConformation(*f);
                    }
                else
                    {
                    conf = tl.GetConformation(*f);
                    }
                conf_list.CountConformation(conf);
                nconf++;
                }
            }
        readerr = bmt2.ReadFrame(); 
        nframe++;
        }

    /*  Previous production version
    //conf_list.PrintHist();
    ConformationList *merged_list = conf_list.MergeMirrorConfs();

    for (map<string,int>::iterator conf = merged_list->m_hist.begin();
                                   conf!= merged_list->m_hist.end();
                                   conf++)
        {
        string mirror = merged_list->MirrorConf(conf->first);
        int diff = abs(conf_list[conf->first] - conf_list[mirror]);
        float fracdiff = (float)diff / (float)(conf->second);
        float prob = (float)(conf->second)/(float)(merged_list->GetNumCounts());

        cout << conf->first << "  " << conf->second << " " << prob << "  "
             << conf_list[conf->first] << "  " << conf_list[mirror]
             << "  " << diff << "  " << fracdiff << endl;
        }
    */

    int num_counts = conf_list.GetNumCounts();
    for (map<string,int>::iterator conf = conf_list.m_hist.begin();
                                   conf!= conf_list.m_hist.end();
                                   conf++)
        {
        float prob = (float)(conf->second)/(float)num_counts;

        cout << conf->first << "  " << conf->second << " " << prob << endl;
        }

    return 0;
}
