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
 
#ifndef PKFXLOG_THROBBER_TEST
#define PKFXLOG_THROBBER_TEST (1)
#endif

#define ASYMMETRICAL_HOODS 0  // if set to other than 0, the value (Rank % ASYMMETRICAL_HOODS) is added to manhattan radius
#define TEST_FUNC          0  // set to do a smaller number of cycles / frags
#define DROP_OUT_NODE      -1 //(-1 for none, otherwise a Rank)
#define THOOD_VERBOSE      0  //(Rank < 10)  //(Rank < 10 ) ////(Rank < 10 || Rank == 56 || Rank == 64 || Rank == 448 || Rank == 440 || Rank == 384 )
#define RECOOK_PHASES      6  //each phase sees the manahttan radius expanded - this is the number of phases

int Rank;

//#include "ThrobbinHood.hpp"
#include <Pk/API.hpp>
//#include <pk/platform.hpp>
#include <Pk/PkPktThrobberIF.hpp>

//#include <pk/fxlogger.hpp>

//static
//void* Core1Exit( void* arg )
//  {
//  exit(0);
//  return( NULL );
//  }

// These need to be static alloc to use dual core
PktThrobberIF Throbber;

static double
MPI_Wtime()
  {
  unsigned long long now = rts_get_timebase();
  double rc = 1.0 * now;
  rc /= 700 * 1000 * 1000;
  return( rc );
  }
  
int SimTick = 0 ;
  
int PkStartType = PkStartAllNodesCore0 ;
int PkTracing = 0 ;

