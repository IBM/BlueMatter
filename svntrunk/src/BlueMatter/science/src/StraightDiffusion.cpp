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
 #include <math.h>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>
#include <BlueMatter/Column.hpp>

using namespace std ;

int main(int argc, char **argv)
{
    //const int nperleaf=36;
    // const int nperleaf=12;
    // const int nlip = 2*nperleaf;

    if (argc < 2) {
	cerr << argv[0] << "f1.bin f2.bin f3.bin" << endl;
	cerr << "Input: list of binary input files with COM removed" << endl;
	cerr << "Output: Lipid-averaged measurement of r^2 over time" << endl;
	// cerr << "Uses dt=0.25, 200 bootstrap ensembles with 2000 samples each" << endl;
	exit(-1);
    }
 
    TrajectoryLipidSet tls;
    
    for (int nf = 1; nf<argc; nf++)
	tls.loadTrajectoryFile(argv[nf]);
    
    // after loading the trajectories, just want straight 2d diffusion of each
    // output lmean, lstd, umean, ustd, allmean, allstd

    Column *diff = new Column[tls.m_NItems];
    Column times;
    float resolution = 0.25;
    tls.diffusion2d(diff, times, resolution, 1.5);

    int nrows = diff[0].m_NPts;

    int dumpraw = 0;

    for (int i=0; i<nrows; i++) {
	double t = times.m_Data[i];
	Column low, up, all;
	if (dumpraw)
	    cout << t << " ";
	for (int n=0; n<tls.m_NItems; n++) {
	    if (n<tls.m_NLower) {
		low.addValue(diff[n].m_Data[i]);
	    } else {
		up.addValue(diff[n].m_Data[i]);
	    }
    	    if (dumpraw)
		cout << diff[n].m_Data[i] << " ";
	    all.addValue(diff[n].m_Data[i]);
	}
	if (dumpraw)
	    cout << endl;

	low.doStats();
	up.doStats();
	all.doStats();

	if (!dumpraw) {
	    cout << t << " " << low.m_Mean << " " << low.m_Sigma << " " << up.m_Mean << " "
		 << up.m_Sigma << " " << all.m_Mean << " " << all.m_Sigma << endl;
	}
	
	low.flush();
	up.flush();
	all.flush();
    }

    return 0;
}