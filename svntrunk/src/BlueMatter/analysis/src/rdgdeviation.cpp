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
 // **********************************************************************
// File: rdgdeviation.cpp
// Date: May 23, 2003
// Description: Adaptation of rdgdiff.cpp to report the sum of the
// squared differences in position and velocity over all sites for
// each time step in two trajectories.
// **********************************************************************

#include <assert.h>
#include <fcntl.h>
#include <cstdio>
#include <iomanip>
#include <iostream>

using namespace std ; 

#include <BlueMatter/ExternalDatagram.hpp>

//#include <pk/fxlogger.hpp>

#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>

void
PrintUsage(const char* argv0)
{
  cout << argv0 << " [-debug -skip1=N -skip2=N -nsteps=N] rdgfile1 rdgfile2" << endl;
}


int
main(int argc, char **argv, char **envp)
{
	char *fname1 = NULL;
        char *fname2 = NULL;
        int NSkip1 = 0;
        int NSkip2 = 0;
        int NSteps = -1;
	int diff = 0;
	int debug = 0;

	if (argc < 3) {
            PrintUsage(argv[0]);
	    return -1;
	}

        int ReportMode = 0;

#define SKIP1 "-skip1="
#define SKIP2 "-skip2="
#define NSTEPS "-nsteps="

        for (int i=1; i<argc; i++) {
            char *s = argv[i];
            char *p;
            if (*s == '-') {
                if (!strncmp(s, SKIP1, strlen(SKIP1)))
                    NSkip1 = atoi(&s[strlen(SKIP1)]);
                else if (!strncmp(s, SKIP2, strlen(SKIP2)))
                    NSkip2 = atoi(&s[strlen(SKIP2)]);
                else if (!strncmp(s, NSTEPS, strlen(NSTEPS)))
                    NSteps = atoi(&s[strlen(NSTEPS)]);
                else if (!strcmp(s, "-debug"))
                    debug = 1;
                else {
                    cerr << "Command line argument " << s << " not recognized" << endl;
                    PrintUsage(argv[0]);
                    exit(-1);
                }
            } else {
                if (!fname1)
                    fname1 = s;
                else if (!fname2)
                    fname2 = s;
                else {
                    cerr << "Too many files specified: " << fname1 << " " << fname2 << " " << s << endl;
                    PrintUsage(argv[0]);
                    exit(-1);
                }
            }
        }

        if (!fname1 || !fname2) {
            PrintUsage(argv[0]);
            exit(-1);
        }
                    
	Frame *pf1, *pf2;

	SimulationParser sp1 = SimulationParser(fname1, 1, 0, 0, debug);
        if (!sp1.OK())
            cout << "Error opening file " << fname1 << endl;
	SimulationParser sp2 = SimulationParser(fname2, 1, 0, 0, debug);
        if (!sp2.OK())
            cout << "Error opening file " << fname2 << endl;

        if (!sp1.OK() || !sp2.OK()) {
            return -1;
        }

	sp1.init();
	sp2.init();

        while (NSkip1 > 0 && !sp1.done()) {
            sp1.getFrame(&pf1);
            NSkip1--;
        }

        while (NSkip2 > 0 && !sp2.done()) {
            sp2.getFrame(&pf2);
            NSkip2--;
        }

	while (!sp1.done() && !sp2.done() && NSteps != 0) {
		pf1 = pf2 = NULL;
		sp1.getFrame(&pf1);
		sp2.getFrame(&pf2);

		// get the site data
		if (pf1->mNSites != pf2->mNSites) {
		  cerr << "ERROR: site count mismatch at timesteps: "
		       << pf1->mOuterTimeStep << " "
		       << pf2->mOuterTimeStep << endl;
		  return(-1);
		}
		
		// check to see if both frames contain sites
		if ((pf1->mSiteData.getSize() != pf1->mNSites) ||
		    (pf2->mSiteData.getSize() != pf2->mNSites)) {
		  continue;
		}

		if (pf1->mSiteData.hasGaps()) {
		  cerr << "Site Data has gaps for pf1" << endl;
		  break;
		}

		if (pf2->mSiteData.hasGaps()) {
		  cerr << "Site Data has gaps for pf2" << endl;
		  break;
		}

		if (pf1->mOuterTimeStep !=
		    pf2->mOuterTimeStep) {
		  continue;
		}

		const tSiteData* tsd1 = pf1->mSiteData.getArray();
		const tSiteData* tsd2 = pf2->mSiteData.getArray();

		double sumDeltaPos2 = 0;
		double sumDeltaVel2 = 0;

		for (int i = 0; i < pf1->mNSites; ++i)
		  {
		    double deltaPos2 =
		      //		      tsd1->mPosition.DistanceSquared(tsd2->mPosition);
		      pf1->mSiteData[i].mPosition.DistanceSquared(pf2->mSiteData[i].mPosition);
		    sumDeltaPos2 += deltaPos2;

		    double deltaVel2 =
		      // tsd1->mVelocity.DistanceSquared(tsd2->mVelocity);
		      pf1->mSiteData[i].mVelocity.DistanceSquared(pf2->mSiteData[i].mVelocity);
		    sumDeltaVel2 += deltaVel2;
		    if (debug) {
		      if ((deltaPos2 != 0) || (deltaVel2 != 0)) {
			cout << "ts: " << pf1->mOuterTimeStep << " ";
			cout << pf2->mOuterTimeStep << " ";
			cout << "site: " << i << " " << endl;
			cout << "file1: " << pf1->mSiteData[i].mPosition << " ";
			cout << pf1->mSiteData[i].mVelocity << " " << endl;
			cout << "file2: " << pf2->mSiteData[i].mPosition << " ";
			cout << pf2->mSiteData[i].mVelocity << " " << endl;
			cout << "deltaPos2 = " << deltaPos2 << " ";
			cout << "deltaVel2 = " << deltaVel2 << " " << endl;
		      }
		    }

		    ++tsd1;
		    ++tsd2;
		  }
		cout << setw(10) << pf1->mOuterTimeStep << " ";
		cout << setw(10) << pf2->mOuterTimeStep << " ";
		cout << setw(20) << setprecision(15) << sumDeltaPos2 << " ";
		cout << setw(20) << setprecision(15) << sumDeltaVel2 << " ";
		cout << endl;

		if (!pf1 || !pf2)
		    break;
                NSteps--;
	}


	sp1.final();
	sp2.final();

	    
	return(0);
}
