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
 #ifndef COLUMN_H_HEADER
#define COLUMN_H_HEADER

#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

class Column
{
public:
    char m_Name[1024];
    vector<double> m_Data;
    int m_NPts;
    double m_Min;
    double m_Max;
    double m_Mean;
    double m_Sigma;
    // double m_Slope;
    // double m_Intercept;
    double m_Sum;
    double m_SumSQ;

    Column()
    {
  strcpy(m_Name, "");
  flushStats();
    }

    Column(char *name)
    {
  strcpy(m_Name, name);
  flushStats();
    }

    inline void addValue(double v)
    {
  m_Data.push_back(v);
  m_NPts++;
    }

    inline void bumpValue(int i, double v)
    {
  m_Data[i] += v;
    }

    inline void bump(Column &c)
    {
  for (int i=0; i<c.m_NPts; i++)
      bumpValue(i, c.m_Data[i]);
    }

    inline void flushStats()
    {
  m_Min = +1.0E99;
  m_Max = -1.0E99;
  m_Sum = 0;
  m_SumSQ = 0;
  m_NPts = 0;
  m_Mean = 0;
  m_Sigma = 0;
    }

    inline void flush()
    {
  m_Data.clear();
  flushStats();
    }

    inline void doStats()
    {
  flushStats();
  for (vector<double>::iterator v=m_Data.begin(); v != m_Data.end(); v++) {
      if (m_Min > *v)
        m_Min = *v;
      if (m_Max < *v)
    m_Max = *v;
      m_Sum += *v;
      m_SumSQ += (*v)*(*v);
  }
  m_NPts = m_Data.size();
  m_Mean = m_Sum/m_NPts;
  m_Sigma = sqrt(m_SumSQ/m_NPts - m_Mean*m_Mean);
    }

    // find index of value nearest v
    inline int findIndexLinear(double v, double &nearest)
    {
  int npts = m_Data.size();
  if (npts == 0) {
      nearest = 0;
      return -1;
  }
  if (npts == 1) {
      nearest = m_Data[0]; 
      return 0;
  }
  double y0 = m_Data[0];
  double yn = m_Data[npts-1];
  int index = (v-y0)/(yn-y0)*(npts-1)+0.5;
  if (index < 0)
      index = 0;
  if (index >= npts)
      index = npts-1;
  nearest = m_Data[index];
  return index;
    }

    // do linear fit over range t1 to t2 using col t as x values.  return slope, intercept, and values at t1, t2
    inline void linearFit(Column &t, double t1, double t2, double &m, double &b, double &y1, double &y2, double &msigma, double &bsigma)
    {
  double tn1, tn2;
  int i1 = t.findIndexLinear(t1, tn1);
  int i2 = t.findIndexLinear(t2, tn2);

  if (i1 == i2) {
      m = 0;
      b = 0;
      y1 = 0;
      y2 = 0;
      msigma = 0;
      bsigma = 0;
      return;
  }

  double sumx = 0;
  double sumy = 0;
  double sumxy = 0;
  double sumxx = 0;
  double sumdd = 0;

  int n = 0;

  for (int i=i1; i<=i2; i++) {
      double x = t.m_Data[i];
      double y = m_Data[i];
      sumx += x;
      sumy += y;
      sumxy += x*y;
      sumxx += x*x;
      n++;
  }

  double Delta = n*sumxx - sumx*sumx;
  m = (n*sumxy - sumx*sumy)/Delta;
  b = (sumy*sumxx - sumxy*sumx)/Delta;

  y1 = m*t1+b;
  y2 = m*t2+b;

  for (int i=i1; i<=i2; i++) {
      double x = t.m_Data[i];
      double y = m_Data[i];
      double d = y-(m*x + b);
      sumdd += d*d;
  }

  double var = sumdd/n;

  msigma = sqrt(n*var/Delta);
  bsigma = sqrt(sumxx*var/Delta);
    }

