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
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
//#include <fstream>
#define BUFFSIZE 2048

#define BEGINTRAJ "TRAJECTORYFORMAT"
#define ATOM "ATOM"

//                               -fff.fff-fff.fff-fff.fff
// 123456789012345678901234567890123456789012345678901234567890
// ATOM      1 HH31 ACE     1      11.996   2.667  -7.715
using namespace std;

#define MAXATOMS 200000

#define EMPTYLINE "                                                                   "

class BMTReader;
int ReadIdList(char *fname,int *n, int **ids);
int ReadIdPairList(char *fname,int *n, int **ids, int **ids2);
int DumpFrameCoords(BMTReader &b,int nframes, int nats, int *ids,double * interp);
double Dist(double x1,double y1,double z1,double x2,double y2,double z2);
double SQDist(double x1,double y1,double z1,double x2,double y2,double z2);
double SQDist2D(double x1,double y1,double x2,double y2);
double SQDist1D(double z1,double z2);
void OutputHist(char *fname,int nbins, int * allhis, int * inthis, int * exthis, int NA, int NI, int NX, double norm, double binwidth );
void OutputCorrFunction(char *fname, int nframes, double *corrav,double *corrvar,  double dt, double plat);  
void PrintTestMenu();

int DoPNHist(BMTReader &b, char *Nname,char *Pname, double vx, double vy, double vz, 
	     double binwidth, int nbins, double slab, int nframes , int *hist, int *count, int exclude_intern, int exclude_extern, int load);
int DoPNCorr(BMTReader &b, char *Nname,char *Pname, 
	     double vx, double vy, double vz,int nframes,double dt,char *outname);
int ExtractAtomListTimeSeries(BMTReader &b,char *fname,int nframes,char *outname, 
		            int average);

double RMSDFromAtomList(BMTReader &b,char *fname,int nframes,int dt,double timestep,
			int average, int DistanceDimension, char *cent_file,char *outname);

int TorsionStats(BMTReader &b,char *idlist1,char *idlist2,char *idlist3,char *idlist4,int nframes,double dt, char *outname);
int IntBuffer[MAXATOMS];
int IntCount = 0;

int DoScd(BMTReader &b, char *Pname,char *Nname,   int nframes, double *Scd, double *er );
int OutputScd(char *outname,char *pname, char*nname, double Scd,double er);
int DumpWaterOrientation(BMTReader &b,char *Pname,int nframes,char *outname);
int DoGroupCrossCorrelation(BMTReader &b, char *Pname, int nframes,int skip,double timestep, char *cent_file,char *outname);
int DoGroupDistanceCorrelation(BMTReader &b, char *Pname,  int nframes,int skip, double timestep,char *cent_file,char *outname);


int DoGroupDisplacementCorrelation(BMTReader &b, char *Pname,  int nframes,int skip, double timestep,char *cent_file,char *outname);

int DoGroupRotationCorrelation(BMTReader &b, char *Pname,  int nframes,int skip, double timestep,char *cent_file,char *outname);
void OutputZHist(char *fname,double binwidth, int nbins, int nframes,int * allhis, int N,int nelect);
int DoZHist(BMTReader &b, char *Pname,  double binwidth, int nbins, int nframes , int *hist, int *count);
int FindClosest(BMTReader &b,char *Pairname,char *Searchname,double dist,int nframes,char *outname);
int ExpandIdList(int *ids, int nids,int length, int **new_nids, int *Nnew_ids);
int ExtractAtomGroupListTimeSeries(BMTReader &b,char *fname,int nframes,int length,int dim,char *outname);

double Vx,Vy,Vz;
double Vx2,Vy2,Vz2;

int CrossCorrelate = 1;
int allocsize = 0;

#define  MEMLOG 0
#define CROSS_CORRELATE CrossCorrelate 

#define MAXATOMS 200000
//           1         2         -fff.fff-fff.fff-fff.fff      6         7         8
// 012345678901234567890123456789012345678901234567890123456789012345678901234567890
// ATOM      1 HH31 ACE     1      11.996   2.667  -7.715
// ATOM      5  H2B STEA    1     -21.157 -13.259  20.028  1.00   .00      L001

#define PDBPREFIX 30
#define PDBSUFFIX 55
#define EMPTYLINE "                                                                   "

#define PDBATOMTAG 12
#define PDBRES 17
#define PDBRESN 22
#define PDBCHAIN 27
#define PDBX 30
#define PDBY 38
#define PDBZ 46
#define PDBSEG 72
#define PDBCOORDFORMAT "%8.3f%8.3f%8.3f"

class PDBBuffer
{
public:
  char PDBFileName[256];
  int  NAtoms;
  
  char buff[BUFFSIZE];
  char atomtag[MAXATOMS][6];
  char res[MAXATOMS][6];
  char chain[MAXATOMS][3];
  char seg[MAXATOMS][15];
  int resnum[MAXATOMS];
  
  double X[MAXATOMS];
  double Y[MAXATOMS];
  double Z[MAXATOMS];
  
  PDBBuffer(char *fname)
    {
      FILE *f = fopen(fname, "r");
      assert(f);
      fgets(buff, BUFFSIZE, f);
      int npdbatom = 0;
      while (!feof(f) && !ferror(f)) 
	{
	  if (!strncmp(buff, ATOM, strlen(ATOM))) 
	    {
	      buff[strlen(buff)-1] = '\0';
	      sscanf(&buff[PDBSEG],"%s", seg[npdbatom]); buff[PDBZ + 8] = '\0';
	      sscanf(&buff[PDBZ],"%lf", &Z[npdbatom]); buff[PDBZ] = '\0';
	      sscanf(&buff[PDBY],"%lf", &Y[npdbatom]); buff[PDBY] = '\0';
	      sscanf(&buff[PDBX],"%lf", &X[npdbatom]); buff[PDBX] = '\0';
	      sscanf(&buff[PDBCHAIN],"%s", chain[npdbatom]);buff[PDBCHAIN] = '\0';
	      sscanf(&buff[PDBRESN],"%d", resnum[npdbatom]); buff[PDBRESN] = '\0';
	      sscanf(&buff[PDBRES],"%s", res[npdbatom]); buff[PDBRES] = '\0';
	      sscanf(&buff[PDBATOMTAG],"%s", atomtag[npdbatom]);
	      npdbatom++;
	      //	    printf("pdbat %d %s\n",npdbatom,buff);
	    }
	  fgets(buff, BUFFSIZE, f);
	}
      NAtoms = npdbatom;
      fclose(f);
    }

  int NumAtoms()
    {return NAtoms;}
  
  int Coord(int i, double &x,double &y,double &z)
    {
      assert(i < NAtoms);
      x = X[i];y = Y[i];z = Z[i];
      return 1;
    }
  double SqDist(int i, int j)
    {
      double dx,dy,dz;
      dx = X[i] - X[j];
      dy = Y[i] - Y[j];
      dz = Z[i] - Z[j];
      return (dx*dx + dy*dy + dz*dz);
    }
  char * ResName(int i) {return res[i];}
  char * SegName(int i) {return seg[i];}
  char * AtomName(int i) {return atomtag[i];}
  int ResNum(int i){return resnum[i];}
};


class BMTReader
{
public:
  double *x,*y,*z,*X1,*Y1,*Z1,*X2,*Y2,*Z2;
  int NAtoms, NProteinAtoms, NWaters,NoWaters,NWaterAtoms;
  int CurrentFrame;
  FILE *f;
  char buff[BUFFSIZE];
  char PDBFileName[256];
  int FramesInMemory;
  double **fx,**fy,**fz;
  int Skip; 
  int Float_Mode;
  double *mMasses;
  double cenX,cenY,cenZ;
  int *mCentIds, mNCents;
  

  BMTReader(FILE *fil, char *massfile = "") : NAtoms(0), Skip(1)
  {
    char trajform[256];
    CurrentFrame = 0;
    FramesInMemory = 0;
    f = fil;
    fgets(buff, BUFFSIZE, f);
    assert(!strncmp(buff, "BMT", 3));
    fgets(buff, BUFFSIZE, f);
    while (strncmp(buff, BEGINTRAJ, strlen(BEGINTRAJ))) {
      if      (1 == sscanf(buff, "STRUCTURE %s\n", PDBFileName))
	;
      else if (1 == sscanf(buff, "NUMNONWATERATOMS %d\n", &NProteinAtoms))
	;
      else if (1 == sscanf(buff, "NUMWATERS %d\n", &NWaters))
	;
      else if (1 == sscanf(buff, "NOWATERS %d\n", &NoWaters))
	;
      fgets(buff, BUFFSIZE, f);

    }

      if (1 == sscanf(buff, "%*s %s\n", trajform))
	{
	  if (!strcmp("FLOAT",trajform))
	    {
	      Float_Mode = 1;
	      printf("Reading BMT in Float mode\n");
	    }
	  else
	    Float_Mode = 0;
	}
    
    if (NoWaters)
      NWaters = 0;
    
    NAtoms = NProteinAtoms + 3 * NWaters;
    
    NWaterAtoms = 3 * NWaters;

    x = new double[NAtoms + 1];
    y = new double[NAtoms + 1];
    z = new double[NAtoms + 1];

    X1 = X2 = Y1 = Y2 = Z1 = Z2 = NULL;

    allocsize += 3 *( NAtoms + 1 ) * sizeof(double);
    AllocateFrameArray();
    mMasses = new double[NAtoms + 1];
    ReadMasses(massfile);
    mCentIds = NULL;
    mNCents = 0;
  }
  
  ~BMTReader()
  {
    if(NAtoms > 0)
      {
	delete [] x;
	delete [] y;
	delete [] z;
	FreeWorkingSets();
      }
  }

  void SetSkip(int s){Skip = s;}
  int GetSkip(){return Skip;}

  void SetCentIds(int *ids,int n)
    {
      if(mCentIds != NULL)
	{ delete [] mCentIds;}
      mCentIds = new int[n+1];
      for(int i = 0;i < n; i++)
	{
	  mCentIds[i] = ids[i] - 1;
	}
      mNCents = n;
    }
  void setCenter(double xx, double yy, double zz)
    {
      cenX = xx;cenY = yy;cenZ = zz;
    }
  void UpdateCenter()
    {
      double xx = 0., yy = 0., zz = 0., m = 0.0;
      for (int i = 0; i < mNCents; i++)
	{
	  int j = mCentIds[i];
	  xx += x[j] * mMasses[j];
	  yy += y[j] * mMasses[j];
	  zz += z[j] * mMasses[j];
	  m += mMasses[j];
	}
      cenX = xx / m; 
      cenY = yy / m; 
      cenZ = zz / m;
    }
  
  
  void Rewind()
  {
    CurrentFrame = 0;
    rewind(f);
    fgets(buff, BUFFSIZE, f);
    while (strncmp(buff, BEGINTRAJ, strlen(BEGINTRAJ))) 
      {fgets(buff, BUFFSIZE, f);}
  }
  int ReadMasses(char *massfile)
    {
      char bufS[256];
      FILE *fPF = fopen(massfile,"r");

      if(fPF != NULL)
	{
	  printf("Reading masses from %s\n",massfile);
	}
      for(int i=0;i< NAtoms;i++)
	{
	  if(fPF != NULL)
	  {
	    fgets(bufS,256,fPF);
	    sscanf(bufS,"%lf",&mMasses[i]);
	    //	  printf("Mass %d = %f\n",i,mMasses[i]);
	  }
	  else
	    mMasses[i] = 1.0;
	}
      fclose(fPF);
      return 1;
    }

  double Mass(int i)
    {
      return mMasses[i];
    }
  int ReadFrame()
  {
    
    for(int sk = 0; sk < Skip; sk++)
      {
	float x0, y0, z0, s;
	
	if(Float_Mode)
	  {
	    for (int i=0; i<NAtoms; i++) 
	      {
		fread(&x0, sizeof(float), 1, f);
		if (feof(f) || ferror(f)) 
		  assert( 0 );
		fread(&y0, sizeof(float), 1, f);
		fread(&z0, sizeof(float), 1, f);
		if(sk == 0)
		  {
		    x[i] = x0;
		    y[i] = y0;
		    z[i] = z0;
		    //		    if(i < 5)
		    //		      printf("site %d: %f %f %f \n",i, x0,y0,z0);
		  }
	      }
	  }
	else
	  {
	    fread(&x0, sizeof(float), 1, f);
	    //printf("X = %f ",x0);
	    fread(&y0, sizeof(float), 1, f);
	    //printf("Y = %f ",y0);
	    fread(&z0, sizeof(float), 1, f);
	    //printf("Z = %f ",z0);
	    fread(&s,  sizeof(float), 1, f);
	    //printf("S = %f ",s);
	    
	    if (feof(f) || ferror(f)) 
	      return 0;
	    
	    unsigned short sx, sy, sz;
	
	    for (int i=0; i<NAtoms; i++) 
	      {
		fread(&sx, sizeof(short), 1, f);
		fread(&sy, sizeof(short), 1, f);
		fread(&sz, sizeof(short), 1, f);
		
		if(sk == 0)
		  {
		    x[i] = x0 + sx * s;
		    y[i] = y0 + sy * s;
		    z[i] = z0 + sz * s;
		  }
	      }
	  }
      }
    if(mNCents > 0)
      {
	UpdateCenter();
	for (int i=0; i<NAtoms; i++) 
	  {
	    x[i] -= cenX;
	    y[i] -= cenY;
	    z[i] -= cenZ;
	  }
      }
    return 1;
  }
  

