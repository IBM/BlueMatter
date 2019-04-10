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
 
#define _ORB_STANDALONE_
#include "../include/orbnode.hpp"

#define SNAP_TO_GRID 0
  
#include <stdio.h>

ORBNode::ORBNode()
{
  id = 0;
  realnode = 0;
  rank = -1;
  double verlet_sum = 0.0;
  for(int i=0;i<DIMS;i++)
    {
      low[i]=high[i]=0.0;
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
#ifndef TEMPERALPHA
#define TEMPERALPHA 0.0
#endif
  mAlphaForWorkVolMix = TEMPERALPHA;
}

ORBNode::~ORBNode()
{
  if(root == this)
    {
      if(NAllocatedSites > 0)
	{
	  delete []SiteArray;
	  delete []VerletSum;
	  NAllocatedSites = NSites = 0;	  
	}
      FreeLoadZones();
    }
  SiteArray = NULL;
  VerletSum = NULL;
  for(int i = 0;i<NCHILDREN;i++)
    {
     ORBNode * t = child[i];
     if(t != NULL)
       {t->~ORBNode(); child[i] = NULL;}
     
    }
  root = NULL;
}

void ORBNode::SetWorkVolMixingParam(double alpha)
{
  mAlphaForWorkVolMix = alpha;
}

void ORBNode::SetProcDims(int NX, int NY, int NZ)
{
  assert(root == this);

  int l,m,s;
  int n = NX*NY*NZ;
  // int p = NXProcs() * NYProcs() * NZProcs();
  //  assert(n == p);
  int nprocs[DIMS];
  nprocs[0] = NX;
  nprocs[1] = NY;
  nprocs[2] = NZ;

  int swap;
  double C[DIMS];

  for(int i = 0; i < DIMS; i++)
    {
      C[i] = root->high[i] - root->low[i];
    }

  do
    {
      swap = 0;
      if ( C[ extfromintern[ 0 ] ] < C[ extfromintern[ 1 ] ])
	{
	  swap = extfromintern[ 1 ];
	  extfromintern[ 1 ] = extfromintern[ 0 ];
	  extfromintern[ 0 ] = swap;
	  swap = 1;	  
	}
      if ( C[ extfromintern[ 1 ] ] < C[ extfromintern[ 2 ] ])
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
      // procnums are in external order
      procnum[i] = nprocs[ i ];
    }

  assert(procnum[ extfromintern[0] ] >= procnum[ extfromintern[ 1 ] ]);
  assert(procnum[ extfromintern[0] ] >= procnum[ extfromintern[ 2 ] ]);
  assert(procnum[ extfromintern[1] ] >= procnum[ extfromintern[ 2 ] ]);

  //
  //Example  :  called with 8 8 16
  // internfromext: [ 1 2 0 ]
  // extfromintern: [ 2 0 1 ]
  //


}

void ORBNode::Adopt(ORBNode *child)
{
  child->root = root;
  child->parent = this;
  child->SiteArray = SiteArray;
  child->VerletSum = VerletSum;
  child->NSites = NSites;
  child->NAllocatedSites = NAllocatedSites;  
  child->mAlphaForWorkVolMix = mAlphaForWorkVolMix;
}

void ORBNode::SetSite( int i, XYZ aSite, double weight )
{
  Site * t = GetSite(i);
  t->id = i;
  t->verlet_count = weight;
  t->c[0] = aSite.mX;
  t->c[1] = aSite.mY;
  t->c[2] = aSite.mZ;
}

void ORBNode::SetSite( int i, XYZ aSite, double weight, FragId f1, FragId f2 )
  {
  Site * t = GetSite(i);
  t->id = i;
  t->verlet_count = weight;
  t->c[0] = aSite.mX;
  t->c[1] = aSite.mY;
  t->c[2] = aSite.mZ;
  
  t->fA = f1;
  t->fB = f2;
  }

#if 0
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
#endif


Site * ORBNode::GetSite(int i)
{
  int ind = start + i;  
  assert( ind>=0 && ind < NSites);
  Site *t = &SiteArray[ind];
  return t;
}

int ORBNode::StartSite()
{
  return start;
}

void ORBNode::SetLen(int n)
{
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

  return 1;
}

void ORBNode::SortRanks(int *ranks,int len)
    {
      qsort(ranks,len,sizeof(int),CompareInt);
    }
void ORBNode::SortDim(int d)
    {
      switch(d)
	{
	case 0:
	  qsort(&SiteArray[start],nsites,sizeof(Site),CompareX);
	  break;
	case 1:
	  qsort(&SiteArray[start],nsites,sizeof(Site),CompareY);
	  break;
	case 2:
	  qsort(&SiteArray[start],nsites,sizeof(Site),CompareZ);
	  break;
	default:
	  assert(0);
	}
    }
void ORBNode::SortX()
    {
      qsort(&SiteArray[start],nsites,sizeof(Site),CompareX);
    }
void ORBNode::SortY()
    {
      qsort(&SiteArray[start],nsites,sizeof(Site),CompareY);
    }
void ORBNode::SortZ()
    {
      qsort(&SiteArray[start],nsites,sizeof(Site),CompareZ);
    }
  
void  ORBNode::SetRange(int dim, double l, double h)
{
  low[dim] = l; high[dim] = h;
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
  return root->procnum[0];
}
int  ORBNode::NYProcs() 
{
//    int nx = 1 << (-id); 
//   int n = (dims > 1) ? nx : nx >> 1; 
//   return n;
  return root->procnum[1];
}
int  ORBNode::NZProcs() 
{
  
//   int nx = 1 << (-id); 
//   int n = (dims > 2) ? nx : nx >> 1; 
//   return n;

  return root->procnum[2];

}

//inline 
VoxelIndex ORBNode::VoxelIdToVoxelIndex( int aVoxelId )
{
//   11 1111 1111
//   3  F    F 

  int mask = (1<<10) -1;
  unsigned int Last10Bits = mask;
  int vzCoord = aVoxelId & Last10Bits;
  int vyCoord = ( aVoxelId >> 10 ) & Last10Bits;
  int vxCoord = ( aVoxelId >> 20 ) & Last10Bits;        

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

int ORBNode::VoxelId(double *v)
{
  double h,l;
  int Id = 0;
  for(int i = 0;i<DIMS;i++)
    {
      Id <<= 10;
      h = root->high[i];
      l = root->low[i];
      double dv = (v[i] - l) / (h - l);
      while (dv < 0.0) dv += 1.0;
      while (dv >= 1.0) dv -= 1.0;
      int N =  dv * root->voxnum[i];
      assert(N < 1<<10);
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

inline
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
  double h,l;
  int Id = 0;
  for(int i = 0;i<DIMS;i++)
    {
      Id <<= 10;
      int n = v[i];
      while (n < 0) n += root->voxnum[i];
      while (n >= root->voxnum[i]) n -= root->voxnum[i];
      assert (n >=0);
      assert(n < 1<<10);
      Id |= n;
    }
  return Id;
}
int ORBNode::VoxelBounds(int Id , double *vlow, double *vhigh)
{
  int mask = (1<<10) -1;
  int n[DIMS];
  n[2] = Id & mask;
  mask <<= 10;
  n[1] = (Id & mask) >> 10;
  mask <<= 10;
  n[0] = (Id & mask) >> 20;
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
  int mask = (1<<10) -1;
  n[2] = Id & mask;
  mask <<= 10;
  n[1] = (Id & mask) >> 10;
  mask <<= 10;
  n[0] = (Id & mask) >> 20;
  return;
}

void ORBNode::CalcVoxelPoint(int aVoxelId, double *p)
{
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
      p[i] = L[i] + 0.95 * delta[i] + 0.01 ;
    }
 
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

void ORBNode::VoxelPoint(int aVoxelId, double *p)
{
#if 1
  typedef double tPoint[3];
  static tPoint * CachedVoxelPoints = NULL;                                                                                                                                                                                    
  // THIS IS NOT THREAD SAFE 
  if( CachedVoxelPoints == NULL )  
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
	//	PLATFORM_ABORT("CachedVoxelPoints == NULL"); 
	assert(	0 );
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

double ORBNode::SnapToGrid(double cut, int d)
{
  double H = root->high[d];
  double L = root->low[d];
  double D = H - L;
  double dV = D / root->voxnum[d];
  double C = (cut - L) / D;
  int N = C * root->voxnum[d];
  double dl = N * dV + L;
  double dh = (N+1) * dV + L;
  double c = ((dh - C) <= (C - dl)) ? dh : dl;
  if ( c > H ) 
    c = H;
  if ( c < L ) 
    c = L;
  return c;
  
}
double ORBNode::SnapLow(double cut, int d)
{
  double H = root->high[d];
  double L = root->low[d];
  double D = H - L;
  double dV = D / root->voxnum[d];      
  double C = (cut - L) / D;
  int N = C * root->voxnum[d];
  double CL = N * dV + L;
  return CL;
}
double ORBNode::SnapHigh(double cut, int d)
{
  double H = root->high[d];
  double L = root->low[d];
  double D = H - L;
  double dV = D / root->voxnum[d];      
  double C = (cut - L) / D;
  int N = C * root->voxnum[d];
  double CH = (N+1) * dV + L;
  return CH;
}
void ORBNode::xyzFromProcCount(int procs, int &x, int &y, int &z)
{
  // Find full space power of 2
  
  int d = 3;
  int levels = 0;
  int t;
  do
    { 
      levels++;
      t = 1 << (levels*d);
    }while (procs > t);
  int base = (levels-1)*d;
  while(d > 0)
    { 
      t = 1 << (base + d);
      if(procs == t) break;
      d--;
    }
  assert(d>0);
  switch(d)
    {
    case 3:
      x = 1 << levels;
      y = 1 << levels;
      z = 1 << levels;
      break;
    case 2:
      x = 1 << levels;
      y = 1 << levels;
      z = 1 << (levels - 1);
      break;
    case 1:
      x = 1 << levels;
      y = 1 << (levels - 1);
      z = 1 << (levels - 1);
      break;
    }  
}

void ORBNode::VerifyContents()
{
  for(int a=0;a<nsites;a++)
    {
      Site *s = GetSite(a);
      for(int i=0;i<DIMS;i++)
	{
	  if(s->c[i] < low[i]){printf("dim %d for site %d is too low! %f s= %f\n", i,a,low[i], s->c[i]); s->Print(); assert(0);} 
	  if(s->c[i] > high[i]){printf("dim %d for site %d is too high! %f s= %f\n",i,a, high[i], s->c[i]); s->Print(); assert(0);} 
	}
    }
}
int ORBNode::SetVoxelNums(int dx,int dy, int dz)
{
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
//       nd[ root->extfromintern[0] ] = dx;
//       nd[ root->extfromintern[1] ] = dy;
//       nd[ root->extfromintern[2] ] = dz;

      nd[ 0 ] = dx;
      nd[ 1 ] = dy;
      nd[ 2 ] = dz;
      //      printf("!!!!!!!!!!!!!!!   MakeRank %d %d %d\n",nd[0],nd[1],nd[2]);
      
      rank = id ; //Platform::Topology::MakeRankFromCoords( nd[ 0 ],nd[ 1 ],nd[ 2 ]);

    }
  return id;
}

ORBNode * ORBNode::NodeFromProcCoord(int x,int y, int z) 
{
  int nd[DIMS];
//   nd[ root->internfromext[0] ] = x;
//   nd[ root->internfromext[1] ] = y;
//   nd[ root->internfromext[2] ] = z;

  nd[ 0 ] = x;
  nd[ 1 ] = y;
  nd[ 2 ] = z;

  return Node(nd[ 0 ],nd[ 1 ],nd[ 2 ]);
}
ORBNode * ORBNode::Node(int x,int y, int z) 
{

  if(id < 0)
    {
      int IgX[3],XgI[3];
      IgX[ root->internfromext[ 0 ] ] = x; 
      IgX[ root->internfromext[ 1 ] ] = y; 
      IgX[ root->internfromext[ 2 ] ] = z; 

      int level = -id;
      int half = 1 << (level-1);
      int xx,yy,zz;
      xx = IgX[0]; yy = IgX[1]; zz = IgX[2];
      
      if(dims == 3)
	{
	  int rx = xx % half;
	  int ry = yy % half;
	  int rz = zz % half;
	  XgI[ root->extfromintern[ 0 ] ] = rx;
	  XgI[ root->extfromintern[ 1 ] ] = ry;
	  XgI[ root->extfromintern[ 2 ] ] = rz;

	  if(xx < half)
	    {
	      if(yy < half)
		{
		  if(zz < half)
		    {   return child[CUTZ1L]->Node(XgI[0],XgI[1],XgI[2]);    }
		  else
		    {   return child[CUTZ1H]->Node(XgI[0],XgI[1],XgI[2]);    }
		}
	      else 
		{
		  if(zz < half)
		    {   return child[CUTZ2L]->Node(XgI[0],XgI[1],XgI[2]);    }
		  else
		    {   return child[CUTZ2H]->Node(XgI[0],XgI[1],XgI[2]);    }
		}
	    }
	  else
	    {
	      if(yy < half)
		{
		  if(zz < half)
		    {   return child[CUTZ3L]->Node(XgI[0],XgI[1],XgI[2]);    }
		  else
		    {   return child[CUTZ3H]->Node(XgI[0],XgI[1],XgI[2]);    }
		}
	      else
		{
		  if(zz < half)
		    {   return child[CUTZ4L]->Node(XgI[0],XgI[1],XgI[2]);    }
		  else
		    {   return child[CUTZ4H]->Node(XgI[0],XgI[1],XgI[2]);    }
		}
	    }
	}
      else if (dims == 2)
	{
	  int rx = xx % half;
	  int ry = yy % half;
	  int rz = zz;
	  XgI[ root->extfromintern[ 0 ] ] = rx;
	  XgI[ root->extfromintern[ 1 ] ] = ry;
	  XgI[ root->extfromintern[ 2 ] ] = rz;
	  if(xx < half)
	    {
	      if(yy < half)
		{	return child[CUTZ1L]->Node(XgI[0],XgI[1],XgI[2]);    }
	      else 
		{	return child[CUTZ1H]->Node(XgI[0],XgI[1],XgI[2]);    }
	    }
	  else
	    {
	      if(yy < half)
		{   return child[CUTZ2L]->Node(XgI[0],XgI[1],XgI[2]);    }
	      else
		{	return child[CUTZ2H]->Node(XgI[0],XgI[1],XgI[2]);    }
	    }
	  
	}
      else if (dims == 1)
	{
	  int rx = xx % half;
	  int ry = yy;
	  int rz = zz;
	  XgI[ root->extfromintern[ 0 ] ] = rx;
	  XgI[ root->extfromintern[ 1 ] ] = ry;
	  XgI[ root->extfromintern[ 2 ] ] = rz;
	  if(xx < half)
	    {  return child[CUTZ1L]->Node(XgI[0],XgI[1],XgI[2]); }
	  else
	    {  return child[CUTZ1H]->Node(XgI[0],XgI[1],XgI[2]); }
	}
      else
	{assert (0);} // bogus dims
    }
  return this;
}

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
  if (!realnode)
    return;
  int printweightedbounds = 1;
  int printbounds = 0;
  int printnsites = 0;
  int printsites = 0;
  int printvoxels = 0;
  int printvolume = 0;
  int printverletsum = 0;
  int printweights = 0;
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
      if(printweightedbounds)
	{
          double w = GetTotalWeight();
	  printf(" %lf %lf %lf %lf %lf %lf %d %d %d %lf",
		 low[0],low[1],low[2],
		 high[0],high[1],high[2],
		 meshnum[0],meshnum[1],meshnum[2], w);		 
	}
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
	      printf("[ %f %f ] ",low[i],high[i]); 
	    }
	  //	  printf("\n");
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
	  printf("Node %d Volume = %f ",id,V);
	}
      if(printweights)
	{
          double w = GetTotalWeight();

	  printf("weight [ %d %d %d ] = %f  ",meshnum[0],meshnum[1],meshnum[2], w);
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
      if(printloadzonecounts)
        {
              printf("LoadZone Node %d count = %d\n",id,nLoadVox);
        }
      
      if( printlocalfrags )
        {
        ReportLocalFrags();
        }
      printf("\n");
    }
}


