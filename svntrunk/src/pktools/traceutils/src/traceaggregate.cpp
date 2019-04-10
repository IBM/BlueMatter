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
// File: traceaggregate.cpp
// Author: RSG
// Date: June 5, 2007
// Class: TraceAggregate
// Description: Class that encapsulates the operations required to
//              take raw trace data (from a .bin file) and for each
//              trace point, compute the distribution function of the
//              absolute times for each time step (iteration)
// *********************************************************************

#include <pktools/traceaggregate.hpp>
#include <pk/stats.hpp>
#include <fstream>

int TraceAggregate::init(const char* binFileName,
                         const int nodeCount,
                         const int syncId)
{
  d_nodeCount = nodeCount;
  std::ifstream trcBin(binFileName);
  if (!trcBin)
    {
      return(-1);
    }
  int count = d_strc.initFromBin(trcBin);

  // synchronize clocks in the traces based on syncId
  if (syncId >= 0)
    {
      std::vector<SelectTrace::pclock_type> syncOffset(nodeCount);
      TracePointKey syncKey(syncId);
      for (int i = 0; i < d_nodeCount; ++i)
        {
          syncKey.d_nodeId = i;
          SelectTrace::trcpt_container_type::const_iterator syncIter = 
            d_strc.traces().find(syncKey);
          if (syncIter == d_strc.traces().end())
            {
              std::cerr << "TraceAggregate::init() Error! Unable to find synchronization id " << syncKey << std::endl;
              return(-2);
            }
          syncOffset[i] = syncIter->second[0];
        }

      for (SelectTrace::trcpt_container_type::iterator iter = 
             d_strc.traces().begin();
           iter != d_strc.traces().end();
           ++iter)
        {
          for (SelectTrace::ts_vect::iterator tsIter = iter->second.begin();
               tsIter != iter->second.end();
               ++tsIter)
            {
              *tsIter -= syncOffset[iter->first.d_nodeId];
            }
        }

    } // end if (syncId >= 0)
  return(count);
}

int TraceAggregate::aggregate(const int traceId,
                              std::vector<SelectTrace::ts_vect>& bin,
                              std::vector<std::vector<unsigned> >& dist,
                              const int nBins)
{
  TracePointKey traceKey(traceId);
  SelectTrace::ts_vect working;
  int ts = 0;
  while(ts >= 0)
    {
      working.reserve(d_nodeCount);
      working.clear();

      for (int i = 0; i < d_nodeCount; ++i)
        {
          traceKey.d_nodeId = i;
          SelectTrace::trcpt_container_type::const_iterator traceIter = 
            d_strc.traces().find(traceKey);

          if (traceIter == d_strc.traces().end())
            {
              std::cerr << "TraceAggregate::aggregate() Error! Unable to find trace id " << traceKey << std::endl;
              return(-1);
            }

          if (ts == 0) // reserve space for bin and dist
            {
              bin.reserve(bin.size() > traceIter->second.size() ?
                          bin.size() : traceIter->second.size());
              dist.reserve(dist.size() > traceIter->second.size() ?
                           dist.size() : traceIter->second.size());
            }

          if (ts >= traceIter->second.size())
            {
              ts = -1;
              break;
            }
          working.push_back(traceIter->second[ts]);
          if (i == 0)
            {
              bin.resize(traceIter->second.size());
              dist.resize(traceIter->second.size());
            }
        } // end of iteration over nodes i
 
      if (ts < 0)
        {
          break;
        }
      else
        {
          Stats<SelectTrace::ts_vect::value_type >::distribution(working,
                                                                 bin[ts],
                                                                 dist[ts],
                                                                 nBins);
          ++ts;
        }
    } // end while (ts >= 0)
  return(dist.size());
}

