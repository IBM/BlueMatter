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
 
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "math.h"

#define MAXFRAGS 20000
#define MAXATOMS 40000
#define NAMELEN 5
#define MAXBONDS 40000
#define MAXANGLES 80000
#define MAXTORSIONS 100000
#define MAXTERMS 200000

#define SORTTERMS 0

#define TYPE_REFERENCES 1

#define BONDCODE 1
#define ANGLECODE 2
#define TORSIONCODE 3
#define IMPROPERCODE 4
#define EXCLUDEDCODE 5





// *********************************************
// 
//  FragSpec class 
//
//  Holds all info needed to contain internal and
//  external terms
//
class FragSpec
{
public:                
char name[50];
int    NumberInternalSites; 
int    NumberExternalSites; 
int *InternalSites;
int *ExternalSites;
int    NumberBondTypes; 
int * BondTypes;
int *BondParamMap;
int    NumberInternalBonds;  
int    NumberExternalBonds;  
int *InternalBonds;
int *ExternalBonds;
int    NumberAngleTypes;  
int * AngleTypes;
int *AngleParamMap;
int    NumberInternalAngles;   
int    NumberExternalAngles;   
int * InternalAngles;
int * ExternalAngles;
int    NumberTorsionTypes;   
int * TorsionTypes;
int* TorsionParamMap;
int    NumberInternalTorsions;   
int    NumberExternalTorsions;   
int * InternalTorsions;
int * ExternalTorsions;
int    NumberImproperTypes;   
int * ImproperTypes;
int* ImproperParamMap;
int    NumberInternalImpropers;   
int    NumberExternalImpropers;   
int * InternalImpropers;
int * ExternalImpropers;
int NumberExcludedSitePairs;
int *ExcludedSitePairs;
int NumberTerms;
};

// *************************************************************
// *************************************************************
//   Class Definitions
//

// *********************************************
//  
//   Fragment class is only used to represent 
//   the fragmentation scheme
class Fragment
{
public:
  int NumberInternalSites;
  int *InternalSites;
};

// *********************************************
//
//   TermSpec class holds the individual terms 
//   These are prepared during output and used
//   to derive the code list.
//   Several compare functions are implemented
//   for various ways of sorting. The SORTTERMS
//   switch enables/disables sorting of terms
class TermSpec
{
public:
static   int CompareDoubles(const void *A, const void *B)
    {
      double const *dAP = ( double *) A;
      double const *dBP = ( double *) B;
      double dA = *dAP;
      double dB = *dBP;
      
      if(dA > dB)
	return 1;
      else if (dA < dB)
	return -1;
      return 0;
    }
static   int CompareScore(const void *termA, const void *termB)
    {
      TermSpec const *a = (TermSpec *)termA;
      TermSpec const *b = (TermSpec *)termB;
      if(a->mScore < b->mScore)
	return -1;
      else if(a->mScore > b->mScore)
	return 1;
      else
	return 0;
    }

static  int CompareSortedAtnums(const void *termA, const void *termB)
    {
      TermSpec const *A = (TermSpec *)termA;
      TermSpec const *B = (TermSpec *)termB;

      int ids1[4];
      int ids2[4];
      int nats;

      ids1[0] = A->mA1;
      ids2[0] = B->mA1;
      ids1[1] = A->mA2;
      ids2[1] = B->mA2;


      switch(A->mTermType)
	{
	case BONDCODE:
	  nats = 2;
	  break;
	case EXCLUDEDCODE:
	  nats = 2;
	  break;
	case ANGLECODE:	  
	  nats = 3;
	  ids1[2] = A->mA3;
	  ids2[2] = B->mA3;
	  break;
	case TORSIONCODE:
	case IMPROPERCODE:
	  ids1[3] = A->mA4;
	  ids2[3] = B->mA4;
	  break;
	default: assert(0); 
	}
      int i;
      int flg = 1;
      int temp;
      while(flg)
	{
	  flg = 0;
	  for(i=0;i<nats-1;i++)
	    {
	      if(ids1[i] > ids1[i+1])
		{ flg = 1; temp = ids1[i]; ids1[i] = ids1[i+1]; ids1[i+1] = temp;}
	      if(ids2[i] > ids2[i+1])
		{ flg = 1; temp = ids2[i]; ids2[i] = ids2[i+1]; ids2[i+1] = temp;}
	    }
	}
      for(i=0;i<nats;i++)
	{
	  if(ids1[i] > ids2[i])
	    { return 1;}
	  else if(ids1[i] < ids2[i])
	    { return -1;}
	}
      return 0;
    }


static  int CompareAtnums(const void *termA, const void *termB)
    {
      TermSpec const *A = (TermSpec *)termA;
      TermSpec const *B = (TermSpec *)termB;

      int ids1[4];
      int ids2[4];
      int nats;

      ids1[0] = A->mA1;
      ids2[0] = B->mA1;
      ids1[1] = A->mA2;
      ids2[1] = B->mA2;


      switch(A->mTermType)
	{
	case BONDCODE:
	  nats = 2;
	  break;
	case EXCLUDEDCODE:
	  nats = 2;
	  break;
	case ANGLECODE:	  
	  nats = 3;
	  ids1[2] = A->mA3;
	  ids2[2] = B->mA3;
	  break;
	case TORSIONCODE:
	case IMPROPERCODE:
	  ids1[3] = A->mA4;
	  ids2[3] = B->mA4;
	  break;
	default: assert(0); 
	}
      int i;

      for(i=0;i<nats;i++)
	{
	  if(ids1[i] > ids2[i])
	    { return 1;}
	  else if(ids1[i] < ids2[i])
	    { return -1;}
	}
      return 0;
    }


  double Score(double base)
    {
      mScore = (((base * mA1)*base + mA2)*base + mA3)*base + mA4;
      return mScore;
    }
  int mTermType;
  int mTermParams;
  int mA1;
  int mA2;
  int mA3;
  int mA4;
  double K0;
  double Eq;
  double mult;
  double mScore;
};


// ****************************************************
// ****************************************************
//
//         Global Variable section
// CONVENTION:
//    Variables in all caps refer to all possible types
//    Variables that are capitalized refer to specific molecule's
//
// ****************************************************

// ***********   Atom type variables
int NA;
int Na;
int NNAMES=0;
char ANAMES[MAXATOMS][NAMELEN];
int NNames;
typedef char NAME[NAMELEN];
NAME *ANames;
int ANUMS[MAXATOMS];
int ANums[MAXATOMS];
double MASS[MAXATOMS];
double Mass[MAXATOMS];
double CHARGE[MAXATOMS];
double Charge[MAXATOMS];
int NATypes;
int ATypes[MAXATOMS];
int ATypeMap[MAXATOMS];

// **********    Bond variables
int NB=0;
int Nb=0;
int BA1[MAXBONDS];
int ba1[MAXBONDS];
int BA2[MAXBONDS];
int ba2[MAXBONDS];
double BK0[MAXBONDS];
double BEQ[MAXBONDS];
int NBTypes;
int BTypes[MAXBONDS];
int BTypeMap[MAXBONDS];


// **********    Angle variables
int NANG=0;
int Nang=0;
int AA1[MAXANGLES];
int aa1[MAXANGLES];
int AA2[MAXANGLES];
int aa2[MAXANGLES];
int AA3[MAXANGLES];
int aa3[MAXANGLES];
double AK0[MAXANGLES];
double AEQ[MAXANGLES];
int NAngTypes;
int AngTypes[MAXANGLES];
int AngTypeMap[MAXANGLES];


// **********    Torsion variables
int NT=0;
int Nt=0;
int TA1[MAXTORSIONS];
int ta1[MAXTORSIONS];
int TA2[MAXTORSIONS];
int ta2[MAXTORSIONS];
int TA3[MAXTORSIONS];
int ta3[MAXTORSIONS];
int TA4[MAXTORSIONS];
int ta4[MAXTORSIONS];
double TK0[MAXTORSIONS];
int WELLS[MAXTORSIONS];
double TEQ[MAXTORSIONS];
int NTTypes;
int TTypes[MAXTORSIONS];
int TTypeMap[MAXTORSIONS];


// **********    Improper variables
int NI=0;
int Ni=0;
int IA1[MAXTORSIONS];
int ia1[MAXTORSIONS];
int IA2[MAXTORSIONS];
int ia2[MAXTORSIONS];
int IA3[MAXTORSIONS];
int ia3[MAXTORSIONS];
int IA4[MAXTORSIONS];
int ia4[MAXTORSIONS];
double IK0[MAXTORSIONS];
double IEQ[MAXTORSIONS];
int NITypes;
int ITypes[MAXTORSIONS];
int ITypeMap[MAXTORSIONS];

// **********    Lennard Jones variables
int NLJ=0;
double LJEPS[MAXATOMS];
double LJRMIN[MAXATOMS];
int  LJTYPE[MAXATOMS];
int LJTypeMap[MAXATOMS];

// ********************************************
// **********    Arrays holding partitioned system
//
Fragment the_frags[MAXFRAGS];          // prepared by reading partion scheme
FragSpec the_fragspecs[MAXFRAGS];      // array holding fragments internal and external terms
FragSpec the_redfragspecs[MAXFRAGS];   // nonredundant, reduced array of fragspecs
int the_RedFragMap[MAXFRAGS];          // mapping giving reduced fragment given fragment
int NFrags=0;
int NFragSpecs=0;
int NRedFrags=0;




// **************************************
// Global working arrays
// 
TermSpec the_Terms[MAXTERMS];
int nTerms = 0;

double the_Doubles[MAXTERMS];
int nDoubles;



// *************************************************************
// *************************************************************
//
//     Useful macros
//

#define OPEN_BLOCK(x) fprintf(fPF,"\n%s = \n{\n\t",x)
#define OPEN_CLASS(x) fprintf(fPF,"\n%s\n{\n\t",x)
#define BLOCKSTR(a1) fprintf(fPF,"{%s}",a1)
#define BLOCK1(a1) fprintf(fPF,"{%4d}",a1)
#define BLOCK2(a1,a2) fprintf(fPF,"{%4d,%4d}",a1,a2)
#define BLOCK2X(a1,a2) fprintf(fPF,"{%4d,  0x%x}",a1,a2)
#define BLOCK3(a1,a2,a3) fprintf(fPF,"{%4d,%4d,%4d}",a1,a2,a3)
#define BLOCK3X(a1,a2,a3) fprintf(fPF,"{%4d,%4d,  0x%x}",a1,a2,a3)
#define END_BLOCK fprintf(fPF,"\n};\n")
#define LINE fprintf(fPF,"\n")
#define CHECK(arg) strstr(bufS,arg)
#define COMA fprintf(fPF," , ")
#define INIT_COUNT(cls,cnt,val) fprintf(fPF,"const int %s::%s = %d;\n",cls,cnt,val)


#define STRNG(str) fprintf(fPF,"%s",str);
#define TAB fprintf(fPF,"\t");
#define SITETYPESPEC(id) fprintf(fPF,"{%4d}",id)
#define SITESPEC(ms,cg,eps,rmin) fprintf(fPF,"{ %lf, %lf * mChargeConversion, %lf, %lf}",ms,cg,eps,rmin)
#define BONDSPEC(a1,a2,typ) { char A1S[10],A2S[10]; \
  if(a1<fs->NumberInternalSites){sprintf(A1S,"{0,%4d}",a1);}else{sprintf(A1S,"{1,%4d}",a1 - fs->NumberInternalSites);}\
  if(a2<fs->NumberInternalSites){sprintf(A2S,"{0,%4d}",a2);}else{sprintf(A2S,"{1,%4d}",a2 - fs->NumberInternalSites);}\
  fprintf(fPF,"{ %s, %s, %4d }",A1S,A2S,typ); } 
#define ANGLESPEC(a1,a2,a3,typ) { char A1S[10],A2S[10],A3S[10]; \
  if(a1<fs->NumberInternalSites){sprintf(A1S,"{0,%4d}",a1);}else{sprintf(A1S,"{1,%4d}",a1 - fs->NumberInternalSites);}\
  if(a2<fs->NumberInternalSites){sprintf(A2S,"{0,%4d}",a2);}else{sprintf(A2S,"{1,%4d}",a2 - fs->NumberInternalSites);}\
  if(a3<fs->NumberInternalSites){sprintf(A3S,"{0,%4d}",a3);}else{sprintf(A3S,"{1,%4d}",a3 - fs->NumberInternalSites);}\
  fprintf(fPF,"{ %s, %s, %s, %4d }",A1S,A2S,A3S,typ); } 
#define TORSIONSPEC(a1,a2,a3,a4,typ) { char A1S[10],A2S[10],A3S[10],A4S[10]; \
  if(a1<fs->NumberInternalSites){sprintf(A1S,"{0,%4d}",a1);}else{sprintf(A1S,"{1,%4d}",a1 - fs->NumberInternalSites);}\
  if(a2<fs->NumberInternalSites){sprintf(A2S,"{0,%4d}",a2);}else{sprintf(A2S,"{1,%4d}",a2 - fs->NumberInternalSites);}\
  if(a3<fs->NumberInternalSites){sprintf(A3S,"{0,%4d}",a3);}else{sprintf(A3S,"{1,%4d}",a3 - fs->NumberInternalSites);}\
  if(a4<fs->NumberInternalSites){sprintf(A4S,"{0,%4d}",a4);}else{sprintf(A4S,"{1,%4d}",a4 - fs->NumberInternalSites);}\
  fprintf(fPF,"{ %s, %s, %s, %s, %4d }",A1S,A2S,A3S,A4S,typ); } 

