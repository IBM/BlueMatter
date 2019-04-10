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
 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


class XYZ
{
public:
  double mX,mY,mZ;
  XYZ(){}
};


#include "../include/orb.hpp"



int main(int argc, char **argv)
{
  if(argc < 7)
    {
      printf("usage: %s <fname> <npoints> <X> <Y> <Z> <N> \n",argv[0]);
      exit(1);
    }
  char * fname = argv[1];
  FILE *fPF = fopen(fname,"r");
  int npoints,procs,N;
  double LX,LY,LZ;
  TreeNode * tree;
  int rotZ = 0;

  sscanf(argv[2],"%d",&npoints);
  sscanf(argv[3],"%lf",&LX);
  sscanf(argv[4],"%lf",&LY);
  sscanf(argv[5],"%lf",&LZ);
  sscanf(argv[6],"%d",&procs);
  if(argc > 7)
    sscanf(argv[7],"%d",&rotZ);

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

  XYZ c;
  double tmp;

  if(rotZ)
    {  tmp = LX; LX = LZ; LZ = tmp; }


  tree = new TreeNode();
  tree->SetLen(npoints);
  for(int i = 0;i < npoints;i++)
    {
      int n = fscanf(fPF,"%lf %lf %lf",&c.mX,&c.mY,&c.mZ);
      assert(n == 3);

      if(rotZ)
	{ tmp = c.mX; c.mX = -c.mZ; c.mZ = tmp;}
      // image coordinates in box
      while (c.mX < 0)   c.mX += LX;
      while (c.mX >= LX) c.mX -= LX;
      while (c.mY < 0)   c.mY += LY;
      while (c.mY >= LY) c.mY -= LY;
      while (c.mZ < 0)   c.mZ += LZ;
      while (c.mZ >= LZ) c.mZ -= LZ;
      tree->SetSite(i,c);
    }
  XYZ box;
  box.mX = LX;
  box.mY = LY;
  box.mZ = LZ;

  
  CookTree(npoints,box,procs,tree);
  if(0)  // Node retrieval by coordinate of each site
    {
      for(int i = 0;i < npoints;i++)
	{
	  Site * s = &tree->sites->site[i];
	  TreeNode * leaf = tree->Node(s->c[0],s->c[1],s->c[2]);
	  assert(leaf);
	  //	  leaf->ReportLeafData();
	  int found = 0;
	  for(int j=0;j<leaf->nsites;j++)
	    {
	      Site *b = &leaf->sites->site[j];
	      if(b->id == s->id)
		{ found = 1; break; }
	    }
	}
      printf("Found all sites by location\n");
    }
  if(0) // Node retreival by proc address
    {

      int nx = tree->NXProcs();
      int ny = tree->NYProcs();
      int nz = tree->NZProcs();
      for(int i=0;i< nx ; i++)
	{
	  for(int j=0;j< ny ; j++)
	    {
	      for(int k=0;k< nz ; k++)
		{
		  TreeNode * leaf = tree->Node(i,j,k);
		  assert(leaf);
		  leaf->ReportLeafData();		  
		}
	    }
	}
   }

  int ranks[10000];
  int n = 0;
  if(0) // Nodes in sphere
    {
      tree->RanksInNeighborhood(0,0,0,.1,ranks,n);
      int nx = tree->NXProcs();
      int ny = tree->NYProcs();
      int nz = tree->NZProcs();
      for(int i=0;i< nx ; i++)
	{
	  for(int j=0;j< ny ; j++)
	    {
	      for(int k=0;k< nz ; k++)
		{
		  TreeNode * leaf = tree->Node(i,j,k);
		  
		  n = 0;
		  tree->RanksInNeighborhood(i,j,k,9.0,ranks,n);
		  printf("node %d has  %d neighbors \n",leaf->id,n);
		  for(int m=0;m<n;m++)
		    {
		      printf("neighbor [ %d ] = %d\n",m,ranks[m]);
		    }
		}
	    }
	}
    }

  if(0) // Rebalance of the tree ny load
    {
      tree->UpdateVerletCount(9.0);  
      TreeNode *baltree = BalanceLoad(tree);
      baltree->ReportLeafData();
      double min = 1e20,max = 0;
      baltree->ReportVerletRange(min,max);
      printf("Range :  %f   min = %f  max = %f\n",max-min,min,max);
    }

  if(0) // Rebalance of the tree ny load
    {
      tree->UpdateVerletCount(9.0);  
      TreeNode *baltree = BalanceLoad(tree);
      baltree->ReportLeafData();
      printf("tree has %d sites from %d originals\n",baltree->nsites,tree->nsites);

      Site* A = new Site[baltree->nsites];
      Site* B = new Site[baltree->nsites];
      int nA,nB;
      for(int i = 0;i < baltree->nsites;i++)
	{
	  Site * a = &baltree->sites->site[i];
	  nA = 0;
	  baltree->GetVerletNeighbors(a->c,9.0,A,nA);
	  for(int j = i+1;j < nA;j++)
	    {
	      Site * b = &A[j];
	      nB = 0;
	      baltree->GetVerletNeighbors(b->c,9.0,B,nB);
	      int found = 0;
	      for(int k = 0;k < nB; k++)
		{
		  Site *c = &B[k];
		  if(c->id == a->id)
		    { found = 1; break; }
		}
	      assert(found);
	    }
	  printf("All %d neighbors of  %d find %d \n", nA,a->id, a->id);
	}


    }


  if(1) // Rebalance of the tree based on midpoints
    {
      tree->UpdateVerletCount(9.0);  
      TreeNode *baltree = new TreeNode();

      int nA,nB = 0;
      Site* A = new Site[500];
      Site mid;
      for(int i = 0;i < tree->nsites;i++)
	{
	  Site * a = &tree->sites->site[i];
	  nA = 0;
	  tree->GetVerletNeighbors(a->c,9.0,A,nA);
	  for(int j = 0;j < nA;j++)
	    {
	      if(a->id < A[j].id)
		{
		  nB++;
		}
	    }
	}
      baltree->SetLen(tree->nsites * 400);

      for(int i = 0;i < tree->nsites;i++)
	{
	  Site * a = &tree->sites->site[i];
	  nA = 0;
	  tree->GetVerletNeighbors(a->c,9.0,A,nA);
	  for(int j = 0;j < nA;j++)
	    {
	      if(a->id < A[j].id)
		{
		    c.mX = (a->c[0] + A[j].c[0]) / 2.0;
		    c.mY = (a->c[1] + A[j].c[1]) / 2.0;
		    c.mZ = (a->c[2] + A[j].c[2]) / 2.0;
		  nB++;
		  baltree->SetSite(nB,c);
		}
	    }
	}
      
      printf("set %d interactions\n",nB);
      CookTree(baltree->nsites,box,procs,baltree);
      baltree->ReportLeafData();
    }
  
      
  //tree->Rank(int px,int py, int pz,double r,int *ranks, int &len);

  exit(1);
}

