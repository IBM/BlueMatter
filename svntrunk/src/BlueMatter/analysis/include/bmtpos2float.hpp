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
// File: bmtpos2float.hpp
// Author: RSG
// Date: March 5, 2002
// Namespace: bmt
// Class: Pos2Float
// Description: Class encapsulating the conversion of an array of bmt::position
//              to an array of floats that can be processed by bmt::Correlation
//              for use in essential dynamics calculations.
// ********************************************************************

#ifndef INCLUDE_BMTPOS2FLOAT_HPP
#define INCLUDE_BMTPOS2FLOAT_HPP

#include <BlueMatter/bmtposition.hpp>
#include <vector>
#include <functional>
#include <utility>

namespace bmt
{
  class Pos2Float : public std::unary_function<const std::vector<Position>&, const std::vector<float>&>
  {
  private:
    std::vector<int> d_site;
    std::vector<float> d_coord;
  public:
    Pos2Float(const std::vector<int>&); // argument is array of site ids
    const std::vector<float>& operator()(const std::vector<Position>&);
  };
}
#endif
