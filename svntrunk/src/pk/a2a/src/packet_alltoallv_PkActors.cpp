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
 // AllToAllV, but with Pk-fiber-integrated actors
//#include <Pk/Platform.hpp>

#define CACHE_ISOLATION_PADDING_SIZE ( 0 )

class complex
  {
  public:
    double re ;
    double im ;
    complex()       { }
    complex(double r, double i = 0.0)       { re=r; im=i; }
    complex(const complex& c) { re = c.re; im = c.im; }
  } ;

int alltoallv_debug = 0;
/// #define DEBUG_ALL2ALL

#define PACKET_POLL_COUNT 4
#define PACKETS_TO_INJECT 6

#ifndef PKFXLOG_BGL_TORUS
#define PKFXLOG_BGL_TORUS (0)
#endif

// #include <pk/platform.hpp>
#include <BonB/BGL_PartitionSPI.h>
#include <Pk/API.hpp>
#include <Pk/FxLogger.hpp>
//#include <pk/pktrace.hpp>
//#include <Pk/Trace.hpp>
#include <pk/a2a/packet_alltoallv_PkActors.hpp>
#include <BonB/BGLTorusAppSupport.c>


static TraceClient AlltoallvInitStart;
static TraceClient AlltoallvInitFinis;
static TraceClient AlltoallvSimpleExecBarrierStart;
static TraceClient AlltoallvSimpleExecBarrierFinis;
static TraceClient AlltoallvExecStart;
static TraceClient AlltoallvExecFinis;
static TraceClient AlltoallvSendStart;
static TraceClient AlltoallvSendFinis;
static TraceClient AlltoallvExecBarrierStart;
static TraceClient AlltoallvExecBarrierFinis;

static TraceClient AlltoallvPermRanksStart;
static TraceClient AlltoallvPermRanksFinis;

static TraceClient AlltoallvShellStart;

#ifndef PKTRACE_ALLTOALLV_INIT
#define PKTRACE_ALLTOALLV_INIT ( 0 )
#endif

#ifndef PKTRACE_ALLTOALLV_DETAIL
#define PKTRACE_ALLTOALLV_DETAIL ( 0 )
#endif

#ifndef PKFXLOG_ALLTOALLV
#define PKFXLOG_ALLTOALLV (0)
#endif

#ifndef PKFXLOG_ALLTOALLV_INIT
#define PKFXLOG_ALLTOALLV_INIT (0)
#endif

#ifndef PKFXLOG_ALLTOALLV_STRAGGLERS
#define PKFXLOG_ALLTOALLV_STRAGGLERS (0)
#endif

#ifndef PKFXLOG_ALLTOALLV_CORRELATE
#define PKFXLOG_ALLTOALLV_CORRELATE (0)
#endif

volatile static int ReceiveByteCount ;
volatile static int ReceiveCount ; 

volatile static int Expecting = 0 ;

static PacketAlltoAllv_PkActors * CurrentA2A ;

class AllToAllSimplePushAsync
{
	public :
	class GenericPushMetaData
	{
		public: 
		unsigned int mSourceRank ;
		unsigned int mSourceIndex ;
		unsigned int mDataSize ;
	} ;
	
//	template<int>
	static int PushFx(void *arg) ;
	
	
//	template<int kPushType>
	static void Push(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) ;
	                 
      
    	
    	                 
	                 
} ;
//	template<int kPushType>
	void AllToAllSimplePushAsync::Push(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex)
    {

        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        // Send request
		unsigned int BytesInBlock = aDataSize;
        char * Payload =  (char *) PkActorReservePacket( aPE,
                                                     0,
                                                     PushFx,
                                                     sizeof( GenericPushMetaData ) + aDataSize );
        GenericPushMetaData * MetaData = (GenericPushMetaData *)Payload ;
        MetaData->mSourceRank         = aSourceRank;
        MetaData->mSourceIndex        = aIndex ;
        MetaData->mDataSize           = aDataSize ;
        
        void * AppPayload = (void *)(Payload+sizeof(GenericPushMetaData)) ;
        
        memcpy(AppPayload,aDataAddr, aDataSize) ;        

        void ** AppPayloadV = (void **) AppPayload ;
        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllSimplePushAsync::Push aPE=" << aPE
        	<< " mSourceRank=" << MetaData->mSourceRank
        	<< " mSourceIndex=" << MetaData->mSourceIndex
        	<< " aDataSize=" << aDataSize
        	<< " data=0x" << AppPayloadV[0]
        	<< AppPayloadV[1]
        	<< AppPayloadV[2]
        	<< AppPayloadV[3]
        	<< " ..."
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );

        }
//	template<int kPushType>
	int AllToAllSimplePushAsync::PushFx(void *arg)
	{
		char * Payload = (char *) arg ;
		GenericPushMetaData * MetaData = (GenericPushMetaData *)arg ;
		unsigned int SourceRank = MetaData -> mSourceRank ;
		unsigned int SourceIndex = MetaData->mSourceIndex ;
        unsigned int DataSize = MetaData->mDataSize ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << SourceIndex
          << "=" << DataSize
          << "= "
          << EndLogLine ;        

        void * AppPayload = (void *)(Payload+sizeof(GenericPushMetaData)) ;
        void ** AppPayloadV = (void **) AppPayload ;
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " mRecvOffset=" << (void *) CurrentA2A ->mRecvOffset
		  << " SourceIndex=" <<  SourceIndex
		  << " DataSize=" << DataSize
		  << " data=0x" << AppPayloadV[0]
		  << AppPayloadV[1]
		  << AppPayloadV[2]
		  << AppPayloadV[3]
		  << " ..."
		  << EndLogLine ;
		  
		// Supposed to copy the data out here ...
		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
//        assert(SourceIndex < ( 1 << CurrentA2A ->mBlockShift) ) ; 
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "CurrentA2A ->mRecvBuff=0x" << (void *) (CurrentA2A ->mRecvBuff)
          << " CurrentA2A ->mRecvOffset[ SourceRank ]=0x" << (void *)(CurrentA2A ->mRecvOffset[ SourceRank ])
          << " CurrentA2A ->mRecvTypeSize=" << CurrentA2A ->mRecvTypeSize 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
// 
//        double _Complex* Dst = (double _Complex *) Dstc ;
//		BegLogLine(PKFXLOG_ALLTOALLV)
//		  << "PushFx Target Address=" << (void *) Dst
//		  << EndLogLine ;

        BegLogLine(PKFXLOG_ALLTOALLV && ( sizeof(int) == DataSize ) )
          << "Setting *(int *)(0x" << (void *)(Dstc)
          << ") = " << *(int *) AppPayload 
          << EndLogLine ;
          
        AssertLogLine(Dstc)
          << "Target address was NULL"
          << EndLogLine ;
        memcpy((void *)(Dstc),AppPayload, DataSize) ;        
//		  ReceiveByteCount += DataSize ;
        ReceiveCount += 1 ;
		  
        
        return NULL ;
	}

#if defined(PKACTORS_A2A_VIRTUAL_SOURCE)
// Version using 'virtual source' in swh for the source rank
class AllToAllPushAsync
{
	public :
    enum PushFxKind {
    	k16Byte,
    	k32Byte, 
    	k64Byte,
    	k128Byte,
    	k128Multiple,
    	kGeneric,
    };
	class GenericPushMetaData
	{
		public: 
		unsigned int mSourceIndex ;
	} ;
	
