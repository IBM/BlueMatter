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
 
#ifndef _INCLUDED_PROBSPEC__
#define _INCLUDED_PROBSPEC__

//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~
//  ~~~  Main Module for PSF or XML to HPP or XML processing
//  ~~~  in BlueMatter. Functions defined within cpp simplify
//  ~~~  MolSys preparation. ProbSpecMain provides a concise
//  ~~~  example of how to use the MolSys class methods.
//  ~~~
//  ~~~ Author: MCP   
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <assert.h>
#include <stdio.h>

#include "molsys.hpp"


#define PSF_WITH_DEFAULT_FRAG_SCHEME 1
#define PSF_TO_XML 2
#define PSF_WITH_FRAG_SCHEME 3
#define XML_WITH_DEFAULT_FRAG_SCHEME 4
#define XML_READ_TEST 5
#define XML_WITH_FRAG_SCHEME 6
#define UNKNOWN 0


// *************************************************************
// *************************************************************
//   Prototypes
//

int  ProbSpecMain(int argc, char **argv);
int  PrepareMolsFromPSFFile(MolecularSystem *ms,char *psffilename,int opcode, char *fname);
int  ParseCharmmParameters(MolecularSystem *ms,char *topfilname,char *paramfilename);
int  PrepareMolecularSystemFromXML(MolecularSystem *the_MolecularSystem, int opcode,char *fragfilename);
void PrintUsageOptions();
int  PrepareMolecularSystem(MolecularSystem* ms,int argc,char **argv);
int DetermineProcessingOperation(int argc, char **argv);
#endif








