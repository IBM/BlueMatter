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
// File: monomerinstancelist.cpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: MonomerInstanceList
// Description: Class encapsulating data and operations associated
//              with the chemical component instances contained within a
//              molecular system.
// ***********************************************************************

#include <BlueMatter/db2/monomerinstancelist.hpp>
#include <BlueMatter/db2/chemicalcomponentinstancelist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <sstream>
#include <stdlib.h>
#include <assert.h>

namespace db2
{

  MonomerInstanceList::MonomerInstanceList()
  {}
  
  void MonomerInstanceList::init(int sysId)
  {
    d_monomer.clear();
    ConnHandle& conn = DBName::instance()->connection();
    std::ostringstream countCmd;
    StmtHandle countStmt(conn);
    countCmd << "select count(*) from mdsystem.chain where "
       << "sys_id = " << sysId << std::ends;
    countStmt.prepare(countCmd.str().c_str());
    SQLINTEGER chainCount;
    countStmt.bindCol(1, chainCount);
    countStmt.execute();
    countStmt.fetch();
    d_monomer.resize(chainCount);
    countStmt.close();

    StmtHandle stmt(conn);
    std::ostringstream cmd;
    cmd << "select chain_id, monomer_ordinal, cc_instance_id "
  << "from mdsystem.monomer_instance "
  << "where sys_id = " << sysId
  << " order by chain_id, monomer_ordinal" << std::ends;
    stmt.prepare(cmd.str().c_str());
    SQLINTEGER chainId;
    SQLINTEGER monomerOrd;
    SQLINTEGER ccInstanceId;
    stmt.bindCol(1, chainId);
    stmt.bindCol(2, monomerOrd);
    stmt.bindCol(3, ccInstanceId);
    stmt.execute();
    int chainIdx = 0;
    int index = 0;
    while(stmt.fetch())
      {
  if (chainIdx != chainId)
    {
      chainIdx = chainId;
      index = 0; // reset monomer index
    }
  if (index != monomerOrd)
    {
      std::cerr << "MonomerInstanceList mismatch between index "
          << "and monomerOrd:"
          << " index = " << index
          << " monomerOrd = " << monomerOrd <<std::endl;
    }
  assert(monomerOrd == index);
  d_monomer[chainId].push_back(ccInstanceId);
  ++index;
      }
  }

  int MonomerInstanceList::main(int argc, char** argv)
  {
    if (argc < 3)
      {
  std::cerr << argv[0] << " dbName sysID" << std::endl;
  return(-1);
      }

    const char* dbName = argv[1];
    DBName::instance(dbName);
    int sysId = atoi(argv[2]);
    MonomerInstanceList moniList;
    moniList.init(sysId);
    ChemicalComponentInstanceList cciList;
    cciList.init(sysId);
    std::cout << "Monomer Instance List" << std::endl;
    for (int i = 0; i < moniList.size(); ++i) 
      {
  int j = 0;
  std::cout << "CHAIN: " << i
      << " of length " << moniList[i].size() << std::endl;
  for (std::vector<int>::const_iterator iter = moniList[i].begin();
       iter != moniList[i].end();
       ++iter)
    {
      std::cout << i << " "
          << j << " "
          << *iter << " "
          << cciList[*iter]
          << std::endl;
      ++j;
    }
      }
  }
}
