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
 * Module:          BladeMpi.cpp
 *
 * Purpose:         Implements blade functions on mpi w/ active messages.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         Dec 10, 2002 BGF Created.
 *
 ***************************************************************************/


#include "BladeMpi.hpp"

int MPI_STATUS_TAG(MPI_Status Status ) { return( Status.tag ); }
int MPI_STATUS_SOURCE(MPI_Status Status ) { return( Status.source ); }
int MPI_STATUS_ERROR(MPI_Status Status ) { return( Status.error ); }

#ifndef PK_THREAD_STACKSIZE_MB
#define PK_THREAD_STACKSIZE_MB 10
#endif

// A reactor thread function must be linked in.
// NEED: try to encapsulate this so that there is a common interface to starting the reactor.
//extern void * ReactorThreadMain( void * );
//extern void * ReactorThread( void * );
//extern void   ReactorInitialize();
//extern void * BladeMain( int, char**, char** );

#if 0
#include <pthread.h>
#endif

#if 0
#ifdef PK_AIX
#include <pk/setpri.hpp>
#include <sys/processor.h>
#include <sys/thread.h>
#endif
#endif

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>

typedef struct T_BLIMPI_STATE
  {
  int    *  mPosix_argc;
  char *** mPosix_argv;
  char *** mPosix_envp;


  int mMpiRank;
  int mMpiCount;

  int mMyX;
  int mMyY;
  int mMyZ;

  int mNodesX;
  int mNodesY;
  int mNodesZ;

  int mRankXYZ;

  } BLIMPI_STATE;

BLIMPI_STATE BS;


// Obtain this Node's coordinates in the Parition.
// Coordinates range from (0 to x-1, 0 to y-1, 0 to z-1).
void BGLPartitionGetCoords( int *x, int *y, int *z )
  {
  *x = BS.mMyX;
  *y = BS.mMyY;
  *z = BS.mMyZ;
  }

// Obtain the Partition's Maximum Extents (0 to x-1, 0 to y-1, 0 to z-1)
void BGLPartitionGetExtents( int *max_x, int *max_y, int *max_z )
  {
  *max_x = BS.mNodesX - 1 ;
  *max_y = BS.mNodesY - 1 ;
  *max_z = BS.mNodesZ - 1 ;
  }

// Obtain the Dimensions of the Partition (number of nodes, not indices)
void BGLPartitionGetDimensions( int *x_nodes, int *y_nodes, int *z_nodes )
  {
  *x_nodes = BS.mNodesX  ;
  *y_nodes = BS.mNodesY  ;
  *z_nodes = BS.mNodesZ  ;
  }

// Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.
int BGLPartitionGetRankXYZ( void )
  {
  return( BS.mRankXYZ );
  }

// Create the Rank of the node at coordinates when X varies fastest.
//  Note: order of args is X, Y, Z.
int BGLPartitionMakeRankXYZ( int x_coord, int y_coord, int z_coord )
  {
  int rank = ( x_coord )
           + ( y_coord * BS.mNodesX )
           + ( z_coord * BS.mNodesX * BS.mNodesY ) ;
  return(rank);
  }

//***********  Packet layer ******************************


