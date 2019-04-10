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
 // timing test for RngMrg32k3a

#include <BlueMatter/RngMrg32k3a.hpp>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>


int main(int argc, char** argv)
{
  int nReps = 300000000;
  switch(argc)
    {
    case 2:
      nReps = std::atoi(argv[1]);
      break;
    default:
      break;
    }

  rng::RngMrg32k3a s1;
  s1.init();

  double val = 0.0;
  for ( int i = 0; i < 1000; ++i)
    {
      val = s1.unif01();
      std::cout << std::setprecision(12) << std::setw(20) << val << std::endl;
    }

  clock_t start = clock();
    
  for (int i = 0; i < nReps; ++i)
    {
      val = s1.unif01();
    }
  clock_t stop = clock();

  double tElapsed = (double)( stop - start ) / CLOCKS_PER_SEC; 
  std::cout << "Time elapsed = " << std::setprecision(3) << std::setw(10) << tElapsed
            << std::endl;
  std::cout << std::setprecision(3) << std::setw(12) << (double)nReps/tElapsed
            << " per second" << std::endl;


  return(0);
}
