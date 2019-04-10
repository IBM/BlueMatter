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
 #ifndef __FREIGHT_ALIGNER_HPP__
#define __FREIGHT_ALIGNER_HPP__



template< class T_TrueType, int T_AlignmentBits = 5 >
class CacheIsolator
  {
  public:

    typedef T_TrueType TrueType;
    enum { AlignmentBits  = T_AlignmentBits };
    enum { Alignment      = 0x01 << AlignmentBits };
    enum { PaddedSize = (Alignment - 1 + sizeof( T_TrueType ) + Alignment - 1) };

    __attribute__ (( aligned( T_Alignment ) ))
    union
      {
      char mBuf[ PaddedSize ];
      T_TrueType Instance;
      };

    Freight *
    GetFreightPtr()
      {
      Freight * rc = (Freight *) ((int)( mBuf + Alignment - 1 ) & ( ~ (Alignment - 1) ) ) ;
      return( rc );
      }

    template< class rc >
    inline
    void
    Sub( int aIndex, rc &r )
      {
      r = (*(GetFreightPtr()))[ aIndex ] ;
      return;
      }


    #if 0
    inline
    Freight *
    operator& ()
      {
      Freight * rc = GetFreightPtr();
      return( rc );
      }

    inline
    Freight *
    operator->()
      {
      Freight * rc = GetFreightPtr();
      return( rc );
      }
    #endif

  };



#ifdef INC_TEST
#include <stdio.h>

typedef int foo[100];

class foox
  {
  public:
    int x;
    char y;
    double z;

  };

int
main(int argc, char** argv, char**)
  {
  char * x = argv[0];
  int  y = argc;
  FreightAligner<foo> bar;

  int h; bar.xxx(3,h);

  printf("%s: args %d ...  f2.mBuf @ %d (%08X)   f2 @ %d (%08X)\n", x, y, bar.mBuf, bar.mBuf, &bar, &bar );

  return(0);
  }
#endif

#endif
