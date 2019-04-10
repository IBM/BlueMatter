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
 // *********************************************************************
// File: stats.hpp
// Author: RSG
// Date: April 11, 2004
// Class: Stats
// Description: Function object to be used with std::for_each to
//              compute a set of statistical quantities on one or more
//              sequences of data
// *********************************************************************
#ifndef INCLUDE_STATS_HPP
#define INCLUDE_STATS_HPP

#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>
#include <sys/limits.h>

template <class T>
class Stats
{
public:
  typedef double t_float;
private:
  t_float d_sumX;
  t_float d_sumXX;
  t_float d_minX;
  t_float d_maxX;
  t_float d_n;
  t_float d_ll;
public:
  Stats();
  Stats(const Stats&);
  inline void operator()(const T);
  t_float avg() const;
  t_float var() const;
  t_float stddev() const;
  t_float min() const;
  t_float max() const;
  t_float count() const;
  static void histogram(const std::vector<T>&, // input data
                        std::vector<T>&,  // bin boundaries
                        std::vector<unsigned>&); // histogram
};

template <class T>
Stats<T>::Stats()
  : d_sumX(0), d_sumXX(0), d_minX(DBL_MAX), d_maxX(-DBL_MAX), d_n(0),
    d_ll(0)
{
}

template <class T>
Stats<T>::Stats(const Stats& proto)
  : d_sumX(proto.d_sumX), d_sumXX(proto.d_sumXX),
    d_minX(proto.d_minX), d_maxX(proto.d_maxX), d_n(proto.d_n),
    d_ll(proto.d_ll)
{}

template <class T>
void Stats<T>::operator()(const T v)
{
  d_sumX += v;
  d_sumXX += v*v;
  d_minX = v < d_minX ? v : d_minX;
  d_maxX = v > d_maxX ? v : d_maxX;
  d_n = d_n + 1;
  //  std::cerr << "elt = " << v << " ";
  //  std::cerr << "sumX = " << d_sumX << " ";
  //  std::cerr << "n = " << d_n << std::endl;
}

template <class T>
Stats::t_float Stats<T>::avg() const
{
  return(d_sumX/d_n);
}

template <class T>
Stats::t_float Stats<T>::var() const
{
  return((d_sumXX - d_sumX*d_sumX/(d_n-1))/(d_n - 1));
}

template <class T>
Stats::t_float Stats<T>::stddev() const
{
  return(std::sqrt((d_sumXX - d_sumX*d_sumX/(d_n-1))/(d_n - 1)));
}
template <class T>
Stats::t_float Stats<T>::min() const
{
  return(d_minX);
}

template <class T>
Stats::t_float Stats<T>::max() const
{
  return(d_maxX);
}

template <class T>
Stats::t_float Stats<T>::count() const
{
  return(d_n);
}

template <class T>
void Stats<T>::histogram(const std::vector<T>& data,
                        std::vector<T>& bins,
                        std::vector<unsigned>& histogram)
{
  if (bins.empty())
    {
      bins.resize(16);
    }
  histogram.resize(bins.size());
  for (std::vector<unsigned>::iterator iter = histogram.begin();
       iter != histogram.end();
       ++iter)
    {
      *iter = 0;
    }
  T minElt = data.min_element();
  T maxElt = data.max_element();
  T delta = (maxElt - minElt)/static_cast<T>(bins.size() - 1);
  for (std::vector<T>::const_iterator iter = data.begin();
       iter != data.end();
       ++iter)
    {
      int idx = (*iter - minElt)/delta;
      idx = idx < 0 ? 0 : idx;
      idx = idx > bins.size() - 1 ? bins.size() - 1 : idx;
      ++histogram[idx];
    }

  for (int i = 0; i < bins.size(); ++i)
    {
      bins[i] = minElt + (0.5 + i)*delta;
    }
}
#endif
