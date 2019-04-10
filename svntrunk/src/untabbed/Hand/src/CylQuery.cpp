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
#define MAX_FEATURES_PER_SNAKE 500000


#include <PovOut.hpp>
#include <CSGModel.hpp>
#include <HandFeatureExtractor.hpp>


int ConfigureQuery(char *csgname,char *objname,Hand &hnd, int Nr,int Nh)
{
   Sphere *sph;
   Cylinder cyl;
   int n;

   FILE *fPF = fopen(csgname,"r");
   cyl.Read(fPF);
   fscanf(fPF,"%d",&n);
   for(int i = 0;i<n;i++)
     {
       sph = new Sphere();
       sph->Read(fPF);
       hnd.AddSelection1Sphere(sph);
     }
   fscanf(fPF,"%d",&n);
   for(int i = 0;i<n;i++)
     {
       sph = new Sphere();
       sph->Read(fPF);
       hnd.AddSelection2Sphere(sph);
     }
   fscanf(fPF,"%d",&n);
   for(int i = 0;i<n;i++)
     {
       sph = new Sphere();
       sph->Read(fPF);
       hnd.AddSelection3Sphere(sph);
     }
   
   FILE *objPF = fopen(objname,"w");
   cyl.AddSurfacePoints("",Nr,Nh,0,objPF);
   
   fclose(objPF);
   objPF = fopen(objname,"r");
   
   PoserObject pobj(objPF);
   fclose(objPF);
   hnd.m_pcur = pobj;
   hnd.m_bvh.m_root.AddCylinder(cyl);
   hnd.SetState(0);
   strcat(objname,".pov");
   objPF = fopen(objname,"w");
   pobj.WritePovrayFaces(objPF,1);
   fclose(objPF);
   return 1;
}

int main( int ac , char **av)
{
    if (ac < 7)
    {
      printf("%s <model db fname> <feature db name> <csgfile> <objfile> <Nr> <Nh> \n",av[0]);
      return 1;
    }
    Hand hnd;
     int Nr,Nh,n;

    char modname[256];
    char featname[256];
    char csgname[256];
    char objname[256];

    sscanf(av[1],"%s",modname);
    sscanf(av[2],"%s",featname);
    sscanf(av[3],"%s",csgname);
    sscanf(av[4],"%s",objname);
    sscanf(av[5],"%d",&Nr);
    sscanf(av[6],"%d",&Nh);


    PV.write_selected1 = 1;
    PV.write_selected2 = 1;
    PV.write_selected3 = 1;
    PV.write_exposed = 1;
    PV.write_features = 0;


    FILE *cPF = fopen("last_command.cylquery.dat","a");
    for(int i = 0;i<ac;i++){fprintf(cPF," %s ",av[i]);}
    fclose(cPF);

    FILE *modPF = fopen(modname,"w");
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
 
    ConfigureQuery(csgname,objname,hnd,Nr,Nh);    

    HandFeatureExtractionManager hndman;

//     Sphere sph[6];
//     if(0)
//       {
// 	// palm
// 	sph[0].m_pos = Vector( -0.7, 2.6, 1.2);    sph[0].m_rad = 1.9;
// 	// fingers pinky to index
// 	sph[1].m_pos = Vector(-2.5, 5.5, 2.0);    sph[1].m_rad = 1.2;
// 	sph[2].m_pos = Vector(-1.5, 5.5, 2.0);    sph[2].m_rad = 1.2;
// 	sph[3].m_pos = Vector( 0.0, 5.5, 2.0);    sph[3].m_rad = 1.2;
// 	sph[4].m_pos = Vector( 1.2, 6.3, 2.3);    sph[4].m_rad = 0.8;
// 	// thumb
// 	sph[5].m_pos = Vector( 0.8, 3.0, 3.9);    sph[5].m_rad = 1.1;
//       }
//     else if (0)
//       {
// 	sph[0].m_pos = Vector( -0.7, 2.6, 1.2);    sph[0].m_rad = 1.9;
// 	sph[1].m_pos = Vector(-2.5, 3.5, 2.0);    sph[1].m_rad = 1.2;
// 	sph[2].m_pos = Vector(-1.5, 4.5, 2.0);    sph[2].m_rad = 1.2;
// 	sph[3].m_pos = Vector( 0.0, 5.5, 2.0);    sph[3].m_rad = 1.2;
// 	sph[4].m_pos = Vector( 1.2, 6.3, 2.3);    sph[4].m_rad = 0.8;
// 	sph[5].m_pos = Vector( 0.8, 3.0, 3.9);    sph[5].m_rad = 1.1;
//       }
//     else
//       {
// 	sph[0].m_pos = Vector( 10.0, 2.0, -1.0);    sph[0].m_rad = 1.5;
// 	sph[1].m_pos = Vector( 9.5, 1.0, 0.5);    sph[1].m_rad = 1.5;
// 	sph[2].m_pos = Vector( 9.5, 2.0, 0.5);    sph[2].m_rad = 1.5;
// 	sph[3].m_pos = Vector( 9.5, 3.0, 0.5);    sph[3].m_rad = 1.5;
// 	sph[4].m_pos = Vector( 9.5, 4.0, 0.5);    sph[4].m_rad = 1.5;
// 	sph[5].m_pos = Vector( 11.0, 4.0, 0.0);    sph[5].m_rad = 1.5;
//       }

//     hnd.AddSelection2Sphere(&sph[1]);
//     hnd.AddSelection2Sphere(&sph[2]);
//     hnd.AddSelection2Sphere(&sph[3]);
//     hnd.AddSelection2Sphere(&sph[4]);
//     hnd.AddSelection3Sphere(&sph[5]);
    //    hnd.DefineSurface(objname);

    hndman.SetOutputFiles(modPF, featPF);
    if(!hndman.ExtractHandFeatures(&hnd,1))
      {
  fprintf(stderr,"Feature extraction failed\n");
      }

}
