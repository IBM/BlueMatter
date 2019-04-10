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
#include <BlueMatter/TableFile.hpp>
#include <BlueMatter/HBond.hpp>

using namespace std ;

Column c[72][36];
double e[72][36];


int main(int argc, char **argv)
{

    // load table of hbond listings
    // 
    TableFile tf("/fs/lipid7/wetsope_14_bond.txt");


    double *corr[72][36];

    int maxstep = tf.m_Columns[0].m_NPts;

    vector<double> &tstep = tf.m_Columns[0].m_Data;
    vector<double> &upper = tf.m_Columns[1].m_Data;
    vector<double> &x = tf.m_Columns[2].m_Data;
    vector<double> &y = tf.m_Columns[3].m_Data;
    vector<double> &energy = tf.m_Columns[5].m_Data;

    double t = tstep[0];
    Column time;
    for (int i=0; i < maxstep; ) {

  // clear bond energy matrix
  for (int j=0; j<72; j++) {
      for (int k=0; k<36; k++)
    e[j][k] = 0;
  }

  // accumulate bonds into the matrix
  while (fabs(tstep[i] - t) < 1.0E-5) {
      // e[int(upper[p])*36 + int(x[p])][int(y[p])] = energy[p];  // single, directed bond energy
      e[int(upper[i]+0.5)*36 + int(x[i]+0.5)][int(y[i]+0.5)] += energy[i];  // add value to both directed pairs
      e[int(upper[i]+0.5)*36 + int(y[i]+0.5)][int(x[i]+0.5)] += energy[i];  // ""
      i++;
  }

  // set binary state of full c matrix for this time
  for (int j=0; j<72; j++) {
      for (int k=0; k<36; k++) {
    c[j][k].addValue(e[j][k] == 0.0 ? 0.0 : 1.0);  // binary addition
      }
  }

  time.addValue(t);
  t=tstep[i];
  // cout << t << endl;
    }

    for (int j=0; j<72; j++) {
  for (int k=0; k<36; k++) {
      c[j][k].doStats();
      c[j][k].autoCorrelation(corr[j][k]);
  }
    }

    // cout << "got autocorrs" << endl;
    int hits;
    int ncorrs = 36*72;
    int npts = c[0][0].m_NPts;
    Column stats;
    for (int i=0; i<npts; i++) {
  hits = 0;
  stats.flush();
  for (int j=0; j<72; j++) {
      for (int k=0; k<36; k++) {
    if (corr[j][k][0] > 0.0) {  // only add the nonconst corrs
        stats.addValue(corr[j][k][i]);
        hits++;
    }
      }
  }
  stats.doStats();
  cout << time.m_Data[i] << " " << stats.m_Mean << " " << stats.m_Sigma << endl;
    }

    return 0;
}
