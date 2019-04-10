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
 
#ifndef _INCLUDED_FRAGSPEC__
#define _INCLUDED_FRAGSPEC__

//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~
//  ~~~  FragSpec:  Class that holds a representation of the atoms 
//  ~~~             and terms within and/or across the borders of 
//  ~~~             a fragment. It is used by incrementally adding 
//  ~~~             the appropriate terms and calling finalize().
//  ~~~             The FragSpec can then by output in c++ class
//  ~~~             format by calling PrintHppInfo()
//  ~~~
//  ~~~ Author: MCP   
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "syscell.hpp"
#include "termspec.hpp"
#include "probspecmacs.hpp"
#include "headerhandler.hpp"

///////////////////////////////////////////////////////////
//             FragSpec 
///////////////
class FragSpec : public List
{

  
///////////////////////////////////////////////////////////
//             FragSpec private internal functions
///////////////
public:

  ////////////////////
  // Processing procedures for fragment preparation
  /////


  //_______________________
  //_______________________ void DeriveExcludedAnd14Lists(Atom **atab);
  //  For every internal and external atom:
  //  Derive the excluded and 14 lists from the passed atab
  //  The  excluded and 14 list are output ready upon
  //  completion

  void DeriveExcludedAnd14Lists(Atom **atab);


  //_______________________
  //_______________________ int  UpdateExcludedPairs(Atom *contree);
  //  Used in DeriveExcludedAnd14Lists to add the atoms neighboring each
  //  member of the fragment to the exclusion list.

  int  UpdateExcludedPairs(Atom *contree);


  //_______________________
  //_______________________ int  Update14Pairs(Atom *contree);
  //  Used in DeriveExcludedAnd14Lists to add the atoms in a 1,4 
  // relation with each member of the fragment to the 14 lists.

  int  Update14Pairs(Atom *contree);

  //_______________________
  //_______________________ int  UpdateExternalTerm(TermSpec *t);
  //   Used to set the external flag for each atom referenced in the term

  int  UpdateExternalTerm(TermSpec *t);

  int NumberCodes();

  ////////////////////
  //  Functions for outputing sections of fragspec header
  /////
  int  PrintSiteRefList(char *fPF);
  int  PrintBondList(char *fPF);
  int  PrintAngleList(char *fPF);
  int  PrintTorsionList(char *fPF);
  int  PrintImproperList(char *fPF);
  int  PrintTermTypes(char *fPF);
  int  PrintExcludeSitePairsList(char *fPF);
  int  Print14PairsList(char *fPF);
  int  PrintTermCounts(char *fPF);
  int  PrintCodeList(char *fPF);
  int  PrintFragmentNeighborhood(char *fPF);


  int PrintForceTermInternalItem(int nsites, int *sites, int type, char *fPF);
  int PrintForceTermExternalItem(int nsites, int *sites, int type,  char *fPF);
  const char * BondTermName() const {return d_bondTermName;}
  const char * AngleTermName() const {return d_angleTermName;}
  const char * UreyTermName() const {return d_ureyTermName;}
  const char * TorsionTermName() const {return d_torsionTermName;}
  const char * ImproperTermName() const {return d_improperTermName;}
  void SetBondTermName(const char *n)                          
             {if(d_bondTermName != NULL)delete [] d_bondTermName; 
	     d_bondTermName = new char[strlen(n)+1];strcpy(d_bondTermName,n);}   
  void SetAngleTermName(const char *n)                         
             {if(d_angleTermName != NULL)delete [] d_angleTermName; 
	     d_angleTermName = new char[strlen(n)+1];strcpy(d_angleTermName,n);}   
  void SetUreyTermName(const char *n)                            
             {if(d_ureyTermName != NULL)delete [] d_ureyTermName;   
	     d_ureyTermName = new char[strlen(n)+1];strcpy(d_ureyTermName,n);}   
  void SetTorsionTermName(const char *n)                         
             {if(d_torsionTermName != NULL)delete [] d_torsionTermName;
	     d_torsionTermName = new char[strlen(n)+1];strcpy(d_torsionTermName,n);}   
  void SetImproperTermName(const char *n)                           
             {if(d_improperTermName != NULL)delete [] d_improperTermName; 
	     d_improperTermName = new char[strlen(n)+1];strcpy(d_improperTermName,n);}   

  const char * BondStructName() const {return d_bondStructName;}
  const char * AngleStructName() const {return d_angleStructName;}
  const char * UreyStructName() const {return d_ureyStructName;} 
  const char * TorsionStructName() const {return d_torsionStructName;}
  const char * ImproperStructName() const {return d_improperStructName;}
  void SetBondStructName(const char *n)                                
             {if(d_bondStructName != NULL)delete [] d_bondStructName;     
	     d_bondStructName = new char[strlen(n)+1];strcpy(d_bondStructName,n);}   
  void SetAngleStructName(const char *n)                               
             {if(d_angleStructName != NULL)delete [] d_angleStructName;   
	     d_angleStructName = new char[strlen(n)+1];strcpy(d_angleStructName,n);}   
  void SetUreyStructName(const char *n)                                
             {if(d_ureyStructName != NULL)delete [] d_ureyStructName;     
	     d_ureyStructName = new char[strlen(n)+1];strcpy(d_ureyStructName,n);}   
  void SetTorsionStructName(const char *n)                             
             {if(d_torsionStructName != NULL)delete [] d_torsionStructName; 
	     d_torsionStructName = new char[strlen(n)+1];strcpy(d_torsionStructName,n);}   
  void SetImproperStructName(const char *n)                              
             {if(d_improperStructName != NULL)delete [] d_improperStructName;
	     d_improperStructName = new char[strlen(n)+1];strcpy(d_improperStructName,n);}   

