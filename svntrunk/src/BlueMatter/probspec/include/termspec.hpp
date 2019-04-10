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
 #ifndef _INCLUDED_TERMSPEC_HPP
#define _INCLUDED_TERMSPEC_HPP

//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~
//  ~~~  TermSpec:  An abstract representation of a term. The
//  ~~~             term has a type code which is used to interpret
//  ~~~             the meaning of its parameter values
//  ~~~
//  ~~~ Author: MCP   
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "terms.hpp"



#define BONDCODE 1
#define ANGLECODE 2
#define TORSIONCODE 3
#define IMPROPERCODE 4
#define EXCLUDEDCODE 5
#define UBCODE 6
#define PAIR14CODE 7
#define OPLSTORSIONCODE  8
#define OPLSIMPROPERCODE 9


class TermSpec : public List
{
public:

  int mTermType;
  int mTermParams;
  int mA1;
  int mA2;
  int mA3;
  int mA4;
  int sa1;
  int sa2;
  int sa3;
  int sa4;
  double K0;
  double Eq;
  double K1;
  double R1;
  double mult;
  double mScore;

  TermSpec(){}
  ~TermSpec(){}

  virtual TermSpec * Next();
  virtual TermSpec * Prev();
  virtual TermSpec *Copy();
  virtual TermSpec *CopyList();
  virtual int & Type(){return d_type;}
  virtual int & LocalId(){return d_localid;}
  virtual int & GlobalId(){return d_globalid;}
  virtual int Flatten(List ***arr);
  void SetNext(List *l);
  void SetPrev(List *l);
  ////////////////////////////////////////////////////////
  //            Term assignment methods
  //////////////
  void SetBondParams(Bond *t, int code);
  void SetAngleParams( Angle *t, int code);
  void SetUreyBradleyParams( Angle *t, int code);
  void SetTorsionParams( Torsion *t, int code);
  void SetImproperParams(ImproperTorsion *t, int code);
  void SetExPairParams(ExPair *t, int code);
  void SetPair14Params(Pair14 *t, int code);
  void SetOplsaTorsionParams( Torsion *t);
  void SetCharmmTorsionParams( Torsion *t);
  void SetCharmmImproperParams(ImproperTorsion *t);
  void SetOplsaImproperParams( ImproperTorsion *t);


  ////////////////////////////////////////////////////////
  //            Compare functions for qsort
  //////////////
  static  int CompareDoubles(const void *A, const void *B);
  static  int CompareScore(const void *termA, const void *termB);
  static  int CompareSortedAtnums(const void *termA, const void *termB);
  static  int CompareAtnums(const void *termA, const void *termB);
  double Score(double base);

};
#endif
