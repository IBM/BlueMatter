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
#ifdef WIN32
#include <io.h>
#define SWAP_IN
#define SWAP_OUT
#endif
#include <iostream>
#include <ostream>
#include <fstream>
#include <string.h>

#define BUFFSIZE 2048
#define MAXATOMS 200000

#define BEGINTRAJ "TRAJECTORYFORMAT"

using namespace std;

struct XYZShort{
    unsigned short mX;
    unsigned short mY;
    unsigned short mZ;
};

class XYZ {
public:
    float mX;
    float mY;
    float mZ;
    
    XYZ operator-=(const XYZ &other)
    {
	mX -= other.mX;
	mY -= other.mY;
	mZ -= other.mZ;
	return (*this);
    }

    XYZ operator+=(const XYZ &other)
    {
	mX += other.mX;
	mY += other.mY;
	mZ += other.mZ;
	return (*this);
    }

    XYZ operator/=(const float other)
    {
	mX /= other;
	mY /= other;
	mZ /= other;
	return (*this);
    }
    
    XYZ operator*=(const float other)
    {
	mX *= other;
	mY *= other;
	mZ *= other;
	return (*this);
    }


    XYZ operator-(const XYZ &other)
    {
	XYZ r;
	r.mX = mX - other.mX;
	r.mY = mY - other.mY;
	r.mZ = mZ - other.mZ;
	return r;
    }

    XYZ operator+(const XYZ &other)
    {
	XYZ r;
	r.mX = mX + other.mX;
	r.mY = mY + other.mY;
	r.mZ = mZ + other.mZ;
	return r;
    }
    
    XYZ operator/(const float d)
    {
	XYZ r;
	r.mX = mX/d;
	r.mY = mY/d;
	r.mZ = mZ/d;
	return r;
    }

    float SquaredMag() const
    {
	return mX*mX + mY*mY + mZ*mZ;
    }

    float Mag() const
    {
	return sqrt(SquaredMag());
    }
};

inline XYZ operator-(const XYZ &a, const XYZ &b)
{
    XYZ r;
    r.mX = a.mX - b.mX;
    r.mY = a.mY - b.mY;
    r.mZ = a.mZ - b.mZ;
    return r;
}

inline XYZ operator+(const XYZ &a, const XYZ &b)
{
    XYZ r;
    r.mX = a.mX + b.mX;
    r.mY = a.mY + b.mY;
    r.mZ = a.mZ + b.mZ;
    return r;
}

inline XYZ operator/=(const XYZ &a, const float &b)
{
    XYZ r;
    r.mX = a.mX/b;
    r.mY = a.mY/b;
    r.mZ = a.mZ/b;
    return r;
}

static XYZ XYZZero = {0, 0, 0};

inline float SquaredMag(const XYZ &a)
{
    return a.SquaredMag();
}

inline float SquaredDistance(const XYZ &a, const XYZ &b)
{
    XYZ d = a-b;
    return d.SquaredMag();
}

inline float Distance(const XYZ &a, const XYZ &b)
{
    XYZ d = a-b;
    return sqrt(d.SquaredMag());
}


inline void Swap(char *c, int j, int k) {
    char t = c[j];
    c[j] = c[k];
    c[k] = t;
}

inline void Swap_2(char *c) {
    Swap(c, 0, 1);
}

inline void Swap_4(char *c) {
    Swap(c, 0, 3);
    Swap(c, 1, 2);
}

inline void Swap_8(char *c) {
    Swap(c, 0, 7);
    Swap(c, 1, 6);
    Swap(c, 2, 5);
    Swap(c, 3, 4);
}

inline void Swap(float *f) {
    Swap_4((char *)f);
}

inline void Swap(double *d) {
    Swap_8((char *)d);
}

inline void Swap(int *i) {
    Swap_4((char *)i);
}

inline void Swap(unsigned short *i) {
    Swap_2((char *)i);
}

inline void Swap(XYZShort *v) {
    Swap(&v->mX);
    Swap(&v->mY);
    Swap(&v->mZ);
}

inline void Swap(XYZ *v) {
    Swap(&v->mX);
    Swap(&v->mY);
    Swap(&v->mZ);
}


template <class T> int GenRead(int f, T *pt) {
    int NumToRead = sizeof(T);
    if (NumToRead != _read(f, pt, NumToRead))
	return 1;
#ifdef SWAP_IN
    Swap(pt);
#endif
    return 0;
}

