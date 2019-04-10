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
// File: ffsitehandler.hpp
// Date: October 5, 2000
// Author: RSG
// Description: Classe derived from interface class
//	       FFHandler to deal with processing site
//	       records that need to be "pushed" into Mike Pitman's
//	       setup code
// ***************************************************************************

#ifndef INCLUDE_FFSITEHANDLER_HPP
#define INCLUDE_FFSITEHANDLER_HPP

#include "BlueMatter/ffhandler.hpp"

class FFSiteHandler : public FFHandler
{
private:
  // enums
  enum {UNKNOWN = -1, NAME = 0, TYPE = 1, EPS = 2, SIGMA = 3, EPS14 = 4, SIGMA14 = 5, MASS =
	6, CHARGE = 7};
  enum {ELEM_COUNT = 8};
  // const data
  static char* s_key[ELEM_COUNT];
  // data
  int d_mode;
  char* d_name;
  char* d_type;
  double d_mass;
  double d_charge;
  double d_eps;
  double d_sigma;
  double d_eps14;
  double d_sigma14;
public:
  // methods
  ~FFSiteHandler();

  inline FFSiteHandler() : d_name(NULL), d_type(NULL), d_mass(0),
    d_charge(0), d_eps(0), d_sigma(0), d_eps14(0), d_sigma14(0), d_mode(UNKNOWN)
    {
      FFHandler::setType(FFHandler::SITE);
    }

  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};

#endif
