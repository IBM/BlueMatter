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
// File: molecularsystem.hpp
// Author: RSG
// Date: April 29, 2002
// Namespace: db2
// Class: MolecularSystem
// Description: Class encapsulating data and operations associated
//              with a molecular system whose data is stored in db2
// ***********************************************************************

#ifndef INCLUDE_DB2_MOLECULARSYSTEM_HPP
#define INCLUDE_DB2_MOLECULARSYSTEM_HPP


#include <BlueMatter/db2/atomtaglist.hpp>
#include <BlueMatter/db2/bondlist.hpp>
#include <BlueMatter/db2/chainlist.hpp>
#include <BlueMatter/db2/chemicalcomponentinstancelist.hpp>
#include <BlueMatter/db2/chemicalcomponentlist.hpp>
#include <db2/dbname.hpp>
#include <BlueMatter/db2/elementlist.hpp>
#include <BlueMatter/db2/moleculeinstancelist.hpp>
#include <BlueMatter/db2/moleculenamelist.hpp>
#include <BlueMatter/db2/moleculetypelist.hpp>
#include <BlueMatter/db2/monomerinstancelist.hpp>
#include <BlueMatter/db2/sitelist.hpp>
#include <BlueMatter/db2/sequenceindex.hpp>

#include <vector>
#include <cstdlib>
#include <string>

namespace db2
{

  class MolecularSystem
  {
  private:
    // data
    std::string  d_sysIdStr;
    AtomTagList* d_atomTag_p;
    BondList d_bond;
    ChainList d_chain;
    ChemicalComponentInstanceList d_ccInstance;
    ChemicalComponentList * d_chemComp_p;
    ElementList* d_element_p;
    MoleculeInstanceList d_molInstance;
    MoleculeNameList d_molName;
    MoleculeTypeList* d_molType_p;
    MonomerInstanceList d_monomer;
    SiteList d_site;
    CompareChemCompInstance* d_cciCompare_p;

    SequenceIndex<std::vector<Site>, CompareChemCompInstance>*
    d_sitesByChemicalComponentInstance_p;
    SequenceIndex<std::vector<Site>, CompareChemComp>*
    d_sitesByChemicalComponent_p;
    SequenceIndex<std::vector<Site>, CompareAtomicNumber>*
    d_sitesByAtomicNumber_p;

    CompareAtomTagIndex* d_atCompare_p;
    SequenceIndex<std::vector<Site>, CompareAtomTagIndex>*
    d_sitesByAtomTag_p;

    // methods
    MolecularSystem();
    MolecularSystem(const MolecularSystem&);
    MolecularSystem& operator=(const MolecularSystem&);
  public:
    ~MolecularSystem();
    // args to constructFromDB2: sys_id, database name (only specify
    // if DBName has not already been initialized)
    static MolecularSystem* constructFromDB2(int, const char* = NULL);
    const BondList& bonds() const;
    const ChainList& chains() const;
    const ChemicalComponentInstanceList& components() const;
    const MoleculeInstanceList& molecules() const;
    const MoleculeNameList& moleculeNames() const;
    const MonomerInstanceList& monomers() const;
    const SiteList& sites() const;
    const SequenceIndex<std::vector<Site>, CompareChemCompInstance>&
    sitesByChemicalComponentInstance() const;
    const SequenceIndex<std::vector<Site>, CompareChemComp>&
    sitesByChemicalComponent() const;
    const SequenceIndex<std::vector<Site>, CompareAtomicNumber>&
    sitesByAtomicNumber() const;

    const SequenceIndex<std::vector<Site>, CompareAtomTagIndex>&
    sitesByAtomTag() const;

    std::vector<int> SelectedSites(const char*) const;


    static int main(int, char**); // test driver
  };
}
#endif
