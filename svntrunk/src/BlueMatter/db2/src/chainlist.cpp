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
// File: chainlist.cpp
// Author: RSG
// Date: May 5, 2002
// Namespace: db2
// Class: ChainList
// Description: Class encapsulating data and operations associated
//              with the "chains" (singly branched polymers) contained
//              within a molecular system.
// ***********************************************************************

#include <BlueMatter/db2/chainlist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>

#include <sstream>
#include <stdlib.h>
#include <assert.h>

namespace db2
{
  ChainList::ChainList()
  {}
  
  void ChainList::init(int sysId)
  {
    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);
    std::ostringstream cmd;
    cmd << "select chain_id, chain_desc from mdsystem.chain "
	<< "where sys_id = " << sysId << " order by chain_id" << std::ends;
    stmt.prepare(cmd.str().c_str());
    char chainDescription[64];
    SQLINTEGER chainId;
    stmt.bindCol(1, chainId);
    stmt.bindCol(2, (SQLCHAR*)&chainDescription[0], 64);
    stmt.execute();
    int index = 0;
    while(stmt.fetch())
      {
	if (index != chainId)
	  {
	    std::cerr << "ChainList  mismatch between index and chain_id:"
		      << " index = " << index
		      << " chain_id = " << chainId <<std::endl;
	  }
	assert(chainId == index);
	std::pair<int, std::string> chainElt(chainId, chainDescription);
	d_chainList.push_back(chainElt);
	++index;
      }
  }

  int ChainList::main(int argc, char** argv)
  {
    if (argc < 3)
      {
	std::cerr << argv[0] << " dbName sysID" << std::endl;
	return(-1);
      }

    const char* dbName = argv[1];
    DBName::instance(dbName);
    int sysId = atoi(argv[2]);
    ChainList cList;
    cList.init(sysId);
    std::cout << "Chain List" << std::endl;
    int chainId = 0;
    for (std::vector<std::pair<int, std::string> >::const_iterator iter =
	   cList.chain().begin();
	 iter != cList.chain().end();
	 ++iter)
      {
	std::cout << chainId << " " << (*iter).first
		  << " " << (*iter).second << std::endl;
	++chainId;
      }
  }
}
