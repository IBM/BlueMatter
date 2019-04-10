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
 
#include <BlueMatter/syscell.hpp>

#include <stream.h>

SysCell* SysCell::Next(){return (SysCell *) d_next;}
SysCell* SysCell::Prev(){return (SysCell *) d_prev;}
int & SysCell::Type(){return d_type;}
int & SysCell::GlobalId(){return d_globalid;}
int & SysCell::LocalId(){return d_localid;}

SysCell::SysCell()
{
  d_nats = d_nbds = d_nangs = d_ntors = d_nimps = 0;
  d_ats = NULL;
  d_bds = NULL;
  d_angles = NULL;
  d_tors = NULL;
  d_imps = NULL;
  d_atab = NULL;
  d_btab = NULL;
  d_angtab = NULL;
  d_tortab = NULL;
  d_imptab = NULL;
  d_natypes = d_napairs = d_na14pairs = d_nbtypes = d_nangtypes = d_ntortypes = d_nimptypes = 0;
  d_atypes = NULL;
  d_aPairs = NULL;
  d_a14Pairs = NULL;
  d_btypes = NULL;
  d_angtypes = NULL;
  d_tortypes = NULL;
  d_imptypes = NULL;
}
SysCell::~SysCell()
{
  Destroy();
}
void SysCell::Destroy()
{
  int i,n;
  if(d_atab != NULL)
    {
      delete  d_atab;  d_atab = NULL;    d_ats = NULL;
    }
  if(d_btab != NULL)
    {
      delete d_btab;      d_btab = NULL; d_bds = NULL;
    }
  if(d_angtab != NULL)
    {
      delete d_angtab;    d_angtab = NULL; d_angtab = NULL;
    }
  if(d_tortab != NULL)
    {
      delete d_tortab;      d_tortab = NULL; d_tortab = NULL;
    }
  if(d_imptab != NULL)
    {
      delete d_imptab;      d_imptab = NULL; d_imptab = NULL;
    }
  if(d_ats) delete d_ats;       d_ats = NULL;
  if(d_atypes) delete d_atypes;    d_atypes = NULL;
  if(d_aPairs) delete d_aPairs;    d_aPairs = NULL;
  if(d_a14Pairs) delete d_a14Pairs;    d_a14Pairs = NULL;
  if(d_bds) delete d_bds;       d_bds = NULL;
  if (d_btypes) delete d_btypes;    d_btypes = NULL;
  if(d_angles)delete d_angles;    d_angles = NULL;
  if(d_angtypes) delete d_angtypes;  d_angtypes = NULL;
  if(d_tors)delete d_tors;      d_tors = NULL;
  if(d_tortypes)delete d_tortypes;  d_tortypes = NULL;
  if(d_imps)delete d_imps;      d_imps = NULL;
  if(d_imptypes)delete d_imptypes;  d_imptypes = NULL;
}
int SysCell::AddAtom(Atom *at)
{
  List ls;
  // Addition appends to list and then rolls forward.
  if(d_ats != NULL)
    {
      d_ats->Append(at);
    }
  else
    {
      d_ats = at;
    }
  d_nats++;
  return d_nats;
}
int SysCell::AddPair(Atom *at)
{
  List ls;
  // Addition appends to list and then rolls forward.
  if(d_aPairs != NULL)
    {
      d_aPairs->Append(at);
    }
  else
    {
      d_aPairs = at;
    }
  d_napairs++;
  return d_napairs;
}
int SysCell::Add14Pair(Atom *at)
{
  List ls;
  // Addition appends to list and then rolls forward.
  if(d_a14Pairs != NULL)
    {
      d_a14Pairs->Append(at);
    }
  else
    {
      d_a14Pairs = at;
    }
  d_na14pairs++;
  return d_na14pairs;
}

int SysCell::AddBond(Bond *bd)
{
  List ls;
  if(d_bds != NULL)
    {
          d_bds->Append(bd);
    }
  else
    {
      d_bds = bd;
    }
  d_nbds++;
  return d_nbds;
}

