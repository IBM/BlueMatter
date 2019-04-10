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
 // *****************************************************************************
// File: permutedtriple.hpp
// Author: RSG
// Date: July 11, 2003
// Class: triple
// Template parameters: TType
// Class: Permutation
// Template parameters: int first, int second, int third
// Description: template class encapsulating permutation of three items
// Class: PermutedTriple
// Template parameters: TTriple, TPermutation
// Description: Template function to return specified permutations of
//              an input triple
// *****************************************************************************
#ifndef INCLUDE_PERMUTEDTRIPLE_HPP
#define INCLUDE_PERMUTEDTRIPLE_HPP

template <int first, int second, int third>
struct Permutation
{
  enum {FIRST = first, SECOND = second, THIRD = third};
};

template <class TType>
struct Triple
{
  typedef TType triple_type;
  TType first;
  TType second;
  TType third;

  Triple(TType t1, TType t2, TType t3)
    : first(t1), second(t2), third(t3)
  {}
};

template <class TTriple, class TPermutation>
class PermutedTriple
{
public:
  static inline typename TTriple::triple_type first(TTriple&);
  static inline typename TTriple::triple_type second(TTriple&);
  static inline typename TTriple::triple_type third(TTriple&);
};

template <class TTriple>
class PermutedTriple<TTriple, Permutation<2,1,3> >
{
public:
  static inline typename TTriple::triple_type first(TTriple&);
  static inline typename TTriple::triple_type second(TTriple&);
  static inline typename TTriple::triple_type third(TTriple&);
};

template <class TTriple>
class PermutedTriple<TTriple, Permutation<3,1,2> >
{
public:
  static inline typename TTriple::triple_type first(TTriple&);
  static inline typename TTriple::triple_type second(TTriple&);
  static inline typename TTriple::triple_type third(TTriple&);
};

template <class TTriple>
class PermutedTriple<TTriple, Permutation<1,3,2> >
{
public:
  static inline typename TTriple::triple_type first(TTriple&);
  static inline typename TTriple::triple_type second(TTriple&);
  static inline typename TTriple::triple_type third(TTriple&);
};

template <class TTriple>
class PermutedTriple<TTriple, Permutation<3,2,1> >
{
public:
  static inline typename TTriple::triple_type first(TTriple&);
  static inline typename TTriple::triple_type second(TTriple&);
  static inline typename TTriple::triple_type third(TTriple&);
};

template <class TTriple>
class PermutedTriple<TTriple, Permutation<2,3,1> >
{
public:
  static inline typename TTriple::triple_type first(TTriple&);
  static inline typename TTriple::triple_type second(TTriple&);
  static inline typename TTriple::triple_type third(TTriple&);
};

// **********************************************************
// begin template definitions
// **********************************************************

// Generic template definintion

template <class TTriple, class TPermutation>
typename TTriple::triple_type PermutedTriple<TTriple, TPermutation>::first(TTriple& triple)
{
  return triple.first;
}

template <class TTriple, class TPermutation>
typename TTriple::triple_type PermutedTriple<TTriple, TPermutation>::second(TTriple& triple)
{
  return triple.second;
}

template <class TTriple, class TPermutation>
typename TTriple::triple_type PermutedTriple<TTriple, TPermutation>::third(TTriple& triple)
{
  return triple.third;
}

// *******************************************************************

// Template definitions for partial specialization for
// Permutation<2,1,3>

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<2,1,3> >::first(TTriple& triple)
{
  return triple.second;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<2,1,3> >::second(TTriple& triple)
{
  return triple.first;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<2,1,3> >::third(TTriple& triple)
{
  return triple.third;
}

// End of template definitions for partial specialization for
// Permutation<2,1,3>

// *******************************************************************

// Template definitions for partial specialization for
// Permutation<3,1,2>

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<3,1,2> >::first(TTriple& triple)
{
  return triple.third;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<3,1,2> >::second(TTriple& triple)
{
  return triple.first;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<3,1,2> >::third(TTriple& triple)
{
  return triple.second;
}

// End of template definitions for partial specialization for
// Permutation<3,1,2>

// *******************************************************************

// Template definitions for partial specialization for
// Permutation<1,3,2>

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<1,3,2> >::first(TTriple& triple)
{
  return triple.first;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<1,3,2> >::second(TTriple& triple)
{
  return triple.third;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<1,3,2> >::third(TTriple& triple)
{
  return triple.second;
}

// End of template definitions for partial specialization for
// Permutation<1,3,2>

// *******************************************************************

// Template definitions for partial specialization for
// Permutation<3,2,1>

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<3,2,1> >::first(TTriple& triple)
{
  return triple.third;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<3,2,1> >::second(TTriple& triple)
{
  return triple.second;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<3,2,1> >::third(TTriple& triple)
{
  return triple.first;
}

// End of template definitions for partial specialization for
// Permutation<3,2,1>

// *******************************************************************

// Template definitions for partial specialization for
// Permutation<2,3,1>

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<2,3,1> >::first(TTriple& triple)
{
  return triple.second;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<2,3,1> >::second(TTriple& triple)
{
  return triple.third;
}

template <class TTriple>
typename TTriple::triple_type PermutedTriple<TTriple, Permutation<2,3,1> >::third(TTriple& triple)
{
  return triple.first;
}

// End of template definitions for partial specialization for
// Permutation<2,3,1>


#endif
