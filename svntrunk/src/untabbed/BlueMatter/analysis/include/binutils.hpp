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
 
#include <vector>
#include <iostream>
#include <assert.h>
#include <math.h>

namespace hist
{
class Histogram;
}

std::ostream& operator<<(std::ostream&, const hist::Histogram&);

namespace hist
{
  class Histogram 
  {
    int mNValues , mNBins;
    std::vector<double> mBinBoundaries;
    std::vector<int> mBoundaryElement;
    std::vector<int> mBinCount;
    std::vector<double> mDataValues;
    std::vector<double> mMeans;

  public:
    Histogram(int nbins = 0)
    {
      mNValues = 0;
      mNBins = 0;
      Adjust(nbins);
    }
    void Adjust(int nbins)
    {
      mBinCount.reserve(nbins);	  
      mBoundaryElement.reserve(nbins);
      mBinBoundaries.reserve(nbins);
      mMeans.reserve(nbins);
      
      for(int i = 0; i < nbins; i++)
  {
    mBinCount[i] = 0;
    mBoundaryElement[i] = 0;
    mBinBoundaries[i] = 0.0;
    mMeans[i] = 0.0;
  }
      mNBins = nbins;

      return;
    }

    int NBins(){ return mNBins; }
    int NValues(){ return mNValues; }
    int BinCount(int i){ assert(i < mNBins); return (mBinCount[i]);}
    int BinItem(int bin, int item)
    { 
      assert(bin < mNBins); 
      assert( mBinCount[bin] > item);
      //      return (mDataElements[bin][item]);
      return 0;
    }

    void SetBoundaryVal(int bin, double upperBound)
    {
      assert( bin < NBins() );
      mBinBoundaries[bin] = upperBound;
    }
    int AddValue(double Value)
    {
      //      printf("appending %f\n",Value);
      mDataValues.push_back(Value);
      mNValues++;
      return mNValues;
    }
    double GetValue(int item)
    {
      assert(item < mNValues);
      return mDataValues[item];
    }

    void TrimDataToPowerOf2()
    {
      int i = 0;
      int n = mNValues;
      while(n > 0)
  {
    n >>= 1;
    i++;
  }
      n = 1 << (i-1);
      assert(n < mNValues);
      mNValues = n;

    }
    void UpdateBins()
    {
      int n,elem;
      int nn;
      int i = 0;
      

      // assuming all the values have been added, assin the boundary elements 
      double binwidth = mNValues / mNBins;
      for(i = 0; i < mNBins; i++)
  {
    mBoundaryElement[i] = (i+1) * binwidth;
    //	  printf("Boundary element %d = %d value %f\n",i,mBoundaryElement[i],mDataValues[mBoundaryElement[i]]);
  }
      mBoundaryElement[mNBins - 1] = mNValues;

      double a,x,xx,axx;
      int min,max;
      xx = 0.0;
     for(int bin = 0;bin < mNBins; bin++)
  {

    x = 0.0;
    min = ( bin == 0 ) ?  0 : mBoundaryElement[bin-1];
    max = mBoundaryElement[bin];
    n = 0;
    for(int i=min;i<max;i++)
      {
        n++;
        x += mDataValues[i];
      }
    x /= n;
    mMeans[bin] = x;
    xx += x;
    //	  printf("Mean %d = %f (sum %f)\n",bin,x,xx);
  }
      xx /= mNBins;
      axx = 0.0;
      for(int bin = 0;bin < mNBins; bin++)
  {
    axx += (mMeans[bin] - xx) * (mMeans[bin] - xx);
  }
      axx = sqrt(axx / (mNBins - 1) / mNBins ) ;
      printf(" %f %f %f \n",binwidth,xx,axx);
      return;
    }

    int BinStats( int bin, int &N, double &mean, double &sigma, double &min, double &max)
    {
      double a,x,xx;
      int i,j,n,elem;
      int nn;
      i = bin;

      x = xx = 0.0;
      min = ( bin == 0 ) ? -99999.0 : mBinBoundaries[bin-1];
      max = ( bin == (mNBins - 1) ) ? 99999.0 : mBinBoundaries[bin];
      n = mBinCount[i];
      nn = 0;
      if(n > 0)
  {
    //	  min = max = mDataElements[i][0];
    for(j = 0; j < n; j++)
      {
        a = xx = 0.0;
        //	      elem = mDataElements[i][j];
        //	      elem = 
        //	      a = mDataValues[ elem ];
        //	      if(a < min) min = a;
        //	      if(a > max) max = a;
        x += a;
      }
    x /= n;
    for(j = 0; j < n; j++)
      {
        xx = 0.0;
        //	      elem = mDataElements[i][j];
        //	      a = mDataValues[ elem ] - x;
        xx += a * a;
      }
    xx = sqrt(xx);
    if(n > 1) xx /= (n-1);
  }
      N = n;
      mean = x;
      sigma = xx;
      return n;
    }


  };
}
