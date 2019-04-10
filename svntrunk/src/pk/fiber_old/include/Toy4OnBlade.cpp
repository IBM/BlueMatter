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
#include <unistd.h>
#include <assert.h>
#include "XYZ.hpp"
#include <string.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <rts.h>
#include <blade_on_blrts.h>

#include "BGLTorusAppSupport.c"
#include <pk/fxlogger.hpp>
#include <pk/platform.hpp>

#include "BonB_CoRoutine.h"

//#include <builtins.h>

#ifndef FXLOG_VERBOSE
#define FXLOG_VERBOSE (0)
#endif

#ifndef PK_COROUTINE
#define PK_COROUTINE (0)
#endif

#ifndef PK_BGL_ACTOR
#define PK_BGL_ACTOR (0)
#endif

#ifndef PK_TORUS_BARRIER
#define PK_TORUS_BARRIER (0)
#endif

#ifndef PK_SHMEM
#define PK_SHMEM (0)
#endif

#ifndef TOY_APP
#define TOY_APP (0)
#endif

unsigned TaskId    = 0xFFFFFFFF;
unsigned TaskCount = 0xFFFFFFFF;

int aCore = 0; // change when we go dual core

double
GetTimeInSeconds()
  {
  unsigned long long now = rts_get_timebase();
  double rc = 1.0 * now;
  rc /= 700 * 1000 * 1000;
  return( rc );
  }

TorusXYZ MakeTaskXYZ( int );

int
MakeTaskId( TorusXYZ aLoc )
  {
  int rc = Platform::Topology::MakeRankFromCoords( aLoc.mX, aLoc.mY, aLoc.mZ );

  #ifndef NDEBUG
    if( aLoc != MakeTaskXYZ( rc ) ) PLATFORM_ABORT( "MakeTaskId() - doesn't match MakeTaskXYZ");
  #endif

  return( rc );
  }

TorusXYZ
MakeTaskXYZ( int aTaskId )
  {
  int x, y, z;
  Platform::Topology::GetCoordsFromRank( aTaskId, &x, &y, &z );
  TorusXYZ rc;
  rc.Set( x, y, z );
  #ifndef NDEBUG
    if( aTaskId != Platform::Topology::MakeRankFromCoords( rc.mX, rc.mY, rc.mZ) )
        PLATFORM_ABORT("MakeTaskXYZ() - doesn't match MakeTaskId");
  #endif
  return( rc );
  }

////////////////////////////// shmem++


#define TCB_AlignmentPadding (0x01 << 10)
#define TCB_AlignmentMask   (TCB_AlignmentPadding - 1)

typedef double TCB_MAGIC[2];

#define ppadsize (1)

struct ThreadControlBlock
  {
  void * StackSeparationPadding[16];  // stack will grow in other dir from tcb

  TCB_MAGIC mBeginning;
void * prepadding[ppadsize];
  _BG_CoRoutine_t SwapData;
void * afterpadding[ppadsize];

  ThreadControlBlock *Next;
  ThreadControlBlock *Prev;

  void * mThreadMemoryMallocBase;

  BG_CoRoutineFunctionType mThreadMainFx;
  void *                   mThreadArg;

  _BGL_TorusPkt   mResponsePacket;
  _BGL_TorusPkt * mResponsePacketPtr;

  int mExited;  // should be a state
  int mDetatched;

  ThreadControlBlock * mJoiner; // probably want to allow a list the list of threads joined to me

  TCB_MAGIC mEnding;
  };

void
SetMagic( ThreadControlBlock *aTCB )
  {
  double dtcb = (unsigned)aTCB;

  aTCB->mBeginning[0] = dtcb;
  aTCB->mBeginning[1] = dtcb + sizeof( ThreadControlBlock );
  aTCB->mEnding   [0] = dtcb * -1;
  aTCB->mEnding   [1] = dtcb + sizeof( ThreadControlBlock ) * -1;
  }

void
CheckMagic( ThreadControlBlock *aTCB )
  {
  double dtcb = (unsigned)aTCB;

  if( aTCB->mBeginning[0] != dtcb )
    PLATFORM_ABORT( "BAD THREAD CONTROL BLOCK MAGIC" );
  if( aTCB->mBeginning[1] != dtcb + sizeof( ThreadControlBlock ) )
    PLATFORM_ABORT( "BAD THREAD CONTROL BLOCK MAGIC" );
  if( aTCB->mEnding   [0] != dtcb * -1 )
    PLATFORM_ABORT( "BAD THREAD CONTROL BLOCK MAGIC" );
  if( aTCB->mEnding   [1] != dtcb + sizeof( ThreadControlBlock ) * -1 )
    PLATFORM_ABORT( "BAD THREAD CONTROL BLOCK MAGIC" );
  }

ThreadControlBlock * ReadyQueueHead     = NULL;
ThreadControlBlock * RunningThreadsTCB  = NULL;

void
Unblock(ThreadControlBlock * aTCB )
  {
  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "Unblock() "
    << " starting on  "    << (void*) aTCB
    << " ReadyQueueHead "  << (void*) ReadyQueueHead
    << EndLogLine;
  assert( aTCB       != NULL );
  assert( aTCB       == aTCB->Next );
  assert( aTCB->Next == aTCB->Prev );

  if( aTCB != aTCB->Next || aTCB->Next != aTCB->Next )
    PLATFORM_ABORT("Unblock - aTCB->Next != aTCB || aTCB->Prev != aTCB" );

  if( ReadyQueueHead == NULL )
    {
    ReadyQueueHead = aTCB;
    }
  else
    {
    aTCB->Next                 = ReadyQueueHead;
    aTCB->Prev                 = ReadyQueueHead->Prev;
    ReadyQueueHead->Prev->Next = aTCB;
    ReadyQueueHead->Prev       = aTCB;
    }
  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "Unblock() "
    << " finished on "     << (void*) aTCB
    << " ReadyQueueHead "  << (void*) ReadyQueueHead
    << EndLogLine;
  }

int PCP_SendPacketQueueHead  = 0;  // next in index
int PCP_SendPacketQueueTail  = 0;  // next out
int RoundRobinFifoSelect     = 0;  // next fifo

// This should turn out to be a 32 byte chunk of memory that is l1 cache aligned.
// We should be able to have an array of these.
typedef QUADWORD _BGL_TorusChunk[2] ALIGN_L1_CACHE;

#define PCP_SendQueueSize (0x01<<(15+1)) // 2**15 = 32k.  32k * 32 bytes = 1MB. 15+1 = 2MB
//_BGL_TorusPktHdr PCP_SendQueue[ PCP_SendQueueSize * 2 ]; //bug Hdr is a quadword // allocate as if every element was a 1 chunk (32 byte) packet
_BGL_TorusChunk PCP_SendQueue[ PCP_SendQueueSize ]; //bug Hdr is a quadword // allocate as if every element was a 1 chunk (32 byte) packet
int PCP_SendQueueActiveMask = PCP_SendQueueSize - 1;


int MakeMeAFunctionAddr(void*) {}

typedef int (*ActorFunctionType)(void * payload);
#define SpecialActorFunction_LeaveInThreadBuffer ((ActorFunctionType)(5 & (unsigned)MakeMeAFunctionAddr))