    // do linear fit over range t1 to t2 using this t as x values, double array v as y values.  return slope, intercept, and values at t1, t2
    inline void linearFit(double *v, double t1, double t2, double &m, double &b, double &y1, double &y2, double &msigma, double &bsigma)
    {
  double tn1, tn2;
  int i1 = findIndexLinear(t1, tn1);
  int i2 = findIndexLinear(t2, tn2);

  if (i1 == i2) {
      m = 0;
      b = 0;
      y1 = 0;
      y2 = 0;
      msigma = 0;
      bsigma = 0;
      return;
  }

  double sumx = 0;
  double sumy = 0;
  double sumxy = 0;
  double sumxx = 0;
  double sumdd = 0;

  int n = 0;

  for (int i=i1; i<=i2; i++) {
      double x = m_Data[i];
      double y = v[i];
      sumx += x;
      sumy += y;
      sumxy += x*y;
      sumxx += x*x;
      n++;
  }

  double Delta = n*sumxx - sumx*sumx;
  m = (n*sumxy - sumx*sumy)/Delta;
  b = (sumy*sumxx - sumxy*sumx)/Delta;

  y1 = m*t1+b;
  y2 = m*t2+b;


  for (int i=i1; i<=i2; i++) {
      double x = m_Data[i];
      double y = v[i];
      double d = y-(m*x + b);
      sumdd += d*d;
  }

  double var = sumdd/n;

  msigma = sqrt(n*var/Delta);
  bsigma = sqrt(sumxx*var/Delta);
    }

    // find linear fit at every point of x (this) over local range of +/- dx.
    // use ysigma, and return with yfitsigma
    void linearFit(Column &y, Column &ysigma, double dx, Column &slopefit, Column &slopefitsigma)
    {
  for (int i=0; i<m_NPts; i++) {
      double ret;
      double S = 0;
      double Sx = 0;
      double Sy = 0;
      double Sxy = 0;
      double Sxx = 0;
      int mini = findIndexLinear(m_Data[i]-dx, ret);
      int maxi = findIndexLinear(m_Data[i]+dx, ret);
      int lgap = i-mini;
      int rgap = maxi-i;
      // cout << "init mini maxi lgap rgap " << mini << " " << maxi << " " << lgap << " " << rgap << endl;
      if (lgap != rgap) {
    if (lgap > 0 && rgap > 0) {
        if (lgap < rgap)
      rgap = lgap;
        else
      lgap = rgap;
    } else {
        if (lgap == 0)
      rgap = 1;
        else
      lgap = 1;
    }
    mini = i-lgap;
    maxi = i+rgap;
      }
      // cout << "next mini maxi lgap rgap " << mini << " " << maxi << " " << lgap << " " << rgap << endl;

      for (int j=mini; j<=maxi; j++) {
    double xx = m_Data[j];
    double yy = y.m_Data[j];
    double ss = ysigma.m_Data[j];

    double invsq = 1.0/(ss*ss);

    if (fabs(ss) < 1.0E-10)
        invsq = 1.0E20;

    S += invsq;
    Sx += xx*invsq;
    Sy += yy*invsq;
    Sxx += xx*xx*invsq;
    Sxy += xx*yy*invsq;
    // cout << "calc S " << mini << " " << maxi << " " << i << " " << j << endl;
      }
      // find slope, intercept, sigma slope
      double D = S*Sxx - Sx*Sx;

      if (fabs(D) < 1.0E-10)
    D = 1.0E-10;

      // push slope and sigma slpe
      slopefit.addValue((S*Sxy-Sx*Sy)/D);
      slopefitsigma.addValue(sqrt(S/D));
      // cout << dx << " " << mini << " " << maxi << " " << D << " " << (S*Sxy-Sx*Sy)/D << " " << sqrt(S/D) << endl;;

  }
    }

    void autoCorrelation(double* &corr, bool normalize=false, bool center=true, bool scale=true);
    void diffusion1D(double* &diff, int &NDPts);
    void diffusion2D(double* &diff, int &NDPts, Column &y);
    void diffusion3D(double* &diff, int &NDPts, Column &y, Column &z);
    void smooth(Column &smoothed, int n, bool reduce=true);

};

#endif
