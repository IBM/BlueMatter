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
// File: FFParameters.cpp
// Author: RSG
// Date: April 25, 2001
// Class: FFParameters
// Inheritance: FFParametersInterface
// Description:  This class implements the methods to provide force
//               field parameter information via the interface defined
//               by FFParametersInterface
// ************************************************************************

#include "ffparameters.hpp"

#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>


#include <dom/DOM_DOMException.hpp>
#include <dom/DOMString.hpp>
#include <dom/DOM_Node.hpp>
#include <dom/DOM_Element.hpp>
#include <parsers/DOMParser.hpp>
#include <dom/DOM.hpp>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


// utility routines

// retrieves transcoded element value for singleton descendants of the
// DOM_Element whose address is passed as the first argument.  The tag
// value to be searched for is passed as the second argument. The
// caller is responsible for deleting the character array returned by
// this routine

static char* getElementValueOfChild(const DOM_Element*, const char*);

static inline double convertToDouble(char* val)
{
  double foo = atof(val);
  delete [] val;
  return foo;
}

static inline int convertToInt(char* val)
{
  int foo = atoi(val);
  delete [] val;
  return foo;
}

// retrieves a DOM_NodeList of elements with tag eltName under an
// element with tag = listName.  If there are multiple lists with the
// same listname, then the attribute class will be used to choose the
// listName node with attribute class = classInstance.

static DOM_NodeList getElementsInList(DOM_Document aDoc,
				      const char* listName,
				      const char* eltName,
				      const char* attribute = NULL,
				      const char* attInstance = NULL);

static int getAndStripIntegerAttribute(DOM_Element* elt_p,
				       const char* att,
				       const char* attBase);

struct SymbolType
{
  double d_mass;
  char* d_name_p;
  // methods
  ~SymbolType()
  {
    delete [] d_name_p;
  }
  static int compare(const void* key, const void* element);
};

class SymbolTypeTable
{
private:
  int d_count;
  SymbolType* d_table_p;
public:
  ~SymbolTypeTable()
  {
    delete [] d_table_p;
  }
  static SymbolTypeTable* create(DOM_Document&);
  const SymbolType* findWithKey(const char*) const;
};

typedef FFParametersInterface::LJParam* LJParamPointer;

struct FFParameters_i
{
  static void processHeader(DOM_Document& aDoc, FFParameters* ffp_p);

  static void processSiteList(DOM_Document& doc, FFParameters* ffp_p,
			    SymbolTypeTable* table_p);

  static void processBondList(DOM_Document& aDoc, FFParameters* ffp_p);

  static void processAngleList(DOM_Document& aDoc, FFParameters* ffp_p);

  static void processTorsionList(DOM_Document& aDoc, FFParameters* ffp_p);

  static void processImproperTorsionList(DOM_Document& aDoc,
					 FFParameters* ffp_p);

  static void processLJNormalList(DOM_Document& aDoc,
				  FFParameters* ffp_p);

  static void processLJ14List(DOM_Document& aDoc,
				  FFParameters* ffp_p);

};

// static test method

int FFParameters::main(int argc, char** argv)
{
  if (argc < 2)
    {
      cerr << argv[0] << " input_filename" << endl;
      exit(-1);
    }

  const char* inputFile = argv[1];

  FFParametersInterface* ffp = new FFParameters();
  assert(ffp != NULL);

  try
    {
      ffp->init(inputFile);
    }
    catch(const XMLException& toCatch)
    {
        char *eMsg = XMLString::transcode(toCatch.getMessage());
        cerr << "FFParameters::init  Error\n"
             << "  Exception message:"
             << eMsg;
        delete eMsg;
	exit(-2);
    }

  cout << *ffp << endl;

  return(0);
}

// constructor
FFParameters::FFParameters()
{}

// Destructor
FFParameters::~FFParameters()
{
  clear();
  XMLPlatformUtils::Terminate();
}

