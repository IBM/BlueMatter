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
 * Module:          pkdebug.hpp
 *
 * Purpose:         Handle the attaching a debugger to the current process.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         0102297 BGF Created.
 *
 ***************************************************************************/
#ifndef __PKDEBUG_HPP__
#define __PKDEBUG_HPP__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef MPI
#include "mpi.hpp"
#endif



static int start_debug()
  {
  // To use this, simply edit the display name to put debug windows where
  // you want.  This could be modified to be a routine automatically called
  // when an assert fails.

#ifdef PKDEBUG_PATH

  char hostname[80];
  char db_cmd[1024];
  gethostname( hostname, 80 );

#ifdef MPI
  int TaskNo  = MpiSubSys::TaskNo;
  int TaskCnt = MpiSubSys::TaskCnt;
#else
  int TaskNo  = 0;
  int TaskCnt = 1;
#endif

  sprintf( db_cmd,
           "%s %d %d %d %s &",
           "pkdebug",
           getpid(),
           TaskNo,
           TaskCnt,
           hostname );


#if 0
//put are in to run quiet till signal: -q
//               -q
#if 1
  sprintf(xldb_cmd,
               "xldb \
               -wb   \
               -i 14 \
               -i 15 \
               -i 23 \
               -I/u/blake/foxfm/foxframe/ \
               -I/u/blake/foxfm/foxpk/ \
               -I/u/blake/foxfm/SampleFeatureDb/ \
               -I/u/blake/foxfm/foxmoddb/ \
               -display localhost:0.0 \
               -title %s:%d/%d:%d  \
               -a %d \
               /u/blake/work/foxfm/pktest/a.out \
               &",       // prevent this proc from blocking on the system call.
               hostname, TaskNo, TaskCnt, getpid(),
               getpid() );
#else
  sprintf(xldb_cmd,
               "aixterm "
               "-title %s:%d/%d:%d "
               "-e dbx "
               "-c /u/blake/foxfm/foxpk/dbx.startup "
               "-a %d "
               "&",       // prevent this proc from blocking on the system call.
               hostname, TaskNo, TaskCnt, getpid(),
               getpid() );
#endif
#endif

  fprintf(stderr, "%s", db_cmd);
  system( db_cmd );
  sleep(1);  //Make sure poe doesn't send us oun our way too soon.
  sleep(1);
  sleep(1);
  sleep(1);
  sleep(1);
  /* kill( getpid(), 18); */
#endif

  return(0);
  }
#endif
