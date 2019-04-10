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
// File: ffparametersinterface.hpp
// Author: RSG
// Date: April 25, 2001
// Class: FFParametersInterface
// Description:  This class provides an interface to the problem setup
//               code providing accessor methods to information
//               required for setup.  Such information includes per
//               atomsite information, bond, angle, and torsion
//               parameters.
// ************************************************************************

#ifndef INCLUDE_FFPARAMETERSINTERFACE_HPP
#define INCLUDE_FFPARAMETERSINTERFACE_HPP


struct ForceFieldSite
{
  double d_mass;
  double d_charge;
  int d_ljType;
};

struct ForceFieldBond
{
  double d_k;
  double d_r0;
  int d_site_1;
  int d_site_2;
};

struct ForceFieldAngle
{
  double d_k;
  double d_th0;
  double d_ku;
  double d_ru;
  int d_site_1;
  int d_site_2;
  int d_site_3;
};

struct ForceFieldTorsion
{
  double d_v;
  double d_th0;
  int d_order;
  int d_site_1;
  int d_site_2;
  int d_site_3;
  int d_site_4;
};

struct ForceFieldLJParam
{
  double d_epsilon;
  double d_sigma;
};

class ostream;

ostream& operator<<(ostream&, const ForceFieldSite&);
ostream& operator<<(ostream&, const ForceFieldBond&);
ostream& operator<<(ostream&, const ForceFieldAngle&);
ostream& operator<<(ostream&, const ForceFieldTorsion&);
ostream& operator<<(ostream&, const ForceFieldLJParam&);

class FFParametersInterface
{
  friend ostream& operator<<(ostream&, const FFParametersInterface&);

protected:
  virtual ostream& output(ostream&) const = 0;
public:
  //enums

  enum {CHARMM_TYPE = 0, OPLSAA_TYPE = 1};
  // typedefs
  typedef ForceFieldSite Site;
  typedef ForceFieldBond Bond;
  typedef ForceFieldAngle Angle;
  typedef ForceFieldTorsion Torsion;
  typedef ForceFieldLJParam LJParam;

  // methods

  // destructor
  virtual ~FFParametersInterface();

  // initialization
  virtual void init(const char*) = 0; // takes file name as argument

  // accessors (all access methods do bound-checking and all indexes
  // start at 0)

  // Header information
  virtual const char* forceFieldFamily() const = 0; // like AMBER
  virtual const char* forceFieldSpec() const = 0; // like AMBER96

  virtual int charge14() const = 0;
  virtual double charge14Scale() const = 0;

  virtual int improperFlag() const = 0;
  virtual int torsionInputFlag() const = 0;

  virtual int urey_bradleyFlag() const = 0;
  virtual int groupingFlag() const = 0;

  virtual const char* waterModel() const = 0;

  virtual int unitsFlag() const = 0;

  // force field parameters
  virtual int siteCount() const = 0;
  virtual const Site& site(int) const = 0;

  virtual int bondCount() const = 0;
  virtual const Bond& bond(int) const = 0;

  virtual int angleCount() const = 0;
  virtual const Angle& angle(int) const = 0;

  virtual int torsionType() const = 0;
  virtual int torsionCount() const = 0;
  virtual const Torsion& torsion(int) const = 0;
  
  virtual int improperTorsionType() const = 0;
  virtual int improperTorsionCount() const = 0;
  virtual const Torsion& improperTorsion(int) const = 0;

  virtual int LJTypeCount() const = 0;
  
  virtual const LJParam& LJNormal(int, int) const = 0;
  virtual const LJParam& LJ14(int, int) const = 0;
  
};

ostream& operator<<(ostream&, const FFParametersInterface&);
#endif
