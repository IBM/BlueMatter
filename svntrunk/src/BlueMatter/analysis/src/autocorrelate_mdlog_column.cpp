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
// File: autocorrelate_mdlog_column.cpp
// Author: RSG
// Date: January 29, 2008
// Description: Reads specified column (zero-based) from MDLogger
// output file and computes the autocorrelation function of that time
// series up to some specified lag. Input data is read from stdin.
//
// Command line arguments:
// maxLag (bound on maximum lag to be computed)
// <inputFileLength = 10**8> (estimate of number of data point in
//                            input)
// **************************************************************

#include <BlueMatter/MDLogUtils.hpp>
#include <BlueMatter/autocorrelation.hpp>
#include <pk/fxlogger.hpp>

#include <iterator>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <iomanip>

int main( int argc, char** argv )
{
  long inputSize = 1e8;
  int maxLag = 1000;
  int col = 0;
  int nThreads = 16;
  switch( argc )
    {
    case 5:
      inputSize = std::atol( argv[4] );
    case 4:
      nThreads = std::atoi( argv[3] );
    case 3:
      maxLag = std::atoi( argv[2] );
    case 2:
      col = std::atoi( argv[1] );
      break;
    default:
      std::cerr << argv[0] << " columnNo(zero-based) <maxLag=1000> <nThreads=16> <inputFileLength = 10**8>"
                << std::endl;
      std::exit( -1 );
    }

  std::vector<AutoCorrelation::TDouble> x;
  x.reserve( inputSize );
  std::vector<long> ts;
  ts.reserve( inputSize );
  x.clear();
  std::vector<std::string> header;
  std::vector<std::string> column;
  long rowCount = 0;
  long oldTimeStep = -1;
  long oldIncrement = 0;
  std::istream_iterator<mdlu::Row> endOfStream;
  for( std::istream_iterator<mdlu::Row> iter( std::cin );
       iter != endOfStream;
       ++iter )
    {
      ++rowCount;
      if ( iter->isHeader() )
        {
          iter->split( header );
          column.reserve( header.size() );
          continue;
        }
      long newIncrement = iter->ts() - oldTimeStep;
      if ( oldIncrement != newIncrement )
        {
          std::cerr << "Mismatch in time-step increments at row: "
                    << rowCount << " Old increment: " << oldIncrement
                    << " New increment: " << newIncrement
                    << " Old time-step: " << oldTimeStep
                    << " New time-step: " << iter->ts()
                    << std::endl;
        }
      oldIncrement = newIncrement;
      oldTimeStep = iter->ts();
      iter->split( column );
      char* endPtr = NULL;
      double value = std::strtod( column[col].c_str(), &endPtr );
      if ( endPtr == column[col].c_str() )
        {
          std::cerr << "Error converting " << column[col]
                    << " to double at row " << rowCount
                    << " of input" << std::endl;
          return( -1 );
        }
      x.push_back( value );
      ts.push_back( iter->ts() );
    }  // finished reading in column from file
  AutoCorrelation ac;
  ac.init( maxLag );
  AutoCorrelation::ReturnData ret = ac.compute( x.begin(), x.end(), nThreads );
  std::cout << "# Estimating autocorrelation for column " << col << " labeled "
            << header[col] << std::endl;
  std::cout << "# Input data contained " << rowCount << " rows" << std::endl;
  std::cout << "# Mean of input data = " << ret.d_mean << std::endl;
  std::cout << "# Variance of input data = " << ret.d_variance << std::endl;
  std::cout << "# Column 1: Lag (assuming unit lag between input data rows)" << std::endl;
  std::cout << "# Column 2: Average value of lag products over input dataset (autocorrelation)" << std::endl;
  for ( int lag = 0; lag < ret.d_maxLag; ++lag )
    {
      std::cout << std::setw( 14 ) << lag << " "
                << std::setw( 15 ) << std::scientific 
                << ret.d_autocorrelation[lag] << std::endl;
    }
  return( 0 );
}
