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
 #ifndef __SETPRI_HPP__
#define __SETPRI_HPP__

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//THIS WHOLE THING DOESN"T WORK ON LINUX GCC
//extern "C" {
//#include <wait.h>  // put in for LINUX, probably won't hurt overall.
//}

// Try to avoid calling system() in this process becuase
// xldb has a bug.  When xldb is fixed, just call system().

#ifndef SETPRI_PATH
#define SETPRI_PATH "/usr/opt/bluegene/bin/utils/setpri"
#endif

static int
SchedTuneSelf()
  {
  int sysrc = 0;
#ifndef PK_LINUX
  char pristr[1024];
  sprintf( pristr, "%s 100 %d", SETPRI_PATH, getpid() );

  int pid = fork();

  if( pid < 0 )
    {
    fprintf(stderr,"FAILED TO FORK\n");
    fflush(stdout);
    return( pid );
    }

  if( pid == 0 )
    {
    // Could use exec... but the shell in system()
    // formats the args for us.
    sysrc = system( pristr );
    if( sysrc != 0 )
      {
      fprintf(stderr,"SchedTuneSelf: fork() child, system() failed.  rc %d\n", sysrc );
      fflush(stderr);
      _exit( -1 );
      }
    _exit( 0 );
    }
  else
    {
    if( (pid = waitpid( pid, &sysrc, 0 )) < 0 )
      {
      if( sysrc != 0 )
        {
        printf("possible setpri error rc %d  >%s<\n", sysrc, pristr );
        fflush(stdout);
        }
      }
    }
#endif
  return( sysrc );
  }

#endif
