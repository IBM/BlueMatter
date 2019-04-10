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
 // This file diffs two PKFXLOG files
// I've got no doubt there are many better ways
// to do this - why don't you code one up?
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <sys/timeb.h>

main( int argc, char **argv, char **)
  {
  if( argc != 2 )
    {
    fprintf(stderr,"Usage: %s <filename1>\n", argv[0] );
    exit(-1);
    }

  FILE *fp1 = fopen( argv[1], "r" );
  if( fp1 == NULL )
    {
    fprintf(stderr,"Usage: %s Can't open file 1 >%s<\n", argv[0], argv[1] );
    exit(-1);
    }

#define MAX_LINE (16*1024)

  char line1[ MAX_LINE ];

  int BadFrame = 0;

  for( int LineNo = 0; !feof( fp1 ) ; LineNo++ )
    {
    char * rc1 = fgets( line1, MAX_LINE, fp1 );

   if( rc1 == NULL )
     return(0);

    int l1_s, l1_e;


    for( l1_s = 0; line1[ l1_s ] != '>' ; l1_s++ )
      {
      //assert( l1_s < strlen( line1 ) );
      if( l1_s == strlen( line1 ) )
        {
        fprintf(stderr,"LineNo %d \nline 1 >%s< \nchar '>' not found in line1 \n", LineNo, line1 );
        BadFrame = 1;
        break;
        }
      }

    for( l1_e = strlen( line1 ) - 1; line1[ l1_e ] != '<' ; l1_e-- )
      {
      ///assert( l1_e > 0 );
      if( l1_e == 0 )
        {
        fprintf(stderr,"strlen( line 1 ) %d\n", strlen( line1 ) );
        fprintf(stderr,"LineNo %d \nline 1 >%s< \nchar '<' not found in line1\n", LineNo, line1 );
        BadFrame = 1;
        break;
        }
      }

    if( BadFrame )
      {
      fprintf(stderr,"\n");
      continue;
      }

    line1[ l1_e + 1 ] = '\0';

    puts( & (line1[ l1_s ]) );

    }
  }
