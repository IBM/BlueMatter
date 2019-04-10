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
#include <BlueMatter/DataReceiverSimpleTimeStamper.hpp>

void usage() {
    printf("SimpleTimeStamper <rawdatagramfile> <-skip N> <-maxtime N> <-summary:raw:detailed:simple> <-interval N> \n");
    printf("-skip N: Take statistics after N steps (default is 10 to skip over initial stuff)\n");
    printf("-interval N: Output data every N steps (default is 1000) \n");
    printf("-summary: Output only the summary info:  NStepsTimed MeanTime StandardDeviation MinTime MaxTime\n");
    printf("-detailed: Output accum histo into uniform bins\n");
    printf("-ndetailedbins: number of detailed bins (default 20)\n");
    printf("-raw: Raw output of TimeStamp packets\n");
    printf("-maxtime: Max time of histogram range\n");
    printf("-integral: output step mean, then time/rate pairs\n\n");
    exit(-1);
}

int
main(int argc, char **argv, char **envp)
{
    // packetfile
    if(argc < 2) {
        usage();
    }

    char* fname = "-";
    int reportmode = DataReceiverSimpleTimeStamper::FULL;
    int nskip = 10;
    int interval = 1000;
    int ndetailedbins = 20;
    float maxtime = 1000.0;


    for (int argn = 1; argn < argc; argn++) {
        char *s = argv[ argn ];
        if(*s != '-') {
            fname = s;
        }
        else if(!strcmp(s, "--")) {
            // read from stdin (default)
        }
        else if(!strcmp(s, "-skip")) {
            nskip = atoi(argv[++argn]);
        }
        else if(!strcmp(s, "-interval")) {
            interval = atoi(argv[++argn]);
        }
        else if(!strcmp(s, "-ndetailedbins")) {
            ndetailedbins = atoi(argv[++argn]);
        }
        else if(!strcmp(s, "-maxtime")) {
            maxtime = atof(argv[++argn]);
        }
        else if(!strcmp(s, "-summary")) {
            reportmode = DataReceiverSimpleTimeStamper::SUMMARY;
        }
        else if(!strcmp(s, "-raw")) {
            reportmode = DataReceiverSimpleTimeStamper::RAW;
        }
        else if(!strcmp(s, "-integral")) {
            reportmode = DataReceiverSimpleTimeStamper::INTEGRAL;
        }
        else if(!strcmp(s, "-simple")) {
            reportmode = DataReceiverSimpleTimeStamper::SIMPLE;
        }
        else if(!strcmp(s, "-detailed")) {
            reportmode = DataReceiverSimpleTimeStamper::DETAILED;
        }
        else {
            usage();
        }
    }

    SimulationParser sp = SimulationParser(fname, 10, 0);

    sp.dumpTimes();

    DataReceiverSimpleTimeStamper DRTimeStamper;

    DRTimeStamper.setParameters(reportmode, interval, nskip, ndetailedbins, maxtime);

    sp.addReceiver(&DRTimeStamper);

    sp.init();

    // don't do synch in simple mode - to keep things simple for yuriy's test
    if (reportmode != DataReceiverSimpleTimeStamper::SIMPLE)
	sp.synchStream();

    sp.run();

    sp.final();
}
