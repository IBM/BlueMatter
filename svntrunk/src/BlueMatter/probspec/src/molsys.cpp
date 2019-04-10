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
 

#include <math.h>
#include <BlueMatter/molsys.hpp>
#include <BlueMatter/parsexmlspec.hpp>
//#include <BlueMatter/pushparams.hpp>
#include <BlueMatter/headerhandler.hpp>
#include <strstream.h>

extern MolecularSystem the_MolecularSystem;

void WriteDataTransferBlock(const char *msdVarName,const char *elemType,const char *nelements,const char* fsVarName, const char *fPF);

MolecularSystem::MolecularSystem()
{
  d_globs = new SysCell();
  d_mols = new SysCell();
  d_frags = NULL;
  d_fragSpecs = NULL;
  d_redFragSpecs = NULL;
  d_nFrags = d_nRedFrags = d_maxFrag = 0;

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
MolecularSystem::~MolecularSystem()
{
  if(d_globs != NULL)
    delete d_globs;
  if(d_mols != NULL)
    delete d_mols;
  if(d_frags != NULL)
    delete  d_frags;
  if(d_fragSpecs != NULL)
    delete d_fragSpecs;
  if(d_redFragSpecs != NULL)
    delete d_redFragSpecs;

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


int MolecularSystem::ApplyFragmentationScheme()
{
  int i,n;
  Fragment *frag = d_frags->GetFragList();
  FragSpec *fs,*f = NULL;
  Atom **atab = d_mols->ATab();
  d_maxFrag = 0;
  int nf = 0;

  while(frag != NULL)
    {
      fs = new FragSpec();

      n = frag->NumberInternalSites;
      assert(n);

      if(n >= d_maxFrag)
        d_maxFrag = fs->NumberInternalSites;

      for(i=0;i<n;i++)
        {
          int s = frag->InternalSites[i];
          Atom * at = atab[s];
          fs->AddInternalSite(at);
          at->LocalId() = atab[s]->LocalId();
        }

      assert(PrepareFragment(fs));
      fs->GlobalId() = nf++;
      if(f == NULL)
        { f = fs; f->LocalId() = 0; }
      else
        f->Append(fs);

      frag = frag->Next();
    }
  if(d_fragSpecs != NULL)
    {delete d_fragSpecs; d_fragSpecs = NULL;}
  d_fragSpecs = f;


  fs = d_fragSpecs;
  while(fs != NULL)
    {
      DeriveFragmentNeighborhood(fs);
      fs = fs->Next();
    }

  CompressFragments();

  return 1;
}

int MolecularSystem::CompressFragments()
{
  int globId = 0;
  FragSpec * fs,*rfs,*trfs;
  assert(d_fragSpecs != NULL);
  if(d_redFragSpecs != NULL)
    { delete d_redFragSpecs; }

  fs = d_fragSpecs;
  rfs = fs->Copy();
  rfs->Type() = fs->Type() = 0;
  d_nRedFrags = 1;
  fs = fs->Next();
  while(fs != NULL)
    {
      int dup = 0;
      trfs = rfs;
      while(trfs != NULL)
        {
          if(*trfs == *fs)
            {
              dup = 1;
              break;
            }
          trfs = trfs->Next();
        }
      if(dup)
        {  rfs->LocalId() = 0; fs->Type() = trfs->Type();       }
      else
        {
          trfs = fs->Copy();
          fs->Type() = trfs->Type() = trfs->GlobalId() = d_nRedFrags;
          rfs->Append(trfs);
          d_nRedFrags++;
        }
      fs = fs->Next();
    }
  d_redFragSpecs = rfs;
  return d_nRedFrags;
}


///////////////////////////////////////////////
//           DeriveParameterMaps
//////////////
int MolecularSystem::DeriveParameterMaps()
{
  int i,j;
  Atom *ls;
  List *tls;
  d_mols->ReduceAtomTypes();   // Maps ats localIds to type indecies
  ls = d_mols->ATypes();
  while(ls != NULL)
    {
      Atom *pls = d_globs->Ats();
      while(pls != NULL)
        {
          if(ls->Type() == pls->Type())
            break;
          pls = pls->Next();
        }
      assert(pls != NULL);
      ls->LocalId()  =  pls->LocalId();   // AType->Type points to globtype local index
      ls->Eps()      =  pls->Eps();
      ls->RMin()     =  pls->RMin();
      ls->Eps14()    =  pls->Eps14();
      ls->RMin14()   =  pls->RMin14();
      for(j=0;j<MAXARGS;j++)
	ls->SetArg(j,pls->Arg(j));
      ls = ls->Next();
    }

  // update lj params for ats
  Atom * at = d_mols->Ats();
  while(at != NULL)
    {
      ls = (Atom *) d_mols->ATypes()->GetItem(at->LocalId());
      assert(ls != NULL);
      i = at->GlobalId();
      d_mols->ATab()[ i ]->GlobalId() = i;
      d_mols->ATab()[ i ]->LocalId()  = at->LocalId();
      d_mols->ATab()[ i ]->Eps()      = at->Eps()      =  ls->Eps();
      d_mols->ATab()[ i ]->RMin()     = at->RMin()     =  ls->RMin();
      d_mols->ATab()[ i ]->Eps14()    = at->Eps14()    =  ls->Eps14();
      d_mols->ATab()[ i ]->RMin14()   = at->RMin14()   =  ls->RMin14();
      for(j=0;j<MAXARGS;j++)
	d_mols->ATab()[ i ]->SetArg(j,ls->Arg(j));
      for(j=0;j<MAXARGS;j++)
	at->SetArg(j,ls->Arg(j));
      at = at->Next();
    }


  Bond *bd = d_mols->Bds();
  while(bd != NULL)
    {
      int found = 0;

      int a1 = d_mols->ATab()[ bd->A1() ]->Type();
      int a2 = d_mols->ATab()[ bd->A2() ]->Type();

      Bond *tbd = d_globs->Bds();
      while(tbd != NULL)
        {
          int A1 = tbd->A1();
          int A2 = tbd->A2();
          if( (A1 == a1 && A2 == a2) || (A1 == a2 && A2 == a1) )
            {
              found      = 1;
              bd->Type() = tbd->Type();
              bd->K0()   = tbd->K0();
              bd->Eq()   = tbd->Eq();
	      for(j=0;j<MAXARGS;j++)
		bd->SetArg(j,tbd->Arg(j));
              break;
            }
          tbd = tbd->Next();
        }
      assert(found);
      bd = bd->Next();
    }

  Angle *ang = d_mols->Angs();
  while(ang != NULL)
    {
      int found = 0;
      int a1 = d_mols->ATab()[ ang->A1() ]->Type();
      int a2 = d_mols->ATab()[ ang->A2() ]->Type();
      int a3 = d_mols->ATab()[ ang->A3() ]->Type();

      Angle *tang = d_globs->Angs();
      while(tang != NULL)
        {
          int A1 = tang->A1();
          int A2 = tang->A2();
          int A3 = tang->A3();
          if( (A1 == a1 && A2 == a2 && A3 == a3) ||
              (A1 == a3 && A2 == a2 && A3 == a1))
            {
              found = 1;
              ang->Type() = tang->Type();
              ang->K0() = tang->K0();
              ang->Eq() = tang->Eq();
              ang->UBK0() = tang->UBK0();
              ang->UBEq() = tang->UBEq();
	      for(j=0;j<MAXARGS;j++)
		ang->SetArg(j,tang->Arg(j));
              break;
            }
          tang = tang->Next();
        }
      assert(found);
      ang = ang->Next();
    }

  // if 0 then Already did Torsion in psf file read or pushtorsion
#if 0
  Torsion *tor = d_mols->Tors();
  while(tor != NULL)
    {
      int found = 0;
      int a1 = d_mols->ATab()[ tor->A1() ]->Type();
      int a2 = d_mols->ATab()[ tor->A2() ]->Type();
      int a3 = d_mols->ATab()[ tor->A3() ]->Type();
      int a4 = d_mols->ATab()[ tor->A4() ]->Type();

      Torsion *ttor = d_globs->Tors();
      while(ttor != NULL)
        {
          int A1 = ttor->A1();
          int A2 = ttor->A2();
          int A3 = ttor->A3();
          int A4 = ttor->A4();

          if( (A1 == a1 && A2 == a2 &&
               A3 == a3 && A4 == a4) ||
              (A1 == a4 && A2 == a3 &&
               A3 == a2 && A4 == a1) ||
              (A1 == 100 && A4 == 100 && A2 == a2 && A3 == a3)||
              (A1 == 100 && A4 == 100 && A3 == a2 && A2 == a3))
            {
              found = 1;
              tor->Type() = ttor->Type();
              tor->K0() = ttor->K0();
              tor->Eq() = ttor->Eq();
              tor->Wells() = ttor->Wells();
              break;
            }
          ttor = ttor->Next();
        }
      assert(found);
      tor = tor->Next();
    }
#endif
  ImproperTorsion *imp = d_mols->Imps();
  while(imp != NULL)
    {
      int found = 0;
      int a1 = d_mols->ATab()[ imp->A1() ]->Type();
      int a2 = d_mols->ATab()[ imp->A2() ]->Type();
      int a3 = d_mols->ATab()[ imp->A3() ]->Type();
      int a4 = d_mols->ATab()[ imp->A4() ]->Type();

      ImproperTorsion *timp = d_globs->Imps();
      while(timp != NULL)
        {
          int A1 = timp->A1();
          int A2 = timp->A2();
          int A3 = timp->A3();
          int A4 = timp->A4();

          if( (A1 == a1 && A2 == a2 &&
               A3 == a3 && A4 == a4) ||
              (A1 == a4 && A2 == a3 &&
               A3 == a2 && A4 == a1) ||
              (A2 == 100 && A3 == 100 && A1 == a1 && A4 == a4)||
              (A2 == 100 && A3 == 100 && A4 == a1 && A1 == a4))
            {
              found = 1;
              imp->Type() = timp->Type();
              imp->K0() = timp->K0();
              imp->Eq() = timp->Eq();
              imp->Wells() = timp->Wells();
	      for(j=0;j<MAXARGS;j++)
		imp->SetArg(j,timp->Arg(j));
              break;
            }
          timp = timp->Next();
        }
      assert(found);
      imp = imp->Next();
    }
   return 1;

}


///////////////////////////////////////////////
//           PrepareFragment
//////////////
int MolecularSystem::PrepareFragment(FragSpec *fs)
{

  ///////
  //     Give each atom a record of what terms in the
  //     fragment refer to it.
  ///////
  Atom *at = fs->InternalSites;
  while(at != NULL)
    {
      UpdateTermIncidenceLists(at,fs);
      at = at->Next();
    }

  // Set Force field codes
  fs->SetBondCode(BondCode());
  fs->SetAngleCode(AngleCode());
  fs->SetUreyCode(UreyCode()) ;
  fs->SetTorsionCode(TorsionCode());
  fs->SetImproperCode(ImproperCode());
  fs->SetExPairCode(ExPairCode());
  fs->SetPair14Code(Pair14Code());

  ///////
  //     Derive external atom list
  ///////
  UpdateExternalSiteList(fs);

  ///////
  //     Derive the fragment excluded and 14 pair lists
  ///////
  fs->DeriveExcludedAnd14Lists(d_mols->ATab());

  CompressTypes(fs);

  return 1;
}

void MolecularSystem::DeriveFragmentNeighborhood(FragSpec* fs)
{
  FragSpec* rfg = d_fragSpecs;
  while(rfg != NULL)
    {
      if (fs != rfg)
        {
          if(CheckIfNeighbor(fs,rfg))
            {
              List *ls = new List();
              ls->GlobalId() =  rfg->GlobalId();
              ls->Type() =  rfg->Type();
              if(fs->FragmentNeighborList == NULL)
                {
                  fs->FragmentNeighborList = ls;
                }
              else
                {
                  fs->FragmentNeighborList->Append(ls);
                }
              fs->NumberFragmentNeighbors++;
            }
        }
      rfg = rfg->Next();
    }
}

int  MolecularSystem::CheckIfNeighbor(FragSpec *fs,FragSpec *rfg)
{
  // traverse each externalsite of fs and see if any of the atoms are in the internalsites of rfg
  Atom *at = fs->ExternalSites;
  while(at != NULL)
    {
      Atom *tat = rfg->InternalSites;
      while(tat != NULL)
        {
          if(at->GlobalId() == tat->GlobalId())
            { return 1; }
          tat = tat->Next();
        }
      at = at->Next();
    }
  return 0;
}

///////////////////////////////////////////////
//           UpdateTermIncidenceLists
//////////////
int MolecularSystem::UpdateTermIncidenceLists(Atom *at,FragSpec *fs)
{
    Bond *b = at->Bonds();
    while(b != NULL)
      {
        if(b->InternalToList(at,2))
          fs->AddInternalBond(b);
        else
          fs->AddExternalBond(b);
        b = b->Next();
      }

    Angle *a = at->Angles();
    while(a != NULL)
      {
        if(a->InternalToList(at,3))
          fs->AddInternalAngle(a);
        else
          fs->AddExternalAngle(a);
        a = a->Next();
      }

    Torsion *t = at->Torsions();
    while(t != NULL)
      {
        if(t->InternalToList(at,4))
          fs->AddInternalTorsion(t);
        else
          fs->AddExternalTorsion(t);
        t = t->Next();
      }

    ImproperTorsion *it = at->Impropers();
    while(it != NULL)
      {
        if(it->InternalToList(at,4))
          fs->AddInternalImproper(it);
        else
          fs->AddExternalImproper(it);
        it = it->Next();
      }
    return 1;
}

///////////////////////////////////////////////
//           UpdateExternalSiteList
//////////////
int MolecularSystem::UpdateExternalSiteList(FragSpec *fs)
{
  int i, flg;
  Atom **atab = d_mols->ATab();
  Atom *at;
  Atom *a = fs->InternalSites;
  Bond *b = fs->ExternalBonds;
  while(b != NULL)
    {
      at= atab[b->A1()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[b->A2()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      b = b->Next();
    }
  Angle *ag = fs->ExternalAngles;
  while(ag != NULL)
    {
      at= atab[ag->A1()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[ag->A2()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[ag->A3()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      ag = ag->Next();
    }
  Torsion *t = fs->ExternalTorsions;
  while(t != NULL)
    {
      at= atab[t->A1()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[t->A2()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[t->A3()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[t->A4()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      t = t->Next();
    }
  ImproperTorsion *it = fs->ExternalImpropers;
  while(it != NULL)
    {
      at= atab[it->A1()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[it->A2()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[it->A3()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      at= atab[it->A4()];
      if(!at->InList(a)) fs->AddExternalSite(at);
      it = it->Next();
    }
  return fs->NumberExternalSites;
}

int MolecularSystem::CompressTypes(FragSpec *fs)
{
  int i,j;
  Bond *b,*bd;
  Angle *ag,*aag;
  Torsion *t,*tt;
  ImproperTorsion *it,*iit;

  //////////////////////////////////////
  //      compress Internal Bonds
  ////////
  b = fs->InternalBonds;
  if(fs->BondTypes != NULL)
    { delete fs->BondTypes; fs->NumberBondTypes = 0; fs->BondTypes = NULL;}
  if(b != NULL)
    {
      b->LocalId() = fs->NumberBondTypes;
      fs->BondTypes = b->Copy();
      fs->NumberBondTypes++;
      fs->AddBondTerm(b);
      b = b->Next();
      while(b != NULL)
        {
          if(b->NewType(fs->BondTypes,(List **)&bd))
            {
              Bond *nn = b->Copy();
              fs->BondTypes->Append(nn);
              nn->LocalId() = b->LocalId() = fs->NumberBondTypes;
              fs->NumberBondTypes++;
            }
          else
            b->LocalId() = bd->LocalId();
          fs->AddBondTerm(b);
          b = b->Next();
        }
    }
  //////////////////////////////////////
  //      compress External Bonds
  ////////
  b = fs->ExternalBonds;
  if(b != NULL)
    {
      if(fs->BondTypes == NULL)
        {
          b->LocalId() = fs->NumberBondTypes;
          fs->BondTypes = b->Copy();
          fs->NumberBondTypes++;
          fs->AddBondTerm(b);
          b = b->Next();
        }
      while(b != NULL)
        {
          if(b->NewType(fs->BondTypes,(List **)&bd))
            {
              Bond *nn = b->Copy();
              fs->BondTypes->Append(nn);
              nn->LocalId() = b->LocalId() = fs->NumberBondTypes;
              fs->NumberBondTypes++;
            }
          else
            b->LocalId() = bd->LocalId();
          fs->AddBondTerm(b);
          b = b->Next();
        }
    }


  //////////////////////////////////////
  //      compress Internal Angles
  ////////
  ag = fs->InternalAngles;
  if(fs->AngleTypes != NULL)
    { delete fs->AngleTypes; fs->NumberAngleTypes = 0; fs->AngleTypes = NULL;}
  if(ag != NULL)
    {
      ag->LocalId() = fs->NumberAngleTypes;
      fs->AngleTypes = ag->Copy();
      fs->NumberAngleTypes++;
      fs->AddAngleTerm(ag);
      ag = ag->Next();
      while(ag != NULL)
        {
          if(ag->NewType(fs->AngleTypes,(List **)&aag))
            {
              Angle *nn = ag->Copy();
              fs->AngleTypes->Append(nn);
              nn->LocalId() = ag->LocalId() = fs->NumberAngleTypes;
              fs->NumberAngleTypes++;
            }
          else
            ag->LocalId() = aag->LocalId();
          fs->AddAngleTerm(ag);
          ag = ag->Next();
        }
    }
  //////////////////////////////////////
  //      compress External Angles
  ////////
  ag = fs->ExternalAngles;
  if(ag != NULL)
    {
      if(fs->AngleTypes == NULL)
        {
          ag->LocalId() = fs->NumberAngleTypes;
          fs->AngleTypes = ag->Copy();
          fs->NumberAngleTypes++;
          fs->AddAngleTerm(ag);
          ag = ag->Next();
        }
      while(ag != NULL)
        {
          if(ag->NewType(fs->AngleTypes,(List **)&aag))
            {
              Angle *nn = ag->Copy();
              fs->AngleTypes->Append(nn);
              nn->LocalId() = ag->LocalId() = fs->NumberAngleTypes;
              fs->NumberAngleTypes++;
            }
          else
            ag->LocalId() = aag->LocalId();
          fs->AddAngleTerm(ag);
          ag = ag->Next();
        }
    }

  // Handle Urey bradley with inside AddAngleTerm terms of fragment

//    //////////////////////////////////////
//    //      compress Internal UreyBradley
//    ////////
//    ag = fs->InternalAngles;
//    if(fs->AngleTypes != NULL)
//      { delete fs->AngleTypes; fs->NumberAngleTypes = 0; fs->AngleTypes = NULL;}
//    if(ag != NULL)
//      {
//        ag->LocalId() = fs->NumberAngleTypes;
//        fs->AngleTypes = ag->Copy();
//        fs->NumberAngleTypes++;
//        fs->AddAngleTerm(ag);
//        ag = ag->Next();
//        while(ag != NULL)
//      {
//        if(ag->NewType(fs->AngleTypes,(List **)&aag))
//          {
//            Angle *nn = ag->Copy();
//            fs->AngleTypes->Append(nn);
//            nn->LocalId() = ag->LocalId() = fs->NumberAngleTypes;
//            fs->NumberAngleTypes++;
//          }
//        else
//          ag->LocalId() = aag->LocalId();
//        fs->AddAngleTerm(ag);
//        ag = ag->Next();
//      }
//      }
//    //////////////////////////////////////
//    //      compress External UreyBradley
//    ////////
//    ag = fs->ExternalAngles;
//    if(ag != NULL)
//      {
//        if(fs->AngleTypes == NULL)
//      {
//        ag->LocalId() = fs->NumberAngleTypes;
//        fs->AngleTypes = ag->Copy();
//        fs->NumberAngleTypes++;
//        fs->AddAngleTerm(ag);
//        ag = ag->Next();
//      }
//        while(ag != NULL)
//      {
//        if(ag->NewType(fs->AngleTypes,(List **)&aag))
//          {
//            Angle *nn = ag->Copy();
//            fs->AngleTypes->Append(nn);
//            nn->LocalId() = ag->LocalId() = fs->NumberAngleTypes;
//            fs->NumberAngleTypes++;
//          }
//        else
//          ag->LocalId() = aag->LocalId();
//        fs->AddAngleTerm(ag);
//        ag = ag->Next();
//      }
//      }


  //////////////////////////////////////
  //      compress Internal Torsions
  ////////



  //////////////////////////////////////
  //      compress Internal Torsions
  ////////
  t = fs->InternalTorsions;
  if(fs->TorsionTypes != NULL)
    { delete fs->TorsionTypes; fs->NumberTorsionTypes = 0; fs->TorsionTypes = NULL;}
  if(t != NULL)
    {
      t->LocalId() = fs->NumberTorsionTypes;
      fs->NumberTorsionTypes++;
      fs->TorsionTypes = t->Copy();
      fs->AddTorsionTerm(t);
      t = t->Next();
      while(t != NULL)
        {
          if(t->NewType(fs->TorsionTypes,(List **)&tt))
            {
              Torsion *nn = t->Copy();
              fs->TorsionTypes->Append(nn);
              nn->LocalId() = t->LocalId() = fs->NumberTorsionTypes;
              fs->NumberTorsionTypes++;
            }
          else
            t->LocalId() = tt->LocalId();
          fs->AddTorsionTerm(t);
          t = t->Next();
        }
    }



//    if(t != NULL)
//      {

//        while(!d_mols->CheckIfOnTorsionList(t->A1(),t->A4()))
//          {
//            t = t->Next();
//            if(t == NULL) break;
//          }
//        if(t != NULL)
//          {
//            t->LocalId() = fs->NumberTorsionTypes;
//            fs->TorsionTypes = t->Copy();
//            fs->NumberTorsionTypes++;
//            fs->AddTorsionTerm(t);
//            t = t->Next();
//          }
//        while(t != NULL)
//          {
//            if(d_mols->CheckIfOnTorsionList(t->A1(),t->A4()))
//              {
//                if(t->NewType(fs->TorsionTypes,(List **)&tt))
//                  {
//                    Torsion *nn = t->Copy();
//                    fs->TorsionTypes->Append(nn);
//                    nn->LocalId() = t->LocalId() = fs->NumberTorsionTypes;
//                    fs->NumberTorsionTypes++;
//                  }
//                else
//                  t->LocalId() = tt->LocalId();
//                fs->AddTorsionTerm(t);
//              }
//            t = t->Next();
//          }
//      }
//////////////////////////////////////
  //      compress External Torsions
  ////////
  t = fs->ExternalTorsions;
  if(t != NULL)
    {
      t->LocalId() = fs->NumberTorsionTypes;
      fs->NumberTorsionTypes++;
      fs->TorsionTypes = t->Copy();
      fs->AddTorsionTerm(t);
      t = t->Next();
      while(t != NULL)
        {
          if(t->NewType(fs->TorsionTypes,(List **)&tt))
            {
              Torsion *nn = t->Copy();
              fs->TorsionTypes->Append(nn);
              nn->LocalId() = t->LocalId() = fs->NumberTorsionTypes;
              fs->NumberTorsionTypes++;
            }
          else
            t->LocalId() = tt->LocalId();
          fs->AddTorsionTerm(t);
          t = t->Next();
        }
    }


//    if(t != NULL)
//      {
//        while(!d_mols->CheckIfOnTorsionList(t->A1(),t->A4()))
//          {
//            t = t->Next();
//            if(t == NULL) break;
//          }
//        if(fs->TorsionTypes == NULL && t != NULL)
//          {
//            t->LocalId() = fs->NumberTorsionTypes;
//            fs->TorsionTypes = t->Copy();
//            fs->NumberTorsionTypes++;
//            fs->AddTorsionTerm(t);
//            t = t->Next();
//          }
//        while(t != NULL)
//          {
//            if(d_mols->CheckIfOnTorsionList(t->A1(),t->A4()))
//              {
//                if(t->NewType(fs->TorsionTypes,(List **)&tt))
//                  {
//                    Torsion *nn = t->Copy();
//                    fs->TorsionTypes->Append(nn);
//                    nn->LocalId() = t->LocalId() = fs->NumberTorsionTypes;
//                    fs->NumberTorsionTypes++;
//                  }
//                else
//                  t->LocalId() = tt->LocalId();
//                fs->AddTorsionTerm(t);
//              }
//            t = t->Next();
//          }
//      }

  //////////////////////////////////////
  //      compress Internal Impropers
  ////////
  it = fs->InternalImpropers;
  if(fs->ImproperTypes != NULL)
    { delete fs->ImproperTypes; fs->NumberImproperTypes = 0; fs->ImproperTypes = NULL;}
  if(it != NULL)
    {
      it->LocalId() = fs->NumberImproperTypes;
      fs->ImproperTypes = it->Copy();
      fs->NumberImproperTypes++;
      fs->AddImproperTerm(it);
      it = it->Next();
      while(it != NULL)
        {
          if(it->NewType(fs->ImproperTypes,(List **)&iit))
            {
              ImproperTorsion *nn = it->Copy();
              fs->ImproperTypes->Append(nn);
              nn->LocalId() = it->LocalId() = fs->NumberImproperTypes;
              fs->NumberImproperTypes++;
            }
          else
            it->LocalId() = iit->LocalId();
          fs->AddImproperTerm(it);
          it = it->Next();
        }
    }
  //////////////////////////////////////
  //      compress External Impropers
  ////////
  it = fs->ExternalImpropers;
  while(it != NULL)
    {
      if(fs->ImproperTypes == NULL)
        {
          it->LocalId() = fs->NumberImproperTypes;
          fs->ImproperTypes = it->Copy();
          fs->NumberImproperTypes++;
          fs->AddImproperTerm(it);
          it = it->Next();
        }
      if(it->NewType(fs->ImproperTypes,(List **)&iit))
        {
          ImproperTorsion *nn = it->Copy();
          fs->ImproperTypes->Append(nn);
          nn->LocalId() = it->LocalId() = fs->NumberImproperTypes;
          fs->NumberImproperTypes++;
        }
      else
        it->LocalId() = iit->LocalId();
      fs->AddImproperTerm(it);
      it = it->Next();
    }

  return 1;
}


void MolecularSystem::ReadFragmentationScheme(FILE *fPF)

    {
      char bufS[1000];
      int i,j,n,nfrags=0;

      if(d_frags != NULL)
        {  delete d_frags;      }
      LINEIN;
      BUFINT(nfrags);
      if(nfrags==0)
        {
          d_frags = NULL;
          d_nFrags = 0;
          return;
        }
      d_frags = new FragmentationScheme();
      i = 0;
      for(i=0;i<nfrags;i++)
        {
          Fragment *f = new Fragment();
          LINEIN;
          BUFINT(n);
          f->NumberInternalSites = n;
          if(n)f->InternalSites = new int[n];
          for(j=0;j<n;j++)
            {
              SCANumberInt(f->InternalSites[j]);
              f->InternalSites[j]--;
            }
          LINEIN;
          d_frags->AddFragment(f);
        }
      d_nFrags = nfrags;
      return;
    }

void MolecularSystem::PreparePeptideFragmentationScheme(FILE *fPF)
{
  char bufS[1000];
  char tag1[20],tag2[20],tag3[20],tag4[20];
  int fragbuf[500];
  int maxfrags = 500;
  int f;

  if(d_frags != NULL)
    {  delete d_frags;  }

  Fragment *frg,*frags;
  d_frags = new FragmentationScheme();
  LINEIN;
  LINEIN;
  LINEIN;
  int i,j,k,d,n,tail;
  sscanf(bufS,"%d",&n);
  for(i=0;i<n;i++)
    {LINEIN;}
  LINEIN;
  LINEIN;
  int na;
  BUFINT(na);

  // Initiate begining fragment - define ammonium group as fragment
  frg = new Fragment();
  frags = frg;
  frg->NumberInternalSites = 4;
  frg->InternalSites = new int[4];
  for(i=0;i<4;i++)
    {
      LINEIN;
      frg->InternalSites[i] = i;
    }
  d_frags->AddFragment(frg);
  frg = new Fragment();
  d_nFrags++;
  f = 1;
  j = 0;
  n = 4;
  tail = 1;
  LINEIN;
  for(i=5;i<=na;i++)
    {
      sscanf(bufS,"%d %s %d ",&d,tag1,&k);

      if(k == f)  // WARNING!!! Assumes no gaps in residue number
        {
          fragbuf[j] = i;
          j++;
        }
      else
        {
          frg->NumberInternalSites = j;
          if(j) frg->InternalSites = new int[j];
          for(k = 0;k<j;k++)
            {
              frg->InternalSites[k] = fragbuf[k] - 1;
            }
          //      frags->Append(frg);
          d_frags->AddFragment(frg);
          frg = new Fragment();
          d_nFrags++;
          f++;
          fragbuf[0] = i;
          if(strstr(tag1,"WAT") || strstr(tag1,"SOLV"))
            {
              tail = 0;
              break;
            }
          j = 1;
        }
      LINEIN;
    }
  if(tail)
    {
      frg->NumberInternalSites = j;
      if(j) frg->InternalSites = new int[j];
      for(k = 0;k<j;k++)
        {
          frg->InternalSites[k] = fragbuf[k] - 1;
        }
      d_frags->AddFragment(frg);
      d_nFrags++;
      frg = new Fragment();
      f++;
    }
  while(strstr(tag1,"WAT") || strstr(tag1,"SOLV"))
    {
      j = 3;
      frg->NumberInternalSites = j;
      frg->InternalSites = new int[j];
      for(k = 0;k<j;k++)
        {
          frg->InternalSites[k] = i - 1;
          i++;
        }
      if(i >= na) break;
      d_frags->AddFragment(frg);
      frg = new Fragment();
      d_nFrags++;
      f++;
      LINEIN;
      LINEIN;
      LINEIN;
      tag1[0]='\0';
      sscanf(bufS,"%d %s %d ",&d,tag1,&k);
    }
  delete frg;
  return;
}

void MolecularSystem::PrepareSingleFragmentScheme()
{
  Fragment *f = new Fragment();
  if(d_frags != NULL)
    { delete d_frags; }
  d_frags = new FragmentationScheme();
  f->NumberInternalSites = d_mols->NAtoms();
  f->InternalSites = new int[d_mols->NAtoms()];
  for(int i=0;i<d_mols->NAtoms();i++)
    {
      f->InternalSites[i] = i;
    }
  d_frags->AddFragment(f);
  d_nFrags = 1;
  return;
}


void MolecularSystem::ReadCharmmAtomTypes(FILE *fPF)
{
  Atom *a;
  char bufS[1000], tag1[50];
  int i;
  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("MASS")) break;
      LINEIN;
    }
  i = 0;
  int t;
  char n[NAMELEN];
  while(CHECK("MASS"))
    {
      a = new Atom();
      sscanf(bufS,"%s %d %s",tag1,&t,n);
      a->Type() = t;
      strcpy(a->Name(),n);
      a->GlobalId() = i;
      d_globs->AddAtom(a);
      LINEIN;
      i++;
    }
  a = new Atom();
  a->Type() = 100;
  strcpy(a->Name(),"X");
  a->GlobalId() = i;
  d_globs->AddAtom(a);
  return;
}

void MolecularSystem::ReadCharmmParameterFile(FILE *fPF)
{

  char bufS[1000],tag1[20],tag2[20],tag3[20],tag4[20];

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("BONDS")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  int nb=0;
  //Now read bond params
  while(!feof(fPF))
    {
      Bond *bd = new Bond();
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %lf %lf",tag1,tag2,&bd->K0(),&bd->Eq());
      bd->A1() = d_globs->AtypeNum(tag1);
      bd->A2() = d_globs->AtypeNum(tag2);
      bd->GlobalId() = d_globs->NBonds();
      d_globs->AddBond(bd);
      bd->Type() = ++nb;
      LINEIN;
      LINEIN;
    }

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("ANGLES")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  int ang = 0;
  //Now read angle params
  while(!feof(fPF))
    {
      char c[15];
      c[0] = '\0';
      Angle *a = new Angle();
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %s %lf %lf %s %lf",tag1,tag2,tag3,
             &a->K0(),&a->Eq(), c, &a->UBEq());
      a->A1() = d_globs->AtypeNum(tag1);
      a->A2() = d_globs->AtypeNum(tag2);
      a->A3() = d_globs->AtypeNum(tag3);
      if (c[0] == '!')
        {
          a->UBK0() = a->UBEq() = 0.0;
        }
      else
        {
          a->UBK0() = atof(c);
        }
      a->GlobalId() = d_globs->NAngles();
      d_globs->AddAngle(a);
      a->Type() = ++ang;
      LINEIN;
      LINEIN;
    }

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("DIHEDRALS")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  int nt=0;
  //Now read for torsion params
  while(!feof(fPF))
    {
      Torsion *t = new Torsion();
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %s %s %lf %lf %lf",tag1,tag2,tag3,tag4,
             &t->K0(),&t->Wells(),&t->Eq());
      t->A1() = d_globs->AtypeNum(tag1);
      t->A2() = d_globs->AtypeNum(tag2);
      t->A3() = d_globs->AtypeNum(tag3);
      t->A4() = d_globs->AtypeNum(tag4);
      t->GlobalId() = d_globs->NTorsions();
      d_globs->AddTorsion(t);
      t->Type() = ++nt;
      LINEIN;
      LINEIN;
    }

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("IMPROPER")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  int ni=0;
  //Now read improper torsion params
  while(!feof(fPF))
    {
      ImproperTorsion *t = new ImproperTorsion;
      int jnk;
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %s %s %lf %lf %lf",tag1,tag2,tag3,tag4,
             &t->K0(),&t->Wells(),&t->Eq());
      t->A1() = d_globs->AtypeNum(tag1);
      t->A2() = d_globs->AtypeNum(tag2);
      t->A3() = d_globs->AtypeNum(tag3);
      t->A4() = d_globs->AtypeNum(tag4);
      t->GlobalId() = d_globs->NImpropers();
      d_globs->AddImproperTorsion(t);
      t->Type() = ++ni;
      LINEIN;
      LINEIN;
    }

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("NONBONDED")) break;
      LINEIN;
    }
  LINEIN;
  LINEIN;
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }

  //Now read for nonbonded param reading
  int nargs;
  while(!feof(fPF))
    {
      double jnk;
      char  c[10];
      c[0] = '\0';
      while(bufS[0] == '!')
        {
          LINEIN;
        }
      if(bufS[0] == '\n'){LINEIN;}
      if(CHECK("HBOND CUTHB")){break;}
      int type;
      double eps,rmin,eps14,rmin14;
      nargs = sscanf(bufS,"%s %lf %lf %lf %s %lf %lf",tag1,&jnk,
                     &eps,&rmin,c,&eps14,&rmin14);
      type = d_globs->AtypeNum(tag1);
      Atom * l = d_globs->Ats();
      while(l != NULL)
        {
          if(l->Type() == type)
            {
              l->Eps() = eps;
              l->RMin() = rmin;
              if(c[0] == '!')
                {
                  l->Eps14() = eps;
                  l->RMin14() = rmin;
		  l->LJType() = type;
                }
              else
                {
                  l->Eps14() = eps14;
                  l->RMin14() = rmin14;
		  l->LJ14Type() = type;
                }
              break;
            }
          l = l->Next();
        }
      LINEIN;
      LINEIN;
    }
  return;
}


void MolecularSystem::ReadPsfFile(FILE *fPF)
{
  char bufS[1000];
  char tag1[20],tag2[20],tag3[20],tag4[20];

  if(d_mols != NULL)
    {  delete d_mols;   }
  d_mols = new SysCell();

  LINEIN;      LINEIN;      LINEIN;
  int i,j,k,n;
  sscanf(bufS,"%d",&n);
  for(i=0;i<n;i++)
    {LINEIN;}
  LINEIN;      LINEIN;
  int na;
  BUFINT(na);

  double mass,charge;
  for(i=0;i<na;i++)
    {
      int num,typ;
      Atom * at = new Atom();
      at->GlobalId() = i;
      LINEIN;
      sscanf(bufS,"%d %s %d %s %s %s %lf %lf",&j,tag1,&k,tag2,tag3,tag4,&charge,&mass);
      if(sscanf(tag4,"%d",&typ))
        {
          assert(d_globs->AtypeName(typ,at->Name()));
        }
      else
        {
          typ = d_globs->AtypeNum(tag4);
          assert(typ != 0);
          d_globs->AtypeName(typ,at->Name());
        }
      at->Type() = typ;
      at->Mass() = mass;
      at->Charge() = charge;
      d_mols->AddAtom(at);
    }

  //---------------------
  LINEIN;  LINEIN;
  int nb;
  BUFINT(nb);

  for(i=0;i<nb;i++)
    {
      Bond * bd = new Bond();
      SCANumberInt2(bd->A1(),bd->A2());
      bd->A1()--;      bd->A2()--;
      bd->GlobalId() = d_mols->NBonds();
      d_mols->AddBond(bd);
    }
  //----------------------
  LINEIN;  LINEIN;  LINEIN;
  int nang;
  BUFINT(nang);

  for(i=0;i<nang;i++)
    {
      Angle *a = new Angle();
      SCANumberInt3(a->A1(),a->A2(),a->A3());
      a->A1()--; a->A2()--; a->A3()--;
      a->GlobalId() = d_mols->NAngles();
      d_mols->AddAngle(a);
    }
  //----------------------

  LINEIN;  LINEIN;  LINEIN;
  // We must flatten the atom list here for random access
  if(d_mols->Ats() != NULL)
    {
      if( d_mols->ATab() != NULL)
        {
          for(i=0;i<d_mols->NAtoms();i++)
            {delete d_mols->d_atab[i]; d_mols->d_atab[i] = NULL;}
          delete [] d_mols->d_atab;
        }
      d_mols->Ats()->Rewind((List **)&d_mols->d_ats);
      d_mols->Ats()->Flatten((List ***)&d_mols->d_atab);
    }

  Torsion *et = NULL;

  int nt;
  BUFINT(nt);
  int found = 0;
  for(i=0;i<nt;i++)
    {
      Torsion *t = new Torsion();
      t->GlobalId() = i;
      SCANumberInt4(t->A1(),t->A2(),t->A3(),t->A4());
      t->A1()--;t->A2()--;t->A3()--;t->A4()--;
      int a1 = d_mols->ATab()[t->A1()]->Type();
      int a2 = d_mols->ATab()[t->A2()]->Type();
      int a3 = d_mols->ATab()[t->A3()]->Type();
      int a4 = d_mols->ATab()[t->A4()]->Type();
      found = 0;
      Torsion *T = d_globs->Tors();
      j = 0;
      while(T != NULL)
        {
          if( (T->A1() == a1 && T->A2() == a2 && T->A3() == a3 && T->A4() == a4) ||
              (T->A1() == a4 && T->A2() == a3 && T->A3() == a2 && T->A4() == a1) )
            {
	      if(!found)
		{
		  t->Type() = T->Type();
		  t->GlobalId() = i;
		  t->CopyParams(T);
		}
	      //              d_mols->AddTorsion(t);
              found++;
              Torsion *TN = T->Next();
              if((TN != NULL) &&
              ( (TN->A1() == a1 && TN->A2() == a2 && TN->A3() == a3 && TN->A4() == a4) ||
                (TN->A1() == a4 && TN->A2() == a3 && TN->A3() == a2 && TN->A4() == a1) ))

                {
                  Torsion *t2 = new Torsion();
                  i++; nt++;
                  t2->A1() = t->A1();             t2->A2() = t->A2();
                  t2->A3() = t->A3();             t2->A4() = t->A4();
		  t2->CopyParams(TN);
		  t2->GlobalId() = i;
		  t2->Type() = TN->Type();
                  if(et == NULL)
		    et = t2;
		  else
		    et->Append(t2);
                }
            }
          j++;
          T = T->Next();
        }
      if(!found)
        {
          j = 0;
          Torsion *TT = d_globs->Tors();
          while(TT != NULL)
            {
              if( (TT->A1() == 100 && TT->A4() == 100 && TT->A2() == a2 && TT->A3() == a3) ||
                  (TT->A1() == 100 && TT->A4() == 100 && TT->A3() == a2 && TT->A2() == a3) )
                {
		  if(!found)
		    {
		      t->Type() = TT->Type();
		      t->GlobalId() = i;
		      t->CopyParams(TT);
		    }
		  //                  d_mols->AddTorsion(t);
                  found++;
                  Torsion *TN = TT->Next();
                  if((TN != NULL) &&
                    ( (TN->A1() == 100 && TN->A4() == 100 && TN->A2() == a2 && TN->A3() == a3) ||
                      (TN->A1() == 100 && TN->A4() == 100 && TN->A3() == a2 && TN->A2() == a3) ))
                    {
                      Torsion *t2 = new Torsion();
                      i++; nt++;
                      t2->A1() = t->A1();                     t2->A2() = t->A2();
                      t2->A3() = t->A3();                     t2->A4() = t->A4();
                      if(et == NULL)
			et = t2;
		      else
			et->Append(t2);
                    }
                }
              j++;
              TT = TT->Next();
            }
        }
      assert(found>0);
      d_mols->AddTorsion(t);
      while(et != NULL)
	{
	  Torsion * tet = et->Next();
	  et->SetNext(NULL);
	  et->SetPrev(NULL);
	  d_mols->AddTorsion(et);
	  et = tet;
	}
    }

  LINEIN;  LINEIN;  LINEIN;
  int ni;
  BUFINT(ni);

  for(i=0;i<ni;i++)
    {
      ImproperTorsion *ls = new ImproperTorsion();
      SCANumberInt4(ls->A1(),ls->A2(),ls->A3(),ls->A4());
      ls->A1()--;ls->A2()--;ls->A3()--;ls->A4()--;
      ls->GlobalId() = d_mols->NImpropers();
      d_mols->AddImproperTorsion(ls);
    }

  return;
}

int MolecularSystem::DeriveElementTypes(Atom ** elems)
{
  // This function grows a list of atoms with unique masses
  Atom *ats = d_mols->Ats();
  Atom *ele = ats->Copy();
  int n = 1;

  ats = ats->Next();
  while(ats != NULL)
    {
      Atom *tele = ele;
      int found = 0;
      while(tele != NULL)
        {
          if(tele->Mass() == ats->Mass())
            {
              found = 1;
              break;
            }
          tele = tele->Next();
        }
      if(found)
        {
          Atom *nat = ats->Copy();
          ele->Append(nat);
          n++;
        }
      ats = ats->Next();
    }
  *elems = ele;
  return n;
}

void MolecularSystem::PushSiteSpec(char *name,char *type, double mass, double charge,
                  double eps,double sigma,double eps14,double sigma14)
{
  int k=0;
  static double rminSigmaFactor = 0.5*pow(2.0,1.0/6.0);
  if (HeaderHandler::instance()->getVDWCombine() == FF_OPLSAA)
    {
      sigma = sigma * rminSigmaFactor;
      sigma14 = sigma14 * rminSigmaFactor;
    }

  Atom *at = new Atom();
  Atom *gat = new Atom();
  Atom *tat;

  strcpy(gat->Name(),name);
  strcpy(at->Name(),name);
  gat->Type() = d_globs->NAtoms();
  if(gat->NewType(d_globs->d_ats,(List**)&tat))
    {
      gat->Mass()       = mass;
      gat->Charge()     = charge;

      gat->LocalId()    = d_globs->NAtoms();
      gat->GlobalId()   = d_globs->NAtoms();
      gat->Type()       = d_globs->NAtoms();
      gat->Eps()        = eps;
      gat->RMin()       = sigma;
      gat->Eps14()      = eps14;
      gat->RMin14()     = sigma14;
      d_globs->AddAtom(gat);

      at->Type() = gat->Type();
    }
  else
    {
      at->Type() = tat->Type();
    }

  at->Mass()       = mass;
  at->Charge()     = charge;
  at->Eps()        = eps;
  at->RMin()       = sigma;
  at->Eps14()      = eps14;
  at->RMin14()     = sigma14;
  at->LocalId()    = d_mols->NAtoms();
  at->GlobalId()   = d_mols->NAtoms();
  d_mols->AddAtom(at);
}


void MolecularSystem::PushBondSpec(char *s1, char *s2, double k0,double r0)
{
  int k;
  Atom *at,*tat;
  Bond * bd = new Bond();
  Bond * gbd = new Bond();

  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  assert(d_mols->LocateAtomName(s1,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  bd->A1() = at->GlobalId();
  gbd->A1() = at->Type();

  assert(the_MolecularSystem.d_mols->LocateAtomName(s2,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  bd->A2() = at->GlobalId();
  gbd->A2() = at->Type();

  bd->K0() = gbd->K0() = k0;
  bd->Eq() = gbd->Eq() = r0;

  bd->Type() = gbd->Type() = d_mols->NBonds();
  bd->GlobalId() = gbd->GlobalId() = d_mols->NBonds();

  d_mols->AddBond(bd);
  d_globs->AddBond(gbd);
}



void MolecularSystem::PushAngleSpec(char *s1, char *s2, char *s3, double k0,double r0, double ku, double ru)
{
  int k;

  Atom *at,*tat;
  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  Angle * ag = new Angle();
  Angle * gag = new Angle();

  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  assert(d_mols->LocateAtomName(s1,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A1() = at->GlobalId();
  gag->A1() = at->Type();

  assert(d_mols->LocateAtomName(s2,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A2() = at->GlobalId();
  gag->A2() = at->Type();

  assert(d_mols->LocateAtomName(s3,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A3() = at->GlobalId();
  gag->A3() = at->Type();

  ag->K0() = gag->K0() = k0;
  ag->Eq() = gag->Eq() = r0;
  ag->d_ubk0 = gag->UBK0() = ku;
  ag->d_ubeq = gag->UBEq() = ru;

  ag->Type() = gag->Type() = d_mols->NAngles();
  ag->GlobalId() = gag->GlobalId() = d_mols->NAngles();

  d_mols->AddAngle(ag);
  d_globs->AddAngle(gag);
}



void MolecularSystem::PushTorsionSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
{
  int k;
  Atom *at,*tat;

  Torsion * ag = new Torsion();
  Torsion * gag = new Torsion();

  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  assert(d_mols->LocateAtomName(s1,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A1() = at->GlobalId();
  gag->A1() = at->Type();
  int a1 = at->Type();

  assert(d_mols->LocateAtomName(s2,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A2() = at->GlobalId();
  gag->A2() = at->Type();
  int a2 = at->Type();

  assert(d_mols->LocateAtomName(s3,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A3() = at->GlobalId();
  gag->A3() = at->Type();
  int a3 = at->Type();

  assert(d_mols->LocateAtomName(s4,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A4() = at->GlobalId();
  gag->A4() = at->Type();
  int a4 = at->Type();

  ag->K0() = gag->K0() = k0;
  ag->Eq() = gag->Eq() = r0;
  ag->Wells() = gag->Wells() = (double)wells;

  ag->Type() = gag->Type() = d_mols->NTorsions();
  ag->GlobalId() = gag->GlobalId() = d_mols->NTorsions();

  d_mols->AddTorsion(ag);
  d_globs->AddTorsion(gag);


  // *************   Corresponds to logic in parameter file for multiple terms

  static int count= 0;
  static int la1 = -1;
  static int la2 = -1;
  static int la3 = -1;
  static int la4 = -1;


  if(a1 == la1 && a2 == la2 && a3 == la3 && a4 == la4)
    {
      count++;
    }
  else
    {
      count = 0;
    }
  //  printf("########## NT,Nt = %d , %d COUNT = %d",NT,Nt,count);
  //   printf(" TTypeMap[%d] = %d  j = %d\n",Nt,TTypeMap[Nt],j);
  //fflush(stdout);

  la1 = a1;
  la2 = a2;
  la3 = a3;
  la4 = a4;

}

void MolecularSystem::PushImproperSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
{
  Atom *at,*tat;
  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  ImproperTorsion * ag = new ImproperTorsion();
  ImproperTorsion * gag = new ImproperTorsion();

  assert(d_mols->LocateAtomName(s1,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A1() = at->GlobalId();
  gag->A1() = at->Type();

  assert(d_mols->LocateAtomName(s2,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A2() = at->GlobalId();
  gag->A2() = at->Type();

  assert(d_mols->LocateAtomName(s3,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A3() = at->GlobalId();
  gag->A3() = at->Type();

  assert(d_mols->LocateAtomName(s4,&tat));
  List::LocateItem(d_globs->d_ats,*tat,(List **)&at);
  ag->A4() = at->GlobalId();
  gag->A4() = at->Type();

  ag->K0() = gag->K0() = k0;
  ag->Eq() = gag->Eq() = r0;
  ag->Wells() = gag->Wells() = wells;

  ag->Type() = gag->Type() = d_mols->d_nimps;
  ag->GlobalId() = gag->GlobalId() = d_mols->d_nimps;

  d_mols->AddImproperTorsion(ag);
  d_globs->AddImproperTorsion(gag);

}


int MolecularSystem::SetHeaderStrings(FragSpec *fs)
{
  fs->SetBondTermName(BondTermName());
  fs->SetAngleTermName(AngleTermName());
  fs->SetUreyTermName(UreyTermName()) ;
  fs->SetTorsionTermName(TorsionTermName());
  fs->SetImproperTermName(ImproperTermName());

  fs->SetBondStructName(BondStructName());
  fs->SetAngleStructName(AngleStructName());
  fs->SetUreyStructName(UreyStructName()) ;
  fs->SetTorsionStructName(TorsionStructName());
  fs->SetImproperStructName(ImproperStructName());


  return 1;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//                    Output methods
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


int MolecularSystem::PrintHeaderFile(char *bufS,int  maxlen)
{
  FragSpec *fs = d_redFragSpecs;
  char * s = bufS;
  int len = 0;


  char tbuf[1024 + 256];

  sprintf(tbuf,"%s\n",BondStructName());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"%s\n",AngleStructName());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"%s\n",UreyStructName());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"%s\n",TorsionStructName());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"%s\n",ImproperStructName());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);


  sprintf(tbuf,"\nenum {NumberOfFragmentTypes = %d};\n",d_nRedFrags);
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"\nenum {NumberOfFragments = %d};\n",d_nFrags);
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"\nenum {NumberOfSiteTypes = %d};\n",d_mols->NATypes());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"\nenum {NumberOfSites = %d};\n",d_mols->NAtoms());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"\nenum {MaxNumberOfInternalSites = %d};\n",d_maxFrag);
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"\nenum {NumberOfLJPairs = %d};\n",d_mols->NPairs());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);
  sprintf(tbuf,"\nenum {NumberOfLJ14Pairs = %d};\n",d_mols->N14Pairs());
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);

  /////////////////////////////////////////////////////////////
  //             Begin Fragment Info
  /////////////////////////////////////////////////////////////
  while(fs != NULL)
    {
      SetHeaderStrings(fs);
      fs->PrintHppInfo(s);s += strlen(s);
      fs = fs->Next();
    }

  PrintFragmentInstanceList(s,MAXBUFLEN); s += strlen(s);
  len = s - bufS; assert(len < maxlen);
  PrintFragmentNeighborTable(s,MAXBUFLEN); s += strlen(s);
  len = s - bufS; assert(len < maxlen);
  PrintExternalSiteTable(s,MAXBUFLEN); s += strlen(s);
  len = s - bufS; assert(len < maxlen);
  PrintSiteSpecTable(s,MAXBUFLEN); s += strlen(s);
  len = s - bufS; assert(len < maxlen);
  PrintLJPairTable(s,MAXBUFLEN); s += strlen(s);
  len = s - bufS; assert(len < maxlen);
  PrintLJ14PairTable(s,MAXBUFLEN); s += strlen(s);
  len = s - bufS; assert(len < maxlen);
  PrintFragSpecList(s,MAXBUFLEN); s += strlen(s);
  len = s - bufS; assert(len < maxlen);


  //////////////////////////////////////////////////////
  /////////////   This forcefield type stuff is getting axed!!!!
  /////////////
  sprintf(tbuf,"\nenum {ForceFieldId = 0};\n");
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);

  sprintf(tbuf,"\nenum {VDWCombine = 0};\n");
  strcat(s,tbuf);
  s += strlen(s);  len = s - bufS; assert(len < maxlen);

  return 1;
}
/////////////////////////////////////////////////////////////
//             Begin Fragment Instance List
/////////////////////////////////////////////////////////////
int  MolecularSystem::PrintFragmentInstanceList(char *fPF,int maxlen)
{
  char bufS[1024 * 32],tbufS[256];
  int c = 0,a = 0,maxfrag=0;
  OPEN_BLOCK("const FragmentInstanceListItem FragmentInstanceList[]");
  FragSpec * fs = d_fragSpecs;
  int nn,base = 0;
  while(fs != NULL)
    {
      nn = fs->NumberFragmentNeighbors;
      if(fs->NumberExternalSites > 0)
        {BLOCK5(fs->Type(),a,c,base,nn); }
      else
        {BLOCK5(fs->Type(),a,-1,base,nn); }
      a += fs->NumberInternalSites;
      c += fs->NumberExternalSites;
      base += nn;
      if(fs->Next() != NULL)
        {COMA; LINE; TAB;}
      fs = fs->Next();
    }
  END_BLOCK;
  return 1;
}
/////////////////////////////////////////////////////////////
//             Begin External Site Table
/////////////////////////////////////////////////////////////

int MolecularSystem::PrintExternalSiteTable(char *fPF,int maxlen)
{
  FragSpec *fs = d_fragSpecs;
  char bufS[256],tbufS[256];
  int c,flg = 0;

  c = 0;
  while(fs != NULL)
    {
      c += fs->NumberExternalSites;
      fs = fs->Next();
    }
  strcpy(bufS,"const int ExternalSiteTable[]");
  if(c  > 0)
    {
      fs = d_fragSpecs;
      OPEN_BLOCK(bufS);
      while(fs != NULL)
        {
          Atom *at = fs->ExternalSites;
          while(at != NULL)
            {
              if(flg)
                {COMA; LINE; TAB; }
              else
                {flg = 1;}
              BLOCK1(at->GlobalId());
              at = at->Next();
            }
          fs = fs->Next();
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = {0xdeadbeaf};\n\n");
      STRNG(bufS);
    }
  return 1;
}
/////////////////////////////////////////////////////////////
//             Begin FragSpec Neighbor Table
/////////////////////////////////////////////////////////////

int MolecularSystem::PrintFragmentNeighborTable(char *fPF,int maxlen)
{
  FragSpec *fs = d_fragSpecs;
  char bufS[256],tbufS[256];
  int c,i,flg = 0;

  c = 0;
  while(fs != NULL)
    {
      c += fs->NumberFragmentNeighbors;
      fs = fs->Next();
    }

  sprintf(tbufS,"\nenum {NumberOfFragmentNeighbors = %d};\n\n",c);
  strcat(fPF,tbufS);

  strcpy(bufS,"const int FragmentNeighborList[]");
  if(c  > 0)
    {
      fs = d_fragSpecs;

      OPEN_BLOCK(bufS);
      while(fs != NULL)
        {
          List *nl  = fs->FragmentNeighborList;
          while(nl != NULL)
            {
              BLOCK1(nl->GlobalId());
              if(nl->Next() != NULL)
                {COMA; TAB; }
              nl = nl->Next();
            }
          if(fs->Next() != NULL)
            {COMA; LINE; TAB; }
          fs = fs->Next();
        }
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = {0xdeadbeaf};\n\n");
      STRNG(bufS);
    }
  return 1;
}
/////////////////////////////////////////////////////////////
//             Begin site spec table
/////////////////////////////////////////////////////////////
int MolecularSystem::PrintSiteSpecTable(char *fPF,int maxlen)
{
  char bufS[1024 * 32],tbufS[256];
  Atom *at = d_mols->ATypes();
  OPEN_BLOCK("const SiteSpec SiteSpecTable[]");
  while(at != NULL)
    {
//        if (HeaderHandler::instance()->getForceFieldType() == 0)
//      {
//        SITESPEC(at->Mass(),at->Charge(),at->Eps(),at->RMin(),
//                 at->Eps14(),at->RMin14());
//      }
//        else if (HeaderHandler::instance()->getForceFieldType() == 1)
//      {
//        OPLSSITESPEC(at->Mass(),at->Charge(),at->Eps(),at->RMin(),
//                 at->Eps14(),at->RMin14());
//      }
//        else
        {
          //default
          SITESPEC(at->Mass(),at->Charge(),at->Eps(),at->RMin(),
                   at->Eps14(),at->RMin14());
        }
      if(at->Next() != NULL){COMA; LINE; TAB;}
      at = at->Next();
    }
  END_BLOCK;
  return 1;
}
/////////////////////////////////////////////////////////////
//             Begin LJ Pair table
/////////////////////////////////////////////////////////////
int MolecularSystem::PrintLJPairTable(char *fPF,int maxlen)
{
  char bufS[1024 * 32],tbufS[256];
  Atom *at = d_mols->APairs();
  OPEN_BLOCK("const LennardJonesParameters CombinedLennardJonesTable[]");
  while(at != NULL)
    {
	//  	strstream a1SS; a1SS.precision(32); a1SS << at->Eps() << endl;           
//  		strstream a2SS; a2SS.precision(32); a2SS << at->RMin() << endl;           
//  		char *a1S = a1SS.str();                                           
//  		char *a2S = a2SS.str();                                           
		sprintf(bufS,"{  %32.15lf  ,  %32.15lf  }",at->Eps(),at->RMin());strcat(fPF,bufS);         
		//		delete a1S, a2S;                                                  

      //      LJSPEC(at->Eps(),at->RMin());
      if(at->Next() != NULL){COMA; LINE; TAB;}
      at = at->Next();
    }
  END_BLOCK;
  return 1;
}
/////////////////////////////////////////////////////////////
//             Begin LJ 14 Pair table
/////////////////////////////////////////////////////////////
int MolecularSystem::PrintLJ14PairTable(char *fPF,int maxlen)
{
  char bufS[1024 * 32],tbufS[256];
  Atom *at = d_mols->A14Pairs();
  OPEN_BLOCK("const LennardJonesParameters CombinedLennardJones14Table[]");
  while(at != NULL)
    {
      //      LJ14SPEC(at->Eps14(),at->RMin14());
      sprintf(bufS,"{  %32.15lf  ,  %32.15lf  }",at->Eps14(),at->RMin14());strcat(fPF,bufS);         
      if(at->Next() != NULL){COMA; LINE; TAB;}
      at = at->Next();
    }
  END_BLOCK;
  return 1;
}



 void WriteDataTransferBlock(const char *msdVarName,const char *elemType,const char *nElements, const char* fsVarName, char *fPF)
   {
     char bufS[256];
     sprintf(bufS,"%s  = (%s *) &( BigBuffa[ NextOffset ] );\n",msdVarName,elemType);
     STRNG(bufS);
     sprintf(bufS,"BlockSize = %s * sizeof( %s );\n",nElements,elemType);
     STRNG(bufS);
     sprintf(bufS,"BegLogLine(1) << \"%s at Offset \" << NextOffset << \" Bytes \" << BlockSize << EndLogLine;\n",msdVarName);
     STRNG(bufS);
     sprintf(bufS,"memcpy( %s,  %s, BlockSize );\n",msdVarName,fsVarName);
     STRNG(bufS);
     sprintf(bufS,"NextOffset += BlockSize;\n");
     STRNG(bufS);
     sprintf(bufS,"NextOffset=round8(NextOffset);\n\n");
     STRNG(bufS);
     return;
   }

  /////////////////////////////////////////////////////////////
  //             Fragspec List
  /////////////////////////////////////////////////////////////
int MolecularSystem::PrintFragSpecList(char *fPF,int maxlen)
{
  int ff = 0;
  char name[256];
  char bufS[256],tbufS[256];
  // Free Fragspec def
  OPEN_CLASS("class FragSpec ");
  STRNG("public:                "); LINE;
  STRNG(" const int           NumberInternalSites; "); LINE;
  STRNG(" const int           NumberExternalSites; "); LINE;
  STRNG(" const int           *InternalSiteSpecTableIndex; "); LINE;
  STRNG(" const int           *ExternalSiteSpecTableIndex; "); LINE;

  STRNG(" const int           NumberBondTypes; "); LINE;
  //  STRNG(" const BondType      *BondTypeTable; "); LINE;
  STRNG(" const int           NumberInternalBonds;  "); LINE;
  STRNG(" const int           NumberExternalBonds;  "); LINE;
  STRNG(" const Bond          *InternalBonds;  "); LINE;
  STRNG(" const Bond          *ExternalBonds;  "); LINE;

  STRNG(" const int           NumberAngleTypes;  "); LINE;
  //  STRNG(" const AngleType     *AngleTypeTable;  "); LINE;
  //  STRNG(" const AngleType     *UbTypeTable;  "); LINE;
  STRNG(" const int           NumberInternalAngles;   "); LINE;
  STRNG(" const int           NumberExternalAngles;   "); LINE;
  STRNG(" const Angle         *InternalAngles; "); LINE;
  STRNG(" const Angle         *ExternalAngles; "); LINE;

  STRNG(" const int           NumberTorsionTypes;   "); LINE;
  //  STRNG(" const TorsionType   *TorsionTypeTable;  "); LINE;
  STRNG(" const int           NumberInternalTorsions;   "); LINE;
  STRNG(" const int           NumberExternalTorsions;   "); LINE;
  STRNG(" const Torsion       *InternalTorsions;"); LINE;
  STRNG(" const Torsion       *ExternalTorsions;"); LINE;

  STRNG(" const int           NumberImproperTypes;   "); LINE;
  //  STRNG(" const ImproperType  *ImproperTypeTable;  "); LINE;
  STRNG(" const int           NumberInternalImpropers;   "); LINE;
  STRNG(" const int           NumberExternalImpropers;   "); LINE;
  STRNG(" const Improper      *InternalImpropers;"); LINE;
  STRNG(" const Improper      *ExternalImpropers;"); LINE;
  STRNG(" const int           NumberExcludedSitePairs;"); LINE;
  STRNG(" const SiteId        *ExcludedSitePairs;"); LINE;
  STRNG(" const int           NumberPair14SitePairs;"); LINE;
  STRNG(" const SiteId        *Pair14SitePairs;"); LINE;
  STRNG(" const int           NumberTerms;"); LINE;
  STRNG(" const Term          *TermList;"); LINE;
  STRNG(" const int           NumberCodes;"); LINE;
  STRNG(" const Code          *CodeList;"); LINE;
  STRNG(" const int           NumberFragmentNeighbors;"); LINE;
  STRNG(" const int           *FragmentNeighborList;"); LINE;
  sprintf(bufS,"const %s          *BondTypeTable;\n",BondTermName());
  STRNG(bufS);
  sprintf(bufS,"const %s          *AngleTypeTable;\n",AngleTermName());
  STRNG(bufS);
  sprintf(bufS,"const %s          *UreyTypeTable;\n",UreyTermName());
  STRNG(bufS);
  sprintf(bufS,"const %s          *TorsionTypeTable;\n",TorsionTermName());
  STRNG(bufS);
  sprintf(bufS,"const %s          *ImproperTypeTable;\n",ImproperTermName());
  STRNG(bufS);

  END_BLOCK;

  // Fragspec Table prep

  OPEN_BLOCK("const FragSpec FragSpecTable[]");

  FragSpec *rfs = d_redFragSpecs;
  while(rfs != NULL)
    {
      sprintf(name,"FragSpec_%d",rfs->LocalId());

      STRNG("{\n\t\t");

      sprintf(bufS,"%s::%s",name,"NumberInternalSites");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalSites");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalSiteSpecTableIndex");

      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalSiteSpecTableIndex");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberBondTypes");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;


      sprintf(bufS,"%s::%s",name,"NumberInternalBonds");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalBonds");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;


      sprintf(bufS,"%s::%s",name,"InternalBonds");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalBonds");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberAngleTypes");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberInternalAngles");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalAngles");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalAngles");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalAngles");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberTorsionTypes");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberInternalTorsions");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalTorsions");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalTorsions");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalTorsions");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberImproperTypes");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberInternalImpropers");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalImpropers");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalImpropers");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalImpropers");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExcludedSitePairs");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExcludedSitePairs");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberPair14SitePairs");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"Pair14SitePairs");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberTerms");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"TermList");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberCodes");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"CodeList");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberFragmentNeighbors");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"FragmentNeighborList");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s_Table",name,BondTermName());
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s_Table",name,AngleTermName());
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s_Table",name,UreyTermName());
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s_Table",name,TorsionTermName());
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s_Table",name,ImproperTermName());
      STRNG(bufS);      COMA; LINE;  TAB; TAB;


      STRNG("\n\t}");

      if (rfs->Next() != NULL)
        {COMA; LINE; TAB;}
      rfs = rfs->Next();
    }
  END_BLOCK;

  /////////////////////// 
  // MSDSuffix

   STRNG("char BigBuffa[ 16 * 1024 * 1024 ];\n"); LINE;
   STRNG("int  NextOffset = 0;\n");LINE;
   STRNG("int  BlockSize;\n");LINE;
   STRNG("\n");
   STRNG("static int roundup(int x, int alignment)\n");
   STRNG("{\n");
   STRNG("   return ( (x+(alignment-1)) / alignment ) * alignment ;\n");
   STRNG("}\n");
   STRNG("\n");
   STRNG("static int round8(int x)\n");
   STRNG("{\n");
   STRNG("   return roundup(x,8) ;\n");
   STRNG("}\n");


   STRNG("int main(int, char**, char**)");LINE;
   STRNG("  {");LINE;
   STRNG("  BegLogLine(1)");LINE;
   STRNG("    << \"Main \" ");LINE;
   STRNG("    << EndLogLine;");LINE;LINE;
   
   STRNG("  MolSysDef *msd = (MolSysDef *) BigBuffa;");LINE;
   STRNG("  NextOffset += sizeof( MolSysDef );");LINE;
   STRNG("  NextOffset=round8(NextOffset) ;");LINE;
   STRNG("  BegLogLine(1) << \"msd NextOffset \" << NextOffset <<  EndLogLine;");LINE;
   
   STRNG("  msd->NumberOfSites             = NumberOfSites;");LINE;
   STRNG("  msd->NumberOfFragmentInstances = NumberOfFragments;");LINE;
   STRNG("  msd->NumberOfFragmentTypes     = NumberOfFragmentTypes;");LINE;
   STRNG("  msd->NumberFragmentNeighbors   = sizeof(FragmentNeighborList) / sizeof(int);");LINE;
   
   STRNG("  msd->NumberOfSiteSpecs         = NumberOfSiteTypes;");LINE;

   char elembuf[30];
   
   WriteDataTransferBlock("msd->FragmentInstanceList","FragmentInstanceListItem ", "NumberOfFragments" ,"FragmentInstanceList",fPF);
   WriteDataTransferBlock("msd->ExternalSiteTable","int ","NumberOfSites","ExternalSiteTable",fPF);
   WriteDataTransferBlock("msd->SiteSpecTable","SiteSpec ","NumberOfSites","SiteSpecTable",fPF);
   WriteDataTransferBlock("msd->FragmentNeighborList","int ","NumberOfFragmentNeighbors", "FragmentNeighborList",fPF);
   WriteDataTransferBlock("msd->FragSpecTable","MolSysDef::FragSpec ", "NumberOfFragments","FragSpecTable",fPF);
   int natypes = d_mols->NATypes();
   int nljt = (1 + natypes) * natypes / 2;
   sprintf(elembuf,"%d",nljt);
   WriteDataTransferBlock("msd->mCombinedLennardJonesParameters","LennardJonesParameters ",elembuf,"CombinedLennardJonesTable",fPF);
   WriteDataTransferBlock("msd->mCombinedLennardJones14Parameters","LennardJonesParameters ",elembuf,"CombinedLennardJones14Table",fPF);
   
   STRNG("  for(int i = 0; i < NumberOfFragmentTypes; i++)"); LINE;
   STRNG("  {");LINE;
   STRNG("     MolSysDef::FragSpec *xfrag = &( msd->FragSpecTable[ i ] );");LINE;
   STRNG("     const FragSpec  *pfrag = &( FragSpecTable[ i ] );");LINE;
   STRNG("  ");LINE;
   STRNG("      BegLogLine(1)");LINE;
   STRNG("      << \"FragNo \" << i");LINE;
   STRNG("      << \"To &(FragSpecTable[ i ]) \" << xfrag");LINE;
   STRNG("      << \"From &(FragSpecTable[ i ]) \" << pfrag");LINE;
   STRNG("      << EndLogLine;");LINE;
   STRNG(" ");LINE;
   STRNG("      xfrag->NumberInternalSites            = pfrag->NumberInternalSites;");LINE;
   STRNG("      xfrag->NumberCodes                    = pfrag->NumberCodes;");LINE;
   STRNG("      xfrag->NumberExcludedSitePairs        = pfrag->NumberExcludedSitePairs;");LINE;
   STRNG("      xfrag->NumberPair14SitePairs          = pfrag->NumberPair14SitePairs;");LINE;
   
   WriteDataTransferBlock("xfrag->InternalSiteSpecTableIndex","int","pfrag->NumberInternalSites","pfrag->InternalSiteSpecTableIndex",fPF);
   WriteDataTransferBlock("xfrag->ExternalSiteSpecTableIndex","int","pfrag->NumberExternalSites","pfrag->ExternalSiteSpecTableIndex",fPF);
   WriteDataTransferBlock("xfrag->CodeList","MDVM_Code ","(pfrag->NumberCodes + 1)","pfrag->CodeList",fPF);

   char tempbuf[256];
   sprintf(tempbuf,"UDF_Registry::%s",BondTermName());
   WriteDataTransferBlock("xfrag->BondTypeTable",tempbuf,"pfrag->NumberBondTypes","pfrag->BondTypeTable",fPF);
   sprintf(tempbuf,"UDF_Registry::%s",AngleTermName());
   WriteDataTransferBlock("xfrag->AngleTypeTable",tempbuf,"pfrag->NumberAngleTypes","pfrag->AngleTypeTable",fPF);
   sprintf(tempbuf,"UDF_Registry::%s",UreyTermName());
   WriteDataTransferBlock("xfrag->UreyTypeTable",tempbuf,"pfrag->NumberAngleTypes","pfrag->UreyTypeTable",fPF);   
   sprintf(tempbuf,"UDF_Registry::%s",TorsionTermName());
   WriteDataTransferBlock("xfrag->TorsionTypeTable",tempbuf,"pfrag->NumberTorsionTypes","pfrag->TorsionTypeTable",fPF);
   sprintf(tempbuf,"UDF_Registry::%s",ImproperTermName());
   WriteDataTransferBlock("xfrag->ImproperTypeTable",tempbuf,  "pfrag->NumberImproperTypes","pfrag->ImproperTypeTable",fPF);

   WriteDataTransferBlock("xfrag->ExcludedSitePairs","SiteRef ","pfrag->NumberExcludedSitePairs","pfrag->ExcludedSitePairs",fPF);
   WriteDataTransferBlock("xfrag->Pair14SitePairs","SiteRef","pfrag->NumberPair14SitePairs","pfrag->Pair14SitePairs",fPF);
   WriteDataTransferBlock("xfrag->FragmentNeighborList","int ","pfrag->NumberFragmentNeighbors","pfrag->FragmentNeighborList",fPF);
   
   
   STRNG("    }");LINE;
   STRNG("  msd->TotalByteSize = NextOffset;");LINE;
   //   STRNG("  msd->VDWCombine  = VDWCombine;");LINE;
   STRNG("  msd->ConvertPointersToOffsets();");LINE;
   STRNG("  int fp = open( \"output.msd\", O_WRONLY | O_CREAT, 0666 );");LINE;
   STRNG("  write( fp, BigBuffa, NextOffset );");LINE;
   STRNG("  close( fp );");LINE;
   STRNG("  return(NULL);");LINE;
   STRNG(" }");LINE;
   
   return 1;
}

//   msd->ExternalSiteTable = (int *) &( BigBuffa[ NextOffset ] );
//   BlockSize = sizeof( ExternalSiteTable );
//   BegLogLine(1) << "msd->ExternalSiteTable at Offset " << NextOffset << " Bytes " << BlockSize << EndLogLine;
//   memcpy( msd->ExternalSiteTable,  ExternalSiteTable, BlockSize );
//   NextOffset += BlockSize;
//   NextOffset=round8(NextOffset) ;

//   msd->SiteSpecTable = (SiteSpec *) &(BigBuffa[ NextOffset ] );
//   BlockSize = sizeof( SiteSpecTable );
//   BegLogLine(1) << "msd->SiteSpecTable NextOffset " << NextOffset << " Bytes " << BlockSize<<  EndLogLine;
//   memcpy( msd->SiteSpecTable, SiteSpecTable, BlockSize );
//   NextOffset += BlockSize;
//   NextOffset=round8(NextOffset) ;

//   msd->FragmentInstanceList = (FragmentInstanceListItem *) &(BigBuffa[ NextOffset ] );
//   BlockSize = sizeof( FragmentInstanceList );
//   BegLogLine(1) << "msd->FragmentInstanceList NextOffset " << NextOffset << " Bytes " << BlockSize<<  EndLogLine;
//   memcpy( msd->FragmentInstanceList, FragmentInstanceList, BlockSize );
//   NextOffset += BlockSize;
//   NextOffset=round8(NextOffset) ;

//   msd->FragmentNeighborList = (int *) &(BigBuffa[ NextOffset ] );
//   BlockSize = NumberOfFragmentNeighbors * sizeof (int);
//   BegLogLine(1) << "msd->FragmentNeighborList NextOffset " << NextOffset << " Bytes " << BlockSize<<  EndLogLine;
//   memcpy( msd->FragmentNeighborList, FragmentNeighborList, BlockSize );
//   NextOffset += BlockSize;
//   NextOffset=round8(NextOffset) ;

//   msd->FragSpecTable = ( MolSysDef::FragSpec * ) &( BigBuffa[ NextOffset ] );
//   BlockSize = NumberOfFragments * sizeof( MolSysDef::FragSpec );
//   BegLogLine(1) << "msd->FragSpecTable NextOffset " << NextOffset << " Bytes " << BlockSize <<  EndLogLine;
//   memcpy( msd->FragSpecTable, FragSpecTable, BlockSize );
//   NextOffset += BlockSize;
//   NextOffset=round8(NextOffset) ;

//   msd->mCombinedLennardJonesParameters = ( LennardJonesParameters * ) &( BigBuffa[ NextOffset ] );
//   BlockSize = (1 + NumberOfSiteTypes) * NumberOfSiteTypes / 2  * sizeof( LennardJonesParameters );
//   BegLogLine(1) << "msd->mCombinedLennardJonesParameters NextOffset " << NextOffset << " Bytes " << BlockSize <<  EndLogLine;
//   memcpy( msd->mCombinedLennardJonesParameters, CombinedLennardJonesTable, BlockSize );
//   NextOffset += BlockSize;
//   NextOffset=round8(NextOffset) ;



//   msd->mCombinedLennardJones14Parameters = ( LennardJonesParameters * ) &( BigBuffa[ NextOffset ] );
//   BlockSize = (1 + NumberOfSiteTypes) * NumberOfSiteTypes / 2 * sizeof( LennardJonesParameters );

//     xfrag->InternalSiteSpecTableIndex     = (int *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberInternalSites * sizeof( int ) ;
//     BegLogLine(1) << "msd->FragSpec->InternalSiteSpecTable NextOffset " << NextOffset << " Bytes " << BlockSize <<  EndLogLine;
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;
//     memcpy( xfrag->InternalSiteSpecTableIndex, pfrag->InternalSiteSpecTableIndex, BlockSize );

//     xfrag->ExternalSiteSpecTableIndex     = (int *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberExternalSites * sizeof( int ) ;
//     BegLogLine(1) << "msd->FragSpec->ExternalSiteSpecTable NextOffset " << NextOffset << " Bytes " << BlockSize <<  EndLogLine;
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;
//     memcpy( xfrag->ExternalSiteSpecTableIndex, pfrag->ExternalSiteSpecTableIndex, BlockSize );


//     xfrag->CodeList     = (MDVM_Code *) &( BigBuffa[ NextOffset ] );
//     printf("Mike, make the NumberCodes value include the Terminator code\n");
//     BlockSize = (pfrag->NumberCodes + 1 ) * sizeof( MDVM_Code )  ;
//     BegLogLine(1) << "msd->FragSpec->CodeList NextOffset " << NextOffset  << " Bytes " << BlockSize <<  EndLogLine;
//     memcpy( xfrag->CodeList, pfrag->CodeList, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;

//     xfrag->BondTypeTable     = (BondType *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberBondTypes * sizeof( BondType ) ;
//     BegLogLine(1) << "msd->FragSpec->BondTypeTable NextOffset " << NextOffset  << " Bytes " << BlockSize <<  EndLogLine;
//     memcpy( xfrag->BondTypeTable, pfrag->BondTypeTable, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;

//     xfrag->AngleTypeTable     = (AngleType *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberAngleTypes * sizeof( AngleType ) ;
//     BegLogLine(1) << "msd->FragSpec->AngleTypeTable NextOffset " << NextOffset << " Bytes " << BlockSize  <<  EndLogLine;
//     memcpy( xfrag->AngleTypeTable, pfrag->AngleTypeTable, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;


//     xfrag->UbTypeTable     = (AngleType *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberAngleTypes * sizeof( AngleType ) ;
//     BegLogLine(1) << "msd->FragSpec->UbTypeTable NextOffset " << NextOffset << " Bytes " << BlockSize  <<  EndLogLine;
//     memcpy( xfrag->UbTypeTable, pfrag->UbTypeTable, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;


//     xfrag->TorsionTypeTable     = (TorsionType *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberTorsionTypes * sizeof( TorsionType ) ;
//     BegLogLine(1) << "msd->FragSpec->TorsionTypeTable NextOffset " << NextOffset  << " Bytes " << BlockSize <<  EndLogLine;
//     memcpy( xfrag->TorsionTypeTable, pfrag->TorsionTypeTable, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;

//     xfrag->ImproperTypeTable     = (ImproperType *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberImproperTypes * sizeof( ImproperType ) ;
//     BegLogLine(1) << "msd->FragSpec->ImproperTypeTable NextOffset " << NextOffset  << " Bytes " << BlockSize <<  EndLogLine;
//     memcpy( xfrag->ImproperTypeTable, pfrag->ImproperTypeTable, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;


//     xfrag->ExcludedSitePairs     = (SiteRef *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberExcludedSitePairs * sizeof( SiteRef ) ;
//     BegLogLine(1) << "msd->FragSpec->ExcludedSitePairs NextOffset " << NextOffset  << " Bytes " << BlockSize <<  EndLogLine;
//     memcpy( xfrag->ExcludedSitePairs, pfrag->ExcludedSitePairs, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;

//     xfrag->Pair14SitePairs     = (SiteRef *) &( BigBuffa[ NextOffset ] );
//     BlockSize = pfrag->NumberPair14SitePairs * sizeof( SiteRef ) ;
//     BegLogLine(1) << "msd->FragSpec->Pair14SitePairs NextOffset " << NextOffset  << " Bytes " << BlockSize <<  EndLogLine;
//     memcpy( xfrag->Pair14SitePairs, pfrag->Pair14SitePairs, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;

//     xfrag->FragmentNeighborList     = (int *) &( BigBuffa[ NextOffset ] );
// //NEED: to think about this getting this info here rather than the frag instance table!!!
//     BlockSize = pfrag->NumberFragmentNeighbors * sizeof( int ) ;
//     BegLogLine(1) << "msd->FragSpec->FragNeighborList Count " << BlockSize / sizeof(int) << " NextOffset " << NextOffset  << " Bytes " << BlockSize <<  EndLogLine;
//     memcpy( xfrag->FragmentNeighborList, pfrag->FragmentNeighborList, BlockSize );
//     NextOffset += BlockSize;
//     NextOffset=round8(NextOffset) ;
//     //#endif


int MolecularSystem::PrintXMLDescription(char *bufS,int maxlen)
{
  char *bufPS = bufS, tbufS[1024];
  char fPF[1024*1024];
  int i;
  char tstrS[10];
  bufS[0] = '\0';
  Atom *at;
  Bond *bd;
  Angle *ang;
  Torsion *tor;
  ImproperTorsion *imp;

  // first write out DTD:
#define ST(x) {sprintf(bufPS, "%s", x ); strcat(bufPS,"\n"); bufPS += strlen( x ) + 1;}
  ST("<?xml version='1.0' encoding='US-ASCII' standalone='no' ?>");
  ST("<!DOCTYPE ffparams [");
  ST("");
  ST("<!ELEMENT ffparams (header_parameter_list, (data_list)+,");
  ST(" (force_list)*, (pair_list)*,");
  ST("(group_list)*)>");
  ST(" ");
  ST("<!-- The header parameter list contains per system information.	 Each -->");
  ST("<!-- parameter has associated name and type attributes -->");
  ST("<!ELEMENT header_parameter_list (header_parameter)+>");
  ST("<!ELEMENT header_parameter (#PCDATA)>");
  ST("<!ATTLIST header_parameter param_id ID #REQUIRED>");
  ST("<!ATTLIST header_parameter type (char | double | int) \"char\">");


  ST("<!-- The data_list contains a list of type_ids that are used -->");
  ST("<!-- for a variety of purposes, including molecule types and molecule -->");
  ST("<!-- instances as well as LJ types.   These types may have -->");
  ST("<!-- parameter values associated with them which are defined in the -->");
  ST("<!-- type_parameter_list. Information about what type_id is -->");
  ST("<!-- bound to a particular site_id must be conveyed via a -->");
  ST("<!-- site_parameter_ref. It is required that type_ids consist of the -->");
  ST("<!-- type_class string with an underscore and number appended -->");
  ST("<!ELEMENT data_list (data_title, data_parameter_list, (data_record)+)>");
  ST("<!ATTLIST data_list data_class_prepend ID #REQUIRED>");
  ST("<!ELEMENT data_title (#PCDATA)>");
  ST("<!ELEMENT data_parameter_list (data_parameter_spec)*>");
  ST("<!ATTLIST data_parameter_list param_prepend ID #REQUIRED>");
  ST("<!ELEMENT data_parameter_spec (#PCDATA)>");
  ST("<!ATTLIST data_parameter_spec param_id ID #REQUIRED>");
  ST("<!ATTLIST data_parameter_spec type (char | double | int | idref) \"char\">");
  ST("<!ATTLIST data_parameter_spec ref_prepend IDREF #IMPLIED>");
  ST("<!ELEMENT data_record ((data_parameter | data_parameter_ref))*>");
  ST("<!ATTLIST data_record data_id ID #REQUIRED>");
  ST("<!ELEMENT data_parameter (#PCDATA)>");
  ST("<!ATTLIST data_parameter param_ref IDREF #REQUIRED>");
  ST("<!ELEMENT data_parameter_ref EMPTY>");
  ST("<!ATTLIST data_parameter_ref param_ref IDREF #REQUIRED>");
  ST("<!ATTLIST data_parameter_ref ref_ptr IDREF #REQUIRED>");
  ST(" ");
  ST(" ");     
	ST("<!-- The force_term_list contains the list of parameters and sites for -->");
	ST("<!-- each force term type defined.  The force_term_id is a unique -->");
	ST("<!-- identifier that specifies the functional form to be used. Each -->");
	ST("<!-- force parameter record contains a list of sites whose positions -->");
	ST("<!-- are required for force computation, as well as the parameter -->");
	ST("<!-- values required. Examples of force_term_ids are: -->");
	ST("<!-- torsion_oplsaa, improper_charmm, bond_harmonic, -->");
	ST("<!-- urey_bradley, angle_harmonic, ...-->");
	ST("<!-- It is required that all param_id values be consecutive integers -->");
	ST("<!-- prepended with a fixed length string specified as an -->");
	ST("<!-- attribute of the force_parameter_list. -->");
	ST("<!ELEMENT force_list (force_parameter_list, (force_parameter_record)*)>");
	ST("<!ATTLIST force_list force_term_id ID #REQUIRED>");
	ST("<!ATTLIST force_list site_count CDATA #REQUIRED>");
	ST("<!ELEMENT force_parameter_list (force_parameter_spec)+>");
	ST("<!ATTLIST force_parameter_list param_prepend NMTOKEN #REQUIRED>");
	ST("<!ELEMENT force_parameter_spec (#PCDATA)>");
	ST("<!ATTLIST force_parameter_spec param_id ID #REQUIRED>");
	ST("<!ATTLIST force_parameter_spec type (double | int) \"double\">");
	ST("<!ELEMENT force_parameter_record ((force_site)+, (force_parameter)+)>");
	ST("<!ELEMENT force_site EMPTY>");
	ST("<!ATTLIST force_site site_ref IDREF #REQUIRED>");
	ST("<!ATTLIST force_site site_ordinal CDATA #REQUIRED>");
	ST("<!ELEMENT force_parameter (#PCDATA)>");
	ST("<!ATTLIST force_parameter param_ref IDREF #REQUIRED>");

	ST("<!-- The pair list contains a list of pair interaction parameters for -->");
	ST("<!-- each pair of the associated types-->");
	ST("<!ELEMENT pair_list (pair_parameter_list, (pair_record)+)>");
	ST("<!ATTLIST pair_list type_class_prepend IDREF #REQUIRED>");
	ST("<!ELEMENT pair_parameter_list (pair_parameter_spec)+>");
	ST("<!ATTLIST pair_parameter_list param_prepend ID #REQUIRED>");
	ST("<!ELEMENT pair_parameter_spec (#PCDATA)>");
	ST("<!ATTLIST pair_parameter_spec param_id ID #REQUIRED>");
	ST("<!ATTLIST pair_parameter_spec type (char | double | int) \"char\">");
	ST("<!ELEMENT pair_record (pair_parameter)+>");
	ST("<!ATTLIST pair_record type_ref_1 IDREF #REQUIRED>");
	ST("<!ATTLIST pair_record type_ref_2 IDREF #REQUIRED>");
	ST("<!ELEMENT pair_parameter (#PCDATA)>");
	ST("<!ATTLIST pair_parameter param_ref IDREF #REQUIRED>");

	ST("<!-- User defined groups contain group information and a list of group -->");
	ST("<!-- members specified by site_id-->");
	ST("<!ELEMENT group_list (group_info, (group_member)+)>");
	ST("<!ATTLIST group_list group_id ID #REQUIRED>");
	ST("<!ATTLIST group_list group_class CDATA #REQUIRED>");
	ST("<!ELEMENT group_info (#PCDATA)>");
	ST("<!ELEMENT group_member EMPTY>");
	ST("<!ATTLIST group_member site_ref IDREF #REQUIRED>");
	ST("]>");
	ST(" ");
	//  ST("<!--  Created by BlueMatter CHARMM Filter");
//  	ST("numberLevels=7, maxRepeats=3, Random seed=989328384991");
//  	ST("	fixedOdds=1, impliedOdds=1, defaultOdds=1");
//  	ST("  maxIdRefs=3, maxEntities=3, maxNMTokens=3");
//  	ST("  isExplicitRoot=false");
//  	ST("	entOdds=1 Entity list:[]");
//  	ST(" 	doctype declaration?true");
	//	ST("   -->");


	ST(" ");
	ST("<ffparams>");
	ST("<header_parameter_list>");
	ST("<header_parameter param_id=\"ff_family\" type=\"char\">CHARMM</header_parameter>");
	ST("<header_parameter param_id=\"ff_spec\">CHARMM22</header_parameter>");
	ST("<header_parameter param_id=\"charge14\" type=\"int\">1</header_parameter>");
	ST("<header_parameter param_id=\"charge14scale\" type=\"double\">1.0</header_parameter>");
	ST("<header_parameter param_id=\"lj14\" type=\"int\">1</header_parameter>");
	ST("<header_parameter param_id=\"lj14scale\" type=\"double\">1.0</header_parameter>");
	ST("<header_parameter param_id=\"torsioninput\" type=\"int\">1</header_parameter>");
	ST("<header_parameter param_id=\"improper\" type=\"int\">1</header_parameter>");
	ST("<header_parameter param_id=\"ureybradley\" type=\"int\">1</header_parameter>");
	ST("<header_parameter param_id=\"grouping\" type=\"int\">1</header_parameter>");
	ST("<header_parameter param_id=\"water\">TIP4P</header_parameter>");
	ST("<header_parameter param_id=\"units\">CHEM</header_parameter>");
	ST("</header_parameter_list>");
	
	ST("<data_list data_class_prepend=\"at_\">");
	ST("  <data_title>atom_type</data_title>");
	ST("  <data_parameter_list param_prepend=\"atp_\">");
	ST("  <data_parameter_spec param_id=\"atp_1\" type=\"char\">atom_type</data_parameter_spec>");
	ST("  </data_parameter_list>");

	at = d_mols->Ats();
	while(at != NULL)
	  {
	    sprintf(tbufS,"<data_record  data_id=\"at_%d\">",at->GlobalId() + 1);
	    ST(tbufS);

	    sprintf(tbufS,"<data_parameter param_ref=\"atp_1\">%s</data_parameter>",at->Name());
	    ST(tbufS);
	    ST("</data_record>");

	    at = at->Next();
	  }
	ST("</data_list>");


	ST("<data_list data_class_prepend=\"lj_\">");
	ST("  <data_title>Lennard_Jones_Types</data_title>");
	ST("  <data_parameter_list param_prepend=\"ljt_\">");
	ST("</data_parameter_list>");

	at = d_mols->ATypes();
	i = 1;
	while(at != NULL)
	  {
	    sprintf(tbufS,"<data_record data_id=\"lj_%d\"></data_record>",i);
	    ST(tbufS);
	    at = at->Next();
	    i++;
	  }
	ST("</data_list>");


	ST("<data_list data_class_prepend=\"lj14_\">");
	ST("  <data_title>Lennard_Jones_14_Types</data_title>");
	ST("  <data_parameter_list param_prepend=\"ljt14_\">");
	ST("</data_parameter_list>");

	i = 1;
	at = d_mols->ATypes();
	while(at != NULL)
	  {
	    sprintf(tbufS,"<data_record data_id=\"lj14_%d\"></data_record>",i);
	    ST(tbufS);
	    at = at->Next(); 
	    i++;
	  }
	ST("</data_list>");

	ST("<data_list data_class_prepend=\"site_\">");
	ST("  <data_title>site_list</data_title>");
	ST("  <data_parameter_list param_prepend=\"sp_\">");
	ST("<data_parameter_spec param_id=\"sp_1\" type=\"double\">mass</data_parameter_spec>");
	ST("<data_parameter_spec param_id=\"sp_2\" type=\"double\">charge</data_parameter_spec>");
	//	ST("<data_parameter_spec param_id=\"sp_3\" type=\"idref\" ref_prepend=\"m_\">molecule</data_parameter_spec>");
	//	ST("<data_parameter_spec param_id=\"sp_4\" type=\"idref\" ref_prepend=\"cc_\">chemical_component</data_parameter_spec>");
	ST("<data_parameter_spec param_id=\"sp_3\" type=\"idref\" ref_prepend=\"at_\">atom_type</data_parameter_spec>");
	ST("<data_parameter_spec param_id=\"sp_4\" type=\"idref\" ref_prepend=\"lj_\">lennard_jones_type</data_parameter_spec>");
	ST("<data_parameter_spec param_id=\"sp_5\" type=\"idref\" ref_prepend=\"lj14_\">lennard_jones_14_type</data_parameter_spec>");
	//	ST("<data_parameter_spec param_id=\"sp_8\" type=\"int\">atomic_number</data_parameter_spec>");
	ST("</data_parameter_list>");

	
	at = d_mols->Ats();
	while(at != NULL)
	  {
	    sprintf(tbufS,"<data_record data_id=\"site_%d\">",at->GlobalId() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"\t<data_parameter param_ref=\"sp_1\">%lf</data_parameter>",at->Mass());
	    ST(tbufS);
	    sprintf(tbufS,"\t<data_parameter param_ref=\"sp_2\">%lf</data_parameter>",at->Charge());
	    ST(tbufS);
	    // local ID has been mapped to index of atype list
	    sprintf(tbufS,"\t<data_parameter_ref param_ref=\"sp_3\" ref_ptr=\"at_%d\"/>",at->LocalId() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"\t<data_parameter_ref param_ref=\"sp_4\" ref_ptr=\"lj_%d\"/>",at->LocalId() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"\t<data_parameter_ref param_ref=\"sp_5\" ref_ptr=\"lj14_%d\"/>",at->LocalId() + 1);
	    ST(tbufS);
	    ST("</data_record>");

	    at = at->Next();
	  }
	ST("</data_list>");



	ST("<data_list data_class_prepend=\"bond_\">");
	ST("  <data_title>bond_list</data_title>");
	ST("  <data_parameter_list param_prepend=\"bondp_\">");
	ST("<data_parameter_spec param_id=\"bondp_1\" type=\"idref\" ref_prepend=\"site_\">site_ref1</data_parameter_spec>");
	ST("<data_parameter_spec param_id=\"bondp_2\" type=\"idref\" ref_prepend=\"site_\">site_ref2</data_parameter_spec>");
	ST("<data_parameter_spec param_id=\"bondp_3\" type=\"char\">type</data_parameter_spec>");
	ST("</data_parameter_list>");

	bd = d_mols->Bds();
	while(bd != NULL)
	  {
	    sprintf(tbufS,"<data_record data_id=\"bond_%d\">",bd->GlobalId() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"\t<data_parameter_ref param_ref=\"bondp_1\" ref_ptr=\"site_%d\"/>",bd->A1() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"\t<data_parameter_ref param_ref=\"bondp_2\" ref_ptr=\"site_%d\"/>",bd->A2() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"\t<data_parameter param_ref=\"bondp_3\">\"%s\"</data_parameter>","TBD");
	    ST(tbufS);
	    ST("</data_record>");

	    bd = bd->Next();
	  }
	ST("</data_list>");



	ST("<force_list site_count=\"2\" force_term_id=\"StdHarmonicBondForce\">");
	ST("<force_parameter_list param_prepend=\"bh_\">");
	ST("<force_parameter_spec param_id=\"bh_1\" type=\"double\">mSpringConstant</force_parameter_spec>");
	ST("<force_parameter_spec param_id=\"bh_2\" type=\"double\">mEquilibriumDistance</force_parameter_spec>");
	ST("</force_parameter_list>");

	bd = d_mols->Bds();
	while(bd != NULL)
	  {
	    ST("<force_parameter_record>");
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"1\"/>",bd->A1() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"2\"/>",bd->A2() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"bh_1\">%lf</force_parameter>",bd->K0());
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"bh_2\">%lf</force_parameter>",bd->Eq());
	    ST(tbufS);
	    ST("</force_parameter_record>");
	    bd = bd->Next();
	  }
	ST("</force_list>");



	ST("<force_list site_count=\"3\" force_term_id=\"StdHarmonicAngleForce\">");
	ST("<force_parameter_list param_prepend=\"angh_\">");
	ST("<force_parameter_spec param_id=\"angh_1\" type=\"double\">mSpringConstant</force_parameter_spec>");
	ST("<force_parameter_spec param_id=\"angh_2\" type=\"double\">mEquilibriumAngle</force_parameter_spec>");
	ST("</force_parameter_list>");
	ang = d_mols->Angs();
	while(ang != NULL)
	  {
	    ST("<force_parameter_record>");
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"1\"/>",ang->A1() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"2\"/>",ang->A2() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"3\"/>",ang->A3() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"angh_1\">%lf</force_parameter>",ang->K0());
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"angh_2\">%20.15lf</force_parameter>",ang->Eq() * M_PI / 180.0);
	    ST(tbufS);
	    ST("</force_parameter_record>");
	    ang = ang->Next();
	  }
	ST("</force_list>");


	ST("<force_list site_count=\"3\" force_term_id=\"UreyBradleyForce\">");
	ST("<force_parameter_list param_prepend=\"ub_\">");
	ST("<force_parameter_spec param_id=\"ub_1\" type=\"double\">mSpringConstant</force_parameter_spec>");
	ST("<force_parameter_spec param_id=\"ub_2\" type=\"double\">mEquilibriumDistance</force_parameter_spec>");
	ST("</force_parameter_list>");
	ang = d_mols->Angs();
	while(ang != NULL)
	  {
	    ST("<force_parameter_record>");
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"1\"/>",ang->A1() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"2\"/>",ang->A2() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"3\"/>",ang->A3() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"ub_1\">%lf</force_parameter>",ang->UBK0());
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"ub_2\">%lf</force_parameter>",ang->UBEq());
	    ST(tbufS);
	    ST("</force_parameter_record>");
	    ang = ang->Next();
	  }
	ST("</force_list>");


	ST("<force_list site_count=\"4\" force_term_id=\"SwopeTorsionForce\">");
	ST("<force_parameter_list param_prepend=\"ctp_\">");
	ST("<force_parameter_spec param_id=\"ctp_1\" type=\"double\">k0</force_parameter_spec>");
	ST("<force_parameter_spec param_id=\"ctp_2\" type=\"int\">multiplicity</force_parameter_spec>");
	ST("<force_parameter_spec param_id=\"ctp_3\" type=\"double\">delta</force_parameter_spec>");
	ST("</force_parameter_list>");
	tor = d_mols->Tors();
	while(tor != NULL)
	  {
	    ST("<force_parameter_record>");
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"1\"/>",tor->A1() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"2\"/>",tor->A2() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"3\"/>",tor->A3() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"4\"/>",tor->A4() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"ctp_1\">%lf</force_parameter>",tor->K0());
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"ctp_2\">%d</force_parameter>",(int)tor->Wells());
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"ctp_3\">%20.15lf</force_parameter>",tor->Eq() * M_PI / 180.0);
	    ST(tbufS);
	    ST("</force_parameter_record>");
	    tor = tor->Next();
	  }
	ST("</force_list>");


	ST("<force_list site_count=\"4\" force_term_id=\"ImproperDihedralForce\">");
	ST("<force_parameter_list param_prepend=\"cip_\">");
	ST("<force_parameter_spec param_id=\"cip_1\" type=\"double\">Kpsi</force_parameter_spec>");
	ST("<force_parameter_spec param_id=\"cip_2\" type=\"double\">psi0</force_parameter_spec>");
	ST("</force_parameter_list>");
	imp = d_mols->Imps();
	while(imp != NULL)
	  {
	    ST("<force_parameter_record>");
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"1\"/>",imp->A1() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"2\"/>",imp->A2() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"3\"/>",imp->A3() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_site site_ref=\"site_%d\" site_ordinal=\"4\"/>",imp->A4() + 1);
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"cip_1\">%lf</force_parameter>",imp->K0());
	    ST(tbufS);
	    sprintf(tbufS,"<force_parameter param_ref=\"cip_2\">%20.15lf</force_parameter>",imp->Eq() * M_PI / 180.0);
	    ST(tbufS);
	    ST("</force_parameter_record>");
	    imp = imp->Next();
	  }
	ST("</force_list>");

	Atom *tat;
	int j;

	ST("<pair_list type_class_prepend=\"lj_\">");
	ST("<pair_parameter_list param_prepend=\"ljpp_\">");
	ST("<pair_parameter_spec param_id=\"ljpp_1\" type=\"double\">epsilon</pair_parameter_spec>");
	ST("<pair_parameter_spec param_id=\"ljpp_2\" type=\"double\">sigma</pair_parameter_spec>");
	ST("</pair_parameter_list>");
	at = d_mols->ATypes();
	for(i=1;i<=d_mols->NATypes();i++)
	  {
	    tat = d_mols->ATypes();
	    for(j=1;j<=d_mols->NATypes();j++)
	      {
		if(j > i) break;
		sprintf(tbufS,"<pair_record type_ref_1=\"lj_%d\" type_ref_2=\"lj_%d\">",i,j);
		ST(tbufS);
	      
		double eps = sqrt(at->Eps() * tat->Eps());
		double sigma = at->RMin() + tat->RMin();
		
		//		strstream epsSS; epsSS.precision(32); epsSS << eps << endl;
		//		strstream sigmaSS; sigmaSS.precision(32); sigmaSS << sigma << endl;
		//		char *epsS = epsSS.str(); 
		//		char *sigmaS = sigmaSS.str(); 
		sprintf(tbufS,"<pair_parameter param_ref=\"ljpp_1\"> %32.15lf </pair_parameter>",eps);
		ST(tbufS);
		sprintf(tbufS,"<pair_parameter param_ref=\"ljpp_2\"> %32.15lf </pair_parameter>",sigma);
		ST(tbufS);
		ST("</pair_record>");
		tat = tat->Next();
		//		delete epsS, sigmaS;
	       }
	    at = at->Next();
	  }
	ST("</pair_list>");

	ST("<pair_list type_class_prepend=\"lj14_\">");
	ST("<pair_parameter_list param_prepend=\"ljp14p_\">");
	ST("<pair_parameter_spec param_id=\"ljp14p_1\" type=\"double\">epsilon</pair_parameter_spec>");
	ST("<pair_parameter_spec param_id=\"ljp14p_2\" type=\"double\">sigma</pair_parameter_spec>");
	ST("</pair_parameter_list>");
	at = d_mols->ATypes();
	for(i=1;i<=d_mols->NATypes();i++)
	  {
	    tat = d_mols->ATypes();
	    for(j=1;j<=d_mols->NATypes();j++)
	      {
		if(j > i) break;
		sprintf(tbufS,"<pair_record type_ref_1=\"lj14_%d\" type_ref_2=\"lj14_%d\">",i,j);
		ST(tbufS);
	      
		double eps = sqrt(at->Eps14() * tat->Eps14());
		double sigma = at->RMin14() + tat->RMin14();
	//  	strstream epsSS; epsSS.precision(32); epsSS << eps << endl;
//  		strstream sigmaSS; sigmaSS.precision(32); sigmaSS << sigma << endl;
//  		char *epsS = epsSS.str(); 
//  		char *sigmaS = sigmaSS.str(); 
		
		sprintf(tbufS,"<pair_parameter param_ref=\"ljp14p_1\"> %32.15lf </pair_parameter>",eps);
		ST(tbufS);
		sprintf(tbufS,"<pair_parameter param_ref=\"ljp14p_2\"> %32.15lf </pair_parameter>",sigma);
		ST(tbufS);
		ST("</pair_record>");
		tat = tat->Next();
		//		delete epsS,sigmaS;
	       }
	    at = at->Next();
	  }
	ST("</pair_list>");

	ST("</ffparams>");

	//  Atom *elems;
	//  DeriveElementTypes(&elems);

