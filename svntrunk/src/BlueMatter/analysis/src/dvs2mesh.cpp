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
#include <cstdio>
#include <stdlib.h>

using namespace std ;
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/DataReceiverDVSParser.hpp>

void usage()
{
    printf("dvs2mesh dvsfile initspacing\n");
    printf("dvs2mesh -BoundingBox <L> <Lx Ly Lz> <xmin ymin zmin xmax ymax zmax> -P3MEinitSpacing d -dvs dvsfile \n");
    exit(1);
}

// check current token, no bump
#define CHECKARG(label, n)                                                 \
    ( !strcmp(argv[argn], "-" #label) )                                               \
    if (argn+n > argc-1) {                                                 \
        cerr << "not enough arguments provided for option " << #label << endl;    \
        exit(1);                                                           \
    } else

// get from next token using pre-bump
#define GOODINT(x) \
    x = atoi(argv[++argn]); \
    if (x==0 && strcmp(argv[argn], "0")) {  \
        cerr << "command line token parsed as zero integer at string " << argv[argn] << endl; \
        exit(1); \
    }

// get from next token using pre-bump
#define GOODFLOAT(x) \
    x = atof(argv[++argn]); \
    if (x==0 && strcmp(argv[argn], "0")) {  \
        cerr << "command line token parsed as zero float at string " << argv[argn] << endl; \
        exit(1); \
    }

// check next token with no bump
#define ISVALUE \
    (argn < argc && !(argv[argn+1][0] == '-' && !isdigit(argv[argn+1][1])))

int opt_num_pts( double boxsize, double grdSp )
{
    double min = ceil(boxsize/grdSp);
    if (min == 0)
	return 0;
    int a, b, c, d;
    int amin, bmin, cmin, dmin;
    int meshPts;
    double lnmin, rema, remb, remc;
    double excess, minexcess;
    static const double ln2 = log(2.), ln3 = log(3.), ln5 = log(5.), ln7 = log(7.);
    static const double ln2_1 = 1.0/ln2, ln3_1 = 1.0/ln3,
        ln5_1 = 1.0/ln5, ln7_1 = 1.0/ln7;

    lnmin = log(min);

    amin = (int)floor(lnmin*ln2_1) + 1;
    bmin = cmin = dmin = 0;
    minexcess = amin*ln2 - lnmin;

    for( a = 0; a <= (int)floor(lnmin*ln2_1) + 1; a++ )
    {
        rema = lnmin - a*ln2;
        for( b = 0; b<=(int)floor(rema*ln3_1)+1; b++ )
        {
            remb = rema - b*ln3;
            for( c = 0; c <= (int)floor(remb*ln5_1)+1; c++ )
            {
                remc = remb - c*ln5;
                d = (int)floor(remc*ln7_1) + 1;
                excess = d*ln7 - remc;
                if (excess<minexcess) {
                    amin = a;
                    bmin = b;
                    cmin = c;
                    dmin = d;
                    minexcess = excess;
                }
            }
        }
    }

    meshPts = 1;
    for( a = 0; a < amin; a++ )
        meshPts *= 2;
    for( b = 0; b < bmin; b++ )
        meshPts *= 3;
    for( c = 0; c < cmin; c++ )
        meshPts *= 5;
    for( d = 0; d < dmin; d++ )
        meshPts *= 7;

    return meshPts;
}



int
main(int argc, char **argv, char **envp)
{
    if (argc < 3)
        usage();

    DataReceiverDVSParser DRDVSParser = DataReceiverDVSParser();

    char *fname = NULL;
    int timeout = 1;
    int waitforfile = 0;
    int currentstepset = 0;
    double initspacing;
    int nx, ny, nz;
    double dx, dy, dz;
    double boxparms[10];
    int nboxparms = 0;

    dx = dy = dz = 0;
    nx = ny = nz = 0;

    int argn = 1;

    // if first param is not of form -param, usage must be 'dvsname spacing'
    if (argv[argn][0] != '-') {
	fname = argv[argn];
	GOODFLOAT(initspacing);
    } else {
	for (; argn < argc; argn++) {
	    if CHECKARG(dvs, 1) {
		fname = argv[++argn];
	    } else if CHECKARG(BoundingBox, 1) {
		while (ISVALUE) {
		    GOODFLOAT(boxparms[nboxparms]);
		    nboxparms++;
		}
	    } else if CHECKARG(P3MEinitSpacing, 1) {
		GOODFLOAT(initspacing);
	    } else {
		cout << "Error parsing " << argv[argn] << endl;
		usage();
	    }
	}
    }

    if (initspacing <= 0) {
	cout << "Init spacing must be specified and positive." << endl;
	usage();
    }

    // First specify all parameters using command line info, then let dvs override them if non-zero

    switch(nboxparms) {
	case 0: break;
	case 1: dx = dy = dz = boxparms[0]; break;
	case 3: dx = boxparms[0]; dy = boxparms[1]; dz = boxparms[2]; break;
	case 6: dx = boxparms[3]-boxparms[0]; dy = boxparms[4]-boxparms[1]; dz = boxparms[5]-boxparms[2]; break;
	default: cout << "Bad number of box parameters." << endl; usage();
    }

    // if dvs specified and it has nonzero box inside, override command specified box, if any
    if (*fname) {
	SimulationParser sp = SimulationParser(fname, timeout, waitforfile);

	sp.addReceiver(&DRDVSParser);
        sp.init();
	sp.run();

#if 0
	// timeout causes not ok, so can't exit here
	if (!sp.OK()) {
	    cout << "ERROR loading dvs file " << fname << endl;
	    exit(-1);
	}
#endif

	double dvsdx, dvsdy, dvsdz;

        dvsdx = fabs(DRDVSParser.rtp.mDynamicBoundingBoxMax.mX - DRDVSParser.rtp.mDynamicBoundingBoxMin.mX);
        dvsdy = fabs(DRDVSParser.rtp.mDynamicBoundingBoxMax.mY - DRDVSParser.rtp.mDynamicBoundingBoxMin.mY);
        dvsdz = fabs(DRDVSParser.rtp.mDynamicBoundingBoxMax.mZ - DRDVSParser.rtp.mDynamicBoundingBoxMin.mZ);

	if (dvsdx > 0 && dvsdy > 0 && dvsdz > 0) {
	    dx = dvsdx;
	    dy = dvsdy;
	    dz = dvsdz;
	}
    }

    nx = opt_num_pts(dx, initspacing);
    ny = opt_num_pts(dy, initspacing);
    nz = opt_num_pts(dz, initspacing);

    if (nx <= 0 || ny <= 0 || nz <= 0) {
	cout << "ERROR in mesh calculation" << endl;
    } else {
	cout << "FFTSize " << nx << " " << ny << " " << nz << endl;
    }

}