double ORBNode::NodeSqrDistance(ORBNode *nd,double *Low, double *High)
{
  if(!realnode || !nd->realnode)
    return 1e20;
  double vec[DIMS];
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = High[i] - Low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      if( low[i] >= nd->high[i])  // low no overlap
	{
	  dp = low[i] - nd->high[i];	      
	  if(dp < 0.0)
	    dp = 0.0;

	  dpp = dV + nd->low[i] - high[i];
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)
	    dp = dpp;
	}
      else if( high[i] <= nd->low[i])
	{
	  dp = nd->low[i] - high[i];	      
	  if(dp < 0.0)
	    dp = 0.0;
	  
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

void ORBNode::NodeMinDeltas(double *alow, double *ahigh,double *Low,double *High,double *vec)
{
  if(!realnode)
    {    assert(0); }
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = High[i] - Low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      if( low[i] >= ahigh[i])  // low no overlap
	{
	  dp = low[i] - ahigh[i];	      
	  if(dp < 0.0)
	    dp = 0.0;	  
	  dpp = dV + alow[i] - high[i];
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)
	    dp = dpp;
	}
      else if( high[i] <= alow[i])
	{
	  dp = alow[i] - high[i];	      
	  if(dp < 0.0)
	    dp = 0.0;	  
	  dpp = low[i] - ( ahigh[i] - dV );
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)
	    dp = dpp;
	}
      assert(dp >= 0.0);
      vec[i] = dp ;
    }
  return;
} 

