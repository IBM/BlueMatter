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
 * Project:         MPI Geometric
 *
 * Module:          mpig.c
 *
 * Purpose:         Implements MPI Geometric (MPIG)
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         Mar 14, 2003 BGF Created.
 *
 ***************************************************************************/


#include "mpig.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>

#define MPIG_NOT_INITIALIZED (-11112)  /* should be defined in terms of mpi.h error codes */
#define MPIG_FAILURE (-11111)  /* should be defined in terms of mpi.h error codes */


MPI_Comm MPI_CART_COMM_WORLD;

typedef struct t_MPIG_Coordinate
  {
  /* sign bit - 0 (non-negative), for valid ordinal or coordinate, 1 for meta data value */
  unsigned int mReserved  : 1;
  /* type bit - 0 for task ordinals, 1 for task coordinates */
  unsigned int mTypeFlag  : 1;
  unsigned int mX         : 10 ;
  unsigned int mY         : 10 ;
  unsigned int mZ         : 10 ;
  } MPIG_Coordinate;

typedef struct T_MPIG_STATE
  {
  int    * mPosix_argc;
  char *** mPosix_argv;
  char *** mPosix_envp;

  int mInitializedFlag;

  /*
  // Torus partition
  */
  int mNodesX;
  int mNodesY;
  int mNodesZ;

  int mMyX;
  int mMyY;
  int mMyZ;

  int mMpiRank;
  int mMpiCount;

  } MPIG_STATE;

MPIG_STATE MpigState;

/*
 * CONVERT MPI RANK TO COORDS - THIS IS THE MOST IMPORTANT ROUTINE AFTER DECIDING DIMS
 * Conversion from MPI ordinal rank to 3D coord
 */
int
MPIG_ConvertRankToCoord( int aRank, int *aX, int *aY, int *aZ )
  {
  assert( aRank >= 0 && aRank < MpigState.mMpiCount );
  int rank = aRank;
  *aX      = rank / (MpigState.mNodesY * MpigState.mNodesZ);
  rank     = rank % (MpigState.mNodesY * MpigState.mNodesZ);
  *aY      = rank / MpigState.mNodesZ;
  rank     = rank % MpigState.mNodesZ;
  *aZ      = rank;
  return( MPI_SUCCESS );
  }


int
MPIG_ConvertCoordsToRank( int x_coord, int y_coord, int z_coord, int *aMpiRank  )
  {
  assert( MpigState.mInitializedFlag );
  int Rc = MPI_SUCCESS;
  int rank = ( x_coord )
           + ( y_coord * MpigState.mNodesX )
           + ( z_coord * MpigState.mNodesX * MpigState.mNodesY ) ;

  if( rank >= 0 && rank < MpigState.mMpiCount )
    {
    *aMpiRank = rank;
    }
  else
    {
    Rc = MPIG_FAILURE;
    }
  return( Rc );
  }

/*
 *MACHINE SIZE
 */

int
MPIG_GetSize( int *x, int *y, int *z )
  {
  if( MpigState.mInitializedFlag )
    {
    *x = MpigState.mNodesX;
    *y = MpigState.mNodesY;
    *z = MpigState.mNodesZ;
    return( MPI_SUCCESS );
    }
  return( MPIG_NOT_INITIALIZED ) ;
  }

int
MPIG_GetSizeX()
  {
  int x, y, z;
  int rc;
  rc = MPIG_GetSize( &x, &y, &z );
  assert( rc == 0 );
  return(x);
  }

int
MPIG_GetSizeY()
  {
  int x, y, z;
  int rc;
  rc = MPIG_GetSize( &x, &y, &z );
  assert( rc == 0 );
  return(y);
  }

int
MPIG_GetSizeZ()
  {
  int x, y, z;
  int rc;
  rc = MPIG_GetSize( &x, &y, &z );
  assert( rc == 0 );
  return(z);
  }

/*
 * My Coord
 */