// Simple H/W + S/W Torus Header Creation using suitable defaults
//  Defaults used:  Point-to-Point,
//                  Normal Priority,
//                  Dynamic on VCD0,
//                  Hardware applied Hint Bits, and
//                  Checksum will skip only Hardware Header.
//  Hdr address must be aligned on 16Byte boundary, i.e. quadword aligned.
_BGL_TorusPktHdr *
BGLTorusMakeHdr( _BGL_TorusPktHdr *hdr, // Filled in on return
                 int dest_x,            // destination coordinates
                 int dest_y,
                 int dest_z,
                 int dest_f,            // destination Fifo Group
                 //int chunks,            // sizeof packet in chunks (1 to 8, pkt layer subtracts 1)
                 //int (*actor)(),        // Active Packet Function matching signature above
                 void *actor,        // Active Packet Function matching signature above
                 Bit32 arg,             // primary argument to actor
                 Bit32 extra,           // secondary 10bit argument to actor
                 int signature )        // Actor Signature from above (must match Actor)
  {

  hdr->hwh0.CSum_Skip = 0 ;           //  : 7; // Number of shorts (2B) to skip in CheckSum.
  hdr->hwh0.Sk        = 0 ;           //  : 1; // 0=use CSum_Skip, 1=Skip entire pkt.
  hdr->hwh0.Hint      = 0 ;           //  : 6; // Hint Bits
  hdr->hwh0.Dp        = 0 ;           //  : 1; // Deposit Bit for Class Routed MultiCast
  hdr->hwh0.Pid       = dest_f ;      //  : 1; // Destination Fifo Group
//  hdr->hwh0.Chunks    = chunks ;      //  : 3; // Size in Chunks of 32B (0 for 1 chunk, ... , 7 for 8 chunks)
  hdr->hwh0.SW_Avail  = 0 ;           //  : 2; // Available for S/W Use.
  hdr->hwh0.Dynamic   = 0 ;           //  : 1; // 1=Dynamic Routing, 0=Determinstic Routing.
  hdr->hwh0.VC        = 0 ;           //  : 2; // Virtual Channel (0=D0,1=D1,2=BN,3=BP)
  hdr->hwh0.X         = dest_x ;      //  : 8; // Destination X Physical Coordinate

  hdr->hwh1.Y         = dest_y ;      //  : 8; // Destination Y Physical Coordinate
  hdr->hwh1.Z         = dest_z ;      //  : 8; // Destination Z Physical Coordinate
  hdr->hwh1.Resvd0    = 0 ;           //  : 8; // Reserved (checksum)
  hdr->hwh1.Resvd1    = 0 ;           //  : 8; // Reserved (checksum)

  hdr->swh0.extra     = extra ;           //  : 10; // additional untyped 10bit argument (may become 8bits)

  hdr->swh0.fcn       = ( (unsigned) actor & 0x00FFFFFF ) >> 2 ;           //  : 22; // "massaged" Active Function Pointer (not directly usable)

  // printf(" Mangled function pointer from %08X to %08X\n", actor, hdr->swh0.fcn );

  hdr->swh1.arg       = arg ;           // untyped 32bit argument

  return( hdr );
  }

MPI_Comm REACTIVE_MESSAGE_COMM_WORLD;

// Use for unaligned data or bytes < full chunks
void
BGLTorusSendPacketUnaligned( _BGL_TorusPktHdr *hdr,   // Previously created header to use
                             void *data,              // Source address of data
                             int bytes )             // Payload bytes <= ((chunks * 32) - 16)
  {
  int MpiDest = BGLPartitionMakeRankXYZ( hdr->hwh0.X, hdr->hwh1.Y, hdr->hwh1.Z );

  // NEED: should this be overwritten here ?
  int i = (bytes + _BGL_TORUS_SIZEOF_CHUNK - 1) / _BGL_TORUS_SIZEOF_CHUNK ;
  hdr->hwh0.Chunks = i - 1;
  assert( hdr->hwh0.Chunks < _BGL_TORUS_PKT_MAX_CHUNKS );

  assert( sizeof( _BGL_TorusPktHdr ) <= _BGL_TORUS_SIZEOF_CHUNK );

  // No of chunks indicated by the data plus 1 for the headre
  int TrueLen =  (hdr->hwh0.Chunks + 1) * _BGL_TORUS_SIZEOF_CHUNK;

  // Tank here - if this goes off, the packet will never be received by MPI
  assert( TrueLen <= sizeof( _BGL_TorusPkt ) );

  _BGL_TorusPkt Pkt;
  Pkt.hdr = *hdr;
  assert( bytes <= sizeof( Pkt.payload ) );
  memcpy( Pkt.payload, data, bytes );

  int  mpirc = MPI_Send((void *) &Pkt,
                        TrueLen,
                        MPI_CHAR,                              // Send data and function pointer to
                        MpiDest,                                // specified task.
                        NULL,                             ///(int) FxPtr,
                        REACTIVE_MESSAGE_COMM_WORLD );

  if( mpirc != 0 )
    {
    MPI_Abort( REACTIVE_MESSAGE_COMM_WORLD, mpirc );
    }

  return;
  }


