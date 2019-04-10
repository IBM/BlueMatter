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
// File: FFHandler.cpp
// Date: October 5, 2000
// Author: RSG
// Description: Family of classes derived from interface class
//             FFHandler to deal with processing different types of
//             records that need to be "pushed" into Mike Pitman's
//             setup code
// ***************************************************************************
#ifndef PKFXLOG_FFHANDLER
#define PKFXLOG_FFHANDLER (0)
#endif

#include "BlueMatter/ffhandler.hpp"
#include "BlueMatter/ffheaderhandler.hpp"
#include "BlueMatter/ffsitehandler.hpp"
#include "BlueMatter/bondlisthandler.hpp"
#include "BlueMatter/anglelisthandler.hpp"
#include "BlueMatter/torsionlisthandler.hpp"
#include "BlueMatter/improperlisthandler.hpp"
#include <util/XMLString.hpp>
#include <assert.h>
//#undef assert
//#include <pk/fxlogger.hpp>

char* FFHandler::s_key[ELEM_COUNT] = {"header", "siterec", "bondrec", "anglerec",
                                 "charmmtorsionrec", "charmmimproperrec",
                                 "oplstorsionrec", "oplsimproperrec"};

char* BondHandler::s_key[ELEM_COUNT] = {"k","r0"};

char* AngleHandler::s_key[ELEM_COUNT] = {"k", "th0", "ku", "ru"};

char* CHARMMTorsionHandler::s_key[ELEM_COUNT]  = {"k", "mult", "th0"};

char* OPLSTorsionHandler::s_key[ELEM_COUNT]  = {"Vn", "n", "phi0"};

char* CHARMMImproperHandler::s_key[ELEM_COUNT]  = {"k", "th0"};

char* OPLSImproperHandler::s_key[ELEM_COUNT]  = {"Vn", "n", "phi0"};

FFHandler::~FFHandler()
{}

int FFHandler::findString(const char* query, char** key, int len)
{
  for (int i = 0; i < len; i++)
    {
      if (XMLString::compareIString(query, key[i]) == 0)
        return(i);
    }
  return(UNKNOWN);
}
FFHandler* FFHandler::createHandler(const char* key)
{
  int index = findString(key, s_key, ELEM_COUNT);
  switch(index)
    {
    default:
    case UNKNOWN:
      return NULL;
    case HEADER:
      return(new FFHeaderHandler());
    case SITE:
      return(new FFSiteHandler());
    case BOND:
      return(new BondHandler());
    case ANGLE:
      return(new AngleHandler());
    case CHARMM_TORSION:
      return(new CHARMMTorsionHandler());
    case CHARMM_IMPROPER:
      return(new CHARMMImproperHandler());
    case OPLS_TORSION:
      return(new OPLSTorsionHandler());
    case OPLS_IMPROPER:
      return(new OPLSImproperHandler());
    }
  return NULL;
}

template <int SITE_COUNT>
void SiteListHandler<SITE_COUNT>::startElement(const char* elem)
{
  d_siteMode = (XMLString::compareIString(elem, "siteid") == 0);
}


template <int SITE_COUNT>
void SiteListHandler<SITE_COUNT>::endElement(const char* elem)
{
  d_siteMode = false;
}


template <int SITE_COUNT>
void SiteListHandler<SITE_COUNT>::charData(char* data)
{
  if (d_siteMode)
    {
#if defined(USE_PKFXLOG)
      BegLogLine( PKFXLOG_FFHANDLER )
        << "SiteListHandler<" << SITE_COUNT << ">::charData has data: "
        << data << " at index = " << d_count
        << EndLogLine;
#endif
      assert(d_count < SITE_COUNT);
      d_site[d_count] = data;
      ++d_count;
    }
}

void BondHandler::startElement(const char* elem)
{
  SiteListHandler<2>::startElement(elem);
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);
}

void BondHandler::endElement(const char* elem)
{
  SiteListHandler<2>::endElement(elem);
  d_mode = UNKNOWN;
}

void BondHandler::charData(char* data)
{
  SiteListHandler<2>::charData(data);
  switch(d_mode)
    {
    default:
    case UNKNOWN:
      break;
    case K:
      d_k0 = atof(data);
      break;
    case R0:
      d_r0 = atof(data);
      break;
    }
}

void BondHandler::initialize()
{}

void BondHandler::finalize()
{
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFHANDLER )
    << "site[1]: " << SiteListHandler<2>::getSite(0)
    << " site[2]: " << SiteListHandler<2>::getSite(1)
    << " k0: " << d_k0
    << " r0: " << d_r0
    << EndLogLine;
#endif
  char* s1 = SiteListHandler<2>::getSite(0);
  char* s2 = SiteListHandler<2>::getSite(1);

  BondListHandler::instance()->push(s1, s2, d_k0, d_r0);
  //  PushBondSpec(SiteListHandler<2>::getSite(0),
  //           SiteListHandler<2>::getSite(1),
  //           d_k0,
  //           d_r0);
}

void AngleHandler::startElement(const char* elem)
{
  SiteListHandler<3>::startElement(elem);
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);
}

