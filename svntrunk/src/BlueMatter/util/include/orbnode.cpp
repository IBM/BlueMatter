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
 #include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/XYZ.hpp>

#include <BlueMatter/math.hpp>
#include <BlueMatter/orbnode.hpp>

//#ifdef _ORB_STANDALONE_
//#include "../include/orbnode.hpp"
//#endif


#if !defined(PKFXLOG_ORBNODE)
#define PKFXLOG_ORBNODE (0) 
#endif

#if !defined(PKFXLOG_ORBSETUP)
#define PKFXLOG_ORBSETUP (0)
#endif

#if !defined(ORB_COMPILE)
#define ORB_COMPILE (1)
#endif

#define SNAP_TO_GRID 0
#define VOX_COORD_SHIFT 8

static inline double NearestInteger(const double x);
  
#include <stdio.h>

#ifndef PKFXLOG_REPORT_LOCAL_ORB_FRAGS
#define PKFXLOG_REPORT_LOCAL_ORB_FRAGS ( 1 )
#endif

#ifndef PKFXLOG_UNSORTED_ORB
#define PKFXLOG_UNSORTED_ORB (0) 
#endif

#define DBG 0
#define DBGV 0


#define ROOT_ID 0xFACEBEEF
#define NODE_ID 0xFADEBEEF
#define UNDEFINED_ID 0xDEADBEAF

//#define CHECK_ORB_ROOT 

#ifdef CHECK_ORB_ROOT 
#define VALIDATE_ROOT { if( root->mTag != ROOT_ID){PLATFORM_ABORT("Bogus Orb Root");}}
#else
#define VALIDATE_ROOT 
#endif



#ifdef _ORB_STANDALONE_ 
#define NODE_ZERO 1
#else
#define NODE_ZERO (Platform::Topology::GetAddressSpaceId() == 0) 
#endif


static int CompareInt(const void *elem1, const void *elem2)
{
  int* A = (int *)elem1;
  int* B = (int *)elem2;
  if (*A == *B)return 0;
  else if (*A < *B) return -1;
  else return 1;
}
static int CompareX(const void *elem1, const void *elem2)
{
  Site* A = (Site *)elem1;
  Site* B = (Site *)elem2;
  if (A->c[0] == B->c[0])return 0;
  else if (A->c[0] < B->c[0]) return -1;
  else return 1;
}
static int CompareY(const void *elem1, const void *elem2)
{
  Site* A = (Site *)elem1;
  Site* B = (Site *)elem2;
  if (A->c[1] == B->c[1])return 0;
  else if (A->c[1] < B->c[1]) return -1;
  else return 1;
}
static int CompareZ(const void *elem1, const void *elem2)
{
  Site* A = (Site *)elem1;
  Site* B = (Site *)elem2;
  if (A->c[2] == B->c[2])return 0;
  else if (A->c[2] < B->c[2]) return -1;
  else return 1;
}


ORBNode::ORBNode()
{
  id = 0;
  mTag = NODE_ID;
  realnode = 0;
  rank = -1;
  double verlet_sum = 0.0;
  for(int i=0;i<DIMS;i++)
    {
      low[i]=high[i]=0.0;
      reciprocal[i]=-1.0 ; // Nominally 'Inf', but this will flag a problem if
                        // we ever use it by accident
      procnum[i] = meshnum[i] = voxnum[i] = 0;
      extfromintern[i] = internfromext[i] = i;
    }
  start = 0;
  int n = NCHILDREN;
  for(int i=0;i<n;i++)
    {
      child[i]=NULL;
    }
  parent = NULL;      
  root = this;
  SiteArray = NULL;
  VerletSum = NULL;
  NSites = NAllocatedSites = 0;
  nsites = 0;
  LoadZone = NULL;
  nLoadVox = 0;
  MaxLoadVox = 0;
}

void ORBNode::ConfigNode(int aid, int arank,double * alow, double *ahigh, ORBNode *aroot)
{
  id = aid;
  rank = arank;
  root = aroot;
  VALIDATE_ROOT 	
  realnode = 1;
  for(int i = 0; i < DIMS; i++)
    {
      low[i] = alow[i];
      high[i] = ahigh[i];
    }
}

#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)
ORBNode::~ORBNode()
{
  if(root == this)
    {
      WhipperUp();
      FreeLoadZones();
    }
  SiteArray = NULL;
  VerletSum = NULL;
#if 1
  ORBNode* child_nodes[NCHILDREN] ;
  for (int h=0;h<NCHILDREN;h+=1)
  {
  	 child_nodes[h] = child[h] ;
 }
  for(int i = 0;i<NCHILDREN;i++)
    {
     ORBNode * t = child_nodes[i];
     child[i] = NULL ;
     if(t != NULL)
     { 
     	t->~ORBNode() ; 
     }
     
    }
#else  
  for(int i = 0;i<NCHILDREN;i++)
    {
     ORBNode * t = child[i];
     if(t != NULL)
       {t->~ORBNode(); child[i] = NULL;}
     
    }
#endif    
  root = NULL;
}
#endif

void ORBNode::SetOrbDims(double simX, double simY, double simZ, int voxX, int voxY, int voxZ, int procX, int procY, int procZ)
{

  //
  //  Set up orb mapping with convention that sim cell has been mapped to the machine
  //  i.e.: derive map so internal cutting proceeds in descending machine dimension order
  //        and external refs to sim space are mapped internally
  //

  assert(root == this);

  int l,m,s;
  int n = procX*procY*procZ;

  int nprocs[DIMS];
  nprocs[0] = procX;
  nprocs[1] = procY;
  nprocs[2] = procZ;

  int swap;
  if(DBG)
    {
       if(NODE_ZERO) printf(" pX %d pY %d pZ %d\n",nprocs[0],nprocs[1],nprocs[2]);
    }
  do
    {
      swap = 0;
      if ( nprocs[ extfromintern[ 0 ] ] < nprocs[ extfromintern[ 1 ] ])
	{
	  swap = extfromintern[ 1 ];
	  extfromintern[ 1 ] = extfromintern[ 0 ];
	  extfromintern[ 0 ] = swap;
	  swap = 1;	  
	}
      if ( nprocs[ extfromintern[ 1 ] ] < nprocs[ extfromintern[ 2 ] ])
	{
	  swap = extfromintern[ 2 ];
	  extfromintern[ 2 ] = extfromintern[ 1 ];
	  extfromintern[ 1 ] = swap;
	  swap = 1;	  
	}
    }  while(swap);

  for( int i = 0; i<DIMS ; i++)
    {
      internfromext[ extfromintern[ i ] ] = i;
    }
  
  for( int i = 0; i<DIMS ; i++)
    {
      procnum[i] = nprocs[ extfromintern[ i ] ];
    }



  //
  //Example  :  called with 8 8 16
  // internfromext: [ 1 2 0 ]
  // extfromintern: [ 2 0 1 ]
  //

  high[ internfromext[ 0 ] ] = simX;
  high[ internfromext[ 1 ] ] = simY;
  high[ internfromext[ 2 ] ] = simZ;

  voxnum[ internfromext[ 0 ] ] = voxX;
  voxnum[ internfromext[ 1 ] ] = voxY;
  voxnum[ internfromext[ 2 ] ] = voxZ;

  if(DBG)
    {
      if(NODE_ZERO) printf("Internal pX %d pY %d pZ %d\n",procnum[0],procnum[1],procnum[2]);
      if(NODE_ZERO) printf("Internal sX %f sY %f sZ %f\n",high[0],high[1],high[2]);
      if(NODE_ZERO) printf("External sX %f sY %f sZ %f\n",high[internfromext[0]],high[internfromext[1]],high[internfromext[2]]);
    }


  for(int i = 0; i < DIMS; i++)
    { 
      low[i] = 0.0; 
      reciprocal[i] = 1.0 / high[ i ];
    }


}


void ORBNode::Adopt(ORBNode *child)
{
  child->root = root;
  VALIDATE_ROOT 
  child->parent = this;
  child->SiteArray = SiteArray;
  child->VerletSum = VerletSum;
  child->NSites = NSites;
  child->NAllocatedSites = NAllocatedSites;  
}

void ORBNode::SetSite( int i, XYZ aSite, double weight )
{
  /// NOTE: sites are left in external space
  Site * t = GetSite(i);
//   t->id = i;
  t->verlet_count = weight;
  t->c[0] = aSite.mX;
  t->c[1] = aSite.mY;
  t->c[2] = aSite.mZ;
}

void ORBNode::SetSite( int i, XYZ aSite, double weight, FragId f1, FragId f2 )
  {
  /// NOTE: sites are left in external space
  Site * t = GetSite(i);
//   t->id = i;
  t->verlet_count = weight;
  t->c[0] = aSite.mX;
  t->c[1] = aSite.mY;
  t->c[2] = aSite.mZ;
  
//   t->fA = f1;
//   t->fB = f2;
  }
#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)
  void ORBNode::SetSiteList( int i, double* weightVector, int VoxelIdA, int* VoxelIdVector, int count) 
  {
  	BegLogLine(PKFXLOG_ORBSETUP)
  	  << "ORBNode::SetSiteList i=" << i
  	  << " count=" << count
  	  << EndLogLine ;
  	for(int k=0;k<count;k+=1)
  	{
        int VoxelIdB = VoxelIdVector[k] ;
        double vox_pt[3];
        int VoxelId = GetMidpointOfVoxelPoints( VoxelIdA, VoxelIdB, vox_pt );
        XYZ midXYZ;
        midXYZ.mX = vox_pt[ 0 ] ;
        midXYZ.mY = vox_pt[ 1 ] ;
        midXYZ.mZ = vox_pt[ 2 ] ;
        SetSite( i+k, midXYZ, weightVector[k], -1, -1 );
  	}
  }

#endif

void ORBNode::Low(double *alow)
{
  ExternalFromInternal(low,alow);
}
void ORBNode::High(double *ahigh)
{
  ExternalFromInternal(high,ahigh);
}

void ORBNode::ExternalProcMeshNum(int * n)
{
  VALIDATE_ROOT 
  for (int i = 0; i < DIMS; i ++)
    n[ root->extfromintern[ i ] ] = meshnum[ i ];
  return;
}

int ORBNode::CoordSum()
{
  Site x;
  for(int i = 0;i<3;i++)
    x.c[i] = 0.0;
  for(int i = 0;i<NSites;i++)
    {
      Site *s = &SiteArray[i];
      for(int j = 0;j<3;j++)
        { x.c[j] += s->c[j]; }
        }
  double y=0.0;
  for(int i = 0;i<3;i++)
    y += x.c[i];
  float z = y;
  // printf("check sum = %f \n",y);
  int a = z;
  return a;
}


Site * ORBNode::GetSite(int i)
{
  int ind = start + i;  
  Site *t = NULL;
  if( ind>=0 && ind < NSites)
    {
      t = &SiteArray[ind];
    }
  else
    {
      if(NODE_ZERO) printf("Indexed site %d start %d offset %d : range is 0 to %d\n",ind,start,i,NSites);
    }
  return t;
}

int ORBNode::StartSite()
{
  return start;
}

void ORBNode::SetLen(int n)
{
  if(DBGV)
    {if(NODE_ZERO) printf("SetLen %d NSites %d\n",n,NSites);}
  if(n > NSites)
    Configure(n);
  nsites = n;
}

void ORBNode::SetStart(int n)
{
  start = n;
}


int ORBNode::Configure(int size)
{
  assert(this == root);
  root->mTag = ROOT_ID;
  if(size < NAllocatedSites)
    {
      NSites = size;
      return 1;
    }
  else if(NSites > 0)
    {

      delete [] SiteArray; 
      delete [] VerletSum; 
      
    }
  NAllocatedSites = size ;
  SiteArray = new Site[NAllocatedSites];
  assert(SiteArray != NULL);
  VerletSum = new double[NAllocatedSites];
  assert(VerletSum != NULL);
  
  NSites = size;
  if(DBG)
    {if(NODE_ZERO) printf("Configured orb to size %d\n",NSites); }
  return 1;
}

void ORBNode::SortRanks(int *ranks,int len)
    {
      Platform::Algorithm::Qsort(ranks,len,sizeof(int),CompareInt);
    }
void ORBNode::Sort(int dm)
    {
      switch (dm)
	{
	case 0:
	  SortX();
	  break;
	case 1:
	  SortY();
	  break;
	case 2:
	  SortZ();
	  break;
	default:
	  char msg[64];
	  sprintf(msg,"Bogus dimension : %d\n" , dm);
	  PLATFORM_ABORT(msg);
	}
    }
void ORBNode::SortX()
    {
      Platform::Algorithm::Qsort(&SiteArray[start],nsites,sizeof(Site),CompareX);
    }
void ORBNode::SortY()
    {
      Platform::Algorithm::Qsort(&SiteArray[start],nsites,sizeof(Site),CompareY);
    }
void ORBNode::SortZ()
    {
      Platform::Algorithm::Qsort(&SiteArray[start],nsites,sizeof(Site),CompareZ);
    }
  

void ORBNode::VoxelNodeDeltas(int voxelId,ORBNode *nd, double *v)
{
  double vlow[DIMS],vhigh[DIMS],iv[DIMS];
  VoxelBounds(voxelId, vlow,vhigh);
  nd->NodeMinDeltas(vlow,vhigh,iv);
  ExternalFromInternal(iv,v);
  return;
}


void  ORBNode::SetRange(int dim, double l, double h)
{
  low[dim] = l; high[dim] = h;
  reciprocal[dim] = 1.0/(h-l) ;
}
void  ORBNode::CopySites(ORBNode *src, int start, int len)
{
  //  for(int i = 0;i< len; i++)
  //    {
  //      *GetSite(i) = *src->GetSite(start + i);
  //    }
}      
int  ORBNode::NXProcs() 
{
  //  return 1 << (-id);
  VALIDATE_ROOT 
  return root->procnum[0];
}
int  ORBNode::NYProcs() 
{
//    int nx = 1 << (-id); 
//   int n = (dims > 1) ? nx : nx >> 1; 
//   return n;
  VALIDATE_ROOT 
  return root->procnum[1];
}
int  ORBNode::NZProcs() 
{
  
//   int nx = 1 << (-id); 
//   int n = (dims > 2) ? nx : nx >> 1; 
//   return n;
  VALIDATE_ROOT 
  return root->procnum[2];

}

