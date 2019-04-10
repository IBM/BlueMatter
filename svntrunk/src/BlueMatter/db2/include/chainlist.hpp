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
// File: chainlist.hpp
// Author: RSG
// Date: May 5, 2002
// Namespace: db2
// Class: ChainList
// Description: Class encapsulating data and operations associated
//              with the "chains" (singly branched polymers) contained
//              within a molecular system.
// ***********************************************************************

#ifndef INCLUDE_DB2_CHAINLIST_HPP
#define INCLUDE_DB2_CHAINLIST_HPP

#include <vector>
#include <utility>
#include <string>

namespace db2
{
  class ChainList
  {
  private:
    // data
    // first element of pair is the chain_id, second is the chain description
    std::vector<std::pair<int, std::string> > d_chainList;
  public:
    ChainList();
    void init(int); // arg is sys_id
    inline const std::vector<std::pair<int, std::string> >& chain()
      const;
    static int main(int, char**); // test driver
  };

  const std::vector<std::pair<int, std::string> >&
  ChainList::chain() const
  {
    return(d_chainList);
  }

}
#endif
