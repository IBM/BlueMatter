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
 * Module:          mpi.cpp
 *
 * Purpose:         Wraps mpi lib in extern scope.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         030796 BGF Created.
 *
 ***************************************************************************/

#include <pk/fxlogger.hpp>

#include <pk/mpi.hpp>

#ifdef MPI  // knock out this whole file if we are not building with mpi lib.

int MpiSubSys::MpiUp;
int MpiSubSys::TaskNo;
int MpiSubSys::TaskCnt;

#ifndef PKFXLOG_MPISUBSYS
#define PKFXLOG_MPISUBSYS  0
#endif

int
MpiSubSys::Init( int argc, char **argv )
  {
  if( MpiUp == 0 )
    {
    MPI_Init(&argc,&argv);  /* initialize MPI environment */
    MPI_Comm_rank(MPI_COMM_WORLD, &MpiSubSys::TaskNo );
    MPI_Comm_size(MPI_COMM_WORLD, &MpiSubSys::TaskCnt );
    MpiSubSys::MpiUp = 1;
    }
  return(0);
  }

#if 0 //NEED: to trash the stuff with trace points!!
MpiSubSys::Init( int argc, char **argv,
                 PkTraceStreamDcl *BeforeTrace,
                 PkTraceStreamDcl *AfterTrace,
                 char             *TraceID)
  {
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Init With Tracing: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  if( MpiUp == 0 )
    {
    MPI_Init(&argc,&argv);  /* initialize MPI environment */
    MPI_Comm_rank(MPI_COMM_WORLD, &MpiSubSys::TaskNo );
    MPI_Comm_size(MPI_COMM_WORLD, &MpiSubSys::TaskCnt );
    MpiSubSys::MpiUp = 1;
    }
  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Init With Tracing: End"
        << EndLogLine;

  return(0);
  }
#endif

int MpiSubSys::Comm_rank(MPI_Comm comm, int *rank)
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_rank: Begin "
        << EndLogLine;

  error = MPI_Comm_rank( comm, rank);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_rank: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Comm_rank(MPI_Comm comm, int *rank,
                         PkTraceStreamDcl        *BeforeTrace,
                         PkTraceStreamDcl        *AfterTrace,
                         char                    *TraceID)
  {
  int arg = 0;
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_rank: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Comm_rank( comm, rank);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_rank: End "
        << EndLogLine;

  return error;

  }


// This routine is implemented in the most hideous way.
int
MpiSubSys::GetTaskNo( char *hostname, unsigned processor )
  {
  static int entry1 = 0;
  static char hostlist[256][32];
  static int hostcount;

  // When there are no args, then the caller wants his own hostname.
  if( hostname == NULL )
    return( TaskNo );

  if( ! entry1 )       //NEED: unsafe for SMP!!!!!
    {
    entry1 = 1;
    FILE *fp = fopen( "host.list", "r" );
    assert( fp != NULL );  // COULDN'T OPEN host.list file.
    int h;
    for( h = 0; ! feof( fp ) && h < MpiSubSys::TaskCnt ; h++ )
      {
      fscanf( fp, "%s\n", hostlist[h] );

      BegLogLine( PKFXLOG_MPISUBSYS )
        << "GetTaskNo: scanning for hosts: host "
        << h
        << " is "
        << hostlist[h]
        << EndLogLine;

      }
    hostcount = h;
    fclose(fp);
    }
  // scan host list for entry.
  // The spooge is to allow us modulo the processor selection number
  // by the number of hostnames that match.
  int found = -1;
  int count =  0;
  while( 1 )
    {
    for( int h = 0; h < hostcount; h++ )
      {
      if( strcmp( hostname, hostlist[h] ) == 0 )
        {
        count++;
        found = h;
        if( processor == 0 )
          break;
        else
          processor--;
        }
      }
    if( count == 0 )
      break;  // no use searching if it isn't here at all.
    if( found == 1 )
      break;  // no use running around if there is only one possibility.
    if( processor == 0 )
      break;  // must have the correct instance.
    count = 0;
    }

  BegLogLine( PKFXLOG_MPISUBSYS )
    << " GetTaskNo "
    << " host >"
    << hostname
    << "< is task no "
    << found
    << EndLogLine;

  return( found );
  }

