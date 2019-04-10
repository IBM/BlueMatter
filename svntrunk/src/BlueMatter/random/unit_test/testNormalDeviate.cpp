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
// File: testNormalDeviate.cpp
// Author: RSG
// Date: January 7, 2008
// Description: Unit test driver for NormalDeviate class.  Generates
//              histogram and distribution function for output.
//              
// Command line args:  nReps max (abs. value) nBins
// ********************************************************************

#include "BlueMatter/NormalDeviate.hpp"
#include "BlueMatter/MRG32k3aEngine.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <cmath>

int main(int argc, char** argv)
{
  int nReps = 10000;
  double yMax = 10;
  int nBins = 100;

  switch( argc )
    {
    case 4:
      nBins = std::atoi( argv[3] );
    case 3:
      yMax = std::atof( argv[2] );
    case 2:
      nReps = std::atoi( argv[1] );
      break;
    default:
      std::cerr << argv[0] << " nReps yMax (abs. value) nBins" << std::endl;
      std::exit(-1);
    }

  std::cout << "# nReps = " << nReps << " "
            << "yMax = " << yMax << " "
            << "nBins = " << nBins << std::endl;

  double binWidth = yMax/nBins;
  std::vector<double> histogram( 2*nBins + 2 );

  std::cout << "DEBUG: histogram.size() = " << histogram.size() << std::endl;

  for ( std::vector<double>::iterator iter = histogram.begin();
        iter != histogram.end();
        ++iter )
    {
      *iter = 0;
    }


  rng::NormalDeviate<MRG32k3aEngine> nd;
  nd.initialize();
  for ( int i = 0; i < nReps; ++i )
    {
      double y = nd();
      //      std::cout << "DEBUG: y = " << y << std::endl;
      if ( y < -yMax )
        {
          ++histogram[0];
          continue;
        }
      if ( y >= yMax )
        {
          ++histogram[histogram.size() - 1];
          continue;
        }
      int iBin = (y + yMax)/binWidth + 1;
      //      std::cout << "DEBUG: iBin = " << iBin << std::endl;
      ++histogram[iBin];
    }

  std::cout << "# Column 1: Bin center\n"
            << "# Column 2: Fraction of counts in bin\n"
            << "# Column 3: Bin edge (used for cumulant)\n"
            << "# Column 4: Cumulant (counts in bins below current bin)\n"
            << "# Column 5: sqrt(nAccum)/nReps\n"
            << "# Column 6: Complement of Cumulant\n"
            << "# Column 7: sqrt(nReps - nAccum)/nReps\n"
            << "# Column 8: Erf(x)\n"
            << "# Column 9: Erfc(x)"
            << std::endl;
  double invSqrt2 = 1.0/sqrt(2.0);
  int nAccum = 0;
  int i = 0;
  for ( std::vector<double>::const_iterator iter = histogram.begin();
        iter != histogram.end();
        ++iter)
    {
      double xBin = (i - 1.5)*binWidth - yMax;
      double cBin = (i - 1.0)*binWidth - yMax;
      double xFrac = static_cast<double>( *iter )/static_cast<double>( nReps );
      double xCum = static_cast<double>(nAccum)/static_cast<double>( nReps );
      double xCompCum = 
        static_cast<double>(nReps - nAccum)/static_cast<double>( nReps );
      nAccum += *iter;
      ++i;
      std::cout << std::setw( 12 ) << xBin << " "
                << std::setw( 12 ) << xFrac << " "
                << std::setw( 12 ) << cBin << " "
                << std::setw( 12 ) << xCum << " "
                << std::setw( 12 ) << sqrt( static_cast<double>( nAccum ) )/static_cast<double>( nReps ) << " "
                << std::setw( 12 ) << xCompCum << " "
                << std::setw( 12 ) << sqrt( static_cast<double>( nReps - nAccum ) )/static_cast<double>( nReps ) << " "
                << std::setw( 12 ) << (erf(invSqrt2*cBin) + 1)*0.5 << " "
                << std::setw( 12 ) << erfc(invSqrt2*cBin)*0.5
                << std::endl;
    }
  return( 0 );
}
