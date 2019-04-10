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
 #ifndef _INCLUDED_RECBIS_
#define _INCLUDED_RECBIS_

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

//
//  The key to the algorithm is contained in the Subdivide and TakeCut methods of class TreeNode
//

class TreeNode;

int CompareX(const void *elem1, const void *elem2);
int CompareY(const void *elem1, const void *elem2);
int CompareZ(const void *elem1, const void *elem2);
// TreeNode *  CookTree(int npoints,double* x,double *y,double *z,double Lx,double Ly,double Lz, int N);



class Site
{
public:
  int id;
  double verlet_count;
  double c[DIMS];
  Site(){id = verlet_count =0; c[0]= c[1]=c[2] = 0.0;}
  Site & operator=(Site &s)
    {
      verlet_count = s.verlet_count;
      id = s.id;
      for(int i=0;i<DIMS;i++){c[i] = s.c[i];}
      return *this;
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
      printf("[ %d %f %f %f %f ]\n",id,verlet_count, c[0],c[1],c[2]);
    }
};

class IdList
{
public:
  int nids;
  int *ids;

  IdList():nids(0),ids(NULL){}
  SetLen(int len)
    {
      if(ids != NULL)
	{ delete [] ids; ids = NULL;}
      nids = len;
      if(len > 0)
	{ ids = new int[nids]; for(int i=0;i<nids;i++){ids[i]=0;}}
      return 1;
    }

};
class SiteList
{
public:
  int nids;
  Site *site;

  SiteList():nids(0),site(NULL){}
  SetLen(int len)
    {
      if(site != NULL)
	{ delete [] site; site = NULL;}
      nids = len;
      if(len > 0)
	{ site = new Site[nids]; }
      return 1;
    }
};
  
  


class TreeNode
{
 public:
  double low[DIMS],high[DIMS];
  int meshnum[DIMS];
  
  SiteList *sites;
  int id;
  int realnode;
  int rank;
  int nsites;
  int dims;
  double verlet_sum;

  TreeNode *child[NCHILDREN];
  TreeNode *parent;
  TreeNode()
    {
      id = 0;
      realnode = 0;
      rank = -1;
      for(int i=0;i<DIMS;i++)
	{
	  low[i]=high[i]=0.0;
	  meshnum[i] = 0;
	}
      int n = NCHILDREN;
      for(int i=0;i<n;i++)
	{
	  child[i]=NULL;
	}
      parent = NULL;      
      sites = NULL;
      nsites = 0;
    }
  
  void SetSite( int i, XYZ aSite )
    {
      sites->site[ i ].id = i;
      sites->site[ i ].c[0] = aSite.mX;
      sites->site[ i ].c[1] = aSite.mY;
      sites->site[ i ].c[2] = aSite.mZ;
    }

  void SetLen(int n)
    {
      if(n <=0)
	{ printf("SetLen n = %d\n",n); assert(0);}
      if(sites != NULL)delete sites;
      sites = new SiteList;
      sites->SetLen(n);
      nsites = n;
    }
  void SortX()
    {
      qsort(sites->site,nsites,sizeof(Site),CompareX);
    }
  void SortY()
    {
      qsort(sites->site,nsites,sizeof(Site),CompareY);
    }
  void SortZ()
    {
      qsort(sites->site,nsites,sizeof(Site),CompareZ);
    }
  
  void SetRange(int dim, double l, double h)
    {
      low[dim] = l; high[dim] = h;
    }
  
  void CopySites(TreeNode *src, int start, int stop)
    {
      int j = 0;
      for(int i=start;i<stop;i++,j++)
	{sites->site[j] = src->sites->site[i];  }
    }      

  int NXProcs() {return 1 << (-id);}
  int NYProcs() 
    {
      int nx = 1 << (-id); 
      int n = (dims > 1) ? nx : nx >> 1; 
      return n;
    }
  int NZProcs() 
    {

      int nx = 1 << (-id); 
      int n = (dims > 2) ? nx : nx >> 1; 
      return n;
    }

  void xyzFromProcCount(int procs, int &x, int &y, int &z)
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
  
  void VerifyContents()
    {
      for(int a=0;a<nsites;a++)
	{
	  Site *s = & sites->site[a];
	  for(int i=0;i<DIMS;i++)
	    {
	      if(s->c[i] < low[i]){printf("dim %d for site %d is too low! %f s= %f\n", i,a,low[i], s->c[i]); s->Print(); assert(0);} 
	      if(s->c[i] > high[i]){printf("dim %d for site %d is too high! %f s= %f\n",i,a, high[i], s->c[i]); s->Print(); assert(0);} 
	    }
	}
    }
  
