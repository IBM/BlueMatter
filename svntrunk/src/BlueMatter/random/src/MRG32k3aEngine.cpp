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
// File: MRG32k3aEngine.cpp
// Date: January 5, 2008
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

#include "BlueMatter/MRG32k3aEngine.hpp"


const double MRG32k3aEngine::cM0 = 4294967087.0;
const double MRG32k3aEngine::cM1 = 4294944443.0;
const double MRG32k3aEngine::cA02 =   1403580.0;
const double MRG32k3aEngine::cA03 =   -810728.0;
const double MRG32k3aEngine::cA11 =    527612.0;
const double MRG32k3aEngine::cA13 =  -1370589.0;
const double MRG32k3aEngine::cNorm = 2.328306549295727688239738e-10; // 1.0/(cM0+1)
const double MRG32k3aEngine::cM0Inv = 1.0/cM0;
const double MRG32k3aEngine::cM1Inv = 1.0/cM1;


// rotate variable stored in Tuint left by number of bits in
// second argument 
template<typename Tuint>
inline Tuint rol( const Tuint, const unsigned long );


std::ostream& MRG32k3aEngineState::output( std::ostream& os ) const
{
  os << mX0_N0 << ":"
     << mX0_N1 << ":"
     << mX0_N2 << ":"
     << mX1_N0 << ":"
     << mX1_N1 << ":"
     << mX1_N2;
  return os;
}

void MRG32k3aEngineState::setState( const unsigned seed )
{
  // A better way to handle this would be to use the seed as the
  // index of the substream to be used--that would guarantee no 
  // overlap between between engines initialized with different
  // seeds (at least for less than 2**76 iterations)
  mX0_N0 = 12345;
  mX0_N1 = 12345;
  mX0_N2 = 12345;
  mX1_N0 = 12345;
  mX1_N1 = 12345;
  mX1_N2 = 12345;

  if ( seed == 0 )
    {
      return;
    }

  unsigned mask = rol( seed, 5 );
  mX0_N0 =  mX0_N0 ^ mask;
  mask = rol( seed, 10 );
  mX0_N1 =  mX0_N1 ^ mask;
  mask = rol( seed, 15 );
  mX0_N2 =  mX0_N2 ^ mask;
  mask = rol( seed, 20 );
  mX1_N0 =  mX0_N0 ^ mask;
  mask = rol( seed, 25 );
  mX1_N1 =  mX0_N1 ^ mask;
  mask = rol( seed, 30 );
  mX1_N2 =  mX0_N2 ^ mask;
}


double MRG32k3aEngine::uniform()
{
  // First component
  double s0 = cA02*mX0_N1 + cA03*mX0_N0;
  int32_t i = static_cast<int32_t>(s0*cM0Inv);
  s0 -= i*cM0;
  s0 = s0 < 0 ? s0 + cM0 : s0;
  mX0_N0 = mX0_N1;
  mX0_N1 = mX0_N2;
  mX0_N2 = s0;

  // Second component
  double s1 = cA11*mX1_N2 + cA13*mX1_N0;
  i = static_cast<int32_t>(s1*cM1Inv);
  s1 -= i*cM1;
  s1 = s1 < 0 ? s1 + cM1 : s1;
  mX1_N0 = mX1_N1;
  mX1_N1 = mX1_N2;
  mX1_N2 = s1;

  // Combination of components
  return( s0 <= s1 ? 
          ( s0 - s1 + cM0 )*cNorm :
          ( s0 - s1 )*cNorm );
}


template<typename Tuint>
Tuint rol( const Tuint n, const unsigned long s )
{
  return( ( n << s ) | ( n >> ( 8*sizeof(Tuint) - s ) ) );
}
