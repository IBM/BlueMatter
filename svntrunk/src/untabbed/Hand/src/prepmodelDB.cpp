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
#include <HandModel.hpp>


int main(int ac, char **av)
{
    if (ac < 4)
    {
      printf("%s <bvh file> <model db fname>  <conf file name> \n",av[0]);
      printf("Reads file with nnewstates and ref_state as first two vals,\n");
      printf("followed by <grab code> data\n");
      printf("Grab codes:\n");
      printf("\t0: uniform curl < curl factor>\n");
      printf("\t1: differential curl <curl factor>\n");
      return 1;
    }
   
    FILE *cPF = fopen("last_command.prepmodelDB.dat","a");
    for(int i = 0;i<ac;i++){fprintf(cPF," %s ",av[i]);}
    fclose(cPF);

    int ref_state,state,grabcode;
    double curl_factor;
    char bvhname[256];
    char modname[256];
    char confname[256];
    Hand hnd;
    char pname[256];
    int tstate,nnew,nstates,start;

    int writedatasingleline = 0;
    sscanf(av[1],"%s",bvhname);
    sscanf(av[2],"%s",modname);
    sscanf(av[3],"%s",confname);

    FILE *modPF = fopen(modname,"w");
    if(modPF == NULL)
      {
  printf("Can't open model output file %s\n",modname);
  return 1;
      }
    FILE *fPF = fopen(confname,"r");
    if(fPF == NULL)
      {
  printf("Can't open conformation space file %s\n",confname);
  return 1;
      }

    FILE *bvhPF = fopen(bvhname,"r");
    hnd.ReadBVHDef(bvhPF);
    

    state = start = hnd.m_bvh.m_nframes;

    // Header is number of states to add, and reference state
    int hd = fscanf(fPF,"%d %d",&nnew,&ref_state);
    assert(hd == 2);

    nstates = hnd.AddStates(nnew);

    while(!feof(fPF))
      {
  hnd.CopyState(ref_state,state);
  hnd.SetState(state);
  if(1 == fscanf(fPF,"%d",&grabcode))
    {
      switch(grabcode)
        {	
        case 1:
    fscanf(fPF,"%lf",&curl_factor);
    hnd.CurlFinger("Pinky",state,2*curl_factor);
    hnd.CurlFinger("Ring",state,1.5*curl_factor);
    hnd.CurlFinger("Middle",state,curl_factor);
    hnd.CurlFinger("Index",state,0.5*curl_factor);
    hnd.CurlFinger("Thumb",state,0.25*curl_factor);
    hnd.SetState(state);
    break;
        case 0:
    fscanf(fPF,"%lf",&curl_factor);
    hnd.UniformGrasp(state,curl_factor);
    break;
        default:
    assert(0);
    break;
        }
      hnd.m_bvh.WriteState(state,modPF);
      state++;
      if(state == hnd.m_bvh.m_nframes)
        break;
    }
      }
    fclose(fPF);
    fclose(modPF);
}
