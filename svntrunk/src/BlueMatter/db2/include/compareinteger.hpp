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
 // ***********************************************************************
// File: compareinteger.hpp
// Author: RSG
// Date: May 7, 2002
// Namespace: db2
// Class: CompareInteger
// Description: Class implementing the methods required by SequenceIndex
// ***********************************************************************

#ifndef INCLUDE_DB2_COMPAREINTEGER_HPP
#define INCLUDE_DB2_COMPAREINTEGER_HPP

#include <vector>

namespace db2
{
  class CompareInteger
  {
  private:
    const std::vector<int>& d_array;
  public:
    CompareInteger(const std::vector<int>&);
    inline bool operator()(int, int) const;
    inline int key(int) const;
  };

  bool CompareInteger::operator()(int i1, int i2) const
  {
    return(d_array[i1] < d_array[i2]);
  }

  int CompareInteger::key(int index) const
  {
    return(d_array[index]);
  }
}
#endif