//    XREC1("<type_symbol_list>");
//    Atom *elem = elems;
//    while(elem != NULL)
//      {
//        char eC[3];
//        char typeS[100];
//        eC[2] = '\0';
//        eC[0] = elem->Name()[0];
//        if((eC[0] == 'X')||(eC[0] == 'M'))
//          {
//            eC[0] = elem->Name()[0];
//            eC[1] = elem->Name()[1];
//          }
//        else
//          eC[1] = '\0';
//        sprintf(typeS,"<type_symbol element_symbol=\"%s\"><mass>%f</mass></type_symbol>\n",eC,elem->Mass());
//        elem = elem->Next();
//      }
//    XREC0("type_symbol_list");





  return 1;

}

int MolecularSystem::Finalize()
{
  d_mols->Finalize();
  DeriveParameterMaps();
  d_mols->DeriveNeighborhoodLists();
  return 1;
}

void MolecularSystem::AddAtomType(char *atypeS,int type)
{
  Atom * gat = new Atom();
  gat->Type() = type;
  gat->GlobalId() = d_globs->NAtoms();
  gat->LocalId() = d_globs->NAtoms();
  strcpy(gat->Name(),atypeS);

  d_globs->AddAtom(gat);
}
void MolecularSystem::AddAtomInstance(char *massS,
				      char *chargeS,
				      char *atypeS,
				      char *ljtypeS,
				      char *lj14typeS)
{

  double mass,charge,zero = 0.0;
  int ljtype,lj14type;
  int n,type;
  sscanf(massS,"%lf",&mass);
  sscanf(chargeS,"%lf",&charge);
  n = strlen("at_");  // NOTE should be ATOM_TYPE_PREPEND !!!!!
  sscanf(&atypeS[n],"%d",&type);
  type--;
  sscanf(ljtypeS,"%d",&ljtype);
  sscanf(lj14typeS,"%d",&lj14type);
  assert(ljtype == lj14type);

  // NOTE: lj params have been phased out due to pairlist. 
  // NOTE: pairlist support still needs to be implemented


  Atom *at = new Atom();
  strcpy(at->Name(),atypeS);
  Atom *gat,*tgat = NULL;
  Atom tat;
  at->Type() = type;
  at->Mass() = mass;
  at->Charge() = charge;
  at->LJType() = ljtype;
  at->LJ14Type() = lj14type;

  int found = 0;
  gat = d_globs->Ats();
  while(gat != NULL)
    {
      if(gat->Type() == type)
	{
	  found = 1;
	  tgat = gat;
	  if(at->SameType((const List &) *gat))
	    break;
	  else if ((gat->Mass() == tat.Mass()) && 
		   (gat->Charge() == tat.Charge())) // unitialize mass and charge
	    {
	      gat->Mass() = at->Mass();
	      gat->Charge() = at->Charge();
	      gat->LJType() = ljtype;
	      gat->LJ14Type() = lj14type;
	      break;
	    }
	  else
	    {
	      // found type with same LJ params but diff mass or charge: keep looking;
	    }
	}
      gat = gat->Next();
    }
  if(gat == NULL && found == 1)
    {
      // We need to add a previous type with the unique mass and charge values
      gat = new Atom(*tgat);
      gat->Mass() = at->Mass();
      gat->Charge() = at->Charge();
      gat->LJType() = ljtype;
      gat->LJ14Type() = lj14type;
      d_globs->AddAtom(gat);
    }

  at->Type() = gat->LocalId();

  at->GlobalId()   = d_mols->NAtoms();
  d_mols->AddAtom(at);
  
}

