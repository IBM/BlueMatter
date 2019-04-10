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
// File: uniform.hpp
// Author: RSG
// Date: February 8, 2004
// Class: Uniform
// Description: Pseudo-random number generator implemented using
//              Algorithm 266 (M.C. Pike and I.D. Hill, Comm. ACM
//              vol. 8, number 10, October 1965, p605).  This
//              implementation returns 
// ********************************************************************

#ifndef INCLUDE_UNIFORM_HPP
#define INCLUDE_UNIFORM_HPP

class Uniform
{
private:
  enum {LIMIT = 67108863};
  enum {LIMIT_PLUS = LIMIT+1};
  enum {MULTIPLIER = 3125};
  long long d_y;
  const double d_invLimit;
public:
  Uniform(const int);
  inline double operator()();
};

Uniform::Uniform(const int y) : d_y(y % LIMIT), d_invLimit(1.0/LIMIT_PLUS)
{
  if (d_y % 2 == 0)
    {
      ++d_y;
    }
}

double Uniform::operator()()
{
  d_y = MULTIPLIER*d_y;
  d_y = d_y - (d_y/LIMIT_PLUS)*LIMIT_PLUS;
  double r = d_y*d_invLimit;
  return(r);
}
#endif
