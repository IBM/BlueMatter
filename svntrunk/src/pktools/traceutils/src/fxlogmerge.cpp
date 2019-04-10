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
 // *************************************************************************
//
// DEL file output formats:
// For id files:
// traceID, node_id, traceType (0=Start, 1=Finis), traceLabel, traceShortTag, traceBaseTag,  traceDescription
//
// For data files:
// traceID, nodeID, timeStamp
//
// *************************************************************************

#include <assert.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cstdlib>
#include <sys/stat.h>

#include <pktools/trcutil.hpp>
#include <pktools/selecttrace.hpp>

int main( int argc, char **argv, char **)
{
  unsigned long RunMgz = 1000 ;  // allow stating the machine clock rate in mgz
  char* inFile = NULL;
  char* outFile = NULL;
  std::ofstream os;
  
  switch(argc)
    {
    default:
      outFile = argv[1];
      break;
    case 2:
    case 1:
      std::cerr << argv[0] << " outFileName inFileName <inFileName> ... <inFileName>" << std::endl;
      std::exit(-1);
    }
  os.open( outFile );
  if ( !os )
    {
      std::cerr << "Unable to open file "
                << outFile
                << " for output"
                << std::endl;
      exit( -1 );
    }
#if defined(PRINT_STUFF)
  std::cout << "argc = " << argc << std::endl;
  for ( int i = 0; i < argc; ++i )
    {
      std::cout << "argv[ " << i << " ] = " << argv[i] << std::endl;
    }
  std::cout.flush();
#endif
  int idOffset = 0;
  for ( int i = 2; i < argc; ++i )
    {
      inFile = argv[i];
      trc::TracePointCollection::const_iterator iter;
      trc::TracePointCollection coll;
      coll.init(inFile, RunMgz, idOffset, os ); 
      int iCount = 0;
      for (iter = coll.begin();
           iter != coll.end();
           ++iter)
        {
          ++iCount;
#if defined(PRINT_STUFF)
          std::cout << "Processing line " 
                    << iCount 
                    << " with traceOffset() = "
                    << iter.traceOffset()
                    << " and renumberTraceId() = "
                    << iter.renumberTraceId()
                    << std::endl;
#endif        
        }
      std::cout << iCount << " lines processed on input from "
                << inFile 
                << " with max trace id = "
                << iter.maxTraceId()
                << std::endl;

      idOffset = iter.maxTraceId() + 1;
    }   
  return(0);
}
