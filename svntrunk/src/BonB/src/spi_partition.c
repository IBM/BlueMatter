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
 /* -------------------------------------------------------------  */
/* Product(s):                                                    */
/* 5733-BG1                                                       */
/*                                                                */
/* (C)Copyright IBM Corp. 2004, 2004                              */
/* All rights reserved.                                           */
/* US Government Users Restricted Rights -                        */
/* Use, duplication or disclosure restricted                      */
/* by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                */
/* Licensed Materials-Property of IBM                             */
/* -------------------------------------------------------------  */
//
//      File: blade/spi/bgl_partition_spi.c
//
//   Purpose: Implement Blade's Partition System Programming Interface.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: See include/spi/BGL_PartitionSPI.h
//
//   History: 05/15/2002: MEG - Created.
//
//
#include <BonB/blade_on_blrts.h>
#include <stdio.h>
#include <stdlib.h> // for exit
#include <stdarg.h>
#include <assert.h>
#include <string.h>

__BEGIN_DECLS

//
// Obtain the Frequency of the Partition (note: could be a lie).
//
int BGLPartitionGetMHz( void )
{
   return( _Blade_Config_Area.freq_in_MHz );
}

// Obtain the amount of memory in the Partition.
//  Note: The partition will be configured for the least amount
//    of memory in any node in the partition.
//  Return values: 0: 256MB
//                 1: 512MB
//                 2: 1GB
//                 3: 2GB
int BGLPartitionGetDDRSize( void )
{
   return( _Blade_Config_Area.bs_flags.ddr_size );
}

// Obtain this Node's coordinates in the Parition.
//  Coordinates range from (0 to x-1, 0 to y-1, 0 to z-1).
void BGLPartitionGetCoords( int *x, int *y, int *z )
{
   *x = _Blade_Config_Area.X;
   *y = _Blade_Config_Area.Y;
   *z = _Blade_Config_Area.Z;
}


// Obtain the Partition's Maximum Extents (0 to x-1, 0 to y-1, 0 to z-1)
void BGLPartitionGetExtents( int *max_x, int *max_y, int *max_z )
{
   *max_x = _Blade_Config_Area.max_X;
   *max_y = _Blade_Config_Area.max_Y;
   *max_z = _Blade_Config_Area.max_Z;
}


// Obtain the Dimensions of the Partition (number of nodes, not indices)
void BGLPartitionGetDimensions( int *x_nodes, int *y_nodes, int *z_nodes )
{
   *x_nodes = _Blade_Config_Area.nodes_X;
   *y_nodes = _Blade_Config_Area.nodes_Y;
   *z_nodes = _Blade_Config_Area.nodes_Z;
}

// Obtain the number of compute nodes in the partition.
int BGLPartitionGetSize( void )
{
  int x,y,z;
  BGLPartitionGetDimensions( &x, &y, &z );
  return( x*y*z );
}

// Obtain the number of compute nodes in the partition.
int BGLPartitionGetNumNodesCompute( void )
{
   return( BGLPartitionGetSize() );
}

// Obtain the number of I/O nodes in the partition.
int BGLPartitionGetNumNodesIO( void )
{
   return( _Blade_Config_Area.nodes_IO );
}

// Obtain the total number of nodes in the partition (compute + I/O).
int BGLPartitionGetNumNodes( void )
{
   return( BGLPartitionGetNumNodesCompute() + BGLPartitionGetNumNodesIO() );
}

// Returns 1 if the caller is running on an I/O node; 0 otherwise.
int BGLPartitionIsIONode( void )
{
   return( _Blade_Config_Area.is_io_node );
}

// Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.
//  This can differ from Real Rank if Single-Node LoopBack test.
int BGLPartitionGetRank( void )
{
   return( _Blade_Config_Area.Rank );
}

// Obtain this Node's Real Rank (range is 0 to Nodes-1) when X varies fastest.
//  If Single-Node LoopBack test, this is our real rank in the partition, and
//  is the only difference between all nodes in the partition.  Always equal to
//  Rank if not Single-Node LoopBack.
int BGLPartitionGetRealRank( void )
{
   return( _Blade_Config_Area.real_Rank );
}

// Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.
int BGLPartitionGetRankXYZ( void )
{
   return( _Blade_Config_Area.Rank );
}


// Obtain this Node's Rank (range is 0 to Nodes-1) when Z varies fastest.
int BGLPartitionGetRankZYX( void )
{
   return(  _Blade_Config_Area.Z +
           (_Blade_Config_Area.Y * _Blade_Config_Area.nodes_Z) +
           (_Blade_Config_Area.X * _Blade_Config_Area.nodes_Z * _Blade_Config_Area.nodes_Y)
         );
}