// initialization
void FFParameters::init(const char* inFile) // takes file name as argument
{


  clear(); // clean up first

  XMLPlatformUtils::Initialize();

  DOMParser *parser = new DOMParser;
  parser->setValidationScheme(DOMParser::Val_Always);
  parser->setDoNamespaces(true);

  parser->parse(inFile);

  DOM_Document doc = parser->getDocument();

  // pull out header information

  DOM_NodeList nList = doc.getElementsByTagName("*");

  int foo = nList.getLength();

  FFParameters_i::processHeader(doc, this);

  // process type_symbol_list to create associative array of masses
  // indexed by element/isotope symbol
  SymbolTypeTable* symTable_p = SymbolTypeTable::create(doc);

  // process ljtype_list to get lj type count
  nList = getElementsInList(doc, "ljtype_list", "ljtype");
  foo = nList.getLength();
  assert(foo > 0);
  d_LJTypeCount = nList.getLength();


  FFParameters_i::processSiteList(doc, this, symTable_p);

  FFParameters_i::processBondList(doc, this);

  FFParameters_i::processAngleList(doc, this);

  FFParameters_i::processTorsionList(doc, this);

  FFParameters_i::processImproperTorsionList(doc, this);

  FFParameters_i::processLJNormalList(doc, this);

  FFParameters_i::processLJ14List(doc, this);
}

// accessors (all access methods do bound-checking and all indexes
// start at 0)

// Header information
const char* FFParameters::forceFieldFamily() const // like AMBER
{
  assert(d_ffFamily != NULL);
  return d_ffFamily;
}
const char* FFParameters::forceFieldSpec() const // like AMBER96
{
  assert(d_ffSpec != NULL);
  return d_ffSpec;
}

int FFParameters::charge14() const
{
  return d_charge14;
}

double FFParameters::charge14Scale() const
{
  return d_charge14Scale;
}

int FFParameters::improperFlag() const
{
  return d_improperFlag;
}

int FFParameters::torsionInputFlag() const
{
  return d_torsionInputFlag;
}

int FFParameters::urey_bradleyFlag() const
{
  return d_ureyBradleyFlag;
}

int FFParameters::groupingFlag() const
{
  return d_groupingFlag;
}

const char* FFParameters::waterModel() const
{
  assert(d_waterModel != NULL);
  return d_waterModel;
}

int FFParameters::unitsFlag() const
{
  return d_unitsFlag;
}

// force field parameters
int FFParameters::siteCount() const
{
  return d_siteCount;
}

const FFParameters::Site& FFParameters::site(int index) const
{
  assert((index >= 0) && (index < d_siteCount));
  return d_site[index];
}


int FFParameters::bondCount() const
{
  return d_bondCount;
}

const FFParameters::Bond& FFParameters::bond(int index) const
{
  assert((index >= 0) && (index < d_bondCount));
  return d_bond[index];
}

int FFParameters::angleCount() const
{
  return d_angleCount;
}

const FFParameters::Angle& FFParameters::angle(int index) const
{
  assert((index >= 0) && (index < d_angleCount));
  return d_angle[index];
}

int FFParameters::torsionType() const
{
  return d_torsionType;
}

int FFParameters::torsionCount() const
{
  return d_torsionCount;
}

const FFParameters::Torsion& FFParameters::torsion(int index) const
{
  assert((index >= 0) && (index < d_torsionCount));
  return d_torsion[index];
}

int FFParameters::improperTorsionType() const
{
  return d_improperTorsionType;
}

int FFParameters::improperTorsionCount() const
{
  return d_improperTorsionCount;
}

const FFParameters::Torsion& FFParameters::improperTorsion(int index) const
{
  assert((index >= 0) && (index < d_improperTorsionCount));
  return d_improperTorsion[index];
}


int FFParameters::LJTypeCount() const
{
  return d_LJTypeCount;
}
  
const FFParameters::LJParam& FFParameters::LJNormal(int i1, int i2) const
{
  assert((i1 >= 0) && (i2 >= 0) &&
	 (i1 < d_LJTypeCount) && (i2 < d_LJTypeCount));
  if (i1 < i2)
    {
      int iLow = i1;
      i1 = i2;
      i2 = iLow;
    }
  assert(i1 >= i2);
  return d_ljNormal[i1][i2];
}

const FFParameters::LJParam& FFParameters::LJ14(int i1, int i2) const
{
  assert((i1 >= 0) && (i2 >= 0) &&
	 (i1 < d_LJTypeCount) && (i2 < d_LJTypeCount));
  if (i1 < i2)
    {
      int iLow = i1;
      i1 = i2;
      i2 = iLow;
    }
  assert(i1 >= i2);
  return d_lj14[i1][i2];
}


