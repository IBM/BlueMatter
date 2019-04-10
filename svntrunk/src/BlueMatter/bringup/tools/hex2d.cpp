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
#include <stdlib.h>
#include <string.h>

// This program finds fields tagged with :XD: followed by 16 hex chars of a hex dumped IEEE double and converts them to
// a human readable format.  The format is %f or one supplied by the caller.

//To compile on aix: xlC -O hex2d.cpp -o hex2d.exe
//To run: hex2d.exe  -f "%g" <samp.txt >out


// Tag string should be 4 chars becuase we look for it as an int
char * TagString = ":XD:";

unsigned char MatchLevel  = 0;


char ** Argv = NULL;
void Usage(char * str)
  {
  fprintf( stderr, "%s: Filter to scan for tag string %s and then scan in 16 hex chars to printf as double.\n", Argv[0] );
  fprintf( stderr, "%s: FAILED : %s\n", Argv[0], str );
  fprintf( stderr, "%s: [-h] [-f fmt] <infile >outfile\n", Argv[0] );
  fprintf( stderr, "%s: -h : print the hex dump string for the double - default is off\n", Argv[0] );
  fprintf( stderr, "%s: -f fmt : where fmt is your fave double precision printf format sting - default: %f\n", Argv[0] );
  exit( -1 );
  }

main(int argc, char ** argv, char **)
  {
  char fmt[32] = "%f";
  int DumpHexFlag = 0;

  for( int argn = 1; argn < argc; argn++ )
    {
    if( !strncmp( argv[ argn ], "-f", 2 ) )
      {
      char * argp = &( argv[ argn ][ 2 ] );
      if( *argp == NULL )
        {
        argn++;
        if( argn >= argc )
          Usage("-f arg needs printf format descriptor");
        argp = argv[ argn ];
        }
      strncpy( fmt, argp, sizeof( fmt ) );
      }
    else if ( !strncmp( argv[ argn ], "-h", 2 ) )
      {
      DumpHexFlag = 1;
      }
    else
      {
      printf( "%s: Bad arg #%d >%s<\n", argv[0], argn, argv[ argn ] );
      Usage("Bad arg.");
      }
    }

  union
    {
    unsigned u32;
    unsigned char u8[4];
    } TestField, MatchField;

  MatchField.u32 = 0;
  TestField.u32  = 0;

  // Prime the pumps.
  for( int i = 0; i < sizeof( MatchField.u32 ); i ++ )
    {
    MatchField.u32     = MatchField.u32 << 8;
    MatchField.u8[ 3 ] = TagString[ i ];
    }

  int BufCnt = 0;

  while( !feof( stdin ) )
    {
    if( TestField.u32 != MatchField.u32 )
      {
      if( BufCnt >= sizeof( TestField ) )
        putc( TestField.u8[ 0 ], stdout );

      TestField.u32     = TestField.u32 << 8;
      TestField.u8[ 3 ] = getc( stdin );
      BufCnt++;
      continue;
      }

    // else... dropping through on means tag string was found.

    union
      {
      long long ll;
      double     d;
      }      V64;

    V64.ll  = 0;
    int v   = 0;

    int error = 0;

    BufCnt        = 0;
    TestField.u32 = 0;
    if( DumpHexFlag )
      printf("%s", TagString );

    for( int cnt = 0; cnt < 16; cnt ++ )
      {
      // we keep this incase we are wrong and have to put out these chars
      char ch = getc(stdin);

      if(DumpHexFlag )
        putc( ch, stdout );

      switch( ch )
        {
        case '0' :  v = 0x00; break;
        case '1' :  v = 0x01; break;
        case '2' :  v = 0x02; break;
        case '3' :  v = 0x03; break;
        case '4' :  v = 0x04; break;
        case '5' :  v = 0x05; break;
        case '6' :  v = 0x06; break;
        case '7' :  v = 0x07; break;
        case '8' :  v = 0x08; break;
        case '9' :  v = 0x09; break;
        case 'A' :  v = 0x0A; break;
        case 'a' :  v = 0x0a; break;
        case 'B' :  v = 0x0B; break;
        case 'b' :  v = 0x0b; break;
        case 'C' :  v = 0x0C; break;
        case 'c' :  v = 0x0c; break;
        case 'D' :  v = 0x0D; break;
        case 'd' :  v = 0x0d; break;
        case 'E' :  v = 0x0E; break;
        case 'e' :  v = 0x0e; break;
        case 'F' :  v = 0x0F; break;
        case 'f' :  v = 0x0f; break;
        default  :
          error = 1;
        };

      if( error )
        break;

      V64.ll = ( V64.ll << 4 ) + v;

      }

    if( ! error )
      {
      if( DumpHexFlag )
        putc( ' ', stdout );
      printf(fmt, V64.d );
      }
    } // end while ( !feof(stdin ) )
  }
