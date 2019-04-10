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
 // ************************************************************************
// File: probspecgen.cpp
// Author: AR
// Date: June 22, 2001
// Class: 
// Description: This class uses the forcelist data structure to generate
// the file which contains the specification of the MD problem
// Modification:
// 6/22/01     -  Birth of probspecgen
// 8/8/01      -  Using a different interface to the XML
// 8/13/01     -  Changing the workorders to be |udf code|siteTupleListPtr|ParamTuplePtr|
// ************************************************************************
//#include <BlueMatter/forcelist.hpp>
#include <BlueMatter/udfinvocationtable.hpp>
#include <BlueMatter/udfinvocationrecord.hpp>
#include <BlueMatter/sitetuplelist.hpp>
#include <BlueMatter/paramtuplelist.hpp>
#include <BlueMatter/typelist.hpp>
#include <BlueMatter/pairlist.hpp>
#include <BlueMatter/parameter.hpp>
#include <BlueMatter/parameterspectable.hpp>
#include <BlueMatter/graphseparation.hpp>
#include <BlueMatter/bondlistasgraph.hpp>
#include <BlueMatter/connectedsubgraphs.hpp>
#include <BlueMatter/headerparameterlist.hpp>

#include <vector>
#include <deque>
#include <queue>
#include <utility>
#include <algorithm>

#include <BlueMatter/ffdomutils.hpp>
#include <BlueMatter/DOMTreeErrorReporter.hpp>
#include <BlueMatter/ffsaxassert.hpp>
#include <BlueMatter/ffsaxutils.hpp>
#include <BlueMatter/ffsaxffparamshandlerstate.hpp>
#include <BlueMatter/ffsaxhandler.hpp>

#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <util/XMLUniDefs.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>
#include <dom/DOM_DOMException.hpp>
#include <parsers/DOMParser.hpp>
#include <parsers/SAXParser.hpp>

#include <BlueMatter/LowerTriangleIndex.hpp>
#include <BlueMatter/UDF_RegistryIF.hpp>
#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/probspecgen.hpp>

/********************************************************************************
 *  Global flags 
 *******************************************************************************/
enum {
  ALL_SITES_IN_ONE_FRAGMENT = 0,
  ALL_MOLECULES_IN_ONE_FRAGMENT = 1,
  ALL_SITES_IN_THEIR_OWN_FRAGMENT = 2,
  ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT = 3
};

int debug         = 0;
int verbose       = 0;
int fragment_mode = ALL_SITES_IN_ONE_FRAGMENT; // put molecules in their own fragment by default
/*******************************************************************************/

class ParameterSorter{
  
  const ParamTupleList* mParamList;

public:  
  ParameterSorter(const ParamTupleList* ptl)
    {
      mParamList = ptl;
    }
  
  int operator() (const int &i1, const int& i2) const
    {
      return (mParamList->getParamTuple(i1) < mParamList->getParamTuple(i2));
    }
};

void assertMsg(int cond, char* message1, char* message2, char* message3) 
{
  if (!cond) 
    {
      cerr << message1 << message2 << message3 << endl;
      exit(1);
    }
}

void assertMsg(int cond, char* message1, int message2, char* message3) 
{
  if (!cond) 
    {
      cerr << message1 << message2 << message3 << endl;
      exit(1);
    }
}

void dumpProbSpec(char* filename, 
      const HeaderParameterList* hpl,
      const UDFInvocationTable*   uil,
      const TypeListTable*       tlt, 
      const PairListTable*       plt,
      GraphSeparation            gSep,
      ConnectedSubgraphs         partition)
      
