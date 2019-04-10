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
 #ifndef __PK_FASCHED_INLINE_CPP__
#define __PK_FASCHED_INLINE_CPP__

#include <Pk/FxLogger.hpp>
#include <Pk/TreeReduce.hpp>

#define PK_NUMBER_OF_POOL_PACKETS (40*1024)


#ifndef KICKPIPES_SYNC_ON_STATUS
#define KICKPIPES_SYNC_ON_STATUS ( 1 )
#endif

#ifndef PKFXLOG_BGL_FASCHED_ALL
#define PKFXLOG_BGL_FASCHED_ALL ( 0 )
#endif

#ifndef PKFXLOG_BGL_TORUS_HI_FREQ
#define PKFXLOG_BGL_TORUS_HI_FREQ ( 0 )
#endif

#ifndef PKFXLOG_BGL_SCHED_HI_FREQ
#define PKFXLOG_BGL_SCHED_HI_FREQ ( 0 )
#endif

#ifndef PKFXLOG_BGL_ACTOR
#define PKFXLOG_BGL_ACTOR ( 0 || PKFXLOG_BGL_FASCHED_ALL  )
#endif

#ifndef PKFXLOG_BGL_CHANNEL
#define PKFXLOG_BGL_CHANNEL ( 0 || PKFXLOG_BGL_FASCHED_ALL  )
#endif

#ifndef PKFXLOG_BGL_TORUS
#define PKFXLOG_BGL_TORUS ( 0 || PKFXLOG_BGL_FASCHED_ALL  )
#endif

#ifndef PKFXLOG_PK_FIBER
#define PKFXLOG_PK_FIBER ( 0 || PKFXLOG_BGL_FASCHED_ALL  )
#endif

// Algorithm selection for send FIFO chooser
#ifndef PK_SEND_BY_GROUP
#define PK_SEND_BY_GROUP ( 1 )
#endif

//*****************************************************************************
// BEGIN REACTIVE PACKET INLINE CODE
//*****************************************************************************
#include <Pk/Heap.hpp>
#include <Pk/ArchConfig.hpp>
#include <BonB/BGLTorusAppSupport.h>

#include <assert.h>

static void PkFiberYield();  // forward dcl - should be in a header somewhere

struct pkFiberControlBlockT;

static int MakeMeAFunctionAddr(void*) {return(0);}

typedef int (*ActorFunctionType)(void * payload);
#define SpecialActorFunction_LeaveInFiberBuffer ((ActorFunctionType)(5 & (unsigned)MakeMeAFunctionAddr))

enum {
     PkPacketActorProtocol_SrcCtxInvalid = 0,
     PkPacketActorProtocol_SrcCtxValid   = 1
     };

enum {
     PkPacketNonActorProtocol_Channel    = 0,
     PkPacketNonActorProtocol_Shmem      = 1
     };


typedef union
  {
  unsigned long mWord;

  struct {
         unsigned mNonActorFlag     :  1 ; // if this bit is zero (0) then we got an actor packet
         unsigned mActorProtocol    :  1 ; // set to 1 if valid src and context packed header
         unsigned mActorFx          : 22 ; // bg/l function address
         unsigned mData             :  8 ; // this is CtxHigh for a SrcCtx packet ( see header word 1 )
         } Actor;

  struct {
         unsigned mNonActorFlag     :  1 ; // if this bit is zero (0) then we got an actor packet
         unsigned mNonActorProtocol :  1 ; // set to 1 if valid src and context packed header
         unsigned mActorFx          : 22 ; // bg/l function address
         unsigned mContextLow       :  8 ; // this is CtxHigh for a SrcCtx packet ( see header word 1 )
         } ActorSrcCtx;

  struct {
         unsigned mNonActorFlag     :  1 ; // bit must be set to 1 to be non actor protocol
         unsigned mNonActorProtocol :  1 ; // right now, just for syste protocols channels and dma em
         unsigned mData             : 30 ; // bits free for protocol
         } NonActor;

  // This break out could be done as part of a handler lib using the NonActor format and not as part of the FA packet header.
  struct {
         unsigned mNonActorFlag     :  1 ; // if this bit is zero (0) then we got an actor packet
         unsigned mNonActorProtocol :  1 ; // set to 1 if valid src and context packed header
         unsigned mItemCount        :  8 ; // in a data packet, this is the number of items in the packet
                                           // mItemCount only needs to be the max number of items in the last packet - 5 bits for 32 bytes
         unsigned mSeqNo            : 22 ; // bits left to be a packet SeqNo
                                           // NOTE: an ack packet is signified by being sent to a receiver w/ ItemCount 0xFF
                                           // NOTE: other values of mItemCount will be used for multipacket items and for delimitting variable length items
         } Channel;

  } PktHdrW0;


typedef union
  {
  unsigned long mWord;

  struct {
         unsigned mData : 32 ; // probably could be a lot few bits
         }Actor;

  struct {
         unsigned mContextHigh     : 12 ; // "massaged" Active Function Pointer (contxt low is 8 data bits in w1)
         unsigned mSrcCore         :  2 ; // good through the P time frame
         unsigned mSrcNode         : 18 ; // bits required for 1KB aligned threads within 512MB
         }ActorSrcCtx;

  struct {
         unsigned mChannelTerminalAddr : 32 ; // probably could be a lot few bits
         }Channel;

  } PktHdrW1;

static void
UnloadChannelHeaderWords( unsigned long        aW0,
                          unsigned long        aW1,
                          unsigned long*       aItemCount,
                          unsigned long*       aSeqNo,
                          ChannelTerminal** aChannelTerminalPtrPtr )
  {
  PktHdrW0 w0;
  PktHdrW1 w1;
  w0.mWord = aW0;
  w1.mWord = aW1;

  AssertLogLine( w0.Channel.mNonActorFlag == 1 )
    << "UnloadChannelHeaderWords():"
    << " NonActorFlag "             << w0.Channel.mNonActorFlag
    << " Protocol "                 << w0.Channel.mNonActorProtocol
    << " aW0 "                      << (void*) aW0
    << " aW1 "                      << (void*) aW1
    << " ChannelProtocol bits are " << (void*)PkPacketNonActorProtocol_Channel
    << EndLogLine;

  *aItemCount             = w0.Channel.mItemCount;
  *aSeqNo                 = w0.Channel.mSeqNo;
  *aChannelTerminalPtrPtr = (ChannelTerminal*) w1.Channel.mChannelTerminalAddr;
  }

static void
LoadChannelHeaderWords( unsigned long      & aW0,
                        unsigned long      & aW1,
                        unsigned long        aItemCount,
                        unsigned long        aSeqNo,
                        ChannelTerminal*  aChannelTerminalPtr )
  {
  PktHdrW0 w0;
  PktHdrW1 w1;

  w0.Channel.mNonActorFlag        = 1; // set not actor flag
  w0.Channel.mNonActorProtocol    = PkPacketNonActorProtocol_Channel;
  w0.Channel.mItemCount           = aItemCount;
  w0.Channel.mSeqNo               = aSeqNo;
  w1.Channel.mChannelTerminalAddr = (unsigned) aChannelTerminalPtr;

  aW0 = w0.mWord;
  aW1 = w1.mWord;

  BegLogLine(PKFXLOG_BGL_CHANNEL)
    << "LoadChannelHeaderWords():"
    << " W0 "        << (void*) aW0
    << " W1 "        << (void*) aW1
    << " NonActorFlag "  << (void*) w0.Channel.mNonActorFlag
    << " Protocol "  << (void*) w0.Channel.mNonActorProtocol
    << " ItemCount " << (void*) w0.Channel.mItemCount
    << " SeqNo "     << (void*) w0.Channel.mSeqNo
    << " TermAddr "  << (void*) w1.Channel.mChannelTerminalAddr
    << EndLogLine;

  #ifndef NDEBUG
  unsigned long        ackf;
  unsigned long        seqno;
  ChannelTerminal*  addr;

  UnloadChannelHeaderWords( aW0, aW1, &ackf, &seqno, &addr );

  AssertLogLine( ackf == aItemCount )
    << "LoadChannelHeaderWords():"
    << " ackf "        << (void*) ackf
    << " aItemCount "  << (void*) aItemCount
    << EndLogLine;

  AssertLogLine( seqno == aSeqNo )
   << " seqno "        << (void*) seqno
   << " aSeqNo "       << (void*) aSeqNo
   << EndLogLine;

  AssertLogLine( addr == aChannelTerminalPtr )
    << "LoadChannelHeaderWords():"
    << " addr "                 << (void*) addr
    << " aChannelTerminalAddr " << (void*) aChannelTerminalPtr
    << EndLogLine;

  #endif

  }

static
inline
unsigned
_pkDownShiftActorFx( ActorFunctionType aActorFx )
  {
  unsigned rc = (((unsigned) aActorFx) & 0x00FFFFFF) >> 2;
  return( rc );
  }

static
inline
ActorFunctionType
_pkUpShiftActorFx( unsigned aActorFx )
  {
  unsigned long ActorFx = (unsigned long) aActorFx;
  unsigned long ThisFx  = (unsigned long) _pkUpShiftActorFx;
  ActorFunctionType af  = (ActorFunctionType) ((ActorFx << 2) | (ThisFx & 0xFF000000));
  return( af );
  }


static
void
LoadActorHeaderSrcCtxWords( unsigned long      & aW0,
                            unsigned long      & aW1,
                            unsigned long        aSourceNode,
                            unsigned long        aSourceCore,
                            ActorFunctionType    aActorFx,
                            pkFiberControlBlockT * aContext)
  {
  PktHdrW0 w0;
  PktHdrW1 w1;

  w0.Actor.mNonActorFlag   = 0;  // zero means it's an actor
  w0.Actor.mActorProtocol  = PkPacketActorProtocol_SrcCtxValid;  // src ctx set

  AssertLogLine( ! ((unsigned)aContext & 0xC00003FF) )
    << "LoadActorHeaderSrcCtxWords():"
    << " aContext has bits in masked areas " << (void*) aContext
    << EndLogLine;

  w0.Actor.mActorFx        = _pkDownShiftActorFx( aActorFx );

  unsigned FiberContext  = ((unsigned) aContext) >> 10; // Fiber context doesn't use low bits
  int ContextHigh  = ((unsigned) FiberContext) >> 8;
  int ContextLow   = ((unsigned) FiberContext) & 0x000000FF;

  w0.ActorSrcCtx.mContextLow    = ContextLow;
  w1.ActorSrcCtx.mContextHigh   = ContextHigh;

  w1.ActorSrcCtx.mSrcNode       = aSourceNode;
  w1.ActorSrcCtx.mSrcCore       = aSourceCore;

  aW0 = w0.mWord;
  aW1 = w1.mWord;

  #if 0 /// we no longer have a reciprocal function
  #ifndef NDEBUG // when debugging, double check that we can unload whatever was loaded without loss
  unsigned long        srcn;
  unsigned long        srcc;
  ActorFunctionType    fx;
  pkFiberControlBlockT * ctx;

  UnloadActorHeaderSrcCtxWords( aW0, aW1, &srcn, &srcc,  &fx, &ctx);

  AssertLogLine( srcn      == aSourceNode )
    << "LoadActorHeaderSrcCtxWords():"
    << " srcn "        << (void*) srcn
    << " aSourceNode " << (void*) aSourceNode
    << EndLogLine;

  AssertLogLine( srcc      == aSourceCore )
   << " srcc "        << (void*) srcc
   << " aSourceCore " << (void*) aSourceCore
   << EndLogLine;

  AssertLogLine( aActorFx == fx      )
    << "LoadActorHeaderSrcCtxWords():"
    << " aActorFx " << (void*)aActorFx
    << " fx " << (void*)fx
    << EndLogLine;

  AssertLogLine( aContext == ctx )
    << "LoadActorHeaderSrcCtxWords():"
    << " aContext " << (void*) aContext
    << " ctx "      << (void*) ctx
    << EndLogLine;
  #endif
  #endif

  }

// Packet pointers are turned over to users as pointers to payload - this figures out where the packet buffer begins
inline
pkFullPacketBuffer*
pkActorPacketPointerFromPayloadPointer( void* aPacketPayloadPtr )
  {
  pkFullPacketBuffer x;
  unsigned addr_offs =   ((unsigned)(&x.mTorusPacket.payload)) - ((unsigned)&x);
  pkFullPacketBuffer* APB = (pkFullPacketBuffer*) ((unsigned)aPacketPayloadPtr - addr_offs);
  return( APB );
  }


static
void
PkActorSetContextWord( void* aPktPayloadPtr, unsigned aContextValue )
  {
  pkFullPacketBuffer* FPB =
    pkActorPacketPointerFromPayloadPointer( aPktPayloadPtr );

  PktHdrW0 *w0Ptr;
  PktHdrW1 *w1Ptr;

  w0Ptr = (PktHdrW0 *) &(FPB->mTorusPacket.hdr.swh0.arg0);
  w1Ptr = (PktHdrW1 *) &(FPB->mTorusPacket.hdr.swh1.arg);

  StrongAssertLogLine( w0Ptr->Actor.mNonActorFlag   == 0) << " Must be an actor packet format w0 " << (void*)FPB->mTorusPacket.hdr.swh0.arg0 << " w1 " << (void*)FPB->mTorusPacket.hdr.swh1.arg << EndLogLine

  // clear src/ctx flag
  w0Ptr->Actor.mActorProtocol = PkPacketActorProtocol_SrcCtxInvalid;  // src ctx set

  w1Ptr->mWord = aContextValue;
  }

