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
 // *********************************************************************
// File: MDLogFile.cpp
// Author: RSG
// Date: March 9, 2008
// Description: Utility to do polynomial fit to a selected column in
//              MDLogger output files.
// *********************************************************************

#include <BlueMatter/MDLogUtils.hpp>
#include <BlueMatter/polynomial.hpp>

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>


int main( int argc, char** argv )
{
  int rowCount = 1e8;
  int maxDegree = 2;
  int fitCol = 1;
  switch( argc )
    {
    case 4:
      rowCount = std::atoi( argv[3] );
    case 3:
      maxDegree = std::atoi( argv[2] );
    case 2:
      fitCol = std::atoi( argv[1] );
      break;
    default:
      std::cerr << argv[0] << " fitCol <maxDegree=2> <rowCount=1e8>" << std::endl;
      std::exit( -1 );
    }
  
  std::istream_iterator<mdlu::Row> iter( std::cin );
  std::vector<std::string> header;
  std::vector<std::string> col;
  std::vector<double> x;
  x.reserve( rowCount );
  std::vector<double> y;
  y.reserve( rowCount );
  char* endPtr;
  while( iter != std::istream_iterator<mdlu::Row>() )
    {
      if ( iter->isHeader() )
        {
          iter->split( header );
          col.reserve( header.size() );
          continue;
        }
      x.push_back( static_cast<double>( iter->ts() ) );
      iter->split( col );
      double val = std::strtod( col[fitCol].c_str(), &endPtr );
      if ( col[fitCol].c_str() == endPtr )
        {
          std::cerr << "Error converting column to double: "
                    << col[fitCol] << std::endl;
          continue;
        }
      y.push_back( val );
    }

  PolyInnerProduct<double, std::vector<double>::iterator> 
    dotProd( x.begin(), x.end() );


  std::vector<Polynomial<double> > basis( maxDegree );
  for ( int i = 0; i < maxDegree; ++i )
    {
      Polynomial<double> p( i );
      p[i] = 1;
      basis[i] = p;
    }

  GramSchmidt( basis, dotProd );

  std::cout << "Orthonormal Basis for Fit:" << std::endl;
  for ( int i = 0; i < maxDegree; ++i )
    {
      std::cout << basis[i] << std::endl;
    }

  std::vector<double> fit( maxDegree );
  Polynomial<double> fpoly;
  for ( int i = 0; i < maxDegree; ++i )
    {
      fit[i] = dotProd( basis[i], y.begin() );
      std::cout << "< basis[" << i << "] | y > = " << fit[i] << std::endl;
      Polynomial<double> dpoly( basis[i] );
      dpoly *= fit[i];
      fpoly += dpoly;
      std::cout << "Fitted Polynomial[" << i << "] = " << fpoly << std::endl;
    }
  return( 0 );
}