	template<int>
	static int PushFx(void *arg) ;
	
	
	template<int kPushType>
	static void Push(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) ;
	                 
      
    	
    	                 
	                 
} ;

    template <> 
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k16Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
//		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+1) ;
//		unsigned int SourceRank = Metadata -> mSourceRank ;
//        unsigned int SourceRank = PkActorGetSourceNodeId() ;
        unsigned long SourceNodeId;
        unsigned long SourceCoreId;
        pkFiberControlBlockT* Context = NULL;
        PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );  
        unsigned int SourceRank = SourceNodeId ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << EndLogLine ;
        BegLogLine(!Expecting)
          << "Unexpected packet, SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
          << EndLogLine ;
		  
		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(16 == CurrentA2A -> mSendBytes ) ; 
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        Dst[0] = temp0 ;	
        
		ReceiveByteCount += 16 ;
        return NULL ;
	}
	
	template <>
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k32Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
//		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+1) ;
//		unsigned int SourceRank = Metadata -> mSourceRank ;
//        unsigned int SourceRank = PkActorGetSourceNodeId() ;
        unsigned long SourceNodeId;
        unsigned long SourceCoreId;
        pkFiberControlBlockT* Context = NULL;
        PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );  
        unsigned int SourceRank = SourceNodeId ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << EndLogLine ;
        BegLogLine(!Expecting)
          << "Unexpected packet, SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
          << EndLogLine ;

		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(32 == CurrentA2A -> mSendBytes ) ; 
		  
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        double _Complex temp1 = Payload[1] ;
        Dst[0] = temp0 ;	
        Dst[1] = temp1 ;	
        
		ReceiveByteCount += 32 ;
        return NULL ;
	}
	
	template <>
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k64Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
//		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+1) ;
//		unsigned int SourceRank = Metadata -> mSourceRank ;
//        unsigned int SourceRank = PkActorGetSourceNodeId() ;
        unsigned long SourceNodeId;
        unsigned long SourceCoreId;
        pkFiberControlBlockT* Context = NULL;
        PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );  
        unsigned int SourceRank = SourceNodeId ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " " << ((double *)Payload)[4]
		  << " " << ((double *)Payload)[5]
		  << " " << ((double *)Payload)[6]
		  << " " << ((double *)Payload)[7]
		  << EndLogLine ;

        BegLogLine(!Expecting)
          << "Unexpected packet, SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " " << ((double *)Payload)[4]
		  << " " << ((double *)Payload)[5]
		  << " " << ((double *)Payload)[6]
		  << " " << ((double *)Payload)[7]          
          << EndLogLine ;
		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(64 == CurrentA2A -> mSendBytes ) ; 
		  
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        double _Complex temp1 = Payload[1] ;
        double _Complex temp2 = Payload[2] ;
        double _Complex temp3 = Payload[3] ;
        Dst[0] = temp0 ;	
        Dst[1] = temp1 ;	
        Dst[2] = temp2 ;	
        Dst[3] = temp3 ;	
        
		ReceiveByteCount += 64 ;
        return NULL ;
	}
	
	template <>
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k128Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
//		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+1) ;
//		unsigned int SourceRank = Metadata -> mSourceRank ;
//        unsigned int SourceRank = PkActorGetSourceNodeId() ;
        unsigned long SourceNodeId;
        unsigned long SourceCoreId;
        pkFiberControlBlockT* Context = NULL;
        PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );  
        unsigned int SourceRank = SourceNodeId ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " " << ((double *)Payload)[4]
		  << " " << ((double *)Payload)[5]
		  << " " << ((double *)Payload)[6]
		  << " " << ((double *)Payload)[7]
		  << " " << ((double *)Payload)[8]
		  << " " << ((double *)Payload)[9]
		  << " " << ((double *)Payload)[10]
		  << " " << ((double *)Payload)[11]
		  << " " << ((double *)Payload)[12]
		  << " " << ((double *)Payload)[13]
		  << " " << ((double *)Payload)[14]
		  << " " << ((double *)Payload)[15]
		  << EndLogLine ;

        BegLogLine(!Expecting)
          << "Unexpected packet, SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " " << ((double *)Payload)[4]
		  << " " << ((double *)Payload)[5]
		  << " " << ((double *)Payload)[6]
		  << " " << ((double *)Payload)[7]          
		  << " " << ((double *)Payload)[8]
		  << " " << ((double *)Payload)[9]
		  << " " << ((double *)Payload)[10]
		  << " " << ((double *)Payload)[11]
		  << " " << ((double *)Payload)[12]
		  << " " << ((double *)Payload)[13]
		  << " " << ((double *)Payload)[14]
		  << " " << ((double *)Payload)[15]
          << EndLogLine ;

		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(128 == CurrentA2A -> mSendBytes ) ; 
		  
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        double _Complex temp1 = Payload[1] ;
        double _Complex temp2 = Payload[2] ;
        double _Complex temp3 = Payload[3] ;
        Dst[0] = temp0 ;	
        Dst[1] = temp1 ;	
        Dst[2] = temp2 ;	
        Dst[3] = temp3 ;	
        double _Complex temp4 = Payload[4] ;
        double _Complex temp5 = Payload[5] ;
        double _Complex temp6 = Payload[6] ;
        double _Complex temp7 = Payload[7] ;
        Dst[4] = temp4 ;	
        Dst[5] = temp5 ;	
        Dst[6] = temp6 ;	
        Dst[7] = temp7 ;	
        
		ReceiveByteCount += 128 ;
        return NULL ;
	}
	
	template<int kPushType>
	int AllToAllPushAsync::PushFx(void *arg)
	{
		unsigned int BytesInBlock = CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift;
		unsigned int DCsInBlock = BytesInBlock/sizeof(double _Complex) ;
		double _Complex * Payload = (double _Complex*) arg ;
		GenericPushMetaData * MetaData = (GenericPushMetaData *)(Payload+DCsInBlock) ;
//		unsigned int SourceRank = Metadata -> mSourceRank ;
//        unsigned int SourceRank = PkActorGetSourceNodeId() ;
        unsigned long SourceNodeId;
        unsigned long SourceCoreId;
        pkFiberControlBlockT* Context = NULL;
        PkActorGetPacketSourceContext( &SourceNodeId, &SourceCoreId, &Context );  
        unsigned int SourceRank = SourceNodeId ;
		unsigned int SourceIndex = MetaData->mSourceIndex ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << SourceIndex
          << "= "
          << EndLogLine ;        

		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " SourceIndex=" <<  SourceIndex
		  << " DataSize=" << (CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift)
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " ..."
		  << EndLogLine ;
		  
		// Supposed to copy the data out here ...
		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(SourceIndex < ( 1 << CurrentA2A ->mBlockShift) ) ; 
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  
                   + SourceIndex * BytesInBlock ;
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx Target Address=" << (void *) Dst
		  << EndLogLine ;

        switch(kPushType)
        {
          case k128Multiple:
          {
          	assert(128 == BytesInBlock) ; 
	        double _Complex temp0 = Payload[0] ;
	        double _Complex temp1 = Payload[1] ;
	        double _Complex temp2 = Payload[2] ;
	        double _Complex temp3 = Payload[3] ;
	        Dst[0] = temp0 ;	
	        Dst[1] = temp1 ;	
	        Dst[2] = temp2 ;	
	        Dst[3] = temp3 ;	
	        double _Complex temp4 = Payload[4] ;
	        double _Complex temp5 = Payload[5] ;
	        double _Complex temp6 = Payload[6] ;
	        double _Complex temp7 = Payload[7] ;
	        Dst[4] = temp4 ;	
	        Dst[5] = temp5 ;	
	        Dst[6] = temp6 ;	
	        Dst[7] = temp7 ;	
		    ReceiveByteCount += 128 ;
          }
          break ;
        default: 
          memcpy((void *)(Dst),Payload, (CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift)) ;        
		  ReceiveByteCount += (CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift) ;
        } 
		  
        
        return NULL ;
	}
	
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k16Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
        // Send request
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacketVirtualSource( aPE,
                                                     0,
                                                     PushFx<k16Byte>,
                                                     16,
                                                     aSourceRank);
        assert(0 == aIndex ) ;
        
        assert(16 == aDataSize) ; 
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        Payload[0]=temp0 ;
       BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " aSourceRank=" << aSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
    }
    	                 
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k32Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
       // Send request
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacketVirtualSource( aPE,
                                                     0,
                                                     PushFx<k32Byte>,
                                                     32 ,
                                                     aSourceRank);
        assert(0 == aIndex ) ;
        
        assert(32 == aDataSize) ; 
        assert(0 == aIndex ) ;
        
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        double _Complex temp1 = src[1] ;
        Payload[0]=temp0 ;
        Payload[1]=temp1 ;
        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " aSourceRank=" << aSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
    }	                 
	                 
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k64Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
       // Send request
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacketVirtualSource( aPE,
                                                     0,
                                                     PushFx<k64Byte>,
                                                     64,
                                                     aSourceRank);
        assert(0 == aIndex ) ;
        
        assert(64 == aDataSize) ; 
        assert(0 == aIndex ) ;
        
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        double _Complex temp1 = src[1] ;
        double _Complex temp2 = src[2] ;
        double _Complex temp3 = src[3] ;
        Payload[0]=temp0 ;
        Payload[1]=temp1 ;
        Payload[2]=temp2 ;
        Payload[3]=temp3 ;
        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " aSourceRank=" << aSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< " " << ((double *)Payload)[4]
        	<< " " << ((double *)Payload)[5]
        	<< " " << ((double *)Payload)[6]
        	<< " " << ((double *)Payload)[7]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
        
    }	                 
	                 
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k128Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        // Send request
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacketVirtualSource( aPE,
                                                     0,
                                                     PushFx<k128Byte>,
                                                     128,
                                                     aSourceRank);
       assert(0 == aIndex ) ;
        
        assert(128 == aDataSize) ; 
        assert(0 == aIndex ) ;
        
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        double _Complex temp1 = src[1] ;
        double _Complex temp2 = src[2] ;
        double _Complex temp3 = src[3] ;
        Payload[0]=temp0 ;
        Payload[1]=temp1 ;
        Payload[2]=temp2 ;
        Payload[3]=temp3 ;
        double _Complex temp4 = src[4] ;
        double _Complex temp5 = src[5] ;
        double _Complex temp6 = src[6] ;
        double _Complex temp7 = src[7] ;
        Payload[4]=temp4 ;
        Payload[5]=temp5 ;
        Payload[6]=temp6 ;
        Payload[7]=temp7 ;
        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " aSourceRank=" << aSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< " " << ((double *)Payload)[4]
        	<< " " << ((double *)Payload)[5]
        	<< " " << ((double *)Payload)[6]
        	<< " " << ((double *)Payload)[7]
        	<< " " << ((double *)Payload)[8]
        	<< " " << ((double *)Payload)[9]
        	<< " " << ((double *)Payload)[10]
        	<< " " << ((double *)Payload)[11]
        	<< " " << ((double *)Payload)[12]
        	<< " " << ((double *)Payload)[13]
        	<< " " << ((double *)Payload)[14]
        	<< " " << ((double *)Payload)[15]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
        
    }	                 
	                 
	template<int kPushType>
	void AllToAllPushAsync::Push(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex)
    {

        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        // Send request
		unsigned int BytesInBlock = CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift;
		unsigned int DCsInBlock = BytesInBlock/sizeof(double _Complex) ;
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacketVirtualSource( aPE,
                                                     0,
                                                     PushFx<kPushType>,
                                                     sizeof( GenericPushMetaData ) + BytesInBlock,
                                                     aSourceRank);
        GenericPushMetaData * MetaData = (GenericPushMetaData *)(Payload+DCsInBlock) ;
        MetaData->mSourceIndex        = aIndex ;
        
        switch(kPushType)
        {
        case k128Multiple:
          {
          	assert(128 == aDataSize) ; 
          	double _Complex temp0 = ((double _Complex*)(aDataAddr))[0] ;
          	double _Complex temp1 = ((double _Complex*)(aDataAddr))[1] ;
          	double _Complex temp2 = ((double _Complex*)(aDataAddr))[2] ;
          	double _Complex temp3 = ((double _Complex*)(aDataAddr))[3] ;
          	Payload[0]=temp0 ;
          	Payload[1] = temp1 ;
          	Payload[2] = temp2 ;
          	Payload[3] = temp3 ;
           	double _Complex temp4 = ((double _Complex*)(aDataAddr))[4] ;
          	double _Complex temp5 = ((double _Complex*)(aDataAddr))[5] ;
          	double _Complex temp6 = ((double _Complex*)(aDataAddr))[6] ;
          	double _Complex temp7 = ((double _Complex*)(aDataAddr))[7] ;
          	Payload[4] = temp4 ;
          	Payload[5] = temp5 ;
          	Payload[6] = temp6 ;
          	Payload[7] = temp7 ;
          	
          }
          break ;
        default: 
          memcpy((void *)Payload,aDataAddr, aDataSize) ;        
        } 

        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " aSourceRank=" << aSourceRank
        	<< " mSourceIndex=" << MetaData->mSourceIndex
        	<< " aDataSize=" << aDataSize
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< " ..."
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );

        }
#else
class AllToAllPushAsync
{
	public :
    enum PushFxKind {
    	k16Byte,
    	k32Byte, 
    	k64Byte,
    	k128Byte,
    	k128Multiple,
    	kGeneric,
    };
	class GenericPushMetaData
	{
		public: 
		unsigned int mSourceRank ;
		unsigned int mSourceIndex ;
	} ;
	class SimplePushMetaData
	{
		public: 
		unsigned int mSourceRank ;
	} ;
	