int MpiSubSys::Recv(void *buf, int count, MPI_Datatype datatype, int source,
                 int   tag, MPI_Comm comm, MPI_Status *status)
  {
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Recv: Begin "
        << EndLogLine;

  error = MPI_Recv(buf, count, datatype, source, tag, comm, status);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Recv: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Recv(void *buf, int count, MPI_Datatype datatype, int source,
                    int   tag, MPI_Comm comm, MPI_Status *status,
                    PkTraceStreamDcl        *BeforeTrace,
                    PkTraceStreamDcl        *AfterTrace,
                    char                    *TraceID)
  {
  int arg = 0;
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Recv: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Recv(buf, count, datatype, source, tag, comm, status);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Recv: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Irecv(void* buf, int count, MPI_Datatype datatype, int source,
                     int tag, MPI_Comm comm, MPI_Request *request)
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Irecv: Begin "
        << EndLogLine;

  error = MPI_Irecv( buf, count, datatype, source, tag, comm, request);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Irecv: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Irecv(void* buf, int count, MPI_Datatype datatype, int source,
                     int tag, MPI_Comm comm, MPI_Request *request,
                         PkTraceStreamDcl        *BeforeTrace,
                         PkTraceStreamDcl        *AfterTrace,
                         char                    *TraceID)
  {
  int arg = 0;

  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Irecv: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Irecv( buf, count, datatype, source, tag, comm, request);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Irecv: End "
        << EndLogLine;

  return error;

  }


int MpiSubSys::Abort(MPI_Comm comm, int errorcode)
  {
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Abort: Begin "
        << EndLogLine;

  error = MPI_Abort(comm, errorcode);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Abort: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Abort(MPI_Comm comm, int errorcode,
                         PkTraceStreamDcl        *BeforeTrace,
                         PkTraceStreamDcl        *AfterTrace,
                         char                    *TraceID)
  {
  int arg = 0;
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Abort: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Abort(comm, errorcode);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Abort: End "
        << EndLogLine;

  return error;

  }


int MpiSubSys::Get_count(MPI_Status *status, MPI_Datatype datatype, int *count)
  {
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Get_count: Begin "
        << EndLogLine;

  error = MPI_Get_count(status, datatype, count);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Get_count: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Get_count(MPI_Status *status, MPI_Datatype datatype, int *count,
                         PkTraceStreamDcl        *BeforeTrace,
                         PkTraceStreamDcl        *AfterTrace,
                         char                    *TraceID)
  {
  int arg = 0;
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Get_count: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Get_count(status, datatype, count);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Get_count: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Waitany(int count, MPI_Request *array_of_requests, int *index,
                MPI_Status *status)
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany: Begin "
        << EndLogLine;



#ifndef MPISUBSYS_TESTANY
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitany as Waitany: Begin "
        << EndLogLine;

  error =  MPI_Waitany(count, array_of_requests, index, status);
  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Waitany: End "
        << EndLogLine;

#else
  int Flag = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Testany: Begin "
        << EndLogLine;

  while (Flag == 0)
    {
    error = MPI_Testany(count, array_of_requests, index, &Flag, status);

    assert (error == 0);

    BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Testany: Flag state [" << Flag
        << "] Looping..."
        << EndLogLine;

    if( Flag == 0 )
      {
      sleep( 1 );
      }
    }

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Testany: End "
        << EndLogLine;

#endif

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitany: End "
        << EndLogLine;


  return error;
  }

int MpiSubSys::Testany(int count, MPI_Request *array_of_requests, int *index, int *Flag,
                MPI_Status *status)
    {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Testany: Begin "
        << EndLogLine;

  *Flag = 0;
  error = MPI_Testany(count, array_of_requests, index, Flag, status);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Testany as Testany: End "
        << EndLogLine;

  return error;
    }

int MpiSubSys::Waitany(int count, MPI_Request *array_of_requests, int *index,
                       MPI_Status *status,
                       PkTraceStreamDcl        *BeforeTrace,
                       PkTraceStreamDcl        *AfterTrace,
                       char                    *TraceID)
  {
  int error ;
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany: Begin "
        << EndLogLine;



#ifndef MPISUBSYS_TESTANY
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitany as Waitany: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error =  MPI_Waitany(count, array_of_requests, index, status);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Waitany: End "
        << EndLogLine;

#else
  int Flag = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Testany: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  while (Flag == 0)
    {
    error = MPI_Testany(count, array_of_requests, index, &Flag, status);

    assert (error == 0);

    BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Testany: Flag state [" << Flag
        << "] Looping..."
        << EndLogLine;

    if( Flag == 0 )
      {
      sleep( 1 );
      }
    }

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << "   MpiSubSys::Waitany as Testany: End "
        << EndLogLine;

#endif

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitany: End "
        << EndLogLine;


  return error;
  }

int MpiSubSys::Comm_size(MPI_Comm comm, int *size)
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_size: Begin "
        << EndLogLine;

  error =  MPI_Comm_size( comm, size);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_size: End"
        << EndLogLine;

  return error;
  }

int MpiSubSys::Comm_size(MPI_Comm comm, int *size,
               PkTraceStreamDcl        *BeforeTrace,
               PkTraceStreamDcl        *AfterTrace,
               char                    *TraceID)
  {
  int error ;
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_size: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error =  MPI_Comm_size( comm, size);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_size: End"
        << EndLogLine;

  return error;
  }

int MpiSubSys::Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_create: Begin "
        << EndLogLine;

  error = MPI_Comm_create( comm,  group, newcomm);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_create: End"
        << EndLogLine;

  return error;
  }

int MpiSubSys::Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm,
                           PkTraceStreamDcl        *BeforeTrace,
                           PkTraceStreamDcl        *AfterTrace,
                           char                    *TraceID)
  {
  int error ;
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_create: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Comm_create( comm,  group, newcomm);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_create: End"
        << EndLogLine;

  return error;
  }

int MpiSubSys::Comm_group(MPI_Comm comm, MPI_Group *group)
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_group: Begin "
        << EndLogLine;

  error = MPI_Comm_group( comm, group);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_group: End "
        << EndLogLine;

  return error;
  }

int MpiSubSys::Comm_group(MPI_Comm comm, MPI_Group *group,
                          PkTraceStreamDcl        *BeforeTrace,
                          PkTraceStreamDcl        *AfterTrace,
                          char                    *TraceID)
  {
  int error ;
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_group: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Comm_group( comm, group);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Comm_group: End "
        << EndLogLine;

  return error;
  }

int MpiSubSys::Send(void* buf, int count, MPI_Datatype datatype, int dest,
                    int tag, MPI_Comm comm)
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Send: Begin "
        << EndLogLine;

  error =  MPI_Send( buf, count, datatype, dest, tag, comm);

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Send: End"
        << EndLogLine;

  return error;
  }

int MpiSubSys::Send(void* buf, int count, MPI_Datatype datatype, int dest,
                    int tag, MPI_Comm comm,
                    PkTraceStreamDcl        *BeforeTrace,
                    PkTraceStreamDcl        *AfterTrace,
                    char                    *TraceID)
  {
  int error ;
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Send: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error =  MPI_Send( buf, count, datatype, dest, tag, comm);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Send: End"
        << EndLogLine;

  return error;
  }

int MpiSubSys::Barrier(MPI_Comm comm )
  {
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Barrier: Begin "
        << EndLogLine;

  error = MPI_Barrier( comm );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Barrier: End"
        << EndLogLine;

  return error;
  }

int MpiSubSys::Barrier(MPI_Comm comm,
                       PkTraceStreamDcl        *BeforeTrace,
                       PkTraceStreamDcl        *AfterTrace,
                       char                    *TraceID )
  {
  int error ;
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Barrier: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Barrier( comm );

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Barrier: End"
        << EndLogLine;

  return error;
  }

// Terminate is an abort with rc = 0;
int MpiSubSys::Terminate(int Rc )
    {
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Terminate: Begin "
        << EndLogLine;

  error = MPI_Abort( MPI_COMM_WORLD, Rc );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Terminate: End "
        << EndLogLine;

  return error;
    }

int MpiSubSys::Finalize(void)
  {
  int error ;
  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Finalize: Begin "
        << EndLogLine;

  error = MPI_Finalize( /*void*/ );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Finalize: End "
        << EndLogLine;

  return error;
  }

int MpiSubSys::Finalize(PkTraceStreamDcl        *BeforeTrace,
                        PkTraceStreamDcl        *AfterTrace,
                        char                    *TraceID)
  {
  int error ;
  int arg = 0;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Finalize: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );

  error = MPI_Finalize( /*void*/ );

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Finalize: End "
        << EndLogLine;

  return error;
  }

