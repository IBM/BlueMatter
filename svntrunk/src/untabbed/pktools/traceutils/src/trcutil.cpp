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
// File: trcutil.cpp
// Author: RSG
// Date: February 26, 2005
// Description: Utilities useful for various things
// Namespace: trc
// *************************************************************************

#define NO_PK_PLATFORM

#include <pktools/trcutil.hpp>

#if !defined(FXLOG_MAX_NODES)
#define FXLOG_MAX_NODES (16384) 
#endif
#include <fstream>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <sys/stat.h>

#ifndef PLATFORM_ABORT
#define PLATFORM_ABORT(reason) \
   std::cerr << reason << " in " << __FILE__ << " at line " \
   << __LINE__ << std::endl; \
   std::exit(-1);
#endif

namespace trc
{
  char TracePointCollectionIterator::FXLOG_TAG_STR[] = "PKTRACE ";
  char TracePointCollectionIterator::FXLOG_DATA_STR[] = "DATA ";
  char TracePointCollectionIterator::FXLOG_ID_STR[]   = "ID ";

  TracePointCollection::TracePointCollection()
  {
    d_begin.d_metaData_p = NULL;
    d_end.d_metaData_p = NULL;
  }

  TracePointCollection::~TracePointCollection()
  {
    if (d_begin.d_metaData_p != NULL)
      {
        delete d_begin.d_metaData_p;
      }
  }

  void TracePointCollection::init(const char* fileName, int runMHz)
  {
    d_runMHz = runMHz;

    d_begin.d_lineNo = -1;
    d_end.d_lineNo = -1;
    d_begin.d_inputLines = 0;
    d_begin.d_byteCount = 0;
    d_end.d_byteCount = -1;
    d_begin.d_fileSize = 0;
    d_end.d_fileSize = -1;
    d_begin.d_is_p = NULL;
    d_end.d_is_p = NULL;
    d_begin.d_cursor = -1;
    d_end.d_cursor = -1;
    d_begin.d_node = -1;
    d_end.d_node = -1;
    if (d_begin.d_metaData_p != NULL)
      {
        delete d_begin.d_metaData_p;
      }
    d_begin.d_metaData_p = new TraceMetaData();
    if (d_begin.d_metaData_p == NULL)
      {
        PLATFORM_ABORT("Unable to allocate memory TraceMetaData");
      }

    struct stat64 fileStat;
    int rc = stat64(fileName, &fileStat);
    if (rc != 0)
      {
  PLATFORM_ABORT("Error on stat64 of file");
      }
    d_begin.d_fileSize = fileStat.st_size;
    d_begin.d_is_p = &std::cin;
    d_end.d_is_p = &std::cin;
#if defined(PRINT_STUFF)
    std::cout << "taking input from stdin" << std::endl;
    std::cout.flush();
#endif

    d_begin.init(d_runMHz);
    rc = d_begin.advance();
#if defined(PRINT_STUFF)
    std::cout << "for begin.advance(), rc = " << rc << std::endl;
#endif
  }

  
  TracePointCollection::const_iterator& TracePointCollection::begin()
  {
    return(d_begin);
  }

  TracePointCollection::const_iterator& TracePointCollection::end()
  {
    return(d_end);
  }

  void TracePointCollectionIterator::init(int runMHz)
  {
    d_runMHz = runMHz;
    d_lineNo = -1;
    d_cursor = -1;
    d_node   = -1;
    d_inputLines = 0;
    d_traceMap.erase(d_traceMap.begin(), d_traceMap.end());
    d_globalIdMap.erase(d_globalIdMap.begin(), d_globalIdMap.end());
    d_byGlobalId.clear();
#if defined(RSG_DEBUG)
    d_trace = new std::ofstream;
    d_nontrace = new std::ofstream;
    if ((d_trace==NULL) || (d_nontrace==NULL))
      {
        PLATFORM_ABORT("Unable to allocate file streams");
      }
    d_trace->open("traceLines.list");
    if (!d_trace)
      {
        PLATFORM_ABORT("Unable to open traceLines.list file");
      }
    d_nontrace->open("nontraceLines.list");
    if (!d_nontrace)
      {
        PLATFORM_ABORT("Unable to open nontraceLines.list file");
      }
#endif
  }