template <class T> int GenRead(FILE *f, T *pt) {
    int NumToRead = sizeof(T);
    if (NumToRead != fread(pt, 1, NumToRead, f))
	return 1;
#ifdef SWAP_IN
    Swap(pt);
#endif
    return 0;
}

template <class T> int GenWrite(int f, T *pt) {
    T p = *pt;
#ifdef SWAP_OUT
    Swap(&p);
#endif
    int NumToWrite = sizeof(T);
    if (NumToWrite != _write(f, p, NumToWrite))
	return 1;
    return 0;
}

template <class T> int GenWrite(FILE *f, T *pt) {
    T p = *pt;
#ifdef SWAP_OUT
    Swap(&p);
#endif
    int NumToWrite = sizeof(T);
    if (NumToWrite != fwrite(&p, 1, NumToWrite, f))
	return 1;
    return 0;
}

char PDBFileName[256];
char InBMTFileName[512];
char OutBMTFileName[512];
ofstream OutStream;
ifstream InStream;
FILE *OutFile;
FILE *InFile;

int  NInWaters = 0;
int  NInWaterAtoms = 0;
int  NNonWaterAtoms = 0;
int  NInAtoms = 0;
int  NOutWaters = -1;
int  NOutAtoms = 0;
int  NoInWaters = 0;

float BoxWidth;
int  MapToBox = 0;
char buff[BUFFSIZE];
XYZ *pInPos;
XYZShort *pOutShortPos;
XYZ *pOutPos;
float *pWaterDistance;
int *pSortedWater;

// These define a triangle strip.  Order is important
const int RefIndices[] = {114, 90, 160, 55, 194, 16, 224};
const int NRefIndices = sizeof(RefIndices)/sizeof(int);
// Number of triangles is 2 less than number of vertices
// Each ref point is the mean of its triangle vertices
const int NRefPoints = NRefIndices - 2;
XYZ RefPoints[NRefPoints];
// The mean ref points is used as the periodic imaging pivot point
XYZ RefPointsMean;

