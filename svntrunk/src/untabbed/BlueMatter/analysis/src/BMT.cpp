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
 #include <assert.h>
// #include <cmath>
#include <math.h>
#include <fcntl.h>
#include <cstdio>
#include <string>
#ifdef WIN32
#include <io.h>
#define SWAP_IN
#define SWAP_OUT
#endif
#include <iostream>
#include <ostream>
#include <fstream>

#include <BlueMatter/Swap.hpp>
#include <BlueMatter/BMT.hpp>
// for IncludeValue
#include <BlueMatter/DataReceiverSimpleLogger.hpp>

   
// class BMT methods

BMT::BMT() {
  bindKnownAttributes();
  locale loc(locale(), new mynumpunct) ;
  cout.imbue(loc);
  m_OutStream.imbue(loc);
  m_DataSeekStart = 0;
}


void BMT::addFragments()
{
  char p[1024];
  ConnectivityString s_p;
  getAttrib("CONNECTIVITY",&s_p);
  strcpy(p,s_p.getConnectivityString());
  // AK Change: m_NAtoms belongs to molecular system properties
  if (strlen(p) == 0) {
      Fragment tempFrag(0,m_MolecularSystem.m_NAtoms);
      m_MolecularSystem.m_FragmentList.AddFragment(tempFrag);
      //m_MolecularSystem.m_FragmentList.AddFragment(new Fragment(0, m_MolecularSystem.m_NAtoms));
  }
  else {
      char *token = strtok(p, "*");
      int index = 0;
      while (token) {
    int numrep  = atoi(token);
    token = strtok(NULL, "+");
    int numatoms = atoi(token);
    token = strtok(NULL, "*");
    for (int i=0; i<numrep; i++) {
            Fragment tempFrag(index,numatoms);
        m_MolecularSystem.m_FragmentList.AddFragment(tempFrag);
        //m_MolecularSystem.m_FragmentList.AddFragment(Fragment(index, numatoms));
        index += numatoms;
    }
      }
  }
}

// This finds the true bounding box of the Pos, for use in data compression
inline void BMT::findBox(XYZ *Pos, const int Start, const int Count, XYZ &origin, XYZ &span, double &MaxSpan)
{
  XYZ LL, UR;
  XYZ *ps = &Pos[Start];
  
  LL.mX = LL.mY = LL.mZ = 1.0E20;
  UR.mX = UR.mY = UR.mZ = -1.0E20;
  
  for (int i=Start; i<Start+Count; i++) {
      XYZ s = *ps;
      if (s.mX < LL.mX) LL.mX = s.mX;
      if (s.mX > UR.mX) UR.mX = s.mX;
      if (s.mY < LL.mY) LL.mY = s.mY;
      if (s.mY > UR.mY) UR.mY = s.mY;
      if (s.mZ < LL.mZ) LL.mZ = s.mZ;
      if (s.mZ > UR.mZ) UR.mZ = s.mZ;
      ps++;
  }
  origin = LL;
  span = UR - LL;
  MaxSpan = span.mX;
  if (span.mY > MaxSpan) MaxSpan = span.mY;
  if (span.mZ > MaxSpan) MaxSpan = span.mZ;
}
    
// This simplfies the sites from double to usXYZ, using the bounding box
inline void BMT::simplifySites(XYZ *Pos, usXYZ *psp, const int Start, const int Count, const XYZ &origin, const double factor)
{
  XYZ *ps = &Pos[Start];
  
  for (int i=Start; i<Start+Count; i++) {
      XYZ p = *ps;
      p -= origin;
      psp->mX = p.mX*factor;
      psp->mY = p.mY*factor;
      psp->mZ = p.mZ*factor;
      ps++;
      psp++;
  }
}

// AK : New methods to get header and frame attributes

bool BMT::hasAttrib(string name,NameTypeHandle::BMTATTRIBUTETYPE *typAttr) {

        if(m_CharAttrib.has(name)) {
                *typAttr=NameTypeHandle::CHAR;
                return true;
        }
        if(m_IntAttrib.has(name)) {
                *typAttr=NameTypeHandle::INT;
                return true;
        }
        if(m_UIntAttrib.has(name)) {
                *typAttr=NameTypeHandle::UINT;
                return true;
        }
        if(m_FloatAttrib.has(name)) {
                *typAttr=NameTypeHandle::FLOAT;
                return true;
        }
        if(m_DoubleAttrib.has(name)) {
                *typAttr=NameTypeHandle::DOUBLE;
                return true;
        }
        if(m_StringAttrib.has(name)) {
                *typAttr=NameTypeHandle::STRING;
                return true;
        }
        if(m_BoolAttrib.has(name)) {
                *typAttr=NameTypeHandle::BOOL;
                return true;
        }
        if(m_BoundingBoxAttrib.has(name)) {
                *typAttr=NameTypeHandle::BOUNDINGBOX;
                return true;
        }
        if(m_ConnectivityStringAttrib.has(name)) {
                *typAttr=NameTypeHandle::CONNECTIVITYSTRING;
                return true;
        }
        *typAttr=NameTypeHandle::INVALID;
        return false;
}

// print attribute function
void BMT::printAttrib(NameTypeHandle::BMTATTRIBUTETYPE typAttr) {

  bool typAll = (typAttr==NameTypeHandle::ALL);

        switch(typAttr) {
    case NameTypeHandle::ALL:  // fall through
                case NameTypeHandle::CHAR: m_CharAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::INT: m_IntAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::UINT: m_UIntAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::FLOAT: m_FloatAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::DOUBLE: m_DoubleAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::STRING: m_StringAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::BOOL: m_BoolAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::BOUNDINGBOX: m_BoundingBoxAttrib.print(); 
        if(!typAll) return;
                case NameTypeHandle::CONNECTIVITYSTRING: m_ConnectivityStringAttrib.print(); 
        if(!typAll) return;
        }
}

// clear attribute function
void BMT::clearAttrib(NameTypeHandle::BMTATTRIBUTETYPE typAttr) {
  bool typAll = (typAttr==NameTypeHandle::ALL);
        switch(typAttr) {
                case NameTypeHandle::ALL: //fall through
                case NameTypeHandle::CHAR: m_CharAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::INT: m_IntAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::UINT: m_UIntAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::FLOAT: m_FloatAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::DOUBLE: m_DoubleAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::STRING: m_StringAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::BOOL: m_BoolAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::BOUNDINGBOX: m_BoundingBoxAttrib.clear(); 
        if(!typAll) return;
                case NameTypeHandle::CONNECTIVITYSTRING: m_ConnectivityStringAttrib.clear(); 
        if(!typAll) return;
        }
}

void BMT::bindKnownAttributes() {

        // integer attributes
        m_IntAttrib.bind("NUMNONWATERATOMS",&(m_MolecularSystem.m_NNonWaterAtoms));
        m_IntAttrib.bind("NUMWATERS",&(m_MolecularSystem.m_NWaters));
        m_IntAttrib.bind("NOWATERS",&m_NoWaters);
        m_IntAttrib.bind("STARTATOMINDEX",&m_StartAtomIndex);
        m_IntAttrib.bind("ENDATOMINDEX",&m_EndAtomIndex);
        m_IntAttrib.bind("INITIALSTEP",&(m_TrajectoryProperties.m_InitialStep));
        m_IntAttrib.bind("SIMULATIONSTARTSTEP",&(m_TrajectoryProperties.m_SimulationStartStep));
        m_IntAttrib.bind("BMTSKIPFACTOR",&(m_ConversionProperties.m_SkipFactor));
        m_IntAttrib.bind("FRAMESIZE",&m_FrameSize);
        m_IntAttrib.bind("SEEKSTART",&m_DataSeekStart);

        // unsigned int attributes
        m_UIntAttrib.bind("EXECUTABLEID",&(m_RDGProperties.m_ExecutableID));
        m_UIntAttrib.bind("SNAPSHOTPERIODINOTS",&(m_TrajectoryProperties.m_SnapshotPeriodInOTS));
        m_UIntAttrib.bind("SOURCEID",&(m_RDGProperties.m_SourceID));
        

        // double attributes
        m_DoubleAttrib.bind("OUTERTIMESTEPINPICOSECONDS",&(m_TrajectoryProperties.m_OuterTimeStepInPicoSeconds));
        
        // string attributes
        m_StringAttrib.bind("VERSION",&m_Version);
        // m_StringAttrib.bind("STRUCTURE",&m_MolecularSystem.m_PDBFileName);
        m_ConnectivityStringAttrib.bind("CONNECTIVITY",&m_MolecularSystem.m_ConnectivityString);
        m_StringAttrib.bind("BYTEORDER",&m_ByteOrder);
        m_StringAttrib.bind("TRAJECTORYFORMAT",&m_ConversionProperties.m_TRAJECTORYFORMATstring);
        
        m_IntAttrib.bind("REVERSE",&(m_TrajectoryProperties.m_Reverse));
      
        // BoundingBox attributes
        m_BoundingBoxAttrib.bind("INITIALBOX",&(m_TrajectoryProperties.m_InitialBox));
}


// class BMTReader methods

BMTReader::BMTReader(char *fname)
{
  strcpy(m_FileName, fname);
  m_pPos = NULL;
  m_pVel = NULL;
  m_pShortPos = NULL;

  m_pPosArray = NULL;
  m_pVelArray = NULL;

  // AK init will be called during construction
  //m_TrajectoryProperties.init();
  m_FrameSize = 0;
  m_DataSeekStart = 0;
  m_NEntries = 0;
  ReadHeader();
}

BMTReader::~BMTReader()
{
    //m_pPos = new XYZ[m_NEntries];
  //m_pVel = new XYZ[m_NEntries];
    //m_pShortPos = new usXYZ[m_NEntries];
  delete [] m_pPos;
  delete [] m_pVel;
  delete [] m_pShortPos;
}