  TracePointCollectionIterator&
  TracePointCollectionIterator::operator=(const TracePointCollectionIterator& other)
  {
    if (this == &other)
      {
        return(*this);
      }
    d_traceMap = other.d_traceMap;
    d_globalIdMap = other.d_globalIdMap;
    d_byGlobalId = other.d_byGlobalId;
    d_runMHz = other.d_runMHz;
    d_is_p = other.d_is_p;
    d_logLine = other.d_logLine;
    d_lineNo = other.d_lineNo;
    d_inputLines = other.d_inputLines;
    d_byteCount = other.d_byteCount;
    d_fileSize = other.d_fileSize;
    d_earliestTraceEvent = other.d_earliestTraceEvent;
    d_current = other.d_current;
    d_cursor = other.d_cursor;
    d_node = other.d_node;
    d_metaData_p = other.d_metaData_p;
#if defined(RSG_DEBUG)
    d_trace = other.d_trace;
    d_nontrace = other.d_nontrace;
#endif
    return(*this);
  }

  // *****************************************************************
  // Returns 1 if we got something invalid, but the
  // logLine buffer is not yet empty.
  // Returns 0 if we got something valid
  // Returns -1 if the logLine buffer is empty
  // *****************************************************************

  int TracePointCollectionIterator::unpack()
  {
    if ( (d_cursor == -1) || 
         (d_logLine[d_cursor] == '<') || 
         (d_logLine.size() <= d_cursor) )
      {
        d_node = -1;
        d_cursor = -1;
        return(EMPTY_BUFFER);
      }
    unsigned long long ll;
    if( Hex2Thing( &d_logLine.c_str()[d_cursor], ll ) != 0 )
      {
  std::cout.flush();
        std::cerr << "Error unpacking in " << __FILE__
                  << " at line " << __LINE__ << std::endl;
        std::cerr << "Cursor at " <<  &d_logLine.c_str()[d_cursor]
                  << std::endl;
        return(GOT_UNKNOWN);
        //                    std::cerr << "\n***Hex2Thing<long long> failed to unload 16 hex chars from log line.  cursor points to: >" 
        //            << &d_logLine.c_str()[d_cursor] << "<" << std::endl;
        // break;  // exit( -1 );
        //    return(ERROR_UNPACKING);
        //                    PLATFORM_ABORT("Error unpacking from hex");
      }
    d_cursor += 2 * sizeof( long long );
#if defined(PRINT_STUFF)
    // printf("%016llX\n", ll );
    std::cout << std::hex << ll << std::dec << std::endl;
#endif

    unsigned short id = ll >> 48;
    ll = ll & 0x0000FFFFFFFFFFFFll;
    
    unsigned long secs = ll / (d_runMHz * 1000000);
    unsigned long pclocks = (ll % (d_runMHz * 1000000));
    unsigned long nsecs = pclocks * (1000.0/d_runMHz) ;
    
    unsigned long long TracerTimeFormat = ll * (1000.0/d_runMHz) ;

#if defined(PRINT_STUFF)
    // printf("id %04X pclocks %18lld (0x%016llX) nanosec %16lld  secs/nsecs %9u.%09u \n", id, ll, ll, TracerTimeFormat, secs, nsecs );
#endif

    // Should remap to a common id scheme right here
    // index by trace_id and node
    TraceMapElt tryElt;
    tryElt.d_traceId = id;
    tryElt.d_node = d_node;
    TraceMap::const_iterator iter = d_traceMap.find(tryElt);
    if (iter != d_traceMap.end())
      {
        //            dataDel << id << ", " << node << ", " << ll
        // << std::endl;
#if 0
        dataDel << iter->d_globalId << ", " << node << ", " << ll
                << std::endl;
#endif
        GlobalMapElt& gElt = d_byGlobalId[iter->d_globalId];
        d_current.d_traceId = gElt.d_label;
        d_current.d_traceType = gElt.d_traceType;
        d_current.d_node = d_node;
        d_current.d_globalId = iter->d_globalId;
        d_current.d_clock = ll;
        ++d_lineNo;
      }
    else
      {
  std::cout.flush();
        std::cerr << "Global ID not found: " << id << ", "
                  << d_node << ", " << ll << std::endl;
        return(GOT_UNKNOWN); // keep going
      }
    return(GOT_DATA);
  }

