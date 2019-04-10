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
 #include <spi/ThrobbinHood.hpp>

int Group::CompareI (const void * a, const void * b)
    {
    if( sizeof( TorusXYZ ) == 1 )
      {
      char ca = *((char *) a );
      char cb = *((char *) b );
      return( ca - cb );
      }
    else if( sizeof( TorusXYZ ) == 2 )
      {
      short ca = *((short *) a );
      short cb = *((short *) b );
      return( ca - cb );
      }
    else if( sizeof( TorusXYZ ) == 3 )
      {
      // Serious questions about whether this works...
      int ca = *((int *) a );
      int cb = *((int *) b );
      ca |= 0x000000FF;
      cb |= 0x000000FF;
      return( ca - cb );
      }
    else if( sizeof( TorusXYZ ) == 4 )
      {
      int ca = *((int *) a );
      int cb = *((int *) b );
      return( ca - cb );
      }

    assert( 0 );
    return( 0 );
    }

Group GroupDummy;
Group GroupDummy1( 1 );

int GroupDummyCompilerTricker()
  {
  TorusXYZ moo;
  moo.Set( 0,0,0 );
  GroupDummy.Add( moo );
  GroupDummy.Reset( 1 );
  GroupDummy.Double();
  GroupDummy.Count();
  GroupDummy.Sort();
  GroupDummy.Search( moo );
  TorusXYZ moo1 = GroupDummy[0];
  Group *GroupDummy2 = new Group;
  if( GroupDummy2 != NULL )
    delete GroupDummy2;
  }

void Group::Reset(int aAllocCount)
  {
  mSorted      = 1;
  mCount       = 0;
  if( mAllocCount )
    {
    assert( mArray );
    delete [] mArray;
    mArray = NULL;
    }
  mAllocCount  = aAllocCount;
  if( mAllocCount == 0 )
    mAllocCount = 1;
  mArray       = new TorusXYZ[ mAllocCount ];
  assert( mArray );
  }

Group::Group( int aAllocCount )
  {
  mAllocCount = 0;
  Reset( 1 );
  }

Group::~Group()
  {
  if( mArray )
    delete [] mArray;
  mArray = NULL;
  }

void Group::Double()
  {
  assert( mArray );
  TorusXYZ * OldArray = mArray;
  mAllocCount *= 2;
  mArray       = new TorusXYZ[ mAllocCount ];
  assert( mArray );
  memcpy( mArray, OldArray, mCount * sizeof( TorusXYZ ) );
  delete [] OldArray;
  }

int Group::Count()
  {
  int rc = mCount;
  return( rc );
  }

void  Group::Add( TorusXYZ aElement )
  {
  if( Search( aElement ) )
    return;
  assert( mCount + 1 <= mAllocCount );
  if ( mCount + 1 == mAllocCount )
    Double();
  mArray[ mCount ] = aElement;
  mCount++;
  mSorted = 0;
  }

void Group::Sort()
  {
  // CompareI won't work if TorusXYZ is of diff size than int
  Platform::Algorithm::Qsort( mArray, mCount, sizeof( TorusXYZ ), CompareI );
  }

int Group::Search( TorusXYZ aKey )
  {
  if( ! mSorted )
    Sort();

  void * rc = bsearch( (void *)(&aKey), (void *) mArray, mCount, sizeof( TorusXYZ ), CompareI );

  return( rc != NULL ? 1 : 0 );
  }

TorusXYZ Group::operator[]( const int aIndex ) const
  {
  assert( aIndex >= 0 && aIndex < mCount );
  TorusXYZ rc = mArray[ aIndex ];
  return( rc );
  }


PktThrobberIF::PktThrobberIF()
  {
  mThrobber = (void *)new ThrobbinHood();
  }

void PktThrobberIF::BroadcastExecute( int aSimTick, int aAssignmentViolation )
  {
  ((ThrobbinHood * )mThrobber)->BroadcastExecute();
  }

int PktThrobberIF::DoesCulling()
  {
  return 0;
  }

void PktThrobberIF::dcache_store()
  {
  ((ThrobbinHood * )mThrobber)->dcache_store();
  }

XYZ* PktThrobberIF::GetFragPositPtrInHood( int aFragId )
  {
  return ( ((ThrobbinHood * )mThrobber)->GetFragPositPtr( aFragId ) );
  }

XYZ* PktThrobberIF::GetFragPositPtrLocal( int aFragId )
  {
  return ( ((ThrobbinHood * )mThrobber)->GetFragPositPtr( aFragId ) );
  }

XYZ* PktThrobberIF::GetFragForcePtr( int aFragId )
  {
  return ( ((ThrobbinHood * )mThrobber)->GetFragForcePtr( aFragId ) );
  }

int PktThrobberIF::GetFragIdInHood( int aFragIter )
  {
  return( ((ThrobbinHood * )mThrobber)->GetFragIdInHood( aFragIter ) );
  }

void PktThrobberIF::SetGlobalFragInfo( int aFragId, int aSiteCount )
  {
  ((ThrobbinHood * )mThrobber)->SetGlobalFragInfo( aFragId, aSiteCount );
  }

void PktThrobberIF::SetFragAsLocal( int aFragId )
  {
  ((ThrobbinHood * )mThrobber)->SetFragAsLocal( aFragId );
  }

void PktThrobberIF::ClearLocalFrags()
  {
  ((ThrobbinHood * )mThrobber)->ClearLocalFrags();
  }

int PktThrobberIF::GetFragsInHoodCount()
  {
  return( ((ThrobbinHood * )mThrobber)->GetFragsInHoodCount() );
  }

void PktThrobberIF::ReductionExecute( int aSimTick )
  {
  ((ThrobbinHood * )mThrobber)->ReductionExecute();
  }

int
PktThrobberIF::WillOtherNodeBroadcastToThisNode( int p )
  {
  int rc = ((ThrobbinHood * )mThrobber)->WillOtherNodeBroadcastToThisNode( p );
  return( rc );
  }

void 
PktThrobberIF::Pack()
  {
  // Not much to say here yet.
  return;
  }

void PktThrobberIF::Init( int  aDimX, int aDimY, int aDimZ,
                          int* aNodesToSendTo,
                          int  aNodesToSendToCount,
                          int  aFragmentsInPartition,
                          int  aMaxLocalFragments,
                          int  aMaxFragmentsInNeighborhood )
    {
    ((ThrobbinHood * )mThrobber)->Init( aDimX, aDimY, aDimZ, 
                                        aNodesToSendTo, aNodesToSendToCount,
                                        aFragmentsInPartition, aMaxFragmentsInNeighborhood );
    }
