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
 /***************************************************************************
 * Project:         dtrace
 *
 * Module:          pktracefile_api.hpp
 *
 * Purpose:         An API to allow access to trace stream files.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef __PKTRACEFILE_API_HPP__
#define __PKTRACEFILE_API_HPP__

#include <pk/pktrace_pk.hpp>


/*
 * Class representing a single trace stream. 
 */
class TraceLineObject
{
  public:
    TraceLineObject(__pktraceRead* ServerRef,int Position);
    ~TraceLineObject();

    __pktraceRead* Server;
    int            TracePosition;
  public:
    LTime GetLTimeAtPosition(int Position);
    char* GetTraceName();
    int   GetNumPoints();

};

/*
 * Class to open trace files and parse through them creating TraceLineObject.
 */
class TraceFileAccess
{
  public:
    // Constructors
    TraceFileAccess();
    TraceFileAccess(char* Filename);
    ~TraceFileAccess();

    // Open the trace stream collection 
    int   OpenFile(char* Filename);
    int   CloseFile();

    // Returns the name of the 
    char* GetOpenFileName();

    // Returns the number of trace streams in the currently open file.
    int   GetNumTraces();

    // Get the trace 
    TraceLineObject* GetPrevTrace();
    //
    TraceLineObject* GetNextTrace();

    // Returns a TraceLineObject generated from the trace at Position in the
    // currently open trace collection file.
    TraceLineObject* GetTraceNameAtPos(int   Position);

  private:
    __pktraceRead* Server;                // Server to access the trace 
                                          // collection file.
    int            CurrentTracePosition;  // Marker for last trace stream
                                          // accessed
};
#endif