typedef union
  {
  unsigned long mWord;
  struct {
         unsigned mSrcHigh        : 8;
         unsigned mCore           : 2;  // prep for P with 4 cores
         unsigned mActorFx        : 22; // "massaged" Active Function Pointer (not directly usable)
         };
  } PktHdrW0;

typedef union
  {
  unsigned long mWord;
  struct {
         unsigned mSrcLow        : 8;  // additional untyped 10bit argument (may become 8bits)
         unsigned mReserved      : 5;  // could be needed in src for big machines, or context for mor mem
         unsigned mThreadContext : 19; // bits required for 1KB aligned threads within 512MB
         };
  } PktHdrW1;

void
UnloadHeaderWords( unsigned long       aW0,
                   unsigned long       aW1,
                   unsigned long       * aSource,
                   ActorFunctionType   * aActorFx,
                   ThreadControlBlock ** aContext)
  {
  PktHdrW0 w0;
  PktHdrW1 w1;
  w0.mWord = aW0;
  w1.mWord = aW1;
  *aSource  = w0.mSrcHigh << 8 | w1.mSrcLow;
  unsigned long ActorFx = (unsigned long) w0.mActorFx;
  unsigned long ThisFx  = (unsigned long) UnloadHeaderWords;
  *aActorFx = (ActorFunctionType) ((ActorFx << 2) | (ThisFx & 0xFF000000));
  *aContext = (ThreadControlBlock *) (w1.mThreadContext << 10);
  }

void
LoadHeaderWords( unsigned long      & aW0,
                 unsigned long      & aW1,
                 unsigned long        aSource,
                 ActorFunctionType    aActorFx,
                 ThreadControlBlock * aContext)
  {
  PktHdrW0 w0;
  PktHdrW1 w1;

  w0.mSrcHigh       = ( aSource >> 8 ) & 0x000000FF;
  w0.mCore          = 0;
  w0.mActorFx       = (((unsigned) aActorFx) & 0x00FFFFFF) >> 2;

  w1.mSrcLow        = aSource & 0x000000FF;
  w1.mReserved      = 0;
  w1.mThreadContext = ((unsigned) aContext) >> 10;

  aW0 = w0.mWord;
  aW1 = w1.mWord;

  #ifndef NDEBUG
  unsigned long        src;
  ActorFunctionType    fx;
  ThreadControlBlock * ctx;

  UnloadHeaderWords( aW0, aW1, &src, &fx, &ctx);
  assert( src      == aSource );
  if( aActorFx != fx )
    BegLogLine( PK_BGL_ACTOR ) << "LoadHeaderWords() ActorFx " << (void*) aActorFx << " w0.mActorFx " << (void*) w0.mActorFx << " fx " << (void*)fx << EndLogLine;
  assert( aActorFx == fx      );
  assert( aContext == ctx     );
  #endif

  }


void *
ReservePacket( unsigned long        aDest,
               ThreadControlBlock * aContext,
               ActorFunctionType    aActorFx,
               unsigned int         aPayloadSize)
  {
  _BGL_TorusPkt * NextPkt = (_BGL_TorusPkt *) & (PCP_SendQueue[ PCP_SendPacketQueueHead ] );

  BegLogLine( PK_BGL_ACTOR )
    << (void*) RunningThreadsTCB << " "
    << "Reserve() "
    << " starting "
    << " Q_Head "       << PCP_SendPacketQueueHead
    << " Q_Tail "       << PCP_SendPacketQueueTail
    << " aDest "        << aDest
    << " Context "      << (void*) aContext
    << " ActorFx "      << (void*) aActorFx
    << " aPayloadSize " << aPayloadSize
    << " Pkt@ "         << (void*) NextPkt
    << EndLogLine;

  assert( aPayloadSize < _BGL_TORUS_PKT_MAX_PAYLOAD );
  unsigned int Chunks = (aPayloadSize + _BGL_TORUS_SIZEOF_CHUNK - 1 ) / _BGL_TORUS_SIZEOF_CHUNK;
  PCP_SendPacketQueueHead += Chunks;

  if( PCP_SendPacketQueueHead > (PCP_SendQueueSize - _BGL_TORUS_PKT_MAX_CHUNKS) )
    PCP_SendPacketQueueHead = 0;

  // Check that there is enough space to put one more packet into send queue while allowing recvs to
  // progress -- recvs need several packets free for responses.
  int ImagedTailIndex = PCP_SendPacketQueueTail;
  if( ImagedTailIndex <= PCP_SendPacketQueueHead )
    ImagedTailIndex += PCP_SendQueueActiveMask + 1;

  // Should have one free packet for each one that might be received AND one extra because we wrap before the end
  // This ensures that each received packet can enqueue one response - not possible?  deadlock risk.
                                                   // 2 * is because buffer is quadwords and not chunks
  if( ImagedTailIndex - PCP_SendPacketQueueHead < (8 * _BGL_TORUS_PKT_MAX_CHUNKS) )
    PLATFORM_ABORT("Reserve packet encountered full buffer situation");

  unsigned long w0;
  unsigned long w1;

  LoadHeaderWords( w0, w1, TaskId, aActorFx, aContext );

  TorusXYZ ThereXYZ  = MakeTaskXYZ( aDest );

  BGLTorusMakeHdrAppChooseRecvFifo( // this method picks a core as it's last arg
                       &(NextPkt->hdr),
                       ThereXYZ.mX, ThereXYZ.mY, ThereXYZ.mZ,
                       w0, w1,
                       aPayloadSize, // Payload in bytes
                       0 ); // core (fifo group) on dest

  BegLogLine( PK_BGL_ACTOR )
    << (void*) RunningThreadsTCB << " "
    << "Reserve() "
    << " starting "
    << " Pkt "          << (void*) NextPkt
    << " Chunks "       << Chunks
    << " Q_Head "       << PCP_SendPacketQueueHead
    << " Q_Tail "       << PCP_SendPacketQueueTail
    << " hwh0 "         << (void*) w0
    << " hwh1 "         << (void*) w1
    << EndLogLine;

  return( (void *) NextPkt->payload );
  }


