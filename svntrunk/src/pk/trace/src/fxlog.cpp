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
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <sys/timeb.h>

#define NO_PK_PLATFORM

#include <pk/pktrace_pk.hpp>
#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_FXLOG
#define PKFXLOG_FXLOG (0)
#endif

#ifndef PKFXLOG_FXLOG_MAIN
#define PKFXLOG_FXLOG_MAIN (0)
#endif


unsigned long RunMhz = 500 ;  // allow stating the machine clock rate in mgz

const char * pkTraceFileName ;

// NOTE: code casts this thing to a long long to check for log line matchs - must be 8 chars
char FXLOG_TAG_STR[] = "PKTRACE ";
#define MAX_LINE (16*1024)

char FXLOG_DATA_STR[] = "DATA ";
char FXLOG_ID_STR[]   = "ID ";

// This should probably be a much tighter struct - like an STL map.
// These represent all the trace lines that might be hit based on messages.
#define FXLOG_MAX_NODES   (512)
#define FXLOG_MAX_STREAMS (256)
TraceClient TraceStreamSet[512][256];

unsigned long long EarliestTraceEvent = 0xFFFFFFFFFFFFFFFFll;
unsigned long long EarliestTraceEventOnNode [ FXLOG_MAX_NODES ];

char LogLine[ MAX_LINE ];

template< class ThingType >
int
Hex2Thing( char *source, ThingType & aThing )
  {
    
    int v ;
    aThing = 0 ;
    char               * ch = source;
    for( int cnt = 0; cnt < 2 * sizeof( ThingType ) ; cnt ++ )
      {
      switch( *ch )
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
           {
           fprintf( stderr, "Hex2Thing() got a non hex character >%c< \n", *ch );
           exit( -1 );
           }
        };
        ch++ ;
        aThing = ( aThing << 4 ) + v ;
        BegLogLine(PKFXLOG_FXLOG)
          << "so far aThing=" << aThing
          << " v=" << v
          << EndLogLine ;
      }
    
    BegLogLine(PKFXLOG_FXLOG)
      << "aThing=" << aThing
      << EndLogLine ;

    return 0 ;
  }
template< class ThingType >
int
xHex2Thing( char *source, ThingType & aThing )
  {
  enum{ llcnt = ( ( sizeof(ThingType) + sizeof(long long) - 1 ) / sizeof( long long ) )  };

  union
    {
    unsigned long long mLLThing[ llcnt ];
    ThingType mThing;
    } TheThing;

  int                  llcounter = 0;
  int                  llworkcount = 0;
  unsigned long long   llwork = 0 ;

  char               * ch = source;
  unsigned             v;


  for( int cnt = 0; cnt < 2 * sizeof( ThingType ) ; cnt ++ )
    {
    switch( *ch )
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
         {
         fprintf( stderr, "Hex2Thing() got a non hex character >%c< \n", *ch );
         exit( -1 );
         }
      };

    llwork      = ( llwork << 4 ) + v;
    llworkcount ++;

    if( llworkcount == 2 * sizeof( long long) )
      {
      TheThing.mLLThing[ llcounter ] = llwork;
      BegLogLine(PKFXLOG_FXLOG)
        << "TheThing.mLLThing[" << llcounter
        << "]=" << llwork
        << " sizeof( long long)=" << sizeof( long long)
        << EndLogLine ;
      llcounter ++ ;
      llwork      = 0;
      llworkcount = 0;
      }

    ch++;

    }

  while( llworkcount < 2 * sizeof( long long ) )
    {
    llwork = llwork << 4;
    llworkcount++;
    }

  TheThing.mLLThing[ llcounter ] = llwork;
  BegLogLine(PKFXLOG_FXLOG)
    << "TheThing.mLLThing[" << llcounter
    << "]=" << llwork
    << " sizeof( long long)=" << sizeof( long long)
    << EndLogLine ;

  memcpy( &aThing, &(TheThing.mThing), sizeof( ThingType ) );

  BegLogLine(PKFXLOG_FXLOG)
    << "aThing=" << aThing
    << EndLogLine ;
  
  return( 0 );

  }

