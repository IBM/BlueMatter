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
#include <string.h>
#include <assert.h>
#include <math.h>


#define FULL_FEATURE_SPACE 0
#define ANGS_PER_SEGMENT 8
#define MAX_FEATURES_PER_SNAKE 100000

#include <PovOut.hpp>
#include <HandFeatureExtractor.hpp>

int ClearSelections(Hand &hnd)
{
  Sphere *tsph,*sph;
  sph = hnd.m_selection1;
  while(sph != NULL)
    {
      tsph = sph->m_next;
      delete sph;
      sph = tsph;
    }
  hnd.m_selection1 = NULL;
  sph = hnd.m_selection2;
  while(sph != NULL)
    {
      tsph = sph->m_next;
      delete sph;
      sph = tsph;
    }
  hnd.m_selection2 = NULL;
  sph = hnd.m_selection3;
  while(sph != NULL)
    {
      tsph = sph->m_next;
      delete sph;
      sph = tsph;
    }
  hnd.m_selection3 = NULL;
  return 1;
}
int AddSelectionSphere(Hand &hnd,Sphere *sph,int sel)
{
  switch(sel)
    {
    case 1:
      hnd.AddSelection1Sphere(sph); break;
    case 2:
      hnd.AddSelection2Sphere(sph); break;
    case 3:
      hnd.AddSelection3Sphere(sph); break;
    }
  return 1;
}
int ConfigTipSphere(Hand &hnd,char *name,int sel)
{
  Sphere *sph;
  char tname[256];
  double tipsel = 0.3;
  double dz = 0.5;
  double dy;

  sprintf(tname,"%sMid",name);
  BVHJoint *doob1,*doob2;

  doob1 = hnd.m_bvh.GetJoint(tname);
  assert(doob1);
//   sph = new Sphere();
//   sph->m_pos = doob1->m_pos + (Vector(0.0,0.0,dz) / doob1->m_R);
//   sph->m_rad = tipsel;
  //  AddSelectionSphere(hnd,sph,sel);

  doob2 = &doob1->m_joint[0];
  sph = new Sphere();
  sph->m_pos = doob1->m_pos + 0.5*(doob2->m_pos - doob1->m_pos) + (Vector(0.0,0.0,dz) / doob1->m_R);
  sph->m_rad = tipsel;
  AddSelectionSphere(hnd,sph,sel);
  


  sprintf(tname,"%sTip",name);
  doob1 = hnd.m_bvh.GetJoint(tname);
  assert(doob1);
//   sph = new Sphere();
//   sph->m_pos = doob1->m_pos + (Vector(0.0,0.0,0.5) / doob1->m_R);
//   sph->m_rad = tipsel;
//   AddSelectionSphere(hnd,sph,sel);

  doob2 = &doob1->m_joint[0];
  sph = new Sphere();
  sph->m_pos = doob1->m_pos + 0.5*(doob2->m_pos - doob1->m_pos) + (Vector(0.0,0.0,dz) / doob1->m_R);
  sph->m_rad = tipsel;
  AddSelectionSphere(hnd,sph,sel);

  sph = new Sphere();
  doob2 = &doob1->m_joint[0];
  assert(doob2);
  sph->m_pos = doob2->m_pos + (Vector(0.0,0.0,0.5) / doob2->m_R);
  sph->m_rad = tipsel;
  AddSelectionSphere(hnd,sph,sel);

  return 1;
}
int SetTipSelections(Hand &hnd)
{
  Sphere *sph;

  sph = new Sphere();
  BVHJoint *doob1,*doob2;

  //Set Palm Base
  doob1 = hnd.m_bvh.GetJoint("PalmCenter");
  sph->m_pos = doob1->m_pos + ( Vector( -0.7, 1.6, 1.2) / doob1->m_R );
  sph->m_rad = 1.9;
  hnd.AddSelection1Sphere(sph);

  ConfigTipSphere(hnd,"Pinky",2);
  ConfigTipSphere(hnd,"Ring",2);
  ConfigTipSphere(hnd,"Middle",2);
  ConfigTipSphere(hnd,"Index",2);

  ConfigTipSphere(hnd,"Thumb",3);
  return 1;
}


int main(int ac, char **av)
{
    if (ac < 4)
    {
      printf("%s <bvh file>  <model db fname> <feature db name> <povout> \n",av[0]);
      return 1;
    }
   
    FILE *cPF = fopen("last_command.prepfeaturedb.dat","a");
    for(int i = 0;i<ac;i++){fprintf(cPF," %s ",av[i]);}
    fclose(cPF);

    int ref_state,state,povout;
    double curl_factor;
    char bvhname[256];
    char objname[256];
    char modname[256];
    char featname[256];

    int writedatasingleline = 0;
    sscanf(av[1],"%s",bvhname);
    sscanf(av[2],"%s",modname);
    sscanf(av[3],"%s",featname);
    sscanf(av[4],"%d",&povout);
    
    strcpy(objname,bvhname);
    strcat(objname,".obj");
    
    PV.write_selected1 = 1;
    PV.write_selected2 = 1;
    PV.write_selected3 = 1;
    PV.write_exposed = 1;
    PV.write_features = 0;

    
    FILE *modPF = fopen(modname,"r");
    if(modPF == NULL)
      {
  printf("Can't open model output file %s\n",modname);
  return 1;
      }
    FILE *featPF = fopen(featname,"w");
    if(featPF == NULL)
      {
  printf("Can't open feature output file %s\n",featname);
  return 1;
      }
    int POVOUT = 0;
    FILE *singlelinedataPF = NULL;
    
    Hand hnd;
    HandFeatureExtractionManager hndman;
    char pname[256];

    FILE *bvhPF = fopen(bvhname,"r");
    hnd.ReadBVHDef(bvhPF);
    fclose(bvhPF);

    hnd.m_bvh.SetState(0);
    hnd.DefineSurface(objname);

    hndman.SetOutputFiles(NULL, featPF);

//     int tstate;
//     state = hnd.AddStates(1) - 1;
//     hnd.CopyState(ref_state,state);

    if(writedatasingleline)
      {	    
  singlelinedataPF = fopen("FeatureData.dat","w");
  HandFeature f;
  hndman.writedatasingleline = writedatasingleline;
  hndman.singlelinedataPF = singlelinedataPF;	  
  f.WriteSingleLine(singlelinedataPF,1);
      }
    
    while(hnd.m_bvh.ReadState(&state,modPF))
      {
  hnd.SetState(state);
  SetTipSelections(hnd);        

  if(povout)
    {
      sprintf(pname,"%s.state_%d.pov",modname,state);
      FILE *outPF = fopen(pname,"w");
      if (1)
        hnd.m_bvh.WritePovray(outPF);
      if (0)
        hnd.m_pcur.WritePovrayFaces(outPF);
      fclose(outPF);
    }

  hndman.ExtractHandFeatures(&hnd,0);
  ClearSelections(hnd);
      }


    fclose(featPF);
    fclose(modPF);

    return 0;
}

