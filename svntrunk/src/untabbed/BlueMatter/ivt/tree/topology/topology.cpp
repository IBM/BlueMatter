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
 /*
 * Project:         BlueMatter
 *
 * Module:          IVT
 *
 * Purpose:         Installation Verification Test
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         050314 TJCW Created from design.
 *
 * Installation Verification Test program for BlueMatter execution context
 * intended to verify behaviour on Blue Gene/L hardware
 * 
 */

#define COMPILE_VIRTUAL_NODE_MODE 1

#define RDG_TO_FXLOGGER 1

#include <pk/platform.hpp>
extern "C" {
extern void * PkMain( int, char**, char** );
};


#include <pk/fxlogger.hpp>

#include <builtins.h>
#include <sys/machine/bl_dcrmap.h>
#include <sys/machine/bl_tree.h>

#include <rts.h>
#include <bglpersonality.h>

void *
PkMain (int argc, char** argv, char** envp)
{
  BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() running. "
    << EndLogLine;

  BGLPersonality Personality ;
  rts_get_personality(&Personality, sizeof(Personality)) ;

  unsigned int rdr0  ;
  unsigned int rdr1  ;
  unsigned int rdr2  ;
  unsigned int rdr3  ;
  unsigned int rdr4  ;
  unsigned int rdr5  ;
  unsigned int rdr6  ;
  unsigned int rdr7  ;
  
  rts_read_dcr(DCR_TR_CLASS_RDR0,&rdr0) ;
  rts_read_dcr(DCR_TR_CLASS_RDR1,&rdr1) ;
  rts_read_dcr(DCR_TR_CLASS_RDR2,&rdr2) ;
  rts_read_dcr(DCR_TR_CLASS_RDR3,&rdr3) ;
  rts_read_dcr(DCR_TR_CLASS_RDR4,&rdr4) ;
  rts_read_dcr(DCR_TR_CLASS_RDR5,&rdr5) ;
  rts_read_dcr(DCR_TR_CLASS_RDR6,&rdr6) ;
  rts_read_dcr(DCR_TR_CLASS_RDR7,&rdr7) ;
  
  BegLogLine(1)
    << "rdr0=" << (void *)rdr0 
    << " rdr1=" << (void *)rdr1 
    << " rdr2=" << (void *)rdr2 
    << " rdr3=" << (void *)rdr3 
    << " rdr4=" << (void *)rdr4 
    << " rdr5=" << (void *)rdr5 
    << " rdr6=" << (void *)rdr6 
    << " rdr7=" << (void *)rdr7 
    << EndLogLine ;

  BegLogLine(1) 
    << "trClR[0]=" << (void *) Personality.treeClassRoute[0] 
    << " trClR[1]=" << (void *) Personality.treeClassRoute[1] 
    << " trClR[2]=" << (void *) Personality.treeClassRoute[2] 
    << " trClR[3]=" << (void *) Personality.treeClassRoute[3] 
    << " trClR[4]=" << (void *) Personality.treeClassRoute[4] 
    << " trClR[5]=" << (void *) Personality.treeClassRoute[5] 
    << " trClR[6]=" << (void *) Personality.treeClassRoute[6] 
    << " trClR[7]=" << (void *) Personality.treeClassRoute[7] 
    << EndLogLine ;

  BegLogLine(1)
    << "trI[0]=" << (void *) Personality.treeInfo[0] 
    << " trI[1]=" << (void *) Personality.treeInfo[1] 
    << " trI[2]=" << (void *) Personality.treeInfo[2] 
    << EndLogLine ;
      
  BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() completed. "
    << EndLogLine;
  
  return NULL ;
}
