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
 
#ifndef _INCLUDED_LIST_HPP_
#define  _INCLUDED_LIST_HPP_

//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~
//  ~~~  List: Base list class with local id, global id, and
//  ~~~        type code support. Lists are doubly linked. Several
//  ~~~        virtual utility functions are implemented.
//  ~~~
//  ~~~ Author: MCP   
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAXARGS 4
#define MAXARGLEN 50

class List
{
protected:
  List * d_prev;
  List * d_next;
  int d_localid;
  int d_globalid;
  int d_type;
  char d_args[MAXARGS][MAXARGLEN];
public:
  List();
  virtual ~List();
  virtual  void Append(List *bd);
  virtual List * Next();
  virtual List * Prev();
  virtual void SetNext(List *l);
  virtual void SetPrev(List *l);
  int & LocalId();
  int & GlobalId();
  int & Type();
  int Count();
  int Rewind(List **ls);
  int FForward(List **ls);

  virtual  int  RemoveItem(List *head, List *item);
  virtual  int  LocateItem(List *head, List const &item,List **cop);
  int  operator==(List const & l);
  List & operator=(List const & l);
  virtual  List * Copy();
  virtual  List * CopyList();
  virtual  int  Flatten(List*** arr);
  virtual  int  NewType(List *head, List **cop);
  virtual  int  Contains(List *a);
  virtual  int  ListContains(List *a);
  virtual  int  InList(List *at);
  virtual  int  InternalToList(List *at,int min);
  virtual  int  TypeOffset(int type,int &offset);
  virtual  int  LocalIdOffset(int type,int &offset);
  virtual  int  GlobalIdOffset(int type,int &offset);
  List *  GetItem(int i);

  const char * Arg(int i)const {return d_args[i];}
  void SetArg(int i, const char * arg){assert(i<MAXARGS && i>=0);assert(strlen(arg)<MAXARGLEN); strcpy(d_args[i],arg);}

};
#endif

