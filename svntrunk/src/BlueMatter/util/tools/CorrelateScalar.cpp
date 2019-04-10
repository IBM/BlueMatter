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

#define UNBIASED_ESTIMATOR 1

int DoColumn(int col, double *c);
int BufferFile(char* fname,int col,double *x);
double Mean(double * x,int start, int n,int skip);
double Var(double * x, double mean,int start, int n, int skip);
double AutoCov(double * x, double mean,int start, int n, int skip);
void Histogram(double *x, int N, int NBins, double *upbound, int *bin,int skip);
void  ConfigureBins(double mean,double var,int nbins,double *upbounds);
void Output(FILE *fPF,double * x,double mean,int start, int n, int skip);



int Start = 0;
int NReadLines = -1;
char histname[100];
int DoHist = 0;
int DoCorr = 1;

int main(int ac, char **av)
{
  char fname[256];
  int col;
  int skip = 1;
  if(ac < 3)
    {
      printf("usage: %s <fname> <col> [timescale in ps (default=0.002)] [start 0] [N lines] [skip (1)] [hist fname] [nbins] \n",*av);
      exit(-1);
    }

  sprintf(fname,"%s",av[1]);
  sscanf(av[2],"%d",&col);
  if(ac > 3)
    {
      sscanf(av[3],"%lf",&timescale);
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
  if(ac > 7)
    {
      sscanf(av[7],"%s",&histname);
      DoHist = 1;
    }
  if(ac > 8)
    {
      sscanf(av[8],"%d",&NBins);
    }

  double x[MAXLINES],corr[MAXLINES];
  
  
  int N = BufferFile(fname,col,x);

  double mean = Mean(x,skip,N,skip);
  double C0 = Var(x,mean,skip,N,skip);


  corr[0] = C0;

  if(DoCorr)
    {
      double csum = 0.0;
      double csum2 = 0.0;
      double cn =  (double) N  / (double) skip; 
      for(int i = skip; i < N ; i+=skip)
	{
	  corr[i] = AutoCov(x,mean,i,N,skip) / C0;
	}
      double j = 0.0;
      for(int i = skip; i < N ; i+=skip)
	{	  	  
	  csum += corr[i];
	  csum2 += corr[i] * (1.0 -  j / cn) ;
	  j += 1.0;
	  printf("%f %f\n",(i * timescale), corr[i]);
	}
      double v1 = csum * 2.0 / cn;
      double v2 = csum2 * 2.0 / sqrt(cn);
      printf("# N mean csum csum2 var var2 %f %f %f %f %f %f\n",cn,mean, csum,csum2,v1,v2);
    }
  
  if(DoHist)
    {
      if(NBins % 2 == 0)NBins++;
      
      int *bin = new int[NBins+1];
      double *upbounds = new double[NBins+1];
      
      ConfigureBins(mean,C0,NBins,upbounds);
      Histogram(x, N,NBins, upbounds,bin,skip);
      double sd = sqrt(C0);
      int nn = N/skip;
      FILE *fPF = fopen(histname,"w");
      fprintf(fPF,"# Histogram mean = %f std = %f Npoints = %d NBins = %d\n",mean,sd,nn,NBins);
      
      double halfwidth = (upbounds[1] - upbounds[0])/2.0;
      double p = upbounds[0] - halfwidth;
      fprintf(fPF,"%f %d\n",p,bin[0]);

      for(int i = 1; i < NBins-1 ;i++)
	{
	  p = upbounds[i] - halfwidth;
	  halfwidth = (upbounds[i] - upbounds[i-1])/2.0;
	  fprintf(fPF,"%f %d\n",p,bin[i]);
	}
      p = upbounds[NBins - 1] - halfwidth;
      fprintf(fPF,"%f %d\n",p,bin[NBins - 1]);
      fclose(fPF);
    }
  int OutputData = 1;
  if(OutputData)
    {
      FILE *fPF = fopen("CorrelateScalar_data.out","w");
      Output(fPF,x,mean,skip,N,skip);
      fclose(fPF);
    }

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
	    //	    sscanf(bufS,format,&x[lin]);	  
	    x[lin] = 100*sin(1.0 / 31.4159 * lin);
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


void Output(FILE *fPF,double * x,double mean,int start, int n, int skip)
{
  double v = 0.0;
  int c = 0;
  for ( int i = start;i< n; i+=skip)
    {
      double xx = (x[i] - mean) * (x[i] - mean);
      v = xx/x[i];
      fprintf(fPF,"%f  %f   %f\n",x[i],xx,v);
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

double Var(double * x, double mean,int start, int n, int skip)
{
  double sum = 0.0;
  int c = 0;
  for ( int i = start;i< n; i+=skip)
    {
      c++;
      sum += (x[i] - mean)*(x[i] - mean);
    }
  sum /= c;

  return sum;
}
double AutoCov(double * x, double mean,int start, int n,int skip)
{
  int c = 0;
  double sum = 0.0;
  for ( int i = start;i< n; i+=skip)
    {
      c++;
      sum += (x[i-start + skip] - mean)*(x[i] - mean);
    }
#ifdef UNBIASED_ESTIMATOR
  sum /= ( (double) (n - skip ) / (double)skip);
#else
  sum /= c;
#endif
  return sum;
}


void Histogram(double *x, int N, int NBins, double *upbound, int *bin,int skip)
{
  int j;
  for(j=0;j<NBins;j++)
    bin[j] = 0;
  for(int i=1;i<N;i+=skip)
    {
      for(j=0;j<NBins-1;j++)
	{
	  if(x[i] < upbound[j])
	    {
	      bin[j]++;
	      break;
	    }
	}
      if(j == NBins)
	{
	  bin[NBins - 1]++;
	}
    }
  return;
}

void  ConfigureBins(double mean,double var,int nbins,double *upbounds)
{

  double sd = 4.0 * sqrt(var);

  double width = 2.0 * sd / nbins;  

  for(int i = 0;i<nbins;i++)
    {
      upbounds[i] = mean - sd + (i+1)*width;
    }
  return;
}
