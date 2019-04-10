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
 #ifndef INCLUDE_THING2HEX_HPP
#define INCLUDE_THING2HEX_HPP

const char hex2digit[16] = { '0',
                             '1',
                             '2',
                             '3',
                             '4',
                             '5',
                             '6',
                             '7',
                             '8',
                             '9',
                             'a',
                             'b',
                             'c',
                             'd',
                             'e',
                             'f' };
                              
// returns length of string, sets target to hex representation of aThing
template <class ThingType>
int thing2Hex( const ThingType& aThing, char* target )
{
  // I'm assuming big-endian for now
  const unsigned char* asChar = 
    reinterpret_cast< const unsigned char* >( &aThing );
  for ( int i = 0; i < 2*sizeof( ThingType ); ++i )
    {
      unsigned char nibble = ( i % 2 == 0 ) ? asChar[ i/2 ] >> 4 : asChar[ i/2 ];
      nibble = nibble & 0x0F;
      target[ i ] = hex2digit[ nibble ];
    }
  return( 2*sizeof( ThingType ) );
}
#endif