int MpiSubSys::Waitsome(int incount, MPI_Request *array_of_requests,
                        int *outcount, int *array_of_indices,
                        MPI_Status *array_of_statuses)
  {

  static PkTraceStreamDcl *MpiSubSysWaitsome_Before;
  static PkTraceStreamDcl *MpiSubSysWaitsome_After;
  int arg = 0;
  int error ;

  if (MpiSubSysWaitsome_Before == NULL)
    {
    BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitsome: Making the Before Trace "
        << EndLogLine;

    MpiSubSysWaitsome_Before = new PkTraceStreamDcl;
    }

  if (MpiSubSysWaitsome_After == NULL)
    {
    BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitsome: Making the After Trace "
        << EndLogLine;

    MpiSubSysWaitsome_After = new PkTraceStreamDcl;
    }


  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitsome: Begin "
        << EndLogLine;

  MpiSubSysWaitsome_Before->PkTraceStreamSourceCON( 0,
                                                "MpiSubSysWaitsome",
                                                (unsigned) arg,
                                                "Before" );

  error = MPI_Waitsome(incount, array_of_requests, outcount,
                           array_of_indices, array_of_statuses);

  MpiSubSysWaitsome_After->PkTraceStreamSourceCON( 0,
                                                "MpiSubSysWaitsome",
                                                (unsigned) arg,
                                                "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitsome: End "
        << EndLogLine;

  return error;

  }

int MpiSubSys::Waitsome(int                      incount,
                        MPI_Request             *array_of_requests,
                        int                     *outcount,
                        int                     *array_of_indices,
                        MPI_Status              *array_of_statuses,
                        PkTraceStreamDcl        *BeforeTrace,
                        PkTraceStreamDcl        *AfterTrace,
                        char                    *TraceID)
  {

  int arg = 0;
  int error ;

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitsome: Begin "
        << EndLogLine;

  BeforeTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "Before" );


  error = MPI_Waitsome(incount, array_of_requests, outcount,
                           array_of_indices, array_of_statuses);

  AfterTrace->PkTraceStreamSourceCON( 0, TraceID, (unsigned) arg, "After" );

  BegLogLine( PKFXLOG_MPISUBSYS )
        << " MpiSubSys::Waitsome: End "
        << EndLogLine;

  return error;

  }

#endif
