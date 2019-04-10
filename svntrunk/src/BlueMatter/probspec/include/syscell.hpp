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
 #ifndef _INCLUDED_SYSCELL_HPP_
#define _INCLUDED_SYSCELL_HPP_


//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~
//  ~~~  SysCell: Base class for molecular system definitions.   
//  ~~~           Contains methods and data members for atom and
//  ~~~           connectivity and all terms. Supports both list  
//  ~~~           and table representations of term instances
//  ~~~           and types.
//  ~~~
//  ~~~ Author: MCP   
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "terms.hpp"

class SysCell : public List
{
friend class MolecularSystem;

public:
  int d_nats,d_nbds,d_nangs,d_ntors,d_nimps;
  int d_natypes,d_napairs,d_na14pairs,d_nbtypes,d_nangtypes,d_ntortypes,d_nimptypes;
  Atom *d_ats,**d_atab,*d_atypes,*d_aPairs, *d_a14Pairs;
  Bond *d_bds,**d_btab,*d_btypes;
  Angle *d_angles,**d_angtab,*d_angtypes;
  Torsion *d_tors,**d_tortab,*d_tortypes;
  ImproperTorsion *d_imps,**d_imptab,*d_imptypes;
  

  void Destroy();

public:
  SysCell();
  ~SysCell();

  Atom ** ATab(){return d_atab;}
  Atom * Ats(){return d_ats;}
  Atom * ATypes(){return d_atypes;}
  Atom * APairs(){return d_aPairs;}
  Atom * A14Pairs(){return d_a14Pairs;}

  Bond ** BTab(){return d_btab;}
  Bond * Bds(){return d_bds;}
  Bond * BTypes(){return d_btypes;}

  Angle ** AngTab(){return d_angtab;}
  Angle * Angs(){return d_angles;}
  Angle * AngTypes(){return d_angtypes;}

  Torsion ** TorTab(){return d_tortab;}
  Torsion * Tors(){return d_tors;}
  Torsion * TorTypes(){return d_tortypes;}

  ImproperTorsion ** ImpTab(){return d_imptab;}
  ImproperTorsion * Imps(){return d_imps;}
  ImproperTorsion * ImpTypes(){return d_imptypes;}

  int NAtoms(){return d_nats;}
  int NBonds(){return d_nbds;}
  int NAngles(){return d_nangs;}
  int NTorsions(){return d_ntors;}
  int NImpropers(){return d_nimps;}
  int NATypes(){return d_natypes;}
  int NPairs(){return d_napairs;}
  int N14Pairs(){return d_na14pairs;}
  int NBTypes(){return d_nbtypes;}
  int NAngTypes(){return d_nangtypes;}
  int NTorTypes(){return d_ntortypes;}
  int NImpTypes(){return d_nimptypes;}

  int CheckIfOnTorsionList(int a1, int a4);

  int AddAtom(Atom *at);
  int AddPair(Atom *at);
  int Add14Pair(Atom *at);
  int AddBond(Bond *bd);
  int AddAngle(Angle *a);
  int AddTorsion(Torsion *a);
  int AddImproperTorsion(ImproperTorsion *a);
  int Finalize();

  int Derive1_NAts(int n, Atom *a,Atom **lv);
  int DeriveNeighborhoodLists();
  int ReduceAtomTypes();
  int AtypeNum(char *tag);
  int AtypeName(int typ,char *name);
  int  LocateAtomName(char * name, Atom **at);
  int operator==( const SysCell &sys);
  SysCell & operator=(const SysCell &sys);
  virtual SysCell * Next();
  virtual SysCell * Prev();
  virtual int & Type();
  virtual int & GlobalId();
  virtual int & LocalId();
  virtual SysCell * Copy();
  virtual SysCell * CopyList();
  int  ListEqv(List *ll,List *rr);

};

#endif



















































