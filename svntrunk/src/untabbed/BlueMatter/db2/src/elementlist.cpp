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
// File: elementlist.cpp
// Author: RSG
// Date: April 29, 2002
// Namespace: db2
// Class: ElementList
// Description: Singleton class encapsulating data and operations associated
//              with available molecular types
// ***********************************************************************

#include <BlueMatter/db2/elementlist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <stdlib.h>
#include <assert.h>

namespace db2
{
  ElementList* ElementList::s_instance = NULL;

  ElementList::ElementList()
  {}

  ElementList::~ElementList()
  {
    delete s_instance;
    s_instance = NULL;
  }

  ElementList* ElementList::instance()
  {
    if (s_instance == NULL)
      {
  s_instance = new ElementList;
  assert(s_instance != NULL);
  s_instance->init();
      }
    return(s_instance);
  }

  void ElementList::init()
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    stmt.prepare("select atomic_number, element_symbol from chem.elements order by atomic_number");
    char elementSymbol[16];
    SQLINTEGER atomicNo;
    stmt.bindCol(1, atomicNo);
    stmt.bindCol(2, (SQLCHAR*)&elementSymbol[0], 16);
    stmt.execute();
    d_elementSymbol.push_back("NULL"); // atomic number = 0
    int index = 1;
    while(stmt.fetch())
      {
  if (d_elementSymbol.size() == atomicNo)
    {
      d_elementSymbol.push_back(elementSymbol);
    }
  d_atomicNumber[elementSymbol] = atomicNo;
  assert(index == atomicNo);
  ++index;
      }
  }

  int ElementList::atomicNumber(const char* elementSymbol) const
  {
    std::map<std::string, int>::const_iterator iter =
      d_atomicNumber.find(elementSymbol);
    if (iter != d_atomicNumber.end())
      {
  return((*iter).second);
      }
    return(-1);

  }

  const char* ElementList::elementSymbol(int atomicNo) const
  {
    if ((atomicNo >=1) && (atomicNo < d_elementSymbol.size()))
      {
  return(d_elementSymbol[atomicNo].c_str());
      }
    return(NULL);
  }

  int ElementList::main(int argc, char** argv)
  {
    if (argc < 2)
      {
  std::cerr << argv[0] << " dbName" << std::endl;
  return(-1);
      }
    const char* dbName = argv[1];
    DBName::instance(dbName);
    ElementList* eltList = ElementList::instance();
    std::cout << "Element List" << std::endl;
    int idx = 1;
    while(eltList->elementSymbol(idx) != NULL)
      {
  std::cout << idx << " " << eltList->elementSymbol(idx) << " " <<
    eltList->atomicNumber(eltList->elementSymbol(idx)) << std:: endl;
  ++idx;
      }
    return(0);
  }

}
