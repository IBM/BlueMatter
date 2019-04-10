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
// File: parsexmlspec.cpp
// Author: RSG
// Date: October 5, 2000
// Description: Class encapsulating the parsing of xml files obeying
//		the schema developed to contain force field
//		parameters. Input is taken from stdin
// ***********************************************************************
#ifndef PKFXLOG_PROBSPEC
#define PKFXLOG_PROBSPEC (0)
#endif

#include "BlueMatter/parsexmlspec.hpp"
//#include <pk/fxlogger.hpp>
#include "BlueMatter/pushparams.hpp"
#include "BlueMatter/SAXFFParamHandlers.hpp"
#include <util/PlatformUtils.hpp>
#include <framework/StdInInputSource.hpp>
#include <parsers/SAXParser.hpp>
#include <stdlib.h>
#include <iostream.h>

int ParseXMLSpec::parse(const char* xmlFile)
{
  // Initialize the XML4C2 system
  try
    {
      XMLPlatformUtils::Initialize();
      cerr << "xml4c initialization complete" << endl;
    }
  catch (const XMLException& toCatch)
    {
      cerr << "Error during initialization! :\n"
	   << XMLString::transcode(toCatch.getMessage()) << endl;
      return 1;
    }
  // create the parser
  SAXParser parser;
  // create an input source for stdin
  StdInInputSource source;

  //  Create the handler object and install it as the document and error
  //  handler for the parser.

  SAXFFParamHandlers handler;
  //  cerr << "parser handler created" << endl;
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_PROBSPEC ) << "parser hander created" << EndLogLine
#endif
  parser.setDocumentHandler(&handler);
  parser.setErrorHandler(&handler);
  //  cerr << "parser handler registered " << endl;
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_PROBSPEC ) << "parser handler registerd" << EndLogLine
#endif
  // Parse the file and catch any exceptions that propogate out
  try
    {
      if (xmlFile == NULL)
	{
		  parser.parse(source);
	}
      else
	{
	  //	  cerr << "begin parsing" << endl;
#if defined(USE_PKFXLOG)
	  BegLogLine( PKFXLOG_PROBSPEC ) << "begin parsing" << EndLogLine
#endif
	  parser.parse(xmlFile);
	  //	  cerr << "parsing completed" << endl;
#if defined(USE_PKFXLOG)
	  BegLogLine( PKFXLOG_PROBSPEC ) << "parsing completed" << EndLogLine
#endif
	}
    }

  catch (const XMLException& toCatch)
    {
      cerr << "Exception message is: \n"
	   << XMLString::transcode(toCatch.getMessage())
	   << "\n" << endl;
      return -1;
    }

return(0);

}
