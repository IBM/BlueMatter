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
// File: testbmtread.hpp
// Author: RSG
// Date: September 26, 2002
// Description: test driver for bmt::BmtRead class
// ********************************************************************

#include <BlueMatter/bmtread.hpp>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdio>

int main(int argc, char** argv)
{
  const char* bmtFile = NULL;
  bool verbose = false;
  int frame = -1;
  switch(argc)
    {
    case 3:
      frame = std::atoi(argv[2]);
      verbose = true;
    case 2:
      bmtFile = argv[1];
      break;
    default:
      std::cerr << argv[0] << " bmtFile" << std::endl;
      return(1);
    }
  bmt::BmtRead bmtReader(bmtFile, verbose);
  int iFrame = 0;
  for(bmt::BmtRead::const_iterator iter = bmtReader.begin();
      iter != bmtReader.end();
      ++iter)
    {
      ++iFrame;
      if (frame > 0 && iFrame > frame)
	{
	  break;
	}
      if (frame > 0 && iFrame != frame)
	{
	  continue;
	}
      for (std::vector<bmt::Position>::const_iterator posIter =
	     (*iter).begin();
	   posIter != (*iter).end();
	   ++posIter)
	{
#if 0
	  std::cout << std::setw(16) << std::setprecision(9) << (*posIter).d_x;
	  std::cout << std::setw(16) << std::setprecision(9) << (*posIter).d_y;
	  std::cout << std::setw(16) << std::setprecision(9) << (*posIter).d_z;
#endif
	  char buff[1024];
	  std::sprintf(buff, "%16.8f %16.8f %16.8f", (*posIter).d_x, (*posIter).d_y, (*posIter).d_z);
	  std::cout << buff << std::endl;
	  //	  std::cout << std::endl;
	}
      std::cout << std::endl;
    }
  return(0);
}
