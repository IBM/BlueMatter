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
#include <math.h>
#include <assert.h>


#define FULL_FEATURE_SPACE 1
#define ANGS_PER_SEGMENT 4
#define MAX_FEATURES_PER_SNAKE 500000

#include <PovOut.hpp>
#include <handhypo.hpp>
#include <HandFeatureExtractor.hpp>

FILE *FeatureDB;
FILE *ModelDB;
FILE *QueryFeatureDB;
FILE *QueryModelDB;

Hand * ConfigureQuery(char *csgname, int Nr, int Nh)
{
    FILE *fPF = fopen(csgname,"r");
    Cylinder cyl;
    cyl.Read(fPF);
    
    char objname[256];
    strcpy(objname,csgname);
    strcat(objname,".obj");
    FILE *objPF = fopen(objname,"w");
    cyl.AddSurfacePoints("",Nr,Nh,0,objPF);

    fclose(objPF);
    objPF = fopen(objname,"r");

    Hand *hnd = new Hand();
    PoserObject pobj(objPF);
    fclose(objPF);
    hnd->m_pcur = pobj;
    hnd->m_bvh.m_root.AddCylinder(cyl);
    hnd->SetState(0);
    strcat(objname,".pov");
    objPF = fopen(objname,"w");
    pobj.WritePovrayFaces(objPF,1);
    fclose(objPF);
    return hnd;
}

Hand * ConfigureRefModel(char *name)
{
  char bvhname[256],objname[256];
  // Create reference model
  strcpy(bvhname,name);
  strcat(bvhname,".bvh");
  strcpy(objname,name);
  strcat(objname,".obj");
  Hand * hnd = new Hand();
  FILE *bvhPF = fopen(bvhname,"r");
  hnd->ReadBVHDef(bvhPF);
  fclose(bvhPF);
  hnd->m_bvh.SetState(0);
  hnd->DefineSurface(objname,2.0);
  return hnd;
}

int WriteResult(Hand *hnd,  Hand_Transformation & hyp, FILE *out)
{
  Vector T;
  Matrix R;
  hyp.getTranslation(T);
  hyp.getRotation(R);
  hnd->SetState(hyp.d_modelId);      
  PoserObject mod;
  mod = hnd->m_pcur;
  mod.Move(R,T);
  fprintf(out,"// ");
  hyp.Write(out);
  mod.WritePovrayFaces(out);  
  return 1;
}

