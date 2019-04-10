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
 // ***********************************************************************
// File: fccgen.cpp
// Date: May 19, 2003
// Author: RSG
// Class: FCCGen
// Description: Class encapsulating the generation of an array of
//              positions on an FCC lattice.  Code is strongly
//              influenced by the Allen & Tildesley routine available
//              from ftp://ftp.dl.ac.uk/ccp5/ALLEN_TILDESLEY/F.23
//              This routine generates positions within a unit cube
//              centered at the origin.
// ***********************************************************************

#include <BlueMatter/fccgen.hpp>
#include <cmath>
#include <iostream>
#include <cassert>

int FCCGen::generate(int nCells, std::vector<XYZ>& position)
{
  position.erase(position.begin(), position.end());
  int particleCount = 4*nCells*nCells*nCells;
  position.reserve(particleCount);

  double a = 1.0/nCells;
  double aHalf = 0.5*a;

  XYZ u0 = {0, 0, 0};
  XYZ u1 = {aHalf, aHalf, 0};
  XYZ u2 = {0, aHalf, aHalf};
  XYZ u3 = {aHalf, 0, aHalf};
  
  XYZ c, delta;
  delta = XYZ::ZERO_VALUE();
  XYZ offset = {-0.5, -0.5, -0.5};

  // set up particles and translate center of simulation volume to origin
  for (int i = 0; i < nCells; ++i)
    {
      delta.mX = a*i;
      for (int j = 0; j < nCells; ++j)
  {
    delta.mY = a*j;
    for (int k = 0; k < nCells; ++k)
      {
        delta.mZ = a*k;
        c = u0 + delta + offset;
        if ((abs(c.mX) > 0.5) || (abs(c.mY) > 0.5) || (abs(c.mZ) > 0.5))
    {
      std::cerr << "Point out of bounds: " << c << std::endl;
    }
        position.push_back(c);
        c = u1 + delta + offset;
        if ((abs(c.mX) > 0.5) || (abs(c.mY) > 0.5) || (abs(c.mZ) > 0.5))
    {
      std::cerr << "Point out of bounds: " << c << std::endl;
    }
        position.push_back(c);
        c = u2 + delta + offset;
        if ((abs(c.mX) > 0.5) || (abs(c.mY) > 0.5) || (abs(c.mZ) > 0.5))
    {
      std::cerr << "Point out of bounds: " << c << std::endl;
    }
        position.push_back(c);
        c = u3 + delta + offset;
        if ((abs(c.mX) > 0.5) || (abs(c.mY) > 0.5) || (abs(c.mZ) > 0.5))
    {
      std::cerr << "Point out of bounds: " << c << std::endl;
    }
        position.push_back(c);

        delta.mZ += a;
      }
    delta.mY += a;
  }
      delta.mX += a;
    }
  assert(position.size() == particleCount);
  return particleCount;
}
