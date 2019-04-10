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
// File: moleculeinstancelist.cpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: MoleculeInstanceList
// Description: Class encapsulating data and operations associated
//              with the molecule instances contained within a
//              molecular system.
// ***********************************************************************

#include <BlueMatter/db2/moleculeinstancelist.hpp>
#include <BlueMatter/db2/moleculenamelist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <sstream>
#include <stdlib.h>
#include <assert.h>

namespace db2
{

  MoleculeInstanceList::MoleculeInstanceList(const MoleculeNameList& nameList)
    : d_nameList(nameList)
  {}
  
  void MoleculeInstanceList::init(int sysId)
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    std::ostringstream cmd;
    d_moleculeList.clear();
    cmd << "select mol_instance_id, mol_id from mdsystem.molecule_instance "
	<< "where sys_id = " << sysId
	<< " order by mol_instance_id" << std::ends;
    stmt.prepare(cmd.str().c_str());
    SQLINTEGER molInstanceId;
    SQLINTEGER molId;
    stmt.bindCol(1, molInstanceId);
    stmt.bindCol(2, molId);
    stmt.execute();
    int index = 0;
    while(stmt.fetch())
      {
	if (index != molInstanceId)
	  {
	    std::cerr << "MoleculeInstanceList mismatch between index and mol_instance_id:"
		      << " index = " << index
		      << " mol_instance_id = " << molInstanceId <<std::endl;
	  }
	assert(molInstanceId == index);
	d_moleculeList.push_back(molId);
	++index;
      }
  }

  int MoleculeInstanceList::main(int argc, char** argv)
  {
    if (argc < 3)
      {
	std::cerr << argv[0] << " dbName sysID" << std::endl;
	return(-1);
      }

    const char* dbName = argv[1];
    DBName::instance(dbName);
    int sysId = atoi(argv[2]);
    MoleculeNameList mList;
    mList.init(sysId);
    MoleculeInstanceList miList(mList);
    miList.init(sysId);
    std::cout << "Molecule Instance List" << std::endl;
    for (int i = 0; i < miList.size(); ++i) 
      {
	std::cout << i << " " << miList[i] << std::endl;
      }
  }
}
