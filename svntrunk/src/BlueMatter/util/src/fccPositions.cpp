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
 // ***************************************************************
// File: fccPositions.cpp
// Date: May 19, 2003
// Author: RSG
// Description: program to set up fcc lattice of particles to
//              specified density.  I believe that all one needs to is
//              to put out a set of ED_DynamicVariablesSite packets
//
// Command line arguments:
//   ncells (fcc cell count in each dimension)
//   density (in kg/m^3)
//
// Notes: Argon liquid density at boiling point 1.013 bar, 15 C is
// 1392.8 kg/m^3
// ***************************************************************

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/math.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/fccgen.hpp>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>

int main(int argc, char** argv)
{
  std::vector<XYZ> position;
  int ncells = 8;
  double density = 1000;
  double mass = 1;

  switch(argc)
    {
    default:
      std::cerr << argv[0]
		<< " ncells(in each dimension) density(kg/m^3) particleMass(amu)"
		<< std::endl;
      exit(-1);
    case 4:
      mass = std::atof(argv[3]);
      density = std::atof(argv[2]);
      ncells = std::atoi(argv[1]);
      break;
    }

  int n = FCCGen::generate(ncells, position);

  // convert from kg/m^3 to amu/AA^3
  density = density*1e-30/SciConst::kg2amu;
  double massTotal = n*mass;
  double volumeTotal = massTotal/density;
  double third = 1.0/3.0;
  double side = pow(volumeTotal, third);

  char buffer[128];
  int siteID = 0;
  // put out dvs file to stdout
  for (std::vector<XYZ>::iterator iter = position.begin();
       iter != position.end();
       ++iter)
    {
      *iter = (*iter)*side; // rescale to correct box size
      ED_FillPacket_DynamicVariablesSite(buffer, 0, 0, siteID, *iter, XYZ::ZERO_VALUE());
      std::cout.write(buffer, ED_DynamicVariablesSite::PackedSize);
      ++siteID;
    }

  if (n != siteID)
    {
      std::cerr << "Inconsistency in site counts" << std::endl;
      return -1;
    }
  
  std::cerr << n << " sites in file" << std::endl;
  std::cerr << std::setprecision(15) << side << " Angstroms/side" << std::endl;

  return 0;
}
