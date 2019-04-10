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
 #include <blade.h>

#include <pk/fxlogger.hpp>
#include <pk/platform.hpp>
////#include "ShmemType.hpp"


template< class T_TrueType, int T_AlignmentBits = 5 >
class ShmemBuffer
  {
  public:

    typedef T_TrueType TrueType;
    enum { AlignmentBits    = T_AlignmentBits };
    enum { Alignment        = 0x01 << AlignmentBits };
    enum { TotalCacheLines  = ( (sizeof( TrueType ) + Alignment - 1) / Alignment) };
    enum { TotalBytes       = TotalCacheLines * Alignment };
    enum { TotalLongDoubles = TotalBytes / sizeof( long double ) };
    ///// enum { TotalFootPrint = sizeof( TrueType ) + (Alignment - ((sizeof( T_TrueType ) + (Alignment - 1)) % Alignment) ) };

    __attribute__ (( aligned( T_Alignment ) ))
    union
      {
      //char mBuf[ TotalFootPrint ];
      long double Buf[ TotalLongDoubles ];
      T_TrueType mInstance;
      };

    inline
    T_TrueType &
    GetPayloadRef()
     {
     return( mInstance );
     }

    inline
    T_TrueType *
    GetPayloadPtr()
     {
     return( & mInstance );
     }

  };

#if 0 /// already define in platform.hpp
template < class TYPE >
class AtomicNative
  {
  public:
    __attribute__ (( aligned( T_Alignment ) ))
    TYPE mVar;

    inline
    TYPE
    operator ++ ()
      {
      mVar++;
      return( mVar );
      }

    inline
    TYPE &
    operator = ()
      {
      return( mVar )
      }

  };
#endif

//////////bgf unused: typedef ShmemNative<unsigned> ShmemCounter;



#define BGL_SHMEM_MAX_PARTITION_SIZE                      (512)

extern "C" {
#include <BGL_TreeBringup.h>
//////int BGL_TreeRouteComputeNodes(int *, int, int* ) ;
};

#define SHMEM_ABORT(ERRSTR) {BegLogLine(1) << "SHMEM_ABORT >" << ERRSTR << "<" << EndLogLine;exit(-1);}

inline int
my_pe(void)
  {
  int  Rank = BGLPartitionGetRankXYZ();
  return( Rank );
  }

inline int
num_pes(void)
  {
  int PartSize = BGLPartitionGetSize();
  return( PartSize );
  }

void
ShmemExit()
  {
  };

// This function is similar to shmem_get(), but does not wait for the
// remote data to arrive before returning. The advantage is that this
// allows multiple get operations (or a get operation and any code that
// does not require the use of target) to proceed concurrently.
// As soon as the data in target becomes valid, the value pointed
// to by *pcount is incremented.
// shmem_cget() is generally used in conjunction with shmem_cwait()
// and is an individual function.
void
shmem_cget (long *target, long *source, int nlong, int node, int *pcount)
  {
  }

void
shmem_double_get( double *target, const double *source, size_t len, int pe)
  {
  return;
  }

int IsProtocolClear( int Mask = 0xFFFFFFFF );

void ShmemBarrier(void)
  {
  BGLPartitionBarrier();
  }

void
ShmemLocalDataBarrier(void)
  {
  Platform::Memory::ExportFence();
  }

void
ShmemGlobalDataBarrier(void)
  {
  BegLogLine(1)
    << "ShmemGlobalDataBarrier START "
    << " Calling IsProtocolClear() "
    << EndLogLine;

  while( ! IsProtocolClear() )
    {
    #ifndef PK_BGL
      sleep(1);
    #endif
    }

  BegLogLine(1)
    << "ShmemGlobalDataBarrier FINISH "
    << " Calling IsProtocolClear() "
    << EndLogLine;
  }

void ShmemControlBarrier(void)
  {
  BGLPartitionBarrier();
  }


typedef struct InternalAddress
  {
  unsigned char dummy;
  unsigned char mZ;
  unsigned char mY;
  unsigned char mX;
  } InternalAddress ;


enum{ ProtocolMax = 8 };  // must be a power of two
enum{ AllProtocolsMask = ProtocolMax - -1 };

long long GlobalConserverOperationsStarted;  // [32];
long long GlobalConserverOperationsFinished; // [32];

// Internals of the 'reduce'
int FullPartitionClass ;
int FullPartitionRoot = 0 ;

int
IsProtocolClear( int Mask )
  {
  /////static int Entry1 = 1;
  /////if( Entry1 )
  /////  {
  /////  Entry1 = 0;
  /////  // Set up the tree routing
  /////  int rc = BGL_TreeRouteComputeNodes(&FullPartitionRoot,1,&FullPartitionClass) ;
  /////  if (rc!= BGLERR_SUCCESS)
  /////    SHMEM_ABORT("IsProtocolClear() TreeRouteCOmputeNodes() FAILED" );
  /////  }

  // sleazy way to get aligned data array for torus packet
  char Data[ _BGL_TREE_PKT_MAX_BYTES + 32 ];
  long long *AlignedData = (long long *)(((unsigned)Data & 0xFFFFFFE0) + 32);

  AlignedData[0] = GlobalConserverOperationsStarted;
  AlignedData[1] = GlobalConserverOperationsFinished;

  // find a nice alignment for the packet - copy the long longs into first 64bytes

  BegLogLine(0)
    << "IsProtocolClear(): BEFORE TREE OP "
    << " TotalTorusSendPackets "
    << AlignedData[0]
    << " TotalTorusRecvPackets "
    << AlignedData[1]
    << EndLogLine;

  _BGL_TreeHwHdr addReduceHdr ;
  _BGL_TreeHwHdr addReduceHdrReturned ;

  BGLTreeMakeAluHdr( &addReduceHdr,
                     FullPartitionClass,             // class
                     _BGL_TREE_OPCODE_ADD,           // opcode
                     _BGL_TREE_OPSIZE_BIT64          // size
                   ) ;

  int maxSendResult = BGLTreeSend(
                      _BGL_TREE_BALANCED_MODE_APP_VC, // class
                      &addReduceHdr,                  // Header we made before
                       AlignedData,                   // Operands being sent
                      _BGL_TREE_PKT_MAX_BYTES         // appropriate number of blocks, by construction
                      ) ;

  if( maxSendResult > 0 )
    SHMEM_ABORT("IsProtocolClear() : Tree send failed.");

  int maxRecvResult = BGLTreeReceive(
                      _BGL_TREE_BALANCED_MODE_APP_VC, // class
                      &addReduceHdrReturned,          // Header after circulating the tree
                       AlignedData,                   // Operands being received
                      _BGL_TREE_PKT_MAX_BYTES         // appropriate number of blocks, by construction
                      ) ;

  if( maxRecvResult > 0 )
    SHMEM_ABORT("IsProtocolClear() : Tree recv failed.");

  int rc = (AlignedData[0] == AlignedData[1]) ;

  BegLogLine(0)
    << "IsProtocolClear(): AFTER TREE OP : "
    << ( rc ? " CLEAR " : " NOT CLEAR " )
    << " Protocol Starts "
    << AlignedData[0]
    << " Protocol Finishes "
    << AlignedData[1]
    << EndLogLine;

  return( rc );
  }


