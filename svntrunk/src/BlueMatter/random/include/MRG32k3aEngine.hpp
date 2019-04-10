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
 // *************************************************************
// File: MRG32k3aEngine.hpp
// Date: October 4, 2007
// Author: RSG
// Classes:  MRG32k3aEngine
// Inheritance: RandomNumberGeneratorIf<MRG32k3aEngine>
// Description: Multiple recursive random number generator based on
//              the description in "Good Parameters and
//              Implementations for Combined Multiple Recursive Random
//              Number Generators" by P. L'Ecuyer 
//              in Section 3 and on the example floating point
//              implementation provided in Figure I.
//              http://www.iro.umontreal.ca/~lecuyer/myftp/papers/combmrg2.ps
//              A shorter version of this paper was published in
//              Operations Research, 47, 1 (1999), 159--164
//
// *************************************************************

#ifndef INCLUDE_MRG2K3AENGINE_HPP
#define INCLUDE_MRG2K3AENGINE_HPP

#include "RandomNumberGeneratorIf.hpp"

#include <stdint.h>
#include <string.h>

struct MRG32k3aEngineState : public rng::RandomNumberGeneratorStateIf<MRG32k3aEngineState>
{
  uint32_t mX0_N0;
  uint32_t mX0_N1;
  uint32_t mX0_N2;
  uint32_t mX1_N0;
  uint32_t mX1_N1;
  uint32_t mX1_N2;

  public:
  // methods
  inline MRG32k3aEngineState();
  std::ostream& output( std::ostream& ) const;
  inline void setState( const MRG32k3aEngineState& );
  void setState( const unsigned = 0 ); // set state from seed
  inline const MRG32k3aEngineState& getState() const;
};

class MRG32k3aEngine : public rng::RandomNumberGeneratorIf<MRG32k3aEngine, MRG32k3aEngineState>
{
private:
  
  // data
  double mX0_N0;
  double mX0_N1;
  double mX0_N2;
  double mX1_N0;
  double mX1_N1;
  double mX1_N2;

  mutable MRG32k3aEngineState mState; // used only for return value of getState()

  // constants

  static const double cM0;
  static const double cM1;
  static const double cA02;
  static const double cA03;
  static const double cA11;
  static const double cA13;
  static const double cNorm;
  static const double cM0Inv;
  static const double cM1Inv;

  // methods:

public:
  typedef MRG32k3aEngineState state;
  // methods
  inline void initFromState(const state&);
  inline const state& getState() const;
  double uniform(); // uniform distribution on the unit interval
};


// Implementations begin here

MRG32k3aEngineState::MRG32k3aEngineState()
{
  mX0_N0 = 12345;
  mX0_N1 = 12345;
  mX0_N2 = 12345;
  mX1_N0 = 12345;
  mX1_N1 = 12345;
  mX1_N2 = 12345;
}

void MRG32k3aEngineState::setState( const MRG32k3aEngineState& s )
{
  memcpy( this, &s, sizeof( MRG32k3aEngineState ) );
}

const MRG32k3aEngineState& MRG32k3aEngineState::getState() const
{
  return( *this );
}

void MRG32k3aEngine::initFromState(const MRG32k3aEngine::state& s)
{
  mX0_N0 = static_cast<const double>(s.mX0_N0);
  mX0_N1 = static_cast<const double>(s.mX0_N1);
  mX0_N2 = static_cast<const double>(s.mX0_N2);
  mX1_N0 = static_cast<const double>(s.mX1_N0);
  mX1_N1 = static_cast<const double>(s.mX1_N1);
  mX1_N2 = static_cast<const double>(s.mX1_N2);
}

const MRG32k3aEngine::state& MRG32k3aEngine::getState() const
{
  mState.mX0_N0 = static_cast<const uint32_t>(mX0_N0);
  mState.mX0_N1 = static_cast<const uint32_t>(mX0_N1);
  mState.mX0_N2 = static_cast<const uint32_t>(mX0_N2);
  mState.mX1_N0 = static_cast<const uint32_t>(mX1_N0);
  mState.mX1_N1 = static_cast<const uint32_t>(mX1_N1);
  mState.mX1_N2 = static_cast<const uint32_t>(mX1_N2);

  return( mState );
}


#endif
