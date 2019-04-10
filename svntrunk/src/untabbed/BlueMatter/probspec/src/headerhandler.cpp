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
 // **************************************************************************
// File: headerhandler.cpp
// Date: November 20, 2000
// Author: RSG (taken from probspec.cpp written by Mike Pitman)
// Description: Class definitions for singleton classes implemented
//		probspec.cpp that are used to load (push) parameters
//		into the internal data structures which are then used
//		to create the flat data structure which defines the
//		"problem" for a particular molecular system and force
//		field.
//
// **************************************************************************

#include "BlueMatter/headerhandler.hpp"
#include <stdlib.h>
#include <assert.h>
#include <iostream.h>

HeaderHandler* HeaderHandler::s_instance_p = NULL;

HeaderHandler::HeaderHandler() 
{}

HeaderHandler* HeaderHandler::instance()
{
  if (s_instance_p == NULL)
    {
      s_instance_p = new HeaderHandler();
      assert(s_instance_p != NULL);
    }
  return(s_instance_p);
}

void HeaderHandler::initialize()
{}

void HeaderHandler::finalize()
{

#if 0
  cerr
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
    << endl;
#endif
}


int HeaderHandler::getCharge() const
{
  return(d_charge);
}

int HeaderHandler::getVDW() const
{
  return(d_vdw);
}

int HeaderHandler::getVDWCombine() const
{
  return(d_vdwCombine);
}

int HeaderHandler::getCharge14() const
{
  return(d_charge14);
}

double HeaderHandler::getCharge14Scale() const
{
  return(d_charge14Scale);
}

int HeaderHandler::getVDW14() const
{
  return(d_vdw14);
}

double HeaderHandler::getVDW14Scale() const
{
  return(d_vdw14Scale);
}

int HeaderHandler::getImproper() const
{
  return(d_improper);
}

int HeaderHandler::getTorsionInput() const
{
  return(d_torsionInput);
}

int HeaderHandler::getUreyBradley() const
{
  return(d_ureyBradley);
}

int HeaderHandler::getGrouping() const
{
  return(d_grouping);
}

int HeaderHandler::getWater() const
{
  return(d_water);
}

int HeaderHandler::getUnits() const
{
  return(d_units);
}

// *******************************************************

void HeaderHandler::setCharge(const int charge)
{
  d_charge = charge;
}

void HeaderHandler::setVDW(const int vdw)
{
  d_vdw = vdw;
}

void HeaderHandler::setVDWCombine(const int vdwCombine)
{
  d_vdwCombine = vdwCombine;
}

void HeaderHandler::setCharge14(const int charge14)
{
  d_charge14 = charge14;
}

void HeaderHandler::setCharge14Scale(const double charge14Scale)
{
  d_charge14Scale = charge14Scale;
}

void HeaderHandler::setVDW14(const int vdw14)
{
  d_vdw14 = vdw14;
}

void HeaderHandler::setVDW14Scale(const double vdw14Scale)
{
  d_vdw14Scale = vdw14Scale;
}

void HeaderHandler::setImproper(const int improper)
{
  d_improper = improper;
}

void HeaderHandler::setTorsionInput(const int torsionInput)
{
  d_torsionInput = torsionInput;
}

void HeaderHandler::setUreyBradley(const int ureyBradley)
{
  d_ureyBradley = ureyBradley;
}

void HeaderHandler::setGrouping(const int grouping)
{
  d_grouping = grouping;
}

void HeaderHandler::setWater(const int water)
{
  d_water = water;
}

void HeaderHandler::setUnits(const int units)
{
  d_units = units;
}