static
unsigned int
PkActorGetContextWord()
  {
  // this is only valid to call from within an actor function and this should be checked.
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  StrongAssertLogLine( fascb->mCurrentActorPacketPtr != NULL )
    << "PkActorGetContextWord():"
    << " called but Fiber Control Block has null current packet pointer"
    << " FASCB " << (void*) fascb
    << EndLogLine;

  PktHdrW0 w0;
  w0.mWord = fascb->mCurrentActorPacketPtr->hdr.swh0.arg0;

  PktHdrW1 w1;
  w1.mWord = fascb->mCurrentActorPacketPtr->hdr.swh1.arg;

  AssertLogLine( w0.Actor.mNonActorFlag == 0 && w0.Actor.mActorProtocol == PkPacketActorProtocol_SrcCtxInvalid )
    << "PkActorGetContextWord():"
    << " header words wrong "
    << " NonActorFlag " <<  w0.Actor.mNonActorFlag
    << " Protocol "     <<  w0.Actor.mActorProtocol
    << " w0 " << (void*) w0.mWord
    << " w1 " << (void*) w1.mWord
    << EndLogLine;

  return( w1.mWord );
  }


static
void
PkActorGetPacketSourceContext( unsigned long         * aSourceNode,
                               unsigned long         * aSourceCore,
                               pkFiberControlBlockT ** aContext = NULL)
  {
  // this is only valid to call from within an actor function and this should be checked.
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  AssertLogLine( fascb->mCurrentActorPacketPtr != NULL )
    << "PkActorGetPacketSourceContext():"
    << " called but Fiber Control Block has null current packet pointer"
    << " FASCB " << (void*) fascb
    << EndLogLine;

  PktHdrW0 w0;
  w0.mWord = fascb->mCurrentActorPacketPtr->hdr.swh0.arg0;

  PktHdrW1 w1;
  w1.mWord = fascb->mCurrentActorPacketPtr->hdr.swh1.arg;

  AssertLogLine( w0.Actor.mNonActorFlag == 0 && w0.Actor.mActorProtocol == PkPacketActorProtocol_SrcCtxValid )
    << "PkActorGetPacketSourceContext():"
    << " header words wrong "
    << " NonActorFlag " <<  w0.Actor.mNonActorFlag
    << " Protocol "     <<  w0.Actor.mActorProtocol
    << " w0 " << (void*) w0.mWord
    << " w1 " << (void*) w1.mWord
    << EndLogLine;

  *aSourceNode          = w1.ActorSrcCtx.mSrcNode;
  *aSourceCore          = w1.ActorSrcCtx.mSrcCore;

  if( aContext != NULL )
    {
    unsigned Context = (((w1.ActorSrcCtx.mContextHigh << 8) | w0.ActorSrcCtx.mContextLow)) << 10;
    *aContext = (pkFiberControlBlockT*) Context;
    }
  }


#include <Pk/Topology.hpp>

//////////////////////////////////////////////////////////////////////
// Official Packet Buffer Pool - used for received packets and maybe send packets
//////////////////////////////////////////////////////////////////////
// Free a pool packet
static int FreesAfterAlloc = 0;
static inline
void
pkActorPacketPoolFree( pkFiberActorSchedulerControlBlockT* fascb, pkFullPacketBuffer *aPacket)
  {
    BegLogLine( PKFXLOG_BGL_ACTOR && FreesAfterAlloc )
    // BegLogLine( 1 )
    << "pkActorPacketPoolFree():"
    << " aPacket "  << (void*)aPacket
    << " PoolHead " << fascb->mActorPacketFreePoolHead
    << EndLogLine;
#ifndef NDEBUG
  for(int i = 0; i < sizeof( pkFullPacketBuffer ); i++)
    ((char*)aPacket)[i] = 0xFE;
#endif
  aPacket->mPBCB.mNext                  = fascb->mActorPacketFreePoolHead;
  fascb->mActorPacketFreePoolHead = aPacket;
  }


static inline
pkFullPacketBuffer*
pkActorPacketPoolAllocate( pkFiberActorSchedulerControlBlockT* fascb )
  {
//   #if PKFXLOG_BGL_ACTOR
   FreesAfterAlloc = 1;
//   #endif

  if( fascb->mActorPacketFreePoolHead == NULL )
    {
    // If you get in here, send fifos are full AND packet buffers are full.
    static int WarnOnceOnPacketPoolEmpty = 0;
//     BegLogLine( (WarnOnceOnPacketPoolEmpty++ % 100000) == 0 )
//       << "pkActorPacketPoolAllocate():"
//       << "******** WARNING PACKET POOL EMPTY ************"
//       << EndLogLine;
    return( NULL );
    }

  pkFullPacketBuffer* Packet      = fascb->mActorPacketFreePoolHead;

  fascb->mActorPacketFreePoolHead = Packet->mPBCB.mNext;

  // zero the header region of the packet  NEED TO DO THIS MORE EXACTLY
  long long *p = (long long*) Packet;
  p[0] = p[1] = p[2] = p[3] = 0;

  Packet->mPBCB.mStorageClass = pkPacketBufferControlBlock::StorageClass_BufferPool;

  BegLogLine( PKFXLOG_BGL_ACTOR )
  // BegLogLine( 1 )
    << "pkActorPacketPoolAllocate():"
    << " Allocated "
    << " Pkt@ "                << (void*) Packet
    << " ActorPacketPoolHead " << (void*) fascb->mActorPacketFreePoolHead
    << " PoolHead->mPBCB.mNext "     << (void*) fascb->mActorPacketFreePoolHead->mPBCB.mNext
    << EndLogLine;

// think through code paths to see if these are useful
//Packet->mPBCB.mNext = NULL;
//Packet->mPBCB.mPrev = NULL;

  return( Packet );
  }


 inline
pkFullPacketBuffer*
PkActorPacketPoolAllocate()
  {
  // NOTE: this is an actor asking for the scheduler on the core it's running on
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  while( fascb->mActorPacketFreePoolHead == NULL )
    {
    // If you get in here, send fifos are full AND packet buffers are full.
    static int WarnOnceOnPacketPoolEmpty = 0;
    BegLogLine( (WarnOnceOnPacketPoolEmpty++ % 10000) == 0 )
      << "PkActorPacketPoolAllocate():"
      << "******** BLOCKED WARNING PACKET POOL EMPTY ************"
      << EndLogLine;
    PkFiberYield();  // Needs to be a block
    }

  pkFullPacketBuffer* Packet = pkActorPacketPoolAllocate( fascb );

  return( Packet );
  }

static inline
void
pkChannelPacketMakeHeader( pkFullPacketBuffer* aPacket,
                           ChannelTerminal&     aChannelTerminal,
                           unsigned int         aItemCount,
                           unsigned int         aSeqNo,
                           unsigned int         aPayloadSize )
  {

  aPacket->mPBCB.mSelfSend = ( PkNodeGetId() == aChannelTerminal.mNode ) ;
//  if( PkNodeGetId() == aChannelTerminal.mNode )
//    aPacket->mPBCB.mSelfSend = 1;
//  else
//    aPacket->mPBCB.mSelfSend = 0;

  unsigned long w0;
  unsigned long w1;

  BegLogLine(PKFXLOG_BGL_CHANNEL)
    << "pkChannelPacketMakeHeader():"
    << " aPacket "          << (void*)aPacket
    << " aChannelTerminal " << (void*) & aChannelTerminal
    << " aItemCount "       << aItemCount
    << " aSeqNo "           << aSeqNo
    << " aPayloadSize "     << aPayloadSize
    << EndLogLine;

  StrongAssertLogLine( aPayloadSize <= 240 )
    << "pkChannelPacketMakeHeader():"
    << " aPacket "          << (void*)aPacket
    << " aChannelTerminal " << (void*) & aChannelTerminal
    << " aItemCount "       << aItemCount
    << " aSeqNo "           << aSeqNo
    << " aPayloadSize "     << aPayloadSize
    << EndLogLine;

  LoadChannelHeaderWords( w0, w1, aItemCount, aSeqNo, aChannelTerminal.mAddr );

//  // This is nuts - we drag in the XYZ class for no reason.
//  TorusXYZ ThereXYZ  = PkTopo3DMakeXYZ( aChannelTerminal.mNode );
//
//  BGLTorusMakeHdrAppChooseRecvFifo( // this method picks a core as it's last arg
//                       &(aPacket->mTorusPacket.hdr),
//                       ThereXYZ.mX, ThereXYZ.mY, ThereXYZ.mZ,
//                       w0, w1,
//                       aPayloadSize, // Payload in bytes
//                       aChannelTerminal.mCore );  // core (fifo group) on dest

  int ThereX = pkNode.NodeXYZ.TargetX(aChannelTerminal.mNode) ;
  int ThereY = pkNode.NodeXYZ.TargetY(aChannelTerminal.mNode) ;
  int ThereZ = pkNode.NodeXYZ.TargetZ(aChannelTerminal.mNode) ;

  BGLTorusMakeHdrAppChooseRecvFifo( // this method picks a core as it's last arg
                       &(aPacket->mTorusPacket.hdr),
                       ThereX, ThereY, ThereZ,
                       w0, w1,
                       aPayloadSize, // Payload in bytes
                       aChannelTerminal.mCore );  // core (fifo group) on dest

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkChannelPacketMakeHeader():"
    << " aItemCount "           << aItemCount
    << " aSeqNo "               << aSeqNo
    << " aChannelTerminalAddr[ "
    << " mNode "                << aChannelTerminal.mNode
    << " mCore "                << aChannelTerminal.mCore
    << " mAddr "                << aChannelTerminal.mAddr
    << " ] "
    << " PayloadSize "          << aPayloadSize
    << " Pkt@ "         << (void*) aPacket
    << " hwh0 "         << (void*) w0
    << " hwh1 "         << (void*) w1
//    << " There.mX "     << (int)ThereXYZ.mX << " mY " << (int)ThereXYZ.mY << " mZ " << (int)ThereXYZ.mZ
    << " ThereX "       << ThereX << " ThereY " << ThereY << " ThereZ " << ThereZ
    << EndLogLine;
  }

static inline
int
pkChannelPacketGetHeaderSeqNo( pkFullPacketBuffer* aPacket )
  {
  StrongAssertLogLine( aPacket != NULL )
    << "pkChannelPacketGetHeaderSeqNo():"
    << " called on NULL packet pointer."
    << EndLogLine;
  // little lack of layering here that could be better - reach in and access item count in packet header
  PktHdrW0 w0;
  w0.mWord  = aPacket->mTorusPacket.hdr.swh0.arg0;

  int SeqNo = w0.Channel.mSeqNo;

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkChannelPacketGetHeaderSeqNo():"
    << " aPacket "    << (void*)aPacket
    << " Pkt SeqNo "   << SeqNo
    << " SlotCnt "   << w0.Channel.mItemCount
    << EndLogLine;

  return( SeqNo );
  }

static inline
int
pkChannelPacketGetHeaderItemCount( pkFullPacketBuffer* aPacket )
  {
  // little lack of layering here that could be better - reach in and access item count in packet header
  PktHdrW0 w0;
  w0.mWord  = aPacket->mTorusPacket.hdr.swh0.arg0;

  int ItemCount = w0.Channel.mItemCount;

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkChannelPacketGetHeaderItemCount():"
    << " aPacket "    << (void*)aPacket
    << " SlotCnt "   << w0.Channel.mItemCount
    << " SeqNo "      << w0.Channel.mSeqNo
    << EndLogLine;

  return( ItemCount );
  }

static inline
void
pkChannelPacketIncrementHeaderItemCount( pkFullPacketBuffer* aPacket )
  {
  //PktHdrW0 *w0 = PktHdrW0
  //w0.mWord  = aPacket->mTorusPacket.hdr.swh0.arg0;

  //int ItemCount = w0.Channel.mItemCount;
  // little lack of layering here that could be better - reach in and access item count in packet header
  ((PktHdrW0*)&aPacket->mTorusPacket.hdr.swh0.arg0)->Channel.mItemCount ++;

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkChannelPacketIncrementHeaderItemCount():"
    << " aPacket "    << (void*)aPacket
    << " SlotCnt "  << ((PktHdrW0*)&aPacket->mTorusPacket.hdr.swh0.arg0)->Channel.mItemCount
    << " SeqNo "      << ((PktHdrW0*)&aPacket->mTorusPacket.hdr.swh0.arg0)->Channel.mSeqNo
    << EndLogLine;

  return;
  }

static inline
void
pkActorPacketMakeHeader( pkFullPacketBuffer*     aPacket,
                         unsigned long           aDestNode,
                         unsigned long           aDestCore,
                         pkFiberControlBlockT*   aContext,
                         ActorFunctionType       aActorFx,
                         unsigned long           aPayloadSize )
  {
  AssertLogLine( aDestNode >= 0 && aDestNode < PkNodeGetCount() )
    << "aDestNode " << aDestNode
    << " out of range. "
    << EndLogLine;

  AssertLogLine( aDestCore >= 0 && aDestCore < PkCoreGetCoresPerNodeCount() )
    << "aDestCore " << aDestCore
    << " out of range. "
    << EndLogLine;
  aPacket->mPBCB.mSelfSend = ( PkNodeGetId() == aDestNode ) ;

  unsigned long w0;
  unsigned long w1;

  LoadActorHeaderSrcCtxWords( w0, w1, PkNodeGetId(), PkCoreGetId(), aActorFx, aContext );

  int ThereX = pkNode.NodeXYZ.TargetX(aDestNode) ;
  int ThereY = pkNode.NodeXYZ.TargetY(aDestNode) ;
  int ThereZ = pkNode.NodeXYZ.TargetZ(aDestNode) ;

  BGLTorusMakeHdrAppChooseRecvFifo( // this method picks a core as it's last arg
                       &(aPacket->mTorusPacket.hdr),
                       ThereX, ThereY, ThereZ,
                       w0, w1,
                       aPayloadSize, // Payload in bytes
                       aDestCore );  // core (fifo group) on dest

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkActorPacketMakeHeader():"
    << " aPacket "     << aPacket
    << " aDestNode "   << aDestNode
    << " aDestCore "   << aDestCore
    << " Context "     << (void*) aContext
    << " ActorFx "     << (void*) aActorFx
    << " PayloadSize " << aPayloadSize
    << " Pkt@ "         << (void*) aPacket
    << " hwh0 "         << (void*) w0
    << " hwh1 "         << (void*) w1
    << " ThereX "       << ThereX << " ThereY " << ThereY << " ThereZ " << ThereZ
    << EndLogLine;
  }

