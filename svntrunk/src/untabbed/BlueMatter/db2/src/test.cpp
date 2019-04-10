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
 // test driver for the db2 chem routines

#include <BlueMatter/db2/moleculetypelist.hpp>
#include <BlueMatter/db2/elementlist.hpp>
#include <BlueMatter/db2/chemicalcomponentlist.hpp>
#include <BlueMatter/db2/atomtaglist.hpp>
#include <BlueMatter/db2/moleculenamelist.hpp>
#include <BlueMatter/db2/moleculeinstancelist.hpp>
#include <BlueMatter/db2/chemicalcomponentinstancelist.hpp>
#include <BlueMatter/db2/chainlist.hpp>
#include <BlueMatter/db2/monomerinstancelist.hpp>
#include <BlueMatter/db2/site.hpp>
#include <BlueMatter/db2/sitelist.hpp>
#include <BlueMatter/db2/bondlist.hpp>
#include <BlueMatter/db2/molecularsystem.hpp>
#include <BlueMatter/db2/sequenceindex.hpp>

int main(int argc, char** argv)
{
  int testId = -1;
  if (argc > 1)
    {
      testId = std::atoi(argv[1]);
    }

  int rc = 0;
  switch(testId)
    {
    default:
      std::cerr << argv[0] << " testID testArg1 testArg2 ... testArgN"
    << std::endl;
      std::cerr
  << "Tests (ids in brackets):\n"
  << "[1] db2::MoleculeTypeList\n"
  << "[2] db2::ElementList\n"
  << "[3] db2::ChemicalComponentList\n"
  << "[4] db2::AtomTagList\n"
  << "[5] db2::MoleculeNameList\n"
  << "[6] db2::MoleculeInstanceList\n"
  << "[7] db2::ChemicalComponentInstanceList\n"
  << "[8] db2::ChainList\n"
  << "[9] db2::MonomerInstanceList\n"
  << "[10] db2::SiteList\n"
  << "[11] db2::BondList\n"
  << "[12] db2::MolecularSystem\n"
  << "[13] db2::SequenceIndex\n"
  << "Test: " << std::endl;
      rc = -1;
      break;
    case 1:
      rc = db2::MoleculeTypeList::main(argc-1, &argv[1]);
      break;
    case 2:
      rc = db2::ElementList::main(argc-1, &argv[1]);
      break;
    case 3:
      rc = db2::ChemicalComponentList::main(argc-1, &argv[1]);
      break;
    case 4:
      rc = db2::AtomTagList::main(argc-1, &argv[1]);
      break;
    case 5:
      rc = db2::MoleculeNameList::main(argc-1, &argv[1]);
      break;
    case 6:
      rc = db2::MoleculeInstanceList::main(argc-1, &argv[1]);
      break;
    case 7:
      rc = db2::ChemicalComponentInstanceList::main(argc-1, &argv[1]);
      break;
    case 8:
      rc = db2::ChainList::main(argc-1, &argv[1]);
      break;
    case 9:
      rc = db2::MonomerInstanceList::main(argc-1, &argv[1]);
      break;
    case 10:
      rc = db2::SiteList::main(argc-1, &argv[1]);
      break;
    case 11:
      rc = db2::BondList::main(argc-1, &argv[1]);
      break;
    case 12:
      rc = db2::MolecularSystem::main(argc-1, &argv[1]);
      break;
    case 13:
      rc = db2::SequenceIndex<std::vector<int>, db2::CompareInteger>::main(argc-1, &argv[1]);
      break;
    }
  return(rc);
}
