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
 #ifndef _INCLUDED_ORBNODE_
#define _INCLUDED_ORBNODE_


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



#define CUTXL 8
#define CUTXH 9
#define CUTY1L 10
#define CUTY1H 11
#define CUTY2L 12
#define CUTY2H 13
#define CUTZ1L 0
#define CUTZ1H 1
#define CUTZ2L 2
#define CUTZ2H 3
#define CUTZ3L 4
#define CUTZ3H 5
#define CUTZ4L 6
#define CUTZ4H 7
#define DIMS 3
#define NCHILDREN 14

static int CompareInt(const void *elem1, const void *elem2);
static int CompareX(const void *elem1, const void *elem2);
static int CompareY(const void *elem1, const void *elem2);
static int CompareZ(const void *elem1, const void *elem2);

typedef  unsigned int VoxelIndex ;
typedef unsigned short FragId ; 

class VoxelNodePair
{
public:
  int voxelId;
  VoxelIndex voxIndex;
  int rank;
  VoxelNodePair()
    { voxelId = 0; voxIndex = 0,rank = 0; }
};

#ifdef _ORB_STANDALONE_
class XYZ
{
public:
  double mX,mY,mZ;
  XYZ(){}
};


#include <iostream.h>
#define BegLogLine(x) if(x)cout
#define EndLogLine endl
#endif

#ifndef _ORB_STANDALONE_
inline
void
NearestVectorInPeriodicVolume(const XYZ &PositionA, const XYZ &PositionB, XYZ &Nearest);

inline
void
NearestImageInPeriodicVolume(const XYZ &PositionA, const XYZ &PositionB, XYZ &Nearest);
#endif



class Site
{
public:
#ifdef _ORB_STANDALONE_
   int id;
#endif
  double verlet_count;
  double c[DIMS];
//   FragId fA;
//   FragId fB;

  Site() { verlet_count =0; c[0]= c[1]=c[2] = 0.0; }
  Site & operator=(Site &s)
    {
      verlet_count = s.verlet_count;
#ifdef _ORB_STANDALONE_
       id = s.id;
#endif
      for(int i=0;i<DIMS;i++){c[i] = s.c[i];}
      return *this;
    }

  void GetXYZ( XYZ& aXYZ )
    {
      aXYZ.mX = c[ 0 ];
      aXYZ.mY = c[ 1 ];
      aXYZ.mZ = c[ 2 ];
    }

  double SqrDist(double *crd,double *dV)
    {
      double p[DIMS],d,r = 0.0;
      for(int i=0;i<3;i++)
        {
          p[i] = c[i];
          while(p[i] < (crd[i] - dV[i]/2.0)) p[i] += dV[i];
          while(p[i] > (crd[i] + dV[i]/2.0)) p[i] -= dV[i];
          d = crd[i] - p[i];
          r += d*d;
        }
      return r;
    }
  void Print()
    {
      printf("[ %f %f %f %f ]\n",verlet_count, c[0],c[1],c[2]);
    }
};


class ORBNode
{
private:
  double low[DIMS],high[DIMS];
  double reciprocal[DIMS] ;

  int mTag;
  int meshnum[DIMS],
      voxnum[DIMS],
      procnum[DIMS],
      extfromintern[DIMS],
      internfromext[DIMS];

  int start;
  int realnode;
  int dims;

public:
  int id;
  int rank;
  double verlet_sum;
  int nLoadVox;
  int *LoadZone;
  Site * SiteArray;
  int nsites;

  inline void SetLen(int n);
  
  ORBNode();
  ~ORBNode() __attribute__((noinline)) ;

private:
  int NSites;
  int NAllocatedSites;
  double *VerletSum;

  ORBNode *child[NCHILDREN];
  ORBNode *parent;
  ORBNode *root;
  int MaxLoadVox;

  inline int FreeLoadZones();  // Don't call
  inline int AllocateLoadZones();  // Called from DeriveLoadZones
  inline int FreeMyLoadZones(int NX, int NY, int NZ);  // Don't call
  inline int AllocateMyLoadZones(int NX, int NY, int NZ);  // Called from DeriveLoadZones
  inline void WhipperUp();


  inline void Adopt(ORBNode *child);


  inline void SetStart(int n);
  inline int StartSite();


  inline void VoxelNodeDeltas(int voxelId,ORBNode *nd, double *v);
    
    // void ReduceIntArray(int *ranks, int &len);

// void SetRange(int dim, double l, double h);
  inline void SortRanks(int * ranks,int len);
  inline void Sort(int dm);
  inline void SortX();
  inline void SortY();
  inline void SortZ();
  inline void ReduceIntArray(int *ranks, int &len);

  inline void SetRange(int dim, double l, double h);

  inline void CopySites(ORBNode *src, int start, int len);

  inline int NXProcs();
  inline int NYProcs();
  inline int NZProcs();
  inline int DetermineDims(int levels);

  inline void xyzFromProcCount(int procs, int &x, int &y, int &z);

  inline void VerifyContents();

  inline int ValidateTree( double aRadius );

   int SetVoxelNums(int dx,int dy, int dz) __attribute__((noinline));

