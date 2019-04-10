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
#include <BlueMatter/JRand.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/TableFile.hpp>
// #include "/fs/dx_basic/BinaryIO/Histogram.hpp"
// #include "HBond.hpp"

using namespace std ;


int main(int argc, char **argv)
{
    int i, j;
    const int nperleaf=36;
    const int nlip = 2*nperleaf;
    double timestep = 0.005;
    // x y z * 72, lower leaf first
    // thus tf will have x0, x1, x2, ... x71, y0, y1... z71
    TableFile tf("/fs/lipid5/sope_diff_xyz.dat", nlip);
    int ncols = tf.m_Columns.size();
    assert(ncols == nlip*3);
    int nrows = tf.m_NRows;

    double **a2d, **a3d;
    double *l2dmean, *u2dmean, *l3dmean, *u3dmean, *a2dmean, *a3dmean;
    double *l2dmeanall, *u2dmeanall, *l3dmeanall, *u3dmeanall, *a2dmeanall, *a3dmeanall;
    int NDPtsa2d=0, NDPtsa3d=0;
    l2dmean = new double[nrows];
    u2dmean = new double[nrows];
    l3dmean = new double[nrows];
    u3dmean = new double[nrows];
    a2dmean = new double[nrows];
    a3dmean = new double[nrows];
    l2dmeanall = new double[nrows];
    u2dmeanall = new double[nrows];
    l3dmeanall = new double[nrows];
    u3dmeanall = new double[nrows];
    a2dmeanall = new double[nrows];
    a3dmeanall = new double[nrows];

    a2d = new double*[nlip];
    a3d = new double*[nlip];

    for (i=0; i<nlip; i++) {
  l2dmeanall[i] = 0;
  u2dmeanall[i] = 0;
  l3dmeanall[i] = 0;
  u3dmeanall[i] = 0;
  a2dmeanall[i] = 0;
  a3dmeanall[i] = 0;
    }

    for (i=0; i<nlip; i++) {
  NDPtsa2d = NDPtsa3d = 0;
  tf.m_Columns[i].diffusion2D(a2d[i], NDPtsa2d, tf.m_Columns[i+nlip]);
  tf.m_Columns[i].diffusion3D(a3d[i], NDPtsa3d, tf.m_Columns[i+nlip], tf.m_Columns[i+2*nlip]);
    }

    jrand jrall(nlip);
    jrand jrleaf(nperleaf);

    int nboots = 200;
    // int nboots = 1;

    Column tcol;
    double mint = 0.5;
    double maxt = 13.0;
    double dt = 0.5;
    int nt = (maxt-mint)/dt+1;
    Column *dlcol = new Column[nt]; // 0.5 to 6.0
    Column *ducol = new Column[nt]; // 0.5 to 6.0
    Column *dacol = new Column[nt]; // 0.5 to 6.0
    // loop ntimes
    // each time find diffusion of randomly created ensemble of lower, upper, and all.
    for (int nboot=0; nboot<nboots; nboot++) {
  for (i=0; i<nrows; i++)
      l2dmean[i] = u2dmean[i] = l3dmean[i] = u3dmean[i] = a2dmean[i] = a3dmean[i]=0;

#if 1	
  // select from all
  for (int ntry=0; ntry<nlip; ntry++) {
      int rall = jrall.get();
      for (j=0; j<NDPtsa2d; j++) {
    a2dmean[j] += a2d[rall][j]/nlip/4.0;
    a3dmean[j] += a3d[rall][j]/nlip/6.0;
      }
  }

  // select from each leaf separately
  for (int ntry=0; ntry<nperleaf; ntry++) {
      int rlower = jrleaf.get();
      int rupper = jrleaf.get()+nperleaf;
      for (j=0; j<NDPtsa2d; j++) {
    l2dmean[j] += a2d[rlower][j]/nperleaf/4.0;
    u2dmean[j] += a2d[rupper][j]/nperleaf/4.0;
    l3dmean[j] += a3d[rlower][j]/nperleaf/6.0;
    u3dmean[j] += a3d[rupper][j]/nperleaf/6.0;
      }
  }
#endif

  // build up time column for use by linear fit calls
  if (tcol.m_NPts == 0) {
      for (i=0; i<NDPtsa2d; i++)
    tcol.addValue(i*timestep);
      tcol.doStats();
  }

#if 0
  // for direct averaging of individ. lipid curves
  for (i=0; i<nt; i++) {
      double x1 = mint+dt*i-dt/2;
      double x2 = mint+dt*i+dt/2;
      double m, b, y1, y2, msigma, bsigma;
      for (int j=0; j<nperleaf; j++) {
    tcol.linearFit(a2d[j], x1, x2, m, b, y1, y2, msigma, bsigma);
    dlcol[i].addValue(m/4);
    dacol[i].addValue(m/4);
    tcol.linearFit(a2d[j+nperleaf], x1, x2, m, b, y1, y2, msigma, bsigma);
    ducol[i].addValue(m/4);
    dacol[i].addValue(m/4);
      }
  }
#endif

#if 1
  for (i=0; i<nt; i++) {
      double x1 = mint+dt*i-dt/2;
      double x2 = mint+dt*i+dt/2;
      double m, b, y1, y2, msigma, bsigma;
      tcol.linearFit(l2dmean, x1, x2, m, b, y1, y2, msigma, bsigma);
      dlcol[i].addValue(m);
      tcol.linearFit(u2dmean, x1, x2, m, b, y1, y2, msigma, bsigma);
      ducol[i].addValue(m);
      tcol.linearFit(a2dmean, x1, x2, m, b, y1, y2, msigma, bsigma);
      dacol[i].addValue(m);
  }
#endif


#if 0
  for (i=0; i<NDPtsa2d; i++) {
      double t = i*timestep;
      cout << t << " " << l2dmean[i] << " " << u2dmean[i] << " " << (l2dmean[i]+u2dmean[i])/2 << " "
    << l3dmean[i] << " " << u3dmean[i] << " " << (l3dmean[i]+u3dmean[i])/2
    << " " << a2dmean[i] << " " << a3dmean[i] << endl;
  }
#endif
    }

    for (i=0; i<nt; i++) {
  dlcol[i].doStats();
  ducol[i].doStats();
  dacol[i].doStats();
    }

    for (i=0; i<nt; i++) {
  double t = mint + dt*i;
  cout << t << " " << dlcol[i].m_Mean << " " << dlcol[i].m_Sigma
            << " " << ducol[i].m_Mean << " " << ducol[i].m_Sigma
            << " " << dacol[i].m_Mean << " " << dacol[i].m_Sigma
      << endl;
    }


    return 0;
}
