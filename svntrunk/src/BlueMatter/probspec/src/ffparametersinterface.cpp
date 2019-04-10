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
// File: ffparametersinterface.cpp
// Author: RSG
// Date: April 25, 2001
// Class: FFParametersInterface
// Description:  This class provides an interface to the problem setup
//               code providing accessor methods to information
//               required for setup.  Such information includes per
//               atomsite information, bond, angle, and torsion
//               parameters.
// ************************************************************************

#include "ffparametersinterface.hpp"
#include <iostream.h>

ostream& operator<<(ostream& os, const FFParametersInterface& ffp)
{
  return(ffp.output(os));
}

FFParametersInterface::~FFParametersInterface()
{}

ostream& operator<<(ostream& os, const ForceFieldSite& site)
{
  os << "mass: " << site.d_mass << " ";
  os << "charge: " << site.d_charge << " ";
  os << "Lennard-Jones Type: " << site.d_ljType;
  return os;
}

ostream& operator<<(ostream& os, const ForceFieldBond& bond)
{
  os << "[" << bond.d_site_1 << ", " << bond.d_site_2 << "] ";
  os << "K: " << bond.d_k << " ";
  os << "R0: " << bond.d_r0;
  return os;
}

ostream& operator<<(ostream&os , const ForceFieldAngle& angle)
{
  os << "[" << angle.d_site_1 << ", " << angle.d_site_2
     << ", " << angle.d_site_3 << "] ";
  os << "K: " << angle.d_k << " ";
  os << "THETA0: " << angle.d_th0 << " ";
  os << "KU: " << angle.d_ku << " ";
  os << "RU: " << angle.d_ru;
  return os;
}

ostream& operator<<(ostream& os, const ForceFieldTorsion& torsion)
{
  os << "[" << torsion.d_site_1 << ", " << torsion.d_site_2
     << ", " << torsion.d_site_3
     << ", " << torsion.d_site_4 << "] ";
  os << "Order: " << torsion.d_order << " ";
  os << "Vn: " << torsion.d_v << " ";
  os << "THETA0: " << torsion.d_th0;
  return os;
}

ostream& operator<<(ostream& os, const ForceFieldLJParam& ljParam)
{
  os << "Epsilon: " << ljParam.d_epsilon << " ";
  os << "Sigma: " << ljParam.d_sigma;
  return os;
}