int
MPIG_GetCoord( int *x, int *y, int *z )
  {
  if( MpigState.mInitializedFlag )
    {
    *x = MpigState.mMyX;
    *y = MpigState.mMyY;
    *z = MpigState.mMyZ;
    return( MPI_SUCCESS );
    }
  return( MPIG_NOT_INITIALIZED ) ;
  }

int
MPIG_GetCoordX()
  {
  assert( MpigState.mInitializedFlag );
  return( MpigState.mMyX );
  }

int
MPIG_GetCoordY()
  {
  assert( MpigState.mInitializedFlag );
  return( MpigState.mMyY );
  }

int
MPIG_GetCoordZ()
  {
  assert( MpigState.mInitializedFlag );
  return( MpigState.mMyZ );
  }

void
FindBestCubicDimension( int aCount , int *aX, int *aY, int *aZ )
  {
  /*
   * SURE - THERE IS A BETTER WAY TO DO WHAT IS IN THIS ROUTINE - SO, GO AHEAD AND DO IT!
   */

  /*
   * ensure that if fixed dims are given, the are given in x y z order (y can't be given without x, etc)
   */
  if( *aZ == -1 )
    assert( *aY == -1 );
  if( *aY == -1 )
    assert( *aX == -1 );

  /*
   * Now, if aX was NOT spec'd, then some scanning is needed
   */
  if( *aX == -1 )
    {
    double volume = aCount;

    /* Set up assuming worst surface to volume */
    double BestS2V =  (4.0 * aCount + 2.0 ) / volume;
    int    BestX = aCount;
    int    BestY = 1;
    int    BestZ = 1;

    int xstart, xend, ystart, yend, zstart, zend;

    if( *aY != -1 )
      {
      ystart = *aY;
      yend   = *aY;
      }
    else
      {
      ystart = 1;
      yend   = aCount;
      }

    if( *aX != -1 )
      {
      xstart = *aX;
      xend   = *aX;
      }
    else
      {
      xstart = 1;
      xend   = aCount;
      }

    for( int z = 1; z < aCount; z++ )
      {
      if( *aY = -1 )
        ystart = z;
      for( int y = ystart; y < yend; y++ )
        {
        if( *aX == -1 )
          xstart = y;
        for( int x = xstart; x < xend; x++ )
          {
          if( x * y * z != aCount )
            continue;
          double fx = x;
          double fy = y;
          double fz = z;
          double surface = 2 * ( fx * fy + fy * fz + fz * fx );
          double s2v = surface / volume;
          if( s2v <= BestS2V )
            {
            BestS2V = s2v;
            BestX   = x;
            BestY   = y;
            BestZ   = z;
            }
          }
        }
      }
    *aX = BestX;
    *aY = BestY;
    *aZ = BestZ;
    }
  /*
   * check if the results are good, and blow out hard if not.
   */
  if( *aX * *aY * *aZ != aCount )
    {
    fprintf( stderr, "FATAL ERROR, Dim spec X=%d Y=%d Z=%d so X*Y*Z (%d) != %d partition size\n",
       *aX, *aY, *aZ,
       *aX * *aY * *aZ,
       aCount );
    fflush(stderr);
    assert( *aX * *aY * *aZ == aCount );
    exit( -1 ); /* in case NDEBUG takes out assert. */
    }

  return;
  }

/*
 * Try to create a reasonable 3D torus out of the mpi partition - fail if not roundly possible.
 * -1 in args frees that dim
 */
