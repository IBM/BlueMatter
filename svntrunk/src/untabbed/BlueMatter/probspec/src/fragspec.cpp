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
 

#include <BlueMatter/fragspec.hpp>

#include <stream.h>
#include <BlueMatter/UDF_RegistryIF.hpp>

extern int IsXmlPath;

FragSpec::FragSpec()
{
  name[0]='\0';
  NumberInternalSites = 0;
  NumberExternalSites = 0;
  InternalSites = NULL;
  ExternalSites = NULL;
  NumberBondTypes = 0;
  BondTypes = NULL;
  NumberInternalBonds = 0;
  NumberExternalBonds = 0;
  InternalBonds = NULL;
  ExternalBonds = NULL;
  NumberAngleTypes = 0;
  AngleTypes = NULL;
  NumberInternalAngles = 0;
  NumberExternalAngles = 0;
  InternalAngles = NULL;
  ExternalAngles = NULL;
  NumberTorsionTypes = 0;
  TorsionTypes = NULL;
  NumberInternalTorsions = 0;
  NumberExternalTorsions = 0;
  InternalTorsions = NULL;
  ExternalTorsions = NULL;
  NumberImproperTypes = 0;
  ImproperTypes = NULL;
  NumberInternalImpropers = 0;
  NumberExternalImpropers = 0;
  InternalImpropers = NULL;
  ExternalImpropers = NULL;
  NumberExcludedSitePairs = 0;
  ExcludedSitePairs = NULL;
  NumberPair14SitePairs = 0;
  Pair14SitePairs = NULL;
  NumberTerms = 0;
  TermList = NULL;
  NumberFragmentNeighbors = 0;
  FragmentNeighborList = NULL;


  d_bondTermName = NULL;  d_bondStructName = NULL;
  d_angleTermName = NULL; d_angleStructName = NULL;
  d_ureyTermName = NULL;  d_ureyStructName = NULL;
  d_torsionTermName = NULL;  d_torsionStructName = NULL;
  d_improperTermName = NULL;  d_improperStructName = NULL;
  
  d_bondCode = 0;
  d_angleCode = 0;
  d_ureyCode = 0;
  d_torsionCode = 0;
  d_improperCode = 0;
  d_exPairCode = 0;
  d_pair14Code = 0;
}
FragSpec::~FragSpec()
{
  d_next = d_prev = NULL;
  NumberTerms = 0;
  name[0] = '\0';
  NumberInternalSites = 0;
  NumberExternalSites = 0;
  if(InternalSites != NULL){delete InternalSites;  InternalSites = NULL;}
  if(ExternalSites != NULL){delete ExternalSites;  ExternalSites = NULL;}
  NumberBondTypes = 0;
  if(BondTypes != NULL){delete BondTypes; BondTypes = NULL;}
  NumberInternalBonds = 0;
  NumberExternalBonds = 0;
  if(InternalBonds != NULL){delete InternalBonds; InternalBonds = NULL;}
  if(ExternalBonds != NULL){delete ExternalBonds; ExternalBonds = NULL;}
  NumberAngleTypes = 0;
  if(AngleTypes !=NULL){delete AngleTypes; AngleTypes = NULL;}
  NumberInternalAngles = 0;
  NumberExternalAngles = 0;
  if(InternalAngles != NULL){delete InternalAngles;InternalAngles = NULL;}
  if(ExternalAngles != NULL){delete ExternalAngles;ExternalAngles = NULL;}
  NumberTorsionTypes = 0;
  if(TorsionTypes != NULL){delete TorsionTypes; TorsionTypes = NULL;}
  NumberInternalTorsions = 0;
  NumberExternalTorsions = 0;
  if(InternalTorsions != NULL){delete InternalTorsions; InternalTorsions = NULL;}
  if(ExternalTorsions != NULL){delete ExternalTorsions; ExternalTorsions = NULL;}
  NumberImproperTypes = 0;
  if(ImproperTypes != NULL){delete ImproperTypes;ImproperTypes = NULL;}
  NumberInternalImpropers = 0;
  NumberExternalImpropers = 0;
  if(InternalImpropers != NULL)
    {delete InternalImpropers;InternalImpropers = NULL;}
  if(ExternalImpropers != NULL)
    {delete ExternalImpropers; ExternalImpropers = NULL;}
  NumberExcludedSitePairs = 0;
  if(ExcludedSitePairs != NULL)
    {delete ExcludedSitePairs; ExcludedSitePairs = NULL;}
  NumberPair14SitePairs = 0;
  if(Pair14SitePairs != NULL)
    {delete Pair14SitePairs; Pair14SitePairs = NULL;}
  NumberTerms = 0;
  if(TermList != NULL){delete TermList; TermList = NULL;}
  NumberFragmentNeighbors = 0;
  FragmentNeighborList = NULL;

  if(d_bondTermName != NULL) delete   d_bondTermName; 
  if(d_bondStructName != NULL) delete d_bondStructName; 
  if(d_angleTermName != NULL) delete d_angleTermName;  
  if(d_angleStructName != NULL) delete d_angleStructName; 
  if(d_ureyTermName != NULL) delete   d_ureyTermName; 
  if(d_ureyStructName != NULL) delete d_ureyStructName; 
  if(d_torsionTermName != NULL) delete   d_torsionTermName; 
  if(d_torsionStructName != NULL) delete d_torsionStructName; 
  if(d_improperTermName != NULL) delete   d_improperTermName; 
  if(d_improperStructName != NULL) delete d_improperStructName; 


}
int FragSpec::AddTerm(TermSpec *trm)
{
 TermSpec *t = trm->Copy();
 if( TermList== NULL)
 {
    TermList = t;
 }
 else
 {
    TermList->Append(t);
 }
 NumberTerms++;
 return NumberTerms;
}
int FragSpec::AddInternalSite(Atom *a)
{
  if(InternalSites != NULL && a->InList(InternalSites))
    return 0;
  Atom *at = a->Copy();
  if(InternalSites == NULL)
    InternalSites = at;
  else
    InternalSites->Append(at);
  at->LocalId() = a->LocalId();
  NumberInternalSites++;
  return NumberInternalSites;
}
int FragSpec::AddExternalSite(Atom *a)
{
  if(ExternalSites != NULL && a->InList(ExternalSites))
    return 0;
  Atom *at = a->Copy();
  if(ExternalSites == NULL)
    ExternalSites = at;
  else
    ExternalSites->Append(at);
  at->LocalId() = a->LocalId();
  NumberExternalSites++;
  return NumberExternalSites;
}

