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
// File: site.cpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: Site
// Description: Class encapsulating data and methods associated with a
//              single site.
// ***********************************************************************

#include <BlueMatter/db2/site.hpp>
#include <BlueMatter/db2/elementlist.hpp>
#include <BlueMatter/db2/chemicalcomponentlist.hpp>
#include <BlueMatter/db2/atomtaglist.hpp>

namespace db2
{

  const char* Site::chemicalComponent() const
  {
    return(ChemicalComponentList::instance()->componentFromIndex(AtomTagList::instance()->tagFromIndex(d_atomTagIdx).first));
  }

  int Site::chemicalComponentIdx() const
  {
    return(AtomTagList::instance()->tagFromIndex(d_atomTagIdx).first);
  }

  const char* Site::atomTag() const
  {
    return(AtomTagList::instance()->tagFromIndex(d_atomTagIdx).second.c_str());
  }

  const char* Site::elementSymbol() const
  {
    return(ElementList::instance()->elementSymbol(d_atomicNo));
  }

  CompareAtomTagIndex::CompareAtomTagIndex(const
           std::vector<Site>&
           site)
    : d_site(site)
  {}

  CompareChemCompInstance::CompareChemCompInstance(const
               std::vector<Site>&
               site)
    : d_site(site)
  {}

  CompareMoleculeInstance::CompareMoleculeInstance(const
               std::vector<Site>&
               site)
    : d_site(site)
  {}

  CompareAtomicNumber::CompareAtomicNumber(const
             std::vector<Site>&
             site)
    : d_site(site)
  {}

  CompareMoleculeType::CompareMoleculeType(const
             std::vector<Site>&
             site,
             const MoleculeInstanceList&
             molInst)
    : d_site(site), d_molInst(molInst)
  {}

}
