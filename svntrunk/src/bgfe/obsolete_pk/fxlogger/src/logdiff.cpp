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
  if( argc != 3 )
    {
    fprintf(stderr,"Usage: %s <filename1> <filename2>", argv[0] );
    exit(-1);
    }

  FILE *fp1 = fopen( argv[1], "r" );
  if( fp1 == NULL )
    {
    fprintf(stderr,"Usage: %s Can't open file 1 >%s<", argv[0], argv[1] );
    exit(-1);
    }

  FILE *fp2 = fopen( argv[2], "r" );
  if( fp2 == NULL )
    {
    fprintf(stderr,"Usage: %s Can't open file 2 >%s<", argv[0], argv[2] );
    exit(-1);
    }

#define MAX_LINE (16*1024)

  char line1[ MAX_LINE ];
  char line2[ MAX_LINE ];

  int MatchCount = 0;
  struct timeb lastoutput;

  ftime( & lastoutput );

  for( int LineNo = 0; !feof( fp1 ) && !feof( fp2 ) ; LineNo++ )
    {
    // EACH LINE MUST MATCH -
    char * rc1 = fgets( line1, MAX_LINE, fp1 );
    char * rc2 = fgets( line2, MAX_LINE, fp2 );

    if( rc1 == NULL || rc2 == NULL )
      {
      if( rc1 == NULL && rc2 != NULL )
        printf( "File 2 longer than File 1\n");
      else if( rc1 != NULL && rc2 == NULL )
        printf( "File 1 longer than File 2\n");
      printf("Matched %d lines\n", MatchCount );
      return(0);
      }

    int l1_s, l1_e, l2_s, l2_e;

    int BadFrame = 0;

    for( l1_s = 0; line1[ l1_s ] != '>' ; l1_s++ )
      {
      //assert( l1_s < strlen( line1 ) );
      if( l1_s == strlen( line1 ) )
        {
        printf("LineNo %d \nline 1 >%s<\n line2 >%s<\n char '>' not found in line1 \n", LineNo, line1, line2 );
        BadFrame = 1;
        break;
        }
      }

    for( l1_e = strlen( line1 ) - 1; line1[ l1_e ] != '<' ; l1_e-- )
      {
      ///assert( l1_e > 0 );
      if( l1_e == 0 )
        {
        printf("strlen( line 1 ) %d\n", strlen( line1 ) );
        printf("LineNo %d \nline 1 >%s<\n line2 >%s<\n char '<' not found in line1\n", LineNo, line1, line2 );
        BadFrame = 1;
        break;
        }
      }

    for( l2_s = 0; line2[ l2_s ] != '>' ; l2_s++ )
      {
      // assert( l2_s < strlen( line2 ) );
      if( l2_s == strlen( line2 ) )
        {
        printf("LineNo %d \nline 1 >%s<\n line2 >%s<\n char '>' not found in line2 \n", LineNo, line1, line2 );
        BadFrame = 1;
        break;
        }
      }

    for( l2_e = strlen( line2 ) - 1; line2[ l2_e ] != '<' ; l2_e-- )
      {
      //assert( l2_e > 0 );
      if( l2_e == 0 )
        {
        printf("strlen( line 2 ) %d\n", strlen( line2 ) );
        printf("LineNo %d \nline 1 >%s<\n line2 >%s<\n char '<' not found in line2\n", LineNo, line1, line2 );
        BadFrame = 1;
        break;
        }
      }

    if( BadFrame )
      continue;

    line1[ l1_e ] = '\0';
    line2[ l2_e ] = '\0';

    if( strcmp( &(line1[ l1_s ]), &(line2[ l2_s ]) ) == 0 )
      {
      MatchCount++;
      struct timeb now;
      ftime( & now );
      if( now.time - lastoutput.time > 1 )
        {
        printf("LineNo %d Matched %d\n", LineNo, MatchCount );
        lastoutput = now;
        }
      }
    else
      {

      line1[ l1_e ] = '<';
      line2[ l2_e ] = '<';

      char *shorty = NULL;
      int   shift = 0;
      if( l1_s < l2_s )
        {
        shorty = line1;
        shift = l2_s - l1_s;
        }
      else if ( l2_s < l1_s )
        {
        shorty = line2;
        shift = l1_s - l2_s;
        }

      if( shorty )
        {
        int i;
        for( i = strlen( shorty ); i ; i-- )
          {
          shorty[ shift + i ] = shorty[ i ];
          }
        for( i = 0; i <= shift; i++ )
          {
          shorty[i] = ' ';
          }
        }

      char Header[ MAX_LINE ];
      sprintf(Header, "___ LINE %6d ___ ",
             LineNo );

      int start = 0;
      for( int i = strlen( Header ); i < strlen( line1 ); i++ )
        {
        if( start )
          {
          if( line1[i] == '<' )
            break;
          if( line1[i] == line2[i] )
            Header[i] = '_';
          else
            Header[i] = 'x';
          }
        else
          Header[i] = '_';
        if( line1[ i ] == '>' )
          start = 1;
        }
      printf("%s\n", Header);

      printf("%s", line1 );
      printf("%s", line2 );
      }
    }
  }