#if 0 /// THIS SHOULD COME FROM platform.cpp now
void
BGLTorusSimpleActivePacketSend( _BGL_TorusPktHdr * pktHdr,
                                Bit32              ActorFx,
                                int                x, int y, int z,
                                int                len,
                                void             * data )
  {
  //BegLogLine( PKFXLOG_BGL_TORUS_ACTIVE_PACKET_SEND )
  //    << "BGLTorusSimpleActivePacketSend:: "
  //    << " Send To: " << x << ","<< y << ","<< z
  //    << EndLogLine;

  // RULE TO FORCE SENDS OF ALL PACKETS WHEN USING BOTH CORES.
  // Application loopback on stack is dangerous because it causes a dual writer situation.
  // On BG/L, this could cause sharing of cache lines between cores and thus, data loss.
  #ifdef BGL_USE_BOTH_CORES
  #define BGL_TORUS_SEND_LOCAL_ACTORS
  #endif

#ifndef BGL_TORUS_SEND_LOCAL_ACTORS  // Allow a flag to test loop back

  int myPx, myPy, myPz;
  BGLPartitionGetCoords( &myPx, &myPy, &myPz );
  if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
    {
    ((BGLTorusBufferedActivePacketHandler)(ActorFx))( (Bit8 (*)[240])data, pktHdr->swh1.arg, pktHdr->swh0.extra );
    }

  else

#endif

    {

    int PollCount = 0;
    while( 1 )
      {
      int rc = BGLTorusSendPacketUnalignedNonBlocking(   pktHdr,  // Previously created header to use
                                                         data,    // Source address of data (16byte aligned)
                                                         len );   // Payload bytes = ((chunks * 32) - 16)
      // send succeds if rc == 0 , otherwise go round, drain network and try again.
      if( rc == 0 )
        break;

      PollCount++;

      #ifdef BGL_TORUS_COMPUTE_PROC_EXECUTES_ACTORS
        // NEED TO DRAIN THE NETWORK
        while( 1 )
          {
          int count = 1;
          BGLTorusPoll( &count );
          if( count == 1 )
            break;
          }
      #endif

      #ifndef SHMEM_SPINWAIT
        if( (PollCount % 16) == 0 )
          {
          //BegLogLine(1)
          //  << "Polling to send active message packet! "
          //  << PollCount
          //  << EndLogLine;
          usleep(10000);
          }
      #endif
      }

    }

  }
#endif

// This structure is used to tell when we are looking at the
// first packet of a multi packet message.
// It is extreamly important to be able to tell when the first
// packet of a new multipacket counter is arriving so that
// the counter area in the message destination can be zeroed.
// This method requires ack-ing.  Acking advances the window.
typedef struct
  {
  unsigned short mStartedBits  ;       // This bit is 1 when recving, 0 when waiting
  unsigned short mFinishedBits ;       // This bit is 1 when ready to ack, 0 when acked
  } OutgoingMessageSlotState;

enum { MsgSlotCount = 8 * sizeof(short) };  // Number of bits used in a short from above struct

OutgoingMessageSlotState OutgoingMessageSlotStateTable[ BGL_SHMEM_MAX_PARTITION_SIZE ];

// Protocols built on this may use these 32 bits how ever they like.
// This state exists only on the receive side.
typedef struct
  {
  unsigned mState       : 30  ;
  unsigned mFinishedBit :  1  ;
  unsigned mStartedBit  :  1  ;
  } IncomingMessageSlotState ;

IncomingMessageSlotState IncomingMessageSlotStateTable [ MsgSlotCount ][ BGL_SHMEM_MAX_PARTITION_SIZE ];


// Returns -1 if no slots avail or 0..MsgSlotCount if a slot is found.
int
ReserveOutgoingSlot( int aDestRank )
  {
  // NEED NEED NEED: to make this a crit section if CP as well as IOP use it.
  OutgoingMessageSlotState * outmss = & (OutgoingMessageSlotStateTable[ aDestRank ]) ;

  // scan for free slot.
  int slot;
  for( slot = 0; slot < MsgSlotCount; slot++ )
    {
    unsigned short MsgBitMask = 0x01 << slot;
    if( ! ( outmss->mStartedBits & MsgBitMask ) )
      {
      // It is a problem if that slot doesn't show as started.
      if( outmss->mFinishedBits & MsgBitMask )
        {
        SHMEM_ABORT("Multipacket protocol error - slot selected as free to start has finished bit set");
        }
      outmss->mStartedBits  |= MsgBitMask;
      break;  //got what we came for...
      }
    }

  BegLogLine( 1 )
    << "ReserveOutgoingSlot() : "
    << " DestRank "
    << aDestRank
    << " SlotNo "
    << slot
    << EndLogLine;

  if( slot == MsgSlotCount )
    return( -1 );
  return( slot );
  }


// Inlining probably takes care of it... but this could be more tightly integrated with next fx.
inline
int
IncrementSlotStateValue( int aSourceRank, int aSlotNo )
  {
  // Short hand I hope doesn't cost after compilation.
  // NOTE: THE ADDRESS OF THE INMSS IS **REDUNDANT** WITH SlotNo and SourceRank - CRUSH HEADER????
  IncomingMessageSlotState * inmss = & (IncomingMessageSlotStateTable [aSlotNo] [ aSourceRank ]);

  if( ! inmss->mStartedBit )
    SHMEM_ABORT("Multipacket protocol error - trying to increment slot state but not started");

  if( inmss->mFinishedBit )
    SHMEM_ABORT("Multipacket protocol error - trying to increment a slot state but already finished");

  inmss->mState++;

  int ssv = inmss->mState;

  BegLogLine( 1 )
    << "IncrementSlotStateValue() : "
    << " SrcRank "
    << aSourceRank
    << " SlotNo "
    << aSlotNo
    << " Incoming message state value now "
    << inmss->mState
    << EndLogLine;

  return( ssv );
  }

