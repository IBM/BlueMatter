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

// #include <pk/fxlogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverDXDumper.hpp>


int
main(int argc, char **argv, char **envp)
{
    int doappend = 0;
    int fromdvs = 0;
    int nowaters = 0;
    int nXImages = 1;
    int nYImages = 1;
    int nZImages = 1;
    int arg0 = 1;
    char connectivity[1024];

#if 0
    if (argc < 6) {
        cout << "rdg2bmt <-append> <-dvs> <-nowaters> < -ximages N > < -yimages N > < -zimages N> <-connectivity 3*2+5*8> RawDatagramOutput BMTFileName PDBFileName NumNonWaterAtoms NumWaters " << endl;
        return 1;
    }


    strcpy(connectivity, "");
    
    while (( *argv[arg0] == '-')&&(strlen (argv[arg0]) > 1)) {
        if (!strcmp(argv[arg0], "-append")) {
            doappend = 1;
        } else if (!strcmp(argv[arg0], "-nowaters")) {
            nowaters = 1;
        } else if (!strcmp(argv[arg0], "-dvs")) {
    fromdvs = 1;
        } else if (!strcmp(argv[arg0], "-ximages")) {
    sscanf(argv[++arg0],"%d",&nXImages);
        } else if (!strcmp(argv[arg0], "-yimages")) {
    sscanf(argv[++arg0],"%d",&nYImages);
        } else if (!strcmp(argv[arg0], "-zimages")) {
    sscanf(argv[++arg0],"%d",&nZImages);
        } else if (!strcmp(argv[arg0], "-connectivity")) {
    sscanf(argv[++arg0],"%s",&connectivity);
        } else {
            cout << "Command option " << argv[arg0] << " not recognized." << endl;
            exit(-1);
        }
        arg0++;
    }

#endif

    SimulationParser sp = SimulationParser(argv[arg0++], 300, 1);

    DataReceiverDXDumper dxd;
    dxd.setParams("OutFrames.bin", "hairpin.pdb", 259, 1660, "1*256+3*1+1660*3");
    sp.addReceiver(&dxd);


    sp.init();
    sp.run();
    sp.final();

    return 0;
}