	template<int>
	static int PushFx(void *arg) ;
	
	
	template<int kPushType>
	static void Push(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) ;
	                 
      
    	
    	                 
	                 
} ;

    template <> 
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k16Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+1) ;
		unsigned int SourceRank = Metadata -> mSourceRank ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << EndLogLine ;
		  
		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(16 == CurrentA2A -> mSendBytes ) ; 
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        Dst[0] = temp0 ;	
        
		ReceiveByteCount += 16 ;
        return NULL ;
	}
	
	template <>
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k32Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+2) ;
		unsigned int SourceRank = Metadata -> mSourceRank ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << EndLogLine ;

		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(32 == CurrentA2A -> mSendBytes ) ; 
		  
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        double _Complex temp1 = Payload[1] ;
        Dst[0] = temp0 ;	
        Dst[1] = temp1 ;	
        
		ReceiveByteCount += 32 ;
        return NULL ;
	}
	
	template <>
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k64Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+4) ;
		unsigned int SourceRank = Metadata -> mSourceRank ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " " << ((double *)Payload)[4]
		  << " " << ((double *)Payload)[5]
		  << " " << ((double *)Payload)[6]
		  << " " << ((double *)Payload)[7]
		  << EndLogLine ;

		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(64 == CurrentA2A -> mSendBytes ) ; 
		  
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        double _Complex temp1 = Payload[1] ;
        double _Complex temp2 = Payload[2] ;
        double _Complex temp3 = Payload[3] ;
        Dst[0] = temp0 ;	
        Dst[1] = temp1 ;	
        Dst[2] = temp2 ;	
        Dst[3] = temp3 ;	
        
		ReceiveByteCount += 64 ;
        return NULL ;
	}
	
	template <>
	int AllToAllPushAsync::PushFx<AllToAllPushAsync::k128Byte>(void *arg)
	{
		double _Complex * Payload = (double _Complex*) arg ;
		SimplePushMetaData * Metadata = (SimplePushMetaData *)(Payload+8) ;
		unsigned int SourceRank = Metadata -> mSourceRank ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << 0
          << "= "
          << EndLogLine ;        
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " " << ((double *)Payload)[4]
		  << " " << ((double *)Payload)[5]
		  << " " << ((double *)Payload)[6]
		  << " " << ((double *)Payload)[7]
		  << " " << ((double *)Payload)[8]
		  << " " << ((double *)Payload)[9]
		  << " " << ((double *)Payload)[10]
		  << " " << ((double *)Payload)[11]
		  << " " << ((double *)Payload)[12]
		  << " " << ((double *)Payload)[13]
		  << " " << ((double *)Payload)[14]
		  << " " << ((double *)Payload)[15]
		  << EndLogLine ;

		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(0 == CurrentA2A -> mBlockShift) ;
        assert(128 == CurrentA2A -> mSendBytes ) ; 
		  
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  ;
                   
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
        double _Complex temp0 = Payload[0] ;
        double _Complex temp1 = Payload[1] ;
        double _Complex temp2 = Payload[2] ;
        double _Complex temp3 = Payload[3] ;
        Dst[0] = temp0 ;	
        Dst[1] = temp1 ;	
        Dst[2] = temp2 ;	
        Dst[3] = temp3 ;	
        double _Complex temp4 = Payload[4] ;
        double _Complex temp5 = Payload[5] ;
        double _Complex temp6 = Payload[6] ;
        double _Complex temp7 = Payload[7] ;
        Dst[4] = temp4 ;	
        Dst[5] = temp5 ;	
        Dst[6] = temp6 ;	
        Dst[7] = temp7 ;	
        
		ReceiveByteCount += 128 ;
        return NULL ;
	}
	
	template<int kPushType>
	int AllToAllPushAsync::PushFx(void *arg)
	{
		unsigned int BytesInBlock = CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift;
		unsigned int DCsInBlock = BytesInBlock/sizeof(double _Complex) ;
		double _Complex * Payload = (double _Complex*) arg ;
		GenericPushMetaData * MetaData = (GenericPushMetaData *)(Payload+DCsInBlock) ;
		unsigned int SourceRank = MetaData->mSourceRank;
		unsigned int SourceIndex = MetaData->mSourceIndex ;
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncActing =" << SourceRank
          << "=" << PkNodeGetId()
          << "=" << SourceIndex
          << "= "
          << EndLogLine ;        

		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx SourceRank=" << SourceRank
		  << " SourceIndex=" <<  SourceIndex
		  << " DataSize=" << (CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift)
		  << " data=" << ((double *)Payload)[0]
		  << " " << ((double *)Payload)[1]
		  << " " << ((double *)Payload)[2]
		  << " " << ((double *)Payload)[3]
		  << " ..."
		  << EndLogLine ;
		  
		// Supposed to copy the data out here ...
		assert(Expecting) ;
        assert(SourceRank < CurrentA2A ->mGroupSize) ;
        assert(SourceIndex < ( 1 << CurrentA2A ->mBlockShift) ) ; 
        char * Dstc = ((char*)(CurrentA2A ->mRecvBuff)) 
                   + ((CurrentA2A ->mRecvOffset[ SourceRank ])*(CurrentA2A ->mRecvTypeSize))  
                   + SourceIndex * BytesInBlock ;
        BegLogLine(PKFXLOG_ALLTOALLV) 
          << "Target address=" << (void *) Dstc
          << EndLogLine ;  
 
        double _Complex* Dst = (double _Complex *) Dstc ;
		BegLogLine(PKFXLOG_ALLTOALLV)
		  << "PushFx Target Address=" << (void *) Dst
		  << EndLogLine ;

        switch(kPushType)
        {
          case k128Multiple:
          {
          	assert(128 == BytesInBlock) ; 
	        double _Complex temp0 = Payload[0] ;
	        double _Complex temp1 = Payload[1] ;
	        double _Complex temp2 = Payload[2] ;
	        double _Complex temp3 = Payload[3] ;
	        Dst[0] = temp0 ;	
	        Dst[1] = temp1 ;	
	        Dst[2] = temp2 ;	
	        Dst[3] = temp3 ;	
	        double _Complex temp4 = Payload[4] ;
	        double _Complex temp5 = Payload[5] ;
	        double _Complex temp6 = Payload[6] ;
	        double _Complex temp7 = Payload[7] ;
	        Dst[4] = temp4 ;	
	        Dst[5] = temp5 ;	
	        Dst[6] = temp6 ;	
	        Dst[7] = temp7 ;	
		    ReceiveByteCount += 128 ;
          }
          break ;
        default: 
//          memcpy((void *)(Dst),Payload, (CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift)) ;        
		  ReceiveByteCount += (CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift) ;
        } 
		  
        
        return NULL ;
	}
	
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k16Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
        // Send request
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacket( aPE,
                                                     0,
                                                     PushFx<k16Byte>,
                                                     sizeof( SimplePushMetaData ) + 16 );
        SimplePushMetaData * MetaData = (SimplePushMetaData *)(Payload+1) ;
        MetaData->mSourceRank         = aSourceRank;
        assert(0 == aIndex ) ;
        
        assert(16 == aDataSize) ; 
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        Payload[0]=temp0 ;
       BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " mSourceRank=" << MetaData->mSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
    }
    	                 
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k32Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
       // Send request
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacket( aPE,
                                                     0,
                                                     PushFx<k32Byte>,
                                                     sizeof( SimplePushMetaData ) + 32 );
        SimplePushMetaData * MetaData = (SimplePushMetaData *)(Payload+2) ;
        MetaData->mSourceRank         = aSourceRank;
        assert(0 == aIndex ) ;
        
        assert(32 == aDataSize) ; 
        assert(0 == aIndex ) ;
        
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        double _Complex temp1 = src[1] ;
        Payload[0]=temp0 ;
        Payload[1]=temp1 ;
        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " mSourceRank=" << MetaData->mSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
    }	                 
	                 
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k64Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
       // Send request
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacket( aPE,
                                                     0,
                                                     PushFx<k64Byte>,
                                                     sizeof( SimplePushMetaData ) + 64 );
        SimplePushMetaData * MetaData = (SimplePushMetaData *)(Payload+4) ;
        MetaData->mSourceRank         = aSourceRank;
        assert(0 == aIndex ) ;
        
        assert(64 == aDataSize) ; 
        assert(0 == aIndex ) ;
        
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        double _Complex temp1 = src[1] ;
        double _Complex temp2 = src[2] ;
        double _Complex temp3 = src[3] ;
        Payload[0]=temp0 ;
        Payload[1]=temp1 ;
        Payload[2]=temp2 ;
        Payload[3]=temp3 ;
        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " mSourceRank=" << MetaData->mSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< " " << ((double *)Payload)[4]
        	<< " " << ((double *)Payload)[5]
        	<< " " << ((double *)Payload)[6]
        	<< " " << ((double *)Payload)[7]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
        
    }	                 
	                 
	template <>
	void AllToAllPushAsync::Push<AllToAllPushAsync::k128Byte>(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex) 
    {
        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        // Send request
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacket( aPE,
                                                     0,
                                                     PushFx<k128Byte>,
                                                     sizeof( SimplePushMetaData ) + 128 );
        SimplePushMetaData * MetaData = (SimplePushMetaData *)(Payload+8) ;
        MetaData->mSourceRank         = aSourceRank;
        assert(0 == aIndex ) ;
        
        assert(128 == aDataSize) ; 
        assert(0 == aIndex ) ;
        
        double _Complex  *src = (double _Complex*) aDataAddr ;
        double _Complex temp0 = src[0] ;
        double _Complex temp1 = src[1] ;
        double _Complex temp2 = src[2] ;
        double _Complex temp3 = src[3] ;
        Payload[0]=temp0 ;
        Payload[1]=temp1 ;
        Payload[2]=temp2 ;
        Payload[3]=temp3 ;
        double _Complex temp4 = src[4] ;
        double _Complex temp5 = src[5] ;
        double _Complex temp6 = src[6] ;
        double _Complex temp7 = src[7] ;
        Payload[4]=temp4 ;
        Payload[5]=temp5 ;
        Payload[6]=temp6 ;
        Payload[7]=temp7 ;
        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " mSourceRank=" << MetaData->mSourceRank
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< " " << ((double *)Payload)[4]
        	<< " " << ((double *)Payload)[5]
        	<< " " << ((double *)Payload)[6]
        	<< " " << ((double *)Payload)[7]
        	<< " " << ((double *)Payload)[8]
        	<< " " << ((double *)Payload)[9]
        	<< " " << ((double *)Payload)[10]
        	<< " " << ((double *)Payload)[11]
        	<< " " << ((double *)Payload)[12]
        	<< " " << ((double *)Payload)[13]
        	<< " " << ((double *)Payload)[14]
        	<< " " << ((double *)Payload)[15]
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );
        
    }	                 
	                 
	template<int kPushType>
	void AllToAllPushAsync::Push(void * aDataAddr,
	                 unsigned int    aDataSize,
	                 unsigned int    aSourceRank, 
	                 unsigned int    aPE,
	                 unsigned int    aIndex)
    {

        BegLogLine(PKFXLOG_ALLTOALLV_CORRELATE) 
          << "AsyncPushing =" << aSourceRank
          << "=" << aPE
          << "=" << aIndex
          << "= "
          << EndLogLine ;        
        // Send request
		unsigned int BytesInBlock = CurrentA2A -> mSendBytes >> CurrentA2A -> mBlockShift;
		unsigned int DCsInBlock = BytesInBlock/sizeof(double _Complex) ;
        double _Complex * Payload = 
              (double _Complex *) PkActorReservePacket( aPE,
                                                     0,
                                                     PushFx<kPushType>,
                                                     sizeof( GenericPushMetaData ) + BytesInBlock );
        GenericPushMetaData * MetaData = (GenericPushMetaData *)(Payload+DCsInBlock) ;
        MetaData->mSourceRank         = aSourceRank;
        MetaData->mSourceIndex        = aIndex ;
        
        switch(kPushType)
        {
        case k128Multiple:
          {
          	assert(128 == aDataSize) ; 
          	double _Complex temp0 = ((double _Complex*)(aDataAddr))[0] ;
          	double _Complex temp1 = ((double _Complex*)(aDataAddr))[1] ;
          	double _Complex temp2 = ((double _Complex*)(aDataAddr))[2] ;
          	double _Complex temp3 = ((double _Complex*)(aDataAddr))[3] ;
          	Payload[0]=temp0 ;
          	Payload[1] = temp1 ;
          	Payload[2] = temp2 ;
          	Payload[3] = temp3 ;
           	double _Complex temp4 = ((double _Complex*)(aDataAddr))[4] ;
          	double _Complex temp5 = ((double _Complex*)(aDataAddr))[5] ;
          	double _Complex temp6 = ((double _Complex*)(aDataAddr))[6] ;
          	double _Complex temp7 = ((double _Complex*)(aDataAddr))[7] ;
          	Payload[4] = temp4 ;
          	Payload[5] = temp5 ;
          	Payload[6] = temp6 ;
          	Payload[7] = temp7 ;
          	
          }
          break ;
        default: 
          memcpy((void *)Payload,aDataAddr, aDataSize) ;        
        } 

        BegLogLine(PKFXLOG_ALLTOALLV)
        	<< "AllToAllPushAsync::Push aPE=" << aPE
        	<< " mSourceRank=" << MetaData->mSourceRank
        	<< " mSourceIndex=" << MetaData->mSourceIndex
        	<< " aDataSize=" << aDataSize
        	<< " data=" << ((double *)Payload)[0]
        	<< " " << ((double *)Payload)[1]
        	<< " " << ((double *)Payload)[2]
        	<< " " << ((double *)Payload)[3]
        	<< " ..."
        	<< EndLogLine ;

        PkActorPacketDispatch( Payload );

        }
