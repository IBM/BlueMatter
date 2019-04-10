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
#include <cstdlib>
#include <memory.h>

using namespace std ;
#include <BlueMatter/ExternalDatagram.hpp>
// #include <pk/fxlogger.hpp>
// #include <BlueMatter/MolSysDef.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/DataReceiverSimpleLogger.hpp>
#include <BlueMatter/DataReceiverFilterWindow.hpp>
#include <BlueMatter/DataReceiverTimeStamper.hpp>

void usage()
{
    printf("MDLogger <rawdatagramfile> <-delaystats N> <-timeout N> <-seekahead N> <-logperiod N> <-bootstrap> <-allsnapshots>\n");
    // printf("-width N specifies widow averaging of results\n");
    printf("-delaystats N causes RMSD values to skip data from first N steps\n");
    printf("-timeout N causes timeout after N seconds (default is 300)\n");
    printf("-logperiod N causes output of every Nth step\n");
    printf("-bootstrap causes bootstrap-driven rmsdrmsd calculation, shown in Run.final\n");
    printf("-allsnapshots causes all snapshots to be saved and numbered\n");
    printf("-quiet causes supression of debug warnings.  USE WITH CARE.\n");
    printf("-debug produces verbose parsing info to stderr.\n");
    printf("-debugstart N causes verbosity to begin after timestep N.\n");
    printf("-skewwarn N causes warnings to begin when frames in mem exceed n (2..252)\n");
    printf("-seekahead N causes read to lag behind end of file as it grows\n");
    printf("-snapnamestem foo causes snapshots to be named foo.dvs, or foo00001.dvs if allsnaps on\n");
    exit(0);
}

int
main(int argc, char **argv, char **envp)
{
      if (argc < 2)
        usage();

    char *fname = "-";
    int width = 0;
    int delaystats = 0;
    int logperiod = 1;
    int timeout = 300;
    int waitforfile = 1;
    int bootstrap = 0;
    int allsnaps = 0;
    int quiet = 0;
    int debug = 0;
    int debugstart = -1;
    int skewwarn = 245;
    int seekahead = 0;
    int snapnameset = 0;
    char *snapnamestem = NULL;

    for (int argn = 1; argn < argc; argn++) {
        char *s = argv[argn];
        if (*s != '-') {
            fname = s;
        } else if (!strcmp(s, "--")) {
	  // read from stdin
        } else if (!strcmp(s, "-width")) {
            width = atoi(argv[++argn]);
        } else if (!strcmp(s, "-delaystats")) {
            delaystats = atoi(argv[++argn]);
        } else if (!strcmp(s, "-timeout")) {
            timeout = atoi(argv[++argn]);
        } else if (!strcmp(s, "-seekahead")) {
            seekahead = atoi(argv[++argn]);
        } else if (!strcmp(s, "-logperiod")) {
            logperiod = atoi(argv[++argn]);
        } else if (!strcmp(s, "-skewwarn")) {
            skewwarn = atoi(argv[++argn]);
        } else if (!strcmp(s, "-snapnamestem")) {
            snapnamestem = argv[++argn];
	    snapnameset = 1;
        } else if (!strcmp(s, "-debug")) {
            debug = 1;
        } else if (!strcmp(s, "-debugstart")) {
            debugstart = atoi(argv[++argn]);
            debug = 1;
        } else if (!strcmp(s, "-bootstrap")) {
            bootstrap = 1;
        } else if (!strcmp(s, "-allsnapshots")) {
            allsnaps = 1;
        } else if (!strcmp(s, "-quiet")) {
            quiet = 1;
        } else {
          usage();
        }
    }

    if (!fname || width < 0 || delaystats < 0 || logperiod < 1)
        usage();

    SimulationParser sp = SimulationParser(fname, timeout, waitforfile, quiet, debug, debugstart, skewwarn, seekahead);
    DataReceiverFilterWindow DRFilterWindow = DataReceiverFilterWindow(width, delaystats);
    DataReceiverSimpleLogger DRSimpleLogger(delaystats, logperiod, bootstrap, allsnaps);
    if (snapnameset)
	DRSimpleLogger.SetSnapshotName(snapnamestem);
    // DataReceiverTimeStamper  DRStamper(1, 1);

    if (width > 0) {
        sp.addReceiver(&DRFilterWindow);
    } else {
        sp.addReceiver(&DRSimpleLogger);
    }

    // sp.addReceiver(&DRStamper);

    sp.init();

    // Frame *pFrame;
    // sp.getFrame(&pFrame);

    sp.run();

    int AbEnd = !sp.OK() || DRSimpleLogger.getFinalStatus();

    sp.final(AbEnd);

    if (access("SnapshotFinal.dvs", 0) != -1)
        unlink("SnapshotFinal.dvs");

    // Only rename if ended ok
    if (access("Snapshot.dvs", 0) != -1 && !AbEnd && !snapnameset)
        rename("Snapshot.dvs", "SnapshotFinal.dvs");

    // Return nonzero on error
    return (AbEnd);
}
