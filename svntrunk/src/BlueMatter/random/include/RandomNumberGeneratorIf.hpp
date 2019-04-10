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
// File: RandomNumberGeneratorIf.hpp
// Author: RSG
// Date: December 30, 2006
// Namespace: rng
// Class: RandomNumberGeneratorIf
// Description: Base class defining interface for Random Number
//              Generation (using the curiously recursive template /
//              Barton and Nackman trick). Recall that the template
//              parameter class T (the "engine") will be derived from
//              RandomNumberGeneratorIf<T>
// Template parameter: T
// 
// ********************************************************************

#ifndef INCLUDE_RNG_RANDOMNUMBERGENERATORIF_HPP
#define INCLUDE_RNG_RANDOMNUMBERGENERATORIF_HPP

#include <iostream>

namespace rng
{
  // The template parameter S needs to be a "flat" class
  template <typename S>
  class RandomNumberGeneratorStateIf
  {
//      friend std::ostream& operator<<(std::ostream&, const RandomNumberGeneratorStateIf<S>&);
  private:
    // methods not to be implemented by engine class S
    inline S& asEngine();
    inline const S& asEngine() const;
  protected:
    inline std::ostream& output( std::ostream& ) const;
  public:
    inline void setState( const S& );
    inline void setState( const unsigned = 0 );
    inline const S& getState() const;
  };

  template <typename S>
  std::ostream& operator<<(std::ostream&, const RandomNumberGeneratorStateIf<S>&);

  template <typename T, typename S>
  class RandomNumberGeneratorIf
  {
  private:
    // methods not to be implemented by engine class T
    inline T& asEngine();
    inline const T& asEngine() const;

  public:
    typedef S state;
    //    typedef typename T::state state;

    //    class state : public T::state
    //    {};
    // methods
    // methods that must be implemented by engine class T
    inline void initFromState( const state& );
    inline const state& getState() const;
    inline double uniform(); // uniform distribution on the unit interval
  };
  
  template <typename S>
  S& RandomNumberGeneratorStateIf<S>::asEngine()
  {
    return static_cast<S&>( *this );
  }

  template <typename S>
  const S& RandomNumberGeneratorStateIf<S>::asEngine() const
  {
    return static_cast<const S&>( *this );
  }

  template <typename S>
  std::ostream& operator<<( std::ostream& os, const RandomNumberGeneratorStateIf<S>& st )
  {
    return( st.output( os ) );
  }

  template <typename S>
  std::ostream& RandomNumberGeneratorStateIf<S>::output( std::ostream& os ) const
  {
    return( asEngine().output( os ) );
  }

  template <typename S>
  void RandomNumberGeneratorStateIf<S>::setState( const S& s )
  {
    asEngine().setState( s );
  }

  template <typename S>
  void RandomNumberGeneratorStateIf<S>::setState( const unsigned seed )
  {
    asEngine().init( seed );
  }

  template <typename S>
  const S& RandomNumberGeneratorStateIf<S>::getState() const
  {
    return( asEngine.getState() );
  }

  template <typename T, typename S>
  void RandomNumberGeneratorIf<T,S>::initFromState( const typename RandomNumberGeneratorIf<T,S>::state& s )
  {
    asEngine().initFromState( s );
  }

  template <typename T, typename S>
  double RandomNumberGeneratorIf<T,S>::uniform()
  {
    return( asEngine().uniform() );
  }

  template <typename T, typename S>
  const typename RandomNumberGeneratorIf<T,S>::state& RandomNumberGeneratorIf<T,S>::getState() const
  {
    return( asEngine().getState() );
  }
  
  template <typename T, typename S>
  T& RandomNumberGeneratorIf<T,S>::asEngine()
  {
    return static_cast<T&>( *this );
  }

  template <typename T, typename S>
  const T& RandomNumberGeneratorIf<T,S>::asEngine() const
  {
    return static_cast<const T&>( *this );
  }

}
#endif
