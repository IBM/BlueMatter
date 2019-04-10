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
 #ifndef _INCLUDED_ANGLE_
#define _INCLUDED_ANGLE_

#ifndef _INCLUDED_LIST_
#include "list.hpp"
#endif

class Angle : public List
{
  public:

  int d_at1;
  int d_at2;
  int d_at3;
  double d_k0;
  double d_eq;
  double d_ubk0;
  double d_ubeq;

public:

  Angle();
  ~Angle();

  int & A1(){return d_at1;}
  int & A2(){return d_at2;}
  int & A3(){return d_at3;}

  double & K0(){return d_k0;}
  double & Eq(){return d_eq;}
  double & UBK0(){return d_ubk0;}
  double & UBEq(){return d_ubeq;}
  virtual Angle * Next();
  virtual Angle * Prev();
  virtual Angle * Copy();
  virtual Angle * CopyList();
  virtual int Flatten(List ***arr);
  virtual void SetNext(List *t){d_next = t;}
  virtual void SetPrev(List *t){d_prev = t;}
  //  virtual  int SameType(List const &a);
  virtual int Contains(List *a);

  Angle &  operator=(Angle const & a);
  int operator==(Angle const & a);

};
#endif