{
  ofstream os(filename, ios::out);

  /*******************************************************************************
   *   Output all symbolic constants from the headerParamer as #defines
   ******************************************************************************/
  if(verbose)
    cout << "Printing out symbolic constants...\n\n" << endl;
  
  HeaderParameter hp;
  for (int b=0; b < hpl->parameterCount(); b++)
    {
      hp = hpl->parameter(b);
      if (hp.isSymbolicConstant())
  os << "#define " << hp.name() << "  " << hp.value() << endl;
    }
  os << "\n\n";
  /*******************************************************************************/ 
  
 

  /*******************************************************************************
   *  Getting out the ljStructure, once the non-bonded stuff are generic
   *  there will be no need for this here.
   ******************************************************************************/
  int   ljCode;
  char* ljStruct;
  char  ljStructName[64];

  UDF_RegistryIF::GetUDFCodeByName("LennardJonesForce", ljCode);
  UDF_RegistryIF::GetStructureStringByUDFCode(ljCode, ljStruct);
  os << ljStruct << endl;

  // Reusing ljStruct, really no need to declare another char*
  UDF_RegistryIF::GetStructureNameByUDFCode(ljCode, ljStruct);
  strcpy(ljStructName, ljStruct);
  /*******************************************************************************/
  


  /*******************************************************************************
   *  Printout all the structures participating pertinent to the specified system
   ******************************************************************************/
  if(verbose)
    cout << "Printing out the udf structures...\n\n" << endl;

  int                         udfCode;  
  char*                       structString  =  NULL;  
  char*                       udfName       =  NULL;
  // This invocationRecord is used through out the file
  const UDFInvocationRecord*  invRecord     =  NULL; 
  
  for (int i = 0; i < uil->size(); i++)
    {
      invRecord = &(uil->getRecord(i));
      assert ( invRecord != NULL );
      
      // If there are no parameters then no need to output the structure.

      // *********************LOOK HERE***************** 

      // Ask Bob about the convention for null paramTupleList
      if( invRecord->getParamTupleList() == NULL )
  continue;
      
      udfName = (char *) invRecord->getUDFName();
      UDF_RegistryIF::GetUDFCodeByName( udfName, udfCode);
      UDF_RegistryIF::GetStructureStringByUDFCode(udfCode, structString);
      os << structString << endl;
    }
  /******************************************************************************/
  


  /*******************************************************************************
   *  Printout the fields of MDProblemSpec
   ******************************************************************************/
  if(verbose)
    cout << "Printing out the MDProblemSpec class... \n\n" << endl;
  
  os << "class MDProblemSpec" << endl;
  os << "{" << endl;
  os << "public:" << endl;

#if 0
  os << "static const void* ParamArray[];" << endl;
  os << "static const int ParamArraySize;\n" << endl;

  os << "static const int SizeOfParams[];" << endl;
  os << "static const int CardinalityOfParams[];\n" << endl;

  os << "static const WorkOrder TotalWorkOrder[];" << endl;
  os << "static const int TotalWorkOrderSize;\n" << endl;
  
  os << "static const WorkOrderMap WorkorderMap[];" << endl;
  os << "static const int WorkOrderMapSize;\n" << endl;
#endif 

  os << "static const UdfInvocation UdfInvocationTable[];" << endl;
  os << "static const int UdfInvocationTableSize;\n" << endl;

  os << "static const int SizeOfParams[];" << endl;
  
  
  /*****************************************************************/
  os << "static const SiteInfo SiteInformation[];" << endl;
  os << "static const int SiteInformationSize;\n" << endl;
  
  os << "static const int SiteIDtoSiteTypeMap[];" << endl; 
  os << "static const int SiteIDtoSiteTypeMapSize;\n" << endl;
  
  os << "static const int SiteIDtoFragmentIDMap[];" << endl;
  os << "static const int SiteIDtoFragmentIDMapSize;\n" << endl;

  os << "static const " <<  ljStructName << " LJPairs[];" << endl;
  os << "static const int LJPairsSize;\n" << endl;

  os << "static const " << ljStructName << " LJ14Pairs[];" << endl;
  os << "static const int LJ14PairsSize;\n" << endl;

  os << "static const ExcludedPair14 Pair14List[];" << endl;
  os << "static const int Pair14ListSize;\n" << endl;

  os << "static const ExcludedPair14 ExcludedList[];" << endl;
  os << "static const int ExcludedListSize;\n" << endl;

  os << "static const IrreduciblePartition IrreduciblePartitionList[];" << endl;
  os << "static const int IrreduciblePartitionListSize;\n" << endl; 
  
  os << "static const int IrreduciblePartitionCount;\n" << endl; 
  /******************************************************************************/



  /*******************************************************************************
   *  Printout the field names of the parameter arrays
   ******************************************************************************/
  int   i;
  int   totalWorkOrderCount  =  0;  
  char* paramName           =  NULL;  
  int   siteCount1;
  for (i = 0; i < uil->size(); i++)
    {
      invRecord = &(uil->getRecord(i));
      assert ( invRecord != NULL );
      
      // Get the size of workorder to be used when allocating the workOrderSize
      totalWorkOrderCount += invRecord->getSiteTupleList()->size();

      int printSites = 1;
      for (int zz=0; zz < i; zz++)
  {
    if( strcmp(invRecord->getSiteTupleList()->label(), 
         uil->getRecord(zz).getSiteTupleList()->label()) == 0)
      {
        printSites = 0;
      }
  }
      
      if(printSites)
  os << "static int " << invRecord->getSiteTupleList()->label() << "[];" << endl;      
      
      // If there are no parameters then no need to output the structure.
      if( invRecord->getParamTupleList()->parameterSpec().parameterCount() == 0 )
  continue;
      
      udfName = (char *) invRecord->getUDFName();
      UDF_RegistryIF::GetUDFCodeByName( udfName, udfCode);
      UDF_RegistryIF::GetStructureNameByUDFCode(udfCode, paramName);
      UDF_RegistryIF::GetNSitesByUDFCode( udfCode, siteCount1 );

      os << "static  " << paramName << " " << udfName << "ParamTupleTable[];\n" << endl;
    }
  
  // Closing the MDProblemSpec class
  os << "};\n" << endl;
  /*****************************************************************************/
  
  

  /******************************************************************************
   * Instantiate the paramArray, sizeOfParams and cardinalityOfParams
   * data structures
   *****************************************************************************/
  typedef char* MyString;
  int           udfInvocationTableSize;
  MyString*     paramTupleArray;
  MyString*     sizeOfParams;
  MyString*     siteTupleArray;
  int*          sizeTupleArray;
  int*          udfCodeArray;
  //  int*          cardinalityOfParams;

  
  udfInvocationTableSize = uil->size();
 
  if( udfInvocationTableSize != 0 ) 
    {

      paramTupleArray = new MyString[ udfInvocationTableSize  ];
      assert ( paramTupleArray != NULL );
      
      siteTupleArray = new MyString[ udfInvocationTableSize ];
      assert ( siteTupleArray != NULL );
      
      sizeTupleArray = new int[ udfInvocationTableSize ];
      assert ( sizeTupleArray != NULL );
      
      udfCodeArray = new int[ udfInvocationTableSize ];
      assert ( udfCodeArray != NULL );
      
      sizeOfParams = new MyString [ udfInvocationTableSize ];
      assert ( sizeOfParams != NULL );
      
      //  cardinalityOfParams = new int[ udfInvocationTableSize ];
      //  assert ( cardinalityOfParams != NULL ); 
      
      for (i = 0; i < udfInvocationTableSize; i++)
  {
    paramTupleArray[i] = new char[128];
    assert( paramTupleArray[i] != NULL );
    
    siteTupleArray[i] = new char[128];
    assert( siteTupleArray[i] != NULL );
    
    sizeOfParams[i] = new char[128];
    assert ( sizeOfParams[i] != NULL ); 
    
    udfCodeArray[i] = -1;
    sizeTupleArray[i] = 0;
    
    //      cardinalityOfParams[i] = 0;
    strcpy( sizeOfParams[i],   "sizeof( NULL )" );
    strcpy( paramTupleArray[i],     "NULL" );
    strcpy( siteTupleArray[i], "NULL" );
  }
      /*****************************************************************************/
      
      
      
      /******************************************************************************
       * Output the contents of the compressed parameters as well as setup 
       * the apropriate data structures for outputing workorders
       *****************************************************************************/
      int                       k,j;
      int                       argCount;
      int                       paramIndex;
      int                       siteCount;
      int                       doParam;
      int                       wCount               = 0;
      int                       parameterListSize;
      int*                      paramMap             = NULL;
      std::vector<int>          paramIndexSorted;
      
      const SiteTupleList*            siteTupleList        = NULL;
      const ParamTupleList*           paramTupleList       = NULL;
      
      typedef std::vector<Parameter> ParamTuple;
      typedef std::vector<int>       SiteTuple;
      
      const ParamTuple*           curParamTuple        = NULL;
      const SiteTuple*            curSiteTuple         = NULL;
      
      std::vector<ParamTuple> paramListCompressed;
      
      char*                     ordName              = NULL;
      char*                     ordType              = NULL;  
      
      char                      paramArrayTemp[128];
      memset(paramArrayTemp, '\0', 128);
      
      char                      siteArrayTemp[128];
      memset(siteArrayTemp, '\0', 128);
      
      for (i = 0; i < uil->size(); i++)
  {
    invRecord = &(uil->getRecord(i));
    assert ( invRecord != NULL );
    
    siteTupleList  = invRecord->getSiteTupleList();
    paramTupleList = invRecord->getParamTupleList(); 
    
    // Get the name of the UDF Operation 
    udfName = (char *) invRecord->getUDFName();
    UDF_RegistryIF::GetUDFCodeByName( udfName, udfCode);
    assertMsg(udfCode != UDF_RegistryIF::CODENOTFOUND, "Invalid UdfName: ", udfName, "");
    
    UDF_RegistryIF::GetNSitesByUDFCode(udfCode, siteCount);
    assertMsg(siteCount != UDF_RegistryIF::CODENOTFOUND, "UdfCode: ", udfCode, " not found.");
    assertMsg( siteTupleList->siteCount() == siteCount, "The site count numbers for ", udfName, " does not match.");
    
    
    if(( siteTupleList != NULL ) &&
       ( paramTupleList != NULL ))
      {
        assert ( siteTupleList->size() == paramTupleList->size() );
      }
    
    // Do the paramTupleList related stuff
    if( paramTupleList != NULL)
      {
        UDF_RegistryIF::GetStructureNameByUDFCode(udfCode, paramName);
        strcpy( sizeOfParams[ i ], "sizeof( ");
        strcat( sizeOfParams[ i ], paramName );
        strcat( sizeOfParams[ i ], " )");
        
        strcpy(paramArrayTemp, udfName);
        strcat(paramArrayTemp, "ParamTupleTable");
        strcpy(paramTupleArray[ i ], paramArrayTemp);
        
        udfCodeArray[ i ] = udfCode;
        sizeTupleArray[i] = paramTupleList->size();
    

        os << "/* " << endl;
        os << "The parameters are in the order of the structs above." << endl;
        os << "*/" << endl;
        
        if(verbose)
    cout << "Printing out " << udfName << "ParamTupleTable " << "...\n\n" << endl;
        
        os << paramName << " MDProblemSpec::" << udfName << "ParamTupleTable[] = {\n" << endl;
        
        // Create the maping
        j = 0;
        UDF_RegistryIF::GetArgCountByUDFCode(udfCode, argCount);
        
        assert( argCount == paramTupleList->parameterSpec().parameterCount() );
        
        int mustEnter;
        paramMap = new int[argCount];
        assert ( paramMap != NULL );
        
        // Set up the map for outputing the parameters in the order as they appear in the udf structs
        while( j < argCount )
    {
      UDF_RegistryIF::GetArgByOrd(udfCode, j, ordName, ordType);
      mustEnter = 0;
      
      for (int z = 0; z < paramTupleList->parameterSpec().parameterCount(); z++)
        {
          if( strcmp( paramTupleList->parameterSpec().parameterSpec(z).d_name, ordName) == 0 )
      {
        paramMap[j] = z;
        mustEnter = 1;
        break;
      }
        }
      
      assertMsg(mustEnter, "The arg name: ", ordName, " is not found in the list of parameters");
      
      j++;
    }
        
        // Output the parameters using the map above
        j = 0;
        while ( j < paramTupleList->size() )
    {
      curParamTuple = &(paramTupleList->getParamTuple(j));
      os << "{ "; 
      for (int n = 0; n < curParamTuple->size(); n++)
        { 
          os << (*curParamTuple)[ paramMap[n] ].value();
          if ( n != curParamTuple->size() - 1)
      os << " , ";
        }
      if (j != paramTupleList->size()-1 )
        os << "} ," << endl;
      else 
        os << "} " << endl;
      
      j++;
    }
        
        os << "};\n" << endl;	  
      }  
    
    if (paramMap != NULL)
      delete paramMap;
    
    if( siteTupleList != NULL )
      {
        udfCodeArray[ i ] = udfCode;
        sizeTupleArray[i] = siteTupleList->size();
        
        int listExists = 0;
        // check if such a list was already defined
        for (int zz=0; zz < i; zz++)
    {
      if (strcmp(siteTupleArray[zz], siteTupleList->label()) == 0)
        {
          listExists = 1;
        }
    }
        
        strcpy( siteArrayTemp, siteTupleList->label());
        strcpy( siteTupleArray[ i ], siteArrayTemp);
        
        if(listExists)
    {
      // no need to print out the sites
      continue;
    }
        
        os << "/* " << endl;
        os << "These are the sites." << endl;
        os << "*/" << endl;
        
        if(verbose)
    cout << "Printing out " << siteTupleList->label() << "...\n\n" << endl;
        
        int siteCount;
        UDF_RegistryIF::GetNSitesByUDFCode( udfCode, siteCount );
        os << "int MDProblemSpec::" << siteTupleList->label() << "[] = {\n" << endl;
        j=0;
        
        int z;
        while( j < siteTupleList->size() )
    {
      z = 0;
      while( z < siteCount )
        {
          os << siteTupleList->getSiteTuple(j)[z];
          if( z != siteCount-1)
      os << ",";
          z++;
        }
      
      if ( j != siteTupleList->size()-1 )
        os << " ," << endl;
      
      j++;
    }
        
        os << "};\n" << endl;
      }
    
    
  } 
      /*****************************************************************************/
      
      
      
      /*****************************************************************************
       *  Output the array with pointers to parater tables
       *****************************************************************************/
      os << "/* " << endl;
      os << "UdfCode, size of tupleList, siteTupleListPtr, paramTupleListPtr" << endl;
      os << "*/\n" << endl;
      os << "const UdfInvocation MDProblemSpec::UdfInvocationTable[] = " << endl;
      os << "{\n"; 
      
      i = 0;
      while ( i < uil->size() )
  {
    os << "{ ";
    os << udfCodeArray[ i ]   << " , ";
    os << sizeTupleArray[ i ] << " , ";
    os << "(int*) " << siteTupleArray[i]   << " , ";
    os << "(void*)" << paramTupleArray[i];
    os << " }";
    
    if ( i != uil->size()-1 ) 
      os << "," << endl;
    
    i++;
  } 
      
      os << "\n};\n" << endl;
      os << "const int MDProblemSpec::UdfInvocationTableSize = " << uil->size() << ";\n" <<endl;
      /*****************************************************************************/
      
      
      
      /*****************************************************************************
       *  Output the array with sizes of structures in use for this system
       *****************************************************************************/
      os << "const int MDProblemSpec::SizeOfParams[] = " << endl;
      os << "{\n";
      
      i = 0;
      char* pName = NULL;
      while ( i < uil->size() )
  {
    os << sizeOfParams[i];
    
    if ( i != uil->size()-1 )
      os << "," << endl;
    i++;
  }
      os << "};\n" << endl;
      delete [] paramTupleArray;
      delete [] siteTupleArray;
      delete [] udfCodeArray;
      delete [] sizeTupleArray;
      delete [] sizeOfParams;
      /*****************************************************************************/
    }
  else 
    {

      // The udfInvocationTable is empty
    
      /*****************************************************************************
       *  Output the array with pointers to parater tables
       *****************************************************************************/
      os << "/* " << endl;
      os << "UdfCode, size of tupleList, siteTupleListPtr, paramTupleListPtr" << endl;
      os << "*/\n" << endl;
      os << "const UdfInvocation MDProblemSpec::UdfInvocationTable[] = " << endl;
      os << "{\n"; 
      
      os << "{ -1, 0, NULL, NULL }";
      
      os << "\n};\n" << endl;
      os << "const int MDProblemSpec::UdfInvocationTableSize = 0;\n" <<endl;
      /*****************************************************************************/

            /*****************************************************************************
       *  Output the array with sizes of structures in use for this system
       *****************************************************************************/
      os << "const int MDProblemSpec::SizeOfParams[] = " << endl;
      os << "{ sizeof(NULL) };\n" << endl;
    }
      
  /******************************************************************************
   *  Code below stays the same  -arayshu 8/8/01
   ******************************************************************************/
  
  
  
  /******************************************************************************
   *  Setup for the siteInfo table
   ******************************************************************************/
  const TypeList*    siteList      = tlt->byTypePrepend("site_");
  int                numSites      = siteList->typeCount();
  char*              ljIndexTemp   = NULL;
  char*              lj14IndexTemp = NULL;
  ParameterSpecTable pst           = siteList->parameterSpec();
  SiteInfoChar*      siteInfoList  = new SiteInfoChar[numSites];

  assert ( siteInfoList != NULL );
  assert( numSites > 0 );
  
  //  os << "TEST RUN of SiteInfo" << endl;
  i=0;
  while (i < numSites) 
    {
      //      paramList = siteList->parameterRecord(i);
      siteInfoList[i].siteId = i;

      strcpy(siteInfoList[i].mass, siteList->parameterRecord(i)[ pst.indexOfParameter("mass") ].value() );
      strcpy(siteInfoList[i].charge,  siteList->parameterRecord(i)[ pst.indexOfParameter("charge") ].value() );
      
      ljIndexTemp = (char *) (siteList->parameterRecord(i)[ pst.indexOfParameter("lennard_jones_type") ].value());
      lj14IndexTemp = (char *) (siteList->parameterRecord(i)[ pst.indexOfParameter("lennard_jones_14_type") ].value());
      
      ljIndexTemp = (char *) (rindex(ljIndexTemp, '_') + 1);
      lj14IndexTemp = (char *) (rindex(lj14IndexTemp, '_') + 1);

      siteInfoList[i].ljIndex = atoi( ljIndexTemp ) - 1;
      siteInfoList[i].lj14Index = atoi( lj14IndexTemp ) - 1;
      
      //  os << siteInfoList[i].mass << " , " << siteInfoList[i].charge << " , "
      //	 << siteInfoList[i].ljIndex << " , " << siteInfoList[i].lj14Index << endl;
      i++;
    }
  /*****************************************************************************/

  

  /******************************************************************************
   *  Compress the siteInfo table
   ******************************************************************************/
  
  int           siteTypeIndex          = 0;
  int           compressedIndex        = -1;
  SiteInfoChar* prevSiteInfo           = NULL;
  SiteInfoChar* curSiteInfo            = NULL;

  SiteInfoChar* compressedSiteInfoList = new SiteInfoChar[numSites];
  assert ( compressedSiteInfoList != NULL );

  qsort(siteInfoList,
  numSites,
  sizeof(SiteInfoChar),
  SiteInfoChar::compareSiteInfoChar);  

  i = 0;
  while( i < numSites )
    {
      curSiteInfo = &(siteInfoList[i]);
      if( !( *(curSiteInfo) == *(prevSiteInfo) ) )
  {
    compressedIndex++;
    compressedSiteInfoList[compressedIndex] = *(curSiteInfo);
  }
      
      siteInfoList[i].type = compressedIndex;
      prevSiteInfo = curSiteInfo;
      i++;
    }
  /*****************************************************************************/



  /*****************************************************************************
   *   Print Out the compressed table with Mass, Charge, LJ, LJ_14
   ****************************************************************************/
  if(verbose)
    cout << "Printing out the SiteInformation Table...\n\n" << endl;

  os << "/* " << endl;
  os << "Indexed by site type  = { mass, halfInverseMass, charge, lj_type, lj_14_type };" << endl;
  os << "*/\n" << endl;
  
  os << "const SiteInfo MDProblemSpec::SiteInformation[] = " << endl;
  os << "{" << endl; 
  
  i=0;
  double mass;
  while( i < compressedIndex+1 )
    {
      os << "{ "; 
      mass = atof(compressedSiteInfoList[i].mass);

      os << mass  << " , "
  //	 << (double)(1.0/(2*mass)) << " , "  
   << "1.0/(2.0 * " << mass << ") , "
   << compressedSiteInfoList[i].charge << " , "
   << compressedSiteInfoList[i].ljIndex << " , "
   << compressedSiteInfoList[i].lj14Index;
      
      if (i != compressedIndex) 
  os << " }," << endl;
      else
  os << " }" << endl;

      i++;
    }
  
  os << "};\n" << endl;
  
  os << "const int MDProblemSpec::SiteInformationSize = " << compressedIndex + 1 << ";\n" << endl;
  /*****************************************************************************/
  


  /*****************************************************************************
   *   Print Out the map into the siteInformation table
   ****************************************************************************/
  os << "/* " << endl;
  os << "Mapping from SiteId to site type." << endl;
  os << "*/\n" << endl;

  os << "const int MDProblemSpec::SiteIDtoSiteTypeMap[] = " << endl;
  os << "{" << endl; 
  
  qsort(siteInfoList,
  numSites,
  sizeof(SiteInfoChar),
  SiteInfoChar::compareSiteIdChar);
  
  i = 0;
  while ( i < numSites )
    {
      os << siteInfoList[i].type;
      
      if( i != numSites-1 )
  os << ", " << endl;
      
      i++;
    } 
  
  os << "};\n" << endl;
  
  os << "const int MDProblemSpec::SiteIDtoSiteTypeMapSize = " << numSites << ";\n" << endl;
  /*****************************************************************************/
  

  
  /*****************************************************************************
   *   Print Out the fragment info 
   ****************************************************************************/
#ifdef EVERY_SITE_TO_ONE_FRAGMENT
  os << "/* " << endl;
  os << "Every site is in ONE fragment" << endl;
  os << "*/\n" << endl;

  os << "const int MDProblemSpec::SiteIDtoFramentIDMap[] = " << endl;
  os << "{" << endl; 

  i = 0;
  while ( i < numSites )
    {
      // every site is in fragment 0;
      os << 0;
      
      if( i != numSites-1 )
  os << ", " << endl;
      
      i++;
    } 
  
  os << "};\n" << endl;
#else  
  os << "/* " << endl;
  os << "Every site is in its own fragment" << endl;
  os << "*/\n" << endl;
  
  os << "const int MDProblemSpec::SiteIDtoFragmentIDMap[] = " << endl;
  os << "{" << endl; 

  i = 0;
  while ( i < numSites )
    {
      // ith site is in ith fragment
      os << i;
      
      if( i != numSites-1 )
  os << ", " << endl;
      
      i++;
    } 
  
  os << "};\n" << endl;
#endif
  os << "const int MDProblemSpec::SiteIDtoFragmentIDMapSize = " << numSites << ";\n" << endl;
  delete compressedSiteInfoList;
  delete siteInfoList;
  /*****************************************************************************/


  
  /**********************************************************************
   *                 Do the LJ calculations
   *********************************************************************/
  int              index;
  const PairList*  ljpairList    = plt->byTypeClassPrepend("lj_");
  int              ljTypeCount   = ljpairList->typeCount();
  assert ( ljTypeCount > 0 );

  int              ljArraySize   = ((ljTypeCount * ljTypeCount) + ljTypeCount) / 2;
  LJPairChar*      ljPairArray   = new LJPairChar[ ljArraySize ];
  assert (ljPairArray != NULL );

  ParameterSpecTable ljParamSpec = ljpairList->parameterSpec();
  int j=0;
  
  i = 0;
  while( i < ljTypeCount )
    {
      j = 0;
      while ( j <= i)
  {
    index = GetLowerTriangleIndex(i,j);

    strcpy(ljPairArray[index].epsilon, ljpairList->paramRecord(i,j).
              d_parameter[ ljParamSpec.indexOfParameter("epsilon") ].value());
    
    strcpy(ljPairArray[index].sigma, ljpairList->paramRecord(i,j).
            d_parameter[ ljParamSpec.indexOfParameter("sigma") ].value());

    j++;
  }
      i++;
    }
  /******************************************************************************/
  
  

  /**********************************************************************
   *                 Print out the lj PairList
   *********************************************************************/
  if(verbose)
    cout << "Printing out the Lennard Jones pairList...\n\n" << endl;

  os << "/* " << endl;
  os << "An array of LJ Pairs" << endl;
  os << "*/\n" << endl;

  os << "const " << ljStructName  << " MDProblemSpec::LJPairs[] = " << endl;
  os << "{" << endl; 
  
  i = 0;
  while ( i < ljArraySize )
    {
      os << "{";
      
      os << ljPairArray[i].epsilon << " , "
   << ljPairArray[i].sigma;
      
      if (i != ljArraySize - 1)
  os << "}," << endl;
      else
  os << "}" << endl;
      
      i++;
    }
  
  delete ljPairArray;
  os << "};" << endl;
  os << "const int MDProblemSpec::LJPairsSize = " << ljArraySize <<";\n" << endl;
  /******************************************************************************/



  /******************************************************************************
   *                 Do the lj14 calculations
   *****************************************************************************/
  int                index14;
  const PairList*    lj14pairList  = plt->byTypeClassPrepend("lj14_");  
  int                lj14TypeCount = lj14pairList->typeCount();
  assert ( lj14TypeCount > 0 );
  
  int                lj14ArraySize = ((lj14TypeCount * lj14TypeCount) + lj14TypeCount) / 2;
  LJPairChar*        lj14PairArray = new LJPairChar[ lj14ArraySize ];
  assert ( lj14PairArray != NULL );
  
  ParameterSpecTable lj14ParamSpec = lj14pairList->parameterSpec();
  
  i = 0;
  while( i < lj14TypeCount )
    {
      j = 0;
      while ( j <= i )
  {
    index14 = GetLowerTriangleIndex(i,j);
    
    strcpy(lj14PairArray[index14].epsilon, lj14pairList->paramRecord(i,j).
              d_parameter[ lj14ParamSpec.indexOfParameter("epsilon") ].value());
    
    strcpy(lj14PairArray[index14].sigma, lj14pairList->paramRecord(i,j).
            d_parameter[ lj14ParamSpec.indexOfParameter("sigma") ].value());

    j++;
  }
      i++;
    }
  /******************************************************************************/


  
  /******************************************************************************
   *                 Print out the LJ14Pair list
   *****************************************************************************/
  if(verbose)
    cout << "Printing out the Lennard Jones14 pairList...\n\n" << endl;

  os << "/* " << endl;
  os << "An array of LJ 1-4 Pairs" << endl;
  os << "*/\n" << endl;

  os << "const " << ljStructName << " MDProblemSpec::LJ14Pairs[] = " << endl;
  os << "{" << endl; 
  
  i = 0;
  while ( i < lj14ArraySize )
    {
      os << "{";
      
      os << lj14PairArray[i].epsilon << " , "
   << lj14PairArray[i].sigma;
      
      if (i != lj14ArraySize - 1)
  os << "}," << endl;
      else
  os << "}" << endl;
  
      i++;
    }

  delete lj14PairArray;
  os << "};\n" << endl;
  os << "const int MDProblemSpec::LJ14PairsSize = " << lj14ArraySize <<";\n" << endl;
  /******************************************************************************/
  

  
  /******************************************************************************
   *    Get the number of excluded and pair14 entries            
   *****************************************************************************/
  int excludedCount = 0;
  int pair14Count   = 0;
  
  // Get the sizes of the excluded and the pair14 arrays
  for ( i = 0; i < gSep.getAdjListSize(); ++i)
    {
      //      cerr << "Size of list " << i << ": " << gSep.getGraphSepArray()[i].size() << endl;
      for (std::vector<std::pair<int, int> >::const_iterator iter =
       gSep.getGraphSepArray()[i].begin();
     iter != gSep.getGraphSepArray()[i].end();
     ++iter)
  {
    //  cerr << "oOo" << endl;
    if ((*iter).first == 3)
      pair14Count++;
    else if ( ((*iter).first == 1) || ((*iter).first == 2) )
      excludedCount++;
  }
    }
  /******************************************************************************/


  
  /******************************************************************************
   *    Setup the pair14 list 
   *****************************************************************************/
  if (  pair14Count != 0 )
    {
      ExcludedPair14Help* pair14List   = new ExcludedPair14Help[ pair14Count ];
      assert ( pair14List != NULL );
      
      pair14Count = 0;
      for ( i = 0; i < gSep.getAdjListSize(); ++i)
  {
    for (std::vector<std::pair<int, int> >::const_iterator iter =
     gSep.getGraphSepArray()[i].begin();
         iter != gSep.getGraphSepArray()[i].end();
         ++iter)
      {
        if ((*iter).first == 3)
    {
      if (i < (*iter).second)
        {
          pair14List[pair14Count].site1 = i;
          pair14List[pair14Count].site2 = (*iter).second;
        }
      else 
        {
          pair14List[pair14Count].site1 = (*iter).second;
          pair14List[pair14Count].site2 = i;
        }
      pair14Count++;
    }
      }
  }
      
      /**************************************************************************
       *                  Compress the Pair14 List
       *************************************************************************/
      qsort(pair14List,
      pair14Count,
      sizeof(ExcludedPair14Help),
      ExcludedPair14Help::compare);
      
      ExcludedPair14Help* prev = NULL;
      ExcludedPair14Help* cur = NULL;
      i=0;
      int pair14CountCompressed = -1;  
      
      while ( i < pair14Count )
  {
    cur = &(pair14List[i]);
    if ( !((*prev) == (*cur)) ) 
      {
        pair14CountCompressed++;
        pair14List[ pair14CountCompressed ].site1 = cur->site1;
        pair14List[ pair14CountCompressed ].site2 = cur->site2;
      }
    prev = cur;
    i++;
  }
      
      /***************************************************************************
       *                  PrintOut the Pair14 List
       **************************************************************************/
      if(verbose)
  cout << "Printing out the Pair14 pairList...\n\n" << endl;
      
      os << "const ExcludedPair14 MDProblemSpec::Pair14List[] = " << endl;
      os << "{" << endl;
      
      i=0;
      while ( i <= pair14CountCompressed)
  {
    os << "{ ";
    
    os << pair14List[i].site1 << " , " 
       << pair14List[i].site2;
    
    if ( i != pair14CountCompressed )
      os << " }," << endl;
    else
      os << " }" << endl;
    i++;
  } 
      
      os <<"};\n" << endl;
      
      os << "const int MDProblemSpec::Pair14ListSize = " << pair14CountCompressed + 1 << ";\n" << endl;
      delete pair14List;
      
      /**************************************************************************/
    }
  else 
    {
      /***************************************************************************
       *            If there are no pair14 entries printout { -1, -1 }
       **************************************************************************/
      os << "const ExcludedPair14 MDProblemSpec::Pair14List[] = " << endl;
      os << "{ {" << endl;
      os << -1 << "," << -1 << endl;
      os <<"} };\n" << endl;
      
      os << "const int MDProblemSpec::Pair14ListSize = " << 1 << ";\n" << endl;
      /**************************************************************************/       
    }
  
  
 /******************************************************************************
   *    Setup the excluded list 
   *****************************************************************************/
  if (excludedCount !=0 )
    {
      ExcludedPair14Help* excludedList = new ExcludedPair14Help[ excludedCount ];
      assert ( excludedList != NULL );

      excludedCount = 0;
      for ( i = 0; i < gSep.getAdjListSize(); ++i)
  {
    for (std::vector<std::pair<int, int> >::const_iterator iter =
       gSep.getGraphSepArray()[i].begin();
         iter != gSep.getGraphSepArray()[i].end();
         ++iter)
      {
        if ( ((*iter).first == 1) || ((*iter).first == 2) )
    {
      if (i < (*iter).second)
        {
          excludedList[excludedCount].site1 = i;
          excludedList[excludedCount].site2 = (*iter).second;
        }
      else
        {
          excludedList[excludedCount].site1 = (*iter).second;
          excludedList[excludedCount].site2 = i;
        }
      excludedCount++;
    }
      }
  }
      
      /********************************************************************
       *                  Compress the Excluded List
       *******************************************************************/
      
      qsort(excludedList,
      excludedCount,
      sizeof(ExcludedPair14Help),
      ExcludedPair14Help::compare);
      
      ExcludedPair14Help* prev1 = NULL;
      ExcludedPair14Help* cur1 = NULL;
      i=0;
      int excludedCountCompressed = -1;
      
      while ( i < excludedCount )
  {
    cur1 = &(excludedList[i]);
    if ( !((*prev1) == (*cur1)) ) 
      {
        excludedCountCompressed++;
        excludedList[ excludedCountCompressed ].site1 = cur1->site1;
        excludedList[ excludedCountCompressed ].site2 = cur1->site2;
      }
    prev1 = cur1;
    i++;
  }

      /********************************************************************
       *                  Printout the Excluded List
       *******************************************************************/      
      if(verbose)
  cout << "Printing out the Excluded pairList...\n\n" << endl;

      os << "const ExcludedPair14 MDProblemSpec::ExcludedList[] = " << endl;
      os << "{" << endl;
      
      i=0;
      while ( i <= excludedCountCompressed)
  {
    os << "{ ";
    
    os << excludedList[i].site1 << " , " 
       << excludedList[i].site2;
    
    if ( i != excludedCountCompressed )
      os << " }," << endl;
    else
      os << " }" << endl;
    i++;
  } 
      
      os <<"};\n" << endl;
      
      os << "const int MDProblemSpec::ExcludedListSize = " << excludedCountCompressed + 1 << ";\n" << endl;
      delete excludedList;
      /**************************************************************************/
    }
  else
    {   
      /***************************************************************************
       *            If there are no excluded entries printout { -1, -1 }
       **************************************************************************/
      os << "const ExcludedPair14 MDProblemSpec::ExcludedList[] = " << endl;
      os << "{ {" << endl;
      os << -1 << "," << -1 << endl;
      os <<"} };\n" << endl;
      
      os << "const int MDProblemSpec::ExcludedListSize = " << 1 << ";\n" << endl;
      /**************************************************************************/       
    }
  
  
  
  /******************************************************************************
   *             Create the irreducible partition table
   *****************************************************************************/
  int                         numNodes       = partition.nodeCount();
  assert (numNodes > 0);
  
  int                         partitionCount = partition.partitionCount();
  int                         curRunCount    = 0;
  int                         irrPartCount   = -1;
  int                         partCount      = 0;  
  
  IrreduciblePartitionHelper* iph            = new IrreduciblePartitionHelper[ numNodes ];
  assert ( iph != NULL );
  
  IrreduciblePartitionHelper* prevIPH        = new IrreduciblePartitionHelper;
  assert ( prevIPH != NULL );
  IrreduciblePartitionHelper* curIPH         = NULL;
  
  IrreduciblePartition*       irrPart        = new IrreduciblePartition[ numNodes ];
  assert ( irrPart != NULL );
  
  

  i=0;
  while( i < numNodes )
    {
      iph[i].partitionId = partition.subgraphLabel( i );
      iph[i].absSiteId = i;
      i++;
    }

  /******************************************************************************
   *           Sort the iph list first by partitionId then by siteId
   *****************************************************************************/
  qsort(iph,
  numNodes,
  sizeof(IrreduciblePartitionHelper),
  IrreduciblePartitionHelper::compare);
  /*****************************************************************************/
  


  /******************************************************************************
   *           Create a run length compression of the irreducible partition
   *****************************************************************************/
  i=0;
  while ( i < numNodes )
    {
      curIPH = &iph[i];
      if ( curIPH->partitionId == prevIPH->partitionId )
  {
    if( (curIPH->absSiteId - prevIPH->absSiteId) == 1) 
      {
        curRunCount++;
        irrPart[ irrPartCount ].numSites = curRunCount;
      }
    else 
      {
        irrPartCount++;
        irrPart[ irrPartCount ].partitionId = curIPH->partitionId;
        irrPart[ irrPartCount ].startSiteId = curIPH->absSiteId;
        irrPart[ irrPartCount ].numSites = 1;
        curRunCount=1;
      }
  }
      else
  {
    partCount++;
    irrPartCount++;
    irrPart[ irrPartCount ].partitionId = curIPH->partitionId;
    irrPart[ irrPartCount ].startSiteId = curIPH->absSiteId;
    irrPart[ irrPartCount ].numSites = 1;
    curRunCount=1;
  }
      
      prevIPH = curIPH;
      i++;
    }
  /*****************************************************************************/
  
  
  switch(fragment_mode)
    {
    case ALL_SITES_IN_ONE_FRAGMENT:
    case ALL_MOLECULES_IN_ONE_FRAGMENT:
      {
  /******************************************************************************
   *           Printout the Irreducible partition
   *****************************************************************************/
  if(verbose)
    cout << "Printing out the Irreducible Partition List... All sites/molecules in ONE fragment \n\n" << endl;
  
        os << "// All sites/molecules in one fragment\n" << endl;
  os << "const IrreduciblePartition MDProblemSpec::IrreduciblePartitionList[] = " << endl;
  os << "{" << endl;
  
// 	i=0;
// 	while (i <= irrPartCount)
// 	  {
// 	    os << "{ 0 , " 
// 	       << irrPart[ i ].startSiteId << " , "
// 	       << irrPart[ i ].numSites;
      
// 	    if (i != irrPartCount )
// 	      os << "}," << endl;
// 	    else
// 	      os << "}" << endl;
// 	    i++;
// 	  }
  
// 	os << "};\n" << endl;
  
// 	os << "const int MDProblemSpec::IrreduciblePartitionListSize = " << irrPartCount + 1 << ";\n" << endl;

  os << "{ 0, 0, " << numNodes << " }" << endl;
  os << "};\n" << endl;
  
  os << "const int MDProblemSpec::IrreduciblePartitionListSize = 1;\n" << endl;

  os << "const int MDProblemSpec::IrreduciblePartitionCount = 1;\n" << endl;
  /*****************************************************************************/
  
  break;
      }
    case ALL_SITES_IN_THEIR_OWN_FRAGMENT:
      {
  /******************************************************************************
   *           Printout the Irreducible partition
   *****************************************************************************/
  if(verbose)
    cout << "Printing out the Irreducible Partition List... All sites in their own fragment \n\n" << endl;
   
        os << "// All sites in thier own fragment\n" << endl;
  os << "const IrreduciblePartition MDProblemSpec::IrreduciblePartitionList[] = " << endl;
  os << "{" << endl;
  
  i=0;
  int partitionID = 0;
  int startSiteId = 0;
  while (i <= irrPartCount)
    {
      assert( irrPart[ i ].numSites > 0 );
      startSiteId =  irrPart[ i ].startSiteId;

      for(int zz = 0; zz < irrPart[ i ].numSites; zz++)
        {
    os << "{ " << partitionID++ << " , " 
       << startSiteId++  << " , "
       << " 1 }";

    if ( zz != irrPart[ i ].numSites-1 )
      os << "," << endl;		
        }
      
      if (i != irrPartCount )
        os << "," << endl;
      
      i++;
      
    }
  
  os << "};\n" << endl;
  
  os << "const int MDProblemSpec::IrreduciblePartitionListSize = " << partitionID  << ";\n" << endl;
  os << "const int MDProblemSpec::IrreduciblePartitionCount = " << partitionID  << ";\n" << endl;
  /*****************************************************************************/
  break;
      }
    case ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT:
      {
  /******************************************************************************
   *           Printout the Irreducible partition
   *****************************************************************************/
  if(verbose)
    cout << "Printing out the Irreducible Partition List... All molecules in their own fragment (default setting)\n\n" << endl;
   
        os << "// All molecules in their own fragment\n" << endl;
  os << "const IrreduciblePartition MDProblemSpec::IrreduciblePartitionList[] = " << endl;
  os << "{" << endl;
  
  i=0;
  while (i <= irrPartCount)
    {
      os << "{ " <<  irrPart[ i ].partitionId << " , " 
         << irrPart[ i ].startSiteId << " , "
         << irrPart[ i ].numSites;
      
      if (i != irrPartCount )
        os << "}," << endl;
      else
        os << "}" << endl;
      i++;
    }
  
  os << "};\n" << endl;
  
  os << "const int MDProblemSpec::IrreduciblePartitionListSize = " << irrPartCount + 1 << ";\n" << endl;
  os << "const int MDProblemSpec::IrreduciblePartitionCount = " << partCount << ";\n" << endl;
  /*****************************************************************************/
  break;
      }
    default:
      {
  assert(0);
      }
    }



  
  // Fenito
  os.close();
  
  if(verbose)
    cout << "Success." << endl;

}