//inline 
VoxelIndex ORBNode::VoxelIdToVoxelIndex( int aVoxelId )
{
//   11 1111 1111
//   3  F    F 
  VALIDATE_ROOT 
  int mask = (1<<VOX_COORD_SHIFT) -1;
  unsigned int LastNBits = mask;
  int vzCoord = aVoxelId & LastNBits;
  int vyCoord = ( aVoxelId >> VOX_COORD_SHIFT ) & LastNBits;
  int vxCoord = ( aVoxelId >> (2*VOX_COORD_SHIFT) ) & LastNBits;

//   if(!( vxCoord >= 0 && vxCoord < root->voxnum[ 0 ] ) )
//     BegLogLine( 1 )
//       << " aVoxelId: " << aVoxelId
//       << " vxCoord: " << vxCoord
//       << " vyCoord: " << vyCoord
//       << " vzCoord: " << vzCoord
//       << EndLogLine;

  assert( vxCoord >= 0 && vxCoord < root->voxnum[ 0 ] );
    
//   if(!( vyCoord >= 0 && vyCoord < root->voxnum[ 1 ] ) )
//     BegLogLine( 1 )
//       << " aVoxelId: " << aVoxelId
//       << " vxCoord: " << vxCoord
//       << " vyCoord: " << vyCoord
//       << " vzCoord: " << vzCoord
//       << EndLogLine;

  assert( vyCoord >= 0 && vyCoord < root->voxnum[ 1 ] );

//   if(!( vzCoord >= 0 && vzCoord < root->voxnum[ 2 ] ) )
//     BegLogLine( 1 )
//       << " aVoxelId: " << aVoxelId
//       << " vxCoord: " << vxCoord
//       << " vyCoord: " << vyCoord
//       << " vzCoord: " << vzCoord
//       << EndLogLine;

  assert( vzCoord >= 0 && vzCoord < root->voxnum[ 2 ] );
  
  int index = ( root->voxnum[ 1 ] * vxCoord + vyCoord ) * root->voxnum[ 2 ] + vzCoord;

  return index;
}

int ORBNode::VoxelIndexToVoxelId( VoxelIndex aVoxIndex )
{
    int coords[3];
    
    int xCoord = aVoxIndex / ( root->voxnum[ 2 ] * root->voxnum[ 1 ] );
    int yCoord = (aVoxIndex - ( xCoord * root->voxnum[ 1 ] * root->voxnum[ 2 ] )) / root->voxnum[ 2 ];
    int zCoord =  aVoxIndex - ( xCoord * root->voxnum[ 1 ] * root->voxnum[ 2 ] ) - (yCoord* root->voxnum[ 2 ]);
    
    coords[ 0 ] = xCoord;
    coords[ 1 ] = yCoord;
    coords[ 2 ] = zCoord;
    
    return VoxelId( coords );
}

int ORBNode::VoxelId(double *v)
{
  double crd[DIMS];
  InternalFromExternal(v,crd);
  return VoxelIdFromInternal(crd);
}
int ORBNode::VoxelIdFromInternal(double *v)
{
  VALIDATE_ROOT 
  double h,l;
  int Id = 0;
  for(int i = 0;i<DIMS;i++)
    {
      Id <<= VOX_COORD_SHIFT;
      h = root->high[i];
      l = root->low[i];
      // Scale to unit cell
      double r = root->reciprocal[i] ;
      BegLogLine(PKFXLOG_ORBNODE) 
        << "ORBNode::VoxelId " << h
        << " " << l
        << " " << r
        << EndLogLine ;
      //    assert(is_almost_zero(r*(h-l)-1.0)) ;  
//#if defined(ORBDEFRAC)     
      double dv = (v[i]-l) * r ; 
      
      // Lose the integer part (keep the fractional part) 
      int idv = dv + 1024.0*1024.0 ; 
      double ridv = idv - 1024*1024 ;
      dv = dv - ridv ;        
//#else
//      double dv = (v[i] - l) / (h - l);
//
//      while (dv < 0.0) dv += 1.0;
//      while (dv >= 1.0) dv -= 1.0;
//#endif      
      int N =  dv * root->voxnum[i];
      assert(N < 1<<VOX_COORD_SHIFT);
      Id |= N;
    }
  return Id;
}

int ORBNode::VoxelId( XYZ& aPos )
  {
  double Pos[ 3 ];
  Pos[ 0 ] = aPos.mX;
  Pos[ 1 ] = aPos.mY;
  Pos[ 2 ] = aPos.mZ;

  int voxelId = VoxelId( (double *) Pos );
  return voxelId;
  }


VoxelIndex ORBNode::GetVoxelIndex( XYZ& aPos )
  {
  double Pos[ 3 ];
  Pos[ 0 ] = aPos.mX;
  Pos[ 1 ] = aPos.mY;
  Pos[ 2 ] = aPos.mZ;
  
  VoxelIndex voxelIndex = GetVoxelIndex( Pos );
  return voxelIndex;
  }

VoxelIndex ORBNode::GetVoxelIndex(double *v)
{
  int Id = VoxelId( v );
  VoxelIndex index = VoxelIdToVoxelIndex( Id );

  return index;
}

int ORBNode::VoxelId(int *v)
{
   VALIDATE_ROOT 
  // NOTE : voxel coords are internal!!!!
  double h,l;
  int Id = 0;
  for(int i = 0;i<DIMS;i++)
    {
      Id <<= VOX_COORD_SHIFT;
      int n = v[i];
      while (n < 0) n += root->voxnum[i];
      while (n >= root->voxnum[i]) n -= root->voxnum[i];
      assert (n >=0);
      assert(n < 1<<VOX_COORD_SHIFT);
      Id |= n;
    }
  return Id;
}

int ORBNode::VoxelExternalId( int *procnums )
{
  VALIDATE_ROOT 
  int n[DIMS];
  for(int i = 0 ; i < DIMS ; i++)
    n[ root->internfromext [ i ] ] = procnums[ i ];
  return VoxelId(n);
}

int ORBNode::VoxelExternalBounds(int Id , double *vlow, double *vhigh)
{
  double tlow[DIMS],thigh[DIMS];
  VoxelBounds(Id,tlow,thigh);
  ExternalFromInternal(tlow,vlow);
  ExternalFromInternal(thigh,vhigh);
  return Id;
}
int ORBNode::VoxelBounds(int Id , double *vlow, double *vhigh)
{
   VALIDATE_ROOT 
  // NOTE : voxel bounds are internal!!!!
  int mask = (1<<VOX_COORD_SHIFT) -1;
  int n[DIMS];
  n[2] = Id & mask;
  mask <<= VOX_COORD_SHIFT;
  n[1] = (Id & mask) >> VOX_COORD_SHIFT;
  mask <<= VOX_COORD_SHIFT;
  n[0] = (Id & mask) >> (2*VOX_COORD_SHIFT);
  for (int i = 0; i < DIMS; i++)
    {
      double H = root->high[i];
      double L = root->low[i];
      double D = H - L;
      double dV = D / root->voxnum[i];      
      vlow[i] = n[i] * dV + L;
      vhigh[i] = (n[i] + 1) * dV + L;
    }
  return 1;
}

void ORBNode::VoxelCoords(int Id , int *n)
{
  // NOTE : voxel coords are internal!!!!

  int mask = (1<<VOX_COORD_SHIFT) -1;
  n[2] = Id & mask;
  mask <<= VOX_COORD_SHIFT;
  n[1] = (Id & mask) >> VOX_COORD_SHIFT;
  mask <<= VOX_COORD_SHIFT;
  n[0] = (Id & mask) >> (2*VOX_COORD_SHIFT);
  return;
}

void ORBNode::CalcVoxelPoint(int aVoxelId, double *p)
{
  VALIDATE_ROOT 
  // Returns EXTERNAL sim space coords
  double L[DIMS],H[DIMS],mid[DIMS],dm[DIMS],dV[DIMS],delta[DIMS],frac[DIMS];
  int vCoord[DIMS];
  VoxelBounds(aVoxelId,L,H);
  VoxelCoords(aVoxelId,vCoord);

#if 1
  srand(aVoxelId);
  for (int i = 0; i < DIMS; i++)
    {
      dm[i] = H[i] - L[i];
      delta[i] = dm[i] * (rand()/(RAND_MAX + 1.0));
      frac[i] = ((double)(vCoord[i]+1))/ ((double)root->voxnum[i]);
    }
  for (int i = 0; i < DIMS; i++)
    {
      mid[i] = L[i] + 0.95 * delta[i] + 0.01 ;
    }
 
    ExternalFromInternal(mid,p);

#else
  VoxelIndex ind = VoxelIdToVoxelIndex(aVoxelId);
  double d = ((double) ind) + 1.0;
  double nV = root->voxnum[0] * root->voxnum[1] * root->voxnum[2] + 1;
  double ratio= d / nV;

  for (int i = 0; i < DIMS; i++)
    {
      dV[i] = (H[i] - L[i]) / root->voxnum[i];
      p[i] = ratio * dV[i] + L[i]; 
    }
#endif
  return;
}

void ORBNode::VoxelPoint(int aVoxelId, double *p, int cacheKnown)
{
#if 1
  VALIDATE_ROOT 
  typedef double tPoint[3];
  static tPoint * CachedVoxelPoints = NULL;                                                                                                                                                                                    
  // THIS IS NOT THREAD SAFE 
  if( (0 == cacheKnown) && (CachedVoxelPoints == NULL) )  
    {
    BegLogLine(0)    
      << "ORBNode::VoxelPoint() begin table gen"
      << EndLogLine;                
    int TotalVoxelCount = 1;      
    for(int i = 0; i < DIMS; i++ )
      {                           
	TotalVoxelCount *= root->voxnum[i]; 
      } 
    CachedVoxelPoints = (tPoint *) malloc( sizeof(tPoint) * TotalVoxelCount ); 
    if( CachedVoxelPoints == NULL )
      { 
      PLATFORM_ABORT("CachedVoxelPoints == NULL"); 
      // assert(	0 );
      }                                                                                                                                                                                                                        
    assert( DIMS == 3 );
    int v[3];
    for( v[0] = 0; v[0] < root->voxnum[0]; v[0]++)
      for( v[1] = 0; v[1] < root->voxnum[1]; v[1]++)
        for( v[2] = 0; v[2] < root->voxnum[2]; v[2]++)
          {          
	    int vid = VoxelId( v );   
	    VoxelIndex vind = VoxelIdToVoxelIndex( vid );
	    assert(vind < TotalVoxelCount );       
	    CalcVoxelPoint( vid, CachedVoxelPoints[ vind ] );    
          }
    BegLogLine(0)   
      << "ORBNode::VoxelPoint() End table gen" 
      << EndLogLine;                        
    }
  VoxelIndex vind = VoxelIdToVoxelIndex( aVoxelId );
  p[0] = CachedVoxelPoints[ vind ][0];  
  p[1] = CachedVoxelPoints[ vind ][1];
  p[2] = CachedVoxelPoints[ vind ][2];
#else   
  CalcVoxelPoint( aVoxelId, p );  
#endif
}


void ORBNode::VerifyContents()
{
  for(int a=0;a<nsites;a++)
    {
      Site *s = GetSite(a);
      for(int i=0;i<DIMS;i++)
	{
	  double v[DIMS];
	  InternalFromExternal(s->c,v);
	  if(v[i] < low[i]){printf("dim %d for site %d is too low! %f s= %f\n", i,a,low[i], v[i]); s->Print(); assert(0);} 
	  if(v[i] > high[i]){printf("dim %d for site %d is too high! %f s= %f\n",i,a, high[i], v[i]); s->Print(); assert(0);} 
	}
    }
}
int ORBNode::SetVoxelNums(int dx,int dy, int dz)
{
  VALIDATE_ROOT 	
  if(id < 0)
    {
      int level = -id;
      int delta = 1 << level-1;
      
      if(dims == 3)
	{
	  child[CUTZ1L]->SetVoxelNums(dx,dy,dz);
	  child[CUTZ3L]->SetVoxelNums(dx + delta,dy,dz);
	  
	  child[CUTZ2L]->SetVoxelNums(dx,dy + delta,dz);
	  child[CUTZ4L]->SetVoxelNums(dx + delta,dy + delta,dz);
	  
	  child[CUTZ1H]->SetVoxelNums(dx,dy,dz + delta);
	  child[CUTZ2H]->SetVoxelNums(dx,dy + delta,dz + delta);
	  child[CUTZ3H]->SetVoxelNums(dx + delta,dy,dz + delta);
	  child[CUTZ4H]->SetVoxelNums(dx + delta,dy + delta,dz + delta);
	}
      else if (dims == 2)
	{
	  child[CUTZ1L]->SetVoxelNums(dx,dy,dz);
	  child[CUTZ1H]->SetVoxelNums(dx,dy + delta,dz);
	  child[CUTZ2L]->SetVoxelNums(dx + delta,dy,dz);
	  child[CUTZ2H]->SetVoxelNums(dx + delta,dy + delta,dz);
	}
      else if (dims == 1)
	{
	  child[CUTZ1L]->SetVoxelNums(dx,dy,dz);
	  child[CUTZ1H]->SetVoxelNums(dx + delta,dy,dz);
	}
      else{ assert(0);} // dims bogus
    }
  else
    {
      assert(realnode);
      meshnum[0] = dx; meshnum[1] = dy; meshnum[2] = dz;

      int nd[DIMS];
      nd[ root->extfromintern[0] ] = dx;
      nd[ root->extfromintern[1] ] = dy;
      nd[ root->extfromintern[2] ] = dz;
#ifdef _ORB_STANDALONE_
      rank = (int ) this;
#else
      rank = Platform::Topology::MakeRankFromCoords( nd[ 0 ],nd[ 1 ],nd[ 2 ]);
#endif
    }
  return id;
}

ORBNode * ORBNode::NodeFromProcCoord(int x,int y, int z) 
{
  VALIDATE_ROOT 
  int nd[DIMS];
  nd[ root->internfromext[0] ] = x;
  nd[ root->internfromext[1] ] = y;
  nd[ root->internfromext[2] ] = z;

  return Node(nd[ 0 ],nd[ 1 ],nd[ 2 ]);
}

#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)
ORBNode * ORBNode::Node(int x,int y, int z)
{

  if(id < 0)
    {
      int level = -id;
      int half = 1 << (level-1);
      int xx,yy,zz;
      xx = x; yy = y; zz = z;
      
      if(dims == 3)
	{
	  int rx = xx % half;
	  int ry = yy % half;
	  int rz = zz % half;
	  
	  if(xx < half)
	    {
	      if(yy < half)
		{
		  if(zz < half)
		    {   return child[CUTZ1L]->Node(rx,ry,rz);    }
		  else
		    {   return child[CUTZ1H]->Node(rx,ry,rz);    }
		}
	      else 
		{
		  if(zz < half)
		    {   return child[CUTZ2L]->Node(rx,ry,rz);    }
		  else
		    {   return child[CUTZ2H]->Node(rx,ry,rz);    }
		}
	    }
	  else
	    {
	      if(yy < half)
		{
		  if(zz < half)
		    {   return child[CUTZ3L]->Node(rx,ry,rz);    }
		  else
		    {   return child[CUTZ3H]->Node(rx,ry,rz);    }
		}
	      else
		{
		  if(zz < half)
		    {   return child[CUTZ4L]->Node(rx,ry,rz);    }
		  else
		    {   return child[CUTZ4H]->Node(rx,ry,rz);    }
		}
	    }
	}
      else if (dims == 2)
	{
	  int rx = xx % half;
	  int ry = yy % half;
	  int rz = zz;
	  if(xx < half)
	    {
	      if(yy < half)
		{	return child[CUTZ1L]->Node(rx,ry,rz);    }
	      else 
		{	return child[CUTZ1H]->Node(rx,ry,rz);    }
	    }
	  else
	    {
	      if(yy < half)
		{   return child[CUTZ2L]->Node(rx,ry,rz);    }
	      else
		{	return child[CUTZ2H]->Node(rx,ry,rz);    }
	    }
	  
	}
      else if (dims == 1)
	{
	  int rx = xx % half;
	  int ry = yy;
	  int rz = zz;
	  if(xx < half)
	    {  return child[CUTZ1L]->Node(rx,ry,rz); }
	  else
	    {  return child[CUTZ1H]->Node(rx,ry,rz); }
	}
      else
	{PLATFORM_ABORT("Bogus Dims");} // bogus dims
    }
  return this;
}
#endif

