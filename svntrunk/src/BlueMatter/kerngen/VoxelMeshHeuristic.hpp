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
#include <math.h>

int
GetVoxelMeshDimensions( int     aTargetVoxelsPerProc,
                        double  MinTargetVoxelDim,
                        double  sX, double sY, double sZ,
                        int     pX, int    pY, int    pZ,
                        int    &mX, int   &mY, int   &mZ )
  {
  int    pVol = pX * pY * pZ;
  double sVol = sX * sY * sZ;

  // The first step should be to realine the simulation space in the processor space.
  // For now, we'll just map SimX to ProcX, etc.

  double s2pX = sX / pX;
  double s2pY = sY / pY;
  double s2pZ = sZ / pZ;
  double NominalVoxelSize = 0.0;

  // Figure out which demension will guide tha mapping

  if( s2pX < s2pY && s2pX < s2pZ )
    {
    NominalVoxelSize = s2pX;
    printf("X axis guides nominal voxel size SimX %f / ProcX %d = Nominal %f\n", sX, pX, NominalVoxelSize );
    }
  else if( s2pY < s2pX && s2pY < s2pZ )  // Y is min aspect ratio
    {
    NominalVoxelSize = s2pY;
    printf("Y axis guides nominal voxel size SimY %f / ProcY %d = Nominal %f\n", sY, pY, NominalVoxelSize );
    }
  else                                   // Z is min aspect ratio
    {
    NominalVoxelSize = s2pZ;
    printf("Z axis guides nominal voxel size SimZ %f / ProcZ %d = Nominal %f\n", sZ, pZ, NominalVoxelSize );
    }

  double NomVoxMeshX = sX / NominalVoxelSize;
  double NomVoxMeshY = sY / NominalVoxelSize;
  double NomVoxMeshZ = sZ / NominalVoxelSize;

  printf("Nominal Voxel Mesh Dimensions X %f   Y %f   Z %f\n",  NomVoxMeshX, NomVoxMeshY, NomVoxMeshZ );

  // Round to nearest even

  int VoxMeshX = NomVoxMeshX + 1;
  int VoxMeshY = NomVoxMeshY + 1;
  int VoxMeshZ = NomVoxMeshZ + 1;

  VoxMeshX = ( VoxMeshX / 2 ) * 2;
  VoxMeshY = ( VoxMeshY / 2 ) * 2;
  VoxMeshZ = ( VoxMeshZ / 2 ) * 2;

  printf("Minimum voxel mesh dimensions X %d   Y %d   Z %d\n", VoxMeshX, VoxMeshY, VoxMeshZ );
  printf("Maximum voxel size            X %f   Y %f   Z %f\n", sX / VoxMeshX, sY / VoxMeshY, sZ / VoxMeshZ );
  int TotalVoxels = VoxMeshX * VoxMeshY * VoxMeshZ;
  int TotalProcs = pX * pY * pZ;
  double TotalVoxelsPerProc =  (1.0 * TotalVoxels) / TotalProcs;
  printf("Total Voxels %d  Total Procs %d  Voxels / Proc %f\n", TotalVoxels, TotalProcs, TotalVoxelsPerProc );

  double factor;
  for( factor = 1.0; ; factor += 0.1 )
    {
    NomVoxMeshX = sX / NominalVoxelSize;
    NomVoxMeshY = sY / NominalVoxelSize;
    NomVoxMeshZ = sZ / NominalVoxelSize;

    printf("Vox mesh scale factor %f\n", factor );

    NomVoxMeshX *= factor ;
    NomVoxMeshY *= factor ;
    NomVoxMeshZ *= factor ;

    printf("Nominal Voxel Mesh Dimensions X %f   Y %f   Z %f\n",  NomVoxMeshX, NomVoxMeshY, NomVoxMeshZ );

    // Round to nearest even

    VoxMeshX = NomVoxMeshX + 1;
    VoxMeshY = NomVoxMeshY + 1;
    VoxMeshZ = NomVoxMeshZ + 1;

    VoxMeshX = ( VoxMeshX / 2 ) * 2;
    VoxMeshY = ( VoxMeshY / 2 ) * 2;
    VoxMeshZ = ( VoxMeshZ / 2 ) * 2;


    printf("Minimum voxel mesh dimensions X %d   Y %d   Z %d\n", VoxMeshX, VoxMeshY, VoxMeshZ );
    printf("Maximum voxel size            X %f   Y %f   Z %f\n", sX / VoxMeshX, sY / VoxMeshY, sZ / VoxMeshZ );
    TotalVoxels = VoxMeshX * VoxMeshY * VoxMeshZ;
    TotalProcs = pX * pY * pZ;
    TotalVoxelsPerProc =  (1.0 * TotalVoxels) / TotalProcs;
    printf("Total Voxels %d  Total Procs %d  Voxels / Proc %f\n", TotalVoxels, TotalProcs, TotalVoxelsPerProc );

    if( (sX / VoxMeshX) < MinTargetVoxelDim || (sY / VoxMeshY) < MinTargetVoxelDim || (sZ / VoxMeshZ) < MinTargetVoxelDim )
      {
      printf("** Reached minimum voxel dim ( %f ) with dims of %f %f %f \n",  MinTargetVoxelDim, sX/VoxMeshX, sY/VoxMeshY, sZ/VoxMeshZ );
      break;
      }

    if( TotalVoxelsPerProc >= aTargetVoxelsPerProc )
      {
      printf("** Reached aTargetVoxelsPerProc %d with actual voxels per proc of %f\n", aTargetVoxelsPerProc, TotalVoxelsPerProc );
      break;
      }
    }

  printf("********************\n");

  mX = VoxMeshX;
  mY = VoxMeshY;
  mZ = VoxMeshZ;

  return(0);
  }

