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
// File: selecttrace.hpp
// Author: RSG
// Date: April 6, 2004
// Class: SelectTrace
// Description: Encapsulates the classification, sorting, and
//              manipulation of trace point data (triples of trace_id,
//              node_id, and timestamp).
// **********************************************************************
#ifndef INCLUDE_SELECTTRACE_HPP
#define INCLUDE_SELECTTRACE_HPP

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

struct TracePointKey
{
  int d_traceId;
  int d_nodeId;
  inline TracePointKey(int = -1, int= -1); // trace_id, node_id
  inline TracePointKey& operator=(const TracePointKey&);
  inline bool operator==(const TracePointKey&) const;
  inline bool operator!=(const TracePointKey&) const;
  inline static const TracePointKey& nullKey();
};

struct TracePointCompare
{
  inline bool operator()(const TracePointKey&, const TracePointKey&) const;
};

class SelectTrace
{
public:
  typedef long long pclock_type;
  typedef std::vector<SelectTrace::pclock_type> ts_vect;
  typedef std::map<TracePointKey, ts_vect, TracePointCompare> trcpt_container_type;

private:
  typedef int t_int;
  trcpt_container_type* d_trcPt_p;
  trcpt_container_type d_trcPtStart;;

public:
  SelectTrace();
  ~SelectTrace();
  int initFromDel(std::istream&, int = 64); // from sorted DEL file
  int initFromBin(std::istream&); // from binary format stream
  int initFromContainer(trcpt_container_type&);
  int putBin(std::ostream&); // put out binary format file
  int putDel(std::ostream&); // put out Del format file
  int getIntervals(const TracePointKey&, // "Start" tracepoint
                   const TracePointKey&, // "Finis" tracepoint
                   ts_vect&); // result
  int getIntervals(const TracePointKey&, // tracepoint to use for ts meas.
                   ts_vect&); // result
  int getGroupedIntervals(const TracePointKey&, // "Start" tracepoint
                          const TracePointKey&, // "Finis" tracepoint
                          const TracePointKey&, // "Bracket"
                                                // tracepoint which is
                                                // guaranteed to come
                                                // before all other
                                                // tracepoints in a
                                                // given ts.
                          ts_vect&); // result

  std::pair<pclock_type, pclock_type>
  getRange(int,                  // bracketId
           int,                  // starting time-step
           int,                  // number of steps
           int);                 // nodeCount

  inline const trcpt_container_type& traces() const;
  inline trcpt_container_type& traces();
};

TracePointKey::TracePointKey(int t, int n) : d_traceId(t), d_nodeId(n)
{}

TracePointKey& TracePointKey::operator=(const TracePointKey&other)
{
  d_traceId = other.d_traceId;
  d_nodeId = other.d_nodeId;
  return(*this);
}

bool TracePointKey::operator==(const TracePointKey&other) const
{
  return((d_traceId == other.d_traceId) && (d_nodeId == other.d_nodeId));
}

bool TracePointKey::operator!=(const TracePointKey&other) const
{
  return((d_nodeId != other.d_nodeId) || (d_traceId != other.d_traceId));
}

const TracePointKey& TracePointKey::nullKey()
{
  static TracePointKey s_null;
  return(s_null);
}

bool TracePointCompare::operator()(const TracePointKey& k1,
                                   const TracePointKey& k2) const
{
  if (k1.d_traceId < k2.d_traceId)
    {
      return true;
    }
  return ((k1.d_traceId == k2.d_traceId) && (k1.d_nodeId < k2.d_nodeId));
}

const SelectTrace::trcpt_container_type& SelectTrace::traces() const
{
  return *d_trcPt_p;
}

SelectTrace::trcpt_container_type& SelectTrace::traces()
{
  return *d_trcPt_p;
}

inline std::ostream& operator<<(std::ostream& os, const TracePointKey& key)
{
  os << key.d_traceId << " "
     << key.d_nodeId;
  return os;
}
#endif