void MolecularSystem::AddLennardJonesType(char *type)
{
}
void MolecularSystem::AddLennardJones14Type(char *type)
{
  
}
void MolecularSystem::AddLennardJonesPair(int type1,
					  int type2,
					  const char *epsS,
					  const char *sigmaS)
{
  Atom *at = new Atom();
  double eps,rmin;
  sscanf(epsS,"%lf",&eps);
  sscanf(sigmaS,"%lf",&rmin);
  at->GlobalId() = d_mols->NPairs();
  at->Eps() = eps;
  at->RMin() = rmin;
  // BAD!!! using type and 14type and placeholders for type1 and type2 
  at->LJType() = type1;
  at->LJ14Type() = type2;
  d_mols->AddPair(at);
}
void MolecularSystem::AddLennardJones14Pair(int type1,
					    int type2,
					    const char *epsS,
					    const char *sigmaS)
{
  double eps,rmin;
  sscanf(epsS,"%lf",&eps);
  sscanf(sigmaS,"%lf",&rmin);
  Atom *at = new Atom();
  at->GlobalId() = d_mols->N14Pairs();
  at->Eps14() = eps;
  at->RMin14() = rmin;
  // BAD!!! using type and 14type and placeholders for type1 and type2 
  at->LJType() = type1;
  at->LJ14Type() = type2;
  d_mols->Add14Pair(at);
}
				       

