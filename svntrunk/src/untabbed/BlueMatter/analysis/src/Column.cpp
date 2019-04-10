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
 #include <BlueMatter/Column.hpp>


// normalize means divide by window width as it shrinks
// allocate space in corr
// return npts in corr
void Column::autoCorrelation(double* &corr, bool normalize, bool center, bool scale)
{
    vector<double>::iterator v;
    double sum = 0;
    double *x = new double[m_NPts];
    double *px = x;
    double sumsq = 0;
    double c;
    corr = new double[m_NPts];
    if (center) {
  doStats();
  for (v=m_Data.begin(); v != m_Data.end(); v++, px++) {
      *px = *v - m_Mean;
      sumsq += (*px)*(*px);
  }
    } else {
  for (v=m_Data.begin(); v != m_Data.end(); v++, px++) {
      *px = *v;
      sumsq += (*px)*(*px);
  }
    }
    bool isconst = false;
    if (sumsq < 1.0E-15) {
  sumsq = 1.0;
  isconst = true;
    }
    double normfactor = 1;
    for (int i=0; i<m_NPts; i++) {
  if (normalize)
      normfactor = double(m_NPts)/(m_NPts-i);
  px = x;
  double *py = px+i;
  c = 0;
  for (int j=i; j<m_NPts; j++, px++, py++)
      c += (*px)*(*py);
  if (!isconst)
      corr[i] = c/sumsq*normfactor;
  else
      corr[i] = 0.0*normfactor;
  if (!scale)
      corr[i] = c;
    }
    delete [] x;
}

void Column::diffusion1D(double* &diff, int &NDPts)
{
    if (NDPts == 0) {
  NDPts = m_NPts-2;
  if (NDPts < 1)
      NDPts = 2;
  diff = new double[NDPts];
    }
    
    // 1D diffusion
    // loop over tau, for each accumulating all possible deltas
    // output r^2 as function of time
    // diff can be allocated with NDPts = m_NPts/2
    diff[0] = 0.0;
    for (int tau=1; tau<NDPts; tau++) {
  int nsum = 0;
  diff[tau] = 0.0;
  for (int i=0; i<m_NPts-tau; i++) {
      double d = m_Data[i+tau] - m_Data[i];
      diff[tau] += d*d;
      nsum++;
  }
  diff[tau] /= nsum;
    }
}

void Column::diffusion2D(double* &diff, int &NDPts, Column &y)
{
    if (NDPts == 0) {
  NDPts = m_NPts-2;
  if (NDPts < 1)
      NDPts = 2;
  diff = new double[NDPts];
    }
    
    // 1D diffusion
    // loop over tau, for each accumulating all possible deltas
    // output r^2 as function of time
    // diff can be allocated with NDPts = m_NPts/2
    diff[0] = 0.0;
    for (int tau=1; tau<NDPts; tau++) {
  int nsum = 0;
  diff[tau] = 0.0;
  for (int i=0; i<m_NPts-tau; i++) {
      double x0 = m_Data[i];
      double x1 = m_Data[i+tau];
      double y0 = y.m_Data[i];
      double y1 = y.m_Data[i+tau];
      double dx = x1-x0;
      double dy = y1-y0;
      diff[tau] += dx*dx+dy*dy;
      nsum++;
  }
  diff[tau] /= nsum;
    }
}

void Column::diffusion3D(double* &diff, int &NDPts, Column &y, Column &z)
{
    if (NDPts == 0) {
  NDPts = m_NPts-2;
  if (NDPts < 1)
      NDPts = 2;
  diff = new double[NDPts];
    }
    
    // 1D diffusion
    // loop over tau, for each accumulating all possible deltas
    // output r^2 as function of time
    // diff can be allocated with NDPts = m_NPts/2
    diff[0] = 0.0;
    for (int tau=1; tau<NDPts; tau++) {
  int nsum = 0;
  diff[tau] = 0.0;
  for (int i=0; i<m_NPts-tau; i++) {
      double x0 = m_Data[i];
      double x1 = m_Data[i+tau];
      double y0 = y.m_Data[i];
      double y1 = y.m_Data[i+tau];
      double z0 = z.m_Data[i];
      double z1 = z.m_Data[i+tau];
      double dx = x1-x0;
      double dy = y1-y0;
      double dz = z1-z0;
      diff[tau] += dx*dx+dy*dy+dz*dz;
      nsum++;
  }
  diff[tau] /= nsum;
    }
}

void Column::smooth(Column &smoothed, int n, bool reduce)
{
    if (reduce) {
  for (int i=0; i<m_NPts; i+= n) {
      double sum = 0;
      for (int j=0; j<n; j++)
    sum += m_Data[i+j];
      smoothed.addValue(sum/n);
  }
  smoothed.doStats();
    }
}
