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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          pkrnew.C
 *
 * Purpose:         Unit test:  remote object "new"ing.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010297 BGF Created.
 *
 ***************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

#define INCLUDE_STATIC_DEFINES

#include "../foxpk/pkrnew.hpp"

class Str
  {
  public:
    char String[81];
    Str()
      {
      String[0] = 0;
      }
    Str( char *S )
      {
      assert( strlen(S) < 80 );
      strcpy( String, S );
      printf("Str() S >%s<\n", S);
      fflush( stdout );
      }
  };


class RemObj
  {
  public:
    RemObj( Str *aStr)
      {
      printf("RemObj(): aStr >%s<\n", aStr->String );
      fflush(stdout);
      }
  };

#include "../foxpk/mpi.hpp"
#include "../foxpk/pkactor.hpp"
#include "../foxpk/pkdebug.hpp"

main(int argc, char **argv, char **envp)
  {
  MpiSubSys::Init( argc, argv );
  PkActiveMsgSubSys::Init();

  int start_debug();
  start_debug();

  for(;;)
    {
    RemoteObjectOf<RemObj,Str> RemObj;
    RemObj.Instantiate( (MpiSubSys::TaskNo + 1) % MpiSubSys::TaskCnt, new Str("Here I am.") );

    }

  }