void FFParameters::clear()
{
  delete [] d_ffFamily;
  d_ffFamily = NULL;

  delete [] d_ffSpec;
  d_ffSpec = NULL;
  
  d_charge14 = 0;
  d_charge14Scale = 0;
  d_improperFlag = 0;
  d_torsionInputFlag = 0;
  d_ureyBradleyFlag = 0;
  d_groupingFlag = 0;
  
  delete [] d_waterModel;
  d_waterModel = NULL;
  
  d_unitsFlag = 0;

  d_siteCount = 0;
  delete [] d_site;
  d_site = NULL;

  d_bondCount = 0;
  delete [] d_bond;
  d_bond = NULL;

  d_angleCount = 0;
  delete [] d_angle;
  d_angle = NULL;

  d_torsionCount = 0;
  delete [] d_torsion;
  d_torsion = NULL;

  d_improperTorsionCount = 0;
  delete [] d_improperTorsion;
  d_improperTorsion = NULL;

  
  for (int i = 0; i < d_LJTypeCount; i++)
    {
      if (d_ljNormal != NULL)
	{
	  delete [] d_ljNormal[i];
	}

      if (d_lj14 != NULL)
	{
	  delete [] d_lj14[i];
	}
    }

  delete [] d_ljNormal;
  d_ljNormal = NULL;

  delete [] d_lj14;
  d_lj14 = NULL;

  int d_LJTypeCount = 0;
}

ostream& FFParameters::output(ostream& os) const
{
  os << "Header data:\n";
  os << "Force Field Family: " << forceFieldFamily() << " ";
  os << "Force Field Specification: " << forceFieldSpec() << "\n";
  os << "Charge14 Flag: " << charge14() << " ";
  os << "Charge14 Scale: " << charge14Scale() << "\n";
  os << "Improper Flag: " << improperFlag() << "\n";
  os << "Torsion Input Flag: " << torsionInputFlag() << "\n";
  os << "Urey-Bradley Flag: " << urey_bradleyFlag() << "\n";
  os << "Grouping Flag: " << groupingFlag() << "\n";
  os << "Water Model: " << waterModel() << "\n";
  os << "Units Flag: " << unitsFlag() << "\n";

  os << "\n\n Force Field Parameters:\n";
  os << "Site Count: " << siteCount() << "\n";
  int i = 0;
  for (i = 0; i < siteCount(); i++)
    {
      os << "Site[" << i << "]: " << site(i) << "\n";
      
    }
  os << "\n\n";

  os << "Bond Count: " << bondCount() << "\n";
  for (i = 0; i < bondCount(); i++)
    {
      os << "Bond" << bond(i) << "\n";
    }
  os << "\n\n";

  os << "Angle Count: " << angleCount() << "\n";
  for (i = 0; i < angleCount(); i++)
    {
      os << "Angle" << angle(i) << "\n";
    }
  os << "\n\n";

  os << "Torsion Count: " << torsionCount() << "\n";
  for (i = 0; i < torsionCount(); i++)
    {
      os << "Torsion" << torsion(i) << "\n";
    }
  os << "\n\n";

  os << "ImproperTorsion Count: " << improperTorsionCount() << "\n";
  for (i = 0; i < improperTorsionCount(); i++)
    {
      os << "ImproperTorsion" << improperTorsion(i) << "\n";
    }
  os << "\n\n";

  os << "Lennard-Jones Type Count: " << LJTypeCount() << "\n";
  int j = 0;
  for (i = 0; i < LJTypeCount(); i++)
    {
      for (j = 0; j <= i; j++)
	{
	  os << "LJ[" << i << ", " << j << "] " << LJNormal(i, j) << "\n";
	}
    }
  os << "\n\n";

  for (i = 0; i < LJTypeCount(); i++)
    {
      for (j = 0; j <= i; j++)
	{
	  os << "LJ14[" << i << ", " << j << "] " << LJ14(i, j) << "\n";
	}
    }
  return os;
}