// Create from Rank of the node the coordinates when X varies fastest.
//  Note: order of args is X, Y, Z.
void BGLPartitionMakeXYZ( int rank, int *x_coord, int *y_coord, int *z_coord )
{
  int nodes_X = _Blade_Config_Area.nodes_X;
  int nodes_Y = _Blade_Config_Area.nodes_Y;
  int nodes_Z = _Blade_Config_Area.nodes_Z;
  if (rank < nodes_X * nodes_Y * nodes_Z) {
    *z_coord = rank / (nodes_X * nodes_Y);
    *y_coord = (rank - (*z_coord * nodes_X * nodes_Y)) / nodes_X;
    *x_coord =  rank - (*z_coord * nodes_X * nodes_Y) - (*y_coord * nodes_X );
  } else {
    *x_coord = rank;
    *y_coord = 0;
    *z_coord = 0;
  }
  return;
}

// Create from Rank of the node the coordinates when Z varies fastest.
//  Note: order of args is X, Y, Z.
void BGLPartitionMakeZYX( int rank, int *x_coord, int *y_coord, int *z_coord )
{
  int nodes_X = _Blade_Config_Area.nodes_X;
  int nodes_Y = _Blade_Config_Area.nodes_Y;
  int nodes_Z = _Blade_Config_Area.nodes_Z;
  if (rank < nodes_X * nodes_Y * nodes_Z) {
    *x_coord = rank / (nodes_Y * nodes_Z);
    *y_coord = (rank - (*x_coord * nodes_Y * nodes_Z)) / nodes_Z;
    *z_coord =  rank - (*x_coord * nodes_Y * nodes_Z) - (*y_coord * nodes_Z );
  } else {
    *z_coord = rank;
    *y_coord = 0;
    *x_coord = 0;
  }
  return;
}

// Create the Rank of the node at coordinates when X varies fastest.
//  Note: order of args is X, Y, Z.
int BGLPartitionMakeRank( int x_coord, int y_coord, int z_coord )
{
   return(  x_coord +
           (y_coord * _Blade_Config_Area.nodes_X) +
           (z_coord * _Blade_Config_Area.nodes_X * _Blade_Config_Area.nodes_Y)
         );
}

// Create the Rank of the node at coordinates when X varies fastest.
//  Note: order of args is X, Y, Z.
int BGLPartitionMakeRankXYZ( int x_coord, int y_coord, int z_coord )
{
   return(  x_coord +
           (y_coord * _Blade_Config_Area.nodes_X) +
           (z_coord * _Blade_Config_Area.nodes_X * _Blade_Config_Area.nodes_Y)
         );
}


// Create the Rank of the node at coordinates when Z varies fastest.
//  Note: order of args is Z, Y, X.
int BGLPartitionMakeRankZYX( int z_coord, int y_coord, int x_coord )
{
   return(  z_coord +
           (y_coord * _Blade_Config_Area.nodes_Z) +
           (x_coord * _Blade_Config_Area.nodes_Z * _Blade_Config_Area.nodes_Y)
         );
}


// Request Partition Exit
void BGLPartitionExit( int code )
{
   // just exit.  my at_exit will do the rest.
   exit( code );
}

#if 0
//
// Install a Global Alert Interrupt Handler.
//
//  Handlers are functions that take no args and return an int, eg:
//
//   int function( void );
//
//
// When BGLParitionAlert is called, then on all nodes,
//   If no handler is installed, the Job will terminate.
//     If a handler is installed, then
//       if the handler returns zero, then
//         the Job will continue,
//       else if the handler returns non-zero, then
//         the Job will terminate.
//
int BGLPartitionInstallAlertHandler( _BL_GI_Handler alert_fcn )
{
   Bit32 orig_msr = BeginCriticalSection();

   dcache_invalidate_line( &_gi_info );
   _gi_info.alert_fcn = alert_fcn;
   dcache_store_line( &_gi_info );
   msync();

   EndCriticalSection( orig_msr );

   return(0);
}

//
// Install a Global Abort Interrupt Handler.
//
//  Handlers are functions that take no args and return an int, eg:
//
//   int function( void );
//
// When BGLParitionAbort is called, then on all nodes,
//   If no handler is installed, the Job will terminate.
//     If a handler is installed, then
//       if the handler returns zero, then
//         the Job will continue,
//       else if the handler returns non-zero, then
//         the Job will terminate.
//
int BGLPartitionInstallAbortHandler( _BL_GI_Handler abort_fcn )
{
   Bit32 orig_msr = BeginCriticalSection();

   dcache_invalidate_line( &_gi_info );
   _gi_info.abort_fcn = abort_fcn;
   dcache_store_line( &_gi_info );
   msync();

   EndCriticalSection( orig_msr );

   return(0);
}