unsigned int
GetSlotStateValue( int aSourceRank, int aSlotNo )
  {
  // Short hand I hope doesn't cost after compilation.
  // NOTE: THE ADDRESS OF THE INMSS IS **REDUNDANT** WITH SlotNo and SourceRank - CRUSH HEADER????
  IncomingMessageSlotState * inmss = & (IncomingMessageSlotStateTable [aSlotNo] [ aSourceRank ]);

  if( ! inmss->mStartedBit )
    SHMEM_ABORT("Multipacket protocol error - asking for Slot state of a not started message is not allowed");

  if( inmss->mFinishedBit )
    SHMEM_ABORT("Multipacket protocol error - asking for Slot state of a finish message is not allowed");

  int ssv = inmss->mState;

  BegLogLine( 1 )
    << "GetSlotStateValue() "
    << " SrcRank "
    << aSourceRank
    << " SlotNo "
    << aSlotNo
    << " StateValue "
    << ssv
    << " StartedBit "
    << inmss->mStartedBit
    << " FinishedBit "
    << inmss->mFinishedBit
    << EndLogLine;

  return( ssv );
  }

// This routine is called on absolutly every multipacket packet that arrives.
// NOTE: we garantee that the slot state is zero - BEFORE WE GET HERE so no branch in this fx.
inline
int
AssertStartOnSlot( int aSourceRank, int aSlotNo )
  {
  // Short hand I hope doesn't cost after compilation.
  // NOTE: THE ADDRESS OF THE INMSS IS **REDUNDANT** WITH SlotNo and SourceRank - CRUSH HEADER????
  IncomingMessageSlotState * inmss = & (IncomingMessageSlotStateTable [aSlotNo] [ aSourceRank ]);

  // it is an error if this message has finished and another starts before an ack clears the bits
  if( inmss->mFinishedBit )
    SHMEM_ABORT("Multipacket protocol error - starting new message slot that hasn't been acked");

  // return is whether this message has been started
  int ThisIsFirstPacket = ! inmss->mStartedBit ;

  // set the bit which flags a message as started
  inmss->mStartedBit = 1;

  if( ThisIsFirstPacket )
    BegLogLine( 1 )
      << "AssertStartOnSlot() RCVS 1ST PKT OF MULTIPACKET MSG : "
      << " SrcRank "
      << aSourceRank
      << " SlotNo "
      << aSlotNo
      << EndLogLine;

  BegLogLine( 1 )
    << "AssertStartOnSlot() : "
    << " SrcRank "
    << aSourceRank
    << " SlotNo "
    << aSlotNo
    << " FirstPacketFlag "
    << ThisIsFirstPacket
    << " StartedBit "
    << inmss->mStartedBit
    << " FinishedBit "
    << inmss->mFinishedBit
    << EndLogLine;

  return( ThisIsFirstPacket );
  }

typedef struct
  {
  int mAckFromRank;
  int mSlotNo;
  } PutMultiPacket_Ack_ActorPacket ;

int
PutMultiPacket_Ack_ActorFx( void *pkt )
  {

  // NEED NEED: this is a crit sect - AND IN AN ACTOR!!! - If CP_main can get to these bits

  PutMultiPacket_Ack_ActorPacket * aap = (PutMultiPacket_Ack_ActorPacket *) pkt;

  OutgoingMessageSlotState * outmss = & (OutgoingMessageSlotStateTable[ aap->mAckFromRank ]) ;

  unsigned short MsgSlotBitMask = 0x01 << aap->mSlotNo ;

  BegLogLine(1)
    << "PutMultiPacket_Ack_ActorFx() SLOT SEND BEFORE CLEARED "
    << " Ack From Rank "
    << aap->mAckFromRank
    << " SlotNo "
    << aap->mSlotNo
    << " BitMask "
    << FormatString("%02X")
    << MsgSlotBitMask
    << " StartedBit "
    << (outmss->mStartedBits & MsgSlotBitMask)
    << " FinishedBit "
    << (outmss->mFinishedBits & MsgSlotBitMask)
    << EndLogLine;

  if( ! outmss->mStartedBits & MsgSlotBitMask )
    {
    SHMEM_ABORT("Multipacket protocol error - trying to ack a slot that doesn't show as started");
    }

  // Clear out bits making slot ready to use again.
  outmss->mStartedBits  &= ~ MsgSlotBitMask;
  outmss->mFinishedBits &= ~ MsgSlotBitMask;

  BegLogLine(1)
    << "PutMultiPacket_Ack_ActorFx() SLOT SEND AFTER CLEARED "
    << " Ack From Rank "
    << aap->mAckFromRank
    << " SlotNo "
    << aap->mSlotNo
    << " BitMask "
    << FormatString("%02X")
    << MsgSlotBitMask
    << " StartedBit "
    << (outmss->mStartedBits & MsgSlotBitMask)
    << " FinishedBit "
    << (outmss->mFinishedBits & MsgSlotBitMask)
    << EndLogLine;


  GlobalConserverOperationsFinished++;


  BegLogLine(1)
    << "PutMultiPacket_Ack_ActorFx() "
    << " GlobalConserverOperationsFinished now = "
    << GlobalConserverOperationsFinished
    << "Based on Ack From Rank "
    << aap->mAckFromRank
    << " SlotNo "
    << aap->mSlotNo
    << EndLogLine;

  return(0);
  }

///void ExportFence( void*, int);

int MultiPacketEmergencyAckScanRequired = 1;

