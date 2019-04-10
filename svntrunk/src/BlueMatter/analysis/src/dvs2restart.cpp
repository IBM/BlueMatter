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
    cerr << "dvs2restart dvsfile <-reset> <-resetall> <-temp T> <-currentstep N> <-nsteps N> <-vrseed N> <-resampleperiod N> <-snapshotperiod N>" << endl;
    cerr << "    <-box x0 y0 z0 x1 y1 z1> <-box1 L> <-box3 L W H> <-box4 x0 y0 z0 L>" << endl;
    cerr << "    <-addv vX vY vZ> <-addp pX pY pZ> <-invertv> <-invertpv> <-dither d> <-dvsseed n>" << endl;
    cerr << "    <-bmt my.bmt> <-timestep n> <-simulationtime time> <-bmtstart> <-bmtend>" << endl;
    cerr << "    <-rotate 0 1 2> to rotate simulation.  Add 3 to first index for parity switch" << endl;
    cerr << "    <-repexperiod N>" << endl;
    cerr <<"    The following require molecular system info, either via rtp packet in dvs, or by providing id on command line" << endl;
    cerr <<"    <-info> <-sourceid n> <-systemid n> <-mapintobox> <-randomscatter N> <-zeromomentum> <-dithermolecule d> <-dbase dbname (default is mdsetup)>" << endl;
    cerr <<"    <-zeroproteinmomentum> <-zerowatermomentum> <-zerononwatermomentum> <-zeroprotmemwatmomentum>" << endl;
    
    cerr << "    -reset sets to 0: NTimeSteps, CurrentStep, ResamplePeriod, ResampleTemperature, SnapPeriod, EnergyLoss, PressureTarget, RandomState" << endl;
    cerr << "    -resetall is same as reset, but also zero's the bounding box (so it is settable by exe)" << endl;
    cerr << "    If you specify a setting BEFORE -reset on command line, that value will be used." << endl;
    exit(1);
}

