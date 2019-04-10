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
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverQuadraticCons.hpp>

void usage()
{
    printf("MDQuadraticCons rdg1 rdg2 rdg3 ...\n");
    exit(0);
}

int
main(int argc, char **argv, char **envp)
{
    if (argc < 2)
      usage();

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
    int AbEnd;

    DataReceiverQuadraticCons *DRQC[100];
    int nfiles = 0;

    for (int i=0; i<argc-1; i++) {
      SimulationParser sp = SimulationParser(argv[i+1], timeout, waitforfile, quiet, debug, debugstart, skewwarn, seekahead);

      DRQC[i] = new DataReceiverQuadraticCons(delaystats, logperiod, bootstrap, allsnaps);

      sp.addReceiver(DRQC[i]);

      sp.init();

      sp.run();

      AbEnd = !sp.OK() || DRQC[i]->getFinalStatus();

      sp.final(AbEnd);

      if (AbEnd) {
	  cerr << "Error reading file " << argv[i+1] << endl;
	  return AbEnd;
      }

      // cout << argv[i] << " " << DRQC[i]->mTimeStep << " " << DRQC[i]->mCQs[0] << " " << DRQC[i]->mCQs.size() << endl;

      nfiles++;
    }

    double E0   = DRQC[0]->mCQs[0];
    double KE0  = DRQC[0]->mKEs[0];
    int    n0   = DRQC[0]->mCQs.size();
    double dt0  = DRQC[0]->mTimeStep;

    double sumsq = 0;

    for (int i=0; i<n0; i++) {
      double d = DRQC[0]->mCQs[i] - E0;
      sumsq += d*d;
    }

    double dE0 = sqrt(sumsq/n0);

    // cout << "dt0 E0 KE0 dE0" << endl;
    // cout << setprecision(10) << setw(15) << dt0 << " " << E0 << " " << KE0 << " " << dE0 << endl;

    double tscaling = dt0/0.001;
    double normde = dE0/tscaling/tscaling;

    cout << "dt E0 KE0 dE0Prev NormDE NormDETotE NormDEKE RelMaxDepartFromPrev sdRelDepartFromPrev RatioDiffPrev" << endl;
    cout << dt0 << " " << E0 << " " << KE0 << " 0 " << normde << " " << fabs(normde/E0) << " " << normde/KE0 << " 0.0 0.0 0.0" << endl;

    double dePrev = dE0;

    // in order from shortest tstep to longest
    for (int i=1; i<nfiles; i++) {
      DataReceiverQuadraticCons *qcprev = DRQC[i-1];
      DataReceiverQuadraticCons *qc = DRQC[i];

      sumsq = 0;
      double esumsq = 0;
      int tratio = (qc->mTimeStep/qcprev->mTimeStep + 0.5);
      double f = tratio*tratio;
      int nsteps = qc->mCQs.size()-1;
      double maxe = 0;

      for (int j=0; j<nsteps; j++) {
	double d = qc->mCQs[j] - qcprev->mCQs[0];  // departure from starting E
	sumsq += d*d;
	double e = d - f*(qcprev->mCQs[j*tratio] - qcprev->mCQs[0]);  // departure of scaled curves in current space
	esumsq += e*e;
	if (maxe < fabs(e))
	    maxe = fabs(e);

	// cout << i << " " << j*istep << " " << nsteps << " " << j << " " << f << " " << d << " " << e << endl;
      }
      double de = sqrt(sumsq/nsteps);  // sd of energies from baseline

      double sde = sqrt(esumsq/nsteps);  // sd of diff between scaled curves

      tscaling = qc->mTimeStep/0.001;  // scale to 1fs tstep
      normde = de/tscaling/tscaling;

      cout << qc->mTimeStep << " " << qc->mCQs[0] << " " << qc->mKEs[0] << " " << qc->mCQs[0] - qcprev->mCQs[0] << " "
	   << normde << " " << fabs(normde/qc->mCQs[0]) << " " << normde/qc->mKEs[0] << " "
	   << maxe/de << " " << sde/de << " " << de/dePrev/f - 1 << endl;

      dePrev = de;
    }

    // Return nonzero on error
    return (AbEnd);
}
