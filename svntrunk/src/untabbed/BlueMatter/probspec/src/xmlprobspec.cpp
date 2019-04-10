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
#include <iostream.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <BlueMatter/xmlprobspec.hpp>
#include <BlueMatter/parsexmlspec.hpp>
#include <BlueMatter/pushparams.hpp>
#include <BlueMatter/headerhandler.hpp>


#include <BlueMatter/headerparameterlist.hpp>
#include <BlueMatter/typelist.hpp>
#include <BlueMatter/forcelist.hpp>
#include <BlueMatter/pairlist.hpp>

#include <BlueMatter/DOMTreeErrorReporter.hpp>
#include <BlueMatter/ffdomutils.hpp>

#include <BlueMatter/UDF_RegistryIF.hpp>

#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <util/XMLUniDefs.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>


#include <dom/DOM_DOMException.hpp>

#include <parsers/DOMParser.hpp>
#include <dom/DOM.hpp>

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
void ReadXMLFile(DOM_Document &);
int GetParameterValue(const TypeList* tl_p,const ParameterSpecTable * aParamSpec,int row, char *columnName, char *value);
int FindForceTermListIndex(const ForceListTable *flt_p,char *tagS);


#define MASS_TAG "mass"
#define CHARGE_TAG "charge"
#define ATOM_TYPE_PREPEND "at_"
#define ATOM_TYPE_TAG "atom_type"
#define LJ_PREPEND "lj_"
#define LJ14_PREPEND "lj14_"
#define SITELIST_PREPEND "site_"
#define LJ_PAIR_PREPEND "lj_"
#define LJ14_PAIR_PREPEND "lj14_"
#define LJ_TYPE_PREPEND "ljt_"
#define LJ14_TYPE_PREPEND "lj14_"

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
    char BigBuffa[50 * 1024 * 1024];
    int maxlen = 1024 * 1024;
    //    FILE *sout = fopen("bar.out","w");
    FILE *sout = stdout;

    int opcode = PrepareMolecularSystem(&the_MolecularSystem,argc,argv);

    the_MolecularSystem.Finalize();

    if(opcode != PSF_TO_XML)
      the_MolecularSystem.ApplyFragmentationScheme();

    if((opcode == PSF_TO_XML) || (opcode == XML_READ_TEST))
      the_MolecularSystem.PrintXMLDescription(BigBuffa,maxlen);
    else
      the_MolecularSystem.PrintHeaderFile(BigBuffa,maxlen);

    fprintf(sout,"%s",BigBuffa);

    if(sout != stdout)fclose(sout);
    return 1;
}
void PrintUsageOptions(int argc,char **argv)
{
  printf("HEADER FILE FROM:\n");
  printf("\tPSF FILE AND FRAGMENTATION SCHEME\n");
  printf("\t\t %s <top file> <param file> <psf file> <frag scheme> \n",*argv);

  printf("\tPSF FILE WITH DEFAULT FRAGMENTATION SCHEME (AMINO ACIDS)\n");
  printf("\t\t %s <top file> <param file> <psf file> <DEFAULT> \n",*argv);

  printf("\tXML FILE AND FRAGMENTATION SCHEME\n");
  printf("\t\t %s <fragfile>  <  <xmlspec> \n",*argv);

  printf("\tXML FILE WITHOUT FRAGMENTATION\n");
  printf("\t\t %s DEFAULT < <xmlspec> \n",*argv);

  printf("XML FILE FROM PSF FILE:\n");
  printf("\t\t %s <top file> <param file> <psf file> <XML> \n",*argv);
}

