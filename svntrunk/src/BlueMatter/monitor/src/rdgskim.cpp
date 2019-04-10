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
 
// xlC -O3 rdgskim.cpp -DNO_PK_PLATFORM -ordgskim.exe -I../../../../export/rios_aix_5/usr/include/


#include <BlueMatter/ExternalDatagram.hpp>

#include <strstream>
#include <unistd.h>
#include <assert.h>
#include <list>
#include <BlueMatter/TailFile.hpp>
#include <iostream>

#define DEBUG 0

int ClassFlag[256];

main( int argc, char **argv )
  {

  if( argc == 1 )
    {
    fprintf( stderr, "Usage %s 1 2 6\n", argv[0] );
    fprintf( stderr, "Integer args are classes of packets to be passed through\n");
    fprintf( stderr, "Known classes are from ../src/BlueMatter/io/include/ExternalDatagram.hpp\n");
    fprintf( stderr, "Unknown = 0,          \n");
    fprintf( stderr, "Control = 1,          \n");
    fprintf( stderr, "DynamicVariables = 2, \n");
    fprintf( stderr, "Energy = 3,           \n");
    fprintf( stderr, "Information = 4,      \n");
    fprintf( stderr, "UDF = 5,              \n");
    fprintf( stderr, "RTPValues = 6         \n");
    exit( -1 );
    }

  for( int i = 1; i < argc; i++ )
    {
    int Class = atoi( argv[ i ] );
    fprintf(stderr, "Skimming class %d\n", Class );
    assert( Class >= 0 && Class < 256 );
    ClassFlag[ Class ] = 1;
    }

  Tail Input;
  Input.Open( "-", 10 );
  assert( Input.OK() );

  while(1)
    {
    char PktBuf[ MAXPACKEDPACKETSIZE ];

    ED_Header * PktHdr = (ED_Header *) PktBuf;

    if( ! Input.Read( PktHdr, sizeof( ED_Header ) ) )
      break;

    int payload_len = PktHdr->mLen - sizeof( ED_Header );
    if( ! Input.Read( & ( PktBuf[ sizeof( ED_Header ) ] ), payload_len ) )
      break;

    if( ClassFlag[ PktHdr->mClass ] )
      write( 1, (void *) &PktBuf, PktHdr->mLen );

    }
  }