void ORBNode::NodeMinDeltas(ORBNode *nd,double *Low,double *High,double *vec)
{
  if(!realnode || !nd->realnode)
    {    assert(0); }
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = High[i] - Low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      if( low[i] >= nd->high[i])  // low no overlap
	{
	  dp = low[i] - nd->high[i];	      
	  if(dp < 0.0)
	    dp = 0.0;

	  dpp = dV + nd->low[i] - high[i];
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)
	    dp = dpp;
	}
      else if( high[i] <= nd->low[i])
	{
	  dp = nd->low[i] - high[i];	      
	  if(dp < 0.0)
	    dp = 0.0;
	  
	  dpp = low[i] - ( nd->high[i] - dV );
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)
	    dp = dpp;
	}
      vec[i] = dp ;
    }
  return;
} 

void ORBNode::NodeMaxDeltas(ORBNode *nd,double *Low,double *High,double *vec)
{
  if(!realnode || !nd->realnode)
    {    assert(0); }
  // determine overlap
  for(int i = 0;i< DIMS;i++)
    {
      double dV = High[i] - Low[i];
      vec[i] = 0.0;
      double dp = 0.0;
      double dpp = 0.0;
      double w1 = high[i] - low[i];
      double w2 = nd->high[i] - nd->low[i];
      if( low[i] >= nd->high[i])  // low no overlap
	{
	  dp = low[i] - nd->high[i];	      	  
	  if(dp < 0.0)
	    dp = 0.0;
	  dpp = dV + nd->low[i] - high[i];
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)	    dp = dpp;
	  vec[i] = dp + w1 + w2;
	}
      else if( high[i] <= nd->low[i])
	{
	  dp = nd->low[i] - high[i];	      
	  if(dp < 0.0)
	    dp = 0.0;
	  dpp = low[i] - ( nd->high[i] - dV );
	  if(dpp < 0.0)
	    dpp = 0.0;
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


int ORBNode::RanksDeltaFromBox(double *alow, double *ahigh, double dX, double dY, double dZ, int* ranks, int &len, int max)
{
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);

  InitSet(ranks,NNodes);

  double delta[DIMS];
  delta[0] = dX;
  delta[1] = dY;
  delta[2] = dZ;
  int nc = 1<< dims;
  if(id < 0)
    {
      for(int i = 0;i < nc;i++)
	{
	  child[i]->AddRanks(alow,ahigh,delta,low,high,ranks,len,max); 
	}
    }
  else
    {
      AddRanks(alow,ahigh,delta,low,high,ranks,len,max);
    }
  CollapseIntSet(ranks,len,NNodes);
  return len;      
}

int ORBNode::RanksInLoadBox(double *vlow, double* vhigh,int *voxels, int &vlen, int vmax,int* ranks, int &len, int max)
{

  int nvox = VoxelsInBox(vlow, vhigh , voxels, vlen, vmax);
  assert(nvox > 0);
  int nranks = RanksFromVoxels(voxels,nvox,ranks,len, max);
  return nranks;
}

int ORBNode::RanksInBox(int px, int py, int pz, double dX, double dY, double dZ, int* ranks, int &len, int max)
{
  ORBNode * mynode = NodeFromProcCoord(px,py,pz);
  if(mynode == NULL)
    { return 0; }

  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);
  InitSet(ranks,NNodes);

  double box[DIMS];
  box[0] = dX*dX;
  box[1] = dY*dY;
  box[2] = dZ*dZ;
  int nc = 1<< dims;
  if(id < 0)
    {
      for(int i = 0;i < nc;i++)
	{
	  child[i]->AddRanks(mynode,box,low,high,ranks,len,max); 
	}
    }
  else
    {
      AddRanks(mynode,box,low,high,ranks,len,max);
    }
  CollapseIntSet(ranks,len,NNodes);

  return len;      
}

