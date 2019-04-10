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
// File: atomtaglist.hpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: AtomTagList
// Description: Singleton class encapsulating data and operations associated
//              with available atom tags.  Atom tags are uniquely
//              defined by the pair: chemical component, tag name.
//              When methods of this class return a tag, it has the
//              type std::pair<int, std::string> tag where tag.first
//              is an index into the ChemicalComponentList and
//              tag.second is the tag name.
// ***********************************************************************
#ifndef INCLUDE_DB2_ATOMTAGLIST_HPP
#define INCLUDE_DB2_ATOMTAGLIST_HPP

#include <utility>
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>

namespace db2
{

  class AtomTagList
  {
  private:
    // data
    static AtomTagList* s_instance;
    static std::pair<int, std::string> s_null;
    std::map<std::pair<int, std::string>, int> d_indexOfTag;
    std::vector<std::pair<int, std::string> > d_tagFromIndex;
    // methods
    AtomTagList();
    void init();
  public:
    ~AtomTagList();
    inline static const std::pair<int, std::string>& null();
    static AtomTagList* instance();
    int indexOfTag(const std::pair<int, std::string>&) const; // returns -1 if not found
    const std::pair<int, std::string>& tagFromIndex(int) const; // returns
    // AtomTagList::null() if not found
    // test driver
    static int main(int, char**);
  };

  const std::pair<int, std::string>& AtomTagList::null()
  {
    return(s_null);
  }
}

#endif
