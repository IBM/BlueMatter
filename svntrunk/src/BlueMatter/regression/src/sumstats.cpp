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
 // *******************************************************************
// File: sumstats.cpp
// Author: RSG
// Date: October 28, 2006
// Description: Program to read in double precision data sorted by
//              mesh coordinate and sum up each subset in all possible
//              permutations. The distribution of summations is then
//              accumulated for histogramming. This routine takes
//              input from stdin and sends output to stdout
// *******************************************************************

#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <BlueMatter/summation.hpp>

int main(int argc, char** argv)
{
  char inBuffer[128];
  int ix;
  int ixOld = -1;
  int iy;
  int iyOld = -1;
  int iz;
  int izOld = -1;
  std::vector<fp::t_float> summand;
  summand.reserve(128);
  summand.clear();

  std::vector<fp::t_float> dist;
  dist.reserve(16*summand.size()); // don't know how to estimate this
  dist.clear();

  union ll2double
  {
    int64_t d_llvalue;
    uint64_t d_ullvalue;
    uint32_t d_uintvalue[2];
    double d_dvalue;
  };

  while(std::cin)
    {
      std::cin >> ix >> iy >> iz >> inBuffer;
      if (!std::cin)
	{
	  break;
	}
      // the string in inBuffer is assumed to be of the form:
      // :XD:3F130F47F7097BFE
      // that is ":XD:" followed by a 16 digit hex number which
      // actually represents a double (fp::t_float)
      if ((ix != ixOld) || (iy != iyOld) || (iz != izOld))
	{
	  if (!summand.empty()) // compute sums of all permutations
				// and store diffs from avg in dist
				// vector
	    {
	      fp::SummationStats<std::vector<fp::t_float>::iterator>::distribution(summand.begin(), summand.end(), dist);
	    }
	  summand.clear();
	  ixOld = ix;
	  iyOld = iy;
	  izOld = iz;
	}
      ll2double conv;
      conv.d_ullvalue = strtoull(inBuffer + 4, NULL, 16);
      summand.push_back(conv.d_dvalue);
    }
  std::sort(dist.begin(), dist.end());
  // now need to create and output actual distribution function
  fp::t_float current = std::numeric_limits<fp::t_float>::min();
  int totalCount = 0;
  int dCount = 0;
  std::cout << "# Column 1: Number of diffs equal to value in column 5\n"
	    << "# Column 2: Total number of diffs <= value in column 5\n"
	    << "# Column 3: Fraction of diffs <= value in column 5\n"
	    << "# Column 4: Fraction of diffs > value in column 5\n"
	    << "# Column 5: Diff value in terms of diff between normalized binary fractions\n"
	    << std::endl;
  // Should I be expressing the diff in terms of
  // numeric_limits<double>::epsilon()?

  for (std::vector<fp::t_float>::iterator iter = dist.begin();
       iter != dist.end();
       ++iter)
    {
      if (*iter == current)
	{
	  ++totalCount;
	  ++dCount;
	  continue;
	}
      if (dCount > 0)
	{
	  std::cout << std::setw(10) << dCount << " "
		    << std::setw(10) << totalCount << " "
		    << std::setprecision(6) 
		    << std::setw(15)
		    << static_cast<double>(totalCount)/dist.size()
		    << " "
		    << std::setw(15) 
		    << static_cast<double>(dist.size()-totalCount)/dist.size()
		    << " "
		    << std::scientific << std::setprecision(17)
		    << std::setw(22) << current
		    << std::endl;
	}
      ++totalCount;
      dCount = 1;
      current = *iter;
    }
  return(0);
}
