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
   Compute the time correlation for two time series of vectors
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <string>
#include <vector>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/correl.hpp>


int main (int argc, char **argv)
{
    char *filename1 = argv[1];
    char *filename2 = argv[2];
    int min_offset = atoi(argv[3]);
    int max_offset = atoi(argv[4]);
    int do_normalize = atoi(argv[5]);

    cout << "# ";
    for(int i = 0; i <argc; i++)
        {
        cout << argv[i] << " ";
        }
    cout << endl;

    vector<fXYZ> v1, v2;
    float dt;
    int npts1 = read_timeseries(filename1, v1, &dt);
    int npts2 = read_timeseries(filename2, v2, &dt);

    cout << "# Read " << npts1 << " from " << filename1 << endl;
    cout << "# Read " << npts2 << " from " << filename2 << endl;

    if (do_normalize)
        {
        normalize(v1);
        normalize(v2);
        }

    /*
    float mean1, dev1;
    float mean2, dev2;

    do_stats(v1, mean1, dev1);
    do_stats(v2, mean2, dev2);
    */

    vector<float> correl;

    for(int i = min_offset; i <= max_offset; i++)
        {
        float c;
        if (i >= 0)
            {
            c = correlate(v1,v2,i);
            }
        else
            {
            c = correlate(v2,v1,-i);
            }
        correl.push_back(c);
        }

    cout << "# Dt      Correl" << endl;
    //float zero_time = correl[0];
    for (int i = 0; i<correl.size(); i++)
        {
        float time = (float)(i + min_offset)*dt;
        //cout << time << "   " << correl[i]/correl[0] << endl;
        cout << time << "   " << correl[i] << endl;
        }
}

