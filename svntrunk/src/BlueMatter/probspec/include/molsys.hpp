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
 
#ifndef _INCLUDED_MOLSYS_
#define _INCLUDED_MOLSYS_


//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~
//  ~~~  MolSys:  Class that contains a representation of all
//  ~~~           parameters, the assign parameters for a specific
//  ~~~           system, a fragmentation scheme, the resulting
//  ~~~           fragspecs, and the reduced set of unique fragment
//  ~~~           types.  
//  ~~~           Supports PSF and XML input methods.
//  ~~~           Supports HPP and XML output methods.
//  ~~~           After preparation of global and specific systems, 
//  ~~~           DeriveParameterMaps() and then ApplyFragmentionScheme().
//  ~~~           Class is then ready for either HPP or XML output.
//  ~~~
//  ~~~ Author: MCP   
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "syscell.hpp"
#include "fragscheme.hpp"
#include "fragspec.hpp"


#define MAXBUFLEN (1024 * 1024)

class MolecularSystem : public List
{

///////////////////////////////////////////////////////////
//             Molsys private internal functions
///////////////
protected:
  int  CompressTypes(FragSpec *fs);
  int  CompressFragments();
  int  UpdateTermIncidenceLists(Atom *at,FragSpec *fs);
  int  UpdateExternalSiteList(FragSpec *fs);
  int  PrepareFragment(FragSpec *fs);
  int  DeriveParameterMaps();
  int  CheckIfNeighbor(FragSpec *fs,FragSpec *rfg);
  void DeriveFragmentNeighborhood(FragSpec* fs); 
  int  DeriveElementTypes(Atom ** elems);
  int  PrintFragmentInstanceList(char *fPF,int max);
  int  PrintFragmentNeighborTable(char *fPF,int max);
  int  PrintExternalSiteTable(char *fPF,int max);
  int  PrintSiteSpecTable(char *fPF,int max);
  int  PrintFragSpecList(char *fPF, int max);

public:
  int SetHeaderStrings(FragSpec *fs);
  const char * BondTermName(){return d_bondTermName;}
  const char * AngleTermName(){return d_angleTermName;}
  const char * UreyTermName(){return d_ureyTermName;}
  const char * TorsionTermName(){return d_torsionTermName;}
  const char * ImproperTermName(){return d_improperTermName;}
  void SetBondTermName(const char *n)                                       
             {if(d_bondTermName != NULL)delete [] d_bondTermName;              
	     d_bondTermName = new char[strlen(n)+1];strcpy(d_bondTermName,n);}   
  void SetAngleTermName(const char *n)                                      
             {if(d_angleTermName != NULL)delete [] d_angleTermName;            
	     d_angleTermName = new char[strlen(n)+1];strcpy(d_angleTermName,n);}   
  void SetUreyTermName(const char *n)                                       
             {if(d_ureyTermName != NULL)  delete [] d_ureyTermName;            
	     int l = strlen(n) + 1;	     d_ureyTermName = new char[l]; 
	     strcpy(d_ureyTermName,n);}     
  void SetTorsionTermName(const char *n)                           
             {if(d_torsionTermName != NULL)delete [] d_torsionTermName; 
	     d_torsionTermName = new char[strlen(n)+1];strcpy(d_torsionTermName,n);}   
  void SetImproperTermName(const char *n)                              
             {if(d_improperTermName != NULL)delete [] d_improperTermName; 
	     d_improperTermName = new char[strlen(n)+1];strcpy(d_improperTermName,n);}   

  const char * BondStructName(){return d_bondStructName;}
  const char * AngleStructName(){return d_angleStructName;}
  const char * UreyStructName(){return d_ureyStructName;}
  const char * TorsionStructName(){return d_torsionStructName;}
  const char * ImproperStructName(){return d_improperStructName;}
  void SetBondStructName(const char *n)                                
             {if(d_bondStructName != NULL)delete [] d_bondStructName;     
	     d_bondStructName = new char[strlen(n)+1];strcpy(d_bondStructName,n);}   
  void SetAngleStructName(const char *n)                               
             {if(d_angleStructName != NULL)delete [] d_angleStructName;   
	     d_angleStructName = new char[strlen(n)+1];strcpy(d_angleStructName,n);}   
  void SetUreyStructName(const char *n)                                
             {if(d_ureyStructName != NULL)  delete [] d_ureyStructName;
	     int l = strlen(n) + 1;      d_ureyStructName = new char[l]; 
	     strcpy(d_ureyStructName,n);}     
  void SetTorsionStructName(const char *n)                           
             {if(d_torsionStructName != NULL)delete [] d_torsionStructName;
	     d_torsionStructName = new char[strlen(n)+1];strcpy(d_torsionStructName,n);}   
  void SetImproperStructName(const char *n)                             
             {if(d_improperStructName != NULL)delete [] d_improperStructName;
	     d_improperStructName = new char[strlen(n)+1];strcpy(d_improperStructName,n);}   