void FFParameters_i::processHeader(DOM_Document& doc, FFParameters* ffp_p)
{

  DOM_NodeList nList = doc.getElementsByTagName("header");

  int foo = nList.getLength();
  assert(foo ==  1); // should be only one header in the document

  const DOM_Node headerNode = nList.item(0);
  assert(headerNode != NULL);

  const DOM_Element* headerElt_p = (DOM_Element*)&headerNode;

  char* tagVal = NULL;

  tagVal = getElementValueOfChild(headerElt_p, "ff_family");
  ffp_p->d_ffFamily = tagVal;

  
  tagVal = getElementValueOfChild(headerElt_p, "ff_spec");
  ffp_p->d_ffSpec = tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "charge14");
  ffp_p->d_charge14 = atoi(tagVal);
  delete [] tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "charge14scale");
  ffp_p->d_charge14Scale = atof(tagVal);
  delete [] tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "improper");
  ffp_p->d_improperFlag = atoi(tagVal);
  delete [] tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "torsioninput");
  ffp_p->d_torsionInputFlag = atoi(tagVal);
  delete [] tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "ureybradley");
  ffp_p->d_ureyBradleyFlag = atoi(tagVal);
  delete [] tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "grouping");
  ffp_p->d_groupingFlag = atoi(tagVal);
  delete [] tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "water");
  ffp_p->d_waterModel = tagVal;

  tagVal = getElementValueOfChild(headerElt_p, "units");
  ffp_p->d_unitsFlag = atoi(tagVal);
  delete [] tagVal;
  
}

const SymbolType* SymbolTypeTable::findWithKey(const char* key) const
{
  SymbolType foo;
  foo.d_name_p = (char*)key;
  return((const SymbolType*)bsearch(&foo, d_table_p, d_count,
				    sizeof(SymbolType), SymbolType::compare));
}

SymbolTypeTable* SymbolTypeTable::create(DOM_Document& doc)
{
  SymbolTypeTable* table_p = new SymbolTypeTable;
  assert(table_p != NULL);
  
  DOM_NodeList nList = getElementsInList(doc,
					 "type_symbol_list", 
					 "type_symbol");
  assert(nList.getLength() > 0);
  table_p->d_table_p = new SymbolType[nList.getLength()];
  assert(table_p->d_table_p != NULL);

  table_p->d_count = nList.getLength();

  for (int i = 0; i < nList.getLength(); i++)
    {
      DOM_Node node = nList.item(i);
      DOM_Element* elt_p = (DOM_Element*)&node;
      DOMString name = elt_p->getAttribute("element_symbol");
      table_p->d_table_p[i].d_name_p = name.transcode();
      char* massString = getElementValueOfChild(elt_p, "mass");
      table_p->d_table_p[i].d_mass = atof(massString);
      delete [] massString;
    }

  qsort(table_p->d_table_p,
	table_p->d_count,
	sizeof(SymbolType),
	SymbolType::compare);

  return table_p;
}

int SymbolType::compare(const void* vKey, const void* vElement)
{
  const SymbolType* key = (const SymbolType*)vKey;
  const SymbolType* elt = (const SymbolType*)vElement;

  return strcmp(key->d_name_p, elt->d_name_p);
}


char* getElementValueOfChild(const DOM_Element* elt_p, const char* tag)
{
  DOM_NodeList nList =  elt_p->getElementsByTagName(tag);

  int foo = nList.getLength();
  assert(foo == 1);

  DOMString val = nList.item(0).getFirstChild().getNodeValue();

  return(val.transcode());
}

DOM_NodeList getElementsInList(DOM_Document aDoc,
			       const char* listName,
			       const char* eltName,
			       const char* attribute,
			       const char* attInstance)
{
  DOM_NodeList xList = aDoc.getElementsByTagName(listName);
  int foo = xList.getLength();
  if (attribute == NULL)
    {
      assert(foo == 1);
    }
  
  for (int i = 0; i < xList.getLength(); i++)
    {
      const DOM_Node listNode = xList.item(i);
      assert(listNode != NULL);
      
      const DOM_Element* list_p =
	(DOM_Element*)&listNode;
      
      if (attribute != NULL)
	{
	  DOMString att = list_p->getAttribute(attribute);
	  DOMString instance(attInstance);
	  if (!att.equals(instance))
	    {
	      continue;
	    }
	}
      return(list_p->getElementsByTagName(eltName));
    }
  return(DOM_NodeList());
}

