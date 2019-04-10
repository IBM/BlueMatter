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
 /*
 * timelim <seconds> <command>
 *
 * Tries to run the given command for the given time, then shoots it
 * down with 'SIGHUP'
 *
 *
 */


#define _ALL_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

static void do_end_child(int unused) {
  kill(-getpid(),SIGHUP) ;
  return ;
  }

static int do_parent(int timeout, pid_t child_pid) {
  struct timestruc_t Rqtp ;
  struct timestruc_t Rmtp ;
  struct sigaction action = { do_end_child, 0, 0 } ;
  struct sigaction ignore = { SIG_IGN,0,0 } ;
  (void) sigaction ( SIGCHLD, &action, NULL) ;
  (void) sigaction ( SIGHUP, &ignore, NULL) ;
  Rqtp.tv_sec = timeout ;
  Rqtp.tv_nsec = 0 ;
  { int nsleep_rc = nsleep(&Rqtp, &Rmtp) ;
    if (nsleep_rc == 0 ) {
      fprintf(stderr,"Test terminated after timeout, %d seconds :-(\n",timeout) ;
      do_end_child(0) ;
      return SIGHUP ;
    } else {
      fprintf(stderr,"Test finished on its own after %d seconds\n",Rqtp.tv_sec-Rmtp.tv_sec) ;
      {
        int Status ;
        (void) waitpid(child_pid, &Status, 0) ;
        fprintf(stderr, "Child status is %i\n",Status) ;
        return WEXITSTATUS(Status) ;
      }
    } /* endif */
  }
  }

static int do_child(char **argv, char **envp) {
  return execvp(argv[0], argv ) ;
  }

static int run_timelim( int allowed_time, char **argv, char **envp ) {
  int pgid_result = setpgid(0,0) ;
  pid_t other_pid = fork() ;
  if (other_pid == 0) {
    return do_child(argv,envp) ;
  } else if ( other_pid > 0 ) {
    return do_parent(allowed_time, other_pid) ;
  } /* endif */
  else return 0 ;
}

static int display_usage(const char * my_name) {
   fprintf(stderr,"%s <seconds> <command> \n"
          "  Try to run the given command for the given time, then stop it"
          " with a SIGHUP\n",
          my_name
         ) ;
  return 0 ;
}

int main(int argc, char **argv, char **envp) {
  if (argc > 2)
  {
     int allowed_time = abs(atoi(*(argv+1))) ;
     if ( allowed_time > 0 )
     {
        return run_timelim(allowed_time, argv+2, envp ) ;
     }
     else
     {
        return display_usage(*argv) ;
     } /* endif */
  }
  else
  {
     return display_usage(*argv) ;
  } /* endif */
}
