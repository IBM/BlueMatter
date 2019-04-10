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
// File: testuniform.cpp
// Author: RSG
// Date: February 8, 2004
// Description: Test driver for Uniform class
// Command line arguments: number of tries, starting seed
// *********************************************************************

#include <BlueMatter/uniform.hpp>

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <vector>
#include <algorithm>

int main(int argc, char** argv)
{
  int step = 100;
  int nTries = 100000;
  int seed = 458912;
  switch(argc)
    {
    case 4:
      step = std::atoi(argv[3]);
      step = step > 0 ? step : -step;
    case 3:
      seed = std::atoi(argv[2]);
    case 2:
      nTries = std::atoi(argv[1]);
      nTries = nTries > 0 ? nTries : -nTries;
      break;
    default:
      std::cerr << argv[0] << " nTries <seed = " << seed << "> <step = "
    << step << ">" << std::endl;
      std::exit(-1);
    }
  seed = seed > 0 ? seed : -seed;

  Uniform gen(seed);

  std::vector<float> data(nTries);
  for (std::vector<float>::iterator iter = data.begin();
       iter != data.end();
       ++iter)
    {
      *iter = gen();
    }
  std::sort(data.begin(), data.end());

  std::cout << "# column 1: domain (x) value" << std::endl;
  std::cout << "# column 2: fraction of counts between zero and value" << std::endl;
  std::cout << "# column 3: estimated uncertainty in fraction of counts between zero and value" << std::endl;
  float xTries = nTries;
  for (int i = step - 1; i < nTries; i += step)
    {
      float value = data[i];

      int count = i + 1;

      std::cout << value << " "
    << count/xTries << " "
    << sqrt((float)count)/xTries << std::endl;
    }
  return(0);
}