// This routine is called only on the last multipacket packet for each multipacket message.
void
AckFinish( int aSourceRank, int aSlotNo )
  {
  // Short hand I hope doesn't cost after compilation.
  IncomingMessageSlotState * inmss = & IncomingMessageSlotStateTable [ aSlotNo ] [ aSourceRank  ];

  BegLogLine(1)
    << "AckFinish() "
    << "aSourceRank "
    << aSourceRank
    << " aSlotNo "
    << aSlotNo
    << " Reveive Packet Count "
    << inmss->mState
    << " StartedBit "
    << inmss->mStartedBit
    << " FinishedBit "
    << inmss->mFinishedBit
    << EndLogLine;

  // it is an error if this message has finished and another starts before an ack clears the bits
  if( inmss->mFinishedBit )
    SHMEM_ABORT("Multipacket protocol error - Finishing a message that has already been finished");

  // set the bit which flags this message as started
  inmss->mFinishedBit = 1;

  // it is an error if this message has finished and another starts before an ack clears the bits
  if( ! inmss->mStartedBit )
    SHMEM_ABORT("Multipacket protocol error - Finishing a message that hasn't been started");

  inmss->mStartedBit = 0;  // clear the star bit.

  PutMultiPacket_Ack_ActorPacket aap;
  aap.mAckFromRank    = BGLPartitionGetRankXYZ() ;
  aap.mSlotNo         = aSlotNo ;

  _BGL_TorusPktHdr BGLTorus_Hdr;

  int srcX, srcY, srcZ;
  BGLPartitionMakeXYZ( aSourceRank, &srcX, &srcY, &srcZ );

  BGLTorusMakeHdr( & BGLTorus_Hdr,        // Filled in on return
                   srcX,
                   srcY,
                   srcZ,                 // destination coordinates
                   0,                     // destination Fifo Group
                   (long unsigned int) PutMultiPacket_Ack_ActorFx,       // Active Packet Function matching signature above
                   0,                     // primary argument to actor
                   0,                     // secondary 10bit argument to actor
                   _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

  //Need a hint bit to send to self - Mark should help with this.

  if( aSourceRank == BGLPartitionGetRank() )
    BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );

  // It is always dangerous to send from within an actor, so be prepared to fail.
  int tsrc = BGLTorusSendPacketUnalignedNonBlocking( & BGLTorus_Hdr,    // Previously created header to use
                                                     & aap,             // Source address of data (16byte aligned)
                                                       sizeof(aap) );   // Payload bytes = ((chunks * 32) - 16)
  if( tsrc == -1 )
    {
    BegLogLine(1)
      << "AckFinish() "
      << "Failed to get a non-blocking ack send off - time to fix this section "
      << EndLogLine;

    // Actually, THE THING TO DO is to chain these ack requests together in a singly linked list using mState field.
    MultiPacketEmergencyAckScanRequired = 1;   // it's gonna be expensive, but we have to flag cond.
                                                 // this could be done with a list of indexs and a count.
    }
  else
    {
    // RACE in principle there is a wee little race condition here
    //      The new start could arrive between the time we send the ack and clear the bit and then
    //      there would be a complaint.
    inmss->mState       = 0 ; // maintained at known zero value unless in use.
    inmss->mFinishedBit = 0 ;
    }

  return;
  }

// The goals here are:
//  *) any packet can start or finish a long message
// Note that by using templates to generate header, most of the
// static info could be moved into the actor function pointer.

enum{ MultiPacket_PutBackeEnd_PaylaodSize = 220 };

typedef struct
  {
  unsigned short mSourceRank;  // mSourceRank and mSlot can be determined by single index to IncomingState
  unsigned char  mSlot;
  unsigned char  mPacketPayloadLen;
  unsigned short mTotalPacketCount; // def don't need 32 bits here.
  unsigned short mThisPacketIndex;  // needed only to figure out what mem to flush
  char *         mPacketDataDestAddr;
  char           mPayload[ MultiPacket_PutBackeEnd_PaylaodSize ];
  } PutMultiPacketBackEnd_ActorPacket ;

int
MultiPacketBackEnd_ActorFx( void * pkt, int aFlag0Put1Get1 )
  {
  PutMultiPacketBackEnd_ActorPacket * pab = (PutMultiPacketBackEnd_ActorPacket *) pkt;

  // This whole little patch could be done better... SourceRank and Slot can be combined into
  // as single smaller, more direct value.
  // yank out of header into proper int values
  int SourceRank = pab->mSourceRank;
  int Slot       = pab->mSlot;

  // Check if this is the first packet in this multi packet message
  // NOTE: if it is the first packet, the value of Slot State will already be 0
  int SlotState = AssertStartOnSlot( SourceRank, Slot );

  BegLogLine(1)
    << "PutMultiPacketBackEnd_ActorFx "
    << "Recv From : "
    << pab->mSourceRank
    << " Slot "
    << pab->mSlot
    << " PktInMsg (Now:Tot)"
    << GetSlotStateValue( SourceRank, Slot )
    << ":"
    << pab->mTotalPacketCount
    << " PktDatLen "
    << pab->mPacketPayloadLen
    << " PktDest Addr "
    << (void *) pab->mPacketDataDestAddr
    << EndLogLine;

  //NEED: FALSE SHARE need to check if we are in a boundary cond here
  memcpy( pab->mPacketDataDestAddr ,
          pab->mPayload,
          pab->mPacketPayloadLen );

  SlotState = IncrementSlotStateValue( SourceRank, Slot );

  // check if we're finished
  if( SlotState == pab->mTotalPacketCount )
    {
    int BackOffsetToMessageStart = pab->mThisPacketIndex * MultiPacket_PutBackeEnd_PaylaodSize;
    // fence memory to CP proc
    char * MsgBaseAddr = ((char *) pab->mPacketDataDestAddr) - BackOffsetToMessageStart;
    int    MsgMaxLen   = pab->mTotalPacketCount * MultiPacket_PutBackeEnd_PaylaodSize ;

    // NEED: to figure out if fencing a little extra is bad...
    Platform::Memory::ExportFence( MsgBaseAddr, MsgMaxLen );


    // should not need the AckFinish stuff to be done before revealing the data to the compute proc.
    if( aFlag0Put1Get1 )
      GlobalConserverOperationsFinished++;

    AckFinish( SourceRank, Slot );

    }
  return( 0 );
  }


// ... not sure what these two are.
long unsigned int
PutMultiPacketBackEnd_ActorFx( void * pkt )
  {
  BegLogLine(1) << "PutMultiPacketBackEnd_ActorFx() Calling general form"<<  EndLogLine;
  MultiPacketBackEnd_ActorFx( pkt, 1 );
  return(0);
  }

// This method was in when porting started... notice name of method called doesn't look right
// Have to figure out what this is
//int
//GetMultiPacketBackEnd_ActorFx( void * pkt )
//  {
//  BegLogLine(1) << "GetMultiPacketBackEnd_ActorFx() Calling general form"<<  EndLogLine;
//  MultiPacetBackEnd_ActorFx( pkt, 0 );
//  }


