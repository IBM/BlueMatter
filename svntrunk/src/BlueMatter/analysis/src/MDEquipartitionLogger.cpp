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
// File: MDEquipartitionLogger.cpp
// Author: RSG
// Date: October 4, 2004
// Description: Program that scans rdg/srdg files and computes the
//              kinetic energy of groups of sites defined by a file
//              whose name is passed on the command line.  This file
//              has the form of a series of whitespace delimited
//              integers corresponding to the group of site ids
//              (zero-indexed) for which you wish to compute the
//              kinetic energy.  Each series will be delimited by one
//              or more lines beginning with a '#' character (all
//              these lines will be ignored)
//
// **********************************************************************

#include <iostream>
#include <cstdlib>

using namespace std ;

#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverEquipartitionMonitor.hpp>
#include <BlueMatter/DataReceiverSimpleLogger.hpp>
#include <BlueMatter/SiteGroupParser.hpp>

#include <iostream>
#include <fstream>

int
main(int argc, char **argv, char **envp)
{
    if (argc < 5)
      {
        std::cerr << argv[0]
                  << " inputFile(\"-\" takes input from stdin) dbName sysId groupingFile"
                  << std::endl;
        std::exit(-1);
      }

    char *fname = argv[1];
    const char* dbName = argv[2];
    int sysId = std::atoi(argv[3]);
    const char* groupFileName = argv[4];

    std::ifstream gFile(groupFileName);
    if (!gFile)
      {
	std::cerr << argv[0] << " unable to open file "
		  << groupFileName << " for input" << std::endl;
	std::exit(-1);
      }
    std::vector<std::vector<int> > siteList;
    SiteGroupParser::parse(gFile, siteList);

    std::cout << "# number of groups = " << siteList.size() << std::endl;
    for (int i = 0; i < siteList.size(); ++i)
      {
	std::cout << "# group[" << i << "] has " << siteList[i].size()
		  << " sites" << std::endl;
      }

    SimulationParser sp = SimulationParser(fname, 5, 0);

    DataReceiverEquipartitionMonitor DREquipartitionMonitor =
      DataReceiverEquipartitionMonitor(dbName, sysId, siteList);

    sp.addReceiver(&DREquipartitionMonitor);

    sp.init();

    sp.run();

    sp.final();

    return (0);
}
