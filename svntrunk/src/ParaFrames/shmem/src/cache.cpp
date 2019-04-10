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
 
#include <stdio.h>

template< class T_TrueType, int T_AlignmentBits = 5 >
class ShmemBuffer
  {
  public:

    typedef T_TrueType TrueType;
    enum { AlignmentBits    = T_AlignmentBits };
    enum { Alignment        = 0x01 << AlignmentBits };
    enum { TotalCacheLines  = ( (sizeof( TrueType ) + Alignment - 1) / Alignment) };
    enum { TotalBytes       = TotalCacheLines * Alignment };
    enum { TotalLongDoubles = TotalBytes / sizeof( long double ) };
    ///// enum { TotalFootPrint = sizeof( TrueType ) + (Alignment - ((sizeof( T_TrueType ) + (Alignment - 1)) % Alignment) ) };

    __attribute__ (( aligned( T_Alignment ) ))
    union
      {
      //char mBuf[ TotalFootPrint ];
      long double Buf[ TotalLongDoubles ];
      T_TrueType mInstance;
      };

    inline
    T_TrueType &
    GetPayloadRef()
     {
     return( mInstance );
     }

    inline
    T_TrueType *
    GetPayloadPtr()
     {
     return( & mInstance );
     }

  };


template < class TYPE, int T_AlignmentBits = 5 >
class ShmemAtomicNative
  {
  public:
    enum { Alignment        = 0x01 << T_AlignmentBits };
    __attribute__ (( aligned( Alignment ) ))
    TYPE mVar;
    char mPadding[ Alignment - sizeof( TYPE ) ];

    inline
    TYPE
    operator ++ ( int )
      {
      mVar++;
      return( mVar );
      }


    inline
    TYPE &
    operator & ()
      {
      return( mVar );
      }

  };


typedef ShmemAtomicNative< int > ShmemCounter;


typedef char foo[23];

typedef ShmemBuffer<foo> foobuf;

int main( int argc, char** argv, char **envp)
  {
  ShmemCounter x;
  x++;

  enum{ BufCnt = 5 };
  foobuf fbarray[BufCnt];
  for( int i = 0; i < BufCnt; i++ )
    {
    foo & f = fbarray[i].GetPayloadRef();
    printf(" @f %08X  @foobuf[ %d ] %08X\n", &f, i, &(fbarray[i].GetPayloadRef()) );
    }
  return(0);
  }



