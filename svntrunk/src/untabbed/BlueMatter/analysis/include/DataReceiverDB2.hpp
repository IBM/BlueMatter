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
// File: DataReceiverDB2.hpp
// Author: RSG
// Date: March 15, 2003
// Class: DataReceiverDB2
// Inheritance: DataReceiver
// Description: DataReceiver class that hooks up to db2 database and
//              creates MolecularSystem object corresponding to a
//              particular sys_id.
// *********************************************************************

#ifndef INCLUDE_DATARECEIVERDB2_HPP
#define INCLUDE_DATARECEIVERDB2_HPP

#include <BlueMatter/DataReceiver.hpp>
//#ifdef AK_DB2
#include <BlueMatter/db2/molecularsystem.hpp>
//#endif
#include <string>
#include <cassert>
#include <cstdlib>

class DataReceiverDB2 : public DataReceiver
{
private:
  db2::MolecularSystem* mMolecularSystem;
  std::string mDatabaseName;
  int mSysId;
protected:
  inline int sysId() const;
  inline db2::MolecularSystem& molecularSystem(); // will assert if
               // mMolecularSystem is
               // NULL
  inline db2::MolecularSystem* molecularSystemPointer();
public:
  virtual ~DataReceiverDB2();
  DataReceiverDB2(const char* dbName, const int sysId);

  // overloads of virtual methods of DataReceiver:
  void init();
};

int DataReceiverDB2::sysId() const
{
  return mSysId;
}

db2::MolecularSystem& DataReceiverDB2::molecularSystem()
{
  assert(mMolecularSystem != NULL);
  return(*mMolecularSystem);
}


db2::MolecularSystem* DataReceiverDB2::molecularSystemPointer()
{
  return(mMolecularSystem);
}

#endif
