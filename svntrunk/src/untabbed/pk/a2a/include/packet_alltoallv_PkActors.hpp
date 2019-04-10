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
 #ifndef PACKET_ALLTOALLV_PKACTORS_HPP_
#define PACKET_ALLTOALLV_PKACTORS_HPP_
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <cstdlib>
#include <rts.h>

#include <Pk/API.hpp>
#include <BonB/blade_on_blrts.h>

// #include <spi/TorusXYZ.hpp>

#include <spi/tree_reduce.hpp>
//#include <pk/coprocessor_on_coprocessor.hpp>

#define SEND_DONE   ( -1 )
#define PA_MEMORY_ALIGN(NUMBITS) __attribute__((aligned(0x01<<NUMBITS)))

#define MAX_PACKET_INJECT 2
#define NUMBER_OF_LINKS   6

//extern "C"
//{
//int PMI_co_start( void*(*fp)(void *), void* arg );
//void* PMI_co_join( int handle );
//};

//struct FifoHelper
//  {
//  _BGL_TorusFifo * mLink;
//  unsigned char    mStat;
//  };
//

//struct SGE_Actors
//{
//int mRank;
//TorusXYZ mCoord;
//} PA_MEMORY_ALIGN( 5 );
//
//#define GDIM 3
//class GroupContainer_Actors
//{
//public:
//  double padding0[4];
//    int   mMyRank ;
//    int   mSize ;
//    SGE_Actors*  mElements ;
//    int   mPlaneMask[ GDIM ] PA_MEMORY_ALIGN( 5 );
//    int   mDim ;
//    int   mDimSize[ GDIM ] PA_MEMORY_ALIGN( 5 );
//    int   mMyCoord[ GDIM ] PA_MEMORY_ALIGN( 5 );
//    int * mBGLRankToGroupRankMap PA_MEMORY_ALIGN( 5 );
//    int   mMachineSize ;
//  double padding1[4];
//
//    void DumpState(int i)
//      {
//      printf("DumpState i: %d mMyRank: %d\n", i, mMyRank );
////       printf("mSize: %d\n", mSize );
////       printf("mPlaneMask: { %d %d %d }\n", mPlaneMask[0], mPlaneMask[1], mPlaneMask[2] );
////       printf("mDimSize: { %d %d %d }\n", mDimSize[0], mDimSize[1], mDimSize[2] );
////       for( int i=0; i<mSize; i++ )
////         {
////         printf("mElements[ %d ].mRank= %d mElements[ %d ].mCoord= { %d %d %d }\n",
////                i, mElements[ i ].mRank,
////                i, mElements[ i ].mCoord.mX, mElements[ i ].mCoord.mY, mElements[ i ].mCoord.mZ );
////         }
//      }
//
//    void
//    Init( int aPlaneMask[ GDIM ], int aDimX=1, int aDimY=1, int aDimZ=1 )
//      {
//      int MyCoordX, MyCoordY, MyCoordZ;
//      BGLPartitionGetCoords( &MyCoordX, &MyCoordY, &MyCoordZ );
//
//      mMyCoord[ 0 ] = MyCoordX;
//      mMyCoord[ 1 ] = MyCoordY;
//      mMyCoord[ 2 ] = MyCoordZ;
//
//      int MachineDimX, MachineDimY, MachineDimZ;
//      BGLPartitionGetDimensions( &MachineDimX, &MachineDimY, &MachineDimZ );
//
//      mMachineSize = MachineDimX * MachineDimY * MachineDimZ;
//
//      mBGLRankToGroupRankMap = (int *) PkHeapAllocate( sizeof( int ) * mMachineSize );
//      assert( mBGLRankToGroupRankMap );
//
//      for( int i=0; i < mMachineSize; i++ )
//        {
//        mBGLRankToGroupRankMap[ i ] = -1;
//        }
//
//      memcpy(mPlaneMask, aPlaneMask, sizeof(int) * GDIM );
//
//      // index = (DimY*x + y)*DimZ + z
//
//      mDim = mPlaneMask[ 0 ] + mPlaneMask[ 1 ] + mPlaneMask[ 2 ];
//
//      mDimSize[ 0 ] = mPlaneMask[ 0 ] ? aDimX : 1;
//      mDimSize[ 1 ] = mPlaneMask[ 1 ] ? aDimY : 1;
//      mDimSize[ 2 ] = mPlaneMask[ 2 ] ? aDimZ : 1;
//
//      mSize = mDimSize[ 0 ] * mDimSize[ 1 ] * mDimSize[ 2 ];
//
//      mElements = (SGE_Actors *) PkHeapAllocate( sizeof( SGE_Actors ) * mSize );
//      assert( mElements );
//
//      int xMyCoord = mPlaneMask[ 0 ] ? MyCoordX : 0;
//      int yMyCoord = mPlaneMask[ 1 ] ? MyCoordY : 0;
//      int zMyCoord = mPlaneMask[ 2 ] ? MyCoordZ : 0;
//
//      for( int x=0; x < mDimSize[ 0 ]; x++ )
//        {
//        for( int y=0; y < mDimSize[ 1 ]; y++ )
//          {
//          for( int z=0; z < mDimSize[ 2 ]; z++ )
//            {
//            int index = (mDimSize[ 1 ] * x + y) * mDimSize[ 2 ] + z;
//            assert( index >=0 && index < mSize );
//
//            mElements[ index ].mRank = index;
//            mElements[ index ].mCoord.Set( mPlaneMask[ 0 ] ? x : mMyCoord[ 0 ],
//                                           mPlaneMask[ 1 ] ? y : mMyCoord[ 1 ],
//                                           mPlaneMask[ 2 ] ? z : mMyCoord[ 2 ] );
//
//            int BGLRank = BGLPartitionMakeRank( mElements[ index ].mCoord.mX,
//                                                mElements[ index ].mCoord.mY,
//                                                mElements[ index ].mCoord.mZ );
//
//            mBGLRankToGroupRankMap[ BGLRank ] = index;
//
//            if( x==xMyCoord && y==yMyCoord && z==zMyCoord )
//              mMyRank = index;
//            }
//          }
//        }
//      }
//
//    inline int GetSize()
//      {
//      return mSize;
//      }
//
//    inline int GetMyRank()
//      {
//      return mMyRank;
//      }
//
//    inline TorusXYZ& GetCoordFromRank( int aRank )
//      {
//      assert( aRank>=0 && aRank < mSize );
//      return mElements[ aRank ].mCoord;
//      }
//
//    inline void GetMyCoords( int* x, int* y, int* z )
//      {
//      *x = mMyCoord[ 0 ];
//      *y = mMyCoord[ 1 ];
//      *z = mMyCoord[ 2 ];
//      }
//
//#if 0
//    inline int GetRankFromCoord( int* aCoord ) const
//      {
//      int Coord[ 3 ];
//      int curI = 0;
//      for( int i=0; i<GDIM; i++ )
//        {
//        if( mPlaneMask[ i ] )
//          Coord[ i ] = aCoord[ curI++ ];
//        else
//          Coord[ i ] = -1;
//        }
//
//      int x = Coord[ 0 ] * mPlaneMask[ 0 ];
//      int y = Coord[ 1 ] * mPlaneMask[ 1 ];
//      int z = Coord[ 2 ] * mPlaneMask[ 2 ];
//
//      int index = (mDimSize[ 1 ]*x + y)*mDimSize[ 2 ] + z;
//
//      if( !( index >=0 && index < mSize ) )
//        {
//        printf("index: %d {x,y,z}: { %d %d %d } mPlaneMask: { %d %d %d } aCoord: { %d %d %d } mDimSize: { %d %d %d }\n",
//               index,
//               x,y,z,
//               mPlaneMask[ 0 ], mPlaneMask[ 1 ], mPlaneMask[ 2 ],
//               aCoord[ 0 ], aCoord[ 1 ], aCoord[ 1 ],
//               mDimSize[ 0 ],mDimSize[ 1 ], mDimSize[ 2 ] );
//        }
//
//      assert( index >=0 && index < mSize );
//      return mElements[ index ].mRank;
//      }
//#endif
//
//#if 1
//    inline int GetRankFromCoord( int* aCoord ) const
//      {
//      int Coord[ 3 ];
//      int curI = 0;
//      for( int i=0; i<GDIM; i++ )
//        {
//        if( mPlaneMask[ i ] )
//          Coord[ i ] = aCoord[ curI++ ];
//        else
//          Coord[ i ] = mMyCoord[ i ];
//        }
//
//      // Convert Coord to BGL Rank;
//      int BGLRank = BGLPartitionMakeRank( Coord[ 0 ], Coord[ 1 ], Coord[ 2 ] );
//
//      assert( BGLRank>=0 && BGLRank < mMachineSize );
//      int Rank = mBGLRankToGroupRankMap[ BGLRank ];
//
//      assert( Rank != -1 );
//      assert( Rank == mElements[ Rank ].mRank );
//      assert( mElements[ Rank ].mCoord[ 0 ] == Coord[ 0 ] &&
//              mElements[ Rank ].mCoord[ 1 ] == Coord[ 1 ] &&
//              mElements[ Rank ].mCoord[ 2 ] == Coord[ 2 ] );
//
//      return Rank;
//      }
//#endif
//};