void MolecularSystem::AddBondTerm(int site1,
				  int site2,
				  const char *k0S,
				  const char *thetaS)
{
  double k0,theta;
  sscanf(k0S,"%lf",&k0);
  sscanf(thetaS,"%lf",&theta);

  int k;
  Atom *at,tat;
  Bond * bd = new Bond();
  Bond * gbd = new Bond();

  bd->SetArg(0,k0S);
  bd->SetArg(1,thetaS);
  gbd->SetArg(0,k0S);
  gbd->SetArg(1,thetaS);

  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  tat.GlobalId() = site1;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  bd->A1() = at->GlobalId();
  gbd->A1() = at->Type();

  tat.GlobalId() = site2;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  bd->A2() = at->GlobalId();
  gbd->A2() = at->Type();

  bd->K0() = gbd->K0() = k0;
  bd->Eq() = gbd->Eq() = theta;

  bd->Type() = gbd->Type() = d_mols->NBonds();
  bd->GlobalId() = gbd->GlobalId() = d_mols->NBonds();

  d_mols->AddBond(bd);
  d_globs->AddBond(gbd);

}


void MolecularSystem::AddAngleTerm(int site1,
				   int site2,
				   int site3,
				   const char *k0S,
				   const char *thetaS)
{

  double k0,theta;
  sscanf(k0S,"%lf",&k0);
  sscanf(thetaS,"%lf",&theta);

  Atom *at,tat;
  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  Angle * ag = new Angle();
  Angle * gag = new Angle();
  ag->SetArg(0,k0S);
  ag->SetArg(1,thetaS);
  gag->SetArg(0,k0S);
  gag->SetArg(1,thetaS);

  tat.GlobalId() = site1;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A1() = at->GlobalId();
  gag->A1() = at->Type();

  tat.GlobalId() = site2;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A2() = at->GlobalId();
  gag->A2() = at->Type();

  tat.GlobalId() = site3;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A3() = at->GlobalId();
  gag->A3() = at->Type();

  ag->K0() = gag->K0() = k0;
  ag->Eq() = gag->Eq() = theta;

  ag->Type() = gag->Type() = d_mols->NAngles();
  ag->GlobalId() = gag->GlobalId() = d_mols->NAngles();

  d_mols->AddAngle(ag);
  d_globs->AddAngle(gag);

}
void MolecularSystem::AddUreyTerm(int site1,
				   int site2,
				   int site3,
				   const char *k0S,
				   const char *thetaS)
{
  int A1,A2,A3;
  Atom *at,*tat;
  Angle * ag  = d_mols->Angs();
  Angle * gag = d_globs->Angs();
  double k0,theta;

  //NEED to decouple ureys from angles completely!!!!!!
  sscanf(k0S,"%lf",&k0);
  sscanf(thetaS,"%lf",&theta);

  if(d_globs->d_atab==NULL)
    assert(0);

  if(d_globs->d_atab == NULL)
    assert(0);

  at = d_mols->Ats();
  while(at != NULL)
    {
      if(at->GlobalId() == site1)	A1 = at->Type();

      if(at->GlobalId() == site2)	A2 = at->Type();

      if(at->GlobalId() == site3)	A3 = at->Type();

      at = at->Next();
    }

  while(ag != NULL)
    {
      if((site1 == ag->A1() && site2 == ag->A2() && site3 == ag->A3()) ||
	 (site3 == ag->A1() && site2 == ag->A2() && site1 == ag->A3())	  )
	break;
      ag = ag->Next();
    }
  while(gag != NULL)
    {
      if((A1 == gag->A1() && A2 == gag->A2() && A3 == gag->A3()) ||
	 (A3 == gag->A1() && A2 == gag->A2() && A1 == gag->A3())	  )
	break;
      gag = gag->Next();
    }
  assert(gag != NULL);
  ag->UBK0() = gag->UBK0() = k0;
  ag->UBEq() = gag->UBEq() = theta;
  ag->SetArg(2,k0S);
  ag->SetArg(3,thetaS);
  gag->SetArg(2,k0S);
  gag->SetArg(3,thetaS);
  return;
}