int ORBNode::NumberLeaves(int n)
{
  if(!realnode)
    return n;
  int count = n;
  if(id < 0) // not a leaf
    {
      int nc = 1 << dims;      
      for(int i=0;i<nc;i++)
	{
	  count = child[i]->NumberLeaves(count);
	}
    }
  else
    {
      id = count++;
    }
  return count;
}

double ORBNode::GetTotalWeight()
  {
  double w = 0.0;
  for(int i=0;i<nsites;i++)
    {
    Site *t = GetSite(i);
    w += t->verlet_count;
    }
  return w;
  }

void ORBNode::ReportLocalFrags()
{
    for(int i=0;i<nsites;i++)
      {
      Site *t = GetSite( i );
      
      //      if( t->fA == 2706 && t->fB == 2708 )
      //      BegLogLine( PKFXLOG_REPORT_LOCAL_ORB_FRAGS )
      //        << "ORBFRAG "
      //        << t->fA 
      //        << " "
      //        << t->fB
      //        << EndLogLine;
      }
}

void ORBNode::ReportLeafData()
{
  VALIDATE_ROOT 
  if (!realnode)
    return;
  int printbounds = 0;
  int printnsites = 0;
  int printsites = 0;
  int printvoxels = 0;
  int printvolume = 1;
  int printverletsum = 0;
  int printweights = 1;
  int printloadzones = 0;
  int printloadzonecounts = 0;  
  int printlocalfrags = 0;  
  
  if(id < 0) // not a leaf
    {
      int nc = 1 << dims;      
      for(int i=0;i<nc;i++)
	{
	  child[i]->ReportLeafData();
	}
    }
  else 
    {
      if(printnsites)
	{
	  printf("Node %d \t %d \n",id,nsites);	  
	}
      if(printverletsum)
	{
	  printf("Node %d \t nsites %d \tverletsum   %f \n",rank,nsites,verlet_sum);	  
	}
      if(printbounds)
	{
	  for(int i = 0; i < DIMS; i++)
	    {      
	      printf("[ %f %f ] ",low[root->extfromintern[i]],high[root->extfromintern[i]]); 
	    }
	  printf("\n");
	}
      if(printsites)
	{
	  for(int i=0;i<nsites;i++)
	    {
	      GetSite(i)->Print();
	    }
	}
      if(printvolume)
        {
          double V = (high[0] - low[0]) * (high[1] - low[1]) * (high[2] - low[2]);
          printf("Node %d Volume = %f \n ",rank,V);
        }
      if(printweights)
	{
          double w = GetTotalWeight();

	  printf("weight [ %d %d %d ] = %f \n ",meshnum[0],meshnum[1],meshnum[2], w);
	}
      if(printvoxels)
	{
	  printf("[ %d %d %d ]\n ",meshnum[0],meshnum[1],meshnum[2]);
	}
      
      if(printloadzones)
        {
          for(int i = 0 ; i < nLoadVox; i++)
            {
              int Id = LoadZone[i];
              VoxelIndex ind = VoxelIdToVoxelIndex(Id);
              printf("LoadZone Node %d Vox Index %d  Vox Id %08x\n",id,ind,Id);
            }
        }
      
      if( printlocalfrags )
        {
        ReportLocalFrags();
        }
      //printf("\n");
    }
}


double ORBNode::NodeSqrDistance(ORBNode *nd)
{
  VALIDATE_ROOT 
  if(!realnode || !nd->realnode)
   {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  double vec[DIMS];
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      if( low[i] >= nd->high[i])  // low no overlap
	{
	  dp = low[i] - nd->high[i];	      
	  
	  dpp = dV + nd->low[i] - high[i];
	  if(dpp < dp)
	    dp = dpp;
	}
      else if( high[i] <= nd->low[i])
	{
	  dp = nd->low[i] - high[i];	      
	  
	  dpp = low[i] - ( nd->high[i] - dV );
	  if(dpp < dp)
	    dp = dpp;
	}
      vec[i] = dp * dp;
    }
  double d = 0;
  for(int i = 0;i< DIMS;i++)
    d += vec[i];
  return d;
} 

void ORBNode::NodeMinDeltas(double *alow, double *ahigh,double *vec)
{
  VALIDATE_ROOT 
  if(!realnode)
   {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      if( low[i] >= ahigh[i])  // low no overlap
	{
	  dp = low[i] - ahigh[i];	      
	  
	  dpp = dV + alow[i] - high[i];
	  if(dpp < dp)
	    dp = dpp;
	}
      else if( high[i] <= alow[i])
	{
	  dp = alow[i] - high[i];	      
	  
	  dpp = low[i] - ( ahigh[i] - dV );
	  if(dpp < dp)
	    dp = dpp;
	}

      if( !(dp >= 0.0) )
        BegLogLine( 1 )
          << i
          << " dp: " << dp
          << " dpp: " << dpp
          << " alow: { " 
          << alow[ 0 ] << " "
          << alow[ 1 ] << " "
          << alow[ 2 ] << " }"
          << " ahigh: { " 
          << ahigh[ 0 ] << " "
          << ahigh[ 1 ] << " "
          << ahigh[ 2 ] << " }"
          << " low: { " 
          << low[ 0 ] << " "
          << low[ 1 ] << " "
          << low[ 2 ] << " }"
          << " high: { " 
          << high[ 0 ] << " "
          << high[ 1 ] << " "
          << high[ 2 ] << " }"
          << EndLogLine;
      
      assert(dp >= 0.0);
      vec[i] = dp ;
    }
  return;
} 

void ORBNode::NodeMinExternalDeltas(ORBNode *nd,double *vec)
{
  double iv[DIMS];
  NodeMinDeltas(nd,iv);
  ExternalFromInternal(iv,vec);

}
void ORBNode::NodeMinDeltas(ORBNode *nd,double *vec)
{
  VALIDATE_ROOT 
  if(!realnode || !nd->realnode)
   {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      if( low[i] >= nd->high[i])  // low no overlap
	{
	  dp = low[i] - nd->high[i];	      
	  
	  dpp = dV + nd->low[i] - high[i];
	  if(dpp < dp)
	    dp = dpp;
	}
      else if( high[i] <= nd->low[i])
	{
	  dp = nd->low[i] - high[i];	      
	  
	  dpp = low[i] - ( nd->high[i] - dV );
	  if(dpp < dp)
	    dp = dpp;
	}
      vec[i] = dp ;
    }
  return;
} 


void ORBNode::NodeBoundaryDistances(ORBNode *nd, double &min, double &max)
{
  VALIDATE_ROOT 
  if(!realnode || !nd->realnode)
     {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}

  min = max = 0.0;
  for(int i = 0;i< DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      double dp = 0.0;
      double dpp = 0.0;
      double dm = 0.0;
      double dmm = 0.0;
      if( low[i] >= nd->high[i])  // low no overlap
	{
	  dp = low[i] - nd->high[i];	      
	  dm = low[i] - nd->low[i];

	  dpp = dV + nd->low[i] - high[i];
	  dmm = dV + nd->high[i] - high[i];
	  if(dpp < dp)
	    { dp = dpp; dm = dmm; }
	}
      else if( high[i] <= nd->low[i])  // high no overlap
	{
	  dp = nd->low[i] - high[i];	      
	  dm = nd->high[i] - high[i];	      
	  
	  dpp = low[i] - ( nd->high[i] - dV );
	  dmm = low[i] - ( nd->low[i] - dV );
	  if(dpp < dp)
	    { dp = dpp; dm = dmm; }
	}
      else if( low[i] >= nd->low[i] ) // low with overlap
	{
	  dp = 0.0;
	  dm = low[i] - nd->low[i];
	}
      else if( high[i] <= nd->high[i] ) // high with overlap
	{
	  dp = 0.0;
	  dm = nd->high[i] - high[i];
	}
      min += dp*dp ;
      max += dm*dm ;
    }
  min = sqrt(min);
  max = sqrt(max);
  return;
} 


void ORBNode::NodeMaxDeltas(ORBNode *nd,double *vec)
{
  VALIDATE_ROOT 
  if(!realnode || !nd->realnode)
     {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      double w1 = high[i] - low[i];
      double w2 = nd->high[i] - nd->low[i];
      if( low[i] >= nd->high[i])  // low no overlap
	{
	  dp = low[i] - nd->high[i];	      	  
	  dpp = dV + nd->low[i] - high[i];
	  if(dpp < dp)	    dp = dpp;
	  vec[i] = dp + w1 + w2;
	}
      else if( high[i] <= nd->low[i])
	{
	  dp = nd->low[i] - high[i];	      
	  dpp = low[i] - ( nd->high[i] - dV );
	  if(dpp < dp)	    dp = dpp;
	  vec[i] = dp + w1 + w2;
	}
      else if( nd->high[i] >= low[i])
	{
	  vec[i] = nd->high[i] - low[i];
	}
      else if( high[i] >= nd->low[i])
	{
	  vec[i] = high[i] - nd->low[i];
	}
      else
	{assert(0);}
    }
  return;
} 


int ORBNode::RanksDeltaFromBox(double *Alow, double *Ahigh, double dX, double dY, double dZ, int* ranks, int &len, int max)
{
  VALIDATE_ROOT 
  double alow[DIMS],ahigh[DIMS], adel[DIMS];
  int vlen = 0;
  len = 0;
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  int NVox = root->voxnum[0] * root->voxnum[1] * root->voxnum[2];
  assert(max >= NNodes);

  InitSet(ranks,NNodes);
  int *voxels = new int[NVox];
  if(voxels == NULL)
    {PLATFORM_ABORT("Bit off more than can be chewed");}
  InitSet(voxels,NVox);

  InternalFromExternal(Alow,alow);
  InternalFromExternal(Ahigh,ahigh);
  adel[ root->internfromext[ 0 ] ] = dX;
  adel[ root->internfromext[ 1 ] ] = dY;
  adel[ root->internfromext[ 2 ] ] = dZ;
  
  for(int i=0;i<DIMS;i++)
    {
      alow[i] -= adel[i];
      ahigh[i] += adel[i];
    }

  int nvox = VoxelsInBox(alow, ahigh , voxels, vlen, NVox);
  assert(nvox > 0);
  int nranks = RanksFromVoxels(voxels,nvox,ranks,len, NNodes);

  delete [] voxels;
  return len;      
}

int ORBNode::RanksInLoadBox(double *Vlow, double* Vhigh,int *voxels, int &vlen, int vmax,int* ranks, int &len, int max)
{

  double vlow[DIMS],vhigh[DIMS];
  InternalFromExternal(Vlow,vlow);
  InternalFromExternal(Vhigh,vhigh);

  int nvox = VoxelsInBox(vlow, vhigh , voxels, vlen, vmax);
  assert(nvox > 0);
  int nranks = RanksFromVoxels(voxels,nvox,ranks,len, max);
  return nranks;
}

int ORBNode::RanksInBox(int px, int py, int pz, double dX, double dY, double dZ, int* ranks, int &len, int max)
{
  VALIDATE_ROOT 
  ORBNode * mynode = NodeFromProcCoord(px,py,pz);
  if(mynode == NULL)
    { return 0; }

  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);
  InitSet(ranks,NNodes);

  double box[DIMS];
  box[ root->internfromext[ 0 ] ] = dX*dX;
  box[ root->internfromext[ 1 ] ] = dY*dY;
  box[ root->internfromext[ 2 ] ] = dZ*dZ;
  int nc = 1<< dims;
  if(id < 0)
    {
      for(int i = 0;i < nc;i++)
	{
	  child[i]->AddRanks(mynode,box,ranks,len,max); 
	}
    }
  else
    {
      AddRanks(mynode,box,ranks,len,max);
    }
  CollapseIntSet(ranks,len,NNodes);

  return len;      
}

int ORBNode::RanksFromVoxels(int * voxels,int nvox, int *ranks,int &len, int max)
{
  VALIDATE_ROOT 
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);
  InitSet(ranks,NNodes);

  for(int i = 0;i< nvox;i++)
    {
      double v[DIMS];
      VoxelPoint(voxels[i],v);
      ORBNode *nd = root->NodeFromSimCoord(v[0],v[1],v[2]);
#ifdef _ORB_STANDALONE_
      ranks[nd->id] = nd->rank;
#else
      ranks[nd->rank] = nd->rank;
#endif
    }
  CollapseIntSet(ranks,len,NNodes);
  return len;      
}
#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)

int ORBNode::VoxelsInExternalBox(double *Vlow, double *Vhigh , int *voxels, int &len, int max)
{
  double vlow[DIMS],vhigh[DIMS];
  InternalFromExternal(Vlow,vlow);
  InternalFromExternal(Vhigh,vhigh);
  return VoxelsInBox(vlow,vhigh,voxels,len,max);
}
#endif
int ORBNode::VoxelsInBox(double *vlow, double *vhigh , int *voxels, int &len, int max)
{
  VALIDATE_ROOT 
  // vlow and vhigh are internal sim coords
  int NVox = root->voxnum[0] *root->voxnum[1] *root->voxnum[2];
  assert(max >= NVox);
  InitSet(voxels,NVox);
  
  double dr[DIMS];
  int vnlow[DIMS],vnhigh[DIMS];

  VoxelRange(vlow,vhigh,vnlow,vnhigh);

  int ni[DIMS];
  for(ni[0] = vnlow[0]; ni[0] <= vnhigh[0] ; ni[0]++)
    {
      for(ni[1] = vnlow[1]; ni[1] <= vnhigh[1] ; ni[1]++)
	{
	  for(ni[2] = vnlow[2]; ni[2] <= vnhigh[2] ; ni[2]++)
	    {
	      int Id = VoxelId(ni);
	      VoxelIndex vi = VoxelIdToVoxelIndex(Id);
	      voxels[vi] = Id;
	    }
	}
    }
  CollapseIntSet(voxels,len,NVox);
  return len;      
}

int ORBNode::VoxelsInNeighborhood( double r, int* voxels, int &len, int max)
{
  VALIDATE_ROOT 
  // allow accumulation  len = 0;
  double rr = r*r;
  int NVox = root->voxnum[0] *root->voxnum[1] *root->voxnum[2];
  assert(max >= NVox);
  InitSet(voxels,NVox);

  OrVoxelNeighborSet( r, voxels, len, max);

  CollapseIntSet(voxels,len,NVox);
  return len;      
}

