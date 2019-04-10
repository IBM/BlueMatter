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
 * History:         050321 TJCW Created from design.
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

int reduction_vector[64] ALIGN_QUADWORD ;

class TreeReduce
{
public:
  static void Init(unsigned int aChannel, unsigned int aClass) ;
  static void AllReduce(
    double* Target,
    const double* Source,
    unsigned int Length,
    unsigned int Channel
    ) ;
  static double AllReduce1(
    double Source,
    unsigned int Channel
    ) ;
} ;
// Same meaning, but with 'C' interface

extern "C" {
   void tr_AllReduceInit(unsigned int aChannel ,unsigned int aClass) ;
   void tr_AllReduceCharInit(unsigned int aChannel ,unsigned int aClass) ;

   void tr_AllReduce(
    double* Target,
    const double* Source,
    unsigned int Length,
    unsigned int Channel
   ) ;
   double tr_AllReduce1(
    double Source,
    unsigned int Channel
   ) ;

  void tr_AllReduceChar(
    char* InOut,
    unsigned int Length,
    unsigned int Channel
  ) ;

} ;

M


void *
PkMain (int argc, char** argv, char** envp)
{
  BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() running. "
    << EndLogLine;

	BGLPersonality Personality ;
	rts_get_personality(&Personality, sizeof(Personality)) ;

  
  BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() completed. "
    << EndLogLine;
	
  return NULL ;
}
