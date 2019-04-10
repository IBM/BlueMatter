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
#include <math.h>
#include <assert.h>

double timescale = 0.002;

#define MAXLINES 10000000
char *  filetxt[MAXLINES]; 


int NLines = 0;
int NBins = 21;


int DoColumn(int col, double *c);
int BufferFile(char* fname,int col,double *x);
double Mean(double * x,int start, int n,int skip);
double Var(double * x, double mean,int start, int n, int skip);
double AutoCov(double * x, double mean,int start, int n, int skip);
void Histogram(double *x, int N, int NBins, double *upbound, int *bin,int skip);
void  ConfigureBins(double mean,double var,int nbins,double *upbounds);
void Output(FILE *fPF,double * x,double mean,int start, int n, int skip);
void OutputKappa(double T,double * x,double mean,int start, int n, int skip);


int Start = 0;
int NReadLines = -1;
char histname[100];
int DoHist = 0;
int DoCorr = 0;

int main(int ac, char **av)
{
  char fname[256];
  double temp = 310.65;
  int col;
  int skip = 1;
  if(ac < 3)
    {
      printf("usage: %s <fname> <col> <temp> [start 0] [N lines] [skip (1)]  \n",*av);
      exit(-1);
    }

  sprintf(fname,"%s",av[1]);
  sscanf(av[2],"%d",&col);
  if(ac > 3)
    {
      sscanf(av[3],"%lf",&temp);
    }

  if(ac > 4)
    {
      sscanf(av[4],"%d",&Start);
    }

  if(ac > 5)
    {
      sscanf(av[5],"%d",&NReadLines);
    }
  if(ac > 6)
    {
      sscanf(av[6],"%d",&skip);
    }

  double x[MAXLINES],corr[MAXLINES];
  
  
  int N = BufferFile(fname,col,x);

  double mean = Mean(x,skip,N,skip);

  int OutputData = 1;
  if(OutputData)
    {
      FILE *fPF = fopen("KappFromFlux_data.out","w");
      Output(fPF,x,mean,skip,N,skip);
      fclose(fPF);
    }

  OutputKappa(temp,x,mean,skip,N,skip);

}

int BufferFile(char *fname,int col, double *x)
{
  int lin;
  char bufS[10000],format[10000],tbufS[10000],labS[256];
  NLines = 0;

  format[0] = '\0';
  strcpy(tbufS," %*s ");
  for(int i = 0 ; i < col - 1; i++)
  {
    strcat(format,tbufS);
  }
  strcpy(tbufS,format);
  strcat(format," %lf");


  FILE *fPF = fopen(fname,"r");
  assert(fPF != NULL);  
  fgets(bufS,100000,fPF);

  while(!feof(fPF))
    {
      if(NLines >= Start)
  {
    lin = NLines - Start;
    if(lin == 0)
      {
        strcat(tbufS," %s");
        sscanf(bufS,tbufS,labS);
        printf("# %s\n",labS);
      }
    else
      sscanf(bufS,format,&x[lin]);	  
  }
      NLines++;
      if(NLines >= MAXLINES)
  break;
      if(NReadLines > 0 && (NLines - Start) >= NReadLines)
  break;
      fgets(bufS,100000,fPF);
    }
  fclose(fPF);
  NLines -= Start;
  return NLines;
}

void OutputKappa(double T,double * x,double mean,int start, int n, int skip)
{

  double sum = 0.0;
  int c = 0;
  for ( int i = start;i< n; i+=skip)
    {
      c++;
      double xx = (x[i] - mean) * (x[i] - mean);
      sum += xx;
    }

  sum /= c * mean;
  double boltz = 1.380662e-23;
  double LAtmperJoule = 1.0/101.325;
  double LperA3 = 1e-27;

  double RelFluxinL = LperA3 * sum;
  double kTinLAtm = boltz * T * LAtmperJoule;
  double kappa = RelFluxinL / kTinLAtm;

  printf("# Kappa T=%f = %g\n",T,kappa);

  return;
  
}  

void Output(FILE *fPF,double * x,double mean,int start, int n, int skip)
{
  double v = 0.0;
  int c = 0;
  fprintf(fPF,"# V[i]  dV^2[i]  dV^2[i]/V[i] V^2[i]\n");
  for ( int i = start;i< n; i+=skip)
    {
  double x2 = x[i] * x[i];
      double xx = (x[i] - mean) * (x[i] - mean);
      v = xx/x[i];
      fprintf(fPF,"%f  %f   %f   %f\n",x[i], xx,v, x2);
    }
  return;
}

  
double Mean(double * x,int start, int n, int skip)
{
  double sum = 0.0;
  int c = 0;
  for ( int i = start;i< n; i+=skip)
    {
      c++;
      sum += x[i];
    }
  sum /= c;
  return sum;
}