int ORBNode::RanksFromVoxels(int * voxels,int nvox, int *ranks,int &len, int max)
{
  int NNodes = root->NXProcs() * root->NYProcs() * root->NZProcs();
  assert(max >= NNodes);
  InitSet(ranks,NNodes);

  for(int i = 0;i< nvox;i++)
    {
      double v[DIMS];
      VoxelPoint(voxels[i],v);
      ORBNode *nd = root->Node(v[0],v[1],v[2]);
      ranks[nd->id] = (int) nd ; //nd->rank;
    }
  CollapseIntSet(ranks,len,NNodes);
  return len;      
}
int ORBNode::VoxelsInBox(double *vlow, double *vhigh , int *voxels, int &len, int max)
{

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
  // allow accumulation  len = 0;
  double rr = r*r;
  int NVox = root->voxnum[0] *root->voxnum[1] *root->voxnum[2];
  assert(max >= NVox);
  InitSet(voxels,NVox);

  OrVoxelNeighborSet( r, voxels, len, max);

  CollapseIntSet(voxels,len,NVox);
  return len;      
}
int ORBNode::VoxelsInWagonWheel( double r, int* voxels, int &len, int max)
{
  // allow accumulation  len = 0;
  double rr = r*r;
  int NVox = root->voxnum[0] *root->voxnum[1] *root->voxnum[2];
  assert(max >= NVox);
  InitSet(voxels,NVox);

  OrVoxelWagonWheelSet( r, voxels, len, max);

  CollapseIntSet(voxels,len,NVox);
  return len;      
}

void ORBNode::VoxelRange(double *vlow, double *vhigh, int *vnlow, int *vnhigh)
{
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
	      NodeMinDeltas(vlow,vhigh,root->low,root->high,vd);
	      double minrr = 0.0;
	      for(int ii=0;ii<DIMS;ii++)
		minrr += vd[ii]*vd[ii];
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
void ORBNode::OrVoxelWagonWheelSet( double r, int* voxels, int &len, int max)
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
  
  ni[0] = (vnlow[0] + vnhigh[0]) / 2;
  for(ni[1] = vnlow[1]; ni[1] <= vnhigh[1] ; ni[1]++)
    {
      for(ni[2] = vnlow[2]; ni[2] <= vnhigh[2] ; ni[2]++)
	{
	  double vd[DIMS];
	  int Id = VoxelId(ni);
	  VoxelBounds(Id,vlow,vhigh);
	  NodeMinDeltas(vlow,vhigh,root->low,root->high,vd);
	  double minrr = 0.0;
	  for(int ii=0;ii<DIMS;ii++)
	    minrr += vd[ii]*vd[ii];
	  if (minrr <= rr)
	    {
	      VoxelIndex vi = VoxelIdToVoxelIndex(Id);
	      voxels[vi] = Id;
	    }
	}
    }
  ni[1] = (vnlow[1] + vnhigh[1]) / 2;
  ni[2] = (vnlow[2] + vnhigh[2]) / 2;
  
  for(ni[0] = vnlow[0]; ni[0] <= vnhigh[0] ; ni[0]++)
    {
	  double vd[DIMS];
	  int Id = VoxelId(ni);
	  VoxelBounds(Id,vlow,vhigh);
	  NodeMinDeltas(vlow,vhigh,root->low,root->high,vd);
	  double minrr = 0.0;
	  for(int ii=0;ii<DIMS;ii++)
	    minrr += vd[ii]*vd[ii];
	  if (minrr <= rr)
	    {
	      VoxelIndex vi = VoxelIdToVoxelIndex(Id);
	      voxels[vi] = Id;
	    }
    } 
  return; 
}

int ORBNode::RanksInWagonWheel(int px, int py, int pz, double r, int* ranks, int &len, int max)
{
  ORBNode * mynode = NodeFromProcCoord(px,py,pz);
  if(mynode == NULL)
    { return 0; }
  
  int MaxVoxels = root->voxnum[ 0 ] * root->voxnum[ 1 ] * root->voxnum[ 2 ];  

  int* MyVoxels = (int *) malloc( sizeof(int) * MaxVoxels );

  if( MyVoxels == NULL )
    {
    assert("MyVoxels == NULL");
    }

  int VoxelCount;
  mynode->VoxelsInWagonWheel( r, MyVoxels, VoxelCount, MaxVoxels );
  
  RanksFromVoxels( MyVoxels, VoxelCount, ranks, len, MaxVoxels );
  
  free( MyVoxels );

  return len;
}
int ORBNode::RanksInHomeZoneSubsetNeighborhood(int voxIdLow, int voxIdHigh, double r, int* ranks, int &len,int max)
{
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
	      nd.NodeMinDeltas(vlow,vhigh,root->low,root->high,vd);
	      double minrr = 0.0;
	      for(int ii=0;ii<DIMS;ii++)
		minrr += vd[ii]*vd[ii];
	      if (minrr <= rr)
		{
		  VoxelPoint(Id,vd);
		  ORBNode *nnd = root->Node(vd[0], vd[1], vd[2]);
		  ranks[nnd->id] = nnd->rank;
		  //		  VoxelIndex vi = VoxelIdToVoxelIndex(Id);
		  //		  voxels[vi] = Id;
		}
	    }
	}
    } 
  CollapseIntSet(ranks,len,NNodes);
  return len; 

}

int ORBNode::RanksInNeighborhoodVersion2(int px, int py, int pz, double r, int* ranks, int &len, int max)
{
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
	  child[i]->AddRanks(mynode,rr,low,high,ranks,len,max); 
	}
    }
  else
    {
      AddRanks(mynode,rr,low,high,ranks,len,max);
    }
  CollapseIntSet(ranks,len,NNodes);

  return len;
}

int ORBNode::AddRanks(ORBNode * mynode, double r, double *Low, double *High,int * ranks, int &len,int max)
{
  if(!realnode)
    return len;
  double d = NodeSqrDistance(mynode,Low,High);
  if(d <= r)
    {
      if(id < 0)
	{
	  int nc = 1<< dims;
	  for(int i = 0;i < nc;i++)
	    {
	      child[i]->AddRanks(mynode,r,Low,High,ranks,len,max); 
	    }
	}
      else
	{
	  // if(id != mynode->id)
	    {
	      ranks[id] = rank;
	    }
	}
    }
  return len;
}

int ORBNode::AddRanks(ORBNode * mynode, double *box, double *Low, double *High,int * ranks, int &len,int max)
{
  if(!realnode)
    return len;
  double deltas[DIMS];
  NodeMinDeltas(mynode,Low,High,deltas);
  int inside = 1;
  for(int i = 0; i< DIMS; i++)
    {
      deltas[i] *= deltas[i];
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
	      child[i]->AddRanks(mynode,box,Low,High,ranks,len,max); 
	    }
	}
      else
	{
	  // if(id != mynode->id)
	    {
	      ranks[id] = (int) this ; //rank;
	    }
	}
    }
  return len;
}

int ORBNode::AddRanks(double *alow, double *ahigh, double *box, double *Low, double *High,int * ranks, int &len,int max)
{
  if(!realnode)
    return len;
  double deltas[DIMS];
  NodeMinDeltas(alow,ahigh,Low,High,deltas);
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
	      child[i]->AddRanks(alow,ahigh,box,Low,High,ranks,len,max); 
	    }
	}
      else
	{
	  // if(id != mynode->id)
	    {
	      ranks[id] = rank;
	    }
	}
    }
  return len;
}