void ORBNode::VoxelRange(double *vlow, double *vhigh, int *vnlow, int *vnhigh)
{
   VALIDATE_ROOT 
  // Note: internal sim coords only!!!
  for(int i = 0;i < DIMS; i++)
    {
      double H = root->high[i];
      double L = root->low[i];
      double D = H - L;
      double CL = (vlow[i] - L) / D ;
      double CH = (vhigh[i] - L) / D;
      while(CL < 0.0) // Get range into positive images before quantization
	{
	  CL += 1.0;
	  CH += 1.0;
	}
      vnlow[i] = (int) (CL * root->voxnum[i]);
      vnhigh[i] = (int) (CH * root->voxnum[i]);
    }
  return;
}


void ORBNode::OrVoxelNeighborSet( double r, int* voxels, int &len, int max)
{
  double dr[DIMS];
  double vlow[DIMS],vhigh[DIMS];
  int vnlow[DIMS],vnhigh[DIMS];

  double rr = r*r;
  for(int i = 0;i < DIMS; i++)
    {
      vlow[i] = low[i] - r;
      vhigh[i] = high[i] + r;
    }
  VoxelRange(vlow,vhigh,vnlow, vnhigh);
  int ni[DIMS];
  for(ni[0] = vnlow[0]; ni[0] <= vnhigh[0] ; ni[0]++)
    {
      for(ni[1] = vnlow[1]; ni[1] <= vnhigh[1] ; ni[1]++)
	{
	  for(ni[2] = vnlow[2]; ni[2] <= vnhigh[2] ; ni[2]++)
	    {
	      double vd[DIMS];
	      int Id = VoxelId(ni);
	      VoxelBounds(Id,vlow,vhigh);
	      NodeMinDeltas(vlow,vhigh,vd);
	      double minrr = 0.0;
	      for(int ii=0;ii<DIMS;ii++)
		minrr += vd[ii]*vd[ii];

	      if(DBGV)
		{if(NODE_ZERO) printf(" voxinneighborhood checking Id %x %d %d %d   minrr %f\n",Id,ni[0],ni[1],ni[2], minrr);}
	      if (minrr <= rr)
		{
		  VoxelIndex vi = VoxelIdToVoxelIndex(Id);
		  voxels[vi] = Id;
		}
	    }
	}
    } 
  return; 
}
int ORBNode::RanksInHomeZoneSubsetNeighborhood(int voxIdLow, int voxIdHigh, double r, int* ranks, int &len,int max)
{
  VALIDATE_ROOT 
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);

  InitSet(ranks,NNodes);

  double dlow[DIMS],dhigh[DIMS],dtemp[DIMS];
  VoxelBounds(voxIdLow,dlow,dtemp);
  VoxelBounds(voxIdHigh,dtemp,dhigh);
  double vlow[DIMS],vhigh[DIMS];
  int vnlow[DIMS],vnhigh[DIMS];
  ORBNode nd;
  nd.realnode = 1;
  nd.root  = root;

  double rr = r*r;
  for(int i = 0;i < DIMS; i++)
    {
      nd.low[i] = dlow[i] - r;
      nd.high[i] = dhigh[i] + r;
      nd.reciprocal[i] = 1.0/(nd.high[i]-nd.low[i]) ;
    }

  VoxelRange(nd.low,nd.high,vnlow, vnhigh);
  int ni[DIMS];
  for(ni[0] = vnlow[0]; ni[0] <= vnhigh[0] ; ni[0]++)
    {
      for(ni[1] = vnlow[1]; ni[1] <= vnhigh[1] ; ni[1]++)
	{
	  for(ni[2] = vnlow[2]; ni[2] <= vnhigh[2] ; ni[2]++)
	    {
	      double vd[DIMS];
	      int Id = VoxelId(ni);
	      VoxelBounds(Id,vlow,vhigh);
	      nd.NodeMinDeltas(vlow,vhigh,vd);
	      double minrr = 0.0;
	      for(int ii=0;ii<DIMS;ii++)
		minrr += vd[ii]*vd[ii];
	      if (minrr <= rr)
		{
		  VoxelPoint(Id,vd);
		  ORBNode *nnd = root->NodeFromSimCoord(vd[0], vd[1], vd[2]);
#ifdef _ORB_STANDALONE_
		  ranks[nnd->id] = nnd->rank;
#else
		  ranks[nnd->rank] = nnd->rank;
#endif
		}
	    }
	}
    } 
  CollapseIntSet(ranks,len,NNodes);
  return len; 

}

int ORBNode::RanksInNeighborhoodVersion2(int px, int py, int pz, double r, int* ranks, int &len, int max)
{
  VALIDATE_ROOT 
  ORBNode * mynode = NodeFromProcCoord(px,py,pz);
  if(mynode == NULL)
    { return 0; }
  
  int MaxVoxels = root->voxnum[ 0 ] * root->voxnum[ 1 ] * root->voxnum[ 2 ];  

  int* MyVoxels = (int *) malloc( sizeof(int) * MaxVoxels );

  if( MyVoxels == NULL )
    {
    PLATFORM_ABORT("MyVoxels == NULL");
    }

  int VoxelCount;
  mynode->VoxelsInNeighborhood( r, MyVoxels, VoxelCount, MaxVoxels );
  
  RanksFromVoxels( MyVoxels, VoxelCount, ranks, len, MaxVoxels );
  
  free( MyVoxels );

  return len;
}

int ORBNode::RanksInNeighborhood(int px, int py, int pz, double r, int* ranks, int &len, int max)
{
  VALIDATE_ROOT 
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);
  InitSet(ranks,NNodes);

  ORBNode * mynode = NodeFromProcCoord(px,py,pz);
  if(mynode == NULL)
    { return 0; }
  
  double rr = r*r;
  int nc = 1<< dims;
  if(id < 0)
    {
      for(int i = 0;i < nc;i++)
	{
	  child[i]->AddRanks(mynode,rr,ranks,len,max); 
	}
    }
  else
    {
      AddRanks(mynode,rr,ranks,len,max);
    }
  CollapseIntSet(ranks,len,NNodes);

  return len;
}

int ORBNode::RanksInNeighborhoodSkin(int px, int py, int pz, double r, int* ranks, int &len, int max)
{
  VALIDATE_ROOT 
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);
  InitSet(ranks,NNodes);

  ORBNode * mynode = NodeFromProcCoord(px,py,pz);
  if(mynode == NULL)
    { 
      printf("Bogus proc coords %d %d %d\n",px,py,pz);
      PLATFORM_ABORT("Bogus Proc Coords"); 
    }
  
  double rr = r*r;
  int nc = 1<< dims;
  if(id < 0)
    {
      for(int i = 0;i < nc;i++)
	{
	  child[i]->AddSkinRanks(mynode,rr,ranks,len,max); 
	}
    }
  else
    {
      AddSkinRanks(mynode,rr,ranks,len,max);
    }
  CollapseIntSet(ranks,len,NNodes);

  return len;
}