int SysCell::AddAngle(Angle *a)
{
  List ls;
  if(d_angles != NULL)
    {
      d_angles->Append(a);
    }
  else
    {
      d_angles = a;
    }
  d_nangs++;
  return d_nangs;
}

int SysCell::AddTorsion(Torsion *a)
{
  List ls;
  if(d_tors != NULL)
    {
      d_tors->Append(a);
    }
  else
    {
      d_tors = a;
    }

  d_ntors++;
  return d_ntors;
}

int SysCell::AddImproperTorsion(ImproperTorsion *a)
{
  List ls;
  if(d_imps != NULL)
    {
      d_imps->Append(a);
    }
  else
    {
      d_imps = a;
    }
  d_nimps++;
  return d_nimps;
}

int SysCell::Derive1_NAts(int n, Atom *a,Atom **lv)
{
  *lv = new Atom[n];
  Atom *lev = *lv;
  lev[0] = *a;
  int i;
  for(i=1;i<n;i++)
    {
      Atom *ls = &lev[i-1];
      lev[i].GlobalId() = -1;
      while(ls != NULL) // add all unique adjacent atoms to lev[i]
        {
          Atom *at = ls;
          Bond *bd = at->Bonds();
          while(bd != NULL)
            {
              Atom *nat;
              if(ls->GlobalId() == bd->A1())
                { nat = d_atab[bd->A2()]->Copy(); }
              else if (ls->GlobalId() == bd->A2())
                { nat = d_atab[bd->A1()]->Copy(); }
              else
                assert(0);
              int j,flg = 1;
              for(j = 0;j <= i;j++)
                {
                  List *cop;
                  if(LocateItem(&lev[j],*nat,&cop))
                    {
                      delete nat;
                      flg = 0;
                      break;
                    }
                }
              if(flg)
                {
                  if(lev[i].GlobalId() == -1)
                    { lev[i] = *nat; }
                  else
                    { lev[i].Append(nat); }
                }
              bd = bd->Next();
            }
          ls = ls->Next();
        }
    }
  return 1;
}

int SysCell::Finalize()
{
  Atom *at;
  List ls;
  // Rewind lists;
  int i,j;
  if(d_ats != NULL)
    {
      if(d_atab != NULL)
        {
          //      for(i=0;i<d_nats;i++){delete d_atab[i]; d_atab[i] = NULL;}
          delete [] d_atab;
        }
      ls.Rewind((List **)&d_ats);
      d_ats->Flatten((List ***)&d_atab);
    }
  if(d_bds != NULL)
    {
      if(d_btab != NULL)
        {
          //      for(i=0;i<d_nbds;i++){delete d_btab[i]; d_btab[i] = NULL;}
          delete [] d_btab;
        }
      ls.Rewind((List **)&d_bds);
      d_bds->Flatten((List ***)&d_btab);
    }
  if(d_angles != NULL)
    {
      if(d_angtab != NULL)
        {
          //      for(i=0;i<d_nangs;i++){delete d_angtab[i]; d_angtab[i] = NULL;}
          delete [] d_angtab;
        }
      ls.Rewind((List **)&d_angles);
      d_angles->Flatten((List ***)&d_angtab);
    }
  if(d_tors != NULL)
    {
      if(d_tortab != NULL)
        {
          //      for(i=0;i<d_ntors;i++){delete d_tortab[i]; d_tortab[i] = NULL;}
          delete [] d_tortab;
        }
      ls.Rewind((List **)&d_tors);
      d_tors->Flatten((List ***)&d_tortab);
    }
  if(d_imps != NULL)
    {
      if(d_imptab != NULL)
        {
          //      for(i=0;i<d_nimps;i++){delete d_imptab[i]; d_imptab[i] = NULL;}
          delete [] d_imptab;
        }
      ls.Rewind((List **)&d_imps);
      d_imps->Flatten((List ***)&d_imptab);
    }
  return 1;
}
int SysCell::DeriveNeighborhoodLists()
{
    Atom *at;
  List ls;
  // Rewind lists;
  int i,j;

  // Derive atom's connection list
  for(i=0;i<d_nbds;i++)
    {
      int s;
      Bond *bd = d_btab[i]->Copy();
      Atom *at1 = d_atab[bd->A1()];
      at1->AddBond(bd);
      bd = bd->Copy();
      Atom *at2 = d_atab[bd->A2()];
      at2->AddBond(bd);
    }

  // Derive atoms's angle list
  for(i=0;i<d_nangs;i++)
    {
      Angle *a = d_angtab[i]->Copy();
      Atom *at1 = d_atab[a->A1()];
      at1->AddAngle(a);
      a = a->Copy();
      Atom *at2 = d_atab[a->A2()];
      at2->AddAngle(a);
      a = a->Copy();
      Atom *at3 = d_atab[a->A3()];
      at3->AddAngle(a);
    }
  // Derive atoms's Torsion list
  for(i=0;i<d_ntors;i++)
    {
      Torsion *a = d_tortab[i];
      a = a->Copy();
      Atom *at1 = d_atab[a->A1()];
      at1->AddTorsion(a);
      a = a->Copy();
      Atom *at2 = d_atab[a->A2()];
      at2->AddTorsion(a);
      a = a->Copy();
      Atom *at3 = d_atab[a->A3()];
      at3->AddTorsion(a);
      a = a->Copy();
      Atom *at4 = d_atab[a->A4()];
      at4->AddTorsion(a);
    }
  // Derive atom's Improper list
  for(i=0;i<d_nimps;i++)
    {
      ImproperTorsion *a = d_imptab[i];
      a = a->Copy();
      Atom *at1 = d_atab[a->A1()];
      at1->AddImproper(a);
      a = a->Copy();
      Atom *at2 = d_atab[a->A2()];
      at2->AddImproper(a);
      a = a->Copy();
      Atom *at3 = d_atab[a->A3()];
      at3->AddImproper(a);
      a = a->Copy();
      Atom *at4 = d_atab[a->A4()];
      at4->AddImproper(a);
    }

//    // Synch ats with atab
//    Atom *tat = d_ats;
//    for(i=0;i<d_nats;i++)
//      {
//        assert(tat != NULL);
//        Atom *at = d_atab[i];
//        *tat= *at;
//        tat = tat->Next();
//      }

  return 1;
}

