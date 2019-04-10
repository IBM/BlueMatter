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

#include <pk/platform.hpp>
#include <BlueMatter/MSD_IF.hpp>

MSD*      MSD_IF::SingletonPtr;
//MSD       MSD_IF::FlatHeader;
SiteInfo* MSD_IF::expandedSiteInfoTable = NULL;
int       MSD_IF::udfCodeToIndexMap[ UDF_Binding::UDF_MAX_CODE + 1 ];

MSD_IF::UdfInUse MSD_IF::UdfsInUse[ UDF_Binding::UDF_MAX_CODE + 1 ];

int*    MSD_IF::nsqIndexToNsq1IndexMap = NULL;
void**  MSD_IF::nsq1Tables = NULL;

typedef union
  {
    UDF_Binding::DefaultForce_Params *           mDefaultForce_Params ;
    UDF_Binding::StdHarmonicBondForce_Params *   mStdHarmonicBondForce_Params ;
    UDF_Binding::StdHarmonicAngleForce_Params *  mStdHarmonicAngleForce_Params ;
    UDF_Binding::SwopeTorsionForce_Params *      mSwopeTorsionForce_Params ;
    UDF_Binding::UreyBradleyForce_Params *       mUreyBradleyForce_Params ;
    UDF_Binding::OPLSTorsionForce_Params *       mOPLSTorsionForce_Params ;
    UDF_Binding::OPLSImproperForce_Params *      mOPLSImproperForce_Params ;
    UDF_Binding::ImproperDihedralForce_Params *  mImproperDihedralForce_Params ;
  } ParamUnion;

    // For debugging
ParamUnion UdfParams[ UDF_Binding::UDF_MAX_CODE + 1 ];


int
AssignMSDPtr() 
  {
      MSD_IF::SetMsd( &MolecularSystemDef );
  }

#if 0
int
MSD_IF::LoadMsdFile( char * MsdFileName )
  {
    int fd = open( MsdFileName, O_RDONLY );
  assert( fd > 0 );  // Failed to open MolSysDef file

  int HeaderBytesRead = Platform::FileIO::Read( fd, (char*)(&FlatHeader) , sizeof( MSD ) );
  assert( HeaderBytesRead == sizeof( MSD ) );

  int TotalByteSize = FlatHeader.TotalByteSize ;

  //BGF allocate as double - this is supposed to get good alinement. Is it enough?  Does it matter?
  SingletonPtr = (MSD *) new double[ ( TotalByteSize + sizeof(double) - 1 ) / 8 ];
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

  //  memcpy( (void *) UdfParams, SingletonPtr->ParamArray, SingletonPtr->ParamArraySize * sizeof(void *) );

  // Copy the corrected values back into the static header.
  memcpy( &FlatHeader, SingletonPtr, sizeof( MSD ) );

  //  memcpy( (void *) UdfParams, FlatHeader.ParamArray, FlatHeader.ParamArraySize * sizeof(void *) );

  return( 0 );
  }
#endif
