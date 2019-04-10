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
// File: sitelist.hpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: SiteList
// Description: Class encapsulating data and methods associated with
//              the list of sites within a molecular system.
// ***********************************************************************

#include <BlueMatter/db2/sitelist.hpp>
#include <BlueMatter/db2/elementlist.hpp>
#include <BlueMatter/db2/chemicalcomponentlist.hpp>
#include <BlueMatter/db2/chemicalcomponentinstancelist.hpp>
#include <BlueMatter/db2/atomtaglist.hpp>
#include <BlueMatter/db2/moleculeinstancelist.hpp>
#include <BlueMatter/db2/moleculenamelist.hpp>

#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>

namespace db2
{

  SiteList::SiteList()
  {}
  
  void SiteList::init(int sysId)
  {
    d_site.clear();
    ConnHandle& conn = DBName::instance()->connection();

    std::ostringstream countCmd;
    StmtHandle countStmt(conn);
    countCmd << "select count(*) from mdsystem.site where "
	     << "sys_id = " << sysId << std::ends;
    countStmt.prepare(countCmd.str().c_str());
    SQLINTEGER siteCount;
    countStmt.bindCol(1, siteCount);
    countStmt.execute();
    countStmt.fetch();
    d_site.reserve(siteCount);
    countStmt.close();

    StmtHandle stmt(conn);
    std::ostringstream cmd;
    cmd << "select site_id, "
	<< "cc_instance_id, "
	<< "mol_instance_id, "
	<< "element_symbol, "
	<< "component_name, "
	<< "atom_tag, "
	<< "atomic_mass "
	<< "from mdsystem.site "
	<< "where sys_id = " << sysId
	<< " order by site_id" << std::ends;
    stmt.prepare(cmd.str().c_str());
    SQLINTEGER siteId;
    SQLINTEGER cciId;
    SQLINTEGER miId;
    char eltSym[64];
    char ccName[64];
    char atomTag[64];
    SQLDOUBLE atomicMass;
    stmt.bindCol(1, siteId);
    stmt.bindCol(2, cciId);
    stmt.bindCol(3, miId);
    stmt.bindCol(4, (SQLCHAR*)&eltSym[0], 64);
    stmt.bindCol(5, (SQLCHAR*)&ccName[0], 64);
    stmt.bindCol(6, (SQLCHAR*)&atomTag[0], 64);
    stmt.bindCol(7, atomicMass);
    stmt.execute();

    int index = 0;
    ElementList* eltList = ElementList::instance();
    ChemicalComponentList* ccList = ChemicalComponentList::instance();
    AtomTagList* atList = AtomTagList::instance();

    while(stmt.fetch())
      {
	if (index != siteId)
	  {
	    std::cerr << "SiteList mismatch between index and site_id:"
		      << " index = " << index
		      << " site_id = " << siteId <<std::endl;
	  }
	assert(siteId == index);
	int eltIdx = eltList->atomicNumber(eltSym);
	int ccIdx = ccList->indexOfComponent(ccName);
	int tagIdx =
	  atList->indexOfTag(std::pair<int, std::string>(ccIdx, atomTag));
	Site newSite(cciId, miId, tagIdx, eltIdx, atomicMass);
	d_site.push_back(newSite);
	++index;
      }
  }

  const Site& SiteList::operator[](int index) const
  {
    return(d_site[index]);
  }

  int SiteList::main(int argc, char** argv)
  {
    if (argc < 3)
      {
	std::cerr << argv[0] << " dbName sysID" << std::endl;
	return(-1);
      }

    const char* dbName = argv[1];
    DBName::instance(dbName);
    int sysId = atoi(argv[2]);
    ElementList* eltList = ElementList::instance();
    ChemicalComponentList* ccList = ChemicalComponentList::instance();
    AtomTagList* atList = AtomTagList::instance();
    MoleculeNameList mList;
    mList.init(sysId);
    MoleculeInstanceList miList(mList);
    miList.init(sysId);
    ChemicalComponentInstanceList cciList;
    cciList.init(sysId);

    
    SiteList sList;
    sList.init(sysId);
    int siteId  = 0;
    std::cout << "Site List" << std::endl;
    std::cout << std::setw(8) << "SiteID" << " "
	      << std::setw(8) << "CCInst" << " "
	      << std::setw(5) << "CC" << " "
	      << std::setw(5) << "ELT" << " "
	      << std::setw(5) << "TAG" << " "
	      << std::setw(8) << "MolInst" << " "
	      << std::setw(16) << "MolType" << " "
	      << std::setw(16) << "MolName" << " "
	      << std::setw(16) << "Mass"
	      << std::endl;
      
    for (std::vector<Site>::const_iterator iter = sList.asVector().begin();
	 iter != sList.asVector().end();
	 ++iter)
      {
	std::cout
	  << std::setw(8) << siteId << " "
	  << std::setw(8) << (*iter).chemicalComponentInstanceId() << " "
	  << std::setw(5) <<
	  cciList[((*iter).chemicalComponentInstanceId())] << " "
	  << std::setw(5)
	  << eltList->elementSymbol((*iter).atomicNumber()) << " "
	  << std::setw(5) <<
	  atList->tagFromIndex((*iter).atomTagIdx()).second << " "
	  << std::setw(8) << (*iter).moleculeInstanceId() << " "
	  << std::setw(16) << miList[(*iter).moleculeInstanceId()] << " "
	  << std::setw(16) << " "
	  << std::setw(16) << (*iter).atomicMass() 
	  << std::endl;
	++siteId;
      }
  }
}