void FFParameters_i::processSiteList(DOM_Document& doc, FFParameters* ffp_p,
				     SymbolTypeTable* table_p)
{
  DOM_NodeList nList = getElementsInList(doc, "site_list", "site");
  int foo = nList.getLength();
  assert(foo > 0);

  ffp_p->d_siteCount = nList.getLength();
  ffp_p->d_site = new FFParametersInterface::Site[nList.getLength()];
  assert(ffp_p->d_site != NULL);

  for (int siteID = 0; siteID < nList.getLength(); siteID++)
    {
      DOM_Node siteNode = nList.item(siteID);
      DOM_Element* siteElt_p = (DOM_Element*)&siteNode;

      int index = getAndStripIntegerAttribute(siteElt_p,
					       "site_id",
					       "site_") - 1;

      assert((index >= 0) && (index < nList.getLength()));

      FFParametersInterface::Site& currentSite = ffp_p->d_site[index];

      DOMString labelTypeSymbol =
	siteElt_p->getAttribute("label_type_symbol");
      assert(labelTypeSymbol != NULL);

      char* typeSymbol = labelTypeSymbol.transcode();
      currentSite.d_mass =
	table_p->findWithKey(typeSymbol)->d_mass;
      delete [] typeSymbol;
      
      currentSite.d_ljType = getAndStripIntegerAttribute(siteElt_p,
						"label_lj_id",
						"lj_") - 1;

      assert((currentSite.d_ljType >= 0) &&
	     (currentSite.d_ljType < ffp_p->d_LJTypeCount));

      currentSite.d_charge =
	convertToDouble(getElementValueOfChild(siteElt_p, "charge"));
    }
}

int getAndStripIntegerAttribute(DOM_Element* elt_p,
				const char* att,
				const char* attBase)
{
  DOMString attributeValue = elt_p->getAttribute(att);
  assert(attributeValue != NULL);

  char* attributeString = attributeValue.transcode();
  int foo = strlen(attBase);
  assert(strncmp(attributeString, attBase, foo) == 0);

  int index = atoi(&attributeString[foo]);

  delete [] attributeString;
  return index;
}

void FFParameters_i::processBondList(DOM_Document& doc, FFParameters* ffp_p)
{
  DOM_NodeList nList = getElementsInList(doc, "bond_list", "bond");
  int foo = nList.getLength();
  assert(foo >= 0);

  ffp_p->d_bondCount = nList.getLength();
  if (ffp_p->d_bondCount == 0)
    {
      return;
    }

  ffp_p->d_bond = new FFParametersInterface::Bond[nList.getLength()];
  assert(ffp_p->d_bond != NULL);
  
  for (int i = 0; i < nList.getLength(); i++)
    {
      DOM_Node bondNode = nList.item(i);
      DOM_Element* bondElt_p = (DOM_Element*)&bondNode;
      
      FFParametersInterface::Bond& currentBond = ffp_p->d_bond[i];
      currentBond.d_site_1 = getAndStripIntegerAttribute(bondElt_p,
							 "site_id1",
							 "site_") - 1;

      assert((currentBond.d_site_1 >= 0) && 
	     (currentBond.d_site_1 < ffp_p->d_siteCount));

      currentBond.d_site_2 = getAndStripIntegerAttribute(bondElt_p,
							 "site_id2",
							 "site_") - 1;
      assert((currentBond.d_site_2 >= 0) && 
	     (currentBond.d_site_2 < ffp_p->d_siteCount));

      currentBond.d_k =
	convertToDouble(getElementValueOfChild(bondElt_p, "k"));

      currentBond.d_r0 =
	convertToDouble(getElementValueOfChild(bondElt_p, "r0"));

    }
}


