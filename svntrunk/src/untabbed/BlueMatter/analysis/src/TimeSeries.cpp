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
 

#include <BlueMatter/TimeSeries.hpp>

TimeSeries::TimeSeries()
{
  mData = NULL;
  mMap = NULL;
  mT = NULL;
  mNA = mNT = mLen = 0;
}

TimeSeries::~TimeSeries()
{
  Clear();
}
void TimeSeries::Init(int nats, int nrows)
{
  Clear();
  mNA = nats;
  mNT = nrows;
  mMap = new int[nats];
  mLen = nats * 3 * nrows;
  mData = new double[mLen];
  mT = new double[nrows];
}

void TimeSeries::Clear()
{
  mNA= mNA = mNT= 0;
  if(mData != NULL)
    { delete [] mData; mData = NULL; }
  if(mMap != NULL)
    { delete [] mMap; mMap = NULL; }
  if(mT != NULL)
    { delete [] mT; mT = NULL; }
}

double TimeSeries::Time(int i)
{
  assert(i < mNT);
  return mT[i];
}
double *TimeSeries::AtCoord(int atom, int time)
{
  assert( atom < mNA );
  assert( atom >= 0 );
  assert( time < mNT);
  int tindex = 3 * ( mNA * time + atom );
  return &mData[tindex];
}

int TimeSeries::AtIndex(int atomnumber)
{
  for(int i=0;i< mNA; i++)
    {
      if(atomnumber == mMap[i])
  return i;
    }
  return -1;
}

void TimeSeries::Vector(int time, int a1, int a2, double * vec)
{
  int tbase = time * 3 * mNA;
  int a1base = tbase + 3 * a1;
  int a2base = tbase + 3 * a2;
  for(int i = 0;i<3;i++)
    {
      vec[i] = mData[a2base + i] - mData[a1base + i];
    }
}
double TimeSeries::SqrDistance(int time, int a1 , int a2)
{
  double vec[3];
  Vector(time,a1,a2,vec);
  double r=0.0;
  for(int i=0;i<3;i++)
    r += vec[i] * vec[i];
  return r;
}

int TimeSeries::Read(int nrows, FILE *fPF)
{
  int nats = 0;
  char bufS[256];
  fgets(bufS,256,fPF);
  fscanf(fPF,"%*s %*s %d",&nats);
  Init(nats,nrows);
  for(int i=0;i<nats;i++)
    {
      fscanf(fPF,"%d",&mMap[i]);
    }
  int rowsread = 0;
  for(int i=0 ; i < nrows; i++)
    {
      double t;
      fscanf(fPF, "%lf", &t); 
      mT[i] = t;
      int tbase = i * 3 * nats;
      for(int j=0 ; j < nats; j++)
  {
    int ind = j * 3 + tbase; 
    int nr = fscanf(fPF," %lf %lf %lf",&mData[ind], &mData[ind + 1], &mData[ind + 2]);
    if(feof(fPF))
      break;
  }
      if(feof(fPF))
  break;
      rowsread++;
    }
  mNT = rowsread;
  return rowsread;
}


int main(int argc, char **argv)
{
  if(argc < 4)
    {
      printf("usage: %s <timeseries file> <nrows> <dt> \n",argv[0]);
      exit(1);
    }
  char fname[256];
  strcpy(fname, argv[1]);
  FILE *fPF = fopen(fname,"r");
  assert(fPF != NULL);
  int nrows=1;
  sscanf(argv[2],"%d",&nrows);
  double dt = 0.002;
  sscanf(argv[3],"%lf",&dt);

  TimeSeries TS;

  TS.Read(nrows,fPF);

  for(int i=0;i<nrows;i++)
    {
      double vec[3];
      TS.Vector(i,0,1,vec);
      double r = TS.SqrDistance(i,0,1);
      double t = TS.Time(i) * dt;
      printf("Time %f : %f %f %f %f\n",t,vec[0],vec[1],vec[2],r);
    }
}