void MolecularSystem::AddTorsionTerm(int site1,
				     int site2,
				     int site3,
				     int site4,
				     const char *k0S,
				     const char *multS,
				     const char *thetaS)
{

  double k0,theta,multD;
  int mult;
  sscanf(k0S,"%lf",&k0);
  sscanf(thetaS,"%lf",&theta);
  sscanf(multS,"%d",&mult);
  multD = mult;
  Atom *at,tat;

  Torsion * ag = new Torsion();
  Torsion * gag = new Torsion();
  ag->SetArg(0,k0S);
  ag->SetArg(1,multS);
  ag->SetArg(2,thetaS);
  gag->SetArg(0,k0S);
  gag->SetArg(1,multS);
  gag->SetArg(2,thetaS);

  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  tat.GlobalId() = site1;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A1() = at->GlobalId();
  gag->A1() = at->Type();
  int a1 = at->Type();

  tat.GlobalId() = site2;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A2() = at->GlobalId();
  gag->A2() = at->Type();
  int a2 = at->Type();

  tat.GlobalId() = site3;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A3() = at->GlobalId();
  gag->A3() = at->Type();
  int a3 = at->Type();

  tat.GlobalId() = site4;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A4() = at->GlobalId();
  gag->A4() = at->Type();
  int a4 = at->Type();

  ag->K0() = gag->K0() = k0;
  ag->Eq() = gag->Eq() = theta;
  ag->Wells() = gag->Wells() = multD;

  ag->Type() = gag->Type() = d_mols->NTorsions();
  ag->GlobalId() = gag->GlobalId() = d_mols->NTorsions();

  d_mols->AddTorsion(ag);
  d_globs->AddTorsion(gag);


}

