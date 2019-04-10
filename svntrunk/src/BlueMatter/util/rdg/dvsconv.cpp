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
 // This program can be used to convert old, text DVS to
// the new binary DVS format.

#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <assert.h>
using namespace std ; 
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/XYZ.hpp>

main(int argc, char **argv )
  {
  assert( argc == 3 );

  cout << "DVS conversion "
    << "Input File : "
    << argv[ 1 ]
    << " Output file "
    << argv[ 2 ]
    << endl;

  ifstream fin( argv[1] );

  int NumPosits;
  fin >> NumPosits;

  XYZ *StartingPositions = new XYZ[ NumPosits ];

  int n;
  for( n = 0; n < NumPosits; n++ )
    {
    fin >> StartingPositions[n].mX >> StartingPositions[n].mY >> StartingPositions[n].mZ;
    }

  XYZ *StartingVelocities = new XYZ[ NumPosits ];

  int i;
  fin >> i;
  assert( i == NumPosits );
  for(  n = 0; n < NumPosits ; n++ )
    {
    fin >> StartingVelocities[n].mX >> StartingVelocities[n].mY >> StartingVelocities[n].mZ;
    }


  int Fd = -1;
  Fd = open( argv[ 2 ], O_WRONLY | O_CREAT | O_TRUNC, 0666 );
  assert( Fd >= 0 );


  for( n = 0; n < NumPosits; n++ )
    {
#if 0
    ED_SiteDynamicVariables ed_pkt;

    ed_pkt.mHeader.mLen   = sizeof( ED_SiteDynamicVariables );
    ed_pkt.mHeader.mClass = ED_CLASS::DynamicVariables;
    ed_pkt.mHeader.mType  = ED_DYNAMIC_VARIABLES_TYPES::Site;

    ed_pkt.mOuterTimeStep                    = 0;

    ed_pkt.mSiteId      = n;
    ed_pkt.mPosition    = StartingPositions[ n ];
    ed_pkt.mVelocity    = StartingVelocities[ n ];

    write( Fd, (void *) (&ed_pkt), sizeof( ED_SiteDynamicVariables ) );
#endif
    char buff[ ED_DynamicVariablesSite::PackedSize ];
    char *p = buff;
    // using 0 for voxel id and 0 for time step
    ED_FillPacket_DynamicVariablesSite( p, 0, 0, n, StartingPositions[ n ], StartingVelocities[ n ] );
    write( Fd, buff, ED_DynamicVariablesSite::PackedSize );
    }

  close( Fd );
  cout << " Done writing " << NumPosits << " pos/vel records " << endl;
  return(0);
  }