void
PutMultiPacket_FrontEnd( void * aSourceAddr,
                         int    aDestRank,
                         void * aDestAddr,
                         int    aMsgLen,
                         int  * aDestFinishCount )
  {
  BegLogLine(1)
    << "PutMultiPacket() Begin send :"
    << " SrcAddr "
    << aSourceAddr
    << " DestRank "
    << aDestRank
    << " DestAddr "
    << aDestAddr
    << " MsgLen "
    << aMsgLen
    << EndLogLine;

  int Slot = ReserveOutgoingSlot( aDestRank );
  if( Slot < 0 )
    SHMEM_ABORT("PutMultiPacket_FrontEnd() : FAILED TO GET SEND SLOT");

  GlobalConserverOperationsStarted++;

  PutMultiPacketBackEnd_ActorPacket pap;

  pap.mTotalPacketCount = (aMsgLen + (MultiPacket_PutBackeEnd_PaylaodSize - 1)) / MultiPacket_PutBackeEnd_PaylaodSize ;
  pap.mSourceRank       = BGLPartitionGetRank();
  pap.mSlot             = Slot;

  int DestX, DestY, DestZ;
  BGLPartitionMakeXYZ( aDestRank, &DestX, &DestY, &DestZ );

  _BGL_TorusPktHdr BGLTorus_Hdr;

  BGLTorusMakeHdr( & BGLTorus_Hdr,        // Filled in on return
                   DestX,
                   DestY,
                   DestZ,                 // destination coordinates
                   0,                     // destination Fifo Group
                   (long unsigned int) PutMultiPacketBackEnd_ActorFx,       // Active Packet Function matching signature above
                   0,                     // primary argument to actor
                   0,                     // secondary 10bit argument to actor
                   _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

  //Need a hint bit to send to self - Mark should help with this.
  if( aDestRank == BGLPartitionGetRank() )
    BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );

  int i;
  for( i = 0; i < pap.mTotalPacketCount; i++ )
    {
    int PacketDataOffsetInMessage =  i * MultiPacket_PutBackeEnd_PaylaodSize ;

    char * Data    = ((char *)aSourceAddr) + PacketDataOffsetInMessage;
    int    DataLen = MultiPacket_PutBackeEnd_PaylaodSize ;

    // If it's the last packet, might have to round off a bit.
    if( i == (pap.mTotalPacketCount - 1))
      DataLen = aMsgLen % MultiPacket_PutBackeEnd_PaylaodSize;

    // Lock n load... too bad we can just send from where the data is already... ask mark
    pap.mPacketPayloadLen   = DataLen;
    pap.mPacketDataDestAddr = ((char *)aDestAddr) + PacketDataOffsetInMessage;
    memcpy( pap.mPayload, Data, DataLen);
    pap.mThisPacketIndex    = i ;

    int TrueLen =   sizeof( PutMultiPacketBackEnd_ActorPacket )
                  - ( MultiPacket_PutBackeEnd_PaylaodSize - DataLen );

    BegLogLine(1)
      << "PutMultiPacket() "
      << "Sending To : "
      << "Rank "
      << aDestRank
      << " XYZ "
      << DestX << ":" << DestY << ":" << DestZ
      << " PktInMsg "
      << i
      << " PktDatLen "
      << DataLen
      << " PktTruLen "
      << TrueLen
      << " SrcRank "
      << pap.mSourceRank
      << " MsgSrc->MsgDest Addr "
      << aSourceAddr
      << "->"
      << aDestAddr
      << " PktSrc->PktDest Addr "
      << (void *) Data
      << "->"
      << (void *) pap.mPacketDataDestAddr
      << EndLogLine;

    // It is always dangerous to send from within an actor, so be prepared to fail.
    BGLTorusSendPacketUnaligned( & BGLTorus_Hdr,    // Previously created header to use
                                   Data,             // Source address of data (16byte aligned)
                                   TrueLen );   // Payload bytes = ((chunks * 32) - 16)
    }
  }

///////////////////////////////////////////////////////////////////////
// GetMultiPacket

// Blocking call - doesn't complete until data is in local address space.

// This op enqueues a put op with enough stated to single the
// context that called this function when the put is done.

struct GetMultiPacketBackEnd_ActorPacket
  {
  unsigned short mDataDestRank;  // mSourceRank and mSlot can be determined by single index to IncomingState
  int            mMsgLen;
  void          *mSourceAddr;
  void          *mDestAddr;
  };



typedef struct
  {
  GetMultiPacketBackEnd_ActorPacket mAP;
  int                               mActive;
  int                               mSlot;
  int                               mCurrentDataElement;
  } AsyncRequestEntry ;

AsyncRequestEntry * AsyncReqQueue_GetEntry();


/// LOOKS LIKE IT IS POSSIBLE TO GENERALIZE TO "Put somptin on reomote queue"

int
GetMultiPacketBackEnd_ActorFx( void * pkt)
  {
  GetMultiPacketBackEnd_ActorPacket * gap = (GetMultiPacketBackEnd_ActorPacket *) pkt;
  BegLogLine(1)
    << "GetMultiPacketBackEnd_ActorFx() "
    << " pkt->mDataDestRank "
    << gap->mDataDestRank
    << " pkt->mMsgLen "
    << gap->mMsgLen
    << " pkt->mSourceAddr "
    << gap->mSourceAddr
    << " pkt->mDestAddr "
    << gap->mDestAddr
    << EndLogLine;


  //GOTTA LOCK THAT QUEUE if CP and IOP can get at it.

  AsyncRequestEntry * qe = AsyncReqQueue_GetEntry();


  qe->mCurrentDataElement = 0;
  memcpy( (void *) (&qe->mAP), pkt, sizeof( GetMultiPacketBackEnd_ActorPacket ) );

  BegLogLine(1)
    << "GetMultiPacketBackEnd_ActorFx() "
    << " AsyncReqestEntry @ "
    << (void *) qe
    << " CurrentDataElement = 0 "
    << " pkt->mDataDestRank "
    << gap->mDataDestRank
    << " pkt->mMsgLen "
    << gap->mMsgLen
    << " pkt->mSourceAddr "
    << gap->mSourceAddr
    << " pkt->mDestAddr "
    << gap->mDestAddr
    << EndLogLine;

  return(0);
  }

