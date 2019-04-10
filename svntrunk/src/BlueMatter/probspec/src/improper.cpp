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
 
#ifndef _INCLUDED_IMPROPER_
#include <BlueMatter/improper.hpp>
#endif

ImproperTorsion * ImproperTorsion::Next(){return (ImproperTorsion *)d_next;}
ImproperTorsion * ImproperTorsion::Prev(){return (ImproperTorsion *)d_prev;}

ImproperTorsion::ImproperTorsion()
{
  d_at1 = d_at2 = d_at3 = d_at4 = 0;
  d_k0 = d_eq = d_wells = 0.0;
}
ImproperTorsion::~ImproperTorsion(){}

  ImproperTorsion &  ImproperTorsion::operator=(ImproperTorsion const & a)
{
  d_at1 = a.d_at1;
  d_at2 = a.d_at2;
  d_at3 = a.d_at3;
  d_at4 = a.d_at4;
  d_localid = a.d_localid;
  d_globalid = a.d_globalid;
  d_type = a.d_type;
  d_k0 = a.d_k0;
  d_eq = a.d_eq;
  d_wells = a.d_wells;
  for(int i=0;i<MAXARGS;i++){strcpy(d_args[i],a.d_args[i]);}
  return *this;
}

int ImproperTorsion::operator==(ImproperTorsion const & a)
{
  if(
     d_at1 == a.d_at1 &&
     d_at2 == a.d_at2 &&
     d_at3 == a.d_at3 &&
     d_at4 == a.d_at4)
  return 1;
  else
  return 0;
}
ImproperTorsion * ImproperTorsion::Copy()
{
  ImproperTorsion *bd = new ImproperTorsion();
  *bd = *this;
  return bd;
}
ImproperTorsion * ImproperTorsion::CopyList()
{
 ImproperTorsion *ls = NULL;
 ImproperTorsion *tls = this;
 while(tls != NULL)
   {
     ImproperTorsion *lls = new ImproperTorsion();
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

 int ImproperTorsion::Flatten(List ***lr)
{
  ImproperTorsion*** ar = (ImproperTorsion ***)lr;
  int i,n = Count();
  ImproperTorsion *ls = this;
  assert(n > 0);
  ImproperTorsion **arr = new ImproperTorsion*[n];
  for(i=0;i<n;i++)
    {
      assert(ls != NULL);
      arr[i] = ls; //ls->Copy();
      //arr[i]->d_localid = i;
      ls = ls->Next();
    }
  *ar = arr;
  return n;
}


 int ImproperTorsion::Contains(List *a)
{
  if(d_at1 == a->GlobalId() ||
     d_at2 == a->GlobalId() ||
     d_at3 == a->GlobalId() ||
     d_at4 == a->GlobalId() )
    return 1;
  return 0;
}