int main( int argc, char** argv )
{
  char* xmlFile          = NULL;
  char* tempStr          = NULL;
  char* probSpecFileName = new char[32];

  if (argc < 3) 
    {
      cerr << "Input: <xmlfile> <name of the resulting hpp file without .hpp> [optional flags]" << endl;
      cerr << "flags: " << endl;
      cerr << "       -v     = Verbose mode" << endl;
      cerr << "       --all_sites_in_one_fragment" << endl;
      cerr << "       --all_molecules_in_one_fragment" << endl;
      cerr << "       --all_sites_in_their_own_fragment" << endl;
      cerr << "       --all_molecules_in_their_own_fragment(default)" << endl;
      return(1);
    }
  
  xmlFile = argv[1];
  tempStr = strcpy(probSpecFileName, argv[2]);
  probSpecFileName = strcat(tempStr, ".hpp");
  
  int i = 3;
  while ( i < argc )
    {
      if (strcmp(argv[i], "-v") == 0)
  verbose  = 1;
      else if (strcmp(argv[i], "--all_sites_in_one_fragment") == 0)
  fragment_mode = ALL_SITES_IN_ONE_FRAGMENT;
      else if (strcmp(argv[i], "--all_molecules_in_one_fragment") == 0)
  fragment_mode = ALL_MOLECULES_IN_ONE_FRAGMENT;
      else if (strcmp(argv[i], "--all_sites_in_their_own_fragment") == 0)
  fragment_mode = ALL_SITES_IN_THEIR_OWN_FRAGMENT;
      else if (strcmp(argv[i], "--all_molecules_in_their_own_fragment") == 0)
  fragment_mode = ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT;
  
      i++;
    }
  
  // Initialize the XML4C2 system
  try
    {
      XMLPlatformUtils::Initialize();
    }
  
  catch (const XMLException& toCatch)
    {
      cerr << "Error during initialization! :\n"
     << DOMString(toCatch.getMessage()) << endl;
      return 1;
    }

  SAXParser parser;
  parser.setValidationScheme( SAXParser::Val_Always );
  parser.setDoNamespaces(true);

  FFSAXFFParamsHandlerState* state = new FFSAXFFParamsHandlerState;
  assert(state != NULL);
  FFSAXHandler handler(state);

  try
    {
      parser.setDocumentHandler(&handler);
      parser.setErrorHandler(&handler);
      parser.parse(xmlFile);
    }
  catch (const XMLException& toCatch)
    {
      cerr << "\nAn error occured\n  Error: "
     << DOMString(toCatch.getMessage())
     << "\n" << endl;
      return -1;
    }

  TypeListTable* tlTable = handler.typeListTable();

  BondListAsGraph bondAdj(*tlTable);

  const std::vector<std::vector<int> >& adjList = bondAdj.asAdjList();
  GraphSeparation gSep(adjList, 3);
  ConnectedSubgraphs partition(adjList);

  dumpProbSpec(probSpecFileName, 
         handler.headerParameterList(),
         handler.udfInvocationTable(),
         //	       handler.forceListTable(), 
         tlTable,
         handler.pairListTable(), 
         gSep,
         partition);

  XMLPlatformUtils::Terminate();

  return(0);
}
