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
 // ******************************************************************
// File: summation.hpp
// Author: RSG
// Date: October 11, 2006
// Namespace: fp (floating point)
// Description: Routines to implement various floating point
//              "compensation" schemes for summation and other
//              operations to improve the reproducibility of our
//              numerical results.
// ******************************************************************

#ifndef INCLUDE_FP_SUMMATION_HPP
#define INCLUDE_FP_SUMMATION_HPP

#include <BlueMatter/diffscalar.hpp>

#include <algorithm>
#include <numeric>
#include <iostream>
#include <vector>

namespace fp
{
  // Function object to accumulate values
  template <typename TFloat>
  class Accumulator
  {
  private:
    TFloat d_sum;
  public:
    Accumulator() : d_sum(0)
      {}
    const TFloat& value() const
      {
  return(d_sum);
      }
    void operator()(const TFloat& f)
      {
  d_sum += f;
      }
  };

  // Function object that compares absolute values
  template<typename TFloat>
  class CmpAbs
  {
  public:
    inline bool operator()(const TFloat&, const TFloat&) const;
  };

  template<typename TFloat>
  bool CmpAbs<TFloat>::operator()(const TFloat& v1, const TFloat& v2) const
  {
    return(std::fabs(v1) < std::fabs(v2));
  }

  // The following is the branchless sum and roundoff error algorithm
  // from D.M. Priest "On Properties of Floating Point Arithmetics:
  // Numerical Stability and the Cost of Accurate Computations", 1992,
  // pp 18-19.

  template<typename TFloat>
  void sumAndError(const TFloat& a, const TFloat& b, TFloat& s, TFloat& r)
  {
    s = a + b;
    TFloat e = s - a;
    TFloat f = b - e;
    //    TFloat g = s - e; // should be able to use: e = s - e
    e = s - e;
    //    TFloat h = g - a; // should be able to use: e = e - a
    e = e - a;
    //    r = f - h;        // should be able to use: r = f - e
    r = f - e;
  }

  // std::iterator_traits<TIter>::value_type must be a floating point type
  template<typename TIter>
  class SummationStats
  {
  public:
    typedef typename std::iterator_traits<TIter>::value_type value_type;
    typedef typename std::iterator_traits<TIter>::reference reference;

    // args are: begin(), end(), and an array in which to return
    // the distribution function of the summations
    static void distribution(TIter, 
           TIter,
           std::vector<typename std::iterator_traits<TIter>::value_type>&);
  };

  template<typename TIter>
  void SummationStats<TIter>::distribution(TIter first,
             TIter last,
             std::vector<typename std::iterator_traits<TIter>::value_type>& dist)
  {
    if (first == last)
      {
  return;
      }

    std::sort(first, last);
    int permCount = 0;
    do
      {
  Accumulator<typename TIter::value_type> sum;
  sum = std::for_each(first, last, sum);
  dist.push_back(sum.value());
  ++permCount;
      }
    while(std::next_permutation(first, last));
    // compute average and store diffs from average in dist.
    Accumulator<typename TIter::value_type> avgsum;
    avgsum = std::for_each(dist.begin() + (dist.size() - permCount), dist.end(), avgsum);
    TIter::value_type avg = avgsum.value()/permCount;
    for (std::vector<TIter::value_type>::iterator iter = 
     dist.begin() + (dist.size() - permCount);
   iter != dist.end();
   ++iter)
      {
  *iter = diffScalar(*iter, avg);
      }
  }
}
#endif

