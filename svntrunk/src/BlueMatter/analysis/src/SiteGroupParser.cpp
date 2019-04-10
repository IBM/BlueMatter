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
 // ************************************************************************
// File: SiteGroupParser.cpp
// Author: RSG
// Date: October 5, 2004
// Class: SiteGroupParser
// Description: Class encapsulating the operations of reading data from an
//              istream that contains groups of site ids
//              (zero-indexed). 
//
//              This stream has the form of a series of whitespace
//              delimited integers corresponding to each group of site
//              ids (zero-indexed) Each series is be delimited by one
//              or more lines beginning with a '#' character (all
//              these lines will be ignored)
// ************************************************************************

#include <BlueMatter/SiteGroupParser.hpp>

#include <string>

void SiteGroupParser::parse(std::istream& is,
			    std::vector<std::vector<int> >& siteList)
{
  enum {READING_SITELIST, WAITING_FOR_NEW_LIST};

  siteList.clear();
  int readState = WAITING_FOR_NEW_LIST;

  int id = -1;
  int foo = -1;
  while(is)
    {
      while (is.peek() == ' ' || is.peek() == '\n' || is.peek() == '\t')
	{
	  is.ignore();
	}
      if (is.peek() == '#')
	{
	  std::string comment;
	  std::getline(is, comment, '\n'); // read to end of line
	  std::cout << comment << std::endl; // put it out
	  readState = WAITING_FOR_NEW_LIST;
	  continue;
	}
      is >> id;
      if (!is)
	{
	  break;
	}
      if (readState == WAITING_FOR_NEW_LIST)
	{
	  readState = READING_SITELIST;
	  siteList.resize(siteList.size() + 1);
	}
      siteList.back().push_back(id);
    }
}
