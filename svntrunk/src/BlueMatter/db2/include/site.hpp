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
// File: site.hpp
// Author: RSG
// Date: May 2, 2002
// Namespace: db2
// Class: Site
// Description: Class encapsulating data and methods associated with a
//              single site.
// ***********************************************************************

#ifndef INCLUDE_DB_SITE_HPP
#define INCLUDE_DB_SITE_HPP

#include <BlueMatter/db2/moleculeinstancelist.hpp>

#include <vector>
#include <cstring>

namespace db2
{
  class Site
  {
    friend class CompareChemComp;
    friend class CompareChemCompInstance;
    friend class CompareMoleculeInstance;
    friend class CompareMoleculeType;
    friend class CompareAtomicNumber;
    friend class CompareAtomTagIndex;
  private:
    double d_atomicMass;
    int d_ccInstId;
    int d_molInstId;
    int d_atomTagIdx;
    int d_atomicNo;
  public:
    inline Site();
    inline Site(int, int, int, int, const double&); // args: ccInstId, molInstId,
      // atomTagIdx, atomicNo, atomicMass
    inline Site(const Site&);
    inline Site& operator=(const Site&);
    inline const double& atomicMass() const;
    inline int chemicalComponentInstanceId() const;
    inline int moleculeInstanceId() const;
    inline int atomTagIdx() const;
    inline int atomicNumber() const;
    int  chemicalComponentIdx() const;
    const char* chemicalComponent() const;
    const char* atomTag() const;
    const char* elementSymbol() const;
  };

  class CompareAtomTagIndex
  {
  private:
    const std::vector<Site>& d_site;
  public:
    CompareAtomTagIndex(const std::vector<Site>&);
    inline bool operator()(int, int) const;
    inline int key(int) const;
  };

  class CompareChemCompInstance
  {
  private:
    const std::vector<Site>& d_site;
  public:
    CompareChemCompInstance(const std::vector<Site>&);
    inline bool operator()(int, int) const;
    inline int key(int) const;
  };

  class CompareChemComp
  {
  private:
    const std::vector<Site>& d_site;
  public:
    CompareChemComp(const std::vector<Site>&);
    inline bool operator()(int, int) const;
    inline int key(int) const;
  };

  class CompareMoleculeInstance
  {
  private:
    const std::vector<Site>& d_site;
  public:
    CompareMoleculeInstance(const std::vector<Site>&);
    inline bool operator()(int, int) const;
    inline int key(int) const;
  };

  class CompareAtomicNumber
  {
  private:
    const std::vector<Site>& d_site;
  public:
    CompareAtomicNumber(const std::vector<Site>&);
    inline bool operator()(int, int) const;
    inline int key(int) const;
  };

  class CompareMoleculeType
  {
  private:
    const std::vector<Site>& d_site;
    const MoleculeInstanceList& d_molInst;
  public:
    CompareMoleculeType(const std::vector<Site>&, const MoleculeInstanceList&);
    inline bool operator()(int, int) const;
    inline int key(int) const;
  };

  Site::Site() : d_ccInstId(-1), d_molInstId(-1),
		 d_atomTagIdx(-1), d_atomicNo(-1)
  {}

  Site::Site(int ccInstId, int molInstId, int atomTagIdx, int
	     atomicNo, const double& atomicMass)
    : d_ccInstId(ccInstId),
      d_molInstId(molInstId),
      d_atomTagIdx(atomTagIdx),
      d_atomicNo(atomicNo),
      d_atomicMass(atomicMass)
  {}

  Site::Site(const Site& proto)
    : d_ccInstId(proto.d_ccInstId),
      d_molInstId(proto.d_molInstId),
      d_atomTagIdx(proto.d_atomTagIdx),
      d_atomicNo(proto.d_atomicNo),
      d_atomicMass(proto.d_atomicMass)
  {}

  Site& Site::operator=(const Site& other)
  {
    if (this != &other)
      {
	d_ccInstId = other.d_ccInstId;
	d_molInstId = other.d_molInstId;
	d_atomTagIdx = other.d_atomTagIdx;
	d_atomicNo = other.d_atomicNo;
	d_atomicMass = other.d_atomicMass;
      }
    return(*this);
  }

  const double& Site::atomicMass() const
  {
    return(d_atomicMass);
  }

  int Site::chemicalComponentInstanceId() const
  {
    return(d_ccInstId);
  }

  int Site::moleculeInstanceId() const
  {
    return(d_molInstId);
  }
  
  int Site::atomTagIdx() const
  {
    return(d_atomTagIdx);
  }

  int Site::atomicNumber() const
  {
    return(d_atomicNo);
  }

  bool CompareChemComp::operator()(int id1, int id2) const
  {
    int cc1 = d_site[id1].chemicalComponentIdx();
    int cc2 = d_site[id2].chemicalComponentIdx();
    return (cc1 < cc2);
  }

  int CompareChemComp::key(int idx) const
  {
    return d_site[idx].chemicalComponentIdx();
  }
  bool CompareAtomTagIndex::operator()(int i1, int i2) const
  {
    return(d_site[i1].d_atomTagIdx < d_site[i2].d_atomTagIdx);
  }

  int CompareAtomTagIndex::key(int idx) const
  {
    return(d_site[idx].d_atomTagIdx);
  }

  bool CompareChemCompInstance::operator()(int i1, int i2) const
  {
    return(d_site[i1].d_ccInstId < d_site[i2].d_ccInstId);
  }

  int CompareChemCompInstance::key(int idx) const
  {
    return(d_site[idx].d_ccInstId);
  }

  bool CompareMoleculeInstance::operator()(int i1, int i2) const
  {
    return(d_site[i1].d_molInstId < d_site[i2].d_molInstId);
  }

  int CompareMoleculeInstance::key(int idx) const
  {
    return(d_site[idx].d_molInstId);
  }

  bool CompareAtomicNumber::operator()(int i1, int i2) const
  {
    return(d_site[i1].d_atomicNo < d_site[i2].d_atomicNo);
  }

  int CompareAtomicNumber::key(int idx) const
  {
    return(d_site[idx].d_atomicNo);
  }

  bool CompareMoleculeType::operator()(int i1, int i2) const
  {
    int rc = std::strcmp(d_molInst[d_site[i1].d_molInstId].moleculeType(),
			 d_molInst[d_site[i2].d_molInstId].moleculeType());
    if (rc == 0)
      {
	rc = std::strcmp(d_molInst[d_site[i1].d_molInstId].name(),
			 d_molInst[d_site[i2].d_molInstId].name());
      }
    return(rc < 0);
  }

  int CompareMoleculeType::key(int idx) const
  {
    return(d_site[idx].d_molInstId);
  }

}
#endif
