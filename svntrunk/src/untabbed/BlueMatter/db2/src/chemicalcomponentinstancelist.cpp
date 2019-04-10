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
// File: chemicalcomponentinstancelist.cpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: ChemicalComponentInstanceList
// Description: Class encapsulating data and operations associated
//              with the chemical component instances contained within a
//              molecular system.
// ***********************************************************************

#include <BlueMatter/db2/chemicalcomponentinstancelist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <sstream>
#include <stdlib.h>
#include <assert.h>

namespace db2
{

  ChemicalComponentInstanceList::ChemicalComponentInstanceList()
  {}
  
  void ChemicalComponentInstanceList::init(int sysId)
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    std::ostringstream cmd;
    d_ccInstList.clear();
    cmd << "select cc_instance_id, component_name from mdsystem.chem_comp_instance "
  << "where sys_id = " << sysId
  << " order by cc_instance_id" << std::ends;
    stmt.prepare(cmd.str().c_str());
    SQLINTEGER ccInstanceId;
    char componentName[64];
    stmt.bindCol(1, ccInstanceId);
    stmt.bindCol(2, (SQLCHAR*)&componentName[0], 64);
    stmt.execute();
    ChemicalComponentList* ccList = ChemicalComponentList::instance();
    int index = 0;
    while(stmt.fetch())
      {
  if (index != ccInstanceId)
    {
      std::cerr << "ChemicalComponentInstanceList mismatch between index and cc_instance_id:"
          << " index = " << index
          << " cc_instance_id = " << ccInstanceId <<std::endl;
    }
  assert(ccInstanceId == index);
  d_ccInstList.push_back(ccList->indexOfComponent(componentName));
  ++index;
      }
  }

  int ChemicalComponentInstanceList::main(int argc, char** argv)
  {
    if (argc < 3)
      {
  std::cerr << argv[0] << " dbName sysID" << std::endl;
  return(-1);
      }

    const char* dbName = argv[1];
    DBName::instance(dbName);
    int sysId = atoi(argv[2]);
    ChemicalComponentInstanceList cciList;
    cciList.init(sysId);
    ChemicalComponentList* ccList = ChemicalComponentList::instance();
    std::cout << "Chemical Component Instance List" << std::endl;
    for (int i = 0; i < cciList.size(); ++i) 
      {
  std::cout << i << " "
      << cciList[i] << std::endl;
      }
  }
}
