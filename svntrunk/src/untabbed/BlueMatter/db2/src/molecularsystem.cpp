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
// File: molecularsystem.cpp
// Author: RSG
// Date: May 6, 2002
// Namespace: db2
// Class: MolecularSystem
// Description: Class encapsulating data and operations associated
//              with a molecular system whose data is stored in db2
// ***********************************************************************

#include <BlueMatter/db2/molecularsystem.hpp>

#include <cstdlib>
#include <assert.h>
#include <string>
#include <iostream>
#include <strstream>
#include <db2/stmthandle.hpp>

namespace db2
{
  MolecularSystem::MolecularSystem()
    : d_molInstance(d_molName)
  {}

  MolecularSystem::MolecularSystem(const MolecularSystem&)
    : d_molInstance(d_molName)
  {}

  MolecularSystem::~MolecularSystem()
  {
    delete d_sitesByChemicalComponentInstance_p;
    delete d_cciCompare_p;

    delete d_sitesByChemicalComponent_p;


  }

  MolecularSystem& MolecularSystem::operator=(const MolecularSystem&)
  {
    return(*this);
  }

  MolecularSystem* MolecularSystem::constructFromDB2(int sysId, const
                 char* dbName)
  {
    MolecularSystem* ms = new MolecularSystem;
    if (ms == NULL)
      {
  return(NULL);
      }
    // setup singletons

    std::strstream buf;
    buf << sysId << std::ends;
    ms->d_sysIdStr = buf.str();


    DBName::instance(dbName);
    ms->d_atomTag_p = AtomTagList::instance();
    ms->d_chemComp_p = ChemicalComponentList::instance();
    ms->d_element_p = ElementList::instance();
    ms->d_molType_p = MoleculeTypeList::instance();

    // other member objects
    ms->d_bond.init(sysId);
    ms->d_chain.init(sysId);
    ms->d_ccInstance.init(sysId);
    ms->d_molName.init(sysId);
    ms->d_molInstance.init(sysId);
    ms->d_monomer.init(sysId);
    ms->d_site.init(sysId);
    
    // create index objects
    ms->d_cciCompare_p = new
      CompareChemCompInstance(ms->d_site.asVector());
    assert(ms->d_cciCompare_p != NULL);

    ms->d_sitesByChemicalComponentInstance_p = 
      SequenceIndex<std::vector<Site>,
      CompareChemCompInstance>::create(ms->d_site.asVector(), *ms->d_cciCompare_p);
    assert(ms->d_sitesByChemicalComponentInstance_p != NULL);

    return(ms);
  }

  std::vector<int> MolecularSystem::SelectedSites(const char* select_stmt) const
  {
    std::strstream buf;
    std::vector<int> idlist;

    ConnHandle& conn = DBName::instance()->connection();
    StmtHandle stmt(conn);

    buf << "select site_id from mdsystem.site where sys_id = " <<  
      d_sysIdStr << " and " << select_stmt << " order by site_id " << std::ends;

    stmt.prepare(buf.str());


    char IDStr[64];
    stmt.bindCol(1, (SQLCHAR*)&IDStr[0], 64);
    stmt.execute();
    while(stmt.fetch())
      {
  int index = atoi(IDStr);
  idlist.push_back(index);
      }
    return idlist;
  }

  const BondList& MolecularSystem::bonds() const
  {
    return(d_bond);
  }
  const ChainList& MolecularSystem::chains() const
  {
    return(d_chain);
  }
  const ChemicalComponentInstanceList& MolecularSystem::components() const
  {
    return(d_ccInstance);
  }
  const MoleculeInstanceList& MolecularSystem::molecules() const
  {
    return(d_molInstance);
  }
  const MoleculeNameList& MolecularSystem::moleculeNames() const
  {
    return(d_molName);
  }
  const MonomerInstanceList& MolecularSystem::monomers() const
  {
    return(d_monomer);
  }
  const SiteList& MolecularSystem::sites() const
  {
    return(d_site);
  }

  const SequenceIndex<std::vector<Site>, CompareChemCompInstance>&
  MolecularSystem::sitesByChemicalComponentInstance() const
  {
    return(*d_sitesByChemicalComponentInstance_p);
  }
  const SequenceIndex<std::vector<Site>, CompareChemComp>&
  MolecularSystem::sitesByChemicalComponent() const
  {
    return(*d_sitesByChemicalComponent_p);
  }

  int MolecularSystem::main(int argc, char** argv)
  {
    if (argc < 3)
      {
  std::cerr << argv[0] << " dbName sysID" << std::endl;
  return(-1);
      }
    
    int sysId = std::atoi(argv[2]);
    MolecularSystem* ms = MolecularSystem::constructFromDB2(sysId, argv[1]);
    if (ms == NULL)
      {
  std::cerr << "Error creating MolecularSystem" << std::endl;
  return(-2);
      }
    std::cout << "Molecular System" << std::endl;
    for (int i = 0; i < ms->components().size(); ++i)
      {
  for (std::vector<int>::const_iterator iter =
         ms->sitesByChemicalComponentInstance().begin(i);
       iter != ms->sitesByChemicalComponentInstance().end(i);
       ++iter)
    {
      std::cout << std::setw(8) << i << " "
          << std::setw(8) << ms->components()[i] << " "
          << std::setw(8) << *iter << " "
          << std::setw(8) << ms->sites()[*iter].atomTag()
          << std::endl;
    }
      }
    return(0);
  }
}

