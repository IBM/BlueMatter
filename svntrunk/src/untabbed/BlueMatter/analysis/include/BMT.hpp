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
 #ifndef BMT_HPP
#define BMT_HPP

#include <cmath>
#include <fcntl.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ostream>
#include <fstream>

#include <map>
#include <string>
#include <assert.h>
#include <string.h> // for memcpy

#include <errno.h>

#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/ExternalDatagram.hpp> // for tSiteData
#include <BlueMatter/Frame.hpp>
#include <BlueMatter/MolecularSystem.hpp>

// AK Change: New includes
#include <BlueMatter/TrajectoryProperties.hpp>
#include <BlueMatter/RDGProperties.hpp>
#include <BlueMatter/ConversionProperties.hpp>
#include <BlueMatter/Imaging.hpp> // for reimaging

// AK Change: Use Trajector starting delimiter depending on the BMT version

#define BEGINTRAJ_VER1 "TRAJECTORYFORMAT"
#define BEGINTRAJ_VER2 "BEGINTRAJECTORY"
#define BEGINFRAMEFORMAT "BEGINFRAMEFORMAT" // this is only in version 2 and above 
#define BMTCOMMENTLINE "#"
// maximum allowed size of the value of an attribute in bytes
#define ATTRIBUTEVALUESIZE 1000 
#define NUMBEROFFRAMEFORMATATTRIBUTES 6 
#define BMTFILEEXTN  ".bmt"
#define COMFILESUFFIX  "_COM"
#define CENTEROFMASS "CENTEROFMASS"

using namespace std ;

// extern int errno;

// AK: New class to encapsulate the attributes of a BMT 
// Some of them might point to member variables of the BMT class 
// 	and its dependent classes;
//	But remaining might point to user-specified new attributes

template <class T> class Attrib : public map<string,T *> {
protected:
        //attribute-value pair (container)
        //map<string,T *> m;
        void init() { clear(); }
public:
        Attrib() { init(); }
        virtual void bind(string name, T *addrVal);
        virtual void set(string name,const T val);
        virtual T get(string name);
        virtual bool has(string name);
        virtual void print();
};

template <class T> class FrameAttrib : public Attrib<T> {
public:
  virtual void *createHandle(string name);
        virtual T *getHandle(string name);
};
 


class NameTypeHandle {
public:
        enum BMTATTRIBUTETYPE {CHAR, INT, UINT, FLOAT, DOUBLE, STRING, BOOL, BOUNDINGBOX, TYPEXYZ, CONNECTIVITYSTRING, ALL, INVALID};

  string name;
  NameTypeHandle::BMTATTRIBUTETYPE type;
  void *ptr;

  NameTypeHandle() ;

  NameTypeHandle(string n,BMTATTRIBUTETYPE t,void *handle) ;

  static string getTypeStr(BMTATTRIBUTETYPE type);

  int writeTo(FILE *fp) ;
  static BMTATTRIBUTETYPE getTypeFromStr(string typstr);

        NameTypeHandle& operator=(const NameTypeHandle& aOther);
};




class NameTypePair {
public:
  string name;
  string type;
};

// AK: New class for BMT version 2.0

class BMT
{
protected:
    enum {MAXFILENAME=2048};
    enum {BUFFSIZE=2048};

    string m_Version;
    int	   m_NoWaters;
    int    m_FrameSize;
    int    m_FileSize;
    int    m_NFrames;
    int    m_DataSeekStart;
    int    m_StartAtomIndex;
    int    m_EndAtomIndex;
    string	m_ByteOrder;

    // AK: new member variables:
    // collection of BMT attributes classified into types
    // the union of the contents of these following map attributes
    //		gives the set of all attributes both inside the 
    //		BMT class and its dependent classes, and also 
    //		user-specified new attributes

    Attrib<char> m_CharAttrib;
    Attrib<int> m_IntAttrib;
    Attrib<unsigned int> m_UIntAttrib;
    Attrib<float> m_FloatAttrib;
    Attrib<double> m_DoubleAttrib;
    Attrib<string> m_StringAttrib;
    Attrib<bool> m_BoolAttrib;
    Attrib<BoundingBox> m_BoundingBoxAttrib;
    Attrib<ConnectivityString> m_ConnectivityStringAttrib;


    // AK: Currently only Double Frame attributes are a reality
    //		but this provides supports for other types 
    //		of frame attributes

