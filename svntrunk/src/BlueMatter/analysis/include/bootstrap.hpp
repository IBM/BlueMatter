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
 // **********************************************************************
//                       CVS INFORMATION
//
// SOURCE: $Source$
// REVISION: $Revision: 3171 $
// DATE: $Date: 2002-05-07 21:01:50 -0400 (Tue, 07 May 2002) $
// AUTHOR: $Author: suits $
// TAG NAME (if applicable): $Name$
// **********************************************************************

// **********************************************************************
// File: bootstrap.hpp
// Author: RSG
// Date: April 17, 2002
// Class: Bootstrap
// Template Parameter: TDerived (function object taking a const reference
//                     to a vector<double> as an argument and
//                     returning a double
// Description: class encapsulating the operations associated with the
//              "bootstrap" procedure used to estimate uncertainties
//              of a function defined on a set of iid values.
// **********************************************************************

#ifndef INCLUDE_BOOTSTRAP_HPP
#define INCLUDE_BOOTSTRAP_HPP

#include <vector>

#include <cstdlib>
#include <numeric>
#include <cmath>
#include <sys/time.h>
#include <sys/types.h>

class Bootstrap
{
private:
  const std::vector<double>& d_data;
  std::vector<double> d_syntheticDerived;
  
public:
  // methods
  Bootstrap(const std::vector<double>&);
  ~Bootstrap();
  const std::vector<double>& eval(int);
};


#endif
