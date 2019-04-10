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
// File: DataReceiverEquipartitionMonitor.cpp
// Author: RSG
// Date: March 15, 2003
// Class: DataReceiverEquipartitionMonitor
// Inheritance: DataReceiverDB2
// Description: DataReceiverEquipartitionMonitor class computes the
//              kinetic energy for user-specified sets of sites in the
//              system and outputs it to stdout
// *********************************************************************

#include <iostream>
#include <iomanip>
using namespace std ;
#include <BlueMatter/DataReceiverEquipartitionMonitor.hpp>

#include <BlueMatter/Frame.hpp>
#include <BlueMatter/XYZ.hpp>

#include <algorithm>

DataReceiverEquipartitionMonitor::DataReceiverEquipartitionMonitor(const char* dbName,
                   const int sysId,
                   const std::vector<std::vector<int> >& siteList)
  : DataReceiverDB2(dbName, sysId),
    mSiteList(siteList),
    mKineticEnergy(siteList.size())
{}

void DataReceiverEquipartitionMonitor::init()
{
  DataReceiverDB2::init();

  const std::vector<db2::Site>& site = molecularSystem().sites().asVector();

  mKineticEnergy.resize(site.size());

  for (std::vector<std::vector<int> >::iterator iter = mSiteList.begin();
       iter != mSiteList.end();
       ++iter)
    {
      std::sort(iter->begin(), iter->end());
    }

  std::cout << "# System ID = " << sysId() << std::endl;

  std::cout.flush();
}

void DataReceiverEquipartitionMonitor::sites(Frame* frame)
{
  if (molecularSystemPointer() == NULL)
    {
      std::cerr << "No molecular system pointer defined" << std::endl;
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

  double totalKineticEnergy = 0;
  for (int i = 0; i < frame->mSiteData.getSize(); ++i)
    {
      mKineticEnergy[i] = 
  0.5*frame->mSiteData[i].mVelocity*frame->mSiteData[i].mVelocity*site[i].atomicMass();
      totalKineticEnergy += mKineticEnergy[i];
    }

  std::cout.precision(14);
  std::cout << std::scientific;
  std::cout << frame->mOuterTimeStep << " ";

  for (std::vector<std::vector<int> >::const_iterator iter = mSiteList.begin();
       iter != mSiteList.end();
       ++iter)
    {
      double ke = 0;
      for (std::vector<int>::const_iterator curSite = iter->begin();
     curSite != iter->end();
     ++curSite)
  {
    ke += mKineticEnergy[*curSite];
  }
      std::cout << std::setw(21) << ke << " ";
    }

  std::cout << std::setw(21) << totalKineticEnergy << std::endl;
  std::cout.flush();

  DataReceiver::sites(frame);

  //  DataReceiver::logInfo(frame);
}
