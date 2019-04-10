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
// File: polynomial.hpp
// Author: RSG
// Date: February 8, 2008
// Class: Polynomial
// Description: Simple polynomial class primarily intended for use in
//              fitting data.
// ***************************************************************
#ifndef INCLUDE_POLYNOMIAL_HPP
#define INCLUDE_POLYNOMIAL_HPP

#include <vector>
#include <cmath>
#include <iostream>

template<typename TC>
class Polynomial;

template <typename TC>
static Polynomial<TC> operator+( const Polynomial<TC>&,
                                 const Polynomial<TC>& );

template <typename TC>
static Polynomial<TC> operator-( const Polynomial<TC>&,
                                 const Polynomial<TC>& );

template <typename TC, typename TFwdIter>
class PolyInnerProduct
{
private:
  TFwdIter d_begin;
  TFwdIter d_end;
  
public:
  typedef Polynomial<TC> vector_type;
  typedef TC return_type;
  PolyInnerProduct( const PolyInnerProduct<TC, TFwdIter>& );
  PolyInnerProduct( TFwdIter,   // beginning of domain
                    TFwdIter ); // end of domain
  // inner product of two polynomials
  TC operator()( const Polynomial<TC>&,
                 const Polynomial<TC>& );
  // inner product of polynomial and arbitrary function defined on domain
  TC operator()( const Polynomial<TC>&,
                 TFwdIter ); // beginning of function values

};

template <typename TC>
std::ostream& operator<<( std::ostream&, const Polynomial<TC>& );

template <typename TC>
class Polynomial
{
  friend std::ostream& operator<< <TC>( std::ostream&, const Polynomial<TC>& );

  friend Polynomial<TC> operator+<TC>( const Polynomial<TC>&,
                                       const Polynomial<TC>& );

  friend Polynomial<TC> operator-<TC>( const Polynomial<TC>&,
                                       const Polynomial<TC>& );

private:
  std::vector<TC> d_c;
public:
  inline Polynomial( int = 0 );
  template <typename TFwdIter>
  inline Polynomial( TFwdIter,
                     TFwdIter );
  inline Polynomial( const Polynomial<TC>&);
  inline Polynomial<TC>& operator=( const Polynomial<TC>& );
  inline TC& operator[]( const int );
  inline const TC& operator[]( const int ) const;
  inline Polynomial<TC>& operator+=( const TC );
  inline Polynomial<TC>& operator+=( const Polynomial<TC>& );
  inline Polynomial<TC>& operator-=( const TC );
  inline Polynomial<TC>& operator-=( const Polynomial<TC>& );
  inline Polynomial<TC>& operator*=( const TC );
  inline Polynomial<TC>& operator/=( const TC );
  inline TC operator()( const TC ) const; // evaluate
  
};

template <typename TIP>
// TIP is the inner product class
// collection of TIP::vector_type must be linearly independent
static void GramSchmidt( std::vector<typename TIP::vector_type>&, TIP& );



template <typename TC>
Polynomial<TC> operator+( const Polynomial<TC>& p,
                          const Polynomial<TC>& q )
{
  Polynomial<TC> result( p.d_c.size() > q.d_c.size() ? p : q );
  result += ( p.d_c.size() > q.d_c.size() ? q : p );
  return( result );
}

template <typename TC>
Polynomial<TC> operator-( const Polynomial<TC>& p,
                          const Polynomial<TC>& q )
{
  Polynomial<TC> result( p );
  result -= q;
  return( result );
}

template <typename TC, typename TFwdIter>
PolyInnerProduct<TC, TFwdIter>::PolyInnerProduct( const PolyInnerProduct<TC, TFwdIter>& proto )
  : d_begin( proto.d_begin ), d_end( proto.d_end )
{}

template<typename TC, typename TFwdIter>
PolyInnerProduct<TC, TFwdIter>::PolyInnerProduct( TFwdIter first,
                                                  TFwdIter last)
  : d_begin( first ), d_end( last )
{}

template <typename TC, typename TFwdIter>
TC PolyInnerProduct<TC, TFwdIter>::operator()( const Polynomial<TC>& p,
                                               const Polynomial<TC>& q )
{
  TC val = 0;
  for ( TFwdIter iter = d_begin; iter != d_end; ++iter )
    {
      val += p( *iter )*q( *iter );
    }
  return( val );
}

template <typename TC, typename TFwdIter>
TC PolyInnerProduct<TC, TFwdIter>::operator()( const Polynomial<TC>& p,
                                               TFwdIter f )
{
  TC val = 0;
  TFwdIter iterf( f );
  for ( TFwdIter iter = d_begin; iter != d_end; ++iter, ++iterf )
    {
      val += p( *iter )*( *iterf );
    }
  return( val );
}

