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
// File: timestamp2mp.cpp
// Author: RSG
// Date: October 29, 2004
// Description: Executable to take binary format trace files from trcdel2bin
//              and a list of start/finish key pairs from stdin and
//              produce a metapost format drawing showing the 
//              the timeline for a selected range of timesteps.
//              The time-steps are defined by
//              a trace point that is guaranteed to begin every time-step.
//
// Command Line Arguments: trcBinFile nodeCount
//                         minTimeStep nSteps bracketPointId 
//                         start/finish id pairs are
//                         passed via stdin 
//                         output goes to stdout
// ******************************************************************

#include <pktools/selecttrace.hpp>
#include <pk/stats.hpp>
#include <pk/reduce.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <algorithm>

int main(int argc, char** argv)
{
  int nodeCount;
  int bracketTrcId;
  char* trcBinFile;
  int minTimeStep = 0;
  int nSteps = 1;
  switch(argc)
    {
    case 6:
      bracketTrcId = std::atoi(argv[5]);
      //    case 5:
      nSteps = std::atoi(argv[4]);
      //    case 4:
      minTimeStep = std::atoi(argv[3]);
      //    case 3:
      nodeCount = std::atoi(argv[2]);
      trcBinFile = argv[1];
      break;
    default:
      std::cerr << argv[0] << " trcBinFile nodeCount minTimeStep bracketId" << std::endl;
      std::exit(-1);
    }

  SelectTrace strc;
  std::ifstream trcBin(trcBinFile);
  if (!trcBin)
    {
      std::cerr << "Error opening binary trace file " << trcBinFile
                << " for input" << std::endl;
      std::exit(-1);
    }
  int count = strc.initFromBin(trcBin);

  int startTrcId;
  int finishTrcId;

  TracePointKey startKey, finishKey, bracketKey;
  SelectTrace::ts_vect interval;
  std::vector<std::string> label;
  std::string tlabel;
  interval.reserve(140);
  
  enum {MPOST_RANGE = 512};
  enum {COLOR_COUNT = 72};

  std::pair<SelectTrace::pclock_type, SelectTrace::pclock_type> range
    = strc.getRange(bracketTrcId, minTimeStep, nSteps, nodeCount);

  std::cerr << "range: " << range.first << "  " << range.second << std::endl;
  std::cerr << "% range: " << range.first << "  " << range.second << std::endl;

  double conv = 0.5*(double)MPOST_RANGE/(double)(range.second - range.first);

  int traceIndex = 0;
  while(std::cin)
    {
      std::cin >> startTrcId >> finishTrcId >> tlabel;
      if (!std::cin)
        {
          break;
        }

      label.push_back(tlabel);
      std::cerr << "Processing " << tlabel << std::endl;
      std::cout << "% drawing " << tlabel << std::endl;
      startKey.d_traceId = startTrcId;
      finishKey.d_traceId = finishTrcId;
      bracketKey.d_traceId = bracketTrcId;
      for (int i = 0; i < nodeCount; ++i)
        {
          startKey.d_nodeId = i;
          finishKey.d_nodeId = i;
          bracketKey.d_nodeId = i;
          int rc =0;
          if (startTrcId == finishTrcId)
            {
              std::cerr << "startTrCId must not be equal to finishTrcId for this routine" << std::endl;
              continue;
            }
          else
            {
              SelectTrace::trcpt_container_type::const_iterator iter1 =
                strc.traces().find(startKey);
              SelectTrace::trcpt_container_type::const_iterator iter2 =
                strc.traces().find(finishKey);
              if ((iter1 == strc.traces().end()) || (iter2 == strc.traces().end()))
                {
                  std::cerr << "Cannot find one of startKey = "
                            << startKey << " or finishKey = "
                            << finishKey << std::endl;
                  continue;
                }

              const SelectTrace::ts_vect& v1 = iter1->second;
              const SelectTrace::ts_vect& v2 = iter2->second;
              
              SelectTrace::ts_vect::const_iterator firstStart =
                std::lower_bound(v1.begin(), v1.end(), range.first);

              SelectTrace::ts_vect::const_iterator firstFinish =
                std::lower_bound(v2.begin(), v2.end(), *firstStart);

              if ((firstStart == v1.end()) || (firstFinish == v2.end()))
                {
                  std::cerr << "There seems to be a mismatch between "
                            << startKey << " and " << finishKey << std::endl;
                  continue;
                }

              SelectTrace::ts_vect::const_iterator i1 = firstStart;
              for (SelectTrace::ts_vect::const_iterator i2 = firstFinish;
                   (i1 != v1.end()) && (i2 != v2.end()) &&
                   (*i1 <= range.second) && (*i2 <= range.second);
                    ++i1, ++i2)
                {
                  double t1 = (*i1 - range.first)*conv;
                  std::cout << "draw (" << t1  << ", " << i << ")--";
                  double t2 = (*i2 - range.first)*conv;
                  std::cout << "(" << t2  << ", " << i << ") ";
                  std::cout << "withcolor col"
                            << ((traceIndex % COLOR_COUNT) + 1)
                            << ";" << std::endl;
                }
            }
        } // end of loop over node count
      ++traceIndex;
    } // end of while(std::cin)
  // put out a label
  int traceOffset = nodeCount;
  std::cout << "numeric a;" << std::endl;
  std::cout << "a := (fontsize defaultfont)*1.1;" << std::endl;
  
  for (int i = 0; i < traceIndex; ++i)
    {
      int ioffset = i+2;
      std::cout << "draw (0, " 
                << traceOffset << " + a*" << ioffset << ")--"
                << "(" << MPOST_RANGE/16 << ", "
                << traceOffset << " + a*" << ioffset 
                << ") withcolor col"
                            << ((i % COLOR_COUNT) + 1)
                            << ";" << std::endl;

      std::cout << "label.rt(\"" << label[i] << "\", ("
                << (5*MPOST_RANGE)/64 << ", "
                << traceOffset << " + a*" << ioffset << "));" << std::endl;
    }
  return(0);
}
