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
//#include <fstream>

#include <BlueMatter/BMT.hpp>

#define BUFFSIZE 2048

using namespace std;

int main(int argc, char **argv, char **envp) {

    char buff[BUFFSIZE];
    bool last=false;
    int arg0=1;

    if (argc < 2) {
        cout << "bmttest BMTFileName" << endl;
        return 1;
    }

    char fname[1000];
    strcpy(fname,argv[arg0]);
    BMTReader bmtr(fname);

    int NStartAtomIndex, NEndAtomIndex;
    cout << "getting some attributes" << endl;
    bmtr.getAttrib("STARTATOMINDEX",&NStartAtomIndex);
    bmtr.getAttrib("ENDATOMINDEX",&NEndAtomIndex);

    int NAtoms = NEndAtomIndex - NStartAtomIndex + 1;
    int NEntries = NAtoms;
    
    cout << "getting frame attribute vector" << endl;
    vector<NameTypeHandle> vec_NTH = bmtr.getFrameAttributeVector();

    cout << "Frame Attributes: " << endl;
    vector<NameTypeHandle>::iterator it=vec_NTH.begin();
    for(;it!=vec_NTH.end();it++) {
	cout << it->name << " : " << NameTypeHandle::getTypeStr(it->type) << endl;
    }

    int NFrames = bmtr.getNFrames();
    cout << "NFrames: " << NFrames << endl;

    int n = NFrames/2;
    cout << "Frame " << n << endl;
    bmtr.ReadFrame(n);
    it=vec_NTH.begin();
    for(;it!=vec_NTH.end();it++) {
	    cout << *it << endl;
    }

    bmtr.Rewind();
    cout << "After Rewind" << endl;
    it=vec_NTH.begin();
    for(;it!=vec_NTH.end();it++) {
	    cout << *it << endl;
    }

    bmtr.ReadFrame(-2);
    cout << "Frame -2" << endl;
    it=vec_NTH.begin();
    for(;it!=vec_NTH.end();it++) {
	    cout << *it << endl;
    }


    bmtr.ReadFrame(-1);
    cout << "Frame -1" << endl;
    // first read the frame header attributes
    it=vec_NTH.begin();
    for(;it!=vec_NTH.end();it++) {
	    cout << *it << endl;
    }


    bmtr.closeFileHandle();
}