#define PACKET_ALLTOALLV_LIMIT_RANK   ( 1 << 14 )
#define PACKET_ALLTOALLV_LIMIT_COUNT  ( 1 << 20 )
#define PACKET_ALLTOALLV_LIMIT_OFFSET ( 1 << 22 )

struct BGL_AlltoallvPacketMetaData
  {
  unsigned long long  mSourceRank     : 14;
  unsigned long long  mTotalCount     : 20;
  unsigned long long  mOffset         : 22;
  unsigned long long  mBytesInPayload : 8;
  };

//struct BGLAlltoallvHeaders
//{
//int              mDestRank;
//_BGL_TorusPktHdr mHdr PA_MEMORY_ALIGN(5);
//};

class AllToAllvHeader
{
  public: 
  unsigned int mPE ; // Target processing element
  unsigned short mTargetRank ; // Target rank of the 'send'
  unsigned short mSourceIndex ; // Sequence number within all those sent from this source to this target
} ;

// Use cases in BlueMatter
// In the case of the fft we can create the entire state
class PacketAlltoAllv_PkActors
{
  public: 
    double padding0[4];
    int                 mMyRank;
    TorusXYZ            mMyCoord;
    TorusXYZ            mPeriod;
    int                 mGroupSize;
    int                 mBlockShift ; 
    int                 mSendBytes ;
    int                 mReceiveBytes ;

