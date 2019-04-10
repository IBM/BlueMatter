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
 // **********************************************************************
//
// File: autocorrelation.hpp
// Author: RSG
// Date: January 24, 2008
// Class: AutoCorrelation
// Description: Computes autocorrelation of a time series expressed as
//              an array of doubles by brute force lag product
//              computation distributed over p processors.
//              The lag products will only be computed up to some
//              maximum lag specified on the command line.  The mean
//              of the input data will be subtracted prior to
//              computation of the autocorrelation.
//
// **********************************************************************

#ifndef INCLUDE_AUTOCORRELATION_HPP
#define INCLUDE_AUTOCORRELATION_HPP

#include <vector>

class AutoCorrelation
{
public:
  typedef float TFloat;
  typedef double TDouble;
private:
  TDouble* d_cxxUnalignedBase_p;
  TDouble* d_cxx; // summed lag products, indexed by lag
  long d_window;
  const int d_chunkSize; // cache_line length
  TDouble d_mean;
public:
  struct ReturnData
  {
    TDouble d_mean;
    TDouble d_variance;
    const TDouble* d_autocorrelation;
    long d_maxLag;
  };
  AutoCorrelation();
  ~AutoCorrelation();
  void init( const long );    // window width (limit on maximum lag)

  const AutoCorrelation::ReturnData
  compute( std::vector<AutoCorrelation::TDouble>::const_iterator, // beginning of input
           std::vector<AutoCorrelation::TDouble>::const_iterator, // end of input
           int threads );                                         // number of threads to use
};


#endif
