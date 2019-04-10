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
 // ************************************************************************
// File: MDLogUtils.hpp
// Author: RSG
// Date: January 21, 2008
// Description: Utilities to manipulate the output of MDLogger.
// ************************************************************************

#ifndef INCLUDE_MDLOGUTILS_HPP
#define INCLUDE_MDLOGUTILS_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

namespace mdlu
{



  // encapsulates data and operations associated with a row output by
  // MDLogger
  class Row
  {
  private:
    enum { IS_HEADER = -1 };
    // data
    std::string d_row;
    long d_ts;
  public:
    inline Row();
    inline Row( const Row& );
    inline Row& operator=( const Row& );
    inline bool operator<( const Row& ) const;
    inline bool operator>( const Row& ) const;
    inline bool operator==( const Row& ) const;
    inline bool isHeader() const;
    inline long ts() const;
    void split( std::vector<std::string>&, const std::string& = " \t" ) const;
    inline std::istream& input( std::istream& );
    inline std::ostream& output( std::ostream& ) const;
  };

  inline std::istream& operator>>( std::istream& is, mdlu::Row& );
  inline std::ostream& operator<<( std::ostream& os, const mdlu::Row& );

    Row::Row() : d_ts( -2 )
  {}

  Row::Row( const Row& proto ) 
    : d_row( proto.d_row ),
      d_ts( proto.d_ts )
  {}

  Row& Row::operator=( const Row& other )
  {
    if ( &other == this )
      {
        return( *this );
      }
    d_row = other.d_row;
    d_ts = other.d_ts;
    return( *this );
  }

  bool Row::operator>( const Row& other ) const
  {
    return( d_ts > other.d_ts );
  }

  bool Row::operator<( const Row& other ) const
  {
    return( d_ts < other.d_ts );
  }

  bool Row::operator==( const Row& other ) const
  {
    return( d_ts == other.d_ts );
  }

  bool Row::isHeader() const
  {
    return( d_ts == IS_HEADER );
  }

  long Row::ts() const
  {
    return( d_ts );
  }

  std::istream& Row::input( std::istream& is )
  {
    getline( is, d_row );
    char* endStr = NULL;
    d_ts = std::strtol( d_row.c_str(), &endStr, 10 );
    if ( endStr == d_row.c_str() )
      {
        d_ts = IS_HEADER;
      }
    return( is );
  }

  std::ostream& Row::output( std::ostream& os ) const
  {
    os << d_row;
    return( os );
  }

  std::istream& operator>>( std::istream& is, mdlu::Row& r )
  {
    return( r.input( is ) );
  }
  
  std::ostream& operator<<( std::ostream& os, const mdlu::Row& r )
  {
    return( r.output( os ) );
  }

}


#endif
