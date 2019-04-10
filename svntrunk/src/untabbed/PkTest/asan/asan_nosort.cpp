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
 #include <stdio.h>
#include <string.h>
#include <sstream>

#include <asan_memory_server_bgltree.h>

#include <inmemdb/GlobalParallelObjectManager.hpp>
#include <inmemdb/NamedAllReduce.hpp>

NamedAllReduce<unsigned long long, NAMED_ALLREDUCE_SUM>   AllReduce;
#include <inmemdb/NamedBarrier.hpp>


#include <inmemdb/Table.hpp>

#include <inmemdb/types.hpp>
#include <inmemdb/utils.hpp>
#include <inmemdb/SortGen.hpp>

#include <sstream>

int PkStartType = PkStartAllCores;

#if defined(PKTRACE)
int PkTracing = 1;
#else
int PkTracing = 0;
#endif

// This object is used by both cores
ASAN_MemoryServer asanMemoryServer;

enum {
  k_CoreIdForMemoryServer = 0
};
int PkMain(int argc, char** argv, char** envp)
{  
  int TraceDumpFreq = atoi( argv[ 0 ] );
  int CoreId = rts_get_processor_id();
  
  BegLogLine( 1 )
    << "TraceDumpFreq: " << TraceDumpFreq
    << " CoreId: " << CoreId
    << EndLogLine;

  if( CoreId == 0 )
    {
    GlobalParallelObject::Init();
    AllReduce.Init( "AAA", PkNodeGetCount() );
    asanMemoryServer.Init();
    }
  
  rts_dcache_evict_normal();
  PkIntraChipBarrier();

  if( CoreId == k_CoreIdForMemoryServer )
    {
       asanMemoryServer.Run( TraceDumpFreq );
    }
  else
    {
      while( 1 ) {
        PkFiberYield();
      }
    }
  
  

  return 0;
}