#define BONDTYPESPEC(a1,a2) fprintf(fPF,"{ %lf, %lf }",a1,a2)
#define ANGLETYPESPEC(a1,a2) fprintf(fPF,"{ %lf, %lf * (M_PI/180.0) }",a1,a2)
#define TORSIONTYPESPEC(a1,a2,a3) fprintf(fPF,"{ %lf, %d, %lf * (M_PI/180.0) }",a1,a2,a3)
#define IMPROPERTYPESPEC(a1,a2) fprintf(fPF,"{ %lf, %lf * (M_PI/180.0) }",a1,a2)
#define EXCLUDESITEPAIR(a1,a2,a3) if(a2>=0){fprintf(fPF,"{0,%4d}, {0,%4d}",a1,a3);} \
                                      else{fprintf(fPF,"{0,%4d}, {1,%4d}",a1,a3-1);}

#define TERMSPEC(ind,s1,a1,s2,a2,s3,a3,s4,a4) fprintf(fPF,"{ %4d, {%3d,%4d}, {%3d,%4d}, {%3d,%4d}, {%3d,%4d}, %4d, %4d, %4d}", \
			      the_Terms[ind].mTermType,                \ 
				      s1,a1,s2,a2,s3,a3,s4,a4,           \
			      (int) the_Terms[ind].K0,                \ 
			      (int) the_Terms[ind].Eq,                \ 
			      (int) the_Terms[ind].mult)

#define TYPEDTERMSPEC(ind,s1,a1,s2,a2,s3,a3,s4,a4) fprintf(fPF,"{ %4d, {%3d,%4d}, {%3d,%4d}, {%3d,%4d}, {%3d,%4d}, %4d}", \
			      the_Terms[ind].mTermType,                \ 
				      s1,a1,s2,a2,s3,a3,s4,a4,           \
			      (int) the_Terms[ind].mTermParams)


#define DOUBLESPEC(ind) fprintf(fPF,"{ %lf }",the_Doubles[ind])

#define LINEIN fgets(bufS,1000,fPF);
#define BUFSTR(str) sscanf(bufS,"%s",str); 
#define BUFINT(str) sscanf(bufS," %d",&str); 
#define SCANSTR(str) fscanf(fPF," %s",str); 
#define SCANumberInt(str) fscanf(fPF," %d",&str); 
#define SCANumberInt2(s1,s2) fscanf(fPF," %d %d",&s1,&s2); 
#define SCANumberInt3(s1,s2,s3) fscanf(fPF," %d %d %d",&s1,&s2,&s3); 
#define SCANumberInt4(s1,s2,s3,s4) fscanf(fPF," %d %d %d %d",&s1,&s2,&s3,&s4); 
#define SCANDBL(str) fscanf(fPF,"%lf",&str); 
#define SCANDBL2(s1,s2) fscanf(fPF," %lf %lf",&s1,&s2); 
#define SCANDBL3(s1,s2,s3) fscanf(fPF," %lf %lf %lf",&s1,&s2,&s3); 
#define SCANDBL4(s1,s2,s3,s4) fscanf(fPF," %lf %lf %lf %lf",&s1,&s2,&s3,&s4); 

#define CATSTR(str) sprintf(tbufS,"%s",str);strcat(bufS,tbufS); 
#define CATDELIM sprintf(tbufS,",\n\t");strcat(bufS,tbufS); 
#define CATINT(str) sprintf(tbufS,"%4d",str);;strcat(bufS,tbufS); 
#define CATSITE(s0,s1,s2) sprintf(tbufS,"{%2d,%2d,%4d}",s0,s1,s2);;strcat(bufS,tbufS); 
#define CATINT2(s1,s2) sprintf(tbufS,"%4d %4d",s1,s2);;strcat(bufS,tbufS); 
#define CATINT3(s1,s2,s3) sprintf(tbufS,"%4d %4d",s1,s2);;strcat(bufS,tbufS); 
#define CATINT4(s1,s2,s3,s4) sprintf(tbufS,"%4d %4d %4d %4d",s1,s2,s3,s4);strcat(bufS,tbufS); 
#define CATDBL(str) sprintf(tbufS,"%lf",&str);;strcat(bufS,tbufS);  
#define CATDBL2(s1,s2) sprintf(tbufS,"%lf %lf",s1,s2);;strcat(bufS,tbufS); 
#define CATDBL3(s1,s2,s3) sprintf(tbufS,"%lf %lf %lf",s1,s2,s3);;strcat(bufS,tbufS);  
#define CATDBL4(s1,s2,s3,s4) sprintf(tbufS,"%lf %lf %lf %lf",s1,s2,s3,s4);;strcat(bufS,tbufS);  


// *************************************************************
// *************************************************************
//   Prototypes
// 

int AtypeNum(char *tag);
int AtypeName(int typ,char *name);
int UpdateExternalSites(int site,int nInsites,int *Insites,int &nExsites,int *Excites);
int IntersectCount(int alen,int *listA,int blen, int *listB);
int IsNew(int len, int *map, int target, int &index);
int IsNewString(int len, char **map, char* target, int &index);
int IsNewName(int len, char **map, char* target, int &index);
IsNewFrag(int len,char **map,char *target,int &ind);
void InitAtypes(FILE *fPF);
void InitPar(FILE *fPF);
void InitPsf(FILE *fPF);
void InitFrags(FILE *fPF);
void SelectTypes();
void ReduceFrags();
void WriteProbDef(FILE *fPF);
void PrepWater(int n);
void PrepSingleFrag();
void WriteMaterializationFile(FILE *fPF);
int AtomsAreInRange(int a1,int a2,FragSpec *fs);
void     PrepareExcludedAtomLists();
void PrepDefaultFrags(FILE *fPF);

void ParseXMLSpec(FILE *fPF);

#include "pushparams.hpp"


// *************************************************************
// *************************************************************
//                    MAIN
// *************************************************************
// *************************************************************
int main(int argc, char **argv)
{

  if(argc == 1)
    {
      printf("form is %s <top file> <param file> <psf file> <frag scheme> \n",*argv);
      printf("\t\t\t problem spec header according to fragmentation scheme\n");

      printf("\t or   %s <top file> <param file> <psf file> <DEFAULT> \n",*argv);
      printf("\t\t\t problem spec header one fragment per amino acid\n");

      printf("\t or   %s <top file> <param file> <nWaters>  \n",*argv);
      printf("\t\t\t problem spec header for n water molecules (TIPS3P\n");

      printf("\t\t\t {<fragfile> | DEFAULT} < xmlspec \n");
      printf("\t  parse xmlspec and fragment by fragfile  \n",*argv);
      return 1;
    }

    FILE *psf = NULL;
    FILE *frag = NULL;


    ANames = new NAME[MAXATOMS];


    // If the first arg is not DEFAULT or a frag file, assume that CHARMM files are supplied
    if(strcmp(argv[1],"DEFAULT") | !strstr(argv[1],".frg"))
      {
	FILE *top = fopen(argv[1],"r");
	FILE *par = fopen(argv[2],"r");
	InitAtypes(top);
	InitPar(par);
	fclose(top);
	fclose(par);

	if(argc == 5)
	  {
	    psf = fopen(argv[3],"r");
	    InitPsf(psf);
	    if(strstr(argv[4],"DEFAULT"))
	      {
		rewind(psf);
		PrepDefaultFrags(psf);
	      }
	    else
	      {
		frag = fopen(argv[4],"r");
		InitFrags(frag);
		fclose(frag);
	      }
	    fclose(psf);
	  }
	else   //  Prepared file of TIP3P water
	  {
	    int n;
	    sscanf(argv[3],"%d",&n);
	    PrepWater(n);
	  }
	SelectTypes();
      }
    else if(!strcmp(argv[1],"DEFAULT"))      
      {
	ParseXMLSpec(stdin);
	PrepSingleFrag();
      }
    else if (strstr(argv[1],".frg"))
      {
	ParseXMLSpec(stdin);
	frag = fopen(argv[1],"r");
	InitFrags(frag);
	fclose(frag);
      }
    else      
      {
	printf("unknown args\n");
	assert(0);
      }
      

    PrepareExcludedAtomLists();
    ReduceFrags();
    WriteProbDef(stdout);

    //    printf("//=================MATERIALIZE.HPP==================\n");
    FILE* mPF = fopen("Materialize.hpp","w");
    WriteMaterializationFile(mPF);
    fclose(mPF);

}

void ParseXMLSpec(FILE *fPF){} // To Be Implemented

void PrepSingleFrag()
{
    char bufS[1000];
  int i,j,n,nas;

  the_frags[0].NumberInternalSites = 1;
  the_frags[0].InternalSites = new int[Na];
  for(j=0;j<Na;j++)
    {
      the_frags[0].InternalSites[j] = j;
    }
  return;
}

void PrepDefaultFrags(FILE *fPF)
{
  char bufS[1000];
  char tag1[20],tag2[20],tag3[20],tag4[20];
  int fragbuf[500];
  int f;

  LINEIN;
  LINEIN;
  LINEIN;
  int i,j,k,d,n,tail;
  sscanf(bufS,"%d",&n);
  for(i=0;i<n;i++)
    {LINEIN;}
  LINEIN;
  LINEIN;
  BUFINT(Na);

  // Initiate begining fragment - define ammonium group as fragment
  the_frags[0].NumberInternalSites = 4;
  the_frags[0].InternalSites = new int[4];
  for(i=0;i<4;i++)
    {
      LINEIN;
      the_frags[0].InternalSites[i] = i;
    }

  f = 1;
  j = 0;
  n = 4;
  tail = 1;
  LINEIN;
  for(i=5;i<=Na;i++)
    {
      sscanf(bufS,"%d %s %d ",&d,tag1,&k);

      if(k == f)
	{
	  fragbuf[j] = i;
	  j++;
	}
      else
	{
	  the_frags[f].NumberInternalSites = j;
	  if(j) the_frags[f].InternalSites = new int[j];
	  for(k = 0;k<j;k++)
	    {
	      the_frags[f].InternalSites[k] = fragbuf[k] - 1;
	    }
	  f++;
	  fragbuf[0] = i;
	  if(strstr(tag1,"WAT"))
	    {
	      tail = 0;
	      break;
	    }
	  j = 1;
	}
      LINEIN;
    }
  if(tail)
    {
	  the_frags[f].NumberInternalSites = j;
	  if(j) the_frags[f].InternalSites = new int[j];
	  for(k = 0;k<j;k++)
	    {
	      the_frags[f].InternalSites[k] = fragbuf[k] - 1;
	    }
	  f++;
    }
  while(strstr(tag1,"WAT"))
    {
      j = 3;
      the_frags[f].NumberInternalSites = j;
      the_frags[f].InternalSites = new int[j];
      for(k = 0;k<j;k++)
	{
	  the_frags[f].InternalSites[k] = i - 1;
	  i++;
	}
      if(i >= Na) break;
      f++;
      LINEIN;
      LINEIN;
      LINEIN;
      tag1[0]='\0';
      sscanf(bufS,"%d %s %d ",&d,tag1,&k);
    }
  NFrags = f;
  return;
}
void InitFrags(FILE *fPF)
{
  char bufS[1000];
  int i,j,n,nas;

  LINEIN;
  BUFINT(NFrags);
  for(i=0;i<NFrags;i++)
    {
      LINEIN;
      BUFINT(n);
      the_frags[i].NumberInternalSites = n;
      if(n)the_frags[i].InternalSites = new int[n];
      for(j=0;j<n;j++)
	{
	  SCANumberInt(the_frags[i].InternalSites[j]);
	  the_frags[i].InternalSites[j]--;
	}
      LINEIN;
    }
  return;
}

void InitPsf(FILE *fPF)
{
  char bufS[1000];
  char tag1[20],tag2[20],tag3[20],tag4[20];

  LINEIN;
  LINEIN;
  LINEIN;
  int i,j,k,n;
  sscanf(bufS,"%d",&n);
  for(i=0;i<n;i++)
    {LINEIN;}
  LINEIN;
  LINEIN;
  BUFINT(Na);

  for(i=0;i<Na;i++)
    {
      LINEIN;
      sscanf(bufS,"%d %s %d %s %s %s %lf %lf",&j,tag1,&k,tag2,tag3,tag4,&Charge[i],&Mass[i]);
      if(sscanf(tag4,"%d",&ANums[i]))
	{
	  assert(AtypeName(ANums[i],ANames[i]));
	}
      else 
	{
	  assert(ANums[i] = AtypeNum(tag4));
	  AtypeName(ANums[i],ANames[i]);
	}
    }

  //---------------------
  LINEIN;
  LINEIN;
  BUFINT(Nb);

  for(i=0;i<Nb;i++)
    {
      SCANumberInt2(ba1[i],ba2[i]); 
      ba1[i]--;      ba2[i]--;
    }
  //----------------------
  LINEIN;
  LINEIN;
  LINEIN;
  BUFINT(Nang);

  for(i=0;i<Nang;i++)
    {
      SCANumberInt3(aa1[i],aa2[i],aa3[i]);
      aa1[i]--;aa2[i]--;aa3[i]--;
    }
  //----------------------

  LINEIN;
  LINEIN;
  LINEIN;
  BUFINT(Nt);
  int found = 0;


  for(i=0;i<Nt;i++)
    {
      SCANumberInt4(ta1[i],ta2[i],ta3[i],ta4[i]);
      ta1[i]--;ta2[i]--;ta3[i]--;ta4[i]--;
      int a1 = ANums[ta1[i]];
      int a2 = ANums[ta2[i]];
      int a3 = ANums[ta3[i]];
      int a4 = ANums[ta4[i]];
      found = 0;
      for(j=0;j<NT;j++)
	{
	  if( (TA1[j] == a1 && TA2[j] == a2 && 
	       TA3[j] == a3 && TA4[j] == a4) ||
	      (TA1[j] == a4 && TA2[j] == a3 && 
	       TA3[j] == a2 && TA4[j] == a1) )
	    {
	      TTypeMap[i] = j;
	      found++;
	      if(TA1[j+1] == TA1[j] && TA2[j+1] == TA2[j] && 
		 TA3[j+1] == TA3[j] && TA4[j+1] == TA4[j])
		{
		      i++; Nt++; 
		      ta1[i] = ta1[i-1];
		      ta2[i] = ta2[i-1];
		      ta3[i] = ta3[i-1];
		      ta4[i] = ta4[i-1];
		}
	    }
	}  

      if(!found)
	{
	  for(j=0;j<NT;j++)
	    {
	      if( (TA1[j] == 100 && TA4[j] == 100 && 
		   TA2[j] == a2 && TA3[j] == a3)||
		  (TA1[j] == 100 && TA4[j] == 100 && 
		   TA3[j] == a2 && TA2[j] == a3))
		{
		  TTypeMap[i] = j;		  
		  if(TA1[j+1] == TA1[j] && TA2[j+1] == TA2[j] && 
		     TA3[j+1] == TA3[j] && TA4[j+1] == TA4[j])
		    {
		      i++; Nt++; 
		      ta1[i] = ta1[i-1];
		      ta2[i] = ta2[i-1];
		      ta3[i] = ta3[i-1];
		      ta4[i] = ta4[i-1];
		    }
		}
	    }  
	}


    }
  //----------------------
//  		{
//  		  do
//  		    {
//  		      i++; Nt++; j++;
//  		      ta1[i] = ta1[i-1];
//  		      ta2[i] = ta2[i-1];
//  		      ta3[i] = ta3[i-1];
//  		      ta4[i] = ta4[i-1];
//  		    }while(TA1[j-1] == TA1[j] && TA2[j-1] == TA2[j] && 
//  			   TA3[j-1] == TA3[j] && TA4[j-1] == TA4[j]);
//  		  i--; Nt--;
//  		  break;
//  		}

  LINEIN;
  LINEIN;
  LINEIN;
  BUFINT(Ni);

  for(i=0;i<Ni;i++)
    {
      SCANumberInt4(ia1[i],ia2[i],ia3[i],ia4[i]);
      ia1[i]--;ia2[i]--;ia3[i]--;ia4[i]--;
    }


}



