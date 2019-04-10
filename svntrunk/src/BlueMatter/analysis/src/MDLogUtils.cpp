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
// File: MDLogUtils.cpp
// Author: RSG
// Date: January 21, 2008
// Description: Utilities to manipulate the output of MDLogger.
// ************************************************************************

#include <BlueMatter/MDLogUtils.hpp>

namespace mdlu
{
  // splits input string (first argument) into a vector of strings
  // (second argument) based on the delimiters defined by the third
  // argument (defaults to " \t"
  static void splitString( const std::string&,
                           std::vector<std::string>&,
                           const std::string& = " \t" );

  void Row::split( std::vector<std::string>& out,
                   const std::string& delimiters ) const
  {
    splitString( d_row, out, delimiters );
  }

  void splitString( const std::string& line,
                    std::vector<std::string>& out,
                    const std::string& delimiters )
  {
    out.clear();
    std::string::size_type begin = 0;
    std::string::size_type end = 0;

    while( ( begin != std::string::npos ) && ( begin < line.size() ) )
      {
        begin = line.find_first_not_of( delimiters, end );
        if ( begin == std::string::npos )
          {
            break;
          }
        end = line.find_first_of( delimiters, begin );
        if ( end == std::string::npos )
          {
            end = line.size();
          }
        out.push_back( line.substr( begin, end - begin ) );
        begin = end;
      }
  }
}
