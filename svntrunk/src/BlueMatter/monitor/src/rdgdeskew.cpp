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
 // * This program provides a filter which will de-skew an rdg packet stream.
// * This code will remove skew from a packet stream.
// * The first packet must not be skewed greater than +- ( ArchPhaseCount / 2 )

// Note, the reason a packet cannot be more than 127 slots skewed is that
// on if the first packet were allowed to be more than 127 out, then it
// would be impossible to tell (simply) if it were a strageler from an
// earlier phase or a very early packet from a future phase.

#include <BlueMatter/ExternalDatagram.hpp>

#include <strstream>
#include <unistd.h>
#include <assert.h>
#include <list>
#include "TailFile.hpp"
#include <iostream>


#define DEBUG 0

inline
int
MinDelta(int aval, int bval, int modval)
  {
  int rc;
  int amb = ((aval - bval) + modval ) % modval;
  int bma = ((bval - aval) + modval ) % modval;
  if( amb < bma )
    rc = amb;
  else
    rc = bma;
  return(rc);
  }

enum { ArchPhaseCount = 1 << ED_BITS_FOR_STEP };
int    SetMaxSkew = ArchPhaseCount / 2 - 1 ; // +- 127

struct RdgPktBuf { char Data[ MAXPACKEDPACKETSIZE ]; };

std::list< RdgPktBuf > PktPhaseBin[ ArchPhaseCount ];

int SkewHist[ ED_BITS_FOR_STEP ];

void DumpPhaseBin( int aPhaseBin )
  {
  for(std::list< RdgPktBuf >::iterator iter = PktPhaseBin[ aPhaseBin ].begin();
      iter != PktPhaseBin[ aPhaseBin ].end();
      ++iter)
    {
    write( 1, (*iter).Data, ((ED_Header *)(*iter).Data)->mLen );
    }
  PktPhaseBin[ aPhaseBin ].clear();
  }

