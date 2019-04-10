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
// File: moleculenamelist.hpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: MoleculeNameList
// Description: Class encapsulating data and operations associated
//              with the "specific" molecule names contained within a
//              molecular system.
// ***********************************************************************

#ifndef INCLUDE_DB2_MOLECULENAMELIST_HPP
#define INCLUDE_DB2_MOLECULENAMELIST_HPP

#include <BlueMatter/db2/moleculetypelist.hpp>

#include <vector>
#include <iostream>
#include <string>

namespace db2
{
  class SpecificMolecule
  {
    friend std::ostream& operator<<(std::ostream&, const SpecificMolecule&);
  private:
    static SpecificMolecule s_null;
    std::string d_molName;
    int d_molType;
  public:
    inline SpecificMolecule();
    inline SpecificMolecule(const std::string&, int);
    inline SpecificMolecule(const SpecificMolecule&);
    inline SpecificMolecule& operator=(const SpecificMolecule&);
    inline const char* name() const;
    inline const char* moleculeType() const;
    static inline const SpecificMolecule& null();
  };

  std::ostream& operator<<(std::ostream&, const SpecificMolecule&);

  class MoleculeNameList
  {
  private:
    // data
    std::vector<SpecificMolecule> d_nameList;
  public:
    MoleculeNameList();
    void init(int); // arg is sys_id
    inline const std::vector<SpecificMolecule>& moleculeName() const;
    static int main(int, char**); // test driver
  };

  SpecificMolecule::SpecificMolecule()
  {}

  SpecificMolecule::SpecificMolecule(const std::string& name, int
				     molType) : d_molName(name),
						d_molType(molType)
  {}

  SpecificMolecule::SpecificMolecule(const SpecificMolecule& proto)
    : d_molName(proto.d_molName), d_molType(proto.d_molType)
  {}

  SpecificMolecule& SpecificMolecule::operator=(const
						SpecificMolecule&
						other)
  {
    if (this != & other)
      {
	d_molName = other.d_molName;
	d_molType = other.d_molType;
      }
    return(*this);
  }

  const char* SpecificMolecule::name() const
  {
    return(d_molName.c_str());
  }

  const char* SpecificMolecule::moleculeType() const
  {
    return(MoleculeTypeList::instance()->typeFromIndex(d_molType));
  }

  const std::vector<SpecificMolecule>&
  MoleculeNameList::moleculeName() const
  {
    return(d_nameList);
  }

  const SpecificMolecule& SpecificMolecule::null()
  {
    return(s_null);
  }

}
#endif