int ORBNode::AddSkinRanks(ORBNode * mynode, double r,int * ranks, int &len,int max)
{
  if(!realnode)
      {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  double d = NodeSqrDistance(mynode);
  if(d <= r)
    {
      if(id < 0)
	{
	  int nc = 1<< dims;
	  for(int i = 0;i < nc;i++)
	    {
	      child[i]->AddSkinRanks(mynode,r,ranks,len,max); 
	    }
	}
      else
	{
	  if(id != mynode->id)
	    {
	      double rr = sqrt(r);
	      int skin_flick = IsNodeOnBoundary(mynode,rr);
	      if(skin_flick)
#ifdef _ORB_STANDALONE_
		ranks[id] = rank;
#else
		ranks[rank] = rank;
#endif
	    }
	}
    }
  return len;
}
#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)

int ORBNode::AddRanks(ORBNode * mynode, double r,int * ranks, int &len,int max)
{
  if(!realnode)
      {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  double d = NodeSqrDistance(mynode);
  if(d <= r)
    {
      if(id < 0)
	{
	  int nc = 1<< dims;
	  for(int i = 0;i < nc;i++)
	    {
	      child[i]->AddRanks(mynode,r,ranks,len,max); 
	    }
	}
      else
	{
	  // if(id != mynode->id)
	    {
#ifdef _ORB_STANDALONE_
	      ranks[id] = rank;
#else
	      ranks[rank] = rank;
#endif
	    }
	}
    }
  return len;
}

int ORBNode::AddRanks(ORBNode * mynode, double *box,int * ranks, int &len,int max)
{
  if(!realnode)
       {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  double del[DIMS];
  NodeMinDeltas(mynode,del);
  int inside = 1;
  for(int i = 0; i< DIMS; i++)
    {
      del[i] *= del[i];
      if (del[i] > box[i]) 
	{inside = 0; break;}
    }
  if(inside )
    {
      if(id < 0)
	{
	  int nc = 1<< dims;
	  for(int i = 0;i < nc;i++)
	    {
	      child[i]->AddRanks(mynode,box,ranks,len,max); 
	    }
	}
      else
	{
	  // if(id != mynode->id)
	    {
#ifdef _ORB_STANDALONE_
	      ranks[id] = rank;
#else
	      ranks[rank] = rank;
#endif
	    }
	}
    }
  return len;
}


int ORBNode::AddRanks(double *alow, double *ahigh, double *box,int * ranks, int &len,int max)
{
  if(!realnode)
      {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  double deltas[DIMS];
  NodeMinDeltas(alow,ahigh,deltas);
  int inside = 1;
  for(int i = 0; i< DIMS; i++)
    {
      if (deltas[i] > box[i]) 
	{inside = 0; break;}
    }
  if(inside )
    {
      if(id < 0)
	{
	  int nc = 1<< dims;
	  for(int i = 0;i < nc;i++)
	    {
	      child[i]->AddRanks(alow,ahigh,box,ranks,len,max); 
	    }
	}
      else
	{
	  // if(id != mynode->id)
	    {
#ifdef _ORB_STANDALONE_
	      ranks[rank] = rank;
#else
	      ranks[rank] = rank;
#endif
	    }
	}
    }
  return len;
}
#endif

ORBNode * ORBNode::NodeFromSimCoord(double x,double y, double z) 
{
  VALIDATE_ROOT 
  double nd[DIMS];
  nd[ root->internfromext[0] ] = x;
  nd[ root->internfromext[1] ] = y;
  nd[ root->internfromext[2] ] = z;

  return Node(nd[ 0 ],nd[ 1 ],nd[ 2 ]);
}

#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)
ORBNode * ORBNode::Node(double x, double y, double z)
{
  if(!realnode)   {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  if(x < low[0]) return NULL;
  if(x > high[0]) return NULL;
  if(y < low[1]) return NULL;
  if(y > high[1]) return NULL;
  if(z < low[2]) return NULL;
  if(z > high[2]) return NULL;
  if(id >=0 )
    return this;
  
  if(dims == 3)
    {
      if(x < child[CUTZ1L]->high[0]) // low X
	{
	  if ( y < child[CUTZ1L]->high[1]) // zcut1 
	    {
	      if( z < child[CUTZ1L]->high[2])  // zcut1L
		{
		  return child[CUTZ1L]->Node(x,y,z);
		}
	      else if( z >= child[CUTZ1H]->low[2])  //  Zcut1H
		{
		  return child[CUTZ1H]->Node(x,y,z);
		}
	      else { assert(0);} // tree is bogus
	    }
	  else   // zcut 2
	    {
	      if( z < child[CUTZ2L]->high[2])  // zcut2L
		{
		  return child[CUTZ2L]->Node(x,y,z);
		}
	      else if( z >= child[CUTZ2H]->low[2])  //  Zcut1H
		{
		  return child[CUTZ2H]->Node(x,y,z);
		}
	      else {assert(0);} // tree is bogus
	    }
	}
      else if(x >= child[CUTZ3L]->low[0]) // high X
	{
	  if ( y < child[CUTZ3L]->high[1]) // zcut3 
	    {
	      if( z < child[CUTZ3L]->high[2])  // zcut3L
		{
		  return child[CUTZ3L]->Node(x,y,z);
		}
	      else if( z >= child[CUTZ3H]->low[2])  //  Zcut3H
		{
		  return child[CUTZ3H]->Node(x,y,z);
		}
	      else {assert(0);} // tree bogus
	    }
	  else  if ( y >= child[CUTZ4H]->low[1]) // zcut 4
	    {
	      if( z < child[CUTZ4L]->high[2])  // zcut4L
		{
		  return child[CUTZ4L]->Node(x,y,z);
		}
	      else if ( z >= child[CUTZ4H]->low[2])  //  Zcut4H
		{
		  return child[CUTZ4H]->Node(x,y,z);
		}
	    }
	  else { assert(0);} // tree is bogus
	}
      else { assert(0);} // tree is bogus
    }
  else if (dims == 2)
    {
      if(x < child[CUTZ1L]->high[0]) // low X
	{
	  if ( y < child[CUTZ1L]->high[1]) // zcut1 
	    {
	      return child[CUTZ1L]->Node(x,y,z);
	    }
	  else   // zcut 2
	    {		      
	      return child[CUTZ1H]->Node(x,y,z);
	    }
	}
      else if(x >= child[CUTZ2L]->low[0]) // high X
	{
	  if ( y < child[CUTZ2L]->high[1]) // zcut3 
	    {
	      return child[CUTZ2L]->Node(x,y,z);
	    }
	  else  if ( y >= child[CUTZ2H]->low[1]) // zcut 4
	    {
	      return child[CUTZ2H]->Node(x,y,z);
	    }
	  else { assert(0);} // tree is bogus
	}
      else { assert(0);} // tree is bogus
    }
  else if(dims == 1)
    {
      if(x < child[CUTZ1L]->high[0]) // low X
	{
	  return child[CUTZ1L]->Node(x,y,z);
	}
      else if(x >= child[CUTZ1H]->low[0]) // high X
	{
	  return child[CUTZ1H]->Node(x,y,z);
	}
      else { assert(0);} // tree is bogus
    }
  else
	{ assert(0); } // dims are bogus
  return NULL;
}
#endif

void ORBNode::SwapNodes(ORBNode **n1,ORBNode **n2)
{ORBNode *t = *n1; *n1 = *n2; *n2 = t;} 

int ORBNode::DetermineDims(int level)
{
  VALIDATE_ROOT 
  int X = root->procnum[0];
  int Y = root->procnum[1];
  int Z = root->procnum[2];
  
  int nX = 0,nY = 0,nZ = 0;

  while(X > 1){ nX++; X >>= 1; }
  while(Y > 1){ nY++; Y >>= 1; }
  while(Z > 1){ nZ++; Z >>= 1; }

  int d = 0;
  if(nX >= level) d++;
  if(nY >= level) d++;
  if(nZ >= level) d++;
  return d;
}

#if defined(ORB_SORTED)
int ORBNode::Subdivide(int levels)
{
  int d = DIMS;
  int targetdim;
  if(parent == NULL)
    {
      realnode = 1;
    }

  if(root == this)
    {
      root->mTag = ROOT_ID;
      /// If we're the root, determine the level based on procnums
      levels = 1;
      while(DetermineDims(levels) > 0)
	{
	  levels++;   // stunt to cause extra divisions of long dim
	}
      levels--;
      id = -levels;
    }

  // Overwrite dims based on level
  dims = DetermineDims(levels);
  if(levels > 0)
    {
      int n = NCHILDREN;
      for(int i = 0;i < n; i++)
	{
	  child[i] = new ORBNode();
          assert( child[i] != NULL );

	  Adopt(child[i]);
	  child[i]->id = -(levels - 1);
	}
 
      // Xcut
      targetdim = root->extfromintern[0];

      Sort(targetdim);
      TakeCut(this,child[CUTXL],child[CUTXH],0);
      
      if(dims > 1)
	{
	  targetdim = root->extfromintern[1];
	  
	  // YCut 1
	  child[CUTXL]->Sort(targetdim);
	  TakeCut(child[CUTXL],child[CUTY1L],child[CUTY1H],1);
	  
	  //Y Cut 2
	  child[CUTXH]->Sort(targetdim);
	  TakeCut(child[CUTXH],child[CUTY2L],child[CUTY2H],1);
	  
	  if(dims > 2)
	    {
	      targetdim = root->extfromintern[2];

	      // ZCut 1
	      child[CUTY1L]->Sort(targetdim);
	      TakeCut(child[CUTY1L],child[CUTZ1L],child[CUTZ1H],2);

	      
	      // ZCut 2
	      child[CUTY1H]->Sort(targetdim);
	      TakeCut(child[CUTY1H],child[CUTZ2L],child[CUTZ2H],2);
	      
	      // ZCut 3
	      child[CUTY2L]->Sort(targetdim);
	      TakeCut(child[CUTY2L],child[CUTZ3L],child[CUTZ3H],2);
	      
	      // ZCut 4
	      child[CUTY2H]->Sort(targetdim);
	      TakeCut(child[CUTY2H],child[CUTZ4L],child[CUTZ4H],2);
	    }
	  else
	    {
	      SwapNodes(&child[CUTY1L],&child[CUTZ1L]);
	      SwapNodes(&child[CUTY1H],&child[CUTZ1H]);
	      SwapNodes(&child[CUTY2L],&child[CUTZ2L]);
	      SwapNodes(&child[CUTY2H],&child[CUTZ2H]);
	    }	      
	}
      else
	{
	  SwapNodes(&child[CUTXL],&child[CUTZ1L]);
	  SwapNodes(&child[CUTXH],&child[CUTZ1H]);
	}
      
      // Final children are arranged 0-7; 8-13 are intermediate and can be removed if memory is and issue
      n = 1 << dims;
      for(int i=0;i<n;i++)
	child[i]->Subdivide(levels - 1);
      
    }
  else
    {
      // Here we have arrived at a leaf.For now just report.
      assert(realnode);
      VerifyContents();
    }

  if(root == this)
    {
      // post init stuff
      NumberLeaves( 0 );
      SetVoxelNums( 0, 0, 0 );
      UpdateNodeVerletCount();
      WhipperUp();
    }
  return 0;
}
#else
int ORBNode::Subdivide(int levels)
{
  int d = DIMS;
  int targetdim;
  if(parent == NULL)
    {
      realnode = 1;
    }

  if(root == this)
    {
      root->mTag = ROOT_ID;
      /// If we're the root, determine the level based on procnums
      levels = 1;
      while(DetermineDims(levels) > 0)
	{
	  levels++;   // stunt to cause extra divisions of long dim
	}
      levels--;
      id = -levels;
    }

  // Overwrite dims based on level
  dims = DetermineDims(levels);
  if(levels > 0)
    {
      int n = NCHILDREN;
      for(int i = 0;i < n; i++)
	{
	  child[i] = new ORBNode();
          assert( child[i] != NULL );

	  Adopt(child[i]);
	  child[i]->id = -(levels - 1);
	}
 
      // Xcut
      targetdim = root->extfromintern[0];

//      Sort(targetdim);
      TakeCutUnsorted(this,child[CUTXL],child[CUTXH],0);
      
      if(dims > 1)
	{
	  targetdim = root->extfromintern[1];
	  
	  // YCut 1
//	  child[CUTXL]->Sort(targetdim);
	  TakeCutUnsorted(child[CUTXL],child[CUTY1L],child[CUTY1H],1);
	  
	  //Y Cut 2
//	  child[CUTXH]->Sort(targetdim);
	  TakeCutUnsorted(child[CUTXH],child[CUTY2L],child[CUTY2H],1);
	  
	  if(dims > 2)
	    {
	      targetdim = root->extfromintern[2];

	      // ZCut 1
//	      child[CUTY1L]->Sort(targetdim);
	      TakeCutUnsorted(child[CUTY1L],child[CUTZ1L],child[CUTZ1H],2);

	      
	      // ZCut 2
//	      child[CUTY1H]->Sort(targetdim);
	      TakeCutUnsorted(child[CUTY1H],child[CUTZ2L],child[CUTZ2H],2);
	      
	      // ZCut 3
//	      child[CUTY2L]->Sort(targetdim);
	      TakeCutUnsorted(child[CUTY2L],child[CUTZ3L],child[CUTZ3H],2);
	      
	      // ZCut 4
//	      child[CUTY2H]->Sort(targetdim);
	      TakeCutUnsorted(child[CUTY2H],child[CUTZ4L],child[CUTZ4H],2);
	    }
	  else
	    {
	      SwapNodes(&child[CUTY1L],&child[CUTZ1L]);
	      SwapNodes(&child[CUTY1H],&child[CUTZ1H]);
	      SwapNodes(&child[CUTY2L],&child[CUTZ2L]);
	      SwapNodes(&child[CUTY2H],&child[CUTZ2H]);
	    }	      
	}
      else
	{
	  SwapNodes(&child[CUTXL],&child[CUTZ1L]);
	  SwapNodes(&child[CUTXH],&child[CUTZ1H]);
	}
      
      // Final children are arranged 0-7; 8-13 are intermediate and can be removed if memory is and issue
      n = 1 << dims;
      for(int i=0;i<n;i++)
	child[i]->Subdivide(levels - 1);
      
    }
  else
    {
      // Here we have arrived at a leaf.For now just report.
      assert(realnode);
      VerifyContents();
    }

  if(root == this)
    {
      // post init stuff
      NumberLeaves( 0 );
      SetVoxelNums( 0, 0, 0 );
      UpdateNodeVerletCount();
      WhipperUp();
    }
  return 0;
}
#endif
int ORBNode::TakeCutUnsorted(ORBNode *src, ORBNode *destL, ORBNode *destH, int dim)
{
	VALIDATE_ROOT
  //
  //  site coords are external
  //
    int xdim = root->extfromintern[dim];
	double cut ;
	int N = src->FindCutSiteUnsorted(cut, xdim);
	src->ArrangeToPartition(N,cut, xdim) ;
//  Site *cutsite = NULL;
//  if(N > 0) cutsite = src->GetSite(N-1);
  int N2 = src->nsites;

//	  int xdim = root->extfromintern[dim];
//	  double cut = (cutsite != NULL) ? cutsite->c[xdim] : 0.0;

  for(int i = 0;i < DIMS;i++)
    {
      destL->low[i] = src->low[i];
      destL->high[i] = src->high[i];
      destL->reciprocal[i] = src->reciprocal[i] ;
      BegLogLine(PKFXLOG_ORBNODE) 
        << "ORBNode::TakeCut " << src->high[i]
        << " " << src->low[i]
        << " " << src->reciprocal[i]
        << EndLogLine ;
      
      destH->low[i] = src->low[i];
      destH->high[i] = src->high[i];
      destH->reciprocal[i] = src->reciprocal[i] ;
    }
  
  destL->SetStart(src->StartSite());
  destL->SetLen(N);	       	  
  destL->CopySites(src,0,N);
  destL->high[dim] = cut;
  destL->reciprocal[dim] = 1.0/(destL->high[dim]-destL->low[dim]) ;
  destL->realnode = 1;
  //  destL->VerifyContents();
  
  destH->SetStart(src->StartSite() + N);
  destH->SetLen(N2-N);
  destH->CopySites(src,N,N2-N);
  destH->low[dim] = cut;
  destH->reciprocal[dim] = 1.0/(destH->high[dim]-destH->low[dim]) ;
  destH->realnode = 1;

  //  destH->VerifyContents();
  
  return 0;
	
}
int ORBNode::TakeCut(ORBNode *src, ORBNode *destL, ORBNode *destH, int dim)
{
  VALIDATE_ROOT 
  int N = src->FindCutSite();
  Site *cutsite = NULL;
  if(N > 0) cutsite = src->GetSite(N-1);
  int N2 = src->nsites;

  //
  //  site coords are external
  //
  int xdim = root->extfromintern[dim];
  double cut = (cutsite != NULL) ? cutsite->c[xdim] : 0.0;

  for(int i = 0;i < DIMS;i++)
    {
      destL->low[i] = src->low[i];
      destL->high[i] = src->high[i];
      destL->reciprocal[i] = src->reciprocal[i] ;
      BegLogLine(PKFXLOG_ORBNODE) 
        << "ORBNode::TakeCut " << src->high[i]
        << " " << src->low[i]
        << " " << src->reciprocal[i]
        << EndLogLine ;
      
      destH->low[i] = src->low[i];
      destH->high[i] = src->high[i];
      destH->reciprocal[i] = src->reciprocal[i] ;
    }
  
  destL->SetStart(src->StartSite());
  destL->SetLen(N);	       	  
  destL->CopySites(src,0,N);
  destL->high[dim] = cut;
  destL->reciprocal[dim] = 1.0/(destL->high[dim]-destL->low[dim]) ;
  destL->realnode = 1;
  //  destL->VerifyContents();
  
  destH->SetStart(src->StartSite() + N);
  destH->SetLen(N2-N);
  destH->CopySites(src,N,N2-N);
  destH->low[dim] = cut;
  destH->reciprocal[dim] = 1.0/(destH->high[dim]-destH->low[dim]) ;
  destH->realnode = 1;

  //  destH->VerifyContents();
  
  return 0;
}
int ORBNode::SubdivideEvenly(int levels)
{
  int d = DIMS;
  int targetdim;

  if(parent == NULL)
    {
      realnode = 1;
    }

  if(root == this)
    {
      root->mTag = ROOT_ID;
      /// If we're the root, determine the level based on procnums
      levels = 1;
      while(DetermineDims(levels) > 0)
	{
	  levels++;   // stunt to cause extra divisions of long dim
	}
      levels--;
      id = -levels;
    }
   // Overwrite dims based on level
  dims = DetermineDims(levels);
  if(levels > 0)
    {
      int n = NCHILDREN;
      for(int i = 0;i < n; i++)
	{
	  child[i] = new ORBNode();
          assert( child[i] != NULL );

	  Adopt(child[i]);
	  child[i]->id = -(levels - 1);
	}
 
      // Xcut
      targetdim = root->extfromintern[0];
      Sort(targetdim);
      TakeEvenCut(this,child[CUTXL],child[CUTXH],0);
      
      if(dims > 1)
	{
	  targetdim = root->extfromintern[1];
	  
	  // YCut 1
	  child[CUTXL]->Sort(targetdim);
	  TakeEvenCut(child[CUTXL],child[CUTY1L],child[CUTY1H],1);
	  
	  //Y Cut 2
	  child[CUTXH]->Sort(targetdim);
	  TakeEvenCut(child[CUTXH],child[CUTY2L],child[CUTY2H],1);
	  
	  if(dims > 2)
	    {
	      targetdim = root->extfromintern[2];
	      // ZCut 1
	      child[CUTY1L]->Sort(targetdim);
	      TakeEvenCut(child[CUTY1L],child[CUTZ1L],child[CUTZ1H],2);

	      // ZCut 2
	      child[CUTY1H]->Sort(targetdim);
	      TakeEvenCut(child[CUTY1H],child[CUTZ2L],child[CUTZ2H],2);
	      
	      // ZCut 3
	      child[CUTY2L]->Sort(targetdim);
	      TakeEvenCut(child[CUTY2L],child[CUTZ3L],child[CUTZ3H],2);
	      
	      // ZCut 4
	      child[CUTY2H]->Sort(targetdim);
	      TakeEvenCut(child[CUTY2H],child[CUTZ4L],child[CUTZ4H],2);

	    }
	  else
	    {
	      SwapNodes(&child[CUTY1L],&child[CUTZ1L]);
	      SwapNodes(&child[CUTY1H],&child[CUTZ1H]);
	      SwapNodes(&child[CUTY2L],&child[CUTZ2L]);
	      SwapNodes(&child[CUTY2H],&child[CUTZ2H]);

	    }	      
	}
      else
	{
	  SwapNodes(&child[CUTXL],&child[CUTZ1L]);
	  SwapNodes(&child[CUTXH],&child[CUTZ1H]);
	}
       
      // Final children are arranged 0-7; 8-13 are intermediate and can be removed if memory is and issue
      n = 1 << dims;
      for(int i=0;i<n;i++)
	child[i]->SubdivideEvenly(levels - 1);
      
    }
  else
    {
      // Here we have arrived at a leaf.For now just report.
      assert(realnode);
      VerifyContents();
    }

  if(root == this)
    {
      // post init stuff
      NumberLeaves( 0 );
      SetVoxelNums( 0, 0, 0 );
      UpdateNodeVerletCount();
      WhipperUp();
    }

  return 0;
}
int ORBNode::TakeEvenCut(ORBNode *src, ORBNode *destL, ORBNode *destH, int dim)
{

  
  int N;
  int N2 = src->nsites;
  double cut = 0.5 * (src->low[dim] + src->high[dim]);
  N = 0;
  for(int i=0;i<src->nsites;i++)
    {
      Site *t = src->GetSite(i);
      if(t->c[root->extfromintern[ dim]] >= cut)
	break;
      N++;
    }

  for(int i = 0;i < DIMS;i++)
    {
      destL->low[i] = src->low[i];
      destL->high[i] = src->high[i];
      destL->reciprocal[i] = src->reciprocal[i] ;
      destH->low[i] = src->low[i];
      destH->high[i] = src->high[i];
      destH->reciprocal[i] = src->reciprocal[i] ;
    }
  
  destL->SetStart(src->StartSite());
  destL->SetLen(N);	       	  
  destL->CopySites(src,0,N);
  destL->high[dim] = cut;  
  destL->reciprocal[dim] = 1.0/(destL->high[dim]-destL->low[dim]) ;
  
  destL->realnode = 1;
  //  destL->VerifyContents();
  
  destH->SetStart(src->StartSite() + N);
  destH->SetLen(N2-N);
  destH->CopySites(src,N,N2-N);
  destH->low[dim] = cut;
  destH->reciprocal[dim] = 1.0/(destH->high[dim]-destH->low[dim]) ;
  
  destH->realnode = 1;

  //  destH->VerifyContents();
  
  return 0;
}

int ORBNode::FindCutSiteUnsorted(double& cutValue, int dim)
{
	enum {
		k_Buckets = 1024 
	} ;
	double histogram[k_Buckets+1] ; 
	int countHistogram[k_Buckets+1] ;
	for ( int i=0;i<k_Buckets+1;i+=1 )
	{
		histogram[i] = 0.0 ;
		countHistogram[i] = 0 ;
	}
	double lo=GetSite(0)->c[dim] ;
	double hi=lo ;
	int n = nsites ;
	for ( int j0=1;j0<n;j0+=1) 
	{
		double c=GetSite(j0)->c[dim] ;
		lo=fsel(c-lo,lo,c) ;
		hi=fsel(c-hi,c,hi) ;
	}
//	double lo=low[dim] ;
//	double hi=high[dim] ; 
	double bucketWidth=(hi-lo)/k_Buckets ; 
	if ( 0.0 == bucketWidth ) 
	{
		bucketWidth = 1.0 ; 
	}
	double bucketReciprocal=1.0/bucketWidth ;
	BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0)) 
	  << "OrbNode::FindCutSiteUnsorted dim=" << dim 
	  << " lo=" << lo
	  << " hi=" << hi
	  << " n=" << n
	  << EndLogLine ;
	for ( int j=0;j<n;j+=1 )
	{
		double c=GetSite(j)->c[dim] ; 
//		double dbucket=(c-lo)*bucketReciprocal ;
//		double dbucketlim=fsel(dbucket,fsel(dbucket-k_Buckets,k_Buckets,dbucket),0.0) ; // bucket, limited 0<=X<=k_Buckets
//		int bucket=dbucketlim ; 
        int bucket=(c-lo)*bucketReciprocal ;
//		BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0))
//		  << "j=" << j
//		  << " c=" << c
//		  << " bucket=" << bucket
//		  << " verlet=" << GetSite(j)->verlet_count 
//		  << EndLogLine ;
        if ( PKFXLOG_UNSORTED_ORB && (bucket < 0 || bucket > k_Buckets )) 
        {
			BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0))
			  << "j=" << j
			  << " c=" << c
			  << " bucket=" << bucket
			  << " verlet=" << GetSite(j)->verlet_count 
			  << EndLogLine ;
        }
		assert(bucket>=0 && bucket <= k_Buckets) ;
		histogram[bucket] += GetSite(j)->verlet_count ; 
		countHistogram[bucket] += 1 ;
	}
	double cuverlet = histogram[0] ;
	int cucount = countHistogram[0] ;
	double cuhistogram[k_Buckets] ; 
	int cucountHistogram[k_Buckets] ;
	
	for ( int k=0;k<k_Buckets;k+=1)
	{
		cuhistogram[k]=cuverlet ;
		cuverlet += histogram[k+1] ;
		cucountHistogram[k]=cucount ;
		cucount += countHistogram[k+1] ;
		BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0))
		  << "cuhistogram[" << k << "]=" << cuhistogram[k]
		  << " cucountHistogram[" << k << "]=" << cucountHistogram[k]
		  << EndLogLine ;
	} ;
	int base=0 ; 
	int slice=k_Buckets/2 ; 
	while (slice>0)
	{
		if (cuhistogram[base+slice] < cuverlet*0.5 ) base += slice ;
		BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0)) 
		  << "Slicing, now at cuhistogram[" << base
		  << "]=" << cuhistogram[base] 
		  << EndLogLine ;
		slice >>= 1 ;
	}
	cutValue = lo + (base+1)*bucketWidth ; 
	int cutCount = cucountHistogram[base] ;
	BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0)) 
	  << "OrbNode::FindCutSiteUnsorted dim=" << dim 
	  << " lo=" << lo
	  << " hi=" << hi
	  << " base=" << base 
	  << " cuverlet=" << cuverlet
	  << " cucount=" << cucount
	  << " cutValue=" << cutValue
	  << " cutCount=" << cutCount
	  << EndLogLine ;
	return cutCount ;
}

