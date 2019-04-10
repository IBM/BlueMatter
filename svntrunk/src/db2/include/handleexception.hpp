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
// File: handleexception.hpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: HandleException
// Inheritance: db2::Exception
// Description:  Exception class for SQLHANDLE operations.  
// *********************************************************************

#ifndef INCLUDE_DB2_HANDLEEXCEPTION_HPP
#define INCLUDE_DB2_HANDLEEXCEPTION_HPP

#include <db2/handle.hpp>
#include <db2/exception.hpp>
#include <db2/db2defs.hpp>
#include <string>
#include <vector>

namespace db2
{

  class ErrorInfo
  {
    friend std::ostream& operator<<(std::ostream&, const ErrorInfo&);
  private:
    char d_sqlState[SQLSTATE_LEN+1];
    int d_errorCode;
    std::string d_msg;
  public:
    ErrorInfo(const char*, int, const char*); // sqlstate, error code,
                                              // msg

    ErrorInfo(const ErrorInfo&);
    ErrorInfo& operator=(const ErrorInfo&);

    const char* sqlState() const
    {return d_sqlState;}

    int errorCode() const
    {return d_errorCode;}

    const char* errorMsg() const
    {return d_msg.data();}
      
  };

  class HandleException : public Exception
  {
  private:
    Handle* d_handle;
    std::vector<ErrorInfo> d_errorInfo;

  public:

    HandleException(const char*, Handle&);
    HandleException(const HandleException& proto);

    HandleException& operator=(const HandleException& other);

    std::ostream& output(std::ostream&) const;

    inline Handle& getHandle();
  };

  std::ostream& operator<<(std::ostream&, const ErrorInfo&);

  Handle& HandleException::getHandle()
  {
    return(*d_handle);
  }
}

#define SQLTEST(rc, handle)\
    if (!(rc == SQL_SUCCESS) ) {\
    HandleException exc("Failed SQLTEST", handle);\
    exc.addLocation(EXCEPTION_LOCATION());\
    throw exc;}\



#endif
