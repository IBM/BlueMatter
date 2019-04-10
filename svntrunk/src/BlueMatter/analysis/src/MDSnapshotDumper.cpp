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
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverSnapshotDumper.hpp>


int
main(int argc, char **argv, char **envp)
{
    int doappend = 0;
    int nowaters = 0;
    int arg0 = 1;

    if (argc < 6) {
        cout << "MDSnapshotDumper <-append> <-nowaters> RawDatagramOutput SSDFileName PDBFileName NumNonWaterAtoms NumWaters" << endl;
        return 1;
    }

    while (*argv[arg0] == '-') {
        if (!strcmp(argv[arg0], "-append")) {
            doappend = 1;
        } else if (!strcmp(argv[arg0], "-nowaters")) {
            nowaters = 1;
        } else {
            cout << "Command option " << argv[arg0] << " not recognized." << endl;
            exit(-1);
        }
        arg0++;
    }


    SimulationParser sp = SimulationParser(argv[arg0++], 5, 0);

    DataReceiverSnapshotDumper DRSnapDumper;

    sp.addReceiver(&DRSnapDumper);

    DRSnapDumper.setParams(argv[arg0], argv[arg0+1], atoi(argv[arg0+2]), atoi(argv[arg0+3]), doappend, nowaters);

    sp.init();

    sp.run();

    sp.final();

    return 0;
}
