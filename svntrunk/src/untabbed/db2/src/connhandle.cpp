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
// File: connhandle.cpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: ConnHandle
// Inheritance: Handle
// Description:  Interface class for connection handle operations. 
// *********************************************************************

#include <db2/connhandle.hpp>
#include <db2/handleexception.hpp>

namespace db2
{
  ConnHandle::ConnHandle(EnvHandle& env)
    : Handle(SQL_HANDLE_DBC, &env),
      d_isConnected(0)
  {}

  ConnHandle::ConnHandle()
  {}

  ConnHandle::~ConnHandle()
  {
    disconnect();
  }

  void ConnHandle::connect(const char* dsn, const char* uid, const char* pw)
  {
    SQLRETURN rc = SQLConnect(*CLIHandle(),
            (SQLCHAR*)dsn,
            SQL_NTS,
            (SQLCHAR*)uid,
            SQL_NTS,
            (SQLCHAR*)pw,
            SQL_NTS);
    SQLTEST(rc, (*this));
    d_isConnected = 1;
}

  void ConnHandle::disconnect()
  {
    if (d_isConnected)
      {
  SQLRETURN rc = SQLDisconnect(*CLIHandle());
  SQLTEST(rc, *this);
      }
    d_isConnected = 0;
  }
}