static void
DoFile(int ScanTime, FILE *fp1 )
  {

  for( int LineNo = 0; !feof( fp1 ) ; LineNo++ )
    {
    char * rc1 = fgets( LogLine, MAX_LINE, fp1 );

    if( rc1 == NULL )
      return;

    if( rc1 < NULL )
      {
      fprintf(stderr, "fgets() returned < NULL\n" );
      exit(-1);
      }

    char * cursor = &( LogLine[ 0 ] );

    while( *cursor && *cursor != '>' )  // search for payload start
      cursor++ ;

    if( ! *cursor )
      continue;     // non logline in stream

    cursor++;

    // Fairly quickly look at the first sizeof(int) bytes of the payload
    // to see if they are what we are looking for - if not, continue scanning.
    if( *((long long*) cursor) != *((long long*)(FXLOG_TAG_STR)) )
      continue;

    cursor += ( sizeof( FXLOG_TAG_STR ) - 1 ); // -1 because sizeof() will include the NULL

    // NOTE : Packet type strings include the space.

    if( ! strncmp( cursor, FXLOG_DATA_STR, strlen( FXLOG_DATA_STR) )  )
      {
        BegLogLine(PKFXLOG_FXLOG)
          << "Data line [" << cursor
          << "]"
          << EndLogLine ;
//      printf("DATA LINE >%s<\n", cursor );
      cursor += strlen( FXLOG_DATA_STR );
//      printf(" Got Data string\n");

      unsigned short node = 0xFFFF;
      Hex2Thing( cursor, node );
      assert( node < FXLOG_MAX_NODES );
      cursor += 2 * sizeof( node );  //
      cursor ++;

      while( *cursor != '<' )
        {
        unsigned long long ll;
        if( Hex2Thing( cursor, ll ) != 0 )
          {
            BegLogLine(PKFXLOG_FXLOG)
              << "***Hex2Thing<long long> failed to unload 16 hex chars from log line.  cursor points to: [" << cursor
              << "]"
              << EndLogLine ;
//          printf("\n***Hex2Thing<long long> failed to unload 16 hex chars from log line.  cursor points to: >%s<\n", cursor );
          break;  // exit( -1 );
          }
        cursor += 2 * sizeof( long long );
        BegLogLine(PKFXLOG_FXLOG)
          << "ll=0x" << (void *)(ll>32) << (void *)(ll & 0xffffffff)
          << EndLogLine ;
//        printf("%016llX\n", ll );

        unsigned short id = ll >> 48;
        ll = ll & 0x0000FFFFFFFFFFFFll;

        unsigned long secs = ll / (RunMhz * 1000000);
        unsigned long pclocks = (ll % (RunMhz * 1000000));
        unsigned long nsecs = pclocks * (1000.0/RunMhz) ;

        unsigned long long TracerTimeFormat = ll * (1000.0/RunMhz) ;

        BegLogLine(PKFXLOG_FXLOG)
          << "id=" << id
          << " pclocks=" << (unsigned int) ll
          << " nanoseconds=" << TracerTimeFormat
          << " secs=" << secs 
          << " nsecs=" << nsecs
          << " secs.nsecs=" << (secs+0.000000001*nsecs)
          << EndLogLine ;
        //printf("id %04X pclocks %18lld (0x%016llX) nanosec %16lld  secs/nsecs %9u.%09u \n", id, ll, ll, TracerTimeFormat, secs, nsecs );


        if( ScanTime )
          {
          ////TraceStreamSet[ node ][ id ].AddHitToTraceStream( TracerTimeFormat );
          if( TracerTimeFormat < EarliestTraceEventOnNode[ node ] )
            EarliestTraceEventOnNode[ node ] = TracerTimeFormat;

          }
        else
          {
          TracerTimeFormat -= EarliestTraceEventOnNode[ node ];
        fflush(stdout) ; 

          TraceStreamSet[ node ][ id ].AddHitToTraceStream( TracerTimeFormat );

          // This is bad - but, you don't get the singleton unless this bit comes after the first Hit.
          if( ll < EarliestTraceEvent )
            {
            EarliestTraceEvent = ll;

            __pkTraceServer* tserver = __pkTraceServer::LockIF(27);

            tserver->SetRawStartTime( TracerTimeFormat );

            tserver->ZeroConversionFactor(); // make sure we don't screw things up by using AIX timebase conversion
            
            BegLogLine(PKFXLOG_FXLOG)
              << "Updating global EarliestTraceEvent value now " << EarliestTraceEvent
              << EndLogLine ;
//            printf("Updating global EarliestTraceEvent value now %ll (0x%0llX)\n", EarliestTraceEvent );
            tserver->UnLockIF();
            }

          }

        }

      }
    else if( ! strncmp( cursor, FXLOG_ID_STR, strlen( FXLOG_ID_STR) )  )
      {
        BegLogLine(PKFXLOG_FXLOG)
          << "ID line [" << cursor
          << "]"
          << EndLogLine ;
//      printf("ID LINE >%s<\n", cursor );
//      printf(">%s<\n", cursor );
      unsigned short id   = 0xFFFF;
      cursor += strlen( FXLOG_ID_STR );

      unsigned short node = 0xFFFF;
      Hex2Thing( cursor, node );
      assert( node < FXLOG_MAX_NODES );
      cursor += 2 * sizeof( node );  //
      cursor ++;

      Hex2Thing( cursor, id );

      cursor += 2 * sizeof( id );  //
      cursor ++;                   // space after id

      char * end_cursor = & ( cursor[ strlen( cursor ) ] );

      while( *end_cursor != '<' && end_cursor >= cursor )
        end_cursor--;

      //NUL Term the name in the buffer
      *end_cursor = 0;

      BegLogLine(PKFXLOG_FXLOG)
        << "Got from node " << node
        << " id=" << id
        << " str[" << cursor
        << "]"
        << EndLogLine ;
//      printf("Got from node >%04X<  id >%04X< str >%s<\n", node, id, cursor );

      //THIS CALL IS: int __pkTraceStream::__pkTraceStreamConnect( char     *TraceStreamName );
      assert( node < FXLOG_MAX_NODES );
      assert( id   < FXLOG_MAX_STREAMS );

      if( ! ScanTime )
        {
        fflush(stdout) ; 
        TraceStreamSet[ node ][ id ].__pkTraceStreamConnect( cursor );
        }
      }

    }
  }

