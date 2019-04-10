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
// File: exception.cpp
// Author: RSG
// Date: December 1, 2001
// Class: Exception
// Namespace: cbc
// Inheritance: None
// Description:	 Base class for exception hierarchy used in the
//		 db2 namespace.  Code and concepts have
//		 been freely stolen from the iexcbase.h file
//		 provided by the IBM C++ compiler
// ***********************************************************************
#include <db2/exception.hpp>
#include <string>
#include <iostream>

namespace db2
{
  ExceptionLocation::ExceptionLocation(const char* fileName,
				       const char* functionName,
				       long int lineNumber) :
    d_lineNumber(lineNumber),
    d_fileName(fileName),
    d_functionName(functionName)
    {}

  ExceptionLocation::ExceptionLocation(const ExceptionLocation&
				       proto) :
    d_lineNumber(proto.d_lineNumber),
    d_fileName(proto.d_fileName),
    d_functionName(proto.d_functionName)
    {}

  ExceptionLocation& ExceptionLocation::operator=(const
						  ExceptionLocation& other)
  {
    if (this == &other)
    {
      return *this;
    }
    d_lineNumber = other.d_lineNumber;
    d_fileName = other.d_fileName;
    d_functionName = other.d_functionName;
    return *this;
  }

  const char* ExceptionLocation::fileName() const
    {
      return d_fileName.c_str();
    }

  const char* ExceptionLocation::functionName() const
    {
      return d_functionName.c_str();
    }

  long int ExceptionLocation::lineNumber() const
    {
      return d_lineNumber;
    }

  Exception::Exception(const char* message)
    : d_message(message)
    {}

  Exception::Exception(const Exception& proto) :
    d_message(proto.d_message),
    d_location(proto.d_location)
    {}

  Exception& Exception::operator=(const Exception& other)
  {
    if (this == &other)
    {
      return *this;
    }

    d_message = other.d_message;
    d_location = other.d_location;
    return *this;
  }

  void Exception::assertParameter(const char* exceptionText,
				  ExceptionLocation location)
    {
      AssertException exc(exceptionText);
      exc.addLocation(location);
      throw exc;
    }

  Exception::~Exception()
    {}

  void Exception::addLocation(const ExceptionLocation& location)
    {
      d_location.push_back(location);
    }

  const ExceptionLocation& Exception::getLocation(int index) const
    {
      return d_location[index];
    }

  std::ostream& operator<<(std::ostream& os, const Exception& exc)
    {
      return(exc.output(os));
    }

  std::ostream& Exception::output(std::ostream& os) const
  {
      for (std::vector<ExceptionLocation>::const_iterator iter =
	     d_location.begin();
	   iter != d_location.end();
	   ++iter)
	{
	  os << "Exception in file: " << (*iter).fileName() << std::endl;
	  os << " at line: " << (*iter).lineNumber() << std::endl;
	  os << " in function: " << (*iter).functionName() << std::endl;
	  os << std::endl;
	}
      os << " Message: " << d_message << std::endl;
      return os;
    }
}

