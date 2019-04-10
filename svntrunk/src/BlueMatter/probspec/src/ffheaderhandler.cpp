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
// File: ffheaderhandler.cpp
// Date: November 21, 2000
// Author: RSG
// Description: Class derived from interface class
//             FFHandler to deal with processing header
//             records that need to be "pushed" into Mike Pitman's
//             setup code
// ***************************************************************************

#ifndef PKFXLOG_FFHEADERHANDLER
#define PKFXLOG_FFHEADERHANDLER (0)
#endif

#include "BlueMatter/ffheaderhandler.hpp"
#include "BlueMatter/headerhandler.hpp"
#undef assert
//#include <pk/fxlogger.hpp>
// fxlogger has its own version of assert, so undef the system one
//#undef assert
#include <assert.h>

char* FFHeaderHandler::s_key[ELEM_COUNT] =
{"charge", "vdw", "vdwcombine", "charge14", "charge14scale", "vdw14",
 "vdw14scale", "improper", "torsioninput", "ureybradley", "grouping",
 "water", "units"};


FFHeaderHandler::~FFHeaderHandler()
{
}

void FFHeaderHandler::startElement(const char* elem)
{
  d_mode = FFHandler::findString(elem, s_key, ELEM_COUNT);
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFHEADERHANDLER )
    << "FFHeaderHandler::startElement mode = " << d_mode
    << " for element: " << elem << EndLogLine;
#endif
}

void FFHeaderHandler::endElement(const char* elem)
{
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFHEADERHANDLER )
    << "FFHeaderHandler::endElement mode = " << d_mode
    << " before mode reset"
    << " for element: " << elem << EndLogLine;
#endif
  d_mode = UNKNOWN;
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFHEADERHANDLER )
    << "FFHeaderHandler::endElement mode = " << d_mode
    << " after mode reset"
    << " for element: " << elem << EndLogLine;
#endif
}

void FFHeaderHandler::charData(char* data)
{
  switch(d_mode)
    {
    default:
    case UNKNOWN:
      break;
    case CHARGE:
      HeaderHandler::instance()->setCharge(atoi(data));
      break;
    case VDW:
      HeaderHandler::instance()->setVDW(atoi(data));
      break;
    case VDWCOMBINE:
      HeaderHandler::instance()->setVDWCombine(atoi(data));
      break;
    case CHARGE14:
      HeaderHandler::instance()->setCharge14(atoi(data));
      break;
    case CHARGE14SCALE:
      HeaderHandler::instance()->setCharge14Scale(atof(data));
      break;
    case VDW14:
      HeaderHandler::instance()->setVDW14(atoi(data));
      break;
    case VDW14SCALE:
      HeaderHandler::instance()->setVDW14Scale(atof(data));
      break;
    case IMPROPER:
      HeaderHandler::instance()->setImproper(atoi(data));
      break;
    case TORSIONINPUT:
      HeaderHandler::instance()->setTorsionInput(atoi(data));
      break;
    case UREYBRADLEY:
      HeaderHandler::instance()->setUreyBradley(atoi(data));
      break;
    case GROUPING:
      HeaderHandler::instance()->setGrouping(atoi(data));
      break;
    case WATER:
      HeaderHandler::instance()->setWater(atoi(data));
      break;
    case UNITS:
      HeaderHandler::instance()->setUnits(atoi(data));
      break;
    }

#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFHEADERHANDLER )
    << "FFHeaderHandler::charData in mode = " << d_mode
    << " finished with "
    << "charge: "
    << HeaderHandler::instance()->getCharge()
    << " vdw: "
    << HeaderHandler::instance()->getVDW()
    << " vdwcombine: "
    << HeaderHandler::instance()->getVDWCombine()
    << " charge14: "
    << HeaderHandler::instance()->getCharge14()
    << " charge14scale: "
    << HeaderHandler::instance()->getCharge14Scale()
    << " vdw14: "
    << HeaderHandler::instance()->getVDW14()
    << " vdw14scale: "
    << HeaderHandler::instance()->getVDW14Scale()
    << " improper: "
    << HeaderHandler::instance()->getImproper()
    << " torsioninput: "
    << HeaderHandler::instance()->getTorsionInput()
    << " ureybradley: "
    << HeaderHandler::instance()->getUreyBradley()
    << " grouping: "
    << HeaderHandler::instance()->getGrouping()
    << " water: "
    << HeaderHandler::instance()->getWater()
    << " units: "
    << HeaderHandler::instance()->getUnits()
    << EndLogLine;
#endif
}

void FFHeaderHandler::initialize()
{
  HeaderHandler::instance()->initialize();
}

void FFHeaderHandler::finalize()
{
  HeaderHandler::instance()->finalize();
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_FFHEADERHANDLER )
    << "charge: "
    << HeaderHandler::instance()->getCharge()
    << " vdw: "
    << HeaderHandler::instance()->getVDW()
    << " vdwcombine: "
    << HeaderHandler::instance()->getVDWCombine()
    << " charge14: "
    << HeaderHandler::instance()->getCharge14()
    << " charge14scale: "
    << HeaderHandler::instance()->getCharge14Scale()
    << " vdw14: "
    << HeaderHandler::instance()->getVDW14()
    << " vdw14scale: "
    << HeaderHandler::instance()->getVDW14Scale()
    << " improper: "
    << HeaderHandler::instance()->getImproper()
    << " torsioninput: "
    << HeaderHandler::instance()->getTorsionInput()
    << " ureybradley: "
    << HeaderHandler::instance()->getUreyBradley()
    << " grouping: "
    << HeaderHandler::instance()->getGrouping()
    << " water: "
    << HeaderHandler::instance()->getWater()
    << " units: "
    << HeaderHandler::instance()->getUnits()
    << EndLogLine;
#endif
}
