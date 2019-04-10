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
//      File: blade/bgl_spi/BGL_PartitionSPI.h
//
//   Purpose: Define Blade's Partition System Programming Interface.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: See bgl_partition_spi.c
//
//   History: 05/15/2002: MEG - Created.
//
//
#ifndef _BGL_PARTITION_SPI_H  // Prevent multiple inclusion
#define _BGL_PARTITION_SPI_H

#include <spi/blade_on_blrts.h>

// Define to make SPI check for obvious errors.  WARNING: THIS REDUCES PERFORMANCE!
#define _SPI_PART_CHECK

#if !defined(__ASSEMBLY__)

__BEGIN_DECLS

#include <spi/bl_gints.h>

/*---------------------------------------------------------------------------*
 *                                                                           *
 *    Interfaces to Query Partition Size, Shape, Frequency, etc.             *
 *                                                                           *
 *---------------------------------------------------------------------------*/

// Obtain the Frequency of the Partition (note: could be a lie).
//  Return value should be in range 500 to 1000.
int BGLPartitionGetMHz( void );


// Obtain the amount of memory in the Partition.
//  Note: The partition will be configured for the least amount
//    of memory in any node in the partition.
//  Return values: 0: 256MB
//                 1: 512MB
//                 2: 1GB
//                 3: 2GB
int BGLPartitionGetDDRSize( void );


// Obtain this Node's coordinates in the Parition.
//  Coordinates range from (0 to x-1, 0 to y-1, 0 to z-1).
void BGLPartitionGetCoords( int *x, int *y, int *z );

// Obtain the Partition's Maximum Extents (0 to x-1, 0 to y-1, 0 to z-1)
void BGLPartitionGetExtents( int *max_x, int *max_y, int *max_z );

// Obtain the Dimensions of the Partition (number of nodes, not indices)
void BGLPartitionGetDimensions( int *x_nodes, int *y_nodes, int *z_nodes );

// Obtain the number of compute nodes in the partition.
int BGLPartitionGetSize( void );

// Obtain the total number of nodes in the partition (Compute + I/O).
int BGLPartitionGetNumNodes( void );

// Obtain the total number of compute nodes in the partition (same as BGLPartitionGetSize).
int BGLPartitionGetNumNodesCompute( void );

// Obtain the total number of I/O nodes in the partition.
int BGLPartitionGetNumNodesIO( void );

// Returns 1 if the caller is running on an I/O node; 0 otherwise.
int BGLPartitionIsIONode( void );

// Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.
//  This can differ from Real Rank if Single-Node LoopBack test.
int BGLPartitionGetRank( void );

// Obtain this Node's Real Rank (range is 0 to Nodes-1) when X varies fastest.
//  If Single-Node LoopBack test, this is our real rank in the partition, and
//  is the only difference between all nodes in the partition.  Always equal to
//  Rank if not Single-Node LoopBack.
int BGLPartitionGetRealRank( void );

// Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.
int BGLPartitionGetRankXYZ( void );

// Obtain this Node's Rank (range is 0 to Nodes-1) when Z varies fastest.
int BGLPartitionGetRankZYX( void );

// Create from Rank of the node the coordinates when X varies fastest.
//  Note: order of args is X, Y, Z.
void BGLPartitionMakeXYZ( int rank, int *x_coord, int *y_coord, int *z_coord );

// Create from Rank of the node the coordinates when Z varies fastest.
//  Note: order of args is X, Y, Z.
void BGLPartitionMakeZYX( int rank, int *x_coord, int *y_coord, int *z_coord );

// Create the Rank of the node at coordinates when X varies fastest.
//  Note: order of args is X, Y, Z.
int BGLPartitionMakeRank( int x_coord, int y_coord, int z_coord );

// Create the Rank of the node at coordinates when X varies fastest.
//  Note: order of args is X, Y, Z.
int BGLPartitionMakeRankXYZ( int x_coord, int y_coord, int z_coord );

// Create the Rank of the node at coordinates when Z varies fastest.
//  Note: order of args is Z, Y, X.
int BGLPartitionMakeRankZYX( int z_coord, int y_coord, int x_coord );


/*---------------------------------------------------------------------------*
 *                                                                           *
 *    Interfaces for Job or Partition Management                             *
 *                                                                           *
 *---------------------------------------------------------------------------*/

// Request Partition Exit (same as normal libc exit)
void BGLPartitionExit( int code );

//
// Install a Global Alert Interrupt Handler.
//
//  Handlers are functions that take no args and return an int, eg:
//
//   int function( void );
//
//
// When BGLParitionAlert is called,
//   If no handler is installed, the Job will terminate.
//     If a handler is installed, then
//       if the handler returns zero, then
//         the Job will continue,
//       else if the handler returns non-zero, then
//         the Job will terminate.
//
int BGLPartitionInstallAlertHandler( _BL_GI_Handler alert_fcn );


//
// Install a Global Abort Interrupt Handler.
//
//  Handlers are functions that take no args and return an int, eg:
//
//   int function( void );
//
// When BGLParitionAbort is called,
//   If no handler is installed, the Job will terminate.
//     If a handler is installed, then
//       if the handler returns zero, then
//         the Job will continue,
//       else if the handler returns non-zero, then
//         the Job will terminate.
//
int BGLPartitionInstallAbortHandler( _BL_GI_Handler abort_fcn );

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
void BGLPartitionAbort( int code, char *msg, ... );

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
void BGLPartitionAlert( int code, char *msg, ... );


// Global Barrier over all Compute Nodes in your Partition.
//  - Must be called by exactly 1 Thread on every node.
//  - If you use multiple Threads, it is up to you to perform a local barrier first, if necessary.
//  - Assumes that _Blade_Config_Area.max_Rank refers to compute nodes only!
//  - Returns on every node at the same time (+/- ~1us).
//  - Blade will provide debugging versions of this function.
void BGLPartitionBarrierCompute( void );
void BGLPartitionBarrierComputeCore0( void );
void BGLPartitionBarrierComputeCore1( void );
void BGLPartitionBarrierCore( void );
#define BGLPartitionBarrier  BGLPartitionBarrierCompute     // Deprecated

// provide access to the polling function supported during wait for barrier
void
BGLGI_BarrierWithWaitFunction( void * fx, void * arg );  // func, arg*

// Global Barrier over all nodes in your partition.
//  - Must be called by exactly 1 Thread on every node.
//  - If you use multiple Threads, it is up to you to perform a local barrier first, if necessary.
//  - Assumes that _Blade_Config_Area.max_Rank refers to compute nodes only!
//  - On hardware, returns on every node at the same time.
//  - Blade will provide debugging versions of this function.
void BGLPartitionBarrierAll( void );

__END_DECLS

#endif // __ASSEMBLY__

#endif // Add nothing below this line.
