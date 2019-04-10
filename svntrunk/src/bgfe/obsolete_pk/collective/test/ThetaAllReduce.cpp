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
 * Module:          ThetaAllReduce.cpp
 *
 * Purpose:         Test case for theta reduction.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         Sept 20, 2002 BGF Created.
 *
 ***************************************************************************/

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/AtomicOps.hpp>
#include <pk/ThreadCreate.hpp>
#include <pk/ThetaAllReduceBC.hpp>

#include        "../../../../BlueMatter/geometry/include/XYZ.hpp"

typedef XYZ XXYYZZ;

enum   { AllReduceTargetCount = 66000 };
XXYYZZ   AllReduceTarget         [ AllReduceTargetCount ];
XXYYZZ   AllReduceSource         [ AllReduceTargetCount ];
int      AllReduceTargetPartition[ AllReduceTargetCount ];

class AssignOp
  {
  public:

    static
    inline
    int
    IsIdentity( const XXYYZZ aXYZ )
      {
      if ( aXYZ == XXYYZZ::ZERO_VALUE() )
        return( 1 );
      return( 0 );
      }

    static
    inline
    void
    Execute( XXYYZZ &Target, const XXYYZZ &Source )
      {
      Target += Source;
      }
  };

typedef ThetaAllReduceBC< XXYYZZ, AssignOp > ThetaAllReduceBCXXYYZZ;
ThetaAllReduceBCXXYYZZ RedEng;

template< class XXYYZZ, class AssignOp > AtomicInteger ThetaAllReduceBC< XXYYZZ, AssignOp >::ReceivedCount;

void *
UserThread( void * )
  {
  int AddressSpaceCount        = Platform::Topology::GetAddressSpaceCount();
  int MyNode                   = Platform::Topology::GetAddressSpaceId();

  for( int i = 0; i < AllReduceTargetCount; i++ )
    {
    if( MyNode == i / (AllReduceTargetCount / AddressSpaceCount) )
      AllReduceSource[ i ].mX = 1 +  i / (AllReduceTargetCount / AddressSpaceCount) ;
    else
      AllReduceSource[ i ] = XXYYZZ::ZERO_VALUE();
    }

  double sum = 0;
  for( int i = 0; i < AllReduceTargetCount; i++ )
    {
    sum +=  AllReduceSource[ i ].mX ;
    }
  BegLogLine(1)
    << "AllReduce::Do() "
    << " Local sum "
    << sum
    << EndLogLine;

  for( int i = 0; i < 10; i++ )
    {
    BegLogLine(1) << "UserThread starting MPI Do " << i << " on " << Platform::Topology::GetAddressSpaceId() << EndLogLine;

    MPI_Allreduce( AllReduceSource, AllReduceTarget , AllReduceTargetCount * 3 , MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    sum = 0;
    for( int i = 0; i < AllReduceTargetCount; i++ )
      {
      sum +=  AllReduceTarget[ i ].mX ;
      }

    BegLogLine(1) << "UserThread Finished MPI Do " << i << " on " << Platform::Topology::GetAddressSpaceId() << " sum " << sum << EndLogLine;

    }

  for( int i = 0; i < AllReduceTargetCount; i++ )
    {
    AllReduceTarget[ i ] = XXYYZZ::ZERO_VALUE();
    }

  MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.
  sleep(1);

  for( int i = 0; i < 10; i++ )
    {
    BegLogLine(1) << "UserThread starting Do " << i << " on " << Platform::Topology::GetAddressSpaceId() << EndLogLine;

    RedEng.Do( AllReduceTargetCount, AllReduceSource, AllReduceTarget ); //////, AllReduceTargetPartition );

    sum = 0;
    for( int i = 0; i < AllReduceTargetCount; i++ )
      {
      sum +=  AllReduceTarget[ i ].mX ;
      }

    BegLogLine(1) << "UserThread Finished Do " << i << " on " << Platform::Topology::GetAddressSpaceId() << " sum " << sum << EndLogLine;

    for( int i = 0; i < AllReduceTargetCount; i++ )
      {
      assert( AllReduceTarget[ i ].mX == 1 + i / (AllReduceTargetCount / AddressSpaceCount) );
      AllReduceTarget[ i ] = XXYYZZ::ZERO_VALUE();
      }

    }
  return( NULL );
  }


int
PkMain(int argc, char **argv, char **)
  {
  BegLogLine(1)
    << " Platform::Reactor::BROADCAST_PAYLOAD_SIZE "
    << Platform::Reactor::BROADCAST_PAYLOAD_SIZE
    << " sizeof( tReactorPacket ) "
    << sizeof( ThetaAllReduceBCXXYYZZ::tActorPacket )
    << " eMaxElementPayloadCount "
    << ThetaAllReduceBCXXYYZZ::tActorPacket::eMaxElementPayloadCount
    << " sizeof( tElement ) "
    << sizeof( XXYYZZ )
    << EndLogLine;

  int AddressSpaceCount        =  Platform::Topology::GetAddressSpaceCount();
  for( int i = 1; i < AddressSpaceCount; i++ )
    {
    ThreadCreate( i, UserThread, 0, NULL );
    }

  UserThread( NULL );

  return(0);
  }