void
SendPackets( _BGL_TorusFifoStatus * FifoStatus )
  {
  BegLogLine(0) // off due to high vol
    << (void*) RunningThreadsTCB << " "
    << "SendPackets() "
    << " Q_Head "       << PCP_SendPacketQueueHead
    << " Q_Tail "       << PCP_SendPacketQueueTail
    << EndLogLine;
  while( PCP_SendPacketQueueTail != PCP_SendPacketQueueHead )
    {
    _BGL_TorusPktHdr* NextPacket = (_BGL_TorusPktHdr  *) & PCP_SendQueue[ PCP_SendPacketQueueTail ];

    int Chunks = NextPacket->hwh0.Chunks + 1;

    _BGL_TorusFifo * NextFifoToSend;
    if( aCore == 0 ) NextFifoToSend = _ts_CheckClearToSendOnLink0(  FifoStatus, RoundRobinFifoSelect, Chunks );
    else             NextFifoToSend = _ts_CheckClearToSendOnLink1(  FifoStatus, RoundRobinFifoSelect, Chunks );

    RoundRobinFifoSelect++;
    if( RoundRobinFifoSelect == 3 )
      RoundRobinFifoSelect = 0;

    // Check if there is room in the fifo to send, if not, break out and try later.  Could be better.
    if( NextFifoToSend == NULL )
      break;

    #if PK_BGL_ACTOR
      int w0 = NextPacket->swh0.arg0;
      int w1 = NextPacket->swh1.arg;
      unsigned long        src;
      ActorFunctionType    fx;
      ThreadControlBlock * ctx;
      UnloadHeaderWords( w0, w1, &src, &fx, &ctx);
      assert( src      == TaskId );
      BegLogLine( PK_BGL_ACTOR )
        << (void*) RunningThreadsTCB << " "
        << "SendPackets() "
        << " INJECTING "
        << " Q_Head "       << PCP_SendPacketQueueHead
        << " Q_Tail "       << PCP_SendPacketQueueTail
        << " pkt addr "     << (void*) & (PCP_SendQueue[ PCP_SendPacketQueueTail ])
        << " chunks "       << Chunks
        << " w0 "           << (void*) w0
        << " w1 "           << (void*) w1
        << " src "          << src
        << " fx "           << (void*) fx
        << " ctx "          << (void*) ctx
        << " pl0 "          << ((void**)NextPacket)[4]
        << " pl1 "          << ((void**)NextPacket)[5]
        << " pl2 "          << ((void**)NextPacket)[6]
        << " pl3 "          << ((void**)NextPacket)[7]
        << EndLogLine;

      BegLogLine( PK_BGL_ACTOR )
        << (void*) RunningThreadsTCB << " "
        << "SendPackets() "
        << " INJECTING "
        << " 0 "          << ((void**)NextPacket)[0]
        << " 1 "          << ((void**)NextPacket)[1]
        << " 2 "          << ((void**)NextPacket)[2]
        << " 3 "          << ((void**)NextPacket)[3]
        << " 4 "          << ((void**)NextPacket)[4]
        << " 5 "          << ((void**)NextPacket)[5]
        << " 6 "          << ((void**)NextPacket)[6]
        << " 7 "          << ((void**)NextPacket)[7]
        << EndLogLine;
    #endif

    BGLTorusInjectPacketImageApp(   NextFifoToSend,
                                  (_BGL_TorusPkt*) NextPacket );

    // Wrap before don't have enough to fit a full packet
    PCP_SendPacketQueueTail += Chunks ;
    if( PCP_SendPacketQueueTail > (PCP_SendQueueSize - _BGL_TORUS_PKT_MAX_CHUNKS ))
      PCP_SendPacketQueueTail = 0;
    }
  }


// global info about packet being received to be used for response
unsigned long         CurrentActorPacketSource;
ThreadControlBlock *  CurrentActorPacketContext;

void *
ReserveResponsePacket( ActorFunctionType aft,
                       unsigned int      aPayloadSize )
  {
  void * ResPkt = ReservePacket( CurrentActorPacketSource,
                                 CurrentActorPacketContext,
                                 aft,
                                 aPayloadSize );
  return( ResPkt );
  }

void
ReceiveAndProcessActorPackets( _BGL_TorusFifoStatus * FifoStatus )
  {
  int ImagedTailIndex = PCP_SendPacketQueueTail;
  if( ImagedTailIndex <= PCP_SendPacketQueueHead )
    ImagedTailIndex += PCP_SendQueueActiveMask + 1;

  // Should have one free packet for each one that might be received AND one extra because we wrap before the end
  // This ensures that each received packet can enqueue one response - not possible?  deadlock risk.
                                          // 2 * is because buffer is quadwords and not chunks
  if( ImagedTailIndex - PCP_SendPacketQueueHead > ( 7 * _BGL_TORUS_PKT_MAX_CHUNKS) )
    {
    // Get 0..6 packets onto stack buffer
    // TRY RECEIVING TO A PACKET POOL ALLOC SO FXs CAN KEEP PACKETS TO SAVE COPIES
    _BGL_TorusPkt SixPackets[ 6 ];

    int PacketCount;
    if( aCore == 0 )
        PacketCount = _ts_PollF0_OnePassReceive_Put(  FifoStatus,
                                                     &( SixPackets[ 0 ] ),
                                                     sizeof( _BGL_TorusPkt )  );
    else
        PacketCount = _ts_PollF1_OnePassReceive_Put( FifoStatus,
                                                     &( SixPackets[ 0 ] ),
                                                     sizeof( _BGL_TorusPkt )  );

    for( int PacketIndex = 0; PacketIndex < PacketCount ;  PacketIndex++ )
      {
      int w0 = SixPackets[ PacketIndex ].hdr.swh0.arg0;
      int w1 = SixPackets[ PacketIndex ].hdr.swh1.arg;

      ActorFunctionType ActorFx;

      UnloadHeaderWords(   w0, w1,
                         & CurrentActorPacketSource,
                         & ActorFx,
                         & CurrentActorPacketContext );

      BegLogLine( PK_BGL_ACTOR )
        << (void*) RunningThreadsTCB << " "
        << "RecvAndProcPkts() "
        << "RECV PKT"
        << " PktInd "       << PacketIndex
        << " w0 "           << (void*) SixPackets[ PacketIndex ].hdr.swh0.arg0
        << " w1 "           << (void*) SixPackets[ PacketIndex ].hdr.swh1.arg
        << " PktSrc "       << CurrentActorPacketSource
        << " ActorFx "      << (void*) ActorFx
        << " Ctx "          << (void*) CurrentActorPacketContext
        << " Chunks "       << SixPackets[ PacketIndex ].hdr.hwh0.Chunks + 1
        << EndLogLine;

      BegLogLine( PK_BGL_ACTOR )
        << (void*) RunningThreadsTCB << " "
        << "RecvAndProcPkts() "
        << " RECEIVED "
        << " 0 "          << ((void**)(&SixPackets[PacketIndex]))[0]
        << " 1 "          << ((void**)(&SixPackets[PacketIndex]))[1]
        << " 2 "          << ((void**)(&SixPackets[PacketIndex]))[2]
        << " 3 "          << ((void**)(&SixPackets[PacketIndex]))[3]
        << " 4 "          << ((void**)(&SixPackets[PacketIndex]))[4]
        << " 5 "          << ((void**)(&SixPackets[PacketIndex]))[5]
        << " 6 "          << ((void**)(&SixPackets[PacketIndex]))[6]
        << " 7 "          << ((void**)(&SixPackets[PacketIndex]))[7]
        << EndLogLine;

      if( ActorFx == SpecialActorFunction_LeaveInThreadBuffer )
        {
        // special case to just deposit the packet in the context
        ThreadControlBlock * PktContextTCB = (ThreadControlBlock *) CurrentActorPacketContext;

        // this is a copy right now but should just hook a pointer to a pool packet up
        //memcpy( & PktContextTCB->mResponsePacket, & (SixPackets[ PacketIndex ]), sizeof(_BGL_TorusPkt) );

        int chunks = SixPackets[ PacketIndex ].hdr.hwh0.Chunks + 1;
        double* pkt = (double*) & (SixPackets[ PacketIndex ]);
        double* buf = (double*) & (PktContextTCB->mResponsePacket);
        buf[0] = pkt[0];
        buf[1] = pkt[1];
        buf[2] = pkt[2];
        buf[3] = pkt[3];
        for(int c = 1; c < chunks; c++ )
          {
          buf[c*2+0] = pkt[c*2+0];
          buf[c*2+1] = pkt[c*2+1];
          buf[c*2+2] = pkt[c*2+2];
          buf[c*2+3] = pkt[c*2+3];
          }

        PktContextTCB->mResponsePacketPtr = & PktContextTCB->mResponsePacket;
        Unblock( PktContextTCB );
        }
      else
        {
        //(((Bit32)actor & 0x00FFFFFC) >> 2);
        ////(*(ActorFunctionType*)ActorFx)( (void *) & SixPackets[PacketIndex].payload );
        ActorFx( (void *) & SixPackets[PacketIndex].payload );
        }

      //PointerChasePacket * SendPktPtr = &(PCP_SendQueue[ PCP_SendPacketQueueHead ]);
      }
    }
  }