//
// Deliver an Alert Interrupt to All Nodes in your Partition.
//
//  If no Alert handler is installed, the partition will be killed.
//  If an Alert handler is installed, it will be called on all nodes
//    in your partition.
//
//  At each node:
//    If the Alert handler returns zero, the node will continue.
//    If the Alert handler returns non-zero, the node will be killed.
//
//  If msg is non-NULL, printf will be called.
//
void BGLPartitionAlert( int code, char *msg, ... )
{
   va_list args;
   char buf[1024];

   if ( msg )
      {
      va_start( args, msg );
      vsprintf( &(buf[0]), msg, args );
      va_end( args );
      s0printf( buf );
      }

   if ( (BGLPartitionGetNumNodes() > 1) && (_Blade_Config_Area.bs_flags.flags & BS_GI_ENABLE) )
      {
      BGLGiSendNotification( _BGL_GI_NOTIFICATION_ALERT );

      __delay( BGLPartitionGetMHz() * 1000 );
      }
   else
      {
      _blade_gints_Alert();
      }
}

//
// Deliver an Abort Interrupt to All Nodes in your Partition.
//
//  If no Abort handler is installed, the partition will be killed.
//  If an Abort handler is installed, it will be called on all nodes
//    in your partition.
//
//  At each node:
//    If the Abort handler returns zero, the node will continue.
//    If the Abort handler returns non-zero, the node will be killed.
//
//  If msg is non-NULL, printf will be called.
//



void BGLPartitionAbort( int code, char *msg, ... )
{
   va_list args;
   char buf[1024];

   if ( msg )
      {
      va_start( args, msg );
      vsprintf( &(buf[0]), msg, args );
      va_end( args );
      s0printf( buf );
      }

   if ( (BGLPartitionGetNumNodes() > 1) && (_Blade_Config_Area.bs_flags.flags & BS_GI_ENABLE) )
      {
      BGLGiSendNotification( _BGL_GI_NOTIFICATION_ABORT );

      __delay( BGLPartitionGetMHz() * 1000 );
      }
   else
      {
      _blade_gints_Abort();
      }
}
#endif

// This wrapper allows using the tree for barriers

#define xBGL_TREE_PAYLOAD_SIZE ( sizeof( _BGL_TreeCollectivePktPyld ) )

// Tree reduce up to a payload full on the tree
// 0 <= aSize <= sizeof( _BGL_TreeCollectivePktPyld )
void xReduceOnTree( _BGL_TreeHwHdr* aHdr, unsigned long * aReduceIn, unsigned long * aReduceOut, int aSize )
{
    _BGL_TreeCollectivePktPyld Payload;
    _BGL_TreeHwHdr TreeHdrDummy;
    _BGL_TreePayload PayloadDummy;

    assert( aSize >= 0 && aSize <= xBGL_TREE_PAYLOAD_SIZE );

    memset( &PayloadDummy, 0, aSize );
    memset( &Payload, 0, aSize );

    memcpy( &Payload, aReduceIn, aSize );

    BGLTreeRawSendPacket(1, aHdr, &Payload );
    BGLTreeReceive( 1, &TreeHdrDummy, &PayloadDummy, xBGL_TREE_PAYLOAD_SIZE );
    memcpy( aReduceOut, &PayloadDummy, aSize );
}

int xGlobalTreeReduce( unsigned long * aReduceIn, unsigned long * aReduceOut, int aSize, unsigned aOperation )
  {
  if( !hard_processor_id() )
    {
    _BGL_TreeHwHdr TreeHdr;
    /// NOTE NOTE NOTE!!! Route 1, ONE!!! Is the only reduction route
    /// NOTE NOTE NOTE!!! Route 1, ONE!!! Is the only reduction route

    /// Saw 3us for a reduction on 1 packet on 4/6/05
    /// Much jitter in one packet timing if I/O is done prior to tree send/recv

    BGLTreeMakeCollectiveHdr( &TreeHdr,
                              1,
                              0,
                              aOperation,
                              _BGL_TREE_OPSIZE_BIT32,
                              999,
                              _BGL_TREE_CSUM_NONE );

    int NumberOfFullPackets = aSize / xBGL_TREE_PAYLOAD_SIZE;
    int BytesInLastPacket   = aSize % xBGL_TREE_PAYLOAD_SIZE;

    for( int i=0; i<NumberOfFullPackets; i++ )
      {
      int index = ( i * xBGL_TREE_PAYLOAD_SIZE ) >> 2;
      assert( index>=0 && index< aSize );
      xReduceOnTree( &TreeHdr, &aReduceIn[ index ], &aReduceOut[ index ], xBGL_TREE_PAYLOAD_SIZE );
      }

    if( BytesInLastPacket )
      {
      int index = ( NumberOfFullPackets * xBGL_TREE_PAYLOAD_SIZE ) >> 2;
      assert( index >= 0 && index < aSize );
      xReduceOnTree( &TreeHdr, &aReduceIn[ index ], &aReduceOut[ index ], BytesInLastPacket );
      }
    }
  return(0);
  }