int SysCell::ReduceAtomTypes()
{
  int i,j;
  Atom * a = d_ats;

  // ensure local Ids are initialized
  i = 0;
  while(a != NULL)
    {
      d_atab[ i ]->LocalId() = a->LocalId() = i;
      i++;
      a = a->Next();
    }
  a = d_ats;
  d_atypes = d_ats->Copy(); // initialize with the first atom
  d_natypes++;
  a = a->Next();
  i=1;
  while(a != NULL)  // for every atom in  ats
    {
      Atom *ls = d_atypes;
      while(ls != NULL)        //  check if its a new type
        {
          if(a->SameType(*ls))
            {  break;   }
          ls = ls->Next();
        }
      if(ls == NULL)          // found new type
        {
          Atom *at = a->Copy();
          d_atypes->Append(at);
          at->LocalId() = a->LocalId() = d_natypes;  //assign local Id in ats to atype index
          d_natypes++;
        }
      else
        {
          a->LocalId() = ls->LocalId();  // copy atype index to ats->localId
        }
      a = a->Next();
      i++;
    }
  return d_natypes;
}
int SysCell::AtypeNum(char *tag)
{
  Atom *at = d_ats;
  while(at != NULL)
    {
      if(!strcmp(tag,at->Name()))
        {
          return at->Type();
        }
      at = at->Next();
    }
  return 0;
}
int SysCell::AtypeName(int typ,char *name)
{
  Atom *at = d_ats;
  while(at != NULL)
    {
      if(typ == at->Type())
        {
          strcpy(name,at->Name());
          return 1;
        }
      at = at->Next();
    }
  return 0;
}


int SysCell::LocateAtomName(char * name, Atom **at)
{
  Atom *a = d_ats;
  while(a != NULL)
    {
      if(!strcmp(name,a->Name()))
        {
          *at = a;
          return 1;
        }
      a = a->Next();
    }
  return 0;
}