// This is suitable to be called by the spi barrier waits
inline
int
KickPipes( void * ) // Named in honour of PHOCH
  {
  _BGL_TorusFifoStatus FifoStatus;
  if( aCore == 0 )
    _ts_GetStatus0( & FifoStatus );
  else
    _ts_GetStatus1( & FifoStatus );

  SendPackets( & FifoStatus );

  ReceiveAndProcessActorPackets( & FifoStatus );

  SendPackets( & FifoStatus );

  return(0);
  }

void
PkCo_Barrier()
  {
  Platform::Control::BarrierWithWaitFunction( (void*) KickPipes, NULL );
  }

void
Yield()
  {
  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "Yield() "
    << " starting "
    << EndLogLine;
  // this block would be handled bellow - but it fast paths a non blocking yield
  // really shouldn't make a diff...
  if( ReadyQueueHead == RunningThreadsTCB )
    {
    KickPipes( NULL );
    return;
    }

  for(;;) // Either return or swap
    {
    KickPipes( NULL );

    if( ReadyQueueHead != NULL )
      {
      if( ReadyQueueHead == RunningThreadsTCB )
        {
        BegLogLine( PK_COROUTINE )
          << (void*) RunningThreadsTCB << " "
          << "Yield() "
          << " finished - NO SWAP "
          << EndLogLine;
        return;
        }
      else
        {
        BegLogLine( PK_COROUTINE )
          << (void*) RunningThreadsTCB << " "
          << "Yield()"
          << " SWAPPING TO "
          << " ReadyQueueHead "   << (void*) ReadyQueueHead
          << " ReadyQueueHead->mThreadMainFx " << (void*) ReadyQueueHead->mThreadMainFx
          << " @ "                << (void*) (&(ReadyQueueHead->mThreadMainFx))
          << " ReadyQueueHead->mThreadArg "    << (void*) ReadyQueueHead->mThreadArg
          << EndLogLine;

        assert( ReadyQueueHead    != NULL );
        assert( RunningThreadsTCB != NULL );
        ThreadControlBlock * FromTCB = RunningThreadsTCB;
        RunningThreadsTCB = ReadyQueueHead; // This is the only place this should be changed

BegLogLine( PK_COROUTINE )
  << (void*) RunningThreadsTCB << " (changed for swap) "
  << "Yield()"
  << " FromTCB @" << (void*) FromTCB
  << " FromTCB->SwapData @"        << (void*) &FromTCB->SwapData
  << " ... @"                      << (void*) (((unsigned)&FromTCB->SwapData) + sizeof( FromTCB->SwapData ))
  << " ReadyQueueHead->SwapData @" << (void*) &ReadyQueueHead->SwapData
  << " ... @"                      << (void*) (((unsigned)&ReadyQueueHead->SwapData) + sizeof( FromTCB->SwapData))
  << " sizeof( SwapData ) "        << sizeof( FromTCB->SwapData )
  << EndLogLine;

if( ppadsize > 1 )
for(int i = 0; i < ppadsize; i++ )
  {
  RunningThreadsTCB->prepadding[i] = &RunningThreadsTCB->prepadding[i];
  RunningThreadsTCB->afterpadding[i] = &RunningThreadsTCB->afterpadding[i];
  }

        _BG_CoRoutine_Swap( & (FromTCB->SwapData), & (ReadyQueueHead->SwapData) );

        BegLogLine( PK_COROUTINE )
          << (void*) RunningThreadsTCB << " "
          << "Yield() "
          << " finished - AFTER SWAP "
          << " ReadyQueueHead "   << (void*) ReadyQueueHead
          << " RQH->Next "        << (void*) ReadyQueueHead->Next
          << " RQH->Prev "        << (void*) ReadyQueueHead->Prev
          << EndLogLine;
        assert( FromTCB == RunningThreadsTCB );
        return;
        }
      }
    }
  }

void
Block()
  {
  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "Block() "
    << " starting "
    << " ReadyQueueHead " << (void*) ReadyQueueHead
    << " Next "          << (void*) RunningThreadsTCB->Next
    << " Prev "          << (void*) RunningThreadsTCB->Prev
    << EndLogLine;

  //CheckMagic( RunningThreadsTCB );

  // The following block will move ReadyQueueHead off blocking TCB if there is another TCB in queue
  if( ReadyQueueHead == RunningThreadsTCB )
    {
    ReadyQueueHead = ReadyQueueHead->Next;
    BegLogLine( PK_COROUTINE )
      << (void*) RunningThreadsTCB << " "
      << "Block() "
      << " RunningThreadsTCB == ReadyQueueHead "
      << " ReadyQueueHead = ReadyQueueHead->Next "
      << ReadyQueueHead
      << EndLogLine;
    }

  // Pull running thread's TCB out of ReadyQueueHead
  ThreadControlBlock *Next = RunningThreadsTCB->Next;
  ThreadControlBlock *Prev = RunningThreadsTCB->Prev;
  Next->Prev = Prev;
  Prev->Next = Next;

  // Make running thread's TCB self referencing
  RunningThreadsTCB->Next = RunningThreadsTCB;
  RunningThreadsTCB->Prev = RunningThreadsTCB;

  if( ReadyQueueHead == RunningThreadsTCB )
    ReadyQueueHead = NULL;

  Yield();

  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "Block() "
    << " finished "
    << EndLogLine;
  }