int
MpiTorusInit(int aX, int aY, int aZ )
  {

  MPI_Comm_rank(MPI_COMM_WORLD, &MpigState.mMpiRank );
  MPI_Comm_size(MPI_COMM_WORLD, &MpigState.mMpiCount );

  MpigState.mNodesX = aX;
  MpigState.mNodesY = aY;
  MpigState.mNodesZ = aZ;

  FindBestCubicDimension(  MpigState.mMpiCount,
                          &MpigState.mNodesX,
                          &MpigState.mNodesY,
                          &MpigState.mNodesZ );

  fprintf( stderr, "MpiTorusInit(): for partition of %d nodes, Torus Dims: X: %d Y: %d Z: %d\n",
           MpigState.mMpiCount, MpigState.mNodesX, MpigState.mNodesY, MpigState.mNodesZ );
  fflush( stderr );


  MPIG_ConvertRankToCoord( MpigState.mMpiRank, &MpigState.mMyX, &MpigState.mMyY, &MpigState.mMyZ );

  fprintf( stderr, "MpiTorusInit(): This node is X: %d Y: %d Z: %d\n",
           MpigState.mMyX, MpigState.mMyY, MpigState.mMyZ );
  fflush( stderr );

  return(0);
  }

int
MPIG_InitSize( int *argc, char ***argv, int x, int y, int z )
  {
  int flag;

  MPI_Initialized( &flag );

  if( ! flag )
    {
    fprintf(stderr, "BladeInit() calling Mpi init\n" );
    MPI_Init( argc, argv );  /* initialize MPI environment */
    }

  /*
   * Remember - there is an extra level of indirection so that we point
   * to the origional argc/argv provided by the call to main.
   */

  MpigState.mPosix_argc = argc;
  MpigState.mPosix_argv = argv;

  /* -1 is the flag that tells this call 'don't know'
   *
   */

  int rc = MpiTorusInit( x, y, z );

  /*
   * Create MPI_Cart_comm_world
   */

  int dims[3];
  dims[0] = MpigState.mNodesX;
  dims[1] = MpigState.mNodesY;
  dims[2] = MpigState.mNodesZ;

  int periods[3];
  periods[0] = 0;
  periods[1] = 0;
  periods[2] = 0;


  int cart_create_rc = MPI_Cart_create( MPI_COMM_WORLD, 3, dims, periods, 0, &MPI_CART_COMM_WORLD );
  assert( cart_create_rc == MPI_SUCCESS );

  MpigState.mInitializedFlag = 1;

  return( rc );
  }

int
MPIG_Init( int *argc, char ***argv )
  {
  /*
  // Call intialize with flags to find best cubic dim
  */
  MPIG_InitSize( argc, argv, -1, -1, -1 );
  }

MPIG_Finalize()
  {
  MPI_Finalize();
  }


int
AttachDebugger( char *file )
  {
  /*
   * To use this, simply edit the display name to put debug windows where
   * you want.  This could be modified to be a routine automatically called
   * when an assert fails.
   */

/*================= begin sample pkdebug file  ===========
echo $0
echo $1
echo $2
echo $3
echo $4
echo $5
# NOTE: XLDB won't find files in DFS.  Set GFEBUILDROOT to a local files system if you want source debuggine.
echo $GFEBUILDROOT
#if (( $2 == 0 )) then
echo xldb -i 14 -i 15 -i 23  -bw -a $1 -title $3 $6 -I./
xldb  -i 14 -i 15 -i 23 -bw -a $1 -title $3 $6 -I./
#fi
=================== end sample pkdebug file ================= */
  char hostname[80];
  char db_cmd[1024];

  gethostname( hostname, 80 );

  int TaskNo;
  MPI_Comm_rank(MPI_COMM_WORLD, &TaskNo );
  int TaskCnt;
  MPI_Comm_size(MPI_COMM_WORLD, &TaskCnt );

  sprintf( db_cmd,
           "%s %d %d [%d,%d,%d] %d %s %s &",
           "pkdebug",
           getpid(),
           TaskNo,
           MpigState.mMyX, MpigState.mMyY, MpigState.mMyZ,
           TaskCnt,
           hostname,
           file );

  fprintf(stderr, "%s", db_cmd);
  fflush(stderr);
  system( db_cmd );
  sleep(1); sleep(1); sleep(1); sleep(1); sleep(1);
  sleep(1); sleep(1); sleep(1); sleep(1); sleep(1);
  /*
  kill( getpid(), 18);
  */
  return(0);
  }


