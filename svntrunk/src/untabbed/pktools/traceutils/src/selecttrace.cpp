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
// File: selecttrace.cpp
// Author: RSG
// Date: April 6, 2004
// Class: SelectTrace
// Description: Encapsulates the classification, sorting, and
//              manipulation of trace point data (triples of trace_id,
//              node_id, and timestamp).
// **********************************************************************

#include <pktools/selecttrace.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <algorithm>

#include <sys/limits.h> // need LONGLONG_MAX and LONGLONG_MIN

SelectTrace::SelectTrace()
{
  d_trcPt_p = &d_trcPtStart;
}

SelectTrace::~SelectTrace()
{}

int SelectTrace::initFromDel(std::istream& trcData, int startSize)
{
  d_trcPt_p->erase(d_trcPt_p->begin(), d_trcPt_p->end()); // make sure we're empty
  // This input file MUST BE SORTED by trace id and node id (ascending).
  pclock_type ts;
  int nodeId;
  int traceId;
  TracePointKey newKey;

  int count = 0;
  std::string inLine;
  while(trcData)
    {
      std::getline(trcData, inLine, '\n');
      std::replace(inLine.begin(), inLine.end(), ',',' ');
      std::istringstream buf(inLine);
      buf >> traceId >> nodeId >> ts;
      //      std::cerr << nodeId << " " << traceId << " " << ts << std::endl;
      if (trcData)
        {
          ++count;
        }
      else
        {
          break;
        }

      newKey.d_nodeId = nodeId;
      newKey.d_traceId = traceId;
      trcpt_container_type::iterator iter = d_trcPt_p->find(newKey);
      if (iter == d_trcPt_p->end())
        {
          trcpt_container_type::value_type item(newKey, ts_vect());

          std::pair<trcpt_container_type::iterator, bool> ret =
            d_trcPt_p->insert(item);
          if (!ret.second)
            {
              std::cerr << "SelectTrace::init(): Invariant violated"
                        << std::endl;
              return(-1);
            }
          //          std::cerr << "New Key " << newKey << " inserted" << std::endl;
          iter = ret.first;
          iter->second.reserve(startSize);
        }
      iter->second.push_back(ts);
    }
  for (trcpt_container_type::iterator iter = d_trcPt_p->begin();
       iter != d_trcPt_p->end();
       ++iter)
    {
      std::sort(iter->second.begin(), iter->second.end());
    }
  return(count);
}

int SelectTrace::initFromBin(std::istream& is)
{
  d_trcPt_p->erase(d_trcPt_p->begin(), d_trcPt_p->end()); // make sure we're empty
  std::vector<int> offset;
  offset.reserve(32768);
  t_int cOffset;
  t_int trace_id;
  t_int node_id;
  TracePointKey newKey;
  ts_vect dummy;
  while(true)
    {
      is.read((char*)&trace_id, sizeof(t_int));
      is.read((char*)&node_id, sizeof(t_int));
      is.read((char*)&cOffset, sizeof(t_int));
      offset.push_back(cOffset);
      newKey.d_traceId = trace_id;
      newKey.d_nodeId = node_id;
      //      std::cerr << "newKey = " << newKey << " offset = " << cOffset << std::endl;
      if (newKey == TracePointKey::nullKey())
        {
          break;
        }
      trcpt_container_type::value_type val(newKey, dummy);
      if (!d_trcPt_p->insert(val).second)
        {
          std::cerr
            << "SelectTrace::initFromBin: Invariant violation, non-unique key "
            << newKey << std::endl;
          return(-1);
        }
    }
  std::cerr << d_trcPt_p->size() << " keys read" << std::endl;
  
  pclock_type timeStamp;
  std::vector<t_int>::const_iterator offIter = offset.begin();
  for (trcpt_container_type::iterator iter = d_trcPt_p->begin();
       iter != d_trcPt_p->end();
       ++iter)
    {
      if (!is)
        {
          std::cerr
            << "SelectTrace::initFromBin: Error reading from input stream"
            << std::endl;
        }
      int len = *(offIter+1) - (*offIter);
      //      std::cerr << len << " timestamps for key = " << iter->first << std::endl;
      for (int i = 0; i < len; ++i)
        {
          is.read((char*)&timeStamp, sizeof(pclock_type));
          iter->second.push_back(timeStamp);
        }
      ++offIter;
    }
  return(0);
}