template<class ItemType>
void ShmemPut( ShmemBuffer<ItemType>     & sb_target,
               ShmemBuffer<ItemType>     & sb_source,
               int                         pe,
               int                         ItemCount = 1 )
  {
  BegLogLine(1)
    << "ShmemPut() "
    << "ShmemPut on ShmemBuffer signature recognized."
    << EndLogLine;

  void * target = sb_target.GetPayloadPtr();
  void * source = sb_source.GetPayloadPtr();

  BegLogLine(1)
    << "ShmemPut():"
    << " START"
    << " Source Node "
    << pe
    << " Target Addr "
    << (void *) target
    << " Source Addr "
    << (void *) source
    << " sizeof( ItemType ) "
    << sizeof( ItemType )
    << " Item Count "
    << ItemCount
    << " Total Bytes "
    << sizeof( ItemType ) * ItemCount
    << EndLogLine;

  PutMultiPacket_FrontEnd( source, pe, target, sizeof( ItemType ) * ItemCount, NULL );

  // PutMultiPacket_FrontEnd( source.GetPayl,  ////////////void * aSourceAddr,
                           ////////////int    aDestRank,
                           ////////////void * aDestAddr,
                           ////////////int    aMsgLen,
                           ////////////int  * aDestFinishCount )

  BegLogLine(1)
    << "ShmemPut():"
    << " FINISH"
    << " Source Node "
    << pe
    << " Target Addr "
    << (void *) target
    << " Source Addr "
    << (void *) source
    << " sizeof( ItemType ) "
    << sizeof( ItemType )
    << " Item Count "
    << ItemCount
    << " Total Bytes "
    << sizeof( ItemType ) * ItemCount
    << EndLogLine;

  }



template<class ItemType>
void ShmemGet( ItemType       &target,
               ItemType       &source,
               int             pe,
               int             ItemCount = 1 )
  {
  BegLogLine(1)
    << "ShmemGet() "
    << "ShmemGet WARNING WRAPPER LESS BUFFER INTERFACE CALLED."
    << EndLogLine;

//if( ((&target) & 0xFFFFFFE0 ) != (&target) )
//  BegLogLine(1) << "ShmemGet() : WARNING target data not cache alined " << EndLogLine();
//
//if( (&source & 0xFFFFFFE0 ) != &source )
//  BegLogLine(1) << "ShmemGet() : WARNING target data not cache alined " << EndLogLine();
//
//if( (sizeof( ItemType ) & 0xFFFFFFE0 ) != sizeof( ItemType ) )
//  BegLogLine(1) << "ShmemGet() : WARNING sizeof( ItemType ) not a round number of cache lines - false sharing possible " << EndLogLine();

  GetMultiPacket_FrontEnd( pe,  &source, &target, sizeof( ItemType ) * ItemCount, NULL );


  }

template<class ItemType>
void ShmemGet( ShmemBuffer<ItemType>     & sb_target,
               ShmemBuffer<ItemType>     & sb_source,
               int                         pe,
               int                         ItemCount = 1 )
  {
  BegLogLine(1)
    << "ShmemGet() "
    << "ShmemGet on ShmemBuffer signature recognized."
    << EndLogLine;

  void * target = sb_target.GetPayloadPtr();
  void * source = sb_source.GetPayloadPtr();

  BegLogLine(1)
    << "ShmemGet():"
    << " START"
    << " Source Node "
    << pe
    << " Target Addr "
    << (void *) target
    << " Source Addr "
    << (void *) source
    << " sizeof( ItemType ) "
    << sizeof( ItemType )
    << " Item Count "
    << ItemCount
    << " Total Bytes "
    << sizeof( ItemType ) * ItemCount
    << EndLogLine;

  GetMultiPacket_FrontEnd( pe,  source, target, sizeof( ItemType ) * ItemCount, NULL );

  BegLogLine(1)
    << "ShmemGet():"
    << " FINISH"
    << " Source Node "
    << pe
    << " Target Addr "
    << (void *) target
    << " Source Addr "
    << (void *) source
    << " sizeof( ItemType ) "
    << sizeof( ItemType )
    << " Item Count "
    << ItemCount
    << " Total Bytes "
    << sizeof( ItemType ) * ItemCount
    << EndLogLine;

  }



void
GetMultiPacket_FrontEnd( int    aSourceRank,
                         void * aSourceAddr,
                         void * aDestAddr,
                         int    aMsgLen,
                         int  * aDestFinishCount )
  {
  BegLogLine(1)
    << "GetMultiPacket() Begin send :"
    << "aSourceRank "
    << aSourceRank
    << " SrcAddr "
    << aSourceAddr
    << " DestAddr "
    << aDestAddr
    << " MsgLen "
    << aMsgLen
    << EndLogLine;

  GlobalConserverOperationsStarted++;

  GetMultiPacketBackEnd_ActorPacket gap;

  gap.mDataDestRank = BGLPartitionGetRank();
  gap.mMsgLen       = aMsgLen;
  gap.mSourceAddr   = aSourceAddr;
  gap.mDestAddr     = aDestAddr;

  int SrcX, SrcY, SrcZ;
  BGLPartitionMakeXYZ( aSourceRank, &SrcX, &SrcY, &SrcZ );

  _BGL_TorusPktHdr BGLTorus_Hdr;

  BGLTorusMakeHdr( & BGLTorus_Hdr,        // Filled in on return
                   SrcX,
                   SrcY,
                   SrcZ,                 // destination coordinates
                   0,                     // destination Fifo Group
                   (long unsigned int) GetMultiPacketBackEnd_ActorFx,       // Active Packet Function matching signature above
                   0,                     // primary argument to actor
                   0,                     // secondary 10bit argument to actor
                   _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

  //Need a hint bit to send to self - Mark should help with this.
  if( aSourceRank == BGLPartitionGetRank() )
    BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );

  BegLogLine(1)
    << "GetMultiPacket() "
    << "SEND PKT To : "
    << "Rank "
    << aSourceRank
    << " XYZ "
    << SrcX << ":" << SrcY << ":" << SrcZ
    << " MsgLen "
    << aMsgLen
    << " MsgSrc->MsgDest Addr "
    << aSourceAddr
    << "->"
    << aDestAddr
    << EndLogLine;

  // It is always dangerous to send from within an actor, so be prepared to fail.
  BGLTorusSendPacketUnaligned(         & BGLTorus_Hdr,    // Previously created header to use
                               (void*) & gap,             // Source address of data (16byte aligned)
                                       sizeof( gap ) );   // Payload bytes = ((chunks * 32) - 16)

  }

#define BGL_SHMEM_MAX_OUTSTANDING_ASYNC_REQUESTS_PER_NODE (2)

#define BGL_SHMEM_MAX_OUSTANDING_ASYNC_REQUESTS_TOTAL   \
         (BGL_SHMEM_MAX_PARTITION_SIZE * BGL_SHMEM_MAX_OUTSTANDING_ASYNC_REQUESTS_PER_NODE )

