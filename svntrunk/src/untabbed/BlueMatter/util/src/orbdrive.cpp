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

class FragmentSimple
{
public:
  int mCount;
  int mParentMoleculeID;
  FragmentSimple(int mlid, int count)
    {
      mCount = count;
      mParentMoleculeID = mlid;
    }
};

 int PairType(const FragmentSimple *a, const FragmentSimple *b)
    {
        int n1 = a->mCount;
        int n2 = b->mCount;
        if (n1 < n2) {
            int tmp = n1;
            n1 = n2;
            n2 = tmp;
        }
        switch(n1) {
            case 1:
                return 0;
            case 2:
                if (n2 == 1)
                    return 1;
                else
                    return 2;
            case 3:
                switch(n2) {
                    case 1: return 3;
                    case 2: return 4;
                    case 3:
                        if (a->mParentMoleculeID == b->mParentMoleculeID)
                            return 5;  // This is intra (slower)
                        else
                            return 6;  // This is inter (faster)
                }
            case 4:
                return 6+n2;
        }
        printf("pair type error\n");
        exit(-1);
  return 1;
    }

    double GetCost(int pairtype)
    {
        switch(pairtype) {
            case 0:
                return 1.048728;
            case 1:
                return 1.608737;
            case 2:
                return 2.728592;
            case 3:
                return 2.044509;
            case 4:
                return 3.670676;
            case 5:
                return 4.448243;
            case 6:
                return 3.462162;
            case 7:
                return 2.949303;
            case 8:
                return 4.426865;
            case 9:
                return 6.332759;
            case 10:
                return 5.655414;
        }
        return -1;
    }

