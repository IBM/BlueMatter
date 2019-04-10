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

#include <BlueMatter/db2/chemicalcomponentlist.hpp>
#include <BlueMatter/db2/atomtaglist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>

namespace db2
{
  AtomTagList* AtomTagList::s_instance = NULL;
  std::pair<int, std::string> AtomTagList::s_null(-1, "NULL");

  AtomTagList::AtomTagList()
  {}

  AtomTagList::~AtomTagList()
  {
    delete s_instance;
    s_instance = NULL;
  }

  AtomTagList* AtomTagList::instance()
  {
    if (s_instance == NULL)
      {
  s_instance = new AtomTagList;
  assert(s_instance != NULL);
  s_instance->init();
      }
    return(s_instance);
  }

  void AtomTagList::init()
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    stmt.prepare("select component_name, atom_tag from chem.atom_tag_table");
    char componentName[64];
    char tagName[64];
    stmt.bindCol(1, (SQLCHAR*)&componentName[0], 64);
    stmt.bindCol(2, (SQLCHAR*)&tagName[0], 64);
    stmt.execute();
    int index = 0;
    while(stmt.fetch())
      {
  std::pair<int, std::string>
    tag(ChemicalComponentList::instance()->indexOfComponent(componentName),
        tagName);
  d_tagFromIndex.push_back(tag);
  d_indexOfTag[tag] = index;
  ++index;
      }
  }

  int AtomTagList::indexOfTag(const std::pair<int, std::string>& atomTag) const
  {
    std::map<std::pair<int, std::string>, int>::const_iterator iter =
      d_indexOfTag.find(atomTag);
    if (iter != d_indexOfTag.end())
      {
  return((*iter).second);
      }
    return(-1);
  }

  const std::pair<int, std::string>& AtomTagList::tagFromIndex(int index) const
  {
    if ((index >=0) && (index < d_tagFromIndex.size()))
      {
  return(d_tagFromIndex[index]);
      }
    return(null());
  }

  int AtomTagList::main(int argc, char** argv)
  {
    if (argc < 2)
      {
  std::cerr << argv[0] << " dbName" << std::endl;
  return(-1);
      }
    const char* dbName = argv[1];
    DBName::instance(dbName);
    AtomTagList* atList = AtomTagList::instance();
    std::cout << "Atom Tag List" << std::endl;
    int idx = 0;
    while(atList->tagFromIndex(idx) != null())
      {
  std::cout
    << idx
    << " "
    << atList->tagFromIndex(idx).first
    << " "
    << atList->tagFromIndex(idx).second
    << " "
    << ChemicalComponentList::instance()->componentFromIndex(atList->tagFromIndex(idx).first)
    << " "
    <<atList->indexOfTag(atList->tagFromIndex(idx))
    << std:: endl;
  ++idx;
      }
    return(0);
  }
}
