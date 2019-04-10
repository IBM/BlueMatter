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
 #ifndef __PHASEDPIPELINESYSTEM_HPP
#define __PHASEDPIPELINESYSTEM_HPP

//#include "foxproto.hpp"
#include "PPL_UserIF.hpp"
#include "PPL_ProtocolDriver.hpp"
#include "pkqueue_set.hpp"

#include "flataschar.hpp"

typedef  unsigned PPL_Phase;

#define HEADER_PHASE_BITS 10

#define MaxPhases ((1<<HEADER_PHASE_BITS)-1)

// This needs to be typedef'd outside the PhasedPipelineItem
// structure so that it can be assigned from one Item to the
// next when they are templated with different UserItemTypes.
typedef struct T_Header
  {
  unsigned  FirstFlag :  1 ;
  unsigned  FlushFlag :  1 ;
  unsigned  Phase     : HEADER_PHASE_BITS ;
  short     UserBits;
  } PhasedPipelineItemHeader;

template<class UserItemType>
class PhasedPipelineItem
  {
  public:
    // Header really ought to be kept at a single word.
    PhasedPipelineItemHeader Header;
    typedef UserItemType     PayloadType;
    union
      {
      struct
        {
        GlobjectReference        GloRef;
        } First;
      struct
        {
        FlatAsChar<UserItemType> PayloadItem;
        }  Middle;
      struct
        {
        } Last;
      } FMLSeq;
  };

#endif
