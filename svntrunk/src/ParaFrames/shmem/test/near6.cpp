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
 #define DATA_COUNT (200)

#include <stdio.h>
#include <shmem.hpp>
#include <pk/fxlogger.hpp>

#include "shmem.cpp"

int _CP_main( int argc, char *argv[], char **envp )
  {
  BegLogLine(1)
    << "Hello from _CP_main() "
    << "Compiled on "
    << __DATE__
    << " at "
    << __TIME__
    << EndLogLine;

  int DimX, DimY, DimZ;
  BGLPartitionGetDimensions( & DimX, & DimY, & DimZ);

  int myPx, myPy, myPz;
  BGLPartitionGetCoords( &myPx, &myPy, &myPz );

  int MyRank = BGLPartitionGetRank();

  BegLogLine(1)
    << "Rank6 "
    << "MyRank "
    << MyRank
    << " (XYZ: " << myPx << "." << myPy << "." << myPz << ") "
    << " Dims "
    << " (XYZ: " << DimX << "." << DimY << "." << DimZ << ") "
    << EndLogLine;

  ShmemLocalDataBarrier();

  int Near6Ranks[6];

  // Would be nice if someone provided a MakeTorusRank()
  Near6Ranks[0] = BGLPartitionMakeRank( (DimX + myPx + 1) % DimX  , (DimY + myPy + 0) % DimY , (DimZ + myPz + 0) % DimZ );
  Near6Ranks[1] = BGLPartitionMakeRank( (DimX + myPx + 0) % DimX  , (DimY + myPy + 1) % DimY , (DimZ + myPz + 0) % DimZ );
  Near6Ranks[2] = BGLPartitionMakeRank( (DimX + myPx + 0) % DimX  , (DimY + myPy + 0) % DimY , (DimZ + myPz + 1) % DimZ );

  Near6Ranks[3] = BGLPartitionMakeRank( (DimX + myPx - 1) % DimX  , (DimY + myPy - 0) % DimY , (DimZ + myPz - 0) % DimZ );
  Near6Ranks[4] = BGLPartitionMakeRank( (DimX + myPx - 0) % DimX  , (DimY + myPy - 1) % DimY , (DimZ + myPz - 0) % DimZ );
  Near6Ranks[5] = BGLPartitionMakeRank( (DimX + myPx - 0) % DimX  , (DimY + myPy - 0) % DimY , (DimZ + myPz - 1) % DimZ );

  for( int i = 0; i < 6; i++ )
    {
    int x, y, z;
    BGLPartitionMakeCoords( Near6Ranks[ i ], &x, &y, &z );

    BegLogLine(1)
      << "Nearest neighbor "
      << i
      << " is Blade Rank "
      << Near6Ranks[ i ]
      << " (XYZ: " << x << "." << y << "." << z << ") "
      << EndLogLine;
    }


  //__attribute__ (( aligned(32) ))
  typedef int DATA_ARRAY[ DATA_COUNT ];

  typedef ShmemBuffer< DATA_ARRAY > SB_DATA_ARRAY;

  BegLogLine(1)
    << "Near6 "
    << " sizeof( DATA_ARRAY ) "
    << sizeof( DATA_ARRAY )
    << " sizeof( SB_DATA_ARRAY ) "
    << sizeof( SB_DATA_ARRAY )
    << EndLogLine;

  SB_DATA_ARRAY SB_SendBufSet[6];
  SB_DATA_ARRAY SB_RecvBufSet[6];

  if( MyRank == 0 )
    {
    for( int n = 0; n < 6; n++ )
      {
      BegLogLine(1)
        << "Near6 "
        << "Address of SendBufSet[ " << n << " ] "
        << (void *) (&SB_SendBufSet[n])
        << "Address of RecvBufSet[ " << n << " ] "
        << (void *) (&SB_RecvBufSet[n])
        << EndLogLine;
      }
    }

  for(int i = 0; i < 2 ; i++)
    {

    /*
     * Load up a bunch of random data - put a sum in loacation 0
     */
    for(int n = 0; n < 6; n++ )
      {
      // Get a reference to one of the ShmemBuffers
      DATA_ARRAY &da_ref = SB_SendBufSet[ n ].GetPayloadRef() ;
      int sum = da_ref[ 1 ] = MyRank;
      for(int d = 2 ; d < DATA_COUNT; d++)
        {
        sum += da_ref[ d ] =  n + d + MyRank;
        }
      da_ref[ 0 ] = sum;
      }

    ShmemLocalDataBarrier();

    BegLogLine(1)
      << "Near6 "
      << " About to begin comms "
      << " Iter "
      << i
      << EndLogLine;

    /*
     * Issue get calls
     */
    for(int n = 0; n < 6; n ++ )
      {
      if( (i & 0x01) == 0 )
        {
        ShmemGet( SB_RecvBufSet[ n ], SB_SendBufSet[ n ], Near6Ranks[ n ] );
        }
      else
        {
        ShmemPut( SB_RecvBufSet[ n ], SB_SendBufSet[ n ], Near6Ranks[ n ] );
        }
      }

    BegLogLine(1)
      << "Near6 "
      << " Done comms, doing GlobalDataBarrier "
      << " Iter "
      << i
      << EndLogLine;

    ShmemGlobalDataBarrier();

    BegLogLine(1)
      << "Near6 "
      << " Done GlobalDataBarrier, checking results "
      << " Iter "
      << i
      << EndLogLine;

    /*
     * Check the results for selfconsistnacy.
     */
    int ErrorCount = 0;
    for(int n = 0; n < 6; n++ )
      {
      DATA_ARRAY &da_ref = SB_RecvBufSet[ n ].GetPayloadRef() ;

      int sum = 0;
      for(int d = 1; d < DATA_COUNT; d++)
        {
        sum += da_ref[ d ];
        }

      // does the packet checksum?
      if( da_ref[ 0 ] != sum )
        {
        BegLogLine(1)
          << "Near6 bad packet checksum from neighbor ordinal = "
          << n
          << " which is rank "
          << Near6Ranks[ n ]
          << " Sender rank was "
          << da_ref[ 1 ]
          << EndLogLine;

        ErrorCount++;
        }

      // is the message from the right place?
      if( da_ref[ 1 ] != Near6Ranks[ n ] )
        {
        BegLogLine(1)
          << "Near6 bad sender/recver missmatch = "
          << n
          << " which is rank "
          << Near6Ranks[ n ]
          << " Sender rank was "
          << da_ref[ 1 ]
          << EndLogLine;

        ErrorCount++;
        }

      }

    BegLogLine(1)
      << "Near6()"
      << " Iter "
      << i
      << " Test "
      << ( ( ErrorCount == 0 ) ? "PASSED " : "FAILED " )
      << " ErrorCount is "
      << ErrorCount
      << EndLogLine;

    }

  BegLogLine(1)
    << "Near6 Done. Calling ShmemExit()"
    << EndLogLine;

  ShmemExit();

  return(0);
  }

