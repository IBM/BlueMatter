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
// File: FFParameters.hpp
// Author: RSG
// Date: April 25, 2001
// Class: FFParameters
// Inheritance: FFParametersInterface
// Description:  This class implements the methods to provide force
//               field parameter information via the interface defined
//               by FFParametersInterface
// ************************************************************************

#ifndef INCLUDE_FFPARAMETERS_HPP
#define INCLUDE_FFPARAMETERS_HPP

#include "ffparametersinterface.hpp"

class FFParameters_i;

class FFParameters : public FFParametersInterface
{
  friend class FFParameters_i;
private:
  // data

  char* d_ffFamily;
  char* d_ffSpec;
  int d_charge14;
  double d_charge14Scale;
  int d_improperFlag;
  int d_torsionInputFlag;
  int d_ureyBradleyFlag;
  int d_groupingFlag;
  char* d_waterModel;
  int d_unitsFlag;

  
  int d_siteCount;
  Site* d_site;
  
  int d_bondCount;
  Bond* d_bond;

  int d_angleCount;
  Angle* d_angle;
  
  int d_torsionType;
  int d_torsionCount;
  Torsion* d_torsion;

  int d_improperTorsionType;
  int d_improperTorsionCount;
  Torsion* d_improperTorsion;

  int d_LJTypeCount;

  LJParam** d_ljNormal;
  LJParam** d_lj14;

  // methods

  void clear();
  ostream& output(ostream&) const;

public:
  // methods

  // static test method

  static int main(int argc, char** argv);

  // constructor
  FFParameters();

  // Destructor
  ~FFParameters();

  // initialization
  void init(const char*); // takes file name as argument

  // accessors (all access methods do bound-checking and all indexes
  // start at 0)

  // Header information
  const char* forceFieldFamily() const; // like AMBER
  const char* forceFieldSpec() const; // like AMBER96

  int charge14() const;
  double charge14Scale() const;

  int improperFlag() const;
  int torsionInputFlag() const;

  int urey_bradleyFlag() const;
  int groupingFlag() const;

  const char* waterModel() const;

  int unitsFlag() const;

  // force field parameters
  int siteCount() const;
  const Site& site(int) const;

  int bondCount() const;
  const Bond& bond(int) const;

  int angleCount() const;
  const Angle& angle(int) const;

  int torsionType() const;
  int torsionCount() const;
  const Torsion& torsion(int) const;
  
  int improperTorsionType() const;
  int improperTorsionCount() const;
  const Torsion& improperTorsion(int) const;

  int LJTypeCount() const;
  
  const LJParam& LJNormal(int, int) const;
  const LJParam& LJ14(int, int) const;

};

#endif