main()
  {
  double sX = 55.2;
  double sY = 77.1;
  double sZ = 105.2;

  int mX, mY, mZ;
  int pX, pY, pZ;

  double MinVoxelDim = 1.5;

  pX = 2; pY = 2; pZ = 1;
  GetVoxelMeshDimensions( 100,
                          MinVoxelDim,
                          sX, sY, sZ,
                          pX, pY, pZ,
                          mX, mY, mZ );

  printf("Mesh for Sim %f %f %f on Proc %d %d %d is %d %d %d\n\n\n",
          sX, sY, sZ,
          pX, pY, pZ,
          mX, mY, mZ );


  pX = 4; pY = 4; pZ = 2;
  GetVoxelMeshDimensions( 100,
                          MinVoxelDim,
                          sX, sY, sZ,
                          pX, pY, pZ,
                          mX, mY, mZ );

  printf("Mesh for Sim %f %f %f on Proc %d %d %d is %d %d %d\n\n\n",
          sX, sY, sZ,
          pX, pY, pZ,
          mX, mY, mZ );

  pX = 8; pY = 8; pZ = 8;
  GetVoxelMeshDimensions( 100,
                          MinVoxelDim,
                          sX, sY, sZ,
                          pX, pY, pZ,
                          mX, mY, mZ );

  printf("Mesh for Sim %f %f %f on Proc %d %d %d is %d %d %d\n\n\n",
          sX, sY, sZ,
          pX, pY, pZ,
          mX, mY, mZ );

  pX = 16; pY = 8; pZ = 8;
  GetVoxelMeshDimensions( 100,
                          MinVoxelDim,
                          sX, sY, sZ,
                          pX, pY, pZ,
                          mX, mY, mZ );

  printf("Mesh for Sim %f %f %f on Proc %d %d %d is %d %d %d\n\n\n",
          sX, sY, sZ,
          pX, pY, pZ,
          mX, mY, mZ );

  pX = 16; pY = 16; pZ = 8;
  GetVoxelMeshDimensions( 100,
                          MinVoxelDim,
                          sX, sY, sZ,
                          pX, pY, pZ,
                          mX, mY, mZ );

  printf("Mesh for Sim %f %f %f on Proc %d %d %d is %d %d %d\n\n\n",
          sX, sY, sZ,
          pX, pY, pZ,
          mX, mY, mZ );

  pX = 16; pY = 16; pZ = 16;
  GetVoxelMeshDimensions( 100,
                          MinVoxelDim,
                          sX, sY, sZ,
                          pX, pY, pZ,
                          mX, mY, mZ );

  printf("Mesh for Sim %f %f %f on Proc %d %d %d is %d %d %d\n\n\n",
          sX, sY, sZ,
          pX, pY, pZ,
          mX, mY, mZ );

  return(0);
  }