  inline double GetTotalWeight();  

  //
  // return the node given the voxel coordinates
  //

         ORBNode * Node(int x,int y, int z) ;
         ORBNode * Node(double x, double y, double z);
  //
  // number leaves from this point down,starting with n. Returns next id
  //
  inline int NumberLeaves(int n);

  inline void NodeMinDeltas(double *alow, double *ahigh,double *vec);
  inline void NodeMinDeltas(ORBNode *mynode,double *deltas);
  inline void NodeMaxDeltas(ORBNode *mynode,double *deltas);
  inline void NodeBoundaryDistances(ORBNode *nd, double &min, double &max);
         int AddRanks(ORBNode * mynode, double r, int * ranks, int &len, int max);
         int AddRanks(ORBNode * mynode, double *box,int * ranks, int &len, int max);
         int AddRanks(double *alow, double *ahigh, double *deltas,int * ranks, int &len, int max);
  inline int AddSkinRanks(ORBNode * mynode, double r, int * ranks, int &len, int max);

  inline void SwapNodes(ORBNode **n1,ORBNode **n2);

  inline void ImagePoint(double *v, double *imagedv);
  inline int  IsImagedPointMine(double *vec);


  inline int VoxelsInBox(double *lbound, double *hbound , int *voxels, int &len, int max);
  inline int VoxelIdFromInternal(double *vec); // assume imaged x,y,z
  inline int VoxelId(int *vec);  // handles imaging internal voxid
  inline int VoxelBounds(int id, double *low, double *high);
  inline void VoxelCoords(int Id , int *n);
  inline double SnapToGrid(double cut, int dim);
  inline double SnapLow(double cut, int dim);
  inline double SnapHigh(double cut, int dim);
  inline void VoxelRange(double *vlow, double *vhigh, int *vnlow, int *vnhigh);
  inline void InitSet(int *set, int max);
  inline void CollapseIntSet(int *set, int &len, int max);
  inline void OrVoxelNeighborSet( double r, int* voxels, int &len, int max);
  inline int TakeCutUnsorted(ORBNode *src, ORBNode *destL, ORBNode *destH, int dim);
  inline int TakeCut(ORBNode *src, ORBNode *destL, ORBNode *destH, int dim);
  inline int TakeEvenCut(ORBNode *src, ORBNode *destL, ORBNode *destH, int dim);
  inline int FindCutSiteUnsorted(double& CutValue, int dim);
  inline int FindCutSite();
  inline int FindMedian(double *ver, int n, int cur,int window);
  inline void ArrangeToPartition(int n, double cutValue, int dim ) ;

  inline double RealSqrDist(double* coord);
  inline double VerletCount(double *coord, double r);


////////////////////////////////////////
/////////////////////    Public Methods
///////////////
///////////////  Use only external coords

public:
   int VoxelId(double *vec); // assume imaged x,y,z
   VoxelIndex VoxelIdToVoxelIndex( int aVoxelId );
   int VoxelIndexToVoxelId( VoxelIndex aVoxIndex );
   VoxelIndex GetVoxelIndex(double *vec); // assume imaged x,y,z
   VoxelIndex GetVoxelIndex( XYZ& aPos ); 
   int VoxelId( XYZ& aPos ); 
   int VoxelExternalId( int *procnums ); 
   int VoxelExternalBounds(int id, double *low, double *high);
         int VoxelsInExternalBox(double *lbound, double *hbound , int *voxels, int &len, int max);
   int CouldVoxelMidpointBeMine(int voxId1, int voxId2);
   int VoxelsInNeighborhood(double r, int *voxels, int &len, int max);
   int IsVoxelOnBoundary(int voxId, double r);
         double MaxVoxelSqrDistance(int voxelId1, int voxelId2);
   int IsMidpointOfVoxelMidpointsMine(int voxid1, int voxid2);
   int IsPointInRandomSphereMine(int voxid1, int voxid2, double aCutoffGuardZone );
   void GetMidpointOfVoxelMidpoints( int voxid1, int voxid2, double* mid );
   double MinVoxelSqrDistance(int voxelId1, int voxelId2);

   void VoxelPoint(int VoxelId, double *p, int cacheKnown=0);
   void CalcVoxelPoint(int VoxelId, double *p);
   int IsVoxelPointOfVoxelPointsMidpointMine(int voxelId1, int voxelId2);
   int GetMidpointOfVoxelPoints( int voxId1, int voxId2, double* mid, int cacheKnown=0 );
   int RanksFromVoxels(int * voxels,int nvox, int *ranks,int &len, int max);
  //
   int IsVoxelPointOfVoxelMidpointsMine(int voxId1, int voxId2);
   int IsMidpointOfVoxelPointsMine(int voxId1, int voxId2, int cacheKnown=0);
  void       IsMidpointOfVoxelPointsMineVector(char* resultVector, int voxId1, int * voxId2Vector, int count);
   int GetVoxelPointOfVoxelMidpoints( int voxId1, int voxId2, double* vox_pt );
   int CouldMidpointBeMine(ORBNode * node1, ORBNode *node2);
   int IsNodeOnBoundary(ORBNode * nd, double r);