void InitAtypes(FILE *fPF)
{
  char bufS[1000], tag1[50];
  int i,j,k,n;
  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("MASS")) break;
      LINEIN;
    }
  NNAMES = 0;
  while(CHECK("MASS"))
    {
      sscanf(bufS,"%s %d %s",tag1,&ANUMS[NNAMES],ANAMES[NNAMES]);
      NNAMES++;
      LINEIN;
    }
  ANUMS[NNAMES] = 100;
  strcpy(ANAMES[NNAMES],"X");
  NNAMES++;
}

void InitPar(FILE *fPF)
{
  char bufS[1000],tag1[20],tag2[20],tag3[20],tag4[20];

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("BONDS")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  NB=0;
  //Now read for bond param reading  while(!feof(fPF))
  while(!feof(fPF))
    {
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %lf %lf",tag1,tag2,&BK0[NB],&BEQ[NB]);
      BA1[NB] = AtypeNum(tag1);
      BA2[NB] = AtypeNum(tag2);
      NB++;
      LINEIN;
      LINEIN;
    }

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("ANGLES")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  NANG = 0;
  //Now read for angle param reading
  while(!feof(fPF))
    {
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %s %lf %lf",tag1,tag2,tag3,&AK0[NANG],&AEQ[NANG]);
      AA1[NANG] = AtypeNum(tag1);
      AA2[NANG] = AtypeNum(tag2);
      AA3[NANG] = AtypeNum(tag3);
      NANG++;
      LINEIN;
      LINEIN;
    }

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("DIHEDRALS")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  NT=0;
  //Now read for torsion param reading
  while(!feof(fPF))
    {
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %s %s %lf %d %lf",tag1,tag2,tag3,tag4,
		 &TK0[NT],&WELLS[NT],&TEQ[NT]);
      TA1[NT] = AtypeNum(tag1);
      TA2[NT] = AtypeNum(tag2);
      TA3[NT] = AtypeNum(tag3);
      TA4[NT] = AtypeNum(tag4);
      NT++;
      LINEIN;
      LINEIN;
    }

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("IMPROPER")) break;
      LINEIN;
    }
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  NI=0;
  //Now read for improper torsion param reading
  while(!feof(fPF))
    {
      int jnk;
      if(bufS[0] == '\n')break;
      sscanf(bufS,"%s %s %s %s %lf %d %lf",tag1,tag2,tag3,tag4,
		 &IK0[NI],&jnk,&IEQ[NI]);
      IA1[NI] = AtypeNum(tag1);
      IA2[NI] = AtypeNum(tag2);
      IA3[NI] = AtypeNum(tag3);
      IA4[NI] = AtypeNum(tag4);
      NI++;
      LINEIN;
      LINEIN;
    }
  

  LINEIN;
  while(!feof(fPF))
    {
      if(CHECK("NONBONDED")) break;
      LINEIN;
    }
  LINEIN;
  LINEIN;
  LINEIN;
  while(bufS[0] == '!')
    {
      LINEIN;
    }
  NLJ=0;
  //Now read for nonbonded param reading
  while(!feof(fPF))
    {
      double jnk;
      while(bufS[0] == '!')
	{
	  LINEIN;
	}
      if(bufS[0] == '\n'){LINEIN;}
      if(CHECK("HBOND CUTHB")){break;}
      sscanf(bufS,"%s %lf %lf %lf",tag1,&jnk,&LJEPS[NLJ],&LJRMIN[NLJ]);
      LJTYPE[NLJ] = AtypeNum(tag1);
      NLJ++;
      LINEIN;
      LINEIN;
    }
  

  return;
}

