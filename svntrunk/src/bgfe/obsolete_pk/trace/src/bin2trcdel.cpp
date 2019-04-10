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
 // ******************************************************************
// File: trcdel2bin.cpp
// Author: RSG
// Date: April 10, 2004
// Description: Executable to convert binary files (output of trcdel2bin)
//              back into del format
//
//
//                         output goes to stdout
// ******************************************************************

#include <pk/selecttrace.hpp>

#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
  int startSize = 130;
  if (argc > 1)
    {
      startSize = std::atoi(argv[1]);
    }
  SelectTrace trc;
  int rc = trc.initFromBin(std::cin);
  if (rc < 0)
    {
      std::cerr << "Error initializing from binary file" << std::endl;
      std::exit(-1);
    }
  rc = trc.putDel(std::cout);
  if (rc < 0)
    {
      std::cerr << "Error writing DEL format trace data file" << std::endl;
      std::exit(-1);
    }

  return(0);
}