  int SetVoxelNums(int dx,int dy, int dz)
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
          // rank = id; //Platform::Topology::MakeRankFromCoords( dz, dy, dx );
          rank = Platform::Topology::MakeRankFromCoords( dx, dy, dz );          
	}
      return id;
    }
  
  //
  // return the node given the voxel coordinates
  //
  TreeNode * Node(int x,int y, int z) 
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
	    {assert (0);} // bogus dims
	}
      return this;
    }
  //
  // number leaves from this point down,starting with n. Returns next id
  //  
  int NumberLeaves(int n)
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
    void ReportLeafData()
    {
      if (!realnode)
	return;
      int printbounds = 0;
      int printnsites = 0;
      int printsites = 0;
      int printvoxels = 0;
      int printvolume = 0;
      int printverletsum = 1;

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
	      printf("Node %d \t %d\n",id,nsites);	  
	    }
	  if(printverletsum)
	    {
	      printf("Node %d \tverletsum   %d\n", rank, verlet_sum);	  
	      // printf("Node %d \t nsites %d \tverletsum   %f\n",id,nsites,verlet_sum);	  
	    }
	  if(printbounds)
	    {
	      for(int i = 0; i < DIMS; i++)
		{      
		  printf("[ %f %f ]\n",low[i],high[i]); 
		}
	    }
	  if(printsites)
	    {
	      for(int i=0;i<nsites;i++)
		{
		  sites->site[i].Print();
		}
	    }
	  if(printvoxels)
	    {
	      printf("[ %d %d %d ]\n",meshnum[0],meshnum[1],meshnum[2]);
	    }

	  if(printvolume)
	    {
	      double V = (high[0] - low[0]) * (high[1] - low[1]) * (high[2] - low[2]);
	      printf("Node %d Volume = %f\n",id,V);
	    }

	}
    }

    void ReportVerletRange(double &min, double &max)
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

  double NodeSqrDistance(TreeNode *nd,double *Low, double *High)
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

  int RanksInNeighborhood(int px, int py, int pz, double r, int* ranks, int &len)
    {
      TreeNode * mynode = Node(px,py,pz);
      if(mynode == NULL)
	{ return 0; }

      double rr = r*r;
      int nc = 1<< dims;
      if(id < 0)
	{
	  for(int i = 0;i < nc;i++)
	    {
	      child[i]->AddRanks(mynode,rr,low,high,ranks,len); 
	    }
	}
      else
	{
	      AddRanks(mynode,rr,low,high,ranks,len);
	}
      return len;      
    }
  int AddRanks(TreeNode * mynode, double r, double *Low, double *High,int * ranks, int &len)
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
		  child[i]->AddRanks(mynode,r,Low,High,ranks,len); 
		}
	    }
	  else
	    {
	      if(id != mynode->id)
		{
		  ranks[len] = rank;
		  len++;
		}
	    }
	}
      return len;
    }

  TreeNode * Node(double x, double y, double z)
    {
      if(!realnode) return NULL;
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



  void SwapNodes(TreeNode **n1,TreeNode **n2)
    {TreeNode *t = *n1; *n1 = *n2; *n2 = t;} 


  // 
  //  Recursive call to process prepare subdivisions
  //
  int Subdivide(int levels,int ndims = 3)
    {
      int d = DIMS;
      dims = ndims;
      if(parent == NULL)
	  realnode = 1;

      if(levels > 0)
	{
	  int n = NCHILDREN;
	  for(int i = 0;i < n; i++)
	    {
	      child[i] = new TreeNode();
	      child[i]->parent = this;
	      child[i]->id = -(levels - 1);
	    }
	  
	  // Xcut
	  TakeCut(this,child[CUTXL],child[CUTXH],0);

	  if(dims > 1)
	    {
	      child[CUTXL]->SortY();
	      child[CUTXH]->SortY();

	      // YCut 1
	      TakeCut(child[CUTXL],child[CUTY1L],child[CUTY1H],1);
	  
	      //Y Cut 2
	      TakeCut(child[CUTXH],child[CUTY2L],child[CUTY2H],1);

	      if(dims > 2)
		{

		  child[CUTY1L]->SortZ();
		  child[CUTY1H]->SortZ();
		  // ZCut 1
		  TakeCut(child[CUTY1L],child[CUTZ1L],child[CUTZ1H],2);
		  child[CUTZ1L]->SortX();
		  child[CUTZ1H]->SortX();
		  
		  // ZCut 2
		  TakeCut(child[CUTY1H],child[CUTZ2L],child[CUTZ2H],2);
		  child[CUTZ2L]->SortX();
		  child[CUTZ2H]->SortX();
		  
		  child[CUTY2L]->SortZ();
		  child[CUTY2H]->SortZ();
		  // ZCut 3
		  TakeCut(child[CUTY2L],child[CUTZ3L],child[CUTZ3H],2);
		  child[CUTZ3L]->SortX();
		  child[CUTZ3H]->SortX();
		  
		  // ZCut 4
		  TakeCut(child[CUTY2H],child[CUTZ4L],child[CUTZ4H],2);
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
  //
  // Actual routine that bisects
  //
  int TakeCut(TreeNode *src, TreeNode *destL, TreeNode *destH, int dim)
    {
 
      int N = src->FindCutSite();
      Site *cutsite = NULL;
      if(N > 0) cutsite = &src->sites->site[N-1];
      int N2 = src->nsites;
      double cut = (cutsite != NULL) ? cutsite->c[dim] : 0.0;
      
      for(int i = 0;i < DIMS;i++)
	{
	  destL->low[i] = src->low[i];
	  destL->high[i] = src->high[i];
	  destH->low[i] = src->low[i];
	  destH->high[i] = src->high[i];
	}

      destL->SetLen(N);	       	  
      destL->CopySites(src,0,N);
      destL->high[dim] = cut;
      destL->realnode = 1;
      
      destH->SetLen(N2-N);
      destH->CopySites(src,N,N2);
      destH->low[dim] = cut;
      destH->realnode = 1;

      return 0;
    }

  int FindCutSite()
    {
      
      if(nsites == 0)
	return 0;
      int N = nsites/2;
      if(sites->site[0].verlet_count == 0.0)
	{
	  return N;
	}

      int m;
      double *ver = new double[nsites];
      for(int i=0;i<nsites;i++)
	{
	  ver[i] = sites->site[i].verlet_count;
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
            delete [] ver;
      return m;      
    }
  int FindMedian(double *ver, int n, int cur,int window)
    {
      double v = ver[cur];
      if(v > 0.5)
	{
	  if (window == 1)
	    {
	      while(ver[cur] > 0.5 && cur > 0)
		cur--;
	      //	      double dcur1 =  ver[cur] - 0.5;
	      //	      double dcur2 =  ver[cur + 1] - 0.5;
	      //	      cur  = (dcur1*dcur1 > dcur2*dcur2) ? cur + 1 : cur;
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
	      //	      double dcur1 =  ver[cur] - 0.5;
	      //	      double dcur2 =  ver[cur - 1] - 0.5;
	      //	      cur  = (dcur1*dcur1 > dcur2*dcur2) ? cur - 1 : cur;
	      //printf("median ver[%d] = %f; half = %d\n",cur,ver[cur],n/2);
	      return cur;
	    }
	  else
	    return FindMedian(ver,n,cur + window/2,window/2);
	}
      return cur;
    }

  //////////////////////////////////////////////////////////
  //  Verlet stuff
  // 
  void UpdateVerletCount(double r)
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
		  TreeNode * src = Node(i,j,k);
		  src->verlet_sum = 0;
		  for(int a = 0; a < src->nsites; a++)
		    {
		      Site *s = &src->sites->site[a];
		      s->verlet_count = VerletCount(s->c,rr,low,high);
		      src->verlet_sum += s->verlet_count;
		    }
		}
	      
	    }
	}
      UpdateNodeVerletCount();
    }

  void UpdateVerletCount(double *verletcountlist)
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
		  TreeNode * src = Node(i,j,k);
		  src->verlet_sum = 0;
		  for(int a = 0; a < src->nsites; a++)
		    {
		      Site *s = &src->sites->site[a];
		      s->verlet_count = verletcountlist[s->id];
		      src->verlet_sum += s->verlet_count;
		    }
		}
	      
	    }
	}
      UpdateNodeVerletCount();
    }


  void UpdateNodeVerletCount()
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
		  TreeNode * src = Node(i,j,k);
		  src->verlet_sum = 0;
		  for(int a = 0; a < src->nsites; a++)
		    {
		      Site *s = &src->sites->site[a];
		      src->verlet_sum += s->verlet_count;
		    }
		}
	      
	    }
	}
      
    }
  double RealSqrDist(double* coord,double *Low, double * High)
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

  double VerletCount(double *coord, double r, double *Low, double * High)
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
		  double d = sites->site[i].SqrDist(coord,dV);
		  if(d <= r) 
		    n += 1.0;
		}
	    }
	}
      return n;
    }
  int GetVerletNeighbors(double *coord, double r, Site *neighbor, int & nneighbors)
    {
      return VerletNeighbors(coord,r*r,low,high,neighbor,nneighbors);
    } 
  int VerletNeighbors(double *coord, double r, double *Low, double * High, Site *neighbor, int & nneighbors)
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
		  double d = sites->site[i].SqrDist(coord,dV);
		  if(d <= r) 
		    {
		      n+= 1.0;
		      neighbor[nneighbors++] = sites->site[i];
		    }
		}
	    }
	}
      return n;
    }

};

