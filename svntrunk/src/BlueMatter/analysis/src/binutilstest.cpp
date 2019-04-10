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
 #include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <binutils.hpp>

#include <BlueMatter/binutils.hpp>

double decay_factor = 0.5;

int ComputeMeanStats(hist::Histogram &histy,int NBins, std::vector<double> &means, double &ax, double &axx);


int main(int ac, char ** av)
{
  int i,j,n;

  char fname[256],bufS[1024];
  if(ac < 4)
    {
      printf("usage: %s <datafile> <column> <line to start reading> \n",*av);
      exit(1);
    }

  strcpy(fname,av[1]);
  n = 0;
  int column;
  int nstart = 1;
  sscanf(av[2],"%d",&column);
  sscanf(av[3],"%d",&nstart);

  char format[256];
  char formatH[256];
  format[0] = '\0';
  for(i =0; i< column;i++)
    {
      strcat(format," %*s");
    }
  strcpy(formatH,format);
  strcat(format," %lf");
  strcat(formatH," %s");
  char heading[256];
  //  sscanf(av[3],"%d",&finBin);

  hist::Histogram histy;

  FILE *fPF = fopen(fname,"r");
  assert(fPF != NULL);
  fgets(bufS,1000,fPF);
  sscanf(bufS,formatH,heading);
  double min,max;
  int flg = 1;


  while(!feof(fPF))
  {
    double val;
    fgets(bufS,1000,fPF);
    n++;
    if(n < nstart)
      {continue; }

    sscanf(bufS,format,&val);
    histy.AddValue(val);
  }
  fclose(fPF);
  
  int NBins,nn;
  
  histy.TrimDataToPowerOf2();
  NBins = histy.NValues();
  
  printf("# binsize   mean  Sigma  -- for =>  %s \n",heading);
  while(NBins > 2) 
    {
      double ax, axx;
      NBins >>= 1;// 1 << binexp;

      //      binexp--;

      // Adjust Bins
      std::vector < double >  means;
      means.reserve(NBins);
      histy.Adjust(NBins);

      //  Set Bin Boundaries
      //  Bin Data
      histy.UpdateBins();
    }
  return 0;

}


