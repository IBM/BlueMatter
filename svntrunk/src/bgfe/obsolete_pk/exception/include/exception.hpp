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
// File: pkexception.hpp
// Author: RSG
// Date: December 19, 1997
// Class: PkException
// Inheritance: None
// Description:  Base class for exception hierarchy used in the
//               Generic Flash Environment.  Code and concepts have
//               been freely stolen from the iexcbase.h file
//               provided by the IBM C++ compiler
// ***********************************************************************

#ifndef INCLUDE_PkEXCEPTION_HPP
#define INCLUDE_PkEXCEPTION_HPP

#if !defined( PK_BLADE )
#include <iostream>
using namespace std ;
// class ostream;

class PkExceptionLocation
{
private:
  enum {MAX_STRING_LEN = 256};
  char d_fileName[MAX_STRING_LEN];
  char d_functionName[MAX_STRING_LEN];
  long int d_lineNumber;
public:
  PkExceptionLocation(const char* fileName = "", const char*
                       functionName = "", long int lineNumber =
                       0);
  PkExceptionLocation(const PkExceptionLocation&);
  PkExceptionLocation& operator=(const PkExceptionLocation&);

  const char* fileName() const;
  const char* functionName() const;
  long int lineNumber() const;
};

class PkException
{
  friend ostream& operator<<(ostream&, const PkException&);
private:
  enum {MAX_STRING_LEN = 256};
  enum {MAX_LOCATION = 5};
  // data
  PkExceptionLocation d_locationArray[MAX_LOCATION];
  int d_locationCount;
  char d_message[MAX_STRING_LEN];

public:
  PkException(const char*);
  PkException(const PkException&);
  PkException& operator=(const PkException&);
  ~PkException();
  void addLocation(const PkExceptionLocation&);
  const PkExceptionLocation& getLocation(int) const;
  static void
  assertParameter ( const char*        exceptionText,
                    PkExceptionLocation location );
};

ostream& operator<<(ostream&, const PkException&);

class PkAssertException : public PkException
{
public:
  PkAssertException(const char* text) : PkException(text)
  {}
  PkAssertException(const PkAssertException& proto) :
    PkException(proto)
  {}
  PkAssertException& operator=(const PkAssertException& other)
  {
    (PkException)*this = PkException::operator=(other);
    return *this;
  }
  ~PkAssertException()
  {}
};


#if defined ( __FUNCTION__ )
  #define PkEXCEPTION_LOCATION() \
    PkExceptionLocation(__FILE__, __FUNCTION__, __LINE__)
#else
  #define PkEXCEPTION_LOCATION() \
    PkExceptionLocation(__FILE__, "", __LINE__)
#endif

#define PkTHROW(exc)\
     exc.addLocation(PkEXCEPTION_LOCATION()),\
     throw exc


#define PkRETHROW(exc)\
     exc.addLocation(PkEXCEPTION_LOCATION()),\
     throw

#define PkASSERT(test)\
      if(!(test))\
      PkException::assertParameter("assert failed: " #test, PkEXCEPTION_LOCATION())

#endif
#endif