int FragSpec::AddInternalBond(Bond *b)
{  if(InternalBonds != NULL && b->InList(InternalBonds))
    return 0;
  Bond *bd = b->Copy();
  if(InternalBonds == NULL)
    InternalBonds = bd;
  else
    InternalBonds->Append(bd);
  bd->LocalId() = b->LocalId();
  NumberInternalBonds++;
   return NumberInternalBonds;
}

int FragSpec::AddExternalBond(Bond *b)
{
  if(ExternalBonds != NULL && b->InList(ExternalBonds))
    return 0;
  Bond *bd = b->Copy();
  if(ExternalBonds == NULL)
    ExternalBonds = bd;
  else
    ExternalBonds->Append(bd);
  bd->LocalId() = b->LocalId();
  NumberExternalBonds++;
  return NumberExternalBonds;
}

int FragSpec::AddInternalAngle(Angle *a)
{
  if(InternalAngles != NULL && a->InList(InternalAngles))
    return 0;
  Angle *ag = a->Copy();
  if(InternalAngles == NULL)
    InternalAngles = ag;
  else
    InternalAngles->Append(ag);
  ag->LocalId() = a->LocalId();
  NumberInternalAngles++;
  return NumberExternalAngles;
}

int FragSpec::AddExternalAngle(Angle *a)
{
  if(ExternalAngles != NULL && a->InList(ExternalAngles))
    return 0;
  Angle *ag = a->Copy();
  if(ExternalAngles == NULL)
    ExternalAngles = ag;
  else
    ExternalAngles->Append(ag);
  ag->LocalId() = a->LocalId();
  NumberExternalAngles++;
  return NumberExternalAngles;
}

int FragSpec::AddInternalTorsion(Torsion *t)
{
  if(InternalTorsions != NULL && t->InList(InternalTorsions))
    return 0;
  Torsion  *tr = t->Copy();
  if(InternalTorsions == NULL)
    InternalTorsions = tr;
  else
    InternalTorsions->Append(tr);
  tr->LocalId() = t->LocalId();
  NumberInternalTorsions++;
  return NumberInternalTorsions;
}

int FragSpec::AddExternalTorsion(Torsion *t)
{
  if(ExternalTorsions != NULL && t->InList(ExternalTorsions))
    return 0;
  Torsion *tr = t->Copy();
  if(ExternalTorsions == NULL)
    ExternalTorsions = tr;
  else
    ExternalTorsions->Append(tr);
  tr->LocalId() = t->LocalId();
  NumberExternalTorsions++;
  return NumberExternalTorsions;
}

int FragSpec::AddInternalImproper(ImproperTorsion *it)
{
  if(InternalImpropers != NULL && it->InList(InternalImpropers))
    return 0;
  ImproperTorsion  *itr = it->Copy();
  if(InternalImpropers == NULL)
    InternalImpropers = itr;
  else
    InternalImpropers->Append(itr);
  itr->LocalId() = it->LocalId();
  NumberInternalImpropers++;
  return NumberInternalImpropers;
}

int FragSpec::AddExternalImproper(ImproperTorsion * it)
{
  if(ExternalImpropers != NULL && it->InList(ExternalImpropers))
    return 0;
  ImproperTorsion *itr = it->Copy();
  if(ExternalImpropers == NULL)
    ExternalImpropers = itr;
  else
    ExternalImpropers->Append(itr);
  itr->LocalId() = it->LocalId();
  NumberExternalImpropers++;
  return NumberExternalImpropers;
}

int FragSpec::AddBondTerm(Bond *b)
{
  TermSpec t;
  t.SetBondParams(b,BondCode());
  UpdateExternalTerm(&t);
  AddTerm(&t);
  return 1;
}

int FragSpec::AddAngleTerm(Angle *a)
{
  TermSpec t;
  t.SetAngleParams(a,AngleCode());
  UpdateExternalTerm(&t);
  AddTerm(&t);
  t.SetUreyBradleyParams(a,UreyCode());
  UpdateExternalTerm(&t);
  AddTerm(&t);
  return 1;
}

int FragSpec::AddTorsionTerm(Torsion *b)
{
  TermSpec t;
  //  if(HeaderHandler::instance()->getTorsionInput() == FF_OPLSAA)
  //    t.SetOplsaTorsionParams(b);
  //  else
  t.SetTorsionParams(b,TorsionCode());

  UpdateExternalTerm(&t);
  AddTerm(&t);
  return 1;
}
int FragSpec::AddImproperTerm(ImproperTorsion *a)
{
  TermSpec t;
  //  if(HeaderHandler::instance()->getTorsionInput() == FF_OPLSAA)
  //    t.SetOplsaImproperParams(a);
  //  else
    t.SetImproperParams(a,ImproperCode());
  UpdateExternalTerm(&t);
  AddTerm(&t);
  return 1;
}

int FragSpec::UpdateExternalTerm(TermSpec *t)
{
  Atom a;
  int off;
  a.GlobalId() = t->mA1;
  if(!IsInternalAtom(&a,off))
    {
      if(!IsExternalAtom(&a,off))
        AddExternalSite(&a);
      t->sa1 = 1;
    }
  t->mA1 = off;

  a.GlobalId() = t->mA2;
  if(!IsInternalAtom(&a,off))
    {
      if(!IsExternalAtom(&a,off))
        AddExternalSite(&a);
      t->sa2 = 1;
    }
  t->mA2 = off;

  if(t->mA3 >=0)
    {
      a.GlobalId() = t->mA3;
      if(!IsInternalAtom(&a,off))
        {
          if(!IsExternalAtom(&a,off))
            AddExternalSite(&a);
          t->sa3 = 1;
        }
      t->mA3 = off;
    }
  if(t->mA4 >=0)
    {
      a.GlobalId() = t->mA4;
      if(!IsInternalAtom(&a,off))
        {
          if(!IsExternalAtom(&a,off))
            AddExternalSite(&a);
          t->sa4 = 1;
        }
      t->mA4 = off;
    }
  return 1;
}
void FragSpec::Finalize(Atom **atab)
{
  DeriveExcludedAnd14Lists(atab);
}