// 'pivot' the ORB around the median
// (There will be some nodes in the 'wrong' halves of the ORB. Swap them to the 'right' halves)
void ORBNode::ArrangeToPartition(int n, double cutValue, int dim)
{
	int n0 = 0 ; 
	int n1 = n ; 
	int n2 = nsites ; 
	BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0) )
	 << "ORBNOde::ArrangeToPartition n=" << n
	 << " n2=" << n2
	 << " cutValue=" << cutValue
	 << " dim=" << dim
	 << EndLogLine ;
	 
	// Find a 'wrong' node in the first half
	while (n0 < n && GetSite(n0)->c[dim] < cutValue )
	{
		n0 += 1 ;
	}
	// Find a 'wrong' node in the second half
	while (n1 < n2 && GetSite(n1)->c[dim] >= cutValue )
	{
		n1 += 1 ;
	}
	while ( n0 < n && n1 < n2)
	{
		// Found a pair to swap. Swap them.
//		BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0))
//		  << "Swapping " << GetSite(n0)->c[dim]
//		  <<  " with " << GetSite(n1)->c[dim]
//		  << EndLogLine ;
		Site * sp0 = GetSite(n0) ; 
		Site * sp1 = GetSite(n1) ; 
		Site s0 = *sp0 ; 
		Site s1 = *sp1 ; 
		*sp1 = s0 ; 
		*sp0 = s1 ; 
		n0 += 1 ;
		n1 += 1 ;
		// Find a 'wrong' node in the first half
		while (n0 < n && GetSite(n0)->c[dim] < cutValue )
		{
			n0 += 1 ;
		}
		// Find a 'wrong' node in the second half
		while (n1 < n2 && GetSite(n1)->c[dim] >= cutValue )
		{
			n1 += 1 ;
		}
	}
	BegLogLine(PKFXLOG_UNSORTED_ORB && (Platform::Topology::GetAddressSpaceId() == 0))
	  << "ORBNOde::ArrangeToPartition ending with n0=" << n0 
	  << " n=" << n 
	  << " n1=" << n1
	  << " n2=" << n2 
	  << EndLogLine ;
	assert(n0 == n) ; 
	assert(n1 == n2) ; 
}

int ORBNode::FindCutSite()
{
  
  if(nsites == 0)
    return 0;
  int N = nsites/2;
  Site *s = GetSite(0);
  if(s->verlet_count == 0.0)
    {
      return N;
    }
  double *ver = VerletSum;
  
  int m;
  for(int i=0;i<nsites;i++)
    {
      ver[i] = GetSite(i)->verlet_count;
      if(i>0)
	{
	  ver[i] += ver[i-1];
	}
    }
  double T = ver[nsites - 1];
  for(int i=0;i<nsites;i++)
    {
      ver[i] /= T;
    }
  m = FindMedian(ver,nsites,N,N);
  return m;      
}
int ORBNode::FindMedian(double *ver, int n, int cur,int window)
{
  double v = ver[cur];
  if(v > 0.5)
    {
      if (window == 1)
	{
	  while( (ver[cur] > 0.5 && cur > 0 ) || (cur < n-1 && ver[cur] == ver[cur+1]))
	    cur--;
	  cur++;
	  //	      printf("median ver[%d] = %f; half = %d\n",cur,ver[cur],n/2);
	  return cur;
	}
      else
	return FindMedian(ver,n,cur - window/2,window/2);
    }
  else if(v <= 0.5)
    {
      if (window == 1)
	{
	  while(ver[cur] <= 0.5 && cur < n-1)
	    cur++;
	  while(cur < n-1 && ver[cur] == ver[cur+1])
	    cur--;
	  //printf("median ver[%d] = %f; half = %d\n",cur,ver[cur],n/2);
	  return cur;
	}
      else
	return FindMedian(ver,n,cur + window/2,window/2);
    }
  return cur;
}
void ORBNode::UpdateVerletCount(double r)
{
  double rr = r*r;
  int vs = 0;
  int nx  = NXProcs();
  int ny  = NYProcs();
  int nz  = NZProcs();
  for(int i = 0;i < nx; i++)
    {
      for(int j = 0;j < ny; j++)
	{
	  for(int k = 0;k < nz; k++)
	    {
	      ORBNode * src = Node(i,j,k);
	      src->verlet_sum = 0;
	      for(int a = 0; a < src->nsites; a++)
		{
		  Site *s = src->GetSite(a);
		  s->verlet_count = VerletCount(s->c,rr);
		  src->verlet_sum += s->verlet_count;
		}
	    }
	  
	}
    }
  UpdateNodeVerletCount();
}


void ORBNode::UpdateVerletCount(double *verletcountlist)
{
#if 0
  int vs = 0;
  int nx  = NXProcs();
  int ny  = NYProcs();
  int nz  = NZProcs();
  for(int i = 0;i < nx; i++)
    {
      for(int j = 0;j < ny; j++)
	{
	  for(int k = 0;k < nz; k++)
	    {
	      ORBNode * src = Node(i,j,k);
	      src->verlet_sum = 0;
	      for(int a = 0; a < src->nsites; a++)
		{
		  Site *s = src->GetSite(a);
		  s->verlet_count = verletcountlist[ s->id ];
		  src->verlet_sum += s->verlet_count;
		}
	    }
	  
	}
    }
  UpdateNodeVerletCount();
#endif
PLATFORM_ABORT("Depricated");
}

void ORBNode::UpdateNodeVerletCount()
{
  int nx  = NXProcs();
  int ny  = NYProcs();
  int nz  = NZProcs();
  for(int i = 0;i < nx; i++)
    {
      for(int j = 0;j < ny; j++)
	{
	  for(int k = 0;k < nz; k++)
	    {
	      ORBNode * src = Node(i,j,k);
	      src->verlet_sum = 0;
	      for(int a = 0; a < src->nsites; a++)
		{
		  Site *s = src->GetSite(a);
		  src->verlet_sum += s->verlet_count;
		}
	    }
	  
	}
    }
  
}

double ORBNode::RealSqrDist(double* coord)
{
  VALIDATE_ROOT 
  double vec[DIMS];
  double d = 0;
  double dp,dpp;
  for (int i = 0;i < DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      if( coord[i] < low[i])
	{
	  dp = low[i] - coord[i];
	  dpp = coord[i] - (high[i] - dV);
	  if(dpp < dp)
	    dp = dpp;
	}
      else if ( coord[i] > high[i] )
	{
	  dp = coord[i] - high[i];
	  dpp = dV + low[i] - coord[i];
	  if(dpp < dp)
	    dp = dpp;
	}
      else
	dp = 0;
      d += dp * dp;	  
    }
  return d;
}
double ORBNode::VerletCount(double *coord, double r)
{

  if(!realnode)
     {PLATFORM_ABORT("NodeSqrDistance between bogus nodes");}
  double n = 0;
  double d = RealSqrDist(coord);
  if(r >= d)
    {
      if(id < 0)
        {
          int nc = 1 << dims;
          for(int i = 0; i < nc; i++)
            {
              n += child[i]->VerletCount(coord,r);
            }
        }
      else
        {
          double dV[DIMS];
          for(int i = 0; i< DIMS;i++)
            dV[i] = root->high[i] - root->low[i];
          for(int i = 0; i< nsites;i++)
            {
              double d = GetSite(i)->SqrDist(coord,dV);
              if(d <= r)
                n += 1.0;
            }
        }
    }
  return n;
}


int ORBNode::CouldMidpointBeMine(ORBNode * node1, ORBNode *node2)
{
  ORBNode *Nlow,*Nhigh;
  int inside;
  double dp,dpp;
  VALIDATE_ROOT 

  for (int i = 0;i < DIMS; i++)
    {
      // Get min dist between low walls of nodes 1 and 2
      double dV = root->high[i] - root->low[i];
      if(node2->low[i] > node1->low[i])   // n1  n2
	{
	  dp = node2->low[i] - node1->low[i];
	  dpp = node1->low[i] - (node2->low[i] - dV);
	  if(dp <= dpp) // no need to image
	    {
	      Nlow = node1; Nhigh = node2;	  //  |  n1  n2  |    
	    }
	  else
	    {
	      Nlow = node2; Nhigh = node1;	  //  n2 |  n1      |    
	      dp = dpp;
	    }
	}
      else                               //  n2 n1
	{
	  dp = node1->low[i] - node2->low[i];
	  dpp = node2->low[i] - (node1->low[i] - dV);
	  if(dp <= dpp) // no need to image
	    {
	      Nlow = node2; Nhigh = node1;	      	  //  |  n2  n1  |    
	    }
	  else
	    {
	      Nlow = node1; Nhigh = node2;	          //  n1 |  n2      |    
	      dp = dpp;
	    }
	}
      // calc high and low midpoint displacements
      double wL = Nlow->high[i] - Nlow->low[i];
      double wH = Nhigh->high[i] - Nhigh->low[i];
      double w = high[i] - low[i];
      double dmL = dp / 2;
      double dmH = (dp + wH + wL) / 2 ;
      double L;

      // check if patch image exists
      // a patch image exists if 
      double pdist = dp + wH;
      double plow = -dV/2.0;
      double phigh = pdist - dV;
      double dpmL = plow / 2.0;
      double dpmH = phigh / 2.0;
      int do_image_patch = ( pdist >= dV / 2.0) ? 1 : 0;


      if(0)
	{
	  if(NODE_ZERO) printf("Low %f High %f dp %f wL %f wH %f w %f dmL %f dmH %f\n",Nlow->low[i], Nhigh->low[i],dp,wL,wH,w,dmL,dmH);

	}
      // Get displacement vector from my low wall to the lowest wall of 1 and 2
      if(low[i] > Nlow->low[i])           //  Nl N  
	{
	  dp = low[i] - Nlow->low[i];
	  dpp = Nlow->low[i] - (low[i] - dV);
	  if(dp <= dpp)                    //     | Nl  N   |
	    {L = dp;}
	  else                            //   N |  Nl     |
	    { L = -dpp;}
	}
      else                               //   N  Nl
	{
	  dp = Nlow->low[i] - low[i];
	  dpp = low[i] - (Nlow->low[i] - dV);
	  if(dp <= dpp)                   //      |  N  Nl   | 
	    {L = -dp;}
	  else                           //   Nl |   N      |
	    {L = dpp;}
	}

      double mH = L + w;
      double mL = L;

      // For low node counts the limits of the node can end up in separate images
      // If it causes the Nl -> H dist to be larger than a unit cell, image down

      if(mH >= dV)
	{ mH -= dV; mL -= dV;}

      if(dmL <= mH && dmH >= mL)
	{inside = 1; }
      else if (do_image_patch && (dpmL <= mH && dpmH >= mL) )
	{
	  inside = 1;
	}
      else
	{inside = 0;}

      if(0)
	{
	  if(NODE_ZERO) printf(" N1: [ %4.1f %4.1f ] N2: [ %4.1f %4.1f ] N: [%4.1f %4.1f ] ( dmL <= mH ){ %4.1f %4.1f } ( dmH >= mL ) { %4.1f %4.1f} inside %d\n",node1->low[i],node1->high[i],node2->low[i],node2->high[i],low[i],high[i],dmL,mH,dmH,mL, inside);
	}
      if(!inside) break;
    }

  return inside;
}


int ORBNode::CouldVoxelMidpointBeMine(int voxId1, int voxId2)
{
  ORBNode N1,N2;
  VALIDATE_ROOT  
  N1.realnode = 1; // Lie to the primatives to avoid code duplication
  N2.realnode = 1;
  N1.root = N2.root = root;
  VoxelBounds(voxId1,N1.low,N1.high);
  VoxelBounds(voxId2,N2.low,N2.high);

  int inside = CouldMidpointBeMine(&N1,&N2);

  return inside;
}

