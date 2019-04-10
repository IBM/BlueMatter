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
// File: FFHandler.hpp
// Date: October 5, 2000
// Author: RSG
// Description: Family of classes derived from interface class
//	       FFHandler to deal with processing different types of
//	       records that need to be "pushed" into Mike Pitman's
//	       setup code
// ***************************************************************************

#ifndef INCLUDE_FFHANDLER_HPP
#define INCLUDE_FFHANDLER_HPP

#include <stdlib.h>
#include <assert.h>

class FFHandler
{
private:
  int d_type;
protected:
  // enums
  enum {UNKNOWN = -1, HEADER = 0, SITE = 1, BOND = 2, ANGLE = 3, CHARMM_TORSION = 4, CHARMM_IMPROPER = 5, OPLS_TORSION = 6, OPLS_IMPROPER = 7};
  enum {ELEM_COUNT = 8};
  // const data
  static char* s_key[ELEM_COUNT];
  // methods
  inline void setType(int aType)
    {
      assert(aType >= 0);
      d_type = aType;
    }
  inline FFHandler() : d_type(UNKNOWN)
    {}
public:
  // find char string in array of char of length n.  return index if
  // found, return < 0 if not found
  static int findString(const char*, char**, int);
  virtual ~FFHandler();
  virtual void startElement(const char*) = 0;
  virtual void endElement(const char*) = 0;
  virtual void charData(char*) = 0;
  virtual void initialize() = 0;
  virtual void finalize() = 0;
  inline const char* getType()
    {
      assert(d_type >= 0);
      return(s_key[d_type]);
    }
  static FFHandler* createHandler(const char*); // factory method
};

// encapsulates behavior required to load up a list of site
// identifiers
template <int SITE_COUNT>
class SiteListHandler : public FFHandler
{
private:
  // data
  char* d_site[SITE_COUNT];
  int d_count;
  int d_siteMode;
public:
  // methods
  inline SiteListHandler() : d_count(0), d_siteMode(0)
    {
      for (int i = 0; i < SITE_COUNT; i++)
      {
	d_site[i] = NULL;
      }
    }

  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);

  inline char* getSite(int i)
    {
      return d_site[i];
    }
};

class BondHandler : public SiteListHandler<2>
{
private:
  // enums
  enum {UNKNOWN = -1, K = 0, R0};
  enum {ELEM_COUNT = 2};
  // const data
  static char* s_key[ELEM_COUNT];
  // data
  double d_k0;
  double d_r0;
  int d_mode;
  // methods
public:
  inline BondHandler() : d_k0(0), d_r0(0)
    {
      FFHandler::setType(FFHandler::BOND);
    }
  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};

class AngleHandler : public SiteListHandler<3>
{
private:
  // enums
  enum {UKNOWN = -1, K = 0, TH0 = 1, KU = 2, RU = 3};
  enum {ELEM_COUNT = 4};
  // const data;
  static char* s_key[ELEM_COUNT];
  // data
  double d_k;
  double d_th0;
  double d_ku;
  double d_ru;
  int d_mode;
public:
  inline AngleHandler() : d_k(0), d_th0(0), d_ku(0), d_ru(0)
    {
      FFHandler::setType(FFHandler::ANGLE);
    }
  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};

class CHARMMTorsionHandler : public SiteListHandler<4>
{
private:
  // enums
  enum {UNKNOWN = -1, K = 0, MULT = 1, TH0 = 2};
  enum {ELEM_COUNT = 3};
  // const data
  static char* s_key[ELEM_COUNT];
  double d_k;
  double d_th0;
  int d_mult;
  int d_mode;
public:
  CHARMMTorsionHandler() : d_k(0), d_mult(0), d_th0(0)
    {
      FFHandler::setType(FFHandler::CHARMM_TORSION);
    }
  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};

class CHARMMImproperHandler : public SiteListHandler<4>
{
  // enums
  enum {UNKNOWN = -1, K = 0, TH0 = 1};
  enum {ELEM_COUNT = 2};
  // const data
  static char*	s_key[ELEM_COUNT];
private:
  double d_k;
  double d_th0;
  int d_mode;
public:
  CHARMMImproperHandler() : d_k(0), d_th0(0)
    {
      FFHandler::setType(FFHandler::CHARMM_IMPROPER);
    }
  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};


class OPLSTorsionHandler : public SiteListHandler<4>
{
  enum {UNKNOWN = -1, K = 0, MULT = 1, TH0 = 2};
  enum {ELEM_COUNT = 3};
private:
  static char* s_key[ELEM_COUNT];
  double d_k;
  int d_mult;
  double d_th0;
  int d_mode;
public:
  OPLSTorsionHandler() : d_k(0), d_mult(0), d_th0(0)
    {
      FFHandler::setType(FFHandler::OPLS_TORSION);
    }
  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};


class OPLSImproperHandler : public SiteListHandler<4>
{
  enum {UNKNOWN = -1, K = 0, MULT = 1, TH0 = 2};
  enum {ELEM_COUNT = 3};
private:
  static char* s_key[ELEM_COUNT];
  double d_k;
  int d_mult;
  double d_th0;
  int d_mode;
public:
  OPLSImproperHandler() : d_k(0), d_mult(0), d_th0(0)
    {
      FFHandler::setType(FFHandler::OPLS_IMPROPER);
    }
  void startElement(const char*);
  void endElement(const char*);
  void charData(char*);
  void initialize();
  void finalize();
};


#endif