// *********************************************************************
// *********************************************************************
// Poorly named function that does most of the work of this program.
// This function assigns types and separates the internal and external
// terms for each fragment 
// 
void SelectTypes()
{


  int i,j,k,n,f;
  int t[4];
  char bufS[100];
  char **typestrings = new char*[Na];
  const int intlen = 500;
  int tempmap[intlen];
  int tempintern[intlen];
  int tempextern[intlen];
  int tempextsite[intlen];
  int NumberIntSite,NumberIntern,NumberExtern,NumberExtSites;

  NATypes = 0;       
  for(i=0;i<Na;i++)
    {
      int a1 = ANums[i];
      for(j=0;j<NNAMES;j++)
	{
	  if(ANUMS[j] == a1)
	    {
	      sprintf(bufS,"%d %f %f",a1,Mass[i],Charge[i]);
	      if(IsNewString(NATypes,typestrings,bufS,k))
		{
		  ATypes[NATypes] = a1;  // gives type given typenumber
		  Mass[NATypes] = Mass[i]; // flatten mass array
		  Charge[NATypes] = Charge[i]; //flatten charge array
		  ATypeMap[i] = NATypes; // gives typenumber given atnum
		  typestrings[NATypes] = new char[strlen(bufS)];
		  strcpy(typestrings[NATypes],bufS);
		  NATypes++;
		}
	      else
		{
		  ATypeMap[i] = k;
		}
	    }
	}
    }

  for(i=0;i<Na;i++)
    {
      delete typestrings[i];
    }
  delete typestrings;

  for(i=0;i<NATypes;i++)
    {
      int a1 = ATypes[i];
      for(j=0;j<NLJ;j++)
	{
	  if( LJTYPE[j] == a1)
	    {
	      LJTypeMap[i] = j;	      // gives LJparm given typenum
	    }
	}
    }



  for(i=0;i<Nb;i++)
    {
      int a1 = ANums[ba1[i]];
      int a2 = ANums[ba2[i]];
      for(j=0;j<NB;j++)
	{
	  if( (BA1[j] == a1 && BA2[j] == a2) ||
	      (BA1[j] == a2 && BA2[j] == a1))
	    {
	      BTypeMap[i] = j;	      
	    }
	}
    }
  for(i=0;i<Nang;i++)
    {
      int a1 = ANums[aa1[i]];
      int a2 = ANums[aa2[i]];
      int a3 = ANums[aa3[i]];
      for(j=0;j<NANG;j++)
	{
	  if( (AA1[j] == a1 && AA2[j] == a2 && AA3[j] == a3) ||
	      (AA1[j] == a3 && AA2[j] == a2 && AA3[j] == a1))
	    {
	      AngTypeMap[i] = j;	      
	    }
	}
    }
  
  // *************   Already prepared in InitPsf due to multiple terms
//    for(i=0;i<Nt;i++)
//      {
//        int a1 = ANums[ta1[i]];
//        int a2 = ANums[ta2[i]];
//        int a3 = ANums[ta3[i]];
//        int a4 = ANums[ta4[i]];
//        for(j=0;j<NT;j++)
//  	{
//  	  if( (TA1[j] == a1 && TA2[j] == a2 && 
//  	       TA3[j] == a3 && TA4[j] == a4) ||
//  	      (TA1[j] == a4 && TA2[j] == a3 && 
//  	       TA3[j] == a2 && TA4[j] == a1) ||
//  	      (TA1[j] == 100 && TA4[j] == 100 && TA2[j] == a2 && TA3[j] == a3)||
//  	      (TA1[j] == 100 && TA4[j] == 100 && TA3[j] == a2 && TA2[j] == a3))
//  	    {
	      
//  	      if(TA1[j+1] == TA1[j] && TA2[j+1] == TA2[j] && 
//  		 TA3[j+1] == TA3[j] && TA4[j+1] == TA4[j])
//  		{
//  		  do
//  		    {
//  		      TTypeMap[i] = j; j++; i++;
//  		    }while(TA1[j-1] == TA1[j] && TA2[j-1] == TA2[j] && 
//  			   TA3[j-1] == TA3[j] && TA4[j-1] == TA4[j]);
//  		  i--; 
//  		  break;
//  		}
//  	      else
//  		{
//  		  TTypeMap[i] = j;
//  		}
//  	    }
//  	}  
//      }

  for(i=0;i<Ni;i++)
    {
      int a1 = ANums[ia1[i]];
      int a2 = ANums[ia2[i]];
      int a3 = ANums[ia3[i]];
      int a4 = ANums[ia4[i]];
      for(j=0;j<NI;j++)
	{
	  if( (IA1[j] == a1 && IA2[j] == a2 && 
	       IA3[j] == a3 && IA4[j] == a4) ||
	      (IA1[j] == a4 && IA2[j] == a3 && 
	       IA3[j] == a2 && IA4[j] == a1) ||
	      (IA2[j] == 100 && IA3[j] == 100 && IA1[j] == a1 && IA4[j] == a4)||
	      (IA2[j] == 100 && IA3[j] == 100 && IA4[j] == a1 && IA1[j] == a4))
	    {
	      ITypeMap[i] = j;	      
	    }
	}
    }

  
  for(f=0;f<NFrags;f++)
    {
      Fragment *frag = &the_frags[f];
      FragSpec *fs = &the_fragspecs[f];
      
      NumberExtSites = 0;
      n = fs->NumberInternalSites = frag->NumberInternalSites;
      if(n) fs->InternalSites = new int[frag->NumberInternalSites];
      for(i=0;i<n;i++)
	{
	  fs->InternalSites[i] = frag->InternalSites[i];
	}
      NumberIntSite = 0;

      
      // All global params are in place
      // construct maps of fragment terms to global terms


      ///////// Derive Internal and External Bonds
      NumberIntern = NumberExtern = 0;
      for(i = 0;i<Nb;i++)
	{
	//    if(BK0[BTypeMap[i]] == 0.0)
	//    continue;
	  t[0] = ba1[i]; t[1] = ba2[i]; 
	  int c = IntersectCount(2,t,frag->NumberInternalSites,frag->InternalSites);
	  switch(c)
	    {
	    case 1:
	      tempextern[NumberExtern] = i;
	      NumberExtern++;
	      break;
	    case 2:
	      tempintern[NumberIntern] = i;
	      NumberIntern++;
	      break;
	    }
	}
      
      // Copy internal and external bond info into fragment
      n = fs->NumberInternalBonds = NumberIntern;
      if(n) fs->InternalBonds = new int[n];
      for(i=0;i<n;i++)
	fs->InternalBonds[i] = tempintern[i];

      n = fs->NumberExternalBonds = NumberExtern;
      if(n) fs->ExternalBonds = new int[n];
      for(i=0;i<n;i++)
	{
	  k = fs->ExternalBonds[i] = tempextern[i];
	  UpdateExternalSites(ba1[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(ba2[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	}

     ///////// Derive Internal and External Angles
      NumberIntern = NumberExtern = 0;
      for(i = 0;i<Nang;i++)
	{
	  t[0] = aa1[i]; t[1] = aa2[i]; t[2] = aa3[i];  
	  int c = IntersectCount(3,t,frag->NumberInternalSites,frag->InternalSites);
	  switch(c)
	    {
	    case 1:
	    case 2:
	      tempextern[NumberExtern] = i;
	      NumberExtern++;
	      break;
	    case 3:
	      tempintern[NumberIntern] = i;
	      NumberIntern++;
	      break;
	    }
	}
          
      // Copy internal and external bond info into fragment
      n = fs->NumberInternalAngles = NumberIntern;
      if(n) fs->InternalAngles = new int[n];
      for(i=0;i<n;i++)
	fs->InternalAngles[i] = tempintern[i];

      n = fs->NumberExternalAngles = NumberExtern;
      if(n) fs->ExternalAngles = new int[n];
      for(i=0;i<n;i++)
	{
	  k = fs->ExternalAngles[i] = tempextern[i];
	  UpdateExternalSites(aa1[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(aa2[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(aa3[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	}
   ///////// Derive Internal and External Torsions
      NumberIntern = NumberExtern = 0;
      for(i = 0;i<Nt;i++)
	{
	  t[0] = ta1[i]; t[1] = ta2[i]; t[2] = ta3[i];  t[3] = ta4[i];
	  int c = IntersectCount(4,t,frag->NumberInternalSites,frag->InternalSites);
	  switch(c)
	    {
	    case 1:
	    case 2:
	    case 3:
	      tempextern[NumberExtern] = i;
	      NumberExtern++;
	      break;
	    case 4:
	      tempintern[NumberIntern] = i;
	      NumberIntern++;
	      break;
	    }
	}
      
      // Copy internal and external bond info into fragment
      n = fs->NumberInternalTorsions = NumberIntern;
      if(n) fs->InternalTorsions = new int[n];
      for(i=0;i<n;i++)
	fs->InternalTorsions[i] = tempintern[i];

      n = fs->NumberExternalTorsions = NumberExtern;
      if(n) fs->ExternalTorsions = new int[n];
      for(i=0;i<n;i++)
	{
	  k = fs->ExternalTorsions[i] = tempextern[i];
	  UpdateExternalSites(ta1[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(ta2[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(ta3[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(ta4[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	}

      n = fs->NumberExternalSites = NumberExtSites;
      if(n) fs->ExternalSites = new int[n];
      for(i=0;i<n;i++)
	{
	  fs->ExternalSites[i] = tempextsite[i];
	}
 


   ///////// Derive Internal and External Impropers
      NumberIntern = NumberExtern = 0;
      for(i = 0;i<Ni;i++)
	{
	  t[0] = ia1[i]; t[1] = ia2[i]; t[2] = ia3[i];  t[3] = ia4[i];
	  int c = IntersectCount(4,t,frag->NumberInternalSites,frag->InternalSites);
	  switch(c)
	    {
	    case 1:
	    case 2:
	    case 3:
	      tempextern[NumberExtern] = i;
	      NumberExtern++;
	      break;
	    case 4:
	      tempintern[NumberIntern] = i;
	      NumberIntern++;
	      break;
	    }
	}
      
      // Copy internal and external bond info into fragment
      n = fs->NumberInternalImpropers = NumberIntern;
      if(n) fs->InternalImpropers = new int[n];
      for(i=0;i<n;i++)
	fs->InternalImpropers[i] = tempintern[i];

      n = fs->NumberExternalImpropers = NumberExtern;
      if(n) fs->ExternalImpropers = new int[n];
      for(i=0;i<n;i++)
	{
	  k = fs->ExternalImpropers[i] = tempextern[i];
	  UpdateExternalSites(ia1[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(ia2[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(ia3[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	  UpdateExternalSites(ia4[k],fs->NumberInternalSites,fs->InternalSites,NumberExtSites,tempextsite);
	}

      n = fs->NumberExternalSites = NumberExtSites;
      if(n) fs->ExternalSites = new int[n];
      for(i=0;i<n;i++)
	{
	  fs->ExternalSites[i] = tempextsite[i];
	}



      // compress maps into type lists and repoint to type
      n = fs->NumberInternalBonds + fs->NumberExternalBonds;
      if (n) fs->BondTypes =  new int[n];
      if (n) fs->BondParamMap = new int[n];
      fs->NumberBondTypes = 0;
      for(i=0;i<fs->NumberInternalBonds;i++)
	{
	  n = BTypeMap[fs->InternalBonds[i]];
	  if(IsNew(fs->NumberBondTypes,fs->BondTypes,n,k))
	    {
	      fs->BondTypes[fs->NumberBondTypes] = n;
	      fs->BondParamMap[i] = fs->NumberBondTypes;      
	      fs->NumberBondTypes++;
	    }
	  else
	    {
	      fs->BondParamMap[i] = k;
	    }
	}
    
     for(i=0;i<fs->NumberExternalBonds;i++)
	{
	  n = BTypeMap[fs->ExternalBonds[i]];
	  if(IsNew(fs->NumberBondTypes,fs->BondTypes,n,k))
	    {
	      fs->BondTypes[fs->NumberBondTypes] = n;
	      fs->BondParamMap[i + fs->NumberInternalBonds] = fs->NumberBondTypes;      
	      fs->NumberBondTypes++;
	    }
	  else
	    {
	      fs->BondParamMap[i + fs->NumberInternalBonds] = k;
	    }
	}


      
      n = fs->NumberInternalAngles + fs->NumberExternalAngles;
      if (n) fs->AngleTypes = new int[n];
      if (n) fs->AngleParamMap = new int[n];
      fs->NumberAngleTypes = 0;
      for(i=0;i<fs->NumberInternalAngles;i++)
	{
	  n = AngTypeMap[fs->InternalAngles[i]];
	  if(IsNew(fs->NumberAngleTypes,fs->AngleTypes,n,k))
	    {
	      fs->AngleTypes[fs->NumberAngleTypes] = n;
	      fs->AngleParamMap[i] = fs->NumberAngleTypes;
	      fs->NumberAngleTypes++;
	    }
	  else
	    {
	      fs->AngleParamMap[i] = k;
	    }
	}

      for(i=0;i<fs->NumberExternalAngles;i++)
	{
	  n = AngTypeMap[fs->ExternalAngles[i]];
	  if(IsNew(fs->NumberAngleTypes,fs->AngleTypes,n,k))
	    {
	      fs->AngleTypes[fs->NumberAngleTypes] = n;
	      fs->AngleParamMap[i + fs->NumberInternalAngles] = fs->NumberAngleTypes;
	      fs->NumberAngleTypes++;
	    }
	  else
	    {
	      fs->AngleParamMap[i + fs->NumberInternalAngles] = k;
	    }
	}


      n = fs->NumberInternalTorsions + fs->NumberExternalTorsions;
      if (n) fs->TorsionTypes = new int[n];
      if (n) fs->TorsionParamMap = new int[n];
      fs->NumberTorsionTypes = 0;
      for(i=0;i<fs->NumberInternalTorsions;i++)
	{
	  n = TTypeMap[fs->InternalTorsions[i]];
	  if(IsNew(fs->NumberTorsionTypes,fs->TorsionTypes,n,k))
	    {
	      fs->TorsionTypes[fs->NumberTorsionTypes] = n;
	      fs->TorsionParamMap[i] = fs->NumberTorsionTypes;
	      fs->NumberTorsionTypes++;
	    }
	  else
	    {
	      fs->TorsionParamMap[i] = k;
	    }
	}
  
      for(i=0;i<fs->NumberExternalTorsions;i++)
	{
	  n = TTypeMap[fs->ExternalTorsions[i]];
	  if(IsNew(fs->NumberTorsionTypes,fs->TorsionTypes,n,k))
	    {
	      fs->TorsionTypes[fs->NumberTorsionTypes] = n;
	      fs->TorsionParamMap[i + fs->NumberInternalTorsions] = fs->NumberTorsionTypes;
	      fs->NumberTorsionTypes++;
	    }
	  else
	    {
	      fs->TorsionParamMap[i + fs->NumberInternalTorsions] = k;
	    }
	}      


      n = fs->NumberInternalImpropers + fs->NumberExternalImpropers;
      if (n) fs->ImproperTypes = new int[n];
      if (n) fs->ImproperParamMap = new int[n];
      fs->NumberImproperTypes = 0;
      for(i=0;i<fs->NumberInternalImpropers;i++)
	{
	  n = ITypeMap[fs->InternalImpropers[i]];
	  if(IsNew(fs->NumberImproperTypes,fs->ImproperTypes,n,k))
	    {
	      fs->ImproperTypes[fs->NumberImproperTypes] = n;
	      fs->ImproperParamMap[i] = fs->NumberImproperTypes;
	      fs->NumberImproperTypes++;
	    }
	  else
	    {
	      fs->ImproperParamMap[i] = k;
	    }
	}
  
      for(i=0;i<fs->NumberExternalImpropers;i++)
	{
	  n = ITypeMap[fs->ExternalImpropers[i]];
	  if(IsNew(fs->NumberImproperTypes,fs->ImproperTypes,n,k))
	    {
 	      fs->ImproperTypes[fs->NumberImproperTypes] = n;
	      fs->ImproperParamMap[i + fs->NumberInternalImpropers] = fs->NumberImproperTypes;
	      fs->NumberImproperTypes++;
	    }
	  else
	    {
	      fs->ImproperParamMap[i + fs->NumberInternalImpropers] = k;
	    }
	}      
    }
  return;
}

void PrepareExcludedAtomLists()
{
  int i,j,k,f;
  int CurrentExcludedAtoms[MAXATOMS];
  int nExcludedAtoms = 0;

  for(f=0;f<NFrags;f++)
    {
      Fragment *frag = &the_frags[f];
      FragSpec *fs = &the_fragspecs[f];
      for(i=0;i<fs->NumberInternalSites;i++)
	{
	  int a1 = fs->InternalSites[i];
	  for(j=0;j<fs->NumberInternalSites;j++)
	    {
	      int a2 = fs->InternalSites[j];
	      if(a1 < a2)
		{
		  if(AtomsAreInRange(a1,a2,fs))
		    {
		      CurrentExcludedAtoms[nExcludedAtoms] = i;
		      nExcludedAtoms++;
		      CurrentExcludedAtoms[nExcludedAtoms] = j;
		      nExcludedAtoms++;
		    }
		}
	    }

	  for(j=0;j<fs->NumberExternalSites;j++)
	    {
	      int a2 = fs->ExternalSites[j];
	      if(a1 < a2)
		{
		  if(AtomsAreInRange(a1,a2,fs))
		    {
		      CurrentExcludedAtoms[nExcludedAtoms] = i;
		      nExcludedAtoms++;
		      CurrentExcludedAtoms[nExcludedAtoms] = -j-1;
		      nExcludedAtoms++;
		    }
		}
	    }
	}
      // First Must be internal. 
      // Those that fail the ordering condition will get picked up elsewhere
      fs->NumberExcludedSitePairs = nExcludedAtoms;
      if(nExcludedAtoms) fs->ExcludedSitePairs = new int[nExcludedAtoms];
      for(i=0;i<nExcludedAtoms;i++)
	{
	  fs->ExcludedSitePairs[i] = CurrentExcludedAtoms[i];
	}
      nExcludedAtoms = 0;
    }
}

int AtomsAreInRange(int a1,int a2,FragSpec *fs)
{
  int i,j,n;
  for(i=0;i<fs->NumberInternalBonds;i++)
    {
      n = fs->InternalBonds[i];
      if((a1 == ba1[n] && a2 == ba2[n]) ||
	 (a2 == ba1[n] && a1 == ba2[n]))
	{
	  return 1;   
	}
    }
  for(i=0;i<fs->NumberExternalBonds;i++)
    {
      n = fs->ExternalBonds[i];
      if((a1 == ba1[n] && a2 == ba2[n]) ||
	 (a2 == ba1[n] && a1 == ba2[n]))
	{
	  return 1;   
	}
    }

  for(i=0;i<fs->NumberInternalAngles;i++)
    {
      n = fs->InternalAngles[i];
      int i1 = aa1[n];
      int i2 = aa2[n];
      int i3 = aa3[n];
      if((a1 == aa1[n] && (a2 == aa2[n])) ||
	 (a2 == aa1[n] && (a1 == aa2[n])) ||
	 (a1 == aa1[n] && (a2 == aa3[n])) ||
	 (a2 == aa1[n] && (a1 == aa3[n])) ||
	 (a1 == aa2[n] && (a2 == aa3[n])) ||
	 (a2 == aa2[n] && (a1 == aa3[n])))
	{
	  return 1;   
	}
    }
  for(i=0;i<fs->NumberExternalAngles;i++)
    {
      n = fs->ExternalAngles[i];
      if((a1 == aa1[n] && a2 == aa2[n]) ||
	 (a2 == aa1[n] && a1 == aa2[n]) ||
	 (a1 == aa1[n] && a2 == aa3[n]) ||
	 (a2 == aa1[n] && a1 == aa3[n]) ||
	 (a1 == aa2[n] && a2 == aa3[n]) ||
	 (a2 == aa2[n] && a1 == aa3[n]))
	{
	  return 1;   
	}
    }

  for(i=0;i<fs->NumberInternalTorsions;i++)
    {
      n = fs->InternalTorsions[i];
      if((a1 == ta1[n] && a2 == ta2[n]) ||
	 (a2 == ta1[n] && a1 == ta2[n]) ||
	 (a1 == ta1[n] && a2 == ta3[n]) ||
	 (a2 == ta1[n] && a1 == ta3[n]) ||
	 (a1 == ta1[n] && a2 == ta4[n]) ||
	 (a2 == ta1[n] && a1 == ta4[n]) ||
	 (a1 == ta2[n] && a2 == ta3[n]) ||
	 (a2 == ta2[n] && a1 == ta3[n]) ||
	 (a1 == ta2[n] && a2 == ta4[n]) ||
	 (a2 == ta2[n] && a1 == ta4[n]) ||
	 (a1 == ta3[n] && a2 == ta4[n]) ||
	 (a2 == ta3[n] && a1 == ta4[n]))
	{
	  return 1;   
	}
    }

  for(i=0;i<fs->NumberExternalTorsions;i++)
    {
      n = fs->ExternalTorsions[i];
      if((a1 == ta1[n] && a2 == ta2[n]) ||
	 (a2 == ta1[n] && a1 == ta2[n]) ||
	 (a1 == ta1[n] && a2 == ta3[n]) ||
	 (a2 == ta1[n] && a1 == ta3[n]) ||
	 (a1 == ta1[n] && a2 == ta4[n]) ||
	 (a2 == ta1[n] && a1 == ta4[n]) ||
	 (a1 == ta2[n] && a2 == ta3[n]) ||
	 (a2 == ta2[n] && a1 == ta3[n]) ||
	 (a1 == ta2[n] && a2 == ta4[n]) ||
	 (a2 == ta2[n] && a1 == ta4[n]) ||
	 (a1 == ta3[n] && a2 == ta4[n]) ||
	 (a2 == ta3[n] && a1 == ta4[n]))
	{
	  return 1;   
	}
    }


 for(i=0;i<fs->NumberInternalImpropers;i++)
    {
      n = fs->InternalImpropers[i];
      if((a1 == ia1[n] && a2 == ia2[n]) ||
	 (a2 == ia1[n] && a1 == ia2[n]) ||
	 (a1 == ia1[n] && a2 == ia3[n]) ||
	 (a2 == ia1[n] && a1 == ia3[n]) ||
	 (a1 == ia1[n] && a2 == ia4[n]) ||
	 (a2 == ia1[n] && a1 == ia4[n]) ||
	 (a1 == ia2[n] && a2 == ia3[n]) ||
	 (a2 == ia2[n] && a1 == ia3[n]) ||
	 (a1 == ia2[n] && a2 == ia4[n]) ||
	 (a2 == ia2[n] && a1 == ia4[n]) ||
	 (a1 == ia3[n] && a2 == ia4[n]) ||
	 (a2 == ia3[n] && a1 == ia4[n]))
	{
	  return 1;   
	}
    }

  for(i=0;i<fs->NumberExternalImpropers;i++)
    {
      n = fs->ExternalImpropers[i];
      if((a1 == ia1[n] && a2 == ia2[n]) ||
	 (a2 == ia1[n] && a1 == ia2[n]) ||
	 (a1 == ia1[n] && a2 == ia3[n]) ||
	 (a2 == ia1[n] && a1 == ia3[n]) ||
	 (a1 == ia1[n] && a2 == ia4[n]) ||
	 (a2 == ia1[n] && a1 == ia4[n]) ||
	 (a1 == ia2[n] && a2 == ia3[n]) ||
	 (a2 == ia2[n] && a1 == ia3[n]) ||
	 (a1 == ia2[n] && a2 == ia4[n]) ||
	 (a2 == ia2[n] && a1 == ia4[n]) ||
	 (a1 == ia3[n] && a2 == ia4[n]) ||
	 (a2 == ia3[n] && a1 == ia4[n]))
	{
	  return 1;   
	}
    }

  return 0;

}

void ReduceFrags()
{
  char bufS[500000],tbufS[100];
  int i,j,k,f,a1,a2,a3,a4,n;
  char *RedFrags[MAXFRAGS];
  NRedFrags = 0;

  bufS[0] = '\0';
  for(f=0;f<NFrags;f++)
    {
      FragSpec *fs = &the_fragspecs[f];
      CATINT(fs->NumberInternalSites);
      CATINT(fs->NumberExternalSites);
      CATINT(fs->NumberInternalBonds);
      CATINT(fs->NumberExternalBonds);
      CATINT(fs->NumberInternalAngles);
      CATINT(fs->NumberExternalAngles);
      CATINT(fs->NumberInternalTorsions);
      CATINT(fs->NumberExternalTorsions);
      CATINT(fs->NumberInternalImpropers);
      CATINT(fs->NumberExternalImpropers);
      CATINT(fs->NumberBondTypes);
      CATINT(fs->NumberAngleTypes);
      CATINT(fs->NumberTorsionTypes);
      for(i=0;i<fs->NumberInternalSites;i++)
	{
	  CATDBL2(Mass[fs->InternalSites[i]],Charge[fs->InternalSites[i]]);
	}
      for(i=0;i<fs->NumberExternalSites;i++)
	{
	  CATDBL2(Mass[fs->ExternalSites[i]],Charge[fs->ExternalSites[i]]);
	}
      for(i=0;i<fs->NumberInternalBonds;i++)
	{
	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ba1[fs->InternalBonds[i]],a1))
	    {
	      printf("atom 1 (%d) of bond %d in frag %d is not on internal site table\n",
		   ba1[fs->InternalBonds[i]],i,f  );
		assert(0);
	    }
	
	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ba2[fs->InternalBonds[i]],a2))
	    {
	      printf("atom 2 (%d) of bond %d in frag %d is not on internal site table\n",
		   ba2[fs->InternalBonds[i]],i,f  );
		assert(0);
	    }
	    CATINT2(a1,a2);
	    n = fs->BondTypes[fs->BondParamMap[i]];
	    CATDBL2(BK0[n],BEQ[n]);	 
	    
	}
    
      for(i=0;i<fs->NumberExternalBonds;i++)
	{
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ba1[fs->ExternalBonds[i]],a1))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ba1[fs->ExternalBonds[i]],a1))
		{assert(0);}
	    }
	  else 
	    a1 += fs->NumberInternalSites;

	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ba2[fs->ExternalBonds[i]],a2))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ba2[fs->ExternalBonds[i]],a2))
		assert(0);
	    }
	  else 
	    a2 += fs->NumberInternalSites;

	  CATINT2(a1,a2);
	  n = fs->BondTypes[fs->BondParamMap[i + fs->NumberInternalBonds]];
	  CATDBL2(BK0[n],BEQ[n]);	 
	}


     for(i=0;i<fs->NumberInternalAngles;i++)
	{
	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,aa1[fs->InternalAngles[i]],a1))
	    {
	      printf("atom 1 (%d) of angle %d in frag %d is not on internal site table\n",
		   aa1[fs->InternalAngles[i]],i,f  );
		assert(0);
	    }

	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,aa2[fs->InternalAngles[i]],a2))
	    {
	      printf("atom 2 (%d) of angle %d in frag %d is not on internal site table\n",
		   aa2[fs->InternalAngles[i]],i,f  );
		assert(0);
	    }

	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,aa3[fs->InternalAngles[i]],a3))
	    {
	      printf("atom 3 (%d) of angle %d in frag %d is not on internal site table\n",
		   aa3[fs->InternalAngles[i]],i,f  );
		assert(0);
	    }
	    CATINT3(a1,a2,a3);
	    n = fs->AngleTypes[fs->AngleParamMap[i]];
	    CATDBL2(AK0[n],AEQ[n]);
	}

      for(i=0;i<fs->NumberExternalAngles;i++)
	{
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,aa1[fs->ExternalAngles[i]],a1))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,aa1[fs->ExternalAngles[i]],a1))
		{assert(0);}
	    }
	  else 
	    a1 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,aa2[fs->ExternalAngles[i]],a2))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,aa2[fs->ExternalAngles[i]],a2))
		{assert(0);}
	    }
	  else 
	    a2 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,aa3[fs->ExternalAngles[i]],a3))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,aa3[fs->ExternalAngles[i]],a3))
		{assert(0);}
	    }
	  else 
	    a3 += fs->NumberInternalSites;

	    CATINT3(a1,a2,a3);
	    n = fs->AngleTypes[fs->AngleParamMap[i + fs->NumberInternalAngles]];
	    CATDBL2(AK0[n],AEQ[n]);
	}





     for(i=0;i<fs->NumberInternalTorsions;i++)
	{
	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta1[fs->InternalTorsions[i]],a1))
	    {
	      printf("atom 1 (%d) of torsion %d in frag %d is not on internal site table\n",
		   ta1[fs->InternalTorsions[i]],i,f  );
		assert(0);
	    }

	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta2[fs->InternalTorsions[i]],a2))
	    {
	      printf("atom 2 (%d) of torsion %d in frag %d is not on internal site table\n",
		   ta2[fs->InternalTorsions[i]],i,f  );
		assert(0);
	    }

	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta3[fs->InternalTorsions[i]],a3))
	    {
	      printf("atom 3 (%d) of torsion %d in frag %d is not on internal site table\n",
		   ta3[fs->InternalTorsions[i]],i,f  );
		assert(0);
	    }
	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta4[fs->InternalTorsions[i]],a4))
	    {
	      printf("atom 3 (%d) of torsion %d in frag %d is not on internal site table\n",
		   ta3[fs->InternalTorsions[i]],i,f  );
		assert(0);
	    }
	    CATINT4(a1,a2,a3,a4);
	    n = fs->TorsionTypes[fs->TorsionParamMap[i]];
	    CATDBL2(TK0[n],TEQ[n]);
	    CATINT(WELLS[n]);
	}

      for(i=0;i<fs->NumberExternalTorsions;i++)
	{
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta1[fs->ExternalTorsions[i]],a1))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta1[fs->ExternalTorsions[i]],a1))
		{assert(0);}
	    }
	  else 
	    a1 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta2[fs->ExternalTorsions[i]],a2))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta2[fs->ExternalTorsions[i]],a2))
		{assert(0);}
	    }
	  else 
	    a2 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta3[fs->ExternalTorsions[i]],a3))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta3[fs->ExternalTorsions[i]],a3))
		{assert(0);}
	    }
	  else 
	    a3 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta4[fs->ExternalTorsions[i]],a4))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ta4[fs->ExternalTorsions[i]],a4))
		{assert(0);}
	    }
	  else 
	    a4 += fs->NumberInternalSites;

	    CATINT4(a1,a2,a3,a4);
	    n = fs->TorsionTypes[fs->TorsionParamMap[i + fs->NumberInternalTorsions]];
	    CATDBL2(TK0[n],TEQ[n]);
	}



     for(i=0;i<fs->NumberInternalImpropers;i++)
	{
	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia1[fs->InternalImpropers[i]],a1))
	    {
	      printf("atom 1 (%d) of improper %d in frag %d is not on internal site table\n",
		   ia1[fs->InternalImpropers[i]],i,f  );
		assert(0);
	    }

	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia2[fs->InternalImpropers[i]],a2))
	    {
	      printf("atom 2 (%d) of improper %d in frag %d is not on internal site table\n",
		   ia2[fs->InternalImpropers[i]],i,f  );
		assert(0);
	    }

	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia3[fs->InternalImpropers[i]],a3))
	    {
	      printf("atom 3 (%d) of improper %d in frag %d is not on internal site table\n",
		   ia3[fs->InternalImpropers[i]],i,f  );
		assert(0);
	    }
	  if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia4[fs->InternalImpropers[i]],a4))
	    {
	      printf("atom 3 (%d) of improper %d in frag %d is not on internal site table\n",
		   ia3[fs->InternalImpropers[i]],i,f  );
		assert(0);
	    }
	    CATINT4(a1,a2,a3,a4);
	    n = fs->ImproperTypes[fs->ImproperParamMap[i]];
	    CATDBL2(IK0[n],IEQ[n]);
	}

      for(i=0;i<fs->NumberExternalImpropers;i++)
	{
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia1[fs->ExternalImpropers[i]],a1))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia1[fs->ExternalImpropers[i]],a1))
		{assert(0);}
	    }
	  else 
	    a1 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia2[fs->ExternalImpropers[i]],a2))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia2[fs->ExternalImpropers[i]],a2))
		{assert(0);}
	    }
	  else 
	    a2 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia3[fs->ExternalImpropers[i]],a3))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia3[fs->ExternalImpropers[i]],a3))
		{assert(0);}
	    }
	  else 
	    a3 += fs->NumberInternalSites;
	  if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia4[fs->ExternalImpropers[i]],a4))
	    {
	      if(IsNew(fs->NumberInternalSites,fs->InternalSites,ia4[fs->ExternalImpropers[i]],a4))
		{assert(0);}
	    }
	  else 
	    a4 += fs->NumberInternalSites;

	    CATINT4(a1,a2,a3,a4);
	    n = fs->ImproperTypes[fs->ImproperParamMap[i + fs->NumberInternalImpropers]];
	    CATDBL2(IK0[n],IEQ[n]);
	}



      assert(strlen(bufS));

      if(IsNewFrag(NRedFrags,RedFrags,bufS,k))
	{
	  the_RedFragMap[f] = NRedFrags;
	  RedFrags[NRedFrags] = new char[strlen(bufS)];
	  strcpy(RedFrags[NRedFrags],bufS);
	  NRedFrags++;
	}
      else
	{
	  the_RedFragMap[f] = k;
	}
      bufS[0] = '\0';

    }
  for(i=0;i<NRedFrags;i++)
   {delete[] RedFrags[i];}
  return;
}

