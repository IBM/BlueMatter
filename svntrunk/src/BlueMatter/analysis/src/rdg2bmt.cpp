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
#include <string>
using namespace std ;

// #include <pk/fxlogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverTrajectoryDumper.hpp>
#include <BlueMatter/DataReceiverDVS2BMT.hpp>
// AK : Includes added
#include <BlueMatter/BMT.hpp>

#define PRECISION_DEF 16
#define PRECISION2_DEF 64
#define SKIPFACTOR_DEF 1
#define SKIPFACTOR2_DEF 20
#define VELOCITIES_DEF 0
#define VELOCITIES2_DEF 1
#define FRAMESPERBMT_DEF 0 // means all frames 
#define FILENAMESIZE 1024
#define MAPINTOBOX_DEF 1
#define MAPINTOBOX2_DEF 0
#define RECENTER_DEF 1
#define RECENTER2_DEF 0

inline bool IsValidPrecision(int p) {
	return (p==16 || p==32 || p ==64 || p==0);
}

inline void complainInvalid(const char *arg,int def_val) {
	cout << "Warning: Invalid argument : " << arg << " defaulted to " << def_val << endl;
}

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
    char second_file[FILENAMESIZE];
    int simulationstart = 0;
    int reverse = 0;

    // these three parameters are used only if -dvs argument (ie., fromdvs) 
    //		is specified
    int nonwateratoms=0;
    int waters=0;
    char pdb_file[FILENAMESIZE];
    strcpy(pdb_file,"");


    int precision=PRECISION_DEF;
    int precision2=PRECISION2_DEF;

    // AK: skip_factor is an option that has been added
    //		If >=1:	skip factor applicable only to the second precision output,
    //			  applicable only if second_precision is either 32 or 64
    //		o/w:	don't care: there will be NO second BMT output
    int skip_factor=SKIPFACTOR_DEF;
    int skip_factor2=SKIPFACTOR2_DEF;

    
    int velocities=VELOCITIES_DEF;
    int velocities2=VELOCITIES2_DEF;

    int framesperbmt = FRAMESPERBMT_DEF;

    int mapintobox = MAPINTOBOX_DEF;
    int mapintobox2 = MAPINTOBOX2_DEF;

    int recenter = RECENTER_DEF;
    int recenter2 = RECENTER2_DEF;

    if (argc < 3) {
        cout << "rdg2bmt <-append> <-dvs> <-nowaters> <-simulationstart N> <-reverse> <-ximages N> <-yimages N> <-zimages N> <-connectivity 3*2+5*8> <-precision 16> <-skip_factor 1> <-velocities 0> <-mapintobox 1> <-recenter 1> <-second_file bmtfilename> <-precision2 64> <-skip_factor2 20> <-velocities2 1> <-mapintobox2 0>  <-recenter2 0> <-pdb pdbfilename> <-nonwateratoms 0> <-waters 0>  <-framesperbmt 0> RawDatagramOutput BMTFileName " << endl;
        return 1;
    }

    strcpy(connectivity, "");
    strcpy(second_file, "");
    
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
        } else if (!strcmp(argv[arg0], "-precision")) {
	  sscanf(argv[++arg0],"%d",&precision);
        } else if (!strcmp(argv[arg0], "-skip_factor")) {
	  sscanf(argv[++arg0],"%d",&skip_factor);
        } else if (!strcmp(argv[arg0], "-second_file")) {
	  strcpy(second_file,argv[++arg0]);
        } else if (!strcmp(argv[arg0], "-precision2")) {
	  sscanf(argv[++arg0],"%d",&precision2);
        } else if (!strcmp(argv[arg0], "-skip_factor2")) {
	  sscanf(argv[++arg0],"%d",&skip_factor2);
        } else if (!strcmp(argv[arg0], "-velocities")) {
	  sscanf(argv[++arg0],"%d",&velocities);
        } else if (!strcmp(argv[arg0], "-velocities2")) {
	  sscanf(argv[++arg0],"%d",&velocities2);
        } else if (!strcmp(argv[arg0], "-pdb")) {
	  sscanf(argv[++arg0],"%s",pdb_file);
        } else if (!strcmp(argv[arg0], "-nonwateratoms")) {
	  sscanf(argv[++arg0],"%d",&nonwateratoms);
        } else if (!strcmp(argv[arg0], "-waters")) {
	  sscanf(argv[++arg0],"%d",&waters);
        } else if (!strcmp(argv[arg0], "-framesperbmt")) {
	  sscanf(argv[++arg0],"%d",&framesperbmt);
        } else if (!strcmp(argv[arg0], "-mapintobox")) {
	  sscanf(argv[++arg0],"%d",&mapintobox);
        } else if (!strcmp(argv[arg0], "-mapintobox2")) {
	  sscanf(argv[++arg0],"%d",&mapintobox2);
        } else if (!strcmp(argv[arg0], "-recenter")) {
	  sscanf(argv[++arg0],"%d",&recenter);
        } else if (!strcmp(argv[arg0], "-recenter2")) {
	  sscanf(argv[++arg0],"%d",&recenter2);
        } else {
            cout << "Command option " << argv[arg0] << " not recognized." << endl;
            exit(-1);
        }
        arg0++;
    }

    if(!IsValidPrecision(precision)) {
	complainInvalid("precision",PRECISION_DEF);
	precision=PRECISION_DEF;
    }

    if(!IsValidPrecision(precision2))  {
	complainInvalid("precision2",PRECISION2_DEF);
	precision2=PRECISION2_DEF;
    }

    if(skip_factor<1) {
	complainInvalid("skip_factor",SKIPFACTOR_DEF);
	skip_factor=SKIPFACTOR_DEF;
    }

    if(skip_factor2<1) {
	complainInvalid("skip_factor2",SKIPFACTOR2_DEF);
	skip_factor2=SKIPFACTOR2_DEF;
    }

    if(velocities!=0 && velocities!=1) {
	complainInvalid("velocities",VELOCITIES_DEF);
	velocities=VELOCITIES_DEF;
    }

    if(velocities2!=0 && velocities2!=1) {
	complainInvalid("velocities2",VELOCITIES2_DEF);
	velocities2=VELOCITIES2_DEF;
    }

    if(framesperbmt<0) {
	cout << "Warning: Frames per BMT should be >0 ! Reset to default : " << FRAMESPERBMT_DEF << endl;
	framesperbmt = FRAMESPERBMT_DEF; 
    }

    if(mapintobox<0 || mapintobox>1) {
	cout << "Warning: mapintobox is to be 0 or 1! Reset to default : " << MAPINTOBOX_DEF << endl;
	mapintobox = MAPINTOBOX_DEF;	
    }

    if(mapintobox2<0 || mapintobox2>1) {
	cout << "Warning: mapintobox2 is to be 0 or 1! Reset to default : " << MAPINTOBOX2_DEF << endl;
	mapintobox2 = MAPINTOBOX2_DEF;	
    }

    if(recenter<0 || recenter>1) {
	cout << "Warning: recenter is to be 0 or 1! Reset to default : " << RECENTER_DEF << endl;
	recenter = RECENTER_DEF;	
    }

    if(recenter2<0 || recenter2>1) {
	cout << "Warning: recenter2 is to be 0 or 1! Reset to default : " << RECENTER2_DEF << endl;
	recenter2 = RECENTER2_DEF;	
    }

    SimulationParser sp = SimulationParser(argv[arg0++], 5, 0);
    DataReceiverDVS2BMT DRdvs2bmt;
    DataReceiverTrajectoryDumper DRTrajDumper;
    DataReceiverTrajectoryDumper DRTrajDumper_2;

    if(fromdvs) {
	if(!nonwateratoms || !waters || strcmp(pdb_file,"")==0 ) {
		cout << "Warning: -dvs option specified without providing a valid entry for one/more of the following arguments: -nonwateratoms, -waters, -pdb" << endl;
	}
	sp.addReceiver(&DRdvs2bmt);
	DRdvs2bmt.setParams(argv[arg0], pdb_file, nonwateratoms, waters, doappend, nowaters, nXImages, nYImages, nZImages);      
    }
    else {

	sp.addReceiver(&DRTrajDumper);
	DRTrajDumper.setParams(argv[arg0], connectivity, doappend, nowaters, simulationstart, reverse, skip_factor,precision,velocities,(mapintobox!=0),(recenter!=0),framesperbmt);

	if(precision2>0) {
		sp.addReceiver(&DRTrajDumper_2);

        	// set the BMT file name for the second precision 
		if(strcmp(second_file,"")==0) {
                       	char sfoo[10];
                       	sprintf(sfoo,"_%d",precision2);
			string s=argv[arg0];
        		string::size_type p1=s.find(BMTFILEEXTN);
        		if(p1!=string::npos) {
                       		s.insert(p1,(string)sfoo);
                       		strcpy(second_file,s.c_str());
			}
			else {
				sprintf(second_file,"%s%s%s",argv[arg0],sfoo,BMTFILEEXTN);
			}
		}
		DRTrajDumper_2.setParams(second_file, connectivity, doappend, nowaters, simulationstart, reverse,skip_factor2, precision2,velocities2,(mapintobox2!=0),(recenter2!=0),framesperbmt);
      	}

     }
    sp.init();
    sp.run();
    sp.final();
    if (fromdvs)
	DRdvs2bmt.final();

    return 0;
}

