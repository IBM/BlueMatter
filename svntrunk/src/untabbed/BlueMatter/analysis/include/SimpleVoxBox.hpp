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
 #ifndef SIMPLEVOXBOX_HPP
#define SIMPLEVOXBOX_HPP

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <BlueMatter/XYZ.hpp>

class SimpleVoxBox
{
public:
  enum {DIMS=3};
  enum {DIMWIDTH=10};
  int mVoxMeshDims[DIMS];
  double mCellLow[DIMS];
  double mCellHigh[DIMS];


  SimpleVoxBox(double CellLow[DIMS],double CellHigh[DIMS], int VoxMesh[DIMS])
    {
      for (int i=0;i<DIMS;i++)
  {
    mCellLow[i] = CellLow[i];
    mCellHigh[i] = CellHigh[i];
    mVoxMeshDims[i] = VoxMesh[i];
  }
    }

  SimpleVoxBox(BoundingBox &bb, int VoxMesh)
    {
      for (int i=0;i<DIMS;i++)
  {
    mCellLow[i] = bb.mMinBox[i];
    mCellHigh[i] = bb.mMaxBox[i];
    mVoxMeshDims[i] = VoxMesh;
  }
    }

  int VoxelId(double *v)
    {
      double h,l;
      int Id = 0;
      for(int i = 0;i<DIMS;i++)
  {
    Id <<= DIMWIDTH;
    h = mCellHigh[i];
    l = mCellLow[i];
    double dv = (v[i] - l) / (h - l);
    while (dv < 0.0) dv += 1.0;
    while (dv >= 1.0) dv -= 1.0;
    int N =  dv * mVoxMeshDims[i];
    assert(N < 1<<DIMWIDTH);
    Id |= N;
  }
      return Id;
    }

  void VoxelCoords(int Id , int *n)
    {
      int mask = (1<<DIMWIDTH) -1;
      n[2] = Id & mask;
      mask <<= DIMWIDTH;
      n[1] = (Id & mask) >> DIMWIDTH;
      mask <<= DIMWIDTH;
      n[0] = (Id & mask) >> 2 * DIMWIDTH;
      return;
    }

  int VoxelBounds(int Id , double *vlow, double *vhigh)
    {
      int mask = (1<<10) -1;
      int n[DIMS];
      VoxelCoords(Id,n);
      for (int i = 0; i < DIMS; i++)
  {
    double H = mCellHigh[i];
    double L = mCellLow[i];
    double D = H - L;
    double dV = D / mVoxMeshDims[i];      
    vlow[i] = n[i] * dV + L;
    vhigh[i] = (n[i] + 1) * dV + L;
  }
      return 1;
    }

  void VoxelPoint(int aVoxelId, double *vpnt)
    {
      double L[DIMS],H[DIMS],delta[DIMS];
      int vCoord[DIMS];

      VoxelBounds(aVoxelId,L,H);
      VoxelCoords(aVoxelId,vCoord);
      
      srand(aVoxelId);
      for (int i = 0; i < DIMS; i++)
  {
    delta[i] = (H[i] - L[i]) * (rand()/(RAND_MAX + 1.0));
    vpnt[i] = L[i] + 0.95 * delta[i] + 0.01 ;
  }
      
      return;
    }

    void Aliased(const XYZ &pin, XYZ &out)
    {
      double p[3];
      p[0] = pin.mX;
      p[1] = pin.mY;
      p[2] = pin.mZ;

      int id = VoxelId(p);
      VoxelPoint(id, p);
      out.mX = p[0];
      out.mY = p[1];
      out.mZ = p[2];
    }

};

#if 0
int main(int argc , char **argv)
{
  double low[DIMS],high[DIMS],v1[DIMS],v2[DIMS];
  int mesh[DIMS];
  

  for(int i=0;i<DIMS;i++)
    {
      low[i] = 0.0;
      high[i] = 100.0;
      mesh[i] = 10.0;
      v1[i] = 33.3;
      v2[i] = 0.0;
    }
  SimpleVoxBox svb(low,high,mesh);

  int voxid = svb.VoxelId(v1);
  svb.VoxelPoint(voxid,v2);

  printf(" input %f %f %f => voxid %x => voxpoint %f %f %f\n",v1[0], v1[1], v1[2], voxid, v2[0], v2[1],v2[2]);
  
  return 0;
}
#endif

#endif
