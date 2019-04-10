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
 * Module:          pkmain.C
 *
 * Purpose:         This holds C process env main() so that calls to start
 *                  environment can be careated.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         170297 BGF Created.
 *
 ***************************************************************************/

#include "unistd.h"  // for nice()

#include <pk/debug.hpp>
#include <pk/fxlogger.hpp>
#include <pk/pktrace.hpp>

#include <pk/exception.hpp> // for exception class
#include <pk/reactor.hpp>

int PkMain( int, char**, char**);

#include <iostream.h>

// GLOBALS
int _pk_PPL_REGLOG_fd      = 0;
int _pk_PPL_REGLOG_fd_LOCK = 0;

// By default, we'll checksum only the first 64KB of the executable.
#ifndef PK_MAX_EXCUTABLE_CHECKSUM
#define PK_MAX_EXCUTABLE_CHECKSUM (64*1024)
#endif


#ifdef PK_MPI_ALL
long long _pk_ExecutableCheckSum = 0;
int AssertExecutableCheckSum( void *TriggerMsg )
  {
  assert( *((long long *)TriggerMsg) == _pk_ExecutableCheckSum );
  fprintf(stderr,"CheckSum confirmed task %d\n", MpiSubSys::GetTaskNo() );
  fflush(stderr);
  return(0);
  }
#endif

main(int argc, char **argv, char **envp)
  {
  nice(1); // parallel process should be nice to each other, right?
#if !defined(EXCEPTIONS_UNAVAILABLE)
  try
#endif
    {
#ifdef PK_MPI_ALL
      // DO a check sum of the executable file to make sure that
      // all address spaces have been loaded with the same executable.
      FILE *fp = fopen( argv[0], "r" );
      assert( fp != NULL );
      // Add up the first bit of the executable file.
      for(int b = 0; (b < PK_MAX_EXCUTABLE_CHECKSUM) && (! feof( fp )); b++ )
        _pk_ExecutableCheckSum += fgetc(fp);
      fclose( fp );

      MpiSubSys::Init( argc, argv );
      //MpiDebug::Init()  ... this call needs to be created.
      int start_debug();
      start_debug();
      PkActiveMsgSubSys::Init();

      if( MpiSubSys::TaskNo > 0 )
        {
        // This is the end of the line for all mpi nodes above 0.
        // Note however that the reactor subsystem has been started.

//*** IT SEEMS MPI ON THE SWITCH DOESN'T WANT THREAD ZERO BEING SHUT DOWN ***
for(;;)
  sleep(1);

        pthread_exit(NULL);
        assert(0); // better not get here
        }

      // Use active message sub system to assert that every
      // address space has been loaded with the same executable.
      for( int i=1; i < MpiSubSys::GetTaskCnt(); i++ )
         {
         PkActiveMsgSubSys::Trigger( i,
                                     AssertExecutableCheckSum,
                                     (void *) &_pk_ExecutableCheckSum,
                                     sizeof( _pk_ExecutableCheckSum  ) );
         }
#endif

      PkMain( argc, argv, envp );
    }
#if !defined(EXCEPTIONS_UNAVAILABLE)
  catch(PkException& gfe)
    {
      // perhaps these messages should go to a log file?
      cerr << "Exception caught in pkmain.cpp ";
#ifdef MPI
      cerr << " MPI TaskNo = " << MpiSubSys::TaskNo;
#endif
      cerr << endl;
      cerr << gfe << endl;
      exit(-1); // just exit
    }
#endif

#ifdef MPI
  MpiSubSys::Terminate();
#endif

  return(0);
  }

