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
 #ifndef DATARECEIVERTRAJECTORYDUMPER_HPP
#define DATARECEIVERTRAJECTORYDUMPER_HPP

#include <ostream>
#include <fstream>
#include <vector>
// AK: Now include DRSimpleLogger instead
//#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/DataReceiverSimpleLogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/BMT.hpp>

using namespace std;

// AK: Inherit from DRSimpleLogger instead, to get energy and stuff
//		(basically the logging utility such as logstats)
//class DataReceiverTrajectoryDumper : public DataReceiver


class DataReceiverTrajectoryDumper : public DataReceiverSimpleLogger
{
protected:
    BMTWriterRDG2BMT mBMT;

public:
    DataReceiverTrajectoryDumper()
    {
  mUDFsKnown=1;
    }

   // AK: Added an argument for multiple precision sampling rate
    void setParams(char *BMTFileName, char *connectivity, int DoAppend=0, int NoWaters=0, int SimulationStart=0, int Reverse=0,int SkipFactor=1,int PrecisionSpecifier=16,int Velocities=0,bool MapIntoBox=true, bool Recenter=true, int FramesPerBMT=0)
    {
  mBMT.setParams(BMTFileName, connectivity, DoAppend, NoWaters, SimulationStart, Reverse,SkipFactor,PrecisionSpecifier,Velocities,MapIntoBox,Recenter,FramesPerBMT);
    }

   // kept for backward compatibility
    void setParams(char *BMTFileName, char *PDBFileName, int NNonWaterAtoms, int NWaters, char *connectivity, int DoAppend=0, int NoWaters=0, int SimulationStart=0, int Reverse=0,int SkipFactor=1,int PrecisionSpecifier=16,int Velocities=0) {
        mBMT.setParamsOld(BMTFileName, PDBFileName, connectivity, NNonWaterAtoms, NWaters, DoAppend, NoWaters, SimulationStart, Reverse,SkipFactor,PrecisionSpecifier,Velocities);
    }



    virtual void init()
    {
    }

    virtual void final(int status=1)
    {
  mBMT.Close();
    }

    // called when sites arrive
   // AK : New function call
    virtual void sites(Frame *f)
    {
  // AK: Added for reading energy and other frame attributes
  doLogStats(f);
  
  mBMT.WriteFrame(f);

    }


    virtual void logInfo(Frame *f) {
  DONEXT_1(logInfo,f);
    }

    // AK: dummy function (mickey for testing BMTReader) 
    //		remove after testing
    void ak_test_bmtreader(char *fname) {
  cout << "AK_debug: ak_test_bmtreader called\n";
  cout << "AK_debug: ak_test_bmtreader called with " << fname << endl;
  BMTReader bmt(fname);
  
  bmt.printAttrib(NameTypeHandle::ALL);
  bmt.ReadFrame();
  bmt.getPositions();

  char s[100];
  strcpy(s,"new10");
  BMTWriter bmtw(s,bmt);
  //bmtw.WriteFrame();
    }

};

#endif