IsNewFrag(int len,char **map,char *target,int &ind)
{
  for(int i=0;i<len;i++)
    {
      if(!strcmp(target,map[i]))
	{
	  ind = i;
	  return 0;
	}
    }
  return 1;
}



int AtypeNum(char *tag)
{
  for(int i=0;i<NNAMES;i++)
    {
      if(!strcmp(tag,ANAMES[i]))
	{
	  return ANUMS[i];
	}
    }
  return 0;
}
int AtypeName(int typ,char *name)
{
  for(int i = 0;i<NNAMES;i++)
    {
	if(typ == ANUMS[i])
	  {
	    strcpy(name,ANAMES[i]);
	    return 1;
	  }
    }
  return 0;
}


int UpdateExternalSites(int target,int nInsites,int *Insites,int &nExsites,int *Exsites)
{
  int i,k=0;
  if(IsNew(nExsites,Exsites,target,k))
    {
      if(IsNew(nInsites,Insites,target,k))
	{
	  Exsites[nExsites] = target;
	  nExsites++;
	}
    }
  return 1;
}


int IntersectCount(int alen,int *listA,int blen, int *listB)
{
  int i,j,c=0;

  for(i=0;i<alen;i++)
    {
      for(j=0;j<blen;j++)
	{
	  if(listA[i] == listB[j])c++;
	}
    }
  return c;
}
int IsNew(int len, int *map, int target, int &index)
{
  for(int i=0;i<len;i++)
    {
      if(target == map[i])
	{
	  index = i;
	  return 0;
	}
    }
  return 1;
}
int IsNewString(int len, char **map, char* target, int &index)
{
  for(int i=0;i<len;i++)
    {
      if(!strcmp(target,map[i]))
	{
	  index = i;
	  return 0;
	}
    }
  return 1;
}


int IsNewName(int len, NAME *map, char* target, int &index)
{
  for(int i=0;i<len;i++)
    {
      if(!strcmp(target,map[i]))
	{
	  index = i;
	  return 0;
	}
    }
  return 1;
}

int IsNewDouble(int len, double *map, double target, int &index)
{
  for(int i=0;i<len;i++)
    {
      if(target == map[i])
	{
	  index = i;
	  return 0;
	}
    }
  return 1;
}