// This starter function catches returns from the thread main
void
ThreadStarter( void * aTCB )
  {
  assert( aTCB != NULL );
  ThreadControlBlock * TCB = (ThreadControlBlock *) aTCB;

  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "ThreadStarter() " << " Arg TCB " << aTCB
    << " Func "           << (void*) TCB->mThreadMainFx
    << " @ "              << (void*) (&(TCB->mThreadMainFx))
    << " Arg "            << (void*) TCB->mThreadArg
    << " RTTCB->Next "    << (void*) RunningThreadsTCB->Next
    << " RTTCB->Prev "    << (void*) RunningThreadsTCB->Prev
    << " ReadyQueueHead " << (void*) ReadyQueueHead
    << " RQH->Head "      << (void*) ReadyQueueHead->Next
    << " RQH->Prev "      << (void*) ReadyQueueHead->Prev
    << EndLogLine;

if( ppadsize > 1 )
for(int i = 0; i < ppadsize; i++ )
  {
  if( RunningThreadsTCB->prepadding[i] != &RunningThreadsTCB->prepadding[i] )
    BegLogLine( PK_COROUTINE )
      << (void*) RunningThreadsTCB << " "
      << "ThreadStarter() "
      << " Corrupt prepadding index " << i
      << " at @"                          << (void*)&RunningThreadsTCB->prepadding[i]
      << " value now "                    << (void*) RunningThreadsTCB->prepadding[i]
      << EndLogLine;
  if( RunningThreadsTCB->afterpadding[i] != &RunningThreadsTCB->afterpadding[i] )
    BegLogLine( PK_COROUTINE )
      << (void*) RunningThreadsTCB << " "
      << "ThreadStarter() "
      << " Corrupt afterpadding index " << i
      << " at @"                          << (void*)&RunningThreadsTCB->afterpadding[i]
      << " value now "                    << (void*) RunningThreadsTCB->afterpadding[i]
      << EndLogLine;
  }

  assert( RunningThreadsTCB == TCB );
  assert( TCB->mThreadMainFx != NULL );

  //CheckMagic( RunningThreadsTCB );

  TCB->mThreadMainFx( TCB->mThreadArg );

  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "ThreadStarter() " << " Returned from ThreadMainFx() TCB " << aTCB << EndLogLine;

  TCB->mExited = 1;

  // do join stuff here... should be a way to detach threads
  if( TCB->mJoiner != NULL )
    {
    assert( TCB->mDetatched == 0 );
    TCB->mDetatched = 1;
    Unblock( TCB->mJoiner );
    }

  if( TCB->mDetatched )
    {
    BegLogLine( PK_COROUTINE )
      << (void*) RunningThreadsTCB << " "
      << "ThreadStarter() "
      << " Detached Thread calling free() on stack @"
      << (void*) TCB->mThreadMemoryMallocBase
      << EndLogLine;
    //free( (char*) TCB->mThreadMemoryMallocBase );
    Platform::Heap::Free( (char*) TCB->mThreadMemoryMallocBase );
    }
  // PROBLEM HERE - we've freed the stack but we need to call yield.
  // For now, we use the stack one last time
  Block();
  }

void
Join( ThreadControlBlock *aTCB )
  {
  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "Join() "
    << " starting join on TCB " << (void*) aTCB
    << " aTCB->mExited " << aTCB->mExited
    << " aTCB->mDetatched " << aTCB->mDetatched
    << EndLogLine;
  //CheckMagic( aTCB );
  if( aTCB->mExited == 1 && ! aTCB->mDetatched )
    {
    BegLogLine( PK_COROUTINE )
      << (void*) RunningThreadsTCB << " "
      << "Join() "
      << " TargetTCB->mExited == 1 && mDetached == 0 "
      << " aTCB "         << (void*) aTCB
      << " stack addr "   <<  aTCB->mThreadMemoryMallocBase
      << EndLogLine;
    /////free( aTCB->mThreadMemoryMallocBase );   // this should perhaps be done after the join
    Platform::Heap::Free( (char*) aTCB->mThreadMemoryMallocBase );
    return;
    }
  else
    {
    assert( aTCB->mJoiner == NULL ); // for now, only one joiner
    assert( RunningThreadsTCB != aTCB );  // would be deadlock without third party kill
    aTCB->mJoiner = RunningThreadsTCB;
    Block();
    }
  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "Join() "
    << " finished join on TCB " << (void*) aTCB
    << EndLogLine;
  }

// CreateThreadControlBlock() allocates a tcb/stack area.  This area is buffered so that
// thread ids can be the actual address of the TCB.

void
CreateThreadControlBlock( ThreadControlBlock * *aTCB, unsigned aReqStackSize )
  {
  unsigned MallocStackSize = TCB_AlignmentPadding + sizeof( ThreadControlBlock ) + aReqStackSize;

  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "CreateTCB() "
    << " ReqStackSize "          << (void*) aReqStackSize      << " " << aReqStackSize
    << " sizeof( ThreadControlBlock ) " << (void*) sizeof( ThreadControlBlock ) << " " << sizeof( ThreadControlBlock )
    << " TCB_AlignmentPadding "   << (void*) TCB_AlignmentPadding << " " << TCB_AlignmentPadding
    << " MallocStackSize "       << (void*) MallocStackSize << " " << MallocStackSize
    << EndLogLine;

  unsigned MallocAddr = (unsigned) Platform::Heap::Allocate( MallocStackSize );

  if( MallocAddr == 0 )
    PLATFORM_ABORT( "Failed to allocate a thread stack" );

  // Force allignment on stack to reduce the number of significant bits in the thread id/address
  // Note that TCB is at the high address end of the malloced region.  The stack begins at the base
  // (low) address of the TCB and grows toward zero.
  unsigned TCB_Address = ( MallocAddr + aReqStackSize + TCB_AlignmentPadding - 1 ) &  ( ~TCB_AlignmentMask);

  ThreadControlBlock * TCB = (ThreadControlBlock *) TCB_Address;

  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "CreateTCB() "
    << " New TCB "              << (void*) TCB
    << " aReqStackSize "        << (void*) aReqStackSize
    << " MallocSize "           << (void*) MallocStackSize
    << " MallocAddr "           << (void*) MallocAddr
    << " Used TCB addr "        << (void*) TCB_Address
    << EndLogLine;


  TCB->mExited = 0;
  TCB->mThreadMemoryMallocBase = (void *) MallocAddr;  // so we can free the the stack later
  TCB->mThreadMainFx           = NULL;
  TCB->mThreadArg              = 0;
  TCB->mJoiner                 = NULL;
  TCB->mDetatched              = 0;
  TCB->Next                    = TCB;
  TCB->Prev                    = TCB;

  SetMagic( TCB );
  //CheckMagic( TCB );

  *aTCB = TCB;
  }

ThreadControlBlock *
CreateThread( unsigned                 aReqStackSize,
              BG_CoRoutineFunctionType aThreadMainFx,
              void *                   aThreadArg )
  {
  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "CreateThread() "
    << " aReqStackSize "      << (void*) aReqStackSize
    << " aThreadMainFx "      << (void*) aThreadMainFx
    << " aThreadArg "         << (void*) aThreadArg
    << " ThreadStarter "      << (void*) ThreadStarter
    << EndLogLine;

  ThreadControlBlock *TCB;

  // Allocate the block of memory that will hold the TCB and the thread stack.
  // Only one addr - TCB - is returned.  The stack is toward zero from this and the tcb the other way.
  CreateThreadControlBlock( & TCB, aReqStackSize );

  TCB->mThreadMainFx           = aThreadMainFx;
  TCB->mThreadArg              = aThreadArg;

  BegLogLine( PK_COROUTINE )
    << (void*) RunningThreadsTCB << " "
    << "CreateThread() "
    << " New TCB "            << (void*) TCB
    << " TCB->mThreadMainFx " << (void*) TCB->mThreadMainFx
    << " TCB->mThreadArg "    << (void*) TCB->mThreadArg
    << " TCB->SwapData @"     << (void*) &TCB->SwapData
    << " sizeof(SwapData) "   << sizeof( TCB->SwapData )
    << " ThreadStarter "      << (void*) ThreadStarter
    << EndLogLine;

  // Init all but the main threads SwapData area.
  // Note: TCB us used as the stack pointer since it is aligned.
  // Stack grows from TCB toward zero, tcb body is the other way.
  _BG_CoRoutine_Init( & TCB->SwapData, ThreadStarter, TCB, (void*)TCB  );

  Unblock( TCB );

  return( TCB );
  }