void MolecularSystem::AddImproperTerm(int site1,
				      int site2,
				      int site3,
				      int site4,
				      const char *k0S,
				      const char *thetaS)
{
  Atom *at,tat;
  double k0,theta;

  if(d_globs->d_atab==NULL)
    d_globs->Finalize();

  sscanf(k0S,"%lf",&k0);
  sscanf(thetaS,"%lf",&theta);

  ImproperTorsion * ag = new ImproperTorsion();
  ImproperTorsion * gag = new ImproperTorsion();
  ag->SetArg(0,k0S);
  ag->SetArg(1,thetaS);
  gag->SetArg(0,k0S);
  gag->SetArg(1,thetaS);


  tat.GlobalId() = site1;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A1() = at->GlobalId();
  gag->A1() = at->Type();


  tat.GlobalId() = site2;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A2() = at->GlobalId();
  gag->A2() = at->Type();


  tat.GlobalId() = site3;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A3() = at->GlobalId();
  gag->A3() = at->Type();


  tat.GlobalId() = site4;
  List::LocateItem(d_mols->d_ats,tat,(List **)&at);
  ag->A4() = at->GlobalId();
  gag->A4() = at->Type();

  ag->K0() = gag->K0() = k0;
  ag->Eq() = gag->Eq() = theta;
  ag->Wells() = gag->Wells() = 0;

  ag->Type() = gag->Type() = d_mols->d_nimps;
  ag->GlobalId() = gag->GlobalId() = d_mols->d_nimps;

  d_mols->AddImproperTorsion(ag);
  d_globs->AddImproperTorsion(gag);
}


