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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <BlueMatter/atom.hpp>

Atom * Atom::Next(){return (Atom *)d_next;}
Atom * Atom::Prev(){return (Atom *)d_prev;}
int Atom::Flatten(List*** lr)
{
  Atom ***ar = (Atom ***)lr;
  int i,n = Count();
  Atom *ls = this;
  assert(n > 0);
  Atom **arr = new Atom*[n];
  for(i=0;i<n;i++)
    {
      assert(ls != NULL);
      arr[i] = ls; //(Atom *)ls->Copy();
      //arr[i]->d_localid = i;
      ls = ls->Next();
    }
  *ar = arr;
  return n;
}

Atom * Atom::Copy()
{
   Atom *at = new Atom();
   *at = *this;
   return at;
}


Atom * Atom::CopyList()
{
 Atom *ls = NULL;
 Atom *tls = this;
 while(tls != NULL)
   {
     Atom *lls = new Atom();
     *lls = *tls;
     if(ls == NULL)
       ls = lls;
     else
       ls->Append(lls);
     lls->LocalId() = tls->LocalId();
     tls = tls->Next();
   }
 return ls;
}

int Atom::SameType(List const &l)
{
  Atom const * a = (Atom const *)&l;
  if(d_type == a->d_type &&
     d_mass == a->d_mass &&
     d_charge == a->d_charge)
    {
      return 1;
    }
  return 0;
}


int Atom::SameName(Atom const &l)
{
  if(!strcmp( l.d_nameS, d_nameS))
    {
      return 1;
    }
  return 0;
}


//  int Atom::RemoveBond(Bond *bd)
//  {
//    if(d_bonds != NULL)
//        return d_bonds->RemoveBond(bd);
//    return 0;
//  }
//  int Atom::RemoveAngle(Angle *a)
//  {
//    if(d_angles != NULL)
//        return d_angle->RemoveAngle(a);
//    return 0;
//  }
//  int Atom::RemoveTorsion(Torsion *t)
//  {
//    if(d_tors != NULL)
//        return d_tors->RemoveTorsion(t);
//    return 0;
//  }
//  int Atom::RemoveImproper(ImproperTorsion *t)
//  {
//    if(d_imps != NULL)
//        return d_imps->RemoveImproper(t);
//    return 0;
//  }


void Atom::Print(char *buf)
{
  char tbuf[256];
  sprintf(tbuf,"Atom: [g: %3d,l: %3d,t: %3d] |%s| bds %2d angs %2d tors %2d imps %2d\n",d_globalid,d_localid,d_type,d_nameS,d_nbonds,d_nangles,d_ntors,d_nimps);
  strcat(buf,tbuf);
  sprintf(tbuf,"\tM: %5.2f Q: %5.2f E: %5.2f S: %5.2f E14: %5.2f S14: %5.2f",d_mass,d_charge,d_ljeps,d_ljrmin,d_ljeps14,d_ljrmin14);
  strcat(buf,tbuf);
}

int Atom::Read(FILE *fPF)
{
   char bufS[101];
   fgets(bufS,100,fPF);
   char *sPS;
   double charge;
   sPS = &bufS[1];
   int offSet;
   sscanf(sPS,"%d %d %n",&d_localid,&d_type, &offSet);
   if (sscanf(sPS + offSet, "%lf", &charge) != 1)
     {  charge = 0.0;   }
   d_charge = charge;
   int p;
   sPS = strstr(bufS,"Atm[");
   if(sPS != NULL)
     {
       sPS += 4; sscanf(sPS,"%d",&p);
       d_globalid = p;
     }

   return 1;
}
int Atom::Write(FILE *fPF)
{
   fprintf(fPF,"{ %4d %d %10.4lf }Atm[%5d,%d]\n",
      d_localid,d_type,d_charge,d_globalid,d_localid);
   return 1;

}

Atom::Atom()
{
  d_bonds = NULL;  d_nbonds = 0;
  d_angles = NULL; d_nangles = 0;
  d_tors = NULL; d_ntors = 0;
  d_imps = NULL; d_nimps = 0;
  d_nameS[0] = '\0';
  d_localid = -1;      d_globalid = -1;      d_type = -1;      d_ljtype = -1;  d_lj14type;
  d_mass = 0.0;      d_charge = 0.0;         d_ljeps = 0.0;
  d_ljrmin = 0.0;      d_ljeps14 = 0.0;      d_ljrmin14 = 0.0;
}