static inline
void
pkActorPacketMakeHeaderVirtualSource( pkFullPacketBuffer*     aPacket,
                         unsigned long           aDestNode,
                         unsigned long           aDestCore,
                         pkFiberControlBlockT*   aContext,
                         ActorFunctionType       aActorFx,
                         unsigned long           aPayloadSize,
                         unsigned int            aVirtualSourceNode)
  {
  AssertLogLine( aDestNode >= 0 && aDestNode < PkNodeGetCount() )
    << "aDestNode " << aDestNode
    << " out of range. "
    << EndLogLine;

  AssertLogLine( aDestCore >= 0 && aDestCore < PkCoreGetCoresPerNodeCount() )
    << "aDestCore " << aDestCore
    << " out of range. "
    << EndLogLine;
  aPacket->mPBCB.mSelfSend = ( PkNodeGetId() == aDestNode ) ;

  unsigned long w0;
  unsigned long w1;

  LoadActorHeaderSrcCtxWords( w0, w1, aVirtualSourceNode, PkCoreGetId(), aActorFx, aContext );

  int ThereX = pkNode.NodeXYZ.TargetX(aDestNode) ;
  int ThereY = pkNode.NodeXYZ.TargetY(aDestNode) ;
  int ThereZ = pkNode.NodeXYZ.TargetZ(aDestNode) ;

  BGLTorusMakeHdrAppChooseRecvFifo( // this method picks a core as it's last arg
                       &(aPacket->mTorusPacket.hdr),
                       ThereX, ThereY, ThereZ,
                       w0, w1,
                       aPayloadSize, // Payload in bytes
                       aDestCore );  // core (fifo group) on dest

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkActorPacketMakeHeaderVirtualSource():"
    << " aPacket "     << aPacket
    << " aDestNode "   << aDestNode
    << " aDestCore "   << aDestCore
    << " Context "     << (void*) aContext
    << " ActorFx "     << (void*) aActorFx
    << " PayloadSize " << aPayloadSize
    << " VirtualSourceNode " << aVirtualSourceNode
    << " Pkt@ "         << (void*) aPacket
    << " hwh0 "         << (void*) w0
    << " hwh1 "         << (void*) w1
    << " ThereX "       << ThereX << " ThereY " << ThereY << " ThereZ " << ThereZ
    << EndLogLine;
  }



template<typename tPayloadType>
inline
tPayloadType*
PkOpenPayloadBay( pkFullPacketBuffer* aPkt )
  {
  AssertLogLine( sizeof(tPayloadType) < 240)
    << "PkOpenPayloadBay():"
    << " Payload type too large for packet payload"
    << " size " << sizeof( tPayloadType )
    << EndLogLine;

  return( (tPayloadType*) & aPkt->mTorusPacket.payload );
  }


inline
void*
PkActorReservePacketNonblocking(   unsigned long         aDestNode,
                                   unsigned long         aDestCore,
                                   ActorFunctionType     aActorFx,
                                   unsigned long          aPayloadSize)
  {
  // the dereference could carry through to the next method.
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  pkFullPacketBuffer *ResPkt = pkActorPacketPoolAllocate( fascb );

  if( ResPkt == NULL )
    return( NULL );

  pkActorPacketMakeHeader( ResPkt,
                           aDestNode,
                           aDestCore,
                           fascb->RunningFibersFCB,
                           aActorFx,
                           aPayloadSize );

  return( (void*) & ResPkt->mTorusPacket.payload );
  }

inline
void*
PkActorReservePacketNonblockingVirtualSource(   unsigned long         aDestNode,
                                   unsigned long         aDestCore,
                                   ActorFunctionType     aActorFx,
                                   unsigned long          aPayloadSize,
                                   unsigned int         aVirtualSourceNode)
  {
  // the dereference could carry through to the next method.
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  pkFullPacketBuffer *ResPkt = pkActorPacketPoolAllocate( fascb );

  if( ResPkt == NULL )
    return( NULL );

  pkActorPacketMakeHeaderVirtualSource( ResPkt,
                           aDestNode,
                           aDestCore,
                           fascb->RunningFibersFCB,
                           aActorFx,
                           aPayloadSize,
                           aVirtualSourceNode);

  return( (void*) & ResPkt->mTorusPacket.payload );
  }


inline
void*
PkActorReservePacket(     unsigned long         aDestNode,
                          unsigned long         aDestCore,
                          ActorFunctionType     aActorFx,
                          unsigned long         aPayloadSize)
  {
  void * rc;

  while( NULL ==
         (rc = PkActorReservePacketNonblocking( aDestNode,
                                                aDestCore,
                                                aActorFx,
                                                aPayloadSize ) ) ) PkFiberYield();

  BegLogLine(PKFXLOG_BGL_TORUS)
    << "PkActorReservePacket payload at 0x" << rc
    << EndLogLine ;

  return( rc );
  }

inline
void*
PkActorReservePacketVirtualSource(     unsigned long         aDestNode,
                          unsigned long         aDestCore,
                          ActorFunctionType     aActorFx,
                          unsigned long         aPayloadSize,
                          unsigned int          aVirtualSourceNode)
  {
  void * rc;

  while( NULL ==
         (rc = PkActorReservePacketNonblockingVirtualSource( aDestNode,
                                                aDestCore,
                                                aActorFx,
                                                aPayloadSize,
                                                aVirtualSourceNode) ) ) PkFiberYield();

  BegLogLine(PKFXLOG_BGL_TORUS)
    << "PkActorReservePacketVirtualSource payload at 0x" << rc
    << EndLogLine ;

  return( rc );
  }


inline
void*
PkActorReserveResponsePacket( ActorFunctionType aActorFx,
                              unsigned long     aPayloadSize )
  {
  // the dereference could carry through to the next method.
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  pkFullPacketBuffer *ResPkt = pkActorPacketPoolAllocate( fascb );

  StrongAssertLogLine( ResPkt != NULL )
    << "ERROR: In PkActorReserveResponsePacket(): No more packets in the packet pool"
    << " PK_NUMBER_OF_POOL_PACKETS: " << PK_NUMBER_OF_POOL_PACKETS
    << " Increase this number."
    << EndLogLine;

  unsigned long Node, Core;
  pkFiberControlBlockT* Context;

  PkActorGetPacketSourceContext( &Node, &Core, &Context );

  pkActorPacketMakeHeader( ResPkt,
                           Node,      //fascb->CurrentActorPacketSourceNode,
                           Core,      //fascb->CurrentActorPacketSourceCore,
                           Context,   //fascb->CurrentActorPacketContext,
                           aActorFx,
                           aPayloadSize );

  return( (void*) & ResPkt->mTorusPacket.payload );
  }



//////////////////////////////////////////////////////////////////////
// PkActorDispatchPacket(): links the packet onto a send queue
//////////////////////////////////////////////////////////////////////

static inline
void
pkActorPacketDispatch( pkFiberActorSchedulerControlBlockT*  fascb,
                       pkFullPacketBuffer*                  aAPB )
  {
  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkActorPacketDispatch():"
    << " Packet " << (void*) aAPB
    << " mActorPacketSendQueue Head " << (void*)fascb->mActorPacketSendQueueHead
    << " Tail " << (void*)fascb->mActorPacketSendQueueTail
    << EndLogLine;

  // The self send flag is set in pkActorPacketMakeHeader
  if( aAPB->mPBCB.mSelfSend )
    {
    if( fascb->mActorPacketSelfSendQueueHead == NULL )
      {
      fascb->mActorPacketSelfSendQueueHead = aAPB;
      fascb->mActorPacketSelfSendQueueTail = aAPB;
      BegLogLine( PKFXLOG_BGL_ACTOR )
        << "pkActorPacketDispatch():"
        << " SELF SEND -- LINKED AT  HEAD "
        << " Packet " << (void*) aAPB
        << " mActorPacketSendQueue Head " << (void*)fascb->mActorPacketSelfSendQueueHead
        << " Tail " << (void*)fascb->mActorPacketSelfSendQueueTail
        << EndLogLine;
      }
    else
      {
      fascb->mActorPacketSelfSendQueueTail->mPBCB.mNext = aAPB;
      fascb->mActorPacketSelfSendQueueTail              = aAPB;
      BegLogLine( PKFXLOG_BGL_ACTOR )
        << "pkActorPacketDispatch():"
        << " SELF SEND -- LINKED AT TAIL "
        << " Packet " << (void*) aAPB
        << " mActorPacketSendQueue Head " << (void*)fascb->mActorPacketSelfSendQueueHead
        << " Tail " << (void*)fascb->mActorPacketSelfSendQueueTail
        << EndLogLine;
      }
    }
  else
    {
    // Queue is kept singly linked.  Branches on both ends, but only 2 cache lines hit to update.
    // mActorPacketSendQueueHead == NULL is the empty marker.  The tail is never null (exept on startup)
    if( fascb->mActorPacketSendQueueHead == NULL )
      {
      fascb->mActorPacketSendQueueHead = aAPB;
      fascb->mActorPacketSendQueueTail = aAPB;
      BegLogLine( PKFXLOG_BGL_ACTOR )
        << "pkActorPacketDispatch():"
        << " NETWORK SEND -- LINKED AS HEAD "
        << " Packet " << (void*) aAPB
        << " mActorPacketSendQueue Head " << (void*)fascb->mActorPacketSendQueueHead
        << " Tail " << (void*)fascb->mActorPacketSendQueueTail
        << EndLogLine;
      }
    else
      {
      fascb->mActorPacketSendQueueTail->mPBCB.mNext = aAPB;
      fascb->mActorPacketSendQueueTail              = aAPB;
      BegLogLine( PKFXLOG_BGL_ACTOR )
        << "pkActorPacketDispatch():"
        << " NETWORK SEND -- LINKED AT TAIL "
        << " Packet " << (void*) aAPB
        << " mActorPacketSendQueue Head " << (void*)fascb->mActorPacketSendQueueHead
        << " Tail " << (void*)fascb->mActorPacketSendQueueTail
        << EndLogLine;
      }
    }

  // singly linked list -- null out the packet's next ptr
  aAPB->mPBCB.mNext = NULL;

  }


static inline
void
pkActorPacketDispatchBlocking( pkFiberActorSchedulerControlBlockT*  fascb,
                               pkFullPacketBuffer*                   aAPB )
  {

  aAPB->mPBCB.mSendCompleteAction = pkPacketBufferControlBlock::TransportSendCompleteAction_Unblock;
  aAPB->mPBCB.mAddr               = (void*) fascb->RunningFibersFCB;
  pkActorPacketDispatch( fascb, aAPB );
  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkActorPacketDispatchBlocking():"
    << " START TO BLOCK "
    << " Packet " << (void*) aAPB
    << EndLogLine;
  PkFiberBlock();
  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "pkActorPacketDispatchBlocking():"
    << " FINISH BLOCK "
    << " Packet " << (void*) aAPB
    << EndLogLine;
  }

static void
PkActorPacketDispatchBlocking( pkFullPacketBuffer*                   aAPB )
{
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  pkActorPacketDispatchBlocking( fascb, aAPB );
}

static inline void
_pkBGLTorusSendPackets( pkFiberActorSchedulerControlBlockT* fascb, _BGL_TorusFifoStatus * FifoStatus ) ;

// This is called with the packet payload address
 inline void
PkActorPacketDispatch( void * aPacketPayloadPtr )
  {

#if 0 // this bit of function moved to a separate routine.
  pkFullPacketBuffer x;
  unsigned addr_offs =   ((unsigned)(&x.mTorusPacket.payload)) - ((unsigned)&x);
  pkFullPacketBuffer* APB = (pkFullPacketBuffer*) ((unsigned)pkt - addr_offs);
#endif

  pkFullPacketBuffer* APB = pkActorPacketPointerFromPayloadPointer( aPacketPayloadPtr );

  int * PacketPayloadAsInt = (int *) aPacketPayloadPtr ;
  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "PkActorPacketDispatch():"
    << " aPkt "      << (void*) aPacketPayloadPtr
    << " APB "       << (void*) APB
    << " Payload 0x" << (void *) (PacketPayloadAsInt[0])
                     << (void *) (PacketPayloadAsInt[1])
                     << (void *) (PacketPayloadAsInt[2])
                     << (void *) (PacketPayloadAsInt[3])
                     << (void *) (PacketPayloadAsInt[4])
                     << (void *) (PacketPayloadAsInt[5])
                     << (void *) (PacketPayloadAsInt[6])
                     << (void *) (PacketPayloadAsInt[7])
    << " ..."
    << EndLogLine;

  pkFiberActorSchedulerControlBlockT* aFASCB = pkCoreGetFASCB();

//#if defined(PK_EARLY_SEND)
//  _BGL_TorusFifoStatus FifoStatus[2] ;
//  _ts_GetStatusBoth_nosync(FifoStatus+0, FifoStatus+1) ;
//#endif

  pkActorPacketDispatch( aFASCB, APB );

#if defined(PK_EARLY_SEND)
  _pkBGLTorusSendPackets( aFASCB, &(aFASCB->mRecentTorusFifoStatus) );
#endif

  }

//////////////////////////////////////////////////////////////////////
// _pkBGLTorusSendPackets - actually put packest into fifos
//                - also, take care of disposing of the packet buffer as needed.
//////////////////////////////////////////////////////////////////////

static inline
void
_pkFiberUnblock(pkFiberActorSchedulerControlBlockT* aFASCB, pkFiberControlBlockT * aFCB );

#if defined(PK_SEND_BY_GROUP)
//// Do one access of the status register, then fill all the FIFOs as far as we can
//static const char RoundRobinNext[3] = { 1 , 2 , 0 } ;
//static const char RoundRobinOverNext[3] = { 2 , 0 , 1 } ;

