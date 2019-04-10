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

#include <pk/trcutil.hpp>
#include <pk/selecttrace.hpp>

int main( int argc, char **argv, char **)
  {

    const char* idFile = "idFile.del";
    const char* outFileBase = "binFile";

    unsigned long RunMgz = 700 ;  // allow stating the machine clock rate in mgz
    char* inFile = NULL;
    // open files for output (DEL format)
    switch(argc)
      {
      case 5:
	inFile = argv[4];
      case 4:
        RunMgz = atoi(argv[1]);
        idFile = argv[2];
        outFileBase = argv[3];
        break;
      default:
        std::cerr << argv[0] << " clockSpeedMhz idFileName outFileBase <fileNameForStat64>" << std::endl;
        std::exit(-1);
      }

    std::ofstream idDel(idFile);
    assert(idDel);
    if (!idDel)
      {
        std::cerr << "Error opening file " << idFile << std::endl;
        std::exit(-1);
      }

    trc::TracePointCollection coll;
    coll.init(inFile, RunMgz); // take input from stdin
    int i = 0;
    SelectTrace::trcpt_container_type trcPt;
    TracePointKey newKey;
    SelectTrace::ts_vect dummy;
    trc::TracePointCollection::const_iterator iter;
    for (iter = coll.begin();
         iter != coll.end();
         ++iter)
      {
        const trc::TracePoint& pt = *iter;
        // we only process those data points that we can associate
        // with a named trace point id
        if (iter->globalId() < iter.byGlobalId().size())
          {
            newKey.d_traceId = iter->globalId();
            newKey.d_nodeId = iter->node();
            SelectTrace::trcpt_container_type::value_type val(newKey, dummy);
            (trcPt.insert(val)).first->second.push_back(iter->clock());
#if defined(PRINT_STUFF)
            std::cout << "fxlog2bin: Data trace point with global id: "
                      << iter->globalId()
                      << " from node: " << iter->node()
                      << " label: "
                      << iter.byGlobalId()[iter->globalId()].d_label
                      << " clock = " << iter->clock()
                      << std::endl;
#endif
          }
        else
          {
#if defined(PRINT_STUFF)
            std::cout << "fxlog2bin: Data trace point with global id: "
                      << iter->globalId()
                      << " from node: " << iter->node()
                      << " has no label / "
                      << " clock = " << iter->clock()
                      << std::endl;
#else
            std::cerr << "Tracepoint with globalID = " << iter->globalId()
                      << " has no association with a named trace point"
                      << std::endl;
#endif
          }
        ++i;
#if defined(PRINT_STUFF)
        std::cout << "Processing line " << i << std::endl;
        if (iter.byGlobalId().size() > iter->globalId())
          {
            std::cout << iter.byGlobalId()[iter->globalId()].d_label << " "
                      << iter.byGlobalId()[iter->globalId()].d_traceType << " node: "
                      << iter->node() << " timestamp: " << iter->clock()
                      << std::endl;
          }
        else
          {
            std::cout << "global id: " << iter->globalId()
                      << " requested, but only " << iter.byGlobalId().size()
                      << " elements in byGlobalId vector" << std::endl;
            std::cout << "globalIdMap().size() = " << iter.globalIdMap().size()
                      << std::endl; 
          }
#endif        
      }
#if defined(PRINT_STUFF)
    std::cout << "TraceMapSize() = " << iter.traceMap().size() << std::endl;
    for (trc::TracePointCollectionIterator::TraceMap::const_iterator mapIter = iter.traceMap().begin();
	 mapIter != iter.traceMap().end();
	 ++mapIter)
      {
	std::cout << "TraceMapDump rawId: " << mapIter->d_traceId
		  << " node: " << mapIter->d_node
		  << " trcType: " << mapIter->d_traceType
		  << " globalId: " << mapIter->d_globalId
		  << std::endl;
      }
#endif
    std::cout << i << " lines processed on input" << std::endl;
    const std::vector<trc::GlobalMapElt>& byGlobalId = iter.byGlobalId();
    for (std::vector<trc::GlobalMapElt>::const_iterator iter2=byGlobalId.begin();
         iter2 != byGlobalId.end();
         ++iter2)
      {
        idDel << iter2->d_globalId << ", " << iter2->d_traceType << ", "
              << "\"" << iter2->d_label << "\", "
              << "\"" << iter2->d_desc << "\""
              << std::endl;
      }

    for (SelectTrace::trcpt_container_type::iterator iter = trcPt.begin();
         iter != trcPt.end();
         ++iter)
      {
        std::sort(iter->second.begin(), iter->second.end());
      }

    std::string nvpName(outFileBase);
    nvpName += ".nvp";
    std::ofstream nvpStream(nvpName.c_str());
    if (!nvpStream)
      {
        std::cerr << "Unable to open file " << nvpName << " for output"
                  << std::endl;
        std::exit(-1);
      }
    nvpStream << coll.metaData() << std::endl;
    nvpStream.close();

    SelectTrace trc;
    trc.initFromContainer(trcPt);

    std::string bName(outFileBase);
    bName += ".bin";
    std::ofstream binStream(bName.c_str());
    if (!binStream)
      {
        std::cerr << "Unable to open file " << bName << " for output"
                  << std::endl;
        std::exit(-1);
      }
    trc.putBin(binStream);
    binStream.close();
    return(0);
  }