main( int argc, char **argv )
  {
  int PacketReadCount = 0;
  int PhaseDumpCount = 0;
  // Allow skew to be setable.
  if( argc == 2 )
    {
    SetMaxSkew = atoi( argv[1] );
    assert(( SetMaxSkew > 0) && (SetMaxSkew <= ( ArchPhaseCount -1 )) );
    }

  Tail Input;
  Input.Open( "-", 10 );
  assert( Input.OK() );

  int TailBackupOkFlag = 1;
  int SkewWindowHead = ArchPhaseCount * -2 ;
  int SkewWindowTail = ArchPhaseCount * -2 ;
  while(1)
    {
    RdgPktBuf PktBuf;
    ED_Header * PktHdr = (ED_Header *) PktBuf.Data;

    if( ! Input.Read( PktHdr, sizeof( ED_Header ) ) )
      break;
    int payload_len = PktHdr->mLen - sizeof( ED_Header );
    if( ! Input.Read( &(PktBuf.Data[ sizeof( ED_Header ) ]), payload_len ) )
      break;

    PacketReadCount++;

    int CurrentPktPhase = PktHdr->mShortTimeStep;

    if( SkewWindowTail == ArchPhaseCount * -2 )
      {
      SkewWindowTail = CurrentPktPhase;
      }
    else if( SkewWindowHead == ArchPhaseCount * -2 )
      {
      int delta = ( SkewWindowTail - CurrentPktPhase ) % ArchPhaseCount ;
      if( abs( delta ) > SetMaxSkew )
        {
        fprintf(stderr,"Initial skew too great at %d\n", abs(delta));
        exit(-1);
        }
      if( delta < 0 )
        {
        SkewWindowHead = CurrentPktPhase;
        }
      else if( delta > 0 )
        {
        SkewWindowHead = SkewWindowTail;
        SkewWindowTail = CurrentPktPhase;
        }
      }
    else
      {
      int TailHeadAbsDelta = MinDelta( SkewWindowTail, SkewWindowHead, ArchPhaseCount);
      assert( TailHeadAbsDelta <= SetMaxSkew );
      int HeadCurAbsDelta  = MinDelta( SkewWindowHead, CurrentPktPhase, ArchPhaseCount );
      int TailCurAbsDelta  = MinDelta( SkewWindowTail, CurrentPktPhase, ArchPhaseCount );

      if( HeadCurAbsDelta + TailCurAbsDelta > TailHeadAbsDelta ) // Are we outside the tail<->head range?
        {
        if( DEBUG ) fprintf(stderr,"\nWindTail %3d   WindHead %3d  Curr %3d\n", SkewWindowTail, SkewWindowHead, CurrentPktPhase);
        if( DEBUG ) fprintf(stderr,"TailAbsD %3d + HeadAbsD %3d  =    %3d  > TailHeadAbs %3d\n",
                                    TailCurAbsDelta, HeadCurAbsDelta,
                                    HeadCurAbsDelta + TailCurAbsDelta, TailHeadAbsDelta ) ;
        if( TailCurAbsDelta < HeadCurAbsDelta ) // Is Cur closer to tail than to head?
          {
          if( (TailHeadAbsDelta + TailCurAbsDelta) < SetMaxSkew ) // Can we back up tail to inlude new packet?
            {
            if( ! TailBackupOkFlag )  // Still allowed to move tail window boundry?
              {
              fprintf(stderr,"Got packet earlier than already output first phase - fatal\n");
              exit(-1);
              }
            SkewWindowTail = CurrentPktPhase;
            }
          else // no, can't back up tail - blow out
            {
            fprintf(stderr,"Got packet requiring backup of tail beyond skew limit - fatal\n");
            exit( -1 );
            }
          }
        else // ... Cur is closer to head than tail
          {
          if( HeadCurAbsDelta < SetMaxSkew )       // This is so based on overall logic
            {
            // Write out phases to until there is room to advance head pointer
            int PhasesToWrite = TailHeadAbsDelta + HeadCurAbsDelta - SetMaxSkew;
            if( PhasesToWrite < 0 )
              PhasesToWrite = 0;
            else
              TailBackupOkFlag = 0;  // we're gonna write, so no more backin up the tail
            for( int i = 0; i < PhasesToWrite; i++ )
              {
              int p = (SkewWindowTail + i) % ArchPhaseCount;
              if( DEBUG ) fprintf(stderr,"Outputing phase %d\n", p );
              if( PktPhaseBin[ p  ].empty() )
                fprintf(stderr,"\nWARNING: EMPTY PHASE NUMBER %d, Bin %d\n", PhaseDumpCount, p);
              PhaseDumpCount++;
              DumpPhaseBin( p );
              }

            if( PhasesToWrite > 0 )
              {
              fprintf(stderr,"PhaseDumpCount %d PhasesToWrite %d\n",
                PhaseDumpCount, PhasesToWrite );
              for( int s = 0; s < (1<<ED_BITS_FOR_STEP); s++)
                if( SkewHist[ s ] )
                  fprintf(stderr," %3d:%7d ", s, SkewHist[ s ] );
              fprintf(stderr,"\n");
              fflush(stderr);
              }

            SkewWindowTail = ( SkewWindowTail + PhasesToWrite ) % ArchPhaseCount;
            SkewWindowHead = ( SkewWindowHead + HeadCurAbsDelta ) % ArchPhaseCount;
            SkewHist[ HeadCurAbsDelta ]++;
            if( DEBUG ) fprintf(stderr,"Done outputing %d phases, Tail now %d Head now %d Cur %d \n",
                                PhasesToWrite, SkewWindowTail, SkewWindowHead, CurrentPktPhase );
            }
          else // new packet is ahead but too far ahead
            {
            fprintf(stderr,"Got packet that is skewed ahead beyond limit - fatal\n");
            exit( -1 );
            }

          }
        }
      }  // end case where head and tail have been realized
    PktPhaseBin[ CurrentPktPhase ].push_back( PktBuf );
    }

  // Dump in order what phases remain buffered as long as they are contig.
  if( SkewWindowTail >= 0 )  // make sure we got something and don't index PktPhaseBin[] with crap.
    {
    for( int p = 0; p < SetMaxSkew; p++ )
      {
      int phase = ( SkewWindowTail + p ) % ArchPhaseCount ;
      if( ! PktPhaseBin[ phase  ].empty() )
        DumpPhaseBin( phase );
      }
    }
  }