double FranksSpiffyWeight(    int mlid1,int mlid2, int at1, int at2)
{
  FragmentSimple f1(mlid1,at1),f2(mlid2,at2);
  int pairtype = PairType(&f1,&f2);
  double cost = GetCost(pairtype);
  return cost;
}
double ModelInteractionWeight(int mlid1,int mlid2, int at1, int at2)
{
  double ModWeight;
  
  int atcount1 = (at1 < at2) ? at1 : at2;
  int atcount2 = (at1 < at2) ? at2 : at1;
  if(mlid1 == mlid2)
    {
      switch(atcount1)
  {
  case 1:  
    switch(atcount2)
      {
      case 1: ModWeight = 1.217803; break; // 1.072657; break;
      case 2: ModWeight = 1.657851; break; // 1.78807; break;
      case 3: ModWeight = 2.092172; break; // 2.285994; break;
      case 4: ModWeight = 2.678360; break; //2.968841; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  case 2:  
    switch(atcount2)
      {
      case 2: ModWeight = 2.678111; break; // 2.768698; break;
      case 3: ModWeight = 3.560940; break;  // 3.729389; break;
      case 4: ModWeight = 4.307348; break; //4.670057; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  case 3:  
    switch(atcount2)
      {
      case 3: ModWeight = 4.448243; break; // 5.2042 ; break;
      case 4: ModWeight = 5.990370; break; // 6.382139 ; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  case 4:  
    switch(atcount2)
      {
      case 4: ModWeight = 5.626951; break; // 7.762542; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  default:
    char msg[256]; 
    sprintf(msg,"Frag atom count out of range: %d\n", atcount1);		  
    PLATFORM_ABORT(msg);		  
  }
    }
  else
    {
      switch(atcount1)
  {
  case 1:  
    switch(atcount2)
      {
      case 1: ModWeight = 1.217803; break; // 1.072657; break;
      case 2: ModWeight = 1.657851; break; // 1.78807; break;
      case 3: ModWeight = 2.092172; break; // 2.285994; break;
      case 4: ModWeight = 2.678360; break; //2.968841; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  case 2:  
    switch(atcount2)
      {
      case 2: ModWeight = 2.678111; break; // 2.768698; break;
      case 3: ModWeight = 3.560940; break;  // 3.729389; break;
      case 4: ModWeight = 4.307348; break; //4.670057; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  case 3:  
    switch(atcount2)
      {
      case 3: ModWeight = 3.462162; break; // 3.439775; break;
      case 4: ModWeight = 5.990370; break; // 6.382139 ; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  case 4:  
    switch(atcount2)
      {
      case 4: ModWeight = 5.626951; break; // 7.762542; break;
      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
        PLATFORM_ABORT(msg);		  
      }
    break;
  default:
    char msg[256]; 
    sprintf(msg,"Frag atom count out of range: %d\n", atcount1);		  
    PLATFORM_ABORT(msg);		  
  }
    }

  return ModWeight;
}


#define DEFAULT_SIZE 100000

int main(int argc, char **argv)
{
  if(argc < 14)
    {
      printf("usage: %s <fname> <npoints> <X> <Y> <Z> <N> <nx> <xy> <nz> <px> <py> <pz> <alpha> \n",argv[0]);
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
  sscanf(argv[6],"%d",&procs);
  sscanf(argv[7],"%d",&nX);
  sscanf(argv[8],"%d",&nY);
  sscanf(argv[9],"%d",&nZ);
  sscanf(argv[10],"%d",&pX);
  sscanf(argv[11],"%d",&pY);
  sscanf(argv[12],"%d",&pZ);
  sscanf(argv[13],"%lf",&alpha);


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

  rotZ = 0;
   if(rotZ)
  {  tmp = LX; LX = LZ; LZ = tmp; tmp = nX; nX = nZ; nZ = tmp; }


  tree = new ORBNode();
  tree->Configure(npoints);
  tree->SetLen(npoints);

  //  tree->SetWorkVolMixingParam(alpha);

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

      if(n > 4)
  {
    //	  weight = ModelInteractionWeight(mlid1,mlid2,atcount1,atcount2);
    weight = FranksSpiffyWeight(mlid1,mlid2,atcount1,atcount2);
  }

       if(rotZ)
      { tmp = c.mX; c.mX = -c.mZ; c.mZ = tmp;}
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
  int npts = tree->NSites;
  cerr << "Total Weight " << totalweight << endl;
  cerr << "Mean " << totalweight/pX/pY/pZ << endl;
  cerr << "N " << npts << " cnt " << cnt << endl;

  XYZ box;
  box.mX = LX;
  box.mY = LY;
  box.mZ = LZ;

       CookEvenTree(npoints,box,procs,tree,nX,nY,nZ,pX,pY,pZ);
       //CookTree(npoints,box,procs,tree,nX,nY,nZ,pX,pY,pZ);
   // tree->ReportLeafData();

   if (0)
     {
       stree = new ORBNode();
       stree->Configure(500000);
       stree->SetLen(npoints);
       for(int i = 0;i < npoints;i++)
   {
     Site * s = tree->GetSite(i);
     int vox = tree->VoxelId(s->c);
     tree->VoxelPoint(vox,s->c);
     XYZ p;
     p.mX = s->c[0];      p.mY = s->c[1];      p.mZ = s->c[2];
     stree->SetSite(i,p,s->verlet_count);
   }
       CookTree(npoints,box,procs,stree,nX,nY,nZ);
     }
   else
     stree = tree;

  stree->ReportLeafData();

  if(0)  // Node retrieval by coordinate of each site
    {
      for(int i = 0;i < npoints;i++)
  {
    Site * s = tree->GetSite(i);
    ORBNode * leaf = tree->Node(s->c[0],s->c[1],s->c[2]);
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

      int nx = tree->NXProcs();
      int ny = tree->NYProcs();
      int nz = tree->NZProcs();
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
  if(0) // sphere vs wagonwheel
    {      
      printf("\tSphere r/2 \tN \tWagWhel r \tN \n");
      for(double r = 6.0; r <= 10.0 ; r+= 0.25)
  {
    tree->RanksInNeighborhoodVersion2(0,0,0,r,ranks,ns,NMAX);
    tree->RanksInWagonWheel(0,0,0,2.0*r,ranks,nw,NMAX);
    printf("\t%f \t%d \t%f \t%d \n",r,ns,2.0*r,nw);
  }
    }
  if(0) // Nodes in sphere
    {
      tree->RanksInNeighborhoodVersion2(0,0,0,.1,ranks,n,NMAX);
      int nx = tree->NXProcs();
      int ny = tree->NYProcs();
      int nz = tree->NZProcs();
      for(int i=0;i< nx ; i++)
  {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
    {
      ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
      
      n = 0;
      tree->RanksInNeighborhood(i,j,k,6.0,ranks,n,NMAX);
      printf("node %d has  %d neighbors \n",leaf->id,n);
      for(int m=0;m<n;m++)
        {
          printf("neighbor [ %d ] = %x\n",m,ranks[m]);
        }
    }
      }
  }
    }

  if(0) // Rebalance of the tree ny load
    {
      tree->UpdateVerletCount(9.0);  
      ORBNode *baltree = BalanceLoad(tree);
      baltree->ReportLeafData();
      double min = 1e20,max = 0;
      baltree->ReportVerletRange(min,max);
      printf("Range :  %f   min = %f  max = %f\n",max-min,min,max);
    }

  if(0) // Rebalance of the tree ny load
    {
      tree->UpdateVerletCount(9.0);  
      ORBNode *baltree = BalanceLoad(tree);
      baltree->ReportLeafData();
      printf("tree has %d sites from %d originals\n",baltree->nsites,tree->nsites);

      Site* A = new Site[baltree->nsites];
      Site* B = new Site[baltree->nsites];
      int nA,nB;
      for(int i = 0;i < baltree->nsites;i++)
  {
    Site * a = baltree->GetSite(i);
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


  if(0) // Rebalance of the tree based on midpoints
    {
      //      ORBNode *baltree = InteractionTree(tree,6.0);
      //      baltree->ReportLeafData();
    }

  if(0) // Check for any load zone overlap
    {

      int nx = tree->procnum[tree->internfromext[0]];
      int ny = tree->procnum[tree->internfromext[1]];
      int nz = tree->procnum[tree->internfromext[2]];
      double dl[3];
      int nvox = 0,nranks = 0;
      
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
          ORBNode n1;
          n1.realnode = 1;
          n1.root = tree;
          tree->VoxelBounds(voxnum,n1.low,n1.high);
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

    }
  if(0) // Check voxel neighborhood consistency 
    {

      int nx = tree->voxnum[0];
      int ny = tree->voxnum[1];
      int nz = tree->voxnum[2];
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
    
      int voxid = tree->VoxelId(dl);
      n = 0;
      double D = 6.82;
      //double D = 2.0;
      ORBNode n1,n2;
      n1.realnode = 1;
      n1.root = tree;
      n2.realnode = 1;
      n2.root = tree;
      nvox = 0;
      tree->VoxelBounds(voxid,n1.low,n1.high);
      n1.VoxelsInNeighborhood(D,voxels,nvox, NMAX);
      int ind = tree->VoxelIdToVoxelIndex(voxid);
      printf("voxindex %d  id %08d has  %d nvoxels \n",ind,voxid,nvox); fflush(stdout);

      for(int m=0;m<nvox;m++)
        {
          int voxid2 = voxels[m];
          nvox2 = 0;
          tree->VoxelBounds(voxid2,n2.low,n2.high);
          n2.VoxelsInNeighborhood(D,voxels2,nvox2, NMAX);

          int found = 0;
          for(int mm=0;mm<nvox2;mm++)
      {
        int testvox = voxels2[mm];
        if(testvox == voxid)
          { found = 1; break;}
      }
          assert(found);
        }		  		  
    }
      }
  }
    }
  if(0) // check load zone nieghborhood self consistency 
    {
      //      ORBNode *temptree = stree;
      //            stree = tree;
      int nx = tree->procnum[tree->internfromext[0]];
      int ny = tree->procnum[tree->internfromext[1]];
      int nz = tree->procnum[tree->internfromext[2]];
      
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
    }
  if(0) // check load zone nieghborhood Count
    {
      //      ORBNode *temptree = stree;
      //            stree = tree;
      int nx = tree->procnum[0];
      int ny = tree->procnum[1];
      int nz = tree->procnum[2];
      
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
      
  if (0)
    {
      printf("=====================================\n");
      printf("This :\n");
      ORBNode *leaf = tree->NodeFromProcCoord(0,3,0);
      leaf->ReportLeafData();
      printf("Node A :\n");
      ORBNode *A = tree->NodeFromProcCoord(1,3,1);
      A->low[0] = 7.735373;
      A->ReportLeafData();
      printf("Node B :\n");
      ORBNode *B = tree->NodeFromProcCoord(3,3,0);
      B->low[0] = 23.209762;
      B->ReportLeafData();
      int mine = leaf->CouldMidpointBeMine(B,A, tree->low, tree->high);
      printf("midpoint inside %d \n", mine);
    }
  //tree->Rank(int px,int py, int pz,double r,int *ranks, int &len);

  if(0)
    {
      double v[3];
      int i = 2;
      int j = 0;
      int k = 1;
      int id1 = 5246982;
      int id2 = 6292486;
      ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
      ORBNode n1;
      ORBNode n2;
      n1.realnode = 1;
      n2.realnode = 1;
      leaf->VoxelBounds(id1,n1.low,n1.high);
      leaf->VoxelBounds(id2,n2.low,n2.high);
      printf("Bounds of Node:\n");
      leaf->ReportLeafData();
      printf("Bounds of voxel %d:\n",id1);
      n1.ReportLeafData();
      
      printf("Bounds of voxel %d:\n",id2);
      n2.ReportLeafData();

      leaf->VoxelPoint(id1,v);
      printf("voxelpoint of %08x is  %f %f %f \n",id1,v[0],v[1],v[2]);
      leaf->VoxelPoint(id2,v);
      printf("voxelpoint of %08x is  %f %f %f \n",id2,v[0],v[1],v[2]);
      leaf->GetMidpointOfVoxelPoints( id1, id2, v );
      printf("midpoint is  %f %f %f \n",v[0],v[1],v[2]);
      int midpointid = leaf->VoxelId(v);
      leaf->VoxelPoint(midpointid,v);
      printf("voxel point of midpoint id %08x is %f %f %f\n",midpointid, v[0],v[1],v[2]);

      int inside = leaf->IsVoxelPointOfVoxelPointsMidpointMine(id1,id2);
      printf("Voxelids %d  and %d midpoint inside = %d\n",id1,id2,inside);

      ORBNode *leaf2 = tree->NodeFromProcCoord(v[0],v[1],v[2]);
      printf("Node owning midpoint id %08x is :\n",midpointid);
      leaf2->ReportLeafData();


      double D = 7.41;
      int nvox = 0;
      leaf->VoxelsInNeighborhood(D,voxels,nvox, NMAX);
      for(int m=0;m<nvox;m++)
  {
    int voxid = voxels[m];
    if(voxid == id1)
      {printf(" Found %d %08x in vox %d %d %d neighborhood D = %f\n",voxid,voxid,leaf->meshnum[0],leaf->meshnum[1],leaf->meshnum[2],D);}
    if(voxid == id2)
      {
        printf(" Found %d %08x in vox %d %d %d neighborhood D = %f\n",voxid,voxid,leaf->meshnum[0],leaf->meshnum[1],leaf->meshnum[2],D);
        double vlow[3],vhigh[3];
        leaf->VoxelBounds(id2,vlow,vhigh);
        leaf->NodeMinDeltas(vlow,vhigh,tree->low,tree->high,v);
        double dd = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
        dd = sqrt(dd);
        printf("min deltas %f %f %f D = %f\n",v[0],v[1],v[2],dd);
      }
  }

    }

  if(0) // voxels in sphere
    {
      int nx = tree->NXProcs();
      int ny = tree->NYProcs();
      int nz = tree->NZProcs();
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
      ORBNode n1;
      ORBNode n2;
      n1.realnode = 1;
      n2.realnode = 1;
      if(1)
        {
          n = 0;
          leaf->VoxelsInNeighborhood(15.0,ranks,n,NMAX);
          printf("node %d has  %d neighbors \n",leaf->id,n);
          for(int m=0;m<n;m++)
      {
        int bound = leaf->IsVoxelOnBoundary(ranks[m],15.0,tree->low,tree->high);
        if(ranks[m] == id1 || ranks[m] == id2)
          printf("neighbor [ %d ] = %d  boundary %d\n",m,ranks[m],bound);
      }
        }
    }
      }
  }
    }

  if(0)
    {
      int nVOX = nX * nY * nZ;
      int *voxlist = new int[nVOX];
      int id[DIMS],nv = 0;
      for(int i=0;i<nX;i++)
  {
    id[0] = i;
    for(int j=0;j<nY;j++)
      {
        id[1] = j;
        for(int k=0;k<nZ;k++)
    {
      id[2] = k;
      int vox = tree->VoxelId(id);
      voxlist[nv] = vox;
      nv++;
    }
      }
  }
      for(int i1 = 1; i1 < nVOX; i1++)
  {
    for(int i2 = 0; i2 < nVOX; i2++)
      {
        int claimed = 0;
        int nx = tree->NXProcs();
        int ny = tree->NYProcs();
        int nz = tree->NZProcs();
        for(int i=0;i< nx ; i++)
    {
      for(int j=0;j< ny ; j++)
        {
          for(int k=0;k< nz ; k++)
      {
        ORBNode * leaf = tree->NodeFromProcCoord(i,j,k);
        claimed += leaf->IsMidpointOfVoxelMidpointsMine(voxlist[i1],voxlist[i2]);
      }
        }
    }
        if(!claimed)
    {
      printf("voxid %d %d is unclaimed\n",voxlist[i1],voxlist[i2]);
      ORBNode n1;
      ORBNode n2;
      int id1 = voxlist[i1];
      int id2 = voxlist[i2];
      n1.realnode = 1;
      n2.realnode = 1;
      tree->VoxelBounds(id1,n1.low,n1.high);
      tree->VoxelBounds(id2,n2.low,n2.high);
      printf("Bounds of voxel %d:\n",id1);
      n1.ReportLeafData();
      
      printf("Bounds of voxel %d:\n",id2);
      n2.ReportLeafData();
      
      double v[DIMS];
      double vec1[DIMS],vec2[DIMS],mid[DIMS],*vl,*vh;
      for(int i=0;i<DIMS;i++)
        {
          double dV = tree->high[i] - tree->low[i];
          vec1[i] = 0.5 * (n1.low[i] + n1.high[i]);
          vec2[i] = 0.5 * (n2.low[i] + n2.high[i]);
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
          if( mid[i] < 0.0)
      mid[i] += dV;
          if(mid[i] >= tree->high[i])
      mid[i] = 0.0;
        }
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
    }
  if(0)
    {



      int xMy = 0;
      int yMy = 0;
      int zMy = 0;
      ORBNode * mRecBisTree = tree;
      ORBNode* MyNode = tree->NodeFromProcCoord( xMy, yMy, zMy );
      int   mMaxVoxelsInRange = 512;
      int* MyVoxels = (int*) malloc( 2 * mMaxVoxelsInRange * sizeof(int));
      assert( MyVoxels != NULL );
          
      int NumberOfMyVoxels=0;
      int NodeVoxels[10000];
      int nnodevox = 0;
          
      MyNode->VoxelsInNeighborhood( 0, MyVoxels, NumberOfMyVoxels,  2 * mMaxVoxelsInRange );
      double MyWeight=0.0;
      for( int i=0; i<MyNode->nsites; i++ )
  {
    Site * t = MyNode->GetSite(i);
    MyWeight += t->verlet_count;
    printf("site %d (local %d) [ %f %f %f ] weight %f\n",t->id,i,t->c[0],t->c[1],t->c[2],t->verlet_count);
    int Voxel = MyNode->VoxelId(t->c);
    NodeVoxels[nnodevox] = Voxel;
    nnodevox++;
  }
      MyNode->ReportLeafData();
      printf("weight = %f\n",MyWeight);

      qsort(NodeVoxels,nnodevox,sizeof(int),CompareInt);
      for( int i=0; i<nnodevox; i++ )
  printf("NodeVoxel %08X\n",NodeVoxels[i]);
      for(int i = 0 ; i< NumberOfMyVoxels ; i++)
  {
    int VoxelId = MyVoxels[i];
    if( MyNode->IsMidpointOfVoxelMidpointsMine( VoxelId, VoxelId ) )
      {
        printf(" Neighbor: %08X\n",VoxelId);
        
      }
  }


//       for( int i=0; i<NumberOfMyVoxels; i++ )
// 	{
// 	  int VoxelId = MyVoxels[i];
    
// 	  if( MyNode->IsMidpointOfVoxelMidpointsMine( VoxelId, VoxelId ) )
// 	    {    
              
// 	      //	      int VoxelIndex = mRecBisTree->VoxelIdToVoxelIndex( VoxelId );
        
// 	      MyWeight += OrbInput[ VoxelIndex ].mWeight;
        
// 	      double tmp[3];
// 	      tmp[0] = OrbInput[ VoxelIndex ].mPoint.mX;
// 	      tmp[1] = OrbInput[ VoxelIndex ].mPoint.mY;
// 	      tmp[2] = OrbInput[ VoxelIndex ].mPoint.mZ;
        
// 	      int VoxelIdOfPoint = mRecBisTree->VoxelId( tmp );
              
// 	      assert( VoxelIdOfPoint == VoxelId );
// 	    }
// 	}
      
//       double OrbWeight = MyNode->verlet_sum;
      
//       BegLogLine( 1 )
// 	<< "MyWeight= " << MyWeight
// 	<< " OrbWeight= " << OrbWeight
// 	<< " Diff= " << MyWeight-OrbWeight
// 	<< EndLogLine;
    }



  if(0) // Node retreival by proc address
    {

      
      int nx = stree->procnum[stree->internfromext[0]];
      int ny = stree->procnum[stree->internfromext[1]];
      int nz = stree->procnum[stree->internfromext[2]];
      for(int i=0;i< nx ; i++)
  {
    for(int j=0;j< ny ; j++)
      {
        for(int k=0;k< nz ; k++)
    {
      ORBNode * leaf = stree->NodeFromProcCoord(i,j,k);
      assert(leaf); 
      printf("Ext %d %d %d : ",i,j,k);
      leaf->ReportLeafData();		  
    }
      }
  }
    }


  if(0)
    {
      //      ORBNode *temptree = stree;
      //            stree = tree;
      int nx = tree->NXProcs();
      int ny = tree->NYProcs();
      int nz = tree->NZProcs();
      int NNodes = tree->NXProcs() * tree->NYProcs() * tree->NZProcs();
      int NVox = tree->voxnum[0] *tree->voxnum[1] *tree->voxnum[2];

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
      pc[ tree->extfromintern[ 0 ] ] = i;
      pc[ tree->extfromintern[ 1 ] ] = j;
      pc[ tree->extfromintern[ 2 ] ] = k;
      ORBNode * leaf = tree->NodeFromProcCoord(pc[0],pc[1],pc[2]);

      n = 0;
      double D = 6.82;
      //double D = 1.0;
      nranks = 0;
      double vlow[3],vhigh[3];
      for(int ii=0;ii< DIMS;ii++)
        {
          vlow[ii] = leaf->low[ii] - D;
          vhigh[ii] = leaf->high[ii] + D;
        }
      tree->RanksInBox(pc[0],pc[1],pc[2],D,D,D, Ranks, nranks, NNodes); 
      //leaf->RanksInLoadBox(vlow,vhigh,Voxels,nvox,NVox, Ranks, nranks, NNodes); 
      printf("node %d %d %d has  %d ranks %d voxels ",leaf->meshnum[0],leaf->meshnum[1],leaf->meshnum[2],nranks,nvox);
      //		  leaf->ReportLeafData();
      for(int m=0;m<nranks;m++)
        {
          ORBNode * nd = (ORBNode *) Ranks[m];
          printf("Neighbor %03d.[%03d] = %03d\n",leaf->id,m,nd->id);
          int nranks2 = 0, nvox2 = 0;
          double vvlow[3],vvhigh[3];
          for(int ii=0;ii< DIMS;ii++)
      {
        vvlow[ii] = nd->low[ii] - D;
        vvhigh[ii] = nd->high[ii] + D;
      }
          int nc[3];
          nc[ tree->extfromintern[ 0 ] ] = nd->meshnum[0];
          nc[ tree->extfromintern[ 1 ] ] = nd->meshnum[1];
          nc[ tree->extfromintern[ 2 ] ] = nd->meshnum[2];

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
        printf("Failed to find %d [%d %d %d ]in neighborlist of %d [ %d %d %d ]\n",leaf->id, leaf->meshnum[0], leaf->meshnum[1] ,leaf->meshnum[2] ,nd->id, nd->meshnum[0] ,nd->meshnum[1] ,nd->meshnum[2] );
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