AsyncRequestEntry AsyncReqQueue[ BGL_SHMEM_MAX_OUSTANDING_ASYNC_REQUESTS_TOTAL ];

unsigned int AsyncReqQueue_TailIndex = 0;
unsigned int AsyncReqQueue_HeadIndex = 0;

int
AsyncReqQueue_IsEmpty()
  {
  if( AsyncReqQueue_HeadIndex == AsyncReqQueue_TailIndex )
    return( 1 );
  return( 0 );
  }

int
AsyncReqQueue_IsFull()
  {
  if( AsyncReqQueue_HeadIndex + 1 == AsyncReqQueue_TailIndex )
    return( 1 );
  return( 0 );
  }

// Returns -1 if full otherwise returns
AsyncRequestEntry *
AsyncReqQueue_GetEntry()
  {
  AsyncRequestEntry *ret;
  if( AsyncReqQueue_IsFull() )
    {
    ret = NULL ;
    }
  else
    {
    ret = & AsyncReqQueue[ AsyncReqQueue_HeadIndex ];
    AsyncReqQueue_HeadIndex =
          ( AsyncReqQueue_HeadIndex + 1 ) % BGL_SHMEM_MAX_OUSTANDING_ASYNC_REQUESTS_TOTAL;
    ret->mActive = 0;
    ret->mSlot = -1; // not required but should catch some problems
    }
  BegLogLine(1)
    << "AsyncReqQueue_GetEntry(): FINISHED "
    << " Ret "
    << (void *)ret
    << " AsyncReqQueue_TailIndex "
    << AsyncReqQueue_TailIndex
    << " AsyncReqQueue_HeadIndex "
    << AsyncReqQueue_HeadIndex
    << EndLogLine;
  return( ret );
  }

// Returns -1 if full otherwise returns
AsyncRequestEntry *
AsyncReqQueue_GetTail()
  {
  AsyncRequestEntry *ret;
  ret = & AsyncReqQueue[ AsyncReqQueue_TailIndex ];
  BegLogLine(1)
    << "AsyncReqQueue_GetTail(): FINISHED "
    << " AsyncReqQueue_TailIndex "
    << AsyncReqQueue_TailIndex
    << " AsyncReqQueue_HeadIndex "
    << AsyncReqQueue_HeadIndex
    << EndLogLine;
  return( ret );
  }

// Returns -1 if full otherwise returns
void
AsyncReqQueue_DeleteTail()
  {
  AsyncReqQueue_TailIndex =
          ( AsyncReqQueue_TailIndex + 1 ) % BGL_SHMEM_MAX_OUSTANDING_ASYNC_REQUESTS_TOTAL;

  BegLogLine(1)
    << "AsyncReqQueue_DeleteTail(): FINISHED "
    << " AsyncReqQueue_TailIndex "
    << AsyncReqQueue_TailIndex
    << " AsyncReqQueue_HeadIndex "
    << AsyncReqQueue_HeadIndex
    << EndLogLine;
  return;
  }


  // This is the state surrounding the send being driven by the IOP
  static int                               ActiveWithARE         = 0;
  static AsyncRequestEntry                 CurrentARE;
  static char *                            CurrentSourceDataAddr = NULL;
  static _BGL_TorusPktHdr                  BGLTorus_Hdr;
  static PutMultiPacketBackEnd_ActorPacket pap;
  static int                               DestX, DestY, DestZ;
  static int                               TrueLen;