void FragSpec::DeriveExcludedAnd14Lists(Atom **atab)
{
  Atom *contree,*at;
  if(ExcludedSitePairs != NULL)
    delete ExcludedSitePairs;
  at = InternalSites;
  while(at != NULL)
    {
      Derive1_NAts(4,at,atab,&contree);
      UpdateExcludedPairs(contree);
      Atom * tat = &contree[3];
      while(tat != NULL)
        {
          if(tat->GlobalId() > 0)
            {
              int id1 = at->GlobalId();
              int id2 = tat->GlobalId();
              if(!CheckIfOnTorsionList(id1,id2))
              {
                tat->GlobalId() = - tat->GlobalId();
              }
            }
          tat = tat->Next();
        }
      Update14Pairs(contree);
      delete[] contree;
      at = at->Next();
    }
}

int FragSpec::Derive1_NAts(int n, Atom *a,Atom **atab,Atom **lv)
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
                { nat = atab[bd->A2()]->Copy(); }
              else
                { nat = atab[bd->A1()]->Copy(); }
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
                    { lev[i] = *nat;  delete nat;}
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


int FragSpec::IsInternalAtom(Atom *at,int &off)
{
  int i=0;
  Atom  *a = InternalSites;
  while (a != NULL)
    {
      if(a->GlobalId() == at->GlobalId())
        {  off = i; return 1;   }
      a = a->Next();
      i++;
    }
  return 0;
}
int FragSpec::IsExternalAtom(Atom *at,int &off)
{
  int i=0;
  Atom  *a = ExternalSites;
  while (a != NULL)
    {
      if(a->GlobalId() == at->GlobalId())
        {  off = i; return 1;   }
      a = a->Next();
      i++;
    }
  return 0;
}

int FragSpec::UpdateExcludedPairs(Atom *contree)
{
  int off;
  int gid = contree[0].GlobalId();
  ExPair x;
  Atom *aa = &contree[0];
  int off1;
  assert(IsInternalAtom(aa,off));
  x.A1() = off;

  for(int i=1;i<3;i++)
    {
      aa = &contree[i];
      if(aa->GlobalId() == -1)
        continue;
      while(aa != NULL)
        {
          if(aa->GlobalId() > gid)
            {
              ExPair *xx = x.Copy();
              TermSpec *t = new TermSpec();
              t->SetExPairParams(xx,ExPairCode());
              if(IsInternalAtom(aa,off))
                {
                  xx->A2() = off;
                  t->sa1 = 0;
                }
              else if(IsExternalAtom(aa,off))
                {
                  t->sa2 = 1;
                  xx->A2() = - (off + 1);
                }
              else
                assert(0);

              t->mA2 = off;


//                if(TermList == NULL)
//                    TermList = t;
//                else
//                    TermList->Append(t);

        //             NumberTerms++;
             if(ExcludedSitePairs == NULL)
                ExcludedSitePairs = xx;
              else
                ExcludedSitePairs->Append(xx);

              NumberExcludedSitePairs++;
            }
          aa = aa->Next();
        }
    }
  return 1;
}
int  FragSpec::Update14Pairs(Atom *contree)
{
  int off;
  Pair14 x;
  Atom *aa = &contree[0];
  int off1;
  assert(IsInternalAtom(aa,off));
  x.A1() = off;
  int gid = aa->GlobalId();
  aa = &contree[3];

  if(aa->GlobalId() == -1)
    return 0;
  while(aa != NULL)
    {
      if(aa->GlobalId() > gid)
        {
          Pair14 *xx = x.Copy();
          TermSpec *t = new TermSpec();
          t->SetPair14Params(xx,Pair14Code());

          if(IsInternalAtom(aa,off))
            {
              xx->A2() = off;
              t->sa2 = 0;
            }
          else if(IsExternalAtom(aa,off))
            {
              t->sa2 = 1;
              xx->A2() = - (off + 1);
            }
          else
            assert(0);
          t->mA2 = off;

//            if(TermList == NULL)
//              TermList = t;
//            else
//              TermList->Append(t);
//            NumberTerms++;
          if(Pair14SitePairs == NULL)
            Pair14SitePairs = xx;
          else
            Pair14SitePairs->Append(xx);
          NumberPair14SitePairs++;
        }
      aa = aa->Next();
    }
  return 1;
}

int FragSpec::CheckIfOnTorsionList(int a1, int a4)
{
  //  cout << "Checking Torsion list for " << a1 << " and " << a4 << endl;

  Torsion *t = InternalTorsions;
  while(t != NULL)
    {
      if((a1 == t->A1() && a4 == t->A4()) || (a1 == t->A4() && a4 == t->A1()))
        {
          //      cout << "found internal torsion " << t->GlobalId()
          //     << " " << t->A1()
          //    << " " << t->A2()
          //    << " " << t->A3()
          //    << " " << t->A4() << endl;
          return 1;
        }
      t = t->Next();
    }
  t = ExternalTorsions;
  while(t != NULL)
    {
      if((a1 == t->A1() && a4 == t->A4()) || (a1 == t->A4() && a4 == t->A1()))
        {
          //      cout << "found internal torsion " << t->GlobalId()
          //    << " " << t->A1()
          //   << " " << t->A2()
          //   << " " << t->A3()
          //   << " " << t->A4() << endl;
          return 1;
        }
      t = t->Next();
    }
  return 0;
}



