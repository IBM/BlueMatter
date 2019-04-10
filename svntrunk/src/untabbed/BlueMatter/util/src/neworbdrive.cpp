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
#include <math.h>

#define _ORB_STANDALONE_

#define PLATFORM_ABORT {assert(0);}
#define fsel(x,y,z) x
#define min(x,y) {(x < y) ? x :  y}


#define is_almost_zero(x) {return (fabs(x) < 1e-10) ? 1 : 0;}
#include "../include/orbnode.hpp"
#include "../include/orbnode.cpp"



#define DEFAULT_SIZE 100000

int main(int argc, char **argv)
{
  if(argc < 13)
    {
      printf("usage: %s <fname> <npoints> <X> <Y> <Z> <nx> <xy> <nz> <px> <py> <pz> <alpha> \n",argv[0]);
      exit(1);
    }
  char * fname = argv[1];
  FILE *fPF = fopen(fname,"r");
  int npoints,procs,N;
  double LX,LY,LZ;
  double alpha;
  ORBNode * tree , *stree;
  int rotZ = 0;
  int nX,nY,nZ,pX,pY,pZ;

  sscanf(argv[2],"%d",&npoints);
  sscanf(argv[3],"%lf",&LX);
  sscanf(argv[4],"%lf",&LY);
  sscanf(argv[5],"%lf",&LZ);
  sscanf(argv[6],"%d",&nX);
  sscanf(argv[7],"%d",&nY);
  sscanf(argv[8],"%d",&nZ);
  sscanf(argv[9],"%d",&pX);
  sscanf(argv[10],"%d",&pY);
  sscanf(argv[11],"%d",&pZ);
  sscanf(argv[12],"%lf",&alpha);



  XYZ c;
  double tmp;

#define ZFACT 1.0  

  LZ *=ZFACT;

  tree = new ORBNode();
  tree->SetLen(npoints);
  tree->SetOrbDims(LX,LY,LZ,nX,nY,nZ,pX,pY,pZ);

  
  int cnt = 0;
  double totalweight = 0.0;
  for(int i = 0;i < npoints;i++)
    {

      double weight = 1.0;
      int mlid1,mlid2,atcount1,atcount2;
      int n = fscanf(fPF,"%lf %lf %lf %lf %d %d %d %d",&c.mX,&c.mY,&c.mZ,&weight,&mlid1,&mlid2,&atcount1,&atcount2);
      if(0)
  {
    printf("%lf %lf %lf %lf %d %d %d %d \n",c.mX,c.mY,c.mZ,weight,mlid1,mlid2,atcount1,atcount2);
  }
      assert(n >= 3);
      //      if(n < 4)
      //	weight = 1.0;

      c.mZ *= ZFACT;
      // image coordinates in box
      while (c.mX < 0)   c.mX += LX;
      while (c.mX >= LX) c.mX -= LX;
      while (c.mY < 0)   c.mY += LY;
      while (c.mY >= LY) c.mY -= LY;
      while (c.mZ < 0)   c.mZ += LZ;
      while (c.mZ >= LZ) c.mZ -= LZ;
      tree->SetSite(i,c,weight);
      totalweight += weight;
      cnt++;
    }
  int npts = tree->NumSites();
  cerr << "Total Weight " << totalweight << endl;
  cerr << "Mean " << totalweight/pX/pY/pZ << endl;
  cerr << "N " << npts << " cnt " << cnt << endl;

  XYZ box;
  box.mX = LX;
  box.mY = LY;
  box.mZ = LZ;

  //CookEvenTree(npoints,box,procs,tree,nX,nY,nZ,pX,pY,pZ);
       //CookTree(npoints,box,procs,tree,nX,nY,nZ,pX,pY,pZ);
  tree->SubdivideEvenly(0);
  tree->DeriveAllLoadZones();

  //  tree->ReportLeafData();

  stree = tree;


  if(0)  // Node retrieval by coordinate of each site
    {
      for(int i = 0;i < npoints;i++)
  {
    Site * s = tree->GetSite(i);
    ORBNode * leaf = tree->NodeFromSimCoord(s->c[0],s->c[1],s->c[2]);
    assert(leaf);
    //	  leaf->ReportLeafData();
    int found = 0;
    for(int j=0;j<leaf->nsites;j++)
      {
        Site *b = leaf->GetSite(j);
        if(b->id == s->id)
    { found = 1; break; }
      }
  }
      printf("Found all sites by location\n");
    }
  if(0) // Node retreival by proc address
    {

      int nx = pX;
      int ny = pY;
      int nz = pZ;
      for(int i=0;i< nx ; i++)
  {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
    {
      ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
      assert(leaf);
      leaf->ReportLeafData();		  
    }
      }
  }
   }