void FFParameters_i::processAngleList(DOM_Document& doc, FFParameters* ffp_p)
{
  DOM_NodeList nList = getElementsInList(doc, "angle_list", "angle");
  int foo = nList.getLength();
  assert(foo >= 0);

  ffp_p->d_angleCount = nList.getLength();
  if (ffp_p->d_angleCount == 0)
    {
      return;
    }

  ffp_p->d_angle = new FFParametersInterface::Angle[nList.getLength()];
  assert(ffp_p->d_angle != NULL);
  
  for (int i = 0; i < nList.getLength(); i++)
    {
      DOM_Node angleNode = nList.item(i);
      DOM_Element* angleElt_p = (DOM_Element*)&angleNode;
      
      FFParametersInterface::Angle& currentAngle = ffp_p->d_angle[i];
      currentAngle.d_site_1 = getAndStripIntegerAttribute(angleElt_p,
							 "site_id1",
							 "site_") - 1;

      assert((currentAngle.d_site_1 >= 0) && 
	     (currentAngle.d_site_1 < ffp_p->d_siteCount));

      currentAngle.d_site_2 = getAndStripIntegerAttribute(angleElt_p,
							 "site_id2",
							 "site_") - 1;
      assert((currentAngle.d_site_2 >= 0) && 
	     (currentAngle.d_site_2 < ffp_p->d_siteCount));

      currentAngle.d_site_3 = getAndStripIntegerAttribute(angleElt_p,
							 "site_id3",
							 "site_") - 1;
      assert((currentAngle.d_site_3 >= 0) && 
	     (currentAngle.d_site_3 < ffp_p->d_siteCount));

      currentAngle.d_k =
	convertToDouble(getElementValueOfChild(angleElt_p, "k"));

      currentAngle.d_th0 =
	convertToDouble(getElementValueOfChild(angleElt_p, "th0"));

      currentAngle.d_ku =
	convertToDouble(getElementValueOfChild(angleElt_p, "ku"));

      currentAngle.d_ru =
	convertToDouble(getElementValueOfChild(angleElt_p, "ru"));

    }
}

void FFParameters_i::processTorsionList(DOM_Document& doc,
					FFParameters* ffp_p)
{
  DOM_NodeList nList = getElementsInList(doc,
					 "torsion_list",
					 "torsion",
					 "torsion_class",
					 "proper");
  int foo = nList.getLength();
  assert(foo >= 0);

  ffp_p->d_torsionCount = nList.getLength();
  if (ffp_p->d_torsionCount == 0)
    {
      return;
    }

  ffp_p->d_torsion = new FFParametersInterface::Torsion[nList.getLength()];
  assert(ffp_p->d_torsion != NULL);

  DOM_NodeList xList = doc.getElementsByTagName("torsion_list");

  for (int itor = 0; itor < xList.getLength(); itor++)
    {
      DOM_Node node = xList.item(itor);
      DOM_Element* elt_p = (DOM_Element*)&node;
      DOMString torClass = elt_p->getAttribute("torsion_class");
      if (torClass.equals("proper"))
      {
	DOMString torType = elt_p->getAttribute("torsion_type");
	if (torType.equals("charmm"))
	  {
	    ffp_p->d_torsionType = FFParametersInterface::CHARMM_TYPE;
	  }
	else if (torType.equals("oplsaa"))
	  {
	    ffp_p->d_torsionType = FFParametersInterface::OPLSAA_TYPE;
	  }
	else
	  {
	    assert(0);
	  }
	break;
      }
    }

  for (int i = 0; i < nList.getLength(); i++)
    {
      DOM_Node torsionNode = nList.item(i);
      DOM_Element* torsionElt_p = (DOM_Element*)&torsionNode;
      
      FFParametersInterface::Torsion& currentTorsion = ffp_p->d_torsion[i];
      currentTorsion.d_site_1 = getAndStripIntegerAttribute(torsionElt_p,
							 "site_id1",
							 "site_") - 1;

      assert((currentTorsion.d_site_1 >= 0) && 
	     (currentTorsion.d_site_1 < ffp_p->d_siteCount));

      currentTorsion.d_site_2 = getAndStripIntegerAttribute(torsionElt_p,
							 "site_id2",
							 "site_") - 1;
      assert((currentTorsion.d_site_2 >= 0) && 
	     (currentTorsion.d_site_2 < ffp_p->d_siteCount));

      currentTorsion.d_site_3 = getAndStripIntegerAttribute(torsionElt_p,
							 "site_id3",
							 "site_") - 1;
      assert((currentTorsion.d_site_3 >= 0) && 
	     (currentTorsion.d_site_3 < ffp_p->d_siteCount));

      currentTorsion.d_site_4 = getAndStripIntegerAttribute(torsionElt_p,
							 "site_id4",
							 "site_") - 1;
      assert((currentTorsion.d_site_4 >= 0) && 
	     (currentTorsion.d_site_4 < ffp_p->d_siteCount));

      currentTorsion.d_v =
	convertToDouble(getElementValueOfChild(torsionElt_p, "k"));

      currentTorsion.d_th0 =
	convertToDouble(getElementValueOfChild(torsionElt_p, "th0"));

      currentTorsion.d_order =
	convertToInt(getElementValueOfChild(torsionElt_p, "mult"));

    }
}

