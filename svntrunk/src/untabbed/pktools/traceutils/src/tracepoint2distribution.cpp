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
// File: tracepoint2distribution.cpp
// Author: RSG
// Date: November 11, 2006
// Description: Executable to take binary format trace files from trcdel2bin
//              and a list of start/finish key pairs (and trace point
//              label) from stdin and produce a distribution function
//              of the intervals for a specified time-step
//
// Command Line Arguments: trcBinFile nodeCount time-step
//                         
//                         start/finish id pairs are
//                         passed via stdin 
//                         output goes to stdout
// ******************************************************************


#include <pktools/selecttrace.hpp>
#include <pk/stats.hpp>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

int main(int argc, char** argv)
{
  bool verbose = false;
  int nodeCount;
  char* trcBinFile;
  int timeStep = 0;
  int nBins = 10;
  switch(argc)
    {
    case 5:
      nBins = std::atoi(argv[4]);
    case 4:
      timeStep = std::atoi(argv[3]);
      nodeCount = std::atoi(argv[2]);
      trcBinFile = argv[1];
      break;
    default:
      std::cerr << argv[0] << " trcBinFile nodeCount timeStep <nBins=10>" << std::endl;
      std::exit(-1);
    }


  time_t tNull;
  time_t startLoad = time(&tNull);
  std::cerr << "Start to load " << trcBinFile 
      << " at t = " << startLoad << std::endl;
  
  SelectTrace strc;
  std::ifstream trcBin(trcBinFile);
  if (!trcBin)
    {
      std::cerr << "Error opening binary trace file " << trcBinFile
                << " for input" << std::endl;
      std::exit(-1);
    }
  int count = strc.initFromBin(trcBin);
  time_t finishLoad = time(&tNull);
  std::cerr << "Finished loading " << trcBinFile 
      << " at t = " << finishLoad << std::endl;
  std::cerr << "Loaded << " << trcBinFile << " in " << finishLoad - startLoad
      << " seconds" << std::endl;

  int startTrcId;
  int finishTrcId;

  TracePointKey startKey, finishKey;
  SelectTrace::ts_vect interval;
  interval.reserve(nodeCount);

  std::vector<std::string> label;
  std::string tlabel;
  
  std::vector<unsigned> dist;
  SelectTrace::ts_vect percentile;

  while(std::cin)
    {
      std::cin >> startTrcId >> finishTrcId >> tlabel;
      if (!std::cin)
        {
          break;
        }
      time_t startTrcTime = time(&tNull);
      label.push_back(tlabel);
      std::cerr << "Processing " << tlabel << std::endl;
      startKey.d_traceId = startTrcId;
      finishKey.d_traceId = finishTrcId;
      interval.clear();
      percentile.clear();
      dist.clear();
      int processed = 0;
      for (int i = 0; i < nodeCount; ++i)
        {
          startKey.d_nodeId = i;
          finishKey.d_nodeId = i;
          int rc =0;
          if (startTrcId == finishTrcId) // insert absolute time
            {
        SelectTrace::trcpt_container_type::const_iterator iter1 = strc.traces().find(startKey);
        if (iter1 != strc.traces().end())
    {
      interval.push_back(iter1->second[timeStep]);
      ++processed;
    }
        else
    {
      if (verbose)
        {
          std::cerr << "trace point for trace ID: " << startTrcId 
        << " labeled " << tlabel
        << " not found at time step " << timeStep << std::endl;
        }
    }
            }
    else // insert interval between start and finish
      {
        SelectTrace::trcpt_container_type::const_iterator iter1 = 
    strc.traces().find(startKey);
        SelectTrace::trcpt_container_type::const_iterator iter2 = 
    strc.traces().find(finishKey);
        if (iter1 == strc.traces().end())
    {
      if (verbose)
        {
          std::cerr << "trace point for start trace ID: " 
        << startTrcId 
        << " labeled " << tlabel
        << " not found at time step " << timeStep 
        << std::endl;
        }
      continue;
    }
        if (iter2 == strc.traces().end())
    {
      if (verbose)
        {
          std::cerr << "trace point for finish trace ID: " 
        << startTrcId 
        << " labeled " << tlabel
        << " not found at time step " << timeStep 
        << std::endl;
        }
      continue;
    }
        interval.push_back(iter2->second[timeStep] - 
         iter1->second[timeStep]);
        ++processed;
      }
  } // end of for loop over node ids
      std::cerr << processed << " records processed for trace "
    << tlabel;

      Stats<SelectTrace::ts_vect::value_type>::distribution(interval,
                  percentile,
                  dist,
                  nBins);
      // column 1: value
      // column 2: number of counts <= to column 1 value
      // column 3: number of counts <= to column 1 value and >
      // previous column 1 value
      // column 4: fraction of counts <= to column 1 value
      // column 5: fraction of counts <= to column 1 value and
      // > previous column 1 value
      // column 6: time-step
      // column 7: label

      double invCount = 1.0/static_cast<double>(interval.size());
      int prevDist = 0;
      for (int i = 0; i < percentile.size(); ++i)
  {
    std::cout << std::setw(20) << percentile[i] << " "
        << std::setw(12) << dist[i] << " "
        << std::setw(12) << dist[i] - prevDist << " "
        << std::setw(15) 
        <<static_cast<double>(dist[i])*invCount << " "
        << std::setw(15) 
        << static_cast<double>(dist[i] - prevDist)*invCount << " "
        << std::setw(8) << timeStep << " "
        << tlabel << std::endl;
    prevDist = dist[i];
  }
    } // end loop: while(std::cin)
  return(0);
}