void PrepWater(int nWaters)
{
  int i,j,k,n,na,nb,nang,f;
  FragSpec *fs;
  Na = Nb = Nang = Nt = 0; 
  NFrags = nWaters;
  for(i=0;i<NFrags;i++)
    {
      n = 3;
      the_frags[i].NumberInternalSites = n;
      the_frags[i].InternalSites = new int[n];
      the_frags[i].InternalSites[0] = Na;
      ba1[Nb] = Na;       ba2[Nb] = Na+1;
      Nb++;
      ba1[Nb] = Na;       ba2[Nb] = Na+2;
      Nb++;
      aa1[Nang] = Na+1; aa2[Nang] = Na; aa3[Nang] = Na + 2;
      Nang++;
      ANums[Na] = 75;      Mass[Na] = 15.99940;      Charge[Na] = -.834;
      AtypeName(ANums[Na],ANames[Na]);
      Na++;
      the_frags[i].InternalSites[1] = Na;
      ANums[Na] = 4;      Mass[Na] = 1.00800;      Charge[Na] = .417;
      AtypeName(ANums[Na],ANames[Na]);
      Na++;
      the_frags[i].InternalSites[2] = Na;
      ANums[Na] = 4;      Mass[Na] = 1.00800;      Charge[Na] = .417;
      AtypeName(ANums[Na],ANames[Na]);
      Na++;
    }
}

void
ReduceTerms()
{

  if(SORTTERMS)
    {
      qsort(the_Terms,nTerms,sizeof(TermSpec),TermSpec::CompareAtnums);
      
      qsort(the_Doubles,nDoubles,sizeof(double),TermSpec::CompareDoubles);
    }

  int i;
  int j;
  int k;
  for(i =0;i<nDoubles - 1;i++)
    {
      if(the_Doubles[i] == the_Doubles[i+1])
	{
	  for(j = i; j < nDoubles-1; j++)
	    {
	      the_Doubles[j] = the_Doubles[j+1];
	    }
	  i--;
	  nDoubles--;
	}
    }
  for(i =0;i<nTerms;i++)
    {
      assert(!IsNewDouble(nDoubles,the_Doubles,the_Terms[i].K0,k));
      the_Terms[i].K0 = (int) k;
      assert(!IsNewDouble(nDoubles,the_Doubles,the_Terms[i].Eq,k));
      the_Terms[i].Eq = (int) k;
      if(the_Terms[i].mult > 0)
	{
            assert(!IsNewDouble(nDoubles,the_Doubles,the_Terms[i].mult,k));
	    the_Terms[i].mult = (int) k;
	}

    }
}