int SelectTrace::initFromContainer(trcpt_container_type& trcPt)
{
  d_trcPt_p = &trcPt;
  return(d_trcPt_p->size());
}

int SelectTrace::putBin(std::ostream& os)
{
  // list of trace point keys and offsets will be terminated by an
  // entry with TracePointKey::nullKey()
  t_int offset = 0; // offsets will be measured in units of pclock_type
  for (trcpt_container_type::iterator iter = d_trcPt_p->begin();
       iter != d_trcPt_p->end();
       ++iter)
    {
      t_int trace_id = iter->first.d_traceId;
      t_int node_id = iter->first.d_nodeId;
      os.write((char*)&trace_id, sizeof(t_int));
      os.write((char*)&node_id, sizeof(t_int));
      os.write((char*)&offset, sizeof(t_int));
      offset += iter->second.size();
    }

  t_int trace_id = TracePointKey::nullKey().d_traceId;
  t_int node_id = TracePointKey::nullKey().d_nodeId;
  // write out this offset with the null key so that we can compute sizes
  os.write((char*)&trace_id, sizeof(t_int));
  os.write((char*)&node_id, sizeof(t_int));
  os.write((char*)&offset, sizeof(t_int));

  for (trcpt_container_type::iterator iter = d_trcPt_p->begin();
       iter != d_trcPt_p->end();
       ++iter)
    {
      for (ts_vect::const_iterator viter = iter->second.begin();
           viter != iter->second.end();
           ++viter)
        {
          os.write((char*)&(*viter), sizeof(pclock_type));
        }
    }
  
  return(offset);
}

int SelectTrace::putDel(std::ostream& os)
{
  int count = 0; 
  for (trcpt_container_type::iterator iter = d_trcPt_p->begin();
       iter != d_trcPt_p->end();
       ++iter)
    {
      for (ts_vect::const_iterator viter = iter->second.begin();
           viter != iter->second.end();
           ++viter)
        {
          os << iter->first.d_traceId << ", "
             << iter->first.d_nodeId << ", "
             << (*viter) << std::endl;
          ++count;
        }
    }
  return(count);
}

int SelectTrace::getIntervals(const TracePointKey& startKey,
                              const TracePointKey& finishKey,
                              SelectTrace::ts_vect& interval)
{
  trcpt_container_type::iterator iter1 = d_trcPt_p->find(startKey);
  trcpt_container_type::iterator iter2 = d_trcPt_p->find(finishKey);

  if ((iter1 == d_trcPt_p->end()) || (iter2 == d_trcPt_p->end()))
    {
      std::cerr << "SelectTrace::getIntervals(): key(s): " << startKey << " "
                << finishKey << " not found!" << std::endl;
      return(-1);
    }
  if (iter1->second.size() != iter2->second.size())
    {
      std::cerr << "SelectTrace::getIntervals(): lengths of timestamp vectors are unequal" << std::endl;
      return(-1);
    }

  interval.clear();
  interval.reserve(iter1->second.size());

  ts_vect::const_iterator iFinish = iter2->second.begin();
  for (ts_vect::const_iterator iStart = iter1->second.begin();
       iStart != iter1->second.end();
       ++iStart)
    {
      interval.push_back(*iFinish - *iStart);
      ++iFinish;
    }
  return(interval.size());
}