static const char FifoChooser[8] = { 0, 2, 1, 1, 0, 2, 0, 0 } ;

static inline void
_pkBGLTorusSendPackets( pkFiberActorSchedulerControlBlockT* fascb, _BGL_TorusFifoStatus * FifoStatus )
  {
  AssertLogLine( fascb == pkCoreGetFASCB() )
    << "_pkBGLTorusSendPackets():"
    << " fascb "          << (void*) fascb
    << " QueueHead "      << (void*) fascb->mActorPacketSendQueueHead
    << EndLogLine;

  AssertLogLine( FifoStatus )
    << "_pkBGLTorusSendPackets():"
    << " FifoStatus " << (void*) FifoStatus
    << EndLogLine;

  BegLogLine(0) // off due to high vol
    << "_pkBGLTorusSendPackets() "
    << " RoundRobinFifoSelect " << fascb->RoundRobinFifoSelect
    << " CoreId " << fascb->mCoreId
    << EndLogLine;


//  char fifo_status[3] ;
//  fifo_status[0] = FifoStatus->i[0] ;
//  fifo_status[1] = FifoStatus->i[1] ;
//  fifo_status[2] = FifoStatus->i[2] ;
  int CoreId = fascb->mCoreId ;
//  _BGL_TorusFifo * fifo_data[3] ;
//
//  if ( 0 == CoreId )
//  {
//  	fifo_data[0] = (_BGL_TorusFifo *)TS0_I0 ;
//  	fifo_data[1] = (_BGL_TorusFifo *)TS0_I1 ;
//  	fifo_data[2] = (_BGL_TorusFifo *)TS0_I2 ;
//  } else {
//  	fifo_data[0] = (_BGL_TorusFifo *)TS1_I0 ;
//  	fifo_data[1] = (_BGL_TorusFifo *)TS1_I1 ;
//  	fifo_data[2] = (_BGL_TorusFifo *)TS1_I2 ;
//  }


  int RoundRobinSelect = fascb->RoundRobinFifoSelect ;
  pkFullPacketBuffer * ActorPacketSendQueueHead = fascb->mActorPacketSendQueueHead ;

  // send packets while they are queued (don't send terminal) or until fifos fill
  while( ActorPacketSendQueueHead != NULL )
    {
      
//    pkFullPacketBuffer* NextSendQueuePacket = fascb->mActorPacketSendQueueHead;
    pkFullPacketBuffer* NextSendQueuePacket = ActorPacketSendQueueHead;

    AssertLogLine( fascb->mActorPacketSendQueueTail != NULL )
      << "_pkBGLTorusSendPackets(): "
      << " mNextPacketSendQueueTail is NULL - very bad "
      << EndLogLine;

    int Chunks = NextSendQueuePacket->mTorusPacket.hdr.hwh0.Chunks + 1;
    int SendCompleteAction = NextSendQueuePacket->mPBCB.mSendCompleteAction ;
    int StorageClass = NextSendQueuePacket->mPBCB.mStorageClass ;
    _BGL_TorusPkt * TorusPacket = &(NextSendQueuePacket->mTorusPacket) ;

// Find the FIFO with the most space in
    int i0 = FifoStatus->i[0] ;
    int i1 = FifoStatus->i[1] ;
    int i2 = FifoStatus->i[2] ;
    int d0 = i0-i1 ;
    int d1 = i1-i2 ;
    int d2 = i2-i0 ;
    int x=((d0>>31) & 0x4) | ((d1>>31) & 0x2) | ((d2>>31) & 0x1) ;
    // x is now binary-coded as
    // 000 -- all fifos same depth
    // 100 -- i1, i2, i0 -- use i0
    // 010 -- i2, i0, i1 -- use i1
    // 001 -- i0, i1, i2 -- use i2
    // 110 -- i2, i1, i0 -- use i0
    // 011 -- i0, i2, i1 -- use i1
    // 101 -- i1, i0, i2 -- use i2
    // 111 -- impossible
    int FifoToTry=FifoChooser[x] ;
    int FifoFullness = FifoStatus->i[FifoToTry] + Chunks ;
    _BGL_TorusFifo * NextFifoToSend =
       (_BGL_TorusFifo *) (( PA_TORUS_0 | TSx_I0 ) + (CoreId << 16) + ( FifoToTry << 12) ) ;
      BegLogLine(PKFXLOG_BGL_TORUS)
        << "_pkBGLTorusSendPackets  i[0]=" << FifoStatus->i[0]
        << " i[1]=" << FifoStatus->i[1]
        << " i[2]=" << FifoStatus->i[2]
        << " Chunks=" << Chunks
        << " Choosing FIFO " << FifoToTry
        << " FifoFullness=" << FifoFullness
        << " NextFifoToSend=0x" << (void *)NextFifoToSend
          << " TorusPacket 0x" << (void *) TorusPacket
        << " SendCompleteAction=" << SendCompleteAction
        << EndLogLine ;
//    _BGL_TorusFifo * NextFifoToSend = fifo_data[FifoToTry] ;
    // See BGL_TorusPktSPI.h for how this gets to the FIFO address ...
//    {
//    	_BGL_TorusFifoStatus CurrentTorusFifoStatus ;
//        _ts_GetStatusN_nosync((_BGL_TorusFifoStatus *)(PA_TORUS_0|TSx_Sx|((fascb->mCoreId)<<16))
//                 , & CurrentTorusFifoStatus ) ;
//         if ( CurrentTorusFifoStatus.i[FifoToTry] > FifoStatus->i[FifoToTry] )
//         {
//         	BegLogLine(1)
//         	  << "FIFO is fuller than we thought , FifoToTry=" << FifoToTry
//         	  << "(" << CurrentTorusFifoStatus.i[0]
//         	  << "," << CurrentTorusFifoStatus.i[1]
//         	  << "," << CurrentTorusFifoStatus.i[2]
//         	  << ") (" << FifoStatus->i[0]
//         	  << "," << FifoStatus->i[1]
//         	  << "," << FifoStatus->i[2]
//         	  << ")"
//         	  << EndLogLine ;
//         }
//    }
    if ( FifoFullness > _BGL_TORUS_FIFO_CHUNKS )
    {
      BegLogLine(PKFXLOG_BGL_TORUS)
        << "_pkBGLTorusSendPackets send fifos full, i[0]=" << FifoStatus->i[0]
        << " i[1]=" << FifoStatus->i[1]
        << " i[2]=" << FifoStatus->i[2]
        << " Chunks=" << Chunks
        << EndLogLine ;
      break ;
    }
    FifoStatus->i[FifoToTry]=FifoFullness ;
//    int RRobinNext = RoundRobinNext[RoundRobinSelect ] ;
//    int RRobinNNext = RoundRobinOverNext[RoundRobinSelect ] ;
//    int RR3Fullness = fifo_status[RoundRobinSelect] + Chunks ;
//    int RR1Fullness = fifo_status[RRobinNext] + Chunks ;
//    int RR2Fullness = fifo_status[RRobinNNext] + Chunks ;
//
//    if ( RR1Fullness <= _BGL_TORUS_FIFO_CHUNKS )
//    {
//    	// Normal case, space in the next FIFO
//    	fifo_status[RRobinNext] = RR1Fullness ;
//    	fStatus.recordSend(RR1Fullness) ;
//    	RoundRobinSelect = RRobinNext ;
//    } else if ( RR2Fullness <= _BGL_TORUS_FIFO_CHUNKS )
//    {
//   	   // Space in the one after next
//   	   fifo_status[RRobinNNext] = RR2Fullness ;
//       fStatus.recordSend(RR2Fullness) ;
//   	   RoundRobinSelect = RRobinNNext ;
//    } else if (RR3Fullness <= _BGL_TORUS_FIFO_CHUNKS )
//    {
//    	// Space in the first one we thought of
//    	fifo_status[RoundRobinSelect] = RR3Fullness ;
//        fStatus.recordSend(RR3Fullness) ;
//    } else break ;
//
//    _BGL_TorusFifo * NextFifoToSend = fifo_data[RoundRobinSelect] ;


    AssertLogLine( NextSendQueuePacket->mTorusPacket.hdr.swh0.arg0 != 0 )
      << "_pkBGLTorusSendPackets(): "
      << "Header hdr.swh0.arg0 is zero - can't be!"
      << " Pkt@ " << NextSendQueuePacket
      << " w0 "   << (void*) NextSendQueuePacket->mTorusPacket.hdr.swh0.arg0
      << EndLogLine;

    AssertLogLine( NextSendQueuePacket->mTorusPacket.hdr.swh1.arg != 0 )
      << "_pkBGLTorusSendPackets(): "
      << "Header hdr.swh1.arg is zero - can't be!"
      << " Pkt@ " << NextSendQueuePacket
      << " w0 "   << (void*) NextSendQueuePacket->mTorusPacket.hdr.swh1.arg
      << EndLogLine;


    BGLTorusInjectPacketImageApp(   NextFifoToSend, TorusPacket );

    // advance the send queue - the flag for an empty queue is a NULL head pointer
    ActorPacketSendQueueHead = NextSendQueuePacket->mPBCB.mNext;

    // this storage class and the after send action stuff need to be merged
    // this is handled elsewhere for selfsends
    switch( SendCompleteAction )
      {
      case pkPacketBufferControlBlock::TransportSendCompleteAction_NoAction:
        break;
      case pkPacketBufferControlBlock::TransportSendCompleteAction_Unblock:
        _pkFiberUnblock( fascb, (pkFiberControlBlockT *)NextSendQueuePacket->mPBCB.mAddr );
        BegLogLine(PKFXLOG_BGL_ACTOR)
          << "_pkBGLTorusSendPackets():"
          << " Unblock called "
          << " FCB "  << NextSendQueuePacket->mPBCB.mAddr
          << " Pkt@ " << (void*) NextSendQueuePacket
          << " StorageClass " << NextSendQueuePacket->mPBCB.mStorageClass
          << EndLogLine;
        break;
      default:
        StrongAssertLogLine(0)
          << "_pkBGLTorusSendPackets():"
          << " Unrecognized or unfielded SendCompleteionAction "
          << " ActionCode " << NextSendQueuePacket->mPBCB.mSendCompleteAction
          << " Pkt@ "       << (void*) NextSendQueuePacket
          << " type "       << NextSendQueuePacket->mPBCB.mStorageClass
          << EndLogLine;
      };

    // in order to free to a specialized pool this would need to be integrated with the SendPacketCompletionFunction
    switch( StorageClass )
      {
      case pkPacketBufferControlBlock::StorageClass_External :
          BegLogLine(PKFXLOG_BGL_ACTOR)
            << "_pkBGLTorusSendPackets():"
            << " Externally allocated packet - taking no action "
            << " Pkt@ " << (void*) NextSendQueuePacket
            << " type " << NextSendQueuePacket->mPBCB.mStorageClass
            << EndLogLine;
        break;
      case pkPacketBufferControlBlock::StorageClass_BufferPool :
          BegLogLine(PKFXLOG_BGL_ACTOR)
            << "_pkBGLTorusSendPackets():"
            << " Freeing packet to buffer pool "
            << " Pkt@ " << (void*) NextSendQueuePacket
            << " type " << NextSendQueuePacket->mPBCB.mStorageClass
            << EndLogLine;
          pkActorPacketPoolFree( fascb, NextSendQueuePacket );
        break;
      case pkPacketBufferControlBlock::StorageClass_PkHeap :
          PkHeapFree( NextSendQueuePacket );
          BegLogLine(PKFXLOG_BGL_ACTOR)
            << "_pkBGLTorusSendPackets():"
            << " Freeing packet to buffer heap "
            << " Pkt@ " << (void*) NextSendQueuePacket
            << " type " << NextSendQueuePacket->mPBCB.mStorageClass
            << EndLogLine;
        break;
      default:
        StrongAssertLogLine(0)
          << "Packet buffer has an unrecognized type so it is impossible to dispose of"
          << EndLogLine;
      }

    BegLogLine(PKFXLOG_BGL_ACTOR)
      << "_pkBGLTorusSendPackets() "
      << " INJECTION DONE "
      << " RoundRobinFifoSelect " << RoundRobinSelect
      << EndLogLine;
    }
    fascb->mActorPacketSendQueueHead = ActorPacketSendQueueHead ;
    fascb->RoundRobinFifoSelect = RoundRobinSelect ;
  }
