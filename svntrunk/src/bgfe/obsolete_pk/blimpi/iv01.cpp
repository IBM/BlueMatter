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
 /***************************************************************************
 * Project:         BLade mplemented on MPI (BLIMPI)
 *
 * Module:          iv00.cpp
 *
 * Purpose:         installation verification test - works over some blade function
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         Dec 10, 2002 BGF Created.
 *
 ***************************************************************************/


#include "BladeMpi.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//#include <pk/platform.hpp>
//#include <pk/fxlogger.hpp>

int print_something( _BGL_TorusPktPayload something, Bit32 dumb, Bit32 Dumber )
  {
  int MpiRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &MpiRank );  // Just to know

  printf("On MpiRank %3d print_something() got >%s<\n", MpiRank, (char *)something );
  fflush( stdout );

  return(0);
  }

int
main(int argc, char **argv, char **envp)
  {
  int MpiRank;
  MPI_Init( &argc, &argv );  /* initialize MPI environment */
  MPI_Comm_rank(MPI_COMM_WORLD, &MpiRank );  // Just to know


  AttachDebugger( argv[0] );

  BladeInit( &argc, &argv );

  //===========================================================================

  // Obtain this Node's coordinates in the Parition.
  // Coordinates range from (0 to x-1, 0 to y-1, 0 to z-1).

  int MyX;
  int MyY;
  int MyZ;

  BGLPartitionGetCoords( &MyX, &MyY, &MyZ );

  printf("BGLPartitionGetCoords() called: MyX %d  MyY %d  MyZ  %d\n",
         MyX, MyY, MyZ);

  //===========================================================================

  // Obtain the Partition's Maximum Extents (0 to x-1, 0 to y-1, 0 to z-1)
  // BGF This seems redundant info with the call following.

  int MaxX;
  int MaxY;
  int MaxZ;

  BGLPartitionGetExtents( &MaxX, &MaxY, &MaxZ );

  printf("BGLPartitionGetExtents() called: MaxX %d  MaxY %d  MaxZ %d\n",
         MaxX, MaxY, MaxZ );

  //===========================================================================

  // Obtain the Dimensions of the Partition (number of nodes, not indices)

  int NodesX;
  int NodesY;
  int NodesZ;

  BGLPartitionGetDimensions( &NodesX, &NodesY, &NodesZ );

  printf("BGLPartitionGetDimensions() called: NodesX %d NodesY %d NodesZ %d\n",
         NodesX, NodesY, NodesZ );

  //===========================================================================

  // Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.

  int RankXYZ;

  RankXYZ = BGLPartitionGetRankXYZ();

  printf("BGLPartitionGetRankXYZ() called: RankXYZ %d\n", RankXYZ );

  //===========================================================================

  // Obtain this Node's Rank (range is 0 to Nodes-1) when Z varies fastest.

  int RankZYX;

  RankZYX = BGLPartitionGetRankZYX();

  printf("BGLPartitionGetRankZYX() called: RankZYX %d\n", RankZYX );

  //===========================================================================

  // Create the Rank of the node at coordinates when X varies fastest.
  //  Note: order of args is X, Y, Z.
  for( int x = 0; x < NodesX; x++ )
    for( int y = 0; y < NodesY; y++ )
      for( int z = 0; z < NodesZ; z++ )
        {
        int RankXYZ = BGLPartitionMakeRankXYZ( x, y, z );
        printf("BGLPartitionMakeRankXYZ() called: RankXYZ %3d FOR: XYZ [%3d,%3d,%3d]\n",
                RankXYZ, x, y, z );
        }

  //===========================================================================

  // Create the Rank of the node at coordinates when Z varies fastest.
  //  Note: order of args is Z, Y, X.

  for( int x = 0; x < NodesX; x++ )
    for( int y = 0; y < NodesY; y++ )
      for( int z = 0; z < NodesZ; z++ )
        {
        int RankZYX = BGLPartitionMakeRankZYX( z, y, x );
        printf("BGLPartitionMakeRankZYX() called: RankZYX %3d FOR: XYZ [%3d,%3d,%3d]\n",
                RankZYX, x, y, z );
        }

  //===========================================================================


  for( int x = 0; x < NodesX; x++ )
    {
    for( int y = 0; y < NodesY; y++ )
      {
      for( int z = 0; z < NodesZ; z++ )
        {
        _BGL_TorusPkt    Pkt;
        int chunks = 7;

        char *str = (char *) Pkt.payload;
        str[0] = NULL;
        sprintf( str,
                "Sent From MpiRank %3d BGL Rank %3d XYZ [%3d,%3d,%3d]  To BGL Rank %3d XYZ [%3d,%3d,%3d]",
                 MpiRank,
                 BGLPartitionMakeRankZYX( MyX, MyY, MyZ ),
                 MyX, MyY, MyZ,
                 BGLPartitionMakeRankZYX( x, y, z ),
                 x, y, z );


       BGLTorusSimpleActivePacketSend( print_something,
                                       x,y,z,
                                       strlen( str ) + 1,
                                       (void *) str );



///////BGLTorusMakeHdr( & Pkt.hdr,             // Filled in on return
///////                  x,y,z,                 // destination coordinates
///////                  0,          //???      // destination Fifo Group
///////                  print_something,       // Active Packet Function matching signature above
///////                  0,                     // primary argument to actor
///////                  0,                     // secondary 10bit argument to actor
///////                  _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)
///////
///////
/////// BGLTorusSendPacketUnaligned( & Pkt.hdr ,      // Previously created header to use
///////                              Pkt.payload,     // Source address of data (16byte aligned)
///////                              strlen( str ) + 1 ); // Payload bytes = ((chunks * 32) - 16)
///////
        }
      }
    }

  while(1)
    sleep(1);
  }