void WriteProbDef(FILE *fPF)
{
  double eps,rmin;
  char bufS[1000000],tbufS[1000];
  char name[256];
  int i,j,k,ff,f,a1,a2,a3,a4;
  FragSpec *fs;

 //    fprintf(fPF,"\n//#include \"ProblemInterface.hpp\" \n\n\n");   // Convert picosec to internal time units


//    fprintf(fPF,"\n\nconst double mTimeConversion   = 0.0488882;\n");   // Convert picosec to internal time units
//    fprintf(fPF,"const double mChargeConversion = 18.2224;\n");   // Convert electronic charge to internal charge units
//    fprintf(fPF,"const double mBoltzmanConstant = 0.00198718;\n");// Boltzmans constant in kcal/mol/degK

//    fprintf(fPF,"const double mTimeStepInPicoSeconds = 0.0005;\n"); // time step
//    fprintf(fPF,"const double mConvertedTimeStep     = mTimeStepInPicoSeconds/ mTimeConversion;\n\n\n"); // diff



  for(f=0,ff=0;f<NRedFrags;f++)
    {
      nTerms = 0;
      nDoubles = 1;
      the_Doubles[0] = 0.0;

      while(the_RedFragMap[ff] != f)ff++;
      sprintf(name,"FragSpec_%d",f);
      fs = &the_fragspecs[ff];
      bufS[0] = '\0';
      CATSTR("class ");
      CATSTR(name);

      OPEN_CLASS(bufS);
      STRNG("public:                "); LINE;
      STRNG("static const int           NumberInternalSites; "); LINE;
      STRNG("static const int           NumberExternalSites; "); LINE;
      STRNG("static const int           InternalSiteSpecTableIndex[]; "); LINE;
      STRNG("static const int           ExternalSiteSpecTableIndex[]; "); LINE;
      
      STRNG("static const int           NumberBondTypes; "); LINE;
      STRNG("static const BondType      BondTypeTable[]; "); LINE;
      STRNG("static const int           NumberInternalBonds;  "); LINE;
      STRNG("static const int           NumberExternalBonds;  "); LINE;
      STRNG("static const Bond          InternalBonds[];  "); LINE;
      STRNG("static const Bond          ExternalBonds[];  "); LINE;
      
      STRNG("static const int           NumberAngleTypes;  "); LINE;
      STRNG("static const AngleType     AngleTypeTable[];  "); LINE;
      STRNG("static const int           NumberInternalAngles;   "); LINE;
      STRNG("static const int           NumberExternalAngles;   "); LINE;
      STRNG("static const Angle         InternalAngles[]; "); LINE;
      STRNG("static const Angle         ExternalAngles[]; "); LINE;
      
      STRNG("static const int           NumberTorsionTypes;   "); LINE;
      STRNG("static const TorsionType   TorsionTypeTable[];  "); LINE;
      STRNG("static const int           NumberInternalTorsions;   "); LINE;
      STRNG("static const int           NumberExternalTorsions;   "); LINE;
      STRNG("static const Torsion       InternalTorsions[];"); LINE;
      STRNG("static const Torsion       ExternalTorsions[];"); LINE;
      
      STRNG("static const int           NumberImproperTypes;   "); LINE;
      STRNG("static const ImproperType   ImproperTypeTable[];  "); LINE;
      STRNG("static const int           NumberInternalImpropers;   "); LINE;
      STRNG("static const int           NumberExternalImpropers;   "); LINE;
      STRNG("static const Improper       InternalImpropers[];"); LINE;
      STRNG("static const Improper       ExternalImpropers[];"); LINE;
      STRNG("static const int NumberExcludedSitePairs;"); LINE;
      STRNG("static const SiteId  ExcludedSitePairs[];"); LINE;
      STRNG("static const int  NumberTerms;"); LINE;
      STRNG("static const Term  TermList[];"); LINE;
      STRNG("static const int  NumberCodes;"); LINE;
      STRNG("static const Code  CodeList[];"); LINE;
      STRNG("static const int  NumberDoubles;"); LINE;
      STRNG("static const double  DoubleList[];"); LINE;

      END_BLOCK;
      
      INIT_COUNT(name,"NumberInternalSites",fs->NumberInternalSites);
      INIT_COUNT(name,"NumberExternalSites",fs->NumberExternalSites);
      INIT_COUNT(name,"NumberBondTypes",fs->NumberBondTypes);
      INIT_COUNT(name,"NumberInternalBonds",fs->NumberInternalBonds);
      INIT_COUNT(name,"NumberExternalBonds",fs->NumberExternalBonds);
      INIT_COUNT(name,"NumberAngleTypes",fs->NumberAngleTypes);
      INIT_COUNT(name,"NumberInternalAngles",fs->NumberInternalAngles);
      INIT_COUNT(name,"NumberExternalAngles",fs->NumberExternalAngles);
      INIT_COUNT(name,"NumberTorsionTypes",fs->NumberTorsionTypes);
      INIT_COUNT(name,"NumberInternalTorsions",fs->NumberInternalTorsions);
      INIT_COUNT(name,"NumberExternalTorsions",fs->NumberExternalTorsions);
      INIT_COUNT(name,"NumberImproperTypes",fs->NumberImproperTypes);
      INIT_COUNT(name,"NumberInternalImpropers",fs->NumberInternalImpropers);
      INIT_COUNT(name,"NumberExternalImpropers",fs->NumberExternalImpropers);
      INIT_COUNT(name,"NumberExcludedSitePairs",fs->NumberExcludedSitePairs);

      sprintf(bufS,"const int %s::InternalSiteSpecTableIndex[]",name);
      OPEN_BLOCK(bufS);
      for(i=0;i<fs->NumberInternalSites;i++)
	{
	  SITETYPESPEC(ATypeMap[fs->InternalSites[i]]);
	  if(i < fs->NumberInternalSites-1){COMA; LINE; TAB;}	  
	}
      END_BLOCK;
      sprintf(bufS,"const int %s::ExternalSiteSpecTableIndex[]",name);
      if(fs->NumberExternalSites>0)
	{
	  OPEN_BLOCK(bufS);
	  {
	    for(i=0;i<fs->NumberExternalSites;i++)
	      {
		SITETYPESPEC(ATypeMap[fs->ExternalSites[i]]);
		if(i < fs->NumberExternalSites-1){COMA; LINE; TAB;}	  
	      }
	    END_BLOCK;
	  }
	}
      else
	{
	  strcat(bufS," = {-1};\n");
	  STRNG(bufS);
	}


      sprintf(bufS,"const Bond %s::InternalBonds[]",name);
      if(fs->NumberInternalBonds > 0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberInternalBonds;i++)
	    {
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ba1[fs->InternalBonds[i]],a1));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ba2[fs->InternalBonds[i]],a2));
	      BONDSPEC(a1,a2,fs->BondParamMap[i]); 
	      if(i < fs->NumberInternalBonds-1){COMA; LINE; TAB;}	  

	      the_Terms[nTerms].mTermType = BONDCODE;
	      the_Terms[nTerms].mTermParams = fs->BondParamMap[i];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = 0; //a3;
	      the_Terms[nTerms].mA4 = 0; //a4;

	      the_Doubles[nDoubles] = BK0[fs->BondTypes[fs->BondParamMap[i]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = BEQ[fs->BondTypes[fs->BondParamMap[i]]];
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Terms[nTerms].mult = 0.0;
	      
	      nTerms++;

	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},-1 };\n");
	  STRNG(bufS);
	}

      sprintf(bufS,"const Bond %s::ExternalBonds[]",name);
      if(fs->NumberExternalBonds > 0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberExternalBonds;i++)
	    {
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ba1[fs->ExternalBonds[i]],a1))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ba1[fs->ExternalBonds[i]],a1));}
	      else 
		a1 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ba2[fs->ExternalBonds[i]],a2))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ba2[fs->ExternalBonds[i]],a2));}
	      else 
		a2 += fs->NumberInternalSites;
	      BONDSPEC(a1,a2,fs->BondParamMap[i+fs->NumberInternalBonds]); 
	      if(i < fs->NumberExternalBonds-1){COMA; LINE; TAB;}	  

	      int tind = i+fs->NumberInternalBonds;
	      the_Terms[nTerms].mTermType = BONDCODE;
	      the_Terms[nTerms].mTermParams = fs->BondParamMap[tind];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = 0; //a3;
	      the_Terms[nTerms].mA4 = 0; //a4;

	      the_Doubles[nDoubles] = BK0[fs->BondTypes[fs->BondParamMap[tind]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = BEQ[fs->BondTypes[fs->BondParamMap[tind]]];
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Terms[nTerms].mult = 0.0;
	      
	      nTerms++;


	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},-1 };\n");
	  STRNG(bufS);
	}


 
      sprintf(bufS,"const Angle %s::InternalAngles[]",name);
      if(fs->NumberInternalAngles)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberInternalAngles;i++)
	    {
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,aa1[fs->InternalAngles[i]],a1));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,aa2[fs->InternalAngles[i]],a2));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,aa3[fs->InternalAngles[i]],a3));
	      ANGLESPEC(a1,a2,a3,fs->AngleParamMap[i]); 
	      if(i < fs->NumberInternalAngles-1){COMA; LINE; TAB;}	  

	      int tind = i;
	      the_Terms[nTerms].mTermType = ANGLECODE;
	      the_Terms[nTerms].mTermParams = fs->AngleParamMap[tind];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = a3;
	      the_Terms[nTerms].mA4 = 0; //a4;

	      the_Doubles[nDoubles] = AK0[fs->AngleTypes[fs->AngleParamMap[tind]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = AEQ[fs->AngleTypes[fs->AngleParamMap[tind]]] * M_PI/180.0;
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Terms[nTerms].mult = 0.0;
	      
	      nTerms++;

	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},{0,0},-1 };\n");
	  STRNG(bufS);
	}



      sprintf(bufS,"const Angle %s::ExternalAngles[]",name);
      
      if(fs->NumberExternalAngles > 0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberExternalAngles;i++)
	    {
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,aa1[fs->ExternalAngles[i]],a1))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,aa1[fs->ExternalAngles[i]],a1));}
	      else 
		a1 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,aa2[fs->ExternalAngles[i]],a2))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,aa2[fs->ExternalAngles[i]],a2));}
	      else 
		a2 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,aa3[fs->ExternalAngles[i]],a3))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,aa3[fs->ExternalAngles[i]],a3));}
	      else 
		a3 += fs->NumberInternalSites;
	      ANGLESPEC(a1,a2,a3,fs->AngleParamMap[i+fs->NumberInternalAngles]); 
	      if(i < fs->NumberExternalAngles-1){COMA; LINE; TAB;}	  

	      int tind = i + fs->NumberInternalAngles;
	      the_Terms[nTerms].mTermType = ANGLECODE;
	      the_Terms[nTerms].mTermParams = fs->AngleParamMap[tind];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = a3;
	      the_Terms[nTerms].mA4 = 0; //a4;

	      the_Doubles[nDoubles] = AK0[fs->AngleTypes[fs->AngleParamMap[tind]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = AEQ[fs->AngleTypes[fs->AngleParamMap[tind]]] * M_PI/180.0;
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Terms[nTerms].mult = 0.0;
	      
	      nTerms++;




	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},{0,0},-1 };\n");
	  STRNG(bufS);
 	}



      sprintf(bufS,"const Torsion %s::InternalTorsions[]",name);
      if(fs->NumberInternalTorsions>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberInternalTorsions;i++)
	    {
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta1[fs->InternalTorsions[i]],a1));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta2[fs->InternalTorsions[i]],a2));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta3[fs->InternalTorsions[i]],a3));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta4[fs->InternalTorsions[i]],a4));
	      TORSIONSPEC(a1,a2,a3,a4,fs->TorsionParamMap[i]);
	      if(i < fs->NumberInternalTorsions-1){COMA; LINE; TAB;}	  

	      int tind = i;
	      the_Terms[nTerms].mTermType = TORSIONCODE;
	      the_Terms[nTerms].mTermParams = fs->TorsionParamMap[tind];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = a3;
	      the_Terms[nTerms].mA4 = a4;

	      the_Doubles[nDoubles] = TK0[fs->TorsionTypes[fs->TorsionParamMap[tind]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = TEQ[fs->TorsionTypes[fs->TorsionParamMap[tind]]] * M_PI/180.0;
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;
	      
	      the_Doubles[nDoubles] = (double)WELLS[fs->TorsionTypes[fs->TorsionParamMap[tind]]];
	      the_Terms[nTerms].mult = the_Doubles[nDoubles];
	      nDoubles++;
	      
	      nTerms++;



	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
	  STRNG(bufS);
	}

      sprintf(bufS,"const Torsion %s::ExternalTorsions[]",name);
      if(fs->NumberExternalTorsions>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberExternalTorsions;i++)
	    {
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta1[fs->ExternalTorsions[i]],a1))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta1[fs->ExternalTorsions[i]],a1));}
	      else 
		a1 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta2[fs->ExternalTorsions[i]],a2))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta2[fs->ExternalTorsions[i]],a2));}
	      else 
		a2 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta3[fs->ExternalTorsions[i]],a3))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta3[fs->ExternalTorsions[i]],a3));}
	      else 
		a3 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ta4[fs->ExternalTorsions[i]],a4))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ta4[fs->ExternalTorsions[i]],a4));}
	      else 
		a4 += fs->NumberInternalSites;
	      TORSIONSPEC(a1,a2,a3,a4,fs->TorsionParamMap[i+fs->NumberInternalTorsions]); 
	      if(i < fs->NumberExternalTorsions-1){COMA; LINE; TAB;}	  

	      int tind = i + fs->NumberInternalTorsions;
	      the_Terms[nTerms].mTermType = TORSIONCODE;
	      the_Terms[nTerms].mTermParams = fs->TorsionParamMap[tind];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = a3;
	      the_Terms[nTerms].mA4 = a4;

	      the_Doubles[nDoubles] = TK0[fs->TorsionTypes[fs->TorsionParamMap[tind]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = TEQ[fs->TorsionTypes[fs->TorsionParamMap[tind]]] * M_PI/180.0;
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;
	      
	      the_Doubles[nDoubles] = (double)WELLS[fs->TorsionTypes[fs->TorsionParamMap[tind]]];
	      the_Terms[nTerms].mult = the_Doubles[nDoubles];
	      nDoubles++;
	      
	      nTerms++;




	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
	  STRNG(bufS);
	}



      sprintf(bufS,"const Improper %s::InternalImpropers[]",name);
      if(fs->NumberInternalImpropers>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberInternalImpropers;i++)
	    {
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia1[fs->InternalImpropers[i]],a1));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia2[fs->InternalImpropers[i]],a2));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia3[fs->InternalImpropers[i]],a3));
	      assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia4[fs->InternalImpropers[i]],a4));
	      TORSIONSPEC(a1,a2,a3,a4,fs->ImproperParamMap[i]); 
	      if(i < fs->NumberInternalImpropers-1){COMA; LINE; TAB;}	  

	      int tind = i;
	      the_Terms[nTerms].mTermType = IMPROPERCODE;
	      the_Terms[nTerms].mTermParams = fs->ImproperParamMap[tind];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = a3;
	      the_Terms[nTerms].mA4 = a4;

	      the_Doubles[nDoubles] = IK0[fs->ImproperTypes[fs->ImproperParamMap[tind]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = IEQ[fs->ImproperTypes[fs->ImproperParamMap[tind]]] * M_PI/180.0;
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;
	      
	      the_Terms[nTerms].mult = 0.0;
	      
	      nTerms++;
	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
	  STRNG(bufS);
	}

      sprintf(bufS,"const Improper %s::ExternalImpropers[]",name);
      if(fs->NumberExternalImpropers>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberExternalImpropers;i++)
	    {
	      // Preserve ordering 
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia1[fs->ExternalImpropers[i]],a1))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia1[fs->ExternalImpropers[i]],a1));}
	      else 
		a1 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia2[fs->ExternalImpropers[i]],a2))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia2[fs->ExternalImpropers[i]],a2));}
	      else 
		a2 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia3[fs->ExternalImpropers[i]],a3))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia3[fs->ExternalImpropers[i]],a3));}
	      else 
		a3 += fs->NumberInternalSites;
	      if(IsNew(fs->NumberExternalSites,fs->ExternalSites,ia4[fs->ExternalImpropers[i]],a4))
		{assert(!IsNew(fs->NumberInternalSites,fs->InternalSites,ia4[fs->ExternalImpropers[i]],a4));}
	      else 
		a4 += fs->NumberInternalSites;
	      TORSIONSPEC(a1,a2,a3,a4,fs->ImproperParamMap[i+fs->NumberInternalImpropers]); 
	      if(i < fs->NumberExternalImpropers-1){COMA; LINE; TAB;}	  

	      int tind = i + fs->NumberInternalImpropers;
	      the_Terms[nTerms].mTermType = IMPROPERCODE;
	      the_Terms[nTerms].mTermParams = fs->ImproperParamMap[tind];
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = a3;
	      the_Terms[nTerms].mA4 = a4;
	      
	      the_Doubles[nDoubles] = IK0[fs->ImproperTypes[fs->ImproperParamMap[tind]]];
	      the_Terms[nTerms].K0 = the_Doubles[nDoubles];
	      nDoubles++;

	      the_Doubles[nDoubles] = IEQ[fs->ImproperTypes[fs->ImproperParamMap[tind]]] * M_PI/180.0;
	      the_Terms[nTerms].Eq = the_Doubles[nDoubles];
	      nDoubles++;
	      
	      the_Terms[nTerms].mult = 0.0;
	      
	      nTerms++;

	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0},{0,0},{0,0},{0,0},-1 };\n");
	  STRNG(bufS);
	}



      
      sprintf(bufS,"const BondType %s::BondTypeTable[]",name);
      if(fs->NumberBondTypes>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberBondTypes;i++)
	    {
	      BONDTYPESPEC(BK0[fs->BondTypes[i]],BEQ[fs->BondTypes[i]]);
	      if(i < fs->NumberBondTypes-1){COMA; LINE; TAB;}	  
	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { 0.0,0.0 };\n");
	  STRNG(bufS);
	}
 
      sprintf(bufS,"const AngleType %s::AngleTypeTable[]",name);
      if(fs->NumberAngleTypes>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberAngleTypes;i++)
	    {
	      ANGLETYPESPEC(AK0[fs->AngleTypes[i]],AEQ[fs->AngleTypes[i]]);
	      if(i < fs->NumberAngleTypes-1){COMA; LINE; TAB;}	  
	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { 0.0,0.0 };\n");
	  STRNG(bufS);
	}  

      sprintf(bufS,"const TorsionType %s::TorsionTypeTable[]",name);
      if(fs->NumberTorsionTypes)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberTorsionTypes;i++)
	    {
	      TORSIONTYPESPEC(TK0[fs->TorsionTypes[i]],WELLS[fs->TorsionTypes[i]],
			      TEQ[fs->TorsionTypes[i]]);
	      if(i < fs->NumberTorsionTypes-1){COMA; LINE; TAB;}	  
	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { 0.0,0,0.0 };\n");
	  STRNG(bufS);
	}
      
      sprintf(bufS,"const ImproperType %s::ImproperTypeTable[]",name);
      if(fs->NumberImproperTypes>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberImproperTypes;i++)
	    {
	      IMPROPERTYPESPEC(IK0[fs->ImproperTypes[i]],IEQ[fs->ImproperTypes[i]]);
	      if(i < fs->NumberImproperTypes-1){COMA; LINE; TAB;}	  
	    }
	  END_BLOCK;	  
	}
      else
	{
	  strcat(bufS," = { 0.0,0.0 };\n");
	  STRNG(bufS);
	}

      
      sprintf(bufS,"const SiteId %s::ExcludedSitePairs[]",name);
      if(fs->NumberExcludedSitePairs>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<fs->NumberExcludedSitePairs;i+=2)
	    {
	      int a1 = fs->ExcludedSitePairs[i];
	      int a2 = fs->ExcludedSitePairs[i+1];
	      int a3;
	      if(a2 < 0)
		{ a3 = -a2;}
	      else
		{ a3 = a2;}

	      EXCLUDESITEPAIR(a1,a2,a3);
	      if(i < fs->NumberExcludedSitePairs-2){COMA; LINE; TAB;}	  

	      int tind = i + fs->NumberExcludedSitePairs;
	      the_Terms[nTerms].mTermType = EXCLUDEDCODE;
	      the_Terms[nTerms].mTermParams = 0;
	      the_Terms[nTerms].mA1 = a1;
	      the_Terms[nTerms].mA2 = a2;
	      the_Terms[nTerms].mA3 = 0; //a3;
	      the_Terms[nTerms].mA4 = 0; //a4;
	      
	      the_Terms[nTerms].K0 = 0.0;
	      the_Terms[nTerms].Eq = 0.0;	      
	      the_Terms[nTerms].mult = 0.0;
	      
	      nTerms++;


	    }
	  END_BLOCK;	  
	}
      else
	{
	  strcat(bufS," = { {0,0} };\n");
	  STRNG(bufS);
	}

      for(i=0;i<nTerms;i++)
	{
	  the_Terms[i].Score(fs->NumberInternalSites);
	}

      ReduceTerms();

      int cnt = 0;
      INIT_COUNT(name,"NumberTerms",nTerms);      
      //      sprintf(bufS,"const int NumberTerms = %d;",nTerms);
      //      STRNG(bufS);
      sprintf(bufS,"const Term %s::TermList[]",name);
      if(nTerms>0)
	{
	  int lss1 = 2,lss2 = 2,lss3 = 2,lss4 = 2;
	  int laa1 = 2,laa2 = 2,laa3 = 2,laa4 = 2; 
	  int bt;
	  OPEN_BLOCK(bufS);
	  bufS[0] = '\0';
	  for(i=0;i<nTerms;i++)
	    {
	      int ss1,ss2,ss3,ss4,aa1,aa2,aa3,aa4;
	      aa1 = the_Terms[i].mA1;                       
	      aa2 = the_Terms[i].mA2;                       
	      aa3 = the_Terms[i].mA3;                       
	      aa4 = the_Terms[i].mA4;                       
	      int nI = fs->NumberInternalSites;
	      if(aa1 < nI){ss1 = 0;}else{ss1 = 1; aa1 -= nI;};
	      if(aa2 < 0){ss2 = 1;aa2 = -aa2-1;}else if (aa2 < nI){ss2 = 0;}else{ss2 = 1;aa2 -= nI;}
	      if(aa3 < nI){ss3 = 0;}else{ss3 = 1; aa3 -= nI;};
	      if(aa4 < nI){ss4 = 0;}else{ss4 = 1; aa4 -= nI;};

	      if(TYPE_REFERENCES)
		{
		  TYPEDTERMSPEC(i,ss1,aa1,ss2,aa2,ss3,aa3,ss4,aa4);
		}
	      else
		{
		  TERMSPEC(i,ss1,aa1,ss2,aa2,ss3,aa3,ss4,aa4);
		}
	      CATSITE(0,0,the_Terms[i].mTermType); CATDELIM; cnt++;
	      if(lss1 == ss1 && laa1 == aa1)bt = 0;else bt = 1;
	      CATSITE(bt,ss1,aa1); CATDELIM; cnt++;
	      if(lss2 == ss2 && laa2 == aa2)bt = 0;else bt = 1;
	      CATSITE(bt,ss2,aa2); CATDELIM; cnt++;
	      switch(the_Terms[i].mTermType)
		{
		case BONDCODE:
		  if(TYPE_REFERENCES) 
		    {
		      int adj = fs->NumberInternalBonds;
		      int bit = (the_Terms[i].mTermParams < adj) ? 0 : 1;
		      adj = (bit) ? adj : 0;
		      //		      CATSITE(1,bit,the_Terms[i].mTermParams - adj); CATDELIM; cnt++;
		      CATSITE(1,0,the_Terms[i].mTermParams); CATDELIM; cnt++;	
		    }
		  else
		    {bt = 1;
		      CATSITE(bt,0,(int)the_Terms[i].K0); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].Eq); CATDELIM; cnt++;
		    }
		  break;
		case EXCLUDEDCODE:
		  break;
		case ANGLECODE:
		  if(TYPE_REFERENCES)
		    {
		      if(lss3 == ss3 && laa3 == aa3)bt = 0;else bt = 1;
		      CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
		      int adj = fs->NumberInternalAngles;
		      int bit = (the_Terms[i].mTermParams < adj) ? 0 : 1;
		      adj = (bit) ? adj : 0;
		      //		      CATSITE(bt,bit,the_Terms[i].mTermParams - adj); CATDELIM; cnt++;
		      CATSITE(1,0,the_Terms[i].mTermParams); CATDELIM; cnt++;	
		    }
		  else
		    {bt = 1;
		      CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].K0); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].Eq); CATDELIM; cnt++;
		    }
		  break;
		case TORSIONCODE:
		  if(TYPE_REFERENCES)
		    {
		      if(lss3 == ss3 && laa3 == aa3)bt = 0;else bt = 1;
		      CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
		      if(lss4 == ss4 && laa4 == aa4)bt = 0;else bt = 1;
		      CATSITE(bt,ss4,aa4); CATDELIM; cnt++;
		      int adj = fs->NumberInternalTorsions;
		      int bit = (the_Terms[i].mTermParams < adj) ? 0 : 1;
		      adj = (bit) ? adj : 0;
		      //		      CATSITE(bt,bit,the_Terms[i].mTermParams - adj); CATDELIM; cnt++;
		      CATSITE(1,0,the_Terms[i].mTermParams); CATDELIM; cnt++;	
		    }
		  else
		    {
		      CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
		      CATSITE(bt,ss4,aa4); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].K0); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].Eq); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].mult); CATDELIM; cnt++;
		    }
		  break;
		case IMPROPERCODE:
		  if(TYPE_REFERENCES)
		    {
		      if(lss3 == ss3 && laa3 == aa3)bt = 0;else bt = 1;
		      CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
		      if(lss4 == ss4 && laa4 == aa4)bt = 0;else bt = 1;
		      CATSITE(bt,ss4,aa4); CATDELIM; cnt++;
		      int adj = fs->NumberInternalImpropers;
		      int bit = (the_Terms[i].mTermParams < adj) ? 0 : 1;
		      adj = (bit) ? adj : 0;
		      CATSITE(1,0,the_Terms[i].mTermParams); CATDELIM; cnt++;
		    }
		  else
		    {
		      CATSITE(bt,ss3,aa3); CATDELIM; cnt++;
		      CATSITE(bt,ss4,aa4); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].K0); CATDELIM; cnt++;
		      CATSITE(bt,0,(int)the_Terms[i].Eq); CATDELIM; cnt++;
		    }
		  break;
		}
