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
// File: headerhandler.hpp
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

#ifndef INCLUDE_HEADERHANDLER_HPP
#define INCLUDE_HEADERHANDLER_HPP

  enum {FF_UNKNOWN = -1, FF_CHARMM = 1, FF_OPLSAA = 2}; // force field types

class HeaderHandler
{
private:
  // data
  static HeaderHandler* s_instance_p;
  int d_charge;
  int d_vdw;
  int d_vdwCombine;
  int d_charge14;
  double d_charge14Scale;
  int d_vdw14;
  double d_vdw14Scale;
  int d_improper;
  int d_torsionInput;
  int d_ureyBradley;
  int d_grouping;
  int d_water;
  int d_units;

  // methods
  HeaderHandler();
public:
  // enums
  // methods
  static HeaderHandler* instance();

  int getCharge() const;
  int getVDW() const;
  int getVDWCombine() const;

  int getCharge14() const;
  double getCharge14Scale() const;
  int getVDW14() const;
  double getVDW14Scale() const;
  int getImproper() const;
  int getTorsionInput() const;
  int getUreyBradley() const;
  int getGrouping() const;
  int getWater() const;
  int getUnits() const;

  void setCharge(const int);
  void setVDW(const int);
  void setVDWCombine(const int);
  void setCharge14(const int);
  void setCharge14Scale(const double);
  void setVDW14(const int);
  void setVDW14Scale(const double);
  void setImproper(const int);
  void setTorsionInput(const int);
  void setUreyBradley(const int);
  void setGrouping(const int);
  void setWater(const int);
  void setUnits(const int);

  void initialize(); // called when start element <header> is encountered
  void finalize(); // called when end element </header> is encountered
};

#endif