//          //  First I get the code by the name in the xml

//  	static inline void GetUDFCodeByName(const char *name, int &code)


//  	  // once I have the code, I can then get the number of sites and params
//  	static inline void GetNSitesByUDFCode(const int code, int &num)
//  	static inline void GetParamCountByUDFCode(const int code, int &count)


//  	  // since I know how many params there are anyway, I can loop through the
//  	  // paramspec and grab each param name. Once I have the param name, I
//  	  // call this function and get the variable type and ordinal
//  	static inline void GetArgByName(const int code, const char *name, int &ord, char *type)


//  	  // Alternatively, if I want to simply loop through the param list in the serial order
//  	  // I could call this function with the loop index and get the name and type. But I 
//  	  // would then have to map back through Bob's interface to get the value.
//  	static inline void GetArgByNum(const int code, const int ord, char *name, char *type)


//  	  // use this function when getting fancy with respa levels.
//  	static inline void IsCodeCategory(const int code, const char *cat, int &result)

//  	  // once I'm dumping the static initialization in the header file, I use the first function
//  	  // to get the just the name for string composition and the second to declare the structure
//  	  // so the compiler can see what is being initialized
//  	static inline void GetStructureNameByNum(const int code, char *name)
//  	static inline void GetStructureStringByNum(const int code, char *struc)

