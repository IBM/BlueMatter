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
// File: timestamp2intervalstats.cpp
// Author: RSG
// Date: April 10, 2004
// Description: Executable to take binary format trace files from trcdel2bin
//              and a list of start/finish key pairs from stdin and produce
//              a set of statistics on the intervals between corresponding
//              start/finish timestamps.
//
// Command Line Arguments: trace_set_id trcBinFile nodeCount minTimeStep
//                         start/finish id pairs are passed via stdin
//                         output goes to stdout
// ******************************************************************

#include <pktools/selecttrace.hpp>
#include <pk/stats.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <algorithm>

int main(int argc, char** argv)
{
  int traceSetId;
  int nodeCount;
  char* trcBinFile;
  int minTimeStep = 0;
  switch(argc)
    {
    case 4:
      minTimeStep = std::atoi(argv[3]);
    case 3:
      nodeCount = std::atoi(argv[2]);
      trcBinFile = argv[1];
      break;
    default:
      std::cerr << argv[0] << " trcBinFile nodeCount" << std::endl;
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

  TracePointKey startKey, finishKey;
  SelectTrace::ts_vect interval;
  std::vector<std::string> label;
  std::string tlabel;
  interval.reserve(140);

  while(std::cin)
    {
      std::cin >> startTrcId >> finishTrcId >> tlabel;
      if (!std::cin)
        {
          break;
        }

      label.push_back(tlabel);
      std::cerr << "Processing " << tlabel << std::endl;
      Stats<SelectTrace::pclock_type> stats;
      startKey.d_traceId = startTrcId;
      finishKey.d_traceId = finishTrcId;
      for (int i = 0; i < nodeCount; ++i)
        {
          startKey.d_nodeId = i;
          finishKey.d_nodeId = i;
          int rc =0;
          if (startTrcId == finishTrcId)
            {
              rc = strc.getIntervals(startKey, interval);
            }
          else
            {
              rc = strc.getIntervals(startKey, finishKey, interval);
            }
          stats = 
            std::for_each(interval.begin()+minTimeStep, interval.end(), stats);
        }
      std::cout << nodeCount << " "
                << stats.avg() << " "
                << stats.stddev() << " "
                << stats.min() << " "
                << stats.max() << " "
                << stats.count() << " "
                << tlabel
                << std::endl;
    }
  return(0);
}
