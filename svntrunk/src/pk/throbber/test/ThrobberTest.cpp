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
 

#define ASYMMETRICAL_HOODS 1  // if set to other than 0, the value (Rank % ASYMMETRICAL_HOODS) is added to manhattan radius
#define TEST_FUNC          0  // set to do a smaller number of cycles / frags
#define DROP_OUT_NODE      -1 //(-1 for none, otherwise a Rank)
#define THOOD_VERBOSE      0  //(Rank < 10)  //(Rank < 10 ) ////(Rank < 10 || Rank == 56 || Rank == 64 || Rank == 448 || Rank == 440 || Rank == 384 )
#define RECOOK_PHASES      6  //each phase sees the manahttan radius expanded - this is the number of phases

#include <spi/ThrobbinHood.hpp>

extern "C" {
// int
// PkMain( int argc, char **argv, char **envp )
void *
PkMain(int argc, char** argv, char** envp)
  {
  int rc;

#if 0
  if( (rc = _blade_on_blrts_init()) )
     {
     if(THOOD_VERBOSE) printf("main: blade_on_blrts_init: rc = %d.\n", rc );
     exit( rc );
     }
#endif

  // Rank = BGLPartitionGetRank();
  Rank = Platform::Topology::GetAddressSpaceId();

  int Core = hard_processor_id();
  int Size = BGLPartitionGetNumNodesCompute();

  int x, y, z;
  BGLPartitionGetDimensions( &x, &y, &z );

  TorusXYZ P;
  P.Set( x , y , z );

  HereXYZ = GetTorusXYZ( Rank );

  printf("% 4d %12.8f :: %s Starting Core %d SYM %d TFUNC %d DROP_OUT %d Part Size % 4d  X % 4d  Y % 4d  Z % 4d\n",
         Rank, MPI_Wtime(), argv[0], Core, ASYMMETRICAL_HOODS, TEST_FUNC, DROP_OUT_NODE, Size, x, y, z );

  int AtomsPerFragment     = 3 ;
  // Expects will be what we expect from the reduce results.
  XYZ * ExpectXYZ =  NULL;

#if TEST_FUNC
  int TSPerPhase = 4;
#else
  int TSPerPhase = 1000;
#endif

  long long TotalReduce = 0;
  long long TotalBCast  = 0;

  ThrobbinHood Throbber;
  Group NodeGroup;  // This group will be used multiple times - Reset() will be called.

  int AtomsPerNode         ;
  int FragmentsInPartition ;
  int FragmentsPerNode     ;
  int FragmentsRootedOnThisNode;

  int Phase   = 0;
  for( int SimTick = 0 ; ; SimTick++ )
    {





    // BEGIN SETUP -- THIS BLOCK RESETS UP FOR SEVERAL TIMES
    if( 0 == (SimTick % TSPerPhase) )
      {
      // NOTE : Without a2a, we gen groups in Thorbber.Init(), not in the app.
      // Come up with a group of nodes - this time, a Manhattan sphere.
      Phase++;



      #if TEST_FUNC
        FragmentsPerNode     = 4;
      #else
        //int FragmentsPerNode     = 16000 / P.IntVolume() ;
        if( Phase % 2 == 0 )
          FragmentsPerNode     = 4 ;
        else
          FragmentsPerNode     = 3 ;
      #endif
        AtomsPerNode         = AtomsPerFragment * FragmentsPerNode;
        FragmentsInPartition = P.IntVolume() * FragmentsPerNode;

        if( ExpectXYZ != NULL )
          {
          delete [] ExpectXYZ;
          ExpectXYZ = NULL;
          }

        ExpectXYZ =  new XYZ[ AtomsPerNode ];

        FragmentsRootedOnThisNode = FragmentsPerNode;
      /// this will pop an assert during GetPosit since you don't yet skip over the -1 frags.
      if( Rank == DROP_OUT_NODE )
      {
      FragmentsRootedOnThisNode = 0;
      }



      if ( Phase > 1 )
        {
        #ifdef FIFO_COUNTERS
          Throbber.PrintFifoStats( TSPerPhase );
        #endif
            double RedMSecs = (1.0 * TotalReduce ) / (700.0 * 1000);
            RedMSecs /= TSPerPhase;

            double BCastMSecs = (1.0 * TotalBCast ) / (700.0 * 1000);
            BCastMSecs /= TSPerPhase;

            printf("% 4d %12.8f :: MANHATTAN SPHERE OF %d  THROBBER TIMING BCAST %f ms  RED %f ms \n",
                    Rank, MPI_Wtime(), Phase, BCastMSecs, RedMSecs );
            fflush(stdout);
            fflush(stderr);
            TotalReduce = 0;
            TotalBCast  = 0;
        }

      if( Phase > RECOOK_PHASES )
        break;  // This is the end of the test

      NodeGroup.Reset();

      int ManhattanSphereRadius = Phase;

      if( ASYMMETRICAL_HOODS )
        {
        ManhattanSphereRadius += Rank % 2;
        }

      int NodesInHoodCount = 0;
      for( int g = 0; g < P.IntVolume(); g++ )
        {
        TorusXYZ Other = GetTorusXYZ( g );
        if( HereXYZ.ManhattanDistanceImaged( P, Other ) <= ManhattanSphereRadius )
          {
          NodeGroup.Add( Other );

          if(THOOD_VERBOSE)printf("% 4d %12.8f :: Added #%4d Rank % 4d  at X % 4d  Y % 4d Z % 4d\n",
                 Rank, MPI_Wtime(), NodesInHoodCount, g, Other.mX, Other.mY, Other.mZ );

          NodesInHoodCount++;
          }
        }

      printf("% 4d %12.8f :: Group of manhattan sphere %d has %d nodes\n",
             Rank, MPI_Wtime(), ManhattanSphereRadius, NodesInHoodCount  );

      Throbber.Init( P, NodeGroup, FragmentsInPartition );


      // BGLPartitionBarrierCompute();
      Platform::Control::Barrier();


      // Throbber needs to know the size of all fragments to pack and unpack packets.
      for( int f = 0; f < FragmentsInPartition; f++ )
        {
        Throbber.SetGlobalFragInfo( f, AtomsPerFragment );
        }

      // This will be called after migration to tell throbber which sites are local
      // NEED: go look at this and make sure it can be updated out of init phases.
      for( int f = 0 ; f < FragmentsRootedOnThisNode ; f++ )
        {
        // Gen a FragId
        int FragId        = (Rank * FragmentsPerNode) + f ;

        if(THOOD_VERBOSE)printf("% 4d %12.8f :: Local (Root) Frag # % 4d is FragId % 4d \n",
                Rank, MPI_Wtime(), f, FragId );

        Throbber.SetFragAsLocal( FragId );
        }

      Platform::Control::Barrier();
      }
    ///  END SETUP

#if 1
    bzero( (void*) ExpectXYZ , AtomsPerNode * sizeof( XYZ ) );
    // UPDATE POSITS
    int AtomOnNode = 0;
    for( int f = 0 ; f < FragmentsRootedOnThisNode ; f++ )
      {
      // Gen a FragId
      int FragId        = (Rank * FragmentsPerNode) + f ;
      int FragAtomCount = 3 ;
      XYZ * FragPositPtr = Throbber.GetFragPositPtr( FragId );
      // Update posits... in the test case, just put in some recognizable fields.
      for( int a = 0; a < FragAtomCount ; a++ )
        {
        // Assign mX to the node atom a will be on
        int AtomId = (FragId * FragAtomCount) + a;
        FragPositPtr[ a ].mX = AtomId ;
        FragPositPtr[ a ].mY = Rank ;
        FragPositPtr[ a ].mZ = SimTick + a ;

        for(int g = 0; g < NodeGroup.Count(); g++ )
          {
          int TaskInHood = MakeTaskId( NodeGroup[ g ] );
// Don't drop out here - after all, the other guys is still going to op on the packests this nodes sends
//// Drop zero
//if( TaskInHood == DROP_OUT_NODE )
//continue;
          ExpectXYZ[ AtomOnNode ].mX += 1.0 * (AtomId);
          ExpectXYZ[ AtomOnNode ].mY += 1.0 * (TaskInHood * Size);
          ExpectXYZ[ AtomOnNode ].mZ += 1.0 * ( SimTick + a );
          }
        AtomOnNode++;
        }
      }
#endif

    // BGLPartitionBarrierCompute();
    Platform::Control::Barrier();
    if(THOOD_VERBOSE)printf("% 4d %12.8f :: Calling from BCAST on TS % 4d\n", Rank, MPI_Wtime(), SimTick );

    unsigned long long BCastStart = rts_get_timebase();
    Throbber.BroadcastExecute();
    unsigned long long BCastEnd = rts_get_timebase();

    if(THOOD_VERBOSE)printf("% 4d %12.8f :: Returned from BCAST on TS % 4d\n", Rank, MPI_Wtime(), SimTick );
    // BGLPartitionBarrierCompute();
    Platform::Control::Barrier();

    unsigned long long BCastTotal = BCastEnd - BCastStart;
    TotalBCast += BCastTotal;
    double             BCastMSecs = (1.0 * BCastTotal) / (700.0 * 1000);

    if(THOOD_VERBOSE)printf("% 4d %12.8f :: TS %d Hood BCast Done %lld  %f ms !\n", Rank, MPI_Wtime(), SimTick, BCastTotal, BCastMSecs );

    int FragsInHoodCount = Throbber.GetFragsInHoodCount();

    if(THOOD_VERBOSE)printf("% 4d %12.8f :: TS %d Hoodcast done!  Received FragsInHood % 4d\n",
            Rank, MPI_Wtime(), SimTick, FragsInHoodCount  );

#if 1
    #define MAX_CHECK_NODES  (64 * 1024)
    char NodeCheckList[ MAX_CHECK_NODES ];
    bzero( (void*)NodeCheckList, MAX_CHECK_NODES );
    for(int f = 0; f < FragsInHoodCount; f++ )
      {
      int FragId = Throbber.GetFragIdInHood( f );
if( FragId == -1 )
{
    printf("% 4d %12.8f :: TS %d GOT DUMMY FRAG\n",
            Rank, MPI_Wtime(), SimTick );
continue;
}


      XYZ * FragPosits = Throbber.GetFragPositPtr( FragId );
      assert( FragPosits );
      assert( FragPosits != (void *)0xFFFFFFFF );

      if(THOOD_VERBOSE) printf("% 4d %12.8f :: Received FragsInHood % 4d/%4d  FragId % 4d  Posit  %f  %f  %f \n",
              Rank, MPI_Wtime(),
              f,FragsInHoodCount, FragId, FragPosits[ 0 ].mX, FragPosits[ 0 ].mY, FragPosits[ 0 ].mZ  );

      // Count packets received from each node to check later
      int PktSrcRank = (int) FragPosits[ 0 ].mY;
      assert( PktSrcRank >= 0 && PktSrcRank < P.IntVolume() );
      NodeCheckList[ PktSrcRank ]++ ;


      XYZ * FragForces = Throbber.GetFragForcePtr( FragId );

      int FragPositCount = 3;
      for( int p = 0; p < FragPositCount; p++ )
        {

        FragForces[ p ]    = FragPosits[ p ];
        FragForces[ p ].mY = Rank * Size;

        if(THOOD_VERBOSE)printf("% 4d %12.8f :: Frag# %d FragId % 4d/%4d  Posit  %f  %f  %f   Force %f  %f  %f\n",
                Rank, MPI_Wtime(),
                f, FragId, FragsInHoodCount,
                FragPosits[ p ].mX, FragPosits[ p ].mY, FragPosits[ p ].mZ,
                FragForces[ p ].mX, FragForces[ p ].mY, FragForces[ p ].mZ  );
        }
      }

    // Go through the check list, compare to the group, decide if we got what we should
    // Can't yet check the number of packets yet.
    // THIS CHECK WON'T WORK IN A WORLD WITHOUT RECIPRICAL GROUPS.
    for( int p = 0; p < P.IntVolume(); p++ )
      {
// Drop zero
if( p == DROP_OUT_NODE )
continue;

      int IsNodeInHood = Throbber.WillOtherNodeBroadcastToThisNode( p ); /// || (p == Rank) ;
#if ! (ASYMMETRICAL_HOODS)
      // When asymetrical hoods not set, check that global hood as symmetrical entries with
      /// the local hood.
      if( IsNodeInHood )
        {
        TorusXYZ pxyz = GetTorusXYZ( p );
        if( ! NodeGroup.Search( pxyz ))
          {
          fprintf(stderr, "% 4d %12.8f :: BCAST ERROR : WITH SYMMETRIC HOODS, BROADCAST RECEIVED FROM % 4d WHICH IS NOT IN GROUP\n",
                  Rank,
                  MPI_Wtime(),
                  p );
          fflush( stderr );
          assert( 0 );
          }
        }
#endif

      int DidReceive   = NodeCheckList[ p ];

      if      ( ! DidReceive && ! IsNodeInHood )
        {
        continue;  // packet has no input from node and node is not in our hood
        }
      else if (   DidReceive &&   IsNodeInHood )
        {
        if(THOOD_VERBOSE) printf("% 4d %12.8f :: BCAST CHECKED : Node % 4d contributed and is in hood\n",
                Rank,
                MPI_Wtime(),
                p );
        continue;
        }
      else if (   DidReceive && ! IsNodeInHood )
        {
        fprintf(stderr, "% 4d %12.8f :: BCAST ERROR : Node % 4d contributed but is not in hood\n",
                Rank,
                MPI_Wtime(),
                p );
        fflush( stderr );
        assert( 0 );
        }
      else if ( ! DidReceive &&   IsNodeInHood )
        {
        fprintf(stderr, "% 4d %12.8f :: BCAST ERROR : Node % 4d is in hood but did not contribute\n",
                Rank,
                MPI_Wtime(),
                p );
        fflush( stderr );
        assert( 0 );
        }
      }
#endif

    // BGLPartitionBarrierCompute();
    Platform::Control::Barrier();

    unsigned long long RedStart = rts_get_timebase();
    Throbber.ReductionExecute();
    unsigned long long RedEnd = rts_get_timebase();

    unsigned long long RedTotal = RedEnd - RedStart;
    TotalReduce += RedTotal;
    double             RedMSecs = (1.0 * RedTotal) / (700.0 * 1000);

    // BGLPartitionBarrierCompute();
    Platform::Control::Barrier();

    if(THOOD_VERBOSE)printf("% 4d %12.8f :: TS %d Hood Reduction Done %lld  %f ms !\n", Rank, MPI_Wtime(), SimTick, RedTotal, RedMSecs );


#if 1
    // CHECK EXPECTS
    AtomOnNode = 0;
    for( int f = 0 ; f < FragmentsRootedOnThisNode ; f++ )
      {
      // Gen a FragId
      int FragId        = (Rank * FragmentsPerNode) + f ;
      int FragAtomCount = 3 ;
      XYZ * FragForces = Throbber.GetFragForcePtr( FragId );
// Could need to check frag id here
      // Update posits... in the test case, just put in some recognizable fields.
      for( int a = 0; a < FragAtomCount ; a++ )
        {
        if( ExpectXYZ[ AtomOnNode ] != FragForces[ a ] )
          {
          printf("% 4d %12.8f :: ERROR REDUCE Frag# %d FragId % 4d  Force Ex/Re  %f = %f , %f = %f , %f = %f\n",
                  Rank, MPI_Wtime(),
                  f, FragId,
                  ExpectXYZ[ AtomOnNode ].mX, FragForces[ a ].mX,
                  ExpectXYZ[ AtomOnNode ].mY, FragForces[ a ].mY,
                  ExpectXYZ[ AtomOnNode ].mZ, FragForces[ a ].mZ );
          fprintf(stderr,"% 4d %12.8f :: ERROR REDUCE Frag# %d FragId % 4d  Force Ex/Re  %f = %f , %f = %f , %f = %f\n",
                  Rank, MPI_Wtime(),
                  f, FragId,
                  ExpectXYZ[ AtomOnNode ].mX, FragForces[ a ].mX,
                  ExpectXYZ[ AtomOnNode ].mY, FragForces[ a ].mY,
                  ExpectXYZ[ AtomOnNode ].mZ, FragForces[ a ].mZ );
          fflush(stderr);
          assert( 0 );
          }
        AtomOnNode++;
        }
      }
#endif

    // BGLPartitionBarrierCompute();
    Platform::Control::Barrier();
    }

  Platform::Control::Barrier();
  // BGLPartitionBarrierCompute();

  printf("% 4d %12.8f :: Finished\n",
          Rank, MPI_Wtime() );

  // BGLPartitionBarrierCompute();
  Platform::Control::Barrier();

  return( NULL );
  }
};