#else
static inline void
_pkBGLTorusSendPackets( pkFiberActorSchedulerControlBlockT* fascb, _BGL_TorusFifoStatus * FifoStatus )
  {
  AssertLogLine( fascb == pkCoreGetFASCB() )
    << "_pkBGLTorusSendPackets():"
    << " fascb "          << (void*) fascb
    << " QueueHead "      << (void*) fascb->mActorPacketSendQueueHead
    << EndLogLine;

  AssertLogLine( FifoStatus )
    << "_pkBGLTorusSendPackets():"
    << " FifoStatus " << (void*) FifoStatus
    << EndLogLine;

  BegLogLine(0) // off due to high vol
    << "_pkBGLTorusSendPackets() "
    << " RoundRobinFifoSelect " << fascb->RoundRobinFifoSelect
    << " CoreId " << fascb->mCoreId
    << EndLogLine;

  // send packets while they are queued (don't send terminal) or until fifos fill
  while( fascb->mActorPacketSendQueueHead != NULL )
    {
    pkFullPacketBuffer* NextSendQueuePacket = fascb->mActorPacketSendQueueHead;

    AssertLogLine( fascb->mActorPacketSendQueueTail != NULL )
      << "_pkBGLTorusSendPackets(): "
      << " mNextPacketSendQueueTail is NULL - very bad "
      << EndLogLine;

    int Chunks = NextSendQueuePacket->mTorusPacket.hdr.hwh0.Chunks + 1;

    _BGL_TorusFifo * NextFifoToSend;
    if( fascb->mCoreId == 0 ) NextFifoToSend = _ts_CheckClearToSendOnLink0(  FifoStatus, fascb->RoundRobinFifoSelect, Chunks );
    else                      NextFifoToSend = _ts_CheckClearToSendOnLink1(  FifoStatus, fascb->RoundRobinFifoSelect, Chunks );

    fascb->RoundRobinFifoSelect++;
    if( fascb->RoundRobinFifoSelect == 3 )
      fascb->RoundRobinFifoSelect = 0;

    // Check if there is room in the fifo to send, if not, break out and try later.  Could be better.
    if( NextFifoToSend == NULL )
      break;

    AssertLogLine( NextSendQueuePacket->mTorusPacket.hdr.swh0.arg0 != 0 )
      << "_pkBGLTorusSendPackets(): "
      << "Header hdr.swh0.arg0 is zero - can't be!"
      << " Pkt@ " << NextSendQueuePacket
      << " w0 "   << (void*) NextSendQueuePacket->mTorusPacket.hdr.swh0.arg0
      << EndLogLine;

#if 0 //// this assert isn't valid with new formats for header words
    AssertLogLine( NextSendQueuePacket->mTorusPacket.hdr.swh1.arg != 0 )
      << "_pkBGLTorusSendPackets(): "
      << "Header hdr.swh1.arg is zero - can't be!"
      << " Pkt@ " << NextSendQueuePacket
      << " w0 "   << (void*) NextSendQueuePacket->mTorusPacket.hdr.swh1.arg
      << EndLogLine;
#endif

// #if PKFXLOG_BGL_ACTOR
   #if 0
      _BGL_TorusPktHdr* NextPacket = & NextSendQueuePacket->mTorusPacket.hdr;
      int w0 = NextPacket->swh0.arg0;
      int w1 = NextPacket->swh1.arg;
      unsigned long        srcn;
      unsigned long        srcc;
      ActorFunctionType    fx;
      pkFiberControlBlockT * ctx;
      UnloadActorHeaderSrcCtxWords( w0, w1, &srcn, &srcc, &fx, &ctx);
      assert( srcn      == fascb->mNodeId );
      BegLogLine( PKFXLOG_BGL_ACTOR )
        << "_pkBGLTorusSendPackets()"
        << " INJECTING "
        << " PoolPkt "      << fascb->mActorPacketSendQueueHead
        << " chunks "       << Chunks
        << " w0 "           << (void*) w0
        << " w1 "           << (void*) w1
        << " SrcNode "      << srcn
        << " SrcCore "      << srcc
        << " fx "           << (void*) fx
        << " ctx "          << (void*) ctx
        << " pl0 "          << ((void**)NextPacket)[4]
        << " pl1 "          << ((void**)NextPacket)[5]
        << " pl2 "          << ((void**)NextPacket)[6]
        << " pl3 "          << ((void**)NextPacket)[7]
        << EndLogLine;

      BegLogLine( PKFXLOG_BGL_ACTOR )
        << "_pkBGLTorusSendPackets()"
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
                                  & NextSendQueuePacket->mTorusPacket );

    // advance the send queue - the flag for an empty queue is a NULL head pointer
    fascb->mActorPacketSendQueueHead = NextSendQueuePacket->mPBCB.mNext;

    // this storage class and the after send action stuff need to be merged
    // this is handled elsewhere for selfsends
    switch( NextSendQueuePacket->mPBCB.mSendCompleteAction )
      {
      case pkPacketBufferControlBlock::TransportSendCompleteAction_NoAction:
        break;
      case pkPacketBufferControlBlock::TransportSendCompleteAction_Unblock:
        _pkFiberUnblock( fascb, (pkFiberControlBlockT *)NextSendQueuePacket->mPBCB.mAddr );
        BegLogLine(PKFXLOG_BGL_ACTOR)
          << "_pkBGLTorusSendPackets():"
          << " Unblock called "
          << " FCB "  << NextSendQueuePacket->mPBCB.mAddr
          << " Pkt@ " << (void*) NextSendQueuePacket
          << " StorageClass " << NextSendQueuePacket->mPBCB.mStorageClass
          << EndLogLine;
        break;
      default:
        StrongAssertLogLine(0)
          << "_pkBGLTorusSendPackets():"
          << " Unrecognized or unfielded SendCompleteionAction "
          << " ActionCode " << NextSendQueuePacket->mPBCB.mSendCompleteAction
          << " Pkt@ "       << (void*) NextSendQueuePacket
          << " type "       << NextSendQueuePacket->mPBCB.mStorageClass
          << EndLogLine;
      };

    // in order to free to a specialized pool this would need to be integrated with the SendPacketCompletionFunction
    switch( NextSendQueuePacket->mPBCB.mStorageClass )
      {
      case pkPacketBufferControlBlock::StorageClass_External :
          BegLogLine(PKFXLOG_BGL_ACTOR)
            << "_pkBGLTorusSendPackets():"
            << " Externally allocated packet - taking no action "
            << " Pkt@ " << (void*) NextSendQueuePacket
            << " type " << NextSendQueuePacket->mPBCB.mStorageClass
            << EndLogLine;
        break;
      case pkPacketBufferControlBlock::StorageClass_BufferPool :
          BegLogLine(PKFXLOG_BGL_ACTOR)
            << "_pkBGLTorusSendPackets():"
            << " Freeing packet to buffer pool "
            << " Pkt@ " << (void*) NextSendQueuePacket
            << " type " << NextSendQueuePacket->mPBCB.mStorageClass
            << EndLogLine;
          pkActorPacketPoolFree( fascb, NextSendQueuePacket );
        break;
      case pkPacketBufferControlBlock::StorageClass_PkHeap :
          PkHeapFree( NextSendQueuePacket );
          BegLogLine(PKFXLOG_BGL_ACTOR)
            << "_pkBGLTorusSendPackets():"
            << " Freeing packet to buffer heap "
            << " Pkt@ " << (void*) NextSendQueuePacket
            << " type " << NextSendQueuePacket->mPBCB.mStorageClass
            << EndLogLine;
        break;
      default:
        StrongAssertLogLine(0)
          << "Packet buffer has an unrecognized type so it is impossible to dispose of"
          << EndLogLine;
      }

    BegLogLine(PKFXLOG_BGL_ACTOR)
      << "_pkBGLTorusSendPackets() "
      << " INJECTION DONE "
      << " RoundRobinFifoSelect " << fascb->RoundRobinFifoSelect
      << EndLogLine;
    }
  }
#endif
//////////////////////////////////////////////////////////////////////
// _pkProcessShmemPackets() - ???
//////////////////////////////////////////////////////////////////////
static inline
void
_pkProcessShmemPacket(  pkFiberActorSchedulerControlBlockT* aFASCB )
  {
  StrongAssertLogLine( 0 )
    << "_pkProcessActorPackets():"
    << " PkPacketNonActorProtocol_Shmem received - not implemented"
    << EndLogLine;
  }

//////////////////////////////////////////////////////////////////////
// _pkProcessChannelPackets() - ???
//////////////////////////////////////////////////////////////////////
static inline
void
_pkProcessChannelPacket(  pkFiberActorSchedulerControlBlockT* aFASCB )
  {
  _BGL_TorusPkt * aRecvBuf = aFASCB->mCurrentActorPacketPtr;

  pkFullPacketBuffer* ChPktBuf = pkActorPacketPoolAllocate( aFASCB );

  StrongAssertLogLine( ChPktBuf != NULL )
    << " FAILED TO ALLOCATE PACKET FOR ARIVING CHANNEL PACKET "
    << EndLogLine;

  //NEED: a copy function that only copies the chuncks used
  ChPktBuf->mTorusPacket = *aRecvBuf;

  int w0 = ChPktBuf->mTorusPacket.hdr.swh0.arg0;
  int w1 = ChPktBuf->mTorusPacket.hdr.swh1.arg;

  unsigned long       ItemCount;
  unsigned long       SeqNo;
  ChannelTerminal* RecvChannelTerminalAddr;

  UnloadChannelHeaderWords(  w0, w1, &ItemCount, &SeqNo, &RecvChannelTerminalAddr);

  BegLogLine(PKFXLOG_BGL_CHANNEL)
    << "_pkProcessChannelPacket():"
    << " aPacketBuf " << (void*) ChPktBuf
    << " w0 "         << (void*) w0
    << " w1 "         << (void*) w1
    << " ItemCount "  << ItemCount
    << " SeqNo "      << SeqNo
    << " ChTermRef "  << (void*)RecvChannelTerminalAddr
    << EndLogLine;

  ChannelTerminal* ChTermPtr = (ChannelTerminal*)RecvChannelTerminalAddr;

  BegLogLine(PKFXLOG_BGL_CHANNEL)
    << "_pkProcessChannelPacket():"
    << " RECVPKT "
    << " ChannelReader@ "        << (void*)ChTermPtr
    << " ChannelReader->mHead "  << ChTermPtr->mHead
    << " ChannelReader->mHead@ " << (void*)&ChTermPtr->mHead
    << " ChPktBuf "              << ChPktBuf
    << EndLogLine;

  if( ChTermPtr->mHead == NULL )
    {
    // mHead is the first packet into a doubly linked list
    ChTermPtr->mHead        = ChPktBuf;
    ChTermPtr->mHead->mPBCB.mNext = ChPktBuf;
    ChTermPtr->mHead->mPBCB.mPrev = ChPktBuf;
    }
  else
    {
    // assume this is going to go onto the tail... but work forward to find ordered position in linked list
    pkFullPacketBuffer* Posit = ChTermPtr->mHead->mPBCB.mPrev;
    while( (Posit != ChTermPtr->mHead) && (SeqNo > pkChannelPacketGetHeaderSeqNo(Posit) ) )
      {
      Posit = Posit->mPBCB.mPrev;
      }
    // put in place behind the posit found
    Posit->mPBCB.mNext->mPBCB.mPrev = ChPktBuf;
    ChPktBuf->mPBCB.mNext     = Posit->mPBCB.mNext;
    Posit->mPBCB.mPrev        = ChPktBuf;
    ChPktBuf->mPBCB.mPrev     = Posit;
    }

  }

//////////////////////////////////////////////////////////////////////
// _pkReceiveAndProcessActorPackets() - receive packets into pool buffers
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// _pkProcessActorPackets() - ???
//////////////////////////////////////////////////////////////////////
static inline
void
_pkProcessActorPacketFromBuffer(  pkFiberActorSchedulerControlBlockT* aFASCB )
  {

  _BGL_TorusPkt * PacketPtr = aFASCB->mCurrentActorPacketPtr;

  PktHdrW0 w0;
  w0.mWord = PacketPtr->hdr.swh0.arg0;

  ActorFunctionType ActorFx = _pkUpShiftActorFx( w0.Actor.mActorFx );


  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "_pkProcessActorPacketFromBuffer() "
    << "START RECV PKT"
    << " w0 "           << (void*) PacketPtr->hdr.swh0.arg0
    << " w1 "           << (void*) PacketPtr->hdr.swh1.arg
    << " ActorFx "      << (void*) ActorFx
    << " Chunks "       << PacketPtr->hdr.hwh0.Chunks + 1
    << EndLogLine;


  int ActorRc;

  do
    {
    ActorRc = ActorFx( (void *) & PacketPtr->payload );
    } while(0);

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "_pkProcessActorPacketFromBuffer() "
    << "START RECV PKT"
    << " ActorRc "      << ActorRc
    << " w0 "           << (void*) PacketPtr->hdr.swh0.arg0
    << " w1 "           << (void*) PacketPtr->hdr.swh1.arg
    << " ActorFx "      << (void*) ActorFx
    << " Chunks "       << PacketPtr->hdr.hwh0.Chunks + 1
    << EndLogLine;

  // this might not need to be a strong assert for long -- but while we are switching from allowing acotrs to keep packets...
  StrongAssertLogLine( ActorRc == 0 )
    << "_pkProcessActorPacketFromBuffer() "
    << "ERROR Rc must be 0 "
    << " ActorRc "      << ActorRc
    << " w0 "           << (void*) PacketPtr->hdr.swh0.arg0
    << " w1 "           << (void*) PacketPtr->hdr.swh1.arg
    << " ActorFx "      << (void*) ActorFx
    << " Chunks "       << PacketPtr->hdr.hwh0.Chunks + 1
    << EndLogLine;

  aFASCB->mCurrentActorPacketPtr = PacketPtr;

  }