int BMTReader::ReadHeader()
{
  struct stat results;
  if (stat(m_FileName, &results) == 0) {
      m_FileSize = results.st_size;
      // cerr << "File Size " << m_FileSize << endl;
  } else {
      cerr << "Error determining size of file " << m_FileName << endl;
      exit(-1);
  }
      
  m_File = fopen(m_FileName, "rb");
  if (!m_File) {
      cerr << "Error reading header of bmt file " << m_FileName << endl;
      exit(-1);
  }
  fgets(m_Buff, BUFFSIZE, m_File);
  assert(!strncmp(m_Buff, "BMT", 3));
  fgets(m_Buff, BUFFSIZE, m_File);

  char sAttr[ATTRIBUTEVALUESIZE];
  char sVal[ATTRIBUTEVALUESIZE];
  // AK Change: First read the version number and then move on
  //	Make the default version as 1.00
  if(sscanf(m_Buff, "VERSION %s\n", sVal)!=1) strcpy(sVal,"1.00");
  //m_Version=sVal;
  setAttrib("VERSION",(string) sVal);
  
  // Set the begin trajectory delimiter based on version
  char sBeginTraj[100];
  if(strcmp(m_Version.c_str(),"1.00")==0) strcpy(sBeginTraj,BEGINTRAJ_VER1);
  else strcpy(sBeginTraj,BEGINTRAJ_VER2);

  // AK: This can be true only for version 2 and above 
  bool bFrameAttributes=false;

  int tempint;
  int iVal;
  double dVal;
  unsigned int uiVal;
  BoundingBox bbVal;
  

  while (1) {
      fgets(m_Buff, BUFFSIZE, m_File);
      if(!strncmp(m_Buff, sBeginTraj, strlen(sBeginTraj)) ) break;

      // AK : See if line is commented (ie., # is the first character) 
      if(0==strncmp(m_Buff,BMTCOMMENTLINE,1)) {
    continue;
      }

      if(strstr(m_Buff,BEGINFRAMEFORMAT)) {
    bFrameAttributes=true;
    continue;
      }

      if(bFrameAttributes) {
    // The format of the line is:	ATTRNAME ATTRTYPE
        if(2 == sscanf(m_Buff, "%s %s\n", sAttr,sVal)) {
      setupNewFrameAttrib(sAttr,sVal);
    }
    continue;
      }

      //if(1 == sscanf(m_Buff, "STRUCTURE %s\n", sVal)) 
    //	setAttrib("STRUCTURE",(string) sVal);
      else if (1 == sscanf(m_Buff, "NUMNONWATERATOMS %d\n", &iVal)) 
      setAttrib("NUMNONWATERATOMS",iVal);
      else if (1 == sscanf(m_Buff, "NUMWATERS %d\n", &iVal))
      setAttrib("NUMWATERS",iVal);
      else if (1 == sscanf(m_Buff, "NOWATERS %d\n", &iVal)) { 
      setAttrib("NOWATERS",iVal);
      m_ConversionProperties.m_IncludeWaters = (m_NoWaters== 0); 
    }
      // STARTINDEX and STARTATOMINDEX denote the same attributes
      // similarly, ENDINDEX and ENDATOMINDEX denote the same attributes
      else if (1 == sscanf(m_Buff, "STARTINDEX %d",&iVal)) 
      setAttrib("STARTATOMINDEX",iVal);
      else if (1 == sscanf(m_Buff, "ENDINDEX %d",&iVal)) {
      setAttrib("ENDATOMINDEX",iVal);
      m_MolecularSystem.m_NAtoms=m_EndAtomIndex+1; 
    }
      else if (1 == sscanf(m_Buff, "STARTATOMINDEX %d",&iVal))
      setAttrib("STARTATOMINDEX",iVal);
      else if (1 == sscanf(m_Buff, "ENDATOMINDEX %d",&iVal)) {
      setAttrib("ENDATOMINDEX",iVal);
      m_MolecularSystem.m_NAtoms=m_EndAtomIndex+1; 
    }
      else if (1 == sscanf(m_Buff, "REVERSE %d\n", &iVal)) 
      setAttrib("REVERSE",iVal);
      else if (1 == sscanf(m_Buff, "INITIALSTEP %d\n", &iVal)) 
      setAttrib("INITIALSTEP",iVal);
      else if (1 == sscanf(m_Buff, "SIMULATIONSTARTSTEP %d\n", &iVal)) 
      setAttrib("SIMULATIONSTARTSTEP",iVal);
      else if (1 == sscanf(m_Buff, "SNAPSHOTPERIODINOTS %u\n", &uiVal)) 
      setAttrib("SNAPSHOTPERIODINOTS",uiVal);
      else if (1 == sscanf(m_Buff, "OUTERTIMESTEPINPICOSECONDS %lf\n", &dVal))
      setAttrib("OUTERTIMESTEPINPICOSECONDS",dVal);
      else if (1 == sscanf(m_Buff, "BMTSKIPFACTOR %d\n", &iVal)) 
      setAttrib("BMTSKIPFACTOR",iVal);
      else if (1 == sscanf(m_Buff, "SOURCEID %u",&uiVal))
      setAttrib("SOURCEID",uiVal);
      else if (1 == sscanf(m_Buff, "EXECUTABLEID %u",&uiVal))
      setAttrib("EXECUTABLEID",uiVal);
      else if (6 == sscanf(m_Buff, "INITIALBOX [ %lf %lf %lf ] [ %lf %lf %lf ]\n", &bbVal.mMinBox.mX, &bbVal.mMinBox.mY, &bbVal.mMinBox.mZ, &bbVal.mMaxBox.mX, &bbVal.mMaxBox.mY, &bbVal.mMaxBox.mZ)) 
      setAttrib("INITIALBOX",bbVal);
      else if (1 == sscanf(m_Buff, "FRAMESIZE %d\n",&iVal))
      setAttrib("FRAMESIZE",iVal);
      // m_DataSeekStart will have to be recomputed here
      else if (1 == sscanf(m_Buff, "SEEKSTART %d\n",&iVal))
      setAttrib("SEEKSTART",iVal);
      else if (1 == sscanf(m_Buff, "BYTEORDER %s\n",sVal)) 
      setAttrib("BYTEORDER",(string) sVal);
      else if (1 == sscanf(m_Buff, "CONNECTIVITY %s\n",sVal))  {
      ConnectivityString cs(sVal);
      setAttrib("CONNECTIVITY",cs);
      }
      else if (1 == sscanf(m_Buff, "TRAJECTORYFORMAT %[^\n]\n",sVal)) 
      setAttrib("TRAJECTORYFORMAT",(string) sVal);
      else 
    // this is the case the user provides a unknown/unclassified attribute 
    // so just take it and store it as-is (as a string attribute)
      if (2 == sscanf(m_Buff, "%s %[^\n]\n",sAttr,sVal)) {
    string sStrVal = sVal;
    char sName[ATTRIBUTEVALUESIZE];
    sprintf(sName,"%s",sAttr);
    setAttrib(sName,sStrVal);	
      }
     
  } // end of while loop

  if(m_Version=="1.00") {
      if (1 == sscanf(m_Buff, "TRAJECTORYFORMAT %[^\n]\n",sVal)) 
      setAttrib("TRAJECTORYFORMAT",(string) sVal);
  }

  m_ConversionProperties.determineReductionType();

  m_ConversionProperties.determineIncludeVelocities();

  m_NetSnapshotPeriodInOTS = m_ConversionProperties.m_SkipFactor*m_TrajectoryProperties.m_SnapshotPeriodInOTS;

  double ReverseFactor = 1.0;
  if (m_TrajectoryProperties.m_Reverse)
      ReverseFactor = -1.0;
  m_NetTimeStepInPicoSeconds = ReverseFactor*m_NetSnapshotPeriodInOTS*m_TrajectoryProperties.m_OuterTimeStepInPicoSeconds;

  if (m_NoWaters) m_MolecularSystem.m_NWaters = 0;

  m_MolecularSystem.m_NAtoms = m_MolecularSystem.m_NNonWaterAtoms + 3 * m_MolecularSystem.m_NWaters;
  
  m_MolecularSystem.m_NWaterAtoms = 3 * m_MolecularSystem.m_NWaters;

  m_TrajectoryProperties.m_InitialSimulationTimeInNanoSeconds = (m_TrajectoryProperties.m_InitialStep-m_TrajectoryProperties.m_SimulationStartStep)*m_TrajectoryProperties.m_OuterTimeStepInPicoSeconds/1000.0;

  setAttrib("SEEKSTART",(int) ftell(m_File));
  m_DataSeekStart = ftell(m_File);

  m_FrameSize = determineFrameSize();

  m_NFrames = (m_FileSize-m_DataSeekStart+1)/m_FrameSize;

  // cerr << "FileSize " << m_FileSize << " FrameSize " << m_FrameSize << endl;
  // cerr << "NFrames " << m_NFrames << " seekstart " << m_DataSeekStart << endl;

  m_NEntries = determineNEntries();

  m_pPos = new XYZ[m_NEntries];
  m_pVel = new XYZ[m_NEntries];
        m_pShortPos = new usXYZ[m_NEntries];

  return 0;
}

int BMTReader::determineNEntries() {

  if(IsCOMTrajectory() ) {
    ConnectivityString cs;
    getAttrib("CONNECTIVITY",&cs);
    return cs.getNumberOfMolecules();
  }
  return m_MolecularSystem.m_NAtoms; // default
}


void *BMT::setupNewFrameAttrib(string sAttrName,string sTypeName) {
  return setupNewFrameAttrib(sAttrName.c_str(),sTypeName.c_str());
}

