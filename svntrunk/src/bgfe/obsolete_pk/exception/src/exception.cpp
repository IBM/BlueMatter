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
 /* -*- Mode: C++ -*- */
// ***********************************************************************
// File: pkexception.cpp
// Author: RSG
// Date: December 19, 1997
// Class: PkException
// Inheritance: None
// Description:  Base class for exception hierarchy used in the
//               Generic Flash Environment.  Code and concepts have
//               been freely stolen from the iexcbase.h file
//               provided by the IBM C++ compiler
// ***********************************************************************
#if 0
#include <pk/exception.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std ;

PkExceptionLocation::PkExceptionLocation(const char* fileName,
                                           const char* functionName,
                                           long int lineNumber)
  : d_lineNumber(lineNumber)
{
  strncpy(d_fileName, fileName, MAX_STRING_LEN - 1);
  strncpy(d_functionName, functionName, MAX_STRING_LEN - 1);
}

PkExceptionLocation::PkExceptionLocation(const PkExceptionLocation&
                                           proto) :
  d_lineNumber(proto.d_lineNumber)
{
  strncpy(d_fileName, proto.d_fileName, MAX_STRING_LEN - 1);
  strncpy(d_functionName, proto.d_functionName, MAX_STRING_LEN - 1);
}

PkExceptionLocation& PkExceptionLocation::operator=(const
                                                      PkExceptionLocation& other)
{
  if (this == &other)
    {
      return *this;
    }
  d_lineNumber = other.d_lineNumber;
  strncpy(d_fileName, other.d_fileName, MAX_STRING_LEN - 1);
  strncpy(d_functionName, other.d_functionName, MAX_STRING_LEN - 1);
  return *this;
}

const char* PkExceptionLocation::fileName() const
{
  return d_fileName;
}

const char* PkExceptionLocation::functionName() const
{
  return d_functionName;
}

long int PkExceptionLocation::lineNumber() const
{
  return d_lineNumber;
}

PkException::PkException(const char* message) : d_locationCount(0)
{
  strncpy(d_message, message, MAX_STRING_LEN - 1);
}

PkException::PkException(const PkException& proto) :
  d_locationCount(proto.d_locationCount)
{
  strcpy(d_message, proto.d_message);
  for (int i = 0; i < d_locationCount; i++)
    {
      d_locationArray[i] = proto.d_locationArray[i];
    }
}

PkException& PkException::operator=(const PkException& other)
{
  if (this == &other)
    {
      return *this;
    }

  d_locationCount = other.d_locationCount;
  strcpy(d_message, other.d_message);
  for (int i = 0; i < d_locationCount; i++)
    {
      d_locationArray[i] = other.d_locationArray[i];
    }
  return *this;
}

void PkException::assertParameter(const char* exceptionText,
                                   PkExceptionLocation location)
{
  PkAssertException exc(exceptionText);
  exc.addLocation(location);
  throw exc;
}

PkException::~PkException()
{}

void PkException::addLocation(const PkExceptionLocation& location)
{
  if (d_locationCount < MAX_LOCATION)
    {
      d_locationArray[d_locationCount] = location;
      ++d_locationCount;
    }
}

const PkExceptionLocation& PkException::getLocation(int index) const
{
  if ((index < 0) || (index >= d_locationCount))
    {
      index = 0;
    }
  return d_locationArray[index];
}

// class ostream ;
ostream& operator<<(ostream& os, const PkException& exc)
{
  for (int i = 0; i < exc.d_locationCount; i++)
    {
      os << "Exception in file: " << exc.getLocation(i).fileName() << endl;
      os << " at line: " << exc.getLocation(i).lineNumber() << endl;
      os << " in function: " << exc.getLocation(i).functionName() << endl;
      os << endl;
    }
  os << " Message: " << exc.d_message;
  return os;
}
#endif