         int RanksInLoadZoneNeighborhood(double r,int *ranks,int &len,int max  );
         int RanksInLoadZoneNeighborhoodVersion2(double r,int *ranks,int &len,int max  );

   int DeriveMyLoadZone();
   int DeriveAllLoadZones();
   int DeriveLoadZones( int i, int j, int k );  // Call on root node only
   int InLoadZone(int voxid);


   void NodeMinExternalDeltas(ORBNode *nd,double *vec);

   double NodeSqrDistance(ORBNode *nd);

   void PrintCoordSum();
    int CoordSum();
   void ReportVoxel( int id );
   void ReportLeafData();
   void ReportLocalFrags();

   void ConfigNode(int id, int rank,double * low, double *high, ORBNode *root);



   void UpdateVerletCount(double r);
   void UpdateVerletCount(double *verletcountlist);
   void UpdateNodeVerletCount();
   int NumSites(){return NSites;}


  void ClosestMidpoint(XYZ a, XYZ b, XYZ aCenter, XYZ &c)
    {
      XYZ d;
#ifndef _ORB_STANDALONE_
      NearestImageInPeriodicVolume(a,b,d);
      d.mX = (d.mX - a.mX) / 2.0 + a.mX;
      d.mY = (d.mY - a.mY) / 2.0 + a.mY;
      d.mZ = (d.mZ - a.mZ) / 2.0 + a.mZ;
      NearestImageInPeriodicVolume(aCenter,d,c);
#endif
    }

  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  /////////////////////////////////////////////////

public:


   void SetOrbDims(double sX, double sY, double sZ, int vX, int vY, int vZ, int pX, int pY, int pZ);
   int Configure(int size);
   void SetSite( int i, XYZ aSite, double weight =1.0);
   void SetSite( int i, XYZ aSite, double weight, FragId f1, FragId f2);
  void SetSiteList( int i, double* weightVector, int voxelIdA, int* voxelIdVector, int count) ;

   Site * GetSite(int i);

   void Low(double *alow);
   void High(double *ahigh);
   void ExternalFromInternal(double *intrn,double *extrn);
   void InternalFromExternal(double *extrn,double *intrn);
   void ExternalProcMeshNum(int * n);

  ORBNode * NodeFromProcCoord(int x,int y, int z);
  ORBNode * NodeFromSimCoord(double x, double y, double z);

  //  Methods for returning neighboing node addresses (ranks)

   int RanksDeltaFromBox(double *alow, double *ahigh, double dx, double dy, double dz, int* ranks, int &len,int max);
   int RanksInBox(int px, int py, int pz, double dx, double dy, double dz, int* ranks, int &len,int max);
   int RanksInNeighborhood(int px, int py, int pz, double r, int* ranks, int &len,int max);
   int RanksInHomeZoneSubsetNeighborhood(int vixIdLow, int voxIdHigh, double r, int* ranks, int &len,int max);
   int RanksInNeighborhoodVersion2(int px, int py, int pz, double r, int* ranks, int &len, int max);
   int RanksInLoadBox(double *vlow, double *vhigh,int *voxels, int &vlen, int vmax,int* ranks, int &len, int max);
   int RanksInNeighborhoodSkin(int px, int py, int pz, double r, int* ranks, int &len,int max);


   void WriteOrb(char *streamPS, int& cursor);

  //  Recursive call to process prepare subdivisions
  //
//  inline int SubdivideSorted(int levels) __attribute__((noinline));
   int Subdivide(int levels) __attribute__((noinline));
   int SubdivideEvenly(int levels) __attribute__((noinline));

};

// static void CookTree(int aNpoints, XYZ& aBoxDim, int aProcNumber, ORBNode* tree,int nX, int nY, int nZ)
// {

//   // Find full space power of 2

//   int d = 3;
//   int levels = 0;
//   int t;
//   do
//     {
//       levels++;
//       t = 1 << (levels*d);
//     }while (aProcNumber > t);
//   int base = (levels-1)*d;
//   while(d > 0)
//     {
//       t = 1 << (base + d);
//       if(aProcNumber == t) break;
//       d--;
//     }
//   assert(d>0);


//   int N=levels;
//   tree->id = -levels;


//   qsort( tree->SiteArray, aNpoints, sizeof( Site ), CompareX );


//   tree->low[ 0 ] = 0.0;
//   tree->low[ 1 ] = 0.0;
//   tree->low[ 2 ] = 0.0;

//   tree->high[ 0 ] = aBoxDim.mX;
//   tree->high[ 1 ] = aBoxDim.mY;
//   tree->high[ 2 ] = aBoxDim.mZ;

//   tree->SetProcDims(nX/2 , nY/2 , nZ/2);


//   tree->Subdivide( levels, d , nX,nY,nZ );


//   tree->NumberLeaves( 0 );
//   tree->SetVoxelNums( 0,0,0 );

//   tree->DeriveAllLoadZones();

  

//   // tree->ReportLeafData();
// }


//#ifndef _ORB_STANDALONE_
//#include <BlueMatter/orbnode.cpp>
//#endif

#endif