Atom::~Atom()
{
  if(d_bonds != NULL)
    {     delete d_bonds;         d_bonds = NULL;       }
  if(d_angles != NULL)
    {     delete d_angles;      d_angles = NULL;        }
  if(d_tors != NULL)
    {     delete d_tors;          d_tors = NULL;        }
  if(d_imps != NULL)
    {     delete d_imps;          d_imps = NULL;        }
}
int Atom::AddBond(Bond *bd)
{
  d_nbonds++;
  if(d_bonds == NULL){d_bonds = bd;     }
      else      {  d_bonds->Append(bd); }

  assert(d_bonds->Count() == d_nbonds);
  return 1;
}
int Atom::AddAngle(Angle *bd)
{
  d_nangles++;
  if(d_angles == NULL){d_angles = bd;}
  else  {  d_angles->Append(bd);        }
  return 1;
}
int Atom::AddTorsion(Torsion *bd)
{
  d_ntors++;
  if(d_tors == NULL){  d_tors = bd;     }
  else  {  d_tors->Append(bd);  }
  return 1;
}
int Atom::AddImproper(ImproperTorsion *bd)
{
  d_nimps++;
  if(d_imps == NULL){  d_imps = bd;     }
  else  {  d_imps->Append(bd);  }
  return 1;
}
Atom & Atom::operator=(Atom const & at)
{

  if(d_bonds != NULL)      { delete d_bonds; d_bonds = NULL; }
  d_nbonds = at.d_nbonds;
  if(d_nbonds > 0)
  {
    Bond *bd = at.d_bonds;
    while(bd != NULL)
    {
      Bond *b = bd->Copy();
      if(d_bonds == NULL)
      d_bonds = b;
      else
      d_bonds->Append(b);
      bd = bd->Next();
    }
  }

  if(d_angles != NULL)      { delete d_angles; d_angles = NULL;}
  d_nangles = at.d_nangles;
  if(d_nangles > 0)
  {
    Angle *bd = at.d_angles;
    while(bd != NULL)
    {
      Angle *b = bd->Copy();
      if(d_angles == NULL)
      d_angles = b;
      else
      d_angles->Append(b);
      bd = bd->Next();
    }
  }

  if(d_tors != NULL)    { delete d_tors; d_tors = NULL;}
  d_ntors = at.d_ntors;

  if(d_ntors > 0)
  {
    Torsion *bd = at.d_tors;
    while(bd != NULL)
    {
      Torsion *b = bd->Copy();
      if(d_tors == NULL)
      d_tors = b;
      else
      d_tors->Append(b);
      bd = bd->Next();
    }
  }

  if(d_imps != NULL)        { delete d_imps; d_imps = NULL; }
  d_nimps = at.d_nimps;
  if(d_nimps > 0)
  {
    ImproperTorsion *bd = at.d_imps;
    while(bd != NULL)
    {
      ImproperTorsion *b = bd->Copy();
      if(d_imps == NULL)
      d_imps = b;
      else
      d_imps->Append(b);
      bd = bd->Next();
    }
  }

  d_localid = at.d_localid;
  d_globalid = at.d_globalid;
  d_type = at.d_type;
  strcpy(d_nameS,at.d_nameS);
  d_mass = at.d_mass;
  d_charge = at.d_charge;
  d_ljeps = at.d_ljeps;
  d_ljrmin = at.d_ljrmin;
  d_ljeps14 = at.d_ljeps14;
  d_ljrmin14 = at.d_ljrmin14;
  d_ljtype = at.d_ljtype;
  d_lj14type = at.d_lj14type;

  for(int i=0;i<MAXARGS;i++){strcpy(d_args[i],at.d_args[i]);}
  // Leave list linkage in tact.
  //  d_next = d_prev = NULL;
  return *this;
}

int Atom::operator==(Atom const & a)
{

  if(SameType(a))
  return 1;
  else
  return 0;
}



