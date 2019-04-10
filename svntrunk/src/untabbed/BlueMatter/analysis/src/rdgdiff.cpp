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

#include <BlueMatter/ExternalDatagram.hpp>
// #include <BlueMatter/MolSysDef.hpp>

// #include <pk/fxlogger.hpp>

#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>

void
PrintUsage()
{
    cout << "rdgdiff [-verbose -debug -summary -sumsummary -deltas -sigfigs -values -sites -skip1=N -skip2=N -nsteps=N] rdgfile1 rdgfile2" << endl;
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
            PrintUsage();
      return -1;
  }

        int ReportMode = 0;
        int Verbose = SimulationParser::REPORT_ABSOLUTE | SimulationParser::REPORT_SIGFIGS | 
                      SimulationParser::REPORT_VALUES | SimulationParser::REPORT_SITES | SimulationParser::REPORT_SUMMARY;

#define SKIP1 "-skip1="
#define SKIP2 "-skip2="
#define NSTEPS "-nsteps="

        for (int i=1; i<argc; i++) {
            char *s = argv[i];
            char *p;
            if (*s == '-') {
                if (!strcmp(s, "-verbose"))
                    ReportMode |= Verbose;
                else if (!strcmp(s, "-sigfigs"))
                    ReportMode |= SimulationParser::REPORT_SIGFIGS;
                else if (!strcmp(s, "-values"))
                    ReportMode |= SimulationParser::REPORT_VALUES;
                else if (!strcmp(s, "-sites"))
                    ReportMode |= SimulationParser::REPORT_SITES;
                else if (!strcmp(s, "-summary"))
                    ReportMode |= SimulationParser::REPORT_SUMMARY;
                else if (!strcmp(s, "-sumsummary"))
                    ReportMode |= SimulationParser::REPORT_SUMSUMMARY;
                else if (!strcmp(s, "-deltas"))
                    ReportMode |= SimulationParser::REPORT_ABSOLUTE;
                else if (!strncmp(s, SKIP1, strlen(SKIP1)))
                    NSkip1 = atoi(&s[strlen(SKIP1)]);
                else if (!strncmp(s, SKIP2, strlen(SKIP2)))
                    NSkip2 = atoi(&s[strlen(SKIP2)]);
                else if (!strncmp(s, NSTEPS, strlen(NSTEPS)))
                    NSteps = atoi(&s[strlen(NSTEPS)]);
                else if (!strcmp(s, "-debug"))
                    debug = 1;
                else {
                    cerr << "Command line argument " << s << " not recognized" << endl;
                    PrintUsage();
                    exit(-1);
                }
            } else {
                if (!fname1)
                    fname1 = s;
                else if (!fname2)
                    fname2 = s;
                else {
                    cerr << "Too many files specified: " << fname1 << " " << fname2 << " " << s << endl;
                    PrintUsage();
                    exit(-1);
                }
            }
        }

        if (!fname1 || !fname2) {
            PrintUsage();
            exit(-1);
        }
                    
  tEnergyInfo *pe1, *pe2;
  Frame *pf1, *pf2;

  SimulationParser sp1 = SimulationParser(fname1, 1, 0, 0, debug);
  sp1.setTolerances(12.5, 12.5, 12.5);
        if (!sp1.OK())
            cout << "Error opening file " << fname1 << endl;
  SimulationParser sp2 = SimulationParser(fname2, 1, 0, 0, debug);
  sp2.setTolerances(12.5, 12.5, 12.5);
        if (!sp2.OK())
            cout << "Error opening file " << fname2 << endl;

        if (!sp1.OK() || !sp2.OK()) {
            return -1;
        }

  sp1.init();
  sp2.init();
        int HeaderCounter = 0;

        while (NSkip1 > 0 && !sp1.done()) {
            sp1.getFrame(&pf1);
            NSkip1--;
        }

        while (NSkip2 > 0 && !sp2.done()) {
            sp2.getFrame(&pf2);
            NSkip2--;
        }

        // some ugliness here so that summary can convey info in last 2 frames
        Frame F1, F2;
        int FramesAssigned = 0;
  int FramesCompared = 0;
  while (!sp1.done() && !sp2.done() && NSteps != 0) {
    pf1 = pf2 = NULL;
    sp1.getFrame(&pf1);
    sp2.getFrame(&pf2);
    // Make sure they both ended at same time - otherwise different lengths
    if ((!pf1 && pf2) || (pf1 && !pf2)) {
        diff |= 8;
        break;
    }
    if (!pf1 || !pf2)
        break;
    // have 2 good frames to compare, so bump compared
    FramesCompared++;
    // cerr << "before assign" << endl;
    // cerr << "nsites: " << pf1->mSiteData.getSize() << " " << pf2->mSiteData.getSize() << endl;
    // cerr << "first site: " << pf1->mSiteData[0].mPosition << " " << pf2->mSiteData[0].mPosition << endl;
    if (ReportMode & SimulationParser::REPORT_SUMMARY) {
        if (FramesAssigned) {
      delete &F1;
      delete &F2;
        }
        F1 = *pf1;
        F2 = *pf2;
        // cerr << "after assign" << endl;
        // cerr << "nsites: " << F1.mSiteData.getSize() << " " << F2.mSiteData.getSize() << endl;
        // cerr << "first site: " << F1.mSiteData[0].mPosition << " " << F2.mSiteData[0].mPosition << endl;
        FramesAssigned = 1;
    }
                int DoHeader = (ReportMode & SimulationParser::REPORT_LOGGING && HeaderCounter%10 == 0) ? SimulationParser::REPORT_HEADER : 0;
                HeaderCounter++;
    diff |= sp1.framesEqual(pf1, pf2, ReportMode | DoHeader);
    // if (diff |= sp1.framesEqual(pf1, pf2, ReportMode | DoHeader)) {
      // diff = 1;
      // break;
    // }
                NSteps--;
  }

  if (sp1.done() != sp2.done())
    diff != 8;

  sp1.final();
  sp2.final();

        if (ReportMode & SimulationParser::REPORT_SUMMARY) {
            cout << "# rdgdiff summary for files " << fname1 << " " << fname2 << endl;
            sp1.reportSummary();
            cout << endl;
            cout << "# Final snapshot diff:" << endl;
            if (FramesAssigned) {
    if (debug) {
        cerr << "nsites: " << F1.mSiteData.getSize() << " " << F2.mSiteData.getSize() << endl;
        cerr << "first site: " << F1.mSiteData[0].mPosition << " " << F2.mSiteData[0].mPosition << endl;
    }
    sp1.framesEqual(&F1, &F2, Verbose | SimulationParser::REPORT_HEADER);
            } else {
                cout << "Two frames not assigned, so no direct diff available" << endl;
            }
        }

        if (ReportMode & SimulationParser::REPORT_SUMSUMMARY) {
            diff |= sp1.reportSumSummary();
        }

        int sumdiff = sp1.checkSummaryEnergyTolerance();
        sumdiff |= sp1.checkSummaryPositionVelocityTolerance();

        if (FramesCompared) {
      printf("# FRAMES COMPARED: %d\n", FramesCompared);
      if (!sumdiff)
    printf("# Datagram streams IDENTICAL\n");
      else if (sumdiff < 2)
    printf("# Datagram streams WITHIN TOLERANCE\n");
      else if (sumdiff & 2)
    printf("# Datagram streams OUT OF TOLERANCE\n");

      if (diff & 4)
    printf("# Datagram streams CONTAIN NAN's\n");

      if (diff & 8)
    printf("# Datagram streams DIFFERENT LENGTHS\n");
  } else {
      printf("# NO FRAMES COMPARED!!!  ONE OR MORE FILES IS CORRUPT OR UNREADABLE!!  NO COMPARISON MADE!!  CHECK FILE AND PACKET INTEGRITY!!\n");
  }

  sumdiff |= diff;
      
  return sumdiff;
}