#endif	


#if 0
// 'furthest first' ordering
void PacketAlltoAllv_Actors::PermuteActiveSendRanks( int* aRanks, int aSize )
{
	
}
#else
// 'random' packet ordering            
void PacketAlltoAllv_PkActors::PermuteActiveSendRanks( void )
  {
  char state[256];
  char *is;
  
  is = initstate(2+mMyRank, (char *)&state, sizeof(state));
  for( int i=0; i<mActiveSendRanksCount; i++ )
    {
    int index = random() % mActiveSendRanksCount;
    assert( index >= 0 && index < mActiveSendRanksCount );

    int temp = mActiveSendRanks[ index ];
    mActiveSendRanks[ index ] = mActiveSendRanks[ i ];
    mActiveSendRanks[ i ] = temp;
    }
  setstate(is);
  }
#endif


struct PacketAlltoAllv_PkActorsInitActorArgs
   {
   PacketAlltoAllv_PkActors* mThis;
   int * mSendCount;
   int * mSendOffsets;
   int   mSendTypeSize;
   int * mRecvCount;
   int * mRecvOffsets;
   int   mRecvTypeSize;
   GroupContainer_Actors* mGroup;
   int   mDebug;
   };

// This version of all-to-all is specialised for the case where the number of bytes
// flowing from node A to node B is either zero, or some number X independent of the
// source and target nodes.
// 'This' node may participate in the sends, the receives, both, or neither.

void* PacketAlltoAllv_PkActors::InitActor( void* arg )
{
  PacketAlltoAllv_PkActorsInitActorArgs* Args =   (PacketAlltoAllv_PkActorsInitActorArgs*) arg;

  // printf("Got here %d\n", __LINE__);

  PacketAlltoAllv_PkActors* ThisPtr = Args->mThis;
  int* SendCount    = Args->mSendCount;
  int* RecvCount    = Args->mRecvCount;
  int* SendOffset   = Args->mSendOffsets;
  int* RecvOffset   = Args->mRecvOffsets;
  int  SendTypeSize = Args->mSendTypeSize;
  int  RecvTypeSize = Args->mRecvTypeSize;
  GroupContainer_Actors* Group = Args->mGroup;

  BegLogLine(PKFXLOG_ALLTOALLV_INIT)
    << "PacketAlltoAllv_PkActors::InitActor ThisPtr=0x" << (void *) ThisPtr
    << EndLogLine ;

  alltoallv_debug    = Args->mDebug;

 // printf( "Group: %08x line: \n", Group, __LINE__ );

//  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
//      printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
//           sizeof( BGL_AlltoallvPacketMetaData ) );

//  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );

  ThisPtr->mPacketsToInject = PACKETS_TO_INJECT;
  ThisPtr->mGroup     = Group;
  ThisPtr->mMyRank    = ThisPtr->mGroup->mMyRank;  // BGLPartitionGetRank();

  // printf( "ThisPtr->mMyRank: %d\n", ThisPtr->mMyRank );

  ThisPtr->mGroupSize = ThisPtr->mGroup->mSize;
  ThisPtr->mGroupElements = ThisPtr->mGroup->mElements;

  BegLogLine(PKFXLOG_ALLTOALLV_INIT)
    << "ThisPtr->mMyRank=" << ThisPtr->mMyRank
    << " ThisPtr->mGroupSize=" << ThisPtr->mGroupSize
    << EndLogLine ;
//  if( !( ThisPtr->mMyRank >= 0 && ThisPtr->mMyRank < ThisPtr->mGroupSize ) )
//    {
//    printf("%d %d\n", ThisPtr->mMyRank, ThisPtr->mGroupSize );
//    }

  assert( ThisPtr->mMyRank >= 0 && ThisPtr->mMyRank < ThisPtr->mGroupSize );

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

//#ifdef PK_PHASE5_SPLIT_COMMS
//  ThisPtr->mMyCore    = 1;
//  ThisPtr->mNumberOfFifosInUse = 3;
//#else
//  ThisPtr->mNumberOfFifosInUse = 6;
//  ThisPtr->mMyCore    = 0;
//#endif

  ThisPtr->mMyCoord   = ThisPtr->mGroupElements[ ThisPtr->mMyRank ].mCoord;

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );
  // mSendBuff      = aSendBuf;
  ThisPtr->mSendCount     = SendCount;
  ThisPtr->mSendOffsetIn  = SendOffset;
  ThisPtr->mSendTypeSize  = SendTypeSize;

  // mRecvBuff     = aRecvBuf;
  ThisPtr->mRecvCount    = RecvCount;
  ThisPtr->mRecvOffset   = RecvOffset;
  ThisPtr->mRecvTypeSize = RecvTypeSize;

  ThisPtr->mBytesRecvFromNode = (int *) PkHeapAllocate( ThisPtr->mGroupSize * sizeof(int) + CACHE_ISOLATION_PADDING_SIZE);
  assert( ThisPtr->mBytesRecvFromNode );

  ThisPtr->mSendOffsets = (int *) PkHeapAllocate( ThisPtr->mGroupSize * sizeof(int) + CACHE_ISOLATION_PADDING_SIZE);
  assert( ThisPtr->mSendOffsets );

  ThisPtr->mActiveSendRanks = (int *) PkHeapAllocate( ThisPtr->mGroupSize * sizeof(int) + CACHE_ISOLATION_PADDING_SIZE);
  assert( ThisPtr->mActiveSendRanks );
  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

// We never send to self.
  ThisPtr->mRecvNonActiveCount = 1;

  ThisPtr->mActiveSendRanksCount = 0;

  int MaxNumberHeaders = 0;


// Work out how much data is being sent
  int SendSize = 0 ;
  int GroupSize =  ThisPtr->mGroupSize ;
  int ReceiveItems = 0 ;
  {
	  for ( int a=0;a<GroupSize;a+=1)
	  {
	  	 BegLogLine(PKFXLOG_ALLTOALLV_INIT)
	  	   << "Rank " << a 
	  	   << " Configuring to send " << ThisPtr->mSendCount[a] 
	  	   << " items and receive " << ThisPtr->mRecvCount[a]
	  	   << EndLogLine ;
	  	SendSize |= ThisPtr->mSendCount[a] | ThisPtr->mRecvCount[a] ;
	  	ReceiveItems += ThisPtr->mRecvCount[a] ;
	  }
  }
// Check that we have a consistent answer
  {
	  for ( int a=0;a<GroupSize;a+=1)
	  {
	  	assert(0 == ThisPtr->mSendCount[a] || SendSize == ThisPtr->mSendCount[a] ) ;
	  	assert(0 == ThisPtr->mRecvCount[a] || SendSize == ThisPtr->mRecvCount[a] ) ;
	  }
  }
  

  int SendBytes = SendSize * ThisPtr->mSendTypeSize ;
  
// Figure how many messages we will need to send to carry the total payload  
  int BlockShift = 0 ; 
  for (;(BlockShift<32) && ( (SendBytes >> BlockShift) > _BGL_TORUS_PKT_MAX_PAYLOAD- sizeof(AllToAllPushAsync::GenericPushMetaData)); BlockShift += 1)
  {
  }
  // We only handle messages that can be chopped into equal-sized packets
  // This includes all power-of-2 sizes
  assert(( (SendBytes >> BlockShift) << BlockShift) == SendBytes ) ;
  int NumberOfHeaders = 1 << BlockShift ;
  ThisPtr-> mBlockShift = BlockShift ;
  ThisPtr -> mSendBytes = SendBytes ;
