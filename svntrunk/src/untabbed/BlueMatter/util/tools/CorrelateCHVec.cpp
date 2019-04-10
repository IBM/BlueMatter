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

double timescale = 1.0;
int DoColumn(int col,double *x,double *y, double *z, double *c);
int BufferFile(char* fname);
char *  filetxt[100000]; 
char *  textcurs[100000]; 

int NLines = 0;

int main(int ac, char **av)
{
  char fname[256];
  int col;

  if(ac < 3)
    {
      printf("usage: %s <fname> <col> [timescale in ps (default=1.0)]\n",*av);
      exit(-1);
    }

  sprintf(fname,"%s",av[1]);
  sscanf(av[2],"%d",&col);
  if(ac > 3)
    {
      sscanf(av[3],"%lf",&timescale);
    }

  double x[100000],y[100000],z[100000],c[100000],corr[100000];
  
  BufferFile(fname);
  int N = DoColumn(1,x,y,z,c);
  for(int i = 0; i < N ; i++)
    {
      corr[i] = c[i];
    }
  for(int j = 2; j <= col ; j++)
    {
      //       printf("Col %d\n",j);
      DoColumn(j,x,y,z,c);
      for(int i = 0; i < N ; i++)
  {
    corr[i] += c[i];
  }
    }
  for(int i = 0; i < N ; i++)
    {
      corr[i] /= col;
      printf("%f %f\n",(i * timescale), corr[i]);
    }
  
  
  
}

int BufferFile(char *fname)
{
  char bufS[100000];
  int l;

  FILE *fPF = fopen(fname,"r");
  assert(fPF != NULL);  
  fgets(bufS,100000,fPF);
  l = strlen(bufS);
  while(!feof(fPF))
    {
      filetxt[NLines] = new char[l+256];
      strcpy(filetxt[NLines],bufS);
      textcurs[NLines] = filetxt[NLines];
      NLines++;
      fgets(bufS,100000,fPF);
    }
  fclose(fPF);
  return NLines;
}
  
int DoColumn(int col,double *x,double *y, double *z, double *c)
{
  char bufS[10000],format[10000],tbufS[256];
  int i,j,n;
  n = NLines;
  FILE *fPF;
  
  format[0] = '\0';
  //  strcpy(tbufS," %*s %*s %*s ");
  //  for(int i = 0 ; i < col - 1; i++)
  //  {
  //    strcat(format,tbufS);
  //  }
  strcat(format," %lf %lf %lf %n");
  
  for(int lin = 0;lin < NLines; lin++)
    {
      sscanf(textcurs[lin],format,&x[lin],&y[lin],&z[lin],&j);
      textcurs[lin]+= j;  
    }
  
  
  for(int dt = 0;dt < n; dt++)
    {
      double sum = 0.0;
      double count = 0.0;
      for(int i = 0; i < n - dt; i++)
  {
    int j = i + dt;
    double d = x[i]*x[j] + y[i]*y[j] + z[i]*z[j];
    //	  double dI = sqrt(x[i]*x[i] + y[i]*y[i] + z[i]*z[i]);
    //	  double dJ = sqrt(x[j]*x[j] + y[j]*y[j] + z[j]*z[j]);
    //	  d /= dI * dJ;
    sum +=   1.5 * d * d - 0.5;
    count++;
  }
      //      sum /= count;
      sum /= n;
      c[dt] = sum;
    }
  return n;
}