  void ClearFrameBuffer()
  {
    if(FramesInMemory > 0)
      {
	for(int i=0;i<NAtoms;i++)
	  {
	    FreeAtomTimeSeries(i);
	  }
	delete [] fx;
	delete [] fy;
	delete [] fz;
	FramesInMemory = 0;
      }
  }
  int AllocateAtomTimeSeries(int i,int nframes)
  {
    FreeAtomTimeSeries(i);
    if(FramesInMemory == 0)
      FramesInMemory = nframes;
    assert(FramesInMemory == nframes);
    fx[i]= new double[nframes];
    if(fx[i] == NULL)
      { printf("failed allocation on frame %d\n",i); fflush(stdout); assert(0);}
    fy[i]= new double[nframes];
    if(fy[i] == NULL)
      { printf("failed allocation on frame %d\n",i); fflush(stdout); assert(0);}
    fz[i]= new double[nframes];
    if(fz[i] == NULL)
      { printf("failed allocation on frame %d\n",i); fflush(stdout); assert(0);}  
    allocsize += 3 *( nframes ) * sizeof(double);
    if(MEMLOG)
      {printf ("AllocateAtomTimeSeries atom %d  nframes %d total = %d\n",i,nframes,allocsize);}
    return 1;
  }

  void FreeAtomTimeSeries(int atom)
  {
    if(FramesInMemory > 0)
      {
	if(fx[atom] != NULL)
	  delete [] fx[atom];
	fx[atom] = NULL;
      }
    return;
  }
  int AllocateFrameArray()
  {
    if(FramesInMemory > 0)
      {
	ClearFrameBuffer();
      }
    assert(fx = new double*[NAtoms]);
    assert(fy = new double*[NAtoms]);
    assert(fz = new double*[NAtoms]);
    allocsize += 3 *( NAtoms ) * sizeof(int);
    if(MEMLOG)
      {printf ("AllocateFrameArray total = %d\n",allocsize);}
    for(int i=0;i<NAtoms;i++)
      {
	fx[i] = NULL;
	fy[i] = NULL;
	fz[i] = NULL;
      }
    return 1;
  }
  int AllocateFrameBuffer(int nframes)
  {
    for(int i=0;i<NAtoms;i++)
      { 
	AllocateAtomTimeSeries(i,nframes);
      }
    FramesInMemory = nframes;
    return 1;
  }
  int AllocateWorkingSets()
  {
    if(X1 == NULL)
      {
	X1 = new double[FramesInMemory + 1];
	Y1 = new double[FramesInMemory + 1];
	Z1 = new double[FramesInMemory + 1];
	X2 = new double[FramesInMemory + 1];
	Y2 = new double[FramesInMemory + 1];
	Z2 = new double[FramesInMemory + 1];
      }
    return 1;
  }
  int FreeWorkingSets()
  {
    if(X1 != NULL)
      {
	delete [] X1; X1 = NULL;
	delete [] Y1; Y1 = NULL;
	delete [] Z1; Z1 = NULL;
	delete [] X2; X2 = NULL;
	delete [] Y2; Y2 = NULL;
	delete [] Z2; Z2 = NULL;
      }
    return 1;
  }
  int BufferFrames(int n)
  {
    AllocateFrameBuffer(n);
    for (int t = 0; t< n; t++)
      {
	assert(ReadFrame());
	for(int a = 0;a<NAtoms;a++)
	  {
	    fx[a][t] = x[a];
	    fy[a][t] = y[a];
	    fz[a][t] = z[a];
	  }
      }
    return 1;
  }
  int BufferAtomPair(int n, int a1, int a2)
  {
    ClearFrameBuffer();
    AllocateFrameArray();
    AllocateAtomTimeSeries(a1,n);
    if(a2 > 0)
      AllocateAtomTimeSeries(a2,n);
    Rewind();
    for (int t = 0; t< n; t++)
      {
	assert(ReadFrame());
	fx[a1][t] = x[a1];
	fy[a1][t] = y[a1];
	fz[a1][t] = z[a1];
	if(a2 > 0)
	  {
	    fx[a2][t] = x[a2];
	    fy[a2][t] = y[a2];
	    fz[a2][t] = z[a2];
	  }
      }
    return 1;
  }
  int BufferAtomList(int nframes, int nats, int *ids, int clear = 0, int fill = 0)
  {
    int grouplen = 1;
    if(clear)
      {
	ClearFrameBuffer();
	AllocateFrameArray();
      }
    Rewind();
    if(fill)
      {
	grouplen = ids[1] - ids[0];
	printf("Group len = %d\n",grouplen);
      }
    for(int i = 0; i < nats; i++)
      {
	for(int m = 0; m < grouplen; m++)
	  {
	    assert(AllocateAtomTimeSeries(ids[i]+m-1,nframes));
	  }
      }
    printf("Buffer Atom List: Now reading %d frames with skip of %d\n",nframes,GetSkip());
    for (int t = 0; t< nframes; t++)
      {
	assert(ReadFrame());
	for(int a = 0; a < nats; a++)
	  {
	    int i = ids[a] - 1;

	    for(int m = 0; m < grouplen; m++)
	      {
		fx[i+m][t] = x[i+m];
		fy[i+m][t] = y[i+m];
		fz[i+m][t] = z[i+m];
	      }
	  }
      }
    return 1;
  }
  
  int BufferAtomListFromTimeSeries(char *fname, int nframes, 
				   int nats, int *ids, int clear = 0)
  {
    if(clear)
      {
	ClearFrameBuffer();
	AllocateFrameArray();
      }
    FILE *fPF = fopen(fname,"r");

    for(int i = 0; i < nats; i++)
      {
	AllocateAtomTimeSeries(ids[i]-1,nframes);
      }
    for (int t = 0; t< nframes; t++)
      {
	assert(ReadTimeSeriesRow(fPF,nats,ids));
	for(int a = 0; a < nats; a++)
	  {
	    int i = ids[a] - 1;
	    fx[i][t] = x[i];
	    fy[i][t] = y[i];
	    fz[i][t] = z[i];
	  }
      }
    fclose(fPF);
    return 1;
  }
  
int ReadTimeSeriesRow(FILE *fPF,int nats,int *ids)
  {
    for(int i = 0; i < nats; i++)
      {
	int j = ids[i]-1;
	assert(fscanf(fPF,"%lf %lf %lf", &x[j],&y[j],&z[j]));
      }
    return 1;    
  }

int CosTorsion(int t,int a1, int a2, int a3, int a4,double &costheta)
{
  int sign;
  double x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4;
  double dx1,dy1,dz1,dx2,dy2,dz2,dx3,dy3,dz3,dx4,dy4,dz4,n1,n2;
  double px1,py1,pz1,px2,py2,pz2,d1,d2;
  GetVector(a1,t,x1,y1,z1);
  GetVector(a2,t,x2,y2,z2);
  GetVector(a3,t,x3,y3,z3);
  GetVector(a4,t,x4,y4,z4);

  dx1 = x3-x2; dy1 = y3-y2; dz1 = z3-z2;
  dx2 = x1-x2; dy2 = y1-y2; dz2 = z1-z2;
  dx3 = x4-x2; dy3 = y4-y2; dz3 = z4-z2;
  px1 = dy1*dz2 - dy2*dz1; py1 = dx2*dz1 - dx1*dz2;  pz1 = dx1*dy2 - dx2*dy1;
  px2 = dy1*dz3 - dy3*dz1; py2 = dx3*dz1 - dx1*dz3;  pz2 = dx1*dy3 - dx3*dy1;
  d1 = sqrt(px1*px1 + py1*py1 + pz1*pz1);
  d2 = sqrt(px2*px2 + py2*py2 + pz2*pz2);
  px1 /= d1;py1 /= d1;pz1 /= d1;
  px2 /= d2;py2 /= d2;pz2 /= d2;
  costheta = px1*px2 + py1*py2 + pz1*pz2;
  d1 = px1 * dx3 + py1 * dy3 + pz1 * dz3;

  double theta = acos(costheta);


  if(d1 <= 0.0)
    sign = 1;
  else
    sign = -1;

  //  printf("costheta %f  theta = %f  sign = %d\n",costheta,theta,sign);

  return sign;
}
  
int CorrelateTorsion(int a1, int a2,int a3, int a4, double * corr)
  {
    int n = FramesInMemory;
    int dt;
    for(int dt = 0; dt < n;dt++)
      {
	int count = 0;
	double sum = 0;
	for(int i = 0;i < n-dt ; i++)
	  {
	    double ct0,ctt; 
	    CosTorsion(i,a1,a2,a3,a4,ct0);
	    
	    int j = i + dt;
	    
	    CosTorsion(j,a1,a2,a3,a4,ctt);

	    sum += ct0 * ctt;
	    count++;
	    
	  }
	sum /= count;
	//	sum /= n;
	corr[dt] = sum;
	
      }
    return 1;
  }
  int CorrelateAtomPair(int a1, int a2, double * corr,double *var,int *N,double &platsum)
  {
    double x1,y1,z1,x2,y2,z2,dx1,dy1,dz1,dx2,dy2,dz2,d;
    int n = FramesInMemory;
    int dt;
    int count;
    AtomPairScd(a1,a2,platsum,d);

    AllocateWorkingSets();
    for(int i = 0;i < n; i++)
      {
	double xi1,yi1,zi1,xi2,yi2,zi2,dxi1,dyi1,dzi1,dxi2,dyi2,dzi2,di;
	double d03,dt3;
	double c0,ct;
	double r0,rt;
	count = 0;
	double sum = 0;
	GetVector(a1,i,xi1,yi1,zi1);
	GetNearVector(a2,i,xi1,yi1,zi1,xi2,yi2,zi2);
	X1[i] = dxi1 = xi2 - xi1;
	Y1[i] = dyi1 = yi2 - yi1;
	Z1[i] = dzi1 = zi2 - zi1;
	di = sqrt(dxi1*dxi1 + dyi1*dyi1 + dzi1*dzi1);
	X2[i] = di * di * di;
	X1[i] /= di;
	Y1[i] /= di;
	Z1[i] /= di;
      }


    for(int dt = 0; dt < n;dt++)
      {
	count = 0;
	double sum = 0;
	for(int i = 0;i < n-dt ; i++)
	  {
	    int j = i + dt;
	    d = X1[i]*X1[j] + Y1[i]*Y1[j] + Z1[i]*Z1[j];

	    sum += 1.5 * d * d - 0.5;
	    count++;
	    
	  }
	// don't normalixe inside... return count	sum /= count;
	corr[dt] += sum;
	N[dt] += count;
      }
    return count;
  }

  int CorrelateAtomPairDistance(int a1, int a2, double * corr,double *var,int *N,double &platsum)
  {
    double x1,y1,z1,x2,y2,z2,dx1,dy1,dz1,dx2,dy2,dz2,d;
    int n = FramesInMemory;
    int dt;
    int count;
    AtomPairScd(a1,a2,platsum,d);

    AllocateWorkingSets();
    for(int i = 0;i < n; i++)
      {
	double xi1,yi1,zi1,xi2,yi2,zi2,dxi1,dyi1,dzi1,dxi2,dyi2,dzi2,di;
	double d03,dt3;
	double c0,ct;
	double r0,rt;
	count = 0;
	double sum = 0;
	GetVector(a1,i,xi1,yi1,zi1);
	GetNearVector(a2,i,xi1,yi1,zi1,xi2,yi2,zi2);
	X1[i] = dxi1 = xi2 - xi1;
	Y1[i] = dyi1 = yi2 - yi1;
	Z1[i] = dzi1 = zi2 - zi1;
	di = sqrt(dxi1*dxi1 + dyi1*dyi1 + dzi1*dzi1);
	//	X2[i] = 1/ (di);
       	X2[i] = 1/ (di * di * di);
	//	X1[i] /= di;
	//	Y1[i] /= di;
	//	Z1[i] /= di;
      }

    for(int dt = 0; dt < n;dt++)
      {
	count = 0;
	double sum = 0;
	for(int i = 0;i < n-dt ; i++)
	  {
	    int j = i + dt;
	    //	    d = X1[i]*X1[j] + Y1[i]*Y1[j] + Z1[i]*Z1[j];
	    d = X2[i]*X2[j];
	    sum += d;
	    // sum += 1.5 * d * d - 0.5;
	    count++;
	    
	  }
	// don't normalixe inside... return count	sum /= count;
	corr[dt] += sum;
	N[dt] += count;
      }
    return count;
  }


