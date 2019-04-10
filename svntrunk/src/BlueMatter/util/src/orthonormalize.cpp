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
 // **************************************************************
// File: orthonormalize.cpp
// Author: RSG
// Date: March 8, 2009
// Description: Test program for polynomial.hpp that starts with a
//              specified domain and maximum degree of polynomial in
//              basis and provides an orthonormal basis set.
// **************************************************************

#include <BlueMatter/polynomial.hpp>

#include <iostream>
#include <cstdlib>
#include <vector>


int main( int argc, char** argv )
{
  int low = 0;
  int high = 128;
  int degree = 4;

  switch( argc )
    {
    case 4:
      degree = std::atoi( argv[3] );
      high = std::atoi( argv[2] );
      low = std::atoi( argv[1] );
      break;
    default:
      std::cerr << argv[0] << " low high degree" << std::endl;
      std::exit( -1 );
    }
  std::vector<double> domain;
  domain.reserve( high - low );
  for ( int i = low; i  < high; ++i )
    {
      domain.push_back( static_cast<double>( i ) );
    }

  std::vector<Polynomial<double> > basis( degree );
  for ( int i = 0; i < degree; ++i )
    {
      Polynomial<double> p( i );
      p[i] = 1;
      basis[i] = p;
      std::cout << basis[i] << std::endl;
    }

  PolyInnerProduct<double, std::vector<double>::iterator> 
    dotProd( domain.begin(), domain.end() );
  

  GramSchmidt( basis, dotProd );

  for ( int i = 0; i < degree; ++i )
    {
      std::cout << basis[i] << std::endl;
    }
  for ( int i = 0; i < degree; ++i )
    {
      for ( int j = 0; j <= i; ++j )
        {
          std::cout << "< " << basis[i] << " | "
                    << basis[j] << " > = "
                    << dotProd( basis[i], basis[j] )
                    << std::endl;
        }
    }
}
