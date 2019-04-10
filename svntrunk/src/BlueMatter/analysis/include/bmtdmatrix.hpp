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
// File: bmtdmatrix.hpp
// Author: RSG
// Date: September 26, 2002
// Namespace: bmt
// Class: DMatrix
// Description: Class encapsulating the computation of the distance
//              matrix describing the distances between sites
//              specified as a vector of integers
// ********************************************************************

#ifndef INCLUDE_BMT_BMTDMATRIX_HPP
#define INCLUDE_BMT_BMTDMATRIX_HPP

#include <BlueMatter/bmtposition.hpp>
#include <vector>
#include <functional>
#include <utility>

namespace bmt
{
  class DMatrix : public std::unary_function<const std::vector<Position>&, const std::vector<float>&>
  {
  private:
    std::vector<int> d_site;
    std::vector<std::pair<int,int> > d_map; // mapping from index in
    // vector of distances to corresponding pairs of site id indexes
    // into site list
    std::vector<float> d_distance; // lower triangular without diagonal
    // matrix
  public:
    typedef std::vector<float>::const_iterator const_iterator;
    DMatrix(const std::vector<int>&); // argument is array of site ids
    const std::vector<float>& operator()(const std::vector<Position>&);
    inline float operator()(const int, const int) const;
    inline int size() const;
    inline const_iterator begin();
    inline const_iterator end();
    const std::vector<std::pair<int,int> >& pairMap() const;
  };

  DMatrix::const_iterator DMatrix::begin()
  {
    return(d_distance.begin());
  }

  DMatrix::const_iterator DMatrix::end()
  {
    return(d_distance.end());
  }

  int DMatrix::size() const
  {
    return(d_site.size());
  }


  float DMatrix::operator()(const int i, const int j) const
  {
    return(j <= i ? d_distance[i*(i-1)+j] : d_distance[j*(j-1)+i]);
  }

}
#endif