void
BGLGI_BarrierWithWaitFunction( void * fx, void * arg )  // func, arg*
  {
  //from: include/BGLGi.h:typedef int (*BGLGI_WaitProc)(void *arg);
  if( (_Blade_Config_Area.max_Rank > 0) && _Blade_Config_Area.have_GlobalInts )
    {
    int core = _Blade_Config_Area.hard_processor_id;
    BGLGI_barrier( (void *)BGL_MEM_GI_USR_BASE, core, (BGLGI_WaitProc) fx, arg );
    }
  else
    {
    _BLADE_ABORT( -1 );
    }
  }

void
xBGLGI_barrier( void * bgl_mem_gi_usr_base,
                int gi,
                void * a,
                void * b )
  {
  #if 1
    BGLGI_barrier( (void *)BGL_MEM_GI_USR_BASE, gi, NULL, NULL );
  #else
    if( gi == 0 )
      {
      long unsigned int in, out;
      xGlobalTreeReduce( &in, &out, sizeof(int) , 1);
      }
  #endif
  }


// Global Barrier over all Compute Nodes in your Partition. BLADE_ON_BLRTS Version!
void BGLPartitionBarrierCompute( void )
{
   if ( (_Blade_Config_Area.max_Rank > 0) && _Blade_Config_Area.have_GlobalInts )
      {
      // both cores do a lockbox barrier
      if ( _Blade_Config_Area.is_VirtualNodeMode )
         _blLockBoxBarrier( _BLADE_VNM_BARRIER );

      if ( !_Blade_Config_Area.hard_processor_id )
         {
         // Core 0 does a global barrier
         xBGLGI_barrier( (void *)BGL_MEM_GI_USR_BASE, 0, NULL, NULL );
         }

      if ( _Blade_Config_Area.is_VirtualNodeMode )
         _blLockBoxBarrier( _BLADE_VNM_BARRIER );
      }
}

// Global Barrier over all Compute Nodes in your Partition. BLADE_ON_BLRTS Version!

// added checks to make sure that these calls come from correct core -- fx name must match

void BGLPartitionBarrierComputeCore0( void )
{
   if ( (_Blade_Config_Area.max_Rank > 0) && _Blade_Config_Area.have_GlobalInts )
      {
      // Check that we're on the core in the name
      if ( _Blade_Config_Area.hard_processor_id )
        {
        _BLADE_ABORT( -1 );
        }
      // Core 0 does a global barrier
      xBGLGI_barrier( (void *)BGL_MEM_GI_USR_BASE, 0, NULL, NULL );
      }
   else
      {
      _BLADE_ABORT( -1 );
      }

}


void BGLPartitionBarrierComputeCore1( void )
{
   if ( (_Blade_Config_Area.max_Rank > 0) && _Blade_Config_Area.have_GlobalInts )
      {
      // Check that we're on the core in the name
      if ( ! _Blade_Config_Area.hard_processor_id )
        {
        _BLADE_ABORT( -1 );
        }
      // Core 0 does a global barrier
      xBGLGI_barrier( (void *)BGL_MEM_GI_USR_BASE,
                     2,        // Global int index
                     NULL,
                     NULL );
      }
   else
      {
      _BLADE_ABORT( -1 );
      }

}



void BGLPartitionBarrierCore( void )
{
      if      ( _Blade_Config_Area.hard_processor_id == 0 )
        {
        BGLPartitionBarrierComputeCore0();
        }
      else if ( _Blade_Config_Area.hard_processor_id == 1 )
        {
        BGLPartitionBarrierComputeCore1();
        }
      else
        {
        _BLADE_ABORT( -1 );
        }
}


#if 0
// Global Barrier over all nodes in your partition.
//  - Must be called by exactly 1 Thread on every node.
//  - If you use multiple Threads, it is up to you to perform a local barrier first, if necessary.
//  - Assumes that _Blade_Config_Area.max_Rank refers to compute nodes only!
//  - On hardware, returns on every node at the same time.
//  - Blade will provide debugging versions of this function.
#undef  FN_NAME
#define FN_NAME "BGLPartitionBarrierAll"
void BGLPartitionBarrierAll( void )
{
#ifdef _SPI_PART_CHECK
   if( (_Blade_Config_Area.max_Rank + 1) != BGLPartitionGetNumNodesCompute() )
      {
      s0printf( "(E) "FN_NAME": SPI assumes max_Rank refers to compute nodes only.\n" );
      _BLADE_ABORT( -1 );
      }
#endif
   if ( ( (_Blade_Config_Area.max_Rank + 1) + _Blade_Config_Area.nodes_IO ) > 1 )
      {
      BGLGiBarrier( _BGL_GI_BARRIER_ALL_NODES );
      }
}
#endif

__END_DECLS