  int CrossCorrelateAtomPair(int a1, int a2, double * corr,double *var, int *N)
  {
    double c,xi1,yi1,zi1,xi2,yi2,zi2,dxi1,dyi1,dzi1,dxi2,dyi2,dzi2,di;
    double xj1,yj1,zj1,xj2,yj2,zj2,dxj1,dyj1,dzj1,dxj2,dyj2,dzj2,dj;
    int n = FramesInMemory;
    int dt,count;
    AllocateWorkingSets();

    for(int i = 0;i < n; i++)
      {
	double d03,dt3;
	double c0,ct;
	double r0,rt;
	count = 0;
	double sum = 0;
	GetVector(a1,i,xi1,yi1,zi1);
	GetNearVector(a2,i,xi1,yi1,zi1,xi2,yi2,zi2);
	dxi1 = xi2 - xi1;
	dyi1 = yi2 - yi1;
	dzi1 = zi2 - zi1;
	di = sqrt(dxi1*dxi1 + dyi1*dyi1 + dzi1*dzi1);
	d03 = di * di * di;
	c0 = dzi1 / di;
	r0 = (3 * c0 * c0 - 1) / d03;
	X1[i] = r0;
      }

    for(int dt = 0; dt < n;dt++)
      {
	count = 0;
	double sum = 0;
	double s = 0;
	double v = 0;
	
	double *r0p = X1;
	double *rtp = &X1[dt];
	for(int i = 0;i < n-dt ; i++)
	  {
	    double r = *r0p * *rtp;
	    sum += r; 
	    v += r*r; 
	    count++;
	    r0p++;
	    rtp++;
	  }

	// don't normalize sum and var
	sum /= count;
	r0p = X1;
	rtp = &X1[dt];
	for(int i = 0;i < n-dt ; i++)
	  {
	    double r = (*r0p * *rtp) - sum;
	    s += r;
	    v += r*r; 
	    r0p++;
	    rtp++;
	  }


	corr[dt] += sum;
	var[dt] += (v - s*s/count) / (count - 1);
	N[dt] += count;
      }// end dt
    return count;
  }

int AtomPairPhi(int a1, int a2, int frame, double &phi, double &d)
{
  double x1,y1,z1,x2,y2,z2,dx1,dy1,dz1,dx2,dy2,dz2;
  
  GetVector(a1,frame,x1,y1,z1);
  GetVector(a2,frame,x2,y2,z2);
  dx1 = x2 - x1;
  dy1 = y2 - y1;
  dz1 = z2 - z1;
  d = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
  phi = dz1 / d;


  return 1;
}

  int AtomPairScd(int a1, int a2, double & Scd, double &var)
  {
    double c0,x1,y1,z1,x2,y2,z2,dx1,dy1,dz1,dx2,dy2,dz2,d;
    int n = FramesInMemory;
    int dt;

    int count = 0;
    double sum = 0,sum2 = 0;
    for(int i = 0;i < n ; i++)
      {
	AtomPairPhi(a1,a2,i,c0,d);
	double r0 = 1.5 * c0 * c0 - 0.5;
	sum += r0;
	sum2 += r0*r0;
	//		printf("phi %d %d frame %d = %f - D = %f\n",a1,a2,i,r0,d );
	count++;
	
      }

    //    printf(" = %f for %d points\n",sum,count);
    Scd += sum;
    var += sum2;
    return count;
  }

  int AtomPairPlateau(int a1, int a2, double & plat)
  {
    double c0,x1,y1,z1,x2,y2,z2,dx1,dy1,dz1,dx2,dy2,dz2,d;
    int n = FramesInMemory;
    int dt;

    int count = 0;
    double sum = 0;
    //printf("plateau for %d %d ",a1,a2);    
    for(int i = 0;i < n ; i++)
      {
	GetVector(a1,i,x1,y1,z1);
	GetVector(a2,i,x2,y2,z2);

	for(int vx = -1; vx < 2; vx++)
	  {
	    for(int vy = -1; vy < 2; vy++)
	      {
		for(int vz = -1; vz < 2; vz++)
		  {
		    
 		    dx1 = x2 + vx*Vx - x1;
		    dy1 = y2 + vy*Vy - y1;
		    dz1 = z2 + vz*Vz - z1;
		    d = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
		    c0 = dz1 / d;
		    double d03 = d * d * d;
		    double r0 = (3 * c0 * c0 - 1) / d03;

		    sum += r0;
		    count++;
		  }
	      }
	  }	
      }

    sum /= count;
    //    sum *=  sqrt(5. / 16. / 3.14159) / count;
    //    printf(" = %f for %d points\n",sum,count);
    plat = sum;
    
    return 1;
  }



  int GetVector(int i, double &xx, double &yy, double &zz) 
  {
    if(i < NAtoms)
      {
	xx = x[i];yy = y[i];zz = z[i];
	return 1;
      }
    else
      {
	printf("GetVector %d NAtoms %d\n",i,NAtoms);
	assert(0);
      }
    return 0;

  }
  int GetGroupCOM(int atom, int frame, int grouplen,double &xx, double &yy, double &zz) 
  {
    double m = 0.0;
    xx = yy = zz = 0.0;
    if((atom+grouplen) < NAtoms && frame < FramesInMemory)
      {
	for(int i = atom; i < atom + grouplen ; i++)
	  {	    
	    xx += fx[i][frame]*mMasses[i];
	    yy += fy[i][frame]*mMasses[i];
	    zz += fz[i][frame]*mMasses[i];
	    m += mMasses[i];
	  }
	xx /= m;yy /= m;zz /= m;
	return 1;
      }
    else
      {
	printf("GetVector at %d NAtoms %d   frame %d   FramesInMemory %d\n",atom,NAtoms,frame,FramesInMemory);
	assert(0);
      }
      return 0;
  }

  int GetVector(int atom, int frame, double &xx, double &yy, double &zz) 
  {
    if(atom < NAtoms && frame < FramesInMemory)
      {
	xx = fx[atom][frame];
	yy = fy[atom][frame];
	zz = fz[atom][frame];
	return 1;
      }
    else
      {
	printf("GetVector at %d NAtoms %d   frame %d   FramesInMemory %d\n",atom,NAtoms,frame,FramesInMemory);
	assert(0);
      }
      return 0;
  }

  int GetNearVector(int atom, int frame,double x0, double y0,double z0, double &xx, double &yy, double &zz) 
  {
    if(atom < NAtoms && frame < FramesInMemory)
      {
	if(frame == -1)
	  {
	    xx = x[atom];
	    yy = y[atom];
	    zz = z[atom];
	  }
	else
	  {
	    xx = fx[atom][frame];
	    yy = fy[atom][frame];
	    zz = fz[atom][frame];
	  }

	while( xx-x0 > Vx2){ xx -= Vx;}
	while( xx-x0 < -Vx2){ xx += Vx;}
	while( yy-y0 > Vy2){ yy -= Vy;}
	while( yy-y0 < -Vy2){ yy += Vy;}
	while( zz-z0 > Vz2){ zz -= Vz;}
	while( zz-z0 < -Vz2){ zz += Vz;}
	//printf("%f %f %f %f %f %f \n",xx - x0, yy - y0, zz - z0, Vx2, Vy2, Vz2 );
	return 1;

        double t = xx - x0;
	int nx = (xx - x0) / Vx2;
	int ny = (yy - y0) / Vy2;
	int nz = (zz - z0) / Vz2;

	nx = (nx < 0) ? (nx - 1) / 2 : (nx + 1) / 2; 
	ny = (ny < 0) ? (ny - 1) / 2 : (ny + 1) / 2; 
	nz = (nz < 0) ? (nz - 1) / 2 : (nz + 1) / 2; 

	//	printf("%f %f %f %f %f %f %d %d %d\n",xx - x0, yy - y0, zz - z0, Vx2, Vy2, Vz2 ,nx,ny,nz);
	xx -= nx * Vx;
	yy -= ny * Vy;
	zz -= nz * Vz;
	return 1;
      }
    else
      {
	printf("GetVector at %d NAtoms %d   frame %d   FramesInMemory %d\n",atom,NAtoms,frame,FramesInMemory);
	assert(0);
      }
      return 0;
  }

};



