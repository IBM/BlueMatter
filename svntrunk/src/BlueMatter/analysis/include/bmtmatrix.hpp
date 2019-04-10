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
// File: bmtmatrix.hpp
// Author: RSG
// Date: October 10, 2002
// Namespace: bmt
// Class: Matrix
// Description: Base class defining interface for matrix (using the
//              Barton and Nackman trick)
// Template parameter: TEngine 
// 
// Iterator: TIter will be invoked by allUniqueDo() via an overloaded
//           function operator with arguments: double& elt, int i, int j
// ********************************************************************

#ifndef INCLUDE_BMT_BMTMATRIX_HPP
#define INCLUDE_BMT_BMTMATRIX_HPP

#include <vector>
#include <utility>
#include <iostream>
#include <iomanip>

namespace bmt
{
  template <class TEngine>
  class Matrix
  {
  private:
    std::vector<double> d_elt;
    int d_rows;
    int d_cols;
  protected:
    inline Matrix(int rows, int cols, int size) :
      d_elt(size, 0), d_rows(rows), d_cols(cols)
    {}

    inline std::vector<double>& elt()
    {
      return d_elt;
    }

  public:
    typedef typename std::vector<double>::iterator iterator;
    typedef typename std::vector<double>::const_iterator const_iterator;


    inline const std::vector<double>& elt() const
    {
      return d_elt;
    }

    inline TEngine& asEngine()
    {
      return static_cast<TEngine&>(*this);
    }

    inline const TEngine& asEngine() const
    {
      return static_cast<const TEngine&>(*this);
    }

    inline int rows() const
    {
      return d_rows;
    }

    inline int cols() const
    {
      return d_cols;
    }

    inline double operator()(int i, int j) const
    {
      return asEngine()(i,j);
    }

    inline double& operator()(int i, int j)
    {
      return asEngine()(i,j);
    }
    
    inline iterator begin()
    {
      return d_elt.begin();
    }

    inline const_iterator begin() const
    {
      return d_elt.begin();
    }
    
    inline iterator end()
    {
      return d_elt.end();
    }

    inline const_iterator end() const
    {
      return d_elt.end();
    }

    inline std::ostream& output(std::ostream& os) const
    {
      for (int i = 0; i < rows(); ++i)
	{
	  for (int j = 0; j < cols(); ++j)
	    {
	      os << std::setw(8) << std::setprecision(2) << (*this)(i,j) << " ";
	    }
	  os << std::endl;
	}
      return os;
    }
  };
}

#endif