  // ***********************************************************************
  // This method is used by begin() and operator++ to advance through
  // the input stream to the next data record.  In the course of this,
  // it processes id records to create the mapping of (id, node) pairs
  // to trace point identifiers and trace point types
  // ***********************************************************************

  int TracePointCollectionIterator::advance()
  {
    int upk;
    while ((upk = unpack()) > 0)
      {}
    if (upk < 0)
      {
        while(std::getline(*d_is_p, d_logLine) && (d_byteCount <= d_fileSize))
          {
      d_byteCount += d_logLine.size();
      if (d_byteCount > d_fileSize)
        {
    break;
        }
      ++d_byteCount;
            ++d_inputLines;
#if defined(PRINT_STUFF)
      std::cout << "Line: " << d_inputLines
          << " ByteCount: " << d_byteCount
          << std::endl;
      std::cout.flush();
            std::cout << d_logLine << std::endl;
#endif

            // search for payload start
            if ((d_cursor = d_logLine.find('>')) == std::string::npos)
              {
                continue; // non-logline in stream
              }
            ++d_cursor;
            // Fairly quickly look at the first sizeof(int) bytes of the payload
            // to see if they are what we are looking for - if not,
            // continue scanning.
#if 0
            if ( *((long long*)(&d_logLine.c_str()[d_cursor])) !=
                 *((long long*)(FXLOG_TAG_STR)) )
#endif
              if (std::strncmp(&d_logLine.c_str()[d_cursor], 
                               FXLOG_TAG_STR,
                               std::strlen(FXLOG_TAG_STR)) != 0)
                {
#if defined(RSG_DEBUG)
                  *d_nontrace << d_logLine << std::endl;
#endif
#if defined(PRINT_STUFF)
                  std::cout << "no FXLOG_TAG_STR found" << std::endl;;
#endif
                  // ***********************************************
                  // Let's check for any metadata lines right here
                  // ***********************************************
                  
                  d_metaData_p->parse(&d_logLine.c_str()[d_cursor]);

                  continue;
                }
#if defined(RSG_DEBUG)
            *d_trace << d_logLine << std::endl;
#endif

            //        d_cursor += ( sizeof( FXLOG_TAG_STR ) - 1 ); // -1 because sizeof() 
                                                   // will include the NULL
            d_cursor += ( std::strlen( FXLOG_TAG_STR ) );
#if defined(PRINT_STUFF)
            std::cout << "After tag string:" << &d_logLine.c_str()[d_cursor] << std::endl;
#endif
            // NOTE : Packet type strings include the space.

            if( ! std::strncmp( &d_logLine.c_str()[d_cursor],
                                FXLOG_DATA_STR,
                                std::strlen( FXLOG_DATA_STR) )  )
              {
#if defined(PRINT_STUFF)
                std::cout << "DATA LINE >" << &d_logLine.c_str()[d_cursor]
                          << "<" << std::endl;
#endif
                d_cursor += std::strlen( FXLOG_DATA_STR );
#if defined(PRINT_STUFF)
                std::cout << " Got Data string" << std::endl;
#endif

                unsigned short node = 0xFFFF;
                if (Hex2Thing( &d_logLine.c_str()[d_cursor], node ) != 0)
                  {
        std::cout.flush();
                    std::cerr << "Error unpacking in " << __FILE__
                              << " at line " << __LINE__ << std::endl;
                    std::cerr << "D_Cursor at " <<  &d_logLine.c_str()[d_cursor]
                              << std::endl;
                    continue; // get another line
                  }
                //            assert( node < FXLOG_MAX_NODES );
                d_cursor += 2 * sizeof( node );  //
                d_node = node;
                d_cursor ++;


                while( (upk = unpack()) > 0)
                  {}
                if (upk < 0)
                  {
                    continue;
                  }
                return(d_lineNo); // Found a data record, now return
              } // closes block: if( ! std::strncmp( &d_logLine.c_str()[d_cursor],
            //                                   FXLOG_DATA_STR,
            //                                   std::strlen( FXLOG_DATA_STR) )  )
            // This closes the block that deals with data records
            // Next block deals with ids.
            else if( ! std::strncmp( &d_logLine.c_str()[d_cursor],
                                     FXLOG_ID_STR,
                                     std::strlen( FXLOG_ID_STR) )  )
              {
#if defined(PRINT_STUFF)
                std::cout << "Processing ID line" << std::endl;;
#endif
                char working[MAX_LINE];
#if defined(PRINT_STUFF)
                // printf("ID LINE >%s<\n", &d_logLine.c_str()[d_cursor] );
                std::cout << "ID LINE >" << &d_logLine.c_str()[d_cursor]
                          << "<" << std::endl;
                //      printf(">%s<\n", &d_logLine.c_str()[d_cursor] );
                std::cout << ">" << &d_logLine.c_str()[d_cursor] << "<" << std::endl;
#endif
                unsigned short id   = 0xFFFF;
                d_cursor += std::strlen( FXLOG_ID_STR );
                
                unsigned short node = 0xFFFF;
                if (Hex2Thing( &d_logLine.c_str()[d_cursor], node ) != 0)
                  {
        std::cout.flush();
                    std::cerr << "Error unpacking in " << __FILE__
                              << " at line " << __LINE__ << std::endl;
                    std::cerr << "Cursor at " <<  &d_logLine.c_str()[d_cursor]
                              << std::endl;
                    //                PLATFORM_ABORT("Error unpacking from hex");
                  }
                assert( node < FXLOG_MAX_NODES );
                d_cursor += 2 * sizeof( node );  //
                d_cursor ++;

                if (Hex2Thing( &d_logLine.c_str()[d_cursor], id ) != 0)
                  {
        std::cout.flush();
                    std::cerr << "Error unpacking in " << __FILE__
                              << " at line " << __LINE__ << std::endl;
                    std::cerr << "Cursor at " <<  &d_logLine.c_str()[d_cursor]
                              << std::endl;
                    //                PLATFORM_ABORT("Error unpacking from hex");
                  }
                d_cursor += 2 * sizeof( id );  //
                d_cursor ++;                   // space after id

                std::string::size_type end_cursor = d_logLine.rfind("<");
                if (end_cursor == std::string::npos)
                  {
                    PLATFORM_ABORT("Ill-formed log line, missing <");
                  }

                //NULL Term the name in the buffer
                d_logLine[end_cursor] = '\0';

#if defined(PRINT_STUFF)
                std::cout << "d_cursor = " << d_cursor << ", "
                          << "end_cursor = " << end_cursor << std::endl;
                std::cout << "end_cursor - d_cursor = " << end_cursor - d_cursor
                          << std::endl;
                std::cout << "std::strlen(&d_logLine.c_str()[d_cursor]) = "
                          << std::strlen(&d_logLine.c_str()[d_cursor])
                          << std::endl;
                //        printf("Got from node >%04X<  id >%04X< str >%s<\n", node, id, &d_logLine.c_str()[d_cursor] );
                std::cout << "Got from node " << node
                          << " id " << id 
                          << " str >" << &d_logLine.c_str()[d_cursor] << "<"
                          << std::endl;
#endif
                // I want to parse some stuff out of the identifier
                std::strcpy(working, &d_logLine.c_str()[d_cursor]);
#if defined(PRINT_STUFF)
    std::cout << "working = " << working << std::endl;
#endif
                char* tok = std::strtok(working, ":");
#if defined(PRINT_STUFF)
    std::cout << "tok = " << tok << std::endl;
#endif
    // Now we need to extract core identification as well
                int aNode = std::atoi(tok); // should be equal to node

                tok = std::strtok(NULL, ":");
#if defined(PRINT_STUFF)
    std::cout << "tok = " << tok << std::endl;
#endif

    int aCore = std::atoi(tok); // core id
#if defined(PRINT_STUFF)
    std::cout << "id = " << id << " ";
    std::cout << "node = " << node << " ";
    std::cout << "aNode = " << aNode << " ";
    std::cout << "aCore = " << aCore << std::endl;
#endif
    
    tok = std::strtok(NULL, ":");
#if defined(PRINT_STUFF)
    std::cout << "tok = " << tok << std::endl;
#endif

    if (aNode != node)
      {
        std::cout.flush();
        PLATFORM_ABORT("Inconsistent node ids from trace id line");
      }

                // Get the "real" identifier first, but clip off any subscript stuff
                char* clip = std::strchr(tok, '[');
                int idLen = 0;
                char* idBase = tok;
                if (clip != NULL)
                  {
                    idLen = clip - idBase;
                  }
                else
                  {
                    idLen = std::strlen(idBase);
                  }

                // next look for Start or Finis in the "real" identifier so that
                // we clip that off and pair up start/finish pairs properly
                int baseLen = idLen - 5;
                if ((std::strncmp(&idBase[baseLen], "Start", 5) == 0) ||
                    (std::strncmp(&idBase[baseLen], "Finis", 5) == 0))
                  {
                    idLen = baseLen;
                  }


                // now grab the "short" identifier that usually carries the
                // Start/Finis suffix
                tok = std::strtok(NULL, ":");

    int totalLen = std::strlen(tok);
                // Now I'm going to assume that this identifier ends in either
                // Start or Finis (5 characters).
                baseLen = std::strlen(tok) - 5;
                int traceType = -1; // 0 is start, 1 is finish
                if (std::strcmp(&tok[baseLen], "Start") == 0)
                  {
                    traceType = 0;
                  }
                else
                  {
                    if (std::strcmp(&tok[baseLen], "Finis") == 0)
                      {
                        traceType = 1;
                      }
                  }

                idBase[idLen] = '\0';
    std::string trcBase(idBase);

    // now I am going to skip over the field that is left null and the file path field
    // and check to see if there is performance counter field on the PKTRACE ID line

    tok = tok + totalLen + 1;
    tok = std::strtok(tok, ":"); // This should be file/path/line field
    tok = std::strtok(NULL, ":"); // If this is non-null, we've got something to look at
    if (tok != NULL)
      {
        trcBase+=":";
        trcBase+=std::string(tok); // this is the performance counter name
        tok = std::strtok(NULL, ":"); // If this is NULL, we are missing edge information
        if (tok != NULL)
          {
      trcBase+=":";
      trcBase+=std::string(tok); // this is the edge information
          }
        else
          {
      std::cout.flush();
      std::cerr << "Missing edge information for " << trcBase << std::endl;
          }
      }
    else
      {
        trcBase+=":TIME:0";
      }

                // **********************************************************
                // From a line like:
                //  "019:0:UDF_KineticEnergy[0]:LFTDStart::|include|BlueMatter|LocalFragmentDirectDriver.hpp[0043]"
                // we should have parsed:
                // node = 19
                // traceType = 0 (from LFTDStart)
                // idBase = "UDF_KineticEnergy"
                // tok = "LFTDStart"
                // tok(2) = "LFTD"
                // **********************************************************
      
                
                GlobalMapElt gElt;
                TraceMapElt tElt;
    std::ostringstream labelStream;
    labelStream << trcBase << ":" << aCore;
                gElt.d_label = labelStream.str();
                gElt.d_traceType = traceType;
                gElt.d_globalId = d_globalIdMap.size();
                gElt.d_desc = std::string(&d_logLine.c_str()[d_cursor]);
                std::pair<GlobalIdMap::iterator, bool> rc1 = d_globalIdMap.insert(gElt);
#if defined(PRINT_STUFF)
                std::cout << "globalIdMap.size() = " << d_globalIdMap.size()
                          << std::endl;
#endif
                if (!rc1.second) // a mapping already exists for this label/trace_type
                  {
                    if (gElt.d_label != rc1.first->d_label)
                      {
      std::cout.flush();
                        std::cerr << "Mismatch between labels for global id = "
                                  << rc1.first->d_globalId << " existing label: "
                                  << rc1.first->d_label << " new label: "
                                  << gElt.d_label << std::endl;
                        PLATFORM_ABORT("Label Mismatch for global id");
                      }
                    tElt.d_globalId = rc1.first->d_globalId;
                  }
                else // no mapping exists
                  {
                    tElt.d_globalId = gElt.d_globalId;
                    d_byGlobalId.push_back(gElt);
#if defined(PRINT_STUFF)
                    std::cout << "byGlobalId.size() = "
                              << d_byGlobalId.size() << std::endl;
                    if (d_byGlobalId.size() != d_globalIdMap.size())
                      {
                        std::cout << "Mismatch between byGlobalId.size() "
                                  << " and globalIdMap.size()" << std::endl;
                      }
#endif
                    // going to put out global_id, trace_type, trace_label,
                    // trace_desc
#if 0                
                    idDel << gElt.d_globalId << ", "
                          << gElt.d_traceType << ", "
                          << "\"" << gElt.d_label << "\"" << ", "
                          << "\"" << gElt.d_desc << "\"" << ", "
                          << std::endl;
#endif
                  }
                tElt.d_traceId = id;
                tElt.d_node = node;
                tElt.d_traceType = traceType;
                std::pair<TraceMap::iterator, bool> rc = d_traceMap.insert(tElt);
                if (!rc.second)
                  {
        std::cout.flush();
                    std::cerr << "Duplicate entries for global id " << rc.first->d_globalId
                              << " label: " << d_byGlobalId[rc.first->d_globalId].d_label
                              << " traceId: " << tElt.d_traceId
                              << " node: " << tElt.d_node << std::endl;
                  }
#if defined(PRINT_STUFF)
    if (rc.second)
      {
        std::cout << "Successful traceMap.insert of: global_id " << rc.first->d_globalId << " "
                              << " label: " << d_byGlobalId[rc.first->d_globalId].d_label
                              << " traceId: " << tElt.d_traceId
                              << " node: " << tElt.d_node 
            << " traceType " << tElt.d_traceType << std::endl;
      }
    else
      {
        std::cout << "Duplicate entries for global id " << rc.first->d_globalId
                              << " label: " << d_byGlobalId[rc.first->d_globalId].d_label
                              << " traceId: " << tElt.d_traceId
                              << " node: " << tElt.d_node
            << " traceType " << tElt.d_traceType << std::endl;
      }

                std::cout << "New traceMap.size() = " << d_traceMap.size() << std::endl;
#endif
                // there is no break here because we want to advance to a data record
              } // end of id record processing block
#if defined(PRINT_STUFF)
            std::cout << "inputLines = " << d_inputLines << std::endl;;
            std::cout.flush();
#endif        
          } // end of while(getline(..... block
        if (d_is_p->eof() || (d_byteCount > d_fileSize))
          {
#if defined(PRINT_STUFF)
            std::cout << "reached EOF" << std::endl;
#endif
            d_lineNo = END_LINE_NO; // when we reach eof or other "bad" condition
            return(END_LINE_NO);
          }
        
        if (!*d_is_p)
          {
            d_lineNo = END_LINE_NO;
            return(ERROR_ON_READ);
          }
      } // end of if block executed if nothing is available in logLine buffer

    return(d_lineNo); 
  }
}
