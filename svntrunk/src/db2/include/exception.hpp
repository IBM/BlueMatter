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
 
// ***********************************************************************
// File: exception.hpp
// Author: RSG
// Date: December 1, 2001
// Class: Exception
// Namespace: db2
// Inheritance: None
// Description:	 Base class for exception hierarchy used in the
//		 db2 namespace.  Code and concepts have
//		 been freely stolen from the iexcbase.h file
//		 provided by the IBM C++ compiler
// ***********************************************************************

#ifndef INCLUDE_DB2_EXCEPTION_HPP
#define INCLUDE_DB2_EXCEPTION_HPP

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace db2
{

  class ExceptionLocation
  {
  private:
    enum {MAX_STRING_LEN = 256};
    std::string d_fileName;
    std::string d_functionName;
    long int d_lineNumber;
  public:
    ExceptionLocation(const char* fileName = "", const char*
		      functionName = "", long int lineNumber =
		      0);
    ExceptionLocation(const ExceptionLocation&);
    ExceptionLocation& operator=(const ExceptionLocation&);

    const char* fileName() const;
    const char* functionName() const;
    long int lineNumber() const;
  };

  class Exception
  {
    friend std::ostream& operator<<(std::ostream&, const Exception&);
  private:
  // data
  std::vector<ExceptionLocation> d_location;
  std::string d_message;

  public:
    Exception(const char*);
    Exception(const Exception&);
    Exception& operator=(const Exception&);
    virtual ~Exception();
    virtual std::ostream& output(std::ostream&) const;
    void addLocation(const ExceptionLocation&);
    const ExceptionLocation& getLocation(int) const;
    static void
    assertParameter ( const char* exceptionText,
		      ExceptionLocation location );
  };

  std::ostream& operator<<(std::ostream&, const Exception&);

  class AssertException : public Exception
  {
  public:
    AssertException(const char* text) : Exception(text)
      {}
    AssertException(const AssertException& proto) :
      Exception(proto)
      {}
    AssertException& operator=(const AssertException& other)
      {
	(Exception)*this = Exception::operator=(other);
	return *this;
      }
    ~AssertException()
      {}
  };


#if defined ( __FUNCTION__ )
  #define EXCEPTION_LOCATION() \
    db2::ExceptionLocation(__FILE__, __FUNCTION__, __LINE__)
#else
  #define EXCEPTION_LOCATION() \
    db2::ExceptionLocation(__FILE__, "", __LINE__)
#endif

#define THROW(exc)\
     exc.addLocation(EXCEPTION_LOCATION()),\
     throw exc


#define RETHROW(exc)\
     exc.addLocation(EXCEPTION_LOCATION()),\
     throw

#define ASSERT(test)\
      if(!(test))\
      db2::Exception::assertParameter("assert failed: " #test, EXCEPTION_LOCATION())

}

#endif
