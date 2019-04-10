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
// File: timestamp2groupedintervalstats.cpp
// Author: RSG
// Date: April 25, 2004
// Description: Executable to take binary format trace files from trcdel2bin
//              and a list of start/finish key pairs from stdin and
//              produce a set of statistics on the intervals (with
//              intervals occurring within the same time-step
//              accumulated together).  The time-steps are defined by
//              a trace point that is guaranteed to begin every time-step.
//
// Command Line Arguments: trcBinFile nodeCount
//                         minTimeStep bracketPointId 
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
#include <sys/time.h>

int main(int argc, char** argv)
{
  int nodeCount;
  int bracketTrcId;
  char* trcBinFile;
  int minTimeStep = 0;
  int skipLastN = 0;
  switch(argc)
    {
    case 6:
      bracketTrcId = std::atoi(argv[5]);
    case 5:
      skipLastN = std::atoi(argv[4]);
      //    case 4:
      minTimeStep = std::atoi(argv[3]);
      //    case 3:
      nodeCount = std::atoi(argv[2]);
      trcBinFile = argv[1];
      break;
    default:
      std::cerr << argv[0] << " trcBinFile nodeCount minTimeStep skipLastNSteps bracketId" << std::endl;
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

  TracePointKey startKey, finishKey, bracketKey;
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
      time_t startTrcTime = time(&tNull);
      label.push_back(tlabel);
      std::cerr << "Processing " << tlabel << std::endl;
      Stats<SelectTrace::pclock_type> stats;
      std::vector<Stats<SelectTrace::pclock_type> > statsByTs;
      startKey.d_traceId = startTrcId;
      finishKey.d_traceId = finishTrcId;
      bracketKey.d_traceId = bracketTrcId;
      for (int i = 0; i < nodeCount; ++i)
        {
          startKey.d_nodeId = i;
          finishKey.d_nodeId = i;
          bracketKey.d_nodeId = i;
          int rc =0;
	  time_t startStats = time(&tNull);

          if (startTrcId == finishTrcId)
            {
	      rc = strc.getIntervals(startKey, interval);
	      time_t finishStats = time(&tNull);
	      std::cerr << "getIntervals(" << tlabel << ", " 
			<< startTrcId << ", "
			<< i << ") finished in "
			<< finishStats - startStats
			<< " with rc = " << rc << std::endl;
              if (rc < 1)
                {
                  continue;
                }
            }
	  if (argc < 6)
	    {
	      rc = strc.getIntervals(startKey, finishKey, interval);
	      time_t finishStats = time(&tNull);
#if PRINT_STUFF
	      std::cerr << "getIntervals(" << tlabel << ", " 
			<< startTrcId << ", "
			<< finishTrcId << ", " << i << ") finished in "
			<< finishStats - startStats
			<< " with rc = " << rc << std::endl;
#endif
              if (rc < 1)
                {
                  continue;
                }
	    }
          else
            {
              rc = strc.getGroupedIntervals(startKey, 
                                            finishKey,
                                            bracketKey,
                                            interval);
	      time_t finishStats = time(&tNull);
#if PRINT_STUFF
	      std::cerr << "getGroupedIntervals(" << tlabel << ", " 
			<< startTrcId << ", "
			<< finishTrcId << ", " << i << ") finished in "
			<< finishStats - startStats
			<< " with rc = " << rc << std::endl;
#endif
              if (rc < 1)
                {
                  continue;
                }
            }
          stats = 
            std::for_each(interval.begin()+minTimeStep,
                          interval.end()-skipLastN,
                          stats);

          if (statsByTs.size() < interval.size())
            {
              statsByTs.resize(interval.size());
            }

          for (int i = 0; i < interval.size(); ++i)
            {
              statsByTs[i](interval[i]);
            }
	  time_t finishStats = time(&tNull);
	  std::cerr << "StatsByTs(" << startTrcId << ", "
		    << finishTrcId << ", " << i << ") finished in "
		    << finishStats - startStats << std::endl;
	  
        } // end iteration over node count
      double minDev = 0;
      double maxDev = 0;
#if defined(PRINT_STUFF)
      std::cout << "startTrcId = " << startTrcId << std::endl;
      std::cout << "finishTrcId = " << finishTrcId << std::endl;
      std::cout << "ts" << " \t"
                << "avg" << " \t"
                << "stddev" << " \t"
                << "min" << " \t"
                << "max" << " \t"
                << "count" << std::endl;

#endif
      for (int i = 0; i < statsByTs.size(); ++i)
        {
#if defined(PRINT_STUFF)
          std::cout << i << " \t"
                    << statsByTs[i].avg() << " \t"
                    << statsByTs[i].stddev() << " \t"
                    << statsByTs[i].min() << " \t"
                    << statsByTs[i].max() << " \t"
                    << statsByTs[i].count()
                    << std::endl;
#endif
          double dev = statsByTs[i].min() - statsByTs[i].avg();
          minDev = minDev <= dev ? minDev : dev;
          dev = statsByTs[i].max() - statsByTs[i].avg();
          maxDev = maxDev >= dev ? maxDev : dev;
        }
      
      std::cout << nodeCount << " "
                << stats.avg() << " "
                << stats.stddev() << " "
                << stats.min() << " "
                << stats.max() << " "
                << minDev << " "
                << maxDev << " "
                << stats.count() << " "
                << tlabel
                << std::endl;
      time_t finishTrcTime = time(&tNull);
      std::cerr << "Finished processing " << tlabel << " in "
		<< finishTrcTime - startTrcTime << " seconds " << std::endl;
    } // end loop over while(std::cin)
  return(0);
}
