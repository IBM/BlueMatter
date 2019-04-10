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
 #ifndef PKFXLOG_FFSITEHANDLER
#define PKFXLOG_FFSITEHANDLER (0)
#endif

#include "BlueMatter/ffsitehandler.hpp"
#include "BlueMatter/sitespeclisthandler.hpp"
//#undef assert
//#include <pk/fxlogger.hpp>

char* FFSiteHandler::s_key[ELEM_COUNT]  = {"siteid", "sitetype", "epsilon",
                                    "sigma", "epsilon14", "sigma14", "mass", "charge"};


FFSiteHandler::~FFSiteHandler()
{
  delete [] d_name;
  delete [] d_type;
}

void FFSiteHandler::startElement(const char* elem)
{
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFSITEHANDLER )
    << "FFSiteHandler::startElement mode = " << d_mode
    << " for element: " << elem << EndLogLine;
#endif
}

void FFSiteHandler::endElement(const char* elem)
{
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFSITEHANDLER )
    << "FFSiteHandler::endElement mode = " << d_mode
    << " before mode reset"
    << " for element: " << elem << EndLogLine;
#endif
  d_mode = UNKNOWN;
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFSITEHANDLER )
    << "FFSiteHandler::endElement mode = " << d_mode
    << " after mode reset"
    << " for element: " << elem << EndLogLine;
#endif
}

void FFSiteHandler::charData(char* data)
{
  switch(d_mode)
    {
    default:
    case UNKNOWN:
      break;
    case NAME:
      d_name = data;
      break;
    case TYPE:
      d_type = data;
      break;
    case MASS:
      d_mass = atof(data);
      break;
    case CHARGE:
      d_charge = atof(data);
      break;
    case EPS:
      d_eps = atof(data);
      break;
    case SIGMA:
      d_sigma = atof(data);
      break;
    case EPS14:
      d_eps14 = atof(data);
      break;
    case SIGMA14:
      d_sigma14 = atof(data);
      break;
    }

#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFSITEHANDLER )
    << "FFSiteHandler::charData in mode = " << d_mode
    << " finished with "
    << "name: " << d_name
    << " type: " << d_type
    << " mass: " << d_mass
    << " charge: " << d_charge
    << " epsilon: " << d_eps
    << " sigma: " << d_sigma
    << " 14 epsilon: " << d_eps14
    << " 14 sigma: " << d_sigma14
    << EndLogLine;
#endif
}

void FFSiteHandler::initialize()
{}

void FFSiteHandler::finalize()
{
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFSITEHANDLER )
    << "name: " << d_name
    << " type: " << d_type
    << " mass: " << d_mass
    << " charge: " << d_charge
    << " epsilon: " << d_eps
    << " sigma: " << d_sigma
    << " 14 epsilon: " << d_eps14
    << " 14 sigma: " << d_sigma14
    << EndLogLine;
#endif
  SiteSpecListHandler::instance()->push(d_name, d_type, d_mass,
                                        d_charge, d_eps, d_sigma,
                                        d_eps14, d_sigma14);
}
