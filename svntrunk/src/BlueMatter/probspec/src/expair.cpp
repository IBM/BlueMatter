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
 
#ifndef _INCLUDED_EXPAIR_
#include <BlueMatter/expair.hpp>
#endif


int& ExPair::A1()
{return d_at1;}

int& ExPair::A2()
{return d_at2;}

int& ExPair::A3()
{return d_type;}

ExPair & ExPair::operator=(ExPair const &x)
{
  List *l = (List *)this;
  *l = (List const &)x;
  d_at1 = x.d_at1;
  d_at2 = x.d_at2;
  return *this;
}

int ExPair::operator==(ExPair const &x)
{
  if(d_type == x.d_type &&
     d_at1 == x.d_at1 &&
     d_at2 == x.d_at2)
  return 1;
  else
  return 0;
}

ExPair * ExPair::Next(){return (ExPair *)d_next;}
ExPair * ExPair::Prev(){return (ExPair *)d_prev;}
ExPair * ExPair::Copy()
{
  ExPair *xx = new ExPair();
  *xx = *this;
  return xx;
}

ExPair * ExPair::CopyList()
{
 ExPair *ls = NULL;
 ExPair *tls = this;
 while(tls != NULL)
   {
     ExPair *lls = tls->Copy();
     if(ls == NULL)
       ls = lls;
     else
       ls->Append(lls);
     lls->LocalId() = tls->LocalId();
     tls = tls->Next();
   }
 return ls;
}


 int ExPair::Flatten(List*** lr)
{
  ExPair ***ar = (ExPair ***)lr;
  int i,n = Count();
  ExPair *ls = this;
  assert(n > 0);
  ExPair **arr = new ExPair*[n];
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