static inline int
_pkProcessPacketFromBuffer(pkFiberActorSchedulerControlBlockT* aFASCB,
                           _BGL_TorusPkt * PacketPtr)
  {
  int chunks = PacketPtr->hdr.hwh0.Chunks ;

  PktHdrW0 w0;
  w0.mWord = PacketPtr->hdr.swh0.arg0;

  PktHdrW1 w1;
  w1.mWord = PacketPtr->hdr.swh1.arg;

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "_pkProcessPacketFromBuffer() "
    << " Pkt@ "     << (void*)PacketPtr
    << " HDR W0 0x" << (void*)w0.mWord
    << " HDR W1 0x" << (void*)w1.mWord
    << EndLogLine;

  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "_pkProcessPacketFromBuffer() "
    << " Pkt@ "     << (void*)PacketPtr
    << " PAYLOAD "
    << " 0 "          << ((void**)(&PacketPtr->payload))[0]
    << " 1 "          << ((void**)(&PacketPtr->payload))[1]
    << " 2 "          << ((void**)(&PacketPtr->payload))[2]
    << " 3 "          << ((void**)(&PacketPtr->payload))[3]
    << " 4 "          << ((void**)(&PacketPtr->payload))[4]
    << " 5 "          << ((void**)(&PacketPtr->payload))[5]
    << " 6 "          << ((void**)(&PacketPtr->payload))[6]
    << " 7 "          << ((void**)(&PacketPtr->payload))[7]
    << EndLogLine;

  AssertLogLine( aFASCB->mCurrentActorPacketPtr == NULL )
    << "_pkProcessPacketFromBuffer() "
    << " CurrentActorPacketPtr is supposed to be NULL when we get here "
    << " Pkt@ "     << (void*)PacketPtr
    << " HDR W0 0x" << (void*)w0.mWord
    << " HDR W1 0x" << (void*)w1.mWord
    << EndLogLine;

  aFASCB->mCurrentActorPacketPtr = PacketPtr;

  if( w0.Actor.mNonActorFlag == 0 )
    {
    _pkProcessActorPacketFromBuffer(aFASCB);

    BegLogLine( PKFXLOG_BGL_ACTOR )
      << "_pkProcessPacketFromBuffer() "
      << " Pkt@ "   << (void*)PacketPtr
      << " Processed as Actor Packet "
      << EndLogLine;
    }
  else
    {
    switch( w0.NonActor.mNonActorProtocol )
      {
      case PkPacketNonActorProtocol_Channel:
        {
        // copy out happens in

        _pkProcessChannelPacket(aFASCB);

        BegLogLine( PKFXLOG_BGL_ACTOR )
          << "_pkProcessPacketFromBuffer() "
          << " Pkt@ "   << (void*)PacketPtr
          << " Processed as Channel Packet "
          << EndLogLine;
        break;
        }
      case PkPacketNonActorProtocol_Shmem:
        {
        _pkProcessShmemPacket(aFASCB);

        BegLogLine( PKFXLOG_BGL_ACTOR )
          << "_pkProcessPacketFromBuffer() "
          << " Pkt@ "   << (void*)PacketPtr
          << " Processed as Shmem Packet "
          << EndLogLine;
        break;
        }
      default:
        StrongAssertLogLine(0) << "_pkProcessPacketFromBuffer():" << " bad protocol code " << w0.NonActor.mNonActorProtocol << EndLogLine;
      };
    }

  AssertLogLine( aFASCB->mCurrentActorPacketPtr == PacketPtr )
    << "_pkProcessPacketFromBuffer() "
    << " CurrentActorPacketPtr is supposed to be the same as the packet ptr passed in "
    << " Pkt@ "     << (void*)PacketPtr
    << " HDR W0 0x" << (void*)w0.mWord
    << " HDR W1 0x" << (void*)w1.mWord
    << EndLogLine;

  // signals that this core and core control block is now not handling a packet
  aFASCB->mCurrentActorPacketPtr = NULL;

  return chunks+1 ;
  }

static void
_pkReceiveAndProcessActorPackets(  pkFiberActorSchedulerControlBlockT* aFASCB,
                                   _BGL_TorusFifoStatus*               FifoStatus )
  {
  double _Complex HalfChunks[2*_BGL_TORUS_RECEPTION_FIFO_CHUNKS] ;
  _BGL_TorusFifoStatus FifoStatusIndexable = *(_BGL_TorusFifoStatus *) FifoStatus ;
  int fifo_address_base = PA_TORUS_0 | ( (aFASCB->mCoreId) << 16 ) ;

  for ( int fifoIndex=0;fifoIndex < 6 ; fifoIndex += 1)
    {
    int fifo_address = fifo_address_base + TSx_R1*fifoIndex ;
    int ChunkTotal = FifoStatusIndexable.r[fifoIndex] ;
    
    AssertLogLine( ChunkTotal <= _BGL_TORUS_RECEPTION_FIFO_CHUNKS )
      << "_pkReceiveAndProcessActorPackets:: ERROR "
      << " ChunkTotal: " << ChunkTotal
      << " 2*_BGL_TORUS_RECEPTION_FIFO_CHUNKS: " << (2*_BGL_TORUS_RECEPTION_FIFO_CHUNKS)
      << EndLogLine; 

    _ts_AppReceiver_Drain( (_BGL_TorusFifo*)fifo_address, ChunkTotal,HalfChunks ) ;
    int ChunkIndex = 0 ; 
    while ( ChunkIndex < ChunkTotal )
      {
      ChunkIndex += _pkProcessPacketFromBuffer(aFASCB,(_BGL_TorusPkt *)(HalfChunks+2*ChunkIndex)) ;
      }
    }

  // send packets while they are queued (don't send terminal) or until fifos fill
  while( aFASCB->mActorPacketSelfSendQueueHead != NULL )
    {
    pkFullPacketBuffer* NextSelfSendQueuePacket = aFASCB->mActorPacketSelfSendQueueHead;

    AssertLogLine( aFASCB->mActorPacketSelfSendQueueTail != NULL )
      << "_pkReceiveAndProcessActorPackets():"
      << " SELF RECV "
      << " mNextPacketSelfSendQueueTail is NULL - very bad "
      << EndLogLine;

    AssertLogLine( NextSelfSendQueuePacket->mTorusPacket.hdr.swh0.arg0 != 0 )
      << "_pkReceiveAndProcessActorPackets():"
      << " SELF RECV "
      << "Header hdr.swh0.arg0 is zero - can't be!"
      << " Pkt@ " << NextSelfSendQueuePacket
      << " w0 "   << (void*) NextSelfSendQueuePacket->mTorusPacket.hdr.swh0.arg0
      << EndLogLine;

#if 0 // this can now be null
    AssertLogLine( NextSelfSendQueuePacket->mTorusPacket.hdr.swh1.arg != 0 )
      << "_pkReceiveAndProcessActorPackets():"
      << " SELF RECV "
      << "Header hdr.swh1.arg is zero - can't be!"
      << " Pkt@ " << NextSelfSendQueuePacket
      << " w0 "   << (void*) NextSelfSendQueuePacket->mTorusPacket.hdr.swh1.arg
      << EndLogLine;
#endif

    // advance the send queue - the flag for an empty queue is a NULL head pointer
    // do it now because the next packet might be freed or kept
    aFASCB->mActorPacketSelfSendQueueHead = NextSelfSendQueuePacket->mPBCB.mNext;

    BegLogLine(PKFXLOG_BGL_ACTOR)
      << "_pkReceiveAndProcessActorPackets():"
      << " SELF RECV About to process "
      << " Pkt@ "      << NextSelfSendQueuePacket
      << EndLogLine;

    _pkProcessPacketFromBuffer(    aFASCB,
                                & (NextSelfSendQueuePacket->mTorusPacket) ) ;

    // we processed a SelfSend packet, we may need to act as the send function would to return the buffer
    switch( NextSelfSendQueuePacket->mPBCB.mStorageClass )
      {
      case pkPacketBufferControlBlock::StorageClass_External :
        {
        BegLogLine(PKFXLOG_BGL_ACTOR)
          << "_pkReceiveAndProcessActorPackets():"
          << " Pkt@ " << (void*) NextSelfSendQueuePacket
          << " SELF RECV External class packet not kept by actor - NO ACTION"
          << EndLogLine;

         switch( NextSelfSendQueuePacket->mPBCB.mSendCompleteAction )
           {
           case pkPacketBufferControlBlock::TransportSendCompleteAction_NoAction:
             break;
           case pkPacketBufferControlBlock::TransportSendCompleteAction_Unblock:
             _pkFiberUnblock( aFASCB, (pkFiberControlBlockT *)NextSelfSendQueuePacket->mPBCB.mAddr );
             BegLogLine(PKFXLOG_BGL_ACTOR)
               << "_pkReceiveAndProcessActorPackets():"
               << " SendComplete Action Unblock called "
               << " FCB "  << NextSelfSendQueuePacket->mPBCB.mAddr
               << " Pkt@ " << (void*) NextSelfSendQueuePacket
               << " StorageClass " << NextSelfSendQueuePacket->mPBCB.mStorageClass
               << EndLogLine;
             break;
           default:
             StrongAssertLogLine(0)
               << "_pkReceiveAndProcessActorPackets():"
               << " Unrecognized or unfielded SendCompleteionAction "
               << " ActionCode " << NextSelfSendQueuePacket->mPBCB.mSendCompleteAction
               << " Pkt@ "       << (void*) NextSelfSendQueuePacket
               << " type "       << NextSelfSendQueuePacket->mPBCB.mStorageClass
               << EndLogLine;
           };
        } // End case of StorageClass External
        break;
      case pkPacketBufferControlBlock::StorageClass_BufferPool :
        {
        BegLogLine(PKFXLOG_BGL_ACTOR)
          << "_pkReceiveAndProcessActorPackets():"
          << " Pkt@ " << (void*) NextSelfSendQueuePacket
          << " SELF RECV Pool class packet not kept by actor - FREED TO POOL"
          << EndLogLine;
        pkActorPacketPoolFree( aFASCB, NextSelfSendQueuePacket );
        }
        break;
      case pkPacketBufferControlBlock::StorageClass_PkHeap :
        {
        BegLogLine(PKFXLOG_BGL_ACTOR)
          << "_pkReceiveAndProcessActorPackets():"
          << " Pkt@ " << (void*) NextSelfSendQueuePacket
          << " SELF RECV Heap class packet not kept by actor - FREED TO HEAP"
          << EndLogLine;
        PkHeapFree( NextSelfSendQueuePacket );
        }
        break;
      default:
        StrongAssertLogLine(0)
          << "_pkReceiveAndProcessActorPackets():"
          << "SELF RECV Packet buffer NOT KEPT  has an unrecognized storage class so it is impossible to dispose of"
          << " Pkt@ " << (void*) NextSelfSendQueuePacket
          << EndLogLine;
      };

    BegLogLine(PKFXLOG_BGL_ACTOR)
      << "_pkReceiveAndProcessActorPackets():"
      << " SELF RECV Done processing "
      << " Pkt@ "      << NextSelfSendQueuePacket
      << " QueueHead " << aFASCB->mActorPacketSelfSendQueueHead
      << EndLogLine;

    }
  }
// This is suitable to be called by the spi barrier waits
static inline
int
_pkKickPipes( pkFiberActorSchedulerControlBlockT *aFASCB ) // Named in honour of PHOCH
  {

#if 1
  AssertLogLine( aFASCB != NULL ) << " NULL scheduler control block pointer passed " << EndLogLine;

//  _BGL_TorusFifoStatus FifoStatus;

  pkFullPacketBuffer*  ActorPacketSendQueueHead= aFASCB->mActorPacketSendQueueHead ;
#if KICKPIPES_SYNC_ON_STATUS
  _ts_GetStatusN_sync((_BGL_TorusFifoStatus *)(PA_TORUS_0|TSx_Sx|((aFASCB->mCoreId)<<16)),
                         & (aFASCB->mRecentTorusFifoStatus )) ;
#else
  _ts_GetStatusN_nosync((_BGL_TorusFifoStatus *)(PA_TORUS_0|TSx_Sx|((aFASCB->mCoreId)<<16)),
                         & (aFASCB->mRecentTorusFifoStatus )) ;
#endif
  BegLogLine(PKFXLOG_BGL_TORUS_HI_FREQ)
    << "_pkKickPipes refreshing status"
    << EndLogLine ;
  fStatus.recordSend(aFASCB->mRecentTorusFifoStatus.i[0]) ;
  fStatus.recordSend(aFASCB->mRecentTorusFifoStatus.i[1]) ;
  fStatus.recordSend(aFASCB->mRecentTorusFifoStatus.i[2]) ;
  fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[0]) ;
  fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[1]) ;
  fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[2]) ;
  fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[3]) ;
  fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[4]) ;
  fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[5]) ;
#if defined(CHECK_OTHER_FIFOS)
  // this whole checking of cores should be driven into templates
  if( aFASCB->mCoreId == 0 )
    {
//    _ts_GetStatus0( & (aFASCB->mRecentTorusFifoStatus ));

    // Check that there is nothing in the 'wrong' fifo set.
    _BGL_TorusFifoStatus FifoStatusOther ;
    _ts_GetStatus1( & FifoStatusOther ) ;

    assert(0 == FifoStatusOther.r[0] ) ;
    assert(0 == FifoStatusOther.r[1] ) ;
    assert(0 == FifoStatusOther.r[2] ) ;
    assert(0 == FifoStatusOther.r[3] ) ;
    assert(0 == FifoStatusOther.r[4] ) ;
    assert(0 == FifoStatusOther.r[5] ) ;
    }
//  else
//  {
//    _ts_GetStatus1( & (aFASCB->mRecentTorusFifoStatus ) );
//  }
#endif
  if( ActorPacketSendQueueHead != NULL )
    {
    _pkBGLTorusSendPackets( aFASCB, & (aFASCB->mRecentTorusFifoStatus ) );
    }

  _pkReceiveAndProcessActorPackets( aFASCB, & (aFASCB->mRecentTorusFifoStatus ) );

  // if packets were generated, send them if possible.
  if( ActorPacketSendQueueHead != NULL )
    {
    _pkBGLTorusSendPackets( aFASCB, & (aFASCB->mRecentTorusFifoStatus ) );
    }
//  _pkBGLTorusSendPackets( aFASCB, & FifoStatus );
//
#endif
  return(0);
  }