int CompareX(const void *elem1, const void *elem2)
{
  Site* A = (Site *)elem1;
  Site* B = (Site *)elem2;
  if (A->c[0] == B->c[0])return 0;
  else if (A->c[0] < B->c[0]) return -1;
  else return 1;
}
int CompareY(const void *elem1, const void *elem2)
{
  Site* A = (Site *)elem1;
  Site* B = (Site *)elem2;
  if (A->c[1] == B->c[1])return 0;
  else if (A->c[1] < B->c[1]) return -1;
  else return 1;
}
int CompareZ(const void *elem1, const void *elem2)
{
  Site* A = (Site *)elem1;
  Site* B = (Site *)elem2;
  if (A->c[2] == B->c[2])return 0;
  else if (A->c[2] < B->c[2]) return -1;
  else return 1;
}

void CookTree(int aNpoints, XYZ& aBoxDim, int aProcNumber, TreeNode* tree )
{

  // Find full space power of 2

  int d = 3;
  int levels = 0;
  int t;
  do
    { 
      levels++;
      t = 1 << (levels*d);
    }while (aProcNumber > t);
  int base = (levels-1)*d;
  while(d > 0)
    { 
      t = 1 << (base + d);
      if(aProcNumber == t) break;
      d--;
    }
  assert(d>0);

  
  int N=levels;
  tree->id = -levels;
  

  qsort( tree->sites->site, aNpoints, sizeof( Site ), CompareX );

  tree->low[ 0 ] = 0.0;
  tree->low[ 1 ] = 0.0;
  tree->low[ 2 ] = 0.0;

  tree->high[ 0 ] = aBoxDim.mX;
  tree->high[ 1 ] = aBoxDim.mY;
  tree->high[ 2 ] = aBoxDim.mZ;

  tree->Subdivide( levels, d );
  tree->NumberLeaves( 0 );
  tree->SetVoxelNums( 0,0,0 );
// tree->ReportLeafData();
}