int SelectTrace::getIntervals(const TracePointKey& key,
                              SelectTrace::ts_vect& interval)
{
  trcpt_container_type::iterator iter = d_trcPt_p->find(key);

  if (iter == d_trcPt_p->end())
    {
      std::cerr << "SelectTrace::getIntervals(): key " << key
                << " not found!" << std::endl;
      return(-1);
    }

  interval.clear();
  interval.reserve(iter->second.size());

  for (ts_vect::const_iterator iFinish = iter->second.begin() + 1;
       iFinish != iter->second.end();
       ++iFinish)
    {
      ts_vect::const_iterator iStart = iFinish - 1;
      interval.push_back(*iFinish - *iStart);
    }
  
  return(interval.size());
}

// tracepoints cut by bracketKey are assumed to come before all
// other trace points in the current time step.
int SelectTrace::getGroupedIntervals(const TracePointKey& startKey,
                                     const TracePointKey& finishKey,
                                     const TracePointKey& bracketKey,
                                     ts_vect& interval)
{
  trcpt_container_type::iterator iter1 = d_trcPt_p->find(startKey);
  trcpt_container_type::iterator iter2 = d_trcPt_p->find(finishKey);
  trcpt_container_type::iterator iter3 = d_trcPt_p->find(bracketKey);

  if ((iter1 == d_trcPt_p->end()) ||
      (iter2 == d_trcPt_p->end()) ||
      (iter3 == d_trcPt_p->end()))
    {
      std::cerr << "SelectTrace::getGroupedIntervals(): key(s): " << startKey << " "
                << finishKey << " " << bracketKey << " not found!" << std::endl;
      return(-1);
    }
  if ((iter1->second.size() != iter2->second.size()))
    {
      std::cerr << "SelectTrace::getGroupedIntervals(): lengths of start/finish timestamp vectors are unequal" << std::endl;
      return(-1);
    }
  if ((iter1->second.size() < 3))
    {
      std::cerr << "SelectTrace::getGroupedIntervals(): start/finish timestamp vectors have length 0" << std::endl;
      return(-1);
    }

  interval.clear();
  interval.reserve(iter3->second.size());


  for (ts_vect::const_iterator iBracket = iter3->second.begin();
       iBracket != iter3->second.end();
       ++iBracket)
    {
      ts_vect::value_type accumulatedTime = 0;
      for (ts_vect::const_iterator iStart =
             std::lower_bound(iter1->second.begin(),
                              iter1->second.end(), *iBracket);
           ((iBracket +1) != iter3->second.end()) ? 
       (*iStart < *(iBracket+1)) && (iStart != iter1->second.end()) 
             : iStart != iter1->second.end();
           ++iStart)
        {
          ts_vect::const_iterator iFinish = 
            iter2->second.begin() + (iStart - iter1->second.begin());
          accumulatedTime += (*iFinish - *iStart);
        }
      interval.push_back(accumulatedTime);
    }
  return(interval.size());
}


std::pair<SelectTrace::pclock_type, SelectTrace::pclock_type>
SelectTrace::getRange(int bracketId,                 // bracketing trace id
                      int startStep,                 // starting time-step
                      int nSteps,                    // number of steps
                      int nodeCount)
{
  static std::pair<SelectTrace::pclock_type, SelectTrace::pclock_type>
    s_error(-1, -1);
  int endStep = startStep + nSteps;
  
  TracePointKey bracketKey;
  bracketKey.d_traceId = bracketId;
  pclock_type minT = LONGLONG_MAX;
  pclock_type maxT = LONGLONG_MIN;

  
  for (int i = 0; i < nodeCount; ++i)
    {
      bracketKey.d_nodeId = i;
      trcpt_container_type::const_iterator iter = d_trcPt_p->find(bracketKey);
      if (iter == d_trcPt_p->end())
        {
          std::cerr << "SelectTrace::getRange(): cannot find bracket key " << bracketKey << std::endl;
          return(s_error);
        }
      const SelectTrace::ts_vect& v = iter->second;
      minT = minT < v[startStep] ? minT : v[startStep];
      maxT = maxT > v[endStep] ? maxT : v[endStep];
    }
  std::pair<SelectTrace::pclock_type, SelectTrace::pclock_type>
    range(minT, maxT);
  return(range);
}
