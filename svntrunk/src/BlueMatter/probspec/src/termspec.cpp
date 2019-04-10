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
 

#include <BlueMatter/termspec.hpp>


static   int CompareDoubles(const void *A, const void *B)
    {
      double const *dAP = ( double *) A;
      double const *dBP = ( double *) B;
      double dA = *dAP;
      double dB = *dBP;

      if(dA > dB)
        return 1;
      else if (dA < dB)
        return -1;
      return 0;
    }
static   int CompareScore(const void *termA, const void *termB)
    {
      TermSpec const *a = (TermSpec *)termA;
      TermSpec const *b = (TermSpec *)termB;
      if(a->mScore < b->mScore)
        return -1;
      else if(a->mScore > b->mScore)
        return 1;
      else
        return 0;
    }

static  int CompareSortedAtnums(const void *termA, const void *termB)
    {
      TermSpec const *A = (TermSpec *)termA;
      TermSpec const *B = (TermSpec *)termB;

      int ids1[4];
      int ids2[4];
      int nats;

      ids1[0] = A->mA1;
      ids2[0] = B->mA1;
      ids1[1] = A->mA2;
      ids2[1] = B->mA2;


      switch(A->mTermType)
        {
        case BONDCODE:
          nats = 2;
          break;
        case EXCLUDEDCODE:
          nats = 2;
          break;
        case ANGLECODE:
          nats = 3;
          ids1[2] = A->mA3;
          ids2[2] = B->mA3;
          break;
        case TORSIONCODE:
        case IMPROPERCODE:
          ids1[3] = A->mA4;
          ids2[3] = B->mA4;
          break;
        default: assert(0);
        }
      int i;
      int flg = 1;
      int temp;
      while(flg)
        {
          flg = 0;
          for(i=0;i<nats-1;i++)
            {
              if(ids1[i] > ids1[i+1])
                { flg = 1; temp = ids1[i]; ids1[i] = ids1[i+1]; ids1[i+1] = temp;}
              if(ids2[i] > ids2[i+1])
                { flg = 1; temp = ids2[i]; ids2[i] = ids2[i+1]; ids2[i+1] = temp;}
            }
        }
      for(i=0;i<nats;i++)
        {
          if(ids1[i] > ids2[i])
            { return 1;}
          else if(ids1[i] < ids2[i])
            { return -1;}
        }
      return 0;
    }


static  int CompareAtnums(const void *termA, const void *termB)
{
  TermSpec const *A = (TermSpec *)termA;
  TermSpec const *B = (TermSpec *)termB;

  int ids1[4];
  int ids2[4];
  int nats;

  ids1[0] = A->mA1;
  ids2[0] = B->mA1;
  ids1[1] = A->mA2;
  ids2[1] = B->mA2;


  switch(A->mTermType)
    {
    case BONDCODE:
      nats = 2;
      break;
    case EXCLUDEDCODE:
      nats = 2;
      break;
    case ANGLECODE:
      nats = 3;
      ids1[2] = A->mA3;
      ids2[2] = B->mA3;
      break;
    case TORSIONCODE:
    case IMPROPERCODE:
      ids1[3] = A->mA4;
      ids2[3] = B->mA4;
      break;
    default: assert(0);
    }
  int i;

  for(i=0;i<nats;i++)
    {
      if(ids1[i] > ids2[i])
        { return 1;}
      else if(ids1[i] < ids2[i])
        { return -1;}
    }
  return 0;
}


double TermSpec::Score(double base)
{
  mScore = (((base * mA1)*base + mA2)*base + mA3)*base + mA4;
  return mScore;
}
TermSpec * TermSpec::Next(){return (TermSpec *)d_next;}
TermSpec * TermSpec::Prev(){return (TermSpec *)d_prev;}
TermSpec * TermSpec::Copy()
{
  TermSpec *t = new TermSpec();
  *t = *this;
  return t;
}
TermSpec * TermSpec::CopyList()
{
  TermSpec *ts = this;
  TermSpec *t = NULL;
  while(ts != NULL)
    {
      TermSpec * tts = ts->Copy();
      if(t == NULL)
        t = tts;
      else
        t->Append(tts);
      tts->LocalId() = ts->LocalId();
      ts = ts->Next();
    }
  return t;
}
  void TermSpec::SetNext(List *l){d_next = l;}
  void TermSpec::SetPrev(List *l){d_prev = l;}

 int TermSpec::Flatten(List ***lr)
{
  TermSpec*** ar = (TermSpec ***)lr;
  int i,n = Count();
  TermSpec *ls = this;
  assert(n > 0);
  TermSpec **arr = new TermSpec*[n];
  for(i=0;i<n;i++)
    {
      assert(ls != NULL);
      arr[i] = ls->Copy();
      arr[i]->d_localid = i;
      ls = ls->Next();
    }
  *ar = arr;
  return n;
}

  void TermSpec::SetBondParams(Bond *t,int code)
    {
      mTermType = code;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      mA3 = -1;
      mA4 = -1;
      sa1 = sa2 = sa3 = sa4 = 0;
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = 0.0;
      R1 = 0.0;
      return;
    }
  void TermSpec::SetAngleParams( Angle *t, int code)
    {
      mTermType = code;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      mA4 = -1;
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = 0.0;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetUreyBradleyParams( Angle *t, int code)
    {
      mTermType = code;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      mA4 = -1;
      K0 = t->d_ubk0;
      Eq = t->d_ubeq;
      mult = 0.0;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetTorsionParams( Torsion *t, int code)
    {
      mTermType = code;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      assert(t->A4() >= 0);
      mA4 = t->A4();
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = t->d_wells;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetCharmmTorsionParams( Torsion *t)
    {
      mTermType = TORSIONCODE;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      assert(t->A4() >= 0);
      mA4 = t->A4();
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = t->d_wells;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetOplsaTorsionParams( Torsion *t)
    {
      mTermType = OPLSTORSIONCODE;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      assert(t->A4() >= 0);
      mA4 = t->A4();
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = t->d_wells;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetImproperParams(ImproperTorsion *t, int code)
    {
      mTermType = code;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      assert(t->A4() >= 0);
      mA4 = t->A4();
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = t->d_wells;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetCharmmImproperParams(ImproperTorsion *t)
    {
      mTermType = IMPROPERCODE;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      assert(t->A4() >= 0);
      mA4 = t->A4();
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = t->d_wells;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetOplsaImproperParams( ImproperTorsion *t)
    {
      mTermType = OPLSTORSIONCODE;
      mTermParams = t->LocalId();
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3();
      assert(t->A4() >= 0);
      mA4 = t->A4();
      K0 = t->d_k0;
      Eq = t->d_eq;
      mult = t->d_wells;
      R1 = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetExPairParams(ExPair *t, int code)
    {
      mTermType = code;
      mTermParams = 0;
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3(); //a3;
      mA4 = -1; //a4;

      K0 = 0.0;
      Eq = 0.0;
      mult = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }
  void TermSpec::SetPair14Params(Pair14 *t, int code)
    {
      mTermType = code;
      mTermParams = 0;
      assert(t->A1() >= 0);
      mA1 = t->A1();
      assert(t->A2() >= 0);
      mA2 = t->A2();
      assert(t->A3() >= 0);
      mA3 = t->A3(); //a3;
      mA4 = -1; //a4;

      K0 = 0.0;
      Eq = 0.0;
      mult = 0.0;
      sa1 = sa2 = sa3 = sa4 = 0;
      return;
    }