int PrepareMolecularSystem(MolecularSystem* ms,int argc,char **argv)
  {
    int opcode = DetermineProcessingOperation(argc, argv);


    switch(opcode)
      {
      case XML_WITH_DEFAULT_FRAG_SCHEME:
      case XML_READ_TEST:
      case XML_WITH_FRAG_SCHEME:
             {
             assert(PrepareMolecularSystemFromXML(ms,opcode,argv[1]));
             break;
         }
      case PSF_WITH_DEFAULT_FRAG_SCHEME:
      case PSF_TO_XML:
      case PSF_WITH_FRAG_SCHEME:
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
          if(strstr(argv[4],"DEFAULT"))
            {
              return PSF_WITH_DEFAULT_FRAG_SCHEME;
            }
          else if(strstr(argv[4],"XML"))
            {
              return PSF_TO_XML;
            }
          else
            {
              FILE *frag = fopen(argv[4],"r");
              if(frag)
                {
                  fclose(frag);
                  return PSF_WITH_FRAG_SCHEME;
                }
              else
                {
                  printf("Couldn't open Fragment scheme file %s.\n",argv[4]);
                  printf("Correct filname or use DEFAULT (for single fragment)\n");
                  return UNKNOWN;
                }
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
  //  ParseXMLSpec::parse();

  char* xmlFile = "xmlprobspec_input.xml";

  // Initialize the XML4C2 system
  try
    {
      XMLPlatformUtils::Initialize();
    }

  catch(const XMLException& toCatch)
    {
      cerr << "Error during Xerces-c Initialization.\n"
     << "	 Exception message:"
     << DOMString(toCatch.getMessage()) << endl;
      return 1;
    }


  //  Create our parser, then attach an error handler to the parser.
  //  The parser will call back to methods of the ErrorHandler if it
  //  discovers errors during the course of parsing the XML document.
  //
  DOMParser *parser = new DOMParser;
  parser->setValidationScheme(DOMParser::Val_Auto);
  parser->setDoNamespaces(true);

  ErrorHandler *errReporter = new DOMTreeErrorReporter();
  parser->setErrorHandler(errReporter);

  //
  //  Parse the XML file, catching any XML exceptions that might propogate
  //  out of it.
  //
  bool errorsOccured = false;
  try
    {
      parser->parse(xmlFile);
    }

  catch (const XMLException& e)
    {
      cerr << "An error occured during parsing\n   Message: "
     << DOMString(e.getMessage()) << endl;
      //      errorsOccured = true;
    }


  catch (const DOM_DOMException& e)
    {
      cerr << "An error occured during parsing\n   Message: "
     << DOMString(e.msg) << endl;
      //      errorsOccured = true;
    }

  catch (...)
    {
      cerr << "An error occured during parsing\n " << endl;
      //      errorsOccured = true;
    }

  // If we parsed successfully, try to run the tests
  if (!errorsOccured)
    {

     DOM_Document doc = parser->getDocument();
     ReadXMLFile(doc);
    }
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

void ReadXMLFile(DOM_Document &doc)
{
      // header
      HeaderParameterList* hpl_p = HeaderParameterList::create(doc);
      assert(hpl_p != NULL);

      //      cout << "Header Parameters:\n";
      //      cout << *hpl_p << endl;
      delete hpl_p;

      const ForceListTable* flt_p = ForceListTable::create(doc);
      assert(flt_p != NULL);

      const PairListTable* plt_p = PairListTable::create(doc);


      const ParameterSpecTable * aParamSpec;
      
      const TypeListTable* tlt_p = TypeListTable::create(doc);
      assert(tlt_p != NULL);
      int i = 0;
      int j;

      // Find and collect all the LJ terms in the document
      const TypeList *aLJList = tlt_p->byTypePrepend(LJ_PREPEND);
      assert(aLJList != NULL);
      aParamSpec = &aLJList->parameterSpec();
      int NumberOfLJTypes = aLJList->typeCount();
      char ljtypeS[15];
      for(i = 0; i < NumberOfLJTypes; i++)
  {
    //	  GetParameterValue(aLJList,aParamSpec,i, LJ_TYPE_PREPEND, ljtypeS);
    sprintf(ljtypeS,"%s%d",LJ_TYPE_PREPEND,i+1);
    the_MolecularSystem.AddLennardJonesType(ljtypeS);
  }
      // Find and collect all the LJ14 terms in the document
      const TypeList *aLJ14List = tlt_p->byTypePrepend(LJ14_PREPEND);
      assert(aLJ14List != NULL);
      aParamSpec = &aLJ14List->parameterSpec();
      int NumberOfLJ14Types = aLJ14List->typeCount();
      char lj14typeS[15];
      for(i = 0; i < NumberOfLJ14Types; i++)
  {
    //	  GetParameterValue(aLJ14List,aParamSpec,i, LJ14_TYPE_PREPEND, lj14typeS);
    sprintf(lj14typeS,"%s%d",LJ14_TYPE_PREPEND,i+1);
    the_MolecularSystem.AddLennardJones14Type(lj14typeS);
  }

      // Find and collect all the AtomType terms in the document
      const TypeList *aTypeList = tlt_p->byTypePrepend(ATOM_TYPE_PREPEND);
      assert(aTypeList != NULL);
      aParamSpec = &aTypeList->parameterSpec();
      int NumberOfATypes = aTypeList->typeCount();
      char aTypeS[15];
      for(i = 0; i < NumberOfATypes; i++)
  {
    GetParameterValue(aTypeList,aParamSpec,i, ATOM_TYPE_TAG, aTypeS);
    the_MolecularSystem.AddAtomType(aTypeS,i);
  }

      // Find and collect all the sites in the document
      const TypeList *aSiteList = tlt_p->byTypePrepend(SITELIST_PREPEND);
      int NumberOfSites = aSiteList->typeCount();
      assert(aSiteList != NULL);
      aParamSpec = &aSiteList->parameterSpec();
      int index;
      // Now we have the site list. We need to find out how many parameters
      for(i = 0; i < NumberOfSites; i++)
  {
    char massS[15],chargeS[15],atypeS[15],ljtypeS[15],lj14typeS[15];
    GetParameterValue(aSiteList,aParamSpec,i, MASS_TAG, massS);
    GetParameterValue(aSiteList,aParamSpec,i, CHARGE_TAG, chargeS);
    //sprintf(atypeS,"%d",i+1);
    GetParameterValue(aSiteList,aParamSpec,i, ATOM_TYPE_TAG, atypeS);
    sprintf(ljtypeS,"%d",i+1);
    //	  GetParameterValue(aSiteList,aParamSpec,i, LJ_TYPE_PREPEND, ljtypeS);
    sprintf(lj14typeS,"%d",i+1);
    //	  GetParameterValue(aSiteList,aParamSpec,i, LJ14_TYPE_PREPEND, lj14typeS);

    the_MolecularSystem.AddAtomInstance(massS,chargeS,atypeS,ljtypeS,lj14typeS);
  }


      int BondIndex = FindForceTermListIndex(flt_p,"Bond");	  
      int AngleIndex = FindForceTermListIndex(flt_p,"Angle");	  
      int UreyIndex = FindForceTermListIndex(flt_p,"Urey");	  
      int TorsionIndex = FindForceTermListIndex(flt_p,"Torsion");	  
      int ImproperIndex = FindForceTermListIndex(flt_p,"Improper");	  
      int paramCount;
#define MAXPARAMS 5
      int paramMap[MAXPARAMS];
#define MAXTYPELEN 50
      char *type; //[MAXTYPELEN];
#define MAXSTRUCTLEN 1024
      char structdef[MAXSTRUCTLEN];
      char *struc = NULL;
      int code,ord;
      int siteCount;
      if(BondIndex >= 0)
  {
    const ForceList *aBondForceList = flt_p->forceList(BondIndex);
    const ParameterSpecTable *aBondParamSpec = &aBondForceList->parameterSpec();
    paramCount = aBondForceList->parameterSpec().parameterCount();
    assert(paramCount == 2);  // fancy bonds come later
    
    
    // This call gets the name of the Bond Term function
    UDF_RegistryIF::GetUDFCodeByName(aBondForceList->forceTermID(), code);

    the_MolecularSystem.SetBondTermName(aBondForceList->forceTermID());
    the_MolecularSystem.SetBondCode(code);
    UDF_RegistryIF::GetStructureNameByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetBondTermName(struc);
    UDF_RegistryIF::GetStructureStringByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetBondStructName(struc);

    siteCount = aBondForceList->siteCount();	  
    char paramValS[2][30];

    for(i=0;i<paramCount;i++)
      {
        //Get name for paramSpec i
        const ParameterSpec &aParamSpec = aBondParamSpec->parameterSpec(i);
        // Get ordinal from registry
        UDF_RegistryIF::GetArgByName(code, aParamSpec.d_name, ord, type);
        paramMap[ord] = i;
      }

    for(i=0;i<aBondForceList->forceCount();i++)
      {
        int a1 = aBondForceList->forceRecord(i).d_siteID[0];
        int a2 = aBondForceList->forceRecord(i).d_siteID[1];
        
        the_MolecularSystem.AddBondTerm(a1,a2,
                aBondForceList->forceRecord(i).d_parameter[paramMap[0]].value(),
                aBondForceList->forceRecord(i).d_parameter[paramMap[1]].value());
      }
  }

      if(AngleIndex >= 0)
  {
    const ForceList *aAngleForceList = flt_p->forceList(AngleIndex);
    const ParameterSpecTable *aAngleParamSpec = &aAngleForceList->parameterSpec();
    paramCount = aAngleForceList->parameterSpec().parameterCount();
    assert(paramCount == 2);  // fancy bonds come later
    
    UDF_RegistryIF::GetUDFCodeByName(aAngleForceList->forceTermID(), code);
    the_MolecularSystem.SetAngleCode(code);
    UDF_RegistryIF::GetStructureNameByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetAngleTermName(struc);
    UDF_RegistryIF::GetStructureStringByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetAngleStructName(struc);

    siteCount = aAngleForceList->siteCount();	  
    char paramValS[2][30];

    for(i=0;i<paramCount;i++)
      {
        //Get name for paramSpec i
        const ParameterSpec &aParamSpec = aAngleParamSpec->parameterSpec(i);
        // Get ordinal from registry
        UDF_RegistryIF::GetArgByName(code, aParamSpec.d_name, ord, type);
        paramMap[ord] = i;
      }

    for(i=0;i<aAngleForceList->forceCount();i++)
      {
        int a1 = aAngleForceList->forceRecord(i).d_siteID[0];
        int a2 = aAngleForceList->forceRecord(i).d_siteID[1];
        int a3 = aAngleForceList->forceRecord(i).d_siteID[2];
        the_MolecularSystem.AddAngleTerm(a1,a2,a3,
                aAngleForceList->forceRecord(i).d_parameter[paramMap[0]].value(),
                aAngleForceList->forceRecord(i).d_parameter[paramMap[1]].value());
      }
  }


      if(UreyIndex >= 0)
  {
    const ForceList *aUreyForceList = flt_p->forceList(UreyIndex);
    const ParameterSpecTable *aUreyParamSpec = &aUreyForceList->parameterSpec();
    paramCount = aUreyForceList->parameterSpec().parameterCount();
    assert(paramCount == 2);  // fancy bonds come later
    siteCount = aUreyForceList->siteCount();	  

    
    UDF_RegistryIF::GetUDFCodeByName(aUreyForceList->forceTermID(), code);
    the_MolecularSystem.SetUreyCode(code);
    UDF_RegistryIF::GetStructureNameByUDFCode(code, struc);
    strcpy(structdef,struc);
    the_MolecularSystem.SetUreyTermName(structdef);
    UDF_RegistryIF::GetStructureStringByUDFCode(code, struc);
    strcpy(structdef,struc);
    the_MolecularSystem.SetUreyStructName(structdef);

    for(i=0;i<paramCount;i++)
      {
        //Get name for paramSpec i
        const ParameterSpec &aParamSpec = aUreyParamSpec->parameterSpec(i);
        // Get ordinal from registry
        UDF_RegistryIF::GetArgByName(code, aParamSpec.d_name, ord, type);
        paramMap[ord] = i;
      }
    for(i=0;i<aUreyForceList->forceCount();i++)
      {
        int a1 = aUreyForceList->forceRecord(i).d_siteID[0];
        int a2 = aUreyForceList->forceRecord(i).d_siteID[1];
        int a3 = aUreyForceList->forceRecord(i).d_siteID[2];
        
        the_MolecularSystem.AddUreyTerm(a1,a2,a3,
                aUreyForceList->forceRecord(i).d_parameter[paramMap[0]].value(),
                aUreyForceList->forceRecord(i).d_parameter[paramMap[1]].value());
      }
  }

      if(TorsionIndex >= 0)
  {
    const ForceList *aTorsionForceList = flt_p->forceList(TorsionIndex);
    const ParameterSpecTable *aTorsionParamSpec = &aTorsionForceList->parameterSpec();
    paramCount = aTorsionForceList->parameterSpec().parameterCount();
    assert(paramCount == 3);  // fancy torsions come later
    
    UDF_RegistryIF::GetUDFCodeByName(aTorsionForceList->forceTermID(), code);
    the_MolecularSystem.SetTorsionCode(code);
    UDF_RegistryIF::GetStructureNameByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetTorsionTermName(struc);
    UDF_RegistryIF::GetStructureStringByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetTorsionStructName(struc);

    siteCount = aTorsionForceList->siteCount();	  
    char paramValS[3][30];
    for(i=0;i<paramCount;i++)
      {
        //Get name for paramSpec i
        const ParameterSpec &aParamSpec = aTorsionParamSpec->parameterSpec(i);
        // Get ordinal from registry
        UDF_RegistryIF::GetArgByName(code, aParamSpec.d_name, ord, type);
        paramMap[ord] = i;
      }
    for(i=0;i<aTorsionForceList->forceCount();i++)
      {
        int a1 = aTorsionForceList->forceRecord(i).d_siteID[0];
        int a2 = aTorsionForceList->forceRecord(i).d_siteID[1];
        int a3 = aTorsionForceList->forceRecord(i).d_siteID[2];
        int a4 = aTorsionForceList->forceRecord(i).d_siteID[3];
        
        the_MolecularSystem.AddTorsionTerm(a1,a2,a3,a4,
                aTorsionForceList->forceRecord(i).d_parameter[paramMap[0]].value(),
                aTorsionForceList->forceRecord(i).d_parameter[paramMap[1]].value(),
                aTorsionForceList->forceRecord(i).d_parameter[paramMap[2]].value());
      }
  }

      if(ImproperIndex >= 0)
  {
    const ForceList *aImproperForceList = flt_p->forceList(ImproperIndex);
    const ParameterSpecTable *aImproperParamSpec = &aImproperForceList->parameterSpec();
    paramCount = aImproperForceList->parameterSpec().parameterCount();
    assert(paramCount == 2);  // fancy impropers come later

    
    UDF_RegistryIF::GetUDFCodeByName(aImproperForceList->forceTermID(), code);
    the_MolecularSystem.SetImproperCode(code);
    UDF_RegistryIF::GetStructureNameByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetImproperTermName(struc);
    UDF_RegistryIF::GetStructureStringByUDFCode(code, struc);
    //	  strcpy(structdef,struc);
    the_MolecularSystem.SetImproperStructName(struc);

    siteCount = aImproperForceList->siteCount();	  
    char paramValS[2][30];
    for(i=0;i<paramCount;i++)
      {
        //Get name for paramSpec i
        const ParameterSpec &aParamSpec = aImproperParamSpec->parameterSpec(i);
        // Get ordinal from registry
        UDF_RegistryIF::GetArgByName(code, aParamSpec.d_name, ord, type);
        paramMap[ord] = i;
      }
    for(i=0;i<aImproperForceList->forceCount();i++)
      {
        int a1 = aImproperForceList->forceRecord(i).d_siteID[0];
        int a2 = aImproperForceList->forceRecord(i).d_siteID[1];
        int a3 = aImproperForceList->forceRecord(i).d_siteID[2];
        int a4 = aImproperForceList->forceRecord(i).d_siteID[3];
        strcpy(paramValS[0],aImproperForceList->forceRecord(i).d_parameter[0].value());
        strcpy(paramValS[1],aImproperForceList->forceRecord(i).d_parameter[1].value());
        
        the_MolecularSystem.AddImproperTerm(a1,a2,a3,a4,
                aImproperForceList->forceRecord(i).d_parameter[paramMap[0]].value(),
                aImproperForceList->forceRecord(i).d_parameter[paramMap[1]].value());

      }
  }

      // pairlists
    UDF_RegistryIF::GetUDFCodeByName("ExPairForce", code);
    the_MolecularSystem.SetExPairCode(code);

    UDF_RegistryIF::GetUDFCodeByName("Pair14Force", code);
    the_MolecularSystem.SetPair14Code(code);

      const PairList *ljPairs = plt_p->byTypeClassPrepend(LJ_PAIR_PREPEND);
      assert(ljPairs != NULL);

      for (i = 0; i < ljPairs->typeCount(); i++)
  {
    for (j = 0; j <= i; j++)
      {
        const PairList::ParamRecord& rec = ljPairs->paramRecord(i,j);
        //	      for (int k = 0; k < pl.paramCount(); k++)
        //	{
      //		  os << "Parameter[" << i << ", " << j << ": ";
      //		  os <<
      //		    pl.d_paramSpecTable.parameterSpec(rec.d_parameter[k].index()).d_name;
      the_MolecularSystem.AddLennardJonesPair(i,j,rec.d_parameter[0].value(),rec.d_parameter[1].value());
      //		}
      }
  }

      const PairList *lj14Pairs = plt_p->byTypeClassPrepend(LJ14_PAIR_PREPEND);
      assert(lj14Pairs != NULL);

      for (i = 0; i < lj14Pairs->typeCount(); i++)
  {
    for (j = 0; j <= i; j++)
      {
        const PairList::ParamRecord& rec = lj14Pairs->paramRecord(i,j);
        //	      for (int k = 0; k < pl.paramCount(); k++)
        //	{
      //		  os << "Parameter[" << i << ", " << j << ": ";
      //		  os <<
      //		    pl.d_paramSpecTable.parameterSpec(rec.d_parameter[k].index()).d_name;
      the_MolecularSystem.AddLennardJones14Pair(i,j,rec.d_parameter[0].value(),rec.d_parameter[1].value());
      //		}
      }
  }

}


int GetParameterValue(const TypeList * tl_p,const ParameterSpecTable * aParamSpec,int row, char *columnName, char *value)
{
  const Parameter *aParam = &tl_p->parameterRecord(row)[aParamSpec->indexOfParameter(columnName)];
  assert(aParam != NULL);
  strcpy(value,aParam->value());
  return 1;
}

int FindForceTermListIndex(const ForceListTable *flt_p,char *tagS)
{
  int i;
  int NumberOfForceTerms = flt_p->forceListCount();
  
  for(i = 0; i < NumberOfForceTerms; i++)
    {
      const ForceList *aForceList = flt_p->forceList(i);
      
      if(strstr(aForceList->forceTermID(),tagS))return i;
    }
  return -1;
}