  int BondCode() const {return d_bondCode;}
  int AngleCode() const {return d_angleCode;}
  int UreyCode() const {return d_ureyCode;}
  int TorsionCode() const {return d_torsionCode;}
  int ImproperCode() const {return d_improperCode;}
  int ExPairCode() const {return d_exPairCode;}
  int Pair14Code() const {return d_pair14Code;}

  void SetBondCode(int code){d_bondCode = code;}
  void SetAngleCode(int code){d_angleCode = code;}
  void SetUreyCode(int code){d_ureyCode = code;}
  void SetTorsionCode(int code){d_torsionCode = code;}
  void SetImproperCode(int code){d_improperCode = code;}
  void SetExPairCode(int code){d_exPairCode = code;}
  void SetPair14Code(int code){d_pair14Code = code;}


  ////////////////////
  //   Utility functions
  /////
public:
  int  IsNew(int n,List *ls,int id,int &pos);
  int  IsInternalAtom(Atom *at,int &off);
  int  IsExternalAtom(Atom *at,int &off);
  int  operator==(const FragSpec &f);
  FragSpec & operator=(const FragSpec &f);
  virtual FragSpec * Next(){return (FragSpec *)d_next;}
  virtual FragSpec * Prev(){return (FragSpec *)d_prev;}
  virtual int & Type(){return d_type;}
  virtual int & GlobalId(){return d_globalid;}
  virtual int & LocalId(){return d_localid;}
  virtual FragSpec * Copy();
  virtual FragSpec * CopyList();
  int  Derive1_NAts(int lvl,Atom *at,Atom **atab,Atom **contree);
  int CheckIfOnTorsionList(int a1, int a4);
//////////////////////////////////////////////////////////
//           Data members accessible but not advertized
/////////////
public:
  char name[50];
  int      NumberInternalSites;
  int      NumberExternalSites;
  Atom    *InternalSites;
  Atom    *ExternalSites;
  int      NumberBondTypes;
  Bond    *BondTypes;
  int     *BondParamMap;
  int      NumberInternalBonds;
  int      NumberExternalBonds;
  Bond    *InternalBonds;
  Bond    *ExternalBonds;
  int      NumberAngleTypes;
  Angle   *AngleTypes;
  int     *AngleParamMap;
  int      NumberInternalAngles;
  int      NumberExternalAngles;
  Angle   *InternalAngles;
  Angle   *ExternalAngles;
  int      NumberTorsionTypes;
  Torsion *TorsionTypes;
  Torsion *TorsionParamMap;
  int      NumberInternalTorsions;
  int      NumberExternalTorsions;
  Torsion *InternalTorsions;
  Torsion *ExternalTorsions;
  int      NumberImproperTypes;
  ImproperTorsion * ImproperTypes;
  ImproperTorsion * ImproperParamMap;
  int      NumberInternalImpropers;
  int      NumberExternalImpropers;
  ImproperTorsion * InternalImpropers;
  ImproperTorsion * ExternalImpropers;
  int      NumberExcludedSitePairs;
  ExPair  *ExcludedSitePairs;
  int      NumberPair14SitePairs;
  Pair14  *Pair14SitePairs;
  TermSpec *TermList;
  int NumberTerms;
  int NumberFragmentNeighbors;
  List * FragmentNeighborList;

  char * d_bondTermName, * d_bondStructName;
  char * d_angleTermName, * d_angleStructName;
  char * d_ureyTermName, * d_ureyStructName;
  char * d_torsionTermName, * d_torsionStructName;
  char * d_improperTermName, * d_improperStructName;

  int d_bondCode;
  int d_angleCode;
  int d_ureyCode;
  int d_torsionCode;
  int d_improperCode;
  int d_exPairCode;
  int d_pair14Code;


public:

  FragSpec();
  ~FragSpec();


  /////////////////////////////////////////////////////////
  //              Incremental build-up methods
  ////////////////
  int  AddInternalSite(Atom *a);
  int  AddExternalSite(Atom *a);
  int  AddInternalBond(Bond *b);
  int  AddExternalBond(Bond *b);
  int  AddInternalAngle(Angle *a);
  int  AddExternalAngle(Angle *a);
  int  AddInternalTorsion(Torsion *t);
  int  AddExternalTorsion(Torsion *t);
  int  AddInternalImproper(ImproperTorsion *it);
  int  AddExternalImproper(ImproperTorsion * it);
  int  AddTerm(TermSpec *t);
  int  AddBondTerm(Bond *b);
  int  AddAngleTerm(Angle *a);
  int  AddTorsionTerm(Torsion *b);
  int  AddImproperTerm(ImproperTorsion *a);



  /////////////////////////////////////////////////////////
  //              Internal preparation procedure
  ////////////////  
  void Finalize(Atom **atab);

  /////////////////////////////////////////////////////////
  //              I/O methods
  ////////////////
  int PrintHppInfo(char *buff);

};


#endif



