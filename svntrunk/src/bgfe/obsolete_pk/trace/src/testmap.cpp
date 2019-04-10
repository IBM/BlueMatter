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
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <locale.h>
#include <fcntl.h>

#include <time.h>
extern "C" {
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>
}

#include <regex.h>
#include <fnmatch.h>
//#include <pthread.h>

#ifndef MAP_FILE_SIZE
#define MAP_FILE_SIZE (1024 * 1024 * 1024) // 256Mb mapped file space
#endif

#ifndef FILE_MODE
#define FILE_MODE ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )
#endif

main(int argc, char ** argv, char ** envp)
  {
  char * MappedFileName = "DeleteThisFile";
  int Fd = open( MappedFileName, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR );

  if( Fd < 0 )
    printf("Error errno %d is: %s\n", errno, strerror( errno ) );

  // Check the file settings
  assert( Fd >= 0 );

    if(lseek(Fd, MAP_FILE_SIZE - 1, SEEK_SET) == -1)
      {
      assert(0);
      }

  if(write(Fd,"",1) != 1)
    {
    assert(0);
    }

  // map the file
  int * FileHeader = (int *) mmap(0, MAP_FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, Fd, 0);

  // Check file was mapped OK
  assert( (caddr_t)FileHeader != (caddr_t) -1 );

  int iter = ( (argc == 2 ) ? atoi( argv[1] ) : 1 );
  for( int j = 0; j < iter; j++ )
    {
    for( int i = 0; i < (MAP_FILE_SIZE / sizeof( int ) ); i++ )
      {
      FileHeader[i] = *((int *)"TEST");
      }
    }
  return(0);
  }
