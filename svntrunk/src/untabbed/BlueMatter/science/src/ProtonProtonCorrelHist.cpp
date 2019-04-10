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
#include <algorithm>
#include <BlueMatter/BMT.hpp>
//#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
//#include <BlueMatter/DXHistogram.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>
#include <BlueMatter/Torsions.hpp>


using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << " FName.bmt fragmentFile segmentName1 segmentName2 " 
                    << " max_delta skip " << endl;
    exit(-1);
}

int main(int argc, char **argv)
{


    if (argc != 7)
        {
        Usage(argv);
        }

    char *bmtname = argv[1];
    char *fragmentFileName = argv[2];
    char *segmentName1 = argv[3];
    char *segmentName2 = argv[4];
    int max_delta = atoi(argv[5]);
    int nskip = atoi(argv[6]);

    int max_pairs = 50000; // I know this fits in memory for my size bmts

    // time series which never get below threshold_distance angstroms are
    // ignored
    double threshold_distance = 8.0;
    double threshold_d3 = threshold_distance*threshold_distance*
                          threshold_distance;
    threshold_d3 = 1.0/threshold_d3;                    

    double hist_min = 0.0;
    //double hist_max = 5e-5;
    double hist_max = 0.05;
    int num_bins = 1000;
    double bin_width = (hist_max - hist_min)/(double)num_bins;

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


    if (bmt.ReadFrame(nskip))
        {
      cerr << "Error reading first bmt frame after skip " << nskip << " frames" << endl;
        exit(-1);
        }

    FragmentListTable flt(fragmentFileName);

    FragmentList &frag1 = flt.GetFragmentList(segmentName1);
    FragmentList &frag2 = flt.GetFragmentList(segmentName2);

    flt.AssignPositions(bmt.getPositions());

    // get the number of atoms in each fragment list
    int num_atoms_f1 = 0;
    int num_atoms_f2 = 0;
    for (vector<Fragment>::iterator f  = frag1.m_Fragments.begin();
                                    f != frag1.m_Fragments.end();
                                    f++)
        {
        num_atoms_f1 += f->m_Count;
        }

    for (vector<Fragment>::iterator f  = frag2.m_Fragments.begin();
                                    f != frag2.m_Fragments.end();
                                    f++)
        {
        num_atoms_f2 += f->m_Count;
        }
    cout << "# Number of atoms in first block: " << num_atoms_f1 << endl;
    cout << "# Number of atoms in second block: " << num_atoms_f2 << endl;
    int num_pairs = num_atoms_f1*num_atoms_f2;
    cout << "# Number of pairs: " << num_pairs << endl;

    // create the storage for the time series
    vector< vector<double> > time_series;
    if (num_pairs < max_pairs)
        {
        max_pairs = num_pairs;
        }
    time_series.resize(max_pairs);
    for (int i=0; i<max_pairs; i++)
        {
        time_series[i].resize(nframes);
        }

    
    cerr << "# Finished resizing the time series vector" << endl;

    // make a list of all the atom pairs
    vector < vector<int> > pair_indices;
    for (vector<Fragment>::iterator f  = frag1.m_Fragments.begin();
                                    f != frag1.m_Fragments.end();
                                    f++)
        {
        for (int i = 0; i<f->m_Count; i++)
            {
            int index1 = f->m_Start + i;
            for (vector<Fragment>::iterator f2  = frag2.m_Fragments.begin();
                                            f2 != frag2.m_Fragments.end();
                                            f2++) 
                {
                for (int j = 0; j<f2->m_Count; j++)
                    {
                    int index2 = f2->m_Start + j;
                    vector<int> *v = new vector<int>;
                    v->push_back(index1);
                    v->push_back(index2);
                    pair_indices.push_back(*v);
                    }
                }
            }
        }
    
    cerr << "# made list of atom pairs" << endl;
    cerr << "# there are " << pair_indices.size() << " pairs" << endl;
    // create the vector to hold the correlation function histogram
    vector <vector <int> > corr;
    vector <int> num_points;
    int num_pairs_used = 0;

    corr.resize(max_delta);
    for (int i=0; i<max_delta; i++)
        {
        corr[i].resize(num_bins);
        corr[i].assign(num_bins, 0);
        }
    
    num_points.resize(max_delta);
    num_points.assign(max_delta, 0);

    int readerr = 0;
    int nframe = 0;
    int i = 0;
    int nconf = 0;

    // loop over pairs of protons and accumulate a time series of 1/r^3
    
    int cycle = 0;
    int num_remaining = num_pairs;
    while (num_remaining > 0)
        {
        // figure out how many pairs are left
        int num_loop;
        if (num_remaining > max_pairs)
            {
            num_remaining -= max_pairs;
            num_loop = max_pairs;
            }
        else
            {
            num_loop = num_remaining;
            num_remaining = 0;
            }

        while (!readerr && nframe < nframes) 
            {
            for (int loop = 0; loop<num_loop; loop++)
                {
                // given pair number, get indices of current pair
                int index = loop + max_pairs*cycle;
                int ind1 = pair_indices[index][0];
                int ind2 = pair_indices[index][1];

                XYZ p1 = frag1.m_Positions[ind1];
                XYZ p2 = frag2.m_Positions[ind2];
#if 0
                if (cycle > 0)
                    {
                    cerr << "loop = " << loop << endl;
                    cerr << "index = " << index << endl;
                    cerr << "ind1 = " << ind1 << "  " << p1 << endl;
                    cerr << "ind2 = " << ind2 << "  " << p2 << endl;
                    cerr << ind1 << "  " << p1 << "  ";
                    cerr << ind2 << "  " << p2 << endl;;
                    }
#endif
                // Scott suggests turning off imaging
                XYZ vec = Imaging::NearestImageDisplacement(p1,p2,box);
                //XYZ vec = p2 - p1;
                double dist = vec.ReciprocalLength();
                double d3 = dist*dist*dist;
                time_series[loop][nframe] = d3;
                }

            readerr = bmt.ReadFrame();
            if (readerr)
                {
                cerr << "Error reading bmt, cycle = " << cycle 
                     << "frame = " << nframe << endl;
                exit(-1);
                }
            nframe++;
            }
        // build up the correlation function based on the current batch of
        // data
        cerr << "# finished reading cycle, computing correl" << endl;
        cerr << "# num pairs remaining = " << num_remaining << endl;

        for (int j=0; j<num_loop; j++)
            {
            vector<double>::iterator max_val = 
                                    max_element(time_series[j].begin(), 
                                                time_series[j].end());
            //cerr << "# trj " << j << "max = " << *max_val << "\t"
            //     << threshold_d3 << endl;
            if (*max_val >= threshold_d3)
                {
                //cerr << "max_val = " << *max_val << endl;
                num_pairs_used++;
                for (int i=0; i<corr.size(); i++)
                    {
                    if (i % 50 == 0)
                        {
                        for (int k=0; k<time_series[j].size()-i-1; k++)
                            {
                            double val = time_series[j][k] * time_series[j][k+i];
                            int bin = int((val-hist_min)/bin_width);
                            if (bin >= num_bins)
                                {
                                cerr << "value out of bounds" << endl;
                                cerr << j << " " << i << " " << " " << k;
                                cerr << " " << val << " " << bin << endl;
                                exit(-1);
                                }
                            corr[i][bin]++;
                            num_points[i]++;
                            }
                        }
                    }
                }
            }
        cout << "# num pairs used so far = " << num_pairs_used << endl;
        
#if 0
        for (int i=0; i<corr.size(); i++)
            {
            for (int j=0; j<num_loop; j++)
                {
                for (int k=0; k<time_series[j].size()-i-1; k++)
                    {
                    corr[i] += time_series[j][k] * time_series[j][k+i];
                    num_points[i]++;
                    }
                }
            //cerr << i << "\t" << corr[i] << endl;
            }
#endif
        // clean out the time series
        for (int i=0; i<num_loop; i++)
            {
            //time_series[i].clear();
            time_series[i].assign(time_series[i].size(), 0.0);
            }

#if 0
        cerr << "# end of loop " << cycle << endl;
        for (int i=0; i<max_delta; i++)
            {
            double val = corr[i] / num_points[i];
            cerr << i << "\t" << val << endl;
            }
        cerr << endl;
#endif
        cycle++;
        nframe = 0;
        if (bmt.ReadFrame(nskip))
            {
            cerr << "Error rewinding bmt at end of cycle" << endl;
            exit(-1);
            }
        }

    cout << "# total frames = " << nframe << endl;
    // compute the correlation functions
    cout << "# Delta  Corr  Prob" << endl;
    for (int i=0; i<max_delta; i++)
        {
        //double val = corr[i] / num_points[i];
        // multiply by num_pairs_used to remove normalization by
        // number of pairs in the correlation function
        if (i % 50  == 0)
            {
            for (int j=0; j<num_bins; j++)
                {
                double corr_val = hist_min + (double)j * bin_width;
                double val = (double)corr[i][j] / (double)num_points[i];
                cout << i << "\t" << corr_val << "\t" << val << endl;
                }
            cout << endl;
            }
        }


    return 0;
}