void findBox(XYZ *Pos, int Start, int Count, XYZ &origin, XYZ &span, double &MaxSpan)
{
    XYZ LL, UR;
    XYZ *ps = &Pos[Start];

    LL.mX = LL.mY = LL.mZ = (float)1.0E20;
    UR.mX = UR.mY = UR.mZ = (float)-1.0E20;
    
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

void simplifySites(XYZ *Pos, XYZShort *psp, int Start, int Count, XYZ &origin, double factor)
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

void WriteHeader()
{
    OutStream.open(OutBMTFileName);
    if (!OutStream.is_open()) {
	cerr << "BMT output file " << OutBMTFileName << " could not be open." << endl;
	exit(-1);
    }
    OutStream << "BMT" << endl;
    OutStream << "VERSION 1.00" << endl;
    OutStream << "STRUCTURE " << PDBFileName << endl;
    OutStream << "BONDS NULL" << endl;
    OutStream << "NUMNONWATERATOMS " << NNonWaterAtoms << endl;
    OutStream << "NUMWATERS " << NOutWaters << endl;
    OutStream << "NOWATERS " << 0 << endl;
    OutStream << "STARTINDEX " << 0 << endl;
    OutStream << "ENDINDEX " << NOutAtoms << endl;
    OutStream << "TRAJECTORYFORMAT 16-bit RESCALE RECENTER" << endl;
    OutStream.close();
    
    // close text mode and switch to binary, appending at end
    OutFile = fopen(OutBMTFileName, "a+b");
    if (!OutFile) {
	cerr << "BMT output file " << OutBMTFileName << " could not be open for binary write/append." << endl;
	exit(-1);
    }
}

int ReadHeader()
{
    InFile = fopen(InBMTFileName, "rb");
    if (!InFile)
	return 1;
    fgets(buff, BUFFSIZE, InFile);
    assert(!strncmp(buff, "BMT", 3));
    fgets(buff, BUFFSIZE, InFile);
    while (strncmp(buff, BEGINTRAJ, strlen(BEGINTRAJ))) {
	if      (1 == sscanf(buff, "STRUCTURE %s\n", PDBFileName))
	    ;
	else if (1 == sscanf(buff, "NUMNONWATERATOMS %d\n", &NNonWaterAtoms))
	    ;
	else if (1 == sscanf(buff, "NUMWATERS %d\n", &NInWaters))
	    ;
	else if (1 == sscanf(buff, "NOWATERS %d\n", &NoInWaters))
	    ;
	fgets(buff, BUFFSIZE, InFile);
    }

    if (NoInWaters)
	NInWaters = 0;

    NInAtoms = NNonWaterAtoms + 3 * NInWaters;

    NInWaterAtoms = 3 * NInWaters;

    return 0;
}

int ReadFrame()
{
    float x0, y0, z0, s;
    // _fmode = _O_BINARY;
    if (GenRead(InFile, &x0))
	return 1;
    if (GenRead(InFile, &y0))
	return 1;
    if (GenRead(InFile, &z0))
	return 1;
    if (GenRead(InFile, &s))
	return 1;

    unsigned short sx, sy, sz;
    
    for (int i=0; i<NInAtoms; i++) {
	if (GenRead(InFile, &sx))
	    return 1;
	if (GenRead(InFile, &sy))
	    return 1;
	if (GenRead(InFile, &sz))
	    return 1;
	
	pInPos[i].mX = x0 + sx * s;
	pInPos[i].mY = y0 + sy * s;
	pInPos[i].mZ = z0 + sz * s;
    }
    return 0;
}

int WriteFrame()
{
    XYZ origin, span;
    double maxspan;
    findBox(pOutPos, 0, NOutAtoms, origin, span, maxspan);
    double factor = 65535.0/maxspan;
    float recipfactor = maxspan/65535;
    simplifySites(pOutPos, pOutShortPos, 0, NOutAtoms, origin, factor);

    float x,y,z;
    x = origin.mX;
    y = origin.mY;
    z = origin.mZ;
    if (GenWrite(OutFile, &x))
	return 1;
    if (GenWrite(OutFile, &y))
	return 1;
    if (GenWrite(OutFile, &z))
	return 1;
    if (GenWrite(OutFile, &recipfactor))
	return 1;
    
    XYZShort *psp = pOutShortPos;
    for (int i=0; i<NOutAtoms; i++) {
	if (GenWrite(OutFile, psp))
	    return 1;
	psp++;
    }
    return 0;
}

float TranslationDistance(float L, float x)
{
    float a = fabs(x);
    if (a < L/2)
	return 0;
    int n = (a-L/2)/L + 1;
    return (x > 0) ? n*L : -n*L;
}

// returns nonzero if no translation
int TranslationVector(float L, XYZ &p, XYZ &v)
{
    v.mX = TranslationDistance(L, p.mX);
    v.mY = TranslationDistance(L, p.mY);
    v.mZ = TranslationDistance(L, p.mZ);
    return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
}

// returns nonzero if no translation
int TranslationVector(XYZ &r, float L, XYZ &p, XYZ &v)
{
    v.mX = TranslationDistance(L, p.mX-r.mX);
    v.mY = TranslationDistance(L, p.mY-r.mY);
    v.mZ = TranslationDistance(L, p.mZ-r.mZ);
    return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
}

void TranslateIntoBox(float L, XYZ *p, int k, int count, int tag)
{
    XYZ v;
    if (TranslationVector(L, p[k+tag], v))
	return;
    for (int i=k; i<k+count; i++)
	p[i] -= v;
}

void TranslateNearTo(XYZ &r, float L, XYZ *p, int k, int count, int tag)
{
    XYZ v;
    if (TranslationVector(r, L, p[k+tag], v))
	return;
    for (int i=k; i<k+count; i++)
	p[i] -= v;
}

#define STARTION 256
#define ENDION 258

void MapIntoBox()
{
    for (int i=0; i<NInWaters; i++)
	TranslateIntoBox(BoxWidth, pInPos, NNonWaterAtoms+3*i, 3, 1);
    for (int i=STARTION; i<=ENDION; i++)
	TranslateIntoBox(BoxWidth, pInPos, i, 1, 0);
}

void MapToRefPoints()
{
    for (int i=0; i<NInWaters; i++)
	TranslateNearTo(RefPointsMean, BoxWidth, pInPos, NNonWaterAtoms+3*i, 3, 1);
    for (int i=STARTION; i<=ENDION; i++)
	TranslateNearTo(RefPointsMean, BoxWidth, pInPos, i, 1, 0);
}

void FindRefPoints()
{
    XYZ ref;
    XYZ RefPointsMean = XYZZero;
    for (int i=0; i<NRefPoints; i++) {
	ref = XYZZero;
	for (int j=i; j<i+3; j++)
	    ref += pInPos[RefIndices[j]-1];
	RefPoints[i] = ref/(float)3.0;
	RefPointsMean += RefPoints[i];
    }
    RefPointsMean /= NRefPoints;
}

float HausdorffDistance(XYZ *p, int np, XYZ *q, int nq)
{
    float max = (float)-1.0E+20;

    for (int i=0; i<nq; i++) {
        float min = (float)1.0E+20;
	for (int j=0; j<np; j++) {
	    float d = SquaredDistance(p[j], q[i]);
	    if (d < min)
		min = d;
	}
	if (max < min)
	    max = min;
    }
    return max;
}

int compare( const void *arg1, const void *arg2 )
{
    float d1 = pWaterDistance[*(int *)arg1];
    float d2 = pWaterDistance[*(int *)arg2];
    return (d1 < d2) ? -1 : (d1 > d2) ? 1 : 0;
}

void SortWaters()
{
    XYZ *WaterPointer = &pInPos[NNonWaterAtoms];
    for (int i=0; i<NInWaters; i++) {
	pSortedWater[i] = i;
	pWaterDistance[i] = HausdorffDistance(RefPoints, NRefPoints, WaterPointer, 3);
	WaterPointer += 3;
    }
    
    qsort( (void *)pSortedWater, (size_t)NInWaters, sizeof( int ), compare );

    for (int n=0; n<NNonWaterAtoms; n++)
	pOutPos[n] = pInPos[n];
    XYZ *pout = &pOutPos[NNonWaterAtoms];
    for (int k=0; k<NOutWaters; k++) {
	XYZ *p = &pInPos[NNonWaterAtoms + pSortedWater[k]*3];
	for (int l=0; l<3; l++)
	    *pout++ = *p++;
    }
}

int main(int argc, char **argv, char **envp)
{
    int  arg0 = 1;
    int rc;
    int maptobox = 0;

    if (argc < 3) {
        cerr << "bmtmodify -box1 L -nearestwaters N bmtinputfilename bmtoutputfilename" << endl;
        return 1;
    }

    while (*argv[arg0] == '-') {
        if (!strcmp(argv[arg0], "-box1")) {
            MapToBox = 1;
	    BoxWidth = atof(argv[++arg0]);
        } else if (!strcmp(argv[arg0], "-nearestwaters")) {
            NOutWaters = atoi(argv[++arg0]);
        } else if (!strcmp(argv[arg0], "-maptobox")) {
	    maptobox = 1;
        } else {
            cerr << "Command option " << argv[arg0] << " not recognized." << endl;
            exit(-1);
        }
        arg0++;
    }

    strcpy(InBMTFileName, argv[arg0++]);
    strcpy(OutBMTFileName, argv[arg0++]);
    
    int NStepsExpected = 100000000;

    rc = ReadHeader();
    if (rc) {
	cerr << "Error reading header" << endl;
	exit(-1);
    }

    if (NOutWaters < 0)
	NOutWaters = NInWaters;

    if (NInWaters < NOutWaters) {
	cout << "NInWaters in file is less than NOutWaters: " << NInWaters << " " << NOutWaters << endl;
	return 1;
    }

    NOutAtoms = NOutWaters*3 + NNonWaterAtoms;

    pInPos = new XYZ[NInAtoms];
    pOutPos = new XYZ[NOutAtoms];
    pOutShortPos = new XYZShort[NOutAtoms];
    pWaterDistance = new float[NInWaters];
    pSortedWater = new int[NInWaters];

    WriteHeader();
    
    for (int NStep = 0; NStep < NStepsExpected; NStep++) {
	// read all sites in the frame
	rc = ReadFrame();
	if (rc)
	    break;
	// Find the current reference points
	FindRefPoints();
	if (maptobox) {
	    MapIntoBox();
	} else {
	    // map all fragments to near the mean reference points
	    MapToRefPoints();
	}
	// map all sites while maintaining fragmentation
	// MapIntoBox();
	// Find hausdorff distance of each water from ref points and sort, perhaps to separate array
	SortWaters();
	// Write out the desired subset of the system
	WriteFrame();
    }

    fclose(InFile);
}
