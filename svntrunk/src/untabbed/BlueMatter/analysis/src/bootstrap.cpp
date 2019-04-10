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
// File: bootstrap.cpp
// Author: RSG
// Date: April 17, 2002
// Class: Bootstrap
// Template Parameter: TDerived (function object taking a const reference
//                     to a vector<double> as an argument and
//                     returning a double
// Description: class encapsulating the operations associated with the
//              "bootstrap" procedure used to estimate uncertainties
//              of a function defined on a set of iid values.
// **********************************************************************

#include <BlueMatter/bootstrap.hpp>
#include <BlueMatter/rmsd.hpp>
#include <algorithm>

Bootstrap::Bootstrap(const std::vector<double>& data) : d_data(data)
{}

Bootstrap::~Bootstrap()
{}

const std::vector<double>& Bootstrap::eval(int size)
{
  Rmsd derived;
  time_t foo;
  foo = time(&foo);
  unsigned int seed = foo;

  unsigned int length = d_data.size();

  char state[256];
  initstate(seed, state, 256);

  std::vector<double> syntheticValue;
  syntheticValue.reserve(d_data.size());
  d_syntheticDerived.clear();
  d_syntheticDerived.reserve(size);

  
  for (int j = 0; j < size; ++j)
    {
      // generate the synthetic data set by random selection from the
      // real dataset with replacement
      for (int i = 0; i < d_data.size(); ++i)
  {
    int index = random() % d_data.size();
    syntheticValue.push_back(d_data[index]);
  }
      // compute the derived quantity for the synthetic dataset
      d_syntheticDerived.push_back(derived(syntheticValue));
      syntheticValue.clear();
    }

  std::sort(d_syntheticDerived.begin(), d_syntheticDerived.end());
  return(d_syntheticDerived);
}