int  SysCell::operator==(const SysCell &f)
{
  List *t,*tt;
  t = d_ats;  tt = f.d_ats;
  if(!ListEqv(t,tt))
     return 0;
  t = d_bds;  tt = f.d_bds;
  if(!ListEqv(t,tt))
     return 0;
  t = d_angles;  tt = f.d_angles;
  if(!ListEqv(t,tt))
     return 0;
  t = d_tors;  tt = f.d_tors;
  if(!ListEqv(t,tt))
     return 0;
  t = d_imps;  tt = f.d_imps;
  if(!ListEqv(t,tt))
     return 0;
  return 1;
}


int SysCell::ListEqv(List *ll,List *rr)
{
  List *l = ll;
  List *r = rr;
  while(l != NULL)
  {
    if(!(*l == *r))
    return 0;
    l = l->Next();
    r = r->Next();
  }
  if(r != NULL)
    return 0;
  else
    return 1;
}

SysCell & SysCell::operator=(SysCell const & s)
{
  Destroy();
  d_nats = s.d_nats;
  d_nbds = s.d_nbds;
  d_nangs = s.d_nangs;
  d_ntors = s.d_ntors;
  d_nimps = s.d_nimps;
  d_ats = (s.d_ats == NULL) ? NULL : s.d_ats->CopyList();
  d_bds = (s.d_bds == NULL) ? NULL : s.d_bds->CopyList();
  d_angles = (s.d_angles == NULL) ? NULL : s.d_angles->CopyList();
  d_tors = (s.d_tors == NULL) ? NULL : s.d_tors->CopyList();
  d_imps = (s.d_imps == NULL) ? NULL : s.d_imps->CopyList();
  if(d_ats != NULL)  d_ats->Flatten((List ***)&d_atab);
  if(d_btab != NULL) d_bds->Flatten((List ***)&d_btab);
  if(d_angles != NULL) d_angles->Flatten((List ***)&d_angtab);
  if(d_tors != NULL) d_tors->Flatten((List ***)&d_tortab);
  if(d_imps != NULL) d_imps->Flatten((List ***)&d_imptab);
  d_natypes = s.d_natypes;
  d_napairs = s.d_napairs;
  d_nbtypes = s.d_nbtypes;
  d_natypes = s.d_natypes;
  d_ntortypes = s.d_ntortypes;
  d_nimptypes = s.d_nimptypes;
  d_atypes = (s.d_atypes == NULL) ? NULL : s.d_atypes->CopyList();
  d_aPairs = (s.d_aPairs == NULL) ? NULL : s.d_aPairs->CopyList();
  d_a14Pairs = (s.d_a14Pairs == NULL) ? NULL : s.d_a14Pairs->CopyList();
  d_btypes = (s.d_btypes == NULL) ? NULL : s.d_btypes->CopyList();
  d_angtypes = (s.d_angtypes == NULL) ? NULL : s.d_angtypes->CopyList();
  d_tortypes = (s.d_tortypes == NULL) ? NULL : s.d_tortypes->CopyList();
  d_imptypes = (s.d_imptypes == NULL) ? NULL : s.d_imptypes->CopyList();
  return *this;
}

SysCell * SysCell::Copy()
{
  SysCell *sys = new SysCell();
  *sys = *this;
  return sys;
}
SysCell * SysCell::CopyList()
{
  SysCell *s = this;
  SysCell *ss = NULL;
  while(s != NULL)
    {
      SysCell *sss = s->Copy();
      if(ss == NULL)
        ss = sss;
      else
        ss->Append(sss);
      s = s->Next();
    }
  return ss;
}
int SysCell::CheckIfOnTorsionList(int a1, int a4)
{
  //  cout << "Checking Torsion list for " << a1 << " and " << a4 << endl;

  Torsion *t = Tors();
  while(t != NULL)
    {
      if((a1 == t->A1() && a4 == t->A4()) || (a1 == t->A4() && a4 == t->A1()))
        {
          //      cout << "Found Torsion " << t->GlobalId()
          //           << " " << t->A1()
          //   << " " << t->A2()
          //   << " " << t->A3()
          //   << " " << t->A4() << endl;
          return 1;
        }
      t = t->Next();
    }
  return 0;
}