template <typename TC>
Polynomial<TC>::Polynomial( int degree ) : d_c( degree + 1, 0 )
{}

template <typename TC> template <typename TFwdIter>
Polynomial<TC>::Polynomial( TFwdIter first,
                            TFwdIter last )
  : d_c( first, last )
{}

template <typename TC>
Polynomial<TC>::Polynomial( const Polynomial<TC>& proto ) : d_c( proto.d_c )
{}

template <typename TC>
Polynomial<TC>& 
Polynomial<TC>::operator=( const Polynomial<TC>& other )
{
  d_c = other.d_c;
  return( *this );
}

template <typename TC>
TC& Polynomial<TC>::operator[]( const int term )
{
  return( d_c[term] );
}

template <typename TC>
const TC& Polynomial<TC>::operator[]( const int term ) const
{
  return( d_c[term] );
}

template <typename TC>
Polynomial<TC>& Polynomial<TC>::operator+=( const TC c0 )
{
  d_c[0] += c0;
  return( *this );
}

template <typename TC>
Polynomial<TC>& 
Polynomial<TC>::operator+=( const Polynomial<TC>& other )
{
  if ( other.d_c.size() > d_c.size() )
    {
      d_c.resize( other.d_c.size(), 0 );
    }

  typename std::vector<TC>::iterator target = d_c.begin();
  for ( typename std::vector<TC>::const_iterator src = other.d_c.begin();
        src != other.d_c.end();
        ++src,++target )
    {
      (*target) += *src;
    }
  

  return( *this );
}

template <typename TC>
Polynomial<TC>& Polynomial<TC>::operator-=( const TC c0 )
{
  d_c[0] -= c0;
  return( *this );
}

template <typename TC>
Polynomial<TC>& 
Polynomial<TC>::operator-=( const Polynomial<TC>& other )
{
  if ( other.d_c.size() > d_c.size() )
    {
      d_c.resize( other.d_c.size(), 0 );
    }

  typename std::vector<TC>::iterator target = d_c.begin();
  for ( typename std::vector<TC>::const_iterator src = other.d_c.begin();
        src != other.d_c.end();
        ++src,++target )
    {
      (*target) -= *src;
    }

  return( *this );
}

template <typename TC>
Polynomial<TC>& Polynomial<TC>::operator*=( const TC c0 )
{
  for ( typename std::vector<TC>::iterator iter = d_c.begin();
        iter != d_c.end();
        ++iter )
    {
      (*iter) *= c0;
    }
  return( *this );
}

template <typename TC>
Polynomial<TC>& Polynomial<TC>::operator/=( const TC c0 )
{
  for ( typename std::vector<TC>::iterator iter = d_c.begin();
        iter != d_c.end();
        ++iter )
    {
      (*iter) /= c0 ;
    }
  return( *this );
}

template <typename TC>
TC Polynomial<TC>::operator()( const TC x ) const
{
  TC val = *d_c.rbegin();
  for ( typename std::vector<TC>::const_reverse_iterator rIter = d_c.rbegin() + 1;
        rIter != d_c.rend();
        ++rIter )
    {
      val = val*x + *rIter;
    }
  return( val );
}


template <typename TC>
std::ostream& operator<<( std::ostream& os, const Polynomial<TC>& p )
{
  int terms = 0;
  for ( int i = 0; i < p.d_c.size(); ++i )
    {
      if ( p.d_c[i] == 0 )
        {
          continue;
        }
      if ( terms != 0 )
        {
          os << " + ";
        }
      os << p.d_c[i] << "x^" << i;
      ++terms;
    }
  if ( terms == 0 )
    {
      os << "0";
    }
  return( os );
}

template <typename TIP>
void GramSchmidt( std::vector<typename TIP::vector_type>& basis, TIP& dot_prod )
{
  for ( typename std::vector<typename TIP::vector_type>::iterator base = basis.begin();
        base != basis.end();
        ++base )
    {
      if ( base != basis.begin() )
        {
          typename TIP::vector_type accum;
          for ( typename std::vector<typename TIP::vector_type>::iterator ortho = basis.begin();
                ortho != base;
                ++ortho )
            {
              typename TIP::vector_type proj( *ortho );
              typename TIP::return_type tmp = dot_prod( *ortho, *base );
              proj *= tmp;
              accum += proj;
            }
          *base -= accum;
        }
      // normalize
      typename TIP::return_type invNorm = 1.0/std::sqrt( dot_prod( *base, *base ) );
      *base *= invNorm;
    }
}

#endif
