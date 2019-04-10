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
// File: NormalDeviate.hpp
// Author: RSG
// Date: November 26, 2007
// Namespace: rng
// Class: NormalDeviate
// Template Parameters: TRng (a random number generator function
//                      object that generates a random number in the
//                      half-open interval [0,1). TRng should be
//                      advanced by the prefix increment operator ++
//
// Description: The implementation follows the description of Algorithm P
//              in: D.E. Knuth, Seminumerical Algorithms, 3rd Edition, p122.
//              
// **********************************************************************

#ifndef INCLUDE_RNG_NORMAL_DEVIATE_HPP
#define INCLUDE_RNG_NORMAL_DEVIATE_HPP

#include "RandomNumberGeneratorIf.hpp"

#if defined(DEBUG)
#include <iostream>
#endif

#if ( defined( NO_PK_PLATFORM ) || defined( RANDOM_FOR_FFT ))
#include <cmath>
#define log_func std::log
#else
#include <BlueMatter/math.hpp>
#define log_func Math::hlog
#endif

namespace rng
{
  template <class TRng>
  struct NormalDeviateState
  {
    NormalDeviateState();
    typename TRng::state mRngState;
    double mNormalDeviate;
    // haveDeviate should be set to false when starting from scratch
    // rather than initializing from stored state
    bool mHaveDeviate;
    int mSeed; // Only used to indicate how the RNG should be initialized
  };

  template <class TRng>
  class NormalDeviate
  {
  private:
    TRng mUniform;
    double mD1;
    double mD2;
    bool mHaveDeviate; // true if mD2 is available
    mutable NormalDeviateState<TRng> mState; // for readout of state only
  public:
    typedef NormalDeviateState<TRng> state;
    void initialize( const NormalDeviateState<TRng>& = 
                     NormalDeviateState<TRng>() );
    void initializeFromSeed( const unsigned );
    double operator()();
    const NormalDeviateState<TRng>& getState() const;
  };

  template <class TRng>
  NormalDeviateState<TRng>::NormalDeviateState()
  {
    mHaveDeviate = false;
  }

  template <class TRng>
  void NormalDeviate<TRng>::initialize(const NormalDeviateState<TRng>& s)
  {
    RandomNumberGeneratorIf<TRng,typename TRng::state>& rng = 
      static_cast<RandomNumberGeneratorIf<TRng, typename TRng::state>&>(mUniform);
#if defined(DEBUG)
    std::cout << "DEBUG: s.mRngState: " << s.mRngState << std::endl;
#endif
    rng.initFromState( s.mRngState );
#if defined(DEBUG)
    std::cout << "DEBUG: rng.getState(): " << rng.getState() << std::endl;
#endif
    mHaveDeviate = s.mHaveDeviate;
    mD2 = s.mNormalDeviate;
  }

  template <class TRng>
  void NormalDeviate<TRng>::initializeFromSeed( const unsigned seed )
  {
    mHaveDeviate = false;
    typename TRng::state tempState;
    tempState.setState( seed );
    mUniform.initFromState( tempState );
  }

  template <class TRng>
  double NormalDeviate<TRng>::operator()()
  {
    if ( mHaveDeviate )
      {
        mHaveDeviate = false;
        return( mD2 );
      }

    RandomNumberGeneratorIf<TRng, typename TRng::state>& rng = 
      static_cast<RandomNumberGeneratorIf<TRng, typename TRng::state>&>(mUniform);
    double v1 = 0;
    double v2 = 0;
    double s = 2;
    do
      {
        v1 = 2*rng.uniform() - 1;
        v2 = 2*rng.uniform() - 1;
        s = v1*v1 + v2*v2;
#if defined(DEBUG)
        std::cout << "v1: " << v1 << " v2: " << v2 << " s: " << s << std::endl;
#endif
      }
    while ( ( s >= 1 ) || ( s == 0 ) );
    double f = sqrt(-2.0*log_func(s)/s);
#if defined(DEBUG)
    std::cout << "f: " << f << std::endl;
#endif
    mD1 = v1*f;
    mD2 = v2*f;

    mHaveDeviate = true;
    return( mD1 );
  }

  template <class TRng>
  const NormalDeviateState<TRng>& NormalDeviate<TRng>::getState() const
  {
    const RandomNumberGeneratorIf<TRng, typename TRng::state>& rng = 
      static_cast<const RandomNumberGeneratorIf<TRng, typename TRng::state>&>(mUniform);
    mState.mRngState = rng.getState();
    mState.mHaveDeviate = mHaveDeviate;
    mState.mNormalDeviate = mD2;
    return( mState );
  }
}

#endif