int
main(int argc, char **argv, char **envp)
{
  double vx,vy,vz,binwidth,dt,D;
  int nbins = 0;
  int lag,average,nelect,length;
  int nframes = 0;
  int *hist = NULL; 
  int *allhist = NULL; 
  int *inthist = NULL; 
  int *exthist = NULL; 
  int total = 0;
  int atotal = 0;
  int itotal = 0;
  int xtotal = 0;
  char Pname[256];
  char Nname[256];
  char idlist1[256];
  char idlist2[256];
  char idlist3[256];
  char idlist4[256];
  char fname[256],outname[256];
  char com[256];
  int distdim;
  double surfdens;
  double Scd,er;
  int skip;
  double slab = 0.0;
  int nats;
  int *ids;
  Vx = Vy = Vz = Vx2 = Vy2 = Vz2 = 0;



  PrintTestMenu();
  
  if (argc < 2) {
    cout << "bmtreader BMTFileName " << endl;
    return 1;
  }
  
  FILE *f = fopen(argv[1], "rb");
  assert(f);
  
  // wetsope default
  BMTReader b(f,"wetsope.masses.dat");
  
  
  FILE *vPF;
  vPF = fopen("vol.dat","r");
  if(vPF != NULL)
    {
      fscanf(vPF,"%lf %lf %lf",&Vx2,&Vy2,&Vz2);	
    }
  else
  {  
    Vx2 = Vy2 = 22.8807;  
    Vz2 = 31.9384;
  }
  Vx = 2.0 * Vx2;
  Vy = 2.0 * Vy2; 
  Vz = 2.0 * Vz2;

  printf("Using volume %f %f %f \n",Vx,Vy,Vz);
    b.SetSkip(1);
    int test_case;    
    gets(com);
    sscanf(com, "%d", &test_case);
    int sk = 1;
    char interp_name[20];
    double interp[6];
    sprintf(fname,"NULL");
    FILE *tfPF;
    char cent_file[256];
    interp[0] = interp[1] = interp[2] = interp[3] = interp[4] = interp[5] = 0.0;
    switch(test_case)
      {
      case 1:
	assert(sscanf(argv[2],"%d",&nframes));
	assert(sscanf(argv[3],"%s",&fname));
	if(sscanf(argv[4],"%d",&sk)  == 1)
	  {b.SetSkip(sk);}

	if(sscanf(argv[5],"%s",&interp_name)  == 1)
	  {
	    // read interpolation factor
	    tfPF = fopen(interp_name,"r");
	    if(tfPF != NULL)
	    assert(6 == fscanf(tfPF,"%lf %lf %lf %lf %lf %lf ",&interp[0],&interp[1],&interp[2],&interp[3],&interp[4],&interp[5]));
	    
	  }

	if(!ReadIdList(fname,&nats,&ids))
	  {	
	    nats = b.NAtoms;
	    ids = new int[nats];
	    for(int i = 0; i < nats; i++)
	      {ids[i] = i+1;}
	  }
	DumpFrameCoords(b,nframes,nats,ids,interp);
	delete [] ids; ids = NULL;
	break;
      case 2:
	// Test for Radial Distribution Function of atom pairs
	// args bmtname  nbins idname1 idname2 slab_thicknes skip nframes outname
	if(argc < 9)
	  { printf("usage: %s bmtname  nbins  idname1 idname2 slab skip nframes outname\n",argv[0]); break;}
	  
	//	assert(sscanf(argv[2],"%lf",&vx));
	//	assert(sscanf(argv[3],"%lf",&vy));
	//	assert(sscanf(argv[4],"%lf",&vz));
	assert(sscanf(argv[2],"%d",&nbins));
	assert(sscanf(argv[3],"%s",Pname));
	assert(sscanf(argv[4],"%s",Nname));
	assert(sscanf(argv[5],"%lf",&slab));
	assert(sscanf(argv[6],"%d",&skip));
	assert(sscanf(argv[7],"%d",&nframes));
	assert(sscanf(argv[8],"%s",outname));

	b.SetSkip(skip);

	allhist = new int[nbins + 1];
	inthist = NULL; //new int[nbins + 1];
	exthist = NULL; //new int[nbins + 1];
	vx = Vx2;  vy = Vy2; vz = Vz2;

	binwidth = sqrt(vx * vx + vy * vy) / nbins ;
       	surfdens = 72.0 / (Vx*Vy*Vz) / 30.0;
       	//surfdens = 36.0/ (4.0 * vx * vy);
	//surfdens = 48.0/ (4.0 * vx * vy);
	// Do all
	if(1)
	  {
	    printf("Doing all\n");
	    total = 0;
	    for(int i = 0;i < nbins; i++){allhist[i] = 0;}
	    DoPNHist(b,Nname,Pname,vx*2.0,vy*2.0,vz*2.0, binwidth, nbins,slab,nframes,allhist,&atotal,0,0,1);
	  }

	// Do internal
	if(0)
	  {
	    printf("Doing internals\n");
	    total = 0;
	    for(int i = 0;i < nbins; i++){inthist[i] = 0;}
	    DoPNHist(b,Pname,Nname,vx*2.0,vy*2.0,vz*2.0, binwidth, nbins,slab,nframes,inthist,&itotal,0,1,0);
	  }

	// Do external
	if(0)
	  {
	    printf("Doing externals\n");
	    total = 0;
	    for(int i = 0;i < nbins; i++){exthist[i] = 0;}
	    DoPNHist(b,Nname,Pname,vx*2.0,vy*2.0,vz*2.0, binwidth, nbins,slab,nframes,exthist,&xtotal,1,0,0);
	  }
	OutputHist(outname, nbins, allhist,inthist,exthist,atotal,itotal, xtotal, surfdens, binwidth);
	delete [] allhist;
	//delete [] inthist;
	//delete [] exthist;
	break;
      case 3:
	// P-N correlation function 
	// args bmtname volx/2 voly/2 volz/2 nframes timestep  idname1 idname2 outname cross_correlate skip
	if(argc < 9)
	  { printf("usage: %s bmtname  nframes timestep  idname1 idname2 outname cross_correlate skip\n",argv[0]); break;}

	//	assert(sscanf(argv[2],"%lf",&vx));
	//	assert(sscanf(argv[3],"%lf",&vy));
	//	assert(sscanf(argv[4],"%lf",&vz));
	assert(sscanf(argv[2],"%d",&nframes));
	assert(sscanf(argv[3],"%lf",&dt));
	assert(sscanf(argv[4],"%s",Pname));
	assert(sscanf(argv[5],"%s",Nname));
	assert(sscanf(argv[6],"%s",outname));
	assert(sscanf(argv[7],"%d",&CrossCorrelate));
	assert(sscanf(argv[8],"%d",&sk));
	b.SetSkip(sk);
	//	Vx2 = vx; Vy2 = vy ; Vz2 = vz;
	//	vx *= 2.0;vy *= 2.0;vz *= 2.0;
	//	Vx = vx; Vy = vy ; Vz = vz;
	DoPNCorr(b,Nname,Pname,Vx,Vy,Vz,nframes,dt,outname);

	break;

      case 4:
	// extract atom list time series
	//  args bmtname idlist nframes outputfilename
	if(argc < 5)
	  { printf("usage: %s bmtname idlist nframes outputfilename\n",argv[0]); break;}
	assert(sscanf(argv[2],"%s",fname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%s",outname));
	ExtractAtomListTimeSeries(b,fname,nframes,outname,0);
	break;

      case 5:
	// calculation of diffusion for atom list 
	//args bmtname idlist nframes skip timelag timestep average DistanceDimension center_file outname
	if(argc < 11)
	  { printf("usage: %s bmtname idlist nframes skip timelagframes timestep_per_frame average DistanceDimension center_file outname\n",argv[0]); break;}
	assert(sscanf(argv[2],"%s",fname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%d",&skip));
	assert(sscanf(argv[5],"%d",&lag));
	assert(sscanf(argv[6],"%lf",&dt));
	assert(sscanf(argv[7],"%d",&average));
	assert(sscanf(argv[8],"%d",&distdim));
	assert(sscanf(argv[9],"%s",cent_file));
	assert(sscanf(argv[10],"%s",outname));
	b.SetSkip(skip);
	D = RMSDFromAtomList(b,fname,nframes,lag,dt,average,distdim,cent_file,outname);
	printf("N lag msd %d %d %g\n" , (nframes - lag), lag, D);
	break;
      case 6:
		// extract atom list time series
	//  args bmtname idlist nframes outputfilename
	if(argc < 5)
	  { printf("usage: %s bmtname idlist nframes outputfilename\n",argv[0]); break;}
	assert(sscanf(argv[2],"%s",fname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%s",outname));
	ExtractAtomListTimeSeries(b,fname,nframes,outname,1);
	break;

      case 7:
	// stats on dihedral angle
	if(argc < 9)
	  { printf("usage: %s bmtname idlist1 idlist2 idlist3 idlist4  nframes timestep outputseedname\n",argv[0]); break;}
	assert(sscanf(argv[2],"%s",idlist1));
	assert(sscanf(argv[3],"%s",idlist2));
	assert(sscanf(argv[4],"%s",idlist3));
	assert(sscanf(argv[5],"%s",idlist4));
	assert(sscanf(argv[6],"%d",&nframes));
	assert(sscanf(argv[7],"%lf",&dt));
	assert(sscanf(argv[8],"%s",outname));
	TorsionStats(b,idlist1,idlist2,idlist3,idlist4,nframes,dt,outname);
	break;
      case 8:

	// Test for Scd of atom pairs
	// args bmtname volx/2 voly/2 volz/2  idname1 idname2  skip nframes outname
	if(argc < 7)
	  { printf("usage: %s bmtname   idname1 idname2 skip nframes outname\n",argv[0]); break;}
	  
	//	assert(sscanf(argv[2],"%lf",&vx));
	//	assert(sscanf(argv[3],"%lf",&vy));
	//	assert(sscanf(argv[4],"%lf",&vz));
	assert(sscanf(argv[2],"%s",Pname));
	assert(sscanf(argv[3],"%s",Nname));
	assert(sscanf(argv[4],"%d",&skip));
	assert(sscanf(argv[5],"%d",&nframes));
	assert(sscanf(argv[6],"%s",outname));

	//	Vx2 = vx; Vy2 = vy ; Vz2 = vz;
	//	vx *= 2.0;vy *= 2.0;vz *= 2.0;
	//	Vx = vx; Vy = vy ; Vz = vz;
	b.SetSkip(skip);

	
	DoScd(b,Pname,Nname, nframes,&Scd,&er);
	OutputScd(outname, Pname,Nname,Scd,er);


	break;
      case 9:
	// DumpWaterOrientations 
	// args bmtname volx/2 voly/2 volz/2  idname1 idname2  skip nframes outname
	if(argc < 9 )
	  { printf("usage: %s bmtname volx/2 voly/2 volz/2  idname1  skip nframes outname\n",argv[0]); break;}
	  
	assert(sscanf(argv[2],"%lf",&vx));
	assert(sscanf(argv[3],"%lf",&vy));
	assert(sscanf(argv[4],"%lf",&vz));
	assert(sscanf(argv[5],"%s",Pname));
	assert(sscanf(argv[6],"%d",&skip));
	assert(sscanf(argv[7],"%d",&nframes));
	assert(sscanf(argv[8],"%s",outname));

	Vx2 = vx; Vy2 = vy ; Vz2 = vz;
	vx *= 2.0;vy *= 2.0;vz *= 2.0;
	Vx = vx; Vy = vy ; Vz = vz;
	b.SetSkip(skip);

	DumpWaterOrientation(b,Pname,nframes,outname);

	break;
      case 10:
	if (argc < 8)
	  {
	    printf("usage: %s bmtname idlist nframes interval timestep cent_file outname\n",argv[0]);
	    break;
	  }
	assert(sscanf(argv[2],"%s",Pname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%d",&skip));
	assert(sscanf(argv[5],"%lf",&dt));
	assert(sscanf(argv[6],"%s",cent_file));
	assert(sscanf(argv[7],"%s",outname));
	DoGroupCrossCorrelation(b,Pname,nframes,skip,dt,cent_file,outname);
	break;
      case 11:
	if (argc < 8)
	  {
	    printf("usage: %s bmtname idlist nframes interval timestep cent_file outname\n",argv[0]);
	    break;
	  }
	assert(sscanf(argv[2],"%s",Pname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%d",&skip));
	assert(sscanf(argv[5],"%lf",&dt));
	assert(sscanf(argv[6],"%s",cent_file));
	assert(sscanf(argv[7],"%s",outname));
	DoGroupDistanceCorrelation(b,Pname,nframes,skip,dt,cent_file,outname);
	break;

      case 12:
	if (argc < 8)
	  {
	    printf("usage: %s bmtname idlist nframes interval timestep cent_file outname\n",argv[0]);
	    break;
	  }
	assert(sscanf(argv[2],"%s",Pname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%d",&skip));
	assert(sscanf(argv[5],"%lf",&dt));
	assert(sscanf(argv[6],"%s",cent_file));
	assert(sscanf(argv[7],"%s",outname));
	DoGroupDisplacementCorrelation(b,Pname,nframes,skip,dt,cent_file,outname);
	break;
      case 13:
	if (argc < 8)
	  {
	    printf("usage: %s bmtname idlist nframes interval timestep cent_file outname\n",argv[0]);
	    break;
	  }
	assert(sscanf(argv[2],"%s",Pname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%d",&skip));
	assert(sscanf(argv[5],"%lf",&dt));
	assert(sscanf(argv[6],"%s",cent_file));
	assert(sscanf(argv[7],"%s",outname));
	DoGroupRotationCorrelation(b,Pname,nframes,skip,dt,cent_file,outname);
	break;


      case 14:

	// Histogram of group along Z
	// args bmtname  nbins idname  slab_thicknes skip nframes outname
	if(argc < 9)
	  { printf("usage: %s bmtname idname1 nbins nelect skip nframes cent_ids outname\n",argv[0]); break;}
	  
	//	assert(sscanf(argv[2],"%lf",&vx));
	//	assert(sscanf(argv[3],"%lf",&vy));
	//	assert(sscanf(argv[4],"%lf",&vz));
	assert(sscanf(argv[2],"%s",Pname));
	assert(sscanf(argv[3],"%d",&nbins));
	assert(sscanf(argv[4],"%d",&nelect));
	assert(sscanf(argv[5],"%d",&skip));
	assert(sscanf(argv[6],"%d",&nframes));
	assert(sscanf(argv[7],"%s",cent_file));
	assert(sscanf(argv[8],"%s",outname));

	b.SetSkip(skip);

	allhist = new int[nbins + 1];

	binwidth = Vz / nbins ;
	total = 0;

	assert(ReadIdList(cent_file,&nats,&ids));
	b.SetCentIds(ids,nats);


	for(int i = 0;i < nbins; i++){allhist[i] = 0;}
	DoZHist(b,Pname,binwidth, nbins,nframes,allhist,&atotal);
	OutputZHist(outname,binwidth,nbins,nframes,allhist,atotal,nelect);
	break;
	

      case 15:

	// Find closest atom in group to a group of vectors (for nonbonded interaction)
	// args bmtname  nbins idname  slab_thicknes skip nframes outname
	if(argc < 8)
	  { printf("usage: %s bmtname vecname searchname dist skip nframes outname\n",argv[0]); break;}
	  
	//	assert(sscanf(argv[2],"%lf",&vx));
	//	assert(sscanf(argv[3],"%lf",&vy));
	//	assert(sscanf(argv[4],"%lf",&vz));
	assert(sscanf(argv[2],"%s",Pname));
	assert(sscanf(argv[3],"%s",Nname));
	assert(sscanf(argv[4],"%lf",&dt));
	assert(sscanf(argv[5],"%d",&skip));
	assert(sscanf(argv[6],"%d",&nframes));
	assert(sscanf(argv[7],"%s",outname));

	b.SetSkip(skip);

	FindClosest(b,Pname,Nname,dt,nframes,outname);
	break;
      case 16:

	// Extract atom group list
	// args bmtname center_file nframes skip outname

	//  args bmtname idlist nframes outputfilename
	if(argc < 8)
	  { printf("usage: %s bmtname idlist nframes skip grouplen center_file outputfilename\n",argv[0]); break;}
	assert(sscanf(argv[2],"%s",fname));
	assert(sscanf(argv[3],"%d",&nframes));
	assert(sscanf(argv[4],"%d",&skip));
	assert(sscanf(argv[5],"%d",&length));
	assert(sscanf(argv[6],"%s",cent_file));
	assert(sscanf(argv[7],"%s",outname));

	b.SetSkip(skip);
	assert(ReadIdList(cent_file,&nats,&ids));
	b.SetCentIds(ids,nats);
	ExtractAtomGroupListTimeSeries(b,fname,nframes,length,2,outname);


	break;

      default:
	PrintTestMenu();

	break;
      }
    return 0;
} 
void PrintTestMenu()
{
	printf("Usage bmtread (args... ) < testId \n");
	printf("\tTest\tDescription\n");
	printf("\t 1 \tDumps Frame Coordinates\n");
	printf("\t 2 \tAtom Pair Radial Distribution\n");
	printf("\t 3 \tAtom Pair Correlation \n");
	printf("\t 4 \tCreate Time Series file from atom ids\n");
	printf("\t 5 \tRMSD for atom list and time lag\n");
	printf("\t 6 \tCreate Time Series file from group average atom ids\n");
	printf("\t 7 \tTorsion correlation\n");
	printf("\t 8 \tScd for Idlist Idlist\n");
	printf("\t 9 \tWater Orientation for OH2_list\n");
	printf("\t 10 \tCross correlate direction for idlist\n");
	printf("\t 11 \tCross correlate Distance for idlist\n");
	printf("\t 12 \tCross correlate Displacement for idlist\n");
	printf("\t 13 \tCross correlate Rotation for idlist\n");
	printf("\t 14 \tZ-Histogram idlist\n");
	printf("\t 15 \tClosest atom in group to a group of vectors\n");
	printf("\t 16 \tTime series for atom groupss\n");
	return;
}

int DoGroupCrossCorrelation(BMTReader &b, char *Pname, 
 int nframes,int skip, double timestep,char *cent_file,char *outname)
{
  int *ids1,*cents;
  int *ids2;
  int nats1,nats2,ncents;
  

  FILE *fPF = fopen(outname,"w");

  assert(ReadIdList(Pname,&nats1,&ids1));
  assert(ReadIdList(cent_file,&ncents,&cents));
  b.SetCentIds(cents,ncents);

  float ***AvePos;
  float ***Xcor;

  int nwin = nframes / skip;

  printf("Allocating memory for %d atoms and %d windows of %d\n",nats1,nwin,skip);
  // avePos[atom mem num][window][coord]
  AvePos = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      AvePos[i] = new float*[nwin + 1];
      for(int j = 0; j <= nwin ; j++)
	{
	  AvePos[i][j] = new float[3];
	  for(int k = 0 ; k < 3 ; k++)
	    AvePos[i][j][k] = 0.0;
	}
    }
    
  // Xcor[atom mem num][atom mem num][window]
  Xcor = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      Xcor[i] = new float*[nats1];
      for(int j = 0; j < nats1 ; j++)
	{
	  Xcor[i][j] = new float[nwin];
	  for(int k = 0;k < nwin; k++)
	    Xcor[i][j][k] = 0.0;
	}
    }
  b.BufferAtomList(nframes,nats1, ids1, 1);  

  printf("Averaging positions over windows\n");
  for(int k = 0 ; k< nats1; k++)
    { 
      int at = ids1[k] - 1;
      for(int i = 0; i < nwin; i++)
	{
	  for(int j = 0; j < skip; j++)
	    {
	      int fr = i*skip + j;
	      double xx,yy,zz;
	      b.GetVector(at,fr,xx,yy,zz);
	      AvePos[k][i][0] += xx;
	      AvePos[k][i][1] += yy;
	      AvePos[k][i][2] += zz;
	    }
	  AvePos[k][i][0] /= skip;
	  AvePos[k][i][1] /= skip;
	  AvePos[k][i][2] /= skip;
	  //if(k == 0)
	  //  {printf("first mem pos win %d: %f %f %f\n",i,AvePos[k][i][0],AvePos[k][i][1],AvePos[k][i][2]);}
	}
    }

  nwin--;

  printf("Converting averaged positions into directions\n");
  // turn AvePos into directions
  for(int k = 0 ; k< nats1; k++)
    { 
      for(int i = 0; i < nwin; i++)
	{
	  AvePos[k][i][0] = AvePos[k][i+1][0] -  AvePos[k][i][0];
	  AvePos[k][i][1] = AvePos[k][i+1][1] -  AvePos[k][i][1];
	  AvePos[k][i][2] = AvePos[k][i+1][2] -  AvePos[k][i][2];
	  float d = sqrt(AvePos[k][i][0] * AvePos[k][i][0] +
			 AvePos[k][i][1] * AvePos[k][i][1] +
			 AvePos[k][i][2] * AvePos[k][i][2]);
	  AvePos[k][i][0] /= d;
	  AvePos[k][i][1] /= d;
	  AvePos[k][i][2] /= d;

	  //	  if(k == 0)
	  //	    {printf("first mem dir win %d: %f %f %f\n",i,AvePos[k][i][0],AvePos[k][i][1],AvePos[k][i][2]);}
	}

    }  


  printf("Computing directional dot products\n");
  // compute accumulate cross correlation matrix
  for(int i = 0; i < nats1-1; i++)
    {
      for(int j = i; j < nats1; j++)
	{
	  for(int k = 0; k < nwin;k++)
	    {
	      double d = AvePos[i][k][0] * AvePos[j][k][0] +
		AvePos[i][k][1] * AvePos[j][k][1] +
		AvePos[i][k][2] * AvePos[j][k][2] ;
	      
	      Xcor[i][j][k] = d; //1.5 * d * d - 0.5;
	      //	      if(i == 0 && j == 1)
	      //		{printf("cross cor %d %d %d %f\n",i,j,k,Xcor[i][j][k]);}

	    }
	}
    }


  float *cor,*corave;
  cor = new float[nwin];
  corave = new float[nwin];

  for(int dt = 0; dt < nwin; dt++)
      corave[dt] = 0.0;

  int ncor = 0;
  printf("Accumulating directional cross correlation\n");
  for(int i = 0; i < nats1 - 1; i++)  
    {
      for(int j = i+1; j < nats1; j++)	
	{
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] = 0.0;
	      int cnt = 0;
	      for(int k = 0;k < nwin - dt ; k++)
		{
		  cor[dt] += Xcor[i][j][k] * Xcor[i][j][k+dt];
		  cnt++;
		}
	      cor[dt] /= cnt;
	    }
	  double C = cor[0];
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] /= C;
	      corave[dt] += cor[dt];
	    }
	  ncor++;
	}            
    }
  float norm = ncor;
  for(int dt = 0; dt < nwin; dt++)
    corave[dt] /= norm;

  printf("Done. Writing result to %s\n", outname);
  for(int dt = 0; dt < nwin; dt++)
    {
      float t = dt * skip * timestep;
      fprintf(fPF,"%f %g\n",t,corave[dt]);
    }
  // ooof - big memory leak coming up!
  return 1;
}

int DoGroupDistanceCorrelation(BMTReader &b, char *Pname, 
 int nframes,int skip, double timestep,char *cent_file,char *outname)
{
  int *ids1,*cents;
  int *ids2;
  int nats1,nats2,ncents;
  

  FILE *fPF = fopen(outname,"w");

  assert(ReadIdList(Pname,&nats1,&ids1));
  assert(ReadIdList(cent_file,&ncents,&cents));
  b.SetCentIds(cents,ncents);

  float ***AvePos;
  float ***Xcor;

  int nwin = nframes / skip;

  printf("Allocating memory for %d atoms and %d windows of %d\n",nats1,nwin,skip);
  // avePos[atom mem num][window][coord]
  AvePos = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      AvePos[i] = new float*[nwin + 1];
      for(int j = 0; j <= nwin ; j++)
	{
	  AvePos[i][j] = new float[3];
	  for(int k = 0 ; k < 3 ; k++)
	    AvePos[i][j][k] = 0.0;
	}
    }
    
  // Xcor[atom mem num][atom mem num][window]
  Xcor = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      Xcor[i] = new float*[nats1];
      for(int j = 0; j < nats1 ; j++)
	{
	  Xcor[i][j] = new float[nwin];
	  for(int k = 0;k < nwin; k++)
	    Xcor[i][j][k] = 0.0;
	}
    }
  b.BufferAtomList(nframes,nats1, ids1, 1);  

  printf("Averaging positions over windows\n");
  for(int k = 0 ; k< nats1; k++)
    { 
      int at = ids1[k] - 1;
      for(int i = 0; i < nwin; i++)
	{
	  for(int j = 0; j < skip; j++)
	    {
	      int fr = i*skip + j;
	      double xx,yy,zz;
	      b.GetVector(at,fr,xx,yy,zz);
	      AvePos[k][i][0] += xx;
	      AvePos[k][i][1] += yy;
	      AvePos[k][i][2] += zz;
	    }
	  AvePos[k][i][0] /= skip;
	  AvePos[k][i][1] /= skip;
	  AvePos[k][i][2] /= skip;
	  //if(k == 0)
	  //  {printf("first mem pos win %d: %f %f %f\n",i,AvePos[k][i][0],AvePos[k][i][1],AvePos[k][i][2]);}
	}
    }



  printf("Computing directional dot products\n");
  // compute accumulate cross correlation matrix
  for(int i = 0; i < nats1-1; i++)
    {
      for(int j = i; j < nats1; j++)
	{
	  for(int k = 0; k < nwin;k++)
	    {
	      double dx = AvePos[i][k][0] - AvePos[j][k][0];
	      double dy = AvePos[i][k][1] - AvePos[j][k][1] ;
	      double dz = AvePos[i][k][2] - AvePos[j][k][2] ;
	      double d = sqrt(dx*dx + dy*dy+ dz*dz);
	      
	      Xcor[i][j][k] = d;
	      //	      if(i == 0 && j == 1)
	      //		{printf("cross cor %d %d %d %f\n",i,j,k,Xcor[i][j][k]);}

	    }
	}
    }


  float *cor,*corave;
  cor = new float[nwin];
  corave = new float[nwin];

  for(int dt = 0; dt < nwin; dt++)
      corave[dt] = 0.0;

  int ncor = 0;
  printf("Accumulating 1/r^6 cross correlation\n");
  for(int i = 0; i < nats1 - 1; i++)  
    {
      for(int j = i+1; j < nats1; j++)	
	{
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] = 0.0;
	      int cnt = 0;
	      for(int k = 0;k < nwin - dt ; k++)
		{
		  cor[dt] += Xcor[i][j][k] * Xcor[i][j][k+dt];
		  cnt++;
		}
	      cor[dt] /= cnt;
	    }
	  double C =  cor[0];
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] /= C;
	      corave[dt] += cor[dt];
	    }
	  ncor++;
	}            
    }
  float norm = ncor;
  for(int dt = 0; dt < nwin; dt++)
    corave[dt] /= norm;

  printf("Done. Writing result to %s\n", outname);
  for(int dt = 0; dt < nwin; dt++)
    {
      float t = dt * skip * timestep;
      fprintf(fPF,"%f %g\n",t,corave[dt]);
    }
  // ooof - big memory leak coming up!
  return 1;
}




int DoGroupDisplacementCorrelation(BMTReader &b, char *Pname, 
 int nframes,int skip, double timestep,char *cent_file,char *outname)
{
  int *ids1,*cents;
  int *ids2;
  int nats1,nats2,ncents;
  

  FILE *fPF = fopen(outname,"w");

  assert(ReadIdList(Pname,&nats1,&ids1));
  assert(ReadIdList(cent_file,&ncents,&cents));
  b.SetCentIds(cents,ncents);

  float ***AvePos;
  float ***Xcor;

  int nwin = nframes / skip;

  printf("Allocating memory for %d atoms and %d windows of %d\n",nats1,nwin,skip);
  // avePos[atom mem num][window][coord]
  AvePos = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      AvePos[i] = new float*[nwin + 1];
      for(int j = 0; j <= nwin ; j++)
	{
	  AvePos[i][j] = new float[3];
	  for(int k = 0 ; k < 3 ; k++)
	    AvePos[i][j][k] = 0.0;
	}
    }
    
  // Xcor[atom mem num][atom mem num][window]
  Xcor = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      Xcor[i] = new float*[nats1];
      for(int j = 0; j < nats1 ; j++)
	{
	  Xcor[i][j] = new float[nwin];
	  for(int k = 0;k < nwin; k++)
	    Xcor[i][j][k] = 0.0;
	}
    }
  b.BufferAtomList(nframes,nats1, ids1, 1);  

  printf("Averaging positions over windows\n");
  for(int k = 0 ; k< nats1; k++)
    { 
      int at = ids1[k] - 1;
      for(int i = 0; i < nwin; i++)
	{
	  for(int j = 0; j < skip; j++)
	    {
	      int fr = i*skip + j;
	      double xx,yy,zz;
	      b.GetVector(at,fr,xx,yy,zz);
	      AvePos[k][i][0] += xx;
	      AvePos[k][i][1] += yy;
	      AvePos[k][i][2] += zz;
	    }
	  AvePos[k][i][0] /= skip;
	  AvePos[k][i][1] /= skip;
	  AvePos[k][i][2] /= skip;
	  //if(k == 0)
	  //  {printf("first mem pos win %d: %f %f %f\n",i,AvePos[k][i][0],AvePos[k][i][1],AvePos[k][i][2]);}
	}
    }


  nwin--;

  printf("Converting averaged positions into displacements\n");
  // turn AvePos into directions
  for(int k = 0 ; k< nats1; k++)
    { 
      for(int i = 0; i < nwin; i++)
	{
	  AvePos[k][i][0] = AvePos[k][i+1][0] -  AvePos[k][i][0];
	  AvePos[k][i][1] = AvePos[k][i+1][1] -  AvePos[k][i][1];
	  AvePos[k][i][2] = AvePos[k][i+1][2] -  AvePos[k][i][2];

	  //	  if(k == 0)
	  //	    {printf("first mem dir win %d: %f %f %f\n",i,AvePos[k][i][0],AvePos[k][i][1],AvePos[k][i][2]);}
	}

    }  



  printf("Computing directional dot products\n");
  // compute accumulate cross correlation matrix
  for(int i = 0; i < nats1-1; i++)
    {
      for(int j = i; j < nats1; j++)
	{
	  for(int k = 0; k < nwin;k++)
	    {
	      double dx = AvePos[i][k][0] * AvePos[j][k][0];
	      double dy = AvePos[i][k][1] * AvePos[j][k][1] ;
	      double dz = AvePos[i][k][2] * AvePos[j][k][2] ;
	      double d =  dx + dy + dz;
	      
	      Xcor[i][j][k] = d;
	      //	      if(i == 0 && j == 1)
	      //		{printf("cross cor %d %d %d %f\n",i,j,k,Xcor[i][j][k]);}

	    }
	}
    }


  float *cor,*corave;
  cor = new float[nwin];
  corave = new float[nwin];

  for(int dt = 0; dt < nwin; dt++)
      corave[dt] = 0.0;

  int ncor = 0;
  printf("Accumulating directional cross correlation\n");
  for(int i = 0; i < nats1 - 1; i++)  
    {
      for(int j = i+1; j < nats1; j++)	
	{
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] = 0.0;
	      int cnt = 0;
	      for(int k = 0;k < nwin - dt ; k++)
		{
		  cor[dt] += Xcor[i][j][k] * Xcor[i][j][k+dt];
		  cnt++;
		}
	      cor[dt] /= cnt;
	    }
	  double C =  cor[0];
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] /= C;
	      corave[dt] += cor[dt];
	    }
	  ncor++;
	}            
    }
  float norm = ncor;
  for(int dt = 0; dt < nwin; dt++)
    corave[dt] /= norm;

  printf("Done. Writing result to %s\n", outname);
  for(int dt = 0; dt < nwin; dt++)
    {
      float t = dt * skip * timestep;
      fprintf(fPF,"%f %g\n",t,corave[dt]);
    }
  // ooof - big memory leak coming up!
  return 1;
}