int CheckIntersection(Sphere *qsphmap,int nqspheres,Cylinder *qcylmap,int nqcyls,Matrix &R,Vector &T,Sphere *sphmap,int nspheres,Cylinder *cylmap,int ncyls)
{
  for(int i = 0; i < ncyls ; i++)
    {
      cylmap[i].Move(R,T);
      for(int j = 0;j < nqcyls; j++)
  {
    if(cylmap[i].DoesCapsuleIntersectCapsule(qcylmap[j]))
      return 1;
  }
      for(int j = 0;j < nqspheres; j++)
  {
    if(cylmap[i].DoesCapsuleIntersectSphere(qsphmap[j]))
      return 1;
  }
    }
  for(int i = 0; i < nspheres ; i++)
    {
      sphmap[i].Move(R,T);
      for(int j = 0;j < nqcyls; j++)
  {
    if(qcylmap[j].DoesCapsuleIntersectSphere(sphmap[i]))
      return 1;
  }
      for(int j = 0;j < nqspheres; j++)
  {
    if(sphmap[i].DoSpheresIntersect(qsphmap[j]))
      return 1;
  }
    }
  return 0;
}
int main(int ac, char ** av)
{

  if (ac < 8)
    {
      printf("%s <model dbname> <feature dbname> <query model dbname> <query feature dbname> <Nr> <Nh>  <min score> \n", av[0]);
      return 1;
    }
    FILE *cPF = fopen("last_command.grab.dat","a");
    for(int i = 0;i<ac;i++){fprintf(cPF," %s ",av[i]);}
    fprintf(cPF,"\n");
    fclose(cPF);


  char *conf = av[1];
  char modS[256], qmodS[256];
  char featS[256], qfeatS[256];
  Hand *sn, *csl, *root;
  int min_score,pov=0,Nr,Nh;

  char bvhname[256];
  char objname[256];


  sscanf(av[1],"%s",modS);
  sscanf(av[2],"%s",featS);
  sscanf(av[3],"%s",qmodS);
  sscanf(av[4],"%s",qfeatS);
  sscanf(av[5],"%d",&Nr);
  sscanf(av[6],"%d",&Nh);
  sscanf(av[7],"%d",&min_score);

  Hand *hand = ConfigureRefModel("hand2");
  Hand *qhand = ConfigureQuery(qmodS,Nr,Nh);

  int POVOUT = 0;
  int writedatasingleline = 0;
  FILE *singlelinedataPF = NULL;

  PV.write_selected1 = 1;
  PV.write_selected2 = 1;
  PV.write_selected3 = 1;
  PV.write_exposed = 1;
  PV.write_features = 0;
    

  if (NULL == (FeatureDB = fopen(featS,"r")))
    {
      printf("Could not open feature database %s\n",featS);
      return 1;
    }
  if (NULL == (ModelDB = fopen(modS,"r")))
    {
      printf("Could not open model database %s\n",modS);
      return 1;
    }
  if (NULL == (QueryFeatureDB = fopen(qfeatS,"r")))
    {
      printf("Could not open query feature database %s\n",qfeatS);
      return 1;
    }
//   if (NULL == (QueryModelDB = fopen(qmodS,"r")))
//     {
//       printf("Could not open query model database %s\n",qmodS);
//       return 1;
//     }
  FeatureHypoManager hm; 

  HandFeature *queryfeatures=NULL; 

  while(!feof(QueryFeatureDB))
    {
      HandFeature feat;
      if(feat.Read(QueryFeatureDB))
  {
    HandFeature *sfeat = new HandFeature;
    assert(sfeat != NULL);
    *sfeat = feat;
    sfeat->next = queryfeatures;
    queryfeatures = sfeat;
  }
    }

  hm.SetOpContext(FeatureDB,ModelDB,hand,min_score);
  HandFeature *query = queryfeatures;

  while(query != NULL)
    {
      if(query->key > 0)
  hm.AddInputItem(query);
      query = query->next;
    }
  hm.FinalizeProcessing();

  Hand_Transformation  hyp, fhyp[1000];

  FILE *hyposPF = fopen("Hypos.dat","w");
  FILE *out = stdout;
  
  int bestscore = 0;
  int besthyp = 0;
  int nkeep = 0;
  int nspheres,ncyls,nqspheres,nqcyls;
  Sphere *sphmap,*qsphmap;
  Cylinder *cylmap,*qcylmap;
  BVHJoint *mroot,*qroot;

  mroot = &hand->m_bvh.m_root;
  qroot = &qhand->m_bvh.m_root;
  mroot->CountExcludedObjects(&nspheres, &ncyls);
  qroot->CountExcludedObjects(&nqspheres, &nqcyls);
  sphmap = (nspheres > 0) ? new Sphere[nspheres] : NULL;
  qsphmap = (nqspheres > 0) ? new Sphere[nqspheres] : NULL;
  cylmap = (ncyls > 0 ) ? new Cylinder[ncyls] : NULL;
  qcylmap = (nqcyls > 0) ? new Cylinder[nqcyls] : NULL;

  qroot->ResetExcludedSphereArray(qsphmap,0, nqspheres);
  qroot->ResetExcludedCylinderArray(qcylmap,0, nqcyls);

  while(hm.GetNextOutputItem(&hyp))
    {
      Vector T;
      Matrix R;
      hyp.getTranslation(T);
      hyp.getRotation(R);

      //      Hand *omod = hm.GetModel(hyp.d_modelId);
      hand->SetState(hyp.d_modelId);      
      mroot->ResetExcludedSphereArray(sphmap,0, nspheres);
      mroot->ResetExcludedCylinderArray(cylmap,0, ncyls);

      //      if(!qhand->m_bvh.m_root.JointIntersection(&hand->m_bvh.m_root))

      if(!CheckIntersection(qsphmap,nqspheres,qcylmap,nqcyls,R,T,sphmap,nspheres,cylmap,ncyls))
  {
    if(bestscore < hyp.score)
      {
        bestscore = hyp.score;
        besthyp = nkeep;
      }
    hyp.Write(hyposPF);
    fhyp[nkeep] = hyp;
    nkeep++;
  }
      else
  {
    //	  fprintf(out,"// Hand intersection: ");
    //hyp.Write(out);
  }
    }

  if(nkeep > 0)
    WriteResult(hand,fhyp[besthyp],out);

  hm.ClearOpContext();
  fclose(FeatureDB);
  fclose(ModelDB);
  fclose(QueryFeatureDB);
  //  fclose(QueryModelDB);
  fclose(hyposPF);
}