  int BondCode(){return d_bondCode;}
  int AngleCode(){return d_angleCode;}
  int UreyCode(){return d_ureyCode;}
  int TorsionCode(){return d_torsionCode;}
  int ImproperCode(){return d_improperCode;}
  int ExPairCode(){return d_exPairCode;}
  int Pair14Code(){return d_pair14Code;}

  void SetBondCode(int code){d_bondCode = code;}
  void SetAngleCode(int code){d_angleCode = code;}
  void SetUreyCode(int code){d_ureyCode = code;}
  void SetTorsionCode(int code){d_torsionCode = code;}
  void SetImproperCode(int code){d_improperCode = code;}
  void SetExPairCode(int code){d_exPairCode = code;}
  void SetPair14Code(int code){d_pair14Code = code;}

  SysCell * Globs(){return d_globs;}
  SysCell * Mols(){return d_mols;}
  FragmentationScheme * FragScheme(){return d_frags;}
  FragSpec *FragSpecs(){return d_fragSpecs;}
  FragSpec *ReducedFragSpecs(){return d_redFragSpecs;}

  ///////////////////////////////////////////////////////
  //         XML parsing callbacks
  ////////
public:
  void PushSiteSpec(char *name,char *type, double mass, double charge,
		    double eps,double sigma,double eps14,double sigma14);
  void PushBondSpec(char *s1, char *s2, double k0,double r0);
  void PushAngleSpec(char *s1, char *s2, char *s3, double k0,double r0, double ku, double ru);
  void PushTorsionSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0);
  void PushImproperSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0);


  void AddLennardJonesType(char *type);
  void AddLennardJones14Type(char *type);
  void AddLennardJonesPair(int type1, int type2, const char *eps, const char *sigma);
  void AddLennardJones14Pair(int type1, int type2,  const char *eps, const char *sigma);
  void AddAtomType(char *atypeS,int ord);
  void AddAtomInstance(char *massS,char *chargeS, char *atypeS, char *ljtypeS, char *lj14typeS);
  void AddBondTerm(int site1, int site2, const char *k0S,  const char *thetaS);
  void AddAngleTerm(int site1, int site2, int site3,  const char *k0S,  const char *thetaS);
  void AddUreyTerm(int site1,  int site2, int site3, const char *k0S, const char *thetaS);
  void AddTorsionTerm(int site1,  int site2,  int site3, int site4,  const char *k0S,  const char *multS, const char *thetaS);
  void AddImproperTerm(int site1, int site2,   int site3,  int site4, const char *k0S,   const char *thetaS);
  int PrintLJPairTable(char *fPF,int maxlen);
  int PrintLJ14PairTable(char *fPF,int maxlen);
//////////////////////////////////////////////////////////
//           Data members
/////////////
protected:
  SysCell *d_globs;
  SysCell *d_mols;
  FragmentationScheme *d_frags;
  FragSpec *d_fragSpecs;
  FragSpec *d_redFragSpecs;
  int d_nFrags,d_nRedFrags;
  int d_maxFrag;
  
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
  MolecularSystem();
  ~MolecularSystem();

  ///////////////////////////////////////////////////////////////////////
  //                Input methods
  /////////////////


  void ReadFragmentationScheme(FILE *fragfile);

  void ReadCharmmAtomTypes(FILE *fPF);

  void ReadCharmmParameterFile(FILE *fPF);

  void ReadPsfFile(FILE *fPF);


  ///////////////////////////////////////////////////////////////////////
  //               Processing methods
  /////////////////

  void PreparePeptideFragmentationScheme(FILE *psffile);

  void PrepareSingleFragmentScheme();

  int  Finalize();

  int  ApplyFragmentationScheme();


  ///////////////////////////////////////////////////////////////////////
  //                Output methods
  /////////////////

  int  PrintXMLDescription(char *bufS,int maxlen);

  int  PrintHeaderFile(char *bufS,int maxlen);

};


#endif