void AngleHandler::endElement(const char* elem)
{
  SiteListHandler<3>::endElement(elem);
  d_mode = UNKNOWN;
}

void AngleHandler::charData(char* data)
{
  SiteListHandler<3>::charData(data);
  switch(d_mode)
    {
    default:
    case UNKNOWN:
      break;
    case K:
      d_k = atof(data);
      break;
    case TH0:
      d_th0 = atof(data);
      break;
    case KU:
      d_ku = atof(data);
      break;
    case RU:
      d_ru = atof(data);
      break;
    }
}

void AngleHandler::initialize()
{}

void AngleHandler::finalize()
{
  AngleListHandler::instance()->push(SiteListHandler<3>::getSite(0),
                                     SiteListHandler<3>::getSite(1),
                                     SiteListHandler<3>::getSite(2),
                                     d_k,
                                     d_th0,
                                     d_ku,
                                     d_ru);
}


void CHARMMTorsionHandler::startElement(const char* elem)
{
  SiteListHandler<4>::startElement(elem);
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);
}

void CHARMMTorsionHandler::endElement(const char* elem)
{
  SiteListHandler<4>::endElement(elem);
  d_mode = UNKNOWN;
}

void CHARMMTorsionHandler::charData(char* data)
{
  SiteListHandler<4>::charData(data);
  switch(d_mode)
    {
    default:
    case UNKNOWN:
      break;
    case K:
      d_k = atof(data);
      break;
    case MULT:
      d_mult = atoi(data);
      break;
    case TH0:
      d_th0 = atof(data);
      break;
    }
}

void CHARMMTorsionHandler::initialize()
{}

void CHARMMTorsionHandler::finalize()
{
  TorsionListHandler::instance()->push(SiteListHandler<4>::getSite(0),
                                       SiteListHandler<4>::getSite(1),
                                       SiteListHandler<4>::getSite(2),
                                       SiteListHandler<4>::getSite(3),
                                       d_k,
                                       d_mult,
                                       d_th0);
}


void CHARMMImproperHandler::startElement(const char* elem)
{
  SiteListHandler<4>::startElement(elem);
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);
}

void CHARMMImproperHandler::endElement(const char* elem)
{
  SiteListHandler<4>::endElement(elem);
  d_mode = UNKNOWN;
}

void CHARMMImproperHandler::charData(char* data)
{
  SiteListHandler<4>::charData(data);
  switch (d_mode)
    {
    default:
      break;
    case UNKNOWN:
      break;
    case K:
      d_k = atof(data);
      break;
    case TH0:
      d_th0 = atof(data);
      break;
    }
}

void CHARMMImproperHandler::initialize()
{}

void CHARMMImproperHandler::finalize()
{
  ImproperListHandler::instance()->push(SiteListHandler<4>::getSite(0),
                                        SiteListHandler<4>::getSite(1),
                                        SiteListHandler<4>::getSite(2),
                                        SiteListHandler<4>::getSite(3),
                                        d_k,
                                        0, // improper doesn't need mult does it?
                                        d_th0);
}

void OPLSTorsionHandler::startElement(const char* elem)
{
  SiteListHandler<4>::startElement(elem);
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);

}

void OPLSTorsionHandler::endElement(const char* elem)
{
  SiteListHandler<4>::endElement(elem);
  d_mode = UNKNOWN;
}

void OPLSTorsionHandler::charData(char* data)
{
  SiteListHandler<4>::charData(data);
  switch(d_mode)
    {
    default:
    case UNKNOWN:
      break;
    case K:
      d_k = atof(data);
      break;
    case MULT:
      d_mult = atoi(data);
      break;
    case TH0:
      d_th0 = atof(data);
      break;
    }
}

void OPLSTorsionHandler::initialize()
{}

void OPLSTorsionHandler::finalize()
{
  TorsionListHandler::instance()->push(SiteListHandler<4>::getSite(0),
                                       SiteListHandler<4>::getSite(1),
                                       SiteListHandler<4>::getSite(2),
                                       SiteListHandler<4>::getSite(3),
                                       d_k,
                                       d_mult,
                                       d_th0);
}


void OPLSImproperHandler::startElement(const char* elem)
{
  SiteListHandler<4>::startElement(elem);
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);
}

void OPLSImproperHandler::endElement(const char* elem)
{
  SiteListHandler<4>::endElement(elem);
  d_mode = UNKNOWN;

}

void OPLSImproperHandler::charData(char* data)
{
  SiteListHandler<4>::charData(data);
  switch(d_mode)
    {
    default:
    case UNKNOWN:
      break;
    case K:
      d_k = atof(data);
      break;
    case MULT:
      d_mult = atoi(data);
      break;
    case TH0:
      d_th0 = atof(data);
      break;
    }

}

void OPLSImproperHandler::initialize()
{}

void OPLSImproperHandler::finalize()
{
  ImproperListHandler::instance()->push(SiteListHandler<4>::getSite(0),
                                        SiteListHandler<4>::getSite(1),
                                        SiteListHandler<4>::getSite(2),
                                        SiteListHandler<4>::getSite(3),
                                        d_k,
                                        d_mult,
                                        d_th0);
}