int FragSpec::IsNew(int n,List *ls,int id,int &pos)
{
  List *l = ls;
  int k = 0;
  while(l != NULL)
    {
      if(l->GlobalId() == id)
        {
          pos = k;
          return 0;
        }
      l = l->Next();
      k++;
      if(k >= n)
        break;
    }
  pos = n;
  return 1;
}
int  FragSpec::operator==(const FragSpec &f)
{
  Atom * ats, *tats;

  ats = InternalSites;
  tats = f.InternalSites;
  while(ats != NULL)
  {
    if(tats == NULL)
    {return 0;}
    if(!ats->SameType(*tats))
    {   return 0; }
    ats = ats->Next();
    tats = ats->Next();
  }
  if(tats != NULL)
  {  return 0; }

  return 1;
}
FragSpec & FragSpec::operator=(const FragSpec &f)
{
  strcpy(name,f.name);

  NumberInternalSites = f.NumberInternalSites;
  if(InternalSites != NULL)delete InternalSites;
  InternalSites = (f.InternalSites == NULL) ? NULL : f.InternalSites->CopyList();

  NumberExternalSites = f.NumberExternalSites;
  if(ExternalSites != NULL)delete ExternalSites;
  ExternalSites = (f.ExternalSites == NULL) ? NULL : f.ExternalSites->CopyList();

  NumberBondTypes = f.NumberBondTypes;
  if(BondTypes != NULL)delete BondTypes;
  BondTypes = (f.BondTypes == NULL) ? NULL : f.BondTypes->CopyList();

  NumberInternalBonds = f.NumberInternalBonds;
  if(InternalBonds != NULL)delete InternalBonds;
  InternalBonds = (f.InternalBonds == NULL) ? NULL : f.InternalBonds->CopyList();

  NumberExternalBonds = f.NumberExternalBonds;
  if(ExternalBonds != NULL)delete ExternalBonds;
  ExternalBonds = (f.ExternalBonds == NULL) ? NULL : f.ExternalBonds->CopyList();

  NumberAngleTypes = f.NumberAngleTypes;
  if(AngleTypes != NULL)delete AngleTypes;
  AngleTypes = (f.AngleTypes == NULL) ? NULL : f.AngleTypes->CopyList();

  NumberInternalAngles = f.NumberInternalAngles;
  if(InternalAngles != NULL)delete InternalAngles;
  InternalAngles = (f.InternalAngles == NULL) ? NULL : f.InternalAngles->CopyList();
  NumberExternalAngles = f.NumberExternalAngles;
  if(ExternalAngles != NULL)delete ExternalAngles;
  ExternalAngles = (f.ExternalAngles == NULL) ? NULL : f.ExternalAngles->CopyList();

  NumberTorsionTypes = f.NumberTorsionTypes;

  TorsionTypes = NULL;
  if(TorsionTypes != NULL)delete TorsionTypes;
  TorsionTypes = (f.TorsionTypes == NULL) ? NULL : f.TorsionTypes->CopyList();

  NumberInternalTorsions = f.NumberInternalTorsions;
  if(InternalTorsions != NULL)delete InternalTorsions;
  InternalTorsions = (f.InternalTorsions == NULL) ? NULL : f.InternalTorsions->CopyList();

  NumberExternalTorsions = f.NumberExternalTorsions;
  if(ExternalTorsions != NULL)delete ExternalTorsions;
  ExternalTorsions = (f.ExternalTorsions == NULL) ? NULL : f.ExternalTorsions->CopyList();

  NumberImproperTypes = f.NumberImproperTypes;
  if(ImproperTypes != NULL)delete ImproperTypes;
  ImproperTypes = (f.ImproperTypes == NULL) ? NULL : f.ImproperTypes->CopyList();

  NumberInternalImpropers = f.NumberInternalImpropers;
  if(InternalImpropers != NULL)delete InternalImpropers;
  InternalImpropers = (f.InternalImpropers == NULL) ? NULL : f.InternalImpropers->CopyList();

  NumberExternalImpropers = f.NumberExternalImpropers;
  if(ExternalImpropers != NULL)delete ExternalImpropers;
  ExternalImpropers = (f.ExternalImpropers == NULL) ? NULL : f.ExternalImpropers->CopyList();

  NumberExcludedSitePairs = f.NumberExcludedSitePairs;
  if(ExcludedSitePairs != NULL)delete ExcludedSitePairs;
  ExcludedSitePairs = (f.ExcludedSitePairs == NULL) ? NULL : f.ExcludedSitePairs->CopyList();

  NumberPair14SitePairs = f.NumberPair14SitePairs;
  if(Pair14SitePairs != NULL)delete Pair14SitePairs;
  Pair14SitePairs = (f.Pair14SitePairs == NULL) ? NULL : f.Pair14SitePairs->CopyList();
  NumberTerms = f.NumberTerms;
  if(TermList != NULL)delete TermList;
  TermList = (f.TermList == NULL) ? NULL : f.TermList->CopyList();

  NumberFragmentNeighbors = f.NumberFragmentNeighbors;
  FragmentNeighborList = f.FragmentNeighborList;

  SetBondTermName(f.BondTermName());
  SetAngleTermName(f.AngleTermName());
  SetUreyTermName(f.UreyTermName()) ;
  SetTorsionTermName(f.TorsionTermName());
  SetImproperTermName(f.ImproperTermName());

  SetBondStructName(f.BondStructName());
  SetAngleStructName(f.AngleStructName());
  SetUreyStructName(f.UreyStructName()) ;
  SetTorsionStructName(f.TorsionStructName());
  SetImproperStructName(f.ImproperStructName());

  SetBondCode(f.BondCode());
  SetAngleCode(f.AngleCode());
  SetUreyCode(f.UreyCode()) ;
  SetTorsionCode(f.TorsionCode());
  SetImproperCode(f.ImproperCode());
  SetExPairCode(f.ExPairCode());
  SetPair14Code(f.Pair14Code());
  return *this;
}

FragSpec * FragSpec::Copy()
{
  FragSpec *fs = new FragSpec();
  *fs = *this;
  return fs;
}
FragSpec * FragSpec::CopyList()
{
  FragSpec *tfs = this;
  FragSpec *fs = NULL;
  while(tfs != NULL)
    {
      FragSpec *ffs = tfs->Copy();
      if(fs == NULL)
        fs = ffs;
      else
        fs->Append(ffs);
    }
  return fs;
}

#include <BlueMatter/fragspec.hpp>


