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
// File: RngMrg32k3a.hpp
// Author: RSG
// Date: December 30, 2006
// Namespace: rng
// Class: RngMrg32k3a
// Base Class: RandomNumberGeneratorIf<RngMrg32k3a>
//
// Description: Engine class providing implementation of interfaces
//              defined in RandomNumberGeneratorIf.hpp for random number
//              generation (using the curiously recursive template /
//              Barton and Nackman trick). Recall that the template
//              parameter class T (the "engine", RngMrg32k3a in this
//              case) is derived from RandomNumberGeneratorIf<T>
//
//              The implementation here is an adaptation of the
//              implementation provided in the streams00 package from
//              http://www.iro.umontreal.ca/~lecuyer/myftp/streams00/
//              and described in L'Ecuyer, P.; Simard, R.; Chen, E. &
//              Kelton, W. An Objected-Oriented Random-Number Package
//              with Many Long Streams and Substreams Operations
//              Research, 2002, 50, 1073-1075.
//
//              The copyright notice in the streams00 package states:
//              * Copyright:      Pierre L'Ecuyer, University of Montreal
//              * Notice:         This code can be used freely for personal, academic,
//              *                 or non-commercial purposes. For commercial purposes, 
//              *                 please contact P. L'Ecuyer at: lecuyer@iro.umontreal.ca
//              * Date:           14 August 2001
//
//              The underlying random number generator, MRG32k3a is
//              described in "Good Parameter Sets for Combined
//              Multiple Recursive Random Number Generators",
//              Operations Research, 47, 1 (1999), 159--164
//
// ********************************************************************

#ifndef INCLUDE_RNG_RNGMRG32K3A_HPP
#define INCLUDE_RNG_RNGMRG32K3A_HPP

#include <BlueMatter/RandomNumberGeneratorIf.hpp>

namespace rng
{
  class RngMrg32k3a;

  class RngMrg32k3aState
  {
    friend std::ostream& operator<<(std::ostream&, const RandomNumberGeneratorStateIf<RngMrg32k3aState>&);
    friend class RngMrg32k3a;
  private:
    unsigned long mSeed[6];
    mutable char mSeedAsFlat[6*sizeof(unsigned long)];
  public:
    void setStateFromFlat(const char*);
    const char* getStateAsFlat() const; 
    int sizeOfStateAsFlat() const;
    std::ostream& output(std::ostream&);
  };

  class RngMrg32k3a : public RandomNumberGeneratorIf<RngMrg32k3a>
  {
  public:
    typedef RngMrg32k3aState state;

  private:
    double mSeedAsDouble[6];
    mutable state mState;
  public:

    void init();
    void initFromState(const state&);
    double unif01();
    const state& getState() const;
  };
}
#endif