    FrameAttrib<char> m_CharFrameAttrib;
    FrameAttrib<int> m_IntFrameAttrib;
    FrameAttrib<unsigned int> m_UIntFrameAttrib;
    FrameAttrib<float> m_FloatFrameAttrib;
    FrameAttrib<double> m_DoubleFrameAttrib;
    FrameAttrib<bool> m_BoolFrameAttrib;
    FrameAttrib<BoundingBox> m_BoundingBoxFrameAttrib;
    FrameAttrib<XYZ> m_XYZFrameAttrib;
    
    // AK : Add association member variables
    MolecularSystemProperties m_MolecularSystem;
    TrajectoryProperties m_TrajectoryProperties;
    ConversionProperties m_ConversionProperties;
    RDGProperties m_RDGProperties;

    DatabaseProperties m_DatabaseProperties;


    vector<NameTypeHandle> m_VecFrameAttributes;

    double *m_pPosArray;
    double *m_pVelArray;

public:

    // AK : variables for internal use
    int m_NetSnapshotPeriodInOTS;
    double m_NetTimeStepInPicoSeconds;
    bool   m_FirstFrame;  	// first frame to be written in the BMT 
        // PS: could be different from first frame supplied 
        //	from a remote source (like from RDG)
    char   m_Buff[BUFFSIZE];

    char      m_FileName[MAXFILENAME];
    ofstream  m_OutStream;
    ifstream  m_InStream;
    FILE     *m_File;

    XYZ   *m_pPos;
    XYZ   *m_pVel;
    usXYZ *m_pShortPos;


    // AK : New access methods for the attributes

    BMT() ; 

    virtual void bindKnownAttributes();

   // AK: The following code does not use templates because
   //		mainly for user-friendliness.. the user of the
   //		code does not need to know template programming.
   //		The other reason is that including templates may necessitate
   //		binding the corresponding objects using static variables
   //		in the attribute class, while there could be multiple
   //		instances of the BMT/attribute class in a BMT client code

    void setAttrib(string name,const char val) {
  m_CharAttrib.set(name,val);
    }

    void setAttrib(string name,const int val) {
  m_IntAttrib.set(name,val);
    }

    void setAttrib(string name,const unsigned int val) {
  m_UIntAttrib.set(name,val);
    }

    void setAttrib(string name,const float val) {
  m_FloatAttrib.set(name,val);
    }

    void setAttrib(string name,const double val) {
  m_DoubleAttrib.set(name,val);
    }

    void setAttrib(string name,const string val) {
  m_StringAttrib.set(name,val);
    }

    void setAttrib(string name,const bool val) {
  m_BoolAttrib.set(name,val);
    }

    void setAttrib(string name,const BoundingBox val) {
  m_BoundingBoxAttrib.set(name,val);
    }

    void setAttrib(string name,const ConnectivityString val) {
  m_ConnectivityStringAttrib.set(name,val);
    }

    // get attribute functions

    int getAttrib(string name,char *val) {
  return (*val = m_CharAttrib.get(name));
    }

    int getAttrib(string name,int *val) {
  return (*val = m_IntAttrib.get(name));
    }

    unsigned int getAttrib(string name,unsigned int *val) {
  return (*val = m_UIntAttrib.get(name));
    }

    float getAttrib(string name,float *val) {
  return (*val = m_FloatAttrib.get(name));
    }

    double getAttrib(string name,double *val) {
  return (*val = m_DoubleAttrib.get(name));
    }

    string getAttrib(string name,string *val) {
  return (*val = m_StringAttrib.get(name));
    }

    bool getAttrib(string name,bool *val) {
  return (*val = m_BoolAttrib.get(name));
    }

    BoundingBox getAttrib(string name,BoundingBox *val) {
  return (*val = m_BoundingBoxAttrib.get(name));
    }

    ConnectivityString getAttrib(string name,ConnectivityString *val) {
  return (*val = m_ConnectivityStringAttrib.get(name));
    }

    // has attribute functions

    bool hasAttrib(string name,NameTypeHandle::BMTATTRIBUTETYPE *typAttr);

    // clear attribute functions
    void printAttrib(NameTypeHandle::BMTATTRIBUTETYPE typAttr);

    // clear attribute functions
    void clearAttrib(NameTypeHandle::BMTATTRIBUTETYPE typAttr);

