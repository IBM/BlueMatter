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
 * Project:         Blue Gene Functional Emulator
 *
 * Module:          redapp.cpp
 *
 * Purpose:         Minimal functional test for reduction queues.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         032900 BGF Created.
 *
 ***************************************************************************/

#include <pk/reactor.hpp>
#include <math.h>

#include <PhysicalTopology.hpp>
#include <ThreadCreate.hpp>
#include <Reduce.hpp>
#include <Broadcast.hpp>
#include <Grid2D.hpp>

#define MAX_ROW_SIZE 200
#define MAX_COL_SIZE 200

class XYZ
  {
  public:
    double mX, mY, mZ;
  };

class XYZ_Sum
  {
  public:
    static void
    Identity( XYZ &aA )
      {
      aA.mX = 0;
      aA.mY = 0;
      aA.mZ = 0;
      }

    static void
    Op(XYZ &aA, XYZ &aB )
      {
      BegLogLine(1)
        << "XYZ_Sum(): Starting "
        << " A.mx " << aA.mX << " A.mY " << aA.mY << " A.mZ " << aA.mZ
        << " PLUS "
        << " B.mx " << aB.mX << " B.mY " << aB.mY << " B.mZ " << aB.mZ
        << EndLogLine;
      aA.mX += aB.mX;
      aA.mY += aB.mY;
      aA.mZ += aB.mZ;
      BegLogLine(1)
        << "XYZ_Sum(): Finished New: "
        << " A.mx " << aA.mX << " A.mY " << aA.mY << " A.mZ " << aA.mZ
        << EndLogLine;
      }
  };

class XYZ_Copy
  {
  public:
    static void
    Identity( XYZ &aA )
      {
      aA.mX = 0;
      aA.mY = 0;
      aA.mZ = 0;
      }

    static void
    Op(XYZ &aA, XYZ &aB )
      {
      BegLogLine(1)
        << "XYZ_Copy(): Starting "
        << " A.mx " << aA.mX << " A.mY " << aA.mY << " A.mZ " << aA.mZ
        << " GETS=: "
        << " B.mx " << aB.mX << " B.mY " << aB.mY << " B.mZ " << aB.mZ
        << EndLogLine;
      aA.mX = aB.mX;
      aA.mY = aB.mY;
      aA.mZ = aB.mZ;
      BegLogLine(1)
        << "XYZ_Copy(): Finished New: "
        << " A.mx " << aA.mX << " A.mY " << aA.mY << " A.mZ " << aA.mZ
        << EndLogLine;
      }
  };


int DoForceContribution( XYZ &MyRowsForceSet, XYZ &MyRowsPositionSet, XYZ &NextPositionSet )
  {
  BegLogLine( 1 )<< "TestThread: DoForceContribution() "<< EndLogLine;
  return(0);
  }


int UpdateVelocityAndPosition( XYZ MyRowsPositionSet, XYZ MyRowsVelocitySet, XYZ MyRowsForceSet )
  {
  BegLogLine( 1 )<< "TestThread: UpdateVelocityAndPosition() " << EndLogLine;
  return(0);
  }

// NOTE: The following to be staticly fixed address (outside a routine, thus determined at link time).
Reduce<XYZ, XYZ_Sum, Grid2D::GroupRow> RowReduce[ MAX_ROW_SIZE ];
Reduce<XYZ, XYZ_Sum, Grid2D::GroupCol> ColReduce[ MAX_ROW_SIZE ];

Broadcast<XYZ, XYZ_Copy, Grid2D::GroupRow> RowBroadcast[ MAX_ROW_SIZE ];
Broadcast<XYZ, XYZ_Copy, Grid2D::GroupCol> ColBroadcast[ MAX_ROW_SIZE ];