// Note how many bytes are expected in, excluding any 'self-sends'  
  ThisPtr -> mReceiveBytes = (ReceiveItems-ThisPtr->mRecvCount[ThisPtr->mMyRank]) * ThisPtr->mSendTypeSize ;
  BegLogLine(PKFXLOG_ALLTOALLV_INIT)
    << "Sending " << SendBytes
    << " setting BlockShift=" << BlockShift
    << " setting mReceiveBytes=" << ThisPtr -> mReceiveBytes
    << EndLogLine ;

  MaxNumberHeaders = NumberOfHeaders ;
  for( int i=0; i < ThisPtr->mGroupSize; i++ )
    {

    BegLogLine(PKFXLOG_ALLTOALLV_INIT)
      << "Considering i=" << i
      << " mSendCount[ i ]=" << ThisPtr->mSendCount[ i ]
      << " mMyRank=" << ThisPtr->mMyRank
      << EndLogLine ; 
    if( ThisPtr->mSendCount[ i ] != 0 && i != ThisPtr->mMyRank )
      {

      ThisPtr->mActiveSendRanks[ ThisPtr->mActiveSendRanksCount ] = i;
      BegLogLine(PKFXLOG_ALLTOALLV_INIT)
        << "mActiveSendRanks[" << ThisPtr->mActiveSendRanksCount
        << "]=" << i
        << EndLogLine ;
      ThisPtr->mActiveSendRanksCount++;
      }

    if( ThisPtr->mRecvCount[ i ] == 0 && i != ThisPtr->mMyRank )
      {
      // Self sends have been accounted
      ThisPtr->mRecvNonActiveCount++;
      }
    }

  // printf("Core(%d): Got Here: %d\n", hard_processor_id(), __LINE__ );

  ThisPtr -> PermuteActiveSendRanks(  );
  

  int xDim, yDim, zDim;
  BGLPartitionGetDimensions( &xDim, &yDim, &zDim );

  ThisPtr->mPeriod.Set( xDim , yDim, zDim );

      ThisPtr->mHeaderCount = MaxNumberHeaders * ThisPtr->mActiveSendRanksCount;

      ThisPtr->mHeaders = (ThisPtr->mHeaderCount > 0 ) 
                        ? (AllToAllvHeader *)PkHeapAllocate(sizeof( AllToAllvHeader ) * ThisPtr->mHeaderCount ) 
                        : NULL ;
      
      BegLogLine(PKFXLOG_ALLTOALLV_INIT )
        << "Allocate mHeaders count=" << ThisPtr->mHeaderCount
        << EndLogLine ; 
  int FifoPicker = 0;

	for( int i=0; i < ThisPtr->mActiveSendRanksCount; i++ )
	{
	  for ( int HeaderIndex = 0 ; HeaderIndex < (1<<BlockShift) ; HeaderIndex += 1)
      {
    
    	int TargetRank = ThisPtr->mActiveSendRanks[i] ;
    	TorusXYZ DestCoord = ThisPtr->mGroupElements[ TargetRank ].mCoord;
    	int TargetPE = PkTopo3DMakeRank(DestCoord.mX, DestCoord.mY, DestCoord.mZ) ;
	
	      AllToAllvHeader* Hdr = &ThisPtr->mHeaders[ (i<<BlockShift)+HeaderIndex ] ;
	
	      Hdr -> mPE = TargetPE ;
	      Hdr -> mTargetRank = TargetRank ;
	      Hdr -> mSourceIndex = HeaderIndex ;
	      BegLogLine(PKFXLOG_ALLTOALLV_INIT)
	        << "Header " << (i<<BlockShift)+HeaderIndex
	        << " mPE=" << Hdr -> mPE
	        << " mTargetRank=" << Hdr -> mTargetRank
	        << " mSourceIndex=" << Hdr -> mSourceIndex 
	        << EndLogLine ;
	        
	    }
    
    }

#ifdef DEBUG_ALL2ALL
  if( Verbose )
      printf("Core(%d): Got here: %d\n",rts_get_processor_id(), __LINE__ );
#endif
  return NULL;
  }

void PacketAlltoAllv_PkActors::InitSingleCore( int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                      int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                      GroupContainer_Actors* aGroup )
  {
  PacketAlltoAllv_PkActorsInitActorArgs Core0Args;

  Core0Args.mThis        = this;
  Core0Args.mSendCount   = aSendCount;
  Core0Args.mRecvCount   = aRecvCount;
  Core0Args.mSendOffsets = aSendOffset;
  Core0Args.mRecvOffsets = aRecvOffset;
  Core0Args.mSendTypeSize = aSendTypeSize;
  Core0Args.mRecvTypeSize = aRecvTypeSize;
  Core0Args.mGroup       = aGroup;
  Core0Args.mDebug       = alltoallv_debug;

  InitActor( &Core0Args );
  }

//extern unsigned SimTick;



struct FinalizeActorArg
  {
  PacketAlltoAllv_PkActors* mThis;
  };

void* PacketAlltoAllv_PkActors::FinalizeActor( void* arg )
  {
  FinalizeActorArg* Args =   (FinalizeActorArg *) arg;

  PacketAlltoAllv_PkActors* ThisPtr = Args->mThis;

  if( ThisPtr->mBytesRecvFromNode != NULL )
    {
    PkHeapFree( ThisPtr->mBytesRecvFromNode );
    ThisPtr->mBytesRecvFromNode = NULL;
    }

  if( ThisPtr->mSendOffsets != NULL )
    {
    PkHeapFree( ThisPtr->mSendOffsets );
    ThisPtr->mSendOffsets = NULL;
    }

  if( ThisPtr->mActiveSendRanks != NULL )
    {
    PkHeapFree( ThisPtr->mActiveSendRanks );
    ThisPtr->mActiveSendRanks = NULL;
    }

  if( ThisPtr->mHeaders != NULL )
    {
     PkHeapFree(ThisPtr->mHeaders) ;
     ThisPtr->mHeaders = NULL;
    }
     
//  for( int i=0; i<NUMBER_OF_LINKS; i++ )
//    {
//    if( ThisPtr->mHeadersForLink[ i ] != NULL )
//      {
//      free( ThisPtr->mHeadersForLink[ i ] );
//      ThisPtr->mHeadersForLink[ i ] = NULL;
//      }
//    }

  return NULL;
  }

#if 0
void PacketAlltoAllv_Actors::Finalize()
  {
  FinalizeActorArg Core0Args;
  Core0Args.mThis        = this;

  FinalizeActor( &Core0Args );
  }
#endif

void PacketAlltoAllv_PkActors::FinalizeSingleCore()
  {
  FinalizeActorArg         Core0Args;
  Core0Args.mThis        = this;
  FinalizeActor( &Core0Args );
  }

void PacketAlltoAllv_PkActors::FinalizeSingleCoreSimple()
  {
  if( mBytesRecvFromNode != NULL )
    {
    free( mBytesRecvFromNode );
    mBytesRecvFromNode = NULL;
    }

  if( mSendOffsets != NULL )
    {
    free( mSendOffsets );
    mSendOffsets = NULL;
    }

  if( mActiveSendRanks != NULL )
    {
    free( mActiveSendRanks );
    mActiveSendRanks = NULL;
    }

  if( mActiveSendRanksTransient != NULL )
    {
        //free( mActiveSendRanksTransient );
    mActiveSendRanksTransient = NULL;
    }
  }


template< int NON_ALIGNED1 >
void PacketAlltoAllv_PkActors::ExecuteSingleCore1( void* aSendBuff, void* aRecvBuff )
  {
//  #ifdef PK_PHASE5_SPLIT_COMMS
//    unsigned long dummy1;
//    unsigned long dummy2;
//    GlobalTreeReduce( &dummy1, &dummy2, sizeof( unsigned long ), _BGL_TREE_OPCODE_MAX );
//  #else
  BegLogLine(PKFXLOG_ALLTOALLV_STRAGGLERS) 
    << "PacketAlltoAllv_PkActors::ExecuteSingleCore1 this=0x" << (void *) this
    << EndLogLine ;

  AlltoallvExecStart.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvExecStart",
      PkNodeGetId(),
      AlltoallvExecStart );

//    BGLPartitionBarrierCore();
//  #endif

//   printf( "ExecuteSingleCore1: Got Here: %d\n", __LINE__ );
//   fflush( stdout );

  mSendBuff = aSendBuff;
  mRecvBuff = aRecvBuff;

  // alltoallv_debug = 1;

  CurrentA2A = this ;
  BegLogLine(PKFXLOG_ALLTOALLV)
    << "PacketAlltoAllv_Actors::ExecuteSingleCore1 mSendCount=" << mSendCount
    << " mBlockShift=" << mBlockShift 
    << EndLogLine ;

  // memcpy data local to this node.
  if( mSendCount[ mMyRank ] )
    {
    char * Src = (char*)mSendBuff + (mSendOffsetIn[ mMyRank ] * mSendTypeSize);
    char * Tgt = (char*)mRecvBuff + (mRecvOffset[ mMyRank ] * mRecvTypeSize);
    int Size   = mSendCount[ mMyRank ] * mSendTypeSize;
    memcpy( Tgt, Src, Size );
    }
  mRecvCount[ mMyRank ] = mSendCount[ mMyRank ];


  mTotalSentPackets = 0;
  mTotalRecvPackets = 0;
  mRecvCountDone = mRecvNonActiveCount;
  
  unsigned int BytesPerPacket = mSendBytes >> mBlockShift ; 
  
  
  for( int i=0; i<mGroupSize; i++ )
    mBytesRecvFromNode[ i ] = 0;

  // Send all the packets (this incorporates draining incoming ones)
  int FinalReceiveByteCount = mReceiveBytes ;
  ReceiveByteCount=0 ; // so we will be able to work out when everything is 'in' 
  Expecting = 1 ; // We are now able to handle incoming packets