    // parse connectivity string to build fragment list, which 
    // is just an array of fragment lengths
    void addFragments();

    // This finds the true bounding box of the Pos, for use in data compression
    static inline void findBox(XYZ *Pos, const int Start, const int Count, XYZ &origin, XYZ &span, double &MaxSpan);
    
    // This simplfies the sites from double to usXYZ, using the bounding box
    static inline void simplifySites(XYZ *Pos, usXYZ *psp, const int Start, const int Count, const XYZ &origin, const double factor);
    
    // return full precision positions
    XYZ *getPositions() { return m_pPos; }
    void setPositions(XYZ *p) { m_pPos = p; }

    XYZ *getVelocities() { return m_pVel; }
    void setVelocities(XYZ *p) { m_pVel= p; }


    // get methods for some protected variables
    inline ConversionProperties::REDUCTIONTYPE getReductionType() {
  return m_ConversionProperties.m_ReductionType;
    }

    double getInitialSimulationTimeInNanoSeconds() {
  return m_TrajectoryProperties.m_InitialSimulationTimeInNanoSeconds;
    }

    bool getMapIntoBox() {
  return m_TrajectoryProperties.getMapIntoBox();
    }

    void setMapIntoBox(bool b) {
  m_TrajectoryProperties.setMapIntoBox(b);
    }

    float getBoxWidth() {
  return m_TrajectoryProperties.m_BoxWidth;
    }

    BoundingBox getInitialBox() {
  return m_TrajectoryProperties.m_InitialBox;
    }

    int getNAtoms() {
  return m_MolecularSystem.m_NAtoms;
    }

    double getNetTimeStepInPicoSeconds() {
  return m_NetTimeStepInPicoSeconds;
    }

    bool getIncludeVelocities() {
  return m_ConversionProperties.m_IncludeVelocities;
    }

    void setIncludeVelocities(bool b) {
  m_ConversionProperties.m_IncludeVelocities=b;
    }

    bool IsCOMTrajectory();

    virtual void *setupNewFrameAttrib(char *sAttrName,char *sTypeName);

    virtual void *setupNewFrameAttrib(string sAttrName,string sTypeName);

    virtual vector<NameTypeHandle> getFrameAttributeVector() {
  return m_VecFrameAttributes;
    }

    virtual void PrintFrameAttributes();

    virtual vector<NameTypePair> getFrameNameTypePair();

    virtual int determineFrameSize();

    virtual int determineFrameHeaderSize();


    static int SizeOf(string type);

    static void ReadAndPrintTypedData(FILE *f,string type);

    template <class T> static inline void ReadTypedData(FILE *f,T *data);

    // wrapper function
    void loadSystemInfo(int sourceid=0,DatabaseProperties *db=NULL){
         m_MolecularSystem.getSystemInfo(sourceid,db);
    }

    inline double *getMassesForSystem() {
  return m_MolecularSystem.getMasses();
    }

    unsigned int getFrameAttrib(string name,unsigned int *val) {
        return (*val = m_UIntFrameAttrib.get(name));
    }

    double getFrameAttrib(string name,double *val) {
        return (*val = m_DoubleFrameAttrib.get(name));
    }

    BoundingBox getFrameAttrib(string name,BoundingBox *val) {
        return (*val = m_BoundingBoxFrameAttrib.get(name));
    }

    bool hasFrameAttrib(string name,NameTypeHandle::BMTATTRIBUTETYPE *typAttr);

    int getPrecisionBits();

};

class BMTReader : public BMT
{
protected:
     fXYZ m_Origin; // valid only for 16-bit rescale trajectories
    //float m_x0,m_y0,m_z0;
    float m_RecipFactor; // valid only for 16-bit rescale trajectories
    int m_NEntries; // keeps track of the number of entries in the BMT per frame
      // generic enough to work for both usual and COM BMTs

    int readAttributeFromFrame(NameTypeHandle nt);

    int LoadXYZBox(XYZ *p);
 
    int LoadXYZ(XYZ *p, ConversionProperties::REDUCTIONTYPE rt);

    int SeekFrame(int i);

public:
    BMTReader(char *fname);
    ~BMTReader();

    int ReadHeader();
    
    int ReadFrameShort();

    int ReadFrameReal(ConversionProperties::REDUCTIONTYPE );

    int ReadFrame();

    int ReadFrame(int FrameNumber);

