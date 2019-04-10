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
 //
//      File: blade/bgl_spi/BGL_PartitionSPI.h
//
//   Purpose: Define Blade's Partition System Programming Interface.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//            (c) IBM, 2000
//
//     Notes: See bgl_partition_spi.c
//
//   History: 05/15/2002: MEG - Created.
//
//
#ifndef _BGL_PARTITION_SPI_H  // Prevent multiple inclusion
#define _BGL_PARTITION_SPI_H

__BEGIN_DECLS

/*---------------------------------------------------------------------------*
 *                                                                           *
 *    Interfaces to Query Partition Size and Shape                           *
 *                                                                           *
 *---------------------------------------------------------------------------*/

// Obtain this Node's coordinates in the Parition.
//  Coordinates range from (0 to x-1, 0 to y-1, 0 to z-1).
void BGLPartitionGetCoords( int *x, int *y, int *z );

// Obtain the Partition's Maximum Extents (0 to x-1, 0 to y-1, 0 to z-1)
void BGLPartitionGetExtents( int *max_x, int *max_y, int *max_z );

// Obtain the Dimensions of the Partition (number of nodes, not indices)
void BGLPartitionGetDimensions( int *x_nodes, int *y_nodes, int *z_nodes );

// Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.
int BGLPartitionGetRank( void );

// Obtain this Node's Rank (range is 0 to Nodes-1) when X varies fastest.
int BGLPartitionGetRankXYZ( void );

// Obtain this Node's Rank (range is 0 to Nodes-1) when Z varies fastest.
int BGLPartitionGetRankZYX( void );

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

// Abnormally terminate the Job running in your Partition.
//  Note: msg can be NULL.
void BGLPartitionAbort( int code, char *msg, va_list ap );


// Global Barrier over all Compute Nodes in your Partition.
//  - Must be called by exactly 1 Thread on every node.
//  - If you use multiple Threads, it is up to you to perform a
//      local barrier first, if necessary.
//  - Returns on every node at the same time (+/- ~1us).
//  - Blade will provide debugging versions of this function.
void BGLPartitionBarrier( void );

#if 0 //@MG: Not implemented yet in Blade's Network Interface Emulator.

// Deliver a Unix-style Signal to all Compute Nodes in the Partition.
//  - What that means to your Job is up to you.
//  - By default, this signal is blocked (masked off) unless you install
//     a signal handler (on each node) to catch it. You will be provided
//     interfaces to poll for such signals/conditions.
void BGLPartitionSignal( void );

#endif // 0 (Not implemented yet.)

__END_DECLS

#endif // Add nothing below this line.
