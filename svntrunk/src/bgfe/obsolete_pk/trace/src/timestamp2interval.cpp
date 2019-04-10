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
// File: timestamp2interval.cpp
// Author: RSG
// Date: April 6, 2004
// Description: Executable to take "converted" del files (commas -> spaces)
//              from fxlog2del (dataFile.del) and produce another del file
//              that can be imported into the trc_interval table
//
// Command Line Arguments: trace_set_id dataDelFile nodeCount
//                         start/finish id pairs are passed via stdin
//                         output goes to stdout
// ******************************************************************

#include <pk/selecttrace.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char** argv)
{
  int traceSetId;
  int nodeCount;
  char* dataDelFile;

  switch(argc)
    {
    case 4:
      nodeCount = std::atoi(argv[3]);
      traceSetId = std::atoi(argv[1]);
      dataDelFile = argv[2];
      break;
    default:
      std::cerr << argv[0] << " trace_set_id dataDelFile nodeCount" << std::endl;
      std::exit(-1);
    }

  std::ifstream dataDel(dataDelFile);
  if (!dataDel)
    {
      std::cerr << "Error opening file " << dataDelFile << std::endl;
      std::exit(-1);
    }
  SelectTrace strc;
  int count = strc.initFromDel(dataDel, 140);
  std::cerr << count << " items registered " << std::endl;

  int startTrcId;
  int finishTrcId;

  TracePointKey startKey, finishKey;
  SelectTrace::ts_vect interval;
  interval.reserve(140);

  while(std::cin)
    {
      std::cin >> startTrcId >> finishTrcId;
      if (!std::cin)
        {
          break;
        }

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
          int ts = 0;
          for (SelectTrace::ts_vect::const_iterator iter = interval.begin();
               iter != interval.end();
               ++iter)
            {
              std::cout << traceSetId << ", "     // trace_set_id
                        << startTrcId << ", "     // start_id
                        << finishTrcId << ", "    // finish_id
                        << i << ", "              // node_id
                        << ts << ", "             // time_step
                        << *iter << std::endl;    // elapsed
              ++ts;
            }
        }
    }
  return(0);
}
