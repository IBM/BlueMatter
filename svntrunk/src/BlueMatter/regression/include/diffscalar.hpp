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
// File: diffscalar.hpp
// Author: RSG
// Date: October 29, 2006
// Namespace: fp
// function: diffScalar
// Description: Computes difference between two floating point numbers
//              and expresses their difference as the difference
//              between their representation as binary fractions with
//              the same exponent.
// ***************************************************************

#ifndef INCLUDE_FP_DIFFSCALAR_HPP
#define INCLUDE_FP_DIFFSCALAR_HPP

#include <cmath>
#include <stdint.h>
namespace fp
{

  // Master typedefs to establish floating point and fixed point types
  typedef double t_float;
  typedef int32_t t_fixed;

  // returns difference of floating point fractions (normalized)
  inline const t_float diffScalar(const t_float&, const t_float&);

  const t_float diffScalar(const t_float& q, const t_float& r)
  {
    int qexp, rexp;
    double qfrac, rfrac;
    qfrac = std::frexp(q, &qexp);
    rfrac = std::frexp(r, &rexp);

    if (qexp == rexp)
      {
	return(qfrac - rfrac);
      }

    if (qexp > rexp)
      {
	return(std::ldexp(qfrac, qexp - rexp) - rfrac);
      }

    return(qfrac - std::ldexp(rfrac, rexp - qexp));
  }
}

#endif