// 'Barrier' here so we don't push any packets in before a node is expecting.
  AlltoallvExecBarrierStart.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvExecBarrierStart",
      PkNodeGetId(),
      AlltoallvExecBarrierStart );

  PkCo_Barrier() ;
  
  AlltoallvExecBarrierFinis.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvExecBarrierFinis",
      PkNodeGetId(),
      AlltoallvExecBarrierFinis );

  AlltoallvSendStart.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvSendStart",
      PkNodeGetId(),
      AlltoallvSendStart );

  for ( int PacketIndex = 0 ; PacketIndex < ( mActiveSendRanksCount << mBlockShift ) ; PacketIndex += 1 )
  {
  	
	  BegLogLine(PKFXLOG_ALLTOALLV) 
		  << "Pushing, PacketIndex=" << PacketIndex
		  << " Quantity=" << ( mActiveSendRanksCount << mBlockShift )
		  << " ReceiveByteCount=" << ReceiveByteCount 
		  << " FinalReceiveByteCount=" << FinalReceiveByteCount
		  << " mHeaders[PacketIndex].mTargetRank=" << mHeaders[PacketIndex].mTargetRank
		  << " mSendOffsetIn[ .. ]=" << mSendOffsetIn[ mHeaders[PacketIndex].mTargetRank ]
		  << " mHeaders[PacketIndex].mSourceIndex=" << mHeaders[PacketIndex].mSourceIndex
		  << EndLogLine ; 
	  char* BlockToSend = ((char *)mSendBuff)  + (mSendOffsetIn[ mHeaders[PacketIndex].mTargetRank ] * mSendTypeSize) + mHeaders[PacketIndex].mSourceIndex*BytesPerPacket;
	  switch (BytesPerPacket) {
	  	case 16:
  	  AllToAllPushAsync::Push<AllToAllPushAsync::k16Byte>( (void *) BlockToSend  
  	                         , BytesPerPacket
  	                         , mMyRank
  	                         , mHeaders[PacketIndex].mPE
  	                         , mHeaders[PacketIndex].mSourceIndex
  	  	                 ) ;
	  	break ;
	  	case 32:
  	  AllToAllPushAsync::Push<AllToAllPushAsync::k32Byte>( (void *) BlockToSend  
  	                         , BytesPerPacket
  	                         , mMyRank
  	                         , mHeaders[PacketIndex].mPE
  	                         , mHeaders[PacketIndex].mSourceIndex
  	  	                 ) ;
	  	break ;
	  	case 64:
  	  AllToAllPushAsync::Push<AllToAllPushAsync::k64Byte>( (void *) BlockToSend  
  	                         , BytesPerPacket
  	                         , mMyRank
  	                         , mHeaders[PacketIndex].mPE
  	                         , mHeaders[PacketIndex].mSourceIndex
  	  	                 ) ;
	  	break ;
	  	case 128:
	  	if ( 0 == mBlockShift )
	  	{
	  	  AllToAllPushAsync::Push<AllToAllPushAsync::k128Byte>( (void *) BlockToSend  
	  	                         , BytesPerPacket
	  	                         , mMyRank
	  	                         , mHeaders[PacketIndex].mPE
	  	                         , mHeaders[PacketIndex].mSourceIndex
	  	  	                 ) ;
	  	} else {
	  	  AllToAllPushAsync::Push<AllToAllPushAsync::kGeneric>( (void *) BlockToSend  
	  	                         , BytesPerPacket
	  	                         , mMyRank
	  	                         , mHeaders[PacketIndex].mPE
	  	                         , mHeaders[PacketIndex].mSourceIndex
	  	  	                 ) ;
	  	}
	  	break ;
	  	default :
	  	  AllToAllPushAsync::Push<AllToAllPushAsync::kGeneric>( (void *) BlockToSend  
	  	                         , BytesPerPacket
	  	                         , mMyRank
	  	                         , mHeaders[PacketIndex].mPE
	  	                         , mHeaders[PacketIndex].mSourceIndex
	  	  	                 ) ;
	  } 
  	  BegLogLine(PKFXLOG_ALLTOALLV) 
  		  << "Sending, ReceiveByteCount=" << ReceiveByteCount
  		  << " FinalReceiveByteCount=" << FinalReceiveByteCount
  		  << EndLogLine ; 
      assert(ReceiveByteCount <= FinalReceiveByteCount) ;
//      PkKickPipesGently() ;
  	  PkFiberYield() ;
  	  	                 
  }
  AlltoallvSendFinis.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvSendFinis",
      PkNodeGetId(),
      AlltoallvSendFinis );

  
//// 'Barrier' here spins until all nodes have everything in their hardware transmit queues
//  PkCo_Barrier() ;
  
  // Spin for stragglers to come in
  int LatestReceiveByteCount = ReceiveByteCount ;
  int NextReceiveByteCount = LatestReceiveByteCount ;
  BegLogLine(PKFXLOG_ALLTOALLV_STRAGGLERS) 
	  << "Starting to spin, NextReceiveByteCount=" << NextReceiveByteCount
	  << " FinalReceiveByteCount=" << FinalReceiveByteCount
	  << EndLogLine ; 

  unsigned int spincount = 0 ; 
	
  while (NextReceiveByteCount != FinalReceiveByteCount)
  {
  	NextReceiveByteCount = ReceiveByteCount ;
  	if ( NextReceiveByteCount != LatestReceiveByteCount )
  	{
  		BegLogLine(PKFXLOG_ALLTOALLV_STRAGGLERS) 
  		  << "Spinning, NextReceiveByteCount=" << NextReceiveByteCount
  		  << " FinalReceiveByteCount=" << FinalReceiveByteCount
  		  << EndLogLine ; 
  		LatestReceiveByteCount = NextReceiveByteCount ; 
  	}
    assert(NextReceiveByteCount <= FinalReceiveByteCount) ;
  	PkFiberYield() ;
  	spincount += 1 ;
  	
  	
  	assert (spincount < 1000000000 ) ; // If this one fails, it is likely we have lost a packet
  }  
  
  Expecting = 0 ;
//// 'Barrier' here spins until all nodes have received everything
//  PkCo_Barrier() ;
// Spin here until the software send queue is cleared into the hardware
  PkClearSendQueue() ;

  AlltoallvExecFinis.HitOE( PKTRACE_ALLTOALLV_DETAIL,
      "AlltoallvExecFinis",
      PkNodeGetId(),
      AlltoallvExecFinis );


  BegLogLine(PKFXLOG_ALLTOALLV_STRAGGLERS) 
    << "ExecuteSingleCore1 complete"
    << EndLogLine ;

  }

void PacketAlltoAllv_PkActors::ExecuteSingleCore( void* aSendBuf, int* aSendCount, int* aSendOffset, int aSendTypeSize,
                                         void* aRecvBuf, int* aRecvCount, int* aRecvOffset, int aRecvTypeSize,
                                         GroupContainer_Actors* aGroup, int aligned, int debug )
  {
  alltoallv_debug = debug;
  InitSingleCore( aSendCount, aSendOffset, aSendTypeSize,
                  aRecvCount, aRecvOffset, aRecvTypeSize, aGroup );

  if( aligned && ( 0 == (mSendTypeSize & 8) ))
    {
    ExecuteSingleCore1<0>( aSendBuf, aRecvBuf );
    }
  else
    {
    ExecuteSingleCore1<1>( aSendBuf, aRecvBuf );
    }

  FinalizeSingleCore();
  alltoallv_debug = 0;
  }
void PacketAlltoAllv_PkActors::InitSingleCoreSimple( int aSendCount[], int* aSendOffset, int aSendTypeSize,
                                            int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                                            GroupContainer_Actors* aGroup,
                                            int* aSendNodeList, int aSendNodeListCount,
                                            int* aRecvNodeList, int aRecvNodeListCount )
  {
  AlltoallvInitStart.HitOE( PKTRACE_ALLTOALLV_INIT,
                           "AlltoallvInitStart",
                            PkNodeGetId(),
                           AlltoallvInitStart );

  // alltoallv_debug    = 0;

  BegLogLine(PKFXLOG_ALLTOALLV)
    << "sizeof( BGL_AlltoallvPacketMetaData )=" << sizeof( BGL_AlltoallvPacketMetaData )
    << EndLogLine ;
//  if( !( sizeof( BGL_AlltoallvPacketMetaData ) == 8 ) )
//    printf("sizeof( BGL_AlltoallvPacketMetaData ): %d\n",
//           sizeof( BGL_AlltoallvPacketMetaData ) );

  assert( sizeof( BGL_AlltoallvPacketMetaData ) == 8 );

  mPacketsToInject = PACKETS_TO_INJECT;  
	BegLogLine( PKFXLOG_ALLTOALLV_INIT  )
//	    << SimTick
	    << " aSendNodeListCount: " << aSendNodeListCount
	    << " aRecvNodeListCount: " << aRecvNodeListCount
	    << " "
	    << EndLogLine;
  {
	  for ( int a=0;a<aSendNodeListCount;a+=1)
	  {
	  	BegLogLine(PKFXLOG_ALLTOALLV_INIT)
	  	  << "aSendNodeList[" << a
	  	  << "]=" << aSendNodeList[a]
	  	  << " aSendCount=" << aSendCount[a]
	  	  << " aSendOffset=0x" << (void *)(aSendOffset[a])
	  	  << EndLogLine ;
	  }
  }
  {
	  for ( int a=0;a<aRecvNodeListCount;a+=1)
	  {
	  	BegLogLine(PKFXLOG_ALLTOALLV_INIT)
	  	  << "aRecvNodeList[" << a
	  	  << "]=" << aRecvNodeList[a]
	  	  << " aRecvCount=" << aRecvCount[a]
	  	  << " aRecvOffset[a]=" << (void *)(aRecvOffset[a])
	  	  << EndLogLine ;
	  }
  }
  
  mGroup     = aGroup;
  mMyRank    = mGroup->mMyRank;

  mGroupSize = mGroup->mSize;
  mGroupElements = mGroup->mElements;

  BegLogLine(PKFXLOG_ALLTOALLV)
    << "mMyRank=" << mMyRank
    << " mGroupSize=" << mGroupSize
    << EndLogLine ;
//  if( !( mMyRank >= 0 && mMyRank < mGroupSize ) )
//    {
//    printf("%d %d\n", mMyRank, mGroupSize );
//    }

  assert( mMyRank >= 0 && mMyRank < mGroupSize );

  mMyCoord   = mGroupElements[ mMyRank ].mCoord;

  mSendCount     = aSendCount;
  mSendOffsetIn  = aSendOffset;
  mSendTypeSize  = aSendTypeSize;

//   if( alltoallv_debug && ( mMyRank == 14 ) )
//     {
//     printf( "SCOUNT: %d\n", mSendCount[ 0 ] );
//     }

  mRecvCount    = aRecvCount;
  mRecvOffset   = aRecvOffset;
  mRecvTypeSize = aRecvTypeSize;

  if( mFirstEntry )
    {
    mBytesRecvFromNode = (int *) PkHeapAllocate( mGroupSize * sizeof(int));
    assert( mBytesRecvFromNode );

    mSendOffsets = (int *) PkHeapAllocate( mGroupSize * sizeof(int));
    assert( mSendOffsets );

    mActiveSendRanks = (int *) PkHeapAllocate( mGroupSize * sizeof(int));
    assert( mActiveSendRanks );

    mActiveSendRanksTransient = (int *) PkHeapAllocate( sizeof(int) * mGroupSize);
    assert( mActiveSendRanksTransient );

    BegLogLine(PKFXLOG_ALLTOALLV) 
      << "Done allocating simple alltoallv members"
      << EndLogLine ;
//    printf( "Done allocating simple alltoallv members\n" );
//    fflush( stdout );

    mFirstEntry = 0;
    }

// We never send to self.
  mRecvNonActiveCount = 1;

  mActiveSendRanksCount = 0;

  int MaxNumberHeaders = 0;

  AlltoallvShellStart.HitOE( PKTRACE_ALLTOALLV_INIT,
                             "AlltoallvShellStart",
                             PkNodeGetId(),
                             AlltoallvShellStart );

//mActiveSendRanks[ mActiveSendRanksCount ] = i;
//mActiveSendRanksCount++;

// mActiveSendRanksCount = aSendNodeListCount;
  mReceiveBytes = 0 ;
if(!( (aSendNodeList == NULL ) && ( aSendNodeListCount == 0  ) && ( aRecvNodeListCount == 0 ) ))
///if( 0 )
    {
    for( int i=0; i < aSendNodeListCount; i++ )
      {
      if( aSendNodeList[ i ] != mMyRank )
        {
        mActiveSendRanks[ mActiveSendRanksCount ] = aSendNodeList[ i ];
        mActiveSendRanksCount++;
        }
      }

    // Loop to see if MyRank is in the list
    int IsMyRankOnList = 0;
    for( int i=0; i < aRecvNodeListCount; i++ )
      {
      int nodeId = aRecvNodeList[ i ];
      mBytesRecvFromNode[ nodeId ] = 0;

      if( nodeId == mMyRank )
        {
        IsMyRankOnList=1;
        // break;
        }
      }

    //
    mRecvNonActiveCount = 0;
    mRecvNonActiveCount += ( (IsMyRankOnList) ?
                             (mGroupSize - (aRecvNodeListCount - 1)): (mGroupSize - aRecvNodeListCount) );

#if 0
    int TempmRecvNonActiveCount = 1;
    int  TempmActiveSendRanksCount = 0;
    int* TempList = (int *) PkHeapAllocate( sizeof(int) * mGroupSize );

    for( int i=0; i < mGroupSize; i++ )
      {
      mBytesRecvFromNode[ i ] = 0;

      if( mSendCount[ i ] != 0 && i != mMyRank )
        {
        TempList[ TempmActiveSendRanksCount ] = i;
        TempmActiveSendRanksCount++;
        }

      if( mRecvCount[ i ] == 0 && i != mMyRank )
        {
        // Self sends have been accounted
        TempmRecvNonActiveCount++;
        }
      }

    if( TempmRecvNonActiveCount != mRecvNonActiveCount )
      {
      BegLogLine( 1 )
        << "TempmRecvNonActiveCount: " << TempmRecvNonActiveCount
        << " mRecvNonActiveCount: " << mRecvNonActiveCount
        << EndLogLine;
      }

    if( TempmActiveSendRanksCount != mActiveSendRanksCount )
      {
      BegLogLine( 1 )
        << "TempmActiveSendRanksCount: " << TempmActiveSendRanksCount
        << " mActiveSendRanksCount: " << mActiveSendRanksCount
        << EndLogLine;
      }

    for( int i=0; i < mActiveSendRanksCount; i++ )
      {
      if( TempList[i] != mActiveSendRanks[ i ] )
        {
        BegLogLine( 1 )
          << "TempList[" << i << "]: " << TempList[i]
          << "mActiveSendRanks[" << i << "]: " << mActiveSendRanks[i]
          << EndLogLine;
        break;
        }
      }
#endif
    }
  else
    {
    for( int i=0; i < mGroupSize; i++ )
      {
      mBytesRecvFromNode[ i ] = 0;

      if( mSendCount[ i ] != 0 && i != mMyRank )
        {
        mActiveSendRanks[ mActiveSendRanksCount ] = i;
        mActiveSendRanksCount++;
        }

      if ( i != mMyRank )
      {
      	 mReceiveBytes += mRecvCount[i] * mSendTypeSize ;
      }
      if( mRecvCount[ i ] == 0 && i != mMyRank )
        {
        // Self sends have been accounted
        mRecvNonActiveCount++;
        }
      }
    }

// if( mMyRank == 0 )
// {
//   printf("S: %d AC: %d\n", SimTick, mActiveSendRanksCount);
//   fflush(stdout);
// }

  mPktInject = 2;
  mCurrentInject = 0;

#if defined(RUN_FOR_REAL)
#ifdef PK_PHASE5_SPLIT_COMMS
  mMyCore             = rts_get_processor_id();
  mNumberOfFifosInUse = 3;
#else
  mMyCore             = 0;
  mNumberOfFifosInUse = 6;
#endif
#endif
  }