void FFParameters_i::processImproperTorsionList(DOM_Document& doc,
						FFParameters* ffp_p)
{
  DOM_NodeList nList = getElementsInList(doc,
					 "torsion_list",
					 "torsion",
					 "torsion_class",
					 "improper");

  int foo = nList.getLength();
  assert(foo >= 0);

  ffp_p->d_improperTorsionCount = nList.getLength();
  if (ffp_p->d_improperTorsionCount == 0)
    {
      return;
    }

  ffp_p->d_improperTorsion = new FFParametersInterface::Torsion[nList.getLength()];
  assert(ffp_p->d_improperTorsion != NULL);

  DOM_NodeList xList = doc.getElementsByTagName("torsion_list");

  for (int itor = 0; itor < xList.getLength(); itor++)
    {
      DOM_Node node = xList.item(itor);
      DOM_Element* elt_p = (DOM_Element*)&node;
      DOMString torClass = elt_p->getAttribute("torsion_class");
      if (torClass.equals("improper"))
      {
	DOMString torType = elt_p->getAttribute("torsion_type");
	if (torType.equals("charmm"))
	  {
	    ffp_p->d_improperTorsionType = FFParametersInterface::CHARMM_TYPE;
	  }
	else if (torType.equals("oplsaa"))
	  {
	    ffp_p->d_improperTorsionType = FFParametersInterface::OPLSAA_TYPE;
	  }
	else
	  {
	    assert(0);
	  }
	break;
      }
    }
  
  for (int i = 0; i < nList.getLength(); i++)
    {
      DOM_Node improperTorsionNode = nList.item(i);
      DOM_Element* improperTorsionElt_p = (DOM_Element*)&improperTorsionNode;
      
      FFParametersInterface::Torsion& currentImproperTorsion =
	ffp_p->d_improperTorsion[i];
      currentImproperTorsion.d_site_1 =
	getAndStripIntegerAttribute(improperTorsionElt_p, 
				    "site_id1",
				    "site_") - 1;

      assert((currentImproperTorsion.d_site_1 >= 0) && 
	     (currentImproperTorsion.d_site_1 < ffp_p->d_siteCount));

      currentImproperTorsion.d_site_2 =
	getAndStripIntegerAttribute(improperTorsionElt_p,
				    "site_id2",
				    "site_") - 1;
      assert((currentImproperTorsion.d_site_2 >= 0) && 
	     (currentImproperTorsion.d_site_2 < ffp_p->d_siteCount));

      currentImproperTorsion.d_site_3 =
	getAndStripIntegerAttribute(improperTorsionElt_p,
				    "site_id3",
				    "site_") - 1;
      assert((currentImproperTorsion.d_site_3 >= 0) && 
	     (currentImproperTorsion.d_site_3 < ffp_p->d_siteCount));

      currentImproperTorsion.d_site_4 =
	getAndStripIntegerAttribute(improperTorsionElt_p,
				    "site_id4",
				    "site_") - 1;
      assert((currentImproperTorsion.d_site_4 >= 0) && 
	     (currentImproperTorsion.d_site_4 < ffp_p->d_siteCount));

      currentImproperTorsion.d_v =
	convertToDouble(getElementValueOfChild(improperTorsionElt_p,
					       "k"));

      currentImproperTorsion.d_th0 =
	convertToDouble(getElementValueOfChild(improperTorsionElt_p,
					       "th0"));

      currentImproperTorsion.d_order =
	convertToInt(getElementValueOfChild(improperTorsionElt_p,
					    "mult"));

    }
}

