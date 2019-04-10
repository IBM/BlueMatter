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
 // *************************************************************************
//
// DEL file output formats:
// For id files:
// traceID, node_id, traceType (0=Start, 1=Finis), traceLabel, traceShortTag, traceBaseTag,  traceDescription
//
// For data files:
// traceID, nodeID, timeStamp
//
// *************************************************************************

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cstdlib>

#include <sys/types.h>

#include <sys/timeb.h>

#define NO_PK_PLATFORM

// This should probably be a much tighter struct - like an STL map.
// These represent all the trace lines that might be hit based on messages.
#define FXLOG_MAX_NODES   (512)
#define FXLOG_MAX_STREAMS (256)

// NOTE: code casts this thing to a long long to check for log line matchs - must be 8 chars
char FXLOG_TAG_STR[] = "PKTRACE ";
#define MAX_LINE (16*1024)

char FXLOG_DATA_STR[] = "DATA ";
char FXLOG_ID_STR[]   = "ID ";

template< class ThingType >
int
Hex2Thing( char *source, ThingType & aThing )
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
         return( -1 );
      };

    llwork      = ( llwork << 4 ) + v;
    llworkcount ++;

    if( llworkcount == 2 * sizeof( long long) )
      {
      TheThing.mLLThing[ llcounter ] = llwork;
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

  memcpy( &aThing, &(TheThing.mThing), sizeof( ThingType ) );


  return( 0 );

  }

struct TraceMapElt
{
  // key components
  int d_traceId;
  int d_node;
  // data
  int d_traceType; // start=0 or finish=1 for now
  int d_globalId; // remapping of d_label, d_traceType onto ints good
                  // for all nodes
};

struct GlobalMapElt
{
  // key components
  std::string d_label;
  int d_traceType;
  //data
  int d_globalId;
  std::string d_desc;
};

struct CompareElt
{
  inline bool operator()(const TraceMapElt& e1, const TraceMapElt& e2) const;
};

struct CompareGlobalElt
{
  inline bool operator()(const GlobalMapElt& e1, const GlobalMapElt& e2) const;
};

bool CompareElt::operator()(const TraceMapElt& e1, const TraceMapElt& e2) const
{
  if (e1.d_traceId != e2.d_traceId)
    {
      return (e1.d_traceId < e2.d_traceId);
    }
  return (e1.d_node < e2.d_node);
}

bool CompareGlobalElt::operator()(const GlobalMapElt& e1, const GlobalMapElt& e2) const
{
  if (e1.d_label != e2.d_label)
    {
      return(e1.d_label < e2.d_label);
    }
  return(e1.d_traceType < e2.d_traceType);
}

