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
 // ********************************************************************
// File: bmtpos2float.cpp
// Author: RSG
// Date: March 5, 2002
// Namespace: bmt
// Class: Pos2Float
// Description: Class encapsulating the conversion of an array of bmt::position
//              to an array of floats that can be processed by bmt::Correlation
//              for use in essential dynamics calculations.
// ********************************************************************

#include <BlueMatter/bmtpos2float.hpp>

namespace bmt
{
  // argument is array of site ids
  Pos2Float::Pos2Float(const std::vector<int>& site) : d_site(site)
  {
    d_coord.resize(3*d_site.size());
  }

  const std::vector<float>& Pos2Float::operator()(const std::vector<Position>& frame)
  {
    int cIndex=0;
    for (std::vector<int>::const_iterator iter = d_site.begin();
   iter != d_site.end();
   ++iter)
      {
  d_coord[cIndex] = frame[*iter].d_x;
  ++cIndex;
  d_coord[cIndex] = frame[*iter].d_y;
  ++cIndex;
  d_coord[cIndex] = frame[*iter].d_z;
  ++cIndex;
      }
    return d_coord;
  }
}
