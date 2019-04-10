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
// File: envhandle.cpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: EnvHandle
// Inheritance: Handle
// Description:  Interface class for environment handle operations. 
// *********************************************************************

#include <db2/envhandle.hpp>
#include <db2/handleexception.hpp>

namespace db2
{
  EnvHandle::EnvHandle() : Handle(SQL_HANDLE_ENV, &Handle::NullHandle)
  {
#if 0
    SQLINTEGER attrValue = SQL_OV_ODBC3;
    SQLRETURN rc = SQLSetEnvAttr(*CLIHandle(),
         SQL_ATTR_ODBC_VERSION,
         &attrValue,
         0);
    SQLTEST(rc, *this);
#endif
  }

  void EnvHandle::setODBCVersion(int versionNo)
  {
    SQLINTEGER attrValue = SQL_OV_ODBC3;
    switch(versionNo)
      {
      case 2:
  attrValue = SQL_OV_ODBC2;
  break;
      case 3:
  attrValue = SQL_OV_ODBC3;
  break;
      default:
  THROW(HandleException("Illegal ODBC Version selection", *this));
      }

    SQLRETURN rc = SQLSetEnvAttr(*CLIHandle(),
         SQL_ATTR_ODBC_VERSION,
         &attrValue,
         0);
    SQLTEST(rc, *this);
  }
}
