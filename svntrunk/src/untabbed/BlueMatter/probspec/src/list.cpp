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
 
#include <BlueMatter/list.hpp>

static int num_list_cells = 0;

List::List()
{
num_list_cells++;
  d_prev = d_next = NULL;
  d_localid = d_globalid = d_type = 0;
  for(int i=0;i<MAXARGS;i++){d_args[i][0]='\0';}
}
List::~List()
{
  // YO: Check that correct binding happens
  //assert(0);
num_list_cells--;
assert(num_list_cells >= 0);

  if(d_next != NULL)
    {
      delete d_next;
      d_next = NULL;
    }
  if(d_prev != NULL)
    {
          d_prev->d_next = NULL;
    }
}

void List::Append(List *bd)
{
  if(d_next != NULL)
    {
      List *tbd = d_next;
      while(tbd->d_next != NULL)
  {
    assert(tbd->d_next != tbd);
          tbd = tbd->d_next;
  }
      bd->d_prev = tbd;
      tbd->d_next = bd;
      bd->d_localid = tbd->d_localid + 1;
    }
  else
    {
      d_next = bd;
      bd->d_prev = this;
      bd->d_localid = d_localid + 1;
    }
}
List * List::Next(){return d_next;}
List * List::Prev(){return d_prev;}
void List::SetNext(List *l){d_next = l;}
void List::SetPrev(List *l){d_prev = l;}
int & List::LocalId(){return d_localid;}
int & List::GlobalId(){return d_globalid;}
int & List::Type(){return d_type;}

int List::RemoveItem(List *head, List *item)
{
  List *tbd = head;
  while(tbd != NULL)
    {
      if( *tbd == *item )
        {
          if(tbd->d_prev != NULL)
            {
              tbd->d_prev->d_next = tbd->d_next;
            }
          if(tbd->d_next != NULL)
            {
              tbd->d_next->d_prev = tbd->d_prev;
            }
          tbd->d_prev = tbd->d_next = NULL;
          delete tbd;
        }
    }
        return 1;
}

int List::LocateItem(List *head, List const &item,List **cop)
{
  List  *ls = head;
  if(head != NULL)
    {
      while(ls != NULL)
        {
          if(*ls == item)
            {
              *cop = ls;
              return 1;
            }
          ls = ls->d_next;
        }
    }
  *cop = NULL;
  return 0;
}

 int List::operator==(List const & l)
{
  return (d_globalid == l.d_globalid);
}
 List & List::operator=(List const & l)
{
  d_localid = l.d_localid;
  d_globalid = l.d_globalid;
  d_type = l.d_type;
//  if(l.d_next!=NULL)
//   d_next = l.d_next->Copy();
  for(int i=0;i<MAXARGS;i++){strcpy(d_args[i],l.d_args[i]);}
  return *this;
}

int List::Count()
{
  List *ls = this;
  int i = 1;
  while(ls->d_next != NULL)
    {
      ls = ls->d_next;
      i++;
    }
  return i;
}
List * List::Copy()
{
 List *ls = new List();
 *ls = *this;
 return ls;
}
List * List::CopyList()
{
 List *ls = NULL;
 List *tls = this;
 while(tls != NULL)
   {
     List *lls = new List();
     *lls = *tls;
     if(ls == NULL)
       ls = lls;
     else
       ls->Append(lls);
     tls = tls->d_next;
   }
 return ls;
}

 int List::Flatten(List*** ar)
{
  int i,n = Count();
  List *ls = this;
  assert(n > 0);
  List **arr = new List*[n];
  for(i=0;i<n;i++)
    {
      assert(ls != NULL);
      arr[i] = ls;  //ls->Copy();
      //arr[i]->d_localid = i;
      ls = ls->d_next;
    }
  *ar = arr;
  return n;
}
int List::Rewind(List **ls)
{
  assert(*ls != NULL);
  List *l = *ls;
  while(l->d_prev != NULL)
    {
      assert(l->d_prev != l);
      l = l->d_prev;
    }
  *ls = l;
  return 1;
}
int List::FForward(List **ls)
{
  assert(*ls != NULL);
  List *l = *ls;
  while(l->d_next != NULL)
    {l = l->d_next;}
  *ls = l;
  return 1;
}
int List::NewType(List *head, List **cop)
{
  List *b = head;
  while(b != NULL)
    {
      if(d_type == b->d_type)
        {
          *cop = b;
          return 0;
        }
      b = b->d_next;
    }
  *cop = NULL;
  return 1;
}
 int List::Contains(List *a)
{
  if (d_globalid == a->d_globalid)return 1;
  else return 0;
}

 int List::ListContains(List *a)
{
  List * b = this;
  Rewind(&b);
  while(b != NULL)
    {
      if(b->Contains(a))
        return 1;
      b = b->d_next;
    }
  return 0;
}
  int List::InList(List *at)
{
  List *a = at;
  Rewind(&a);
  while(a != NULL)
    {
      if(a->d_globalid == d_globalid)
        {return 1;}
      a = a->d_next;
    }
  return 0;
}
 int List::InternalToList(List *at,int min)
{
  // WARNING: assumes no duplicates in list
  int cnt = 0;
  List *a = at;
  at->Rewind(&a);
  while(a != NULL)
    {
      if(Contains(a)){cnt++;}
      a = a->d_next;
    }
  if(cnt >= min)
    return 1;
  return 0;
}

int List::TypeOffset(int type,int &offset)
{
  int i=0;
  List *ll = this;
  while(ll != NULL)
    {
      if(ll->d_type == type)
        {
          offset = i;
          return 1;
        }
      i++;
      ll = ll->Next();
    }
  offset = -1;
  return 0;
}

int List::LocalIdOffset(int id,int &offset)
{
  int i=0;
  List *ll = this;
  while(ll != NULL)
    {
      if(ll->d_localid == id)
        {
          offset = i;
          return 1;
        }
      i++;
      ll = ll->Next();
    }
  return 0;
}


int List::GlobalIdOffset(int id,int &offset)
{
  int i=0;
  List *ll = this;
  while(ll != NULL)
    {
      if(ll->d_globalid == id)
        {
          offset = i;
          return 1;
        }
      i++;
      ll = ll->Next();
    }
  return 0;
}

List * List::GetItem(int i)
{
  List *ls = this;
  int j = 0;
  while(ls != NULL)
    {
      if(i == j)break;
      j++;
      ls = ls->Next();
      assert(ls != NULL);
    }
  return ls;
}


