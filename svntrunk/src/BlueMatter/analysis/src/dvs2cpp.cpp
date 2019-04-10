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
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>


void usage()
{
  printf("dvs2cpp <dvs file>\n");
}

int main(int argc, char **argv, char **envp)
{
  if( argc != 2 )
    {
    usage();
    return -1;
    }

  char* filename = argv[1];
  
  int fd_dvs_read = open( filename, O_RDONLY );  

  
  // char buff[ 128 ];
  int TotalBytesRead = 0;
  
  struct stat fileStatus;
  
  fstat(fd_dvs_read, &fileStatus);
  int fileSize = fileStatus.st_size;

  char * dvsFile = new char[ fileSize ];
  int ind = 0;

  while( 1 )
    {      
      int BytesRead = read( fd_dvs_read, &dvsFile[ ind ] , 128 );      
      if( BytesRead == 0 )
        break;
      
      ind += BytesRead;
      TotalBytesRead += BytesRead;
    }


  assert( TotalBytesRead == fileSize );
  printf("int dvsFileSize = %d;\n", TotalBytesRead );

  printf("char dvsFilePtr[] = {");
  for( int i=0; i < TotalBytesRead; i++)
    {
      if( i % 32 == 0 )
        printf("\n");

      printf("0x%02x", dvsFile[i] );
      if( i != TotalBytesRead -1 )
        {
          printf(",");
        }
    }
  printf("};\n");


  if( dvsFile != NULL )
    delete [] dvsFile;

  return 0;
}