void
InitThreadSystem()
  {
  assert( RunningThreadsTCB == NULL );
  CreateThreadControlBlock( & RunningThreadsTCB, 0);
  RunningThreadsTCB->Next = RunningThreadsTCB;
  RunningThreadsTCB->Prev = RunningThreadsTCB;
  ReadyQueueHead = RunningThreadsTCB;  // main thread is ready until it blocks
  assert( RunningThreadsTCB );
  return;
  }


template<class ItemType>
class GetItemIndirectAsync
  {
  public:

    struct
    ResponsePacket
      {
      struct Header
        {
        unsigned int   mFlagBitAddr;  // Which addr of bit to set on return... bit number in lower 3 bits
        ItemType*      mDataAddr;
        } mHeader;
      ItemType         mData;
      };

    struct
    RequestPacket
      {
      ItemType**             mBasePtr;
      unsigned int           mIndex;
      typename ResponsePacket::Header mResponseHeader;
      };

    inline
    static
    int
    RequestFx( void *arg )
      {
      RequestPacket * ReqPkt = (RequestPacket *) arg;

      ResponsePacket * ResPkt =
            (ResponsePacket *)
            ReserveResponsePacket( ResponseFx, sizeof( ResponsePacket ) );

      ItemType * LocalArray = *(ReqPkt->mBasePtr);

      BegLogLine( PK_SHMEM )
        << (void*) RunningThreadsTCB << " "
        << "RequestFx() " // << (void*) RequestFx<ItemType>
        << " BasePtr "      << (void*) ReqPkt->mBasePtr
        << " Index "        << ReqPkt->mIndex
        << " LoadAddr "     << (void*) &(LocalArray[ ReqPkt->mIndex ])
        << " sizeof(Item) " << sizeof( ItemType )
        << EndLogLine;

      ResPkt->mData   = LocalArray[ ReqPkt->mIndex ];
      ResPkt->mHeader = ReqPkt->mResponseHeader;

      //EnqueuePacket();
      }

    static
    inline
    void
    SetFlagBit(unsigned int aFlagBitAddr )
      {
      int    flag_bit  = 0x0000001F & aFlagBitAddr;  // pick the bit in word index
      int *  flag_addr = (int*) ( (aFlagBitAddr >> 3) & (~0x03) ); // slide down to have the word addr
      int    flag_mask = 0x01 << flag_bit;
      *flag_addr |= flag_mask;
      }

    inline
    static
    int
    ResponseFx( void *arg )
      {
      ResponsePacket * ResPkt = (ResponsePacket *) arg;

      // Move the data item into place
      *ResPkt->mHeader.mDataAddr = ResPkt->mData;

      // set the bit showing the op is completed
      SetFlagBit( ResPkt->mHeader.mFlagBitAddr );

      BegLogLine( PK_SHMEM )
        << (void*) RunningThreadsTCB << " "
        << "ResponseFx() " // << (void*) RequestFx<ItemType>
        << " sizeof(Item) " << sizeof( ItemType )
        << " Item "         << (ResPkt->mData)
        << " FlagBitAddr "  << (void*) ResPkt->mHeader.mFlagBitAddr
        << " Bit "          << (void*) (0x0000001F & ( ResPkt->mHeader.mFlagBitAddr ))
        << " Addr "         << (void*) ( ResPkt->mHeader.mFlagBitAddr >> 5 )
        << EndLogLine;
      //EnqueuePacket();
      }

    static
    void
    Fetch( ItemType*        aDataAddr,
           unsigned  int   *aFlagAddr,
           unsigned  int    aFlagBit,
           unsigned  int    aPE,
           ItemType**       aBasePtr,
           unsigned  int    aIndex)
      {
      assert( aFlagBit >= 0 && aFlagBit < 32 );
      assert( (aFlagAddr & 0x00000003) == NULL );  // must be 4 byte aligned
      assert( (aFlagAddr & 0xE0000000) == NULL );

      // addr is a word aligned addr ... lower two bits should be 0
      unsigned int FlagBitAddr = (((unsigned)aFlagAddr)<<3) + aFlagBit;

      BegLogLine( PK_SHMEM )
        << (void*) RunningThreadsTCB << " "
        << "ShmemLoadFromBase() "
        << " aTarget "              << (void*) aDataAddr
        << " FlagBitAddr  "         << (void*) FlagBitAddr
        << " Addr  "                << (void*) aFlagAddr
        << " Bit "                  << (void*) aFlagBit
        << " aPE "                  << aPE
        << " aBasePtr "             << (void*) aBasePtr
        << " aIndex "               << aIndex
        << EndLogLine;


      if( aPE == TaskId )
        {
        *aDataAddr = (*aBasePtr)[ aIndex ];
        SetFlagBit( FlagBitAddr );
        BegLogLine( PK_SHMEM )
          << (void*) RunningThreadsTCB << " "
          << "ShmemLoadFromBase() "
          << " Local PE return"
          << " value " << *aDataAddr
          << EndLogLine;
        }
      else
        {
        // Send request
        RequestPacket * ReqPkt =
              (RequestPacket *) ReservePacket( aPE,
                                               RunningThreadsTCB,
                                               RequestFx,
                                               sizeof( RequestPacket ) );

        ReqPkt->mBasePtr                     = aBasePtr;
        ReqPkt->mIndex                       = aIndex;
        ReqPkt->mResponseHeader.mFlagBitAddr = FlagBitAddr;
        ReqPkt->mResponseHeader.mDataAddr    = aDataAddr;

        //EnqueuePacket();

        BegLogLine( PK_SHMEM )
          << (void*) RunningThreadsTCB << " "
          << "ShmemLoadFromBase() "
          << " remote node returning "
          << EndLogLine;
        //FreeResponsePacket();
        }
      }
  };


struct
LoaderThreadArgs
  {
  ThreadControlBlock * TCB; // not required but convenient
  unsigned long long   mInitialValue;
  unsigned int         mIterations;
  unsigned int         mNumberOfOutstandingLoads;
  unsigned int         mLocalArrayCount;
  unsigned long long ** mBigArrayAnchorAddr;
  };

#define MAX_OUTSTANDING_LOADS (32)

