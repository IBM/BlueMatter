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
 #ifndef CORREL_HPP
#define CORREL_HPP

#include <BlueMatter/JRand.hpp>

int read_timeseries(char *filename, vector<fXYZ> &v, float *dt)
    {
    string line;
    ifstream fin(filename);

    v.clear();
    float time_old = 0.0;

    //while (fin.getline(line,1024))
    while (getline(fin,line))
        {
        if (strncmp(line.c_str(),"#",1))
            {
            istringstream str(line);
            float time,x,y,z;
            str >> time;
            *dt = time - time_old;
            time_old = time;
            str >> x;
            str >> y;
            str >> z;
            fXYZ  *vec = new fXYZ;
            vec->mX = x;
            vec->mY = y;
            vec->mZ = z;
            v.push_back(*vec);
            }
        }
    fin.close();
    return v.size();
    }

float correlate(const vector<fXYZ> &v1, const vector<fXYZ> &v2, 
                const int offset)
    {
    float corr = 0.0;
    int points = 0;
    for (int i = 0; i<v1.size(),i+offset<v2.size(); i++)
        {
        corr += v1[i].DotProduct(v2[i+offset]);
        points++;
        }
    // TODO: Think harder about whether this is right
    // if this is right, need to add it to mc_correlate as well
    //for (int i = offset; i<v1.size(),i-offset<v2.size(); i++)
    //    {
    //    corr += v1[i].DotProduct(v2[i-offset]);
    //    points++;
    //    }
    return corr/(float)points;
    }

// add the reverse time component as well
float mc_correlate(const vector<fXYZ> &v1, const vector<fXYZ> &v2,
                   const int offset, const int num_samples)
    {
    jrand random(v2.size()-offset);
    float corr = 0.0;
    for (int i=0; i<num_samples; i++)    
        {
        int index = random.get();
        corr += v1[index].DotProduct(v2[index+offset]);
        }
    return corr / (float)num_samples;
    }


void normalize(vector <fXYZ> &v)
/*
   Normlize the series of vectors v, so we can compute correlations of 
   flucutations.  This is done by subtracting the average vector then rescaling
   to make each vector unit length.
 */
    {
    fXYZ ave;
    ave.mX=0.0;
    ave.mY=0.0;
    ave.mZ=0.0;

    for(int i=0;i<v.size();i++)
        {
        ave += v[i];
        }

    ave.mX /= (float)v.size();
    ave.mY /= (float)v.size();
    ave.mZ /= (float)v.size();

    for(int i=0;i<v.size();i++)
        {
        v[i] -= ave;
        float len = v[i].Length();
        v[i].mX /= len;
        v[i].mY /= len;
        v[i].mZ /= len;
        }

    }


#endif
