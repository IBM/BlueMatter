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
 #ifndef __PHASEDPIPELINEUSERIF_HPP__
#define __PHASEDPIPELINEUSERIF_HPP__

//#include <pk/globject.hpp>

// This enum describes the allowed return codes made by
// the pipeline domain to the pipeline protocol driver.
//
typedef int  PPL_ORDINAL;
#define PPL_ORDINAL_UNDEFINED -1
#define PPL_ORDINAL_ERROR     -2
#define PPL_ORDINAL_NULL      -3


typedef enum { PPLUSERIF_RC_Unknown,
               PPLUSERIF_RC_Accept,
               PPLUSERIF_RC_AcceptWithOutputReady,
               PPLUSERIF_RC_AcceptAndFlushRestOfPipeline,
               PPLUSERIF_RC_Error } PPLUSERIF_RC;

#ifndef PKFXLOG_DOMIF
#define PKFXLOG_DOMIF 0
#endif

#ifndef BegLogLine
#define BegLogLine(n) if(n){ cout
#define EndLogLine }
#endif

class FoxBase_DomainPipelineProcess
  {
  public:
    char *mName;  //NEED: wonder who sets this

    class CreateMsgType{};

    PPLUSERIF_RC SetOpContext( unsigned UserBits, GlobjectReference &GloRef )
      {
      BegLogLine( PKFXLOG_DOMIF )
        << mName
        << " SetOpContext() defaulted " << EndLogLine;
      return( PPLUSERIF_RC_Accept );
      }

    PPLUSERIF_RC FinalizeProcessing( )
      {
      BegLogLine( PKFXLOG_DOMIF )
        << mName
        << " FinalizeProcessing() defaulted " << EndLogLine;
      return( PPLUSERIF_RC_Accept );
      }

    PPL_ORDINAL GetOutputItemDest()
      {
      BegLogLine( PKFXLOG_DOMIF )
        << mName
        << " GetOutputItemDest() defaulted " << EndLogLine;
      return( 0 );
      }

    PPLUSERIF_RC ReadyNextOutputItem()
      {
      BegLogLine( PKFXLOG_DOMIF )
        << mName
        << " ReadyOutputItem() defaulted " << EndLogLine;
      return( PPLUSERIF_RC_Accept );
      }

    PPLUSERIF_RC ClearOpContext()
      {
      BegLogLine( PKFXLOG_DOMIF )
        << mName
        << " Clear() defaulted " << EndLogLine;
      return( PPLUSERIF_RC_Accept );
      }

  };

// THIS SHOULD INHERIT FROM A CHANNEL SENDER SET
template<class InputItemType>
class PipeEntranceControl
  {
  public:
    int IncrementPhase()
      {
      }

  };

template<class ItemType>
class PipeExitControl
  {
  };


#endif
