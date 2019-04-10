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
 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "xmlprobspec.hpp"
#include "parsexmlspec.hpp"
#include "pushparams.hpp"
#include "headerhandler.hpp"

int IsXmlPath = 0;



#define PSF_WITH_DEFAULT_FRAG_SCHEME 1
#define PSF_TO_XML 2
#define PSF_WITH_FRAG_SCHEME 3
#define XML_WITH_DEFAULT_FRAG_SCHEME 4
#define XML_READ_TEST 5
#define XML_WITH_FRAG_SCHEME 6
#define UNKNOWN 0



// *************************************************************
// *************************************************************
//   Prototypes
//

int  ProbSpecMain(int argc, char **argv);
int  PrepareMolsFromPSFFile(MolecularSystem *ms,char *psffilename,int opcode, char *fname);
int  ParseCharmmParameters(MolecularSystem *ms,char *topfilname,char *paramfilename);
int  PrepareMolecularSystemFromXML(MolecularSystem *the_MolecularSystem, int opcode,char *fragfilename);
void PrintUsageOptions();
int  PrepareMolecularSystem(MolecularSystem* ms,int argc,char **argv);
int DetermineProcessingOperation(int argc, char **argv);


MolecularSystem the_MolecularSystem;


// *************************************************************
// *************************************************************
//                    MAIN
// *************************************************************
// *************************************************************
int main(int argc, char **argv)
{
//    char *av[5] ;
//    av[0] = "xmlprobspec";
//    av[1] = "../data/top_all22_prot.inp";
//    av[2] = "../data/par_all22_prot.inp";
//    av[3] = "../data/enkp.psf";
//    av[4] = "DEFAULT";
  //  int ac = 5;

  return ProbSpecMain(argc,argv);
}
int ProbSpecMain(int argc, char **argv)
{
    char BigBuffa[1024 * 1024];
    int maxlen = 1024 * 1024;
    //    FILE *sout = fopen("bar.out","w");
    FILE *sout = stdout;
    
    int opcode = PrepareMolecularSystem(&the_MolecularSystem,argc,argv);

    the_MolecularSystem.Finalize();

    the_MolecularSystem.PrintXMLDescription(BigBuffa,maxlen);
    
    fprintf(sout,"%s",BigBuffa);

    if(sout != stdout)fclose(sout);
    return 1;
}
void PrintUsageOptions(int argc,char **argv)
{
  printf("\t\t %s <top file> <param file> <psf file> <XML> \n",*argv);
}

int PrepareMolecularSystem(MolecularSystem* ms,int argc,char **argv)
  {
    int opcode = DetermineProcessingOperation(argc, argv);

    
    switch(opcode)
      {       
      case PSF_TO_XML:
	     {
	     assert(ParseCharmmParameters(ms,argv[1],argv[2]));
	  
	     assert(PrepareMolsFromPSFFile(ms,argv[3],opcode,argv[4]));
	  
	     break;
	     }
      default:
	     PrintUsageOptions(argc,argv);
	     return 0;
      }
    return opcode;
  }
int DetermineProcessingOperation(int argc, char **argv)
{

  if(argc == 1)
    return UNKNOWN;
  // If the first arg is not DEFAULT or a frag file, assume that CHARMM files are supplied
  if(  strcmp(argv[1],"DEFAULT") && (!strstr(argv[1],".frg") ) )
    { 
      if(argc == 5)
	{
	  if(strstr(argv[4],"XML"))
	    {
	      return PSF_TO_XML;
            }
	  else
	    {
	      return UNKNOWN;
	    }
	}
      else
	{
	  printf("Incomplete arguments to determine operation mode\n");
	  return UNKNOWN;
	}
    }
  else if(!strcmp(argv[1],"DEFAULT"))      
    {
      return XML_WITH_DEFAULT_FRAG_SCHEME;
    }
  else if(!strcmp(argv[1],"TEST"))      
    {
      return XML_READ_TEST;
    }
  else if (strstr(argv[1],".frg"))
    {
      return XML_WITH_FRAG_SCHEME;
    }
  return UNKNOWN;
}

int ParseCharmmParameters(MolecularSystem *ms,char *topfilename,char *paramfilename)
{
  FILE *topfile = fopen(topfilename,"r");
  if(topfile == NULL)
    {
      printf("Couldn't open topology file |%s|\n",topfilename);
      return 0;
    }
  ms->ReadCharmmAtomTypes(topfile);
  fclose(topfile);
  
  FILE *parfile = fopen(paramfilename,"r");
  if(parfile == NULL)
    {
      printf("Couldn't open param file |%s|\n",paramfilename);
      return 0;
    }
  ms->ReadCharmmParameterFile(parfile);
  fclose(parfile);

  return 1;
}


int PrepareMolsFromPSFFile(MolecularSystem *ms,char *psffilename,int opcode,char *fname)
{
  FILE *psf = NULL;
  psf = fopen(psffilename,"r");
  if(psf == NULL)
    {
      printf("Couldn't open psf file |%s|\n",psffilename);
    }
  ms->ReadPsfFile(psf);
  switch(opcode)
    {
    case PSF_WITH_DEFAULT_FRAG_SCHEME:
      rewind(psf);
	ms->PreparePeptideFragmentationScheme(psf);
      break;
    case PSF_TO_XML:
      rewind(psf);
      ms->PrepareSingleFragmentScheme();
      break;
    case PSF_WITH_FRAG_SCHEME:
      FILE *fragfile = fopen(fname,"r");
      ms->ReadFragmentationScheme(fragfile);		    
      fclose(fragfile);
      break;
    }
  fclose(psf);
  return 1;
}

int PrepareMolecularSystemFromXML(MolecularSystem *ms, int opcode,char *fragfilename)
{
  IsXmlPath = 1;
  ParseXMLSpec::parse();
  
  switch(opcode)
    {
    case XML_WITH_DEFAULT_FRAG_SCHEME:
    case XML_READ_TEST:		
      ms->PrepareSingleFragmentScheme();
      break;
    case XML_WITH_FRAG_SCHEME:
      FILE *fragfile = fopen(fragfilename,"r");
      if(fragfile == NULL)
	{
	  printf("Couldn't open fragmentation file |%s|\n",fragfilename);
	  return 0;
	}
      ms->ReadFragmentationScheme(fragfile);
      fclose(fragfile);
      break;
    }
  return 1;
}


void PushSiteSpec(char *name,char *type, double mass, double charge,
		 double eps, double sigma, double eps14, double sigma14)
{the_MolecularSystem.PushSiteSpec(name,type, mass, charge, eps, sigma, eps14, sigma14);}
void PushBondSpec(char *s1, char *s2, double k0,double r0)
{the_MolecularSystem.PushBondSpec(s1, s2, k0,r0);}
void PushAngleSpec(char *s1, char *s2, char *s3, double k,double th0, double ku, double ru)
{the_MolecularSystem.PushAngleSpec(s1, s2, s3, k, th0, ku, ru);}
void PushTorsionSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
{the_MolecularSystem.PushTorsionSpec(s1, s2, s3, s4, k0, wells, r0);}
void PushImproperSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
{the_MolecularSystem.PushImproperSpec(s1, s2, s3, s4, k0, wells, r0);}
