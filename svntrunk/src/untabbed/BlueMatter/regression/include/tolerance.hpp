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
 // ****************************************************************
// File: tolerance.hpp
// Author: RSG
// Date: October 11, 2006
// Namespace: trc
// Class: Tolerance
// Description: Singleton to hold the diff tolerance above which we
//              will print out additional diagnostic information
// ****************************************************************
#ifndef INCLUDE_TRC_TOLERANCE_HPP
#define INCLUDE_TRC_TOLERANCE_HPP

#include <iostream>
#include <cmath>
#include <cstdlib>

namespace trc
{
  class Tolerance
  {
  private:
    static Tolerance* s_instance_p;
    double d_tolerance;
    Tolerance(const double tolerance = -1) : d_tolerance(tolerance)
    {}
  public:
    static Tolerance* instance()
    {
      if (s_instance_p == NULL)
  {
    std::cerr << "trc::Tolerance::instance() called without prior initialization" << std::endl;
    s_instance_p = new Tolerance();
    if (s_instance_p == NULL)
      {
        std::cerr << "trc::Tolerance::instance() error allocating s_instance_p" << std::endl;
        std::exit(-1);
      }
  }
      return(s_instance_p);
    }

    inline double getTolerance() const
    {
      return(d_tolerance);
    }

    inline bool inTolerance(const double& diff) const
    {
      return((d_tolerance < 0) || (std::fabs(diff) < d_tolerance));
    }

    static void setTolerance(const double);
  };
}
#endif
