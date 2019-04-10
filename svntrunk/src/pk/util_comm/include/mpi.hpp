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
 * Project:         pK
 *
 * Module:          mpi.hpp
 *
 * Purpose:         Wraps mpi lib in extern scope.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         030796 BGF Created.
 *
 ***************************************************************************/

#ifndef _MPI_HPP_
#define _MPI_HPP_

/* $Id: mpi.hpp 9204 2006-06-21 12:26:46Z tjcw $ */

// The following seems to work to block change linkage convention
// for all routine prototypes in mpi.h.


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include "pktrace.hpp"


#ifndef MPI  // if no mpi, we define a class to resolve static config methods.
             // MUST NOT LINK IN THE ROUTINES FROM mpi.cpp FILE.

class MpiSubSys
  {
  public:
    // These aren't used but are here for compatability.

    int
    Init( int argc, char **argv )
      {
      assert(0);  // NON-MPI lib version should not be trying to init.
      return(0);
      }

    static int
    IsUp()
      {
      return(0);
      }

    static int
    GetTaskNo( char *aHostname = NULL, int aProcess = 0 )
      {
      // should assert that the hostname is the one that
      if( aHostname != NULL )
        {
        if( strcmp( aHostname, "localhost" ) != 0 )
          {
          char hostname[256];
          gethostname( hostname, 256 );
          if( strcmp( aHostname, hostname ) != 0 )
            {
            fprintf(stderr, "GetTaskNo called with hostname >%s<.\nMust be %s or %s\n",
              aHostname, "localhost", hostname );
            fflush( stderr );
            assert(0);
            }
          }
        }
      return( 0 );
      }

    static int
    GetTaskCnt()
      {
      return(1);
      }
  };

#else   // MPI is specified...

extern "C"
{
#include <mpi.h>
}

// The following takes care of accessing the field of an MPI_Status struct for IBM and MPICH
#ifdef MPICH
#define    MPI_STATUS_TAG(Status)    (Status.MPI_TAG)
#define    MPI_STATUS_SOURCE(Status) (Status.MPI_SOURCE)
#define    MPI_STATUS_ERROR(Status)  (Status.MPI_ERROR)
////int MPI_STATUS_TAG(MPI_Status Status ) { return( Status.MPI_TAG ); }
////int MPI_STATUS_SOURCE(MPI_Status Status ) { return( Status.MPI_SOURCE ); }
////int MPI_STATUS_ERROR(MPI_Status Status ) { return( Status.MPI_ERROR ); }
#else // IBM mpi
    int MPI_STATUS_TAG(MPI_Status Status ) { return( Status.tag ); }
    int MPI_STATUS_SOURCE(MPI_Status Status ) { return( Status.source ); }
    int MPI_STATUS_ERROR(MPI_Status Status ) { return( Status.error ); }
#endif


#if 0  // seems unused
inline MPI_GetRank()
  {
  static int rank = -1;
  if( rank == -1 )
    MPI_Comm_rank(MPI_COMM_WORLD,&rank );
  return( rank );
  }
#endif

