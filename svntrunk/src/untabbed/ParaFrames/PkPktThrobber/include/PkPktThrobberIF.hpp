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
 #ifndef __THROBBINHOODIF_HPP__
#define __THROBBINHOODIF_HPP__

// #include <spi/TorusXYZ.hpp>

#include <Pk/XYZ.hpp>
typedef cXYZ TorusXYZ ;

static void
GetCoords( int aTaskId, int *aX, int *aY, int *aZ )
  {
    TorusXYZ target = PkTopo3DMakeXYZ(aTaskId) ;
    *aX=target.mX ;
    *aY=target.mY ;
    *aZ=target.mZ ;
  // Platform::Topology::GetCoordsFromRank( aTaskId, aX, aY, aZ );
  return;
  }

static TorusXYZ
GetTorusXYZ( int aTaskId )
  {
//  int x, y, z;
//  GetCoords( aTaskId, &x, &y, &z );
//  TorusXYZ rc;
//  rc.Set( x, y, z );
//  return( rc );
    return PkTopo3DMakeXYZ(aTaskId) ;
  }

struct Group
  {
  int mSorted;
  int mAllocCount;
  int mCount;
  TorusXYZ *mArray;

  public:

  static int CompareI (const void * a, const void * b);

  void Reset(int aAllocCount = 1);

  Group( int aAllocCount = 1 );

  ~Group();

  void Double();

  int Count();

  void Add( TorusXYZ aElement );

  void Sort();

  int Search( TorusXYZ aKey );

  TorusXYZ
  operator[]( const int aIndex ) const;

  };

class PktThrobberIF  // the hood throbber -- posits bcast followed by a force reduce.
  {
public:
  double padding0[4];
  void * mThrobber;
  double padding1[4];

  void dcache_store();
  int WillOtherNodeBroadcastToThisNode( int p );

  PktThrobberIF();

  int DoesCulling();     

  void ReductionExecute( int aSimTick );
  void BroadcastExecute( int aSimTick, int aAssignmentViolation );

  int GetFragsInHoodCount();
  int GetFragsInHood( int aFragOrd );

      // XYZ* GetFragPositPtr( int aFragId );
  XYZ* GetFragPositPtrLocal( int aFragId );
  XYZ* GetFragPositPtrInHood( int aFragId );

  XYZ* GetFragForcePtr( int aFragId );
  int GetFragIdInHood( int aFragIter );
  void SetGlobalFragInfo( int aFragId, int aSiteCount );
  void SetFragAsLocal( int aFragId );
  void ClearLocalFrags();
  void Pack();

  void
  Init( int  aDimX, int aDimY, int aDimZ,
        int* aNodesToSendTo,
        int  aNodesToSendToCount,
        int  aFragmentsInPartition,
        int  aMaxLocalFragments,
        int  aMaxFragmentsInNeighborhood = -1 );
  }; // end of class HoodThrobber

#endif