int FragSpec::PrintHppInfo(char *fPF)
{
  char *s = fPF;
  PrintTermCounts(s); s += strlen(s);
  PrintSiteRefList(s); s += strlen(s);
  PrintBondList(s); s += strlen(s);
  PrintAngleList(s); s += strlen(s);
  PrintTorsionList(s); s += strlen(s);
  PrintImproperList(s); s += strlen(s);
  PrintTermTypes(s); s += strlen(s);
  PrintExcludeSitePairsList(s); s += strlen(s);
  Print14PairsList(s); s += strlen(s);
  PrintCodeList(s); s += strlen(s);
  PrintFragmentNeighborhood(s); s += strlen(s);
  return s - fPF;
}
int FragSpec::PrintTermCounts(char *fPF)
{
  char bufS[1024 + 256],tbufS[256];
  sprintf(name,"FragSpec_%d",LocalId());
  bufS[0] = '\0';
  CATSTR("class ");
  CATSTR(name);

  OPEN_CLASS(bufS);
  STRNG("public:                "); LINE;
  STRNG("static const int           NumberInternalSites; "); LINE;
  STRNG("static const int           NumberExternalSites; "); LINE;
  STRNG("static const int           InternalSiteSpecTableIndex[]; "); LINE;
  STRNG("static const int           ExternalSiteSpecTableIndex[]; "); LINE;

  STRNG("static const int           NumberBondTypes; "); LINE;
  sprintf(bufS,"static const %s  %s_Table[];  \n",BondTermName(),BondTermName());  strcat(fPF,bufS);
  STRNG("static const int           NumberInternalBonds;  "); LINE;
  STRNG("static const int           NumberExternalBonds;  "); LINE;
  STRNG("static const Bond          InternalBonds[];  "); LINE;
  STRNG("static const Bond          ExternalBonds[];  "); LINE;

  STRNG("static const int           NumberAngleTypes;  "); LINE;
  sprintf(bufS,"static const %s  %s_Table[];  \n",AngleTermName(),AngleTermName());  strcat(fPF,bufS);
  sprintf(bufS,"static const %s  %s_Table[];  \n",UreyTermName(),UreyTermName());  strcat(fPF,bufS);
  STRNG("static const int           NumberInternalAngles;   "); LINE;
  STRNG("static const int           NumberExternalAngles;   "); LINE;
  STRNG("static const Angle         InternalAngles[]; "); LINE;
  STRNG("static const Angle         ExternalAngles[]; "); LINE;

  STRNG("static const int           NumberTorsionTypes;   "); LINE;
  sprintf(bufS,"static const %s %s_Table[];  \n",TorsionTermName(),TorsionTermName());  strcat(fPF,bufS);
  STRNG("static const int           NumberInternalTorsions;   "); LINE;
  STRNG("static const int           NumberExternalTorsions;   "); LINE;
  STRNG("static const Torsion       InternalTorsions[];"); LINE;
  STRNG("static const Torsion       ExternalTorsions[];"); LINE;

  STRNG("static const int           NumberImproperTypes;   "); LINE;
  sprintf(bufS,"static const %s %s_Table[];  \n",ImproperTermName(),ImproperTermName());  strcat(fPF,bufS);
  STRNG("static const int           NumberInternalImpropers;   "); LINE;
  STRNG("static const int           NumberExternalImpropers;   "); LINE;
  STRNG("static const Improper       InternalImpropers[];"); LINE;
  STRNG("static const Improper       ExternalImpropers[];"); LINE;
  STRNG("static const int NumberExcludedSitePairs;"); LINE;
  STRNG("static const SiteId  ExcludedSitePairs[];"); LINE;
  STRNG("static const int NumberPair14SitePairs;"); LINE;
  STRNG("static const SiteId  Pair14SitePairs[];"); LINE;
  STRNG("static const int  NumberTerms;"); LINE;
  STRNG("static const Term  TermList[];"); LINE;
  STRNG("static const int  NumberCodes;"); LINE;
  STRNG("static const Code  CodeList[];"); LINE;
  STRNG("static const int NumberFragmentNeighbors;"); LINE;
  STRNG("static const int FragmentNeighborList[];"); LINE;


  END_BLOCK;

  INIT_COUNT(name,"NumberInternalSites",NumberInternalSites);
  INIT_COUNT(name,"NumberExternalSites",NumberExternalSites);
  INIT_COUNT(name,"NumberBondTypes",NumberBondTypes);
  INIT_COUNT(name,"NumberInternalBonds",NumberInternalBonds);
  INIT_COUNT(name,"NumberExternalBonds",NumberExternalBonds);
  INIT_COUNT(name,"NumberAngleTypes",NumberAngleTypes);
  INIT_COUNT(name,"NumberInternalAngles",NumberInternalAngles);
  INIT_COUNT(name,"NumberExternalAngles",NumberExternalAngles);
  INIT_COUNT(name,"NumberTorsionTypes",NumberTorsionTypes);
  INIT_COUNT(name,"NumberInternalTorsions",NumberInternalTorsions);
  INIT_COUNT(name,"NumberExternalTorsions",NumberExternalTorsions);
  INIT_COUNT(name,"NumberImproperTypes",NumberImproperTypes);
  INIT_COUNT(name,"NumberInternalImpropers",NumberInternalImpropers);
  INIT_COUNT(name,"NumberExternalImpropers",NumberExternalImpropers);
  INIT_COUNT(name,"NumberExcludedSitePairs",NumberExcludedSitePairs*2);
  INIT_COUNT(name,"NumberPair14SitePairs",NumberPair14SitePairs*2);

  return 1;
}
  /////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////
  //             Site ref list
  /////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////
