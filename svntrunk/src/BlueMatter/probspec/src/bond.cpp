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
 

#ifndef _INCLUDED_BOND_
#include <BlueMatter/bond.hpp>
#endif



Bond * Bond::Next(){return (Bond *)d_next;}
Bond * Bond::Prev(){return (Bond *)d_prev;}


void Bond::Print(char *buf)
{
  char tbuf[256];
  sprintf(tbuf,"Bond: [g: %3d,l: %3d,t: %3d] A1: %3d A2: %3d",d_globalid,d_localid,d_type,d_at1,d_at2);
  strcat(buf,tbuf);
  sprintf(tbuf,"\t K0: %5.2f, Eq: %5.2f",d_k0,d_eq);
  strcat(buf,tbuf);
}

Bond::Bond()
{
  d_at1 = d_at2 = 0;
  d_k0 = d_eq = 0.0;
}

Bond::~Bond(){ }

int Bond::Flatten(List*** lr)
{
  Bond ***ar = (Bond***)lr;
  int i,n = Count();
  Bond *ls = this;
  assert(n > 0);
  Bond **arr = new Bond*[n];
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

 Bond & Bond::operator=(Bond const & bd)
{
  d_prev = d_next = NULL;
  d_at1 = bd.d_at1;
  d_at2 = bd.d_at2;
  d_localid = bd.d_localid;
  d_globalid = bd.d_globalid;
  d_type = bd.d_type;
  d_k0 = bd.d_k0;
  d_eq = bd.d_eq;
  for(int i=0;i<MAXARGS;i++){strcpy(d_args[i],bd.d_args[i]);}
  return *this;
}
 Bond * Bond::Copy()
{
  Bond *bd = new Bond();
  *bd = *this;
  return bd;
}

Bond * Bond::CopyList()
{
 Bond *ls = NULL;
 Bond *tls = this;
 while(tls != NULL)
   {
     Bond *lls = new Bond();
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


 int Bond::operator==(Bond const &bd)
{
  if( (d_at1 == bd.d_at1 && d_at2 == bd.d_at2) ||
      (d_at1 == bd.d_at2 && d_at2 == bd.d_at1) )
  return 1;
  else
  return 0;
}
 int Bond::Contains(List *a)
{
  if(d_at1 == a->GlobalId() ||
     d_at2 == a->GlobalId() )
    return 1;
  return 0;
}





