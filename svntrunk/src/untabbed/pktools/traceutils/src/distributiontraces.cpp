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
// File: distributiontraces.cpp
// Author: RSG
// Date: June 6, 2007
// Description: Application that takes raw trace data (from a .bin
//              file) and for each trace point provided from stdin,
//              compute the distribution function of the 
//              absolute times for each time step (iteration).
//              A synchronization trace point may be specified to
//              align the clocks.
// *********************************************************************



#include <pktools/traceaggregate.hpp>
#include <pktools/selecttrace.hpp>
#include <pk/stats.hpp>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

void printHeader();
void printData(const std::vector<SelectTrace::ts_vect>&, // startBin boundaries
               const std::vector<std::vector<unsigned> >&, // startDist
               const std::vector<SelectTrace::ts_vect>&,    // finishBin
               const std::vector<std::vector<unsigned> >&); // finishDist

int main(int argc, char** argv)
{
  bool verbose = false;
  int nodeCount;
  char* trcBinFile;
  int syncId = -1;
  int nBins = 10;
  switch(argc)
    {
    case 5:
      nBins = std::atoi(argv[4]);
    case 4:
      syncId = std::atoi(argv[3]);
    case 3:
      nodeCount = std::atoi(argv[2]);
      trcBinFile = argv[1];
      break;
    default:
      std::cerr << argv[0] << " trcBinFile nodeCount <syncId=-1 (none)> <nBins=10>" << std::endl;
      std::exit(-1);
    }


  time_t tNull;
  time_t startInit = time(&tNull);
  
  TraceAggregate ta;
  int rc = ta.init(trcBinFile, nodeCount, syncId);
  if (rc == -1)
    {
      std::cerr << "Error opening binary trace file " << trcBinFile
                << " for input" << std::endl;
      std::exit(-1);
    }

  time_t finishInit = time(&tNull);
  std::cerr << "Initialization from << " << trcBinFile << " in "
            << finishInit - startInit
      << " seconds" << std::endl;

  int startTrcId;
  int finishTrcId;

  std::vector<std::string> label;
  std::string tlabel;
  
  std::vector<std::vector<unsigned> > startDist;
  std::vector<std::vector<unsigned> > finishDist;
  std::vector<SelectTrace::ts_vect> startBins;
  std::vector<SelectTrace::ts_vect> finishBins;

  int trcCount = 0;
  printHeader();
  while(std::cin)
    {
      std::cin >> startTrcId >> finishTrcId >> tlabel;
      if (!std::cin)
        {
          break;
        }
      time_t startProc = time(&tNull);
      label.push_back(tlabel);
      std::cerr << "Processing " << tlabel << std::endl;
      if (startTrcId == finishTrcId) // we skip it
        {
          continue;
        }
      else // compute distributions for start and finish trace points
        {
          startDist.clear();
          startBins.clear();
          finishDist.clear();
          finishBins.clear();
          // start trace point
          int ts1 = ta.aggregate(startTrcId, startBins, startDist, nBins);
          if (ts1 < 0)
            {
              continue;
            }
          int ts2 = ta.aggregate(finishTrcId, finishBins, finishDist, nBins);
          if (ts2 < 0)
            {
              continue;
            }
          if (ts1 != ts2)
            {
              std::cerr << "Start/Finish Pairs for " << tlabel
                        << " do not match up" << std::endl;
              continue;
            }
          std::cout << "# Index " << trcCount << " " << tlabel
                    << " Start/Finish Trace IDs = " << startTrcId 
                    << " / " << finishTrcId << std::endl;
          printData(startBins, startDist, finishBins, finishDist);
          ++trcCount;
        }
    } // end while(std::cin)
  std::cerr << trcCount << "start/finish trace pairs processed"
            << tlabel;
  return(0);
}


void printHeader()
{
  std::cout << "# Column 1: Time Step (ordinal)\n";
  std::cout << "# Column 2: Time Stamp (nanoseconds) to go with Column 3\n";
  std::cout << "# Column 3: Distribution Function for start tracepoint, 1 - F(x) for stop tracepoint\n";
  std::cout << "# Column 4: Time Stamp (nanoseconds) to go with Column 5\n";
  std::cout << "# Column 5: Distribution Function for start/stop tracepoints\n";
}

void printData(const std::vector<SelectTrace::ts_vect>& startBin,
               const std::vector<std::vector<unsigned> >& startDist,
               const std::vector<SelectTrace::ts_vect>& finishBin,
               const std::vector<std::vector<unsigned> >& finishDist)
{
  for (int i = 0; i < startBin.size(); ++i)
    {
      for (int j = 0; j < startBin[i].size(); ++j)
        {
          std::cout << std::setw(6) << i 
                    << std::setw(20) << startBin[i][j]
                    << std::setw(20) << static_cast<double>(startDist[i][j])/static_cast<double>(*startDist[i].rbegin())
                    << std::setw(20) << startBin[i][j]
                    << std::setw(20) << static_cast<double>(startDist[i][j])/static_cast<double>(*startDist[i].rbegin())
                    << std::endl;
        }
      for (int j = 0; j < finishBin[i].size(); ++j)
        {
          std::cout << std::setw(6) << i 
                    << std::setw(20) << finishBin[i][j]
                    << std::setw(20) << 1.0 - static_cast<double>(finishDist[i][j])/static_cast<double>(*finishDist[i].rbegin())
                    << std::setw(20) << finishBin[i][finishBin[i].size() - j - 1]
                    << std::setw(20) << static_cast<double>(finishDist[i][finishBin[i].size() - j - 1])/static_cast<double>(*finishDist[i].rbegin())
                    << std::endl;
        }
    }
  std::cout << "\n" << std::endl;
}