int FragSpec::PrintSiteRefList(char *fPF)
{
  char bufS[256],tbufS[256];
  sprintf(bufS,"const int %s::InternalSiteSpecTableIndex[]",name);
  OPEN_BLOCK(bufS);
  List *ls;
  ls = InternalSites;
  while(ls != NULL)
    {
      //SITETYPESPEC(ATypeMap[InternalSites[i]]);
      SITETYPESPEC(ls->LocalId());
      if(ls->Next() != NULL){COMA; LINE; TAB;}
      ls = ls->Next();
    }
  END_BLOCK;
  sprintf(bufS,"const int %s::ExternalSiteSpecTableIndex[]",name);
  if(NumberExternalSites>0)
    {
      OPEN_BLOCK(bufS);
      ls = ExternalSites;
      while(ls != NULL)
        {
          //SITETYPESPEC(ATypeMap[ExternalSites[i]]);
          SITETYPESPEC(ls->LocalId());
          if(ls->Next() != NULL){COMA; LINE; TAB;}
          ls = ls->Next();
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = {-1};\n");
      STRNG(bufS);
    }
  return 1;
}



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//             Begin force term list
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int  FragSpec::PrintForceTermInternalItem(int nsites, int *sites, int type, char *fPF)
{
  int i,a1;
  char bufS[256],tbufS[256];
  sprintf(bufS,"{ ");
  for(i=0;i<nsites;i++)
    {
      assert(!IsNew(NumberInternalSites,InternalSites,sites[i],a1));
      if(a1 < NumberInternalSites)
  {
    sprintf(tbufS,"{0,%4d}, ",a1);
    strcat(bufS,tbufS);
  }
      else
  {
    sprintf(tbufS,"{1,%4d}, ",a1);
    strcat(bufS,tbufS);
  }
    }
  sprintf(tbufS,"%4d }",type);
  strcat(bufS,tbufS);
  strcat(fPF,bufS);
  return 1;
}

int  FragSpec::PrintForceTermExternalItem(int nsites, int *sites, int type,  char *fPF)
{
  int i,a1;
  char bufS[256],tbufS[256];
  sprintf(bufS,"{ ");
  for(i=0;i<nsites;i++)
    {
      if(IsNew(NumberExternalSites,ExternalSites,sites[i],a1))
  {assert(!IsNew(NumberInternalSites,InternalSites,sites[i],a1));}
      else
  a1 += NumberInternalSites;
      if(a1 < NumberInternalSites)
  {
    sprintf(tbufS,"{0,%4d}, ",a1);
    strcat(bufS,tbufS);
  }
      else
  {
    sprintf(tbufS,"{1,%4d}, ",a1);
    strcat(bufS,tbufS);
  }
    }
  sprintf(tbufS,"%4d }",type);
  strcat(bufS,tbufS);
  strcat(fPF,bufS);
  return 1;
}



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//             Begin bond list
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int FragSpec::PrintBondList(char *fPF)
{
  int n = 0;
  int a1,a2;
  char bufS[256],tbufS[256];
  int sites[2];
  char termname[256];
  sprintf(termname,"Bond");
  sprintf(bufS,"const %s %s::Internal%ss[]",termname,name,termname);
  if(NumberInternalBonds > 0)
    {
      OPEN_BLOCK(bufS);
      Bond *bd = InternalBonds;
      while (bd != NULL)
        {
    sites[0] = bd->A1();
    sites[1] = bd->A2();
    PrintForceTermInternalItem(2, sites, bd->LocalId(), fPF);
          if(bd->Next() != NULL){COMA; LINE; TAB;}
          bd = bd->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }

  sprintf(bufS,"const %s %s::External%ss[]",termname,name,termname);
  if(NumberExternalBonds > 0)
    {
      OPEN_BLOCK(bufS);
      Bond *bd = ExternalBonds;
      while(bd != NULL)
        {
    sites[0] = bd->A1();
    sites[1] = bd->A2();
    PrintForceTermExternalItem(2, sites, bd->LocalId(), fPF);
          if(bd->Next() != NULL){COMA; LINE; TAB;}
          bd = bd->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }
  return n;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//             Begin Angle list
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int FragSpec::PrintAngleList(char *fPF)
{
  int n = 0;
  int typ,a1,a2,a3;
  char bufS[256],tbufS[256];
  int sites[3];
  char termname[256];
  sprintf(termname,"Angle");
  sprintf(bufS,"const %s %s::Internal%ss[]",termname,name,termname);
  if(NumberInternalAngles)
    {
      OPEN_BLOCK(bufS);
      Angle *a = InternalAngles;
      while(a != NULL)
        {
    sites[0] = a->A1();
    sites[1] = a->A2();
    sites[2] = a->A3();
    PrintForceTermInternalItem(3, sites, a->LocalId(), fPF);
          if(a->Next() != NULL){COMA; LINE; TAB;}
          a = a->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }

  sprintf(bufS,"const %s %s::External%ss[]",termname, name, termname);
  if(NumberExternalAngles > 0)
    {
      OPEN_BLOCK(bufS);
      Angle *a = ExternalAngles;
      while(a != NULL)
        {
    sites[0] = a->A1();
    sites[1] = a->A2();
    sites[2] = a->A3();
    PrintForceTermInternalItem(3, sites, a->LocalId(), fPF);
          if(a->Next() != NULL){COMA; LINE; TAB;}
          a = a->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }
  return n;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//             Begin torsion list
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int FragSpec::PrintTorsionList(char *fPF)
{

  char bufS[256],tbufS[256];
  int a1,a2,a3,a4,i,j,n;
  int sites[4];
  char termname[256];
  sprintf(termname,"Torsion");
  sprintf(bufS,"const %s %s::Internal%ss[]",termname,name,termname);

  if(NumberInternalTorsions>0)
    {
      OPEN_BLOCK(bufS);
      Torsion *t = InternalTorsions;
      while(t != NULL)
        {
    sites[0] = t->A1();
    sites[1] = t->A2();
    sites[2] = t->A3();
    sites[3] = t->A4();
    PrintForceTermInternalItem(4, sites, t->LocalId(), fPF);
          if(t->Next() != NULL){COMA; LINE; TAB;}
          t = t->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }

  sprintf(bufS,"const %s %s::External%ss[]",termname,name,termname);
  if(NumberExternalTorsions>0)
    {
      OPEN_BLOCK(bufS);
      Torsion *t = ExternalTorsions;
      while(t != NULL)
        {
    sites[0] = t->A1();
    sites[1] = t->A2();
    sites[2] = t->A3();
    sites[3] = t->A4();
    PrintForceTermExternalItem(4, sites, t->LocalId(), fPF);
          if(t->Next() != NULL){COMA; LINE; TAB;}
          t = t->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }
  return n;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//             Begin Improper list
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int FragSpec::PrintImproperList(char *fPF)
{
  char bufS[256],tbufS[256];
  int a1,a2,a3,a4;
  int n;
  int sites[4];
  char termname[256];
  sprintf(termname,"Improper");
  sprintf(bufS,"const %s %s::Internal%ss[]",termname,name,termname);
  if(NumberInternalImpropers>0)
    {
      OPEN_BLOCK(bufS);
      ImproperTorsion *t = InternalImpropers;
      while(t != NULL)
        {
    sites[0] = t->A1();
    sites[1] = t->A2();
    sites[2] = t->A3();
    sites[3] = t->A4();
    PrintForceTermInternalItem(4, sites, t->LocalId(), fPF);
          if(t->Next() != NULL){COMA; LINE; TAB;}
          t = t->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }

  sprintf(bufS,"const %s %s::External%ss[]",termname,name,termname);
  if(NumberExternalImpropers>0)
    {
      OPEN_BLOCK(bufS);
      ImproperTorsion *t = ExternalImpropers;
      while(t != NULL)
        {
    sites[0] = t->A1();
    sites[1] = t->A2();
    sites[2] = t->A3();
    sites[3] = t->A4();
    PrintForceTermExternalItem(4, sites, t->LocalId(), fPF);
          if(t->Next() != NULL){COMA; LINE; TAB;}
          t = t->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
      STRNG(bufS);
    }
  return n;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//             Begin term types lists
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int FragSpec::PrintTermTypes(char *fPF)
{
  int n=0;
  char bufS[256],tbufS[256];



  sprintf(bufS,"const %s %s::%s_Table[]",BondTermName(),name,BondTermName());
  if(NumberBondTypes>0)
    {
      OPEN_BLOCK(bufS);
      Bond *bd = BondTypes;
      while(bd != NULL)
        {
    sprintf(bufS,"{ %s, %s }",bd->Arg(0),bd->Arg(1));
    strcat(fPF,bufS);
    //          BONDTYPESPEC(bd->d_k0,bd->d_eq);
          if(bd->Next() != NULL){COMA; LINE; TAB;}
          bd = bd->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { 0.0,0.0 };\n");
      STRNG(bufS);
    }

  sprintf(bufS,"const %s %s::%s_Table[]",AngleTermName(),name,AngleTermName());
  if(NumberAngleTypes>0)
    {
      OPEN_BLOCK(bufS);
      Angle *a = AngleTypes;
      while(a != NULL)
        {
    sprintf(bufS,"{ %s, %s }",a->Arg(0),a->Arg(1));
    strcat(fPF,bufS);
    //          ANGLETYPESPEC(a->d_k0,a->d_eq);
          if(a->Next() != NULL){COMA; LINE; TAB;}
          a = a->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { 0.0,0.0 };\n");
      STRNG(bufS);
    }

  sprintf(bufS,"const %s %s::%s_Table[]",UreyTermName(),name,UreyTermName());
  if(NumberAngleTypes>0)
    {
      OPEN_BLOCK(bufS);
      Angle *a = AngleTypes;
      while(a != NULL)
        {
    sprintf(bufS,"{ %s, %s }",a->Arg(2),a->Arg(3));
    strcat(fPF,bufS);
    //          UBTYPESPEC(a->d_ubk0,a->d_ubeq);
          if(a->Next() != NULL){COMA; LINE; TAB;}
          a = a->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { 0.0,0.0 };\n");
      STRNG(bufS);
    }


  sprintf(bufS,"const %s %s::%s_Table[]",TorsionTermName(),name,TorsionTermName());
  if(NumberTorsionTypes)
    {
      OPEN_BLOCK(bufS);
      Torsion *a = TorsionTypes;
      while(a != NULL)
        {
    sprintf(bufS,"{ %s, %s, %s }",a->Arg(0),a->Arg(1),a->Arg(2));
    strcat(fPF,bufS);
    //          TORSIONTYPESPEC(a->d_k0,(int)a->d_wells,a->d_eq);
          if(a->Next() != NULL){COMA; LINE; TAB;}
          a = a->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { 0.0,0,0.0 };\n");
      STRNG(bufS);
    }

  sprintf(bufS,"const %s %s::%s_Table[]",ImproperTermName(),name,ImproperTermName());
  if(NumberImproperTypes>0)
    {
      OPEN_BLOCK(bufS);
      ImproperTorsion *a = ImproperTypes;
      while(a != NULL)
        {
    sprintf(bufS,"{ %s, %s }",a->Arg(0),a->Arg(1));
    strcat(fPF,bufS);
      //          TORSIONTYPESPEC(a->d_k0,(int)a->d_wells,a->d_eq);
          if(a->Next() != NULL){COMA; LINE; TAB;}
          a = a->Next();
          n++;
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { 0.0,0.0 };\n");
      STRNG(bufS);
    }
  return 1;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//             Begin Excluded site pairs list
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int FragSpec::PrintExcludeSitePairsList(char *fPF)
{
   char bufS[256],tbufS[256];
   ExPair *xx;
  sprintf(bufS,"const SiteId %s::ExcludedSitePairs[]",name);
  if(NumberExcludedSitePairs>0)
    {
      OPEN_BLOCK(bufS);
      xx = ExcludedSitePairs;
      while(xx != NULL)
        {
          int a1 = xx->A1();
          int a2 = xx->A2();
          int a3 = xx->A3();
          if(a2 < 0)
            { a3  = -a2; }
          else
            { a3  =  a2;  }

          EXCLUDESITEPAIR(a1,a2,a3);
          if(xx->Next() != NULL){COMA; LINE; TAB;}
          xx = xx->Next();
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0} };\n");
      STRNG(bufS);
    }
  return 1;
}
int FragSpec::Print14PairsList(char *fPF)
{
 Pair14 *xx;
   char bufS[256],tbufS[256];
  sprintf(bufS,"const SiteId %s::Pair14SitePairs[]",name);
  if(NumberPair14SitePairs>0)
    {
      OPEN_BLOCK(bufS);
      xx = Pair14SitePairs;
      while(xx != NULL)
        {
          int a1 = xx->A1();
          int a2 = xx->A2();
          int a3 = xx->A3();
          if(a2 < 0)
            { a3 = -a2;}
          else
            { a3 = a2;}

          EXCLUDESITEPAIR(a1,a2,a3);
          if(xx->Next() != NULL){COMA; LINE; TAB;}
          xx = xx->Next();
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0} };\n");
      STRNG(bufS);
    }
  return 1;
}
int FragSpec::NumberCodes()
{
  int cnt = 0;
  int nTerms = NumberTerms;
  TermSpec *trm = TermList;
  if(nTerms > 0)
    {
      while(trm != NULL)
        {
          cnt++;
          cnt++;
          switch(trm->mTermType)
            {
            case BONDCODE:
              cnt++;
              break;
            case EXCLUDEDCODE:
              break;
            case PAIR14CODE:
              break;
            case ANGLECODE:
              cnt++;
              cnt++;
              break;
            case UBCODE:
              cnt++;
              cnt++;
              break;
            case TORSIONCODE:
            case OPLSTORSIONCODE:
              cnt++;
              cnt++;
              cnt++;
              break;
            case IMPROPERCODE:
            case OPLSIMPROPERCODE:
              cnt++;
              cnt++;
              cnt++;
              break;
            }
          trm = trm->Next();
          cnt++;
        }
    }
  return cnt;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//           Begin code list
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int FragSpec::PrintCodeList(char *fPF)
{
  int cnt = 0;
  char bufS[1024 * 1024],tbufS[1000];
  int nTerms = NumberTerms;
  TermSpec *trm = TermList;
  INIT_COUNT(name,"NumberTerms",1);
  sprintf(bufS,"const Term %s::TermList[] = {    {    0, {  0,   0}, {  0,   0}, {  0,   0}, {  0,   0},    0} };\n\n",name);
  STRNG(bufS);

  // sprintf(bufS,"const Term %s::TermList[]",name);
  int bt=1;
  int lvls = 2;
  int nsites;
  int nparams = 0;
  bufS[0] = '\0';
  if(nTerms > 0)
    {
      sprintf(bufS,"const Code %s::CodeList[]",name);
      OPEN_BLOCK(bufS);
      while(trm != NULL)
        {
          int nI = NumberInternalSites;
          int ss1,ss2,ss3,ss4,aa1,aa2,aa3,aa4;
          aa1 = trm->mA1;
          aa2 = trm->mA2;
          aa3 = trm->mA3;
          aa4 = trm->mA4;
          ss1 = trm->sa1;
          ss2 = trm->sa2;
          ss3 = trm->sa3;
          ss4 = trm->sa4;
          //      if(aa1 < nI){ss1 = 0;}else{ss1 = 1; aa1 -= nI;};
          //      if(aa2 < 0){ss2 = 1;aa2 = -aa2-1;}else if (aa2 < nI){ss2 = 0;}else{ss2 = 1;aa2 -= nI;}
          //      if(aa3 < nI){ss3 = 0;}else{ss3 = 1; aa3 -= nI;};
          //      if(aa4 < nI){ss4 = 0;}else{ss4 = 1; aa4 -= nI;};

    UDF_RegistryIF::GetNSitesByUDFCode(trm->mTermType, nsites);

          CATSITE(0,0,trm->mTermType); CATDELIM; cnt++;
    if(nsites > 0)
      {
        CATSITE(bt,ss1,aa1); CATDELIM; cnt++;
      }
    if(nsites > 1)
      {
        CATSITE(bt,ss2,aa2); CATDELIM; cnt++;
      }
    if(nsites > 2)
      {
        CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
      }
    if(nsites > 3)
      {
        CATSITE(bt,ss4,aa4); CATDELIM; cnt++;
      }
    UDF_RegistryIF::GetParamCountByUDFCode(trm->mTermType,nparams);
    if(nparams > 0)
      {
        CATSITE(1,0,trm->mTermParams); CATDELIM; cnt++;
      }
        
    
//            CATSITE(0,0,trm->mTermType); CATDELIM; cnt++;
//            CATSITE(bt,ss1,aa1); CATDELIM; cnt++;
//            CATSITE(bt,ss2,aa2); CATDELIM; cnt++;

//            switch(trm->mTermType)
//              {
//              case BONDCODE:
//                CATSITE(1,0,trm->mTermParams); CATDELIM; cnt++;
//                break;
//              case EXCLUDEDCODE:
//                break;
//              case PAIR14CODE:
//                break;
//              case ANGLECODE:
//                CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
//                CATSITE(1,0,trm->mTermParams); CATDELIM; cnt++;
//                break;
//              case UBCODE:
//                    CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
//                    CATSITE(1,0,trm->mTermParams); CATDELIM; cnt++;
//                    break;

//              case TORSIONCODE:
//              case OPLSTORSIONCODE:
//                CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
//                CATSITE(bt,ss4,aa4); CATDELIM; cnt++;
//                CATSITE(1,0,trm->mTermParams); CATDELIM; cnt++;
//                break;
//              case IMPROPERCODE:
//              case OPLSIMPROPERCODE:
//                CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
//                CATSITE(bt,ss4,aa4); CATDELIM; cnt++;
//                CATSITE(1,0,trm->mTermParams); CATDELIM; cnt++;
//                break;
//              }
          trm = trm->Next();
        }
      CATSITE(0,0,0);

      END_BLOCK;
    }
  else
    {
      strcat(bufS," = { {0,0,0,0,0,0,0,0} };\n");
      STRNG(bufS);
      sprintf(bufS,"const Code %s::CodeList[] = { {0,0} };",name);
      STRNG(bufS);
    }
  INIT_COUNT(name,"NumberCodes",cnt);
  return 1;
}

int FragSpec::PrintFragmentNeighborhood(char *fPF)
{
  int cnt = 0;
  char bufS[1024 * 1024],tbufS[1000];
  int nn = NumberFragmentNeighbors;
  List *fs = FragmentNeighborList;
  INIT_COUNT(name,"NumberFragmentNeighbors",nn);
  sprintf(bufS,"const int %s::FragmentNeighborList[] ",name);
  OPEN_BLOCK(bufS);
  if(NumberFragmentNeighbors > 0)
    {
      while(fs != NULL)
        {
          BLOCK1(fs->GlobalId());
          if(fs->Next() != NULL)
            { CATDELIM; }
          fs = fs->Next();
        }
    }
  else
    {
      STRNG(" { -1 } \n");
    }
  END_BLOCK;
  return 1;
}





