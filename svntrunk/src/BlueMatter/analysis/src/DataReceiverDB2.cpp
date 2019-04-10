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
// File: DataReceiverDB2.cpp
// Author: RSG
// Date: March 15, 2003
// Class: DataReceiverDB2
// Inheritance: DataReceiver
// Description: DataReceiver class that hooks up to db2 database and
//              creates MolecularSystem object corresponding to a
//              particular sys_id.
// *********************************************************************

#include <iostream>
using namespace std ;
#include <BlueMatter/DataReceiverDB2.hpp>
#include <db2/exception.hpp>

DataReceiverDB2::~DataReceiverDB2()
{
  delete mMolecularSystem;
}

DataReceiverDB2::DataReceiverDB2(const char* dbName, const int sysId)
  : mDatabaseName(dbName), mSysId(sysId), mMolecularSystem(NULL)
{}

void DataReceiverDB2::init()
{
  try
    {
      if (mMolecularSystem == NULL)
        {
          mMolecularSystem =
            db2::MolecularSystem::constructFromDB2(mSysId, mDatabaseName.c_str());
        }
    }
  catch(db2::Exception exc)
    {
      std::cerr << exc << std::endl;
    }
  DataReceiver::init();
}