int ORBNode::IsVoxelOnBoundary(int voxId1, double r)
{
  ORBNode N1;
  VALIDATE_ROOT  
  N1.realnode = 1; // Lie to the primatives to avoid code duplication
  N1.root = root;

  VoxelBounds(voxId1,N1.low,N1.high);

  int bounds = IsNodeOnBoundary(&N1,r);

  return bounds;
}

#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)
double ORBNode::MaxVoxelSqrDistance(int voxId1, int voxId2)
{
  ORBNode v1,v2;
  double vec[DIMS];
  VALIDATE_ROOT
  v1.realnode = v2.realnode = 1;
  v1.root = v2.root = root;
  VoxelBounds(voxId1,v1.low,v1.high);
  VoxelBounds(voxId2,v2.low,v2.high);
  v1.NodeMaxDeltas(&v2,vec);
  double r = 0.0;
  for(int i = 0; i< DIMS;i++)r += vec[i]*vec[i];
  return r;
}
#endif

void ORBNode::GetMidpointOfVoxelMidpoints( int voxId1, int voxId2, double* xmid )
{
  VALIDATE_ROOT 
  ORBNode v1,v2;
  double vec1[DIMS],vec2[DIMS],*vl,*vh,mid[DIMS];
  v1.realnode = v2.realnode = 1;
  v1.root = v2.root = root;
  VoxelBounds(voxId1,v1.low,v1.high);
  VoxelBounds(voxId2,v2.low,v2.high);

  for(int i=0;i<DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      vec1[i] = 0.5 * (v1.low[i] + v1.high[i]);
      vec2[i] = 0.5 * (v2.low[i] + v2.high[i]);
      if(vec1[i] > vec2[i])
	{  vl = vec2; vh = vec1; }
      else
	{  vl = vec1; vh = vec2; }
      double dx = vh[i] - vl[i];
      double dxx = vl[i] - (vh[i] - dV);
      if(dx > dxx)
	{
	  vh[i] -= dV;
	}
      mid[i] = 0.5 * (vh[i] + vl[i]);
      if( mid[i] < root->low[i])
	mid[i] += dV;
      if(mid[i] >= root->high[i])
	mid[i] = root->low[i];
    }  
  ExternalFromInternal(mid,xmid);
}

int ORBNode::GetVoxelPointOfVoxelMidpoints( int voxId1, int voxId2, double* vox_pt )
{
  VALIDATE_ROOT 
  ORBNode v1,v2;
  double vec1[DIMS],vec2[DIMS],*vl,*vh, mid[DIMS];
  v1.realnode = v2.realnode = 1;
  v1.root = v2.root = root;
  VoxelBounds(voxId1,v1.low,v1.high);
  VoxelBounds(voxId2,v2.low,v2.high);

  for(int i=0;i<DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
      vec1[i] = 0.5 * (v1.low[i] + v1.high[i]);
      vec2[i] = 0.5 * (v2.low[i] + v2.high[i]);
      if(vec1[i] > vec2[i])
	{  vl = vec2; vh = vec1; }
      else
	{  vl = vec1; vh = vec2; }
      double dx = vh[i] - vl[i];
      double dxx = vl[i] - (vh[i] - dV);
      if(dx > dxx)
	{
	  vh[i] -= dV;
	}
      mid[i] = 0.5 * (vh[i] + vl[i]);
      if( mid[i] < root->low[i])
	mid[i] += dV;
      if(mid[i] >= root->high[i])
	mid[i] = root->low[i];
    }  

  int mid_id = VoxelIdFromInternal( mid );
  VoxelPoint( mid_id, vox_pt );
  return mid_id;
}
 int ORBNode::GetMidpointOfVoxelPoints( int voxId1, int voxId2, double* result, int cacheKnown )
{
  VALIDATE_ROOT 
  double * vl,* vh;
  double vec1[DIMS],vec2[DIMS], mid[DIMS] ;

  VoxelPoint(voxId1,result, cacheKnown);
  InternalFromExternal(result,vec1);

  VoxelPoint(voxId2,result,1);
  InternalFromExternal(result,vec2);
  
  double r[DIMS] ;

  for(int i=0;i<DIMS;i++)
    {
#if 0
    		      double brl = root->low[i] ; 
			      double brh = root->high[i] ;
			      double bvv1 = vec1[i] ; 
			      double bvv2 = vec2[i] ;
			      double bdV = brh-brl;        // root delta
			      
			      double bvv = bvv1 - bvv2 ;
			      double bvl = fsel(bvv,bvv2,bvv1) ; // coordinate lo
			      double bvh = fsel(bvv,bvv1,bvv2) ; // coordinate hi
			      double bdx = bvh - bvl ; // coordinate delta if in same image
			      double bdxx = bvl - bvh + bdV ; // coordinate delta if in different images
			      double bvhh = fsel(bdx-bdxx,bvh-bdV,bvh) ; // Sometihng that can be averaged with bvl
			      
			      double bm = 0.5*(bvhh+bvl) ; // Midpoint
			      double bm1 = fsel(bm-brl,bm,bm+bdV) ; 
			      double bm2 = fsel(bm1-brh,brl,bm1) ;
        mid[i] = bm2 ; 
#else
      double dV = root->high[i] - root->low[i]; // root delta
      if(vec1[i] > vec2[i])
	{  vl = vec2; vh = vec1; }
      else
	{  vl = vec1; vh = vec2; }
      double dx = vh[i] - vl[i]; // coordinate delta if in same image
      double dxx = vl[i] - (vh[i] - dV); // coordinate delta if in different images
      if(dx > dxx)
	{
	  vh[i] -= dV;  // Flip into same image so we can average it
	}
      mid[i] = 0.5 * (vh[i] + vl[i]);
      if( mid[i] < root->low[i])
	mid[i] += dV;
      if(mid[i] >= root->high[i])
	mid[i] = root->low[i];
#endif

//  BegLogLine(0)
//    << "GetMidpointOfVoxelPoints i=" << i
//    << " dxx=" << dxx 
//    << " bdxx=" << bdxx 
//    << " bvl=" << bvl 
//    << " bvh=" << bvh 
//    << " bdV=" << bdV
//    << EndLogLine ;
//    
//  assert( abs(dx - bdx) < 0.001 ) ; 
//  assert( abs(dxx - bdxx) < 0.001 ) ; 
//  assert( abs(mid[i] - bm2) < 0.001 ) ; 
    }  

  ExternalFromInternal(mid,result);
  
  return VoxelId( result );
}
 

int ORBNode::IsVoxelPointOfVoxelPointsMidpointMine(int voxId1, int voxId2)
{
  int inside = 1;
  double mid[ DIMS ], imid[DIMS];
  
  GetMidpointOfVoxelPoints( voxId1, voxId2, mid );
  int vox = VoxelId(mid);
  VoxelPoint(vox,mid);
  InternalFromExternal(mid,imid);

  for(int i=0;i<DIMS;i++)
    {
      if( imid[i] < low[i] || imid[i] >= high[i] )
	{ inside = 0; break; }	
    }
  
  return inside;
}

int ORBNode::IsImagedPointMine(double *vec)
{
  int inside=1;
  double ivec[DIMS];
  InternalFromExternal(vec,ivec);
  for(int i=0;i<DIMS;i++)
    {
    if( ivec[i] < low[i] || ivec[i] >= high[i] )
      { inside = 0; break; } 
    }
  
  return inside;
}

void ORBNode::ImagePoint(double *v, double *imagedv)
{
  VALIDATE_ROOT 
  double h,l;
  double iv[DIMS],ivv[DIMS];
  InternalFromExternal(v,iv);
  for(int i = 0;i<DIMS;i++)
    {
    h = root->high[i];
    l = root->low[i];
    double V = h-l;
    double dv = iv[i] - l;
    while (dv < 0.0) dv += V;
    while (dv >= V) dv -= V;
    ivv[i] = dv;
    }
  ExternalFromInternal(ivv,imagedv);
  return;
}

static int TotalCallsToIsPointInRandomSphereMine = 0;
static int NumberOfMovedAssignement = 0;
static double TotalRadius = 0.0;
static double TotalRadius1 = 0.0;

int ORBNode::IsPointInRandomSphereMine(int voxId1, int voxId2, double aCutoffGuardZone )
  {
  double MinDistanceBetweenVoxelsSrt = MinVoxelSqrDistance( voxId1, voxId2 );
    
  // Ouch!!!
  double MinDistanceBetweenVoxels    = sqrt( MinDistanceBetweenVoxelsSrt );
  double RadiusOfRandomSphere        = 0.5 * ( aCutoffGuardZone - MinDistanceBetweenVoxels );
  
  double FUDGE_ZONE = 10.0;
  double Radius = min( FUDGE_ZONE, RadiusOfRandomSphere );
  
  if( Radius <= 0.0 )
    {
    return 0;
    }

  double midAB[ DIMS ];  
  double midABTemp[ DIMS ];  
  GetMidpointOfVoxelPoints( voxId1, voxId2, midAB );
  
  midABTemp[ 0 ] = midAB[ 0 ];
  midABTemp[ 1 ] = midAB[ 1 ];
  midABTemp[ 2 ] = midAB[ 2 ];

  double RandomBox[ DIMS ];
  srand( voxId1 ^ voxId2 );
  RandomBox[ 0 ] = Radius * (2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0 );
  RandomBox[ 1 ] = Radius * (2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0 );
  RandomBox[ 2 ] = Radius * (2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0 );
  
  double Length = sqrt( RandomBox[ 0 ] * RandomBox[ 0 ] + 
                        RandomBox[ 1 ] * RandomBox[ 1 ] + 
                        RandomBox[ 2 ] * RandomBox[ 2 ] );

  double RandomNormlizedBox[ DIMS ];

  double RandNum = (rand() / (RAND_MAX + 1.0));

   RandomNormlizedBox[ 0 ] = (RandomBox[ 0 ] / Length) * Radius * RandNum;
   RandomNormlizedBox[ 1 ] = (RandomBox[ 1 ] / Length) * Radius * RandNum;
   RandomNormlizedBox[ 2 ] = (RandomBox[ 2 ] / Length) * Radius * RandNum;

//  RandomNormlizedBox[ 0 ] = (RandomBox[ 0 ] / Length) * Radius;
//  RandomNormlizedBox[ 1 ] = (RandomBox[ 1 ] / Length) * Radius;
//  RandomNormlizedBox[ 2 ] = (RandomBox[ 2 ] / Length) * Radius;
  
  midAB[ 0 ] += RandomNormlizedBox[ 0 ];
  midAB[ 1 ] += RandomNormlizedBox[ 1 ];
  midAB[ 2 ] += RandomNormlizedBox[ 2 ];

  double midABImg[ DIMS ];
  ImagePoint( midAB, midABImg );

  int rc = IsImagedPointMine( midABImg );

#if 0
  BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 && voxId1 == 0 )
    << "aCutoffGuardZone: " << aCutoffGuardZone
    << " Radius: " << Radius
    << " RandomBox: { " 
    << RandomBox[ 0 ] << " "
    << RandomBox[ 1 ] << " "
    << RandomBox[ 2 ] << " } "
    << " RandomNormlizedBox: { "
    << RandomNormlizedBox[ 0 ] << " "
    << RandomNormlizedBox[ 1 ] << " "
    << RandomNormlizedBox[ 2 ] << " } "
    << " midABImg: { "
    << midABImg[ 0 ] << " "
    << midABImg[ 1 ] << " "
    << midABImg[ 2 ] << " } "
    << EndLogLine;
#endif
  
#if 0
  // If the voxel pair is assigned here, try to get it out by adding 180 degrees to the random vector
  if( rc )
    {
    midAB[ 0 ] -= 2.0 * RandomNormlizedBox[ 0 ];
    midAB[ 1 ] -= 2.0 * RandomNormlizedBox[ 1 ];
    midAB[ 2 ] -= 2.0 * RandomNormlizedBox[ 2 ];
    
    ImagePoint( midAB, midABImg );
    
    rc = IsImagedPointMine( midABImg );
    }
#endif

  if( IsImagedPointMine( midABTemp ) )
    {
    TotalRadius += Radius;
    TotalCallsToIsPointInRandomSphereMine++;
    }

  if( IsImagedPointMine( midABTemp ) && !rc )
    {
    // Originally this pair would be assigned here, but it got assigned somewhere else.
    NumberOfMovedAssignement++;
    }

  return rc;
}

int ORBNode::IsMidpointOfVoxelMidpointsMine(int voxId1, int voxId2)
{
  int inside = 1;
  double mid[ DIMS ],imid[DIMS];
  
  GetMidpointOfVoxelMidpoints( voxId1, voxId2, mid );
  InternalFromExternal(mid,imid);

  for(int i=0;i<DIMS;i++)
    {
      if( imid[i] < low[i] || imid[i] >= high[i] )
	{ inside = 0; break; }	
    }
  
  return inside;
}
int ORBNode::IsMidpointOfVoxelPointsMine(int voxId1, int voxId2, int cacheKnown)
{
  double mid[ DIMS ],imid[DIMS];;
  
  GetMidpointOfVoxelPoints( voxId1, voxId2, mid, cacheKnown );
  InternalFromExternal(mid,imid);

  if ( 3 == DIMS )
  {
  	 double n0l = imid[0]-low[0] ;
  	 double n0h = imid[0]-high[0] ;
  	 double n0 = fsel(n0l,fsel(n0h,0.0, 1.0), 0.0) ;
     
  	 double n1l = imid[1]-low[1] ;
  	 double n1h = imid[1]-high[1] ;
  	 double n1 = fsel(n1l,fsel(n1h,0.0, 1.0), 0.0) ;
     
  	 double n2l = imid[2]-low[2] ;
  	 double n2h = imid[2]-high[2] ;
  	 double n2 = fsel(n2l,fsel(n2h,0.0, 1.0), 0.0) ;
     
     return n0*n1*n2 ;
  } else {
  	  int inside = 1;
  	
	  for(int i=0;i<DIMS;i++)
	    {
	      if(!( imid[i] >= low[i] && imid[i] < high[i]) )
		  { inside = 0; break; }	
	    }  
    return inside;
    
  }
  
}

#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)
void ORBNode::IsMidpointOfVoxelPointsMineVector(char* resultVector, int voxId1, int * voxId2Vector, int count)
{
	if ( count > 0 )
	{
		resultVector[0]=IsMidpointOfVoxelPointsMine(voxId1,voxId2Vector[0], 0) ;
		for (int j=1;j<count;j+=1)
		{
			resultVector[j]=IsMidpointOfVoxelPointsMine(voxId1,voxId2Vector[j], 1 ) ;
		}
	}
}
#endif