ORBNode * ORBNode::Node(double x, double y, double z)
{
  if(!realnode) return NULL;
  if(x < low[0]) return NULL;
  if(x >= high[0]) return NULL;
  if(y < low[1]) return NULL;
  if(y >= high[1]) return NULL;
  if(z < low[2]) return NULL;
  if(z >= high[2]) return NULL;
  if(id >=0 )
    return this;
  
  int IgX[3],XgI[3];
  double Ic[3],Xc[3];
  int d0 = root->extfromintern[ 0 ];
  int d1 = root->extfromintern[ 1 ];
  int d2 = root->extfromintern[ 2 ];

  Xc[ 0 ] = x; 
  Xc[ 1 ] = y; 
  Xc[ 2 ] = z; 
  for(int i=0;i<DIMS;i++)
	Ic[ i ] = Xc[ root->extfromintern[ i ] ];

  if(dims == 3)
    {
      if(Xc[d0] < child[CUTZ1L]->high[d0]) // low X
	{
	  if ( Xc[d1] < child[CUTZ1L]->high[d1]) // zcut1 
	    {
	      if( Xc[d2] < child[CUTZ1L]->high[d2])  // zcut1L
		{
		  return child[CUTZ1L]->Node(x,y,z);
		}
	      else if( Xc[d2] >= child[CUTZ1H]->low[d2])  //  Zcut1H
		{
		  return child[CUTZ1H]->Node(x,y,z);
		}
	      else { assert(0);} // tree is bogus
	    }
	  else   // zcut 2
	    {
	      if( Xc[d2] < child[CUTZ2L]->high[d2])  // zcut2L
		{
		  return child[CUTZ2L]->Node(x,y,z);
		}
	      else if( Xc[d2] >= child[CUTZ2H]->low[d2])  //  Zcut1H
		{
		  return child[CUTZ2H]->Node(x,y,z);
		}
	      else {assert(0);} // tree is bogus
	    }
	}
      else if(Xc[d0] >= child[CUTZ3L]->low[d0]) // high X
	{
	  if ( Xc[d1] < child[CUTZ3L]->high[d1]) // zcut3 
	    {
	      if( Xc[d2] < child[CUTZ3L]->high[d2])  // zcut3L
		{
		  return child[CUTZ3L]->Node(x,y,z);
		}
	      else if( Xc[d2] >= child[CUTZ3H]->low[d2])  //  Zcut3H
		{
		  return child[CUTZ3H]->Node(x,y,z);
		}
	      else {assert(0);} // tree bogus
	    }
	  else  if ( Xc[d1] >= child[CUTZ4H]->low[d1]) // zcut 4
	    {
	      if( Xc[d2] < child[CUTZ4L]->high[d2])  // zcut4L
		{
		  return child[CUTZ4L]->Node(x,y,z);
		}
	      else if ( Xc[d2] >= child[CUTZ4H]->low[d2])  //  Zcut4H
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
      if(Xc[d0] < child[CUTZ1L]->high[d0]) // low X
	{
	  if ( Xc[d1] < child[CUTZ1L]->high[d1]) // zcut1 
	    {
	      return child[CUTZ1L]->Node(x,y,z);
	    }
	  else   // zcut 2
	    {		      
	      return child[CUTZ1H]->Node(x,y,z);
	    }
	}
      else if(Xc[d0] >= child[CUTZ2L]->low[d0]) // high X
	{
	  if ( Xc[d1] < child[CUTZ2L]->high[d1]) // zcut3 
	    {
	      return child[CUTZ2L]->Node(x,y,z);
	    }
	  else  if ( Xc[d1] >= child[CUTZ2H]->low[d1]) // zcut 4
	    {
	      return child[CUTZ2H]->Node(x,y,z);
	    }
	  else { assert(0);} // tree is bogus
	}
      else { assert(0);} // tree is bogus
    }
  else if(dims == 1)
    {
      if(Xc[d0] < child[CUTZ1L]->high[d0]) // low X
	{
	  return child[CUTZ1L]->Node(x,y,z);
	}
      else if(Xc[d0] >= child[CUTZ1H]->low[d0]) // high X
	{
	  return child[CUTZ1H]->Node(x,y,z);
	}
      else { assert(0);} // tree is bogus
    }
  else
	{ assert(0); } // dims are bogus
  return NULL;
}
void ORBNode::SwapNodes(ORBNode **n1,ORBNode **n2)
{ORBNode *t = *n1; *n1 = *n2; *n2 = t;} 

int ORBNode::DetermineDims(int level)
{
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


int ORBNode::Subdivide(int levels,int ndims = 3, int nX=8, int nY=8, int nZ=8)
{
  int d = DIMS;
  int d0 = root->extfromintern[0];
  int d1 = root->extfromintern[1];
  int d2 = root->extfromintern[2];
  dims = ndims;
  if(parent == NULL)
    {
      realnode = 1;
    }

  if(root == this)
    {
      voxnum[ 0 ] = nX;
      voxnum[ 1 ] = nY;
      voxnum[ 2 ] = nZ;


      /// If we're the root, determine the level based on procnums
      levels = 1;
      while(DetermineDims(levels) > 0)
	{
	  levels++;   // stunt to cause extra divisions of long dim
	}
      levels--;
      id = -levels;
      SortDim(d0);
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
      TakeCut(this,child[CUTXL],child[CUTXH],d0);
      
      if(dims > 1)
	{
	  child[CUTXL]->SortDim(d1);
	  child[CUTXH]->SortDim(d1);
	  
	  // YCut 1
	  TakeCut(child[CUTXL],child[CUTY1L],child[CUTY1H],d1);
	  
	  //Y Cut 2
	  TakeCut(child[CUTXH],child[CUTY2L],child[CUTY2H],d1);
	  
	  if(dims > 2)
	    {
	      child[CUTY1L]->SortDim(d2);
	      child[CUTY1H]->SortDim(d2);
	      // ZCut 1
	      TakeCut(child[CUTY1L],child[CUTZ1L],child[CUTZ1H],d2);
	      child[CUTZ1L]->SortDim(d0);
	      child[CUTZ1H]->SortDim(d0);
	      
	      // ZCut 2
	      TakeCut(child[CUTY1H],child[CUTZ2L],child[CUTZ2H],d2);
	      child[CUTZ2L]->SortDim(d0);
	      child[CUTZ2H]->SortDim(d0);
	      
	      child[CUTY2L]->SortDim(d2);
	      child[CUTY2H]->SortDim(d2);
	      // ZCut 3
	      TakeCut(child[CUTY2L],child[CUTZ3L],child[CUTZ3H],d2);
	      child[CUTZ3L]->SortDim(d0);
	      child[CUTZ3H]->SortDim(d0);
	      
	      // ZCut 4
	      TakeCut(child[CUTY2H],child[CUTZ4L],child[CUTZ4H],d2);
	      child[CUTZ4L]->SortDim(d0);
	      child[CUTZ4H]->SortDim(d0);
	    }
	  else
	    {
	      SwapNodes(&child[CUTY1L],&child[CUTZ1L]);
	      SwapNodes(&child[CUTY1H],&child[CUTZ1H]);
	      SwapNodes(&child[CUTY2L],&child[CUTZ2L]);
	      SwapNodes(&child[CUTY2H],&child[CUTZ2H]);
	      child[CUTZ1L]->SortDim(d0);
	      child[CUTZ1H]->SortDim(d0);
	      child[CUTZ2L]->SortDim(d0);
	      child[CUTZ2H]->SortDim(d0);
	    }	      
	}
      else
	{
	  SwapNodes(&child[CUTXL],&child[CUTZ1L]);
	  SwapNodes(&child[CUTXH],&child[CUTZ1H]);
	  child[CUTZ1L]->SortDim(d0);
	  child[CUTZ1H]->SortDim(d0);
	}
      
      // Final children are arranged 0-7; 8-13 are intermediate and can be removed if memory is and issue
      n = 1 << dims;
      for(int i=0;i<n;i++)
	child[i]->Subdivide(levels - 1,DIMS);
      
    }
  else
    {
      // Here we have arrived at a leaf.For now just report.
      assert(realnode);
      VerifyContents();
    }
  return 0;
}
int ORBNode::TakeCut(ORBNode *src, ORBNode *destL, ORBNode *destH, int dim)
{
  
  int N = src->FindCutSite();
  Site *cutsite = NULL;
  if(N > 0) cutsite = src->GetSite(N-1);
  int N2 = src->nsites;

  double WorkCut = (cutsite != NULL) ? cutsite->c[dim] : 0.0;
  double EvenCut = 0.5 * (src->low[dim] + src->high[dim]);

  double delta = EvenCut - WorkCut;
  double cut  = mAlphaForWorkVolMix * delta + WorkCut;
  double WorkCutLength = WorkCut - src->low[dim];
  double MixProportion = delta / WorkCutLength; 

  if (0)
    fprintf(stderr,"Mixproprotion %f cut length %f delta %f alpha %f\n",MixProportion, WorkCutLength, delta, mAlphaForWorkVolMix);


  N = 0;
  for(int i=0;i<src->nsites;i++)
    {
      Site *t = src->GetSite(i);
      if(t->c[dim] >= cut)
	break;
      N++;
    }



  if(SNAP_TO_GRID)
    {
      cut = SnapToGrid(cut,dim);
      // Find New cutsite
      N = 0;
      for(int i=0;i<src->nsites;i++)
	{
	  Site *t = src->GetSite(i);
	  if(t->c[dim] >= cut)
	    break;
	  N++;
	}
    }


  for(int i = 0;i < DIMS;i++)
    {
      destL->low[i] = src->low[i];
      destL->high[i] = src->high[i];
      destH->low[i] = src->low[i];
      destH->high[i] = src->high[i];
    }
  
  destL->SetStart(src->StartSite());
  destL->SetLen(N);	       	  
  destL->CopySites(src,0,N);
  destL->high[dim] = cut;
  destL->realnode = 1;
  //  destL->VerifyContents();
  
  destH->SetStart(src->StartSite() + N);
  destH->SetLen(N2-N);
  destH->CopySites(src,N,N2-N);
  destH->low[dim] = cut;
  destH->realnode = 1;

  //  destH->VerifyContents();
  
  return 0;
}
int ORBNode::SubdivideEvenly(int levels,int ndims = 3, int nX=8, int nY=8, int nZ=8)
{
  int d = DIMS;
  dims = ndims;
  if(parent == NULL)
    {
      realnode = 1;
    }

  if(root == this)
    {
      voxnum[0] = nX;
      voxnum[1] = nY;
      voxnum[2] = nZ;

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
      TakeEvenCut(this,child[CUTXL],child[CUTXH],0);
      
      if(dims > 1)
	{
	  child[CUTXL]->SortY();
	  child[CUTXH]->SortY();
	  
	  // YCut 1
	  TakeEvenCut(child[CUTXL],child[CUTY1L],child[CUTY1H],1);
	  
	  //Y Cut 2
	  TakeEvenCut(child[CUTXH],child[CUTY2L],child[CUTY2H],1);
	  
	  if(dims > 2)
	    {
	      
	      child[CUTY1L]->SortZ();
	      child[CUTY1H]->SortZ();
	      // ZCut 1
	      TakeEvenCut(child[CUTY1L],child[CUTZ1L],child[CUTZ1H],2);
	      child[CUTZ1L]->SortX();
	      child[CUTZ1H]->SortX();
	      
	      // ZCut 2
	      TakeEvenCut(child[CUTY1H],child[CUTZ2L],child[CUTZ2H],2);
	      child[CUTZ2L]->SortX();
	      child[CUTZ2H]->SortX();
	      
	      child[CUTY2L]->SortZ();
	      child[CUTY2H]->SortZ();
	      // ZCut 3
	      TakeEvenCut(child[CUTY2L],child[CUTZ3L],child[CUTZ3H],2);
	      child[CUTZ3L]->SortX();
	      child[CUTZ3H]->SortX();
	      
	      // ZCut 4
	      TakeEvenCut(child[CUTY2H],child[CUTZ4L],child[CUTZ4H],2);
	      child[CUTZ4L]->SortX();
	      child[CUTZ4H]->SortX();
	    }
	  else
	    {
	      SwapNodes(&child[CUTY1L],&child[CUTZ1L]);
	      SwapNodes(&child[CUTY1H],&child[CUTZ1H]);
	      SwapNodes(&child[CUTY2L],&child[CUTZ2L]);
	      SwapNodes(&child[CUTY2H],&child[CUTZ2H]);
	      child[CUTZ1L]->SortX();
	      child[CUTZ1H]->SortX();
	      child[CUTZ2L]->SortX();
	      child[CUTZ2H]->SortX();
	    }	      
	}
      else
	{
	  SwapNodes(&child[CUTXL],&child[CUTZ1L]);
	  SwapNodes(&child[CUTXH],&child[CUTZ1H]);
	  child[CUTZ1L]->SortX();
	  child[CUTZ1H]->SortX();
	}
      
      // Final children are arranged 0-7; 8-13 are intermediate and can be removed if memory is and issue
      n = 1 << dims;
      for(int i=0;i<n;i++)
	child[i]->SubdivideEvenly(levels - 1,DIMS);
      
    }
  else
    {
      // Here we have arrived at a leaf.For now just report.
      assert(realnode);
      VerifyContents();
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
      if(t->c[dim] >= cut)
	break;
      N++;
    }

  for(int i = 0;i < DIMS;i++)
    {
      destL->low[i] = src->low[i];
      destL->high[i] = src->high[i];
      destH->low[i] = src->low[i];
      destH->high[i] = src->high[i];
    }
  
  destL->SetStart(src->StartSite());
  destL->SetLen(N);	       	  
  destL->CopySites(src,0,N);
  destL->high[dim] = cut;
  destL->realnode = 1;
  //  destL->VerifyContents();
  
  destH->SetStart(src->StartSite() + N);
  destH->SetLen(N2-N);
  destH->CopySites(src,N,N2-N);
  destH->low[dim] = cut;
  destH->realnode = 1;

  //  destH->VerifyContents();
  
  return 0;
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
		  s->verlet_count = VerletCount(s->c,rr,low,high);
		  src->verlet_sum += s->verlet_count;
		}
	    }
	  
	}
    }
  UpdateNodeVerletCount();
}