int DoGroupRotationCorrelation(BMTReader &b, char *Pname, 
 int nframes,int skip, double timestep,char *cent_file,char *outname)
{
  int *ids1,*cents;
  int *ids2;
  int nats1,nats2,ncents;
  

  FILE *fPF = fopen(outname,"w");

  assert(ReadIdList(Pname,&nats1,&ids1));
  assert(ReadIdList(cent_file,&ncents,&cents));
  b.SetCentIds(cents,ncents);

  float ***AvePos;
  float ***Xcor;

  int nwin = nframes / skip;

  printf("Allocating memory for %d atoms and %d windows of %d\n",nats1,nwin,skip);
  // avePos[atom mem num][window][coord]
  AvePos = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      AvePos[i] = new float*[nwin + 1];
      for(int j = 0; j <= nwin ; j++)
	{
	  AvePos[i][j] = new float[3];
	  for(int k = 0 ; k < 3 ; k++)
	    AvePos[i][j][k] = 0.0;
	}
    }
    
  // Xcor[atom mem num][atom mem num][window]
  Xcor = new float**[nats1];
  for(int i = 0 ; i < nats1 ; i++)
    {
      Xcor[i] = new float*[nats1];
      for(int j = 0; j < nats1 ; j++)
	{
	  Xcor[i][j] = new float[nwin];
	  for(int k = 0;k < nwin; k++)
	    Xcor[i][j][k] = 0.0;
	}
    }
  b.BufferAtomList(nframes,nats1, ids1, 1);  

  printf("Averaging positions over windows\n");
  for(int k = 0 ; k< nats1; k++)
    { 
      int at = ids1[k] - 1;
      for(int i = 0; i < nwin; i++)
	{
	  for(int j = 0; j < skip; j++)
	    {
	      int fr = i*skip + j;
	      double xx,yy,zz;
	      b.GetVector(at,fr,xx,yy,zz);
	      AvePos[k][i][0] += xx;
	      AvePos[k][i][1] += yy;
	      AvePos[k][i][2] += zz;
	    }
	  AvePos[k][i][0] /= skip;
	  AvePos[k][i][1] /= skip;
	  AvePos[k][i][2] /= skip;
	  //if(k == 0)
	  //  {printf("first mem pos win %d: %f %f %f\n",i,AvePos[k][i][0],AvePos[k][i][1],AvePos[k][i][2]);}
	}
    }


  nwin--;

  printf("Converting averaged positions into displacements\n");
  // turn AvePos into directions
  for(int k = 0 ; k< nats1; k++)
    { 
      for(int i = 0; i < nwin; i++)
	{
	  AvePos[k][i][0] = AvePos[k][i+1][0] -  AvePos[k][i][0];
	  AvePos[k][i][1] = AvePos[k][i+1][1] -  AvePos[k][i][1];
	  AvePos[k][i][2] = AvePos[k][i+1][2] -  AvePos[k][i][2];

	  //	  if(k == 0)
	  //	    {printf("first mem dir win %d: %f %f %f\n",i,AvePos[k][i][0],AvePos[k][i][1],AvePos[k][i][2]);}
	}

    }  


  nwin--;


  printf("Computing directional dot products\n");
  // compute accumulate cross correlation matrix
  for(int i = 0; i < nats1-1; i++)
    {
      for(int j = i; j < nats1; j++)
	{
	  for(int k = 0; k < nwin;k++)
	    {
	      //	      double dxi = AvePos[i][k][1] * AvePos[i][k+1][2] - AvePos[i][k][2] * AvePos[i][k+1][1];
	      //	      double dyi = AvePos[i][k][2] * AvePos[i][k+1][0] - AvePos[i][k][0] * AvePos[i][k+1][2];
	      double dzi = AvePos[i][k][0] * AvePos[i][k+1][1] - AvePos[i][k][1] * AvePos[i][k+1][0];
	      //	      double dxj = AvePos[j][k][1] * AvePos[j][k+1][2] - AvePos[j][k][2] * AvePos[j][k+1][1];
	      //	      double dyj = AvePos[j][k][2] * AvePos[j][k+1][0] - AvePos[j][k][0] * AvePos[j][k+1][2];
	      double dzj = AvePos[j][k][0] * AvePos[j][k+1][1] - AvePos[j][k][1] * AvePos[j][k+1][0];

	      // take only xy rotation 
	      double d =  dzi * dzj;
	      
	      Xcor[i][j][k] = d;
	      //	      if(i == 0 && j == 1)
	      //		{printf("cross cor %d %d %d %f\n",i,j,k,Xcor[i][j][k]);}

	    }
	}
    }


  float *cor,*corave;
  cor = new float[nwin];
  corave = new float[nwin];

  for(int dt = 0; dt < nwin; dt++)
      corave[dt] = 0.0;

  int ncor = 0;
  printf("Accumulating directional cross correlation\n");
  for(int i = 0; i < nats1 - 1; i++)  
    {
      for(int j = i+1; j < nats1; j++)	
	{
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] = 0.0;
	      int cnt = 0;
	      for(int k = 0;k < nwin - dt ; k++)
		{
		  cor[dt] += Xcor[i][j][k] * Xcor[i][j][k+dt];
		  cnt++;
		}
	      cor[dt] /= cnt;
	    }
	  double C =  cor[0];
	  for(int dt = 0; dt < nwin; dt++)
	    {
	      cor[dt] /= C;
	      corave[dt] += cor[dt];
	    }
	  ncor++;
	}            
    }
  float norm = ncor;
  for(int dt = 0; dt < nwin; dt++)
    corave[dt] /= norm;

  printf("Done. Writing result to %s\n", outname);
  for(int dt = 0; dt < nwin; dt++)
    {
      float t = dt * skip * timestep;
      fprintf(fPF,"%f %g\n",t,corave[dt]);
    }
  // ooof - big memory leak coming up!
  return 1;
}


