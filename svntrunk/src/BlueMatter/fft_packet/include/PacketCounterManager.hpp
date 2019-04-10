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
 #ifndef __PACKET_COUNTER_DEBUGGER__
#define __PACKET_COUNTER_DEBUGGER__

#include <sys/mman.h>
#include <sys/types.h>
//#include <iomanip.h>
//#include <stdio.h>
//#include <string.h>
#include <sys/signal.h>
//#include <assert.h>
//#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#ifndef DIRECT_BLNIE_BUILD
#include <BlueMatter/ImportExportFence.hpp>
#else
#include "ImportExportFence.hpp"
#endif

class PacketCounterManager 
{
public:
  
  struct CounterRecord 
  {
    int sendCount;
    int recvCount;    
    int lastLineNumber;
    int barrierCount;
  };

  CounterRecord* mCounts;
  
  int mCounterCount;
  int mFD;   
  //  int mLockVar;
  int mLockSendVar;
  int mLockRecvVar;
  int mLockBarrVar;

  
  char mMappedFileName[ 64 ];

  PacketCounterManager( int count, int reset, char* filename );
  
  void TickSend( int aRank );
  void TickRecv( int aRank );
  void TickBarrier( int aRank, int aLineNumber );
  void DumpState();
  void Reset();
};

PacketCounterManager::PacketCounterManager( int aCounterCount, int reset, char* filename )
{  
  mCounterCount = aCounterCount;
  
#ifndef MANAGER_OFF 
  sprintf(mMappedFileName,"%s", filename);
 
  mFD = open( mMappedFileName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
  
  if( mFD < 0 )
    printf("PacketCounterManager::PacketCounterManager()::ERROR:: opening /tmp/counter_file" );
 


  mCounts = (CounterRecord *) mmap( 0, 
                                    sizeof( CounterRecord ) * mCounterCount,
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED, 
                                    mFD, 
                                    0 );

  ftruncate(mFD, sizeof( CounterRecord ) * mCounterCount );

  assert( mCounts != NULL ); 
  assert( mFD > 0 );

  if( reset == 1 )
    Reset();

  mLockSendVar = 0;
  mLockRecvVar = 0;
  mLockBarrVar = 0;
#endif
}

void PacketCounterManager::TickSend( int aRank )
{
#ifndef MANAGER_OFF 
  //  Mutex::YieldLock( mLockSendVar );

  // assert( aRank >= 0 && aRank < mCounterCount );
  //printf("TickSend(%d)=%d\n", aRank, mCounts[ aRank ].sendCount);
  AtomicAdd( mCounts[ aRank ].sendCount, 1 );
  //printf("TickSend(%d)=%d\n", aRank, mCounts[ aRank ].sendCount);
  //mCounts[ aRank ].sendCount++;

  //Mutex::Unlock( mLockSendVar );
#endif
}

void PacketCounterManager::TickRecv( int aRank )
{
#ifndef MANAGER_OFF 
  //  Mutex::YieldLock( mLockRecvVar );

  // assert( aRank >= 0 && aRank < mCounterCount );
  AtomicAdd( mCounts[ aRank ].recvCount, 1 );
  //mCounts[ aRank ].recvCount++;

  // Mutex::Unlock( mLockRecvVar );
#endif
}

void PacketCounterManager::TickBarrier( int aRank, int aLineNumber )
{
#ifndef MANAGER_OFF 
  //  Mutex::YieldLock( mLockBarrVar );

  //  assert( aRank >= 0 && aRank < mCounterCount );
  AtomicAdd( mCounts[ aRank ].barrierCount, 1 );
  // mCounts[ aRank ].barrierCount++;

  // mCounts[ aRank ].lastLineNumber = aLineNumber;

  //Mutex::Unlock( mLockBarrVar );
#endif
}

void PacketCounterManager::DumpState()
{
  printf( "Mapped filename: %s\n", mMappedFileName );
  printf( "Counters Address: %x\n",mCounts );
  printf( "Nodes: %d\n", mCounterCount );
  printf( "Rank\t\tSent\t\tRecv\t\tBarr\t\tLine\n" );
  printf( "----\t\t----\t\t----\t\t----\t\t----\n" );

  for( int i=0; i < mCounterCount; i++ )
    {
      printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n", 
             i, 
             mCounts[ i ].sendCount, 
             mCounts[ i ].recvCount, 
             mCounts[ i ].barrierCount, 
             mCounts[ i ].lastLineNumber 
             );
    }
}

void PacketCounterManager::Reset()
{
#ifndef MANAGER_OFF 
  for(int i=0; i < mCounterCount; i++)
    {
      mCounts[ i ].sendCount = 0;
      mCounts[ i ].recvCount = 0;
      mCounts[ i ].lastLineNumber = 0;
      mCounts[ i ].barrierCount = 0;
    }
#endif
}

#endif