void
LoaderThread( void * args )
  {
  int a;
  int b;
  BegLogLine( TOY_APP )
    << (void*) RunningThreadsTCB << " "
    << "LoaderThread() Entered " << " @a " << (void*) &a << " @b " << (void*) &b << EndLogLine;
  LoaderThreadArgs * LTA = (LoaderThreadArgs *) args;
  assert( LTA );
  unsigned long long *BigArray = *LTA->mBigArrayAnchorAddr;
  BegLogLine( TOY_APP )
    << (void*) RunningThreadsTCB << " "
    << "LoaderThread() "
    << " arg "             << (void*) args
    << " InitValue "       << LTA->mInitialValue
    << " Iterations "      << LTA->mIterations
    << " NumberOfOutstandingLoads " << LTA->mNumberOfOutstandingLoads
    << " LocalArrayCount " << LTA->mLocalArrayCount
    << " BigArrayAnchorAddr " << (void*) LTA->mBigArrayAnchorAddr
    << " BigArray "           << (void*) BigArray
    << EndLogLine;

  unsigned long long Check[ MAX_OUTSTANDING_LOADS ];
  unsigned long long Value[ MAX_OUTSTANDING_LOADS ];

  int NumberOfOutstandingLoads = LTA->mNumberOfOutstandingLoads;
  assert( NumberOfOutstandingLoads > 0 );
  assert( NumberOfOutstandingLoads < MAX_OUTSTANDING_LOADS );

  // Prime the main loop.  Set up connurrent load contexts.
  unsigned int Flags = 0;
  for(int i = 0; i < NumberOfOutstandingLoads; i++ )
    {
    Flags   |= 1<<i;
    Value[i] = BigArray[i];
    BegLogLine( TOY_APP )
      << (void*) RunningThreadsTCB << " "
      << "LoaderThread() "
      << " Initials "             << i
      << " Flags "      << (void*) Flags
      << " Value " << Value[i]
      << EndLogLine;
    }


  int Dispatched = 0;
  int Retired    = 0 - NumberOfOutstandingLoads ;  // loop below will use retire code path to do initial issues

  //for( int l = 0; l < LTA->mIterations; l++ )
    BegLogLine( TOY_APP )
      << (void*) RunningThreadsTCB << " "
      << "LoaderThread() "
      << " Retired " << Retired
      << " Issued " << Dispatched
      << " LTA->mIterations " <<  LTA->mIterations
      << EndLogLine;

  // Move the unsigned into a signed var otherwise while loop isn't entered.
  int  Iterations = LTA->mIterations;
  unsigned int LocalArrayCount = LTA->mLocalArrayCount;
  unsigned long long ** BigArrayAnchorAddr = LTA->mBigArrayAnchorAddr;

  while(Retired < Iterations )
    {
    // Effectively block this coroutine on flags being set - this could be done better with coroutine infrastructure
    //int i = 32;
    ///while( (index = __cntlz4( Flags )) == 32 )

    Yield();

    unsigned int SnapShot = Flags;  // probably not required ... if no call services the network...
    Flags = 0;

    int Bit = 0;
    int BitMask = 1;

    // Handle all data flaged in snapshot
    while( SnapShot != 0 )
      {
      // Get here, we got a bit ... should be cntlz

      while( ! ( SnapShot & BitMask ) )
        {
        Bit++;
        BitMask <<= 1;
        }

      Check[Bit] ^= Value[Bit];
      unsigned int Node  = Value[Bit] / LocalArrayCount;
      unsigned int Index = Value[Bit] % LocalArrayCount;

      BegLogLine( TOY_APP )
        << (void*) RunningThreadsTCB << " "
        << "LoaderThread() "
        << " RETIRED "
        << " Flags "    << (void*) Flags
        << " SnapShot " << (void*) SnapShot
        << " i "        << Bit
        << " Retired "  << Retired
        << " Value "    << Value[Bit]
        << " Issued "   << Dispatched
        << " Node "     << Node
        << " Index "    << Index
        << " Iters "    << Iterations
        << " BitMask "  << BitMask
        << EndLogLine;


      // Turn off the flag in both the SnapShot and the Flags
      SnapShot ^= BitMask;
      //////// now set to 0 after snapshot is taken above Flags    ^= BitMask;

      Retired++;

      if( Dispatched < Iterations )
        {
        GetItemIndirectAsync<unsigned long long>::Fetch( &(Value[Bit]),           // address to store return value
                                                &Flags,            // address of flag word
                                                 Bit,               // bit number is context
                                                 Node,            // Fetch target node
                                                 BigArrayAnchorAddr,  // Fetch target indirect base addr
                                                 Index );         // Fetch target index into array pointed to by base
        Dispatched++;
        BegLogLine( TOY_APP )
          << (void*) RunningThreadsTCB << " "
          << "LoaderThread() "
          << " ISSUED "
          << " Flags "    << (void*) Flags
          << " SnapShot " << (void*) SnapShot
          << " i "        << Bit
          << " Retired "  << Retired
          << " Value "    << Value[Bit]
          << " Issued "   << Dispatched
          << " Node "     << Node
          << " Index "    << Index
          << " Iters "    << Iterations
          << " BitMask "  << BitMask
          << EndLogLine;
        }
      }
    }


  BegLogLine( TOY_APP )
    << (void*) RunningThreadsTCB << " "
    << "LoaderThread() Exiting "
    << " Retired " << Retired
        << " Iters "    << LTA->mIterations
    << EndLogLine;
  }

////  multiple loader thread
const unsigned long long pseudo_a = 4194305LL;
const unsigned long long pseudo_c = 237851704637LL;
const unsigned long long pseudo_m = 1099511627776LL;

static unsigned long long nextRandom(unsigned long long max_value, unsigned long long prev) {
  //
  //   Return the next random number in the sequence
  //
  unsigned long long orig = prev;
  unsigned long long next;


  next = ((prev * pseudo_a) + pseudo_c) % pseudo_m;
  while (next >= max_value) {
    prev = next;
    next = ((prev * pseudo_a) + pseudo_c) % pseudo_m;
  }

  return next;
}

unsigned long long * BigArray = NULL;