int DumpWaterOrientation(BMTReader &b,char *Pname,int nframes,char *outname)
{
  int *ids1;
  int nats1;
  char corname[256],histname[256];
  double D = 0.0;
  double x1,y1,z1,x2,y2,z2;
  double Hx1,Hy1,Hz1,Hx2,Hy2,Hz2;
  double dx1,dy1,dz1,dx2,dy2,dz2;
  FILE *fPF = fopen(outname,"w");

  assert(ReadIdList(Pname,&nats1,&ids1));


  int frame = 0 ;
  while(b.ReadFrame())
    {	      
      frame++;
      for(int ii=0;ii<nats1;ii++)
	{
	  b.GetVector(ids1[ii]-1,x1,y1,z1);
	  b.GetVector(ids1[ii],Hx1,Hy1,Hz1);
	  b.GetVector(ids1[ii]+1,Hx2,Hy2,Hz2);
	  dx1 = (Hx2 + Hx1) * 0.5;
	  dy1 = (Hy2 + Hy1) * 0.5;
	  dz1 = (Hz2 + Hz1) * 0.5;
	  dx2 = dx1 - x1;
	  dy2 = dy1 - y1;
	  dz2 = dz1 - z1;
	  double d = sqrt(dx2 * dx2 + dy2 * dy2 + dz2 * dz2 );
	  dx2 /= d;	  dy2 /= d;	  dz2 /= d;
	  fprintf(fPF, "%d %f %f %f %f %f %f\n",frame,x1,y1,z1,dx2,dy2,dz2);
	}
      
    }
  fclose(fPF);
  return 1;
  
}


int FindClosest(BMTReader &b,char *Pairname,char *Searchname,double dist,int nframes,char *outname)
{
  int *ids1,*ids2,*ids3;
  int nats1,nats3,Bid;
  double D = 0.0,BD;
  double x1,y1,z1,x2,y2,z2;
  double Hx1,Hy1,Hz1,Hx2,Hy2,Hz2;
  double dx1,dy1,dz1,dx2,dy2,dz2;
  FILE *fPF = fopen(outname,"w");

  assert(ReadIdPairList(Pairname,&nats1,&ids1,&ids2));
  assert(ReadIdList(Searchname,&nats3,&ids3));


  int frame = 0 ;
  while(b.ReadFrame())
    {	      
      frame++;
      for(int ii=0;ii<nats1;ii++)
	{
	  b.GetVector(ids1[ii]-1,x1,y1,z1);
	  b.GetVector(ids2[ii]-1,x2,y2,z2);
	  dx1 = x2 - x1;dy1 = y2 - y1;dz1 = z2 - z1;
	  D = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
	  dx1 *= dist/D;dy1 *= dist/D;dz1 *= dist/D;
	  dx1 = x2;dy1 += y2;dz1 += z2;

	  BD = 10000.; Bid = -1;
	  for(int jj=0;jj<nats3;jj++)
	    {
	      b.GetVector(ids3[jj],x2,y2,z2);	      
	      dx2 = x2 - dx1;dy2 = y2 - dy1;dz2 = z2 - dz1;
	      D = sqrt(dx2*dx2 + dy2*dy2 + dz2*dz2);
	      if(D < BD)
		{
		  BD = D;
		  Bid = jj;
		}
	    }
	  b.GetNearVector(ids3[Bid],-1,x1,y1,z1,x2,y2,z2);	      
	  
	  fprintf(fPF,"%5d\t%5d\t%5d\t%5d\t%8.3f\n",frame,ids1[ii],ids2[ii],ids3[Bid],BD);
	}
    }
  return 1;
}


