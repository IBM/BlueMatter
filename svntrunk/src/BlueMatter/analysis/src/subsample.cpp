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
// File: subsample.cpp
// Author: RSG
// Date: January 29, 2008
// Description: Subsamples specified column(s) from MDLogger file
//              and outputs it to stdout
//
// Command line arguments:
// subsample factor n (1 out of n will be output)
// <inputFileLength = 10**8> (estimate of number of data point in
//                            input)
// <column1> <column2> ... <columnN>
// if no column numbers (column 0 with the time-stamps is always
// output) are provided, the entire row is output.
// **************************************************************

#include <BlueMatter/MDLogUtils.hpp>

#include <cstdlib>
#include <iostream>
#include <istream>
#include <iterator>
#include <iomanip>
#include <vector>
#include <string>

int main( int argc, char** argv )
{
  int columnCount = argc - 2;
  std::vector<int> columnList;
  int subsampleFactor = 1e4;
  
  switch( argc )
    {
    case 1:
      std::cerr << argv[0] << " subsampleFactor <column1> <column2> .... <columnN>" << std::endl;
      std::exit( -1 );
    default:
    case 2:
      subsampleFactor = std::atoi( argv[1] );
      break;
    }

  for ( int i = 3; i < argc+1; ++i )
    {
      columnList.push_back( std::atoi( argv[i] ) );
    }
  
  std::vector<std::string> header;
  std::vector<std::string> column;
  std::istream_iterator<mdlu::Row> endOfStream;
  long rowCount = 0;

  for( std::istream_iterator<mdlu::Row> iter( std::cin );
       iter != endOfStream;
       ++iter, ++rowCount )
    {
      if ( iter->isHeader() )
        {
          iter->split( header );
          std::cout << "# Sampling with subsampling factor: " 
                    << subsampleFactor << "\n";
          std::cout << "# Column 1: Time Step\n";
          int colNo = 2;
          if (columnCount < 1 )
            {
              for ( int i = 1; i < header.size(); ++i )
                {
                  columnList.push_back( i );
                }
            }
          for ( std::vector<int>::const_iterator iter = columnList.begin();
                iter != columnList.end();
                ++iter, ++colNo )
            {
              std::cout << "# Column " << colNo << ": " 
                        << header[*iter] << "\n";
            }
          std::cout.flush();
          column.reserve( header.size() );
          continue;
        }
      if ( rowCount % subsampleFactor == 0 )
        {
          std::cout << std::setw( 12 ) << iter->ts() << " ";
          iter->split( column );
          for ( std::vector<int>::const_iterator iter = columnList.begin();
                iter != columnList.end();
                ++iter )
            {
              std::cout << column[*iter] << " ";
            }
          std::cout << std::endl;
        }
    }

  return( 0 );
}
