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
 #include <stdio.h>
#include <sys/sched.h>
#include <sys/errno.h>
int main( int argc, char ** argv, char ** envp )
{
   int newpri ;
   int ax ;
   pid_t pid ;
   int result ;
   int err ;
   int rcode = 0 ;
   if (argc > 2)
   {
      newpri = atoi(argv[1]) ;
      if (newpri > PRIORITY_MIN && newpri < PRIORITY_MAX)
      {
         for (ax = 2; ax < argc; ax += 1)
         {
            pid = atoi(argv[ax]) ;
            if (pid > 1)
            {
               result = setpri(pid, newpri) ;
               if (result == -1)
               {
                  err = errno ;
                  fprintf(stderr,"%s: Error %i setting priority of process %i to %i.\n",
                                  argv[0], err, pid, newpri) ;
                  rcode = 1 ;
               }
               else
               {
               /*
                * No message when things work right.
                */
               /*
                * fprintf(stderr, "%s: Priority of process %i was %i.\n",
                *                  argv[0], pid, result) ;
                */
               } /* endif */
            }
            else
            {
               fprintf(stderr,"%s: Invalid process (%i).\n",
                               argv[0], pid) ;
               rcode = 1 ;
            } /* endif */
         } /* endfor */
      }
      else
      {
         fprintf(stderr,"%s: Priority (%i) out of range.\n", argv[0], newpri) ;
         rcode = 1 ;
      } /* endif */
   }
   else
   {
      fprintf(stderr,"%s <new-pri> <pid1> <pid2> ...\n", argv[0]) ;
      rcode = 1 ;
   } /* endif */
   seteuid(getuid()) ; /* Set back to the natural user, so any exit processing (gmon.out) behaves */
   return rcode ;
}