int TorsionStats(BMTReader &b,char *idlist1,char *idlist2,char *idlist3,char *idlist4,int nframes,double dt, char *outname)
{
  int *ids1,*ids2,*ids3,*ids4;
  int nats1,nats2,nats3,nats4;
  char corname[256],histname[256];
  double D = 0.0;

  sprintf(corname,"%s_cor.dat",outname);
  sprintf(histname,"%s_hist.dat",outname);


  assert(ReadIdList(idlist1,&nats1,&ids1));
  assert(ReadIdList(idlist2,&nats2,&ids2));
  assert(ReadIdList(idlist3,&nats3,&ids3));
  assert(ReadIdList(idlist4,&nats4,&ids4));

  double *corr = new double[nframes];
  double *corrav = new double[nframes];
  allocsize += 2 *( nframes ) * sizeof(double);
    if(MEMLOG)
      {printf ("torsionstats frames %d total = %d\n",nframes,allocsize);}

  for(int i = 0; i < nframes; i++)
    corrav[i] = 0.0;


  b.BufferAtomList(nframes,nats1, ids1, 1);  
  b.BufferAtomList(nframes,nats2, ids2, 0);
  b.BufferAtomList(nframes,nats3, ids3, 0);
  b.BufferAtomList(nframes,nats4, ids4, 0);

  double theta,costheta;
  int binwidth = 5;
  int nbins = 360/binwidth;
  int sign;
  int *hist = new int[nbins+1];
  for(int i = 0;i<nbins;i++)
    {hist[i]=0;}

  for(int i = 0; i < nats1; i++)
    {
      for (int j = 0;j < nframes;j++)
	{
	  sign =  b.CosTorsion(j,ids1[i]-1,ids2[i]-1,ids3[i]-1,ids4[i]-1,costheta);
	  theta = acos(costheta) * 180.0 / 3.14159;
	  //	  printf("theta = %d %f  \n",sign, theta);
	  if(sign > 0)
	    {
	      int k = nbins/2 + theta / binwidth;
	      hist[k]++;
	    }
	  else
	    {
	      int k = nbins/2 - theta/binwidth ;
	      hist[k]++;
	    }
	}
    }
  double tot = nats1 * nframes;
  FILE *fPF = fopen(histname,"w");
  for(int i = 0;i<nbins/2;i++)
    {
      int b;
      double r;
      b = nbins/2 + i;
      r = hist[b] / tot;
      fprintf(fPF,"%d %g\n",b,r);
      b = nbins/2 - i ;
      r = hist[b] / tot;
      fprintf(fPF,"%d %g\n",b,r);
    }
  fclose(fPF);
  double plat = 0.0;

  for(int i = 0; i < nats1; i++)
    {
      b.CorrelateTorsion(ids1[i]-1,ids2[i]-1,ids3[i]-1,ids4[i]-1,corr);
      for(int j = 0; j < nframes; j++)
	corrav[j] += corr[j];
      // hack for now, just passing average twice instead of variance
      OutputCorrFunction(corname, nframes, corrav,corrav, dt,plat);
    }
  for(int j = 0; j < nframes; j++)
    corrav[j] /= nats1;
  for(int j = 0; j < nframes; j++)
    corrav[j] /= corrav[0];
  OutputCorrFunction(corname, nframes, corrav,corrav, dt, plat);
  return 1;
}


double DimDist(int distdim,double x1, double x2, double y1,double y2, double z1, double z2); 
double DimDist(int distdim,double x1, double x2, double y1,double y2, double z1, double z2) 
{
  double d2,d;
  switch(distdim)
    {
    case 3:
      d2 = SQDist(x1,y1,z1,x2,y2,z2) / 6.0;
      break;
    case 2:
      d = SQDist2D(x1,y1,x2,y2);
      //printf("lag %d d = %f\n",lag, sqrt(d));
      d2 = d * .25;
      break;
    case 1:
      d2 = SQDist1D(z1,z2) *.5;
      break;
    default:
      assert(0);
    }
  return d2;
}

double RMSDFromAtomList(BMTReader &b,char *fname,int nframes,int lagframes,double timestep,
			int average, int DistanceDimension, char *cent_file,char *outname)
{

  // DistanceDimension 1: Z only; 2: XY only; 3: 3D
  int skip = b.GetSkip();
  int nats,ncents;
  int *ids,*centids;
  double D = 0.0,T;
  FILE *fPF = fopen(outname,"w");;
  assert(ReadIdList(cent_file,&ncents,&centids));
  b.SetCentIds(centids,ncents);
  assert(ReadIdList(fname,&nats,&ids));
  b.BufferAtomList(nframes,nats, ids,1,1);
  int count = 0;
  int grouplen = ids[1] - ids[0];

  double time_per_frame = timestep * skip;
  double lag_time = time_per_frame * lagframes;

  printf("RMSD nframes %d skip %d  time per frame (ps) %f lag time (ps) %f\n", nframes,skip, time_per_frame, lag_time );


#if 0
  {
    FILE *afPF = fopen("GroupCOM.pos","w");
    double x1,y1,z1;
    for(int t = 0;t < nframes;t++)
      {
	for(int i = 0;i<nats;i++)
	  {
	    int a = ids[i] - 1;
	    b.GetGroupCOM(a,t,grouplen,x1,y1,z1);
	    fprintf(afPF,"\t%f \t%f \t%f      ",x1,y1,z1);
	  }
	fprintf(afPF,"\n");
      }
    
    fclose(afPF);
  }
#endif

  for(int lag = lagframes; lag < nframes; lag+=lagframes) //in terms of frames
    {
      D = 0.0;
      int nlag = nframes - lag;
      count = 0;
      for(int t = 0; t < nlag; t++)
	{
	  int tt = t + lag;
	  double d2 = 0.0;
	  double d;
	  double x0,y0,z0;
	  double x1,y1,z1;
	  double x2,y2,z2;
	  
	  for(int i = 0; i < nats; i++)
	    {
	      int a = ids[i] - 1;
	      if( average)
		{ 
		  b.GetGroupCOM(a,t,grouplen,x1,y1,z1);
		  b.GetGroupCOM(a,tt,grouplen,x2,y2,z2);
		}
	      else
		{
		  b.GetVector(a,t,x1,y1,z1);
		  b.GetVector(a,tt,x2,y2,z2);
		}
	      
	      d2 += DimDist(DistanceDimension,x1,x2,y1,y2,z1,z2);
	    }
	  d2 /= nats;
	  D += d2;
	  count++;	      
	}// end lag average
      D /= count;
      T = time_per_frame * lag;

      fprintf(fPF,"%f %g\n",T,D); 
      printf("%f %g\n",T,D); 
    }// end lag

  
  return D;
}