void FFParameters_i::processLJNormalList(DOM_Document& doc,
					 FFParameters* ffp_p)
{
  if (ffp_p->d_LJTypeCount == 0)
    {
      return;
    }


  // allocate memory for triangular matrix
  ffp_p->d_ljNormal = new
    LJParamPointer[ffp_p->d_LJTypeCount];
  assert(ffp_p->d_ljNormal != NULL);
  int i = 0;
  for (i = 0; i < ffp_p->d_LJTypeCount; i++)
    {
      ffp_p->d_ljNormal[i] = new FFParametersInterface::LJParam[i+1];
      assert(ffp_p->d_ljNormal[i] != NULL);
    }

  DOM_NodeList nList = getElementsInList(doc,
					 "ljpair_param_list",
					 "ljpair_param",
					 "lj_class",
					 "normal");

  int foo = nList.getLength();
  assert(foo == (ffp_p->d_LJTypeCount*(ffp_p->d_LJTypeCount + 1))/2);

  int index = 0;
  for (i = 0; i < nList.getLength(); i++)
    {
      DOM_Node ljParamNode = nList.item(i);
      DOM_Element* ljParamElt_p = (DOM_Element*)&ljParamNode;

      int i1 = getAndStripIntegerAttribute(ljParamElt_p, 
					   "id_1",
					   "lj_") - 1;

      int i2 = getAndStripIntegerAttribute(ljParamElt_p, 
					   "id_2",
					   "lj_") - 1;

      assert((i1 >= 0) && 
	     (i1 < ffp_p->d_LJTypeCount));
      assert((i2 >= 0) && 
	     (i2 < ffp_p->d_LJTypeCount));

      if (i1 < i2)
	{
	  int iLow = i1;
	  i1 = i2;
	  i2 = iLow;
	}
      assert(i1 >= i2);

      FFParametersInterface::LJParam& currentljNormal =
	    ffp_p->d_ljNormal[i1][i2];


      currentljNormal.d_sigma =
	convertToDouble(getElementValueOfChild(ljParamElt_p,
					       "sigma"));
      currentljNormal.d_epsilon =
	convertToDouble(getElementValueOfChild(ljParamElt_p,
					       "epsilon"));
    }

}

void FFParameters_i::processLJ14List(DOM_Document& doc,
				     FFParameters* ffp_p)
{
  if (ffp_p->d_LJTypeCount == 0)
    {
      return;
    }

  // allocate memory for triangular matrix
  ffp_p->d_lj14 = new
    LJParamPointer[ffp_p->d_LJTypeCount];
  assert(ffp_p->d_lj14 != NULL);
  int i = 0;
  for (i = 0; i < ffp_p->d_LJTypeCount; i++)
    {
      ffp_p->d_lj14[i] = new FFParametersInterface::LJParam[i+1];
      assert(ffp_p->d_lj14[i] != NULL);
    }

  DOM_NodeList nList = getElementsInList(doc,
					 "ljpair_param_list",
					 "ljpair_param",
					 "lj_class",
					 "lj14");

  int foo = nList.getLength();
  assert(foo == (ffp_p->d_LJTypeCount*(ffp_p->d_LJTypeCount + 1))/2);

  int index = 0;
  for (i = 0; i < nList.getLength(); i++)
    {
      DOM_Node ljParamNode = nList.item(i);
      DOM_Element* ljParamElt_p = (DOM_Element*)&ljParamNode;

      int i1 = getAndStripIntegerAttribute(ljParamElt_p, 
					   "id_1",
					   "lj_") - 1;

      int i2 = getAndStripIntegerAttribute(ljParamElt_p, 
					   "id_2",
					   "lj_") - 1;

      assert((i1 >= 0) && 
	     (i1 < ffp_p->d_LJTypeCount));
      assert((i2 >= 0) && 
	     (i2 < ffp_p->d_LJTypeCount));

      if (i1 < i2)
	{
	  int iLow = i1;
	  i1 = i2;
	  i2 = iLow;
	}
      assert(i1 >= i2);

      FFParametersInterface::LJParam& currentlj14 =
	    ffp_p->d_lj14[i1][i2];


      currentlj14.d_sigma =
	convertToDouble(getElementValueOfChild(ljParamElt_p,
					       "sigma"));
      currentlj14.d_epsilon =
	convertToDouble(getElementValueOfChild(ljParamElt_p,
					       "epsilon"));
    }
}