main( int argc, char **argv, char **)
  {
    typedef std::set<TraceMapElt, CompareElt> TraceMap;
    TraceMap traceMap;

    typedef std::set<GlobalMapElt, CompareGlobalElt> GlobalIdMap;
    GlobalIdMap globalIdMap;
    std::vector<GlobalMapElt> byGlobalId;

    const char* idFile = "idFile.del";
    const char* dataFile = "traceData.del";

    unsigned long RunMgz = 700 ;  // allow stating the machine clock rate in mgz

    // open files for output (DEL format)
    switch(argc)
      {
      case 4:
        RunMgz = atoi(argv[1]);
        idFile = argv[2];
        dataFile = argv[3];
        break;
      default:
        std::cerr << argv[0] << " clockSpeedMhz idFileName dataFileName" << std::endl;
        exit(-1);
      }

    std::ofstream idDel(idFile);
    assert(idDel);
    if (!idDel)
      {
        std::cerr << "Error opening file " << idFile << std::endl;
        exit(-1);
      }
    std::ofstream dataDel(dataFile);
    assert(dataDel);
    if (!dataDel)
      {
        std::cerr << "Error opening file " << dataFile << std::endl;
        exit(-1);
      }
  ////////Need to think about this.  It might be slick to convert to time
  ////////... the trace tool needs the internal 'LTime' to represent pico seconds
  ////////
  ////////double Clock2NanoSecsMult = 1000000000.0 / ( RunMgz * 1000 * 1000 )

  FILE *fp1 = stdin;

  char LogLine[ MAX_LINE ];

  unsigned long long EarliestTraceEvent = 0xFFFFFFFFFFFFFFFFll;

  for( int LineNo = 0; !feof( fp1 ) ; LineNo++ )
    {
    char * rc1 = fgets( LogLine, MAX_LINE, fp1 );

    if( rc1 == NULL )
      return(0);

    if( rc1 < NULL )
      exit(-1);

    char * cursor = &( LogLine[ 0 ] );

    while( *cursor && (*cursor != '>') && ((cursor - &LogLine[0]) < MAX_LINE) )  // search for payload start
      cursor++ ;

    if( (! *cursor) || ((cursor - &LogLine[0]) >= MAX_LINE) )
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
#if defined(PRINT_STUFF)
	printf("DATA LINE >%s<\n", cursor );
#endif
	cursor += strlen( FXLOG_DATA_STR );
#if defined(PRINT_STUFF)
	printf(" Got Data string\n");
#endif

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
		printf("\n***Hex2Thing<long long> failed to unload 16 hex chars from log line.  cursor points to: >%s<\n", cursor );
		break;  // exit( -1 );
	      }
	    cursor += 2 * sizeof( long long );
#if defined(PRINT_STUFF)
	    printf("%016llX\n", ll );
#endif

        unsigned short id = ll >> 48;
        ll = ll & 0x0000FFFFFFFFFFFFll;

        unsigned long secs = ll / (RunMgz * 1000000);
        unsigned long pclocks = (ll % (RunMgz * 1000000));
        unsigned long nsecs = pclocks * (1000.0/RunMgz) ;

        unsigned long long TracerTimeFormat = ll * (1000.0/RunMgz) ;

#if defined(PRINT_STUFF)
        printf("id %04X pclocks %18lld (0x%016llX) nanosec %16lld  secs/nsecs %9u.%09u \n", id, ll, ll, TracerTimeFormat, secs, nsecs );
#endif

        // Should remap to a common id scheme right here
        // index by trace_id and node
        TraceMapElt tryElt;
        tryElt.d_traceId = id;
        tryElt.d_node = node;
        TraceMap::const_iterator iter = traceMap.find(tryElt);
        if (iter != traceMap.end())
          {
            //            dataDel << id << ", " << node << ", " << ll
            // << std::endl;
            dataDel << iter->d_globalId << ", " << node << ", " << ll
                    << std::endl;
          }
        else
          {
            std::cerr << "Global ID not found: " << id << ", "
                      << node << ", " << ll << std::endl;

          }

        ///// TraceStreamSet[ node ][ id ].AddHitToTraceStream( ll );
        // ******************************************************************
        //        TraceStreamSet[ node ][ id ].AddHitToTraceStream( TracerTimeFormat );
        // ******************************************************************

        } //matches while

      }
    else if( ! strncmp( cursor, FXLOG_ID_STR, strlen( FXLOG_ID_STR) )  )
      {
      char working[MAX_LINE];
#if defined(PRINT_STUFF)
      printf("ID LINE >%s<\n", cursor );
      printf(">%s<\n", cursor );
#endif
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

      //NULL Term the name in the buffer
      *end_cursor = NULL;

#if defined(PRINT_STUFF)
      printf("Got from node >%04X<  id >%04X< str >%s<\n", node, id, cursor );
#endif
      // I want to parse some stuff out of the identifier
      strcpy(working, cursor);
      char* tok = strtok(working, ":");
      for (int i = 0; i < 2; ++i)
      {
        tok = strtok(NULL, ":");
      }
      // Get the "real" identifier first, but clip off any subscript stuff
      char* clip = strchr(tok, '[');
      int idLen = 0;
      char* idBase = tok;
      if (clip != NULL)
        {
          idLen = clip - idBase;
        }
      else
        {
          idLen = strlen(idBase);
        }

      // next look for Start or Finis in the "real" identifier so that
      // we clip that off and pair up start/finish pairs properly
      int baseLen = idLen - 5;
      if ((strncmp(&idBase[baseLen], "Start", 5) == 0) ||
          (strncmp(&idBase[baseLen], "Finis", 5) == 0))
        {
          idLen = baseLen;
        }


      // now grab the "short" identifier that usually carries the
      // Start/Finis suffix
      tok = strtok(NULL, ":");

      // Now I'm going to assume that this identifier ends in either
      // Start or Finis (5 characters).
      baseLen = strlen(tok) - 5;
      int traceType = -1; // 0 is start, 1 is finish
      if (strcmp(&tok[baseLen], "Start") == 0)
      {
        traceType = 0;
      }
      else
      {
        if (strcmp(&tok[baseLen], "Finis") == 0)
        {
          traceType = 1;
        }
      }

      // ***********************************************************************
      // From a line like:
      //  "019:0:UDF_KineticEnergy[0]:LFTDStart::|include|BlueMatter|LocalFragmentDirectDriver.hpp[0043]"
      // we should have parsed:
      // node = 19
      // traceType = 0 (from LFTDStart)
      // idBase = "UDF_KineticEnergy"
      // tok = "LFTDStart"
      // tok(2) = "LFTD"
      // ***********************************************************************
      
      idBase[idLen] = '\0';
      
      GlobalMapElt gElt;
      TraceMapElt tElt;

      gElt.d_label = std::string(idBase);
      gElt.d_traceType = traceType;
      gElt.d_globalId = globalIdMap.size();
      gElt.d_desc = std::string(cursor);
      std::pair<GlobalIdMap::iterator, bool> rc1 = globalIdMap.insert(gElt);
      if (!rc1.second) // a mapping already exists for this label/trace_type
        {
          if (gElt.d_label != rc1.first->d_label)
            {
              std::cerr << "Mismatch between labels for global id = "
                        << rc1.first->d_globalId << " existing label: "
                        << rc1.first->d_label << " new label: "
                        << gElt.d_label << std::endl;
              std::exit(-1);
            }
          tElt.d_globalId = rc1.first->d_globalId;
        }
      else // no mapping exists
        {
          tElt.d_globalId = gElt.d_globalId;
          byGlobalId.push_back(gElt);

          // going to put out global_id, trace_type, trace_label,
          // trace_desc
          
          idDel << gElt.d_globalId << ", "
                << gElt.d_traceType << ", "
                << "\"" << gElt.d_label << "\"" << ", "
                << "\"" << gElt.d_desc << "\"" << ", "
                << std::endl;

        }
      tElt.d_traceId = id;
      tElt.d_node = node;
      tElt.d_traceType = traceType;
      std::pair<TraceMap::iterator, bool> rc = traceMap.insert(tElt);
      if (!rc.second)
        {
          std::cerr << "Duplicate entries for global id " << rc.first->d_globalId
                    << " label: " << byGlobalId[rc.first->d_globalId].d_label
                    << " traceId: " << tElt.d_traceId
                    << " node: " << tElt.d_node << std::endl;
        }

      //THIS CALL IS: int __pkTraceStream::__pkTraceStreamConnect( char     *TraceStreamName );
      assert( node < FXLOG_MAX_NODES );
      assert( id   < FXLOG_MAX_STREAMS );
      // *************************************************************
      // next line is commented out because it may cause a hang for large
      // files
      //      TraceStreamSet[ node ][ id ].__pkTraceStreamConnect( cursor );
      // *************************************************************

      }

    }

  // **************************************************************************
  // THE PROGRAM NEVER GETS HERE SO THESE INSTRUCTIONS ARE NEVER EXECUTED
  // **************************************************************************

  std::cerr << "byGlobalId.size() = " << byGlobalId.size() << std::endl;
  for (std::vector<GlobalMapElt>::const_iterator iter = byGlobalId.begin();
       iter != byGlobalId.end();
       ++iter)
    {
      // going to put out global_id, trace_type, trace_label,
      // trace_desc
      
      idDel << iter->d_globalId << ", "
            << iter->d_traceType << ", "
            << "\"" << iter->d_label << "\"" << ", "
            << "\"" << iter->d_desc << "\"" << ", "
            << std::endl;
    }

  fflush(stdout);
  idDel.close();
  dataDel.close();
  return(0);
  }