void ORBNode::UpdateVerletCount(double *verletcountlist)
{
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
		  s->verlet_count = verletcountlist[s->id];
		  src->verlet_sum += s->verlet_count;
		}
	    }
	  
	}
    }
  UpdateNodeVerletCount();
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

double ORBNode::RealSqrDist(double* coord,double *Low, double * High)
{
  
  double vec[DIMS];
  double d = 0;
  double dp,dpp;
  for (int i = 0;i < DIMS;i++)
    {
      double dV = High[i] - Low[i];
      if( coord[i] < low[i])
	{
	  dp = low[i] - coord[i];
	  if(dp < 0.0)
	    dp = 0.0;
	  dpp = coord[i] - (high[i] - dV);
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)
	    dp = dpp;
	}
      else if ( coord[i] > high[i] )
	{
	  dp = coord[i] - high[i];
	  if(dpp < 0.0)
	    dpp = 0.0;
	  dpp = dV + low[i] - coord[i];
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dpp < dp)
	    dp = dpp;
	}
      else
	dp = 0;
      d += dp * dp;	  
    }
  return d;
}

double ORBNode::VerletCount(double *coord, double r, double *Low, double * High)
{
  
  if(!realnode)
    return 0;
  double n = 0;
  double d = RealSqrDist(coord,Low,High);
  if(r >= d)
    {
      if(id < 0)
	{
	  int nc = 1 << dims;
	  for(int i = 0; i < nc; i++)
	    {
	      n += child[i]->VerletCount(coord,r,Low,High);
	    }
	}
      else
	{
	  double dV[DIMS];
	  for(int i = 0; i< DIMS;i++)
	    dV[i] = High[i] - Low[i];
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

int ORBNode::VerletNeighbors(double *coord, double r, double *Low, double * High, Site *neighbor, int & nneighbors)
{
  
  if(!realnode)
    return 0;
  double n = 0.0;
  double d = RealSqrDist(coord,Low,High);
  if(r >= d)
    {
      if(id < 0)
	{
	  int nc = 1 << dims;
	  for(int i = 0; i < nc; i++)
	    {
	      n += child[i]->VerletNeighbors(coord,r,Low,High,neighbor,nneighbors);
	    }
	}
      else
	{
	  double dV[DIMS];
	  for(int i = 0; i< DIMS;i++)
	    dV[i] = High[i] - Low[i];
	  for(int i = 0; i< nsites;i++)
	    {
	      double d = GetSite(i)->SqrDist(coord,dV);
	      if(d <= r) 
		{
		  n+= 1.0;
		  neighbor[nneighbors++] = *GetSite(i);
		  // HACK
		  //		  return n;
		}
	    }
	}
    }
  return n;
}
int ORBNode::VerletNeighborsHacked(double *coord, double r, double *Low, double * High, Site *neighbor, int & nneighbors)
{
  
  if(!realnode)
    return 0;
  double n = 0.0;
  double d = RealSqrDist(coord,Low,High);
  if(r >= d)
    {
      if(id < 0)
	{
	  int nc = 1 << dims;
	  for(int i = 0; i < nc; i++)
	    {
	      n += child[i]->VerletNeighborsHacked(coord,r,Low,High,neighbor,nneighbors);
	    }
	}
      else
	{
	  double dV[DIMS];
	  for(int i = 0; i< DIMS;i++)
	    dV[i] = High[i] - Low[i];
	  for(int i = 0; i< nsites;i++)
	    {
	      double d = GetSite(i)->SqrDist(coord,dV);
	      if(d <= r) 
		{
		  n+= 1.0;
		  neighbor[nneighbors++] = *GetSite(i);
		  // HACK
		  		  return n;
		}
	    }
	}
    }
  return n;
}
void ORBNode::ReportVerletRange(double &min, double &max)
{
  if (!realnode)
    return;
  
  if(id < 0) // not a leaf
    {
      int nc = 1 << dims;      
      for(int i=0;i<nc;i++)
	{
	  child[i]->ReportVerletRange(min,max);
	}
    }
  else 
    {
      if(verlet_sum < min)
	min = verlet_sum;
      if(verlet_sum > max)
	max = verlet_sum;
    }
}

int ORBNode::GetVerletNeighbors(double *coord, double r, Site *neighbor, int & nneighbors)
{
  return VerletNeighbors(coord,r*r,low,high,neighbor,nneighbors);
} 
int ORBNode::GetVerletNeighborsHacked(double *coord, double r, Site *neighbor, int & nneighbors)
{
  return VerletNeighborsHacked(coord,r*r,low,high,neighbor,nneighbors);
} 


int ORBNode::CouldMidpointBeMine(ORBNode * node1, ORBNode *node2, double *Low, double *High)
{
  ORBNode *Nlow,*Nhigh;
  int inside;
  double dp,dpp;

  for (int i = 0;i < DIMS; i++)
    {
      // Get min dist between low walls of nodes 1 and 2
      double dV = High[i] - Low[i];
      if(node2->low[i] > node1->low[i])   // n1  n2
	{
	  dp = node2->low[i] - node1->low[i];
	  if(dp < 0.0)
	    dp = 0.0;
	  dpp = node1->low[i] - (node2->low[i] - dV);
	  if(dpp < 0.0)
	    dpp = 0.0;
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
	  if(dp < 0.0)
	    dp = 0.0;
	  dpp = node2->low[i] - (node1->low[i] - dV);
	  if(dpp < 0.0)
	    dpp = 0.0;
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
	  printf("Low %f High %f dp %f wL %f wH %f w %f dmL %f dmH %f\n",Nlow->low[i], Nhigh->low[i],dp,wL,wH,w,dmL,dmH);

	}
      // Get displacement vector from my low wall to the lowest wall of 1 and 2
      if(low[i] > Nlow->low[i])           //  Nl N  
	{
	  dp = low[i] - Nlow->low[i];
	  if(dp < 0.0)
	    dp = 0.0;
	  dpp = Nlow->low[i] - (low[i] - dV);
	  if(dpp < 0.0)
	    dpp = 0.0;
	  if(dp <= dpp)                    //     | Nl  N   |
	    {L = dp;}
	  else                            //   N |  Nl     |
	    { L = -dpp;}
	}
      else                               //   N  Nl
	{
	  dp = Nlow->low[i] - low[i];
	  if(dp < 0.0)
	    dp = 0.0;
	  dpp = low[i] - (Nlow->low[i] - dV);
	  if(dpp < 0.0)
	    dpp = 0.0;
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
	  printf(" N1: [ %4.1f %4.1f ] N2: [ %4.1f %4.1f ] N: [%4.1f %4.1f ] ( dmL <= mH ){ %4.1f %4.1f } ( dmH >= mL ) { %4.1f %4.1f} inside %d\n",node1->low[i],node1->high[i],node2->low[i],node2->high[i],low[i],high[i],dmL,mH,dmH,mL, inside);
	}
      if(!inside) break;
    }

  return inside;
}


int ORBNode::CouldVoxelMidpointBeMine(int voxId1, int voxId2, double *Low, double *High)
{
  ORBNode N1,N2;
  
  N1.realnode = 1; // Lie to the primatives to avoid code duplication
  N2.realnode = 1;
  VoxelBounds(voxId1,N1.low,N1.high);
  VoxelBounds(voxId2,N2.low,N2.high);

  int inside = CouldMidpointBeMine(&N1,&N2,Low,High);

  return inside;
}

int ORBNode::IsVoxelOnBoundary(int voxId1, double r, double *Low, double *High)
{
  ORBNode N1;
  
  N1.realnode = 1; // Lie to the primatives to avoid code duplication
  VoxelBounds(voxId1,N1.low,N1.high);

  int bounds = IsNodeOnBoundary(&N1,r,Low,High);

  return bounds;
}

double ORBNode::MaxVoxelSqrDistance(int voxId1, int voxId2)
{
  ORBNode v1,v2;
  double vec[DIMS];
  v1.realnode = v2.realnode = 1;
  VoxelBounds(voxId1,v1.low,v1.high);
  VoxelBounds(voxId2,v2.low,v2.high);
  v1.NodeMaxDeltas(&v2,root->low,root->high,vec);
  double r = 0.0;
  for(int i = 0; i< DIMS;i++)r += vec[i]*vec[i];
  return r;
}

void ORBNode::GetMidpointOfVoxelMidpoints( int voxId1, int voxId2, double* mid )
{
  ORBNode v1,v2;
  double vec1[DIMS],vec2[DIMS],*vl,*vh;
  v1.realnode = v2.realnode = 1;
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
}

int ORBNode::GetVoxelPointOfVoxelMidpoints( int voxId1, int voxId2, double* vox_pt )
{
  ORBNode v1,v2;
  double vec1[DIMS],vec2[DIMS],*vl,*vh, mid[DIMS];
  v1.realnode = v2.realnode = 1;
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

  int mid_id = VoxelId( mid );
  VoxelPoint( mid_id, vox_pt );
  return mid_id;
}

int ORBNode::GetMidpointOfVoxelPoints( int voxId1, int voxId2, double* mid )
{
  ORBNode v1,v2;
  double vec1[DIMS],vec2[DIMS],*vl,*vh;
  v1.realnode = v2.realnode = 1;
  VoxelPoint(voxId1,vec1);
  VoxelPoint(voxId2,vec2);

  for(int i=0;i<DIMS;i++)
    {
      double dV = root->high[i] - root->low[i];
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
  
  return VoxelId( mid );
}

int ORBNode::IsVoxelPointOfVoxelPointsMidpointMine(int voxId1, int voxId2)
{
  int inside = 1;
  double mid[ DIMS ];
  
  GetMidpointOfVoxelPoints( voxId1, voxId2, mid );
  int vox = VoxelId(mid);
  VoxelPoint(vox,mid);

  for(int i=0;i<DIMS;i++)
    {
      if( mid[i] < low[i] || mid[i] >= high[i] )
	{ inside = 0; break; }	
    }
  
  return inside;
}

int ORBNode::IsMidpointOfVoxelMidpointsMine(int voxId1, int voxId2)
{
  int inside = 1;
  double mid[ DIMS ];
  
  GetMidpointOfVoxelMidpoints( voxId1, voxId2, mid );

  for(int i=0;i<DIMS;i++)
    {
      if( mid[i] < low[i] || mid[i] >= high[i] )
	{ inside = 0; break; }	
    }
  
  return inside;
}
int ORBNode::IsMidpointOfVoxelPointsMine(int voxId1, int voxId2)
{
  int inside = 1;
  double mid[ DIMS ];
  
  GetMidpointOfVoxelPoints( voxId1, voxId2, mid );

  for(int i=0;i<DIMS;i++)
    {
      if(!( mid[i] >= low[i] && mid[i] < high[i]) )
	{ inside = 0; break; }	
    }
  
  return inside;
}

int ORBNode::IsVoxelPointOfVoxelMidpointsMine(int voxId1, int voxId2)
{
  int inside = 1;
  double vox_pt[ DIMS ];
  
  GetVoxelPointOfVoxelMidpoints( voxId1, voxId2, vox_pt );

  for(int i=0;i<DIMS;i++)
    {
      if( vox_pt[i] < low[i] || vox_pt[i] >= high[i] )
	{ inside = 0; break; }	
    }
  
  return inside;
}

double ORBNode::MinVoxelSqrDistance(int voxId1, int voxId2)
{
  ORBNode v1,v2;
  double vec[DIMS];
  v1.realnode = v2.realnode = 1;
  VoxelBounds(voxId1,v1.low,v1.high);
  VoxelBounds(voxId2,v2.low,v2.high);
  v1.NodeMinDeltas(&v2,root->low,root->high,vec);
  double r = 0.0;
  for(int i = 0; i< DIMS;i++)r += vec[i]*vec[i];
  return r;
}

int ORBNode::IsNodeOnBoundary(ORBNode * nd, double r, double *Low, double *High)
{
  double d = NodeSqrDistance(nd,Low,High);
  if (d > r*r)
    return 0;
  double vec[DIMS];
  NodeMaxDeltas(nd,Low,High,vec);
  double md = 0.0;
  for(int i = 0;i<DIMS;i++)
    md += vec[i]*vec[i];
  ////printf("max dist %f r^2 %f\n",md,r*r);
  if(md > r*r)
    return 1;
  else return 0;
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
int ORBNode::FreeLoadZones()
{
  assert( this == root);
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

int ORBNode::DeriveAllLoadZones()
  {
  assert( this == root);
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
  //  FreeLoadZones();
  //  AllocateLoadZones();
  
  ORBNode *nd = Node( i, j, k );
  nd->DeriveMyLoadZone();


//   int nx = NXProcs();
//   int ny = NYProcs();
//   int nz = NZProcs();

//   for(int i = 0; i < nx; i++)
//     {
//       for(int j = 0; j < ny; j++)
//         {
//           for(int k = 0; k < nz; k++)
//             {
//               ORBNode *nd = Node( i, j, k );
//               nd->DeriveMyLoadZone();
//             }
//         }
//     }
  //    No longer valid check for surface voxel load zones
//   int nvox = 0;
//   for(int i = 0; i < nx; i++)
//     {
//       for(int j = 0; j < ny; j++)
// 	{
// 	  for(int k = 0; k < nz; k++)
// 	    {
// 	      ORBNode *nd = Node(i,j,k);
// 	      nvox += nd->nLoadVox;
// 	    }
// 	}
//     }
//   int realnvox = root->voxnum[0] * root->voxnum[1] * root->voxnum[2];
//   assert(realnvox == nvox);

  return 1;
}

int ORBNode::DeriveMyLoadZone()
{
  double dr[DIMS];
  double vlow[DIMS],vhigh[DIMS];
  int vnlow[DIMS],vnhigh[DIMS];
  ORBNode n1;
  n1.realnode = 1;
  n1.root = root;

  VoxelRange(low,high,vnlow,vnhigh);

  //  printf(">%d vnlow %d %d %d vnhigh %d %d %d\n",id,vnlow[0],vnlow[1],vnlow[2],vnhigh[0],vnhigh[1],vnhigh[2]);

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
              ORBNode *tnd = root->Node(vd[0], vd[1], vd[2]);
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

int ORBNode::RanksInLoadZoneNeighborhood(double r,int *ranks,int &len,int max  )
{
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
  nd.root = this->root;
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
