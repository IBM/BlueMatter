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
 // ***************************************************************************
// File: ffheaderhandler.hpp
// Date: October 5, 2000
// Author: RSG
// Description: Class derived from interface class
//	       FFHandler to deal with processing header
//	       records that need to be "pushed" into Mike Pitman's
//	       setup code
// ***************************************************************************

#ifndef INCLUDE_FFHEADERHANDLER_HPP
#define INCLUDE_FFHEADERHANDLER_HPP

#include "BlueMatter/ffhandler.hpp"

class FFHeaderHandler : public FFHandler
{
private:
  // enums
  enum {UNKNOWN = -1, CHARGE = 0, VDW = 1, VDWCOMBINE = 2, CHARGE14 = 3,
	CHARGE14SCALE = 4, VDW14 = 5, VDW14SCALE = 6, IMPROPER = 7,
	TORSIONINPUT = 8, UREYBRADLEY = 9, GROUPING = 10, WATER = 11,
	UNITS = 12, ELEM_COUNT = 13
  };
  // const data
  static char* s_key[ELEM_COUNT];
  // data
  int d_mode;

public:
  // methods
  ~FFHeaderHandler();

  inline FFHeaderHandler() : d_mode(UNKNOWN)
    {
      FFHandler::setType(FFHandler::HEADER);
    }

  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};

#endif