static void UsageMessage(const char * programName)
{
        fprintf(stderr,"%s [-deskew] [-runmhz RunMHZ] [-i infile] [-o outfile]\n"
                        "    Convert BGL log-lines to AIX memory-map format\n"
                        "    Output file goes to Maptrc/* if not specified explicitly\n"
                        ,programName) ;
}

// Parse arguments
static void init( int argc, char*argv[],
          int& aHelpRequest,
          const char *& aInFile,
          const char *& aOutFile,
          int& aDeskew,
          unsigned long& aRunMHz)
  {          
    BegLogLine(PKFXLOG_FXLOG_MAIN)
      << "init argc=" << argc
      << EndLogLine ;
    for(int i=1; i<argc; i++)
      {
          BegLogLine(PKFXLOG_FXLOG_MAIN)
            << " argv[" << i
            << "]=" << argv[i]
            << EndLogLine ;
          if (!strcmp(argv[i],"-runmhz"))
              {
                aRunMHz=atoi(argv[++i]);
              }
          else
          if (!strcmp(argv[i],"-i"))
              {
                aInFile = argv[++i];
              }
          else    
          if (!strcmp(argv[i],"-o")) 
              {
                aOutFile = argv[++i] ;
              }
          else    
          if (!strcmp(argv[i],"-deskew")) 
              {
                aDeskew = 1 ;
              }
          else 
            {
              aHelpRequest = 1 ;
            }
      }
}

int main( int argc, char **argv, char **)
  {
  ////////Need to think about this.  It might be slick to convert to time
  ////////... the trace tool needs the internal 'LTime' to represent pico seconds

    const char * inFile = "-" ;
    int deskew = 0 ; 
    int helpRequest=0 ;
    init(argc,argv,helpRequest,inFile,pkTraceFileName,deskew,RunMhz) ;
    if( helpRequest )
      {
        UsageMessage(argv[0]) ;       
      }
    else
      {   
        FILE *fp1 ;
        if( !strcmp(inFile,"-"))
          {
            fp1 = stdin;
          }
        else
          {
            fp1 = fopen(inFile,"r") ;
          }
        assert(fp1) ;
        fprintf(stderr,"RunMhz = %d\n", RunMhz );
        for( int i = 0; i < FXLOG_MAX_NODES; i++ )
           {
             EarliestTraceEventOnNode[ i ] = 0xFFFFFFFFFFFFFFFFll;
           }
        if( deskew )
          {
             DoFile( 1, fp1 );
             fseek( fp1, 0, SEEK_SET );            
          }
        DoFile( 0, fp1 );
      }
    return 0 ;
  }


