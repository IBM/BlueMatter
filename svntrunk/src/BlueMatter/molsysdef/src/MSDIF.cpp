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
 // This contains the access methods for a molecular system definition

THIS IS DEAD CODE

#include <pk/platform.hpp>
#include <BlueMatter/MolSysDef.hpp>
#include <BlueMatter/MSDIF.hpp>

MolSysDef * MSDIF_BASE::SingletonPtr;
MolSysDef   MSDIF_BASE::FlatHeader;

int
MSDIF_BASE::LoadMsdFile( char * MsdFileName )
{
  int fd = open( MsdFileName, O_RDONLY );
  assert( fd > 0 );  // Failed to open MolSysDef file

  int HeaderBytesRead = Platform::FileIO::Read( fd, (char*)(&FlatHeader) , sizeof( MolSysDef ) );
  assert( HeaderBytesRead == sizeof( MolSysDef ) );

  int TotalByteSize = FlatHeader.TotalByteSize ;

  //BGF allocate as double - this is supposed to get good alinement. Is it enough?  Does it matter?
  SingletonPtr = (MolSysDef *) new double[ ( TotalByteSize + sizeof(double) - 1 ) / 8 ];
  assert( SingletonPtr != NULL );

  lseek( fd, 0, SEEK_SET );

  int BytesRead = Platform::FileIO::Read( fd, SingletonPtr, TotalByteSize );
  assert( BytesRead == TotalByteSize );

  close( fd );

  BegLogLine(1)
    << "MSDIF_BASE() read file >"
    << MsdFileName
    << "< for "
    << BytesRead
    << " BytesRead "
    << EndLogLine;

  Access().ConvertOffsetsToPointers();

  // Copy the corrected values back into the static header.
  memcpy( &FlatHeader, SingletonPtr, sizeof( MolSysDef ) );

  return( 0 );
}