#define CHECKARG(label, n)                                                        \
    ( !strcmp(s, "-" #label) )                                                    \
    if (argn+n > argc-1) {                                                        \
        cerr << "not enough arguments provided for option " << #label << endl;    \
        exit(1);                                                                  \
    } else

#define GOODINT(x)                                                                            \
    x = atoi(argv[++argn]);                                                                   \
    if (x==0 && strcmp(argv[argn], "0")) {                                                    \
        cerr << "command line token parsed as zero integer at string " << argv[argn] << endl; \
        exit(1);                                                                              \
    }

#define GOODFLOAT(x)                                                                        \
    x = atof(argv[++argn]);                                                                 \
    if (x==0 && strcmp(argv[argn], "0")) {                                                  \
        cerr << "command line token parsed as zero float at string " << argv[argn] << endl; \
        exit(1);                                                                            \
    }



int
main(int argc, char **argv, char **envp)
{
    if (argc < 2)
        usage();

    DataReceiverDVSParser DRDVSParser = DataReceiverDVSParser();


    char *fname = NULL;
    int timeout = 1;
    int waitforfile = 1;
    int currentstepset = 0;
    double x;
    int n;

    // set all command line args as "known" so they override values in input dvs file
    for (int argn = 1; argn < argc; argn++) {
        char *s = argv[argn];
        if (*s != '-') {
            fname = s;
        } else if CHECKARG(timeout, 1) {
            GOODINT(timeout);
        } else if CHECKARG(resetall, 0) {
            DRDVSParser.resetAll();
        } else if CHECKARG(reset, 0) {
            DRDVSParser.reset();
        } else if CHECKARG(temp, 1) {
            GOODFLOAT(x);
            DRDVSParser.setTemp(x);
        } else if CHECKARG(systemid, 1) {
            GOODINT(n);
            DRDVSParser.setSystemID(n);
        } else if CHECKARG(sourceid, 1) {
            GOODINT(n);
            DRDVSParser.setSourceID(n);
        } else if CHECKARG(repexperiod, 1) {
            GOODINT(n);
            DRDVSParser.setRepExPeriod(n);
        } else if CHECKARG(nsteps, 1) {
            GOODINT(n);
            DRDVSParser.setNSteps(n);
        } else if CHECKARG(vrseed, 1) {
            GOODINT(n);
            DRDVSParser.setVelocityResampleRandomSeed(n);
        } else if CHECKARG(resampleperiod, 1) {
            GOODINT(n);
            DRDVSParser.setVelocityResamplePeriod(n);
        } else if CHECKARG(snapshotperiod, 1) {
            GOODINT(n);
            DRDVSParser.setSnapshotPeriod(n);
        } else if CHECKARG(mapintobox, 0) {
            DRDVSParser.setMapIntoBox();
        } else if CHECKARG(dither, 1) {
            GOODFLOAT(x);
            DRDVSParser.setDither(x);
	} else if CHECKARG(bmt, 0) {
	    DRDVSParser.setBMTName(argv[++argn]);
	} else if CHECKARG(timestep, 1) {
	    GOODINT(n);
	    DRDVSParser.setBMTTimeStep(n);
	} else if CHECKARG(bmtstart, 0) {
	    DRDVSParser.setBMTStart();
	} else if CHECKARG(bmtend, 0) {
	    DRDVSParser.setBMTEnd();
	} else if CHECKARG(simulationtime, 0) { // no argcheck since could be negative
	    GOODFLOAT(x);
	    DRDVSParser.setBMTSimulationTime(x);
        } else if CHECKARG(dvsseed, 1) {
            GOODINT(n);
            srandom(n);
        } else if CHECKARG(dithermolecule, 1) {
            GOODFLOAT(x);
            DRDVSParser.setDitherMolecule(x);
        } else if CHECKARG(box1, 1) {
            GOODFLOAT(x);
            DRDVSParser.setBoundingBox(-x/2, -x/2, -x/2, x/2, x/2, x/2);
        } else if CHECKARG(box3, 3) {
            double L, W, H;
            GOODFLOAT(L);
            GOODFLOAT(W);
            GOODFLOAT(H);
            DRDVSParser.setBoundingBox(-L/2, -W/2, -H/2, L/2, W/2, H/2);
        } else if CHECKARG(box4, 4) {
            double x, y, z, L;
            GOODFLOAT(x);
            GOODFLOAT(y);
            GOODFLOAT(z);
            GOODFLOAT(L);
            DRDVSParser.setBoundingBox(x-L/2, y-L/2, z-L/2, x+L/2, x+L/2, x+L/2);
        } else if CHECKARG(box, 6) {
            double x0, y0, z0, x1, y1, z1;
            GOODFLOAT(x0);
            GOODFLOAT(y0);
            GOODFLOAT(z0);
            GOODFLOAT(x1);
            GOODFLOAT(y1);
            GOODFLOAT(z1);
            DRDVSParser.setBoundingBox(x0, y0, z0, x1, y1, z1);
        } else if CHECKARG(currentstep, 1) {
            GOODINT(n);
            DRDVSParser.setCurrentStep(n);
            currentstepset = 1;
        } else if CHECKARG(randomscatter, 1) {
            GOODINT(n);
            DRDVSParser.setRandomScatter(n);
        } else if CHECKARG(invertv, 0) {
            DRDVSParser.setInvertV();
        } else if CHECKARG(invertpv, 0) {
            DRDVSParser.setInvertPV();
        } else if CHECKARG(zeromomentum, 0) {
            DRDVSParser.setZeroMomentum();
        } else if CHECKARG(zeroproteinmomentum, 0) {
            DRDVSParser.setZeroProteinMomentum();
        } else if CHECKARG(zerowatermomentum, 0) {
            DRDVSParser.setZeroWaterMomentum();
        } else if CHECKARG(zerononwatermomentum, 0) {
            DRDVSParser.setZeroNonWaterMomentum();
        } else if CHECKARG(zeroprotmemwatmomentum, 0) {
            DRDVSParser.setZeroProtMemWatMomentum();
        } else if CHECKARG(dbase, 1) {
            DRDVSParser.setDBaseName(argv[++argn]);
	} else if CHECKARG(info, 0) {
	    DRDVSParser.info = 1;
        } else if CHECKARG(addp, 3) {
            XYZ v;
            GOODFLOAT(v.mX);
            GOODFLOAT(v.mY);
            GOODFLOAT(v.mZ);
            DRDVSParser.setPShift(v);
        } else if CHECKARG(addv, 3) {
            XYZ v;
            GOODFLOAT(v.mX);
            GOODFLOAT(v.mY);
            GOODFLOAT(v.mZ);
            DRDVSParser.setVShift(v);
        } else if CHECKARG(rotate, 3) {
            int a, b, c;
            GOODINT(a);
            GOODINT(b);
            GOODINT(c);
            DRDVSParser.setRotation(a, b, c);
        } else {
            cerr << "Argument " << argv[argn] << " not recognized." << endl;
            usage();
        }
    }

    if (!fname)
        usage();

    SimulationParser sp = SimulationParser(fname, timeout, waitforfile);

    sp.addReceiver(&DRDVSParser);

    sp.init();

    sp.run();

    if (sp.mCurrentFullTimeStepKnown && !currentstepset)
        DRDVSParser.setCurrentStep(sp.mCurrentFullTimeStep);

    sp.final();

    // Return nonzero on error
    return (sp.OK()==0);
}
