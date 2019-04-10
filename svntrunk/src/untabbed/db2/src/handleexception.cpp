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
// File: handleexception.cpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: HandleException
// Inheritance: db2::Exception
// Description:  Exception class for SQLHANDLE operations.  
// *********************************************************************

#include <db2/handleexception.hpp>
#include <sqlcli1.h>
#include <string.h>

namespace db2
{
  ErrorInfo::ErrorInfo(const char* sqlState,
           int errorCode,
           const char* msgBuffer)
    : d_errorCode(errorCode), d_msg(msgBuffer)
  {
    memcpy(d_sqlState, sqlState, SQLSTATE_LEN);
    d_sqlState[SQLSTATE_LEN] = '\0';
  }

  ErrorInfo::ErrorInfo(const ErrorInfo& proto)
    : d_msg(proto.d_msg),
      d_errorCode(proto.d_errorCode)
  {
    memcpy(d_sqlState, proto.d_sqlState, SQLSTATE_LEN+1);
  }

  ErrorInfo& ErrorInfo::operator=(const ErrorInfo& other)
  {
    if (this != &other)
      {
  d_msg = other.d_msg;
  d_errorCode = other.d_errorCode;
  memcpy(d_sqlState, other.d_sqlState, SQLSTATE_LEN+1);
      }
    return(*this);
  }

  std::ostream& operator<<(std::ostream& os, const ErrorInfo& info)
  {
    os << "RC = " << info.errorCode() << " ";
    os << "SQLSTATE = " << info.sqlState() << "\n";
    os << info.errorMsg() << "\n";
    return(os);
  }

  HandleException::HandleException(const char* msg, Handle& handle)
    : Exception(msg), d_handle(&handle)
  {
    SQLSMALLINT recID = 1;
    SQLCHAR sqlState[SQLSTATE_LEN+1];
    SQLINTEGER errorNo;
    SQLCHAR msgBuffer[MSG_BUFF_LEN];
    SQLSMALLINT bufLen = MSG_BUFF_LEN;
    SQLSMALLINT msgLen = 0;
    while(SQLGetDiagRec(handle.handleType(),
      *handle.CLIHandle(),
      recID,
      sqlState,
      &errorNo,
      msgBuffer,
      bufLen,
      &msgLen) == SQL_SUCCESS)
      {
  ErrorInfo info((const char*)&sqlState[0],
           errorNo,
           (const char*)&msgBuffer[0]);
  d_errorInfo.push_back(info);
  ++recID;
      }
  }

  HandleException::HandleException(const HandleException& proto)
    : Exception(proto),
      d_handle(proto.d_handle),
      d_errorInfo(proto.d_errorInfo)
  {}

HandleException& HandleException::operator=(const HandleException& other)
  {
    if (this != &other)
      {
  (Exception)*this = Exception::operator=(other);
  d_handle = other.d_handle;
  d_errorInfo = other.d_errorInfo;
      }
    return(*this);
  }

  std::ostream& HandleException::output(std::ostream& os) const
  {
    Exception::output(os);

    for (std::vector<ErrorInfo>::const_iterator iter = d_errorInfo.begin();
   iter != d_errorInfo.end();
   ++iter)
      {
  os << *iter << "\n";
      }

    return(os);
  }

}
