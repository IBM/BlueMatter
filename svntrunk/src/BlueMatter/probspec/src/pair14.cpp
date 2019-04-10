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
 

#ifndef _INCLUDED_PAIR14_
#include <BlueMatter/pair14.hpp>
#endif

int& Pair14::A1()
{return d_at1;}

int& Pair14::A2()
{return d_at2;}

int& Pair14::A3()
{return d_type;}

Pair14 * Pair14::Next(){return (Pair14 *)d_next;}
Pair14 * Pair14::Prev(){return (Pair14 *)d_prev;}
Pair14 * Pair14::Copy()
{
  Pair14 *xx = new Pair14();
  *xx = *this;
  return xx;
}
Pair14 * Pair14::CopyList()
{
 Pair14 *ls = NULL;
 Pair14 *tls = this;
 while(tls != NULL)
   {
     Pair14 *lls = tls->Copy();
     if(ls == NULL)
       ls = lls;
     else
       ls->Append(lls);
     tls = tls->Next();
   }
 return ls;
}

 int Pair14::Flatten(List*** lr)
{
  Pair14 ***ar = (Pair14 ***)lr;
  int i,n = Count();
  Pair14 *ls = this;
  assert(n > 0);
  Pair14 **arr = new Pair14*[n];
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
Pair14 & Pair14::operator=(Pair14 const &x)
{
  List *l = (List *)this;
  *l = (List const &)x;
  d_at1 = x.d_at1;
  d_at2 = x.d_at2;
  return *this;
}

int Pair14::operator==(Pair14 const &x)
{
  if(d_type == x.d_type &&
     d_at1 == x.d_at1 &&
     d_at2 == x.d_at2)
  return 1;
  else
  return 0;
}

