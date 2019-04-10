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
        cout << "bmt2txt <-last> BMTFileName" << endl;
        return 1;
    }

    while (( *argv[arg0] == '-')&&(strlen (argv[arg0]) > 1)) {
        if (!strcmp(argv[arg0++], "-last")) {
            last = true;
  }
    }

    char fname[1000];
    strcpy(fname,argv[arg0]);
    BMTReader bmtr(fname);

    // prints out all the BMT header attributes
    bmtr.printAttrib(NameTypeHandle::ALL);


    // read the header for frame format and computing the number of atoms

    int NStartAtomIndex, NEndAtomIndex;
    bmtr.getAttrib("STARTATOMINDEX",&NStartAtomIndex);
    bmtr.getAttrib("ENDATOMINDEX",&NEndAtomIndex);

    int NAtoms = NEndAtomIndex - NStartAtomIndex + 1;
    int NEntries = NAtoms;
    
    vector<NameTypeHandle> vec_NTH = bmtr.getFrameAttributeVector();

    cout << "Frame Attributes: " << endl;
    vector<NameTypeHandle>::iterator it=vec_NTH.begin();
    for(;it!=vec_NTH.end();it++) {
  cout << it->name << " : " << NameTypeHandle::getTypeStr(it->type) << endl;
    }

    cout << "Frames: " << endl;

    // Before printing out the frames just make sure there is no other
    //	tag along header stuff.. like the four floats that will be there
    //	if the frames are of type "short" ie., 16-bit trajectory format

    string format;
    bmtr.getAttrib("TRAJECTORYFORMAT",&format);

    bool b16bit=false;
    if(format.find("16")!=string::npos) b16bit=true;

    bool bVelocities=bmtr.getIncludeVelocities();

    bool bCOMTraj = bmtr.IsCOMTrajectory();

    if(bCOMTraj) {
  ConnectivityString cs;
  bmtr.getAttrib("CONNECTIVITY",&cs);
  NEntries = cs.getNumberOfMolecules();
    }

    while(1) {

      if(last) {
        if(bmtr.ReadLastFrame()) break;
  }
  else if(bmtr.ReadFrame() ) break;
  

  cout << endl;

  // first read the frame header attributes
      it=vec_NTH.begin();
      for(;it!=vec_NTH.end();it++) {
    cout << *it << endl;
      }

  if(b16bit) {
    cout << bmtr.getOrigin() << " : " << bmtr.getRecipFactor() << endl;
  }

  XYZ *pos = bmtr.getPositions();
        for (int i=0; i<NEntries; i++) {
              sprintf(buff, "%16.8lf %16.8lf %16.8lf", pos[i].mX, pos[i].mY, pos[i].mZ);
              cout << buff << endl;
        }

  if(!bVelocities) {
    if(last) break;
    continue;
  }

  cout << endl;

  XYZ *vel = bmtr.getVelocities();
        for (int i=0; i<NEntries; i++) {
              sprintf(buff, "%16.8lf %16.8lf %16.8lf", vel[i].mX, vel[i].mY, vel[i].mZ);
              cout << buff << endl;
        }

  if(last) break;
    }

} // end main



    







