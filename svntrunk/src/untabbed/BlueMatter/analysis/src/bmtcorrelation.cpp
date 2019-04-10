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
 // ********************************************************************
// File: bmtcorrelation.cpp
// Author: RSG
// Date: September 28, 2002
// Namespace: bmt
// Class: Correlation
// Description: Class encapsulating the computation of correlation matrix
//              between a set of variables whose instances are
//              iterated over by some STL-style iterator.
// Template parameter: TData -- class which has the characteristics of
//                              a vector of vectors. The first index
//                              varies over variables while the second
//                              index varies over instances of
//                              variable sets.  Specifically, TData
//                              must have the methods begin() and
//                              end() which return const_iterators
//                              which point to std::vector<double/float>
// ********************************************************************


#include <BlueMatter/bmtcorrelation.hpp>
#include <cassert>
#include <iomanip>
#include <cstdlib>

namespace bmt
{

  Correlation::Correlation() : d_count(0), d_matrix_p(NULL)
  {}

  Correlation::~Correlation()
  {
    delete d_matrix_p;
  }
  int Correlation::operator()(const std::vector<float>& data)
  {
    if (d_count == 0) // one-time initialization to set sizes
      {
  std::cerr << "Correlation: doing initialization" << std::endl;
  d_sumX.resize(data.size(), 0.0);
  d_sumXY.resize(d_sumX.size());
  if (d_matrix_p != NULL)
    {
      delete d_matrix_p;
      d_matrix_p = NULL;
    }
  d_matrix_p = new LowerPackedSymmetricMatrix(data.size());
  if (d_matrix_p == NULL)
    {
      std::cerr << "Correlation::operator() ERROR allocating memory for symmetric matrix" << std::endl;
      return(-1);
    }
  int rowLen = 1;
  for (std::vector<std::vector<double> >::iterator iter =
         d_sumXY.begin();
       iter != d_sumXY.end();
       ++iter)
    {
      (*iter).resize(rowLen, 0.0);
      ++rowLen;
    }
      } // end of one-time initialization

    assert(d_sumX.size() == data.size()); // sanity check
    // accumulate contributions
    for (int i = 0; i < data.size(); ++i)
      {
  d_sumX[i] += data[i];
  for (int j = 0; j <= i; ++j)
    {
      d_sumXY[i][j] += data[i]*data[j];
    }
      }

    ++d_count;
    std::cerr << ":";
    if (d_count % 100 == 0)
      {
  std::cerr << std::endl;
      }
    return(d_count);
  }

  void Correlation::finalize()
  {
    double invCount = 1.0/d_count;
    for (std::vector<double>::iterator iter = d_sumX.begin();
   iter != d_sumX.end();
   ++iter)
      {
  *iter *= invCount;
      }
    for (std::vector<std::vector<double> >::iterator iterX = d_sumXY.begin();
   iterX != d_sumXY.end();
   ++iterX)
      {
  for (std::vector<double>::iterator iterY = (*iterX).begin();
       iterY != (*iterX).end();
       ++iterY)
    {
      *iterY *= invCount;
    }
      }

    LowerPackedSymmetricMatrix& matrix = *d_matrix_p;
    for (int i = 0; i < d_sumX.size(); ++i)
      {
  for (int j = 0; j <= i; ++j)
    {
      matrix(i,j) = d_sumXY[i][j] - d_sumX[i]*d_sumX[j];
    }
      }
  }
}

std::ostream& operator<<(std::ostream& os, const bmt::Correlation& c)
{
  const bmt::LowerPackedSymmetricMatrix& matrix = *c.d_matrix_p;
  for (int i = 0; i < c.size(); ++i)
    {
      for (int j = 0; j < c.size(); ++j)
  {
    os << std::scientific << std::setw(12) << std::setprecision(3) << matrix(i,j) << " ";
  }
      os << std::endl;
    }
  
  return os;
}
