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
 /************************************************************************
Name:           Alex Rayshubskiy
Program:        InstructionSet.hpp
Date:           06/05/01
Company:        IBM Corp.
************************************************************************/

#ifndef _INSTRUCTIONSET_HPP_
#define _INSTRUCTIONSET_HPP_

#include <BlueMatter/MDVM_IF.hpp>

#define LOAD_SITE     0
#define LOAD_PARAM    1
//////////////////////////////////#define SET_MAP       2
#define EXECUTE       2
#define TERMINATION   3

class Instruction 
{
public:
  
  char instrId;

  int intInfo;  
  short shortInfo;
  char charInfo;
  int* map;

  Instruction() 
    {
      instrId = -1;
      intInfo = -1;
      shortInfo = -1;
      charInfo = -1;
      map = NULL;
    }
  
  Instruction(char iId, int infId)
    {
      instrId    = iId;
      intInfo    = infId;
      shortInfo  = -1;
      charInfo   = -1;
      map = NULL;
    }
  
  ~Instruction() {}
};

//typedef Instruction InstructionStream;

#endif