TreeNode*  BalanceLoad(TreeNode *stree)
{
  TreeNode *tree = new TreeNode();
  
  Site * sites = new Site[stree->nsites];
  tree->id = stree->id;
  tree->dims = stree->dims;
  int N = -stree->id;
  tree->SetLen(stree->nsites);
  for(int i = 0;i < DIMS;i++)
    {
      tree->low[i] = stree->low[i];
      tree->high[i] = stree->high[i];
    }

  int nx = tree->NXProcs();
  int ny = tree->NYProcs();
  int nz = tree->NZProcs();
  int ns = 0;
  for(int i = 0;i < nx; i++)
    {
      for(int j = 0;j < ny; j++)
	{
	  for(int k = 0;k < nz; k++)
	    {
	      TreeNode * src = stree->Node(i,j,k);
	      src->verlet_sum = 0;
	      for(int a = 0; a < src->nsites; a++)
		{
		  tree->sites->site[ns++] = src->sites->site[a];
		}
	    }
	}
    }

  qsort(tree->sites->site,tree->nsites,sizeof(Site),CompareX);

  tree->Subdivide(N,stree->dims);
  tree->NumberLeaves(0);
  tree->SetVoxelNums(0,0,0);
  tree->UpdateNodeVerletCount();

  return tree;
}


#endif