int
PkMain(int argc, char** argv, char** envp)
  {

  PkTopology::Init(&argc, &argv) ;
  int Rank = PkNodeGetId() ;
  TorusXYZ m=PkTopo3DMakeXYZ( Rank ) ;
//  int mx, my, mz;
//  Platform::Topology::GetCoordsFromRank( Rank, &mx, &my, &mz );
  int mx=m.mX ; 
  int my=m.mY ; 
  int mz=m.mZ ; 

  BegLogLine(1)
    << "PkMain() "
    << " Rank " << Rank
    << " "      << PkTopology::GetAddressSpaceId()
    << " " << mx << " " << my << " " << mz
    << EndLogLine;

  int rc;

  int Core = hard_processor_id();
  int PartitionSize = PkTopology::GetAddressSpaceCount();

  int x, y, z;
  PkTopology::GetDimensions( &x, &y, &z );

  BegLogLine(1)
    << "PkMain() "
    << " Rank "                    << Rank
    << " P::T::GetAddressSpaceId " << PkTopology::GetAddressSpaceId()
    << " P::T::GetCoordsFromRank " << mx << " " << my << " " << mz
    << " Core "                    << Core
    << " Partitions Size "         << PartitionSize
    << " Dims "                    << x << " " << y << " " << z
    << EndLogLine;


  int AtomsPerFragment     = 3 ;
#if TEST_FUNC
  int FragmentsPerNode     = 4;
#else
  //int FragmentsPerNode     = 16000 / PartitionSize ;
  int FragmentsPerNode = 1;
#endif
  int AtomsPerNode         = AtomsPerFragment * FragmentsPerNode;
  int FragmentsInPartition = PartitionSize * FragmentsPerNode;

  // Expects will be what we expect from the reduce results.
  XYZ * ExpectXYZ =  (XYZ *)PkHeapAllocate(AtomsPerNode*sizeof(XYZ));

#if TEST_FUNC
  int TSPerPhase = 4;
#else
  int TSPerPhase = 100000;
#endif

  long long TotalReduce = 0;
  long long TotalBCast  = 0;

  int FragmentsRootedOnThisNode = FragmentsPerNode;

  /// this will pop an assert during GetPosit since you don't yet skip over the -1 frags.
  if( Rank == DROP_OUT_NODE )
  {
  FragmentsRootedOnThisNode = 0;
  }

  int Phase   = 0;
  int NodesInHoodCount = 0;
  int * NodesInHoodList = (int *)PkHeapAllocate(sizeof(int)*PartitionSize) ;
  assert( NodesInHoodList );


  for( int SimTick = 0 ; ; SimTick++ )
    {

    if( 0 == (SimTick % TSPerPhase) )
      {

      if ( Phase > 0 ) // report when this is beyond the first pass
        {
        #ifdef FIFO_COUNTERS
          Throbber.PrintFifoStats( TSPerPhase );
        #endif
            double RedMSecs = (1.0 * TotalReduce ) / (700.0 * 1000);
            RedMSecs /= TSPerPhase;

            double BCastMSecs = (1.0 * TotalBCast ) / (700.0 * 1000);
            BCastMSecs /= TSPerPhase;

            BegLogLine(1)
              << "PkMain(): Rank "         << Rank
              << " Phase "                 << Phase
              << " NodesInHoodCount "  << NodesInHoodCount
              << " BCastMSecs  "           << BCastMSecs
              << " RedMSecs "              << RedMSecs
              << EndLogLine;

            TotalReduce = 0;
            TotalBCast  = 0;
        }

      Phase++;

      ///////////////////NodeGroup.Reset();

      BegLogLine(PKFXLOG_THROBBER_TEST)
        << "Phase=" << Phase
        << " argc=" << argc 
        << EndLogLine ;
      if( Phase >= argc )
        break;

      //printf("% 4d %12.8f :: Opening hood file >%s< \n",
      //        Rank, MPI_Wtime(), argv[ Phase ] );

      BegLogLine(PKFXLOG_THROBBER_TEST) 
        << "HoodFile name [" << argv[ Phase ]
        << "] "
        << EndLogLine ;
        
      FILE *HoodFile = fopen( argv[ Phase ], "r" );

      if( HoodFile == NULL )
        {
          BegLogLine(PKFXLOG_THROBBER_TEST)
            << "Failed opening hood file"
            << EndLogLine ;
        printf("% 4d %12.8f :: FAILED Opening hood file >%s< \n",
                Rank, MPI_Wtime(), argv[ Phase ] );
        return( -1 );
        }

      NodesInHoodCount = 0;

      while( ! feof( HoodFile ) )
        {
        int LineType, HereRank, OtherRank;
        int rc = fscanf(HoodFile, "%d %d %d\n", &LineType, &HereRank, &OtherRank );
        if( rc != 3 )
          {
            BegLogLine(1)
              << "FAILED Reading hood file"
              << EndLogLine ;
          printf("% 4d %12.8f :: FAILED Reading hood file >%s< \n",
                  Rank, MPI_Wtime(), argv[ Phase ] );
          return -1 ;
          }
        BegLogLine(0)
          << "LineType=" << LineType
          << " HereRank=" << HereRank
          << " OtherRank=" << OtherRank
          << EndLogLine ;
        // type 0 will be the bcast send side -- type 1 will be the bcast receive from side
        if( LineType == 0 )
          {
          ///////////TorusXYZ OtherXYZ = GetTorusXYZ( OtherRank );
          if( HereRank == Rank )
            {
            // We need this for regression.
            ///////////////////////////////////NodeGroup.Add( OtherXYZ );

            NodesInHoodList[ NodesInHoodCount ] = OtherRank;
            BegLogLine(PKFXLOG_THROBBER_TEST)
              << " NodesInHoodList[" << NodesInHoodCount
              << "] = " << OtherRank
              << EndLogLine ;

            if(THOOD_VERBOSE)printf("% 4d %12.8f :: Added #%4d Rank % 4d \n",
                   Rank, MPI_Wtime(), NodesInHoodCount, HereRank );

            NodesInHoodCount++;
            }
          }
        }

      //printf("% 4d %12.8f :: Neighborhood Group %d nodes\n",
      //       Rank, MPI_Wtime(), NodesInHoodCount  );

      
      BegLogLine(PKFXLOG_THROBBER_TEST)
        << "Initialising throbber NodesInHoodList=" << NodesInHoodList
        << " NodesInHoodCount=" << NodesInHoodCount
        << " FragmentsInPartition=" << FragmentsInPartition
        << " FragmentsPerNode=" << FragmentsPerNode
        << EndLogLine ;
      Throbber.Init( x,y,z,
                     NodesInHoodList,
                     NodesInHoodCount,
                     FragmentsInPartition,
                     FragmentsPerNode * 2 );

      PkCo_Barrier() ;

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

        if(THOOD_VERBOSE) printf("% 4d %12.8f :: Local (Root) Frag # % 4d is FragId % 4d \n",
                Rank, MPI_Wtime(), f, FragId );

        Throbber.SetFragAsLocal( FragId );
        }

      // try to quiess the machine
      fflush(stdout);
      fflush(stderr);
      sync();
      /////////////////////sleep(1);
      PkCo_Barrier() ;
      PkCo_Barrier() ;
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
      XYZ * FragPositPtr = Throbber.GetFragPositPtrLocal( FragId );
      // Update posits... in the test case, just put in some recognizable fields.
      for( int a = 0; a < FragAtomCount ; a++ )
        {
        // Assign mX to the node atom a will be on
        int AtomId = (FragId * FragAtomCount) + a;
        FragPositPtr[ a ].mX = AtomId ;
        FragPositPtr[ a ].mY = Rank ;
        FragPositPtr[ a ].mZ = SimTick + a ;

        for(int g = 0; g < NodesInHoodCount; g++ )
          {
          int TaskInHood = NodesInHoodList[ g ];
          ExpectXYZ[ AtomOnNode ].mX += 1.0 * (AtomId);
          ExpectXYZ[ AtomOnNode ].mY += 1.0 * (TaskInHood * PartitionSize);
          ExpectXYZ[ AtomOnNode ].mZ += 1.0 * ( SimTick + a );
          }
        AtomOnNode++;
        }
      }
#endif

    // BGLPartitionBarrierCompute();
    PkCo_Barrier() ;
    if(THOOD_VERBOSE)printf("% 4d %12.8f :: Calling from BCAST on TS % 4d\n", Rank, MPI_Wtime(), SimTick );

    unsigned long long BCastStart = rts_get_timebase();
    Throbber.BroadcastExecute( SimTick, 0 );
    unsigned long long BCastEnd = rts_get_timebase();

    if(THOOD_VERBOSE)printf("% 4d %12.8f :: Returned from BCAST on TS % 4d\n", Rank, MPI_Wtime(), SimTick );
    // BGLPartitionBarrierCompute();
    PkCo_Barrier() ;

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


      XYZ * FragPosits = Throbber.GetFragPositPtrInHood( FragId );
      assert( FragPosits );
      assert( FragPosits != (void *)0xFFFFFFFF );

      if(THOOD_VERBOSE) printf("% 4d %12.8f :: Received FragsInHood % 4d/%4d  FragId % 4d  Posit @%08X  %f  %f  %f \n",
              Rank, MPI_Wtime(),
              f,FragsInHoodCount, FragId, FragPosits, FragPosits[ 0 ].mX, FragPosits[ 0 ].mY, FragPosits[ 0 ].mZ  );

      // Count packets received from each node to check later
      int PktSrcRank = (int) FragPosits[ 0 ].mY;
      assert( PktSrcRank >= 0 && PktSrcRank < PartitionSize );
      NodeCheckList[ PktSrcRank ]++ ;


      XYZ * FragForces = Throbber.GetFragForcePtr( FragId );

      int FragPositCount = 3;
      for( int p = 0; p < FragPositCount; p++ )
        {

        FragForces[ p ]    = FragPosits[ p ];
        FragForces[ p ].mY = Rank * PartitionSize;

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
    for( int p = 0; p < PartitionSize; p++ )
      {
// Drop zero
if( p == DROP_OUT_NODE )
continue;

      int IsNodeInHood = Throbber.WillOtherNodeBroadcastToThisNode( p ); /// || (p == Rank) ;

#if 0 /// this doesn't work if Throbber adds nodes to bcast to support reduce ! (ASYMMETRICAL_HOODS)
      // When asymetrical hoods not set, check that global hood as symmetrical entries with
      /// the local hood.
      if( IsNodeInHood )
        {
        TorusXYZ pxyz = GetTorusXYZ( p );
        if( ! NodeGroup.Search( pxyz ))
          {
          fprintf(stdout, "% 4d %12.8f :: BCAST ERROR : WITH SYMMETRIC HOODS, BROADCAST RECEIVED FROM % 4d WHICH IS NOT IN GROUP\n",
                  Rank,
                  MPI_Wtime(),
                  p );
          fflush( stdout );
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
        fprintf(stdout, "% 4d %12.8f :: BCAST ERROR : Node % 4d contributed but is not in hood\n",
                Rank,
                MPI_Wtime(),
                p );
        fflush( stdout );
        assert( 0 );
        }
      else if ( ! DidReceive &&   IsNodeInHood )
        {
        fprintf(stdout, "% 4d %12.8f :: BCAST ERROR : Node % 4d is in hood but did not contribute\n",
                Rank,
                MPI_Wtime(),
                p );
        fflush( stdout );
        assert( 0 );
        }
      }
#endif

    if(THOOD_VERBOSE)printf("% 4d %12.8f :: Hood Reduction Entering Barrier\n", Rank, MPI_Wtime() );

    // BGLPartitionBarrierCompute();
    PkCo_Barrier() ;

    unsigned long long RedStart = rts_get_timebase();
    Throbber.ReductionExecute( SimTick );
    unsigned long long RedEnd = rts_get_timebase();

    unsigned long long RedTotal = RedEnd - RedStart;
    TotalReduce += RedTotal;
    double             RedMSecs = (1.0 * RedTotal) / (700.0 * 1000);

    // BGLPartitionBarrierCompute();
    PkCo_Barrier() ;

    if(THOOD_VERBOSE)printf("% 4d %12.8f :: TS %d Hood Reduction Done %lld  %f ms !\n", Rank, MPI_Wtime(), SimTick, RedTotal, RedMSecs );


#if 0
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
          fprintf(stdout,"% 4d %12.8f :: ERROR REDUCE Frag# %d FragId % 4d  Force Ex/Re  %f = %f , %f = %f , %f = %f\n",
                  Rank, MPI_Wtime(),
                  f, FragId,
                  ExpectXYZ[ AtomOnNode ].mX, FragForces[ a ].mX,
                  ExpectXYZ[ AtomOnNode ].mY, FragForces[ a ].mY,
                  ExpectXYZ[ AtomOnNode ].mZ, FragForces[ a ].mZ );
          fflush(stdout);
////          assert( 0 );
          }
        AtomOnNode++;
        }
      }
#endif

    // BGLPartitionBarrierCompute();
    PkCo_Barrier() ;
    }

    PkCo_Barrier() ;
  // BGLPartitionBarrierCompute();

  //printf("% 4d %12.8f :: Finished\n",
  //        Rank, MPI_Wtime() );
  BegLogLine(1)
    << "PkMain(): Finished."
    << EndLogLine;


  // BGLPartitionBarrierCompute();
  PkCo_Barrier() ;

  #if THROBBER_USE_COROUTINE_INTERNALS
    vnm_co_start( Core1Exit, NULL, 0 );
  #endif
  return 0 ;
  }

