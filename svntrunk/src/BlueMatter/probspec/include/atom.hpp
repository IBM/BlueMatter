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
 #ifndef _INCLUDED_ATOM_
#define _INCLUDED_ATOM_

#include "list.hpp"
#include "bond.hpp"
#include "angle.hpp"
#include "torsion.hpp"
#include "improper.hpp"
#define NAMELEN 15
typedef char NAME[NAMELEN];


class Atom : public List
{
  Bond *d_bonds;
  Angle *d_angles;
  Torsion *d_tors;
  ImproperTorsion *d_imps;
  int d_nbonds;
  int d_nangles;
  int d_ntors;
  int d_nimps;

  NAME d_nameS;
  double d_mass;
  double d_charge;
  double d_ljeps;
  double d_ljrmin;
  double d_ljeps14;
  double d_ljrmin14;
  int  d_ljtype;
  int  d_lj14type;

public:
  Atom();
  ~Atom();
  virtual Atom * Next();
  virtual Atom * Prev();
  virtual Atom * Copy();
  virtual Atom * CopyList();
  virtual int Flatten(List ***arr);
  virtual void SetNext(List *t){d_next = t;}
  virtual void SetPrev(List *t){d_prev = t;}
  virtual  int SameType(List const &a);
  int SameName(Atom const &a);

  Atom & operator=(Atom const & at);
  int operator==(Atom const & a);

  int AddBond(Bond *bd);
  int AddAngle(Angle *bd);
  int AddTorsion(Torsion *bd);
  int AddImproper(ImproperTorsion *bd);

  char * Name(){return d_nameS;}
  Bond * Bonds(){return d_bonds;}
  Angle * Angles(){return d_angles;}
  Torsion * Torsions(){return d_tors;}
  ImproperTorsion * Impropers(){return d_imps;}

  int NBonds(){return d_nbonds;}
  int NAngles(){return d_nangles;}
  int NTorsions(){return d_ntors;}
  int NImpropers(){return d_nimps;}

  double & Mass(){return d_mass;}
  double & Charge(){return d_charge;}
  double & Eps(){return d_ljeps;}
  double & RMin(){return d_ljrmin;}
  double & Eps14(){return d_ljeps14;}
  double & RMin14(){return d_ljrmin14;}
  int & LJType(){ return d_ljtype;}
  int & LJ14Type(){ return d_lj14type;} 
  void Print(char *buf);
  int Read(FILE *fPF);
  int Write(FILE *fPF);
};


#endif
