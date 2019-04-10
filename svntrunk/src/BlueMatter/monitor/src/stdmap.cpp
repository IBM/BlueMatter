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
#include <iostream>
#include <map>

class IpAddrPort
  {
  public:

    IpAddrPort()
      {
      mAddr = 0xFFFFFFFF;
      mPort = 0xFFFFFFFF;
      }

    IpAddrPort( unsigned aAddr, unsigned aPort )
      {
      mAddr = aAddr;
      mPort = aPort;
      }

    void
    Init( unsigned aAddr, unsigned aPort )
      {
      mAddr = aAddr;
      mPort = aPort;
      }

    unsigned mAddr;
    unsigned mPort;

  };


class ConnectionState
  {
  public:
    long long mSeqNo;
    long long mAckSeqNo;
    ConnectionState( long long aSeqNo = 0, long long aAckSeqNo = 0 )
      {
      mSeqNo    = aSeqNo;
      mAckSeqNo = aAckSeqNo;
      }

  };

struct LessIpAddrPort
  {
  bool
  operator()(const IpAddrPort iapA, const IpAddrPort iapB ) const
    {
    int rc = 0;
    if( iapA.mAddr < iapB.mAddr )
      rc = 1;
    else if( (iapA.mAddr == iapB.mAddr) && (iapA.mPort < iapB.mPort) )
      rc = 1;
    return( rc );
    }
  };


std::map < IpAddrPort, ConnectionState, LessIpAddrPort> IpAddrPortToConIdMap;

main()
  {
  IpAddrPort work;

  work.Init( 2, 97 );
  IpAddrPortToConIdMap[ work ] = ConnectionState(0);
  work.Init( 3, 98 );
  IpAddrPortToConIdMap[ work ] = ConnectionState(11);
  work.Init( 4, 99 );
  IpAddrPortToConIdMap[ work ] = ConnectionState(2);


  work.Init( 3, 98 );
  //////  std::map<IpAddrPort, int, LessIpAddrPort>::iterator cur = IpAddrPortToConIdMap.find( work );

  printf("value is %lld\n", IpAddrPortToConIdMap[ work ].mSeqNo );

  IpAddrPortToConIdMap[ work ].mSeqNo = 89;
  ConnectionState & iap = IpAddrPortToConIdMap[ work ];
  iap.mSeqNo = 99;

  printf("value is %lld\n", IpAddrPortToConIdMap[ work ].mSeqNo );


  std::map<IpAddrPort, ConnectionState, LessIpAddrPort>::iterator cur = IpAddrPortToConIdMap.find( work );
  if( cur == IpAddrPortToConIdMap.end() )
    printf(" didn't find one that we should have \n");

  cur = IpAddrPortToConIdMap.find( IpAddrPort(23,59) );
  if( cur != IpAddrPortToConIdMap.end() )
    printf(" found one we shouldn't have \n");


  }
