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
// File: bmtdmatrix.cpp
// Author: RSG
// Date: September 28, 2002
// Namespace: bmt
// Class: DMatrix
// Description: Class encapsulating the computation of the distance
//              matrix describing the distances between sites
//              specified as a vector of integers
// ********************************************************************

#include <BlueMatter/bmtdmatrix.hpp>
#include <cmath>
#include <iostream>

namespace bmt
{
  DMatrix::DMatrix(const std::vector<int>& site) : d_site(site)
  {
    d_distance.resize(d_site.size()*(d_site.size()-1)/2);
    d_map.reserve(d_site.size()*(d_site.size()-1)/2);
    std::cerr << "bmt::DMatrix::DMatrix() for size = " <<
      d_distance.size() << std::endl;

    for (int i=1; i < d_site.size(); ++i)
      {
	for (int j=0; j < i; ++j)
	  {
	    std::pair<int,int> sitePair(i,j);
	    d_map.push_back(sitePair);
	  }
      }
  }

  const std::vector<float>& DMatrix::operator()(const
						std::vector<Position>&
						frame)
  {
    int offset = 0;
    for (int i=1; i < d_site.size(); ++i)
      {
	for (int j=0; j < i; ++j)
	  {
	    float dx = frame[i].d_x - frame[j].d_x;
	    float dy = frame[i].d_y - frame[j].d_y;
	    float dz = frame[i].d_z - frame[j].d_z;
	    d_distance[offset]=std::sqrt(dx*dx+dy*dy+dz*dz);
	    ++offset;
	  }
      }
    return(d_distance);
  }

  const std::vector<std::pair<int,int> >& DMatrix::pairMap() const
  {
    return d_map;
  }

}