void
BGLTorusSimpleActivePacketSend( void *ActorFx, int x, int y, int z, int len, void *data )
  {
  // NEED TO SHORT CUT LOCAL SENDS

  _BGL_TorusPkt    Pkt;
  int chunks = 7;
  char *str = (char *) Pkt.payload;

  BGLTorusMakeHdr( & Pkt.hdr,               // Filled in on return
                     x,y,z,                 // destination coordinates
                     0,          //???      // destination Fifo Group
                     ActorFx,       // Active Packet Function matching signature above
                     0,                     // primary argument to actor
                     0,                     // secondary 10bit argument to actor
                     _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

  memcpy( Pkt.payload, data, len );
  int chunk_count = (len +  _BGL_TORUS_SIZEOF_CHUNK - 1) /  _BGL_TORUS_SIZEOF_CHUNK;

  int rc = 0;

  // local actors to occur on caller's stack
  if( (x == BS.mMyX) && (y == BS.mMyY) && (z == BS.mMyZ) )
    {
    ((BGLTorusBufferedActivePacketHandler)(ActorFx))( & Pkt.payload, Pkt.hdr.swh1.arg, Pkt.hdr.swh0.extra );
    }
  else
    {
    BGLTorusSendPacketUnaligned( & Pkt.hdr ,      // Previously created header to use
                                   Pkt.payload,     // Source address of data (16byte aligned)
                                   len ); // Payload bytes = ((chunks * 32) - 16)
    }
  }


void
FindBestCubicDimension( int aCount , int *aX, int *aY, int *aZ )
  {
  double volume = aCount;

  double BestS2V =  (4.0 * aCount + 2.0 ) / volume;
  int    BestX = 1;
  int    BestY = 1;
  int    BestZ = aCount;

  for( int z = 1; z < aCount; z++ )
  for( int y = z; y < aCount; y++ )
  for( int x = y; x < aCount; x++ )
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

  *aX = BestX;
  *aY = BestY;
  *aZ = BestZ;

  return;
  }




_BGL_TorusPkt  *AllBuf     ;    // A pointer to an array of pointers.
MPI_Request    *AllRequests;
MPI_Status     *AllStatus  ;


#define PKREACTOR_THREAD_STACK_SIZE_MB     (10)

#ifndef PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT
#define PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT (64)
#endif

#define TOTAL_DATAGRAMS_IN_FLIGHT \
        (PKREACTOR_DATAGRAMS_PER_TASK_IN_FLIGHT * BS.mMpiCount )

void * ReactorThreadMain( void * );

//-----------------------------------------------------------------------------
// This constructor is called during the init of the
// pk. It can be assumed to be running in all tasks.

void
ReactorInitialize()
  {

  AllBuf      = new _BGL_TorusPkt [ TOTAL_DATAGRAMS_IN_FLIGHT ];
  AllRequests = new MPI_Request   [ TOTAL_DATAGRAMS_IN_FLIGHT ];
  AllStatus   = new MPI_Status    [ TOTAL_DATAGRAMS_IN_FLIGHT ];

  assert( AllBuf      != NULL );
  assert( AllRequests != NULL );
  assert( AllStatus   != NULL );

  MPI_Group REACTIVE_MESSAGE_GROUP;

  MPI_Comm_group( MPI_COMM_WORLD, &REACTIVE_MESSAGE_GROUP); //Make REACTIVE_MESSAGE_GROUP

  MPI_Comm_create( MPI_COMM_WORLD,
                   REACTIVE_MESSAGE_GROUP,        //Create a new communicator called
                  &REACTIVE_MESSAGE_COMM_WORLD);  // REACTIVE_MESSAGE_COMM_WORLD that includes
                                                         // all processes in REACTIVE_MESSAGE_GROUP.
  int t;
  for( t = 0; t<TOTAL_DATAGRAMS_IN_FLIGHT;t++)
    {
    int msgrc;

    msgrc = MPI_Irecv( (void *) & AllBuf[t],             //Post non-blocking receive for
                                  sizeof( _BGL_TorusPkt ),               // each request.
                                  MPI_CHAR,
                                  MPI_ANY_SOURCE,
                                  MPI_ANY_TAG,
                                  REACTIVE_MESSAGE_COMM_WORLD,
                                 &AllRequests[t]);
    }

  for( ; t < TOTAL_DATAGRAMS_IN_FLIGHT; t++ )                 //Initialise requests to NULL
    {
    AllRequests[t] = MPI_REQUEST_NULL;
    }

  //NEED: might want to ponder this here even in MPI world.
  MPI_Barrier( REACTIVE_MESSAGE_COMM_WORLD );         //Wait for all processes to get here.


  pthread_t                    tid;
  pthread_attr_t               attr;

  pthread_attr_init( &( attr ) );
  pthread_attr_setdetachstate( &( attr ), PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize(   &( attr ), PKREACTOR_THREAD_STACK_SIZE_MB * 1024*1024 ) ;
  errno = 0;
  int rc = pthread_create( &( tid  ), //NEED: to think about a potential race cond here.
                           &( attr ),
                           ReactorThreadMain,
                          (void *) 0 );

  // NEED TO DO SCHED_TUNE!!!!

  }



void*
ReactorThreadMain( void *arg )     //This goes round and round and round...........
  {
  int mpirc;
  int ReactorRc;

  MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.

  // Continuous loop
  for(int d = 0; (1) ; d++)
    {
    unsigned msgrc;
    MPI_Status Status;
    int ReqIndex;

    int Flag = 0;

    for( int PollCount = 0; Flag == 0; PollCount++ )   //Loop until a request is complete
      {
      mpirc = MPI_Testany(TOTAL_DATAGRAMS_IN_FLIGHT, AllRequests, &ReqIndex, &Flag, &Status);
      assert( mpirc == 0);
      if( ! Flag )
        sched_yield();
      }

    int Len;

    MPI_Get_count( &Status, MPI_CHAR, &Len );//Use Status retuurned above to get message length.

    _BGL_TorusPkt  *Pkt = &( AllBuf[ ReqIndex ] );

    unsigned actor = Pkt->hdr.swh0.fcn;
    actor = ( actor << 2 ) | ((( unsigned ) ReactorThreadMain ) & 0xFF000000 );

    // printf("Unmangled function pointer from %08X to %08X\n", Pkt->hdr.swh0.fcn, actor );

    ((BGLTorusBufferedActivePacketHandler)(actor))( & Pkt->payload, Pkt->hdr.swh1.arg, Pkt->hdr.swh0.extra );


    /////  ReactorRc = (Platform::Reactor::FunctionPointerType)(MPI_STATUS_TAG( Status ))( AllBuf[ReqIndex] );

    msgrc = MPI_Irecv( (void *) & AllBuf[ReqIndex],      //Post a non-blocking receive on the
                              sizeof(  _BGL_TorusPkt ),               //buffer we just used.
                              MPI_CHAR,
                              MPI_ANY_SOURCE,
                              MPI_ANY_TAG,
                              REACTIVE_MESSAGE_COMM_WORLD,
                             &AllRequests[ReqIndex] );


    if( msgrc != 0 )
      {
      MPI_Abort( MPI_COMM_WORLD, msgrc );
      }
    }

  MPI_Finalize();
  _exit(0);
  return( NULL ); // Keep compiler quiet
  }


int
AttachDebugger( char *file )
  {
  // To use this, simply edit the display name to put debug windows where
  // you want.  This could be modified to be a routine automatically called
  // when an assert fails.

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
           BS.mMyX, BS.mMyY, BS.mMyZ,
           TaskCnt,
           hostname,
           file );

  fprintf(stderr, "%s", db_cmd);
  fflush(stderr);
  system( db_cmd );
  sleep(1); sleep(1); sleep(1); sleep(1); sleep(1);
  sleep(1); sleep(1); sleep(1); sleep(1); sleep(1);
  /* kill( getpid(), 18); */
  return(0);
  }

// This is the main that MPI will call.
// BladeMain invoked by this routine.
int InnerBladeInit()
  {
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sigaction(SIGSEGV, &sa, NULL);

  nice(1);

  MPI_Comm_rank(MPI_COMM_WORLD, &BS.mMpiRank );
  MPI_Comm_size(MPI_COMM_WORLD, &BS.mMpiCount );

  FindBestCubicDimension( BS.mMpiCount, &BS.mNodesX, &BS.mNodesY, &BS.mNodesZ );

  printf("MpiCount %d, Dim: X %d Y %d Z %d\n",
          BS.mMpiCount, BS.mNodesX, BS.mNodesY, BS.mNodesZ );

  int rank = BS.mMpiRank;
  BS.mMyZ  = rank / (BS.mNodesY * BS.mNodesX);
  rank     = rank % (BS.mNodesY * BS.mNodesX);
  BS.mMyY  = rank / BS.mNodesX;
  rank     = rank % BS.mNodesX;
  BS.mMyX  = rank;


  fprintf(stderr,"rank %d MpiRank %d - XYZ %d %d %d \n", rank, BS.mMpiRank, BS.mMyX, BS.mMyY, BS.mMyZ );
  BS.mRankXYZ = BGLPartitionMakeRankXYZ( BS.mMyX, BS.mMyY, BS.mMyZ );


  assert( BS.mRankXYZ == BS.mMpiRank );

#if 0
  // This makes sure that all the executables in the partition are identical
  FILE *fp = fopen( argv[0], "r" );
  assert( fp != NULL );
  // Add up the first bit of the executable file.
  for(int b = 0; (b < PK_MAX_EXCUTABLE_CHECKSUM) && (! feof( fp )); b++ )
    _pk_ExecutableCheckSum += fgetc(fp);
  fclose( fp );
#endif

  int MpiRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &MpiRank );
  printf("MpiRank %d\n", MpiRank );

  ReactorInitialize();

  return(0);

  }


int BladeInit(int *argc, char ***argv)
  {
  int flag;

  MPI_Initialized( &flag );

  if( ! flag )
    {
    fprintf(stderr, "BladeInit() calling Mpi init\n" );
    MPI_Init( argc, argv );  /* initialize MPI environment */
    }

  //Remember - there is an extra level of indirection so that we point
  // to the origional argc/argv provided by the call to main.

  BS.mPosix_argc = argc;
  BS.mPosix_argv = argv;

  int rc = InnerBladeInit();

  return( rc );
  }

