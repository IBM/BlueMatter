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
 // *********************************************************************
// File: DataReceiverMomentumMonitor.cpp
// Author: RSG
// Date: March 15, 2003
// Class: DataReceiverMomentumMonitor
// Inheritance: DataReceiverDB2
// Description: DataReceiver class computes the total linear momentum
//              for each frame and outputs it to stdout
// *********************************************************************

#include <iostream>
#include <iomanip>
using namespace std ;
#include <BlueMatter/DataReceiverMomentumMonitor.hpp>

#include <BlueMatter/Frame.hpp>
#include <BlueMatter/XYZ.hpp>

DataReceiverMomentumMonitor::DataReceiverMomentumMonitor(const char* dbName,
                                                         const int sysId)
  : DataReceiverDB2(dbName, sysId)
{
  mOldMomentum.Zero();
}

void DataReceiverMomentumMonitor::sites(Frame* frame)
{
  double mass = 0; // could compute total mass once at initialization
  XYZ cmPos;
  cmPos.Zero();
  XYZ pTot;
  pTot.Zero();

  if (molecularSystemPointer() == NULL)
    {
      DataReceiver::sites(frame);
      return;
    }

  const std::vector<db2::Site>& site = molecularSystem().sites().asVector();

  if (frame->mSiteData.getSize() != site.size())
    {
      std::cerr << "ERROR: size mismatch between site arrays in db2 and frame: "
                << "at timestep " << frame->mOuterTimeStep
                << " frame->mSiteData.getSize() = " << frame->mSiteData.getSize()
                << " site.size() = " << site.size()
                << std::endl;
      DataReceiver::sites(frame);
      return;
    }
  for (int i = 0; i < frame->mSiteData.getSize(); ++i)
    {
      mass += site[i].atomicMass();
      cmPos += frame->mSiteData[i].mPosition*site[i].atomicMass();
      pTot += frame->mSiteData[i].mVelocity*site[i].atomicMass();
    }

  cmPos = cmPos/mass;
  std::cout.precision(14);
  std::cout << std::scientific;
  std::cout << frame->mOuterTimeStep << " "
            << std::setw(21) << cmPos.mX << " "
            << std::setw(21) << cmPos.mY << " "
            << std::setw(21) << cmPos.mZ << " "
            << std::setw(21) << pTot.mX << " "
            << std::setw(21) << pTot.mY << " "
            << std::setw(21) << pTot.mZ << " "
            << std::setw(21) << pTot.mX - mOldMomentum.mX << " "
            << std::setw(21) << pTot.mY - mOldMomentum.mY << " "
            << std::setw(21) << pTot.mZ - mOldMomentum.mZ << " "
            << std::endl;
  mOldMomentum =  pTot;
  std::cout.flush();

  //  DataReceiver::logInfo(frame);
}
