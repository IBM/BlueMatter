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
 
// #include <assert.h>
#include <fcntl.h>
#include <cstdio>
#include <stdlib.h>
#include <memory.h>

using namespace std ;

#include <BlueMatter/ExternalDatagram.hpp>
// #include <BlueMatter/MolSysDef.hpp>
#include <BlueMatter/SimulationParser.hpp>

void usage()
{
    printf("PacketDumper <packetfile> <-skip NBytes> \n");
    exit(0);
}
    

int
main(int argc, char **argv, char **envp)
{
    int skip = -1;

    if (argc < 2) {
  usage();
    }

    if (argc > 2) {
  if (strcmp(argv[2], "-skip"))
      usage();
  skip = atoi(argv[3]);
    }

    SimulationParser sp = SimulationParser(argv[1]);

    if (skip >= 0) {
  sp.skip(skip);
  sp.synchStream();
    }

    sp.dumpPackets();

    sp.init();

    sp.run();

    sp.final();
}