void *
GccPkMain(int argc, char ** argv, char ** envp )
  {
  TaskId    = Platform::Topology::GetAddressSpaceId();
  TaskCount = Platform::Topology::GetAddressSpaceCount();


  BegLogLine(TOY_APP)
    << "Main starting TaskId "
    << TaskId
    << " TaskCount "
    << TaskCount
    << EndLogLine;

#if FXLOG_VERBOSE
  int MegabytesPerNode = (1);
#else
  // int MegabytesPerNode = (500);
  int MegabytesPerNode = (1);
#endif

  unsigned long long LocalArrayCount = (MegabytesPerNode * 1024 * 1024) / 8;
  unsigned long long GlobalArrayCount = LocalArrayCount * TaskCount;

#ifdef USE_UNIX_MALLOC
  ///////////unsigned long long * BigArray = (unsigned long long *) malloc( sizeof(unsigned long long) * LocalArrayCount);
  BigArray = (unsigned long long *) malloc( sizeof(unsigned long long) * LocalArrayCount);
#else
  ////////////////unsigned long long * BigArray =
  BigArray =
      (unsigned long long *) Platform::Heap::Allocate( sizeof(unsigned long long) * LocalArrayCount);
#endif

  if (BigArray == NULL)
    {
    printf("could not allocate %lld bytes\n", LocalArrayCount * sizeof( unsigned long long ) );
    exit(2);
    }

  Platform::Control::Barrier();

  BegLogLine(0 == Platform::Topology::GetAddressSpaceId())
    << "Before init"
    << " MegabytesPerNode " << MegabytesPerNode
    << " LocalArrayCount "  << LocalArrayCount
    << " GlobalArrayCount " << GlobalArrayCount
    << " &BigArray "         << (void*)&BigArray
    << " BigArray "         << (void*) BigArray
    << EndLogLine;

  double tstart = GetTimeInSeconds();

  // prime - would be better deterministicly see rands for blocks of mem to support determinism with different partition sizes
  //BigArray[ 0 ] = nextRandom(GlobalArrayCount, (LocalArrayCount * TaskId) );

  srand( TaskId );

  for( unsigned long long index = 0 ; index < LocalArrayCount ; index++)
    {
#if 1
    // this is how it was - numbers that result are not very random using their fx
    BigArray[index] = nextRandom(GlobalArrayCount, (LocalArrayCount * TaskId) + index);
    unsigned long long node = (unsigned long long) (BigArray[index] / TaskCount);
    unsigned long long i    = (unsigned long long) (BigArray[index] % LocalArrayCount);
#else
    // modidied to use last rand to seed next rather than BigArray index
    // stlll terrible BigArray[index] = nextRandom(GlobalArrayCount, BigArray[ index - 1 ] );
    //BigArray[index] = ((((unsigned long long)rand())<<31) + rand()) % GlobalArrayCount;
    unsigned long long node = (unsigned long long) (rand() * ((TaskCount-1)/(RAND_MAX * 1.0)));
    unsigned long long i    = (unsigned long long) (rand() * ((LocalArrayCount-1)/(RAND_MAX * 1.0)));
    assert( node < TaskCount );
    assert( i    < LocalArrayCount );
    BigArray[ index ] = node * LocalArrayCount + i ;
#endif
    assert( BigArray[ index ] >= 0 );
    assert( BigArray[ index ] < GlobalArrayCount );
    BegLogLine( 0 )
      << "Rand index " << index << " value " << BigArray[ index ] << " node " << node << " i " << i
      << EndLogLine;
    }

  double tend = GetTimeInSeconds();
  Platform::Control::Barrier();

  BegLogLine(0 == Platform::Topology::GetAddressSpaceId())
    << "random init (secs) = "
    <<  tend - tstart
    << EndLogLine;

  //unsigned long long search_value = nextRandom(GlobalArrayCount, 1);
  unsigned long long search_value = (unsigned long long) (rand() * (LocalArrayCount/(RAND_MAX * 1.0)));
  int search_count = 0;
  Platform::Control::Barrier();

  tstart = GetTimeInSeconds();

  for(unsigned long long index = 0 ; index < LocalArrayCount ; index++)
    {
    if( BigArray[ index ] == search_value )
      search_count ++;
    }

  Platform::Control::Barrier();
  tend = GetTimeInSeconds();

  BegLogLine(0 == Platform::Topology::GetAddressSpaceId())
    << "search (secs) = "
    <<  tend - tstart
    << " count "
    << search_count
    << " node mb/s "
    << ((LocalArrayCount*8) / (tend - tstart))/(1024*1024)
    << " global mb/s "
    << ((GlobalArrayCount*8) / (tend - tstart))/(1024*1024)
    << EndLogLine;

#if FXLOG_VERBOSE
  unsigned long long IterationsPerReadNode = 32;
#else
  unsigned long long IterationsPerReadNode = 1024 * 1024;
#endif

  unsigned long long MaxReadThreadsPerNode = 32;

// The creates a ThreadControlBlock for main and sets it to be the one running.
InitThreadSystem();

#ifdef USE_UNIX_MALLOC
  LoaderThreadArgs * LTA = (LoaderThreadArgs *) malloc( MaxReadThreadsPerNode * sizeof( LoaderThreadArgs ) );
#else
  LoaderThreadArgs * LTA = (LoaderThreadArgs *) Platform::Heap::Allocate( MaxReadThreadsPerNode * sizeof( LoaderThreadArgs ) );
#endif
  if( LTA == NULL )
    PLATFORM_ABORT("LoaderThreadArgs failed to malloc");

  for(int GlobalNodesReading = 1;
          GlobalNodesReading <= TaskCount;
          GlobalNodesReading *= 2 )
    {
    BegLogLine(0)
      << " GlobalNodesReading "
      << GlobalNodesReading
      << EndLogLine;

    for(int ReadThreadsPerNode = 1;
            ReadThreadsPerNode <= MaxReadThreadsPerNode;
            ReadThreadsPerNode ++ )
      {
      BegLogLine(0)
        << "ReadThreadsPerNode "
        << ReadThreadsPerNode
        << EndLogLine;

      ////int local_threads_this_time = ReadThreadsPerNode;
      int local_threads_this_time = 1; // one thread... multi contexts

      // knock out the chips that aren't yet participating
      if( 0 != TaskId % (TaskCount / GlobalNodesReading) )
        local_threads_this_time = 0;

      unsigned long long TrueIterationsPerReadNode = 0LL;

      for( unsigned long long index = 0; index < local_threads_this_time; index++ )
        {
        LTA[index].mInitialValue       = BigArray[ index ];
        LTA[index].mNumberOfOutstandingLoads = ReadThreadsPerNode;
        LTA[index].mIterations         = IterationsPerReadNode;
        TrueIterationsPerReadNode     += LTA[index].mIterations;
        LTA[index].mBigArrayAnchorAddr = &BigArray;
        LTA[index].mLocalArrayCount    = LocalArrayCount;
        }

      BegLogLine( TOY_APP )
        << "CALL "
        << "TotalNodes: "      << TaskCount
        << " MemPerNode(MB): " << MegabytesPerNode
        << " ReadNodes: "      << GlobalNodesReading
        << " ReadsPerNode: "   << IterationsPerReadNode
        << " ThrPerReadNode "  << ReadThreadsPerNode
        << EndLogLine;


      ThreadControlBlock* tcb[ 256 ];
      if( local_threads_this_time > 256 ) PLATFORM_ABORT(" too many threads for a local buffer");

      for( unsigned index = 0; index < local_threads_this_time; index++ )
        {
        tcb[index] = CreateThread( 128*1024, LoaderThread, (void *) & (LTA[index]) );
        BegLogLine( TOY_APP ) << "Created Thread " << (void*) tcb[index] << EndLogLine;
        }

      BegLogLine( TOY_APP ) << "Done with all creates" << EndLogLine;

      tstart = GetTimeInSeconds();

      for( unsigned index = 0; index < local_threads_this_time; index++ )
        {
        BegLogLine( TOY_APP ) << "About to join " << index << " ctx " << (void*) tcb[index] << EndLogLine;

        Join( tcb[ index ] );

        BegLogLine( TOY_APP ) << "Finished join " << index << " ctx " << (void*) tcb[index] << EndLogLine;
        }

      BegLogLine( TOY_APP ) << "Done with all joins" << EndLogLine;

//      OuterLoopControl.Wait();
PkCo_Barrier();

      tend = GetTimeInSeconds();

      BegLogLine(0 == Platform::Topology::GetAddressSpaceId())
        << "RESULTS MemGet "
        << "TotalNodes: "      << TaskCount
        << " MemPerNode(MB): " << MegabytesPerNode
        << " ReadNodes: "      << GlobalNodesReading
        << " ReadsPerNode: "   << TrueIterationsPerReadNode
        << " ThrPerReadNode "  << ReadThreadsPerNode
        << " time "            << tend - tstart
        << " rate GL/S"        << ((GlobalNodesReading * TrueIterationsPerReadNode)/(tend - tstart))/(1024.0*1024.0*1024.0)
        << EndLogLine;

      }
    }
  BegLogLine(FXLOG_VERBOSE)
    << "PkMain() Exiting"
    << EndLogLine;
//  PLATFORM_ABORT(" PROGRAM FINISHED CORRECTLY - BRING DOWN PARTITION");
  return( NULL );
  }