    int ReadLastFrame() {
        return ReadFrame(-1);
    }

    int getNFrames() {
  return m_NFrames;
    }

    void Rewind() {
  SeekFrame(0);
    }

    void closeFileHandle() { fclose(m_File); }

    inline fXYZ getOrigin() { 
  return m_Origin;
    }

    inline float getRecipFactor() {
  return m_RecipFactor;
    }

    int ReadFrameBySimulationTime(double SimTime) ;

    int ReadFrameByTimeStep(int TimeStep);

    virtual int determineNEntries();

    virtual int getNEntries() {
  return m_NEntries;
    }

    double *getPositionsArray() ;
    double *getVelocitiesArray() ;
  
};

class BMTWriter : public BMT
{
protected:
    // 		if a BMTWriter is constructed from a BMTReader obj then 
    //		the user can specify a reduced skip factor when compared
    //		to whats already in the BMTReader obj
    //		eg., if BMTSKIPFACTOR=10 in BMTReader, and the user wants
    //		the new_BMTSKIPFACTOR=20, then the effective SkipFactor=2

    int m_OldSkipFactor;// default is same as m_ConversionProperties.m_SkipFactor
      // equals the skip factor of the frame source (eg., another BMT)
      // for RDG inputs, this is always equal to 1

    int m_CurrentStep;

    bool m_ForceWrite;

    bool m_bRecenter;
    XYZ *m_pRecenter;

    void InitFromBMTReader(char *fname, BMTReader &bmtr);

    virtual int WriteFrameVelocities() {
  return 0;
    }

    virtual void ReimageCoords(double dx, double DmaxX, double Lx,double *p, double xi);

public:
    bool m_DoAppend;

    virtual void InitDefault();

    BMTWriter() { 
  InitDefault(); 
  // BindNewFrameFormatVector();
    }

    BMTWriter(char *fname);

    BMTWriter(char *fname, BMTReader &bmtr);

    BMTWriter(char *fname, BMTReader &bmtr, int newSkipFactor);

    
    void setParams(char *BMTFileName, char *connectivity, int DoAppend, int NoWaters, int SimulationStart, int Reverse,int SkipFactor,int PrecisionSpecifier,int Velocities,bool MapIntoBox,bool Recenter);

    void setParams(char *BMTFileName, char *RDGFileName, MolecularSystemProperties &ms, int sourceid=0) {}

    // for backward compatibility (remove once you fix rdg2dx caller )
    void setParamsOld(char *BMTFileName, char *PDBFileName, char *connectivity, int NNonWaterAtoms, int NWaters, int DoAppend, int NoWaters, int SimulationStart, int Reverse,int SkipFactor,int PrecisionSpecifier,int Velocities);

    virtual void OpenForAppend();

    virtual void WriteHeader();

    virtual void BindOldFrameFormatVector(vector<NameTypeHandle> ); 

    // AK: implement in base class for any specific energy, temperature
    // frame format attributes
    virtual void BindNewFrameFormatVector(Frame *f) {
    }

    virtual void PrintFrameFormatInBMTHeader();

    virtual void AcquireFrameAttributes(Frame *f) {}


    virtual int WriteFrameAttributes();

    virtual int WriteFrameReal();
    
    virtual int WriteFrameShort();

    virtual int WriteFrame();

    virtual int WriteFrameHeaderShort();

    virtual int WriteFrameHeaderReal();

    // AK_old_code - kept for backward compatibility

    virtual int Close();

#if 0
    virtual int WriteFrame(tSiteData *ps, int NSites, unsigned int CurrentStep);
#endif

    void ReimagePositions();

    void ReimageMolecule(XYZ *pos,int start,int end,XYZ &L, XYZ &Dmax);

    void RecenterOnProtein();

    void MapIntoBoxPositions();

    string DetermineTrajectoryFormatString(int precision=16,int velocities=0,bool Recenter=false);

    void CopyFrameAttributes(vector<NameTypeHandle> vec);

    int getCurrentStep() { return m_CurrentStep; }
    
    virtual void AdvanceCurrentStep();

    virtual bool SkipFrame();

    virtual void ManageFrames() {}

    void setForceWrite(bool b) {
  m_ForceWrite = b;
    }

    bool getForceWrite() {
  return m_ForceWrite ;
    }

    void setDoAppend(bool b) {
  m_DoAppend = b;
    }

    bool getDoAppend() {
  return m_DoAppend ;
    }

