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
// File: bmtcorrelation.hpp
// Author: RSG
// Date: September 28, 2002
// Namespace: bmt
// Class: Correlation
// Description: Class encapsulating the computation of correlation matrix
//              between a set of variables whose instances are
//              iterated over by a class which is a model of an input iterator.
//              Dereferencing this input iterator must return a class
//              which is a model of a random access iterator which
//              returns a floating point value when dereferenced.
// Template parameter: TInputIterator -- 
// ********************************************************************

#ifndef INCLUDE_BMT_CORRELATION_HPP
#define INCLUDE_BMT_CORRELATION_HPP

#include <BlueMatter/bmtlowerpackedsymmetricmatrix.hpp>

#include <vector>
#include <functional>
#include <iostream>

namespace bmt
{
class Correlation;
}

std::ostream& operator<<(std::ostream&, const bmt::Correlation&);

namespace bmt
{
  class Correlation : public std::unary_function<const std::vector<double>&, int>
  {
    friend std::ostream& ::operator<<(std::ostream&, const bmt::Correlation&);
  private:
    std::vector<double> d_sumX;
    std::vector<std::vector<double> > d_sumXY;
    LowerPackedSymmetricMatrix* d_matrix_p; // symmetric matrix
    int d_count;
  public:
    Correlation();
    ~Correlation();
    // accessor for correlation matrix
    inline double operator()(int, int) const;
    inline int size() const;
    inline int count() const;
    // function object to compute contributions to correlation matrix
    int operator()(const std::vector<float>&);
    // function object returning LowerPackedSymmetricMatrix
    inline const LowerPackedSymmetricMatrix& operator()() const;
    void finalize(); // called after all contributions have been made
  };

  double Correlation::operator()(int i, int j) const
  {
    return((*d_matrix_p)(i,j));
  }

  const LowerPackedSymmetricMatrix& Correlation::operator()() const
  {
    return(*d_matrix_p);
  }

  int Correlation::size() const
  {
    return(d_matrix_p->rows());
  }

  int Correlation::count() const
  {
    return(d_count);
  }
}


#endif