template< int NON_ALIGNED1 >
void PacketAlltoAllv_PkActors::ExecuteSingleCoreSimple1( void* aSendBuff, void* aRecvBuff )
  {
  	BegLogLine(PKFXLOG_ALLTOALLV) 
  	  << "PacketAlltoAllv_PkActors::ExecuteSingleCoreSimple1 mRecvNonActiveCount=" << mRecvNonActiveCount
  	  << " mActiveSendRanksCount=" << mActiveSendRanksCount
  	  << " mGroupSize=" << mGroupSize
  	  << " aSendBuff=0x" << aSendBuff
  	  << " aRecvBuff=0x" << aRecvBuff
  	  << EndLogLine ;
  	  	BegLogLine( PKFXLOG_ALLTOALLV_INIT  )
//	    << SimTick
	    << " mActiveSendRanksCount: " << mActiveSendRanksCount
//	    << " aRecvNodeListCount: " << aRecvNodeListCount
	    << " "
	    << EndLogLine;
  {
	  for ( int a=0;a<mActiveSendRanksCount;a+=1)
	  {
	  	BegLogLine(PKFXLOG_ALLTOALLV_INIT)
	  	  << "mActiveSendRanks[" << a
	  	  << "]=" << mActiveSendRanks[a]
	  	  << " mSendCount=" << mSendCount[a]
	  	  << " mSendOffsetIn=0x" << (void *)(mSendOffsetIn[a])
	  	  << EndLogLine ;
	  }
  }
//  {
//	  for ( int a=0;a<aRecvNodeListCount;a+=1)
//	  {
//	  	BegLogLine(PKFXLOG_ALLTOALLV_INIT)
//	  	  << "aRecvNodeList[" << a
//	  	  << "]=" << aRecvNodeList[a]
//	  	  << " aRecvCount=" << aRecvCount[a]
//	  	  << " aRecvOffset=0x" << (void *)(aRecvOffset[a])
//	  	  << EndLogLine ;
//	  }
//  }
  	  
  AlltoallvSimpleExecBarrierStart.HitOE( PKTRACE_ALLTOALLV_INIT,
      "AlltoallvSimpleExecBarrierStart",
      PkNodeGetId(),
      AlltoallvSimpleExecBarrierStart );

  BGLPartitionBarrierCore();

  AlltoallvSimpleExecBarrierFinis.HitOE( PKTRACE_ALLTOALLV_INIT,
      "AlltoallvSimpleExecBarrierFinis",
      PkNodeGetId(),
      AlltoallvSimpleExecBarrierFinis );

  mSendBuff = aSendBuff;
  mRecvBuff = aRecvBuff;

  // memcpy data local to this node.
  if( mSendCount[ mMyRank ] )
    {
    char * Src = (char*)mSendBuff + (mSendOffsetIn[ mMyRank ] * mSendTypeSize);
    char * Tgt = (char*)mRecvBuff + (mRecvOffset[ mMyRank ] * mRecvTypeSize);
    int Size   = mSendCount[ mMyRank ] * mSendTypeSize;
//    memcpy_double( (double *) Tgt, (double *) Src, Size );
    }
  mRecvCount[ mMyRank ] = mSendCount[ mMyRank ];

//   if( alltoallv_debug && ( mMyRank == 14 ) )
//     {
//     printf( "SCOUNT: %d\n", mSendCount[ 0 ] );
//     }

  _BGL_TorusFifoStatus FifoStatus0;
  _BGL_TorusFifoStatus FifoStatus1;
  _BGL_TorusFifoStatus FifoStatus;

  mTotalSentPackets = 0;
  mTotalRecvPackets = 0;
  mRecvCountDone = mRecvNonActiveCount;

//   for( int i=0; i<mGroupSize; i++ )
//     mBytesRecvFromNode[ i ] = 0;

  {
  for( int i=0; i<mActiveSendRanksCount; i++ )
    {
    int Rank = mActiveSendRanks[ i ];

    assert( Rank >=0 && Rank < mGroupSize );
    mSendOffsets[ Rank ] = 0;
    }
  }
  // Send all the packets (this incorporates draining incoming ones)
  int FinalReceiveByteCount = mReceiveBytes ;
  ReceiveByteCount=0 ; // so we will be able to work out when everything is 'in' 
  ReceiveCount=0 ;
  CurrentA2A = this ; 
  
  Expecting = 1 ; // We are now able to handle incoming packets
// 'Barrier' here so we don't push any packets in before a node is expecting.
  PkCo_Barrier() ;
  
  {
	  for ( int i=0;i<mActiveSendRanksCount;i++ )
	  {
         int DestRank = mActiveSendRanks[ i ];
         assert( DestRank >= 0 && DestRank < mGroupSize );
         int DestCoordX = mGroup->mElements[ DestRank ].mCoord.mX;
         int DestCoordY = mGroup->mElements[ DestRank ].mCoord.mY;
         int DestCoordZ = mGroup->mElements[ DestRank ].mCoord.mZ;

    	int TargetPE = PkTopo3DMakeRank(DestCoordX, DestCoordY, DestCoordZ) ;
//        MetaData->mSourceRank          = mMyRank;
//        MetaData->mTotalCount          = mSendCount[ DestRank ];
//        MetaData->mOffset              = mSendOffsets[ DestRank ];
//        MetaData->mBytesInPayload      = BytesToSend;

        char* BlockToSend = ((char *)mSendBuff) + (mSendOffsetIn[ DestRank ]*mSendTypeSize) + mSendOffsets[ DestRank ];
	  	  AllToAllSimplePushAsync::Push( (void *) BlockToSend  
	  	                         , mSendCount[ DestRank ] * mSendTypeSize
	  	                         , mMyRank
	  	                         , TargetPE
	  	                         , 0
	  	  	                 ) ;
//      PkKickPipesGently() ;
        PkFiberYield() ;
	  	
	  }
  }
  
  // Spin for stragglers to come in
  int LatestReceiveCount = ReceiveCount ;
  int NextReceiveCount = LatestReceiveCount ;
  int FinalReceiveCount = mGroupSize-mRecvNonActiveCount ;
  BegLogLine(PKFXLOG_ALLTOALLV_STRAGGLERS) 
	  << "Starting to spin, NextReceiveCount=" << NextReceiveCount
	  << " FinalReceiveCount=" << FinalReceiveCount
	  << EndLogLine ; 

  unsigned int spincount = 0 ; 
  while (NextReceiveCount != FinalReceiveCount)
  {
  	NextReceiveCount = ReceiveCount ;
  	if ( NextReceiveCount != LatestReceiveCount )
  	{
  		BegLogLine(PKFXLOG_ALLTOALLV_STRAGGLERS) 
  		  << "Spinning, NextReceiveCount=" << NextReceiveCount
  		  << " FinalReceiveCount=" << FinalReceiveCount
  		  << EndLogLine ; 
  		LatestReceiveCount = NextReceiveCount ; 
  	}
    assert(NextReceiveCount <= FinalReceiveCount) ;
  	PkFiberYield() ;
  	spincount += 1 ;
  	
  	
  	assert (spincount < 1000000000 ) ; // If this one fails, it is likely we have lost a packet
  }  
  
  Expecting = 0 ;
//// 'Barrier' here spins until all nodes have received everything
//  PkCo_Barrier() ;
// Spin here until the software send queue is cleared into the hardware
  PkClearSendQueue() ;
// 'Barrier' here spins until all nodes have received everything --- to give 'Throbber' a clear run
  PkCo_Barrier() ;
//  if( mActiveSendRanksCount > 0 )
//    {
//    memcpy( mActiveSendRanksTransient,
//            mActiveSendRanks, sizeof(int)*mActiveSendRanksCount);
//    mCurrentRankPtr = mActiveSendRanksTransient;
//    }
//
//  if( mActiveSendRanksCount == 0 )
//    mEndRankPtr = mCurrentRankPtr;
//  else
//    mEndRankPtr = &(mActiveSendRanksTransient[ mActiveSendRanksCount - 1 ]);
//
//  int LinkPicker = 0;
//  int  DestRank = GetNextRankToSend();
//
//  int PktCount = 0;
//
//  int LinkListsDone = 0;
//  int CurLinkIndex  = 0;
//  int StuckToSendCount = 0;
//
//  int FifoPicker;
//  if( mMyCore == 0 )
//    FifoPicker = 0;
//  else
//    FifoPicker = 1;
//
//  AlltoallvInitFinis.HitOE( PKTRACE_ALLTOALLV_INIT,
//                            "AlltoallvInitFinis",
//                            PkNodeGetId(),
//                            AlltoallvInitFinis );
//
//	  	  	                 
//  while( ( DestRank != SEND_DONE ) ||
//         ( mRecvCountDone != mGroupSize ) )
//    {
//    if( mMyCore == 0 )
//    {
//      _ts_GetStatus0_nosync( & FifoStatus );
//    }
//    else
//    {
//      _ts_GetStatus1_nosync( & FifoStatus );
//    }
//
//    StuckToSendCount = 0;
//
//    // While there are packets to send keep trying to inject packets.
//    while( DestRank != SEND_DONE )
//      {
//      // Check if there's room in the fifo to send
//
//      int TotalBytesToSend = mSendCount[ DestRank ] * mSendTypeSize;
//      int BytesSent        = mSendOffsets[ DestRank ];
//      int BytesToSend      = 0;
//
//      assert( BytesSent < TotalBytesToSend );
//      if( ( BytesSent + _BGL_TORUS_PKT_MAX_PAYLOAD ) <=  TotalBytesToSend )
//        {
//        BytesToSend = _BGL_TORUS_PKT_MAX_PAYLOAD;
//        }
//      else
//        {
//        // Last packet might not be full
//        BytesToSend = ( TotalBytesToSend - BytesSent );
//        }
//
//      int chunks = ((BytesToSend + 15) >> 5) ;
//
//      _BGL_TorusFifo * NextFifoToSend;
//      if( mMyCore == 0 ) NextFifoToSend = _ts_GetNextFifoCheckClearToSendOnLink0( & FifoStatus, chunks );
//      else               NextFifoToSend = _ts_GetNextFifoCheckClearToSendOnLink1( & FifoStatus, chunks );
//
//      if( NextFifoToSend != NULL )
//        {
//        // We can send!
//        assert( DestRank >= 0 && DestRank < mGroupSize );
//        int DestCoordX = mGroup->mElements[ DestRank ].mCoord.mX;
//        int DestCoordY = mGroup->mElements[ DestRank ].mCoord.mY;
//        int DestCoordZ = mGroup->mElements[ DestRank ].mCoord.mZ;
//
//        // Send a full packet
//        BGLTorusMakeHdrAppChooseRecvFifo( & ( mAlltoAllvPacket.hdr ),
//                                          DestCoordX,
//                                          DestCoordY,
//                                          DestCoordZ,
//                                          0,
//                                          0,
//                                          BytesToSend,
//                                          FifoPicker );
//
//        // if first packet of message, send the control key which has the number of elements the other size expects to receive.
//        // The packet header has 2 words. Below is my current strategy for using that memory
//        // 2 bytes source
//        // 3 bytes TotalCount
//        // 3 bytes Offset
//        BGL_AlltoallvPacketMetaData* MetaData = (BGL_AlltoallvPacketMetaData *) &mAlltoAllvPacket.hdr.swh0;
//
//        MetaData->mSourceRank          = mMyRank;
//        MetaData->mTotalCount          = mSendCount[ DestRank ];
//        MetaData->mOffset              = mSendOffsets[ DestRank ];
//        MetaData->mBytesInPayload      = BytesToSend;
//
//        char* BlockToSend = ((char *)mSendBuff) + (mSendOffsetIn[ DestRank ]*mSendTypeSize) + MetaData->mOffset;
//
//        memcpy_double( (double *) mAlltoAllvPacket.payload, (double *)BlockToSend, BytesToSend );
//         //memcpy( mAlltoAllvPacket.payload, BlockToSend, BytesToSend );
//
//          BegLogLine(PKFXLOG_ALLTOALLV) 
//            << "BGLTorusInjectPacketImageApp"
//            << " mMyRank=" << mMyRank
//            << " DestRank=" << DestRank
//            << " BytesToSend=" << BytesToSend
//            << " MetaData->mOffset=" << MetaData->mOffset
//            << " MetaData->mTotalCount=" << MetaData->mTotalCount
//            << " mRecvTypeSize=" << mRecvTypeSize
//            << " NextFifoToSend=" << NextFifoToSend
//            << " &mAlltoAllvPacket=" << &mAlltoAllvPacket
//            << EndLogLine ; 
//        BGLTorusInjectPacketImageApp( NextFifoToSend,
//                                      &mAlltoAllvPacket );
//          BegLogLine(PKFXLOG_ALLTOALLV) 
//            << "BGLTorusInjectPacketImageApp returned"
//            << EndLogLine ; 
//
////         printf( "Chunks %d sent to dest: %d\n", chunks, DestRank );
////         fflush( stdout);
//        // Need to do this once we actually send something
//        mSendOffsets[ DestRank ] += BytesToSend;
//        DestRank = GetNextRankToSend();
//        }
//      else
//        {
//        StuckToSendCount++;
//
//        if( StuckToSendCount == mNumberOfFifosInUse )
//          break;
//        }
//      }
//
//    // While we haven't received all needed packets keep listening
//#if !defined(PK_ALL2ALL_BY_MESSAGE)
//   if ( mRecvCountDone != mGroupSize )
//   {
//   	
//#ifndef PK_PHASE5_SPLIT_COMMS
//      if( mMyCore == 0 )
//        {
//        if( mNumberOfFifosInUse == 6 )
//          {
//          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
//          _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
//          }
//        else
//          {
//          _ts_GetStatus0_nosync( & FifoStatus0 );
//          _ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, this );
//          }
//        }
//      else
//        {
//        if( mNumberOfFifosInUse == 6 )
//          {
//          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
//          _ts_PollBoth_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
//          }
//        else
//          {
//          _ts_GetStatus1_nosync( & FifoStatus1 );
//          _ts_Poll1_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus1, this );
//          }
//        }
//#else
//      // If we have nothing to receive try to send
////       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
////       fflush( stdout );
//
//          _ts_GetStatus0_nosync( & FifoStatus0 );
//          _ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus0, this );
////      if( !_ts_Poll0_OnePassReceive_Alltoallv_Drain< NON_ALIGNED1 >( & FifoStatus, this ) )
////        break;
//
////       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
////       fflush( stdout );
//
//#endif
//
//      assert( mRecvCountDone <= mGroupSize );
//   }
//#else    
//    int PollCount = PACKET_POLL_COUNT ;
//    while( PollCount && (mRecvCountDone != mGroupSize) )
//      {
//      // Read the status.
//#ifndef PK_PHASE5_SPLIT_COMMS
//      if( mMyCore == 0 )
//        {
//        if( mNumberOfFifosInUse == 6 )
//          {
//          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
//          _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
//          }
//        else
//          {
//          _ts_GetStatus0_nosync( & FifoStatus0 );
//          _ts_Poll0_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, this );
//          }
//        }
//      else
//        {
//        if( mNumberOfFifosInUse == 6 )
//          {
//          _ts_GetStatusBoth_nosync( & FifoStatus0, & FifoStatus1 );
//          _ts_PollBoth_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus0, & FifoStatus1, this );
//          }
//        else
//          {
//          _ts_GetStatus1_nosync( & FifoStatus1 );
//          _ts_Poll1_OnePassReceive_Alltoallv_Put< NON_ALIGNED1 >( & FifoStatus1, this );
//          }
//        }
//#else
//      // If we have nothing to receive try to send
////       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
////       fflush( stdout );
//
//      if( !_ts_Poll0_OnePassReceive_Alltoallv_Chunk_Put< NON_ALIGNED1 >( & FifoStatus, this ) )
//        break;
//
////       printf( "rd: %d gs: %d l: %d\n", mRecvCountDone, mGroupSize, __LINE__ );
////       fflush( stdout );
//
//#endif
//
//      assert( mRecvCountDone <= mGroupSize );
//      PollCount--;
//      }
//#endif      
//    }
   BegLogLine(PKFXLOG_ALLTOALLV) 
  	  << "PacketAlltoAllv_PkActors::ExecuteSingleCoreSimple1 complete" 
  	  << EndLogLine ;
  }
  