static inline
void
_pkClearSendQueue( pkFiberActorSchedulerControlBlockT *aFASCB ) // Named in honour of PHOCH
  {
  assert( aFASCB != NULL );

  pkFullPacketBuffer*  ActorPacketSendQueueHead= aFASCB->mActorPacketSendQueueHead ;
  while ( ActorPacketSendQueueHead != NULL )
  {
#if KICKPIPES_SYNC_ON_STATUS
    _ts_GetStatusN_sync((_BGL_TorusFifoStatus *)(PA_TORUS_0|TSx_Sx|((aFASCB->mCoreId)<<16))
                 , & (aFASCB->mRecentTorusFifoStatus )) ;
#else
    _ts_GetStatusN_nosync((_BGL_TorusFifoStatus *)(PA_TORUS_0|TSx_Sx|((aFASCB->mCoreId)<<16))
                 , & (aFASCB->mRecentTorusFifoStatus )) ;
#endif
    fStatus.recordSend(aFASCB->mRecentTorusFifoStatus.i[0]) ;
    fStatus.recordSend(aFASCB->mRecentTorusFifoStatus.i[1]) ;
    fStatus.recordSend(aFASCB->mRecentTorusFifoStatus.i[2]) ;
    fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[0]) ;
    fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[1]) ;
    fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[2]) ;
    fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[3]) ;
    fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[4]) ;
    fStatus.recordReceive(aFASCB->mRecentTorusFifoStatus.r[5]) ;
  BegLogLine(PKFXLOG_BGL_TORUS)
      << "_pkClearSendQueue refreshing status i[0]=" << aFASCB->mRecentTorusFifoStatus.i[0]
      << " i[1]=" << aFASCB->mRecentTorusFifoStatus.i[1]
      << " i[2]=" << aFASCB->mRecentTorusFifoStatus.i[2]
      << " r[0]=" << aFASCB->mRecentTorusFifoStatus.r[0]
      << " r[1]=" << aFASCB->mRecentTorusFifoStatus.r[1]
      << " r[2]=" << aFASCB->mRecentTorusFifoStatus.r[2]
      << " r[3]=" << aFASCB->mRecentTorusFifoStatus.r[3]
      << " r[4]=" << aFASCB->mRecentTorusFifoStatus.r[4]
      << " r[5]=" << aFASCB->mRecentTorusFifoStatus.r[5]
      << EndLogLine ;
  _pkReceiveAndProcessActorPackets( aFASCB, & (aFASCB->mRecentTorusFifoStatus ) );
    _pkBGLTorusSendPackets( aFASCB, & (aFASCB->mRecentTorusFifoStatus ) );
    ActorPacketSendQueueHead= aFASCB->mActorPacketSendQueueHead ;
  }
  }

             #if 0 /// can't do this in the fiber world
             // Kick pipes in a way that only allows sends and not receives
             static inline
             int
             PkKickPipesGently( void ) // Named in honour of PHOCH
             {
              return _pkKickPipesGently(pkCoreGetFASCB()) ;
             }
             #endif

static inline
void
PkClearSendQueue(void)
{
  _pkClearSendQueue(pkCoreGetFASCB()) ;
}

//*****************************************************************************
// BEGIN FIBER INLINE CODE
//*****************************************************************************


#include <BonB/blade_on_blrts.h>
#include <BonB/bl_light_context_swap.h>

#define FCB_AlignmentPadding (0x01 << 10)
#define FCB_AlignmentMask   (FCB_AlignmentPadding - 1)

typedef unsigned long long FID;

struct
FID_MAP
  {
  unsigned short Node;
  unsigned short Core;
  unsigned       Fiber;
  };

#if 0

  inline
  FID
  Create( BG_CoRoutineFunctionType aFiberMainFx,
          void *                   aFiberArg     = NULL,
          unsigned                 aReqStackSize = 0,
          PkThread_Id_T            aThreadId     = PkThread_Id_LOCAL,
          PkProc_Id_T              aProcId       = PkProc_Id_LOCAL
         )
    {
    }
#endif

static
inline
pkFiberControlBlockT *
_pkFiberGetRunningFCB(pkFiberActorSchedulerControlBlockT* aFASCB )
  {
  AssertLogLine( aFASCB != NULL )
    << "_pkFiberGetRunningFCB() called with NULL core's FiberActorSchedControlBlock (FASCB)"
    << EndLogLine;

  AssertLogLine( aFASCB->RunningFibersFCB != NULL )
    << "_pkFiberGetRunningFCB() called with NULL in core's RunningFibersFCB"
    << EndLogLine;

  AssertLogLine( aFASCB->mCurrentActorPacketPtr == NULL )
    << "_pkFiberGetRunningFCB() called with a linked receive packet -- probably in an actor fx"
    << EndLogLine;

  pkFiberControlBlockT* Rc = aFASCB->RunningFibersFCB;
  return(Rc);
  }

static
inline
pkFiberControlBlockT *
pkFiberGetRunningFCB()
  {
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();
  pkFiberControlBlockT* Rc = _pkFiberGetRunningFCB( fascb );
  return(Rc);
  }

//////////////////////////////////////////////////////////////////////
// Unblock- unblock another fiber
//        - global interface is dangerous because syncronization is not provided
//////////////////////////////////////////////////////////////////////
static inline
void
_pkFiberUnblock(pkFiberActorSchedulerControlBlockT* aFASCB, pkFiberControlBlockT * aFCB )
  {
  // arguement aFASCB is probably not needed.
  AssertLogLine( aFCB->mFASCB == aFASCB )
    << "_pkFiberUnblock():"
    << " aFCB->mFASCB " << aFCB->mFASCB
    << " aFASCB "       << aFASCB
    << EndLogLine;

  BegLogLine( PKFXLOG_PK_FIBER )
    << "_pkFiberUnblock():"
    << " starting on  "    << (void*) aFCB
    << " ReadyQueueHead "  << (void*) aFASCB->ReadyQueueHead
    << EndLogLine;

  AssertLogLine( aFCB != NULL && aFCB == aFCB->Next && aFCB->Next == aFCB->Prev )
    << "_pkFiberUnblock():"
    << " target fiber not blocked "
    << " target FCB "      << (void*) aFCB
    << " FCB->Next "       << aFCB->Next
    << " FCB->Prev "       << aFCB->Prev
    << " ReadyQueueHead "  << (void*) aFASCB->ReadyQueueHead
    << EndLogLine;

  if( aFASCB->ReadyQueueHead == NULL )
    {
    aFASCB->ReadyQueueHead = aFCB;
    }
  else
    {
    aFCB->Next                 = aFASCB->ReadyQueueHead;
    aFCB->Prev                 = aFASCB->ReadyQueueHead->Prev;
    aFASCB->ReadyQueueHead->Prev->Next = aFCB;
    aFASCB->ReadyQueueHead->Prev       = aFCB;
    }
  BegLogLine( PKFXLOG_PK_FIBER )
    << "Unblock() "
    << " finished on "     << (void*) aFCB
    << " ReadyQueueHead "  << (void*) aFASCB->ReadyQueueHead
    << EndLogLine;
  }

static inline
void
pkFiberUnblock(pkFiberControlBlockT * aFCB = NULL )
  {
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();
  _pkFiberUnblock( fascb, aFCB );
  }

 inline
void
PkFiberUnblock( FID aFID )
  {
  // if local, do here, if remote use active packets to reach back end fx
  // really questionable whether this should be provided.
  assert(0); //not implemented
  }

//////////////////////////////////////////////////////////////////////
// Yield - see if the currently running fiber is still highest pri in run queue
//       - no global interface
// THESE ARE TOP OVERHEAD PRODUCTING CODE PATHS ***THINK*** BEFORE ADDING TO IT
//////////////////////////////////////////////////////////////////////
static
inline
void
_pkFiberSwapToReadyQueueHead( pkFiberActorSchedulerControlBlockT* aFASCB)
  {
  BegLogLine( PKFXLOG_BGL_SCHED_HI_FREQ )
    << "_pkFiberSwapToReadyQueueHead():"
    << " SWAPPING TO "
    << " ReadyQueueHead "   << (void*) aFASCB->ReadyQueueHead
    << " ReadyQueueHead->mFiberMainFx " << (void*) aFASCB->ReadyQueueHead->mFiberMainFx
    << " @ "                << (void*) (&(aFASCB->ReadyQueueHead->mFiberMainFx))
    << " ReadyQueueHead->mFiberArg "    << (void*) aFASCB->ReadyQueueHead->mFiberArg
    << EndLogLine;

  assert( aFASCB->ReadyQueueHead    != NULL );
  assert( aFASCB->RunningFibersFCB  != NULL );
  assert( aFASCB->ReadyQueueHead != aFASCB->RunningFibersFCB );  // why is this wasteful thing happening?
  pkFiberControlBlockT * FromFCB = aFASCB->RunningFibersFCB;
  aFASCB->RunningFibersFCB = aFASCB->ReadyQueueHead; // This is the only place this should be changed

  BegLogLine( PKFXLOG_BGL_SCHED_HI_FREQ )
    << "_pkFiberSwapToReadyQueueHead():"
    << " FromFCB @" << (void*) FromFCB
    << " FromFCB->SwapData @"        << (void*) &FromFCB->SwapData
    << " ... @"                      << (void*) (((unsigned)&FromFCB->SwapData) + sizeof( FromFCB->SwapData ))
    << " ReadyQueueHead->SwapData @" << (void*) &aFASCB->ReadyQueueHead->SwapData
    << " ... @"                      << (void*) (((unsigned)&aFASCB->ReadyQueueHead->SwapData) + sizeof( FromFCB->SwapData))
    << " sizeof( SwapData ) "        << sizeof( FromFCB->SwapData )
    << EndLogLine;

  _BG_CoRoutine_Swap( & (FromFCB->SwapData), & (aFASCB->ReadyQueueHead->SwapData) );

  BegLogLine( PKFXLOG_BGL_SCHED_HI_FREQ )
    << "_pkFiberSwapToReadyQueueHead():"
    << " finished - AFTER SWAP "
    << " ReadyQueueHead "   << (void*) aFASCB->ReadyQueueHead
    << " RQH->Next "        << (void*) aFASCB->ReadyQueueHead->Next
    << " RQH->Prev "        << (void*) aFASCB->ReadyQueueHead->Prev
    << EndLogLine;
  assert( FromFCB == aFASCB->RunningFibersFCB );
  return;
  }

static inline
void
_pkAdvanceReadyQueueHead(pkFiberActorSchedulerControlBlockT* aFASCB)
  {
  aFASCB->ReadyQueueHead = aFASCB->ReadyQueueHead->Next;
  }

// Whether we're building a version which supports controlled stop on timer
enum {
    k_PkSupportsExpiry = 1 
} ;

static
inline
void
_pkFiberYield(pkFiberActorSchedulerControlBlockT* aFASCB)
  {
  BegLogLine( 0 && PKFXLOG_PK_FIBER ) // this log line is hit fairly often.
    << "_pkFiberYield():"
    << " starting "
    << EndLogLine;

  // Kick the pipes - this is how we send and receive everything - happens on yield!
  _pkKickPipes( aFASCB );

  long long TerminationTime = pkTerminationTime ;

  // This loop handles polling comms and scheduling when a fiber is ready to run.
  for(;;) // Either return or swap
    {
        if( k_PkSupportsExpiry && ( PkTimeGetRaw() > TerminationTime ) )
          {
            _pkExit(0) ;
          }
        if( aFASCB->ReadyQueueHead != NULL )
          {
          // Advance the ready queue head to allow fairness - if there is only one fiber, it will still
          // be ready to run. (More overhead though!!!)
          aFASCB->ReadyQueueHead = aFASCB->ReadyQueueHead->Next;
    
          if( aFASCB->ReadyQueueHead == aFASCB->RunningFibersFCB )
            {
            // this log line is very productive when there's nothing to run
            BegLogLine( 0 && PKFXLOG_PK_FIBER )
              << "_pkFiberYield():"
              << " finished - NO SWAP "
              << EndLogLine;
            return;
            }
          else
            {
            _pkFiberSwapToReadyQueueHead( aFASCB );
            return;
            }
          }
        // At the bottom because we kicked the pipes on entry.
        _pkKickPipes( aFASCB );
    }
  }

 inline
void
PkFiberYield()
  {
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();
  _pkFiberYield( fascb );
  }

//////////////////////////////////////////////////////////////////////
// PkCo_Barrier ... hmmmmmm.
//////////////////////////////////////////////////////////////////////
// this function only works on ONE CORE ... this is not the end of the road on barriers.
static
inline
void
PkCo_Barrier()
  {
  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "PkCoBarrier() END OF THE LINE"
    << EndLogLine;

  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  BGLGI_BarrierWithWaitFunction( (void*)_pkKickPipes, (void*) fascb );

  //Platform::Control::BarrierWithWaitFunction( (void*) _pkKickPipes, NULL );

}

//////////////////////////////////////////////////////////////////////
// PkCo_Barrier ... hmmmmmm. Yep. Hmmmm... Tree reduction calls yield
// If the packet never came in.
//////////////////////////////////////////////////////////////////////
// this function only works on ONE CORE ... this is not the end of the road on barriers.
static inline
void
PkCo_BarrierWithYield()
  {
  BegLogLine( PKFXLOG_BGL_ACTOR )
    << "PkCoBarrier() END OF THE LINE"
    << EndLogLine;

  int Dummy;
  GlobalTreeReduce( (unsigned long *) &Dummy, (unsigned long *) &Dummy, sizeof( unsigned long ), _BGL_TREE_OPCODE_ADD );
}


//////////////////////////////////////////////////////////////////////
// Block - take fiber Running Fiber (only) off run queue to wait for an event
//       - it's a real question whether this should have user level interfaces
//       - it CANNOT be called globaly but only on the current fiber on
//         the local core.
//////////////////////////////////////////////////////////////////////

// ... again... there should never be a case where you need to block and don't already know
// which core you are on.  that's why no pkFiberBlock()