void *BMT::setupNewFrameAttrib(char *sAttrName,char *sTypeName) {
  string sName = sAttrName;
  void *handle=NULL;

  if(!strcasecmp(sTypeName,"DOUBLE")) {
    handle = m_DoubleFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::DOUBLE,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  if(!strcasecmp(sTypeName,"CHAR")) {
    handle = m_CharFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::CHAR,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  if(!strcasecmp(sTypeName,"INT")) {
    handle = m_IntFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::INT,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  if(!strcasecmp(sTypeName,"UINT")) {
    handle = m_UIntFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::UINT,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  if(!strcasecmp(sTypeName,"FLOAT")) {
    handle = m_FloatFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::FLOAT,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  if(!strcasecmp(sTypeName,"BOOL")) {
    handle = m_BoolFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::BOOL,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  if(!strcasecmp(sTypeName,"BOUNDINGBOX")) {
    handle = m_BoundingBoxFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::BOUNDINGBOX,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  if(!strcasecmp(sTypeName,"TYPEXYZ")) {
    handle = m_XYZFrameAttrib.createHandle(sName);
    NameTypeHandle nt(sName,NameTypeHandle::TYPEXYZ,handle);
    m_VecFrameAttributes.push_back(nt);
    return handle;
  }
  cerr <<"Attempt to set undefined frame attribute " << sAttrName << " " << sTypeName << endl;
  assert(0);
  // default : don't create anything for unsupported types
  return NULL;
}
    
// read next frame of positions and do appropriate decoding and expansion
int BMTReader::ReadFrameShort()
{
  // read the box used for compression

  float x0, y0, z0, s;
       
  if (SwapIO::Read(m_File, &x0))
      return 1;
  if (SwapIO::Read(m_File, &y0))
      return 1;
  if (SwapIO::Read(m_File, &z0))
      return 1;
  if (SwapIO::Read(m_File, &s))
      return 1;

  m_Origin.mX = x0;
  m_Origin.mY = y0;
  m_Origin.mZ = z0;
  m_RecipFactor = s;
  
        int rc = LoadXYZBox(m_pPos);

  if(rc==1) return 1;

  if(!getIncludeVelocities()) return 0;
  
  unsigned short sx, sy, sz;
  for (int i=0; i<m_NEntries; i++) {
      if (SwapIO::Read(m_File, &sx))
    return 1;
      if (SwapIO::Read(m_File, &sy))
    return 1;
      if (SwapIO::Read(m_File, &sz))
    return 1;
      m_pVel[i].mX = sx;
      m_pVel[i].mY = sy;
      m_pVel[i].mZ = sz;
        }
   
  return 0;
}

int BMTReader::LoadXYZBox(XYZ *p) {
  // expand the sites using compression
  unsigned short sx, sy, sz;
  
  for (int i=0; i<m_NEntries; i++) {
      if (SwapIO::Read(m_File, &sx))
    return 1;
      if (SwapIO::Read(m_File, &sy))
    return 1;
      if (SwapIO::Read(m_File, &sz))
    return 1;
      
      p[i].mX = m_Origin.mX + sx * m_RecipFactor;
      p[i].mY = m_Origin.mY + sy * m_RecipFactor;
      p[i].mZ = m_Origin.mZ + sz * m_RecipFactor;
  }
  return 0;
}


// read next frame of positions and do appropriate decoding and expansion
int BMTReader::ReadFrameReal(ConversionProperties::REDUCTIONTYPE rt)
{
  XYZ *pp = m_pPos;

  LoadXYZ(pp, rt); 

  if(getIncludeVelocities())  {
    XYZ *pv = m_pVel;
    LoadXYZ(pv, rt); 
      }
  return 0;
}

int BMTReader::LoadXYZ(XYZ *p, ConversionProperties::REDUCTIONTYPE rt) {
  
  double dv;
  float fv;

  for (int i=0; i<m_NEntries; i++, p++) {
      for (int j=0; j<3; j++) {
    dv = (*p)[j];
    fv = (float)dv;
    if (rt == ConversionProperties::FLOAT) {
        if (SwapIO::Read(m_File, &fv))
      return 1;
        (*p)[j] = fv;
    } else {
        if (SwapIO::Read(m_File, &dv))
      return 1;
        (*p)[j] = dv;
    }
      }
  }
  return 0;
}

// AK: readAttributeFromFrame:
//	This function reads a frame attribute from the input BMT file
//	at the start of each frame and updates the corresponding store 
// 	Comment: Only fixed size data types can be supported
// 		for eg., string data type is not supported

int BMTReader::readAttributeFromFrame(NameTypeHandle nt) {
  char c,*pc;
  int i,*pi;
  unsigned int ui,*pui;
  float f,*pf;
  double d,*pd;
  bool b,*pb;
  BoundingBox bb,*pbb;
  XYZ xyz,*pxyz;
  
  // first get the type and read the corresponding data from the BMT file
  // then get a handle to the attribute in the corresponding map store
  // then update the contents with the newly read value 

  switch(nt.type) {
    case NameTypeHandle::DOUBLE:	
        if(SwapIO::Read(m_File,&d)) return 1; // error
        pd = (double *)nt.ptr;
        *pd = d;
        break;
    case NameTypeHandle::CHAR:	
        if(SwapIO::Read(m_File,&c)) return 1; // error
        pc = (char *)nt.ptr;
        *pc = c;
        break;
    case NameTypeHandle::UINT:	
        if(SwapIO::Read(m_File,&ui)) return 1; // error
        pui = (unsigned int *)nt.ptr;
        *pui = ui;
        break;
    case NameTypeHandle::INT:	
        if(SwapIO::Read(m_File,&i)) return 1; // error
        pi = (int *)nt.ptr;
        *pi = i;
        break;
    case NameTypeHandle::FLOAT:	
        if(SwapIO::Read(m_File,&f)) return 1; // error
        pf = (float *)nt.ptr;
        *pf = f;
        break;
    case NameTypeHandle::BOOL:	
        if(SwapIO::Read(m_File,&b)) return 1; // error
        pb = (bool *)nt.ptr;
        *pb = b;
        break;
    case NameTypeHandle::BOUNDINGBOX:	
        if(SwapIO::Read(m_File,&bb)) return 1; // error
        pbb = (BoundingBox *)nt.ptr;
        *pbb = bb;
        break;
    case NameTypeHandle::TYPEXYZ:	
        if(SwapIO::Read(m_File,&xyz)) return 1; // error
        pxyz = (XYZ *)nt.ptr;
        pxyz->mX = xyz.mX;
        pxyz->mY = xyz.mY;
        pxyz->mZ = xyz.mZ;
        break;
  }
  return 0;
}

int BMTReader::ReadFrame()
{
  int rc = 1;

  if(feof(m_File) || ferror(m_File)) return 1;
  
  int noff = ftell(m_File);
  // cout << "Read Frame at offset " << noff << endl;

  vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();
  for(;it!=m_VecFrameAttributes.end();it++) {
    rc = readAttributeFromFrame(*it);
    if(rc==1) {	
      return 1;
    }	
  }

  noff = ftell(m_File);
  // cout << "after reading attributes, offset " << noff << endl;

  switch(m_ConversionProperties.m_ReductionType) {
      case ConversionProperties::SHORT:
    rc = ReadFrameShort();
    break;
      case ConversionProperties::FLOAT:
      case ConversionProperties::DOUBLE:
    rc = ReadFrameReal(m_ConversionProperties.m_ReductionType);
    break;
  }
  noff = ftell(m_File);
  // cout << "after reading contents, offset " << noff << endl;
  return rc;
}

// class BMTWriter methods

void BMTWriter::InitDefault()
{
  setAttrib("VERSION",(string) "2.00");
  //setAttrib("STRUCTURE",(string) "null");
  setAttrib("NUMNONWATERATOMS",0);
  setAttrib("NUMWATERS",0);
  ConnectivityString cs("null");
  setAttrib("CONNECTIVITY",cs);
  setAttrib("STARTATOMINDEX",-1);
  setAttrib("ENDATOMINDEX",-1);
  setAttrib("SIMULATIONSTARTSTEP",0);
  setAttrib("INITIALSTEP",0);
  setAttrib("REVERSE",0);
  setAttrib("OUTERTIMESTEPINPICOSECONDS",0.0);
  setAttrib("SNAPSHOTPERIODINOTS",(unsigned int)1);
  setAttrib("BMTSKIPFACTOR",1);
  setAttrib("SOURCEID",(unsigned int) 0);
  setAttrib("EXECUTABLEID",(unsigned int) 0);
  setAttrib("FRAMESIZE", 0);
  setAttrib("SEEKSTART", 0);
  setAttrib("NOWATERS", 0);
  setAttrib("BYTEORDER", (string) "MSB");
  setAttrib("TRAJECTORYFORMAT", DetermineTrajectoryFormatString(16,0,true));
  m_ConversionProperties.determineReductionType();

  BoundingBox bb_temp;
  bb_temp.mMinBox = XYZ::ZERO_VALUE();
  bb_temp.mMaxBox = XYZ::ZERO_VALUE();
  setAttrib("INITIALBOX",bb_temp);
  
  strcpy(m_FileName, "");
  m_TrajectoryProperties.m_InitialSimulationTimeInNanoSeconds=0.0;
  m_TrajectoryProperties.m_MapIntoBox = false;
  m_TrajectoryProperties.m_BoxWidth = 0;
  m_FirstFrame = true;
  m_DoAppend = false;
  m_NetTimeStepInPicoSeconds = 1.0;
  m_NetSnapshotPeriodInOTS = 1;

  getAttrib("BMTSKIPFACTOR",&m_OldSkipFactor);
  getAttrib("INITIALSTEP",&m_CurrentStep);

  setIncludeVelocities(false);
  m_ForceWrite=false;
  
  m_pPos=NULL;
  m_pVel=NULL;
  m_pShortPos=NULL;
  m_pPosArray = NULL;
  m_pVelArray = NULL;

  setRecenter(false);
  m_pRecenter = NULL;

  m_DataSeekStart = 0;

}


BMTWriter::BMTWriter(char *fname)
{
  InitDefault();
  strcpy(m_FileName, fname);

  // AK: Create space for Frame format attributes:
  //	The corresponding implementation of the dynamic BMTWriter
  //	subclassed object will be invoked
  // FS: This is now handled as part of the dynamic process
  // BindNewFrameFormatVector();
}

BMTWriter::BMTWriter(char *fname, BMTReader &bmtr) {
  InitFromBMTReader(fname,bmtr);
}

BMTWriter::BMTWriter(char *fname, BMTReader &bmtr,int newSkipFactor) {
  InitFromBMTReader(fname,bmtr);
}

void BMTWriter::InitFromBMTReader(char *fname, BMTReader &bmtr)
{
  int ifoo;
  float ffoo;
  double dfoo;
  unsigned int uifoo;
  string sfoo;
  bool bfoo;
  BoundingBox bbfoo;
  ConnectivityString csfoo;

  // AK: check if redundant call
  InitDefault();
  strcpy(m_FileName, fname);
  m_pPos = bmtr.m_pPos;
  m_pVel = bmtr.m_pVel;

  setAttrib("VERSION","2.00");
  //setAttrib("STRUCTURE",bmtr.getAttrib("STRUCTURE",&sfoo));
  setAttrib("NUMNONWATERATOMS",bmtr.getAttrib("NUMNONWATERATOMS",&ifoo));
  setAttrib("NUMWATERS",bmtr.getAttrib("NUMWATERS",&ifoo));
  setAttrib("CONNECTIVITY",bmtr.getAttrib("CONNECTIVITY",&csfoo));
  setAttrib("STARTATOMINDEX",bmtr.getAttrib("STARTATOMINDEX",&ifoo));
  setAttrib("ENDATOMINDEX",bmtr.getAttrib("ENDATOMINDEX",&ifoo));
  setAttrib("SIMULATIONSTARTSTEP",bmtr.getAttrib("SIMULATIONSTARTSTEP",&ifoo));
  setAttrib("INITIALSTEP",bmtr.getAttrib("INITIALSTEP",&m_CurrentStep));
  setAttrib("REVERSE",bmtr.getAttrib("REVERSE",&ifoo));
  setAttrib("OUTERTIMESTEPINPICOSECONDS",bmtr.getAttrib("OUTERTIMESTEPINPICOSECONDS",&dfoo));
  setAttrib("SNAPSHOTPERIODINOTS",bmtr.getAttrib("SNAPSHOTPERIODINOTS",&uifoo));
  setAttrib("BMTSKIPFACTOR",bmtr.getAttrib("BMTSKIPFACTOR",&m_OldSkipFactor));
  setAttrib("SOURCEID",bmtr.getAttrib("SOURCEID",&uifoo));
  setAttrib("EXECUTABLEID",bmtr.getAttrib("EXECUTABLEID",&uifoo));
  setAttrib("FRAMESIZE", bmtr.getAttrib("FRAMESIZE",&ifoo));
  setAttrib("SEEKSTART", bmtr.getAttrib("SEEKSTART",&ifoo));
  setAttrib("NOWATERS", bmtr.getAttrib("NOWATERS",&ifoo));
  setAttrib("TRAJECTORYFORMAT", bmtr.getAttrib("TRAJECTORYFORMAT",&sfoo));
  setAttrib("INITIALBOX",bmtr.getAttrib("INITIALBOX",&bbfoo));
  
  m_ConversionProperties.determineReductionType();

  m_ConversionProperties.determineIncludeVelocities();

  m_TrajectoryProperties.m_InitialSimulationTimeInNanoSeconds=bmtr.getInitialSimulationTimeInNanoSeconds();
  m_TrajectoryProperties.m_MapIntoBox = bmtr.getMapIntoBox();
  m_TrajectoryProperties.m_BoxWidth = bmtr.getBoxWidth();
  m_FirstFrame = true; 
  m_DoAppend = false; 
  m_NetTimeStepInPicoSeconds = bmtr.m_NetTimeStepInPicoSeconds; 
  m_NetSnapshotPeriodInOTS = bmtr.m_NetSnapshotPeriodInOTS; 

  m_ForceWrite=false;

  string bmtr_ver;
  bmtr.getAttrib("VERSION",&bmtr_ver);

  if(strcmp((char *)&bmtr_ver,"1.00")!=0) {
    setAttrib("BYTEORDER", bmtr.getAttrib("BYTEORDER",&sfoo));
  }

        m_MolecularSystem.m_NAtoms = m_EndAtomIndex - m_StartAtomIndex + 1;

  // AK: Deal with Frame attributes:
  // A shallow-copy will suffice, as long as we create separate store
  //	for each measure in the corresponding FrameAttrib object
  //	locally in this BMTWriter object
  BindOldFrameFormatVector(bmtr.getFrameAttributeVector());

        // AMG: I commented out the next two lines, because
        // a) they don't make any sense -- we just set m_pPos = bmtr.m_pPos
        // b) the code segfaults if it's there -- the whole point of 
        //    initializing a writer from a reader is to copy the coordinates
        //    ...what the hell were Frank and Ananth thinking?
  //m_pPos=NULL;
  //m_pVel=NULL;
  m_pShortPos=NULL;
  m_pPosArray = NULL;
  m_pVelArray = NULL;

  setRecenter(false);
  m_pRecenter = (XYZ *) getProteinCOMHandle();
  m_DataSeekStart = 0;
}

// AK: Obsolete function 
//		New calls will not have NNonWaterAtoms and NWaters 
//		Still used by rdg2dx - so fix it there before removing

void BMTWriter::setParamsOld(char *BMTFileName, char *PDBFileName, char *connectivity, int NNonWaterAtoms, int NWaters, int DoAppend, int NoWaters, int SimulationStart, int Reverse,int SkipFactor,int PrecisionSpecifier,int Velocities)
{      
  // setAttrib("STRUCTURE",(string) PDBFileName);
  setAttrib("NUMNONWATERATOMS",NNonWaterAtoms);
  setAttrib("NUMWATERS",NWaters);
  setAttrib("NOWATERS",NoWaters);
  setAttrib("STARTATOMINDEX",0);
  if (m_NoWaters) {
      m_EndAtomIndex = NNonWaterAtoms - 1;
      m_MolecularSystem.m_NAtoms = NNonWaterAtoms;
  } else {
      m_EndAtomIndex = NNonWaterAtoms + 3*NWaters - 1;
     m_MolecularSystem.m_NAtoms = m_EndAtomIndex+1;
  }
  ConnectivityString cs(connectivity);
  setAttrib("CONNECTIVITY",cs);
  addFragments();
  setAttrib("SIMULATIONSTARTSTEP",SimulationStart);
  setAttrib("REVERSE",Reverse);

  setAttrib("BMTSKIPFACTOR",SkipFactor);

  setAttrib("TRAJECTORYFORMAT",DetermineTrajectoryFormatString(PrecisionSpecifier,Velocities));
  m_ConversionProperties.determineReductionType();

  setIncludeVelocities(Velocities==1);


  // set the BMT file name
  strcpy(m_FileName,BMTFileName);

}


void BMTWriter::setParams(char *BMTFileName, char *connectivity, int DoAppend, int NoWaters, int SimulationStart, int Reverse,int SkipFactor,int PrecisionSpecifier,int Velocities, bool MapIntoBox,bool Recenter)
{      
  setAttrib("NOWATERS",NoWaters);
  // AK: this will be overwritten anyway - from db
  //		remove this
        ConnectivityString cs(connectivity);
  setAttrib("CONNECTIVITY",cs);
  addFragments();
  setAttrib("SIMULATIONSTARTSTEP",SimulationStart);
  setAttrib("REVERSE",Reverse);

  setAttrib("BMTSKIPFACTOR",SkipFactor);

  setAttrib("TRAJECTORYFORMAT",DetermineTrajectoryFormatString(PrecisionSpecifier,Velocities,Recenter));
  m_ConversionProperties.determineReductionType();

  setIncludeVelocities(Velocities==1);

  // set the BMT file name
  strcpy(m_FileName,BMTFileName);
  
  setMapIntoBox(MapIntoBox);

  setRecenter(Recenter);
  m_pRecenter = NULL; 

}

void BMTWriterRDG2BMT::setParams(char *BMTFileName, char *connectivity, int DoAppend, int NoWaters, int SimulationStart, int Reverse,int SkipFactor,int PrecisionSpecifier,int Velocities,bool MapIntoBox, bool Recenter,int FramesPerBMT) {

  m_FramesPerBMT = FramesPerBMT;
  strcpy(m_OrigFileName,BMTFileName);

  BMTWriter::setParams(BMTFileName, connectivity, DoAppend, NoWaters, SimulationStart, Reverse,SkipFactor,PrecisionSpecifier,Velocities,MapIntoBox,Recenter) ;
}


void BMTWriter::OpenForAppend()
{
  // close text mode and switch to binary, appending at end
  m_File = fopen(m_FileName, "a+b");
  if (!m_File) {
      cerr << "BMT output file " << m_FileName << " could not be open for binary write/append." << endl;
      exit(-1);
  }
}

void BMTWriter::WriteHeader()
{
  int ifoo;
  float ffoo;
  double dfoo;
  unsigned int uifoo;
  string sfoo;
  bool bfoo;
  BoundingBox bbfoo;
  ConnectivityString csfoo;

  m_OutStream.open(m_FileName);
  if (!m_OutStream.is_open()) {
      cerr << "BMT output file " << m_FileName << " could not be open for write." << endl;
      exit(-1);
  }
  m_OutStream << "BMT" << endl;

  // AK Change: Pump up the BMT Version Number
  m_OutStream << "VERSION " << getAttrib("VERSION",&sfoo) << endl;
  //m_OutStream << "STRUCTURE " << getAttrib("STRUCTURE",&sfoo) << endl;
  m_OutStream << "NUMNONWATERATOMS " << getAttrib("NUMNONWATERATOMS",&ifoo) << endl;
  m_OutStream << "NUMWATERS " << getAttrib("NUMWATERS",&ifoo) << endl;
  m_OutStream << "NOWATERS " << getAttrib("NOWATERS",&ifoo) << endl;
  m_OutStream << "STARTATOMINDEX " << getAttrib("STARTATOMINDEX",&ifoo) << endl;
  m_OutStream << "ENDATOMINDEX " << getAttrib("ENDATOMINDEX",&ifoo) << endl;
   getAttrib("CONNECTIVITY",&csfoo);
  m_OutStream << "CONNECTIVITY " << csfoo << endl;
  m_OutStream << "REVERSE " << getAttrib("REVERSE",&ifoo) << endl;
  m_OutStream << "INITIALSTEP " << getAttrib("INITIALSTEP",&ifoo)  << endl;
  m_OutStream << "SIMULATIONSTARTSTEP " << getAttrib("SIMULATIONSTARTSTEP",&ifoo) << endl;
  m_OutStream << "SNAPSHOTPERIODINOTS " << getAttrib("SNAPSHOTPERIODINOTS",&uifoo) << endl;
  m_OutStream << "OUTERTIMESTEPINPICOSECONDS " << getAttrib("OUTERTIMESTEPINPICOSECONDS",&dfoo) << endl;
  m_OutStream << "BMTSKIPFACTOR " << getAttrib("BMTSKIPFACTOR",&ifoo) << endl;
  m_OutStream << "SOURCEID " << getAttrib("SOURCEID",&uifoo) << endl;
  m_OutStream << "EXECUTABLEID " << getAttrib("EXECUTABLEID",&uifoo) << endl;
  BoundingBox bbptr = getAttrib("INITIALBOX",&bbfoo);
  m_OutStream << "INITIALBOX [ " << bbptr.mMinBox.mX << " " << bbptr.mMinBox.mY << " " << bbptr.mMinBox.mZ << " ] "; 
  m_OutStream <<            "[ " << bbptr.mMaxBox.mX << " " << bbptr.mMaxBox.mY << " " << bbptr.mMaxBox.mZ << " ]";
  m_OutStream << endl;

  m_OutStream << "TRAJECTORYFORMAT " << getAttrib("TRAJECTORYFORMAT",&sfoo) << endl;
  m_OutStream << "BYTEORDER " << getAttrib("BYTEORDER",&sfoo) << endl;
  m_OutStream << "FRAMESIZE " << getAttrib("FRAMESIZE",&ifoo) << endl;
  // AK: For later fix
  // m_OutStream << "SEEKSTART " << getAttrib("SEEKSTART",&ifoo) << endl;
  m_OutStream << BEGINFRAMEFORMAT << endl;

  // AK: print the frame format in the header
  PrintFrameFormatInBMTHeader();

  m_OutStream << "BEGINTRAJECTORY" << endl;
  m_OutStream.close();

  OpenForAppend();
}
    
int BMTWriter::WriteFrameReal()
{

        ConversionProperties::REDUCTIONTYPE rt = getReductionType();

  XYZ *pp = m_pPos;
  double dv;
  float fv;

  for (int i=0; i<m_MolecularSystem.m_NAtoms; i++, pp++) {
      for (int j=0; j<3; j++) {
    dv = (*pp)[j];
    fv = (float)dv;
    if (rt == ConversionProperties::FLOAT) {
        if (SwapIO::Write(m_File, &fv)) {
      cout << "Warning: WriteFrameReal Write() wrote probably less number of bytes \n";
      return 1;
        }
    } else {
        if (SwapIO::Write(m_File, &dv)) {
      cout << "Warning: WriteFrameReal Write() wrote probably less number of bytes \n";
      return 1;
        }
    }
      }
  }
  WriteFrameVelocities();
  return 0;
}

int BMTWriterRDG2BMT::WriteFrameVelocities()
{

  if(!getIncludeVelocities()) return 0;

  ConversionProperties::REDUCTIONTYPE rt = getReductionType();

  XYZ *pv = m_pVel;
  double dv;
  float fv;

  for (int i=0; i<m_MolecularSystem.m_NAtoms; i++, pv++) {
      for (int j=0; j<3; j++) {
    dv = (*pv)[j];
    fv = (float)dv;
    if (rt == ConversionProperties::FLOAT) {
        if (SwapIO::Write(m_File, &fv)) {
      cout << "Warning: WriteFrameReal Write() wrote probably less number of bytes \n";
      return 1;
        }
    } else {
        if (SwapIO::Write(m_File, &dv)) {
      cout << "Warning: WriteFrameReal Write() wrote probably less number of bytes \n";
      return 1;
        }
    }
      }
  }
  return 0;
}
    
int BMTWriter::WriteFrameShort()
{
  usXYZ *psp = m_pShortPos;

  for (int i=0; i<m_MolecularSystem.m_NAtoms; i++, psp++) {
      for (int j=0; j<3; j++) {
    if (SwapIO::Write(m_File, &(*psp)[j]))
        return 1;
      }
  }
  return 0;
}

bool BMTWriter::SkipFrame() {

  if(m_ForceWrite) return false;
  
  unsigned int Snapshot;
  getAttrib("SNAPSHOTPERIODINOTS",&Snapshot);

  int Skip ;
  getAttrib("BMTSKIPFACTOR",&Skip);

  int SimStartStep;
  getAttrib("SIMULATIONSTARTSTEP",&SimStartStep);


  if((m_CurrentStep-SimStartStep) % (Skip*Snapshot) != 0 ) {
    return true;
  }

  return false;
}

// Reset the current step to the timestep of the expected next frame
// This method should be overridden depending on where the next frame is coming from
//	(the below will work as-is if source is RDG or another BMT ie., BMTReader)

void BMTWriter::AdvanceCurrentStep() {
  unsigned int Snapshot;
  getAttrib("SNAPSHOTPERIODINOTS",&Snapshot);

  m_CurrentStep += m_OldSkipFactor * Snapshot;
}

int BMTWriter::WriteFrame()
{
  int rc = 1;

  if(SkipFrame()) {
    AdvanceCurrentStep(); 
    return 0;
  }

  ManageFrames();

  if (m_FirstFrame) {
      if(getRecenter()) {
    m_pRecenter = (XYZ *) getProteinCOMHandle();
    if(!m_pRecenter) {
      m_pRecenter = (XYZ *) setupNewFrameAttrib("ProteinCOM","TYPEXYZ");
    }
      }	

      setAttrib("INITIALSTEP",m_CurrentStep);
      if (m_DoAppend) OpenForAppend();
      else WriteHeader();


          m_FirstFrame = false;
  }
  
  ReimagePositions();
  
  RecenterOnProtein();

  MapIntoBoxPositions();

  switch(m_ConversionProperties.m_ReductionType) {
      case ConversionProperties::SHORT:
    WriteFrameHeaderShort();
    rc = WriteFrameShort();
    break;
      case ConversionProperties::FLOAT:
      case ConversionProperties::DOUBLE:
    WriteFrameHeaderReal();
    rc = WriteFrameReal();
    break;
  }
  return rc;
}

// AK: Create space for new frame attributes in this object

void BMTWriterRDG2BMT::BindNewFrameFormatVector(Frame *f) {
  
  setupNewFrameAttrib("Step","UINT");
  setupNewFrameAttrib("SimulationTime","DOUBLE");
  setupNewFrameAttrib("TotalEnergy","DOUBLE");
  setupNewFrameAttrib("KineticEnergy","DOUBLE");
  setupNewFrameAttrib("PotentialEnergy","DOUBLE");
  setupNewFrameAttrib("Temperature","DOUBLE");
  setupNewFrameAttrib("ConservedQuantity","DOUBLE");
  setupNewFrameAttrib("DynamicBoundingBox", "BOUNDINGBOX");

  for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
      if (f->mUDFs[i].isComplete() && DataReceiverSimpleLogger::IncludeValue(i)) {
    setupNewFrameAttrib(DataReceiverSimpleLogger::HeaderStringFromUDFCode(i), "DOUBLE");
    if (i == UDF_Binding::EwaldKSpaceForce_Code) {
        setupNewFrameAttrib("EwaldSum", "DOUBLE");
    }
      }
  }

}

// AK: Create space for the frame attributes in this object
//	name, and type variables in each nth record is valid
//	Update only the ptr variable

void BMTWriter::BindOldFrameFormatVector(vector<NameTypeHandle> vec) {

  string stType;

  vector<NameTypeHandle>::iterator it=vec.begin();
  for(;it!=vec.end();it++) {
    stType = NameTypeHandle::getTypeStr(it->type);

    char sAttr[ATTRIBUTEVALUESIZE];	
    char sType[ATTRIBUTEVALUESIZE];	
    strcpy(sAttr,it->name.c_str());
    strcpy(sType,stType.c_str());
    
    setupNewFrameAttrib(sAttr,sType);
  }
}


// AK:  Printout all the frame attributes
//	These will be printed in the BMT header under BEGINFRAMEFORMAT

void BMTWriter::PrintFrameFormatInBMTHeader() {


  string sType;
  vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();
  for(;it!=m_VecFrameAttributes.end();it++) {
    sType = NameTypeHandle::getTypeStr(it->type);
    m_OutStream << it->name << "  " << sType << endl;
  }
}

// AK: Get <name,type> pairs for frame attributes

void BMT::PrintFrameAttributes() {
  string sType;
  vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();
  for(;it!=m_VecFrameAttributes.end();it++) {
    sType = NameTypeHandle::getTypeStr(it->type);
    cout << it->name << "  " << sType << endl;
  }
}

vector<NameTypePair> BMT::getFrameNameTypePair() {
  vector<NameTypePair> vec_ntp;
  NameTypePair ntp;

        vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();
        for(;it!=m_VecFrameAttributes.end();it++) {
    ntp.name = it->name;
    ntp.type =  NameTypeHandle::getTypeStr(it->type);
    vec_ntp.push_back(ntp);
        }
  return vec_ntp;
}

int BMTWriter::Close()
{
  if(m_File) fclose(m_File);
  return 0;
}

// called when sites arrive
// Will be called from DataReceiverTrajectoryDumper
// AK Added: 6/14/2005
int BMTWriterRDG2BMT::WriteFrame(Frame *f)
{
  m_CurrentStep = (int) f->mOuterTimeStep;
  tSiteData *ps = f->mSiteData.getArray();
  int NSites = f->mSiteData.getSize();

  if (m_FirstRDGFrame) {
      XYZ min,max,width;
      min = f->mInformationRTP.mDynamicBoundingBoxMin;
      max = f->mInformationRTP.mDynamicBoundingBoxMax;
        // Ensure that min < max
        // AMG added 8/28/07
        if (min.mX > max.mX) {
            double tmp = max.mX;
            max.mX = min.mX;
            min.mX = tmp;
        }
        if (min.mY > max.mY) {
            double tmp = max.mY;
            max.mY = min.mY;
            min.mY = tmp;
        }
        if (min.mZ > max.mZ) {
            double tmp = max.mZ;
            max.mZ = min.mZ;
            min.mZ = tmp;
        }

      width = max - min;
      BoundingBox thisInitBox;
      thisInitBox.mMinBox = min;
      thisInitBox.mMaxBox = max;
      setAttrib("INITIALBOX",thisInitBox);
      setAttrib("OUTERTIMESTEPINPICOSECONDS", f->mInformationRTP.mOuterTimeStepInPicoSeconds);
      setAttrib("SNAPSHOTPERIODINOTS", f->mInformationRTP.mSnapshotPeriodInOTS);
      setAttrib("SOURCEID", f->mInformationRTP.mSourceID);
      setAttrib("EXECUTABLEID", f->mInformationRTP.mExecutableID);
      if(!m_pPos) m_pPos = new XYZ[NSites];
      if(!m_pShortPos) m_pShortPos = new usXYZ[NSites];

      if(!m_pVel) m_pVel = new XYZ[NSites];
      m_MolecularSystem.getSystemInfo(f->mInformationRTP.mSourceID,&m_DatabaseProperties);

////////////////////
            ConnectivityString cs;
      getAttrib("CONNECTIVITY",&cs);
      int NNonWaterAtoms = cs.getNumberOfNonWaterAtoms();
      int NWaters = cs.getNumberOfWaters();
      setAttrib("NUMNONWATERATOMS",NNonWaterAtoms);
      setAttrib("NUMWATERS",NWaters);
      setAttrib("STARTATOMINDEX",0);
      if (m_NoWaters) {
        m_EndAtomIndex = NNonWaterAtoms - 1;
        m_MolecularSystem.m_NAtoms = NNonWaterAtoms;
      } else {
        m_EndAtomIndex = NNonWaterAtoms + 3*NWaters - 1;
      m_MolecularSystem.m_NAtoms = m_EndAtomIndex+1;
      }
/////////////////// 

      // Frank's code to dynamically bind frame attributes from the frame
      AddCurrentFrameAttributes(f);

      // compute frame size
      setAttrib("FRAMESIZE",determineFrameSize());

      m_FirstRDGFrame=false;

  }

  for (int i=0; i<NSites; i++) {
      m_pPos[i] = ps[i].mPosition;
      // Read the velocities also 
      m_pVel[i] = ps[i].mVelocity;
  }

  // AK: read and store the frame attributes 
  AcquireFrameAttributes(f);

  return BMTWriter::WriteFrame();
}

// Add frame attributes here
// TimeStep SimTime TotalE KE PE Temp CQ
// Then add remaining components based on their presence as UDF's.
// Later add box and pressure for pressure control
void BMTWriterRDG2BMT::AddCurrentFrameAttributes(Frame *f)
{
    BindNewFrameFormatVector(f);
}


// AK: New function:
// 	code for storing all frame attributes in order from the Frame object
//	into each's corresponding store in the m_VecFrameAttributes
// Order:       TOTALENERGY 
//		KINETICENERGY 
//		POTENTIALENERGY
//		TEMPERATURE 
//        	RESAMPLEENERGYLOSS
//        	CONSERVEDQUANTITY

void BMTWriterRDG2BMT::AcquireFrameAttributes(Frame *f) {
  double *handle;
  unsigned int *uihandle;
  BoundingBox *bbhandle;

  vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();

  uihandle = (unsigned int *) it->ptr;
  *uihandle = f->mOuterTimeStep;
  it++;

  handle = (double *) it->ptr;
  *handle = (f->mOuterTimeStep - m_TrajectoryProperties.m_SimulationStartStep) *
      m_TrajectoryProperties.m_OuterTimeStepInPicoSeconds/1000.0;
  it++;

  handle = (double *) it->ptr;
  *handle = f->mEnergyInfo.mEnergySums.mTotalE;
  it++;

  handle = (double *) it->ptr;
  *handle = f->mEnergyInfo.mEnergySumAccumulator.mKineticEnergy;
  it++;

  handle = (double *) it->ptr;
  *handle = f->mEnergyInfo.mEnergySums.mTotalE - 
      f->mEnergyInfo.mEnergySumAccumulator.mKineticEnergy;
  it++;

  handle = (double *) it->ptr;
  *handle = f->mEnergyInfo.mEnergySums.mTemp;
  it++;

  handle = (double *) it->ptr;
  *handle = f->mEnergyInfo.mEnergySums.mConservedQuantity;
  it++;

  bbhandle = (BoundingBox *) it->ptr;
  BoundingBox bb;
  bb.mMinBox = f->mInformationRTP.mDynamicBoundingBoxMin;
  bb.mMaxBox = f->mInformationRTP.mDynamicBoundingBoxMax;
  *bbhandle = bb;
  it++;

  for (int i=0; i<UDF_Binding::UDF_CODE_COUNT; i++) {
      if (f->mUDFs[i].isComplete() && DataReceiverSimpleLogger::IncludeValue(i)) {
    handle = (double *) it->ptr;
    *handle = f->mUDFs[i].getSum();
    it++;
    if (i == UDF_Binding::EwaldKSpaceForce_Code) {
        handle = (double *) it->ptr;
        *handle = f->mEwaldSum;
        it++;
    }
      }
  }
}

// AK: New function:
// 	code for writing energy, temperature and other frame attributes 
//	into each frame.

int BMTWriter::WriteFrameAttributes() {

  vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();

  for(;it!=m_VecFrameAttributes.end();it++) {
    it->writeTo(m_File);
  }
  return 0;
}



template <class T> void Attrib<T>::bind(string name, T *addrVal) {
  if(has(name)) {
    cout << "Warning: BMT_Attrib::bind: new binding for "<<name << endl;
  }
        (*this)[name] = addrVal;
}

template <class T> void Attrib<T>::set(string name,T val) {
  if(has(name)) {
    // this is already pointing to something
  }
  else{
    // new attribute - so allocate new memory
    (*this)[name] = new T;
  }
  *((*this)[name]) = val; //assuming = operator is overloaded in T
}

template <class T> T Attrib<T>::get(string name) {
  assert(has(name));
  typename Attrib<T>::iterator it = find(name);
  return *it->second;
}

template <class T> bool Attrib<T>::has(string name) {
  typename Attrib<T>::iterator it = find(name);
  return (it!=end());
}

template <class T> void Attrib<T>::print() {
  typename Attrib<T>::iterator it;
  for(it=begin();it!=end();++it) {
    cout << "BMT Attribute: "<< it->first << ":= " << *it->second << endl;
  } 
}

template <class T> T* FrameAttrib<T>::getHandle(string name) {
  assert(has(name));
  typename FrameAttrib<T>::iterator it = find(name);
  return it->second;
}

template <class T> void *FrameAttrib<T>::createHandle(string name) {
  if(has(name)) { // this is already pointing to something
    cout << "Warning: BMT_FrameAttrib::createHandle: overwriting already existing binding for "<<name << endl;
  }
  // new attribute - so allocate new memory
  return (*this)[name] = new T;
}


NameTypeHandle::NameTypeHandle() {
       name = "";
       type = NameTypeHandle::INVALID;
       ptr=NULL;
}

NameTypeHandle::NameTypeHandle(string n,NameTypeHandle::BMTATTRIBUTETYPE t,void *handle) {
       name = n;
       type = t;
       ptr=handle;
}

int NameTypeHandle::writeTo(FILE *fp) {
  XYZ *p;
       switch(type)  {
          case NameTypeHandle::DOUBLE: 
        if (SwapIO::Write(fp, (double *)ptr)) return 1;
        return 0;
                case NameTypeHandle::CHAR: 
        if (SwapIO::Write(fp, (char *)ptr)) return 1;
        return 0;
                case NameTypeHandle::INT: 
        if (SwapIO::Write(fp, (int *)ptr)) return 1;
        return 0;
                case NameTypeHandle::UINT:
        if (SwapIO::Write(fp, (unsigned int *)ptr)) return 1;
        return 0;
                case NameTypeHandle::FLOAT:
        if (SwapIO::Write(fp, (float *)ptr)) return 1;
        return 0;
                case NameTypeHandle::BOOL:
        if (SwapIO::Write(fp, (bool *)ptr)) return 1;
        return 0;
                case NameTypeHandle::BOUNDINGBOX: 
        if (SwapIO::Write(fp, (BoundingBox *)ptr)) return 1;
        return 0;
                case NameTypeHandle::TYPEXYZ: 
        p = (XYZ *)ptr;
        double dv;
        dv = p->mX;
        if (SwapIO::Write(fp, &dv)) return 1;
        dv = p->mY;
        if (SwapIO::Write(fp, &dv)) return 1;
        dv = p->mZ;
        if (SwapIO::Write(fp, &dv)) return 1;
        return 0;
                default:       
      cerr << "NameTypeHandle_writeTo: Cannot write undefined type to SwapIO" << endl;
      assert(0);
      break;
       }
  return 0;
}
        
string NameTypeHandle::getTypeStr(NameTypeHandle::BMTATTRIBUTETYPE type) {
       switch(type)  {
          case NameTypeHandle::DOUBLE: return "DOUBLE";
                case NameTypeHandle::CHAR: return "CHAR";
                case NameTypeHandle::INT: return "INT";
                case NameTypeHandle::UINT: return "UINT";
                case NameTypeHandle::FLOAT: return "FLOAT";
                case NameTypeHandle::BOOL: return "BOOL";
                case NameTypeHandle::BOUNDINGBOX: return "BOUNDINGBOX";
                case NameTypeHandle::TYPEXYZ: return "TYPEXYZ";
                case NameTypeHandle::STRING: return "STRING";
                case NameTypeHandle::ALL: return "ALL";
                default:        return "INVALID";
       }
}

NameTypeHandle::BMTATTRIBUTETYPE getTypeFromStr(string type) {
  if(type == "DOUBLE") return NameTypeHandle::DOUBLE;
  if(type == "CHAR") return NameTypeHandle::CHAR;
  if(type == "INT") return NameTypeHandle::INT;
  if(type == "UINT") return NameTypeHandle::UINT;
  if(type == "FLOAT") return NameTypeHandle::FLOAT;
  if(type == "BOOL") return NameTypeHandle::BOOL;
  if(type == "BOUNDINGBOX") return NameTypeHandle::BOUNDINGBOX;
  if(type == "TYPEXYZ") return NameTypeHandle::TYPEXYZ;
  if(type == "STRING") return NameTypeHandle::STRING;
  if(type == "ALL") return NameTypeHandle::ALL;
  return NameTypeHandle::INVALID;
}

NameTypeHandle& NameTypeHandle::operator=(const NameTypeHandle& aOther) {
        name = aOther.name;
        type = aOther.type;

       switch(type)  {
          case NameTypeHandle::DOUBLE: 
      memcpy(ptr,aOther.ptr,sizeof(double)); break;
                case NameTypeHandle::CHAR: 
      memcpy(ptr,aOther.ptr,sizeof(char)); break;
                case NameTypeHandle::INT: 
      memcpy(ptr,aOther.ptr,sizeof(int)); break;
                case NameTypeHandle::UINT:
      memcpy(ptr,aOther.ptr,sizeof(unsigned int)); break;
                case NameTypeHandle::FLOAT:
      memcpy(ptr,aOther.ptr,sizeof(float)); break;
                case NameTypeHandle::BOOL:
      memcpy(ptr,aOther.ptr,sizeof(bool)); break;
                case NameTypeHandle::BOUNDINGBOX: 
      memcpy(ptr,aOther.ptr,sizeof(BoundingBox)); break;
                case NameTypeHandle::TYPEXYZ: 
      memcpy(ptr,aOther.ptr,sizeof(XYZ)); break;
                default:       
       ptr=NULL;
       }
       return *this;
}


int BMT::SizeOf(string type) {
  if(type == "DOUBLE") return sizeof(double);
  if(type == "CHAR") return sizeof(char);
  if(type == "INT") return sizeof(int);
  if(type == "UINT") return sizeof(unsigned int);
  if(type == "FLOAT") return sizeof(float);
  if(type == "BOOL") return sizeof(bool);
  if(type == "BOUNDINGBOX") return sizeof(BoundingBox);
  if(type == "TYPEXYZ") return sizeof(XYZ);
  return 0;
}

template <class T> void BMT::ReadTypedData(FILE *f,T *data) {
  int count =  fread(data, sizeof(T), 1, f);
  if(count !=1 || feof(f) || ferror(f) ) return;
   cout << *data <<endl;
}


void BMT::ReadAndPrintTypedData(FILE *f,string type) {
        double dfoo;
        int ifoo;
        char cfoo;
        short sfoo;
        unsigned int uifoo;
        float ffoo;
        bool bfoo;
        BoundingBox bbfoo;

        if(type == "DOUBLE") {
    ReadTypedData(f,&dfoo);
                return;
        }
        if(type == "FLOAT") {
    ReadTypedData(f,&ffoo);
                return;
        }
        if(type == "INT") {
    ReadTypedData(f,&ifoo);
                return;
        }
        if(type == "UINT") {
    ReadTypedData(f,&uifoo);
                return;
        }
        if(type == "CHAR") {
    ReadTypedData(f,&cfoo);
                return;
        }
        if(type == "BOOL") {
    ReadTypedData(f,&bfoo);
                return;
        }
        // default: read nothing
}

// AK: This routine computes the size of each frame in bytes
//	If there are only positions:
//	FrameSize:= size of frame header fields 
//	 		+ number of atoms * 3 * precision_bytes
// 	If there are both positions and velocities:
//	FrameSize:= size of frame header fields 
//	 		+ number of atoms * 3 * precision_bytes * 2
//	Plus: space for 4 shorts must be added for scaling factor and origin
//		if the precision_bits is 16

int BMT::determineFrameSize() {
  int nFrameSize=0;

  nFrameSize=determineFrameHeaderSize();
  // cerr << "Header size: " << nFrameSize << endl;

        ConversionProperties::REDUCTIONTYPE rt ;
        rt = m_ConversionProperties.determineReductionType();
        if(rt==ConversionProperties::SHORT) nFrameSize+=4*sizeof(float);

  int NEntries = m_EndAtomIndex-m_StartAtomIndex+1;

  if(IsCOMTrajectory()) { // only one entry per molecule
    ConnectivityString cs;	
    getAttrib("CONNECTIVITY",&cs);
    NEntries = cs.getNumberOfMolecules();
  }

  // cerr << "NEntries: " << NEntries << endl;

        int for_pos = NEntries*3*m_ConversionProperties.SizeOf(rt);
  // cerr << "PosSize: " << for_pos << endl;


  if(getIncludeVelocities()) for_pos = for_pos*2;
  // cerr << "WithVel: " << for_pos << endl;

  nFrameSize+=for_pos;
  // cerr << "Total Frame size: " << nFrameSize << endl;

  return nFrameSize;
}

int BMT::determineFrameHeaderSize() {
  int nFrameHSize=0;

  vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();
  for(;it!=m_VecFrameAttributes.end();it++) {
    nFrameHSize+=SizeOf(NameTypeHandle::getTypeStr(it->type));	
  }

  return nFrameHSize;
}


int BMTWriter::WriteFrameHeaderShort() {
  // AK: for writing energy, temperature and other
  //	frame attributes into each frame.
  WriteFrameAttributes();

        // Write origin and scaling factor
  XYZ origin, span;
  double maxspan;
  findBox(m_pPos, 0, m_MolecularSystem.m_NAtoms, origin, span, maxspan);
  double factor = 65535.0/maxspan;
  float recipfactor = maxspan/65535;
  if(!m_pShortPos) m_pShortPos = new usXYZ[m_MolecularSystem.m_NAtoms];
  simplifySites(m_pPos, m_pShortPos, 0, m_MolecularSystem.m_NAtoms, origin, factor);
  
  float x,y,z;
  x = (float)origin.mX;
  y = (float)origin.mY;
  z = (float)origin.mZ;
  if (SwapIO::Write(m_File, &x))
      return 1;
  if (SwapIO::Write(m_File, &y))
      return 1;
  if (SwapIO::Write(m_File, &z))
      return 1;
  if (SwapIO::Write(m_File, &recipfactor))
      return 1;

  return 0;
}

int BMTWriter::WriteFrameHeaderReal() {
  // AK: for writing energy, temperature and other
  //	frame attributes into each frame.
  return WriteFrameAttributes();
  
}


/// AK: Method to read frame with a specified SimulationTime t relative to SimulationStartTime:
//		t' <- t - InitialSimulationTime
//		dt <- m_NetTimeStepInPicoSeconds (ie., OTS x SnapshotPeriodInOTS x SkipFactor )
//		i  <- round ( t'/dt ) 
//		Return Frame_i

int BMTReader::ReadFrameBySimulationTime(double t) {

  // cout << "AK_debug: ReadFrameBySimulationTime " << t << endl;

  double tprime,dt;
  double initSimTime = m_TrajectoryProperties.m_InitialSimulationTimeInNanoSeconds;

  tprime = t - initSimTime;
  dt = m_NetTimeStepInPicoSeconds/1000;

  // cout << "AK_debug: t = " << t << endl;
  // cout << "AK_debug: initSimTime = " << initSimTime << endl;
  // cout << "AK_debug: dt = " << dt << endl;
  // cout << "AK_debug: t' = " << tprime << endl;
  

  assert(dt>0);
  int i = (int) (tprime/dt + 0.5);

  // cout << "AK_debug: i = " << i << endl;
  return ReadFrame(i);
}

// Method to read frame with a specified TimeStep x relative to the SimulationStartStep:
//		x' <-  x - InitialStep
//		i <- round (x'/skipfactor)
//		Return Frame_i

int BMTReader::ReadFrameByTimeStep(int x) {

  // cout << "AK_debug: ReadFrameByTimeStep " << x << endl;

  int xprime = x - m_TrajectoryProperties.m_InitialStep;
  int skipf = m_ConversionProperties.m_SkipFactor;
  unsigned int SnapshotPeriodInOTS;
  getAttrib("SNAPSHOTPERIODINOTS",&SnapshotPeriodInOTS);
  
  unsigned int netSkip = skipf * SnapshotPeriodInOTS;

  assert(netSkip>0);
  
  int i = (int) ((double) (xprime/netSkip) + 0.5); 

  // cout << "AK_debug: x' = " << xprime << endl;
  // cout << "AK_debug: skipf = " << skipf << endl;
  // cout << "AK_debug: Snapshot  = " << SnapshotPeriodInOTS << endl;
  // cout << "AK_debug: netSkip = " << netSkip << endl;
  // cout << "AK_debug: i = " << i << endl;

  return ReadFrame(i);
}


// Read the frame i
// 	Convention: frame numbers starting at 0 in the BMT file and incrementing by 1

int BMTReader::ReadFrame(int i) {

  int rc = SeekFrame(i);
  if(rc==0) {
    cerr << "Error: ReadFrame: Frame " << i << " not in BMT !! " << endl;
    return 1;
  }

  return ReadFrame();
}


int BMTReader::SeekFrame(int i) {
  int rc = 0;
  char buff[2048];

      // Get the version

  if (i < 0) {
      i = m_NFrames + i;
      if (i < 0) {
    cerr << "Error seeking frame " << i << endl;
    exit(-1);
      }
  }

  off_t pos = m_DataSeekStart + i*m_FrameSize;
  // cerr << "Seeking to frame " << i << " at offset " << pos << endl;
  rc = fseeko(m_File, pos, SEEK_SET);
  if (rc == 0) {
      // cerr << "seek ok" << endl;
      return 1;
  } else {
      cerr << "Error seeking frame " << i << endl;
      exit(-1);
  }
  return 0;
#if 0
      bool bOldVer = true;
      string s_ver;
      NameTypeHandle::BMTATTRIBUTETYPE typ;
      if(hasAttrib("VERSION",&typ)) {
          if(typ == NameTypeHandle::STRING) {
                  getAttrib("VERSION",&s_ver);
                  if(s_ver == "1.00" ) bOldVer = true;
                  else bOldVer = false;
          }
      }

  // scroll to first frame (F_0) ie., skip header

  if(m_File) fclose(m_File);

  m_File = fopen(m_FileName, "rb");
        if (!m_File) {
            cerr << "Error reading header of bmt file " << m_FileName << endl;
            exit(-1);
        }

    while (1)  {
          if(feof(m_File) || ferror(m_File) )  {
      break;
    }
          fgets(buff, BUFFSIZE, m_File);
          if(bOldVer && (0==strncmp(buff, BEGINTRAJ_VER1 , strlen(BEGINTRAJ_VER1)) ) ) {
                  break;
    }
          if(!bOldVer && (0==strncmp(buff, BEGINTRAJ_VER2 , strlen(BEGINTRAJ_VER2)) ) ) {
                  break;
    }
  }
  
  if(m_FrameSize<=0) m_FrameSize = determineFrameSize();

  if(i==0) return 1; // want the first frame

  if(i<0) { // want the last frame
          int f1 = open(m_FileName,O_RDONLY);
          off_t e = lseek(f1,0,SEEK_END); // get file size in bytes
          close(f1);

    off_t curr_off = ftello(m_File);
    assert(curr_off<=e);
    
    int nFrames = (e-curr_off+1)/m_FrameSize; // this will be floored automatically

    fseeko(m_File,(nFrames-1)*m_FrameSize,SEEK_CUR);
    return 1;
  }

  // want the ith frame
  off_t jump_by = m_FrameSize*i;
  
  off_t b4 = ftello(m_File);
  
  if(0 != fseeko(m_File,jump_by,SEEK_CUR) ) {
    int err=errno;
    printf("Error code: SeekFrame(): errno = %d, error: %s\n",err,strerror(err));
    cerr << "Error: SeekFrame():  Could not seek by " << jump_by + b4 << " bytes!!" << endl;
    return 0;
  }

  off_t af = ftello(m_File);

  if( (af-b4)!=jump_by) {
    cout << "Error: SeekFrame():  Seek required = " << jump_by << " and Seek performed by " << af-b4 << " bytes!!" << endl;
    return 0;
  }

  return 1;
#endif
}

void BMTWriter::MapIntoBoxPositions() {
  
  if(!getMapIntoBox()) return ;

  ConnectivityString cs;
  getAttrib("CONNECTIVITY",&cs);
  vector<MoleculeRun> mr = cs.parseConnectivityString();
  
  XYZ *MinBox = &m_TrajectoryProperties.m_InitialBox.mMinBox;
  XYZ *MaxBox = &m_TrajectoryProperties.m_InitialBox.mMaxBox;
  XYZ Width = *MaxBox - *MinBox;
  
  XYZ origin;
  origin.mX = (MaxBox->mX + MinBox->mX)/2; 
  origin.mY = (MaxBox->mY + MinBox->mY)/2; 
  origin.mZ = (MaxBox->mZ + MinBox->mZ)/2; 

  tImaging<double>::MapIntoBox(m_pPos,Width,origin,mr);
}

void BMTWriter::ReimagePositions() {

        // do the atom-to-molecule bugfix reimage unconditionally

  XYZ *MinBox = &m_TrajectoryProperties.m_InitialBox.mMinBox;
  XYZ *MaxBox = &m_TrajectoryProperties.m_InitialBox.mMaxBox;

  XYZ Width = *MaxBox - *MinBox;
  XYZ Widthby2 = Width/2; // this is the allowable threshold distance 

  ConnectivityString cs;
  getAttrib("CONNECTIVITY",&cs);
  vector<MoleculeRun> mr = cs.parseConnectivityString();
  
  int start,end;
  start=end=0;
  int k=0;

  vector<MoleculeRun>::iterator it=mr.begin();
  for(;it!=mr.end();it++) {
    for(int i=0;i<it->m_Count;i++) {
      start = k;
      end = k + it->m_Size - 1;
      ReimageMolecule(m_pPos,start,end,Width,Widthby2);
      k+=it->m_Size;
    }
  }
}

void BMTWriter::ReimageMolecule(XYZ *pos,int start,int end,XYZ &L, XYZ &Dmax) {
  for(int i=start;i<end;i++ ) {
    XYZ d = pos[i+1] - pos[i];

    // Bring [i+1] closer to [i], if necessary 

    double dx = fabs(d.mX);
    double dy = fabs(d.mY);
    double dz = fabs(d.mZ);

    if(dx > Dmax.mX) { 
      ReimageCoords(d.mX,Dmax.mX,L.mX,&(pos[i+1].mX),pos[i].mX);
    }

    if(dy > Dmax.mY) { 
      ReimageCoords(d.mY,Dmax.mY,L.mY,&(pos[i+1].mY),pos[i].mY);
    }

    if(dz > Dmax.mZ) { 
      ReimageCoords(d.mZ,Dmax.mZ,L.mZ,&(pos[i+1].mZ),pos[i].mZ);
    }

  }

}

void BMTWriter::ReimageCoords(double dx, double DmaxX, double Lx,double *p, double xi) {

  double dx_abs = fabs(dx);
  if(dx_abs <= DmaxX) return;

  if(dx_abs <= Lx) { // still within box range
    if(dx > 0) *p -= Lx ;
    else *p += Lx ;
    return;
  }
  double rem = fmod(dx_abs , Lx);
  if(dx < 0 ) {
    *p = xi - rem;
    if(rem > DmaxX)  *p += Lx;
  }
  else {
    *p = xi + rem;
    if(rem > DmaxX)  *p -= Lx;
  }
}

string BMTWriter::DetermineTrajectoryFormatString(int precision,int velocities,bool recenter) {

  string s="";
  
        switch(precision) {
                case 32: s = TRAJ32BIT; break;
                case 64: s = TRAJ64BIT; break;
                default: s = TRAJ16BIT; break;
  }

  if(velocities==1) s = s + " " + TRAJVELOCITIES; 
  if(recenter) s = s + " " + TRAJRECENTER;
        return s;
}

int BMT::getPrecisionBits() {
  string s;
  getAttrib("TRAJECTORYFORMAT",&s);
  if(s.find("64")!=string::npos) return 64;
  else
  if(s.find("32")!=string::npos) return 32;
  else
  if(s.find("16")!=string::npos) return 16;

  cerr << "Warning: Invalid precision bits in : " << s << " !! " << endl;

  return 0;
}

bool BMT::hasFrameAttrib(string name,NameTypeHandle::BMTATTRIBUTETYPE *typAttr) {

        if(m_UIntFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::UINT;
                return true;
        }
        if(m_DoubleFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::DOUBLE;
                return true;
        }
        if(m_BoundingBoxFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::BOUNDINGBOX;
                return true;
        }
        if(m_XYZFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::TYPEXYZ;
                return true;
        }
        if(m_CharFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::CHAR;
                return true;
        }
        if(m_IntFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::INT;
                return true;
        }
        if(m_FloatFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::FLOAT;
                return true;
        }
        if(m_BoolFrameAttrib.has(name)) {
                *typAttr=NameTypeHandle::BOOL;
                return true;
        }
        *typAttr=NameTypeHandle::INVALID;
        return false;
}

void BMTWriter::CopyFrameAttributes(vector<NameTypeHandle> vec) {
  vector<NameTypeHandle>::iterator it1=vec.begin();
  vector<NameTypeHandle>::iterator it2=m_VecFrameAttributes.begin();
  
  assert(vec.size() == m_VecFrameAttributes.size());

  for(;(it1!=vec.end()) && (it1!=vec.end());it1++,it2++) {
          switch(it2->type)  {
            case NameTypeHandle::DOUBLE: 
        it2->ptr = (double *)it1->ptr; continue;
                  case NameTypeHandle::UINT:
        it2->ptr = (unsigned int *)it1->ptr; continue;
                  case NameTypeHandle::BOUNDINGBOX: 
        it2->ptr = (BoundingBox *)it1->ptr; continue;
                  case NameTypeHandle::TYPEXYZ: 
        it2->ptr = (XYZ *)it1->ptr; continue;
                  case NameTypeHandle::CHAR: 
        it2->ptr = (char *)it1->ptr; continue;
                  case NameTypeHandle::INT: 
        it2->ptr = (int *)it1->ptr; continue;
                  case NameTypeHandle::FLOAT:
        it2->ptr = (float *)it1->ptr; continue;
                  case NameTypeHandle::BOOL:
        it2->ptr = (bool *)it1->ptr; continue;
      default:	it2->ptr = NULL;
          }
  }
}

void BMTWriterRDG2BMT::ManageFrames() {
  if(m_FramesPerBMT<=0) return;

  if( (m_FrameCount++%m_FramesPerBMT) == 0) {
    cout << "Closing " << m_FileName << endl;
    cout << "Writing a new batch with FrameCount " << m_FrameCount ;
    cout << " and starting with CurrentStep " << m_CurrentStep;
    Close();
    char sfoo[MAXFILENAME];
    sprintf(sfoo,"_batch_%04d", m_FrameCount/m_FramesPerBMT);
                string  s = m_OrigFileName;
                string::size_type p1=s.find(BMTFILEEXTN);
                if(p1!=string::npos) {
                        s.insert(p1,(string) sfoo);
                        strcpy(m_FileName,s.c_str());
      cout << " as FileName " << m_FileName << " ... " << endl;
                }
                else {
                        sprintf(m_FileName,"%s%s%s",m_OrigFileName,sfoo,BMTFILEEXTN);
      cout << " as FileName " << m_FileName << " ... " << endl;
                }
    m_FirstFrame=true;
  }
}



double *BMTReader::getPositionsArray()  {

  if(m_pPos==NULL) {
    m_pPosArray = NULL;
    return m_pPosArray;
  }

  if(m_pPosArray==NULL) {
    m_pPosArray = new double[m_NEntries*3];
  }

  int i,j;
  for(i=0,j=0;i<m_NEntries;i++,j+=3) {
    m_pPosArray[j] = m_pPos[i].mX;
    m_pPosArray[j+1] = m_pPos[i].mY;
    m_pPosArray[j+2] = m_pPos[i].mZ;
  }
  return m_pPosArray;
}

double *BMTReader::getVelocitiesArray() {

  if(m_pVel==NULL) {
    m_pVelArray = NULL;
    return m_pVelArray;
  }

  if(m_pVelArray==NULL) {
    m_pVelArray = new double[m_NEntries*3];
  }

  int i,j;
  for(i=0,j=0;i<m_NEntries;i++,j+=3) {
    m_pVelArray[j] = m_pVel[i].mX;
    m_pVelArray[j+1] = m_pVel[i].mY;
    m_pVelArray[j+2] = m_pVel[i].mZ;
  }
  return m_pVelArray;
}


void BMTWriter::WriteReaderFrame(BMTReader *rd) {
        CopyFrameAttributes(rd->getFrameAttributeVector());
        setPositions(rd->getPositions());
        setVelocities(rd->getVelocities());
        WriteFrame();
}

bool BMT::IsCOMTrajectory() {
        string sTraj;
        getAttrib("TRAJECTORYFORMAT",&sTraj);
        string::size_type p1=sTraj.find(CENTEROFMASS);
        if(p1!=string::npos) return true;
  return false;
}

// Methods for BMTWriterBMT2COM

BMTWriterBMT2COM::BMTWriterBMT2COM(char *ifname,char *ofname, BMTReader *bmtr, XYZ *refCOM,int Precision) {
        setOutputFileName(ifname,ofname);
        InitFromBMTReader(ofname,*bmtr);
        InitWriterForCOM(refCOM,Precision);
}


void BMTWriterBMT2COM::setOutputFileName(char *ifname,char *ofname) {
        if(strcmp(ofname,"")==0) {
                // default output filename to : oldbmt_COM.bmt
                //      (assuming the old bmt file name is oldbmt.bmt or oldbmt)
                char *sfoo=COMFILESUFFIX;
                string  s=ifname;
                string::size_type p1=s.find(BMTFILEEXTN);
                if(p1!=string::npos) {
                        s.insert(p1,(string)sfoo);
                        strcpy(ofname,s.c_str());
                }
                else {
                        sprintf(ofname,"%s%s%s",ifname,sfoo,BMTFILEEXTN);
                }
        }
}

void BMTWriterBMT2COM::setTrajectoryFormatToCOM(int Precision) {

        string sTraj;

  if(IsCOMTrajectory()) {
                cerr << "Error: Cannot supply input BMT already with Center of Mass :";
    cerr << getAttrib("TRAJECTORYFORMAT",&sTraj) << endl;
                assert(false);
        }

        getAttrib("TRAJECTORYFORMAT",&sTraj);

  if(Precision<=0) {
    // inherit from input BMT
    string temp = sTraj + " " + CENTEROFMASS;
          setAttrib("TRAJECTORYFORMAT",temp);
    m_ConversionProperties.determineReductionType();
    return;
  }

  // just modify precision bits and insert center of mass tag

  int Vel=0;
  if(sTraj.find(TRAJVELOCITIES)!=string::npos) Vel=1;

  bool Recenter=false;
  if(sTraj.find(TRAJRECENTER)!=string::npos) {
    setRecenter(true);
    Recenter=true;
  }

  string temp = DetermineTrajectoryFormatString(Precision,Vel,Recenter);
        temp = temp + " " + CENTEROFMASS;
        setAttrib("TRAJECTORYFORMAT",temp);
  m_ConversionProperties.determineReductionType();
}

void BMTWriterBMT2COM::InitWriterForCOM(XYZ *refCOM,int Precision) {
        setTrajectoryFormatToCOM(Precision);

        // load system info for masses and connectivity string
        unsigned int sourceid;
        getAttrib("SOURCEID",&sourceid);
        loadSystemInfo(sourceid,&m_DatabaseProperties);

  if(getReductionType()==ConversionProperties::SHORT) {
    if(!m_pShortPos) m_pShortPos = new usXYZ[m_MolecularSystem.m_NAtoms];
  }

  m_bDiffusion=false;

  ConnectivityString cs;
  getAttrib("CONNECTIVITY",&cs);
  m_NMols = cs.getNumberOfMolecules();

  m_pCOM = new XYZ[m_NMols];
  if(!refCOM) m_bRef = false;
  else {
    m_bRef = true;
    for(int i=0;i<m_NMols;i++) m_pCOM[i] = refCOM[i];
  }

  m_pShortCOM = NULL;
  if(!getIncludeVelocities()) m_pVelCOM = NULL;
  else m_pVelCOM = new XYZ[m_NMols];

        XYZ *MinBox = &m_TrajectoryProperties.m_InitialBox.mMinBox;
        XYZ *MaxBox = &m_TrajectoryProperties.m_InitialBox.mMaxBox;
        m_Lby2 = (*MaxBox - *MinBox)/2;

  m_ForceWrite=true ;
}

int BMTWriterBMT2COM::WriteFrame()
{
        int rc = 1;

        if(SkipFrame()) {
                AdvanceCurrentStep();
                return 0;
        }

        ManageFrames();

        if (m_FirstFrame) {
            if(getRecenter()) {
                m_pRecenter = (XYZ *) getProteinCOMHandle();
            }

            setAttrib("INITIALSTEP",m_CurrentStep);
            if (m_DoAppend) OpenForAppend();
            else WriteHeader();

            m_FirstFrame = false;
        }

        ReimagePositions();

  ComputeCOM(m_pPos,m_pCOM,true);
  if(getIncludeVelocities()) ComputeCOM(m_pVel,m_pVelCOM,false);

        switch(m_ConversionProperties.m_ReductionType) {
            case ConversionProperties::SHORT:
                WriteFrameHeaderShort();
                rc = WriteFrameShort();
                break;
            case ConversionProperties::FLOAT:
            case ConversionProperties::DOUBLE:
                WriteFrameHeaderReal();
                rc = WriteFrameReal();
                break;
        }
        return rc;
}


int BMTWriterBMT2COM::WriteFrameShort()
{
  usXYZ *psp = m_pShortCOM;
        for (int i=0; i<m_NMols; i++, psp++) {
            for (int j=0; j<3; j++) {
                if (SwapIO::Write(m_File, &(*psp)[j]))
                    return 1;
            }
        }
        return 0;
}


int BMTWriterBMT2COM::WriteFrameHeaderShort() {
        WriteFrameAttributes();

        // Write origin and scaling factor
        XYZ origin, span;
        double maxspan;
        findBox(m_pCOM, 0, m_NMols, origin, span, maxspan);
        double factor = 65535.0/maxspan;
        float recipfactor = maxspan/65535;
        if(!m_pShortCOM) m_pShortCOM = new usXYZ[m_NMols];
        simplifySites(m_pCOM, m_pShortCOM, 0, m_NMols, origin, factor);

        float x,y,z;
        x = (float)origin.mX;
        y = (float)origin.mY;
        z = (float)origin.mZ;
        if (SwapIO::Write(m_File, &x))
            return 1;
        if (SwapIO::Write(m_File, &y))
            return 1;
        if (SwapIO::Write(m_File, &z))
            return 1;
        if (SwapIO::Write(m_File, &recipfactor))
            return 1;

        return 0;
}

// psp can point to either m_pPos or m_pVel
// if m_pPos:	values written are COM for each molecule
// if m_pVel:	values written are the velocities of the COM for each molecule 
//		ie., sum of momentum for all atoms / molecular mass


int BMTWriterBMT2COM::WriteFrameReal(XYZ *pp)
{
        ConversionProperties::REDUCTIONTYPE rt = getReductionType();

        double dv;
        float fv;

        for (int i=0; i<m_NMols; i++, pp++) {
            for (int j=0; j<3; j++) {
                dv = (*pp)[j];
                fv = (float)dv;
                if (rt == ConversionProperties::FLOAT) {
                    if (SwapIO::Write(m_File, &fv)) {
                        cout << "Warning: WriteFrameReal Write() wrote probably less number of bytes \n";
                        return 1;
                    }
                } else {
                    if (SwapIO::Write(m_File, &dv)) {
                        cout << "Warning: WriteFrameReal Write() wrote probably less number of bytes \n";
                        return 1;
                    }
                }
            }
        }
        return 0;
}



void BMTWriterBMT2COM::UndoMapIntoBox(XYZ &com,int ref_k) {

  if(!m_bDiffusion) return;

  if(!m_bRef) return; // first frame could have an empty reference

  XYZ *r = &(m_pCOM[ref_k]); // access old reference COM

  if(ref_k==0) {
    // cout << " Before com: [ " << com.mX << " " << com.mY << " " << com.mZ << " ] " << endl;
    //cout << " Before ref: [ " << r->mX << " " << r->mY << " " << r->mZ << " ] " << endl;
  }

  com.mX = Unmap(com.mX,r->mX,m_Lby2.mX);
  com.mY = Unmap(com.mY,r->mY,m_Lby2.mY);
  com.mZ = Unmap(com.mZ,r->mZ,m_Lby2.mZ);

  if(ref_k==0) {
    // cout << " After com: [ " << com.mX << " " << com.mY << " " << com.mZ << " ] " << endl;
  }

}

// Inverse function for MapIntoBox:
// this code is awfully similar to the bug-fix code in ReimagePositions because 
// 	all it takes is to bring curr as close as possible to ref 
// 	without worrying about box boundaries

double BMTWriterBMT2COM::Unmap(double curr, const double ref,const double Lby2) {
  double x = curr - ref;
  double a = fabs(x);

  if( a < Lby2)  return curr;

  double L = Lby2*2;

        if(a <= L) { // still within box range
                if(curr > ref) return (curr-L);
                return (curr+L);
        }

        double rem = fmod(a , L);
        if(curr < ref ) {
                double ret = ref - rem;
                if(rem >= Lby2)  ret += L;
    return ret;
        }
  // else
        double ret = ref + rem;
        if(rem >= Lby2)  ret -= L;
  return ret;
}



void BMTWriterBMT2COM::ComputeCOM(XYZ *p,XYZ *pCOM, bool bPositions) {
  
        int atomit=0;
  int molit=0;

  XYZ adjustCOM=XYZ::ZERO_VALUE();

  double *masses = getMassesForSystem();

  vector<MoleculeRun> *mr = m_MolecularSystem.m_ConnectivityString.getMRHandle();
        vector<MoleculeRun>::iterator it=mr->begin();
        for(;it!=mr->end();it++) { // for each molecule "block"
                int nMol = it->m_Count;
                int nAtoms = it->m_Size;
                // first sum up the masses
    int massit=atomit;
                double molMass=0;
                for(int j=0;j<nAtoms;j++) {
                  molMass+=masses[massit++];
                }
                for(int i=0;i<nMol;i++,molit++) { // for each molecule 
      XYZ numerator=XYZ::ZERO_VALUE();

                        for(int j=0;j<nAtoms;j++,atomit++) {
                          numerator.mX = numerator.mX + p[atomit].mX * masses[atomit];
                          numerator.mY = numerator.mY + p[atomit].mY * masses[atomit];
                          numerator.mZ = numerator.mZ + p[atomit].mZ * masses[atomit];
      }
      XYZ com = numerator/molMass;
      if(bPositions) UndoMapIntoBox(com,molit);
      if(bPositions && (molit==0) && (nMol==1) && getRecenter()) {
        // first protein molecule
        assert(m_pRecenter!=NULL);
        pCOM[molit]= XYZ::ZERO_VALUE();
        adjustCOM.mX = com.mX;
        adjustCOM.mY = com.mY;
        adjustCOM.mZ = com.mZ;
        *m_pRecenter += adjustCOM;
        continue;
      }

      if(bPositions && molit>0 && getRecenter()) {
        pCOM[molit] = com - adjustCOM;
        continue;
      }
      pCOM[molit] = com; // this will also be the reference for next frame
    }
  }
  m_bRef=true;
}    

int BMTWriterBMT2COM::WriteFrameReal() {

  WriteFrameReal(m_pCOM);

        if(!getIncludeVelocities()) return 0;
  WriteFrameReal(m_pVelCOM);

  return 0;
}

void BMTWriter::RecenterOnProtein() {
  if(!getRecenter()) return;

        vector<MoleculeRun> *mr = m_MolecularSystem.m_ConnectivityString.getMRHandle();
  vector<MoleculeRun>::iterator it = mr->begin();
  
  // assumes protein is always the first molecule entry in the connectivity string
  assert(it!=mr->end());
  assert(it->m_Count==1);

  int nAtoms = it->m_Size;

        int atomit=0;
        int massit=0;
  int molit=0;

  double *masses = getMassesForSystem();

        // first sum up the masses
        double molMass=0;
        for(int j=0;j<nAtoms;j++) {
          molMass+=masses[massit++];
        }
        XYZ numerator=XYZ::ZERO_VALUE();

        for(int j=0;j<nAtoms;j++,atomit++) {
          numerator.mX = numerator.mX + m_pPos[atomit].mX * masses[atomit];
                numerator.mY = numerator.mY + m_pPos[atomit].mY * masses[atomit];
                numerator.mZ = numerator.mZ + m_pPos[atomit].mZ * masses[atomit];
        }
        XYZ com = numerator/molMass;

  // this is similar to acquireframeattrib for the ProteinCOM entry
  assert(m_pRecenter!=NULL);
  m_pRecenter->mX = com.mX ;
  m_pRecenter->mY = com.mY ;
  m_pRecenter->mZ = com.mZ ;

  // Recenter the whole system now with com ==> (0,0,0)
  RecenterSystem(com);

}


void BMTWriter::RecenterSystem(XYZ &new_origin) {
  for(int i=0;i< m_MolecularSystem.m_NAtoms;i++) {
    m_pPos[i] = m_pPos[i] - new_origin;
  }
}

XYZ *BMTWriter::getProteinCOMHandle() {
  
        vector<NameTypeHandle>::iterator it=m_VecFrameAttributes.begin();

        for(;it!=m_VecFrameAttributes.end();it++) {
                if(it->name=="ProteinCOM") {
      return ((XYZ *) (it->ptr));
    }
        }

  return NULL;
}