void PacketAlltoAllv_PkActors::ExecuteSingleCoreSimple( void* aSendBuf, int aSendCount[], int* aSendOffset, int aSendTypeSize,
                                               void* aRecvBuf, int aRecvCount[], int* aRecvOffset, int aRecvTypeSize,
                                               GroupContainer_Actors* aGroup, int aligned, int debug,
                                               int* aSendNodeList, int aSendNodeListCount,
                                               int* aRecvNodeList, int aRecvNodeListCount )
  {
  alltoallv_debug = debug;

//   printf( "alltoallv_debug= %d\n", alltoallv_debug );
//   fflush( stdout );

  BegLogLine(PKFXLOG_ALLTOALLV) 
    << "ExecuteSingleCoreSimple start"
    << EndLogLine ; 
  InitSingleCoreSimple( aSendCount, aSendOffset, aSendTypeSize,
                        aRecvCount, aRecvOffset, aRecvTypeSize, aGroup,
      aSendNodeList, aSendNodeListCount, aRecvNodeList, aRecvNodeListCount );

  if( aligned && ( 0 == (mSendTypeSize & 8) ) )
    // We are carrying around things which are at least doubleword-aligne doubles
    ExecuteSingleCoreSimple1<0>( aSendBuf, aRecvBuf );
  else
    ExecuteSingleCoreSimple1<1>( aSendBuf, aRecvBuf );

#if 0
  FinalizeSingleCoreSimple();
#endif

  alltoallv_debug = 0;
  BegLogLine(PKFXLOG_ALLTOALLV) 
    << "ExecuteSingleCoreSimple end"
    << EndLogLine ; 
  }


void PacketAlltoAllv_PkActors::ReportFifoHistogram(void)
{
	fStatus.display() ;
}
  
