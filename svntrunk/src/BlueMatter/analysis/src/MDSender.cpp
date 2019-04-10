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

// #include <pk/fxlogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
// #include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverTrajectorySender.hpp>

using namespace std ;


int
main(int argc, char **argv, char **envp)
{
    int nowaters = 0;
    int portnum = 5123;
    int arg0 = 1;

    if (argc < 2) {
        cout << "MDSender <-port N> RawDatagramOutput" << endl;
        return 1;
    }

    while (( *argv[arg0] == '-')&&(strlen (argv[arg0]) > 1)) {
        if (!strcmp(argv[arg0], "-port")) {
            portnum = atoi(argv[++arg0]);
        } else {
            cout << "Command option " << argv[arg0] << " not recognized." << endl;
            exit(-1);
        }
        arg0++;
    }

    SimulationParser sp = SimulationParser(argv[arg0++], 500, 1);
    DataReceiverTrajectorySender DRTrajSender;

    sp.addReceiver(&DRTrajSender);
    DRTrajSender.setParams(portnum);      
    sp.init();
    sp.run();
    sp.final();

    return 0;
}