#define NMAX (64*64*64 +1)
  int ranks[NMAX];
  int voxels[NMAX];
  int ranks2[NMAX];
  int voxels2[NMAX];
  int ns = 0,nw = 0,n=0;

  if(0) // Nodes in sphere
    {
      double R = 6.0;
      tree->RanksInNeighborhood(0,0,0,R,ranks,n,NMAX);
      int nx = pX;
      int ny = pY;
      int nz = pZ;
      for(int i=0;i< nx ; i++)
  {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
    {
      ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
      
      n = 0;
      tree->RanksInNeighborhood(i,j,k,R,ranks,n,NMAX);
      printf("node %d has  %d neighbors \n",leaf->id,n);
      for(int m=0;m<n;m++)
        {
          printf("neighbor [ %d ] = %x\n",m,ranks[m]);
        }
    }
      }
  }
    }


  if(0) // Check for any load zone overlap
    {

      int nx = pX;
      int ny = pY;
      int nz = pZ;
      double dl[3];
      int nvox = 0,nranks = 0;
      ORBNode n1;

      for(int i=0;i< nx ; i++)
        {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
          {
      ORBNode * leaf1 = tree->NodeFromProcCoord(i,j,k);
      
      for(int ii=i;ii< nx ; ii++)
        {
          for(int jj=j;jj< ny ; jj++)
            {
        for(int kk=k;kk< nz ; kk++)
          {
            if(i == ii && j == jj & k == kk)
              continue;

            ORBNode * leaf2 = tree->NodeFromProcCoord(ii,jj,kk);
            int n = 0,nn = 0;
            if(leaf1->LoadZone[n] < leaf2->LoadZone[nn])
              {
          n++;
          if(n == leaf1->nLoadVox)
            break;
              }
            else if(leaf1->LoadZone[n] > leaf2->LoadZone[nn])
              {
          nn++;
          if(nn == leaf2->nLoadVox)
            break;
              }
            else
              {
          // found overlap
          int voxnum = leaf1->LoadZone[n];
          leaf1->ReportLeafData();
          leaf2->ReportLeafData();
          double low[3],high[3];
          tree->VoxelExternalBounds(voxnum,low,high);
          double dd[DIMS];
          n1.VoxelPoint(voxnum,dd);
          printf("found leaf %d and %d have voxel %08d in common\n",leaf1->id,leaf2->id,leaf1->LoadZone[n]);
          
          assert(0);
              }
          }
            }
        }
          }
      }
        }
      printf("Passed load zone orthogonality test\n");

    }
  if(0) // Check voxel neighborhood consistency 
    {

      int nx = nX;
      int ny = nY;
      int nz = nZ;
      int dl[DIMS];
      int nvox = 0,nvox2 = 0,nranks = 0;

      for(int i=0;i< nx ; i++)
  {
    dl[0] = i;
    for(int j=0;j< ny ; j++)
      {
        dl[1] = j;
        for(int k=0;k< nz ; k++)
    {
      dl[2] = k;
    
      int voxid = tree->VoxelExternalId(dl);
      n = 0;
      double D = 0.1;
      //double D = 2.0;
      double low1[3],low2[3],low3[3],high1[3],high2[3],high3[3],tlow[3],thigh[3];
      ORBNode n1,n2,n3;
      nvox = 0;
      tree->VoxelExternalBounds(voxid,tlow,thigh);
      tree->InternalFromExternal(tlow,low1);
      tree->InternalFromExternal(thigh,high1);
      n1.ConfigNode(1,1,low1,high1,tree);
      n1.VoxelsInNeighborhood(D,voxels,nvox, NMAX);
      int ind = tree->VoxelIdToVoxelIndex(voxid);
      printf("voxindex %d  id %08d has  %d nvoxels \n",ind,voxid,nvox); fflush(stdout);
      printf("Target id %x\n",voxid);
      tree->ReportVoxel(voxid);
      for(int m=0;m<nvox;m++)
        {
          int voxid2 = voxels[m];
          nvox2 = 0;
          tree->VoxelExternalBounds(voxid2,tlow,thigh);
          tree->InternalFromExternal(tlow,low2);
          tree->InternalFromExternal(thigh,high2);
          n2.ConfigNode(2,2,low2,high2,tree);
          n2.VoxelsInNeighborhood(D,voxels2,nvox2, NMAX);
          printf("Neighbor %d id %x\n",m,voxid2);
          tree->ReportVoxel(voxid2);
          int found = 0;
          for(int mm=0;mm<nvox2;mm++)
      {
        int testvox = voxels2[mm];
        printf("\tNeigbor Neighbor %d id %x\n",mm,testvox);
        tree->ReportVoxel(testvox);

        if(testvox == voxid)
          { found = 1; break;}
      }
          assert(found);
        }		  		  
    }
      }
  }
      printf("Passed voxel neighborhood consistency test\n");
    }
  if(0) // check load zone nieghborhood self consistency 
    {
      //      ORBNode *temptree = stree;
      //            stree = tree;
      int nx = pX;
      int ny = pY;
      int nz = pZ;
      
      double dl[3];
      int nvox = 0,nranks = 0;

      for(int i=0;i< nx ; i++)
  {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
    {
      ORBNode * leaf = stree->NodeFromProcCoord(i,j,k);

      n = 0;
      double D = 6.82;
      //double D = 1.0;
      nranks = 0;
      leaf->RanksInLoadZoneNeighborhood(D, ranks, nranks, NMAX); 
      printf("node %d has  %d ranks %d voxels ",leaf->id,nranks,nvox);
      leaf->ReportLeafData();
      for(int m=0;m<nranks;m++)
        {
          ORBNode * nd = (ORBNode *) ranks[m];
          printf("Neighbor %03d.[%03d] = %03d\n",leaf->id,m,nd->id);
          int nranks2 = 0, nvox2 = 0;
          nd->RanksInLoadZoneNeighborhood(D, ranks2, nranks2, NMAX); 
          int found = 0;
          for(int mm=0;mm<nranks2;mm++)
      {
        int testint = (int) leaf;
        if(ranks2[mm] == testint)
          { found = 1; break;}
      }
          assert(found);
        }		  		  
      printf("Passed self consistency for node %d %d %d\n",i,j,k);
    }
      }
  }
      printf("Passed self consistency test\n");
    }
  if(0) // check load zone nieghborhood Count
    {
      //      ORBNode *temptree = stree;
      //            stree = tree;
      int nx = pX;
      int ny = pY;
      int nz = pZ;
      
      double dl[3];
      int nvox = 0,nranks = 0;
      int totranks = 0;
      int minranks = 1000000000;
      int maxranks = 0;
      for(int i=0;i< nx ; i++)
  {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
    {
      ORBNode * leaf = stree->NodeFromProcCoord(i,j,k);

      n = 0;
      double D = 8.6;
      //double D = 1.0;
      nranks = 0;
      leaf->RanksInLoadZoneNeighborhood(D, ranks, nranks, NMAX); 
      totranks += nranks;
      if(nranks > maxranks)
        maxranks = nranks;
      if(nranks < minranks)
        minranks = nranks;
      printf("node %d [ %d %d %d ] ranks=  %d    total ranks= %d   min= %d  max= %d\n",leaf->id,i,j,k,nranks, totranks, minranks, maxranks);
      //leaf->ReportLeafData();
    }
      }
  }
    }

  if(0) // voxels in sphere or spot test
    {
      int nx = pX;
      int ny = pY;
      int nz = pZ;
      //      for(int i=0;i< nx ; i++)
  {
    //	  for(int j=0;j< ny ; j++)
      {
        //	      for(int k=0;k< nz ; k++)
    {
      int i = 0;
      int j = 3;
      int k = 1;
      int id1 = 15750175;
      int id2 = 1066015;
      ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
      if(1)
        {
          n = 0;
          leaf->VoxelsInNeighborhood(15.0,ranks,n,NMAX);
          printf("node %d has  %d neighbors \n",leaf->id,n);
          for(int m=0;m<n;m++)
      {
        int bound = leaf->IsVoxelOnBoundary(ranks[m],15.0);
        if(ranks[m] == id1 || ranks[m] == id2)
          printf("neighbor [ %d ] = %d  boundary %d\n",m,ranks[m],bound);
      }
        }
    }
      }
  }
    }

  if(1)
    {
      int nVOX = nX * nY * nZ;
      int *voxlist = new int[nVOX];
      int id[DIMS],nv = 0;
      double D=8.6;
      for(int i=0;i<pX;i++)
  {
    for(int j=0;j<pY;j++)
      {
        for(int k=0;k<pZ;k++)
    {
      nv = n = 0;
      ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
      leaf->VoxelsInNeighborhood(D,voxlist,nv, NMAX);
      tree->RanksInNeighborhood(i,j,k,D,ranks,n,NMAX);
      printf("node %d %d %d nvox %d nodes %d\n", i , j , k ,nv ,n);

      for(int i1 = 1; i1 < nv; i1++)
        {
          int id1 = voxlist[i1];
          for(int i2 = 0; i2 < nv; i2++)
      {			  
        int id2 = voxlist[i2];
        int found = 0;
        for(int nn=0; nn < n;nn++)
          {
            ORBNode *nd1 = (ORBNode *) ranks[nn];
            found += nd1->IsMidpointOfVoxelPointsMine(voxlist[i1],voxlist[i2]);
          }
        if(!found)
          {
            double vec1[DIMS],vec2[DIMS],mid[DIMS],vl[DIMS],vh[DIMS];
            leaf->Low(vl);
            leaf->High(vh);
            tree->VoxelPoint(id1,vec1);
            tree->VoxelPoint(id2,vec2);
            tree->GetMidpointOfVoxelPoints(id1,id2,mid);
            printf("Node low %f %f %f   high %f %f %f\n",vl[0],vl[1],vl[2],vh[0],vh[1],vh[2]);
            printf("midpoint of Vox1 %f %f %f\n",vec1[0],vec1[1],vec1[2]);
            printf("midpoint of Vox2 %f %f %f\n",vec2[0],vec2[1],vec2[2]);
            printf("midpoint of Voxels %f %f %f\n",mid[0],mid[1],mid[2]);
            printf("Node %d %d %d vx %d %d not found\n",i,j,k,id1,id2);
            assert(0);
          }
        if(found > 1)
          {
            printf("Node %d %d %d vx %d %d  found %d times\n",i,j,k,id1,id2,found);
            assert(0);
          }
      }
        }
    }
      }
  }
#if 0
      for(int i1 = 1; i1 < nV; i1++)
  {
    for(int i2 = 0; i2 < nV; i2++)
      {
        int claimed = 0;
        int nx = pX;
        int ny = pY;
        int nz = pZ;
        for(int i=0;i< nx ; i++)
    {
      for(int j=0;j< ny ; j++)
        {
          for(int k=0;k< nz ; k++)
      {
        ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
        claimed += leaf->IsMidpointOfVoxelPointsMine(voxlist[i1],voxlist[i2]);
      }
        }
    }
        if(!claimed)
    {
      printf("voxid %d %d is unclaimed\n",voxlist[i1],voxlist[i2]);
      ORBNode n1;
      ORBNode n2;
      double low1[3],low2[3],high1[3],high2[3],tlow[3],thigh[3];
      int id1 = voxlist[i1];
      int id2 = voxlist[i2];
      
      double v[DIMS],Vhigh[DIMS],Vlow[DIMS];
      tree->Low(Vlow);
      tree->High(Vhigh);
      double vec1[DIMS],vec2[DIMS],mid[DIMS],*vl,*vh;
      tree->VoxelPoint(id1,vec1);
      tree->VoxelPoint(id2,vec2);
      tree->GetMidpointOfVoxelPoints(id1,id2,mid);
      printf("midpoint of Vox1 %f %f %f\n",vec1[0],vec1[1],vec1[2]);
      printf("midpoint of Vox2 %f %f %f\n",vec2[0],vec2[1],vec2[2]);
      printf("midpoint of Voxels %f %f %f\n",mid[0],mid[1],mid[2]);
      assert(0);
    }
        else if(claimed > 1)
    {
      printf("voxid %d %d is claimed %d times\n",voxlist[i1],voxlist[i2],claimed);
      assert(0);
    }
        //	      else 
        //		{printf("voxid %d %d is claimed  \n",voxlist[i1],voxlist[i2]);}
      }
  }
#endif


    }


  if(0)
    {
      //      ORBNode *temptree = stree;
      //            stree = tree;
      int nx = pX;
      int ny = pY;
      int nz = pZ;
      int NNodes = pX * pY * pZ;
      int NVox = nX * nY * nZ;

      int *Voxels = new int[NVox];
      int *Ranks = new int[NNodes];
      int *Ranks2 = new int[NNodes];
      
      double dl[3];
      int nvox = 0,nranks = 0;

      for(int i=0;i< nx ; i++)
  {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
    {
      int pc[3];
      ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);

      n = 0;
      double D = 6.82;
      //double D = 1.0;
      nranks = 0;
      double vlow[3],vhigh[3],nlow[3],nhigh[3];
      leaf->Low(nlow);
      leaf->High(nhigh);
      for(int ii=0;ii< DIMS;ii++)
        {
          vlow[ii] = nlow[ii] - D;
          vhigh[ii] = nhigh[ii] + D;
        }
      tree->RanksInBox(i,j,k,D,D,D, Ranks, nranks, NNodes); 
      //leaf->RanksInLoadBox(vlow,vhigh,Voxels,nvox,NVox, Ranks, nranks, NNodes); 
      printf("node %d %d %d has  %d ranks %d voxels ",i,j,k,nranks,nvox);
      //		  leaf->ReportLeafData();
      for(int m=0;m<nranks;m++)
        {
          ORBNode * nd = (ORBNode *) Ranks[m];
          printf("Neighbor %03d.[%03d] = %03d\n",leaf->id,m,nd->id);
          int nranks2 = 0, nvox2 = 0;
          double vvlow[3],vvhigh[3];
          nd->Low(nlow);
          nd->High(nhigh);
          for(int ii=0;ii< DIMS;ii++)
      {
        vvlow[ii] = nlow[ii] - D;
        vvhigh[ii] = nhigh[ii] + D;
      }
          int nc[3];
          tree->ExternalProcMeshNum(nc);
          tree->RanksInBox(nc[0],nc[1],nc[2],D,D,D, Ranks2, nranks2, NNodes); 
          //		      nd->RanksInLoadBox(vvlow,vvhigh,Voxels,nvox,NVox, Ranks2, nranks2, NNodes); 
          int found = 0;
          for(int mm=0;mm<nranks2;mm++)
      {
        int testint = (int) leaf;
        ORBNode *nnd = (ORBNode *)Ranks2[mm];
        if(Ranks2[mm] == testint)
          { found = 1; break;}
      }
          if(found == 0)
      {
        printf("Failed to find %d [%d %d %d ]in neighborlist of %d [ %d %d %d ]\n",leaf->id, i, j ,k ,nd->id, nc[0] ,nc[1] ,nc[2] );
        leaf->ReportLeafData();
        nd->ReportLeafData();
      }
          assert(found);
        }		  		  
      printf("Passed self consistency for node %d %d %d\n",i,j,k);
    }
      }
  }
      
    }
  exit(1);
}
