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
// File: moleculetypelist.cpp
// Author: RSG
// Date: April 29, 2002
// Namespace: db2
// Class: MoleculeTypeList
// Description: Singleton class encapsulating data and operations associated
//              with available molecular types
// ***********************************************************************

#include <BlueMatter/db2/moleculetypelist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>

namespace db2
{
  MoleculeTypeList* MoleculeTypeList::s_instance = NULL;

  MoleculeTypeList::MoleculeTypeList()
  {}

  MoleculeTypeList::~MoleculeTypeList()
  {
    delete s_instance;
    s_instance = NULL;
  }

  MoleculeTypeList* MoleculeTypeList::instance()
  {
    if (s_instance == NULL)
      {
  s_instance = new MoleculeTypeList;
  assert(s_instance != NULL);
  s_instance->init();
      }
    return(s_instance);
  }

  void MoleculeTypeList::init()
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    stmt.prepare("select class from chem.molecule_type");
    char className[64];
    stmt.bindCol(1, (SQLCHAR*)&className[0], 64);
    stmt.execute();
    int index = 0;
    while(stmt.fetch())
      {
  d_typeFromIndex.push_back(className);
  d_indexOfType[className] = index;
  ++index;
      }
  }

  int MoleculeTypeList::indexOfType(const char* moleculeType) const
  {
    std::map<std::string, int>::const_iterator iter =
      d_indexOfType.find(moleculeType);
    if (iter != d_indexOfType.end())
      {
  return((*iter).second);
      }
    return(-1);
  }

  const char* MoleculeTypeList::typeFromIndex(int index) const
  {
    if ((index >=0) && (index < d_typeFromIndex.size()))
      {
  return(d_typeFromIndex[index].c_str());
      }
    return(NULL);
  }

  int MoleculeTypeList::main(int argc, char** argv)
  {
    if (argc < 2)
      {
  std::cerr << argv[0] << " dbName" << std::endl;
  return(-1);
      }
    const char* dbName = argv[1];
    DBName::instance(dbName);
    MoleculeTypeList* mtList = MoleculeTypeList::instance();
    std::cout << "Molecule Type List" << std::endl;
    int idx = 0;
    while(mtList->typeFromIndex(idx) != NULL)
      {
  std::cout << idx << " " << mtList->typeFromIndex(idx) << " " <<
    mtList->indexOfType(mtList->typeFromIndex(idx)) << std:: endl;
  ++idx;
      }
    return(0);
  }
}
