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
 * Module:          BladeMpi.hpp
 *
 * Purpose:         Implements blade functions on mpi w/ active messages.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         Dec 10, 2002 BGF Created.
 *
 ***************************************************************************/


#ifndef _BLADE_HPP_
#define _BLADE_HPP_

#define __BEGIN_DECLS
#define __END_DECLS
#define ALIGN_QUADWORD __attribute__((aligned(16)))

typedef unsigned char Bit8;
typedef unsigned int  Bit32;
typedef unsigned int  Bit128[4];

// cut-n-pasted from first google hit, then modified to be c/c++ compat
typedef struct t_iovec
  {
  char *iov_base; /* start of block */
  int iov_len; /* length of block */
  } iovec ;

#include "BGL_PartitionSPI.h"
#include "BGL_TorusPktSPI.h"

int BladeInit();
int BladeInit(int *, char ***);

extern "C"
{
#include <mpi.h>
}

// The following takes care of accessing the field of an MPI_Status struct for IBM and MPICH
#ifdef MPICH
#define    MPI_STATUS_TAG(Status)    (Status.MPI_TAG)
#define    MPI_STATUS_SOURCE(Status) (Status.MPI_SOURCE)
#define    MPI_STATUS_ERROR(Status)  (Status.MPI_ERROR)
#else // IBM mpi
// For IBM mpi, these are implemented as functions in the cpp file.
#endif


///void BGLTorusSimpleActivePacketSend( void *ActorFx, int x, int y, int z, int len, void *data );

int AttachDebugger( char *file );

#endif