void
CheckAsyncQueueReq()
  {

  BegLogLine(1)
    << "CheckAsyncQueueReq(): Entered "
    << "Active "
    << ActiveWithARE
    << " AsyncReqQueue_TailIndex "
    << AsyncReqQueue_TailIndex
    << " AsyncReqQueue_HeadIndex "
    << AsyncReqQueue_HeadIndex
    << EndLogLine;


  if( ! ActiveWithARE )
    {
    BegLogLine(1) << "Not ActiveWithARE" << EndLogLine;
    if( ! AsyncReqQueue_IsEmpty() )
      {
      BegLogLine(1) << "Queue Is Not Empty" << EndLogLine;

      AsyncRequestEntry * qe = AsyncReqQueue_GetTail();
      CurrentARE = *qe;
      AsyncReqQueue_DeleteTail();

      //By design, we MUST be able to get a slot here.
      int Slot = ReserveOutgoingSlot( CurrentARE.mAP.mDataDestRank );
      if( Slot < 0 )
        SHMEM_ABORT("DoAsyncQueueReq() : FAILED TO GET SEND SLOT");

      CurrentSourceDataAddr   = (char *) CurrentARE.mAP.mSourceAddr;

      pap.mSourceRank         = BGLPartitionGetRank();
      pap.mSlot               = Slot;
      pap.mPacketDataDestAddr = (char *) CurrentARE.mAP.mDestAddr;
      pap.mThisPacketIndex    = 0 ;
      pap.mTotalPacketCount   =  (CurrentARE.mAP.mMsgLen + (MultiPacket_PutBackeEnd_PaylaodSize - 1))
                                 / MultiPacket_PutBackeEnd_PaylaodSize ;

      // Just in case the first is also the last packet...
      if( pap.mThisPacketIndex == (pap.mTotalPacketCount - 1) )
        {
        pap.mPacketPayloadLen = CurrentARE.mAP.mMsgLen % MultiPacket_PutBackeEnd_PaylaodSize;
        TrueLen               = sizeof( PutMultiPacketBackEnd_ActorPacket )
                                    - ( MultiPacket_PutBackeEnd_PaylaodSize - pap.mPacketPayloadLen );
        }
      else  // This is in fact a multipacket send - these will not change until last packet.
        {
        pap.mPacketPayloadLen = MultiPacket_PutBackeEnd_PaylaodSize;
        TrueLen               = sizeof( PutMultiPacketBackEnd_ActorPacket );
        }

      // If we used iovec this copy could be avoided.
      memcpy( pap.mPayload, CurrentSourceDataAddr, pap.mPacketPayloadLen );

      BGLPartitionMakeXYZ( CurrentARE.mAP.mDataDestRank, &DestX, &DestY, &DestZ );


      BGLTorusMakeHdr( & BGLTorus_Hdr,        // Filled in on return
                       DestX,
                       DestY,
                       DestZ,                 // destination coordinates
                       0,                     // destination Fifo Group
                       (long unsigned int) PutMultiPacketBackEnd_ActorFx,       // Active Packet Function matching signature above
                       0,                     // primary argument to actor
                       0,                     // secondary 10bit argument to actor
                       _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

      //Need a hint bit to send to self - Mark should help with this.
      if( CurrentARE.mAP.mDataDestRank == BGLPartitionGetRank() )
        BGLTorusUpdateHdrHints( & BGLTorus_Hdr, _BGL_TORUS_PKT_HINT_XP );

      ActiveWithARE = 1;
      }
    }

  // Nothing found to do - return.
  if( ! ActiveWithARE )
    return;

#ifndef BGL_SHMEM_MAX_SEND_BURST_BEFORE_POLL
#define BGL_SHMEM_MAX_SEND_BURST_BEFORE_POLL (6)
#endif

  // By here, we are active AND totally ready to blast a packet out

  BegLogLine(1) << "Starting Burst" << EndLogLine;

  int BurstCount = 0;

  while(1)  // we def. go in the first time ... contidtions will be checked to bail out of this loop
    {
    BegLogLine(1)
      << "CheckAsyncQueueReq() "
      << "Sending To : "
      << "DestRank "
      << CurrentARE.mAP.mDataDestRank
      << " (XYZ: " << DestX << "." << DestY << "." << DestZ << ") "
      << " Slot "
      << pap.mSlot
      << " PktCnt "
      << pap.mThisPacketIndex + 1
      << ":"
      << pap.mTotalPacketCount
      << " PayloadLen "
      << pap.mPacketPayloadLen
      << " PktTrueLen "
      << TrueLen
      << " MsgSrc@->MsgDest@ "
      << CurrentARE.mAP.mSourceAddr
      << "->"
      << CurrentARE.mAP.mDestAddr
      << " PktSrc@->PktDest@ "
      << (void *) CurrentSourceDataAddr
      << "->"
      << (void *) pap.mPacketDataDestAddr
      << EndLogLine;

    // It is always dangerous to send from within an actor, so be prepared to fail.
    int rc = BGLTorusSendPacketUnalignedNonBlocking( & BGLTorus_Hdr,    // Previously created header to use
                                                     & pap,             // Source address of data (16byte aligned)
                                                       TrueLen );   // Payload bytes = ((chunks * 32) - 16)
 ///if( rc != 0 )
    if( rc == 0 )
      {
      BegLogLine(1) << " CheckAsyncQueueReq():" << " rc to Send Unaligned " << rc << EndLogLine;
      break;     // FIFOs full - can't send
      }

    pap.mThisPacketIndex ++ ;
    if( pap.mThisPacketIndex == pap.mTotalPacketCount  )
      {
      BegLogLine(1) << "CheckAsyncQueueReq():" << "Finished Last Packet " << EndLogLine;
      ActiveWithARE = 0;
      break;    // Finished with message
      }

    // If it's the last packet, might have to round off a bit.
    if( pap.mThisPacketIndex == (pap.mTotalPacketCount - 1) )
      {
      pap.mPacketPayloadLen = CurrentARE.mAP.mMsgLen % MultiPacket_PutBackeEnd_PaylaodSize;
      TrueLen               = sizeof( PutMultiPacketBackEnd_ActorPacket )
                                  - ( MultiPacket_PutBackeEnd_PaylaodSize - pap.mPacketPayloadLen );
      BegLogLine(1) << "CheckAsyncQueueReq():" << "Next Packet is last " << pap.mThisPacketIndex << " PktSz " << pap.mPacketPayloadLen << EndLogLine;
      }

    // update state to be ready for next send
    pap.mPacketDataDestAddr += MultiPacket_PutBackeEnd_PaylaodSize ;
    CurrentSourceDataAddr   += MultiPacket_PutBackeEnd_PaylaodSize ;

    // If we used iovec this copy could be avoided.
    memcpy( pap.mPayload, CurrentSourceDataAddr, pap.mPacketPayloadLen );

    // Limit the number of consecutive packets sent to burst count.
    // This check is last so we've already set up for next pkt.
    BurstCount ++ ;
    if(  BurstCount > BGL_SHMEM_MAX_SEND_BURST_BEFORE_POLL )
      {
      BegLogLine(1) << "CheckAsyncQueueReq():" << "BurstCount reached " << EndLogLine;
      break;    // Burst done, return to poll for incoming packets
      }

    }

  }

////static
////int
////AttachDebugger( char *file )
////  {
////  // To use this, simply edit the display name to put debug windows where
////  // you want.  This could be modified to be a routine automatically called
////  // when an assert fails.
////
////  char hostname[80];
////  char db_cmd[1024];
////
////  gethostname( hostname, 80 );
////
////  int TaskNo  = BGLPartitionGetRank();
////  int TaskCnt = -1;
////
////  sprintf( db_cmd,
////          "%s %d %d %d %s %s &",
////          "pkdebug",
////           getpid(),
////           TaskNo,
////           TaskCnt,
////           hostname,
////           file );
////
////  fprintf(stderr, "%s", db_cmd);
////  system( db_cmd );
////  sleep(1);  //Make sure poe doesn't send us oun our way too soon.
////  sleep(1);
////  sleep(1);
////  sleep(1);
////
////  return(0);
////  }



/////// IO Processer is used to dispatch active messages.
/////int _IOP_main( int argc, char *argv[], char **envp )
/////  {
/////  int rc = 0;
/////
/////
/////////  AttachDebugger( argv[0] );
/////
/////  BegLogLine( 1 )
/////    << "_IOP_main() running ... reporting via fxlogging!"
/////    << EndLogLine;
/////
/////  //Don't begin until told to.
/////  ///  Mutex::YieldLock( FFT_InitMutex );
/////  while(1)
/////    {
/////
/////    // Check if the async queue is empty
/////    if( AsyncReqQueue_IsEmpty() )
/////      {
/////      // When the async req queue is empty, we can wait on the fifos.
/////      int count = 1;
/////      rc = BGLTorusWaitCount( &count );  // wait on both fifo groups
/////      }
/////    else
/////      {
/////      // If the async req queue is not empty, we need to alternate between executing
/////      // a queue item and polling.
/////      int count = 1;
/////      BGLTorusPoll( &count );
/////      }
/////
/////    CheckAsyncQueueReq();
/////
/////    }
/////
/////  BegLogLine(1)
/////    << "_IOP_main: All Packets Received, returning"
/////    << " BGLTorusWait rc "
/////    << rc
/////    << EndLogLine;
/////
/////  return(rc);
/////  }
