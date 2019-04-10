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
// File: moleculenamelist.cpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: MoleculeNameList
// Description: Class encapsulating data and operations associated
//              with the "specific" molecule names contained within a
//              molecular system.
// ***********************************************************************

#include <BlueMatter/db2/moleculenamelist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>

namespace db2
{
  SpecificMolecule SpecificMolecule::s_null("NULL", -1);

  MoleculeNameList::MoleculeNameList()
  {}
  
  void MoleculeNameList::init(int sysId)
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    std::ostringstream cmd;
    cmd << "select mol_id, class, molecule_name from mdsystem.specific_molecule "
  << "where sys_id = " << sysId << " order by mol_id" << std::ends;
    stmt.prepare(cmd.str().c_str());
    char moleculeName[64];
    char moleculeType[64];
    SQLINTEGER molId;
    stmt.bindCol(1, molId);
    stmt.bindCol(2, (SQLCHAR*)&moleculeType[0], 64);
    stmt.bindCol(3, (SQLCHAR*)&moleculeName[0], 64);
    stmt.execute();
    int index = 0;
    while(stmt.fetch())
      {
  if (index != molId)
    {
      std::cerr << "MoleculeNameList  mismatch between index and mol_id:"
          << " index = " << index
          << " mol_id = " << molId <<std::endl;
    }
  assert(molId == index);
  MoleculeTypeList* mtList = MoleculeTypeList::instance();
  SpecificMolecule
    mol(moleculeName,
        mtList->indexOfType(moleculeType));
  d_nameList.push_back(mol);
  ++index;
      }
  }

  std::ostream& operator<<(std::ostream& os, const SpecificMolecule& mol)
  {
    os << std::setw(4) << mol.d_molType << " "
       << std::setw(12) << mol.moleculeType() << " "
       << std::setw(16) << mol.name();
    return(os);
  }

  int MoleculeNameList::main(int argc, char** argv)
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
    std::cout << "Molecule Name List" << std::endl;
    int molId = 0;
    for (std::vector<SpecificMolecule>::const_iterator iter =
     mList.moleculeName().begin();
   iter != mList.moleculeName().end();
   ++iter)
      {
  std::cout << molId << " " << *iter << std::endl;
  ++molId;
      }
  }
}
