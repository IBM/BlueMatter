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
 
#ifdef AK_ONLY_FOR_DEBUG_INCLUDE_LATER

#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <cstdio>
using namespace std ;

// #include <pk/fxlogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverTrajectoryDumperCOM.hpp>
#include <BlueMatter/DataReceiverDVS2BMT.hpp>

#define MAXATOMS 200000

double Masses[MAXATOMS];

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
    char massfilename[256];
    int simulationstart;
    int reverse;

    if (argc < 6) {
        cout << "rdg2bmt <-append> <-dvs> <-nowaters> <-simulationstart N> <-reverse> < -ximages N > < -yimages N > < -zimages N> <-connectivity 3*2+5*8> <-massfile fname> RawDatagramOutput BMTFileName PDBFileName NumNonWaterAtoms NumWaters " << endl;
        return 1;
    }

    for(int i = 0; i < MAXATOMS; i++){Masses[i] = 1.0;}


    strcpy(connectivity, "");
    
    while (( *argv[arg0] == '-')&&(strlen (argv[arg0]) > 1)) {
        if (!strcmp(argv[arg0], "-append")) {
            doappend = 1;
        } else if (!strcmp(argv[arg0], "-nowaters")) {
            nowaters = 1;
        } else if (!strcmp(argv[arg0], "-dvs")) {
    fromdvs = 1;
        } else if (!strcmp(argv[arg0], "-simulationstart")) {
    simulationstart = atoi(argv[++arg0]);
        } else if (!strcmp(argv[arg0], "-reverse")) {
    reverse = 1;
        } else if (!strcmp(argv[arg0], "-ximages")) {
    sscanf(argv[++arg0],"%d",&nXImages);
        } else if (!strcmp(argv[arg0], "-yimages")) {
    sscanf(argv[++arg0],"%d",&nYImages);
        } else if (!strcmp(argv[arg0], "-zimages")) {
    sscanf(argv[++arg0],"%d",&nZImages);
        } else if (!strcmp(argv[arg0], "-connectivity")) {
    sscanf(argv[++arg0],"%s",&connectivity);
        } else if (!strcmp(argv[arg0], "-massfile")) {
    sscanf(argv[++arg0],"%s",massfilename);
    FILE* massPF = fopen(massfilename,"r");
    int i = 0;
    assert(massPF);
    if(fscanf(massPF,"%lf",&Masses[i]))
      {
        i++;
        while(!feof(massPF))
    {
      fscanf(massPF,"%lf",&Masses[i]);
      i++;
    }	    
      }
    else
      {
        printf("Can't read masses in file %s\n",massfilename);
        exit(1);
      }
    fclose(massPF);
        } else {
            cout << "Command option " << argv[arg0] << " not recognized." << endl;
            exit(-1);
        }
        arg0++;
    }


    SimulationParser sp = SimulationParser(argv[arg0++], 5, 0);
    DataReceiverDVS2BMT DRdvs2bmt;
    DataReceiverTrajectoryDumperCOM DRTrajDumper;

    if(fromdvs)
      {
  sp.addReceiver(&DRdvs2bmt);
  DRdvs2bmt.setParams(argv[arg0], argv[arg0+1], atoi(argv[arg0+2]), atoi(argv[arg0+3]), doappend, nowaters, nXImages, nYImages, nZImages);      
      }
    else
      {
      sp.addReceiver(&DRTrajDumper);
      DRTrajDumper.setParams(argv[arg0], argv[arg0+1], atoi(argv[arg0+2]), atoi(argv[arg0+3]), connectivity, Masses, doappend, nowaters,simulationstart, reverse,  nXImages, nYImages, nZImages);      
      }


    sp.init();
    sp.run();
    sp.final();
    //     DRdvs2bmt.final();

    return 0;
}
#endif
