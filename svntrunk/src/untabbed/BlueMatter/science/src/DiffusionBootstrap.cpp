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
#include <BlueMatter/JRand.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/TableFile.hpp>
#include <BlueMatter/SimpleTrajectory.hpp>

using namespace std ;

void Usage(char **argv)
{
    cerr << argv[0] << "f1.bin f2.bin f3.bin -start t1ns -stop t2ns -coherence t -cohmax tmax" << endl;
    cerr << "Input: list of binary input files with COM removed" << endl;
    cerr << "Output: Bootstrapped measurement of r^2 over time" << endl;
    cerr << "-start -stop specify range used for stats" << endl;
    cerr << "-coherence t causes bootstrap sample intervals based on t <1.0ns>" << endl;
    cerr << "Uses dt=0.25, 200 bootstrap ensembles" << endl;
    exit(-1);
}

int main(int argc, char **argv)
{    
    if (argc < 2)
  Usage(argv);

    int nfiles = 0;
    char *fnames[100];
    bool startset = false;
    bool stopset = false;
    float start;
    float stop;
    float tcoherence = 1.0;
    float tcohmax = 0.0;

    int argn=1;
    while (argn < argc) {
  if (!strcmp(argv[argn], "-start")) {
      start = atof(argv[++argn]);
      startset = true;
  } else if (!strcmp(argv[argn], "-stop")) {
      stop = atof(argv[++argn]);
      stopset = true;
  } else if (!strcmp(argv[argn], "-coherence")) {
      tcoherence = atof(argv[++argn]);
  } else if (!strcmp(argv[argn], "-cohmax")) {
      tcohmax = atof(argv[++argn]);
  } else if (argv[argn][0] != '-') {
      fnames[nfiles++] = argv[argn];
  } else {
      cerr << "arg " << argv[argn] << " not recognized" << endl;
      Usage(argv);
  }
  ++argn;
    }

    
    TrajectoryLipidSet tls;
    
    for (int nf = 0; nf<nfiles; nf++)
  tls.loadTrajectoryFile(fnames[nf]);
    
    double dt = 0.25;
    double t0 = dt;

    if (!startset)
  start = tls.m_StartTime;
    if (!stopset)
  stop = tls.m_EndTime;
    
    double tTot = stop - start;
    double tMaxSampleFrom = tTot;
    double tTimeRange = tls.m_EndTime - tls.m_StartTime;
    
    jrand jrlower(tls.m_NLower);
    jrand jrupper(tls.m_NUpper);
    jrand jrall(tls.m_NItems);
    jrand junit;

    Column lower, upper, all;
    Column blower, bupper, ball;
    
    int nboot = 200;
    int maxsamples = 1000;

    int ncohsteps = 1;
    float tcohmin = tcoherence;
    float tcohstep = 0.2;
    if (tcohmax > 0) {
  tcohmin = 0.2;
  ncohsteps = (tcohmax-tcohmin)/tcohstep + 0.5;
  t0 = 10.0;
    }

    for (int ncohstep = 0; ncohstep < ncohsteps; ncohstep++) {
    tcoherence = tcohmin+ncohstep*tcohstep;
  

    int ncoherence = fabs(tcoherence/tls.m_Trajectory[0]->m_TimeStep);


    for (double t=t0; t<tTot; t+= dt) {
  int ntimesamples = tls.m_NSteps*(tTimeRange-t);
  //if (ntimesamples > maxsamples)
  //    ntimesamples = maxsamples;

  for (int b=0; b<nboot; b++) {
      XYZ delta;
      int n;
      int samplefactor, nsamples;

      samplefactor = tls.m_NLower;
      nsamples = (ntimesamples*samplefactor)/ncoherence;
      for (n=0; n<nsamples; n++) {
    int llower = jrlower.get();
    float tstart = junit.fget()*(tMaxSampleFrom-t)+start;
    while (!tls.getDelta(llower, tstart, t, delta))
              tstart = junit.fget()*(tMaxSampleFrom-t)+start;
    lower.addValue(delta.mX*delta.mX + delta.mY*delta.mY);
      }

      samplefactor = tls.m_NUpper;
      nsamples = (ntimesamples*samplefactor)/ncoherence;
      for (n=0; n<nsamples; n++) {
    int lupper = jrupper.get()+tls.m_NLower;
    float tstart = junit.fget()*(tMaxSampleFrom-t)+start;
    while (!tls.getDelta(lupper, tstart, t, delta))
              tstart = junit.fget()*(tMaxSampleFrom-t)+start;
    upper.addValue(delta.mX*delta.mX + delta.mY*delta.mY);
      }

      samplefactor = tls.m_NUpper+tls.m_NLower;
      nsamples = (ntimesamples*samplefactor)/ncoherence;
      for (n=0; n<nsamples; n++) {
    int lall = jrall.get();
    float tstart = junit.fget()*(tMaxSampleFrom-t)+start;

    while (!tls.getDelta(lall, tstart, t, delta))
              tstart = junit.fget()*(tMaxSampleFrom-t)+start;
    all.addValue(delta.mX*delta.mX + delta.mY*delta.mY);
      }

      // for (int i=0; i<lower.m_Data.size(); i++)
    // cout << lower.m_Data[i] << endl;
      lower.doStats();
      upper.doStats();
      all.doStats();
      
      blower.addValue(lower.m_Mean);
      bupper.addValue(upper.m_Mean);
      ball.addValue(all.m_Mean);
      
      lower.flush();
      upper.flush();
      all.flush();
      
  }
  blower.doStats();
  bupper.doStats();
  ball.doStats();

  if (ncohsteps > 1)
      cout << tcoherence << " ";
  
  cout << t << " " << blower.m_Mean << " " << blower.m_Sigma
      << " " << bupper.m_Mean << " " << bupper.m_Sigma
      << " " <<   ball.m_Mean << " " <<   ball.m_Sigma << endl;
  
  blower.flush();
  bupper.flush();
  ball.flush();
  if (ncohsteps > 1)
      break;
    }
    }
        
    return 0;
}