int ORBNode::IsVoxelPointOfVoxelMidpointsMine(int voxId1, int voxId2)
{
  int inside = 1;
  double vox_pt[ DIMS ], imid[DIMS];
  
  GetVoxelPointOfVoxelMidpoints( voxId1, voxId2, vox_pt );
  InternalFromExternal(vox_pt,imid);

  for(int i=0;i<DIMS;i++)
    {
      if( imid[i] < low[i] || imid[i] >= high[i] )
	{ inside = 0; break; }	
    }
  
  return inside;
}

double ORBNode::MinVoxelSqrDistance(int voxId1, int voxId2)
{
  ORBNode v1,v2;
  double vec[DIMS];
  v1.realnode = v2.realnode = 1;
  v1.root = v2.root = root;
  VoxelBounds(voxId1,v1.low,v1.high);
  VoxelBounds(voxId2,v2.low,v2.high);
  v1.NodeMinDeltas(&v2,vec);
  double r = 0.0;
  for(int i = 0; i< DIMS;i++)r += vec[i]*vec[i];
  return r;
}

int ORBNode::IsNodeOnBoundary(ORBNode * nd, double r)
{
  double min,max;
  NodeBoundaryDistances(nd,min,max);
  if ( (r >= min)  && (r <= max) ) 
    return 1;
  else 
    return 0;
}

void ORBNode::ReportVoxel( int id )
{
  double vlow[3];
  double vhigh[3];

  VoxelBounds(id,vlow,vhigh);

  BegLogLine( 1 )
    << "VoxelId: " << id
    << " vlow " 
    << vlow[ 0 ] << " "
    << vlow[ 1 ] << " "
    << vlow[ 2 ] << " "
    << " vhigh " 
    << vhigh[ 0 ] << " "
    << vhigh[ 1 ] << " "
    << vhigh[ 2 ] 
    << EndLogLine;
}

int ORBNode::AllocateLoadZones()
{
  assert( this == root);
  VALIDATE_ROOT 
  double nvox = voxnum[0] * voxnum[1] * voxnum[2];
  int nx = NXProcs();
  int ny = NYProcs();
  int nz = NZProcs();
  double nnodes = NXProcs() * NYProcs() * NZProcs();

  int nMaxVoxPerNode = 100.0 * nvox/nnodes;
  for(int i = 0; i < nx; i++)
    {
      for(int j = 0; j < ny; j++)
        {
          for(int k = 0; k < nz; k++)
            {
              ORBNode *nd = Node(i,j,k);
              if(nd->LoadZone != NULL)
                assert(0);
              nd->LoadZone = new int[nMaxVoxPerNode];

              if( nd->LoadZone == NULL )
                PLATFORM_ABORT( "nd->LoadZone == NULL" );

              nd->nLoadVox = 0;
              nd->MaxLoadVox = nMaxVoxPerNode;
            }
        }
    }
  return 1;
}

int ORBNode::AllocateMyLoadZones( int i, int j, int k )
{
  assert( this == root );
  VALIDATE_ROOT 
  double nvox = voxnum[0] * voxnum[1] * voxnum[2];
  double nnodes = NXProcs() * NYProcs() * NZProcs();

  int nMaxVoxPerNode = 100.0 * nvox/nnodes;
  
  ORBNode *nd = NodeFromProcCoord( i, j, k);
  if(nd->LoadZone != NULL)
    PLATFORM_ABORT( "nd->LoadZone != NULL" );
  
  nd->LoadZone = new int[ nMaxVoxPerNode ];
  
  if( nd->LoadZone == NULL )
    PLATFORM_ABORT( "nd->LoadZone == NULL" );
  
  nd->nLoadVox = 0;
  nd->MaxLoadVox = nMaxVoxPerNode;
  
  return 1;
}

void ORBNode::WhipperUp()
  {
#ifndef _ORB_STANDALONE_
  if(NAllocatedSites > 0)
    {
    delete []SiteArray;
    delete []VerletSum;
    NAllocatedSites = NSites = 0;
    SiteArray = NULL;
    VerletSum = NULL;
    }
#endif
  }

int ORBNode::FreeLoadZones()
{
  assert( this == root);
  VALIDATE_ROOT 
  int nx = NXProcs();
  int ny = NYProcs();
  int nz = NZProcs();

  for(int i = 0; i < nx; i++)
    {
      for(int j = 0; j < ny; j++)
	{
	  for(int k = 0; k < nz; k++)
	    {
	      ORBNode *nd = Node(i,j,k);
	      if(nd->LoadZone != NULL)
		delete [] nd->LoadZone;
	      nd->LoadZone = NULL;
	      nd->nLoadVox = 0;
	      nd->MaxLoadVox = 0;
	    }
	}
    }
  return 1;
}

int ORBNode::FreeMyLoadZones( int i, int j, int k )
{
    ORBNode *nd = NodeFromProcCoord( i, j, k );

    if(nd->LoadZone != NULL)
        delete [] nd->LoadZone;

    nd->LoadZone = NULL;
    nd->nLoadVox = 0;
    nd->MaxLoadVox = 0;

    return 1;
}

int ORBNode::DeriveAllLoadZones()
  {
  assert( this == root);
  VALIDATE_ROOT 
  FreeLoadZones();
  AllocateLoadZones();

  int nx = NXProcs();
  int ny = NYProcs();
  int nz = NZProcs();

  for(int i = 0; i < nx; i++)
    {
    for(int j = 0; j < ny; j++)
      {
      for(int k = 0; k < nz; k++)
        {
        ORBNode *nd = Node( i, j, k );
        nd->DeriveMyLoadZone();
        }
      }
    }  
  return 1;
  }

int ORBNode::DeriveLoadZones( int i, int j, int k )
{
  assert( this == root);
  VALIDATE_ROOT 
  FreeMyLoadZones( i, j, k );
  AllocateMyLoadZones( i, j, k );
  
  ORBNode *nd = NodeFromProcCoord( i, j, k );
  nd->DeriveMyLoadZone();

  return 1;
}

int ORBNode::DeriveMyLoadZone()
{
  VALIDATE_ROOT 
  double dr[DIMS];
  double vlow[DIMS],vhigh[DIMS];
  int vnlow[DIMS],vnhigh[DIMS];
  ORBNode n1;
  n1.realnode = 1;
  n1.root = root;

  VoxelRange(low,high,vnlow,vnhigh);

  //printf(">%d vnlow %d %d %d vnhigh %d %d %d\n",id,vnlow[0],vnlow[1],vnlow[2],vnhigh[0],vnhigh[1],vnhigh[2]);

  int ni[DIMS];
  for(ni[0] = vnlow[0]; ni[0] <= vnhigh[0] ; ni[0]++)
    {
      for(ni[1] = vnlow[1]; ni[1] <= vnhigh[1] ; ni[1]++)
        {
          for(ni[2] = vnlow[2]; ni[2] <= vnhigh[2] ; ni[2]++)
            {
              double vd[DIMS];
              int Id = VoxelId(ni);
              VoxelPoint(Id,vd);
              ORBNode *tnd = root->NodeFromSimCoord(vd[0], vd[1], vd[2]);
              if(tnd == this)
                {
                  LoadZone[nLoadVox] = Id;
                  nLoadVox++;

                  if( nLoadVox >= MaxLoadVox )
                    {
                      BegLogLine( 1 )
                        << "nLoadVox= " << nLoadVox
                        << " MaxLoadVox= " << MaxLoadVox
                        << " tnd->low { " << tnd->low[0] << " " << tnd->low[1] << " " << tnd->low[2] << " }"
                        << " tnd->high { " << tnd->high[0] << " " << tnd->high[1] << " " << tnd->high[2] << " }"
                        << " vnlow: { " << vnlow[ 0 ] << " " << vnlow[ 1 ] << " " << vnlow[ 2 ] << " }"
                        << " vnhigh: { " << vnhigh[ 0 ] << " " << vnhigh[ 1 ] << " " << vnhigh[ 2 ] << " }"
                        << " vd { " << vd[ 0 ] << " " << vd[ 1 ] << " " << vd[ 2 ] << " }"
                        << EndLogLine;
                    }

                  assert(nLoadVox < MaxLoadVox);
                }
            }
        }
    }
  ReduceIntArray(LoadZone,nLoadVox);
  int *surflist = new int[nLoadVox];
  int nsurfs = 0;

  // surfvox is any vox with an exposed face
  for(int i = 0;i<nLoadVox;i++)
    {
      int cvox = LoadZone[i];
      VoxelCoords(cvox,ni);
      int voxid;
      int surfset = 0;
      for(int d = 0;d < DIMS; d++)
	{
	  ni[d] -= 1;
	  voxid = VoxelId(ni);
	  if(!InLoadZone(voxid))
	    {  surfset = 1;     break;   }
	  ni[d] += 2;
	  voxid = VoxelId(ni);
	  if(!InLoadZone(voxid))
	    {  surfset = 1;     break;   }
	  ni[d] -= 1;
	}
      if(surfset)
	{
	  surflist[nsurfs] = cvox;
	  nsurfs++;
	}
    }
  for(int i = 0;i<nsurfs;i++)
    {
      LoadZone[i] = surflist[i];
    }

  BegLogLine( 0 )
      << "nLoadVox: "
      << nLoadVox
      << " nsurfs: "
      << nsurfs
      << EndLogLine;

  nLoadVox = nsurfs;
  return nLoadVox;
}

int ORBNode::InLoadZone(int voxid)
{
  for(int i = 0 ;i< nLoadVox; i++)
    {
      if(voxid == LoadZone[i])
	return 1;
    }
  return 0;
}

#if defined(ORB_COMPILE) || !defined(ORB_SEPARATE)

int ORBNode::RanksInLoadZoneNeighborhood(double r,int *ranks,int &len,int max  )
{
  VALIDATE_ROOT 
  len = 0;
  int nvox = 0;
  int NVox = root->voxnum[0] *root->voxnum[1] *root->voxnum[2];
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  int *buffer = new int[NVox];
  assert(max >= NVox);

  if( buffer == NULL )
    PLATFORM_ABORT( "buffer == NULL" );
  InitSet(buffer,NVox);
  ORBNode nd;
  nd.realnode = 1;
  nd.root = root;
  
  BegLogLine( 0 )
      << "nLoadVox: "
      << nLoadVox
      << EndLogLine;

  if( !nLoadVox )
    PLATFORM_ABORT( "No load zones, make neighborhood calls dull" );  

  for(int i=0;i<nLoadVox;i++)
    {
      VoxelBounds(LoadZone[i],nd.low,nd.high);      
      nd.OrVoxelNeighborSet(r,buffer,nvox,NVox);
    }
  CollapseIntSet(buffer,nvox,NVox);
  RanksFromVoxels( buffer, nvox, ranks, len, NNodes );
  CollapseIntSet(ranks,len,NNodes);

  delete [] buffer;
  return len;
}


int ORBNode::RanksInLoadZoneNeighborhoodVersion2(double r,int *ranks,int &len,int max  )
{
  // Return the list of homezones within R from this loadzone
  VALIDATE_ROOT 
  len = 0;
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();

  InitSet(ranks,NNodes);

  ORBNode nd;
  nd.realnode = 1;
  nd.root = this->root;
  
  BegLogLine( 0 )
      << "nLoadVox: "
      << nLoadVox
      << EndLogLine;

  if( !nLoadVox )
    PLATFORM_ABORT( "No load zones, make neighborhood calls dull" );  

  double rr = r*r;

  for(int i=0;i<nLoadVox;i++)
    {
      VoxelBounds(LoadZone[i],nd.low,nd.high);      
      root->AddRanks(&nd,rr,ranks,len,max);
    }
  // take out self node
#ifdef _ORB_STANDALONE_
  ranks[id] = -1;
#else
  ranks[rank] = -1;
#endif
  CollapseIntSet(ranks,len,NNodes);

  return len;
}
#endif

void ORBNode::InitSet(int *set, int max)
{
  for(int i = 0;i<max;i++)
    set[i] = -1;
}

void ORBNode::CollapseIntSet(int *set, int &len, int max)
{
  len = 0;
  for(int i = 0;i<max;i++)
  {
    if(set[i] > -1)
      {
	set[len] = set[i];
	if(i > len)
	  set[i] = -1;
	len++;
      }
  }
  return;
}

void ORBNode::ReduceIntArray(int *ranks, int &len)
{
  SortRanks(ranks,len);
  int i = 0;
  int p = 0;
  while( p < len)
    {
      if(ranks[p] < 0)
	p++;
      else
	{
	  ranks[i] = ranks[p];
	  while(ranks[i] == ranks[p])
	    p++;
	  i++;
	}
    }
  len = i;
}

void ORBNode::WriteOrb(char *streamPS, int& cursor)
{
  streamPS[cursor] = '\0';
  char bufS[1024];
  sprintf(bufS,"{\n"); 
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);

  sprintf(bufS,"\t %d %d %d\n", id,rank,dims); 
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);


  sprintf(bufS,"\t %lf %lf %lf\n", low[0],low[1],low[2]); 
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);
  sprintf(bufS,"\t %lf %lf %lf\n", high[0],high[1],high[2]); 
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);
  sprintf(bufS,"\t %d %d %d\n", meshnum[0],meshnum[1],meshnum[2]); 
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);
  sprintf(bufS,"\t %d %d %d\n", voxnum[0],voxnum[1],voxnum[2]); 
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);

  sprintf(bufS,"\t %d \n", nLoadVox);
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);
  for(int i = 0;i<nLoadVox;i++)
    {
      sprintf(bufS,"\t %8x \n", LoadZone[i]);
      strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);
    }

  sprintf(bufS,"\t %d \n", NCHILDREN);
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);
  for(int i = 0;i < NCHILDREN; i++)
    {
      if(child[i] != NULL)
	{
	  child[i]->WriteOrb(streamPS,cursor);
	}
      else
	{
	  sprintf(bufS,"\t NULL \n");
	  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);
	}
    }

  sprintf(bufS,"}\n"); 
  strcat(&streamPS[cursor],bufS); cursor += strlen(bufS);

}

void ORBNode::ExternalFromInternal(double *intrn,double *extrn)
{
  VALIDATE_ROOT 
  int mappings[DIMS] ;
  {
  	for ( int i=0;i<DIMS;i+=1 )
  	{
  		mappings[i] = root->extfromintern[ i ] ;
  	}
  }
  double intrns[DIMS] ;
  {
  	for ( int i=0;i<DIMS;i+=1)
  	{
  		intrns[i] = intrn[i] ;
  	}
  }
  {
	  for(int i = 0;i < DIMS; i++)
	    {
	      extrn[ mappings[i] ] = intrns[ i ];
	    }
  }
}
void ORBNode::InternalFromExternal(double *extrn,double *intrn)
{
  VALIDATE_ROOT 
  int mappings[DIMS] ;
  {
  	for ( int i=0;i<DIMS;i+=1 )
  	{
  		mappings[i] = root->internfromext[ i ] ;
  	}
  }
  double extrns[DIMS] ;
  {
  	for ( int i=0;i<DIMS;i+=1)
  	{
  		extrns[i] = extrn[i] ;
  	}
  }
  
  {
	  for(int i = 0;i < DIMS; i++)
	    {
	      intrn[ mappings[i] ] = extrns[ i ];
	    }
  }
}
