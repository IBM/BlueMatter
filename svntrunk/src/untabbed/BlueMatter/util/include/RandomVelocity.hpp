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
 #ifndef _RANDOMVELOCITY_HPP_
#define _RANDOMVELOCITY_HPP_

// suits 20011107 will now get BoltzmanConst from one place:  SciConst
// This is defined JUST FOR THIS FILE
// #define BoltzmanConstant 0.00198718

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/math.hpp>
////#include <BlueMatter/random.hpp>

inline
XYZ RandomVelocity( double mass,double temperature )
  {
  XYZ crd;
  double btm = sqrt( SciConst::KBoltzmann_IU
         * temperature/mass);
#if 0  
  crd.mX = sqrt( BoltzmanConstant
                * temperature/mass)
                * gasdev(&rSeed);
  crd.mY = sqrt( BoltzmanConstant
                * temperature/mass)
                * gasdev(&rSeed);
  crd.mZ = sqrt( BoltzmanConstant
                * temperature/mass)
                * gasdev(&rSeed);
#endif

  crd.mX = btm * RanNumGenIF.GasDev();
  crd.mY = btm * RanNumGenIF.GasDev();
  crd.mZ = btm * RanNumGenIF.GasDev();

  return crd;
  }

// #undef BoltzmanConstant

#endif