    void setRecenter(bool b) {
  m_bRecenter=b;
    }

    bool getRecenter() {
  return m_bRecenter;
    }

    void RecenterSystem(XYZ &new_origin);

    void WriteReaderFrame(BMTReader *bmtr);

    XYZ *getProteinCOMHandle();
    
};

// AK: special subclass of BMTWriter that will be instantiated only from
//		rdg2bmt/DataReceiverTrajectorDumper module

class BMTWriterRDG2BMT : public BMTWriter {
protected:

    int m_FrameCount;

    int m_FramesPerBMT; // a default value of 0 means all frames from RDG into one BMT file

    bool   m_FirstRDGFrame; 	// first frame coming from RDG 
        // (this will point to the InitialStep in the BMT)

    char m_OrigFileName[MAXFILENAME];

    virtual void BindNewFrameFormatVector(Frame *f);

    virtual void AcquireFrameAttributes(Frame *f);

    // virtual int WriteFrameAttributes();

    virtual int WriteFrameVelocities();

    virtual void AddCurrentFrameAttributes(Frame *f);

    virtual void InitWriterRDG2BMT() {
  m_FrameCount=0;
  m_FirstRDGFrame=true;
  m_OldSkipFactor=1;
    }


public:

    virtual int WriteFrame(Frame *f);

    BMTWriterRDG2BMT() { 
  InitWriterRDG2BMT();
    }

    BMTWriterRDG2BMT(char *fname) { 
  InitWriterRDG2BMT();
  strcpy(m_OrigFileName,fname);
    }

    void setParams(char *BMTFileName, char *connectivity, int DoAppend, int NoWaters, int SimulationStart, int Reverse,int SkipFactor,int PrecisionSpecifier,int Velocities,bool MapIntoBox, bool Recenter, int FramesPerBMT);

    virtual void ManageFrames();

};


class BMTWriterBMT2COM : public BMTWriter {
protected:
    vector<MoleculeRun> *m_mr;

    bool m_bDiffusion;

    XYZ *m_pCOM;
    usXYZ *m_pShortCOM;
    XYZ *m_pVelCOM;

    int m_NMols;
    bool m_bRef;
    XYZ m_Lby2;

    void setOutputFileName(char *ifname,char *ofname);

    void setTrajectoryFormatToCOM(int Precision);
    
    void InitWriterForCOM(XYZ *,int Precision);

    void UndoMapIntoBox(XYZ &com,int ref_k);

    double Unmap(double curr, const double ref,const double Lby2);


public:

    BMTWriterBMT2COM(char *ifname,char *ofname="", BMTReader *bmtr=NULL,XYZ *refCOM=NULL,int Precision=32);

    void setDiffusionFlag(bool b) {
  m_bDiffusion=b;
    }

    bool getDiffusionFlag() {
  return m_bDiffusion;
    }

    virtual void ComputeCOM(XYZ *input,XYZ *output,bool);

    virtual int WriteFrame();

    virtual int WriteFrameShort();

    virtual int WriteFrameHeaderShort();
    
    virtual int WriteFrameReal();

    virtual int WriteFrameReal(XYZ *psp);

};

ostream& operator<<(ostream& os, const NameTypeHandle &nth) {
        NameTypeHandle::BMTATTRIBUTETYPE type;

  type = nth.type;

        switch(type)  {
                case NameTypeHandle::DOUBLE: 	
      os << nth.name << " " << *((double *)(nth.ptr)); break;
                case NameTypeHandle::CHAR:
      os << nth.name << " " << *((char *)(nth.ptr)); break;
                case NameTypeHandle::INT:
      os << nth.name << " " << *((int *)(nth.ptr)); break;
                case NameTypeHandle::UINT: 
      os << nth.name << " " << *((unsigned int *)(nth.ptr)); break;
                case NameTypeHandle::FLOAT:
      os << nth.name << " " << *((float *)(nth.ptr)); break;
                case NameTypeHandle::BOOL:
      os << nth.name << " " << *((bool *)(nth.ptr)); break;
                case NameTypeHandle::BOUNDINGBOX:
      os << nth.name << " " << *((BoundingBox *)(nth.ptr)); break;
                case NameTypeHandle::TYPEXYZ:
      os << nth.name << " " << *((XYZ *)(nth.ptr)); break;
        }
        return os;
}


#endif
