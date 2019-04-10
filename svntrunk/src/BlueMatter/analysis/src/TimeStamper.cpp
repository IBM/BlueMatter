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
 #include <assert.h>
#include <fcntl.h>
#include <cstdio>
using namespace std ;

#include <BlueMatter/ExternalDatagram.hpp>
// #include <pk/fxlogger.hpp>

#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverTimeStamper.hpp>

void usage() {
    printf("TimeStamper <rawdatagramfile> <-delay N> <-summary> \n");
    printf("-delay N: Take statistics after N steps\n");
    printf("-summary: Output only the summary info:  NStepsTimed MeanTime StandardDeviation MinTime MaxTime\n\n");
}

int
main(int argc, char **argv, char **envp)
{
    // packetfile
    if(argc < 2) {
        usage();
    }

    char* fname = NULL;
    int delaySteps = TIMESTEPS_TO_TUNE;
    int dumpSummaryOnly = 0;

    for (int argn = 1; argn < argc; argn++) {
        char *s = argv[ argn ];
        if(*s != '-') {
            fname = s;
        }
        else if(!strcmp(s, "-delay")) {
            delaySteps = atoi(argv[++argn]);
        }
        else if(!strcmp(s, "-summary")) {
            dumpSummaryOnly = 1;
        }
        else {
            usage();
        }
    }

    SimulationParser sp = SimulationParser(fname, 5, 0);

    sp.dumpTimes();

    DataReceiverTimeStamper DRTimeStamper( delaySteps, dumpSummaryOnly );

    sp.addReceiver(&DRTimeStamper);

    sp.init();

    sp.run();

    sp.final();
}
