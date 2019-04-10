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
 * Module:          Physical Topology
 *
 * Purpose:         Contains structure holding routing tables.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         032900 BGF Created.
 *
 ***************************************************************************/
#ifndef _PHYSICALTOPOLOGY_HPP_
#define _PHYSICALTOPOLOGY_HPP_

#include <pk/mpi.hpp>


// this class contains the physical topology.
// for udp, it would be ip address, port
// for mpi, it is just taskno
// since there is only one physical topology, everything can and is static.

class PhysicalTopology
  {
  public:
    class Route
      {
      public:
        int TaskNo;
      };

    static int
    AddressSpaceCount()
      {
      return( MpiSubSys::GetTaskCnt() );
      }

    static int
    AddressSpaceSelf()
      {
      return( MpiSubSys::GetTaskNo() );
      }

    static Route
    RouteToAddressSpace( int aAddressSpaceId )
      {
      Route rc;
      rc.TaskNo = aAddressSpaceId;
      return( rc );
      }
  };




#endif