//  	      lss1 = ss1; laa1 = aa1; 
//  	      lss2 = ss2; laa2 = aa2; 
//  	      lss3 = ss3; laa3 = aa3; 
//  	      lss4 = ss4; laa4 = aa4; 
	      if(i < nTerms-1){COMA; LINE; TAB;} 
	    }
	  CATSITE(0,0,0);
	  END_BLOCK;

	  INIT_COUNT(name,"NumberCodes",cnt);      
	  sprintf(tbufS,"const Code %s::CodeList[]",name);
	  OPEN_BLOCK(tbufS);
	  STRNG(bufS);
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0,0,0,0,0,0,0,0} };\n");
	  STRNG(bufS);
	  sprintf(bufS,"const Code %s::CodeList[] = { {0,0} };",name);
	  STRNG(bufS);
	}

      INIT_COUNT(name,"NumberDoubles",nDoubles);      

      sprintf(bufS,"const double %s::DoubleList[]",name);
      if(nTerms>0)
	{
	  OPEN_BLOCK(bufS);
	  for(i=0;i<nDoubles;i++)
	    {	      
	      DOUBLESPEC(i);
	      if(i < nDoubles-1){COMA; LINE; TAB;}	  
	    }
	  END_BLOCK;
	}
      else
	{
	  strcat(bufS," = { {0.0} };\n");
	  STRNG(bufS);
	}
    }



  STRNG("class FragmentInstanceListItem"); LINE;
  STRNG("{");            LINE; 
  STRNG("public:");     LINE;  TAB;
  STRNG("   int FragmentTypeId;");  LINE; TAB; 
  STRNG("   int InternalAbsoluteSiteStartIndex;"); LINE; TAB;
  STRNG("   int ExternalSiteTableIndex;"); LINE;
  STRNG("};");

  int c = 0,a = 0,maxfrag=0;
  OPEN_BLOCK("const FragmentInstanceListItem FragmentInstanceList[]");
  for(i=0;i<NFrags;i++)
    {
      if(the_fragspecs[i].NumberExternalSites > 0)
	{BLOCK3(the_RedFragMap[i],a,c); }
      else
	{BLOCK3(the_RedFragMap[i],a,-1); }
      a += the_fragspecs[i].NumberInternalSites;
      c += the_fragspecs[i].NumberExternalSites;
      if(the_fragspecs[i].NumberInternalSites > maxfrag)
	maxfrag = the_fragspecs[i].NumberInternalSites;
      if(i < NFrags-1)
	{COMA; LINE; TAB;}      
    }
  END_BLOCK;

  
  int flg = 0;
  strcpy(bufS,"const int ExternalSiteTable[]");
  if(c  > 0)
    {
      OPEN_BLOCK(bufS);
      for(i=0;i<NFrags;i++)
	{
	  for(j=0;j<the_fragspecs[i].NumberExternalSites;j++)
	    {
	      if(flg)
		{COMA; LINE; TAB; }
	      else
		{flg = 1;}
	      BLOCK1(the_fragspecs[i].ExternalSites[j]);
	    }
	}
      END_BLOCK;
    }
  else
    {
      strcat(bufS," = {0xdeadbeaf};");
      STRNG(bufS);
    }
  OPEN_BLOCK("const SiteSpec SiteSpecTable[]");
  for(i=0;i<NATypes;i++)
    {
      eps = LJEPS[LJTypeMap[i]];
      rmin = LJRMIN[LJTypeMap[i]];
      SITESPEC(Mass[i],Charge[i],eps,rmin); 
      if(i < NATypes-1){COMA; LINE; TAB;}	  
    }
  END_BLOCK;
 

  ff = 0;
  
  // Free Fragspec def
      OPEN_CLASS("class FragSpec ");
      STRNG("public:                "); LINE;
      STRNG(" const int           NumberInternalSites; "); LINE;
      STRNG(" const int           NumberExternalSites; "); LINE;
      STRNG(" const int           *InternalSiteSpecTableIndex; "); LINE;
      STRNG(" const int           *ExternalSiteSpecTableIndex; "); LINE;
      
      STRNG(" const int           NumberBondTypes; "); LINE;
      STRNG(" const BondType      *BondTypeTable; "); LINE;
      STRNG(" const int           NumberInternalBonds;  "); LINE;
      STRNG(" const int           NumberExternalBonds;  "); LINE;
      STRNG(" const Bond          *InternalBonds;  "); LINE;
      STRNG(" const Bond          *ExternalBonds;  "); LINE;
      
      STRNG(" const int           NumberAngleTypes;  "); LINE;
      STRNG(" const AngleType     *AngleTypeTable;  "); LINE;
      STRNG(" const int           NumberInternalAngles;   "); LINE;
      STRNG(" const int           NumberExternalAngles;   "); LINE;
      STRNG(" const Angle         *InternalAngles; "); LINE;
      STRNG(" const Angle         *ExternalAngles; "); LINE;
      
      STRNG(" const int           NumberTorsionTypes;   "); LINE;
      STRNG(" const TorsionType   *TorsionTypeTable;  "); LINE;
      STRNG(" const int           NumberInternalTorsions;   "); LINE;
      STRNG(" const int           NumberExternalTorsions;   "); LINE;
      STRNG(" const Torsion       *InternalTorsions;"); LINE;
      STRNG(" const Torsion       *ExternalTorsions;"); LINE;
      
      STRNG(" const int           NumberImproperTypes;   "); LINE;
      STRNG(" const ImproperType  *ImproperTypeTable;  "); LINE;
      STRNG(" const int           NumberInternalImpropers;   "); LINE;
      STRNG(" const int           NumberExternalImpropers;   "); LINE;
      STRNG(" const Improper      *InternalImpropers;"); LINE;
      STRNG(" const Improper      *ExternalImpropers;"); LINE;
      STRNG(" const int           NumberExcludedSitePairs;"); LINE;
      STRNG(" const SiteId        *ExcludedSitePairs;"); LINE;
      STRNG(" const int           NumberTerms;"); LINE;
      STRNG(" const Term          *TermList;"); LINE;
      STRNG(" const int           NumberCodes;"); LINE;
      STRNG(" const Code          *CodeList;"); LINE;
      STRNG(" const int           NumberDoubles;"); LINE;
      STRNG(" const double        *DoubleList;"); LINE;

      END_BLOCK;
 


  // Fragspec Table prep

  OPEN_BLOCK("const FragSpec FragSpecTable[]");
  for(f=0,ff=0;f<NRedFrags;f++)
    {
      nTerms = 0;
      nDoubles = 1;
      the_Doubles[0] = 0.0;

      while(the_RedFragMap[ff] != f)ff++;
      sprintf(name,"FragSpec_%d",f);
      fs = &the_fragspecs[ff];
        
      STRNG("{\n\t\t");

      sprintf(bufS,"%s::%s",name,"NumberInternalSites");
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalSites"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalSiteSpecTableIndex"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalSiteSpecTableIndex"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;
      
      sprintf(bufS,"%s::%s",name,"NumberBondTypes"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;


      sprintf(bufS,"%s::%s",name,"BondTypeTable"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberInternalBonds"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalBonds"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;


      sprintf(bufS,"%s::%s",name,"InternalBonds"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalBonds"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;
      
      sprintf(bufS,"%s::%s",name,"NumberAngleTypes"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"AngleTypeTable"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberInternalAngles"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalAngles"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalAngles"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalAngles"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;
      
      sprintf(bufS,"%s::%s",name,"NumberTorsionTypes"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"TorsionTypeTable"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberInternalTorsions"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalTorsions"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalTorsions"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalTorsions"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;
      
      sprintf(bufS,"%s::%s",name,"NumberImproperTypes"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ImproperTypeTable"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberInternalImpropers"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExternalImpropers"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"InternalImpropers"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExternalImpropers"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberExcludedSitePairs"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"ExcludedSitePairs"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberTerms"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"TermList"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberCodes"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"CodeList"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"NumberDoubles"); 
      STRNG(bufS);      COMA; LINE;  TAB; TAB;

      sprintf(bufS,"%s::%s",name,"DoubleList"); 
      STRNG(bufS);     
      

      STRNG("\n\t}");
      
      if(i < NFrags-1)
	{COMA; LINE; TAB;}      
    }
  END_BLOCK;

 






  fprintf(fPF,"\nenum {NumberOfFragmentTypes = %d};\n",NRedFrags);
  fprintf(fPF,"\nenum {NumberOfFragments = %d};\n",NFrags);
  fprintf(fPF,"\nenum {NumberOfSiteTypes = %d};\n",NATypes);
  fprintf(fPF,"\nenum {NumberOfSites = %d};\n",Na);
  fprintf(fPF,"\nenum {MaxNumberOfInternalSites = %d};\n",maxfrag);


} 

void WriteMaterializationFile(FILE *fPF)
{
  int i;
  char name[256];
  STRNG("\ntypedef FragmentControlBlock* (*MaterializeFxPtr)(int);\n\n");
  
  OPEN_BLOCK("const MaterializeFxPtr MaterializationFxPtrTable[]");
  for(i=0;i<NRedFrags;i++)
    {
      sprintf(name,"Fragment<FragSpec_%d>::Materialize",i);
      BLOCKSTR(name);
      if(i < NRedFrags-1){COMA; LINE; TAB;}	  
    }
  END_BLOCK;

}


void PushSiteSpec(char *name,char *type, double mass, double charge,
		  double eps,double sigma,double ubeps,double ubr0)
{
  
  strcpy(ANAMES[NNAMES],name);
  ANUMS[NNAMES] = NNAMES;
  ANums[NNAMES] = NNAMES;
  Mass[NNAMES] = mass;
  Charge[NNAMES] = charge;
  LJTYPE[NNAMES] = NNAMES;
  LJEPS[NNAMES] = eps;
  LJRMIN[NNAMES] = sigma;
  ATypes[NNAMES] = NNAMES;  // gives type given typenumber
  ATypeMap[NNAMES] = NNAMES; // gives typenumber given atnum

  // UBEPS[NNAMES] = ubeps;
  // UBR0[NNAMES] = ubr0;
  NATypes++;
  NNAMES++;

}

void PushBondSpec(char *s1, char *s2, double k0,double r0)
{
  int k;

  assert(!IsNewName(Na,ANames,s1,k));
  ba1[NB] = k;
  BA1[NB] = k;

  assert(!IsNewName(Na,ANames,s2,k));
  ba2[NB] = k;
  BA2[NB] = k;

  BK0[NB] = k0;
  BEQ[NB] = r0;

  BTypes[NB] = NB;
  BTypeMap[NB] = NB;

  Nb++;
  NB++;
  NBTypes++;
}



void PushAngleSpec(char *s1, char *s2, char *s3, double k0,double r0)
{
  int k;


  assert(!IsNewName(Na,ANames,s1,k));
  aa1[Nang] = k;
  AA1[Nang] = k;

  assert(!IsNewName(Na,ANames,s2,k));
  aa2[Nang] = k;
  AA2[Nang] = k;

  assert(!IsNewName(Na,ANames,s3,k));
  aa3[Nang] = k;
  AA3[Nang] = k;

  AK0[Nang] = k0;
  AEQ[Nang] = r0;

  AngTypes[Nang] = NB;
  AngTypeMap[Nang] = NB;

  NANG++;
  Nang++;
  NAngTypes++;
}



void PushTorsionSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
{
  int k;

  assert(!IsNewName(Na,ANames,s1,k));
  ta1[NT] = k;
  TA1[NT] = k;

  assert(!IsNewName(Na,ANames,s2,k));
  ta2[NT] = k;
  TA2[NT] = k;

  assert(!IsNewName(Na,ANames,s3,k));
  ta3[NT] = k;
  TA3[NT] = k;

  assert(!IsNewName(Na,ANames,s4,k));
  ta4[NT] = k;
  TA4[NT] = k;

  TK0[NT] = k0;
  TEQ[NT] = r0;
  WELLS[NT] = wells;

  TTypes[NT] = NB;
  TTypeMap[NT] = NB;

  NT++;
  Nt++;
  NTTypes++;
}

void PushImproperSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
{
  int k;

  assert(!IsNewName(Na,ANames,s1,k));
  ia1[NT] = k;
  IA1[NT] = k;

  assert(!IsNewName(Na,ANames,s2,k));
  ia2[NT] = k;
  IA2[NT] = k;

  assert(!IsNewName(Na,ANames,s3,k));
  ia3[NT] = k;
  IA3[NT] = k;

  assert(!IsNewName(Na,ANames,s4,k));
  ia4[NT] = k;
  IA4[NT] = k;

  IK0[NT] = k0;
  IEQ[NT] = r0;
  WELLS[NT] = wells;

  ITypes[NT] = NB;
  ITypeMap[NT] = NB;

  NI++;
  Ni++;
  NITypes++;
}

