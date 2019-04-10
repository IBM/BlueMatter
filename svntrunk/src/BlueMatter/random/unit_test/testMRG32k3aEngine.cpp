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
 // ******************************************************************************
// File: testMRG32k3aEngine
// Author: RSG
// Date: January 5, 2008
// Description: Simple test driver for MRG32k3aEngine class to allow
//              comparison with data from reference implementation of
//              the RNG by P. L'Ecuyer.
// Command line arguments: nReps (number of repetitions)
//
// ******************************************************************************

#include "BlueMatter/MRG32k3aEngine.hpp"


#include <iostream>
#include <iomanip>
#include <cstdlib>

void usage(const char* exe)
{
  std::cerr << exe << " <nReps = 100>" << std::endl;
}

int main(int argc, char** argv)
{
  int nReps = 100;

  switch ( argc )
    {
    case 1:
      break;
    case 2:
      if ( *argv[1] == '?' )
        {
          usage(argv[0]);
          std::exit(-1);
        }
      nReps = std::atoi( argv[1] );
      break;
    default:
      usage(argv[0]);
      std::exit(-1);
    }

  MRG32k3aEngine rng;

  MRG32k3aEngine::state tmp;
  tmp.setState( 0 );
  rng.initFromState( tmp );

  for (int i = 0; i < nReps; ++i)
    {
      std::cout << std::setprecision(20) << rng.uniform() << std::endl;
    }
  return( 0 );
}
