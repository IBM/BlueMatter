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
// File: DataReceiverEquipartitionMonitor.hpp
// Author: RSG
// Date: October 4, 2004
// Class: DataReceiverEquipartitionMonitor
// Inheritance: DataReceiverDB2
// Description: DataReceiverEquipartitionMonitor class computes the
//              kinetic energy for user-specified sets of sites in the
//              system and outputs it to stdout
// *********************************************************************

#ifndef INCLUDE_DATARECEIVEREQUIPARTITIONMONITOR_HPP
#define INCLUDE_DATARECEIVEREQUIPARTITIONMONITOR_HPP

class Frame;

#include <BlueMatter/DataReceiverDB2.hpp>
#include <BlueMatter/XYZ.hpp>

#include <vector>

class DataReceiverEquipartitionMonitor : public DataReceiverDB2
{
private:
  std::vector<std::vector<int> > mSiteList;
  std::vector<double> mKineticEnergy;
public:
  DataReceiverEquipartitionMonitor(const char* dbName,
           const int sysId,
           const std::vector<std::vector<int> >&);
  void init();
  void sites(Frame*);
};
#endif
