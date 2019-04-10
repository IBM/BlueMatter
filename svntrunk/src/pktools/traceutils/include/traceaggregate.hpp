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
// File: traceaggregate.hpp
// Author: RSG
// Date: June 5, 2007
// Class: TraceAggregate
// Description: Class that encapsulates the operations required to
//              take raw trace data (from a .bin file) and for each
//              trace point, compute the distribution function of the
//              absolute times for each time step (iteration)
// *********************************************************************

#ifndef INCLUDE_TRACEAGGREGATE_HPP
#define INCLUDE_TRACEAGGREGATE_HPP

#include <pktools/selecttrace.hpp>

#include <vector>

class TraceAggregate
{
private:
  SelectTrace d_strc;
  int d_nodeCount;
public:
  int init(const char*, // initialize with name of .bin file to read
           const int,   // node count
           const int); // trace id to use for alignment/syncing of
                       // traces

  int aggregate(const int, // tracepoint to be aggregated
                std::vector<SelectTrace::ts_vect>&, // vector of bin boundaries
                std::vector<std::vector<unsigned> >&, // vector of
                                                      // dist. functions
                const int); // number of bins
};

#endif

