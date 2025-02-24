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

using namespace std;

int main(int argc, char **argv, char **envp) {
    int arg0=1;
    bool bRecenter=false;
    char ifname[FNAMESIZE];
    char ofname[FNAMESIZE];

    strcpy(ifname,"");
    strcpy(ofname,"");

    if (argc < 2) {
        cout << "bmtmapintobox InputBMTFileName OutputBMTFileName" << endl;
        return 1;
    }

    strcpy(ifname,argv[arg0]);
    strcpy(ofname,argv[++arg0]);

    BMTReader bmtr(ifname);
    BMTWriter bmtw(ofname,bmtr);

    bmtw.setForceWrite(true);
//    bmtw.setMapIntoBox(true);

    int fr = 0;
    while(!bmtr.ReadFrame()) {
	bmtw.WriteReaderFrame(&bmtr);
        printf("Wrote frame %d\n",fr++);
    }
    

}

