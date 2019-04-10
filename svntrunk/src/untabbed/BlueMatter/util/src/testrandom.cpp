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
// File: testrandom.cpp
// Author: RSG
// Date: February 8, 2004
// Description: Test driver for RandomNumberGenerator class
// Command line arguments: number of tries, starting seed
// *********************************************************************

#include <BlueMatter/RandomNumberGenerator.hpp>

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
  seed = seed > 0 ? -seed : seed;

  RandomNumberState state;
  state.mSeed = seed;

  RandomNumberGenerator gen;
  gen.InitState(state);

  std::vector<double> data(nTries);
  for (std::vector<double>::iterator iter = data.begin();
       iter != data.end();
       ++iter)
    {
      *iter = gen.GasDev();
    }
  std::sort(data.begin(), data.end());

  std::cout << "# column 1: domain (x) value" << std::endl;
  std::cout << "# column 2: fraction of counts between value and zero" << std::endl;
  std::cout << "# column 3: estimated uncertainty in fraction of counts between value and zero" << std::endl;
  std::cout << "# column 4: fraction of counts with values s.t. abs(val2) > abs(val)" << std::endl;
  std::cout << "# column 5: estimated uncertainty in fraction of counts with values s.t. abs(val2) > abs(val)" << std::endl;
  std::cout << "# column 6: erf(abs(value))" << std::endl;
  std::cout << "# column 7: erfc(abs(value))" << std::endl;
  std::vector<double>::iterator zeroIter =
    std::lower_bound(data.begin(), data.end(), 0.0);
  int zeroIndex = zeroIter - data.begin();
  int count = 0;
  float xTries = nTries;
  float invRoot2 = 1.0/sqrt(2.0);
  for (int i = step - 1; i < nTries; i += step)
    {
      float value = data[i];
      float absValue = value >= 0.0 ? value : -value;
      int count1 = i < zeroIndex ? zeroIndex - i + 1: i + 1 - zeroIndex;
      int count2 = i < zeroIndex ? i + 1 : nTries - i + 1;
      std::cout << value << " "
    << count1/xTries << " "
    << sqrt((float)count1)/xTries << " "
    << count2/xTries << " "
    << sqrt((float)count2)/xTries << " "
    << 0.5*erf(absValue*invRoot2) << " "
    << 0.5*erfc(absValue*invRoot2) << std::endl;
    }
  return(0);
}