    int*                mSendOffsets;
    int*                mBytesRecvFromNode;
    int*                mActiveSendRanks;
    int*                mActiveSendRanksTransient;
    int                 mActiveSendRanksCount;

    int                 mCurrentInject;
    int                 mPktInject;
    char                mPacketsToInjectByStatus[_BGL_TORUS_FIFO_CHUNKS ] ;

    int*                mCurrentRankPtr;
    int*                mEndRankPtr;
    int                 mLargestPacketChunks;
    int                 mMinPacketsInSendFIFO;

//    BGLAlltoallvHeaders*   mHeadersForLink[ NUMBER_OF_LINKS ] PA_MEMORY_ALIGN(5);
    AllToAllvHeader*   mHeaders PA_MEMORY_ALIGN(5);
    int                 mHeaderCount;

    GroupContainer_Actors*     mGroup;

    SGE*                mGroupElements;

    int                 mFirstEntry;

    int mMyCore;

    int mPacketsToInject;

    void* mSendBuff;
    int*  mSendCount;
    int*  mSendOffsetIn;
    int   mSendTypeSize;

    void* mRecvBuff;
    int*  mRecvCount;
    int*  mRecvOffset;
    int   mRecvTypeSize;

    int mRecvCountDone;
    int mRecvDoneBits;
    int mRecvDoneExpectBits;
    int mRecvNonActiveCount;

    int mTotalSentPackets PA_MEMORY_ALIGN( 5 );
    int mTotalRecvPackets PA_MEMORY_ALIGN( 5 );
    double padding1[4];

    // FifoHelper mFifoStats[ 6 ] PA_MEMORY_ALIGN( 5 );

    // Randomize Tools


    void PermuteActiveSendRanks( void );

    inline int GetNextRankToSend();

//    static inline void SendMessages( PacketAlltoAllv* ThisPtr );
//    static inline void SendMessages_Old();

    // inline void SendMessages_Old();

//    template< int NON_ALIGNED1 > static inline void RecvMessages( PacketAlltoAllv* ThisPtr );

    static void* InitActor( void * );
    static void* ExecuteActor( void * );
    static void* FinalizeActor( void * );

public:
    PacketAlltoAllv_PkActors()
      {
      mFirstEntry = 1;

      }

    ~PacketAlltoAllv_PkActors()
      {
      }
      
    void Actor(unsigned int aFlags, unsigned int aParam, unsigned int aCells, void * aPayload) ;

    static void RandomizeHeaders( AllToAllvHeader* aHdrs, int aCount );

#if 0
    void Init( int* aSendCount, int* aSendOffset, int aSendTypeSize,
               int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
               GroupContainer_Actors* aGroup );

    void Finalize();

    void Execute( void* aSendBuf, void* aRecvBuf );

    void Execute( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
                  void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                  GroupContainer_Actors* aGroup, int debug=0 );

#endif

    void InitSingleCore( int aSendCount[], int* aSendOffset, int aSendTypeSize,
                         int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                         GroupContainer_Actors* aGroup );

    void FinalizeSingleCore();


    template< int NON_ALIGNED1 > void ExecuteSingleCore1( void* aSendBuf, void* aRecvBuf );

    void ExecuteSingleCore( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
                            void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                            GroupContainer_Actors* aGroup, int aligned=1, int debug=0 );

    void InitSingleCoreSimple( int* aSendCount, int* aSendOffset, int aSendTypeSize,
                               int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                               GroupContainer_Actors* aGroup,
                               int* aSendNodeList = NULL, int aSendNodeListCount = 0,
                               int* aRecvNodeList = NULL, int aRecvNodeListCount = 0 );

    template< int NON_ALIGNED1 > void ExecuteSingleCoreSimple1( void* aSendBuf, void* aRecvBuf );

    void ExecuteSingleCoreSimple( void* aSendBuf, int aSendCount[], int* aSendOffset, int aSendTypeSize,
                                  void* aRecvBuf, int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                                  GroupContainer_Actors* aGroup, int aligned=1, int debug = 0,
                                  int* aSendNodeList = NULL, int aSendNodeListCount = 0,
                                  int* aRecvNodeList = NULL, int aRecvNodeListCount = 0 );

    void FinalizeSingleCoreSimple();
    
    static void ReportFifoHistogram(void) ;

};

#endif /*PACKET_ALLTOALLV_PKACTORS_HPP_*/
