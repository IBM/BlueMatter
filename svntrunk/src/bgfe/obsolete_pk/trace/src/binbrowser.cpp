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
// File: binbrowser.cpp
// Author: RSG
// Date: April , 2004
// Description: Executable to browse binary format trace files from trcdel2bin
//
// Command Line Arguments: binFileName Initially it returns the
// contents of the directory of the bin file.  Subsequently, it takes
// trace id, node id pairs as input and returns the vector of time stamps
// ******************************************************************

#include <pk/selecttrace.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iomanip>

int main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cerr << argv[0] << " trcBinFile" << std::endl;
      return(-1);
    }

  const char* trcBinFile = argv[1];
  SelectTrace strc;
  std::ifstream trcBin(trcBinFile);
  if (!trcBin)
    {
      std::cerr << "Error opening binary trace file " << trcBinFile
                << " for input" << std::endl;
      std::exit(-1);
    }
  int count = strc.initFromBin(trcBin);
  int trcId = -1;
  int node = -1;
  TracePointKey key;

  std::cout << std::setw(8) << "traceId" << " "
	    << std::setw(8) << "nodeId" << " "
	    << std::setw(8) << "size" << std::endl;
  for (SelectTrace::trcpt_container_type::const_iterator iter = strc.traces().begin();
       iter != strc.traces().end();
       ++iter)
    {
      std::cout << std::setw(8) << iter->first.d_traceId << " "
		<< std::setw(8) << iter->first.d_nodeId << " "
		<< std::setw(8) << iter->second.size() << std::endl;
    }

  while(std::cin)
    {
      std::cerr << "Type trcId node" << std::endl;
      std::cin >> trcId >> node;
      if (!std::cin)
        {
          break;
        }
      std::cout << "TrcId: " << trcId << " Node: " << node << std::endl;
      key.d_traceId = trcId;
      key.d_nodeId = node;
      SelectTrace::trcpt_container_type::const_iterator iter = 
	strc.traces().find(key);

      if (iter == strc.traces().end())
	{
	  std::cerr << "key " << key
		    << " not found!" << std::endl;
	  continue;
	}
      int ts = 0;
      for (SelectTrace::trcpt_container_type::mapped_type::const_iterator it = 
	     iter->second.begin();
	   it != iter->second.end();
	   ++it)
	{
	  std::cout << std::setw(8) << ts << " "
		    << std::setw(16) << *it << std::endl;
	  ++ts;
	}
    }

  return(0);
}