void *
TestThread( void * arg )
  {
  fprintf(stderr,"alive from %d which is %d\n", PhysicalTopology::AddressSpaceSelf(), getppid() );
  fflush(stderr);
  sync();
  usleep(1000);
  usleep(1000);
  usleep(1000);
  // We'll be very very nice here and wait for the comms to come up.
  // This is required for staticly defined threads.
  // NEED to get this into a wrapper and to start static defined threads
  // after active message comms.
  while( ! PkActiveMsgSubSys::IsInit() )
    usleep(1000);


  BegLogLine( 1 )
    << "TestThread: "
    << " starting "
    << EndLogLine;

  //This ensures only one of these threads per task (... if you are lucky)
  static int MyAddSpcId = -1;
  assert( MyAddSpcId == -1 );  // cheesy check that we are the only thread on this node.
  MyAddSpcId         = PhysicalTopology::AddressSpaceSelf();

  Grid2D ReduceGrid;

  ReduceGrid.MaximumNearSquareWithColumnsLonger();

  BegLogLine( 1 )
  << "TestThread: "
  << " Total Address Spaces " << PhysicalTopology::AddressSpaceCount()
  << " X Dim " << ReduceGrid.GetXDimension()
  << " Y Dim " << ReduceGrid.GetYDimension()
  << EndLogLine;

  // Check if this thread is participating, if not, exit.
  if( ! ReduceGrid.IsMember( PhysicalTopology::AddressSpaceSelf() ) )
    {
    BegLogLine( 1 )
      << "TestThread: "
      << " MyAddSpcId " << PhysicalTopology::AddressSpaceSelf()
      << " Not on ReduceGrid, getting out of the game "
      << EndLogLine;
    return(0);
    }

  // Some useful shorthand
  int MyRow     = ReduceGrid.MyYCoordinate();
  int MyCol     = ReduceGrid.MyXCoordinate();
  int RowsTotal = ReduceGrid.GetYDimension();

  BegLogLine( 1 ) << "TestThread: "<< " MyAddSpcId " << MyAddSpcId  << " MyCol " << MyCol << " MyRow " << MyRow << EndLogLine;

  RowBroadcast[MyRow].Attach( ReduceGrid.AddressSpaceAt( 0, MyRow ),   // root
                              ReduceGrid.GetRowGroupIF( MyRow )     ); // group

  RowReduce[MyRow].Attach( ReduceGrid.AddressSpaceAt( 0, MyRow ),    // root of this op
                           ReduceGrid.GetRowGroupIF( MyRow )     );  // group

  //Join a broadcast and reduce column for each row
  int RowNum;
  for( RowNum = 0; RowNum < RowsTotal ; RowNum++)
    {
    ColBroadcast[ RowNum ].Attach( ReduceGrid.AddressSpaceAt( MyCol, RowNum ),
                                   ReduceGrid.GetColGroupIF( MyCol )            );
    }

  // THIS REDUCE IS ONLY TO CREATE APPLICATION LEVEL SEQUENCING
  for(  RowNum = 0; RowNum < RowsTotal ; RowNum++)
    {
    ColReduce[ RowNum ].Attach( ReduceGrid.AddressSpaceAt( MyCol, RowNum ),
                                   ReduceGrid.GetColGroupIF( MyCol )            );
    }

  BegLogLine( 1 )<< "TestThread: "<< " MyAddSpcId " << MyAddSpcId  << " done attaching "<< EndLogLine;

  // Setup somge data to reduce
  XYZ MyRowsPositionSet;
  XYZ MyRowsForceSet;
  XYZ MyRowsVelocitySet;

  //Fill out a source position set
  if( RowBroadcast[ MyCol ].AmRoot() )
    {
    MyRowsPositionSet.mX = MyRow + 1; // +1 becuase 0 always looks like a new side effect.
    MyRowsPositionSet.mY = MyCol + 1;
    MyRowsPositionSet.mZ = MyAddSpcId;
    }
  else
    {
    MyRowsPositionSet.mX = -1 * (MyRow + 1); // +1 becuase 0 always looks like a new side effect.
    MyRowsPositionSet.mY = -1 * (MyCol + 1);
    MyRowsPositionSet.mZ = -1 * MyAddSpcId;
    }

  BegLogLine(1)
    << "TestThread: Initial values : "
    <<    " MyRowsPositionSet.mX "    << MyRowsPositionSet.mX
    <<    " MyRowsPositionSet.mY "    << MyRowsPositionSet.mY
    <<    " MyRowsPositionSet.mZ "    << MyRowsPositionSet.mZ
    << EndLogLine;

  BegLogLine( 1 )
    << "TestThread: before reduct "
    << " MyAddSpcId " << MyAddSpcId
    << EndLogLine;

  // think about doing interactions within set.

  int StepNo = 0;
  for(StepNo = 0; StepNo < 10000; StepNo++ )
    {

    //int error = MPI_Barrier( MPI_COMM_WORLD );
    //sleep(1);

    BegLogLine(1) << "\n\n\nTestThread: BEGIN : " << StepNo << EndLogLine;

    // Broadcast source set atom positions allong rows.
    if( RowBroadcast[ MyRow ].AmRoot() )
      {
      BegLogLine(1) << "TestThread: Before Push: Step : " << StepNo << EndLogLine;
      RowBroadcast[ MyRow ].Push( MyRowsPositionSet, StepNo );
      BegLogLine(1) << "TestThread: After Push: Step : " << StepNo << EndLogLine;
      }

    // Everyone including root pulls.  This will make it easier to thread later.
    BegLogLine(1) << "TestThread: Before WaitPull: Step : " << StepNo << EndLogLine;
    RowBroadcast[ MyRow ].WaitPull( MyRowsPositionSet, StepNo );
    BegLogLine(1) << "TestThread: After WaitPull: Step : " << StepNo << EndLogLine;

    // Broadcast-push atom positions set to all others in column
    BegLogLine(1) << "TestThread: start ColBroadcast:Push() subCol " << MyRow << EndLogLine;
    ColBroadcast[ MyRow ].Push( MyRowsPositionSet, StepNo );
    BegLogLine(1) << "TestThread: done  ColBroadcast:Push() subCol " << MyRow << EndLogLine;

    // Broadcast-pull an atom position set from every other in column
    // Note this loop could load balance by eagerly operating on first come first served basis
    int RowCount;
    for( RowCount = 0; RowCount < RowsTotal; RowCount++ )
      {
      XYZ NextPositionSet;
      // Receive the broad cast from each row inluding thie row context in this column
      BegLogLine(1) << "TestThread: Start ColBroadcast:Pull subCol " << RowCount << EndLogLine;
      ColBroadcast[RowCount].WaitPull( NextPositionSet, StepNo );
      BegLogLine(1) << "TestThread: done  ColBroadcast:Pull subCol " << RowCount << EndLogLine;
      DoForceContribution( MyRowsForceSet, MyRowsPositionSet, NextPositionSet );
      }
#if 1
        // Reduce-push on every row
        // Note this loop could load balance by eagerly operating on first come first served basis
        for(  RowCount = 0; RowCount < RowsTotal; RowCount++ )
          {
          // Receive the broad cast from each row inluding thie row context in this column
          BegLogLine(1) << "TestThread: Start ColReduce:Push subCol " << RowCount << EndLogLine;
          ColReduce[ RowCount ].Push( MyRowsPositionSet, StepNo );
          BegLogLine(1) << "TestThread: done  ColReduce:Push subCol " << RowCount << EndLogLine;
          }

        // Wait on this row only
        BegLogLine(1) << "TestThread: start ColReduce:Push() subCol " << MyRow << EndLogLine;
        ColReduce[ MyRow ].WaitPull( MyRowsPositionSet, StepNo );
        BegLogLine(1) << "TestThread: done  ColReduce:Push() subCol " << MyRow << EndLogLine;
#endif

    BegLogLine(1) << " TestThread: Start RowRecuce:Push " << MyRow << EndLogLine;
    RowReduce[ MyRow ].Push( MyRowsForceSet, StepNo );
    BegLogLine(1) << " TestThread: Done  RowRecuce:Push " << MyRow << EndLogLine;

    if( RowReduce[ MyRow ].AmRoot() )
      {
      BegLogLine(1) << " TestThread: Start RowRecuce:WaitPull " << MyRow << EndLogLine;
      RowReduce[ MyRow ].WaitPull( MyRowsForceSet, StepNo );
      BegLogLine(1) << " TestThread: Done  RowRecuce:WaitPull " << MyRow << EndLogLine;
      UpdateVelocityAndPosition( MyRowsPositionSet, MyRowsVelocitySet, MyRowsForceSet );
      }
    else
      {
      BegLogLine(1) << " TestThread: Not Root in RowRecuce:WaitPull " << MyRow << EndLogLine;
      }
    }

  BegLogLine(1) << " TestThread: Done Steps: " << StepNo << EndLogLine;
  BegLogLine(1) << " TestThread: Done Steps: " << StepNo << EndLogLine;
  printf("TestThread: Done.  Steps: %d\n", StepNo );
  fflush(stdout);
  return(0);  // reactor exit code to release buffer
  }

//ThreadCreate StaticStartThread( -1,                   // -1 is the flag for 'local'
//                                TestThread,  // What function to run
//                                NULL,                 // no arg pointer
//                                0 );                  // no len
//

void PkMain(int argc, char **argv, char **envp)
  {

    BegLogLine( PKFXLOG_PKREACTOR )
      << " PKMAIN() "
      << " Send Triggers... "
      << EndLogLine;

   // Start a test thread on each node.
   for( int i = 0; i < MpiSubSys::GetTaskCnt(); i++ )
     {
     ThreadCreate( i, TestThread, NULL, 0 );

     sleep( 1 ); // might make mpich work
     }

    BegLogLine( PKFXLOG_PKREACTOR )
      << " PKMAIN() "
      << " Sent Triggers... "
      << EndLogLine;


  while(1)
    sched_yield();


  }





