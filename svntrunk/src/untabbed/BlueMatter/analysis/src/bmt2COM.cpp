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
#include <stdio.h>
#include <iostream>
#include <string>

#include <BlueMatter/BMT.hpp>

#define BUFFSIZE 2048
#define FNAMESIZE 2048
#define PRECISION_DEF 32

using namespace std;

inline bool IsValidPrecision(int p) {
        return (p==16 || p==32 || p ==64 || p==0);
}


int main(int argc, char **argv, char **envp) {
    int arg0=1;
    char ifname[FNAMESIZE];
    char ofname[FNAMESIZE];

    strcpy(ofname,"");

    if (argc < 2) {
        cout << "bmt2COM <-o OutputFile> <-diffusion> <-previous PreviousCOMBMT> <-precision 32> BMTFile " << endl;
        return 1;
    }

    bool bDiffuse=false;
    char prevBMT[FNAMESIZE];
    int precision = PRECISION_DEF;
    strcpy(prevBMT,"");

    while (( *argv[arg0] == '-')&&(strlen (argv[arg0]) > 1)) {
  if (!strcmp(argv[arg0],"-o")) {
    strcpy(ofname,argv[++arg0]);
  }
  else if (!strcmp(argv[arg0],"-diffusion")) {
    bDiffuse=true;
  }
  else if (!strcmp(argv[arg0],"-previous")) {
    strcpy(prevBMT,argv[++arg0]);
  }
  else if (!strcmp(argv[arg0],"-precision")) {
    precision = atoi(argv[++arg0]);
  }
  arg0++;
    }
    strcpy(ifname,argv[arg0]);

    if(!IsValidPrecision(precision)) {
  cerr << " Warning: Argument -precision should be either 16,32,64 or 0" << endl;
  precision = PRECISION_DEF;
    }

    XYZ *prev = NULL;

    BMTReader bmtr(ifname);
    if(bDiffuse) {
  if(strcmp(prevBMT,"")!=0)  {
    BMTReader bmtr_prev(prevBMT);
    bmtr_prev.ReadLastFrame();
    prev = bmtr_prev.getPositions();
  }
    }


    BMTWriterBMT2COM bmtw(ifname,ofname,&bmtr,prev,precision);
    bmtw.setDiffusionFlag(bDiffuse);
    while(!bmtr.ReadFrame()) {
  bmtw.WriteReaderFrame(&bmtr);
    }
}