static
inline
void
pkFiberBlock(pkFiberActorSchedulerControlBlockT* aFASCB)
  {
  assert( aFASCB == pkCoreGetFASCB() );

  BegLogLine( PKFXLOG_PK_FIBER )
    << "Block() "
    << " starting "
    << " ReadyQueueHead " << (void*) aFASCB->ReadyQueueHead
    << " Next "          << (void*) aFASCB->RunningFibersFCB->Next
    << " Prev "          << (void*) aFASCB->RunningFibersFCB->Prev
    << EndLogLine;

  // The following block will move ReadyQueueHead off blocking FCB if there is another FCB in queue
  if( aFASCB->ReadyQueueHead == aFASCB->RunningFibersFCB )
    {
    aFASCB->ReadyQueueHead = aFASCB->ReadyQueueHead->Next;
    BegLogLine( PKFXLOG_PK_FIBER )
      << "Block() "
      << " RunningFibersFCB == ReadyQueueHead "
      << " ReadyQueueHead = ReadyQueueHead->Next "
      << aFASCB->ReadyQueueHead
      << EndLogLine;
    }

  // Pull running thread's FCB out of ReadyQueueHead
  pkFiberControlBlockT *Next = aFASCB->RunningFibersFCB->Next;
  pkFiberControlBlockT *Prev = aFASCB->RunningFibersFCB->Prev;
  Next->Prev = Prev;
  Prev->Next = Next;

  // Make running thread's FCB self referencing
  aFASCB->RunningFibersFCB->Next = aFASCB->RunningFibersFCB;
  aFASCB->RunningFibersFCB->Prev = aFASCB->RunningFibersFCB;

  if( aFASCB->ReadyQueueHead == aFASCB->RunningFibersFCB )
    aFASCB->ReadyQueueHead = NULL;

  PkFiberYield();

  BegLogLine( PKFXLOG_PK_FIBER )
    << "Block() "
    << " finished "
    << EndLogLine;
  }

//////////////////////////////////////////////////////////////////////
// Starter - function at base of fiber stack
//////////////////////////////////////////////////////////////////////

// This starter function catches returns from the fiber entry point
static void
_pkFiberStarter( void * aFCB )
  {
  assert( aFCB != NULL );
  pkFiberControlBlockT * FCB = (pkFiberControlBlockT *) aFCB;

  pkFiberActorSchedulerControlBlockT* aFASCB = pkCoreGetFASCB();

  BegLogLine( PKFXLOG_PK_FIBER )
    << "_pkFiberStarter() "  << " Arg FCB " << aFCB
    << " START CALL TO "
    << " Func "           << (void*) FCB->mFiberMainFx
    << " @ "              << (void*) (&(FCB->mFiberMainFx))
    << " Arg "            << (void*) FCB->mFiberArg
    << " Flags "          << (void*) FCB->mFlags
    << " Status "          << (void*) FCB->mStatus
    << " RTFCB->Next "    << (void*) aFASCB->RunningFibersFCB->Next
    << " RTFCB->Prev "    << (void*) aFASCB->RunningFibersFCB->Prev
    << " ReadyQueueHead " << (void*) aFASCB->ReadyQueueHead
    << " RQH->Head "      << (void*) aFASCB->ReadyQueueHead->Next
    << " RQH->Prev "      << (void*) aFASCB->ReadyQueueHead->Prev
    << EndLogLine;

  StrongAssertLogLine( aFASCB->RunningFibersFCB == FCB ) << EndLogLine;
  StrongAssertLogLine( FCB->mFiberMainFx != NULL ) << EndLogLine;

  FCB->mFiberMainFx( FCB->mFiberArg );
  BegLogLine( PKFXLOG_PK_FIBER )
    << "_pkFiberStarter() "  << " Arg FCB " << aFCB
    << " FIBER RETURNED TO SYSTEM "
    << " Func "           << (void*) FCB->mFiberMainFx
    << " @ "              << (void*) (&(FCB->mFiberMainFx))
    << " Arg "            << (void*) FCB->mFiberArg
    << " Flags "          << (void*) FCB->mFlags
    << " Status "          << (void*) FCB->mStatus
    << " RTFCB->Next "    << (void*) aFASCB->RunningFibersFCB->Next
    << " RTFCB->Prev "    << (void*) aFASCB->RunningFibersFCB->Prev
    << " ReadyQueueHead " << (void*) aFASCB->ReadyQueueHead
    << " RQH->Head "      << (void*) aFASCB->ReadyQueueHead->Next
    << " RQH->Prev "      << (void*) aFASCB->ReadyQueueHead->Prev
    << EndLogLine;

  // this should be mStatus
  FCB->mStatus |= PK_FIBER_STATUS_EXITED;

  // do join stuff here... should be a way to detach threads
  if( FCB->mJoiner != NULL )
    {
    StrongAssertLogLine( FCB->mFlags ^ PK_FIBER_FLAG_JOIN_REQUIRED )
      << " JOIN TO NON-JOINABLE FIBER "
      << " Func "           << (void*) FCB->mFiberMainFx
      << " @ "              << (void*) (&(FCB->mFiberMainFx))
      << " Arg "            << (void*) FCB->mFiberArg
      << " Flags "          << (void*) FCB->mFlags
      << " Status "          << (void*) FCB->mStatus
      << " RTFCB->Next "    << (void*) aFASCB->RunningFibersFCB->Next
      << " RTFCB->Prev "    << (void*) aFASCB->RunningFibersFCB->Prev
      << " ReadyQueueHead " << (void*) aFASCB->ReadyQueueHead
      << " RQH->Head "      << (void*) aFASCB->ReadyQueueHead->Next
      << " RQH->Prev "      << (void*) aFASCB->ReadyQueueHead->Prev
      << EndLogLine;

    FCB->mStatus |= PK_FIBER_STATUS_JOIN_PROCESSED;
    pkFiberUnblock( FCB->mJoiner );
    }

  if( FCB->mStatus ^ PK_FIBER_STATUS_JOIN_PROCESSED  )
    {
    BegLogLine( 0 )
      << "_pkFiberStarter() "
      << " FCB->mStatus.mJoinsProcessed set "
      << " *********** MEMORY LEAK ---- NOT FREEING STACK *********  NEED INFRASTRUCTURE"
      << " Fiber calling free() on stack @"
      << (void*) FCB->mFiberMemoryMallocBase
      << EndLogLine;

//NEED THIS PkHeapFree( (char*) FCB->mFiberMemoryMallocBase );
    }
  // PROBLEM HERE - we've freed the stack but we need to call yield.
  // For now, we use the stack one last time
  pkFiberBlock( aFASCB );
  }

//////////////////////////////////////////////////////////////////////
// Join
//////////////////////////////////////////////////////////////////////
static
inline
void
_pkFiberJoin( pkFiberActorSchedulerControlBlockT* aFASCB, pkFiberControlBlockT *aFCB )
  {
  StrongAssertLogLine( aFCB != NULL ) // for now, only one joiner
    << "_pkFiberJoin(): "
    << "aFCB is NULL "
    << EndLogLine;

  BegLogLine( PKFXLOG_PK_FIBER )
    << " _pkFiberJoin(): "
    << " STARTING JOIN on FCB "         << (void*) aFCB
    << " aFCB->mStatus.mExited "        << (aFCB->mStatus ^ PK_FIBER_STATUS_EXITED)
    << " aFCB->mStauts.mDetatched "     << (aFCB->mStatus ^ PK_FIBER_STATUS_JOIN_PROCESSED)
    << " aFCB->mFlags.mJoinRequred "    << (aFCB->mFlags  ^ PK_FIBER_FLAG_JOIN_REQUIRED)
    << EndLogLine;

  StrongAssertLogLine( aFCB->mFlags ^ PK_FIBER_FLAG_JOIN_REQUIRED )
    << " _pkFiberJoin(): "
    << " THIS THREAD WILL NOT PROCESS A JOIN "
    << " fix: add PK_FIBER_JOIN_REQUIRED  to PkFiberCreate flags "
    << EndLogLine;

  if( aFCB->mStatus ^ PK_FIBER_STATUS_EXITED && ( ! (aFCB->mStatus ^ PK_FIBER_STATUS_JOIN_PROCESSED) ) )
    {
    BegLogLine( PKFXLOG_PK_FIBER )
      << " _pkFiberJoin(): "
      << " TargetFCB->mExited == 1 && mDetached == 0 "
      << " Freeing stack at base address "
      << " aFCB "             << (void*) aFCB
      << " malloc base addr " <<  aFCB->mFiberMemoryMallocBase
      << EndLogLine;
    PkHeapFree( (char*) aFCB->mFiberMemoryMallocBase );
    return;
    }
  else
    {
    StrongAssertLogLine( aFCB->mJoiner == NULL ) // for now, only one joiner
      << "_pkFiberJoin(): "
      << " FCB has a non-null mJoiner field - it's probably been joined already but only one join allowed "
      << " aFCB "             << (void*) aFCB
      << " aFCB->mJoiner "    << (void*) aFCB->mJoiner
      << EndLogLine;

    StrongAssertLogLine( aFASCB->RunningFibersFCB != aFCB )  // would be deadlock without third party kill
      << "_pkFiberJoin(): "
      << " A fiber cannot join itself "
      << EndLogLine;

    aFCB->mJoiner = aFASCB->RunningFibersFCB;
    pkFiberBlock( aFASCB );
    }

  BegLogLine( PKFXLOG_PK_FIBER )
    << "_pkFiberJoin(): "
    << " FINISHED JOIN on FCB " << (void*) aFCB
    << EndLogLine;
  }

// CreatepkFiberControlBlock() allocates a tcb/stack area.  This area is buffered so that
// thread ids can be the actual address of the FCB.

static void
_pkCreateFiberControlBlock( pkFiberControlBlockT * *aFCB, unsigned aReqStackSize )
  {
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();

  unsigned MallocStackSize = FCB_AlignmentPadding + sizeof( pkFiberControlBlockT ) + aReqStackSize;

  BegLogLine( PKFXLOG_PK_FIBER )
    << "CreateFCB() "
    << " ReqStackSize "          << (void*) aReqStackSize      << " " << aReqStackSize
    << " sizeof( pkFiberControlBlockT ) " << (void*) sizeof( pkFiberControlBlockT ) << " " << sizeof( pkFiberControlBlockT )
    << " FCB_AlignmentPadding "   << (void*) FCB_AlignmentPadding << " " << FCB_AlignmentPadding
    << " MallocStackSize "       << (void*) MallocStackSize << " " << MallocStackSize
    << EndLogLine;

  ////unsigned MallocAddr = (unsigned) Platform::Heap::Allocate( MallocStackSize );
  unsigned MallocAddr = (unsigned) PkHeapAllocate( MallocStackSize );

  if( MallocAddr == 0 )
    _exit( __LINE__ * -1); // PLATFORM_ABORT( "Failed to allocate a thread stack" );

  // Force allignment on stack to reduce the number of significant bits in the thread id/address
  // Note that FCB is at the high address end of the malloced region.  The stack begins at the base
  // (low) address of the FCB and grows toward zero.
  unsigned FCB_Address = ( MallocAddr + aReqStackSize + FCB_AlignmentPadding - 1 ) &  ( ~FCB_AlignmentMask);

  pkFiberControlBlockT * FCB = (pkFiberControlBlockT *) FCB_Address;

  BegLogLine( PKFXLOG_PK_FIBER )
    << "CreateFCB() "
    << " New FCB "              << (void*) FCB
    << " aReqStackSize "        << (void*) aReqStackSize
    << " MallocSize "           << (void*) MallocStackSize
    << " MallocAddr "           << (void*) MallocAddr
    << " Used FCB addr "        << (void*) FCB_Address
    << " FiberNumber "          << fascb->mNextFiberNumber
    << EndLogLine;


  FCB->mExited = 0;
  FCB->mFiberMemoryMallocBase = (void *) MallocAddr;  // so we can free the the stack later
  FCB->mFiberMainFx           = NULL;
  FCB->mFiberArg              = 0;
  FCB->mFlags                 = 0;
  FCB->mStatus                = 0;
  FCB->mJoiner                = NULL;
  FCB->Next                   = FCB;
  FCB->Prev                   = FCB;
  FCB->mFASCB                 = fascb;
  FCB->mFiberNumber           = fascb->mNextFiberNumber;

  bzero( FCB->StackSeparationPadding, sizeof ( void* ) * 16 );

  fascb->mNextFiberNumber++;

  *aFCB = FCB;
  }

static
pkFiberControlBlockT *
pkCreateFiber( unsigned                aReqStackSize,
               BG_CoRoutineFunctionType aFiberMainFx,
               void *                   aFiberArg,
               unsigned                 aFlags  )
  {
  BegLogLine( PKFXLOG_PK_FIBER )
    << "CreateFiber() "
    << " aReqStackSize "     << (void*) aReqStackSize
    << " aFiberMainFx "      << (void*) aFiberMainFx
    << " aFiberArg "         << (void*) aFiberArg
    << " aFlags "            << (void*) aFlags
    << " _pkFiberStarter "   << (void*) _pkFiberStarter
    << EndLogLine;

  pkFiberControlBlockT* FCB;

  // Allocate the block of memory that will hold the FCB and the thread stack.
  // Only one addr - FCB - is returned.  The stack is toward zero from this and the tcb the other way.
  _pkCreateFiberControlBlock( & FCB, aReqStackSize );

  FCB->mFiberMainFx           = aFiberMainFx;
  FCB->mFiberArg              = aFiberArg;
  FCB->mFlags                 = aFlags;

  BegLogLine( PKFXLOG_PK_FIBER )
    << "CreateFiber() "
    << " New FCB "           << (void*) FCB
    << " FCB->mFiberMainFx " << (void*) FCB->mFiberMainFx
    << " FCB->mFiberArg "    << (void*) FCB->mFiberArg
    << " FCB->mFlags "       << (void*) FCB->mFlags
    << " FCB->SwapData @"    << (void*) &FCB->SwapData
    << " sizeof(SwapData) "  << sizeof( FCB->SwapData )
    << " _pkFiberStarter "   << (void*) _pkFiberStarter
    << EndLogLine;

  // Init all but the main threads SwapData area.
  // Note: FCB us used as the stack pointer since it is aligned.
  // Stack grows from FCB toward zero, tcb body is the other way.
  _BG_CoRoutine_Init( & FCB->SwapData, _pkFiberStarter, FCB, (void*)FCB  );

  pkFiberUnblock( FCB );

  return( FCB );
  }

static
inline
void
pkFiberJoin( pkFiberControlBlockT *aFCB )
  {
  pkFiberActorSchedulerControlBlockT* fascb = pkCoreGetFASCB();
  _pkFiberJoin( fascb, aFCB );
  }


inline
void
PkFiberJoin( unsigned long long aFID )
  {
  assert(0);
  }

#endif