int ExtractAtomListTimeSeries(BMTReader &b,char *fname,int nframes,char *outname, 
			      int average = 0)
{
  int nats;
  int *ids;

  assert(ReadIdList(fname,&nats,&ids));
  b.BufferAtomList(nframes,nats, ids);
  FILE *fPF = fopen(outname,"w");
  for(int t = 0; t < nframes; t++)
    {
      double x0,y0,z0;
      x0 = y0 = z0 = 0.0;
      for(int i = 0; i < nats; i++)
	{
	  double xx,yy,zz;
	  if(average)
	    {
	      b.GetVector(ids[i]-1,t,xx,yy,zz);
	      x0 += xx;   y0 += yy;   z0 += zz;
	    }
	  else
	    {
	      b.GetVector(ids[i]-1,t,xx,yy,zz);
	      fprintf(fPF,"%12.6f %12.6f %12.6f   ",xx,yy,zz);
	    }
	}
      if(average)
	{
	  x0 /= nats;   y0 /= nats;    z0 /= nats;
	  fprintf(fPF,"%12.6f %12.6f %12.6f   \n",x0,y0,z0);
	}
      else
	fprintf(fPF,"\n");
    }
  fclose(fPF);
  return 1;
}
int DoPNCorr(BMTReader &b, char *Nname,char *Pname, 
	     double vx, double vy, double vz,int nf,double dt,char *outname)
{
  int nNats,nPats;
  int *Nids,*Pids;
  int delta = dt * b.GetSkip();
  int nframes = nf / b.GetSkip();
  assert(ReadIdList(Nname,&nNats,&Nids));
  assert(ReadIdList(Pname,&nPats,&Pids));

  printf("total = %d skipping %d actual %d frames\n",nf,b.GetSkip(),nframes);



#define DO_FULL

  double *corrvar = new double[nframes];
  double *corrav = new double[nframes];
  int *N = new int[nframes];
  allocsize += 2 *( nframes ) * sizeof(double);
    if(MEMLOG)
      {printf ("DoPNCorr  nframes %d total = %d\n",nframes,allocsize);}

  for(int i = 0; i < nframes; i++)
    {corrav[i] = corrvar[i] = 0.0; N[i] = 0;}

  b.BufferAtomList(nframes,nPats,Pids,1);
  b.BufferAtomList(nframes,nNats,Nids,0);

  double x,tx,plat = 0;
  tx = 0;
  int txc = 0;
  int ncor = 0;
  if(CROSS_CORRELATE)
    {
      plat = 0.0;
      for(int i = 0; i < nPats; i++)
	{
	  for(int j = 0; j < nNats; j++)
	    {
	      if(Pids[i] != Nids[j])
		 {
		   b.CrossCorrelateAtomPair(Pids[i]-1,Nids[j]-1,corrav,corrvar,N);
		 }
	    }
	  OutputCorrFunction(outname, nframes, corrav, corrvar,delta, plat);

	  int per = 100 * i / nPats;
	  printf("Progress %d %% \r",per); fflush(stdout);
	}
      double xx = 4*3.14159;
      for(int k=0;k<nframes;k++)
	{
	  corrav[k] /= xx;
	  corrvar[k] /= xx*xx;
	}
#if 0
      for(int i = 0; i < nNats; i++)
	{
	  for(int j = 0 ; j < nPats; j++) 
	  {
	    if(Pids[j] != Nids[i])
	      {
		b.AtomPairPlateau(Pids[j]-1,Nids[i]-1,x);
		tx += x;
		txc++;
	      }
	  }
	}
      
      tx /= txc;
      plat = tx*tx;
      printf("Plateau value = %g\n\n",plat);
#endif      

    }
  else // autocorrelate
    {

      double pl = 0.0;
      plat = 0.0;
      
      for(int i = 0; i < nNats; i++)
	{
	  if(Pids[i] != Nids[i])
	    {
	      b.CorrelateAtomPair(Pids[i]-1,Nids[i]-1,corrav,corrvar,N,pl);
	      //	      b.CorrelateAtomPairDistance(Pids[i]-1,Nids[i]-1,corrav,corrvar,N,pl);
      	      plat = pl / ncor / nframes;
	      printf("plsum %f plat %f ncor %d\n",pl,plat,N[0]);
	      plat *= plat;
	      OutputCorrFunction(outname, nframes, corrav, corrvar,delta, plat);
	    }
	}
      for(int k=0;k<nframes;k++)
	{
	  corrav[k] /= N[k];
	}
      double C0 = corrav[0];
      for(int k=0;k<nframes;k++)
	{
	  corrav[k] /= C0;
	}
    }      
  for(int k=0;k<nframes;k++)
    {
      corrvar[k] = sqrt(corrvar[k]);
    }
  

  OutputCorrFunction(outname, nframes, corrav,corrvar, delta, plat);

  delete [] corrvar;
  delete [] corrav;
  return 1;
} 
int DoPNHist(BMTReader &b, char *Pname,char *Nname, double vx, double vy, double vz, 
	     double binwidth, int nbins, double slab, int nframes , int *hist, int *count, int exclude_intern, int exclude_extern, int load)
{

  int nNats,nPats;
  int *Nids,*Pids;
  double sl = slab * slab;

  if(load)
    {
      assert(ReadIdList(Nname,&nNats,&Nids));
      assert(ReadIdList(Pname,&nPats,&Pids));
      
      printf("Now buffering atom coords (%d)\n",nPats);
      b.BufferAtomList(nframes,nPats,Pids,1);
      printf("Now buffering atom coords (%d)\n",nNats);
      b.BufferAtomList(nframes,nNats,Nids,0);
      
    }
  int cnt = *count;
  for(int frm=0;frm < nframes;frm++)
    {
      printf("\nFrame %d\n",frm);
      for(int p = 0; p < nPats; p++)
	{
	  //	  printf("Rad Dist for element %d \r", p);
	  double px,py,pz;
	  b.GetVector(Pids[p]-1,frm,px,py,pz);
	  for(int n = 0; n < nNats; n++)
	    {
	      int bin;
	      double d,nx,ny,nz;
	      
	      if( n == p && exclude_intern)
		continue;
	      if( n != p && exclude_extern)
		continue;
	      //b.GetVector(Nids[n]-1,frm,nx,ny,nz);
      	      b.GetNearVector(Nids[n]-1,frm,px,py,pz,nx,ny,nz);
	      double dz = nz - pz;
	      dz *= dz;
	      if(slab > 0.0 && sl < dz)
		continue;
	      if(0)// nz * pz > 0.0)  // test for same bilayer (assumes system is centered)
		{		  
		  d = Dist(px,py,pz,nx,ny,nz);
		  bin = (int) (d / binwidth);
		  // printf("d = %f bin = %d\n",d,bin);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image -x
		  d = Dist(px,py,pz,nx - vx,ny,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image +x
		  d = Dist(px,py,pz,nx + vx,ny,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image -y
		  d = Dist(px,py,pz,nx,ny - vy,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image +y
		  d = Dist(px,py,pz,nx,ny + vy,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image -x -y
		  d = Dist(px,py,pz,nx - vx,ny - vy,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image +x +y
		  d = Dist(px,py,pz,nx + vx,ny + vy,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image +x -y
		  d = Dist(px,py,pz,nx + vx,ny - vy,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  // image -x +y
		  d = Dist(px,py,pz,nx - vx,ny + vy,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	
		  
		}	
	      else 
		{
		  d = Dist(px,py,pz,nx,ny,nz);
		  bin = (int) (d / binwidth);
		  if (bin < nbins){  hist[bin]++;     cnt++;    }	

		}
	    }
	}
	
    }
  *count = cnt;
  return 1;

}


int DoZHist(BMTReader &b, char *Pname,double binwidth, int nbins, int nframes , int *hist, int *count)
{

  int nPats;
  int *Pids;

  assert(ReadIdList(Pname,&nPats,&Pids));
  
  printf("Now buffering atom coords (%d) from %s\n",nPats, Pname);
  b.BufferAtomList(nframes,nPats,Pids,1);
  
  int cnt = 0;
  for(int frm=0;frm < nframes;frm++)
    {
      //printf("\nFrame %d\n",frm);
      for(int p = 0; p < nPats; p++)
	{
	  //	  printf("Rad Dist for element %d \r", p);
	  double px,py,pz;
	  b.GetVector(Pids[p]-1,frm,px,py,pz);
	  while(pz > Vz2){pz -=Vz;}
	  while(pz <= -Vz2){pz +=Vz;}


	  int slabnumber = nbins * (pz + Vz2) / Vz ;
	  slabnumber = (slabnumber < 0) ? 0: slabnumber;
	  slabnumber = (slabnumber >= nbins) ? nbins - 1: slabnumber;
	  hist[slabnumber]++;
	  cnt++;
	}
	
    }
  *count = cnt;
  return 1;

}




int DoScd(BMTReader &b, char *Pname,char *Nname,  int nframes, double *Scd ,double *er)
{

  int nNats,nPats;
  int *Nids,*Pids;

  double scd = 0.0,sum = 0.0,var = 0.0,tvar = 0.0;
  int count = 0;

  assert(ReadIdList(Nname,&nNats,&Nids));
  assert(ReadIdList(Pname,&nPats,&Pids));

  printf("Now buffering atom coords (%d)\n",nPats);
  b.BufferAtomList(nframes,nPats,Pids,1);
  printf("Now buffering atom coords (%d)\n",nNats);
  b.BufferAtomList(nframes,nNats,Nids,0);

  assert(nPats == nNats);
  double lastS = 0.0;
  int hist[100];

  for(int i = 0;i < 100; i++)
    hist[i] = 0;

  for(int p = 0; p < nPats; p++)
    {
      count += b.AtomPairScd(Pids[p]-1,Nids[p]-1,scd,var);
      double S = (scd - lastS) / nframes;
      lastS = scd;
      int ind = 50 + 100 * S;
      hist[ind]++;
      if(S > -0.1)
	printf("SCD %d %d = %f\n",Pids[p],Nids[p],S);

      
      //            printf("Scd for element %d %f\n", p,S);
            sum += scd;
            tvar += var;
    }
  FILE *fPF = fopen("scdhist.out","w");
  for(int i=0;i<100;i++)
    {
      double x = (i - 50.0) / 100.0;
      fprintf(fPF,"%f %d\n", x,hist[i]);
    }
  fclose(fPF);
  double N = count;
  scd /= N;
  var = var/ N - scd*scd ;
  var /= N;
  *Scd = scd;
  *er = sqrt(var);
  
  printf("SCD= %f +- %f",*Scd,*er);
  return 1;

}

int OutputScd(char *outname,char *pname, char*nname, double Scd,double er)
{
  FILE *fPF = fopen(outname,"a+");
  fprintf(fPF,"%15s \t%15s \t%f \t%f\n",pname,nname,Scd,er);
  fclose(fPF);
  return 1;
}

int DumpFrameCoords(BMTReader &b,int nframes, int nats, int *ids, double * interp)
{
  int nf = nframes / b.GetSkip();
    int frame = -1;
    double x,y,z,s;
    s = 1.0 / (double) nframes;
    printf("using interpolation %f %f %f %f %f %f\n",interp[0],interp[1],interp[2],interp[3],interp[4],interp[5]);
    while(b.ReadFrame())
      {	
	frame++;
	double fr = frame;
	printf("####  Frame %d\n",frame);
	for(int ii=0;ii<nats;ii++)
	  {
	    int i = ids[ii] - 1;

	    x = b.x[i] - interp[0] - fr * s * (interp[3] - interp[0]);  
	    y = b.y[i] - interp[1] - fr * s * (interp[4] - interp[1]);  
	    z = b.z[i] - interp[2] - fr * s * (interp[5] - interp[2]);  

       	    printf("%d %f %f %f \n",i, x,y,z);
	  }

	if(nf > 0 && frame >= nf - 1)
	  break;
      }
    return 1;
}

int ReadIdList(char *fname,int *n, int **ids)
{
  char buff[256];
  FILE *fPF = fopen(fname,"r");

  if(fPF == NULL)
    {
      *n = 0;
      *ids = NULL;
      return 0;
    }
  IntCount = 0;
  fgets(buff,100,fPF);
  while(!feof(fPF))
    {
      
      if(isdigit(buff[0]))
	{
	  int nb = sscanf(buff,"%d",&IntBuffer[IntCount]);
	  assert(nb);
	  IntCount++;
	}
      fgets(buff,100,fPF);      
    }   
	    
  if(IntCount > 0)
    {
      *n = IntCount;
      int * Ids = new int[IntCount];
      for(int i=0;i<IntCount;i++)
	Ids[i] = IntBuffer[i];
      *ids = Ids;
      return 1;
    }

  *ids = NULL;
  *n = 0;
  return 0;

}


int ReadIdPairList(char *fname,int *n, int **ids, int **ids2)
{
  char buff[256];
  FILE *fPF = fopen(fname,"r");

  if(fPF == NULL)
    {
      *n = 0;
      *ids = NULL;
      *ids2 = NULL;
      return 0;
    }
  IntCount = 0;
  fgets(buff,100,fPF);
  while(!feof(fPF))
    {
      
      if(isdigit(buff[0]))
	{
	  int id1,id2;
	  int nb = sscanf(buff,"%d %d",&id1,&id2);
	  assert(nb);
	  IntBuffer[IntCount] = id1;
	  IntCount++;
	  IntBuffer[IntCount] = id2;
	  IntCount++;
	}
      fgets(buff,100,fPF);      
    }   
	    
  if(IntCount > 0)
    {
      IntCount /= 2;
      *n = IntCount;
      int * Ids = new int[IntCount];
      int * Ids2 = new int[IntCount];
      for(int i=0;i<IntCount;i++)
	{
	  Ids[i] = IntBuffer[2*i];
	  Ids2[i] = IntBuffer[2*i + 1];
	}
      *ids = Ids;
      *ids2 = Ids2;
      return 1;
    }

  *ids = NULL;
  *n = 0;
  return 0;

}


int ExpandIdList(int *ids, int nids,int length, int **new_ids, int *Nnew_ids)
{
  int n = nids * length;
  int c = 0;
  int *lids = new int[n];
  for(int i = 0;i<nids;i++)
    {
      for(int j = 0;j < length;j++)
	{
	  lids[c] = ids[i] + j;
	  c++;
	}
    }
  *new_ids = lids;
  *Nnew_ids = n;
  return 1;
}


void OutputCorrFunction(char *fname, int nframes, double *corrav, double *corrvar, double dt, double plat)
{
  FILE *fPF = fopen(fname,"w");
  for(int i = 0; i < nframes; i++)
    {
      fprintf(fPF,"%f %g %g %g\n",(i * dt), corrav[i],corrvar[i], plat);
    }
  fclose(fPF);
}
void OutputHist(char *fname,int nbins, int * allhis, int * inthis, int * exthis, int NA,int NI, int NX, double norm, double binwidth )
{
  FILE *fPF = fopen(fname,"w");
    int i;
    double normfactor;
    fprintf(fPF,"# totalN \t binwidth \n");
    fprintf(fPF,"# %8d %8d %8d\t %8.4f\n", NA, NI,NX, binwidth );
    
    for(i=0;i<nbins;i++)
      {
	double r1 = i * binwidth; 
	double r2 = (i+1) * binwidth; 
	double r = i * binwidth  + binwidth / 2.0;
	double nid = 3.14159 * (r2*r2 -r1*r1);
	//double nid = 4.0/3.0 * 3.14159  * (r2 * r2 * r2 - r1 * r1 * r1);
	//	normfactor = 1.0 / A / norm / norm;
	//	normfactor =  30.0 *30.0 ; /// vol ; // 1.0 / norm / vol;// / norm / norm;
	double vol = Vx * Vy;// * Vz;
	normfactor = 36 * 36  / vol /vol ;
	double pA = ((double)allhis[i]) / ((double)NA) / nid / normfactor ;	
	double pI = ((double)inthis[i]) / ((double)NI) * normfactor * norm;
	double pX = ((double)exthis[i]) / ((double)NX) * normfactor;
	fprintf(fPF,"%10f %15.7g %15.7g %15.7g\n",r,pA,pI,pX);
      }
    fprintf(fPF,"\n");
    
    fclose(fPF);
}

void OutputZHist(char *fname,double binwidth, int nbins, int nframes, int * allhis, int N,int nelect)
{
  FILE *fPF = fopen(fname,"w");
    int i;

    double angsperslab = Vx*Vy*binwidth;
    double normfactor;
    double na = N/nframes;
    printf(" natoms = %f\n",na);
    double nea = nelect / na;
    double angsperbox = Vx*Vy*binwidth;
    double sum = 0.0;
    for(i=0;i<nbins;i++)
      {
	double z = i * binwidth - Vz2 + 0.5 * binwidth; 

	//            atoms per slab / atoms per box 
	double pz = ((double)allhis[i]) / N;
	sum += pz;
	// pz(/box) * (electrons/atom) * (box/ang3)
	double D = pz * na  *  nea / angsperbox;
	fprintf(fPF,"%10f %15.7g\n",z,D);
      }
    fprintf(fPF,"\n");
    printf("Sum pf p = %f\n",sum);
    fclose(fPF);
}


double Dist(double x1,double y1,double z1,double x2,double y2,double z2)
{
  double dx = x1 - x2;
  double dy = y1 - y2;
  double dz = z1 - z2;
  double d = sqrt(dx*dx + dy*dy + dz*dz); 
  return d;
}
double SQDist(double x1,double y1,double z1,double x2,double y2,double z2)
{
  double dx = x1 - x2;
  double dy = y1 - y2;
  double dz = z1 - z2;
  double d = dx*dx + dy*dy + dz*dz; 
  return d;
}
double SQDist2D(double x1,double y1,double x2,double y2)
{
  double dx = x1 - x2;
  double dy = y1 - y2;
  double d = dx*dx + dy*dy; 
  return d;
}
double SQDist1D(double z1,double z2)
{
  double dz = z1 - z2;
  double d = dz*dz; 
  return d;
}

int ExtractAtomGroupListTimeSeries(BMTReader &b,char *fname,int nframes,int length,int dim,char *outname)
{
  int nats,xnids;
  int *ids,*xids;

  
  assert(ReadIdList(fname,&nats,&ids));
  ExpandIdList(ids,nats,length,&xids,&xnids);

  printf("skipping %d actual %d frames\n",b.GetSkip(),nframes);

  b.BufferAtomList(nframes,xnids, xids);
  FILE *fPF = fopen(outname,"w");
  for(int t = 0; t < nframes; t++)
    {
      double x0,y0,z0,m0,m;
      for(int i = 0; i < nats; i++)
	{
	  x0 = y0 = z0 = m0 = 0.0;
	  double xx,yy,zz;	  
	  for(int j=0;j < length; j++)
	    {
	      b.GetVector(ids[i]-1,t,xx,yy,zz);
	      m =  b.Mass(ids[i]-1);
	      x0 += m*xx;   y0 += m*yy;   z0 += m*zz; m0 += m;
	    }
	  x0 /= m0; y0 /= m0; z0 /= m0;
	  switch(dim)
	    {
	    case 1:
	      fprintf(fPF,"%12.6f\t",z0);
	      break;
	    case 2:
	      fprintf(fPF,"%12.6f %12.6f\t",x0,y0);
	      break;
	    case 3:
	      fprintf(fPF,"%12.6f %12.6f %12.6f\t",x0,y0,z0);
	      break;
	    default:
	      assert(0);
	    }
	}
      fprintf(fPF,"\n");
    }
  fclose(fPF);
  return 1;
}




