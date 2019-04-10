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

#define MAX_LIPID 10000

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile segmentName " 
                    << "TorsionFile skip " << endl;
    exit(-1);
}

int main(int argc, char **argv)
{

    if (argc != 6)
        {
        Usage(argv);
        }

    char *bmtname = argv[1];
    char *fragmentFileName = argv[2];
    char *segmentName = argv[3];
    char *torsionFileName = argv[4];
    int nskip = atoi(argv[5]);


    cout << "# ";
    for (int i = 0; i<argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    BMTReader bmt(argv[1]);

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

    FragmentList &selected = flt.GetFragmentList(segmentName);

    flt.AssignPositions(bmt.getPositions());


    TorsionList tl(torsionFileName);

    vector<ConformationList> confs;
    ConformationList all_confs(tl.NumTorsions()/2 ); 
    for (int i = 0; i<selected.m_Fragments.size(); i++)
        {
        ConformationList conf_list(tl.NumTorsions()/2 );
        confs.push_back(conf_list);
        }

    int readerr = 0;
    int nframe = 0;
    int i = 0;
    int nconf = 0;

    while (!readerr && nframe < 100000000) 
        {
        for (int i = 0; i<selected.m_Fragments.size(); i++)
            {
            Fragment f = selected.m_Fragments[i];
            string conf = tl.GetPairedConformation(f);
            confs[i].CountConformation(conf);
            all_confs.CountConformation(conf);
            nconf++;
            }
        readerr = bmt.ReadFrame(); 
        nframe++;
        }

    cout << "# Conf  All Lipids  IndividualLipids" << endl;
    for (map<string,int>::iterator conf = all_confs.m_hist.begin();
                                   conf!= all_confs.m_hist.end();
                                   conf++)
        {
        cout << conf->first << "  " << conf->second << "  ";
        // use conf->first to look up the prob for each lipid
        // spit them out in some same format
        for (int i = 0; i<selected.m_Fragments.size(); i++)
            {
            cout << confs[i][conf->first] << "  ";
            }
        cout << endl;
        }


    return 0;
}