class MpiSubSys
  {
  private:
    MpiSubSys(){}

  public:
    static int  MpiUp;
    static int  TaskNo;
    static int  TaskCnt;
  public:

    static int
    IsUp()
      {
      return(MpiUp);
      }

    static int
    GetTaskCnt()
      {
      return(TaskCnt);
      }

    static int
    Init( int argc, char **argv );

    static int
    Init( int argc, char **argv,
          PkTraceStreamDcl        *BeforeTrace ,
          PkTraceStreamDcl        *AfterTrace ,
          char                    *TraceID  );

    static
    int
    GetTaskNo( char *hostname = NULL, unsigned processor = 0 );

    static
    int Recv( void *buf, int count, MPI_Datatype datatype, int source,
              int   tag, MPI_Comm comm, MPI_Status *status);

    static
    int Recv( void *buf, int count, MPI_Datatype datatype, int source,
              int   tag, MPI_Comm comm, MPI_Status *status,
              PkTraceStreamDcl        *BeforeTrace,
              PkTraceStreamDcl        *AfterTrace,
              char                    *TraceID);

    static
    int Irecv(void* buf, int count, MPI_Datatype datatype, int source, int tag,
              MPI_Comm comm, MPI_Request *request);

    static
    int Irecv( void* buf, int count, MPI_Datatype datatype, int source, int tag,
               MPI_Comm comm, MPI_Request *request,
               PkTraceStreamDcl        *BeforeTrace,
               PkTraceStreamDcl        *AfterTrace,
               char                    *TraceID);

    static
    int Abort(MPI_Comm comm, int errorcode);

    static
    int Abort(MPI_Comm comm,
              int errorcode,
              PkTraceStreamDcl        *BeforeTrace,
              PkTraceStreamDcl        *AfterTrace,
              char                    *TraceID);

    static
    int Comm_rank(MPI_Comm comm, int *rank);

    static
    int Comm_rank(MPI_Comm comm, int *rank,
                  PkTraceStreamDcl        *BeforeTrace,
                  PkTraceStreamDcl        *AfterTrace,
                  char                    *TraceID);

    static
    int Get_count(MPI_Status *status, MPI_Datatype datatype, int *count);

    static
    int Get_count(MPI_Status *status,
                  MPI_Datatype datatype,
                  int *count,
                  PkTraceStreamDcl        *BeforeTrace,
                  PkTraceStreamDcl        *AfterTrace,
                  char                    *TraceID);

    static
    int Waitany(int count, MPI_Request *array_of_requests, int *index,
                MPI_Status *status);

    static
    int Testany(int count, MPI_Request *array_of_requests, int *index, int *Flag,
                MPI_Status *status);

    static
    int Waitany( int count,
                 MPI_Request *array_of_requests,
                 int *index,
                 MPI_Status *status,
                 PkTraceStreamDcl        *BeforeTrace,
                 PkTraceStreamDcl        *AfterTrace,
                 char                    *TraceID);


    static
    int Comm_size(MPI_Comm comm, int *size);

    static
    int Comm_size( MPI_Comm                 comm,
                   int                     *size,
                   PkTraceStreamDcl        *BeforeTrace,
                   PkTraceStreamDcl        *AfterTrace,
                   char                    *TraceID);

    static
    int Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);

    static
    int Comm_create( MPI_Comm comm,
                     MPI_Group group,
                     MPI_Comm *newcomm,
                     PkTraceStreamDcl        *BeforeTrace,
                     PkTraceStreamDcl        *AfterTrace,
                     char                    *TraceID);

    static
    int Comm_group(MPI_Comm comm, MPI_Group *group);

    static
    int Comm_group( MPI_Comm comm, MPI_Group *group,
                    PkTraceStreamDcl        *BeforeTrace,
                    PkTraceStreamDcl        *AfterTrace,
                    char                    *TraceID);

    static
    int Send(void* buf, int count, MPI_Datatype datatype, int dest, int tag,
             MPI_Comm comm);

    static
    int Send( void* buf, int count, MPI_Datatype datatype, int dest, int tag,
              MPI_Comm comm,
              PkTraceStreamDcl        *BeforeTrace,
              PkTraceStreamDcl        *AfterTrace,
              char                    *TraceID);

    static
    int Barrier(MPI_Comm comm );

    static
    int Barrier( MPI_Comm comm,
                 PkTraceStreamDcl        *BeforeTrace,
                 PkTraceStreamDcl        *AfterTrace,
                 char                    *TraceID );

    static
    int Finalize(void);

    static
    int Terminate(int rc = 0);

    static
    int Finalize( PkTraceStreamDcl        *BeforeTrace,
                  PkTraceStreamDcl        *AfterTrace,
                  char                    *TraceID);

    static
    int Waitsome(int incount, MPI_Request *array_of_requests, int *outcount,
                 int *array_of_indices, MPI_Status *array_of_statuses);

    static
    int Waitsome( int          incount,
                  MPI_Request *array_of_requests,
                  int         *outcount,
                  int         *array_of_indices,
                  MPI_Status  *array_of_statuses,
                  PkTraceStreamDcl        *BeforeTrace,
                  PkTraceStreamDcl        *AfterTrace,
                  char                    *TraceID);

  };

#endif
#endif // _MPI_HPP_
