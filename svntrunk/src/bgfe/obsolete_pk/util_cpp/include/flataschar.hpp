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
 * Project:         FoxFm: Feature Oriented eXecutive for Fuzzy Matching.
 *
 * Module:          flataschar.hpp
 *
 * Purpose:         A templatized way to assure structures are flat
 *                  and thus can be used in unions.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         101197 BGF Created from foxfm.hpp
 *
 ***************************************************************************/

#ifndef __FLATASCHAR_HPP__
#define __FLATASCHAR_HPP__

// The following class encapsulates the casting of
// domain flat types to char[*} types.  This is
// required to place these flat structures into
// unions representing the pipeline message buffers.
// This is allowed because we have already stipulated
// to the domain that his flat types MUST be not just
// flat, but memcpy()able as well.
// NOTE: DO NOT GIVE THIS CLASS A CONSTRUCTOR OR DESTRUCTOR!!!!

template<class ItemType>
class FlatAsChar
  {
  // NOTE: DO NOT GIVE THIS CLASS A CONSTRUCTOR OR DESTRUCTOR!!!!
  public:
    typedef  ItemType TrueType;

    typedef char BufferType[ sizeof( ItemType ) ];

    typedef union
    {
       double Dummy ;
       BufferType Buffer ;
    } DoubleAlignedType ;

    DoubleAlignedType AlignedBuffer;


    int SizeOf()
      {
      return( sizeof( AlignedBuffer) );
      }

    void
    Flatten( const ItemType &Item )
      {
      memcpy( AlignedBuffer.Buffer, &Item, sizeof( ItemType ) );
      }

    void
    Flatten( ItemType *Item )
      {
      memcpy( AlignedBuffer.Buffer, Item, sizeof( ItemType ) );
      }

    char AsChar(int Index)
      {
      return( AlignedBuffer.Buffer[ Index ] );
      }

    BufferType &AsChar()
      {
      return( AlignedBuffer.Buffer );
      }

    ItemType&
    AsTyped()
      {
return
#if defined(LINUX) || defined(CYCLOPS)  // LINUX GCC doesn't like the later... not sure if this is the same though.
       *((ItemType *) AlignedBuffer.Buffer)  //LINUX GCC this seems to have worked.
#else
       (ItemType &) AlignedBuffer.Buffer    //LINUX GCC seems to try to call constructor for ItemType
#endif
        ;
      }

    // return the first four bytes as an unsigned - probably to print out
    unsigned
    AsUnsigned()
      {
      return( *((unsigned *)AlignedBuffer.Buffer) );
      }

    ItemType *
    operator-> ()
      {
      // Dissallow framework from looking into object.
      assert(0);
      return(NULL);
      }
  };

#endif
