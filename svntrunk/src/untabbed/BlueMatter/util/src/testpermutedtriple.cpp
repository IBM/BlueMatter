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
 // ****************************************************************
// File: testpermutedtriple.cpp
// Author: RSG
// Date: July 11, 2003
// Description: test for permutation template function
// ****************************************************************


#include <BlueMatter/permutedtriple.hpp>

#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char** argv)
{
  if (argc != 4)
    {
      std::cerr << argv[0] << " first second third" << std::endl;
      std::exit(-1);
    }
  std::string s1(argv[1]);
  std::string s2(argv[2]);
  std::string s3(argv[3]);

  Triple<std::string> triple(s1,s2,s3);

  std::cout << "First: " << s1
      << " Second: " << s2
      << " Third: " << s3 << std::endl;

  std::cout << "Permutation<1,2,3>: "
      << PermutedTriple<Triple<std::string>, Permutation<1,2,3> >::first(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<1,2,3> >::second(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<1,2,3> >::third(triple)
      << std::endl;

  std::cout << "Permutation<2,1,3>: "
      << PermutedTriple<Triple<std::string>, Permutation<2,1,3> >::first(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<2,1,3> >::second(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<2,1,3> >::third(triple)
      << std::endl;

  std::cout << "Permutation<3,1,2>: "
      << PermutedTriple<Triple<std::string>, Permutation<3,1,2> >::first(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<3,1,2> >::second(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<3,1,2> >::third(triple)
      << std::endl;

  std::cout << "Permutation<1,3,2>.: "
      << PermutedTriple<Triple<std::string>, Permutation<1,3,2> >::first(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<1,3,2> >::second(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<1,3,2> >::third(triple)
      << std::endl;

  std::cout << "Permutation<3,2,1>: "
      << PermutedTriple<Triple<std::string>, Permutation<3,2,1> >::first(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<3,2,1> >::second(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<3,2,1> >::third(triple)
      << std::endl;

  std::cout << "Permutation<2,3,1>: "
      << PermutedTriple<Triple<std::string>, Permutation<2,3,1> >::first(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<2,3,1> >::second(triple) << " "
      << PermutedTriple<Triple<std::string>, Permutation<2,3,1> >::third(triple)
      << std::endl;

}
