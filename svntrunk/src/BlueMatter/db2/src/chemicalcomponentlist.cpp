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
// File: chemicalcomponentlist.cpp
// Author: RSG
// Date: April 29, 2002
// Namespace: db2
// Class: ChemicalComponentList
// Description: Singleton class encapsulating data and operations associated
//              with available molecular types
// ***********************************************************************

#include <BlueMatter/db2/chemicalcomponentlist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>

namespace db2
{
  ChemicalComponentList* ChemicalComponentList::s_instance = NULL;

  ChemicalComponentList::ChemicalComponentList()
  {}

  ChemicalComponentList::~ChemicalComponentList()
  {
    delete s_instance;
    s_instance = NULL;
  }

  ChemicalComponentList* ChemicalComponentList::instance()
  {
    if (s_instance == NULL)
      {
	s_instance = new ChemicalComponentList;
	assert(s_instance != NULL);
	s_instance->init();
      }
    return(s_instance);
  }

  void ChemicalComponentList::init()
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    stmt.prepare("select component_name from chem.chemical_component");
    char componentName[64];
    stmt.bindCol(1, (SQLCHAR*)&componentName[0], 64);
    stmt.execute();
    int index = 0;
    while(stmt.fetch())
      {
	d_componentFromIndex.push_back(componentName);
	d_indexOfComponent[componentName] = index;
	++index;
      }
  }

  int ChemicalComponentList::indexOfComponent(const char* chemicalComponent) const
  {
    std::map<std::string, int>::const_iterator iter =
      d_indexOfComponent.find(chemicalComponent);
    if (iter != d_indexOfComponent.end())
      {
	return((*iter).second);
      }
    return(-1);
  }

  const char* ChemicalComponentList::componentFromIndex(int index) const
  {
    if ((index >=0) && (index < d_componentFromIndex.size()))
      {
	return(d_componentFromIndex[index].c_str());
      }
    return(NULL);
  }

  int ChemicalComponentList::main(int argc, char** argv)
  {
    if (argc < 2)
      {
	std::cerr << argv[0] << " dbName" << std::endl;
	return(-1);
      }
    const char* dbName = argv[1];
    DBName::instance(dbName);
    ChemicalComponentList* mtList = ChemicalComponentList::instance();
    std::cout << "Chemical Component List" << std::endl;
    int idx = 0;
    while(mtList->componentFromIndex(idx) != NULL)
      {
	std::cout << idx << " " << mtList->componentFromIndex(idx) << " " <<
	  mtList->indexOfComponent(mtList->componentFromIndex(idx)) << std:: endl;
	++idx;
      }
    return(0);
  }
}
