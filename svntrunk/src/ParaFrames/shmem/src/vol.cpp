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
 
//void PutMultiPacket_FrontEnd( void *, void *, int, int, int *);

#define DATA_COUNT (100)

#include <stdio.h>
#include <shmem.hpp>
#include "../fft/pk/fxlogger.hpp"

#include "shmem.cpp"

class PartitionPeriodicSpace
  {
  private:
    double mSpaceX, mSpaceY, mSpaceZ;
    double mUnitVoxel, mUnitVoxelY, mUnitVoxelZ;

  public:
    Init( double aSpaceX, double aSpaceY, double aSpaceZ )
      {
      mSpaceX = aSpaceX;
      mSpaceY = aSpaceY;
      mSpaceZ = aSpaceZ;

      mUnitVoxelX = mSpaceX / ShmemGetExtent3DX();
      mUnitVoxelY = mSpaceY / ShmemGetExtent3DY();
      mUnitVoxelZ = mSpaceZ / ShmemGetExtent3DZ();

      }

    int
    GetRank( double aX, double aY, double aZ )
      {
      double x = aX / mSpaceX;
      double y = aY / mSpaceY;
      double z = aZ / mSpaceZ;

      int xp = x / mUnitVoxelX;
      int yp = y / mUnitVoxelY;
      int zp = z / mUnitVoxelZ;

      }

  };



class SphericalNeighbors
  {

  double mRadius;
  double mSpaceX, mSpaceY, mSpaceZ;

  public:
    Init( double aVolumeRadius, double aSpaceX, double aSpaceY, double aSpaceZ )
      {
      mRadius = aVolumeRadius;
      mSpaceX = aSpaceX;
      mSpaceY = aSpaceY;
      mSpaceZ = aSpaceZ;

      ShmemGet

      if( (aVolumeRadius > 0.5 * mSpaceX)
          (aVolumeRadius > 0.5 * mSpaceX)
          (aVolumeRadius > 0.5 * mSpaceX) )
        {
        BegLogLine(1)
          << "SphericalNeighbors(): VolumeRadius too big for unit space."
          << EndLogLine;
        }

      }




  };

int _CP_main( int argc, char *argv[], char **envp )
  {
  BegLogLine(1)
    << "Hello from _CP_main() "
    << " my_pe "
    << my_pe()
    <<  "num_pes "
    << num_pes()
    << EndLogLine;

  int MyRank = my_pe();
  int Nodes  = num_pes();

  int NextRank = (MyRank + 1) % Nodes;
  int PrevRank = (MyRank + Nodes - 1) % Nodes;

  int myPx, myPy, myPz;
  BGLPartitionGetCoords( &myPx, &myPy, &myPz );

  int NextPx, NextPy, NextPz;
  BGLPartitionMakeCoords( NextRank,&NextPx, &NextPy, &NextPz );

  int PrevPx, PrevPy, PrevPz;
  BGLPartitionMakeCoords( PrevRank,&PrevPx, &PrevPy, &PrevPz );

  BegLogLine(1)
    << "Ring Def : "
    << "Nodes "
    << Nodes
    << " MyRank "
    << MyRank
    << " (XYZ: " << myPx << "." << myPy << "." << myPz << ") "
    << "RingNextRank "
    << NextRank
    << " (XYZ: " << NextPx << "." << NextPy << "." << NextPz << ") "
    << "RingPrevRank "
    << PrevRank
    << " (XYZ: " << PrevPx << "." << PrevPy << "." << PrevPz << ") "
    << EndLogLine;

  double BufA[ DATA_COUNT ];
  double BufB[ DATA_COUNT ];

  double BufC[ DATA_COUNT ];
  double BufD[ DATA_COUNT ];

  BufA[0]  =  0.0;
  BufB[0]  = -1.0;
  for(int i=1; i < DATA_COUNT; i++)
    {
    BufA[ i ] = MyRank + 1;
    BufB[ i ] = -1.0;
    BufA[ 0 ] += BufA[ i ];
    }

  /* Data barrier  */
  ShmemControlBarrier();

  for(int i = 0; i < Nodes * 10; i++)
    {
    ShmemGet( BufD, BufC, PrevRank );

    ShmemGet( BufB, BufA, NextRank );

    ShmemDataBarrier();

    double sum = 0.0;

    for(int j = 1; j < DATA_COUNT; j++ )
      {
      sum       += BufB[ j ];
      BufA[ j ]  = BufB[ j ];
      }
    BufA[ 0 ] = BufB[ 0 ];

    BegLogLine(1)
      << "Ring()"
      << " Iter "
      << i
      << " Test is "
      << (( sum == BufB[ 0 ] ) ? "PASSING " : "FAILING " )
      << EndLogLine;


    }

  BegLogLine(1)
    << "Ring Done."
    << EndLogLine;
  return(0);
  }

