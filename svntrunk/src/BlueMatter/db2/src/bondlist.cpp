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
// File: bondlist.hpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: BondList
// Description: Class encapsulating data and methods associated with
//              the list of bonds within a molecular system.
// ***********************************************************************

#include <BlueMatter/db2/bondlist.hpp>
#include <db2/dbname.hpp>

#include <db2/stmthandle.hpp>

#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>

namespace db2
{

  BondList::BondList()
  {}
  
  void BondList::init(int sysId)
  {
    d_adjList.clear();
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
    d_adjList.resize(siteCount);
    countStmt.close();

    StmtHandle stmt(conn);
    std::ostringstream cmd;
    cmd << "select site_id1, "
	<< "site_id2 "
	<< "from mdsystem.bond "
	<< "where sys_id = " << sysId
	<< std::ends;
    stmt.prepare(cmd.str().c_str());
    SQLINTEGER siteId1;
    SQLINTEGER siteId2;
    stmt.bindCol(1, siteId1);
    stmt.bindCol(2, siteId2);
    stmt.execute();

    while(stmt.fetch())
      {
	d_adjList[siteId1].push_back(siteId2);
	d_adjList[siteId2].push_back(siteId1);
      }
  }

  int BondList::main(int argc, char** argv)
  {
    if (argc < 3)
      {
	std::cerr << argv[0] << " dbName sysID" << std::endl;
	return(-1);
      }

    const char* dbName = argv[1];
    DBName::instance(dbName);
    int sysId = atoi(argv[2]);
    
    BondList bList;
    bList.init(sysId);
    std::cout << "Bond List" << std::endl;
    std::cout << std::setw(8) << "SiteID" << " "
	      << std::setw(8) << "Adj1" << " "
	      << std::setw(8) << "Adj2" << " "
	      << std::setw(8) << "Adj3" << " "
	      << std::setw(8) << "Adj4"
	      << std::endl;
      
    int siteId  = 0;
    for (std::vector<std::vector<int> >::const_iterator iter =
	   bList.asAdjList().begin();
	 iter != bList.asAdjList().end();
	 ++iter)
      {
	std::cout
	  << std::setw(8) << siteId << " ";
	  for (std::vector<int>::const_iterator iter2 =
		 (*iter).begin();
	       iter2 != (*iter).end();
	       ++iter2)
	    {
	      std::cout
		<< std::setw(8) << (*iter2) << " ";
	    }
	  std::cout << std::endl;
	  ++siteId;
      }
    return(0);
  }
}
