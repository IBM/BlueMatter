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
 #include <assert.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

void error_exit(char *str) { fprintf(stderr,"\n%s\n",str); fflush(stderr); exit(-1); }

main(int argc, char ** argv, char ** envp)
  {
  struct stat    statbuf;
  struct dirent  *dirp;
  int             ret;
  char           *ptr;

  if( argc != 2 )
    error_exit( " please give one arg - a dir name " );

  char * fullpath = argv[1];

  int lstat_rc = lstat( fullpath, &statbuf );

  if( lstat_rc < 0 )
    error_exit( " could not stat fullpath ");

  if( 0 == S_ISDIR(statbuf.st_mode) )
    error_exit( " fullpath is not a directory " );

  DIR * dp = opendir( fullpath );

  if( dp == NULL )
    error_exit( " failed to read directory ");

  while( (dirp = readdir( dp ) ) != NULL )
    {
    char filepath[1024];
    sprintf( filepath, "%s/%s", fullpath, dirp->d_name );

    struct stat sb;
    int stat_rc = stat( filepath, & sb );
    if( stat_rc < 0 )
      error_exit( " stat of file failed " );

    printf("%s %d \n", filepath, sb.st_mtime );
    }

  }
